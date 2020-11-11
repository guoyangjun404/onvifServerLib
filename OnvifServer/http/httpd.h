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

#ifndef HTTPD_H
#define HTTPD_H

#include "sys_inc.h"
#include "http.h"


struct mime_handler 
{
	const char * pattern;
	const char * mime_type;
	int          auth;
	
	int (*output)(const char *filename, char *buff, int buflen);
};


#ifdef __cplusplus
extern "C" {
#endif

void http_process_request(HTTPCLN * p_user, HTTPMSG * rx_msg);

#ifdef __cplusplus
}
#endif

#endif

