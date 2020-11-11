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

#include "onvif_doorcontrol.h"
#include "onvif.h"
#include "onvif_event.h"

#ifdef PROFILE_C_SUPPORT

extern ONVIF_CFG g_onvif_cfg;

/***************************************************************************************/

void onvif_AccessPointStateEnabledChangedNotify(ONVIF_AccessPoint * p_accesspoint)
{
    ONVIF_NotificationMessage * p_message = onvif_add_NotificationMessage(NULL);
	if (p_message)
	{
	    ONVIF_SimpleItem * p_simpleitem;
	    
		strcpy(p_message->NotificationMessage.Dialect, "http://www.onvif.org/ver10/tev/topicExpression/ConcreteSet");
		strcpy(p_message->NotificationMessage.Topic, "tns1:AccessPoint/State/Enabled");
		p_message->NotificationMessage.Message.PropertyOperationFlag = 1;
		p_message->NotificationMessage.Message.PropertyOperation = PropertyOperation_Changed;
		p_message->NotificationMessage.Message.UtcTime = time(NULL)+1;

        p_message->NotificationMessage.Message.SourceFlag = 1;
        
		p_simpleitem = onvif_add_SimpleItem(&p_message->NotificationMessage.Message.Source.SimpleItem);
		if (p_simpleitem)
		{		    
			strcpy(p_simpleitem->SimpleItem.Name, "AccessPointToken");
			strcpy(p_simpleitem->SimpleItem.Value, p_accesspoint->AccessPointInfo.token);
		}

        p_message->NotificationMessage.Message.DataFlag = 1;
        
		p_simpleitem = onvif_add_SimpleItem(&p_message->NotificationMessage.Message.Data.SimpleItem);
		if (p_simpleitem)
		{		    
			strcpy(p_simpleitem->SimpleItem.Name, "State");
			sprintf(p_simpleitem->SimpleItem.Value, "%s", p_accesspoint->Enabled ? "true" : "false");
		}

		onvif_put_NotificationMessage(p_message);
	}
}

/***************************************************************************************/

/**
 * GetAccessPointInfoList 
 *
 * @param p_req the request parameter
 * @param p_res the response parameter
 * @return ONVIF_OK or ONVIF_ERR_InvalidStartReference
 */
ONVIF_RET onvif_tac_GetAccessPointInfoList(tac_GetAccessPointInfoList_REQ * p_req, tac_GetAccessPointInfoList_RES * p_res)
{
    int nums = 0;
    ONVIF_AccessPoint * p_accesspoint = g_onvif_cfg.access_points;
    
    if (p_req->StartReferenceFlag)
    {
        p_accesspoint = onvif_find_AccessPoint(p_req->StartReference);
        if (NULL == p_accesspoint)
        {
            return ONVIF_ERR_InvalidStartReference;
        }
    }

    while (p_accesspoint)
    {
        ONVIF_AccessPoint * p_accesspoint1 = onvif_add_AccessPoint(&p_res->AccessPointInfo);
        if (p_accesspoint1)
        {
            memcpy(&p_accesspoint1->AccessPointInfo, &p_accesspoint->AccessPointInfo, sizeof(onvif_AccessPointInfo));
        }
        
        p_accesspoint = p_accesspoint->next;

        nums++;
        if (p_req->LimitFlag && nums >= p_req->Limit)
        {
            break;
        }
    }

    if (p_accesspoint)
    {
        p_res->NextStartReferenceFlag = 1;
        strcpy(p_res->NextStartReference, p_accesspoint->AccessPointInfo.token);
    }    
    
    return ONVIF_OK;
}

/**
 * GetAreaInfoList 
 *
 * @param p_req the request parameter
 * @param p_res the response parameter
 * @return ONVIF_OK or ONVIF_ERR_InvalidStartReference
 */
ONVIF_RET onvif_tac_GetAreaInfoList(tac_GetAreaInfoList_REQ * p_req, tac_GetAreaInfoList_RES * p_res)
{
    int nums = 0;
    ONVIF_AreaInfo * p_area = g_onvif_cfg.area_info;
    
    if (p_req->StartReferenceFlag)
    {
        p_area = onvif_find_AreaInfo(p_req->StartReference);
        if (NULL == p_area)
        {
            return ONVIF_ERR_InvalidStartReference;
        }
    }

    while (p_area)
    {
        ONVIF_AreaInfo * p_info = onvif_add_AreaInfo(&p_res->AreaInfo);
        if (p_info)
        {
            memcpy(&p_info->AreaInfo, &p_area->AreaInfo, sizeof(onvif_AreaInfo));
        }
        
        p_area = p_area->next;

        nums++;
        if (p_req->LimitFlag && nums >= p_req->Limit)
        {
            break;
        }
    }

    if (p_area)
    {
        p_res->NextStartReferenceFlag = 1;
        strcpy(p_res->NextStartReference, p_area->AreaInfo.token);
    }    
    
    return ONVIF_OK;
}

