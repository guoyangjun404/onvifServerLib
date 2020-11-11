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

#ifndef ONVIF_DOORCONTROL_H
#define ONVIF_DOORCONTROL_H

#include "sys_inc.h"
#include "onvif_cm.h"


typedef struct
{
    uint32  LimitFlag           : 1;
    uint32  StartReferenceFlag  : 1;
    uint32  Reserved            : 30;
    
    int     Limit;	                                    // optional, Maximum number of entries to return. If not specified, less than one or higher than what the device supports, the number of items is determined by the device
	char    StartReference[256];	                    // optional, Start returning entries from this start reference. If not specified, entries shall start from the beginning of the dataset
} tac_GetAccessPointInfoList_REQ;

typedef struct
{
    uint32  NextStartReferenceFlag : 1;
    uint32  Reserved               : 31;
    
    char    NextStartReference[ONVIF_TOKEN_LEN];        // optional, StartReference to use in next call to get the following items. If absent, no more items to get
	
	ONVIF_AccessPoint * AccessPointInfo;	            // optional, List of AccessPointInfo items
} tac_GetAccessPointInfoList_RES;

typedef struct
{
    char    token[ACCESS_CTRL_MAX_LIMIT][ONVIF_TOKEN_LEN];  // Tokens of AccessPointInfo items to get     
} tac_GetAccessPointInfo_REQ;

typedef struct
{
	ONVIF_AccessPoint * AccessPointInfo;	            // List of AccessPointInfo items
} tac_GetAccessPointInfo_RES;

typedef struct
{
    uint32  LimitFlag           : 1;
    uint32  StartReferenceFlag  : 1;
    uint32  Reserved            : 30;
    
    int     Limit;	                                    // optional, Maximum number of entries to return. If not specified, or higher than what the device supports, the number of items shall be determined by the device
	char    StartReference[256];	                    // optional, Start returning entries from this start reference. If not specified, entries shall start from the beginning of the dataset
} tac_GetAreaInfoList_REQ;

typedef struct
{
    uint32  NextStartReferenceFlag : 1;
    uint32  Reserved               : 31;
    
    char    NextStartReference[ONVIF_TOKEN_LEN];        // optional, StartReference to use in next call to get the following items. If absent, no more items to get
	
	ONVIF_AreaInfo * AreaInfo;	                        // optional, List of AreaInfo items
} tac_GetAreaInfoList_RES;

typedef struct
{
    char    token[ACCESS_CTRL_MAX_LIMIT][ONVIF_TOKEN_LEN];  // Tokens of DoorInfo items to get 
} tac_GetAreaInfo_REQ;

typedef struct
{
	ONVIF_AreaInfo * AreaInfo;	                        // List of AreaInfo items
} tac_GetAreaInfo_RES;

typedef struct
{
    char    Token[ONVIF_TOKEN_LEN];                     // required, Token of AccessPoint instance to get AccessPointState for
} tac_GetAccessPointState_REQ;

typedef struct
{
    BOOL    Enabled;                                    // required, Indicates that the AccessPoint is enabled. By default this field value shall be True, if the DisableAccessPoint capabilities is not supported
} tac_GetAccessPointState_RES;

typedef struct
{
    char    Token[ONVIF_TOKEN_LEN];                     // required, Token of the AccessPoint instance to enable
} tac_EnableAccessPoint_REQ;

typedef struct
{
    char    Token[ONVIF_TOKEN_LEN];                     // required, Token of the AccessPoint instance to disable
} tac_DisableAccessPoint_REQ;


typedef struct
{
    uint32  LimitFlag           : 1;
    uint32  StartReferenceFlag  : 1;
    uint32  Reserved            : 30;
    
    int     Limit;	                                    // optional, Maximum number of entries to return. If not specified, or higher than what the device supports, the number of items shall be determined by the device
	char    StartReference[256];	                    // optional, Start returning entries from this start reference. If not specified, entries shall start from the beginning of the dataset
} tdc_GetDoorInfoList_REQ;

typedef struct
{
    uint32  NextStartReferenceFlag : 1;
    uint32  Reserved               : 31;
    
    char    NextStartReference[ONVIF_TOKEN_LEN];        // optional, StartReference to use in next call to get the following items. If absent, no more items to get
	
	ONVIF_Door * DoorInfo;	                            // optional, List of DoorInfo items
} tdc_GetDoorInfoList_RES;

typedef struct
{
    char    token[DOOR_CTRL_MAX_LIMIT][ONVIF_TOKEN_LEN];// Tokens of DoorInfo items to get 
} tdc_GetDoorInfo_REQ;

