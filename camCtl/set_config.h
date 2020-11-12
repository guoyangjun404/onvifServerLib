
#ifndef __SET_CONFIG_H__
#define	__SET_CONFIG_H__


#ifdef __cplusplus
extern "C" {
#endif

/////
typedef  int    BOOL;
typedef unsigned int               u32_t;
typedef unsigned char           u8_t;
typedef unsigned char*         u8ptr_t;
typedef float                               float32_t;
typedef unsigned short int   u16_t;

#define    ONVIF_URI_LEN    300

#define  g711   "G711"
#define  g726   "G762"
#define  aac      "AAC"

typedef struct 
{
    
} Audio_Source;


typedef struct 
{ 
    u32_t    session_timeout;            // The rtsp session timeout for the related audio stream
    u32_t    sample_rate;                // The output sample rate in kHz
    u32_t    bitrate;                    // The output bitrate in kbps
    u8_t     a_encoding[64];             //  G711 G726 AAC
                                                // Audio codec used for encoding the audio input (either G711, G726 or AAC)
} Audio_Encoder;

typedef struct 
{
    u32_t    width;
    u32_t    height;
} Video_Source;

#define  H264_PROFILE_MAIN      "Main"  
#define  H264_PROFILE_HIGH      "High"
#define  H264_PROFILE_EXTENDED  "Extended"  
#define  H265_PROFILE_MAIN      "Main"
#define  H264_PROFILE_MAIN10    "Main10"
typedef struct 
{
    u32_t    gov_length;                  // Group of Video frames length. Determines typically the interval in which the
                                                 // I-Frames will be coded. An entry of 1 indicates I-Frames are continuously
                                                 // generated. An entry of 2 indicates that every 2nd image is an I-Frame, and 3 only
                                                 // every 3rd frame, etc. The frames in between are coded as P or B Frames
    u8_t     encode_profile[64];          // H26x_PROFILE_xxx
                                                 // If H.264 profile, either Baseline, Main, Extended or High
                                                 // if H.265 profile, either Main or Main10
                                                 // if Mpeg4 profile, either simple profile (SP) or advanced simple profile (ASP)
} Encoding_profile;

#define  VIEDO_ENCODE_JPEG   "JPEG" 
#define  VIEDO_ENCODE_MPEG4  "MPEG4"
#define  VIEDO_ENCODE_H264   "H264"
#define  VIEDO_ENCODE_H265   "H265"
typedef struct 
{
    u8ptr_t       v_encoding;              // VIEDO_ENCODE_XXX
    Encoding_profile    v_encoding_profile; 
} Video_encode;

typedef struct 
{
    u32_t        width;
    u32_t        height;
    float32_t    quality;                   // Relative value for the video quantizers and the quality of the video
    u32_t        session_timeout;           // The rtsp session timeout for the related video stream
    float32_t    framerate;                 // Desired frame rate in fps , Maximum output framerate in fps
    u32_t        encoding_interval;         // Interval at which images are encoded and transmitted,(A value of 1 means
                                                  // that every frame is encoded, a value of 2 means that every 2nd frame is encoded ...)
    u32_t        bitrate_limit;             // The maximum output bitrate in kbps
    Video_encode       video_encoding;                 
} Video_Encoder;
  

////
typedef struct 
{
    u8_t     server_ip[128];
    u16_t    server_port;
    u32_t    http_max_users;                 // max http connection clients
    u32_t    https_enable;                   // Indicates whether enable https connection, 0 is disable, 1 enable 
    u32_t    need_auth;                      // Indicates whether authentication is required, 0 don’t require, 1 require.
} CONFIG_Server;                    

typedef struct 
{
    u8_t    manufacturer[64];                 // The manufactor of the device
    u8_t    model[64];                                // The device model
    u8_t    firmware_version[64];         // The firmware version in the device
    u8_t    serial_number[64];               // The serial number of the device
    u8_t    hardware_id[64];                   // The hardware ID of the device
} CONFIG_Information;

#define  USER_LEVEL_ADMINISTRATOR  "Administrator" 
#define  USER_LEVEL_OPERATOR       "Operator"
#define  USER_LEVEL_USER           "User"
#define  USER_LEVEL_ANONYMOUS      "Anonymous"
#define  USER_LEVEL_EXTENDED       "Extended" 
typedef struct 
{
    u8ptr_t    user_name;
    u8ptr_t    password;                      // optional
    u8ptr_t    user_level;                    // USER_LEVEL_XXX          
} CONFIG_Users;

typedef struct 
{
    Video_Source      video_source;                 // If the media profile contains a video, the video source configuration
    Video_Encoder     video_encoder;                // If the media profile contains a video, the video encoder configuration
    Audio_Source      audio_source;                 // If the media profile contains a audio, the audio source configuration
    Audio_Encoder     audio_encoder;                // If the media profile contains a audio, the audio encoder configuration
} CONFIG_Profiles;

typedef struct 
{
    u8_t  scope_item[128];
} CONFIG_Scopes;   
/////


typedef struct
{
	float saturation;         //色饱和度       
	float contrast;             //对比度
	float brightness;         //亮度补偿 
    float sharp;                   //清晰度 (锐度)
} ImgParam_t;

typedef struct
{
	int userPalette;             //色板  
	int wideDynamic;        //宽动态
	int orgData;                    //数据源
    int actime;                      //自动校正间隔
} ThermalParam1_t;

typedef struct
{
	float emissivity;           //发射率    
	float distance;               //距离
    float humidity;             //湿度
	float correction;           //修正
    float reflection;            //反射温度
    float amb;                      //环境温度
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
* FuncName: onvif_get_devinfo       
* Describe:  获取设备信息
* Params  :                                
* [IN]      
*    speed : 方位 
* Return  :                                                  
**********************************************/
int onvif_get_devinfo(CONFIG_Information * p_devInfo);



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
* FuncName: getImgParam       
* Describe:  获取 摄像头参数
* Params  :  参数数据  >0 时才有效设置，<= 0 表示没有设置                             
* [IN]      
* Return  :                                                  
**********************************************/
void getImgParam(ImgParam_t *imgParams);

/*********************************************
* FuncName: setImgParam       
* Describe:  设置 摄像头参数
* Params  :  参数数据  >0 时才有效设置，<= 0 表示没有设置                             
* [OUT]      
* Return  :                                                  
**********************************************/
void setImgParam(ImgParam_t *imgParams);


/*********************************************
* FuncName: getThermalParam1       
* Describe:  获取 热成像参数配置_1 , 获取:色板、宽动态、亮度补偿、清晰度 (锐度)
* Params  :                                
* [IN]      
* Return  :                              
**********************************************/
void getThermalParam1(ThermalParam1_t *thermalParam1);

/*********************************************
* FuncName: setThermalParam1       
* Describe:  热成像参数配置设置_1 , 设置:色板、宽动态、亮度补偿、清晰度 (锐度)
* Params  :                                
* [OUT]      
* Return  :                              
**********************************************/
void setThermalParam1(ThermalParam1_t *thermalParam1);

/*********************************************
* FuncName: getThermalParam2       
* Describe:  获取 热成像参数配置设置_1 , 获取:发射率、距离、湿度、修正、反射温度、环境温度
* Params  :                                
* [IN]      
* Return  :                              
**********************************************/
void getThermalParam2(ThermalParam2_t *thermalParam2);

/*********************************************
* FuncName: setThermalParam2       
* Describe:  热成像参数配置设置_1 , 设置:发射率、距离、湿度、修正、反射温度、环境温度
* Params  :                                
* [OUT]      
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

