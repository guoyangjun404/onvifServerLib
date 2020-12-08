
#include <stdio.h>
#include <unistd.h>
#include <errno.h>
#include <signal.h>
#include <stdbool.h>
#include "set_config.h"
#include "ntp_conf.h"
#include "utils_log.h"

#define PTZ_COM     "/dev/ttyAMA2"
#define VISCA_COM "/dev/ttyAMA3"

void signal_handler(int sig)
{
    if (SIGSEGV == sig){
        printf("SIGSEGV   \n"); 
        exit(0);
    }
}

void init_signal()
{
	//signal (SIGPIPE, pipe_handler);
    signal (SIGTERM, signal_handler);
    signal (SIGQUIT, signal_handler);
    signal (SIGINT,  signal_handler);
    signal (SIGTSTP, signal_handler);
    signal (SIGUSR1, signal_handler);
    //signal (SIGSEGV, signal_handler);
}

int main(int argc, char * argv[])
{
	init_signal();

	/* 打开onvif log开关 */
    // logOpen();
    logClose();

    /* 设备初始化 */
    devInit(PTZ_COM, VISCA_COM);

    /* onvi初始化 */
    onvifInit();

	/* 开启ntp线程 */
	ntp_update_run();


    /* onvif 开始 */
    onvifStart(); 

    while(1)
    {
        if (getchar() == 'q')
        {
            break;
        }
        usleep(5*1000*1000LL);
    } 
	
	// onvif 停止
	onvifStop();

    // onvif 反初始化
    onvifDeinit();
}


