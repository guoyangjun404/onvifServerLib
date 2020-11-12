
#include <stdio.h>
#include "set_config.h"
#include "visca_api.h"
#include "ptz.h" 


int onvif_get_devinfo(CONFIG_Information * p_devInfo)
{
	strncpy(p_devInfo->manufacturer,"Huaxiaxin",sizeof(p_devInfo->manufacturer)-1);
	strncpy(p_devInfo->model,"IPCamera",sizeof(p_devInfo->model)-1);
    strncpy(p_devInfo->firmware_version,"2.4",sizeof(p_devInfo->firmware_version)-1);
	strncpy(p_devInfo->serial_number,"123456",sizeof(p_devInfo->serial_number)-1);
	strncpy(p_devInfo->hardware_id,"1.0",sizeof(p_devInfo->hardware_id )-1);

	if (p_devInfo->manufacturer[0] != '\0' &&
		p_devInfo->model[0] != '\0' &&
		p_devInfo->firmware_version[0] != '\0' &&
		p_devInfo->serial_number[0] != '\0' &&
		p_devInfo->hardware_id[0] != '\0')
	{
		return 0;
	}
}



int devInit(char *ptzDevID, const char *cameraDEVID)
{
 //visca 设备初始化 
 // visca_init(cameraDEVID);		

   int ret =RET_ERR;
   ret=pelco_Init(ptzDevID,9600);

    return ret;
}


void controlPtzPos(float Xspeed, float Yspeed, float Zspeed)
{
	printf("cset_config | ontrolPtzPos :  x=%f , y = %f , z = %f \n",Xspeed, Yspeed, Zspeed);

	if (Xspeed > 0 && Yspeed == 0){					//右
		pelco_Right(0x20);
	} else if (Xspeed < 0 && Yspeed == 0){		//左
		pelco_Left(0x20);
	} else if (Yspeed > 0 && Xspeed == 0){		//上
		pelco_Up(0x20);
	} else if(Yspeed < 0 && Xspeed == 0){	    //下
		pelco_Down(0x20);
	}
}

int ptzStop()
{
	int ret;
	ret=pelco_Stop();
        if(ret==0){
			printf("set_config | send ptz Stop ok\r\n");
		}
        else {
			printf("set_config | send ptz  Stop error !!!\r\n");
		}
	return ret;
}

void setPtzPreset(unsigned short location)
{
	printf("set_config | pelco_set_point = %d\n",location);
	pelco_set_point(location);
}
void gotoPtzPreset(unsigned short location)
{
	printf("set_config | pelco_get_point = %d\n",location);
	pelco_get_point(location);
}


void focusMove(float zoom)
{
	printf("set_config |  foucsMov =%f\n",zoom);
}


void setImgParam(float saturation,  float contrast, float brightness,float sharp)
{
	printf("set_config | Brightness 亮度= %0.2f ,ColorSaturation 饱和度 = %0.2f , Contrast 对比度 = %0.2f , Sharpness 锐度= %0.2f\n", brightness, saturation, contrast, sharp);
}


/* 获取 热成像参数配置_1*/
void getThermalParam1(ThermalParam1_t *thermalParam1)
{
	thermalParam1->userPalette = 1;
	thermalParam1->wideDynamic = 1;
	thermalParam1->orgData = 1;
	thermalParam1->actime = 300;
}

/*  设置 热成像参数配置_1 */
void setThermalParam1(ThermalParam1_t *thermalParam1)
{
	printf("set_config | 色板:%d 宽动态:%d  数据源:%d 自动校正间隔:%d\n", thermalParam1->userPalette, thermalParam1->wideDynamic, thermalParam1->orgData, thermalParam1->actime);
}


/* 获取 热成像参数配置_2*/
void getThermalParam2(ThermalParam2_t *thermalParam2)
{
	thermalParam2->emissivity = 0.98;
	thermalParam2->distance = 3;
	thermalParam2->humidity = 0.45;
	thermalParam2->correction = 0;
	thermalParam2->reflection = 26;
	thermalParam2->amb  =26;
}

/* 设置 热成像参数配置_2*/
void setThermalParam2(ThermalParam2_t *thermalParam2)
{
	printf("set_config | 发射率:%0.2f 距离:%0.2f 湿度:%0.2f 修正:%0.2f 反射温度:%0.2f  环境温度:%0.2f\n",
				 thermalParam2->emissivity, thermalParam2->distance, thermalParam2->humidity, thermalParam2->correction, thermalParam2->reflection, thermalParam2->amb);
}

/* 获取 dula数据参数 */
void getDulaParam(DulaInformation_t *dulaInfo)
{
	dulaInfo->focal = 1;
	dulaInfo->lens = 2;
	dulaInfo->distance = 3;
	dulaInfo->dula_model = 4;
	dulaInfo->x = 5;
	dulaInfo->y = 6;
	dulaInfo->scale = 7;
}

/* 设置 dula数据参数 */
void setDulaParam(DulaInformation_t *dalaInfo)
{
	printf("set_config | focal:%d, lens:%0.2f, distance:%0.2f, dula_model:%d, x:%d, y:%d, scale:%0.2f\n", dalaInfo->focal, dalaInfo->lens,
				dalaInfo->distance, dalaInfo->dula_model, dalaInfo->x, dalaInfo->y, dalaInfo->scale);
}
