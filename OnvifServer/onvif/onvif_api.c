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

#include "sys_inc.h"
#include "hxml.h"
#include "xml_node.h"
#include "onvif_probe.h"
#include "http.h"
#include "http_parse.h"
#include "onvif_device.h"
#include "onvif.h"
#include "onvif_timer.h"
#include "onvif_api.h"

#include "set_config.h"

/***************************************************************************************/
static HTTPSRV http_srv[MAX_SERVERS];
extern ONVIF_CLS g_onvif_cls;
extern ONVIF_CFG g_onvif_cfg;

int logEnable = 0;

#define ONVIF_MAJOR_VERSION 7
#define ONVIF_MINOR_VERSION 2

/***************************************************************************************/

void logOpen()
{
    // g_onvif_cfg.log_enable = 1;
	logEnable = 1;
	log_set_level(LOG_DBG);		//log level在[1~5]都可以有打印
}
void logClose()
{
    // g_onvif_cfg.log_enable = 0;
	logEnable = 0;
	log_set_level(LOG_ERR);		//log level在[4~5]可以有打印，错误或者致命信息打印
}


void * onvif_task(void * argv)
{
    OIMSG stm;

	while (1)
	{
		if (hqBufGet(g_onvif_cls.msg_queue, (char *)&stm))
		{
			HTTPCLN * p_cln = (HTTPCLN *)stm.msg_dua;
			
			switch (stm.msg_src)
			{
			case ONVIF_MSG_SRC:
				http_msg_handler(p_cln, (HTTPMSG *)stm.msg_buf);
				if (stm.msg_buf) http_free_msg((HTTPMSG *)stm.msg_buf);
				if (p_cln) http_free_cln((HTTPSRV *)p_cln->p_srv, p_cln);
				break;

			case ONVIF_DEL_UA_SRC:
			    http_free_cln((HTTPSRV *)p_cln->p_srv, p_cln);
				break;

			case ONVIF_TIMER_SRC:
				onvif_timer();
				break;

			case ONVIF_EXIT:
			    goto EXIT;
			}
		}
	}

EXIT:

    g_onvif_cls.tid_main = 0;
    
	return NULL;
}


void onvif_start()
{
	int i;

	// onvif_init();

	if (logEnable)
	{
		printf("log_enable = %d. +++++++ 0:close, 1:open  +++++++\n", logEnable);
		log_init("ipsee.txt");
		// log_set_level(LOG_DBG);
	}

	// printf("\r\nHappytime onvif server version %d.%d\r\n", ONVIF_MAJOR_VERSION, ONVIF_MINOR_VERSION);

    g_onvif_cls.msg_queue = hqCreate(100, sizeof(OIMSG), HQ_GET_WAIT);
	if (g_onvif_cls.msg_queue == NULL)
	{
		log_print(LOG_ERR, "%s, create task queue failed!!!\r\n", __FUNCTION__);
		return;
	}
	
    g_onvif_cls.tid_main = sys_os_create_thread((void *)onvif_task, NULL);
    
    for (i = 0; i < g_onvif_cfg.servs_num; i++)
    {
        if (http_srv_init(&http_srv[i], g_onvif_cfg.servs[i].serv_ip, g_onvif_cfg.servs[i].serv_port, 
            g_onvif_cfg.http_max_users, g_onvif_cfg.https_enable) < 0)
    	{
    		printf("http server listen on %s:%d failed\r\n", g_onvif_cfg.servs[i].serv_ip, g_onvif_cfg.servs[i].serv_port);
    	}
    	else
    	{
    	    printf("\nOnvif server running at %s:%d\r\n", g_onvif_cfg.servs[i].serv_ip, g_onvif_cfg.servs[i].serv_port);
    	}
    }

	onvif_timer_init();

	onvif_start_discovery();
}

void onvif_stop()
{
    int i;

    OIMSG stm;
    memset(&stm, 0, sizeof(stm));

    stm.msg_src = ONVIF_EXIT;

    hqBufPut(g_onvif_cls.msg_queue, (char *)&stm);

    while (g_onvif_cls.tid_main)
    {
        usleep(10*1000);
    }
    
	onvif_stop_discovery();

	onvif_timer_deinit();

    for (i = 0; i < g_onvif_cfg.servs_num; i++)
    {
	    http_srv_deinit(&http_srv[i]);
	}

    log_close();
}


