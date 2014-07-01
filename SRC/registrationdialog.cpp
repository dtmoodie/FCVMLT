#include "registrationdialog.h"
#include "ui_registrationdialog.h"

registrationDialog::registrationDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::registrationDialog),
	selectSource(false),
	selectInput(false),
	currentOverlayMethod(Edges),
	currentSource(NULL),
	currentInput(NULL)
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

	connect(ui->registered, SIGNAL(dragStart(QPoint)),	this, SLOT(handleDragStart(QPoint)));
	connect(ui->registered, SIGNAL(dragPos(QPoint)),	this, SLOT(handleDrag(QPoint)));


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
			handleKeyPress(key);
			return true;
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
	imgContainer* child = new imgContainer(currentInput);
	child->setText(0, "Registered");
	transformedInput.copyTo(child->M());
	child->name = "Registered";
	child->dirName = currentInput->dirName + "Registered";
	child->baseName = currentInput->baseName;
	child->cached = true;
	child->isTop = false;
	child->save();
	currentInput->addChild(child);
}
void 
registrationDialog::on_btnCancel_clicked()
{

}
void 
registrationDialog::on_technique_currentIndexChanged(int index)
{
	
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
}
void 
registrationDialog::on_boarderMethod_currentIndexChanged(int index)
{

}
void 
registrationDialog::handleImageSelect(container* cont)
{
	if (cont->type != container::Img) return;
	imgContainer* img = dynamic_cast<imgContainer*>(cont);
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
			ui->registered->changeImg(img);
			selectInput = false;
			ui->btnSelectInput->setChecked(false);
		}
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