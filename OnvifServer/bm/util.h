/***************************************************************************************
 *
 *  IMPORTANT: READ BEFORE DOWNLOADING, COPYING, INSTALLING OR USING.
 *
 *  By downloading, copying, installing or using the software you agree to this license.
 *  If you do not agree to this license, do not download, install, 
 *  copy or use the software.
 *
 *  Copyright (C) 2014-2019, Happytimesoft Corporation, all rights reserved.
 *
 *  Redistribution and use in binary forms, with or without modification, are permitted.
 *
 *  Unless required by applicable law or agreed to in writing, software distributed 
 *  under the License is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
 *  CONDITIONS OF ANY KIND, either express or implied. See the License for the specific
 *  language governing permissions and limitations under the License.
 *
****************************************************************************************/

#ifndef	__H_UTIL_H__
#define	__H_UTIL_H__


/*************************************************************************/

#ifdef __cplusplus
extern "C" {
#endif

/*************************************************************************/

#define MIN(a, b)           ((a) < (b) ? (a) : (b))
#define ARRAY_SIZE(ary)     (sizeof(ary) / sizeof(ary[0]))

/*************************************************************************/
#if __LINUX_OS__
#define stricmp(ds,ss) 		strcasecmp(ds,ss)
#define strnicmp(ds,ss,len) strncasecmp(ds,ss,len)
#endif

/*************************************************************************/
int 			get_if_nums();
uint32 	        get_if_ip(int index);
uint32 	        get_route_if_ip(uint32 dst_ip);
uint32 	        get_default_if_ip();
int 			get_default_if_mac(uint8 * mac);
uint32 	        get_address_by_name(const char * host_name);
const char    * get_default_gateway();
const char    * get_dns_server();
const char    * get_mask_by_prefix_len(int len);
int 			get_prefix_len_by_mask(const char * mask);
const char    * get_ip_str(uint32 ipaddr /* network byte order */);


/*************************************************************************/
char          * lowercase(char * str);
char          * uppercase(char * str);
int 			unicode(char ** dst, char * src);

char          * printmem(char * src, size_t len, int bitwidth);
char 		  * scanmem(char * src, int bitwidth);

int             url_encode(const char * src, const int srcsize, char * dst, const int dstsize);
int             url_decode(char * dst, char const * src, uint32 len);

/*************************************************************************/
time_t 	        get_time_by_string(char * p_time_str);
void            get_time_str(char * buff, int len);
void            get_time_str_day_off(time_t nt, char * buff, int len, int dayoff);
void            get_time_str_mon_off(time_t nt, char * buff, int len, int moffset);

SOCKET    		tcp_connect_timeout(uint32 rip, int port, int timeout);

/*************************************************************************/

#if __LINUX_OS__
int             daemon_init();
#endif

#if __WINDOWS_OS__
int             gettimeofday(struct timeval * tp, int * /*tz*/);
#endif

#ifdef __cplusplus
}
#endif

#endif	//	__H_UTIL_H__



