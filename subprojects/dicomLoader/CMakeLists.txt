cmake_minimum_required(VERSION 2.8)
PROJECT(dicomLoader)

set(CMAKE_INCLUDE_CURRENT_DIR ON)
if(WIN32)
SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /EHsc")
endif(WIN32)

FIND_PACKAGE(OpenCV REQUIRED core)
set(OpenCV_LIBS opencv_core opencv_imgproc opencv_highgui)

INCLUDE_DIRECTORIES(${OPENCV_INCLUDE_DIR})
file(GLOB sources "*.cpp" "base/src/*.cpp" "imebra/src/*.cpp")
file(GLOB headers "*.h" "base/include/*.h" "imebra/include/*.h")

INCLUDE_DIRECTORIES(${CMAKE_CURRENT_SOURCE_DIR}/Headers)


add_library(dicomLoader SHARED
	${sources} 
	${headers} 
)

TARGET_LINK_LIBRARIES(dicomLoader 
	${OpenCV_LIBS} 
)

if(WIN32)
set(dicomLoader_FOUND TRUE PARENT_SCOPE)
set(dicomLoader_DEFINITIONS "-DDICOM_LOADER_FOUND" PARENT_SCOPE)
set(dicomLoader_LIBS "${CMAKE_BINARY_DIR}/bin/Debug/dicomLoader.lib" PARENT_SCOPE)
else(WIN32)

endif(WIN32)
