#include "imageedit.h"
#include "ui_imageedit.h"
imgDropLabel::imgDropLabel(QWidget* parent, imgSourcesWidget* sourceList_):
	QLabel(parent)
{
	sourceList = sourceList_;
	setAcceptDrops(true);
}
void
imgDropLabel::setSourceList(imgSourcesWidget* sourceList_)
{
	sourceList = sourceList_;
}
void
imgDropLabel::dragEnterEvent(QDragEnterEvent * event)
{
	const QMimeData* data = event->mimeData();
	if (data->hasFormat("application/x-qabstractitemmodeldatalist"))
	{
		QObject* src = event->source();
		if (src != NULL && sourceList != NULL)
		{
			if (src == (QObject*)sourceList->sourceList)
			{
				event->setDropAction(Qt::LinkAction);
				event->accept();
			}
			else
			{
				event->ignore();
			}
		}
	}
}
void 
imgDropLabel::dragMoveEvent(QDragMoveEvent *event)
{
	const QMimeData* data = event->mimeData();
	if (data->hasFormat("application/x-qabstractitemmodeldatalist"))
	{
		QObject* src = event->source();
		if (src != NULL && sourceList != NULL)
		{
			if (src == (QObject*)sourceList->sourceList)
			{
				event->setDropAction(Qt::LinkAction);
				event->accept();
			}
			else
			{
				event->ignore();
			}
		}
	}
}
void 
imgDropLabel::dropEvent(QDropEvent *event)
{
	const QMimeData* data = event->mimeData();
	if (data->hasFormat("application/x-qabstractitemmodeldatalist"))
	{
		QObject* src = event->source();
		if (src != NULL && sourceList != NULL)
		{
			if (src == (QObject*)sourceList->sourceList)
			{
				QTreeWidgetItem* item = sourceList->sourceList->currentItem();
				event->setDropAction(Qt::LinkAction);
				event->accept();

				containerPtr img = sourceList->getContainer(dynamic_cast<container*>(item));
				if (img == NULL) return;
				setText(img->name);
				emit imgReceived(img);
			}
			else
			{
				event->ignore();
			}
		}
	}
}

#ifdef OPENGL
GLimageEdit::GLimageEdit(QWidget *parent)
{
	mSceneChanged = false;
	mBgColor = QColor::fromRgb(150, 150, 150);

	mOutH = 0;
	mOutW = 0;
	mImgRatio = 4.0f / 3.0f;

	mPosX = 0;
	mPosY = 0;
	transZ = 0;
	installEventFilter(this);
}
/// Used to set the image to be viewed
bool    
GLimageEdit::showImage(cv::Mat image)
{
	image.copyTo(mOrigImage);

	mImgRatio = (float)image.cols / (float)image.rows;

	glEnable(GL_TEXTURE_RECTANGLE);
	GLuint texID;
	glGenTextures(1, &texID);
	glBindTexture(GL_TEXTURE_RECTANGLE, texID);
	
	if (image.type() == CV_8UC3)
		glTexImage2D(GL_TEXTURE_RECTANGLE, 0, GL_BGR, image.cols, image.rows,
		0, GL_BGR, GL_UNSIGNED_INT8_VEC3_NV, image.data);
	else
		return false;
	
	glBegin(GL_QUADS);
	glTexCoord2f(0, 0);
	glVertex2f(0, 0);
	glTexCoord2f(image.cols, 0);
	glVertex2f(width(), 0);
	glTexCoord2f(image.cols, image.rows);
	glVertex2f(width(), height());
	glTexCoord2f(0, image.rows);
	glVertex2f(0, height());
	glEnd();
	
	glDisable(GL_TEXTURE_RECTANGLE);

	if (mOrigImage.channels() == 3)
		mRenderQtImg = QImage((const unsigned char*)(mOrigImage.data),
		mOrigImage.cols, mOrigImage.rows,
		mOrigImage.step, QImage::Format_RGB888);
	else if (mOrigImage.channels() == 1)
		mRenderQtImg = QImage((const unsigned char*)(mOrigImage.data),
		mOrigImage.cols, mOrigImage.rows,
		mOrigImage.step, QImage::Format_Indexed8);
	else
		return false;

	mRenderQtImg = QGLWidget::convertToGLFormat(mRenderQtImg);

	mSceneChanged = true;

	updateScene();

	return true;
}
bool	
GLimageEdit::eventFilter(QObject* o, QEvent* e)
{
	if (o == this)
	{
		if (e->type() == QEvent::MouseButtonPress)
		{
			QMouseEvent* me = dynamic_cast<QMouseEvent*>(e);
			QPoint pos = me->pos();
			float x = pos.x() - mPosX;
			float y = pos.y() - mPosY;
			x = x * mOrigImage.cols / mOutW;
			y = y * mOrigImage.rows / mOutH;
			//glTranslatef(0, 0, 1);
		}
		if (e->type() == QEvent::MouseButtonRelease)
		{

		}
		if (e->type() == QEvent::MouseMove)
		{

		}
		if (e->type() == QEvent::Wheel)
		{

		}
	}

	return false;
}

