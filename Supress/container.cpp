#include "container.h"

container::container(QTreeWidget *parent) :
	QTreeWidgetItem(parent),
	isFeatures(false),
	saved(false),
	isLabel(false),
	label(0),
	idx(-1),
	isStream(false),
	savedToDisk(false)
{
	updateDisplay();
}
container::container(QTreeWidgetItem* parent) :
	QTreeWidgetItem(parent)
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

void container::updateDisplay()
{
	this->setText(0, baseName);
}

// ************************************************ imgContainer ***************************************
imgContainer::imgContainer(QTreeWidget* parent) :
	matrixContainer(parent)
{
	type = Img;
}

imgContainer::imgContainer(QTreeWidgetItem* parent) :
	matrixContainer(parent)
{
	type = Img;
}
imgContainer::~imgContainer()
{

}
bool imgContainer::load()
{
	if (!M.empty()) return true;
	try
	{
		if (filePath.size() > 0)
		{
			if (type == container::Img)
				M = cv::imread(filePath.toStdString());
			if (type == container::Label)
				M = cv::imread(filePath.toStdString(),0);
		}
		if (M.empty()) return false; // Still empty, loading failed
	}
	catch (cv::Exception &e)
	{
		std::cout << e.what();
		return false;
	}
	return true;
}
void
imgContainer::updateDisplay()
{
	this->setText(0, baseName);
}

// ************************************************ streamContainer ***************************************

streamContainer::streamContainer(QTreeWidget *parent):
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

void streamContainer::openCamera(int deviceID, int resolution, int frameRate)
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

void streamContainer::capture()
{
    while(!_quit)
    {
        boost::this_thread::sleep(boost::posix_time::milliseconds(1));
        if(_camObj->read(M))
        {
            if(!M.empty())
            {
                emit imgUpdated(M);
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


labelContainer::~labelContainer()
{

}

bool labelContainer::saveLabelXML(QString fileName)
{
	cv::FileStorage fs(fileName.toStdString(),cv::FileStorage::WRITE);
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


// ************************************************ matrixContainer ***************************************
matrixContainer::matrixContainer(QTreeWidget* parent):
	container(parent)
{
	type = Matrix;
}
matrixContainer::matrixContainer(QTreeWidgetItem* parent):
	container(parent)
{

}
matrixContainer::~matrixContainer()
{

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

}

featureContainer::~featureContainer()
{

}


// *********************** filterContainer **************************
filterContainer::filterContainer(QTreeWidget* parent):
	imgContainer(parent),
	filterParent(NULL),
	filterChild(NULL)
{
	type = Filter;
}
filterContainer::~filterContainer()
{

}

// *********************** compoundFilterContainer **************************

compoundFilterContainer::compoundFilterContainer(QTreeWidget* parent):
	filterContainer(parent)
{
	type = CompoundFilter;
}

compoundFilterContainer::~compoundFilterContainer()
{

}


// *********************** statContainer **************************
statContainer::statContainer(QTreeWidget* parent):
	matrixContainer(parent)
{
	type = Stat;
}
statContainer::~statContainer()
{

}


// *********************** mlContainer **************************

mlContainer::mlContainer(QTreeWidget* parent)
{
	type = ML;
}
mlContainer::~mlContainer()
{

}
