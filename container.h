#ifndef CONTAINER_H
#define CONTAINER_H
// Contains anything that can be selected from a widget, such as a filter or a source image

#include <QObject>
#include <QString>
#include <QTreeWidgetItem>
#include <opencv2/core/core.hpp>
#include "imgFilter.h"


#include <boost/thread.hpp>


enum ItemType
{
    imgContainer = 1001,
    filterContainer = 1002,
    statContainer = 1003,
    MLcontainer = 1004
};
class container :public QObject, public QTreeWidgetItem
{
    Q_OBJECT
public:
    explicit container(QTreeWidget *parent = 0);
    ~container();
    void openCamera(int deviceID, int resolution, int frameRate);

    bool    isImg;
    bool    isLabel;
    int     label;
    bool    isFeatures;
    bool    isTop;
    bool    isStream;
	int		frameRate;
    cv::Mat img;
    QString filePath;
    QString baseName;
    QString dirName;
    imgFilter filter;
    //featureWindow window;
    //featureExtractor extractor;
    int idx;
    bool saved;
	bool savedToDisk;
	container* parentFilter;

    // Statistic extraction stuff
    featureExtractor    statExtractor;
    featureWindow       statWindow;
    int                 sourceIdx;
    QString             sourceName;



    //

    bool quit;
    int msDelay;
public slots:

signals:
    void imgUpdated();


private:
    void capture();
    boost::shared_ptr<boost::thread> camThread;
    boost::shared_ptr<cv::VideoCapture> camObj;


};

#endif // CONTAINER_H
