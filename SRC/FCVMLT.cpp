#include "FCVMLT.h"


container::container(QTreeWidget *parent) :
	QTreeWidgetItem(parent),
	isData(false),
	isTop(false),
	saved(false),
	cached(false),
	parentContainer(NULL)
{
	type = Display;
	//updateDisplay();
}
container::container(QTreeWidgetItem* parent) :
	QTreeWidgetItem(parent),
	saved(false),
	cached(false),
	parentContainer(dynamic_cast<container*>(parent))
{
	type = Display;
}
container::~container()
{
	for (unsigned int i = 0; i < childCount(); ++i)
	{
		removeChild((child(i)));
	}
}
bool 
container::load()
{
	return true;
}
bool
container::save()
{
	return true;
}
void 
container::updateDisplay()
{
	setText(0, baseName);
}
// ************************************************ matrixContainer ***************************************
matrixContainer::matrixContainer(QTreeWidget* parent) :
	container(parent)
{
	type = Matrix;
	max = 0;
	min = 0;
	maxMinValid = false;
}
matrixContainer::matrixContainer(QTreeWidgetItem* parent) :
	container(parent)
{
	type = Matrix;
	max = 0;
	min = 0;
	maxMinValid = false;
}
matrixContainer::~matrixContainer()
{

}
cv::Mat& 
matrixContainer::M()
{
	if (_M.empty())
	{
		load();
	}
	return _M;
}
bool 
matrixContainer::load()
{
	return true;
}
// *********************** processingContainer **********************
processingContainer::processingContainer(QTreeWidget* parent):
matrixContainer(parent)
{
	type = Processor;
	saveName = name;
}
processingContainer::processingContainer(QTreeWidgetItem* parent) :
	matrixContainer(parent)
{
	type = Processor;
	saveName = name;
}
processingContainer::processingContainer(processingContainer* cpy):
	parameters(cpy->parameters)
{
	type = cpy->type;
	cached = cpy->cached;
	saved = cpy->saved;
	numThreads = cpy->numThreads;
	name = cpy->name;
	saveName = cpy->saveName;
}
processingContainer::processingContainer(QWidget* parent) :
QObject(parent)
{
	type = Processor;
}
processingContainer::~processingContainer()
{

}
container* 
processingContainer::getDisplayCopy()
{
	processingContainer* out = new processingContainer(this);
	return out;
}
QWidget* 
processingContainer::getParamControlWidget(QWidget* parent)
{
	QWidget* out = new QWidget(parent);
	QGridLayout* layout = new QGridLayout(out);
	for (unsigned int i = 0; i < parameters.size(); ++i)
	{
		QLabel* label = new QLabel(parameters[i].name);
		layout->addWidget(label, i, 0);
		if (parameters[i].type == t_char)
		{
			QSpinBox* box = new QSpinBox;
			box->setMinimum(0);
			box->setMaximum(255);
			box->setObjectName(parameters[i].name);
			box->setValue((int)parameters[i].value);
			parameters[i].box = box;
			layout->addWidget(box, i, 1);
			connect(box, SIGNAL(valueChanged(QString)), this, SLOT(handleParamChange(QString)));
		}
		if (parameters[i].type == t_float)
		{
			QDoubleSpinBox* box = new QDoubleSpinBox;
			box->setObjectName(parameters[i].name);
			box->setValue(parameters[i].value);
			box->setMinimum(parameters[i].minVal);
			box->setMaximum(parameters[i].maxVal);
			box->setSingleStep(0.1);
			box->setValue(parameters[i].value);
			parameters[i].box = box;
			layout->addWidget(box, i, 1);
			connect(box, SIGNAL(valueChanged(QString)), this, SLOT(handleParamChange(QString)));
		}
		if (parameters[i].type == t_double)
		{
			QDoubleSpinBox* box = new QDoubleSpinBox;
			box->setObjectName(parameters[i].name);
			box->setValue(parameters[i].value);
			box->setMinimum(parameters[i].minVal);
			box->setMaximum(parameters[i].maxVal);
			box->setSingleStep(0.1);
			box->setValue(parameters[i].value);
			parameters[i].box = box;
			layout->addWidget(box, i, 1);
			connect(box, SIGNAL(valueChanged(QString)), this, SLOT(handleParamChange(QString)));

		}
		if (parameters[i].type == t_int)
		{
			QSpinBox* box = new QSpinBox;
			box->setObjectName(parameters[i].name);
			box->setValue(parameters[i].value);
			layout->addWidget(box, i, 1);
			box->setMinimum((int)parameters[i].minVal);
			box->setMaximum((int)parameters[i].maxVal);
			box->setValue(parameters[i].value);
			parameters[i].box = box;
			connect(box, SIGNAL(valueChanged(QString)), this, SLOT(handleParamChange(QString)));

		}
		if (parameters[i].type == t_pullDown)
		{
			QComboBox* box = new QComboBox;
			box->setObjectName(parameters[i].name);
			box->setToolTip(parameters[i].toolTip);
			layout->addWidget(box, i, 1);
			box->addItems(parameters[i].pullDownItems);
			parameters[i].box = box;
			connect(box, SIGNAL(currentIndexChanged(QString)), this, SLOT(handleParamChange(QString)));
		}
	}
	QPushButton* accept = new QPushButton;
	QPushButton* cancel = new QPushButton;
	accept->setText("Accept Changes");
	cancel->setText("Cancel Changes");
	layout->addWidget(accept, parameters.size(), 0);
	layout->addWidget(cancel, parameters.size(), 1);
	connect(accept, SIGNAL(clicked()), this, SLOT(handleAccept()));
	connect(cancel, SIGNAL(clicked()), this, SLOT(handleCancel()));
	out->setLayout(layout);
	out->setWindowTitle(name + " Settings");
	return out;
}
void 
processingContainer::handleParamChange(QString val)
{
		for (unsigned int i = 0; i < parameters.size(); ++i)
	{
		if (sender() == (QObject*)parameters[i].box)
		{
			if (parameters[i].type == t_pullDown)
			{
				for (unsigned int j = 0; j < parameters[i].pullDownItems.size(); ++j)
				{
					if (parameters[i].pullDownItems[j] == val)
					{
						emit log(name + ": " + parameters[i].name + " updated to " + val, 0);
						parameters[i].value = j;
						update();
					}
				}
			}else
			{
				double value = val.toDouble();
				if (value >= parameters[i].minVal && value <= parameters[i].maxVal)
				{
					emit log(name + ": " + parameters[i].name + " updated to " + val, 0);
					parameters[i].value = value;
					update();
				}
			}
			return;
		}
	}
}
void
processingContainer::handleAccept()
{
	emit accepted();
}
void
processingContainer::handleCancel()
{
	emit canceled();
}
void
processingContainer::update()
{

}
void
processingContainer::setInput(container* cont_)
{
	//input(input_);
}
container* 
processingContainer::process(container* cont_)
{
	//input(input_);
	//emit processingFinished(output);
	container* output = new container;
	return output;
}
// ************************************************ featureExtractorContainer ***********************
featureExtractorContainer::featureExtractorContainer(QTreeWidget* parent, statType type_):
	processingContainer(parent)
{
	type = FeatureExtractor;
	sType = type_;
	initialize();
}
featureExtractorContainer::featureExtractorContainer(QTreeWidgetItem* parent, statType type_):
	processingContainer(parent)
{
	type = FeatureExtractor;
	sType = type_;
	initialize();
}
featureExtractorContainer::featureExtractorContainer(QWidget* parent, statType type_):
	processingContainer(parent)
{
	type = FeatureExtractor;
	sType = type_;
	initialize();
}
featureExtractorContainer::featureExtractorContainer(statType type_)
{
	type = FeatureExtractor;
	sType = type_;
	initialize();
}
featureExtractorContainer::~featureExtractorContainer()
{

}
bool 
featureExtractorContainer::extractFeatures(cv::Mat src, cv::Mat& features, cv::Mat& dispImg)
{
	switch (sType)
	{
	case sum:
	{
		cv::Scalar output = cv::sum(src);
		for (int i = 0; i < src.channels(); ++i)
		{
			((float*)features.data)[i] = output[i];
		}
		break;
	}
	case avg:
	{
		cv::Scalar out = cv::mean(src);
		for (int i = 0; i < src.channels(); ++i)
		{
			((float*)features.data)[i] = out[i];
		}
		break;
	}
	case median:
	{
		break;
	}
	case stdev:
		/*
		cv::Mat stddev;
		cv::meanStdDev(src,cv::Mat(), stddev);
		for(unsigned int i = 0; i < src.channels(); ++i)
		{
		features.at<float>(i) = stddev.at<float>(i);
		}*/
		break;
	case hist:
	{
		std::vector<cv::Mat> channels;
		cv::split(src, channels);
		float range[2];
		if (parameters[5].value == 1)
		{
			double minVal, maxVal;
			cv::minMaxLoc(src, &minVal, &maxVal);
			range[0] = (float)minVal;
			range[1] = (float)maxVal;
			parameters[1].value = minVal;
			parameters[2].value = maxVal;
		}
		else
		{
			range[0] = (float)parameters[1].value;
			range[1] = (float)parameters[2].value;
		}
		const float* histRange = { range };
		int histSize = parameters[0].value;
		std::vector<cv::Mat> histPlanes;
		histPlanes.resize(src.channels());
		for (int i = 0; i < src.channels(); ++i)
			cv::calcHist(&channels[i], 1, 0, cv::Mat(), histPlanes[i], 1, &histSize, &histRange, parameters[3].value, parameters[4].value);
		for (int i = 0; i < src.channels(); ++i)
		{
			for (int j = 0; j < histPlanes[i].rows; ++j)
			{
				float val = histPlanes[i].at<float>(j);
				if (features.channels() == 3)
					features.at<cv::Vec3f>(0, j)[i] = val;
				else
					features.at<float>(0, j) = val;
			}
		}
		break;
	}
	
	}
	return true;
}
bool
featureExtractorContainer::extractKeyPoints(cv::Mat src, featureContainer* features, cv::Mat& dispImg)
{
	switch (sType)
	{
	case sift:
	{
				 cv::SIFT sift;
				 std::vector<cv::KeyPoint> keyPts;
				 sift(src, cv::Mat(), keyPts);
				 features->keyPts.reserve(keyPts.size());
				 for (int i = 0; i < keyPts.size(); ++i)
				 {
					 features->keyPts.push_back(keyPts[i].pt);
				 }
				 // Figure out how to get the sift features and put them into features->M
				 cv::drawKeypoints(dispImg, keyPts, dispImg, cv::Scalar(255, 0, 0));
				 break;
	}
	case HoG:
	{
				break;
	}
	case orb:
	{

				break;
	}
	case line:
	{
		//std::vector<cv::Vec2f> lines;
		cv::Mat lines;
		cv::Mat img;
		if (src.channels() != 1)
			cv::cvtColor(src, img, CV_BGR2GRAY);
		else img = src;
		cv::HoughLines(img, 
			lines, 
			parameters[0].value, 
			parameters[1].value*CV_PI / 180, 
			parameters[2].value, 
			parameters[3].value, 
			parameters[4].value);
		float scaleX = 1;
		float scaleY = 1;
		if (src.size != dispImg.size)
		{
			scaleX = dispImg.cols / src.cols;
			scaleY = dispImg.rows / src.rows;
		}
		for (size_t i = 0; i < lines.cols; i++)
		{
			cv::Vec2f line = lines.at<cv::Vec2f>(i);
			//float rho = lines[i][0], theta = lines[i][1];
			float rho = line.val[0], theta = line[1];
			cv::Point pt1, pt2;
			double a = cos(theta), b = sin(theta);
			double x0 = a*rho, y0 = b*rho;
			pt1.x = cvRound(x0 + 1000 * (-b))*scaleX;
			pt1.y = cvRound(y0 + 1000 * (a))*scaleY;
			pt2.x = cvRound(x0 - 1000 * (-b))*scaleX;
			pt2.y = cvRound(y0 - 1000 * (a))*scaleY;
			cv::line(dispImg, pt1, pt2, cv::Scalar(0, 0, 255), 3, CV_AA);
		}
		features->M() = lines;
		break;
	}
	case lineP:
	{
		cv::Mat img;
		if (src.channels() != 1)
			cv::cvtColor(src, img, CV_BGR2GRAY);
		else img = src;
		cv::Mat lines;
		cv::HoughLinesP(img,
			lines,
			parameters[0].value,
			parameters[1].value,
			parameters[2].value,
			parameters[3].value,
			parameters[4].value);
		for (int i = 0; i < lines.cols; ++i)
		{
			cv::Vec4i line = lines.at<cv::Vec4i>(i);
			cv::Point p1, p2;
			p1.x = line.val[0];
			p1.y = line.val[1];
			p2.x = line.val[2];
			p2.y = line.val[3];
			cv::line(dispImg, p1, p2, cv::Scalar(0, 0, 255), 3, CV_AA);
		}
		features->M() = lines;
		break;
	}
	case circle:
	{
		//std::vector<cv::Vec3f> circles;
		cv::Mat circles;
		cv::HoughCircles(src, circles, CV_HOUGH_GRADIENT,
			parameters[0].value,
			parameters[1].value,
			parameters[2].value,
			parameters[3].value,
			parameters[4].value,
			parameters[5].value);
		if (circles.empty()) return false;
		for (size_t i = 0; i < circles.cols; i++)
		{
			cv::Vec3f circle = circles.at<cv::Vec3f>(i);
			cv::Point center(cvRound(circle.val[0]), cvRound(circle.val[1]));
			int radius = cvRound(circle.val[2]);
			// circle center
			cv::circle(dispImg, center, 3, cv::Scalar(0, 255, 0), -1, 8, 0);
			// circle outline
			cv::circle(dispImg, center, radius, cv::Scalar(0, 0, 255), 3, 8, 0);
		}
		features->M() = circles;
		break;
	}
	}
	return true;
}
void
featureExtractorContainer::initialize()
{
	name = STAT_TYPES[(int)sType];
	isKeyPoint = false;
	switch (sType)
	{
	case sum:
		numFeatures = 1;
		name = "Sum";
		break;
	case avg:
		numFeatures = 1;
		name = "Average";
		break;
	case median:
		numFeatures = 1;
		name = "Median";
		break;
	case stdev:
		numFeatures = 1;
		name = "Standard Deviation";
		break;
	case hist:
		name = "Histogram";
		numFeatures = 10;
		parameters.resize(6);
		parameters[0] = param(t_int,	10,		"Num bins");
		parameters[1] = param(t_double, 0,		"Bottom of Range");
		parameters[2] = param(t_double, 255,	"Top of Range");
		parameters[3] = param(t_int,	1,		"Uniform bin sizes");
		parameters[4] = param(t_int,	0,		"Accumulate histograms");
		parameters[5] = param(t_bool,	1,		"Auto select range");
		break;
	case sift:
		name = "Scale invariant feature transform";
		numFeatures = 128;
		isKeyPoint = true;
		break;
	case HoG:
		name = "Histogram of Gradients";
		numFeatures = 1;
		isKeyPoint = true;
		break;
	case orb:
		name = "Orb";
		numFeatures = 1;
		isKeyPoint = true;
		break;
	case line:
		name = "Hough line detection";
		numFeatures = 1;
		isKeyPoint = true;
		parameters.resize(5);
		parameters[0] = param(t_double, 1, "Rho","",1,100000);
		parameters[1] = param(t_double, 1, "Theta","",1,360);
		parameters[2] = param(t_double, 40, "Intersection threshold","",1,10000);
		parameters[3] = param(t_double, 0, "SRN");
		parameters[4] = param(t_double, 0, "STN");
		break;
	case lineP:
		name = "Probabilistic hough line detection";
		numFeatures = 1;
		isKeyPoint = true;
		parameters.resize(5);
		parameters[0] = param(t_double, 1, "Rho","",1,100000);
		parameters[1] = param(t_double, 1, "Theta","",1,360);
		parameters[2] = param(t_int,	50, "Intersection threshold", "", 1, 10000);
		parameters[3] = param(t_int,	50, "Min line Length");
		parameters[4] = param(t_int,	10, "Max line gap");
		break;
	case circle:
		name = "Hough circle detector";
		numFeatures = 1;
		isKeyPoint = true;
		parameters.resize(6);
		//parameters[0] = param(t_pullDown,)
		parameters[0] = param(t_double, 1, "Inverse ratio of resolution");
		parameters[1] = param(t_double, 60, "Min distance between circles", QStringList(""), "Should be rows / 8");
		parameters[2] = param(t_double, 200, "Upper canny threshold");
		parameters[3] = param(t_double, 100, "Center detection theshold");
		parameters[4] = param(t_double, 0, "Minimum radius");
		parameters[5] = param(t_double, 0, "Maximum radius");
	}
}
void 
featureExtractorContainer::recalculateNumParams()
{
	switch (sType)
	{
		case hist:
		{
			numFeatures = parameters[0].value;
			break;
		}
	}
}
void
featureExtractorContainer::handleParamChange(QString val)
{
	processingContainer::handleParamChange(val);
	emit updated();
}
// ************************************************ featureWindowContainer ****************************
featureWindowContainer::featureWindowContainer(QTreeWidget* parent):
processingContainer(parent)
{
	type = FeatureWindow;
	method = wholeImage;
	initialize();
}
featureWindowContainer::featureWindowContainer(QTreeWidget* parent, statMethod method_, featureExtractorContainer* curExtractor_) :
processingContainer(parent)
{
	type = FeatureWindow;
	method = method_;
	initialize();
}
featureWindowContainer::featureWindowContainer(QTreeWidgetItem* parent, statMethod method_, featureExtractorContainer* curExtractor_) :
processingContainer(parent)
{
	type = FeatureWindow;
	method = method_;
	initialize();
}
featureWindowContainer::featureWindowContainer(statMethod method_, featureExtractorContainer* curExtractor_)
{
	type = FeatureWindow;
	method = method_;
	initialize();
}
featureWindowContainer::featureWindowContainer(statMethod method_)
{
	type = FeatureWindow;
	method = method_;
	initialize();
}
void						
featureWindowContainer::initialize()
{
	curExtractor = NULL;
	curSource = NULL;
	switch (method)
	{
	case wholeImage:
		name = "Whole Image";
		break;
	case ROI:
		name = "Region of Interest";
		break;
	case superPixel:
		name = "Super Pixel";
		break;
	case perPixel:
		name = "Per Pixel";
		break;
	case keyPoint:
		name = "Key point";
		break;
	}
}
featureContainer* 
featureWindowContainer::extractFeatures(imgContainer* src, cv::Mat &displayImg, featureExtractorContainer* curExtractor_)
{
	if (curExtractor_ == NULL) return NULL;
	if (src == NULL) return NULL;
	if (src->M().empty()) return NULL;
	if (curExtractor != NULL) disconnect(curExtractor, SIGNAL(updated()));
	connect(curExtractor_, SIGNAL(updated()), this, SLOT(handleExtractorUpdate()));
	curExtractor = curExtractor_;
	curSource = src;
	dispImg = displayImg;
	//img = cv::Mat(); // Make empty image
	boost::posix_time::ptime start = boost::posix_time::microsec_clock::universal_time();
	featureContainer* output = new featureContainer();
	if (curExtractor_->isKeyPoint && displayImg.empty())
	{
		src->M().copyTo(displayImg);
	}
	if (method == wholeImage)
	{
		// Need to initialize the size of output inside this function so that it can be done once
		output->M() = cv::Mat::zeros(1, curExtractor_->numFeatures * src->M().channels(), CV_32F);
		if (curExtractor_->isKeyPoint)
			if (!curExtractor_->extractKeyPoints(src->M(), output, displayImg)) return NULL;
		else
			if (!curExtractor_->extractFeatures(src->M(), output->M(), displayImg)) return NULL;
	}
	if (method == ROI)
	{
		src->M() = src->M()(cv::Rect(parameters[0].value, parameters[1].value, parameters[2].value, parameters[3].value));
		output->M() = cv::Mat::zeros(1, curExtractor_->numFeatures * src->M().channels(), CV_32F);
		// Replace displayImg with the same ROI from the source image, so that key display is copied back to the right image location
		if (curExtractor_->isKeyPoint)
		if (!curExtractor_->extractKeyPoints(src->M(), output, displayImg)) return NULL;
		else
		if (!curExtractor_->extractFeatures(src->M(), output->M(), displayImg)) return NULL;
	}
	if (method == superPixel)
	{
		int stepsX = (src->M().cols - parameters[0].value) / parameters[2].value;
		int stepsY = (src->M().rows - parameters[1].value) / parameters[3].value;
		// 0 - width, 1 - height, 2 - stepX, 3 - step y
		if (parameters[0].value == -1)
		{
			// Calculate strips along the width of the image
			stepsX = 1;
			parameters[0].value = src->M().cols;
		}
		if (parameters[1].value == -1)
		{
			// Calculate strips along the height of the image
			stepsY = 1;
			parameters[1].value = src->M().rows;
		}
		if (src->M().channels() == 1)
			output->M() = cv::Mat::zeros(stepsY, stepsX*curExtractor_->numFeatures, CV_32F);
		else
			output->M() = cv::Mat::zeros(stepsY, stepsX*curExtractor_->numFeatures, CV_32FC3);
		for (int i = 0; i < stepsY; ++i)
		{
			for (int j = 0; j < stepsX; ++j)
			{
				if (j * parameters[2].value + parameters[0].value > src->M().cols)
					break;
				if (i * parameters[3].value + parameters[1].value > src->M().rows)
					break;
				cv::Mat roi = src->M()(cv::Rect(j * parameters[2].value,
					i * parameters[3].value,
					parameters[0].value,
					parameters[1].value) );
				//curExtractor_->extractFeatures(roi, output->M(cv::Rect(j * curExtractor_->numFeatures, i, curExtractor_->numFeatures, 1)));
			}
		}
	}
	if (method == perPixel)
	{

	}
	if (method == keyPoint)
	{

	}
	boost::posix_time::ptime end = boost::posix_time::microsec_clock::universal_time();
	boost::posix_time::time_duration delta = end - start;
	emit log(curExtractor_->name + " extraction took " + QString::number(delta.total_milliseconds()) + " milliseconds", 1);
	return output;
}
void
featureWindowContainer::handleExtractorUpdate()
{
	featureContainer* f = extractFeatures(curSource, dispImg, curExtractor);
	emit displayImage(dispImg);
	emit extractedFeatures(f);
}
// ************************************************ imgContainer ***************************************
imgContainer::imgContainer(QTreeWidget* parent) :
	matrixContainer(parent)
{
	mask = false;
	type = Img;
}
imgContainer::imgContainer(QTreeWidgetItem* parent) :
	matrixContainer(parent)
{
	type = Img;
	mask = false;
}
imgContainer::imgContainer(imgContainer* cpy)
{
	mask = false;
	type = Img;
	cpy->M().copyTo(_M);
	name = cpy->name;
}
imgContainer::imgContainer(QString absFilePath, QTreeWidget* parent):
	matrixContainer(parent)
{
	type = Img;
	name = "Source";
	QFileInfo file(absFilePath);
	if (file.exists())
	{
		baseName = file.baseName();
		filePath = file.absoluteFilePath();
		dirName = file.absolutePath();
		setText(0, baseName);
		isTop = true;
		container* child = new container;
		child->setText(0, "Directory");
		child->setText(1, dirName);
		addChild(child);
	}	
}
imgContainer::imgContainer(QString absFilePath, imgContainer* parent):
	matrixContainer((QTreeWidgetItem*)parent)
{
	type = Img;
	baseName = parent->baseName;
	QFileInfo file(absFilePath);
	if (file.exists())
	{
		filePath = file.absoluteFilePath();
		dirName = file.absolutePath();
		isTop = false;
	}
	QString tmpName = file.baseName();
	name = tmpName.mid(parent->baseName.size() + 1);
	setText(0, name);
}
imgContainer::~imgContainer()
{

}
bool 
imgContainer::load()
{
	if (!_M.empty()) return true;
	try
	{
		if (filePath.size() > 0)
		{
			if (filePath.endsWith("dcm", Qt::CaseInsensitive))
			{
				_M = readDCM(filePath.toStdString());
				//tmp.convertTo(_M, CV_32F);
			}else
			{
				if (type == container::Img)
					_M = cv::imread(filePath.toStdString());
				if (type == container::Label)
					_M = cv::imread(filePath.toStdString(), 0);
			}
			
		}
		if (_M.empty()) return false; // Still empty, loading failed
	}
	catch (cv::Exception &e)
	{
		std::cout << e.what();
		return false;
	}
	return true;
}
bool
imgContainer::save()
{
	if (_M.empty()) return false;
	//QString dir = dynamic_cast<container*>(orig->parent())->dirName;
	//QString baseName = dynamic_cast<container*>(orig->parent())->baseName;
	QDir dir(dirName);
	if (!QDir(dirName).exists())
	{
		// Processing directory doesn't exist, need to make it.
		QDir().mkdir(dirName);
	}
	cv::imwrite((dirName + "/" + baseName + "_" + text(0) + ".jpg").toStdString(), _M);
	saved = true;
	if (parentContainer != NULL)
		parentContainer->saved = true;
	return true;
}
void
imgContainer::updateDisplay()
{
	this->setText(0, baseName);
}
bool 
imgContainer::loadChildFromDir(QString dirPath)
{
	QDir dir(dirPath);
	QStringList entryList = dir.entryList();
	for (int i = 2; i < entryList.size(); ++i)
	{
		if (entryList[i].startsWith(baseName) && !entryList[i].endsWith("yml"))
		{
			loadChild(dir.dirName(), dirPath + "/" + entryList[i]);
		}
	}
	return true;
}
bool 
imgContainer::loadChild(QString dirName_, QString absFileName_)
{
	if (dirName_ == "labels")
	{
		labelContainer* child = new labelContainer(absFileName_, baseName, this);
	}else
	{
		imgContainer* tmp = new imgContainer(absFileName_, this);
	}
	return true;
}
// ************************************************ streamContainer ***************************************
streamContainer::streamContainer(QTreeWidget *parent):
	imgContainer(parent)
{
	type = Stream;
}
streamContainer::streamContainer(QTreeWidgetItem* parent):
	imgContainer(parent)
{
	type = Stream;
}
streamContainer::~streamContainer()
{
    for(unsigned int i = 0; i < childCount(); ++i)
    {
        removeChild((child(i)));
    }
    _quit = true;
    if(_camThread)
        _camThread->join();
}
void 
streamContainer::openCamera(int deviceID, int resolution, int frameRate)
{
	frameRate = frameRate;
    _camObj.reset(new cv::VideoCapture(deviceID));
	bool success = false;
	success = _camObj->set(CV_CAP_PROP_FOURCC, CV_FOURCC('M', 'J', 'P', 'G'));
    if(resolution == 0)
    {
		success = _camObj->set(CV_CAP_PROP_FRAME_HEIGHT, 1080);
        success = _camObj->set(CV_CAP_PROP_FRAME_WIDTH, 1920);
    }
    if(resolution == 1)
    {
		success = _camObj->set(CV_CAP_PROP_FRAME_HEIGHT, 720);
		success = _camObj->set(CV_CAP_PROP_FRAME_WIDTH, 1280);
    }
    if(resolution == 2)
    {
		success = _camObj->set(CV_CAP_PROP_FRAME_HEIGHT, 480);
		success = _camObj->set(CV_CAP_PROP_FRAME_WIDTH, 640);
    }
	success = _camObj->set(CV_CAP_PROP_FPS, double(frameRate));
	
	_camThread.reset(new boost::thread(boost::bind(&streamContainer::capture, this)));
    _quit = false;
}
void 
streamContainer::capture()
{
    while(!_quit)
    {
        boost::this_thread::sleep(boost::posix_time::milliseconds(1));
        if(_camObj->read(_M))
        {
            if(!_M.empty())
            {
				emit imgUpdated();
                emit imgUpdated(_M);
            }
        }
    }
}
// ************************************************ labelContainer ***************************************
labelContainer::labelContainer(QTreeWidget* parent) :
	imgContainer(parent)
{
	type = Label;
}
labelContainer::labelContainer(QTreeWidgetItem* parent) :
	imgContainer(parent)
{
	type = Label;
}
labelContainer::labelContainer(QString absFilePath, QString parentName, QTreeWidgetItem* parent):
	imgContainer(parent)
{
	baseName = parentName;
	QFileInfo file(absFilePath);
	type = Label;
	name = "Label";
	isTop = false;
	label = -2;
	if (file.exists())
	{
		QString lblName = file.baseName();
		filePath = file.absoluteFilePath(); 
		dirName = file.absolutePath();
		QString txtLabel = lblName.mid(parentName.size() + 1, lblName.size() - parentName.size());
		label = txtLabel.toInt();
		setText(0, txtLabel + " mask");
		setText(1, txtLabel);
		parent->addChild(this);
	}
	QFileInfo polyFile(absFilePath.mid(0, absFilePath.size() - 4) + ".yml");
	if (polyFile.exists())
	{
		cv::FileStorage fs(polyFile.absoluteFilePath().toStdString(), cv::FileStorage::READ);
		int polys = (int)fs["polygons"];
		polygons.reserve(polys);
		for (int i = 0; i < polys; ++i)
		{
			cv::FileNode poly = fs["Polygon-" + QString::number(i).toStdString()];
			cv::FileNodeIterator it = poly.begin();
			cv::FileNodeIterator end = poly.end();
			std::vector<cv::Point> polygon;
			for (; it != end; ++it)
			{
				int x = (int)(*it)["x"];
				int y = (int)(*it)["y"];
				polygon.push_back(cv::Point(x, y));
			}
			polygons.push_back(polygon);
		}
	}
}
labelContainer::~labelContainer()
{

}
bool 
labelContainer::load()
{
	if (!_M.empty()) return true;
	return imgContainer::load();
	
	//_M = cv::imread(filePath.toStdString(), 0);
	//return !_M.empty();
}
bool 
labelContainer::save()
{
	cv::imwrite((dirName + "/" + baseName + "-" + QString::number(label) + ".jpg").toStdString(), _M);
	cv::FileStorage fs((dirName + "/" + baseName + "-" + QString::number(label)+ ".yml").toStdString(), cv::FileStorage::WRITE);
	fs << "polygons" << polygons.size();
	for (int i = 0; i < polygons.size(); ++i)
	{
		fs << ("Polygon-" + QString::number(i).toStdString()) << "[";
		for (int j = 0; j < polygons[i].size(); ++j)
		{
			fs << "{:" << "x" << polygons[i][j].x << "y" << polygons[i][j].y << "}";
		}
		fs << "]";
	}
	fs.release();
	return true;
}
// *********************** featureContainer **************************
featureContainer::featureContainer(QTreeWidget* parent):
	matrixContainer(parent)

