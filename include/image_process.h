#ifndef IMAGE_PROCESS_H
#define IMAGE_PROCESS_H

#include <QThread>
#include <functional>
#include <QtCore/QThread>
#include <QWaitCondition>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>

#include "define.h"

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
	int DeepImgFinds_write_rgb(Mat depthColor, Mat resized_color, int blurSize, int morphW, int morphH);
	void eventCallback(TY_EVENT_INFO *event_info, void *userdata);
	bool verifySizes(Rect mr);
	bool calcSafeRect(const RotatedRect &roi_rect, const Mat &src,Rect_<float> &safeBoundRect);
	void depthTransfer(cv::Mat depth, uint16_t* t_data, cv::Mat* newDepth, cv::Mat* blackDepth);
	
signals: 
	void EmitFrameMessage(cv::Mat* stFrameItem, int nCh = 0); 

};
#endif // IMAGE_PROCESS_H
