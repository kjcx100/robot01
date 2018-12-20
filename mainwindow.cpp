#include <QSettings>
#include <QTextCodec>
#include <QtDebug>
#include <QString>

#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "include/image_process.h"
static int Timer_Count = 0;

extern cv::Mat g_cvimg;
extern CAMMER_PARA_S g_SysParam;
extern QString		curr_path;

QImage		   g_VideoImage;
QImage		   g_VideoImageOut;

ImageProcessThread m_ImageProcessThread;

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    //ui->RawImg->show();
    //ui->OutImg->show();
	pVideoImage = new PictureBox(this);
	if(pVideoImage != NULL)
	{	        
		pVideoImage->setMode((AUTO_FILL));
        pVideoImage->hide();
	}
    //Time  1s
    pTimer500ms = new QTimer(this);
    if(pTimer500ms)
    {
        connect( pTimer500ms, SIGNAL(timeout()), this, SLOT(call_timerDone_500ms()));
        pTimer500ms->start(500);              // 500ms单触发定时器
    }
    //Time  1s
    pTimer1S = new QTimer(this);
    if(pTimer1S)
    {
        connect( pTimer1S, SIGNAL(timeout()), this, SLOT(call_timerDone_1s()) );
        //pTimer1S->start(1000);              // 1秒单触发定时器
    }
    m_ImageProcessThread.start();
	qDebug("m_ImageProcessThread start!!!%d\n",__LINE__);
	connect(&m_ImageProcessThread, SIGNAL(EmitFrameMessage(cv::Mat*, int)), this, SLOT(EmitFrameMessage(cv::Mat*, int)));
	connect(&m_ImageProcessThread, SIGNAL(EmitOutFrameMessage(cv::Mat*, int)), this, SLOT(EmitOutFrameMessage(cv::Mat*, int)));
}