{
	type = Features;
}
featureContainer::featureContainer(QTreeWidgetItem* parent) :
	matrixContainer(parent)
{
	type = Features;
}
featureContainer::~featureContainer()
{

}
// *********************** filterContainer **************************
filterContainer::filterContainer(QTreeWidget* parent, filterType type_):
	processingContainer(parent),
	filterParent(NULL),
	filterChild(NULL)
	//output(NULL)
{
	type = Filter;
	fType = type_;
	initialize();
}
filterContainer::filterContainer(QTreeWidgetItem* parent, filterType type_):
	processingContainer(parent),
	filterParent(NULL),
	filterChild(NULL)
	//output(NULL)
{
	type = Filter;
	fType = type_;
	initialize();
}
filterContainer::filterContainer(filterType type_):
	filterParent(NULL),
	filterChild(NULL)
	//output(NULL)
{
	type = Filter;
	fType = type_;
//	output = NULL;
	initialize();
}
filterContainer::filterContainer(cv::FileNode& node):
	filterParent(NULL),
	filterChild(NULL)
	//output(NULL)
{
	type = Filter;
	name = QString::fromStdString((std::string)node["FilterName"]);
	for (int i = 0; i < FILTER_NAMES.size(); ++i)
	{
		if (FILTER_NAMES[i] == name)
		{
			fType = (filterType)i;
			break;
		}
	}
	cached = (int)node["Cached"];
	saved =  (int)node["Saved"];
	saveName = QString::fromStdString((std::string)node["SaveName"]);
	if (saveName.size() == 0) saveName = name;
	setText(0, name);
	initialize();
	numParameters = (int)node["NumParameters"];
	cv::FileNode parameterNode = node["Parameters"];
	if (numParameters > 0)
	{
		for (cv::FileNodeIterator paramItr = parameterNode.begin();
			paramItr != parameterNode.end();
			++paramItr)
		{
			QString paramName = QString::fromStdString((std::string)(*paramItr)["name"]);
			double paramValue = (double)(*paramItr)["value"];
			for (int i = 0; i < parameters.size(); ++i)
			{
				if (paramName == parameters[i].name)
				{
					parameters[i].value = paramValue;
					break;
				}
			}
		}
	}
}
filterContainer::filterContainer(filterContainer* cpy):
	processingContainer(cpy)
	//output(NULL)
{
	// Copy constructor
	type = cpy->type;
	fType = cpy->fType;
	numParameters = cpy->numParameters;
	setText(0, cpy->name);
}
filterContainer::~filterContainer()
{

}
container*
filterContainer::getDisplayCopy()
{
	// Use copy constructor to copy parameters
	filterContainer* out = new filterContainer(this);
	out->makeDisplayCopy();
	//_M.copyTo(out->_M);
	out->_M = _M.clone();
	if (input != NULL)
	{
		out->baseNames << input->name;
		out->input = input;
	}else
	{
		input = NULL;
	}
/*	if (output != NULL)
	{
		out->output = new imgContainer((imgContainer*)output);
	}else
	{
		out->output = NULL;
	}*/

	return out;
}
void
filterContainer::makeDisplayCopy()
{
	isTop = true;
	setText(0, name);
	
	for (unsigned int i = 0; i <parameters.size(); ++i)
	{
		container* child = new container();
		child->isTop = false;
		child->setText(0, parameters[i].name);
		child->setText(1, QString::number(parameters[i].value));
		addChild(child);
	}
}
void
filterContainer::initialize()
{
	input = NULL;
	name = FILTER_NAMES[(int)fType];
	if (fType == threshold)
	{
		parameters.resize(2);
		parameters[0] = param(t_char, 0, "Lower");
		parameters[1] = param(t_char, 255, "Upper");
		numParameters = 2;
	}
	if (fType == erode)
	{
		numParameters = 1;
		parameters.resize(numParameters);
		parameters[0] = param(t_int, 1, "Erosion Size", "", 0, 1000);
	}
	if (fType == dilate)
	{
		numParameters = 1;
		parameters.resize(numParameters);
		parameters[0] = param(t_int, 1, "Dilation Size","",0,1000);
	}
	if (fType == sobel)
	{
		parameters.resize(5);
		parameters[0] = param(t_int, 1, "Scale");
		parameters[1] = param(t_int, 0, "Delta");
		parameters[2] = param(t_int, 3, "Depth");
		parameters[3] = param(t_int, 3, "Kernel Size");
		parameters[4] = param(t_char, 0, "Direction");
		numParameters = 5;
	}
	if (fType == smooth)
	{
		numParameters = 5;
		parameters.resize(numParameters);
		QStringList types;
		types << "Gaussian";
		types << "Blur No Scale";
		types << "Blur";
		types << "Median";
		types << "Bilateral";
		types << "Adaptive Bilateral";
		parameters[0] = param(t_pullDown, 0, "Smooth type", types);
		parameters[1] = param(t_int, 3, "Size 1","", 0,255);
		parameters[2] = param(t_int, 3, "Size 2", "", 0, 255);
		parameters[3] = param(t_double, 0, "Sigma 1","", 0, 100);
		parameters[4] = param(t_double, 0, "Sigma 2","", 0, 100);
	}
	if (fType == gabor)
	{
		numParameters = 6;
		parameters.resize(numParameters);
		parameters[0] = param(t_int, 5, "Kernel Size");
		parameters[1] = param(t_double, 1, "Sigma");
		parameters[2] = param(t_double, 0, "Theta");
		parameters[3] = param(t_double, 0, "Lambda");
		parameters[4] = param(t_double, 0, "Gamma");
		parameters[5] = param(t_double, 3.14159, "Psi");
	}
	if (fType == resize)
	{
		numParameters = 2;
		parameters.resize(2);
		parameters[0] = param(t_int, 640, "Width", "", 1, 10000);
		parameters[1] = param(t_int, 480, "Height", "", 1, 10000);
	}
	if (fType == crop)
	{
		numParameters = 4;
		parameters.resize(numParameters);
		parameters[0] = param(t_int, 0, "Left", "", 0, 100000);
		parameters[1] = param(t_int, 0, "Top", "", 0, 100000);
		parameters[2] = param(t_int, 640, "Width", "", 0, 100000);
		parameters[3] = param(t_int, 480, "Height", "", 0, 100000);
	}
	if (fType == grey)
	{
		numParameters = 0;
		parameters.resize(numParameters);
	}
	if (fType == HSV_hue)
	{
		numParameters = 0;
		parameters.resize(numParameters);
	}
	if (fType == HSV_sat)
	{
		numParameters = 0;
		parameters.resize(numParameters);
	}
	if (fType == HSV_val)
	{
		numParameters = 0;
		parameters.resize(numParameters);
	}
	if (fType == laplacian)
	{
		numParameters = 3;
		parameters.resize(numParameters);
		parameters[0] = param(t_int, 1, "Scale");
		parameters[1] = param(t_int, 0, "Delta");
		parameters[2] = param(t_int, 3, "Kernel Size");
	}
	if (fType == pyrMeanShift)
	{
		numParameters = 3;
		parameters.resize(numParameters);
		parameters[0] = param(t_double, 3, "Spatial Window Radius");
		parameters[1] = param(t_double, 3, "Color Window Radius");
		parameters[2] = param(t_int, 3, "Max level");
	}
	if (fType == scharr)
	{
		numParameters = 4;
		parameters.resize(numParameters);
		parameters[0] = param(t_int, 1, "Dx Order");
		parameters[1] = param(t_int, 0, "Dy Order");
		parameters[2] = param(t_double, 1, "Scale");
		parameters[3] = param(t_double, 0, "Delta");
	}
	if (fType == canny)
	{
		numParameters = 3;
		parameters.resize(numParameters);
		parameters[0] = param(t_int, 50, "Low Threshold");
		parameters[1] = param(t_int, 100, "High Threshold");
		parameters[2] = param(t_int, 3, "Kernel Size");
	}
	if (fType == gradientOrientation)
	{
		parameters.resize(4);
		parameters[0] = param(t_int, 1, "Scale");
		parameters[1] = param(t_int, 0, "Delta");
		parameters[2] = param(t_int, 3, "Kernel Size");
		parameters[3] = param(t_char, 0, "Direction");
		numParameters = 5;
	}
	if (fType == contourFilter)
	{
		parameters.resize(4);
		parameters[0] = param(t_int, 1, "Min pixels","",0,100000);
		parameters[1] = param(t_int, 500, "Max pixels", "", 1, 1000000);
		QStringList mode;
		mode << "External";
		mode << "List";
		mode << "CComp";
		mode << "Tree";
		mode << "Flood Fill";
		parameters[2] = param(t_pullDown, 0, "Retrieval Mode", mode);
		QStringList method;
		method << "Chain code";
		method << "Chain Approx None";
		method << "Chain Approx Simple";
		method << "Chain approx TC89_L1";
		method << "Chain Approx TC89_KCOS";
		method << "Link Runs";
		parameters[3] = param(t_pullDown, 0, "Approximation method", method);
	}
}
void			
filterContainer::writeSettingsXML(cv::FileStorage& fs)
{
	fs << "NumParameters" << (int)parameters.size();
	fs << "FilterName" << name.toStdString();
	fs << "Cached" << cached;
	fs << "Saved" << saved;
	fs << "SaveName" << saveName.toStdString();
	if (parameters.size() > 0)
	{
		fs << "Parameters" << "[";
		for (int i = 0; i < parameters.size(); ++i)
		{
			fs << "{:" << "name" << parameters[i].name.toStdString() << "value" << parameters[i].value << "}";
		}
		fs << "]";
	}
}
void 
filterContainer::update()
{

	emit parameterUpdated();
	//emit processingFinished(process(input));
}
void 
filterContainer::setInput(container* cont_)
{
	input = cont_;
}
imgContainer*
filterContainer::process(container* cont_)
{
	if (cont_ == NULL) return NULL;
	if (cont_->type != container::Img) return NULL;
	input = cont_;
	imgContainer* I = dynamic_cast<imgContainer*>(cont_);
	if (output != NULL && output != cont_) delete output;
	output = new imgContainer;
	imgContainer* output;
	boost::posix_time::ptime start = boost::posix_time::microsec_clock::universal_time();
	switch (fType)
	{
	case threshold:
		output = processThreshold(I->M());
		break;
	case sobel:
		output = processSobel(I->M());
		break;
	case smooth:
		output = processSmooth(I->M());
		break;
	case gabor:
		output = processGabor(I->M());
		break;
	case resize:
		output = processResize(I->M());
		break;
	case crop:
		output = processCrop(I->M());
		break;
	case grey:
		output = processGrey(I->M());
		break;
	case HSV_hue:
		output = processHSV_hue(I->M());
		break;
	case HSV_sat:
		output = processHSV_sat(I->M());
		break;
	case HSV_val:
		output = processHSV_value(I->M());
		break;
	case laplacian:
		output = processLaplacian(I->M());
		break;
	case pyrMeanShift:
		output = processPyrMeanShift(I->M());
		break;
	case scharr:
		output = processScharr(I->M());
		break;
	case canny:
		output = processCanny(I->M());
		break;
	case gradientOrientation:
		output = processGradientOrientation(I->M());
		break;
	case erode:
		output = processErode(I->M());
		break;
	case dilate:
		output = processDilate(I->M());
		break;
	case contourFilter:
		output = processContourFilter(I->M());
	}
	boost::posix_time::ptime end = boost::posix_time::microsec_clock::universal_time();
	boost::posix_time::time_duration delta = end - start;
	emit log(name + " processing took " + QString::number(delta.total_milliseconds()) + " milliseconds", 1);
	if (output == NULL) return NULL;
	_M = static_cast<imgContainer*>(output)->_M;
	return output;
}
imgContainer*
filterContainer::process()
{
	if (input == NULL) return NULL;
	return process(input);
}
imgContainer*		
filterContainer::processThreshold(cv::Mat img)
{
	//imgContainer* output = new imgContainer;
	cv::Mat greater = img > parameters[0].value;
	cv::Mat less = img < parameters[1].value;
	output->_M = greater.mul(less);
	return output;
}

