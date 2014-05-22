#include "container.h"

container::container(QTreeWidget *parent) :
    QTreeWidgetItem(parent),
    isImg(true),
    isFeatures(false),
    saved(false),
    isLabel(false),
    label(0),
    idx(-1),
    isStream(false),
    quit(true),
    msDelay(20),
	savedToDisk(false)
{
}


container::~container()
{
    for(unsigned int i = 0; i < childCount(); ++i)
    {
        removeChild((child(i)));
    }
    quit = true;
    if(camThread)
        camThread->join();
}


void container::openCamera(int deviceID, int resolution, int frameRate)
{
    camObj.reset(new cv::VideoCapture(deviceID));
	bool success = false;
	success = camObj->set(CV_CAP_PROP_FOURCC, CV_FOURCC('M', 'J', 'P', 'G'));
    if(resolution == 0)
    {

		success = camObj->set(CV_CAP_PROP_FRAME_HEIGHT, 1080);
        success = camObj->set(CV_CAP_PROP_FRAME_WIDTH, 1920);
    }
    if(resolution == 1)
    {

		success = camObj->set(CV_CAP_PROP_FRAME_HEIGHT, 720);
		success = camObj->set(CV_CAP_PROP_FRAME_WIDTH, 1280);
    }
    if(resolution == 2)
    {

		success = camObj->set(CV_CAP_PROP_FRAME_HEIGHT, 480);
		success = camObj->set(CV_CAP_PROP_FRAME_WIDTH, 640);
    }
	success = camObj->set(CV_CAP_PROP_FPS, double(frameRate));
	
    camThread.reset(new boost::thread(boost::bind(&container::capture,this)));
    quit = false;
}

void container::capture()
{
    while(!quit)
    {
        boost::this_thread::sleep(boost::posix_time::milliseconds(1));
        if(camObj->read(img))
        {
            if(!img.empty())
            {
                emit imgUpdated();
            }
        }
    }
}
