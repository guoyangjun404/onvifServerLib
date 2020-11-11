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
#include "ppstack.h"

/***************************************************************************************/
PPSN_CTX * pps_ctx_fl_init_assign(char * mem_addr, unsigned long mem_len, unsigned long node_num, unsigned long content_size, BOOL bNeedMutex)
{
	PPSN_CTX * ctx_ptr;
	uint32 i = 0;
	unsigned long unit_len = content_size + sizeof(PPSN);
	unsigned long content_len = node_num * unit_len;
	
	if (mem_len < (content_len + sizeof(PPSN_CTX)))
	{
		log_print(LOG_ERR, "%s, assign mem len too short!!!\r\n", __FUNCTION__);
		return NULL;
	}

	ctx_ptr = (PPSN_CTX *)mem_addr;
	memset(ctx_ptr, 0, sizeof(PPSN_CTX));
	memset((char *)(mem_addr+sizeof(PPSN_CTX)), 0, content_len);

	for (; i<node_num; i++)
	{
		uint32 offset = sizeof(PPSN_CTX) + unit_len * i;
		PPSN * p_node = (PPSN *)(mem_addr + offset);

		if (ctx_ptr->head_node == 0)
		{
			ctx_ptr->head_node = offset;
			ctx_ptr->tail_node = offset;
		}
		else
		{
			PPSN * p_prev_node = (PPSN *)(mem_addr + ctx_ptr->tail_node);
			p_prev_node->next_node = offset;
			p_node->prev_node = ctx_ptr->tail_node;
			ctx_ptr->tail_node = offset;
		}

		p_node->node_flag = 1;

		(ctx_ptr->node_num)++;
	}

	if (bNeedMutex)
	{
		ctx_ptr->ctx_mutex = sys_os_create_mutex();
	}	
	else
	{
		ctx_ptr->ctx_mutex = 0;
	}
	
	ctx_ptr->fl_base = (char *)ctx_ptr;
	ctx_ptr->low_offset = sizeof(PPSN_CTX) + sizeof(PPSN);
	ctx_ptr->high_offset = sizeof(PPSN_CTX) + content_len - unit_len + sizeof(PPSN);
	ctx_ptr->unit_size = unit_len;

	return ctx_ptr;
}

PPSN_CTX * pps_ctx_fl_init(unsigned long node_num, unsigned long content_size, BOOL bNeedMutex)
{
	unsigned long unit_len = content_size + sizeof(PPSN);
	unsigned long content_len = node_num * unit_len;
	char * content_ptr;
	PPSN_CTX * ctx_ptr;
	
	content_ptr = (char *)malloc(content_len + sizeof(PPSN_CTX));
	if (content_ptr == NULL)
	{
		log_print(LOG_ERR, "%s, memory malloc failed,len = %d\r\n", __FUNCTION__, content_len);
		return NULL;
	}

	ctx_ptr = pps_ctx_fl_init_assign(content_ptr, content_len+sizeof(PPSN_CTX),	node_num, content_size, bNeedMutex);

	return ctx_ptr;
}

void pps_fl_free(PPSN_CTX * fl_ctx)
{
	if (fl_ctx == NULL) 
	{
		return;
	}
	
	if (fl_ctx->ctx_mutex)
	{
		sys_os_destroy_sig_mutx(fl_ctx->ctx_mutex);
	}

	free(fl_ctx);
}

