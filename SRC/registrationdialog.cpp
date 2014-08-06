#include "registrationdialog.h"
#include "ui_registrationdialog.h"

registrationDialog::registrationDialog(QWidget *parent, imgSourcesWidget* sourceList) :
QDialog(parent),
ui(new Ui::registrationDialog),
selectSource(false),
selectInput(false),
currentOverlayMethod(Edges),
_sourceList(sourceList)
{
	ui->setupUi(this);
	connect(ui->registered, SIGNAL(keyPress(int)), this, SLOT(handleKeyPress(int)));
	ui->registered->installEventFilter(this);
	ui->source->installEventFilter(this);
	ui->boarderMethod->installEventFilter(this);
	ui->btnCancel->installEventFilter(this);
	ui->btnSaveRegistration->installEventFilter(this);
	ui->btnSelectInput->installEventFilter(this);
	ui->btnSelectSource->installEventFilter(this);
	ui->displayMethod->installEventFilter(this);
	ui->technique->installEventFilter(this);
	currentTransform = cv::Mat::eye(2, 3, CV_32F);

	ui->displayMethod->addItem("Edges");
	ui->displayMethod->addItem("Overlay");

	ui->technique->addItem("Manual");
	ui->technique->addItem("Line constraint");
	ui->technique->addItem("Points");

	connect(ui->registered, SIGNAL(dragStart(QPoint)), this, SLOT(handleDragStart(QPoint)));
	connect(ui->registered, SIGNAL(dragPos(QPoint)), this, SLOT(handleDrag(QPoint)));
	connect(ui->input, SIGNAL(lineDrawn(cv::Point, cv::Point)), this, SLOT(handleLineSelect(cv::Point, cv::Point)));
	connect(ui->source, SIGNAL(lineDrawn(cv::Point, cv::Point)), this, SLOT(handleLineSelect(cv::Point, cv::Point))); 
	connect(ui->input, SIGNAL(imageChanged(imgPtr)), this, SLOT(handleImageChanged(imgPtr)));
	connect(ui->source, SIGNAL(imageChanged(imgPtr)), this, SLOT(handleImageChanged(imgPtr)));
	_sourceVec = cv::Vec2f(-1, -1);
	_destVec = cv::Vec2f(-1, -1);
	_sourcePt = cv::Vec2f(-1, -1);
	_destPt = cv::Vec2f(-1, -1);
	_keyPressTimer = new QTimer();
	_keyPressTimer->start(40);
	connect(_keyPressTimer, SIGNAL(timeout()), this, SLOT(checkKeyRelease()));
	connect(ui->source, SIGNAL(newROI(cv::Rect)), ui->input, SLOT(handleROIUpdate(cv::Rect)));
	connect(ui->input, SIGNAL(newROI(cv::Rect)), ui->source, SLOT(handleROIUpdate(cv::Rect)));
	Qt::WindowFlags flags = windowFlags();
	flags |= Qt::WindowMaximizeButtonHint;
	setWindowFlags(flags);
	ui->input->sourceList = sourceList;
	ui->source->sourceList = sourceList;
}
registrationDialog::~registrationDialog()
{
    delete ui;
}
bool 
registrationDialog::eventFilter(QObject* o, QEvent* e)
{
	if (e->type() == QEvent::KeyPress)
	{
		QKeyEvent* keyEvent = dynamic_cast<QKeyEvent*>(e);
		int key = keyEvent->key();

        if (key == Qt::Key::Key_Up || key == Qt::Key::Key_Down ||
			key == Qt::Key::Key_Right || key == Qt::Key::Key_Left)
		{
			_keyPressed = true;
			_keyPreviouslyPressed = true;
			// Resets the timeout if the key is pressed again
			_keyPressTimer->start();
			handleKeyPress(key);
			return true;
		}
		if (key == Qt::Key::Key_Z || QApplication::keyboardModifiers() == Qt::ControlModifier)
		{
			// Undo the last transform
			if (_transformHistory.size() == 0) return true;
			if (_transformHistory.size() == 1)
			{
				currentTransform = cv::Mat::eye(2, 3, CV_32F);
			}else
			{
				_transformHistory.pop_back();
				currentTransform = _transformHistory[_transformHistory.size() - 1];
			}
			cv::warpAffine(currentInput->M(), transformedInput, currentTransform, currentInput->M().size());
			updateRegistered(transformedInput);
			return true;
		}
	}
	if (e->type() == QEvent::KeyRelease)
	{
		QKeyEvent* keyEvent = dynamic_cast<QKeyEvent*>(e);
		int key = keyEvent->key();
		if (key == Qt::Key::Key_Up || key == Qt::Key::Key_Down ||
			key == Qt::Key::Key_Right || key == Qt::Key::Key_Left)
		{
			_keyPressed = false;
		}
	}
	return false;
}
void 
registrationDialog::on_btnSelectSource_clicked(bool val)
{
	selectSource = val;
}
void 
registrationDialog::on_btnSelectInput_clicked(bool val)
{
	selectInput = val;
}
void 
registrationDialog::on_btnSaveRegistration_clicked()
{
	imgPtr child( new imgContainer(currentInput.get())) ;
	child->setText(0, "Registered");
	transformedInput.copyTo(child->M());
	child->name = "Registered";
	child->dirName = currentInput->dirName + "/Registered";
	child->baseName = currentInput->baseName;
	child->cached = true;
	child->isTop = false;
	child->save();
	currentInput->addChild(child.get());
	currentInput->childContainers.push_back(child);
}
void 
registrationDialog::on_btnCancel_clicked()
{

}
void 
registrationDialog::on_technique_currentIndexChanged(int index)
{
	currentRegistrationTechnique = (registrationTechnique)index;
	if (currentSource == NULL) return;
	if (currentRegistrationTechnique == Line)
	{
		ui->input->handleLineToggled(true);
		ui->source->handleLineToggled(true);
	}else
	{
		ui->input->handleLineToggled(false);
		ui->source->handleLineToggled(false);
	}
}
void 
registrationDialog::on_displayMethod_currentIndexChanged(int index)
{
	currentOverlayMethod = (overlayMethod)index;
	if (currentSource == NULL) return;
	if (currentOverlayMethod == Edges)
	{
		cv::Mat edges;
		cv::Canny(currentSource->M(), edges, 50, 100, 3);
		overlayBase = cv::Mat::zeros(edges.size(), CV_8UC3);
		for (int i = 0; i < edges.rows * edges.cols; ++i)
		{
			overlayBase.at<cv::Vec3b>(i) = cv::Vec3b(0, 0, edges.at<uchar>(i));
		}
	}
	else
	{
		overlayBase = currentSource->M() / 2;
	}
	applyTransform();
}
void 
registrationDialog::on_boarderMethod_currentIndexChanged(int index)
{

}
void
registrationDialog::handleImageChanged(imgPtr img)
{
	if (sender() == (QObject*)ui->source)
	{
		currentSource = img;
		if (currentOverlayMethod == Edges)
		{
			cv::Mat edges;
			cv::Canny(img->M(), edges, 50, 100, 3);
			overlayBase = cv::Mat::zeros(edges.size(), CV_8UC3);
			for (int i = 0; i < edges.rows * edges.cols; ++i)
			{
				overlayBase.at<cv::Vec3b>(i) = cv::Vec3b(0, 0, edges.at<uchar>(i));
			}
		}
		else
		{
			overlayBase = img->M() / 2;
		}

	}
	if (sender() == (QObject*)ui->input)
	{
		currentInput = img;
		updateRegistered(img->M());
	}
}
void 
registrationDialog::handleImageSelect(containerPtr cont)
{
	if (cont->type != container::Img) return;
	imgPtr img = boost::dynamic_pointer_cast<imgContainer,container>(cont);
	if (selectSource)
	{
		currentSource = img;
		ui->source->changeImg(img);
		selectSource = false;
		ui->btnSelectSource->setChecked(false);
		if (currentOverlayMethod == Edges)
		{
			cv::Mat edges;
			cv::Canny(img->M(), edges, 50, 100, 3);
			overlayBase = cv::Mat::zeros(edges.size(), CV_8UC3);
			for (int i = 0; i < edges.rows * edges.cols; ++i)
			{
				overlayBase.at<cv::Vec3b>(i) = cv::Vec3b(0, 0, edges.at<uchar>(i));
			}
		}else
		{
			overlayBase = img->M() / 2;
		}
	}else
	{
		if (selectInput)
		{
			currentInput = img;
			ui->input->changeImg(img);
			selectInput = false;
			ui->btnSelectInput->setChecked(false);
			updateRegistered(img->M());
		}
	}
}
void
registrationDialog::handleLineSelect(cv::Point start, cv::Point end)
{
	if (sender() == (QObject*)ui->source)
	{
		_sourcePt = cv::Vec2f(start.x, start.y);
		_sourceVec = cv::Vec2f(end.x - start.x, end.y - start.y);
		_sourceVec = _sourceVec / cv::norm(_sourceVec);
		if (_destVec[0] == -1 && _destVec[1] == -1)
		{
			_destVec = _sourceVec;
		}
		if (_destPt[0] == -1 && _destPt[1] == -1)
		{
			_destPt = _sourcePt;
		}
	}
	if (sender() == (QObject*)ui->input)
	{
		_destPt = cv::Vec2f(start.x, start.y);
		_destVec = cv::Vec2f(end.x - start.x, end.y - start.y);
		_destVec = _destVec / cv::norm(_destVec);
		if (_sourceVec[0] == -1 && _sourceVec[1] == -1)
		{
			_sourceVec = _destVec;
		}
		if (_sourcePt[0] == -1 && _sourcePt[1] == -1)
		{
			_sourcePt = _destPt;
		}
	}
	// Determine rotation between the lines
	float theta = acos(_sourceVec.dot(_destVec));
	
	cv::Vec2f tmpVec = (_sourcePt - _destPt) - (_sourcePt - _destPt).dot(_sourceVec)*(_sourceVec);
	bool signX = tmpVec[0] > 0;
	bool signY = tmpVec[1] > 0;
	float d = cv::norm(tmpVec);
	cv::Mat T = cv::getRotationMatrix2D(cv::Point(0, 0), theta*180/3.14, 1);
	T.convertTo(T, CV_32F);
	cv::Mat R = T(cv::Range(0, 2), cv::Range(0, 2));
	R.copyTo(currentTransform(cv::Range(0, 2), cv::Range(0, 2)));
	cv::Mat tmp = currentTransform;
	if (signX)
	{
		currentTransform.at<float>(0, 2) = d;
	}else
	{
		currentTransform.at<float>(0, 2) = -d;
	}
	applyTransform();
}
void 
registrationDialog::checkKeyRelease()
{
	if (_keyPreviouslyPressed && !_keyPressed)
	{
		// Save this current transform so we can undo
		_transformHistory.push_back(currentTransform.clone());
		_keyPreviouslyPressed = false;
	}
}
void 
registrationDialog::handleKeyPress(int key)
{
	//emit log("Key pressed: " + QString::number(key), 0);
	if (key == Qt::Key::Key_Up)
	{
		--currentTransform.at<float>(1, 2);
	}
	if (key == Qt::Key::Key_Down)
	{
		++currentTransform.at<float>(1, 2);
	}
	if (key == Qt::Key::Key_Right)
	{
		++currentTransform.at<float>(0, 2);
	}
	if (key == Qt::Key::Key_Left)
	{
		--currentTransform.at<float>(0, 2);
	}
	applyTransform();
}
void 
registrationDialog::handleDragStart(QPoint pt)
{
	affineCenter = pt;
}
void 
registrationDialog::handleDrag(QPoint pt)
{
	if (QApplication::keyboardModifiers() != Qt::ControlModifier) return;
	float x = affineCenter.x() - pt.x();
	float y = affineCenter.y() - pt.y();
	float dist = x + y;
	dist = dist / 100;
	cv::Mat T = cv::getRotationMatrix2D(cv::Point2f(affineCenter.x(), affineCenter.y()), dist, 1);
	T.convertTo(T, CV_32F);
	cv::Mat R = T(cv::Range(0, 2), cv::Range(0, 2));
	currentTransform(cv::Range(0, 2), cv::Range(0, 2)) = R*currentTransform(cv::Range(0, 2), cv::Range(0, 2));
	applyTransform();
}
void 
registrationDialog::handleDragEnd(QPoint pt)
{
	// Save this current transform so we can undo
	_transformHistory.push_back(currentTransform.clone());
}
void 
registrationDialog::applyTransform()
{
	if (currentTransform.empty()) return;
	cv::warpAffine(currentInput->M(), transformedInput, currentTransform, currentInput->M().size());
	updateRegistered(transformedInput);
}

void
registrationDialog::updateRegistered(cv::Mat img)
{
	if (img.empty()) return;
	cv::Mat disp;
	if (currentOverlayMethod == Overlay)
	{
		disp = overlayBase + img / 2;
	}
	if (currentOverlayMethod == Edges)
	{
		disp = img + overlayBase;
	}
	ui->registered->changeImg(disp);
}
