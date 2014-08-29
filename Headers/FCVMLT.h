#ifndef CONTAINER_H
#define CONTAINER_H
// Contains anything that can be selected from a widget, such as a filter or a source image
#include <QObject>
#include <QString>
#include <QTreeWidgetItem>
#include <qwidget.h>
#include <qgridlayout.h>
#include <qlabel.h>
#include <qspinbox.h>
#include <qcombobox.h>
#include <qpushbutton.h>
#include <qdir.h>
#include "qfileinfo.h"
#include <qcheckbox.h>
#include <qlineedit.h>
#include <qfiledialog.h>
#include <opencv2/core/core.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/features2d/features2d.hpp>
#include <opencv2/nonfree/nonfree.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/ml/ml.hpp>

#include <boost/thread.hpp>
#include <boost/shared_ptr.hpp>

#include <dlib/svm.h>
#include <dlib/opencv.h>

#ifdef DICOM_LOADER_FOUND
#include "subprojects/dicomLoader/dicomLoader.h"
#endif
class container;
class matrixContainer;
class featureContainer;
class processingContainer;
class mlContainer;
class imgContainer;
class streamContainer;
class labelContainer;
class filterContainer;
class compoundFilterContainer;
class featureExtractorContainer;
class featureWindowContainer;
class statContainer;
class filterMacro;

typedef boost::shared_ptr<container> containerPtr;
typedef boost::shared_ptr<matrixContainer> matrixPtr;
typedef boost::shared_ptr<featureContainer> featurePtr;
typedef boost::shared_ptr<processingContainer> processingPtr;
typedef boost::shared_ptr<mlContainer> mlPtr;
typedef boost::shared_ptr<imgContainer> imgPtr;
typedef boost::shared_ptr<streamContainer> streamPtr;
typedef boost::shared_ptr<labelContainer> labelPtr;
typedef boost::shared_ptr<filterContainer> filterPtr;
typedef boost::shared_ptr<compoundFilterContainer> compoundFilterPtr;
typedef boost::shared_ptr<featureExtractorContainer> featureExtractorPtr;
typedef boost::shared_ptr<featureWindowContainer> featureWindowPtr;
typedef boost::shared_ptr<statContainer> statPtr;
typedef boost::shared_ptr<filterMacro> macroPtr;

// Machine learning dlib typedefs

typedef dlib::matrix<double, 0, 1> sample_type;


enum filterType
{
	threshold = 0,
	erode,
	dilate,
	sobel,
	smooth,
	gabor,
	resize,
	crop,
	grey,
	HSV,
	HSV_hue,
	HSV_sat,
	HSV_val,
	laplacian,
	pyrMeanShift,
	scharr,
	canny,
	gradientOrientation,
	contourFilter,
	nonMaxSuppress,
	wearDetect
};

static const QList<filterType> FILTER_TYPES(QList<filterType>()
	<< threshold << erode << dilate << sobel << smooth << gabor << resize << crop 
	<< grey << HSV << HSV_hue << HSV_sat << HSV_val << laplacian << pyrMeanShift 
	<< scharr << canny << gradientOrientation << contourFilter << nonMaxSuppress
	<< wearDetect);

static const QStringList FILTER_NAMES(QStringList()
	<< "Threshold" << "Erode" << "Dilate" << "Sobel" << "Smooth" << "Gabor Filter" 
	<< "Resize" << "Crop" << "Convert to Grey" << "Single HSV plane extraction" << "Convert to HSV hue"
	<< "Convert to HSV saturation" << "Convert to HSV value" << "Laplacian" 
	<< "Pyramid Mean Shift" << "Sharr Filter" << "Canny Edge Detect" << "Gradient Orientation"  << "Contour Filter" << "Non maximal suppression" << "Wear Line Detector");
