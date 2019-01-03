#include "../common/common.hpp"
#include<stdio.h>
#include<string.h>
#include<algorithm>
#include<opencv2/opencv.hpp>
#include<opencv2/calib3d/calib3d.hpp>

#define TEMPLATE_DEBUG
#define		DEEPIMG_WIDTH	640
#define		DEEPIMG_HEIGHT	480

static char buffer[1024*1024];
static bool fakeLock = false; // NOTE: fakeLock may lock failed

cv::Mat  grayImage, out_Canny;
int min_Thresh = 3;
int max_Thresh = 30;

cv::Mat org,dst,img,tmp;

struct CallbackData {
    int             index;
    TY_DEV_HANDLE   hDevice;
    DepthRender*    render;
    bool            saveFrame;
    int             saveIdx;
    cv::Mat         depth;
    cv::Mat         leftIR;
    cv::Mat         rightIR;
    cv::Mat         color;
    cv::Mat         point3D;
};

void frameCallback(TY_FRAME_DATA* frame, void* userdata)
{
    CallbackData* pData = (CallbackData*) userdata;
    LOGD("=== Get frame %d", ++pData->index);

    while(fakeLock){
        MSLEEP(10);
    }
    fakeLock = true;

    pData->depth.release();
    pData->leftIR.release();
    pData->rightIR.release();
    pData->color.release();
    pData->point3D.release();

    parseFrame(*frame, &pData->depth, &pData->leftIR, &pData->rightIR
            , &pData->color, &pData->point3D);

    fakeLock = false;

    if(!pData->color.empty()){
        LOGI("Color format is %s", colorFormatName(TYImageInFrame(*frame, TY_COMPONENT_RGB_CAM)->pixelFormat));
    }

    LOGD("=== Callback: Re-enqueue buffer(%p, %d)", frame->userBuffer, frame->bufferSize);
    ASSERT_OK( TYEnqueueBuffer(pData->hDevice, frame->userBuffer, frame->bufferSize) );
}

void eventCallback(TY_EVENT_INFO *event_info, void *userdata)
{
    if (event_info->eventId == TY_EVENT_DEVICE_OFFLINE) {
        LOGD("=== Event Calllback: Device Offline!");
    }
}

void depthTransfer(cv::Mat depth, uint16_t* t_data, cv::Mat* newDepth, cv::Mat* blackDepth)
{
	int i=0;
	uint16_t dst_data[480*640];
	uint16_t blk_data[480*640];
	uint16_t* src_data;
	uint16_t treshhold;
	
	src_data = (uint16_t *)depth.data;
	memset(blk_data,0,480*640*2);
	for(i=0;i<(480*640);i++)
	{
		treshhold = (uint16_t)(t_data[i]*0.03);
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
			dst_data[i] = 1200;
		}
		else if((src_data[i] - t_data[i]) > (treshhold * 1.5))
		{
			dst_data[i] = 2500;
		}
		else
		{
			dst_data[i] = 0;
		}
		/*
		if(i == (240*640 + 320))
		{
			LOGD("--------------------- treshhold:%d, src_data[i]-t_data[i]:%d", treshhold, (src_data[i] - t_data[i]));
			LOGD("---------------------------------------------- dst_data[]:%d", dst_data[i]);
		}
		*/
		//dst_data[i] = src_data[i] - t_data[i];
	}
	
	dst_data[0] = 500;
	dst_data[1] = 4000;
	
	blk_data[0] = 500;
	blk_data[1] = 4000;
	
	*newDepth = cv::Mat(480, 640, CV_16U, dst_data);
	*blackDepth = cv::Mat(480, 640, CV_16U, blk_data);
}

void combineDepth(cv::Mat depth1, cv::Mat depth2, cv::Mat* dst_depth)
{
	int i=0;
	uint16_t* src_data1;
	uint16_t* src_data2;
	uint16_t dst_data[480*640];
	
	src_data1 = (uint16_t *)depth1.data;
	src_data2 = (uint16_t *)depth2.data;
	for(i=0;i<(480*640);i++)
	{
		dst_data[i] = src_data1[i] + src_data2[i];
	}
	
	*dst_depth = cv::Mat(480, 640, CV_16U, dst_data);
}

void Find_Draw_COntours(int, void*)
{
	Canny(grayImage, out_Canny, min_Thresh, max_Thresh *2, 3);
	imshow("window2", out_Canny);
}

