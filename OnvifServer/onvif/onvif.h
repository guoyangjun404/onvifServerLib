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

#ifndef	__H_ONVIF_H__
#define	__H_ONVIF_H__

#include "sys_inc.h"
#include "onvif_cm.h"
#include "linked_list.h"
#include "hqueue.h"

/***************************************************************************************/

#define ONVIF_MSG_SRC	    1
#define ONVIF_TIMER_SRC	    2
#define ONVIF_DEL_UA_SRC    3
#define ONVIF_EXIT          4

/***************************************************************************************/

// video source list
typedef struct _ONVIF_VideoSource
{
	struct _ONVIF_VideoSource * next;
	
    onvif_VideoSource       VideoSource; 
    onvif_VideoSourceMode	VideoSourceMode;

#ifdef THERMAL_SUPPORT
    BOOL                                    ThermalSupport;
    onvif_ThermalConfiguration              ThermalConfiguration;
    onvif_ThermalConfigurationOptions       ThermalConfigurationOptions;
    onvif_RadiometryConfiguration           RadiometryConfiguration;
    onvif_RadiometryConfigurationOptions    RadiometryConfigurationOptions;
#endif
} ONVIF_VideoSource;

// video source mode list
typedef struct _ONVIF_VideoSourceMode
{
    struct _ONVIF_VideoSourceMode * next;

	onvif_VideoSourceMode	VideoSourceMode;  	
} ONVIF_VideoSourceMode;

// video source configuration list
typedef struct _ONVIF_VideoSourceConfiguration
{
	struct _ONVIF_VideoSourceConfiguration * next;

	onvif_VideoSourceConfiguration Configuration; 
} ONVIF_VideoSourceConfiguration;

// video encoder configuration list
typedef struct _ONVIF_VideoEncoderConfiguration
{    
	struct _ONVIF_VideoEncoderConfiguration * next;	

	onvif_VideoEncoderConfiguration	Configuration;	
} ONVIF_VideoEncoderConfiguration;

// audio source list
typedef struct _ONVIF_AudioSource
{    
	struct _ONVIF_AudioSource * next;
	
    onvif_AudioSource AudioSource;	
} ONVIF_AudioSource;

// audio source configuration list
typedef struct _ONVIF_AudioSourceConfiguration
{    
	struct _ONVIF_AudioSourceConfiguration * next;
	
    onvif_AudioSourceConfiguration 	Configuration;	
} ONVIF_AudioSourceConfiguration;

// audio encoder configuration list
typedef struct _ONVIF_AudioEncoderConfiguration
{
	struct _ONVIF_AudioEncoderConfiguration * next;
	
    onvif_AudioEncoderConfiguration Configuration;
} ONVIF_AudioEncoderConfiguration;

typedef struct _ONVIF_MetadataConfiguration
{
	struct _ONVIF_MetadataConfiguration * next;
	
	onvif_MetadataConfiguration Configuration;
} ONVIF_MetadataConfiguration;

typedef struct
{
    BOOL    UsedFlag;
    
	uint32   zoomVal;								// add by xieqingpu 加了摄像机焦距,目的是使预置位对应相应的焦距
    onvif_PTZPreset	PTZPreset;
} ONVIF_PTZPreset;

// ptz configuration list
typedef struct _ONVIF_PTZConfiguration
{
	struct _ONVIF_PTZConfiguration * next;
	
	onvif_PTZConfiguration  Configuration;
} ONVIF_PTZConfiguration;

// ptz node list
typedef struct _ONVIF_PTZNode
{
	struct _ONVIF_PTZNode * next;

	onvif_PTZNode	PTZNode;	
} ONVIF_PTZNode;

// preset tour list
typedef struct _ONVIF_PresetTour
{
	struct _ONVIF_PresetTour * next;

	onvif_PresetTour    PresetTour;
} ONVIF_PresetTour;

// video analytics configuration list
typedef struct _ONVIF_VideoAnalyticsConfiguration
{
	struct _ONVIF_VideoAnalyticsConfiguration	* next;

	onvif_SupportedRules  SupportedRules;			    // supported rules
	
	onvif_VideoAnalyticsConfiguration   Configuration;
} ONVIF_VideoAnalyticsConfiguration;

