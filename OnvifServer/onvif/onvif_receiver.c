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
#include "onvif_receiver.h"
#include "onvif.h"
#include "onvif_event.h"
    

#ifdef RECEIVER_SUPPORT
    
/********************************************************************************/
extern ONVIF_CFG g_onvif_cfg;
extern ONVIF_CLS g_onvif_cls;

/********************************************************************************/

void onvif_ReceiverChangeStateNotify(ONVIF_Receiver * p_req)
{
    ONVIF_NotificationMessage * p_message = onvif_add_NotificationMessage(NULL);
	if (p_message)
	{
	    ONVIF_SimpleItem * p_simpleitem;
	    
		strcpy(p_message->NotificationMessage.Dialect, "http://www.onvif.org/ver10/tev/topicExpression/ConcreteSet");
		strcpy(p_message->NotificationMessage.Topic, "tns1:Receiver/ChangeState");
		p_message->NotificationMessage.Message.PropertyOperationFlag = 1;
		p_message->NotificationMessage.Message.PropertyOperation = PropertyOperation_Changed;
		p_message->NotificationMessage.Message.UtcTime = time(NULL)+1;

        p_message->NotificationMessage.Message.SourceFlag = 1;
        
		p_simpleitem = onvif_add_SimpleItem(&p_message->NotificationMessage.Message.Source.SimpleItem);
		if (p_simpleitem)
		{		    
			strcpy(p_simpleitem->SimpleItem.Name, "ReceiverToken");
			strcpy(p_simpleitem->SimpleItem.Value, p_req->Receiver.Token);
		}

		p_message->NotificationMessage.Message.DataFlag = 1;

		p_simpleitem = onvif_add_SimpleItem(&p_message->NotificationMessage.Message.Data.SimpleItem);
		if (p_simpleitem)
		{		    
			strcpy(p_simpleitem->SimpleItem.Name, "NewState");
			strcpy(p_simpleitem->SimpleItem.Value, onvif_ReceiverStateToString(p_req->StateInformation.State));
		}

		p_simpleitem = onvif_add_SimpleItem(&p_message->NotificationMessage.Message.Data.SimpleItem);
		if (p_simpleitem)
		{		    
			strcpy(p_simpleitem->SimpleItem.Name, "MediaUri");
			strcpy(p_simpleitem->SimpleItem.Value, p_req->Receiver.Configuration.MediaUri);
		}

		onvif_put_NotificationMessage(p_message);
	}
}

void onvif_ReceiverConnectionFailedNotify(ONVIF_Receiver * p_req)
{
    ONVIF_NotificationMessage * p_message = onvif_add_NotificationMessage(NULL);
	if (p_message)
	{
	    ONVIF_SimpleItem * p_simpleitem;
	    
		strcpy(p_message->NotificationMessage.Dialect, "http://www.onvif.org/ver10/tev/topicExpression/ConcreteSet");
		strcpy(p_message->NotificationMessage.Topic, "tns1:Receiver/ConnectionFailed");
		p_message->NotificationMessage.Message.PropertyOperationFlag = 1;
		p_message->NotificationMessage.Message.PropertyOperation = PropertyOperation_Changed;
		p_message->NotificationMessage.Message.UtcTime = time(NULL)+1;

        p_message->NotificationMessage.Message.SourceFlag = 1;
        
		p_simpleitem = onvif_add_SimpleItem(&p_message->NotificationMessage.Message.Source.SimpleItem);
		if (p_simpleitem)
		{		    
			strcpy(p_simpleitem->SimpleItem.Name, "ReceiverToken");
			strcpy(p_simpleitem->SimpleItem.Value, p_req->Receiver.Token);
		}

		p_message->NotificationMessage.Message.DataFlag = 1;

		p_simpleitem = onvif_add_SimpleItem(&p_message->NotificationMessage.Message.Data.SimpleItem);
		if (p_simpleitem)
		{		    
			strcpy(p_simpleitem->SimpleItem.Name, "MediaUri");
			strcpy(p_simpleitem->SimpleItem.Value, p_req->Receiver.Configuration.MediaUri);
		}

		onvif_put_NotificationMessage(p_message);
	}
}


ONVIF_RET onvif_trv_GetReceivers(trv_GetReceivers_RES * p_res)
{
    p_res->Receivers = g_onvif_cfg.receiver;

    return ONVIF_OK;
}

/**
 * The possible return values:
 *  ONVIF_ERR_UnknownToken
 **/
ONVIF_RET onvif_trv_GetReceiver(trv_GetReceiver_REQ * p_req, trv_GetReceiver_RES * p_res)
{
    ONVIF_Receiver * p_receiver = onvif_find_Receiver(p_req->ReceiverToken);
    if (NULL == p_receiver)
    {
        return ONVIF_ERR_NotFound;
    }

    memcpy(&p_res->Receiver, &p_receiver->Receiver, sizeof(onvif_Receiver));

    return ONVIF_OK;
}

/**
 * The possible return values:
 *  ONVIF_ERR_BadConfiguration
 *  ONVIF_ERR_MaxReceivers
 **/