void show_template(DepthViewer* depth_view,int counter)
{
	uint16_t data[DEEPIMG_HEIGHT*DEEPIMG_WIDTH];
	uint16_t count[DEEPIMG_HEIGHT*DEEPIMG_WIDTH];
	uint32_t temp[DEEPIMG_HEIGHT*DEEPIMG_WIDTH];
	uint16_t avg_data[DEEPIMG_HEIGHT*DEEPIMG_WIDTH];
	uint8_t u8_data[DEEPIMG_HEIGHT*DEEPIMG_WIDTH];
	char num_arr[10];
	int i=0,j=0;
	FILE *fp;
	FILE *avg_fp;
	uint16_t  min_count = 0;
	char min_count_str[10];
	
	//DepthViewer depth_view;
	cv::Mat image;
	
	if(count <= 0)
	{
		return;
	}
	
	memset(count,0,DEEPIMG_HEIGHT*DEEPIMG_WIDTH*2);
	memset(temp,0,DEEPIMG_HEIGHT*DEEPIMG_WIDTH*4);
	for(i=0;i<counter;i++)
	{
		sprintf(num_arr, "front_depth_img/%d.img", i);
		
		fp = fopen(num_arr, "r+");
		fseek(fp, SEEK_SET, 0);
    	fread(data, 2, DEEPIMG_HEIGHT*DEEPIMG_WIDTH, fp);

    	fclose(fp);
    	for(j=0;j<DEEPIMG_HEIGHT*DEEPIMG_WIDTH;j++)
    	{
    		if(data[j] > 0)
    		{
    			temp[j] = temp[j] + data[j];
    			count[j] = count[j]+1;
    		}
    	}
	}
	
	for(i=0;i<DEEPIMG_HEIGHT;i++)
	{
		uint32_t valid_value = 0;
		uint16_t valid_count_avg = 0;
		uint16_t valid_count = 0;
		for(j=0;j<DEEPIMG_WIDTH;j++)
		{
			if((((i > 430) && (j >= 260) && (j <= 332)) || ((i >= 408) && (j >= 518)) || (i <= 50)) && (temp[i * DEEPIMG_WIDTH + j] == 0))
			{
				count[i * DEEPIMG_WIDTH + j] = counter;
			}
			
			if((temp[i * DEEPIMG_WIDTH + j] > 0) && (count[i * DEEPIMG_WIDTH + j] > 0))
			{
				valid_value = valid_value + temp[i * DEEPIMG_WIDTH + j];
				valid_count_avg = valid_count_avg + count[i * DEEPIMG_WIDTH + j];
				valid_count++;
			}
			
		}
		LOGD("------------------total valid_value and valid_count_avg and valid_count: %d.%d.%d", valid_value, valid_count_avg, valid_count);
		if(valid_count == 0)
		{
			if((i < 5) || (i>440))
			{
				valid_count = counter;
			}
			else
			{
				valid_count = 1;
			}
		}
		valid_value = valid_value/valid_count;
		valid_count_avg = valid_count_avg/valid_count;
		//valid_value = 1000;
		//valid_count_avg = 5;
		for(j=0;j<DEEPIMG_WIDTH;j++)
		{
			if(i <= 100)
			{
				if(temp[i * DEEPIMG_WIDTH + j] == 0)
				{
					temp[i * DEEPIMG_WIDTH + j] = valid_value;
					count[i * DEEPIMG_WIDTH + j] = valid_count_avg;
				}
			}
		}
	}
	
	for(i=0;i<DEEPIMG_HEIGHT*DEEPIMG_WIDTH;i++)
    {
    	if(count[i] > 0)
    	{
    		avg_data[i] = (uint16_t)(temp[i]/count[i]);
    	}
    	else
    	{
    		avg_data[i] = 0;
    	}
    }
    
    min_count = *(std::min_element(count,count+DEEPIMG_HEIGHT*DEEPIMG_WIDTH));
    sprintf(min_count_str, "%d", min_count);
    
    //for(i=0;i<(480*640);i++)
	//{
	//	u8_data[i] = avg_data[i]/9;
	//}
	//image = cv::Mat(480, 640, CV_8U, u8_data);
	
	//cv::namedWindow("Display Image", cv::WINDOW_AUTOSIZE );
    //cv::imshow("Display Image", image);
    
    image = cv::Mat(DEEPIMG_HEIGHT, DEEPIMG_WIDTH, CV_16U, avg_data);
    
    avg_fp = fopen("front_depth_img/template.img", "wb");
    fwrite(avg_data, 2, DEEPIMG_HEIGHT*DEEPIMG_WIDTH, avg_fp);
    fclose(avg_fp);
    
    //cv::putText(image,min_count_str,cv::Point(2,50),cv::FONT_HERSHEY_SIMPLEX,1,cv::Scalar(255,0,0),2);
    //cv::namedWindow("Display Image", cv::WINDOW_AUTOSIZE );
    depth_view->show("Display Image", image);
    //cv::imshow("Display Image", image);
    //cv::waitKey(10);
}

