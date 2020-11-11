
#ifndef __SET_CONFIG_H__
#define	__SET_CONFIG_H__

#ifdef __cplusplus
extern "C" {
#endif


typedef struct
{
	int userPalette;               
	int wideDynamic;     
	int orgData;      
    int actime;     
} ThermalParam1_t;

typedef struct
{
	float emissivity;               
	float distance;     
    float humidity;
	float correction;      
    float reflection; 
    float amb;
} ThermalParam2_t;

typedef struct
{
	int focal;                      //焦点
    float lens;                    //长度
    float distance;           //距离

    int dula_model;          //融合模式
    signed short int x;      //x偏移
    signed short int y;      //y偏移
    float scale;                     //缩放
} DulaInformation_t;

typedef  int  BOOL;

/*********************************************
* FuncName: logOpen       
* Describe:  log开, log文件为"ipsee.txt"
* Params  :                                
**********************************************/
void logOpen();

/*********************************************
* FuncName: logClose       
* Describe:  log关, log文件为"ipsee.txt"
* Params  :                                
**********************************************/
void logClose();


/*********************************************
* FuncName: onvifInit       
* Describe:  onvif初始化
* Params  :                                
**********************************************/
void onvifInit(void);

/*********************************************
* FuncName: onvifStart       
* Describe:  开始onvif服务端
* Params  :                                
**********************************************/
void onvifStart(void);

/*********************************************
* FuncName: onvifStop       
* Describe:  开始onvif服务端
* Params  :                                
**********************************************/
void onvifStop(void);

/*********************************************
* FuncName: onvifDeinit       
* Describe:  onvif反初始化
* Params  :                                
**********************************************/
void onvifDeinit(void);



/*********************************************
* FuncName: devInit       
* Describe:  设备初始化
* Params  :                                
**********************************************/
int devInit(char *ptzDevID, const char *cameraDEVID);


/*********************************************
* FuncName: controlPtzLeft       
* Describe:  控制PTZ的转向, 转动哪个方向才有哪个方向的数值，不转动的方向数值为0
* Params  :                                
* [OUT]      
*    Xspeed : 水平转动速度  -为左转，+为右转
*    Yspeed : 垂直转动速度  -为下转，+为上转
*    Zspeed : 水平转动速度  -为缩小焦距，+为放大焦距
* Return  :                                                  
**********************************************/
void controlPtzPos(float Xspeed, float Yspeed, float Zspeed);

/*********************************************
* FuncName: setPtzPreset       
* Describe:  设置PTZ的预置位
* Params  :                                
* Return  :   0：success                                               
**********************************************/
int ptzStop();

/*********************************************
* FuncName: setPtzPreset       
* Describe:  设置PTZ的预置位
* Params  :                                
* [OUT]      
*    speed : 方位 
* Return  :                                                  
**********************************************/
void setPtzPreset(unsigned short location);

/*********************************************
* FuncName: gotoPtzPreset       
* Describe:  转到PTZ的预置位
* Params  :                                
* [OUT]      
*    speed : 方位 
* Return  :                                                  
**********************************************/
void gotoPtzPreset(unsigned short location);


/*********************************************
* FuncName: focusMove       
* Describe:  摄像头调焦
* Params  :                                
* [OUT]      
*    zoom : 缩放速度 
* Return  :                                                  
**********************************************/
void focusMove(float zoom);


/*********************************************
* FuncName: setImgParam       
* Describe:  摄像头参数设置
* Params  :  参数数据  >0 时才有效设置，<= 0 表示没有设置                             
* [OUT]      
*    saturation : 色饱和度 
*    contrast : 对比度
*    brightness : 亮度补偿
*    sharp : 清晰度 (锐度)
* Return  :                                                  
**********************************************/
void setImgParam(float saturation,  float contrast, float brightness,float sharp);


/*********************************************
* FuncName: getThermalParam1       
* Describe:  获取 热成像参数配置_1 , 获取:色板、宽动态、亮度补偿、清晰度 (锐度)
* Params  :                                
* [IN]      
*    userPalette : 色板
*    wideDynamic : 宽动态
*    orgData : 数据源
*    actime : 自动校正间隔
* Return  :                              
**********************************************/
void getThermalParam1(ThermalParam1_t *thermalParam1);

/*********************************************
* FuncName: setThermalParam1       
* Describe:  热成像参数配置设置_1 , 设置:色板、宽动态、亮度补偿、清晰度 (锐度)
* Params  :                                
* [OUT]      
*    userPalette : 色板
*    wideDynamic : 宽动态
*    orgData : 数据源
*    actime : 自动校正间隔
* Return  :                              
**********************************************/
void setThermalParam1(ThermalParam1_t *thermalParam1);

/*********************************************
* FuncName: setThermalParam2       
* Describe:  获取 热成像参数配置设置_1 , 获取:发射率、距离、湿度、修正、反射温度、环境温度
* Params  :                                
* [IN]      
*    emissivity : 发射率
*    distance : 距离
*    humidity : 湿度
*    correction : 修正
*    reflection :反射温度
*    amb : 环境温度
* Return  :                              
**********************************************/
void getThermalParam2(ThermalParam2_t *thermalParam2);

/*********************************************
* FuncName: setThermalParam2       
* Describe:  热成像参数配置设置_1 , 设置:发射率、距离、湿度、修正、反射温度、环境温度
* Params  :                                
* [OUT]      
*    emissivity : 发射率
*    distance : 距离
*    humidity : 湿度
*    correction : 修正
*    reflection :反射温度
*    amb : 环境温度
* Return  :                              
**********************************************/
void setThermalParam2(ThermalParam2_t *thermalParam2);



/*********************************************
* FuncName: setDulaParam        
* Describe:  获取dula参数数据
* Params  :                                
* [IN]      
*   dulaInfo :  获取dulaInfo数据参数
* Return  :                                                  
**********************************************/
void getDulaParam(DulaInformation_t *dulaInfo);

/*********************************************
* FuncName: setDulaParam        
* Describe:  设置dula参数数据
* Params  :                                
* [OUT]      
*   dulaInfo :  设置dulaInfo数据
* Return  :                                                  
**********************************************/
void setDulaParam(DulaInformation_t *dulaInfo);



#ifdef __cplusplus
}
#endif

#endif

