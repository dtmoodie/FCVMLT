cmake_minimum_required(VERSION 2.8)
PROJECT(TEST_CONTAINER)

set(CMAKE_AUTO_MOC ON)
file(GLOB test 		"*.cpp")
file(GLOB sources 	"../../SRC/*.cpp")
file(GLOB headers 	"../../HEADERS/*.cpp")
file(GLOB forms 	"../../UIs/*.ui")

FIND_PACKAGE(Qt5 COMPONENTS Gui Widgets Core)
FIND_PACKAGE(Boost 1.47.0 REQUIRED COMPONENTS 
	unit_test_framework 
	thread 
	date_time 
	iostreams 
	system 
	filesystem 
	chrono
)

INCLUDE_DIRECTORIES(${Boost_INCLUDE_DIR})
LINK_DIRECTORIES(${BOOST_LIBRARY_DIRS})

QT5_WRAP_UI(UIS 
	${forms}
)

QT5_WRAP_CPP(MOC 
	${sources} 	
	${headers}
)
#printfiles(${sources})
#printfiles(${headers})
#printfiles(${forms})
#printfiles(${test})
source_group("Forms" 		FILES ${forms})
source_group("UI Headers" 	FILES ${UIS})
source_group("MOC'd files" 	FILES ${MOC})
source_group("Base files" 	FILES ${sources})
source_group("Base headers" FILES ${headers})

ADD_EXECUTABLE(TEST_CONTAINER 
	${test}
	${headers}
)

target_link_libraries(TEST_CONTAINER 
	${QT_LIBRARIES} 
	${OpenCV_LIBS} 
	${Boost_LIBRARIES}
	Qt5::Gui
	Qt5::Widgets
	Qt5::Core
	Qt5::OpenGL
	${libs}
)
	