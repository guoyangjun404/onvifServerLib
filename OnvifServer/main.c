
#include <stdio.h>
#include <unistd.h>

#include "set_config.h"

#define VISCA_COM_ID	"/dev/ttyUSB1"
#define PTZ_COM_ID	      "/dev/ttyACM0"


int main(int argc, char * argv[])
{

    //设备初始化
    int ret = devInit(PTZ_COM_ID, VISCA_COM_ID);
    if (ret == 0)   printf("++++ dev init successful. +++++\r\n");

    //onvi初始化
    onvifInit();

    //打开onvif log开关
    // logOpen();
    // logClose();

    //onvif 开始
    onvifStart(); 

    for (;;)
    {
        if (getchar() == 'q')
        {
            // onvif 停止
            onvifStop();
            break;
        }

        sleep(5);
    } 

    // onvif 反初始化
    onvifDeinit();
}


