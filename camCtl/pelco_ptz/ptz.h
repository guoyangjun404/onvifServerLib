/*---------------------------- pelco.h---------------------------------------
*	Serial control port for 1650A.
*	01/18/97	started	Joe Briggs
*-------------------------------------------------------------------------*/
#ifndef __PTZ_H
#define __PTZ_H
#include <sys/signal.h>
#include <sys/types.h>
#include <string.h>
#include <termios.h>
#include <unistd.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <termios.h>
#include <sys/ioctl.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <time.h>

#define RET_OK  0
#define RET_ERR  -1

typedef int error_code;
typedef unsigned int u_int32;

typedef enum {p_sync,p_addr,p_cmd1,p_cmd2,p_data1,p_data2,p_cksum}pelco_t;
/*
十四条基本命令，应用层可以任意组合出丰富复杂的控制云台命令
以下函数返回只是当前的命令读写状态正确与否，并不能保证云台正确接收和执行，需要应用层通过查询执行前后的变化做出判断
*/


/* device:打开的串口设备名称,baud_rate:baud_rate */
error_code pelco_Init(char* device, u_int32 baud_rate);
error_code  pelco_close();
/* force:Speed  上下左右速度只取低8位*/
error_code pelco_Left(unsigned short force);
error_code pelco_Right(unsigned short force);
error_code pelco_Up(unsigned short force);
error_code pelco_Down(unsigned short force);

/* force:Speed  低8位是水平方向速度取值区间(0x00-0x3F)，高8位是垂直方向的速度取值区间(0x00-0x3F)*/

error_code pelco_left_down(unsigned short force);
error_code pelco_right_down(unsigned short force);
error_code pelco_left_up(unsigned short force);
error_code pelco_right_up(unsigned short force);

/* 预置点设置和调用命令
location:location  只用低8位,取值范围0-0x80 
*/
error_code pelco_set_point(unsigned short location);
error_code pelco_get_point(unsigned short location);


/*设置位置  location:location要设置的位置数据   */
error_code pelco_set_vertical(unsigned short location);
error_code pelco_set_horizontal(unsigned short location);


/*获取位置  location:location 返回的位置数据 */

error_code pelco_get_vertical(unsigned short *location);
error_code pelco_get_horizontal(unsigned short* location);

/*停止运动*/
error_code pelco_Stop(void);


#endif /* ptz */
