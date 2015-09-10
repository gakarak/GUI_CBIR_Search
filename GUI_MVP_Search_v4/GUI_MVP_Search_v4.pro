#-------------------------------------------------
#
# Project created by QtCreator 2013-08-09T10:47:13
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET   = GUI_MVP_Search_v4
TEMPLATE = app

PRJ_DIR = $$PWD
DESTDIR = ${PRJ_DIR}/../../../bin

unix {
    QMAKE_CXXFLAGS  += -fopenmp
    QMAKE_LFLAGS    += -fopenmp
} else {
    QMAKE_CXXFLAGS  += /fopenmp
    QMAKE_LFLAGS    += /fopenmp
}

INCLUDEPATH += include
INCLUDEPATH += ../DSC_Plugin_Interface

unix {
    INCLUDEPATH += /home/ar/dev/opencv-2.4.9/include/opencv /home/ar/dev/opencv-2.4.9/include
    LIBS        += -L/home/ar/dev/opencv-2.4.9/lib  -lopencv_calib3d -lopencv_contrib -lopencv_core -lopencv_features2d -lopencv_flann -lopencv_gpu -lopencv_highgui -lopencv_imgproc -lopencv_legacy -lopencv_ml -lopencv_nonfree -lopencv_objdetect -lopencv_photo -lopencv_stitching -lopencv_superres -lopencv_ts -lopencv_video -lopencv_videostab

#    INCLUDEPATH += /home/ar/dev/opencv-2.4.6.1-cuda-release/include/opencv /home/ar/dev/opencv-2.4.6.1-cuda-release/include
#    LIBS        += -L/home/ar/dev/opencv-2.4.6.1-cuda-release/lib  -lopencv_calib3d -lopencv_contrib -lopencv_core -lopencv_features2d -lopencv_flann -lopencv_gpu -lopencv_highgui -lopencv_imgproc -lopencv_legacy -lopencv_ml -lopencv_nonfree -lopencv_objdetect -lopencv_photo -lopencv_stitching -lopencv_superres -lopencv_ts -lopencv_video -lopencv_videostab

#    INCLUDEPATH += /home/ar/dev/opencv-2.4.6.1-release/include/opencv /home/ar/dev/opencv-2.4.6.1-release/include
#    LIBS        += -L/home/ar/dev/opencv-2.4.6.1-release/lib  -lopencv_calib3d -lopencv_contrib -lopencv_core -lopencv_features2d -lopencv_flann -lopencv_gpu -lopencv_highgui -lopencv_imgproc -lopencv_legacy -lopencv_ml -lopencv_nonfree -lopencv_objdetect -lopencv_photo -lopencv_stitching -lopencv_superres -lopencv_ts -lopencv_video -lopencv_videostab
} else {
    INCLUDEPATH += e:/dev/opencv/build/include/opencv e:/dev/opencv/build/include 
    LIBS        += -Le:/dev/opencv/build/x86/vc10/lib  -lopencv_core -lopencv_highgui -lopencv_imgproc
}

#INCLUDEPATH     += /home/ar/dev/pHash-0.9.6/include
#LIBS            += /home/ar/dev/pHash-0.9.6/lib/libpHash.a


SOURCES += main.cpp\
        mainwindow.cpp \
    model2.cpp \
    src/mvptree_mod.cpp \
#    src/lib_descriptor2sl.cpp \
    src/lib_utils.cpp \
    draglabel.cpp \
    draglistview.cpp \
    workerthread.cpp \
    descriptorcalculator.cpp \
    workerthreadsorter.cpp \
    sortedindex.cpp
#    src/dscplugincoomatrix.cpp
#    src/dscplugininterface.cpp

HEADERS  += mainwindow.h \
    model2.h \
    include/mvptree_mod.h \
#    include/lib_descriptor2sl.h \
    include/lib_utils.h \
    draglabel.h \
    draglistview.h \
    workerthread.h \
    descriptorcalculator.h \
    workerthreadsorter.h \
    sortedindex.h \
    ../DSC_Plugin_Interface/dscplugininterface.h
#    include/dscplugincoomatrix.h \


!unix {
    SOURCES += src/mman.c
    HEADERS += include/mman.h
}

FORMS    += mainwindow.ui

RESOURCES += \
    images.qrc
