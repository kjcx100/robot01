
#include <stdlib.h>
#include <stdio.h>
#include <iostream>
#include <QFontDatabase>
#include <QDebug>

#include "include/image_process.h"
#include "../common/common.hpp"
#include "mainwindow.h"

using namespace std;
using namespace cv;

static char buffer[1024 * 1024 * 20];

char *g_tmpbuffer_0 = NULL;
char * g_imgtempImgBuf_0 = NULL;
char *g_tmpbuffer_1 = NULL;
char * g_imgtempImgBuf_1 = NULL;
char *g_tmpbuffer_2 = NULL;
char * g_imgtempImgBuf_2 = NULL;
char *g_tmpbuffer_3 = NULL;
char * g_imgtempImgBuf_3 = NULL;
char *g_tmpbuffer_4 = NULL;
char * g_imgtempImgBuf_4 = NULL;

volatile bool exit_main;
volatile bool save_frame;
volatile bool save_rect_color;
static int gsave_rect_count = 0;
extern CAMMER_PARA_S g_SysParam;
extern int main_PointsLine[MAX_DEVNUM][DEEPIMG_WIDTH];
extern int main_DistPointsLine[MAX_DEVNUM][DEEPIMG_DRAWPOINT];

extern int g_is_point_OK;
extern float SINX[90];


cv::Mat g_cvimg;
Mat g_cvdeepimg;
Mat g_cvOutimg;
Mat g_cvRawTempimg;

int g_IsTemp_btn = 0;
int gm_width = 640;
int gm_hight = 480;


#define CVIMGSHOW   0
#define USE_RGBIMG   0
#define	MUTEX_DEVICE	1


//另一个线程读取图像
BTCommunThread::BTCommunThread(QObject *parent) :
    QThread(parent)
{
    bStop = false;
}

void BTCommunThread::stop()
{
    bStop = true;
}
void BTCommunThread::run()
{
    printf("ok:Open BTCommunThread!\n");
    qDebug("ok:Open BTCommunThread!\n");
    int count = 0;
    #if 0
    const char* IP = NULL;
    const char* ID = NULL;
    TY_DEV_HANDLE hDevice;
    int m_width = 640;
    int m_hight = 480;
    FILE *filetmp;
    const char* tempimg = "./template.yuv";
    filetmp = fopen( tempimg, "rb");
    if (NULL == filetmp)
    {
        printf("Error:Open tempimg file fail!\n");
        qDebug("Error:Open tempimg file fail!\n");
        return ;
    }

    printf("fopen %s ok\n",tempimg);
    qDebug("fopen %s ok\n",tempimg);
    //U16* pfilebuftmp = new U16[m_width*m_hight];//相机采图分辨率

    if (m_width*m_hight * 2 != fread(tmpbuffer, 1, m_width*m_hight*2, filetmp))
    {
        //提示文件读取错误
        fclose(filetmp);
        cout << "fread_s filetmp ERR!!!" << endl;
        return ;
    }
    fclose(filetmp);
    #endif
    char szFilename[32] = {0};
    while(1)
    {
        if(bStop)
            break;
        sleep(5);
        count ++;
        #if 1
        sprintf(szFilename,"./%d.jpg",count);
        qDebug(" BTCommunThread szFilename==%s\n",szFilename);
        Mat image = cv::imread(szFilename);
        if(image.data)
        {
            qDebug(" BTCommunThread run image.cols==%d,rows==%d!\n",image.cols,image.rows);
            g_cvimg = image;
        }
        if(count > 10)
            count = 0;
        #endif
        qDebug(" BTCommunThread run times==%d!\n",count);
        printf(" BTCommunThread run times==%d!\n",count);
    }

}
int Open_TempImg()
{
	return 1;
}
//===============================================================
//select方式延时： 替代usleep(usleep 跟系统时间相关)
//===============================================================
void MY_SLEEP_MS(int ms)
{
	struct timeval delay;

	delay.tv_sec  = 0;
	delay.tv_usec = ms * 1000;

	select(0, NULL, NULL, NULL, &delay);
}

void MY_SLEEP_NS(int s)
{
	struct timeval delay;

	delay.tv_sec  = s;
	delay.tv_usec = 0;

	select(0, NULL, NULL, NULL, &delay);
}

struct CallbackData {
	int             index;
	TY_DEV_HANDLE   hDevice;
	DepthRender*    render;

	TY_CAMERA_DISTORTION color_dist;
	TY_CAMERA_INTRINSIC color_intri;
};
	//立体视角转换为水平上的二维投影
void f_Trans2D(cv::Mat depth,uint8_t* r_data,cv::Mat* p_2D_gray)
{
	int i=0,j=0;
	int l_size = 5;
	uint16_t src_depth_data[450*565];
	uint8_t dst_data[450*565];
	int dst_line[565];
	int dis_line[565];
	//src_depth_data = (uint16_t *)depth.data;
	
	memcpy(src_depth_data,depth.data,450*565*2);
	
	for(i=0;i<565;i++)
	{
		dis_line[i] = 0;
		for(j=0;j<430;j++)
		{
			if(src_depth_data[(j*565) + i] > 5)
			{
				dis_line[i] = j;
			}
		}
		//dst_line[i] = (((450-dis_line[i])/450.0)*1430) + 340;
		dst_line[i] = (int)(tan((((450-dis_line[i])/450.0)*0.817) + 0.35) * 770);
	}
	//LOGD("------------src_depth_data center: %d", src_depth_data[(225*565)+280]);
	//LOGD("------------dstline center: %d", dst_line[280]);
	int st = 0,ed = 0;
	for(i=0;i<51;i++)
	{
		st = i * 11;
		//ed = (i+1) * 11;
		r_data[i] = dst_line[st]/20.0;
		for(int n=0;n<11;n++)
		{
			if((dst_line[st+n]/20.0) < r_data[i])
			{
				r_data[i] = dst_line[st+n]/20.0;
			}
		}
		if(r_data[i] >= 88)
		{
			r_data[i] = 1;
		}
		//r_data[i] = *(std::min_element(dst_line + st,dst_line + ed));
	}
	//LOGD("@@@@@@@@@@@@@@@@---------------r_data value: %d", r_data[5]);
	
	for(i=0;i<450;i++)
	{
		for(j=0;j<565;j++)
		{
			if((i < (dis_line[j]+l_size)) && (i > (dis_line[j]-l_size)))
			{
				dst_data[i*565 + j] = 250;
			}
			else
			{
				dst_data[i*565 + j] = 50;
			}
		}
	}
	*p_2D_gray = cv::Mat(450, 565, CV_8U, dst_data);
}

