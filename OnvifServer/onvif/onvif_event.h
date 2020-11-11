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

#ifndef ONVIF_EVENT_H
#define ONVIF_EVENT_H


/***************************************************************************************/
#define MAX_NUM_EUA			    200
#define MAX_FILTER_NUMS         10

/***************************************************************************************/

typedef struct _ONVIF_FILTER
{
	char    TopicExpression[MAX_FILTER_NUMS][256];
    char    MessageContent[MAX_FILTER_NUMS][256];
} ONVIF_FILTER;

typedef struct _ONVIF_EVENT_AGENT
{
	uint32  FiltersFlag : 1;
	uint32  used        : 1;
	uint32  reserver    : 30;
	
	BOOL	pollMode;								    // ture - poll mode, false - notify mode
	
	char 	consumer_addr[256];

	char	host[32];
	char	url[256];

	int		port;
	int		https;
	
	char 	producter_addr[256];
	int  	init_term_time;
	
	time_t	subscibe_time;
	time_t	last_renew_time;

	ONVIF_FILTER Filters;

	LINKED_LIST* msg_list;                              // for poll mode, save notify message to list
} EUA;

typedef struct
{
    uint32  FiltersFlag                 : 1;
	uint32	InitialTerminationTimeFlag	: 1;			// Indicates whether the field InitialTerminationTime is valid
	uint32 	Reserved					: 30;
	
	char  	ConsumerReference[256];						// required
	int   	InitialTerminationTime;						// optional

    ONVIF_FILTER Filters;
    
	EUA   * p_eua;
} Subscribe_REQ;

typedef struct
{
    int     TerminationTimeType;                        // TerminationTime type, 0 - datetime, 1 - duration
    
	time_t 	TerminationTime;							// required
	char  	ProducterReference[256];					// required
} Renew_REQ;

typedef struct
{
    uint32  FiltersFlag                 : 1;
	uint32	InitialTerminationTimeFlag	: 1;			// Indicates whether the field InitialTerminationTime is valid
	uint32 	Reserved					: 30;
	
	ONVIF_FILTER Filters;
	
	int		InitialTerminationTime;						// optional, Initial termination time

	EUA   * p_eua;
} CreatePullPointSubscription_REQ;

typedef struct
{
	int		eua_idx;									// required
	
	int		Timeout;									// required, Maximum time to block until this method returns
	int 	MessageLimit;								// required, Upper limit for the number of messages to return at once. A server implementation may decide to return less messages
} PullMessages_REQ;




#ifdef __cplusplus
extern "C" {
#endif


/***************************************************************************************/
void 	  onvif_eua_init();
void 	  onvif_eua_deinit();
EUA     * onvif_get_idle_eua();
void 	  onvif_set_idle_eua(EUA * p_eua);
uint32 	  onvif_get_eua_index(EUA * p_eua);
EUA     * onvif_get_eua_by_index(uint32 index);
EUA     * onvif_eua_lookup_by_addr(const char * addr);
void      onvif_free_used_eua(EUA * p_eua);

BOOL 	  onvif_put_NotificationMessage(ONVIF_NotificationMessage * p_message);

/***************************************************************************************/
ONVIF_RET onvif_Subscribe(const char * lip, uint32 lport, Subscribe_REQ * p_req);
ONVIF_RET onvif_Renew(Renew_REQ * p_req);
ONVIF_RET onvif_Unsubscribe(const char * addr);
ONVIF_RET onvif_CreatePullPointSubscription(const char * lip, uint32 lport, CreatePullPointSubscription_REQ * p_req);
ONVIF_RET onvif_tev_SetSynchronizationPoint();
ONVIF_RET onvif_PullMessages(PullMessages_REQ * p_req);
/***************************************************************************************/
void      onvif_notify(EUA * p_eua, ONVIF_NotificationMessage * p_message);
BOOL      onvif_event_filter(ONVIF_NotificationMessage * p_message, EUA * p_eua);

ONVIF_NotificationMessage * onvif_init_NotificationMessage();
ONVIF_NotificationMessage * onvif_init_NotificationMessage1();
ONVIF_NotificationMessage * onvif_init_NotificationMessage2();
ONVIF_NotificationMessage * onvif_init_NotificationMessage3(const char * topic);


#ifdef __cplusplus
}
#endif


#endif


