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
	GetCammerSysParamFile(&g_SysParam);
	///////////检测参数合法否
	for(int device = 0; device < MAX_DEVNUM -1 ;device++)
	{
		if(g_SysParam.stPerCammer[device].EditVer_Angl < 0 || g_SysParam.stPerCammer[device].EditVer_Angl > 85)
		{
			g_SysParam.stPerCammer[device].EditVer_Angl = 45;
			printf("set EditVer_Angl 45\n");
			//SetCammerSetParamFile(&g_SysParam);
		}
		if(g_SysParam.stPerCammer[device].Edit_instalHeight < 0 || g_SysParam.stPerCammer[device].Edit_instalHeight > 185)
		{
			g_SysParam.stPerCammer[device].Edit_instalHeight = 45;
			printf("set Edit_instalHeight 45\n");
			//SetCammerSetParamFile(&g_SysParam);
		}
		if(g_SysParam.stPerCammer[device].Temp_threshold < 0.01 || g_SysParam.stPerCammer[device].Temp_threshold > 10)
		{
			g_SysParam.stPerCammer[device].Temp_threshold = 0.03;
			printf("set Temp_threshold 0.03\n");
			//SetCammerSetParamFile(&g_SysParam);
		}
		if(g_SysParam.stPerCammer[device].Gain_max < 10)
		{
			g_SysParam.stPerCammer[device].Gain_max = 250;
			printf("set Gain_max 250\n");
			//SetCammerSetParamFile(&g_SysParam);
		}
		if(g_SysParam.stPerCammer[device].Gain_thold_max < 10)
		{
			g_SysParam.stPerCammer[device].Gain_thold_max = 100;
			printf("set Gain_thold_max 100\n");
			//SetCammerSetParamFile(&g_SysParam);
		}
		if(g_SysParam.stPerCammer[device].GussBlurSize < 1)
		{
			g_SysParam.stPerCammer[device].GussBlurSize = 5;
			printf("set GussBlurSize 5\n");
		}
		if(g_SysParam.stPerCammer[device].MidBlurSize < 1)
		{
			g_SysParam.stPerCammer[device].MidBlurSize = 5;
			printf("set MidBlurSize 5\n");
		}
	}
	SetCammerSetParamFile(&g_SysParam);
	m_mainwin.SetUIDispParam(&g_SysParam);
    qDebug("main start!!!%d\n",__LINE__);
    //m_BTCommunThread.start();

    //m_ImageDispThread.start();
    //MY_SLEEP_NS(5);

    return a.exec();
}


