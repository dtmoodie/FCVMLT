#-------------------------------------------------
#
# Project created by QtCreator 2014-03-12T14:40:57
#
#-------------------------------------------------

QT       += core gui
CONFIG   += console
greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = FCVMLT
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp \
    container.cpp \
    filterwidget.cpp \
    imageedit.cpp \
    imgsourceswidget.cpp \
    imgFilter.cpp \
    filterselectwidget.cpp \
    filtersettingwidget.cpp \
    imgstatswidget.cpp \
    statsettingwidget.cpp \
    matview.cpp \
    mlwidget.cpp \
    compoundimgfunction.cpp \
    qtcontainer.cpp \
    savestreamdialog.cpp

HEADERS  += mainwindow.h \
    container.h \
    filterwidget.h \
    imageedit.h \
    imgsourceswidget.h \
    imgFilter.h \
    filterselectwidget.h \
    filtersettingwidget.h \
    imgstatswidget.h \
    statsettingwidget.h \
    matview.h \
    mlwidget.h \
    compoundimgfunction.h \
    qtcontainer.h \
    savestreamdialog.h

FORMS    += mainwindow.ui \
    filterwidget.ui \
    imageedit.ui \
    imgsourceswidget.ui \
    filterselectwidget.ui \
    filtersettingwidget.ui \
    imgstatswidget.ui \
    statsettingwidget.ui \
    mlwidget.ui \
    compoundimgfunction.ui \
    savestreamdialog.ui


INCLUDEPATH += C:/Opencv248/build/include
INCLUDEPATH += "C:/Program Files/boost/include"
LIBS += -LC:/Opencv248/build/x64/vc10/lib \
        -lopencv_core248d \
        -lopencv_imgproc248d \
        -lopencv_highgui248d \
        -lopencv_contrib248d


LIBS += -L"C:/Program Files/boost/lib" \
        -llibboost_chrono-vc100-mt-gd-1_50 \
        -llibboost_system-vc100-mt-gd-1_50


LIBS += -LE:/LIBS/opencv243/build/x64/vc10/lib \
        -lopencv_core243d \
        -lopencv_imgproc243d \
        -lopencv_highgui243d
