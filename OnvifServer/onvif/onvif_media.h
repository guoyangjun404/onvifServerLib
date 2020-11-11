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

#ifndef ONVIF_MEDIA_H
#define ONVIF_MEDIA_H

/***************************************************************************************/
typedef struct
{ 
    char    ProfileToken[ONVIF_TOKEN_LEN];          // required, this command requests a specific profile
} GetProfile_REQ;

typedef struct
{
	uint32	TokenFlag	: 1;						// Indicates whether the field Token is valid
	uint32 	Reserved	: 31;
	
	char 	Name[ONVIF_NAME_LEN];					// required, friendly name of the profile to be created
	char 	Token[ONVIF_TOKEN_LEN];					// optional, Optional token, specifying the unique identifier of the new profile. A device supports at least a token length of 12 characters and characters "A-Z" | "a-z" | "0-9" | "-."
} CreateProfile_REQ;

typedef struct
{
    char    ProfileToken[ONVIF_TOKEN_LEN];	        // required, This element contains a  reference to the profile that should be deleted
} DeleteProfile_REQ;

typedef struct
{
	char 	ProfileToken[ONVIF_TOKEN_LEN];			// required, Reference to the profile where the configuration should be added
	char 	ConfigurationToken[ONVIF_TOKEN_LEN];	// required, Contains a reference to the VideoSourceConfiguration to add
} AddVideoSourceConfiguration_REQ;

typedef struct
{
    char 	ProfileToken[ONVIF_TOKEN_LEN];			// required, Contains a reference to the media profile from which the VideoSourceConfiguration shall be removed
} RemoveVideoSourceConfiguration_REQ;

typedef struct
{
	char 	ProfileToken[ONVIF_TOKEN_LEN];			// required, Reference to the profile where the configuration should be added
	char 	ConfigurationToken[ONVIF_TOKEN_LEN];	// required, Contains a reference to the VideoSourceConfiguration to add
} AddVideoEncoderConfiguration_REQ;

typedef struct
{
    char 	ProfileToken[ONVIF_TOKEN_LEN];			// required, Contains a reference to the media profile from which the VideoEncoderConfiguration shall be removed
} RemoveVideoEncoderConfiguration_REQ;

typedef struct
{
	char 	ProfileToken[ONVIF_TOKEN_LEN];			// required, Reference to the profile where the configuration should be added
	char 	ConfigurationToken[ONVIF_TOKEN_LEN];	// required, Contains a reference to the AudioSourceConfiguration to add
} AddAudioSourceConfiguration_REQ;

typedef struct
{
	char 	ProfileToken[ONVIF_TOKEN_LEN];			// required, Reference to the profile where the configuration should be added
	char 	ConfigurationToken[ONVIF_TOKEN_LEN];	// required, Contains a reference to the AudioSourceConfiguration to add
} AddAudioEncoderConfiguration_REQ;

typedef struct
{
	char 	ProfileToken[ONVIF_TOKEN_LEN];			// required, Reference to the profile where the configuration should be added
	char 	ConfigurationToken[ONVIF_TOKEN_LEN];	// required, Contains a reference to the PTZConfiguration to add
} AddPTZConfiguration_REQ;

typedef struct
{
	char 	ProfileToken[ONVIF_TOKEN_LEN];			// required, The ProfileToken element indicates the media profile to use and will define the configuration of the content of the stream

	onvif_StreamSetup	StreamSetup;				// required, 
} GetStreamUri_REQ;

typedef struct
{  
    onvif_MediaUri  MediaUri;                       // required, 
} GetStreamUri_RES;

typedef struct
{
	onvif_VideoEncoderConfiguration Configuration;  // required, Contains the modified video encoder configuration. The configuration shall exist in the device
	
	BOOL  	ForcePersistence;						// required, The ForcePersistence element is obsolete and should always be assumed to be true
} SetVideoEncoderConfiguration_REQ;

typedef struct
{
	uint32	ProfileTokenFlag		: 1;			// Indicates whether the field ProfileToken is valid
	uint32	ConfigurationTokenFlag	: 1;			// Indicates whether the field ConfigurationToken is valid
	uint32 	Reserved				: 30;
	
	char 	ProfileToken[ONVIF_TOKEN_LEN];			// Optional ProfileToken that specifies an existing media profile that the options shall be compatible with
	char 	ConfigurationToken[ONVIF_TOKEN_LEN];	// Optional video source configurationToken that specifies an existing configuration that the options are intended for
} GetVideoSourceConfigurationOptions_REQ;