imgContainer*
filterContainer::processErode(cv::Mat img)
{
	//imgContainer* output = new imgContainer;
	cv::Mat tmp;
	cv::Mat erosionElement = cv::getStructuringElement(cv::MORPH_RECT,
								cv::Size(2 * parameters[0].value + 1, 2 * parameters[0].value + 1),
								cv::Point(parameters[0].value, parameters[0].value));
	cv::erode(img, tmp, erosionElement);
	
	output->_M = tmp;
	return output;
}
imgContainer*
filterContainer::processDilate(cv::Mat img)
{
	//imgContainer* output = new imgContainer;
	cv::Mat tmp;
	cv::Mat dilationElement = cv::getStructuringElement(cv::MORPH_RECT,
		cv::Size(2 * parameters[0].value + 1, 2 * parameters[0].value + 1),
		cv::Point(parameters[0].value, parameters[0].value));
	cv::dilate(img, tmp, dilationElement);
	output->_M = tmp;
	return output;
}
imgContainer*
filterContainer::processSobel(cv::Mat img)
{
	//imgContainer* output = new imgContainer;
	cv::Mat grad_x, grad_y;
	if (parameters[4].value == 0 || parameters[4].value == 2)
	{
		try
		{
			cv::Sobel(img, grad_x, CV_32F, 1, 0, parameters[3].value, parameters[0].value, parameters[1].value, cv::BORDER_DEFAULT);
		}
		catch (std::exception &e)
		{
			std::cout << e.what() << std::endl;
			return output;
		}
	}
	if (parameters[4].value == 1 || parameters[4].value == 2)
	{
		try
		{
			cv::Sobel(img, grad_y, CV_32F, 0, 1, parameters[3].value, parameters[0].value, parameters[1].value, cv::BORDER_DEFAULT);
		}
		catch (std::exception &e)
		{
			std::cout << e.what() << std::endl;
			return output;
		}
	}
	if (parameters[4].value == 0)
	{
		((imgContainer*)output)->_M = grad_x;
	}
	if (parameters[4].value == 1)
	{
		((imgContainer*)output)->_M = grad_y;
	}
	if (parameters[4].value == 2)
	{
		cv::addWeighted(grad_x, 0.5, grad_y, 0.5, 0, ((imgContainer*)output)->_M);
	}
	return output;
}
imgContainer*
filterContainer::processSmooth(cv::Mat img)
{
	//imgContainer* output = new imgContainer;
	if (!((int)parameters[1].value % 2)) return output;
	if (!((int)parameters[2].value % 2)) return output;
	if (parameters[0].value == 0)
	{
		try
		{
			cv::GaussianBlur(img, ((imgContainer*)output)->_M, cv::Size(parameters[1].value, parameters[2].value), parameters[3].value, parameters[4].value);
		}
		catch (cv::Exception &e)
		{
			std::cout << e.what() << std::endl;
		}
	}
	if (parameters[0].value == 1)
	{
		// Blur no scale
		try
		{
			cv::blur(img, ((imgContainer*)output)->_M, cv::Size(parameters[1].value, parameters[2].value));
		}
		catch (cv::Exception &e)
		{
			std::cout << e.what();
		}
	}
	if (parameters[0].value == 2)
	{
		// Blur
		try
		{
			cv::blur(img, ((imgContainer*)output)->_M, cv::Size(parameters[1].value, parameters[2].value));
		}
		catch (cv::Exception &e)
		{
			std::cout << e.what();
		}
	}
	if (parameters[0].value == 3)
	{
		// Median
		try
		{
			cv::medianBlur(img, ((imgContainer*)output)->_M, parameters[1].value);
		}
		catch (cv::Exception &e)
		{
			std::cout << e.what();
		}

	}
	if (parameters[0].value == 4)
	{
		// Bilateral
		try
		{
			cv::bilateralFilter(img, ((imgContainer*)output)->_M, parameters[1].value, parameters[3].value, parameters[4].value);
		}
		catch (cv::Exception &e)
		{
			std::cout << e.what();
		}
	}
	if (parameters[0].value == 5)
	{
		try
		{
			//cv::adaptiveBilateralFilter(img,output,cv::Size(parameters[1].value,parameters[2].value),parameters[3].value);
		}
		catch (cv::Exception &e)
		{
			std::cout << e.what();
		}
	}
	return output;
}
imgContainer*
filterContainer::processGabor(cv::Mat img)
{
	//imgContainer* output = new imgContainer;
	cv::Mat filter = cv::getGaborKernel(cv::Size(parameters[0].value, parameters[0].value),
		parameters[1].value, parameters[2].value, parameters[3].value, parameters[4].value, parameters[5].value);
	cv::Mat grey = img;
	if (img.type() == CV_8UC3)
		cv::cvtColor(grey, grey, CV_BGR2GRAY);
	cv::filter2D(grey, ((imgContainer*)output)->_M, CV_32F, filter);
	cv::convertScaleAbs(((imgContainer*)output)->_M, ((imgContainer*)output)->_M);
	((imgContainer*)output)->_M.convertTo(((imgContainer*)output)->_M, CV_8U, 255);
	return output;
}
imgContainer*
filterContainer::processResize(cv::Mat img)
{
	//imgContainer* output = new imgContainer;
	cv::resize(img, ((imgContainer*)output)->_M, cv::Size(parameters[0].value, parameters[1].value));
	return output;
}
imgContainer*
filterContainer::processCrop(cv::Mat img)
{
	//imgContainer* output = new imgContainer;
	cv::Rect ROI(parameters[0].value, parameters[1].value,
		parameters[2].value, parameters[3].value);
	try
	{
		((imgContainer*)output)->_M = img(ROI);
	}
	catch (cv::Exception &e)
	{
		std::cout << e.what() << std::endl;
	}
	emit log("Cropped image to " + QString::number(ROI.height) + " " + QString::number(ROI.width), 0);
	return output;
}
imgContainer*
filterContainer::processGrey(cv::Mat img)
{
	//imgContainer* output = new imgContainer;
	if (img.channels() == 1)
	{
		std::cout << "Img already grey!";
		((imgContainer*)output)->_M = img;
	}
	else
	{
		cv::cvtColor(img, ((imgContainer*)output)->_M, CV_RGB2GRAY);
		//std::cout << output->M().type() << std::endl;
	}
	return output;
}
imgContainer*
filterContainer::processHSV_hue(cv::Mat img)
{
	//imgContainer* output = new imgContainer;
	if (img.channels() == 1)
	{
		std::cout << "Cannot convert single channel image";
		return NULL;
	}
	else
	{
		cv::Mat tmp;
		imgContainer* output = new imgContainer;
		cv::cvtColor(img, tmp, CV_BGR2HSV);
		std::vector<cv::Mat> planes;
		cv::split(tmp, planes);
		output->_M = planes[0];
		return output;
	}
}
imgContainer*
filterContainer::processHSV_sat(cv::Mat img)
{
	//imgContainer* output = new imgContainer;
	if (img.channels() == 1)
	{
		std::cout << "Cannot convert single channel image";
		return NULL;
	}
	else
	{
		imgContainer* output = new imgContainer;
		cv::Mat tmp;
		cv::cvtColor(img, tmp, CV_BGR2HSV);
		std::vector<cv::Mat> planes;
		cv::split(tmp, planes);
		output->_M = planes[1];
		return output;
	}
}
imgContainer*
filterContainer::processHSV_value(cv::Mat img)
{
	//imgContainer* output = new imgContainer;
	if (img.channels() == 1)
	{
		std::cout << "Cannot convert single channel image";
		return NULL;
	}
	else
	{
		imgContainer* output = new imgContainer;
		cv::Mat tmp;
		cv::cvtColor(img, tmp, CV_BGR2HSV);
		std::vector<cv::Mat> planes;
		cv::split(tmp, planes);
		output->_M = planes[2];
		return output;
	}
}

