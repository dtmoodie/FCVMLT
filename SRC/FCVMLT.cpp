#include "FCVMLT.h"


container::container(QTreeWidget *parent) :
	QTreeWidgetItem(parent),
	isData(false),
	isTop(false),
	saved(false),
	cached(false)
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
	childContainers.clear();
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
containerPtr
container::getChild(QString name)
{
	for (int i = 0; i < childContainers.size(); ++i)
	{
		if (childContainers[i]->name == name)
		{
			return childContainers[i];
		}
		containerPtr child = childContainers[i]->getChild(name);
		if (child != NULL) return child;
	}
	return containerPtr();
}
containerPtr
container::getChild(container* ptr)
{
	for (int i = 0; i < childContainers.size(); ++i)
	{
		if (childContainers[i].get() == ptr)
		{
			return childContainers[i];
		}
		containerPtr child = childContainers[i]->getChild(ptr);
		if (child != NULL) return child;
	}
	return containerPtr();
}
container*
container::getParent()
{
	if (isTop) return this;
	return parentContainer;
}
bool
container::deleteChild(QString name)
{
	for (int i = 0; i < childContainers.size(); ++i)
	{
		if (childContainers[i]->name == name)
		{
			childContainers.removeAt(i);
			return true;
		}
		if (childContainers[i]->deleteChild(name)) return true;
	}
	return false;
}
bool
container::deleteChild(container* ptr)
{
	for (int i = 0; i < childContainers.size(); ++i)
	{
		if (childContainers[i].get() == ptr)
		{
			childContainers.removeAt(i);
			return true;
		}
		if (childContainers[i]->deleteChild(ptr))
		{
			return true;
		}
	}
	return false;
}

// ************************************************ referenceContainer ************************************
referenceContainer::referenceContainer(QTreeWidget* parent):
container(parent),
scale(1.0)
{
	H = cv::Mat::eye(2, 3, CV_32F);
}
referenceContainer::referenceContainer(QTreeWidgetItem* parent) :
container(parent),
scale(1.0)
{
	name = "Reference";
	setText(0, "Reference");
	baseName = dynamic_cast<container*>(parent)->baseName;
	dirName = dynamic_cast<container*>(parent)->dirName + "/Reference";
	H = cv::Mat::eye(2, 3, CV_32F);
}
referenceContainer::referenceContainer(containerPtr parent)
{

}
referenceContainer::referenceContainer(imgPtr parent)
{
	parent->addChild(this);
	this->setText(0, "Reference");
	this->name = "Reference";
	this->baseName = parent->baseName;
	this->dirName = parent->dirName + "/Reference";
}
referenceContainer::referenceContainer(referenceContainer* cpy) :
container(cpy),
scale(1.0)
{
	H = cv::Mat::eye(2, 3, CV_32F);
}
referenceContainer::referenceContainer(QString absFilePath, QTreeWidget* parent) :
scale(1.0)
{
	H = cv::Mat::eye(2, 3, CV_32F);
}
referenceContainer::referenceContainer(QString absFilePath, imgContainer* parent) :
scale(1.0)
{
	H = cv::Mat::eye(2, 3, CV_32F);
}

