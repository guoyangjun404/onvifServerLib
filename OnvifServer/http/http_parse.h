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

#ifndef HTTP_PARSE_H
#define HTTP_PARSE_H

#include "sys_inc.h"
#include "http.h"


#ifdef __cplusplus
extern "C" {
#endif

/***********************************************************************/
BOOL 	  http_msg_buf_init(int num);
void 	  http_msg_buf_deinit();

/***********************************************************************/
BOOL 	  http_is_http_msg(char * msg_buf);
int       http_pkt_find_end(char * p_buf);
void 	  http_headl_parse(char * pline, int llen, HTTPMSG * p_msg);
int 	  http_line_parse(char * p_buf, int max_len, char sep_char, PPSN_CTX * p_ctx);
int 	  http_ctt_parse(HTTPMSG * p_msg);
int 	  http_msg_parse(char * msg_buf, int msg_buf_len, HTTPMSG * msg);
int 	  http_msg_parse_part1(char * p_buf, int buf_len, HTTPMSG * msg);
int 	  http_msg_parse_part2(char * p_buf, int buf_len, HTTPMSG * msg);
HDRV  	* http_find_headline(HTTPMSG * msg, const char * head);
char  	* http_get_headline(HTTPMSG * msg, const char * head);
HDRV  	* http_find_ctt_headline(HTTPMSG * msg, const char * head);
char  	* http_get_ctt(HTTPMSG * msg);
BOOL      http_get_auth_digest_info(HTTPMSG * rx_msg, HD_AUTH_INFO * p_auth);

HTTPMSG * http_get_msg_buf();
HTTPMSG * http_get_msg_large_buf(int size);
void 	  http_msg_ctx_init(HTTPMSG * msg);
void 	  http_free_msg_buf(HTTPMSG * msg);
uint32    http_idle_msg_buf_num();

/***********************************************************************/
void 	  http_free_msg(HTTPMSG * msg);
void 	  http_free_msg_content(HTTPMSG * msg);
void 	  http_free_msg_ctx(HTTPMSG * msg, int type);


#ifdef __cplusplus
}
#endif

#endif



