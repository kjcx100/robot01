#include <stdlib.h>
#include <stdio.h>
#include <iostream>

#include <QSettings>
#include <QTextCodec>
#include <QtDebug>
#include <QString>
#include <QPainter>
#include <qmath.h>


#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "include/image_process.h"
static int Timer_Count = 0;

extern cv::Mat g_cvimg;
extern CAMMER_PARA_S g_SysParam;
extern QString		curr_path;
extern cv::Mat g_cvOutimg;
extern cv::Mat g_cvdeepimg;
extern cv::Mat g_cvRawTempimg;

extern int g_IsTemp_btn;
extern char* g_tmpbuffer;
extern int gm_width ;
extern int gm_hight ;
extern volatile bool exit_main;
extern volatile bool save_frame;
extern volatile bool save_rect_color;

QImage		   g_VideoImage;
QImage		   g_VideoImageOut;
int main_PointsLine[MAX_DEVNUM][DEEPIMG_WIDTH];
int main_DistPointsLine[MAX_DEVNUM][DEEPIMG_DRAWPOINT];

int g_is_point_OK = 0;
float SINX[90] = {0.0175,0.0349,0.0523,0.0698,0.0872,0.1045,0.1287,0.1392,0.1564,0.1737,0.1908,0.2079,0.2250,0.2419,0.2589,
				0.2756,0.2924,0.3090,0.3256,0.3420,0.3584,0.3746,0.3907,0.4067,0.4226,0.4384,0.4540,0.4695,0.4848,0.5000,
				0.5151,0.5300,0.5446,0.5600,0.5736,0.5878,0.6018,0.6157,0.6293,0.6428,0.6560,0.6691,0.6820,0.6747,0.7071,
				0.7200,0.7314,0.7431,0.7547,0.7660,0.7771,0.7880,0.7986,0.8090,0.8191,0.8290,0.8387,0.8480,0.8572,0.8660,
				0.8746,0.8829,0.8910,0.8989,0.9063,0.9135,0.9205,0.9272,0.9336,0.9367,0.9455,0.9510,0.9563,0.9613,0.9660,
				0.9703,0.9744,0.9785,0.9816,0.9848,0.9878,0.9902,0.9925,0.9945,0.9962,0.9976,0.9986,0.9993,0.9998,1.0000};
float TANX[90] = {0.0175,0.0369,0.0524,0.0699,0.0875,0.1051,0.1228,0.1405,0.1584,0.1763,0.1944,0.2126,0.2309,0.2493,0.2679,
				0.2867,0.3057,0.3249,0.3443,0.3639,0.3839,0.4040,0.4245,0.4452,0.4663,0.4877,0.5095,0.5317,0.5543,0.5773,
				0.6008,0.6249,0.6494,0.6745,0.7002,0.7265,0.7535,0.7813,0.8098,0.8391,0.8693,0.9004,0.9324,0.9657,1.0000,
				1.0355,1.0724,1.1106,1.1503,1.1917,1.2349,1.2799,1.3270,1.3764,1.4281,1.4826,1.5399,1.6000,1.6643,1.7320,
				1.8040,1.8807,1.9626,2.0503,2.1445,2.2460,2.3558,2.4750,2.6051,2.7475,2.9042,3.0777,3.2708,3.4874,3.7320,
				4.0108,4.3315,4.7046,5.1445,5.6713,6.3137,7.1154,8.1444,9.5144,11.430,11.430,11.430,11.430,11.430,11.430};

ImageProcessThread m_ImageProcessThread;

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    //ui->RawImg->show();
    //ui->OutImg->show();
    //this->setMouseTracking(true);//设置窗体可响应 Mouse Move
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
        pTimer500ms->start(150);              // 50ms单触发定时器
    }
    //Time  1s
    pTimer1S = new QTimer(this);
    if(pTimer1S)
    {
        connect( pTimer1S, SIGNAL(timeout()), this, SLOT(call_timerDone_1s()) );
        pTimer1S->start(1000);              // 1秒单触发定时器
    }

	connect(&m_ImageProcessThread, SIGNAL(EmitFrameMessage(cv::Mat*, int)), this, SLOT(EmitFrameMessage(cv::Mat*, int)));
	connect(&m_ImageProcessThread, SIGNAL(EmitOutFrameMessage(cv::Mat*, int)), this, SLOT(EmitOutFrameMessage(cv::Mat*, int)));
	connect(&m_ImageProcessThread, SIGNAL(EmitRawTempMessage(cv::Mat*, int)), this, SLOT(EmitRawTempMessage(cv::Mat*, int)));
	for(int devi = 0; devi < MAX_DEVNUM; devi++)
	{
		for(int i=0; i < gm_width ; i++)
		{
			main_PointsLine[devi][i] = 0 ;
			main_DistPointsLine[devi][i] = 0 ;
		}
	}
    m_ImageProcessThread.start();
	qDebug("m_ImageProcessThread start!!!%d\n",__LINE__);}
