cmake_minimum_required(VERSION 2.8)
PROJECT(FCVMLT)

macro(printfiles VAR)
message("Printing globbed files")
foreach(f ${VAR})
	message(${f})
endforeach(f)
endmacro(printfiles)

set(CMAKE_INCLUDE_CURRENT_DIR ON)
#set(Qt5_DIR $ENV{Qt5_DIR})
#message(${Qt5_DIR})
#set(OPENCV_DIR $ENV{OPENCV_DIR})
#message(${OPENCV_DIR})
set(CMAKE_AUTO_MOC ON)
set(CMAKE_AUTOMOC_MOC_OPTIONS "-bstdafx.h" "-fstdafx.h")

FIND_PACKAGE(Qt5 COMPONENTS Gui Widgets Core OpenGL)
FIND_PACKAGE(OpenCV REQUIRED core imgproc highgui contrib)
set(OpenCV_LIBS opencv_core opencv_imgproc opencv_highgui opencv_contrib)

FIND_PACKAGE(OpenGL)
if( OPENGL_FOUND)
message(STATUS "Found OpenGL")
INCLUDE_DIRECTORIES(${OpenGL_INCLUDE_DIRS})
LINK_DIRECTORIES(${OpenGL_LIBRARY_DIRS})
ADD_DEFINITIONS(${OpenGL_DEFINITIONS})
ADD_DEFINITIONS(-DOPENGL)
endif(OPENGL_FOUND)


set(Boost_USE_STATIC_LIBS @Boost_USE_STATIC_LIBS@)
set(Boost_USE_STATIC @Boost_USE_STATIC@)
set(Boost_USE_MULTITHREAD @Boost_USE_MULTITHREAD@)
set(BOOST_INCLUDEDIR "@Boost_INCLUDE_DIR@")
set(BOOST_REQUIRED_MODULES system filesystem thread date_time iostreams chrono)
set(Boost_NO_BOOST_CMAKE ON)
find_package(Boost 1.47.0 REQUIRED COMPONENTS ${BOOST_REQUIRED_MODULES})

#doesn't work for some reason
SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /EHsc")

file(GLOB sources "SRC/*.cpp")# "base/src/*.cpp" "imebra/src/*.cpp")
file(GLOB headers "Headers/*.h")# "base/include/*.h" "imebra/include/*.h")
file(GLOB forms "*UIs/*.ui")
file(GLOB libs "../build/dicomLoader/Release/*.lib")

QT5_WRAP_UI(UIS ${forms})
QT5_WRAP_CPP(MOC ${sources} ${headers})
set(CMAKE_CXX_FLAGS "${Qt5Widgets_EXECUTABLE_COMPILE_FLAGS}")

#add_definitions(-DKEITH_BUILD)
add_definitions(-DPUNTOEXE_WINDOWS)
add_definitions(-DNOMINMAX)

source_group("Forms" 		FILES ${forms})
source_group("UI Headers" 	FILES ${UIS})
source_group("MOC'd files" 	FILES ${MOC})

INCLUDE_DIRECTORIES(${CMAKE_CURRENT_SOURCE_DIR}/Headers)
INCLUDE_DIRECTORIES(${Boost_INCLUDE_DIR})
INCLUDE_DIRECTORIES(${OPENCV_INCLUDE_DIR})
INCLUDE_DIRECTORIES(${Qt5Gui_INCLUDE_DIRS})
INCLUDE_DIRECTORIES(${Qt5Core_INCLUDE_DIRS})
INCLUDE_DIRECTORIES(${Qt5Widgets_INCLUDE_DIRS})
INCLUDE_DIRECTORIES(${Qt5OpenGL_INCLUDE_DIRS})
INCLUDE_DIRECTORIES("../dicomLoader")

LINK_DIRECTORIES(${BOOST_LIBRARY_DIRS}

)

LINK_DIRECTORIES(${OPENCV_LIBRARY_DIRS}

)

ADD_DEFINITIONS(${QT_DEFINITIONS}

)

ADD_EXECUTABLE(FCVMLT 
	${sources} 
	${MOC}
	${headers_moc} 
	${headers} 
	${UIS}
)

TARGET_LINK_LIBRARIES(FCVMLT 
	${QT_LIBRARIES} 
	${OpenCV_LIBS} 
	${Boost_LIBRARIES}
	Qt5::Gui
	Qt5::Widgets
	Qt5::Core
	Qt5::OpenGL
	${libs}
	${OPENGL_LIBRARIES}
)

# Add test projects
file(GLOB dirs "Test/*")
message("Building test projects for: ")
foreach(dir ${dirs})
		if(IS_DIRECTORY ${dir})
			if(NOT ${dir} STREQUAL ${CMAKE_CURRENT_SOURCE_DIR}/CMakeFiles)
				message(${dir})
				#add_subdirectory(${dir})
				#include_directories(${dir})
				add_subdirectory(${dir})
			endif()
		endif()
endforeach(dir)
