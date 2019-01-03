#include "../common/common.hpp"
#include<stdio.h>
#include<algorithm>
#include"HttpReq.h"
#include<string.h>
#include<iostream>
#include<json/json.h>
#include<math.h>

#include <unistd.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <errno.h>
#include <stdlib.h>

#define BRIGHTNESS_NUM 3
//static std::vector<int> brightness_areas(BRIGHTNESS_NUM);
static int total_brightness_points = 0x00;

static int brightness_areas[BRIGHTNESS_NUM] = { 0x00, 0x00, 0x00 }; 

static char buffer[1024*1024];
static bool fakeLock = false; // NOTE: fakeLock may lock failed

TY_FRAME_DATA m_frame;

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
            
    //copy to m_frame for getting brightness
    m_frame = *frame;

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

void f_Override_Depth(cv::Mat depth,cv::Mat* override_Depth)
{
	int i=0;
	uint16_t* src_depth_data;
	uint16_t dst_data[450*565];
	src_depth_data = (uint16_t *)depth.data;
	for(i=0;i<(450*565);i++)
	{
		if(src_depth_data[i] == 0)
		{
			dst_data[i] = 5000;
		}
		else
		{
			dst_data[i] = src_depth_data[i];
		}
	}
	*override_Depth = cv::Mat(450, 565, CV_16U, dst_data);
}

