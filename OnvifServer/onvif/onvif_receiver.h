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

#ifndef ONVIF_RECEIVER_H
#define ONVIF_RECEIVER_H

#include "sys_inc.h"
#include "onvif_cm.h"

typedef struct
{
    ONVIF_Receiver * Receivers;                         // required, A list of all receivers that currently exist on the device
} trv_GetReceivers_RES;

typedef struct
{
    char    ReceiverToken[ONVIF_TOKEN_LEN];             // required, The token of the receiver to be retrieved
} trv_GetReceiver_REQ;

typedef struct
{
    onvif_Receiver  Receiver;                           // required, The details of the receiver
} trv_GetReceiver_RES;

typedef struct 
{
	onvif_ReceiverConfiguration Configuration;	        // required, The initial configuration for the new receiver
} trv_CreateReceiver_REQ;

typedef struct 
{
	onvif_Receiver  Receiver;	                        // required, The details of the receiver that was created
} trv_CreateReceiver_RES;

typedef struct 
{
	char    ReceiverToken[ONVIF_TOKEN_LEN];	            // required, The token of the receiver to be deleted
} trv_DeleteReceiver_REQ;

typedef struct 
{
	char    ReceiverToken[ONVIF_TOKEN_LEN];	            // required, The token of the receiver to be configured
	
	onvif_ReceiverConfiguration Configuration;	        // required, The new configuration for the receiver
} trv_ConfigureReceiver_REQ;

typedef struct 
{
	char    ReceiverToken[ONVIF_TOKEN_LEN];	            // required, The token of the receiver to be changed
	
	onvif_ReceiverMode Mode;	                        // required, The new receiver mode
} trv_SetReceiverMode_REQ;

typedef struct 
{
	char    ReceiverToken[ONVIF_TOKEN_LEN];	            // required, The token of the receiver to be queried
} trv_GetReceiverState_REQ;

typedef struct 
{
	onvif_ReceiverStateInformation  ReceiverState;	    // required, Description of the current receiver state
} trv_GetReceiverState_RES;


#ifdef __cplusplus
extern "C" {
#endif

ONVIF_RET onvif_trv_GetReceivers(trv_GetReceivers_RES * p_res);
ONVIF_RET onvif_trv_GetReceiver(trv_GetReceiver_REQ * p_req, trv_GetReceiver_RES * p_res);
ONVIF_RET onvif_trv_CreateReceiver(trv_CreateReceiver_REQ * p_req, trv_CreateReceiver_RES * p_res);
ONVIF_RET onvif_trv_DeleteReceiver(trv_DeleteReceiver_REQ * p_req);
ONVIF_RET onvif_trv_ConfigureReceiver(trv_ConfigureReceiver_REQ * p_req);
ONVIF_RET onvif_trv_SetReceiverMode(trv_SetReceiverMode_REQ * p_req);
ONVIF_RET onvif_trv_GetReceiverState(trv_GetReceiverState_REQ * p_req, trv_GetReceiverState_RES * p_res);

#ifdef __cplusplus
}
#endif

#endif // end of ONVIF_RECEIVER_H