typedef struct
{
	onvif_VideoSourceConfiguration	Configuration;		// required, Contains the modified video source configuration. The configuration shall exist in the device

	BOOL  	ForcePersistence;						// required, The ForcePersistence element is obsolete and should always be assumed to be true
} SetVideoSourceConfiguration_REQ;

typedef struct
{
	uint32	ProfileTokenFlag		: 1;			// Indicates whether the field ProfileToken is valid
	uint32	ConfigurationTokenFlag	: 1;			// Indicates whether the field ConfigurationToken is valid
	uint32 	Reserved				: 30;
	
	char 	ProfileToken[ONVIF_TOKEN_LEN];			// Optional ProfileToken that specifies an existing media profile that the options shall be compatible with
	char 	ConfigurationToken[ONVIF_TOKEN_LEN];	// Optional video encoder configuration token that specifies an existing configuration that the options are intended for
} GetVideoEncoderConfigurationOptions_REQ;

typedef struct
{
	uint32	ProfileTokenFlag		: 1;			// Indicates whether the field ProfileToken is valid
	uint32	ConfigurationTokenFlag	: 1;			// Indicates whether the field ConfigurationToken is valid
	uint32 	Reserved				: 30;
	
	char 	ProfileToken[ONVIF_TOKEN_LEN];			// Optional ProfileToken that specifies an existing media profile that the options shall be compatible with
	char 	ConfigurationToken[ONVIF_TOKEN_LEN];	// Optional audio source configuration token that specifies an existing configuration that the options are intended for
} GetAudioSourceConfigurationOptions_REQ;

typedef struct
{
	onvif_AudioSourceConfiguration	Configuration;  // required, Contains the modified audio source configuration. The configuration shall exist in the device
	
	BOOL  	ForcePersistence;						// required, The ForcePersistence element is obsolete and should always be assumed to be true
} SetAudioSourceConfiguration_REQ;

typedef struct
{
	uint32	ProfileTokenFlag		: 1;			// Indicates whether the field ProfileToken is valid
	uint32	ConfigurationTokenFlag	: 1;			// Indicates whether the field ConfigurationToken is valid
	uint32 	Reserved				: 30;
	
	char 	ProfileToken[ONVIF_TOKEN_LEN];			// Optional ProfileToken that specifies an existing media profile that the options shall be compatible with
	char 	ConfigurationToken[ONVIF_TOKEN_LEN];	// Optional audio encoder configuration token that specifies an existing configuration that the options are intended fo
} GetAudioEncoderConfigurationOptions_REQ;

typedef struct
{
	onvif_AudioEncoderConfiguration	Configuration;	// required, Contains the modified audio encoder configuration. The configuration shall exist in the device
	
	BOOL  	ForcePersistence;						// required, The ForcePersistence element is obsolete and should always be assumed to be true
} SetAudioEncoderConfiguration_REQ;

typedef struct
{
	uint32	ConfigurationTokenFlag	: 1;			// Indicates whether the field ConfigurationToken is valid
	uint32 	Reserved				: 31;
	
	char	ConfigurationToken[ONVIF_TOKEN_LEN];	// Optional, Token of the Video Source Configuration, which has OSDs associated with are requested. If token not exist, request all available OSDs
} GetOSDs_REQ;

typedef struct
{
	char	OSDToken[ONVIF_TOKEN_LEN];				// required, The GetOSD command fetches the OSD configuration if the OSD token is known
} GetOSD_REQ;

typedef struct
{
	onvif_OSDConfiguration	OSD;					// required, Contains the modified OSD configuration
} SetOSD_REQ;

typedef struct
{
	uint32	ConfigurationTokenFlag	: 1;			// Indicates whether the field ConfigurationToken is valid
	uint32 	Reserved				: 31;
	
	char	ConfigurationToken[ONVIF_TOKEN_LEN];	// Optional, Video Source Configuration Token that specifies an existing video source configuration that the options shall be compatible with
} GetOSDOptions_REQ;

typedef struct
{
	onvif_OSDConfiguration	OSD;					// required, Contain the initial OSD configuration for create
} CreateOSD_REQ;

typedef struct
{
	char	OSDToken[ONVIF_TOKEN_LEN];				// required, This element contains a reference to the OSD configuration that should be deleted
} DeleteOSD_REQ;

typedef struct 
{
	uint32	ConfigurationTokenFlag	: 1;			// Indicates whether the field ConfigurationToken is valid
	uint32	ProfileTokenFlag		: 1;			// Indicates whether the field ProfileToken is valid
	uint32 	Reserved				: 30;
	
	char 	ConfigurationToken[256];				// optional, Contains a list of audio output configurations that are compatible with the specified media profile
	char 	ProfileToken[ONVIF_TOKEN_LEN];			// optional, Contains the token of an existing media profile the configurations shall be compatible with
} GetMetadataConfigurationOptions_REQ;

