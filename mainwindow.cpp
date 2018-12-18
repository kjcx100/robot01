#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "include/image_process.h"
static int Timer_Count = 0;

extern Mat g_cvimg;
//ImageProcessThread m_ImageProcessThread;

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    //ui->RawImg->show();
    //ui->OutImg->show();
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
        pTimer1S->start(1000);              // 1秒单触发定时器
    }

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
    //if(image.data) {
    //    ui->pushButton_2->setEnabled(true);
    //}
    //将Mat图像转换为QImage图像，才能显示在label上
    QImage img = QImage((const unsigned char*)(image.data),
                        image.cols, image.rows, QImage::Format_RGB888);
    //设定图像大小自适应label窗口的大小
    img = img.scaled(ui->RawImg->size(), Qt::IgnoreAspectRatio, Qt::SmoothTransformation);
    ui->RawImg->setPixmap(QPixmap::fromImage(img));
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
