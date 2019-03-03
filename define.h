#ifndef DEFINE_H
#define DEFINE_H

#define  	APP_PATH		"."
#define		DEEPIMG_WIDTH	640
#define		DEEPIMG_HEIGHT	480
#define		DEEPIMG_DRAWPOINT	56
#define		DRAWPOINT_START		8
#define 	DEEP_DISTANSLINE   	1
#define 	MAX_DEVNUM	5
#define BUFF_SIZE  1024*1024


typedef struct _CAMMER_CONF_S			// size = 64
{
//安装参数
	float		EditHor_View;	//50du
	float		EditVer_View;	//垂直视场：45度（自动计算
	int 		PixWidth_Start; //裁剪起止范围
	int 		PixWidth_End;
	int 		PixHight_Start;
	int 		PixHight_End;
	float		EditHor_Angl;	//安装角度
	float		EditVer_Angl;
	int 		Edit_instalHeight;	//安装高度
//滤波参数
	int 		GussBlurSize;	//高斯滤波
	int 		MidBlurSize;
	int 		MorphOpenSize;	//开操作大小
	int 		MorphCloseSize;
	float		calib_horizon;	//几何校准 水平角度
	float		calib_vertical;
	int 		threshold_val;	//二值化阀值
	float		Temp_threshold; //模板转换阀值
//增益参数
	int 		Gain_thold_min; //增益范围
	int 		Gain_thold_max;
	int 		Gain_Fre;		//调整频次
	int 		Gain_min;
	int 		Gain_max;
}CAMMER_CONF_S;

typedef struct _CAMMER_PARA_S			// size = 64
{
	CAMMER_CONF_S 	stPerCammer[MAX_DEVNUM];
//配置参数
	int			CurrentCam;		//配置相机
	int			IsApplyCam01;
	int			IsApplyCam02;
	int			IsApplyCam03;
	int			IsApplyCam04;
	int			IsApplyCam05;
//ID
    char                id1[32];
	char				id2[32];
	char                id3[32];
	char                id4[32];
	char                id5[32];	

}CAMMER_PARA_S;


#endif // DEFINE_H