ONVIF_RET onvif_trv_CreateReceiver(trv_CreateReceiver_REQ * p_req, trv_CreateReceiver_RES * p_res)
{
    int nums;
    ONVIF_Receiver * p_receiver;

    nums = onvif_get_Receiver_nums();
    if (nums >= g_onvif_cfg.Capabilities.receiver.SupportedReceivers)
    {
        return ONVIF_ERR_MaxReceivers;
    }

    p_receiver = onvif_add_Receiver();
    if (NULL == p_receiver)
    {
        return ONVIF_ERR_MaxReceivers;
    }

    sprintf(p_receiver->Receiver.Token, "ReceiverToken%d", g_onvif_cls.receiver_idx++);
    memcpy(&p_receiver->Receiver.Configuration, &p_req->Configuration, sizeof(onvif_ReceiverConfiguration));

    memcpy(&p_res->Receiver, &p_receiver->Receiver, sizeof(onvif_Receiver));

    return ONVIF_OK;
}

/**
 * The possible return values:
 *  ONVIF_ERR_UnknownToken
 *  ONVIF_ERR_CannotDeleteReceiver
 **/
ONVIF_RET onvif_trv_DeleteReceiver(trv_DeleteReceiver_REQ * p_req)
{
    ONVIF_Receiver * p_receiver = onvif_find_Receiver(p_req->ReceiverToken);
    if (NULL == p_receiver)
    {
        return ONVIF_ERR_UnknownToken;
    }

    onvif_free_Receiver(p_receiver);

    return ONVIF_OK;
}

/**
 * The possible return values:
 *  ONVIF_ERR_UnknownToken
 *  ONVIF_ERR_BadConfiguration
 **/
ONVIF_RET onvif_trv_ConfigureReceiver(trv_ConfigureReceiver_REQ * p_req)
{
    onvif_ReceiverState State;
    ONVIF_Receiver * p_receiver = onvif_find_Receiver(p_req->ReceiverToken);
    if (NULL == p_receiver)
    {
        return ONVIF_ERR_UnknownToken;
    }
    
    memcpy(&p_receiver->Receiver.Configuration, &p_req->Configuration, sizeof(onvif_ReceiverConfiguration));

    // todo : here add your handler code ...


    State = p_receiver->StateInformation.State;
    
    if (ReceiverMode_AlwaysConnect == p_receiver->Receiver.Configuration.Mode)
    {
        p_receiver->StateInformation.State = ReceiverState_Connected;
        p_receiver->StateInformation.AutoCreated = FALSE;
    }
    else if (ReceiverMode_AutoConnect == p_receiver->Receiver.Configuration.Mode)
    {
        p_receiver->StateInformation.State = ReceiverState_Connected;
        p_receiver->StateInformation.AutoCreated = TRUE;
    }
    else 
    {
        p_receiver->StateInformation.State = ReceiverState_NotConnected;
        p_receiver->StateInformation.AutoCreated = FALSE;
    }

    if (State != p_receiver->StateInformation.State)
    {
        onvif_ReceiverChangeStateNotify(p_receiver);
    }    

    return ONVIF_OK;
}

/**
 * The possible return values:
 *  ONVIF_ERR_UnknownToken
 **/
ONVIF_RET onvif_trv_SetReceiverMode(trv_SetReceiverMode_REQ * p_req)
{
    onvif_ReceiverState State;
    ONVIF_Receiver * p_receiver = onvif_find_Receiver(p_req->ReceiverToken);
    if (NULL == p_receiver)
    {
        return ONVIF_ERR_UnknownToken;
    }

    p_receiver->Receiver.Configuration.Mode = p_req->Mode;

    // todo : here add your handler code ...


    State = p_receiver->StateInformation.State;
    
    if (ReceiverMode_AlwaysConnect == p_receiver->Receiver.Configuration.Mode)
    {
        p_receiver->StateInformation.State = ReceiverState_Connected;
        p_receiver->StateInformation.AutoCreated = FALSE;
    }
    else if (ReceiverMode_AutoConnect == p_receiver->Receiver.Configuration.Mode)
    {
        p_receiver->StateInformation.State = ReceiverState_Connected;
        p_receiver->StateInformation.AutoCreated = TRUE;
    }
    else 
    {
        p_receiver->StateInformation.State = ReceiverState_NotConnected;
        p_receiver->StateInformation.AutoCreated = FALSE;
    }

    if (State != p_receiver->StateInformation.State)
    {
        onvif_ReceiverChangeStateNotify(p_receiver);
    }   
    
    return ONVIF_OK;
}

/**
 * The possible return values:
 *  ONVIF_ERR_UnknownToken
 **/
ONVIF_RET onvif_trv_GetReceiverState(trv_GetReceiverState_REQ * p_req, trv_GetReceiverState_RES * p_res)
{
    ONVIF_Receiver * p_receiver = onvif_find_Receiver(p_req->ReceiverToken);
    if (NULL == p_receiver)
    {
        return ONVIF_ERR_UnknownToken;
    }

    memcpy(&p_res->ReceiverState, &p_receiver->StateInformation, sizeof(onvif_ReceiverStateInformation));

    return ONVIF_OK;
}

#endif // end of RECEIVER_SUPPORT