// network interface list
typedef struct _ONVIF_NetworkInterface
{
	struct _ONVIF_NetworkInterface * next;
	
	onvif_NetworkInterface	NetworkInterface; 
} ONVIF_NetworkInterface;

typedef struct 
{
	onvif_NetworkProtocol		    NetworkProtocol;
	onvif_DNSInformation		    DNSInformation;
	onvif_NTPInformation		    NTPInformation;
	onvif_HostnameInformation	    HostnameInformation;
	onvif_NetworkGateway		    NetworkGateway;
	onvif_DiscoveryMode			    DiscoveryMode;
	onvif_NetworkZeroConfiguration  ZeroConfiguration;
	
	ONVIF_NetworkInterface        * interfaces;
} ONVIF_NET;

// osd configuration list
typedef struct _ONVIF_OSDConfiguration
{
	struct _ONVIF_OSDConfiguration * next;
	
	onvif_OSDConfiguration OSD;
} ONVIF_OSDConfiguration;

typedef struct _ONVIF_Recording
{
	struct _ONVIF_Recording * next;

	onvif_Recording	Recording;
} ONVIF_Recording;

typedef struct _ONVIF_RECORDINGJOB
{	
	struct _ONVIF_RECORDINGJOB * next;

	onvif_RecordingJob	RecordingJob;
} ONVIF_RecordingJob;

typedef struct _ONVIF_NotificationMessage
{
	struct _ONVIF_NotificationMessage * next;

    int     refcnt;     // reference count

	onvif_NotificationMessage	NotificationMessage;
} ONVIF_NotificationMessage;

typedef struct ONVIF_PROFILE     // ONVIF_PROFILE 
{ 
	struct ONVIF_PROFILE * next;
	
    ONVIF_VideoSourceConfiguration      * v_src_cfg;    // video source configuration
    ONVIF_VideoEncoder2Configuration	* v_enc_cfg;    // video encoder configuration
    
#ifdef AUDIO_SUPPORT    
	ONVIF_AudioSourceConfiguration      * a_src_cfg;    // audio source configuration
	ONVIF_AudioEncoder2Configuration    * a_enc_cfg;    // audio encoder configuration
	ONVIF_AudioDecoderConfiguration     * a_dec_cfg;    // audio decoder configuration
#endif

#ifdef PTZ_SUPPORT
	ONVIF_PTZConfiguration              * ptz_cfg;	    // ptz configuration
#endif

	ONVIF_MetadataConfiguration         * metadata_cfg; // metadata configuration

#ifdef VIDEO_ANALYTICS
	ONVIF_VideoAnalyticsConfiguration   * va_cfg;	    // video analytics configuration
#endif

#ifdef DEVICEIO_SUPPORT
    ONVIF_AudioOutputConfiguration      * a_output_cfg; // audio output configuration
#endif

#ifdef PTZ_SUPPORT
    ONVIF_PTZPreset   	  	presets[MAX_PTZ_PRESETS];	// ptz presets
#endif

    char name[ONVIF_NAME_LEN];						    // profile name
    char token[ONVIF_TOKEN_LEN];					    // profile token
    char stream_uri[ONVIF_URI_LEN];					    // rtsp stream address
    BOOL fixed;										    // fixed profile flag	
    BOOL multicasting;								    // sending multicast streaming flag
} ONVIF_PROFILE;

typedef struct
{
    char 			serv_ip[128];					    // The Config service address
	unsigned short 	serv_port;						    // The Config service port
} ONVIF_SRV;

