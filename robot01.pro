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

ROS_DIR = /opt/ros/kinetic/
DDS_ROOT_DIR  = /software/OpenDDS-3.13

INCLUDEPATH += /usr/include/opencv  \
            /usr/include/opencv2
INCLUDEPATH += $$PWD/camer-obs-lib/include	\
			$$PWD/camer-obs-lib/include/opendds \
			$$PWD/camer-obs-lib/include/opendds/idl		\
			$$PWD/camer-obs-lib/include/ros	\
			$$ROS_DIR	\
			$$ROS_DIR/include	\
			$$DDS_ROOT_DIR/ACE_wrappers	\
			$$DDS_ROOT_DIR//ACE_wrappers/TAO
			
			
 #           /usr/include/curl/      \
 #           /usr/include/jsoncpp/json/

LIBS += -L /usr/local/lib/libopencv_* \
        -L/usr/lib/x86_64-linux-gnu/ -lcurl -ljsoncpp
LIBS += -L$$PWD/camer-obs-lib/libs -lcamera-obs	\
		-L$$ROS_DIR/lib -lroscpp -lrosconsole -lroscpp_serialization -lrostime	\
		-L$$DDS_ROOT_DIR/lib -lOpenDDS_Dcps -lOpenDDS_FACE -lOpenDDS_Federator -lOpenDDS_InfoRepoDiscovery -lOpenDDS_InfoRepoLib -lOpenDDS_InfoRepoServ	\
		-lOpenDDS_Model -lOpenDDS_monitor -lOpenDDS_Multicast -lOpenDDS_Rtps -lOpenDDS_Rtps_Udp -lOpenDDS_Shmem -lOpenDDS_Tcp -lOpenDDS_Udp -lSyncClient	\
		-lSyncServer -lTestFramework \
		-L$$DDS_ROOT_DIR/ACE_wrappers/lib -lACE -lACEXML_Parser -lACEXML -lTAO_AnyTypeCode -lTAO_Async_ImR_Client_IDL -lTAO_Async_IORTable -lTAO_BiDirGIOP \
		-lTAO_CodecFactory -lTAO_Codeset -lTAO_CosNaming_Serv -lTAO_CosNaming_Skel -lTAO_CosNaming -lTAO_CSD_Framework -lTAO_CSD_ThreadPool -lTAO_DynamicInterface \
		-lTAO_FT_ClientORB -lTAO_FTORB_Utils -lTAO_FT_ServerORB -lTAO_IDL_BE -lTAO_IDL_FE -lTAO_ImR_Activator_IDL -lTAO_ImR_Activator -lTAO_ImR_Client \
		-lTAO_ImR_Locator_IDL -lTAO_ImR_Locator -lTAO_IORManip -lTAO_IORTable -lTAO_Messaging -lTAO_PI_Server -lTAO_PI -lTAO_PortableGroup -lTAO_PortableServer \
		-lTAO -lTAO_Strategies -lTAO_Svc_Utils -lTAO_Valuetype 

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
SOURCES += $$PWD/camer-obs-lib/src/opendds/ddsothersensordata.cpp	\
		$$PWD/camer-obs-lib/src/opendds/ddsothersensordatalistenerimpl.cpp	\
		$$PWD/camer-obs-lib/src/ros/othersensordata.cpp

win32:CONFIG(release, debug|release): LIBS += -L$$PWD/lib_x64/release/ -ltycam
else:win32:CONFIG(debug, debug|release): LIBS += -L$$PWD/lib_x64/debug/ -ltycam
else:unix: LIBS += -L$$PWD/lib_x64/ -ltycam

INCLUDEPATH += $$PWD/lib_x64
DEPENDPATH += $$PWD/lib_x64