enum compoundFilterType
{
    add = 0,
    subtract,
    align

};
enum paramType
{
    t_char,
    t_float,
    t_double,
    t_int,
    t_bool,
    t_pullDown,
	t_path
};
enum statMethod
{
    wholeImage = 0,
    ROI,
    superPixel,
    perPixel,
    keyPoint,
	mask
};
enum statType
{
    sum = 0,
    avg,
	copy,
    median,
    stdev,
    hist,
    sift,
    HoG,
    orb,
	surf,
	circle,
	line,
	lineP
};
enum mlType
{
	// Dlib learners
	dlib_SVM_radialBasisKernel = 0,



	// opencv learners
	cv_NBC,
	cv_KNN,
	cv_SVM,
	cv_DT,
	cv_BOOSTED,
	cv_GBOOSTED,
	cv_RANDOM_TREE,
	cv_ER_TREES,
	cv_EM,
	cv_NN
};
struct param
{
    param()
    {
    }

    param(paramType type_, double value_, QString name_, 
		QStringList pullDown_ = QStringList(""), QString toolTip_ = ""):
		type(type_), value(value_), name(name_), toolTip(toolTip_), pullDownItems(pullDown_), btn(NULL)
    {
		minVal = -10000;
		maxVal = 10000;
    }
	param(paramType type_, double value_, QString name_, QString toolTip_, double min_, double max_):
		type(type_), value(value_), name(name_), toolTip(toolTip_), maxVal(max_), minVal(min_), btn(NULL)
	{

	}
	param(paramType type_, QString text_, QString name_, QString toolTip_):
		pathText(text_), type(type_), name(name_), toolTip(toolTip_), btn(NULL)
	{

	}
    paramType type;
    double value;
	double maxVal;
	double minVal;
    QString name;
    QObject* box;
	QObject* btn;
    QString toolTip;
    QStringList pullDownItems;
	QString pathText;

	
};

static const QList<statMethod> STAT_METHODS(QList<statMethod>()
				<<wholeImage<<ROI<<superPixel<<perPixel<<keyPoint<<mask);
static const QList<compoundFilterType> COMPOUND_FILTER_TYPES(QList<compoundFilterType>()
				<<add<<subtract<<align);
static const QList<statType> STAT_TYPES(QList<statType>()
				<<sum<<avg<<copy<<median<<stdev<<hist<<sift<<HoG<<orb<<surf<<circle<<line<<lineP);
static const QList<mlType> MACHINE_LEARNING_ALGORITHMS(QList<mlType>() 
			<< dlib_SVM_radialBasisKernel
			<< cv_NBC
			<< cv_KNN
			<< cv_SVM
			<< cv_DT
			<< cv_BOOSTED
			<< cv_GBOOSTED
			<< cv_RANDOM_TREE
			<< cv_ER_TREES
			<< cv_EM
			<< cv_NN
			);
// Defines how a specific color will scale with values
struct colorScale
{
	colorScale(double start_ = 0, double slope_ = 1, bool symmetric_ = false)
	{
		start = start_;
		slope = slope_;
		symmetric = symmetric_;
		flipped = false;
		inverted = false;
	}
	// Defines where this color starts to take effect, between zero and 1000
	double start;
	// Defines the slope of increase / decrease for this color between 1 and 255
	double slope;
	// Defines if the slope decreases after it peaks
	bool	symmetric;
	// Defines if this color starts high then goes low
	bool	inverted;
	uchar operator ()(double location)
	{
		return getValue(location);
	}
	uchar getValue(double location_)
	{
		double value = 0;
		if (location_ > start)
		{
			value = (location_ - start)*slope;
		}
		else
		{
			value = 0;
		}
		if (value > 255)
		{
			if (symmetric) value = 512 - value;
			else value = 255;
		}
		if (value < 0) value = 0;
		if (inverted) value = 255 - value;
		return (uchar)value;
	}
	bool flipped;
};
class container: public QTreeWidgetItem
{
public:
	enum containerType
	{
		Display = 0,
		Reference,
		Matrix,
		Img,
		Label,
		Stream,
		Features,
		Processor,
		FeatureExtractor,
		FeatureWindow,
		Filter,
		CompoundFilter,
		FilterMacro,
		Stat,
		ML
	};
	// ************* constructors ***************
	explicit container(QTreeWidget *parent = 0);
	explicit container(QTreeWidgetItem* parent);
	~container();
	// ************ functions *******************
	virtual bool
		load();
	virtual bool
		save();
	// Recursive search for a specific child based on name
	virtual containerPtr getChild(QString name);
	virtual containerPtr getChild(container* ptr);
	virtual bool deleteChild(QString name);
	virtual bool deleteChild(container* ptr);