typedef struct 
{
	uint32 	    need_auth	 	: 1;			        // Whether need auth request flag
	uint32	    evt_sim_flag 	: 1;			        // event simulate flag
	uint32      log_enable      : 1;			        // Whether log enable flag
	uint32 	    https_enable	: 1;				    // whether https enable flag
	uint32	    reserved     	: 28;

    int         servs_num;                              // the servers number
    ONVIF_SRV   servs[MAX_SERVERS];
	int         http_max_users;                         // max http connection clients        
	
	onvif_User  users[MAX_USERS];	
	onvif_Scope scopes[MAX_SCOPE_NUMS];
	ONVIF_NET   network;
	
	char        EndpointReference[64];
	
	int         evt_renew_time;
	int         evt_sim_interval;				        // event simulate interval 

#ifdef PROFILE_Q_SUPPORT
    int         device_state;                           // 0 - Factory Default state, 1 - Operational State
                                                        // Factory Default State requires WS-Discovery, DHCP and IPv4 Link Local Address to be enabled by default
                                                        // A device shall provide full anonymous access to all ONVIF commands while the device operates in Factory Default State
#endif

	/********************************************************/
	ONVIF_VideoSource   		            * v_src;	
	ONVIF_VideoSourceConfiguration	        * v_src_cfg;
	ONVIF_VideoEncoder2Configuration	    * v_enc_cfg;	

#ifdef AUDIO_SUPPORT	
	ONVIF_AudioSource   		            * a_src;
	ONVIF_AudioSourceConfiguration 	        * a_src_cfg;
	ONVIF_AudioEncoder2Configuration        * a_enc_cfg;
	ONVIF_AudioDecoderConfiguration         * a_dec_cfg;
#endif

	ONVIF_PROFILE 		                    * profiles;	
	ONVIF_OSDConfiguration     		        * OSDs;
	ONVIF_MetadataConfiguration	            * metadata_cfg;

#ifdef MEDIA2_SUPPORT
    ONVIF_Mask                              * mask;
    onvif_MaskOptions                       MaskOptions;
#endif

#ifdef PTZ_SUPPORT
	ONVIF_PTZNode      		                * ptz_node;
	ONVIF_PTZConfiguration	                * ptz_cfg;
#endif

#ifdef VIDEO_ANALYTICS
	ONVIF_VideoAnalyticsConfiguration       * va_cfg;	
#endif

#ifdef PROFILE_G_SUPPORT
	ONVIF_Recording                         * recordings;
	ONVIF_RecordingJob                      * recording_jobs;
	int 	                                  replay_session_timeout;
#endif

#ifdef PROFILE_C_SUPPORT
    ONVIF_AccessPoint						* access_points;
    ONVIF_Door                              * doors;
    ONVIF_AreaInfo                          * area_info;
#endif

#ifdef DEVICEIO_SUPPORT
	ONVIF_VideoOutput						* v_output;
	ONVIF_VideoOutputConfiguration          * v_output_cfg;
	ONVIF_AudioOutput                       * a_output;
	ONVIF_AudioOutputConfiguration          * a_output_cfg;
	ONVIF_RelayOutput                       * relay_output;
	ONVIF_DigitalInput                      * digit_input;
	ONVIF_SerialPort                        * serial_port;
	
	onvif_AudioOutputConfigurationOptions   AudioOutputConfigurationOptions;
	onvif_RelayOutputOptions                RelayOutputOptions;
	onvif_DigitalInputConfigurationInputOptions DigitalInputConfigurationInputOptions;
#endif

	/********************************************************/
	onvif_DeviceInformation					DeviceInformation;
	onvif_ImagingSettings					ImagingSettings;
	onvif_ImagingOptions					ImagingOptions;
	onvif_Capabilities						Capabilities;
	onvif_SystemDateTime					SystemDateTime;
	onvif_RemoteUser                        RemoteUser;
	
	onvif_VideoSourceConfigurationOptions   VideoSourceConfigurationOptions;
	onvif_VideoEncoderConfigurationOptions  VideoEncoderConfigurationOptions;
	
	ONVIF_VideoEncoder2ConfigurationOptions * v_enc_cfg_opt;

#ifdef AUDIO_SUPPORT	
	ONVIF_AudioEncoder2ConfigurationOptions * a_enc_cfg_opt;
	onvif_AudioDecoderConfigurationOptions  a_dec_cfg_opt;
#endif
#if defined(MEDIA2_SUPPORT) && defined(AUDIO_SUPPORT)
    ONVIF_AudioEncoder2ConfigurationOptions * a_dec2_cfg_opt;
#endif

#ifdef PTZ_SUPPORT
	onvif_PTZConfigurationOptions           PTZConfigurationOptions;
#endif

	onvif_MetadataConfigurationOptions		MetadataConfigurationOptions;
	onvif_OSDConfigurationOptions			OSDConfigurationOptions;

#ifdef CREDENTIAL_SUPPORT
    ONVIF_Credential                      * credential;
#endif

#ifdef ACCESS_RULES
    ONVIF_AccessProfile                   * access_rules;
#endif

#ifdef SCHEDULE_SUPPORT
    ONVIF_Schedule                        * schedule;
    ONVIF_SpecialDayGroup                 * specialdaygroup;
#endif

#ifdef RECEIVER_SUPPORT
    ONVIF_Receiver                        * receiver;
#endif

#ifdef IPFILTER_SUPPORT
    onvif_IPAddressFilter                   ipaddr_filter;
#endif
} ONVIF_CFG;


