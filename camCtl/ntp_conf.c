/* *********************************************************************
 *	Description :
 *		Set the NTP server or PC clock for adjusting system time.
 *
 ************************************************************************/
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <fcntl.h>
#include <getopt.h>
#include <errno.h>
#include <stdint.h>
#include <sys/socket.h>
#include <netdb.h>
#include <sys/ioctl.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <sys/time.h>
#include <time.h>
#include <linux/rtc.h>
#include <fcntl.h>
#include <pthread.h>
#include <sys/prctl.h>
#include "onvif.h"
#include "set_config.h"
#include "utils_log.h"

#define NTP_VERSION 		0xe3
#define NTP_DEFAULT_PORT	"123"
typedef struct tag_ntp_server 
{
	char server[24];
	char port[8]; 
}NTP_SERVER_T;

extern ONVIF_CFG g_onvif_cfg;

#define SEC_IN_YEAR 		31556926
#define UNIX_OFFSET 		2208988800UL
#define VN_BITMASK(byte) 	((byte & 0x3f) >> 3)
#define LI_BITMASK(byte) 	(byte >> 6)
#define MODE_BITMASK(byte) 	(byte & 0x7)
#define ENDIAN_SWAP32(data)  	((data >> 24) | /* right shift 3 bytes */ \
	((data & 0x00ff0000) >> 8) | /* right shift 1 byte */ \
	((data & 0x0000ff00) << 8) | /* left shift 1 byte  */ \
	((data & 0x000000ff) << 24)) /* left shift 3 bytes */
	
struct ntpPacket 
{	
	uint8_t flags;	
	uint8_t stratum;	
	uint8_t poll;	
	uint8_t precision;	
	uint32_t root_delay;	
	uint32_t root_dispersion;	
	uint8_t referenceID[4];	
	uint32_t ref_ts_sec;	
	uint32_t ref_ts_frac;	
	uint32_t origin_ts_sec;	
	uint32_t origin_ts_frac;	
	uint32_t recv_ts_sec;	
	uint32_t recv_ts_frac;	
	uint32_t trans_ts_sec;	
	uint32_t trans_ts_frac;
} __attribute__((__packed__)); /* this is not strictly necessary, structure follows alignment rules */


NTP_SERVER_T sync_server_info[]={
	{"ntp1.aliyun.com",    NTP_DEFAULT_PORT},
	{"time1.cloud.tencent.com",      NTP_DEFAULT_PORT},
	{"0.asia.pool.ntp.org",NTP_DEFAULT_PORT},
	{"ntp.ntsc.ac.cn",NTP_DEFAULT_PORT},
	{"cn.pool.ntp.org",NTP_DEFAULT_PORT},
    {"pool.ntp.org",NTP_DEFAULT_PORT},
};

int check_tm_duration(struct tm *rtctime, struct tm *datetime)
{
	int ucRtcTimeSeconds = rtctime->tm_hour*60*60 + rtctime->tm_min*60 + rtctime->tm_sec;
	int ucDateTimeSeconds = datetime->tm_hour*60*60 + datetime->tm_min*60 + datetime->tm_sec;

   if (rtctime->tm_year == datetime->tm_year && 
	 	rtctime->tm_mon == datetime->tm_mon &&
	 	rtctime->tm_mday == datetime->tm_mday){
	       if (abs(ucRtcTimeSeconds - ucDateTimeSeconds) < 10){
		   	   UTIL_INFO("[ooo]system time and rtc time <10 [ooo]!!!!!");
		   	   return 0;
	       }
	}
    return -1;
}

void COMM_GetSystemUpMSecs(unsigned long long *pullTimeStamp)
{
	struct timespec structTimeSpec;
	clock_gettime(CLOCK_MONOTONIC,&structTimeSpec);
	*pullTimeStamp = (unsigned long long)((unsigned long long)structTimeSpec.tv_sec*1000 
                      + (unsigned long long)structTimeSpec.tv_nsec/1000000);  
}