void MainWindow::paintEvent(QPaintEvent *)
{
	//绘制结果显示背景	26+420+420	-->>40+420+420
	drawRectInPos(ui->RawImg->x()+560,ui->RawImg->y() + ui->RawImg->height() + 40,ui->RawImg->width(),ui->RawImg->height());
	//drawBarrierLine(ui->RawImg->x()+560,ui->RawImg->y() + ui->RawImg->height() + 40,ui->RawImg->width(),ui->RawImg->height());
	//drawRectInPos(100,100,100,100);

}

void MainWindow::call_timerDone_500ms()
{
    //printf("call_timerDone_500ms:: line=%d\n",__LINE__);
    //pTimer500ms->stop();
	if(g_is_point_OK)
	{
		g_is_point_OK = 0;
		update();
	}
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_TempBtn_clicked()
{
#if 0 
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
#else
	    //调用窗口打开文件
    QString filename = QFileDialog::getOpenFileName(this,
                                                    QObject::tr("open image"),
                                                    ".",
                                                    QObject::tr("Image file(*.png *.jpg *.bmp *.yuv)"));

    qDebug("filename=%s\n",filename.toLocal8Bit().data());
	if(NULL == filename.toLocal8Bit().data())
	{
		printf("Error:filename NULL!\n");
        return ;
	}	
	FILE *filetmp;
    //const char* tempimg = "./template.yuv";
	filetmp = fopen( filename.toLocal8Bit().data(), "rb");
	if (NULL == filetmp)
	{
		printf("Error:Open tempimg file fail!\n");
        qDebug("Error:Open tempimg file fail#####%s\n",filename.toLocal8Bit().data());
        return ;
	}
	printf("fopen %s ok,gm_width==%d,gm_hight==%d\n",filename.toLocal8Bit().data(),gm_width,gm_hight);
	if (gm_width*gm_hight * 2 != fread(g_tmpbuffer, 1, gm_width*gm_hight*2, filetmp))
	{
		//提示文件读取错误  
		fclose(filetmp);
		printf("Error:read tempimg file fail!\n");
        return ;
	}
	fclose(filetmp);
	g_IsTemp_btn = 1;
#endif
}
void MainWindow::call_timerDone_1s()
{
#if 0
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
#endif
	if(m_ImageProcessThread.bStop)
	{
		m_ImageProcessThread.start();
		m_ImageProcessThread.bStop = false;
	}
}
void MainWindow::drawRectInPos(int start_x,int start_y,int w,int h)
{
	
	//printf("drawRectInPos:: line=%d,m_mouse_x==%d\n",__LINE__,m_mouse_x);
    QPainter painter(this);
    //创建画笔
    QPen pen(Qt::green, 2, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin);
	CAMMER_PARA_S st_SysParam;
	GetCammerSysParam(&st_SysParam);
	int MIDRECT_DRAW_LINE = 3;
	const int Min_paintLen = 80;
	
	    // 创建画刷
    //QBrush brush(QColor(0, 0, 0), Qt::SolidPattern);
    // 使用画刷
    //painter.setBrush(brush);
	    //绘制一个矩形
    painter.drawRect(start_x, start_y, w, h);	
	    // 使用画刷填充一个矩形区域
    painter.fillRect(QRect(start_x, start_y, w, h),QColor(15,45,188));
	//重新设置画笔
    pen.setWidth(1);
    pen.setColor(Qt::white);
    painter.setPen(pen);

	int circleNum = 8;
	painter.drawLine( QPoint(start_x ,start_y + h/2), QPoint(start_x + w,start_y + h/2));
	painter.drawLine( QPoint(start_x + w/2,start_y), QPoint(start_x + w/2,start_y + h));
    //painter.drawEllipse(QPointF(start_x + w/2, start_y + h/2), h/circleNum, h/circleNum);
    painter.drawEllipse(QPointF(start_x + w/2, start_y + h/2), h*2/circleNum, h*2/circleNum);
    painter.drawEllipse(QPointF(start_x + w/2, start_y + h/2), h*3/circleNum, h*3/circleNum);
    painter.drawEllipse(QPointF(start_x + w/2, start_y + h/2), h*4/circleNum, h*4/circleNum);
    QPixmap pix;
    pix.load(":/bg_img/car_black.jpg");
	int carpic_w = 40;
	int carpic_h = 60;
    painter.drawPixmap(start_x + w/2 - carpic_w/2, start_y + h/2 - carpic_h/2, carpic_w, carpic_h, pix);
	QFont font("AR PL UKai CN", 14, QFont::Bold, true);
	//https://blog.csdn.net/qq_40194498/article/details/79171149
	//设置下划线
	//font.setUnderline(true);
	//使用字体
	painter.setFont(font);
	painter.drawText(start_x + 4, start_y + 24, tr("极坐标图"));
	//painter.drawText(start_x + 4, start_y + 40,tr("(%1, %2)").arg(m_mouse_x).arg(m_mouse_y));

	//###############分割线
	//重新设置画笔
    pen.setWidth(2);
    pen.setColor(Qt::yellow);
    painter.setPen(pen);
	int qPNum = 60;
	int halfqPNum = 30;
	QPoint DrawPoint[MAX_DEVNUM][qPNum];
	int PointsLine[MAX_DEVNUM][qPNum];
	int DisPointsLine[MAX_DEVNUM][qPNum];	//由point转换为距离 dispoint = PointsLine/tan(x)
	int DisLine[MAX_DEVNUM][qPNum];
	for(int device = 0; device <= MAX_DEVNUM ;device++)
	{
		#if DEEP_DISTANSLINE
		for(int a = 0;a < qPNum; a++)
		{
			PointsLine[device][a] = main_DistPointsLine[device][a];
		}
		#else
		for(int a = 0;a < qPNum; a++)
		{
			PointsLine[device][a] = main_PointsLine[device][39 + a*10];
		}
		#endif
		for (int lj = 0; lj < qPNum; lj++)
		{
			//输出到rgb
			if(lj >= MIDRECT_DRAW_LINE)
			{
				if(0 == PointsLine[device][lj] && PointsLine[device][lj-1] != 0)	//点无变化
				{
					PointsLine[device][lj] = 10;//PointsLine[lj-1];
				}
			}
		}
		for(int a = 0;a < qPNum; a++)
		{
			DisLine[device][a] = st_SysParam.Edit_instalHeight*TANX[(int)((DEEPIMG_HEIGHT - PointsLine[device][a])/10 + st_SysParam.EditVer_Angl-23)];
			//if(DisLine[a] > 200)
			//	DisLine[a] = 200;
		}
		if(0 == device)
		{
			for(int a = 0;a <= halfqPNum; a++)
			{
				DrawPoint[device][a].setX(start_x + w/2 - (DEEPIMG_HEIGHT - PointsLine[device][a])/2*SINX[90-60-a]);
				DrawPoint[device][a].setY(start_y + h/2 - (DEEPIMG_HEIGHT - PointsLine[device][a])/2*SINX[60+a]);
				//DrawPoint[a].setX(start_x + w/2 - (DEEPIMG_HEIGHT)/4*SINX[90-60-a]);
				//DrawPoint[a].setY(start_y + h/2 - (DEEPIMG_HEIGHT)/4*SINX[60+a]);
				//painter.drawPoint(DrawPoint[a]);
				//for test
				//painter.drawPoint(QPoint(start_x + w/2 +100,start_y + h/2 + a+100));
				//for test end
			}
			for(int a = 30;a <= qPNum; a++)
			{
				DrawPoint[device][a].setX(start_x + w/2 + (DEEPIMG_HEIGHT - PointsLine[device][a])/2*SINX[a - 30]);
				DrawPoint[device][a].setY(start_y + h/2 - (DEEPIMG_HEIGHT - PointsLine[device][a])/2*SINX[120 - a]);
				//DrawPoint[a].setX(start_x + w/2 + (DEEPIMG_HEIGHT)/4*SINX[a - 30]);
				//DrawPoint[a].setY(start_y + h/2 - (DEEPIMG_HEIGHT)/4*SINX[120 - a]);
				//painter.drawPoint(DrawPoint[a]);
			}
			DrawPoint[device][30].setX(start_x + w/2);
			DrawPoint[device][30].setY(start_y + h/2 - (DEEPIMG_HEIGHT - PointsLine[device][30])/2);
		}
		else if(1 == device)
		{
			for(int a = 0;a <= halfqPNum; a++)
			{
				DrawPoint[device][a].setX(start_x + w/2 + (DEEPIMG_HEIGHT - PointsLine[device][a])/2*SINX[60+a]);
				DrawPoint[device][a].setY(start_y + h/2 - (DEEPIMG_HEIGHT - PointsLine[device][a])/2*SINX[30-a]);
			}
			for(int a = 30;a <= qPNum; a++)
			{
				DrawPoint[device][a].setX(start_x + w/2 + (DEEPIMG_HEIGHT - PointsLine[device][a])/2*SINX[120 - a]);
				DrawPoint[device][a].setY(start_y + h/2 + (DEEPIMG_HEIGHT - PointsLine[device][a])/2*SINX[a - 30]);
			}
			DrawPoint[device][30].setX(start_x + w/2 + (DEEPIMG_HEIGHT - PointsLine[device][30])/2);
			DrawPoint[device][30].setY(start_y + h/2);
		}
		else if(2 == device)
		{
			for(int a = 0;a <= halfqPNum; a++)
			{
				DrawPoint[device][a].setX(start_x + w/2 + (DEEPIMG_HEIGHT - PointsLine[device][a])/2*SINX[90-60-a]);
				DrawPoint[device][a].setY(start_y + h/2 + (DEEPIMG_HEIGHT - PointsLine[device][a])/2*SINX[60+a]);
			}
			for(int a = 30;a <= qPNum; a++)
			{
				DrawPoint[device][a].setX(start_x + w/2 - (DEEPIMG_HEIGHT - PointsLine[device][a])/2*SINX[a - 30]);
				DrawPoint[device][a].setY(start_y + h/2 + (DEEPIMG_HEIGHT - PointsLine[device][a])/2*SINX[120 - a]);
			}
			DrawPoint[device][30].setX(start_x + w/2);
			DrawPoint[device][30].setY(start_y + h/2 + (DEEPIMG_HEIGHT - PointsLine[device][30])/2);
		}
		else if(3 == device)
		{
			for(int a = 0;a <= halfqPNum; a++)
			{
				DrawPoint[device][a].setX(start_x + w/2 - (DEEPIMG_HEIGHT - PointsLine[device][a])/2*SINX[60+a]);
				DrawPoint[device][a].setY(start_y + h/2 + (DEEPIMG_HEIGHT - PointsLine[device][a])/2*SINX[30-a]);
			}
			for(int a = 30;a <= qPNum; a++)
			{
				DrawPoint[device][a].setX(start_x + w/2 - (DEEPIMG_HEIGHT - PointsLine[device][a])/2*SINX[120 - a]);
				DrawPoint[device][a].setY(start_y + h/2 - (DEEPIMG_HEIGHT - PointsLine[device][a])/2*SINX[a - 30]);
			}
			DrawPoint[device][30].setX(start_x + w/2 - (DEEPIMG_HEIGHT - PointsLine[device][30])/2);
			DrawPoint[device][30].setY(start_y + h/2);
		}

		for(int a = 0;a < qPNum -1; a++)
		{
			if(PointsLine[device][a] > Min_paintLen && PointsLine[device][a+1] > Min_paintLen)
				painter.drawLine(DrawPoint[device][a], DrawPoint[device][a+1]);
				//painter.drawPoint(DrawPoint[a]);
			//QPoint qPLine;
			//qPLine.setX(start_x + w/2 +a);
			//qPLine.setY(start_y + h/2 - PointsLine[a]);
			//painter.drawPoint(qPLine);
		}
	}
	////画角度变换后的线//////////
	//重新设置画笔
    pen.setWidth(2);
    pen.setColor(Qt::red);
    painter.setPen(pen);
	for(int device = 0; device <= MAX_DEVNUM ;device++)
	{
		if(0 == device)
		{
			#if DEEP_DISTANSLINE
			for(int a = 0;a <= halfqPNum; a++)
			{
				DrawPoint[device][a].setX(start_x + w/2 - (DisLine[device][a])*SINX[90-60-a]);
				DrawPoint[device][a].setY(start_y + h/2 - (DisLine[device][a])*SINX[60+a]);
			}
			for(int a = 30;a <= qPNum; a++)
			{
				DrawPoint[device][a].setX(start_x + w/2 + (DisLine[device][a])*SINX[a - 30]);
				DrawPoint[device][a].setY(start_y + h/2 - (DisLine[device][a])*SINX[120 - a]);
			}
			DrawPoint[device][30].setX(start_x + w/2);
			DrawPoint[device][30].setY(start_y + h/2 - (DisLine[device][30]));
			#else
			for(int a = 0;a <= halfqPNum; a++)
			{
				DrawPoint[device][a].setX(start_x + w/2 - (DisLine[device][a])*2*SINX[90-60-a]);
				DrawPoint[device][a].setY(start_y + h/2 - (DisLine[device][a])*2*SINX[60+a]);
			}
			for(int a = 30;a <= qPNum; a++)
			{
				//DrawPoint[a].setX(start_x + w/2 + (DEEPIMG_HEIGHT - DisLine[a])/2*SINX[a - 30]);
				DrawPoint[device][a].setX(start_x + w/2 + (DisLine[device][a])*2*SINX[a - 30]);
				DrawPoint[device][a].setY(start_y + h/2 - (DisLine[device][a])*2*SINX[120 - a]);
			}
			DrawPoint[device][30].setX(start_x + w/2);
			DrawPoint[device][30].setY(start_y + h/2 - (DisLine[device][30])*2);
			#endif
		}
		for(int a = 0;a < qPNum -1; a++)
		{
			if(PointsLine[device][a] > Min_paintLen && PointsLine[device][a+1] > Min_paintLen)
				painter.drawLine(DrawPoint[device][a], DrawPoint[device][a+1]);
				//painter.drawPoint(DrawPoint[a]);
		}
	}
}
//画障碍物曲线，以画框中心为原点，极坐标的方式
void MainWindow::drawBarrierLine(int start_x,int start_y,int w,int h)
{
	//printf("drawBarrierLine:: line=%d\n",__LINE__);
	#if 0
	QPainter painter(this);
    //创建画笔
    QPen pen(Qt::red, 2, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin);
	painter.setPen(pen);
	int qPNum = 60;
	int halfqPNum = 30;
	QPoint DrawPoint[qPNum];
	int PointsLine[qPNum];
	for(int a = 0;a < qPNum; a++)
	{
		PointsLine[a] = main_PointsLine[39 + a*10];
	}
	for(int a = 0;a <= halfqPNum; a++)
	{
		DrawPoint[a].setX(start_x + w/2 - (DEEPIMG_HEIGHT - PointsLine[a])*SINX[90-60-a]);
		DrawPoint[a].setY(start_y + h/2 + (DEEPIMG_HEIGHT - PointsLine[a])*SINX[60+a]);
		painter.drawPoint(DrawPoint[a]);
	}
	for(int a = 31;a <= halfqPNum; a++)
	{
		DrawPoint[a].setX(start_x + w/2 + (DEEPIMG_HEIGHT - PointsLine[a])*SINX[a-30]);
		DrawPoint[a].setY(start_y + h/2 + (DEEPIMG_HEIGHT - PointsLine[a])*(1-SINX[a-30]) );
		painter.drawPoint(DrawPoint[a]);
	}
	#endif
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
void MainWindow::SetImageRawTempImg(QImage *img)
{
    if(img != NULL) {
        //设定图像大小自适应label窗口的大小
        img->scaled(ui->Raw_Temp->size(), Qt::IgnoreAspectRatio, Qt::SmoothTransformation);
        ui->Raw_Temp->setPixmap(QPixmap::fromImage(*img));
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
//--------------------- 
//作者：Hail-Linux 
//来源：CSDN 
//原文：https://blog.csdn.net/wr132/article/details/54428144 
//版权声明：本文为博主原创文章，转载请附上博文链接！

QImage MainWindow::cvMat2QImage(const cv::Mat& mat)
{
    // 8-bits unsigned, NO. OF CHANNELS = 1
    if(mat.type() == CV_8UC1)
    {
        QImage image(mat.cols, mat.rows, QImage::Format_Indexed8);
        // Set the color table (used to translate colour indexes to qRgb values)
        //image.setNumColors(256);
        for(int i = 0; i < 256; i++)
        {
            image.setColor(i, qRgb(i, i, i));
        }
        // Copy input Mat
        uchar *pSrc = mat.data;
        for(int row = 0; row < mat.rows; row ++)
        {
            uchar *pDest = image.scanLine(row);
            memcpy(pDest, pSrc, mat.cols);
            pSrc += mat.step;
        }
        return image;
    }
    // 8-bits unsigned, NO. OF CHANNELS = 3
    else if(mat.type() == CV_8UC3)
    {
        // Copy input Mat
        const uchar *pSrc = (const uchar*)mat.data;
        // Create QImage with same dimensions as input Mat
        QImage image(pSrc, mat.cols, mat.rows, mat.step, QImage::Format_RGB888);
        return image.rgbSwapped();
    }
    else if(mat.type() == CV_8UC4)
    {
    	printf("type() == CV_8UC4\n");
        // Copy input Mat
        const uchar *pSrc = (const uchar*)mat.data;
        // Create QImage with same dimensions as input Mat
        QImage image(pSrc, mat.cols, mat.rows, mat.step, QImage::Format_ARGB32);
        return image.copy();
    }
    else
    {
        return QImage();
    }
}


void MainWindow::EmitFrameMessage(cv::Mat* stFrameItem, int nCh)
{
	//printf("###[%s][%d],type=%d in!!!\n", __func__, __LINE__,stFrameItem->type());
	if(pVideoImage == NULL)
		return;
	//g_VideoImage = g_Video_cBtoQI[nCh].BMP24ToQImage24(szBmp + 54, stFrameItem.dwWidth, stFrameItem.dwHeight, stFrameItem.dwWidth * 3, 0);
	QImage img = cvMat2QImage(g_cvdeepimg);//QImage((const unsigned char*)(stFrameItem->data),stFrameItem->cols, stFrameItem->rows, QImage::Format_RGB888);
	//g_VideoImage = g_VideoImage.mirrored(false, true);
    SetImageQimage(&img);
}
void MainWindow::EmitOutFrameMessage(cv::Mat* stFrameItem, int nCh)
{
	if(pVideoImage == NULL)
		return;
	//QSize outsize = ui->OutImg->size();
	QImage img = cvMat2QImage(g_cvOutimg);//QImage((const unsigned char*)(stFrameItem->data),stFrameItem->cols, stFrameItem->rows, QImage::Format_RGB888);
    SetImageOutImg(&img);
}
void MainWindow::EmitRawTempMessage(cv::Mat* stFrameItem, int nCh)
{
	if(pVideoImage == NULL)
		return;
	//QSize outsize = ui->OutImg->size();
	QImage img = cvMat2QImage(g_cvRawTempimg);//QImage((const unsigned char*)(stFrameItem->data),stFrameItem->cols, stFrameItem->rows, QImage::Format_RGB888);
    SetImageRawTempImg(&img);
}

void MainWindow::keyPressEvent(QKeyEvent *e)
{
   if(e->key() == Qt::Key_Enter || e->key() == Qt::Key_Return)
   {
		on_SaveBtn_clicked();
   }
	int key = -1;
	//int key = cv::waitKey(1);
	//LOGD(">>>>>>>>>>key==%d\n", key);
	switch (e->key()){

	case Qt::Key_Q:
		exit_main = true;
		break;
	case Qt::Key_S:
		save_frame = true;
		break;
	case Qt::Key_A:
		save_rect_color = true;
		break;
	default:
		break;
	}
}
void MainWindow::mousePressEvent(QMouseEvent *e)
{
	int dis_x = ui->RawImg->x()+560+ui->RawImg->width()/2 - e->x();
	int dis_y = ui->RawImg->y() + ui->RawImg->height()+ui->RawImg->height()/2  + 40 - e->y();
	int dist = qSqrt(dis_x*dis_x+dis_y*dis_y);
	ui->Disp_Mouse->setText(tr("(%1)").arg(dist));
	printf("mouse x==%d,mouse y ==%d\n",e->x(),e->y());
	m_mouse_x = e->x();
	m_mouse_y = e->y();

}
void MainWindow::on_SaveBtn_clicked()
{
	printf("###[%s][%d], in!!!\n", __func__, __LINE__);
	CAMMER_PARA_S st_SysParam;
	GetCammerSysParam(&st_SysParam);
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
	line_Qby = ui->Temp_threshold->text().toLatin1();
    line_getNum = atof(line_Qby);
	st_SysParam.Temp_threshold = line_getNum;
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
	line_Qby = ui->Edit_instalHeight->text().toLatin1();
    line_getInt = atoi(line_Qby);
	st_SysParam.Edit_instalHeight = line_getInt;
	line_Qby = ui->GussBlurSize->text().toLatin1();
    line_getInt = atoi(line_Qby);
	st_SysParam.GussBlurSize = line_getInt;
	line_Qby = ui->MidBlurSize->text().toLatin1();
    line_getInt = atoi(line_Qby);
	st_SysParam.MidBlurSize = line_getInt;
	line_Qby = ui->MorphOpenSize->text().toLatin1();
    line_getInt = atoi(line_Qby);
	st_SysParam.MorphOpenSize = line_getInt;
	line_Qby = ui->MorphCloseSize->text().toLatin1();
    line_getInt = atoi(line_Qby);
	st_SysParam.MorphCloseSize = line_getInt;

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

	st_SysParam.CurrentCam 		= ui->Comb_Cam->currentIndex();
	st_SysParam.IsApplyCam01 	= ui->ApyCam1->checkState();
	st_SysParam.IsApplyCam02 	= ui->ApyCam2->checkState();
	st_SysParam.IsApplyCam03 	= ui->ApyCam3->checkState();
	st_SysParam.IsApplyCam04 	= ui->ApyCam4->checkState();
	st_SysParam.IsApplyCam05 	= ui->ApyCam5->checkState();
	
    qDebug("###[%s][%d],line_getNum==%f\n", __func__, __LINE__, line_getNum);
	SetCammerSetParamFile(&st_SysParam);
	SetUIDispParam(&st_SysParam);
}

void MainWindow::on_Comb_Cam_currentIndexChanged(int index)
{
	printf("###[%s][%d], in!!!,index==%d\n", __func__, __LINE__,index);
	//m_ImageProcessThread.stop();	//5个一起工作，不停止线程
	//m_ImageProcessThread.requestInterruption();
	on_SaveBtn_clicked();
	
}

int MainWindow::SetUIDispParam(CAMMER_PARA_S* pstparam)
{
	if(pstparam == NULL)
        return -1;
	CAMMER_PARA_S stparam ; 
	memcpy(& stparam, pstparam, sizeof(CAMMER_PARA_S));
	QString str;
	str.sprintf("%.2f",stparam.EditHor_View);
	ui->EditHor_View->setText(str);
	str.sprintf("%.2f",stparam.EditVer_View);
	ui->EditVer_View->setText(str);
	str.sprintf("%.2f",stparam.EditHor_Angl);
	ui->EditHor_Angl->setText(str);
	str.sprintf("%.2f",stparam.EditVer_Angl);
	ui->EditVer_Angl->setText(str);
	str.sprintf("%.2f",stparam.calib_horizon);
	ui->calib_horizon->setText(str);
	str.sprintf("%.2f",stparam.calib_vertical);
	ui->calib_vertical->setText(str);
	str.sprintf("%.2f",stparam.Temp_threshold);
	ui->Temp_threshold->setText(str);
	//int
	str.sprintf("%d",stparam.PixWidth_Start);
	ui->PixWidth_Start->setText(str);
	str.sprintf("%d",stparam.PixWidth_End);
	ui->PixWidth_End->setText(str);
	str.sprintf("%d",stparam.PixHight_Start);
	ui->PixHight_Start->setText(str);
	str.sprintf("%d",stparam.PixHight_End);
	ui->PixHight_End->setText(str);
	str.sprintf("%d",stparam.Edit_instalHeight);
	ui->Edit_instalHeight->setText(str);
	str.sprintf("%d",stparam.GussBlurSize);
	ui->GussBlurSize->setText(str);
	str.sprintf("%d",stparam.MidBlurSize);
	ui->MidBlurSize->setText(str);
	str.sprintf("%d",stparam.MorphOpenSize);
	ui->MorphOpenSize->setText(str);
	str.sprintf("%d",stparam.MorphCloseSize);
	ui->MorphCloseSize->setText(str);
	str.sprintf("%d",stparam.threshold_val);
	ui->threshold_val->setText(str);
	str.sprintf("%d",stparam.Gain_thold_min);
	ui->Gain_thold_min->setText(str);
	str.sprintf("%d",stparam.Gain_thold_max);
	ui->Gain_thold_max->setText(str);
	str.sprintf("%d",stparam.Gain_Fre);
	ui->Gain_Fre->setText(str);
	str.sprintf("%d",stparam.Gain_min);
	ui->Gain_min->setText(str);
	str.sprintf("%d",stparam.Gain_max);
	ui->Gain_max->setText(str);

	ui->Comb_Cam->setCurrentIndex(stparam.CurrentCam);
	ui->ApyCam1->setCheckState((Qt::CheckState)stparam.IsApplyCam01);
	ui->ApyCam2->setCheckState((Qt::CheckState)stparam.IsApplyCam02);
	ui->ApyCam3->setCheckState((Qt::CheckState)stparam.IsApplyCam03);
	ui->ApyCam4->setCheckState((Qt::CheckState)stparam.IsApplyCam04);
	ui->ApyCam5->setCheckState((Qt::CheckState)stparam.IsApplyCam05);
	
	
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


//===================================================
//QString  <--> char *
//===================================================
int CopyQString2Chars(QString str, char *mm)
{
    QByteArray ba = str.toUtf8().data();

	if(mm == NULL)
		return -1;
	if(str.length() < 1)
		return -2;

	strcpy(mm, ba.data());
    return 0;
}

////////////参数保存

int GetCammerSysParam(CAMMER_PARA_S* pstparam)
{
	if(pstparam == NULL)
        return -1;
    
    memcpy(pstparam, &g_SysParam, sizeof(CAMMER_PARA_S));    

    return 0;
}

int GetCammerSysParamFile(CAMMER_PARA_S* pstparam)
{
    QSettings  *pIniFile = NULL;
	if(pstparam == NULL)
	{
		qDebug( "GetCammerSysParamFile err  = NULL\n");
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
		pstparam->Edit_instalHeight = pIniFile->value("/CammerParam/Edit_instalHeight", "0").toInt();
		pstparam->EditHor_Angl 		= pIniFile->value("/CammerParam/EditHor_Angl", "0").toFloat();
		pstparam->EditVer_Angl 		= pIniFile->value("/CammerParam/EditVer_Angl", "0").toFloat();
		//滤波参数
		pstparam->GussBlurSize	 	= pIniFile->value("/CammerParam/GussBlurSize", "0").toInt();
		pstparam->MidBlurSize 		= pIniFile->value("/CammerParam/MidBlurSize", "0").toInt();
		pstparam->MorphOpenSize 	= pIniFile->value("/CammerParam/MorphOpenSize", "0").toInt();
		pstparam->MorphCloseSize	= pIniFile->value("/CammerParam/MorphCloseSize", "0").toInt();
		pstparam->calib_horizon 	= pIniFile->value("/CammerParam/calib_horizon", "0").toFloat();
		pstparam->calib_vertical	= pIniFile->value("/CammerParam/calib_vertical", "0").toFloat();
		pstparam->Temp_threshold	= pIniFile->value("/CammerParam/Temp_threshold", "0").toFloat();
		pstparam->threshold_val	 	= pIniFile->value("/CammerParam/threshold_val", "0").toInt();
		//增益参数
		pstparam->Gain_thold_min	= pIniFile->value("/CammerParam/Gain_thold_min", "0").toInt();
		pstparam->Gain_thold_max	= pIniFile->value("/CammerParam/Gain_thold_max", "0").toInt();
		pstparam->Gain_Fre		 	= pIniFile->value("/CammerParam/Gain_Fre", "0").toInt();
		pstparam->Gain_min	 		= pIniFile->value("/CammerParam/Gain_min", "0").toInt();
		pstparam->Gain_max	 		= pIniFile->value("/CammerParam/Gain_max", "0").toInt();

		pstparam->CurrentCam	 		= pIniFile->value("/CammerParam/CurrentCam", "0").toInt();
		pstparam->IsApplyCam01	 		= pIniFile->value("/CammerParam/IsApplyCam01", "0").toInt();
		pstparam->IsApplyCam02	 		= pIniFile->value("/CammerParam/IsApplyCam02", "0").toInt();
		pstparam->IsApplyCam03	 		= pIniFile->value("/CammerParam/IsApplyCam03", "0").toInt();
		pstparam->IsApplyCam04	 		= pIniFile->value("/CammerParam/IsApplyCam04", "0").toInt();
		pstparam->IsApplyCam05	 		= pIniFile->value("/CammerParam/IsApplyCam05", "0").toInt();

		QString sid1				   = pIniFile->value("/IDParam/id1").toString();
		CopyQString2Chars(sid1, pstparam->id1);
		printf("Get id1=%s\n",pstparam->id1);
		QString sid2				   = pIniFile->value("/IDParam/id2").toString();
		CopyQString2Chars(sid2, pstparam->id2);
		QString sid3				   = pIniFile->value("/IDParam/id3").toString();
		CopyQString2Chars(sid3, pstparam->id3);
		QString sid4				   = pIniFile->value("/IDParam/id4").toString();
		CopyQString2Chars(sid4, pstparam->id4);
		QString sid5				   = pIniFile->value("/IDParam/id5").toString();
		CopyQString2Chars(sid5, pstparam->id5);
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
		pIniFile->setValue("/CammerParam/Edit_instalHeight", pstparam->Edit_instalHeight);
		pIniFile->setValue("/CammerParam/EditHor_Angl", pstparam->EditHor_Angl);
		pIniFile->setValue("/CammerParam/EditVer_Angl", pstparam->EditVer_Angl);
		
		pIniFile->setValue("/CammerParam/GussBlurSize", pstparam->GussBlurSize);
		pIniFile->setValue("/CammerParam/MidBlurSize", pstparam->MidBlurSize);
		pIniFile->setValue("/CammerParam/MorphOpenSize", pstparam->MorphOpenSize);
		pIniFile->setValue("/CammerParam/MorphCloseSize", pstparam->MorphCloseSize);
		pIniFile->setValue("/CammerParam/calib_horizon", pstparam->calib_horizon);
		pIniFile->setValue("/CammerParam/calib_vertical", pstparam->calib_vertical);
		pIniFile->setValue("/CammerParam/Temp_threshold", pstparam->Temp_threshold);
		pIniFile->setValue("/CammerParam/threshold_val", pstparam->threshold_val);
		
		pIniFile->setValue("/CammerParam/Gain_thold_min", pstparam->Gain_thold_min);
		pIniFile->setValue("/CammerParam/Gain_thold_max", pstparam->Gain_thold_max);
		pIniFile->setValue("/CammerParam/Gain_Fre", pstparam->Gain_Fre);
		pIniFile->setValue("/CammerParam/Gain_min", pstparam->Gain_min);
		pIniFile->setValue("/CammerParam/Gain_max", pstparam->Gain_max);		

		pIniFile->setValue("/CammerParam/CurrentCam", pstparam->CurrentCam);		
		pIniFile->setValue("/CammerParam/IsApplyCam01", pstparam->IsApplyCam01);		
		pIniFile->setValue("/CammerParam/IsApplyCam02", pstparam->IsApplyCam02);		
		pIniFile->setValue("/CammerParam/IsApplyCam03", pstparam->IsApplyCam03);		
		pIniFile->setValue("/CammerParam/IsApplyCam04", pstparam->IsApplyCam04);		
		pIniFile->setValue("/CammerParam/IsApplyCam05", pstparam->IsApplyCam05);		
		
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




