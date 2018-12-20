#ifndef DEFINE_H
#define DEFINE_H

#define  APP_PATH		"/user"

typedef struct _CAMMER_PARA_S			// size = 64
{
//安装参数
	float	 	EditHor_View;	//50du
	float 		EditVer_View;	//垂直视场：45度（自动计算
	int 		PixWidth_Start;
	int 		PixWidth_End;
	int 		PixHight_Start;
	int 		PixHight_End;
	float		EditHor_Angl;
	float		EditVer_Angl;
//滤波参数
	int 		GussBlurSize;
	int			MidBlurSize;
	float		calib_horizon;
	float		calib_vertical;
	int			threshold_val;
//增益参数
	int 		Gain_thold_min;
	int			Gain_thold_max;
	int			Gain_Fre;
	int			Gain_min;
	int			Gain_max;
	

}CAMMER_PARA_S;


#endif // DEFINE_H
