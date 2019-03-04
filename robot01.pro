#-------------------------------------------------
#
# Project created by QtCreator 2018-12-06T22:57:59
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = robot01
TEMPLATE = app

# The following define makes your compiler emit warnings if you use
# any feature of Qt which has been marked as deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0


INCLUDEPATH += /usr/include/opencv  \
            /usr/include/opencv2
 #           /usr/include/curl/      \
 #           /usr/include/jsoncpp/json/

LIBS += -L /usr/local/lib/libopencv_* \
        /usr/lib/x86_64-linux-gnu/ -lcurl -ljsoncpp
 #       /usr/lib/x86_64-linux-gnu/lcurl

FORMS += \
        mainwindow.ui

RESOURCES += \
    bg_img/bg_img.qrc

DISTFILES +=

HEADERS += \
    define.h \
    common/common.hpp \
    common/DepthRender.hpp \
    common/MatViewer.hpp \
    common/PointCloudViewer.hpp \
    common/Utils.hpp \
    include/TY_API.h \
    include/TYImageProc.h \
    define.h \
    mainwindow.h \
    include/image_process.h \
    include/image_process.h \
    include/picturebox.h

SOURCES += \
    main.cpp \
    mainwindow.cpp \
    image_process.cpp \
    picturebox.cpp

win32:CONFIG(release, debug|release): LIBS += -L$$PWD/lib_x64/release/ -ltycam
else:win32:CONFIG(debug, debug|release): LIBS += -L$$PWD/lib_x64/debug/ -ltycam
else:unix: LIBS += -L$$PWD/lib_x64/ -ltycam

INCLUDEPATH += $$PWD/lib_x64
DEPENDPATH += $$PWD/lib_x64