/***************************************************************************************/
void pps_fl_reinit(PPSN_CTX * fl_ctx)
{
	uint32 i = 0;
	char * mem_addr;
	char * content_start;
	char * content_end;
	unsigned long content_len;
	
	if (fl_ctx == NULL) 
	{
		return;
	}
	
	mem_addr = (char *)fl_ctx;

	pps_wait_mutex(fl_ctx);

	content_start = (char *)(mem_addr + fl_ctx->low_offset - sizeof(PPSN));
	content_end = (char *)(mem_addr + fl_ctx->high_offset - sizeof(PPSN) + fl_ctx->unit_size);

	content_len = (unsigned long)(content_end - content_start);
	fl_ctx->node_num = content_len / fl_ctx->unit_size;
	fl_ctx->head_node = 0;
	fl_ctx->tail_node = 0;

	memset(content_start, 0, content_len);
	
	for (; i<fl_ctx->node_num; i++)
	{
		uint32 offset = sizeof(PPSN_CTX) + fl_ctx->unit_size * i;
		PPSN * p_node = (PPSN *)(mem_addr + offset);

		if (fl_ctx->head_node == 0)
		{
			fl_ctx->head_node = offset;
			fl_ctx->tail_node = offset;
		}
		else
		{
			PPSN * p_prev_node = (PPSN *)(mem_addr + fl_ctx->tail_node);
			p_prev_node->next_node = offset;
			p_node->prev_node = fl_ctx->tail_node;
			fl_ctx->tail_node = offset;
		}

		p_node->node_flag = 1;	
	}

	pps_post_mutex(fl_ctx);
}

BOOL pps_fl_push(PPSN_CTX * pps_ctx, void * content_ptr)
{
	PPSN * p_node;
	unsigned long offset;
	
	if (pps_ctx == NULL || content_ptr == NULL)
	{
		return FALSE;
	}
	
	if (pps_safe_node(pps_ctx, content_ptr) == FALSE)
	{
		log_print(LOG_WARN, "%s, unit ptr error!!!\r\n", __FUNCTION__);
		return FALSE;
	}

	p_node = (PPSN *)(((char *)content_ptr) - sizeof(PPSN));

	offset = (unsigned long)((char *)p_node - pps_ctx->fl_base);

	pps_wait_mutex(pps_ctx);

	if (p_node->node_flag == 1)
	{
		log_print(LOG_WARN, "%s, unit node %d already in freelist !!!\r\n", __FUNCTION__, pps_get_index(pps_ctx, content_ptr));

		pps_post_mutex(pps_ctx);
		return FALSE;
	}

	p_node->prev_node = 0;
	p_node->node_flag = 1;

	if (pps_ctx->head_node == 0)
	{
		pps_ctx->head_node = offset;
		pps_ctx->tail_node = offset;
		p_node->next_node = 0;
	}
	else
	{
		PPSN * p_prev = (PPSN *)(pps_ctx->head_node + (char *)pps_ctx->fl_base);
		p_prev->prev_node = offset;
		p_node->next_node = pps_ctx->head_node;
		pps_ctx->head_node = offset;
	}

	pps_ctx->node_num++;
	pps_ctx->push_cnt++;

	pps_post_mutex(pps_ctx);

	return TRUE;
}

BOOL pps_fl_push_tail(PPSN_CTX * pps_ctx, void * content_ptr)
{
	PPSN * p_node;
	unsigned long offset;
	
	if (pps_ctx == NULL || content_ptr == NULL)
	{
		return FALSE;
	}
	
	if (pps_safe_node(pps_ctx, content_ptr) == FALSE)
	{
		log_print(LOG_WARN, "%s, unit ptr error!!!\r\n", __FUNCTION__);
		return FALSE;
	}

	p_node = (PPSN *)(((char *)content_ptr) - sizeof(PPSN));

	offset = (unsigned long)((char *)p_node - pps_ctx->fl_base);

	pps_wait_mutex(pps_ctx);

	if (p_node->node_flag == 1)
	{
		log_print(LOG_WARN, "%s, unit node %d already in freelist !!!\r\n", __FUNCTION__, pps_get_index(pps_ctx, content_ptr));

		pps_post_mutex(pps_ctx);
		return FALSE;
	}

	p_node->prev_node = 0;
	p_node->next_node = 0;
	p_node->node_flag = 1;

	if (pps_ctx->tail_node == 0)
	{
		pps_ctx->head_node = offset;
		pps_ctx->tail_node = offset;
	}
	else
	{
		PPSN * p_prev;

		p_node->prev_node = pps_ctx->tail_node;
		p_prev = (PPSN *)(pps_ctx->tail_node + (char *)pps_ctx);
		p_prev->next_node = offset;
		pps_ctx->tail_node = offset;
	}

	pps_ctx->node_num++;
	pps_ctx->push_cnt++;

	pps_post_mutex(pps_ctx);

	return TRUE;
}