bool ImageProcessThread::verifySizes(cv::Rect mr) {
	// Set a min and max area. All other patchs are discarded
	int min = 400 * 4;  // minimum area
	int max = 640 * 480;  // maximum area

	float area = mr.height * mr.width;
	// cout << "area:" << area << endl;
	//|| mr.y > (mr.height - (mr.height >> 3))
	if (area < min || area > max || (mr.y > 413))	//坐标在下边8分之1
		return false;
	else
	{
		//cout << "mr.y:" << mr.y << "(472*7/8):" << (472*7/8) << endl;
		return true;
	}
}
//  calc safe Rect
//  if not exit, return false
bool ImageProcessThread::calcSafeRect(const RotatedRect &roi_rect, const Mat &src,
	Rect_<float> &safeBoundRect) {
	Rect_<float> boudRect = roi_rect.boundingRect();

	float tl_x = boudRect.x > 0 ? boudRect.x : 0;
	float tl_y = boudRect.y > 0 ? boudRect.y : 0;

	float br_x = boudRect.x + boudRect.width < src.cols
		? boudRect.x + boudRect.width - 1
		: src.cols - 1;
	float br_y = boudRect.y + boudRect.height < src.rows
		? boudRect.y + boudRect.height - 1
		: src.rows - 1;

	float roi_width = br_x - tl_x;
	float roi_height = br_y - tl_y;

	if (roi_width <= 0 || roi_height <= 0) return false;
	//  a new rect not out the range of mat
	safeBoundRect = Rect_<float>(tl_x, tl_y, roi_width, roi_height);

	return true;
}

void ImageProcessThread::depthTransfer(cv::Mat depth, uint16_t* t_data, cv::Mat* newDepth, cv::Mat* blackDepth)
{
	int i=0;
	uint16_t dst_data[480*640];
	uint16_t blk_data[480*640];
	uint16_t* src_data;
	uint16_t treshhold;
	int shift_y = 0;
	int Total_pix = gm_hight*gm_width;
	uint32_t temp_dmin = 0;
	uint32_t temp_dmax = 0;
	float shift_index = 0;
	for(int j = 0; j < 10 ; j++)
	{
		temp_dmax += t_data[480*10+j*6];
	}
	temp_dmax = temp_dmax/10;
	for(int lj = 0; lj < 10 ; lj++)
	{
		temp_dmin += t_data[Total_pix - 480*10 - lj*6];
	}
	temp_dmin = temp_dmin/10;
	if((temp_dmax - temp_dmin) > 10)		
		shift_index = 480/(temp_dmax - temp_dmin);
	else
		shift_index = 0.25;
	CAMMER_PARA_S st_SysParam;
	GetCammerSysParam(&st_SysParam);
	
	src_data = (uint16_t *)depth.data;
	memset(blk_data,0,480*640*2);
	for(int ii=0;ii<DEEPIMG_HEIGHT;ii++)
	{
		for (int j = 0; j < DEEPIMG_WIDTH; j++)
		{
			i = ii*DEEPIMG_WIDTH + j;
			treshhold = (uint16_t)(t_data[i]*st_SysParam.Temp_threshold);
			if(src_data[i] == 0)
			{
				dst_data[i] = 0;
				if(i > (100*640))
				{
					blk_data[i] = 500;
				}
			}
			else if((src_data[i] - t_data[i]) < (-1 * treshhold))
			{
				//dst_data[i] = 0;	//凹下去的也设置为0
				//dst_data[i] = 1200;
				#if 1
				shift_y = (t_data[i] - src_data[i])*shift_index*SINX[(int)((DEEPIMG_HEIGHT - ii)/10 + st_SysParam.EditVer_Angl-23)];	//y坐标的偏移量=距离差*sinx
				if(i + shift_y*gm_width < Total_pix)
					dst_data[i + shift_y*gm_width] = 2500;
				else
					dst_data[Total_pix] = 2500;
				#endif
			}
			else if((src_data[i] - t_data[i]) > (treshhold * 1.5))
			{
				//dst_data[i] = 2500;
				//dst_data[i] = t_data[i];	//保留深度 信息
				shift_y = (src_data[i] - t_data[i])*shift_index*SINX[(int)((DEEPIMG_HEIGHT - ii)/10 + st_SysParam.EditVer_Angl-23)];	//y坐标的偏移量=距离差*sinx
				if(i + shift_y*gm_width < Total_pix)
					dst_data[i + shift_y*gm_width] = 2500;
				else
					dst_data[Total_pix] = 2500;
			}
			else
			{
				dst_data[i] = 0;
			}

		}

	}
	
	dst_data[0] = 500;
	dst_data[1] = 4000;
	
	blk_data[0] = 500;
	blk_data[1] = 4000;
	
	*newDepth = cv::Mat(480, 640, CV_16U, dst_data);
	*blackDepth = cv::Mat(480, 640, CV_16U, blk_data);
}

void ImageProcessThread::depthTrans_BarrierLine(cv::Mat depth, uint16_t* t_data, cv::Mat* newDepth, cv::Mat* blackDepth)
{
	int i=0;
	uint16_t dst_data[480*640];
	uint16_t blk_data[480*640];
	uint16_t* src_data;
	uint16_t treshhold;
	uint16_t temp_dist = 0;
	CAMMER_PARA_S st_SysParam;
	GetCammerSysParam(&st_SysParam);
	
	src_data = (uint16_t *)depth.data;
	memset(blk_data,0,480*640*2);
	for(i=0;i<(480*640);i++)
	{
		treshhold = (uint16_t)(t_data[i]*st_SysParam.Temp_threshold);
		if(src_data[i] == 0)
		{
			dst_data[i] = 0;
			if(i > (100*640))
			{
				blk_data[i] = 500;
			}
		}
		else if((t_data[i] - src_data[i]) < (-1 * treshhold))
		{
			dst_data[i] = 0;	//凹下去的也设置为0
			//dst_data[i] = 1200;
			//dst_data[i] = src_data[i];
		}
		else if((t_data[i] - src_data[i]) > (treshhold * 1.5))
		{
			//dst_data[i] = 2500;
			dst_data[i] = src_data[i];	//保留深度 信息
		}
		else
		{
			dst_data[i] = 0;
		}
		//输入depth图像先切掉两块 上边，左边
		if(i <= st_SysParam.PixHight_End*640)
			dst_data[i] = 0;
		//if( i > 40 && j <= 112)
		if( i > st_SysParam.PixHight_End*640 && (i%640) <= st_SysParam.PixWidth_End)
			dst_data[i] = 0;

	}
	#if 0
	for (int j = 0; j < DEEPIMG_WIDTH; j++)
	{
		m_DistansLine[j] = 10000;
		int Has_distrans = 0;	//一列中有凸起的点
		for(int ii=0;ii<DEEPIMG_HEIGHT;ii++)
		{
			if(dst_data[j + ii*DEEPIMG_WIDTH] != 0)	//凸起的点
			{
				Has_distrans = 1;
				if(dst_data[j + ii*DEEPIMG_WIDTH] < m_DistansLine[j])
				{
					m_DistansLine[j] = dst_data[j + ii*DEEPIMG_WIDTH];
				}
			}
		}
		if(0 == Has_distrans)
			m_DistansLine[j] = 0;
		//printf("m_DistansLine[%d]==%d\n",j,m_DistansLine[j]);
	}
	#endif
	dst_data[0] = 500;
	dst_data[1] = 4000;
	
	blk_data[0] = 500;
	blk_data[1] = 4000;
	
	*newDepth = cv::Mat(480, 640, CV_16U, dst_data);
	*blackDepth = cv::Mat(480, 640, CV_16U, blk_data);
}

