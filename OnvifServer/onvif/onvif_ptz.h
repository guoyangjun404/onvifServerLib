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

#ifndef ONVIF_PTZ_H
#define ONVIF_PTZ_H

#include "sys_inc.h"
#include "onvif.h"


typedef struct
{
	uint32	TimeoutFlag 	: 1;				// Indicates whether the field Timeout is valid
	uint32  Reserved	 	: 31;
	
	char	ProfileToken[ONVIF_TOKEN_LEN];		// required, A reference to the MediaProfile
	
	onvif_PTZSpeed	Velocity;					// required, A Velocity vector specifying the velocity of pan, tilt and zoom
	
	int		Timeout;							// optional, An optional Timeout parameter, unit is second
} ContinuousMove_REQ;

typedef struct 
{
	uint32	PanTiltFlag 	: 1;				// Indicates whether the field PanTilt is valid
	uint32	ZoomFlag 		: 1;				// Indicates whether the field Zoom is valid
	uint32  Reserved	 	: 30;
	
	char	ProfileToken[ONVIF_TOKEN_LEN];		// required, A reference to the MediaProfile that indicate what should be stopped

	BOOL	PanTilt;							// optional, Set true when we want to stop ongoing pan and tilt movements.If PanTilt arguments are not present, this command stops these movements
	BOOL	Zoom;								// optional, Set true when we want to stop ongoing zoom movement.If Zoom arguments are not present, this command stops ongoing zoom movement
} PTZ_Stop_REQ;

typedef struct
{
	uint32	SpeedFlag 		: 1;				// Indicates whether the field Speed is valid
	uint32  Reserved	 	: 31;
	
    char	ProfileToken[ONVIF_TOKEN_LEN];		// required, A reference to the MediaProfile

	onvif_PTZVector	Position;					// required, A Position vector specifying the absolute target position
	onvif_PTZSpeed	Speed;						// optional, An optional Speed    
} AbsoluteMove_REQ;

typedef struct
{
	uint32	SpeedFlag 		: 1;				// Indicates whether the field Speed is valid
	uint32  Reserved	 	: 31;
	
    char	ProfileToken[ONVIF_TOKEN_LEN];		// required, A reference to the MediaProfile
    
    onvif_PTZVector	Translation;				// required, A positional Translation relative to the current position
	onvif_PTZSpeed	Speed;						// optional, An optional Speed parameter
} RelativeMove_REQ;

typedef struct
{
	uint32	PresetTokenFlag : 1;				// Indicates whether the field PresetToken is valid
	uint32	PresetNameFlag 	: 1;				// Indicates whether the field PresetName is valid
	uint32  Reserved	 	: 30;
	
    char	ProfileToken[ONVIF_TOKEN_LEN];		// required, A reference to the MediaProfile where the operation should take place
    char	PresetToken[ONVIF_TOKEN_LEN];		// optional, A requested preset token
    char    PresetName[ONVIF_NAME_LEN];			// optional, A requested preset name
} SetPreset_REQ;

typedef struct
{
	char	ProfileToken[ONVIF_TOKEN_LEN];		// required, A reference to the MediaProfile where the operation should take place
    char	PresetToken[ONVIF_TOKEN_LEN];		// required, A requested preset token
} RemovePreset_REQ;

typedef struct
{
	uint32	SpeedFlag 		: 1;				// Indicates whether the field Speed is valid
	uint32  Reserved	 	: 31;
	
	char	ProfileToken[ONVIF_TOKEN_LEN];		// required, A reference to the MediaProfile where the operation should take place
    char	PresetToken[ONVIF_TOKEN_LEN];		// required, A requested preset token

	onvif_PTZSpeed	Speed;						// optional, A requested speed.The speed parameter can only be specified when Speed Spaces are available for the PTZ Node
} GotoPreset_REQ;

typedef struct
{
	uint32	SpeedFlag 		: 1;				// Indicates whether the field Speed is valid
	uint32  Reserved	 	: 31;
	
    char	ProfileToken[ONVIF_TOKEN_LEN];		// required, A reference to the MediaProfile where the operation should take place

    onvif_PTZSpeed	Speed;						// optional, A requested speed.The speed parameter can only be specified when Speed Spaces are available for the PTZ Node
} GotoHomePosition_REQ;

typedef struct
{
	onvif_PTZConfiguration  PTZConfiguration;	// required, 

	BOOL	ForcePersistence;					// required, 	
} SetConfiguration_REQ;

typedef struct
{
    char    ProfileToken[ONVIF_TOKEN_LEN];	    // required, Contains the token of an existing media profile the configurations shall be compatible with
} GetCompatibleConfigurations_REQ;


#ifdef __cplusplus
extern "C" {
#endif

ONVIF_RET onvif_ptz_GetStatus(ONVIF_PROFILE * p_profile, onvif_PTZStatus * p_ptz_status);

ONVIF_RET onvif_ContinuousMove(ContinuousMove_REQ * p_req);
ONVIF_RET onvif_ptz_Stop(PTZ_Stop_REQ * p_req);
ONVIF_RET onvif_AbsoluteMove(AbsoluteMove_REQ * p_req);
ONVIF_RET onvif_RelativeMove(RelativeMove_REQ * p_req);
ONVIF_RET onvif_SetPreset(SetPreset_REQ * p_req);
ONVIF_RET onvif_RemovePreset(RemovePreset_REQ * p_req);
ONVIF_RET onvif_GotoPreset(GotoPreset_REQ * p_req);
ONVIF_RET onvif_GotoHomePosition(GotoHomePosition_REQ * p_req);
ONVIF_RET onvif_SetHomePosition(const char * token);
ONVIF_RET onvif_SetConfiguration(SetConfiguration_REQ * p_req);

#ifdef __cplusplus
}
#endif


#endif