typedef struct
{
	uint32	        sys_timer_run	: 1;			    // timer running flag
	uint32          discovery_flag	: 1;			    // discovery flag
	
	uint32	        reserved		: 30;

#ifdef MEDIA2_SUPPORT
    int             mask_idx;                           // mask index
#endif

#ifdef AUDIO_SUPPORT
	int	 			a_src_idx;                          // audio source index
	int	 			a_enc_idx;                          // audio encoder index
	int             a_dec_idx;                          // audio decoder index
#endif

	int	 			v_src_idx;                          // video source index	
	int	 			v_enc_idx;                          // video encoder index	
	int  			profile_idx;	                    // profile index
	int				netinf_idx;                         // network interface index
	int				osd_idx;                            // osd index

#ifdef PTZ_SUPPORT
    int             preset_idx;                         // preset index
#endif

#ifdef PROFILE_G_SUPPORT
	int				recording_idx;                      // recording index
	int				recordingjob_idx;                   // recording job index
	int             track_idx;                          // track index
#endif

#ifdef PROFILE_C_SUPPORT
    int             aceess_point_idx;                   // access point info index
    int             door_idx;                           // door info index
    int             area_idx;                           // area info index
#endif

#ifdef PROFILE_G_SUPPORT
    int             search_idx;                         // search index
    LINKED_LIST   * search_list;                        // search ua list
#endif

#ifdef DEVICEIO_SUPPORT
    int             v_out_idx;                          // video output index
    int             v_out_cfg_idx;                      // video output configuration index
    int             a_out_idx;                          // audio output index
    int             a_out_cfg_idx;                      // audio output configuration index
    int             relay_idx;                          // relay output index
    int             digit_input_idx;                    // digit input index
    int             serial_port_idx;                    // serial port index
#endif

#ifdef CREDENTIAL_SUPPORT
    int             credential_idx;                     // credential index
#endif

#ifdef ACCESS_RULES
    int             accessrule_idx;                     // accessrule index
#endif

#ifdef SCHEDULE_SUPPORT
    int             schedule_idx;                       // schedule index
    int             specialdaygroup_idx;                // special day group index
#endif

#ifdef RECEIVER_SUPPORT
    int             receiver_idx;                       // receiver index
#endif

#ifdef VIDEO_ANALYTICS
    int             va_idx;                            // video analytics index
#endif

	PPSN_CTX      * eua_fl;                             // event subscriber free list    
	PPSN_CTX      * eua_ul;                             // event subscriber used list  
	
	uint32 	   		timer_id;                           // timer id

	SOCKET	   		discovery_fd;					    // discovery socket handler
	pthread_t  		discovery_tid;					    // discovery task handler

    HQUEUE *		msg_queue;		                    // message receive queue
	pthread_t		tid_main;                           // main task thread
} ONVIF_CLS;

typedef struct onvif_internal_msg
{
	char *  msg_dua;		    // message destination unit
	uint32	msg_evt;		    // event / command value
	uint32	msg_src;		    // message type
	int	    msg_len;		    // message buffer length
	char *  msg_buf;		    // message buffer
} OIMSG;


