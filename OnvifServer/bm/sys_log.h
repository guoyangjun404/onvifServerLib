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

#ifndef	__H_SYS_LOG_H__
#define	__H_SYS_LOG_H__

// log level

#define LOG_TRC     0        
#define LOG_DBG     1
#define LOG_INFO    2
#define LOG_WARN    3
#define LOG_ERR     4
#define LOG_FATAL   5

#ifdef __cplusplus
extern "C" {
#endif

int 	log_init(const char * log_fname);
int 	log_time_init(const char * fname_prev);
int     log_reinit(const char * log_fname);
int     log_time_reinit(const char * fname_prev);
void 	log_close();
void    log_set_level(int level);
int     log_get_level();

#ifdef IOS
int 	log_printfff(int level, const char * fmt,...);
#define log_print log_printfff
#else
int 	log_print(int level, const char * fmt,...);
#endif

int 	log_lock_start(const char * fmt,...);
int 	log_lock_print(const char * fmt,...);
int 	log_lock_end(const char * fmt,...);

#ifdef __cplusplus
}
#endif

#endif	//	__H_SYS_LOG_H__