referenceContainer::~referenceContainer()
{

}
void
referenceContainer::setOrigin(cv::Point2f pt)
{
	H.at<float>(0, 2) = pt.x;
	H.at<float>(1, 2) = pt.y;
}
void
referenceContainer::setRotation(float theta)
{

}
void
referenceContainer::setRotation(cv::Vec2f x_axis)
{

}
void
referenceContainer::setScale(cv::Vec2f UVdist, float realDist)
{

}
void 
referenceContainer::setScale(float UVdist, float realDist)
{
	scale = realDist / UVdist;
	setText(1, "Scale: " + QString::number(scale));
}
void
referenceContainer::setTransform(cv::Point2f pt, float theta)
{
	setOrigin(pt);
}
void
referenceContainer::setTransform(cv::Point2f pt, cv::Vec2f x_axis)
{
	setOrigin(pt);
}
cv::Mat
referenceContainer::transformPoints(cv::Mat pts)
{
	if (H.type() != CV_32F) H.convertTo(H, CV_32F);
	if (pts.type() != CV_32F) pts.convertTo(pts, CV_32F);
	if (pts.channels() != 1)
	{
		//cv::convertPointsToHomogeneous(pts, pts);
	}try
	{
		
		cv::Mat out = H*pts;
		return out*scale;
	}catch (cv::Exception &e)
	{
		std::cout << e.what() << std::endl;
		return pts;
	}
}
cv::Mat
referenceContainer::transformPoints(std::vector<cv::Point2f> pts)
{
	return cv::Mat(pts);
}
cv::Mat 
referenceContainer::pointsFromMask(cv::Mat mask)
{
	if (mask.type() != CV_8U) return cv::Mat();
	int numPts = cv::countNonZero(mask);
	cv::Mat pts = cv::Mat::ones(3, numPts, CV_32F);
	int itr = 0;
	for (int y = 0; y < mask.rows; ++y)
	{
		for (int x = 0; x < mask.cols; ++x)
		{
			if (mask.at<uchar>(y, x) != 0)
			{
				pts.at<float>(0, itr) = x;
				pts.at<float>(1, itr) = y;
				++itr;
			}
		}
	}
	return transformPoints(pts);;
}
cv::Mat
referenceContainer::M()
{
	// Get the image from the parent container and then draw the reference on it
	//if (parentContainer->type != Img) return cv::Mat();
	cv::Mat src = dynamic_cast<imgContainer*>(parentContainer)->M();
	// For now just drawing the origin point
	cv::Point2f origin(H.at<float>(0, 2), H.at<float>(1, 2));
	cv::Mat output = src.clone();
	cv::circle(output, origin, 50, cv::Scalar(0, 255, 0), 5);
	cv::circle(output, origin, 4, cv::Scalar(0, 255, 0), 4);
	return output;
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
containerPtr
processingContainer::getDisplayCopy()
{
	containerPtr out(new processingContainer(this));
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
			QSpinBox* box = new QSpinBox(out);
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
			QDoubleSpinBox* box = new QDoubleSpinBox(out);
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
			QDoubleSpinBox* box = new QDoubleSpinBox(out);
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
			QSpinBox* box = new QSpinBox(out);
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
			QComboBox* box = new QComboBox(out);
			box->setObjectName(parameters[i].name);
			box->setToolTip(parameters[i].toolTip);
			layout->addWidget(box, i, 1);
			box->addItems(parameters[i].pullDownItems);
			parameters[i].box = box;
			connect(box, SIGNAL(currentIndexChanged(QString)), this, SLOT(handleParamChange(QString)));
		}
		if (parameters[i].type == t_bool)
		{
			QCheckBox* box = new QCheckBox(out);
			box->setObjectName(parameters[i].name);
			box->setToolTip(parameters[i].toolTip);
			layout->addWidget(box, i, 1);
			box->setChecked(parameters[i].value == 1);
			parameters[i].box = box;
			connect(box, SIGNAL(stateChanged(int)), this, SLOT(handleParamChange(int)));
		}
		if (parameters[i].type == t_path)
		{
			QLineEdit* box = new QLineEdit(out);
			box->setObjectName(parameters[i].name);
			box->setToolTip(parameters[i].toolTip);
			layout->addWidget(box, i, 1);
			parameters[i].box = box;
			box->setText(parameters[i].pathText);
			connect(box, SIGNAL(textEdited(QString)), this, SLOT(handleParamChange(QString)));

			QPushButton* btn = new QPushButton(out);
			btn->setText("Select file");
			layout->addWidget(btn, i, 2);
			parameters[i].btn = btn;
			connect(btn, SIGNAL(clicked()), parent, SLOT(handlePathSelect()));
		}
	}
	out->setLayout(layout);
	out->setWindowTitle(name + " Settings");
	return out;
}
void
processingContainer::handlePathSelect()
{
	for (unsigned int i = 0; i < parameters.size(); ++i)
	{
		if (sender() == (QObject*)parameters[i].btn)
		{
			//parameters[i].pathText = QFileDialog::getOpenFileName(this, "Select file");
		}
	}
}
void 
processingContainer::handleParamChange(QString val)
{
	for (unsigned int i = 0; i < parameters.size(); ++i)
	{
		if (sender() == (QObject*)parameters[i].box)
		{
			if (parameters[i].type == t_path)
			{
				parameters[i].pathText = val;
				emit log(name + ": " + parameters[i].name + " updated to " + val, 0);
				update();
			}
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
processingContainer::handleParamChange(int val)
{
	for (unsigned int i = 0; i < parameters.size(); ++i)
	{
		if (sender() == (QObject*)parameters[i].box)
		{
			if (val == 0)
				parameters[i].value = 0;
			else
				parameters[i].value = 1;
			update();
			emit log(name + ": " + parameters[i].name + " updated to " + QString::number(parameters[i].value), 0);
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
processingContainer::setInput(containerPtr cont_)
{
	//input(input_);
}
containerPtr
processingContainer::process(containerPtr cont_)
{
	//input(input_);
	//emit processingFinished(output);;
	return containerPtr();
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
featureExtractorContainer::featureExtractorContainer(featureExtractorContainer* cpy)
{
	sType = cpy->sType;
	numFeatures = cpy->numFeatures;
	numThreads = cpy->numThreads;
	visualizableResult = cpy->visualizableResult;
	parameters = cpy->parameters;
	name = cpy->name;
	saveName = cpy->saveName;

}
featureExtractorContainer::~featureExtractorContainer()
{

}
bool 
featureExtractorContainer::extractFeatures(cv::Mat src, cv::Mat& features, cv::Mat* dispImg)
{
	switch (sType)
	{
	case sum:
	{
		cv::Scalar output = cv::sum(src);
		for (int i = 0; i < src.channels(); ++i)
		{
			((float*)features.data)[i] = (float)output[i];
		}
		break;
	}
	case avg:
	{
		cv::Scalar out = cv::mean(src);
		for (int i = 0; i < src.channels(); ++i)
		{
			((float*)features.data)[i] = (float)out[i];
		}
		break;
	}
	case copy:
	{

		cv::Mat out; //src.reshape(src.channels(), 1);
		src.copyTo(out);
		out = out.reshape(src.channels(), 1);
		if(src.channels() == 1) out.convertTo(features, CV_32F);
		if (src.channels() == 3) out.convertTo(features, CV_32FC3);
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
featureExtractorContainer::extractKeyPoints(cv::Mat src, featurePtr features, cv::Mat* dispImg)
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
		if (dispImg != NULL)
		cv::drawKeypoints(*dispImg, keyPts, *dispImg, cv::Scalar(255, 0, 0));
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
		if (dispImg != NULL)
		{
			if (src.size != dispImg->size)
			{
				scaleX = dispImg->cols / src.cols;
				scaleY = dispImg->rows / src.rows;
			}
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
			if (dispImg != NULL)
				cv::line(*dispImg, pt1, pt2, cv::Scalar(0, 0, 255), 3, CV_AA);
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
			if (dispImg != NULL)
				cv::line(*dispImg, p1, p2, cv::Scalar(0, 0, 255), 3, CV_AA);
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
			if (dispImg != NULL)
			{
				// circle center
				cv::circle(*dispImg, center, 3, cv::Scalar(0, 255, 0), -1, 8, 0);
				// circle outline
				cv::circle(*dispImg, center, radius, cv::Scalar(0, 0, 255), 3, 8, 0);
			}
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
	visualizableResult = false;
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
	case copy:
		numFeatures = 1;
		name = "Copy";
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
		visualizableResult = true;
		break;
	case HoG:
		name = "Histogram of Gradients";
		numFeatures = 1;
		visualizableResult = true;
		break;
	case orb:
		name = "Orb";
		numFeatures = 1;
		visualizableResult = true;
		break;
	case surf:
		name = "SURF-64";
		visualizableResult = true;
		numFeatures = 64;
	case line:
		name = "Hough line detection";
		numFeatures = 1;
		visualizableResult = true;
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
		visualizableResult = true;
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
		visualizableResult = true;
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
featureWindowContainer::featureWindowContainer(QTreeWidget* parent, statMethod method_, featureExtractorPtr curExtractor_) :
processingContainer(parent)
{
	curExtractor = curExtractor_;
	type = FeatureWindow;
	method = method_;
	initialize();
}
featureWindowContainer::featureWindowContainer(QTreeWidgetItem* parent, statMethod method_, featureExtractorPtr curExtractor_) :
processingContainer(parent)
{
	curExtractor = curExtractor_;
	type = FeatureWindow;
	method = method_;
	initialize();
}
featureWindowContainer::featureWindowContainer(statMethod method_, featureExtractorPtr curExtractor_)
{
	curExtractor = curExtractor_;
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
featureWindowContainer::featureWindowContainer(featureWindowContainer* cpy)
{
	type = FeatureWindow;
	method = cpy->method;
	parameters = cpy->parameters;
}
void						
featureWindowContainer::initialize()
{
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
		parameters.resize(4);
		parameters[0] = param(t_int, 1, "Width");
		parameters[1] = param(t_int, 1, "Height");
		parameters[2] = param(t_int, 1, "Step X");
		parameters[3] = param(t_int, 1, "Step Y");
		break;
	case perPixel:
		name = "Per Pixel";
		break;
	case keyPoint:
		name = "Key point";
		break;
	}
}
// Workhorse of this object.
// Based on the extraction method, this function extracts regions of interest and passes those regions to the feature extractor
// it also attemtps to initialize the feature output matrix so that it is only initialized once, then passes the output and an index
// to the feature extractor.
featurePtr
featureWindowContainer::extractFeatures()
{
	if (curExtractor == NULL) featurePtr();
	if (curSource == NULL)featurePtr();
	if (curSource->M().empty()) featurePtr();
	if (curExtractor != NULL) disconnect(curExtractor.get(), SIGNAL(updated()));
	connect(curExtractor.get(), SIGNAL(updated()), this, SLOT(handleExtractorUpdate()));

	boost::posix_time::ptime start = boost::posix_time::microsec_clock::universal_time();
	featurePtr output(new featureContainer());
	output->dirName = curSource->dirName;
	output->baseName = curSource->baseName;
	cv::Mat* disp = NULL;
	if (curExtractor->visualizableResult)
	{
		if (dispImg.empty())
			curSource->M().copyTo(dispImg);
		disp = &dispImg;
	}
	if (method == wholeImage)
	{
		output->M() = cv::Mat::zeros(1, curExtractor->numFeatures * curSource->M().channels(), CV_32F);
		if (curExtractor->visualizableResult)
		{
			// Pass display image in so that it can be updated by the extractor
			if (!curExtractor->extractKeyPoints(curSource->M(), output, disp))
			{
				return featurePtr();
			}
			else
			{
				if (!curExtractor->extractFeatures(curSource->M(), output->M(), disp))
				{
					return featurePtr();
				}
			}
		}else
		{
			if (!curExtractor->extractFeatures(curSource->M(), output->M()))
			{
				return featurePtr();
			}
		}
	}
	
	if (method == ROI)
	{
		cv::Mat M = curSource->M()(cv::Rect(parameters[0].value, parameters[1].value, parameters[2].value, parameters[3].value));
		output->_M = cv::Mat::zeros(1, curExtractor->numFeatures * curSource->M().channels(), CV_32F);
		// Replace displayImg with the same ROI from the source image, so that key display is copied back to the right image location
		if (curExtractor->visualizableResult)
		{
			if (!curExtractor->extractKeyPoints(M, output, disp))
			{
				return featurePtr();
			}
			else
			{
				if (!curExtractor->extractFeatures(M, output->M(), disp))
				{
					return featurePtr();
				}
			}
		}
	}
	
	
	if (method == superPixel)
	{
		int stepsX = (curSource->M().cols - parameters[0].value) / parameters[2].value;
		int stepsY = (curSource->M().rows - parameters[1].value) / parameters[3].value;
		// 0 - width, 1 - height, 2 - stepX, 3 - step y
		if (parameters[0].value == -1)
		{
			// Calculate strips along the width of the image
			stepsX = 1;
			parameters[0].value = curSource->M().cols;
		}
		if (parameters[1].value == -1)
		{
			// Calculate strips along the height of the image
			stepsY = 1;
			parameters[1].value = curSource->M().rows;
		}
		int rows = stepsY*stepsX;
		bool useMask = false;
		if (mask.size() == curSource->M().size())
		{
			rows = cv::countNonZero(mask);
			useMask = true;
		}
		int numFeatures = curExtractor->numFeatures;
		if (curExtractor->sType == copy) numFeatures = parameters[0].value * parameters[1].value;
		if (curSource->M().channels() == 1)
		{
			output->_M = cv::Mat::zeros(rows, numFeatures, CV_32F);
		}
		else
		{
			output->_M = cv::Mat::zeros(rows, numFeatures, CV_32FC3);
			//dispImg = cv::Mat::zeros(stepsY, stepsX, CV_32FC3);
		}
		unsigned int featureRowCount = 0;
		for (int i = 0; i < stepsY; ++i)
		{
			for (int j = 0; j < stepsX; ++j)
			{
				if (useMask)
				{
					if (mask.at<uchar>(i, j) == 0) continue;
				}
				if (j * parameters[2].value + parameters[0].value > curSource->M().cols)
					break;
				if (i * parameters[3].value + parameters[1].value > curSource->M().rows)
					break;
				// top left X, Y, width, height
				cv::Mat roi = curSource->M()(cv::Rect(j*parameters[2].value - (int)parameters[0].value / 2,
													  i*parameters[3].value - (int)parameters[1].value / 2,
													  parameters[0].value,
													  parameters[1].value));

				//cv::Mat roi = curSource->M()(cv::Rect(j * parameters[2].value,
				//							i * parameters[3].value,
				//							parameters[0].value,
				//							parameters[1].value));
                cv::Mat tmp = output->_M.row(featureRowCount);
                curExtractor->extractFeatures(roi, tmp);
				++featureRowCount;
			}
		}
		if (useMask == false)
		{
			dispImg = output->_M.reshape(curSource->M().channels(), stepsY);
			emit displayImage(dispImg);
		}
	}
	/*
	if (method == perPixel)
	{

	}
	if (method == keyPoint)
	{

	}
	if (method == mask)
	{

	}
	*/
	boost::posix_time::ptime end = boost::posix_time::microsec_clock::universal_time();
	boost::posix_time::time_duration delta = end - start;
	emit log(curExtractor->name + " extraction took " + QString::number(delta.total_milliseconds()) + " milliseconds", 1);
	return output;
}
featurePtr
featureWindowContainer::extractFeatures(imgPtr src)
{
	curSource = src;
	return extractFeatures();
}
featurePtr
featureWindowContainer::extractFeatures(imgPtr src, cv::Mat mask_)
{
	curSource = src;
	mask = mask_;
	return extractFeatures();
}
featurePtr
featureWindowContainer::extractFeatures(imgPtr src, cv::Mat mask_, cv::Mat& displayImg)
{
	mask = mask_;
	curSource = src;
	dispImg = displayImg;
	return extractFeatures();
}
featurePtr
featureWindowContainer::extractFeatures(imgPtr src, cv::Mat mask_, cv::Mat& displayImg, featureExtractorPtr curExtractor_)
{
	mask = mask_;
	curSource = src;
	dispImg = displayImg;
	curExtractor = curExtractor_;
	return extractFeatures();
}
void
featureWindowContainer::setSrc(imgPtr src)
{
	curSource = src;
}
void
featureWindowContainer::setMask(cv::Mat mask_)
{
	mask = mask_;
}
void
featureWindowContainer::setExtractor(featureExtractorPtr curExtractor_)
{
	curExtractor = curExtractor_;
}
void
featureWindowContainer::setDisplayImage(imgPtr img)
{
	dispImg = img->M();
}
void
featureWindowContainer::handleExtractorUpdate()
{
	//featureContainer* f = extractFeatures(curSource, dispImg, curExtractor);
	//emit displayImage(dispImg);
	//emit extractedFeatures(f);
}
// ************************************************ imgContainer ***************************************
imgContainer::imgContainer(QTreeWidget* parent) :
	matrixContainer(parent)
{
	parentContainer = NULL;
	mask = false;
	type = Img;
}
imgContainer::imgContainer(QTreeWidgetItem* parent) :
	matrixContainer(parent)
{
	parentContainer = NULL;
	type = Img;
	mask = false;
}
imgContainer::imgContainer(imgContainer* cpy)
{
	parentContainer = NULL;
	mask = false;
	type = Img;
	cpy->M().copyTo(_M);
	name = cpy->name;
}
imgContainer::imgContainer(QString absFilePath, QTreeWidget* parent):
	matrixContainer(parent)
{
	parentContainer = NULL;
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
	parentContainer = parent;
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
	childContainers.clear();
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
#ifdef DICOM_LOADER_FOUND
				_M = readDCM(filePath.toStdString());
#else
#endif
				//tmp.convertTo(_M, CV_32F);
			}else
			{
				if (type == container::Img)
					_M = cv::imread(filePath.toStdString());
				cv::Mat tmp = _M;
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
		containerPtr child(new labelContainer(absFileName_, baseName, this));
		childContainers.push_back(child);
		return true;
	}else
	{
		if (absFileName_.endsWith(".xml", Qt::CaseInsensitive))
		{
			containerPtr tmp(new featureContainer(absFileName_,baseName, this));
			childContainers.push_back(tmp);
			return true;
		}else
		{
			containerPtr tmp(new imgContainer(absFileName_, this));
			childContainers.push_back(tmp);
			return true;
		}		
	}
	return false;
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
labelContainer::labelContainer(QString absFilePath, QString parentName, QTreeWidgetItem* parent) :
	imgContainer(parent)
{
	baseName = parentName;
	QFileInfo file(absFilePath);
	type = Label;
	//name = "Label";
	isTop = false;
	label = -2;
	if (file.exists())
	{
		QString lblName = file.baseName();
		filePath = file.absoluteFilePath(); 
		dirName = file.absolutePath();
		if (lblName.contains("positive") || lblName.contains("Pos"))
		{
			label = 1;
			setText(0, "positive mask");
			setText(1, "1");
			name = "positive mask";
		}else
		{
			if (lblName.contains("negative") || lblName.contains("Neg"))
			{
				label = -1;
				setText(0, "negative mask");
				setText(1, "-1");
				name = "negative mask";
			}else
			{
				QString txtLabel = lblName.mid(parentName.size() + 1, lblName.size() - parentName.size());
				label = txtLabel.toInt();
				setText(0, txtLabel + " mask");
				setText(1, txtLabel);
				name = txtLabel + " mask";
			}
		}
		parent->addChild(this);
	}
	QFileInfo polyFile(absFilePath.mid(0, absFilePath.size() - 4) + ".yml");
	if (polyFile.exists())
	{
		cv::FileStorage fs(polyFile.absoluteFilePath().toStdString(), cv::FileStorage::READ);
		int polys = (int)fs["polygons"];
		className = QString::fromStdString((std::string)fs["ClassName"]);
		label = (int)fs["ClassNumber"];
		setText(0, className + " mask");
		name = className + " mask";
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
	fs << "ClassName" << className.toStdString();
	fs << "polygons" << polygons.size();
	fs << "ClassNumber" << label;
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
featureContainer::featureContainer(QString absFilePath, QString parentName, QTreeWidgetItem* parent)
{
	baseName = parentName;
	QFileInfo file(absFilePath);
	filePath = absFilePath;
	type = Features;
	isTop = false;
	if (file.exists())
	{
		cv::FileStorage fs(file.absoluteFilePath().toStdString(), cv::FileStorage::READ);
		std::string ty = (std::string)fs["type"];
		if (ty == "FeatureContainer")
		{
			name = QString::fromStdString((std::string)fs["name"]);
			//cv::FileNode pts = fs["keyPoints"];
			// TODO fix keypoint loading with opencv 3.0
			//fs["keyPoints"] >> keyPts;
		}
	}
	setText(0,name);
	parent->addChild(this);
}
featureContainer::~featureContainer()
{

}
cv::Mat&
featureContainer::lbl()
{
	if (!label.empty()) return label;
    if (filePath.size() == 0) return label;
	cv::FileStorage fs(filePath.toStdString(), cv::FileStorage::READ);
	fs["label"] >> label;
	return label;
}
bool
featureContainer::save()
{
	if (!QDir(dirName + "/Features/").exists())
	{
		// Processing directory doesn't exist, need to make it.
		QDir().mkdir(dirName + "/Features/");
	}
	QString saveName = dirName + "/Features/" + baseName + "_" + text(0) + ".xml";
	cv::FileStorage fs;
	fs.open(saveName.toStdString(), cv::FileStorage::WRITE);
	fs << "type" << "FeatureContainer";
	fs << "name" << name.toStdString();
	fs << "matrix" << _M;
	fs << "KeyPoints" << keyPts;
	fs << "label" << label;
	return true;
}
bool
featureContainer::load()
{
	if (!_M.empty()) return true;
	if (filePath.size() == 0) return false;
	cv::FileStorage fs(filePath.toStdString(), cv::FileStorage::READ);
	fs["matrix"] >> _M;
	return !_M.empty();
}
// *********************** filterContainer **************************
filterContainer::filterContainer(QTreeWidget* parent, filterType type_):
	processingContainer(parent)
	//output(NULL)
{
	type = Filter;
	fType = type_;
	initialize();
}
filterContainer::filterContainer(QTreeWidgetItem* parent, filterType type_):
	processingContainer(parent)
	//output(NULL)
{
	type = Filter;
	fType = type_;
	initialize();
}
filterContainer::filterContainer(filterType type_)
	//output(NULL)
{
	type = Filter;
	fType = type_;
//	output = NULL;
	initialize();
}
filterContainer::filterContainer(cv::FileNode& node)
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
filterPtr
filterContainer::getDisplayCopy()
{
	// Use copy constructor to copy parameters
	filterPtr out(new filterContainer(this));
	out->makeDisplayCopy();
	//_M.copyTo(out->_M);
	out->_M = _M.clone();
	if (input != NULL)
	{
		out->baseNames << input->name;
		out->input = input;
	}
	out->output = output;
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
	if (fType == HSV)
	{
		numParameters = 1;
		parameters.resize(numParameters);
		QStringList names;
		names << "Hue";
		names << "Saturation";
		names << "Value";
		parameters[0] = param(t_pullDown, 0, "Desired Plane", names);
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
	if (fType == nonMaxSuppress)
	{
		parameters.resize(1);
		parameters[0] = param(t_float, 0.8, "% of max", "", 0, 1);
	}
	if (fType == wearDetect)
	{
		name = "Wear Line Detector";
		parameters.resize(2);
		parameters[0] = param(t_float, 0.1, "% of max", "", 0, 1);
		parameters[1] = param(t_path, "C:/code/build/FCVMLT/bin/debug/learned_function.dat", "Path", "Path to saved function");
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
filterContainer::setInput(containerPtr cont_)
{
	input = cont_;
}
containerPtr
filterContainer::process(containerPtr cont_)
{
	if (cont_ == NULL) return containerPtr();
	if (cont_->type != container::Img) return containerPtr();
	// Attempts to de-allocate previous input.  Is this the desired functionality?
	//input = cont_; 
	imgPtr I = boost::dynamic_pointer_cast<imgContainer,container>(cont_);
	if (output != NULL && output != cont_)
	{
		//delete output;
	}
	if (output == NULL)
	{
		output.reset(new imgContainer);
	}
	output->baseName = cont_->baseName;
	output->dirName = cont_->dirName;
	output->name = this->name;
	//output = new imgContainer;
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
	case HSV:
		output = processHSV(I->M());
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
		break;
	case nonMaxSuppress:
		output = processNonMaxSuppress(I->M());
		break;
	case wearDetect:
		output = processWearLineExtractor(I->M());
		break;
	}
	boost::posix_time::ptime end = boost::posix_time::microsec_clock::universal_time();
	boost::posix_time::time_duration delta = end - start;
	emit log(name + " processing took " + QString::number(delta.total_milliseconds()) + " milliseconds", 1);
	if (output == NULL) return containerPtr();
	_M = output->_M;
	return output;
}
imgPtr
filterContainer::processThreshold(cv::Mat img)
{
	//imgContainer* output = new imgContainer;
	cv::Mat greater = img > parameters[0].value;
	cv::Mat less = img < parameters[1].value;
	output->_M = greater.mul(less);
	return output;
}

imgPtr
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
imgPtr
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
imgPtr
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
		output->_M = grad_x;
	}
	if (parameters[4].value == 1)
	{
		output->_M = grad_y;
	}
	if (parameters[4].value == 2)
	{
		cv::addWeighted(grad_x, 0.5, grad_y, 0.5, 0, output->_M);
	}
	return output;
}
imgPtr
filterContainer::processSmooth(cv::Mat img)
{
	//imgContainer* output = new imgContainer;
	if (!((int)parameters[1].value % 2)) return output;
	if (!((int)parameters[2].value % 2)) return output;
	if (parameters[0].value == 0)
	{
		try
		{
			cv::GaussianBlur(img, output->_M, cv::Size(parameters[1].value, parameters[2].value), parameters[3].value, parameters[4].value);
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
			cv::blur(img, output->_M, cv::Size(parameters[1].value, parameters[2].value));
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
			cv::blur(img, output->_M, cv::Size(parameters[1].value, parameters[2].value));
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
			cv::medianBlur(img, output->_M, parameters[1].value);
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
			cv::bilateralFilter(img, output->_M, parameters[1].value, parameters[3].value, parameters[4].value);
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
imgPtr
filterContainer::processGabor(cv::Mat img)
{
	//imgContainer* output = new imgContainer;
	cv::Mat filter = cv::getGaborKernel(cv::Size(parameters[0].value, parameters[0].value),
		parameters[1].value, parameters[2].value, parameters[3].value, parameters[4].value, parameters[5].value);
	cv::Mat grey = img;
	if (img.type() == CV_8UC3)
		cv::cvtColor(grey, grey, CV_BGR2GRAY);
	cv::filter2D(grey, output->_M, CV_32F, filter);
	cv::convertScaleAbs(output->_M, output->_M);
	output->_M.convertTo(output->_M, CV_8U, 255);
	return output;
}
imgPtr
filterContainer::processResize(cv::Mat img)
{
	//imgContainer* output = new imgContainer;
	cv::resize(img, output->_M, cv::Size(parameters[0].value, parameters[1].value));
	return output;
}
imgPtr
filterContainer::processCrop(cv::Mat img)
{
	//imgContainer* output = new imgContainer;
	cv::Rect ROI(parameters[0].value, parameters[1].value,
		parameters[2].value, parameters[3].value);
	try
	{
		output->_M = img(ROI);
	}
	catch (cv::Exception &e)
	{
		std::cout << e.what() << std::endl;
	}
	emit log("Cropped image to " + QString::number(ROI.height) + " " + QString::number(ROI.width), 0);
	return output;
}
imgPtr
filterContainer::processGrey(cv::Mat img)
{
	//imgContainer* output = new imgContainer;
	if (img.channels() == 1)
	{
		std::cout << "Img already grey!";
		output->_M = img;
	}
	else
	{
		cv::cvtColor(img, output->_M, CV_RGB2GRAY);
		//std::cout << output->M().type() << std::endl;
	}
	return output;
}
imgPtr
filterContainer::processHSV(cv::Mat img)
{
	if (img.channels() == 1)
	{
		std::cout << "Cannot convert single channel image";
		return imgPtr();
	}
	else
	{
		cv::Mat tmp;
		cv::cvtColor(img, tmp, CV_BGR2HSV);
		std::vector<cv::Mat> planes;
		cv::split(tmp, planes);
		output->_M = planes[parameters[0].value];
		return output;
	}
}
imgPtr
filterContainer::processHSV_hue(cv::Mat img)
{
	//imgContainer* output = new imgContainer;
	if (img.channels() == 1)
	{
		std::cout << "Cannot convert single channel image";
		return imgPtr();
	}
	else
	{
		cv::Mat tmp;
		cv::cvtColor(img, tmp, CV_BGR2HSV);
		std::vector<cv::Mat> planes;
		cv::split(tmp, planes);
		output->_M = planes[0];
		return output;
	}
}
imgPtr
filterContainer::processHSV_sat(cv::Mat img)
{
	//imgContainer* output = new imgContainer;
	if (img.channels() == 1)
	{
		std::cout << "Cannot convert single channel image";
		return imgPtr();
	}
	else
	{
		cv::Mat tmp;
		cv::cvtColor(img, tmp, CV_BGR2HSV);
		std::vector<cv::Mat> planes;
		cv::split(tmp, planes);
		output->_M = planes[1];
		return output;
	}
}
imgPtr
filterContainer::processHSV_value(cv::Mat img)
{
	//imgContainer* output = new imgContainer;
	if (img.channels() == 1)
	{
		std::cout << "Cannot convert single channel image";
		return imgPtr();
	}
	else
	{
		cv::Mat tmp;
		cv::cvtColor(img, tmp, CV_BGR2HSV);
		std::vector<cv::Mat> planes;
		cv::split(tmp, planes);
		output->_M = planes[2];
		return output;
	}
}
imgPtr
filterContainer::processLaplacian(cv::Mat img)
{
	//imgContainer* output = new imgContainer;
	try
	{
		cv::Laplacian(img, output->_M, CV_16S, parameters[2].value, parameters[0].value, parameters[1].value);
		cv::convertScaleAbs(output->_M, output->_M);
	}
	catch (cv::Exception &e)
	{
		std::cout << e.what() << std::endl;
	}
	return output;
}
imgPtr
filterContainer::processPyrMeanShift(cv::Mat img)
{
	//imgContainer* output = new imgContainer;
	cv::pyrMeanShiftFiltering(img, output->_M, parameters[0].value, parameters[1].value, parameters[2].value);
	return output;
}
imgPtr
filterContainer::processScharr(cv::Mat img)
{
	//imgContainer* output = new imgContainer;
	try
	{
		cv::Scharr(img, output->_M, CV_32S, parameters[0].value, parameters[1].value, parameters[2].value, parameters[3].value);
	}
	catch (cv::Exception &e)
	{
		std::cout << e.what() << std::endl;
	}
	
	return output;
}
imgPtr
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
			cv::Canny(img, output->_M, parameters[0].value, parameters[1].value, parameters[2].value);
		}
		catch (cv::Exception &e)
		{
			std::cout << e.what() << std::endl;
		}
	}
	return output;
}
imgPtr
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
	output->_M = cv::Mat::zeros(img.rows, img.cols, CV_32F);
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
imgPtr
filterContainer::processContourFilter(cv::Mat img)
{
	//imgContainer* output = new imgContainer;
	if (img.channels() != 1)
	{
		log("Need to pass single channel image into contour filter", 2);
		return imgPtr();
	}
	
    std::vector<std::vector<cv::Point> > contours;
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
		return imgPtr();
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
imgPtr
filterContainer::processNonMaxSuppress(cv::Mat img)
{
	if (img.channels() != 1)
	{
		cv::cvtColor(img, img, CV_BGR2GRAY);
	}
	double minVal, maxVal;
	cv::minMaxIdx(img, &minVal, &maxVal);
	output->_M = img > maxVal*parameters[0].value;
	return output;
}
imgPtr
filterContainer::processWearLineExtractor(cv::Mat img)
{
	// Load the SVM from disk
	typedef dlib::matrix<double, 0, 1> sample_type;
	typedef dlib::radial_basis_kernel<sample_type> kernel_type;
	typedef dlib::decision_function<kernel_type> dec_funct_type;
	typedef dlib::normalized_function<dec_funct_type> funct_type;
	funct_type learned_funct;
	try
	{
		dlib::deserialize(parameters[1].pathText.toStdString()) >> learned_funct;
	}
	catch (std::exception &e)
	{
		std::cout << e.what() << std::endl;
		emit log(QString(e.what()), 4);
		return output;
	}
	
	// The input image for this function should be the output of simple.xml
	// Thus img is a sobel of the saturation plane of the original image
	
	// Perform non-maximal suppression to get a mask for processing
	double minVal, maxVal;
	cv::minMaxIdx(img, &minVal, &maxVal);
	cv::Mat mask = img > maxVal*parameters[0].value;
	output->_M = cv::Mat::zeros(img.size(), CV_8U);
	for (int y = 0; y < img.rows; ++y)
	{
		for(int x = 0; x < img.cols; ++x)
		{
			if (mask.at<uchar>(y, x) == 255)
			{
				cv::Mat roi;
				try
				{
					roi = img(cv::Rect(x, y, 20, 1));
				}catch (cv::Exception &e)
				{
					continue;
				}
				sample_type samp;
				samp.set_size(20, 1);
				for (int i = 0; i < 20; ++i)
				{
					samp(i) = (double)roi.at<float>(i);
				}
				double val = learned_funct(samp);
				if (val > 0)
				{
					// This is a positive example
					output->_M.at<uchar>(y,x) = 255;
				}
				if (val < 0)
				{

				}
			}
		}
	}
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
mlContainer::mlContainer(QTreeWidget* parent, mlType type_):
	processingContainer(parent)
{
	MLType = type_;
	initialize();
}
mlContainer::mlContainer(QTreeWidgetItem* parent, mlType type_) :
	processingContainer(parent)
{
	MLType = type_;
	initialize();
}
mlContainer::mlContainer(mlType type_)
{
	MLType = type_;
	initialize();
}
mlContainer::~mlContainer()
{

}
void
mlContainer::initialize()
{
	type = ML;
	parameters.push_back(param(t_bool, 0, "Normalize"));
	if (MLType == dlib_SVM_radialBasisKernel)
	{
		name = "dlib Support Vector Machine: Radial Basis Kernel";
		parameters.push_back(param(t_double, 0.5, "Gamma"));
		parameters.push_back(param(t_double, 0.5, "nu"));
	}
	if (MLType == cv_NBC)
	{
		name = "Opencv normal bayes classifier";
	}
	if (MLType == cv_KNN)
	{
		name = "Opencv K Nearest Neighbors";
	}
	if (MLType == cv_SVM)
	{
		name = "Opencv Support Vector Machine";
		QStringList types;
		types << "C_SVC" << "NU_SVC" << "One class" << "Eps SVR" << "Nu SVR";
		QStringList kernels;
		kernels << "Linear" << "Polynomial" << "Radial Basis Function" << "Sigmoid";

		parameters.push_back(param(t_pullDown, 0, "SVM Type", types));
		parameters.push_back(param(t_pullDown, 0, "Kernel Type", kernels));
		parameters.push_back(param(t_double, 0, "Degree"));
		parameters.push_back(param(t_double, 0, "Gamma"));
		parameters.push_back(param(t_double, 0, "Coefficient 0"));
		parameters.push_back(param(t_double, 0, "C value"));
		parameters.push_back(param(t_double, 0, "Nu"));
		parameters.push_back(param(t_double, 0, "P"));
		parameters.push_back(param(t_double, 1000, "Max iterations"));
		parameters.push_back(param(t_double, FLT_EPSILON, "Epsilon"));
	}
	if (MLType == cv_DT)
	{
		name = "Opencv Decision Tree";
	}
	if (MLType == cv_BOOSTED)
	{
		name = "Opencv Boosted Decision Tree";
	}
	if (MLType == cv_GBOOSTED)
	{
		name = "Opencv Gradient Boosted Tree";
	}
	if (MLType == cv_RANDOM_TREE)
	{
		name = "Opencv Random Tree";
	}
	if (MLType == cv_ER_TREES)
	{
		name = "Opencv extremely random tree";
	}
	if (MLType == cv_EM)
	{
		name = "Opencv expectation maximization";
	}
	if (MLType == cv_NN)
	{
		name = "Opencv neural network";
	}
}
float	
mlContainer::train(cv::Mat features, cv::Mat labels)
{
	if (MLType == dlib_SVM_radialBasisKernel)
	{
		typedef dlib::radial_basis_kernel<sample_type> kernel_type;
		// Restructure data to fit dlib format
		std::vector<sample_type> X;
		std::vector<double> Y;
		// A matrix needs to be created that is the square of the number of features, the max size of this matrix
		// (nc*nr) has to be smaller than a 32bit long. Thus the max number of points is sqrt(2^32-1)
		// TODO: Fix change dlib to use 64bit ints instead of 32bit ints for storing matrix info.
		int maxSize = std::min(65000, features.rows);
		X.reserve(maxSize);
		Y.reserve(maxSize);
		for (int i = 0; i < maxSize; ++i)
		{
			sample_type samp;
			samp.set_size(features.cols, 1);
			for (int j = 0; j < features.cols; ++j)
			{
				samp(j) = (double)features.at<float>(i, j);
			}
			X.push_back(samp);
			Y.push_back((double)labels.at<float>(i));
		}
		typedef dlib::decision_function<kernel_type> dec_funct_type;
		typedef dlib::normalized_function<dec_funct_type> funct_type;
		funct_type learned_function;
		
		if (parameters[0].value == 1)
		{
			dlib::vector_normalizer<sample_type> normalizer;
			normalizer.train(X);
			for (int i = 0; i < X.size(); ++i)
			{
				X[i] = normalizer(X[i]);
			}
			learned_function.normalizer = normalizer;
		}
		
		dlib::randomize_samples(X, Y);
		const double max_nu = dlib::maximum_nu(Y);
		if (max_nu == 0.0)
		{
			emit log("Error max nu is zero, only one class found in dataset", 5);
		}
		dlib::svm_nu_trainer<kernel_type> trainer;
		trainer.set_kernel(kernel_type(parameters[0].value));
		trainer.set_nu(parameters[1].value);
		double bestGamma = 0.0;
		double bestNu = 0.0;
		double bestVal = 0.0;
		for (double gamma = 0.00001; gamma <= 1; gamma *= 5)
		{
			for (double nu = 0.00001; nu < max_nu; nu *= 5)
			{
				trainer.set_kernel(kernel_type(gamma));
				trainer.set_nu(nu);
				std::cout << "Gamma: " << gamma << " nu: " << nu;
				try{
					dlib::matrix<double, 1, 2> score = dlib::cross_validate_trainer(trainer, X, Y, 3);
					std::cout << " Accuracy: " << score;
					if (score(0) + score(1) > bestVal)
					{
						bestVal = score(0) + score(1);
						bestGamma = gamma;
						bestNu = nu;
					}
					emit results(name + " trained with gamma: " + QString::number(gamma) +
						" nu: " + QString::number(nu) + " score: " + QString::number(score(0)) + " / " + QString::number(score(1)));
				}
				catch (std::exception &e){
					std::cout << e.what() << std::endl;
				}
			}
		}
		// Now retrain using the best paramters that we found and save that shit yo
		
		trainer.set_kernel(kernel_type(bestGamma));
		trainer.set_nu(bestNu);
		dlib::matrix<double, 1, 2> score = dlib::cross_validate_trainer(trainer, X, Y, 3);
		learned_function.function = trainer.train(X, Y);
		dlib::serialize("learned_function.dat") << learned_function;
		int numVectors = learned_function.function.basis_vectors.size();
		emit results(name + " training final function with gamma: " + QString::number(bestGamma) + " and nu: " + QString::number(bestNu) +
			" results in a function with: " + QString::number(numVectors) + " support vectors");
		emit results("Reducing support vectors");
		if (numVectors > 300)
		{
			for (int i = numVectors; i > 100; i /= 10)
			{
				dlib::matrix<double, 1, 2> score = dlib::cross_validate_trainer(dlib::reduced2(trainer, i), X, Y, 3);
				emit results(name + " trained with " + QString::number(i) +
					" support vectors, score: " + QString::number(score(0)) + " / " + QString::number(score(1)));
				learned_function.function = reduced2(trainer, 10).train(X, Y);
				dlib::serialize("learned_function_" + QString::number(i).toStdString() + ".dat") << learned_function;
			}
		}
		return 0;
	}
	// Normalize parameters for Opencv machine learning tasks
	if (parameters[0].value == 1)
	{
		normalizationParameters = cv::Mat::zeros(2, features.cols, CV_32F);
		for (int i = 0; i < features.cols; ++i)
		{
			cv::Mat mean, var;
			cv::meanStdDev(features.col(i), mean, var);
			features.col(i) = (features.col(i) - (float)mean.at<double>(0)) / (float)var.at<double>(0);
			normalizationParameters.at<float>(0, i) = (float)mean.at<double>(0);
			normalizationParameters.at<float>(1, i) = (float)var.at<double>(0);
		}
	}
	if (MLType == cv_NBC)
	{
		CvNormalBayesClassifier* tmpClassifier = new CvNormalBayesClassifier(features, labels);
		classifier = tmpClassifier;
		emit results("Normal bayes classifier trained");
		test(features, labels);
		save();
	}
	if (MLType == cv_SVM)
	{
		CvSVM* tmpClassifier;
		try
		{
			CvSVMParams params;
			params.svm_type = parameters[1].value;
			params.kernel_type = parameters[2].value;
			params.degree = parameters[3].value;
			params.gamma = parameters[4].value;
			params.coef0 = parameters[5].value;
			params.C = parameters[6].value;
			params.nu = parameters[7].value;
			params.p = parameters[8].value;
			params.term_crit = cv::TermCriteria(CV_TERMCRIT_EPS + CV_TERMCRIT_ITER, parameters[9].value, parameters[10].value);
			tmpClassifier = new CvSVM(features, labels,cv::Mat(),cv::Mat(),params);
		}catch (cv::Exception &e)
		{
			emit results("SVM failed to train");
			return -1;
		}
		emit results("SVM trained");
		classifier = tmpClassifier;
		save();
		return test(features, labels);
	}
}
float	
mlContainer::test(cv::Mat features, cv::Mat labels)
{
	// In order to build a confusion matrix, I need to figure out how many different classes I'm dealing with
	// Then I need to make a matrix to enter that data into
	// Somehow I also need to deal with when the class is -1
	// As well, it will be important to determine if the class list is continuous
	float correct = 0;
	for (int i = 0; i < features.rows; ++i)
	{
		float val = predict(features.row(i));
		if (val == labels.at<float>(i))
		{
			++correct;
		}
	}
	emit results(name + " testing results (% Correct): " + QString::number(correct / features.rows));
	return correct/features.rows;
}
bool	
mlContainer::save()
{
	if (MLType == dlib_SVM_radialBasisKernel)
	{

	}
	if (MLType == cv_NBC)
	{
		CvNormalBayesClassifier* tmpClassifier = dynamic_cast<CvNormalBayesClassifier*>(classifier);
		tmpClassifier->save((dirName + "/" + name + ".xml").toStdString().c_str());
	}
	if (MLType == cv_SVM)
	{
		CvSVM* tmpClassifier = dynamic_cast<CvSVM*>(classifier);
		tmpClassifier->save((dirName + "/" + name + ".xml").toStdString().c_str());
	}
	if (MLType == cv_DT)
	{
		
	}
	if (MLType == cv_BOOSTED)
	{
		
	}
	if (MLType == cv_GBOOSTED)
	{
		
	}
	if (MLType == cv_RANDOM_TREE)
	{
		
	}
	if (MLType == cv_ER_TREES)
	{
		
	}
	if (MLType == cv_EM)
	{
		
	}
	if (MLType == cv_NN)
	{
		
	}
	return true;
}
bool	
mlContainer::load()
{
	return true;
}
float
mlContainer::predict(cv::Mat features)
{
	if (MLType == cv_NBC)
	{
		CvNormalBayesClassifier* tmpClassifier = dynamic_cast<CvNormalBayesClassifier*>(classifier);
		return tmpClassifier->predict(features);
	}
	if (MLType == cv_SVM)
	{
		CvSVM* tmpClassifier = dynamic_cast<CvSVM*>(classifier);
		return tmpClassifier->predict(features);
	}
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
		filterPtr tmp( new filterContainer(filterNode) );
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
