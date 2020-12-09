#ifndef __SD_UTILS_LOG_H__
#define __SD_UTILS_LOG_H__


#ifdef __cplusplus
extern "C"
{
#endif
#include <stdio.h>
#include <stdarg.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <string.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h> 
#include <sys/ipc.h>
#include <sys/msg.h>
#include <syslog.h>
#include <sys/time.h>
#include <time.h>

#define COLOR_NONE              "\033[0000m"
#define COLOR_BLACK             "\033[0;30m"
#define COLOR_LIGHT_GRAY        "\033[0;37m"
#define COLOR_DARK_GRAY         "\033[1;30m"
#define COLOR_BLUE              "\033[0;34m"
#define COLOR_LIGHT_BLUE        "\033[1;34m"
#define COLOR_GREEN             "\033[1;32m"
#define COLOR_LIGHT_GREEN       "\033[1;32m"
#define COLOR_CYAN              "\033[0;36m"
#define COLOR_LIGHT_CYAN        "\033[1;36m"
#define COLOR_RED               "\033[1;31m"
#define COLOR_LIGHT_RED         "\033[1;31m"
#define COLOR_PURPLE            "\033[0;35m"
#define COLOR_LIGHT_PURPLE      "\033[1;35m"
#define COLOR_BROWN             "\033[0;33m"
#define COLOR_YELLOW            "\033[1;33m"
#define COLOR_WHITE             "\033[1;37m"

typedef enum
{
    LOG_DBG_TRACE = 0,
    LOG_DBG_DEBUG = 1,
    LOG_DBG_INFO  = 2,
    LOG_DBG_WARN  = 3,
    LOG_DBG_ERROR = 4,
    LOG_DBG_FATAL = 5,
	LOG_DBG_ZONE  = 6,
	LOG_DBG_ALIVE = 7,
}LOG_DBG_LEVEL;

#define LOG_MAX_SIZE 512

#define UTIL_TRACE(fmt,args...)	\
	do{\
		LogDBG_Utils(LOG_DBG_TRACE, "[Trace]=>[%s:%d]"  fmt  "" , __FUNCTION__, __LINE__, ##args);\
	}while(0)

#define UTIL_DEBUG(fmt,args...)	\
	do{\
		LogDBG_Utils(LOG_DBG_DEBUG, "[Debug]=>[%s:%d]"  fmt  "" , __FUNCTION__, __LINE__, ##args);\
	}while(0)

#define UTIL_INFO(fmt,args...)	\
    do{\
		LogDBG_Utils(LOG_DBG_INFO, "[Info]=>[%s:%d]"  fmt  "" , __FUNCTION__, __LINE__, ##args);\
	}while(0)

#define UTIL_WARN(fmt,args...)	\
	do{\
		LogDBG_Utils(LOG_DBG_WARN, "[Warn]=>[%s:%d]"  fmt  "" , __FUNCTION__, __LINE__, ##args);\
	}while(0)

#define UTIL_ERR(fmt,args...)	\
	do{\
		LogDBG_Utils(LOG_DBG_ERROR, "[Error]=>[%s:%d]"  fmt  "" , __FUNCTION__, __LINE__, ##args);\
	}while(0)

#define UTIL_FATAL(fmt,args...)	\
	do{\
		LogDBG_Utils(LOG_DBG_FATAL, "[Fatal]=>[%s:%d]"  fmt  "" , __FUNCTION__, __LINE__, ##args);\
	}while(0)


#define UTIL_SHOW(fmt,args...)	\
	do{\
		LogDBG_Utils(LOG_DBG_INFO, "[Debug]=>[%s:%d]"  fmt  "" , __FUNCTION__, __LINE__, ##args);\
	}while(0)
	
#define UTIL_OTHER(fmt,args...)	\
	do{\
		LogDBG_Utils(LOG_DBG_DEBUG, "[Debug]=>[%s:%d]"  fmt  "" , __FUNCTION__, __LINE__, ##args);\
	}while(0)
	
#define UTIL_ZONE(fmt,args...)	\
	do{\
		LogDBG_Utils(LOG_DBG_ZONE, "[SETZONE]=>[%s:%d]"	fmt  "" , __FUNCTION__, __LINE__, ##args);\
	}while(0)
	
#define UTIL_ALIVE(fmt,args...)	\
	do{\
		LtyLogOut(LOG_INFO, "[ALIVE]=>[%s:%d]"	fmt  "" , __FUNCTION__, __LINE__, ##args);\
	}while(0)


#define LOG_MSG_MAX_SIZE 	4094 // 2048 - sizeof(mLevel) - 1
typedef struct tag_LogDBG_Pkg
{
	unsigned int  iLogLen;
    unsigned char pLogBuf[LOG_MSG_MAX_SIZE-4];
}LogContext;

static void LtyLogOut(int priority, const char *fmt, ...)
{
    va_list ap;
    va_start(ap, fmt);
    vsyslog(priority, fmt, ap);
    va_end(ap);
}


static int LogDBG_Utils(unsigned char ucLevel, const char* fmt, ...)
{
    LogContext pStContext;
    int size = 0;
    memset(pStContext.pLogBuf, 0, sizeof(pStContext.pLogBuf));
    
    va_list ap;
    va_start(ap, fmt);
    size += vsnprintf(pStContext.pLogBuf + size, sizeof(pStContext.pLogBuf) - size - 1, fmt, ap);
    pStContext.pLogBuf[size] = '\0';
    va_end(ap);

	LtyLogOut(LOG_INFO, "%s\n", pStContext.pLogBuf);
	/*struct tm NowTime; 
	char TimeBuf[1024];   
	struct timeval structTimeSpec;
    gettimeofday(&structTimeSpec, NULL);
    localtime_r(&structTimeSpec.tv_sec, &NowTime);
	snprintf(TimeBuf, sizeof(TimeBuf), "[%d-%02d-%02d %02d:%02d:%02d] %s\n", 
			 1900 + NowTime.tm_year, 1 + NowTime.tm_mon, NowTime.tm_mday, 
			 NowTime.tm_hour, NowTime.tm_min, NowTime.tm_sec, pStContext.pLogBuf); 
	write(stdout, TimeBuf, strlen(TimeBuf)-1);*/

	printf("%s\n", pStContext.pLogBuf);
	return 0;
}

#ifdef __cplusplus
}
#endif

#endif //__SD_UTILS_LOG_H__