	// ************ members *********************
	// True if saved back to the img sources widget for further handling
	bool				cached;
	// True if saved to disk
	bool				saved;
	// True if a top level item
	bool				isTop;
	// Does this container hold processed data?  True = holds data, false = display or parameter container
	bool				isData; 
	// Full path to file
	QString				filePath; 
	// Image name, stays constant for all children
	QString				baseName; 
	// Directory name
	QString				dirName;  
	// Display name, changes based on what processing has been done
	QString				name;
	containerType		type;
	container*			parentContainer;
	QList<containerPtr>	childContainers;
	virtual void
		updateDisplay();
};
// *********************** referenceContainer ***********************
class referenceContainer : public container
{
	// This container is used for definining reference datum in an image for conversion 
	// of pixel coordinates to real world coordinates relative to a location.
	// The location can be either a point or a line pair.  When it is a point then either x or y 
	// coordinates can be extracted relative to it.  But with no rotation adjustment.
	// When the location is a line pair, then the origin is the intersection of the line pair, and rotation is adjusted accordingly.
	// Line pairs are drawn with the X-axis in green and the Y-axis in blue.
	// Points are drawn with a green circle of radius 5
	referenceContainer(QTreeWidget* parent = 0);
	referenceContainer(QTreeWidgetItem* parent);
	referenceContainer(referenceContainer* cpy);
	referenceContainer(QString absFilePath, QTreeWidget* parent);
	referenceContainer(QString absFilePath, imgContainer* parent);
	~referenceContainer();

	// Defines the conversion scale between pixel coordinates and real world coordinates
	// (U,V)*scale = (X,Y)
	float scale;
	// Rotates / offsets the pixel coordinates based on some origin
	cv::Mat H;

	// Set the origin based on a point in UV coordinates
	void setOrigin(cv::Point2f pt);
	// Set the rotation based on an angle
	void setRotation(float theta);
	// Set the rotation based on a vector
	void setRotation(cv::Vec2f x_axis);
	// Sets the transformation based on the point and an angle
	void setTransform(cv::Point2f pt, float theta);
	// Sets the transformation based on a point and a vector 
	void setTransform(cv::Point2f pt, cv::Vec2f x_axis);
	// Given an input set of points in UV coordinates, returns a set of points in XY coordinates
	cv::Mat transformPoints(cv::Mat pts);
	cv::Mat transformPoints(std::vector<cv::Point2f> pts);
	// Given an input mask, converts all non zero points to UV coordinates then converts to XY and returns
	cv::Mat pointsFromMask(cv::Mat mask);

	// This function gets the parent container then draws the reference coordinate system and a scale on the image

	cv::Mat M();

};
// *********************** matrixContainer **************************
class matrixContainer : public container
{
public:
	// Constructors
	matrixContainer(QTreeWidget* parent = 0);
	matrixContainer(QTreeWidgetItem* parent);
	~matrixContainer();
	bool load();
	cv::Mat& M();
	// Members
	cv::Mat						_M;
	bool						isImg;
	double						max;
	double						min;
	bool						maxMinValid;
};
// *********************** featureContainer *************************
// Feature vector / feature image
class featureContainer : public matrixContainer
{
public:
	featureContainer(QTreeWidget* parent = 0);
	featureContainer(QTreeWidgetItem* parent);
	featureContainer(QString absFilePath, QString parentName, QTreeWidgetItem* parent);
	~featureContainer();
	bool save();
	bool load();
	cv::Mat& lbl();
	std::vector<cv::Point2f> keyPts;
	cv::Mat	label;
	double maxVal;
	double minVal;

};
// *********************** processingContainer **********************
class processingContainer : public QObject, public matrixContainer
{
	Q_OBJECT
public:
	explicit processingContainer(QTreeWidget* parent = 0);
	explicit processingContainer(QTreeWidgetItem* parent);
	explicit processingContainer(processingContainer* cpy);
	explicit processingContainer(QWidget* parent);
	~processingContainer();