void MainWindow::call_timerDone_500ms()
{
    printf("call_timerDone_500ms:: line=%d\n",__LINE__);
    pTimer500ms->stop();
   // m_ImageProcessThread.start();
    //Init();
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_TempBtn_clicked()
{
    //调用窗口打开文件
    QString filename = QFileDialog::getOpenFileName(this,
                                                    tr("open image"),
                                                    ".",
                                                    tr("Image file(*.png *.jpg *.bmp)"));
    qDebug("filename=%s\n",filename.toLocal8Bit().data());
    image = cv::imread(filename.toLocal8Bit().data(),1);
	if(image.data) {
	    //    ui->pushButton_2->setEnabled(true);
	    //将Mat图像转换为QImage图像，才能显示在label上
	    QImage img = QImage((const unsigned char*)(image.data),
	                        image.cols, image.rows, QImage::Format_RGB888);
	    //设定图像大小自适应label窗口的大小
	    img = img.scaled(ui->RawImg->size(), Qt::IgnoreAspectRatio, Qt::SmoothTransformation);
	    ui->RawImg->setPixmap(QPixmap::fromImage(img));
	}
}
void MainWindow::call_timerDone_1s()
{
    char szFilename[32] = {0};

    Timer_Count++;
    qDebug("Timer_Count==%d\n",Timer_Count);
    sprintf(szFilename,"./%d.jpg",Timer_Count);
    qDebug(" call_timerDone_1s szFilename==%s\n",szFilename);
    Mat image = cv::imread(szFilename);
    SetImageData(image.data,image.cols, image.rows);
    SetImageMat(&g_cvimg);
    if(Timer_Count > 10)
        Timer_Count = 0;

}

void MainWindow::SetImageData(uchar* data,int imgcols,int imgrows)
{
    if(data != NULL) {
        //将Mat图像转换为QImage图像，才能显示在label上
        QImage img = QImage((const unsigned char*)(data),imgcols, imgrows, QImage::Format_RGB888);
        //设定图像大小自适应label窗口的大小
        //aaasss;
        img = img.scaled(ui->RawImg->size(), Qt::IgnoreAspectRatio, Qt::SmoothTransformation);
        ui->RawImg->setPixmap(QPixmap::fromImage(img));
    }
}
void MainWindow::SetImageQimage(QImage *img)
{
    if(img != NULL) {
        //设定图像大小自适应label窗口的大小
        img->scaled(ui->RawImg->size(), Qt::IgnoreAspectRatio, Qt::SmoothTransformation);
        ui->RawImg->setPixmap(QPixmap::fromImage(*img));
    }
}
void MainWindow::SetImageOutImg(QImage *img)
{
    if(img != NULL) {
        //设定图像大小自适应label窗口的大小
        img->scaled(ui->OutImg->size(), Qt::IgnoreAspectRatio, Qt::SmoothTransformation);
        ui->OutImg->setPixmap(QPixmap::fromImage(*img));
    }
}

void MainWindow::SetImageMat(Mat *cvmat)
{
    if(cvmat->data != NULL) {
        //将Mat图像转换为QImage图像，才能显示在label上
        QImage img = QImage((const unsigned char*)(cvmat->data),cvmat->cols, cvmat->rows, QImage::Format_RGB888);
        //设定图像大小自适应label窗口的大小
        //aaasss;
        img = img.scaled(ui->RawImg->size(), Qt::IgnoreAspectRatio, Qt::SmoothTransformation);
        ui->OutImg->setPixmap(QPixmap::fromImage(img));
    }
}
void MainWindow::EmitFrameMessage(cv::Mat* stFrameItem, int nCh)
{
	printf("###[%s][%d],type=%d in!!!\n", __func__, __LINE__,stFrameItem->type());
	if(pVideoImage == NULL)
		return;
	//g_VideoImage = g_Video_cBtoQI[nCh].BMP24ToQImage24(szBmp + 54, stFrameItem.dwWidth, stFrameItem.dwHeight, stFrameItem.dwWidth * 3, 0);
	g_VideoImage = QImage((const unsigned char*)(stFrameItem->data),stFrameItem->cols, stFrameItem->rows, QImage::Format_RGB888);
	//g_VideoImage = g_VideoImage.mirrored(false, true);
    SetImageQimage(&g_VideoImage);
}
void MainWindow::EmitOutFrameMessage(cv::Mat* stFrameItem, int nCh)
{
	printf("###[%s][%d],type=%d in!!!\n", __func__, __LINE__,stFrameItem->type());
	if(pVideoImage == NULL)
		return;
	//g_VideoImage = g_Video_cBtoQI[nCh].BMP24ToQImage24(szBmp + 54, stFrameItem.dwWidth, stFrameItem.dwHeight, stFrameItem.dwWidth * 3, 0);
	g_VideoImageOut = QImage((const unsigned char*)(stFrameItem->data),stFrameItem->cols, stFrameItem->rows, QImage::Format_RGB888);
	//g_VideoImage = g_VideoImage.mirrored(false, true);
    SetImageOutImg(&g_VideoImageOut);
}
void MainWindow::on_SaveBtn_clicked()
{
	printf("###[%s][%d], in!!!\n", __func__, __LINE__);
	CAMMER_PARA_S st_SysParam;
	GetCammerSetParam(&st_SysParam);
    QByteArray line_Qby;
    float line_getNum = 0;
	int line_getInt = 0;	
    line_Qby = ui->EditHor_View->text().toLatin1();
    line_getNum = atof(line_Qby);
	st_SysParam.EditHor_View = line_getNum;
	line_Qby = ui->EditVer_View->text().toLatin1();
    line_getNum = atof(line_Qby);
	st_SysParam.EditVer_View = line_getNum;
	line_Qby = ui->EditHor_Angl->text().toLatin1();
    line_getNum = atof(line_Qby);
	st_SysParam.EditHor_Angl = line_getNum;
	line_Qby = ui->EditVer_Angl->text().toLatin1();
    line_getNum = atof(line_Qby);
	st_SysParam.EditVer_Angl = line_getNum;
	line_Qby = ui->calib_horizon->text().toLatin1();
    line_getNum = atof(line_Qby);
	st_SysParam.calib_horizon = line_getNum;
	line_Qby = ui->calib_vertical->text().toLatin1();
    line_getNum = atof(line_Qby);
	st_SysParam.calib_vertical = line_getNum;
	//int
	line_Qby = ui->PixWidth_Start->text().toLatin1();
    line_getInt = atoi(line_Qby);
	st_SysParam.PixWidth_Start = line_getInt;
	line_Qby = ui->PixWidth_End->text().toLatin1();
    line_getInt = atoi(line_Qby);
	st_SysParam.PixWidth_End = line_getInt;
	line_Qby = ui->PixHight_Start->text().toLatin1();
    line_getInt = atoi(line_Qby);
	st_SysParam.PixHight_Start = line_getInt;
	line_Qby = ui->PixHight_End->text().toLatin1();
    line_getInt = atoi(line_Qby);
	st_SysParam.PixHight_End = line_getInt;
	line_Qby = ui->GussBlurSize->text().toLatin1();
    line_getInt = atoi(line_Qby);
	st_SysParam.GussBlurSize = line_getInt;
	line_Qby = ui->MidBlurSize->text().toLatin1();
    line_getInt = atoi(line_Qby);
	st_SysParam.MidBlurSize = line_getInt;

	line_Qby = ui->threshold_val->text().toLatin1();
    line_getInt = atoi(line_Qby);
	st_SysParam.threshold_val = line_getInt;
	line_Qby = ui->Gain_thold_min->text().toLatin1();
    line_getInt = atoi(line_Qby);
	st_SysParam.Gain_thold_min = line_getInt;
	line_Qby = ui->Gain_thold_max->text().toLatin1();
    line_getInt = atoi(line_Qby);
	st_SysParam.Gain_thold_max = line_getInt;
	line_Qby = ui->Gain_Fre->text().toLatin1();
    line_getInt = atoi(line_Qby);
	st_SysParam.Gain_Fre = line_getInt;
	line_Qby = ui->Gain_min->text().toLatin1();
    line_getInt = atoi(line_Qby);
	st_SysParam.Gain_min = line_getInt;
	line_Qby = ui->Gain_max->text().toLatin1();
    line_getInt = atoi(line_Qby);
	st_SysParam.Gain_max = line_getInt;
	
    qDebug("###[%s][%d],line_getNum==%f\n", __func__, __LINE__, line_getNum);
	SetCammerSetParamFile(&st_SysParam);
}

int MainWindow::SetUIDispParam(CAMMER_PARA_S* pstparam)
{
	if(pstparam == NULL)
        return -1;
	CAMMER_PARA_S stparam ; 
	memcpy(& stparam, pstparam, sizeof(CAMMER_PARA_S));
	QString str;
	str.sprintf("%.3f",stparam.EditHor_View);
	ui->EditHor_View->setText(str);
    return 0;
}

ImageDispThread::ImageDispThread(QObject *parent) :
    QThread(parent)
{
    bStop = false;
}

void ImageDispThread::stop()
{
    bStop = true;

}
void ImageDispThread::run()
{
    qDebug("ImageDispThread::run=%d\n",__LINE__);
    MainWindow w;
    w.show();
}

////////////参数保存

int GetCammerSetParam(CAMMER_PARA_S* pstparam)
{
	if(pstparam == NULL)
        return -1;
    
    memcpy(pstparam, &g_SysParam, sizeof(CAMMER_PARA_S));    

    return 0;
}

int GetCammerSetParamFile(CAMMER_PARA_S* pstparam)
{
    QSettings  *pIniFile = NULL;
	if(pstparam == NULL)
	{
		qDebug( "GetCammerSetParamFile err  = NULL\n");
		return -2;
	}
	QString  fileName = curr_path + "/Config/SysParam.ini";

    pIniFile = new QSettings(fileName, QSettings::IniFormat);
    if(pIniFile != NULL)
    {
        pIniFile->setIniCodec(QTextCodec::codecForName("UTF8"));
    }
//	SS_SYSLOG(LOG_INFO, "GetHttPSubscribeParamFile in file = %s curr_path = %s\n",	qPrintable(fileName), qPrintable(curr_path));
	QString sTemp;
    if(pIniFile != NULL)
    {

		pstparam->EditHor_View 		= pIniFile->value("/CammerParam/EditHor_View", "0").toFloat();
		pstparam->EditVer_View 		= pIniFile->value("/CammerParam/EditVer_View", "0").toFloat();
		pstparam->PixWidth_Start 	= pIniFile->value("/CammerParam/PixWidth_Start", "0").toInt();		
		pstparam->PixWidth_End	 	= pIniFile->value("/CammerParam/PixWidth_End", "0").toInt();
		pstparam->PixHight_Start 	= pIniFile->value("/CammerParam/PixHight_Start", "0").toInt();
		pstparam->PixHight_End 		= pIniFile->value("/CammerParam/PixHight_End", "0").toInt();
		pstparam->EditHor_Angl 		= pIniFile->value("/CammerParam/EditHor_Angl", "0").toFloat();
		pstparam->EditVer_Angl 		= pIniFile->value("/CammerParam/EditVer_Angl", "0").toFloat();
		//滤波参数
		pstparam->GussBlurSize	 	= pIniFile->value("/CammerParam/GussBlurSize", "0").toInt();
		pstparam->MidBlurSize 		= pIniFile->value("/CammerParam/MidBlurSize", "0").toInt();
		pstparam->calib_horizon 	= pIniFile->value("/CammerParam/calib_horizon", "0").toFloat();
		pstparam->calib_vertical	= pIniFile->value("/CammerParam/calib_vertical", "0").toFloat();
		pstparam->threshold_val	 	= pIniFile->value("/CammerParam/threshold_val", "0").toInt();
		//增益参数
		pstparam->Gain_thold_min	= pIniFile->value("/CammerParam/Gain_thold_min", "0").toInt();
		pstparam->Gain_thold_max	= pIniFile->value("/CammerParam/Gain_thold_max", "0").toInt();
		pstparam->Gain_Fre		 	= pIniFile->value("/CammerParam/Gain_Fre", "0").toInt();
		pstparam->Gain_min	 		= pIniFile->value("/CammerParam/Gain_min", "0").toInt();
		pstparam->Gain_max	 		= pIniFile->value("/CammerParam/Gain_max", "0").toInt();
        //printf("GetDynamicPassParamFile,pDynamicPass=%s\n",pDynamicPass);
		//printf("GetDynamicPassParamFile,pszTime=%s\n",pszTime);
        delete pIniFile;
        pIniFile = NULL;
        return 0;
    }
	else
	{
		qDebug( "GetDynamicPassParamFile err %s curr_path = %s\n",qPrintable(fileName), qPrintable(curr_path));
	}

    return -1;
}

int SetCammerSetParamFile(CAMMER_PARA_S* pstparam)
{
    QSettings  *pIniFile = NULL;
	if(pstparam == NULL)
	{
		qDebug("SetHttpPostParamFile err  = NULL\n");
		return -2;
	}
	QString  fileName = curr_path + "/Config/SysParam.ini";

    pIniFile = new QSettings(fileName, QSettings::IniFormat);
    if(pIniFile != NULL)
    {
        pIniFile->setIniCodec(QTextCodec::codecForName("UTF8"));
    }

	qDebug( "SetHttpPostParamFile in file = %s curr_path = %s\n",	qPrintable(fileName), qPrintable(curr_path));
	memcpy(&g_SysParam, pstparam, sizeof(CAMMER_PARA_S)); 

    if(pIniFile != NULL)
    {
	    pIniFile->setValue("/CammerParam/EditHor_View", pstparam->EditHor_View);
		pIniFile->setValue("/CammerParam/EditVer_View", pstparam->EditVer_View);
		pIniFile->setValue("/CammerParam/PixWidth_Start", pstparam->PixWidth_Start);
		pIniFile->setValue("/CammerParam/PixWidth_End", pstparam->PixWidth_End);
		pIniFile->setValue("/CammerParam/PixHight_Start", pstparam->PixHight_Start);
		pIniFile->setValue("/CammerParam/PixHight_End", pstparam->PixHight_End);
		pIniFile->setValue("/CammerParam/EditHor_Angl", pstparam->EditHor_Angl);
		pIniFile->setValue("/CammerParam/EditVer_Angl", pstparam->EditVer_Angl);
		
		pIniFile->setValue("/CammerParam/GussBlurSize", pstparam->GussBlurSize);
		pIniFile->setValue("/CammerParam/MidBlurSize", pstparam->MidBlurSize);
		pIniFile->setValue("/CammerParam/calib_horizon", pstparam->calib_horizon);
		pIniFile->setValue("/CammerParam/calib_vertical", pstparam->calib_vertical);
		pIniFile->setValue("/CammerParam/threshold_val", pstparam->threshold_val);
		
		pIniFile->setValue("/CammerParam/Gain_thold_min", pstparam->Gain_thold_min);
		pIniFile->setValue("/CammerParam/Gain_thold_max", pstparam->Gain_thold_max);
		pIniFile->setValue("/CammerParam/Gain_Fre", pstparam->Gain_Fre);
		pIniFile->setValue("/CammerParam/Gain_min", pstparam->Gain_min);
		pIniFile->setValue("/CammerParam/Gain_max", pstparam->Gain_max);		
		
        delete pIniFile;
        pIniFile = NULL;
        return 0;
    }
	else
	{
		qDebug( "SetDynamicPassParamFile err %s curr_path = %s\n",qPrintable(fileName), qPrintable(curr_path));
	}

    return -1;
}



