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
#include <opencv2/core/core.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/features2d/features2d.hpp>
#include <opencv2/nonfree/nonfree.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <boost/thread.hpp>
//#include "imebra\include\imebra.h"
#include "dicomLoader.h"

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
	HSV_hue,
	HSV_sat,
	HSV_val,
	laplacian,
	pyrMeanShift,
	scharr,
	canny,
	gradientOrientation,
	contourFilter
};
static const QList<filterType> FILTER_TYPES(QList<filterType>()
	<< threshold << erode << dilate << sobel << smooth << gabor << resize << crop << grey << HSV_hue << HSV_sat << HSV_val << laplacian << pyrMeanShift << scharr << canny << gradientOrientation << contourFilter);
static const QStringList FILTER_NAMES(QStringList()
	<< "Threshold" << "Erode" << "Dilate" << "Sobel" << "Smooth" << "Gabor Filter" 
	<< "Resize" << "Crop" << "Convert to Grey" << "Convert to HSV hue"
	<< "Convert to HSV saturation" << "Convert to HSV value" << "Laplacian" 
	<< "Pyramid Mean Shift" << "Sharr Filter" << "Canny Edge Detect" << "Gradient Orientation"  << "Contour Filter");
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
    t_pullDown
};
enum statMethod
{
    wholeImage = 0,
    ROI,
    superPixel,
    perPixel,
    keyPoint
};
enum statType
{
    sum = 0,
    avg,
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
struct param
{
    param()
    {
    }

    param(paramType type_, double value_, QString name_, 
		QStringList pullDown_ = QStringList(""), QString toolTip_ = ""):
        type(type_), value(value_), name(name_), toolTip(toolTip_), pullDownItems(pullDown_)
    {
		minVal = -10000;
		maxVal = 10000;
    }
	param(paramType type_, double value_, QString name_, QString toolTip_, double min_, double max_):
		type(type_), value(value_), name(name_), toolTip(toolTip_), maxVal(max_), minVal(min_)
	{

	}
    paramType type;
    double value;
	double maxVal;
	double minVal;
    QString name;
    QObject* box;
    QString toolTip;
    QStringList pullDownItems;

	
};
enum colorMap
{
	custom = 0
};
// Applies a custom colormapping to an image. Incomplete
static cv::Mat applyColorMap(cv::InputArray input, colorMap map, double max, double min)
{
	cv::Mat M = input.getMat();
	double range = max - min;

	if (M.type() == CV_32F)
	{
		// Since this is continuous data, need to calculate a value for each pixel.  This may just be too expensive to actually use
	}
	if (M.type() == CV_8U)
	{

	}
	if (M.type() == CV_16U)
	{
		// Specifically for 16bit dicom images
		std::vector<cv::Vec3b> LUT;
		LUT.reserve(USHRT_MAX);
		uchar selector = 0;
		//for (int i = 0; i < )
	}

	return M;
}
static const QList<statMethod> STAT_METHODS(QList<statMethod>()
				<<wholeImage<<ROI<<superPixel<<perPixel<<keyPoint);
static const QList<compoundFilterType> COMPOUND_FILTER_TYPES(QList<compoundFilterType>()
				<<add<<subtract<<align);
static const QList<statType> STAT_TYPES(QList<statType>()
				<<sum<<avg<<median<<stdev<<hist<<sift<<HoG<<orb<<surf<<circle<<line<<lineP);
class container: public QTreeWidgetItem
{
public:
	enum containerType
	{
		Display = 0,
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
	QList<container*>	childContainers;
	virtual void
		updateDisplay();
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
	~featureContainer();
	std::vector<cv::Point2f> keyPts;
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
	// Handle accept clicked for this object
	virtual void		handleAccept();
	virtual void		handleCancel();
	// Handles an updates to the processing parameters
	virtual void		update();
	// Set the input container for processing
	virtual void		setInput(container* cont);
	// Process data
	virtual container*	process(container* cont);
	// Make a copy with current parameters for display
	virtual container*  getDisplayCopy();


signals:
	void accepted();
	void canceled();
	void log(QString line, int level);
	void processingFinished(container* cont);
	void featureMatrix(container* cont);
	void displayMatrix(cv::Mat img);
	void parameterUpdated();

private:
	container* input;

};
// *********************** mlContainer *****************************
// Machine learning extraction technique
class mlContainer : public processingContainer
{
public:
	explicit mlContainer(QTreeWidget* parent = 0);
	explicit mlContainer(QTreeWidgetItem* parent);
	~mlContainer();
	container* process(cv::InputArray input_);
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
	labelContainer(QString absFilePath, QString parentName, QTreeWidgetItem* parent);
	~labelContainer();
	bool
		load();
	bool 
		save();
	int label;
	QList<std::vector<cv::Point>> polygons;
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
	container* input;
	//
	imgContainer* output;
	// Previous processing step
	filterContainer*								filterParent;
	// Next processing step
	filterContainer*								filterChild;
	std::vector<boost::shared_ptr<boost::thread>>	procThreads;
	QPushButton*									buttonPtr;