	// Used to build the user interface for this processing node
	std::vector<param>	parameters;
	// Number of child threads for processing
	int					numThreads;
	// Name of componenets used as inputs for this processing node
	QStringList				baseNames;
	// Save name is the name used to save to disk and save to the image sources widget, by defult it is the same as name
	QString					saveName;
public slots:
	// Get a widget for controlling the parameters of this object
	QWidget*			getParamControlWidget(QWidget* parent);
	// Update parameters of this object
	virtual void		handleParamChange(QString val);
	// Used for handling state change of checkboxes
	virtual void		handleParamChange(int val);
	virtual void		handlePathSelect();
	// Handle accept clicked for this object
	virtual void		handleAccept();
	virtual void		handleCancel();
	// Handles an updates to the processing parameters
	virtual void		update();
	// Set the input container for processing
	virtual void		setInput(containerPtr cont);
	// Process data
	virtual containerPtr	process(containerPtr cont_);
	// Make a copy with current parameters for display
	containerPtr	  getDisplayCopy();


signals:
	void accepted();
	void canceled();
	void log(QString line, int level);
	void processingFinished(containerPtr cont);
	void featureMatrix(containerPtr cont);
	void displayMatrix(cv::Mat img);
	void parameterUpdated();

private:
	container* input;

};
// *********************** mlContainer *****************************
// Machine learning extraction technique
class mlContainer : public processingContainer
{
	Q_OBJECT
public:
	explicit	mlContainer(QTreeWidget* parent = 0, mlType type_ = dlib_SVM_radialBasisKernel);
	explicit	mlContainer(QTreeWidgetItem* parent, mlType type_ = dlib_SVM_radialBasisKernel);
	explicit	mlContainer(mlType type_);

				~mlContainer();
	void		initialize();
	float		train(cv::Mat features, cv::Mat labels);
	float		test(cv::Mat features, cv::Mat labels);
	float		predict(cv::Mat features);
	bool		save();
	bool		load();
	mlType		MLType;
	CvStatModel* classifier;
	cv::Mat		normalizationParameters;
signals:
	void results(QString line);
};
// *********************** imgContainer *****************************
class imgContainer : public matrixContainer
{
public:
	imgContainer(QTreeWidget* parent = 0);
	imgContainer(QTreeWidgetItem* parent);
	imgContainer(imgContainer* cpy);
	// Create an image container referencing an image on disk
	imgContainer(QString absFilePath, QTreeWidget* parent);
	imgContainer(QString absFilePath, imgContainer* parent);
	~imgContainer();

	// Name if loaded from disk
	QString		sourceName;	
	// Source directory if loaded from disk
	QString		sourceDir;
	bool		mask;
	virtual void
		updateDisplay();
	bool 
		load();
	bool
		save();
	// Searches this directory for any children of this parent, and loads it
	bool loadChildFromDir(QString dirPath);
	bool loadChild(QString dirName_, QString absFileName_);
};

// *********************** streamContainer **************************
class streamContainer : public QObject, public imgContainer
{
	Q_OBJECT
public:

	streamContainer(QTreeWidget *parent = 0);
	streamContainer(QTreeWidgetItem* parent);
	~streamContainer();

	void
		openCamera(int deviceID, int resolution, int frameRate);

