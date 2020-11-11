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

#ifndef	HQUEUE_H
#define	HQUEUE_H


/***********************************************************/
#define	HQ_PUT_WAIT		0x00000001
#define	HQ_GET_WAIT		0x00000002
#define	HQ_NO_EVENT		0x00000004

/***********************************************************/
typedef struct h_queue
{
	uint32	    queue_mode;
	uint32	    unit_num;
	uint32	    unit_size;
	uint32	    front;
	uint32	    rear;
	uint32	    queue_buffer;
	uint32	    count_put_full;

	void *      queue_putMutex;	
	void *      queue_nnulEvent;
	void *      queue_nfulEvent;
} HQUEUE;


#ifdef __cplusplus
extern "C" {
#endif

/***********************************************************/
HQUEUE * hqCreate(uint32 unit_num, uint32 unit_size, uint32 queue_mode);
void     hqDelete(HQUEUE * phq);

BOOL 	 hqBufPut(HQUEUE * phq, char * buf);
BOOL 	 hqBufGet(HQUEUE * phq, char * buf);

BOOL 	 hqBufIsEmpty(HQUEUE * phq);
BOOL 	 hqBufIsFull(HQUEUE * phq);

char   * hqBufGetWait(HQUEUE * phq);
void 	 hqBufGetWaitPost(HQUEUE * phq);

char   * hqBufPutPtrWait(HQUEUE * phq);
void     hqBufPutPtrWaitPost(HQUEUE * phq, BOOL bPutFinish);
BOOL 	 hqBufPeek(HQUEUE * phq, char * buf);

#ifdef __cplusplus
}
#endif

#endif