typedef struct
{
	onvif_MetadataConfiguration	Configuration;		// required,  Contains the modified metadata configuration. The configuration shall exist in the device
	BOOL 	ForcePersistence;						// required, The ForcePersistence element is obsolete and should always be assumed to be true
} SetMetadataConfiguration_REQ;

typedef struct
{
	char	ProfileToken[ONVIF_TOKEN_LEN];			// required,  Reference to the profile where the configuration should be added
	char 	ConfigurationToken[ONVIF_TOKEN_LEN];	// required,  Contains a reference to the MetadataConfiguration to add	
} AddMetadataConfiguration_REQ;

typedef struct 
{
    uint32  ConfigurationTokenFlag  : 1;            // Indicates whether the field ConfigurationToken is valid
    uint32  ProfileTokenFlag        : 1;            // Indicates whether the field ProfileToken is valid
    uint32  Reserved                : 30;
    
	char    ConfigurationToken[ONVIF_TOKEN_LEN];	// optional, Optional audio output configuration token that specifies an existing configuration that the options are intended for
	char    ProfileToken[ONVIF_TOKEN_LEN];	        // optional, Optional ProfileToken that specifies an existing media profile that the options shall be compatible with
} trt_GetAudioOutputConfigurationOptions_REQ;

typedef struct
{
    char    ConfigurationToken[ONVIF_TOKEN_LEN];    // required, Token of the requested audio output configuration
} trt_GetAudioOutputConfiguration_REQ;

typedef struct 
{
	char    ProfileToken[ONVIF_TOKEN_LEN];	        // required, Contains the token of an existing media profile the configurations shall be compatible with
} GetCompatibleAudioOutputConfigurations_REQ;

typedef struct 
{
	char    ProfileToken[ONVIF_TOKEN_LEN];	        // required, Reference to the profile where the configuration should be added
	char    ConfigurationToken[ONVIF_TOKEN_LEN];    // required, Contains a reference to the AudioOutputConfiguration to add
} AddAudioOutputConfiguration_REQ;

typedef struct 
{
	char    ProfileToken[ONVIF_TOKEN_LEN];	        // required, Contains a reference to the media profile from which the AudioOutputConfiguration shall be removed
} RemoveAudioOutputConfiguration_REQ;

typedef struct 
{
	char    ProfileToken[ONVIF_TOKEN_LEN];	        // required, This element contains a reference to the profile where the configuration should be added
	char    ConfigurationToken[ONVIF_TOKEN_LEN];    // required, This element contains a reference to the AudioDecoderConfiguration to add
} AddAudioDecoderConfiguration_REQ;

typedef struct 
{
	char    ProfileToken[ONVIF_TOKEN_LEN];	        // required, This element contains a  reference to the media profile from which the AudioDecoderConfiguration shall be removed
} RemoveAudioDecoderConfiguration_REQ;

typedef struct 
{
	onvif_AudioDecoderConfiguration Configuration;	// required, Contains the modified audio decoder configuration. The configuration shall exist in the device
	BOOL    ForcePersistence;	                    // required, The ForcePersistence element is obsolete and should always be assumed to be true
} SetAudioDecoderConfiguration_REQ;

typedef struct 
{
    uint32  ConfigurationTokenFlag  : 1;            // Indicates whether the field ConfigurationToken is valid
    uint32  ProfileTokenFlag        : 1;            // Indicates whether the field ProfileToken is valid
    uint32  Reserved                : 30;
    
	char    ConfigurationToken[ONVIF_TOKEN_LEN];	// optional, Optional audio decoder configuration token that specifies an existing configuration that the options are intended for
	char    ProfileToken[ONVIF_TOKEN_LEN];	        // optional, Optional ProfileToken that specifies an existing media profile that the options shall be compatible with
} GetAudioDecoderConfigurationOptions_REQ;

typedef struct 
{
	char    ProfileToken[ONVIF_TOKEN_LEN];	        // required, Contains the token of an existing media profile the configurations shall be compatible with
} GetCompatibleAudioDecoderConfigurations_REQ;

typedef struct 
{
    char    VideoSourceToken[ONVIF_TOKEN_LEN];	    // required
} GetVideoSourceModes_REQ;

typedef struct
{
    char    VideoSourceToken[ONVIF_TOKEN_LEN];	    // required
	char    VideoSourceModeToken[ONVIF_TOKEN_LEN];	// required
} SetVideoSourceMode_REQ;

typedef struct
{
    BOOL    Reboot;                                 // required, The response contains information about rebooting after returning response. 
                                                    //  When Reboot is set true, a device will reboot automatically after setting mode
} SetVideoSourceMode_RES;