// ************************* PROTECTED *********************************************
/// OpenGL initialization
void 	
GLimageEdit::initializeGL()
{
	makeCurrent();
	qglClearColor(mBgColor.darker());
}
/// OpenGL Rendering
void 	
GLimageEdit::paintGL()
{
	makeCurrent();

	if (!mSceneChanged)
		return;

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	renderImage();

	mSceneChanged = false;
}
/// Widget Resize Event
void 	
GLimageEdit::resizeGL(int width, int height)
{
	makeCurrent();
	glViewport(0, 0, (GLint)width, (GLint)height);

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();

	glOrtho(0, width, 0, height, 0, 1);	// To Draw image in the center of the area

	glMatrixMode(GL_MODELVIEW);

	// ---> Scaled Image Sizes
	mOutH = width / mImgRatio;
	mOutW = width;

	if (mOutH>height)
	{
		mOutW = height*mImgRatio;
		mOutH = height;
	}

	emit imageSizeChanged(mOutW, mOutH);
	// <--- Scaled Image Sizes

	mPosX = (width - mOutW) / 2;
	mPosY = (height - mOutH) / 2;

	mSceneChanged = true;

	updateScene();
}
void        
GLimageEdit::updateScene()
{
	if (mSceneChanged && this->isVisible())
		updateGL();
}
void        
GLimageEdit::renderImage()
{
	boost::posix_time::ptime start = boost::posix_time::microsec_clock::universal_time();
	makeCurrent();

	glClear(GL_COLOR_BUFFER_BIT);

	if (!mRenderQtImg.isNull())
	{
		glLoadIdentity();

		QImage image; // the image rendered
		glPushMatrix();
		{
			int imW = mRenderQtImg.width();
			int imH = mRenderQtImg.height();

			// The image is to be resized to fit the widget?
			if (imW != this->size().width() &&
				imH != this->size().height())
			{

				image = mRenderQtImg.scaled( //this->size(),
											QSize(mOutW, mOutH),
											Qt::IgnoreAspectRatio,
											Qt::SmoothTransformation
											);

				//qDebug( QString( "Image size: (%1x%2)").arg(imW).arg(imH).toAscii() );
			}
			else
				image = mRenderQtImg;

			// ---> Centering image in draw area            
			//glTranslatef(0, 0, transZ);
			glRasterPos2i( mPosX, mPosY );
			// <--- Centering image in draw area

			imW = image.width();
			imH = image.height();
			//glDraw();
			glDrawPixels( imW, imH, GL_RGBA, GL_UNSIGNED_BYTE, image.bits());
		}
		glPopMatrix();

		// end
		glFlush();
		boost::posix_time::ptime end = boost::posix_time::microsec_clock::universal_time();
		boost::posix_time::time_duration delta = end - start;
		cv::Mat tmp = mOrigImage;
		emit log("Rendering image took " + QString::number(delta.total_milliseconds()) + " milliseconds", 0);
	}
}
#endif // OPENGL
imageEdit::imageEdit(QWidget *parent, imgSourcesWidget* sourceList_) :
	QWidget(parent),
	labeling(false),
	className(NULL),
	classAdd(NULL),
	clearLabel(NULL),
	acceptLabel(NULL),
	aspectRatioLabel(NULL),
	labelHeight(NULL),
	labelWidth(NULL),
	ui(new Ui::imageEdit),
	_erase(NULL),
	_draw(NULL),
	_editing(false),
	_toolSize(0),
	sourceList(sourceList_)
{
    blockUpdate = false;
    zoomFactor = 1.0;
	minVal = maxVal = 0;
    dragging = false;
    layout = new QGridLayout(this);
#ifdef OPENGL
	imgDisp = new GLimageEdit(this);
	connect(imgDisp, SIGNAL(log(QString, int)), this, SLOT(handleLog(QString, int)));
#else
    imgDisp = new QLabel(this);
    imgDisp->setMinimumHeight(240);
    imgDisp->setMinimumWidth(320);
    imgDisp->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
	imgDisp->installEventFilter(this);
	
#endif
    layout->addWidget(imgDisp,0,0,1,4);
	lineEdit = new QLineEdit(this);
	layout->addWidget(lineEdit, 1, 0, 1, 4);
    setLayout(layout);
	classSelect = new QComboBox(this);
	classSelect->setInsertPolicy(QComboBox::InsertAtBottom);
	classSelect->hide();
	classSelect->setEditable(true);
	QLineEdit* ptr = classSelect->lineEdit();
	connect(ptr, SIGNAL(enterPressed()), this, SLOT(handleAddClass()));
	useColormap = false;
	numThreads = 8;
	_editingEnable = new QAction(this);
	imgDisp->addAction(_editingEnable);
	_editingEnable->setText("Enable editing");
	_editingEnable->setCheckable(true);
	connect(_editingEnable, SIGNAL(checked(bool)), this, SLOT(handleImageEditing(bool)));
	setAcceptDrops(true);
	imgDisp->setContextMenuPolicy(Qt::CustomContextMenu);
	connect(imgDisp, SIGNAL(customContextMenuRequested(const QPoint&)),
		this, SLOT(onRightClick(const QPoint&)));
}
imageEdit::~imageEdit()
{
    delete ui;
}
bool 
imageEdit::eventFilter(QObject *obj, QEvent *ev)
{
    if(orgImg.empty()) return false;
    if(obj == (QObject*)imgDisp)
    {
		if (ev->type() == QEvent::KeyPress)
		{
			QKeyEvent* keyEv = dynamic_cast<QKeyEvent*>(ev);
			emit keyPress(keyEv->key());
		}
        if(ev->type() == QEvent::Resize)
        {
#ifndef OPENGL
            imgDisp->setPixmap(pixmap.scaled(imgDisp->width(),imgDisp->height() /*,Qt::KeepAspectRatio*/));
#endif
            return true;
        }
        if(ev->type() == QEvent::MouseButtonPress)
        {
            QMouseEvent* event = dynamic_cast<QMouseEvent*>(ev);
            QPoint pos = event->pos();
            start = pos;
            lastPos = pos;
			float scaleX = (float)currentROI.width / (float)imgDisp->width();
			float scaleY = (float)currentROI.height / (float)imgDisp->height();
			float posX = scaleX*pos.x() + currentROI.x;
			float posY = scaleY*pos.y() + currentROI.y;
			cv::Point orgImgPoint(posX, posY);
			if (QApplication::keyboardModifiers() == Qt::ControlModifier && _editing)
			{
				cv::Rect drawRect(posX - _toolSize, posY - _toolSize, _toolSize * 2 + 1, _toolSize * 2 + 1);
				try
				{
					dispImg(drawRect) = _drawTool;
				}
				catch (cv::Exception &e)
				{
					return true;
				}
				if (_drawEnabled)
				{
					emit drawPt(drawRect, _drawTool);
				}
				if (_eraseEnabled)
				{
					emit erasePt(drawRect);
				}
				handleROIUpdate(currentROI);
			}

			if (posY >= orgImg.rows || posX >= orgImg.cols) return false;
			QString value;
			if (orgImg.type() == CV_8U)
			{
				value = QString::number(orgImg.at<uchar>(posY, posX));
			}
			if (orgImg.type() == CV_8UC3)
			{
				cv::Vec3b pixel = orgImg.at<cv::Vec3b>(posY, posX);
				value = "[" + QString::number(pixel.val[0]) + "," + QString::number(pixel.val[1]) + "," + QString::number(pixel.val[2]) + "]";
			}
			if (orgImg.type() == CV_16U)
			{
				value = QString::number(orgImg.at<unsigned short>(posY, posX));
			}
			if (orgImg.type() == CV_32F)
			{
				value = QString::number(orgImg.at<float>(posY, posX));
			}
			if (orgImg.type() == CV_32FC3)
			{
				cv::Vec3f pixel = orgImg.at<cv::Vec3f>(posY, posX);
				value = "[" + QString::number(pixel.val[0]) + "," + QString::number(pixel.val[1]) + "," + QString::number(pixel.val[2]) + "]";
			}

			lineEdit->setText("(" + QString::number(posX) + "," + QString::number(posY) + ") " + value);

            emit dragStart(pos);
            if((QApplication::keyboardModifiers() == Qt::AltModifier || 
				QApplication::keyboardModifiers() == Qt::ControlModifier) && !dragging)
            {

                firstClickInOriginal = cv::Point(posX,posY);
				if (labeling)
					polygonInOriginal.push_back(firstClickInOriginal);
            }
			dragging = true;
            return true;
        }
        if(ev->type() == QEvent::MouseButtonRelease)
        {
            QMouseEvent* event = dynamic_cast<QMouseEvent*>(ev);
            end = event->pos();
			dragging = false;
            emit dragEnd(event->pos());

			float scaleX = (float)currentROI.width / (float)imgDisp->width();
			float scaleY = (float)currentROI.height / (float)imgDisp->height();
			float posXS = scaleX*lastPos.x() + currentROI.x;
			float posYS = scaleY*lastPos.y() + currentROI.y;

			float posXE = scaleX*end.x() + currentROI.x;
			float posYE = scaleY*end.y() + currentROI.y;
			// Point in original image without scaling or shifting
			orgImgPoint = cv::Point2f(posXE, posYE);
			if (event->button() == Qt::RightButton)
			{
				// Create a popup at this location

			}
			if (QApplication::keyboardModifiers() == Qt::ControlModifier && !_editing)
			{
				float x = MIN(orgImgPoint.x, firstClickInOriginal.x);
				float y = MIN(orgImgPoint.y, firstClickInOriginal.y);
				float width = abs(orgImgPoint.x - firstClickInOriginal.x);
				float height = abs(orgImgPoint.y - firstClickInOriginal.y);
				cv::Rect rect(x, y, width, height);
				emit rectSelect(rect);
			}
			if (_editing)
			{
				if (QApplication::keyboardModifiers() == Qt::ControlModifier && _drawLine)
				{
					cv::Mat draw;
					cv::Point start(scaleX*lastPos.x(), scaleY*lastPos.y());
					cv::Point cvend(scaleX*end.x(), scaleY*end.y());
					roiImg.copyTo(draw);
					cv::line(draw,start,cvend , cv::Scalar(0, 0, 255), 1);
					drawImg(draw);
					emit lineDrawn(start, cvend);
					return true;
				}
			}
            return true;
        }
        if(ev->type() == QEvent::MouseMove)
        {
            QMouseEvent* event = dynamic_cast<QMouseEvent*>(ev);
            QPoint pos = event->pos();
			float scaleX = (float)currentROI.width  / (float)imgDisp->width();
			float scaleY = (float)currentROI.height / (float)imgDisp->height();
			// Position in ROI image... Need to adjust based on current ROI position

			float posXS = scaleX*lastPos.x() + currentROI.x;
			float posYS = scaleY*lastPos.y() + currentROI.y;

			float posXE = scaleX*pos.x() + currentROI.x;
			float posYE = scaleY*pos.y() + currentROI.y;
			// Point in original image without scaling or shifting
			cv::Point orgImgPoint(posXE, posYE);
			if (QApplication::keyboardModifiers() == Qt::ControlModifier && _editing)
			{
				cv::Rect drawRect(posXE - _toolSize, posYE - _toolSize, _toolSize * 2 + 1, _toolSize * 2 + 1);
				try
				{
					_drawTool.copyTo(dispImg(drawRect));
				}catch (cv::Exception &e)
				{
					return true;
				}
				if (_drawEnabled)
				{
					emit drawPt(drawRect, _drawTool);
				}
				if (_eraseEnabled)
				{
					emit erasePt(drawRect);
				}
				handleROIUpdate(currentROI);
			}

            if(dragging == true)
                emit dragPos(pos);
			
			// Move the viewport
			if (QApplication::keyboardModifiers() != Qt::AltModifier  && 
				QApplication::keyboardModifiers() != Qt::ControlModifier && 
				dragging)
            {
                if(blockUpdate == false)
                {
                    blockUpdate = true;
					cv::Rect ROI = currentROI;
                    ROI.x += (posXS - posXE);
                    ROI.y += (posYS - posYE);
                    updateROI(ROI);
                    lastPos = pos;
                    return true;
                }else
                {
                    blockUpdate = false;
                    return true;
                }
            }
			if (_editing && dragging && _drawLine)
			{
				cv::Mat tmp;
				roiImg.copyTo(tmp);
				cv::line(tmp, cv::Point(lastPos.x(), lastPos.y()), cv::Point(pos.x(), pos.y()), cv::Scalar(0, 0, 255));
				drawImg(tmp);
			}
			// Handle free form region of interest mask creation
			if (labeling)
			{	
				if (QApplication::keyboardModifiers() == Qt::AltModifier)
				{
					polygonInOriginal.push_back(orgImgPoint);
				}
				if (QApplication::keyboardModifiers() == Qt::ControlModifier)
				{
					float x = MIN(orgImgPoint.x, firstClickInOriginal.x);
					float y = MIN(orgImgPoint.y, firstClickInOriginal.y);
					float width = abs(orgImgPoint.x - firstClickInOriginal.x);
					float height = abs(orgImgPoint.y - firstClickInOriginal.y);
					// Constant aspect ratio square
					if (labelWidth->value() != 0 || labelHeight->value() != 0)
					{
						float AR = width / height;
						float AR_ = (float)labelWidth->value() / (float)labelHeight->value();
						if (AR > AR_) // width is greater, thus scale height accordingly
						{
							height = width / AR_;
						}
						if (AR < AR_) // height is greater, thus scale width accordingly
						{
							width = AR_ * height;
						}
					}
					polygonInOriginal.resize(4);
					polygonInOriginal[0] = cv::Point(x, y);
					polygonInOriginal[1] = cv::Point(x, y + height);
					polygonInOriginal[2] = cv::Point(x + width, y + height);
					polygonInOriginal[3] = cv::Point(x + width, y);
				}
				displaySelection();
				return true;
			}
        }
		// Handle zooming
        if(ev->type() == QEvent::Wheel)
        {
            QWheelEvent* event = dynamic_cast<QWheelEvent*>(ev);
            QPoint pos = event->pos();
            float scaleX = (float)currentROI.width  / (float)imgDisp->width();
            float scaleY = (float)currentROI.height / (float)imgDisp->height();
            // Position in ROI image... Need to adjust based on current ROI position
            float posX = scaleX*pos.x() + currentROI.x;
            float posY = scaleY*pos.y() + currentROI.y;

            zoomFactor += (float)event->delta() / 300;
            if(zoomFactor < 1) zoomFactor = 1;
            cv::Rect ROI(posX - orgImg.cols / zoomFactor / 2,
                         posY - orgImg.rows / zoomFactor / 2,
                         orgImg.cols / zoomFactor,
                         orgImg.rows / zoomFactor);
            updateROI(ROI);
            return true;
        }
        return false;
    }
    return false;
}
void 
imageEdit::addClasses(QStringList classes)
{
	classSelect->addItems(classes);
}
void 
imageEdit::addClass(std::pair<QString, int> class_)
{
	classSelect->addItem(class_.first);
	classes.push_back(class_);
}
void 
imageEdit::changeImg(container *cont)
{
	switch (cont->type)
	{
	case container::Img:
	{
		imgContainer* tmp = dynamic_cast<imgContainer*>(cont);
		if (!tmp->maxMinValid)
		{
			cv::minMaxIdx(tmp->M(), &tmp->min, &tmp->max);
			tmp->maxMinValid = true;
		}
		if (minVal != tmp->min || maxVal != tmp->max)
		{
			updateLUT = true;
			minVal = tmp->min;
			maxVal = tmp->max;
		}
		curCont = tmp;
		changeImg(tmp->M(), cont->type != container::Stream);
		emit sendPairedImage(tmp->M());
		break;
	}
	case container::Label:
	{
		labelContainer* tmp = dynamic_cast<labelContainer*>(cont);
		curCont = tmp;
		changeImg(tmp->M(), cont->type != container::Stream);
		emit sendPairedImage(tmp->M());
		break;
	}
	}
}
void
imageEdit::changeImg(containerPtr cont)
{
	changeImg(cont.get());
	emit imageChanged(boost::dynamic_pointer_cast<imgContainer, container>(cont));
}
void 
imageEdit::changeImg(cv::Mat img)
{
	changeImg(img, true);
}
void
imageEdit::changeImg(cv::Mat img, bool update)
{
	if (img.empty()) return;
	emit sendPairedImage(img);
	if (update)
	{
		adjustROI(img);
	}
	if (img.channels() == 3)
	{
		try
		{
            cv::cvtColor(img, orgImg, BGR2RGB);
		}
		catch (cv::Exception &e)
		{
			emit log("Error: " + QString::fromLocal8Bit(e.what()), 2);
		}
	}
	else orgImg = img;
	if (useColormap && orgImg.channels() == 1)
	{
		dispImg = colorMapImg(img);
	}
	else
	{
		dispImg = orgImg;
	}
	try
	{
		roiImg = dispImg(currentROI);
	}
	catch (cv::Exception &e)
	{

	}
	drawImg(roiImg);
}
void 
imageEdit::onRightClick(const QPoint& pt)
{
	float scaleX = (float)currentROI.width / (float)imgDisp->width();
	float scaleY = (float)currentROI.height / (float)imgDisp->height();
	// Position in ROI image... Need to adjust based on current ROI position
	// Start positions in original image
	float posXS = scaleX*start.x() + currentROI.x;
	float posYS = scaleY*start.y() + currentROI.y;
	// End positions in original image
	float posXE = scaleX*pt.x() + currentROI.x;
	float posYE = scaleY*pt.y() + currentROI.y;
	QPoint globalPos = imgDisp->mapToGlobal(pt);
	QMenu myMenu;
	myMenu.addAction("Set Reference");
	myMenu.addAction("Measure Artifact");
	QAction* selectedItem = myMenu.exec(globalPos);
	if (selectedItem)
	{
		QString text = selectedItem->text();
		if (text == "Set Reference")
		{
			referencePtr ref = boost::dynamic_pointer_cast<referenceContainer, container>(curCont->getChild("Reference"));
			if (ref == NULL)
			{
				ref.reset(new referenceContainer((QTreeWidgetItem*)curCont));
			}
			ref->setOrigin(cv::Point2f(posXE, posYE));
			curCont->getParent()->addChild(ref.get());
			curCont->getParent()->childContainers.push_back(ref);
			// Display the new origin
			changeImg(ref->M());
		}
		if (text == "Measure Artifact")
		{
			referencePtr ref = boost::dynamic_pointer_cast<referenceContainer, container>(curCont->getChild("Reference"));
			if (ref == NULL)
			{
				ref.reset(new referenceContainer((QTreeWidgetItem*)curCont));
			}
			float UVdist = sqrt((posXE - posXS)*(posXE - posXS) + (posYE - posYS)*(posYE - posYS));
			// Create a blocking popup dialog for entering the real world mm distance
			QDialog tmpDialog;
			QGridLayout tmpLayout(&tmpDialog);
			tmpDialog.setLayout(&tmpLayout);

			QLabel tmpLabel(&tmpDialog);
			tmpLabel.setText("Enter real world distance");
			tmpLayout.addWidget(&tmpLabel);

			QLineEdit tmpEdit(&tmpDialog);
			tmpLayout.addWidget(&tmpEdit);
			QPushButton ok(&tmpDialog);
			tmpLayout.addWidget(&ok);
			ok.setText("Ok");
			connect(&ok, SIGNAL(clicked()), &tmpDialog, SLOT(accept()));

			tmpDialog.exec();
			float realDist = tmpEdit.text().toFloat();
			ref->setScale(UVdist, realDist);
			emit log("Scale set to " + QString::number(realDist) + " units per " + QString::number(UVdist) + " pixels", 4);
			curCont->getParent()->addChild(ref.get());
			curCont->getParent()->childContainers.push_back(ref);
			// Display the new origin
			changeImg(ref->M());
		}
	}
}
void 
imageEdit::dragEnterEvent(QDragEnterEvent *event)
{
	const QMimeData* data = event->mimeData();
	if (data->hasFormat("application/x-qabstractitemmodeldatalist"))
	{
		QObject* src = event->source();
		if (src != NULL && sourceList != NULL)
		{
			if (src == (QObject*)sourceList->sourceList)
			{
				event->setDropAction(Qt::LinkAction);
				event->accept();
			}else
			{
				event->ignore();
			}

		}
	}
}
void 
imageEdit::dragMoveEvent(QDragMoveEvent *event)
{
	const QMimeData* data = event->mimeData();
	if (data->hasFormat("application/x-qabstractitemmodeldatalist"))
	{
		QObject* src = event->source();
		if (src != NULL && sourceList != NULL)
		{
			if (src == (QObject*)sourceList->sourceList)
			{
				event->setDropAction(Qt::LinkAction);
				event->accept();
			}else
			{
				event->ignore();
			}
		}
	}
}
void 
imageEdit::dropEvent(QDropEvent *event)
{
	const QMimeData* data = event->mimeData();
	if (data->hasFormat("application/x-qabstractitemmodeldatalist"))
	{
		QObject* src = event->source();
		if (src != NULL && sourceList != NULL)
		{
			if (src == (QObject*)sourceList->sourceList)
			{
				event->setDropAction(Qt::LinkAction);
				event->accept();
				QByteArray d = data->data("application/x-qabstractitemmodeldatalist");
				QString name = QString::fromWCharArray((wchar_t*)d.data(), d.size() / 2);
				QString name2 = name.mid(13, name.size() - 18);
				containerPtr img = sourceList->getContainer(name2);
				if (img == NULL) return;
				changeImg(img);
			}
			else
			{
				event->ignore();
			}
		}
	}
}
void 
imageEdit::handleImageEditing(bool enable)
{
	if (enable)
	{
		if (_draw == NULL) _draw = new QAction(this);
		if (_erase == NULL) _erase = new QAction(this);
		addAction(_draw);
		addAction(_erase);
		_draw->setCheckable(true);
		_erase->setCheckable(true);
		connect(_draw, SIGNAL(checked(bool)), this, SLOT(handleDrawToggled(bool)));
		connect(_erase, SIGNAL(checked(bool)), this, SLOT(handleEraseToggled(bool)));
	}else
	{
		disconnect(_draw, SIGNAL(checked(bool)), this, SLOT(handleDrawToggled(bool)));
		disconnect(_erase, SIGNAL(checked(bool)), this, SLOT(handleEraseToggled(bool)));
	}
}
void 
imageEdit::toggleColormap(bool value)
{
	useColormap = value;
	if (value)
	{
		if (dispImg.channels() == 1)
		{
			dispImg = colorMapImg(dispImg);
		}
	}
	handleROIUpdate(currentROI);
}
void 
imageEdit::receivePairedImage(cv::Mat img)
{
	if (orgImg.size != img.size) return;
	if (orgImg.channels() == 3 && img.channels() == 1)
	{
		// Make a new copy of the original for this overlay
		dispImg = orgImg.clone();
		cv::Mat tmp = orgImg;
		cv::Mat tmp2 = dispImg;
		if (img.type() == CV_8U)
		{
			cv::Vec3b mergeColor(0,0,0);
			for (int i = 0; i < img.rows * img.cols; ++i)
			{
				uchar val = img.at<uchar>(i);
				mergeColor.val[0] = val;
				if (val == 255)
					dispImg.at<cv::Vec3b>(i) = mergeColor;
				else
					dispImg.at<cv::Vec3b>(i) += mergeColor;
			}
		}
		if (img.type() == CV_32F)
		{
			cv::Vec3b mergeColor(0, 0, 0);
			for (int i = 0; i < img.rows * img.cols; ++i)
			{
				float val = img.at<float>(i);
				mergeColor.val[2] = val;
				dispImg.at<cv::Vec3b>(i) += mergeColor;
			}
		}
	}
	if (orgImg.channels() == img.channels() && orgImg.type() == img.type())
	{
		dispImg = orgImg + img;
	}
	handleROIUpdate(currentROI);
}
void
imageEdit::handleSaveDrawing()
{
	curCont->_M = dispImg;
	curCont->baseName.append("_edited");
	curCont->save();
}
void
imageEdit::mirrorDraw(cv::Rect rect, cv::Mat tool)
{
	if (tool.type() != dispImg.type())
	{
		tool = cv::Mat::zeros(tool.size(), dispImg.type());
		for (int i = 0; i < tool.rows*tool.cols; ++i)
		{
			tool.at<cv::Vec3b>(i) = cv::Vec3b(255, 0, 0);
		}
	}
	tool.copyTo(dispImg(rect));
	handleROIUpdate(currentROI);
}
void
imageEdit::mirrorErase(cv::Rect rect)
{
	orgImg(rect).copyTo(dispImg(rect));
	handleROIUpdate(currentROI);
}
void
imageEdit::receivePairedImage(cv::Mat img, cv::Rect ROI)
{

}
void 
imageEdit::drawImg(cv::Mat img)
{
	int frameWidth = width();
	int frameHeight = height();
	int imgWidth = img.cols;
	int imgHeight = img.rows;
	int x = 100;
	//applyColorMap(100);
	boost::posix_time::ptime start = boost::posix_time::microsec_clock::universal_time();
#ifndef OPENGL
	if (img.type() == CV_8UC3)
	{
		QImage tmpImg((uchar*)img.data, img.cols, img.rows, img.step, QImage::Format_RGB888);
		pixmap = QPixmap::fromImage(tmpImg);
		imgDisp->setPixmap(pixmap.scaled(imgDisp->width(), imgDisp->height()));
	}
	if (img.type() == CV_8U)
	{
		// Color mapping is now handled on the whole image at the beginning
		if (false)
		{
			cv::Mat disp;
			cv::minMaxIdx(img, &minVal, &maxVal);
			disp = 255 * (img - minVal) / (maxVal - minVal);
			disp.convertTo(disp, CV_8U);
            //cv::applyColorMap(disp, disp, cv::COLORMAP_HSV);
			QImage tmpImg((uchar*)disp.data, disp.cols, disp.rows, disp.step, QImage::Format_RGB888);
			pixmap = QPixmap::fromImage(tmpImg);
			imgDisp->setPixmap(pixmap.scaled(imgDisp->width(), imgDisp->height()));
		}else
		{
			QImage tmpImg((uchar*)img.data, img.cols, img.rows, img.step, QImage::Format_Indexed8);
			pixmap = QPixmap::fromImage(tmpImg);
			imgDisp->setPixmap(pixmap.scaled(imgDisp->width(), imgDisp->height()));
		}
	}
	if (img.type() == CV_16U)
	{
		cv::Mat disp;
		cv::minMaxIdx(img, &minVal, &maxVal);
		disp = 255 * (img - minVal) / (maxVal - minVal);
		disp.convertTo(disp, CV_8U);
        //cv::applyColorMap(disp, disp, cv::COLORMAP_HSV);
		QImage tmpImg((uchar*)disp.data, disp.cols, disp.rows, disp.step, QImage::Format_RGB888);
		pixmap = QPixmap::fromImage(tmpImg);
		imgDisp->setPixmap(pixmap.scaled(imgDisp->width(), imgDisp->height()));
	}
	if (img.type() == CV_32F)
	{
		double minVal, maxVal;
		cv::Mat disp;
		cv::minMaxIdx(img, &minVal, &maxVal);
		disp = 255 * (img - minVal) / (maxVal - minVal);
		disp.convertTo(disp, CV_8U);
        //cv::applyColorMap(disp, disp, cv::COLORMAP_HSV);
		QImage tmpImg((uchar*)disp.data, disp.cols, disp.rows, disp.step, QImage::Format_Indexed8);
		pixmap = QPixmap::fromImage(tmpImg);
		imgDisp->setPixmap(pixmap.scaled(imgDisp->width(), imgDisp->height()));
	}
	if (img.type() == CV_32FC3)
	{
		cv::Mat disp;
		convertScaleAbs(img, disp);
		QImage tmpImg((uchar*)disp.data, disp.cols, disp.rows, disp.step, QImage::Format_RGB888);
		pixmap = QPixmap::fromImage(tmpImg);
		imgDisp->setPixmap(pixmap.scaled(imgDisp->width(), imgDisp->height()));
	}
	boost::posix_time::ptime end = boost::posix_time::microsec_clock::universal_time();
	boost::posix_time::time_duration delta = end - start;
	log("Displaying image took " + QString::number(delta.total_milliseconds()) + " milliseconds", 0);
#else
	imgDisp->showImage(img);

#endif

}
cv::Mat 
imageEdit::colorMapImg(cv::Mat img)
{
	// Check if current bounds are sufficient
	double tmpMin, tmpMax;
	cv::minMaxIdx(img, &tmpMin, &tmpMax);
	if (tmpMin != minVal || tmpMax != maxVal)
	{
		updateLUT = true;
		minVal = tmpMin;
		maxVal = tmpMax;
	}
	// Build a lookup table for this dataset if needed
	if (LUT.size() == 0 || updateLUT)
	{
		buildLUT(maxVal - minVal);
	}
	if (orgImg.type() == CV_16U || orgImg.type() == CV_8U)
	{
		boost::posix_time::ptime start = boost::posix_time::microsec_clock::universal_time();
		float scale = (float)COLORMAP_RESOLUTION / (maxVal - minVal);
		cv::Mat output = cv::Mat::zeros(orgImg.size(), CV_8UC3);
        std::vector<boost::shared_ptr<boost::thread> > threads;
		threads.reserve(numThreads);
		for (int i = 0; i < numThreads; ++i)
		{
            boost::shared_ptr<boost::thread> thr(new boost::thread( boost::bind(&imageEdit::colorMapHelper, orgImg, output, minVal, LUT, i, numThreads) ) );
            threads.push_back(thr);
		}
		for (int i = 0; i < numThreads; ++i)
		{
            threads[i]->join();
		}
		cv::Mat tmp = output; // Just used so I can check the image with image watch
		boost::posix_time::ptime end = boost::posix_time::microsec_clock::universal_time();
		boost::posix_time::time_duration delta = end - start;
		log("Color mapping took " + QString::number(delta.total_milliseconds()) + " milliseconds", 0);
		return output;
	}
	return img;
}
void
imageEdit::colorMapHelper(cv::Mat src, cv::Mat dest, int minVal, std::vector<cv::Vec3b>& LUT, int threadNum, int numThreads)
{
	if (dest.size() != src.size()) return;
	if (LUT.size() < 10) return;
	if (src.type() == CV_8U)
	{
		unsigned char val;
		for (int i = threadNum; i < src.rows*src.cols; i += numThreads)
		{
			val = src.at<unsigned char>(i);
			dest.at<cv::Vec3b>(i) = LUT[val - minVal];
		}
	}
	if (src.type() == CV_16S)
	{
		unsigned short val;
		for (int i = threadNum; i < src.rows*src.cols; i += numThreads)
		{
			val = src.at<unsigned short>(i);
			dest.at<cv::Vec3b>(i) = LUT[val - minVal];
		}
	}
	
}
void 
imageEdit::adjustROI(cv::Mat img)
{
	if (orgImg.size != img.size)
		currentROI = cv::Rect(0, 0, img.cols, img.rows);
	else
	{
		if (currentROI.x + currentROI.width > img.cols || currentROI.y + currentROI.height > img.rows)
		{
			currentROI = cv::Rect(0, 0, img.cols, img.rows);
		}
	}
}
void 
imageEdit::buildLUT(int maxVal)
{
	boost::posix_time::ptime start = boost::posix_time::microsec_clock::universal_time();
	double range = minVal - maxVal;
	//double scale = range / (double)COLORMAP_RESOLUTION;
	LUT.resize(maxVal + 1);
	colorScale RED(50,			255 / 25,	true);
	colorScale GREEN(25 / 3,	255 / 25,	true);
	colorScale BLUE(0,			255/25,		true);
	BLUE.inverted = true;
	double step = 100.0 / (double)maxVal;
	double location = 0.0;
	for (int i = 0; i != maxVal; ++i, location += step)
	{
		LUT[i] = cv::Vec3b(RED(location), GREEN(location), BLUE(location));
	}
	boost::posix_time::ptime end = boost::posix_time::microsec_clock::universal_time();
	boost::posix_time::time_duration delta = end - start;
	emit log("Lookup table construction took " + QString::number(delta.total_milliseconds()) + " milliseconds", 0);
}
cv::Rect 
imageEdit::calcRect(QPoint start_, QPoint end_)
{
    QString output = "Original Image: " + QString::number(orgImg.cols) + " x " + QString::number(orgImg.rows) +
            " Display label: " + QString::number(imgDisp->width()) + " x " + QString::number(imgDisp->height());
	emit log(output, 0);
    float scaleX = (float)orgImg.cols / (float)imgDisp->width();
    float scaleY = (float)orgImg.rows / (float)imgDisp->height();
    float minX = scaleX*(float)std::min(start_.x(), end_.x());
	float maxX = scaleX*(float)std::max(start_.x(), end_.x());
	float minY = scaleY*(float)std::min(start_.y(), end_.y());
	float maxY = scaleY*(float)std::max(start_.y(), end_.y());
    output = "Selected Rectangle: " + QString::number(minX) + " " + QString::number(minY) + " " +
            QString::number(maxX) + " " + QString::number(minY);
	log(output, 0);
    cv::Rect rectangle(minX,minY,maxX - minX, maxY - minY);
    emit rectSelect(rectangle);
    return rectangle;
}
void 
imageEdit::updateROI(cv::Rect ROI, bool source)
{
    if(zoomFactor == 1)
        ROI = cv::Rect(0,0,orgImg.cols, orgImg.rows);
    if(ROI.x < 0)
        ROI.x = 0;
    if(ROI.y < 0)
        ROI.y = 0;
    if(ROI.x + ROI.width > orgImg.cols)
        ROI.x -= ROI.x + ROI.width - orgImg.cols;
    if(ROI.y + ROI.height > orgImg.rows)
        ROI.y -= ROI.y + ROI.height - orgImg.rows;
    currentROI = ROI;
	if (source == true)
		emit newROI(currentROI);
	handleROIUpdate(currentROI);
}
void 
imageEdit::handleROIUpdate(cv::Rect ROI)
{
	currentROI = ROI;
	try
	{
		roiImg = dispImg(ROI);
	}
	catch (cv::Exception &e)
	{
		std::cout << e.what() << std::endl;
		return;
	}
	if (labeling)
		displaySelection();
	else
	{
		drawImg(roiImg);
	}
}
void 
imageEdit::handleDrawToggled(bool val)
{
	_editing = val;
	_drawEnabled = val;
	_eraseEnabled = !val;
	_drawTool = cv::Mat::ones(_toolSize * 2 + 1, _toolSize * 2 + 1, orgImg.type());
	_drawTool = _drawTool * 255;
}
void
imageEdit::handleEraseToggled(bool val)
{
	_editing = val;
	_eraseEnabled = val;
	_drawEnabled = !val;
	_drawTool = cv::Mat::zeros(_toolSize * 2 + 1, _toolSize * 2 + 1, orgImg.type());
}
void 
imageEdit::handleSizeChanged(int val)
{
	_toolSize = val;
	_drawTool = cv::Mat::zeros(_toolSize * 2 + 1, _toolSize * 2 + 1, orgImg.type());
}
void
imageEdit::handleLineToggled(bool val)
{
	_editing = val;
	_drawLine = val;
}
void 
imageEdit::handleLog(QString text, int level)
{
	emit log(text, level);
}
void 
imageEdit::labelImg()
{
    // initialized in constructor to allow loading of classes
	if(classSelect->isHidden()) classSelect->show();
    layout->addWidget(classSelect,2,0);

	if (acceptLabel == NULL)
	{
		acceptLabel = new QPushButton(this);
		acceptLabel->setText("Accept this labeling");
		connect(acceptLabel, SIGNAL(clicked()), this, SLOT(handleAcceptLabel()));
		layout->addWidget(acceptLabel, 2, 1);
	}
    if(acceptLabel->isHidden()) acceptLabel->show();
	
	if (clearLabel == NULL)
	{
		clearLabel = new QPushButton(this);
		clearLabel->setText("Clear Labeling");
		connect(clearLabel, SIGNAL(clicked()), this, SLOT(handleClearLabel()));
		layout->addWidget(clearLabel, 2, 2);
	}
    if(clearLabel->isHidden())  clearLabel->show();


	if (aspectRatioLabel == NULL)
	{
		aspectRatioLabel = new QLabel(this);
		layout->addWidget(aspectRatioLabel, 3, 0);
		aspectRatioLabel->setText("Aspect ratio: Width, Height");
	}
	if (aspectRatioLabel->isHidden()) aspectRatioLabel->show();
	

	if (labelWidth == NULL)
	{
		labelWidth = new QSpinBox(this);
		layout->addWidget(labelWidth, 3, 1);
		labelWidth->setValue(0);
		labelWidth->setMaximum(10000);
		labelWidth->setMinimum(0);
	}
	if (labelWidth->isHidden()) labelWidth->show();
	
	if (labelHeight == NULL)
	{
		labelHeight = new QSpinBox(this);
		layout->addWidget(labelHeight, 3, 2);
		labelHeight->setValue(0);
		labelHeight->setMaximum(10000);
		labelHeight->setMinimum(0);
	}
	if (labelHeight->isHidden()) labelHeight->show();

    polygonInOriginal.clear();
    labeling = true;
}
void 
imageEdit::stopLabelImg()
{
    if(classSelect != NULL)
    {
        classSelect->hide();
        //layout->removeWidget(classSelect);
    }
    if(className != NULL)
    {
        className->hide();
        //layout->removeWidget(className);
    }
    if(classAdd != NULL)
    {
        classAdd->hide();
        //layout->removeWidget(classAdd);
    }
    if(acceptLabel != NULL)
    {
        acceptLabel->hide();
        //layout->removeWidget(acceptLabel);
    }
    if(clearLabel != NULL)
    {
        clearLabel->hide();
        //layout->removeWidget(clearLabel);
    }
	if (aspectRatioLabel != NULL)
		aspectRatioLabel->hide();
	if (labelWidth != NULL)
		labelWidth->hide();
	if (labelHeight != NULL)
		labelHeight->hide();
}
void 
imageEdit::displaySelection()
{
    cv::Mat img;
	roiImg.copyTo(img);
    // Perform drawing steps here
    std::vector<cv::Point> roiPoly(polygonInOriginal.size());
    for(unsigned int i = 0; i < polygonInOriginal.size(); ++i)
    {
        roiPoly[i] = polygonInOriginal[i];
        // Shift to the coordinates of the region of interest
        roiPoly[i].x -= currentROI.x;
        roiPoly[i].y -= currentROI.y;
    }
    // roiPoly is in the global coordinate system :( need to move to the local coordinates
	cv::polylines(img, roiPoly, true, cv::Scalar(0, 0, 255),5);
	drawImg(img);
}
void 
imageEdit::handleClearLabel()
{
    polygonInOriginal.clear();
    updateROI(currentROI);
}
void 
imageEdit::handleAcceptLabel()
{
    cv::Mat labelMask = cv::Mat::zeros(orgImg.rows,orgImg.cols,CV_8U);
	if (polygonInOriginal.size() == 0) return;
    const cv::Point* elementPoints[1] = {&polygonInOriginal[0]};
    int numPoints = (int)polygonInOriginal.size();
    fillPoly(labelMask, elementPoints,&numPoints,1,255);
	QDir dir;
	dir.mkdir(curCont->dirName + "/labels");
    int lbl;
    QString txtLabel = classSelect->currentText();
	lbl = handleAddClass();
	
    for(int i = 0; i < curCont->childCount(); ++i)
    {
        container* cont = dynamic_cast<container*>(curCont->child(i));
		if (cont->type != container::Label) continue;
		labelContainer* tmp = dynamic_cast<labelContainer*>(cont);
		tmp->load();
		if (tmp->label != lbl) continue;
		tmp->M() = tmp->M() + labelMask;
		tmp->polygons.push_back(polygonInOriginal);
        polygonInOriginal.clear();
		//tmp->saveLabelXML(curCont->dirName + "/labels/" + curCont->baseName + "-" + QString::number(label) + ".yml");
		tmp->save();
		//cv::imwrite((curCont->dirName + "/labels/" + curCont->baseName + "-" + QString::number(label) + ".jpg").toStdString(), tmp->M);
        return;
    }

	labelPtr child( new labelContainer(dynamic_cast<QTreeWidget*>(curCont)) );
	child->className = txtLabel;
	child->M() = labelMask;
	child->isTop = false;
	child->label = lbl;
    child->setText(0,txtLabel + " mask");
	child->setText(1, QString::number(lbl));
	child->name = txtLabel + " mask";
	child->polygons.push_back(polygonInOriginal);
	child->dirName = curCont->dirName + "/labels";
	child->baseName = curCont->baseName;
	polygonInOriginal.clear();
	child->save();
	curCont->addChild(child.get());
	curCont->childContainers.push_back(child);
	emit label(child.get());
}
int 
imageEdit::handleAddClass()
{
	QString name = classSelect->currentText();
	int label = classes.size();
	for (int i = 0; i < classes.size(); ++i)
	{
		if (classes[i].first.compare(name, Qt::CaseInsensitive) == 0)
		{
			label = classes[i].second;
			return label;
		}
	}
	if (name == "-1" || name.compare("negative", Qt::CaseInsensitive) == 0 || name.compare("neg", Qt::CaseInsensitive) == 0) label = -1;
	if (name ==  "1" || name.compare("positive", Qt::CaseInsensitive) == 0 || name.compare("pos", Qt::CaseInsensitive) == 0) label =  1;
	//classSelect->addItem(name);
	classes.push_back( std::pair<QString, int>(name, label) );


	cv::FileStorage fs((curCont->dirName + "/labels/Classes.yml").toStdString(), cv::FileStorage::WRITE);
	fs << "ClassCount" << (int)classes.size();
	for (int i = 0; i < classes.size(); ++i)
	{
		fs << ("Class-" + QString::number(i)).toStdString() << "[";
		fs << "{:" << "name" << classes[i].first.toStdString() << "idx" << classes[i].second << "}";
		fs << "]";
	}
	fs.release();
	return label;
}