void show_template_origin(DepthViewer* depth_view,int counter)
{
	uint16_t data[480*640];
	uint16_t count[480*640];
	uint32_t temp[480*640];
	uint16_t avg_data[480*640];
	uint8_t u8_data[480*640];
	char num_arr[10];
	int i=0,j=0;
	FILE *fp;
	FILE *avg_fp;
	uint16_t  min_count = 0;
	char min_count_str[10];
	
	//DepthViewer depth_view;
	cv::Mat image;
	
	if(count <= 0)
	{
		return;
	}
	
	memset(count,0,480*640*2);
	memset(temp,0,480*640*4);
	for(i=0;i<counter;i++)
	{
		sprintf(num_arr, "depth_img1/%d.img", i);
		
		fp = fopen(num_arr, "r+");
		fseek(fp, SEEK_SET, 0);
    	fread(data, 2, 480*640, fp);

    	fclose(fp);
    	for(j=0;j<480*640;j++)
    	{
    		if(data[j] > 0)
    		{
    			temp[j] = temp[j] + data[j];
    			count[j] = count[j]+1;
    		}
    	}
	}
	
	for(i=0;i<480;i++)
	{
		uint32_t valid_value = 0;
		uint16_t valid_count_avg = 0;
		uint16_t valid_count = 0;
		for(j=0;j<640;j++)
		{
			if(j>=68)
			{
				if((((i > 440) && (j >= 330) && (j <= 402)) || ((i >= 478) && (j >= 588)) || (i <= 60)) && (temp[i * 640 + j] == 0))
				{
					count[i * 640 + j] = counter;
				}
			
				if((temp[i * 640 + j] > 0) && (count[i * 640 + j] > 0))
				{
					valid_value = valid_value + temp[i * 640 + j];
					valid_count_avg = valid_count_avg + count[i * 640 + j];
					valid_count++;
				}
			}
			
		}
		LOGD("------------------total valid_value and valid_count_avg and valid_count: %d.%d.%d", valid_value, valid_count_avg, valid_count);
		if(valid_count == 0)
		{
			if((i < 15) || (i>DEEPIMG_HEIGHT))
			{
				valid_count = counter;
			}
			else
			{
				valid_count = 1;
			}
		}
		valid_value = valid_value/valid_count;
		valid_count_avg = valid_count_avg/valid_count;
		//valid_value = 1000;
		//valid_count_avg = 5;
		for(j=0;j<640;j++)
		{
			if(i <= 100)
			{
				if(j >= 68)
				{
					if(temp[i * 640 + j] == 0)
					{
						temp[i * 640 + j] = valid_value;
						count[i * 640 + j] = valid_count_avg;
					}
				}
				else
				{
					temp[i * 640 + j] = 0;
					count[i * 640 + j] = 1;
				}
			}
		}
	}
	
	for(i=0;i<480*640;i++)
    {
    	if(count[i] > 0)
    	{
    		avg_data[i] = (uint16_t)(temp[i]/count[i]);
    	}
    	else
    	{
    		avg_data[i] = 0;
    	}
    }
    
    min_count = *(std::min_element(count,count+480*640));
    sprintf(min_count_str, "%d", min_count);
    
    //for(i=0;i<(480*640);i++)
	//{
	//	u8_data[i] = avg_data[i]/9;
	//}
	//image = cv::Mat(480, 640, CV_8U, u8_data);
	
	//cv::namedWindow("Display Image", cv::WINDOW_AUTOSIZE );
    //cv::imshow("Display Image", image);
    
    image = cv::Mat(480, 640, CV_16U, avg_data);
    
    avg_fp = fopen("template1.img", "wb");
    fwrite(avg_data, 2, 480*640, avg_fp);
    fclose(avg_fp);
    
    cv::putText(image,min_count_str,cv::Point(2,50),cv::FONT_HERSHEY_SIMPLEX,1,cv::Scalar(255,0,0),2);
    //cv::namedWindow("Display Image", cv::WINDOW_AUTOSIZE );
    depth_view->show("Display Image", image);
    //cv::imshow("Display Image", image);
    //cv::waitKey(10);
}


