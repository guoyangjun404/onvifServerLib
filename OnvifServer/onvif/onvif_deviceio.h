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

#ifndef ONVIF_DEVICEIO_H
#define ONVIF_DEVICEIO_H

#include "onvif.h"

typedef struct
{
	char 	VideoOutputToken[ONVIF_TOKEN_LEN];					// required, Token of the requested VideoOutput
} GetVideoOutputConfiguration_REQ;

typedef struct 
{
	onvif_VideoOutputConfiguration	VideoOutputConfiguration;	// required, Current configuration of the Video output
} GetVideoOutputConfiguration_RES;

typedef struct 
{
	onvif_VideoOutputConfiguration  Configuration;	            // required

	BOOL    ForcePersistence;	                                // required, The ForcePersistence element determines how configuration
							                                    //  changes shall be stored. If true, changes shall be persistent. 
							                                    //  If false, changes MAY revert to previous values after reboot
} SetVideoOutputConfiguration_REQ;

typedef struct
{
    char 	VideoOutputToken[ONVIF_TOKEN_LEN];					// required, Token of the Video Output whose options are requested
} GetVideoOutputConfigurationOptions_REQ;

typedef struct
{
    char    AudioOutputToken[ONVIF_TOKEN_LEN];                  // required, Token of the physical Audio output
} GetAudioOutputConfiguration_REQ;  

typedef struct
{
    onvif_AudioOutputConfiguration AudioOutputConfiguration;    // required, Current configuration of the Audio output
} GetAudioOutputConfiguration_RES; 

typedef struct 
{
	onvif_AudioOutputConfiguration  Configuration;	            // required, 
	
	BOOL    ForcePersistence;	                                // required, The ForcePersistence element determines how configuration changes shall be stored
} SetAudioOutputConfiguration_REQ;

typedef struct
{
    char    AudioOutputToken[ONVIF_TOKEN_LEN];                  // required, Token of the physical Audio Output whose options are requested
} GetAudioOutputConfigurationOptions_REQ;

typedef struct 
{
    uint32  RelayOutputTokenFlag : 1;
    uint32  Reserved             : 32;
    
	char    RelayOutputToken[ONVIF_TOKEN_LEN];	                // optional, Optional reference token to the relay for which the options are requested
} GetRelayOutputOptions_REQ;

typedef struct 
{
	onvif_RelayOutput   RelayOutput;	                        // required
} SetRelayOutputSettings_REQ;

typedef struct 
{
	char    RelayOutputToken[ONVIF_TOKEN_LEN];	                // required
	
	onvif_RelayLogicalState LogicalState;	                    // required 
} SetRelayOutputState_REQ;

typedef struct 
{
    uint32  TokenFlag   : 1;
    uint32  Reserved    : 32;
    
	char    Token[ONVIF_TOKEN_LEN];	                            // optional, 
} GetDigitalInputConfigurationOptions_REQ;

typedef struct
{
    ONVIF_DigitalInput * DigitalInputs;
} SetDigitalInputConfigurations_REQ;

typedef struct 
{
	char    SerialPortToken[ONVIF_TOKEN_LEN];	                // required
} GetSerialPortConfiguration_REQ;

typedef struct 
{
	char    SerialPortToken[ONVIF_TOKEN_LEN];	                // required
} GetSerialPortConfigurationOptions_REQ;

typedef struct
{
    onvif_SerialPortConfiguration   SerialPortConfiguration;    // required 
    
	BOOL    ForcePersistance;	                                // required 
} SetSerialPortConfiguration_REQ;

typedef struct
{
    char    token[ONVIF_TOKEN_LEN];
    
    onvif_SendReceiveSerialCommand  Command;
} SendReceiveSerialCommand_REQ;

typedef struct
{
    uint32  SerialDataFlag  : 1;
    uint32  Reserved        : 31;
    
    onvif_SerialData    SerialData;                             // optional
} SendReceiveSerialCommand_RES;


#ifdef __cplusplus
extern "C" {
#endif

ONVIF_RET onvif_tmd_GetRelayOutputs();
ONVIF_RET onvif_SetVideoOutputConfiguration(SetVideoOutputConfiguration_REQ * p_req);
ONVIF_RET onvif_SetAudioOutputConfiguration(SetAudioOutputConfiguration_REQ * p_req);
ONVIF_RET onvif_SetRelayOutputSettings(SetRelayOutputSettings_REQ * p_req);
ONVIF_RET onvif_SetRelayOutputState(SetRelayOutputState_REQ * p_req);
ONVIF_RET onvif_SetDigitalInputConfigurations(SetDigitalInputConfigurations_REQ * p_req);
ONVIF_RET onvif_SetSerialPortConfiguration(SetSerialPortConfiguration_REQ * p_req);
ONVIF_RET onvif_SendReceiveSerialCommandRx(SendReceiveSerialCommand_REQ * p_req, SendReceiveSerialCommand_RES * p_res);

#ifdef __cplusplus
}
#endif


#endif // end of ONVIF_DEVICEIO_H


