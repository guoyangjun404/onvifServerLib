#ifndef __IR_H__
#define __IR_H__


#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif /* End of #ifdef __cplusplus */


#define IR_IMAGEWIDTH 384
#define IR_IMAGEHEIGHT 292


typedef struct
{
	int userPalette;        //色板  
	int wideDynamic;        //宽动态
	int orgData;            //数据源
	int actime;             //自动校正间隔
} ThermalBaseParam;

typedef struct
{
	float emissivity;           //发射率    
	float distance;             //距离
	float humidity;             //湿度
	float correction;           //修正
	float reflection;           //反射温度
	float amb;                  //环境温度
} ThermalEnvParam;



//获取图像及温度
int getIrImage(unsigned char** ir, float* maxx, float* maxy, float* maxT, float* minx, float* miny, float* minT);


//获取热成像基础参数,成功返回0，失败返回-1
int getThermalBaseParam(ThermalBaseParam *param);

//设置热成像基础参数,成功返回0，失败返回-1
int setThermalBaseParam(ThermalBaseParam *param);


//获取热成像环境参数,成功返回0，失败返回-1
int getThermalEnvParam(ThermalEnvParam *param);

//设置热成像环境参数，成功返回0，失败返回-1
int setThermalEnvParam(ThermalEnvParam *param);

//启动红外线程
int start_ir_thread();
//图像抓拍
int Common_Venc_SnapProcess(char *acFile);



#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* End of #ifdef __cplusplus */

#endif 