void f_Trans2D(cv::Mat depth,uint16_t* t_data,uint8_t* r_data,cv::Mat* p_2D_gray)
{
	int i=0,j=0;
	int l_size = 5;
	uint16_t* src_depth_data;
	uint8_t dst_data[450*565];
	int dst_line[565];
	int dis_line[565];
	src_depth_data = (uint16_t *)depth.data;
	
	for(i=0;i<565;i++)
	{
		dst_line[i] = 3000;
		for(j=0;j<430;j++)
		{
			if((t_data[(j*565) + i] - src_depth_data[(j*565) + i]) > 100)
			{
		
				if((src_depth_data[(j*565) + i] < dst_line[i]) && (src_depth_data[(j*565) + i] > 5))
				{
				
					dst_line[i] = src_depth_data[(j*565) + i];
					/*
					if(t_data[(j*565) + i] > 5)
					{
						dst_line[i] = t_data[(j*565) + i];
					}
					*/
				}
			}
		}
		dis_line[i] = 450 - (int)((dst_line[i]/3000.0)*450);
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
		if(r_data[i] >= 150)
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

#if 0
void get_position(char* url,double* x,double* y,double* z,double* qx,double* qy,double* qz,double* qw)
{
	HttpRequest* Http;
    char http_return[1024] = {0};
    char http_msg[1024] = {0};
	char* str1;
	char* str2;
	int len;
	std::string	dst_str,dst_sub_str;
	Json::Reader  reader,subreader;
	Json::Value   value,subvalue;

    strcpy(http_msg, url);

    if(Http->HttpGet(http_msg, http_return)){
        //std::cout << http_return << std::endl;
		str1 = strstr(http_return,"{");
		str2 = strrchr(str1,'}');
		len = str2 - str1 + 1;
		std::string str_str1(str1);
		dst_str = str_str1.substr(0, len);
		//std::cout << dst_str << std::endl;
		
		if(reader.parse(dst_str,value))
		{
			if(!value["type"].isNull())
			{
				//std::cout<<value["type"].asString()<<std::endl;
				//std::cout<<value["result"].asString()<<std::endl;
			}
			if(!value["result"].isNull())
			{
				dst_sub_str = value["result"].asString();
				//std::cout<<dst_sub_str<<std::endl;

				subreader.parse(dst_sub_str,subvalue);
				
				if(!subvalue["position"].isNull())
				{
					//std::cout<<subvalue["position"]["x"].asDouble()<<std::endl;
					//std::cout<<subvalue["position"]["y"].asDouble()<<std::endl;
					//std::cout<<subvalue["position"]["z"].asDouble()<<std::endl;
					*x = subvalue["position"]["x"].asDouble();
					*y = subvalue["position"]["y"].asDouble();
					*z = subvalue["position"]["z"].asDouble();
				}
				if(!subvalue["orientation"].isNull())
				{
					//std::cout<<subvalue["orientation"]["x"].asDouble()<<std::endl;
					//std::cout<<subvalue["orientation"]["y"].asDouble()<<std::endl;
					//std::cout<<subvalue["orientation"]["z"].asDouble()<<std::endl;
					//std::cout<<subvalue["orientation"]["w"].asDouble()<<std::endl;
					*qx = subvalue["orientation"]["x"].asDouble();
					*qy = subvalue["orientation"]["y"].asDouble();
					*qz = subvalue["orientation"]["z"].asDouble();
					*qw = subvalue["orientation"]["w"].asDouble();
				}
			}
		}
    }
}

void post_data(char* url,std::string body_data)
{
	HttpRequest* Http;
    char http_return[1024] = {0};
    char http_url[1024] = {0};
	char http_data[1024] = {0};

	strcpy(http_url, url);
	strcpy(http_data, body_data.data());

	if(Http->HttpPost(http_url, http_data, http_return)){
        //std::cout << http_return << std::endl;
    }
}

void post_other_sensor_data(uint8_t* line_data)
{
	Json::Value item;
	Json::Value arrayObj;

    char get_url[100] = "http://192.168.31.200:8080/getCurrentSavePose";
	char post_url[100] = "http://192.168.31.200:8080/otherSensorData";
	double x,y,z,qx,qy,qz,qw;
	double angle;
	float rx[11] = {1.5,1.5,1.5,1.5,1.5,1.5,1.5,1.5,1.5,1.5,1.5};
	float ry[11] = {0,0.05,0.1,0.15,0.2,0.25,0.3,0.35,0.4,0.45,0.5};
	float px[11],py[11],pz[11];
	int i=0;
	/*
	for(i=0;i<51;i++)
	{
		rx[i] = line_data[i] * cos((3.14159*(25-i))/180);
		ry[i] = line_data[i] * sin((3.14159*(25-i))/180);
	}
	*/
	std::string out_str;
	get_position(get_url,&x,&y,&z,&qx,&qy,&qz,&qw);
	//std::cout<<x<<std::endl;
	//std::cout<<y<<std::endl;
	//std::cout<<z<<std::endl;

	//std::cout<<qx<<std::endl;
	//std::cout<<qy<<std::endl;
	//std::cout<<qz<<std::endl;
	//std::cout<<qw<<std::endl;

	angle = atan2(2.0 * (qw * qz + qx * qy), 1.0 - 2.0 * (qy * qy + qz * qz));
	
	for(i=0;i < 11; i++)
	{
		px[i] = x - ry[i] * sin(angle) + rx[i] * cos(angle);
		py[i] = y + rx[i] * sin(angle) + ry[i] * cos(angle);
		pz[i] = 0.38;
		item["x"] = px[i];
		item["y"] = py[i];
		item["z"] = pz[i];
		arrayObj.append(item);
	}

	out_str = arrayObj.toStyledString();

	//std::cout<<out_str<<std::endl;

	out_str = "msg=" + out_str;

	//post_data(post_url,out_str);
	
}
#endif

void tcp_client_init(int* socket_fd, struct sockaddr_in* server_addr)
{
	if( (*socket_fd = socket(AF_INET,SOCK_STREAM,0)) < 0 ) {
            printf("create socket error: %s(errno:%d)\n)",strerror(errno),errno);
            exit(0);
    }
 
    memset(server_addr,0,sizeof(*server_addr));
    server_addr->sin_family = AF_INET;
    server_addr->sin_port = htons(atoi("9527"));
 
    if( inet_pton(AF_INET,"127.0.0.1",&(server_addr->sin_addr)) <=0 ) {
            printf("inet_pton error for %s\n","127.0.0.1");
            exit(0);
    }
 
    if( connect(*socket_fd,(struct sockaddr*)server_addr,sizeof(*server_addr))<0) {
            printf("connect error: %s(errno: %d)\n",strerror(errno),errno);
            exit(0);
    }
    
    LOGD("---------------connect server successfully!!!");
}

void tcp_client_send(int* socket_fd,char* message)
{
	//char message[50] = "abcdefg";
	if(send(*socket_fd,message,strlen(message),0) < 0)
	{
		LOGD("send message error\n");
		exit(0);
	}
}

int main(int argc, char* argv[])
{
    const char* IP = NULL;
    const char* ID = NULL;
    TY_DEV_HANDLE hDevice;
    
    cv::Mat medium_resut1,medium_resut2,resized_depth,override_depth,p_2D_gray;
    
    FILE *m_fp;
    uint16_t t_data[450*565];
    uint8_t  r_data[51];
    int f_count = 0;
    
    uint8_t bright_gain = 48;
	uint8_t ori_gain = bright_gain;
    
    //paramaters of socket
    int socket_fd;
    char message[512];
    struct sockaddr_in server_addr;
    

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
    
    LOGD("=== Init lib");
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
    
    // set cam_gain and laser_power
    ASSERT_OK(TYSetInt(hDevice, TY_COMPONENT_IR_CAM_LEFT, TY_INT_GAIN, 250));  //16/32/64/128/254  3~254
	ASSERT_OK(TYSetInt(hDevice, TY_COMPONENT_IR_CAM_RIGHT, TY_INT_GAIN, 250)); //16/32/64/128/254
	ASSERT_OK(TYSetInt(hDevice, TY_COMPONENT_LASER, TY_INT_LASER_POWER,100)); // 1~100
	
	//get the histro 
	ASSERT_OK(TYEnableComponents(hDevice, TY_COMPONENT_BRIGHT_HISTO));

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
    ASSERT_OK( TYSetBool(hDevice, TY_COMPONENT_DEVICE, TY_BOOL_TRIGGER_MODE, false) );

    LOGD("=== Start capture");
    ASSERT_OK( TYStartCapture(hDevice) );

    LOGD("=== Wait for callback");
    bool exit_main = false;
    DepthViewer depthViewer;
    
    /* 打开文件用于读写 */
	m_fp = fopen("template.img", "r+");
	/* 查找文件的开头 */
   	fseek(m_fp, SEEK_SET, 0);
   	/* 读取并显示数据 */
	fread(t_data, 2, 450*565, m_fp);
	
	fclose(m_fp);
	
	tcp_client_init(&socket_fd, &server_addr);
	
    while(!exit_main){
        while(fakeLock){
            MSLEEP(10);
        }
        fakeLock = true;
        /*
        for (int i = 0; i < m_frame.validCount; i++) 
    	{
			if (m_frame.image[i].componentID == TY_COMPONENT_BRIGHT_HISTO) 
			{
				int32_t *ir_left_his, *ir_right_his;
				ir_left_his = (int32_t *)m_frame.image[i].buffer;
				ir_right_his = (int32_t *)m_frame.image[i].buffer + 256;
				int i;
				for ( i = 0; i<256; i++) 
				{
					if (i < 25)
						brightness_areas[0] += ir_left_his[i];
					else if (i < 253)
						brightness_areas[1] += ir_left_his[i];
					else
						brightness_areas[2] += ir_left_his[i];
					//LOGI("ir_left_his[%d] = %u, ir_right_his[%d] = %u", i, ir_left_his[i], i, ir_right_his[i]);
					//LOGI("ir_left_his[%d] = %u", i, ir_left_his[i]);
				}
			}

			total_brightness_points = (brightness_areas[0] + brightness_areas[1] + brightness_areas[2]);
			brightness_areas[0] = 100 * (float)brightness_areas[0] / (float)total_brightness_points;
			brightness_areas[1] = 100 * (float)brightness_areas[1] / (float)total_brightness_points;
			brightness_areas[2] = 100 * (float)brightness_areas[2] / (float)total_brightness_points;

			printf("area[0] = %u, area[1] = %u, area[2] = %u\n", brightness_areas[0], brightness_areas[1], brightness_areas[2]);
		}
		*/

        if(!cb_data.depth.empty()){
            //depthViewer.show("depth", cb_data.depth);
            cv::medianBlur(cb_data.depth,medium_resut1,5);
            //depthViewer.show("medianBlur", medium_resut1);
            cv::Rect rect(70, 10, 565, 450);
            cv::resize(medium_resut1(rect), resized_depth, cv::Size(565,450));
            f_Override_Depth(resized_depth,&override_depth);
            //depthViewer.show("override Depth", override_depth);
            cv::GaussianBlur(override_depth, medium_resut2, cv::Size(11, 11), 0);
            //depthViewer.show("GaussianBlur Depth", override_depth);
            
            f_Trans2D(medium_resut2,t_data,r_data,&p_2D_gray);
            
            f_count++;
            
            if(f_count > 1)
            {
            	//post_other_sensor_data(r_data);
            	tcp_client_send(&socket_fd, (char*)r_data);
            	f_count = 0;
            }
            
            //cv::imshow("2D_gray", p_2D_gray);
        }
        if(!cb_data.leftIR.empty()){
            //cv::imshow("LeftIR", cb_data.leftIR);
        }
        if(!cb_data.rightIR.empty()){
            //cv::imshow("RightIR", cb_data.rightIR);
        }
        if(!cb_data.color.empty()){
            //cv::imshow("color", cb_data.color);
        }

        if(cb_data.saveFrame && !cb_data.depth.empty() && !cb_data.leftIR.empty() && !cb_data.rightIR.empty()){
            LOGI(">>>> save frame %d", cb_data.saveIdx);
            char f[32];
            sprintf(f, "%d.img", cb_data.saveIdx++);
            FILE* fp = fopen(f, "wb");
            fwrite(cb_data.depth.data, 2, cb_data.depth.size().area(), fp);
            fwrite(cb_data.color.data, 3, cb_data.color.size().area(), fp);

            // fwrite(cb_data.leftIR.data, 1, cb_data.leftIR.size().area(), fp);
            // fwrite(cb_data.rightIR.data, 1, cb_data.rightIR.size().area(), fp);
            fclose(fp);

            cb_data.saveFrame = false;
        }

        fakeLock = false;
        /*
        if((brightness_areas[1] + brightness_areas[2]) > 80)
		{
			bright_gain = bright_gain - 10;
			if(bright_gain <= 16)
			{
				bright_gain = 16;
			}
			//ASSERT_OK(TYSetInt(hDevice, TY_COMPONENT_IR_CAM_LEFT, TY_INT_GAIN, bright_gain));  //16/32/64/128/254  3~254
			//ASSERT_OK(TYSetInt(hDevice, TY_COMPONENT_IR_CAM_RIGHT, TY_INT_GAIN, bright_gain)); //16/32/64/128/254
			//ASSERT_OK(TYSetInt(hDevice, TY_COMPONENT_LASER, TY_INT_LASER_POWER,100)); // 1~100
		}
		if((brightness_areas[1] + brightness_areas[2]) < 20)
		{
			bright_gain = bright_gain + 10;
			if(bright_gain >=245)
			{
				bright_gain = 245;
			}
			//ASSERT_OK(TYSetInt(hDevice, TY_COMPONENT_IR_CAM_LEFT, TY_INT_GAIN, bright_gain));  //16/32/64/128/254  3~254
			//ASSERT_OK(TYSetInt(hDevice, TY_COMPONENT_IR_CAM_RIGHT, TY_INT_GAIN, bright_gain)); //16/32/64/128/254
			//ASSERT_OK(TYSetInt(hDevice, TY_COMPONENT_LASER, TY_INT_LASER_POWER,100)); // 1~100
		}
		
		if(bright_gain != ori_gain)
		{
			ASSERT_OK(TYSetInt(hDevice, TY_COMPONENT_IR_CAM_LEFT, TY_INT_GAIN, bright_gain));  //16/32/64/128/254  3~254
			ASSERT_OK(TYSetInt(hDevice, TY_COMPONENT_IR_CAM_RIGHT, TY_INT_GAIN, bright_gain)); //16/32/64/128/254
			ASSERT_OK(TYSetInt(hDevice, TY_COMPONENT_LASER, TY_INT_LASER_POWER,100)); // 1~100
			
			ori_gain = bright_gain;
		}
		printf("----bright gain is:%d\n",bright_gain);
		*/
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
            default:
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
