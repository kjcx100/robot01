#ifndef IMAGE_PROCESS_H
#define IMAGE_PROCESS_H

#include <QThread>
#include <functional>
#include <QtCore/QThread>
#include <QWaitCondition>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>

#include "define.h"
#include "../common/common.hpp"

int gm_width = 640;
int gm_hight = 480;

void MY_SLEEP_MS(int ms);
void MY_SLEEP_NS(int s);

class BTCommunThread : public QThread
{
    Q_OBJECT
public:
    bool bStop ;
    explicit BTCommunThread(QObject *parent = 0);
    void run();
    void stop();
    //int CheckRFCard(unsigned char *pCardSN, unsigned char SNLen);

signals:


};
class ImageProcessThread : public QThread
{
    Q_OBJECT

public:
    bool bStop ;
    explicit ImageProcessThread(QObject *parent = 0);
    void run();
    void stop();	
	void handleFrame(TY_FRAME_DATA* frame, void* userdata ,void* tempdata);
	int DeepImgFinds_write_rgb(cv::Mat depthColor, cv::Mat resized_color, int blurSize, int morphW, int morphH);
	//void eventCallback(TY_EVENT_INFO *event_info, void *userdata);
	bool verifySizes(cv::Rect mr);
	bool calcSafeRect(const cv::RotatedRect &roi_rect, const cv::Mat &src,cv::Rect_<float> &safeBoundRect);
	void depthTransfer(cv::Mat depth, uint16_t* t_data, cv::Mat* newDepth, cv::Mat* blackDepth);
	cv::Mat cvMatRect2Tetra(cv::Mat mtxSrc, int iDstX1, int iDstY1, int iDstX2, int iDstY2,
						int iDstX3, int iDstY3, int iDstX4, int iDstY4, int iDstWidth, int iDstHeight);
signals: 
	void EmitFrameMessage(cv::Mat* stFrameItem, int nCh = 0); 
	void EmitOutFrameMessage(cv::Mat* stFrameItem, int nCh = 0); 

private:
	int m_PointsLine[gm_width];	//存储障碍物曲线的points

};
#endif // IMAGE_PROCESS_H
