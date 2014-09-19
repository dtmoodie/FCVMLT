// dicomLoader.cpp : Defines the entry point for the console application.
//
#include "dicomLoader.h"
#include "imebra/include/imebra.h"

#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
cv::Mat DLL_EXPORTED readDCM(std::string name)
{
	puntoexe::ptr<puntoexe::stream> inputStream(new puntoexe::stream);
	inputStream->openFile(name, std::ios_base::in);
	puntoexe::ptr<puntoexe::streamReader> reader(new puntoexe::streamReader(inputStream));
	puntoexe::ptr<puntoexe::imebra::codecs::codecFactory> factory(puntoexe::imebra::codecs::codecFactory::getCodecFactory());
	puntoexe::ptr<puntoexe::imebra::dataSet> dataSet;
	dataSet = factory->load(reader, 2048);
	puntoexe::ptr<puntoexe::imebra::image> img(dataSet->getImage(0));
	puntoexe::imebra::image::bitDepth depth = img->getDepth();
	unsigned int x, y, rowSize, pixelSize, numChannels;
	img->getSize(&x, &y);

	int size = img->getDataHandler(false, &rowSize, &pixelSize, &numChannels)->getMemorySize();
	cv::Mat cvImg(y,x,CV_16U,img->getDataHandler(false, &rowSize, &pixelSize, &numChannels)->getMemoryBuffer());
	cv::Mat out;
	cvImg.copyTo(out);
	return out;
}

/*int main()
{
	readDCM("E:/data/images/test.dcm");
	return 0;
}*/