	int										frameRate;
private:
	void									
		capture();
	bool									_quit;
	int										_msDelay;
	boost::shared_ptr<boost::thread>		_camThread;
	boost::shared_ptr<cv::VideoCapture>		_camObj;
signals:
	// Might only be used by stream container, if so move it over there.
	void imgUpdated(cv::Mat img);	
	void imgUpdated();
};
// *********************** labelContainer **************************
// Holds a mask object and a vector of bounding boxes for this label
class labelContainer : public imgContainer//, QObject
{
public:
	// ********** Constructors
	labelContainer(QTreeWidget* parent);
	labelContainer(QTreeWidgetItem* parent);
	// absFilePath is the absolute path to this file, parentName is the base name of the parent object, parent is a pointer to the widget
	labelContainer(QString absFilePath, QString parentName,  QTreeWidgetItem* parent);
	~labelContainer();
	bool
		load();
	bool 
		save();
	int label;
	QString className;
    QList<std::vector<cv::Point> > polygons;
};
// *********************** filterContainer *************************
// Filter name and parameters
class filterContainer : public processingContainer
{
	Q_OBJECT
public:
	filterContainer(QTreeWidget* parent = 0, filterType type_ = threshold);
	filterContainer(QTreeWidgetItem* parent, filterType type_ = threshold);
	filterContainer(filterType type_);
	filterContainer(filterContainer* cpy);
	filterContainer(cv::FileNode& node);
	~filterContainer();
	filterType fType;
	int numParameters;
	int idx;
	// Keep a copy of the input so that it can be reprocessed easily on parameter update
	containerPtr		input;

	imgPtr				output;
	// Previous processing step
	filterPtr			filterParent;
	// Next processing step
	filterPtr			filterChild;
	
	QPushButton*		buttonPtr;

	filterPtr			getDisplayCopy();
	void				makeDisplayCopy();
	//public slots:
	void				writeSettingsXML(cv::FileStorage& fs);
	void				initialize();
	containerPtr		process(containerPtr cont);
	void				setInput(containerPtr cont);
	void				update();
	

signals:

private:
	void		gradientOrientationHelper(cv::Mat gradX, cv::Mat gradY, cv::Mat orientation, int threadNum);
	imgPtr		processThreshold(cv::Mat img);
	imgPtr		processErode(cv::Mat img);
	imgPtr		processDilate(cv::Mat img);
	imgPtr		processSobel(cv::Mat img);
	imgPtr		processSmooth(cv::Mat img);
	imgPtr		processGabor(cv::Mat img);
	imgPtr		processResize(cv::Mat img);
	imgPtr		processCrop(cv::Mat img);
	imgPtr		processGrey(cv::Mat img);
	imgPtr		processHSV(cv::Mat img);
	imgPtr		processHSV_hue(cv::Mat img);
	imgPtr		processHSV_sat(cv::Mat img);
	imgPtr		processHSV_value(cv::Mat img);
	imgPtr		processLaplacian(cv::Mat img);
	imgPtr		processPyrMeanShift(cv::Mat img);
	imgPtr		processScharr(cv::Mat img);
	imgPtr		processCanny(cv::Mat img);
	imgPtr		processGradientOrientation(cv::Mat img);
	imgPtr		processNonMaxSuppress(cv::Mat img);
	// Requires a boolean input image
	imgPtr		processContourFilter(cv::Mat img);
	imgPtr		processWearLineExtractor(cv::Mat img);

    std::vector<boost::shared_ptr<boost::thread> >	procThreads;

};
// *********************** compoundFilterContainer *****************
class compoundFilterContainer : public filterContainer
{
public:
	compoundFilterContainer(QTreeWidget* parent = 0);
	compoundFilterContainer(QTreeWidgetItem* parent);
	~compoundFilterContainer();
};
// *********************** featureExtractorContainer ****************
// This object extracts features from an image in areas defined by the feature window container.
// Given the sType which defines the type of feature to extract, this image will extract those features from
// an input src image.  The input src image should be the region of interest from the feature window container.
// dispImg is mostly used with keyPoint extractors like sift, and feature extractors such as hough lines.
class featureExtractorContainer :public processingContainer
{
	Q_OBJECT
public:
	featureExtractorContainer(QTreeWidget* parent = 0, statType type_ = sum);
	featureExtractorContainer(QTreeWidgetItem* parent, statType type_);
	featureExtractorContainer(statType type_);
	featureExtractorContainer(QWidget* parent, statType type_);
	featureExtractorContainer(featureExtractorContainer* cpy);