void * pps_fl_pop(PPSN_CTX * pps_ctx)
{
	PPSN * p_node;
	
	if (pps_ctx == NULL)
	{
		return NULL;
	}
	
	pps_wait_mutex(pps_ctx);

	if (pps_ctx->head_node == 0)
	{
		pps_post_mutex(pps_ctx);
		return NULL;
	}

	p_node = (PPSN *)(pps_ctx->fl_base + pps_ctx->head_node);

	pps_ctx->head_node = p_node->next_node;

	if (pps_ctx->head_node == 0)
	{
		pps_ctx->tail_node = 0;
	}	
	else
	{
		PPSN * p_new_head = (PPSN *)(pps_ctx->fl_base + pps_ctx->head_node);
		p_new_head->prev_node = 0;
	}

	(pps_ctx->node_num)--;
	(pps_ctx->pop_cnt)++;

	pps_post_mutex(pps_ctx);

	memset(p_node, 0, sizeof(PPSN));	

	return (void *)(((char *)p_node) + sizeof(PPSN));
}

void pps_ctx_fl_show(PPSN_CTX * pps_ctx)
{
	uint32 offset;
	unsigned long ctx_count = 0;
	
	if (pps_ctx == NULL)
	{
		return;
	}
	
	pps_wait_mutex(pps_ctx);

	log_print(LOG_DBG, "PPSN_CTX[0x%p]::unit size = %d,unit num = %d,head = %d,tail = %d\r\n",
		pps_ctx->fl_base, pps_ctx->unit_size, pps_ctx->node_num, pps_ctx->head_node, pps_ctx->tail_node);

	offset = pps_ctx->head_node;
	
	while (offset != 0)
	{
		PPSN * p_node = (PPSN *)(pps_ctx->fl_base + offset);
		
		log_print(LOG_DBG, "0x%p == FLAG: %d  next: 0x%08x  prev: 0x%08x\r\n",
			p_node, p_node->node_flag, p_node->next_node, p_node->prev_node);

		ctx_count++;

		if (ctx_count > pps_ctx->node_num)
		{
			log_print(LOG_WARN, "\r\n!!!FreeList Error,Linked item count[%u] > real item count[%u]\r\n", ctx_count, pps_ctx->node_num);
			break;
		}

		offset = p_node->next_node;
	}

	log_print(LOG_INFO, "\r\nFreeList Linked item count[%d]\r\n", ctx_count);

	pps_post_mutex(pps_ctx);
}

/***************************************************************************************/
BOOL pps_ctx_ul_init_assign(PPSN_CTX * ul_ctx, PPSN_CTX * fl_ctx, BOOL bNeedMutex)
{
	if (ul_ctx == NULL || fl_ctx == NULL)
	{
		return FALSE;
	}
	
	memset(ul_ctx, 0, sizeof(PPSN_CTX));

	ul_ctx->fl_base = fl_ctx->fl_base;
	ul_ctx->high_offset = fl_ctx->high_offset;
	ul_ctx->low_offset = fl_ctx->low_offset;
	ul_ctx->unit_size = fl_ctx->unit_size;

	if (bNeedMutex)
	{
		ul_ctx->ctx_mutex = sys_os_create_mutex();
	}	
	else
	{
		ul_ctx->ctx_mutex = 0;
	}
	
	return TRUE;
}

