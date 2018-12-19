#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "include/image_process.h"
static int Timer_Count = 0;

extern cv::Mat g_cvimg;
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
	printf("###[%s][%d], in!!!\n", __func__, __LINE__);
	if(pVideoImage == NULL)
		return;
	//g_VideoImage = g_Video_cBtoQI[nCh].BMP24ToQImage24(szBmp + 54, stFrameItem.dwWidth, stFrameItem.dwHeight, stFrameItem.dwWidth * 3, 0);
	g_VideoImage = QImage((const unsigned char*)(stFrameItem->data),stFrameItem->cols, stFrameItem->rows, QImage::Format_RGB888);
	//g_VideoImage = g_VideoImage.mirrored(false, true);
    SetImageQimage(&g_VideoImage);
}
void MainWindow::EmitOutFrameMessage(cv::Mat* stFrameItem, int nCh)
{
	printf("###[%s][%d], in!!!\n", __func__, __LINE__);
	if(pVideoImage == NULL)
		return;
	//g_VideoImage = g_Video_cBtoQI[nCh].BMP24ToQImage24(szBmp + 54, stFrameItem.dwWidth, stFrameItem.dwHeight, stFrameItem.dwWidth * 3, 0);
	g_VideoImageOut = QImage((const unsigned char*)(stFrameItem->data),stFrameItem->cols, stFrameItem->rows, QImage::Format_RGB888);
	//g_VideoImage = g_VideoImage.mirrored(false, true);
    SetImageOutImg(&g_VideoImageOut);
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
