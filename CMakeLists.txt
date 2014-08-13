cmake_minimum_required(VERSION 2.8)
PROJECT(FCVMLT)

macro(printfiles VAR)
message("Printing globbed files")
foreach(f ${VAR})
	message(${f})
endforeach(f)
endmacro(printfiles)

macro(ADD_SUB_PROJECTS curDir parent)
	FILE(GLOB children "subprojects/*")
	foreach(subproject ${children})
		get_filename_component(PROJECT_NAME ${subproject} NAME)
		add_subdirectory(${subproject})
		if(${PROJECT_NAME}_FOUND)
			message("Found subproject:  ${PROJECT_NAME}")
                        set(SUBPROJECTS "${SUBPROJECTS} ${PROJECT_NAME}")
			if(${${PROJECT_NAME}_INCLUDES})
				INCLUDE_DIRECTORIES("${${PROJECT_NAME}_INCLUDES}")
			endif(${${PROJECT_NAME}_INCLUDES})
			#add an option box for this specific project
			#if(${${PROJECT_NAME}_DEFINITIONS} STREQUAL "")
                        message(${${PROJECT_NAME}_DEFINITIONS})
                        ADD_DEFINITIONS("${${PROJECT_NAME}_DEFINITIONS}")
			#endif(${${PROJECT_NAME}_DEFINITIONS})
			#set(SUBPROJECT_LIBS "${${PROJECT_NAME}_LIBS} ${SUBPROJECT_LIBS}")
			LIST(APPEND SUBPROJECT_LIBS ${${PROJECT_NAME}_LIBS})
			message(${SUBPROJECT_LIBS})
		endif(${PROJECT_NAME}_FOUND)
	endforeach(subproject)
endmacro(ADD_SUB_PROJECTS)


set(Qt5_DIR $ENV{Qt5_ROOT})
set(OpenCV_DIR $ENV{OpenCV_ROOT})
set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(SUBPROJECTS "")
ADD_SUB_PROJECTS(${CMAKE_CURRENT_SOURCE_DIR} FCVMLT)
SET(LIBRARY_OUTPUT_PATH ${CMAKE_BINARY_DIR}/Bin/${CMAKE_BUILD_TYPE} CACHE PATH "Library output path")
SET(EXECUTABLE_OUTPUT_PATH ${CMAKE_BINARY_DIR}/Bin/${CMAKE_BUILD_TYPE} CACHE PATH "Executable output path")
SET(DLIB_PATH $ENV{dlib_ROOT})
set(CMAKE_AUTO_MOC ON)
set(CMAKE_AUTOMOC_MOC_OPTIONS "-bstdafx.h" "-fstdafx.h")



FIND_PACKAGE(Qt5 COMPONENTS Gui Widgets Core OpenGL PrintSupport)
FIND_PACKAGE(OpenCV 2.4.9 EXACT REQUIRED core imgproc highgui nonfree)
set(OpenCV_LIBS opencv_highgui opencv_imgproc opencv_nonfree opencv_core)

FIND_PACKAGE(OpenGL)
if( OPENGL_FOUND)
message(STATUS "Found OpenGL")
INCLUDE_DIRECTORIES(${OpenGL_INCLUDE_DIRS})
LINK_DIRECTORIES(${OpenGL_LIBRARY_DIRS})
ADD_DEFINITIONS(${OpenGL_DEFINITIONS})
#ADD_DEFINITIONS(-DOPENGL)
endif(OPENGL_FOUND)


set(Boost_USE_STATIC_LIBS @Boost_USE_STATIC_LIBS@)
set(Boost_USE_STATIC @Boost_USE_STATIC@)
set(Boost_USE_MULTITHREAD @Boost_USE_MULTITHREAD@)
set(BOOST_INCLUDEDIR "@Boost_INCLUDE_DIR@")
set(BOOST_REQUIRED_MODULES system filesystem thread date_time iostreams chrono)
set(Boost_NO_BOOST_CMAKE ON)
find_package(Boost 1.47.0 REQUIRED COMPONENTS ${BOOST_REQUIRED_MODULES})
#find_package(Qwt)
if(WIN32)
SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /EHsc")
SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /MP")

else(WIN32)
SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++0x")
endif(WIN32)


file(GLOB sources "SRC/*.cpp")# "base/src/*.cpp" "imebra/src/*.cpp")
file(GLOB headers "Headers/*.h")# "base/include/*.h" "imebra/include/*.h")
file(GLOB forms "*UIs/*.ui")

#file(GLOB libs "../build/dicomLoader/Release/*.lib")

QT5_WRAP_UI(UIS ${forms})
QT5_WRAP_CPP(MOC ${sources} ${headers})
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${Qt5Widgets_EXECUTABLE_COMPILE_FLAGS}")

#add_definitions(-DKEITH_BUILD)
add_definitions(-DPUNTOEXE_WINDOWS)
add_definitions(-DNOMINMAX)

source_group("Forms" 		FILES ${forms})
source_group("UI Headers" 	FILES ${UIS})
source_group("MOC'd files" 	FILES ${MOC})
source_group("Linked Libraries" FILES ${QT_LIBRARIES})# ${OpenCV_LIBS} ${Boost_LIBRARIES} Qt5::Gui Qt5::Widgets Qt5::Core Qt5::OpenGL Qt5::PrintSupport ${OPENGL_LIBRARIES} ${SUBPROJECT_LIBS})

INCLUDE_DIRECTORIES(${CMAKE_CURRENT_SOURCE_DIR}/Headers)
INCLUDE_DIRECTORIES(${Boost_INCLUDE_DIR})
INCLUDE_DIRECTORIES(${OPENCV_INCLUDE_DIR})
INCLUDE_DIRECTORIES(${Qt5Gui_INCLUDE_DIRS})
INCLUDE_DIRECTORIES(${Qt5Core_INCLUDE_DIRS})
INCLUDE_DIRECTORIES(${Qt5PrintSupport_INCLUDE_DIRS})
INCLUDE_DIRECTORIES(${Qt5Widgets_INCLUDE_DIRS})
INCLUDE_DIRECTORIES(${Qt5OpenGL_INCLUDE_DIRS})
INCLUDE_DIRECTORIES(${DLIB_PATH})


ADD_EXECUTABLE(FCVMLT 
	${sources} 
	${MOC}
	${headers_moc} 
	${headers} 
	${UIS}
)
foreach(P ${SUBPROJECTS})
        add_dependencies(FCVMLT ${P})
        message("Adding Dependency: ${P}")
endforeach(P)

TARGET_LINK_LIBRARIES(FCVMLT 
	${QT_LIBRARIES} 
	${OpenCV_LIBS} 
	${Boost_LIBRARIES}
	Qt5::Gui
	Qt5::Widgets
	Qt5::Core
	Qt5::OpenGL
	Qt5::PrintSupport
	${dicomLoader}
	${OPENGL_LIBRARIES}
	${SUBPROJECT_LIBS}
)

# Add test projects
file(GLOB dirs "Test/*")
#message("Building test projects for: ")
foreach(dir ${dirs})
		if(IS_DIRECTORY ${dir})
			if(NOT ${dir} STREQUAL ${CMAKE_CURRENT_SOURCE_DIR}/CMakeFiles)
				#message(${dir})
				#add_subdirectory(${dir})
				#include_directories(${dir})
				#add_subdirectory(${dir})
			endif()
		endif()
endforeach(dir)