PPSN_CTX * pps_ctx_ul_init(PPSN_CTX * fl_ctx, BOOL bNeedMutex)
{
	PPSN_CTX * ctx_ptr;
	
	if (fl_ctx == NULL)
	{
		return NULL;
	}
	
	ctx_ptr = (PPSN_CTX *)malloc(sizeof(PPSN_CTX));
	if (ctx_ptr == NULL)
	{
		return NULL;
	}

	memset(ctx_ptr, 0, sizeof(PPSN_CTX));

	ctx_ptr->fl_base = fl_ctx->fl_base;
	ctx_ptr->high_offset = fl_ctx->high_offset;	// + fl_ctx->fl_base;
	ctx_ptr->low_offset = fl_ctx->low_offset;	// + fl_ctx->fl_base;
	ctx_ptr->unit_size = fl_ctx->unit_size;

	if (bNeedMutex)
	{
		ctx_ptr->ctx_mutex = sys_os_create_mutex();
	}	
	else
	{
		ctx_ptr->ctx_mutex = 0;
	}
	
	return ctx_ptr;
}

BOOL pps_ctx_ul_init_nm(PPSN_CTX * fl_ctx, PPSN_CTX * ul_ctx)
{
	return pps_ctx_ul_init_assign(ul_ctx, fl_ctx, FALSE);
}

/***************************************************************************************/
void pps_ul_reinit(PPSN_CTX * ul_ctx)
{
	if (ul_ctx == NULL)
	{
		return;
	}
	
	ul_ctx->node_num = 0;
	ul_ctx->head_node = 0;
	ul_ctx->tail_node = 0;

	pps_wait_mutex(ul_ctx);
	pps_post_mutex(ul_ctx);

	if (ul_ctx->ctx_mutex)
	{
		sys_os_destroy_sig_mutx(ul_ctx->ctx_mutex);
	}	
}

void pps_ul_free(PPSN_CTX * ul_ctx)
{
	if (ul_ctx == NULL)
	{
		return;
	}
	
	if (ul_ctx->ctx_mutex)
	{
		sys_os_destroy_sig_mutx(ul_ctx->ctx_mutex);
	}

	free(ul_ctx);
}

BOOL pps_ctx_ul_del(PPSN_CTX * ul_ctx, void * content_ptr)
{
	PPSN * p_node;
	
	if (pps_used_node(ul_ctx, content_ptr) == FALSE)
	{
		return FALSE;
	}
	
	p_node = (PPSN *)(((char *)content_ptr) - sizeof(PPSN));

	pps_wait_mutex(ul_ctx);

	if (p_node->prev_node == 0)
	{
		ul_ctx->head_node = p_node->next_node;
	}	
	else
	{
		((PPSN *)(ul_ctx->fl_base + p_node->prev_node))->next_node = p_node->next_node;
	}
	
	if (p_node->next_node == 0)
	{
		ul_ctx->tail_node = p_node->prev_node;
	}	
	else
	{
		((PPSN *)(ul_ctx->fl_base + p_node->next_node))->prev_node = p_node->prev_node;
	}
	
	(ul_ctx->node_num)--;

	pps_post_mutex(ul_ctx);

	memset(p_node, 0, sizeof(PPSN));

	return TRUE;
}

PPSN * pps_ctx_ul_del_node_unlock(PPSN_CTX * ul_ctx, PPSN * p_node)
{
	if (p_node->node_flag != 2)
	{
		log_print(LOG_WARN, "%s, unit not in used list!!!\r\n", __FUNCTION__);
		return NULL;
	}

	if (ul_ctx->head_node == 0)
	{
		log_print(LOG_WARN, "%s, used list is empty!!!\r\n", __FUNCTION__);
		return NULL;
	}

	if (p_node->prev_node == 0)
	{
		ul_ctx->head_node = p_node->next_node;
	}	
	else
	{
		((PPSN *)(ul_ctx->fl_base + p_node->prev_node))->next_node = p_node->next_node;
	}
	
	if (p_node->next_node == 0)
	{
		ul_ctx->tail_node = p_node->prev_node;
	}	
	else
	{
		((PPSN *)(ul_ctx->fl_base + p_node->next_node))->prev_node = p_node->prev_node;
	}
	
	(ul_ctx->node_num)--;

	if (p_node->next_node == 0)
	{
		return NULL;
	}	
	else
	{
		return (PPSN *)(ul_ctx->fl_base + p_node->next_node);
	}	
}

