#include "mainwindow.h"
#include <QApplication>
#include "define.h"
#include "include/image_process.h"

//ImageProcessThread m_ImageProcessThread;
//ImageDispThread m_ImageDispThread;
BTCommunThread m_BTCommunThread;
CAMMER_PARA_S g_SysParam;
QString		curr_path;



int main(int argc, char *argv[])
{
	QApplication a(argc, argv);
    qDebug("main start!!!%d\n",__LINE__);
    //MainWindow w;
	MainWindow m_mainwin;
    m_mainwin.show();    
	QDir dir;
    curr_path = APP_PATH;			//"/FaceGate";	//dir.currentPath();
	QString	curr_path_tmp = dir.currentPath();
	
    qDebug( "curr path = %s\n",		qPrintable(curr_path));
	qDebug( "curr_path_tmp = %s\n",	qPrintable(curr_path_tmp));
	GetCammerSetParamFile(&g_SysParam);

	m_mainwin.SetUIDispParam(&g_SysParam);
    qDebug("main start!!!%d\n",__LINE__);
    m_BTCommunThread.start();

    //m_ImageDispThread.start();
    //MY_SLEEP_NS(5);

    return a.exec();
}


