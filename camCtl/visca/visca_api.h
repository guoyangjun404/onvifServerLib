#ifndef __VISCA_IMG_H
#define __VISCA_IMG_H


#ifdef __cplusplus
extern "C" {
#endif

 
 /* 设置焦距值 */
int set_zoom(unsigned short val);
/* 获取焦距的初始值 */
int get_zoom_val();


/* 设备初始化 */
int visca_init(const char* device);

int visca_deinit();

/* 色饱和度 0-100 */
int set_colorsatuation_value(int param);

/* 对比度0-100 */
int set_contract_value(int param);

/* 亮度补偿 */
int set_exp_comp_value(int param);

/* 清晰度 (锐度) 0-15 */
int set_aperture_value(int param);

//相机焦距
int set_zoom_tele();
int set_zoom_wide();

int set_zoom_tele_speed(speed);
int set_zoom_wide_speed(speed);


int set_zoom_stop();


#ifdef __cplusplus
}
#endif

#endif