/**
 * EnableAccessPoint 
 *
 * @param p_req the request parameter
 * @return ONVIF_OK or ONVIF_ERR_NotFound or ONVIF_ERR_ONVIF_ERR_NotSupported
 */
ONVIF_RET onvif_tac_EnableAccessPoint(tac_EnableAccessPoint_REQ * p_req)
{
    ONVIF_AccessPoint * p_accesspoint = onvif_find_AccessPoint(p_req->Token);
    if (NULL == p_accesspoint)
    {
        return ONVIF_ERR_NotFound;
    }

    p_accesspoint->Enabled = TRUE;

    // send event topic tns1:AccessPoint/State/Enabled notify
    onvif_AccessPointStateEnabledChangedNotify(p_accesspoint);	

    return ONVIF_OK;
}

/**
 * DisableAccessPoint 
 *
 * @param p_req the request parameter
 * @return ONVIF_OK or ONVIF_ERR_NotFound or ONVIF_ERR_ONVIF_ERR_NotSupported
 */
ONVIF_RET onvif_tac_DisableAccessPoint(tac_DisableAccessPoint_REQ * p_req)
{
    ONVIF_AccessPoint * p_accesspoint = onvif_find_AccessPoint(p_req->Token);
    if (NULL == p_accesspoint)
    {
        return ONVIF_ERR_NotFound;
    }

    p_accesspoint->Enabled = FALSE;

    // send event topic tns1:AccessPoint/State/Enabled notify
    onvif_AccessPointStateEnabledChangedNotify(p_accesspoint);	
    
    return ONVIF_OK;
}


/**
 * GetDoorInfoList 
 *
 * @param p_req the request parameter
 * @param p_res the response parameter
 * @return ONVIF_OK or ONVIF_ERR_InvalidStartReference
 */
ONVIF_RET onvif_tdc_GetDoorInfoList(tdc_GetDoorInfoList_REQ * p_req, tdc_GetDoorInfoList_RES * p_res)
{
    int nums = 0;
    ONVIF_Door * p_door = g_onvif_cfg.doors;
    
    if (p_req->StartReferenceFlag)
    {
        p_door = onvif_find_Door(p_req->StartReference);
        if (NULL == p_door)
        {
            return ONVIF_ERR_InvalidStartReference;
        }
    }

    while (p_door)
    {
        ONVIF_Door * p_info = onvif_add_Door(&p_res->DoorInfo);
        if (p_info)
        {
            memcpy(&p_info->DoorInfo, &p_door->DoorInfo, sizeof(onvif_DoorInfo));
        }
        
        p_door = p_door->next;

        nums++;
        if (p_req->LimitFlag && nums >= p_req->Limit)
        {
            break;
        }
    }

    if (p_door)
    {
        p_res->NextStartReferenceFlag = 1;
        strcpy(p_res->NextStartReference, p_door->DoorInfo.token);
    }    
    
    return ONVIF_OK;
}


/**
 * AccessDoor 
 *
 * @param p_req the request parameter
 * @return ONVIF_OK or ONVIF_ERR_NotFound or ONVIF_ERR_Failure
 */
ONVIF_RET onvif_tdc_AccessDoor(tdc_AccessDoor_REQ * p_req)
{
    ONVIF_Door * p_door = onvif_find_Door(p_req->Token);
    if (NULL == p_door)
    {
        return ONVIF_ERR_NotFound;
    }

    if (p_door->DoorInfo.Capabilities.Access == FALSE)
    {
        return ONVIF_ERR_Failure;
    }
    
    // todo : do access door operation ...

    // todo : if the door state changed, send event notify, please refer onvif_AccessPointStateEnabledChangedNotify
    
    return ONVIF_OK;
}

/**
 * LockDoor 
 *
 * @param p_req the request parameter
 * @return ONVIF_OK or ONVIF_ERR_NotFound or ONVIF_ERR_Failure
 */
ONVIF_RET onvif_tdc_LockDoor(tdc_LockDoor_REQ * p_req)
{

    ONVIF_Door * p_door = onvif_find_Door(p_req->Token);
    if (NULL == p_door)
    {
        return ONVIF_ERR_NotFound;
    }

    if (p_door->DoorInfo.Capabilities.Lock == FALSE)
    {
        return ONVIF_ERR_Failure;
    }
    
    // todo : do lock door operation ...

    // todo : if the door state changed, send event notify, please refer onvif_AccessPointStateEnabledChangedNotify
    
    return ONVIF_OK;
}

/**
 * LockDoor 
 *
 * @param p_req the request parameter
 * @return ONVIF_OK or ONVIF_ERR_NotFound or ONVIF_ERR_Failure
 */
ONVIF_RET onvif_tdc_UnlockDoor(tdc_UnlockDoor_REQ * p_req)
{
    ONVIF_Door * p_door = onvif_find_Door(p_req->Token);
    if (NULL == p_door)
    {
        return ONVIF_ERR_NotFound;
    }

    if (p_door->DoorInfo.Capabilities.Unlock == FALSE)
    {
        return ONVIF_ERR_Failure;
    }
    
    // todo : do unlock door operation ...

    // todo : if the door state changed, send event notify, please refer onvif_AccessPointStateEnabledChangedNotify
    
    return ONVIF_OK;
}