imgContainer*
filterContainer::processLaplacian(cv::Mat img)
{
	//imgContainer* output = new imgContainer;
	try
	{
		cv::Laplacian(img, ((imgContainer*)output)->_M, CV_16S, parameters[2].value, parameters[0].value, parameters[1].value);
		cv::convertScaleAbs(((imgContainer*)output)->_M, ((imgContainer*)output)->_M);
	}
	catch (cv::Exception &e)
	{
		std::cout << e.what() << std::endl;
	}
	return output;
}
imgContainer*
filterContainer::processPyrMeanShift(cv::Mat img)
{
	//imgContainer* output = new imgContainer;
	cv::pyrMeanShiftFiltering(img, ((imgContainer*)output)->_M, parameters[0].value, parameters[1].value, parameters[2].value);
	return output;
}
imgContainer*
filterContainer::processScharr(cv::Mat img)
{
	//imgContainer* output = new imgContainer;
	try
	{
		cv::Scharr(img, ((imgContainer*)output)->_M, CV_32S, parameters[0].value, parameters[1].value, parameters[2].value, parameters[3].value);
	}
	catch (cv::Exception &e)
	{
		std::cout << e.what() << std::endl;
	}
	
	return output;
}
imgContainer*
filterContainer::processCanny(cv::Mat img)
{
	//imgContainer* output = new imgContainer;
	if (((int)parameters[2].value) % 2)
	{
		if (img.channels() != 1)
		{
			std::cout << "Can only perform canny on a single channel" << std::endl;
			cv::cvtColor(img, img, CV_RGB2GRAY);
		}
		try
		{
			cv::Canny(img, ((imgContainer*)output)->_M, parameters[0].value, parameters[1].value, parameters[2].value);
		}
		catch (cv::Exception &e)
		{
			std::cout << e.what() << std::endl;
		}
	}
	return output;
}
imgContainer*
filterContainer::processGradientOrientation(cv::Mat img)
{
	//imgContainer* output = new imgContainer;
	if (img.channels() == 3)
	{
		cv::cvtColor(img, img, CV_BGR2GRAY);
	}
	cv::Mat gradX, gradY;
	cv::Sobel(img, gradX, CV_32F, 1, 0, parameters[2].value, parameters[0].value, parameters[1].value, cv::BORDER_DEFAULT);
	cv::Sobel(img, gradY, CV_32F, 0, 1, parameters[2].value, parameters[0].value, parameters[1].value, cv::BORDER_DEFAULT);
	((imgContainer*)output)->_M = cv::Mat::zeros(img.rows, img.cols, CV_32F);
	if (procThreads.size() != numThreads)
		procThreads.resize(numThreads);
	for (int i = 0; i < numThreads; ++i)
	{
		procThreads[i].reset(new boost::thread(boost::bind(&filterContainer::gradientOrientationHelper, this, gradX, gradY, output->_M, i)));
	}
	for (int i = 0; i < numThreads; ++i)
	{
		procThreads[i]->join();
	}
	return output;
}
imgContainer*
filterContainer::processContourFilter(cv::Mat img)
{
	//imgContainer* output = new imgContainer;
	if (img.channels() != 1)
	{
		log("Need to pass single channel image into contour filter", 2);
		return NULL;
	}
	
	std::vector<std::vector<cv::Point>> contours;
	std::vector<cv::Vec4i> hierarchy;
	//cv::findContours(img, contours, hierarchy, CV_RETR_CCOMP, CV_CHAIN_APPROX_NONE);
	try
	{
		cv::findContours(img, contours, hierarchy, (int)parameters[2].value, (int)parameters[3].value);
	}
	catch (cv::Exception &e)
	{
		std::cout << e.what() << std::endl;
		log("Finding contours failed due to: \n " + QString::fromLocal8Bit(e.what()), 2);
		return NULL;
	}
	
	emit log("Found " + QString::number(contours.size()) + " individual segments", 1);
	for (int i = 0; i < contours.size(); ++i)
	{
		emit log("Contour " + QString::number(i) + " contains " + QString::number(contours[i].size()) + " pixels", 1);
	}
	cv::Mat tmp = cv::Mat::zeros(img.size(), CV_8U);
	for (int i = 0; i < contours.size(); ++i)
	{
		for (int j = 0; j < contours[i].size(); ++j)
		{
			tmp.at<uchar>(contours[i][j]) = i;
		}
	}
	for (int i = 0; i < contours.size(); ++i)
	{
		if (contours[i].size() < parameters[0].value || contours[i].size() > parameters[1].value)
		{
			// Contour does not pass filter, remove from mask
			for (int j = 0; j < contours[i].size(); ++j)
			{
				try
				{
					img.at<uchar>(contours[i][j]) = 0;
				}
				catch (cv::Exception &e)
				{
					continue;
				}
			}
		}
	}
	output->_M = img.clone();
	return output;
}
void 
filterContainer::gradientOrientationHelper(cv::Mat gradX, cv::Mat gradY, cv::Mat orientation, int threadNum)
{
	for (int i = threadNum; i < orientation.rows * orientation.cols; i += numThreads)
	{
		((float*)orientation.data)[i] = atan2(((float*)gradY.data)[i], ((float*)gradX.data)[i]);
	}
}
// *********************** compoundFilterContainer **************************
compoundFilterContainer::compoundFilterContainer(QTreeWidget* parent):
	filterContainer(parent)
{
	type = CompoundFilter;
}
compoundFilterContainer::compoundFilterContainer(QTreeWidgetItem* parent) :
filterContainer(parent)
{
	type = CompoundFilter;
}
compoundFilterContainer::~compoundFilterContainer()
{

}
// *********************** statContainer **************************
statContainer::statContainer(QTreeWidget* parent):
	processingContainer(parent)
{
	type = Stat;
}
statContainer::statContainer(QTreeWidgetItem* parent) :
	processingContainer(parent)
{
	type = Stat;
}
statContainer::~statContainer()
{

}
// *********************** mlContainer **************************
mlContainer::mlContainer(QTreeWidget* parent):
	processingContainer(parent)
{
	type = ML;
}
mlContainer::mlContainer(QTreeWidgetItem* parent) :
	processingContainer(parent)
{
	type = ML;
}
mlContainer::~mlContainer()
{

}
container* 
mlContainer::process(cv::InputArray input_)
{
	featureContainer* out = new featureContainer;
	return out;
}

