#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QFileDialog>
#include <QThread>
#include <QTimer>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include "include/picturebox.h"
using namespace cv;

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    void SetImageData(uchar* data,int imgcols,int imgrows);
	void SetImageQimage(QImage *img);
	void SetImageOutImg(QImage *img);	
    void SetImageMat(Mat *cvmat);
    ~MainWindow();

private slots:
    void on_TempBtn_clicked();
    void call_timerDone_500ms();
    void call_timerDone_1s();
	void EmitFrameMessage(cv::Mat* stFrameItem, int nCh = 0);
	void EmitOutFrameMessage(cv::Mat* stFrameItem, int nCh = 0);

private:
    Ui::MainWindow *ui;
    Mat image;
    QTimer *pTimer500ms;
    QTimer *pTimer1S;
	PictureBox *pVideoImage;        //实时视频


};


class ImageDispThread : public QThread
{
    Q_OBJECT

public:
    bool bStop ;
    explicit ImageDispThread(QObject *parent = 0);
    void run();
    void stop();
};
#endif // MAINWINDOW_H