//resize color mat
void color_pic_resize(cv::Mat srcMat, cv::Mat* dstMat)
{
	uint8_t dstData[1080*690*3];
	uint8_t srcData[1280*960*3];
	int i=0,j=0;
	int k=0;
	float rate = 0;
	int dest_line;
	int dest_row;
	int dstCount = 0;
	
	memcpy(srcData,srcMat.data,1280*960*3);
	for(i=140;i<830;i++)
	{
		rate = 0.685185 + (((1 - 0.685185)/690) * (i - 140));
		for(j=100;j<1180;j++)
		{
			dest_line = (int)((rate * j) + ((1 - rate) * 640));
			dest_row  = i;
			dstData[dstCount] = srcData[((dest_row * 1280) + dest_line) * 3];
			dstData[dstCount+1] = srcData[(((dest_row * 1280) + dest_line) * 3) + 1];
			dstData[dstCount+2] = srcData[(((dest_row * 1280) + dest_line) * 3) + 2];
			//dstData[dstCount+3] = 100;
			k=k+1;
			dstCount = k*3;
		}
	}
	
	*dstMat = cv::Mat(690, 1080, CV_8UC3, dstData);
}

//check color mat
void color_pic_check(cv::Mat srcMat, cv::Mat* dstMat)
{
	uint8_t dstData[540*540*3];
	uint8_t srcData[540*540*3];
	int i=0,j=0;
	float angle_mx,angle_mn,angle,rate;
	int subscript = 0;
	
	angle_mx = atan(94/76.0);
	angle_mn = atan(34/76.0);
	rate = (angle_mx - angle_mn)/540.0;
	memcpy(srcData,srcMat.data,540*540*3);
	for(i=0;i<540;i++)
	{
		angle = angle_mx - (i * rate);
		subscript = (int)(((94 - (tan(angle) * 76))/60.0000) * 540);
		for(j=0;j<540;j++)
		{
			dstData[((subscript*540) + j) * 3] = srcData[((i*540) + j) * 3];
			dstData[(((subscript*540) + j) * 3) + 1] = srcData[(((i*540) + j) * 3) + 1];
			dstData[(((subscript*540) + j) * 3) + 2] = srcData[(((i*540) + j) * 3) + 2];
		}
	}
	
	*dstMat = cv::Mat(540, 540, CV_8UC3, dstData);
}

void camera_calibration(cv::Mat srcMat, cv::Mat* dstMat)
{
	/// 读取一副图片，不改变图片本身的颜色类型（该读取方式为DOS运行模式）
	cv::Mat src = srcMat;
	cv::Mat distortion = src.clone();
	cv::Mat camera_matrix = cv::Mat(3, 3, CV_32FC1);
	cv::Mat distortion_coefficients;

	//导入相机内参和畸变系数矩阵
	cv::FileStorage file_storage("out_camera_data.xml", cv::FileStorage::READ);
	file_storage["Camera_Matrix"] >> camera_matrix;
	file_storage["Distortion_Coefficients"] >> distortion_coefficients;
	file_storage.release();
	
	//矫正
	undistort(src, distortion, camera_matrix, distortion_coefficients);
	
	*dstMat = distortion;
}

//the mouse move callback function
void on_mouse(int event,int x,int y,int flags,void *ustc)//event鼠标事件代号，x,y鼠标坐标，flags拖拽和键盘操作的代号  
{  
    static cv::Point pre_pt = cv::Point(-1,-1);//初始坐标
    static cv::Point cur_pt = cv::Point(-1,-1);//实时坐标
    char temp[16];  
    if (event == CV_EVENT_LBUTTONDOWN)//左键按下，读取初始坐标，并在图像上该点处划圆  
    {  
        org.copyTo(img);//将原始图片复制到img中  
        sprintf(temp,"(%d,%d)",x,y);  
        pre_pt = cv::Point(x,y);  
        cv::putText(img,temp,pre_pt,cv::FONT_HERSHEY_SIMPLEX,0.5,cv::Scalar(0,0,0,255),1,8);//在窗口上显示坐标  
        cv::circle(img,pre_pt,2,cv::Scalar(255,0,0,0),CV_FILLED,CV_AA,0);//划圆  
        cv::imshow("img",img);  
    }  
    else if (event == CV_EVENT_MOUSEMOVE && !(flags & CV_EVENT_FLAG_LBUTTON))//左键没有按下的情况下鼠标移动的处理函数  
    {  
        img.copyTo(tmp);//将img复制到临时图像tmp上，用于显示实时坐标  
        sprintf(temp,"(%d,%d)",x,y);  
        cur_pt = cv::Point(x,y);  
        cv::putText(tmp,temp,cur_pt,cv::FONT_HERSHEY_SIMPLEX,0.5,cv::Scalar(0,0,0,255));//只是实时显示鼠标移动的坐标  
        cv::imshow("img",tmp);  
    }  
    else if (event == CV_EVENT_MOUSEMOVE && (flags & CV_EVENT_FLAG_LBUTTON))//左键按下时，鼠标移动，则在图像上划矩形  
    {  
        img.copyTo(tmp);  
        sprintf(temp,"(%d,%d)",x,y);  
        cur_pt = cv::Point(x,y);  
        cv::putText(tmp,temp,cur_pt,cv::FONT_HERSHEY_SIMPLEX,0.5,cv::Scalar(0,0,0,255));  
        cv::rectangle(tmp,pre_pt,cur_pt,cv::Scalar(0,255,0,0),1,8,0);//在临时图像上实时显示鼠标拖动时形成的矩形  
        cv::imshow("img",tmp);  
    }  
    else if (event == CV_EVENT_LBUTTONUP)//左键松开，将在图像上划矩形  
    {  
        org.copyTo(img);  
        sprintf(temp,"(%d,%d)",x,y);  
        cur_pt = cv::Point(x,y);  
        cv::putText(img,temp,cur_pt,cv::FONT_HERSHEY_SIMPLEX,0.5,cv::Scalar(0,0,0,255));  
        cv::circle(img,pre_pt,2,cv::Scalar(255,0,0,0),CV_FILLED,CV_AA,0);  
        cv::rectangle(img,pre_pt,cur_pt,cv::Scalar(0,255,0,0),1,8,0);//根据初始点和结束点，将矩形画到img上  
        cv::imshow("img",img);  
        img.copyTo(tmp);  
        //截取矩形包围的图像，并保存到dst中  
        int width = abs(pre_pt.x - cur_pt.x);  
        int height = abs(pre_pt.y - cur_pt.y);  
        if (width == 0 || height == 0)  
        {  
            printf("width == 0 || height == 0");  
            return;  
        }  
        dst = org(cv::Rect(std::min(cur_pt.x,pre_pt.x),std::min(cur_pt.y,pre_pt.y),width,height));  
        cv::namedWindow("dst");  
        cv::imshow("dst",dst);  
        cv::waitKey(0);  
    }  
}