int sync_time_with_server(NTP_SERVER_T *ntp_server, struct timeval tv) {
	char *server_addr = ntp_server->server;
	char *port = ntp_server->port;
	socklen_t addrlen = sizeof(struct sockaddr_storage);

	struct ntpPacket packet;

	int server_sock; /* send through this socket */
	int error; /* error checking */
	unsigned int recv_secs;
    struct tm sysnow;
    struct timeval structsysTimeSpec;

	time_t total_secs;
	struct tm now;

	fd_set read_fds;
	struct timeval timeout;
	int results;
	char cmd[50];
	int updatedone = -1;

    struct sockaddr_in addr;

	memset(&packet, 0, sizeof(struct ntpPacket));
	packet.flags = NTP_VERSION;

	struct addrinfo hints, *result, *resave;
	bzero(&hints, sizeof (struct addrinfo)) ;
	memset(&hints, 0, sizeof(hints));
	hints.ai_socktype = SOCK_DGRAM;
	if ((error=getaddrinfo(server_addr, port, &hints, &result))!=0) {
		//UTIL_ERR("getaddrinfo %s failed:errno %s\n", server_addr, gai_strerror(error));
		return -1;
	}
	
	resave = result;
	do {
	    if (result->ai_family == AF_INET){
			bzero(&addr,sizeof(addr));
			addr.sin_port = htons(port);
			memcpy(&addr,(struct sockaddr_in*)result->ai_addr, result->ai_addrlen);
			break;
	    }
	}while ((result=result->ai_next)!=NULL);
	
	freeaddrinfo(resave);
	
	if (result == NULL)
		return -1;

    server_sock = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
    if (server_sock < 0)
    {
        UTIL_ERR("socket() error");
		return -1;
    }
    
	error = sendto(server_sock, (char *)&packet, sizeof(struct ntpPacket), 0, (struct sockaddr *)&addr, sizeof(addr));
	if (error == -1) {
		UTIL_ERR("sendto() error %d %s", errno, strerror(errno));
		close(server_sock);
		return -1;
	}

	FD_ZERO(&read_fds);
	FD_SET(server_sock, &read_fds);

	timeout.tv_sec = tv.tv_sec;
	timeout.tv_usec = tv.tv_usec;

	results = select(server_sock + 1, &read_fds, NULL, NULL, &timeout);
	if (results < 0) {
		UTIL_ERR("select error");
	}
	else if (results == 0) {
		UTIL_INFO("select time out");
	}
	else {
		if (FD_ISSET(server_sock, &read_fds)) {
			error = recvfrom(server_sock, &packet, sizeof(struct ntpPacket), 0, (struct sockaddr *)&addr, &addrlen);
			if (error == -1) {
				UTIL_ERR("recvfrom() error");
				close(server_sock);
				return -1;
			}

			packet.recv_ts_sec = ENDIAN_SWAP32(packet.recv_ts_sec);
			/* print date with receive timestamp */
			recv_secs = packet.recv_ts_sec - UNIX_OFFSET; /* convert to unix time */
			total_secs = recv_secs;
			localtime_r(&total_secs, &now);	
            
            gettimeofday(&structsysTimeSpec, NULL);
            localtime_r(&structsysTimeSpec.tv_sec, &sysnow);
			if (check_tm_duration(&sysnow, &now) < 0)
			{
				snprintf(cmd, sizeof(cmd), "date -s %04d%02d%02d%02d%02d.%02d", now.tm_year+1900, now.tm_mon+1,
						now.tm_mday, now.tm_hour, now.tm_min, now.tm_sec);
				UTIL_INFO("set system time with comand \"%s\"", cmd);
				system_ex(cmd);
				system_ex("hwclock -w;hwclock -s");
			}
			updatedone = 1;
		}
	}
    close(server_sock);
	return updatedone;
}

int sync_time(onvif_NTPInformation		*pNTPInformation) 
{
	int i = 0;
	int result = 0;
	struct timeval tv;
	tv.tv_sec = 20;
	tv.tv_usec = 0;
	
    //从自定义的ntp服务器获取
	if (pNTPInformation->FromDHCP == FALSE) {
		NTP_SERVER_T ntp_server_info;
		if (strlen(pNTPInformation->NTPServer[0]) > 0) {
			memset(&ntp_server_info, 0x0, sizeof(ntp_server_info));
			strcpy(ntp_server_info.port, NTP_DEFAULT_PORT);
			strncpy(ntp_server_info.server, pNTPInformation->NTPServer[0], strlen(pNTPInformation->NTPServer[0]));			
			UTIL_INFO("ntp_server_info.server=%s", ntp_server_info.server);
			result = sync_time_with_server(&ntp_server_info, tv);
			if (result > 0) {
				UTIL_INFO("NTP Success!");
			}
		}
	}
	else {//从默认ntp服务器获取
		UTIL_INFO("DEFAULT server=%s", sync_server_info[0].server);
		for (i = 0; i < (sizeof(sync_server_info)/sizeof(NTP_SERVER_T)); i++) {
			result = sync_time_with_server(&(sync_server_info[i]), tv);
			if (result > 0) {
				UTIL_INFO("NTP Success!");
				break;
			}
		}
	}
	return result;
}