void ImageProcessThread::depthTrans_FindLine(cv::Mat Transdepth ,int devcount)
{
	int i=0;
	int Aver_Useful_Count = 0;	//10个平均点中有用的个数
	uint16_t* src_data;
	uint16_t temp_dist = 0;
	const uint16_t Lonest_dist = 2200;
	CAMMER_PARA_S st_SysParam;
	GetCammerSysParam(&st_SysParam);
	
	src_data = (uint16_t *)Transdepth.data;
	
	for (int j = 0; j < DEEPIMG_WIDTH; j++)
	{
		m_DistansLine[devcount][j] = 10000;
		int Has_distrans = 0;	//一列中有凸起的点
		for(int ii=0;ii<DEEPIMG_HEIGHT;ii++)
		{
			if(src_data[j + ii*DEEPIMG_WIDTH] != 0)	//凸起的点
			{
				if(src_data[j + ii*DEEPIMG_WIDTH] < m_DistansLine[devcount][j] 
					&& src_data[j + ii*DEEPIMG_WIDTH] > 300)
				{
					Has_distrans = 1;
					m_DistansLine[devcount][j] = src_data[j + ii*DEEPIMG_WIDTH];
					m_DistansLine[devcount][j] = m_DistansLine[devcount][j]*SINX[(int)((DEEPIMG_HEIGHT - ii)/10 + st_SysParam.EditVer_Angl-23)];
				}
			}
		}
		if(0 == Has_distrans)
			m_DistansLine[devcount][j] = Lonest_dist;
		//printf("m_DistansLine[%d]==%d\n",j,m_DistansLine[j]);
	}
	for (int j = 40; j < DEEPIMG_WIDTH - 2; j++)
	{
		if(m_DistansLine[devcount][j] > 3000 || m_DistansLine[devcount][j] < 300)
		{
			m_DistansLine[devcount][j] = m_DistansLine[devcount][j+1];
		}
	}
	for (int j = 4; j < DEEPIMG_DRAWPOINT +4; j++)
	{
		Aver_Useful_Count = 0;
		for(int lj = 0; lj < 10; lj++)
		{
			if(m_DistansLine[devcount][j*10 + lj] != Lonest_dist)
			{	
				Aver_Useful_Count++;
				m_DrawDistLine[devcount][j] += m_DistansLine[devcount][j*10 + lj];
			}
		}
		if(Aver_Useful_Count)
			m_DrawDistLine[devcount][j] = m_DrawDistLine[devcount][j]/Aver_Useful_Count;
		else
			m_DrawDistLine[devcount][j] = Lonest_dist;
		if(m_DrawDistLine[devcount][j] > Lonest_dist || m_DrawDistLine[devcount][j] < 300)
		{
			m_DrawDistLine[devcount][j] = Lonest_dist;
		}
		main_DistPointsLine[devcount][j] = DEEPIMG_HEIGHT - (uint16_t)m_DrawDistLine[devcount][j]/5;
		//printf("main_DistPointsLine[%d]==%d,m_DrawDistLine[%d]==%d\n",j,main_DistPointsLine[j],j,m_DrawDistLine[j]);
	}
}

cv::Mat ImageProcessThread::cvMatRect2Tetra(cv::Mat mtxSrc, int iDstX1, int iDstY1, int iDstX2, int iDstY2,
						int iDstX3, int iDstY3, int iDstX4, int iDstY4, int iDstWidth, int iDstHeight)
{
	cv::Mat mtxDst;
	std::vector<cv::Point2f> src_corners(4);
	std::vector<cv::Point2f> dst_corners(4);
 
	src_corners[0]= cv::Point2f(0,0);
	src_corners[1]= cv::Point2f(mtxSrc.cols - 1,0);
	src_corners[2]= cv::Point2f(0, mtxSrc.rows - 1);
	src_corners[3]= cv::Point2f(mtxSrc.cols - 1, mtxSrc.rows - 1);
 
	dst_corners[0] = cv::Point2f(iDstX1, iDstY1);
	dst_corners[1] = cv::Point2f(iDstX2, iDstY2);
	dst_corners[2] = cv::Point2f(iDstX3, iDstY3);
	dst_corners[3] = cv::Point2f(iDstX4, iDstY4);
 
	cv::Mat transMtx = cv::getPerspectiveTransform(src_corners, dst_corners);
	cv::warpPerspective(mtxSrc, mtxDst, transMtx, cv::Size(iDstWidth, iDstHeight));
 
	return mtxDst;
}

