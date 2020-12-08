
#include <errno.h> 
#include <pthread.h>

#include "visca_api.h"
#include "libvisca.h"
#include "rw_config.h"


#include "../utils_log.h"

VISCAInterface_t iface;
VISCACamera_t camera;


set_auto_icr(VISCAInterface_t *iface, VISCACamera_t *camera, int on)
{
	VISCAPacket_t packet;
	
	_VISCA_init_packet(&packet);
	_VISCA_append_byte(&packet, VISCA_COMMAND);
	_VISCA_append_byte(&packet, 0x04);
	_VISCA_append_byte(&packet, 0x51);

	if(on)
		_VISCA_append_byte(&packet, VISCA_IRRECEIVE_ON);
	else
		_VISCA_append_byte(&packet, VISCA_IRRECEIVE_OFF);
	
	return _VISCA_send_packet_with_reply(iface, camera, &packet);
}


set_icr(VISCAInterface_t *iface, VISCACamera_t *camera, int on)
{
	VISCAPacket_t packet;
	
	_VISCA_init_packet(&packet);
	_VISCA_append_byte(&packet, VISCA_COMMAND);
	_VISCA_append_byte(&packet, 0x04);
	_VISCA_append_byte(&packet, 0x01);

	if(on)
		_VISCA_append_byte(&packet, VISCA_IRRECEIVE_ON);
	else
		_VISCA_append_byte(&packet, VISCA_IRRECEIVE_OFF);
	
	return _VISCA_send_packet_with_reply(iface, camera, &packet);
}


VISCA_API uint32_t
VISCA_set_flip(VISCAInterface_t *iface, VISCACamera_t *camera, int flip)
{
  VISCAPacket_t packet;

  _VISCA_init_packet(&packet);
  _VISCA_append_byte(&packet, VISCA_COMMAND);
  _VISCA_append_byte(&packet, VISCA_CATEGORY_CAMERA1);
  _VISCA_append_byte(&packet, 0x66);

  if(flip)
  	_VISCA_append_byte(&packet, 0x02);
  else
	_VISCA_append_byte(&packet, 0x03);

  return _VISCA_send_packet_with_reply(iface, camera, &packet);
}



/*
VISCA_API uint32_t
VISCA_set_zoom_value(VISCAInterface_t *iface, VISCACamera_t *camera, uint32_t zoom)
{
  VISCAPacket_t packet;

  _VISCA_init_packet(&packet);
  _VISCA_append_byte(&packet, VISCA_COMMAND);
  _VISCA_append_byte(&packet, VISCA_CATEGORY_CAMERA1);
  _VISCA_append_byte(&packet, VISCA_ZOOM_VALUE);
  _VISCA_append_byte(&packet, (zoom & 0xF000) >> 12);
  _VISCA_append_byte(&packet, (zoom & 0x0F00) >>  8);
  _VISCA_append_byte(&packet, (zoom & 0x00F0) >>  4);
  _VISCA_append_byte(&packet, (zoom & 0x000F));

  return _VISCA_send_packet_with_reply(iface, camera, &packet);
}
*/
VISCA_API uint32_t
VISCA_colorsatuation_value(VISCAInterface_t *iface, VISCACamera_t *camera, uint32_t value)
{
  VISCAPacket_t packet;

  _VISCA_init_packet(&packet);
  _VISCA_append_byte(&packet, VISCA_COMMAND);
  _VISCA_append_byte(&packet, VISCA_CATEGORY_PAN_TILTER);
  _VISCA_append_byte(&packet, 0x17);
  _VISCA_append_byte(&packet, VISCA_CATEGORY_INTERFACE );
  _VISCA_append_byte(&packet, VISCA_CATEGORY_INTERFACE );

  _VISCA_append_byte(&packet, (value & 0xF0) >>  4);
  _VISCA_append_byte(&packet, (value & 0x0F));

  return _VISCA_send_packet_with_reply(iface, camera, &packet);
}

VISCA_API uint32_t
VISCA_contract_value(VISCAInterface_t *iface, VISCACamera_t *camera, uint32_t value)
{
  VISCAPacket_t packet;

  _VISCA_init_packet(&packet);
  _VISCA_append_byte(&packet, VISCA_COMMAND);
  _VISCA_append_byte(&packet, VISCA_CATEGORY_PAN_TILTER);
  _VISCA_append_byte(&packet, 0x19);
  _VISCA_append_byte(&packet, VISCA_CATEGORY_INTERFACE );
  _VISCA_append_byte(&packet, VISCA_CATEGORY_INTERFACE );

  _VISCA_append_byte(&packet, (value & 0xF0) >>  4);
  _VISCA_append_byte(&packet, (value & 0x0F));

  return _VISCA_send_packet_with_reply(iface, camera, &packet);
}

