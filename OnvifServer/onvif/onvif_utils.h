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

#ifndef _ONVIF_UTILS_H_
#define _ONVIF_UTILS_H_

#include "sys_inc.h"
#include "onvif.h"

#define onvif_print(...) fprintf(stdout, __VA_ARGS__); fflush(stdout)

#ifdef __cplusplus
extern "C" {
#endif

void 	     onvif_get_time_str(char * buff, int len, int sec_off);
void 	     onvif_get_time_str_s(char * buff, int len, time_t nowtime, int sec_off);
BOOL 	     onvif_is_valid_hostname(const char * name);
BOOL 	     onvif_is_valid_timezone(const char * tz);
void 	     onvif_get_timezone(char * tz, int len);

const char * onvif_uuid_create();
const char * onvif_get_local_ip();

time_t 	     onvif_timegm(struct tm *T);
int  	     onvif_parse_xaddr(const char * pdata, char * host, char * url, int * port, int * https);

time_t       onvif_datetime_to_time_t(onvif_DateTime * p_datetime);
void         onvif_time_t_to_datetime(time_t n, onvif_DateTime * p_datetime);
const char * onvif_format_datetime_str(time_t n, int flag, const char * format);
time_t	     onvif_timegm(struct tm *T);
int          onvif_parse_uri(const char * p_in, char * p_out, int outlen);

#ifdef __cplusplus
}
#endif


#endif