int ImageProcessThread::DeepImgFinds_write_rgb(Mat depthColor, Mat resized_color, int blurSize, int morphW, int morphH)
{
	int SOBEL_SCALE = 0;
	int SOBEL_DELTA = 0.5;
	int SOBEL_DDEPTH = CV_16S;
	int SOBEL_X_WEIGHT = 1;
	const int MIDBLUR_DRAW_LINE = 3;
	CAMMER_PARA_S st_SysParam;
	GetCammerSysParam(&st_SysParam);
	//memset(m_PointsLine , 0 ,sizeof(int)*gm_width);
	for(int i=0; i < gm_width ; i++)
	{
		m_PointsLine[i] = 0 ;
	}
	
	//Mat mat_blur;
	Mat In_rgb = resized_color.clone();
	//mat_blur = depthColor.clone();
	//GaussianBlur(depthColor, mat_blur, Size(blurSize, blurSize), 0, 0, BORDER_DEFAULT);

	Mat mat_gray;
	if (depthColor.channels() == 3)
		cvtColor(depthColor, mat_gray, CV_RGB2GRAY);
	else
		mat_gray = depthColor;
	//输入depth图像先切掉两块 上边，左边
	for (int i = 0; i <  mat_gray.rows; i++)
	{
		uchar* data_gray = mat_gray.ptr<uchar>(i);
		for (int j = 0; j <  mat_gray.cols; j++)
		{
			//输出到rgb
			//if(i <= 40)
			if(i <= st_SysParam.PixHight_End)
				data_gray[j] = 0;
			//if( i > 40 && j <= 112)
			if( i > st_SysParam.PixHight_End && j <= st_SysParam.PixWidth_End)
				data_gray[j] = 0;
		}
	}
	int scale = SOBEL_SCALE;
	int delta = SOBEL_DELTA;
	int ddepth = SOBEL_DDEPTH;

	Mat grad_x, grad_y;
	Mat abs_grad_x, abs_grad_y;

	Mat mat_threshold;
	double otsu_thresh_val = threshold(mat_gray, mat_threshold, st_SysParam.threshold_val, 255, CV_THRESH_BINARY);
	//threshold(grad, mat_threshold, 0, 255, CV_THRESH_OTSU + CV_THRESH_BINARY);
	//############先开操作，去掉一些小的区域####################
	//int Open_morphW = 3;
	//int Open_morphH = 3;
	Mat element = getStructuringElement(MORPH_RECT, Size(st_SysParam.MorphOpenSize, st_SysParam.MorphOpenSize));
	morphologyEx(mat_threshold, mat_threshold, MORPH_OPEN, element);
	char jpgfileopen[1024] = { 0 };
	char morphopen[1024] = { 0 };
	//strncpy_s(morphopen, filename + st_len_dirout, strlen(filename) - 4 - st_len_dirout);
	//sprintf_s(jpgfileopen, "./outdir%s_morphology.jpg", morphopen);
	//imwrite(jpgfileopen, mat_threshold);
	#if CVIMGSHOW
	imshow("morphologyOpen", mat_threshold);
    #endif
	if (save_frame){
		LOGD(">>>>>>>>>> write jpgfileopen");
		imwrite("jpgfile_open.png", mat_threshold);
	}
	#if 1
		//lxl log 12-25 MORPH_CLOSE后感觉边界更整齐，have a try
		element = getStructuringElement(MORPH_RECT, Size(morphW, morphH));
		morphologyEx(mat_threshold, mat_threshold, MORPH_CLOSE, element);

		//namedWindow("morphologyEx");
	    #if CVIMGSHOW
		imshow("morphologyClose", mat_threshold);
	    #endif
		if (save_frame) {
			LOGD(">>>>>>>>>> write morphologyClose");
			imwrite("morphologyClose.png", mat_threshold);
		}
	#endif
	//#if 1 //先找连通区域
	Mat findContour;
	mat_threshold.copyTo(findContour);
	vector<vector<Point> > contours;
	findContours(findContour,
		contours,               // a vector of contours
		CV_RETR_LIST,
		CV_CHAIN_APPROX_NONE);  // all pixels of each contours
	vector<vector<Point> >::iterator itc = contours.begin();
	int Count_contours = 0;
	vector<Rect> first_rects;
 
	while (itc != contours.end()) {
		RotatedRect mr = minAreaRect(Mat(*itc));
		Rect_<float> safeBoundRect;
		if (!calcSafeRect(mr, mat_threshold, safeBoundRect))
		{
			cout << "calcSafeRect is false" << endl;
		}
		else
		{
			if (verifySizes(safeBoundRect)) {
				first_rects.push_back(safeBoundRect);
				//cout << "safeBoundRect.area:" << safeBoundRect.area() << endl;
				//rectangle(depthColor, safeBoundRect, Scalar(0, 0, 255));
				//rectangle(In_rgb, safeBoundRect, Scalar(0, 255, 255));
				#if 0	//lxl add 2018-12-21 暂时不用rgb检测物体
				double safecontourArea = contourArea(Mat(*itc));
				//取出检测到的区域 and canny
				Mat imageIn_rects = In_rgb(safeBoundRect);
				Mat imageIn_gray,imageIn_edge;
				cvtColor(imageIn_rects, imageIn_gray, CV_RGB2GRAY);
				cv::blur(imageIn_gray, imageIn_edge, cv::Size(7, 7));
				Canny(imageIn_edge, imageIn_edge, 10, 60, 3);
				char szimageIn_edge[1024] = {0};
				sprintf(szimageIn_edge, "imageIn_edge%d.jpg", Count_contours);
				if (save_frame) {
					LOGD(">>>>>>>>>> write imageIn_edge");
					imwrite(szimageIn_edge, imageIn_edge);
				}
				Mat rectsContour;
				imageIn_edge.copyTo(rectsContour);
				vector<vector<Point> > rectscontours;
				findContours(rectsContour,
					rectscontours,               // a vector of contours
					CV_RETR_EXTERNAL,
					CV_CHAIN_APPROX_NONE);  // all pixels of each contours
				//imageIn_edge.copyTo(imageIn_rects);
				std::cout <<"rectscontours.size:" << rectscontours.size()<< endl;
				for (int i = 0; i < rectscontours.size(); i++)
				{
					double rectscontourArea = contourArea(Mat(rectscontours[i]));
					std::cout <<"rectscontourArea:" << rectscontourArea <<"  safecontourArea:" << safecontourArea<< endl;
					if(abs(rectscontourArea - safecontourArea) < safecontourArea*0.05)
					{
						for (int j = 0; j < rectscontours[i].size(); j++)
						{
							//绘制出contours向量内所有的像素点
							Point P = Point(rectscontours[i][j].x, rectscontours[i][j].y);
							//输出到rgb
							//In_rgb.at<uchar>(P) = 0;
							circle(imageIn_rects, P, 0, Scalar(255, 255, 0));
						}
					}
				}
				//namedWindow("imageIn_rects");
				//imshow("imageIn_rects", imageIn_rects);
				//cvWaitKey(0);
				//destroyWindow("imageIn_rects");
				char szimageIn_rects[1024] = {0};
				sprintf(szimageIn_rects, "imageIn_rects%d.jpg", Count_contours);
				if (save_frame) {
					LOGD(">>>>>>>>>> write imageIn_rects");
					imwrite(szimageIn_rects, imageIn_rects);
				}
				#endif
				for (int j = 0; j < contours[Count_contours].size(); j++)
				{
					//绘制出contours向量内所有的像素点
					Point P = Point(contours[Count_contours][j].x, contours[Count_contours][j].y);
					//输出到rgb
					//circle(depthColor, P, 0, Scalar(255, 255, 0));
					//circle(In_rgb, P, 0, Scalar(255, 255, 0));
					if(m_PointsLine[P.x] < P.y)
					{
						m_PointsLine[P.x] = P.y;
					}
				}
			}
			else//不满足条件的，填充黑色
			{
				//rectangle(depthColor, safeBoundRect, Scalar(0, 255, 0));
				Mat FillImg = Mat::zeros(safeBoundRect.height, safeBoundRect.width, CV_8UC1);
				//cout << "FillImg.cols:" << FillImg.cols << "  FillImg.rows:" << FillImg.rows << endl;
				Rect fillRect = safeBoundRect;
				Mat imageROI = mat_threshold(fillRect);
				floodFill(imageROI, Point2f(imageROI.cols >> 1, imageROI.rows >> 1), 0);
				FillImg.copyTo(imageROI);
				//contours[i]代表的是第i个轮廓，contours[i].size()代表的是第i个轮廓上所有的像素点数
				for (int j = 0; j < contours[Count_contours].size(); j++)
				{
					//绘制出contours向量内所有的像素点
					Point P = Point(contours[Count_contours][j].x, contours[Count_contours][j].y);
					mat_threshold.at<uchar>(P) = 0;
				}
			}
		}
		++itc;
		++Count_contours;
	}
	#if 0
	for (int j = 0; j < DEEPIMG_WIDTH; j++)
	{
		//找出第一个有效的点
		if(0 != m_PointsLine[j] && (j > MIDBLUR_DRAW_LINE))	//
		{
			m_PointsLine[0] = m_PointsLine[j];
			m_PointsLine[1] = m_PointsLine[j];
			m_PointsLine[2] = m_PointsLine[j];
			//printf("The first useful Point is %d\n",j);
			break;
		}
	}
	for (int j = 0; j < DEEPIMG_WIDTH; j++)
	{
		//输出到rgb
		if(j >= MIDBLUR_DRAW_LINE)
		{
			if(0 == m_PointsLine[j] && m_PointsLine[j-1] != 0)	//点无变化
			{
				m_PointsLine[j] = m_PointsLine[j-1];
			}
		}
	}
	#endif
	for (int j = 0; j < DEEPIMG_WIDTH; j++)
	{
		//绘制出m_PointsLine向量内所有的像素点
		Point P = Point(j, m_PointsLine[j]);
		//输出到rgb
		if(j > MIDBLUR_DRAW_LINE && (j < (DEEPIMG_WIDTH - MIDBLUR_DRAW_LINE)))
		{
			if(abs(m_PointsLine[j+1] - m_PointsLine[j]) > 10 )	//点距相差过大，画线
			{
				Point P2 = Point(j+1, m_PointsLine[j+1]);
				line(depthColor,P,P2,Scalar(0, 128, 128),2);
			}else
				circle(depthColor, P, 0, Scalar(0, 128, 128),2);
		}else
			circle(depthColor, P, 0, Scalar(0, 128, 128),2);
	}
    #if CVIMGSHOW
	imshow("in_add_rect", depthColor);
    #endif
	//emit EmitOutFrameMessage(&depthColor,0);
	//printf("###[%s][%d], EmitOutFrameMessage\n", __func__, __LINE__);
	if (save_frame) {
		LOGD(">>>>>>>>>> write jpgin_add_rect");
		imwrite("jpgin_add_rect.png", depthColor);
	}
	char szsave_jpgin_add[1024] = {0};
	sprintf(szsave_jpgin_add, "jpgin_add_rect%d.jpg", gsave_rect_count);
	if (save_rect_color) {
		LOGD(">>>>>>>>>> write jpgin_add_rect");
		imwrite(szsave_jpgin_add, depthColor);
	}
	//#endif #if 1//先找连通区域
	#if 0
	//lxl log 12-25 MORPH_CLOSE后感觉边界更整齐，have a try
	element = getStructuringElement(MORPH_RECT, Size(morphW, morphH));
	morphologyEx(mat_threshold, mat_threshold, MORPH_CLOSE, element);

	//namedWindow("morphologyEx");
    #if CVIMGSHOW
	imshow("morphologyClose", mat_threshold);
    #endif
	if (save_frame) {
		LOGD(">>>>>>>>>> write morphologyClose");
		imwrite("morphologyClose.png", mat_threshold);
	}
	#endif
	if(depthColor.data != NULL )
	{
		cv::Rect rect(st_SysParam.PixWidth_End, st_SysParam.PixHight_End, depthColor.rows - st_SysParam.PixWidth_End, depthColor.cols - st_SysParam.PixHight_End);
		//Mat imageROI = depthColor(rect);
		g_cvOutimg = depthColor.clone();//imageROI.clone();//depthColor.clone();
		emit EmitOutFrameMessage(&g_cvOutimg,0);
	}
	#if CVIMGSHOW
	cv::imshow("rect_resized_color", In_rgb);
	#endif
	char szsave_rect_color[1024] = {0};
	sprintf(szsave_rect_color, "rect_resized_color%d.jpg", gsave_rect_count);
	if (save_rect_color) {
		LOGD(">>>>>>>>>> write resized_color add rect");
		imwrite(szsave_rect_color, In_rgb);
	}
	return 0;
}