//亮度补偿
VISCA_API uint32_t
VISCA_set_ExpComp_value(VISCAInterface_t *iface, VISCACamera_t *camera, uint32_t value)
{
  VISCAPacket_t packet;

  _VISCA_init_packet(&packet);
  _VISCA_append_byte(&packet, VISCA_COMMAND);
  _VISCA_append_byte(&packet, VISCA_CATEGORY_PAN_TILTER);
  _VISCA_append_byte(&packet, 0x1A);
  _VISCA_append_byte(&packet, VISCA_CATEGORY_INTERFACE );
  _VISCA_append_byte(&packet, VISCA_CATEGORY_INTERFACE );

  _VISCA_append_byte(&packet, (value & 0xF0) >>  4);
  _VISCA_append_byte(&packet, (value & 0x0F));

  return _VISCA_send_packet_with_reply(iface, camera, &packet);
}

//光圈
VISCA_API uint32_t
VISCA_set_Aperture_value(VISCAInterface_t *iface, VISCACamera_t *camera, uint32_t value)
{
  VISCAPacket_t packet;

  _VISCA_init_packet(&packet);
  _VISCA_append_byte(&packet, VISCA_COMMAND);
  _VISCA_append_byte(&packet, VISCA_CATEGORY_CAMERA1);
  _VISCA_append_byte(&packet, 0x42);
  _VISCA_append_byte(&packet, VISCA_CATEGORY_INTERFACE );
  _VISCA_append_byte(&packet, VISCA_CATEGORY_INTERFACE );

  _VISCA_append_byte(&packet, (value & 0xF0) >>  4);
  _VISCA_append_byte(&packet, (value & 0x0F));

  return _VISCA_send_packet_with_reply(iface, camera, &packet);
}


/*
void set_Contract(int param){

	VISCA_set_md_adjust_huelevel(&iface, &camera, param);
}
*/
//VISCA_get_md_huelevel(VISCAInterface_t *iface, VISCACamera_t *camera, uint8_t *power);
//VISCA_set_md_adjust_huelevel(VISCAInterface_t *iface, VISCACamera_t *camera, uint8_t power);
//VISCA_set_bright_value(VISCAInterface_t *iface, VISCACamera_t *camera, uint32_t value);
//VISCA_get_bright_value(VISCAInterface_t *iface, VISCACamera_t *camera, uint16_t *value);

int set_ColorHue(int param){

	// UTIL_INFO("set_ColorHue %d\n", param);
	return	VISCA_set_md_adjust_huelevel(&iface, &camera, param);
	printf("VISCA_clear ok\n");
}
//色饱和度 0-100
int set_colorsatuation_value(int param){
	// UTIL_INFO("set_colorsatuation_value %d\n", param);

	return	VISCA_colorsatuation_value(&iface, &camera, param);
}
//对比度0-100
int set_contract_value(int param){//00-100
	// UTIL_INFO("set_contract_value %d\n", param);

	return	VISCA_contract_value(&iface, &camera, param);
}

//0 -19
int set_zoom_value(int param){
	int command = 0;
	switch(param){
	case 0:
		command = 0x0000;
		break;
	case 1:
		command = 0x1584;
		break;
	case 2:
		command = 0x2005;
		break;
	case 3:
		command = 0x266d;
		break;
	case 4:
		command = 0x2af1;
		break;
	case 5:
		command = 0x2e66;
		break;
	case 6:
		command = 0x3135;
		break;
	case 7:
		command = 0x3383;
		break;
	case 8:
		command = 0x3596;
		break;
	case 9:
		command = 0x3756;
		break;
	case 10:
		command = 0x38e7;
		break;
	case 11:
		command = 0x3a3d;
		break;
	case 12:
		command = 0x3b64;
		break;
	case 13:
		command = 0x3c67;
		break;
	case 14:
		command = 0x3d48;
		break;
	case 15:
		command = 0x3e04;
		break;
	case 16:
		command = 0x3e9e;
		break;
	case 17:
		command = 0x3f2b;
		break;
	case 18:
		command = 0x3f8a;
		break;
	case 19:
		command = 0x4000;
		break;
	default:
		command = 0x0000;
			
	}
	printf("param =%d\n",param);
	printf("command =%x\n",command);
	return VISCA_set_zoom_value(&iface, &camera, command);
}