#ifdef __cplusplus
extern "C" {
#endif


/***************************************************************************************/
void                                        onvif_init();
void                                        onvif_init_cfg();

/***************************************************************************************/
BOOL                                        onvif_is_scope_exist(const char * scope);
onvif_Scope *                               onvif_find_scope(const char * scope);
ONVIF_RET                                   onvif_add_scope(const char * scope, BOOL fixed);
onvif_Scope *                               onvif_get_idle_scope();

/***************************************************************************************/
BOOL                                        onvif_is_user_exist(const char * username);
ONVIF_RET                                   onvif_add_user(onvif_User * p_user);
ONVIF_RET                                   add_to_Gusers(void);			// add by xieqingpu

onvif_User *                                onvif_find_user(const char * username);
onvif_User *                                onvif_get_idle_user();
const char *                                onvif_get_user_pass(const char * username);

/***************************************************************************************/
ONVIF_PROFILE *                             onvif_find_profile(const char * token);
ONVIF_PROFILE *                             onvif_add_profile(BOOL fixed);

ONVIF_VideoSource *                         onvif_find_VideoSource(const char * token);
ONVIF_VideoSource *                         onvif_find_VideoSource_by_size(int w, int h);
ONVIF_VideoSource *                         onvif_add_VideoSource(int w, int h);

ONVIF_VideoSourceConfiguration *            onvif_add_VideoSourceConfiguration(int w, int h);
ONVIF_VideoSourceConfiguration *            onvif_find_VideoSourceConfiguration(const char * token);
ONVIF_VideoSourceConfiguration *            onvif_find_VideoSourceConfiguration_by_size(int w, int h);

ONVIF_VideoEncoder2Configuration *          onvif_add_VideoEncoderConfiguration(ONVIF_VideoEncoder2Configuration * p_v_enc_cfg);
ONVIF_VideoEncoder2Configuration *          onvif_find_VideoEncoderConfiguration(const char * token);
ONVIF_VideoEncoder2Configuration *          onvif_find_VideoEncoderConfiguration_by_param(ONVIF_VideoEncoder2Configuration * p_v_enc_cfg);

ONVIF_NetworkInterface *                    onvif_find_NetworkInterface(const char * token);
ONVIF_OSDConfiguration *                    onvif_add_OSDConfiguration();
ONVIF_OSDConfiguration *                    onvif_find_OSDConfiguration(const char * token);
ONVIF_MetadataConfiguration *               onvif_find_MetadataConfiguration(const char * token);

ONVIF_NotificationMessage *                 onvif_add_NotificationMessage(ONVIF_NotificationMessage ** p_head);
void                                        onvif_free_NotificationMessage(ONVIF_NotificationMessage * p_message);
void                                        onvif_free_NotificationMessages(ONVIF_NotificationMessage ** p_head);

ONVIF_SimpleItem *                          onvif_add_SimpleItem(ONVIF_SimpleItem ** p_head);
void                                        onvif_free_SimpleItems(ONVIF_SimpleItem ** p_head);

ONVIF_ElementItem *                         onvif_add_ElementItem(ONVIF_ElementItem ** p_head);
void                                        onvif_free_ElementItems(ONVIF_ElementItem ** p_head);

ONVIF_VideoEncoder2ConfigurationOptions   * onvif_add_VideoEncoder2ConfigurationOptions(ONVIF_VideoEncoder2ConfigurationOptions ** p_head);
ONVIF_VideoEncoder2ConfigurationOptions   * onvif_find_VideoEncoder2ConfigurationOptions(const char * Encoding);

#ifdef MEDIA2_SUPPORT
ONVIF_Mask *                                onvif_add_Mask();
ONVIF_Mask *                                onvif_find_Mask(const char * token);
void                                        onvif_free_Masks(ONVIF_Mask ** p_head);
#endif

/***************************************************************************************/
#ifdef AUDIO_SUPPORT
ONVIF_AudioSource *                         onvif_find_AudioSource(const char * token);
ONVIF_AudioSourceConfiguration            * onvif_add_AudioSourceConfiguration();
ONVIF_AudioSourceConfiguration            * onvif_find_AudioSourceConfiguration(const char * token);
ONVIF_AudioEncoder2Configuration          * onvif_add_AudioEncoderConfiguration(ONVIF_AudioEncoder2Configuration * p_a_enc_cfg);
ONVIF_AudioEncoder2Configuration          * onvif_find_AudioEncoderConfiguration(const char * token);
ONVIF_AudioEncoder2Configuration          * onvif_find_AudioEncoderConfiguration_by_param(ONVIF_AudioEncoder2Configuration * p_a_enc_cfg);
ONVIF_AudioEncoder2ConfigurationOptions   * onvif_add_AudioEncoder2ConfigurationOptions(ONVIF_AudioEncoder2ConfigurationOptions ** p_head);
ONVIF_AudioEncoder2ConfigurationOptions   * onvif_find_AudioEncoder2ConfigurationOptions(const char * Encoding);

ONVIF_AudioDecoderConfiguration           * onvif_add_AudioDecoderConfiguration();
ONVIF_AudioDecoderConfiguration           * onvif_find_AudioDecoderConfiguration(const char * token);
#endif

/***************************************************************************************/

#ifdef PTZ_SUPPORT
ONVIF_PTZNode                             * onvif_find_PTZNode(const char * token);
ONVIF_PTZConfiguration                    * onvif_find_PTZConfiguration(const char * token);
ONVIF_PTZPreset                           * onvif_find_PTZPreset(const char * profile_token, const char  * preset_token);
ONVIF_PTZPreset                           * onvif_get_idle_PTZPreset(const char * profile_token);
#endif

/***************************************************************************************/

#ifdef PROFILE_G_SUPPORT
ONVIF_Recording  *                          onvif_add_Recording();
ONVIF_Recording  *                          onvif_find_Recording(const char * token);
void                                        onvif_free_Recording(ONVIF_Recording * p_recording);
ONVIF_Track *                               onvif_add_Track(ONVIF_Track ** p_head);	
void                                        onvif_free_Track(ONVIF_Track ** p_head, ONVIF_Track * p_track);
void                                        onvif_free_Tracks(ONVIF_Track ** p_head);
ONVIF_Track *                               onvif_find_Track(ONVIF_Track	* p_head, const char * token);
int	                                        onvif_get_track_nums_by_type(ONVIF_Track * p_head, onvif_TrackType);
ONVIF_RecordingJob *                        onvif_add_RecordingJob();
ONVIF_RecordingJob *                        onvif_find_RecordingJob(const char * token);
void                                        onvif_free_RecordingJob(ONVIF_RecordingJob * p_head);

ONVIF_RecordingInformation *                onvif_add_RecordingInformation(ONVIF_RecordingInformation ** p_head);
void                                        onvif_free_RecordingInformations(ONVIF_RecordingInformation ** p_head);
ONVIF_FindEventResult *                     onvif_add_FindEventResult(ONVIF_FindEventResult ** p_head);
void                                        onvif_free_FindEventResults(ONVIF_FindEventResult ** p_head);
ONVIF_FindMetadataResult *                  onvif_add_FindMetadataResult(ONVIF_FindMetadataResult ** p_head);
void                                        onvif_free_FindMetadataResults(ONVIF_FindMetadataResult ** p_head);
ONVIF_FindPTZPositionResult *               onvif_add_FindPTZPositionResult(ONVIF_FindPTZPositionResult ** p_head);
void                                        onvif_free_FindPTZPositionResult(ONVIF_FindPTZPositionResult ** p_head);
#endif

/***************************************************************************************/
#ifdef VIDEO_ANALYTICS
ONVIF_Config *                              onvif_add_Config(ONVIF_Config ** p_head);
void                                        onvif_free_Config(ONVIF_Config * p_config);
void                                        onvif_free_Configs(ONVIF_Config ** p_head);
ONVIF_Config *                              onvif_find_Config(ONVIF_Config ** p_head, const char * name);
void                                        onvif_remove_Config(ONVIF_Config ** p_head, ONVIF_Config * p_remove);
ONVIF_Config *                              onvif_get_prev_Config(ONVIF_Config ** p_head, ONVIF_Config * p_found);

ONVIF_ConfigDescription *                   onvif_add_ConfigDescription(ONVIF_ConfigDescription ** p_head);
void                                        onvif_free_ConfigDescriptions(ONVIF_ConfigDescription ** p_head);

ONVIF_ConfigDescription_Messages *          onvif_add_ConfigDescription_Message(ONVIF_ConfigDescription_Messages ** p_head);
void                                        onvif_free_ConfigDescription_Messages(ONVIF_ConfigDescription_Messages ** p_head);

ONVIF_ConfigOptions *                       onvif_add_ConfigOptions(ONVIF_ConfigOptions ** p_head);
void                                        onvif_free_ConfigOptions(ONVIF_ConfigOptions ** p_head);

ONVIF_SimpleItemDescription *               onvif_add_SimpleItemDescription(ONVIF_SimpleItemDescription ** p_head);
void                                        onvif_free_SimpleItemDescriptions(ONVIF_SimpleItemDescription ** p_head);

ONVIF_VideoAnalyticsConfiguration *         onvif_add_VideoAnalyticsConfiguration(ONVIF_VideoAnalyticsConfiguration ** p_head);
ONVIF_VideoAnalyticsConfiguration *         onvif_find_VideoAnalyticsConfiguration(const char * token);
void                                        onvif_free_VideoAnalyticsConfigurations(ONVIF_VideoAnalyticsConfiguration ** p_head);
#endif

/***************************************************************************************/
#ifdef PROFILE_C_SUPPORT
ONVIF_AccessPoint *                         onvif_add_AccessPoint(ONVIF_AccessPoint ** p_head);
ONVIF_AccessPoint *                         onvif_find_AccessPoint(const char * token);
void                                        onvif_free_AccessPoints(ONVIF_AccessPoint ** p_head);
ONVIF_Door *                                onvif_add_Door(ONVIF_Door ** p_head);
ONVIF_Door *                                onvif_find_Door(const char * token);
void                                        onvif_free_Doors(ONVIF_Door ** p_head);
ONVIF_AreaInfo *                            onvif_add_AreaInfo(ONVIF_AreaInfo ** p_head);
ONVIF_AreaInfo *                            onvif_find_AreaInfo(const char * token);
void                                        onvif_free_AreaInfos(ONVIF_AreaInfo ** p_head);
#endif // end of PROFILE_C_SUPPORT

/***************************************************************************************/
#ifdef DEVICEIO_SUPPORT
ONVIF_PaneLayout *                          onvif_add_PaneLayout(ONVIF_PaneLayout ** p_head);
ONVIF_PaneLayout *                          onvif_find_PaneLayout(const char * token);
void                                        onvif_free_PaneLayouts(ONVIF_PaneLayout ** p_head);

ONVIF_VideoOutput *                         onvif_add_VideoOutput(ONVIF_VideoOutput ** p_head);
ONVIF_VideoOutput *                         onvif_find_VideoOutput(const char * token);
void                                        onvif_free_VideoOutputs(ONVIF_VideoOutput ** p_head);

ONVIF_VideoOutputConfiguration *            onvif_add_VideoOutputConfiguration(ONVIF_VideoOutputConfiguration ** p_head);
ONVIF_VideoOutputConfiguration *            onvif_find_VideoOutputConfiguration(const char * token);
ONVIF_VideoOutputConfiguration *            onvif_find_VideoOutputConfiguration_by_OutputToken(const char * token);
void                                        onvif_free_VideoOutputConfigurations(ONVIF_VideoOutputConfiguration ** p_head);

ONVIF_AudioOutput *                         onvif_add_AudioOutput(ONVIF_AudioOutput ** p_head);
ONVIF_AudioOutput *                         onvif_find_AudioOutput(const char * token);
void                                        onvif_free_AudioOutputs(ONVIF_AudioOutput ** p_head);

ONVIF_AudioOutputConfiguration *            onvif_add_AudioOutputConfiguration(ONVIF_AudioOutputConfiguration ** p_head);
ONVIF_AudioOutputConfiguration *            onvif_find_AudioOutputConfiguration(const char * token);
ONVIF_AudioOutputConfiguration *            onvif_find_AudioOutputConfiguration_by_OutputToken(const char * token);
void                                        onvif_free_AudioOutputConfigurations(ONVIF_AudioOutputConfiguration ** p_head);

ONVIF_RelayOutput *                         onvif_add_RelayOutput(ONVIF_RelayOutput ** p_head);
ONVIF_RelayOutput *                         onvif_find_RelayOutput(const char * token);
void                                        onvif_free_RelayOutputs(ONVIF_RelayOutput ** p_head);

ONVIF_DigitalInput *                        onvif_add_DigitalInput(ONVIF_DigitalInput ** p_head);
ONVIF_DigitalInput *                        onvif_find_DigitalInput(const char * token);
void                                        onvif_free_DigitalInputs(ONVIF_DigitalInput ** p_head);

ONVIF_SerialPort *                          onvif_add_SerialPort(ONVIF_SerialPort ** p_head);
ONVIF_SerialPort *                          onvif_find_SerialPort(const char * token);
ONVIF_SerialPort *                          onvif_find_SerialPort_by_ConfigurationToken(const char * token);
void                                        onvif_free_SerialPorts(ONVIF_SerialPort ** p_head);
void                                        onvif_malloc_SerialData(onvif_SerialData * p_data, int union_SerialData, int size);
void                                        onvif_free_SerialData(onvif_SerialData * p_data);

#endif // end of DEVICEIO_SUPPORT

/***************************************************************************************/
#ifdef THERMAL_SUPPORT
ONVIF_ColorPalette *                        onvif_add_ColorPalette(ONVIF_ColorPalette ** p_head);
void                                        onvif_free_ColorPalettes(ONVIF_ColorPalette ** p_head);
ONVIF_NUCTable *                            onvif_add_NUCTable(ONVIF_NUCTable ** p_head);
void                                        onvif_free_NUCTables(ONVIF_NUCTable ** p_head);
BOOL                                        onvif_init_Thermal(ONVIF_VideoSource * p_req);
#endif // end of THERMAL_SUPPORT

/***************************************************************************************/
#ifdef CREDENTIAL_SUPPORT
ONVIF_Credential *                          onvif_add_Credential();
ONVIF_Credential *                          onvif_find_Credential(const char * token);
void                                        onvif_free_Credential(ONVIF_Credential * p_node);
void                                        onvif_free_Credentials();
BOOL                                        onvif_init_Credential();
#endif // end of CREDENTIAL_SUPPORT

/***************************************************************************************/
#ifdef ACCESS_RULES
ONVIF_AccessProfile *                       onvif_add_AccessProfile();
ONVIF_AccessProfile *                       onvif_find_AccessProfile(const char * token);
void                                        onvif_free_AccessProfile(ONVIF_AccessProfile * p_node);
void                                        onvif_free_AccessProfiles();
BOOL                                        onvif_init_AccessProfile();
#endif // end of ACCESS_RULES

#ifdef SCHEDULE_SUPPORT
ONVIF_Schedule *                            onvif_add_Schedule();
ONVIF_Schedule *                            onvif_find_Schedule(const char * token);
void                                        onvif_free_Schedule(ONVIF_Schedule * p_node);
void                                        onvif_free_Schedules();
BOOL                                        onvif_init_Schedule();

ONVIF_SpecialDayGroup *                     onvif_add_SpecialDayGroup();
ONVIF_SpecialDayGroup *                     onvif_find_SpecialDayGroup(const char * token);
void                                        onvif_free_SpecialDayGroup(ONVIF_SpecialDayGroup * p_node);
void                                        onvif_free_SpecialDayGroups();
BOOL                                        onvif_init_SpecialDayGroup();
#endif // end of SCHEDULE_SUPPORT

#ifdef RECEIVER_SUPPORT
ONVIF_Receiver *                            onvif_add_Receiver();
ONVIF_Receiver *                            onvif_find_Receiver(const char * token);
void                                        onvif_free_Receiver(ONVIF_Receiver * p_node);
void                                        onvif_free_Receivers();
int                                         onvif_get_Receiver_nums();
#endif // end of RECEIVER_SUPPORT

#ifdef IPFILTER_SUPPORT
BOOL                                        onvif_is_ipaddr_filter_exist(onvif_PrefixedIPAddress * p_head, int size, onvif_PrefixedIPAddress * p_item);
onvif_PrefixedIPAddress *                   onvif_find_ipaddr_filter(onvif_PrefixedIPAddress * p_head, int size, onvif_PrefixedIPAddress * p_item);
onvif_PrefixedIPAddress *                   onvif_get_idle_ipaddr_filter(onvif_PrefixedIPAddress * p_head, int size);
ONVIF_RET                                   onvif_add_ipaddr_filter(onvif_PrefixedIPAddress * p_head, int size, onvif_PrefixedIPAddress * p_item);
#endif // IPFILTER_SUPPORT


#ifdef __cplusplus
}
#endif

#endif


