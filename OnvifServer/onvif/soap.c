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
#include "hxml.h"
#include "xml_node.h"
#include "onvif.h"
#include "http.h"
#include "http_parse.h"
#include "soap.h"
#include "onvif_device.h"
#include "onvif_pkt.h"
#include "soap_parser.h"
#include "onvif_event.h"
#include "sha1.h"
#include "onvif_ptz.h"
#include "onvif_err.h"
#include "onvif_image.h"
#include "http_auth.h"
#include "base64.h"
#include "onvif_utils.h"
#include "onvif_probe.h"

#ifdef PROFILE_G_SUPPORT
#include "onvif_recording.h"
#endif

#ifdef CREDENTIAL_SUPPORT
#include "onvif_credential.h"
#endif

#ifdef ACCESS_RULES
#include "onvif_accessrules.h"
#endif

#ifdef HTTPS
#include "openssl/ssl.h"
#endif


/***************************************************************************************/
HD_AUTH_INFO        g_onvif_auth;

extern ONVIF_CFG    g_onvif_cfg;

extern int logEnable;
/***************************************************************************************/
char xml_hdr[] = "<?xml version=\"1.0\" encoding=\"UTF-8\"?>";

char onvif_xmlns[] = 
	"<s:Envelope "
    "xmlns:s=\"http://www.w3.org/2003/05/soap-envelope\" "
    "xmlns:e=\"http://www.w3.org/2003/05/soap-encoding\" "
    "xmlns:wsa=\"http://www.w3.org/2005/08/addressing\" "    
    "xmlns:xs=\"http://www.w3.org/2001/XMLSchema\" "
    "xmlns:xsi=\"http://www.w3.org/2001/XMLSchema-instance\" "
    "xmlns:wsaw=\"http://www.w3.org/2006/05/addressing/wsdl\" "
    "xmlns:wsnt=\"http://docs.oasis-open.org/wsn/b-2\" " 
    "xmlns:wstop=\"http://docs.oasis-open.org/wsn/t-1\" "     
    "xmlns:wsntw=\"http://docs.oasis-open.org/wsn/bw-2\" "
    "xmlns:wsrf-rw=\"http://docs.oasis-open.org/wsrf/rw-2\" "
    "xmlns:wsrf-r=\"http://docs.oasis-open.org/wsrf/r-2\" "
    "xmlns:wsrf-bf=\"http://docs.oasis-open.org/wsrf/bf-2\" " 
    "xmlns:wsdl=\"http://schemas.xmlsoap.org/wsdl\" "
    "xmlns:wsoap12=\"http://schemas.xmlsoap.org/wsdl/soap12\" "
    "xmlns:http=\"http://schemas.xmlsoap.org/wsdl/http\" " 
    "xmlns:d=\"http://schemas.xmlsoap.org/ws/2005/04/discovery\" "
    "xmlns:wsadis=\"http://schemas.xmlsoap.org/ws/2004/08/addressing\" "
    "xmlns:tt=\"http://www.onvif.org/ver10/schema\" " 
    "xmlns:tns1=\"http://www.onvif.org/ver10/topics\" "
    "xmlns:tds=\"http://www.onvif.org/ver10/device/wsdl\" " 
    "xmlns:trt=\"http://www.onvif.org/ver10/media/wsdl\" "
    "xmlns:tev=\"http://www.onvif.org/ver10/events/wsdl\" "    
    "xmlns:timg=\"http://www.onvif.org/ver20/imaging/wsdl\" "
    "xmlns:tst=\"http://www.onvif.org/ver10/storage/wsdl\" "
    "xmlns:dn=\"http://www.onvif.org/ver10/network/wsdl\" "
    
#ifdef MEDIA2_SUPPORT
    "xmlns:tr2=\"http://www.onvif.org/ver20/media/wsdl\" "
#endif
    
#ifdef PTZ_SUPPORT    
    "xmlns:tptz=\"http://www.onvif.org/ver20/ptz/wsdl\" "   
#endif   

#ifdef VIDEO_ANALYTICS
	"xmlns:tan=\"http://www.onvif.org/ver20/analytics/wsdl\" "
	"xmlns:axt=\"http://www.onvif.org/ver20/analytics\" "
#endif

#ifdef PROFILE_G_SUPPORT    
    "xmlns:trp=\"http://www.onvif.org/ver10/replay/wsdl\" "
    "xmlns:tse=\"http://www.onvif.org/ver10/search/wsdl\" "
    "xmlns:trc=\"http://www.onvif.org/ver10/recording/wsdl\" "    
#endif

#ifdef PROFILE_C_SUPPORT
    "xmlns:tac=\"http://www.onvif.org/ver10/accesscontrol/wsdl\" "
    "xmlns:tdc=\"http://www.onvif.org/ver10/doorcontrol/wsdl\" "
    "xmlns:pt=\"http://www.onvif.org/ver10/pacs\" "
#endif

#ifdef DEVICEIO_SUPPORT
	"xmlns:tmd=\"http://www.onvif.org/ver10/deviceIO/wsdl\" "
#endif

#ifdef THERMAL_SUPPORT
    "xmlns:tth=\"http://www.onvif.org/ver10/thermal/wsdl\" "
#endif

#ifdef CREDENTIAL_SUPPORT
    "xmlns:tcr=\"http://www.onvif.org/ver10/credential/wsdl\" "
#endif

#ifdef ACCESS_RULES
    "xmlns:tar=\"http://www.onvif.org/ver10/accessrules/wsdl\" "
#endif

#ifdef SCHEDULE_SUPPORT
    "xmlns:tsc=\"http://www.onvif.org/ver10/schedule/wsdl\" "
#endif

#ifdef RECEIVER_SUPPORT
    "xmlns:trv=\"http://www.onvif.org/ver10/receiver/wsdl\" "
#endif
    
	"xmlns:ter=\"http://www.onvif.org/ver10/error\">";

char soap_head[] = 
	"<s:Header>"
    	"<wsa:Action>%s</wsa:Action>"
	"</s:Header>";		

char soap_body[] = 
    "<s:Body>";

char soap_tailer[] =
    "</s:Body></s:Envelope>";


/***************************************************************************************/
int soap_http_rly(HTTPCLN * p_user, HTTPMSG * rx_msg, const char * p_xml, int len)
{
    int tlen;
	char * p_bufs;

	p_bufs = (char *)malloc(len + 1024);
	if (NULL == p_bufs)
	{
		return -1;
	}
	
	tlen = sprintf(p_bufs,	"HTTP/1.1 200 OK\r\n"
							"Server: hsoap/2.8\r\n"
							"Access-Control-Allow-Origin: *\r\n"
							"Access-Control-Allow-Methods: GET, POST, PUT, OPTIONS\r\n"
							"Access-Control-Allow-Headers: Content-Type, Authorization, X-Custom-Header\r\n"
							"Content-Type: %s\r\n"
							"Content-Length: %d\r\n"
							"Connection: close\r\n\r\n",
							http_get_headline(rx_msg, "Content-Type"), len);

	if (p_xml && len > 0)
	{
		memcpy(p_bufs+tlen, p_xml, len);
		tlen += len;
	}

	p_bufs[tlen] = '\0';
	log_print(LOG_DBG, "TX >> %s\r\n\r\n", p_bufs);

#ifndef HTTPS	
	send(p_user->cfd, p_bufs, tlen, 0);
#else
	if (g_onvif_cfg.https_enable)
	{
		SSL_write(p_user->ssl, p_bufs, tlen);
	}
	else
	{
		send(p_user->cfd, p_bufs, tlen, 0);
	}
#endif

	free(p_bufs);
	
	return tlen;
}

int soap_http_err_rly(HTTPCLN * p_user, HTTPMSG * rx_msg, int err_code, const char * err_str, const char * p_xml, int len)
{
    int tlen;
	char * p_bufs;
	char auth[256] = {'\0'};

	p_bufs = (char *)malloc(1024 * 16);
	if (NULL == p_bufs)
	{
		return -1;
	}

    if (g_onvif_cfg.need_auth)
    {
        if (g_onvif_auth.auth_nonce[0] == '\0')
        {
            sprintf(g_onvif_auth.auth_nonce, "%08X%08X", rand(), rand());
		    strcpy(g_onvif_auth.auth_qop, "auth");
		    strcpy(g_onvif_auth.auth_realm, "happytimesoft");
		}
		
        sprintf(auth, "WWW-Authenticate: Digest realm=\"%s\", qop=\"%s\", nonce=\"%s\"\r\n", 
            g_onvif_auth.auth_realm, g_onvif_auth.auth_qop, g_onvif_auth.auth_nonce);
    }
	
	tlen = sprintf(p_bufs,	"HTTP/1.1 %d %s\r\n"
							"Server: hsoap/2.8\r\n"
							"Access-Control-Allow-Origin: *\r\n"
							"Access-Control-Allow-Methods: GET, POST, PUT, OPTIONS\r\n"
							"Access-Control-Allow-Headers: Content-Type, Authorization, X-Custom-Header\r\n"
							"Content-Type: %s\r\n"
							"Content-Length: %d\r\n"
							"%s"
							"Connection: close\r\n\r\n",
							err_code, err_str,
							http_get_headline(rx_msg, "Content-Type"), len, auth);

	if (p_xml && len > 0)
	{
		memcpy(p_bufs+tlen, p_xml, len);
		tlen += len;
	}

	p_bufs[tlen] = '\0';
	log_print(LOG_DBG, "TX >> %s\r\n\r\n", p_bufs);

#ifndef HTTPS	
	send(p_user->cfd, p_bufs, tlen, 0);
#else
	if (g_onvif_cfg.https_enable)
	{
		SSL_write(p_user->ssl, p_bufs, tlen);
	}
	else
	{
		send(p_user->cfd, p_bufs, tlen, 0);
	}
#endif

	free(p_bufs);
	
	return tlen;
}

int soap_err_rly
(
HTTPCLN * p_user, 
HTTPMSG * rx_msg, 
const char * code, 
const char * subcode, 
const char * subcode_ex,
const char * reason,
const char * action,
int http_err_code, 
const char * http_err_str
)
{
    int ret = -1, mlen = 1024*16, xlen;
    char * p_xml;
    
	onvif_print("%s\r\n", __FUNCTION__);
	
	p_xml = (char *)malloc(mlen);
	if (NULL == p_xml)
	{
		goto soap_rly_err;
	}
	
	xlen = build_err_rly_xml(p_xml, mlen, code, subcode, subcode_ex, reason, action);
	if (xlen < 0 || xlen >= mlen)
	{
		goto soap_rly_err;
	}
	
	ret = soap_http_err_rly(p_user, rx_msg, http_err_code, http_err_str, p_xml, xlen);
	
soap_rly_err:

	if (p_xml)
	{
		free(p_xml);
	}
	
	return ret;
}

int soap_err_def_rly(HTTPCLN * p_user, HTTPMSG * rx_msg)
{
	return soap_err_rly(p_user, rx_msg, ERR_RECEIVER, ERR_ACTIONNOTSUPPORTED, NULL, "Action Not Implemented", NULL, 400, "Bad Request");
}

int soap_err_def2_rly(HTTPCLN * p_user, HTTPMSG * rx_msg, const char * code, const char * subcode, const char * subcode_ex, const char * reason)
{
	return soap_err_rly(p_user, rx_msg, code, subcode, subcode_ex, reason, NULL, 400, "Bad Request");
}

int soap_err_def3_rly(HTTPCLN * p_user, HTTPMSG * rx_msg, const char * code, const char * subcode, const char * subcode_ex, const char * reason, const char * action)
{
	return soap_err_rly(p_user, rx_msg, code, subcode, subcode_ex, reason, action, 400, "Bad Request");
}

int soap_security_rly(HTTPCLN * p_user, HTTPMSG * rx_msg, int errcode)
{
	onvif_print("%s\r\n", __FUNCTION__);

    return soap_err_rly(p_user, rx_msg, ERR_SENDER, ERR_NOTAUTHORIZED, NULL, "Sender not Authorized", NULL, errcode, "Not Authorized");
}

int soap_build_err_rly(HTTPCLN * p_user, HTTPMSG * rx_msg, ONVIF_RET err)
{
	int ret = 0;
	
	switch (err)
	{
	case ONVIF_ERR_InvalidIPv4Address:
		ret = soap_err_def2_rly(p_user, rx_msg, ERR_SENDER, ERR_INVALIDARGVAL, "ter:InvalidIPv4Address", "Invalid IPv4 Address");
		break;

	case ONVIF_ERR_InvalidIPv6Address:
		ret = soap_err_def2_rly(p_user, rx_msg, ERR_SENDER, ERR_INVALIDARGVAL, "ter:InvalidIPv6Address", "Invalid IPv6 Address");	
		break;

	case ONVIF_ERR_InvalidDnsName:
		ret = soap_err_def2_rly(p_user, rx_msg, ERR_SENDER, ERR_INVALIDARGVAL, "ter::InvalidDnsName", "Invalid DNS Name");	
		break;	

	case ONVIF_ERR_ServiceNotSupported:
		ret = soap_err_def2_rly(p_user, rx_msg, ERR_SENDER, ERR_INVALIDARGVAL, "ter:ServiceNotSupported", "Service Not Supported");	
		break;

	case ONVIF_ERR_PortAlreadyInUse:
		ret = soap_err_def2_rly(p_user, rx_msg, ERR_SENDER, ERR_INVALIDARGVAL, "ter:PortAlreadyInUse", "Port Already In Use");	
		break;	

	case ONVIF_ERR_InvalidGatewayAddress:
		ret = soap_err_def2_rly(p_user, rx_msg, ERR_SENDER, ERR_INVALIDARGVAL, "ter:InvalidGatewayAddress", "Invalid Gateway Address");	
		break;	

	case ONVIF_ERR_InvalidHostname:
		ret = soap_err_def2_rly(p_user, rx_msg, ERR_SENDER, ERR_INVALIDARGVAL, "ter:InvalidHostname", "Invalid Hostname");	
		break;	

	case ONVIF_ERR_MissingAttribute:
		ret = soap_err_def2_rly(p_user, rx_msg, ERR_SENDER, ERR_MISSINGATTR, NULL, "Missing Attribute");	
		break;	

	case ONVIF_ERR_InvalidDateTime:
		ret = soap_err_def2_rly(p_user, rx_msg, ERR_SENDER, ERR_INVALIDARGVAL, "ter:InvalidDateTime", "Invalid Datetime");	
		break;		

	case ONVIF_ERR_InvalidTimeZone:
		ret = soap_err_def2_rly(p_user, rx_msg, ERR_SENDER, ERR_INVALIDARGVAL, "ter:InvalidTimeZone", "Invalid Timezone");	
		break;	

	case ONVIF_ERR_ProfileExists:
		ret = soap_err_def2_rly(p_user, rx_msg, ERR_SENDER, ERR_INVALIDARGVAL, "ter:ProfileExists", "Profile Exist");	
		break;	

	case ONVIF_ERR_MaxNVTProfiles:
		ret = soap_err_def2_rly(p_user, rx_msg, ERR_RECEIVER, ERR_ACTION, "ter:MaxNVTProfiles", "Max Profiles");
		break;

	case ONVIF_ERR_NoProfile:
		ret = soap_err_def2_rly(p_user, rx_msg, ERR_SENDER, ERR_INVALIDARGVAL, "ter:NoProfile", "Profile Not Exist");
		break;

	case ONVIF_ERR_DeletionOfFixedProfile:
		ret = soap_err_def2_rly(p_user, rx_msg, ERR_SENDER, ERR_ACTION, "ter:DeletionOfFixedProfile", "Deleting Fixed Profile");
		break;

	case ONVIF_ERR_NoConfig:
		ret = soap_err_def2_rly(p_user, rx_msg, ERR_SENDER, ERR_INVALIDARGVAL, "ter:NoConfig", "Config Not Exist");
		break;

	case ONVIF_ERR_NoPTZProfile:
		ret = soap_err_def2_rly(p_user, rx_msg, ERR_SENDER, ERR_INVALIDARGVAL, "ter:NoPTZProfile", "PTZ Profile Not Exist");
		break;	

	case ONVIF_ERR_NoHomePosition:
		ret = soap_err_def2_rly(p_user, rx_msg, ERR_SENDER, ERR_ACTION, "ter:NoHomePosition", "No Home Position");
		break;	

	case ONVIF_ERR_NoToken:
		ret = soap_err_def2_rly(p_user, rx_msg, ERR_SENDER, ERR_ACTION, "ter:NoToken", "The requested token does not exist.");
		break;	

	case ONVIF_ERR_PresetExist:
		ret = soap_err_def2_rly(p_user, rx_msg, ERR_SENDER, ERR_ACTION, "ter:PresetExist", "The requested name already exist for another preset.");
		break;

	case ONVIF_ERR_TooManyPresets:
		ret = soap_err_def2_rly(p_user, rx_msg, ERR_SENDER, ERR_ACTION, "ter:TooManyPresets", "Maximum number of Presets reached.");
		break;	

	case ONVIF_ERR_MovingPTZ:
		ret = soap_err_def2_rly(p_user, rx_msg, ERR_SENDER, ERR_ACTION, "ter:MovingPTZ", "Preset cannot be set while PTZ unit is moving.");
		break;

	case ONVIF_ERR_NoEntity:
		ret = soap_err_def2_rly(p_user, rx_msg, ERR_SENDER, ERR_INVALIDARGVAL, "ter:NoEntity", "No such PTZ Node on the device");
		break;	

    case ONVIF_ERR_InvalidNetworkInterface:
        ret = soap_err_def2_rly(p_user, rx_msg, ERR_SENDER, ERR_INVALIDARGVAL, "ter:InvalidNetworkInterface", "The supplied network interface token does not exist");
		break;	

    case ONVIF_ERR_InvalidMtuValue:
        ret = soap_err_def2_rly(p_user, rx_msg, ERR_SENDER, ERR_INVALIDARGVAL, "ter:InvalidMtuValue", "The MTU value is invalid");
		break;	

    case ONVIF_ERR_ConfigModify:
        ret = soap_err_def2_rly(p_user, rx_msg, ERR_SENDER, ERR_INVALIDARGVAL, "ter:ConfigModify", "The configuration parameters are not possible to set");
		break;

	case ONVIF_ERR_ConfigurationConflict:
        ret = soap_err_def2_rly(p_user, rx_msg, ERR_SENDER, ERR_INVALIDARGVAL, "ter:ConfigurationConflict", "The new settings conflicts with other uses of the configuration");
		break;

	case ONVIF_ERR_InvalidPosition:
        ret = soap_err_def2_rly(p_user, rx_msg, ERR_SENDER, ERR_INVALIDARGVAL, "ter:InvalidPosition", "Invalid Postion");
		break;	

	case ONVIF_ERR_TooManyScopes:
		ret = soap_err_def2_rly(p_user, rx_msg, ERR_SENDER, ERR_INVALIDARGVAL, "ter:TooManyScopes", "The requested scope list exceeds the supported number of scopes");
		break;

	case ONVIF_ERR_FixedScope:
		ret = soap_err_def2_rly(p_user, rx_msg, ERR_SENDER, ERR_INVALIDARGVAL, "ter:FixedScope", "Trying to Remove fixed scope parameter, command rejected");
		break;

	case ONVIF_ERR_NoScope:
		ret = soap_err_def2_rly(p_user, rx_msg, ERR_SENDER, ERR_INVALIDARGVAL, "ter:NoScope", "Trying to Remove scope which does not exist");
		break;

	case ONVIF_ERR_ScopeOverwrite:
		ret = soap_err_def2_rly(p_user, rx_msg, ERR_SENDER, ERR_OPERATIONPROHIBITED, "ter:ScopeOverwrite", "Scope Overwrite");
		break;

    case ONVIF_ERR_ResourceUnknownFault:
        ret = soap_err_def3_rly(p_user, rx_msg, ERR_RECEIVER, "wsrf-rw:ResourceUnknownFault", NULL, "ResourceUnknownFault", "http://www.w3.org/2005/08/addressing/soap/fault");
        break;
        
	case ONVIF_ERR_NoSource:
		ret = soap_err_def2_rly(p_user, rx_msg, ERR_SENDER, ERR_INVALIDARGVAL, "ter:NoSource", "The requested VideoSource does not exist");
		break;

	case ONVIF_ERR_CannotOverwriteHome:
		ret = soap_err_def2_rly(p_user, rx_msg, ERR_RECEIVER, ERR_ACTION, "ter:CannotOverwriteHome", "The home position is fixed and cannot be overwritten");
		break;

	case ONVIF_ERR_SettingsInvalid:
		ret = soap_err_def2_rly(p_user, rx_msg, ERR_SENDER, ERR_INVALIDARGVAL, "ter:SettingsInvalid", "The requested settings are incorrect");
		break;

	case ONVIF_ERR_NoImagingForSource:
		ret = soap_err_def2_rly(p_user, rx_msg, ERR_RECEIVER, ERR_ACTIONNOTSUPPORTED, "ter:NoImagingForSource", "The requested VideoSource does not support imaging settings");
		break;

	case ONVIF_ERR_UsernameClash:
		ret = soap_err_def2_rly(p_user, rx_msg, ERR_SENDER, ERR_OPERATIONPROHIBITED, "ter:UsernameClash", "Username already exists");
		break;
		
	case ONVIF_ERR_PasswordTooLong:
		ret = soap_err_def2_rly(p_user, rx_msg, ERR_SENDER, ERR_OPERATIONPROHIBITED, "ter:PasswordTooLong", "The password is too long");
		break;
		
	case ONVIF_ERR_UsernameTooLong:
		ret = soap_err_def2_rly(p_user, rx_msg, ERR_SENDER, ERR_OPERATIONPROHIBITED, "ter:UsernameTooLong", "The username is too long");
		break;
		
	case ONVIF_ERR_Password:
		ret = soap_err_def2_rly(p_user, rx_msg, ERR_SENDER, ERR_OPERATIONPROHIBITED, "ter:Password", "Too weak password");
		break;
		
	case ONVIF_ERR_TooManyUsers:
		ret = soap_err_def2_rly(p_user, rx_msg, ERR_SENDER, ERR_ACTION, "ter:TooManyUsers", "Maximum number of supported users exceeded");
		break;
		
	case ONVIF_ERR_AnonymousNotAllowed:
		ret = soap_err_def2_rly(p_user, rx_msg, ERR_SENDER, ERR_OPERATIONPROHIBITED, "ter:AnonymousNotAllowed", "User level anonymous is not allowed");
		break;
		
	case ONVIF_ERR_UsernameTooShort:
		ret = soap_err_def2_rly(p_user, rx_msg, ERR_SENDER, ERR_OPERATIONPROHIBITED, "ter:UsernameTooShort", "The username is too short");
		break;
		
	case ONVIF_ERR_UsernameMissing:
		ret = soap_err_def2_rly(p_user, rx_msg, ERR_SENDER, ERR_INVALIDARGVAL, "ter:UsernameMissing", "Username not recognized");
		break;
		
	case ONVIF_ERR_FixedUser:
		ret = soap_err_def2_rly(p_user, rx_msg, ERR_SENDER, ERR_INVALIDARGVAL, "ter:FixedUser", "Username may not be deleted");
		break;

	case ONVIF_ERR_MaxOSDs:
		ret = soap_err_def2_rly(p_user, rx_msg, ERR_RECEIVER, ERR_ACTION, "ter:MaxOSDs", "The maximum number of supported OSDs has been reached");
		break;

	case ONVIF_ERR_InvalidStreamSetup:
		ret = soap_err_def2_rly(p_user, rx_msg, ERR_SENDER, ERR_INVALIDARGVAL, "ter:InvalidStreamSetup", "Specification of Stream Type or Transport part in StreamSetup is not supported");
		break;

	case ONVIF_ERR_BadConfiguration:
		ret = soap_err_def2_rly(p_user, rx_msg, ERR_SENDER, ERR_INVALIDARGVAL, "ter:BadConfiguration", "The configuration is invalid");
		break;
		
	case ONVIF_ERR_MaxRecordings:
		ret = soap_err_def2_rly(p_user, rx_msg, ERR_RECEIVER, ERR_ACTION, "ter:MaxRecordings", "Max recordings");
		break;
		
	case ONVIF_ERR_NoRecording:
		ret = soap_err_def2_rly(p_user, rx_msg, ERR_SENDER, ERR_INVALIDARGVAL, "ter:NoRecording", "The RecordingToken does not reference an existing recording");
		break;
		
	case ONVIF_ERR_CannotDelete:
		ret = soap_err_def2_rly(p_user, rx_msg, ERR_RECEIVER, ERR_ACTION, "ter:CannotDelete", "Can not delete");
		break;
		
	case ONVIF_ERR_MaxTracks:
		ret = soap_err_def2_rly(p_user, rx_msg, ERR_RECEIVER, ERR_ACTION, "ter:MaxTracks", "Max tracks");
		break;
		
	case ONVIF_ERR_NoTrack:
		ret = soap_err_def2_rly(p_user, rx_msg, ERR_SENDER, ERR_INVALIDARGVAL, "ter:NoTrack", "The TrackToken does not reference an existing track of the recording");
		break;
		
	case ONVIF_ERR_MaxRecordingJobs:
		ret = soap_err_def2_rly(p_user, rx_msg, ERR_RECEIVER, ERR_ACTION, "ter:MaxRecordingJobs", "The maximum number of recording jobs that the device can handle has been reached");
		break;
		
	case ONVIF_ERR_MaxReceivers:
		ret = soap_err_def2_rly(p_user, rx_msg, ERR_RECEIVER, ERR_ACTION, "ter:MaxReceivers", "Max receivers");
		break;
		
	case ONVIF_ERR_NoRecordingJob:
		ret = soap_err_def2_rly(p_user, rx_msg, ERR_SENDER, ERR_INVALIDARGVAL, "ter:NoRecordingJob", "The JobToken does not reference an existing job");
		break;
		
	case ONVIF_ERR_BadMode:
		ret = soap_err_def2_rly(p_user, rx_msg, ERR_SENDER, ERR_INVALIDARGVAL, "ter:BadMode", "The Mode is invalid");
		break;
		
	case ONVIF_ERR_InvalidToken:
		ret = soap_err_def2_rly(p_user, rx_msg, ERR_SENDER, ERR_INVALIDARGVAL, "ter:InvalidToken", "The Token is not valid");
		break;

    case ONVIF_ERR_InvalidRule:
        ret = soap_err_def2_rly(p_user, rx_msg, ERR_SENDER, ERR_INVALIDARGVAL, "ter:InvalidRule", "The suggested rules configuration is not valid on the device");
        break;
        
	case ONVIF_ERR_RuleAlreadyExistent:
	    ret = soap_err_def2_rly(p_user, rx_msg, ERR_SENDER, ERR_INVALIDARGVAL, "ter:RuleAlreadyExistent", "The same rule name exists already in the configuration");
	    break;
        
	case ONVIF_ERR_TooManyRules:
	    ret = soap_err_def2_rly(p_user, rx_msg, ERR_RECEIVER, ERR_ACTION, "ter:TooManyRules", "There is not enough space in the device to add the rules to the configuration");
	    break;
        
	case ONVIF_ERR_RuleNotExistent:
	    ret = soap_err_def2_rly(p_user, rx_msg, ERR_SENDER, ERR_INVALIDARGVAL, "ter:RuleNotExistent", "The rule name or names do not exist");
	    break;
        
	case ONVIF_ERR_NameAlreadyExistent:
	    ret = soap_err_def2_rly(p_user, rx_msg, ERR_SENDER, ERR_INVALIDARGVAL, "ter:NameAlreadyExistent", "The same analytics module name exists already in the configuration");
	    break;
        
	case ONVIF_ERR_TooManyModules:
	    ret = soap_err_def2_rly(p_user, rx_msg, ERR_RECEIVER, ERR_ACTION, "ter:TooManyModules", "There is not enough space in the device to add the analytics modules to the configuration");
	    break;
        
	case ONVIF_ERR_InvalidModule:
	    ret = soap_err_def2_rly(p_user, rx_msg, ERR_SENDER, ERR_INVALIDARGVAL, "ter:InvalidModule", "The suggested module configuration is not valid on the device");
	    break;
        
	case ONVIF_ERR_NameNotExistent:
	    ret = soap_err_def2_rly(p_user, rx_msg, ERR_SENDER, ERR_INVALIDARGVAL, "ter:NameNotExistent", "The analytics module with the requested name does not exist");
        break;
        
	case ONVIF_ERR_InvalidFilterFault:
	    ret = soap_err_def3_rly(p_user, rx_msg, ERR_SENDER, ERR_INVALIDARGVAL, "wsntw:InvalidFilterFault", "InvalidFilterFault", "http://www.w3.org/2005/08/addressing/soap/fault");
	    break;
        
	case ONVIF_ERR_InvalidTopicExpressionFault:
	    ret = soap_err_def3_rly(p_user, rx_msg, ERR_SENDER, ERR_INVALIDARGVAL, "wsntw:InvalidTopicExpressionFault", "InvalidTopicExpressionFault", "http://www.w3.org/2005/08/addressing/soap/fault");
	    break;
        
	case ONVIF_ERR_TopicNotSupportedFault:
	    ret = soap_err_def3_rly(p_user, rx_msg, ERR_SENDER, ERR_INVALIDARGVAL, "wsntw:TopicNotSupportedFault", "TopicNotSupportedFault", "http://www.w3.org/2005/08/addressing/soap/fault");
	    break;
        
	case ONVIF_ERR_InvalidMessageContentExpressionFault:
	    ret = soap_err_def3_rly(p_user, rx_msg, ERR_SENDER, ERR_INVALIDARGVAL, "wsntw:InvalidMessageContentExpressionFault", "InvalidMessageContentExpressionFault", "http://www.w3.org/2005/08/addressing/soap/fault");
	    break;

    case ONVIF_ERR_InvalidStartReference:
        ret = soap_err_def2_rly(p_user, rx_msg, ERR_SENDER, ERR_INVALIDARGVAL, "ter::InvalidStartReference", "StartReference is invalid");
        break;
        
    case ONVIF_ERR_TooManyItems:
        ret = soap_err_def2_rly(p_user, rx_msg, ERR_SENDER, ERR_INVALIDARGVAL, "ter::TooManyItems", "Too many items were requested, see MaxLimit capability");
        break;
        
    case ONVIF_ERR_NotFound:
        ret = soap_err_def2_rly(p_user, rx_msg, ERR_SENDER, ERR_INVALIDARGVAL, "ter:NotFound", "Not found");
        break;

    case ONVIF_ERR_NotSupported:
        ret = soap_err_def2_rly(p_user, rx_msg, ERR_RECEIVER, ERR_ACTIONNOTSUPPORTED, "ter:NotSupported", "The operation is not supported");
        break;

    case ONVIF_ERR_Failure:
        ret = soap_err_def2_rly(p_user, rx_msg, ERR_RECEIVER, ERR_ACTION, "ter:Failure", "Failed to go to Accessed state and unlock the door");
        break;

    case ONVIF_ERR_NoVideoOutput:
        ret = soap_err_def2_rly(p_user, rx_msg, ERR_SENDER, ERR_INVALIDARGVAL, "ter:NoVideoOutput", "The requested VideoOutput indicated with VideoOutputToken does not exist");
        break;

    case ONVIF_ERR_NoAudioOutput:
        ret = soap_err_def2_rly(p_user, rx_msg, ERR_SENDER, ERR_INVALIDARGVAL, "ter:NoAudioOutput", "The requested AudioOutput indicated with AudioOutputToken does not exist");
        break;

    case ONVIF_ERR_RelayToken:
        ret = soap_err_def2_rly(p_user, rx_msg, ERR_SENDER, ERR_INVALIDARGVAL, "ter:RelayToken", "Unknown relay token reference");
        break;

    case ONVIF_ERR_ModeError:
        ret = soap_err_def2_rly(p_user, rx_msg, ERR_SENDER, ERR_INVALIDARGVAL, "ter:ModeError", "Monostable delay time not valid");
        break;

    case ONVIF_ERR_InvalidSerialPort:
        ret = soap_err_def2_rly(p_user, rx_msg, ERR_SENDER, ERR_INVALIDARGVAL, "ter:InvalidSerialPort", "The supplied serial port token does not exist");
        break;

    case ONVIF_ERR_DataLengthOver:
        ret = soap_err_def2_rly(p_user, rx_msg, ERR_SENDER, ERR_INVALIDARGVAL, "ter:DataLengthOver", "Number of available bytes exceeded");
        break;

    case ONVIF_ERR_DelimiterNotSupported:
        ret = soap_err_def2_rly(p_user, rx_msg, ERR_SENDER, ERR_INVALIDARGVAL, "ter:DelimiterNotSupported", "Sequence of character (delimiter) is not supported");
        break;

    case ONVIF_ERR_InvalidDot11:
        ret = soap_err_def2_rly(p_user, rx_msg, ERR_RECEIVER, ERR_ACTIONNOTSUPPORTED, "ter:InvalidDot11", "IEEE 802.11 configuration is not supported");
        break;

    case ONVIF_ERR_NotDot11:
        ret = soap_err_def2_rly(p_user, rx_msg, ERR_SENDER, ERR_INVALIDARGVAL, "ter:NotDot11", "The interface is not an IEEE 802.11 interface");
        break;

    case ONVIF_ERR_NotConnectedDot11:
        ret = soap_err_def2_rly(p_user, rx_msg, ERR_RECEIVER, ERR_ACTION, "ter:NotConnectedDot11", "IEEE 802.11 network is not connected");
        break;

    case ONVIF_ERR_NotScanAvailable:
        ret = soap_err_def2_rly(p_user, rx_msg, ERR_RECEIVER, ERR_ACTIONNOTSUPPORTED, "ter:NotScanAvailable", "ScanAvailableDot11Networks is not supported");
        break;

    case ONVIF_ERR_NotRemoteUser:
        ret = soap_err_def2_rly(p_user, rx_msg, ERR_RECEIVER, ERR_ACTIONNOTSUPPORTED, "ter:NotRemoteUser", "Remote User handling is not supported");
        break;

    case ONVIF_ERR_NoVideoSource:
        ret = soap_err_def2_rly(p_user, rx_msg, ERR_SENDER, ERR_INVALIDARGVAL, "ter:NoVideoSource", "The requested video source does not exist");
        break;

    case ONVIF_ERR_NoVideoSourceMode:
        ret = soap_err_def2_rly(p_user, rx_msg, ERR_SENDER, ERR_INVALIDARGVAL, "ter:NoVideoSourceMode", "The requested video source mode does not exist");
        break;

    case ONVIF_ERR_NoThermalForSource:
        ret = soap_err_def2_rly(p_user, rx_msg, ERR_RECEIVER, ERR_ACTIONNOTSUPPORTED, "ter:NoThermalForSource", "The requested VideoSource does not support thermal configuration");
        break;
        
    case ONVIF_ERR_NoRadiometryForSource:
        ret = soap_err_def2_rly(p_user, rx_msg, ERR_RECEIVER, ERR_ACTIONNOTSUPPORTED, "ter:NoRadiometryForSource", "The requested VideoSource does not support radiometry config settings");
        break;
        
    case ONVIF_ERR_InvalidConfiguration:
        ret = soap_err_def2_rly(p_user, rx_msg, ERR_SENDER, ERR_INVALIDARGVAL, "ter:InvalidConfiguration", "The requested configuration is incorrect");
        break;

    case ONVIF_ERR_MaxAccessProfilesPerCredential:
        ret = soap_err_def2_rly(p_user, rx_msg, ERR_SENDER, ERR_CAPABILITYVIOLATED, "ter:MaxAccessProfilesPerCredential", "There are too many access profiles per credential");
        break;
        
    case ONVIF_ERR_CredentialValiditySupported:
        ret = soap_err_def2_rly(p_user, rx_msg, ERR_SENDER, ERR_CAPABILITYVIOLATED, "ter:CredentialValiditySupported", "Credential validity is not supported by device");
        break;
        
    case ONVIF_ERR_CredentialAccessProfileValiditySupported:
        ret = soap_err_def2_rly(p_user, rx_msg, ERR_SENDER, ERR_CAPABILITYVIOLATED, "ter:CredentialAccessProfileValiditySupported", "Credential access profile validity is not supported by the device");
        break;
        
    case ONVIF_ERR_SupportedIdentifierType:
        ret = soap_err_def2_rly(p_user, rx_msg, ERR_SENDER, ERR_CAPABILITYVIOLATED, "ter:SupportedIdentifierType", "Specified identifier type is not supported by device");
        break;
        
    case ONVIF_ERR_DuplicatedIdentifierType:
        ret = soap_err_def2_rly(p_user, rx_msg, ERR_SENDER, ERR_CAPABILITYVIOLATED, "ter:DuplicatedIdentifierType", "The same identifier type was used more than once");
        break;
        
    case ONVIF_ERR_InvalidFormatType:
        ret = soap_err_def2_rly(p_user, rx_msg, ERR_SENDER, ERR_INVALIDARGVAL, "ter:InvalidFormatType", "Specified identifier format type is not supported by the device");
        break;
        
    case ONVIF_ERR_InvalidIdentifierValue:
        ret = soap_err_def2_rly(p_user, rx_msg, ERR_SENDER, ERR_INVALIDARGVAL, "ter:InvalidIdentifierValue", "Specified identifier value is not as per FormatType definition");
        break;
        
    case ONVIF_ERR_DuplicatedIdentifierValue:
        ret = soap_err_def2_rly(p_user, rx_msg, ERR_SENDER, ERR_INVALIDARGVAL, "ter:DuplicatedIdentifierValue", "The same combination of identifier type, format and value was used more than once");
        break;
        
    case ONVIF_ERR_ReferenceNotFound:
        ret = soap_err_def2_rly(p_user, rx_msg, ERR_SENDER, ERR_INVALIDARGVAL, "ter:ReferenceNotFound", "A referred entity token is not found");
        break;
        
    case ONVIF_ERR_ExemptFromAuthenticationSupported:
        ret = soap_err_def2_rly(p_user, rx_msg, ERR_SENDER, ERR_INVALIDARGVAL, "ter:ExemptFromAuthenticationSupported", "Exempt from authentication is not supported by the device");
        break;

    case ONVIF_ERR_MaxCredentials:
        ret = soap_err_def2_rly(p_user, rx_msg, ERR_RECEIVER, ERR_CAPABILITYVIOLATED, "ter:MaxCredentials", "There is not enough space to create a new credential");
        break;

    case ONVIF_ERR_ReferenceInUse:
        ret = soap_err_def2_rly(p_user, rx_msg, ERR_SENDER, ERR_INVALIDARGVAL, "ter:ReferenceInUse", "Failed to delete, credential token is in use");
        break;

    case ONVIF_ERR_MinIdentifiersPerCredential:
        ret = soap_err_def2_rly(p_user, rx_msg, ERR_RECEIVER, ERR_CONSTRAINTVIOLATED, "ter:MinIdentifiersPerCredential", "At least one credential identifier is required");
        break;

    case ONVIF_ERR_InvalidArgs:
        ret = soap_err_def2_rly(p_user, rx_msg, ERR_SENDER, ERR_INVALIDARGS, NULL, "InvalidArgs");
        break;

    case ONVIF_ERR_MaxAccessProfiles:
        ret = soap_err_def2_rly(p_user, rx_msg, ERR_RECEIVER, ERR_CAPABILITYVIOLATED, "ter:MaxAccessProfiles", "There is not enough space to add new AccessProfile, see the MaxAccessProfiles capability");
        break;
        
    case ONVIF_ERR_MaxAccessPoliciesPerAccessProfile:
        ret = soap_err_def2_rly(p_user, rx_msg, ERR_SENDER, ERR_CAPABILITYVIOLATED, "ter:MaxAccessPoliciesPerAccessProfile", "There are too many AccessPolicies in anAccessProfile, see MaxAccessPoliciesPerAccessProfile capability");
        break;
        
    case ONVIF_ERR_MultipleSchedulesPerAccessPointSupported:
        ret = soap_err_def2_rly(p_user, rx_msg, ERR_SENDER, ERR_CAPABILITYVIOLATED, "ter:MultipleSchedulesPerAccessPointSupported", "Multiple AccessPoints are not supported for the same schedule, see MultipleSchedulesPerAccessPointSupported capability");
        break;

    case ONVIF_ERR_InvalidArgVal:
        ret = soap_err_def2_rly(p_user, rx_msg, ERR_SENDER, ERR_INVALIDARGVAL, NULL, "InvalidArgVal");
        break;

    case ONVIF_ERR_MaxSchedules:
        ret = soap_err_def2_rly(p_user, rx_msg, ERR_RECEIVER, ERR_CAPABILITYVIOLATED, "ter:MaxSchedules", "There is not enough space to add new schedule, see MaxSchedules capability");
        break;
        
    case ONVIF_ERR_MaxSpecialDaysSchedules:
        ret = soap_err_def2_rly(p_user, rx_msg, ERR_SENDER, ERR_CAPABILITYVIOLATED, "ter:MaxSpecialDaysSchedule", "There are too many SpecialDaysSchedule entities referred in this schedule, see MaxSpecialDaysSchedules capability");
        break;
        
    case ONVIF_ERR_MaxTimePeriodsPerDay: 
        ret = soap_err_def2_rly(p_user, rx_msg, ERR_SENDER, ERR_CAPABILITYVIOLATED, "ter:MaxTimePeriodsPerDay", "There are too many time periods in a day schedule, see MaxTimePeriodsPerDay capability");
        break;
        
    case ONVIF_ERR_MaxSpecialDayGroups:
        ret = soap_err_def2_rly(p_user, rx_msg, ERR_RECEIVER, ERR_CAPABILITYVIOLATED, "ter:MaxSpecialDayGroups", "There is not enough space to add new SpecialDayGroup items, see the MaxSpecialDayGroups capabilit");
        break;
        
    case ONVIF_ERR_MaxDaysInSpecialDayGroup:
        ret = soap_err_def2_rly(p_user, rx_msg, ERR_SENDER, ERR_CAPABILITYVIOLATED, "ter:MaxDaysInSpecialDayGroup", "There are too many special days in a SpecialDayGroup, see MaxDaysInSpecialDayGroup capability");
        break;

    case ONVIF_ERR_UnknownToken:
        ret = soap_err_def2_rly(p_user, rx_msg, ERR_SENDER, ERR_INVALIDARGVAL, "ter:UnknownToken", "The receiver indicated by ReceiverToken does not exist");
        break;
        
    case ONVIF_ERR_CannotDeleteReceiver:
        ret = soap_err_def2_rly(p_user, rx_msg, ERR_RECEIVER, ERR_ACTION, "ter:CannotDeleteReceiver", "It is not possible to delete the specified receiver");
        break;   

    case ONVIF_ERR_MaxMasks:
        ret = soap_err_def2_rly(p_user, rx_msg, ERR_RECEIVER, ERR_ACTION, "ter:MaxMasks", "The maximum number of supported masks by the specific VideoSourceConfiguration has been reached");
        break;
        
	default:
		ret = soap_err_def_rly(p_user, rx_msg);
		break;
	}

	return ret;
}


typedef int (*soap_build_xml)(char * p_buf, int mlen, const char * argv);


int soap_build_header(char * p_xml, int mlen, const char * action, XMLN * p_header)
{
    int offset = 0;
    
    offset += snprintf(p_xml, mlen, xml_hdr);
	offset += snprintf(p_xml+offset, mlen-offset, onvif_xmlns);

	if (p_header)
	{
	    XMLN * p_MessageID;
	    XMLN * p_ReplyTo;

        offset += snprintf(p_xml+offset, mlen-offset, "<s:Header>");
        
	    p_MessageID = xml_node_soap_get(p_header, "MessageID");
	    if (p_MessageID && p_MessageID->data)
	    {
	        offset += snprintf(p_xml+offset, mlen-offset, 
	            "<wsa:MessageID>%s</wsa:MessageID>",
	            p_MessageID->data);
	    }

	    p_ReplyTo = xml_node_soap_get(p_header, "ReplyTo");
	    if (p_ReplyTo)
	    {
	        XMLN * p_Address;

	        p_Address = xml_node_soap_get(p_ReplyTo, "Address");
	        if (p_Address && p_Address->data)
	        {
	            offset += snprintf(p_xml+offset, mlen-offset, 
    	            "<wsa:To>%s</wsa:To>",
    	            p_Address->data);
	        }
	    }

	    if (action)
    	{
    	    offset += snprintf(p_xml+offset, mlen-offset, "<wsa:Action>%s</wsa:Action>", action);
    	}

	    offset += snprintf(p_xml+offset, mlen-offset, "</s:Header>");
	}	
	else if (action)
	{
	    offset += snprintf(p_xml+offset, mlen-offset, soap_head, action);
	}

	return offset;
}

ONVIF_RET soap_build_send_rly(HTTPCLN * p_user, HTTPMSG * rx_msg, soap_build_xml build_xml, const char * argv, const char * action, XMLN * p_header)
{
    int offset = 0;
	int ret = -1, mlen = 1024*40, xlen;
	
	char * p_xml = (char *)malloc(mlen);
	if (NULL == p_xml)
	{
		return (ONVIF_RET)-1;
	}

    offset += soap_build_header(p_xml, mlen, action, p_header);
	
	offset += snprintf(p_xml+offset, mlen-offset, soap_body);
	
	xlen = build_xml(p_xml+offset, mlen-offset, argv);
	if (xlen < 0)
	{
		ret = soap_build_err_rly(p_user, rx_msg, (ONVIF_RET)xlen);
	}
    else
    {
        offset += xlen;
	    offset += snprintf(p_xml+offset, mlen-offset, soap_tailer);

	    ret = soap_http_rly(p_user, rx_msg, p_xml, offset);
	}

	free(p_xml);
	
	return (ONVIF_RET)ret;
}

int soap_GetDeviceInformation(HTTPCLN * p_user, HTTPMSG * rx_msg, XMLN * p_body, XMLN * p_header)
{
	onvif_print("%s\r\n", __FUNCTION__);

	return soap_build_send_rly(p_user, rx_msg, build_GetDeviceInformation_rly_xml, NULL, NULL, p_header);
}

int soap_GetSystemUris(HTTPCLN * p_user, HTTPMSG * rx_msg, XMLN * p_body, XMLN * p_header)
{
    ONVIF_RET ret;
    GetSystemUris_RES res;
    
    onvif_print("%s\r\n", __FUNCTION__);

    memset(&res, 0, sizeof(res));
    
    ret = onvif_GetSystemUris(p_user->lip, p_user->lport, &res);
    if (ONVIF_OK == ret)
    {
        return soap_build_send_rly(p_user, rx_msg, build_GetSystemUris_rly_xml, (char*)&res, NULL, p_header);
    }
        
    return soap_build_err_rly(p_user, rx_msg, ret);
}

int soap_GetCapabilities(HTTPCLN * p_user, HTTPMSG * rx_msg, XMLN * p_body, XMLN * p_header)
{
    ONVIF_RET ret;
    XMLN * p_GetCapabilities;
    GetCapabilities_REQ req;
    
    onvif_print("%s\r\n", __FUNCTION__);

    p_GetCapabilities = xml_node_soap_get(p_body, "GetCapabilities");
    assert(p_GetCapabilities);

    memset(&req, 0, sizeof(req));

    ret = parse_GetCapabilities(p_GetCapabilities, &req);
    if (ONVIF_OK == ret)
    {
	    if (CapabilityCategory_Invalid       == req.Category 
#ifdef PTZ_SUPPORT	    
	    	|| (CapabilityCategory_PTZ       == req.Category && g_onvif_cfg.Capabilities.ptz.support == 0)
#else
            || (CapabilityCategory_PTZ       == req.Category) 
#endif	    	
	    	|| (CapabilityCategory_Media     == req.Category && g_onvif_cfg.Capabilities.media.support == 0)
	    	|| (CapabilityCategory_Events    == req.Category && g_onvif_cfg.Capabilities.events.support == 0)
	    	|| (CapabilityCategory_Imaging   == req.Category && g_onvif_cfg.Capabilities.image.support == 0)
#ifdef VIDEO_ANALYTICS
			|| (CapabilityCategory_Analytics == req.Category && g_onvif_cfg.Capabilities.analytics.support == 0)
#else
            || (CapabilityCategory_Analytics == req.Category)
#endif
	    	)
	    {
	    	return soap_err_def2_rly(p_user, rx_msg, ERR_RECEIVER, ERR_ACTIONNOTSUPPORTED, "ter:NoSuchService", "No Such Service");
	    }
	    else
	    {
	        return soap_build_send_rly(p_user, rx_msg, build_GetCapabilities_rly_xml, (char *)&req, NULL, p_header);
	    }
    }
		
    return soap_build_err_rly(p_user, rx_msg, ret);
}


int soap_GetProfiles(HTTPCLN * p_user, HTTPMSG * rx_msg, XMLN * p_body, XMLN * p_header)
{
	onvif_print("%s\r\n", __FUNCTION__);

	return soap_build_send_rly(p_user, rx_msg, build_GetProfiles_rly_xml, NULL, NULL, p_header);
}

int soap_GetProfile(HTTPCLN * p_user, HTTPMSG * rx_msg, XMLN * p_body, XMLN * p_header)
{
    ONVIF_RET ret;
    XMLN * p_GetProfile;
    GetProfile_REQ req;
    
	onvif_print("%s\r\n", __FUNCTION__);

    p_GetProfile = xml_node_soap_get(p_body, "GetProfile");
    assert(p_GetProfile);

    memset(&req, 0, sizeof(req));
    
    ret = parse_GetProfile(p_GetProfile, &req);
    if (ONVIF_OK == ret)
	{
		return soap_build_send_rly(p_user, rx_msg, build_GetProfile_rly_xml, (char *)&req, NULL, p_header);
	}

	return soap_build_err_rly(p_user, rx_msg, ret);
}

int soap_CreateProfile(HTTPCLN * p_user, HTTPMSG * rx_msg, XMLN * p_body, XMLN * p_header)
{
    ONVIF_RET ret;
    XMLN * p_CreateProfile;
    CreateProfile_REQ req;
    
	onvif_print("%s\r\n", __FUNCTION__);

	p_CreateProfile = xml_node_soap_get(p_body, "CreateProfile");
    assert(p_CreateProfile);
    
    memset(&req, 0, sizeof(req));

    ret = parse_CreateProfile(p_CreateProfile, &req);
    if (ONVIF_OK == ret)
    {
    	ret = onvif_CreateProfile(p_user->lip, p_user->lport, &req);
    	if (ONVIF_OK == ret)
    	{
    		return soap_build_send_rly(p_user, rx_msg, build_CreateProfile_rly_xml, req.Token, NULL, p_header);
    	}
    }

    return soap_build_err_rly(p_user, rx_msg, ret);
}

int soap_DeleteProfile(HTTPCLN * p_user, HTTPMSG * rx_msg, XMLN * p_body, XMLN * p_header)
{
    ONVIF_RET ret;
    XMLN * p_DeleteProfile;    
    DeleteProfile_REQ req;
    
	onvif_print("%s\r\n", __FUNCTION__);

	p_DeleteProfile = xml_node_soap_get(p_body, "DeleteProfile");
    assert(p_DeleteProfile);

	memset(&req, 0, sizeof(req));
	
    ret = parse_DeleteProfile(p_DeleteProfile, &req);
    if (ONVIF_OK == ret)
    {
    	ret = onvif_DeleteProfile(&req);
    	if (ONVIF_OK == ret)
    	{
    		return soap_build_send_rly(p_user, rx_msg, build_DeleteProfile_rly_xml, NULL, NULL, p_header);
    	}    	
    }

    return soap_build_err_rly(p_user, rx_msg, ret);
}

int soap_AddVideoSourceConfiguration(HTTPCLN * p_user, HTTPMSG * rx_msg, XMLN * p_body, XMLN * p_header)
{
    ONVIF_RET ret;
    XMLN * p_AddVideoSourceConfiguration;
    AddVideoSourceConfiguration_REQ req;
    
	onvif_print("%s\r\n", __FUNCTION__);

	p_AddVideoSourceConfiguration = xml_node_soap_get(p_body, "AddVideoSourceConfiguration");
    assert(p_AddVideoSourceConfiguration);
	
	memset(&req, 0, sizeof(req));
	
    ret = parse_AddVideoSourceConfiguration(p_AddVideoSourceConfiguration, &req);
    if (ONVIF_OK == ret)
    {
    	ret = onvif_AddVideoSourceConfiguration(&req);
    	if (ONVIF_OK == ret)
    	{
    		return soap_build_send_rly(p_user, rx_msg, build_AddVideoSourceConfiguration_rly_xml, NULL, NULL, p_header);
    	} 	
    }

    return soap_build_err_rly(p_user, rx_msg, ret);
}

int soap_RemoveVideoSourceConfiguration(HTTPCLN * p_user, HTTPMSG * rx_msg, XMLN * p_body, XMLN * p_header)
{
    ONVIF_RET ret;
    XMLN * p_RemoveVideoSourceConfiguration;
    RemoveVideoSourceConfiguration_REQ req;
    
	onvif_print("%s\r\n", __FUNCTION__);
	
	p_RemoveVideoSourceConfiguration = xml_node_soap_get(p_body, "RemoveVideoSourceConfiguration");
	assert(p_RemoveVideoSourceConfiguration);

	memset(&req, 0, sizeof(req));
	
    ret = parse_RemoveVideoSourceConfiguration(p_RemoveVideoSourceConfiguration, &req);
    if (ONVIF_OK == ret)
    {
    	ret = onvif_RemoveVideoSourceConfiguration(&req);
    	if (ONVIF_OK == ret)
    	{
    		return soap_build_send_rly(p_user, rx_msg, build_RemoveVideoSourceConfiguration_rly_xml, NULL, NULL, p_header);
    	}
    }

    return soap_build_err_rly(p_user, rx_msg, ret);
}

int soap_AddVideoEncoderConfiguration(HTTPCLN * p_user, HTTPMSG * rx_msg, XMLN * p_body, XMLN * p_header)
{
    ONVIF_RET ret;
    XMLN * p_AddVideoEncoderConfiguration;
    AddVideoEncoderConfiguration_REQ req;
    
	onvif_print("%s\r\n", __FUNCTION__);

	p_AddVideoEncoderConfiguration = xml_node_soap_get(p_body, "AddVideoEncoderConfiguration");
	assert(p_AddVideoEncoderConfiguration);
	
	memset(&req, 0, sizeof(req));
	
	ret = parse_AddVideoEncoderConfiguration(p_AddVideoEncoderConfiguration, &req);
	if (ONVIF_OK == ret)
	{
		ret = onvif_AddVideoEncoderConfiguration(&req);
		if (ONVIF_OK == ret)
		{
			return soap_build_send_rly(p_user, rx_msg, build_AddVideoEncoderConfiguration_rly_xml, NULL, NULL, p_header);
		}
	}
	
	return soap_build_err_rly(p_user, rx_msg, ret);
}

int soap_RemoveVideoEncoderConfiguration(HTTPCLN * p_user, HTTPMSG * rx_msg, XMLN * p_body, XMLN * p_header)
{
    ONVIF_RET ret;
    XMLN * p_RemoveVideoEncoderConfiguration;
    RemoveVideoEncoderConfiguration_REQ req;
    
	onvif_print("%s\r\n", __FUNCTION__);
	
	p_RemoveVideoEncoderConfiguration = xml_node_soap_get(p_body, "RemoveVideoEncoderConfiguration");
	assert(p_RemoveVideoEncoderConfiguration);	

	memset(&req, 0, sizeof(req));
	
	ret = parse_RemoveVideoEncoderConfiguration(p_RemoveVideoEncoderConfiguration, &req);
	if (ONVIF_OK == ret)
    {
    	ret = onvif_RemoveVideoEncoderConfiguration(&req);
    	if (ONVIF_OK == ret)
    	{
    		return soap_build_send_rly(p_user, rx_msg, build_RemoveVideoEncoderConfiguration_rly_xml, NULL, NULL, p_header);
    	}
    }

    return soap_build_err_rly(p_user, rx_msg, ret);
}

int soap_GetSystemDateAndTime(HTTPCLN * p_user, HTTPMSG * rx_msg, XMLN * p_body, XMLN * p_header)
{
	onvif_print("%s\r\n", __FUNCTION__);

	return soap_build_send_rly(p_user, rx_msg, build_GetSystemDateAndTime_rly_xml, NULL, NULL, p_header);
}

int soap_GetStreamUri(HTTPCLN * p_user, HTTPMSG * rx_msg, XMLN * p_body, XMLN * p_header)
{
    ONVIF_RET ret;
    XMLN * p_GetStreamUri;
    GetStreamUri_REQ req;
    
	onvif_print("%s\r\n", __FUNCTION__);

    p_GetStreamUri = xml_node_soap_get(p_body, "GetStreamUri");
    assert(p_GetStreamUri);
	
	memset(&req, 0, sizeof(req));

	ret = parse_GetStreamUri(p_GetStreamUri, &req);
	if (ONVIF_OK == ret)
	{
	    GetStreamUri_RES res;
	    memset(&res, 0, sizeof(res));
	    
		ret = onvif_GetStreamUri(p_user->lip, p_user->lport, &req, &res);
		if (ONVIF_OK == ret)
		{
			return soap_build_send_rly(p_user, rx_msg, build_GetStreamUri_rly_xml, (char *)&res, NULL, p_header);
		}
	}
	
	return soap_build_err_rly(p_user, rx_msg, ret);
}


int soap_GetSnapshotUri(HTTPCLN * p_user, HTTPMSG * rx_msg, XMLN * p_body, XMLN * p_header)
{
    ONVIF_RET ret;
    XMLN * p_GetSnapshotUri;    
    GetSnapshotUri_REQ req;
    
    onvif_print("%s\r\n", __FUNCTION__);

    p_GetSnapshotUri = xml_node_soap_get(p_body, "GetSnapshotUri");
    assert(p_GetSnapshotUri);

    memset(&req, 0, sizeof(req));

    ret = parse_GetSnapshotUri(p_GetSnapshotUri, &req);
	if (ONVIF_OK == ret)
	{
	    GetSnapshotUri_RES res;
	    memset(&res, 0, sizeof(res));
	    
		ret = onvif_GetSnapshotUri(p_user->lip, p_user->lport, &req, &res);
		if (ONVIF_OK == ret)
		{
			return soap_build_send_rly(p_user, rx_msg, build_GetSnapshotUri_rly_xml, (char *)&res, NULL, p_header);
		}
	}
	
	return soap_build_err_rly(p_user, rx_msg, ret);
}


int soap_GetNetworkInterfaces(HTTPCLN * p_user, HTTPMSG * rx_msg, XMLN * p_body, XMLN * p_header)
{
	onvif_print("%s\r\n", __FUNCTION__);

	return soap_build_send_rly(p_user, rx_msg, build_GetNetworkInterfaces_rly_xml, NULL, NULL, p_header);
}

int soap_SetNetworkInterfaces(HTTPCLN * p_user, HTTPMSG * rx_msg, XMLN * p_body, XMLN * p_header)
{
    ONVIF_RET ret;
    XMLN * p_SetNetworkInterfaces;
    SetNetworkInterfaces_REQ req;
    
	onvif_print("%s\r\n", __FUNCTION__);

	p_SetNetworkInterfaces = xml_node_soap_get(p_body, "SetNetworkInterfaces");
    assert(p_SetNetworkInterfaces);
	
	memset(&req, 0, sizeof(req));
	
	ret = parse_SetNetworkInterfaces(p_SetNetworkInterfaces, &req);
	if (ONVIF_OK == ret)
	{
		ret = onvif_SetNetworkInterfaces(&req);
		if (ONVIF_OK == ret)
		{
			ret = soap_build_send_rly(p_user, rx_msg, build_SetNetworkInterfaces_rly_xml, NULL, NULL, p_header);
		}
	}

    /*if (ret > 0)
    {
	    // send onvif hello message 
	    sleep(3);
        onvif_hello();
        return ret;
    }*/
    
	return soap_build_err_rly(p_user, rx_msg, ret);
}


int soap_GetVideoSources(HTTPCLN * p_user, HTTPMSG * rx_msg, XMLN * p_body, XMLN * p_header)
{
	onvif_print("%s\r\n", __FUNCTION__);
    
#ifdef DEVICEIO_SUPPORT	
    if (strstr(rx_msg->first_line.value_string, "deviceIO"))
    {
        return soap_build_send_rly(p_user, rx_msg, build_tmd_GetVideoSources_rly_xml, NULL, NULL, p_header);
    }
#endif

	return soap_build_send_rly(p_user, rx_msg, build_GetVideoSources_rly_xml, NULL, NULL, p_header);	
}

int soap_GetVideoEncoderConfigurations(HTTPCLN * p_user, HTTPMSG * rx_msg, XMLN * p_body, XMLN * p_header)
{
	onvif_print("%s\r\n", __FUNCTION__);

	return soap_build_send_rly(p_user, rx_msg, build_GetVideoEncoderConfigurations_rly_xml, NULL, NULL, p_header);
}

int soap_GetCompatibleVideoEncoderConfigurations(HTTPCLN * p_user, HTTPMSG * rx_msg, XMLN * p_body, XMLN * p_header)
{
    XMLN * p_GetCompatibleVideoEncoderConfigurations;
    XMLN * p_ProfileToken;
    
	onvif_print("%s\r\n", __FUNCTION__);
	
	p_GetCompatibleVideoEncoderConfigurations = xml_node_soap_get(p_body, "GetCompatibleVideoEncoderConfigurations");
    assert(p_GetCompatibleVideoEncoderConfigurations);

    p_ProfileToken = xml_node_soap_get(p_GetCompatibleVideoEncoderConfigurations, "ProfileToken");
    if (p_ProfileToken && p_ProfileToken->data)
    {
    	return soap_build_send_rly(p_user, rx_msg, build_GetCompatibleVideoEncoderConfigurations_rly_xml, p_ProfileToken->data, NULL, p_header);
    }
	
    return soap_build_err_rly(p_user, rx_msg, ONVIF_ERR_MissingAttribute);
}

int soap_GetVideoEncoderConfiguration(HTTPCLN * p_user, HTTPMSG * rx_msg, XMLN * p_body, XMLN * p_header)
{
    XMLN * p_GetVideoEncoderConfiguration;
    XMLN * p_ConfigurationToken;
    
	onvif_print("%s\r\n", __FUNCTION__);

    p_GetVideoEncoderConfiguration = xml_node_soap_get(p_body, "GetVideoEncoderConfiguration");
    assert(p_GetVideoEncoderConfiguration);

	p_ConfigurationToken = xml_node_soap_get(p_GetVideoEncoderConfiguration, "ConfigurationToken");
	if (p_ConfigurationToken && p_ConfigurationToken->data)
	{
		return soap_build_send_rly(p_user, rx_msg, build_GetVideoEncoderConfiguration_rly_xml, p_ConfigurationToken->data, NULL, p_header);
	}
	else 
	{
		return soap_err_def2_rly(p_user, rx_msg, ERR_SENDER, ERR_MISSINGATTR, NULL, "Missing Attribute");
	}
}

int soap_GetVideoSourceConfigurations(HTTPCLN * p_user, HTTPMSG * rx_msg, XMLN * p_body, XMLN * p_header)
{
	onvif_print("%s\r\n", __FUNCTION__);

	return soap_build_send_rly(p_user, rx_msg, build_GetVideoSourceConfigurations_rly_xml, NULL, NULL, p_header);
}

int soap_GetVideoSourceConfiguration(HTTPCLN * p_user, HTTPMSG * rx_msg, XMLN * p_body, XMLN * p_header)
{
    XMLN * p_GetVideoSourceConfiguration;
    XMLN * p_ConfigurationToken;
    
    onvif_print("%s\r\n", __FUNCTION__);

    p_GetVideoSourceConfiguration = xml_node_soap_get(p_body, "GetVideoSourceConfiguration");
    assert(p_GetVideoSourceConfiguration);

	p_ConfigurationToken = xml_node_soap_get(p_GetVideoSourceConfiguration, "ConfigurationToken");
	if (p_ConfigurationToken && p_ConfigurationToken->data)
	{
	    return soap_build_send_rly(p_user, rx_msg, build_GetVideoSourceConfiguration_rly_xml, p_ConfigurationToken->data, NULL, p_header);
	}
	else 
	{
		return soap_err_def2_rly(p_user, rx_msg, ERR_SENDER, ERR_MISSINGATTR, NULL, "Missing Attribute");
	}
}

int soap_SetVideoSourceConfiguration(HTTPCLN * p_user, HTTPMSG * rx_msg, XMLN * p_body, XMLN * p_header)
{
    ONVIF_RET ret;
    XMLN * p_SetVideoSourceConfiguration;
    SetVideoSourceConfiguration_REQ req;
    
	onvif_print("%s\r\n", __FUNCTION__);

    p_SetVideoSourceConfiguration = xml_node_soap_get(p_body, "SetVideoSourceConfiguration");
    assert(p_SetVideoSourceConfiguration);
    
    memset(&req, 0, sizeof(req));

    ret = parse_SetVideoSourceConfiguration(p_SetVideoSourceConfiguration, &req);
    if (ONVIF_OK == ret)
    {
    	ret = onvif_SetVideoSourceConfiguration(&req);
    	if (ONVIF_OK == ret)
    	{
    		return soap_build_send_rly(p_user, rx_msg, build_SetVideoSourceConfiguration_rly_xml, NULL, NULL, p_header);
    	}
    }    

    return soap_build_err_rly(p_user, rx_msg, ret);
}

int soap_GetVideoSourceConfigurationOptions(HTTPCLN * p_user, HTTPMSG * rx_msg, XMLN * p_body, XMLN * p_header)
{
    ONVIF_RET ret;
    XMLN * p_GetVideoSourceConfigurationOptions;
    GetVideoSourceConfigurationOptions_REQ req;
    
    onvif_print("%s\r\n", __FUNCTION__);

    p_GetVideoSourceConfigurationOptions = xml_node_soap_get(p_body, "GetVideoSourceConfigurationOptions");
    assert(p_GetVideoSourceConfigurationOptions);
	
	memset(&req, 0, sizeof(req));
	
	ret = parse_GetVideoSourceConfigurationOptions(p_GetVideoSourceConfigurationOptions, &req);
	if (ONVIF_OK == ret)
	{
		return soap_build_send_rly(p_user, rx_msg, build_GetVideoSourceConfigurationOptions_rly_xml, (char *)&req, NULL, p_header);
	}

	return soap_build_err_rly(p_user, rx_msg, ret);
}

int soap_GetVideoEncoderConfigurationOptions(HTTPCLN * p_user, HTTPMSG * rx_msg, XMLN * p_body, XMLN * p_header)
{
	ONVIF_RET ret;
	XMLN * p_GetVideoEncoderConfigurationOptions;
	GetVideoEncoderConfigurationOptions_REQ req;
	
    onvif_print("%s\r\n", __FUNCTION__);

	p_GetVideoEncoderConfigurationOptions = xml_node_soap_get(p_body, "GetVideoEncoderConfigurationOptions");
    assert(p_GetVideoEncoderConfigurationOptions);
    
    memset(&req, 0, sizeof(req));

	ret = parse_GetVideoEncoderConfigurationOptions(p_GetVideoEncoderConfigurationOptions, &req);
	if (ONVIF_OK == ret)
	{
	    return soap_build_send_rly(p_user, rx_msg, build_GetVideoEncoderConfigurationOptions_rly_xml, (char *)&req, NULL, p_header);
	}

    return soap_build_err_rly(p_user, rx_msg, ret);    
}

int soap_GetCompatibleVideoSourceConfigurations(HTTPCLN * p_user, HTTPMSG * rx_msg, XMLN * p_body, XMLN * p_header)
{
	XMLN * p_GetCompatibleVideoSourceConfigurations;
	XMLN * p_ProfileToken;
	
	onvif_print("%s\r\n", __FUNCTION__);

    p_GetCompatibleVideoSourceConfigurations = xml_node_soap_get(p_body, "GetCompatibleVideoSourceConfigurations");
    assert(p_GetCompatibleVideoSourceConfigurations);

	p_ProfileToken = xml_node_soap_get(p_GetCompatibleVideoSourceConfigurations, "ProfileToken");
	if (p_ProfileToken && p_ProfileToken->data)
	{
	    return soap_build_send_rly(p_user, rx_msg, build_GetCompatibleVideoSourceConfigurations_rly_xml, p_ProfileToken->data, NULL, p_header);
	}
	else 
	{
		return soap_err_def2_rly(p_user, rx_msg, ERR_SENDER, ERR_MISSINGATTR, NULL, "Missing Attribute");
	}
}

int soap_SetVideoEncoderConfiguration(HTTPCLN * p_user, HTTPMSG * rx_msg, XMLN * p_body, XMLN * p_header)
{
	ONVIF_RET ret;
	XMLN * p_SetVideoEncoderConfiguration;
	SetVideoEncoderConfiguration_REQ req;
	
    onvif_print("%s\r\n", __FUNCTION__);

    p_SetVideoEncoderConfiguration = xml_node_soap_get(p_body, "SetVideoEncoderConfiguration");
    assert(p_SetVideoEncoderConfiguration);
	
	memset(&req, 0, sizeof(req));

	ret = parse_SetVideoEncoderConfiguration(p_SetVideoEncoderConfiguration, &req);
    if (ONVIF_OK == ret)
    {
        ret = onvif_SetVideoEncoderConfiguration(&req);
        if (ONVIF_OK == ret)
        {
        	return soap_build_send_rly(p_user, rx_msg, build_SetVideoEncoderConfiguration_rly_xml, NULL, NULL, p_header);
        }
    }
    
    return soap_build_err_rly(p_user, rx_msg, ret);
}

int soap_SetSynchronizationPoint(HTTPCLN * p_user, HTTPMSG * rx_msg, XMLN * p_body, XMLN * p_header)
{
    ONVIF_RET ret;
    XMLN * p_SetSynchronizationPoint;
    SetSynchronizationPoint_REQ req;

    onvif_print("%s\r\n", __FUNCTION__);
    
    memset(&req, 0, sizeof(req));
    
    p_SetSynchronizationPoint = xml_node_soap_get(p_body, "SetSynchronizationPoint");
    assert(p_SetSynchronizationPoint);

    ret = parse_SetSynchronizationPoint(p_SetSynchronizationPoint, &req);
    if (ONVIF_OK == ret)
    {
        ret = onvif_SetSynchronizationPoint(&req);
        if (ONVIF_OK == ret)
        {
            return soap_build_send_rly(p_user, rx_msg, build_SetSynchronizationPoint_rly_xml, (char *)&req, NULL, p_header);
        }
    }

    return soap_build_err_rly(p_user, rx_msg, ret);
}

int soap_SystemReboot(HTTPCLN * p_user, HTTPMSG * rx_msg, XMLN * p_body, XMLN * p_header)
{
	int ret;
	
    onvif_print("%s\r\n", __FUNCTION__);

	ret = soap_build_send_rly(p_user, rx_msg, build_SystemReboot_rly_xml, NULL, NULL, p_header);

	onvif_SystemReboot();

    return ret;
}

int soap_SetSystemFactoryDefault(HTTPCLN * p_user, HTTPMSG * rx_msg, XMLN * p_body, XMLN * p_header)
{
	int ret;
	int type = 0;
	XMLN * p_SetSystemFactoryDefault;
	XMLN * p_FactoryDefault;
	
	onvif_print("%s\r\n", __FUNCTION__);

	p_SetSystemFactoryDefault = xml_node_soap_get(p_body, "SetSystemFactoryDefault");
    assert(p_SetSystemFactoryDefault);	
	
    p_FactoryDefault = xml_node_soap_get(p_SetSystemFactoryDefault, "FactoryDefault");
    if (p_FactoryDefault && p_FactoryDefault->data)
    {
    	if (strcasecmp(p_FactoryDefault->data, "Hard") == 0)
    	{
    		type = 1;
    	}
    }

    ret = soap_build_send_rly(p_user, rx_msg, build_SetSystemFactoryDefault_rly_xml, NULL, NULL, p_header);
	
	onvif_SetSystemFactoryDefault(type);

    return ret;
}

int soap_GetSystemLog(HTTPCLN * p_user, HTTPMSG * rx_msg, XMLN * p_body, XMLN * p_header)
{
	ONVIF_RET ret;
	XMLN * p_GetSystemLog;
	GetSystemLog_REQ req;
	
	onvif_print("%s\r\n", __FUNCTION__);

	p_GetSystemLog = xml_node_soap_get(p_body, "GetSystemLog");
    assert(p_GetSystemLog);    

	memset(&req, 0, sizeof(req));
	
    ret = parse_GetSystemLog(p_GetSystemLog, &req);
    if (ONVIF_OK == ret)
    {
        GetSystemLog_RES res;
        memset(&res, 0, sizeof(res));
        
        ret = onvif_GetSystemLog(&req, &res);
        if (ONVIF_OK == ret)
        {
            return soap_build_send_rly(p_user, rx_msg, build_GetSystemLog_rly_xml, (char *)&res, NULL, p_header);
        }
    }

    return soap_build_err_rly(p_user, rx_msg, ret);
}

int soap_SetSystemDateAndTime(HTTPCLN * p_user, HTTPMSG * rx_msg, XMLN * p_body, XMLN * p_header)
{
	ONVIF_RET ret;
	XMLN * p_SetSystemDateAndTime;
	SetSystemDateAndTime_REQ req;
	
    onvif_print("%s\r\n", __FUNCTION__);

    p_SetSystemDateAndTime = xml_node_soap_get(p_body, "SetSystemDateAndTime");
    assert(p_SetSystemDateAndTime);
	
	memset(&req, 0, sizeof(SetSystemDateAndTime_REQ));

	ret = parse_SetSystemDateAndTime(p_SetSystemDateAndTime, &req);
    if (ONVIF_OK == ret)
    {
    	ret = onvif_SetSystemDateAndTime(&req);
        if (ONVIF_OK == ret)
        {
        	return soap_build_send_rly(p_user, rx_msg, build_SetSystemDateAndTime_rly_xml, NULL, NULL, p_header); 
        }        
    }

    return soap_build_err_rly(p_user, rx_msg, ret);
}

int soap_GetServices(HTTPCLN * p_user, HTTPMSG * rx_msg, XMLN * p_body, XMLN * p_header)
{
    ONVIF_RET ret;
	XMLN * p_GetServices;	
	GetServices_REQ req;
	
    onvif_print("%s\r\n", __FUNCTION__);

    p_GetServices = xml_node_soap_get(p_body, "GetServices");
    assert(p_GetServices);

    memset(&req, 0, sizeof(req));

    ret = parse_GetServices(p_GetServices, &req);
    if (ONVIF_OK == ret)
    {
        return soap_build_send_rly(p_user, rx_msg, build_GetServices_rly_xml, (char *)&req, NULL, p_header);
    }

    return soap_build_err_rly(p_user, rx_msg, ret);
}

int soap_Subscribe(HTTPCLN * p_user, HTTPMSG * rx_msg, XMLN * p_body, XMLN * p_header)
{
	ONVIF_RET ret;
	XMLN * p_Subscribe;
	Subscribe_REQ req;
	
    onvif_print("%s\r\n", __FUNCTION__);

    p_Subscribe = xml_node_soap_get(p_body, "Subscribe");
    assert(p_Subscribe);
	
	memset(&req, 0, sizeof(req));
	
	ret = parse_Subscribe(p_Subscribe, &req);
	if (ONVIF_OK == ret)
	{
	    ret = onvif_Subscribe(p_user->lip, p_user->lport, &req);
	    if (ONVIF_OK == ret)
	    {
			ret = soap_build_send_rly(p_user, rx_msg, build_Subscribe_rly_xml, (char *)req.p_eua, 
				"http://docs.oasis-open.org/wsn/bw-2/NotificationProducer/SubscribeResponse", p_header); 
		}
	}

    if (ret > 0)
    {
    	if (g_onvif_cfg.evt_sim_flag)
    	{
    	    ONVIF_NotificationMessage * p_message;
    	    
    	    // generate event, just for test
            
    	    p_message = onvif_init_NotificationMessage1();
    		if (p_message)
    		{
    			onvif_put_NotificationMessage(p_message);
    		}
            
    		p_message = onvif_init_NotificationMessage2();
    		if (p_message)
    		{
    			onvif_put_NotificationMessage(p_message);
    		}
    	}

    	return ret;
	}
		
	return soap_build_err_rly(p_user, rx_msg, ret);
}

int soap_Unsubscribe(HTTPCLN * p_user, HTTPMSG * rx_msg, XMLN * p_body, XMLN * p_header)
{
	XMLN * p_Unsubscribe;
	XMLN * p_To;
	
    onvif_print("%s\r\n", __FUNCTION__);

    p_Unsubscribe = xml_node_soap_get(p_body, "Unsubscribe");
    assert(p_Unsubscribe);

    p_To = xml_node_soap_get(p_header, "To");
	if (p_To && p_To->data)
	{
		ONVIF_RET ret = onvif_Unsubscribe(p_To->data);		
	    if (ONVIF_OK == ret)
	    {
	        return soap_build_send_rly(p_user, rx_msg, build_Unsubscribe_rly_xml, NULL, 
				"http://docs.oasis-open.org/wsn/bw-2/SubscriptionManager/UnsubscribeResponse", p_header);
	    }
	    else
	    {
	    	return soap_build_err_rly(p_user, rx_msg, ret);
	    }
	}
	
	return soap_err_def2_rly(p_user, rx_msg, ERR_SENDER, ERR_MISSINGATTR, NULL, "Missing Attibute");
}

int soap_Renew(HTTPCLN * p_user, HTTPMSG * rx_msg, XMLN * p_body, XMLN * p_header)
{
	ONVIF_RET ret;
	XMLN * p_Renew;
	XMLN * p_To;
	Renew_REQ req;
	
	onvif_print("%s\r\n", __FUNCTION__);

	p_Renew = xml_node_soap_get(p_body, "Renew");
	assert(p_Renew);
	
	memset(&req, 0, sizeof(req));

	p_To = xml_node_soap_get(p_header, "To");
	if (p_To && p_To->data)
	{
		strncpy(req.ProducterReference, p_To->data, sizeof(req.ProducterReference)-1);
	}
	
	ret = parse_Renew(p_Renew, &req);
	if (ONVIF_OK == ret)
	{
		ret = onvif_Renew(&req);
		if (ONVIF_OK == ret)
		{
			return soap_build_send_rly(p_user, rx_msg, build_Renew_rly_xml, NULL, 
				"http://docs.oasis-open.org/wsn/bw-2/SubscriptionManager/RenewResponse", p_header);
		}
	}
	
	return soap_build_err_rly(p_user, rx_msg, ret);
}

int soap_CreatePullPointSubscription(HTTPCLN * p_user, HTTPMSG * rx_msg, XMLN * p_body, XMLN * p_header)
{
	ONVIF_RET ret;
	XMLN * p_CreatePullPointSubscription;
	CreatePullPointSubscription_REQ req;
	
	onvif_print("%s\r\n", __FUNCTION__);

	p_CreatePullPointSubscription = xml_node_soap_get(p_body, "CreatePullPointSubscription");
	assert(p_CreatePullPointSubscription);

	memset(&req, 0, sizeof(req));

	ret = parse_CreatePullPointSubscription(p_CreatePullPointSubscription, &req);
	if (ONVIF_OK == ret)
	{
		ret = onvif_CreatePullPointSubscription(p_user->lip, p_user->lport, &req);
		if (ONVIF_OK == ret)
		{
			ret = soap_build_send_rly(p_user, rx_msg, build_CreatePullPointSubscription_rly_xml, (char *)req.p_eua, 
				"http://www.onvif.org/ver10/events/wsdl/EventPortType/CreatePullPointSubscriptionResponse", p_header);
		}
	}

    if (ret > 0)
    {
    	// generate event, just for test
    	
    	if (g_onvif_cfg.evt_sim_flag)
    	{
    	    // for ODTT test case IMAGING-4-1-1-V18.06, IMAGING-4-1-2-V18.06, 
    	    //  IMAGING-4-1-3-V18.06, IMAGING-4-1-4-V18.06, IMAGING-4-1-5-V18.06
    	    if (req.FiltersFlag && 
    	        (soap_strcmp(req.Filters.TopicExpression[0], "VideoSource/ImageTooBlurry/ImagingService")== 0 ||
    	         soap_strcmp(req.Filters.TopicExpression[0], "VideoSource/ImageTooDark/ImagingService")== 0 || 
    	         soap_strcmp(req.Filters.TopicExpression[0], "VideoSource/ImageTooBright/ImagingService")== 0 || 
    	         soap_strcmp(req.Filters.TopicExpression[0], "VideoSource/GlobalSceneChange/ImagingService")== 0 ||
    	         soap_strcmp(req.Filters.TopicExpression[0], "VideoSource/MotionAlarm")== 0))
    	    {
    	        ONVIF_NotificationMessage * p_message = onvif_init_NotificationMessage3(req.Filters.TopicExpression[0]);
        		if (p_message)
        		{
        			onvif_put_NotificationMessage(p_message);
        		}
    	    }
    	    else
    	    {
        	    ONVIF_NotificationMessage * p_message = onvif_init_NotificationMessage1();
        		if (p_message)
        		{
        			onvif_put_NotificationMessage(p_message);
        		}

        		p_message = onvif_init_NotificationMessage2();
        		if (p_message)
        		{
        			onvif_put_NotificationMessage(p_message);
        		}
    		}
    	}

    	return ret;
	}
	
	return soap_build_err_rly(p_user, rx_msg, ret);
}

int soap_PullMessages(HTTPCLN * p_user, HTTPMSG * rx_msg, XMLN * p_body, XMLN * p_header)
{
	ONVIF_RET ret;
	XMLN * p_PullMessages;
	PullMessages_REQ req;
	
	onvif_print("%s\r\n", __FUNCTION__);

	p_PullMessages = xml_node_soap_get(p_body, "PullMessages");
	assert(p_PullMessages);

	memset(&req, 0, sizeof(req));

	sscanf(rx_msg->first_line.value_string, "/event_service/%u", &req.eua_idx);
	
	ret = parse_PullMessages(p_PullMessages, &req);
	if (ONVIF_OK == ret)
	{
		ret = onvif_PullMessages(&req);
		if (ONVIF_OK == ret)
		{
			return soap_build_send_rly(p_user, rx_msg, build_PullMessages_rly_xml, (char *)&req, 
				"http://www.onvif.org/ver10/events/wsdl/PullPointSubscription/PullMessagesResponse", p_header);
		}
	}

	return soap_build_err_rly(p_user, rx_msg, ret);
}

int soap_tev_SetSynchronizationPoint(HTTPCLN * p_user, HTTPMSG * rx_msg, XMLN * p_body, XMLN * p_header)
{
    int ret;
	EUA * p_eua = NULL;
	
	onvif_print("%s\r\n", __FUNCTION__);

	onvif_tev_SetSynchronizationPoint();

    ret = soap_build_send_rly(p_user, rx_msg, build_tev_SetSynchronizationPoint_rly_xml, NULL, 
		"http://www.onvif.org/ver10/events/wsdl/PullPointSubscription/SetSynchronizationPointResponse", p_header);

    // generate event, just for test
    
    if (g_onvif_cfg.evt_sim_flag)
	{
	    ONVIF_NotificationMessage * p_message = onvif_init_NotificationMessage();
		if (p_message)
		{
			onvif_put_NotificationMessage(p_message);
		}

		p_message = onvif_init_NotificationMessage1();
		if (p_message)
		{
			onvif_put_NotificationMessage(p_message);
		}
	}

    return ret;
}

int soap_GetScopes(HTTPCLN * p_user, HTTPMSG * rx_msg, XMLN * p_body, XMLN * p_header)
{
    onvif_print("%s\r\n", __FUNCTION__);

    return soap_build_send_rly(p_user, rx_msg, build_GetScopes_rly_xml, NULL, NULL, p_header); 
}

int soap_AddScopes(HTTPCLN * p_user, HTTPMSG * rx_msg, XMLN * p_body, XMLN * p_header)
{
    int r;
	ONVIF_RET ret;
	XMLN * p_AddScopes;
	AddScopes_REQ req;
	
    onvif_print("%s\r\n", __FUNCTION__);

    p_AddScopes = xml_node_soap_get(p_body, "AddScopes");
    assert(p_AddScopes);
    
    memset(&req, 0, sizeof(req));
    
    ret = parse_AddScopes(p_AddScopes, &req);
    if (ONVIF_OK == ret)
    {
        ret = onvif_AddScopes(&req);
        if (ONVIF_OK == ret)
        {
    		r = soap_build_send_rly(p_user, rx_msg, build_AddScopes_rly_xml, NULL, NULL, p_header);

    		//onvif_hello();
    		return r;
    	}
    }

    return soap_build_err_rly(p_user, rx_msg, ret);
}

int soap_SetScopes(HTTPCLN * p_user, HTTPMSG * rx_msg, XMLN * p_body, XMLN * p_header)
{
    int r;
	ONVIF_RET ret;
	XMLN * p_SetScopes;
	SetScopes_REQ req;
	
	onvif_print("%s\r\n", __FUNCTION__);

    p_SetScopes = xml_node_soap_get(p_body, "SetScopes");
    assert(p_SetScopes);
    
    memset(&req, 0, sizeof(req));
    
    ret = parse_SetScopes(p_SetScopes, &req);
    if (ONVIF_OK == ret)
    {
        ret = onvif_SetScopes(&req);
        if (ONVIF_OK == ret)
        {
    		r = soap_build_send_rly(p_user, rx_msg, build_SetScopes_rly_xml, NULL, NULL, p_header);

    		//onvif_hello();
    		return r;
    	}
    }

    return soap_build_err_rly(p_user, rx_msg, ret);
}

int soap_RemoveScopes(HTTPCLN * p_user, HTTPMSG * rx_msg, XMLN * p_body, XMLN * p_header)
{
    int r;
	ONVIF_RET ret;
	XMLN * p_RemoveScopes;
	RemoveScopes_REQ req;
	
	onvif_print("%s\r\n", __FUNCTION__);

    p_RemoveScopes = xml_node_soap_get(p_body, "RemoveScopes");
    assert(p_RemoveScopes);
	
    memset(&req, 0, sizeof(req));
    
    ret = parse_RemoveScopes(p_RemoveScopes, &req);
    if (ONVIF_OK == ret)
    {
        ret = onvif_RemoveScopes(&req);
        if (ONVIF_OK == ret)
        {
    		r = soap_build_send_rly(p_user, rx_msg, build_RemoveScopes_rly_xml, (char *)&req, NULL, p_header);

    		//onvif_hello();
    		return r;
    	}
    }

    return soap_build_err_rly(p_user, rx_msg, ret);
}

int soap_GetHostname(HTTPCLN * p_user, HTTPMSG * rx_msg, XMLN * p_body, XMLN * p_header)
{
    onvif_print("%s\r\n", __FUNCTION__);

    return soap_build_send_rly(p_user, rx_msg, build_GetHostname_rly_xml, NULL, NULL, p_header); 
}

int soap_SetHostname(HTTPCLN * p_user, HTTPMSG * rx_msg, XMLN * p_body, XMLN * p_header)
{
	XMLN * p_SetHostname;
	XMLN * p_Name;
	
    onvif_print("%s\r\n", __FUNCTION__);

    p_SetHostname = xml_node_soap_get(p_body, "SetHostname");
    assert(p_SetHostname);

    p_Name = xml_node_soap_get(p_SetHostname, "Name");
	if (p_Name)
	{
	    ONVIF_RET ret;
	    
	    if (p_Name->data)
	    {
		    ret = onvif_SetHostname(p_Name->data, FALSE);
	    }
	    else
	    {
	        ret = onvif_SetHostname(NULL, TRUE);
	    }

	    if (ONVIF_OK == ret)
	    {
	        return soap_build_send_rly(p_user, rx_msg, build_SetHostname_rly_xml, NULL, NULL, p_header);
	    }
	    else if (ONVIF_ERR_InvalidHostname == ret)
	    {
	    	return soap_err_def2_rly(p_user, rx_msg, ERR_SENDER, ERR_INVALIDARGVAL, "ter:InvalidHostname", "Invalid Hostname");
	    }
	}
	
    return soap_err_def2_rly(p_user, rx_msg, ERR_SENDER, ERR_INVALIDARGVAL, "ter:InvalidHostname", "Invalid ArgVal"); 
}

int soap_SetHostnameFromDHCP(HTTPCLN * p_user, HTTPMSG * rx_msg, XMLN * p_body, XMLN * p_header)
{
	XMLN * p_SetHostname;
	XMLN * p_FromDHCP;
	
    onvif_print("%s\r\n", __FUNCTION__);

    p_SetHostname = xml_node_soap_get(p_body, "SetHostnameFromDHCP");
    assert(p_SetHostname);

    p_FromDHCP = xml_node_soap_get(p_SetHostname, "FromDHCP");
	if (p_FromDHCP && p_FromDHCP->data)
	{
	    ONVIF_RET ret;

	    if (strcasecmp(p_FromDHCP->data, "true") == 0)
	    {
	    	ret = onvif_SetHostname(NULL, TRUE);
	    }
	    else
	    {
	        ret = onvif_SetHostname(g_onvif_cfg.network.HostnameInformation.Name, FALSE);
	    }

	    if (ONVIF_OK == ret)
	    {
	        return soap_build_send_rly(p_user, rx_msg, build_SetHostnameFromDHCP_rly_xml, NULL, NULL, p_header);
	    }
	    else if (ONVIF_ERR_InvalidHostname == ret)
	    {
	    	return soap_err_def2_rly(p_user, rx_msg, ERR_SENDER, ERR_INVALIDARGVAL, "ter:InvalidHostname", "Invalid Hostname");
	    }
	}
	
    return soap_err_def_rly(p_user, rx_msg); 
}


int soap_GetNetworkProtocols(HTTPCLN * p_user, HTTPMSG * rx_msg, XMLN * p_body, XMLN * p_header)
{
    onvif_print("%s\r\n", __FUNCTION__);

    return soap_build_send_rly(p_user, rx_msg, build_GetNetworkProtocols_rly_xml, NULL, NULL, p_header); 
}

int soap_SetNetworkProtocols(HTTPCLN * p_user, HTTPMSG * rx_msg, XMLN * p_body, XMLN * p_header)
{
	ONVIF_RET ret;
	XMLN * p_SetNetworkProtocols;
	SetNetworkProtocols_REQ req;
	
    onvif_print("%s\r\n", __FUNCTION__);

    p_SetNetworkProtocols = xml_node_soap_get(p_body, "SetNetworkProtocols");
    assert(p_SetNetworkProtocols);
	
	memset(&req, 0, sizeof(req));

	ret = parse_SetNetworkProtocols(p_SetNetworkProtocols, &req);
    if (ONVIF_OK == ret)
    {    
    	ret = onvif_SetNetworkProtocols(&req);
    	if (ONVIF_OK == ret)
    	{
    		return soap_build_send_rly(p_user, rx_msg, build_SetNetworkProtocols_rly_xml, NULL, NULL, p_header); 
    	}
    }

    return soap_build_err_rly(p_user, rx_msg, ret);
}

int soap_GetNetworkDefaultGateway(HTTPCLN * p_user, HTTPMSG * rx_msg, XMLN * p_body, XMLN * p_header)
{
    onvif_print("%s\r\n", __FUNCTION__);

    return soap_build_send_rly(p_user, rx_msg, build_GetNetworkDefaultGateway_rly_xml, NULL, NULL, p_header); 
}

int soap_SetNetworkDefaultGateway(HTTPCLN * p_user, HTTPMSG * rx_msg, XMLN * p_body, XMLN * p_header)
{
	ONVIF_RET ret;
	XMLN * p_SetNetworkDefaultGateway;
	SetNetworkDefaultGateway_REQ req;
	
    onvif_print("%s\r\n", __FUNCTION__);

	p_SetNetworkDefaultGateway = xml_node_soap_get(p_body, "SetNetworkDefaultGateway");
    assert(p_SetNetworkDefaultGateway);
	
	memset(&req, 0, sizeof(SetNetworkDefaultGateway_REQ));

	ret = parse_SetNetworkDefaultGateway(p_SetNetworkDefaultGateway, &req);
    if (ONVIF_OK == ret)
    {    
    	ret = onvif_SetNetworkDefaultGateway(&req);
    	if (ONVIF_OK == ret)
    	{
    		return soap_build_send_rly(p_user, rx_msg, build_SetNetworkDefaultGateway_rly_xml, NULL, NULL, p_header); 
    	}
    }
    	
    return soap_build_err_rly(p_user, rx_msg, ret);
}

int soap_GetDiscoveryMode(HTTPCLN * p_user, HTTPMSG * rx_msg, XMLN * p_body, XMLN * p_header)
{
    onvif_print("%s\r\n", __FUNCTION__);

    return soap_build_send_rly(p_user, rx_msg, build_GetDiscoveryMode_rly_xml, NULL, NULL, p_header); 
}

int soap_SetDiscoveryMode(HTTPCLN * p_user, HTTPMSG * rx_msg, XMLN * p_body, XMLN * p_header)
{
	ONVIF_RET ret;
	XMLN * p_SetDiscoveryMode;
	SetDiscoveryMode_REQ req;
	
    onvif_print("%s\r\n", __FUNCTION__);

    p_SetDiscoveryMode = xml_node_soap_get(p_body, "SetDiscoveryMode");
    assert(p_SetDiscoveryMode);
	
	memset(&req, 0, sizeof(SetDiscoveryMode_REQ));

	ret = parse_SetDiscoveryMode(p_SetDiscoveryMode, &req);
	if (ONVIF_OK == ret)
	{
	    ret = onvif_SetDiscoveryMode(&req);
	    if (ONVIF_OK == ret)
	    {
			return soap_build_send_rly(p_user, rx_msg, build_SetDiscoveryMode_rly_xml, NULL, NULL, p_header); 
		}
	}
	
	return soap_build_err_rly(p_user, rx_msg, ret);
}

int soap_GetDNS(HTTPCLN * p_user, HTTPMSG * rx_msg, XMLN * p_body, XMLN * p_header)
{
	onvif_print("%s\r\n", __FUNCTION__);

    return soap_build_send_rly(p_user, rx_msg, build_GetDNS_rly_xml, NULL, NULL, p_header); 
}

int soap_SetDNS(HTTPCLN * p_user, HTTPMSG * rx_msg, XMLN * p_body, XMLN * p_header)
{
	ONVIF_RET ret;
	XMLN * p_SetDNS;
	SetDNS_REQ req;
	
	onvif_print("%s\r\n", __FUNCTION__);

	p_SetDNS = xml_node_soap_get(p_body, "SetDNS");
    assert(p_SetDNS);
    
    memset(&req, 0, sizeof(SetDNS_REQ));

	ret = parse_SetDNS(p_SetDNS, &req);
    if (ONVIF_OK == ret)
    {
    	ret = onvif_SetDNS(&req);
    	if (ONVIF_OK == ret)
    	{
    		return soap_build_send_rly(p_user, rx_msg, build_SetDNS_rly_xml, NULL, NULL, p_header); 
    	}
    }
    
    return soap_build_err_rly(p_user, rx_msg, ret);
}

int soap_GetNTP(HTTPCLN * p_user, HTTPMSG * rx_msg, XMLN * p_body, XMLN * p_header)
{
	onvif_print("%s\r\n", __FUNCTION__);

    return soap_build_send_rly(p_user, rx_msg, build_GetNTP_rly_xml, NULL, NULL, p_header); 
}

int soap_SetNTP(HTTPCLN * p_user, HTTPMSG * rx_msg, XMLN * p_body, XMLN * p_header)
{
	ONVIF_RET ret;
	XMLN * p_SetNTP;
	SetNTP_REQ req;
	
	onvif_print("%s\r\n", __FUNCTION__);

	p_SetNTP = xml_node_soap_get(p_body, "SetNTP");
    assert(p_SetNTP);
    
    memset(&req, 0, sizeof(SetNTP_REQ));

	ret = parse_SetNTP(p_SetNTP, &req);
    if (ONVIF_OK == ret)
    {
    	ret = onvif_SetNTP(&req);
    	if (ONVIF_OK == ret)
    	{
    		return soap_build_send_rly(p_user, rx_msg, build_SetNTP_rly_xml, NULL, NULL, p_header); 
    	}
    }

    return soap_build_err_rly(p_user, rx_msg, ret);
}

int soap_GetZeroConfiguration(HTTPCLN * p_user, HTTPMSG * rx_msg, XMLN * p_body, XMLN * p_header)
{
    onvif_print("%s\r\n", __FUNCTION__);

    return soap_build_send_rly(p_user, rx_msg, build_GetZeroConfiguration_rly_xml, NULL, NULL, p_header); 
}

int soap_SetZeroConfiguration(HTTPCLN * p_user, HTTPMSG * rx_msg, XMLN * p_body, XMLN * p_header)
{
    ONVIF_RET ret;
	XMLN * p_SetZeroConfiguration;
	SetZeroConfiguration_REQ req;
	
	onvif_print("%s\r\n", __FUNCTION__);

	p_SetZeroConfiguration = xml_node_soap_get(p_body, "SetZeroConfiguration");
    assert(p_SetZeroConfiguration);
    
    memset(&req, 0, sizeof(SetZeroConfiguration_REQ));

	ret = parse_SetZeroConfiguration(p_SetZeroConfiguration, &req);
    if (ONVIF_OK == ret)
    {
    	ret = onvif_SetZeroConfiguration(&req);
    	if (ONVIF_OK == ret)
    	{
    		return soap_build_send_rly(p_user, rx_msg, build_SetZeroConfiguration_rly_xml, NULL, NULL, p_header); 
    	}
    }

    return soap_build_err_rly(p_user, rx_msg, ret);
}

int soap_GetDot11Capabilities(HTTPCLN * p_user, HTTPMSG * rx_msg, XMLN * p_body, XMLN * p_header)
{
    onvif_print("%s\r\n", __FUNCTION__);

    return soap_build_send_rly(p_user, rx_msg, build_GetDot11Capabilities_rly_xml, NULL, NULL, p_header);
}

int soap_GetDot11Status(HTTPCLN * p_user, HTTPMSG * rx_msg, XMLN * p_body, XMLN * p_header)
{
    ONVIF_RET ret;
	XMLN * p_GetDot11Status;
	GetDot11Status_REQ req;
	
	onvif_print("%s\r\n", __FUNCTION__);

	p_GetDot11Status = xml_node_soap_get(p_body, "GetDot11Status");
    assert(p_GetDot11Status);
    
    memset(&req, 0, sizeof(req));

	ret = parse_GetDot11Status(p_GetDot11Status, &req);
    if (ONVIF_OK == ret)
    {
        GetDot11Status_RES res;
        memset(&res, 0, sizeof(res));
        
    	ret = onvif_GetDot11Status(&req, &res);
    	if (ONVIF_OK == ret)
    	{
    		return soap_build_send_rly(p_user, rx_msg, build_GetDot11Status_rly_xml, (char *)&res, NULL, p_header); 
    	}
    }

    return soap_build_err_rly(p_user, rx_msg, ret);
}

int soap_ScanAvailableDot11Networks(HTTPCLN * p_user, HTTPMSG * rx_msg, XMLN * p_body, XMLN * p_header)
{
    ONVIF_RET ret;
	XMLN * p_ScanAvailableDot11Networks;
	ScanAvailableDot11Networks_REQ req;
	
	onvif_print("%s\r\n", __FUNCTION__);

	p_ScanAvailableDot11Networks = xml_node_soap_get(p_body, "ScanAvailableDot11Networks");
    assert(p_ScanAvailableDot11Networks);
    
    memset(&req, 0, sizeof(req));

	ret = parse_ScanAvailableDot11Networks(p_ScanAvailableDot11Networks, &req);
    if (ONVIF_OK == ret)
    {
        ScanAvailableDot11Networks_RES res;
        memset(&res, 0, sizeof(res));
        
    	ret = onvif_ScanAvailableDot11Networks(&req, &res);
    	if (ONVIF_OK == ret)
    	{
    		return soap_build_send_rly(p_user, rx_msg, build_ScanAvailableDot11Networks_rly_xml, (char *)&res, NULL, p_header); 
    	}
    }

    return soap_build_err_rly(p_user, rx_msg, ret);
}

int soap_GetServiceCapabilities(HTTPCLN * p_user, HTTPMSG * rx_msg, XMLN * p_body, XMLN * p_header)
{
	char * post;
	XMLN * p_GetServiceCapabilities;
	onvif_CapabilityCategory category;
	
	onvif_print("%s\r\n", __FUNCTION__);

    p_GetServiceCapabilities = xml_node_soap_get(p_body, "GetServiceCapabilities");
    assert(p_GetServiceCapabilities);    
	
	post = rx_msg->first_line.value_string;
	if (NULL == post)
	{
		category = CapabilityCategory_Device;
	}
#ifdef DEVICEIO_SUPPORT
	else if (strstr(post, "deviceIO"))
	{
		category = CapabilityCategory_DeviceIO;
	}
#endif
#ifdef MEDIA2_SUPPORT
    else if (strstr(post, "media2"))
    {
        category = CapabilityCategory_Media2;
    }
#endif
	else if (strstr(post, "media"))
	{
		category = CapabilityCategory_Media;
	}
	else if (strstr(post, "device"))
	{
		category = CapabilityCategory_Device;
	}
	else if (strstr(post, "image"))
	{
		category = CapabilityCategory_Imaging;
	}
	else if (strstr(post, "ptz"))
	{
		category = CapabilityCategory_PTZ;
	}
	else if (strstr(post, "event"))
	{
		category = CapabilityCategory_Events;
	}
#ifdef VIDEO_ANALYTICS	
	else if (strstr(post, "analytics"))
	{
		category = CapabilityCategory_Analytics;
	}
#endif
#ifdef PROFILE_G_SUPPORT
	else if (strstr(post, "recording"))
	{
		category = CapabilityCategory_Recording;
	}
	else if (strstr(post, "search"))
	{
		category = CapabilityCategory_Search;
	}
	else if (strstr(post, "replay"))
	{
		category = CapabilityCategory_Replay;
	}
#endif
#ifdef PROFILE_C_SUPPORT
    else if (strstr(post, "accesscontrol"))
	{
		category = CapabilityCategory_AccessControl;
	}
	else if (strstr(post, "doorcontrol"))
	{
		category = CapabilityCategory_DoorControl;
	}
#endif
#ifdef THERMAL_SUPPORT
    else if (strstr(post, "thermal"))
	{
		category = CapabilityCategory_Thermal;
	}
#endif
#ifdef CREDENTIAL_SUPPORT
    else if (strstr(post, "credential"))
	{
		category = CapabilityCategory_Credential;
	}
#endif
#ifdef ACCESS_RULES
    else if (strstr(post, "accessrules"))
	{
        category = CapabilityCategory_AccessRules;
    }
#endif
#ifdef SCHEDULE_SUPPORT
    else if (strstr(post, "schedule"))
    {
        category = CapabilityCategory_Schedule;
    }
#endif
#ifdef RECEIVER_SUPPORT
    else if (strstr(post, "receiver"))
    {
        category = CapabilityCategory_Receiver;
    }
#endif
	else
	{
		category = CapabilityCategory_Device;
	}

	if (CapabilityCategory_Events == category)
	{
	    return soap_build_send_rly(p_user, rx_msg, build_GetServiceCapabilities_rly_xml, (char *)&category, 
			"http://www.onvif.org/ver10/events/wsdl/EventPortType/GetServiceCapabilitiesResponse", p_header); 	
	}
	
	return soap_build_send_rly(p_user, rx_msg, build_GetServiceCapabilities_rly_xml, (char *)&category, NULL, p_header); 	
}

int soap_GetEventProperties(HTTPCLN * p_user, HTTPMSG * rx_msg, XMLN * p_body, XMLN * p_header)
{
	onvif_print("%s\r\n", __FUNCTION__);

	return soap_build_send_rly(p_user, rx_msg, build_GetEventProperties_rly_xml, NULL, 
		"http://www.onvif.org/ver10/events/wsdl/EventPortType/GetEventPropertiesResponse", p_header);
}

int soap_GetWsdlUrl(HTTPCLN * p_user, HTTPMSG * rx_msg, XMLN * p_body, XMLN * p_header)
{
	onvif_print("%s\r\n", __FUNCTION__);

	return soap_build_send_rly(p_user, rx_msg, build_GetWsdlUrl_rly_xml, NULL, NULL, p_header);
}

int soap_GetEndpointReference(HTTPCLN * p_user, HTTPMSG * rx_msg, XMLN * p_body, XMLN * p_header)
{
    onvif_print("%s\r\n", __FUNCTION__);

	return soap_build_send_rly(p_user, rx_msg, build_GetEndpointReference_rly_xml, NULL, NULL, p_header);
}

int soap_GetGuaranteedNumberOfVideoEncoderInstances(HTTPCLN * p_user, HTTPMSG * rx_msg, XMLN * p_body, XMLN * p_header)
{
	XMLN * p_GetGuaranteedNumberOfVideoEncoderInstances;
	XMLN * p_ConfigurationToken;
	
	onvif_print("%s\r\n", __FUNCTION__);

    p_GetGuaranteedNumberOfVideoEncoderInstances = xml_node_soap_get(p_body, "GetGuaranteedNumberOfVideoEncoderInstances");
	assert(p_GetGuaranteedNumberOfVideoEncoderInstances);

	p_ConfigurationToken = xml_node_soap_get(p_GetGuaranteedNumberOfVideoEncoderInstances, "ConfigurationToken");
	if (p_ConfigurationToken && p_ConfigurationToken->data)
	{
		return soap_build_send_rly(p_user, rx_msg, build_GetGuaranteedNumberOfVideoEncoderInstances_rly_xml, p_ConfigurationToken->data, NULL, p_header);
	}

	return soap_build_err_rly(p_user, rx_msg, ONVIF_ERR_MissingAttribute);
}

int soap_GetImagingSettings(HTTPCLN * p_user, HTTPMSG * rx_msg, XMLN * p_body, XMLN * p_header)
{
	XMLN * p_GetImagingSettings;
	XMLN * p_VideoSourceToken;
	
	onvif_print("%s\r\n", __FUNCTION__);

    p_GetImagingSettings = xml_node_soap_get(p_body, "GetImagingSettings");
	assert(p_GetImagingSettings);

	p_VideoSourceToken = xml_node_soap_get(p_GetImagingSettings, "VideoSourceToken");
	if (p_VideoSourceToken && p_VideoSourceToken->data)
	{
		return soap_build_send_rly(p_user, rx_msg, build_GetImagingSettings_rly_xml, p_VideoSourceToken->data, NULL, p_header);
	}

	return soap_build_err_rly(p_user, rx_msg, ONVIF_ERR_MissingAttribute);
}

int soap_SetImagingSettings(HTTPCLN * p_user, HTTPMSG * rx_msg, XMLN * p_body, XMLN * p_header)
{
	ONVIF_RET ret;
	XMLN * p_SetImagingSettings;
	SetImagingSettings_REQ req;
	
	onvif_print("\n%s\r\n", __FUNCTION__);

    p_SetImagingSettings = xml_node_soap_get(p_body, "SetImagingSettings");
	assert(p_SetImagingSettings);
	
	memset(&req, 0, sizeof(SetImagingSettings_REQ));

	ret = parse_SetImagingSettings(p_SetImagingSettings, &req);
	if (ONVIF_OK == ret)
	{
		ret = onvif_SetImagingSettings(&req);
		if (ONVIF_OK == ret)
		{
			return soap_build_send_rly(p_user, rx_msg, build_SetImagingSettings_rly_xml, NULL, NULL, p_header);
		}
	}

	return soap_build_err_rly(p_user, rx_msg, ret);
}

int soap_GetOptions(HTTPCLN * p_user, HTTPMSG * rx_msg, XMLN * p_body, XMLN * p_header)
{
	XMLN * p_GetOptions;
	XMLN * p_VideoSourceToken;
	
	onvif_print("%s\r\n", __FUNCTION__);

    p_GetOptions = xml_node_soap_get(p_body, "GetOptions");
	assert(p_GetOptions);

	p_VideoSourceToken = xml_node_soap_get(p_GetOptions, "VideoSourceToken");
	if (p_VideoSourceToken && p_VideoSourceToken->data)
	{
		return soap_build_send_rly(p_user, rx_msg, build_GetOptions_rly_xml, p_VideoSourceToken->data, NULL, p_header);
	}

	return soap_build_err_rly(p_user, rx_msg, ONVIF_ERR_MissingAttribute);
}

int soap_GetMoveOptions(HTTPCLN * p_user, HTTPMSG * rx_msg, XMLN * p_body, XMLN * p_header)
{
	XMLN * p_GetMoveOptions;
	XMLN * p_VideoSourceToken;
	
	onvif_print("%s\r\n", __FUNCTION__);

    p_GetMoveOptions = xml_node_soap_get(p_body, "GetMoveOptions");
	assert(p_GetMoveOptions);

	p_VideoSourceToken = xml_node_soap_get(p_GetMoveOptions, "VideoSourceToken");
	if (p_VideoSourceToken && p_VideoSourceToken->data)
	{
		return soap_build_send_rly(p_user, rx_msg, build_GetMoveOptions_rly_xml, p_VideoSourceToken->data, NULL, p_header);
	}

	return soap_build_err_rly(p_user, rx_msg, ONVIF_ERR_MissingAttribute);
}

int soap_Move(HTTPCLN * p_user, HTTPMSG * rx_msg, XMLN * p_body, XMLN * p_header)
{
	ONVIF_RET ret;
	XMLN * p_Move;
	Move_REQ req;
	
	onvif_print("%s\r\n", __FUNCTION__);

	p_Move = xml_node_soap_get(p_body, "Move");
	assert(p_Move);
	
	memset(&req, 0, sizeof(Move_REQ));

	ret = parse_Move(p_Move, &req);
	if (ONVIF_OK == ret)
	{
		ret = onvif_Move(&req);
		if (ONVIF_OK == ret)
		{
			return soap_build_send_rly(p_user, rx_msg, build_Move_rly_xml, NULL, NULL, p_header);
		}
	}
	
	return soap_build_err_rly(p_user, rx_msg, ret);
}

int soap_GetUsers(HTTPCLN * p_user, HTTPMSG * rx_msg, XMLN * p_body, XMLN * p_header)
{
	onvif_print("%s\r\n", __FUNCTION__);
	
	return soap_build_send_rly(p_user, rx_msg, build_GetUsers_rly_xml, NULL, NULL, p_header);
}

int soap_CreateUsers(HTTPCLN * p_user, HTTPMSG * rx_msg, XMLN * p_body, XMLN * p_header)
{
	ONVIF_RET ret;
	XMLN * p_CreateUsers;
	CreateUsers_REQ req;
	
	onvif_print("%s\r\n", __FUNCTION__);

	p_CreateUsers = xml_node_soap_get(p_body, "CreateUsers");
	assert(p_CreateUsers);
	
    memset(&req, 0, sizeof(req));
    
    ret = parse_CreateUsers(p_CreateUsers, &req);
    if (ONVIF_OK == ret)
    {
        ret = onvif_CreateUsers(&req);
        if (ONVIF_OK == ret)
        {
    		return soap_build_send_rly(p_user, rx_msg, build_CreateUsers_rly_xml, NULL, NULL, p_header);
    	}
    }

    return soap_build_err_rly(p_user, rx_msg, ret);
}
	
int soap_DeleteUsers(HTTPCLN * p_user, HTTPMSG * rx_msg, XMLN * p_body, XMLN * p_header)
{
	ONVIF_RET ret;
	XMLN * p_DeleteUsers;
	DeleteUsers_REQ req;
	
	onvif_print("%s\r\n", __FUNCTION__);

	p_DeleteUsers = xml_node_soap_get(p_body, "DeleteUsers");
	assert(p_DeleteUsers);
	
    memset(&req, 0, sizeof(req));
    
    ret = parse_DeleteUsers(p_DeleteUsers, &req);
    if (ONVIF_OK == ret)
    {
        ret = onvif_DeleteUsers(&req);
        if (ONVIF_OK == ret)
        {
    		return soap_build_send_rly(p_user, rx_msg, build_DeleteUsers_rly_xml, NULL, NULL, p_header);
    	}
    }

    return soap_build_err_rly(p_user, rx_msg, ret);
}

int soap_SetUser(HTTPCLN * p_user, HTTPMSG * rx_msg, XMLN * p_body, XMLN * p_header)
{
	ONVIF_RET ret;
	XMLN * p_SetUser;
	SetUser_REQ req;
	
	onvif_print("%s\r\n", __FUNCTION__);

	p_SetUser = xml_node_soap_get(p_body, "SetUser");
	assert(p_SetUser);
	
    memset(&req, 0, sizeof(req));
    
    ret = parse_SetUser(p_SetUser, &req);
    if (ONVIF_OK == ret)
    {
        ret = onvif_SetUser(&req);
        if (ONVIF_OK == ret)
        {
    		return soap_build_send_rly(p_user, rx_msg, build_SetUser_rly_xml, NULL, NULL, p_header);
    	}
    }

    return soap_build_err_rly(p_user, rx_msg, ret);
}

int soap_GetRemoteUser(HTTPCLN * p_user, HTTPMSG * rx_msg, XMLN * p_body, XMLN * p_header)
{	
    ONVIF_RET ret;
    GetRemoteUser_RES res;
    
	onvif_print("%s\r\n", __FUNCTION__);

    memset(&res, 0, sizeof(res));
    
	ret = onvif_GetRemoteUser(&res);
    if (ONVIF_OK == ret)
    {
		return soap_build_send_rly(p_user, rx_msg, build_GetRemoteUser_rly_xml, (char *)&res, NULL, p_header);
	}
    	
	return soap_build_err_rly(p_user, rx_msg, ret);
}

int soap_SetRemoteUser(HTTPCLN * p_user, HTTPMSG * rx_msg, XMLN * p_body, XMLN * p_header)
{
	ONVIF_RET ret;
	XMLN * p_SetRemoteUser;
	SetRemoteUser_REQ req;
	
	onvif_print("%s\r\n", __FUNCTION__);

	p_SetRemoteUser = xml_node_soap_get(p_body, "SetRemoteUser");
	assert(p_SetRemoteUser);
	
    memset(&req, 0, sizeof(req));
    
    ret = parse_SetRemoteUser(p_SetRemoteUser, &req);
    if (ONVIF_OK == ret)
    {
        ret = onvif_SetRemoteUser(&req);
        if (ONVIF_OK == ret)
        {
    		return soap_build_send_rly(p_user, rx_msg, build_SetRemoteUser_rly_xml, NULL, NULL, p_header);
    	}
    }

    return soap_build_err_rly(p_user, rx_msg, ret);
}

#ifdef IPFILTER_SUPPORT	

int soap_GetIPAddressFilter(HTTPCLN * p_user, HTTPMSG * rx_msg, XMLN * p_body, XMLN * p_header)
{
    onvif_print("%s\r\n", __FUNCTION__);
	
	return soap_build_send_rly(p_user, rx_msg, build_GetIPAddressFilter_rly_xml, NULL, NULL, p_header);
}

int soap_SetIPAddressFilter(HTTPCLN * p_user, HTTPMSG * rx_msg, XMLN * p_body, XMLN * p_header)
{
    ONVIF_RET ret;
	XMLN * p_SetIPAddressFilter;
	SetIPAddressFilter_REQ req;
	
	onvif_print("%s\r\n", __FUNCTION__);

	p_SetIPAddressFilter = xml_node_soap_get(p_body, "SetIPAddressFilter");
	assert(p_SetIPAddressFilter);
	
    memset(&req, 0, sizeof(req));
    
    ret = parse_SetIPAddressFilter(p_SetIPAddressFilter, &req);
    if (ONVIF_OK == ret)
    {
        ret = onvif_SetIPAddressFilter(&req);
        if (ONVIF_OK == ret)
        {
    		return soap_build_send_rly(p_user, rx_msg, build_SetIPAddressFilter_rly_xml, NULL, NULL, p_header);
    	}
    }

    return soap_build_err_rly(p_user, rx_msg, ret);
}

int soap_AddIPAddressFilter(HTTPCLN * p_user, HTTPMSG * rx_msg, XMLN * p_body, XMLN * p_header)
{
    ONVIF_RET ret;
	XMLN * p_AddIPAddressFilter;
	AddIPAddressFilter_REQ req;
	
	onvif_print("%s\r\n", __FUNCTION__);

	p_AddIPAddressFilter = xml_node_soap_get(p_body, "AddIPAddressFilter");
	assert(p_AddIPAddressFilter);
	
    memset(&req, 0, sizeof(req));
    
    ret = parse_AddIPAddressFilter(p_AddIPAddressFilter, &req);
    if (ONVIF_OK == ret)
    {
        ret = onvif_AddIPAddressFilter(&req);
        if (ONVIF_OK == ret)
        {
    		return soap_build_send_rly(p_user, rx_msg, build_AddIPAddressFilter_rly_xml, NULL, NULL, p_header);
    	}
    }

    return soap_build_err_rly(p_user, rx_msg, ret);
}

int soap_RemoveIPAddressFilter(HTTPCLN * p_user, HTTPMSG * rx_msg, XMLN * p_body, XMLN * p_header)
{
    ONVIF_RET ret;
	XMLN * p_RemoveIPAddressFilter;
	RemoveIPAddressFilter_REQ req;
	
	onvif_print("%s\r\n", __FUNCTION__);

	p_RemoveIPAddressFilter = xml_node_soap_get(p_body, "RemoveIPAddressFilter");
	assert(p_RemoveIPAddressFilter);
	
    memset(&req, 0, sizeof(req));
    
    ret = parse_RemoveIPAddressFilter(p_RemoveIPAddressFilter, &req);
    if (ONVIF_OK == ret)
    {
        ret = onvif_RemoveIPAddressFilter(&req);
        if (ONVIF_OK == ret)
        {
    		return soap_build_send_rly(p_user, rx_msg, build_RemoveIPAddressFilter_rly_xml, NULL, NULL, p_header);
    	}
    }

    return soap_build_err_rly(p_user, rx_msg, ret);
}

#endif // end of IPFILTER_SUPPORT

int soap_UpgradeSystemFirmware(HTTPCLN * p_user, HTTPMSG * rx_msg, XMLN * p_body, XMLN * p_header)
{
	onvif_print("%s\r\n", __FUNCTION__);

	return 0;	
}

int soap_StartFirmwareUpgrade(HTTPCLN * p_user, HTTPMSG * rx_msg, XMLN * p_body, XMLN * p_header)
{
	StartFirmwareUpgrade_RES res;
	
	onvif_print("%s\r\n", __FUNCTION__);

	memset(&res, 0, sizeof(res));
	
	if (onvif_StartFirmwareUpgrade(p_user->lip, p_user->lport, &res))
	{	
		return soap_build_send_rly(p_user, rx_msg, build_StartFirmwareUpgrade_rly_xml, (char *)&res, NULL, p_header);
	}
	
	return soap_err_def_rly(p_user, rx_msg);
}

void soap_FirmwareUpgrade(HTTPCLN * p_user, HTTPMSG * rx_msg)
{
	char * p_buff = http_get_ctt(rx_msg);
	
	onvif_print("%s\r\n", __FUNCTION__);
	onvif_print("rx_msg->ctt_len===%d\n", rx_msg->ctt_len);
	if (onvif_FirmwareUpgradeCheck(p_buff, rx_msg->ctt_len))
	{
		if (onvif_FirmwareUpgrade(p_buff, rx_msg->ctt_len))
		{
			soap_http_rly(p_user, rx_msg, NULL, 0);

			onvif_FirmwareUpgradePost();
		}
		else
		{
			soap_http_err_rly(p_user, rx_msg, 500, "Internal Server Error", NULL, 0);
		}
	}
	else
	{
		soap_http_err_rly(p_user, rx_msg, 415, "Unsupported Media Type", NULL, 0);
	}	
}

int soap_StartSystemRestore(HTTPCLN * p_user, HTTPMSG * rx_msg, XMLN * p_body, XMLN * p_header)
{
    StartSystemRestore_RES res;
	
	onvif_print("%s\r\n", __FUNCTION__);

	memset(&res, 0, sizeof(res));
	
	if (onvif_StartSystemRestore(p_user->lip, p_user->lport, &res))
	{	
		return soap_build_send_rly(p_user, rx_msg, build_StartSystemRestore_rly_xml, (char *)&res, NULL, p_header);
	}
	
	return soap_err_def_rly(p_user, rx_msg);
}

void soap_SystemRestore(HTTPCLN * p_user, HTTPMSG * rx_msg)
{
    char * p_buff = http_get_ctt(rx_msg);
	
	if (onvif_SystemRestoreCheck(p_buff, rx_msg->ctt_len))
	{
		if (onvif_SystemRestore(p_buff, rx_msg->ctt_len))
		{
			soap_http_rly(p_user, rx_msg, NULL, 0);

			onvif_SystemRestorePost();
		}
		else
		{
			soap_http_err_rly(p_user, rx_msg, 500, "Internal Server Error", NULL, 0);
		}
	}
	else
	{
		soap_http_err_rly(p_user, rx_msg, 415, "Unsupported Media Type", NULL, 0);
	}	
}

void soap_GetSnapshot(HTTPCLN * p_user, HTTPMSG * rx_msg)
{
    char *p_bufs = NULL;    
    int  rlen = 1500*1024;
    char profile_token[ONVIF_TOKEN_LEN] = {'\0'};

    // get profile token
    char * post = rx_msg->first_line.value_string;
    char * p1 = strstr(post, "snapshot");
    if (p1)
    {
        char * p2 = strchr(p1+1, '/');
        if (p2)
        {   
            int i = 0;
            
            p2++;
            while (p2 && *p2 != '\0')
            {
                if (*p2 == ' ')
                {
                    break;
                }

                if (i < ONVIF_TOKEN_LEN-1)
                {
                    profile_token[i++] = *p2;  
                } 

                p2++;
            }

            profile_token[i] = '\0';
        }
    }

    if (profile_token[0] == '\0')
    {
        soap_http_err_rly(p_user, rx_msg, 500, "Internal Server Error", NULL, 0);
        return;
    }
	
	p_bufs = (char *)malloc(rlen);
	if (NULL == p_bufs)
	{
		soap_http_err_rly(p_user, rx_msg, 500, "Internal Server Error", NULL, 0);
		return;
	}
	
    if (ONVIF_OK == onvif_GetSnapshot(p_bufs, &rlen, profile_token))
    {
#ifdef HTTPS 
		if (g_onvif_cfg.https_enable)
		{
			SSL_write(p_user->ssl, p_bufs, rlen);
		}
		else
		{
			send(p_user->cfd, p_bufs, rlen, 0);
		}
#else
    	send(p_user->cfd, p_bufs, rlen, 0);
#endif

    }
    else
    {
        soap_http_err_rly(p_user, rx_msg, 500, "Internal Server Error", NULL, 0);
    }

	if (p_bufs)
	{
		free(p_bufs);
		p_bufs = NULL;
	}
}

void soap_GetHttpSystemLog(HTTPCLN * p_user, HTTPMSG * rx_msg)
{
    int tlen;
    int rlen;
    char buff[1024] = {'\0'};
    char * p_bufs;

    strcpy(buff, "test system log");    
    rlen = strlen(buff);
    
    p_bufs = (char *)malloc(rlen + 1024);
	if (NULL == p_bufs)
	{
	    soap_http_err_rly(p_user, rx_msg, 500, "Internal Server Error", NULL, 0);
		return;
	}	
	
	tlen = sprintf(p_bufs,	"HTTP/1.1 200 OK\r\n"
							"Server: hsoap/2.8\r\n"
							"Access-Control-Allow-Origin: *\r\n"
							"Access-Control-Allow-Methods: GET, POST, PUT, OPTIONS\r\n"
							"Access-Control-Allow-Headers: Content-Type, Authorization, X-Custom-Header\r\n"
							"Content-Type: text/plain\r\n"
							"Content-Length: %d\r\n"
							"Connection: close\r\n\r\n",
							rlen);

	memcpy(p_bufs+tlen, buff, rlen);
	tlen += rlen;

#ifdef HTTPS 
	if (g_onvif_cfg.https_enable)
	{
		SSL_write(p_user->ssl, p_bufs, tlen);
	}
	else
	{
		send(p_user->cfd, p_bufs, tlen, 0);
	}
#else
	send(p_user->cfd, p_bufs, tlen, 0);
#endif

	free(p_bufs);
}

void soap_GetHttpAccessLog(HTTPCLN * p_user, HTTPMSG * rx_msg)
{
    int tlen;
    int rlen;
    char buff[1024] = {'\0'};
    char * p_bufs;

    strcpy(buff, "test access log");    
    rlen = strlen(buff);
    
    p_bufs = (char *)malloc(rlen + 1024);
	if (NULL == p_bufs)
	{
	    soap_http_err_rly(p_user, rx_msg, 500, "Internal Server Error", NULL, 0);
		return;
	}	
	
	tlen = sprintf(p_bufs,	"HTTP/1.1 200 OK\r\n"
							"Server: hsoap/2.8\r\n"
							"Access-Control-Allow-Origin: *\r\n"
							"Access-Control-Allow-Methods: GET, POST, PUT, OPTIONS\r\n"
							"Access-Control-Allow-Headers: Content-Type, Authorization, X-Custom-Header\r\n"
							"Content-Type: text/plain\r\n"
							"Content-Length: %d\r\n"
							"Connection: close\r\n\r\n",
							rlen);

	memcpy(p_bufs+tlen, buff, rlen);
	tlen += rlen;

#ifdef HTTPS 
	if (g_onvif_cfg.https_enable)
	{
		SSL_write(p_user->ssl, p_bufs, tlen);
	}
	else
	{
		send(p_user->cfd, p_bufs, tlen, 0);
	}
#else
	send(p_user->cfd, p_bufs, tlen, 0);
#endif

	free(p_bufs);
}

void soap_GetSupportInfo(HTTPCLN * p_user, HTTPMSG * rx_msg)
{
    int tlen;
    int rlen;
    char buff[1024] = {'\0'};
    char * p_bufs;

    strcpy(buff, "test support info");    
    rlen = strlen(buff);
    
    p_bufs = (char *)malloc(rlen + 1024);
	if (NULL == p_bufs)
	{
	    soap_http_err_rly(p_user, rx_msg, 500, "Internal Server Error", NULL, 0);
		return;
	}	
	
	tlen = sprintf(p_bufs,	"HTTP/1.1 200 OK\r\n"
							"Server: hsoap/2.8\r\n"
							"Access-Control-Allow-Origin: *\r\n"
							"Access-Control-Allow-Methods: GET, POST, PUT, OPTIONS\r\n"
							"Access-Control-Allow-Headers: Content-Type, Authorization, X-Custom-Header\r\n"
							"Content-Type: text/plain\r\n"
							"Content-Length: %d\r\n"
							"Connection: close\r\n\r\n",
							rlen);

	memcpy(p_bufs+tlen, buff, rlen);
	tlen += rlen;

#ifdef HTTPS 
	if (g_onvif_cfg.https_enable)
	{
		SSL_write(p_user->ssl, p_bufs, tlen);
	}
	else
	{
		send(p_user->cfd, p_bufs, tlen, 0);
	}
#else
	send(p_user->cfd, p_bufs, tlen, 0);
#endif

	free(p_bufs);
}

void soap_GetSystemBackup(HTTPCLN * p_user, HTTPMSG * rx_msg)
{
    int tlen;
    int rlen;
    char buff[1024] = {'\0'};
    char * p_bufs;

    strcpy(buff, "test system backup");    
    rlen = strlen(buff);
    
    p_bufs = (char *)malloc(rlen + 1024);
	if (NULL == p_bufs)
	{
	    soap_http_err_rly(p_user, rx_msg, 500, "Internal Server Error", NULL, 0);
		return;
	}	
	
	tlen = sprintf(p_bufs,	"HTTP/1.1 200 OK\r\n"
							"Server: hsoap/2.8\r\n"
							"Access-Control-Allow-Origin: *\r\n"
							"Access-Control-Allow-Methods: GET, POST, PUT, OPTIONS\r\n"
							"Access-Control-Allow-Headers: Content-Type, Authorization, X-Custom-Header\r\n"
							"Content-Type: text/plain\r\n"
							"Content-Length: %d\r\n"
							"Connection: close\r\n\r\n",
							rlen);

	memcpy(p_bufs+tlen, buff, rlen);
	tlen += rlen;

#ifdef HTTPS 
	if (g_onvif_cfg.https_enable)
	{
		SSL_write(p_user->ssl, p_bufs, tlen);
	}
	else
	{
		send(p_user->cfd, p_bufs, tlen, 0);
	}
#else
	send(p_user->cfd, p_bufs, tlen, 0);
#endif

	free(p_bufs);
}


int soap_GetOSDs(HTTPCLN * p_user, HTTPMSG * rx_msg, XMLN * p_body, XMLN * p_header)
{
	ONVIF_RET ret;
	XMLN * p_GetOSDs;
	GetOSDs_REQ req;
	
	onvif_print("%s\r\n", __FUNCTION__);

	p_GetOSDs = xml_node_soap_get(p_body, "GetOSDs");
	assert(p_GetOSDs);
	
	memset(&req, 0, sizeof(req));
    
    ret = parse_GetOSDs(p_GetOSDs, &req);
    if (ONVIF_OK == ret)
    {
		return soap_build_send_rly(p_user, rx_msg, build_GetOSDs_rly_xml, (char *)&req, NULL, p_header);
    }

    return soap_build_err_rly(p_user, rx_msg, ret);	
}

int soap_GetOSD(HTTPCLN * p_user, HTTPMSG * rx_msg, XMLN * p_body, XMLN * p_header)
{
	ONVIF_RET ret;
	XMLN * p_GetOSD;
	GetOSD_REQ req;
	
	onvif_print("%s\r\n", __FUNCTION__);

	p_GetOSD = xml_node_soap_get(p_body, "GetOSD");
	assert(p_GetOSD);
	
	memset(&req, 0, sizeof(req));
    
    ret = parse_GetOSD(p_GetOSD, &req);
    if (ONVIF_OK == ret)
    {
		return soap_build_send_rly(p_user, rx_msg, build_GetOSD_rly_xml, (char *)&req, NULL, p_header);
    }

    return soap_build_err_rly(p_user, rx_msg, ret);
} 

int soap_SetOSD(HTTPCLN * p_user, HTTPMSG * rx_msg, XMLN * p_body, XMLN * p_header)
{
	ONVIF_RET ret;
	XMLN * p_SetOSD;
	SetOSD_REQ req;
		
	onvif_print("%s\r\n", __FUNCTION__);

	p_SetOSD = xml_node_soap_get(p_body, "SetOSD");
	assert(p_SetOSD);
	
	memset(&req, 0, sizeof(req));
    
    ret = parse_SetOSD(p_SetOSD, &req);
    if (ONVIF_OK == ret)
    {
    	ret = onvif_SetOSD(&req);
    	if (ONVIF_OK == ret)
    	{
			return soap_build_send_rly(p_user, rx_msg, build_SetOSD_rly_xml, NULL, NULL, p_header);
		}
    }

    return soap_build_err_rly(p_user, rx_msg, ret);
}

int soap_GetOSDOptions(HTTPCLN * p_user, HTTPMSG * rx_msg, XMLN * p_body, XMLN * p_header)
{
	onvif_print("%s\r\n", __FUNCTION__);

	return soap_build_send_rly(p_user, rx_msg, build_GetOSDOptions_rly_xml, NULL, NULL, p_header);	
}

int soap_CreateOSD(HTTPCLN * p_user, HTTPMSG * rx_msg, XMLN * p_body, XMLN * p_header)
{
	ONVIF_RET ret;
	XMLN * p_CreateOSD;
	CreateOSD_REQ req;
	
	onvif_print("%s\r\n", __FUNCTION__);

	p_CreateOSD = xml_node_soap_get(p_body, "CreateOSD");
	assert(p_CreateOSD);
	
	memset(&req, 0, sizeof(req));
    
    ret = parse_CreateOSD(p_CreateOSD, &req);
    if (ONVIF_OK == ret)
    {
    	ret = onvif_CreateOSD(&req);
    	if (ONVIF_OK == ret)
    	{
			return soap_build_send_rly(p_user, rx_msg, build_CreateOSD_rly_xml, req.OSD.token, NULL, p_header);
		}
    }

    return soap_build_err_rly(p_user, rx_msg, ret);
}

int soap_DeleteOSD(HTTPCLN * p_user, HTTPMSG * rx_msg, XMLN * p_body, XMLN * p_header)
{
	ONVIF_RET ret;
	XMLN * p_DeleteOSD;
	DeleteOSD_REQ req;
		
	onvif_print("%s\r\n", __FUNCTION__);

	p_DeleteOSD = xml_node_soap_get(p_body, "DeleteOSD");
	assert(p_DeleteOSD);
	
	memset(&req, 0, sizeof(req));
    
    ret = parse_DeleteOSD(p_DeleteOSD, &req);
    if (ONVIF_OK == ret)
    {
    	ret = onvif_DeleteOSD(&req);
    	if (ONVIF_OK == ret)
    	{
			return soap_build_send_rly(p_user, rx_msg, build_DeleteOSD_rly_xml, NULL, NULL, p_header);
		}
    }

    return soap_build_err_rly(p_user, rx_msg, ret);
}

int soap_StartMulticastStreaming(HTTPCLN * p_user, HTTPMSG * rx_msg, XMLN * p_body, XMLN * p_header)
{
	XMLN * p_StartMulticastStreaming;
	XMLN * p_ProfileToken;
	ONVIF_RET ret = ONVIF_ERR_MissingAttribute;
	
	onvif_print("%s\r\n", __FUNCTION__);

    p_StartMulticastStreaming = xml_node_soap_get(p_body, "StartMulticastStreaming");
	assert(p_StartMulticastStreaming);	
	
	p_ProfileToken = xml_node_soap_get(p_StartMulticastStreaming, "ProfileToken");
	if (p_ProfileToken && p_ProfileToken->data)
	{
		ret = onvif_StartMulticastStreaming(p_ProfileToken->data);
		if (ONVIF_OK == ret)
		{
			return soap_build_send_rly(p_user, rx_msg, build_StartMulticastStreaming_rly_xml, NULL, NULL, p_header);
		}
	}

	return soap_build_err_rly(p_user, rx_msg, ret);
}

int soap_StopMulticastStreaming(HTTPCLN * p_user, HTTPMSG * rx_msg, XMLN * p_body, XMLN * p_header)
{
	XMLN * p_StopMulticastStreaming;
	XMLN * p_ProfileToken;
	ONVIF_RET ret = ONVIF_ERR_MissingAttribute;
	
	onvif_print("%s\r\n", __FUNCTION__);

    p_StopMulticastStreaming = xml_node_soap_get(p_body, "StopMulticastStreaming");
	assert(p_StopMulticastStreaming);	
	
	p_ProfileToken = xml_node_soap_get(p_StopMulticastStreaming, "ProfileToken");
	if (p_ProfileToken && p_ProfileToken->data)
	{
		ret = onvif_StopMulticastStreaming(p_ProfileToken->data);
		if (ONVIF_OK == ret)
		{
			return soap_build_send_rly(p_user, rx_msg, build_StopMulticastStreaming_rly_xml, NULL, NULL, p_header);
		}
	}

	return soap_build_err_rly(p_user, rx_msg, ret);
}

int soap_GetMetadataConfigurations(HTTPCLN * p_user, HTTPMSG * rx_msg, XMLN * p_body, XMLN * p_header)
{
	onvif_print("%s\r\n", __FUNCTION__);

	return soap_build_send_rly(p_user, rx_msg, build_GetMetadataConfigurations_rly_xml, NULL, NULL, p_header);
}

int soap_GetMetadataConfiguration(HTTPCLN * p_user, HTTPMSG * rx_msg, XMLN * p_body, XMLN * p_header)
{	
	XMLN * p_GetMetadataConfiguration;
	XMLN * p_ConfigurationToken;
	ONVIF_RET ret = ONVIF_ERR_MissingAttribute;
	
	onvif_print("%s\r\n", __FUNCTION__);

    p_GetMetadataConfiguration = xml_node_soap_get(p_body, "GetMetadataConfiguration");
	assert(p_GetMetadataConfiguration);	
	
	p_ConfigurationToken = xml_node_soap_get(p_GetMetadataConfiguration, "ConfigurationToken");
	if (p_ConfigurationToken && p_ConfigurationToken->data)
	{
		return soap_build_send_rly(p_user, rx_msg, build_GetMetadataConfiguration_rly_xml, p_ConfigurationToken->data, NULL, p_header);
	}

	return soap_build_err_rly(p_user, rx_msg, ret);
}

int soap_GetCompatibleMetadataConfigurations(HTTPCLN * p_user, HTTPMSG * rx_msg, XMLN * p_body, XMLN * p_header)
{
	XMLN * p_GetCompatibleMetadataConfigurations;
	XMLN * p_ProfileToken;
	ONVIF_RET ret = ONVIF_ERR_MissingAttribute;
	
	onvif_print("%s\r\n", __FUNCTION__);

    p_GetCompatibleMetadataConfigurations = xml_node_soap_get(p_body, "GetCompatibleMetadataConfigurations");
	assert(p_GetCompatibleMetadataConfigurations);	
	
	p_ProfileToken = xml_node_soap_get(p_GetCompatibleMetadataConfigurations, "ProfileToken");
	if (p_ProfileToken && p_ProfileToken->data)
	{
		return soap_build_send_rly(p_user, rx_msg, build_GetCompatibleMetadataConfigurations_rly_xml, p_ProfileToken->data, NULL, p_header);
	}

	return soap_build_err_rly(p_user, rx_msg, ret);
}

int soap_GetMetadataConfigurationOptions(HTTPCLN * p_user, HTTPMSG * rx_msg, XMLN * p_body, XMLN * p_header)
{
	ONVIF_RET ret;
	XMLN * p_GetMetadataConfigurationOptions;
	GetMetadataConfigurationOptions_REQ req;
		
	onvif_print("%s\r\n", __FUNCTION__);

	p_GetMetadataConfigurationOptions = xml_node_soap_get(p_body, "GetMetadataConfigurationOptions");
	assert(p_GetMetadataConfigurationOptions);
	
	memset(&req, 0, sizeof(req));
    
    ret = parse_GetMetadataConfigurationOptions(p_GetMetadataConfigurationOptions, &req);
    if (ONVIF_OK == ret)
    {
		return soap_build_send_rly(p_user, rx_msg, build_GetMetadataConfigurationOptions_rly_xml, (char *)&req, NULL, p_header);
    }

    return soap_build_err_rly(p_user, rx_msg, ret);
}

int soap_SetMetadataConfiguration(HTTPCLN * p_user, HTTPMSG * rx_msg, XMLN * p_body, XMLN * p_header)
{
	ONVIF_RET ret;
	XMLN * p_SetMetadataConfiguration;
	SetMetadataConfiguration_REQ req;
		
	onvif_print("%s\r\n", __FUNCTION__);

	p_SetMetadataConfiguration = xml_node_soap_get(p_body, "SetMetadataConfiguration");
	assert(p_SetMetadataConfiguration);
	
	memset(&req, 0, sizeof(req));
    
    ret = parse_SetMetadataConfiguration(p_SetMetadataConfiguration, &req);
    if (ONVIF_OK == ret)
    {
    	ret = onvif_SetMetadataConfiguration(&req);
    	if (ONVIF_OK == ret)
    	{
			return soap_build_send_rly(p_user, rx_msg, build_SetMetadataConfiguration_rly_xml, NULL, NULL, p_header);
		}
    }

    return soap_build_err_rly(p_user, rx_msg, ret);
}

int soap_AddMetadataConfiguration(HTTPCLN * p_user, HTTPMSG * rx_msg, XMLN * p_body, XMLN * p_header)
{
	ONVIF_RET ret;
	XMLN * p_AddMetadataConfiguration;
	AddMetadataConfiguration_REQ req;
		
	onvif_print("%s\r\n", __FUNCTION__);

	p_AddMetadataConfiguration = xml_node_soap_get(p_body, "AddMetadataConfiguration");
	assert(p_AddMetadataConfiguration);
	
	memset(&req, 0, sizeof(req));
    
    ret = parse_AddMetadataConfiguration(p_AddMetadataConfiguration, &req);
    if (ONVIF_OK == ret)
    {
    	ret = onvif_AddMetadataConfiguration(&req);
    	if (ONVIF_OK == ret)
    	{
			return soap_build_send_rly(p_user, rx_msg, build_AddMetadataConfiguration_rly_xml, NULL, NULL, p_header);
		}
    }

    return soap_build_err_rly(p_user, rx_msg, ret);
}

int soap_RemoveMetadataConfiguration(HTTPCLN * p_user, HTTPMSG * rx_msg, XMLN * p_body, XMLN * p_header)
{
	XMLN * p_RemoveMetadataConfiguration;
	XMLN * p_ProfileToken;
	ONVIF_RET ret = ONVIF_ERR_MissingAttribute;
	
	onvif_print("%s\r\n", __FUNCTION__);

    p_RemoveMetadataConfiguration = xml_node_soap_get(p_body, "RemoveMetadataConfiguration");
	assert(p_RemoveMetadataConfiguration);	
	
	p_ProfileToken = xml_node_soap_get(p_RemoveMetadataConfiguration, "ProfileToken");
	if (p_ProfileToken && p_ProfileToken->data)
	{
		ret = onvif_RemoveMetadataConfiguration(p_ProfileToken->data);
		if (ONVIF_OK == ret)
		{
			return soap_build_send_rly(p_user, rx_msg, build_RemoveMetadataConfiguration_rly_xml, NULL, NULL, p_header);
		}	
	}

	return soap_build_err_rly(p_user, rx_msg, ret);
}

int soap_GetStatus(HTTPCLN * p_user, HTTPMSG * rx_msg, XMLN * p_body, XMLN * p_header)
{
	char * post;
	XMLN * p_GetStatus;
	
	onvif_print("%s\r\n", __FUNCTION__);

	p_GetStatus = xml_node_soap_get(p_body, "GetStatus");
	assert(p_GetStatus);

	post = rx_msg->first_line.value_string;
	
#ifdef PTZ_SUPPORT	
	if (strstr(post, "ptz"))
	{
		XMLN * p_ProfileToken = xml_node_soap_get(p_GetStatus, "ProfileToken");
		if (p_ProfileToken && p_ProfileToken->data)
		{
			return soap_build_send_rly(p_user, rx_msg, build_ptz_GetStatus_rly_xml, p_ProfileToken->data, NULL, p_header);
		}
	}
	else 
#endif	
	if (strstr(post, "image"))
	{
		XMLN * p_VideoSourceToken = xml_node_soap_get(p_GetStatus, "VideoSourceToken");
		if (p_VideoSourceToken && p_VideoSourceToken->data)
		{
			return soap_build_send_rly(p_user, rx_msg, build_img_GetStatus_rly_xml, p_VideoSourceToken->data, NULL, p_header);
		}
	}	
	
	return soap_err_def_rly(p_user, rx_msg);
}

int soap_Stop(HTTPCLN * p_user, HTTPMSG * rx_msg, XMLN * p_body, XMLN * p_header)
{
	char * post;
	XMLN * p_Stop;
	
	onvif_print("%s\r\n", __FUNCTION__);

	p_Stop = xml_node_soap_get(p_body, "Stop");
	assert(p_Stop);

	post = rx_msg->first_line.value_string;

#ifdef PTZ_SUPPORT	
	if (strstr(post, "ptz"))
	{
		ONVIF_RET ret;
		PTZ_Stop_REQ req;
		
		memset(&req, 0, sizeof(req));		

		ret = parse_ptz_Stop(p_Stop, &req);
		if (ONVIF_OK == ret)
		{
			ret = onvif_ptz_Stop(&req);
			if (ONVIF_OK == ret)
			{
				return soap_build_send_rly(p_user, rx_msg, build_ptz_Stop_rly_xml, NULL, NULL, p_header);
			}
		}

		return soap_build_err_rly(p_user, rx_msg, ret);
	}
	else 
#endif	
	if (strstr(post, "image"))
	{
		XMLN * p_VideoSourceToken = xml_node_soap_get(p_Stop, "VideoSourceToken");
		if (p_VideoSourceToken && p_VideoSourceToken->data)
		{
		    ONVIF_RET ret;
		    
		    ret = onvif_img_Stop(p_VideoSourceToken->data);
		    if (ONVIF_OK == ret)
		    {
			    return soap_build_send_rly(p_user, rx_msg, build_img_Stop_rly_xml, p_VideoSourceToken->data, NULL, p_header);
			}
		}
	}
	
	return soap_err_def_rly(p_user, rx_msg);
}

int soap_GetVideoSourceModes(HTTPCLN * p_user, HTTPMSG * rx_msg, XMLN * p_body, XMLN * p_header)
{
    ONVIF_RET ret;
    XMLN * p_GetVideoSourceModes;
    GetVideoSourceModes_REQ req;
    
	onvif_print("%s\r\n", __FUNCTION__);

	p_GetVideoSourceModes = xml_node_soap_get(p_body, "GetVideoSourceModes");
    assert(p_GetVideoSourceModes);

	memset(&req, 0, sizeof(req));
	
    ret = parse_GetVideoSourceModes(p_GetVideoSourceModes, &req);
    if (ONVIF_OK == ret)
    {
		return soap_build_send_rly(p_user, rx_msg, build_GetVideoSourceModes_rly_xml, NULL, NULL, p_header);
    }

    return soap_build_err_rly(p_user, rx_msg, ret);
}

int soap_SetVideoSourceMode(HTTPCLN * p_user, HTTPMSG * rx_msg, XMLN * p_body, XMLN * p_header)
{
    ONVIF_RET ret;
    XMLN * p_SetVideoSourceMode;
    SetVideoSourceMode_REQ req;
    
	onvif_print("%s\r\n", __FUNCTION__);

	p_SetVideoSourceMode = xml_node_soap_get(p_body, "SetVideoSourceMode");
    assert(p_SetVideoSourceMode);

	memset(&req, 0, sizeof(req));
	
    ret = parse_SetVideoSourceMode(p_SetVideoSourceMode, &req);
    if (ONVIF_OK == ret)
    {
        SetVideoSourceMode_RES res;
        memset(&res, 0, sizeof(res));
        
        ret = onvif_SetVideoSourceMode(&req, &res);
    	if (ONVIF_OK == ret)
    	{
		    return soap_build_send_rly(p_user, rx_msg, build_SetVideoSourceMode_rly_xml, (char*)&res, NULL, p_header);
		}
    }

    return soap_build_err_rly(p_user, rx_msg, ret);
}

#ifdef AUDIO_SUPPORT

int soap_AddAudioSourceConfiguration(HTTPCLN * p_user, HTTPMSG * rx_msg, XMLN * p_body, XMLN * p_header)
{
    ONVIF_RET ret;
    XMLN * p_AddAudioSourceConfiguration;
    AddAudioSourceConfiguration_REQ req;;
    
	onvif_print("%s\r\n", __FUNCTION__);

	p_AddAudioSourceConfiguration = xml_node_soap_get(p_body, "AddAudioSourceConfiguration");
    assert(p_AddAudioSourceConfiguration);

	memset(&req, 0, sizeof(req));
	
    ret = parse_AddAudioSourceConfiguration(p_AddAudioSourceConfiguration, &req);
    if (ONVIF_OK == ret)
    {
    	ret = onvif_AddAudioSourceConfiguration(&req);
    	if (ONVIF_OK == ret)
    	{
    		return soap_build_send_rly(p_user, rx_msg, build_AddAudioSourceConfiguration_rly_xml, NULL, NULL, p_header);
    	} 	
    }

    return soap_build_err_rly(p_user, rx_msg, ret);
}

int soap_RemoveAudioSourceConfiguration(HTTPCLN * p_user, HTTPMSG * rx_msg, XMLN * p_body, XMLN * p_header)
{
    XMLN * p_RemoveAudioSourceConfiguration;
    XMLN * p_ProfileToken;
    ONVIF_RET ret = ONVIF_ERR_MissingAttribute;
    
	onvif_print("%s\r\n", __FUNCTION__);
	
	p_RemoveAudioSourceConfiguration = xml_node_soap_get(p_body, "RemoveAudioSourceConfiguration");
	assert(p_RemoveAudioSourceConfiguration);	
	
	p_ProfileToken = xml_node_soap_get(p_RemoveAudioSourceConfiguration, "ProfileToken");
    if (p_ProfileToken && p_ProfileToken->data)
    {
    	ret = onvif_RemoveAudioSourceConfiguration(p_ProfileToken->data);
    	if (ONVIF_OK == ret)
    	{
    		return soap_build_send_rly(p_user, rx_msg, build_RemoveAudioSourceConfiguration_rly_xml, NULL, NULL, p_header);
    	}
    }

    return soap_build_err_rly(p_user, rx_msg, ret);
}

int soap_AddAudioEncoderConfiguration(HTTPCLN * p_user, HTTPMSG * rx_msg, XMLN * p_body, XMLN * p_header)
{
    ONVIF_RET ret;
    XMLN * p_AddAudioEncoderConfiguration;
    AddAudioEncoderConfiguration_REQ req;
    
	onvif_print("%s\r\n", __FUNCTION__);

	p_AddAudioEncoderConfiguration = xml_node_soap_get(p_body, "AddAudioEncoderConfiguration");
	assert(p_AddAudioEncoderConfiguration);
	
	memset(&req, 0, sizeof(req));
	
	ret = parse_AddAudioEncoderConfiguration(p_AddAudioEncoderConfiguration, &req);
	if (ONVIF_OK == ret)
	{
		ret = onvif_AddAudioEncoderConfiguration(&req);
		if (ONVIF_OK == ret)
		{
			return soap_build_send_rly(p_user, rx_msg, build_AddAudioEncoderConfiguration_rly_xml, NULL, NULL, p_header);
		}
	}

	return soap_build_err_rly(p_user, rx_msg, ret);
}

int soap_RemoveAudioEncoderConfiguration(HTTPCLN * p_user, HTTPMSG * rx_msg, XMLN * p_body, XMLN * p_header)
{	
    XMLN * p_RemoveAudioEncoderConfiguration;
    XMLN * p_ProfileToken;
    ONVIF_RET ret = ONVIF_ERR_MissingAttribute;
    
	onvif_print("%s\r\n", __FUNCTION__);
	
	p_RemoveAudioEncoderConfiguration = xml_node_soap_get(p_body, "RemoveAudioEncoderConfiguration");
	assert(p_RemoveAudioEncoderConfiguration);
	
	p_ProfileToken = xml_node_soap_get(p_RemoveAudioEncoderConfiguration, "ProfileToken");
    if (p_ProfileToken && p_ProfileToken->data)
    {
    	ret = onvif_RemoveAudioEncoderConfiguration(p_ProfileToken->data);
    	if (ONVIF_OK == ret)
    	{
    		return soap_build_send_rly(p_user, rx_msg, build_RemoveAudioEncoderConfiguration_rly_xml, NULL, NULL, p_header);
    	}
    }

    return soap_build_err_rly(p_user, rx_msg, ret);
}

int soap_GetAudioSources(HTTPCLN * p_user, HTTPMSG * rx_msg, XMLN * p_body, XMLN * p_header)
{
	onvif_print("%s\r\n", __FUNCTION__);

	return soap_build_send_rly(p_user, rx_msg, build_GetAudioSources_rly_xml, NULL, NULL, p_header);
}

int soap_GetAudioEncoderConfigurations(HTTPCLN * p_user, HTTPMSG * rx_msg, XMLN * p_body, XMLN * p_header)
{
	onvif_print("%s\r\n", __FUNCTION__);

	return soap_build_send_rly(p_user, rx_msg, build_GetAudioEncoderConfigurations_rly_xml, NULL, NULL, p_header);
}

int soap_GetCompatibleAudioEncoderConfigurations(HTTPCLN * p_user, HTTPMSG * rx_msg, XMLN * p_body, XMLN * p_header)
{
    XMLN * p_GetCompatibleAudioEncoderConfigurations;
    XMLN * p_ProfileToken;
    
	onvif_print("%s\r\n", __FUNCTION__);

	p_GetCompatibleAudioEncoderConfigurations = xml_node_soap_get(p_body, "GetCompatibleAudioEncoderConfigurations");
    assert(p_GetCompatibleAudioEncoderConfigurations);

    p_ProfileToken = xml_node_soap_get(p_GetCompatibleAudioEncoderConfigurations, "ProfileToken");
    if (p_ProfileToken && p_ProfileToken->data)
    {
    	return soap_build_send_rly(p_user, rx_msg, build_GetCompatibleAudioEncoderConfigurations_rly_xml, p_ProfileToken->data, NULL, p_header);
    }
	
    return soap_build_err_rly(p_user, rx_msg, ONVIF_ERR_MissingAttribute);
}

int soap_GetAudioSourceConfigurations(HTTPCLN * p_user, HTTPMSG * rx_msg, XMLN * p_body, XMLN * p_header)
{
	onvif_print("%s\r\n", __FUNCTION__);

	return soap_build_send_rly(p_user, rx_msg, build_GetAudioSourceConfigurations_rly_xml, NULL, NULL, p_header);
}

int soap_GetCompatibleAudioSourceConfigurations(HTTPCLN * p_user, HTTPMSG * rx_msg, XMLN * p_body, XMLN * p_header)
{
	XMLN * p_GetCompatibleAudioSourceConfigurations;
	XMLN * p_ProfileToken;
		
	onvif_print("%s\r\n", __FUNCTION__);

	p_GetCompatibleAudioSourceConfigurations = xml_node_soap_get(p_body, "GetCompatibleAudioSourceConfigurations");
    assert(p_GetCompatibleAudioSourceConfigurations);

    p_ProfileToken = xml_node_soap_get(p_GetCompatibleAudioSourceConfigurations, "ProfileToken");
	if (p_ProfileToken && p_ProfileToken->data)
	{
	    return soap_build_send_rly(p_user, rx_msg, build_GetCompatibleAudioSourceConfigurations_rly_xml, p_ProfileToken->data, NULL, p_header);
	}
	else 
	{
		return soap_err_def2_rly(p_user, rx_msg, ERR_SENDER, ERR_MISSINGATTR, NULL, "Missing Attribute");
	}
}

int soap_GetAudioSourceConfigurationOptions(HTTPCLN * p_user, HTTPMSG * rx_msg, XMLN * p_body, XMLN * p_header)
{
	ONVIF_RET ret;
	XMLN * p_GetAudioSourceConfigurationOptions;
	GetAudioSourceConfigurationOptions_REQ req;
	
	onvif_print("%s\r\n", __FUNCTION__);

	p_GetAudioSourceConfigurationOptions = xml_node_soap_get(p_body, "GetAudioSourceConfigurationOptions");
    assert(p_GetAudioSourceConfigurationOptions);
    
	memset(&req, 0, sizeof(req));
	
	ret = parse_GetAudioSourceConfigurationOptions(p_GetAudioSourceConfigurationOptions, &req);
	if (ONVIF_OK == ret)
	{
		return soap_build_send_rly(p_user, rx_msg, build_GetAudioSourceConfigurationOptions_rly_xml, (char *)&req, NULL, p_header);
	}

	return soap_build_err_rly(p_user, rx_msg, ret);
}

int soap_GetAudioSourceConfiguration(HTTPCLN * p_user, HTTPMSG * rx_msg, XMLN * p_body, XMLN * p_header)
{
	XMLN * p_GetAudioSourceConfiguration;
	XMLN * p_ConfigurationToken;
	
    onvif_print("%s\r\n", __FUNCTION__);

    p_GetAudioSourceConfiguration = xml_node_soap_get(p_body, "GetAudioSourceConfiguration");
    assert(p_GetAudioSourceConfiguration);

	p_ConfigurationToken = xml_node_soap_get(p_GetAudioSourceConfiguration, "ConfigurationToken");
	if (p_ConfigurationToken && p_ConfigurationToken->data)
	{
	    return soap_build_send_rly(p_user, rx_msg, build_GetAudioSourceConfiguration_rly_xml, p_ConfigurationToken->data, NULL, p_header);
	}
	else 
	{
		return soap_err_def2_rly(p_user, rx_msg, ERR_SENDER, ERR_MISSINGATTR, NULL, "Missing Attribute");
	}
}

int soap_SetAudioSourceConfiguration(HTTPCLN * p_user, HTTPMSG * rx_msg, XMLN * p_body, XMLN * p_header)
{
	ONVIF_RET ret;
	XMLN * p_SetAudioSourceConfiguration;
	SetAudioSourceConfiguration_REQ req;
	
	onvif_print("%s\r\n", __FUNCTION__);

    p_SetAudioSourceConfiguration = xml_node_soap_get(p_body, "SetAudioSourceConfiguration");
    assert(p_SetAudioSourceConfiguration);
    
    memset(&req, 0, sizeof(req));

    ret = parse_SetAudioSourceConfiguration(p_SetAudioSourceConfiguration, &req);
    if (ONVIF_OK == ret)
    {
    	ret = onvif_SetAudioSourceConfiguration(&req);
    	if (ONVIF_OK == ret)
    	{
    		return soap_build_send_rly(p_user, rx_msg, build_SetAudioSourceConfiguration_rly_xml, NULL, NULL, p_header);
    	}
    }    

    return soap_build_err_rly(p_user, rx_msg, ret);
}

int soap_GetAudioEncoderConfiguration(HTTPCLN * p_user, HTTPMSG * rx_msg, XMLN * p_body, XMLN * p_header)
{
    XMLN * p_GetAudioEncoderConfiguration;
    XMLN * p_ConfigurationToken;
    
	onvif_print("%s\r\n", __FUNCTION__);

    p_GetAudioEncoderConfiguration = xml_node_soap_get(p_body, "GetAudioEncoderConfiguration");
    assert(p_GetAudioEncoderConfiguration);

	p_ConfigurationToken = xml_node_soap_get(p_GetAudioEncoderConfiguration, "ConfigurationToken");
	if (p_ConfigurationToken && p_ConfigurationToken->data)
	{
		return soap_build_send_rly(p_user, rx_msg, build_GetAudioEncoderConfiguration_rly_xml, p_ConfigurationToken->data, NULL, p_header);
	}
	else 
	{
		return soap_err_def2_rly(p_user, rx_msg, ERR_SENDER, ERR_MISSINGATTR, NULL, "Missing Attribute");
	}
}

int soap_SetAudioEncoderConfiguration(HTTPCLN * p_user, HTTPMSG * rx_msg, XMLN * p_body, XMLN * p_header)
{
    ONVIF_RET ret;
    XMLN * p_SetAudioEncoderConfiguration;
    SetAudioEncoderConfiguration_REQ req;
    
	onvif_print("%s\r\n", __FUNCTION__);

    p_SetAudioEncoderConfiguration = xml_node_soap_get(p_body, "SetAudioEncoderConfiguration");
    assert(p_SetAudioEncoderConfiguration);
	
	memset(&req, 0, sizeof(req));

	ret = parse_SetAudioEncoderConfiguration(p_SetAudioEncoderConfiguration, &req);
    if (ONVIF_OK == ret)
    {
        ret = onvif_SetAudioEncoderConfiguration(&req);
        if (ONVIF_OK == ret)
        {
        	return soap_build_send_rly(p_user, rx_msg, build_SetAudioEncoderConfiguration_rly_xml, NULL, NULL, p_header);
        }
    }
    
    return soap_build_err_rly(p_user, rx_msg, ret);
}

int soap_GetAudioEncoderConfigurationOptions(HTTPCLN * p_user, HTTPMSG * rx_msg, XMLN * p_body, XMLN * p_header)
{
	ONVIF_RET ret;
	XMLN * p_GetAudioEncoderConfigurationOptions;
	GetAudioEncoderConfigurationOptions_REQ req;
	
    onvif_print("%s\r\n", __FUNCTION__);

	p_GetAudioEncoderConfigurationOptions = xml_node_soap_get(p_body, "GetAudioEncoderConfigurationOptions");
    assert(p_GetAudioEncoderConfigurationOptions);    
	
    memset(&req, 0, sizeof(req));

	ret = parse_GetAudioEncoderConfigurationOptions(p_GetAudioEncoderConfigurationOptions, &req);
	if (ONVIF_OK == ret)
	{
		return soap_build_send_rly(p_user, rx_msg, build_GetAudioEncoderConfigurationOptions_rly_xml, (char *)&req, NULL, p_header);	
	}

    return soap_build_err_rly(p_user, rx_msg, ret);  
}

int soap_AddAudioDecoderConfiguration(HTTPCLN * p_user, HTTPMSG * rx_msg, XMLN * p_body, XMLN * p_header)
{
    ONVIF_RET ret;
    XMLN * p_AddAudioDecoderConfiguration;
    AddAudioDecoderConfiguration_REQ req;;
    
	onvif_print("%s\r\n", __FUNCTION__);

	p_AddAudioDecoderConfiguration = xml_node_soap_get(p_body, "AddAudioDecoderConfiguration");
    assert(p_AddAudioDecoderConfiguration);

	memset(&req, 0, sizeof(req));
	
    ret = parse_AddAudioDecoderConfiguration(p_AddAudioDecoderConfiguration, &req);
    if (ONVIF_OK == ret)
    {
    	ret = onvif_AddAudioDecoderConfiguration(&req);
    	if (ONVIF_OK == ret)
    	{
    		return soap_build_send_rly(p_user, rx_msg, build_AddAudioDecoderConfiguration_rly_xml, NULL, NULL, p_header);
    	} 	
    }

    return soap_build_err_rly(p_user, rx_msg, ret);
}
   
int soap_GetAudioDecoderConfigurations(HTTPCLN * p_user, HTTPMSG * rx_msg, XMLN * p_body, XMLN * p_header)
{
    onvif_print("%s\r\n", __FUNCTION__);

	return soap_build_send_rly(p_user, rx_msg, build_GetAudioDecoderConfigurations_rly_xml, NULL, NULL, p_header);
}
  
int soap_GetAudioDecoderConfiguration(HTTPCLN * p_user, HTTPMSG * rx_msg, XMLN * p_body, XMLN * p_header)
{
    XMLN * p_GetAudioDecoderConfiguration;
    XMLN * p_ConfigurationToken;
    
	onvif_print("%s\r\n", __FUNCTION__);

    p_GetAudioDecoderConfiguration = xml_node_soap_get(p_body, "GetAudioDecoderConfiguration");
    assert(p_GetAudioDecoderConfiguration);

	p_ConfigurationToken = xml_node_soap_get(p_GetAudioDecoderConfiguration, "ConfigurationToken");
	if (p_ConfigurationToken && p_ConfigurationToken->data)
	{
		return soap_build_send_rly(p_user, rx_msg, build_GetAudioDecoderConfiguration_rly_xml, p_ConfigurationToken->data, NULL, p_header);
	}
	else 
	{
		return soap_err_def2_rly(p_user, rx_msg, ERR_SENDER, ERR_MISSINGATTR, NULL, "Missing Attribute");
	}
}
  
int soap_RemoveAudioDecoderConfiguration(HTTPCLN * p_user, HTTPMSG * rx_msg, XMLN * p_body, XMLN * p_header)
{
    ONVIF_RET ret;
    XMLN * p_RemoveAudioDecoderConfiguration;
    RemoveAudioDecoderConfiguration_REQ req;;
    
	onvif_print("%s\r\n", __FUNCTION__);

	p_RemoveAudioDecoderConfiguration = xml_node_soap_get(p_body, "RemoveAudioDecoderConfiguration");
    assert(p_RemoveAudioDecoderConfiguration);

	memset(&req, 0, sizeof(req));
	
    ret = parse_RemoveAudioDecoderConfiguration(p_RemoveAudioDecoderConfiguration, &req);
    if (ONVIF_OK == ret)
    {
    	ret = onvif_RemoveAudioDecoderConfiguration(&req);
    	if (ONVIF_OK == ret)
    	{
    		return soap_build_send_rly(p_user, rx_msg, build_RemoveAudioDecoderConfiguration_rly_xml, NULL, NULL, p_header);
    	} 	
    }

    return soap_build_err_rly(p_user, rx_msg, ret);
}
  
int soap_SetAudioDecoderConfiguration(HTTPCLN * p_user, HTTPMSG * rx_msg, XMLN * p_body, XMLN * p_header)
{
    ONVIF_RET ret;
    XMLN * p_SetAudioDecoderConfiguration;
    SetAudioDecoderConfiguration_REQ req;
    
	onvif_print("%s\r\n", __FUNCTION__);

    p_SetAudioDecoderConfiguration = xml_node_soap_get(p_body, "SetAudioDecoderConfiguration");
    assert(p_SetAudioDecoderConfiguration);
	
	memset(&req, 0, sizeof(req));

	ret = parse_SetAudioDecoderConfiguration(p_SetAudioDecoderConfiguration, &req);
    if (ONVIF_OK == ret)
    {
        ret = onvif_SetAudioDecoderConfiguration(&req);
        if (ONVIF_OK == ret)
        {
        	return soap_build_send_rly(p_user, rx_msg, build_SetAudioDecoderConfiguration_rly_xml, NULL, NULL, p_header);
        }
    }
    
    return soap_build_err_rly(p_user, rx_msg, ret);
}
  
int soap_GetAudioDecoderConfigurationOptions(HTTPCLN * p_user, HTTPMSG * rx_msg, XMLN * p_body, XMLN * p_header)
{
    ONVIF_RET ret;
	XMLN * p_GetAudioDecoderConfigurationOptions;
	GetAudioDecoderConfigurationOptions_REQ req;
	
    onvif_print("%s\r\n", __FUNCTION__);

	p_GetAudioDecoderConfigurationOptions = xml_node_soap_get(p_body, "GetAudioDecoderConfigurationOptions");
    assert(p_GetAudioDecoderConfigurationOptions);    
	
    memset(&req, 0, sizeof(req));

	ret = parse_GetAudioDecoderConfigurationOptions(p_GetAudioDecoderConfigurationOptions, &req);
	if (ONVIF_OK == ret)
	{
		return soap_build_send_rly(p_user, rx_msg, build_GetAudioDecoderConfigurationOptions_rly_xml, (char *)&req, NULL, p_header);	
	}

    return soap_build_err_rly(p_user, rx_msg, ret);
}
  
int soap_GetCompatibleAudioDecoderConfigurations(HTTPCLN * p_user, HTTPMSG * rx_msg, XMLN * p_body, XMLN * p_header)
{
    XMLN * p_GetCompatibleAudioDecoderConfigurations;
    XMLN * p_ProfileToken;
    
	onvif_print("%s\r\n", __FUNCTION__);

	p_GetCompatibleAudioDecoderConfigurations = xml_node_soap_get(p_body, "GetCompatibleAudioDecoderConfigurations");
    assert(p_GetCompatibleAudioDecoderConfigurations);

    p_ProfileToken = xml_node_soap_get(p_GetCompatibleAudioDecoderConfigurations, "ProfileToken");
    if (p_ProfileToken && p_ProfileToken->data)
    {
    	return soap_build_send_rly(p_user, rx_msg, build_GetCompatibleAudioDecoderConfigurations_rly_xml, p_ProfileToken->data, NULL, p_header);
    }
	
    return soap_build_err_rly(p_user, rx_msg, ONVIF_ERR_MissingAttribute);
}

#endif // end of AUDIO_SUPPORT


#ifdef PTZ_SUPPORT

int soap_GetNodes(HTTPCLN * p_user, HTTPMSG * rx_msg, XMLN * p_body, XMLN * p_header)
{
	onvif_print("%s\r\n", __FUNCTION__);

	return soap_build_send_rly(p_user, rx_msg, build_GetNodes_rly_xml, NULL, NULL, p_header);
}

int soap_GetNode(HTTPCLN * p_user, HTTPMSG * rx_msg, XMLN * p_body, XMLN * p_header)
{
	XMLN * p_GetNode;
	XMLN * p_NodeToken;
		
	onvif_print("%s\r\n", __FUNCTION__);

	p_GetNode = xml_node_soap_get(p_body, "GetNode");
	assert(p_GetNode);
	
    p_NodeToken = xml_node_soap_get(p_GetNode, "tptz:NodeToken");
	if (p_NodeToken && p_NodeToken->data)
	{
		return soap_build_send_rly(p_user, rx_msg, build_GetNode_rly_xml, p_NodeToken->data, NULL, p_header);
	}
	
	return soap_err_def_rly(p_user, rx_msg);
}

int soap_GetConfigurations(HTTPCLN * p_user, HTTPMSG * rx_msg, XMLN * p_body, XMLN * p_header)
{
	onvif_print("%s\r\n", __FUNCTION__);
		
	return soap_build_send_rly(p_user, rx_msg, build_GetConfigurations_rly_xml, NULL, NULL, p_header);
}

int soap_GetCompatibleConfigurations(HTTPCLN * p_user, HTTPMSG * rx_msg, XMLN * p_body, XMLN * p_header)
{
    ONVIF_RET ret;
	XMLN * p_GetCompatibleConfigurations;
	GetCompatibleConfigurations_REQ req;
		
	onvif_print("%s\r\n", __FUNCTION__);

	p_GetCompatibleConfigurations = xml_node_soap_get(p_body, "GetCompatibleConfigurations");
	assert(p_GetCompatibleConfigurations);
	
	memset(&req, 0, sizeof(req));
    
    ret = parse_GetCompatibleConfigurations(p_GetCompatibleConfigurations, &req);
    if (ONVIF_OK == ret)
    {
		return soap_build_send_rly(p_user, rx_msg, build_GetCompatibleConfigurations_rly_xml, NULL, NULL, p_header);
    }

    return soap_build_err_rly(p_user, rx_msg, ret);
}

int soap_GetConfiguration(HTTPCLN * p_user, HTTPMSG * rx_msg, XMLN * p_body, XMLN * p_header)
{
	XMLN * p_GetConfiguration;
	XMLN * p_PTZConfigurationToken;
	
	onvif_print("%s\r\n", __FUNCTION__);

	p_GetConfiguration = xml_node_soap_get(p_body, "GetConfiguration");
	assert(p_GetConfiguration);
	
	p_PTZConfigurationToken = xml_node_soap_get(p_GetConfiguration, "tptz:PTZConfigurationToken");
	if (p_PTZConfigurationToken && p_PTZConfigurationToken->data)
	{
		return soap_build_send_rly(p_user, rx_msg, build_GetConfiguration_rly_xml, p_PTZConfigurationToken->data, NULL, p_header);
	}
	
	return soap_err_def_rly(p_user, rx_msg);
}

int soap_SetConfiguration(HTTPCLN * p_user, HTTPMSG * rx_msg, XMLN * p_body, XMLN * p_header)
{
	ONVIF_RET ret;
	XMLN * p_SetConfiguration;
	SetConfiguration_REQ req;
		
	onvif_print("%s\r\n", __FUNCTION__);

	p_SetConfiguration = xml_node_soap_get(p_body, "SetConfiguration");
	assert(p_SetConfiguration);
	
	memset(&req, 0, sizeof(req));
    
    ret = parse_SetConfiguration(p_SetConfiguration, &req);
    if (ONVIF_OK == ret)
    {
    	ret = onvif_SetConfiguration(&req);
    	if (ONVIF_OK == ret)
    	{
			return soap_build_send_rly(p_user, rx_msg, build_SetConfiguration_rly_xml, NULL, NULL, p_header);
		}
    }

    return soap_build_err_rly(p_user, rx_msg, ret);
}

int soap_AddPTZConfiguration(HTTPCLN * p_user, HTTPMSG * rx_msg, XMLN * p_body, XMLN * p_header)
{
	ONVIF_RET ret;
	XMLN * p_AddPTZConfiguration;
	AddPTZConfiguration_REQ req;
	
	onvif_print("%s\r\n", __FUNCTION__);

	p_AddPTZConfiguration = xml_node_soap_get(p_body, "AddPTZConfiguration");
	assert(p_AddPTZConfiguration);	
	
	memset(&req, 0, sizeof(req));

	ret = parse_AddPTZConfiguration(p_AddPTZConfiguration, &req);
	if (ONVIF_OK == ret)
	{
		ret = onvif_AddPTZConfiguration(&req);
		if (ONVIF_OK == ret)
    	{
    		return soap_build_send_rly(p_user, rx_msg, build_AddPTZConfiguration_rly_xml, NULL, NULL, p_header);
    	}
	}
	
	return soap_build_err_rly(p_user, rx_msg, ret);
}

int soap_RemovePTZConfiguration(HTTPCLN * p_user, HTTPMSG * rx_msg, XMLN * p_body, XMLN * p_header)
{
	XMLN * p_RemovePTZConfiguration;
	XMLN * p_ProfileToken;
	
	onvif_print("%s\r\n", __FUNCTION__);

	p_RemovePTZConfiguration = xml_node_soap_get(p_body, "RemovePTZConfiguration");
	assert(p_RemovePTZConfiguration);

	p_ProfileToken = xml_node_soap_get(p_RemovePTZConfiguration, "ProfileToken");
    if (p_ProfileToken && p_ProfileToken->data)
    {
    	ONVIF_RET ret = onvif_RemovePTZConfiguration(p_ProfileToken->data);
    	if (ONVIF_OK == ret)
    	{
    		return soap_build_send_rly(p_user, rx_msg, build_RemovePTZConfiguration_rly_xml, NULL, NULL, p_header);
    	}
    	else if (ONVIF_ERR_NoProfile == ret)
    	{
    		return soap_err_def2_rly(p_user, rx_msg, ERR_SENDER, ERR_INVALIDARGVAL, "ter:NoProfile", "Profile Not Exist");
    	}
    }
    else
    {
    	return soap_err_def2_rly(p_user, rx_msg, ERR_SENDER, ERR_MISSINGATTR, NULL, "Missing Attribute");
    }

    return soap_err_def_rly(p_user, rx_msg);
}


int soap_GetConfigurationOptions(HTTPCLN * p_user, HTTPMSG * rx_msg, XMLN * p_body, XMLN * p_header)
{
	XMLN * p_GetConfigurationOptions;
	XMLN * p_ConfigurationToken;
	
	onvif_print("%s\r\n", __FUNCTION__);

	p_GetConfigurationOptions = xml_node_soap_get(p_body, "GetConfigurationOptions");
	assert(p_GetConfigurationOptions);
	
	p_ConfigurationToken = xml_node_soap_get(p_GetConfigurationOptions, "ConfigurationToken");
	if (p_ConfigurationToken && p_ConfigurationToken->data)
	{
		return soap_build_send_rly(p_user, rx_msg, build_GetConfigurationOptions_rly_xml, p_ConfigurationToken->data, NULL, p_header);
	}
	
	return soap_err_def_rly(p_user, rx_msg);
}

int soap_ContinuousMove(HTTPCLN * p_user, HTTPMSG * rx_msg, XMLN * p_body, XMLN * p_header)
{
	ONVIF_RET ret;
	XMLN * p_ContinuousMove;
	ContinuousMove_REQ req;
	
	onvif_print("\n%s\r\n", __FUNCTION__);
	
	p_ContinuousMove = xml_node_soap_get(p_body, "ContinuousMove");
	assert(p_ContinuousMove);
	
	memset(&req, 0, sizeof(ContinuousMove_REQ));

	ret = parse_ContinuousMove(p_ContinuousMove, &req);
	if (ONVIF_OK == ret)
	{
		ret = onvif_ContinuousMove(&req);
		if (ONVIF_OK == ret)
		{
			return soap_build_send_rly(p_user, rx_msg, build_ContinuousMove_rly_xml, NULL, NULL, p_header);
		}
	}
	
	return soap_build_err_rly(p_user, rx_msg, ret);		
}

int soap_AbsoluteMove(HTTPCLN * p_user, HTTPMSG * rx_msg, XMLN * p_body, XMLN * p_header)
{
	ONVIF_RET ret;
	XMLN * p_AbsoluteMove;
	AbsoluteMove_REQ req;
	
    onvif_print("%s\r\n", __FUNCTION__);

    p_AbsoluteMove = xml_node_soap_get(p_body, "AbsoluteMove");
	assert(p_AbsoluteMove);
	
	memset(&req, 0, sizeof(AbsoluteMove_REQ));
	
    ret = parse_AbsoluteMove(p_AbsoluteMove, &req);
	if (ONVIF_OK == ret)
	{
		ret = onvif_AbsoluteMove(&req);
	    if (ONVIF_OK == ret)
		{
			return soap_build_send_rly(p_user, rx_msg, build_AbsoluteMove_rly_xml, NULL, NULL, p_header);
		}
	}

	return soap_build_err_rly(p_user, rx_msg, ret);
}

int soap_RelativeMove(HTTPCLN * p_user, HTTPMSG * rx_msg, XMLN * p_body, XMLN * p_header)
{
	ONVIF_RET ret;
	XMLN * p_RelativeMove;
	RelativeMove_REQ req;
	
    onvif_print("%s\r\n", __FUNCTION__);

    p_RelativeMove = xml_node_soap_get(p_body, "RelativeMove");
	assert(p_RelativeMove);
    
	memset(&req, 0, sizeof(req));

	ret = parse_RelativeMove(p_RelativeMove, &req);    
	if (ONVIF_OK == ret)
	{
		ret = onvif_RelativeMove(&req);		
	    if (ONVIF_OK == ret)
		{
			return soap_build_send_rly(p_user, rx_msg, build_RelativeMove_rly_xml, NULL, NULL, p_header);
		}
	}

	return soap_build_err_rly(p_user, rx_msg, ret);
}

int soap_SetPreset(HTTPCLN * p_user, HTTPMSG * rx_msg, XMLN * p_body, XMLN * p_header)
{
	ONVIF_RET ret;
	XMLN * p_SetPreset;
	SetPreset_REQ req;
	
    onvif_print("%s\r\n", __FUNCTION__);

    p_SetPreset = xml_node_soap_get(p_body, "SetPreset");
	assert(p_SetPreset);
	
	memset(&req, 0, sizeof(SetPreset_REQ));

	ret = parse_SetPreset(p_SetPreset, &req);
	if (ONVIF_OK == ret)
	{
		ret = onvif_SetPreset(&req);
	    if (ONVIF_OK == ret)
		{
			return soap_build_send_rly(p_user, rx_msg, build_SetPreset_rly_xml, req.PresetToken, NULL, p_header);
		}
	}

	return soap_build_err_rly(p_user, rx_msg, ret);
}

int soap_GetPresets(HTTPCLN * p_user, HTTPMSG * rx_msg, XMLN * p_body, XMLN * p_header)
{
	XMLN * p_GetPresets;
	XMLN * p_ProfileToken;
	
    onvif_print("%s\r\n", __FUNCTION__);

    p_GetPresets = xml_node_soap_get(p_body, "GetPresets");
	assert(p_GetPresets);

	p_ProfileToken = xml_node_soap_get(p_GetPresets, "ProfileToken");
	if (p_ProfileToken && p_ProfileToken->data)
	{
		return soap_build_send_rly(p_user, rx_msg, build_GetPresets_rly_xml, p_ProfileToken->data, NULL, p_header);
	}
	else 
	{
		return soap_err_def2_rly(p_user, rx_msg, ERR_SENDER, ERR_MISSINGATTR, NULL, "Missing Attribute");
	}
}

int soap_RemovePreset(HTTPCLN * p_user, HTTPMSG * rx_msg, XMLN * p_body, XMLN * p_header)
{
	ONVIF_RET ret;
	XMLN * p_RemovePreset;
	RemovePreset_REQ req;
	
    onvif_print("%s\r\n", __FUNCTION__);

    p_RemovePreset = xml_node_soap_get(p_body, "RemovePreset");
	assert(p_RemovePreset);
	
	memset(&req, 0, sizeof(req));
	
	ret = parse_RemovePreset(p_RemovePreset, &req);
	if (ONVIF_OK == ret)
	{
		ret = onvif_RemovePreset(&req);
		if (ONVIF_OK == ret)
		{
			return soap_build_send_rly(p_user, rx_msg, build_RemovePreset_rly_xml, NULL, NULL, p_header);
		}
	}
	
	return soap_build_err_rly(p_user, rx_msg, ret);
}

int soap_GotoPreset(HTTPCLN * p_user, HTTPMSG * rx_msg, XMLN * p_body, XMLN * p_header)
{
	ONVIF_RET ret;
	XMLN * p_GotoPreset;
	GotoPreset_REQ req;
	
    onvif_print("%s\r\n", __FUNCTION__);

    p_GotoPreset = xml_node_soap_get(p_body, "GotoPreset");
	assert(p_GotoPreset);
	
	memset(&req, 0, sizeof(req));

	ret = parse_GotoPreset(p_GotoPreset, &req);
	if (ONVIF_OK == ret)
	{
		ret = onvif_GotoPreset(&req);
		if (ONVIF_OK == ret)
		{
			return soap_build_send_rly(p_user, rx_msg, build_GotoPreset_rly_xml, NULL, NULL, p_header);
		}
	}
	
	return soap_build_err_rly(p_user, rx_msg, ret);
}

int soap_GotoHomePosition(HTTPCLN * p_user, HTTPMSG * rx_msg, XMLN * p_body, XMLN * p_header)
{
	ONVIF_RET ret;
	XMLN * p_GotoHomePosition;
	GotoHomePosition_REQ req;
	
    onvif_print("%s\r\n", __FUNCTION__);

    p_GotoHomePosition = xml_node_soap_get(p_body, "GotoHomePosition");
	assert(p_GotoHomePosition);
	
	memset(&req, 0, sizeof(req));

	ret = parse_GotoHomePosition(p_GotoHomePosition, &req);
	if (ONVIF_OK == ret)
	{
		ret = onvif_GotoHomePosition(&req);
	    if (ONVIF_OK == ret)
		{
			return soap_build_send_rly(p_user, rx_msg, build_GotoHomePosition_rly_xml, NULL, NULL, p_header);
		}
	}

	return soap_build_err_rly(p_user, rx_msg, ret);
}

int soap_SetHomePosition(HTTPCLN * p_user, HTTPMSG * rx_msg, XMLN * p_body, XMLN * p_header)
{
	XMLN * p_SetHomePosition;
	XMLN * p_ProfileToken;
	ONVIF_RET ret = ONVIF_ERR_MissingAttribute;
	
	onvif_print("%s\r\n", __FUNCTION__);

    p_SetHomePosition = xml_node_soap_get(p_body, "SetHomePosition");
	assert(p_SetHomePosition);	
	
	p_ProfileToken = xml_node_soap_get(p_SetHomePosition, "ProfileToken");
	if (p_ProfileToken && p_ProfileToken->data)
	{
		ret = onvif_SetHomePosition(p_ProfileToken->data);
		if (ONVIF_OK == ret)
		{
			return soap_build_send_rly(p_user, rx_msg, build_SetHomePosition_rly_xml, NULL, NULL, p_header);
		}
	}

	return soap_build_err_rly(p_user, rx_msg, ret);
}

#endif // PTZ_SUPPORT

#ifdef VIDEO_ANALYTICS

int soap_GetVideoAnalyticsConfigurations(HTTPCLN * p_user, HTTPMSG * rx_msg, XMLN * p_body, XMLN * p_header)
{
	onvif_print("%s\r\n", __FUNCTION__);
	
	return soap_build_send_rly(p_user, rx_msg, build_GetVideoAnalyticsConfigurations_rly_xml, NULL, NULL, p_header);
}

int soap_AddVideoAnalyticsConfiguration(HTTPCLN * p_user, HTTPMSG * rx_msg, XMLN * p_body, XMLN * p_header)
{
	XMLN * p_AddVideoAnalyticsConfiguration;
	AddVideoAnalyticsConfiguration_REQ req;
	ONVIF_RET ret;
	
	onvif_print("%s\r\n", __FUNCTION__);

    p_AddVideoAnalyticsConfiguration = xml_node_soap_get(p_body, "AddVideoAnalyticsConfiguration");
	assert(p_AddVideoAnalyticsConfiguration);	

	memset(&req, 0, sizeof(req));
	
	ret = parse_AddVideoAnalyticsConfiguration(p_AddVideoAnalyticsConfiguration, &req);
	if (ONVIF_OK == ret)
	{
		ret = onvif_AddVideoAnalyticsConfiguration(&req);
		if (ONVIF_OK == ret)
		{
			return soap_build_send_rly(p_user, rx_msg, build_AddVideoAnalyticsConfiguration_rly_xml, NULL, NULL, p_header);
		}	
	}

	return soap_build_err_rly(p_user, rx_msg, ret);
}

int soap_GetVideoAnalyticsConfiguration(HTTPCLN * p_user, HTTPMSG * rx_msg, XMLN * p_body, XMLN * p_header)
{
	XMLN * p_GetVideoAnalyticsConfiguration;
	GetVideoAnalyticsConfiguration_REQ req;
	ONVIF_RET ret;
	
	onvif_print("%s\r\n", __FUNCTION__);

    p_GetVideoAnalyticsConfiguration = xml_node_soap_get(p_body, "GetVideoAnalyticsConfiguration");
	assert(p_GetVideoAnalyticsConfiguration);	

	memset(&req, 0, sizeof(req));
	
	ret = parse_GetVideoAnalyticsConfiguration(p_GetVideoAnalyticsConfiguration, &req);
	if (ONVIF_OK == ret)
	{
		return soap_build_send_rly(p_user, rx_msg, build_GetVideoAnalyticsConfiguration_rly_xml, (char *)&req, NULL, p_header);
	}

	return soap_build_err_rly(p_user, rx_msg, ret);
}

int soap_RemoveVideoAnalyticsConfiguration(HTTPCLN * p_user, HTTPMSG * rx_msg, XMLN * p_body, XMLN * p_header)
{
	XMLN * p_RemoveVideoAnalyticsConfiguration;
	RemoveVideoAnalyticsConfiguration_REQ req;
	ONVIF_RET ret;
	
	onvif_print("%s\r\n", __FUNCTION__);

    p_RemoveVideoAnalyticsConfiguration = xml_node_soap_get(p_body, "RemoveVideoAnalyticsConfiguration");
	assert(p_RemoveVideoAnalyticsConfiguration);	

	memset(&req, 0, sizeof(req));
	
	ret = parse_RemoveVideoAnalyticsConfiguration(p_RemoveVideoAnalyticsConfiguration, &req);
	if (ONVIF_OK == ret)
	{
		ret = onvif_RemoveVideoAnalyticsConfiguration(&req);
		if (ONVIF_OK == ret)
		{
			return soap_build_send_rly(p_user, rx_msg, build_RemoveVideoAnalyticsConfiguration_rly_xml, NULL, NULL, p_header);
		}	
	}

	return soap_build_err_rly(p_user, rx_msg, ret);
}

int soap_SetVideoAnalyticsConfiguration(HTTPCLN * p_user, HTTPMSG * rx_msg, XMLN * p_body, XMLN * p_header)
{
	XMLN * p_SetVideoAnalyticsConfiguration;
	SetVideoAnalyticsConfiguration_REQ req;
	ONVIF_RET ret;
	
	onvif_print("%s\r\n", __FUNCTION__);

    p_SetVideoAnalyticsConfiguration = xml_node_soap_get(p_body, "SetVideoAnalyticsConfiguration");
	assert(p_SetVideoAnalyticsConfiguration);	

	memset(&req, 0, sizeof(req));
	
	ret = parse_SetVideoAnalyticsConfiguration(p_SetVideoAnalyticsConfiguration, &req);
	if (ONVIF_OK == ret)
	{
		ret = onvif_SetVideoAnalyticsConfiguration(&req);
		if (ONVIF_OK == ret)
		{
			return soap_build_send_rly(p_user, rx_msg, build_SetVideoAnalyticsConfiguration_rly_xml, NULL, NULL, p_header);
		}	
	}

	return soap_build_err_rly(p_user, rx_msg, ret);
}

int soap_GetSupportedRules(HTTPCLN * p_user, HTTPMSG * rx_msg, XMLN * p_body, XMLN * p_header)
{
	XMLN * p_GetSupportedRules;
	GetSupportedRules_REQ req;
	ONVIF_RET ret;
	
	onvif_print("%s\r\n", __FUNCTION__);

    p_GetSupportedRules = xml_node_soap_get(p_body, "GetSupportedRules");
	assert(p_GetSupportedRules);	

	memset(&req, 0, sizeof(req));
	
	ret = parse_GetSupportedRules(p_GetSupportedRules, &req);
	if (ONVIF_OK == ret)
	{
		GetSupportedRules_RES res;
		memset(&res, 0, sizeof(res));
		
		ret = onvif_GetSupportedRules(&req, &res);
		if (ONVIF_OK == ret)
		{
			return soap_build_send_rly(p_user, rx_msg, build_GetSupportedRules_rly_xml, (char *)&res, NULL, p_header);
		}	
	}

	return soap_build_err_rly(p_user, rx_msg, ret);
}

int soap_CreateRules(HTTPCLN * p_user, HTTPMSG * rx_msg, XMLN * p_body, XMLN * p_header)
{
	XMLN * p_CreateRules;
	CreateRules_REQ req;
	ONVIF_RET ret;
	
	onvif_print("%s\r\n", __FUNCTION__);

    p_CreateRules = xml_node_soap_get(p_body, "CreateRules");
	assert(p_CreateRules);	

	memset(&req, 0, sizeof(req));
	
	ret = parse_CreateRules(p_CreateRules, &req);
	if (ONVIF_OK == ret)
	{		
		ret = onvif_CreateRules(&req);
		if (ONVIF_OK == ret)
		{			
			return soap_build_send_rly(p_user, rx_msg, build_CreateRules_rly_xml, NULL, NULL, p_header);
		}	
	}

	return soap_build_err_rly(p_user, rx_msg, ret);
}

int soap_DeleteRules(HTTPCLN * p_user, HTTPMSG * rx_msg, XMLN * p_body, XMLN * p_header)
{
	XMLN * p_DeleteRules;
	DeleteRules_REQ req;
	ONVIF_RET ret;
	
	onvif_print("%s\r\n", __FUNCTION__);

    p_DeleteRules = xml_node_soap_get(p_body, "DeleteRules");
	assert(p_DeleteRules);	

	memset(&req, 0, sizeof(req));
	
	ret = parse_DeleteRules(p_DeleteRules, &req);
	if (ONVIF_OK == ret)
	{		
		ret = onvif_DeleteRules(&req);
		if (ONVIF_OK == ret)
		{
			return soap_build_send_rly(p_user, rx_msg, build_DeleteRules_rly_xml, NULL, NULL, p_header);
		}	
	}

	return soap_build_err_rly(p_user, rx_msg, ret);
}

int soap_GetRules(HTTPCLN * p_user, HTTPMSG * rx_msg, XMLN * p_body, XMLN * p_header)
{
	XMLN * p_GetRules;
	GetRules_REQ req;
	ONVIF_RET ret;
	
	onvif_print("%s\r\n", __FUNCTION__);

    p_GetRules = xml_node_soap_get(p_body, "GetRules");
	assert(p_GetRules);	

	memset(&req, 0, sizeof(req));
	
	ret = parse_GetRules(p_GetRules, &req);
	if (ONVIF_OK == ret)
	{
		GetRules_RES res;
		memset(&res, 0, sizeof(res));
		
		ret = onvif_GetRules(&req, &res);
		if (ONVIF_OK == ret)
		{
			return soap_build_send_rly(p_user, rx_msg, build_GetRules_rly_xml, (char *)&res, NULL, p_header);
		}	
	}

	return soap_build_err_rly(p_user, rx_msg, ret);
}

int soap_ModifyRules(HTTPCLN * p_user, HTTPMSG * rx_msg, XMLN * p_body, XMLN * p_header)
{
	XMLN * p_ModifyRules;
	ModifyRules_REQ req;
	ONVIF_RET ret;
	
	onvif_print("%s\r\n", __FUNCTION__);

    p_ModifyRules = xml_node_soap_get(p_body, "ModifyRules");
	assert(p_ModifyRules);	

	memset(&req, 0, sizeof(req));
	
	ret = parse_ModifyRules(p_ModifyRules, &req);
	if (ONVIF_OK == ret)
	{
		ret = onvif_ModifyRules(&req);
		if (ONVIF_OK == ret)
		{
			return soap_build_send_rly(p_user, rx_msg, build_ModifyRules_rly_xml, NULL, NULL, p_header);
		}	
	}
	
	return soap_build_err_rly(p_user, rx_msg, ret);
}

int soap_CreateAnalyticsModules(HTTPCLN * p_user, HTTPMSG * rx_msg, XMLN * p_body, XMLN * p_header)
{
	XMLN * p_CreateAnalyticsModules;
	CreateAnalyticsModules_REQ req;
	ONVIF_RET ret;
	
	onvif_print("%s\r\n", __FUNCTION__);

    p_CreateAnalyticsModules = xml_node_soap_get(p_body, "CreateAnalyticsModules");
	assert(p_CreateAnalyticsModules);	

	memset(&req, 0, sizeof(req));
	
	ret = parse_CreateAnalyticsModules(p_CreateAnalyticsModules, &req);
	if (ONVIF_OK == ret)
	{
		ret = onvif_CreateAnalyticsModules(&req);
		if (ONVIF_OK == ret)
		{
			return soap_build_send_rly(p_user, rx_msg, build_CreateAnalyticsModules_rly_xml, NULL, NULL, p_header);
		}	
	}

	return soap_build_err_rly(p_user, rx_msg, ret);
}

int soap_DeleteAnalyticsModules(HTTPCLN * p_user, HTTPMSG * rx_msg, XMLN * p_body, XMLN * p_header)
{
	XMLN * p_DeleteAnalyticsModules;
	DeleteAnalyticsModules_REQ req;
	ONVIF_RET ret;
	
	onvif_print("%s\r\n", __FUNCTION__);

    p_DeleteAnalyticsModules = xml_node_soap_get(p_body, "DeleteAnalyticsModules");
	assert(p_DeleteAnalyticsModules);	

	memset(&req, 0, sizeof(req));
	
	ret = parse_DeleteAnalyticsModules(p_DeleteAnalyticsModules, &req);
	if (ONVIF_OK == ret)
	{
		ret = onvif_DeleteAnalyticsModules(&req);
		if (ONVIF_OK == ret)
		{
			return soap_build_send_rly(p_user, rx_msg, build_DeleteAnalyticsModules_rly_xml, NULL, NULL, p_header);
		}	
	}

	return soap_build_err_rly(p_user, rx_msg, ret);
}

int soap_GetAnalyticsModules(HTTPCLN * p_user, HTTPMSG * rx_msg, XMLN * p_body, XMLN * p_header)
{
	XMLN * p_GetAnalyticsModules;
	GetAnalyticsModules_REQ req;
	ONVIF_RET ret;
	
	onvif_print("%s\r\n", __FUNCTION__);

    p_GetAnalyticsModules = xml_node_soap_get(p_body, "GetAnalyticsModules");
	assert(p_GetAnalyticsModules);	

	memset(&req, 0, sizeof(req));
	
	ret = parse_GetAnalyticsModules(p_GetAnalyticsModules, &req);
	if (ONVIF_OK == ret)
	{
		GetAnalyticsModules_RES res;
		memset(&res, 0, sizeof(res));
		
		ret = onvif_GetAnalyticsModules(&req, &res);
		if (ONVIF_OK == ret)
		{
			return soap_build_send_rly(p_user, rx_msg, build_GetAnalyticsModules_rly_xml, (char *)&res, NULL, p_header);
		}	
	}

	return soap_build_err_rly(p_user, rx_msg, ret);
}

int soap_ModifyAnalyticsModules(HTTPCLN * p_user, HTTPMSG * rx_msg, XMLN * p_body, XMLN * p_header)
{
	XMLN * p_ModifyAnalyticsModules;
	ModifyAnalyticsModules_REQ req;
	ONVIF_RET ret;
	
	onvif_print("%s\r\n", __FUNCTION__);

    p_ModifyAnalyticsModules = xml_node_soap_get(p_body, "ModifyAnalyticsModules");
	assert(p_ModifyAnalyticsModules);	

	memset(&req, 0, sizeof(req));
	
	ret = parse_ModifyAnalyticsModules(p_ModifyAnalyticsModules, &req);
	if (ONVIF_OK == ret)
	{
		ret = onvif_ModifyAnalyticsModules(&req);
		if (ONVIF_OK == ret)
		{
			return soap_build_send_rly(p_user, rx_msg, build_ModifyAnalyticsModules_rly_xml, NULL, NULL, p_header);
		}	
	}

	return soap_build_err_rly(p_user, rx_msg, ret);
}

int soap_GetAnalyticsConfigurations(HTTPCLN * p_user, HTTPMSG * rx_msg, XMLN * p_body, XMLN * p_header)
{
    onvif_print("%s\r\n", __FUNCTION__);

	return soap_build_send_rly(p_user, rx_msg, build_GetAnalyticsConfigurations_rly_xml, NULL, NULL, p_header);
}

int soap_GetRuleOptions(HTTPCLN * p_user, HTTPMSG * rx_msg, XMLN * p_body, XMLN * p_header)
{
    XMLN * p_GetRuleOptions;
	GetRuleOptions_REQ req;
	ONVIF_RET ret;
	
	onvif_print("%s\r\n", __FUNCTION__);

    p_GetRuleOptions = xml_node_soap_get(p_body, "GetRuleOptions");
	assert(p_GetRuleOptions);	

	memset(&req, 0, sizeof(req));
	
	ret = parse_GetRuleOptions(p_GetRuleOptions, &req);
	if (ONVIF_OK == ret)
	{
		return soap_build_send_rly(p_user, rx_msg, build_GetRuleOptions_rly_xml, (char *)&req, NULL, p_header);
	}

	return soap_build_err_rly(p_user, rx_msg, ret);
}

int soap_GetSupportedAnalyticsModules(HTTPCLN * p_user, HTTPMSG * rx_msg, XMLN * p_body, XMLN * p_header)
{
    XMLN * p_GetSupportedAnalyticsModules;
	GetSupportedAnalyticsModules_REQ req;
	ONVIF_RET ret;
	
	onvif_print("%s\r\n", __FUNCTION__);

    p_GetSupportedAnalyticsModules = xml_node_soap_get(p_body, "GetSupportedAnalyticsModules");
	assert(p_GetSupportedAnalyticsModules);	

	memset(&req, 0, sizeof(req));
	
	ret = parse_GetSupportedAnalyticsModules(p_GetSupportedAnalyticsModules, &req);
	if (ONVIF_OK == ret)
	{
		return soap_build_send_rly(p_user, rx_msg, build_GetSupportedAnalyticsModules_rly_xml, (char *)&req, NULL, p_header);
	}

	return soap_build_err_rly(p_user, rx_msg, ret);
}

int soap_GetAnalyticsModuleOptions(HTTPCLN * p_user, HTTPMSG * rx_msg, XMLN * p_body, XMLN * p_header)
{
    XMLN * p_GetAnalyticsModuleOptions;
	GetAnalyticsModuleOptions_REQ req;
	ONVIF_RET ret;
	
	onvif_print("%s\r\n", __FUNCTION__);

    p_GetAnalyticsModuleOptions = xml_node_soap_get(p_body, "GetAnalyticsModuleOptions");
	assert(p_GetAnalyticsModuleOptions);	

	memset(&req, 0, sizeof(req));
	
	ret = parse_GetAnalyticsModuleOptions(p_GetAnalyticsModuleOptions, &req);
	if (ONVIF_OK == ret)
	{
		return soap_build_send_rly(p_user, rx_msg, build_GetAnalyticsModuleOptions_rly_xml, (char *)&req, NULL, p_header);
	}

	return soap_build_err_rly(p_user, rx_msg, ret);
}


#endif // end of VIDEO_ANALYTICS

#ifdef PROFILE_G_SUPPORT

int soap_GetRecordingSummary(HTTPCLN * p_user, HTTPMSG * rx_msg, XMLN * p_body, XMLN * p_header)
{
	ONVIF_RET ret;
	GetRecordingSummary_RES res;
	
	onvif_print("%s\r\n", __FUNCTION__);

	memset(&res, 0, sizeof(res));
	
	ret = onvif_GetRecordingSummary(&res);
	if (ONVIF_OK == ret)
	{
		return soap_build_send_rly(p_user, rx_msg, build_GetRecordingSummary_rly_xml, (char *)&res, NULL, p_header);
	}

	return soap_build_err_rly(p_user, rx_msg, ret);
}

int soap_GetRecordingInformation(HTTPCLN * p_user, HTTPMSG * rx_msg, XMLN * p_body, XMLN * p_header)
{
	XMLN * p_GetRecordingInformation;
	GetRecordingInformation_REQ req;
	ONVIF_RET ret = ONVIF_ERR_MissingAttribute;
	
	onvif_print("%s\r\n", __FUNCTION__);

	p_GetRecordingInformation = xml_node_soap_get(p_body, "GetRecordingInformation");
	assert(p_GetRecordingInformation);

    memset(&req, 0, sizeof(req));
    
    ret = parse_GetRecordingInformation(p_GetRecordingInformation, &req);
	if (ONVIF_OK == ret)
	{
		GetRecordingInformation_RES res;
		memset(&res, 0, sizeof(res));
		
		ret = onvif_GetRecordingInformation(req.RecordingToken, &res);
		if (ONVIF_OK == ret)
		{
			return soap_build_send_rly(p_user, rx_msg, build_GetRecordingInformation_rly_xml, (char *)&res, NULL, p_header);
		}
	}

	return soap_build_err_rly(p_user, rx_msg, ret);
}

int soap_GetMediaAttributes(HTTPCLN * p_user, HTTPMSG * rx_msg, XMLN * p_body, XMLN * p_header)
{
	XMLN * p_GetMediaAttributes;
	GetMediaAttributes_REQ req;
	ONVIF_RET ret;
	
	onvif_print("%s\r\n", __FUNCTION__);

	p_GetMediaAttributes = xml_node_soap_get(p_body, "GetMediaAttributes");
	assert(p_GetMediaAttributes);

	memset(&req, 0, sizeof(req));
	
	ret = parse_GetMediaAttributes(p_GetMediaAttributes, &req);
	if (ONVIF_OK == ret)
	{
		GetMediaAttributes_RES res;
		memset(&res, 0, sizeof(res));
	
		ret = onvif_GetMediaAttributes(&req, &res);
		if (ONVIF_OK == ret)
		{
			return soap_build_send_rly(p_user, rx_msg, build_GetMediaAttributes_rly_xml, (char *)&res, NULL, p_header);
		}
	}
	
	return soap_build_err_rly(p_user, rx_msg, ret);
}

int soap_FindRecordings(HTTPCLN * p_user, HTTPMSG * rx_msg, XMLN * p_body, XMLN * p_header)
{
	XMLN * p_FindRecordings;
	FindRecordings_REQ req;
	ONVIF_RET ret;
	
	onvif_print("%s\r\n", __FUNCTION__);

	p_FindRecordings = xml_node_soap_get(p_body, "FindRecordings");
	assert(p_FindRecordings);

	memset(&req, 0, sizeof(req));
	
	ret = parse_FindRecordings(p_FindRecordings, &req);
	if (ONVIF_OK == ret)
	{
		FindRecordings_RES res;
		memset(&res, 0, sizeof(res));
	
		ret = onvif_FindRecordings(&req, &res);
		if (ONVIF_OK == ret)
		{
			return soap_build_send_rly(p_user, rx_msg, build_FindRecordings_rly_xml, (char *)&res, NULL, p_header);
		}
	}
	
	return soap_build_err_rly(p_user, rx_msg, ret);
}

int soap_GetRecordingSearchResults(HTTPCLN * p_user, HTTPMSG * rx_msg, XMLN * p_body, XMLN * p_header)
{
	XMLN * p_GetRecordingSearchResults;
	GetRecordingSearchResults_REQ req;
	ONVIF_RET ret;
	
	onvif_print("%s\r\n", __FUNCTION__);

	p_GetRecordingSearchResults = xml_node_soap_get(p_body, "GetRecordingSearchResults");
	assert(p_GetRecordingSearchResults);

	memset(&req, 0, sizeof(req));
	
	ret = parse_GetRecordingSearchResults(p_GetRecordingSearchResults, &req);
	if (ONVIF_OK == ret)
	{
		GetRecordingSearchResults_RES res;
		memset(&res, 0, sizeof(res));
	
		ret = onvif_GetRecordingSearchResults(&req, &res);
		if (ONVIF_OK == ret)
		{
			ret = soap_build_send_rly(p_user, rx_msg, build_GetRecordingSearchResults_rly_xml, (char *)&res, NULL, p_header);

			onvif_free_RecordingInformations(&res.ResultList.RecordInformation);

			return ret;
		}
	}
	
	return soap_build_err_rly(p_user, rx_msg, ret);
}

int soap_FindEvents(HTTPCLN * p_user, HTTPMSG * rx_msg, XMLN * p_body, XMLN * p_header)
{
	XMLN * p_FindEvents;
	FindEvents_REQ req;
	ONVIF_RET ret;
	
	onvif_print("%s\r\n", __FUNCTION__);

	p_FindEvents = xml_node_soap_get(p_body, "FindEvents");
	assert(p_FindEvents);

	memset(&req, 0, sizeof(req));
	
	ret = parse_FindEvents(p_FindEvents, &req);
	if (ONVIF_OK == ret)
	{
		FindEvents_RES res;
		memset(&res, 0, sizeof(res));
	
		ret = onvif_FindEvents(&req, &res);
		if (ONVIF_OK == ret)
		{
			return soap_build_send_rly(p_user, rx_msg, build_FindEvents_rly_xml, (char *)&res, NULL, p_header);
		}
	}
	
	return soap_build_err_rly(p_user, rx_msg, ret);
}

int soap_GetEventSearchResults(HTTPCLN * p_user, HTTPMSG * rx_msg, XMLN * p_body, XMLN * p_header)
{
	XMLN * p_GetEventSearchResults;
	GetEventSearchResults_REQ req;
	ONVIF_RET ret;
	
	onvif_print("%s\r\n", __FUNCTION__);

	p_GetEventSearchResults = xml_node_soap_get(p_body, "GetEventSearchResults");
	assert(p_GetEventSearchResults);

	memset(&req, 0, sizeof(req));
	
	ret = parse_GetEventSearchResults(p_GetEventSearchResults, &req);
	if (ONVIF_OK == ret)
	{
		GetEventSearchResults_RES res;
		memset(&res, 0, sizeof(res));
	
		ret = onvif_GetEventSearchResults(&req, &res);
		if (ONVIF_OK == ret)
		{
			ret = soap_build_send_rly(p_user, rx_msg, build_GetEventSearchResults_rly_xml, (char *)&res, NULL, p_header);

			onvif_free_FindEventResults(&res.ResultList.Result);

			return ret;
		}
	}
	
	return soap_build_err_rly(p_user, rx_msg, ret);
}

int soap_FindMetadata(HTTPCLN * p_user, HTTPMSG * rx_msg, XMLN * p_body, XMLN * p_header)
{
    XMLN * p_FindMetadata;
	FindMetadata_REQ req;
	ONVIF_RET ret;
	
	onvif_print("%s\r\n", __FUNCTION__);

	p_FindMetadata = xml_node_soap_get(p_body, "FindMetadata");
	assert(p_FindMetadata);

	memset(&req, 0, sizeof(req));
	
	ret = parse_FindMetadata(p_FindMetadata, &req);
	if (ONVIF_OK == ret)
	{
		FindMetadata_RES res;
		memset(&res, 0, sizeof(res));
	
		ret = onvif_FindMetadata(&req, &res);
		if (ONVIF_OK == ret)
		{
			return soap_build_send_rly(p_user, rx_msg, build_FindMetadata_rly_xml, (char *)&res, NULL, p_header);
		}
	}
	
	return soap_build_err_rly(p_user, rx_msg, ret);
}

int soap_GetMetadataSearchResults(HTTPCLN * p_user, HTTPMSG * rx_msg, XMLN * p_body, XMLN * p_header)
{
    XMLN * p_GetMetadataSearchResults;
	GetMetadataSearchResults_REQ req;
	ONVIF_RET ret;
	
	onvif_print("%s\r\n", __FUNCTION__);

	p_GetMetadataSearchResults = xml_node_soap_get(p_body, "GetMetadataSearchResults");
	assert(p_GetMetadataSearchResults);

	memset(&req, 0, sizeof(req));
	
	ret = parse_GetMetadataSearchResults(p_GetMetadataSearchResults, &req);
	if (ONVIF_OK == ret)
	{
		GetMetadataSearchResults_RES res;
		memset(&res, 0, sizeof(res));
	
		ret = onvif_GetMetadataSearchResults(&req, &res);
		if (ONVIF_OK == ret)
		{
			ret = soap_build_send_rly(p_user, rx_msg, build_GetMetadataSearchResults_rly_xml, (char *)&res, NULL, p_header);

			onvif_free_FindMetadataResults(&res.ResultList.Result);

			return ret;
		}
	}
	
	return soap_build_err_rly(p_user, rx_msg, ret);
}

#ifdef PTZ_SUPPORT

int soap_FindPTZPosition(HTTPCLN * p_user, HTTPMSG * rx_msg, XMLN * p_body, XMLN * p_header)
{
    XMLN * p_FindPTZPosition;
	FindPTZPosition_REQ req;
	ONVIF_RET ret;
	
	onvif_print("%s\r\n", __FUNCTION__);

	p_FindPTZPosition = xml_node_soap_get(p_body, "FindPTZPosition");
	assert(p_FindPTZPosition);

	memset(&req, 0, sizeof(req));
	
	ret = parse_FindPTZPosition(p_FindPTZPosition, &req);
	if (ONVIF_OK == ret)
	{
		FindPTZPosition_RES res;
		memset(&res, 0, sizeof(res));
	
		ret = onvif_FindPTZPosition(&req, &res);
		if (ONVIF_OK == ret)
		{
			return soap_build_send_rly(p_user, rx_msg, build_FindPTZPosition_rly_xml, (char *)&res, NULL, p_header);
		}
	}
	
	return soap_build_err_rly(p_user, rx_msg, ret);
}

int soap_GetPTZPositionSearchResults(HTTPCLN * p_user, HTTPMSG * rx_msg, XMLN * p_body, XMLN * p_header)
{
    XMLN * p_GetPTZPositionSearchResults;
	GetPTZPositionSearchResults_REQ req;
	ONVIF_RET ret;
	
	onvif_print("%s\r\n", __FUNCTION__);

	p_GetPTZPositionSearchResults = xml_node_soap_get(p_body, "GetPTZPositionSearchResults");
	assert(p_GetPTZPositionSearchResults);

	memset(&req, 0, sizeof(req));
	
	ret = parse_GetPTZPositionSearchResults(p_GetPTZPositionSearchResults, &req);
	if (ONVIF_OK == ret)
	{
		GetPTZPositionSearchResults_RES res;
		memset(&res, 0, sizeof(res));
	
		ret = onvif_GetPTZPositionSearchResults(&req, &res);
		if (ONVIF_OK == ret)
		{
			ret = soap_build_send_rly(p_user, rx_msg, build_GetPTZPositionSearchResults_rly_xml, (char *)&res, NULL, p_header);

			onvif_free_FindPTZPositionResult(&res.ResultList.Result);

			return ret;
		}
	}
	
	return soap_build_err_rly(p_user, rx_msg, ret);
}

#endif

int soap_EndSearch(HTTPCLN * p_user, HTTPMSG * rx_msg, XMLN * p_body, XMLN * p_header)
{
	XMLN * p_EndSearch;
	EndSearch_REQ req;
	ONVIF_RET ret;
	
	onvif_print("%s\r\n", __FUNCTION__);

	p_EndSearch = xml_node_soap_get(p_body, "EndSearch");
	assert(p_EndSearch);

	memset(&req, 0, sizeof(req));
	
	ret = parse_EndSearch(p_EndSearch, &req);
	if (ONVIF_OK == ret)
	{
		EndSearch_RES res;
		memset(&res, 0, sizeof(res));
	
		ret = onvif_EndSearch(&req, &res);
		if (ONVIF_OK == ret)
		{
			return soap_build_send_rly(p_user, rx_msg, build_EndSearch_rly_xml, (char *)&res, NULL, p_header);
		}
	}
	
	return soap_build_err_rly(p_user, rx_msg, ret);
}

int soap_GetSearchState(HTTPCLN * p_user, HTTPMSG * rx_msg, XMLN * p_body, XMLN * p_header)
{
	XMLN * p_GetSearchState;
	GetSearchState_REQ req;
	ONVIF_RET ret;
	
	onvif_print("%s\r\n", __FUNCTION__);

	p_GetSearchState = xml_node_soap_get(p_body, "GetSearchState");
	assert(p_GetSearchState);

	memset(&req, 0, sizeof(req));
	
	ret = parse_GetSearchState(p_GetSearchState, &req);
	if (ONVIF_OK == ret)
	{
		GetSearchState_RES res;
		memset(&res, 0, sizeof(res));
	
		ret = onvif_GetSearchState(&req, &res);
		if (ONVIF_OK == ret)
		{
			return soap_build_send_rly(p_user, rx_msg, build_GetSearchState_rly_xml, (char *)&res, NULL, p_header);
		}
	}
	
	return soap_build_err_rly(p_user, rx_msg, ret);
}

int soap_CreateRecording(HTTPCLN * p_user, HTTPMSG * rx_msg, XMLN * p_body, XMLN * p_header)
{
	ONVIF_RET ret;
	XMLN * p_CreateRecording;
	CreateRecording_REQ req;
	
	onvif_print("%s\r\n", __FUNCTION__);

	p_CreateRecording = xml_node_soap_get(p_body, "CreateRecording");
	assert(p_CreateRecording);

	memset(&req, 0, sizeof(req));
	
	ret = parse_CreateRecording(p_CreateRecording, &req);
	if (ONVIF_OK == ret)
	{
		ret = onvif_CreateRecording(&req);
		if (ONVIF_OK == ret)
		{
			return soap_build_send_rly(p_user, rx_msg, build_CreateRecording_rly_xml, req.RecordingToken, NULL, p_header);
		}
	}
	
	return soap_build_err_rly(p_user, rx_msg, ret);
}

int soap_DeleteRecording(HTTPCLN * p_user, HTTPMSG * rx_msg, XMLN * p_body, XMLN * p_header)
{
	XMLN * p_DeleteRecording;
	XMLN * p_RecordingToken;
	ONVIF_RET ret = ONVIF_ERR_NoRecording;
	
	onvif_print("%s\r\n", __FUNCTION__);

	p_DeleteRecording = xml_node_soap_get(p_body, "DeleteRecording");
	assert(p_DeleteRecording);

	p_RecordingToken = xml_node_soap_get(p_DeleteRecording, "RecordingToken");
	if (p_RecordingToken && p_RecordingToken->data)
	{
		ret = onvif_DeleteRecording(p_RecordingToken->data);
		if (ONVIF_OK == ret)
		{
			return soap_build_send_rly(p_user, rx_msg, build_DeleteRecording_rly_xml, NULL, NULL, p_header);
		}
	}

	return soap_build_err_rly(p_user, rx_msg, ret);
}

int soap_GetRecordings(HTTPCLN * p_user, HTTPMSG * rx_msg, XMLN * p_body, XMLN * p_header)
{
	onvif_print("%s\r\n", __FUNCTION__);

	return soap_build_send_rly(p_user, rx_msg, build_GetRecordings_rly_xml, NULL, NULL, p_header);
}

int soap_SetRecordingConfiguration(HTTPCLN * p_user, HTTPMSG * rx_msg, XMLN * p_body, XMLN * p_header)
{
	ONVIF_RET ret;
	XMLN * p_SetRecordingConfiguration;
	SetRecordingConfiguration_REQ req;
	
	onvif_print("%s\r\n", __FUNCTION__);

	p_SetRecordingConfiguration = xml_node_soap_get(p_body, "SetRecordingConfiguration");
	assert(p_SetRecordingConfiguration);

	memset(&req, 0, sizeof(req));
	
	ret = parse_SetRecordingConfiguration(p_SetRecordingConfiguration, &req);
	if (ONVIF_OK == ret)
	{
		ret = onvif_SetRecordingConfiguration(&req);
		if (ONVIF_OK == ret)
		{
			return soap_build_send_rly(p_user, rx_msg, build_SetRecordingConfiguration_rly_xml, NULL, NULL, p_header);
		}
	}
	
	return soap_build_err_rly(p_user, rx_msg, ret);
}

int soap_GetRecordingConfiguration(HTTPCLN * p_user, HTTPMSG * rx_msg, XMLN * p_body, XMLN * p_header)
{
	XMLN * p_GetRecordingConfiguration;
	XMLN * p_RecordingToken;
	ONVIF_RET ret = ONVIF_ERR_NoRecording;
	
	onvif_print("%s\r\n", __FUNCTION__);

	p_GetRecordingConfiguration = xml_node_soap_get(p_body, "GetRecordingConfiguration");
	assert(p_GetRecordingConfiguration);

	p_RecordingToken = xml_node_soap_get(p_GetRecordingConfiguration, "RecordingToken");
	if (p_RecordingToken)
	{
	    if (p_RecordingToken->data)
	    {
		    return soap_build_send_rly(p_user, rx_msg, build_GetRecordingConfiguration_rly_xml, p_RecordingToken->data, NULL, p_header);
		}
		else
		{
		    return soap_build_send_rly(p_user, rx_msg, build_GetRecordingConfiguration_rly_xml, "", NULL, p_header);
		}
	}

	return soap_build_err_rly(p_user, rx_msg, ret);
}

int soap_CreateTrack(HTTPCLN * p_user, HTTPMSG * rx_msg, XMLN * p_body, XMLN * p_header)
{
	ONVIF_RET ret;
	XMLN * p_CreateTrack;
	CreateTrack_REQ req;
	
	onvif_print("%s\r\n", __FUNCTION__);

	p_CreateTrack = xml_node_soap_get(p_body, "CreateTrack");
	assert(p_CreateTrack);

	memset(&req, 0, sizeof(req));
	
	ret = parse_CreateTrack(p_CreateTrack, &req);
	if (ONVIF_OK == ret)
	{
		ret = onvif_CreateTrack(&req);
		if (ONVIF_OK == ret)
		{
			return soap_build_send_rly(p_user, rx_msg, build_CreateTrack_rly_xml, req.TrackToken, NULL, p_header);
		}
	}
	
	return soap_build_err_rly(p_user, rx_msg, ret);
}

int soap_DeleteTrack(HTTPCLN * p_user, HTTPMSG * rx_msg, XMLN * p_body, XMLN * p_header)
{
	ONVIF_RET ret;
	XMLN * p_DeleteTrack;
	DeleteTrack_REQ req;
	
	onvif_print("%s\r\n", __FUNCTION__);

	p_DeleteTrack = xml_node_soap_get(p_body, "DeleteTrack");
	assert(p_DeleteTrack);

	memset(&req, 0, sizeof(req));
	
	ret = parse_DeleteTrack(p_DeleteTrack, &req);
	if (ONVIF_OK == ret)
	{
		ret = onvif_DeleteTrack(&req);
		if (ONVIF_OK == ret)
		{
			return soap_build_send_rly(p_user, rx_msg, build_DeleteTrack_rly_xml, req.TrackToken, NULL, p_header);
		}
	}
	
	return soap_build_err_rly(p_user, rx_msg, ret);
}

int soap_GetTrackConfiguration(HTTPCLN * p_user, HTTPMSG * rx_msg, XMLN * p_body, XMLN * p_header)
{
	ONVIF_RET ret;
	XMLN * p_GetTrackConfiguration;
	GetTrackConfiguration_REQ req;
	
	onvif_print("%s\r\n", __FUNCTION__);

	p_GetTrackConfiguration = xml_node_soap_get(p_body, "GetTrackConfiguration");
	assert(p_GetTrackConfiguration);

	memset(&req, 0, sizeof(req));
	
	ret = parse_GetTrackConfiguration(p_GetTrackConfiguration, &req);
	if (ONVIF_OK == ret)
	{
		return soap_build_send_rly(p_user, rx_msg, build_GetTrackConfiguration_rly_xml, (char *)&req, NULL, p_header);
	}
	
	return soap_build_err_rly(p_user, rx_msg, ret);
}

int soap_SetTrackConfiguration(HTTPCLN * p_user, HTTPMSG * rx_msg, XMLN * p_body, XMLN * p_header)
{
	ONVIF_RET ret;
	XMLN * p_SetTrackConfiguration;
	SetTrackConfiguration_REQ req;
	
	onvif_print("%s\r\n", __FUNCTION__);

	p_SetTrackConfiguration = xml_node_soap_get(p_body, "SetTrackConfiguration");
	assert(p_SetTrackConfiguration);

	memset(&req, 0, sizeof(req));
	
	ret = parse_SetTrackConfiguration(p_SetTrackConfiguration, &req);
	if (ONVIF_OK == ret)
	{
		ret = onvif_SetTrackConfiguration(&req);
		if (ONVIF_OK == ret)
		{
			return soap_build_send_rly(p_user, rx_msg, build_SetTrackConfiguration_rly_xml, NULL, NULL, p_header);
		}	
	}
	
	return soap_build_err_rly(p_user, rx_msg, ret);
}

int soap_CreateRecordingJob(HTTPCLN * p_user, HTTPMSG * rx_msg, XMLN * p_body, XMLN * p_header)
{
	ONVIF_RET ret;
	XMLN * p_CreateRecordingJob;
	CreateRecordingJob_REQ req;
	
	onvif_print("%s\r\n", __FUNCTION__);

	p_CreateRecordingJob = xml_node_soap_get(p_body, "CreateRecordingJob");
	assert(p_CreateRecordingJob);

	memset(&req, 0, sizeof(req));
	
	ret = parse_CreateRecordingJob(p_CreateRecordingJob, &req);
	if (ONVIF_OK == ret)
	{
		ret = onvif_CreateRecordingJob(&req);
		if (ONVIF_OK == ret)
		{
			return soap_build_send_rly(p_user, rx_msg, build_CreateRecordingJob_rly_xml, (char *)&req, NULL, p_header);
		}	
	}
	
	return soap_build_err_rly(p_user, rx_msg, ret);
}

int soap_DeleteRecordingJob(HTTPCLN * p_user, HTTPMSG * rx_msg, XMLN * p_body, XMLN * p_header)
{
	XMLN * p_DeleteRecordingJob;
	XMLN * p_JobToken;
	ONVIF_RET ret = ONVIF_ERR_NoRecordingJob;
	
	onvif_print("%s\r\n", __FUNCTION__);

	p_DeleteRecordingJob = xml_node_soap_get(p_body, "DeleteRecordingJob");
	assert(p_DeleteRecordingJob);

	p_JobToken = xml_node_soap_get(p_DeleteRecordingJob, "JobToken");
	if (p_JobToken && p_JobToken->data)
	{	
		ret = onvif_DeleteRecordingJob(p_JobToken->data);
		if (ONVIF_OK == ret)
		{
			return soap_build_send_rly(p_user, rx_msg, build_DeleteRecordingJob_rly_xml, NULL, NULL, p_header);
		}
	}

	return soap_build_err_rly(p_user, rx_msg, ret);
}

int soap_GetRecordingJobs(HTTPCLN * p_user, HTTPMSG * rx_msg, XMLN * p_body, XMLN * p_header)
{
	onvif_print("%s\r\n", __FUNCTION__);

	return soap_build_send_rly(p_user, rx_msg, build_GetRecordingJobs_rly_xml, NULL, NULL, p_header);
}

int soap_SetRecordingJobConfiguration(HTTPCLN * p_user, HTTPMSG * rx_msg, XMLN * p_body, XMLN * p_header)
{
	ONVIF_RET ret;
	XMLN * p_SetRecordingJobConfiguration;
	SetRecordingJobConfiguration_REQ req;
	
	onvif_print("%s\r\n", __FUNCTION__);

	p_SetRecordingJobConfiguration = xml_node_soap_get(p_body, "SetRecordingJobConfiguration");
	assert(p_SetRecordingJobConfiguration);

	memset(&req, 0, sizeof(req));
	
	ret = parse_SetRecordingJobConfiguration(p_SetRecordingJobConfiguration, &req);
	if (ONVIF_OK == ret)
	{
		ret = onvif_SetRecordingJobConfiguration(&req);
		if (ONVIF_OK == ret)
		{
			return soap_build_send_rly(p_user, rx_msg, build_SetRecordingJobConfiguration_rly_xml, (char *)&req, NULL, p_header);
		}	
	}
	
	return soap_build_err_rly(p_user, rx_msg, ret);
}

int soap_GetRecordingJobConfiguration(HTTPCLN * p_user, HTTPMSG * rx_msg, XMLN * p_body, XMLN * p_header)
{
	XMLN * p_GetRecordingJobConfiguration;
	XMLN * p_JobToken;
	ONVIF_RET ret = ONVIF_ERR_NoRecordingJob;
	
	onvif_print("%s\r\n", __FUNCTION__);

	p_GetRecordingJobConfiguration = xml_node_soap_get(p_body, "GetRecordingJobConfiguration");
	assert(p_GetRecordingJobConfiguration);

	p_JobToken = xml_node_soap_get(p_GetRecordingJobConfiguration, "JobToken");
	if (p_JobToken && p_JobToken->data)
	{
		return soap_build_send_rly(p_user, rx_msg, build_GetRecordingJobConfiguration_rly_xml, p_JobToken->data, NULL, p_header);
	}

	return soap_build_err_rly(p_user, rx_msg, ret);
}

int soap_SetRecordingJobMode(HTTPCLN * p_user, HTTPMSG * rx_msg, XMLN * p_body, XMLN * p_header)
{
	ONVIF_RET ret;
	XMLN * p_SetRecordingJobMode;
	SetRecordingJobMode_REQ req;
	
	onvif_print("%s\r\n", __FUNCTION__);

	p_SetRecordingJobMode = xml_node_soap_get(p_body, "SetRecordingJobMode");
	assert(p_SetRecordingJobMode);

	memset(&req, 0, sizeof(req));
	
	ret = parse_SetRecordingJobMode(p_SetRecordingJobMode, &req);
	if (ONVIF_OK == ret)
	{
		ret = onvif_SetRecordingJobMode(&req);
		if (ONVIF_OK == ret)
		{
			return soap_build_send_rly(p_user, rx_msg, build_SetRecordingJobMode_rly_xml, NULL, NULL, p_header);
		}	
	}
	
	return soap_build_err_rly(p_user, rx_msg, ret);
}

int soap_GetRecordingJobState(HTTPCLN * p_user, HTTPMSG * rx_msg, XMLN * p_body, XMLN * p_header)
{
	XMLN * p_GetRecordingJobState;
	XMLN * p_JobToken;
	ONVIF_RET ret = ONVIF_ERR_NoRecordingJob;
	
	onvif_print("%s\r\n", __FUNCTION__);

	p_GetRecordingJobState = xml_node_soap_get(p_body, "GetRecordingJobState");
	assert(p_GetRecordingJobState);

	p_JobToken = xml_node_soap_get(p_GetRecordingJobState, "JobToken");
	if (p_JobToken && p_JobToken->data)
	{
		onvif_RecordingJobStateInformation state;
		memset(&state, 0, sizeof(state));
		
		ret = onvif_GetRecordingJobState(p_JobToken->data, &state);
		if (ONVIF_OK == ret)
		{
			return soap_build_send_rly(p_user, rx_msg, build_GetRecordingJobState_rly_xml, (char*)&state, NULL, p_header);
		}
	}

	return soap_build_err_rly(p_user, rx_msg, ret);
}

int soap_GetRecordingOptions(HTTPCLN * p_user, HTTPMSG * rx_msg, XMLN * p_body, XMLN * p_header)
{
	XMLN * p_GetRecordingOptions;
	XMLN * p_RecordingToken;
	ONVIF_RET ret = ONVIF_ERR_NoRecording;
	
	onvif_print("%s\r\n", __FUNCTION__);

	p_GetRecordingOptions = xml_node_soap_get(p_body, "GetRecordingOptions");
	assert(p_GetRecordingOptions);

	p_RecordingToken = xml_node_soap_get(p_GetRecordingOptions, "RecordingToken");
	if (p_RecordingToken)
	{
		onvif_RecordingOptions options;
		memset(&options, 0, sizeof(options));

		if (p_RecordingToken->data)
		{
		    ret = onvif_GetRecordingOptions(p_RecordingToken->data, &options);
		}
		else
		{
		    ret = onvif_GetRecordingOptions("", &options);
		}
		
		if (ONVIF_OK == ret)
		{
			return soap_build_send_rly(p_user, rx_msg, build_GetRecordingOptions_rly_xml, (char *)&options, NULL, p_header);
		}
	}

	return soap_build_err_rly(p_user, rx_msg, ret);
}

int soap_GetReplayUri(HTTPCLN * p_user, HTTPMSG * rx_msg, XMLN * p_body, XMLN * p_header)
{
	XMLN * p_GetReplayUri;
	GetReplayUri_REQ req;
	ONVIF_RET ret;
	
	onvif_print("%s\r\n", __FUNCTION__);

	p_GetReplayUri = xml_node_soap_get(p_body, "GetReplayUri");
	assert(p_GetReplayUri);

	memset(&req, 0, sizeof(req));
	
	ret = parse_GetReplayUri(p_GetReplayUri, &req);
	if (ONVIF_OK == ret)
	{
		GetReplayUri_RES res;
		memset(&res, 0, sizeof(res));
	
		ret = onvif_GetReplayUri(p_user->lip, p_user->lport, &req, &res);
		if (ONVIF_OK == ret)
		{
			return soap_build_send_rly(p_user, rx_msg, build_GetReplayUri_rly_xml, (char *)&res, NULL, p_header);
		}
	}
	
	return soap_build_err_rly(p_user, rx_msg, ret);
}

int soap_GetReplayConfiguration(HTTPCLN * p_user, HTTPMSG * rx_msg, XMLN * p_body, XMLN * p_header)
{
	ONVIF_RET ret;	
	GetReplayConfiguration_RES res;

	onvif_print("%s\r\n", __FUNCTION__);
	
	memset(&res, 0, sizeof(res));

	ret = onvif_GetReplayConfiguration(&res);
	if (ONVIF_OK == ret)
	{
		return soap_build_send_rly(p_user, rx_msg, build_GetReplayConfiguration_rly_xml, (char *)&res, NULL, p_header);
	}

	return soap_build_err_rly(p_user, rx_msg, ret);
}

int soap_SetReplayConfiguration(HTTPCLN * p_user, HTTPMSG * rx_msg, XMLN * p_body, XMLN * p_header)
{
	XMLN * p_SetReplayConfiguration;
	SetReplayConfiguration_REQ req;
	ONVIF_RET ret;
	
	onvif_print("%s\r\n", __FUNCTION__);

	p_SetReplayConfiguration = xml_node_soap_get(p_body, "SetReplayConfiguration");
	assert(p_SetReplayConfiguration);

	memset(&req, 0, sizeof(req));
	
	ret = parse_SetReplayConfiguration(p_SetReplayConfiguration, &req);
	if (ONVIF_OK == ret)
	{	
		ret = onvif_SetReplayConfiguration(&req);
		if (ONVIF_OK == ret)
		{
			return soap_build_send_rly(p_user, rx_msg, build_SetReplayConfiguration_rly_xml, NULL, NULL, p_header);
		}
	}
	
	return soap_build_err_rly(p_user, rx_msg, ret);
}

#endif	// end of PROFILE_G_SUPPORT

#ifdef PROFILE_C_SUPPORT

int soap_tac_GetAccessPointInfoList(HTTPCLN * p_user, HTTPMSG * rx_msg, XMLN * p_body, XMLN * p_header)
{
    XMLN * p_GetAccessPointInfoList;
	tac_GetAccessPointInfoList_REQ req;
	ONVIF_RET ret;
	
	onvif_print("%s\r\n", __FUNCTION__);

	p_GetAccessPointInfoList = xml_node_soap_get(p_body, "GetAccessPointInfoList");
	assert(p_GetAccessPointInfoList);

	memset(&req, 0, sizeof(req));
	
	ret = parse_tac_GetAccessPointInfoList(p_GetAccessPointInfoList, &req);
	if (ONVIF_OK == ret)
	{	
	    tac_GetAccessPointInfoList_RES res;
	    memset(&res, 0, sizeof(res));
	    
		ret = onvif_tac_GetAccessPointInfoList(&req, &res);
		if (ONVIF_OK == ret)
		{
			ret = soap_build_send_rly(p_user, rx_msg, build_tac_GetAccessPointInfoList_rly_xml, (char *)&res, NULL, p_header);
			onvif_free_AccessPoints(&res.AccessPointInfo);
			return ret;
		}
	}
	
	return soap_build_err_rly(p_user, rx_msg, ret);
}

int soap_tac_GetAccessPointInfo(HTTPCLN * p_user, HTTPMSG * rx_msg, XMLN * p_body, XMLN * p_header)
{
    XMLN * p_GetAccessPointInfo;
	tac_GetAccessPointInfo_REQ req;
	ONVIF_RET ret;
	
	onvif_print("%s\r\n", __FUNCTION__);

	p_GetAccessPointInfo = xml_node_soap_get(p_body, "GetAccessPointInfo");
	assert(p_GetAccessPointInfo);

	memset(&req, 0, sizeof(req));
	
	ret = parse_tac_GetAccessPointInfo(p_GetAccessPointInfo, &req);
	if (ONVIF_OK == ret)
	{
		return soap_build_send_rly(p_user, rx_msg, build_tac_GetAccessPointInfo_rly_xml, (char *)&req, NULL, p_header);
	}
	
	return soap_build_err_rly(p_user, rx_msg, ret);
}

int soap_tac_GetAreaInfoList(HTTPCLN * p_user, HTTPMSG * rx_msg, XMLN * p_body, XMLN * p_header)
{
    XMLN * p_GetAreaInfoList;
	tac_GetAreaInfoList_REQ req;
	ONVIF_RET ret;
	
	onvif_print("%s\r\n", __FUNCTION__);

	p_GetAreaInfoList = xml_node_soap_get(p_body, "GetAreaInfoList");
	assert(p_GetAreaInfoList);

	memset(&req, 0, sizeof(req));
	
	ret = parse_tac_GetAreaInfoList(p_GetAreaInfoList, &req);
	if (ONVIF_OK == ret)
	{	
	    tac_GetAreaInfoList_RES res;
	    memset(&res, 0, sizeof(res));
	    
		ret = onvif_tac_GetAreaInfoList(&req, &res);
		if (ONVIF_OK == ret)
		{
			ret = soap_build_send_rly(p_user, rx_msg, build_tac_GetAreaInfoList_rly_xml, (char *)&res, NULL, p_header);
			onvif_free_AreaInfos(&res.AreaInfo);
			return ret;
		}
	}
	
	return soap_build_err_rly(p_user, rx_msg, ret);
}

int soap_tac_GetAreaInfo(HTTPCLN * p_user, HTTPMSG * rx_msg, XMLN * p_body, XMLN * p_header)
{
    XMLN * p_GetAreaInfo;
	tac_GetAreaInfo_REQ req;
	ONVIF_RET ret;
	
	onvif_print("%s\r\n", __FUNCTION__);

	p_GetAreaInfo = xml_node_soap_get(p_body, "GetAreaInfo");
	assert(p_GetAreaInfo);

	memset(&req, 0, sizeof(req));
	
	ret = parse_tac_GetAreaInfo(p_GetAreaInfo, &req);
	if (ONVIF_OK == ret)
	{
		return soap_build_send_rly(p_user, rx_msg, build_tac_GetAreaInfo_rly_xml, (char *)&req, NULL, p_header);
	}
	
	return soap_build_err_rly(p_user, rx_msg, ret);
}

int soap_tac_GetAccessPointState(HTTPCLN * p_user, HTTPMSG * rx_msg, XMLN * p_body, XMLN * p_header)
{
    XMLN * p_GetAccessPointState;
	tac_GetAccessPointState_REQ req;
	ONVIF_RET ret;
	
	onvif_print("%s\r\n", __FUNCTION__);

	p_GetAccessPointState = xml_node_soap_get(p_body, "GetAccessPointState");
	assert(p_GetAccessPointState);

	memset(&req, 0, sizeof(req));
	
	ret = parse_tac_GetAccessPointState(p_GetAccessPointState, &req);
	if (ONVIF_OK == ret)
	{
		return soap_build_send_rly(p_user, rx_msg, build_tac_GetAccessPointState_rly_xml, (char *)&req, NULL, p_header);
	}
	
	return soap_build_err_rly(p_user, rx_msg, ret);
}

int soap_tac_EnableAccessPoint(HTTPCLN * p_user, HTTPMSG * rx_msg, XMLN * p_body, XMLN * p_header)
{
    XMLN * p_EnableAccessPoint;
	tac_EnableAccessPoint_REQ req;
	ONVIF_RET ret;
	
	onvif_print("%s\r\n", __FUNCTION__);

	p_EnableAccessPoint = xml_node_soap_get(p_body, "EnableAccessPoint");
	assert(p_EnableAccessPoint);

	memset(&req, 0, sizeof(req));
	
	ret = parse_tac_EnableAccessPoint(p_EnableAccessPoint, &req);
	if (ONVIF_OK == ret)
	{
	    ret = onvif_tac_EnableAccessPoint(&req);
	    if (ONVIF_OK == ret)
	    {
		    return soap_build_send_rly(p_user, rx_msg, build_tac_EnableAccessPoint_rly_xml, NULL, NULL, p_header);
		}
	}
	
	return soap_build_err_rly(p_user, rx_msg, ret);
}

int soap_tac_DisableAccessPoint(HTTPCLN * p_user, HTTPMSG * rx_msg, XMLN * p_body, XMLN * p_header)
{
    XMLN * p_DisableAccessPoint;
	tac_DisableAccessPoint_REQ req;
	ONVIF_RET ret;
	
	onvif_print("%s\r\n", __FUNCTION__);

	p_DisableAccessPoint = xml_node_soap_get(p_body, "DisableAccessPoint");
	assert(p_DisableAccessPoint);

	memset(&req, 0, sizeof(req));
	
	ret = parse_tac_DisableAccessPoint(p_DisableAccessPoint, &req);
	if (ONVIF_OK == ret)
	{
	    ret = onvif_tac_DisableAccessPoint(&req);
	    if (ONVIF_OK == ret)
	    {
		    return soap_build_send_rly(p_user, rx_msg, build_tac_DisableAccessPoint_rly_xml, NULL, NULL, p_header);
		}    
	}
	
	return soap_build_err_rly(p_user, rx_msg, ret);
}

int soap_tdc_GetDoorInfoList(HTTPCLN * p_user, HTTPMSG * rx_msg, XMLN * p_body, XMLN * p_header)
{
    XMLN * p_GetDoorInfoList;
	tdc_GetDoorInfoList_REQ req;
	ONVIF_RET ret;
	
	onvif_print("%s\r\n", __FUNCTION__);

	p_GetDoorInfoList = xml_node_soap_get(p_body, "GetDoorInfoList");
	assert(p_GetDoorInfoList);

	memset(&req, 0, sizeof(req));
	
	ret = parse_tdc_GetDoorInfoList(p_GetDoorInfoList, &req);
	if (ONVIF_OK == ret)
	{	
	    tdc_GetDoorInfoList_RES res;
	    memset(&res, 0, sizeof(res));
	    
		ret = onvif_tdc_GetDoorInfoList(&req, &res);
		if (ONVIF_OK == ret)
		{
			ret = soap_build_send_rly(p_user, rx_msg, build_tdc_GetDoorInfoList_rly_xml, (char *)&res, NULL, p_header);
			onvif_free_Doors(&res.DoorInfo);
			return ret;
		}
	}
	
	return soap_build_err_rly(p_user, rx_msg, ret);
}

int soap_tdc_GetDoorInfo(HTTPCLN * p_user, HTTPMSG * rx_msg, XMLN * p_body, XMLN * p_header)
{
    XMLN * p_GetDoorInfo;
	tdc_GetDoorInfo_REQ req;
	ONVIF_RET ret;
	
	onvif_print("%s\r\n", __FUNCTION__);

	p_GetDoorInfo = xml_node_soap_get(p_body, "GetDoorInfo");
	assert(p_GetDoorInfo);

	memset(&req, 0, sizeof(req));
	
	ret = parse_tdc_GetDoorInfo(p_GetDoorInfo, &req);
	if (ONVIF_OK == ret)
	{
		return soap_build_send_rly(p_user, rx_msg, build_tdc_GetDoorInfo_rly_xml, (char *)&req, NULL, p_header);
	}
	
	return soap_build_err_rly(p_user, rx_msg, ret);
}

int soap_tdc_GetDoorState(HTTPCLN * p_user, HTTPMSG * rx_msg, XMLN * p_body, XMLN * p_header)
{
    XMLN * p_GetDoorState;
	tdc_GetDoorState_REQ req;
	ONVIF_RET ret;
	
	onvif_print("%s\r\n", __FUNCTION__);

	p_GetDoorState = xml_node_soap_get(p_body, "GetDoorState");
	assert(p_GetDoorState);

	memset(&req, 0, sizeof(req));
	
	ret = parse_tdc_GetDoorState(p_GetDoorState, &req);
	if (ONVIF_OK == ret)
	{
		return soap_build_send_rly(p_user, rx_msg, build_tdc_GetDoorState_rly_xml, (char *)&req, NULL, p_header);
	}
	
	return soap_build_err_rly(p_user, rx_msg, ret);
}

int soap_tdc_AccessDoor(HTTPCLN * p_user, HTTPMSG * rx_msg, XMLN * p_body, XMLN * p_header)
{
    XMLN * p_AccessDoor;
	tdc_AccessDoor_REQ req;
	ONVIF_RET ret;
	
	onvif_print("%s\r\n", __FUNCTION__);

	p_AccessDoor = xml_node_soap_get(p_body, "AccessDoor");
	assert(p_AccessDoor);

	memset(&req, 0, sizeof(req));
	
	ret = parse_tdc_AccessDoor(p_AccessDoor, &req);
	if (ONVIF_OK == ret)
	{
	    ret = onvif_tdc_AccessDoor(&req);
	    if (ONVIF_OK == ret)
	    {
		    return soap_build_send_rly(p_user, rx_msg, build_tdc_AccessDoor_rly_xml, (char *)&req, NULL, p_header);
		}
	}
	
	return soap_build_err_rly(p_user, rx_msg, ret);
}

int soap_tdc_LockDoor(HTTPCLN * p_user, HTTPMSG * rx_msg, XMLN * p_body, XMLN * p_header)
{
    XMLN * p_LockDoor;
	tdc_LockDoor_REQ req;
	ONVIF_RET ret;
	
	onvif_print("%s\r\n", __FUNCTION__);

	p_LockDoor = xml_node_soap_get(p_body, "LockDoor");
	assert(p_LockDoor);

	memset(&req, 0, sizeof(req));
	
	ret = parse_tdc_LockDoor(p_LockDoor, &req);
	if (ONVIF_OK == ret)
	{
	    ret = onvif_tdc_LockDoor(&req);
	    if (ONVIF_OK == ret)
	    {
		    return soap_build_send_rly(p_user, rx_msg, build_tdc_LockDoor_rly_xml, (char *)&req, NULL, p_header);
		}
	}
	
	return soap_build_err_rly(p_user, rx_msg, ret);
}

int soap_tdc_UnlockDoor(HTTPCLN * p_user, HTTPMSG * rx_msg, XMLN * p_body, XMLN * p_header)
{
    XMLN * p_UnlockDoor;
	tdc_UnlockDoor_REQ req;
	ONVIF_RET ret;
	
	onvif_print("%s\r\n", __FUNCTION__);

	p_UnlockDoor = xml_node_soap_get(p_body, "UnlockDoor");
	assert(p_UnlockDoor);

	memset(&req, 0, sizeof(req));
	
	ret = parse_tdc_UnlockDoor(p_UnlockDoor, &req);
	if (ONVIF_OK == ret)
	{
	    ret = onvif_tdc_UnlockDoor(&req);
	    if (ONVIF_OK == ret)
	    {
		    return soap_build_send_rly(p_user, rx_msg, build_tdc_UnlockDoor_rly_xml, (char *)&req, NULL, p_header);
		}
	}
	
	return soap_build_err_rly(p_user, rx_msg, ret);
}

int soap_tdc_DoubleLockDoor(HTTPCLN * p_user, HTTPMSG * rx_msg, XMLN * p_body, XMLN * p_header)
{
    XMLN * p_DoubleLockDoor;
	tdc_DoubleLockDoor_REQ req;
	ONVIF_RET ret;
	
	onvif_print("%s\r\n", __FUNCTION__);

	p_DoubleLockDoor = xml_node_soap_get(p_body, "DoubleLockDoor");
	assert(p_DoubleLockDoor);

	memset(&req, 0, sizeof(req));
	
	ret = parse_tdc_DoubleLockDoor(p_DoubleLockDoor, &req);
	if (ONVIF_OK == ret)
	{
	    ret = onvif_tdc_DoubleLockDoor(&req);
	    if (ONVIF_OK == ret)
	    {
		    return soap_build_send_rly(p_user, rx_msg, build_tdc_DoubleLockDoor_rly_xml, (char *)&req, NULL, p_header);
		}
	}
	
	return soap_build_err_rly(p_user, rx_msg, ret);
}

int soap_tdc_BlockDoor(HTTPCLN * p_user, HTTPMSG * rx_msg, XMLN * p_body, XMLN * p_header)
{
    XMLN * p_BlockDoor;
	tdc_BlockDoor_REQ req;
	ONVIF_RET ret;
	
	onvif_print("%s\r\n", __FUNCTION__);

	p_BlockDoor = xml_node_soap_get(p_body, "BlockDoor");
	assert(p_BlockDoor);

	memset(&req, 0, sizeof(req));
	
	ret = parse_tdc_BlockDoor(p_BlockDoor, &req);
	if (ONVIF_OK == ret)
	{
	    ret = onvif_tdc_BlockDoor(&req);
	    if (ONVIF_OK == ret)
	    {
		    return soap_build_send_rly(p_user, rx_msg, build_tdc_BlockDoor_rly_xml, (char *)&req, NULL, p_header);
		}
	}
	
	return soap_build_err_rly(p_user, rx_msg, ret);
}

int soap_tdc_LockDownDoor(HTTPCLN * p_user, HTTPMSG * rx_msg, XMLN * p_body, XMLN * p_header)
{
    XMLN * p_LockDownDoor;
	tdc_LockDownDoor_REQ req;
	ONVIF_RET ret;
	
	onvif_print("%s\r\n", __FUNCTION__);

	p_LockDownDoor = xml_node_soap_get(p_body, "LockDownDoor");
	assert(p_LockDownDoor);

	memset(&req, 0, sizeof(req));
	
	ret = parse_tdc_LockDownDoor(p_LockDownDoor, &req);
	if (ONVIF_OK == ret)
	{
	    ret = onvif_tdc_LockDownDoor(&req);
	    if (ONVIF_OK == ret)
	    {
		    return soap_build_send_rly(p_user, rx_msg, build_tdc_LockDownDoor_rly_xml, (char *)&req, NULL, p_header);
		}
	}
	
	return soap_build_err_rly(p_user, rx_msg, ret);
}

int soap_tdc_LockDownReleaseDoor(HTTPCLN * p_user, HTTPMSG * rx_msg, XMLN * p_body, XMLN * p_header)
{
    XMLN * p_LockDownReleaseDoor;
	tdc_LockDownReleaseDoor_REQ req;
	ONVIF_RET ret;
	
	onvif_print("%s\r\n", __FUNCTION__);

	p_LockDownReleaseDoor = xml_node_soap_get(p_body, "LockDownReleaseDoor");
	assert(p_LockDownReleaseDoor);

	memset(&req, 0, sizeof(req));
	
	ret = parse_tdc_LockDownReleaseDoor(p_LockDownReleaseDoor, &req);
	if (ONVIF_OK == ret)
	{
	    ret = onvif_tdc_LockDownReleaseDoor(&req);
	    if (ONVIF_OK == ret)
	    {
		    return soap_build_send_rly(p_user, rx_msg, build_tdc_LockDownReleaseDoor_rly_xml, (char *)&req, NULL, p_header);
		}
	}
	
	return soap_build_err_rly(p_user, rx_msg, ret);
}

int soap_tdc_LockOpenDoor(HTTPCLN * p_user, HTTPMSG * rx_msg, XMLN * p_body, XMLN * p_header)
{
    XMLN * p_LockOpenDoor;
	tdc_LockOpenDoor_REQ req;
	ONVIF_RET ret;
	
	onvif_print("%s\r\n", __FUNCTION__);

	p_LockOpenDoor = xml_node_soap_get(p_body, "LockOpenDoor");
	assert(p_LockOpenDoor);

	memset(&req, 0, sizeof(req));
	
	ret = parse_tdc_LockOpenDoor(p_LockOpenDoor, &req);
	if (ONVIF_OK == ret)
	{
	    ret = onvif_tdc_LockOpenDoor(&req);
	    if (ONVIF_OK == ret)
	    {
		    return soap_build_send_rly(p_user, rx_msg, build_tdc_LockOpenDoor_rly_xml, (char *)&req, NULL, p_header);
		}
	}
	
	return soap_build_err_rly(p_user, rx_msg, ret);
}

int soap_tdc_LockOpenReleaseDoor(HTTPCLN * p_user, HTTPMSG * rx_msg, XMLN * p_body, XMLN * p_header)
{
    XMLN * p_LockOpenReleaseDoor;
	tdc_LockOpenReleaseDoor_REQ req;
	ONVIF_RET ret;
	
	onvif_print("%s\r\n", __FUNCTION__);

	p_LockOpenReleaseDoor = xml_node_soap_get(p_body, "LockOpenReleaseDoor");
	assert(p_LockOpenReleaseDoor);

	memset(&req, 0, sizeof(req));
	
	ret = parse_tdc_LockOpenReleaseDoor(p_LockOpenReleaseDoor, &req);
	if (ONVIF_OK == ret)
	{
	    ret = onvif_tdc_LockOpenReleaseDoor(&req);
	    if (ONVIF_OK == ret)
	    {
		    return soap_build_send_rly(p_user, rx_msg, build_tdc_LockOpenReleaseDoor_rly_xml, (char *)&req, NULL, p_header);
		}
	}
	
	return soap_build_err_rly(p_user, rx_msg, ret);
}
    
#endif // end of PROFILE_C_SUPPORT

#ifdef DEVICEIO_SUPPORT

int soap_GetVideoOutputs(HTTPCLN * p_user, HTTPMSG * rx_msg, XMLN * p_body, XMLN * p_header)
{
	onvif_print("%s\r\n", __FUNCTION__);
	
    return soap_build_send_rly(p_user, rx_msg, build_GetVideoOutputs_rly_xml, NULL, NULL, p_header);
}

int soap_GetVideoOutputConfiguration(HTTPCLN * p_user, HTTPMSG * rx_msg, XMLN * p_body, XMLN * p_header)
{
	XMLN * p_GetVideoOutputConfiguration;
	GetVideoOutputConfiguration_REQ req;
	ONVIF_RET ret;
	
	onvif_print("%s\r\n", __FUNCTION__);

	p_GetVideoOutputConfiguration = xml_node_soap_get(p_body, "GetVideoOutputConfiguration");
	assert(p_GetVideoOutputConfiguration);

	memset(&req, 0, sizeof(req));
	
	ret = parse_GetVideoOutputConfiguration(p_GetVideoOutputConfiguration, &req);
	if (ONVIF_OK == ret)
	{
	    return soap_build_send_rly(p_user, rx_msg, build_GetVideoOutputConfiguration_rly_xml, (char *)&req, NULL, p_header);
	}
	
	return soap_build_err_rly(p_user, rx_msg, ret);
}

int soap_SetVideoOutputConfiguration(HTTPCLN * p_user, HTTPMSG * rx_msg, XMLN * p_body, XMLN * p_header)
{
    XMLN * p_SetVideoOutputConfiguration;
	SetVideoOutputConfiguration_REQ req;
	ONVIF_RET ret;
	
	onvif_print("%s\r\n", __FUNCTION__);

	p_SetVideoOutputConfiguration = xml_node_soap_get(p_body, "SetVideoOutputConfiguration");
	assert(p_SetVideoOutputConfiguration);

	memset(&req, 0, sizeof(req));
	
	ret = parse_SetVideoOutputConfiguration(p_SetVideoOutputConfiguration, &req);
	if (ONVIF_OK == ret)
	{
	    ret = onvif_SetVideoOutputConfiguration(&req);
	    if (ONVIF_OK == ret)
	    {
		    return soap_build_send_rly(p_user, rx_msg, build_SetVideoOutputConfiguration_rly_xml, NULL, NULL, p_header);
		}    
	}
	
	return soap_build_err_rly(p_user, rx_msg, ret);
}

int soap_GetVideoOutputConfigurationOptions(HTTPCLN * p_user, HTTPMSG * rx_msg, XMLN * p_body, XMLN * p_header)
{
    XMLN * p_GetVideoOutputConfigurationOptions;
	GetVideoOutputConfigurationOptions_REQ req;
	ONVIF_RET ret;
	
	onvif_print("%s\r\n", __FUNCTION__);

	p_GetVideoOutputConfigurationOptions = xml_node_soap_get(p_body, "GetVideoOutputConfigurationOptions");
	assert(p_GetVideoOutputConfigurationOptions);

	memset(&req, 0, sizeof(req));
	
	ret = parse_GetVideoOutputConfigurationOptions(p_GetVideoOutputConfigurationOptions, &req);
	if (ONVIF_OK == ret)
	{
	    return soap_build_send_rly(p_user, rx_msg, build_GetVideoOutputConfigurationOptions_rly_xml, (char *)&req, NULL, p_header);
	}
	
	return soap_build_err_rly(p_user, rx_msg, ret);
}

int soap_GetAudioOutputs(HTTPCLN * p_user, HTTPMSG * rx_msg, XMLN * p_body, XMLN * p_header)
{
    char * post;
    
    onvif_print("%s\r\n", __FUNCTION__);

    post = rx_msg->first_line.value_string;

    if (strstr(post, "deviceIO"))
    {
	    return soap_build_send_rly(p_user, rx_msg, build_GetAudioOutputs_rly_xml, NULL, NULL, p_header);
	}
	else
	{
	    return soap_build_send_rly(p_user, rx_msg, build_trt_GetAudioOutputs_rly_xml, NULL, NULL, p_header);
	}
}

int soap_AddAudioOutputConfiguration(HTTPCLN * p_user, HTTPMSG * rx_msg, XMLN * p_body, XMLN * p_header)
{
    XMLN * p_AddAudioOutputConfiguration;
	AddAudioOutputConfiguration_REQ req;
	ONVIF_RET ret;
	
	onvif_print("%s\r\n", __FUNCTION__);

	p_AddAudioOutputConfiguration = xml_node_soap_get(p_body, "AddAudioOutputConfiguration");
	assert(p_AddAudioOutputConfiguration);

	memset(&req, 0, sizeof(req));
	
	ret = parse_AddAudioOutputConfiguration(p_AddAudioOutputConfiguration, &req);
	if (ONVIF_OK == ret)
	{
	    ret = onvif_AddAudioOutputConfiguration(&req);
	    if (ONVIF_OK == ret)
	    {
		    return soap_build_send_rly(p_user, rx_msg, build_AddAudioOutputConfiguration_rly_xml, NULL, NULL, p_header);
		}    
	}
	
	return soap_build_err_rly(p_user, rx_msg, ret);
}

int soap_RemoveAudioOutputConfiguration(HTTPCLN * p_user, HTTPMSG * rx_msg, XMLN * p_body, XMLN * p_header)
{
    XMLN * p_RemoveAudioOutputConfiguration;
	RemoveAudioOutputConfiguration_REQ req;
	ONVIF_RET ret;
	
	onvif_print("%s\r\n", __FUNCTION__);

	p_RemoveAudioOutputConfiguration = xml_node_soap_get(p_body, "RemoveAudioOutputConfiguration");
	assert(p_RemoveAudioOutputConfiguration);

	memset(&req, 0, sizeof(req));
	
	ret = parse_RemoveAudioOutputConfiguration(p_RemoveAudioOutputConfiguration, &req);
	if (ONVIF_OK == ret)
	{
	    ret = onvif_RemoveAudioOutputConfiguration(&req);
	    if (ONVIF_OK == ret)
	    {
		    return soap_build_send_rly(p_user, rx_msg, build_RemoveAudioOutputConfiguration_rly_xml, NULL, NULL, p_header);
		}    
	}
	
	return soap_build_err_rly(p_user, rx_msg, ret);
}

int soap_GetAudioOutputConfigurations(HTTPCLN * p_user, HTTPMSG * rx_msg, XMLN * p_body, XMLN * p_header)
{
    onvif_print("%s\r\n", __FUNCTION__);

	return soap_build_send_rly(p_user, rx_msg, build_GetAudioOutputConfigurations_rly_xml, NULL, NULL, p_header);
}

int soap_GetCompatibleAudioOutputConfigurations(HTTPCLN * p_user, HTTPMSG * rx_msg, XMLN * p_body, XMLN * p_header)
{
    XMLN * p_GetCompatibleAudioOutputConfigurations;
	GetCompatibleAudioOutputConfigurations_REQ req;
	ONVIF_RET ret;
	
	onvif_print("%s\r\n", __FUNCTION__);

	p_GetCompatibleAudioOutputConfigurations = xml_node_soap_get(p_body, "GetCompatibleAudioOutputConfigurations");
	assert(p_GetCompatibleAudioOutputConfigurations);

	memset(&req, 0, sizeof(req));
	
	ret = parse_GetCompatibleAudioOutputConfigurations(p_GetCompatibleAudioOutputConfigurations, &req);
	if (ONVIF_OK == ret)
	{
	    return soap_build_send_rly(p_user, rx_msg, build_GetCompatibleAudioOutputConfigurations_rly_xml, (char *)&req, NULL, p_header);
	}
	
	return soap_build_err_rly(p_user, rx_msg, ret);
}

int soap_GetAudioOutputConfiguration(HTTPCLN * p_user, HTTPMSG * rx_msg, XMLN * p_body, XMLN * p_header)
{
    char * post;
    XMLN * p_GetAudioOutputConfiguration;	
	ONVIF_RET ret;
	
	onvif_print("%s\r\n", __FUNCTION__);

	p_GetAudioOutputConfiguration = xml_node_soap_get(p_body, "GetAudioOutputConfiguration");
	assert(p_GetAudioOutputConfiguration);
	
	post = rx_msg->first_line.value_string;

	if (strstr(post, "media"))
	{
	    trt_GetAudioOutputConfiguration_REQ req;
	    memset(&req, 0, sizeof(req));
	    
	    ret = parse_trt_GetAudioOutputConfiguration(p_GetAudioOutputConfiguration, &req);
    	if (ONVIF_OK == ret)
    	{
	        return soap_build_send_rly(p_user, rx_msg, build_trt_GetAudioOutputConfiguration_rly_xml, (char *)&req, NULL, p_header);
    	}
	}
	else
	{
	    GetAudioOutputConfiguration_REQ req;
	    memset(&req, 0, sizeof(req));
	    
    	ret = parse_GetAudioOutputConfiguration(p_GetAudioOutputConfiguration, &req);
    	if (ONVIF_OK == ret)
    	{
	        return soap_build_send_rly(p_user, rx_msg, build_GetAudioOutputConfiguration_rly_xml, (char *)&req, NULL, p_header);
    	}    
	}
	
	return soap_build_err_rly(p_user, rx_msg, ret);
}

int soap_SetAudioOutputConfiguration(HTTPCLN * p_user, HTTPMSG * rx_msg, XMLN * p_body, XMLN * p_header)
{
    XMLN * p_SetAudioOutputConfiguration;
	SetAudioOutputConfiguration_REQ req;
	ONVIF_RET ret;
	
	onvif_print("%s\r\n", __FUNCTION__);

	p_SetAudioOutputConfiguration = xml_node_soap_get(p_body, "SetAudioOutputConfiguration");
	assert(p_SetAudioOutputConfiguration);

	memset(&req, 0, sizeof(req));
	
	ret = parse_SetAudioOutputConfiguration(p_SetAudioOutputConfiguration, &req);
	if (ONVIF_OK == ret)
	{
	    ret = onvif_SetAudioOutputConfiguration(&req);
	    if (ONVIF_OK == ret)
	    {
		    return soap_build_send_rly(p_user, rx_msg, build_SetAudioOutputConfiguration_rly_xml, NULL, NULL, p_header);
		}    
	}
	
	return soap_build_err_rly(p_user, rx_msg, ret);
}

int soap_trt_GetAudioOutputConfigurationOptions(HTTPCLN * p_user, HTTPMSG * rx_msg, XMLN * p_body, XMLN * p_header)
{
    XMLN * p_GetAudioOutputConfigurationOptions;
	trt_GetAudioOutputConfigurationOptions_REQ req;
	ONVIF_RET ret;
	
	onvif_print("%s\r\n", __FUNCTION__);

	p_GetAudioOutputConfigurationOptions = xml_node_soap_get(p_body, "GetAudioOutputConfigurationOptions");
	assert(p_GetAudioOutputConfigurationOptions);

	memset(&req, 0, sizeof(req));
	
	ret = parse_trt_GetAudioOutputConfigurationOptions(p_GetAudioOutputConfigurationOptions, &req);
	if (ONVIF_OK == ret)
	{
	    return soap_build_send_rly(p_user, rx_msg, build_trt_GetAudioOutputConfigurationOptions_rly_xml, (char *)&req, NULL, p_header);
	}
	
	return soap_build_err_rly(p_user, rx_msg, ret);
}

int soap_tmd_GetAudioOutputConfigurationOptions(HTTPCLN * p_user, HTTPMSG * rx_msg, XMLN * p_body, XMLN * p_header)
{
    XMLN * p_GetAudioOutputConfigurationOptions;
	GetAudioOutputConfigurationOptions_REQ req;
	ONVIF_RET ret;
	
	onvif_print("%s\r\n", __FUNCTION__);

	p_GetAudioOutputConfigurationOptions = xml_node_soap_get(p_body, "GetAudioOutputConfigurationOptions");
	assert(p_GetAudioOutputConfigurationOptions);

	memset(&req, 0, sizeof(req));
	
	ret = parse_GetAudioOutputConfigurationOptions(p_GetAudioOutputConfigurationOptions, &req);
	if (ONVIF_OK == ret)
	{
	    return soap_build_send_rly(p_user, rx_msg, build_GetAudioOutputConfigurationOptions_rly_xml, (char *)&req, NULL, p_header);
	}
	
	return soap_build_err_rly(p_user, rx_msg, ret);
}

int soap_GetRelayOutputs(HTTPCLN * p_user, HTTPMSG * rx_msg, XMLN * p_body, XMLN * p_header)
{
    onvif_print("%s\r\n", __FUNCTION__);
    
    return soap_build_send_rly(p_user, rx_msg, build_tds_GetRelayOutputs_rly_xml, NULL, NULL, p_header);
}

int soap_tmd_GetRelayOutputs(HTTPCLN * p_user, HTTPMSG * rx_msg, XMLN * p_body, XMLN * p_header)
{
    onvif_print("%s\r\n", __FUNCTION__);
    
    return soap_build_send_rly(p_user, rx_msg, build_tmd_GetRelayOutputs_rly_xml, NULL, NULL, p_header);
}

int soap_GetRelayOutputOptions(HTTPCLN * p_user, HTTPMSG * rx_msg, XMLN * p_body, XMLN * p_header)
{
    XMLN * p_GetRelayOutputOptions;
	GetRelayOutputOptions_REQ req;
	ONVIF_RET ret;
	
	onvif_print("%s\r\n", __FUNCTION__);

	p_GetRelayOutputOptions = xml_node_soap_get(p_body, "GetRelayOutputOptions");
	assert(p_GetRelayOutputOptions);

	memset(&req, 0, sizeof(req));
	
	ret = parse_GetRelayOutputOptions(p_GetRelayOutputOptions, &req);
	if (ONVIF_OK == ret)
	{
	    return soap_build_send_rly(p_user, rx_msg, build_GetRelayOutputOptions_rly_xml, (char *)&req, NULL, p_header);
	}
	
	return soap_build_err_rly(p_user, rx_msg, ret);
}

int soap_SetRelayOutputSettings(HTTPCLN * p_user, HTTPMSG * rx_msg, XMLN * p_body, XMLN * p_header)
{
    char * post;
    XMLN * p_SetRelayOutputSettings;
	SetRelayOutputSettings_REQ req;
	ONVIF_RET ret;
	
	onvif_print("%s\r\n", __FUNCTION__);

    post = rx_msg->first_line.value_string;
    
	p_SetRelayOutputSettings = xml_node_soap_get(p_body, "SetRelayOutputSettings");
	assert(p_SetRelayOutputSettings);

	memset(&req, 0, sizeof(req));

	if (strstr(post, "deviceIO"))
	{
	    ret = parse_SetRelayOutputSettings(p_SetRelayOutputSettings, &req);
	}
	else
	{
	    ret = parse_trt_SetRelayOutputSettings(p_SetRelayOutputSettings, &req);
	}
	
	if (ONVIF_OK == ret)
	{
	    ret = onvif_SetRelayOutputSettings(&req);
	    if (ONVIF_OK == ret)
	    {
            if (strstr(post, "deviceIO"))
            {
		        return soap_build_send_rly(p_user, rx_msg, build_tmd_SetRelayOutputSettings_rly_xml, NULL, NULL, p_header);
		    }
		    else
		    {
		        return soap_build_send_rly(p_user, rx_msg, build_tds_SetRelayOutputSettings_rly_xml, NULL, NULL, p_header);
		    }
		}
	}
	
	return soap_build_err_rly(p_user, rx_msg, ret);
}

int soap_SetRelayOutputState(HTTPCLN * p_user, HTTPMSG * rx_msg, XMLN * p_body, XMLN * p_header)
{
    XMLN * p_SetRelayOutputState;
	SetRelayOutputState_REQ req;
	ONVIF_RET ret;
	
	onvif_print("%s\r\n", __FUNCTION__);

	p_SetRelayOutputState = xml_node_soap_get(p_body, "SetRelayOutputState");
	assert(p_SetRelayOutputState);

	memset(&req, 0, sizeof(req));
	
	ret = parse_SetRelayOutputState(p_SetRelayOutputState, &req);
	if (ONVIF_OK == ret)
	{
	    ret = onvif_SetRelayOutputState(&req);
	    if (ONVIF_OK == ret)
	    {
		    return soap_build_send_rly(p_user, rx_msg, build_SetRelayOutputState_rly_xml, NULL, NULL, p_header);
		}
	}
	
	return soap_build_err_rly(p_user, rx_msg, ret);
}

int soap_tmd_SetRelayOutputState(HTTPCLN * p_user, HTTPMSG * rx_msg, XMLN * p_body, XMLN * p_header)
{
    XMLN * p_SetRelayOutputState;
	SetRelayOutputState_REQ req;
	ONVIF_RET ret;
	
	onvif_print("%s\r\n", __FUNCTION__);

	p_SetRelayOutputState = xml_node_soap_get(p_body, "SetRelayOutputState");
	assert(p_SetRelayOutputState);

	memset(&req, 0, sizeof(req));
	
	ret = parse_SetRelayOutputState(p_SetRelayOutputState, &req);
	if (ONVIF_OK == ret)
	{
	    ret = onvif_SetRelayOutputState(&req);
	    if (ONVIF_OK == ret)
	    {
		    return soap_build_send_rly(p_user, rx_msg, build_tmd_SetRelayOutputState_rly_xml, NULL, NULL, p_header);
		}
	}
	
	return soap_build_err_rly(p_user, rx_msg, ret);
}

int soap_GetDigitalInputs(HTTPCLN * p_user, HTTPMSG * rx_msg, XMLN * p_body, XMLN * p_header)
{
    onvif_print("%s\r\n", __FUNCTION__);

	return soap_build_send_rly(p_user, rx_msg, build_GetDigitalInputs_rly_xml, NULL, NULL, p_header);
}

int soap_GetDigitalInputConfigurationOptions(HTTPCLN * p_user, HTTPMSG * rx_msg, XMLN * p_body, XMLN * p_header)
{
    XMLN * p_GetDigitalInputConfigurationOptions;
	GetDigitalInputConfigurationOptions_REQ req;
	ONVIF_RET ret;
	
	onvif_print("%s\r\n", __FUNCTION__);

	p_GetDigitalInputConfigurationOptions = xml_node_soap_get(p_body, "GetDigitalInputConfigurationOptions");
	assert(p_GetDigitalInputConfigurationOptions);

	memset(&req, 0, sizeof(req));
	
	ret = parse_GetDigitalInputConfigurationOptions(p_GetDigitalInputConfigurationOptions, &req);
	if (ONVIF_OK == ret)
	{
	    return soap_build_send_rly(p_user, rx_msg, build_GetDigitalInputConfigurationOptions_rly_xml, (char *)&req, NULL, p_header);
	}
	
	return soap_build_err_rly(p_user, rx_msg, ret);
}

int soap_SetDigitalInputConfigurations(HTTPCLN * p_user, HTTPMSG * rx_msg, XMLN * p_body, XMLN * p_header)
{
    XMLN * p_SetDigitalInputConfigurations;
	SetDigitalInputConfigurations_REQ req;
	ONVIF_RET ret;
	
	onvif_print("%s\r\n", __FUNCTION__);

	p_SetDigitalInputConfigurations = xml_node_soap_get(p_body, "SetDigitalInputConfigurations");
	assert(p_SetDigitalInputConfigurations);

	memset(&req, 0, sizeof(req));
	
	ret = parse_SetDigitalInputConfigurations(p_SetDigitalInputConfigurations, &req);
	if (ONVIF_OK == ret)
	{
	    ret = onvif_SetDigitalInputConfigurations(&req);
        onvif_free_DigitalInputs(&req.DigitalInputs);
        
	    if (ONVIF_OK == ret)
	    {
		    return soap_build_send_rly(p_user, rx_msg, build_SetDigitalInputConfigurations_rly_xml, NULL, NULL, p_header);
		}
	}
	
	return soap_build_err_rly(p_user, rx_msg, ret);
}

int soap_GetSerialPorts(HTTPCLN * p_user, HTTPMSG * rx_msg, XMLN * p_body, XMLN * p_header)
{
    onvif_print("%s\r\n", __FUNCTION__);

	return soap_build_send_rly(p_user, rx_msg, build_GetSerialPorts_rly_xml, NULL, NULL, p_header);
}

int soap_GetSerialPortConfiguration(HTTPCLN * p_user, HTTPMSG * rx_msg, XMLN * p_body, XMLN * p_header)
{
    XMLN * p_GetSerialPortConfiguration;
	GetSerialPortConfiguration_REQ req;
	ONVIF_RET ret;
	
	onvif_print("%s\r\n", __FUNCTION__);

	p_GetSerialPortConfiguration = xml_node_soap_get(p_body, "GetSerialPortConfiguration");
	assert(p_GetSerialPortConfiguration);

	memset(&req, 0, sizeof(req));
	
	ret = parse_GetSerialPortConfiguration(p_GetSerialPortConfiguration, &req);
	if (ONVIF_OK == ret)
	{
	    return soap_build_send_rly(p_user, rx_msg, build_GetSerialPortConfiguration_rly_xml, (char *)&req, NULL, p_header);
	}
	
	return soap_build_err_rly(p_user, rx_msg, ret);
}

int soap_GetSerialPortConfigurationOptions(HTTPCLN * p_user, HTTPMSG * rx_msg, XMLN * p_body, XMLN * p_header)
{
    XMLN * p_GetSerialPortConfigurationOptions;
	GetSerialPortConfigurationOptions_REQ req;
	ONVIF_RET ret;
	
	onvif_print("%s\r\n", __FUNCTION__);

	p_GetSerialPortConfigurationOptions = xml_node_soap_get(p_body, "GetSerialPortConfigurationOptions");
	assert(p_GetSerialPortConfigurationOptions);

	memset(&req, 0, sizeof(req));
	
	ret = parse_GetSerialPortConfigurationOptions(p_GetSerialPortConfigurationOptions, &req);
	if (ONVIF_OK == ret)
	{
	    return soap_build_send_rly(p_user, rx_msg, build_GetSerialPortConfigurationOptions_rly_xml, (char *)&req, NULL, p_header);
	}
	
	return soap_build_err_rly(p_user, rx_msg, ret);
}

int soap_SetSerialPortConfiguration(HTTPCLN * p_user, HTTPMSG * rx_msg, XMLN * p_body, XMLN * p_header)
{
    XMLN * p_SetSerialPortConfiguration;
	SetSerialPortConfiguration_REQ req;
	ONVIF_RET ret;
	
	onvif_print("%s\r\n", __FUNCTION__);

	p_SetSerialPortConfiguration = xml_node_soap_get(p_body, "SetSerialPortConfiguration");
	assert(p_SetSerialPortConfiguration);

	memset(&req, 0, sizeof(req));
	
	ret = parse_SetSerialPortConfiguration(p_SetSerialPortConfiguration, &req);
	if (ONVIF_OK == ret)
	{
	    ret = onvif_SetSerialPortConfiguration(&req);        
	    if (ONVIF_OK == ret)
	    {
		    return soap_build_send_rly(p_user, rx_msg, build_SetSerialPortConfiguration_rly_xml, NULL, NULL, p_header);
		}
	}
	
	return soap_build_err_rly(p_user, rx_msg, ret);
}

int soap_SendReceiveSerialCommand(HTTPCLN * p_user, HTTPMSG * rx_msg, XMLN * p_body, XMLN * p_header)
{
    XMLN * p_SendReceiveSerialCommand;
	SendReceiveSerialCommand_REQ req;
	ONVIF_RET ret;
	
	onvif_print("%s\r\n", __FUNCTION__);

	p_SendReceiveSerialCommand = xml_node_soap_get(p_body, "SendReceiveSerialCommand");
	assert(p_SendReceiveSerialCommand);

	memset(&req, 0, sizeof(req));
	
	ret = parse_SendReceiveSerialCommand(p_SendReceiveSerialCommand, &req);
	if (ONVIF_OK == ret)
	{
	    SendReceiveSerialCommand_RES res;
	    memset(&res, 0, sizeof(res));
	    
	    ret = onvif_SendReceiveSerialCommandRx(&req, &res);        
	    if (ONVIF_OK == ret)
	    {
		    ret = soap_build_send_rly(p_user, rx_msg, build_SendReceiveSerialCommand_rly_xml, (char *)&res, NULL, p_header);
		}

        if (req.Command.SerialDataFlag)
        {
            onvif_free_SerialData(&req.Command.SerialData);
        }
        if (res.SerialDataFlag)
        {
            onvif_free_SerialData(&res.SerialData);
        }
        
		return ret;
	}
	
	return soap_build_err_rly(p_user, rx_msg, ret);
}

int soap_tmd_GetAudioSources(HTTPCLN * p_user, HTTPMSG * rx_msg, XMLN * p_body, XMLN * p_header)
{
    onvif_print("%s\r\n", __FUNCTION__);

	return soap_build_send_rly(p_user, rx_msg, build_tmd_GetAudioSources_rly_xml, NULL, NULL, p_header);
}

#endif // end of DEVICEIO_SUPPORT

#ifdef MEDIA2_SUPPORT

int soap_tr2_GetProfiles(HTTPCLN * p_user, HTTPMSG * rx_msg, XMLN * p_body, XMLN * p_header)
{
    ONVIF_RET ret;
	XMLN * p_GetProfiles;
	tr2_GetProfiles_REQ req;
	
    onvif_print("%s\r\n", __FUNCTION__);

    p_GetProfiles = xml_node_soap_get(p_body, "GetProfiles");
    assert(p_GetProfiles);
	
	memset(&req, 0, sizeof(req));

	ret = parse_tr2_GetProfiles(p_GetProfiles, &req);
    if (ONVIF_OK == ret)
    {
    	return soap_build_send_rly(p_user, rx_msg, build_tr2_GetProfiles_rly_xml, (char *)&req, NULL, p_header);
    }
    
    return soap_build_err_rly(p_user, rx_msg, ret);
}

int soap_tr2_CreateProfile(HTTPCLN * p_user, HTTPMSG * rx_msg, XMLN * p_body, XMLN * p_header)
{
    ONVIF_RET ret;
	XMLN * p_CreateProfile;
	tr2_CreateProfile_REQ req;
	
    onvif_print("%s\r\n", __FUNCTION__);

    p_CreateProfile = xml_node_soap_get(p_body, "CreateProfile");
    assert(p_CreateProfile);
	
	memset(&req, 0, sizeof(req));

	ret = parse_tr2_CreateProfile(p_CreateProfile, &req);
    if (ONVIF_OK == ret)
    {
        tr2_CreateProfile_RES res;
        memset(&res, 0, sizeof(res));
        
        ret = onvif_tr2_CreateProfile(p_user->lip, p_user->lport, &req, &res);
        if (ONVIF_OK == ret)
        {
        	return soap_build_send_rly(p_user, rx_msg, build_tr2_CreateProfile_rly_xml, (char *)&res, NULL, p_header);
        }
    }
    
    return soap_build_err_rly(p_user, rx_msg, ret);
}

int soap_tr2_DeleteProfile(HTTPCLN * p_user, HTTPMSG * rx_msg, XMLN * p_body, XMLN * p_header)
{
    ONVIF_RET ret;
	XMLN * p_DeleteProfile;
	tr2_DeleteProfile_REQ req;
	
    onvif_print("%s\r\n", __FUNCTION__);

    p_DeleteProfile = xml_node_soap_get(p_body, "DeleteProfile");
    assert(p_DeleteProfile);
	
	memset(&req, 0, sizeof(req));

	ret = parse_tr2_DeleteProfile(p_DeleteProfile, &req);
    if (ONVIF_OK == ret)
    {
        ret = onvif_tr2_DeleteProfile(&req);
        if (ONVIF_OK == ret)
        {
        	return soap_build_send_rly(p_user, rx_msg, build_tr2_DeleteProfile_rly_xml, (char *)&req, NULL, p_header);
        }
    }
    
    return soap_build_err_rly(p_user, rx_msg, ret);
}

int soap_tr2_AddConfiguration(HTTPCLN * p_user, HTTPMSG * rx_msg, XMLN * p_body, XMLN * p_header)
{
    ONVIF_RET ret;
	XMLN * p_AddConfiguration;
	tr2_AddConfiguration_REQ req;
	
    onvif_print("%s\r\n", __FUNCTION__);

    p_AddConfiguration = xml_node_soap_get(p_body, "AddConfiguration");
    assert(p_AddConfiguration);
	
	memset(&req, 0, sizeof(req));

	ret = parse_tr2_AddConfiguration(p_AddConfiguration, &req);
    if (ONVIF_OK == ret)
    {
        ret = onvif_tr2_AddConfiguration(&req);
        if (ONVIF_OK == ret)
        {
        	return soap_build_send_rly(p_user, rx_msg, build_tr2_AddConfiguration_rly_xml, (char *)&req, NULL, p_header);
        }
    }
    
    return soap_build_err_rly(p_user, rx_msg, ret);
}

int soap_tr2_RemoveConfiguration(HTTPCLN * p_user, HTTPMSG * rx_msg, XMLN * p_body, XMLN * p_header)
{
    ONVIF_RET ret;
	XMLN * p_RemoveConfiguration;
	tr2_RemoveConfiguration_REQ req;
	
    onvif_print("%s\r\n", __FUNCTION__);

    p_RemoveConfiguration = xml_node_soap_get(p_body, "RemoveConfiguration");
    assert(p_RemoveConfiguration);
	
	memset(&req, 0, sizeof(req));

	ret = parse_tr2_RemoveConfiguration(p_RemoveConfiguration, &req);
    if (ONVIF_OK == ret)
    {
        ret = onvif_tr2_RemoveConfiguration(&req);
        if (ONVIF_OK == ret)
        {
        	return soap_build_send_rly(p_user, rx_msg, build_tr2_RemoveConfiguration_rly_xml, (char *)&req, NULL, p_header);
        }
    }
    
    return soap_build_err_rly(p_user, rx_msg, ret);
}

int soap_tr2_GetVideoEncoderConfigurations(HTTPCLN * p_user, HTTPMSG * rx_msg, XMLN * p_body, XMLN * p_header)
{
    ONVIF_RET ret;
    XMLN * p_GetVideoEncoderConfigurations;
    tr2_GetVideoEncoderConfigurations_REQ req;

    onvif_print("%s\r\n", __FUNCTION__);
    
    memset(&req, 0, sizeof(req));
    
    p_GetVideoEncoderConfigurations = xml_node_soap_get(p_body, "GetVideoEncoderConfigurations");
    assert(p_GetVideoEncoderConfigurations);

    ret = parse_tr2_GetConfiguration(p_GetVideoEncoderConfigurations, &req.GetConfiguration);
    if (ONVIF_OK == ret)
    {
        return soap_build_send_rly(p_user, rx_msg, build_tr2_GetVideoEncoderConfigurations_rly_xml, (char *)&req, NULL, p_header);
    }

    return soap_build_err_rly(p_user, rx_msg, ret);
}

int soap_tr2_GetVideoSourceConfigurations(HTTPCLN * p_user, HTTPMSG * rx_msg, XMLN * p_body, XMLN * p_header)
{
    ONVIF_RET ret;
    XMLN * p_GetVideoSourceConfigurations;
    tr2_GetVideoSourceConfigurations_REQ req;

    onvif_print("%s\r\n", __FUNCTION__);
    
    memset(&req, 0, sizeof(req));
    
    p_GetVideoSourceConfigurations = xml_node_soap_get(p_body, "GetVideoSourceConfigurations");
    assert(p_GetVideoSourceConfigurations);

    ret = parse_tr2_GetConfiguration(p_GetVideoSourceConfigurations, &req.GetConfiguration);
    if (ONVIF_OK == ret)
    {
        return soap_build_send_rly(p_user, rx_msg, build_tr2_GetVideoSourceConfigurations_rly_xml, (char *)&req, NULL, p_header);
    }

    return soap_build_err_rly(p_user, rx_msg, ret);
}

#ifdef AUDIO_SUPPORT

int soap_tr2_GetAudioEncoderConfigurations(HTTPCLN * p_user, HTTPMSG * rx_msg, XMLN * p_body, XMLN * p_header)
{
    ONVIF_RET ret;
    XMLN * p_GetAudioEncoderConfigurations;
    tr2_GetAudioEncoderConfigurations_REQ req;

    onvif_print("%s\r\n", __FUNCTION__);
    
    memset(&req, 0, sizeof(req));
    
    p_GetAudioEncoderConfigurations = xml_node_soap_get(p_body, "GetAudioEncoderConfigurations");
    assert(p_GetAudioEncoderConfigurations);

    ret = parse_tr2_GetConfiguration(p_GetAudioEncoderConfigurations, &req.GetConfiguration);
    if (ONVIF_OK == ret)
    {
        return soap_build_send_rly(p_user, rx_msg, build_tr2_GetAudioEncoderConfigurations_rly_xml, (char *)&req, NULL, p_header);
    }

    return soap_build_err_rly(p_user, rx_msg, ret);
}

int soap_tr2_GetAudioSourceConfigurations(HTTPCLN * p_user, HTTPMSG * rx_msg, XMLN * p_body, XMLN * p_header)
{
    ONVIF_RET ret;
    XMLN * p_GetAudioSourceConfigurations;
    tr2_GetAudioSourceConfigurations_REQ req;

    onvif_print("%s\r\n", __FUNCTION__);
    
    memset(&req, 0, sizeof(req));
    
    p_GetAudioSourceConfigurations = xml_node_soap_get(p_body, "GetAudioSourceConfigurations");
    assert(p_GetAudioSourceConfigurations);

    ret = parse_tr2_GetConfiguration(p_GetAudioSourceConfigurations, &req.GetConfiguration);
    if (ONVIF_OK == ret)
    {
        return soap_build_send_rly(p_user, rx_msg, build_tr2_GetAudioSourceConfigurations_rly_xml, (char *)&req, NULL, p_header);
    }

    return soap_build_err_rly(p_user, rx_msg, ret);
}

int soap_tr2_SetAudioEncoderConfiguration(HTTPCLN * p_user, HTTPMSG * rx_msg, XMLN * p_body, XMLN * p_header)
{
    ONVIF_RET ret;
	XMLN * p_SetAudioEncoderConfiguration;
	tr2_SetAudioEncoderConfiguration_REQ req;
	
    onvif_print("%s\r\n", __FUNCTION__);

    p_SetAudioEncoderConfiguration = xml_node_soap_get(p_body, "SetAudioEncoderConfiguration");
    assert(p_SetAudioEncoderConfiguration);
	
	memset(&req, 0, sizeof(req));

	ret = parse_tr2_SetAudioEncoderConfiguration(p_SetAudioEncoderConfiguration, &req);
    if (ONVIF_OK == ret)
    {
        ret = onvif_tr2_SetAudioEncoderConfiguration(&req);
        if (ONVIF_OK == ret)
        {
        	return soap_build_send_rly(p_user, rx_msg, build_tr2_SetAudioEncoderConfiguration_rly_xml, NULL, NULL, p_header);
        }
    }
    
    return soap_build_err_rly(p_user, rx_msg, ret);
}

int soap_tr2_GetAudioSourceConfigurationOptions(HTTPCLN * p_user, HTTPMSG * rx_msg, XMLN * p_body, XMLN * p_header)
{
    ONVIF_RET ret;
    XMLN * p_GetAudioSourceConfigurationOptions;
    tr2_GetAudioSourceConfigurationOptions_REQ req;

    onvif_print("%s\r\n", __FUNCTION__);
    
    memset(&req, 0, sizeof(req));
    
    p_GetAudioSourceConfigurationOptions = xml_node_soap_get(p_body, "GetAudioSourceConfigurationOptions");
    assert(p_GetAudioSourceConfigurationOptions);

    ret = parse_tr2_GetConfiguration(p_GetAudioSourceConfigurationOptions, &req.GetConfiguration);
    if (ONVIF_OK == ret)
    {
        return soap_build_send_rly(p_user, rx_msg, build_tr2_GetAudioSourceConfigurationOptions_rly_xml, (char *)&req, NULL, p_header);
    }

    return soap_build_err_rly(p_user, rx_msg, ret);
}

int soap_tr2_GetAudioEncoderConfigurationOptions(HTTPCLN * p_user, HTTPMSG * rx_msg, XMLN * p_body, XMLN * p_header)
{
    ONVIF_RET ret;
    XMLN * p_GetAudioEncoderConfigurationOptions;
    tr2_GetAudioEncoderConfigurationOptions_REQ req;

    onvif_print("%s\r\n", __FUNCTION__);
    
    memset(&req, 0, sizeof(req));
    
    p_GetAudioEncoderConfigurationOptions = xml_node_soap_get(p_body, "GetAudioEncoderConfigurationOptions");
    assert(p_GetAudioEncoderConfigurationOptions);

    ret = parse_tr2_GetConfiguration(p_GetAudioEncoderConfigurationOptions, &req.GetConfiguration);
    if (ONVIF_OK == ret)
    {
        return soap_build_send_rly(p_user, rx_msg, build_tr2_GetAudioEncoderConfigurationOptions_rly_xml, (char *)&req, NULL, p_header);
    }

    return soap_build_err_rly(p_user, rx_msg, ret);
}

int soap_tr2_SetAudioSourceConfiguration(HTTPCLN * p_user, HTTPMSG * rx_msg, XMLN * p_body, XMLN * p_header)
{
    ONVIF_RET ret;
	XMLN * p_SetAudioSourceConfiguration;
	tr2_SetAudioSourceConfiguration_REQ req;
	
    onvif_print("%s\r\n", __FUNCTION__);

    p_SetAudioSourceConfiguration = xml_node_soap_get(p_body, "SetAudioSourceConfiguration");
    assert(p_SetAudioSourceConfiguration);
	
	memset(&req, 0, sizeof(req));

	ret = parse_tr2_SetAudioSourceConfiguration(p_SetAudioSourceConfiguration, &req);
    if (ONVIF_OK == ret)
    {
        ret = onvif_tr2_SetAudioSourceConfiguration(&req);
        if (ONVIF_OK == ret)
        {
        	return soap_build_send_rly(p_user, rx_msg, build_tr2_SetAudioSourceConfiguration_rly_xml, NULL, NULL, p_header);
        }
    }
    
    return soap_build_err_rly(p_user, rx_msg, ret);
}

#endif // end of AUDIO_SUPPORT

int soap_tr2_GetMetadataConfigurations(HTTPCLN * p_user, HTTPMSG * rx_msg, XMLN * p_body, XMLN * p_header)
{
    ONVIF_RET ret;
    XMLN * p_GetMetadataConfigurations;
    tr2_GetMetadataConfigurations_REQ req;

    onvif_print("%s\r\n", __FUNCTION__);
    
    memset(&req, 0, sizeof(req));
    
    p_GetMetadataConfigurations = xml_node_soap_get(p_body, "GetMetadataConfigurations");
    assert(p_GetMetadataConfigurations);

    ret = parse_tr2_GetConfiguration(p_GetMetadataConfigurations, &req.GetConfiguration);
    if (ONVIF_OK == ret)
    {
        return soap_build_send_rly(p_user, rx_msg, build_tr2_GetMetadataConfigurations_rly_xml, (char *)&req, NULL, p_header);
    }

    return soap_build_err_rly(p_user, rx_msg, ret);
}

int soap_tr2_SetVideoEncoderConfiguration(HTTPCLN * p_user, HTTPMSG * rx_msg, XMLN * p_body, XMLN * p_header)
{
    ONVIF_RET ret;
	XMLN * p_SetVideoEncoderConfiguration;
	tr2_SetVideoEncoderConfiguration_REQ req;
	
    onvif_print("%s\r\n", __FUNCTION__);

    p_SetVideoEncoderConfiguration = xml_node_soap_get(p_body, "SetVideoEncoderConfiguration");
    assert(p_SetVideoEncoderConfiguration);
	
	memset(&req, 0, sizeof(req));

	ret = parse_tr2_SetVideoEncoderConfiguration(p_SetVideoEncoderConfiguration, &req);
    if (ONVIF_OK == ret)
    {
        ret = onvif_tr2_SetVideoEncoderConfiguration(&req);
        if (ONVIF_OK == ret)
        {
        	return soap_build_send_rly(p_user, rx_msg, build_tr2_SetVideoEncoderConfiguration_rly_xml, NULL, NULL, p_header);
        }
    }
    
    return soap_build_err_rly(p_user, rx_msg, ret);
}

int soap_tr2_SetVideoSourceConfiguration(HTTPCLN * p_user, HTTPMSG * rx_msg, XMLN * p_body, XMLN * p_header)
{
    ONVIF_RET ret;
	XMLN * p_SetVideoSourceConfiguration;
	tr2_SetVideoSourceConfiguration_REQ req;
	
    onvif_print("%s\r\n", __FUNCTION__);

    p_SetVideoSourceConfiguration = xml_node_soap_get(p_body, "SetVideoSourceConfiguration");
    assert(p_SetVideoSourceConfiguration);
	
	memset(&req, 0, sizeof(req));

	ret = parse_tr2_SetVideoSourceConfiguration(p_SetVideoSourceConfiguration, &req);
    if (ONVIF_OK == ret)
    {
        ret = onvif_tr2_SetVideoSourceConfiguration(&req);
        if (ONVIF_OK == ret)
        {
        	return soap_build_send_rly(p_user, rx_msg, build_tr2_SetVideoSourceConfiguration_rly_xml, NULL, NULL, p_header);
        }
    }
    
    return soap_build_err_rly(p_user, rx_msg, ret);
}

int soap_tr2_SetMetadataConfiguration(HTTPCLN * p_user, HTTPMSG * rx_msg, XMLN * p_body, XMLN * p_header)
{
    ONVIF_RET ret;
	XMLN * p_SetMetadataConfiguration;
	tr2_SetMetadataConfiguration_REQ req;
	
    onvif_print("%s\r\n", __FUNCTION__);

    p_SetMetadataConfiguration = xml_node_soap_get(p_body, "SetMetadataConfiguration");
    assert(p_SetMetadataConfiguration);
	
	memset(&req, 0, sizeof(req));

	ret = parse_tr2_SetMetadataConfiguration(p_SetMetadataConfiguration, &req);
    if (ONVIF_OK == ret)
    {
        ret = onvif_tr2_SetMetadataConfiguration(&req);
        if (ONVIF_OK == ret)
        {
        	return soap_build_send_rly(p_user, rx_msg, build_tr2_SetMetadataConfiguration_rly_xml, NULL, NULL, p_header);
        }
    }
    
    return soap_build_err_rly(p_user, rx_msg, ret);
}

int soap_tr2_GetVideoSourceConfigurationOptions(HTTPCLN * p_user, HTTPMSG * rx_msg, XMLN * p_body, XMLN * p_header)
{
    ONVIF_RET ret;
    XMLN * p_GetVideoSourceConfigurationOptions;
    tr2_GetVideoSourceConfigurationOptions_REQ req;

    onvif_print("%s\r\n", __FUNCTION__);
    
    memset(&req, 0, sizeof(req));
    
    p_GetVideoSourceConfigurationOptions = xml_node_soap_get(p_body, "GetVideoSourceConfigurationOptions");
    assert(p_GetVideoSourceConfigurationOptions);

    ret = parse_tr2_GetConfiguration(p_GetVideoSourceConfigurationOptions, &req.GetConfiguration);
    if (ONVIF_OK == ret)
    {
        return soap_build_send_rly(p_user, rx_msg, build_tr2_GetVideoSourceConfigurationOptions_rly_xml, (char *)&req, NULL, p_header);
    }

    return soap_build_err_rly(p_user, rx_msg, ret);
}

int soap_tr2_GetVideoEncoderConfigurationOptions(HTTPCLN * p_user, HTTPMSG * rx_msg, XMLN * p_body, XMLN * p_header)
{
    ONVIF_RET ret;
    XMLN * p_GetVideoEncoderConfigurationOptions;
    tr2_GetVideoEncoderConfigurationOptions_REQ req;

    onvif_print("%s\r\n", __FUNCTION__);
    
    memset(&req, 0, sizeof(req));
    
    p_GetVideoEncoderConfigurationOptions = xml_node_soap_get(p_body, "GetVideoEncoderConfigurationOptions");
    assert(p_GetVideoEncoderConfigurationOptions);

    ret = parse_tr2_GetConfiguration(p_GetVideoEncoderConfigurationOptions, &req.GetConfiguration);
    if (ONVIF_OK == ret)
    {
        return soap_build_send_rly(p_user, rx_msg, build_tr2_GetVideoEncoderConfigurationOptions_rly_xml, (char *)&req, NULL, p_header);
    }

    return soap_build_err_rly(p_user, rx_msg, ret);
}

int soap_tr2_GetMetadataConfigurationOptions(HTTPCLN * p_user, HTTPMSG * rx_msg, XMLN * p_body, XMLN * p_header)
{
    ONVIF_RET ret;
    XMLN * p_GetMetadataConfigurationOptions;
    tr2_GetMetadataConfigurationOptions_REQ req;

    onvif_print("%s\r\n", __FUNCTION__);
    
    memset(&req, 0, sizeof(req));
    
    p_GetMetadataConfigurationOptions = xml_node_soap_get(p_body, "GetMetadataConfigurationOptions");
    assert(p_GetMetadataConfigurationOptions);

    ret = parse_tr2_GetConfiguration(p_GetMetadataConfigurationOptions, &req.GetConfiguration);
    if (ONVIF_OK == ret)
    {
        return soap_build_send_rly(p_user, rx_msg, build_tr2_GetMetadataConfigurationOptions_rly_xml, (char *)&req, NULL, p_header);
    }

    return soap_build_err_rly(p_user, rx_msg, ret);
}

int soap_tr2_GetVideoEncoderInstances(HTTPCLN * p_user, HTTPMSG * rx_msg, XMLN * p_body, XMLN * p_header)
{
    ONVIF_RET ret;
    XMLN * p_GetVideoEncoderInstances;
    tr2_GetVideoEncoderInstances_REQ req;

    onvif_print("%s\r\n", __FUNCTION__);
    
    memset(&req, 0, sizeof(req));
    
    p_GetVideoEncoderInstances = xml_node_soap_get(p_body, "GetVideoEncoderInstances");
    assert(p_GetVideoEncoderInstances);

    ret = parse_tr2_GetVideoEncoderInstances(p_GetVideoEncoderInstances, &req);
    if (ONVIF_OK == ret)
    {
        tr2_GetVideoEncoderInstances_RES res;
        memset(&res, 0, sizeof(res));
        
        ret = onvif_tr2_GetVideoEncoderInstances(&req, &res);
        if (ONVIF_OK == ret)
        {
            return soap_build_send_rly(p_user, rx_msg, build_tr2_GetVideoEncoderInstances_rly_xml,(char *)&res, NULL, p_header);
        }
    }

    return soap_build_err_rly(p_user, rx_msg, ret);
}

int soap_tr2_GetStreamUri(HTTPCLN * p_user, HTTPMSG * rx_msg, XMLN * p_body, XMLN * p_header)
{
    ONVIF_RET ret;
    XMLN * p_GetStreamUri;
    tr2_GetStreamUri_REQ req;

    onvif_print("%s\r\n", __FUNCTION__);
    
    memset(&req, 0, sizeof(req));
    
    p_GetStreamUri = xml_node_soap_get(p_body, "GetStreamUri");
    assert(p_GetStreamUri);

    ret = parse_tr2_GetStreamUri(p_GetStreamUri, &req);
    if (ONVIF_OK == ret)
    {
        tr2_GetStreamUri_RES res;
	    memset(&res, 0, sizeof(res));
	    
		ret = onvif_tr2_GetStreamUri(p_user->lip, p_user->lport, &req, &res);
		if (ONVIF_OK == ret)
		{
			return soap_build_send_rly(p_user, rx_msg, build_tr2_GetStreamUri_rly_xml, (char *)&res, NULL, p_header);
		}        
    }

    return soap_build_err_rly(p_user, rx_msg, ret);
}

int soap_tr2_SetSynchronizationPoint(HTTPCLN * p_user, HTTPMSG * rx_msg, XMLN * p_body, XMLN * p_header)
{
    ONVIF_RET ret;
    XMLN * p_SetSynchronizationPoint;
    tr2_SetSynchronizationPoint_REQ req;

    onvif_print("%s\r\n", __FUNCTION__);
    
    memset(&req, 0, sizeof(req));
    
    p_SetSynchronizationPoint = xml_node_soap_get(p_body, "SetSynchronizationPoint");
    assert(p_SetSynchronizationPoint);

    ret = parse_tr2_SetSynchronizationPoint(p_SetSynchronizationPoint, &req);
    if (ONVIF_OK == ret)
    {
        ret = onvif_tr2_SetSynchronizationPoint(&req);
        if (ONVIF_OK == ret)
        {
            return soap_build_send_rly(p_user, rx_msg, build_tr2_SetSynchronizationPoint_rly_xml, (char *)&req, NULL, p_header);
        }
    }

    return soap_build_err_rly(p_user, rx_msg, ret);
}

int soap_tr2_GetVideoSourceModes(HTTPCLN * p_user, HTTPMSG * rx_msg, XMLN * p_body, XMLN * p_header)
{
    ONVIF_RET ret;
    XMLN * p_GetVideoSourceModes;
    tr2_GetVideoSourceModes_REQ req;

    onvif_print("%s\r\n", __FUNCTION__);
    
    memset(&req, 0, sizeof(req));
    
    p_GetVideoSourceModes = xml_node_soap_get(p_body, "GetVideoSourceModes");
    assert(p_GetVideoSourceModes);

    ret = parse_tr2_GetVideoSourceModes(p_GetVideoSourceModes, &req);
    if (ONVIF_OK == ret)
    {
        return soap_build_send_rly(p_user, rx_msg, build_tr2_GetVideoSourceModes_rly_xml, (char *)&req, NULL, p_header);
    }

    return soap_build_err_rly(p_user, rx_msg, ret);
}

int soap_tr2_SetVideoSourceMode(HTTPCLN * p_user, HTTPMSG * rx_msg, XMLN * p_body, XMLN * p_header)
{
    ONVIF_RET ret;
    XMLN * p_SetVideoSourceMode;
    tr2_SetVideoSourceMode_REQ req;
    
	onvif_print("%s\r\n", __FUNCTION__);

	p_SetVideoSourceMode = xml_node_soap_get(p_body, "SetVideoSourceMode");
    assert(p_SetVideoSourceMode);

	memset(&req, 0, sizeof(req));
	
    ret = parse_tr2_SetVideoSourceMode(p_SetVideoSourceMode, &req);
    if (ONVIF_OK == ret)
    {
        tr2_SetVideoSourceMode_RES res;
        memset(&res, 0, sizeof(res));
        
        ret = onvif_tr2_SetVideoSourceMode(&req, &res);
    	if (ONVIF_OK == ret)
    	{
		    return soap_build_send_rly(p_user, rx_msg, build_tr2_SetVideoSourceMode_rly_xml, (char*)&res, NULL, p_header);
		}
    }

    return soap_build_err_rly(p_user, rx_msg, ret);
}

int soap_tr2_GetSnapshotUri(HTTPCLN * p_user, HTTPMSG * rx_msg, XMLN * p_body, XMLN * p_header)
{
    ONVIF_RET ret;
    XMLN * p_GetSnapshotUri;    
    tr2_GetSnapshotUri_REQ req;
    
    onvif_print("%s\r\n", __FUNCTION__);

    p_GetSnapshotUri = xml_node_soap_get(p_body, "GetSnapshotUri");
    assert(p_GetSnapshotUri);

    memset(&req, 0, sizeof(req));

    ret = parse_tr2_GetSnapshotUri(p_GetSnapshotUri, &req);
	if (ONVIF_OK == ret)
	{
	    tr2_GetSnapshotUri_RES res;
	    memset(&res, 0, sizeof(res));
	    
		ret = onvif_tr2_GetSnapshotUri(p_user->lip, p_user->lport, &req, &res);
		if (ONVIF_OK == ret)
		{
			return soap_build_send_rly(p_user, rx_msg, build_tr2_GetSnapshotUri_rly_xml, (char *)&res, NULL, p_header);
		}
	}
	
	return soap_build_err_rly(p_user, rx_msg, ret);

}

int soap_tr2_SetOSD(HTTPCLN * p_user, HTTPMSG * rx_msg, XMLN * p_body, XMLN * p_header)
{
    ONVIF_RET ret;
	XMLN * p_SetOSD;
	SetOSD_REQ req;
		
	onvif_print("%s\r\n", __FUNCTION__);

	p_SetOSD = xml_node_soap_get(p_body, "SetOSD");
	assert(p_SetOSD);
	
	memset(&req, 0, sizeof(req));
    
    ret = parse_SetOSD(p_SetOSD, &req);
    if (ONVIF_OK == ret)
    {
    	ret = onvif_SetOSD(&req);
    	if (ONVIF_OK == ret)
    	{
			return soap_build_send_rly(p_user, rx_msg, build_tr2_SetOSD_rly_xml, NULL, NULL, p_header);
		}
    }

    return soap_build_err_rly(p_user, rx_msg, ret);
}

int soap_tr2_GetOSDOptions(HTTPCLN * p_user, HTTPMSG * rx_msg, XMLN * p_body, XMLN * p_header)
{
    onvif_print("%s\r\n", __FUNCTION__);

	return soap_build_send_rly(p_user, rx_msg, build_tr2_GetOSDOptions_rly_xml, NULL, NULL, p_header);	
}

int soap_tr2_GetOSDs(HTTPCLN * p_user, HTTPMSG * rx_msg, XMLN * p_body, XMLN * p_header)
{
    ONVIF_RET ret;
	XMLN * p_GetOSDs;
	tr2_GetOSDs_REQ req;
	
	onvif_print("%s\r\n", __FUNCTION__);

	p_GetOSDs = xml_node_soap_get(p_body, "GetOSDs");
	assert(p_GetOSDs);
	
	memset(&req, 0, sizeof(req));
    
    ret = parse_tr2_GetOSDs(p_GetOSDs, &req);
    if (ONVIF_OK == ret)
    {
		return soap_build_send_rly(p_user, rx_msg, build_tr2_GetOSDs_rly_xml, (char *)&req, NULL, p_header);
    }

    return soap_build_err_rly(p_user, rx_msg, ret);	
}

int soap_tr2_CreateOSD(HTTPCLN * p_user, HTTPMSG * rx_msg, XMLN * p_body, XMLN * p_header)
{
    ONVIF_RET ret;
	XMLN * p_CreateOSD;
	CreateOSD_REQ req;
	
	onvif_print("%s\r\n", __FUNCTION__);

	p_CreateOSD = xml_node_soap_get(p_body, "CreateOSD");
	assert(p_CreateOSD);
	
	memset(&req, 0, sizeof(req));
    
    ret = parse_CreateOSD(p_CreateOSD, &req);
    if (ONVIF_OK == ret)
    {
    	ret = onvif_CreateOSD(&req);
    	if (ONVIF_OK == ret)
    	{
			return soap_build_send_rly(p_user, rx_msg, build_tr2_CreateOSD_rly_xml, req.OSD.token, NULL, p_header);
		}
    }

    return soap_build_err_rly(p_user, rx_msg, ret);
}

int soap_tr2_DeleteOSD(HTTPCLN * p_user, HTTPMSG * rx_msg, XMLN * p_body, XMLN * p_header)
{
    ONVIF_RET ret;
	XMLN * p_DeleteOSD;
	DeleteOSD_REQ req;
	
	onvif_print("%s\r\n", __FUNCTION__);

	p_DeleteOSD = xml_node_soap_get(p_body, "DeleteOSD");
	assert(p_DeleteOSD);
	
	memset(&req, 0, sizeof(req));
    
    ret = parse_DeleteOSD(p_DeleteOSD, &req);
    if (ONVIF_OK == ret)
    {
    	ret = onvif_DeleteOSD(&req);
    	if (ONVIF_OK == ret)
    	{
			return soap_build_send_rly(p_user, rx_msg, build_tr2_DeleteOSD_rly_xml, NULL, NULL, p_header);
		}
    }

    return soap_build_err_rly(p_user, rx_msg, ret);
}

int soap_tr2_CreateMask(HTTPCLN * p_user, HTTPMSG * rx_msg, XMLN * p_body, XMLN * p_header)
{
    ONVIF_RET ret;
	XMLN * p_CreateMask;
	tr2_CreateMask_REQ req;
	
	onvif_print("%s\r\n", __FUNCTION__);

	p_CreateMask = xml_node_soap_get(p_body, "CreateMask");
	assert(p_CreateMask);
	
	memset(&req, 0, sizeof(req));
    
    ret = parse_tr2_CreateMask(p_CreateMask, &req);
    if (ONVIF_OK == ret)
    {
    	ret = onvif_tr2_CreateMask(&req);
    	if (ONVIF_OK == ret)
    	{
			return soap_build_send_rly(p_user, rx_msg, build_tr2_CreateMask_rly_xml, req.Mask.token, NULL, p_header);
		}
    }

    return soap_build_err_rly(p_user, rx_msg, ret);
}

int soap_tr2_DeleteMask(HTTPCLN * p_user, HTTPMSG * rx_msg, XMLN * p_body, XMLN * p_header)
{
    ONVIF_RET ret;
	XMLN * p_DeleteMask;
	tr2_DeleteMask_REQ req;
	
	onvif_print("%s\r\n", __FUNCTION__);

	p_DeleteMask = xml_node_soap_get(p_body, "DeleteMask");
	assert(p_DeleteMask);
	
	memset(&req, 0, sizeof(req));
    
    ret = parse_tr2_DeleteMask(p_DeleteMask, &req);
    if (ONVIF_OK == ret)
    {
    	ret = onvif_tr2_DeleteMask(&req);
    	if (ONVIF_OK == ret)
    	{
			return soap_build_send_rly(p_user, rx_msg, build_tr2_DeleteMask_rly_xml, NULL, NULL, p_header);
		}
    }

    return soap_build_err_rly(p_user, rx_msg, ret);
}

int soap_tr2_GetMasks(HTTPCLN * p_user, HTTPMSG * rx_msg, XMLN * p_body, XMLN * p_header)
{
    ONVIF_RET ret;
	XMLN * p_GGetMasks;
	tr2_GetMasks_REQ req;
	
	onvif_print("%s\r\n", __FUNCTION__);

	p_GGetMasks = xml_node_soap_get(p_body, "GetMasks");
	assert(p_GGetMasks);
	
	memset(&req, 0, sizeof(req));
    
    ret = parse_tr2_GetMasks(p_GGetMasks, &req);
    if (ONVIF_OK == ret)
    {
		return soap_build_send_rly(p_user, rx_msg, build_tr2_GetMasks_rly_xml, (char *)&req, NULL, p_header);
    }

    return soap_build_err_rly(p_user, rx_msg, ret);
}

int soap_tr2_SetMask(HTTPCLN * p_user, HTTPMSG * rx_msg, XMLN * p_body, XMLN * p_header)
{
    ONVIF_RET ret;
	XMLN * p_SetMask;
	tr2_SetMask_REQ req;
		
	onvif_print("%s\r\n", __FUNCTION__);

	p_SetMask = xml_node_soap_get(p_body, "SetMask");
	assert(p_SetMask);
	
	memset(&req, 0, sizeof(req));
    
    ret = parse_tr2_SetMask(p_SetMask, &req);
    if (ONVIF_OK == ret)
    {
    	ret = onvif_tr2_SetMask(&req);
    	if (ONVIF_OK == ret)
    	{
			return soap_build_send_rly(p_user, rx_msg, build_tr2_SetMask_rly_xml, NULL, NULL, p_header);
		}
    }

    return soap_build_err_rly(p_user, rx_msg, ret);
}

int soap_tr2_GetMaskOptions(HTTPCLN * p_user, HTTPMSG * rx_msg, XMLN * p_body, XMLN * p_header)
{
    onvif_print("%s\r\n", __FUNCTION__);

	return soap_build_send_rly(p_user, rx_msg, build_tr2_GetMaskOptions_rly_xml, NULL, NULL, p_header);	
}


#ifdef DEVICEIO_SUPPORT

int soap_tr2_GetAudioOutputConfigurationOptions(HTTPCLN * p_user, HTTPMSG * rx_msg, XMLN * p_body, XMLN * p_header)
{
    XMLN * p_GetAudioOutputConfigurationOptions;
	trt_GetAudioOutputConfigurationOptions_REQ req;
	ONVIF_RET ret;
	
	onvif_print("%s\r\n", __FUNCTION__);

	p_GetAudioOutputConfigurationOptions = xml_node_soap_get(p_body, "GetAudioOutputConfigurationOptions");
	assert(p_GetAudioOutputConfigurationOptions);

	memset(&req, 0, sizeof(req));
	
	ret = parse_trt_GetAudioOutputConfigurationOptions(p_GetAudioOutputConfigurationOptions, &req);
	if (ONVIF_OK == ret)
	{
	    return soap_build_send_rly(p_user, rx_msg, build_tr2_GetAudioOutputConfigurationOptions_rly_xml, (char *)&req, NULL, p_header);
	}
	
	return soap_build_err_rly(p_user, rx_msg, ret);    
}

int soap_tr2_GetAudioOutputConfigurations(HTTPCLN * p_user, HTTPMSG * rx_msg, XMLN * p_body, XMLN * p_header)
{
    XMLN * p_GetAudioOutputConfigurations;
	tr2_GetAudioOutputConfigurationOptions_REQ req;
	ONVIF_RET ret;
	
	onvif_print("%s\r\n", __FUNCTION__);

	p_GetAudioOutputConfigurations = xml_node_soap_get(p_body, "GetAudioOutputConfigurations");
	assert(p_GetAudioOutputConfigurations);

	memset(&req, 0, sizeof(req));
	
	ret = parse_tr2_GetConfiguration(p_GetAudioOutputConfigurations, &req.GetConfiguration);
	if (ONVIF_OK == ret)
	{
	    return soap_build_send_rly(p_user, rx_msg, build_tr2_GetAudioOutputConfigurations_rly_xml, (char *)&req, NULL, p_header);
	}
	
	return soap_build_err_rly(p_user, rx_msg, ret);
}

int soap_tr2_SetAudioOutputConfiguration(HTTPCLN * p_user, HTTPMSG * rx_msg, XMLN * p_body, XMLN * p_header)
{
    XMLN * p_SetAudioOutputConfiguration;
	tr2_SetAudioOutputConfiguration_REQ req;
	ONVIF_RET ret;
	
	onvif_print("%s\r\n", __FUNCTION__);

	p_SetAudioOutputConfiguration = xml_node_soap_get(p_body, "SetAudioOutputConfiguration");
	assert(p_SetAudioOutputConfiguration);

	memset(&req, 0, sizeof(req));
	
	ret = parse_tr2_SetAudioOutputConfiguration(p_SetAudioOutputConfiguration, &req);
	if (ONVIF_OK == ret)
	{
	    ret = onvif_tr2_SetAudioOutputConfiguration(&req);
	    if (ONVIF_OK == ret)
	    {
		    return soap_build_send_rly(p_user, rx_msg, build_tr2_SetAudioOutputConfiguration_rly_xml, NULL, NULL, p_header);
		}    
	}
	
	return soap_build_err_rly(p_user, rx_msg, ret);
}

#endif // end of DEVICEIO_SUPPORT

#ifdef AUDIO_SUPPORT

int soap_tr2_GetAudioDecoderConfigurations(HTTPCLN * p_user, HTTPMSG * rx_msg, XMLN * p_body, XMLN * p_header)
{
    XMLN * p_GetAudioDecoderConfigurations;
	tr2_GetAudioDecoderConfigurations_REQ req;
	ONVIF_RET ret;
	
	onvif_print("%s\r\n", __FUNCTION__);

	p_GetAudioDecoderConfigurations = xml_node_soap_get(p_body, "GetAudioDecoderConfigurations");
	assert(p_GetAudioDecoderConfigurations);

	memset(&req, 0, sizeof(req));
	
	ret = parse_tr2_GetConfiguration(p_GetAudioDecoderConfigurations, &req.GetConfiguration);
	if (ONVIF_OK == ret)
	{
	    return soap_build_send_rly(p_user, rx_msg, build_tr2_GetAudioDecoderConfigurations_rly_xml, (char*)&req, NULL, p_header);
	}
	
	return soap_build_err_rly(p_user, rx_msg, ret);
}

int soap_tr2_SetAudioDecoderConfiguration(HTTPCLN * p_user, HTTPMSG * rx_msg, XMLN * p_body, XMLN * p_header)
{
    ONVIF_RET ret;
    XMLN * p_SetAudioDecoderConfiguration;
    tr2_SetAudioDecoderConfiguration_REQ req;
    
	onvif_print("%s\r\n", __FUNCTION__);

    p_SetAudioDecoderConfiguration = xml_node_soap_get(p_body, "SetAudioDecoderConfiguration");
    assert(p_SetAudioDecoderConfiguration);
	
	memset(&req, 0, sizeof(req));

	ret = parse_tr2_SetAudioDecoderConfiguration(p_SetAudioDecoderConfiguration, &req);
    if (ONVIF_OK == ret)
    {
        ret = onvif_tr2_SetAudioDecoderConfiguration(&req);
        if (ONVIF_OK == ret)
        {
        	return soap_build_send_rly(p_user, rx_msg, build_tr2_SetAudioDecoderConfiguration_rly_xml, NULL, NULL, p_header);
        }
    }
    
    return soap_build_err_rly(p_user, rx_msg, ret);
}

int soap_tr2_GetAudioDecoderConfigurationOptions(HTTPCLN * p_user, HTTPMSG * rx_msg, XMLN * p_body, XMLN * p_header)
{
    ONVIF_RET ret;
	XMLN * p_GetAudioDecoderConfigurationOptions;
	tr2_GetAudioDecoderConfigurationOptions_REQ req;
	
    onvif_print("%s\r\n", __FUNCTION__);

	p_GetAudioDecoderConfigurationOptions = xml_node_soap_get(p_body, "GetAudioDecoderConfigurationOptions");
    assert(p_GetAudioDecoderConfigurationOptions);    
	
    memset(&req, 0, sizeof(req));

	ret = parse_tr2_GetConfiguration(p_GetAudioDecoderConfigurationOptions, &req.GetConfiguration);
	if (ONVIF_OK == ret)
	{
		return soap_build_send_rly(p_user, rx_msg, build_tr2_GetAudioDecoderConfigurationOptions_rly_xml, (char *)&req, NULL, p_header);	
	}

    return soap_build_err_rly(p_user, rx_msg, ret);
}

#endif // end of AUDIO_SUPPORT

#ifdef VIDEO_ANALYTICS

int soap_tr2_GetAnalyticsConfigurations(HTTPCLN * p_user, HTTPMSG * rx_msg, XMLN * p_body, XMLN * p_header)
{
    ONVIF_RET ret;
    XMLN * p_GetAnalyticsConfigurations;
    tr2_GetAnalyticsConfigurations_REQ req;

    onvif_print("%s\r\n", __FUNCTION__);
    
    memset(&req, 0, sizeof(req));
    
    p_GetAnalyticsConfigurations = xml_node_soap_get(p_body, "GetAnalyticsConfigurations");
    assert(p_GetAnalyticsConfigurations);

    ret = parse_tr2_GetConfiguration(p_GetAnalyticsConfigurations, &req.GetConfiguration);
    if (ONVIF_OK == ret)
    {
        return soap_build_send_rly(p_user, rx_msg, build_tr2_GetAnalyticsConfigurations_rly_xml, (char *)&req, NULL, p_header);
    }

    return soap_build_err_rly(p_user, rx_msg, ret);
}

#endif // end of VIDEO_ANALYTICS

#endif // end of MEDIA2_SUPPORT

#ifdef THERMAL_SUPPORT

int soap_tth_GetConfigurations(HTTPCLN * p_user, HTTPMSG * rx_msg, XMLN * p_body, XMLN * p_header)
{
	onvif_print("%s\r\n", __FUNCTION__);
		
	return soap_build_send_rly(p_user, rx_msg, build_tth_GetConfigurations_rly_xml, NULL, NULL, p_header);
}

int soap_tth_GetConfiguration(HTTPCLN * p_user, HTTPMSG * rx_msg, XMLN * p_body, XMLN * p_header)
{
    ONVIF_RET ret;
	XMLN * p_GetConfiguration;
	tth_GetConfiguration_REQ req;
	
	onvif_print("%s\r\n", __FUNCTION__);

    memset(&req, 0, sizeof(req));

	p_GetConfiguration = xml_node_soap_get(p_body, "GetConfiguration");
	assert(p_GetConfiguration);	

	ret = parse_tth_GetConfiguration(p_GetConfiguration, &req);
	if (ONVIF_OK == ret)
	{
		return soap_build_send_rly(p_user, rx_msg, build_tth_GetConfiguration_rly_xml, (char*)&req, NULL, p_header);
	}
	
	return soap_build_err_rly(p_user, rx_msg, ret);
}

int soap_tth_SetConfiguration(HTTPCLN * p_user, HTTPMSG * rx_msg, XMLN * p_body, XMLN * p_header)
{
	ONVIF_RET ret;
	XMLN * p_SetConfiguration;
	tth_SetConfiguration_REQ req;
		
	onvif_print("%s\r\n", __FUNCTION__);

	p_SetConfiguration = xml_node_soap_get(p_body, "SetConfiguration");
	assert(p_SetConfiguration);
	
	memset(&req, 0, sizeof(req));
    
    ret = parse_tth_SetConfiguration(p_SetConfiguration, &req);
    if (ONVIF_OK == ret)
    {
    	ret = onvif_tth_SetConfiguration(&req);
    	if (ONVIF_OK == ret)
    	{
			return soap_build_send_rly(p_user, rx_msg, build_tth_SetConfiguration_rly_xml, NULL, NULL, p_header);
		}
    }

    return soap_build_err_rly(p_user, rx_msg, ret);
}

int soap_tth_GetConfigurationOptions(HTTPCLN * p_user, HTTPMSG * rx_msg, XMLN * p_body, XMLN * p_header)
{
	ONVIF_RET ret;
	XMLN * p_GetConfigurationOptions;
	tth_GetConfigurationOptions_REQ req;
	
	onvif_print("%s\r\n", __FUNCTION__);

    memset(&req, 0, sizeof(req));

	p_GetConfigurationOptions = xml_node_soap_get(p_body, "GetConfigurationOptions");
	assert(p_GetConfigurationOptions);	

	ret = parse_tth_GetConfigurationOptions(p_GetConfigurationOptions, &req);
	if (ONVIF_OK == ret)
	{
		return soap_build_send_rly(p_user, rx_msg, build_tth_GetConfigurationOptions_rly_xml, (char*)&req, NULL, p_header);
	}
	
	return soap_build_err_rly(p_user, rx_msg, ret);
}

int soap_tth_GetRadiometryConfiguration(HTTPCLN * p_user, HTTPMSG * rx_msg, XMLN * p_body, XMLN * p_header)
{
    ONVIF_RET ret;
	XMLN * p_GetRadiometryConfiguration;
	tth_GetRadiometryConfiguration_REQ req;
	
	onvif_print("%s\r\n", __FUNCTION__);

    memset(&req, 0, sizeof(req));

	p_GetRadiometryConfiguration = xml_node_soap_get(p_body, "GetRadiometryConfiguration");
	assert(p_GetRadiometryConfiguration);	

	ret = parse_tth_GetRadiometryConfiguration(p_GetRadiometryConfiguration, &req);
	if (ONVIF_OK == ret)
	{
		return soap_build_send_rly(p_user, rx_msg, build_tth_GetRadiometryConfiguration_rly_xml, (char*)&req, NULL, p_header);
	}
	
	return soap_build_err_rly(p_user, rx_msg, ret);
}

int soap_tth_SetRadiometryConfiguration(HTTPCLN * p_user, HTTPMSG * rx_msg, XMLN * p_body, XMLN * p_header)
{
    ONVIF_RET ret;
	XMLN * p_SetRadiometryConfiguration;
	tth_SetRadiometryConfiguration_REQ req;
		
	onvif_print("%s\r\n", __FUNCTION__);

	p_SetRadiometryConfiguration = xml_node_soap_get(p_body, "SetRadiometryConfiguration");
	assert(p_SetRadiometryConfiguration);
	
	memset(&req, 0, sizeof(req));
    
    ret = parse_tth_SetRadiometryConfiguration(p_SetRadiometryConfiguration, &req);
    if (ONVIF_OK == ret)
    {
    	ret = onvif_tth_SetRadiometryConfiguration(&req);
    	if (ONVIF_OK == ret)
    	{
			return soap_build_send_rly(p_user, rx_msg, build_tth_SetRadiometryConfiguration_rly_xml, NULL, NULL, p_header);
		}
    }

    return soap_build_err_rly(p_user, rx_msg, ret);
}

int soap_tth_GetRadiometryConfigurationOptions(HTTPCLN * p_user, HTTPMSG * rx_msg, XMLN * p_body, XMLN * p_header)
{
    ONVIF_RET ret;
	XMLN * p_GetRadiometryConfigurationOptions;
	tth_GetRadiometryConfigurationOptions_REQ req;
	
	onvif_print("%s\r\n", __FUNCTION__);

    memset(&req, 0, sizeof(req));

	p_GetRadiometryConfigurationOptions = xml_node_soap_get(p_body, "GetRadiometryConfigurationOptions");
	assert(p_GetRadiometryConfigurationOptions);	

	ret = parse_tth_GetRadiometryConfigurationOptions(p_GetRadiometryConfigurationOptions, &req);
	if (ONVIF_OK == ret)
	{
		return soap_build_send_rly(p_user, rx_msg, build_tth_GetRadiometryConfigurationOptions_rly_xml, (char*)&req, NULL, p_header);
	}
	
	return soap_build_err_rly(p_user, rx_msg, ret);
}

#endif // end of THERMAL_SUPPORT

#ifdef CREDENTIAL_SUPPORT
int soap_tcr_GetCredentialInfo(HTTPCLN * p_user, HTTPMSG * rx_msg, XMLN * p_body, XMLN * p_header)
{
    ONVIF_RET ret;
	XMLN * p_GetCredentialInfo;
	tcr_GetCredentialInfo_REQ req;
	
	onvif_print("%s\r\n", __FUNCTION__);

    memset(&req, 0, sizeof(req));

	p_GetCredentialInfo = xml_node_soap_get(p_body, "GetCredentialInfo");
	assert(p_GetCredentialInfo);	

	ret = parse_tcr_GetCredentialInfo(p_GetCredentialInfo, &req);
	if (ONVIF_OK == ret)
	{
	    tcr_GetCredentialInfo_RES res;
	    memset(&res, 0, sizeof(res));
	    
	    ret = onvif_tcr_GetCredentialInfo(&req, &res);
	    if (ONVIF_OK == ret)
	    {
		    return soap_build_send_rly(p_user, rx_msg, build_tcr_GetCredentialInfo_rly_xml, (char*)&res, NULL, p_header);
		}
	}
	
	return soap_build_err_rly(p_user, rx_msg, ret);
}

int soap_tcr_GetCredentialInfoList(HTTPCLN * p_user, HTTPMSG * rx_msg, XMLN * p_body, XMLN * p_header)
{
    ONVIF_RET ret;
	XMLN * p_GetCredentialInfoList;
	tcr_GetCredentialInfoList_REQ req;
	
	onvif_print("%s\r\n", __FUNCTION__);

    memset(&req, 0, sizeof(req));

	p_GetCredentialInfoList = xml_node_soap_get(p_body, "GetCredentialInfoList");
	assert(p_GetCredentialInfoList);	

	ret = parse_tcr_GetCredentialInfoList(p_GetCredentialInfoList, &req);
	if (ONVIF_OK == ret)
	{
	    tcr_GetCredentialInfoList_RES res;
	    memset(&res, 0, sizeof(res));
	    
	    ret = onvif_tcr_GetCredentialInfoList(&req, &res);
	    if (ONVIF_OK == ret)
	    {
		    return soap_build_send_rly(p_user, rx_msg, build_tcr_GetCredentialInfoList_rly_xml, (char*)&res, NULL, p_header);
		}
	}
	
	return soap_build_err_rly(p_user, rx_msg, ret);
}

int soap_tcr_GetCredentials(HTTPCLN * p_user, HTTPMSG * rx_msg, XMLN * p_body, XMLN * p_header)
{
    ONVIF_RET ret;
	XMLN * p_GetCredentials;
	tcr_GetCredentials_REQ req;
	
	onvif_print("%s\r\n", __FUNCTION__);

    memset(&req, 0, sizeof(req));

	p_GetCredentials = xml_node_soap_get(p_body, "GetCredentials");
	assert(p_GetCredentials);	

	ret = parse_tcr_GetCredentials(p_GetCredentials, &req);
	if (ONVIF_OK == ret)
	{
	    tcr_GetCredentials_RES res;
	    memset(&res, 0, sizeof(res));
	    
	    ret = onvif_tcr_GetCredentials(&req, &res);
	    if (ONVIF_OK == ret)
	    {
		    return soap_build_send_rly(p_user, rx_msg, build_tcr_GetCredentials_rly_xml, (char*)&res, NULL, p_header);
		}
	}
	
	return soap_build_err_rly(p_user, rx_msg, ret);
}

int soap_tcr_GetCredentialList(HTTPCLN * p_user, HTTPMSG * rx_msg, XMLN * p_body, XMLN * p_header)
{
    ONVIF_RET ret;
	XMLN * p_GetCredentialList;
	tcr_GetCredentialList_REQ req;
	
	onvif_print("%s\r\n", __FUNCTION__);

    memset(&req, 0, sizeof(req));

	p_GetCredentialList = xml_node_soap_get(p_body, "GetCredentialList");
	assert(p_GetCredentialList);	

	ret = parse_tcr_GetCredentialList(p_GetCredentialList, &req);
	if (ONVIF_OK == ret)
	{
	    tcr_GetCredentialList_RES res;
	    memset(&res, 0, sizeof(res));
	    
	    ret = onvif_tcr_GetCredentialList(&req, &res);
	    if (ONVIF_OK == ret)
	    {
		    return soap_build_send_rly(p_user, rx_msg, build_tcr_GetCredentialList_rly_xml, (char*)&res, NULL, p_header);
		}
	}
	
	return soap_build_err_rly(p_user, rx_msg, ret);
}

int soap_tcr_CreateCredential(HTTPCLN * p_user, HTTPMSG * rx_msg, XMLN * p_body, XMLN * p_header)
{
    ONVIF_RET ret;
	XMLN * p_CreateCredential;
	tcr_CreateCredential_REQ req;
	
	onvif_print("%s\r\n", __FUNCTION__);

    memset(&req, 0, sizeof(req));

	p_CreateCredential = xml_node_soap_get(p_body, "CreateCredential");
	assert(p_CreateCredential);	

	ret = parse_tcr_CreateCredential(p_CreateCredential, &req);
	if (ONVIF_OK == ret)
	{
	    tcr_CreateCredential_RES res;
	    memset(&res, 0, sizeof(res));
	    
	    ret = onvif_tcr_CreateCredential(&req, &res);
	    if (ONVIF_OK == ret)
	    {
		    return soap_build_send_rly(p_user, rx_msg, build_tcr_CreateCredential_rly_xml, (char*)&res, NULL, p_header);
		}
	}
	
	return soap_build_err_rly(p_user, rx_msg, ret);
}

int soap_tcr_ModifyCredential(HTTPCLN * p_user, HTTPMSG * rx_msg, XMLN * p_body, XMLN * p_header)
{
    ONVIF_RET ret;
	XMLN * p_ModifyCredential;
	tcr_ModifyCredential_REQ req;
	
	onvif_print("%s\r\n", __FUNCTION__);

    memset(&req, 0, sizeof(req));

	p_ModifyCredential = xml_node_soap_get(p_body, "ModifyCredential");
	assert(p_ModifyCredential);	

	ret = parse_tcr_ModifyCredential(p_ModifyCredential, &req);
	if (ONVIF_OK == ret)
	{	    
	    ret = onvif_tcr_ModifyCredential(&req);
	    if (ONVIF_OK == ret)
	    {
		    return soap_build_send_rly(p_user, rx_msg, build_tcr_ModifyCredential_rly_xml, (char*)&req, NULL, p_header);
		}
	}
	
	return soap_build_err_rly(p_user, rx_msg, ret);
}

int soap_tcr_DeleteCredential(HTTPCLN * p_user, HTTPMSG * rx_msg, XMLN * p_body, XMLN * p_header)
{
    ONVIF_RET ret;
	XMLN * p_DeleteCredential;
	tcr_DeleteCredential_REQ req;
	
	onvif_print("%s\r\n", __FUNCTION__);

    memset(&req, 0, sizeof(req));

	p_DeleteCredential = xml_node_soap_get(p_body, "DeleteCredential");
	assert(p_DeleteCredential);	

	ret = parse_tcr_DeleteCredential(p_DeleteCredential, &req);
	if (ONVIF_OK == ret)
	{	    
	    ret = onvif_tcr_DeleteCredential(&req);
	    if (ONVIF_OK == ret)
	    {
		    return soap_build_send_rly(p_user, rx_msg, build_tcr_DeleteCredential_rly_xml, (char*)&req, NULL, p_header);
		}
	}
	
	return soap_build_err_rly(p_user, rx_msg, ret);
}

int soap_tcr_GetCredentialState(HTTPCLN * p_user, HTTPMSG * rx_msg, XMLN * p_body, XMLN * p_header)
{
    ONVIF_RET ret;
	XMLN * p_GetCredentialState;
	tcr_GetCredentialState_REQ req;
	
	onvif_print("%s\r\n", __FUNCTION__);

    memset(&req, 0, sizeof(req));

	p_GetCredentialState = xml_node_soap_get(p_body, "GetCredentialState");
	assert(p_GetCredentialState);	

	ret = parse_tcr_GetCredentialState(p_GetCredentialState, &req);
	if (ONVIF_OK == ret)
	{
	    tcr_GetCredentialState_RES res;
	    memset(&res, 0, sizeof(res));
	    
	    ret = onvif_tcr_GetCredentialState(&req, &res);
	    if (ONVIF_OK == ret)
	    {
		    return soap_build_send_rly(p_user, rx_msg, build_tcr_GetCredentialState_rly_xml, (char*)&res, NULL, p_header);
		}
	}
	
	return soap_build_err_rly(p_user, rx_msg, ret);
}

int soap_tcr_EnableCredential(HTTPCLN * p_user, HTTPMSG * rx_msg, XMLN * p_body, XMLN * p_header)
{
    ONVIF_RET ret;
	XMLN * p_EnableCredential;
	tcr_EnableCredential_REQ req;
	
	onvif_print("%s\r\n", __FUNCTION__);

    memset(&req, 0, sizeof(req));

	p_EnableCredential = xml_node_soap_get(p_body, "EnableCredential");
	assert(p_EnableCredential);	

	ret = parse_tcr_EnableCredential(p_EnableCredential, &req);
	if (ONVIF_OK == ret)
	{	    
	    ret = onvif_tcr_EnableCredential(&req);
	    if (ONVIF_OK == ret)
	    {
		    return soap_build_send_rly(p_user, rx_msg, build_tcr_EnableCredential_rly_xml, (char*)&req, NULL, p_header);
		}
	}
	
	return soap_build_err_rly(p_user, rx_msg, ret);
}

int soap_tcr_DisableCredential(HTTPCLN * p_user, HTTPMSG * rx_msg, XMLN * p_body, XMLN * p_header)
{
    ONVIF_RET ret;
	XMLN * p_DisableCredential;
	tcr_DisableCredential_REQ req;
	
	onvif_print("%s\r\n", __FUNCTION__);

    memset(&req, 0, sizeof(req));

	p_DisableCredential = xml_node_soap_get(p_body, "DisableCredential");
	assert(p_DisableCredential);	

	ret = parse_tcr_DisableCredential(p_DisableCredential, &req);
	if (ONVIF_OK == ret)
	{	    
	    ret = onvif_tcr_DisableCredential(&req);
	    if (ONVIF_OK == ret)
	    {
		    return soap_build_send_rly(p_user, rx_msg, build_tcr_DisableCredential_rly_xml, (char*)&req, NULL, p_header);
		}
	}
	
	return soap_build_err_rly(p_user, rx_msg, ret);
}

int soap_tcr_ResetAntipassbackViolation(HTTPCLN * p_user, HTTPMSG * rx_msg, XMLN * p_body, XMLN * p_header)
{
    ONVIF_RET ret;
	XMLN * p_ResetAntipassbackViolation;
	tcr_ResetAntipassbackViolation_REQ req;
	
	onvif_print("%s\r\n", __FUNCTION__);

    memset(&req, 0, sizeof(req));

	p_ResetAntipassbackViolation = xml_node_soap_get(p_body, "ResetAntipassbackViolation");
	assert(p_ResetAntipassbackViolation);	

	ret = parse_tcr_ResetAntipassbackViolation(p_ResetAntipassbackViolation, &req);
	if (ONVIF_OK == ret)
	{	    
	    ret = onvif_tcr_ResetAntipassbackViolation(&req);
	    if (ONVIF_OK == ret)
	    {
		    return soap_build_send_rly(p_user, rx_msg, build_tcr_ResetAntipassbackViolation_rly_xml, (char*)&req, NULL, p_header);
		}
	}
	
	return soap_build_err_rly(p_user, rx_msg, ret);
}

int soap_tcr_GetSupportedFormatTypes(HTTPCLN * p_user, HTTPMSG * rx_msg, XMLN * p_body, XMLN * p_header)
{
    ONVIF_RET ret;
	XMLN * p_GetSupportedFormatTypes;
	tcr_GetSupportedFormatTypes_REQ req;
	
	onvif_print("%s\r\n", __FUNCTION__);

    memset(&req, 0, sizeof(req));

	p_GetSupportedFormatTypes = xml_node_soap_get(p_body, "GetSupportedFormatTypes");
	assert(p_GetSupportedFormatTypes);	

	ret = parse_tcr_GetSupportedFormatTypes(p_GetSupportedFormatTypes, &req);
	if (ONVIF_OK == ret)
	{
	    tcr_GetSupportedFormatTypes_RES res;
	    memset(&res, 0, sizeof(res));
	    
	    ret = onvif_tcr_GetSupportedFormatTypes(&req, &res);
	    if (ONVIF_OK == ret)
	    {
		    return soap_build_send_rly(p_user, rx_msg, build_tcr_GetSupportedFormatTypes_rly_xml, (char*)&res, NULL, p_header);
		}
	}
	
	return soap_build_err_rly(p_user, rx_msg, ret);
}

int soap_tcr_GetCredentialIdentifiers(HTTPCLN * p_user, HTTPMSG * rx_msg, XMLN * p_body, XMLN * p_header)
{
    ONVIF_RET ret;
	XMLN * p_GetCredentialIdentifiers;
	tcr_GetCredentialIdentifiers_REQ req;
	
	onvif_print("%s\r\n", __FUNCTION__);

    memset(&req, 0, sizeof(req));

	p_GetCredentialIdentifiers = xml_node_soap_get(p_body, "GetCredentialIdentifiers");
	assert(p_GetCredentialIdentifiers);	

	ret = parse_tcr_GetCredentialIdentifiers(p_GetCredentialIdentifiers, &req);
	if (ONVIF_OK == ret)
	{
	    tcr_GetCredentialIdentifiers_RES res;
	    memset(&res, 0, sizeof(res));
	    
	    ret = onvif_tcr_GetCredentialIdentifiers(&req, &res);
	    if (ONVIF_OK == ret)
	    {
		    return soap_build_send_rly(p_user, rx_msg, build_tcr_GetCredentialIdentifiers_rly_xml, (char*)&res, NULL, p_header);
		}
	}
	
	return soap_build_err_rly(p_user, rx_msg, ret);
}

int soap_tcr_SetCredentialIdentifier(HTTPCLN * p_user, HTTPMSG * rx_msg, XMLN * p_body, XMLN * p_header)
{
    ONVIF_RET ret;
	XMLN * p_SetCredentialIdentifier;
	tcr_SetCredentialIdentifier_REQ req;
	
	onvif_print("%s\r\n", __FUNCTION__);

    memset(&req, 0, sizeof(req));

	p_SetCredentialIdentifier = xml_node_soap_get(p_body, "SetCredentialIdentifier");
	assert(p_SetCredentialIdentifier);	

	ret = parse_tcr_SetCredentialIdentifier(p_SetCredentialIdentifier, &req);
	if (ONVIF_OK == ret)
	{	    
	    ret = onvif_tcr_SetCredentialIdentifier(&req);
	    if (ONVIF_OK == ret)
	    {
		    return soap_build_send_rly(p_user, rx_msg, build_tcr_SetCredentialIdentifier_rly_xml, (char*)&req, NULL, p_header);
		}
	}
	
	return soap_build_err_rly(p_user, rx_msg, ret);
}

int soap_tcr_DeleteCredentialIdentifier(HTTPCLN * p_user, HTTPMSG * rx_msg, XMLN * p_body, XMLN * p_header)
{
    ONVIF_RET ret;
	XMLN * p_DeleteCredentialIdentifier;
	tcr_DeleteCredentialIdentifier_REQ req;
	
	onvif_print("%s\r\n", __FUNCTION__);

    memset(&req, 0, sizeof(req));

	p_DeleteCredentialIdentifier = xml_node_soap_get(p_body, "DeleteCredentialIdentifier");
	assert(p_DeleteCredentialIdentifier);	

	ret = parse_tcr_DeleteCredentialIdentifier(p_DeleteCredentialIdentifier, &req);
	if (ONVIF_OK == ret)
	{	    
	    ret = onvif_tcr_DeleteCredentialIdentifier(&req);
	    if (ONVIF_OK == ret)
	    {
		    return soap_build_send_rly(p_user, rx_msg, build_tcr_DeleteCredentialIdentifier_rly_xml, (char*)&req, NULL, p_header);
		}
	}
	
	return soap_build_err_rly(p_user, rx_msg, ret);
}

int soap_tcr_GetCredentialAccessProfiles(HTTPCLN * p_user, HTTPMSG * rx_msg, XMLN * p_body, XMLN * p_header)
{
    ONVIF_RET ret;
	XMLN * p_GetCredentialAccessProfiles;
	tcr_GetCredentialAccessProfiles_REQ req;
	
	onvif_print("%s\r\n", __FUNCTION__);

    memset(&req, 0, sizeof(req));

	p_GetCredentialAccessProfiles = xml_node_soap_get(p_body, "GetCredentialAccessProfiles");
	assert(p_GetCredentialAccessProfiles);	

	ret = parse_tcr_GetCredentialAccessProfiles(p_GetCredentialAccessProfiles, &req);
	if (ONVIF_OK == ret)
	{
	    tcr_GetCredentialAccessProfiles_RES res;
	    memset(&res, 0, sizeof(res));
	    
	    ret = onvif_tcr_GetCredentialAccessProfiles(&req, &res);
	    if (ONVIF_OK == ret)
	    {
		    return soap_build_send_rly(p_user, rx_msg, build_tcr_GetCredentialAccessProfiles_rly_xml, (char*)&res, NULL, p_header);
		}
	}
	
	return soap_build_err_rly(p_user, rx_msg, ret);
}

int soap_tcr_SetCredentialAccessProfiles(HTTPCLN * p_user, HTTPMSG * rx_msg, XMLN * p_body, XMLN * p_header)
{
    ONVIF_RET ret;
	XMLN * p_SetCredentialAccessProfiles;
	tcr_SetCredentialAccessProfiles_REQ req;
	
	onvif_print("%s\r\n", __FUNCTION__);

    memset(&req, 0, sizeof(req));

	p_SetCredentialAccessProfiles = xml_node_soap_get(p_body, "SetCredentialAccessProfiles");
	assert(p_SetCredentialAccessProfiles);	

	ret = parse_tcr_SetCredentialAccessProfiles(p_SetCredentialAccessProfiles, &req);
	if (ONVIF_OK == ret)
	{	    
	    ret = onvif_tcr_SetCredentialAccessProfiles(&req);
	    if (ONVIF_OK == ret)
	    {
		    return soap_build_send_rly(p_user, rx_msg, build_tcr_SetCredentialAccessProfiles_rly_xml, (char*)&req, NULL, p_header);
		}
	}
	
	return soap_build_err_rly(p_user, rx_msg, ret);
}

int soap_tcr_DeleteCredentialAccessProfiles(HTTPCLN * p_user, HTTPMSG * rx_msg, XMLN * p_body, XMLN * p_header)
{
    ONVIF_RET ret;
	XMLN * p_DeleteCredentialAccessProfiles;
	tcr_DeleteCredentialAccessProfiles_REQ req;
	
	onvif_print("%s\r\n", __FUNCTION__);

    memset(&req, 0, sizeof(req));

	p_DeleteCredentialAccessProfiles = xml_node_soap_get(p_body, "DeleteCredentialAccessProfiles");
	assert(p_DeleteCredentialAccessProfiles);	

	ret = parse_tcr_DeleteCredentialAccessProfiles(p_DeleteCredentialAccessProfiles, &req);
	if (ONVIF_OK == ret)
	{	    
	    ret = onvif_tcr_DeleteCredentialAccessProfiles(&req);
	    if (ONVIF_OK == ret)
	    {
		    return soap_build_send_rly(p_user, rx_msg, build_tcr_DeleteCredentialAccessProfiles_rly_xml, (char*)&req, NULL, p_header);
		}
	}
	
	return soap_build_err_rly(p_user, rx_msg, ret);
}
#endif // end of CREDENTIAL_SUPPORT

#ifdef ACCESS_RULES
int soap_tar_GetAccessProfileInfo(HTTPCLN * p_user, HTTPMSG * rx_msg, XMLN * p_body, XMLN * p_header)
{
    ONVIF_RET ret;
	XMLN * p_GetAccessProfileInfo;
	tar_GetAccessProfileInfo_REQ req;
	
	onvif_print("%s\r\n", __FUNCTION__);

    memset(&req, 0, sizeof(req));

	p_GetAccessProfileInfo = xml_node_soap_get(p_body, "GetAccessProfileInfo");
	assert(p_GetAccessProfileInfo);	

	ret = parse_tar_GetAccessProfileInfo(p_GetAccessProfileInfo, &req);
	if (ONVIF_OK == ret)
	{
	    tar_GetAccessProfileInfo_RES res;
	    memset(&res, 0, sizeof(res));
	    
	    ret = onvif_tar_GetAccessProfileInfo(&req, &res);
	    if (ONVIF_OK == ret)
	    {
		    return soap_build_send_rly(p_user, rx_msg, build_tar_GetAccessProfileInfo_rly_xml, (char*)&res, NULL, p_header);
		}
	}
	
	return soap_build_err_rly(p_user, rx_msg, ret);
}

int soap_tar_GetAccessProfileInfoList(HTTPCLN * p_user, HTTPMSG * rx_msg, XMLN * p_body, XMLN * p_header)
{
    ONVIF_RET ret;
	XMLN * p_GetAccessProfileInfoList;
	tar_GetAccessProfileInfoList_REQ req;
	
	onvif_print("%s\r\n", __FUNCTION__);

    memset(&req, 0, sizeof(req));

	p_GetAccessProfileInfoList = xml_node_soap_get(p_body, "GetAccessProfileInfoList");
	assert(p_GetAccessProfileInfoList);	

	ret = parse_tar_GetAccessProfileInfoList(p_GetAccessProfileInfoList, &req);
	if (ONVIF_OK == ret)
	{
	    tar_GetAccessProfileInfoList_RES res;
	    memset(&res, 0, sizeof(res));
	    
	    ret = onvif_tar_GetAccessProfileInfoList(&req, &res);
	    if (ONVIF_OK == ret)
	    {
		    return soap_build_send_rly(p_user, rx_msg, build_tar_GetAccessProfileInfoList_rly_xml, (char*)&res, NULL, p_header);
		}
	}
	
	return soap_build_err_rly(p_user, rx_msg, ret);
}

int soap_tar_GetAccessProfiles(HTTPCLN * p_user, HTTPMSG * rx_msg, XMLN * p_body, XMLN * p_header)
{
    ONVIF_RET ret;
	XMLN * p_GetAccessProfiles;
	tar_GetAccessProfiles_REQ req;
	
	onvif_print("%s\r\n", __FUNCTION__);

    memset(&req, 0, sizeof(req));

	p_GetAccessProfiles = xml_node_soap_get(p_body, "GetAccessProfiles");
	assert(p_GetAccessProfiles);	

	ret = parse_tar_GetAccessProfiles(p_GetAccessProfiles, &req);
	if (ONVIF_OK == ret)
	{
	    tar_GetAccessProfiles_RES res;
	    memset(&res, 0, sizeof(res));
	    
	    ret = onvif_tar_GetAccessProfiles(&req, &res);
	    if (ONVIF_OK == ret)
	    {
		    return soap_build_send_rly(p_user, rx_msg, build_tar_GetAccessProfiles_rly_xml, (char*)&res, NULL, p_header);
		}
	}
	
	return soap_build_err_rly(p_user, rx_msg, ret);
}

int soap_tar_GetAccessProfileList(HTTPCLN * p_user, HTTPMSG * rx_msg, XMLN * p_body, XMLN * p_header)
{
    ONVIF_RET ret;
	XMLN * p_GetAccessProfileList;
	tar_GetAccessProfileList_REQ req;
	
	onvif_print("%s\r\n", __FUNCTION__);

    memset(&req, 0, sizeof(req));

	p_GetAccessProfileList = xml_node_soap_get(p_body, "GetAccessProfileList");
	assert(p_GetAccessProfileList);	

	ret = parse_tar_GetAccessProfileList(p_GetAccessProfileList, &req);
	if (ONVIF_OK == ret)
	{
	    tar_GetAccessProfileList_RES res;
	    memset(&res, 0, sizeof(res));
	    
	    ret = onvif_tar_GetAccessProfileList(&req, &res);
	    if (ONVIF_OK == ret)
	    {
		    return soap_build_send_rly(p_user, rx_msg, build_tar_GetAccessProfileList_rly_xml, (char*)&res, NULL, p_header);
		}
	}
	
	return soap_build_err_rly(p_user, rx_msg, ret);
}

int soap_tar_CreateAccessProfile(HTTPCLN * p_user, HTTPMSG * rx_msg, XMLN * p_body, XMLN * p_header)
{
    ONVIF_RET ret;
	XMLN * p_CreateAccessProfile;
	tar_CreateAccessProfile_REQ req;
	
	onvif_print("%s\r\n", __FUNCTION__);

    memset(&req, 0, sizeof(req));

	p_CreateAccessProfile = xml_node_soap_get(p_body, "CreateAccessProfile");
	assert(p_CreateAccessProfile);	

	ret = parse_tar_CreateAccessProfile(p_CreateAccessProfile, &req);
	if (ONVIF_OK == ret)
	{
	    tar_CreateAccessProfile_RES res;
	    memset(&res, 0, sizeof(res));
	    
	    ret = onvif_tar_CreateAccessProfile(&req, &res);
	    if (ONVIF_OK == ret)
	    {
		    return soap_build_send_rly(p_user, rx_msg, build_tar_CreateAccessProfile_rly_xml, (char*)&res, NULL, p_header);
		}
	}
	
	return soap_build_err_rly(p_user, rx_msg, ret);
}

int soap_tar_ModifyAccessProfile(HTTPCLN * p_user, HTTPMSG * rx_msg, XMLN * p_body, XMLN * p_header)
{
    ONVIF_RET ret;
	XMLN * p_ModifyAccessProfile;
	tar_ModifyAccessProfile_REQ req;
	
	onvif_print("%s\r\n", __FUNCTION__);

    memset(&req, 0, sizeof(req));

	p_ModifyAccessProfile = xml_node_soap_get(p_body, "ModifyAccessProfile");
	assert(p_ModifyAccessProfile);	

	ret = parse_tar_ModifyAccessProfile(p_ModifyAccessProfile, &req);
	if (ONVIF_OK == ret)
	{	    
	    ret = onvif_tar_ModifyAccessProfile(&req);
	    if (ONVIF_OK == ret)
	    {
		    return soap_build_send_rly(p_user, rx_msg, build_tar_ModifyAccessProfile_rly_xml, NULL, NULL, p_header);
		}
	}
	
	return soap_build_err_rly(p_user, rx_msg, ret);
}

int soap_tar_DeleteAccessProfile(HTTPCLN * p_user, HTTPMSG * rx_msg, XMLN * p_body, XMLN * p_header)
{
    ONVIF_RET ret;
	XMLN * p_DeleteAccessProfile;
	tar_DeleteAccessProfile_REQ req;
	
	onvif_print("%s\r\n", __FUNCTION__);

    memset(&req, 0, sizeof(req));

	p_DeleteAccessProfile = xml_node_soap_get(p_body, "DeleteAccessProfile");
	assert(p_DeleteAccessProfile);	

	ret = parse_tar_DeleteAccessProfile(p_DeleteAccessProfile, &req);
	if (ONVIF_OK == ret)
	{
	    ret = onvif_tar_DeleteAccessProfile(&req);
	    if (ONVIF_OK == ret)
	    {
		    return soap_build_send_rly(p_user, rx_msg, build_tar_DeleteAccessProfile_rly_xml, NULL, NULL, p_header);
		}
	}
	
	return soap_build_err_rly(p_user, rx_msg, ret);
}
#endif // end of ACCESS_RULES

#ifdef SCHEDULE_SUPPORT
int soap_tsc_GetScheduleInfo(HTTPCLN * p_user, HTTPMSG * rx_msg, XMLN * p_body, XMLN * p_header)
{
    ONVIF_RET ret;
	XMLN * p_GetScheduleInfo;
	tsc_GetScheduleInfo_REQ req;
	
	onvif_print("%s\r\n", __FUNCTION__);

    memset(&req, 0, sizeof(req));

	p_GetScheduleInfo = xml_node_soap_get(p_body, "GetScheduleInfo");
	assert(p_GetScheduleInfo);	

	ret = parse_tsc_GetScheduleInfo(p_GetScheduleInfo, &req);
	if (ONVIF_OK == ret)
	{
	    tsc_GetScheduleInfo_RES res;
	    memset(&res, 0, sizeof(res));
	    
	    ret = onvif_tsc_GetScheduleInfo(&req, &res);
	    if (ONVIF_OK == ret)
	    {
		    return soap_build_send_rly(p_user, rx_msg, build_tsc_GetScheduleInfo_rly_xml, (char*)&res, NULL, p_header);
		}
	}
	
	return soap_build_err_rly(p_user, rx_msg, ret);
}
  
int soap_tsc_GetScheduleInfoList(HTTPCLN * p_user, HTTPMSG * rx_msg, XMLN * p_body, XMLN * p_header)
{
    ONVIF_RET ret;
	XMLN * p_GetScheduleInfoList;
	tsc_GetScheduleInfoList_REQ req;
	
	onvif_print("%s\r\n", __FUNCTION__);

    memset(&req, 0, sizeof(req));

	p_GetScheduleInfoList = xml_node_soap_get(p_body, "GetScheduleInfoList");
	assert(p_GetScheduleInfoList);	

	ret = parse_tsc_GetScheduleInfoList(p_GetScheduleInfoList, &req);
	if (ONVIF_OK == ret)
	{
	    tsc_GetScheduleInfoList_RES res;
	    memset(&res, 0, sizeof(res));
	    
	    ret = onvif_tsc_GetScheduleInfoList(&req, &res);
	    if (ONVIF_OK == ret)
	    {
		    return soap_build_send_rly(p_user, rx_msg, build_tsc_GetScheduleInfoList_rly_xml, (char*)&res, NULL, p_header);
		}
	}
	
	return soap_build_err_rly(p_user, rx_msg, ret);
}
 
int soap_tsc_GetSchedules(HTTPCLN * p_user, HTTPMSG * rx_msg, XMLN * p_body, XMLN * p_header)
{
    ONVIF_RET ret;
	XMLN * p_GetSchedules;
	tsc_GetSchedules_REQ req;
	
	onvif_print("%s\r\n", __FUNCTION__);

    memset(&req, 0, sizeof(req));

	p_GetSchedules = xml_node_soap_get(p_body, "GetSchedules");
	assert(p_GetSchedules);	

	ret = parse_tsc_GetSchedules(p_GetSchedules, &req);
	if (ONVIF_OK == ret)
	{
	    tsc_GetSchedules_RES res;
	    memset(&res, 0, sizeof(res));
	    
	    ret = onvif_tsc_GetSchedules(&req, &res);
	    if (ONVIF_OK == ret)
	    {
		    return soap_build_send_rly(p_user, rx_msg, build_tsc_GetSchedules_rly_xml, (char*)&res, NULL, p_header);
		}
	}
	
	return soap_build_err_rly(p_user, rx_msg, ret);
}
 
int soap_tsc_GetScheduleList(HTTPCLN * p_user, HTTPMSG * rx_msg, XMLN * p_body, XMLN * p_header)
{
    ONVIF_RET ret;
	XMLN * p_GetScheduleList;
	tsc_GetScheduleList_REQ req;
	
	onvif_print("%s\r\n", __FUNCTION__);

    memset(&req, 0, sizeof(req));

	p_GetScheduleList = xml_node_soap_get(p_body, "GetScheduleList");
	assert(p_GetScheduleList);	

	ret = parse_tsc_GetScheduleList(p_GetScheduleList, &req);
	if (ONVIF_OK == ret)
	{
	    tsc_GetScheduleList_RES res;
	    memset(&res, 0, sizeof(res));
	    
	    ret = onvif_tsc_GetScheduleList(&req, &res);
	    if (ONVIF_OK == ret)
	    {
		    return soap_build_send_rly(p_user, rx_msg, build_tsc_GetScheduleList_rly_xml, (char*)&res, NULL, p_header);
		}
	}
	
	return soap_build_err_rly(p_user, rx_msg, ret);
}
 
int soap_tsc_CreateSchedule(HTTPCLN * p_user, HTTPMSG * rx_msg, XMLN * p_body, XMLN * p_header)
{
    ONVIF_RET ret;
	XMLN * p_CreateSchedule;
	tsc_CreateSchedule_REQ req;
	
	onvif_print("%s\r\n", __FUNCTION__);

    memset(&req, 0, sizeof(req));

	p_CreateSchedule = xml_node_soap_get(p_body, "CreateSchedule");
	assert(p_CreateSchedule);	

	ret = parse_tsc_CreateSchedule(p_CreateSchedule, &req);
	if (ONVIF_OK == ret)
	{
	    tsc_CreateSchedule_RES res;
	    memset(&res, 0, sizeof(res));
	    
	    ret = onvif_tsc_CreateSchedule(&req, &res);
	    if (ONVIF_OK == ret)
	    {
		    return soap_build_send_rly(p_user, rx_msg, build_tsc_CreateSchedule_rly_xml, (char*)&res, NULL, p_header);
		}
	}
	
	return soap_build_err_rly(p_user, rx_msg, ret);
}
 
int soap_tsc_ModifySchedule(HTTPCLN * p_user, HTTPMSG * rx_msg, XMLN * p_body, XMLN * p_header)
{
    ONVIF_RET ret;
	XMLN * p_ModifySchedule;
	tsc_ModifySchedule_REQ req;
	
	onvif_print("%s\r\n", __FUNCTION__);

    memset(&req, 0, sizeof(req));

	p_ModifySchedule = xml_node_soap_get(p_body, "ModifySchedule");
	assert(p_ModifySchedule);

	ret = parse_tsc_ModifySchedule(p_ModifySchedule, &req);
	if (ONVIF_OK == ret)
	{
	    ret = onvif_tsc_ModifySchedule(&req);
	    if (ONVIF_OK == ret)
	    {
		    return soap_build_send_rly(p_user, rx_msg, build_tsc_ModifySchedule_rly_xml, NULL, NULL, p_header);
		}
	}
	
	return soap_build_err_rly(p_user, rx_msg, ret);
}
 
int soap_tsc_DeleteSchedule(HTTPCLN * p_user, HTTPMSG * rx_msg, XMLN * p_body, XMLN * p_header)
{
    ONVIF_RET ret;
	XMLN * p_DeleteSchedule;
	tsc_DeleteSchedule_REQ req;
	
	onvif_print("%s\r\n", __FUNCTION__);

    memset(&req, 0, sizeof(req));

	p_DeleteSchedule = xml_node_soap_get(p_body, "DeleteSchedule");
	assert(p_DeleteSchedule);

	ret = parse_tsc_DeleteSchedule(p_DeleteSchedule, &req);
	if (ONVIF_OK == ret)
	{	    
	    ret = onvif_tsc_DeleteSchedule(&req);
	    if (ONVIF_OK == ret)
	    {
		    return soap_build_send_rly(p_user, rx_msg, build_tsc_DeleteSchedule_rly_xml, NULL, NULL, p_header);
		}
	}
	
	return soap_build_err_rly(p_user, rx_msg, ret);
}
 
int soap_tsc_GetSpecialDayGroupInfo(HTTPCLN * p_user, HTTPMSG * rx_msg, XMLN * p_body, XMLN * p_header)
{
    ONVIF_RET ret;
	XMLN * p_GetSpecialDayGroupInfo;
	tsc_GetSpecialDayGroupInfo_REQ req;
	
	onvif_print("%s\r\n", __FUNCTION__);

    memset(&req, 0, sizeof(req));

	p_GetSpecialDayGroupInfo = xml_node_soap_get(p_body, "GetSpecialDayGroupInfo");
	assert(p_GetSpecialDayGroupInfo);	

	ret = parse_tsc_GetSpecialDayGroupInfo(p_GetSpecialDayGroupInfo, &req);
	if (ONVIF_OK == ret)
	{
	    tsc_GetSpecialDayGroupInfo_RES res;
	    memset(&res, 0, sizeof(res));
	    
	    ret = onvif_tsc_GetSpecialDayGroupInfo(&req, &res);
	    if (ONVIF_OK == ret)
	    {
		    return soap_build_send_rly(p_user, rx_msg, build_tsc_GetSpecialDayGroupInfo_rly_xml, (char*)&res, NULL, p_header);
		}
	}
	
	return soap_build_err_rly(p_user, rx_msg, ret);
}
 
int soap_tsc_GetSpecialDayGroupInfoList(HTTPCLN * p_user, HTTPMSG * rx_msg, XMLN * p_body, XMLN * p_header)
{
    ONVIF_RET ret;
	XMLN * p_GetSpecialDayGroupInfoList;
	tsc_GetSpecialDayGroupInfoList_REQ req;
	
	onvif_print("%s\r\n", __FUNCTION__);

    memset(&req, 0, sizeof(req));

	p_GetSpecialDayGroupInfoList = xml_node_soap_get(p_body, "GetSpecialDayGroupInfoList");
	assert(p_GetSpecialDayGroupInfoList);	

	ret = parse_tsc_GetSpecialDayGroupInfoList(p_GetSpecialDayGroupInfoList, &req);
	if (ONVIF_OK == ret)
	{
	    tsc_GetSpecialDayGroupInfoList_RES res;
	    memset(&res, 0, sizeof(res));
	    
	    ret = onvif_tsc_GetSpecialDayGroupInfoList(&req, &res);
	    if (ONVIF_OK == ret)
	    {
		    return soap_build_send_rly(p_user, rx_msg, build_tsc_GetSpecialDayGroupInfoList_rly_xml, (char*)&res, NULL, p_header);
		}
	}
	
	return soap_build_err_rly(p_user, rx_msg, ret);
}
 
int soap_tsc_GetSpecialDayGroups(HTTPCLN * p_user, HTTPMSG * rx_msg, XMLN * p_body, XMLN * p_header)
{
    ONVIF_RET ret;
	XMLN * p_GetSpecialDayGroups;
	tsc_GetSpecialDayGroups_REQ req;
	
	onvif_print("%s\r\n", __FUNCTION__);

    memset(&req, 0, sizeof(req));

	p_GetSpecialDayGroups = xml_node_soap_get(p_body, "GetSpecialDayGroups");
	assert(p_GetSpecialDayGroups);	

	ret = parse_tsc_GetSpecialDayGroups(p_GetSpecialDayGroups, &req);
	if (ONVIF_OK == ret)
	{
	    tsc_GetSpecialDayGroups_RES res;
	    memset(&res, 0, sizeof(res));
	    
	    ret = onvif_tsc_GetSpecialDayGroups(&req, &res);
	    if (ONVIF_OK == ret)
	    {
		    return soap_build_send_rly(p_user, rx_msg, build_tsc_GetSpecialDayGroups_rly_xml, (char*)&res, NULL, p_header);
		}
	}
	
	return soap_build_err_rly(p_user, rx_msg, ret);
}
 
int soap_tsc_GetSpecialDayGroupList(HTTPCLN * p_user, HTTPMSG * rx_msg, XMLN * p_body, XMLN * p_header)
{
    ONVIF_RET ret;
	XMLN * p_GetSpecialDayGroupList;
	tsc_GetSpecialDayGroupList_REQ req;
	
	onvif_print("%s\r\n", __FUNCTION__);

    memset(&req, 0, sizeof(req));

	p_GetSpecialDayGroupList = xml_node_soap_get(p_body, "GetSpecialDayGroupList");
	assert(p_GetSpecialDayGroupList);	

	ret = parse_tsc_GetSpecialDayGroupList(p_GetSpecialDayGroupList, &req);
	if (ONVIF_OK == ret)
	{
	    tsc_GetSpecialDayGroupList_RES res;
	    memset(&res, 0, sizeof(res));
	    
	    ret = onvif_tsc_GetSpecialDayGroupList(&req, &res);
	    if (ONVIF_OK == ret)
	    {
		    return soap_build_send_rly(p_user, rx_msg, build_tsc_GetSpecialDayGroupList_rly_xml, (char*)&res, NULL, p_header);
		}
	}
	
	return soap_build_err_rly(p_user, rx_msg, ret);
}
 
int soap_tsc_CreateSpecialDayGroup(HTTPCLN * p_user, HTTPMSG * rx_msg, XMLN * p_body, XMLN * p_header)
{
    ONVIF_RET ret;
	XMLN * p_CreateSpecialDayGroup;
	tsc_CreateSpecialDayGroup_REQ req;
	
	onvif_print("%s\r\n", __FUNCTION__);

    memset(&req, 0, sizeof(req));

	p_CreateSpecialDayGroup = xml_node_soap_get(p_body, "CreateSpecialDayGroup");
	assert(p_CreateSpecialDayGroup);	

	ret = parse_tsc_CreateSpecialDayGroup(p_CreateSpecialDayGroup, &req);
	if (ONVIF_OK == ret)
	{
	    tsc_CreateSpecialDayGroup_RES res;
	    memset(&res, 0, sizeof(res));
	    
	    ret = onvif_tsc_CreateSpecialDayGroup(&req, &res);
	    if (ONVIF_OK == ret)
	    {
		    return soap_build_send_rly(p_user, rx_msg, build_tsc_CreateSpecialDayGroup_rly_xml, (char*)&res, NULL, p_header);
		}
	}
	
	return soap_build_err_rly(p_user, rx_msg, ret);
}
 
int soap_tsc_ModifySpecialDayGroup(HTTPCLN * p_user, HTTPMSG * rx_msg, XMLN * p_body, XMLN * p_header)
{
    ONVIF_RET ret;
	XMLN * p_ModifySpecialDayGroup;
	tsc_ModifySpecialDayGroup_REQ req;
	
	onvif_print("%s\r\n", __FUNCTION__);

    memset(&req, 0, sizeof(req));

	p_ModifySpecialDayGroup = xml_node_soap_get(p_body, "ModifySpecialDayGroup");
	assert(p_ModifySpecialDayGroup);	

	ret = parse_tsc_ModifySpecialDayGroup(p_ModifySpecialDayGroup, &req);
	if (ONVIF_OK == ret)
	{	    
	    ret = onvif_tsc_ModifySpecialDayGroup(&req);
	    if (ONVIF_OK == ret)
	    {
		    return soap_build_send_rly(p_user, rx_msg, build_tsc_ModifySpecialDayGroup_rly_xml, NULL, NULL, p_header);
		}
	}
	
	return soap_build_err_rly(p_user, rx_msg, ret);
}
 
int soap_tsc_DeleteSpecialDayGroup(HTTPCLN * p_user, HTTPMSG * rx_msg, XMLN * p_body, XMLN * p_header)
{
    ONVIF_RET ret;
	XMLN * p_DeleteSpecialDayGroup;
	tsc_DeleteSpecialDayGroup_REQ req;
	
	onvif_print("%s\r\n", __FUNCTION__);

    memset(&req, 0, sizeof(req));

	p_DeleteSpecialDayGroup = xml_node_soap_get(p_body, "DeleteSpecialDayGroup");
	assert(p_DeleteSpecialDayGroup);	

	ret = parse_tsc_DeleteSpecialDayGroup(p_DeleteSpecialDayGroup, &req);
	if (ONVIF_OK == ret)
	{	    
	    ret = onvif_tsc_DeleteSpecialDayGroup(&req);
	    if (ONVIF_OK == ret)
	    {
		    return soap_build_send_rly(p_user, rx_msg, build_tsc_DeleteSpecialDayGroup_rly_xml, NULL, NULL, p_header);
		}
	}
	
	return soap_build_err_rly(p_user, rx_msg, ret);
}
 
int soap_tsc_GetScheduleState(HTTPCLN * p_user, HTTPMSG * rx_msg, XMLN * p_body, XMLN * p_header)
{
    ONVIF_RET ret;
	XMLN * p_GetScheduleState;
	tsc_GetScheduleState_REQ req;
	
	onvif_print("%s\r\n", __FUNCTION__);

    memset(&req, 0, sizeof(req));

	p_GetScheduleState = xml_node_soap_get(p_body, "GetScheduleState");
	assert(p_GetScheduleState);	

	ret = parse_tsc_GetScheduleState(p_GetScheduleState, &req);
	if (ONVIF_OK == ret)
	{
	    tsc_GetScheduleState_RES res;
	    memset(&res, 0, sizeof(res));
	    
	    ret = onvif_tsc_GetScheduleState(&req, &res);
	    if (ONVIF_OK == ret)
	    {
		    return soap_build_send_rly(p_user, rx_msg, build_tsc_GetScheduleState_rly_xml, (char*)&res, NULL, p_header);
		}
	}
	
	return soap_build_err_rly(p_user, rx_msg, ret);
} 
#endif // end of SCHEDULE_SUPPORT

#ifdef RECEIVER_SUPPORT

int soap_trv_GetReceivers(HTTPCLN * p_user, HTTPMSG * rx_msg, XMLN * p_body, XMLN * p_header)
{
    ONVIF_RET ret;
    trv_GetReceivers_RES res;

    memset(&res, 0, sizeof(res));
    
    ret = onvif_trv_GetReceivers(&res);
    if (ONVIF_OK == ret)
    {
	    return soap_build_send_rly(p_user, rx_msg, build_trv_GetReceivers_rly_xml, (char*)&res, NULL, p_header);
	}
	
	return soap_build_err_rly(p_user, rx_msg, ret);
}

int soap_trv_GetReceiver(HTTPCLN * p_user, HTTPMSG * rx_msg, XMLN * p_body, XMLN * p_header)
{
    ONVIF_RET ret;
	XMLN * p_GetReceiver;
	trv_GetReceiver_REQ req;
	
	onvif_print("%s\r\n", __FUNCTION__);

    memset(&req, 0, sizeof(req));

	p_GetReceiver = xml_node_soap_get(p_body, "GetReceiver");
	assert(p_GetReceiver);	

	ret = parse_trv_GetReceiver(p_GetReceiver, &req);
	if (ONVIF_OK == ret)
	{
	    trv_GetReceiver_RES res;
	    memset(&res, 0, sizeof(res));
	    
	    ret = onvif_trv_GetReceiver(&req, &res);
	    if (ONVIF_OK == ret)
	    {
		    return soap_build_send_rly(p_user, rx_msg, build_trv_GetReceiver_rly_xml, (char*)&res, NULL, p_header);
		}
	}
	
	return soap_build_err_rly(p_user, rx_msg, ret);
}

int soap_trv_CreateReceiver(HTTPCLN * p_user, HTTPMSG * rx_msg, XMLN * p_body, XMLN * p_header)
{
    ONVIF_RET ret;
	XMLN * p_CreateReceiver;
	trv_CreateReceiver_REQ req;
	
	onvif_print("%s\r\n", __FUNCTION__);

    memset(&req, 0, sizeof(req));

	p_CreateReceiver = xml_node_soap_get(p_body, "CreateReceiver");
	assert(p_CreateReceiver);	

	ret = parse_trv_CreateReceiver(p_CreateReceiver, &req);
	if (ONVIF_OK == ret)
	{
	    trv_CreateReceiver_RES res;
	    memset(&res, 0, sizeof(res));
	    
	    ret = onvif_trv_CreateReceiver(&req, &res);
	    if (ONVIF_OK == ret)
	    {
		    return soap_build_send_rly(p_user, rx_msg, build_trv_CreateReceiver_rly_xml, (char*)&res, NULL, p_header);
		}
	}
	
	return soap_build_err_rly(p_user, rx_msg, ret);
}

int soap_trv_DeleteReceiver(HTTPCLN * p_user, HTTPMSG * rx_msg, XMLN * p_body, XMLN * p_header)
{
    ONVIF_RET ret;
	XMLN * p_DeleteReceiver;
	trv_DeleteReceiver_REQ req;
	
	onvif_print("%s\r\n", __FUNCTION__);

    memset(&req, 0, sizeof(req));

	p_DeleteReceiver = xml_node_soap_get(p_body, "DeleteReceiver");
	assert(p_DeleteReceiver);	

	ret = parse_trv_DeleteReceiver(p_DeleteReceiver, &req);
	if (ONVIF_OK == ret)
	{	    
	    ret = onvif_trv_DeleteReceiver(&req);
	    if (ONVIF_OK == ret)
	    {
		    return soap_build_send_rly(p_user, rx_msg, build_trv_DeleteReceiver_rly_xml, NULL, NULL, p_header);
		}
	}
	
	return soap_build_err_rly(p_user, rx_msg, ret);
}

int soap_trv_ConfigureReceiver(HTTPCLN * p_user, HTTPMSG * rx_msg, XMLN * p_body, XMLN * p_header)
{
    ONVIF_RET ret;
	XMLN * p_ConfigureReceiver;
	trv_ConfigureReceiver_REQ req;
	
	onvif_print("%s\r\n", __FUNCTION__);

    memset(&req, 0, sizeof(req));

	p_ConfigureReceiver = xml_node_soap_get(p_body, "ConfigureReceiver");
	assert(p_ConfigureReceiver);	

	ret = parse_trv_ConfigureReceiver(p_ConfigureReceiver, &req);
	if (ONVIF_OK == ret)
	{	    
	    ret = onvif_trv_ConfigureReceiver(&req);
	    if (ONVIF_OK == ret)
	    {
		    return soap_build_send_rly(p_user, rx_msg, build_trv_ConfigureReceiver_rly_xml, NULL, NULL, p_header);
		}
	}
	
	return soap_build_err_rly(p_user, rx_msg, ret);
}

int soap_trv_SetReceiverMode(HTTPCLN * p_user, HTTPMSG * rx_msg, XMLN * p_body, XMLN * p_header)
{
    ONVIF_RET ret;
	XMLN * p_SetReceiverMode;
	trv_SetReceiverMode_REQ req;
	
	onvif_print("%s\r\n", __FUNCTION__);

    memset(&req, 0, sizeof(req));

	p_SetReceiverMode = xml_node_soap_get(p_body, "SetReceiverMode");
	assert(p_SetReceiverMode);

	ret = parse_trv_SetReceiverMode(p_SetReceiverMode, &req);
	if (ONVIF_OK == ret)
	{	    
	    ret = onvif_trv_SetReceiverMode(&req);
	    if (ONVIF_OK == ret)
	    {
		    return soap_build_send_rly(p_user, rx_msg, build_trv_SetReceiverMode_rly_xml, NULL, NULL, p_header);
		}
	}
	
	return soap_build_err_rly(p_user, rx_msg, ret);
}

int soap_trv_GetReceiverState(HTTPCLN * p_user, HTTPMSG * rx_msg, XMLN * p_body, XMLN * p_header)
{
    ONVIF_RET ret;
	XMLN * p_GetReceiverState;
	trv_GetReceiverState_REQ req;
	
	onvif_print("%s\r\n", __FUNCTION__);

    memset(&req, 0, sizeof(req));

	p_GetReceiverState = xml_node_soap_get(p_body, "GetReceiverState");
	assert(p_GetReceiverState);	

	ret = parse_trv_GetReceiverState(p_GetReceiverState, &req);
	if (ONVIF_OK == ret)
	{
	    trv_GetReceiverState_RES res;
	    memset(&res, 0, sizeof(res));
	    
	    ret = onvif_trv_GetReceiverState(&req, &res);
	    if (ONVIF_OK == ret)
	    {
		    return soap_build_send_rly(p_user, rx_msg, build_trv_GetReceiverState_rly_xml, (char*)&res, NULL, p_header);
		}
	}
	
	return soap_build_err_rly(p_user, rx_msg, ret);
}

#endif // end of RECEIVER_SUPPORT

/*********************************************************/
void soap_calc_digest(const char *created, unsigned char *nonce, int noncelen, const char *password, unsigned char hash[20])
{
	sha1_context ctx;
	
	sha1_starts(&ctx);
	sha1_update(&ctx, (unsigned char *)nonce, noncelen);
	sha1_update(&ctx, (unsigned char *)created, strlen(created));
	sha1_update(&ctx, (unsigned char *)password, strlen(password));
	sha1_finish(&ctx, (unsigned char *)hash);
}

BOOL soap_auth_process(XMLN * p_Security, int oplevel)
{
	int nonce_len;
	XMLN * p_UsernameToken;
	XMLN * p_Username;
	XMLN * p_Password;
	XMLN * p_Nonce;
	XMLN * p_Created;
	char HABase64[100];
	unsigned char nonce[200];
	unsigned char HA[20];
	const char * p_Type;
	onvif_User * p_user;

	p_UsernameToken = xml_node_soap_get(p_Security, "wsse:UsernameToken");
	if (NULL == p_UsernameToken)
	{
		return FALSE;
	}

	p_Username = xml_node_soap_get(p_UsernameToken, "wsse:Username");
	p_Password = xml_node_soap_get(p_UsernameToken, "wsse:Password");
	p_Nonce = xml_node_soap_get(p_UsernameToken, "wsse:Nonce");
	p_Created = xml_node_soap_get(p_UsernameToken, "wsse:Created");

	if (NULL == p_Username || NULL == p_Username->data || 
		NULL == p_Password || NULL == p_Password->data || 
		NULL == p_Nonce || NULL == p_Nonce->data ||
		NULL == p_Created || NULL == p_Created->data)
	{
		return FALSE;
	}

    p_Type = xml_attr_get(p_Password, "Type");
    if (NULL == p_Type)
    {
        return FALSE;
    }

	p_user = onvif_find_user(p_Username->data);
	if (NULL == p_user)	// user not exist
	{
		return FALSE;
	}

	if (p_user->UserLevel > oplevel)
	{
	    return FALSE;
	}
		
	nonce_len = base64_decode(p_Nonce->data, nonce, sizeof(nonce));	
	
	soap_calc_digest(p_Created->data, nonce, nonce_len, p_user->Password, HA);
	base64_encode(HA, 20, HABase64, sizeof(HABase64));

	if (strcmp(HABase64, p_Password->data) == 0)
	{
		return TRUE;
	}
	
	return FALSE;
}

/**
 * if the request not need auth, return TRUE, or FALSE
 */
BOOL soap_IsReqNotNeedAuth(const char * request)
{
#ifdef PROFILE_Q_SUPPORT
    // A device shall provide full anonymous access to all ONVIF commands 
    //  while the device operates in Factory Default State
    
    if (0 == g_onvif_cfg.device_state)
    {
        return TRUE;
    }
#endif

    if (soap_strcmp(request, "GetCapabilities") == 0)
    {
        return TRUE;
    }
    else if (soap_strcmp(request, "GetServices") == 0)
    {
        return TRUE;
    }
    else if (soap_strcmp(request, "GetServiceCapabilities") == 0)
    {
        return TRUE;
    }
    else if (soap_strcmp(request, "GetSystemDateAndTime") == 0)
    {
        return TRUE;
    }
    else if (soap_strcmp(request, "GetEndpointReference") == 0)
    {
        return TRUE;
    }
    else if (soap_strcmp(request, "GetWsdlUrl") == 0)
    {
        return TRUE;
    }
    else if (soap_strcmp(request, "GetHostname") == 0)
    {
        return TRUE;
    }
    
    return FALSE;
}

onvif_UserLevel soap_getUserOpLevel(const char * request)
{
    onvif_UserLevel level = UserLevel_User;
    
    if (soap_strcmp(request, "SetScopes") == 0)
    {
        level = UserLevel_Administrator;
    }
    else if (soap_strcmp(request, "SetDiscoveryMode") == 0)
    {
        level = UserLevel_Administrator;
    }
    else if (soap_strcmp(request, "GetAccessPolicy") == 0)
    {
        level = UserLevel_Administrator;
    }
    else if (soap_strcmp(request, "CreateUsers") == 0)
    {
        level = UserLevel_Administrator;
    }
    else if (soap_strcmp(request, "SetSystemDateAndTime") == 0)
    {
        level = UserLevel_Administrator;
    }

    return level;
}

BOOL soap_matchNamespace(XMLN * p_node, const char * ns, const char * prefix)
{
    XMLN * p_attr;
    
    p_attr = p_node->f_attrib;
	while (p_attr != NULL)
	{
		if (NTYPE_ATTRIB == p_attr->type)
		{
		    if (NULL == prefix)
		    {
    	        if (strcasecmp(p_attr->data, ns) == 0)
    	        {
    		        return TRUE;
    		    }
		    }
		    else
		    {
		        const char * ptr1;

		        ptr1 = strchr(p_attr->name, ':');
		        if (ptr1)
		        {
		            if (strcasecmp(ptr1+1, prefix) == 0 && 
		                strcasecmp(p_attr->data, ns) == 0)
		            {
		                return TRUE;
		            }
		        }
		    }
        }
        
		p_attr = p_attr->next;
	}

	return FALSE;
}

BOOL soap_isDeviceServiceNamespace(XMLN * p_body)
{
    const char * ptr1;
    XMLN * p_name = p_body->f_child;
    char prefix[32] = {'\0'};

    ptr1 = strchr(p_name->name, ':');
	if (ptr1)
	{
	    memcpy(prefix, p_name->name, ptr1 - p_name->name);
	}

    if (prefix[0] == '\0')
    {
        return soap_matchNamespace(p_name, "http://www.onvif.org/ver10/device/wsdl", NULL);        
	}
    else
    {
        while (p_name)
        {
            if (soap_matchNamespace(p_name, "http://www.onvif.org/ver10/device/wsdl", prefix))
            {
                return TRUE;
            }

            p_name = p_name->parent;
        }
    }

	return FALSE;
}


/*********************************************************
 *
 * process soap request
 *
 * p_user [in] 	 --- http client
 * rx_msg [in] --- http message
 *
**********************************************************/ 
void soap_process_request(HTTPCLN * p_user, HTTPMSG * rx_msg)
{
    int errcode = 401;
	BOOL auth = FALSE;	
	char * p_xml;
	XMLN * p_node;
	XMLN * p_header;
	XMLN * p_body;
    char * p_post;
    const char * p_name;
    onvif_UserLevel oplevel = UserLevel_Anonymous;
    
	p_xml = http_get_ctt(rx_msg);
	if (NULL == p_xml)
	{
		log_print(LOG_ERR, "%s, http_get_ctt ret null!!!\r\n", __FUNCTION__);
		return;
	}


	if (logEnable == 1){
		printf("\r\nsoap_process::rx xml:\r\n%s\r\n", p_xml);		// Print the XML file sent by the client
	}

	p_node = xxx_hxml_parse(p_xml, strlen(p_xml));
	if (NULL == p_node || NULL == p_node->name)
	{
		log_print(LOG_ERR, "%s, xxx_hxml_parse ret null!!!\r\n", __FUNCTION__);
		return;
	}
	
	if (soap_strcmp(p_node->name, "Envelope") != 0)
	{
		log_print(LOG_ERR, "%s, node name[%s] != [Envelope]!!!\r\n", __FUNCTION__, p_node->name);
		xml_node_del(p_node);
		return;
	}

    p_body = xml_node_soap_get(p_node, "Body");
	if (NULL == p_body)
	{
		log_print(LOG_ERR, "%s, xml_node_soap_get[Body] ret null!!!\r\n", __FUNCTION__);
		xml_node_del(p_node);
		return;
	}

	if (NULL == p_body->f_child)
	{
		log_print(LOG_ERR, "%s, body first child node is null!!!\r\n", __FUNCTION__);
		xml_node_del(p_node);
		return;
	}	
	else if (NULL == p_body->f_child->name)
	{
		log_print(LOG_ERR, "%s, body first child node name is null!!!\r\n", __FUNCTION__);
		xml_node_del(p_node);
		return;
	}	
	else
	{
		log_print(LOG_INFO, "%s, body first child node name[%s]\r\n", __FUNCTION__, p_body->f_child->name);
	}

	auth = soap_IsReqNotNeedAuth(p_body->f_child->name);

    oplevel = soap_getUserOpLevel(p_body->f_child->name);
    
 /* begin auth processing */	
	p_header = xml_node_soap_get(p_node, "Header");
	if (p_header && g_onvif_cfg.need_auth && !auth)
	{
		XMLN * p_Security = xml_node_soap_get(p_header, "Security");
		if (p_Security)
		{
		    errcode = 400;
			auth = soap_auth_process(p_Security, oplevel);
		}
	}

	if (g_onvif_cfg.need_auth && !auth)
	{
	    HD_AUTH_INFO auth_info;

	    // check http digest auth information
	    if (http_get_auth_digest_info(rx_msg, &auth_info))
	    {
	        auth = DigestAuthProcess(&auth_info, &g_onvif_auth, "POST", oplevel);
	    }

	    if (auth == FALSE)
	    {
    		soap_security_rly(p_user, rx_msg, errcode);
    		xml_node_del(p_node);
    		return;
		}
	}
 /* end of auth processing */

    p_post = rx_msg->first_line.value_string;
    p_name = p_body->f_child->name;
    
	if (soap_strcmp(p_name, "GetDeviceInformation") == 0)
	{
		soap_GetDeviceInformation(p_user, rx_msg, p_body, p_header);
	}
	else if (soap_strcmp(p_name, "GetSystemUris") == 0)
	{
		soap_GetSystemUris(p_user, rx_msg, p_body, p_header);
	}
	else if (soap_strcmp(p_name, "GetCapabilities") == 0)
	{
        soap_GetCapabilities(p_user, rx_msg, p_body, p_header);
	}
	else if (soap_strcmp(p_name, "GetSystemDateAndTime") == 0)
	{
		soap_GetSystemDateAndTime(p_user, rx_msg, p_body, p_header);
	}
	else if (soap_strcmp(p_name, "SetSystemDateAndTime") == 0)
	{
        soap_SetSystemDateAndTime(p_user, rx_msg, p_body, p_header);
	}
	else if (soap_strcmp(p_name, "GetNetworkInterfaces") == 0)
	{	
		soap_GetNetworkInterfaces(p_user, rx_msg, p_body, p_header);
	}
	else if (soap_strcmp(p_name, "SetNetworkInterfaces") == 0)
	{
		soap_SetNetworkInterfaces(p_user, rx_msg, p_body, p_header);
	}
	else if (soap_strcmp(p_name, "SystemReboot") == 0)
	{
		soap_SystemReboot(p_user, rx_msg, p_body, p_header);
	}
	else if (soap_strcmp(p_name, "SetSystemFactoryDefault") == 0)
	{
		soap_SetSystemFactoryDefault(p_user, rx_msg, p_body, p_header);
	}
	else if (soap_strcmp(p_name, "GetSystemLog") == 0)
	{
		soap_GetSystemLog(p_user, rx_msg, p_body, p_header);
	}	
	else if (soap_strcmp(p_name, "GetServices") == 0)
	{
		soap_GetServices(p_user, rx_msg, p_body, p_header);
	}
	else if (soap_strcmp(p_name, "GetScopes") == 0)
	{
		soap_GetScopes(p_user, rx_msg, p_body, p_header);
	}
	else if (soap_strcmp(p_name, "AddScopes") == 0)
	{
		soap_AddScopes(p_user, rx_msg, p_body, p_header);
	}
	else if (soap_strcmp(p_name, "SetScopes") == 0)
	{
		soap_SetScopes(p_user, rx_msg, p_body, p_header);
	}
	else if (soap_strcmp(p_name, "RemoveScopes") == 0)
	{
		soap_RemoveScopes(p_user, rx_msg, p_body, p_header);
	}	
	else if (soap_strcmp(p_name, "GetHostname") == 0)
	{
		soap_GetHostname(p_user, rx_msg, p_body, p_header);
	}
	else if (soap_strcmp(p_name, "SetHostname") == 0)
	{
		soap_SetHostname(p_user, rx_msg, p_body, p_header);
	}
	else if (soap_strcmp(p_name, "SetHostnameFromDHCP") == 0)
	{
		soap_SetHostnameFromDHCP(p_user, rx_msg, p_body, p_header);
	}
	else if (soap_strcmp(p_name, "GetNetworkProtocols") == 0)
	{
		soap_GetNetworkProtocols(p_user, rx_msg, p_body, p_header);
	}
	else if (soap_strcmp(p_name, "SetNetworkProtocols") == 0)
	{
		soap_SetNetworkProtocols(p_user, rx_msg, p_body, p_header);
	}
	else if (soap_strcmp(p_name, "GetNetworkDefaultGateway") == 0)
	{
		soap_GetNetworkDefaultGateway(p_user, rx_msg, p_body, p_header);
	}
	else if (soap_strcmp(p_name, "SetNetworkDefaultGateway") == 0)
	{
		soap_SetNetworkDefaultGateway(p_user, rx_msg, p_body, p_header);
	}
	else if (soap_strcmp(p_name, "GetDiscoveryMode") == 0)
	{
		soap_GetDiscoveryMode(p_user, rx_msg, p_body, p_header);
	}
	else if (soap_strcmp(p_name, "SetDiscoveryMode") == 0)
	{
		soap_SetDiscoveryMode(p_user, rx_msg, p_body, p_header);
	}
	else if (soap_strcmp(p_name, "GetDNS") == 0)
	{
		soap_GetDNS(p_user, rx_msg, p_body, p_header);
	}
	else if (soap_strcmp(p_name, "SetDNS") == 0)
	{
		soap_SetDNS(p_user, rx_msg, p_body, p_header);
	}
	else if (soap_strcmp(p_name, "GetNTP") == 0)
	{
		soap_GetNTP(p_user, rx_msg, p_body, p_header);
	}
	else if (soap_strcmp(p_name, "SetNTP") == 0)
	{
		soap_SetNTP(p_user, rx_msg, p_body, p_header);
	}
	else if (soap_strcmp(p_name, "GetZeroConfiguration") == 0)
	{
		soap_GetZeroConfiguration(p_user, rx_msg, p_body, p_header);
	}
	else if (soap_strcmp(p_name, "SetZeroConfiguration") == 0)
	{
		soap_SetZeroConfiguration(p_user, rx_msg, p_body, p_header);
	}
	else if (soap_strcmp(p_name, "GetDot11Capabilities") == 0)
	{
		soap_GetDot11Capabilities(p_user, rx_msg, p_body, p_header);
	}
	else if (soap_strcmp(p_name, "GetDot11Status") == 0)
	{
		soap_GetDot11Status(p_user, rx_msg, p_body, p_header);
	}
	else if (soap_strcmp(p_name, "ScanAvailableDot11Networks") == 0)
	{
		soap_ScanAvailableDot11Networks(p_user, rx_msg, p_body, p_header);
	}
	else if (soap_strcmp(p_name, "GetServiceCapabilities") == 0)
	{			
		soap_GetServiceCapabilities(p_user, rx_msg, p_body, p_header);
	}
	else if (soap_strcmp(p_name, "GetWsdlUrl") == 0)
	{
		soap_GetWsdlUrl(p_user, rx_msg, p_body, p_header);
	}
	else if (soap_strcmp(p_name, "GetEndpointReference") == 0)
	{
	    soap_GetEndpointReference(p_user, rx_msg, p_body, p_header);
	}
	else if (soap_strcmp(p_name, "GetUsers") == 0)
	{
		soap_GetUsers(p_user, rx_msg, p_body, p_header);
	}
	else if (soap_strcmp(p_name, "CreateUsers") == 0)
	{
		soap_CreateUsers(p_user, rx_msg, p_body, p_header);
	}
	else if (soap_strcmp(p_name, "DeleteUsers") == 0)
	{
		soap_DeleteUsers(p_user, rx_msg, p_body, p_header);
	}
	else if (soap_strcmp(p_name, "SetUser") == 0)
	{
		soap_SetUser(p_user, rx_msg, p_body, p_header);
	}
	else if (soap_strcmp(p_name, "GetRemoteUser") == 0)
	{
		soap_GetRemoteUser(p_user, rx_msg, p_body, p_header);
	}
	else if (soap_strcmp(p_name, "SetRemoteUser") == 0)
	{
		soap_SetRemoteUser(p_user, rx_msg, p_body, p_header);
	}
	else if (soap_strcmp(p_name, "UpgradeSystemFirmware") == 0)
	{
		soap_UpgradeSystemFirmware(p_user, rx_msg, p_body, p_header);
	}
	else if (soap_strcmp(p_name, "StartFirmwareUpgrade") == 0)
	{
		soap_StartFirmwareUpgrade(p_user, rx_msg, p_body, p_header);
	}
	else if (soap_strcmp(p_name, "StartSystemRestore") == 0)
	{
		soap_StartSystemRestore(p_user, rx_msg, p_body, p_header);
	}
#ifdef IPFILTER_SUPPORT	
	else if (soap_strcmp(p_name, "GetIPAddressFilter") == 0)
	{
	    soap_GetIPAddressFilter(p_user, rx_msg, p_body, p_header);
	}	
	else if (soap_strcmp(p_name, "SetIPAddressFilter") == 0)
	{
	    soap_SetIPAddressFilter(p_user, rx_msg, p_body, p_header);
	}
	else if (soap_strcmp(p_name, "AddIPAddressFilter") == 0)
	{
	    soap_AddIPAddressFilter(p_user, rx_msg, p_body, p_header);
	}
	else if (soap_strcmp(p_name, "RemoveIPAddressFilter") == 0)
	{
	    soap_RemoveIPAddressFilter(p_user, rx_msg, p_body, p_header);
	}
#endif	
	else if (soap_strcmp(p_name, "GetProfiles") == 0)
	{
#ifdef MEDIA2_SUPPORT
        if (strstr(p_post, "media2"))
        {
            soap_tr2_GetProfiles(p_user, rx_msg, p_body, p_header);
        }
        else
#endif
		soap_GetProfiles(p_user, rx_msg, p_body, p_header);
	}
	else if (soap_strcmp(p_name, "GetProfile") == 0)
	{
		soap_GetProfile(p_user, rx_msg, p_body, p_header);
	}
	else if (soap_strcmp(p_name, "CreateProfile") == 0)
	{
#ifdef MEDIA2_SUPPORT
        if (strstr(p_post, "media2"))
        {
            soap_tr2_CreateProfile(p_user, rx_msg, p_body, p_header);
        }
        else
#endif
		soap_CreateProfile(p_user, rx_msg, p_body, p_header);
	}
	else if (soap_strcmp(p_name, "DeleteProfile") == 0)
	{
#ifdef MEDIA2_SUPPORT
        if (strstr(p_post, "media2"))
        {
            soap_tr2_DeleteProfile(p_user, rx_msg, p_body, p_header);
        }
        else
#endif	
		soap_DeleteProfile(p_user, rx_msg, p_body, p_header);
	}
	else if (soap_strcmp(p_name, "GetVideoSourceModes") == 0)
	{
#ifdef MEDIA2_SUPPORT
        if (strstr(p_post, "media2"))
        {
            soap_tr2_GetVideoSourceModes(p_user, rx_msg, p_body, p_header);
        }
        else
#endif	
		soap_GetVideoSourceModes(p_user, rx_msg, p_body, p_header);
	}
	else if (soap_strcmp(p_name, "SetVideoSourceMode") == 0)
	{
#ifdef MEDIA2_SUPPORT
        if (strstr(p_post, "media2"))
        {
            soap_tr2_SetVideoSourceMode(p_user, rx_msg, p_body, p_header);
        }
        else
#endif	
		soap_SetVideoSourceMode(p_user, rx_msg, p_body, p_header);
	}
	else if (soap_strcmp(p_name, "AddVideoSourceConfiguration") == 0)
	{
		soap_AddVideoSourceConfiguration(p_user, rx_msg, p_body, p_header);
	}
	else if (soap_strcmp(p_name, "RemoveVideoSourceConfiguration") == 0)
	{
		soap_RemoveVideoSourceConfiguration(p_user, rx_msg, p_body, p_header);
	}			
	else if (soap_strcmp(p_name, "AddVideoEncoderConfiguration") == 0)
	{
		soap_AddVideoEncoderConfiguration(p_user, rx_msg, p_body, p_header);
	}
	else if (soap_strcmp(p_name, "RemoveVideoEncoderConfiguration") == 0)
	{
		soap_RemoveVideoEncoderConfiguration(p_user, rx_msg, p_body, p_header);
	}
	else if (soap_strcmp(p_name, "GetStreamUri") == 0)
	{
#ifdef MEDIA2_SUPPORT
        if (strstr(p_post, "media2"))
        {
            soap_tr2_GetStreamUri(p_user, rx_msg, p_body, p_header);
        }
        else
#endif		
		soap_GetStreamUri(p_user, rx_msg, p_body, p_header);
	}	
	else if (soap_strcmp(p_name, "GetVideoSources") == 0)
	{
	    soap_GetVideoSources(p_user, rx_msg, p_body, p_header);
	}	
	else if (soap_strcmp(p_name, "GetVideoEncoderConfigurations") == 0)
	{
#ifdef MEDIA2_SUPPORT
        if (strstr(p_post, "media2"))
        {
            soap_tr2_GetVideoEncoderConfigurations(p_user, rx_msg, p_body, p_header);
        }
        else
#endif	
		soap_GetVideoEncoderConfigurations(p_user, rx_msg, p_body, p_header);
	}
	else if (soap_strcmp(p_name, "GetCompatibleVideoEncoderConfigurations") == 0)
	{
		soap_GetCompatibleVideoEncoderConfigurations(p_user, rx_msg, p_body, p_header);
	}	
	else if (soap_strcmp(p_name, "GetVideoSourceConfigurations") == 0)
	{
#ifdef MEDIA2_SUPPORT
        if (strstr(p_post, "media2"))
        {
            soap_tr2_GetVideoSourceConfigurations(p_user, rx_msg, p_body, p_header);
        }
        else
#endif	
		soap_GetVideoSourceConfigurations(p_user, rx_msg, p_body, p_header);
	}
	else if (soap_strcmp(p_name, "GetVideoSourceConfiguration") == 0)
	{
		soap_GetVideoSourceConfiguration(p_user, rx_msg, p_body, p_header);
	}
	else if (soap_strcmp(p_name, "GetVideoSourceConfigurationOptions") == 0)
	{
#ifdef MEDIA2_SUPPORT
        if (strstr(p_post, "media2"))
        {
            soap_tr2_GetVideoSourceConfigurationOptions(p_user, rx_msg, p_body, p_header);
        }
        else
#endif	
		soap_GetVideoSourceConfigurationOptions(p_user, rx_msg, p_body, p_header);
	}	
	else if (soap_strcmp(p_name, "SetVideoSourceConfiguration") == 0)
	{
#ifdef MEDIA2_SUPPORT
        if (strstr(p_post, "media2"))
        {
            soap_tr2_SetVideoSourceConfiguration(p_user, rx_msg, p_body, p_header);
        }
        else
#endif	
		soap_SetVideoSourceConfiguration(p_user, rx_msg, p_body, p_header);
	}	
	else if (soap_strcmp(p_name, "GetVideoEncoderConfiguration") == 0)
	{
	    soap_GetVideoEncoderConfiguration(p_user, rx_msg, p_body, p_header);	    	
	}
	else if (soap_strcmp(p_name, "SetVideoEncoderConfiguration") == 0)
	{
#ifdef MEDIA2_SUPPORT
        if (strstr(p_post, "media2"))
        {
            soap_tr2_SetVideoEncoderConfiguration(p_user, rx_msg, p_body, p_header);
        }
        else
#endif
        soap_SetVideoEncoderConfiguration(p_user, rx_msg, p_body, p_header);
	}	
	else if (soap_strcmp(p_name, "GetVideoEncoderConfigurationOptions") == 0)
	{
#ifdef MEDIA2_SUPPORT
        if (strstr(p_post, "media2"))
        {
            soap_tr2_GetVideoEncoderConfigurationOptions(p_user, rx_msg, p_body, p_header);
        }
        else
#endif	
		soap_GetVideoEncoderConfigurationOptions(p_user, rx_msg, p_body, p_header);
	}	
	else if (soap_strcmp(p_name, "GetCompatibleVideoSourceConfigurations") == 0)
	{
		soap_GetCompatibleVideoSourceConfigurations(p_user, rx_msg, p_body, p_header);
	}	
	else if (soap_strcmp(p_name, "GetSnapshotUri") == 0)
	{
#ifdef MEDIA2_SUPPORT
        if (strstr(p_post, "media2"))
        {
            soap_tr2_GetSnapshotUri(p_user, rx_msg, p_body, p_header);
        }
        else
#endif	
		soap_GetSnapshotUri(p_user, rx_msg, p_body, p_header);
	}		
	else if (soap_strcmp(p_name, "GetEventProperties") == 0)
	{
		soap_GetEventProperties(p_user, rx_msg, p_body, p_header);
	}
	else if (soap_strcmp(p_name, "Subscribe") == 0)
	{
	    soap_Subscribe(p_user, rx_msg, p_body, p_header);
	}
	else if (soap_strcmp(p_name, "Unsubscribe") == 0)
	{
		soap_Unsubscribe(p_user, rx_msg, p_body, p_header);
	}
	else if (soap_strcmp(p_name, "Renew") == 0)
	{
		soap_Renew(p_user, rx_msg, p_body, p_header);
	}
	else if (soap_strcmp(p_name, "CreatePullPointSubscription") == 0)
	{
		soap_CreatePullPointSubscription(p_user, rx_msg, p_body, p_header);
	}
	else if (soap_strcmp(p_name, "PullMessages") == 0)
	{
		soap_PullMessages(p_user, rx_msg, p_body, p_header);
	}
	else if (soap_strcmp(p_name, "SetSynchronizationPoint") == 0)
	{
#ifdef MEDIA2_SUPPORT
        if (strstr(p_post, "media2"))
        {
            soap_tr2_SetSynchronizationPoint(p_user, rx_msg, p_body, p_header);
        }
        else
#endif	
        if (strstr(p_post, "media"))
        {
            soap_SetSynchronizationPoint(p_user, rx_msg, p_body, p_header);
        } 
        else 
        {
		    soap_tev_SetSynchronizationPoint(p_user, rx_msg, p_body, p_header);
		}
	}	
	else if (soap_strcmp(p_name, "GetGuaranteedNumberOfVideoEncoderInstances") == 0)
	{
		soap_GetGuaranteedNumberOfVideoEncoderInstances(p_user, rx_msg, p_body, p_header);		
	}
	else if (soap_strcmp(p_name, "GetImagingSettings") == 0)
	{
		soap_GetImagingSettings(p_user, rx_msg, p_body, p_header);
	}
	else if (soap_strcmp(p_name, "SetImagingSettings") == 0)
	{
		soap_SetImagingSettings(p_user, rx_msg, p_body, p_header);
	}
	else if (soap_strcmp(p_name, "GetOptions") == 0)
	{
		soap_GetOptions(p_user, rx_msg, p_body, p_header);
	}
	else if (soap_strcmp(p_name, "GetMoveOptions") == 0)
	{
		soap_GetMoveOptions(p_user, rx_msg, p_body, p_header);
	}
	else if (soap_strcmp(p_name, "Move") == 0)
	{
		soap_Move(p_user, rx_msg, p_body, p_header);
	}	
	else if (soap_strcmp(p_name, "GetOSDs") == 0) 
	{
#ifdef MEDIA2_SUPPORT
        if (strstr(p_post, "media2"))
        {
            soap_tr2_GetOSDs(p_user, rx_msg, p_body, p_header);
        }
        else
#endif	
		soap_GetOSDs(p_user, rx_msg, p_body, p_header);
	}
	else if (soap_strcmp(p_name, "GetOSD") == 0) 
	{
		soap_GetOSD(p_user, rx_msg, p_body, p_header);
	}
	else if (soap_strcmp(p_name, "SetOSD") == 0) 
	{
#ifdef MEDIA2_SUPPORT
        if (strstr(p_post, "media2"))
        {
            soap_tr2_SetOSD(p_user, rx_msg, p_body, p_header);
        }
        else
#endif	
		soap_SetOSD(p_user, rx_msg, p_body, p_header);
	}
	else if (soap_strcmp(p_name, "GetOSDOptions") == 0) 
	{
#ifdef MEDIA2_SUPPORT
        if (strstr(p_post, "media2"))
        {
            soap_tr2_GetOSDOptions(p_user, rx_msg, p_body, p_header);
        }
        else
#endif	
		soap_GetOSDOptions(p_user, rx_msg, p_body, p_header);
	}
	else if (soap_strcmp(p_name, "CreateOSD") == 0) 
	{
#ifdef MEDIA2_SUPPORT
        if (strstr(p_post, "media2"))
        {
            soap_tr2_CreateOSD(p_user, rx_msg, p_body, p_header);
        }
        else
#endif	
		soap_CreateOSD(p_user, rx_msg, p_body, p_header);
	}
	else if (soap_strcmp(p_name, "DeleteOSD") == 0) 
	{
#ifdef MEDIA2_SUPPORT
        if (strstr(p_post, "media2"))
        {
            soap_tr2_DeleteOSD(p_user, rx_msg, p_body, p_header);
        }
        else
#endif	
		soap_DeleteOSD(p_user, rx_msg, p_body, p_header);
	}	
	else if (soap_strcmp(p_name, "StartMulticastStreaming") == 0)
	{
		soap_StartMulticastStreaming(p_user, rx_msg, p_body, p_header);
	}
	else if (soap_strcmp(p_name, "StopMulticastStreaming") == 0)
	{
		soap_StopMulticastStreaming(p_user, rx_msg, p_body, p_header);
	}
	else if (soap_strcmp(p_name, "GetMetadataConfigurations") == 0)
	{
#ifdef MEDIA2_SUPPORT
        if (strstr(p_post, "media2"))
        {
            soap_tr2_GetMetadataConfigurations(p_user, rx_msg, p_body, p_header);
        }
        else
#endif	
		soap_GetMetadataConfigurations(p_user, rx_msg, p_body, p_header);
	}
	else if (soap_strcmp(p_name, "GetMetadataConfiguration") == 0)
	{
		soap_GetMetadataConfiguration(p_user, rx_msg, p_body, p_header);
	}
	else if (soap_strcmp(p_name, "GetCompatibleMetadataConfigurations") == 0)
	{
		soap_GetCompatibleMetadataConfigurations(p_user, rx_msg, p_body, p_header);
	}
	else if (soap_strcmp(p_name, "GetMetadataConfigurationOptions") == 0)
	{
#ifdef MEDIA2_SUPPORT
        if (strstr(p_post, "media2"))
        {
            soap_tr2_GetMetadataConfigurationOptions(p_user, rx_msg, p_body, p_header);
        }
        else
#endif	
		soap_GetMetadataConfigurationOptions(p_user, rx_msg, p_body, p_header);
	}	 
	else if (soap_strcmp(p_name, "SetMetadataConfiguration") == 0)
	{
#ifdef MEDIA2_SUPPORT
        if (strstr(p_post, "media2"))
        {
            soap_tr2_SetMetadataConfiguration(p_user, rx_msg, p_body, p_header);
        }
        else
#endif		
		soap_SetMetadataConfiguration(p_user, rx_msg, p_body, p_header);
	}	
	else if (soap_strcmp(p_name, "AddMetadataConfiguration") == 0)
	{
		soap_AddMetadataConfiguration(p_user, rx_msg, p_body, p_header);
	}
	else if (soap_strcmp(p_name, "RemoveMetadataConfiguration") == 0)
	{
		soap_RemoveMetadataConfiguration(p_user, rx_msg, p_body, p_header);
	}
    else if (soap_strcmp(p_name, "GetStatus") == 0)
	{
		soap_GetStatus(p_user, rx_msg, p_body, p_header);
	}
	else if (soap_strcmp(p_name, "Stop") == 0)
	{
		soap_Stop(p_user, rx_msg, p_body, p_header);
	}

#ifdef AUDIO_SUPPORT
    else if (soap_strcmp(p_name, "AddAudioSourceConfiguration") == 0)
	{
		soap_AddAudioSourceConfiguration(p_user, rx_msg, p_body, p_header);
	}
	else if (soap_strcmp(p_name, "RemoveAudioSourceConfiguration") == 0)
	{
		soap_RemoveAudioSourceConfiguration(p_user, rx_msg, p_body, p_header);
	}
	else if (soap_strcmp(p_name, "AddAudioEncoderConfiguration") == 0)
	{
		soap_AddAudioEncoderConfiguration(p_user, rx_msg, p_body, p_header);
	}
	else if (soap_strcmp(p_name, "RemoveAudioEncoderConfiguration") == 0)
	{
		soap_RemoveAudioEncoderConfiguration(p_user, rx_msg, p_body, p_header);
	}
	else if (soap_strcmp(p_name, "GetAudioSources") == 0)
	{
#ifdef DEVICEIO_SUPPORT
        if (strstr(p_post, "deviceIO"))
	    {
	        soap_tmd_GetAudioSources(p_user, rx_msg, p_body, p_header);
	    }else
#endif
	    soap_GetAudioSources(p_user, rx_msg, p_body, p_header);
	}
	else if (soap_strcmp(p_name, "GetAudioEncoderConfigurations") == 0)
	{
#ifdef MEDIA2_SUPPORT
        if (strstr(p_post, "media2"))
        {
            soap_tr2_GetAudioEncoderConfigurations(p_user, rx_msg, p_body, p_header);
        }
        else
#endif	
		soap_GetAudioEncoderConfigurations(p_user, rx_msg, p_body, p_header);
	}
	else if (soap_strcmp(p_name, "GetCompatibleAudioEncoderConfigurations") == 0)
	{
		soap_GetCompatibleAudioEncoderConfigurations(p_user, rx_msg, p_body, p_header);
	}
	else if (soap_strcmp(p_name, "GetAudioSourceConfigurations") == 0)
	{
#ifdef MEDIA2_SUPPORT
        if (strstr(p_post, "media2"))
        {
            soap_tr2_GetAudioSourceConfigurations(p_user, rx_msg, p_body, p_header);
        }
        else
#endif
		soap_GetAudioSourceConfigurations(p_user, rx_msg, p_body, p_header);
	}
	else if (soap_strcmp(p_name, "GetCompatibleAudioSourceConfigurations") == 0)
	{
		soap_GetCompatibleAudioSourceConfigurations(p_user, rx_msg, p_body, p_header);
	}
	else if (soap_strcmp(p_name, "GetAudioSourceConfigurationOptions") == 0)
	{
#ifdef MEDIA2_SUPPORT
        if (strstr(p_post, "media2"))
        {
            soap_tr2_GetAudioSourceConfigurationOptions(p_user, rx_msg, p_body, p_header);
        }
        else
#endif		
		soap_GetAudioSourceConfigurationOptions(p_user, rx_msg, p_body, p_header);
	}	
	else if (soap_strcmp(p_name, "GetAudioSourceConfiguration") == 0)
	{
		soap_GetAudioSourceConfiguration(p_user, rx_msg, p_body, p_header);
	}
	else if (soap_strcmp(p_name, "SetAudioSourceConfiguration") == 0)
	{
#ifdef MEDIA2_SUPPORT
        if (strstr(p_post, "media2"))
        {
            soap_tr2_SetAudioSourceConfiguration(p_user, rx_msg, p_body, p_header);
        }
        else
#endif		
		soap_SetAudioSourceConfiguration(p_user, rx_msg, p_body, p_header);
	}	
	else if (soap_strcmp(p_name, "GetAudioEncoderConfiguration") == 0)
	{
		soap_GetAudioEncoderConfiguration(p_user, rx_msg, p_body, p_header);
	}	
	else if (soap_strcmp(p_name, "SetAudioEncoderConfiguration") == 0)
	{
#ifdef MEDIA2_SUPPORT
        if (strstr(p_post, "media2"))
        {
            soap_tr2_SetAudioEncoderConfiguration(p_user, rx_msg, p_body, p_header);
        }
        else
#endif	
		soap_SetAudioEncoderConfiguration(p_user, rx_msg, p_body, p_header);
	}
	else if (soap_strcmp(p_name, "GetAudioEncoderConfigurationOptions") == 0)
	{
#ifdef MEDIA2_SUPPORT
        if (strstr(p_post, "media2"))
        {
            soap_tr2_GetAudioEncoderConfigurationOptions(p_user, rx_msg, p_body, p_header);
        }
        else
#endif	
		soap_GetAudioEncoderConfigurationOptions(p_user, rx_msg, p_body, p_header);
	}
	else if (soap_strcmp(p_name, "AddAudioDecoderConfiguration") == 0)
	{
	    soap_AddAudioDecoderConfiguration(p_user, rx_msg, p_body, p_header);
	}
    else if (soap_strcmp(p_name, "GetAudioDecoderConfigurations") == 0)
	{
#ifdef MEDIA2_SUPPORT	    
	    if (strstr(p_post, "media2"))
	    {
	        soap_tr2_GetAudioDecoderConfigurations(p_user, rx_msg, p_body, p_header);
	    }else 
#endif		
	    soap_GetAudioDecoderConfigurations(p_user, rx_msg, p_body, p_header);
	}
	else if (soap_strcmp(p_name, "GetAudioDecoderConfiguration") == 0)
	{
	    soap_GetAudioDecoderConfiguration(p_user, rx_msg, p_body, p_header);
	}
    else if (soap_strcmp(p_name, "RemoveAudioDecoderConfiguration") == 0)
	{
	    soap_RemoveAudioDecoderConfiguration(p_user, rx_msg, p_body, p_header);
	}
	else if (soap_strcmp(p_name, "SetAudioDecoderConfiguration") == 0)
	{
#ifdef MEDIA2_SUPPORT	    
	    if (strstr(p_post, "media2"))
	    {
	        soap_tr2_SetAudioDecoderConfiguration(p_user, rx_msg, p_body, p_header);
	    }else 
#endif	
	    soap_SetAudioDecoderConfiguration(p_user, rx_msg, p_body, p_header);
	}
	else if (soap_strcmp(p_name, "GetAudioDecoderConfigurationOptions") == 0)
	{
#ifdef MEDIA2_SUPPORT	    
	    if (strstr(p_post, "media2"))
	    {
	        soap_tr2_GetAudioDecoderConfigurationOptions(p_user, rx_msg, p_body, p_header);
	    }else 
#endif	
	    soap_GetAudioDecoderConfigurationOptions(p_user, rx_msg, p_body, p_header);
	}
	else if (soap_strcmp(p_name, "GetCompatibleAudioDecoderConfigurations") == 0)
	{
	    soap_GetCompatibleAudioDecoderConfigurations(p_user, rx_msg, p_body, p_header);
	}	
#endif // end of AUDIO_SUPPORT

#if defined(PTZ_SUPPORT) || defined(THERMAL_SUPPORT)
    else if (soap_strcmp(p_name, "GetConfigurations") == 0)
	{
#ifdef THERMAL_SUPPORT	    
	    if (strstr(p_post, "thermal"))
	    {
	        soap_tth_GetConfigurations(p_user, rx_msg, p_body, p_header);
	    } 
#endif	
#ifdef PTZ_SUPPORT
        if (strstr(p_post, "ptz"))
        {
		    soap_GetConfigurations(p_user, rx_msg, p_body, p_header);
		}
#endif
	}
	else if (soap_strcmp(p_name, "GetConfiguration") == 0)
	{
#ifdef THERMAL_SUPPORT
	    if (strstr(p_post, "thermal"))
	    {
	        soap_tth_GetConfiguration(p_user, rx_msg, p_body, p_header);
	    } 
#endif	
#ifdef PTZ_SUPPORT
        if (strstr(p_post, "ptz"))
        {
		    soap_GetConfiguration(p_user, rx_msg, p_body, p_header);
		}
#endif		
	}
	else if (soap_strcmp(p_name, "SetConfiguration") == 0)
	{
#ifdef THERMAL_SUPPORT	    
	    if (strstr(p_post, "thermal"))
	    {
	        soap_tth_SetConfiguration(p_user, rx_msg, p_body, p_header);
	    } 
#endif
#ifdef PTZ_SUPPORT
        if (strstr(p_post, "ptz"))
        {
		    soap_SetConfiguration(p_user, rx_msg, p_body, p_header);
		}
#endif		
	}
	else if (soap_strcmp(p_name, "GetConfigurationOptions") == 0)
	{	
#ifdef THERMAL_SUPPORT	    
	    if (strstr(p_post, "thermal"))
	    {
	        soap_tth_GetConfigurationOptions(p_user, rx_msg, p_body, p_header);
	    } 
#endif	
#ifdef PTZ_SUPPORT
        if (strstr(p_post, "ptz"))
        {
		    soap_GetConfigurationOptions(p_user, rx_msg, p_body, p_header);
		}
#endif
	}
#endif // defined(PTZ_SUPPORT) || defined(THERMAL_SUPPORT) 	

#ifdef PTZ_SUPPORT	
	else if (soap_strcmp(p_name, "GetNodes") == 0)
	{
		soap_GetNodes(p_user, rx_msg, p_body, p_header);
	}
	else if (soap_strcmp(p_name, "GetNode") == 0)
	{
		soap_GetNode(p_user, rx_msg, p_body, p_header);
	}	
	else if (soap_strcmp(p_name, "GetCompatibleConfigurations") == 0)
	{
	    soap_GetCompatibleConfigurations(p_user, rx_msg, p_body, p_header);
	}	
	else if (soap_strcmp(p_name, "AddPTZConfiguration") == 0)
	{
		soap_AddPTZConfiguration(p_user, rx_msg, p_body, p_header);
	}
	else if (soap_strcmp(p_name, "RemovePTZConfiguration") == 0)
	{
		soap_RemovePTZConfiguration(p_user, rx_msg, p_body, p_header);
	}
	else if (soap_strcmp(p_name, "ContinuousMove") == 0)
	{
		soap_ContinuousMove(p_user, rx_msg, p_body, p_header);
	}	
    else if (soap_strcmp(p_name, "AbsoluteMove") == 0)
	{
		soap_AbsoluteMove(p_user, rx_msg, p_body, p_header);
	}
	else if (soap_strcmp(p_name, "RelativeMove") == 0)
	{
		soap_RelativeMove(p_user, rx_msg, p_body, p_header);
	}
	else if (soap_strcmp(p_name, "SetPreset") == 0)
	{
		soap_SetPreset(p_user, rx_msg, p_body, p_header);
	}
	else if (soap_strcmp(p_name, "GetPresets") == 0)
	{
		soap_GetPresets(p_user, rx_msg, p_body, p_header);
	}
	else if (soap_strcmp(p_name, "RemovePreset") == 0)
	{
		soap_RemovePreset(p_user, rx_msg, p_body, p_header);
	}
	else if (soap_strcmp(p_name, "GotoPreset") == 0)
	{
		soap_GotoPreset(p_user, rx_msg, p_body, p_header);
	}
	else if (soap_strcmp(p_name, "GotoHomePosition") == 0)
	{
		soap_GotoHomePosition(p_user, rx_msg, p_body, p_header);
	}
	else if (soap_strcmp(p_name, "SetHomePosition") == 0)
	{
		soap_SetHomePosition(p_user, rx_msg, p_body, p_header);
	}	
#endif // PTZ_SUPPORT

#ifdef VIDEO_ANALYTICS	
	else if (soap_strcmp(p_name, "GetVideoAnalyticsConfigurations") == 0)
	{
		soap_GetVideoAnalyticsConfigurations(p_user, rx_msg, p_body, p_header);
	}
	else if (soap_strcmp(p_name, "AddVideoAnalyticsConfiguration") == 0)
	{
		soap_AddVideoAnalyticsConfiguration(p_user, rx_msg, p_body, p_header);
	}
	else if (soap_strcmp(p_name, "GetVideoAnalyticsConfiguration") == 0)
	{
		soap_GetVideoAnalyticsConfiguration(p_user, rx_msg, p_body, p_header);
	}
	else if (soap_strcmp(p_name, "RemoveVideoAnalyticsConfiguration") == 0)
	{
		soap_RemoveVideoAnalyticsConfiguration(p_user, rx_msg, p_body, p_header);
	}
	else if (soap_strcmp(p_name, "SetVideoAnalyticsConfiguration") == 0)
	{
		soap_SetVideoAnalyticsConfiguration(p_user, rx_msg, p_body, p_header);
	}	
	else if (soap_strcmp(p_name, "GetSupportedRules") == 0)
	{
		soap_GetSupportedRules(p_user, rx_msg, p_body, p_header);
	}
	else if (soap_strcmp(p_name, "CreateRules") == 0)
	{
		soap_CreateRules(p_user, rx_msg, p_body, p_header);
	}
	else if (soap_strcmp(p_name, "DeleteRules") == 0)
	{
		soap_DeleteRules(p_user, rx_msg, p_body, p_header);
	}
	else if (soap_strcmp(p_name, "GetRules") == 0)
	{
		soap_GetRules(p_user, rx_msg, p_body, p_header);
	}
	else if (soap_strcmp(p_name, "ModifyRules") == 0)
	{
		soap_ModifyRules(p_user, rx_msg, p_body, p_header);
	}
	else if (soap_strcmp(p_name, "CreateAnalyticsModules") == 0)
	{
		soap_CreateAnalyticsModules(p_user, rx_msg, p_body, p_header);
	}
	else if (soap_strcmp(p_name, "DeleteAnalyticsModules") == 0)
	{
		soap_DeleteAnalyticsModules(p_user, rx_msg, p_body, p_header);
	}
	else if (soap_strcmp(p_name, "GetAnalyticsModules") == 0)
	{
		soap_GetAnalyticsModules(p_user, rx_msg, p_body, p_header);
	}
	else if (soap_strcmp(p_name, "ModifyAnalyticsModules") == 0)
	{
		soap_ModifyAnalyticsModules(p_user, rx_msg, p_body, p_header);
	}
	else if (soap_strcmp(p_name, "GetAnalyticsConfigurations") == 0)
	{
#ifdef MEDIA2_SUPPORT
        if (strstr(p_post, "media2"))
	    {
	        soap_tr2_GetAnalyticsConfigurations(p_user, rx_msg, p_body, p_header);
	    }else
#endif
        soap_GetAnalyticsConfigurations(p_user, rx_msg, p_body, p_header);
	}
	else if (soap_strcmp(p_name, "GetRuleOptions") == 0)
	{
	    soap_GetRuleOptions(p_user, rx_msg, p_body, p_header);
	}
	else if (soap_strcmp(p_name, "GetSupportedAnalyticsModules") == 0)
	{
	    soap_GetSupportedAnalyticsModules(p_user, rx_msg, p_body, p_header);
	}
	else if (soap_strcmp(p_name, "GetAnalyticsModuleOptions") == 0)
	{
	    soap_GetAnalyticsModuleOptions(p_user, rx_msg, p_body, p_header);
	}
#endif	// endif of VIDEO_ANALYTICS

#ifdef PROFILE_G_SUPPORT
	else if (soap_strcmp(p_name, "GetRecordingSummary") == 0)
	{
		soap_GetRecordingSummary(p_user, rx_msg, p_body, p_header);
	}
	else if (soap_strcmp(p_name, "GetRecordingInformation") == 0)
	{
		soap_GetRecordingInformation(p_user, rx_msg, p_body, p_header);
	}
	else if (soap_strcmp(p_name, "GetMediaAttributes") == 0)
	{
		soap_GetMediaAttributes(p_user, rx_msg, p_body, p_header);
	}
	else if (soap_strcmp(p_name, "FindRecordings") == 0)
	{
		soap_FindRecordings(p_user, rx_msg, p_body, p_header);
	}
	else if (soap_strcmp(p_name, "GetRecordingSearchResults") == 0)
	{
		soap_GetRecordingSearchResults(p_user, rx_msg, p_body, p_header);
	}
	else if (soap_strcmp(p_name, "FindEvents") == 0)
	{
		soap_FindEvents(p_user, rx_msg, p_body, p_header);
	}
	else if (soap_strcmp(p_name, "GetEventSearchResults") == 0)
	{
		soap_GetEventSearchResults(p_user, rx_msg, p_body, p_header);
	}
	else if (soap_strcmp(p_name, "FindMetadata") == 0)
	{
		soap_FindMetadata(p_user, rx_msg, p_body, p_header);
	}
	else if (soap_strcmp(p_name, "GetMetadataSearchResults") == 0)
	{
		soap_GetMetadataSearchResults(p_user, rx_msg, p_body, p_header);
	}
#ifdef PTZ_SUPPORT
	else if (soap_strcmp(p_name, "FindPTZPosition") == 0)
	{
	    soap_FindPTZPosition(p_user, rx_msg, p_body, p_header);
	}
	else if (soap_strcmp(p_name, "GetPTZPositionSearchResults") == 0)
	{
	    soap_GetPTZPositionSearchResults(p_user, rx_msg, p_body, p_header);
	}
#endif
	else if (soap_strcmp(p_name, "EndSearch") == 0)
	{
		soap_EndSearch(p_user, rx_msg, p_body, p_header);
	}
	else if (soap_strcmp(p_name, "GetSearchState") == 0)
	{
		soap_GetSearchState(p_user, rx_msg, p_body, p_header);
	}
	else if (soap_strcmp(p_name, "CreateRecording") == 0)
	{
		soap_CreateRecording(p_user, rx_msg, p_body, p_header);
	}
	else if (soap_strcmp(p_name, "DeleteRecording") == 0)
	{
		soap_DeleteRecording(p_user, rx_msg, p_body, p_header);
	}
	else if (soap_strcmp(p_name, "GetRecordings") == 0)
	{
		soap_GetRecordings(p_user, rx_msg, p_body, p_header);
	}
	else if (soap_strcmp(p_name, "SetRecordingConfiguration") == 0)
	{
		soap_SetRecordingConfiguration(p_user, rx_msg, p_body, p_header);
	}
	else if (soap_strcmp(p_name, "GetRecordingConfiguration") == 0)
	{
		soap_GetRecordingConfiguration(p_user, rx_msg, p_body, p_header);
	}
	else if (soap_strcmp(p_name, "CreateTrack") == 0)
	{
		soap_CreateTrack(p_user, rx_msg, p_body, p_header);
	}
	else if (soap_strcmp(p_name, "DeleteTrack") == 0)
	{
		soap_DeleteTrack(p_user, rx_msg, p_body, p_header);
	}
	else if (soap_strcmp(p_name, "GetTrackConfiguration") == 0)
	{
		soap_GetTrackConfiguration(p_user, rx_msg, p_body, p_header);
	}
	else if (soap_strcmp(p_name, "SetTrackConfiguration") == 0)
	{
		soap_SetTrackConfiguration(p_user, rx_msg, p_body, p_header);
	}
	else if (soap_strcmp(p_name, "CreateRecordingJob") == 0)
	{
		soap_CreateRecordingJob(p_user, rx_msg, p_body, p_header);
	}
	else if (soap_strcmp(p_name, "DeleteRecordingJob") == 0)
	{
		soap_DeleteRecordingJob(p_user, rx_msg, p_body, p_header);
	}
	else if (soap_strcmp(p_name, "GetRecordingJobs") == 0)
	{
		soap_GetRecordingJobs(p_user, rx_msg, p_body, p_header);
	}
	else if (soap_strcmp(p_name, "SetRecordingJobConfiguration") == 0)
	{
		soap_SetRecordingJobConfiguration(p_user, rx_msg, p_body, p_header);
	}
	else if (soap_strcmp(p_name, "GetRecordingJobConfiguration") == 0)
	{
		soap_GetRecordingJobConfiguration(p_user, rx_msg, p_body, p_header);
	}
	else if (soap_strcmp(p_name, "SetRecordingJobMode") == 0)
	{
		soap_SetRecordingJobMode(p_user, rx_msg, p_body, p_header);
	}
	else if (soap_strcmp(p_name, "GetRecordingJobState") == 0)
	{
		soap_GetRecordingJobState(p_user, rx_msg, p_body, p_header);
	}
	else if (soap_strcmp(p_name, "GetRecordingOptions") == 0)
	{
		soap_GetRecordingOptions(p_user, rx_msg, p_body, p_header);
	}
	else if (soap_strcmp(p_name, "GetReplayUri") == 0)
	{
		soap_GetReplayUri(p_user, rx_msg, p_body, p_header);
	}
	else if (soap_strcmp(p_name, "GetReplayConfiguration") == 0)
	{
		soap_GetReplayConfiguration(p_user, rx_msg, p_body, p_header);
	}
	else if (soap_strcmp(p_name, "SetReplayConfiguration") == 0)
	{
		soap_SetReplayConfiguration(p_user, rx_msg, p_body, p_header);
	}
#endif	// end of PROFILE_G_SUPPORT

#ifdef PROFILE_C_SUPPORT
    else if (soap_strcmp(p_name, "GetAccessPointInfoList") == 0)
	{
		soap_tac_GetAccessPointInfoList(p_user, rx_msg, p_body, p_header);
	}
	else if (soap_strcmp(p_name, "GetAccessPointInfo") == 0)
	{
		soap_tac_GetAccessPointInfo(p_user, rx_msg, p_body, p_header);
	}
	else if (soap_strcmp(p_name, "GetAreaInfoList") == 0)
	{
		soap_tac_GetAreaInfoList(p_user, rx_msg, p_body, p_header);
	}
	else if (soap_strcmp(p_name, "GetAreaInfo") == 0)
	{
		soap_tac_GetAreaInfo(p_user, rx_msg, p_body, p_header);
	}
	else if (soap_strcmp(p_name, "GetAccessPointState") == 0)
	{
		soap_tac_GetAccessPointState(p_user, rx_msg, p_body, p_header);
	}
	else if (soap_strcmp(p_name, "EnableAccessPoint") == 0)
	{
		soap_tac_EnableAccessPoint(p_user, rx_msg, p_body, p_header);
	}
	else if (soap_strcmp(p_name, "DisableAccessPoint") == 0)
	{
		soap_tac_DisableAccessPoint(p_user, rx_msg, p_body, p_header);
	}	
	else if (soap_strcmp(p_name, "GetDoorInfoList") == 0)
	{
		soap_tdc_GetDoorInfoList(p_user, rx_msg, p_body, p_header);
	}
	else if (soap_strcmp(p_name, "GetDoorInfo") == 0)
	{
		soap_tdc_GetDoorInfo(p_user, rx_msg, p_body, p_header);
	}	
	else if (soap_strcmp(p_name, "GetDoorState") == 0)
	{
		soap_tdc_GetDoorState(p_user, rx_msg, p_body, p_header);
	}
	else if (soap_strcmp(p_name, "AccessDoor") == 0)
	{
		soap_tdc_AccessDoor(p_user, rx_msg, p_body, p_header);
	}
	else if (soap_strcmp(p_name, "LockDoor") == 0)
	{
		soap_tdc_LockDoor(p_user, rx_msg, p_body, p_header);
	}
	else if (soap_strcmp(p_name, "UnlockDoor") == 0)
	{
		soap_tdc_UnlockDoor(p_user, rx_msg, p_body, p_header);
	}
	else if (soap_strcmp(p_name, "DoubleLockDoor") == 0)
	{
		soap_tdc_DoubleLockDoor(p_user, rx_msg, p_body, p_header);
	}
	else if (soap_strcmp(p_name, "BlockDoor") == 0)
	{
		soap_tdc_BlockDoor(p_user, rx_msg, p_body, p_header);
	}
	else if (soap_strcmp(p_name, "LockDownDoor") == 0)
	{
		soap_tdc_LockDownDoor(p_user, rx_msg, p_body, p_header);
	}
	else if (soap_strcmp(p_name, "LockDownReleaseDoor") == 0)
	{
		soap_tdc_LockDownReleaseDoor(p_user, rx_msg, p_body, p_header);
	}
	else if (soap_strcmp(p_name, "LockOpenDoor") == 0)
	{
		soap_tdc_LockOpenDoor(p_user, rx_msg, p_body, p_header);
	}
	else if (soap_strcmp(p_name, "LockOpenReleaseDoor") == 0)
	{
		soap_tdc_LockOpenReleaseDoor(p_user, rx_msg, p_body, p_header);
	}	
#endif // end of PROFILE_C_SUPPORT

#ifdef DEVICEIO_SUPPORT
	else if (soap_strcmp(p_name, "GetVideoOutputs") == 0)
	{
		soap_GetVideoOutputs(p_user, rx_msg, p_body, p_header);
	}
	else if (soap_strcmp(p_name, "GetVideoOutputConfiguration") == 0)
	{
		soap_GetVideoOutputConfiguration(p_user, rx_msg, p_body, p_header);
	}
	else if (soap_strcmp(p_name, "SetVideoOutputConfiguration") == 0)
	{
		soap_SetVideoOutputConfiguration(p_user, rx_msg, p_body, p_header);
	}
	else if (soap_strcmp(p_name, "GetVideoOutputConfigurationOptions") == 0)
	{
		soap_GetVideoOutputConfigurationOptions(p_user, rx_msg, p_body, p_header);
	}
	else if (soap_strcmp(p_name, "GetAudioOutputs") == 0)
	{
		soap_GetAudioOutputs(p_user, rx_msg, p_body, p_header);
	}
	else if (soap_strcmp(p_name, "AddAudioOutputConfiguration") == 0)
	{
	    soap_AddAudioOutputConfiguration(p_user, rx_msg, p_body, p_header);
	}
	else if (soap_strcmp(p_name, "RemoveAudioOutputConfiguration") == 0)
	{
	    soap_RemoveAudioOutputConfiguration(p_user, rx_msg, p_body, p_header);
	}
	else if (soap_strcmp(p_name, "GetAudioOutputConfigurations") == 0)
	{
#ifdef MEDIA2_SUPPORT	    
	    if (strstr(p_post, "media2"))
	    {
	        soap_tr2_GetAudioOutputConfigurations(p_user, rx_msg, p_body, p_header);
	    }else 
#endif	
	    soap_GetAudioOutputConfigurations(p_user, rx_msg, p_body, p_header);
	}
	else if (soap_strcmp(p_name, "GetCompatibleAudioOutputConfigurations") == 0)
	{
	    soap_GetCompatibleAudioOutputConfigurations(p_user, rx_msg, p_body, p_header);
	}
	else if (soap_strcmp(p_name, "GetAudioOutputConfiguration") == 0)
	{
		soap_GetAudioOutputConfiguration(p_user, rx_msg, p_body, p_header);
	}	
	else if (soap_strcmp(p_name, "SetAudioOutputConfiguration") == 0)
	{
#ifdef MEDIA2_SUPPORT    
	    if (strstr(p_post, "media2"))
	    {
	        soap_tr2_SetAudioOutputConfiguration(p_user, rx_msg, p_body, p_header);
	    }else 
#endif	
		soap_SetAudioOutputConfiguration(p_user, rx_msg, p_body, p_header);
	}
	else if (soap_strcmp(p_name, "GetAudioOutputConfigurationOptions") == 0)
	{
#ifdef MEDIA2_SUPPORT	    
	    if (strstr(p_post, "media2"))
	    {
	        soap_tr2_GetAudioOutputConfigurationOptions(p_user, rx_msg, p_body, p_header);
	    }else 
#endif
	    if (strstr(p_post, "media"))
	    {
	        soap_trt_GetAudioOutputConfigurationOptions(p_user, rx_msg, p_body, p_header);
	    }
	    else 
	    {
		    soap_tmd_GetAudioOutputConfigurationOptions(p_user, rx_msg, p_body, p_header);
		}
	}
	else if (soap_strcmp(p_name, "GetRelayOutputs") == 0)
	{
	    if (soap_isDeviceServiceNamespace(p_body))
	    {
	        soap_GetRelayOutputs(p_user, rx_msg, p_body, p_header);
	    }
	    else
	    {
		    soap_tmd_GetRelayOutputs(p_user, rx_msg, p_body, p_header);
		}
	}
	else if (soap_strcmp(p_name, "GetRelayOutputOptions") == 0)
	{
		soap_GetRelayOutputOptions(p_user, rx_msg, p_body, p_header);
	}
	else if (soap_strcmp(p_name, "SetRelayOutputSettings") == 0)
	{
		soap_SetRelayOutputSettings(p_user, rx_msg, p_body, p_header);
	}
	else if (soap_strcmp(p_name, "SetRelayOutputState") == 0)
	{
	    if (soap_isDeviceServiceNamespace(p_body))
	    {
	        soap_SetRelayOutputState(p_user, rx_msg, p_body, p_header);
	    }
	    else
	    {
	        soap_tmd_SetRelayOutputState(p_user, rx_msg, p_body, p_header);
	    }
	}
	else if (soap_strcmp(p_name, "GetDigitalInputs") == 0)
	{
		soap_GetDigitalInputs(p_user, rx_msg, p_body, p_header);
	}
	else if (soap_strcmp(p_name, "GetDigitalInputConfigurationOptions") == 0)
	{
		soap_GetDigitalInputConfigurationOptions(p_user, rx_msg, p_body, p_header);
	}
	else if (soap_strcmp(p_name, "SetDigitalInputConfigurations") == 0)
	{
		soap_SetDigitalInputConfigurations(p_user, rx_msg, p_body, p_header);
	}
	else if (soap_strcmp(p_name, "GetSerialPorts") == 0)
	{
		soap_GetSerialPorts(p_user, rx_msg, p_body, p_header);
	}
	else if (soap_strcmp(p_name, "GetSerialPortConfiguration") == 0)
	{
		soap_GetSerialPortConfiguration(p_user, rx_msg, p_body, p_header);
	}
	else if (soap_strcmp(p_name, "GetSerialPortConfigurationOptions") == 0)
	{
		soap_GetSerialPortConfigurationOptions(p_user, rx_msg, p_body, p_header);
	}
	else if (soap_strcmp(p_name, "SetSerialPortConfiguration") == 0)
	{
		soap_SetSerialPortConfiguration(p_user, rx_msg, p_body, p_header);
	}
	else if (soap_strcmp(p_name, "SendReceiveSerialCommand") == 0)
	{
		soap_SendReceiveSerialCommand(p_user, rx_msg, p_body, p_header);
	}
#endif // end of DEVICEIO_SUPPORT

#ifdef MEDIA2_SUPPORT    
    else if (soap_strcmp(p_name, "AddConfiguration") == 0)
	{
		soap_tr2_AddConfiguration(p_user, rx_msg, p_body, p_header);
	}
	else if (soap_strcmp(p_name, "RemoveConfiguration") == 0)
	{
		soap_tr2_RemoveConfiguration(p_user, rx_msg, p_body, p_header);
	}
	else if (soap_strcmp(p_name, "GetVideoEncoderInstances") == 0)
	{
	    soap_tr2_GetVideoEncoderInstances(p_user, rx_msg, p_body, p_header);
	}
	else if (soap_strcmp(p_name, "CreateMask") == 0)
	{	    
		soap_tr2_CreateMask(p_user, rx_msg, p_body, p_header);
	}
	else if (soap_strcmp(p_name, "DeleteMask") == 0)
	{	    
		soap_tr2_DeleteMask(p_user, rx_msg, p_body, p_header);
	}
	else if (soap_strcmp(p_name, "GetMasks") == 0)
	{	    
		soap_tr2_GetMasks(p_user, rx_msg, p_body, p_header);
	}
	else if (soap_strcmp(p_name, "SetMask") == 0)
	{	    
		soap_tr2_SetMask(p_user, rx_msg, p_body, p_header);
	}
	else if (soap_strcmp(p_name, "GetMaskOptions") == 0)
	{	    
		soap_tr2_GetMaskOptions(p_user, rx_msg, p_body, p_header);
	}
#endif // end of MEDIA2_SUPPORT

#ifdef THERMAL_SUPPORT
    else if (soap_strcmp(p_name, "GetRadiometryConfiguration") == 0)
	{
		soap_tth_GetRadiometryConfiguration(p_user, rx_msg, p_body, p_header);
	}
	else if (soap_strcmp(p_name, "SetRadiometryConfiguration") == 0)
	{
		soap_tth_SetRadiometryConfiguration(p_user, rx_msg, p_body, p_header);
	}
	else if (soap_strcmp(p_name, "GetRadiometryConfigurationOptions") == 0)
	{
		soap_tth_GetRadiometryConfigurationOptions(p_user, rx_msg, p_body, p_header);
	}
#endif // end of THERMAL_SUPPORT

#ifdef CREDENTIAL_SUPPORT
    else if (soap_strcmp(p_name, "GetCredentialInfo") == 0)
	{
		soap_tcr_GetCredentialInfo(p_user, rx_msg, p_body, p_header);
	}
	else if (soap_strcmp(p_name, "GetCredentialInfoList") == 0)
	{
		soap_tcr_GetCredentialInfoList(p_user, rx_msg, p_body, p_header);
	}
	else if (soap_strcmp(p_name, "GetCredentials") == 0)
	{
		soap_tcr_GetCredentials(p_user, rx_msg, p_body, p_header);
	}
	else if (soap_strcmp(p_name, "GetCredentialList") == 0)
	{
		soap_tcr_GetCredentialList(p_user, rx_msg, p_body, p_header);
	}
	else if (soap_strcmp(p_name, "CreateCredential") == 0)
	{
		soap_tcr_CreateCredential(p_user, rx_msg, p_body, p_header);
	}	
	else if (soap_strcmp(p_name, "ModifyCredential") == 0)
	{
		soap_tcr_ModifyCredential(p_user, rx_msg, p_body, p_header);
	}	
	else if (soap_strcmp(p_name, "DeleteCredential") == 0)
	{
		soap_tcr_DeleteCredential(p_user, rx_msg, p_body, p_header);
	}
	else if (soap_strcmp(p_name, "GetCredentialState") == 0)
	{
		soap_tcr_GetCredentialState(p_user, rx_msg, p_body, p_header);
	}
	else if (soap_strcmp(p_name, "EnableCredential") == 0)
	{
		soap_tcr_EnableCredential(p_user, rx_msg, p_body, p_header);
	}
	else if (soap_strcmp(p_name, "DisableCredential") == 0)
	{
		soap_tcr_DisableCredential(p_user, rx_msg, p_body, p_header);
	}
	else if (soap_strcmp(p_name, "ResetAntipassbackViolation") == 0)
	{
		soap_tcr_ResetAntipassbackViolation(p_user, rx_msg, p_body, p_header);
	}
	else if (soap_strcmp(p_name, "GetSupportedFormatTypes") == 0)
	{
		soap_tcr_GetSupportedFormatTypes(p_user, rx_msg, p_body, p_header);
	}
	else if (soap_strcmp(p_name, "GetCredentialIdentifiers") == 0)
	{
		soap_tcr_GetCredentialIdentifiers(p_user, rx_msg, p_body, p_header);
	}
	else if (soap_strcmp(p_name, "SetCredentialIdentifier") == 0)
	{
		soap_tcr_SetCredentialIdentifier(p_user, rx_msg, p_body, p_header);
	}
	else if (soap_strcmp(p_name, "DeleteCredentialIdentifier") == 0)
	{
		soap_tcr_DeleteCredentialIdentifier(p_user, rx_msg, p_body, p_header);
	}
	else if (soap_strcmp(p_name, "GetCredentialAccessProfiles") == 0)
	{
		soap_tcr_GetCredentialAccessProfiles(p_user, rx_msg, p_body, p_header);
	}
	else if (soap_strcmp(p_name, "SetCredentialAccessProfiles") == 0)
	{
		soap_tcr_SetCredentialAccessProfiles(p_user, rx_msg, p_body, p_header);
	}	
	else if (soap_strcmp(p_name, "DeleteCredentialAccessProfiles") == 0)
	{
		soap_tcr_DeleteCredentialAccessProfiles(p_user, rx_msg, p_body, p_header);
	}
#endif // end of CREDENTIAL_SUPPORT

#ifdef ACCESS_RULES
    else if (soap_strcmp(p_name, "GetAccessProfileInfo") == 0)
	{
		soap_tar_GetAccessProfileInfo(p_user, rx_msg, p_body, p_header);
	}
    else if (soap_strcmp(p_name, "GetAccessProfileInfoList") == 0)
	{
		soap_tar_GetAccessProfileInfoList(p_user, rx_msg, p_body, p_header);
	}
	else if (soap_strcmp(p_name, "GetAccessProfiles") == 0)
	{
		soap_tar_GetAccessProfiles(p_user, rx_msg, p_body, p_header);
	}
	else if (soap_strcmp(p_name, "GetAccessProfileList") == 0)
	{
		soap_tar_GetAccessProfileList(p_user, rx_msg, p_body, p_header);
	}
	else if (soap_strcmp(p_name, "CreateAccessProfile") == 0)
	{
		soap_tar_CreateAccessProfile(p_user, rx_msg, p_body, p_header);
	}
	else if (soap_strcmp(p_name, "ModifyAccessProfile") == 0)
	{
		soap_tar_ModifyAccessProfile(p_user, rx_msg, p_body, p_header);
	}
	else if (soap_strcmp(p_name, "DeleteAccessProfile") == 0)
	{
		soap_tar_DeleteAccessProfile(p_user, rx_msg, p_body, p_header);
	}
#endif // end of ACCESS_RULES

#ifdef SCHEDULE_SUPPORT
    else if (soap_strcmp(p_name, "GetScheduleInfo") == 0)
	{
		soap_tsc_GetScheduleInfo(p_user, rx_msg, p_body, p_header);
	}
	else if (soap_strcmp(p_name, "GetScheduleInfoList") == 0)
	{
		soap_tsc_GetScheduleInfoList(p_user, rx_msg, p_body, p_header);
	}
	else if (soap_strcmp(p_name, "GetSchedules") == 0)
	{
		soap_tsc_GetSchedules(p_user, rx_msg, p_body, p_header);
	}
	else if (soap_strcmp(p_name, "GetScheduleList") == 0)
	{
		soap_tsc_GetScheduleList(p_user, rx_msg, p_body, p_header);
	}
	else if (soap_strcmp(p_name, "CreateSchedule") == 0)
	{
		soap_tsc_CreateSchedule(p_user, rx_msg, p_body, p_header);
	}
	else if (soap_strcmp(p_name, "ModifySchedule") == 0)
	{
		soap_tsc_ModifySchedule(p_user, rx_msg, p_body, p_header);
	}
	else if (soap_strcmp(p_name, "DeleteSchedule") == 0)
	{
		soap_tsc_DeleteSchedule(p_user, rx_msg, p_body, p_header);
	}
	else if (soap_strcmp(p_name, "GetSpecialDayGroupInfo") == 0)
	{
		soap_tsc_GetSpecialDayGroupInfo(p_user, rx_msg, p_body, p_header);
	}
	else if (soap_strcmp(p_name, "GetSpecialDayGroupInfoList") == 0)
	{
		soap_tsc_GetSpecialDayGroupInfoList(p_user, rx_msg, p_body, p_header);
	}
	else if (soap_strcmp(p_name, "GetSpecialDayGroups") == 0)
	{
		soap_tsc_GetSpecialDayGroups(p_user, rx_msg, p_body, p_header);
	}
	else if (soap_strcmp(p_name, "GetSpecialDayGroupList") == 0)
	{
		soap_tsc_GetSpecialDayGroupList(p_user, rx_msg, p_body, p_header);
	}
	else if (soap_strcmp(p_name, "CreateSpecialDayGroup") == 0)
	{
		soap_tsc_CreateSpecialDayGroup(p_user, rx_msg, p_body, p_header);
	}
	else if (soap_strcmp(p_name, "ModifySpecialDayGroup") == 0)
	{
		soap_tsc_ModifySpecialDayGroup(p_user, rx_msg, p_body, p_header);
	}
	else if (soap_strcmp(p_name, "DeleteSpecialDayGroup") == 0)
	{
		soap_tsc_DeleteSpecialDayGroup(p_user, rx_msg, p_body, p_header);
	}
	else if (soap_strcmp(p_name, "GetScheduleState") == 0)
	{
		soap_tsc_GetScheduleState(p_user, rx_msg, p_body, p_header);
	}
#endif // end of SCHEDULE_SUPPORT

#ifdef RECEIVER_SUPPORT
    else if (soap_strcmp(p_name, "GetReceivers") == 0)
	{
		soap_trv_GetReceivers(p_user, rx_msg, p_body, p_header);
	}
	else if (soap_strcmp(p_name, "GetReceiver") == 0)
	{
		soap_trv_GetReceiver(p_user, rx_msg, p_body, p_header);
	}
	else if (soap_strcmp(p_name, "CreateReceiver") == 0)
	{
		soap_trv_CreateReceiver(p_user, rx_msg, p_body, p_header);
	}
	else if (soap_strcmp(p_name, "DeleteReceiver") == 0)
	{
		soap_trv_DeleteReceiver(p_user, rx_msg, p_body, p_header);
	}
	else if (soap_strcmp(p_name, "ConfigureReceiver") == 0)
	{
		soap_trv_ConfigureReceiver(p_user, rx_msg, p_body, p_header);
	}
	else if (soap_strcmp(p_name, "SetReceiverMode") == 0)
	{
		soap_trv_SetReceiverMode(p_user, rx_msg, p_body, p_header);
	}
    else if (soap_strcmp(p_name, "GetReceiverState") == 0)
	{
		soap_trv_GetReceiverState(p_user, rx_msg, p_body, p_header);
	}
#endif // end of RECEIVER_SUPPORT

	else
	{
		soap_err_def_rly(p_user, rx_msg);
	}
	
	xml_node_del(p_node);
}



