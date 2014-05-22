#ifndef IMGFILTER_H
#define IMGFILTER_H
#include <QObject>
#include <QString>
#include <QVector>
#include <QPushButton>
#include <QComboBox>
#include <QAbstractSpinBox>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <iostream>
#include <boost/thread.hpp>

enum filterType
{
    threshold = 0,
    sobel = 1,
    smooth = 2,
    gabor = 3,
    resize = 4,
    crop = 5,
    grey = 6,
    laplacian = 7,
    pyrMeanShift = 8,
    scharr = 9,
    canny = 10,
    gradientOrientation
};

enum compoundFilterType
{
    add = 0,
    subtract = 1,
    align = 2,

};

enum paramType
{
    t_char = 0,
    t_float = 1,
    t_double = 2,
    t_int = 3,
    t_bool = 4,
    t_pullDown
};

enum statMethod
{
    wholeImage  = 0,
    ROI         = 1,
    superPixel  = 2,
    perPixel    = 3,
    keyPoint    = 4
};

enum statType
{
    sum     = 0,
    avg     = 1,
    median  = 2,
    stdev   = 3,
    hist    = 4,
    sift    = 5,
    HoG     = 6,
    orb     = 7
};

struct param
{
    param()
    {

    }

    param(paramType type_, double value_, QString name_, QStringList pullDown_ = QStringList(""), QString toolTip_ = "") :
        type(type_),
        value(value_),
        name(name_),
        toolTip(toolTip_),
        pullDownItems(pullDown_)
    {
    }
    paramType type;
    double value;
    QString name;
    QObject* box;
    QString toolTip;
    QStringList pullDownItems;
};

class imgFilter
{
public:
    // Constructors
    imgFilter();
    imgFilter(filterType type_);
    // Functions
    cv::Mat applyFilter(cv::Mat img);
    void gradientOrientationHelper(cv::Mat gradX, cv::Mat gradY, cv::Mat orientation, int threadNum);

    // Members
    filterType type;
    unsigned int numParameters;
    std::vector<param> parameters;
    std::string filterName;
    QPushButton* buttonPtr;

private:
    std::vector<boost::shared_ptr<boost::thread>> procThreads;
    int numThreads;
};
// This class extracts a feature based on the feature window selection class found below
class featureExtractor
{
public:
    featureExtractor();
    featureExtractor(statType type_);
    bool extractFeatures(cv::Mat src, cv::Mat& features);
    void recalculateNumParams();
    statType type;
    QString name;
    std::vector<param> parameters;
    int numFeatures; // number of features that will be extracted
    std::vector<cv::Mat> histPlanes;
};


// This class extracts features from an image
class featureWindow
{
public:
    featureWindow();
    featureWindow(statMethod method_, featureExtractor** curExtractor_);

    cv::Mat extractFeatures(cv::Mat src, bool &isImg);
    cv::Mat extractFeatures(cv::Mat src, bool &isImg, featureExtractor** curExtractor_);
    QPushButton* button;
    QString statName;
    statMethod method;
    std::vector<param> parameters;
    featureExtractor** curExtractor;
    std::vector<boost::shared_ptr<boost::thread>> procThread;
};

class compoundFilter
{
public:
    compoundFilter(compoundFilterType type_);

    void applyFilter(std::vector<cv::Mat> &input, std::vector<cv::Mat> &output);

    int numInputs;
    int numOutputs;
    std::vector<param> parameters;

private:
    compoundFilterType type;

};

#endif // IMGFILTER_H