void ImageProcessThread::handleFrame(TY_FRAME_DATA* frame, void* userdata ,void* tempdata,CAMMER_PARA_S st_SysParam ,int devcount)
{
	
	const char* ID = NULL;
	//CallbackData* pData = (CallbackData*)userdata;
    CamInfo* pData = (CamInfo*) userdata;
	//LOGD("=== Get frame %d", ++pData->idx);
	//CAMMER_PARA_S st_SysParam;
	//GetCammerSysParam(&st_SysParam);
	cv::Mat depth, irl, irr, color, point3D;
	parseFrame(*frame, &depth, &irl, &irr, &color, &point3D);
    cv::Mat newDepth,TransDepth,blackDepth;
	cv::Mat resized_color;
	#if 1
	if (!depth.empty()){
				/////lxl add///////
		cv::Mat tempDepth = cv::Mat(depth.rows, depth.cols, CV_16U, (uint16_t *)tempdata);
		Mat mat_blur;
		//mat_blur = depth.clone();
		#if DEEP_DISTANSLINE
		depthTrans_BarrierLine(depth, (uint16_t*)tempdata, &mat_blur, &blackDepth);
		depthTrans_FindLine(mat_blur, devcount);
		GaussianBlur(mat_blur, TransDepth, Size(st_SysParam.GussBlurSize, st_SysParam.GussBlurSize), 0, 0, BORDER_DEFAULT); 	
		#else
		depthTransfer(depth, (uint16_t*)tempdata, &mat_blur, &blackDepth);
		GaussianBlur(mat_blur, TransDepth, Size(st_SysParam.GussBlurSize, st_SysParam.GussBlurSize), 0, 0, BORDER_DEFAULT);		
		#endif
		#if USE_RGBIMG
			cv::resize(color, resized_color, depth.size());
			#if CVIMGSHOW
			cv::imshow("resizedColor", resized_color);
			#endif
			if (save_frame){
				LOGD(">>>>>>>>>> write resized_color");
				imwrite("resized_color.png", resized_color);
				//save_frame = false;
			}
		#endif
		//lxl add output grayimg
		pData->render.SetColorType(DepthRender::COLORTYPE_BLUERED);//(DepthRender::COLORTYPE_GRAY);
		cv::Mat depthColor = pData->render.Compute(TransDepth);
		//lxl modify 2018-12-27 直接显示deep原图
		cv::Mat RawdepthColor = pData->render.Compute(depth);
		if(st_SysParam.CurrentCam <= 0)
			st_SysParam.CurrentCam = 0;
		if(0 == st_SysParam.CurrentCam)
			ID = st_SysParam.id1;
		else if(1 == st_SysParam.CurrentCam)
			ID = st_SysParam.id2;
		else if(2 == st_SysParam.CurrentCam)
			ID = st_SysParam.id3;
		else if(3 == st_SysParam.CurrentCam)
			ID = st_SysParam.id4;
		else if(4 == st_SysParam.CurrentCam)
			ID = st_SysParam.id5;
		if(0 == strcmp(pData->sn,ID))
		{
			g_cvdeepimg = RawdepthColor.clone();
			emit EmitFrameMessage(&g_cvdeepimg,0);
			#if DEEP_DISTANSLINE	
			for (int j = 0; j < DEEPIMG_WIDTH; j++)
			{
				//绘制出m_PointsLine向量内所有的像素点
				m_DistansLine[devcount][j] = DEEPIMG_HEIGHT - m_DistansLine[devcount][j]/5;
				Point P = Point(j, m_DistansLine[devcount][j]);
				//输出到rgb
				if(j > 3 && (j < (DEEPIMG_WIDTH - 3)))
				{
					if(0)//(abs(m_DistansLine[j+1] - m_DistansLine[j]) > 10 
						//&& abs(m_DistansLine[j+1] - m_DistansLine[j]) < DEEPIMG_HEIGHT/3
						//&& m_DistansLine[j] < DEEPIMG_HEIGHT - 100)	//点距相差过大，画线
					{
						Point P2 = Point(j+1, m_DistansLine[devcount][j+1]);
						line(depthColor,P,P2,Scalar(0, 128, 128),2);
					}else
						circle(depthColor, P, 0, Scalar(0, 128, 128),2);
				}else
					circle(depthColor, P, 0, Scalar(0, 128, 128),2);
			}
			for (unsigned int j = 4; j < DEEPIMG_DRAWPOINT +4; j++)
			{
				int y = main_DistPointsLine[devcount][j];
				Point Pdist = Point(j*10, y);
				////circle(depthColor, Pdist, 0, Scalar(160, 32, 240),4);
			}
			#endif
			//printf("###[%s][%d], EmitFrameMessage\n", __func__, __LINE__);
			g_cvRawTempimg = depthColor.clone();
			emit EmitRawTempMessage(&g_cvRawTempimg,0);
		}
		if (save_frame){
			LOGD(">>>>>>>>>> write depthColor");
			imwrite("TransdepthColor.png", depthColor);
			//save_frame = false;
		}
		//g_cvdeepimg = depthColor.clone();	//lxl modify 2018-12-27
		//depthColor = depthColor / 2 + resized_color / 2;
		#if CVIMGSHOW
		cv::imshow("projected depth", depthColor);
		#endif
		//std::cout << "depthColor.channels:" << depthColor.channels() << "  rows:" << depthColor.rows << "  cols:" << depthColor.cols << std::endl;
		if (save_frame){
			LOGD(">>>>>>>>>> write projected_depth");
			imwrite("projected_depth.png", depthColor);
			//save_frame = false;
		}
		//DeepImgFinds_write_rgb(depthColor,resized_color, 3, 7, 7);
		#if DEEP_DISTANSLINE
		#else
		//lxl add at 12-25 使用系统参数
		DeepImgFinds_write_rgb(depthColor,resized_color, st_SysParam.MidBlurSize,
								st_SysParam.MorphCloseSize, st_SysParam.MorphCloseSize);
		#endif
	}
	// do Registration
	#else
	cv::Mat newDepth,TransDepth,blackDepth;
	if (!point3D.empty() && !color.empty()) 
	{
		// ASSERT_OK( TYRegisterWorldToColor2(pData->hDevice, (TY_VECT_3F*)point3D.data, 0
		//             , point3D.cols * point3D.rows, color.cols, color.rows, (uint16_t*)buffer, sizeof(buffer)
		//             ));
		ASSERT_OK(TYRegisterWorldToColor(pData->hDevice, (TY_VECT_3F*)point3D.data, 0
			, point3D.cols * point3D.rows, (uint16_t*)buffer, sizeof(buffer)
			));
		newDepth = cv::Mat(color.rows, color.cols, CV_16U, (uint16_t*)buffer);
		cv::Mat resized_color;
		cv::Mat temp;
		//you may want to use median filter to fill holes in projected depth image or do something else here
		cv::medianBlur(newDepth, temp, 5);
		newDepth = temp;
		//resize to the same size for display
		cv::resize(newDepth, newDepth, depth.size(), 0, 0, 0);
		/////lxl add///////
		//cv::Mat tempDepth = cv::Mat(depth.rows, depth.cols, CV_16U, tempdata);
		//for(int i =0 ; i < depth.rows*depth.cols ; i ++)
		//newDepth = newDepth - tempDepth ;	//深度图减去模板

		depthTransfer(newDepth, (uint16_t*)tempdata, &TransDepth, &blackDepth);
		cv::resize(color, resized_color, depth.size());
		#if CVIMGSHOW
		cv::imshow("resizedColor", resized_color);
        #endif
		if (save_frame){
			LOGD(">>>>>>>>>> write resized_color");
			imwrite("resized_color.png", resized_color);
			//save_frame = false;
		}
		//lxl add output grayimg
		pData->render->SetColorType(DepthRender::COLORTYPE_GRAY);
		cv::Mat depthColor = pData->render->Compute(TransDepth);

		if (save_frame){
			LOGD(">>>>>>>>>> write depthColor");
			imwrite("TransdepthColor.png", depthColor);
			//save_frame = false;
		}
		depthColor = depthColor / 2 + resized_color / 2;
        #if CVIMGSHOW
		cv::imshow("projected depth", depthColor);
        #endif
		std::cout << "depthColor.channels:" << depthColor.channels() << "  rows:" << depthColor.rows << "  cols:" << depthColor.cols << std::endl;
		if (save_frame){
			LOGD(">>>>>>>>>> write projected_depth");
			imwrite("projected_depth.png", depthColor);
			//save_frame = false;
		}
        DeepImgFinds_write_rgb(depthColor,resized_color, 3, 7, 7);
	}
	#endif
	#if USE_RGBIMG
	if (save_frame){
		LOGD(">>>>>>>>>> write images");
		//imwrite("depth.png", newDepth);
		imwrite("color.png", color);
		save_frame = false;
	}
	#endif
	if (save_rect_color) {
		save_rect_color = false;
		gsave_rect_count++;
	}

	//LOGD("=== Callback: Re-enqueue buffer(%p, %d)", frame->userBuffer, frame->bufferSize);
	ASSERT_OK(TYEnqueueBuffer(pData->hDev, frame->userBuffer, frame->bufferSize));
}