void * pps_ctx_ul_del_unlock(PPSN_CTX * ul_ctx, void * content_ptr)
{
	PPSN * p_node;
	PPSN * p_ret;
	
	if (pps_used_node(ul_ctx, content_ptr) == FALSE)
	{
		return FALSE;
	}
	
	p_node = (PPSN *)(((char *)content_ptr) - sizeof(PPSN));

	p_ret = pps_ctx_ul_del_node_unlock(ul_ctx, p_node);
	if (p_ret == NULL)
	{
		return NULL;
	}	
	else
	{
		void * ret_ptr = (void *)(((char *)p_ret) + sizeof(PPSN));
		return ret_ptr;
	}
}

BOOL pps_ctx_ul_add(PPSN_CTX * ul_ctx, void * content_ptr)
{
	PPSN * p_node;
	uint32 offset;
	
	if (pps_safe_node(ul_ctx, content_ptr) == FALSE)
	{
		return FALSE;
	}
	
	p_node = (PPSN *)(((char *)content_ptr) - sizeof(PPSN));
	if (p_node->node_flag != 0)
	{
		return FALSE;
	}
	
	pps_wait_mutex(ul_ctx);

	p_node->next_node = 0;
	p_node->node_flag = 2;

	offset = (unsigned long)((char *)p_node - ul_ctx->fl_base);

	if (ul_ctx->tail_node == 0)
	{
		ul_ctx->tail_node = offset;
		ul_ctx->head_node = offset;
		p_node->prev_node = 0;
	}
	else
	{
		PPSN * p_tail = (PPSN *)(ul_ctx->fl_base + ul_ctx->tail_node);
		p_tail->next_node = offset;
		p_node->prev_node = ul_ctx->tail_node;
		ul_ctx->tail_node = offset;
	}

	(ul_ctx->node_num)++;

	pps_post_mutex(ul_ctx);

	return TRUE;
}

BOOL pps_ctx_ul_add_head(PPSN_CTX * ul_ctx, void * content_ptr)
{
	PPSN * p_node;
	uint32 offset;
	
	if (pps_safe_node(ul_ctx, content_ptr) == FALSE)
	{
		return FALSE;
	}
	
	p_node = (PPSN *)(((char *)content_ptr) - sizeof(PPSN));
	if(p_node->node_flag != 0)
	{
		return FALSE;
	}
	
	pps_wait_mutex(ul_ctx);

	offset = (unsigned long)((char *)p_node - ul_ctx->fl_base);
	p_node->node_flag = 2;
	p_node->prev_node = 0;

	if (ul_ctx->head_node == 0)
	{
		ul_ctx->tail_node = offset;
		ul_ctx->head_node = offset;
		p_node->next_node = 0;
	}
	else
	{
		PPSN * p_head = (PPSN *)(ul_ctx->fl_base + ul_ctx->head_node);
		p_head->prev_node = offset;
		p_node->next_node = ul_ctx->head_node;
		ul_ctx->head_node = offset;
	}

	(ul_ctx->node_num)++;

	pps_post_mutex(ul_ctx);

	return TRUE;
}


PPSN * _pps_node_head_start(PPSN_CTX * pps_ctx)
{
	if (pps_ctx == NULL)
	{
		return NULL;
	}
	
	pps_wait_mutex(pps_ctx);

	if (pps_ctx->head_node == 0)
	{
		return NULL;
	}	
	else
	{
		return (PPSN *)(pps_ctx->fl_base + pps_ctx->head_node);
	}	
}

PPSN * _pps_node_tail_start(PPSN_CTX * pps_ctx)
{
	if (pps_ctx == NULL)
	{
		return NULL;
	}
	
	pps_wait_mutex(pps_ctx);

	if (pps_ctx->tail_node == 0)
	{
		return NULL;
	}	
	else
	{
		return (PPSN *)(pps_ctx->fl_base + pps_ctx->tail_node);
	}	
}

