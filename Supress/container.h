#ifndef CONTAINER_H
#define CONTAINER_H
// Contains anything that can be selected from a widget, such as a filter or a source image

#include <QObject>
#include <QString>
#include <QTreeWidgetItem>

#include <opencv2/core/core.hpp>
#include <boost/thread.hpp>

class container :public QObject, public QTreeWidgetItem
{
    Q_OBJECT
public:
	enum containerType
	{
		Display = 1000,
		Matrix,
		Img,
		Label,
		Stream,
		Features,
		Filter,
		CompoundFilter,
		Stat,
		ML
	};

	explicit container(QTreeWidget *parent = 0);
	explicit container(QTreeWidgetItem* parent);
	~container();
	virtual bool
		load();

	virtual bool
		save();
	bool    isTop;
	// Does this container hold processed data?  True = holds data, false = display or parameter container
	bool	isData
	QString filePath; // Full path to file
	QString baseName; // Image name
	QString dirName;  // Directory name

	containerType		type;
	container* parentFilter;

public slots:
	virtual void
		updateDisplay();
	
signals:
	void imgUpdated();
};

class extractorContainer : public container
{

};

// *********************** matrixContainer **************************
class matrixContainer : public container
{
	Q_OBJECT
public:
	
	matrixContainer(QTreeWidget* parent);
	matrixContainer(QTreeWidgetItem* parent);

	~matrixContainer();
	cv::Mat						M;
	bool						isImg;
};

// *********************** featureContainer **************************
// Feature vector / feature image
class featureContainer : public matrixContainer
{
	Q_OBJECT
public:
	featureContainer(QTreeWidget* parent);
	featureContainer(QTreeWidgetItem* parent);
	~featureContainer();
	std::vector<cv::KeyPoint> keyPts;
	// If ordered, then the M matrix is ordered similar to the source image
	bool isOrdered; 
};

// *********************** imgContainer **************************
class imgContainer : public matrixContainer
{
	Q_OBJECT
public:
	imgContainer(QTreeWidget* parent = 0);
	imgContainer(QTreeWidgetItem* parent);
	~imgContainer();

	QString		sourceName;	// Name if loaded from disk
	QString		sourceDir;	// Source directory if loaded from disk
	virtual void
		updateDisplay();

	bool 
		load();

signals:
	// Might only be used by stream container, if so move it over there.
	void 
		imgUpdated(cv::Mat img);

private:
};

// *********************** streamContainer **************************
class streamContainer : public imgContainer
{
	Q_OBJECT
public:

	streamContainer(QTreeWidget *parent = 0);
	~streamContainer();

	void
		openCamera(int deviceID, int resolution, int frameRate);
private:
	void									capture();
	int										_frameRate;
	bool									_quit;
	int										_msDelay;
	boost::shared_ptr<boost::thread>		_camThread;
	boost::shared_ptr<cv::VideoCapture>		_camObj;
};

// *********************** labelContainer **************************
// Holds a mask object and a vector of bounding boxes for this label
class labelContainer : public imgContainer
{
	Q_OBJECT
public:
	labelContainer(QTreeWidget* parent);
	labelContainer(QTreeWidgetItem* parent);
	~labelContainer();
	QList<std::vector<cv::Point>> polygons;
	bool 
		saveLabelXML(QString fileName);
};

// *********************** filterContainer **************************
// Filter name and parameters
class filterContainer : public imgContainer
{
	Q_OBJECT
public:
	filterContainer(QTreeWidget* parent);
	~filterContainer();
	imgFilter filter;
	filterContainer* filterParent;
	filterContainer* filterChild;
};

// *********************** compoundFilterContainer **************************
class compoundFilterContainer : public filterContainer
{
public:
	compoundFilterContainer(QTreeWidget* parent);
	~compoundFilterContainer();
};

// *********************** statContainer **************************
// Statistical extraction technique
class statContainer : public matrixContainer
{
	Q_OBJECT
public:
	statContainer(QTreeWidget* parent);
	statContainer(QTreeWidgetItem* parent);
	~statContainer();
	// Statistic extraction stuff

	// Determines if the extracted feature vector is organized in an image or unorganized
	
	featureExtractor    statExtractor;
	featureWindow       statWindow;
	int                 sourceIdx;
	QString             sourceName;
	bool				quit;
	int					msDelay;

};

// *********************** mlContainer **************************
// Machine learning extraction technique
class mlContainer : public container
{
	Q_OBJECT
public:
	mlContainer(QTreeWidget* parent);
	~mlContainer();
};

#endif // CONTAINER_H