/**
 * DoubleLockDoor 
 *
 * @param p_req the request parameter
 * @return ONVIF_OK or ONVIF_ERR_NotFound or ONVIF_ERR_Failure
 */
ONVIF_RET onvif_tdc_DoubleLockDoor(tdc_DoubleLockDoor_REQ * p_req)
{
    ONVIF_Door * p_door = onvif_find_Door(p_req->Token);
    if (NULL == p_door)
    {
        return ONVIF_ERR_NotFound;
    }

    if (p_door->DoorInfo.Capabilities.DoubleLock == FALSE)
    {
        return ONVIF_ERR_Failure;
    }
    
    // todo : do double lock door operation ...

    // todo : if the door state changed, send event notify, please refer onvif_AccessPointStateEnabledChangedNotify
    
    return ONVIF_OK;
}

/**
 * BlockDoor 
 *
 * @param p_req the request parameter
 * @return ONVIF_OK or ONVIF_ERR_NotFound or ONVIF_ERR_Failure
 */
ONVIF_RET onvif_tdc_BlockDoor(tdc_BlockDoor_REQ * p_req)
{
    ONVIF_Door * p_door = onvif_find_Door(p_req->Token);
    if (NULL == p_door)
    {
        return ONVIF_ERR_NotFound;
    }

    if (p_door->DoorInfo.Capabilities.Block == FALSE)
    {
        return ONVIF_ERR_Failure;
    }
    
    // todo : do block door operation ...

    // todo : if the door state changed, send event notify, please refer onvif_AccessPointStateEnabledChangedNotify
    
    return ONVIF_OK;
}

/**
 * LockDownDoor 
 *
 * @param p_req the request parameter
 * @return ONVIF_OK or ONVIF_ERR_NotFound or ONVIF_ERR_Failure
 */
ONVIF_RET onvif_tdc_LockDownDoor(tdc_LockDownDoor_REQ * p_req)
{
    ONVIF_Door * p_door = onvif_find_Door(p_req->Token);
    if (NULL == p_door)
    {
        return ONVIF_ERR_NotFound;
    }

    if (p_door->DoorInfo.Capabilities.LockDown == FALSE)
    {
        return ONVIF_ERR_Failure;
    }
    
    // todo : do lock down door operation ...

    // todo : if the door state changed, send event notify, please refer onvif_AccessPointStateEnabledChangedNotify
    
    return ONVIF_OK;
}

/**
 * LockDownReleaseDoor 
 *
 * @param p_req the request parameter
 * @return ONVIF_OK or ONVIF_ERR_NotFound or ONVIF_ERR_Failure
 */
ONVIF_RET onvif_tdc_LockDownReleaseDoor(tdc_LockDownReleaseDoor_REQ * p_req)
{
    ONVIF_Door * p_door = onvif_find_Door(p_req->Token);
    if (NULL == p_door)
    {
        return ONVIF_ERR_NotFound;
    }

    if (p_door->DoorInfo.Capabilities.LockDown == FALSE)
    {
        return ONVIF_ERR_Failure;
    }
    
    // todo : do lock down release door operation ...

    // todo : if the door state changed, send event notify, please refer onvif_AccessPointStateEnabledChangedNotify
    
    return ONVIF_OK;
}

/**
 * LockOpenDoor 
 *
 * @param p_req the request parameter
 * @return ONVIF_OK or ONVIF_ERR_NotFound or ONVIF_ERR_Failure
 */
ONVIF_RET onvif_tdc_LockOpenDoor(tdc_LockOpenDoor_REQ * p_req)
{
    ONVIF_Door * p_door = onvif_find_Door(p_req->Token);
    if (NULL == p_door)
    {
        return ONVIF_ERR_NotFound;
    }

    if (p_door->DoorInfo.Capabilities.LockOpen == FALSE)
    {
        return ONVIF_ERR_Failure;
    }
    
    // todo : do lock open door operation ...

    // todo : if the door state changed, send event notify, please refer onvif_AccessPointStateEnabledChangedNotify
    
    return ONVIF_OK;
}

/**
 * LockOpenReleaseDoor 
 *
 * @param p_req the request parameter
 * @return ONVIF_OK or ONVIF_ERR_NotFound or ONVIF_ERR_Failure
 */
ONVIF_RET onvif_tdc_LockOpenReleaseDoor(tdc_LockOpenReleaseDoor_REQ * p_req)
{
    ONVIF_Door * p_door = onvif_find_Door(p_req->Token);
    if (NULL == p_door)
    {
        return ONVIF_ERR_NotFound;
    }

    if (p_door->DoorInfo.Capabilities.LockOpen == FALSE)
    {
        return ONVIF_ERR_Failure;
    }
    
    // todo : do lock open relase door operation ...

    // todo : if the door state changed, send event notify, please refer onvif_AccessPointStateEnabledChangedNotify
    
    return ONVIF_OK;
}

#endif // PROFILE_C_SUPPORT