PPSN * _pps_node_next(PPSN_CTX * pps_ctx, PPSN * p_node)
{
	char * ctx_ptr;
	
	if (pps_ctx == NULL || p_node == NULL)
	{
		return NULL;
	}
	
	ctx_ptr = ((char *)p_node) + sizeof(PPSN);

	if (ctx_ptr < (pps_ctx->fl_base + pps_ctx->low_offset) ||
		ctx_ptr > (pps_ctx->fl_base + pps_ctx->high_offset))
	{
		log_print(LOG_WARN, "%s, unit ptr error!!!!!!\r\n", __FUNCTION__);
		return NULL;
	}

	if (p_node->next_node == 0)
	{
		return NULL;
	}	
	else
	{
		return (PPSN *)(p_node->next_node + pps_ctx->fl_base);
	}	
}

PPSN * _pps_node_prev(PPSN_CTX * pps_ctx, PPSN * p_node)
{
	char * ctx_ptr;
	
	if (pps_ctx == NULL || p_node == NULL)
	{
		return NULL;
	}
	
	ctx_ptr = ((char *)p_node) + sizeof(PPSN);

	if (ctx_ptr < (pps_ctx->low_offset+pps_ctx->fl_base) ||
		ctx_ptr > (pps_ctx->high_offset+pps_ctx->fl_base))
	{
		log_print(LOG_WARN, "%s, unit ptr error!!!!!!\r\n", __FUNCTION__);	
		return NULL;
	}

	if (p_node->prev_node == 0)
	{
		return NULL;
	}	
	else
	{
		return (PPSN *)(pps_ctx->fl_base + p_node->prev_node);
	}	
}

void _pps_node_end(PPSN_CTX * pps_ctx)
{
	pps_post_mutex(pps_ctx);
}

void * pps_lookup_start(PPSN_CTX * pps_ctx)
{
	if (pps_ctx == NULL)
	{
		return NULL;
	}
	
	pps_wait_mutex(pps_ctx);

	if (pps_ctx->head_node)
	{
		void * ret_ptr = (void *)(pps_ctx->fl_base + pps_ctx->head_node + sizeof(PPSN));
		return ret_ptr;
	}

	return NULL;
}

void * pps_lookup_next(PPSN_CTX * pps_ctx, void * ctx_ptr)
{
	PPSN * p_node;
	
	if (pps_ctx == NULL || ctx_ptr == NULL)
	{
		return NULL;
	}
	
	if ((char *)ctx_ptr < (pps_ctx->fl_base + pps_ctx->low_offset) ||
		(char *)ctx_ptr > (pps_ctx->fl_base + pps_ctx->high_offset))
	{
		log_print(LOG_WARN, "%s, unit ptr error!!!\r\n", __FUNCTION__);
		return NULL;
	}

	p_node = (PPSN *)(((char *)ctx_ptr) - sizeof(PPSN));

	if (p_node->next_node == 0)
	{
		return NULL;
	}	
	else
	{
		void * ret_ptr = (void *)(pps_ctx->fl_base + p_node->next_node + sizeof(PPSN));
		return ret_ptr;
	}
}

void pps_lookup_end(PPSN_CTX * pps_ctx)
{
	pps_post_mutex(pps_ctx);
}

void * pps_lookback_start(PPSN_CTX * pps_ctx)
{
	if (pps_ctx == NULL) 
	{
		return NULL;
	}
	
	pps_wait_mutex(pps_ctx);

	if (pps_ctx->tail_node)
	{
		void * ret_ptr = (void *)(pps_ctx->tail_node + sizeof(PPSN)+pps_ctx->fl_base);
		return ret_ptr;
	}

	return NULL;
}

