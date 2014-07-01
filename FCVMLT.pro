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


SOURCES += SRC/main.cpp\
        SRC/mainwindow.cpp \
    SRC/container.cpp \
    SRC/filterwidget.cpp \
    SRC/imageedit.cpp \
    SRC/imgsourceswidget.cpp \
    SRC/imgFilter.cpp \
    SRC/filterselectwidget.cpp \
    SRC/filtersettingwidget.cpp \
    SRC/imgstatswidget.cpp \
    SRC/statsettingwidget.cpp \
    SRC/matview.cpp \
    SRC/mlwidget.cpp \
    SRC/compoundimgfunction.cpp \
    SRC/qtcontainer.cpp \
    SRC/savestreamdialog.cpp \
    SRC/registrationdialog.cpp
HEADERS  += headers/mainwindow.h \
    headers/container.h \
    headers/filterwidget.h \
    headers/imageedit.h \
    headers/imgsourceswidget.h \
    headers/imgFilter.h \
    headers/filterselectwidget.h \
    headers/filtersettingwidget.h \
    headers/imgstatswidget.h \
    headers/statsettingwidget.h \
    headers/matview.h \
    headers/mlwidget.h \
    headers/compoundimgfunction.h \
    headers/qtcontainer.h \
    headers/savestreamdialog.h \
    headers/registrationdialog.h

FORMS    += UIs/mainwindow.ui \
    UIs/filterwidget.ui \
    UIs/imageedit.ui \
    UIs/imgsourceswidget.ui \
    UIs/filterselectwidget.ui \
    UIs/filtersettingwidget.ui \
    UIs/imgstatswidget.ui \
    UIs/statsettingwidget.ui \
    UIs/mlwidget.ui \
    UIs/compoundimgfunction.ui \
    UIs/savestreamdialog.ui \
    UIs/registrationdialog.ui


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
