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

#ifndef ONVIF_IMAGE_H
#define ONVIF_IMAGE_H

#include "sys_inc.h"
#include "onvif.h"

/***************************************************************************************/
typedef struct
{
	uint32		ForcePersistenceFlag	: 1;		// Indicates whether the field ForcePersistence is valid
	uint32  	Reserved	 			: 31;
	
	char		VideoSourceToken[ONVIF_TOKEN_LEN];	// required		
	BOOL		ForcePersistence;					// optional, Flag that makes configuration persistent. Example: User wants the configuration to exist after reboot

	onvif_ImagingSettings	ImagingSettings;		// required
} SetImagingSettings_REQ;

typedef struct
{
	uint32		SpeedFlag	: 1;					// Indicates whether the field Speed is valid
	uint32  	Reserved	: 31;
	
	float		Position;							// required, Position parameter for the absolute focus control
	float		Speed;								// optional, Speed parameter for the absolute focus control
} AbsoluteFocus;

typedef struct
{
	uint32		SpeedFlag	: 1;					// Indicates whether the field Speed is valid
	uint32  	Reserved	: 31;
	
	float		Distance;							// required, Distance parameter for the relative focus control
	float		Speed;								// optional, Speed parameter for the relative focus control
} RelativeFocus;

typedef struct
{
	float		Speed;								// required, Speed parameter for the Continuous focus control
} ContinuousFocus;

typedef struct
{
	uint32		AbsoluteFlag	: 1;				// Indicates whether the field Absolute is valid
	uint32		RelativeFlag	: 1;				// Indicates whether the field Relative is valid
	uint32		ContinuousFlag	: 1;				// Indicates whether the field Continuous is valid
	uint32  	Reserved		: 29;
	
	AbsoluteFocus	Absolute;						// optional, Parameters for the absolute focus control
	RelativeFocus	Relative;						// optional, Parameters for the relative focus control
	ContinuousFocus	Continuous;						// optional, Parameter for the continuous focus control
} FocusMove;

typedef struct
{
	char		VideoSourceToken[ONVIF_TOKEN_LEN];	// required, Reference to the VideoSource for the requested move (focus) operation	
	FocusMove	Focus;								// required, Content of the requested move (focus) operation
} Move_REQ;

#ifdef __cplusplus
extern "C" {
#endif

/***************************************************************************************/
ONVIF_RET onvif_SetImagingSettings(SetImagingSettings_REQ * p_req);
ONVIF_RET onvif_Move(Move_REQ * p_req);
ONVIF_RET onvif_img_Stop(const char * VideoSourceToken);
ONVIF_RET onvif_img_GetStatus(ONVIF_VideoSource * p_v_src, onvif_ImagingStatus * p_status);
ONVIF_RET onvif_GetMoveOptions(ONVIF_VideoSource * p_v_src, onvif_MoveOptions20 * p_option);

#ifdef __cplusplus
}
#endif


#endif





