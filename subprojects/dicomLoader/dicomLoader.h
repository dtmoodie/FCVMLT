#include <opencv2/core/core.hpp>

#if defined _MSC_VER
#define DLL_EXPORTED __declspec(dllexport)
#else
#define DLL_EXPORTED __attribute__((__visibility__("default")))
#endif

extern cv::Mat DLL_EXPORTED readDCM(std::string name);