	~featureExtractorContainer();
	statType			sType;
	int					numFeatures;
	bool				visualizableResult;
	//public slots:

	// Src is the source image to extract features
	// Features is the container to put the features into.  Pre initialized so now it just directly puts data into the matrix
	// The other option here is to return a row of the feature matrix and copy it into the initialized feature matrix
	// dispImg is an image used for visualization of the features
	bool				extractFeatures(cv::Mat src, cv::Mat& features, cv::Mat* dispImg = NULL);
	
	// These extractors work on the entire image and extract things such as key points using sift, surf, orb, 
	// or lines and circles
	bool				extractKeyPoints(cv::Mat src, featurePtr features, cv::Mat* dispImg = NULL);
	void				initialize();
	void				recalculateNumParams();

	void				handleParamChange(QString val);

signals:
	void				updated();
};
// *********************** featureWindowContainer *******************
// The feature window container class defines the technique used for extracting features from an image
// Techniques include, whole image extraction, region of interest, super-pixel, perPixel, keypoint, and mask.
// In all cases, if a mask is included, the mask will be used to determine regions of extraction.
// An example of this would be using a mask with superPixel ectraction, this would extract features in a super pixel at each
// point where the mask is non-negative.
class featureWindowContainer : public processingContainer
{
	Q_OBJECT
public:
    featureWindowContainer(QTreeWidget* parent = NULL);
	featureWindowContainer(QTreeWidget* parent, statMethod method_, featureExtractorPtr curExtractor_);
	featureWindowContainer(QTreeWidgetItem* parent, statMethod method_, featureExtractorPtr curExtractor_);
	featureWindowContainer(statMethod method_, featureExtractorPtr curExtractor_);
    featureWindowContainer(statMethod method_);
    featureWindowContainer(featureWindowContainer* cpy);

	void						initialize();
	void						setSrc(imgPtr src);
	void						setMask(cv::Mat mask);
	void						setExtractor(featureExtractorPtr curExtractor_);
	void						setDisplayImage(imgPtr img);
	
	// Given the input image
	featurePtr						extractFeatures();
	featurePtr						extractFeatures(imgPtr src);
	featurePtr						extractFeatures(imgPtr src, cv::Mat mask_);
	featurePtr						extractFeatures(imgPtr src, cv::Mat mask_, cv::Mat& displayImg);
	featurePtr						extractFeatures(imgPtr src, cv::Mat mask_, cv::Mat& displayImg, featureExtractorPtr curExtractor_);


	statMethod					method;
public slots:
	void						handleExtractorUpdate();

signals:
	void						displayImage(cv::Mat img);
	void						extractedFeatures(featurePtr f);

private:
	featureExtractorPtr			curExtractor;
	imgPtr						curSource;
	cv::Mat						dispImg;
	cv::Mat						mask;
};
// *********************** statContainer ***************************
// Statistical extraction technique
class statContainer : public processingContainer
{
public:
	statContainer(QTreeWidget* parent = 0);
	statContainer(QTreeWidgetItem* parent);
	~statContainer();

	featureExtractorPtr			extractor;
	featureWindowPtr			window;
	QString						mask;
	int							sourceIdx;
	QString						inputName;
};
// *********************** filterMacro *****************************
class filterMacro : public container
{
public:
	filterMacro();
	filterMacro(QString fileName);
	// List of all filters in the order they are applied
	bool load(QString fileName);
	bool save();
	bool save(QString fileName);
	QList<filterPtr> filters;
	int numFilters;
};

#endif // CONTAINER_H