void * pps_lookback_next(PPSN_CTX * pps_ctx, void * ctx_ptr)
{
	PPSN * p_node;
	
	if (pps_ctx == NULL || ctx_ptr == NULL)
	{
		return NULL;
	}
	
	if ((char *)ctx_ptr < (pps_ctx->low_offset+pps_ctx->fl_base) ||
		(char *)ctx_ptr > (pps_ctx->high_offset+pps_ctx->fl_base))
	{
		log_print(LOG_WARN, "%s, unit ptr error!!!\r\n", __FUNCTION__);
		return NULL;
	}

	p_node = (PPSN *)(((char *)ctx_ptr) - sizeof(PPSN));

	if (p_node->prev_node == 0)
	{
		return NULL;
	}	
	else
	{
		void * ret_ptr = (void *)(p_node->prev_node + sizeof(PPSN)+pps_ctx->fl_base);
		return ret_ptr;
	}
}

void pps_lookback_end(PPSN_CTX * pps_ctx)
{
	pps_post_mutex(pps_ctx);
}

uint32 pps_get_index(PPSN_CTX * pps_ctx, void * content_ptr)
{
	uint32 index;
	uint32 offset;
	
	if (pps_ctx == NULL || content_ptr == NULL)
	{
		return 0xFFFFFFFF;
	}
	
	if ((char *)content_ptr < (pps_ctx->low_offset+pps_ctx->fl_base) ||
		(char *)content_ptr > (pps_ctx->high_offset+pps_ctx->fl_base))
	{
		log_print(LOG_WARN, "%s, unit ptr error!!!\r\n", __FUNCTION__);
		return 0xFFFFFFFF;
	}

	index = (uint32)((char *)content_ptr - pps_ctx->low_offset - pps_ctx->fl_base);
	offset = index % pps_ctx->unit_size;
	if (offset != 0)
	{
		index = index /pps_ctx->unit_size;

		log_print(LOG_WARN, "%s, unit ptr error,pps_ctx[0x%08x],ptr[0x%08x],low_offset[0x%08x],offset[0x%08x],like entry[%u]\r\n",
			__FUNCTION__, pps_ctx, content_ptr, pps_ctx->low_offset, offset,index);
		return 0xFFFFFFFF;
	}

	index = index / pps_ctx->unit_size;

	return index;
}

void * pps_get_node_by_index(PPSN_CTX * pps_ctx, unsigned long index)
{
	unsigned long content_offset;
	
	if (pps_ctx == NULL)
	{
		return NULL;
	}
	
	content_offset = pps_ctx->low_offset + index * pps_ctx->unit_size;

	if (content_offset > pps_ctx->high_offset)
	{
		if (index != 0xFFFFFFFF)
		{
			log_print(LOG_WARN, "%s, index [%u]error!!!\r\n", __FUNCTION__, index);
		}	
		return NULL;
	}

	return (void *)(content_offset + pps_ctx->fl_base);
}

void pps_wait_mutex(PPSN_CTX * pps_ctx)
{
	if (pps_ctx == NULL)
	{
		log_print(LOG_WARN, "%s, pps_ctx == NULL!!!\r\n", __FUNCTION__);
		return;
	}

	if (pps_ctx->ctx_mutex)
	{
		sys_os_mutex_enter (pps_ctx->ctx_mutex);
	}
}

void pps_post_mutex(PPSN_CTX * pps_ctx)
{
	if (pps_ctx == NULL)
	{
		log_print(LOG_WARN, "%s, pps_ctx == NULL!!!\r\n", __FUNCTION__);
		return;
	}

	if (pps_ctx->ctx_mutex)
	{
		sys_os_mutex_leave (pps_ctx->ctx_mutex);
	}
}

