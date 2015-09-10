#-------------------------------------------------
#
# Project created by QtCreator 2013-10-17T18:33:15
#
#-------------------------------------------------

QT       -= gui

TEMPLATE     = lib
TARGET       = dscplugin_bfpca
CONFIG      += plugin

DEFINES     += DSCPLUGIN_BFPCA_LIBRARY
VERSION     += 1.0.0

INCLUDEPATH += ./include
INCLUDEPATH += ../../DSC_Plugin_Interface


###
PRJ_DIR = $$PWD
DESTDIR = ${PRJ_DIR}/../../../../bin/dsc
install_it.path   = ${PRJ_DIR}/../../bin/dsc
install_it.files += libdscplugin_bfpca.cfg
INSTALLS += install_it

unix {
    INCLUDEPATH += /home/ar/dev/opencv-2.4.9/include/opencv /home/ar/dev/opencv-2.4.9/include
    LIBS        += -L/home/ar/dev/opencv-2.4.9/lib  -lopencv_calib3d -lopencv_contrib -lopencv_core -lopencv_features2d -lopencv_flann -lopencv_gpu -lopencv_highgui -lopencv_imgproc -lopencv_legacy -lopencv_ml -lopencv_nonfree -lopencv_objdetect -lopencv_photo -lopencv_stitching -lopencv_superres -lopencv_ts -lopencv_video -lopencv_videostab

#    INCLUDEPATH += /home/ar/dev/opencv-2.4.6.1-cuda-release/include/opencv /home/ar/dev/opencv-2.4.6.1-cuda-release/include
#    LIBS        += -L/home/ar/dev/opencv-2.4.6.1-cuda-release/lib  -lopencv_calib3d -lopencv_contrib -lopencv_core -lopencv_features2d -lopencv_flann -lopencv_gpu -lopencv_highgui -lopencv_imgproc -lopencv_legacy -lopencv_ml -lopencv_nonfree -lopencv_objdetect -lopencv_photo -lopencv_stitching -lopencv_superres -lopencv_ts -lopencv_video -lopencv_videostab

#    INCLUDEPATH += /home/ar/dev/opencv-2.4.6.1-release/include/opencv /home/ar/dev/opencv-2.4.6.1-release/include
#    LIBS        += -L/home/ar/dev/opencv-2.4.6.1-release/lib  -lopencv_calib3d -lopencv_contrib -lopencv_core -lopencv_features2d -lopencv_flann -lopencv_gpu -lopencv_highgui -lopencv_imgproc -lopencv_legacy -lopencv_ml -lopencv_nonfree -lopencv_objdetect -lopencv_photo -lopencv_stitching -lopencv_superres -lopencv_ts -lopencv_video -lopencv_videostab
} else {
#    INCLUDEPATH += e:/dev/opencv/build/include/opencv e:/dev/opencv/build/include
#    LIBS        += -Le:/dev/opencv/build/x86/vc10/lib  -lopencv_core -lopencv_highgui -lopencv_imgproc
    INCLUDEPATH += c:/dev/opencv/build/include c:/dev/opencv/build/include/opencv
#    LIBS        += -Lc:/dev/opencv/build/x86/vc10/lib  -lopencv_core246d -lopencv_highgui246d -lopencv_imgproc246d
    LIBS        += -Lc:/dev/opencv/build/x86/vc10/lib  -lopencv_core246 -lopencv_highgui246 -lopencv_imgproc246
}

SOURCES +=  src/dscpluginbfpca.cpp \
            src/lib_helpers.cpp \
            src/lib_fpcan.cpp \
            src/lib_fpcan_dsc.cpp

HEADERS +=  include/dscpluginbfpca.h \
            include/dscpluginbfpca_global.h \
            include/lib_helpers.h \
            include/lib_fpcan.h \
            include/lib_fpcan_dsc.h \
            ../../DSC_Plugin_Interface/dscplugininterface.h

OTHER_FILES += \
    dscplugin_bfpca.json \
    libdscplugin_bfpca.cfg
