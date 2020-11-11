
#include <errno.h> 
#include <pthread.h>

#include "visca_api.h"
#include "libvisca.h"
#include "rw_config.h"


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


//void* visca_thread(void* param);


static pthread_t gs_pid;
/*
int start_visca_thread()
{
    return pthread_create(&gs_pid, 0, visca_thread, NULL);
}

*/
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

	printf("VISCA_clear ok\n");
	return	VISCA_set_md_adjust_huelevel(&iface, &camera, param);
	printf("VISCA_clear ok\n");
}
//色饱和度 0-100
int set_colorsatuation_value(int param){

	return	VISCA_colorsatuation_value(&iface, &camera, param);
}
//对比度0-100
int set_contract_value(int param){//00-100

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
//亮度补偿
int set_exp_comp_value(int param){
	return VISCA_set_ExpComp_value(&iface, &camera, param);
}
//清晰度 (锐度)0-15
int set_aperture_value(int param){
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


int set_zoom_wide()
{
	return VISCA_set_zoom_wide(&iface, &camera);

}

int set_zoom_stop()
{
	return VISCA_set_zoom_stop(&iface, &camera);
}



visca_init(const char* device)
{
	//打开串口
	if (VISCA_open_serial(&iface, VISCA_COM_ID)!=VISCA_SUCCESS)
	{
		printf("unable to open serial device %s\n", VISCA_COM_ID);
		exit(1);
	}

	int camera_num;

	iface.broadcast=0;
	VISCA_set_address(&iface, &camera_num);

	camera.address=1;

	//获取信息
	VISCA_get_camera_info(&iface, &camera);
	printf("Some camera info:\n------------------\n");
	printf("vendor: 0x%04x\n model: 0x%04x\n ROM version: 0x%04x\n socket number: 0x%02x\n",
					camera.vendor, camera.model, camera.rom_version, camera.socket_num);
}



//VISCA_set_focus_far(VISCAInterface_t *iface, VISCACamera_t *camera)
//VISCA_set_focus_near(VISCAInterface_t *iface, VISCACamera_t *camera)
int main_test()
{

	//VISCAInterface_t iface;
	//VISCACamera_t camera;


	int camera_num;
	uint8_t value;
	uint16_t zoom;


	uint16_t zoom_value=0;


	//打开串口
	if (VISCA_open_serial(&iface, VISCA_COM_ID)!=VISCA_SUCCESS)
	{
		printf("unable to open serial device\n");
		exit(1);
	}



	printf("VISCA_set_address....\n");


	iface.broadcast=0;
	VISCA_set_address(&iface, &camera_num);




	camera.address=1;
	//VISCA_clear(&iface, &camera);
   //printf("VISCA_clear ok\n");



	//获取信息
	VISCA_get_camera_info(&iface, &camera);
	printf("Some camera info:\n------------------\n");
	printf("vendor: 0x%04x\n model: 0x%04x\n ROM version: 0x%04x\n socket number: 0x%02x\n",
		camera.vendor, camera.model, camera.rom_version, camera.socket_num);

	VISCA_usleep(500000);

	//set_auto_icr(&iface, &camera,  0);
	//VISCA_usleep(1000000);


	//初始化数据  read config.ini
	
	int colorsatuation_value = GetIniKeyInt("COLORSATUATION", "value", "config.ini");
	int contract_value = GetIniKeyInt("CONTRACT", "value", "config.ini");
	int zoom_parm = GetIniKeyInt("ZOOM", "value", "config.ini");
	int exp_comp_value = 50;
	int aperture_value = 0;
	int iris_value = 5;
	uint16_t get_value = 2;
	uint16_t *p = &get_value;
	int focus_value = 0x6000;
	//set_focus_near_limit(focus_value);
	//设置光圈手动模式
	//VISCA_set_spot_ae_off(&iface, &camera);
	//设置手动聚焦
	VISCA_set_focus_Manual(&iface, &camera);
	int speed = 0;
	while(1)
	{
		
		uint16_t value;
		int ret=0;
/*		
		VISCA_set_focus_near_speed(&iface, &camera, speed);
		VISCA_usleep(3000000);
		speed++;
		if(speed > 7)
			speed = 0;
*/
		
/*		
		int colorHue_value = 50;
		printf("colorHue_value=%x\n",colorHue_value);
		ret = set_ColorHue(colorHue_value);
		
		if(ret!=VISCA_SUCCESS)
			printf("set_ColorHue error:%d\n",ret);
		else 
			printf("colorHue_value=%x\n",colorHue_value);
			
		VISCA_usleep(3000000);
*/		
#if 0		
		ret = set_colorsatuation_value(colorsatuation_value);
		if(ret!=VISCA_SUCCESS)
			printf("set_colorsatuation error:%d\n",ret);
		else
			printf("colorsatuation_value=%x\n",colorsatuation_value);
			
		VISCA_usleep(3000000);
		colorsatuation_value+=10;
		if(colorsatuation_value > 100) 
			colorsatuation_value = 0;
		
		
		ret = set_contract_value(contract_value);
		if(ret!=VISCA_SUCCESS)
			printf("set_contract_value error:%d\n",ret);
		else
			printf("contract_value=%x\n",contract_value);
		PutIniKeyInt("CONTRACT", "value", contract_value, "config.ini");	
		VISCA_usleep(3000000);
		contract_value+=10;
		if(contract_value > 100) 
			contract_value = 0;
		
			
	
		ret = set_zoom_value(zoom_parm);
		if(ret!=VISCA_SUCCESS)
			printf("set_colorsatuation error:%d\n",ret);
		else
			printf("zoom_parm =%x\n",zoom_parm);
			
		VISCA_usleep(3000000);
		zoom_parm++;
		if(zoom_parm > 19) 
			zoom_parm = 0;
	

		ret = set_exp_comp_value(exp_comp_value);
		if(ret!=VISCA_SUCCESS)
			printf("set_exp_comp_value error:%d\n",ret);
		else
			printf("exp_comp_value=%x\n",exp_comp_value);
			
		VISCA_usleep(3000000);
		exp_comp_value+=10;
		if(exp_comp_value > 100) 
			exp_comp_value = 0;
#endif	
/*	
		ret = set_aperture_value(aperture_value);
		if(ret!=VISCA_SUCCESS)
			printf("set_aperture_value error:%d\n",ret);
		else
			printf("aperture_value=%x\n",aperture_value);
			
		VISCA_usleep(3000000);
		
		if(aperture_value == 0) 
			aperture_value = 15;
		else
			aperture_value = 0;	
*/
/*		
		ret = set_iris_reset();
		if(ret!=VISCA_SUCCESS)
			printf("set_iris_reset error:%d\n",ret);
		else
			printf("set_iris_reset\n");
		VISCA_usleep(3000000);
*/		
/*
		ret = set_iris_value(iris_value);
		if(ret!=VISCA_SUCCESS)
			printf("set_aperture_value error:%d\n",ret);
		else
			printf("set_iris_value=%x\n",iris_value);
			
		VISCA_usleep(3000000);
		iris_value++;
		if(iris_value > 17) 
			iris_value = 5;
			

		ret = VISCA_get_iris_value(&iface, &camera, &p);
		if(ret!=VISCA_SUCCESS)
			printf("get_iris_value error");
		else
			printf("get_iris_value :%d\n",(*p));
			*/
		
		for(int i =0;i < 10; i++){
			ret = VISCA_set_focus_far(&iface, &camera);
			if(ret!=VISCA_SUCCESS)
				printf("set_focus_far error");
			else
				printf("set_focus_far\n");
			VISCA_usleep(3000000);
		}

		for(int i =0;i < 10; i++){
			ret = VISCA_set_focus_near(&iface, &camera);
			if(ret!=VISCA_SUCCESS)
				printf("set_focus_near error");
			else
				printf("set_focus_near\n");
			VISCA_usleep(3000000);
		}

/*
		//聚焦
		ret = set_focus_near_limit(focus_value);
		if(ret!=VISCA_SUCCESS)
			printf("set_focus_near_limit error:%d\n",ret);
		else
			printf("set_focus_near_limit=%x\n",focus_value);
			
		VISCA_usleep(3000000);
		focus_value = focus_value + 0x1000;
		if(focus_value > 0x7000)
			focus_value = 0x1000;
*/
/*
		ret = set_iris_up();
		if(ret!=VISCA_SUCCESS)
			printf("set_iris_up error");
		else
			printf("set_iris_up OK");
*/			


//		VISCA_usleep(3000000);
/*
		ret = VISCA_set_flip(&iface, &camera, 0);
		if(ret!=VISCA_SUCCESS)
			printf("set flip error:%d\n",ret);

			VISCA_usleep(3000000);

			
		ret = VISCA_set_flip(&iface, &camera, 1);
		if(ret!=VISCA_SUCCESS)
			printf("set flip error:%d\n",ret);

		VISCA_usleep(3000000);




		zoom_value=0;
		ret = VISCA_get_zoom_value(&iface, &camera, &zoom_value);

		if(ret==VISCA_SUCCESS)
			printf("zoom_value=%x\n",zoom_value);
		else
			printf("get zoom error:%d\n",ret);
		VISCA_usleep(3000000);



		if (VISCA_set_zoom_value(&iface, &camera, 0x3c68)!=VISCA_SUCCESS)
		  printf("error setting zoom\n");
		
		VISCA_usleep(3000000);

		zoom_value=0;
		VISCA_get_zoom_value(&iface, &camera, &zoom_value);
		if(ret==VISCA_SUCCESS)
			printf("zoom_value=%x\n",zoom_value);
		else
			printf("get zoom error:%d\n",ret);

		VISCA_usleep(3000000);

		
		if (VISCA_set_zoom_value(&iface, &camera, 0x1613)!=VISCA_SUCCESS)
		  printf("error setting zoom\n");
		VISCA_usleep(3000000);



		zoom_value=0;
		VISCA_get_zoom_value(&iface, &camera, &zoom_value);
		if(ret==VISCA_SUCCESS)
			printf("zoom_value=%x\n",zoom_value);
		else
			printf("get zoom error:%d\n",ret);

		VISCA_usleep(3000000);
*/
		
	}

	return 0;
}









