typedef struct
{
	ONVIF_Door * DoorInfo;	                            // List of DoorInfo items
} tdc_GetDoorInfo_RES;

typedef struct 
{
	char    Token[ONVIF_TOKEN_LEN];	                    // required, Token of the Door instance to get the state for
} tdc_GetDoorState_REQ;

typedef struct 
{
    uint32  UseExtendedTimeFlag : 1;
    uint32  AccessTimeFlag      : 1;
    uint32  OpenTooLongTimeFlag : 1;
    uint32  PreAlarmTimeFlag    : 1;
    uint32  Reserved            : 28;
    
	char    Token[ONVIF_TOKEN_LEN];	                    // required, Token of the Door instance to control
	BOOL    UseExtendedTime;	                        // optional, Indicates that the configured extended time should be used
	int     AccessTime;	                                // optional, overrides AccessTime if specified
	int     OpenTooLongTime;	                        // optional, overrides OpenTooLongTime if specified (DOTL)
	int     PreAlarmTime;	                            // optional, overrides PreAlarmTime if specified
} tdc_AccessDoor_REQ;

typedef struct
{
    char    Token[ONVIF_TOKEN_LEN];	                    // required, Token of the Door instance to control
} tdc_LockDoor_REQ;

typedef struct
{
    char    Token[ONVIF_TOKEN_LEN];	                    // required, Token of the Door instance to control
} tdc_UnlockDoor_REQ;

typedef struct
{
    char    Token[ONVIF_TOKEN_LEN];	                    // required, Token of the Door instance to control
} tdc_DoubleLockDoor_REQ;

typedef struct
{
    char    Token[ONVIF_TOKEN_LEN];	                    // required, Token of the Door instance to control
} tdc_BlockDoor_REQ;

typedef struct
{
    char    Token[ONVIF_TOKEN_LEN];	                    // required, Token of the Door instance to control
} tdc_LockDownDoor_REQ;

typedef struct
{
    char    Token[ONVIF_TOKEN_LEN];	                    // required, Token of the Door instance to control
} tdc_LockDownReleaseDoor_REQ;

typedef struct
{
    char    Token[ONVIF_TOKEN_LEN];	                    // required, Token of the Door instance to control
} tdc_LockOpenDoor_REQ;

typedef struct
{
    char    Token[ONVIF_TOKEN_LEN];	                    // required, Token of the Door instance to control
} tdc_LockOpenReleaseDoor_REQ;


#ifdef __cplusplus
extern "C" {
#endif

ONVIF_RET onvif_tac_GetAccessPointInfoList(tac_GetAccessPointInfoList_REQ * p_req, tac_GetAccessPointInfoList_RES * p_res);
ONVIF_RET onvif_tac_GetAreaInfoList(tac_GetAreaInfoList_REQ * p_req, tac_GetAreaInfoList_RES * p_res);
ONVIF_RET onvif_tac_EnableAccessPoint(tac_EnableAccessPoint_REQ * p_req);
ONVIF_RET onvif_tac_DisableAccessPoint(tac_DisableAccessPoint_REQ * p_req);

ONVIF_RET onvif_tdc_GetDoorInfoList(tdc_GetDoorInfoList_REQ * p_req, tdc_GetDoorInfoList_RES * p_res);
ONVIF_RET onvif_tdc_AccessDoor(tdc_AccessDoor_REQ * p_req);
ONVIF_RET onvif_tdc_LockDoor(tdc_LockDoor_REQ * p_req);
ONVIF_RET onvif_tdc_UnlockDoor(tdc_UnlockDoor_REQ * p_req);
ONVIF_RET onvif_tdc_DoubleLockDoor(tdc_DoubleLockDoor_REQ * p_req);
ONVIF_RET onvif_tdc_BlockDoor(tdc_BlockDoor_REQ * p_req);
ONVIF_RET onvif_tdc_LockDownDoor(tdc_LockDownDoor_REQ * p_req);
ONVIF_RET onvif_tdc_LockDownReleaseDoor(tdc_LockDownReleaseDoor_REQ * p_req);
ONVIF_RET onvif_tdc_LockOpenDoor(tdc_LockOpenDoor_REQ * p_req);
ONVIF_RET onvif_tdc_LockOpenReleaseDoor(tdc_LockOpenReleaseDoor_REQ * p_req);

#ifdef __cplusplus
}
#endif

#endif // ONVIF_DOORCONTROL_H