int main(int argc, char* argv[])
{
    const char* IP = NULL;
    const char* ID = NULL;
    TY_DEV_HANDLE hDevice;
    cv::Mat medium_resut1,medium_resut2,newDepth,blackDepth,dst_Depth,single_blackDepth,all_blackDepth;
    cv::Mat resized_color,calibration_color;
    DepthRender _render;
    int key_value = 0;
    FILE *fp;
    uint16_t t_data[480*640];
    bool m_display = false;
    bool m_color_set = false;
    int m_color_count = 0;

    for(int i = 1; i < argc; i++){
        if(strcmp(argv[i], "-id") == 0){
            ID = argv[++i];
        }else if(strcmp(argv[i], "-ip") == 0){
            IP = argv[++i];
        }else if(strcmp(argv[i], "-h") == 0){
            LOGI("Usage: SimpleView_Callback [-h] [-ip <IP>]");
            return 0;
        }
    }
    
    LOGD("=== Init lib for tempyuv");
    ASSERT_OK( TYInitLib() );
    TY_VERSION_INFO* pVer = (TY_VERSION_INFO*)buffer;
    ASSERT_OK( TYLibVersion(pVer) );
    LOGD("     - lib version: %d.%d.%d", pVer->major, pVer->minor, pVer->patch);

    if(IP) {
        LOGD("=== Open device %s", IP);
        ASSERT_OK( TYOpenDeviceWithIP(IP, &hDevice) );
    } else {
        if(ID == NULL){
            LOGD("=== Get device info");
            int n;
            ASSERT_OK( TYGetDeviceNumber(&n) );
            LOGD("     - device number %d", n);

            TY_DEVICE_BASE_INFO* pBaseInfo = (TY_DEVICE_BASE_INFO*)buffer;
            ASSERT_OK( TYGetDeviceList(pBaseInfo, 100, &n) );

            if(n == 0){
                LOGD("=== No device got");
                return -1;
            }
            ID = pBaseInfo[0].id;
        }

        LOGD("=== Open device: %s", ID);
        ASSERT_OK( TYOpenDevice(ID, &hDevice) );
    }

#ifdef DEVELOPER_MODE
    LOGD("=== Enter Developer Mode");
    ASSERT_OK(TYEnterDeveloperMode(hDevice));
#endif

    int32_t allComps;
    ASSERT_OK( TYGetComponentIDs(hDevice, &allComps) );
    if(allComps & TY_COMPONENT_RGB_CAM){
        LOGD("=== Has RGB camera, open RGB cam");
        ASSERT_OK( TYEnableComponents(hDevice, TY_COMPONENT_RGB_CAM) );
    }

    LOGD("=== Configure components, open depth cam");
    int32_t componentIDs = TY_COMPONENT_DEPTH_CAM | TY_COMPONENT_IR_CAM_LEFT | TY_COMPONENT_IR_CAM_RIGHT;
    // int32_t componentIDs = TY_COMPONENT_DEPTH_CAM;
    // int32_t componentIDs = TY_COMPONENT_DEPTH_CAM | TY_COMPONENT_IR_CAM_LEFT;
    ASSERT_OK( TYEnableComponents(hDevice, componentIDs) );

    int err = TYSetEnum(hDevice, TY_COMPONENT_DEPTH_CAM, TY_ENUM_IMAGE_MODE, TY_IMAGE_MODE_640x480);
    ASSERT(err == TY_STATUS_OK || err == TY_STATUS_NOT_PERMITTED);

    LOGD("=== Prepare image buffer");
    int32_t frameSize;
    ASSERT_OK( TYGetFrameBufferSize(hDevice, &frameSize) );
    LOGD("     - Get size of framebuffer, %d", frameSize);
    ASSERT( frameSize >= 640*480*2 );

    LOGD("     - Allocate & enqueue buffers");
    char* frameBuffer[2];
    frameBuffer[0] = new char[frameSize];
    frameBuffer[1] = new char[frameSize];
    LOGD("     - Enqueue buffer (%p, %d)", frameBuffer[0], frameSize);
    ASSERT_OK( TYEnqueueBuffer(hDevice, frameBuffer[0], frameSize) );
    LOGD("     - Enqueue buffer (%p, %d)", frameBuffer[1], frameSize);
    ASSERT_OK( TYEnqueueBuffer(hDevice, frameBuffer[1], frameSize) );

    LOGD("=== Register frame callback");
    LOGD("Note:  user should call TYEnqueueBuffer to re-enqueue frame buffer.");
    DepthRender render;
    CallbackData cb_data;
    cb_data.index = 0;
    cb_data.hDevice = hDevice;
    cb_data.render = &render;
    cb_data.saveFrame = false;
    cb_data.saveIdx = 0;
    ASSERT_OK( TYRegisterCallback(hDevice, frameCallback, &cb_data) );

    LOGD("=== Register event callback");
    LOGD("Note: Callback may block internal data receiving,");
    LOGD("      so that user should not do long time work in callback.");
    ASSERT_OK(TYRegisterEventCallback(hDevice, eventCallback, NULL));

    LOGD("=== Disable trigger mode");
    //ASSERT_OK( TYSetBool(hDevice, TY_COMPONENT_DEVICE, TY_BOOL_TRIGGER_MODE, false) );

    LOGD("=== Start capture");
    ASSERT_OK( TYStartCapture(hDevice) );

    LOGD("=== Wait for callback");
    bool exit_main = false;
    DepthViewer depthViewer;

#ifndef TEMPLATE_DEBUG
    /* 打开文件用于读写 */
	fp = fopen("template.img", "r+");
	/* 查找文件的开头 */
   	fseek(fp, SEEK_SET, 0);
   	/* 读取并显示数据 */
	fread(t_data, 2, 480*640, fp);
	
	fclose(fp);
#endif

	cv::Mat camera_matrix = cv::Mat(3, 3, CV_32FC1);
	cv::Mat distortion_coefficients;

	//导入相机内参和畸变系数矩阵
	cv::FileStorage file_storage("out_camera_data.xml", cv::FileStorage::READ);
	file_storage["Camera_Matrix"] >> camera_matrix;
	file_storage["Distortion_Coefficients"] >> distortion_coefficients;
	file_storage.release();
    
    while(!exit_main){
        while(fakeLock){
            MSLEEP(10);
        }
        fakeLock = true;

        if(!cb_data.depth.empty()){
            depthViewer.show("depth", cb_data.depth);
            
            if(m_display)
            {
            	//show_template(&depthViewer,(cb_data.saveIdx-1));
            	show_template(&depthViewer,(cb_data.saveIdx-1));
            	m_display = false;
            }
            //cv::GaussianBlur(cb_data.depth, medium_resut, cv::Size(11, 11), 0);
			//depthViewer.show("depth_Gaussian11", medium_resut);
			//cv::GaussianBlur(cb_data.depth, medium_resut, cv::Size(3, 3), 0);
			//depthViewer.show("depth_Gaussian3", medium_resut);
#ifndef TEMPLATE_DEBUG	
			depthTransfer(cb_data.depth, t_data, &newDepth, &blackDepth);
			depthViewer.show("newDepth", newDepth);
			depthViewer.show("blackDepth", blackDepth);
			
			cv::GaussianBlur(newDepth, medium_resut1, cv::Size(3, 3), 0);
			//depthViewer.show("newDepth_Gaussian3", medium_resut1);
			
			//cv::GaussianBlur(blackDepth, medium_resut2, cv::Size(3, 3), 0);
			cv::medianBlur(blackDepth,medium_resut2,5);
			depthViewer.show("blackDepth_medium5", medium_resut2);
			
			combineDepth(medium_resut1, medium_resut2, &dst_Depth);
			cv::Rect rect(70, 10, DEEPIMG_WIDTH, DEEPIMG_HEIGHT);
			dst_Depth = dst_Depth(rect);
			depthViewer.show("combineDepth", dst_Depth);
			
			if(!cb_data.color.empty())
			{
				cv::imshow("color", cb_data.color);
				//cv::resize(dst_Depth, dst_Depth, dst_Depth.size(), 0, 0, 0);
				cv::Rect color_rect(165, 0, 1025, 840);
				cv::resize(cb_data.color(color_rect), resized_color, dst_Depth.size());
				cv::imshow("resized_color", resized_color);
				//check_Black_Depth(dst_Depth,resized_color,&single_blackDepth,&all_blackDepth);
				//depthViewer.show("single_blackDepth", single_blackDepth);
				//depthViewer.show("all_blackDepth", all_blackDepth);
				
				cv::cvtColor(resized_color, grayImage, cv::COLOR_BGR2GRAY);
				cv::blur(grayImage, grayImage, cv::Size(7, 7));
				//cv::imshow("grayImage", grayImage);
				//check_Black_Depth(dst_Depth,grayImage,&single_blackDepth,&all_blackDepth);
				//depthViewer.show("single_blackDepth", single_blackDepth);
				//depthViewer.show("all_blackDepth", all_blackDepth);
				
				Canny(grayImage, out_Canny, min_Thresh, max_Thresh *2, 3);
				cv::Mat gray2Color;
				cv::cvtColor(out_Canny, gray2Color, cv::COLOR_GRAY2BGR);
				cv::imshow("gray2Color", gray2Color);
				//Find_Draw_COntours(0, 0);
				//resized_color = resized_color(rect);
				cv::Mat depthColor = _render.Compute(dst_Depth);
				
				depthColor = depthColor / 2 + gray2Color / 2;
				cv::imshow("depth_color", depthColor);
			}
#endif
        }
        if(!cb_data.point3D.empty())
        {
        	//cv::imshow("point3D", cb_data.point3D);
        	//depthViewer.show("point3D", cb_data.point3D);
        }
        if(!cb_data.leftIR.empty()){
            //cv::imshow("LeftIR", cb_data.leftIR);
        }
        if(!cb_data.rightIR.empty()){
            //cv::imshow("RightIR", cb_data.rightIR);
        }
        if(!cb_data.color.empty()){
        	cv::Mat dest_mat,dest_resized_Mat,rect_Mat,checked_Mat,medium_Mat,pointed_Mat;
            cv::imshow("color", cb_data.color);
            
            //camera_calibration(cb_data.color, &calibration_color);
            cv::Mat src = cb_data.color;
            cv::Mat distortion = src.clone();
            
            //矫正
			cv::undistort(src, distortion, camera_matrix, distortion_coefficients);
            
            cv::namedWindow("calibration_color",CV_GUI_EXPANDED);
            cv::imshow("calibration_color", distortion);
            
            /*
            org = distortion;
            org.copyTo(img);  
			org.copyTo(tmp);  
			cv::namedWindow("img");//定义一个img窗口  
    		cv::setMouseCallback("img",on_mouse,0);//调用回调函数  

    		cv::imshow("img",img);
    		//cv::waitKey(0);  
            */
            
            //cv::resize(cb_data.color, dest_mat, cv::Size(224,224));
            //dest_mat = cb_data.color;
            color_pic_resize(distortion, &dest_resized_Mat);
            //cv::resize(cb_data.color, dest_resized_Mat, cv::Size(1100,540));
            
            cv::imshow("resized_color", dest_resized_Mat);
            
            cv::resize(dest_resized_Mat, rect_Mat, cv::Size(540,540));
            
            //check mat data
            color_pic_check(rect_Mat, &checked_Mat);
            
            cv::medianBlur(checked_Mat,medium_Mat,5);
            
            cv::imshow("rect_color", rect_Mat);
            
            cv::imshow("checked_color", checked_Mat);
            
            cv::imshow("medium_color", medium_Mat);
            
            cv::resize(medium_Mat, pointed_Mat, cv::Size(100,100));
            
            cv::imshow("min_color", pointed_Mat);
            /*
            cv::namedWindow("window1",1);
			cv::imshow("window1", cb_data.color);
			
			//cv::Rect color_rect(165, 0, 1025, 840);
			//cv::resize(cb_data.color(color_rect), resized_color, cb_data.depth.size());

			cv::cvtColor(cb_data.color, grayImage, cv::COLOR_BGR2GRAY);
			//cv::imshow("gray_image", grayImage);
			cv::blur(grayImage, grayImage, cv::Size(7, 7));
			
			cv::createTrackbar("CANNY 值：", "window1", &min_Thresh, max_Thresh, Find_Draw_COntours);
			Find_Draw_COntours(0, 0);
			*/
        }

        if(cb_data.saveFrame && !cb_data.leftIR.empty() && !cb_data.rightIR.empty()){
            LOGI(">>>> save frame %d", cb_data.saveIdx);
            char f[32];
#ifndef TEMPLATE_DEBUG
            sprintf(f, "%c-%d-d.img", key_value,cb_data.saveIdx);
            FILE* fp = fopen(f, "wb");
            fwrite(cb_data.depth.data, 2, cb_data.depth.size().area(), fp);
            
            fclose(fp);
            
            sprintf(f, "%c-%d-c.img", key_value,cb_data.saveIdx);
            FILE* fpc = fopen(f, "wb");
            //fwrite(medium_resut.data, 2, cb_data.depth.size().area(), fp);
            fwrite(cb_data.color.data, 3, cb_data.color.size().area(), fp);

            // fwrite(cb_data.leftIR.data, 1, cb_data.leftIR.size().area(), fp);
            // fwrite(cb_data.rightIR.data, 1, cb_data.rightIR.size().area(), fp);
            fclose(fpc);
#endif
#ifdef TEMPLATE_DEBUG
			sprintf(f, "front_depth_img/%d.img", cb_data.saveIdx++);
			FILE* fp = fopen(f, "wb");
			
			cv::Mat temp_Depth;
			temp_Depth = cb_data.depth;
			//cv::Rect rect(70, 10, DEEPIMG_WIDTH, DEEPIMG_HEIGHT);
			//cv::resize(cb_data.depth(rect), temp_Depth, cv::Size(DEEPIMG_WIDTH,DEEPIMG_HEIGHT));
			cv::medianBlur(temp_Depth,temp_Depth,5);
			fwrite(temp_Depth.data, 2, temp_Depth.size().area(), fp);
			
			//fwrite(cb_data.depth.data, 2, cb_data.depth.size().area(), fp);
			
            
            fclose(fp);
#endif
            cb_data.saveFrame = false;
        }
        
        if(m_color_set && !cb_data.color.empty()){
        	//LOGI(">>>> save frame %d", cb_data.saveIdx);
            char f[32];
            sprintf(f, "color_png/%d.png", m_color_count++);
            std::vector<int> compression_params;
            cv::Mat dest_mat,dest_resized_Mat,rect_Mat,pointed_Mat;
            
            //cv::resize(cb_data.color, dest_mat, cv::Size(224,224));
            //dest_mat = cb_data.color;
            color_pic_resize(cb_data.color, &dest_resized_Mat);
            //cv::resize(cb_data.color, dest_resized_Mat, cv::Size(1100,540));
            
            cv::imshow("resized_color", dest_resized_Mat);
            
            cv::resize(dest_resized_Mat, rect_Mat, cv::Size(540,540));
            
            cv::imshow("rect_color", rect_Mat);
            
            cv::resize(rect_Mat, pointed_Mat, cv::Size(100,100));
            
            dest_mat = cb_data.color;

    		compression_params.push_back(cv::IMWRITE_PNG_COMPRESSION);
    		compression_params.push_back(9);
    		cv::imwrite(f, dest_mat, compression_params);
    		
    		m_color_set = false;
        }

        fakeLock = false;

        int key = cv::waitKey(10);
        switch(key & 0xff){
            case 0xff:
                break;
            case 'q':
                exit_main = true;
                break;
            case 's':
                cb_data.saveFrame = true;
                break;
            case 'c':
            	//show_template(cb_data.saveIdx-1);
            	m_display = true;
            	break;
            case 'p':
            	m_color_set = true;
            	break;
            default:
#ifndef TEMPLATE_DEBUG
            	if(key == key_value)
            	{
            		cb_data.saveIdx++;
            	}
            	else
            	{
            		key_value = key;
            		cb_data.saveIdx = 0;
            	}
#endif
                LOGD("Unmapped key %d", key);
        }

#ifdef DEVELOPER_MODE
        DEVELOPER_MODE_PRINT();
#endif
    }

    ASSERT_OK( TYStopCapture(hDevice) );
    ASSERT_OK( TYCloseDevice(hDevice) );
    ASSERT_OK( TYDeinitLib() );
    delete frameBuffer[0];
    delete frameBuffer[1];

    LOGD("=== Main done!");
    return 0;
}