void eventCallback(TY_EVENT_INFO *event_info, void *userdata)
{
	if (event_info->eventId == TY_EVENT_DEVICE_OFFLINE) {
		LOGD("=== Event Callback: Device Offline!");
		// Note: 
		//     Please set TY_BOOL_KEEP_ALIVE_ONOFF feature to false if you need to debug with breakpoint!
	}
	else if (event_info->eventId == TY_EVENT_LICENSE_ERROR) {
		LOGD("=== Event Callback: License Error!");
	}
}


ImageProcessThread::ImageProcessThread(QObject *parent) :
    QThread(parent)
{
    bStop = false;
	g_tmpbuffer_0 = (char *)malloc(BUFF_SIZE);
	g_imgtempImgBuf_0 = (char *)malloc(BUFF_SIZE);
	g_tmpbuffer_1 = (char *)malloc(BUFF_SIZE);
	g_imgtempImgBuf_1 = (char *)malloc(BUFF_SIZE);
	g_tmpbuffer_2 = (char *)malloc(BUFF_SIZE);
	g_imgtempImgBuf_2 = (char *)malloc(BUFF_SIZE);
	g_tmpbuffer_3 = (char *)malloc(BUFF_SIZE);
	g_imgtempImgBuf_3 = (char *)malloc(BUFF_SIZE);
	g_tmpbuffer_4 = (char *)malloc(BUFF_SIZE);
	g_imgtempImgBuf_4 = (char *)malloc(BUFF_SIZE);
}
ImageProcessThread::~ImageProcessThread()
{
        // 请求终止
        //QThread->requestInterruption();
        //QThread->quit();
        //QThread->wait();
	free(g_tmpbuffer_0);
	free(g_imgtempImgBuf_0);
	free(g_tmpbuffer_1);
	free(g_imgtempImgBuf_1);
	free(g_tmpbuffer_2);
	free(g_imgtempImgBuf_2);
	free(g_tmpbuffer_3);
	free(g_imgtempImgBuf_3);
	free(g_tmpbuffer_4);
	free(g_imgtempImgBuf_4);
	g_tmpbuffer_0 = NULL;
	g_imgtempImgBuf_0 = NULL;
	g_tmpbuffer_1 = NULL;
	g_imgtempImgBuf_1 = NULL;
	g_tmpbuffer_2 = NULL;
	g_imgtempImgBuf_2 = NULL;
	g_tmpbuffer_3 = NULL;
	g_imgtempImgBuf_3 = NULL;
	g_tmpbuffer_4 = NULL;
	g_imgtempImgBuf_4 = NULL;
}


