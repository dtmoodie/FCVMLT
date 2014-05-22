#include "imageedit.h"
#include "ui_imageedit.h"

imageEdit::imageEdit(QWidget *parent) :
    QWidget(parent),
    labeling(false),
    classSelect(NULL),
    className(NULL),
    classAdd(NULL),
    clearLabel(NULL),
    acceptLabel(NULL),
    ui(new Ui::imageEdit)
{
    blockUpdate = false;
    zoomFactor = 1.0;
    dragging = false;
    layout = new QGridLayout(this);
    imgDisp = new QLabel(this);
    imgDisp->setMinimumHeight(240);
    imgDisp->setMinimumWidth(320);
    imgDisp->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    layout->addWidget(imgDisp,0,0,1,4);
    imgDisp->installEventFilter(this);
    setLayout(layout);

}

imageEdit::~imageEdit()
{
    delete ui;
}

bool imageEdit::eventFilter(QObject *obj, QEvent *ev)
{
    if(orgImg.empty()) return false;
    if(obj == (QObject*)imgDisp)
    {
        if(ev->type() == QEvent::Resize)
        {
            //scaled = pixmap.scaled(imgDisp->width(),imgDisp->height(),Qt::KeepAspectRatio);

            imgDisp->setPixmap(pixmap.scaled(imgDisp->width(),imgDisp->height() /*,Qt::KeepAspectRatio*/));
            return true;

        }
        if(ev->type() == QEvent::MouseButtonPress)
        {
            QMouseEvent* event = dynamic_cast<QMouseEvent*>(ev);
            dragging = true;
            QPoint pos = event->pos();
            start = pos;
            lastPos = pos;
            emit dragStart(pos);
            if(QApplication::keyboardModifiers() == Qt::AltModifier )
            {
                float scaleX = (float)currentROI.width  / (float)imgDisp->width();
                float scaleY = (float)currentROI.height / (float)imgDisp->height();
                float posY = scaleX*pos.x() + currentROI.x;
                float posX = scaleY*pos.y() + currentROI.y;
                firstClickInOriginal = cv::Point(posY,posX);
                polygonInOriginal.push_back(firstClickInOriginal);
            }
            return true;
        }
        if(ev->type() == QEvent::MouseButtonRelease)
        {
            QMouseEvent* event = dynamic_cast<QMouseEvent*>(ev);
            dragging = false;
            end = event->pos();
            emit dragEnd(event->pos());
            calcRect(start,end);
            return true;

        }
        if(ev->type() == QEvent::MouseMove)
        {
            QMouseEvent* event = dynamic_cast<QMouseEvent*>(ev);
            QPoint pos = event->pos();
            if(dragging == true)
                emit dragPos(pos);
            if(QApplication::keyboardModifiers() != Qt::AltModifier && dragging)
            {
                if(blockUpdate == false)
                {
                    blockUpdate = true;
                    cv::Rect ROI = currentROI;
                    float scaleX = (float)currentROI.width  / (float)imgDisp->width();
                    float scaleY = (float)currentROI.height / (float)imgDisp->height();
                    // Position in ROI image... Need to adjust based on current ROI position

                    float posXS = scaleX*lastPos.x() + currentROI.x;
                    float posYS = scaleY*lastPos.y() + currentROI.y;

                    float posXE = scaleX*pos.x() + currentROI.x;
                    float posYE = scaleY*pos.y() + currentROI.y;

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
            if(QApplication::keyboardModifiers() == Qt::AltModifier  && labeling)
            {
                // Labeling calculations for a polygon
                float scaleX = (float)currentROI.width  / (float)imgDisp->width();
                float scaleY = (float)currentROI.height / (float)imgDisp->height();
                float posY = scaleX*pos.x() + currentROI.x;
                float posX = scaleY*pos.y() + currentROI.y;
                cv::Point orgImgPoint(posY,posX);
                polygonInOriginal.push_back(orgImgPoint);
                displaySelection();
            }

        }
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

void imageEdit::addClasses(QStringList classes)
{
	classSelect->addItems(classes);
}


void imageEdit::changeImg(container *cont)
{
    curCont = cont;
    changeImg(cont->img, !cont->isStream);
}


void imageEdit::changeImg(cv::Mat img, bool update)
{

    if(update)
        currentROI = cv::Rect(0,0,img.cols,img.rows);
	roiImg = img(currentROI);
    if(img.type() == CV_8UC3)
    {
        cv::cvtColor(img,orgImg,CV_BGR2RGB);
        QImage tmpImg((uchar*)orgImg.data, orgImg.cols, orgImg.rows, orgImg.step, QImage::Format_RGB888);
        pixmap = QPixmap::fromImage(tmpImg);
        imgDisp->setPixmap(pixmap.scaled(imgDisp->width(),imgDisp->height()));
    }
    if(img.type() == CV_8U)
    {
        orgImg = img;
        QImage tmpImg((uchar*)orgImg.data, orgImg.cols, orgImg.rows, orgImg.step, QImage::Format_Indexed8);
        pixmap = QPixmap::fromImage(tmpImg);
        //imgDisp->setPixmap(pixmap.scaled(imgDisp->width(),imgDisp->height(),Qt::KeepAspectRatio));
        imgDisp->setPixmap(pixmap.scaled(imgDisp->width(),imgDisp->height()));
    }
    if(img.type() == CV_32F)
    {
        double minVal, maxVal;
        cv::minMaxIdx(img,&minVal, &maxVal);
        orgImg = 255*(img - minVal) / (maxVal - minVal);
        orgImg.convertTo(orgImg,CV_8U);
        cv::applyColorMap(orgImg,orgImg,cv::COLORMAP_HSV);
        QImage tmpImg((uchar*)orgImg.data, orgImg.cols, orgImg.rows, orgImg.step, QImage::Format_RGB888);
        pixmap = QPixmap::fromImage(tmpImg);
        //imgDisp->setPixmap(pixmap.scaled(imgDisp->width(),imgDisp->height(),Qt::KeepAspectRatio));
        imgDisp->setPixmap(pixmap.scaled(imgDisp->width(),imgDisp->height()));

    }
    if(img.type() == CV_32FC3)
    {
        convertScaleAbs( img, orgImg );
        cv::cvtColor(orgImg,orgImg,CV_BGR2RGB);
        QImage tmpImg((uchar*)orgImg.data, orgImg.cols, orgImg.rows, orgImg.step, QImage::Format_RGB888);
        pixmap = QPixmap::fromImage(tmpImg);
        imgDisp->setPixmap(pixmap.scaled(imgDisp->width(),imgDisp->height()));
    }
    //labelImg();
}

cv::Rect imageEdit::calcRect(QPoint start_, QPoint end_)
{
    QString output = "Original Image: " + QString::number(orgImg.cols) + " x " + QString::number(orgImg.rows) +
            " Display label: " + QString::number(imgDisp->width()) + " x " + QString::number(imgDisp->height());
    emit consoleOutput(output);
    float scaleX = (float)orgImg.cols / (float)imgDisp->width();
    float scaleY = (float)orgImg.rows / (float)imgDisp->height();
    float minX = scaleX*(float)std::min(start_.x(), end_.x());
    float maxX = scaleX*(float)std::max(start_.x(), end_.x());
    float minY = scaleY*(float)std::min(start_.y(), end_.y());
    float maxY = scaleY*(float)std::max(start_.y(), end_.y());
    output = "Selected Rectangle: " + QString::number(minX) + " " + QString::number(minY) + " " +
            QString::number(maxX) + " " + QString::number(minY);
    emit consoleOutput(output);
    cv::Rect rectangle(minX,minY,maxX - minX, maxY - minY);
    emit rectSelect(rectangle);
    return rectangle;
}

void imageEdit::updateROI(cv::Rect ROI)
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
    roiImg;
    try
    {
        roiImg = orgImg(ROI);
    }
    catch(cv::Exception &e)
    {
        std::cout << e.what() << std::endl;
        return;
    }
    if(labeling)
        displaySelection();
    else
    {
        QImage tmpImg;
        if(roiImg.type() == CV_8UC3)
        {
            tmpImg = QImage((uchar*)roiImg.data, roiImg.cols, roiImg.rows, roiImg.step, QImage::Format_RGB888);
        }else
        {
            if(roiImg.type() == CV_8U)
            {
                tmpImg = QImage((uchar*)roiImg.data, roiImg.cols, roiImg.rows, roiImg.step, QImage::Format_Indexed8);
            }
        }
        pixmap = QPixmap::fromImage(tmpImg);
        imgDisp->setPixmap(pixmap.scaled(imgDisp->width(),imgDisp->height()));
    }
}

void imageEdit::labelImg()
{
    if(classSelect == NULL)     classSelect = new QComboBox(this);
    if(classSelect->isHidden()) classSelect->show();
    layout->addWidget(classSelect,1,0);

    if(className == NULL)       className = new QPlainTextEdit(this);
    if(className->isHidden())   className->show();

    className->setMaximumHeight(classSelect->height());
    layout->addWidget(className,1,1);

    if(classAdd == NULL)        classAdd = new QPushButton(this);
    if(classAdd->isHidden())    classAdd->show();

    classAdd->setText("Add class");
    connect(classAdd, SIGNAL(clicked()), this, SLOT(handleAddClass()));
    layout->addWidget(classAdd,1,2);

    if(acceptLabel == NULL)     acceptLabel = new QPushButton(this);
    if(acceptLabel->isHidden()) acceptLabel->show();

    acceptLabel->setText("Accept this labeling");
    connect(acceptLabel, SIGNAL(clicked()), this, SLOT(handleAcceptLabel()));

    if(clearLabel == NULL)      clearLabel = new QPushButton(this);
    if(clearLabel->isHidden())  clearLabel->show();

    connect(clearLabel, SIGNAL(clicked()), this, SLOT(handleClearLabel()));
    clearLabel->setText("Clear Labeling");


    layout->addWidget(acceptLabel,2,0);
    layout->addWidget(clearLabel,2,1);
    polygonInOriginal.clear();
    labeling = true;
}

void imageEdit::stopLabelImg()
{
    if(classSelect != NULL)
    {
        classSelect->hide();
        layout->removeWidget(classSelect);
    }
    if(className != NULL)
    {
        className->hide();
        layout->removeWidget(className);
    }
    if(classAdd != NULL)
    {
        classAdd->hide();
        layout->removeWidget(classAdd);
    }
    if(acceptLabel != NULL)
    {
        acceptLabel->hide();
        layout->removeWidget(acceptLabel);
    }
    if(clearLabel != NULL)
    {
        clearLabel->hide();
        layout->removeWidget(clearLabel);
    }
}

void imageEdit::displaySelection()
{
    cv::Mat drawImg;
    roiImg.copyTo(drawImg);
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
    cv::polylines(drawImg, roiPoly, true, cv::Scalar(0,0,255));
	cv::cvtColor(drawImg, drawImg, CV_BGR2RGB);
    QImage tmpImg((uchar*)drawImg.data, drawImg.cols, drawImg.rows, drawImg.step, QImage::Format_RGB888);
    pixmap = QPixmap::fromImage(tmpImg);
    imgDisp->setPixmap(pixmap.scaled(imgDisp->width(),imgDisp->height()));
}

void imageEdit::handleClearLabel()
{
    polygonInOriginal.clear();
    updateROI(currentROI);
}

void imageEdit::handleAcceptLabel()
{
    cv::Mat labelMask = cv::Mat::zeros(orgImg.rows,orgImg.cols,CV_8U);
	if (polygonInOriginal.size() == 0) return;
    const cv::Point* elementPoints[1] = {&polygonInOriginal[0]};
    int numPoints = (int)polygonInOriginal.size();
    fillPoly(labelMask, elementPoints,&numPoints,1,255);

    int label;
    QString txtLabel = classSelect->currentText();
	label = classSelect->currentIndex();
    if(txtLabel == "-1" ||txtLabel == "Negative" || txtLabel == "negative" || txtLabel == "neg") label = -1;
    if(txtLabel == "1" ||txtLabel == "Positive" || txtLabel == "positive" || txtLabel == "pos") label = 1;

    for(int i = 0; i < curCont->childCount(); ++i)
    {
        container* cont = dynamic_cast<container*>(curCont->child(i));
        if(cont->isLabel && cont->label == label)
        {
            cont->img = cont->img + labelMask;
            polygonInOriginal.clear();
            return;
        }
    }

    //emit label(labelMask,classSelect->currentIndex());
    container* child = new container(dynamic_cast<QTreeWidget*>(curCont));
    child->img = labelMask;
    child->isLabel = true;
    child->isTop = false;
    child->label = classSelect->currentIndex();
    child->setText(0,classSelect->currentText() + " mask");
    child->setText(1,QString::number(label));

    QDir dir;
    dir.mkdir(curCont->dirName + "/labels");
    cv::imwrite((curCont->dirName + "/labels/" + curCont->baseName +"-" +  QString::number(label) + ".jpg").toStdString(),labelMask);
    curCont->addChild(child);
    QFile file(curCont->dirName + "/labels/Classes.txt");
    file.open(QIODevice::WriteOnly | QIODevice::Text);
    QTextStream out(&file);
    for(int i = 0; i < classSelect->count(); ++i)
    {
        if(i != 0)
        {
            out << "\n";
        }
        if(classSelect->itemText(i) == "-1"              ||
           classSelect->itemText(i) == "Negative"   ||
           classSelect->itemText(i) == "negative")
        {
            out << -1 << " ";
        }else
        {
            if(classSelect->itemText(i) == "1"           ||
               classSelect->itemText(i) == "Positive"   ||
               classSelect->itemText(i) == "positive")
            {
                out << 1 << " ";
            }else
            {
                out << i << " ";
            }
        }
        out << classSelect->itemText(i);
    }
    file.close();
}

void imageEdit::handleAddClass()
{
    classSelect->addItem(className->toPlainText());
}