int set_zoom(unsigned short val)
{

	return VISCA_set_zoom_value(&iface, &camera, val);
}


int get_zoom_val()
{
	unsigned short zoom_value=0;
	int ret = VISCA_get_zoom_value(&iface, &camera, &zoom_value);
	
	if(ret==VISCA_SUCCESS)
		return zoom_value;
	else
	{
		printf("get zoom error:%d\n",ret);
		return -1;
	}
}


//亮度补偿
int set_exp_comp_value(int param){
	return VISCA_set_ExpComp_value(&iface, &camera, param);
}
//清晰度 (锐度)0-15
int set_aperture_value(int param){
	// UTIL_INFO("set_aperture_value %d\n", param);

	return VISCA_set_Aperture_value(&iface, &camera, param);
}

int aperture_down(){
	return VISCA_set_aperture_down(&iface, &camera);
}

int set_iris_down(){
	return VISCA_set_iris_down(&iface, &camera);
}

int set_iris_up(){
	return VISCA_set_iris_up(&iface, &camera);
}

int get_iris_value(uint16_t* param){
	return VISCA_get_iris_value(&iface, &camera, &param);
}
//VISCA_set_iris_down(VISCAInterface_t *iface, VISCACamera_t *camera)
//VISCA_set_iris_up(VISCAInterface_t *iface, VISCACamera_t *camera)
//光圈 5-17
int set_iris_value(int param){
}

int set_iris_reset(){
	return VISCA_set_iris_reset(&iface, &camera);
}

int set_focus_near_limit(int param){
	return VISCA_set_focus_near_limit(&iface, &camera,param);
}
//焦聚  远近
int set_focus_far(){

	VISCA_set_focus_Manual(&iface, &camera);
	return VISCA_set_focus_far(&iface, &camera);
}

int set_focus_near(){

	VISCA_set_focus_Manual(&iface, &camera);
	return  VISCA_set_focus_near(&iface, &camera);
}




int set_focus_stop()
{
	VISCA_set_focus_stop(&iface, &camera);
}


int set_zoom_tele()
{
 	return VISCA_set_zoom_tele(&iface, &camera);
}

int set_zoom_tele_speed(speed)		////
{
 	return VISCA_set_zoom_tele_speed(&iface, &camera, speed);
}

int set_zoom_wide()
{
	return VISCA_set_zoom_wide(&iface, &camera);

}


int set_zoom_wide_speed(speed)		////
{
	return VISCA_set_zoom_wide_speed(&iface, &camera, speed);

}


int set_zoom_stop()
{
	return VISCA_set_zoom_stop(&iface, &camera);
}


static char* com_dev;
static pthread_t gs_pid;




void* visca_init_thread(void* param)
{
	int ret;
	while(1)
	{
		//打开串口
		if (VISCA_open_serial(&iface, com_dev)!=VISCA_SUCCESS)
		{
			UTIL_ERR("unable to open serial device %s\n", com_dev);
			sleep(3);
			continue;
		}
		else
			break;
	}
	
	// sleep(5);
	UTIL_INFO("sleep 30 second visca \n");

	while(1)
	{
		int camera_num;
		iface.broadcast=0;
	
		if(VISCA_set_address(&iface, &camera_num)!=VISCA_SUCCESS)
		{
			UTIL_ERR("visca VISCA_set_address fail\n");
			sleep(3);
			continue;
		}
		else
			break;
	}


	while(1)
	{
		camera.address=1;
		
		//获取信息
		ret = VISCA_get_camera_info(&iface, &camera);
		if(ret==VISCA_SUCCESS)
		{
			UTIL_INFO("camera info vendor: 0x%04x\n model: 0x%04x\n ROM version: 0x%04x\n socket number: 0x%02x\n",
					camera.vendor, camera.model, camera.rom_version, camera.socket_num);

			return VISCA_SUCCESS;
		}
		else
			UTIL_ERR("VISCA_get_camera_info fail\n");

		sleep(3);
	}

}



int visca_init(const char* device)
{
	com_dev = device;

	pthread_create(&gs_pid, 0, visca_init_thread, NULL);
	return 0;
}



int visca_deinit()
{
	return VISCA_close_serial(&iface);
}
