void ImageProcessThread::stop()
{
    bStop = true;
    //QThread->requestInterruption();

}

void ImageProcessThread::run()
{
	MY_SLEEP_NS(3);
	//const char* IP = NULL;
	const char* HAS_ID = NULL;
	TY_DEV_HANDLE hDevice;	
	int devnum;
	TY_DEVICE_BASE_INFO* pBaseInfo = (TY_DEVICE_BASE_INFO*)buffer;
	CallbackData cb_data[MAX_DEVNUM];
	GetCammerSysParamFile(&g_SysParam);
	CAMMER_PARA_S st_SysParam;
	GetCammerSysParam(&st_SysParam);
	
    qDebug("ImageProcessThread::run=%d\n",__LINE__);
	{
	    QFontDatabase database;
		QString family;
	    foreach(const QString &family, database.families(QFontDatabase::SimplifiedChinese))
	    {
	        qDebug() << family;
	    }
	}
//以上这段代码就可以枚举出系统中所有支持中文的字体名称。

	FILE *filetmp;
    //const char* tempimg = "/home/apple/qt_prj/build-robot01-Desktop_Qt_5_10_1_GCC_64bit-Debug/template.yuv";
    const char* tempimg = "./template.yuv";
	filetmp = fopen( tempimg, "rb");
	if (NULL == filetmp)
	{
		printf("Error:Open tempimg file fail!\n");
        qDebug("Error:Open tempimg file fail#####%s\n",tempimg);
        return ;
	}
	printf("fopen %s ok\n",tempimg);
	if (gm_width*gm_hight * 2 != fread(g_imgtempImgBuf_0, 1, gm_width*gm_hight*2, filetmp))
	{
		//提示文件读取错误  
		fclose(filetmp);
		cout << "fread_s filetmp ERR!!!" << endl;
        return ;
	}
	fclose(filetmp);	

	LOGD("=== Init lib");
	ASSERT_OK(TYInitLib());
	TY_VERSION_INFO* pVer = (TY_VERSION_INFO*)buffer;
	ASSERT_OK(TYLibVersion(pVer));
	LOGD("     - lib version: %d.%d.%d", pVer->major, pVer->minor, pVer->patch);
	printf("st_SysParam.CurrentCam==%d\n",st_SysParam.CurrentCam);
	if(HAS_ID == NULL){
        LOGD("=== Get device info");
        ASSERT_OK( TYGetDeviceNumber(&devnum) );
        LOGD("     - device number %d", devnum);
        ASSERT_OK( TYGetDeviceList(pBaseInfo, 100, &devnum) );

        if(devnum < 1){
            LOGD("=== No device got");
            return ;
        }

		//LOGD("=== st_SysParam device: %s", ID);
		//ID = pBaseInfo[0].id;
	}
	std::vector<CamInfo> cams(devnum);
	for(int i = 0; i < devnum; i++)
	{
        LOGD("=== Open device %d (id: %s)", i, pBaseInfo[i].id);		
        strncpy(cams[i].sn, pBaseInfo[i].id, sizeof(cams[i].sn));
		ASSERT_OK(TYOpenDevice(pBaseInfo[i].id, &cams[i].hDev));

		int32_t allComps;
		ASSERT_OK(TYGetComponentIDs(cams[i].hDev, &allComps));
		if (!(allComps & TY_COMPONENT_RGB_CAM)){
			LOGE("=== Has no RGB camera, allComps==%0X",allComps);
			#if USE_RGBIMG
	        return ;
			#endif
		}

		LOGD("=== Configure components");
		//int32_t componentIDs = TY_COMPONENT_POINT3D_CAM | TY_COMPONENT_RGB_CAM;
	    int32_t componentIDs = TY_COMPONENT_DEPTH_CAM | TY_COMPONENT_IR_CAM_LEFT | TY_COMPONENT_IR_CAM_RIGHT;
		ASSERT_OK(TYEnableComponents(cams[i].hDev, componentIDs));
		#if 1
	    // set cam_gain and laser_power
	    ASSERT_OK(TYSetInt(cams[i].hDev, TY_COMPONENT_IR_CAM_LEFT, TY_INT_GAIN, st_SysParam.Gain_max));  //16/32/64/128/254  3~254
		ASSERT_OK(TYSetInt(cams[i].hDev, TY_COMPONENT_IR_CAM_RIGHT, TY_INT_GAIN, st_SysParam.Gain_max)); //16/32/64/128/254
		ASSERT_OK(TYSetInt(cams[i].hDev, TY_COMPONENT_LASER, TY_INT_LASER_POWER,st_SysParam.Gain_thold_max)); // 1~100
		
		//get the histro 
		ASSERT_OK(TYEnableComponents(cams[i].hDev, TY_COMPONENT_BRIGHT_HISTO));
	    int err = TYSetEnum(cams[i].hDev, TY_COMPONENT_DEPTH_CAM, TY_ENUM_IMAGE_MODE, TY_IMAGE_MODE_640x480);
	    ASSERT(err == TY_STATUS_OK || err == TY_STATUS_NOT_PERMITTED);
		#endif

		LOGD("=== Prepare image buffer");
		int32_t frameSize;

		//frameSize = 1280 * 960 * (3 + 2 + 2);
		ASSERT_OK(TYGetFrameBufferSize(cams[i].hDev, &frameSize));
		LOGD("     - Get size of framebuffer, %d", frameSize);
		LOGD("     - Allocate & enqueue buffers");
		//char* frameBuffer[2];	//not use anymore
		cams[i].fb[0] = new char[frameSize];
		cams[i].fb[1] = new char[frameSize];
		LOGD("     - Enqueue buffer (%p, %d)", cams[i].fb[0], frameSize);
		ASSERT_OK(TYEnqueueBuffer(cams[i].hDev, cams[i].fb[0], frameSize));
		LOGD("     - Enqueue buffer (%p, %d)", cams[i].fb[1], frameSize);
		ASSERT_OK(TYEnqueueBuffer(cams[i].hDev, cams[i].fb[1], frameSize));
		#if 0
		LOGD("=== Register callback");
		LOGD("Note: Callback may block internal data receiving,");
		LOGD("      so that user should not do long time work in callback.");
		LOGD("      To avoid copying data, we pop the framebuffer from buffer queue and");
		LOGD("      give it back to user, user should call TYEnqueueBuffer to re-enqueue it.");
		DepthRender render;
		cb_data[i].index = 0;
		cb_data[i].hDevice = cams[i].hDev;
		cb_data[i].render = &render;
		// ASSERT_OK( TYRegisterCallback(hDevice, frameCallback, &cb_data) );

		LOGD("=== Register event callback");
		LOGD("Note: Callback may block internal data receiving,");
		LOGD("      so that user should not do long time work in callback.");
		ASSERT_OK(TYRegisterEventCallback(cams[i].hDev, eventCallback, NULL));
		#endif
		LOGD("=== Disable trigger mode");
		ASSERT_OK(TYSetBool(cams[i].hDev, TY_COMPONENT_DEVICE, TY_BOOL_TRIGGER_MODE, false));

		LOGD("=== Start capture");
		ASSERT_OK(TYStartCapture(cams[i].hDev));
		#if 0
		LOGD("=== Read color rectify matrix");
		{
			TY_CAMERA_DISTORTION color_dist;
			TY_CAMERA_INTRINSIC color_intri;
			TY_STATUS ret = TYGetStruct(cams[i].hDev, TY_COMPONENT_RGB_CAM, TY_STRUCT_CAM_DISTORTION, &color_dist, sizeof(color_dist));
			ret |= TYGetStruct(cams[i].hDev, TY_COMPONENT_RGB_CAM, TY_STRUCT_CAM_INTRINSIC, &color_intri, sizeof(color_intri));
			if (ret == TY_STATUS_OK)
			{
				cb_data[i].color_intri = color_intri;
				cb_data[i].color_dist = color_dist;
			}
			else
			{ //reading data from device failed .set some default values....
				memset(cb_data[i].color_dist.data, 0, 12 * sizeof(float));
				memset(cb_data[i].color_intri.data, 0, 9 * sizeof(float));
				cb_data[i].color_intri.data[0] = 1000.f;
				cb_data[i].color_intri.data[4] = 1000.f;
				cb_data[i].color_intri.data[2] = 600.f;
				cb_data[i].color_intri.data[5] = 450.f;
			}
		}
		#endif
	}
	LOGD("=== Wait for callback");
	exit_main = false;
	//while (!exit_main)
	//while(!isInterruptionRequested())
	while(1)
	{
		if(bStop)
            break;
		GetCammerSysParam(&st_SysParam);
		for(int i = 0; i < cams.size(); i++) 
		{
			//TY_FRAME_DATA frame;
			int err = 0;
			if(1)//(i == st_SysParam.CurrentCam)
				err = TYFetchFrame(cams[i].hDev, &cams[i].frame, 1000);
			if (err != TY_STATUS_OK) {
				LOGE("Fetch frame error %d at cams[%d]: %s", err,i, TYErrorString(err));
				break;
			}
			else //if(i == st_SysParam.CurrentCam)
			{	//目前先处理选中的相机
	            handleFrame(&cams[i].frame, &cams[i] , (void*)g_imgtempImgBuf_0 ,st_SysParam ,i);
			}
			//LOGD("Fetch frame sucess %d at cams[%d]", err,i);
			if(g_IsTemp_btn >= 0)
			{
				//Open_TempImg();
				//for(int i = 0; i < MAX_DEVNUM; i++)
				{	//哪个相机的模板换了就替换哪个buf
					memcpy(g_imgtempImgBuf_0, g_tmpbuffer_0, BUFF_SIZE);
				}
				g_IsTemp_btn  = -1;
			}
			if(1)//(m_PointsLine[0] != 0)	//modify at 2019-1-26
			{
				memcpy(&main_PointsLine[i], &m_PointsLine[i], sizeof(int)*DEEPIMG_WIDTH);
				g_is_point_OK = 1;
			}
	        MY_SLEEP_MS(1);
	        //usleep(2000);
		}
	}
	for(int i = 0; i < devnum; i++)
	{
		ASSERT_OK(TYStopCapture(cams[i].hDev));
		ASSERT_OK(TYCloseDevice(cams[i].hDev));
		ASSERT_OK(TYDeinitLib());
		delete cams[i].fb[0];
		delete cams[i].fb[1];
	}

	LOGD("=== Main done!");
}