BOOL pps_safe_node(PPSN_CTX * pps_ctx, void * content_ptr)
{
	uint32 index;
	uint32 offset;
	
	if (pps_ctx == NULL || content_ptr == NULL)
	{
		return FALSE;
	}
	
	if ((char *)content_ptr < (pps_ctx->low_offset + pps_ctx->fl_base) ||
		(char *)content_ptr > (pps_ctx->high_offset + pps_ctx->fl_base))
	{
		// log_print(LOG_WARN, "%s, unit ptr error!!!\r\n", __FUNCTION__);
		return FALSE;
	}

	index = (unsigned long)((char *)content_ptr - pps_ctx->low_offset - pps_ctx->fl_base);
	offset = index % pps_ctx->unit_size;

	if (offset != 0)
	{
		index = index / pps_ctx->unit_size;

		log_print(LOG_WARN, "%s, unit ptr error,pps_ctx[0x%08x],ptr[0x%08x],low_offset[0x%08x],offset[0x%08x],like entry[%u]\r\n",
			__FUNCTION__, pps_ctx, content_ptr, pps_ctx->low_offset, offset, index);
		return FALSE;
	}

	return TRUE;
}

BOOL pps_idle_node(PPSN_CTX * pps_ctx, void * content_ptr)
{
	PPSN * p_node;
	
	if (pps_safe_node(pps_ctx, content_ptr) == FALSE)
	{
		return FALSE;
	}
	
	p_node = (PPSN *)(((char *)content_ptr) - sizeof(PPSN));
	
	return (p_node->node_flag == 1);
}

BOOL pps_exist_node(PPSN_CTX * pps_ctx, void * content_ptr)
{
	PPSN * p_node;
	
	if (pps_safe_node(pps_ctx, content_ptr) == FALSE)
	{
		return FALSE;
	}
	
	p_node = (PPSN *)(((char *)content_ptr) - sizeof(PPSN));
	
	return (p_node->node_flag != 1);
}

BOOL pps_used_node(PPSN_CTX * pps_ctx, void * content_ptr)
{
	PPSN * p_node;
	
	if (pps_safe_node(pps_ctx, content_ptr) == FALSE)
	{
		return FALSE;
	}
	
	if (pps_ctx->head_node == 0)	
	{
		log_print(LOG_WARN, "%s, used list is empty!!!\r\n", __FUNCTION__);
		return FALSE;
	}

	p_node = (PPSN *)(((char *)content_ptr) - sizeof(PPSN));
	
	return (p_node->node_flag == 2);
}

void * _pps_node_get_data(PPSN * p_node)
{
	if (p_node == NULL)
	{
		return NULL;
	}
	
	return (void *)(((char *)p_node) + sizeof(PPSN));
}

PPSN * _pps_data_get_node(void * p_data)
{
	PPSN * p_node;
	
	if (p_data == NULL)
	{
		return NULL;
	}
	
	p_node = (PPSN *)(((char *)p_data) - sizeof(PPSN));
	
	return p_node;
}

int pps_node_count(PPSN_CTX * pps_ctx)
{
    if (pps_ctx == NULL)
    {
    	return 0;
    }
    
    return pps_ctx->node_num;
}

void * pps_get_head_node(PPSN_CTX * pps_ctx)
{
    if (pps_ctx == NULL) 
    {
    	return NULL;
	}
	
	//pps_wait_mutex(pps_ctx);

	if (pps_ctx->head_node)
	{
		void * ret_ptr = (void *)(pps_ctx->head_node + sizeof(PPSN));
		return ret_ptr;
	}

	return NULL;
}

void * pps_get_tail_node(PPSN_CTX * pps_ctx)
{
    if (pps_ctx == NULL)
    {
    	return NULL;
	}
	
	//pps_wait_mutex(pps_ctx);

	if (pps_ctx->tail_node)
	{
		void * ret_ptr = (void *)(pps_ctx->tail_node + sizeof(PPSN));
		return ret_ptr;
	}

	return NULL;
}

void * pps_get_next_node(PPSN_CTX * pps_ctx, void * content_ptr)
{
    PPSN * p_node = _pps_data_get_node(content_ptr);
    p_node = _pps_node_next(pps_ctx, p_node);
    return _pps_node_get_data(p_node);
}

void * pps_get_prev_node(PPSN_CTX * pps_ctx, void * content_ptr)
{
    PPSN * p_node = _pps_data_get_node(content_ptr);
    p_node = _pps_node_prev(pps_ctx, p_node);
    return _pps_node_get_data(p_node);
}


