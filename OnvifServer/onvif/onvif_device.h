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

#ifndef _DEVICE_H_
#define _DEVICE_H_

#include "sys_inc.h"
#include "onvif.h"

/***************************************************************************************/
typedef struct 
{
    onvif_CapabilityCategory    Category;               // optional, List of categories to retrieve capability information on
} GetCapabilities_REQ;

typedef struct
{
    BOOL    IncludeCapability;
} GetServices_REQ;

typedef struct 
{
	onvif_DNSInformation	DNSInformation;			    // required, 
} SetDNS_REQ;

typedef struct 
{
	onvif_NTPInformation	NTPInformation;			    // required, 
} SetNTP_REQ;

typedef struct
{
	onvif_NetworkProtocol	NetworkProtocol;		    // required,  
} SetNetworkProtocols_REQ;

typedef struct
{
	char 	IPv4Address[MAX_GATEWAY][32];			    // optional, Sets IPv4 gateway address used as default setting
} SetNetworkDefaultGateway_REQ;

typedef struct
{
    onvif_SystemLogType LogType;	                    // required, Specifies the type of system log to get
} GetSystemLog_REQ;

typedef struct
{
    char    String[2048];                               // optional, Contains the system log information
} GetSystemLog_RES;

typedef struct
{	
	uint32	UTCDateTimeFlag	: 1;					    // Indicates whether the field UTCDateTime is valid
	uint32 	Reserved		: 31;
	
    onvif_SystemDateTime	SystemDateTime;			    // required,     
    onvif_DateTime 			UTCDateTime;			    // optional, Date and time in UTC. If time is obtained via NTP, UTCDateTime has no meaning
} SetSystemDateAndTime_REQ;

typedef struct
{
    uint32  SystemLogUriFlag    : 1;
    uint32  AccessLogUriFlag    : 1;
    uint32  SupportInfoUriFlag  : 1;
    uint32  SystemBackupUriFlag : 1;
    uint32  Reserved            : 28;
    
    char    SystemLogUri[256];                          // optional
    char    AccessLogUri[256];                          // optional
	char    SupportInfoUri[256];	                    // optional
	char    SystemBackupUri[256];	                    // optional
} GetSystemUris_RES;

typedef struct
{
	onvif_NetworkInterface	NetworkInterface;		    // required,  
} SetNetworkInterfaces_REQ;

typedef struct
{
	onvif_DiscoveryMode	DiscoveryMode;				    // required, Indicator of discovery mode: Discoverable, NonDiscoverable
} SetDiscoveryMode_REQ;

typedef struct
{	
	char	UploadUri[256];							    // required, A URL to which the firmware file may be uploaded
	int		UploadDelay;							    // required, An optional delay; the client shall wait for this amount of time before initiating the firmware upload, unit is second
	int		ExpectedDownTime;						    // required, A duration that indicates how long the device expects to be unavailable after the firmware upload is complete, unit is second
} StartFirmwareUpgrade_RES;

typedef struct 
{
    char    UploadUri[256];	                            // required
	int     ExpectedDownTime;	                        // required
} StartSystemRestore_RES;

typedef struct
{
    onvif_NetworkZeroConfiguration ZeroConfiguration;   // Contains the zero-configuration
} GetZeroConfiguration_RES;

typedef struct
{
    char    InterfaceToken[ONVIF_TOKEN_LEN];            // requied, Unique identifier referencing the physical interface
    BOOL    Enabled;                                    // requied, Specifies if the zero-configuration should be enabled or not
} SetZeroConfiguration_REQ;

typedef struct 
{
    onvif_User User[MAX_USERS];
} CreateUsers_REQ;

typedef struct 
{
    char    Username[MAX_USERS][64];
} DeleteUsers_REQ;

typedef struct 
{
    onvif_User User[MAX_USERS];
} SetUser_REQ;

typedef struct
{
    uint32	RemoteUserFlag	: 1;					    // Indicates whether the field RemoteUser is valid
	uint32 	Reserved		: 31;
	
    onvif_RemoteUser RemoteUser;                        // optional
} GetRemoteUser_RES;

typedef struct
{
    uint32	RemoteUserFlag	: 1;					    // Indicates whether the field RemoteUser is valid
	uint32 	Reserved		: 31;
	
    onvif_RemoteUser RemoteUser;                       // optional     
} SetRemoteUser_REQ;

typedef struct
{
	onvif_IPAddressFilter   IPAddressFilter;	        // required
} GetIPAddressFilter_RES;

typedef struct 
{
	onvif_IPAddressFilter   IPAddressFilter;	        // required
} SetIPAddressFilter_REQ;

typedef struct
{
	onvif_IPAddressFilter   IPAddressFilter;	        // required
} AddIPAddressFilter_REQ;

