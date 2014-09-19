#include "measurementdialog.h"
#include "ui_measurementdialog.h"

measurementDialog::measurementDialog(QWidget *parent, imgSourcesWidget* sourceList_) :
    QDialog(parent),
    ui(new Ui::measurementDialog)
{
    ui->setupUi(this);
	ui->referenceImg->setSourceList(sourceList_);
	ui->maskImg->setSourceList(sourceList_);
	ui->lineImg->setSourceList(sourceList_);
	connect(ui->maskImg, SIGNAL(imgReceived(containerPtr)), this, SLOT(onImgDrop(containerPtr)));
	connect(ui->lineImg, SIGNAL(imgReceived(containerPtr)), this, SLOT(onImgDrop(containerPtr)));
	connect(ui->referenceImg, SIGNAL(imgReceived(containerPtr)), this, SLOT(onImgDrop(containerPtr)));

}

measurementDialog::~measurementDialog()
{
    delete ui;
}
void
measurementDialog::onImgDrop(containerPtr cont)
{
	if (cont->type == container::Img || cont->type == container::Label)
	{
		imgPtr img = boost::dynamic_pointer_cast<imgContainer, container>(cont);
		if (sender() == ui->referenceImg)
		{
			reference = img;
		}
		if (sender() == ui->maskImg)
		{
			mask = img;
		}
		if (sender() == ui->lineImg)
		{
			line = img;
		}
		calculate();
	}	
}
void
measurementDialog::calculate()
{
	if (reference == NULL || line == NULL) return;
	// Find reference coordinate system
	
	containerPtr refCont = dynamic_cast<container*>(reference->parent())->getChild("Reference");
	referencePtr ref = boost::dynamic_pointer_cast<referenceContainer, container>(refCont);
	if (refCont == NULL)
	{
		emit log("No coordinate system defined, define a coordinate system in the reference image", 4);
		return;
	}
	cv::Mat img = line->M();
	cv::Mat refImg = reference->M();
	refImg = refImg > 100;
	img = img > 100;
	if(refImg.channels() != 1) cv::cvtColor(refImg, refImg, CV_BGR2GRAY);
	if (img.channels() != 1) cv::cvtColor(img, img, CV_BGR2GRAY);
	if (mask == NULL)
	{
		emit log("Warning no mask present", 5);
	}else
	{
		cv::Mat maskImg = mask->M();
		maskImg = maskImg > 100;
		if (maskImg.channels() != 1) cv::cvtColor(maskImg, maskImg, CV_BGR2GRAY);
		img = img.mul(maskImg);
	}
	std::vector<cv::Point2f> points;
	std::vector<cv::Point2f> refPoints;
	std::vector<float> dist;
	for (int y = 0; y < img.rows; ++y)
	{
		for (int x = 0; x < img.cols; ++x)
		{
			if (img.at<uchar>(y, x) != 0)
			{
				for (int i = x; i < x + 100; ++i)
				{
					if (refImg.at<uchar>(y, i) != 0)
					{
						dist.push_back(i - x);
						points.push_back(cv::Point2f(x, y));
						refPoints.push_back(cv::Point2f(i, y));
					}
				}
			}
		}
	}
	if (points.size() > 0)
	{
		// grrrrr Opencv needs a better way of multiplying a homogeneous matrix by a transform
		cv::Mat pts = cv::Mat::ones(3, points.size(), CV_32F);
		cv::Mat refPts = cv::Mat::ones(3, points.size(), CV_32F);
		for (int i = 0; i < points.size(); ++i)
		{
			pts.at<float>(0, i) = points[i].x - ref->H.at<float>(0,2);
			pts.at<float>(1, i) = points[i].y - ref->H.at<float>(1,2);
			refPts.at<float>(0, i) = refPoints[i].x - ref->H.at<float>(0,2);
			refPts.at<float>(1, i) = refPoints[i].y - ref->H.at<float>(1, 2);
		}
		pts = pts * ref->scale;
		refPts = refPts*ref->scale;
		//pts = ref->transformPoints(pts);
		//refPts = ref->transformPoints(refPts);
		// Pts are now in real world units.... Save to disk?
		std::fstream fs;
		container* parent = line->getParent();
		QString path;
		if (parent->filePath.size() == 0)
		{
			path = line->dirName + "/" + line->baseName + ".csv";
		}else
			path = parent->filePath.mid(0, parent->filePath.size() - 4) + ".csv";
		fs.open(path.toStdString(), std::fstream::out);
		fs << "Source X, Source Y, Ref X, Ref Y, dist";
		for (int i = 0; i < refPts.cols; ++i)
		{
			fs	<< std::endl;
			fs	<< pts.at<float>(0, i) << "," << pts.at<float>(1, i) << "," 
				<< refPts.at<float>(0, i) << "," << refPts.at<float>(1, i) << ","
				<< dist[i]*ref->scale;
		}
		fs.close();
	}
}