// ******************************************************** filterMacro ****************************************************************
filterMacro::filterMacro()
{
	type = FilterMacro;
}
filterMacro::filterMacro(QString fileName)
{
	type = FilterMacro;
	load(fileName);
}
bool 
filterMacro::load(QString fileName)
{
	cv::FileStorage fs(fileName.toStdString(), cv::FileStorage::READ);
	name = QString::fromStdString((std::string)fs["MacroName"]);
	numFilters = (int)fs["NumFilters"];
	for (int i = 0; i < numFilters; ++i)
	{
		cv::FileNode filterNode = fs["Filter-" + QString::number(i).toStdString()];
		filterContainer* tmp = new filterContainer(filterNode);
		tmp->makeDisplayCopy();
		filters.push_back(tmp);
	}
	return true;
}
bool 
filterMacro::save()
{
	return save(name + "_macro.xml");
}
bool
filterMacro::save(QString fileName)
{
	cv::FileStorage fs(fileName.toStdString() , cv::FileStorage::WRITE);
	fs << "MacroName" << name.toStdString();
	fs << "NumFilters" << (int)filters.size();
	//fs << "Filters" << "[";
	for (int i = 0; i < filters.size(); ++i)
	{
		fs << "Filter-" + QString::number(i).toStdString() << "{:";
		filters[i]->writeSettingsXML(fs);
		fs << "}";
	}
	return true;
}