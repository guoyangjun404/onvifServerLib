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
#include "sys_buf.h"


/***************************************************************************************/
PPSN_CTX * net_buf_fl = NULL;

PPSN_CTX * hdrv_buf_fl = NULL;


/***************************************************************************************/
BOOL net_buf_init(int num, int size)
{
	net_buf_fl = pps_ctx_fl_init(num, size, TRUE);
	if (net_buf_fl == NULL)
	{
		return FALSE;
	}
	
	log_print(LOG_INFO, "%s, num = %lu\r\n", __FUNCTION__, net_buf_fl->node_num);

	return TRUE;
}

char * net_buf_get_idle()
{
	return (char *)pps_fl_pop(net_buf_fl);
}

void net_buf_free(char * rbuf)
{
	if (rbuf == NULL)
	{
		return;
	}
	
	if (pps_safe_node(net_buf_fl, rbuf))
	{
		pps_fl_push_tail(net_buf_fl, rbuf);
	}	
	else
	{
		free(rbuf);
	}	
}

uint32 net_buf_idle_num()
{
	if (net_buf_fl == NULL)
	{
		return 0;
	}
	
	return net_buf_fl->node_num;
}

void net_buf_deinit()
{
	if (net_buf_fl)
	{
		pps_fl_free(net_buf_fl);
		net_buf_fl = NULL;
	}
}

BOOL hdrv_buf_init(int num)
{
	hdrv_buf_fl = pps_ctx_fl_init(num, sizeof(HDRV), TRUE);
	if (hdrv_buf_fl == NULL)
	{
		return FALSE;
	}
	
	log_print(LOG_INFO, "%s, num = %lu\r\n", __FUNCTION__, hdrv_buf_fl->node_num);

	return TRUE;
}

void hdrv_buf_deinit()
{
	if (hdrv_buf_fl)
	{
		pps_fl_free(hdrv_buf_fl);
		hdrv_buf_fl = NULL;
	}
}

HDRV * hdrv_buf_get_idle()
{
	HDRV * p_ret = (HDRV *)pps_fl_pop(hdrv_buf_fl);

	return p_ret;
}

void hdrv_buf_free(HDRV * pHdrv)
{
	if (pHdrv == NULL)
	{
		return;
	}
	
	pHdrv->header[0] = '\0';
	pHdrv->value_string = NULL;
	
	pps_fl_push(hdrv_buf_fl, pHdrv);
}

uint32 hdrv_buf_idle_num()
{
	if (NULL == hdrv_buf_fl)
	{
		return 0;
	}
	
	return hdrv_buf_fl->node_num;
}

void hdrv_ctx_ul_init(PPSN_CTX * ul_ctx)
{
	pps_ctx_ul_init_nm(hdrv_buf_fl, ul_ctx);
}

void hdrv_ctx_free(PPSN_CTX * p_ctx)
{
	HDRV * p_free;

	if (p_ctx == NULL)
	{
		return;
	}
	
	p_free = (HDRV *)pps_lookup_start(p_ctx);
	while (p_free != NULL) 
	{
		HDRV * p_next = (HDRV *)pps_lookup_next(p_ctx, p_free);

		pps_ctx_ul_del(p_ctx, p_free);
		hdrv_buf_free(p_free);

		p_free = p_next;		
	}
	pps_lookup_end(p_ctx);
}


BOOL sys_buf_init(int nums)
{
	if (net_buf_init(nums, 2048) == FALSE)
	{
		log_print(LOG_ERR, "%s, net_buf_init failed!!!\r\n", __FUNCTION__);
		return FALSE;
	}

	if (hdrv_buf_init(8*nums) == FALSE)
	{
		log_print(LOG_ERR, "%s, hdrv_buf_init failed!!!\r\n", __FUNCTION__);
		return FALSE;
	}
    
	return TRUE;
}

void sys_buf_deinit()
{
	net_buf_deinit();
	hdrv_buf_deinit();
}