void * ntpdate_Thread (void * arg)
{	
	prctl(PR_SET_NAME, (unsigned long)"NtpThread");
    unsigned long long ulllasttimeMs = 0;
    unsigned long long ullcurtimeMs = 0;
	
    unsigned long long ullheartlasttimeMs = 0;
    unsigned long long ullheartcurtimeMs = 0;
	int firstflag = 0;

	if (g_onvif_cfg.SystemDateTime.DateTimeType == SetDateTimeType_NTP) {
		UTIL_INFO("SetDateTimeType_NTP");
		firstflag = 1;
	}
	else {
		UTIL_INFO("SetDateTimeType_MANUAL");
		//同步rtc时间
		sync_hwclock_tosys();
	}
	
    while(1){
		//客户设置立即生效
		if (1 == Get_Start_NTP_Server()) {
			Set_Start_NTP_Server(0);
			firstflag = 1;
			if (g_onvif_cfg.SystemDateTime.DateTimeType == SetDateTimeType_NTP)
				UTIL_INFO("SetDateTimeType_NTP");
			else 
				UTIL_INFO("SetDateTimeType_MANUAL");
		    UTIL_INFO("Start_NTP_Server!!!!");
		}
		
		if (g_onvif_cfg.SystemDateTime.DateTimeType == SetDateTimeType_NTP) {
	        COMM_GetSystemUpMSecs(&ullcurtimeMs);
	        if (firstflag == 1 || (ullcurtimeMs - ulllasttimeMs)/1000 >= 12 * 60 * 60){
				if (sync_time(&g_onvif_cfg.network.NTPInformation) > 0) {
				    UTIL_INFO("beyond 12 hours Sync time success");
				}
	            else{
	                UTIL_INFO("beyond 12 hours but fail to sync time");
	            }
				firstflag = 0;
				ulllasttimeMs = ullcurtimeMs;
	        }
		}

        //主要用于心跳
		COMM_GetSystemUpMSecs(&ullheartcurtimeMs);
        if ((ullheartcurtimeMs - ullheartlasttimeMs) >= 30* 1000){
			ullheartlasttimeMs = ullheartcurtimeMs;
			UTIL_ALIVE("MAIN PROCESS!!!");
        }
        usleep(10*1000*1000LL);
    }
    
	return NULL;
}


int Init_TimeZone()
{
	char cscurTZ[32] = {0};
	char linuxTz[32] = {0};
    int ret = 0;
	char tmp[4] = {0};
	int tmp1 = 0;
		
	sprintf(cscurTZ, "%s", g_onvif_cfg.SystemDateTime.TimeZone.TZ);
	UTIL_INFO("cscurTZ===%s", g_onvif_cfg.SystemDateTime.TimeZone.TZ);
	
	if (strstr(cscurTZ, "CST") != NULL) 
	{
        //是否夏令时
		/*if (g_onvif_cfg.SystemDateTime.DaylightSavings == TRUE)
		{
			snprintf(tmp, 3, "%s", &cscurTZ[4]);
			if (strncmp(&cscurTZ[3], "+", 1) == 0)
			{
				tmp1 = atoi(tmp) + 1;
				sprintf(linuxTz, "GMT-%02d:%s", tmp1, &cscurTZ[7]);
			}
			else
			{
				tmp1 = atoi(tmp) - 1;
				sprintf(linuxTz, "GMT+%02d:%s", tmp1, &cscurTZ[7]);
			}
		}
		else */
		{
			if (strncmp(&cscurTZ[3], "+", 1) == 0)
			{
				sprintf(linuxTz, "GMT+%s", &cscurTZ[4]);
			}
			else
			{
				sprintf(linuxTz, "GMT-%s", &cscurTZ[4]);
			}
		}
	}
	else
	{
		strcpy(linuxTz, "GMT-08:00");
	}
	UTIL_INFO("the current cscurTZ Tz is %s linuxTZP:%s", cscurTZ, linuxTz);	

    ret = setenv("TZ", linuxTz, 1);
    if (ret != 0){
        UTIL_INFO("fail to set Tz %s", linuxTz);
    }
	
	return 0;
}

int ntp_update_run()
{
	int ret = -1;
	pthread_t thread_id;
	
	Init_TimeZone();

	ret=pthread_create(&thread_id, NULL, (void *)ntpdate_Thread, NULL);
	if (ret != 0)
	{
		UTIL_INFO("ntp_update_thread create error!");
		return -1;
	}
	pthread_detach(thread_id);
	
	return 0;
}