typedef struct
{
	onvif_IPAddressFilter   IPAddressFilter;	        // required
} RemoveIPAddressFilter_REQ;

typedef struct
{
    char    ScopeItem[MAX_SCOPE_NUMS][128];
} AddScopes_REQ;

typedef struct
{
    char    Scopes[MAX_SCOPE_NUMS][128];
} SetScopes_REQ;

typedef struct
{
    char    ScopeItem[MAX_SCOPE_NUMS][128];
} RemoveScopes_REQ;

typedef struct 
{
	char    InterfaceToken[ONVIF_TOKEN_LEN];	        // required
} GetDot11Status_REQ;

typedef struct 
{
	onvif_Dot11Status   Status;	                        // required
} GetDot11Status_RES;

typedef struct 
{
	char    InterfaceToken[ONVIF_TOKEN_LEN];	        // required
} ScanAvailableDot11Networks_REQ;

typedef struct 
{
    int     sizeNetworks;	                            // sequence of elements <Networks>
	onvif_Dot11AvailableNetworks Networks[10];
} ScanAvailableDot11Networks_RES;


#ifdef __cplusplus
extern "C" {
#endif

ONVIF_RET onvif_GetSystemLog(GetSystemLog_REQ * p_req, GetSystemLog_RES * p_res);
ONVIF_RET onvif_SetSystemDateAndTime(SetSystemDateAndTime_REQ * p_req);
ONVIF_RET onvif_SetHostname(const char * name, BOOL fromdhcp);
ONVIF_RET onvif_SetDNS(SetDNS_REQ * p_req);
ONVIF_RET onvif_SetNTP(SetNTP_REQ * p_req);
ONVIF_RET onvif_SetZeroConfiguration(SetZeroConfiguration_REQ * p_req);
ONVIF_RET onvif_SetNetworkProtocols(SetNetworkProtocols_REQ * p_req);
ONVIF_RET onvif_GetSystemUris(const char * lip, uint32 lport, GetSystemUris_RES * p_res);
ONVIF_RET onvif_SetNetworkDefaultGateway(SetNetworkDefaultGateway_REQ * p_req);
ONVIF_RET onvif_SystemReboot();
ONVIF_RET onvif_SetSystemFactoryDefault(int type /* 0:soft, 1:hard */);
ONVIF_RET onvif_SetNetworkInterfaces(SetNetworkInterfaces_REQ * p_req);
ONVIF_RET onvif_SetDiscoveryMode(SetDiscoveryMode_REQ * p_req);
ONVIF_RET onvif_GetDot11Status(GetDot11Status_REQ * p_req, GetDot11Status_RES * p_res);
ONVIF_RET onvif_ScanAvailableDot11Networks(ScanAvailableDot11Networks_REQ * p_req, ScanAvailableDot11Networks_RES * p_res);

ONVIF_RET onvif_CreateUsers(CreateUsers_REQ * p_req);
ONVIF_RET onvif_DeleteUsers(DeleteUsers_REQ * p_req);
ONVIF_RET onvif_SetUser(SetUser_REQ * p_req);
ONVIF_RET onvif_GetRemoteUser(GetRemoteUser_RES * p_res);
ONVIF_RET onvif_SetRemoteUser(SetRemoteUser_REQ * p_req);

ONVIF_RET onvif_AddScopes(AddScopes_REQ * p_req);
ONVIF_RET onvif_SetScopes(SetScopes_REQ * p_req);
ONVIF_RET onvif_RemoveScopes(RemoveScopes_REQ * p_req);

BOOL	  onvif_StartFirmwareUpgrade(const char * lip, uint32 lport, StartFirmwareUpgrade_RES * p_res);	
BOOL 	  onvif_FirmwareUpgradeCheck(const char * buff, int len);
BOOL 	  onvif_FirmwareUpgrade(const char * buff, int len);
void 	  onvif_FirmwareUpgradePost();

BOOL      onvif_StartSystemRestore(const char * lip, uint32 lport, StartSystemRestore_RES * p_res);
BOOL      onvif_SystemRestoreCheck(const char * buff, int len);
BOOL      onvif_SystemRestore(const char * buff, int len);
void      onvif_SystemRestorePost();

#ifdef IPFILTER_SUPPORT	

ONVIF_RET onvif_SetIPAddressFilter(SetIPAddressFilter_REQ * p_req);
ONVIF_RET onvif_AddIPAddressFilter(AddIPAddressFilter_REQ * p_req);
ONVIF_RET onvif_RemoveIPAddressFilter(RemoveIPAddressFilter_REQ * p_req);

#endif // end of IPFILTER_SUPPORT

#ifdef __cplusplus
}
#endif

#endif // _DEVICE_H_