	container*  getDisplayCopy();
	void		makeDisplayCopy();
	//public slots:
	void			writeSettingsXML(cv::FileStorage& fs);
	void			initialize();
	imgContainer*		process(container* cont);
	imgContainer*		process();
	void			setInput(container* cont);
	void			update();
	

signals:

private:
	void				gradientOrientationHelper(cv::Mat gradX, cv::Mat gradY, cv::Mat orientation, int threadNum);
	imgContainer*		processThreshold(cv::Mat img);
	imgContainer*		processErode(cv::Mat img);
	imgContainer*		processDilate(cv::Mat img);
	imgContainer*		processSobel(cv::Mat img);
	imgContainer*		processSmooth(cv::Mat img);
	imgContainer*		processGabor(cv::Mat img);
	imgContainer*		processResize(cv::Mat img);
	imgContainer*		processCrop(cv::Mat img);
	imgContainer*		processGrey(cv::Mat img);
	imgContainer*		processHSV_hue(cv::Mat img);
	imgContainer*		processHSV_sat(cv::Mat img);
	imgContainer*		processHSV_value(cv::Mat img);
	imgContainer*		processLaplacian(cv::Mat img);
	imgContainer*		processPyrMeanShift(cv::Mat img);
	imgContainer*		processScharr(cv::Mat img);
	imgContainer*		processCanny(cv::Mat img);
	imgContainer*		processGradientOrientation(cv::Mat img);
	// Requires a boolean input image
	imgContainer*		processContourFilter(cv::Mat img);

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
	bool				isKeyPoint;
	//public slots:

	// Src is the source image to extract features
	// Features is the container to put the features into
	// dispImg is an image used for visualization of hte features
	bool				extractFeatures(cv::Mat src, cv::Mat& features, cv::Mat& dispImg);
	bool				extractKeyPoints(cv::Mat src, featureContainer* features, cv::Mat& dispImg);
	void				initialize();
	void				recalculateNumParams();

	void				handleParamChange(QString val);

signals:
	void				updated();
};
// *********************** featureWindowContainer *******************
class featureWindowContainer : public processingContainer
{
	Q_OBJECT
public:
	featureWindowContainer::featureWindowContainer(QTreeWidget* parent = NULL);
	featureWindowContainer::featureWindowContainer(QTreeWidget* parent, statMethod method_, featureExtractorContainer* curExtractor_);
	featureWindowContainer::featureWindowContainer(QTreeWidgetItem* parent, statMethod method_, featureExtractorContainer* curExtractor_);
	featureWindowContainer::featureWindowContainer(statMethod method_, featureExtractorContainer* curExtractor_);
	featureWindowContainer::featureWindowContainer(statMethod method_);
	featureWindowContainer::featureWindowContainer(featureWindowContainer* cpy);
	void						initialize();
	featureContainer*			extractFeatures(imgContainer* src, cv::Mat &displayImg);
	featureContainer*			extractFeatures(imgContainer* src, cv::Mat &displayImg, featureExtractorContainer* curExtractor_);
	statMethod					method;
public slots:
	void						handleExtractorUpdate();

signals:
	void						displayImage(cv::Mat img);
	void						extractedFeatures(featureContainer* f);
private:
	featureExtractorContainer*	curExtractor;
	imgContainer*				curSource;
	cv::Mat						dispImg;

};
// *********************** statContainer ***************************
// Statistical extraction technique
class statContainer : public processingContainer
{
public:
	statContainer(QTreeWidget* parent = 0);
	statContainer(QTreeWidgetItem* parent);
	~statContainer();

	featureExtractorContainer*	extractor;
	featureWindowContainer*		window;
	int							sourceIdx;
};
class filterMacro : public container
{
public:
	filterMacro();
	filterMacro(QString fileName);
	// List of all filters in the order they are applied
	bool load(QString fileName);
	bool save();
	bool save(QString fileName);
	QList<filterContainer*> filters;
	int numFilters;
};
#endif // CONTAINER_H
