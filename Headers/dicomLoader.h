#include "imebra\include\imebra.h"
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>

extern cv::Mat __declspec(dllexport) readDCM(std::string name);