typedef struct
{
    char    ProfileToken[ONVIF_TOKEN_LEN];
} GetSnapshotUri_REQ;

typedef struct
{
    onvif_MediaUri  MediaUri;
} GetSnapshotUri_RES;

typedef struct
{
    char    ProfileToken[ONVIF_TOKEN_LEN];          // required, 
} SetSynchronizationPoint_REQ;



#ifdef __cplusplus
extern "C" {
#endif

ONVIF_RET onvif_CreateProfile(const char * lip, uint32 lport, CreateProfile_REQ * p_req);
ONVIF_RET onvif_DeleteProfile(DeleteProfile_REQ * p_req);
ONVIF_RET onvif_AddVideoSourceConfiguration(AddVideoSourceConfiguration_REQ * p_req);
ONVIF_RET onvif_AddVideoEncoderConfiguration(AddVideoEncoderConfiguration_REQ * p_req);
ONVIF_RET onvif_AddAudioSourceConfiguration(AddAudioSourceConfiguration_REQ * p_req);
ONVIF_RET onvif_AddAudioEncoderConfiguration(AddAudioEncoderConfiguration_REQ * p_req);
ONVIF_RET onvif_GetStreamUri(const char * lip, uint32 lport, GetStreamUri_REQ * p_req, GetStreamUri_RES * p_res);

#ifdef PTZ_SUPPORT
ONVIF_RET onvif_AddPTZConfiguration(AddPTZConfiguration_REQ * p_req);
ONVIF_RET onvif_RemovePTZConfiguration(const char * token);
#endif

ONVIF_RET onvif_RemoveVideoEncoderConfiguration(RemoveVideoEncoderConfiguration_REQ * p_req);
ONVIF_RET onvif_RemoveVideoSourceConfiguration(RemoveVideoSourceConfiguration_REQ * p_req);
ONVIF_RET onvif_RemoveAudioEncoderConfiguration(const char * token);
ONVIF_RET onvif_RemoveAudioSourceConfiguration(const char * token);

ONVIF_RET onvif_SetVideoEncoderConfiguration(SetVideoEncoderConfiguration_REQ * p_req);
ONVIF_RET onvif_SetVideoSourceConfiguration(SetVideoSourceConfiguration_REQ * p_req);
ONVIF_RET onvif_SetAudioSourceConfiguration(SetAudioSourceConfiguration_REQ * p_req);
ONVIF_RET onvif_SetAudioEncoderConfiguration(SetAudioEncoderConfiguration_REQ * p_req);

ONVIF_RET onvif_AddAudioDecoderConfiguration(AddAudioDecoderConfiguration_REQ * p_req);
ONVIF_RET onvif_RemoveAudioDecoderConfiguration(RemoveAudioDecoderConfiguration_REQ * p_req);
ONVIF_RET onvif_SetAudioDecoderConfiguration(SetAudioDecoderConfiguration_REQ * p_req);
ONVIF_RET onvif_SetSynchronizationPoint(SetSynchronizationPoint_REQ * p_req);

ONVIF_RET onvif_GetSnapshot(char * buff, int * rlen, char * profile_token);
ONVIF_RET onvif_SetOSD(SetOSD_REQ * p_req);
ONVIF_RET onvif_CreateOSD(CreateOSD_REQ * p_req);
ONVIF_RET onvif_DeleteOSD(DeleteOSD_REQ * p_req);
ONVIF_RET onvif_StartMulticastStreaming(const char * token);
ONVIF_RET onvif_StopMulticastStreaming(const char * token);
ONVIF_RET onvif_SetMetadataConfiguration(SetMetadataConfiguration_REQ * p_req);
ONVIF_RET onvif_AddMetadataConfiguration(AddMetadataConfiguration_REQ * p_req);
ONVIF_RET onvif_RemoveMetadataConfiguration(const char * profile_token);

#ifdef DEVICEIO_SUPPORT
ONVIF_RET onvif_AddAudioOutputConfiguration(AddAudioOutputConfiguration_REQ * p_req);
ONVIF_RET onvif_RemoveAudioOutputConfiguration(RemoveAudioOutputConfiguration_REQ * p_req);
#endif

ONVIF_RET onvif_SetVideoSourceMode(SetVideoSourceMode_REQ * p_req, SetVideoSourceMode_RES * p_res);
ONVIF_RET onvif_GetSnapshotUri(const char * lip, uint32 lport, GetSnapshotUri_REQ * p_req, GetSnapshotUri_RES * p_res);

#ifdef __cplusplus
}
#endif


#endif


