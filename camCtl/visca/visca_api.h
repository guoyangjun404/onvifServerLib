#ifndef __VISCA_IMG_H
#define __VISCA_IMG_H


#ifdef __cplusplus
extern "C" {
#endif
#define VISCA_COM_ID	"/dev/ttyUSB0"
// #define VISCA_COM_ID	"/dev/ttyAMA6"


/* 设备初始化 */
visca_init(const char* device);

/* 色饱和度 0-100 */
int set_colorsatuation_value(int param);

/* 对比度0-100 */
int set_contract_value(int param);

/* 亮度补偿 */
int set_exp_comp_value(int param);

/* 清晰度 (锐度) 0-15 */
int set_aperture_value(int param);


#ifdef __cplusplus
}
#endif

#endif