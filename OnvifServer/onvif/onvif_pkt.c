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

/***************************************************************************************/
#include "sys_inc.h"
#include "onvif.h"
#include "xml_node.h"
#include "onvif_device.h"
#include "onvif_pkt.h"
#include "onvif_event.h"
#include "onvif_ptz.h"
#include "onvif_utils.h"
#include "onvif_err.h"
#include "onvif_media.h"
#include "onvif_image.h"
#ifdef VIDEO_ANALYTICS
#include "onvif_analytics.h"
#endif
#ifdef PROFILE_G_SUPPORT
#include "onvif_recording.h"
#endif
#ifdef PROFILE_C_SUPPORT
#include "onvif_doorcontrol.h"
#endif
#ifdef DEVICEIO_SUPPORT
#include "onvif_deviceio.h"
#endif
#ifdef MEDIA2_SUPPORT
#include "onvif_media2.h"
#endif
#ifdef THERMAL_SUPPORT
#include "onvif_thermal.h"
#endif
#ifdef CREDENTIAL_SUPPORT
#include "onvif_credential.h"
#endif
#ifdef ACCESS_RULES
#include "onvif_accessrules.h"
#endif
#ifdef SCHEDULE_SUPPORT
#include "onvif_schedule.h"
#endif
#ifdef RECEIVER_SUPPORT
#include "onvif_receiver.h"
#endif


#if __WINDOWS_OS__
#pragma warning(disable:4996)
#endif

/***************************************************************************************/
extern ONVIF_CFG g_onvif_cfg;
extern ONVIF_CLS g_onvif_cls;

extern char xml_hdr[];
extern char onvif_xmlns[];
extern char soap_head[];
extern char soap_body[];
extern char soap_tailer[];  

/***************************************************************************************/

#ifdef VIDEO_ANALYTICS
int build_VideoAnalyticsConfiguration_xml(char * p_buf, int mlen, onvif_VideoAnalyticsConfiguration * p_req);
#endif

#ifdef DEVICEIO_SUPPORT
int build_AudioOutputConfiguration_xml(char * p_buf, int mlen, onvif_AudioOutputConfiguration * p_req);
#endif

#ifdef AUDIO_SUPPORT
int build_AudioDecoderConfiguration_xml(char * p_buf, int mlen, onvif_AudioDecoderConfiguration * p_req);
#endif

/***************************************************************************************/

int build_err_rly_xml
(
char * p_buf, 
int mlen, 
const char * code, 
const char * subcode, 
const char * subcode_ex, 
const char * reason,
const char * action
)
{
	int offset = snprintf(p_buf, mlen, xml_hdr);

	offset += snprintf(p_buf+offset, mlen-offset, onvif_xmlns);

	if (action)
	{
	    offset += snprintf(p_buf+offset, mlen-offset, soap_head, action);
	}
	
	offset += snprintf(p_buf+offset, mlen-offset, soap_body);

    offset += snprintf(p_buf+offset, mlen-offset, "<s:Fault>");
    offset += snprintf(p_buf+offset, mlen-offset, "<s:Code>");    
	offset += snprintf(p_buf+offset, mlen-offset, "<s:Value>%s</s:Value>", code);

	if (subcode)
	{
	    offset += snprintf(p_buf+offset, mlen-offset, "<s:Subcode>");
	    offset += snprintf(p_buf+offset, mlen-offset, "<s:Value>%s</s:Value>", subcode);

	    if (subcode_ex)
        {
            offset += snprintf(p_buf+offset, mlen-offset, 
                "<s:Subcode><s:Value>%s</s:Value></s:Subcode>", 
                subcode_ex);
        }

        offset += snprintf(p_buf+offset, mlen-offset, "</s:Subcode>");
	}
    
    offset += snprintf(p_buf+offset, mlen-offset, "</s:Code>");

    if (reason)
    {
	    offset += snprintf(p_buf+offset, mlen-offset, 
	        "<s:Reason><s:Text xml:lang=\"en\">%s</s:Text></s:Reason>", 
	        reason);
    }

    offset += snprintf(p_buf+offset, mlen-offset, "</s:Fault>");
    
	offset += snprintf(p_buf+offset, mlen-offset, soap_tailer);
	
	return offset;
}


int build_GetDeviceInformation_rly_xml(char * p_buf, int mlen, const char * argv)
{
	int offset = 0;
	
	offset += snprintf(p_buf+offset, mlen-offset, 
		"<tds:GetDeviceInformationResponse>"
		    "<tds:Manufacturer>%s</tds:Manufacturer>"
		    "<tds:Model>%s</tds:Model>"
		    "<tds:FirmwareVersion>%s</tds:FirmwareVersion>"
		    "<tds:SerialNumber>%s</tds:SerialNumber>"
		    "<tds:HardwareId>%s</tds:HardwareId>"
	    "</tds:GetDeviceInformationResponse>", 
    	g_onvif_cfg.DeviceInformation.Manufacturer, 
    	g_onvif_cfg.DeviceInformation.Model, 
    	g_onvif_cfg.DeviceInformation.FirmwareVersion, 
    	g_onvif_cfg.DeviceInformation.SerialNumber, 
    	g_onvif_cfg.DeviceInformation.HardwareId);
	
	return offset;
}

int build_GetSystemUris_rly_xml(char * p_buf, int mlen, const char * argv)
{
    int offset = 0;
    GetSystemUris_RES * p_res = (GetSystemUris_RES *)argv;
	
	offset += snprintf(p_buf+offset, mlen-offset, "<tds:GetSystemUrisResponse>");
	offset += snprintf(p_buf+offset, mlen-offset, "<tds:SystemLogUris>");
        
    if (p_res->SystemLogUriFlag)
    {
        offset += snprintf(p_buf+offset, mlen-offset,
            "<tt:SystemLog>"
                "<tt:Type>System</tt:Type>"
                "<tt:Uri>%s</tt:Uri>"
            "</tt:SystemLog>", p_res->SystemLogUri);
    }

    if (p_res->AccessLogUriFlag)
    {
        offset += snprintf(p_buf+offset, mlen-offset,
            "<tt:SystemLog>"
                "<tt:Type>Access</tt:Type>"
                "<tt:Uri>%s</tt:Uri>"
            "</tt:SystemLog>", p_res->AccessLogUri);
    }

    offset += snprintf(p_buf+offset, mlen-offset, "</tds:SystemLogUris>");

    if (p_res->SupportInfoUriFlag)
    {
        offset += snprintf(p_buf+offset, mlen-offset,
            "<tds:SupportInfoUri>%s</tds:SupportInfoUri>",
            p_res->SupportInfoUri);
    }

    if (p_res->SystemBackupUriFlag)
    {
        offset += snprintf(p_buf+offset, mlen-offset,
            "<tds:SystemBackupUri>%s</tds:SystemBackupUri>",
            p_res->SystemBackupUri);
    }

    offset += snprintf(p_buf+offset, mlen-offset, "</tds:GetSystemUrisResponse>");
	
	return offset;
}

int build_MulticastConfiguration_xml(char * p_buf, int mlen, onvif_MulticastConfiguration * p_cfg)
{
	int offset = 0;
	
	offset += snprintf(p_buf+offset, mlen-offset, 
		"<tt:Multicast>"
			"<tt:Address>"
				"<tt:Type>IPv4</tt:Type>"
				"<tt:IPv4Address>%s</tt:IPv4Address>"
			"</tt:Address>"
			"<tt:Port>%d</tt:Port>"
			"<tt:TTL>%d</tt:TTL>"
			"<tt:AutoStart>%s</tt:AutoStart>"
		"</tt:Multicast>", 
	    p_cfg->IPv4Address,
	    p_cfg->Port,
	    p_cfg->TTL,
	    p_cfg->AutoStart ? "true" : "false");

	return offset;	    
}

int build_VideoEncoderConfiguration_xml(char * p_buf, int mlen, onvif_VideoEncoder2Configuration * p_v_enc_cfg)
{
	int offset = 0;
	
	offset += snprintf(p_buf+offset, mlen-offset, 
		"<tt:Name>%s</tt:Name>"
		"<tt:UseCount>%d</tt:UseCount>"
	    "<tt:Encoding>%s</tt:Encoding>"
	    "<tt:Resolution>"
	    	"<tt:Width>%d</tt:Width>"
	    	"<tt:Height>%d</tt:Height>"
	    "</tt:Resolution>"
	    "<tt:Quality>%d</tt:Quality>",
	    p_v_enc_cfg->Name, 
	    p_v_enc_cfg->UseCount, 
	    onvif_VideoEncodingToString(p_v_enc_cfg->VideoEncoding), 
	    p_v_enc_cfg->Resolution.Width, 
	    p_v_enc_cfg->Resolution.Height, 
	    (int)p_v_enc_cfg->Quality);

	if (p_v_enc_cfg->RateControlFlag)
	{
		offset += snprintf(p_buf+offset, mlen-offset,     
		    "<tt:RateControl>"
		    	"<tt:FrameRateLimit>%d</tt:FrameRateLimit>"
		    	"<tt:EncodingInterval>%d</tt:EncodingInterval>"
		    	"<tt:BitrateLimit>%d</tt:BitrateLimit>"
		    "</tt:RateControl>",		    
		    (int)p_v_enc_cfg->RateControl.FrameRateLimit,
		    p_v_enc_cfg->RateControl.EncodingInterval, 
		    p_v_enc_cfg->RateControl.BitrateLimit);
	}
	
	if (p_v_enc_cfg->VideoEncoding == VideoEncoding_H264)
	{
		offset += snprintf(p_buf+offset, mlen-offset, 
			"<tt:H264>"
				"<tt:GovLength>%d</tt:GovLength>"
    			"<tt:H264Profile>%s</tt:H264Profile>"
    		"</tt:H264>", 
    		p_v_enc_cfg->GovLength,
	    	p_v_enc_cfg->Profile);
	}
	else if (p_v_enc_cfg->VideoEncoding == VideoEncoding_MPEG4)
	{
		offset += snprintf(p_buf+offset, mlen-offset, 
			"<tt:MPEG4>"
				"<tt:GovLength>%d</tt:GovLength>"
    			"<tt:Mpeg4Profile>%s</tt:Mpeg4Profile>"
    		"</tt:MPEG4>", 
    		p_v_enc_cfg->GovLength,
	    	p_v_enc_cfg->Profile);
	}
    else if (p_v_enc_cfg->VideoEncoding == VideoEncoding_H265)
	{
		offset += snprintf(p_buf+offset, mlen-offset, 
			"<tt:H265>"
				"<tt:GovLength>%d</tt:GovLength>"
    			"<tt:H265Profile>%s</tt:H265Profile>"
    		"</tt:H265>", 
    		p_v_enc_cfg->GovLength,
	    	p_v_enc_cfg->Profile);
	}

	offset += build_MulticastConfiguration_xml(p_buf+offset, mlen-offset, &p_v_enc_cfg->Multicast);
	
	offset += snprintf(p_buf+offset, mlen-offset, 		
	    "<tt:SessionTimeout>PT%dS</tt:SessionTimeout>",
	    p_v_enc_cfg->SessionTimeout);

	return offset;    
}

#ifdef AUDIO_SUPPORT

int build_AudioSourceConfiguration_xml(char * p_buf, int mlen, onvif_AudioSourceConfiguration * p_req)
{
	int offset = 0;

	offset += snprintf(p_buf+offset, mlen-offset,
		"<tt:Name>%s</tt:Name>"
	    "<tt:UseCount>%d</tt:UseCount>"
	    "<tt:SourceToken>%s</tt:SourceToken>", 
		p_req->Name, 
        p_req->UseCount, 
        p_req->SourceToken);

	return offset;            
}

int build_AudioEncoderConfiguration_xml(char * p_buf, int mlen, ONVIF_AudioEncoder2Configuration * p_a_enc_cfg)
{
	int offset = 0;
	
	offset += snprintf(p_buf+offset, mlen-offset, 
		"<tt:Name>%s</tt:Name>"
		"<tt:UseCount>%d</tt:UseCount>"
		"<tt:Encoding>%s</tt:Encoding>"
		"<tt:Bitrate>%d</tt:Bitrate>"
		"<tt:SampleRate>%d</tt:SampleRate>", 
		p_a_enc_cfg->Configuration.Name, 
    	p_a_enc_cfg->Configuration.UseCount, 
    	onvif_AudioEncodingToString(p_a_enc_cfg->Configuration.AudioEncoding), 
	    p_a_enc_cfg->Configuration.Bitrate, 
	    p_a_enc_cfg->Configuration.SampleRate); 

	offset += build_MulticastConfiguration_xml(p_buf+offset, mlen-offset, &p_a_enc_cfg->Configuration.Multicast);

	offset += snprintf(p_buf+offset, mlen-offset, 		
	    "<tt:SessionTimeout>PT%dS</tt:SessionTimeout>", 
		p_a_enc_cfg->Configuration.SessionTimeout);
		
	return offset;    	    
}

#endif // end of AUDIO_SUPPORT

#ifdef PTZ_SUPPORT

int build_PTZConfiguration_xml(char * p_buf, int mlen, ONVIF_PTZConfiguration * p_ptz_cfg)
{
	int offset = 0;
	
	offset += snprintf(p_buf+offset, mlen-offset,
    	"<tt:Name>%s</tt:Name>"
    	"<tt:UseCount>%d</tt:UseCount>"
    	"<tt:NodeToken>%s</tt:NodeToken>",     	
    	p_ptz_cfg->Configuration.Name, 
    	p_ptz_cfg->Configuration.UseCount,
    	p_ptz_cfg->Configuration.NodeToken);

   offset += snprintf(p_buf+offset, mlen-offset,  	
	    "<tt:DefaultAbsolutePantTiltPositionSpace>"
	    	"http://www.onvif.org/ver10/tptz/PanTiltSpaces/PositionGenericSpace"
    	"</tt:DefaultAbsolutePantTiltPositionSpace>"
	    "<tt:DefaultAbsoluteZoomPositionSpace>"
	    	"http://www.onvif.org/ver10/tptz/ZoomSpaces/PositionGenericSpace"
    	"</tt:DefaultAbsoluteZoomPositionSpace>"
	    "<tt:DefaultRelativePanTiltTranslationSpace>"
	    	"http://www.onvif.org/ver10/tptz/PanTiltSpaces/TranslationGenericSpace"
    	"</tt:DefaultRelativePanTiltTranslationSpace>"
	    "<tt:DefaultRelativeZoomTranslationSpace>"
	    	"http://www.onvif.org/ver10/tptz/ZoomSpaces/TranslationGenericSpace"
	    "</tt:DefaultRelativeZoomTranslationSpace>"
	    "<tt:DefaultContinuousPanTiltVelocitySpace>"
	   	 	"http://www.onvif.org/ver10/tptz/PanTiltSpaces/VelocityGenericSpace"
	    "</tt:DefaultContinuousPanTiltVelocitySpace>"
	    "<tt:DefaultContinuousZoomVelocitySpace>"
	    	"http://www.onvif.org/ver10/tptz/ZoomSpaces/VelocityGenericSpace"
	    "</tt:DefaultContinuousZoomVelocitySpace>");

	if (p_ptz_cfg->Configuration.DefaultPTZSpeedFlag)
	{
		offset += snprintf(p_buf+offset, mlen-offset, "<tt:DefaultPTZSpeed>"); 	    
		if (p_ptz_cfg->Configuration.DefaultPTZSpeed.PanTiltFlag)
		{
			offset += snprintf(p_buf+offset, mlen-offset, 
				"<tt:PanTilt x=\"%0.1f\" y=\"%0.1f\" space=\"http://www.onvif.org/ver10/tptz/PanTiltSpaces/GenericSpeedSpace\" />",
				p_ptz_cfg->Configuration.DefaultPTZSpeed.PanTilt.x, 
				p_ptz_cfg->Configuration.DefaultPTZSpeed.PanTilt.y);
		}
		if (p_ptz_cfg->Configuration.DefaultPTZSpeed.ZoomFlag)
		{
			offset += snprintf(p_buf+offset, mlen-offset, 
				"<tt:Zoom x=\"%0.1f\" space=\"http://www.onvif.org/ver10/tptz/ZoomSpaces/ZoomGenericSpeedSpace\" />",
				p_ptz_cfg->Configuration.DefaultPTZSpeed.Zoom.x);
		}	
		offset += snprintf(p_buf+offset, mlen-offset, "</tt:DefaultPTZSpeed>"); 
	}

	if (p_ptz_cfg->Configuration.DefaultPTZTimeoutFlag)
	{
		offset += snprintf(p_buf+offset, mlen-offset,  	 	
	    	"<tt:DefaultPTZTimeout>PT%dS</tt:DefaultPTZTimeout>", 
	    	p_ptz_cfg->Configuration.DefaultPTZTimeout);
    }	

	if (p_ptz_cfg->Configuration.PanTiltLimitsFlag)
	{
	    offset += snprintf(p_buf+offset, mlen-offset,  	 	
			"<tt:PanTiltLimits>"
				"<tt:Range>"
					"<tt:URI>http://www.onvif.org/ver10/tptz/PanTiltSpaces/PositionGenericSpace</tt:URI>"
					"<tt:XRange>"
						"<tt:Min>%0.1f</tt:Min>"
						"<tt:Max>%0.1f</tt:Max>"
					"</tt:XRange>"
					"<tt:YRange>"
						"<tt:Min>%0.1f</tt:Min>"
						"<tt:Max>%0.1f</tt:Max>"
					"</tt:YRange>"
				"</tt:Range>"
			"</tt:PanTiltLimits>",
			p_ptz_cfg->Configuration.PanTiltLimits.XRange.Min, 
			p_ptz_cfg->Configuration.PanTiltLimits.XRange.Max,
			p_ptz_cfg->Configuration.PanTiltLimits.YRange.Min, 
			p_ptz_cfg->Configuration.PanTiltLimits.YRange.Max);
	}

	if (p_ptz_cfg->Configuration.ZoomLimitsFlag)
	{
		offset += snprintf(p_buf+offset, mlen-offset,  	 	
			"<tt:ZoomLimits>"
				"<tt:Range>"
					"<tt:URI>http://www.onvif.org/ver10/tptz/ZoomSpaces/PositionGenericSpace</tt:URI>"
					"<tt:XRange>"
						"<tt:Min>%0.1f</tt:Min>"
						"<tt:Max>%0.1f</tt:Max>"
					"</tt:XRange>"
				"</tt:Range>"
			"</tt:ZoomLimits>",
			p_ptz_cfg->Configuration.ZoomLimits.XRange.Min,
			p_ptz_cfg->Configuration.ZoomLimits.XRange.Max);
	}

    if (p_ptz_cfg->Configuration.ExtensionFlag)
    {
        offset += snprintf(p_buf+offset, mlen-offset, "<tt:Extension>");
        if (p_ptz_cfg->Configuration.Extension.PTControlDirectionFlag)
        {
            offset += snprintf(p_buf+offset, mlen-offset, "<tt:PTControlDirection>");
            if (p_ptz_cfg->Configuration.Extension.PTControlDirection.EFlipFlag)
            {
                offset += snprintf(p_buf+offset, mlen-offset, 
                    "<tt:EFlip>"
                        "<tt:Mode>%s</tt:Mode>"
                    "</tt:EFlip>",
                    onvif_EFlipModeToString(p_ptz_cfg->Configuration.Extension.PTControlDirection.EFlip));                
            }
            if (p_ptz_cfg->Configuration.Extension.PTControlDirection.ReverseFlag)
            {
                offset += snprintf(p_buf+offset, mlen-offset, 
                    "<tt:Reverse>"
                        "<tt:Mode>%s</tt:Mode>"
                    "</tt:Reverse>",
                    onvif_ReverseModeToString(p_ptz_cfg->Configuration.Extension.PTControlDirection.Reverse));
            }
            offset += snprintf(p_buf+offset, mlen-offset, "</tt:PTControlDirection>");
        }    
        offset += snprintf(p_buf+offset, mlen-offset, "</tt:Extension>");
    }

	return offset;
}

#endif // end of PTZ_SUPPORT

int build_VideoSourceConfiguration_xml(char * p_buf, int mlen, onvif_VideoSourceConfiguration * p_req)
{
	int offset = 0;
	
	offset += snprintf(p_buf+offset, mlen-offset, 
		"<tt:Name>%s</tt:Name>"
	    "<tt:UseCount>%d</tt:UseCount>"
	    "<tt:SourceToken>%s</tt:SourceToken>"
	    "<tt:Bounds height=\"%d\" width=\"%d\" y=\"%d\" x=\"%d\" />",
    	p_req->Name, 
        p_req->UseCount, 
        p_req->SourceToken, 
        p_req->Bounds.height, 
        p_req->Bounds.width, 
        p_req->Bounds.y, 
        p_req->Bounds.x);

	return offset;            
}

int build_MetadataConfiguration_xml(char * p_buf, int mlen, onvif_MetadataConfiguration * p_req)
{
	int offset = 0;

	offset += snprintf(p_buf+offset, mlen-offset,
		"<tt:Name>%s</tt:Name>"
    	"<tt:UseCount>%d</tt:UseCount>",
    	p_req->Name,
    	p_req->UseCount);

	if (p_req->PTZStatusFlag)
	{
		offset += snprintf(p_buf+offset, mlen-offset,
			"<tt:PTZStatus>"
				"<tt:Status>%s</tt:Status>"
				"<tt:Position>%s</tt:Position>"
			"</tt:PTZStatus>",
	    	p_req->PTZStatus.Status ? "true" : "false",
	    	p_req->PTZStatus.Position ? "true" : "false");
	}

    if (p_req->EventsFlag)
    {
        offset += snprintf(p_buf+offset, mlen-offset,
			"<tt:Events>"
    			"<tt:Filter>"
    				"<wsnt:TopicExpression Dialect=\"%s\">%s</wsnt:TopicExpression>"
    			"</tt:Filter>"	
			"</tt:Events>",
			p_req->Events.Dialect,
	    	p_req->Events.TopicExpression);
    }
    
	if (p_req->AnalyticsFlag)
	{
		offset += snprintf(p_buf+offset, mlen-offset,
			"<tt:Analytics>%s</tt:Analytics>",
	    	p_req->Analytics ? "true" : "false");
	}

    offset += build_MulticastConfiguration_xml(p_buf+offset, mlen-offset, &p_req->Multicast);

	offset += snprintf(p_buf+offset, mlen-offset,
			"<tt:SessionTimeout>PT%dS</tt:SessionTimeout>",
	    	p_req->SessionTimeout);

    return offset;
}

int build_Profile_xml(char * p_buf, int mlen, ONVIF_PROFILE * p_profile)
{
	int offset = 0;
	
	if (p_profile->v_src_cfg)
    {
        offset += snprintf(p_buf+offset, mlen-offset, 
        	"<tt:VideoSourceConfiguration token=\"%s\">", 
            p_profile->v_src_cfg->Configuration.token);            
        offset += build_VideoSourceConfiguration_xml(p_buf+offset, mlen-offset, &p_profile->v_src_cfg->Configuration);    
        offset += snprintf(p_buf+offset, mlen-offset, "</tt:VideoSourceConfiguration>");	            
    }

#ifdef AUDIO_SUPPORT
    if (p_profile->a_src_cfg)
    {
        offset += snprintf(p_buf+offset, mlen-offset, 
            "<tt:AudioSourceConfiguration token=\"%s\">",
            p_profile->a_src_cfg->Configuration.token);
        offset += build_AudioSourceConfiguration_xml(p_buf+offset, mlen-offset, &p_profile->a_src_cfg->Configuration);
        offset += snprintf(p_buf+offset, mlen-offset, "</tt:AudioSourceConfiguration>");	            
    }
#endif

    if (p_profile->v_enc_cfg)
    {
        offset += snprintf(p_buf+offset, mlen-offset, 
        	"<tt:VideoEncoderConfiguration token=\"%s\">", 
        	p_profile->v_enc_cfg->Configuration.token);
		offset += build_VideoEncoderConfiguration_xml(p_buf+offset, mlen-offset, &p_profile->v_enc_cfg->Configuration);        	    
        offset += snprintf(p_buf+offset, mlen-offset, "</tt:VideoEncoderConfiguration>");	            
    }

#ifdef AUDIO_SUPPORT
    if (p_profile->a_enc_cfg)
    {
        offset += snprintf(p_buf+offset, mlen-offset, 
            "<tt:AudioEncoderConfiguration token=\"%s\">", 
            p_profile->a_enc_cfg->Configuration.token);
        offset += build_AudioEncoderConfiguration_xml(p_buf+offset, mlen-offset, p_profile->a_enc_cfg);
        offset += snprintf(p_buf+offset, mlen-offset, "</tt:AudioEncoderConfiguration>");	            
    }
#endif

#ifdef VIDEO_ANALYTICS
    if (p_profile->va_cfg)
    {
        offset += snprintf(p_buf+offset, mlen-offset, "<tt:Configurations token=\"%s\">", p_profile->va_cfg->Configuration.token);
		offset += build_VideoAnalyticsConfiguration_xml(p_buf+offset, mlen-offset, &p_profile->va_cfg->Configuration);
		offset += snprintf(p_buf+offset, mlen-offset, "</tt:Configurations>");
    }
#endif

#ifdef PTZ_SUPPORT
    if (p_profile->ptz_cfg)
    {
    	offset += snprintf(p_buf+offset, mlen-offset, 
            "<tt:PTZConfiguration token=\"%s\" MoveRamp=\"%d\" PresetRamp=\"%d\" PresetTourRamp=\"%d\">", 
            p_profile->ptz_cfg->Configuration.token, p_profile->ptz_cfg->Configuration.MoveRamp,
            p_profile->ptz_cfg->Configuration.PresetRamp, p_profile->ptz_cfg->Configuration.PresetTourRamp);
    	offset += build_PTZConfiguration_xml(p_buf+offset, mlen-offset, p_profile->ptz_cfg);
    	offset += snprintf(p_buf+offset, mlen-offset, "</tt:PTZConfiguration>");
    }
#endif

    if (p_profile->metadata_cfg)
    {
    	offset += snprintf(p_buf+offset, mlen-offset, 
            "<tt:MetadataConfiguration token=\"%s\">", 
            p_profile->metadata_cfg->Configuration.token);
        offset += build_MetadataConfiguration_xml(p_buf+offset, mlen-offset, &p_profile->metadata_cfg->Configuration);
        offset += snprintf(p_buf+offset, mlen-offset, "</tt:MetadataConfiguration>");    
    }

    offset += snprintf(p_buf+offset, mlen-offset, "<tt:Extension>");

#ifdef DEVICEIO_SUPPORT
    if (p_profile->a_output_cfg)
    {
        offset += snprintf(p_buf+offset, mlen-offset, 
            "<tt:AudioOutputConfiguration token=\"%s\">", 
            p_profile->a_output_cfg->Configuration.token);
        offset += build_AudioOutputConfiguration_xml(p_buf+offset, mlen-offset, &p_profile->a_output_cfg->Configuration);
        offset += snprintf(p_buf+offset, mlen-offset, "</tt:AudioOutputConfiguration>");
    }
#endif

#ifdef AUDIO_SUPPORT
    if (p_profile->a_dec_cfg)
    {
        offset += snprintf(p_buf+offset, mlen-offset, 
            "<tt:AudioDecoderConfiguration token=\"%s\">", 
            p_profile->a_dec_cfg->Configuration.token);
        offset += build_AudioDecoderConfiguration_xml(p_buf+offset, mlen-offset, &p_profile->a_dec_cfg->Configuration);
        offset += snprintf(p_buf+offset, mlen-offset, "</tt:AudioDecoderConfiguration>");
    }        
#endif

    offset += snprintf(p_buf+offset, mlen-offset, "</tt:Extension>");
    
    return offset;
}
    
int build_GetProfiles_rly_xml(char * p_buf, int mlen, const char * argv)
{
    int offset = 0;
    ONVIF_PROFILE * profile = g_onvif_cfg.profiles;

	offset += snprintf(p_buf+offset, mlen-offset, "<trt:GetProfilesResponse>");
	
	while (profile)
	{
	    offset += snprintf(p_buf+offset, mlen-offset, 
	        "<trt:Profiles fixed=\"%s\" token=\"%s\"><tt:Name>%s</tt:Name>",
	        profile->fixed ? "true" : "false", profile->token, profile->name);

	    offset += build_Profile_xml(p_buf+offset, mlen-offset, profile);
	    
	    offset += snprintf(p_buf+offset, mlen-offset, "</trt:Profiles>");

	    profile = profile->next;
    }
    
    offset += snprintf(p_buf+offset, mlen-offset, "</trt:GetProfilesResponse>");            

	return offset;
}

int build_GetProfile_rly_xml(char * p_buf, int mlen, const char * argv)
{	
	int offset = 0;
	GetProfile_REQ * p_req = (GetProfile_REQ *) argv;
    ONVIF_PROFILE * profile = onvif_find_profile(p_req->ProfileToken);
    if (NULL == profile)
    {
    	return ONVIF_ERR_NoProfile;
    }
	
	offset += snprintf(p_buf+offset, mlen-offset, "<trt:GetProfileResponse>");

    offset += snprintf(p_buf+offset, mlen-offset, 
        "<trt:Profile fixed=\"%s\" token=\"%s\"><tt:Name>%s</tt:Name>",
        profile->fixed ? "true" : "false", profile->token, profile->name);

   	offset += build_Profile_xml(p_buf+offset, mlen-offset, profile);
	    
    offset += snprintf(p_buf+offset, mlen-offset, "</trt:Profile>");    
    offset += snprintf(p_buf+offset, mlen-offset, "</trt:GetProfileResponse>"); 

	return offset;
}

int build_CreateProfile_rly_xml(char * p_buf, int mlen, const char * argv)
{
	int offset = 0;
	ONVIF_PROFILE * profile = onvif_find_profile(argv);
    if (NULL == profile)
    {
    	return -1;
    }
	
	offset += snprintf(p_buf+offset, mlen-offset, "<trt:CreateProfileResponse>");

    offset += snprintf(p_buf+offset, mlen-offset, 
        "<trt:Profile fixed=\"%s\" token=\"%s\"><tt:Name>%s</tt:Name>",
        profile->fixed ? "true" : "false", profile->token, profile->name);

    offset += build_Profile_xml(p_buf+offset, mlen-offset, profile);
    
    offset += snprintf(p_buf+offset, mlen-offset, "</trt:Profile>");

    
    offset += snprintf(p_buf+offset, mlen-offset, "</trt:CreateProfileResponse>");            

	return offset;
}

int build_DeleteProfile_rly_xml(char * p_buf, int mlen, const char * argv)
{
	int offset = 0;
	offset += snprintf(p_buf+offset, mlen-offset, "<trt:DeleteProfileResponse />");
	return offset;
}

int build_AddVideoSourceConfiguration_rly_xml(char * p_buf, int mlen, const char * argv)
{
	int offset = 0;
    offset += snprintf(p_buf+offset, mlen-offset, "<trt:AddVideoSourceConfigurationResponse />");
	return offset;
}

int build_RemoveVideoSourceConfiguration_rly_xml(char * p_buf, int mlen, const char * argv)
{
	int offset = 0;
    offset += snprintf(p_buf+offset, mlen-offset, "<trt:RemoveVideoSourceConfigurationResponse />");
	return offset;
}

int build_AddVideoEncoderConfiguration_rly_xml(char * p_buf, int mlen, const char * argv)
{
	int offset = 0;
	offset += snprintf(p_buf+offset, mlen-offset, "<trt:AddVideoEncoderConfigurationResponse />");
	return offset;
}

int build_RemoveVideoEncoderConfiguration_rly_xml(char * p_buf, int mlen, const char * argv)
{
	int offset = 0;
	offset += snprintf(p_buf+offset, mlen-offset, "<trt:RemoveVideoEncoderConfigurationResponse />");
	return offset;
}

int build_GetStreamUri_rly_xml(char * p_buf, int mlen, const char * argv)
{
	int offset = 0;
	GetStreamUri_RES * p_res = (GetStreamUri_RES *)argv;
    
    offset += snprintf(p_buf+offset, mlen-offset, 
    	"<trt:GetStreamUriResponse>"
		    "<trt:MediaUri>"
			    "<tt:Uri>%s</tt:Uri>"
			    "<tt:InvalidAfterConnect>%s</tt:InvalidAfterConnect>"
			    "<tt:InvalidAfterReboot>%s</tt:InvalidAfterReboot>"
			    "<tt:Timeout>PT%dS</tt:Timeout>"
		    "</trt:MediaUri>"
	    "</trt:GetStreamUriResponse>", 
		p_res->MediaUri.Uri,
	    p_res->MediaUri.InvalidAfterConnect ? "true" : "false",
	    p_res->MediaUri.InvalidAfterReboot ? "true" : "false",
	    p_res->MediaUri.Timeout);

    // onvif_print("======= build_GetStreamUri_rly_xml | rtspuri : %s =========\n", p_res->MediaUri.Uri);

	return offset;
}

int build_GetSnapshotUri_rly_xml(char * p_buf, int mlen, const char * argv)
{
	int offset = 0;
	GetSnapshotUri_RES * p_res = (GetSnapshotUri_RES *) argv;
    
	offset += snprintf(p_buf+offset, mlen-offset, 
        "<trt:GetSnapshotUriResponse>"
            "<trt:MediaUri>"
                "<tt:Uri>%s</tt:Uri>"
                "<tt:InvalidAfterConnect>%s</tt:InvalidAfterConnect>"
                "<tt:InvalidAfterReboot>%s</tt:InvalidAfterReboot>"
                "<tt:Timeout>PT%dS</tt:Timeout>"
            "</trt:MediaUri>"
        "</trt:GetSnapshotUriResponse>",
        p_res->MediaUri.Uri, 
        p_res->MediaUri.InvalidAfterConnect ? "true" : "false",
        p_res->MediaUri.InvalidAfterReboot ? "true" : "false",
        p_res->MediaUri.Timeout);
        
	return offset;
}

int build_MediaCapabilities_xml(char * p_buf, int mlen)
{
	int offset = 0;	
     
    offset += snprintf(p_buf+offset, mlen-offset, 
    	"<tt:Media>"
		    "<tt:XAddr>%s</tt:XAddr>"
		    "<tt:StreamingCapabilities>"
			    "<tt:RTPMulticast>%s</tt:RTPMulticast>"
			    "<tt:RTP_TCP>%s</tt:RTP_TCP>"
			    "<tt:RTP_RTSP_TCP>%s</tt:RTP_RTSP_TCP>"
		    "</tt:StreamingCapabilities>"
		    "<tt:Extension>"
				"<tt:ProfileCapabilities>"
					"<tt:MaximumNumberOfProfiles>%d</tt:MaximumNumberOfProfiles>"
				"</tt:ProfileCapabilities>"
			"</tt:Extension>"
    	"</tt:Media>",		    
    	g_onvif_cfg.Capabilities.media.XAddr,
    	g_onvif_cfg.Capabilities.media.RTPMulticast ? "true" : "false",
    	g_onvif_cfg.Capabilities.media.RTP_TCP ? "true" : "false",
    	g_onvif_cfg.Capabilities.media.RTP_RTSP_TCP ? "true" : "false",
    	g_onvif_cfg.Capabilities.media.MaximumNumberOfProfiles);

	return offset;    	
}

int build_DeviceCapabilities_xml(char * p_buf, int mlen)
{
#ifdef DEVICEIO_SUPPORT
    int i;
#endif

	int offset = 0;
	
    offset += snprintf(p_buf+offset, mlen-offset, 
    	"<tt:Device>"
    	"<tt:XAddr>%s</tt:XAddr>",
    	g_onvif_cfg.Capabilities.device.XAddr);    	
		    
	offset += snprintf(p_buf+offset, mlen-offset, 
		"<tt:Network>"
		    "<tt:IPFilter>%s</tt:IPFilter>"
		    "<tt:ZeroConfiguration>%s</tt:ZeroConfiguration>"
		    "<tt:IPVersion6>%s</tt:IPVersion6>"
		    "<tt:DynDNS>%s</tt:DynDNS>"
		    "<tt:Extension>"
		    	"<tt:Dot11Configuration>%s</tt:Dot11Configuration>"       
  			"</tt:Extension>"
	    "</tt:Network>",	    
    	g_onvif_cfg.Capabilities.device.IPFilter ? "true" : "false",
    	g_onvif_cfg.Capabilities.device.ZeroConfiguration ? "true" : "false",
    	g_onvif_cfg.Capabilities.device.IPVersion6 ? "true" : "false",
    	g_onvif_cfg.Capabilities.device.DynDNS ? "true" : "false",
    	g_onvif_cfg.Capabilities.device.Dot11Configuration ? "true" : "false");

	offset += snprintf(p_buf+offset, mlen-offset, 
		"<tt:System>"
		    "<tt:DiscoveryResolve>%s</tt:DiscoveryResolve>"
		    "<tt:DiscoveryBye>%s</tt:DiscoveryBye>"
		    "<tt:RemoteDiscovery>%s</tt:RemoteDiscovery>"
		    "<tt:SystemBackup>%s</tt:SystemBackup>"
		    "<tt:SystemLogging>%s</tt:SystemLogging>"
		    "<tt:FirmwareUpgrade>%s</tt:FirmwareUpgrade>"
		    "<tt:SupportedVersions>"
			    "<tt:Major>17</tt:Major>"
			    "<tt:Minor>12</tt:Minor>"
		    "</tt:SupportedVersions>"
		    "<tt:SupportedVersions>"
			    "<tt:Major>2</tt:Major>"
			    "<tt:Minor>6</tt:Minor>"
		    "</tt:SupportedVersions>"
		    "<tt:SupportedVersions>"
			    "<tt:Major>2</tt:Major>"
			    "<tt:Minor>4</tt:Minor>"
		    "</tt:SupportedVersions>"
		    "<tt:SupportedVersions>"
			    "<tt:Major>2</tt:Major>"
			    "<tt:Minor>0</tt:Minor>"
		    "</tt:SupportedVersions>"
		    "<tt:Extension>"
				"<tt:HttpFirmwareUpgrade>%s</tt:HttpFirmwareUpgrade>"
				"<tt:HttpSystemBackup>%s</tt:HttpSystemBackup>"
				"<tt:HttpSystemLogging>%s</tt:HttpSystemLogging>"
				"<tt:HttpSupportInformation>%s</tt:HttpSupportInformation>"
			"</tt:Extension>"
	    "</tt:System>",
	    g_onvif_cfg.Capabilities.device.DiscoveryResolve ? "true" : "false",
	    g_onvif_cfg.Capabilities.device.DiscoveryBye ? "true" : "false",
	    g_onvif_cfg.Capabilities.device.RemoteDiscovery ? "true" : "false",
	    g_onvif_cfg.Capabilities.device.SystemBackup ? "true" : "false",
	    g_onvif_cfg.Capabilities.device.SystemLogging ? "true" : "false",
	    g_onvif_cfg.Capabilities.device.FirmwareUpgrade ? "true" : "false",
	    g_onvif_cfg.Capabilities.device.HttpFirmwareUpgrade ? "true" : "false",
	    g_onvif_cfg.Capabilities.device.HttpSystemBackup ? "true" : "false",
	    g_onvif_cfg.Capabilities.device.HttpSystemLogging ? "true" : "false",
	    g_onvif_cfg.Capabilities.device.HttpSupportInformation ? "true" : "false");    

#ifdef DEVICEIO_SUPPORT
	offset += snprintf(p_buf+offset, mlen-offset, "<tt:IO>");
	offset += snprintf(p_buf+offset, mlen-offset, 
	    "<tt:InputConnectors>%d</tt:InputConnectors>"
		"<tt:RelayOutputs>%d</tt:RelayOutputs>",
		g_onvif_cfg.Capabilities.device.InputConnectors,
		g_onvif_cfg.Capabilities.device.RelayOutputs);
    offset += snprintf(p_buf+offset, mlen-offset, "<tt:Extension>");
    offset += snprintf(p_buf+offset, mlen-offset, 
        "<tt:Auxiliary>%s</tt:Auxiliary>", 
        g_onvif_cfg.Capabilities.device.Auxiliary ? "true" : "false");
        
    for  (i = 0; i < g_onvif_cfg.Capabilities.device.sizeAuxiliaryCommands; i++)
    {
        offset += snprintf(p_buf+offset, mlen-offset, 
        "<tt:AuxiliaryCommands>%s</tt:AuxiliaryCommands>", 
        g_onvif_cfg.Capabilities.device.AuxiliaryCommands[i]);
    }
    
    offset += snprintf(p_buf+offset, mlen-offset, "<tt:Extension />");
    offset += snprintf(p_buf+offset, mlen-offset, "</tt:Extension>");
	offset += snprintf(p_buf+offset, mlen-offset, "</tt:IO>");
#endif

	offset += snprintf(p_buf+offset, mlen-offset, 
		"<tt:Security>"
			"<tt:TLS1.1>%s</tt:TLS1.1>"
			"<tt:TLS1.2>%s</tt:TLS1.2>"
			"<tt:OnboardKeyGeneration>%s</tt:OnboardKeyGeneration>"
			"<tt:AccessPolicyConfig>%s</tt:AccessPolicyConfig>"
			"<tt:X.509Token>%s</tt:X.509Token>"
			"<tt:SAMLToken>%s</tt:SAMLToken>"
			"<tt:KerberosToken>%s</tt:KerberosToken>"
			"<tt:RELToken>%s</tt:RELToken>"
			"<tt:Extension>"
				"<tt:TLS1.0>%s</tt:TLS1.0>"
				"<tt:Extension>"
					"<tt:Dot1X>%s</tt:Dot1X>"					
					"<tt:SupportedEAPMethod>%d</tt:SupportedEAPMethod>"
					"<tt:RemoteUserHandling>%s</tt:RemoteUserHandling>"
				"</tt:Extension>"
			"</tt:Extension>"	
		"</tt:Security>",
		g_onvif_cfg.Capabilities.device.TLS11 ? "true" : "false",
		g_onvif_cfg.Capabilities.device.TLS12 ? "true" : "false",
		g_onvif_cfg.Capabilities.device.OnboardKeyGeneration ? "true" : "false",
		g_onvif_cfg.Capabilities.device.AccessPolicyConfig ? "true" : "false",
		g_onvif_cfg.Capabilities.device.X509Token ? "true" : "false",
		g_onvif_cfg.Capabilities.device.SAMLToken ? "true" : "false",
		g_onvif_cfg.Capabilities.device.KerberosToken ? "true" : "false",
		g_onvif_cfg.Capabilities.device.RELToken ? "true" : "false",
		g_onvif_cfg.Capabilities.device.TLS10 ? "true" : "false",
		g_onvif_cfg.Capabilities.device.Dot1X ? "true" : "false",
		g_onvif_cfg.Capabilities.device.SupportedEAPMethods,
		g_onvif_cfg.Capabilities.device.RemoteUserHandling ? "true" : "false");
		
	offset += snprintf(p_buf+offset, mlen-offset, "</tt:Device>");
	
	return offset;    	
}

int build_EventsCapabilities_xml(char * p_buf, int mlen)
{
	int offset = 0;	
     
    offset += snprintf(p_buf+offset, mlen-offset, 
    	"<tt:Events>"
		    "<tt:XAddr>%s</tt:XAddr>"
		    "<tt:WSSubscriptionPolicySupport>%s</tt:WSSubscriptionPolicySupport>"
		    "<tt:WSPullPointSupport>%s</tt:WSPullPointSupport>"
		    "<tt:WSPausableSubscriptionManagerInterfaceSupport>%s</tt:WSPausableSubscriptionManagerInterfaceSupport>"
	    "</tt:Events>",		    
    	g_onvif_cfg.Capabilities.events.XAddr,
    	g_onvif_cfg.Capabilities.events.WSSubscriptionPolicySupport ? "true" : "false",
    	g_onvif_cfg.Capabilities.events.WSPullPointSupport ? "true" : "false",
    	g_onvif_cfg.Capabilities.events.WSPausableSubscriptionManagerInterfaceSupport ? "true" : "false");

	return offset;    	
}

int build_ImagingCapabilities_xml(char * p_buf, int mlen)
{
	int offset = 0;	
     
    offset += snprintf(p_buf+offset, mlen-offset, 
    	"<tt:Imaging>"
    		"<tt:XAddr>%s</tt:XAddr>"
    	"</tt:Imaging>",		    
    	g_onvif_cfg.Capabilities.image.XAddr);

	return offset;    	
}

#ifdef PTZ_SUPPORT
int build_PTZCapabilities_xml(char * p_buf, int mlen)
{
	int offset = 0;	
     
    offset += snprintf(p_buf+offset, mlen-offset, 
    	"<tt:PTZ>"
    		"<tt:XAddr>%s</tt:XAddr>"
    	"</tt:PTZ>",		    
    	g_onvif_cfg.Capabilities.ptz.XAddr);

	return offset;    	
}
#endif

#ifdef VIDEO_ANALYTICS
int build_AnalyticsCapabilities_xml(char * p_buf, int mlen)
{
	int offset = 0;
     
    offset += snprintf(p_buf+offset, mlen-offset, 
    	"<tt:Analytics>"
    		"<tt:XAddr>%s</tt:XAddr>"
    		"<tt:RuleSupport>%s</tt:RuleSupport>"
		 	"<tt:AnalyticsModuleSupport>%s</tt:AnalyticsModuleSupport>"
    	"</tt:Analytics>",		    
    	g_onvif_cfg.Capabilities.analytics.XAddr,
    	g_onvif_cfg.Capabilities.analytics.RuleSupport ? "true" : "false",
    	g_onvif_cfg.Capabilities.analytics.AnalyticsModuleSupport ? "true" : "false");

	return offset;
}
#endif

#ifdef PROFILE_G_SUPPORT

int build_RecordingCapabilities_xml(char * p_buf, int mlen)
{
	int offset = 0;	
     
    offset += snprintf(p_buf+offset, mlen-offset, 
		"<tt:Recording>"
			"<tt:XAddr>%s</tt:XAddr>"
			"<tt:ReceiverSource>%s</tt:ReceiverSource>"
			"<tt:MediaProfileSource>%s</tt:MediaProfileSource>"
			"<tt:DynamicRecordings>%s</tt:DynamicRecordings>"
			"<tt:DynamicTracks>%s</tt:DynamicTracks>"
			"<tt:MaxStringLength>%d</tt:MaxStringLength>"
		"</tt:Recording>",		    
    	g_onvif_cfg.Capabilities.recording.XAddr,
    	g_onvif_cfg.Capabilities.recording.ReceiverSource ? "true" : "false",
    	g_onvif_cfg.Capabilities.recording.MediaProfileSource ? "true" : "false",
    	g_onvif_cfg.Capabilities.recording.DynamicRecordings ? "true" : "false", 
    	g_onvif_cfg.Capabilities.recording.DynamicTracks ? "true" : "false",
    	g_onvif_cfg.Capabilities.recording.MaxStringLength);

	return offset;    	
}

int build_SearchCapabilities_xml(char * p_buf, int mlen)
{
	int offset = 0;	
     
    offset += snprintf(p_buf+offset, mlen-offset, 
		"<tt:Search>"
			"<tt:XAddr>%s</tt:XAddr>"
			"<tt:MetadataSearch>%s</tt:MetadataSearch>"
		"</tt:Search>",		    
    	g_onvif_cfg.Capabilities.search.XAddr,
    	g_onvif_cfg.Capabilities.search.MetadataSearch ? "true" : "false");

	return offset;    	
}

int build_ReplayCapabilities_xml(char * p_buf, int mlen)
{
	int offset = 0;	
     
    offset += snprintf(p_buf+offset, mlen-offset, 
		"<tt:Replay>"
			"<tt:XAddr>%s</tt:XAddr>"
		"</tt:Replay>",		    
    	g_onvif_cfg.Capabilities.replay.XAddr);

	return offset;
}

#endif // end of PROFILE_G_SUPPORT

#ifdef DEVICEIO_SUPPORT
int build_DeviceIOCapabilities_xml(char * p_buf, int mlen)
{
	int offset = 0;	
     
    offset += snprintf(p_buf+offset, mlen-offset, "<tt:DeviceIO>");
    offset += snprintf(p_buf+offset, mlen-offset, 
    	"<tt:XAddr>%s</tt:XAddr>", 
    	g_onvif_cfg.Capabilities.deviceIO.XAddr);
    	
	offset += snprintf(p_buf+offset, mlen-offset, 
		"<tt:VideoSources>%d</tt:VideoSources>"
		"<tt:VideoOutputs>%d</tt:VideoOutputs>"
		"<tt:AudioSources>%d</tt:AudioSources>"
		"<tt:AudioOutputs>%d</tt:AudioOutputs>"
		"<tt:RelayOutputs>%d</tt:RelayOutputs>",
		g_onvif_cfg.Capabilities.deviceIO.VideoSources,
		g_onvif_cfg.Capabilities.deviceIO.VideoOutputs,
		g_onvif_cfg.Capabilities.deviceIO.AudioSources,
		g_onvif_cfg.Capabilities.deviceIO.AudioOutputs,
		g_onvif_cfg.Capabilities.deviceIO.RelayOutputs);

	offset += snprintf(p_buf+offset, mlen-offset, "</tt:DeviceIO>");

	return offset;
}
#endif // end of DEVICEIO_SUPPORT

#ifdef RECEIVER_SUPPORT
int build_ReceiverCapabilities_xml(char * p_buf, int mlen)
{
    int offset = 0;	
     
    offset += snprintf(p_buf+offset, mlen-offset, "<tt:Receiver>");
    offset += snprintf(p_buf+offset, mlen-offset, 
    	"<tt:XAddr>%s</tt:XAddr>", 
    	g_onvif_cfg.Capabilities.receiver.XAddr);
    	
	offset += snprintf(p_buf+offset, mlen-offset, 
		"<tt:RTP_Multicast>%s</tt:RTP_Multicast>"
		"<tt:RTP_TCP>%s</tt:RTP_TCP>"
		"<tt:RTP_RTSP_TCP>%s</tt:RTP_RTSP_TCP>"
		"<tt:SupportedReceivers>%d</tt:SupportedReceivers>"
		"<tt:MaximumRTSPURILength>%d</tt:MaximumRTSPURILength>",
		g_onvif_cfg.Capabilities.receiver.RTP_USCOREMulticast ? "true" : "false",
		g_onvif_cfg.Capabilities.receiver.RTP_USCORETCP ? "true" : "false",
		g_onvif_cfg.Capabilities.receiver.RTP_USCORERTSP_USCORETCP ? "true" : "false",
		g_onvif_cfg.Capabilities.receiver.SupportedReceivers,
		g_onvif_cfg.Capabilities.receiver.MaximumRTSPURILength);

	offset += snprintf(p_buf+offset, mlen-offset, "</tt:Receiver>");

	return offset;
}
#endif // end of RECEIVER_SUPPORT

int build_GetCapabilities_rly_xml(char * p_buf, int mlen, const char * argv)
{
    int offset = 0;
	GetCapabilities_REQ * p_req = (GetCapabilities_REQ *)argv;
	
	offset += snprintf(p_buf+offset, mlen-offset, "<tds:GetCapabilitiesResponse><tds:Capabilities>");

	if (CapabilityCategory_Media == p_req->Category)
	{
	    offset += build_MediaCapabilities_xml(p_buf+offset, mlen-offset);
	}
	else if (CapabilityCategory_Device == p_req->Category)
	{
	    offset += build_DeviceCapabilities_xml(p_buf+offset, mlen-offset);
	}
    else if (CapabilityCategory_Events == p_req->Category)
    {
        offset += build_EventsCapabilities_xml(p_buf+offset, mlen-offset);
    }
    else if (CapabilityCategory_Imaging == p_req->Category)
    {
        offset += build_ImagingCapabilities_xml(p_buf+offset, mlen-offset);
    }
#ifdef PTZ_SUPPORT    
    else if (CapabilityCategory_PTZ == p_req->Category)
    {
        offset += build_PTZCapabilities_xml(p_buf+offset, mlen-offset);
    }
#endif    
#ifdef VIDEO_ANALYTICS
	else if (CapabilityCategory_Analytics == p_req->Category)
    {
        offset += build_AnalyticsCapabilities_xml(p_buf+offset, mlen-offset);
    }
#endif    
	else if (CapabilityCategory_All == p_req->Category)
	{
#ifdef VIDEO_ANALYTICS
		offset += build_AnalyticsCapabilities_xml(p_buf+offset, mlen-offset);
#endif	
	    offset += build_DeviceCapabilities_xml(p_buf+offset, mlen-offset);
	    offset += build_EventsCapabilities_xml(p_buf+offset, mlen-offset);
	    offset += build_ImagingCapabilities_xml(p_buf+offset, mlen-offset);
	    offset += build_MediaCapabilities_xml(p_buf+offset, mlen-offset);	
#ifdef PTZ_SUPPORT	    
	    offset += build_PTZCapabilities_xml(p_buf+offset, mlen-offset);
#endif
	    offset += snprintf(p_buf+offset, mlen-offset, "<tt:Extension>");
#ifdef DEVICEIO_SUPPORT
		if (g_onvif_cfg.Capabilities.deviceIO.support)
	    {
	    	offset += build_DeviceIOCapabilities_xml(p_buf+offset, mlen-offset);
	    }
#endif
#ifdef PROFILE_G_SUPPORT	    
	    if (g_onvif_cfg.Capabilities.recording.support)
	    {
	    	offset += build_RecordingCapabilities_xml(p_buf+offset, mlen-offset);
	    }
	    if (g_onvif_cfg.Capabilities.search.support)
	    {
	    	offset += build_SearchCapabilities_xml(p_buf+offset, mlen-offset);
	    }
	    if (g_onvif_cfg.Capabilities.replay.support)
	    {
	    	offset += build_ReplayCapabilities_xml(p_buf+offset, mlen-offset);
	    }
#endif
#ifdef RECEIVER_SUPPORT
        if (g_onvif_cfg.Capabilities.receiver.support)
	    {
	    	offset += build_ReceiverCapabilities_xml(p_buf+offset, mlen-offset);
	    }
#endif
	    offset += snprintf(p_buf+offset, mlen-offset, "</tt:Extension>");
	}
	
	offset += snprintf(p_buf+offset, mlen-offset, "</tds:Capabilities></tds:GetCapabilitiesResponse>");
	
	return offset;
}

int build_Dot11Configuration_xml(char * p_buf, int mlen, onvif_Dot11Configuration * p_req)
{
    int offset = 0;

    offset += snprintf(p_buf+offset, mlen-offset, 
        "<tt:SSID>%s</tt:SSID>"
        "<tt:Mode>%s</tt:Mode>"
        "<tt:Alias>%s</tt:Alias>"
        "<tt:Priority>%d</tt:Priority>", 
        p_req->SSID,
        onvif_Dot11StationModeToString(p_req->Mode),
        p_req->Alias,
        p_req->Priority);

    offset += snprintf(p_buf+offset, mlen-offset, "<tt:Security>");
    
    offset += snprintf(p_buf+offset, mlen-offset, "<tt:Mode>%s</tt:Mode>", 
        onvif_Dot11SecurityModeToString(p_req->Security.Mode));

    if (p_req->Security.AlgorithmFlag)
    {
        offset += snprintf(p_buf+offset, mlen-offset, "<tt:Algorithm>%s</tt:Algorithm>", 
            onvif_Dot11CipherToString(p_req->Security.Algorithm));
    }

    if (p_req->Security.PSKFlag)
    {
        offset += snprintf(p_buf+offset, mlen-offset, "<tt:PSK>");
        
        if (p_req->Security.PSK.KeyFlag)
        {
            offset += snprintf(p_buf+offset, mlen-offset, "<tt:Key>%s</tt:Key>", p_req->Security.PSK.Key);
        }

        if (p_req->Security.PSK.PassphraseFlag)
        {
            offset += snprintf(p_buf+offset, mlen-offset, "<tt:Passphrase>%s</tt:Passphrase>", p_req->Security.PSK.Passphrase);
        }
        
        offset += snprintf(p_buf+offset, mlen-offset, "</tt:PSK>");
    }

    if (p_req->Security.Dot1XFlag)
    {
        offset += snprintf(p_buf+offset, mlen-offset, "<tt:Dot1X>%s</tt:Dot1X>", p_req->Security.Dot1X);
    }
    
    offset += snprintf(p_buf+offset, mlen-offset, "</tt:Security>");
    return offset;
}

int build_NetworkInterface_xml(char * p_buf, int mlen, onvif_NetworkInterface * p_req)
{
    int i;
    int offset = 0;

    offset += snprintf(p_buf+offset, mlen-offset, "<tt:Enabled>%s</tt:Enabled>", p_req->Enabled ? "true" : "false");
		
	if (p_req->InfoFlag)
	{
	    offset += snprintf(p_buf+offset, mlen-offset, "<tt:Info>");
		if (p_req->Info.NameFlag)
		{
		    offset += snprintf(p_buf+offset, mlen-offset, "<tt:Name>%s</tt:Name>", p_req->Info.Name);
		}    
		offset += snprintf(p_buf+offset, mlen-offset, "<tt:HwAddress>%s</tt:HwAddress>", p_req->Info.HwAddress);
		if (p_req->Info.MTUFlag)
		{
			offset += snprintf(p_buf+offset, mlen-offset, "<tt:MTU>%d</tt:MTU>", p_req->Info.MTU);
		}	
	    offset += snprintf(p_buf+offset, mlen-offset, "</tt:Info>");
	}

	if (p_req->IPv4Flag)
	{
        offset += snprintf(p_buf+offset, mlen-offset, "<tt:IPv4>");
        offset += snprintf(p_buf+offset, mlen-offset, "<tt:Enabled>%s</tt:Enabled>", p_req->IPv4.Enabled ? "true" : "false");
        offset += snprintf(p_buf+offset, mlen-offset, "<tt:Config>");

		if (p_req->IPv4.Config.DHCP == FALSE)
		{
		    offset += snprintf(p_buf+offset, mlen-offset, 
		        "<tt:Manual>"
			        "<tt:Address>%s</tt:Address>"
			        "<tt:PrefixLength>%d</tt:PrefixLength>"
		        "</tt:Manual>",
		        p_req->IPv4.Config.Address, 
		        p_req->IPv4.Config.PrefixLength);				
		}
		else
		{
		    offset += snprintf(p_buf+offset, mlen-offset, 
		        "<tt:FromDHCP>"
			        "<tt:Address>%s</tt:Address>"
			        "<tt:PrefixLength>%d</tt:PrefixLength>"
		        "</tt:FromDHCP>",
		        p_req->IPv4.Config.Address, 
		        p_req->IPv4.Config.PrefixLength);
		}
		
		offset += snprintf(p_buf+offset, mlen-offset, "<tt:DHCP>%s</tt:DHCP>", p_req->IPv4.Config.DHCP ? "true" : "false");

		offset += snprintf(p_buf+offset, mlen-offset, "</tt:Config>");
		offset += snprintf(p_buf+offset, mlen-offset, "</tt:IPv4>");
	}

    if (p_req->ExtensionFlag)
    {
        offset += snprintf(p_buf+offset, mlen-offset, "<tt:Extension>");
        offset += snprintf(p_buf+offset, mlen-offset, "<tt:InterfaceType>%d</tt:InterfaceType>", p_req->Extension.InterfaceType);

        for (i = 0; i < p_req->Extension.sizeDot11; i++)
        {
            offset += snprintf(p_buf+offset, mlen-offset, "<tt:Dot11>");
            offset += build_Dot11Configuration_xml(p_buf+offset, mlen-offset, &p_req->Extension.Dot11[i]);
            offset += snprintf(p_buf+offset, mlen-offset, "</tt:Dot11>");
        }

        offset += snprintf(p_buf+offset, mlen-offset, "</tt:Extension>");
    }
    
    return offset;
}

int build_GetNetworkInterfaces_rly_xml(char * p_buf, int mlen, const char * argv)
{
    int offset = 0;
	ONVIF_NetworkInterface * p_net_inf = g_onvif_cfg.network.interfaces;
	
	offset += snprintf(p_buf+offset, mlen-offset, "<tds:GetNetworkInterfacesResponse>");
	
	while (p_net_inf)
	{
		offset += snprintf(p_buf+offset, mlen-offset, "<tds:NetworkInterfaces token=\"%s\">", p_net_inf->NetworkInterface.token);			
		offset += build_NetworkInterface_xml(p_buf+offset, mlen-offset, &p_net_inf->NetworkInterface);
		offset += snprintf(p_buf+offset, mlen-offset, "</tds:NetworkInterfaces>");
		
		p_net_inf = p_net_inf->next;
	}

	offset += snprintf(p_buf+offset, mlen-offset, "</tds:GetNetworkInterfacesResponse>");		
	
	return offset;
}

int build_SetNetworkInterfaces_rly_xml(char * p_buf, int mlen, const char * argv)
{
	int offset = 0;

	offset += snprintf(p_buf+offset, mlen-offset, 
		"<tds:SetNetworkInterfacesResponse>"
			"<tds:RebootNeeded>false</tds:RebootNeeded>"
		"</tds:SetNetworkInterfacesResponse>");		
	
	return offset;
}

int build_ImageSettings_xml(char * p_buf, int mlen)
{
    int offset = 0;
    
    if (g_onvif_cfg.ImagingSettings.BacklightCompensationFlag)
	{
		offset += snprintf(p_buf+offset, mlen-offset, "<tt:BacklightCompensation>");
		offset += snprintf(p_buf+offset, mlen-offset, "<tt:Mode>%s</tt:Mode>", onvif_BacklightCompensationModeToString(g_onvif_cfg.ImagingSettings.BacklightCompensation.Mode));
		if (g_onvif_cfg.ImagingSettings.BacklightCompensation.LevelFlag)
		{
			offset += snprintf(p_buf+offset, mlen-offset, "<tt:Level>%0.1f</tt:Level>", g_onvif_cfg.ImagingSettings.BacklightCompensation.Level);
		}	
		offset += snprintf(p_buf+offset, mlen-offset, "</tt:BacklightCompensation>");
	}

	if (g_onvif_cfg.ImagingSettings.BrightnessFlag)
	{
		offset += snprintf(p_buf+offset, mlen-offset, "<tt:Brightness>%0.1f</tt:Brightness>", g_onvif_cfg.ImagingSettings.Brightness);
	}
	if (g_onvif_cfg.ImagingSettings.ColorSaturationFlag)
	{
		offset += snprintf(p_buf+offset, mlen-offset, "<tt:ColorSaturation>%0.1f</tt:ColorSaturation>", g_onvif_cfg.ImagingSettings.ColorSaturation);
	}
	if (g_onvif_cfg.ImagingSettings.ContrastFlag)
	{
		offset += snprintf(p_buf+offset, mlen-offset, "<tt:Contrast>%0.1f</tt:Contrast>", g_onvif_cfg.ImagingSettings.Contrast);
	}

	if (g_onvif_cfg.ImagingSettings.ExposureFlag)
	{
		offset += snprintf(p_buf+offset, mlen-offset, "<tt:Exposure>");
		offset += snprintf(p_buf+offset, mlen-offset, "<tt:Mode>%s</tt:Mode>", onvif_ExposureModeToString(g_onvif_cfg.ImagingSettings.Exposure.Mode));
		if (g_onvif_cfg.ImagingSettings.Exposure.PriorityFlag)
		{
			offset += snprintf(p_buf+offset, mlen-offset, "<tt:Priority>%s</tt:Priority>", onvif_ExposurePriorityToString(g_onvif_cfg.ImagingSettings.Exposure.Priority));
		}

        offset += snprintf(p_buf+offset, mlen-offset, "<tt:Window bottom=\"%0.1f\" top=\"%0.1f\" right=\"%0.1f\" left=\"%0.1f\"></tt:Window>",
            g_onvif_cfg.ImagingSettings.Exposure.Window.bottom, g_onvif_cfg.ImagingSettings.Exposure.Window.top,
            g_onvif_cfg.ImagingSettings.Exposure.Window.right, g_onvif_cfg.ImagingSettings.Exposure.Window.left);
		
		if (g_onvif_cfg.ImagingSettings.Exposure.MinExposureTimeFlag)
		{
			offset += snprintf(p_buf+offset, mlen-offset, "<tt:MinExposureTime>%0.1f</tt:MinExposureTime>", g_onvif_cfg.ImagingSettings.Exposure.MinExposureTime);
		}
		if (g_onvif_cfg.ImagingSettings.Exposure.MaxExposureTimeFlag)
		{
			offset += snprintf(p_buf+offset, mlen-offset, "<tt:MaxExposureTime>%0.1f</tt:MaxExposureTime>", g_onvif_cfg.ImagingSettings.Exposure.MaxExposureTime);
		}
		if (g_onvif_cfg.ImagingSettings.Exposure.MinGainFlag)
		{
			offset += snprintf(p_buf+offset, mlen-offset, "<tt:MinGain>%0.1f</tt:MinGain>", g_onvif_cfg.ImagingSettings.Exposure.MinGain);
		}
		if (g_onvif_cfg.ImagingSettings.Exposure.MaxGainFlag)
		{
			offset += snprintf(p_buf+offset, mlen-offset, "<tt:MaxGain>%0.1f</tt:MaxGain>", g_onvif_cfg.ImagingSettings.Exposure.MaxGain);
		}
		if (g_onvif_cfg.ImagingSettings.Exposure.MinIrisFlag)
		{
			offset += snprintf(p_buf+offset, mlen-offset, "<tt:MinIris>%0.1f</tt:MinIris>", g_onvif_cfg.ImagingSettings.Exposure.MinIris);
		}
		if (g_onvif_cfg.ImagingSettings.Exposure.MaxIrisFlag)
		{
	    	offset += snprintf(p_buf+offset, mlen-offset, "<tt:MaxIris>%0.1f</tt:MaxIris>", g_onvif_cfg.ImagingSettings.Exposure.MaxIris);
	    }	
	    if (g_onvif_cfg.ImagingSettings.Exposure.ExposureTimeFlag)
	    {
	    	offset += snprintf(p_buf+offset, mlen-offset, "<tt:ExposureTime>%0.1f</tt:ExposureTime>", g_onvif_cfg.ImagingSettings.Exposure.ExposureTime);
	    }	
	    if (g_onvif_cfg.ImagingSettings.Exposure.GainFlag)
	    {
	    	offset += snprintf(p_buf+offset, mlen-offset, "<tt:Gain>%0.1f</tt:Gain>", g_onvif_cfg.ImagingSettings.Exposure.Gain);
	    }	
	    if (g_onvif_cfg.ImagingSettings.Exposure.IrisFlag)
	    {
	    	offset += snprintf(p_buf+offset, mlen-offset, "<tt:Iris>%0.1f</tt:Iris>", g_onvif_cfg.ImagingSettings.Exposure.Iris);
	    }
		offset += snprintf(p_buf+offset, mlen-offset, "</tt:Exposure>");			
	}

	if (g_onvif_cfg.ImagingSettings.FocusFlag)
	{
		offset += snprintf(p_buf+offset, mlen-offset, "<tt:Focus>");
	    offset += snprintf(p_buf+offset, mlen-offset, "<tt:AutoFocusMode>%s</tt:AutoFocusMode>", onvif_AutoFocusModeToString(g_onvif_cfg.ImagingSettings.Focus.AutoFocusMode));
	    if (g_onvif_cfg.ImagingSettings.Focus.DefaultSpeedFlag)
	    {
	    	offset += snprintf(p_buf+offset, mlen-offset, "<tt:DefaultSpeed>%0.1f</tt:DefaultSpeed>", g_onvif_cfg.ImagingSettings.Focus.DefaultSpeed);
	    }
	    if (g_onvif_cfg.ImagingSettings.Focus.NearLimitFlag)
	    {
	    	offset += snprintf(p_buf+offset, mlen-offset, "<tt:NearLimit>%0.1f</tt:NearLimit>", g_onvif_cfg.ImagingSettings.Focus.NearLimit);
	    }
	    if (g_onvif_cfg.ImagingSettings.Focus.FarLimitFlag)
	    {
	    	offset += snprintf(p_buf+offset, mlen-offset, "<tt:FarLimit>%0.1f</tt:FarLimit>", g_onvif_cfg.ImagingSettings.Focus.FarLimit);
	    }	
	    offset += snprintf(p_buf+offset, mlen-offset, "</tt:Focus>");
    }

    if (g_onvif_cfg.ImagingSettings.IrCutFilterFlag)
    {
		offset += snprintf(p_buf+offset, mlen-offset, "<tt:IrCutFilter>%s</tt:IrCutFilter>", onvif_IrCutFilterModeToString(g_onvif_cfg.ImagingSettings.IrCutFilter));
	}

	if (g_onvif_cfg.ImagingSettings.SharpnessFlag)
	{
		offset += snprintf(p_buf+offset, mlen-offset, "<tt:Sharpness>%0.1f</tt:Sharpness>", g_onvif_cfg.ImagingSettings.Sharpness);
	}

	if (g_onvif_cfg.ImagingSettings.WideDynamicRangeFlag)
	{
		offset += snprintf(p_buf+offset, mlen-offset, "<tt:WideDynamicRange>");
		offset += snprintf(p_buf+offset, mlen-offset, "<tt:Mode>%s</tt:Mode>", onvif_WideDynamicModeToString(g_onvif_cfg.ImagingSettings.WideDynamicRange.Mode));
		if (g_onvif_cfg.ImagingSettings.WideDynamicRange.LevelFlag)
		{
			offset += snprintf(p_buf+offset, mlen-offset, "<tt:Level>%0.1f</tt:Level>", g_onvif_cfg.ImagingSettings.WideDynamicRange.Level);
		}	
		offset += snprintf(p_buf+offset, mlen-offset, "</tt:WideDynamicRange>");	
	}

	if (g_onvif_cfg.ImagingSettings.WhiteBalanceFlag)
	{
		offset += snprintf(p_buf+offset, mlen-offset, "<tt:WhiteBalance>");
		offset += snprintf(p_buf+offset, mlen-offset, "<tt:Mode>%s</tt:Mode>", onvif_WhiteBalanceModeToString(g_onvif_cfg.ImagingSettings.WhiteBalance.Mode));
		if (g_onvif_cfg.ImagingSettings.WhiteBalance.CrGainFlag)
		{
			offset += snprintf(p_buf+offset, mlen-offset, "<tt:CrGain>%0.1f</tt:CrGain>", g_onvif_cfg.ImagingSettings.WhiteBalance.CrGain);
		}
		if (g_onvif_cfg.ImagingSettings.WhiteBalance.CbGainFlag)
		{
	    	offset += snprintf(p_buf+offset, mlen-offset, "<tt:CbGain>%0.1f</tt:CbGain>", g_onvif_cfg.ImagingSettings.WhiteBalance.CbGain);
	    }	
		offset += snprintf(p_buf+offset, mlen-offset, "</tt:WhiteBalance>");	
	}

	//// 扩展 add by xieqingpu
	if ( g_onvif_cfg.ImagingSettings.ThermalSettings_extFlag ){
		offset += snprintf(p_buf+offset, mlen-offset, "<tt:Extension>");
		offset += snprintf(p_buf+offset, mlen-offset, "<tt:ThermalSettings>");

		//热成像第一个设置
		if ( g_onvif_cfg.ImagingSettings.ThermalSettings.ThermalSet_ext1Flag ){
			offset += snprintf(p_buf+offset, mlen-offset, "<tt:UserPalette>%d</tt:UserPalette>", g_onvif_cfg.ImagingSettings.ThermalSettings.ThermalSet1.UserPalette);
			offset += snprintf(p_buf+offset, mlen-offset, "<tt:WideDynamic>%d</tt:WideDynamic>", g_onvif_cfg.ImagingSettings.ThermalSettings.ThermalSet1.WideDynamic);
			offset += snprintf(p_buf+offset, mlen-offset, "<tt:OrgData>%d</tt:OrgData>", g_onvif_cfg.ImagingSettings.ThermalSettings.ThermalSet1.OrgData);
			offset += snprintf(p_buf+offset, mlen-offset, "<tt:Actime>%d</tt:Actime>", g_onvif_cfg.ImagingSettings.ThermalSettings.ThermalSet1.Actime);
		}
		//热成像第二个设置
		if (g_onvif_cfg.ImagingSettings.ThermalSettings.ThermalSet_ext2Flag){
			offset += snprintf(p_buf+offset, mlen-offset, "<tt:Emissivity>%0.2f</tt:Emissivity>", g_onvif_cfg.ImagingSettings.ThermalSettings.ThermalSet2.Emissivity);
			offset += snprintf(p_buf+offset, mlen-offset, "<tt:Distance>%0.2f</tt:Distance>", g_onvif_cfg.ImagingSettings.ThermalSettings.ThermalSet2.Distance);
			offset += snprintf(p_buf+offset, mlen-offset, "<tt:Humidity>%0.2f</tt:Humidity>", g_onvif_cfg.ImagingSettings.ThermalSettings.ThermalSet2.Humidity);
			offset += snprintf(p_buf+offset, mlen-offset, "<tt:Correction>%0.2f</tt:Correction>", g_onvif_cfg.ImagingSettings.ThermalSettings.ThermalSet2.Correction);
			offset += snprintf(p_buf+offset, mlen-offset, "<tt:Reflection>%0.2f</tt:Reflection>", g_onvif_cfg.ImagingSettings.ThermalSettings.ThermalSet2.Reflection);
			offset += snprintf(p_buf+offset, mlen-offset, "<tt:Amb>%0.2f</tt:Amb>", g_onvif_cfg.ImagingSettings.ThermalSettings.ThermalSet2.Amb);
		}
		offset += snprintf(p_buf+offset, mlen-offset, "</tt:ThermalSettings>");	
		
		//Dula的设置数据
		if ( g_onvif_cfg.ImagingSettings.DulaInformationFlag ){
			offset += snprintf(p_buf+offset, mlen-offset, "<tt:DulaInfoSettings>");
			offset += snprintf(p_buf+offset, mlen-offset, "<tt:Focal>%d</tt:Focal>", g_onvif_cfg.ImagingSettings.DulaInfo.focal);
			offset += snprintf(p_buf+offset, mlen-offset, "<tt:Lens>%0.2f</tt:Lens>", g_onvif_cfg.ImagingSettings.DulaInfo.lens);
			offset += snprintf(p_buf+offset, mlen-offset, "<tt:Distance>%0.2f</tt:Distance>", g_onvif_cfg.ImagingSettings.DulaInfo.distance);
			offset += snprintf(p_buf+offset, mlen-offset, "<tt:DulaModel>%d</tt:DulaModel>", g_onvif_cfg.ImagingSettings.DulaInfo.dula_model);
			offset += snprintf(p_buf+offset, mlen-offset, "<tt:X>%d</tt:X>", g_onvif_cfg.ImagingSettings.DulaInfo.x);
			offset += snprintf(p_buf+offset, mlen-offset, "<tt:Y>%d</tt:Y>", g_onvif_cfg.ImagingSettings.DulaInfo.y);
			offset += snprintf(p_buf+offset, mlen-offset, "<tt:Scale>%0.2f</tt:Scale>", g_onvif_cfg.ImagingSettings.DulaInfo.scale);
			offset += snprintf(p_buf+offset, mlen-offset, "</tt:DulaInfoSettings>");	
		}

		offset += snprintf(p_buf+offset, mlen-offset, "</tt:Extension>");	
	}
	////

	return offset;
}

int build_GetVideoSources_rly_xml(char * p_buf, int mlen, const char * argv)
{
    int offset = 0;
	ONVIF_VideoSource * p_v_src = g_onvif_cfg.v_src;
	
	offset += snprintf(p_buf+offset, mlen-offset, "<trt:GetVideoSourcesResponse>");

	while (p_v_src)
	{
	    offset += snprintf(p_buf+offset, mlen-offset, 
	    	"<trt:VideoSources token=\"%s\">"
		    	"<tt:Framerate>%0.1f</tt:Framerate>"
			    "<tt:Resolution>"
				    "<tt:Width>%d</tt:Width>"
					"<tt:Height>%d</tt:Height>"
				"</tt:Resolution>", 
			p_v_src->VideoSource.token, 
			p_v_src->VideoSource.Framerate, 
			p_v_src->VideoSource.Resolution.Width, 
			p_v_src->VideoSource.Resolution.Height); 
			
		offset += snprintf(p_buf+offset, mlen-offset, "<tt:Imaging>");
		offset += build_ImageSettings_xml(p_buf+offset, mlen-offset);
		offset += snprintf(p_buf+offset, mlen-offset, "</tt:Imaging>");
		
		offset += snprintf(p_buf+offset, mlen-offset, "</trt:VideoSources>");
	    
	    p_v_src = p_v_src->next;
	}
	
	offset += snprintf(p_buf+offset, mlen-offset, "</trt:GetVideoSourcesResponse>");
	
	return offset;
}

int build_GetVideoEncoderConfiguration_rly_xml(char * p_buf, int mlen, const char * token)
{
	int offset = 0;
	ONVIF_VideoEncoder2Configuration * p_v_enc_cfg = onvif_find_VideoEncoderConfiguration(token);
    if (NULL == p_v_enc_cfg)
    {
    	return ONVIF_ERR_NoConfig;
    }
	
	offset += snprintf(p_buf+offset, mlen-offset, "<trt:GetVideoEncoderConfigurationResponse>");

    offset += snprintf(p_buf+offset, mlen-offset, "<trt:Configuration token=\"%s\">", p_v_enc_cfg->Configuration.token);
    offset += build_VideoEncoderConfiguration_xml(p_buf+offset, mlen-offset, &p_v_enc_cfg->Configuration);	
	offset += snprintf(p_buf+offset, mlen-offset, "</trt:Configuration>");
	
	offset += snprintf(p_buf+offset, mlen-offset, "</trt:GetVideoEncoderConfigurationResponse>");

	return offset;
}

int build_GetVideoEncoderConfigurations_rly_xml(char * p_buf, int mlen, const char * argv)
{
    int offset = 0;
    ONVIF_VideoEncoder2Configuration * p_v_enc_cfg = g_onvif_cfg.v_enc_cfg;
	
	offset += snprintf(p_buf+offset, mlen-offset, "<trt:GetVideoEncoderConfigurationsResponse>");

	while (p_v_enc_cfg)
	{
	    offset += snprintf(p_buf+offset, mlen-offset, "<trt:Configurations token=\"%s\">", p_v_enc_cfg->Configuration.token);
    	offset += build_VideoEncoderConfiguration_xml(p_buf+offset, mlen-offset, &p_v_enc_cfg->Configuration);
    	offset += snprintf(p_buf+offset, mlen-offset, "</trt:Configurations>");
    	
    	p_v_enc_cfg = p_v_enc_cfg->next;
	}
	
	offset += snprintf(p_buf+offset, mlen-offset, "</trt:GetVideoEncoderConfigurationsResponse>");

	return offset;
}

int build_GetCompatibleVideoEncoderConfigurations_rly_xml(char * p_buf, int mlen, const char * argv)
{
	int offset = 0;
	ONVIF_VideoEncoder2Configuration * p_v_enc_cfg;
	ONVIF_PROFILE * p_profile = onvif_find_profile(argv);
	if (NULL == p_profile)
	{
		return ONVIF_ERR_NoProfile;
	}
	
	p_v_enc_cfg = g_onvif_cfg.v_enc_cfg;
    
	offset += snprintf(p_buf+offset, mlen-offset, "<trt:GetCompatibleVideoEncoderConfigurationsResponse>");

	while (p_v_enc_cfg)
	{
	    offset += snprintf(p_buf+offset, mlen-offset, "<trt:Configurations token=\"%s\">", p_v_enc_cfg->Configuration.token);
    	offset += build_VideoEncoderConfiguration_xml(p_buf+offset, mlen-offset, &p_v_enc_cfg->Configuration);
    	offset += snprintf(p_buf+offset, mlen-offset, "</trt:Configurations>");
    	
    	p_v_enc_cfg = p_v_enc_cfg->next;
	}
	
	offset += snprintf(p_buf+offset, mlen-offset, "</trt:GetCompatibleVideoEncoderConfigurationsResponse>");

	return offset;
}

int build_GetVideoSourceConfigurations_rly_xml(char * p_buf, int mlen, const char * argv)
{
    int offset = 0;
    ONVIF_VideoSourceConfiguration * p_v_src_cfg = g_onvif_cfg.v_src_cfg;
    
	offset += snprintf(p_buf+offset, mlen-offset, "<trt:GetVideoSourceConfigurationsResponse>");

	while (p_v_src_cfg)
	{
	    offset += snprintf(p_buf+offset, mlen-offset, 
	    	"<trt:Configurations token=\"%s\">", 
	    	p_v_src_cfg->Configuration.token);
	    offset += build_VideoSourceConfiguration_xml(p_buf+offset, mlen-offset, &p_v_src_cfg->Configuration);	
	    offset += snprintf(p_buf+offset, mlen-offset, "</trt:Configurations>");
	    
	    p_v_src_cfg = p_v_src_cfg->next;
	}
	
	offset += snprintf(p_buf+offset, mlen-offset, "</trt:GetVideoSourceConfigurationsResponse>");
	
	return offset;
}

int build_GetVideoSourceConfiguration_rly_xml(char * p_buf, int mlen, const char * token)
{
	int offset = 0;
    ONVIF_VideoSourceConfiguration * p_v_src_cfg = onvif_find_VideoSourceConfiguration(token);
    if (NULL == p_v_src_cfg)
    {
    	return ONVIF_ERR_NoConfig;
    }
	
	offset += snprintf(p_buf+offset, mlen-offset, "<trt:GetVideoSourceConfigurationResponse>");

    offset += snprintf(p_buf+offset, mlen-offset, "<trt:Configuration token=\"%s\">", p_v_src_cfg->Configuration.token);
    offset += build_VideoSourceConfiguration_xml(p_buf+offset, mlen-offset, &p_v_src_cfg->Configuration);	
    offset += snprintf(p_buf+offset, mlen-offset, "</trt:Configuration>");	    
	
	offset += snprintf(p_buf+offset, mlen-offset, "</trt:GetVideoSourceConfigurationResponse>");
	
	return offset;
}

int build_SetVideoSourceConfiguration_rly_xml(char * p_buf, int mlen, const char * argv)
{
	int offset = 0;
	offset += snprintf(p_buf+offset, mlen-offset, "<trt:SetVideoSourceConfigurationResponse />");	
	return offset;
}

int build_VideoSourceConfigurationOptions_xml(char * p_buf, int mlen, onvif_VideoSourceConfigurationOptions * p_req)
{
    int offset = 0;
    ONVIF_VideoSource * p_v_src = g_onvif_cfg.v_src;
    
    offset += snprintf(p_buf+offset, mlen-offset, 
    	"<tt:BoundsRange>"
			"<tt:XRange>"
				"<tt:Min>%d</tt:Min>"
				"<tt:Max>%d</tt:Max>"
			"</tt:XRange>"
			"<tt:YRange>"
				"<tt:Min>%d</tt:Min>"
				"<tt:Max>%d</tt:Max>"
			"</tt:YRange>"
			"<tt:WidthRange>"
				"<tt:Min>%d</tt:Min>"
				"<tt:Max>%d</tt:Max>"
			"</tt:WidthRange>"
			"<tt:HeightRange>"
				"<tt:Min>%d</tt:Min>"
				"<tt:Max>%d</tt:Max>"
			"</tt:HeightRange>"
		"</tt:BoundsRange>", 
		p_req->BoundsRange.XRange.Min, 
		p_req->BoundsRange.XRange.Max,
		p_req->BoundsRange.YRange.Min, 
		p_req->BoundsRange.YRange.Max,
		p_req->BoundsRange.WidthRange.Min, 
		p_req->BoundsRange.WidthRange.Max,
		p_req->BoundsRange.HeightRange.Min, 
		p_req->BoundsRange.HeightRange.Max);
	
	while (p_v_src)
	{
		offset += snprintf(p_buf+offset, mlen-offset, 
		    "<tt:VideoSourceTokensAvailable>%s</tt:VideoSourceTokensAvailable>", 
		    p_v_src->VideoSource.token);
		
		p_v_src = p_v_src->next;
	}

	return offset;
}

int build_GetVideoSourceConfigurationOptions_rly_xml(char * p_buf, int mlen, const char * argv)
{
	int offset = 0;
	ONVIF_VideoSourceConfiguration * p_v_src_cfg = NULL;
	
	GetVideoSourceConfigurationOptions_REQ * p_req = (GetVideoSourceConfigurationOptions_REQ *)argv;
	if (p_req->ProfileTokenFlag && p_req->ProfileToken[0] != '\0')
	{
		ONVIF_PROFILE * p_profile = onvif_find_profile(p_req->ProfileToken);
		if (NULL == p_profile)
		{
			return ONVIF_ERR_NoProfile;
		}

		p_v_src_cfg = p_profile->v_src_cfg;
	}

	if (p_req->ConfigurationTokenFlag && p_req->ConfigurationToken[0] != '\0')
	{
		p_v_src_cfg = onvif_find_VideoSourceConfiguration(p_req->ConfigurationToken);
		if (NULL == p_v_src_cfg)
		{
			return ONVIF_ERR_NoConfig;
		}
	}

	offset += snprintf(p_buf+offset, mlen-offset, "<trt:GetVideoSourceConfigurationOptionsResponse>");
    offset += snprintf(p_buf+offset, mlen-offset, "<trt:Options>");
    
    offset += build_VideoSourceConfigurationOptions_xml(p_buf+offset, mlen-offset, &g_onvif_cfg.VideoSourceConfigurationOptions);
    
	offset += snprintf(p_buf+offset, mlen-offset, "</trt:Options>");
	offset += snprintf(p_buf+offset, mlen-offset, "</trt:GetVideoSourceConfigurationOptionsResponse>");
	
	return offset;
}

int build_GetCompatibleVideoSourceConfigurations_rly_xml(char * p_buf, int mlen, const char * token)
{
	int offset = 0;
	ONVIF_VideoSourceConfiguration * p_v_src_cfg;
	ONVIF_PROFILE * p_profile = onvif_find_profile(token);
	if (NULL == p_profile)
	{
		return ONVIF_ERR_NoProfile;
	}

	p_v_src_cfg = g_onvif_cfg.v_src_cfg;
	
	offset += snprintf(p_buf+offset, mlen-offset, "<trt:GetCompatibleVideoSourceConfigurationsResponse>");

	while (p_v_src_cfg)
	{
	    offset += snprintf(p_buf+offset, mlen-offset, "<trt:Configurations token=\"%s\">", p_v_src_cfg->Configuration.token);
	    offset += build_VideoSourceConfiguration_xml(p_buf+offset, mlen-offset, &p_v_src_cfg->Configuration);	
	    offset += snprintf(p_buf+offset, mlen-offset, "</trt:Configurations>");
	    
	    p_v_src_cfg = p_v_src_cfg->next;
	}
	
	offset += snprintf(p_buf+offset, mlen-offset, "</trt:GetCompatibleVideoSourceConfigurationsResponse>");
	
	return offset;
}

int build_VideoResolution_xml(char * p_buf, int mlen, onvif_VideoResolution * p_req)
{
    int offset = 0;
    
	offset += snprintf(p_buf+offset, mlen-offset, 
		"<tt:ResolutionsAvailable>"
			"<tt:Width>%d</tt:Width>"
			"<tt:Height>%d</tt:Height>"
		"</tt:ResolutionsAvailable>",
		p_req->Width, 
		p_req->Height);

	return offset;
}

int build_JpegOptions_xml(char * p_buf, int mlen, onvif_JpegOptions * p_options)
{
	int i;
    int offset = 0;

    for (i = 0; i < ARRAY_SIZE(p_options->ResolutionsAvailable); i++)
    {
        if (p_options->ResolutionsAvailable[i].Width == 0 || p_options->ResolutionsAvailable[i].Height == 0)
        {
            continue;
        }
        
        offset += build_VideoResolution_xml(p_buf+offset, mlen-offset, &p_options->ResolutionsAvailable[i]);
    }
	
	offset += snprintf(p_buf+offset, mlen-offset, 
		"<tt:FrameRateRange>"
			"<tt:Min>%d</tt:Min>"
			"<tt:Max>%d</tt:Max>"
		"</tt:FrameRateRange>"
		"<tt:EncodingIntervalRange>"
			"<tt:Min>%d</tt:Min>"
			"<tt:Max>%d</tt:Max>"
		"</tt:EncodingIntervalRange>",
		p_options->FrameRateRange.Min, 
		p_options->FrameRateRange.Max,
		p_options->EncodingIntervalRange.Min, 
		p_options->EncodingIntervalRange.Max);

    return offset;		
}

int build_Mpeg4Options_xml(char * p_buf, int mlen, onvif_Mpeg4Options * p_options)
{
	int i;
    int offset = 0;

    for (i = 0; i < ARRAY_SIZE(p_options->ResolutionsAvailable); i++)
    {
        if (p_options->ResolutionsAvailable[i].Width == 0 || p_options->ResolutionsAvailable[i].Height == 0)
        {
            continue;
        }
        
        offset += build_VideoResolution_xml(p_buf+offset, mlen-offset, &p_options->ResolutionsAvailable[i]);
    }
	
	offset += snprintf(p_buf+offset, mlen-offset, 
		"<tt:GovLengthRange>"
			"<tt:Min>%d</tt:Min>"
			"<tt:Max>%d</tt:Max>"
		"</tt:GovLengthRange>"
		"<tt:FrameRateRange>"
			"<tt:Min>%d</tt:Min>"
			"<tt:Max>%d</tt:Max>"
		"</tt:FrameRateRange>"
		"<tt:EncodingIntervalRange>"
			"<tt:Min>%d</tt:Min>"
			"<tt:Max>%d</tt:Max>"
		"</tt:EncodingIntervalRange>",
		p_options->GovLengthRange.Min, 
		p_options->GovLengthRange.Max, 
		p_options->FrameRateRange.Min, 
		p_options->FrameRateRange.Max,
		p_options->EncodingIntervalRange.Min, 
		p_options->EncodingIntervalRange.Max);

	if (p_options->Mpeg4Profile_SP)
	{
		offset += snprintf(p_buf+offset, mlen-offset, "<tt:Mpeg4ProfilesSupported>SP</tt:Mpeg4ProfilesSupported>");
	}
	
	if (p_options->Mpeg4Profile_ASP)
	{
		offset += snprintf(p_buf+offset, mlen-offset, "<tt:Mpeg4ProfilesSupported>ASP</tt:Mpeg4ProfilesSupported>");
	}

	return offset;
}

int build_H264Options_xml(char * p_buf, int mlen, onvif_H264Options * p_options)
{
    int i;
    int offset = 0;
    
    for (i = 0; i < ARRAY_SIZE(p_options->ResolutionsAvailable); i++)
    {
        if (p_options->ResolutionsAvailable[i].Width == 0 || p_options->ResolutionsAvailable[i].Height == 0)
        {
            continue;
        }
        
        offset += build_VideoResolution_xml(p_buf+offset, mlen-offset, &p_options->ResolutionsAvailable[i]);
    }
    
	offset += snprintf(p_buf+offset, mlen-offset, 
		"<tt:GovLengthRange>"
			"<tt:Min>%d</tt:Min>"
			"<tt:Max>%d</tt:Max>"
		"</tt:GovLengthRange>"
		"<tt:FrameRateRange>"
			"<tt:Min>%d</tt:Min>"
			"<tt:Max>%d</tt:Max>"
		"</tt:FrameRateRange>"
		"<tt:EncodingIntervalRange>"
			"<tt:Min>%d</tt:Min>"
			"<tt:Max>%d</tt:Max>"
		"</tt:EncodingIntervalRange>",
		p_options->GovLengthRange.Min, 
		p_options->GovLengthRange.Max, 
		p_options->FrameRateRange.Min, 
		p_options->FrameRateRange.Max,
		p_options->EncodingIntervalRange.Min, 
		p_options->EncodingIntervalRange.Max);

	if (p_options->H264Profile_Baseline)
	{
		offset += snprintf(p_buf+offset, mlen-offset, "<tt:H264ProfilesSupported>Baseline</tt:H264ProfilesSupported>");
	}
	
	if (p_options->H264Profile_Main)
	{
		offset += snprintf(p_buf+offset, mlen-offset, "<tt:H264ProfilesSupported>Main</tt:H264ProfilesSupported>");
	}

	if (p_options->H264Profile_Extended)
	{
		offset += snprintf(p_buf+offset, mlen-offset, "<tt:H264ProfilesSupported>Extended</tt:H264ProfilesSupported>");
	}

	if (p_options->H264Profile_High)
	{
		offset += snprintf(p_buf+offset, mlen-offset, "<tt:H264ProfilesSupported>High</tt:H264ProfilesSupported>");
	}
    
    return offset;
}

int build_BitrateRange_xml(char * p_buf, int mlen, onvif_IntRange * p_req)
{
    int offset = 0;

    offset = snprintf(p_buf+offset, mlen-offset, 
        "<tt:BitrateRange>"
            "<tt:Min>%d</tt:Min>"
            "<tt:Max>%d</tt:Max>"
        "</tt:BitrateRange>",
        p_req->Min, p_req->Max);

    return offset;      
}

int build_GetVideoEncoderConfigurationOptions_rly_xml(char * p_buf, int mlen, const char * argv)
{   
	int offset = 0;
	GetVideoEncoderConfigurationOptions_REQ * p_req = (GetVideoEncoderConfigurationOptions_REQ *) argv;

	if (p_req->ProfileTokenFlag && p_req->ProfileToken[0] != '\0')
	{
		ONVIF_PROFILE * p_profile = onvif_find_profile(p_req->ProfileToken);
		if (NULL == p_profile)
		{
			return ONVIF_ERR_NoProfile;
		}
	}

	if (p_req->ConfigurationTokenFlag && p_req->ConfigurationToken[0] != '\0')
	{
		ONVIF_VideoEncoder2Configuration * p_v_enc_cfg = onvif_find_VideoEncoderConfiguration(p_req->ConfigurationToken);
		if (NULL == p_v_enc_cfg)
		{
			return ONVIF_ERR_NoConfig;
		}
	}

	offset += snprintf(p_buf+offset, mlen-offset, "<trt:GetVideoEncoderConfigurationOptionsResponse><trt:Options>");
	offset += snprintf(p_buf+offset, mlen-offset, 
		"<tt:QualityRange>"
			"<tt:Min>%d</tt:Min>"
			"<tt:Max>%d</tt:Max>"
		"</tt:QualityRange>",
		g_onvif_cfg.VideoEncoderConfigurationOptions.QualityRange.Min, 
		g_onvif_cfg.VideoEncoderConfigurationOptions.QualityRange.Max);

	if (g_onvif_cfg.VideoEncoderConfigurationOptions.JPEGFlag)
	{
		// JPEG options	
		offset += snprintf(p_buf+offset, mlen-offset, "<tt:JPEG>");
		
		offset += build_JpegOptions_xml(p_buf+offset, mlen-offset, &g_onvif_cfg.VideoEncoderConfigurationOptions.JPEG);

		offset += snprintf(p_buf+offset, mlen-offset, "</tt:JPEG>");			
	}

	if (g_onvif_cfg.VideoEncoderConfigurationOptions.MPEG4Flag)
	{
		// MPEG4 options
		offset += snprintf(p_buf+offset, mlen-offset, "<tt:MPEG4>");		
		offset += build_Mpeg4Options_xml(p_buf+offset, mlen-offset, &g_onvif_cfg.VideoEncoderConfigurationOptions.MPEG4);			
		offset += snprintf(p_buf+offset, mlen-offset, "</tt:MPEG4>");	
	}

	if (g_onvif_cfg.VideoEncoderConfigurationOptions.H264Flag)
	{
		// H264 options
		offset += snprintf(p_buf+offset, mlen-offset, "<tt:H264>");		
		offset += build_H264Options_xml(p_buf+offset, mlen-offset, &g_onvif_cfg.VideoEncoderConfigurationOptions.H264);			
		offset += snprintf(p_buf+offset, mlen-offset, "</tt:H264>");	
	}

	if (g_onvif_cfg.VideoEncoderConfigurationOptions.ExtensionFlag)
	{
	    offset += snprintf(p_buf+offset, mlen-offset, "<tt:Extension>");

        if (g_onvif_cfg.VideoEncoderConfigurationOptions.Extension.JPEGFlag)
    	{
    		// JPEG options	
    		offset += snprintf(p_buf+offset, mlen-offset, "<tt:JPEG>");
    		
    		offset += build_JpegOptions_xml(p_buf+offset, mlen-offset, &g_onvif_cfg.VideoEncoderConfigurationOptions.Extension.JPEG.JpegOptions);
    		offset += build_BitrateRange_xml(p_buf+offset, mlen-offset, &g_onvif_cfg.VideoEncoderConfigurationOptions.Extension.JPEG.BitrateRange);

    		offset += snprintf(p_buf+offset, mlen-offset, "</tt:JPEG>");			
    	}

    	if (g_onvif_cfg.VideoEncoderConfigurationOptions.Extension.MPEG4Flag)
    	{
    		// MPEG4 options
    		offset += snprintf(p_buf+offset, mlen-offset, "<tt:MPEG4>");
    		
    		offset += build_Mpeg4Options_xml(p_buf+offset, mlen-offset, &g_onvif_cfg.VideoEncoderConfigurationOptions.Extension.MPEG4.Mpeg4Options);
    		offset += build_BitrateRange_xml(p_buf+offset, mlen-offset, &g_onvif_cfg.VideoEncoderConfigurationOptions.Extension.MPEG4.BitrateRange);
    			
    		offset += snprintf(p_buf+offset, mlen-offset, "</tt:MPEG4>");	
    	}

    	if (g_onvif_cfg.VideoEncoderConfigurationOptions.Extension.H264Flag)
    	{
    		// H264 options
    		offset += snprintf(p_buf+offset, mlen-offset, "<tt:H264>");
    		
    		offset += build_H264Options_xml(p_buf+offset, mlen-offset, &g_onvif_cfg.VideoEncoderConfigurationOptions.Extension.H264.H264Options);
    		offset += build_BitrateRange_xml(p_buf+offset, mlen-offset, &g_onvif_cfg.VideoEncoderConfigurationOptions.Extension.H264.BitrateRange);
    			
    		offset += snprintf(p_buf+offset, mlen-offset, "</tt:H264>");	
    	}
	    
	    offset += snprintf(p_buf+offset, mlen-offset, "</tt:Extension>");
	}
	
	offset += snprintf(p_buf+offset, mlen-offset, "</trt:Options></trt:GetVideoEncoderConfigurationOptionsResponse>");	    
	
	return offset;
}

int build_SystemReboot_rly_xml(char * p_buf, int mlen, const char * argv)
{
    int offset = 0;
    
	offset += snprintf(p_buf+offset, mlen-offset, 
		"<tds:SystemRebootResponse>"
			"<tds:Message>Rebooting</tds:Message>"
		"</tds:SystemRebootResponse>");	    
	
	return offset;
}

int build_SetSystemFactoryDefault_rly_xml(char * p_buf, int mlen, const char * argv)
{
	int offset = 0;	
	offset += snprintf(p_buf+offset, mlen-offset, "<tds:SetSystemFactoryDefaultResponse />");	    
	return offset;
}

int build_GetSystemLog_rly_xml(char * p_buf, int mlen, const char * argv)
{
    int offset = 0;
    GetSystemLog_RES * p_res = (GetSystemLog_RES *)argv;
    
	offset += snprintf(p_buf+offset, mlen-offset, 
	    "<tds:GetSystemLogResponse>"
	        "<tds:SystemLog>"
	            "<tt:String>%s</tt:String>"
            "</tds:SystemLog>"
        "</tds:GetSystemLogResponse>",
        p_res->String);	    
	
	return offset;
}

int build_SetVideoEncoderConfiguration_rly_xml(char * p_buf, int mlen, const char * argv)
{
    int offset = 0;
	offset += snprintf(p_buf+offset, mlen-offset, "<trt:SetVideoEncoderConfigurationResponse />");		    
	return offset;
}

int build_SetSynchronizationPoint_rly_xml(char * p_buf, int mlen, const char * argv)
{
    int offset = 0;
	offset += snprintf(p_buf+offset, mlen-offset, "<trt:SetSynchronizationPointResponse />");		    
	return offset;
}

int build_GetSystemDateAndTime_rly_xml(char * p_buf, int mlen, const char * argv)
{
    int offset = 0;
	time_t nowtime;
	struct tm *gtime;

	time(&nowtime);
	gtime = gmtime(&nowtime);
    
	offset += snprintf(p_buf+offset, mlen-offset, 
		"<tds:GetSystemDateAndTimeResponse>"
			"<tds:SystemDateAndTime>"
			"<tt:DateTimeType>%s</tt:DateTimeType>"
			"<tt:DaylightSavings>%s</tt:DaylightSavings>",
			onvif_SetDateTimeTypeToString(g_onvif_cfg.SystemDateTime.DateTimeType), 
			g_onvif_cfg.SystemDateTime.DaylightSavings ? "true" : "false");

	if (g_onvif_cfg.SystemDateTime.TimeZoneFlag && 
		g_onvif_cfg.SystemDateTime.TimeZone.TZ[0] != '\0')
	{
		offset += snprintf(p_buf+offset, mlen-offset, 
			"<tt:TimeZone><tt:TZ>%s</tt:TZ></tt:TimeZone>", 
			g_onvif_cfg.SystemDateTime.TimeZone.TZ);			
	}
		
	offset += snprintf(p_buf+offset, mlen-offset, 			
			"<tt:UTCDateTime>"
				"<tt:Time>"
					"<tt:Hour>%d</tt:Hour>"
					"<tt:Minute>%d</tt:Minute>"
					"<tt:Second>%d</tt:Second>"
				"</tt:Time>"
				"<tt:Date>"
					"<tt:Year>%d</tt:Year>"
					"<tt:Month>%d</tt:Month>"
					"<tt:Day>%d</tt:Day>"
				"</tt:Date>"
			"</tt:UTCDateTime>"	
			"</tds:SystemDateAndTime>"
		"</tds:GetSystemDateAndTimeResponse>",
		gtime->tm_hour, gtime->tm_min, gtime->tm_sec, 
		gtime->tm_year+1900, gtime->tm_mon+1, gtime->tm_mday);		
	
	return offset;
}


int build_SetSystemDateAndTime_rly_xml(char * p_buf, int mlen, const char * argv)
{
    int offset = 0;
	offset += snprintf(p_buf+offset, mlen-offset, "<tds:SetSystemDateAndTimeResponse></tds:SetSystemDateAndTimeResponse>");
	return offset;
}

int build_DeviceServicesCapabilities_xml(char * p_buf, int mlen)
{
    int i;
	int offset = 0;

	offset += snprintf(p_buf+offset, mlen-offset, "<tds:Capabilities>");
	
	offset += snprintf(p_buf+offset, mlen-offset, 
		"<tds:Network IPFilter=\"%s\" ZeroConfiguration=\"%s\" "
			"IPVersion6=\"%s\" DynDNS=\"%s\" Dot11Configuration=\"%s\" "
			"Dot1XConfigurations=\"%d\" HostnameFromDHCP=\"%s\" NTP=\"%d\" DHCPv6=\"%s\">"
		"</tds:Network>",
		g_onvif_cfg.Capabilities.device.IPFilter ? "true" : "false",
		g_onvif_cfg.Capabilities.device.ZeroConfiguration ? "true" : "false",
		g_onvif_cfg.Capabilities.device.IPVersion6 ? "true" : "false",
		g_onvif_cfg.Capabilities.device.DynDNS ? "true" : "false",
		g_onvif_cfg.Capabilities.device.Dot11Configuration ? "true" : "false",
		g_onvif_cfg.Capabilities.device.Dot1XConfigurations,
		g_onvif_cfg.Capabilities.device.HostnameFromDHCP ? "true" : "false",
		g_onvif_cfg.Capabilities.device.NTP,
		g_onvif_cfg.Capabilities.device.DHCPv6 ? "true" : "false");
	
	offset += snprintf(p_buf+offset, mlen-offset, 
		"<tds:Security TLS1.0=\"%s\" TLS1.1=\"%s\" TLS1.2=\"%s\" "
			"OnboardKeyGeneration=\"%s\" AccessPolicyConfig=\"%s\" DefaultAccessPolicy=\"%s\" "
			"Dot1X=\"%s\" RemoteUserHandling=\"%s\" X.509Token=\"%s\" SAMLToken=\"%s\" "
			"KerberosToken=\"%s\" UsernameToken=\"%s\" HttpDigest=\"%s\" RELToken=\"%s\" "
			"SupportedEAPMethods=\"%d\" MaxUsers=\"%d\" "
			"MaxUserNameLength=\"%d\" MaxPasswordLength=\"%d\">"
		"</tds:Security>",
		g_onvif_cfg.Capabilities.device.TLS10 ? "true" : "false",
		g_onvif_cfg.Capabilities.device.TLS11 ? "true" : "false",
		g_onvif_cfg.Capabilities.device.TLS12 ? "true" : "false",
		g_onvif_cfg.Capabilities.device.OnboardKeyGeneration ? "true" : "false",
		g_onvif_cfg.Capabilities.device.AccessPolicyConfig ? "true" : "false",
		g_onvif_cfg.Capabilities.device.DefaultAccessPolicy ? "true" : "false",
		g_onvif_cfg.Capabilities.device.Dot1X ? "true" : "false",
		g_onvif_cfg.Capabilities.device.RemoteUserHandling ? "true" : "false",
		g_onvif_cfg.Capabilities.device.X509Token ? "true" : "false",
		g_onvif_cfg.Capabilities.device.SAMLToken ? "true" : "false",
		g_onvif_cfg.Capabilities.device.KerberosToken ? "true" : "false",
		g_onvif_cfg.Capabilities.device.UsernameToken ? "true" : "false",
		g_onvif_cfg.Capabilities.device.HttpDigest ? "true" : "false",
		g_onvif_cfg.Capabilities.device.RELToken ? "true" : "false",
		g_onvif_cfg.Capabilities.device.SupportedEAPMethods,
		g_onvif_cfg.Capabilities.device.MaxUsers,
		g_onvif_cfg.Capabilities.device.MaxUserNameLength,
		g_onvif_cfg.Capabilities.device.MaxPasswordLength);
	
	offset += snprintf(p_buf+offset, mlen-offset, 
		"<tds:System DiscoveryResolve=\"%s\" DiscoveryBye=\"%s\" "
			"RemoteDiscovery=\"%s\" SystemBackup=\"%s\" SystemLogging=\"%s\" "
			"FirmwareUpgrade=\"%s\" HttpFirmwareUpgrade=\"%s\" HttpSystemBackup=\"%s\" "
			"HttpSystemLogging=\"%s\" HttpSupportInformation=\"%s\" StorageConfiguration=\"%s\" "
			"MaxStorageConfigurations=\"%d\" GeoLocationEntries=\"%d\" AutoGeo=\"%s\" StorageTypesSupported=\"%s\">"
		"</tds:System>",
		g_onvif_cfg.Capabilities.device.DiscoveryResolve ? "true" : "false",
		g_onvif_cfg.Capabilities.device.DiscoveryBye ? "true" : "false",
		g_onvif_cfg.Capabilities.device.RemoteDiscovery ? "true" : "false",
		g_onvif_cfg.Capabilities.device.SystemBackup ? "true" : "false",
		g_onvif_cfg.Capabilities.device.SystemLogging ? "true" : "false",
		g_onvif_cfg.Capabilities.device.FirmwareUpgrade ? "true" : "false",
		g_onvif_cfg.Capabilities.device.HttpFirmwareUpgrade ? "true" : "false",
		g_onvif_cfg.Capabilities.device.HttpSystemBackup ? "true" : "false",
		g_onvif_cfg.Capabilities.device.HttpSystemLogging ? "true" : "false",
		g_onvif_cfg.Capabilities.device.HttpSupportInformation ? "true" : "false",
		g_onvif_cfg.Capabilities.device.StorageConfiguration ? "true" : "false",
		g_onvif_cfg.Capabilities.device.MaxStorageConfigurations,
		g_onvif_cfg.Capabilities.device.GeoLocationEntries,
		g_onvif_cfg.Capabilities.device.AutoGeo,
		g_onvif_cfg.Capabilities.device.StorageTypesSupported);

    offset += snprintf(p_buf+offset, mlen-offset, "<tds:Misc AuxiliaryCommands=\"");
    for (i = 0; i < g_onvif_cfg.Capabilities.device.sizeAuxiliaryCommands; i++)
    {
        offset += snprintf(p_buf+offset, mlen-offset, "%s ", g_onvif_cfg.Capabilities.device.AuxiliaryCommands[i]);
    }
    offset += snprintf(p_buf+offset, mlen-offset, "\"></tds:Misc>");
    
	offset += snprintf(p_buf+offset, mlen-offset, "</tds:Capabilities>");

	return offset;
}

int build_MediaServicesCapabilities_xml(char * p_buf, int mlen)
{
	int offset = 0;

	offset += snprintf(p_buf+offset, mlen-offset, 
		"<trt:Capabilities SnapshotUri=\"%s\" Rotation=\"%s\" VideoSourceMode=\"%s\" "
		"OSD=\"%s\" TemporaryOSDText=\"%s\" EXICompression=\"%s\">",
		g_onvif_cfg.Capabilities.media.SnapshotUri ? "true" : "false",
		g_onvif_cfg.Capabilities.media.Rotation ? "true" : "false",
		g_onvif_cfg.Capabilities.media.VideoSourceMode ? "true" : "false",
		g_onvif_cfg.Capabilities.media.OSD ? "true" : "false",
		g_onvif_cfg.Capabilities.media.TemporaryOSDText ? "true" : "false",
		g_onvif_cfg.Capabilities.media.EXICompression ? "true" : "false");
	
	offset += snprintf(p_buf+offset, mlen-offset, 
		"<trt:ProfileCapabilities MaximumNumberOfProfiles=\"%d\" />"
		"<trt:StreamingCapabilities RTPMulticast=\"%s\" RTP_TCP=\"%s\" RTP_RTSP_TCP=\"%s\" "
			"NonAggregateControl=\"%s\" NoRTSPStreaming=\"%s\" />",
		g_onvif_cfg.Capabilities.media.MaximumNumberOfProfiles,
		g_onvif_cfg.Capabilities.media.RTPMulticast ? "true" : "false",
		g_onvif_cfg.Capabilities.media.RTP_TCP ? "true" : "false",
		g_onvif_cfg.Capabilities.media.RTP_RTSP_TCP ? "true" : "false",
		g_onvif_cfg.Capabilities.media.NonAggregateControl ? "true" : "false",
		g_onvif_cfg.Capabilities.media.NoRTSPStreaming ? "true" : "false");
			
	offset += snprintf(p_buf+offset, mlen-offset, "</trt:Capabilities>");

	return offset;
}

#ifdef MEDIA2_SUPPORT
int build_MediaServicesCapabilities2_xml(char * p_buf, int mlen)
{
	int offset = 0;

	offset += snprintf(p_buf+offset, mlen-offset, 
		"<tr2:Capabilities SnapshotUri=\"%s\" Rotation=\"%s\" VideoSourceMode=\"%s\" OSD=\"%s\" "
		"TemporaryOSDText=\"%s\" Mask=\"%s\" SourceMask=\"%s\">",
		g_onvif_cfg.Capabilities.media2.SnapshotUri ? "true" : "false",
		g_onvif_cfg.Capabilities.media2.Rotation ? "true" : "false",
		g_onvif_cfg.Capabilities.media2.VideoSourceMode ? "true" : "false",
		g_onvif_cfg.Capabilities.media2.OSD ? "true" : "false",
		g_onvif_cfg.Capabilities.media2.TemporaryOSDText ? "true" : "false",
		g_onvif_cfg.Capabilities.media2.Mask ? "true" : "false",
		g_onvif_cfg.Capabilities.media2.SourceMask ? "true" : "false");
	
	offset += snprintf(p_buf+offset, mlen-offset, 
		"<tr2:ProfileCapabilities MaximumNumberOfProfiles=\"%d\" ConfigurationsSupported=\"%s\" />"
		"<tr2:StreamingCapabilities RTSPStreaming=\"%s\" RTPMulticast=\"%s\" RTP_RTSP_TCP=\"%s\" "
		"NonAggregateControl=\"%s\" AutoStartMulticast=\"%s\" />", 
		g_onvif_cfg.Capabilities.media2.ProfileCapabilities.MaximumNumberOfProfiles,
		g_onvif_cfg.Capabilities.media2.ProfileCapabilities.ConfigurationsSupported,
		g_onvif_cfg.Capabilities.media2.StreamingCapabilities.RTSPStreaming ? "true" : "false",
		g_onvif_cfg.Capabilities.media2.StreamingCapabilities.RTPMulticast ? "true" : "false",
		g_onvif_cfg.Capabilities.media2.StreamingCapabilities.RTP_USCORERTSP_USCORETCP ? "true" : "false",
		g_onvif_cfg.Capabilities.media2.StreamingCapabilities.NonAggregateControl ? "true" : "false",
		//g_onvif_cfg.Capabilities.media2.StreamingCapabilities.RTSPWebSocketUri,
		g_onvif_cfg.Capabilities.media2.StreamingCapabilities.AutoStartMulticast ? "true" : "false");
			
	offset += snprintf(p_buf+offset, mlen-offset, "</tr2:Capabilities>");

	return offset;
}
#endif

int build_EventsServicesCapabilities_xml(char * p_buf, int mlen)
{
	int offset = 0;
	
	offset += snprintf(p_buf+offset, mlen-offset, 
		"<tev:Capabilities WSSubscriptionPolicySupport=\"%s\" WSPullPointSupport=\"%s\" "
			"WSPausableSubscriptionManagerInterfaceSupport=\"%s\" MaxNotificationProducers=\"%d\" "
			"MaxPullPoints=\"%d\" PersistentNotificationStorage=\"%s\">"
		"</tev:Capabilities>",
		g_onvif_cfg.Capabilities.events.WSSubscriptionPolicySupport ? "true" : "false",
		g_onvif_cfg.Capabilities.events.WSPullPointSupport ? "true" : "false",
		g_onvif_cfg.Capabilities.events.WSPausableSubscriptionManagerInterfaceSupport ? "true" : "false",
		g_onvif_cfg.Capabilities.events.MaxNotificationProducers,
		g_onvif_cfg.Capabilities.events.MaxPullPoints,
		g_onvif_cfg.Capabilities.events.PersistentNotificationStorage ? "true" : "false");

	return offset;
}

#ifdef PTZ_SUPPORT
int build_PTZServicesCapabilities_xml(char * p_buf, int mlen)
{
	int offset = 0;
	
	offset += snprintf(p_buf+offset, mlen-offset, 
		"<tptz:Capabilities EFlip=\"%s\" Reverse=\"%s\" GetCompatibleConfigurations=\"%s\" MoveStatus=\"%s\" StatusPosition=\"%s\" />",
		g_onvif_cfg.Capabilities.ptz.EFlip ? "true" : "false",
		g_onvif_cfg.Capabilities.ptz.Reverse ? "true" : "false",
		g_onvif_cfg.Capabilities.ptz.GetCompatibleConfigurations ? "true" : "false",
		g_onvif_cfg.Capabilities.ptz.MoveStatus ? "true" : "false",
		g_onvif_cfg.Capabilities.ptz.StatusPosition ? "true" : "false");

	return offset;
}
#endif

int build_ImagingServicesCapabilities_xml(char * p_buf, int mlen)
{
	int offset = 0;
	
	offset += snprintf(p_buf+offset, mlen-offset, 
		"<timg:Capabilities ImageStabilization=\"%s\" Presets=\"%s\" />",
		g_onvif_cfg.Capabilities.image.ImageStabilization ? "true" : "false", 
		g_onvif_cfg.Capabilities.image.Presets ? "true" : "false");

	return offset;
}

#ifdef VIDEO_ANALYTICS
int build_AnalyticsServicesCapabilities_xml(char * p_buf, int mlen)
{
	int offset = 0;
	
	offset += snprintf(p_buf+offset, mlen-offset, 
		"<tan:Capabilities RuleSupport=\"%s\" AnalyticsModuleSupport=\"%s\" CellBasedSceneDescriptionSupported=\"%s\" "
		"RuleOptionsSupported=\"%s\" AnalyticsModuleOptionsSupported=\"%s\" />",
		g_onvif_cfg.Capabilities.analytics.RuleSupport ? "true" : "false",
		g_onvif_cfg.Capabilities.analytics.AnalyticsModuleSupport ? "true" : "false",
		g_onvif_cfg.Capabilities.analytics.CellBasedSceneDescriptionSupported ? "true" : "false",
		g_onvif_cfg.Capabilities.analytics.RuleOptionsSupported ? "true" : "false",
		g_onvif_cfg.Capabilities.analytics.AnalyticsModuleOptionsSupported ? "true" : "false");

	return offset;
}
#endif	// end of VIDEO_ANALYTICS

#ifdef PROFILE_G_SUPPORT

int build_RecordingServicesCapabilities_xml(char * p_buf, int mlen)
{
	int offset = 0;
	char Encoding[100];

	memset(Encoding, 0, sizeof(Encoding));
	
	if (g_onvif_cfg.Capabilities.recording.JPEG)
	{
		strcat(Encoding, "JPEG ");
	}
	if (g_onvif_cfg.Capabilities.recording.MPEG4)
	{
		strcat(Encoding, "MPEG4 ");
	}
	if (g_onvif_cfg.Capabilities.recording.H264)
	{
		strcat(Encoding, "H264 ");
	}
	if (g_onvif_cfg.Capabilities.recording.G711)
	{
		strcat(Encoding, "G711 ");
	}
	if (g_onvif_cfg.Capabilities.recording.G726)
	{
		strcat(Encoding, "G726 ");
	}
	if (g_onvif_cfg.Capabilities.recording.AAC)
	{
		strcat(Encoding, "AAC ");
	}
	
	offset += snprintf(p_buf+offset, mlen-offset, 
		"<trc:Capabilities DynamicRecordings=\"%s\" DynamicTracks=\"%s\" Encoding=\"%s\" "
		"MaxRate=\"%0.1f\" MaxTotalRate=\"%0.1f\" MaxRecordings=\"%d\" MaxRecordingJobs=\"%d\" "
		"Options=\"%s\" MetadataRecording=\"%s\" SupportedExportFileFormats=\"%s\" />",
		g_onvif_cfg.Capabilities.recording.DynamicRecordings ? "true" : "false",
		g_onvif_cfg.Capabilities.recording.DynamicTracks ? "true" : "false",
		Encoding,
		g_onvif_cfg.Capabilities.recording.MaxRate,
		g_onvif_cfg.Capabilities.recording.MaxTotalRate,
		g_onvif_cfg.Capabilities.recording.MaxRecordings,
		g_onvif_cfg.Capabilities.recording.MaxRecordingJobs,
		g_onvif_cfg.Capabilities.recording.Options ? "true" : "false",
		g_onvif_cfg.Capabilities.recording.MetadataRecording ? "true" : "false",
		g_onvif_cfg.Capabilities.recording.SupportedExportFileFormats);

	return offset;
}

int build_SearchServicesCapabilities_xml(char * p_buf, int mlen)
{
	int offset = 0;
	
	offset += snprintf(p_buf+offset, mlen-offset, 
		"<tse:Capabilities MetadataSearch=\"%s\" GeneralStartEvents=\"%s\" />",
		g_onvif_cfg.Capabilities.search.MetadataSearch ? "true" : "false",
		g_onvif_cfg.Capabilities.search.GeneralStartEvents ? "true" : "false");

	return offset;
}

int build_ReplayServicesCapabilities_xml(char * p_buf, int mlen)
{
	int offset = 0;
	
	offset += snprintf(p_buf+offset, mlen-offset, 
		"<trp:Capabilities ReversePlayback=\"%s\" SessionTimeoutRange=\"%0.1f %0.1f\" "
		"RTP_RTSP_TCP=\"%s\" RTSPWebSocketUri=\"%s\" />",
		g_onvif_cfg.Capabilities.replay.ReversePlayback ? "true" : "false",
		g_onvif_cfg.Capabilities.replay.SessionTimeoutRange.Min,
		g_onvif_cfg.Capabilities.replay.SessionTimeoutRange.Max,
		g_onvif_cfg.Capabilities.replay.RTP_RTSP_TCP ? "true" : "false",
		g_onvif_cfg.Capabilities.replay.RTSPWebSocketUri);

	return offset;
}

#endif // PROFILE_G_SUPPORT

#ifdef PROFILE_C_SUPPORT

int build_AccessControlServicesCapabilities_xml(char * p_buf, int mlen)
{
    int offset = 0;
	
	offset += snprintf(p_buf+offset, mlen-offset, 
		"<tac:Capabilities MaxLimit=\"%d\" MaxAccessPoints=\"%d\" MaxAreas=\"%d\" ClientSuppliedTokenSupported=\"%s\" />",
		g_onvif_cfg.Capabilities.accesscontrol.MaxLimit,
		g_onvif_cfg.Capabilities.accesscontrol.MaxAccessPoints,
		g_onvif_cfg.Capabilities.accesscontrol.MaxAreas,
		g_onvif_cfg.Capabilities.accesscontrol.ClientSuppliedTokenSupported ? "true" : "false");

	return offset;
}

int build_DoorControlServicesCapabilities_xml(char * p_buf, int mlen)
{
    int offset = 0;
	
	offset += snprintf(p_buf+offset, mlen-offset, 
		"<tdc:Capabilities MaxLimit=\"%d\" MaxDoors=\"%d\" ClientSuppliedTokenSupported=\"%s\" />",
		g_onvif_cfg.Capabilities.doorcontrol.MaxLimit,
		g_onvif_cfg.Capabilities.doorcontrol.MaxDoors,
		g_onvif_cfg.Capabilities.doorcontrol.ClientSuppliedTokenSupported ? "true" : "false");

	return offset;
}

#endif // PROFILE_C_SUPPORT

#ifdef DEVICEIO_SUPPORT
int build_DeviceIOServicesCapabilities_xml(char * p_buf, int mlen)
{
	int offset = 0;

    offset += snprintf(p_buf+offset, mlen-offset, 
		"<tmd:Capabilities VideoSources=\"%d\" VideoOutputs=\"%d\" "
		"AudioSources=\"%d\" AudioOutputs=\"%d\" RelayOutputs=\"%d\" SerialPorts=\"%d\" "
		"DigitalInputs=\"%d\" DigitalInputOptions=\"%s\" />",
		g_onvif_cfg.Capabilities.deviceIO.VideoSources,
		g_onvif_cfg.Capabilities.deviceIO.VideoOutputs,
		g_onvif_cfg.Capabilities.deviceIO.AudioSources,
		g_onvif_cfg.Capabilities.deviceIO.AudioOutputs,
		g_onvif_cfg.Capabilities.deviceIO.RelayOutputs,
		g_onvif_cfg.Capabilities.deviceIO.SerialPorts,
		g_onvif_cfg.Capabilities.deviceIO.DigitalInputs,
		g_onvif_cfg.Capabilities.deviceIO.DigitalInputOptions ? "true" : "false");

	return offset;
}
#endif

#ifdef THERMAL_SUPPORT
int build_ThermalServicesCapabilities_xml(char * p_buf, int mlen)
{
    int offset = 0;
	
	offset += snprintf(p_buf+offset, mlen-offset, 
		"<tth:Capabilities Radiometry=\"%s\" />",
		g_onvif_cfg.Capabilities.thermal.Radiometry ? "true" : "false");

	return offset;
}
#endif

#ifdef CREDENTIAL_SUPPORT
int build_CredentialServicesCapabilities_xml(char * p_buf, int mlen)
{
    int i;
    int offset = 0;

    offset += snprintf(p_buf+offset, mlen-offset, 
		"<tcr:Capabilities MaxLimit=\"%d\" CredentialValiditySupported=\"%s\" "
		"CredentialAccessProfileValiditySupported=\"%s\" ValiditySupportsTimeValue=\"%s\" "
		"MaxCredentials=\"%d\" MaxAccessProfilesPerCredential=\"%d\" ResetAntipassbackSupported=\"%s\" "
		"ClientSuppliedTokenSupported=\"%s\" DefaultCredentialSuspensionDuration=\"%s\">",
		g_onvif_cfg.Capabilities.credential.MaxLimit,
		g_onvif_cfg.Capabilities.credential.CredentialValiditySupported ? "true" : "false",
		g_onvif_cfg.Capabilities.credential.CredentialAccessProfileValiditySupported ? "true" : "false",
		g_onvif_cfg.Capabilities.credential.ValiditySupportsTimeValue ? "true" : "false",
		g_onvif_cfg.Capabilities.credential.MaxCredentials,
		g_onvif_cfg.Capabilities.credential.MaxAccessProfilesPerCredential,
		g_onvif_cfg.Capabilities.credential.ResetAntipassbackSupported ? "true" : "false",
		g_onvif_cfg.Capabilities.credential.ClientSuppliedTokenSupported ? "true" : "false",
		g_onvif_cfg.Capabilities.credential.DefaultCredentialSuspensionDuration);

    for (i = 0; i < g_onvif_cfg.Capabilities.credential.sizeSupportedIdentifierType; i++)
    {
        offset += snprintf(p_buf+offset, mlen-offset, 
            "<tcr:SupportedIdentifierType>%s</tcr:SupportedIdentifierType>",
            g_onvif_cfg.Capabilities.credential.SupportedIdentifierType[i]);
    }

    if (g_onvif_cfg.Capabilities.credential.ExtensionFlag)
    {
        offset += snprintf(p_buf+offset, mlen-offset, "<tcr:Extension>");

        for (i = 0; i < g_onvif_cfg.Capabilities.credential.Extension.sizeSupportedExemptionType; i++)
        {
            offset += snprintf(p_buf+offset, mlen-offset, 
                "<tcr:SupportedExemptionType>%s</tcr:SupportedExemptionType>",
                g_onvif_cfg.Capabilities.credential.Extension.SupportedExemptionType[i]);
        }
    
        offset += snprintf(p_buf+offset, mlen-offset, "</tcr:Extension>");
    }

    offset += snprintf(p_buf+offset, mlen-offset, "</tcr:Capabilities>");

    return offset;
}
#endif // end of CREDENTIAL_SUPPORT

#ifdef ACCESS_RULES
int build_AccessRulesServicesCapabilities_xml(char * p_buf, int mlen)
{
    int offset = 0;

    offset += snprintf(p_buf+offset, mlen-offset, 
		"<tar:Capabilities MaxLimit=\"%d\" MaxAccessProfiles=\"%d\" "
		"MaxAccessPoliciesPerAccessProfile=\"%d\" MultipleSchedulesPerAccessPointSupported=\"%s\" "
		"ClientSuppliedTokenSupported=\"%s\" />",
		g_onvif_cfg.Capabilities.accessrules.MaxLimit,
		g_onvif_cfg.Capabilities.accessrules.MaxAccessProfiles,
		g_onvif_cfg.Capabilities.accessrules.MaxAccessPoliciesPerAccessProfile,
		g_onvif_cfg.Capabilities.accessrules.MultipleSchedulesPerAccessPointSupported ? "true" : "false",
		g_onvif_cfg.Capabilities.accessrules.ClientSuppliedTokenSupported ? "true" : "false");

    return offset;
}
#endif // end of ACCESS_RULES

#ifdef SCHEDULE_SUPPORT
int build_ScheduleServicesCapabilities_xml(char * p_buf, int mlen)
{
    int offset = 0;

    offset += snprintf(p_buf+offset, mlen-offset, 
		"<tsc:Capabilities MaxLimit=\"%d\" MaxSchedules=\"%d\" MaxTimePeriodsPerDay=\"%d\" "
		"MaxSpecialDayGroups=\"%d\" MaxDaysInSpecialDayGroup=\"%d\" MaxSpecialDaysSchedules=\"%d\" "
		"ExtendedRecurrenceSupported=\"%s\" SpecialDaysSupported=\"%s\" StateReportingSupported=\"%s\" "
		"ClientSuppliedTokenSupported=\"%s\" />",
		g_onvif_cfg.Capabilities.schedule.MaxLimit,
		g_onvif_cfg.Capabilities.schedule.MaxSchedules,
		g_onvif_cfg.Capabilities.schedule.MaxTimePeriodsPerDay,
		g_onvif_cfg.Capabilities.schedule.MaxSpecialDayGroups,
		g_onvif_cfg.Capabilities.schedule.MaxDaysInSpecialDayGroup,
		g_onvif_cfg.Capabilities.schedule.MaxSpecialDaysSchedules,
		g_onvif_cfg.Capabilities.schedule.ExtendedRecurrenceSupported ? "true" : "false",
		g_onvif_cfg.Capabilities.schedule.SpecialDaysSupported ? "true" : "false",
		g_onvif_cfg.Capabilities.schedule.StateReportingSupported ? "true" : "false",
		g_onvif_cfg.Capabilities.schedule.ClientSuppliedTokenSupported ? "true" : "false");

    return offset;
}
#endif // end of SCHEDULE_SUPPORT

#ifdef RECEIVER_SUPPORT
int build_ReceiverServicesCapabilities_xml(char * p_buf, int mlen)
{
    int offset = 0;

    offset += snprintf(p_buf+offset, mlen-offset, 
		"<trv:Capabilities RTP_Multicast=\"%s\" RTP_TCP=\"%s\" RTP_RTSP_TCP=\"%s\" SupportedReceivers=\"%d\" MaximumRTSPURILength=\"%d\" />",
		g_onvif_cfg.Capabilities.receiver.RTP_USCOREMulticast ? "true" : "false",
		g_onvif_cfg.Capabilities.receiver.RTP_USCORETCP ? "true" : "false",
		g_onvif_cfg.Capabilities.receiver.RTP_USCORERTSP_USCORETCP ? "true" : "false",
		g_onvif_cfg.Capabilities.receiver.SupportedReceivers,
		g_onvif_cfg.Capabilities.receiver.MaximumRTSPURILength);

    return offset;
}
#endif // end of RECEIVER_SUPPORT

int build_Version_xml(char * p_buf, int mlen, int major, int minor)
{
	int offset = 0;

	offset += snprintf(p_buf+offset, mlen-offset, 
		"<tds:Version>"
		    "<tt:Major>%d</tt:Major>"
		    "<tt:Minor>%d</tt:Minor>"
	    "</tds:Version>",
	    major, minor);

	return offset;
}

int build_GetServices_rly_xml(char * p_buf, int mlen, const char * argv)
{
    int offset = 0;
	GetServices_REQ * p_req = (GetServices_REQ *)argv;

	offset += snprintf(p_buf+offset, mlen-offset, "<tds:GetServicesResponse>");

	// device manager
	offset += snprintf(p_buf+offset, mlen-offset, 
	    "<tds:Service>"
	    "<tds:Namespace>http://www.onvif.org/ver10/device/wsdl</tds:Namespace>"
	    "<tds:XAddr>%s</tds:XAddr>", 
	    g_onvif_cfg.Capabilities.device.XAddr);	
	if (p_req->IncludeCapability)
	{
        offset += snprintf(p_buf+offset, mlen-offset, "<tds:Capabilities>");        
        offset += build_DeviceServicesCapabilities_xml(p_buf+offset, mlen-offset);				
		offset += snprintf(p_buf+offset, mlen-offset, "</tds:Capabilities>");
	}    
	offset += build_Version_xml(p_buf+offset, mlen-offset, 17, 12);
	offset += snprintf(p_buf+offset, mlen-offset, "</tds:Service>");

#ifdef MEDIA2_SUPPORT
    // media service 2
	if (g_onvif_cfg.Capabilities.media2.support)
	{
		offset += snprintf(p_buf+offset, mlen-offset, 
		    "<tds:Service>"
		    "<tds:Namespace>http://www.onvif.org/ver20/media/wsdl</tds:Namespace>"
		    "<tds:XAddr>%s</tds:XAddr>", 
		    g_onvif_cfg.Capabilities.media2.XAddr);	
		if (p_req->IncludeCapability)
		{
	        offset += snprintf(p_buf+offset, mlen-offset, "<tds:Capabilities>");
	        offset += build_MediaServicesCapabilities2_xml(p_buf+offset, mlen-offset);				
			offset += snprintf(p_buf+offset, mlen-offset, "</tds:Capabilities>");
		}    
		offset += build_Version_xml(p_buf+offset, mlen-offset, 17, 12);
		offset += snprintf(p_buf+offset, mlen-offset, "</tds:Service>");
	}
#endif

	// media service 1
	if (g_onvif_cfg.Capabilities.media.support)
	{
		offset += snprintf(p_buf+offset, mlen-offset, 
		    "<tds:Service>"
		    "<tds:Namespace>http://www.onvif.org/ver10/media/wsdl</tds:Namespace>"
		    "<tds:XAddr>%s</tds:XAddr>", 
		    g_onvif_cfg.Capabilities.media.XAddr);	
		if (p_req->IncludeCapability)
		{
	        offset += snprintf(p_buf+offset, mlen-offset, "<tds:Capabilities>");
	        offset += build_MediaServicesCapabilities_xml(p_buf+offset, mlen-offset);				
			offset += snprintf(p_buf+offset, mlen-offset, "</tds:Capabilities>");
		}    
		offset += build_Version_xml(p_buf+offset, mlen-offset, 17, 12);
		offset += snprintf(p_buf+offset, mlen-offset, "</tds:Service>");
	}

	// event 
	if (g_onvif_cfg.Capabilities.events.support)
	{
		offset += snprintf(p_buf+offset, mlen-offset, 
		    "<tds:Service>"
		    "<tds:Namespace>http://www.onvif.org/ver10/events/wsdl</tds:Namespace>"
		    "<tds:XAddr>%s</tds:XAddr>", 
		    g_onvif_cfg.Capabilities.events.XAddr);	
		if (p_req->IncludeCapability)
		{
			offset += snprintf(p_buf+offset, mlen-offset, "<tds:Capabilities>");
	        offset += build_EventsServicesCapabilities_xml(p_buf+offset, mlen-offset);				
			offset += snprintf(p_buf+offset, mlen-offset, "</tds:Capabilities>");
		}    
		offset += build_Version_xml(p_buf+offset, mlen-offset, 17, 12);
		offset += snprintf(p_buf+offset, mlen-offset, "</tds:Service>");
	}

#ifdef PTZ_SUPPORT
	// ptz
	if (g_onvif_cfg.Capabilities.ptz.support)
	{
		offset += snprintf(p_buf+offset, mlen-offset, 
		    "<tds:Service>"
		    "<tds:Namespace>http://www.onvif.org/ver20/ptz/wsdl</tds:Namespace>"
		    "<tds:XAddr>%s</tds:XAddr>", 
		    g_onvif_cfg.Capabilities.ptz.XAddr);	
		if (p_req->IncludeCapability)
		{
			offset += snprintf(p_buf+offset, mlen-offset, "<tds:Capabilities>");
	        offset += build_PTZServicesCapabilities_xml(p_buf+offset, mlen-offset);				
			offset += snprintf(p_buf+offset, mlen-offset, "</tds:Capabilities>");
		}    
		offset += build_Version_xml(p_buf+offset, mlen-offset, 17, 12);
		offset += snprintf(p_buf+offset, mlen-offset, "</tds:Service>");
	}
#endif	

	// image
	if (g_onvif_cfg.Capabilities.image.support)
	{
		offset += snprintf(p_buf+offset, mlen-offset, 
		    "<tds:Service>"
		    "<tds:Namespace>http://www.onvif.org/ver20/imaging/wsdl</tds:Namespace>"
		    "<tds:XAddr>%s</tds:XAddr>", 
		    g_onvif_cfg.Capabilities.image.XAddr);	
		if (p_req->IncludeCapability)
		{
			offset += snprintf(p_buf+offset, mlen-offset, "<tds:Capabilities>");
	        offset += build_ImagingServicesCapabilities_xml(p_buf+offset, mlen-offset);				
			offset += snprintf(p_buf+offset, mlen-offset, "</tds:Capabilities>");
		}    
		offset += build_Version_xml(p_buf+offset, mlen-offset, 17, 12);
		offset += snprintf(p_buf+offset, mlen-offset, "</tds:Service>");
	}

#ifdef VIDEO_ANALYTICS
	// analytics
	if (g_onvif_cfg.Capabilities.analytics.support)
	{
		offset += snprintf(p_buf+offset, mlen-offset, 
		    "<tds:Service>"
		    "<tds:Namespace>http://www.onvif.org/ver20/analytics/wsdl</tds:Namespace>"
		    "<tds:XAddr>%s</tds:XAddr>", 
		    g_onvif_cfg.Capabilities.analytics.XAddr);	
		if (p_req->IncludeCapability)
		{
			offset += snprintf(p_buf+offset, mlen-offset, "<tds:Capabilities>");
	        offset += build_AnalyticsServicesCapabilities_xml(p_buf+offset, mlen-offset);				
			offset += snprintf(p_buf+offset, mlen-offset, "</tds:Capabilities>");
		}    
		offset += build_Version_xml(p_buf+offset, mlen-offset, 17, 12);
		offset += snprintf(p_buf+offset, mlen-offset, "</tds:Service>");
	}
#endif

#ifdef PROFILE_G_SUPPORT

	// recording
	if (g_onvif_cfg.Capabilities.recording.support)
	{
		offset += snprintf(p_buf+offset, mlen-offset, 
		    "<tds:Service>"
		    "<tds:Namespace>http://www.onvif.org/ver10/recording/wsdl</tds:Namespace>"
		    "<tds:XAddr>%s</tds:XAddr>", 
		    g_onvif_cfg.Capabilities.recording.XAddr);	
		if (p_req->IncludeCapability)
		{
			offset += snprintf(p_buf+offset, mlen-offset, "<tds:Capabilities>");
	        offset += build_RecordingServicesCapabilities_xml(p_buf+offset, mlen-offset);				
			offset += snprintf(p_buf+offset, mlen-offset, "</tds:Capabilities>");
		}    
		offset += build_Version_xml(p_buf+offset, mlen-offset, 17, 12);
		offset += snprintf(p_buf+offset, mlen-offset, "</tds:Service>");
	}

	// search
	if (g_onvif_cfg.Capabilities.search.support)
	{
		offset += snprintf(p_buf+offset, mlen-offset, 
		    "<tds:Service>"
		    "<tds:Namespace>http://www.onvif.org/ver10/search/wsdl</tds:Namespace>"
		    "<tds:XAddr>%s</tds:XAddr>", 
		    g_onvif_cfg.Capabilities.search.XAddr);	
		if (p_req->IncludeCapability)
		{
			offset += snprintf(p_buf+offset, mlen-offset, "<tds:Capabilities>");
	        offset += build_SearchServicesCapabilities_xml(p_buf+offset, mlen-offset);				
			offset += snprintf(p_buf+offset, mlen-offset, "</tds:Capabilities>");
		}    
		offset += build_Version_xml(p_buf+offset, mlen-offset, 17, 12);
		offset += snprintf(p_buf+offset, mlen-offset, "</tds:Service>");
	}

	// replay
	if (g_onvif_cfg.Capabilities.replay.support)
	{
		offset += snprintf(p_buf+offset, mlen-offset, 
		    "<tds:Service>"
		    "<tds:Namespace>http://www.onvif.org/ver10/replay/wsdl</tds:Namespace>"
		    "<tds:XAddr>%s</tds:XAddr>", 
		    g_onvif_cfg.Capabilities.replay.XAddr);	
		if (p_req->IncludeCapability)
		{
			offset += snprintf(p_buf+offset, mlen-offset, "<tds:Capabilities>");
	        offset += build_ReplayServicesCapabilities_xml(p_buf+offset, mlen-offset);				
			offset += snprintf(p_buf+offset, mlen-offset, "</tds:Capabilities>");
		}    
		offset += build_Version_xml(p_buf+offset, mlen-offset, 17, 12);
		offset += snprintf(p_buf+offset, mlen-offset, "</tds:Service>");
	}
	
#endif // end of PROFILE_G_SUPPORT

#ifdef PROFILE_C_SUPPORT

    // access control
    if (g_onvif_cfg.Capabilities.accesscontrol.support)
	{
		offset += snprintf(p_buf+offset, mlen-offset, 
		    "<tds:Service>"
		    "<tds:Namespace>http://www.onvif.org/ver10/accesscontrol/wsdl</tds:Namespace>"
		    "<tds:XAddr>%s</tds:XAddr>", 
		    g_onvif_cfg.Capabilities.accesscontrol.XAddr);	
		if (p_req->IncludeCapability)
		{
			offset += snprintf(p_buf+offset, mlen-offset, "<tds:Capabilities>");
	        offset += build_AccessControlServicesCapabilities_xml(p_buf+offset, mlen-offset);				
			offset += snprintf(p_buf+offset, mlen-offset, "</tds:Capabilities>");
		}    
		offset += build_Version_xml(p_buf+offset, mlen-offset, 17, 12);
		offset += snprintf(p_buf+offset, mlen-offset, "</tds:Service>");
	}
    
    // door control
    if (g_onvif_cfg.Capabilities.doorcontrol.support)
	{
		offset += snprintf(p_buf+offset, mlen-offset, 
		    "<tds:Service>"
		    "<tds:Namespace>http://www.onvif.org/ver10/doorcontrol/wsdl</tds:Namespace>"
		    "<tds:XAddr>%s</tds:XAddr>", 
		    g_onvif_cfg.Capabilities.doorcontrol.XAddr);	
		if (p_req->IncludeCapability)
		{
			offset += snprintf(p_buf+offset, mlen-offset, "<tds:Capabilities>");
	        offset += build_DoorControlServicesCapabilities_xml(p_buf+offset, mlen-offset);				
			offset += snprintf(p_buf+offset, mlen-offset, "</tds:Capabilities>");
		}    
		offset += build_Version_xml(p_buf+offset, mlen-offset, 17, 12);
		offset += snprintf(p_buf+offset, mlen-offset, "</tds:Service>");
	}
	
#endif // end of PROFILE_C_SUPPORT

#ifdef DEVICEIO_SUPPORT
	// deviceio
	if (g_onvif_cfg.Capabilities.deviceIO.support)
	{
		offset += snprintf(p_buf+offset, mlen-offset, 
			"<tds:Service>"
			"<tds:Namespace>http://www.onvif.org/ver10/deviceIO/wsdl</tds:Namespace>"
			"<tds:XAddr>%s</tds:XAddr>", 
			g_onvif_cfg.Capabilities.deviceIO.XAddr);	
		if (p_req->IncludeCapability)
		{
			offset += snprintf(p_buf+offset, mlen-offset, "<tds:Capabilities>");
			offset += build_DeviceIOServicesCapabilities_xml(p_buf+offset, mlen-offset);				
			offset += snprintf(p_buf+offset, mlen-offset, "</tds:Capabilities>");
		}	 
		offset += build_Version_xml(p_buf+offset, mlen-offset, 17, 12);
		offset += snprintf(p_buf+offset, mlen-offset, "</tds:Service>");
	}
#endif // end of DEVICEIO_SUPPORT

#ifdef THERMAL_SUPPORT
    // thermal
    if (g_onvif_cfg.Capabilities.thermal.support)
    {
        offset += snprintf(p_buf+offset, mlen-offset, 
            "<tds:Service>"
            "<tds:Namespace>http://www.onvif.org/ver10/thermal/wsdl</tds:Namespace>"
            "<tds:XAddr>%s</tds:XAddr>", 
            g_onvif_cfg.Capabilities.thermal.XAddr);   
        if (p_req->IncludeCapability)
        {
            offset += snprintf(p_buf+offset, mlen-offset, "<tds:Capabilities>");
            offset += build_ThermalServicesCapabilities_xml(p_buf+offset, mlen-offset);                
            offset += snprintf(p_buf+offset, mlen-offset, "</tds:Capabilities>");
        }    
        offset += build_Version_xml(p_buf+offset, mlen-offset, 17, 12);
        offset += snprintf(p_buf+offset, mlen-offset, "</tds:Service>");
    }
#endif // end of THERMAL_SUPPORT

#ifdef CREDENTIAL_SUPPORT
    // credential
    if (g_onvif_cfg.Capabilities.credential.support)
    {
        offset += snprintf(p_buf+offset, mlen-offset, 
            "<tds:Service>"
            "<tds:Namespace>http://www.onvif.org/ver10/credential/wsdl</tds:Namespace>"
            "<tds:XAddr>%s</tds:XAddr>", 
            g_onvif_cfg.Capabilities.credential.XAddr);   
        if (p_req->IncludeCapability)
        {
            offset += snprintf(p_buf+offset, mlen-offset, "<tds:Capabilities>");
            offset += build_CredentialServicesCapabilities_xml(p_buf+offset, mlen-offset);                
            offset += snprintf(p_buf+offset, mlen-offset, "</tds:Capabilities>");
        }    
        offset += build_Version_xml(p_buf+offset, mlen-offset, 17, 12);
        offset += snprintf(p_buf+offset, mlen-offset, "</tds:Service>");
    }
#endif // end of CREDENTIAL_SUPPORT

#ifdef ACCESS_RULES
    // access rules
    if (g_onvif_cfg.Capabilities.accessrules.support)
    {
        offset += snprintf(p_buf+offset, mlen-offset, 
            "<tds:Service>"
            "<tds:Namespace>http://www.onvif.org/ver10/accessrules/wsdl</tds:Namespace>"
            "<tds:XAddr>%s</tds:XAddr>", 
            g_onvif_cfg.Capabilities.accessrules.XAddr);   
        if (p_req->IncludeCapability)
        {
            offset += snprintf(p_buf+offset, mlen-offset, "<tds:Capabilities>");
            offset += build_AccessRulesServicesCapabilities_xml(p_buf+offset, mlen-offset);                
            offset += snprintf(p_buf+offset, mlen-offset, "</tds:Capabilities>");
        }    
        offset += build_Version_xml(p_buf+offset, mlen-offset, 17, 12);
        offset += snprintf(p_buf+offset, mlen-offset, "</tds:Service>");
    }
#endif // end of ACCESS_RULES

#ifdef SCHEDULE_SUPPORT
    // schedule
    if (g_onvif_cfg.Capabilities.schedule.support)
    {
        offset += snprintf(p_buf+offset, mlen-offset, 
            "<tds:Service>"
            "<tds:Namespace>http://www.onvif.org/ver10/schedule/wsdl</tds:Namespace>"
            "<tds:XAddr>%s</tds:XAddr>", 
            g_onvif_cfg.Capabilities.schedule.XAddr);   
        if (p_req->IncludeCapability)
        {
            offset += snprintf(p_buf+offset, mlen-offset, "<tds:Capabilities>");
            offset += build_ScheduleServicesCapabilities_xml(p_buf+offset, mlen-offset);                
            offset += snprintf(p_buf+offset, mlen-offset, "</tds:Capabilities>");
        }    
        offset += build_Version_xml(p_buf+offset, mlen-offset, 17, 12);
        offset += snprintf(p_buf+offset, mlen-offset, "</tds:Service>");
    }
#endif // end of SCHEDULE_SUPPORT

#ifdef RECEIVER_SUPPORT
    // receiver
    if (g_onvif_cfg.Capabilities.receiver.support)
    {
        offset += snprintf(p_buf+offset, mlen-offset, 
            "<tds:Service>"
            "<tds:Namespace>http://www.onvif.org/ver10/receiver/wsdl</tds:Namespace>"
            "<tds:XAddr>%s</tds:XAddr>", 
            g_onvif_cfg.Capabilities.receiver.XAddr);   
        if (p_req->IncludeCapability)
        {
            offset += snprintf(p_buf+offset, mlen-offset, "<tds:Capabilities>");
            offset += build_ReceiverServicesCapabilities_xml(p_buf+offset, mlen-offset);                
            offset += snprintf(p_buf+offset, mlen-offset, "</tds:Capabilities>");
        }    
        offset += build_Version_xml(p_buf+offset, mlen-offset, 17, 12);
        offset += snprintf(p_buf+offset, mlen-offset, "</tds:Service>");
    }
#endif // end of RECEIVER_SUPPORT

	offset += snprintf(p_buf+offset, mlen-offset, "</tds:GetServicesResponse>");
	
	return offset;
}

int build_GetScopes_rly_xml(char * p_buf, int mlen, const char * argv)
{	
	int i;
	int offset = 0;

    offset += snprintf(p_buf+offset, mlen-offset, "<tds:GetScopesResponse>");

	for (i = 0; i < ARRAY_SIZE(g_onvif_cfg.scopes); i++)
	{
		if (g_onvif_cfg.scopes[i].ScopeItem[0] != '\0')
		{
			offset += snprintf(p_buf+offset, mlen-offset, 
				"<tds:Scopes>"
					"<tt:ScopeDef>%s</tt:ScopeDef>"
					"<tt:ScopeItem>%s</tt:ScopeItem>"
				"</tds:Scopes>",
				onvif_ScopeDefinitionToString(g_onvif_cfg.scopes[i].ScopeDef), 
				g_onvif_cfg.scopes[i].ScopeItem);
		}
	}

#ifdef PROFILE_Q_SUPPORT
    offset += snprintf(p_buf+offset, mlen-offset, "<tds:Scopes>");
    offset += snprintf(p_buf+offset, mlen-offset, "<tt:ScopeDef>Fixed</tt:ScopeDef>");
    offset += snprintf(p_buf+offset, mlen-offset, "<tt:ScopeItem>");
    offset += snprintf(p_buf+offset, mlen-offset, "onvif://www.onvif.org/Profile/Q/");
    
    if (g_onvif_cfg.device_state)
    {
        offset += snprintf(p_buf+offset, mlen-offset, "Operational");
    }
    else
    {
        offset += snprintf(p_buf+offset, mlen-offset, "FactoryDefault");
    }
    offset += snprintf(p_buf+offset, mlen-offset, "</tt:ScopeItem>");
    offset += snprintf(p_buf+offset, mlen-offset, "</tds:Scopes>");
#endif

    offset += snprintf(p_buf+offset, mlen-offset, "</tds:GetScopesResponse>");    	

	return offset;
}

int build_AddScopes_rly_xml(char * p_buf, int mlen, const char * argv)
{
	int offset = 0;
    offset += snprintf(p_buf+offset, mlen-offset, "<tds:AddScopesResponse />");    
	return offset;
}

int build_SetScopes_rly_xml(char * p_buf, int mlen, const char * argv)
{
	int offset = 0;
    offset += snprintf(p_buf+offset, mlen-offset, "<tds:SetScopesResponse />");    
	return offset;
}

int build_RemoveScopes_rly_xml(char * p_buf, int mlen, const char * argv)
{
	int i;
	int offset = 0;
	RemoveScopes_REQ * p_req = (RemoveScopes_REQ *)argv;
	
    offset += snprintf(p_buf+offset, mlen-offset, "<tds:RemoveScopesResponse>");

    for (i = 0; i < ARRAY_SIZE(p_req->ScopeItem); i++)
	{
		if (p_req->ScopeItem[i][0] == '\0')
		{
			break;
		}

		offset += snprintf(p_buf+offset, mlen-offset, "<tds:ScopeItem>%s</tds:ScopeItem>", p_req->ScopeItem[i]);  
	}
	
    offset += snprintf(p_buf+offset, mlen-offset, "</tds:RemoveScopesResponse>");    

	return offset;
}

int build_GetHostname_rly_xml(char * p_buf, int mlen, const char * argv)
{
	int offset = 0;

    offset += snprintf(p_buf+offset, mlen-offset, 
        "<tds:GetHostnameResponse>"
        	"<tds:HostnameInformation>"
	            "<tt:FromDHCP>%s</tt:FromDHCP>"
	            "<tt:Name>%s</tt:Name>"    
       		"</tds:HostnameInformation>"
       	"</tds:GetHostnameResponse>",
      	g_onvif_cfg.network.HostnameInformation.FromDHCP ? "true" : "false",
      	g_onvif_cfg.network.HostnameInformation.Name);

	return offset;
}

int build_SetHostname_rly_xml(char * p_buf, int mlen, const char * argv)
{
	int offset = 0;
    offset += snprintf(p_buf+offset, mlen-offset, "<tds:SetHostnameResponse />");
	return offset;
}

int build_SetHostnameFromDHCP_rly_xml(char * p_buf, int mlen, const char * argv)
{
	int offset = 0;

    offset += snprintf(p_buf+offset, mlen-offset, 
    	"<tds:SetHostnameFromDHCPResponse>"
    		"<tds:RebootNeeded>%s</tds:RebootNeeded>"
    	"</tds:SetHostnameFromDHCPResponse>",
    	g_onvif_cfg.network.HostnameInformation.RebootNeeded ? "true" : "false");
    
	return offset;
}

int build_GetNetworkProtocols_rly_xml(char * p_buf, int mlen, const char * argv)
{
	int i = 0;
	int offset = 0;

	offset += snprintf(p_buf+offset, mlen-offset, "<tds:GetNetworkProtocolsResponse>");

	if (g_onvif_cfg.network.NetworkProtocol.HTTPFlag)
	{
		offset += snprintf(p_buf+offset, mlen-offset, 
			"<tds:NetworkProtocols>"
			"<tt:Name>HTTP</tt:Name>"
			"<tt:Enabled>%s</tt:Enabled>", 
			g_onvif_cfg.network.NetworkProtocol.HTTPEnabled ? "true" : "false");

		for (i = 0; i < ARRAY_SIZE(g_onvif_cfg.network.NetworkProtocol.HTTPPort); i++)
		{
			if (g_onvif_cfg.network.NetworkProtocol.HTTPPort[i] == 0)
			{
				continue;
			}
			
			offset += snprintf(p_buf+offset, mlen-offset, "<tt:Port>%d</tt:Port>", g_onvif_cfg.network.NetworkProtocol.HTTPPort[i]);
		}

		offset += snprintf(p_buf+offset, mlen-offset, "</tds:NetworkProtocols>");
	}

	if (g_onvif_cfg.network.NetworkProtocol.HTTPSFlag)
	{
		offset += snprintf(p_buf+offset, mlen-offset, 
			"<tds:NetworkProtocols>"
			"<tt:Name>HTTPS</tt:Name>"
			"<tt:Enabled>%s</tt:Enabled>", 
			g_onvif_cfg.network.NetworkProtocol.HTTPSEnabled ? "true" : "false");

		for (i = 0; i < ARRAY_SIZE(g_onvif_cfg.network.NetworkProtocol.HTTPSPort); i++)
		{
			if (g_onvif_cfg.network.NetworkProtocol.HTTPSPort[i] == 0)
			{
				continue;
			}
			
			offset += snprintf(p_buf+offset, mlen-offset, "<tt:Port>%d</tt:Port>", g_onvif_cfg.network.NetworkProtocol.HTTPSPort[i]);
		}

		offset += snprintf(p_buf+offset, mlen-offset, "</tds:NetworkProtocols>");
	}

	if (g_onvif_cfg.network.NetworkProtocol.RTSPFlag)
	{
		offset += snprintf(p_buf+offset, mlen-offset, 
			"<tds:NetworkProtocols>"
			"<tt:Name>RTSP</tt:Name>"
			"<tt:Enabled>%s</tt:Enabled>", 
			g_onvif_cfg.network.NetworkProtocol.RTSPEnabled ? "true" : "false");

		for (i = 0; i < ARRAY_SIZE(g_onvif_cfg.network.NetworkProtocol.RTSPPort); i++)
		{
			if (g_onvif_cfg.network.NetworkProtocol.RTSPPort[i] == 0)
			{
				continue;
			}
			
			offset += snprintf(p_buf+offset, mlen-offset, "<tt:Port>%d</tt:Port>", g_onvif_cfg.network.NetworkProtocol.RTSPPort[i]);
		}

		offset += snprintf(p_buf+offset, mlen-offset, "</tds:NetworkProtocols>");
	}
	
	offset += snprintf(p_buf+offset, mlen-offset, "</tds:GetNetworkProtocolsResponse>");	

	return offset;
}

int build_SetNetworkProtocols_rly_xml(char * p_buf, int mlen, const char * argv)
{
	int offset = 0;
	offset += snprintf(p_buf+offset, mlen-offset, "<tds:SetNetworkProtocolsResponse />");
	return offset;
}

int build_GetNetworkDefaultGateway_rly_xml(char * p_buf, int mlen, const char * argv)
{
	int i;	
	int offset = 0;
	
	offset += snprintf(p_buf+offset, mlen-offset, "<tds:GetNetworkDefaultGatewayResponse><tds:NetworkGateway>");

	for (i = 0; i < ARRAY_SIZE(g_onvif_cfg.network.NetworkGateway.IPv4Address); i++)
	{
		if (g_onvif_cfg.network.NetworkGateway.IPv4Address[i][0] != '\0')
		{
			offset += snprintf(p_buf+offset, mlen-offset, "<tt:IPv4Address>%s</tt:IPv4Address>", g_onvif_cfg.network.NetworkGateway.IPv4Address[i]);
		}
	}
	
	offset += snprintf(p_buf+offset, mlen-offset, "</tds:NetworkGateway></tds:GetNetworkDefaultGatewayResponse>");		

	return offset;
}

int build_SetNetworkDefaultGateway_rly_xml(char * p_buf, int mlen, const char * argv)
{
	int offset = 0;	
	offset += snprintf(p_buf+offset, mlen-offset, "<tds:SetNetworkDefaultGatewayResponse />");
	return offset;
}

int build_GetDiscoveryMode_rly_xml(char * p_buf, int mlen, const char * argv)
{
	int offset = 0;
	
	offset += snprintf(p_buf+offset, mlen-offset, 
		"<tds:GetDiscoveryModeResponse>"
   			"<tds:DiscoveryMode>%s</tds:DiscoveryMode>"
	  	"</tds:GetDiscoveryModeResponse>", 
	  	onvif_DiscoveryModeToString(g_onvif_cfg.network.DiscoveryMode));	

	return offset;
}

int build_SetDiscoveryMode_rly_xml(char * p_buf, int mlen, const char * argv)
{
	int offset = 0;
	offset += snprintf(p_buf+offset, mlen-offset, "<tds:SetDiscoveryModeResponse />");	
	return offset;
}

int build_GetDNS_rly_xml(char * p_buf, int mlen, const char * argv)
{
	int i;
	int offset = 0;

	offset += snprintf(p_buf+offset, mlen-offset, "<tds:GetDNSResponse><tds:DNSInformation>");
	offset += snprintf(p_buf+offset, mlen-offset, "<tt:FromDHCP>%s</tt:FromDHCP>", g_onvif_cfg.network.DNSInformation.FromDHCP ? "true" : "false");

	if (g_onvif_cfg.network.DNSInformation.SearchDomainFlag)
	{
		for (i = 0; i < ARRAY_SIZE(g_onvif_cfg.network.DNSInformation.SearchDomain); i++)
		{
			if (g_onvif_cfg.network.DNSInformation.SearchDomain[i][0] == '\0')
			{
				continue;
			}

			offset += snprintf(p_buf+offset, mlen-offset, "<tt:SearchDomain>%s</tt:SearchDomain>", g_onvif_cfg.network.DNSInformation.SearchDomain[i]);
		}
	}
	
	for (i = 0; i < ARRAY_SIZE(g_onvif_cfg.network.DNSInformation.DNSServer); i++)
	{
		if (g_onvif_cfg.network.DNSInformation.DNSServer[i][0] == '\0')
		{
			continue;
		}
		
		if (g_onvif_cfg.network.DNSInformation.FromDHCP)
		{
			offset += snprintf(p_buf+offset, mlen-offset, "<tt:DNSFromDHCP>");
		}
		else
		{
			offset += snprintf(p_buf+offset, mlen-offset, "<tt:DNSManual>");
		}

		offset += snprintf(p_buf+offset, mlen-offset, 
			"<tt:Type>IPv4</tt:Type><tt:IPv4Address>%s</tt:IPv4Address>",
			g_onvif_cfg.network.DNSInformation.DNSServer[i]);

		if (g_onvif_cfg.network.DNSInformation.FromDHCP)
		{
			offset += snprintf(p_buf+offset, mlen-offset, "</tt:DNSFromDHCP>");
		}
		else
		{
			offset += snprintf(p_buf+offset, mlen-offset, "</tt:DNSManual>");
		}
	}
	
	offset += snprintf(p_buf+offset, mlen-offset, "</tds:DNSInformation></tds:GetDNSResponse>");	

	return offset;
}

int build_SetDNS_rly_xml(char * p_buf, int mlen, const char * argv)
{
	int offset = 0;
	offset += snprintf(p_buf+offset, mlen-offset, "<tds:SetDNSResponse />");	
	return offset;
}

int build_GetNTP_rly_xml(char * p_buf, int mlen, const char * argv)
{
	int i;
	int offset = 0;

	offset += snprintf(p_buf+offset, mlen-offset, "<tds:GetNTPResponse><tds:NTPInformation>");

	offset += snprintf(p_buf+offset, mlen-offset, 
	    "<tt:FromDHCP>%s</tt:FromDHCP>", 
	    g_onvif_cfg.network.NTPInformation.FromDHCP ? "true" : "false");

	for (i = 0; i < ARRAY_SIZE(g_onvif_cfg.network.NTPInformation.NTPServer); i++)
	{
		if (g_onvif_cfg.network.NTPInformation.NTPServer[i][0] == '\0')
		{
			continue;
		}
		
		if (g_onvif_cfg.network.NTPInformation.FromDHCP)
		{
			offset += snprintf(p_buf+offset, mlen-offset, "<tt:NTPFromDHCP>");
		}
		else
		{
			offset += snprintf(p_buf+offset, mlen-offset, "<tt:NTPManual>");
		}
		
		if (is_ip_address(g_onvif_cfg.network.NTPInformation.NTPServer[i]))
		{
		    offset += snprintf(p_buf+offset, mlen-offset, "<tt:Type>IPv4</tt:Type>");
			offset += snprintf(p_buf+offset, mlen-offset, "<tt:IPv4Address>%s</tt:IPv4Address>", g_onvif_cfg.network.NTPInformation.NTPServer[i]);
		}
		else
		{
		    offset += snprintf(p_buf+offset, mlen-offset, "<tt:Type>DNS</tt:Type>");
			offset += snprintf(p_buf+offset, mlen-offset, "<tt:DNSname>%s</tt:DNSname>", g_onvif_cfg.network.NTPInformation.NTPServer[i]);
		}

		if (g_onvif_cfg.network.NTPInformation.FromDHCP)
		{
			offset += snprintf(p_buf+offset, mlen-offset, "</tt:NTPFromDHCP>");
		}
		else
		{
			offset += snprintf(p_buf+offset, mlen-offset, "</tt:NTPManual>");
		}
	}
	
	offset += snprintf(p_buf+offset, mlen-offset, "</tds:NTPInformation></tds:GetNTPResponse>");	

	return offset;
}

int build_SetNTP_rly_xml(char * p_buf, int mlen, const char * argv)
{
	int offset = 0;
	offset += snprintf(p_buf+offset, mlen-offset, "<tds:SetNTPResponse />");	
	return offset;
}

int build_GetZeroConfiguration_rly_xml(char * p_buf, int mlen, const char * argv)
{
    int i;
    int offset = 0;

	offset += snprintf(p_buf+offset, mlen-offset, "<tds:GetZeroConfigurationResponse><tds:ZeroConfiguration>");	
	offset += snprintf(p_buf+offset, mlen-offset, 
	    "<tt:InterfaceToken>%s</tt:InterfaceToken>"
	    "<tt:Enabled>%s</tt:Enabled>",
	    g_onvif_cfg.network.ZeroConfiguration.InterfaceToken,
	    g_onvif_cfg.network.ZeroConfiguration.Enabled ? "true" : "false");
    for (i = 0; i < g_onvif_cfg.network.ZeroConfiguration.sizeAddresses; i++)
    {
        offset += snprintf(p_buf+offset, mlen-offset, 
            "<tt:Addresses>%s</tt:Addresses>",
            g_onvif_cfg.network.ZeroConfiguration.Addresses[i]);	
    }
	offset += snprintf(p_buf+offset, mlen-offset, "</tds:ZeroConfiguration></tds:GetZeroConfigurationResponse>");	

	return offset;
}

int build_SetZeroConfiguration_rly_xml(char * p_buf, int mlen, const char * argv)
{
    int offset = 0;
	offset += snprintf(p_buf+offset, mlen-offset, "<tds:SetZeroConfigurationResponse />");	
	return offset;
}

int build_GetDot11Capabilities_rly_xml(char * p_buf, int mlen, const char * argv)
{
    int offset = 0;

	offset += snprintf(p_buf+offset, mlen-offset, 
	    "<tds:GetDot11CapabilitiesResponse>"
            "<tds:Capabilities>"
                "<tt:TKIP>%s</tt:TKIP>"
                "<tt:ScanAvailableNetworks>%s</tt:ScanAvailableNetworks>"
                "<tt:MultipleConfiguration>%s</tt:MultipleConfiguration>"
                "<tt:AdHocStationMode>%s</tt:AdHocStationMode>"
                "<tt:WEP>%s</tt:WEP>"
            "</tds:Capabilities>"    
	    "</tds:GetDot11CapabilitiesResponse>",
	    g_onvif_cfg.Capabilities.dot11.TKIP ? "true" : "false",
	    g_onvif_cfg.Capabilities.dot11.ScanAvailableNetworks ? "true" : "false",
	    g_onvif_cfg.Capabilities.dot11.MultipleConfiguration ? "true" : "false",
	    g_onvif_cfg.Capabilities.dot11.AdHocStationMode ? "true" : "false",
	    g_onvif_cfg.Capabilities.dot11.WEP ? "true" : "false");

	return offset;
}

int build_GetDot11Status_rly_xml(char * p_buf, int mlen, const char * argv)
{
    int offset = 0;
    GetDot11Status_RES * p_res = (GetDot11Status_RES *)argv;
    
	offset += snprintf(p_buf+offset, mlen-offset, "<tds:GetDot11StatusResponse><tds:Status>");
	
	offset += snprintf(p_buf+offset, mlen-offset, "<tt:SSID>%s</tt:SSID>", p_res->Status.SSID);

    if (p_res->Status.BSSIDFlag)
    {
	    offset += snprintf(p_buf+offset, mlen-offset, "<tt:BSSID>%s</tt:BSSID>", p_res->Status.BSSID);
	}

	if (p_res->Status.PairCipherFlag)
    {
	    offset += snprintf(p_buf+offset, mlen-offset, "<tt:PairCipher>%s</tt:PairCipher>", 
	        onvif_Dot11CipherToString(p_res->Status.PairCipher));
	}

	if (p_res->Status.GroupCipherFlag)
    {
	    offset += snprintf(p_buf+offset, mlen-offset, "<tt:GroupCipher>%s</tt:GroupCipher>", 
	        onvif_Dot11CipherToString(p_res->Status.GroupCipher));
	}

	if (p_res->Status.SignalStrengthFlag)
    {
	    offset += snprintf(p_buf+offset, mlen-offset, "<tt:SignalStrength>%s</tt:SignalStrength>", 
	        onvif_Dot11SignalStrengthToString(p_res->Status.SignalStrength));
	}

	offset += snprintf(p_buf+offset, mlen-offset, "<tt:ActiveConfigAlias>%s</tt:ActiveConfigAlias>", p_res->Status.ActiveConfigAlias);
	
	offset += snprintf(p_buf+offset, mlen-offset, "</tds:Status></tds:GetDot11StatusResponse>");

	return offset;    
}

int build_Dot11AvailableNetworks(char * p_buf, int mlen, onvif_Dot11AvailableNetworks * p_req)
{
    int i;
    int offset = 0;

    offset += snprintf(p_buf+offset, mlen-offset, "<tt:SSID>%s</tt:SSID>", p_req->BSSID);

    if (p_req->BSSIDFlag)
    {
	    offset += snprintf(p_buf+offset, mlen-offset, "<tt:BSSID>%s</tt:BSSID>", p_req->BSSID);
	}

	for (i = 0; i < p_req->sizeAuthAndMangementSuite; i++)
	{
	    offset += snprintf(p_buf+offset, mlen-offset, "<tt:AuthAndMangementSuite>%s</tt:AuthAndMangementSuite>", 
	        onvif_Dot11AuthAndMangementSuiteToString(p_req->AuthAndMangementSuite[i]));
	}

	for (i = 0; i < p_req->sizePairCipher; i++)
	{
	    offset += snprintf(p_buf+offset, mlen-offset, "<tt:PairCipher>%s</tt:PairCipher>", 
	        onvif_Dot11CipherToString(p_req->PairCipher[i]));
	}

	for (i = 0; i < p_req->sizeGroupCipher; i++)
	{
	    offset += snprintf(p_buf+offset, mlen-offset, "<tt:GroupCipher>%s</tt:GroupCipher>", 
	        onvif_Dot11CipherToString(p_req->GroupCipher[i]));
	}

	if (p_req->SignalStrengthFlag)
    {
	    offset += snprintf(p_buf+offset, mlen-offset, "<tt:SignalStrength>%s</tt:SignalStrength>", 
	        onvif_Dot11SignalStrengthToString(p_req->SignalStrength));
	}

    return offset;
}

int build_ScanAvailableDot11Networks_rly_xml(char * p_buf, int mlen, const char * argv)
{
    int i;
    int offset = 0;
    ScanAvailableDot11Networks_RES * p_res = (ScanAvailableDot11Networks_RES *)argv;
    
	offset += snprintf(p_buf+offset, mlen-offset, "<tds:ScanAvailableDot11NetworksResponse>");

    for (i = 0; i < p_res->sizeNetworks; i++)
    {
        offset += snprintf(p_buf+offset, mlen-offset, "<tds:Networks>");
        offset += build_Dot11AvailableNetworks(p_buf+offset, mlen-offset, &p_res->Networks[i]);
        offset += snprintf(p_buf+offset, mlen-offset, "</tds:Networks>");
    }
	
	offset += snprintf(p_buf+offset, mlen-offset, "</tds:ScanAvailableDot11NetworksResponse>");

	return offset; 
}

int build_GetServiceCapabilities_rly_xml(char * p_buf, int mlen, const char * argv)
{
    int offset = 0;
	onvif_CapabilityCategory category = *(onvif_CapabilityCategory *)argv;

	if (CapabilityCategory_Device == category)
	{
		offset += snprintf(p_buf+offset, mlen-offset, "<tds:GetServiceCapabilitiesResponse>");
		offset += build_DeviceServicesCapabilities_xml(p_buf+offset, mlen-offset);
		offset += snprintf(p_buf+offset, mlen-offset, "</tds:GetServiceCapabilitiesResponse>");
	}
#ifdef MEDIA2_SUPPORT
    else if (CapabilityCategory_Media2 == category)
    {
        offset += snprintf(p_buf+offset, mlen-offset, "<tr2:GetServiceCapabilitiesResponse>");
	    offset += build_MediaServicesCapabilities2_xml(p_buf+offset, mlen-offset);
	    offset += snprintf(p_buf+offset, mlen-offset, "</tr2:GetServiceCapabilitiesResponse>");
    }
#endif
	else if (CapabilityCategory_Media == category)
	{
		offset += snprintf(p_buf+offset, mlen-offset, "<trt:GetServiceCapabilitiesResponse>");
		offset += build_MediaServicesCapabilities_xml(p_buf+offset, mlen-offset);
		offset += snprintf(p_buf+offset, mlen-offset, "</trt:GetServiceCapabilitiesResponse>");
	}
	else if (CapabilityCategory_Events == category)
	{
		offset += snprintf(p_buf+offset, mlen-offset, "<tev:GetServiceCapabilitiesResponse>");
		offset += build_EventsServicesCapabilities_xml(p_buf+offset, mlen-offset);
		offset += snprintf(p_buf+offset, mlen-offset, "</tev:GetServiceCapabilitiesResponse>");
	}
#ifdef PTZ_SUPPORT	
	else if (CapabilityCategory_PTZ == category)
	{
		offset += snprintf(p_buf+offset, mlen-offset, "<tptz:GetServiceCapabilitiesResponse>");
		offset += build_PTZServicesCapabilities_xml(p_buf+offset, mlen-offset);
		offset += snprintf(p_buf+offset, mlen-offset, "</tptz:GetServiceCapabilitiesResponse>");
	}
#endif	
	else if (CapabilityCategory_Imaging == category)
	{
		offset += snprintf(p_buf+offset, mlen-offset, "<timg:GetServiceCapabilitiesResponse>");
		offset += build_ImagingServicesCapabilities_xml(p_buf+offset, mlen-offset);
		offset += snprintf(p_buf+offset, mlen-offset, "</timg:GetServiceCapabilitiesResponse>");
	}
#ifdef VIDEO_ANALYTICS	
	else if (CapabilityCategory_Analytics == category)
	{
		offset += snprintf(p_buf+offset, mlen-offset, "<tan:GetServiceCapabilitiesResponse>");
		offset += build_AnalyticsServicesCapabilities_xml(p_buf+offset, mlen-offset);
		offset += snprintf(p_buf+offset, mlen-offset, "</tan:GetServiceCapabilitiesResponse>");
	}	
#endif	
#ifdef PROFILE_G_SUPPORT
	else if (CapabilityCategory_Recording == category)
	{
		offset += snprintf(p_buf+offset, mlen-offset, "<trc:GetServiceCapabilitiesResponse>");
		offset += build_RecordingServicesCapabilities_xml(p_buf+offset, mlen-offset);
		offset += snprintf(p_buf+offset, mlen-offset, "</trc:GetServiceCapabilitiesResponse>");
	}
	else if (CapabilityCategory_Search == category)
	{
		offset += snprintf(p_buf+offset, mlen-offset, "<tse:GetServiceCapabilitiesResponse>");
		offset += build_SearchServicesCapabilities_xml(p_buf+offset, mlen-offset);
		offset += snprintf(p_buf+offset, mlen-offset, "</tse:GetServiceCapabilitiesResponse>");
	}
	else if (CapabilityCategory_Replay == category)
	{
		offset += snprintf(p_buf+offset, mlen-offset, "<trp:GetServiceCapabilitiesResponse>");
		offset += build_ReplayServicesCapabilities_xml(p_buf+offset, mlen-offset);
		offset += snprintf(p_buf+offset, mlen-offset, "</trp:GetServiceCapabilitiesResponse>");
	}
#endif
#ifdef PROFILE_C_SUPPORT
    else if (CapabilityCategory_AccessControl == category)
	{
		offset += snprintf(p_buf+offset, mlen-offset, "<tac:GetServiceCapabilitiesResponse>");
		offset += build_AccessControlServicesCapabilities_xml(p_buf+offset, mlen-offset);
		offset += snprintf(p_buf+offset, mlen-offset, "</tac:GetServiceCapabilitiesResponse>");
	}
	else if (CapabilityCategory_DoorControl == category)
	{
		offset += snprintf(p_buf+offset, mlen-offset, "<tdc:GetServiceCapabilitiesResponse>");
		offset += build_DoorControlServicesCapabilities_xml(p_buf+offset, mlen-offset);
		offset += snprintf(p_buf+offset, mlen-offset, "</tdc:GetServiceCapabilitiesResponse>");
	}
#endif
#ifdef DEVICEIO_SUPPORT
	else if (CapabilityCategory_DeviceIO == category)
	{
		offset += snprintf(p_buf+offset, mlen-offset, "<tmd:GetServiceCapabilitiesResponse>");
		offset += build_DeviceIOServicesCapabilities_xml(p_buf+offset, mlen-offset);
		offset += snprintf(p_buf+offset, mlen-offset, "</tmd:GetServiceCapabilitiesResponse>");
	}
#endif
#ifdef THERMAL_SUPPORT
    else if (CapabilityCategory_Thermal == category)
	{
		offset += snprintf(p_buf+offset, mlen-offset, "<tth:GetServiceCapabilitiesResponse>");
		offset += build_ThermalServicesCapabilities_xml(p_buf+offset, mlen-offset);
		offset += snprintf(p_buf+offset, mlen-offset, "</tth:GetServiceCapabilitiesResponse>");
	}
#endif
#ifdef CREDENTIAL_SUPPORT
    else if (CapabilityCategory_Credential == category)
	{
		offset += snprintf(p_buf+offset, mlen-offset, "<tcr:GetServiceCapabilitiesResponse>");
		offset += build_CredentialServicesCapabilities_xml(p_buf+offset, mlen-offset);
		offset += snprintf(p_buf+offset, mlen-offset, "</tcr:GetServiceCapabilitiesResponse>");
	}
#endif
#ifdef ACCESS_RULES
    else if (CapabilityCategory_AccessRules == category)
    {
        offset += snprintf(p_buf+offset, mlen-offset, "<tar:GetServiceCapabilitiesResponse>");
        offset += build_AccessRulesServicesCapabilities_xml(p_buf+offset, mlen-offset);
        offset += snprintf(p_buf+offset, mlen-offset, "</tar:GetServiceCapabilitiesResponse>");
    }
#endif
#ifdef SCHEDULE_SUPPORT
    else if (CapabilityCategory_Schedule == category)
    {
        offset += snprintf(p_buf+offset, mlen-offset, "<tsc:GetServiceCapabilitiesResponse>");
        offset += build_ScheduleServicesCapabilities_xml(p_buf+offset, mlen-offset);
        offset += snprintf(p_buf+offset, mlen-offset, "</tsc:GetServiceCapabilitiesResponse>");
    }
#endif
#ifdef RECEIVER_SUPPORT
    else if (CapabilityCategory_Receiver == category)
    {
        offset += snprintf(p_buf+offset, mlen-offset, "<trv:GetServiceCapabilitiesResponse>");
        offset += build_ReceiverServicesCapabilities_xml(p_buf+offset, mlen-offset);
        offset += snprintf(p_buf+offset, mlen-offset, "</trv:GetServiceCapabilitiesResponse>");
    }
#endif

	return offset;
}

#ifdef PROFILE_Q_SUPPORT

int build_Base_EventProperties_xml(char * p_buf, int mlen)
{
    int offset = snprintf(p_buf, mlen, 
        // tns1:Monitoring/ProcessorUsage
        "<tns1:Monitoring wstop:topic=\"true\">"
    		"<ProcessorUsage wstop:topic=\"true\">"
    		"<tt:MessageDescription IsProperty=\"true\">"
    		"<tt:Source>"
    		    "<tt:SimpleItemDescription Name=\"Token\" Type=\"tt:ReferenceToken\"/>"
    		"</tt:Source>"
    		"<tt:Data>"
    		    "<tt:SimpleItemDescription Name=\"Value\" Type=\"xs:float\"/>"
    		"</tt:Data>"					
    		"</tt:MessageDescription>"
    		"</ProcessorUsage>"
    	"</tns1:Monitoring>"

    	// tns1:Monitoring/OperatingTime/LastReset
        "<tns1:Monitoring wstop:topic=\"true\">"
    		"<OperatingTime wstop:topic=\"true\">"
        		"<LastReset wstop:topic=\"true\">"
        		"<tt:MessageDescription IsProperty=\"true\">"
        		"<tt:Source>"        		    
        		"</tt:Source>"
        		"<tt:Data>"
        		    "<tt:SimpleItemDescription Name=\"Status\" Type=\"xs:dateTime\"/>"
        		"</tt:Data>"
        		"</tt:MessageDescription>"
        		"</LastReset>"
    		"</OperatingTime>"
    	"</tns1:Monitoring>"

    	// tns1:Monitoring/OperatingTime/LastReboot
        "<tns1:Monitoring wstop:topic=\"true\">"
    		"<OperatingTime wstop:topic=\"true\">"
        		"<LastReboot wstop:topic=\"true\">"
        		"<tt:MessageDescription IsProperty=\"true\">"
        		"<tt:Source>"        		    
        		"</tt:Source>"
        		"<tt:Data>"
        		    "<tt:SimpleItemDescription Name=\"Status\" Type=\"xs:dateTime\"/>"
        		"</tt:Data>"
        		"</tt:MessageDescription>"
        		"</LastReboot>"
    		"</OperatingTime>"
    	"</tns1:Monitoring>"

    	// tns1:Monitoring/OperatingTime/LastClockSynchronization
        "<tns1:Monitoring wstop:topic=\"true\">"
    		"<OperatingTime wstop:topic=\"true\">"
        		"<LastClockSynchronization wstop:topic=\"true\">"
        		"<tt:MessageDescription IsProperty=\"true\">"
        		"<tt:Source>"        		    
        		"</tt:Source>"
        		"<tt:Data>"
        		    "<tt:SimpleItemDescription Name=\"Status\" Type=\"xs:dateTime\"/>"
        		"</tt:Data>"
        		"</tt:MessageDescription>"
        		"</LastClockSynchronization>"
    		"</OperatingTime>"
    	"</tns1:Monitoring>"
    );

    return offset;
}

#endif // PROFILE_Q_SUPPORT

int build_Imaging_EventProperties_xml(char * p_buf, int mlen)
{
    int offset = snprintf(p_buf, mlen, 

        // tns1:VideoSource/ImageTooBlurry/ImagingService
        "<tns1:VideoSource wstop:topic=\"true\">"
    		"<ImageTooBlurry wstop:topic=\"true\">"
        		"<ImagingService wstop:topic=\"true\">"
        		"<tt:MessageDescription IsProperty=\"true\">"
        		"<tt:Source>"
        		    "<tt:SimpleItemDescription Name=\"Source\" Type=\"tt:ReferenceToken\"/>"
        		"</tt:Source>"
        		"<tt:Data>"
        		    "<tt:SimpleItemDescription Name=\"State\" Type=\"xs:boolean\"/>"
        		"</tt:Data>"					
        		"</tt:MessageDescription>"
        		"</ImagingService>"
    		"</ImageTooBlurry>"
    	"</tns1:VideoSource>"

    	// tns1:VideoSource/ImageTooDark/ImagingService
        "<tns1:VideoSource wstop:topic=\"true\">"
    		"<ImageTooDark wstop:topic=\"true\">"
        		"<ImagingService wstop:topic=\"true\">"
        		"<tt:MessageDescription IsProperty=\"true\">"
        		"<tt:Source>"
        		    "<tt:SimpleItemDescription Name=\"Source\" Type=\"tt:ReferenceToken\"/>"
        		"</tt:Source>"
        		"<tt:Data>"
        		    "<tt:SimpleItemDescription Name=\"State\" Type=\"xs:boolean\"/>"
        		"</tt:Data>"					
        		"</tt:MessageDescription>"
        		"</ImagingService>"
    		"</ImageTooDark>"
    	"</tns1:VideoSource>"

    	// tns1:VideoSource/ImageTooBright/ImagingService
        "<tns1:VideoSource wstop:topic=\"true\">"
    		"<ImageTooBright wstop:topic=\"true\">"
        		"<ImagingService wstop:topic=\"true\">"
        		"<tt:MessageDescription IsProperty=\"true\">"
        		"<tt:Source>"
        		    "<tt:SimpleItemDescription Name=\"Source\" Type=\"tt:ReferenceToken\"/>"
        		"</tt:Source>"
        		"<tt:Data>"
        		    "<tt:SimpleItemDescription Name=\"State\" Type=\"xs:boolean\"/>"
        		"</tt:Data>"					
        		"</tt:MessageDescription>"
        		"</ImagingService>"
    		"</ImageTooBright>"
    	"</tns1:VideoSource>"

    	// tns1:VideoSource/GlobalSceneChange/ImagingService
        "<tns1:VideoSource wstop:topic=\"true\">"
    		"<GlobalSceneChange wstop:topic=\"true\">"
        		"<ImagingService wstop:topic=\"true\">"
        		"<tt:MessageDescription IsProperty=\"true\">"
        		"<tt:Source>"
        		    "<tt:SimpleItemDescription Name=\"Source\" Type=\"tt:ReferenceToken\"/>"
        		"</tt:Source>"
        		"<tt:Data>"
        		    "<tt:SimpleItemDescription Name=\"State\" Type=\"xs:boolean\"/>"
        		"</tt:Data>"					
        		"</tt:MessageDescription>"
        		"</ImagingService>"
    		"</GlobalSceneChange>"
    	"</tns1:VideoSource>"

    	// tns1:VideoSource/SignalLoss
        "<tns1:VideoSource wstop:topic=\"true\">"
    		"<SignalLoss wstop:topic=\"true\">"
    		"<tt:MessageDescription IsProperty=\"true\">"
    		"<tt:Source>"
    		    "<tt:SimpleItemDescription Name=\"Source\" Type=\"tt:ReferenceToken\"/>"
    		"</tt:Source>"
    		"<tt:Data>"
    		    "<tt:SimpleItemDescription Name=\"State\" Type=\"xs:boolean\"/>"
    		"</tt:Data>"					
    		"</tt:MessageDescription>"
    		"</SignalLoss>"
    	"</tns1:VideoSource>"

    	// tns1:VideoSource/MotionAlarm
		"<tns1:VideoSource wstop:topic=\"true\">"
			"<MotionAlarm wstop:topic=\"true\">"
			"<tt:MessageDescription IsProperty=\"true\">"
			"<tt:Source>"
				"<tt:SimpleItemDescription Name=\"Source\" Type=\"tt:ReferenceToken\"/>"
			"</tt:Source>"
			"<tt:Data>"
			    "<tt:SimpleItemDescription Name=\"State\" Type=\"xs:boolean\"/>"
			"</tt:Data>"					
			"</tt:MessageDescription>"	
			"</MotionAlarm>"
		"</tns1:VideoSource>"
    );

    return offset;
}

#ifdef VIDEO_ANALYTICS

int build_Analytics_EventProperties_xml(char * p_buf, int mlen)
{
    int offset = snprintf(p_buf, mlen, 
        // tns1:RuleEngine/MotionRegionDetector/Motion
		"<tns1:RuleEngine wstop:topic=\"true\">"
			"<MotionRegionDetector wstop:topic=\"true\">"
			"<Motion wstop:topic=\"true\">"
			"<tt:MessageDescription IsProperty=\"true\">"
			"<tt:Source>"
				"<tt:SimpleItemDescription Name=\"VideoSource\" Type=\"tt:ReferenceToken\"/>"
				"<tt:SimpleItemDescription Name=\"RuleName\" Type=\"xs:string\"/>"
			"</tt:Source>"
			"<tt:Data>"
			    "<tt:SimpleItemDescription Name=\"State\" Type=\"xs:boolean\"/>"
			"</tt:Data>"					
			"</tt:MessageDescription>"					
			"</Motion>"
			"</MotionRegionDetector>"
		"</tns1:RuleEngine>"
    );

    return offset;        
}

#endif

#ifdef DEVICEIO_SUPPORT

int build_DeviceIO_EventProperties_xml(char * p_buf, int mlen)
{
    int offset = snprintf(p_buf, mlen, 
        // tns1:Device/Trigger/DigitalInput
        "<tns1:Device wstop:topic=\"true\">"
    		"<Trigger wstop:topic=\"true\">"
    		"<DigitalInput wstop:topic=\"true\">"
    		"<tt:MessageDescription IsProperty=\"true\">"
    		"<tt:Source>"
    		    "<tt:SimpleItemDescription Name=\"InputToken\" Type=\"tt:ReferenceToken\" />"
    		"</tt:Source>"
    		"<tt:Data>"
    		    "<tt:SimpleItemDescription Name=\"LogicalState\" Type=\"xs:boolean\" />"
    		"</tt:Data>"					
    		"</tt:MessageDescription>"
    		"</DigitalInput>"
    		"</Trigger>"
    	"</tns1:Device>"

    	// tns1:Device/Trigger/Relay
        "<tns1:Device wstop:topic=\"true\">"
    		"<Trigger wstop:topic=\"true\">"
    		"<Relay wstop:topic=\"true\">"
    		"<tt:MessageDescription IsProperty=\"true\">"
    		"<tt:Source>"
    		    "<tt:SimpleItemDescription Name=\"RelayToken\" Type=\"tt:ReferenceToken\" />"
    		"</tt:Source>"
    		"<tt:Data>"
    		    "<tt:SimpleItemDescription Name=\"LogicalState\" Type=\"tt:RelayLogicalState\" />"
    		"</tt:Data>"					
    		"</tt:MessageDescription>"
    		"</Relay>"
    		"</Trigger>"
    	"</tns1:Device>"
    );

    return offset;
}

#endif // end if DEVICEIO_SUPPORT

#ifdef PROFILE_G_SUPPORT

int build_ProfileG_EventProperties_xml(char * p_buf, int mlen)
{
    int offset = snprintf(p_buf, mlen, 
        // tns1:RecordingConfig/CreateRecording
        "<tns1:RecordingConfig wstop:topic=\"true\">"
    		"<CreateRecording wstop:topic=\"true\">"
    		"<tt:MessageDescription IsProperty=\"false\">"
    		"<tt:Source>"
    		    "<tt:SimpleItemDescription Name=\"RecordingToken\" Type=\"tt:RecordingReference\"/>"
    		"</tt:Source>"
    		"<tt:Data>"
    		"</tt:Data>"					
    		"</tt:MessageDescription>"
    		"</CreateRecording>"
    	"</tns1:RecordingConfig>"

    	// tns1:RecordingConfig/DeleteRecording
        "<tns1:RecordingConfig wstop:topic=\"true\">"
    		"<DeleteRecording wstop:topic=\"true\">"
    		"<tt:MessageDescription IsProperty=\"false\">"
    		"<tt:Source>"
    		    "<tt:SimpleItemDescription Name=\"RecordingToken\" Type=\"tt:RecordingReference\"/>"
    		"</tt:Source>"
    		"<tt:Data>"
    		"</tt:Data>"					
    		"</tt:MessageDescription>"
    		"</DeleteRecording>"
    	"</tns1:RecordingConfig>"

    	// tns1:RecordingConfig/CreateTrack
        "<tns1:RecordingConfig wstop:topic=\"true\">"
    		"<CreateTrack wstop:topic=\"true\">"
    		"<tt:MessageDescription IsProperty=\"false\">"
    		"<tt:Source>"
    		    "<tt:SimpleItemDescription Name=\"RecordingToken\" Type=\"tt:RecordingReference\"/>"
    		    "<tt:SimpleItemDescription Name=\"TrackToken\" Type=\"tt:TrackReference\"/>"
    		"</tt:Source>"
    		"<tt:Data>"
    		"</tt:Data>"					
    		"</tt:MessageDescription>"
    		"</CreateTrack>"
    	"</tns1:RecordingConfig>"

    	// tns1:RecordingConfig/DeleteTrack
        "<tns1:RecordingConfig wstop:topic=\"true\">"
    		"<DeleteTrack wstop:topic=\"true\">"
    		"<tt:MessageDescription IsProperty=\"false\">"
    		"<tt:Source>"
    		    "<tt:SimpleItemDescription Name=\"RecordingToken\" Type=\"tt:RecordingReference\"/>"
    		    "<tt:SimpleItemDescription Name=\"TrackToken\" Type=\"tt:TrackReference\"/>"
    		"</tt:Source>"
    		"<tt:Data>"
    		"</tt:Data>"					
    		"</tt:MessageDescription>"
    		"</DeleteTrack>"
    	"</tns1:RecordingConfig>"

    	// tns1:RecordingConfig/JobState
        "<tns1:RecordingConfig wstop:topic=\"true\">"
    		"<JobState wstop:topic=\"true\">"
    		"<tt:MessageDescription IsProperty=\"true\">"
    		"<tt:Source>"
    		    "<tt:SimpleItemDescription Name=\"RecordingJobToken\" Type=\"tt:RecordingJobReference\"/>"
    		"</tt:Source>"
    		"<tt:Data>"
    		    "<tt:SimpleItemDescription Name=\"State\" Type=\"xs:string\" />"
    		    "<tt:ElementItemDescription Name=\"Information\" Type=\"tt:RecordingJobStateInformation\"/>"
    		"</tt:Data>"					
    		"</tt:MessageDescription>"
    		"</JobState>"
    	"</tns1:RecordingConfig>"

    	// tns1:RecordingConfig/RecordingConfiguration
        "<tns1:RecordingConfig wstop:topic=\"true\">"
    		"<RecordingConfiguration wstop:topic=\"true\">"
    		"<tt:MessageDescription IsProperty=\"false\">"
    		"<tt:Source>"
    		    "<tt:SimpleItemDescription Name=\"RecordingToken\" Type=\"tt:RecordingReference\"/>"
    		"</tt:Source>"
    		"<tt:Data>"
    		    "<tt:ElementItemDescription Name=\"Configuration\" Type=\"tt:RecordingConfiguration\"/>"
    		"</tt:Data>"					
    		"</tt:MessageDescription>"
    		"</RecordingConfiguration>"
    	"</tns1:RecordingConfig>"

    	// tns1:RecordingConfig/TrackConfiguration
        "<tns1:RecordingConfig wstop:topic=\"true\">"
    		"<TrackConfiguration wstop:topic=\"true\">"
    		"<tt:MessageDescription IsProperty=\"false\">"
    		"<tt:Source>"
    		    "<tt:SimpleItemDescription Name=\"RecordingToken\" Type=\"tt:RecordingReference\"/>"
    		    "<tt:SimpleItemDescription Name=\"TrackToken\" Type=\"tt:TrackReference\" />"
    		"</tt:Source>"
    		"<tt:Data>"
    		    "<tt:ElementItemDescription Name=\"Configuration\" Type=\"tt:TrackConfiguration\"/>"
    		"</tt:Data>"					
    		"</tt:MessageDescription>"
    		"</TrackConfiguration>"
    	"</tns1:RecordingConfig>"

    	// tns1:RecordingConfig/RecordingJobConfiguration
        "<tns1:RecordingConfig wstop:topic=\"true\">"
    		"<RecordingJobConfiguration wstop:topic=\"true\">"
    		"<tt:MessageDescription IsProperty=\"false\">"
    		"<tt:Source>"
    		    "<tt:SimpleItemDescription Name=\"RecordingJobToken\" Type=\"tt:RecordingJobReference\"/>"
    		"</tt:Source>"
    		"<tt:Data>"
    		    "<tt:ElementItemDescription Name=\"Configuration\" Type=\"tt:RecordingJobConfiguration\"/>"
    		"</tt:Data>"					
    		"</tt:MessageDescription>"
    		"</RecordingJobConfiguration>"
    	"</tns1:RecordingConfig>"

    	// tns1:RecordingConfig/DeleteTrackData
    	"<tns1:RecordingConfig wstop:topic=\"true\">"
    		"<DeleteTrackData wstop:topic=\"true\">"
    		"<tt:MessageDescription IsProperty=\"false\">"
    		"<tt:Source>"
    		    "<tt:SimpleItemDescription Name=\"RecordingToken\" Type=\"tt:RecordingReference\"/>"
    		    "<tt:SimpleItemDescription Name=\"TrackToken\" Type=\"tt:TrackReference\"/>"
    		"</tt:Source>"
    		"<tt:Data>"
    		    "<tt:SimpleItemDescription Name=\"StartTime\" Type=\"xs:dateTime\"/>"
    		    "<tt:SimpleItemDescription Name=\"EndTime\" Type=\"xs:dateTime\"/>"
    		"</tt:Data>"					
    		"</tt:MessageDescription>"
    		"</DeleteTrackData>"
    	"</tns1:RecordingConfig>"
    );

    return offset;
}

#endif // end of PROFILE_G_SUPPORT

#ifdef PROFILE_C_SUPPORT

int build_ProfileC_EventProperties_xml(char * p_buf, int mlen)
{
    int offset = snprintf(p_buf, mlen, 
    
        // tns1:AccessControl/AccessGranted/Anonymous
        "<tns1:AccessControl wstop:topic=\"true\">"
			"<AccessGranted wstop:topic=\"true\">"
			"<Anonymous wstop:topic=\"true\">"
			"<tt:MessageDescription IsProperty=\"false\">"
			"<tt:Source>"
			    "<tt:SimpleItemDescription Name=\"AccessPointToken\" Type=\"pt:ReferenceToken\"/>"
			"</tt:Source>"
			"<tt:Data>"
			    "<tt:SimpleItemDescription Name=\"External\" Type=\"xs:boolean\"/>"
			"</tt:Data>"					
			"</tt:MessageDescription>"					
			"</Anonymous>"
			"</AccessGranted>"
		"</tns1:AccessControl>"

		// tns1:AccessControl/AccessGranted/Credential
        "<tns1:AccessControl wstop:topic=\"true\">"
			"<AccessGranted wstop:topic=\"true\">"
			"<Credential wstop:topic=\"true\">"
			"<tt:MessageDescription IsProperty=\"false\">"
			"<tt:Source>"
			    "<tt:SimpleItemDescription Name=\"AccessPointToken\" Type=\"pt:ReferenceToken\"/>"
			"</tt:Source>"
			"<tt:Data>"
			    "<tt:SimpleItemDescription Name=\"External\" Type=\"xs:boolean\"/>"
			    "<tt:SimpleItemDescription Name=\"CredentialToken\" Type=\"pt:ReferenceToken\"/>"
			    "<tt:SimpleItemDescription Name=\"CredentialHolderName\" Type=\"xs:string\"/>"
			"</tt:Data>"					
			"</tt:MessageDescription>"					
			"</Credential>"
			"</AccessGranted>"
		"</tns1:AccessControl>"

		// tns1:AccessControl/AccessTaken/Anonymous
        "<tns1:AccessControl wstop:topic=\"true\">"
			"<AccessTaken wstop:topic=\"true\">"
			"<Anonymous wstop:topic=\"true\">"
			"<tt:MessageDescription IsProperty=\"false\">"
			"<tt:Source>"
			    "<tt:SimpleItemDescription Name=\"AccessPointToken\" Type=\"pt:ReferenceToken\"/>"
			"</tt:Source>"					
			"</tt:MessageDescription>"					
			"</Anonymous>"
			"</AccessTaken>"
		"</tns1:AccessControl>"

		// tns1:AccessControl/AccessTaken/Credential
        "<tns1:AccessControl wstop:topic=\"true\">"
			"<AccessTaken wstop:topic=\"true\">"
			"<Credential wstop:topic=\"true\">"
			"<tt:MessageDescription IsProperty=\"false\">"
			"<tt:Source>"
			    "<tt:SimpleItemDescription Name=\"AccessPointToken\" Type=\"pt:ReferenceToken\"/>"
			"</tt:Source>"
			"<tt:Data>"
			    "<tt:SimpleItemDescription Name=\"CredentialToken\" Type=\"pt:ReferenceToken\"/>"
			    "<tt:SimpleItemDescription Name=\"CredentialHolderName\" Type=\"xs:string\"/>"
			"</tt:Data>"					
			"</tt:MessageDescription>"					
			"</Credential>"
			"</AccessTaken>"
		"</tns1:AccessControl>"

		// tns1:AccessControl/AccessNotTaken/Anonymous
        "<tns1:AccessControl wstop:topic=\"true\">"
			"<AccessNotTaken wstop:topic=\"true\">"
			"<Anonymous wstop:topic=\"true\">"
			"<tt:MessageDescription IsProperty=\"false\">"
			"<tt:Source>"
			    "<tt:SimpleItemDescription Name=\"AccessPointToken\" Type=\"pt:ReferenceToken\"/>"
			"</tt:Source>"					
			"</tt:MessageDescription>"					
			"</Anonymous>"
			"</AccessNotTaken>"
		"</tns1:AccessControl>"

		// tns1:AccessControl/AccessNotTaken/Credential
        "<tns1:AccessControl wstop:topic=\"true\">"
			"<AccessNotTaken wstop:topic=\"true\">"
			"<Credential wstop:topic=\"true\">"
			"<tt:MessageDescription IsProperty=\"false\">"
			"<tt:Source>"
			    "<tt:SimpleItemDescription Name=\"AccessPointToken\" Type=\"pt:ReferenceToken\"/>"
			"</tt:Source>"
			"<tt:Data>"
			    "<tt:SimpleItemDescription Name=\"CredentialToken\" Type=\"pt:ReferenceToken\"/>"
			    "<tt:SimpleItemDescription Name=\"CredentialHolderName\" Type=\"xs:string\"/>"
			"</tt:Data>"					
			"</tt:MessageDescription>"					
			"</Credential>"
			"</AccessNotTaken>"
		"</tns1:AccessControl>"

		// tns1:tns1:AccessControl/Denied/Anonymous
        "<tns1:AccessControl wstop:topic=\"true\">"
			"<Denied wstop:topic=\"true\">"
			"<Anonymous wstop:topic=\"true\">"
			"<tt:MessageDescription IsProperty=\"false\">"
			"<tt:Source>"
			    "<tt:SimpleItemDescription Name=\"AccessPointToken\" Type=\"pt:ReferenceToken\"/>"
			"</tt:Source>"
			"<tt:Data>"
			    "<tt:SimpleItemDescription Name=\"External\" Type=\"xs:boolean\"/>"
			    "<tt:SimpleItemDescription Name=\"Reason\" Type=\"xs:string\"/>"
			"</tt:Data>"					
			"</tt:MessageDescription>"					
			"</Anonymous>"
			"</Denied>"
		"</tns1:AccessControl>"

		// tns1:AccessControl/Denied/Credential
        "<tns1:AccessControl wstop:topic=\"true\">"
			"<Denied wstop:topic=\"true\">"
			"<Credential wstop:topic=\"true\">"
			"<tt:MessageDescription IsProperty=\"false\">"
			"<tt:Source>"
			    "<tt:SimpleItemDescription Name=\"AccessPointToken\" Type=\"pt:ReferenceToken\"/>"
			"</tt:Source>"
			"<tt:Data>"
			    "<tt:SimpleItemDescription Name=\"External\" Type=\"xs:boolean\"/>"
			    "<tt:SimpleItemDescription Name=\"CredentialToken\" Type=\"pt:ReferenceToken\"/>"
			    "<tt:SimpleItemDescription Name=\"CredentialHolderName\" Type=\"xs:string\"/>"
			    "<tt:SimpleItemDescription Name=\"Reason\" Type=\"xs:string\"/>"
			"</tt:Data>"					
			"</tt:MessageDescription>"					
			"</Credential>"
			"</Denied>"
		"</tns1:AccessControl>"

		// tns1:AccessControl/Denied/CredentialNotFound/Card
		"<tns1:AccessControl wstop:topic=\"true\">"
			"<Denied wstop:topic=\"true\">"
			"<CredentialNotFound wstop:topic=\"true\">"
			"<Card wstop:topic=\"true\">"
			"<tt:MessageDescription IsProperty=\"false\">"
			"<tt:Source>"
			    "<tt:SimpleItemDescription Name=\"AccessPointToken\" Type=\"pt:ReferenceToken\"/>"
			"</tt:Source>"
			"<tt:Data>"
			    "<tt:SimpleItemDescription Name=\"Card\" Type=\"xs:string\"/>"
			"</tt:Data>"					
			"</tt:MessageDescription>"
			"</Card>"
			"</CredentialNotFound>"
			"</Denied>"
		"</tns1:AccessControl>"

		// tns1:AccessControl/Duress
		"<tns1:AccessControl wstop:topic=\"true\">"
			"<Duress wstop:topic=\"true\">"
			"<tt:MessageDescription IsProperty=\"false\">"
			"<tt:Source>"
			    "<tt:SimpleItemDescription Name=\"AccessPointToken\" Type=\"pt:ReferenceToken\"/>"
			"</tt:Source>"
			"<tt:Data>"
			    "<tt:SimpleItemDescription Name=\"CredentialToken\" Type=\"pt:ReferenceToken\"/>"
			    "<tt:SimpleItemDescription Name=\"CredentialHolderName\" Type=\"xs:string\"/>"
			    "<tt:SimpleItemDescription Name=\"Reason\" Type=\"xs:string\"/>"
			"</tt:Data>"					
			"</tt:MessageDescription>"
			"</Duress>"
		"</tns1:AccessControl>"
		
        // tns1:AccessPoint/State/Enabled
        "<tns1:AccessPoint wstop:topic=\"true\">"
			"<State wstop:topic=\"true\">"
			"<Enabled wstop:topic=\"true\">"
			"<tt:MessageDescription IsProperty=\"true\">"
			"<tt:Source>"
			    "<tt:SimpleItemDescription Name=\"AccessPointToken\" Type=\"pt:ReferenceToken\"/>"
			"</tt:Source>"
			"<tt:Data>"
			    "<tt:SimpleItemDescription Name=\"State\" Type=\"xs:boolean\"/>"
			"</tt:Data>"					
			"</tt:MessageDescription>"					
			"</Enabled>"
			"</State>"
		"</tns1:AccessPoint>"

        // tns1:Configuration/AccessPoint/Changed
        "<tns1:Configuration wstop:topic=\"true\">"
			"<AccessPoint wstop:topic=\"true\">"
			"<Changed wstop:topic=\"true\">"
			"<tt:MessageDescription IsProperty=\"false\">"
			"<tt:Source>"
			    "<tt:SimpleItemDescription Name=\"AccessPointToken\" Type=\"pt:ReferenceToken\"/>"
			"</tt:Source>"					
			"</tt:MessageDescription>"					
			"</Changed>"
			"</AccessPoint>"
		"</tns1:Configuration>"

		// tns1:Configuration/AccessPoint/Removed
        "<tns1:Configuration wstop:topic=\"true\">"
			"<AccessPoint wstop:topic=\"true\">"
			"<Removed wstop:topic=\"true\">"
			"<tt:MessageDescription IsProperty=\"false\">"
			"<tt:Source>"
			    "<tt:SimpleItemDescription Name=\"AccessPointToken\" Type=\"pt:ReferenceToken\"/>"
			"</tt:Source>"					
			"</tt:MessageDescription>"					
			"</Removed>"
			"</AccessPoint>"
		"</tns1:Configuration>"

		// Topic: tns1:Configuration/Area/Changed
		"<tns1:Configuration wstop:topic=\"true\">"
			"<Area wstop:topic=\"true\">"
			"<Changed wstop:topic=\"true\">"
			"<tt:MessageDescription IsProperty=\"false\">"
			"<tt:Source>"
			    "<tt:SimpleItemDescription Name=\"AreaToken\" Type=\"pt:ReferenceToken\"/>"
			"</tt:Source>"					
			"</tt:MessageDescription>"					
			"</Changed>"
			"</Area>"
		"</tns1:Configuration>"

		// tns1:Configuration/Area/Removed
		"<tns1:Configuration wstop:topic=\"true\">"
			"<Area wstop:topic=\"true\">"
			"<Removed wstop:topic=\"true\">"
			"<tt:MessageDescription IsProperty=\"false\">"
			"<tt:Source>"
			    "<tt:SimpleItemDescription Name=\"AreaToken\" Type=\"pt:ReferenceToken\"/>"
			"</tt:Source>"					
			"</tt:MessageDescription>"					
			"</Removed>"
			"</Area>"
		"</tns1:Configuration>"

		// tns1:Door/State/DoorMode
        "<tns1:Door wstop:topic=\"true\">"
			"<State wstop:topic=\"true\">"
			"<DoorMode wstop:topic=\"true\">"
			"<tt:MessageDescription IsProperty=\"true\">"
			"<tt:Source>"
			    "<tt:SimpleItemDescription Name=\"DoorToken\" Type=\"pt:ReferenceToken\"/>"
			"</tt:Source>"	
			"<tt:Data>"
			    "<tt:SimpleItemDescription Name=\"State\" Type=\"tdc:DoorMode\"/>"
			"</tt:Data>"
			"</tt:MessageDescription>"					
			"</DoorMode>"
			"</State>"
		"</tns1:Door>"

		// tns1:Door/State/DoorPhysicalState
		"<tns1:Door wstop:topic=\"true\">"
			"<State wstop:topic=\"true\">"
			"<DoorPhysicalState wstop:topic=\"true\">"
			"<tt:MessageDescription IsProperty=\"true\">"
			"<tt:Source>"
			    "<tt:SimpleItemDescription Name=\"DoorToken\" Type=\"pt:ReferenceToken\"/>"
			"</tt:Source>"	
			"<tt:Data>"
			    "<tt:SimpleItemDescription Name=\"State\" Type=\"tdc:DoorPhysicalState\"/>"
			"</tt:Data>"
			"</tt:MessageDescription>"					
			"</DoorPhysicalState>"
			"</State>"
		"</tns1:Door>"

		// tns1:Door/State/LockPhysicalState
		"<tns1:Door wstop:topic=\"true\">"
			"<State wstop:topic=\"true\">"
			"<LockPhysicalState wstop:topic=\"true\">"
			"<tt:MessageDescription IsProperty=\"true\">"
			"<tt:Source>"
			    "<tt:SimpleItemDescription Name=\"DoorToken\" Type=\"pt:ReferenceToken\"/>"
			"</tt:Source>"	
			"<tt:Data>"
			    "<tt:SimpleItemDescription Name=\"State\" Type=\"tdc:LockPhysicalState\"/>"
			"</tt:Data>"
			"</tt:MessageDescription>"					
			"</LockPhysicalState>"
			"</State>"
		"</tns1:Door>"

		// tns1:Door/State/DoubleLockPhysicalState
		"<tns1:Door wstop:topic=\"true\">"
			"<State wstop:topic=\"true\">"
			"<DoubleLockPhysicalState wstop:topic=\"true\">"
			"<tt:MessageDescription IsProperty=\"true\">"
			"<tt:Source>"
			    "<tt:SimpleItemDescription Name=\"DoorToken\" Type=\"pt:ReferenceToken\"/>"
			"</tt:Source>"	
			"<tt:Data>"
			    "<tt:SimpleItemDescription Name=\"State\" Type=\"tdc:LockPhysicalState\"/>"
			"</tt:Data>"
			"</tt:MessageDescription>"					
			"</DoubleLockPhysicalState>"
			"</State>"
		"</tns1:Door>"

        // tns1:Door/State/DoorAlarm
        "<tns1:Door wstop:topic=\"true\">"
			"<State wstop:topic=\"true\">"
			"<DoorAlarm wstop:topic=\"true\">"
			"<tt:MessageDescription IsProperty=\"true\">"
			"<tt:Source>"
			    "<tt:SimpleItemDescription Name=\"DoorToken\" Type=\"pt:ReferenceToken\"/>"
			"</tt:Source>"	
			"<tt:Data>"
			    "<tt:SimpleItemDescription Name=\"State\" Type=\"tdc:DoorAlarmState\"/>"
			"</tt:Data>"
			"</tt:MessageDescription>"					
			"</DoorAlarm>"
			"</State>"
		"</tns1:Door>"

        // tns1:Door/State/DoorTamper
        "<tns1:Door wstop:topic=\"true\">"
			"<State wstop:topic=\"true\">"
			"<DoorTamper wstop:topic=\"true\">"
			"<tt:MessageDescription IsProperty=\"true\">"
			"<tt:Source>"
			    "<tt:SimpleItemDescription Name=\"DoorToken\" Type=\"pt:ReferenceToken\"/>"
			"</tt:Source>"	
			"<tt:Data>"
			    "<tt:SimpleItemDescription Name=\"State\" Type=\"tdc:DoorTamperState\"/>"
			"</tt:Data>"
			"</tt:MessageDescription>"					
			"</DoorTamper>"
			"</State>"
		"</tns1:Door>"

		// tns1:Door/State/DoorFault
		"<tns1:Door wstop:topic=\"true\">"
			"<State wstop:topic=\"true\">"
			"<DoorFault wstop:topic=\"true\">"
			"<tt:MessageDescription IsProperty=\"true\">"
			"<tt:Source>"
			    "<tt:SimpleItemDescription Name=\"DoorToken\" Type=\"pt:ReferenceToken\"/>"
			"</tt:Source>"	
			"<tt:Data>"
			    "<tt:SimpleItemDescription Name=\"State\" Type=\"tdc:DoorFaultState\"/>"
			    "<tt:SimpleItemDescription Name=\"Reason\" Type=\"xs:string\"/>"
			"</tt:Data>"
			"</tt:MessageDescription>"					
			"</DoorFault>"
			"</State>"
		"</tns1:Door>"
		
		// tns1:Configuration/Door/Changed
		"<tns1:Configuration wstop:topic=\"true\">"
			"<Door wstop:topic=\"true\">"
			"<Changed wstop:topic=\"true\">"
			"<tt:MessageDescription IsProperty=\"false\">"
			"<tt:Source>"
			    "<tt:SimpleItemDescription Name=\"DoorToken\" Type=\"pt:ReferenceToken\"/>"
			"</tt:Source>"
			"</tt:MessageDescription>"					
			"</Changed>"
			"</Door>"
		"</tns1:Configuration>"
		
		// tns1:Configuration/Door/Removed
		"<tns1:Configuration wstop:topic=\"true\">"
			"<Door wstop:topic=\"true\">"
			"<Removed wstop:topic=\"true\">"
			"<tt:MessageDescription IsProperty=\"false\">"
			"<tt:Source>"
			    "<tt:SimpleItemDescription Name=\"DoorToken\" Type=\"pt:ReferenceToken\"/>"
			"</tt:Source>"
			"</tt:MessageDescription>"					
			"</Removed>"
			"</Door>"
		"</tns1:Configuration>"
	);

    return offset;				
}

#endif // end of PROFILE_C_SUPPORT

#ifdef CREDENTIAL_SUPPORT

int build_Credential_EventProperties_xml(char * p_buf, int mlen)
{
    int offset = snprintf(p_buf, mlen, 
    
        // tns1:Credential/State/Enabled
        "<tns1:Credential wstop:topic=\"true\">"
			"<State wstop:topic=\"true\">"
			"<Enabled wstop:topic=\"true\">"
			"<tt:MessageDescription IsProperty=\"false\">"
			"<tt:Source>"
			    "<tt:SimpleItemDescription Name=\"CredentialToken\" Type=\"pt:ReferenceToken\" />"
			"</tt:Source>"
			"<tt:Data>"
			    "<tt:SimpleItemDescription Name=\"State\" Type=\"xs:boolean\" />"
			    "<tt:SimpleItemDescription Name=\"Reason\" Type=\"xs:string\" />"
			    "<tt:SimpleItemDescription Name=\"ClientUpdated\" Type=\"xs:boolean\" />"
			"</tt:Data>"					
			"</tt:MessageDescription>"					
			"</Enabled>"
			"</State>"
		"</tns1:Credential>"

		// tns1:Credential/State/ApbViolation
        "<tns1:Credential wstop:topic=\"true\">"
			"<State wstop:topic=\"true\">"
			"<ApbViolation wstop:topic=\"true\">"
			"<tt:MessageDescription IsProperty=\"false\">"
			"<tt:Source>"
			    "<tt:SimpleItemDescription Name=\"CredentialToken\" Type=\"pt:ReferenceToken\" />"
			"</tt:Source>"
			"<tt:Data>"
			    "<tt:SimpleItemDescription Name=\"ApbViolation\" Type=\"xs:boolean\" />"
			    "<tt:SimpleItemDescription Name=\"ClientUpdated\" Type=\"xs:boolean\" />"
			"</tt:Data>"					
			"</tt:MessageDescription>"					
			"</ApbViolation>"
			"</State>"
		"</tns1:Credential>"

		// tns1:Configuration/Credential/Changed
        "<tns1:Configuration wstop:topic=\"true\">"
			"<Credential wstop:topic=\"true\">"
			"<Changed wstop:topic=\"true\">"
			"<tt:MessageDescription IsProperty=\"false\">"
			"<tt:Source>"
			    "<tt:SimpleItemDescription Name=\"CredentialToken\" Type=\"pt:ReferenceToken\" />"
			"</tt:Source>"					
			"</tt:MessageDescription>"					
			"</Changed>"
			"</Credential>"
		"</tns1:Configuration>"

		// tns1:Configuration/Credential/Removed
        "<tns1:Configuration wstop:topic=\"true\">"
			"<Credential wstop:topic=\"true\">"
			"<Removed wstop:topic=\"true\">"
			"<tt:MessageDescription IsProperty=\"false\">"
			"<tt:Source>"
			    "<tt:SimpleItemDescription Name=\"CredentialToken\" Type=\"pt:ReferenceToken\" />"
			"</tt:Source>"					
			"</tt:MessageDescription>"					
			"</Removed>"
			"</Credential>"
		"</tns1:Configuration>"
	);

	return offset;
}

#endif // end of CREDENTIAL_SUPPORT

#ifdef ACCESS_RULES

int build_AccessRules_EventProperties_xml(char * p_buf, int mlen)
{
    int offset = snprintf(p_buf, mlen, 

		// tns1:Configuration/AccessProfile/Changed
        "<tns1:Configuration wstop:topic=\"true\">"
			"<AccessProfile wstop:topic=\"true\">"
			"<Changed wstop:topic=\"true\">"
			"<tt:MessageDescription IsProperty=\"false\">"
			"<tt:Source>"
			    "<tt:SimpleItemDescription Name=\"AccessProfileToken\" Type=\"pt:ReferenceToken\" />"
			"</tt:Source>"					
			"</tt:MessageDescription>"					
			"</Changed>"
			"</AccessProfile>"
		"</tns1:Configuration>"

		// tns1:Configuration/AccessProfile/Removed
        "<tns1:Configuration wstop:topic=\"true\">"
			"<AccessProfile wstop:topic=\"true\">"
			"<Removed wstop:topic=\"true\">"
			"<tt:MessageDescription IsProperty=\"false\">"
			"<tt:Source>"
			    "<tt:SimpleItemDescription Name=\"AccessProfileToken\" Type=\"pt:ReferenceToken\" />"
			"</tt:Source>"					
			"</tt:MessageDescription>"					
			"</Removed>"
			"</AccessProfile>"
		"</tns1:Configuration>"
	);

	return offset;
}

#endif // end of ACCESS_RULES

#ifdef SCHEDULE_SUPPORT

int build_Schedule_EventProperties_xml(char * p_buf, int mlen)
{
    int offset = snprintf(p_buf, mlen, 

		// tns1:Schedule/State/Active
        "<tns1:Schedule wstop:topic=\"true\">"
			"<State wstop:topic=\"true\">"
			"<Active wstop:topic=\"true\">"
			"<tt:MessageDescription IsProperty=\"true\">"
			"<tt:Source>"
			    "<tt:SimpleItemDescription Name=\"ScheduleToken\" Type=\"pt:ReferenceToken\" />"
			    "<tt:SimpleItemDescription Name=\"Name\" Type=\"xs:string\" />"
			"</tt:Source>"
			"<tt:Data>"
			    "<tt:SimpleItemDescription Name=\"Active\" Type=\"xs:boolean\" />"
			    "<tt:SimpleItemDescription Name=\"SpecialDay\" Type=\"xs:boolean\" />"
			"</tt:Data>"
			"</tt:MessageDescription>"					
			"</Active>"
			"</State>"
		"</tns1:Schedule>"

		// tns1:Configuration/Schedule/Changed
        "<tns1:Configuration wstop:topic=\"true\">"
			"<Schedule wstop:topic=\"true\">"
			"<Changed wstop:topic=\"true\">"
			"<tt:MessageDescription IsProperty=\"false\">"
			"<tt:Source>"
			    "<tt:SimpleItemDescription Name=\"ScheduleToken\" Type=\"pt:ReferenceToken\" />"
			"</tt:Source>"					
			"</tt:MessageDescription>"					
			"</Changed>"
			"</Schedule>"
		"</tns1:Configuration>"

		// tns1:Configuration/Schedule/Removed
        "<tns1:Configuration wstop:topic=\"true\">"
			"<Schedule wstop:topic=\"true\">"
			"<Removed wstop:topic=\"true\">"
			"<tt:MessageDescription IsProperty=\"false\">"
			"<tt:Source>"
			    "<tt:SimpleItemDescription Name=\"ScheduleToken\" Type=\"pt:ReferenceToken\" />"
			"</tt:Source>"					
			"</tt:MessageDescription>"					
			"</Removed>"
			"</Schedule>"
		"</tns1:Configuration>"

		// tns1:Configuration/SpecialDays/Changed
        "<tns1:Configuration wstop:topic=\"true\">"
			"<SpecialDays wstop:topic=\"true\">"
			"<Changed wstop:topic=\"true\">"
			"<tt:MessageDescription IsProperty=\"false\">"
			"<tt:Source>"
			    "<tt:SimpleItemDescription Name=\"SpecialDaysToken\" Type=\"pt:ReferenceToken\" />"
			"</tt:Source>"					
			"</tt:MessageDescription>"					
			"</Changed>"
			"</SpecialDays>"
		"</tns1:Configuration>"

		// tns1:Configuration/SpecialDays/Removed
        "<tns1:Configuration wstop:topic=\"true\">"
			"<SpecialDays wstop:topic=\"true\">"
			"<Removed wstop:topic=\"true\">"
			"<tt:MessageDescription IsProperty=\"false\">"
			"<tt:Source>"
			    "<tt:SimpleItemDescription Name=\"SpecialDaysToken\" Type=\"pt:ReferenceToken\" />"
			"</tt:Source>"					
			"</tt:MessageDescription>"					
			"</Removed>"
			"</SpecialDays>"
		"</tns1:Configuration>"
	);

	return offset;
}

#endif // end of SCHEDULE_SUPPORT

#ifdef RECEIVER_SUPPORT

int build_Receiver_EventProperties_xml(char * p_buf, int mlen)
{
    int offset = snprintf(p_buf, mlen, 

		// tns1:Receiver/ChangeState
        "<tns1:Receiver wstop:topic=\"true\">"
			"<ChangeState wstop:topic=\"true\">"
			"<tt:MessageDescription IsProperty=\"false\">"
			"<tt:Source>"
			    "<tt:SimpleItemDescription Name=\"ReceiverToken\" Type=\"tt:ReferenceToken\" />"
			"</tt:Source>"
			"<tt:Data>"
			    "<tt:SimpleItemDescription Name=\"NewState\" Type=\"tt:ReceiverState\" />"
			    "<tt:SimpleItemDescription Name=\"MediaUri\" Type=\"tt:MediaUri\" />"
			"</tt:Data>"
			"</tt:MessageDescription>"
			"</ChangeState>"
		"</tns1:Receiver>"

		// tns1:Receiver/ConnectionFailed
        "<tns1:Receiver wstop:topic=\"true\">"
			"<ChangeState wstop:topic=\"true\">"
			"<tt:MessageDescription IsProperty=\"false\">"
			"<tt:Source>"
			    "<tt:SimpleItemDescription Name=\"ReceiverToken\" Type=\"tt:ReferenceToken\" />"
			"</tt:Source>"
			"<tt:Data>"
			    "<tt:SimpleItemDescription Name=\"MediaUri\" Type=\"tt:MediaUri\" />"
			"</tt:Data>"
			"</tt:MessageDescription>"					
			"</ChangeState>"
		"</tns1:Receiver>");

	return offset;
}

#endif // end of RECEIVER_SUPPORT

int build_GetEventProperties_rly_xml(char * p_buf, int mlen, const char * argv)
{
	int offset = 0;

	offset += snprintf(p_buf+offset, mlen-offset, 
		"<tev:GetEventPropertiesResponse>"
			"<tev:TopicNamespaceLocation>"
				"http://www.onvif.org/onvif/ver10/topics/topicns.xml"
			"</tev:TopicNamespaceLocation>"
			"<wsnt:FixedTopicSet>true</wsnt:FixedTopicSet>"
			"<wstop:TopicSet xmlns=\"\">");

#ifdef PROFILE_Q_SUPPORT
    offset += build_Base_EventProperties_xml(p_buf+offset, mlen-offset);
#endif

    offset += build_Imaging_EventProperties_xml(p_buf+offset, mlen-offset);

#ifdef VIDEO_ANALYTICS
    offset += build_Analytics_EventProperties_xml(p_buf+offset, mlen-offset);            
#endif

#ifdef DEVICEIO_SUPPORT
    offset += build_DeviceIO_EventProperties_xml(p_buf+offset, mlen-offset);
#endif

#ifdef PROFILE_G_SUPPORT
    offset += build_ProfileG_EventProperties_xml(p_buf+offset, mlen-offset);                
#endif

#ifdef PROFILE_C_SUPPORT
    offset += build_ProfileC_EventProperties_xml(p_buf+offset, mlen-offset);                
#endif

#ifdef CREDENTIAL_SUPPORT
    offset += build_Credential_EventProperties_xml(p_buf+offset, mlen-offset);  
#endif

#ifdef ACCESS_RULES
    offset += build_AccessRules_EventProperties_xml(p_buf+offset, mlen-offset);
#endif

#ifdef SCHEDULE_SUPPORT
    offset += build_Schedule_EventProperties_xml(p_buf+offset, mlen-offset);
#endif

#ifdef RECEIVER_SUPPORT
    offset += build_Receiver_EventProperties_xml(p_buf+offset, mlen-offset);
#endif

    offset += snprintf(p_buf+offset, mlen-offset, 				
			"</wstop:TopicSet>"	

			"<wsnt:TopicExpressionDialect>"
				"http://www.onvif.org/ver10/tev/topicExpression/ConcreteSet"										
			"</wsnt:TopicExpressionDialect>"
			"<wsnt:TopicExpressionDialect>"
				"http://docs.oasis-open.org/wsnt/t-1/TopicExpression/ConcreteSet"
			"</wsnt:TopicExpressionDialect>"
			"<wsnt:TopicExpressionDialect>"
				"http://docs.oasis-open.org/wsn/t-1/TopicExpression/Concrete"
			"</wsnt:TopicExpressionDialect>"	
			"<tev:MessageContentFilterDialect>"
				"http://www.onvif.org/ver10/tev/messageContentFilter/ItemFilter"
			"</tev:MessageContentFilterDialect>"
			"<tev:MessageContentSchemaLocation>"
				"http://www.onvif.org/onvif/ver10/schema/onvif.xsd"
			"</tev:MessageContentSchemaLocation>"
		"</tev:GetEventPropertiesResponse>");

	return offset;
}

int build_Subscribe_rly_xml(char * p_buf, int mlen, const char * argv)
{
	int offset = 0;
	EUA * p_eua = (EUA *) argv;
	char cur_time[100], term_time[100];
	
	if (NULL == p_eua)
	{
		return -1;
	}

	onvif_get_time_str(cur_time, sizeof(cur_time), 0);	
	
	if (g_onvif_cfg.evt_renew_time < p_eua->init_term_time)
	{
		onvif_get_time_str(term_time, sizeof(term_time), g_onvif_cfg.evt_renew_time);
	}
	else
	{
		onvif_get_time_str(term_time, sizeof(term_time), p_eua->init_term_time);
	}
	
    offset += snprintf(p_buf+offset, mlen-offset, 
    	"<wsnt:SubscribeResponse>"
	        "<wsnt:SubscriptionReference>"
	            "<wsa:Address>%s</wsa:Address>"
	        "</wsnt:SubscriptionReference>"
	        "<wsnt:CurrentTime>%s</wsnt:CurrentTime>"
	        "<wsnt:TerminationTime>%s</wsnt:TerminationTime>"
	    "</wsnt:SubscribeResponse>",
	    p_eua->producter_addr, cur_time, term_time);

	return offset;
}


int build_Unsubscribe_rly_xml(char * p_buf, int mlen, const char * argv)
{
	int offset = 0;
	offset += snprintf(p_buf+offset, mlen-offset, "<wsnt:UnsubscribeResponse></wsnt:UnsubscribeResponse>");
	return offset;
}

int build_Renew_rly_xml(char * p_buf, int mlen, const char * argv)
{
    int offset = 0;
	char cur_time[100], term_time[100];

	onvif_get_time_str(cur_time, sizeof(cur_time), 0);
	onvif_get_time_str(term_time, sizeof(term_time), 60);
		
	offset += snprintf(p_buf+offset, mlen-offset, 
		"<wsnt:RenewResponse>"			
	        "<wsnt:TerminationTime>%s</wsnt:TerminationTime>"
	        "<wsnt:CurrentTime>%s</wsnt:CurrentTime>"
		"</wsnt:RenewResponse>", term_time, cur_time);

	return offset;
}

int build_CreatePullPointSubscription_rly_xml(char * p_buf, int mlen, const char * argv)
{
    int offset = 0;
	char cur_time[100], term_time[100];
	EUA * p_eua = (EUA *) argv;

	onvif_get_time_str(cur_time, sizeof(cur_time), 0);

    if (g_onvif_cfg.evt_renew_time < p_eua->init_term_time)
	{
		onvif_get_time_str(term_time, sizeof(term_time), g_onvif_cfg.evt_renew_time);
	}
	else
	{
		onvif_get_time_str(term_time, sizeof(term_time), p_eua->init_term_time);
	}
	
	offset += snprintf(p_buf+offset, mlen-offset, 
		"<tev:CreatePullPointSubscriptionResponse>"	
			"<tev:SubscriptionReference>"
	            "<wsa:Address>%s</wsa:Address>"
	        "</tev:SubscriptionReference>"	        
	        "<wsnt:CurrentTime>%s</wsnt:CurrentTime>"
	        "<wsnt:TerminationTime>%s</wsnt:TerminationTime>"
		"</tev:CreatePullPointSubscriptionResponse>", 
		p_eua->producter_addr,
		cur_time, term_time);

	return offset;
}

int build_SimpleItem_xml(char * p_buf, int mlen, onvif_SimpleItem * p_req)
{
	int offset = 0;
	
	offset += snprintf(p_buf+offset, mlen-offset, 
		"<tt:SimpleItem Name=\"%s\" Value=\"%s\" />",
		p_req->Name, p_req->Value);

	return offset;
}

int build_ElementItem_xml(char * p_buf, int mlen, onvif_ElementItem * p_req)
{
	int offset = 0;

	if (p_req->AnyFlag)
	{
	    offset += snprintf(p_buf+offset, mlen-offset, "<tt:ElementItem Name=\"%s\">%s</tt:ElementItem>", p_req->Name, p_req->Any);
	}
	else
	{
	    offset += snprintf(p_buf+offset, mlen-offset, "<tt:ElementItem Name=\"%s\" />", p_req->Name);
	}

	return offset;
}


int build_NotificationMessage_xml(char * p_buf, int mlen, onvif_NotificationMessage * p_req)
{
	int offset = 0;
	char utctime[64];
	ONVIF_SimpleItem * p_simpleitem;
	ONVIF_ElementItem * p_elementitem;
	struct tm *gtime;
	
	gtime = gmtime(&p_req->Message.UtcTime);	
	snprintf(utctime, sizeof(utctime), "%04d-%02d-%02dT%02d:%02d:%02dZ", 		 
		gtime->tm_year+1900, gtime->tm_mon+1, gtime->tm_mday, gtime->tm_hour, gtime->tm_min, gtime->tm_sec);
		
	offset += snprintf(p_buf+offset, mlen-offset, 		
		"<wsnt:Topic Dialect=\"%s\">%s</wsnt:Topic>",
		p_req->Dialect,
		p_req->Topic);

	offset += snprintf(p_buf+offset, mlen-offset, "<wsnt:Message>");
	offset += snprintf(p_buf+offset, mlen-offset, "<tt:Message UtcTime=\"%s\"", utctime);
	if (p_req->Message.PropertyOperationFlag)
	{
		offset += snprintf(p_buf+offset, mlen-offset, " PropertyOperation=\"%s\"", 
			onvif_PropertyOperationToString(p_req->Message.PropertyOperation));
	}
	offset += snprintf(p_buf+offset, mlen-offset, ">");

	if (p_req->Message.SourceFlag)
	{
		offset += snprintf(p_buf+offset, mlen-offset, "<tt:Source>");
		p_simpleitem = p_req->Message.Source.SimpleItem;
		while (p_simpleitem)
		{
			offset += build_SimpleItem_xml(p_buf+offset, mlen-offset, &p_simpleitem->SimpleItem);
			p_simpleitem = p_simpleitem->next;
		}
		
		p_elementitem = p_req->Message.Source.ElementItem;
		while (p_elementitem)
		{
			offset += build_ElementItem_xml(p_buf+offset, mlen-offset, &p_elementitem->ElementItem);
			p_elementitem = p_elementitem->next;
		}
		offset += snprintf(p_buf+offset, mlen-offset, "</tt:Source>");
	}

	if (p_req->Message.KeyFlag)
	{
		offset += snprintf(p_buf+offset, mlen-offset, "<tt:Key>");
		p_simpleitem = p_req->Message.Key.SimpleItem;
		while (p_simpleitem)
		{
			offset += build_SimpleItem_xml(p_buf+offset, mlen-offset, &p_simpleitem->SimpleItem);
			p_simpleitem = p_simpleitem->next;
		}

		p_elementitem = p_req->Message.Key.ElementItem;
		while (p_elementitem)
		{
			offset += build_ElementItem_xml(p_buf+offset, mlen-offset, &p_elementitem->ElementItem);
			p_elementitem = p_elementitem->next;
		}
		offset += snprintf(p_buf+offset, mlen-offset, "</tt:Key>");
	}

	if (p_req->Message.DataFlag)
	{
		offset += snprintf(p_buf+offset, mlen-offset, "<tt:Data>");
		p_simpleitem = p_req->Message.Data.SimpleItem;
		while (p_simpleitem)
		{
			offset += build_SimpleItem_xml(p_buf+offset, mlen-offset, &p_simpleitem->SimpleItem);
			p_simpleitem = p_simpleitem->next;
		}

		p_elementitem = p_req->Message.Data.ElementItem;
		while (p_elementitem)
		{
			offset += build_ElementItem_xml(p_buf+offset, mlen-offset, &p_elementitem->ElementItem);
			p_elementitem = p_elementitem->next;
		}
		offset += snprintf(p_buf+offset, mlen-offset, "</tt:Data>");
	}
	
	offset += snprintf(p_buf+offset, mlen-offset, "</tt:Message>");		
	offset += snprintf(p_buf+offset, mlen-offset, "</wsnt:Message>");

	return offset;
}

int build_PullMessages_rly_xml(char * p_buf, int mlen, const char * argv)
{
	int offset = 0, msg_nums = 0;
	char cur_time[100], term_time[100];
	LINKED_NODE *p_node, *p_next;
	ONVIF_NotificationMessage * p_tmp;
	ONVIF_NotificationMessage * p_message;
	
	PullMessages_REQ * p_req = (PullMessages_REQ *)argv;
	EUA * p_eua = onvif_get_eua_by_index(p_req->eua_idx);

	assert(p_eua);

	onvif_get_time_str(cur_time, sizeof(cur_time), 0);
	onvif_get_time_str(term_time, sizeof(term_time), 60);

	offset += snprintf(p_buf+offset, mlen-offset, "<tev:PullMessagesResponse>");
	offset += snprintf(p_buf+offset, mlen-offset, 
        "<tev:CurrentTime>%s</tev:CurrentTime>"
        "<tev:TerminationTime>%s</tev:TerminationTime>",
        cur_time, term_time);

	// get notify message from message list
	
	p_node = h_list_lookback_start(p_eua->msg_list);
	while (p_node && msg_nums < p_req->MessageLimit)
	{
		p_message = (ONVIF_NotificationMessage *) p_node->p_data;	
		
		p_tmp = p_message;
		while (p_tmp)
		{	
		    if (onvif_event_filter(p_tmp, p_eua))
		    {
				offset += snprintf(p_buf+offset, mlen-offset, "<wsnt:NotificationMessage>");
				offset += build_NotificationMessage_xml(p_buf+offset, mlen-offset, &p_tmp->NotificationMessage);
				offset += snprintf(p_buf+offset, mlen-offset, "</wsnt:NotificationMessage>");

				msg_nums++;
			}

			p_tmp = p_tmp->next;
		}

		p_next = h_list_lookback_next(p_eua->msg_list, p_node);

		h_list_remove(p_eua->msg_list, p_node);
		onvif_free_NotificationMessage(p_message);

		p_node = p_next;
	}
	h_list_lookback_end(p_eua->msg_list);
        
	offset += snprintf(p_buf+offset, mlen-offset, "</tev:PullMessagesResponse>");

	return offset;
}

int build_tev_SetSynchronizationPoint_rly_xml(char * p_buf, int mlen, const char * argv)
{
	int offset = 0;		
	offset += snprintf(p_buf+offset, mlen-offset, "<tev:SetSynchronizationPointResponse />");
	return offset;
}

int build_Notify_xml(char * p_buf, int mlen, const char * argv)
{
	ONVIF_NotificationMessage * p_message = (ONVIF_NotificationMessage *)argv;
	ONVIF_NotificationMessage * p_tmp = p_message;
	
	int offset = snprintf(p_buf, mlen, xml_hdr);
	
	offset += snprintf(p_buf+offset, mlen-offset, onvif_xmlns);
	offset += snprintf(p_buf+offset, mlen-offset, soap_head, "http://docs.oasis-open.org/wsn/bw-2/NotificationConsumer/Notify");
	offset += snprintf(p_buf+offset, mlen-offset, soap_body);
		
	offset += snprintf(p_buf+offset, mlen-offset, "<wsnt:Notify>");
	while (p_tmp)
	{
    	offset += snprintf(p_buf+offset, mlen-offset, "<wsnt:NotificationMessage>");   
    	offset += build_NotificationMessage_xml(p_buf+offset, mlen-offset, &p_tmp->NotificationMessage);
		offset += snprintf(p_buf+offset, mlen-offset, "</wsnt:NotificationMessage>");
		
		p_tmp = p_tmp->next;
	}			
	offset += snprintf(p_buf+offset, mlen-offset, "</wsnt:Notify>");
	
	offset += snprintf(p_buf+offset, mlen-offset, soap_tailer);

	return offset;
}

int build_GetWsdlUrl_rly_xml(char * p_buf, int mlen, const char * argv)
{
	int offset = 0;
    
	offset += snprintf(p_buf+offset, mlen-offset, 
		"<tds:GetWsdlUrlResponse>"
			"<tds:WsdlUrl>http://www.onvif.org/</tds:WsdlUrl>"
		"</tds:GetWsdlUrlResponse>");
		
	return offset;
}

int build_GetEndpointReference_rly_xml(char * p_buf, int mlen, const char * argv)
{
    int offset = 0;
    
	offset += snprintf(p_buf+offset, mlen-offset, 
		"<tds:GetEndpointReferenceResponse>"
			"<tds:GUID>%s</tds:GUID>"
		"</tds:GetEndpointReferenceResponse>",
		g_onvif_cfg.EndpointReference);
	
	return offset;
}

int build_GetGuaranteedNumberOfVideoEncoderInstances_rly_xml(char * p_buf, int mlen, const char * argv)
{
	int offset = 0;

	int jpeg = 1;
	int h264 = 4;
	int mpeg4= 2;

	if (0 == g_onvif_cfg.VideoEncoderConfigurationOptions.JPEGFlag)
	{
	    jpeg = 0;
	}

	if (0 == g_onvif_cfg.VideoEncoderConfigurationOptions.MPEG4Flag)
	{
	    mpeg4 = 0;
	}

	if (0 == g_onvif_cfg.VideoEncoderConfigurationOptions.H264Flag)
	{
	    h264 = 0;
	}
	
	offset += snprintf(p_buf+offset, mlen-offset, 
	    "<trt:GetGuaranteedNumberOfVideoEncoderInstancesResponse>"
		    "<trt:TotalNumber>%d</trt:TotalNumber>"
		    "<trt:JPEG>%d</trt:JPEG>"
		    "<trt:H264>%d</trt:H264>"
		    "<trt:MPEG4>%d</trt:MPEG4>"
		"</trt:GetGuaranteedNumberOfVideoEncoderInstancesResponse>", 
		2, jpeg, h264, mpeg4);	

	return offset;
}

int build_GetImagingSettings_rly_xml(char * p_buf, int mlen, const char * argv)
{
	int offset = 0;
	ONVIF_VideoSource * p_v_src = onvif_find_VideoSource(argv);
	if (NULL == p_v_src)
	{
		return ONVIF_ERR_NoSource;
	}
	
	offset += snprintf(p_buf+offset, mlen-offset, "<timg:GetImagingSettingsResponse><timg:ImagingSettings>");
	offset += build_ImageSettings_xml(p_buf+offset, mlen-offset);	
	offset += snprintf(p_buf+offset, mlen-offset, "</timg:ImagingSettings></timg:GetImagingSettingsResponse>");

	return offset;
}

int build_GetOptions_rly_xml(char * p_buf, int mlen, const char * argv)
{
	int offset = 0;
	ONVIF_VideoSource * p_v_src = onvif_find_VideoSource(argv);
	if (NULL == p_v_src)
	{
		return ONVIF_ERR_NoSource;
	}
	
	offset += snprintf(p_buf+offset, mlen-offset, "<timg:GetOptionsResponse><timg:ImagingOptions>");

	if (g_onvif_cfg.ImagingOptions.BacklightCompensationFlag)
	{
		offset += snprintf(p_buf+offset, mlen-offset, "<tt:BacklightCompensation>");
		if (g_onvif_cfg.ImagingOptions.BacklightCompensation.Mode_OFF)
		{
			offset += snprintf(p_buf+offset, mlen-offset, "<tt:Mode>OFF</tt:Mode>");
		}
		if (g_onvif_cfg.ImagingOptions.BacklightCompensation.Mode_ON)
		{
			offset += snprintf(p_buf+offset, mlen-offset, "<tt:Mode>ON</tt:Mode>");
		}
		if (g_onvif_cfg.ImagingOptions.BacklightCompensation.LevelFlag)
		{
			offset += snprintf(p_buf+offset, mlen-offset, 
			    "<tt:Level>"
		            "<tt:Min>%0.1f</tt:Min>"
		            "<tt:Max>%0.1f</tt:Max>"
		        "</tt:Level>",
		        g_onvif_cfg.ImagingOptions.BacklightCompensation.Level.Min,
		        g_onvif_cfg.ImagingOptions.BacklightCompensation.Level.Max);
	    }    
		offset += snprintf(p_buf+offset, mlen-offset, "</tt:BacklightCompensation>");
	}

	if (g_onvif_cfg.ImagingOptions.BrightnessFlag)
	{
		offset += snprintf(p_buf+offset, mlen-offset, 
		    "<tt:Brightness>"
	    	    "<tt:Min>%0.1f</tt:Min>"
	    	    "<tt:Max>%0.1f</tt:Max>"
		    "</tt:Brightness>",
			g_onvif_cfg.ImagingOptions.Brightness.Min, 
			g_onvif_cfg.ImagingOptions.Brightness.Max);
	}

	if (g_onvif_cfg.ImagingOptions.ColorSaturationFlag)
	{
		offset += snprintf(p_buf+offset, mlen-offset, 
		    "<tt:ColorSaturation>"
		        "<tt:Min>%0.1f</tt:Min>"
		        "<tt:Max>%0.1f</tt:Max>"
		    "</tt:ColorSaturation>",
			g_onvif_cfg.ImagingOptions.ColorSaturation.Min, 
			g_onvif_cfg.ImagingOptions.ColorSaturation.Max);
	}

	if (g_onvif_cfg.ImagingOptions.ContrastFlag)
	{
		offset += snprintf(p_buf+offset, mlen-offset, 
		    "<tt:Contrast>"
			    "<tt:Min>%0.1f</tt:Min>"
			    "<tt:Max>%0.1f</tt:Max>"
		    "</tt:Contrast>",
			g_onvif_cfg.ImagingOptions.Contrast.Min, 
			g_onvif_cfg.ImagingOptions.Contrast.Max);
	}

	if (g_onvif_cfg.ImagingOptions.ExposureFlag)
	{
		offset += snprintf(p_buf+offset, mlen-offset, "<tt:Exposure>");
		if (g_onvif_cfg.ImagingOptions.Exposure.Mode_AUTO)
		{
			offset += snprintf(p_buf+offset, mlen-offset, "<tt:Mode>AUTO</tt:Mode>");
		}
		if (g_onvif_cfg.ImagingOptions.Exposure.Mode_MANUAL)
		{
			offset += snprintf(p_buf+offset, mlen-offset, "<tt:Mode>MANUAL</tt:Mode>");
		}
		if (g_onvif_cfg.ImagingOptions.Exposure.Priority_LowNoise)
		{
			offset += snprintf(p_buf+offset, mlen-offset, "<tt:Priority>LowNoise</tt:Priority>");
		}
		if (g_onvif_cfg.ImagingOptions.Exposure.Priority_FrameRate)
		{
			offset += snprintf(p_buf+offset, mlen-offset, "<tt:Priority>FrameRate</tt:Priority>");
		}
		if (g_onvif_cfg.ImagingOptions.Exposure.MinExposureTimeFlag)
		{
			offset += snprintf(p_buf+offset, mlen-offset, 
			    "<tt:MinExposureTime>"
		    	    "<tt:Min>%0.1f</tt:Min>"
		    	    "<tt:Max>%0.1f</tt:Max>"
			    "</tt:MinExposureTime>",
				g_onvif_cfg.ImagingOptions.Exposure.MinExposureTime.Min, 
				g_onvif_cfg.ImagingOptions.Exposure.MinExposureTime.Max);
		}
		if (g_onvif_cfg.ImagingOptions.Exposure.MaxExposureTimeFlag)
		{
			offset += snprintf(p_buf+offset, mlen-offset, 
			    "<tt:MaxExposureTime>"
			        "<tt:Min>%0.1f</tt:Min>"
			        "<tt:Max>%0.1f</tt:Max>"
			    "</tt:MaxExposureTime>",
				g_onvif_cfg.ImagingOptions.Exposure.MaxExposureTime.Min, 
				g_onvif_cfg.ImagingOptions.Exposure.MaxExposureTime.Max);	
		}	
		if (g_onvif_cfg.ImagingOptions.Exposure.MinGainFlag)
		{
			offset += snprintf(p_buf+offset, mlen-offset, 
			    "<tt:MinGain>"
		    	    "<tt:Min>%0.1f</tt:Min>"
		    	    "<tt:Max>%0.1f</tt:Max>"
			    "</tt:MinGain>",
				g_onvif_cfg.ImagingOptions.Exposure.MinGain.Min, 
				g_onvif_cfg.ImagingOptions.Exposure.MinGain.Max);
		}	
		if (g_onvif_cfg.ImagingOptions.Exposure.MaxGainFlag)
		{
			offset += snprintf(p_buf+offset, mlen-offset, 
			    "<tt:MaxGain>"
		    	    "<tt:Min>%0.1f</tt:Min>"
		    	    "<tt:Max>%0.1f</tt:Max>"
			    "</tt:MaxGain>",
				g_onvif_cfg.ImagingOptions.Exposure.MaxGain.Min, 
				g_onvif_cfg.ImagingOptions.Exposure.MaxGain.Max);
		}
		if (g_onvif_cfg.ImagingOptions.Exposure.MinIrisFlag)
		{
			offset += snprintf(p_buf+offset, mlen-offset, 
			    "<tt:MinIris>"
		    	    "<tt:Min>%0.1f</tt:Min>"
		    	    "<tt:Max>%0.1f</tt:Max>"
			    "</tt:MinIris>",
				g_onvif_cfg.ImagingOptions.Exposure.MinIris.Min, 
				g_onvif_cfg.ImagingOptions.Exposure.MinIris.Max);
		}
		if (g_onvif_cfg.ImagingOptions.Exposure.MaxIrisFlag)
		{
			offset += snprintf(p_buf+offset, mlen-offset, 
			    "<tt:MaxIris>"
		    	    "<tt:Min>%0.1f</tt:Min>"
		    	    "<tt:Max>%0.1f</tt:Max>"
			    "</tt:MaxIris>",
				g_onvif_cfg.ImagingOptions.Exposure.MaxIris.Min, 
				g_onvif_cfg.ImagingOptions.Exposure.MaxIris.Max);	
		}	
		if (g_onvif_cfg.ImagingOptions.Exposure.ExposureTimeFlag)
		{
			offset += snprintf(p_buf+offset, mlen-offset, 
			    "<tt:ExposureTime>"
		    	    "<tt:Min>%0.1f</tt:Min>"
		    	    "<tt:Max>%0.1f</tt:Max>"
			    "</tt:ExposureTime>",
				g_onvif_cfg.ImagingOptions.Exposure.ExposureTime.Min, 
				g_onvif_cfg.ImagingOptions.Exposure.ExposureTime.Max);	
		}
		if (g_onvif_cfg.ImagingOptions.Exposure.GainFlag)
		{
			offset += snprintf(p_buf+offset, mlen-offset, 
			    "<tt:Gain>"
		    	    "<tt:Min>%0.1f</tt:Min>"
		    	    "<tt:Max>%0.1f</tt:Max>"
			    "</tt:Gain>",
				g_onvif_cfg.ImagingOptions.Exposure.Gain.Min, 
				g_onvif_cfg.ImagingOptions.Exposure.Gain.Max);	
		}	
		if (g_onvif_cfg.ImagingOptions.Exposure.IrisFlag)
		{
			offset += snprintf(p_buf+offset, mlen-offset, 
			    "<tt:Iris>"
		    	    "<tt:Min>%0.1f</tt:Min>"
		    	    "<tt:Max>%0.1f</tt:Max>"
			    "</tt:Iris>",
				g_onvif_cfg.ImagingOptions.Exposure.Iris.Min, 
				g_onvif_cfg.ImagingOptions.Exposure.Iris.Max);	
		}	
		offset += snprintf(p_buf+offset, mlen-offset, "</tt:Exposure>");
	}	

	if (g_onvif_cfg.ImagingOptions.FocusFlag)
	{
		offset += snprintf(p_buf+offset, mlen-offset, "<tt:Focus>");
		if (g_onvif_cfg.ImagingOptions.Focus.AutoFocusModes_AUTO)
		{
			offset += snprintf(p_buf+offset, mlen-offset, "<tt:AutoFocusModes>AUTO</tt:AutoFocusModes>");
		}
		if (g_onvif_cfg.ImagingOptions.Focus.AutoFocusModes_MANUAL)
		{
			offset += snprintf(p_buf+offset, mlen-offset, "<tt:AutoFocusModes>MANUAL</tt:AutoFocusModes>");
		}
		if (g_onvif_cfg.ImagingOptions.Focus.DefaultSpeedFlag)
		{
			offset += snprintf(p_buf+offset, mlen-offset, 
			    "<tt:DefaultSpeed>"
		    	    "<tt:Min>%0.1f</tt:Min>"
		    	    "<tt:Max>%0.1f</tt:Max>"
			    "</tt:DefaultSpeed>",
				g_onvif_cfg.ImagingOptions.Focus.DefaultSpeed.Min, 
				g_onvif_cfg.ImagingOptions.Focus.DefaultSpeed.Max);
		}	
		if (g_onvif_cfg.ImagingOptions.Focus.NearLimitFlag)
		{
			offset += snprintf(p_buf+offset, mlen-offset, 
			    "<tt:NearLimit>"
		    	    "<tt:Min>%0.1f</tt:Min>"
		    	    "<tt:Max>%0.1f</tt:Max>"
			    "</tt:NearLimit>",
				g_onvif_cfg.ImagingOptions.Focus.NearLimit.Min, 
				g_onvif_cfg.ImagingOptions.Focus.NearLimit.Max);
		}	
		if (g_onvif_cfg.ImagingOptions.Focus.FarLimitFlag)
		{
			offset += snprintf(p_buf+offset, mlen-offset, 
			    "<tt:FarLimit>"
		    	    "<tt:Min>%0.1f</tt:Min>"
		    	    "<tt:Max>%0.1f</tt:Max>"
			    "</tt:FarLimit>",
				g_onvif_cfg.ImagingOptions.Focus.FarLimit.Min, 
				g_onvif_cfg.ImagingOptions.Focus.FarLimit.Max);
		}	
		offset += snprintf(p_buf+offset, mlen-offset, "</tt:Focus>");
	}	
	
	if (g_onvif_cfg.ImagingOptions.IrCutFilterMode_ON)
	{
		offset += snprintf(p_buf+offset, mlen-offset, "<tt:IrCutFilterModes>ON</tt:IrCutFilterModes>");
	}
	if (g_onvif_cfg.ImagingOptions.IrCutFilterMode_OFF)
	{
		offset += snprintf(p_buf+offset, mlen-offset, "<tt:IrCutFilterModes>OFF</tt:IrCutFilterModes>");
	}
	if (g_onvif_cfg.ImagingOptions.IrCutFilterMode_AUTO)
	{
		offset += snprintf(p_buf+offset, mlen-offset, "<tt:IrCutFilterModes>AUTO</tt:IrCutFilterModes>");
	}

	if (g_onvif_cfg.ImagingOptions.SharpnessFlag)
	{
		offset += snprintf(p_buf+offset, mlen-offset, 
		    "<tt:Sharpness>"
		        "<tt:Min>%0.1f</tt:Min>"
		        "<tt:Max>%0.1f</tt:Max>"
		    "</tt:Sharpness>",
			g_onvif_cfg.ImagingOptions.Sharpness.Min, 
			g_onvif_cfg.ImagingOptions.Sharpness.Max);
	}	

	if (g_onvif_cfg.ImagingOptions.WideDynamicRangeFlag)
	{
		offset += snprintf(p_buf+offset, mlen-offset, "<tt:WideDynamicRange>");
		if (g_onvif_cfg.ImagingOptions.WideDynamicRange.Mode_ON)
		{
			offset += snprintf(p_buf+offset, mlen-offset, "<tt:Mode>ON</tt:Mode>");
		}
		if (g_onvif_cfg.ImagingOptions.WideDynamicRange.Mode_OFF)
		{
			offset += snprintf(p_buf+offset, mlen-offset, "<tt:Mode>OFF</tt:Mode>");
		}
		if (g_onvif_cfg.ImagingOptions.WideDynamicRange.LevelFlag)
		{
			offset += snprintf(p_buf+offset, mlen-offset, 
			    "<tt:Level>"
			        "<tt:Min>%0.1f</tt:Min>"
			        "<tt:Max>%0.1f</tt:Max>"
			    "</tt:Level>",
				g_onvif_cfg.ImagingOptions.WideDynamicRange.Level.Min, 
				g_onvif_cfg.ImagingOptions.WideDynamicRange.Level.Max);
		}	
		offset += snprintf(p_buf+offset, mlen-offset, "</tt:WideDynamicRange>");
	}

	if (g_onvif_cfg.ImagingOptions.WhiteBalanceFlag)
	{
		offset += snprintf(p_buf+offset, mlen-offset, "<tt:WhiteBalance>");
		if (g_onvif_cfg.ImagingOptions.WhiteBalance.Mode_AUTO)
		{
			offset += snprintf(p_buf+offset, mlen-offset, "<tt:Mode>AUTO</tt:Mode>");
		}
		if (g_onvif_cfg.ImagingOptions.WhiteBalance.Mode_MANUAL)
		{
			offset += snprintf(p_buf+offset, mlen-offset, "<tt:Mode>MANUAL</tt:Mode>");
		}
		if (g_onvif_cfg.ImagingOptions.WhiteBalance.YrGainFlag)
		{
			offset += snprintf(p_buf+offset, mlen-offset, 
			    "<tt:YrGain>"
			        "<tt:Min>%0.1f</tt:Min>"
			        "<tt:Max>%0.1f</tt:Max>"
			    "</tt:YrGain>",
				g_onvif_cfg.ImagingOptions.WhiteBalance.YrGain.Min, 
				g_onvif_cfg.ImagingOptions.WhiteBalance.YrGain.Max);
		}	
		if (g_onvif_cfg.ImagingOptions.WhiteBalance.YbGainFlag)
		{
			offset += snprintf(p_buf+offset, mlen-offset, 
			    "<tt:YbGain>"
			        "<tt:Min>%0.1f</tt:Min>"
			        "<tt:Max>%0.1f</tt:Max>"
			    "</tt:YbGain>",
				g_onvif_cfg.ImagingOptions.WhiteBalance.YbGain.Min, 
				g_onvif_cfg.ImagingOptions.WhiteBalance.YbGain.Max);
		}	
		offset += snprintf(p_buf+offset, mlen-offset, "</tt:WhiteBalance>");
	}

	offset += snprintf(p_buf+offset, mlen-offset, "</timg:ImagingOptions></timg:GetOptionsResponse>");

	return offset;
}

int build_SetImagingSettings_rly_xml(char * p_buf, int mlen, const char * argv)
{
	int offset = 0;
	offset += snprintf(p_buf+offset, mlen-offset, "<timg:SetImagingSettingsResponse />");
	return offset;
}

int build_FloatRange_xml(char * p_buf, int mlen, onvif_FloatRange * p_req)
{
    int offset = 0;

    offset += snprintf(p_buf+offset, mlen-offset, 
        "<tt:Min>%0.2f</tt:Min>"
        "<tt:Max>%0.2f</tt:Max>",
        p_req->Min, p_req->Max);

    return offset;
}

int build_GetMoveOptions_rly_xml(char * p_buf, int mlen, const char * argv)
{	
	int offset = 0;
	onvif_MoveOptions20 option;
	ONVIF_VideoSource * p_v_src = onvif_find_VideoSource(argv);
	if (NULL == p_v_src)
	{
		return ONVIF_ERR_NoSource;
	}

	memset(&option, 0, sizeof(option));

	if (ONVIF_OK != onvif_GetMoveOptions(p_v_src, &option))
	{
		return -1;
	}
	
	offset += snprintf(p_buf+offset, mlen-offset, "<timg:GetMoveOptionsResponse><timg:MoveOptions>");

	if (option.AbsoluteFlag)
	{
	    offset += snprintf(p_buf+offset, mlen-offset, "<tt:Absolute>");
	    offset += snprintf(p_buf+offset, mlen-offset, "<tt:Position>");
	    offset += build_FloatRange_xml(p_buf+offset, mlen-offset, &option.Absolute.Position);
	    offset += snprintf(p_buf+offset, mlen-offset, "</tt:Position>");
	    if (option.Absolute.SpeedFlag)
	    {
	        offset += snprintf(p_buf+offset, mlen-offset, "<tt:Speed>");
    	    offset += build_FloatRange_xml(p_buf+offset, mlen-offset, &option.Absolute.Speed);
    	    offset += snprintf(p_buf+offset, mlen-offset, "</tt:Speed>");
	    }
	    offset += snprintf(p_buf+offset, mlen-offset, "</tt:Absolute>");
	}
	
	if (option.RelativeFlag)
	{
	    offset += snprintf(p_buf+offset, mlen-offset, "<tt:Relative>");
	    offset += snprintf(p_buf+offset, mlen-offset, "<tt:Distance>");
	    offset += build_FloatRange_xml(p_buf+offset, mlen-offset, &option.Relative.Distance);
	    offset += snprintf(p_buf+offset, mlen-offset, "</tt:Distance>");
	    if (option.Absolute.SpeedFlag)
	    {
	        offset += snprintf(p_buf+offset, mlen-offset, "<tt:Speed>");
    	    offset += build_FloatRange_xml(p_buf+offset, mlen-offset, &option.Relative.Speed);
    	    offset += snprintf(p_buf+offset, mlen-offset, "</tt:Speed>");
	    }
	    offset += snprintf(p_buf+offset, mlen-offset, "</tt:Relative>");
	}
	
	if (option.ContinuousFlag)
	{
	    offset += snprintf(p_buf+offset, mlen-offset, "<tt:Continuous><tt:Speed>");
	    offset += build_FloatRange_xml(p_buf+offset, mlen-offset, &option.Continuous.Speed);
	    offset += snprintf(p_buf+offset, mlen-offset, "</tt:Speed></tt:Continuous>");
	}
	
	offset += snprintf(p_buf+offset, mlen-offset, "</timg:MoveOptions></timg:GetMoveOptionsResponse>");
	
	return offset;
}

int build_Move_rly_xml(char * p_buf, int mlen, const char * argv)
{
	int offset = 0;
	offset += snprintf(p_buf+offset, mlen-offset, "<timg:MoveResponse />");
	return offset;
}

int build_img_GetStatus_rly_xml(char * p_buf, int mlen, const char * argv)
{
	int offset = 0;
	onvif_ImagingStatus status;
	ONVIF_VideoSource * p_v_src = onvif_find_VideoSource(argv);
	if (NULL == p_v_src)
	{
		return ONVIF_ERR_NoSource;
	}

	memset(&status, 0, sizeof(status));

	if (ONVIF_OK != onvif_img_GetStatus(p_v_src, &status))
	{
		return -1;
	}

	offset += snprintf(p_buf+offset, mlen-offset, "<timg:GetStatusResponse>");
	
	if (status.FocusStatusFlag)
	{
		offset += snprintf(p_buf+offset, mlen-offset, "<timg:Status>");
		offset += snprintf(p_buf+offset, mlen-offset, "<tt:FocusStatus20>");
		offset += snprintf(p_buf+offset, mlen-offset, 
			"<tt:Position>%0.1f</tt:Position>"
			"<tt:MoveStatus>%s</tt:MoveStatus>",
			status.FocusStatus.Position,
			onvif_MoveStatusToString(status.FocusStatus.MoveStatus));
		if (status.FocusStatus.ErrorFlag && status.FocusStatus.Error[0] != '\0')
		{
			offset += snprintf(p_buf+offset, mlen-offset, "<tt:Error>%s</tt:Error>", status.FocusStatus.Error);
		}
		offset += snprintf(p_buf+offset, mlen-offset, "</tt:FocusStatus20>");
		offset += snprintf(p_buf+offset, mlen-offset, "</timg:Status>");
	}
	
	offset += snprintf(p_buf+offset, mlen-offset, "</timg:GetStatusResponse>");	
	
	return offset;
}

int build_img_Stop_rly_xml(char * p_buf, int mlen, const char * argv)
{
	int offset = 0;
	offset += snprintf(p_buf+offset, mlen-offset, "<timg:StopResponse />");
	return offset;
}

int build_GetUsers_rly_xml(char * p_buf, int mlen, const char * argv)
{
	int i;
	int offset = 0;

    offset += snprintf(p_buf+offset, mlen-offset, "<tds:GetUsersResponse>");

	for (i = 0; i < ARRAY_SIZE(g_onvif_cfg.users); i++)
	{
		if (g_onvif_cfg.users[i].Username[0] != '\0')
		{		
			offset += snprintf(p_buf+offset, mlen-offset, 
				"<tds:User>"
					"<tt:Username>%s</tt:Username>"
					"<tt:UserLevel>%s</tt:UserLevel>"
				"</tds:User>", 
				g_onvif_cfg.users[i].Username, 
				onvif_UserLevelToString(g_onvif_cfg.users[i].UserLevel));
		}
	}
    
    offset += snprintf(p_buf+offset, mlen-offset, "</tds:GetUsersResponse>");    	

	return offset;
}

int build_CreateUsers_rly_xml(char * p_buf, int mlen, const char * argv)
{
	int offset = 0;
	offset += snprintf(p_buf+offset, mlen-offset, "<tds:CreateUsersResponse />");
	return offset;
}

int build_DeleteUsers_rly_xml(char * p_buf, int mlen, const char * argv)
{
	int offset = 0;
	offset += snprintf(p_buf+offset, mlen-offset, "<tds:DeleteUsersResponse />");
	return offset;
}

int build_SetUser_rly_xml(char * p_buf, int mlen, const char * argv)
{
	int offset = 0;
	offset += snprintf(p_buf+offset, mlen-offset, "<tds:SetUserResponse />");
	return offset;
}

int build_GetRemoteUser_rly_xml(char * p_buf, int mlen, const char * argv)
{
    int offset = 0;
    GetRemoteUser_RES * p_res = (GetRemoteUser_RES *) argv;

	offset += snprintf(p_buf+offset, mlen-offset, "<tds:GetRemoteUserResponse>");

	if (p_res->RemoteUserFlag)
	{
    	offset += snprintf(p_buf+offset, mlen-offset, 
	        "<tds:RemoteUser>"
        	    "<tt:Username>%s</tt:Username>"
                "<tt:UseDerivedPassword>%s</tt:UseDerivedPassword>"
            "</tds:RemoteUser>",
            p_res->RemoteUser.Username,
    	    p_res->RemoteUser.UseDerivedPassword ? "true" : "false");
    }
    
	offset += snprintf(p_buf+offset, mlen-offset, "</tds:GetRemoteUserResponse>");
	
	return offset;
}

int build_SetRemoteUser_rly_xml(char * p_buf, int mlen, const char * argv)
{
	int offset = 0;
	offset += snprintf(p_buf+offset, mlen-offset, "<tds:SetRemoteUserResponse />");
	return offset;
}

int build_StartFirmwareUpgrade_rly_xml(char * p_buf, int mlen, const char * argv)
{
    int offset = 0;
	StartFirmwareUpgrade_RES * p_res = (StartFirmwareUpgrade_RES *) argv;
	
	offset += snprintf(p_buf+offset, mlen-offset, 
		"<tds:StartFirmwareUpgradeResponse>"
	   		"<tds:UploadUri>%s</tds:UploadUri>"
	   		"<tds:UploadDelay>PT%dS</tds:UploadDelay>"
	   		"<tds:ExpectedDownTime>PT%dS</tds:ExpectedDownTime>"
  		"</tds:StartFirmwareUpgradeResponse>", 
  		p_res->UploadUri, p_res->UploadDelay, p_res->ExpectedDownTime);
	
	return offset;
}

int build_StartSystemRestore_rly_xml(char * p_buf, int mlen, const char * argv)
{
    int offset = 0;
    StartSystemRestore_RES * p_res = (StartSystemRestore_RES *) argv;
	
	offset += snprintf(p_buf+offset, mlen-offset, 
		"<tds:StartSystemRestoreResponse>"
	   		"<tds:UploadUri>%s</tds:UploadUri>"
	   		"<tds:ExpectedDownTime>PT%dS</tds:ExpectedDownTime>"
  		"</tds:StartSystemRestoreResponse>", 
  		p_res->UploadUri, p_res->ExpectedDownTime);
	
	return offset;
}

int build_OSD_xml(char * p_buf, int mlen, ONVIF_OSDConfiguration * p_osd)
{
	int offset = 0;

	offset += snprintf(p_buf+offset, mlen-offset, "<tt:VideoSourceConfigurationToken>%s</tt:VideoSourceConfigurationToken>",
		p_osd->OSD.VideoSourceConfigurationToken);
	offset += snprintf(p_buf+offset, mlen-offset, "<tt:Type>%s</tt:Type>",
		onvif_OSDTypeToString(p_osd->OSD.Type));

	offset += snprintf(p_buf+offset, mlen-offset, "<tt:Position><tt:Type>%s</tt:Type>", 
	    onvif_OSDPosTypeToString(p_osd->OSD.Position.Type));
	if (p_osd->OSD.Position.PosFlag)
	{
	    offset += snprintf(p_buf+offset, mlen-offset, "<tt:Pos x=\"%0.2f\" y=\"%0.2f\"></tt:Pos>", 
		    p_osd->OSD.Position.Pos.x, p_osd->OSD.Position.Pos.y);
	}
	offset += snprintf(p_buf+offset, mlen-offset, "</tt:Position>");

	if (p_osd->OSD.Type == OSDType_Text)
	{
		offset += snprintf(p_buf+offset, mlen-offset, "<tt:TextString>");
		offset += snprintf(p_buf+offset, mlen-offset, "<tt:Type>%s</tt:Type>", onvif_OSDTextTypeToString(p_osd->OSD.TextString.Type));

		if (p_osd->OSD.TextString.Type == OSDTextType_Date)
		{
			offset += snprintf(p_buf+offset, mlen-offset, "<tt:DateFormat>%s</tt:DateFormat>", p_osd->OSD.TextString.DateFormat);
		}
		else if (p_osd->OSD.TextString.Type == OSDTextType_Time)
		{
			offset += snprintf(p_buf+offset, mlen-offset, "<tt:TimeFormat>%s</tt:TimeFormat>", p_osd->OSD.TextString.TimeFormat);
		}
		else if (p_osd->OSD.TextString.Type == OSDTextType_DateAndTime)
		{
			offset += snprintf(p_buf+offset, mlen-offset, "<tt:DateFormat>%s</tt:DateFormat>", p_osd->OSD.TextString.DateFormat);
			offset += snprintf(p_buf+offset, mlen-offset, "<tt:TimeFormat>%s</tt:TimeFormat>", p_osd->OSD.TextString.TimeFormat);
		}

		if (p_osd->OSD.TextString.FontSizeFlag)
		{
			offset += snprintf(p_buf+offset, mlen-offset, "<tt:FontSize>%d</tt:FontSize>", p_osd->OSD.TextString.FontSize);
		}

		if (p_osd->OSD.TextString.FontColorFlag)
		{
			if (p_osd->OSD.TextString.FontColor.TransparentFlag)
			{
				offset += snprintf(p_buf+offset, mlen-offset, "<tt:FontColor Transparent=\"%d\">", p_osd->OSD.TextString.FontColor.Transparent);
			}	
			else
			{
				offset += snprintf(p_buf+offset, mlen-offset, "<tt:FontColor>");
			}

			offset += snprintf(p_buf+offset, mlen-offset, "<tt:Color X=\"%0.1f\" Y=\"%0.1f\" Z=\"%0.1f\"></tt:Color>", 
				p_osd->OSD.TextString.FontColor.X, p_osd->OSD.TextString.FontColor.Y, p_osd->OSD.TextString.FontColor.Z);

			offset += snprintf(p_buf+offset, mlen-offset, "</tt:FontColor>");	
		}

		if (p_osd->OSD.TextString.BackgroundColorFlag)
		{
			if (p_osd->OSD.TextString.BackgroundColor.TransparentFlag)
			{
				offset += snprintf(p_buf+offset, mlen-offset, "<tt:BackgroundColor Transparent=\"%d\">", p_osd->OSD.TextString.BackgroundColor.Transparent);
			}	
			else
			{
				offset += snprintf(p_buf+offset, mlen-offset, "<tt:BackgroundColor>");
			}

			offset += snprintf(p_buf+offset, mlen-offset, "<tt:Color X=\"%0.1f\" Y=\"%0.1f\" Z=\"%0.1f\"></tt:Color>", 
				p_osd->OSD.TextString.BackgroundColor.X, p_osd->OSD.TextString.BackgroundColor.Y, p_osd->OSD.TextString.BackgroundColor.Z);

			offset += snprintf(p_buf+offset, mlen-offset, "</tt:BackgroundColor>");	
		}

		if (p_osd->OSD.TextString.Type == OSDTextType_Plain)
		{
			offset += snprintf(p_buf+offset, mlen-offset, "<tt:PlainText>%s</tt:PlainText>", p_osd->OSD.TextString.PlainText);
		}

		offset += snprintf(p_buf+offset, mlen-offset, "</tt:TextString>");
	}
	else if (p_osd->OSD.Type == OSDType_Image)
	{
		offset += snprintf(p_buf+offset, mlen-offset, "<tt:Image><tt:ImgPath>%s</tt:ImgPath></tt:Image>", p_osd->OSD.Image.ImgPath);
	}	
	
	return offset;
}

int build_GetOSDs_rly_xml(char * p_buf, int mlen, const char * argv)
{
	int offset = 0;
	ONVIF_OSDConfiguration * p_osd;
	GetOSDs_REQ * p_req = (GetOSDs_REQ *) argv;

	if (p_req->ConfigurationTokenFlag)
	{
		ONVIF_VideoSourceConfiguration * p_v_src = onvif_find_VideoSourceConfiguration(p_req->ConfigurationToken);
		if (NULL == p_v_src)
		{
			return ONVIF_ERR_NoConfig;
		}
	}
	
	p_osd = g_onvif_cfg.OSDs;

	offset += snprintf(p_buf+offset, mlen-offset, "<trt:GetOSDsResponse>");

	while (p_osd)
	{
		offset += snprintf(p_buf+offset, mlen-offset, "<trt:OSDs token=\"%s\">", p_osd->OSD.token);
		offset += build_OSD_xml(p_buf+offset, mlen-offset, p_osd);
		offset += snprintf(p_buf+offset, mlen-offset, "</trt:OSDs>");
		p_osd = p_osd->next;
	}

	offset += snprintf(p_buf+offset, mlen-offset, "</trt:GetOSDsResponse>");
	
	return offset;
}

int build_GetOSD_rly_xml(char * p_buf, int mlen, const char * argv)
{
	int offset = 0;
	GetOSD_REQ * p_req = (GetOSD_REQ *) argv;	
	ONVIF_OSDConfiguration * p_osd = onvif_find_OSDConfiguration(p_req->OSDToken);
	if (NULL == p_osd)
	{
		return ONVIF_ERR_NoConfig;
	}

	offset += snprintf(p_buf+offset, mlen-offset, "<trt:GetOSDResponse>");

	if (p_osd)
	{
		offset += snprintf(p_buf+offset, mlen-offset, "<trt:OSD token=\"%s\">", p_osd->OSD.token);
		offset += build_OSD_xml(p_buf+offset, mlen-offset, p_osd);
		offset += snprintf(p_buf+offset, mlen-offset, "</trt:OSD>");
		p_osd = p_osd->next;
	}

	offset += snprintf(p_buf+offset, mlen-offset, "</trt:GetOSDResponse>");
	
	return offset;
}

int build_SetOSD_rly_xml(char * p_buf, int mlen, const char * argv)
{
	int offset = 0;
	offset += snprintf(p_buf+offset, mlen-offset, "<trt:SetOSDResponse />");
	return offset;
}

int build_CreateOSD_rly_xml(char * p_buf, int mlen, const char * argv)
{
	int offset = 0;

	offset += snprintf(p_buf+offset, mlen-offset, 
	    "<trt:CreateOSDResponse><trt:OSDToken>%s</trt:OSDToken></trt:CreateOSDResponse>", argv);

	return offset;
}

int build_DeleteOSD_rly_xml(char * p_buf, int mlen, const char * argv)
{
	int offset = 0;
	offset += snprintf(p_buf+offset, mlen-offset, "<trt:DeleteOSDResponse />");
	return offset;
}

int build_GetOSDOptions_rly_xml(char * p_buf, int mlen, const char * argv)
{
	int i;
	int offset = 0;

	offset += snprintf(p_buf+offset, mlen-offset, "<trt:GetOSDOptionsResponse><trt:OSDOptions>");

	offset += snprintf(p_buf+offset, mlen-offset, "<tt:MaximumNumberOfOSDs Total=\"%d\"",
		g_onvif_cfg.OSDConfigurationOptions.MaximumNumberOfOSDs.Total);
	if (g_onvif_cfg.OSDConfigurationOptions.MaximumNumberOfOSDs.ImageFlag)
	{
		offset += snprintf(p_buf+offset, mlen-offset, " Image=\"%d\"", 
			g_onvif_cfg.OSDConfigurationOptions.MaximumNumberOfOSDs.Image);
	}
	if (g_onvif_cfg.OSDConfigurationOptions.MaximumNumberOfOSDs.PlainTextFlag)
	{
		offset += snprintf(p_buf+offset, mlen-offset, " PlainText=\"%d\"", 
			g_onvif_cfg.OSDConfigurationOptions.MaximumNumberOfOSDs.PlainText);
	}
	if (g_onvif_cfg.OSDConfigurationOptions.MaximumNumberOfOSDs.DateFlag)
	{
		offset += snprintf(p_buf+offset, mlen-offset, " Date=\"%d\"", 
			g_onvif_cfg.OSDConfigurationOptions.MaximumNumberOfOSDs.Date);
	}
	if (g_onvif_cfg.OSDConfigurationOptions.MaximumNumberOfOSDs.TimeFlag)
	{
		offset += snprintf(p_buf+offset, mlen-offset, " Time=\"%d\"", 
			g_onvif_cfg.OSDConfigurationOptions.MaximumNumberOfOSDs.Time);
	}
	if (g_onvif_cfg.OSDConfigurationOptions.MaximumNumberOfOSDs.DateAndTimeFlag)
	{
		offset += snprintf(p_buf+offset, mlen-offset, " DateAndTime=\"%d\"", 
			g_onvif_cfg.OSDConfigurationOptions.MaximumNumberOfOSDs.DateAndTime);
	}
	offset += snprintf(p_buf+offset, mlen-offset, "></tt:MaximumNumberOfOSDs>");

	if (g_onvif_cfg.OSDConfigurationOptions.OSDType_Text)
	{
		offset += snprintf(p_buf+offset, mlen-offset, "<tt:Type>%s</tt:Type>", 
			onvif_OSDTypeToString(OSDType_Text));
	}
	if (g_onvif_cfg.OSDConfigurationOptions.OSDType_Image)
	{
		offset += snprintf(p_buf+offset, mlen-offset, "<tt:Type>%s</tt:Type>", 
			onvif_OSDTypeToString(OSDType_Image));
	}
	if (g_onvif_cfg.OSDConfigurationOptions.OSDType_Extended)
	{
		offset += snprintf(p_buf+offset, mlen-offset, "<tt:Type>%s</tt:Type>", 
			onvif_OSDTypeToString(OSDType_Extended));
	}

	if (g_onvif_cfg.OSDConfigurationOptions.OSDPosType_LowerLeft)
	{
		offset += snprintf(p_buf+offset, mlen-offset, "<tt:PositionOption>%s</tt:PositionOption>", 
			onvif_OSDPosTypeToString(OSDPosType_LowerLeft));
	}
	if (g_onvif_cfg.OSDConfigurationOptions.OSDPosType_LowerRight)
	{
		offset += snprintf(p_buf+offset, mlen-offset, "<tt:PositionOption>%s</tt:PositionOption>", 
			onvif_OSDPosTypeToString(OSDPosType_LowerRight));
	}
	if (g_onvif_cfg.OSDConfigurationOptions.OSDPosType_UpperLeft)
	{
		offset += snprintf(p_buf+offset, mlen-offset, "<tt:PositionOption>%s</tt:PositionOption>", 
			onvif_OSDPosTypeToString(OSDPosType_UpperLeft));
	}
	if (g_onvif_cfg.OSDConfigurationOptions.OSDPosType_UpperRight)
	{
		offset += snprintf(p_buf+offset, mlen-offset, "<tt:PositionOption>%s</tt:PositionOption>", 
			onvif_OSDPosTypeToString(OSDPosType_UpperRight));
	}
	if (g_onvif_cfg.OSDConfigurationOptions.OSDPosType_Custom)
	{
		offset += snprintf(p_buf+offset, mlen-offset, "<tt:PositionOption>%s</tt:PositionOption>", 
			onvif_OSDPosTypeToString(OSDPosType_Custom));
	}

	if (g_onvif_cfg.OSDConfigurationOptions.TextOptionFlag)
	{
		offset += snprintf(p_buf+offset, mlen-offset, "<tt:TextOption>");
		
		if (g_onvif_cfg.OSDConfigurationOptions.TextOption.OSDTextType_Plain)
		{
			offset += snprintf(p_buf+offset, mlen-offset, "<tt:Type>%s</tt:Type>", 
				onvif_OSDTextTypeToString(OSDTextType_Plain));
		}
		if (g_onvif_cfg.OSDConfigurationOptions.TextOption.OSDTextType_Date)
		{
			offset += snprintf(p_buf+offset, mlen-offset, "<tt:Type>%s</tt:Type>", 
				onvif_OSDTextTypeToString(OSDTextType_Date));
		}
		if (g_onvif_cfg.OSDConfigurationOptions.TextOption.OSDTextType_Time)
		{
			offset += snprintf(p_buf+offset, mlen-offset, "<tt:Type>%s</tt:Type>", 
				onvif_OSDTextTypeToString(OSDTextType_Time));
		}
		if (g_onvif_cfg.OSDConfigurationOptions.TextOption.OSDTextType_DateAndTime)
		{
			offset += snprintf(p_buf+offset, mlen-offset, "<tt:Type>%s</tt:Type>", 
				onvif_OSDTextTypeToString(OSDTextType_DateAndTime));
		}

		if (g_onvif_cfg.OSDConfigurationOptions.TextOption.FontSizeRangeFlag)
		{
			offset += snprintf(p_buf+offset, mlen-offset, 
				"<tt:FontSizeRange>"
					"<tt:Min>%d</tt:Min>"				
					"<tt:Max>%d</tt:Max>"				
				"</tt:FontSizeRange>", 
				g_onvif_cfg.OSDConfigurationOptions.TextOption.FontSizeRange.Min,
				g_onvif_cfg.OSDConfigurationOptions.TextOption.FontSizeRange.Max);
		}

		for (i = 0; i < g_onvif_cfg.OSDConfigurationOptions.TextOption.DateFormatSize; i++)
		{
			offset += snprintf(p_buf+offset, mlen-offset, "<tt:DateFormat>%s</tt:DateFormat>",
				g_onvif_cfg.OSDConfigurationOptions.TextOption.DateFormat[i]);
		}
		
		for (i = 0; i < g_onvif_cfg.OSDConfigurationOptions.TextOption.TimeFormatSize; i++)
		{
			offset += snprintf(p_buf+offset, mlen-offset, "<tt:TimeFormat>%s</tt:TimeFormat>",
				g_onvif_cfg.OSDConfigurationOptions.TextOption.TimeFormat[i]);
		}

		// build onvif color options ...

		offset += snprintf(p_buf+offset, mlen-offset, "</tt:TextOption>");
	}

	if (g_onvif_cfg.OSDConfigurationOptions.ImageOptionFlag)
	{
		offset += snprintf(p_buf+offset, mlen-offset, "<tt:ImageOption>");

		for (i = 0; i < g_onvif_cfg.OSDConfigurationOptions.ImageOption.ImagePathSize; i++)
		{
			offset += snprintf(p_buf+offset, mlen-offset, "<tt:ImagePath>%s</tt:ImagePath>",
				g_onvif_cfg.OSDConfigurationOptions.ImageOption.ImagePath[i]);
		}
		
		offset += snprintf(p_buf+offset, mlen-offset, "</tt:ImageOption>");
	}
	
	offset += snprintf(p_buf+offset, mlen-offset, "</trt:OSDOptions></trt:GetOSDOptionsResponse>");

	return offset;
}

int build_SetConfiguration_rly_xml(char * p_buf, int mlen, const char * argv)
{
	int offset = 0;
	offset += snprintf(p_buf+offset, mlen-offset, "<tptz:SetConfigurationResponse />");
	return offset;
}

int build_StartMulticastStreaming_rly_xml(char * p_buf, int mlen, const char * argv)
{
	int offset = 0;
	offset += snprintf(p_buf+offset, mlen-offset, "<trt:StartMulticastStreamingResponse />");
	return offset;
}

int build_StopMulticastStreaming_rly_xml(char * p_buf, int mlen, const char * argv)
{
	int offset = 0;
	offset += snprintf(p_buf+offset, mlen-offset, "<trt:StopMulticastStreamingResponse />");
	return offset;
}

int build_GetMetadataConfigurations_rly_xml(char * p_buf, int mlen, const char * argv)
{
    int offset = 0;
	ONVIF_MetadataConfiguration * p_cfg = g_onvif_cfg.metadata_cfg;

	offset += snprintf(p_buf+offset, mlen-offset, "<trt:GetMetadataConfigurationsResponse>");
	while (p_cfg)
	{
		offset += snprintf(p_buf+offset, mlen-offset, "<trt:Configurations token=\"%s\">", p_cfg->Configuration.token);
		offset += build_MetadataConfiguration_xml(p_buf+offset, mlen-offset, &p_cfg->Configuration);
		offset += snprintf(p_buf+offset, mlen-offset, "</trt:Configurations>");
		
		p_cfg = p_cfg->next;
	}
	offset += snprintf(p_buf+offset, mlen-offset, "</trt:GetMetadataConfigurationsResponse>");
	
	return offset;
}

int build_GetMetadataConfiguration_rly_xml(char * p_buf, int mlen, const char * argv)
{
	int offset = 0;
    ONVIF_MetadataConfiguration * p_cfg = onvif_find_MetadataConfiguration(argv);
    if (NULL == p_cfg)
    {
    	return ONVIF_ERR_NoConfig;
    }

	offset += snprintf(p_buf+offset, mlen-offset, "<trt:GetMetadataConfigurationResponse>");
	offset += snprintf(p_buf+offset, mlen-offset, "<trt:Configuration token=\"%s\">", p_cfg->Configuration.token);
	offset += build_MetadataConfiguration_xml(p_buf+offset, mlen-offset, &p_cfg->Configuration);
	offset += snprintf(p_buf+offset, mlen-offset, "</trt:Configuration>");
	offset += snprintf(p_buf+offset, mlen-offset, "</trt:GetMetadataConfigurationResponse>");

	return offset;
}

int build_GetCompatibleMetadataConfigurations_rly_xml(char * p_buf, int mlen, const char * argv)
{
	int offset = 0;	
	ONVIF_MetadataConfiguration * p_cfg = g_onvif_cfg.metadata_cfg;
	ONVIF_PROFILE * p_profile = onvif_find_profile(argv);
	if (NULL == p_profile)
	{
		return ONVIF_ERR_NoProfile;
	}

	offset += snprintf(p_buf+offset, mlen-offset, "<trt:GetCompatibleMetadataConfigurationsResponse>");
	while (p_cfg)
	{
		offset += snprintf(p_buf+offset, mlen-offset, "<trt:Configurations token=\"%s\">", p_cfg->Configuration.token);
		offset += build_MetadataConfiguration_xml(p_buf+offset, mlen-offset, &p_cfg->Configuration);
		offset += snprintf(p_buf+offset, mlen-offset, "</trt:Configurations>");
		
		p_cfg = p_cfg->next;
	}
	offset += snprintf(p_buf+offset, mlen-offset, "</trt:GetCompatibleMetadataConfigurationsResponse>");
	
	return offset;
}

int build_MetadataConfigurationOptions_xml(char * p_buf, int mlen, onvif_MetadataConfigurationOptions * p_req)
{
    int offset = 0;
    
    offset += snprintf(p_buf+offset, mlen-offset, "<tt:PTZStatusFilterOptions>");

	offset += snprintf(p_buf+offset, mlen-offset, 
		"<tt:PanTiltStatusSupported>%s</tt:PanTiltStatusSupported>"
		"<tt:ZoomStatusSupported>%s</tt:ZoomStatusSupported>",
		p_req->PTZStatusFilterOptions.PanTiltStatusSupported ? "true" : "false",
		p_req->PTZStatusFilterOptions.ZoomStatusSupported ? "true" : "false");

	if (p_req->PTZStatusFilterOptions.PanTiltPositionSupportedFlag)
	{
		offset += snprintf(p_buf+offset, mlen-offset, 
			"<tt:PanTiltPositionSupported>%s</tt:PanTiltPositionSupported>",
			p_req->PTZStatusFilterOptions.PanTiltPositionSupported ? "true" : "false");
	}

	if (p_req->PTZStatusFilterOptions.ZoomPositionSupportedFlag)
	{
		offset += snprintf(p_buf+offset, mlen-offset, 
			"<tt:ZoomPositionSupported>%s</tt:ZoomPositionSupported>",
			p_req->PTZStatusFilterOptions.ZoomPositionSupported ? "true" : "false");
	}
	
	offset += snprintf(p_buf+offset, mlen-offset, "</tt:PTZStatusFilterOptions>");

	return offset;
}

int build_GetMetadataConfigurationOptions_rly_xml(char * p_buf, int mlen, const char * argv)
{
	int offset = 0;
	ONVIF_PROFILE * p_profile = NULL;
	GetMetadataConfigurationOptions_REQ * p_req = (GetMetadataConfigurationOptions_REQ *) argv;

	if (p_req->ProfileTokenFlag)
	{
		p_profile = onvif_find_profile(p_req->ProfileToken);
		if (NULL == p_profile)
		{
			return ONVIF_ERR_NoProfile;
		}
	}

	offset += snprintf(p_buf+offset, mlen-offset, "<trt:GetMetadataConfigurationOptionsResponse>");
	offset += snprintf(p_buf+offset, mlen-offset, "<trt:Options>");

	offset += build_MetadataConfigurationOptions_xml(p_buf+offset, mlen-offset, &g_onvif_cfg.MetadataConfigurationOptions);
	
	offset += snprintf(p_buf+offset, mlen-offset, "</trt:Options>");
	offset += snprintf(p_buf+offset, mlen-offset, "</trt:GetMetadataConfigurationOptionsResponse>");
	
	return offset;
}

int build_SetMetadataConfiguration_rly_xml(char * p_buf, int mlen, const char * argv)
{
	int offset = 0;
	offset += snprintf(p_buf+offset, mlen-offset, "<trt:SetMetadataConfigurationResponse />");
	return offset;
}

int build_AddMetadataConfiguration_rly_xml(char * p_buf, int mlen, const char * argv)
{
	int offset = 0;
	offset += snprintf(p_buf+offset, mlen-offset, "<trt:AddMetadataConfigurationResponse />");
	return offset;
}

int build_RemoveMetadataConfiguration_rly_xml(char * p_buf, int mlen, const char * argv)
{
	int offset = 0;
	offset += snprintf(p_buf+offset, mlen-offset, "<trt:RemoveMetadataConfigurationResponse />");
	return offset;
}

int build_IntList_xml(char * p_buf, int mlen, onvif_IntList * p_req)
{
    int i, offset = 0;

    for (i = 0; i < p_req->sizeItems; i++)
    {
        offset += snprintf(p_buf+offset, mlen-offset,
            "<tt:Items>%d</tt:Items>", p_req->Items[i]);
    }       

    return offset;
}

int build_FloatList_xml(char * p_buf, int mlen, onvif_FloatList * p_req)
{
    int i, offset = 0;

    for (i = 0; i < p_req->sizeItems; i++)
    {
        offset += snprintf(p_buf+offset, mlen-offset,
            "<tt:Items>%0.2f</tt:Items>", p_req->Items[i]);
    }       

    return offset;
}

int build_GetVideoSourceModes_rly_xml(char * p_buf, int mlen, const char * argv)
{
    int offset = 0;
    GetVideoSourceModes_REQ * p_req = (GetVideoSourceModes_REQ *)argv;

    ONVIF_VideoSource * p_v_src = onvif_find_VideoSource(p_req->VideoSourceToken);
    if (NULL == p_v_src)
    {
        return ONVIF_ERR_NoVideoSource;
    }

    offset += snprintf(p_buf+offset, mlen-offset, "<trt:GetVideoSourceModesResponse>");
    offset += snprintf(p_buf+offset, mlen-offset, "<trt:VideoSourceModes token=\"%s\" Enabled=\"%s\">",
        p_v_src->VideoSourceMode.token, p_v_src->VideoSourceMode.Enabled ? "true" : "false");

    offset += snprintf(p_buf+offset, mlen-offset, 
        "<trt:MaxFramerate>%0.1f</trt:MaxFramerate>"
        "<trt:MaxResolution>"
            "<tt:Width>%d</tt:Width>"
            "<tt:Height>%d</tt:Height>"
        "</trt:MaxResolution>"
        "<trt:Encodings>%s</trt:Encodings>"
        "<trt:Reboot>%s</trt:Reboot>",
        p_v_src->VideoSourceMode.MaxFramerate,
        p_v_src->VideoSourceMode.MaxResolution.Width,
        p_v_src->VideoSourceMode.MaxResolution.Height,
        p_v_src->VideoSourceMode.Encodings,
        p_v_src->VideoSourceMode.Reboot ? "true" : "false");

    if (p_v_src->VideoSourceMode.DescriptionFlag)
    {
        offset += snprintf(p_buf+offset, mlen-offset, 
            "<trt:Description>%s</trt:Description>",
            p_v_src->VideoSourceMode.Description);
    }

    offset += snprintf(p_buf+offset, mlen-offset, "</trt:VideoSourceModes>");
    offset += snprintf(p_buf+offset, mlen-offset, "</trt:GetVideoSourceModesResponse>");

	return offset;
}

int build_SetVideoSourceMode_rly_xml(char * p_buf, int mlen, const char * argv)
{
    int offset = 0;
    SetVideoSourceMode_RES * p_res = (SetVideoSourceMode_RES *) argv;
    
    offset += snprintf(p_buf+offset, mlen-offset, 
        "<trt:SetVideoSourceModeResponse>"
            "<trt:Reboot>%s</trt:Reboot>"
        "</trt:SetVideoSourceModeResponse>",
        p_res->Reboot ? "true" : "false");

	return offset;
}

int build_StreamSetup_xml(char * p_buf, int mlen, onvif_StreamSetup * p_res)
{
    int offset = 0;

    offset += snprintf(p_buf+offset, mlen-offset, 
		"<tt:Stream>%s</tt:Stream>"
		"<tt:Transport>"
			"<tt:Protocol>%s</tt:Protocol>"
		"</tt:Transport>",
		onvif_StreamTypeToString(p_res->Stream),
		onvif_TransportProtocolToString(p_res->Transport.Protocol));
		
    return offset;
}

#ifdef IPFILTER_SUPPORT	

int build_IPAddressFilter_xml(char * p_buf, int mlen, onvif_IPAddressFilter * p_res)
{
    int i;
    int offset = 0;

    offset += snprintf(p_buf+offset, mlen-offset, "<tt:Type>%s</tt:Type>", 
        onvif_IPAddressFilterTypeToString(p_res->Type));

	for (i = 0; i < ARRAY_SIZE(p_res->IPv4Address); i++)
    {
        if (p_res->IPv4Address[i].Address[0] == '\0')
        {
            continue;
        }
        
        offset += snprintf(p_buf+offset, mlen-offset, 
            "<tt:IPv4Address>"
                "<tt:Address>%s</tt:Address>"
                "<tt:PrefixLength>%d</tt:PrefixLength>"
            "</tt:IPv4Address>", 
            p_res->IPv4Address[i].Address,
            p_res->IPv4Address[i].PrefixLength);
    }

    for (i = 0; i < ARRAY_SIZE(p_res->IPv6Address); i++)
    {
        if (p_res->IPv6Address[i].Address[0] == '\0')
        {
            continue;
        }
        
        offset += snprintf(p_buf+offset, mlen-offset, 
            "<tt:IPv6Address>"
                "<tt:Address>%s</tt:Address>"
                "<tt:PrefixLength>%d</tt:PrefixLength>"
            "</tt:IPv6Address>", 
            p_res->IPv6Address[i].Address,
            p_res->IPv6Address[i].PrefixLength);
    }
    
    return offset;
}

int build_GetIPAddressFilter_rly_xml(char * p_buf, int mlen, const char * argv)
{
    int offset = 0;
    
    offset += snprintf(p_buf+offset, mlen-offset, "<tds:GetIPAddressFilterResponse><tds:IPAddressFilter>");
    offset += build_IPAddressFilter_xml(p_buf+offset, mlen-offset, &g_onvif_cfg.ipaddr_filter);    
    offset += snprintf(p_buf+offset, mlen-offset, "</tds:IPAddressFilter></tds:GetIPAddressFilterResponse>");

	return offset;
}

int build_SetIPAddressFilter_rly_xml(char * p_buf, int mlen, const char * argv)
{
    int offset = 0;
    offset += snprintf(p_buf+offset, mlen-offset, "<tds:SetIPAddressFilterResponse />");
	return offset;
}

int build_AddIPAddressFilter_rly_xml(char * p_buf, int mlen, const char * argv)
{
    int offset = 0;
    offset += snprintf(p_buf+offset, mlen-offset, "<tds:AddIPAddressFilterResponse />");
	return offset;
}

int build_RemoveIPAddressFilter_rly_xml(char * p_buf, int mlen, const char * argv)
{
    int offset = 0;
    offset += snprintf(p_buf+offset, mlen-offset, "<tds:RemoveIPAddressFilterResponse />");
	return offset;
}

#endif // end of IPFILTER_SUPPORT

#ifdef AUDIO_SUPPORT

int build_AudioSourceConfigurationOptions_xml(char * p_buf, int mlen)
{
	int offset = 0;	
	ONVIF_AudioSource * p_a_src = g_onvif_cfg.a_src;
	
	while (p_a_src)
	{
		offset += snprintf(p_buf+offset, mlen-offset, 
    		"<tt:InputTokensAvailable>%s</tt:InputTokensAvailable>", 
    		p_a_src->AudioSource.token);
		
		p_a_src = p_a_src->next;
	}
	
	return offset;
}

int build_AddAudioSourceConfiguration_rly_xml(char * p_buf, int mlen, const char * argv)
{
	int offset = 0;
    offset += snprintf(p_buf+offset, mlen-offset, "<trt:AddAudioSourceConfigurationResponse />");
	return offset;
}

int build_RemoveAudioSourceConfiguration_rly_xml(char * p_buf, int mlen, const char * argv)
{
	int offset = 0;
    offset += snprintf(p_buf+offset, mlen-offset, "<trt:RemoveAudioSourceConfigurationResponse />");
	return offset;
}

int build_AddAudioEncoderConfiguration_rly_xml(char * p_buf, int mlen, const char * argv)
{
	int offset = 0;
	offset += snprintf(p_buf+offset, mlen-offset, "<trt:AddAudioEncoderConfigurationResponse />");
	return offset;
}

int build_RemoveAudioEncoderConfiguration_rly_xml(char * p_buf, int mlen, const char * argv)
{
	int offset = 0;
	offset += snprintf(p_buf+offset, mlen-offset, "<trt:RemoveAudioEncoderConfigurationResponse />");
	return offset;
}

int build_GetAudioSources_rly_xml(char * p_buf, int mlen, const char * argv)
{
    int offset = 0;
	ONVIF_AudioSource * p_a_src = g_onvif_cfg.a_src;
    
	offset += snprintf(p_buf+offset, mlen-offset, "<trt:GetAudioSourcesResponse>");
	
	while (p_a_src)
	{
	    offset += snprintf(p_buf+offset, mlen-offset, 
	    	"<trt:AudioSources token=\"%s\">"
	    		"<tt:Channels>%d</tt:Channels>"
	    	"</trt:AudioSources>", 
	    	p_a_src->AudioSource.token,
	    	p_a_src->AudioSource.Channels);
	    
	    p_a_src = p_a_src->next;
	}
	
	offset += snprintf(p_buf+offset, mlen-offset, "</trt:GetAudioSourcesResponse>");
	
	return offset;
}

int build_GetAudioEncoderConfigurations_rly_xml(char * p_buf, int mlen, const char * argv)
{
    int offset = 0;
    ONVIF_AudioEncoder2Configuration * p_a_enc_cfg = g_onvif_cfg.a_enc_cfg;
    
	offset += snprintf(p_buf+offset, mlen-offset, "<trt:GetAudioEncoderConfigurationsResponse>");

	while (p_a_enc_cfg)
	{
	    offset += snprintf(p_buf+offset, mlen-offset, "<trt:Configurations token=\"%s\">", p_a_enc_cfg->Configuration.token);
    	offset += build_AudioEncoderConfiguration_xml(p_buf+offset, mlen-offset, p_a_enc_cfg);
    	offset += snprintf(p_buf+offset, mlen-offset, "</trt:Configurations>");
    	
    	p_a_enc_cfg = p_a_enc_cfg->next;
	}
	
	offset += snprintf(p_buf+offset, mlen-offset, "</trt:GetAudioEncoderConfigurationsResponse>");

	return offset;
}

int build_GetAudioEncoderConfiguration_rly_xml(char * p_buf, int mlen, const char * token)
{
	int offset = 0;
	ONVIF_AudioEncoder2Configuration * p_a_enc_cfg = onvif_find_AudioEncoderConfiguration(token);
    if (NULL == p_a_enc_cfg)
    {
    	return ONVIF_ERR_NoConfig;
    }
	
	offset += snprintf(p_buf+offset, mlen-offset, "<trt:GetAudioEncoderConfigurationResponse>");

    offset += snprintf(p_buf+offset, mlen-offset, "<trt:Configuration token=\"%s\">", p_a_enc_cfg->Configuration.token);
	offset += build_AudioEncoderConfiguration_xml(p_buf+offset, mlen-offset, p_a_enc_cfg);
	offset += snprintf(p_buf+offset, mlen-offset, "</trt:Configuration>");
	
	offset += snprintf(p_buf+offset, mlen-offset, "</trt:GetAudioEncoderConfigurationResponse>");

	return offset;
}

int build_SetAudioEncoderConfiguration_rly_xml(char * p_buf, int mlen, const char * argv)
{
	int offset = 0;
	offset += snprintf(p_buf+offset, mlen-offset, "<trt:SetAudioEncoderConfigurationResponse />");		    
	return offset;
}

int build_GetAudioSourceConfigurations_rly_xml(char * p_buf, int mlen, const char * argv)
{
    int offset = 0;
    ONVIF_AudioSourceConfiguration * p_a_src_cfg = g_onvif_cfg.a_src_cfg;
    
	offset += snprintf(p_buf+offset, mlen-offset, "<trt:GetAudioSourceConfigurationsResponse>");

	while (p_a_src_cfg)
	{
	    offset += snprintf(p_buf+offset, mlen-offset, "<trt:Configurations token=\"%s\">", p_a_src_cfg->Configuration.token);
	    offset += build_AudioSourceConfiguration_xml(p_buf+offset, mlen-offset, &p_a_src_cfg->Configuration);
	    offset += snprintf(p_buf+offset, mlen-offset, "</trt:Configurations>");
	    
	    p_a_src_cfg = p_a_src_cfg->next;
	}
	
	offset += snprintf(p_buf+offset, mlen-offset, "</trt:GetAudioSourceConfigurationsResponse>");
	
	return offset;
}

int build_GetCompatibleAudioSourceConfigurations_rly_xml(char * p_buf, int mlen, const char * argv)
{
	int offset = 0;
	ONVIF_AudioSourceConfiguration * p_a_src_cfg;
	ONVIF_PROFILE * p_profile = onvif_find_profile(argv);
	if (NULL == p_profile)
	{
		return ONVIF_ERR_NoProfile;
	}

	p_a_src_cfg = g_onvif_cfg.a_src_cfg;
    
	offset += snprintf(p_buf+offset, mlen-offset, "<trt:GetCompatibleAudioSourceConfigurationsResponse>");

	while (p_a_src_cfg)
	{
	    offset += snprintf(p_buf+offset, mlen-offset, "<trt:Configurations token=\"%s\">", p_a_src_cfg->Configuration.token);
	    offset += build_AudioSourceConfiguration_xml(p_buf+offset, mlen-offset, &p_a_src_cfg->Configuration);
	    offset += snprintf(p_buf+offset, mlen-offset, "</trt:Configurations>");
	    
	    p_a_src_cfg = p_a_src_cfg->next;
	}
	
	offset += snprintf(p_buf+offset, mlen-offset, "</trt:GetCompatibleAudioSourceConfigurationsResponse>");
	
	return offset;
}

int build_GetAudioSourceConfigurationOptions_rly_xml(char * p_buf, int mlen, const char * argv)
{
	int offset = 0;
	ONVIF_AudioSourceConfiguration * p_a_src_cfg = NULL;
	
	GetAudioSourceConfigurationOptions_REQ * p_req = (GetAudioSourceConfigurationOptions_REQ *)argv;
	if (p_req->ProfileTokenFlag && p_req->ProfileToken[0] != '\0')
	{
		ONVIF_PROFILE * p_profile = onvif_find_profile(p_req->ProfileToken);
		if (NULL == p_profile)
		{
			return ONVIF_ERR_NoProfile;
		}

		p_a_src_cfg = p_profile->a_src_cfg;
	}

	if (p_req->ConfigurationTokenFlag && p_req->ConfigurationToken[0] != '\0')
	{
		p_a_src_cfg = onvif_find_AudioSourceConfiguration(p_req->ConfigurationToken);
		if (NULL == p_a_src_cfg)
		{
			return ONVIF_ERR_NoConfig;
		}
	}
	
	offset += snprintf(p_buf+offset, mlen-offset, "<trt:GetAudioSourceConfigurationOptionsResponse>");
	offset += snprintf(p_buf+offset, mlen-offset, "<trt:Options>");

	offset += build_AudioSourceConfigurationOptions_xml(p_buf+offset, mlen-offset);

    offset += snprintf(p_buf+offset, mlen-offset, "</trt:Options>");
	offset += snprintf(p_buf+offset, mlen-offset, "</trt:GetAudioSourceConfigurationOptionsResponse>");
	
	return offset;
}

int build_GetAudioEncoderConfigurationOptions_rly_xml(char * p_buf, int mlen, const char * argv)
{    
	int offset = 0;
	GetAudioEncoderConfigurationOptions_REQ * p_req = (GetAudioEncoderConfigurationOptions_REQ *) argv;
    ONVIF_AudioEncoder2ConfigurationOptions * p_option;

	if (p_req->ProfileTokenFlag && p_req->ProfileToken[0] != '\0')
	{
		ONVIF_PROFILE * p_profile = onvif_find_profile(p_req->ProfileToken);
		if (NULL == p_profile)
		{
			return ONVIF_ERR_NoProfile;
		}
	}

	if (p_req->ConfigurationTokenFlag && p_req->ConfigurationToken[0] != '\0')
	{
		ONVIF_AudioEncoder2Configuration * p_a_enc_cfg = onvif_find_AudioEncoderConfiguration(p_req->ConfigurationToken);
		if (NULL == p_a_enc_cfg)
		{
			return ONVIF_ERR_NoConfig;
		}
	}

	p_option = g_onvif_cfg.a_enc_cfg_opt;
	
	offset += snprintf(p_buf+offset, mlen-offset, "<trt:GetAudioEncoderConfigurationOptionsResponse><trt:Options>");

	while (p_option)
	{
		offset += snprintf(p_buf+offset, mlen-offset, "<tt:Options>");		
		offset += snprintf(p_buf+offset, mlen-offset, 
			"<tt:Encoding>%s</tt:Encoding>", 
			onvif_AudioEncodingToString(p_option->Options.AudioEncoding));
			
		offset += snprintf(p_buf+offset, mlen-offset, "<tt:BitrateList>");
		offset += build_IntList_xml(p_buf+offset, mlen-offset, &p_option->Options.BitrateList);
		offset += snprintf(p_buf+offset, mlen-offset, "</tt:BitrateList>");
		
		offset += snprintf(p_buf+offset, mlen-offset, "<tt:SampleRateList>");
		offset += build_IntList_xml(p_buf+offset, mlen-offset, &p_option->Options.SampleRateList);
		offset += snprintf(p_buf+offset, mlen-offset, "</tt:SampleRateList>");
		offset += snprintf(p_buf+offset, mlen-offset, "</tt:Options>");

		p_option = p_option->next;
	}
	
	offset += snprintf(p_buf+offset, mlen-offset, "</trt:Options></trt:GetAudioEncoderConfigurationOptionsResponse>");
	
	return offset;
}

int build_GetAudioSourceConfiguration_rly_xml(char * p_buf, int mlen, const char * token)
{
	int offset = 0;
    ONVIF_AudioSourceConfiguration * p_a_src_cfg = onvif_find_AudioSourceConfiguration(token);
    if (NULL == p_a_src_cfg)
    {
    	return ONVIF_ERR_NoConfig;
    }
	
	offset += snprintf(p_buf+offset, mlen-offset, "<trt:GetAudioSourceConfigurationResponse>");

    offset += snprintf(p_buf+offset, mlen-offset, "<trt:Configuration token=\"%s\">", p_a_src_cfg->Configuration.token);
    offset += build_AudioSourceConfiguration_xml(p_buf+offset, mlen-offset, &p_a_src_cfg->Configuration);
    offset += snprintf(p_buf+offset, mlen-offset, "</trt:Configuration>");	    
	
	offset += snprintf(p_buf+offset, mlen-offset, "</trt:GetAudioSourceConfigurationResponse>");
	
	return offset;
}

int build_SetAudioSourceConfiguration_rly_xml(char * p_buf, int mlen, const char * argv)
{
	int offset = 0;
	offset += snprintf(p_buf+offset, mlen-offset, "<trt:SetAudioSourceConfigurationResponse />");
	return offset;
}

int build_GetCompatibleAudioEncoderConfigurations_rly_xml(char * p_buf, int mlen, const char * argv)
{
	int offset = 0;
	ONVIF_AudioEncoder2Configuration * p_a_enc_cfg;
	ONVIF_PROFILE * p_profile = onvif_find_profile(argv);
	if (NULL == p_profile)
	{
		return ONVIF_ERR_NoProfile;
	}

	p_a_enc_cfg = g_onvif_cfg.a_enc_cfg;
	
	offset += snprintf(p_buf+offset, mlen-offset, "<trt:GetCompatibleAudioEncoderConfigurationsResponse>");

	while (p_a_enc_cfg)
	{
	    offset += snprintf(p_buf+offset, mlen-offset, "<trt:Configurations token=\"%s\">", p_a_enc_cfg->Configuration.token);
    	offset += build_AudioEncoderConfiguration_xml(p_buf+offset, mlen-offset, p_a_enc_cfg);
    	offset += snprintf(p_buf+offset, mlen-offset, "</trt:Configurations>");
    	
    	p_a_enc_cfg = p_a_enc_cfg->next;
	}
	
	offset += snprintf(p_buf+offset, mlen-offset, "</trt:GetCompatibleAudioEncoderConfigurationsResponse>");

	return offset;
}

int build_AddAudioDecoderConfiguration_rly_xml(char * p_buf, int mlen, const char * argv)
{
    int offset = 0;
	offset += snprintf(p_buf+offset, mlen-offset, "<trt:AddAudioDecoderConfigurationResponse />");
	return offset;
}

int build_AudioDecoderConfiguration_xml(char * p_buf, int mlen, onvif_AudioDecoderConfiguration * p_req)
{
    int offset = 0;

    offset += snprintf(p_buf+offset, mlen-offset, 
        "<tt:Name>%s</tt:Name>"
        "<tt:UseCount>%d</tt:UseCount>",
        p_req->Name,
        p_req->UseCount);

    return offset;        
}

int build_GetAudioDecoderConfigurations_rly_xml(char * p_buf, int mlen, const char * argv)
{
    int offset = 0;
	ONVIF_AudioDecoderConfiguration * p_a_dec_cfg;

	p_a_dec_cfg = g_onvif_cfg.a_dec_cfg;
    
	offset += snprintf(p_buf+offset, mlen-offset, "<trt:GetAudioDecoderConfigurationsResponse>");

	while (p_a_dec_cfg)
	{
	    offset += snprintf(p_buf+offset, mlen-offset, "<trt:Configurations token=\"%s\">", p_a_dec_cfg->Configuration.token);
    	offset += build_AudioDecoderConfiguration_xml(p_buf+offset, mlen-offset, &p_a_dec_cfg->Configuration);
    	offset += snprintf(p_buf+offset, mlen-offset, "</trt:Configurations>");
    	
    	p_a_dec_cfg = p_a_dec_cfg->next;
	}
	
	offset += snprintf(p_buf+offset, mlen-offset, "</trt:GetAudioDecoderConfigurationsResponse>");

	return offset;
}

int build_GetAudioDecoderConfiguration_rly_xml(char * p_buf, int mlen, const char * argv)
{
    int offset = 0;
	ONVIF_AudioDecoderConfiguration * p_a_dec_cfg;

	p_a_dec_cfg = onvif_find_AudioDecoderConfiguration(argv);
	if (NULL == p_a_dec_cfg)
	{
	    return ONVIF_ERR_InvalidToken;
	}
	
	offset += snprintf(p_buf+offset, mlen-offset, "<trt:GetAudioDecoderConfigurationResponse>");

    offset += snprintf(p_buf+offset, mlen-offset, "<trt:Configuration token=\"%s\">", p_a_dec_cfg->Configuration.token);
	offset += build_AudioDecoderConfiguration_xml(p_buf+offset, mlen-offset, &p_a_dec_cfg->Configuration);
	offset += snprintf(p_buf+offset, mlen-offset, "</trt:Configuration>");
	
	p_a_dec_cfg = p_a_dec_cfg->next;
	
	offset += snprintf(p_buf+offset, mlen-offset, "</trt:GetAudioDecoderConfigurationResponse>");

	return offset;
}

int build_RemoveAudioDecoderConfiguration_rly_xml(char * p_buf, int mlen, const char * argv)
{
    int offset = 0;
	offset += snprintf(p_buf+offset, mlen-offset, "<trt:RemoveAudioDecoderConfigurationResponse />");
	return offset;
}

int build_SetAudioDecoderConfiguration_rly_xml(char * p_buf, int mlen, const char * argv)
{
    int offset = 0;
	offset += snprintf(p_buf+offset, mlen-offset, "<trt:SetAudioDecoderConfigurationResponse />");
	return offset;
}

int build_GetAudioDecoderConfigurationOptions_rly_xml(char * p_buf, int mlen, const char * argv)
{
    int offset = 0;
    GetAudioDecoderConfigurationOptions_REQ * p_req = (GetAudioDecoderConfigurationOptions_REQ *)argv;

    if (p_req->ProfileTokenFlag && p_req->ProfileToken[0] != '\0')
	{
		ONVIF_PROFILE * p_profile = onvif_find_profile(p_req->ProfileToken);
		if (NULL == p_profile)
		{
			return ONVIF_ERR_NoProfile;
		}
	}

	if (p_req->ConfigurationTokenFlag && p_req->ConfigurationToken[0] != '\0')
	{
		ONVIF_AudioDecoderConfiguration * p_a_dec_cfg = onvif_find_AudioDecoderConfiguration(p_req->ConfigurationToken);
		if (NULL == p_a_dec_cfg)
		{
			return ONVIF_ERR_NoConfig;
		}
	}
	
	offset += snprintf(p_buf+offset, mlen-offset, "<trt:GetAudioDecoderConfigurationOptionsResponse><trt:Options>");
	
	if (g_onvif_cfg.a_dec_cfg_opt.AACDecOptionsFlag)
	{
	    offset += snprintf(p_buf+offset, mlen-offset, "<tt:AACDecOptions>");
	    offset += snprintf(p_buf+offset, mlen-offset, "<tt:Bitrate>");
	    offset += build_IntList_xml(p_buf+offset, mlen-offset, &g_onvif_cfg.a_dec_cfg_opt.AACDecOptions.Bitrate);
	    offset += snprintf(p_buf+offset, mlen-offset, "</tt:Bitrate>");
	    offset += snprintf(p_buf+offset, mlen-offset, "<tt:SampleRateRange>");
	    offset += build_IntList_xml(p_buf+offset, mlen-offset, &g_onvif_cfg.a_dec_cfg_opt.AACDecOptions.SampleRateRange);
	    offset += snprintf(p_buf+offset, mlen-offset, "</tt:SampleRateRange>");
	    offset += snprintf(p_buf+offset, mlen-offset, "</tt:AACDecOptions>");
	}
	if (g_onvif_cfg.a_dec_cfg_opt.G711DecOptionsFlag)
	{
	    offset += snprintf(p_buf+offset, mlen-offset, "<tt:G711DecOptions>");
	    offset += snprintf(p_buf+offset, mlen-offset, "<tt:Bitrate>");
	    offset += build_IntList_xml(p_buf+offset, mlen-offset, &g_onvif_cfg.a_dec_cfg_opt.G711DecOptions.Bitrate);
	    offset += snprintf(p_buf+offset, mlen-offset, "</tt:Bitrate>");
	    offset += snprintf(p_buf+offset, mlen-offset, "<tt:SampleRateRange>");
	    offset += build_IntList_xml(p_buf+offset, mlen-offset, &g_onvif_cfg.a_dec_cfg_opt.G711DecOptions.SampleRateRange);
	    offset += snprintf(p_buf+offset, mlen-offset, "</tt:SampleRateRange>");
	    offset += snprintf(p_buf+offset, mlen-offset, "</tt:G711DecOptions>");
	}
	if (g_onvif_cfg.a_dec_cfg_opt.G726DecOptionsFlag)
	{
	    offset += snprintf(p_buf+offset, mlen-offset, "<tt:G726DecOptions>");
	    offset += snprintf(p_buf+offset, mlen-offset, "<tt:Bitrate>");
	    offset += build_IntList_xml(p_buf+offset, mlen-offset, &g_onvif_cfg.a_dec_cfg_opt.G726DecOptions.Bitrate);
	    offset += snprintf(p_buf+offset, mlen-offset, "</tt:Bitrate>");
	    offset += snprintf(p_buf+offset, mlen-offset, "<tt:SampleRateRange>");
	    offset += build_IntList_xml(p_buf+offset, mlen-offset, &g_onvif_cfg.a_dec_cfg_opt.G726DecOptions.SampleRateRange);
	    offset += snprintf(p_buf+offset, mlen-offset, "</tt:SampleRateRange>");
	    offset += snprintf(p_buf+offset, mlen-offset, "</tt:G726DecOptions>");
	}	
	
	offset += snprintf(p_buf+offset, mlen-offset, "</trt:Options></trt:GetAudioDecoderConfigurationOptionsResponse>");
	
	return offset;
}

int build_GetCompatibleAudioDecoderConfigurations_rly_xml(char * p_buf, int mlen, const char * argv)
{
    int offset = 0;
	ONVIF_AudioDecoderConfiguration * p_a_dec_cfg;
	ONVIF_PROFILE * p_profile = onvif_find_profile(argv);
	if (NULL == p_profile)
	{
		return ONVIF_ERR_NoProfile;
	}

	p_a_dec_cfg = g_onvif_cfg.a_dec_cfg;
    
	offset += snprintf(p_buf+offset, mlen-offset, "<trt:GetCompatibleAudioDecoderConfigurationsResponse>");

	while (p_a_dec_cfg)
	{
	    offset += snprintf(p_buf+offset, mlen-offset, "<trt:Configurations token=\"%s\">", p_a_dec_cfg->Configuration.token);
    	offset += build_AudioDecoderConfiguration_xml(p_buf+offset, mlen-offset, &p_a_dec_cfg->Configuration);
    	offset += snprintf(p_buf+offset, mlen-offset, "</trt:Configurations>");
    	
    	p_a_dec_cfg = p_a_dec_cfg->next;
	}
	
	offset += snprintf(p_buf+offset, mlen-offset, "</trt:GetCompatibleAudioDecoderConfigurationsResponse>");

	return offset;

}

#endif // end of AUDIO_SUPPORT

#ifdef PTZ_SUPPORT

int builid_PTZSpaces_xml(char * p_buf, int mlen, ONVIF_PTZNode * p_node)
{
    int offset = 0;
    
    if (p_node->PTZNode.SupportedPTZSpaces.AbsolutePanTiltPositionSpaceFlag)
	{
		offset += snprintf(p_buf+offset, mlen-offset, 
			"<tt:AbsolutePanTiltPositionSpace>"
				"<tt:URI>http://www.onvif.org/ver10/tptz/PanTiltSpaces/PositionGenericSpace</tt:URI>"
				"<tt:XRange>"
					"<tt:Min>%0.1f</tt:Min>"
					"<tt:Max>%0.1f</tt:Max>"
				"</tt:XRange>"
				"<tt:YRange>"
					"<tt:Min>%0.1f</tt:Min>"
					"<tt:Max>%0.1f</tt:Max>"
				"</tt:YRange>"
			"</tt:AbsolutePanTiltPositionSpace>", 
			p_node->PTZNode.SupportedPTZSpaces.AbsolutePanTiltPositionSpace.XRange.Min,
			p_node->PTZNode.SupportedPTZSpaces.AbsolutePanTiltPositionSpace.XRange.Max,
			p_node->PTZNode.SupportedPTZSpaces.AbsolutePanTiltPositionSpace.YRange.Min,
			p_node->PTZNode.SupportedPTZSpaces.AbsolutePanTiltPositionSpace.YRange.Max);
	}

	if (p_node->PTZNode.SupportedPTZSpaces.AbsoluteZoomPositionSpaceFlag)
	{
		offset += snprintf(p_buf+offset, mlen-offset, 
			"<tt:AbsoluteZoomPositionSpace>"
				"<tt:URI>http://www.onvif.org/ver10/tptz/ZoomSpaces/PositionGenericSpace</tt:URI>"
				"<tt:XRange>"
					"<tt:Min>%0.1f</tt:Min>"
					"<tt:Max>%0.1f</tt:Max>"
				"</tt:XRange>"
			"</tt:AbsoluteZoomPositionSpace>", 
			p_node->PTZNode.SupportedPTZSpaces.AbsoluteZoomPositionSpace.XRange.Min,
			p_node->PTZNode.SupportedPTZSpaces.AbsoluteZoomPositionSpace.XRange.Max);
    }

    if (p_node->PTZNode.SupportedPTZSpaces.RelativePanTiltTranslationSpaceFlag)
    {
		offset += snprintf(p_buf+offset, mlen-offset, 
			"<tt:RelativePanTiltTranslationSpace>"
				"<tt:URI>http://www.onvif.org/ver10/tptz/PanTiltSpaces/TranslationGenericSpace</tt:URI>"
				"<tt:XRange>"
					"<tt:Min>%0.1f</tt:Min>"
					"<tt:Max>%0.1f</tt:Max>"
				"</tt:XRange>"
				"<tt:YRange>"
					"<tt:Min>%0.1f</tt:Min>"
					"<tt:Max>%0.1f</tt:Max>"
				"</tt:YRange>"
			"</tt:RelativePanTiltTranslationSpace>",
			p_node->PTZNode.SupportedPTZSpaces.RelativePanTiltTranslationSpace.XRange.Min,
			p_node->PTZNode.SupportedPTZSpaces.RelativePanTiltTranslationSpace.XRange.Max,
			p_node->PTZNode.SupportedPTZSpaces.RelativePanTiltTranslationSpace.YRange.Min,
			p_node->PTZNode.SupportedPTZSpaces.RelativePanTiltTranslationSpace.YRange.Max);
    }

    if (p_node->PTZNode.SupportedPTZSpaces.RelativeZoomTranslationSpaceFlag)
    {
		offset += snprintf(p_buf+offset, mlen-offset, 			
			"<tt:RelativeZoomTranslationSpace>"
				"<tt:URI>http://www.onvif.org/ver10/tptz/ZoomSpaces/TranslationGenericSpace</tt:URI>"
				"<tt:XRange>"
					"<tt:Min>%0.1f</tt:Min>"
					"<tt:Max>%0.1f</tt:Max>"
				"</tt:XRange>"
			"</tt:RelativeZoomTranslationSpace>", 
			p_node->PTZNode.SupportedPTZSpaces.RelativeZoomTranslationSpace.XRange.Min,
			p_node->PTZNode.SupportedPTZSpaces.RelativeZoomTranslationSpace.XRange.Max);	
	}

	if (p_node->PTZNode.SupportedPTZSpaces.ContinuousPanTiltVelocitySpaceFlag)
	{
		offset += snprintf(p_buf+offset, mlen-offset, 	
			"<tt:ContinuousPanTiltVelocitySpace>"
				"<tt:URI>http://www.onvif.org/ver10/tptz/PanTiltSpaces/VelocityGenericSpace</tt:URI>"
				"<tt:XRange>"
					"<tt:Min>%0.1f</tt:Min>"
					"<tt:Max>%0.1f</tt:Max>"
				"</tt:XRange>"
				"<tt:YRange>"
					"<tt:Min>%0.1f</tt:Min>"
					"<tt:Max>%0.1f</tt:Max>"
				"</tt:YRange>"
			"</tt:ContinuousPanTiltVelocitySpace>",
			p_node->PTZNode.SupportedPTZSpaces.ContinuousPanTiltVelocitySpace.XRange.Min,
			p_node->PTZNode.SupportedPTZSpaces.ContinuousPanTiltVelocitySpace.XRange.Max,
			p_node->PTZNode.SupportedPTZSpaces.ContinuousPanTiltVelocitySpace.YRange.Min,
			p_node->PTZNode.SupportedPTZSpaces.ContinuousPanTiltVelocitySpace.YRange.Max);	
	}

	if (p_node->PTZNode.SupportedPTZSpaces.ContinuousZoomVelocitySpaceFlag)
	{
		offset += snprintf(p_buf+offset, mlen-offset, 	
			"<tt:ContinuousZoomVelocitySpace>"
				"<tt:URI>http://www.onvif.org/ver10/tptz/ZoomSpaces/VelocityGenericSpace</tt:URI>"
				"<tt:XRange>"
					"<tt:Min>%0.1f</tt:Min>"
					"<tt:Max>%0.1f</tt:Max>"
				"</tt:XRange>"
			"</tt:ContinuousZoomVelocitySpace>",
			p_node->PTZNode.SupportedPTZSpaces.ContinuousZoomVelocitySpace.XRange.Min,
			p_node->PTZNode.SupportedPTZSpaces.ContinuousZoomVelocitySpace.XRange.Max);	
	}

	if (p_node->PTZNode.SupportedPTZSpaces.PanTiltSpeedSpaceFlag)
	{
		offset += snprintf(p_buf+offset, mlen-offset, 	
			"<tt:PanTiltSpeedSpace>"
				"<tt:URI>http://www.onvif.org/ver10/tptz/PanTiltSpaces/GenericSpeedSpace</tt:URI>"
				"<tt:XRange>"
					"<tt:Min>%0.1f</tt:Min>"
					"<tt:Max>%0.1f</tt:Max>"
				"</tt:XRange>"
			"</tt:PanTiltSpeedSpace>",  
			p_node->PTZNode.SupportedPTZSpaces.PanTiltSpeedSpace.XRange.Min,
			p_node->PTZNode.SupportedPTZSpaces.PanTiltSpeedSpace.XRange.Max);	
	}

	if (p_node->PTZNode.SupportedPTZSpaces.ZoomSpeedSpaceFlag)
	{
		offset += snprintf(p_buf+offset, mlen-offset, 			
			"<tt:ZoomSpeedSpace>"
				"<tt:URI>http://www.onvif.org/ver10/tptz/ZoomSpaces/ZoomGenericSpeedSpace</tt:URI>"
				"<tt:XRange>"
					"<tt:Min>%0.1f</tt:Min>"
					"<tt:Max>%0.1f</tt:Max>"
				"</tt:XRange>"
			"</tt:ZoomSpeedSpace>",
			p_node->PTZNode.SupportedPTZSpaces.ZoomSpeedSpace.XRange.Min,
			p_node->PTZNode.SupportedPTZSpaces.ZoomSpeedSpace.XRange.Max);
	}

	return offset;
}

int build_PTZNode_xml(char * p_buf, int mlen, ONVIF_PTZNode * p_node)
{
	int offset = 0;
	
	offset += snprintf(p_buf+offset, mlen-offset, 
	    "<tptz:PTZNode token=\"%s\" FixedHomePosition=\"%s\">",
		p_node->PTZNode.token, 
		p_node->PTZNode.FixedHomePosition ? "true" : "false");
	offset += snprintf(p_buf+offset, mlen-offset, "<tt:Name>%s</tt:Name>", p_node->PTZNode.Name);
	offset += snprintf(p_buf+offset, mlen-offset, "<tt:SupportedPTZSpaces>");

	offset += builid_PTZSpaces_xml(p_buf+offset, mlen-offset, p_node);
	
	offset += snprintf(p_buf+offset, mlen-offset, "</tt:SupportedPTZSpaces>");
	offset += snprintf(p_buf+offset, mlen-offset, 
	    "<tt:MaximumNumberOfPresets>%d</tt:MaximumNumberOfPresets>", 
	    p_node->PTZNode.MaximumNumberOfPresets);
    offset += snprintf(p_buf+offset, mlen-offset, 
        "<tt:HomeSupported>%s</tt:HomeSupported>",
        p_node->PTZNode.HomeSupported ? "true" : "false");
    offset += snprintf(p_buf+offset, mlen-offset, "</tptz:PTZNode>"); 

    return offset;
}

int build_GetNodes_rly_xml(char * p_buf, int mlen, const char * argv)
{
    int offset = 0;
	ONVIF_PTZNode * p_node = g_onvif_cfg.ptz_node;
	
	offset += snprintf(p_buf+offset, mlen-offset, "<tptz:GetNodesResponse>");

	while (p_node)
	{
		offset += build_PTZNode_xml(p_buf+offset, mlen-offset, p_node);

    	p_node = p_node->next;
    }
    
	offset += snprintf(p_buf+offset, mlen-offset, "</tptz:GetNodesResponse>");	

	return offset;
}


int build_GetNode_rly_xml(char * p_buf, int mlen, const char * argv)
{
	int offset = 0;
	ONVIF_PTZNode * p_node = onvif_find_PTZNode(argv);
	if (NULL == p_node)
	{
		return ONVIF_ERR_NoEntity;
	}

	offset += snprintf(p_buf+offset, mlen-offset, "<tptz:GetNodeResponse>");

	offset += build_PTZNode_xml(p_buf+offset, mlen-offset, p_node);
	   
	offset += snprintf(p_buf+offset, mlen-offset, "</tptz:GetNodeResponse>");	

	return offset;
}

int build_GetConfigurations_rly_xml(char * p_buf, int mlen, const char * argv)
{
    int offset = 0;
	ONVIF_PTZConfiguration * p_ptz_cfg = g_onvif_cfg.ptz_cfg;

	offset += snprintf(p_buf+offset, mlen-offset, "<tptz:GetConfigurationsResponse>");

	while (p_ptz_cfg)
	{
		offset += snprintf(p_buf+offset, mlen-offset, 
            "<tptz:PTZConfiguration token=\"%s\" MoveRamp=\"%d\" PresetRamp=\"%d\" PresetTourRamp=\"%d\">", 
            p_ptz_cfg->Configuration.token, p_ptz_cfg->Configuration.MoveRamp,
            p_ptz_cfg->Configuration.PresetRamp, p_ptz_cfg->Configuration.PresetTourRamp);
    	offset += build_PTZConfiguration_xml(p_buf+offset, mlen-offset, p_ptz_cfg);
    	offset += snprintf(p_buf+offset, mlen-offset, "</tptz:PTZConfiguration>");

		p_ptz_cfg = p_ptz_cfg->next;
	}	
	
	offset += snprintf(p_buf+offset, mlen-offset, "</tptz:GetConfigurationsResponse>");	

	return offset;
}

int build_GetCompatibleConfigurations_rly_xml(char * p_buf, int mlen, const char * argv)
{
    int offset = 0;
    GetCompatibleConfigurations_REQ * p_req = (GetCompatibleConfigurations_REQ *)argv;
    ONVIF_PTZConfiguration * p_ptz_cfg = g_onvif_cfg.ptz_cfg;
    ONVIF_PROFILE * p_profile = onvif_find_profile(p_req->ProfileToken);

	offset += snprintf(p_buf+offset, mlen-offset, "<tptz:GetCompatibleConfigurationsResponse>");

	while (p_ptz_cfg)
	{
		offset += snprintf(p_buf+offset, mlen-offset, 
            "<tptz:PTZConfiguration token=\"%s\" MoveRamp=\"%d\" PresetRamp=\"%d\" PresetTourRamp=\"%d\">", 
            p_ptz_cfg->Configuration.token, p_ptz_cfg->Configuration.MoveRamp,
            p_ptz_cfg->Configuration.PresetRamp, p_ptz_cfg->Configuration.PresetTourRamp);
    	offset += build_PTZConfiguration_xml(p_buf+offset, mlen-offset, p_ptz_cfg);
    	offset += snprintf(p_buf+offset, mlen-offset, "</tptz:PTZConfiguration>");

		p_ptz_cfg = p_ptz_cfg->next;
	}	
	
	offset += snprintf(p_buf+offset, mlen-offset, "</tptz:GetCompatibleConfigurationsResponse>");	

	return offset;   
}

int build_GetConfiguration_rly_xml(char * p_buf, int mlen, const char * argv)
{
	int offset = 0;
	ONVIF_PTZConfiguration * p_ptz_cfg = onvif_find_PTZConfiguration(argv);
	if (NULL == p_ptz_cfg)
	{
		return ONVIF_ERR_NoConfig;
	}

	offset += snprintf(p_buf+offset, mlen-offset, "<tptz:GetConfigurationResponse>");

	offset += snprintf(p_buf+offset, mlen-offset, 
            "<tptz:PTZConfiguration token=\"%s\" MoveRamp=\"%d\" PresetRamp=\"%d\" PresetTourRamp=\"%d\">", 
            p_ptz_cfg->Configuration.token,
            p_ptz_cfg->Configuration.MoveRampFlag ? p_ptz_cfg->Configuration.MoveRamp : 0,
            p_ptz_cfg->Configuration.PresetRampFlag ? p_ptz_cfg->Configuration.PresetRamp : 0,
            p_ptz_cfg->Configuration.PresetTourRampFlag ? p_ptz_cfg->Configuration.PresetTourRamp : 0);
	offset += build_PTZConfiguration_xml(p_buf+offset, mlen-offset, p_ptz_cfg);
	offset += snprintf(p_buf+offset, mlen-offset, "</tptz:PTZConfiguration>");
   	
	offset += snprintf(p_buf+offset, mlen-offset, "</tptz:GetConfigurationResponse>");	

	return offset;
}


int build_AddPTZConfiguration_rly_xml(char * p_buf, int mlen, const char * argv)
{
	int offset = 0;
	offset += snprintf(p_buf+offset, mlen-offset, "<trt:AddPTZConfigurationResponse />");	
	return offset;
}

int build_RemovePTZConfiguration_rly_xml(char * p_buf, int mlen, const char * argv)
{
	int offset = 0;
	offset += snprintf(p_buf+offset, mlen-offset, "<trt:RemovePTZConfigurationResponse />");	
	return offset;
}

int build_GetConfigurationOptions_rly_xml(char * p_buf, int mlen, const char * argv)
{
	int offset = 0;
	ONVIF_PTZNode * p_node;
	ONVIF_PTZConfiguration * p_ptz_cfg = onvif_find_PTZConfiguration(argv);
	if (NULL == p_ptz_cfg)
	{
		return ONVIF_ERR_NoConfig;
	}

	p_node = onvif_find_PTZNode(p_ptz_cfg->Configuration.NodeToken);
	assert(p_node);
	
	offset += snprintf(p_buf+offset, mlen-offset, "<tptz:GetConfigurationOptionsResponse>");
	offset += snprintf(p_buf+offset, mlen-offset, "<tptz:PTZConfigurationOptions>");
	
	offset += snprintf(p_buf+offset, mlen-offset, "<tt:Spaces>");

	offset += builid_PTZSpaces_xml(p_buf+offset, mlen-offset, p_node);
	
	offset += snprintf(p_buf+offset, mlen-offset, "</tt:Spaces>");

	offset += snprintf(p_buf+offset, mlen-offset, 
		"<tt:PTZTimeout>"
			"<tt:Min>PT%dS</tt:Min>"
			"<tt:Max>PT%dS</tt:Max>"
		"</tt:PTZTimeout>", 
		g_onvif_cfg.PTZConfigurationOptions.PTZTimeout.Min, 
		g_onvif_cfg.PTZConfigurationOptions.PTZTimeout.Max);

	if (g_onvif_cfg.PTZConfigurationOptions.PTControlDirectionFlag)
	{
	    offset += snprintf(p_buf+offset, mlen-offset, "<tt:PTControlDirection>");
	    offset += snprintf(p_buf+offset, mlen-offset, "<tt:EFlip>");
	    if (g_onvif_cfg.PTZConfigurationOptions.PTControlDirection.EFlipMode_OFF)
	    {
	        offset += snprintf(p_buf+offset, mlen-offset, "<tt:Mode>OFF</tt:Mode>");
	    }
	    if (g_onvif_cfg.PTZConfigurationOptions.PTControlDirection.EFlipMode_ON)
	    {
	        offset += snprintf(p_buf+offset, mlen-offset, "<tt:Mode>ON</tt:Mode>");
	    }
	    if (g_onvif_cfg.PTZConfigurationOptions.PTControlDirection.EFlipMode_Extended)
	    {
	        offset += snprintf(p_buf+offset, mlen-offset, "<tt:Mode>Extended</tt:Mode>");
	    }
	    offset += snprintf(p_buf+offset, mlen-offset, "</tt:EFlip>");
	    offset += snprintf(p_buf+offset, mlen-offset, "<tt:Reverse>");
	    if (g_onvif_cfg.PTZConfigurationOptions.PTControlDirection.ReverseMode_OFF)
	    {
	        offset += snprintf(p_buf+offset, mlen-offset, "<tt:Mode>OFF</tt:Mode>");
	    }
	    if (g_onvif_cfg.PTZConfigurationOptions.PTControlDirection.ReverseMode_ON)
	    {
	        offset += snprintf(p_buf+offset, mlen-offset, "<tt:Mode>ON</tt:Mode>");
	    }
	    if (g_onvif_cfg.PTZConfigurationOptions.PTControlDirection.ReverseMode_AUTO)
	    {
	        offset += snprintf(p_buf+offset, mlen-offset, "<tt:Mode>AUTO</tt:Mode>");
	    }
	    if (g_onvif_cfg.PTZConfigurationOptions.PTControlDirection.ReverseMode_Extended)
	    {
	        offset += snprintf(p_buf+offset, mlen-offset, "<tt:Mode>Extended</tt:Mode>");
	    }
	    offset += snprintf(p_buf+offset, mlen-offset, "</tt:Reverse>");
	    offset += snprintf(p_buf+offset, mlen-offset, "</tt:PTControlDirection>");
	}
    
	offset += snprintf(p_buf+offset, mlen-offset, "</tptz:PTZConfigurationOptions>");	
	offset += snprintf(p_buf+offset, mlen-offset, "</tptz:GetConfigurationOptionsResponse>");	

	return offset;
}


int build_ptz_GetStatus_rly_xml(char * p_buf, int mlen, const char * argv)
{
	int offset = 0;
	onvif_PTZStatus ptz_status;
	ONVIF_PROFILE * p_profile = onvif_find_profile(argv);
	if (NULL == p_profile)
	{
		return ONVIF_ERR_NoProfile;
	}

	memset(&ptz_status, 0, sizeof(onvif_PTZStatus));

	if (ONVIF_OK != onvif_ptz_GetStatus(p_profile, &ptz_status))
	{
		return -1;
	}

	offset += snprintf(p_buf+offset, mlen-offset, "<tptz:GetStatusResponse>");
	offset += snprintf(p_buf+offset, mlen-offset, "<tptz:PTZStatus>");

	if (ptz_status.PositionFlag)
	{
		offset += snprintf(p_buf+offset, mlen-offset, "<tt:Position>");
		if (ptz_status.Position.PanTiltFlag)
		{
			offset += snprintf(p_buf+offset, mlen-offset, 
				"<tt:PanTilt x=\"%0.1f\" y=\"%0.1f\" space=\"http://www.onvif.org/ver10/tptz/PanTiltSpaces/PositionGenericSpace\" />",
				ptz_status.Position.PanTilt.x,
				ptz_status.Position.PanTilt.y);
		}	
		if (ptz_status.Position.ZoomFlag)
		{
			offset += snprintf(p_buf+offset, mlen-offset, 
				"<tt:Zoom x=\"%0.1f\" space=\"http://www.onvif.org/ver10/tptz/ZoomSpaces/PositionGenericSpace\" />",
				ptz_status.Position.Zoom.x);
		}		
		offset += snprintf(p_buf+offset, mlen-offset, "</tt:Position>");
	}

	if (ptz_status.MoveStatusFlag)
	{
		offset += snprintf(p_buf+offset, mlen-offset, "<tt:MoveStatus>");
		if (ptz_status.MoveStatus.PanTiltFlag)
		{
			offset += snprintf(p_buf+offset, mlen-offset, 
				"<tt:PanTilt>%s</tt:PanTilt>",
				onvif_MoveStatusToString(ptz_status.MoveStatus.PanTilt));
		}
		if (ptz_status.MoveStatus.ZoomFlag)
		{
			offset += snprintf(p_buf+offset, mlen-offset, 
				"<tt:Zoom>%s</tt:Zoom>",
				onvif_MoveStatusToString(ptz_status.MoveStatus.Zoom));
		}
		offset += snprintf(p_buf+offset, mlen-offset, "</tt:MoveStatus>");
	}
	
	if (ptz_status.ErrorFlag && strlen(ptz_status.Error) > 0)
	{
		offset += snprintf(p_buf+offset, mlen-offset, "<tt:Error>%s</tt:Error>", ptz_status.Error);
	}
    
	offset += snprintf(p_buf+offset, mlen-offset, "<tt:UtcTime>%s</tt:UtcTime>", 
	    onvif_format_datetime_str(ptz_status.UtcTime, 1, "%Y-%m-%dT%H:%M:%SZ"));
	
	offset += snprintf(p_buf+offset, mlen-offset, "</tptz:PTZStatus>");
	offset += snprintf(p_buf+offset, mlen-offset, "</tptz:GetStatusResponse>");
	
	return offset;
}


int build_ContinuousMove_rly_xml(char * p_buf, int mlen, const char * argv)
{
	int offset = 0;
	offset += snprintf(p_buf+offset, mlen-offset, "<tptz:ContinuousMoveResponse />");	
	return offset;
}

int build_ptz_Stop_rly_xml(char * p_buf, int mlen, const char * argv)
{
	int offset = 0;
	offset += snprintf(p_buf+offset, mlen-offset, "<tptz:StopResponse />");	
	return offset;
}

int build_AbsoluteMove_rly_xml(char * p_buf, int mlen, const char * argv)
{
    int offset = 0;
	offset += snprintf(p_buf+offset, mlen-offset, "<tptz:AbsoluteMoveResponse />");	
	return offset;
}

int build_RelativeMove_rly_xml(char * p_buf, int mlen, const char * argv)
{
    int offset = 0;
	offset += snprintf(p_buf+offset, mlen-offset, "<tptz:RelativeMoveResponse />");	
	return offset;
}

int build_SetPreset_rly_xml(char * p_buf, int mlen, const char * argv)
{
    int offset = 0;
	
	offset += snprintf(p_buf+offset, mlen-offset, 
	    "<tptz:SetPresetResponse>"
	    	"<tptz:PresetToken>%s</tptz:PresetToken>"
	    "</tptz:SetPresetResponse>", argv);	

	return offset;
}

int build_GetPresets_rly_xml(char * p_buf, int mlen, const char * argv)
{
	int i;
	int offset = 0;
    ONVIF_PROFILE * p_profile = onvif_find_profile(argv);  // g_onvif_cfg.profiles
    if (NULL == p_profile)
    {
        return ONVIF_ERR_NoProfile;
    }

	//// add by xieqingpu
	if (readPtzPreset(p_profile->presets, 128) != 0)		// 128:由于云台设备支持128个预置位
	{
		printf("read PTZ preset faile.\n");
	}
	/* else{
		for(int j = 0; j < 64; j++)
		{
			printf("xxx read1 p_profile->presets[%d].UsedFlag:%d\n",j, p_profile->presets[j].UsedFlag);
			printf("xxx read1 Preset[%d],token:%s Name:%s\n\n",j, p_profile->presets[j].PTZPreset.token, p_profile->presets[j].PTZPreset.Name);
		}
	} */
	////

	offset += snprintf(p_buf+offset, mlen-offset, "<tptz:GetPresetsResponse>");

	for (i = 0; i < ARRAY_SIZE(p_profile->presets); i++)
	{
	    if (p_profile->presets[i].UsedFlag == 0)
	    {
	        continue;
	    }
		/////
		/* printf("xxx read2 p_profile->presets[%d].UsedFlag:%d\n",i, p_profile->presets[i].UsedFlag);
		printf("xxx read2 Preset[%d],token:%s Name:%s\n",i, p_profile->presets[i].PTZPreset.token, p_profile->presets[i].PTZPreset.Name); */
	    
    	offset += snprintf(p_buf+offset, mlen-offset, 
    		"<tptz:Preset token=\"%s\">",
    		p_profile->presets[i].PTZPreset.token);  //token
    	offset += snprintf(p_buf+offset, mlen-offset, 
    		"<tt:Name>%s</tt:Name>", 
    		p_profile->presets[i].PTZPreset.Name);   //name
    		
		if (p_profile->presets[i].PTZPreset.PTZPositionFlag)
		{
			offset += snprintf(p_buf+offset, mlen-offset, "<tt:PTZPosition>");
			if (p_profile->presets[i].PTZPreset.PTZPosition.PanTiltFlag)
			{
		    	offset += snprintf(p_buf+offset, mlen-offset, 
		    		"<tt:PanTilt x=\"%0.1f\" y=\"%0.1f\" />",
		    		p_profile->presets[i].PTZPreset.PTZPosition.PanTilt.x,
		    		p_profile->presets[i].PTZPreset.PTZPosition.PanTilt.y);
		    }
		    if (p_profile->presets[i].PTZPreset.PTZPosition.ZoomFlag)
		    {
		    	offset += snprintf(p_buf+offset, mlen-offset, 
		    		"<tt:Zoom x=\"%0.1f\" />",
		    		p_profile->presets[i].PTZPreset.PTZPosition.Zoom.x);
		    }        
	        offset += snprintf(p_buf+offset, mlen-offset, "</tt:PTZPosition>");
	    }
	    
        offset += snprintf(p_buf+offset, mlen-offset, "</tptz:Preset>");
    }
    
	offset += snprintf(p_buf+offset, mlen-offset, "</tptz:GetPresetsResponse>");
	
	return offset;
}

int build_RemovePreset_rly_xml(char * p_buf, int mlen, const char * argv)
{
    int offset = 0;
	offset += snprintf(p_buf+offset, mlen-offset, "<tptz:RemovePresetResponse />");	
	return offset;
}

int build_GotoPreset_rly_xml(char * p_buf, int mlen, const char * argv)
{
    int offset = 0;
	offset += snprintf(p_buf+offset, mlen-offset, "<tptz:GotoPresetResponse />");	
	return offset;
}

int build_GotoHomePosition_rly_xml(char * p_buf, int mlen, const char * argv)
{
    int offset = 0;
	offset += snprintf(p_buf+offset, mlen-offset, "<tptz:GotoHomePositionResponse />");	
	return offset;
}

int build_SetHomePosition_rly_xml(char * p_buf, int mlen, const char * argv)
{
	int offset = 0;
	offset += snprintf(p_buf+offset, mlen-offset, "<tptz:SetHomePositionResponse />");	
	return offset;
}

#endif // end of PTZ_SUPPORT


#ifdef PROFILE_G_SUPPORT

int build_GetRecordingSummary_rly_xml(char * p_buf, int mlen, const char * argv)
{
	char DataFrom[64];
	char DataUntil[64];

	int offset = 0;
	GetRecordingSummary_RES * p_res = (GetRecordingSummary_RES *)argv;

	onvif_get_time_str_s(DataFrom, sizeof(DataFrom), p_res->Summary.DataFrom, 0);
	onvif_get_time_str_s(DataUntil, sizeof(DataUntil), p_res->Summary.DataUntil, 0);
	
	offset += snprintf(p_buf+offset, mlen-offset, 
		"<tse:GetRecordingSummaryResponse>"
			"<tse:Summary>"
				"<tt:DataFrom>%s</tt:DataFrom>"
				"<tt:DataUntil>%s</tt:DataUntil>"
				"<tt:NumberRecordings>%d</tt:NumberRecordings>"
			"</tse:Summary>"
		"</tse:GetRecordingSummaryResponse>",
		DataFrom,
		DataUntil,
		p_res->Summary.NumberRecordings);

	return offset;
}

int build_RecordingSourceInformation_xml(char * p_buf, int mlen, onvif_RecordingSourceInformation * p_req)
{
	int offset = 0;
	
	offset += snprintf(p_buf+offset, mlen-offset, 
		"<tt:Source>"
			"<tt:SourceId>%s</tt:SourceId>"
			"<tt:Name>%s</tt:Name>"
			"<tt:Location>%s</tt:Location>"
			"<tt:Description>%s</tt:Description>"
			"<tt:Address>%s</tt:Address>"
		"</tt:Source>",
		p_req->SourceId,
		p_req->Name,
		p_req->Location,
		p_req->Description,
		p_req->Address);
		
	return offset;		
}

int build_TrackInformation_xml(char * p_buf, int mlen, onvif_TrackInformation * p_req)
{
	int offset = 0;
	char DataFrom[64], DataTo[64];

	onvif_get_time_str_s(DataFrom, sizeof(DataFrom), p_req->DataFrom, 0);
	onvif_get_time_str_s(DataTo, sizeof(DataTo), p_req->DataTo, 0);

	offset += snprintf(p_buf+offset, mlen-offset, 
		"<tt:TrackToken>%s</tt:TrackToken>"
		"<tt:TrackType>%s</tt:TrackType>"
		"<tt:Description>%s</tt:Description>"
		"<tt:DataFrom>%s</tt:DataFrom>"
		"<tt:DataTo>%s</tt:DataTo>",
		p_req->TrackToken,
		onvif_TrackTypeToString(p_req->TrackType),
		p_req->Description,
		DataFrom,
		DataTo);

	return offset;			
}

int build_RecordingInformation_xml(char * p_buf, int mlen, onvif_RecordingInformation * p_req)
{
	int i;
	int offset = 0;
	
	offset += snprintf(p_buf+offset, mlen-offset, "<tt:RecordingToken>%s</tt:RecordingToken>", p_req->RecordingToken);
	offset += build_RecordingSourceInformation_xml(p_buf+offset, mlen-offset, &p_req->Source);

	if (p_req->EarliestRecordingFlag)
	{
		char EarliestRecording[64];
		onvif_get_time_str_s(EarliestRecording, sizeof(EarliestRecording), p_req->EarliestRecording, 0);
		offset += snprintf(p_buf+offset, mlen-offset, "<tt:EarliestRecording>%s</tt:EarliestRecording>", EarliestRecording);
	}

	if (p_req->LatestRecordingFlag)
	{
		char LatestRecording[64];
		onvif_get_time_str_s(LatestRecording, sizeof(LatestRecording), p_req->LatestRecording, 0);
		offset += snprintf(p_buf+offset, mlen-offset, "<tt:LatestRecording>%s</tt:LatestRecording>", LatestRecording);
	}

	offset += snprintf(p_buf+offset, mlen-offset, "<tt:Content>%s</tt:Content>", p_req->Content);

	for (i = 0; i < p_req->sizeTrack; i++)
	{
		offset += snprintf(p_buf+offset, mlen-offset, "<tt:Track>");
		offset += build_TrackInformation_xml(p_buf+offset, mlen-offset, &p_req->Track[i]);
		offset += snprintf(p_buf+offset, mlen-offset, "</tt:Track>");
	}

	offset += snprintf(p_buf+offset, mlen-offset, "<tt:RecordingStatus>%s</tt:RecordingStatus>", 
		onvif_RecordingStatusToString(p_req->RecordingStatus));

	return offset;		
}

int build_GetRecordingInformation_rly_xml(char * p_buf, int mlen, const char * argv)
{
    int offset = 0;
	GetRecordingInformation_RES * p_res = (GetRecordingInformation_RES *)argv;

	offset += snprintf(p_buf+offset, mlen-offset, "<tse:GetRecordingInformationResponse><tse:RecordingInformation>");	
	offset += build_RecordingInformation_xml(p_buf+offset, mlen-offset, &p_res->RecordingInformation);
	offset += snprintf(p_buf+offset, mlen-offset, "</tse:RecordingInformation></tse:GetRecordingInformationResponse>");

	return offset;
}

int build_TrackAttributes_xml(char * p_buf, int mlen, onvif_TrackAttributes * p_req)
{
	int offset = 0;

	offset += build_TrackInformation_xml(p_buf+offset, mlen-offset, &p_req->TrackInformation);

	if (p_req->VideoAttributesFlag)
	{
		offset += snprintf(p_buf+offset, mlen-offset, "<tt:VideoAttributes>");
		if (p_req->VideoAttributes.BitrateFlag)
		{
			offset += snprintf(p_buf+offset, mlen-offset, "<tt:Bitrate>%d</tt:Bitrate>", p_req->VideoAttributes.Bitrate);
		}
		offset += snprintf(p_buf+offset, mlen-offset, 
			"<tt:Width>%d</tt:Width>"
			"<tt:Height>%d</tt:Height>"
			"<tt:Encoding>%s</tt:Encoding>"
			"<tt:Framerate>%0.1f</tt:Framerate>",
			p_req->VideoAttributes.Width,
			p_req->VideoAttributes.Height,
			onvif_VideoEncodingToString(p_req->VideoAttributes.Encoding),
			p_req->VideoAttributes.Framerate);			
		offset += snprintf(p_buf+offset, mlen-offset, "</tt:VideoAttributes>");		
	}

	if (p_req->AudioAttributesFlag)
	{
		offset += snprintf(p_buf+offset, mlen-offset, "<tt:AudioAttributes>");
		if (p_req->AudioAttributes.BitrateFlag)
		{
			offset += snprintf(p_buf+offset, mlen-offset, "<tt:Bitrate>%d</tt:Bitrate>", p_req->AudioAttributes.Bitrate);
		}
		offset += snprintf(p_buf+offset, mlen-offset,
			"<tt:Encoding>%s</tt:Encoding>"
			"<tt:Samplerate>%d</tt:Samplerate>",
			onvif_AudioEncodingToString(p_req->AudioAttributes.Encoding),
			p_req->AudioAttributes.Samplerate);			
		offset += snprintf(p_buf+offset, mlen-offset, "</tt:AudioAttributes>");
	}

	if (p_req->MetadataAttributesFlag)
	{		
		if (p_req->MetadataAttributes.PtzSpacesFlag)
		{
			offset += snprintf(p_buf+offset, mlen-offset, "<tt:MetadataAttributes PtzSpaces=\"%s\">", p_req->MetadataAttributes.PtzSpaces);
		}
		else
		{
			offset += snprintf(p_buf+offset, mlen-offset, "<tt:MetadataAttributes>");
		}
		offset += snprintf(p_buf+offset, mlen-offset, 
			"<tt:CanContainPTZ>%s</tt:CanContainPTZ>"
			"<tt:CanContainAnalytics>%s</tt:CanContainAnalytics>"
			"<tt:CanContainNotifications>%s</tt:CanContainNotifications>",
			p_req->MetadataAttributes.CanContainPTZ ? "true" : "false",
			p_req->MetadataAttributes.CanContainAnalytics ? "true" : "false",
			p_req->MetadataAttributes.CanContainNotifications ? "true" : "false");
		offset += snprintf(p_buf+offset, mlen-offset, "</tt:MetadataAttributes>");
	}

	return offset;
}

int build_MediaAttributes_xml(char * p_buf, int mlen, onvif_MediaAttributes * p_req)
{
	int i, offset = 0;
	char From[64], Until[64];

	onvif_get_time_str_s(From, sizeof(From), p_req->From, 0);
	onvif_get_time_str_s(Until, sizeof(Until), p_req->Until, 0);
	
	offset += snprintf(p_buf+offset, mlen-offset, "<tt:RecordingToken>%s</tt:RecordingToken>", p_req->RecordingToken);

	for (i = 0; i < p_req->sizeTrackAttributes; i++)
	{
		offset += snprintf(p_buf+offset, mlen-offset, "<tt:TrackAttributes>");
		offset += build_TrackAttributes_xml(p_buf+offset, mlen-offset, &p_req->TrackAttributes[i]);
		offset += snprintf(p_buf+offset, mlen-offset, "</tt:TrackAttributes>");
	}
	
	offset += snprintf(p_buf+offset, mlen-offset, 
		"<tt:From>%s</tt:From>"
    	"<tt:Until>%s</tt:Until>", 
    	From,
    	Until);

	return offset;    	
}

int build_GetMediaAttributes_rly_xml(char * p_buf, int mlen, const char * argv)
{
	int i;
	int offset = 0;
	GetMediaAttributes_RES * p_res = (GetMediaAttributes_RES *)argv;

	offset += snprintf(p_buf+offset, mlen-offset, "<tse:GetMediaAttributesResponse>");

	for (i = 0; i < p_res->sizeMediaAttributes; i++)
	{
		offset += snprintf(p_buf+offset, mlen-offset, "<tse:MediaAttributes>");
		offset += build_MediaAttributes_xml(p_buf+offset, mlen-offset, &p_res->MediaAttributes[i]);
		offset += snprintf(p_buf+offset, mlen-offset, "</tse:MediaAttributes>");
	}
	
	offset += snprintf(p_buf+offset, mlen-offset, "</tse:GetMediaAttributesResponse>");
	
	return offset;
}

int build_FindRecordings_rly_xml(char * p_buf, int mlen, const char * argv)
{
    int offset = 0;
	FindRecordings_RES * p_res = (FindRecordings_RES *)argv;
	
	offset += snprintf(p_buf+offset, mlen-offset, 
		"<tse:FindRecordingsResponse>"
			"<tse:SearchToken>%s</tse:SearchToken>"
		"</tse:FindRecordingsResponse>",
		p_res->SearchToken);

	return offset;
}

int build_GetRecordingSearchResults_rly_xml(char * p_buf, int mlen, const char * argv)
{
    int offset = 0;
	ONVIF_RecordingInformation * p_RecInf;
	GetRecordingSearchResults_RES * p_res = (GetRecordingSearchResults_RES *)argv;
	
	offset += snprintf(p_buf+offset, mlen-offset, "<tse:GetRecordingSearchResultsResponse><tse:ResultList>");
	offset += snprintf(p_buf+offset, mlen-offset, "<tt:SearchState>%s</tt:SearchState>", onvif_SearchStateToString(p_res->ResultList.SearchState));

	p_RecInf = p_res->ResultList.RecordInformation;
	while (p_RecInf)
	{
		offset += snprintf(p_buf+offset, mlen-offset, "<tt:RecordingInformation>");
		offset += build_RecordingInformation_xml(p_buf+offset, mlen-offset, &p_RecInf->RecordingInformation);
		offset += snprintf(p_buf+offset, mlen-offset, "</tt:RecordingInformation>");

		p_RecInf = p_RecInf->next;
	}
	
	offset += snprintf(p_buf+offset, mlen-offset, "</tse:ResultList></tse:GetRecordingSearchResultsResponse>");
	
	return offset;
}

int build_FindEvents_rly_xml(char * p_buf, int mlen, const char * argv)
{
    int offset = 0;
	FindEvents_RES * p_res = (FindEvents_RES *)argv;
	
	offset += snprintf(p_buf+offset, mlen-offset, 
		"<tse:FindEventsResponse>"
			"<tse:SearchToken>%s</tse:SearchToken>"
		"</tse:FindEventsResponse>",
		p_res->SearchToken);

	return offset;
}

int build_FindEventResult_xml(char * p_buf, int mlen, onvif_FindEventResult * p_req)
{
	int offset = 0;
	char TimeBuff[64];

	onvif_get_time_str_s(TimeBuff, sizeof(TimeBuff), p_req->Time, 0);

	offset += snprintf(p_buf+offset, mlen-offset, 
		"<tt:RecordingToken>%s</tt:RecordingToken>"
     	"<tt:TrackToken>%s</tt:TrackToken>"
     	"<tt:Time>%s</tt:Time>",
     	p_req->RecordingToken, 
     	p_req->TrackToken, 
     	TimeBuff);

	offset += snprintf(p_buf+offset, mlen-offset, "<tt:Event>");
	offset += build_NotificationMessage_xml(p_buf+offset, mlen-offset, &p_req->Event);
	offset += snprintf(p_buf+offset, mlen-offset, "</tt:Event>");
	
	offset += snprintf(p_buf+offset, mlen-offset, 
		"<tt:StartStateEvent>%s</tt:StartStateEvent>",
		p_req->StartStateEvent ? "true" : "false");
	
	return offset;
}

int build_GetEventSearchResults_rly_xml(char * p_buf, int mlen, const char * argv)
{
    int offset = 0;
	ONVIF_FindEventResult * p_EventResult;
	GetEventSearchResults_RES * p_res = (GetEventSearchResults_RES *)argv;
	
	offset += snprintf(p_buf+offset, mlen-offset, "<tse:GetEventSearchResultsResponse><tse:ResultList>");
	offset += snprintf(p_buf+offset, mlen-offset, "<tt:SearchState>%s</tt:SearchState>", onvif_SearchStateToString(p_res->ResultList.SearchState));

	p_EventResult = p_res->ResultList.Result;
	while (p_EventResult)
	{
		offset += snprintf(p_buf+offset, mlen-offset, "<tt:Result>");
		offset += build_FindEventResult_xml(p_buf+offset, mlen-offset, &p_EventResult->FindEventResult);
		offset += snprintf(p_buf+offset, mlen-offset, "</tt:Result>");

		p_EventResult = p_EventResult->next;
	}
	
	offset += snprintf(p_buf+offset, mlen-offset, "</tse:ResultList></tse:GetEventSearchResultsResponse>");
	
	return offset;
}

int build_FindMetadata_rly_xml(char * p_buf, int mlen, const char * argv)
{
    int offset = 0;
	FindMetadata_RES * p_res = (FindMetadata_RES *)argv;
	
	offset += snprintf(p_buf+offset, mlen-offset, 
		"<tse:FindMetadataResponse>"
			"<tse:SearchToken>%s</tse:SearchToken>"
		"</tse:FindMetadataResponse>",
		p_res->SearchToken);

	return offset;
}

int build_FindMetadataResult_xml(char * p_buf, int mlen, onvif_FindMetadataResult * p_res)
{
    int offset = 0;
	char TimeBuff[64];

	onvif_get_time_str_s(TimeBuff, sizeof(TimeBuff), p_res->Time, 0);

	offset += snprintf(p_buf+offset, mlen-offset, 
		"<tt:RecordingToken>%s</tt:RecordingToken>"
     	"<tt:TrackToken>%s</tt:TrackToken>"
     	"<tt:Time>%s</tt:Time>",
     	p_res->RecordingToken, 
     	p_res->TrackToken, 
     	TimeBuff);	
	
	return offset;
}

int build_GetMetadataSearchResults_rly_xml(char * p_buf, int mlen, const char * argv)
{
    int offset = 0;
    ONVIF_FindMetadataResult * p_Result;
	GetMetadataSearchResults_RES * p_res = (GetMetadataSearchResults_RES *)argv;
	
	offset += snprintf(p_buf+offset, mlen-offset, "<tse:GetMetadataSearchResultsResponse><tse:ResultList>");
	offset += snprintf(p_buf+offset, mlen-offset, "<tt:SearchState>%s</tt:SearchState>", onvif_SearchStateToString(p_res->ResultList.SearchState));

	p_Result = p_res->ResultList.Result;
	while (p_Result)
	{
		offset += snprintf(p_buf+offset, mlen-offset, "<tt:Result>");
		offset += build_FindMetadataResult_xml(p_buf+offset, mlen-offset, &p_Result->Result);
		offset += snprintf(p_buf+offset, mlen-offset, "</tt:Result>");

		p_Result = p_Result->next;
	}
	
	offset += snprintf(p_buf+offset, mlen-offset, "</tse:ResultList></tse:GetMetadataSearchResultsResponse>");
	
	return offset;
}

int build_FindPTZPosition_rly_xml(char * p_buf, int mlen, const char * argv)
{
    int offset = 0;
    FindPTZPosition_RES * p_res = (FindPTZPosition_RES *)argv;
	
	offset += snprintf(p_buf+offset, mlen-offset, 
		"<tse:FindPTZPositionResponse>"
			"<tse:SearchToken>%s</tse:SearchToken>"
		"</tse:FindPTZPositionResponse>",
		p_res->SearchToken);

	return offset;
}

int build_FindPTZPositionResult_xml(char * p_buf, int mlen, onvif_FindPTZPositionResult * p_res)
{
    int offset = 0;
	char TimeBuff[64];

	onvif_get_time_str_s(TimeBuff, sizeof(TimeBuff), p_res->Time, 0);

	offset += snprintf(p_buf+offset, mlen-offset, 
		"<tt:RecordingToken>%s</tt:RecordingToken>"
     	"<tt:TrackToken>%s</tt:TrackToken>"
     	"<tt:Time>%s</tt:Time>",
     	p_res->RecordingToken, 
     	p_res->TrackToken, 
     	TimeBuff);

	offset += snprintf(p_buf+offset, mlen-offset, "<tt:Position>");
	if (p_res->Position.PanTiltFlag)
	{
		offset += snprintf(p_buf+offset, mlen-offset, 
			"<tt:PanTilt x=\"%0.1f\" y=\"%0.1f\" space=\"http://www.onvif.org/ver10/tptz/PanTiltSpaces/PositionGenericSpace\" />",
			p_res->Position.PanTilt.x,
			p_res->Position.PanTilt.y);
	}	
	if (p_res->Position.ZoomFlag)
	{
		offset += snprintf(p_buf+offset, mlen-offset, 
			"<tt:Zoom x=\"%0.1f\" space=\"http://www.onvif.org/ver10/tptz/ZoomSpaces/PositionGenericSpace\" />",
			p_res->Position.Zoom.x);
	}		
	offset += snprintf(p_buf+offset, mlen-offset, "</tt:Position>");	
	
	return offset;
}

int build_GetPTZPositionSearchResults_rly_xml(char * p_buf, int mlen, const char * argv)
{
    int offset = 0;
    ONVIF_FindPTZPositionResult * p_Result;
	GetPTZPositionSearchResults_RES * p_res = (GetPTZPositionSearchResults_RES *)argv;
	
	offset += snprintf(p_buf+offset, mlen-offset, "<tse:GetPTZPositionSearchResultsResponse><tse:ResultList>");
	offset += snprintf(p_buf+offset, mlen-offset, "<tt:SearchState>%s</tt:SearchState>", onvif_SearchStateToString(p_res->ResultList.SearchState));

	p_Result = p_res->ResultList.Result;
	while (p_Result)
	{
		offset += snprintf(p_buf+offset, mlen-offset, "<tt:Result>");
		offset += build_FindPTZPositionResult_xml(p_buf+offset, mlen-offset, &p_Result->FindPTZPositionResult);
		offset += snprintf(p_buf+offset, mlen-offset, "</tt:Result>");

		p_Result = p_Result->next;
	}
	
	offset += snprintf(p_buf+offset, mlen-offset, "</tse:ResultList></tse:GetPTZPositionSearchResultsResponse>");
	
	return offset;
}

int build_EndSearch_rly_xml(char * p_buf, int mlen, const char * argv)
{
    int offset = 0;
	char EndPoint[64];
	EndSearch_RES * p_res = (EndSearch_RES *)argv;
	
	onvif_get_time_str_s(EndPoint, sizeof(EndPoint), p_res->Endpoint, 0);

	offset += snprintf(p_buf+offset, mlen-offset, 
		"<tse:EndSearchResponse>"
			"<tse:Endpoint>%s</tse:Endpoint>"
		"</tse:EndSearchResponse>",
		EndPoint);

	return offset;
}

int build_GetSearchState_rly_xml(char * p_buf, int mlen, const char * argv)
{
    int offset = 0;
	GetSearchState_RES * p_res = (GetSearchState_RES *)argv;
	
	offset += snprintf(p_buf+offset, mlen-offset, 
		"<tse:GetSearchStateResponse>"
			"<tse:State>%s</tse:State>"
		"</tse:GetSearchStateResponse>",
		onvif_SearchStateToString(p_res->State));

	return offset;
}

int build_CreateRecording_rly_xml(char * p_buf, int mlen, const char * argv)
{
	int offset = 0;

	offset += snprintf(p_buf+offset, mlen-offset, 
		"<trc:CreateRecordingResponse>"
			"<trc:RecordingToken>%s</trc:RecordingToken>"
		"</trc:CreateRecordingResponse>",
		argv);

	return offset;
}

int build_DeleteRecording_rly_xml(char * p_buf, int mlen, const char * argv)
{
	int offset = 0;
	offset += snprintf(p_buf+offset, mlen-offset, "<trc:DeleteRecordingResponse />");
	return offset;
}

int build_RecordingConfiguration_xml(char * p_buf, int mlen, onvif_RecordingConfiguration * p_req)
{
	int offset = 0;
	
	offset += build_RecordingSourceInformation_xml(p_buf+offset, mlen-offset, &p_req->Source);
	offset += snprintf(p_buf+offset, mlen-offset, "<tt:Content>%s</tt:Content>", p_req->Content);
	
	if (p_req->MaximumRetentionTimeFlag)
	{
		offset += snprintf(p_buf+offset, mlen-offset, "<tt:MaximumRetentionTime>PT%dS</tt:MaximumRetentionTime>", p_req->MaximumRetentionTime);
	}

	return offset;
}

int build_TrackConfiguration_xml(char * p_buf, int mlen, onvif_TrackConfiguration * p_req)
{
	int offset = 0;

	offset += snprintf(p_buf+offset, mlen-offset, 
		"<tt:TrackType>%s</tt:TrackType>"
		"<tt:Description>%s</tt:Description>",
		onvif_TrackTypeToString(p_req->TrackType),
		p_req->Description);

	return offset;		
}

int build_GetRecordings_rly_xml(char * p_buf, int mlen, const char * argv)
{
    int offset = 0;
	ONVIF_Recording * p_recording = g_onvif_cfg.recordings;
	
	offset += snprintf(p_buf+offset, mlen-offset, "<trc:GetRecordingsResponse>");
	while (p_recording)
	{
		ONVIF_Track * p_track = p_recording->Recording.Tracks;
		
		offset += snprintf(p_buf+offset, mlen-offset, "<trc:RecordingItem>");
		offset += snprintf(p_buf+offset, mlen-offset, "<tt:RecordingToken>%s</tt:RecordingToken>", p_recording->Recording.RecordingToken);
		offset += snprintf(p_buf+offset, mlen-offset, "<tt:Configuration>");
		offset += build_RecordingConfiguration_xml(p_buf+offset, mlen-offset, &p_recording->Recording.Configuration);
		offset += snprintf(p_buf+offset, mlen-offset, "</tt:Configuration>");
		
		offset += snprintf(p_buf+offset, mlen-offset, "<tt:Tracks>");
		while (p_track)
		{
			offset += snprintf(p_buf+offset, mlen-offset, "<tt:Track><tt:TrackToken>%s</tt:TrackToken><tt:Configuration>", p_track->Track.TrackToken);
			offset += build_TrackConfiguration_xml(p_buf+offset, mlen-offset, &p_track->Track.Configuration);
			offset += snprintf(p_buf+offset, mlen-offset, "</tt:Configuration></tt:Track>");
			
			p_track = p_track->next;
		}
		offset += snprintf(p_buf+offset, mlen-offset, "</tt:Tracks>");
		
		offset += snprintf(p_buf+offset, mlen-offset, "</trc:RecordingItem>");
		
		p_recording = p_recording->next;
	}
	offset += snprintf(p_buf+offset, mlen-offset, "</trc:GetRecordingsResponse>");

	return offset;
}

int build_SetRecordingConfiguration_rly_xml(char * p_buf, int mlen, const char * argv)
{
	int offset = 0;
	offset += snprintf(p_buf+offset, mlen-offset, "<trc:SetRecordingConfigurationResponse />");
	return offset;
}

int build_GetRecordingConfiguration_rly_xml(char * p_buf, int mlen, const char * argv)
{
	int offset = 0;
	ONVIF_Recording * p_recording = onvif_find_Recording(argv);
	if (NULL == p_recording)
	{
		return ONVIF_ERR_NoRecording;
	}
	
	offset += snprintf(p_buf+offset, mlen-offset, "<trc:GetRecordingConfigurationResponse><trc:RecordingConfiguration>");
	offset += build_RecordingConfiguration_xml(p_buf+offset, mlen-offset, &p_recording->Recording.Configuration);
	offset += snprintf(p_buf+offset, mlen-offset, "</trc:RecordingConfiguration></trc:GetRecordingConfigurationResponse>");

	return offset;
}

int build_CreateTrack_rly_xml(char * p_buf, int mlen, const char * argv)
{
	int offset = 0;

	offset += snprintf(p_buf+offset, mlen-offset, 
		"<trc:CreateTrackResponse>"
			"<trc:TrackToken>%s</trc:TrackToken>"
		"</trc:CreateTrackResponse>",
		argv);

	return offset;
}

int build_DeleteTrack_rly_xml(char * p_buf, int mlen, const char * argv)
{
	int offset = 0;
	offset += snprintf(p_buf+offset, mlen-offset, "<trc:DeleteTrackResponse />");
	return offset;
}

int build_GetTrackConfiguration_rly_xml(char * p_buf, int mlen, const char * argv)
{
	int offset = 0;
	ONVIF_Track * p_track;
	ONVIF_Recording * p_recording;
	GetTrackConfiguration_REQ * p_req = (GetTrackConfiguration_REQ *)argv;

	p_recording = onvif_find_Recording(p_req->RecordingToken);
	if (NULL == p_recording)
	{
		return ONVIF_ERR_NoRecording;
	}

	p_track = onvif_find_Track(p_recording->Recording.Tracks, p_req->TrackToken);
	if (NULL == p_track)
	{
		return ONVIF_ERR_NoTrack;
	}
	
	offset += snprintf(p_buf+offset, mlen-offset, "<trc:GetTrackConfigurationResponse><trc:TrackConfiguration>");
	offset += build_TrackConfiguration_xml(p_buf+offset, mlen-offset, &p_track->Track.Configuration);
	offset += snprintf(p_buf+offset, mlen-offset, "</trc:TrackConfiguration></trc:GetTrackConfigurationResponse>");

	return offset;
}

int build_SetTrackConfiguration_rly_xml(char * p_buf, int mlen, const char * argv)
{
	int offset = 0;
	offset += snprintf(p_buf+offset, mlen-offset, "<trc:SetTrackConfigurationResponse />");
	return offset;
}

int build_RecordingJobConfiguration_xml(char * p_buf, int mlen, onvif_RecordingJobConfiguration * p_req)
{
	int i, j, offset = 0;

	offset += snprintf(p_buf+offset, mlen-offset, 
		"<tt:RecordingToken>%s</tt:RecordingToken>"
		"<tt:Mode>%s</tt:Mode>"
		"<tt:Priority>%d</tt:Priority>", 
		p_req->RecordingToken,
		p_req->Mode, 
		p_req->Priority);

	for (i = 0; i < p_req->sizeSource; i++)
	{
		offset += snprintf(p_buf+offset, mlen-offset, "<tt:Source>");
		
		if (p_req->Source[i].SourceTokenFlag)
		{
			if (p_req->Source[i].SourceToken.TypeFlag)
			{
				offset += snprintf(p_buf+offset, mlen-offset, "<tt:SourceToken Type=\"%s\">", p_req->Source[i].SourceToken.Type);
			}
			else
			{
				offset += snprintf(p_buf+offset, mlen-offset, "<tt:SourceToken>");
			}

			offset += snprintf(p_buf+offset, mlen-offset, "<tt:Token>%s</tt:Token>", p_req->Source[i].SourceToken.Token);
			offset += snprintf(p_buf+offset, mlen-offset, "</tt:SourceToken>");
		}

		for (j = 0; j < p_req->Source[i].sizeTracks; j++)
		{
			offset += snprintf(p_buf+offset, mlen-offset, 
				"<tt:Tracks>"
					"<tt:SourceTag>%s</tt:SourceTag>"
	      			"<tt:Destination>%s</tt:Destination>"
      			"</tt:Tracks>",
      			p_req->Source[i].Tracks[j].SourceTag,
      			p_req->Source[i].Tracks[j].Destination);
		}
		
		offset += snprintf(p_buf+offset, mlen-offset, "</tt:Source>");
	}
	
	return offset;
}

int build_CreateRecordingJob_rly_xml(char * p_buf, int mlen, const char * argv)
{
    int offset = 0;
	CreateRecordingJob_REQ * p_req = (CreateRecordingJob_REQ *) argv;

	offset += snprintf(p_buf+offset, mlen-offset, "<trc:CreateRecordingJobResponse>");
	offset += snprintf(p_buf+offset, mlen-offset, "<trc:JobToken>%s</trc:JobToken>", p_req->JobToken);
	offset += snprintf(p_buf+offset, mlen-offset, "<trc:JobConfiguration>");
	offset += build_RecordingJobConfiguration_xml(p_buf+offset, mlen-offset, &p_req->JobConfiguration);
	offset += snprintf(p_buf+offset, mlen-offset, "</trc:JobConfiguration>");
	offset += snprintf(p_buf+offset, mlen-offset, "</trc:CreateRecordingJobResponse>");

	return offset;
}

int build_DeleteRecordingJob_rly_xml(char * p_buf, int mlen, const char * argv)
{
	int offset = 0;
	offset += snprintf(p_buf+offset, mlen-offset, "<trc:DeleteRecordingJobResponse />");
	return offset;
}

int build_GetRecordingJobs_rly_xml(char * p_buf, int mlen, const char * argv)
{
    int offset = 0;
	ONVIF_RecordingJob * p_recordingjob = g_onvif_cfg.recording_jobs;
	
	offset += snprintf(p_buf+offset, mlen-offset, "<trc:GetRecordingJobsResponse>");
	while (p_recordingjob)
	{
		offset += snprintf(p_buf+offset, mlen-offset, "<trc:JobItem>");
		offset += snprintf(p_buf+offset, mlen-offset, "<tt:JobToken>%s</tt:JobToken>", p_recordingjob->RecordingJob.JobToken);
		offset += snprintf(p_buf+offset, mlen-offset, "<tt:JobConfiguration>");
		offset += build_RecordingJobConfiguration_xml(p_buf+offset, mlen-offset, &p_recordingjob->RecordingJob.JobConfiguration);
		offset += snprintf(p_buf+offset, mlen-offset, "</tt:JobConfiguration>");
		offset += snprintf(p_buf+offset, mlen-offset, "</trc:JobItem>");

		p_recordingjob = p_recordingjob->next;
	}
	offset += snprintf(p_buf+offset, mlen-offset, "</trc:GetRecordingJobsResponse>");

	return offset;
}

int build_SetRecordingJobConfiguration_rly_xml(char * p_buf, int mlen, const char * argv)
{
	int offset = 0;
	SetRecordingJobConfiguration_REQ * p_req = (SetRecordingJobConfiguration_REQ *)argv;

	offset += snprintf(p_buf+offset, mlen-offset, "<trc:SetRecordingJobConfigurationResponse><trc:JobConfiguration>");
	offset += build_RecordingJobConfiguration_xml(p_buf+offset, mlen-offset, &p_req->JobConfiguration);
	offset += snprintf(p_buf+offset, mlen-offset, "</trc:JobConfiguration></trc:SetRecordingJobConfigurationResponse>");
    
	return offset;
}

int build_GetRecordingJobConfiguration_rly_xml(char * p_buf, int mlen, const char * argv)
{
	int offset = 0;
	ONVIF_RecordingJob * p_recordingjob = onvif_find_RecordingJob(argv);
	if (NULL == p_recordingjob)
	{
		return ONVIF_ERR_NoRecordingJob;
	}
	offset += snprintf(p_buf+offset, mlen-offset, "<trc:GetRecordingJobConfigurationResponse><trc:JobConfiguration>");
	offset += build_RecordingJobConfiguration_xml(p_buf+offset, mlen-offset, &p_recordingjob->RecordingJob.JobConfiguration);
	offset += snprintf(p_buf+offset, mlen-offset, "</trc:JobConfiguration></trc:GetRecordingJobConfigurationResponse>");
	
	return offset;
}

int build_SetRecordingJobMode_rly_xml(char * p_buf, int mlen, const char * argv)
{
	int offset = 0;
	offset += snprintf(p_buf+offset, mlen-offset, "<trc:SetRecordingJobModeResponse />");
	return offset;
}

int build_RecordingJobStateInformation_xml(char * p_buf, int mlen, onvif_RecordingJobStateInformation * p_res)
{
    int i, j;
    int offset = 0;
    
    offset += snprintf(p_buf+offset, mlen-offset, 
		"<tt:RecordingToken>%s</tt:RecordingToken>" 
		"<tt:State>%s</tt:State>", 
		p_res->RecordingToken,
		p_res->State);

    for (i = 0; i < p_res->sizeSources; i++)
	{
		offset += snprintf(p_buf+offset, mlen-offset, "<tt:Sources>");
		
		if (p_res->Sources[i].SourceToken.TypeFlag)
		{
			offset += snprintf(p_buf+offset, mlen-offset, "<tt:SourceToken Type=\"%s\">", 
				p_res->Sources[i].SourceToken.Type);
		}
		else
		{
			offset += snprintf(p_buf+offset, mlen-offset, "<tt:SourceToken>");
		}
		offset += snprintf(p_buf+offset, mlen-offset, "<tt:Token>%s</tt:Token>",
			p_res->Sources[i].SourceToken.Token);
		offset += snprintf(p_buf+offset, mlen-offset, "</tt:SourceToken>");

		offset += snprintf(p_buf+offset, mlen-offset, "<tt:State>%s</tt:State>",
			p_res->Sources[i].State);

		// tracks
		offset += snprintf(p_buf+offset, mlen-offset, "<tt:Tracks>");

		for (j = 0; j < p_res->Sources[i].sizeTrack; j++)
		{
	      	offset += snprintf(p_buf+offset, mlen-offset, "<tt:Track>");
	      	offset += snprintf(p_buf+offset, mlen-offset, 
	      		"<tt:SourceTag>%s</tt:SourceTag>"
	      		"<tt:Destination>%s</tt:Destination>", 
	      		p_res->Sources[i].Track[j].SourceTag,
	      		p_res->Sources[i].Track[j].Destination);
	      	if (p_res->Sources[i].Track[j].ErrorFlag)
	      	{
	      		offset += snprintf(p_buf+offset, mlen-offset, "<tt:Error>%s</tt:Error>", 
		      		p_res->Sources[i].Track[j].Error);
	      	}
      		offset += snprintf(p_buf+offset, mlen-offset, "<tt:State>%s</tt:State>", 
	      		p_res->Sources[i].Track[j].State);
	      	offset += snprintf(p_buf+offset, mlen-offset, "</tt:Track>");
		}
		
		offset += snprintf(p_buf+offset, mlen-offset, "</tt:Tracks>");
		// end of tracks
		
		offset += snprintf(p_buf+offset, mlen-offset, "</tt:Sources>");	
	}  

	return offset;
}

int build_GetRecordingJobState_rly_xml(char * p_buf, int mlen, const char * argv)
{
    int offset = 0;
	onvif_RecordingJobStateInformation * p_res = (onvif_RecordingJobStateInformation *) argv;

	offset += snprintf(p_buf+offset, mlen-offset, "<trc:GetRecordingJobStateResponse><trc:State>");
    offset += build_RecordingJobStateInformation_xml(p_buf+offset, mlen-offset, p_res);
	offset += snprintf(p_buf+offset, mlen-offset, "</trc:State></trc:GetRecordingJobStateResponse>");

	return offset;
}

int build_GetRecordingOptions_rly_xml(char * p_buf, int mlen, const char * argv)
{
    int offset = 0;
	onvif_RecordingOptions * p_res = (onvif_RecordingOptions *) argv;

	offset += snprintf(p_buf+offset, mlen-offset, "<trc:GetRecordingOptionsResponse><trc:Options>");

	// job options
	offset += snprintf(p_buf+offset, mlen-offset, "<trc:Job");
	if (p_res->Job.SpareFlag)
	{
		offset += snprintf(p_buf+offset, mlen-offset, " Spare=\"%d\" ", p_res->Job.Spare);
	}
	if (p_res->Job.CompatibleSourcesFlag)
	{
		offset += snprintf(p_buf+offset, mlen-offset, " CompatibleSources=\"%s\"", p_res->Job.CompatibleSources);
	}
	offset += snprintf(p_buf+offset, mlen-offset, "></trc:Job>");

	// track options
	offset += snprintf(p_buf+offset, mlen-offset, "<trc:Track");
	if (p_res->Track.SpareTotalFlag)
	{
		offset += snprintf(p_buf+offset, mlen-offset, " SpareTotal=\"%d\"", p_res->Track.SpareTotal);
	}
	if (p_res->Track.SpareVideoFlag)
	{
		offset += snprintf(p_buf+offset, mlen-offset, " SpareVideo=\"%d\"", p_res->Track.SpareVideo);
	}
	if (p_res->Track.SpareAudioFlag)
	{
		offset += snprintf(p_buf+offset, mlen-offset, " SpareAudio=\"%d\"", p_res->Track.SpareAudio);
	}
	if (p_res->Track.SpareMetadataFlag)
	{
		offset += snprintf(p_buf+offset, mlen-offset, " SpareMetadata=\"%d\"", p_res->Track.SpareMetadata);
	}
	offset += snprintf(p_buf+offset, mlen-offset, "></trc:Track>");
	
	offset += snprintf(p_buf+offset, mlen-offset, "</trc:Options></trc:GetRecordingOptionsResponse>");

	return offset;
}

int build_GetReplayUri_rly_xml(char * p_buf, int mlen, const char * argv)
{
    int offset = 0;
	GetReplayUri_RES * p_res = (GetReplayUri_RES *)argv;
	
	offset += snprintf(p_buf+offset, mlen-offset, 
		"<trp:GetReplayUriResponse>"
			"<trp:Uri>%s</trp:Uri>"
		"</trp:GetReplayUriResponse>",
		p_res->Uri);

	return offset;
}

int build_GetReplayConfiguration_rly_xml(char * p_buf, int mlen, const char * argv)
{
    int offset = 0;
	GetReplayConfiguration_RES * p_res = (GetReplayConfiguration_RES *)argv;
	
	offset += snprintf(p_buf+offset, mlen-offset, 
		"<trp:GetReplayConfigurationResponse>"
			"<trp:Configuration>"
				"<tt:SessionTimeout>PT%dS</tt:SessionTimeout>"
			"</trp:Configuration>"
		"</trp:GetReplayConfigurationResponse>",
		p_res->SessionTimeout);

	return offset;
}

int build_SetReplayConfiguration_rly_xml(char * p_buf, int mlen, const char * argv)
{
	int offset = 0;
	offset += snprintf(p_buf+offset, mlen-offset, "<trp:SetReplayConfigurationResponse />");
	return offset;
}

#endif // end of PROFILE_G_SUPPORT

#ifdef VIDEO_ANALYTICS

int build_Config_xml(char * p_buf, int mlen, onvif_Config * p_req)
{
	int offset = 0;
	ONVIF_SimpleItem * p_simpleitem;
	ONVIF_ElementItem * p_elementitem;

	offset += snprintf(p_buf+offset, mlen-offset, "<tt:Parameters>");

	p_simpleitem = p_req->Parameters.SimpleItem;
	while (p_simpleitem)
	{
		offset += build_SimpleItem_xml(p_buf+offset, mlen-offset, &p_simpleitem->SimpleItem);
		
		p_simpleitem = p_simpleitem->next;
	}

	p_elementitem = p_req->Parameters.ElementItem;
	while (p_elementitem)
	{
		offset += snprintf(p_buf+offset, mlen-offset, "<tt:ElementItem Name=\"%s\">", p_elementitem->ElementItem.Name);

        if (p_elementitem->ElementItem.AnyFlag && p_elementitem->ElementItem.Any)
        {
            offset += snprintf(p_buf+offset, mlen-offset, "%s", p_elementitem->ElementItem.Any);
        }

		offset += snprintf(p_buf+offset, mlen-offset, "</tt:ElementItem>");
		
		p_elementitem = p_elementitem->next;
	}
	
	offset += snprintf(p_buf+offset, mlen-offset, "</tt:Parameters>");
	
	return offset;
}

int build_VideoAnalyticsConfiguration_xml(char * p_buf, int mlen, onvif_VideoAnalyticsConfiguration * p_req)
{
	int offset = 0;
	ONVIF_Config * p_config;
	
	offset += snprintf(p_buf+offset, mlen-offset, 
		"<tt:Name>%s</tt:Name>"
    	"<tt:UseCount>%d</tt:UseCount>",
    	p_req->Name, p_req->UseCount);

	offset += snprintf(p_buf+offset, mlen-offset, "<tt:AnalyticsEngineConfiguration>");

	p_config = p_req->AnalyticsEngineConfiguration.AnalyticsModule;
	while (p_config)
	{
		offset += snprintf(p_buf+offset, mlen-offset, "<tt:AnalyticsModule Name=\"%s\" Type=\"%s\">", 
			p_config->Config.Name, p_config->Config.Type);
		offset += build_Config_xml(p_buf+offset, mlen-offset, &p_config->Config);
		offset += snprintf(p_buf+offset, mlen-offset, "</tt:AnalyticsModule>");

		p_config = p_config->next;
	}
	
	offset += snprintf(p_buf+offset, mlen-offset, "</tt:AnalyticsEngineConfiguration>");

	offset += snprintf(p_buf+offset, mlen-offset, "<tt:RuleEngineConfiguration>");

	p_config = p_req->RuleEngineConfiguration.Rule;
	while (p_config)
	{
	    if (p_config->Config.attrFlag)
	    {
	        offset += snprintf(p_buf+offset, mlen-offset, 
		        "<tt:Rule Name=\"%s\" Type=\"%s\" %s>", 
		        p_config->Config.Name, p_config->Config.Type, p_config->Config.attr);
	    }
	    else
	    {
		    offset += snprintf(p_buf+offset, mlen-offset, 
		        "<tt:Rule Name=\"%s\" Type=\"%s\">", 
		        p_config->Config.Name, p_config->Config.Type);
		}
		
		offset += build_Config_xml(p_buf+offset, mlen-offset, &p_config->Config);
		offset += snprintf(p_buf+offset, mlen-offset, "</tt:Rule>");

		p_config = p_config->next;
	}
	
	offset += snprintf(p_buf+offset, mlen-offset, "</tt:RuleEngineConfiguration>");
	
	return offset;
}

int build_GetVideoAnalyticsConfigurations_rly_xml(char * p_buf, int mlen, const char * argv)
{
    int offset = 0;
	ONVIF_VideoAnalyticsConfiguration * p_va_cfg = g_onvif_cfg.va_cfg;
	
	offset += snprintf(p_buf+offset, mlen-offset, "<trt:GetVideoAnalyticsConfigurationsResponse>");

	while (p_va_cfg)
	{
		offset += snprintf(p_buf+offset, mlen-offset, "<trt:Configurations token=\"%s\">", p_va_cfg->Configuration.token);
		offset += build_VideoAnalyticsConfiguration_xml(p_buf+offset, mlen-offset, &p_va_cfg->Configuration);
		offset += snprintf(p_buf+offset, mlen-offset, "</trt:Configurations>");

		p_va_cfg = p_va_cfg->next;
	}
	
	offset += snprintf(p_buf+offset, mlen-offset, "</trt:GetVideoAnalyticsConfigurationsResponse>");

	return offset;
}

int build_AddVideoAnalyticsConfiguration_rly_xml(char * p_buf, int mlen, const char * argv)
{
	int offset = 0;
	offset += snprintf(p_buf+offset, mlen-offset, "<trt:AddVideoAnalyticsConfigurationResponse />");
	return offset;
}

int build_GetVideoAnalyticsConfiguration_rly_xml(char * p_buf, int mlen, const char * argv)
{
	int offset = 0;
	GetVideoAnalyticsConfiguration_REQ * p_req = (GetVideoAnalyticsConfiguration_REQ *)argv;
	ONVIF_VideoAnalyticsConfiguration * p_va_cfg = onvif_find_VideoAnalyticsConfiguration(p_req->ConfigurationToken);
	if (NULL == p_va_cfg)
	{
		return ONVIF_ERR_NoConfig;
	}
	
	offset += snprintf(p_buf+offset, mlen-offset, "<trt:GetVideoAnalyticsConfigurationResponse>");
	offset += snprintf(p_buf+offset, mlen-offset, "<trt:Configuration token=\"%s\">", p_va_cfg->Configuration.token);
	offset += build_VideoAnalyticsConfiguration_xml(p_buf+offset, mlen-offset, &p_va_cfg->Configuration);
	offset += snprintf(p_buf+offset, mlen-offset, "</trt:Configuration>");
	offset += snprintf(p_buf+offset, mlen-offset, "</trt:GetVideoAnalyticsConfigurationResponse>");

	return offset;
}

int build_RemoveVideoAnalyticsConfiguration_rly_xml(char * p_buf, int mlen, const char * argv)
{
	int offset = 0;
	offset += snprintf(p_buf+offset, mlen-offset, "<trt:RemoveVideoAnalyticsConfigurationResponse />");
	return offset;
}

int build_SetVideoAnalyticsConfiguration_rly_xml(char * p_buf, int mlen, const char * argv)
{
	int offset = 0;
	offset += snprintf(p_buf+offset, mlen-offset, "<trt:SetVideoAnalyticsConfigurationResponse />");
	return offset;
}

int build_ItemListDescription_xml(char * p_buf, int mlen, onvif_ItemListDescription * p_req)
{
	int offset = 0;
	ONVIF_SimpleItemDescription * p_simpleitem_desc;
	
	p_simpleitem_desc = p_req->SimpleItemDescription;
	while (p_simpleitem_desc)
	{
		offset += snprintf(p_buf+offset, mlen-offset, "<tt:SimpleItemDescription Name=\"%s\" Type=\"%s\" />",
			p_simpleitem_desc->SimpleItemDescription.Name, p_simpleitem_desc->SimpleItemDescription.Type);
		p_simpleitem_desc = p_simpleitem_desc->next;
	}

	p_simpleitem_desc = p_req->ElementItemDescription;
	while (p_simpleitem_desc)
	{
		offset += snprintf(p_buf+offset, mlen-offset, "<tt:ElementItemDescription Name=\"%s\" Type=\"%s\" />",
			p_simpleitem_desc->SimpleItemDescription.Name, p_simpleitem_desc->SimpleItemDescription.Type);
		p_simpleitem_desc = p_simpleitem_desc->next;
	}

	return offset;
}

int build_ConfigDescription_Messages_xml(char * p_buf, int mlen, onvif_ConfigDescription_Messages * p_req)
{
	int offset = 0;

	if (p_req->SourceFlag)
	{
		offset += snprintf(p_buf+offset, mlen-offset, "<tt:Source>");
		offset += build_ItemListDescription_xml(p_buf+offset, mlen-offset, &p_req->Source);		
		offset += snprintf(p_buf+offset, mlen-offset, "</tt:Source>");
	}

	if (p_req->KeyFlag)
	{
		offset += snprintf(p_buf+offset, mlen-offset, "<tt:Key>");
		offset += build_ItemListDescription_xml(p_buf+offset, mlen-offset, &p_req->Key);		
		offset += snprintf(p_buf+offset, mlen-offset, "</tt:Key>");
	}

	if (p_req->DataFlag)
	{
		offset += snprintf(p_buf+offset, mlen-offset, "<tt:Data>");
		offset += build_ItemListDescription_xml(p_buf+offset, mlen-offset, &p_req->Data);		
		offset += snprintf(p_buf+offset, mlen-offset, "</tt:Data>");
	}

	offset += snprintf(p_buf+offset, mlen-offset, "<tt:ParentTopic>%s</tt:ParentTopic>", p_req->ParentTopic);
	
	return offset;
}

int build_GetSupportedRules_rly_xml(char * p_buf, int mlen, const char * argv)
{
	int i;
	int offset = 0;
	GetSupportedRules_RES * p_res = (GetSupportedRules_RES *)argv;
	ONVIF_ConfigDescription * p_cfg_desc;
	ONVIF_SimpleItemDescription * p_simpleitem_desc;
	ONVIF_ConfigDescription_Messages * p_cfg_desc_msg;
	
	offset += snprintf(p_buf+offset, mlen-offset, "<tan:GetSupportedRulesResponse><tan:SupportedRules>");

	for (i = 0; i < p_res->SupportedRules.sizeRuleContentSchemaLocation; i++)
	{
		offset += snprintf(p_buf+offset, mlen-offset, 
			"<tt:RuleContentSchemaLocation>%s</tt:RuleContentSchemaLocation>",
			p_res->SupportedRules.RuleContentSchemaLocation[i]);
	}

	p_cfg_desc = p_res->SupportedRules.RuleDescription;
	while (p_cfg_desc)
	{		
		offset += snprintf(p_buf+offset, mlen-offset, 
		    "<tt:RuleDescription Name=\"%s\" fixed=\"%s\" maxInstances=\"%d\">", 
		    p_cfg_desc->ConfigDescription.Name,
		    p_cfg_desc->ConfigDescription.fixed ? "true" : "false",
		    p_cfg_desc->ConfigDescription.maxInstances);

		offset += snprintf(p_buf+offset, mlen-offset, "<tt:Parameters>");

		p_simpleitem_desc = p_cfg_desc->ConfigDescription.Parameters.SimpleItemDescription;
		while (p_simpleitem_desc)
		{
			offset += snprintf(p_buf+offset, mlen-offset, "<tt:SimpleItemDescription Name=\"%s\" Type=\"%s\" />", 
				p_simpleitem_desc->SimpleItemDescription.Name, p_simpleitem_desc->SimpleItemDescription.Type);
			p_simpleitem_desc = p_simpleitem_desc->next;
		}

		p_simpleitem_desc = p_cfg_desc->ConfigDescription.Parameters.ElementItemDescription;
		while (p_simpleitem_desc)
		{
			offset += snprintf(p_buf+offset, mlen-offset, "<tt:ElementItemDescription Name=\"%s\" Type=\"%s\" />", 
				p_simpleitem_desc->SimpleItemDescription.Name, p_simpleitem_desc->SimpleItemDescription.Type);
			p_simpleitem_desc = p_simpleitem_desc->next;
		}
		
		offset += snprintf(p_buf+offset, mlen-offset, "</tt:Parameters>");

		p_cfg_desc_msg = p_cfg_desc->ConfigDescription.Messages;
		while (p_cfg_desc_msg)
		{
			offset += snprintf(p_buf+offset, mlen-offset, "<tt:Messages IsProperty=\"%s\">", p_cfg_desc_msg->Messages.IsProperty ? "true" : "false");
			offset += build_ConfigDescription_Messages_xml(p_buf+offset, mlen-offset, &p_cfg_desc_msg->Messages);			
			offset += snprintf(p_buf+offset, mlen-offset, "</tt:Messages>");
			
			p_cfg_desc_msg = p_cfg_desc_msg->next;
		}
		
		offset += snprintf(p_buf+offset, mlen-offset, "</tt:RuleDescription>");
		
		p_cfg_desc = p_cfg_desc->next;
	}
	
	offset += snprintf(p_buf+offset, mlen-offset, "</tan:SupportedRules></tan:GetSupportedRulesResponse>");

	return offset;
}

int build_CreateRules_rly_xml(char * p_buf, int mlen, const char * argv)
{
	int offset = 0;
	offset += snprintf(p_buf+offset, mlen-offset, "<tan:CreateRulesResponse />");
	return offset;
}

int build_DeleteRules_rly_xml(char * p_buf, int mlen, const char * argv)
{
	int offset = 0;
	offset += snprintf(p_buf+offset, mlen-offset, "<tan:DeleteRulesResponse />");
	return offset;
}

int build_GetRules_rly_xml(char * p_buf, int mlen, const char * argv)
{
    int offset = 0;
	GetRules_RES * p_res = (GetRules_RES *)argv;
	ONVIF_Config * p_config;
	
	offset += snprintf(p_buf+offset, mlen-offset, "<tan:GetRulesResponse>");

	p_config = p_res->Rule;
	while (p_config)
	{
	    if (p_config->Config.attrFlag)
	    {
	        offset += snprintf(p_buf+offset, mlen-offset, 
		        "<tan:Rule Name=\"%s\" Type=\"%s\" %s>", 
		        p_config->Config.Name, p_config->Config.Type, p_config->Config.attr);
	    }
	    else
	    {
		    offset += snprintf(p_buf+offset, mlen-offset, 
		        "<tan:Rule Name=\"%s\" Type=\"%s\">", 
		        p_config->Config.Name, p_config->Config.Type);
		}
		
		offset += build_Config_xml(p_buf+offset, mlen-offset, &p_config->Config);
		offset += snprintf(p_buf+offset, mlen-offset, "</tan:Rule>");
		
		p_config = p_config->next;
	}
	
	offset += snprintf(p_buf+offset, mlen-offset, "</tan:GetRulesResponse>");

	return offset;
}

int build_ModifyRules_rly_xml(char * p_buf, int mlen, const char * argv)
{
	int offset = 0;
	offset += snprintf(p_buf+offset, mlen-offset, "<tan:ModifyRulesResponse />");
	return offset;
}

int build_CreateAnalyticsModules_rly_xml(char * p_buf, int mlen, const char * argv)
{
	int offset = 0;
	offset += snprintf(p_buf+offset, mlen-offset, "<tan:CreateAnalyticsModulesResponse />");
	return offset;
}

int build_DeleteAnalyticsModules_rly_xml(char * p_buf, int mlen, const char * argv)
{
	int offset = 0;
	offset += snprintf(p_buf+offset, mlen-offset, "<tan:DeleteAnalyticsModulesResponse />");
	return offset;
}

int build_GetAnalyticsModules_rly_xml(char * p_buf, int mlen, const char * argv)
{
    int offset = 0;
	GetAnalyticsModules_RES * p_res = (GetAnalyticsModules_RES *)argv;
	ONVIF_Config * p_config;
	
	offset += snprintf(p_buf+offset, mlen-offset, "<tan:GetAnalyticsModulesResponse>");

	p_config = p_res->AnalyticsModule;
	while (p_config)
	{
		offset += snprintf(p_buf+offset, mlen-offset, "<tan:AnalyticsModule Name=\"%s\" Type=\"%s\">", p_config->Config.Name, p_config->Config.Type);
		offset += build_Config_xml(p_buf+offset, mlen-offset, &p_config->Config);
		offset += snprintf(p_buf+offset, mlen-offset, "</tan:AnalyticsModule>");
		
		p_config = p_config->next;
	}
	
	offset += snprintf(p_buf+offset, mlen-offset, "</tan:GetAnalyticsModulesResponse>");

	return offset;
}

int build_ModifyAnalyticsModules_rly_xml(char * p_buf, int mlen, const char * argv)
{
	int offset = 0;
	offset += snprintf(p_buf+offset, mlen-offset, "<tan:ModifyAnalyticsModulesResponse />");
	return offset;
}

int build_GetAnalyticsConfigurations_rly_xml(char * p_buf, int mlen, const char * argv)
{
    int offset = 0;
    ONVIF_VideoAnalyticsConfiguration * p_cfg = g_onvif_cfg.va_cfg;
	
	offset += snprintf(p_buf+offset, mlen-offset, "<trt:GetAnalyticsConfigurationsResponse>");
	while (p_cfg)
	{
		offset += snprintf(p_buf+offset, mlen-offset, "<trt:Configurations token=\"%s\">", p_cfg->Configuration.token);
		offset += build_VideoAnalyticsConfiguration_xml(p_buf+offset, mlen-offset, &p_cfg->Configuration);
		offset += snprintf(p_buf+offset, mlen-offset, "</trt:Configurations>");
		
		p_cfg = p_cfg->next;
	}
	offset += snprintf(p_buf+offset, mlen-offset, "</trt:GetAnalyticsConfigurationsResponse>");

	return offset;
}

int build_GetRuleOptions_rly_xml(char * p_buf, int mlen, const char * argv)
{
    int offset = 0;
    ONVIF_ConfigDescription * p_desc;
    ONVIF_ConfigOptions * p_options;
    GetRuleOptions_REQ * p_req = (GetRuleOptions_REQ *)argv;
    ONVIF_VideoAnalyticsConfiguration * p_va_cfg = onvif_find_VideoAnalyticsConfiguration(p_req->ConfigurationToken);
    if (NULL == p_va_cfg)
    {
        return ONVIF_ERR_NoConfig;
    }

    offset += snprintf(p_buf+offset, mlen-offset, "<tan:GetRuleOptionsResponse>");

    p_desc = p_va_cfg->SupportedRules.RuleDescription;
    while (p_desc)
    {
        if (p_req->RuleType[0] == '\0' || 
            soap_strcmp(p_req->RuleType, p_desc->ConfigDescription.Name) == 0)
        {
            p_options = p_desc->RuleOptions;
            while (p_options)
            {
                offset += snprintf(p_buf+offset, mlen-offset, "<tan:RuleOptions Name=\"%s\" Type=\"%s\">",
                    p_options->Options.Name, p_options->Options.Type);                

                if (p_options->Options.any)
                {
                    offset += snprintf(p_buf+offset, mlen-offset, "%s", p_options->Options.any);
                }
                
                offset += snprintf(p_buf+offset, mlen-offset, "</tan:RuleOptions>");
                
                p_options = p_options->next;
            }
        }
        
        p_desc = p_desc->next;
    }    

    offset += snprintf(p_buf+offset, mlen-offset, "</tan:GetRuleOptionsResponse>");

    return offset;
}

int build_GetSupportedAnalyticsModules_rly_xml(char * p_buf, int mlen, const char * argv)
{
    int i;
    int offset = 0;
    ONVIF_SimpleItemDescription * p_simpleitem_desc;
    ONVIF_ConfigDescription_Messages * p_cfg_desc_msg;
    ONVIF_ConfigDescription * p_cfg_desc;
    GetSupportedAnalyticsModules_REQ * p_req = (GetSupportedAnalyticsModules_REQ *)argv;
    ONVIF_VideoAnalyticsConfiguration * p_va_cfg = onvif_find_VideoAnalyticsConfiguration(p_req->ConfigurationToken);
    if (NULL == p_va_cfg)
    {
        return ONVIF_ERR_NoConfig;
    }

    offset += snprintf(p_buf+offset, mlen-offset, "<tan:GetSupportedAnalyticsModulesResponse>");

    for (i = 0; i < p_va_cfg->SupportedRules.sizeRuleContentSchemaLocation; i++)
	{
		offset += snprintf(p_buf+offset, mlen-offset, 
			"<tt:RuleContentSchemaLocation>%s</tt:RuleContentSchemaLocation>",
			p_va_cfg->SupportedRules.RuleContentSchemaLocation[i]);
	}

	p_cfg_desc = p_va_cfg->SupportedRules.RuleDescription;
	while (p_cfg_desc)
	{		
		offset += snprintf(p_buf+offset, mlen-offset, 
		    "<tt:AnalyticsModuleDescription Name=\"%s\" fixed=\"%s\" maxInstances=\"%d\">", 
		    p_cfg_desc->ConfigDescription.Name,
		    p_cfg_desc->ConfigDescription.fixed ? "true" : "false",
		    p_cfg_desc->ConfigDescription.maxInstances);

		offset += snprintf(p_buf+offset, mlen-offset, "<tt:Parameters>");

		p_simpleitem_desc = p_cfg_desc->ConfigDescription.Parameters.SimpleItemDescription;
		while (p_simpleitem_desc)
		{
			offset += snprintf(p_buf+offset, mlen-offset, "<tt:SimpleItemDescription Name=\"%s\" Type=\"%s\" />", 
				p_simpleitem_desc->SimpleItemDescription.Name, p_simpleitem_desc->SimpleItemDescription.Type);
			p_simpleitem_desc = p_simpleitem_desc->next;
		}

		p_simpleitem_desc = p_cfg_desc->ConfigDescription.Parameters.ElementItemDescription;
		while (p_simpleitem_desc)
		{
			offset += snprintf(p_buf+offset, mlen-offset, "<tt:ElementItemDescription Name=\"%s\" Type=\"%s\" />", 
				p_simpleitem_desc->SimpleItemDescription.Name, p_simpleitem_desc->SimpleItemDescription.Type);
			p_simpleitem_desc = p_simpleitem_desc->next;
		}
		
		offset += snprintf(p_buf+offset, mlen-offset, "</tt:Parameters>");

		p_cfg_desc_msg = p_cfg_desc->ConfigDescription.Messages;
		while (p_cfg_desc_msg)
		{
			offset += snprintf(p_buf+offset, mlen-offset, "<tt:Messages IsProperty=\"%s\">", p_cfg_desc_msg->Messages.IsProperty ? "true" : "false");
			offset += build_ConfigDescription_Messages_xml(p_buf+offset, mlen-offset, &p_cfg_desc_msg->Messages);			
			offset += snprintf(p_buf+offset, mlen-offset, "</tt:Messages>");
			
			p_cfg_desc_msg = p_cfg_desc_msg->next;
		}
		
		offset += snprintf(p_buf+offset, mlen-offset, "</tt:AnalyticsModuleDescription>");
		
		p_cfg_desc = p_cfg_desc->next;
	}

    offset += snprintf(p_buf+offset, mlen-offset, "</tan:GetSupportedAnalyticsModulesResponse>");

    return offset;
}

int build_GetAnalyticsModuleOptions_rly_xml(char * p_buf, int mlen, const char * argv)
{
    int offset = 0;
    ONVIF_ConfigDescription * p_desc;
    ONVIF_ConfigOptions * p_options;
    GetAnalyticsModuleOptions_REQ * p_req = (GetAnalyticsModuleOptions_REQ *)argv;
    ONVIF_VideoAnalyticsConfiguration * p_va_cfg = onvif_find_VideoAnalyticsConfiguration(p_req->ConfigurationToken);
    if (NULL == p_va_cfg)
    {
        return ONVIF_ERR_NoConfig;
    }

    offset += snprintf(p_buf+offset, mlen-offset, "<tan:GetAnalyticsModuleOptionsResponse>");

    p_desc = p_va_cfg->SupportedRules.RuleDescription;
    while (p_desc)
    {
        p_options = p_desc->RuleOptions;
        while (p_options)
        {
            offset += snprintf(p_buf+offset, mlen-offset, "<tan:Options Type=\"%s\">",
                p_options->Options.Type);
            offset += snprintf(p_buf+offset, mlen-offset, "</tan:Options>");
            
            p_options = p_options->next;
        }
        
        p_desc = p_desc->next;
    }    

    offset += snprintf(p_buf+offset, mlen-offset, "</tan:GetAnalyticsModuleOptionsResponse>");

    return offset;
}


#endif // end of VIDEO_ANALYTICS

#ifdef PROFILE_C_SUPPORT

int build_AccessPointInfo_xml(char * p_buf, int mlen, onvif_AccessPointInfo * p_res)
{
    int offset = 0;

    offset += snprintf(p_buf+offset, mlen-offset, "<tac:AccessPointInfo token=\"%s\">", p_res->token);
    offset += snprintf(p_buf+offset, mlen-offset, "<tac:Name>%s</tac:Name>", p_res->Name);

    if (p_res->DescriptionFlag)
    {
        offset += snprintf(p_buf+offset, mlen-offset, "<tac:Description>%s</tac:Description>", p_res->Description);
    }
    if (p_res->AreaFromFlag)
    {
        offset += snprintf(p_buf+offset, mlen-offset, "<tac:AreaFrom>%s</tac:AreaFrom>", p_res->AreaFrom);
    }
    if (p_res->AreaToFlag)
    {
        offset += snprintf(p_buf+offset, mlen-offset, "<tac:AreaTo>%s</tac:AreaTo>", p_res->AreaTo);
    }
    if (p_res->EntityTypeFlag)
    {
        offset += snprintf(p_buf+offset, mlen-offset, "<tac:EntityType>%s</tac:EntityType>", p_res->EntityType);
    }

    offset += snprintf(p_buf+offset, mlen-offset, "<tac:Entity>%s</tac:Entity>", p_res->Entity);

    offset += snprintf(p_buf+offset, mlen-offset, 
        "<tac:Capabilities DisableAccessPoint=\"%s\" Duress=\"%s\" AnonymousAccess=\"%s\" AccessTaken=\"%s\" ExternalAuthorization=\"%s\" />",
        p_res->Capabilities.DisableAccessPoint ? "true" : "false",
        p_res->Capabilities.Duress ? "true" : "false",
        p_res->Capabilities.AnonymousAccess ? "true" : "false",
        p_res->Capabilities.AccessTaken ? "true" : "false",
        p_res->Capabilities.ExternalAuthorization ? "true" : "false");
    
    offset += snprintf(p_buf+offset, mlen-offset, "</tac:AccessPointInfo>");
    
    return offset;
}

int build_DoorInfo_xml(char * p_buf, int mlen, onvif_DoorInfo * p_res)
{
    int offset = 0;

    offset += snprintf(p_buf+offset, mlen-offset, "<tdc:DoorInfo token=\"%s\">", p_res->token);
    offset += snprintf(p_buf+offset, mlen-offset, "<tdc:Name>%s</tdc:Name>", p_res->Name);

    if (p_res->DescriptionFlag)
    {
        offset += snprintf(p_buf+offset, mlen-offset, "<tdc:Description>%s</tdc:Description>", p_res->Description);
    }

    offset += snprintf(p_buf+offset, mlen-offset, 
        "<tdc:Capabilities Access=\"%s\" AccessTimingOverride=\"%s\" Lock=\"%s\" Unlock=\"%s\" Block=\"%s\" DoubleLock=\"%s\" LockDown=\"%s\" "
        "LockOpen=\"%s\" DoorMonitor=\"%s\" LockMonitor=\"%s\" DoubleLockMonitor=\"%s\" Alarm=\"%s\" Tamper=\"%s\" Fault=\"%s\" />",
        p_res->Capabilities.Access ? "true" : "false",
        p_res->Capabilities.AccessTimingOverride ? "true" : "false",
        p_res->Capabilities.Lock ? "true" : "false",
        p_res->Capabilities.Unlock ? "true" : "false",
        p_res->Capabilities.Block ? "true" : "false",
        p_res->Capabilities.DoubleLock ? "true" : "false",
        p_res->Capabilities.LockDown ? "true" : "false",
        p_res->Capabilities.LockOpen ? "true" : "false",
        p_res->Capabilities.DoorMonitor ? "true" : "false",
        p_res->Capabilities.LockMonitor ? "true" : "false",
        p_res->Capabilities.DoubleLockMonitor ? "true" : "false",
        p_res->Capabilities.Alarm ? "true" : "false",
        p_res->Capabilities.Tamper ? "true" : "false",
        p_res->Capabilities.Fault ? "true" : "false");

    offset += snprintf(p_buf+offset, mlen-offset, "</tdc:DoorInfo>");

    return offset;
}

int build_AreaInfo_xml(char * p_buf, int mlen, onvif_AreaInfo * p_res)
{
    int offset = 0;

    offset += snprintf(p_buf+offset, mlen-offset, "<tac:AreaInfo token=\"%s\">", p_res->token);
    offset += snprintf(p_buf+offset, mlen-offset, "<tac:Name>%s</tac:Name>", p_res->Name);

    if (p_res->DescriptionFlag)
    {
        offset += snprintf(p_buf+offset, mlen-offset, "<tac:Description>%s</tac:Description>", p_res->Description);
    }

    offset += snprintf(p_buf+offset, mlen-offset, "</tac:AreaInfo>");

    return offset;
}

int build_tac_GetAccessPointInfoList_rly_xml(char * p_buf, int mlen, const char * argv)
{
    int offset = 0;
    tac_GetAccessPointInfoList_RES * p_res = (tac_GetAccessPointInfoList_RES *)argv;
    ONVIF_AccessPoint * p_info = p_res->AccessPointInfo;

	offset += snprintf(p_buf+offset, mlen-offset, "<tac:GetAccessPointInfoListResponse>");

    if (p_res->NextStartReferenceFlag)
    {
        offset += snprintf(p_buf+offset, mlen-offset, 
            "<tac:NextStartReference>%s</tac:NextStartReference>",
            p_res->NextStartReference);
    }

    while (p_info)
    {
        offset += build_AccessPointInfo_xml(p_buf+offset, mlen-offset, &p_info->AccessPointInfo);
        
        p_info = p_info->next;
    }

	offset += snprintf(p_buf+offset, mlen-offset, "</tac:GetAccessPointInfoListResponse>");

	return offset;
}

int build_tac_GetAccessPointInfo_rly_xml(char * p_buf, int mlen, const char * argv)
{
    int i = 0;
    int offset = 0;
    tac_GetAccessPointInfo_REQ * p_req = (tac_GetAccessPointInfo_REQ *)argv;
    ONVIF_AccessPoint * p_accesspoint;

	offset += snprintf(p_buf+offset, mlen-offset, "<tac:GetAccessPointInfoResponse>");

    for (i = 0; i < ARRAY_SIZE(p_req->token); i++)
    {
        if (p_req->token[i][0] == '\0')
        {
            break;
        }

        p_accesspoint = onvif_find_AccessPoint(p_req->token[i]);
        if (p_accesspoint)
        {
            offset += build_AccessPointInfo_xml(p_buf+offset, mlen-offset, &p_accesspoint->AccessPointInfo);
        }
    }
	
    offset += snprintf(p_buf+offset, mlen-offset, "</tac:GetAccessPointInfoResponse>");

	return offset;
}

int build_tac_GetAreaInfoList_rly_xml(char * p_buf, int mlen, const char * argv)
{
    int offset = 0;
    tac_GetAreaInfoList_RES * p_res = (tac_GetAreaInfoList_RES *)argv;
    ONVIF_AreaInfo * p_info = p_res->AreaInfo;

	offset += snprintf(p_buf+offset, mlen-offset, "<tac:GetAreaInfoListResponse>");

    if (p_res->NextStartReferenceFlag)
    {
        offset += snprintf(p_buf+offset, mlen-offset, 
            "<tac:NextStartReference>%s</tac:NextStartReference>",
            p_res->NextStartReference);
    }

    while (p_info)
    {
        offset += build_AreaInfo_xml(p_buf+offset, mlen-offset, &p_info->AreaInfo);
        
        p_info = p_info->next;
    }

	offset += snprintf(p_buf+offset, mlen-offset, "</tac:GetAreaInfoListResponse>");

	return offset;
}

int build_tac_GetAreaInfo_rly_xml(char * p_buf, int mlen, const char * argv)
{
    int i = 0;
    int offset = 0;
    tac_GetAreaInfo_REQ * p_req = (tac_GetAreaInfo_REQ *)argv;
    ONVIF_AreaInfo * p_info;

	offset += snprintf(p_buf+offset, mlen-offset, "<tac:GetAreaInfoResponse>");

    for (i = 0; i < ARRAY_SIZE(p_req->token); i++)
    {
        if (p_req->token[i][0] == '\0')
        {
            break;
        }

        p_info = onvif_find_AreaInfo(p_req->token[i]);
        if (p_info)
        {
            offset += build_AreaInfo_xml(p_buf+offset, mlen-offset, &p_info->AreaInfo);
        }
    }
	
    offset += snprintf(p_buf+offset, mlen-offset, "</tac:GetAreaInfoResponse>");

	return offset;
}

int build_tac_GetAccessPointState_rly_xml(char * p_buf, int mlen, const char * argv)
{
    int offset = 0;
    tac_GetAccessPointState_REQ * p_req = (tac_GetAccessPointState_REQ *)argv;
    ONVIF_AccessPoint * p_accesspoint = onvif_find_AccessPoint(p_req->Token);
    if (NULL == p_accesspoint)
    {
        return ONVIF_ERR_NotFound;
    }

	offset += snprintf(p_buf+offset, mlen-offset, 
	    "<tac:GetAccessPointStateResponse>"
	        "<tac:AccessPointState>"
                "<tac:Enabled>%s</tac:Enabled>"
            "</tac:AccessPointState>"
        "</tac:GetAccessPointStateResponse>",
        p_accesspoint->Enabled ? "true" : "false");
    
    return offset;
}

int build_tac_EnableAccessPoint_rly_xml(char * p_buf, int mlen, const char * argv)
{
    int offset = 0;
	offset += snprintf(p_buf+offset, mlen-offset, "<tac:EnableAccessPointResponse />");
	return offset;
}

int build_tac_DisableAccessPoint_rly_xml(char * p_buf, int mlen, const char * argv)
{
    int offset = 0;
	offset += snprintf(p_buf+offset, mlen-offset, "<tac:DisableAccessPointResponse />");
	return offset;
}

int build_tdc_GetDoorInfoList_rly_xml(char * p_buf, int mlen, const char * argv)
{
    int offset = 0;
    tdc_GetDoorInfoList_RES * p_res = (tdc_GetDoorInfoList_RES *)argv;
    ONVIF_Door * p_info = p_res->DoorInfo;

	offset += snprintf(p_buf+offset, mlen-offset, "<tdc:GetDoorInfoListResponse>");

    if (p_res->NextStartReferenceFlag)
    {
        offset += snprintf(p_buf+offset, mlen-offset, 
            "<tdc:NextStartReference>%s</tdc:NextStartReference>",
            p_res->NextStartReference);
    }

    while (p_info)
    {
        offset += build_DoorInfo_xml(p_buf+offset, mlen-offset, &p_info->DoorInfo);
        
        p_info = p_info->next;
    }

	offset += snprintf(p_buf+offset, mlen-offset, "</tdc:GetDoorInfoListResponse>");

	return offset;
}

int build_tdc_GetDoorInfo_rly_xml(char * p_buf, int mlen, const char * argv)
{
    int i = 0;
    int offset = 0;
    tdc_GetDoorInfo_REQ * p_req = (tdc_GetDoorInfo_REQ *)argv;
    ONVIF_Door * p_info;

	offset += snprintf(p_buf+offset, mlen-offset, "<tdc:GetDoorInfoResponse>");

    for (i = 0; i < ARRAY_SIZE(p_req->token); i++)
    {
        if (p_req->token[i][0] == '\0')
        {
            break;
        }

        p_info = onvif_find_Door(p_req->token[i]);
        if (p_info)
        {
            offset += build_DoorInfo_xml(p_buf+offset, mlen-offset, &p_info->DoorInfo);
        }
    }
	
    offset += snprintf(p_buf+offset, mlen-offset, "</tdc:GetDoorInfoResponse>");

	return offset;
}

int build_tdc_GetDoorState_rly_xml(char * p_buf, int mlen, const char * argv)
{
    int offset = 0;
    tdc_GetDoorState_REQ * p_req = (tdc_GetDoorState_REQ *)argv;
    ONVIF_Door * p_info = onvif_find_Door(p_req->Token);
    if (NULL == p_info)
    {
        return ONVIF_ERR_NotFound;
    }

	offset += snprintf(p_buf+offset, mlen-offset, "<tdc:GetDoorStateResponse><tdc:DoorState>");

    if (p_info->DoorState.DoorPhysicalStateFlag)
    {
        offset += snprintf(p_buf+offset, mlen-offset, 
            "<tdc:DoorPhysicalState>%s</tdc:DoorPhysicalState>",
            onvif_DoorPhysicalStateToString(p_info->DoorState.DoorPhysicalState));
    }

    if (p_info->DoorState.LockPhysicalStateFlag)
    {
        offset += snprintf(p_buf+offset, mlen-offset, 
            "<tdc:LockPhysicalState>%s</tdc:LockPhysicalState>",
            onvif_LockPhysicalStateToString(p_info->DoorState.LockPhysicalState));
    }

    if (p_info->DoorState.DoubleLockPhysicalStateFlag)
    {
        offset += snprintf(p_buf+offset, mlen-offset, 
            "<tdc:DoubleLockPhysicalState>%s</tdc:DoubleLockPhysicalState>",
            onvif_LockPhysicalStateToString(p_info->DoorState.DoubleLockPhysicalState));
    }

    if (p_info->DoorState.AlarmFlag)
    {
        offset += snprintf(p_buf+offset, mlen-offset, 
            "<tdc:Alarm>%s</tdc:Alarm>",
            onvif_DoorAlarmStateToString(p_info->DoorState.Alarm));
    }

    if (p_info->DoorState.TamperFlag)
    {
        offset += snprintf(p_buf+offset, mlen-offset, "<tdc:Tamper>");
        if (p_info->DoorState.Tamper.ReasonFlag)
        {
            offset += snprintf(p_buf+offset, mlen-offset, 
                "<tdc:Reason>%s</tdc:Reason>", 
                p_info->DoorState.Tamper.Reason);
        }
        offset += snprintf(p_buf+offset, mlen-offset, 
            "<tdc:State>%s</tdc:State>",
            onvif_DoorTamperStateToString(p_info->DoorState.Tamper.State));
        offset += snprintf(p_buf+offset, mlen-offset, "</tdc:Tamper>");
    }

    if (p_info->DoorState.FaultFlag)
    {
        offset += snprintf(p_buf+offset, mlen-offset, "<tdc:Fault>");
        if (p_info->DoorState.Fault.ReasonFlag)
        {
            offset += snprintf(p_buf+offset, mlen-offset, 
                "<tdc:Reason>%s</tdc:Reason>", 
                p_info->DoorState.Fault.Reason);
        }
        offset += snprintf(p_buf+offset, mlen-offset, 
            "<tdc:State>%s</tdc:State>",
            onvif_DoorFaultStateToString(p_info->DoorState.Fault.State));
        offset += snprintf(p_buf+offset, mlen-offset, "</tdc:Fault>");
    }

	offset += snprintf(p_buf+offset, mlen-offset, 
        "<tdc:DoorMode>%s</tdc:DoorMode>",
        onvif_DoorModeToString(p_info->DoorState.DoorMode));
            
    offset += snprintf(p_buf+offset, mlen-offset, "</tdc:DoorState></tdc:GetDoorStateResponse>");

    return offset;
}

int build_tdc_AccessDoor_rly_xml(char * p_buf, int mlen, const char * argv)
{
    int offset = 0;
	offset += snprintf(p_buf+offset, mlen-offset, "<tdc:AccessDoorResponse />");
	return offset;
}

int build_tdc_LockDoor_rly_xml(char * p_buf, int mlen, const char * argv)
{
    int offset = 0;
	offset += snprintf(p_buf+offset, mlen-offset, "<tdc:LockDoorResponse />");
	return offset;
}

int build_tdc_UnlockDoor_rly_xml(char * p_buf, int mlen, const char * argv)
{
    int offset = 0;
	offset += snprintf(p_buf+offset, mlen-offset, "<tdc:UnlockDoorResponse />");
	return offset;
}

int build_tdc_DoubleLockDoor_rly_xml(char * p_buf, int mlen, const char * argv)
{
    int offset = 0;
	offset += snprintf(p_buf+offset, mlen-offset, "<tdc:DoubleLockDoorResponse />");
	return offset;
}

int build_tdc_BlockDoor_rly_xml(char * p_buf, int mlen, const char * argv)
{
    int offset = 0;
	offset += snprintf(p_buf+offset, mlen-offset, "<tdc:BlockDoorResponse />");
	return offset;
}

int build_tdc_LockDownDoor_rly_xml(char * p_buf, int mlen, const char * argv)
{
    int offset = 0;
	offset += snprintf(p_buf+offset, mlen-offset, "<tdc:LockDownDoorResponse />");
	return offset;
}

int build_tdc_LockDownReleaseDoor_rly_xml(char * p_buf, int mlen, const char * argv)
{
    int offset = 0;
	offset += snprintf(p_buf+offset, mlen-offset, "<tdc:LockDownReleaseDoorResponse />");
	return offset;
}

int build_tdc_LockOpenDoor_rly_xml(char * p_buf, int mlen, const char * argv)
{
    int offset = 0;
	offset += snprintf(p_buf+offset, mlen-offset, "<tdc:LockOpenDoorResponse />");
	return offset;
}

int build_tdc_LockOpenReleaseDoor_rly_xml(char * p_buf, int mlen, const char * argv)
{
    int offset = 0;
	offset += snprintf(p_buf+offset, mlen-offset, "<tdc:LockOpenReleaseDoorResponse />");
	return offset;
}

#endif // end of PROFILE_C_SUPPORT

#ifdef DEVICEIO_SUPPORT

int build_tmd_GetVideoSources_rly_xml(char * p_buf, int mlen, const char * argv)
{
    int offset = 0;
    ONVIF_VideoSource * p_v_src = g_onvif_cfg.v_src;
    
	offset += snprintf(p_buf+offset, mlen-offset, "<tmd:GetVideoSourcesResponse>");

	while (p_v_src)
	{
	    offset += snprintf(p_buf+offset, mlen-offset, 
	    	"<tmd:Token>%s</tmd:Token>", p_v_src->VideoSource.token); 
	    
	    p_v_src = p_v_src->next;
	}
	
	offset += snprintf(p_buf+offset, mlen-offset, "</tmd:GetVideoSourcesResponse>");
	
	return offset;
}

int build_PaneLayout_xml(char * p_buf, int mlen, onvif_PaneLayout * p_PaneLayout)
{
    int offset = 0;

    offset += snprintf(p_buf+offset, mlen-offset, 
        "<tt:PaneLayout>"
            "<tt:Pane>%s</tt:Pane>"
            "<tt:Area bottom=\"%0.1f\" top=\"%0.1f\" right=\"%0.1f\" left=\"%0.1f\">"
            "</tt:Area>"
        "</tt:PaneLayout>",
        p_PaneLayout->Pane,
        p_PaneLayout->Area.bottom,
        p_PaneLayout->Area.top,
        p_PaneLayout->Area.right,
        p_PaneLayout->Area.left);

    return offset;        
}

int build_Layout_xml(char * p_buf, int mlen, onvif_Layout * p_Layout)
{
    int offset = 0;
    ONVIF_PaneLayout * p_PaneLayout = p_Layout->PaneLayout;

    offset += snprintf(p_buf+offset, mlen-offset, "<tt:Layout>");

    while (p_PaneLayout)
    {
        offset += build_PaneLayout_xml(p_buf+offset, mlen-offset, &p_PaneLayout->PaneLayout);
        
        p_PaneLayout = p_PaneLayout->next;
    }    

    offset += snprintf(p_buf+offset, mlen-offset, "</tt:Layout>");

    return offset;
}

int build_VideoOutput_xml(char * p_buf, int mlen, onvif_VideoOutput * p_VideoOutput)
{
    int offset = 0;

    offset += build_Layout_xml(p_buf+offset, mlen-offset, &p_VideoOutput->Layout);
    if (p_VideoOutput->ResolutionFlag)
    {
        offset += build_VideoResolution_xml(p_buf+offset, mlen-offset, &p_VideoOutput->Resolution);
    }
    if (p_VideoOutput->RefreshRateFlag)
    {
        offset += snprintf(p_buf+offset, mlen-offset, "<tt:RefreshRate>%0.2f</tt:RefreshRate>", p_VideoOutput->RefreshRate);
    }
    if (p_VideoOutput->AspectRatioFlag)
    {
        offset += snprintf(p_buf+offset, mlen-offset, "<tt:AspectRatio>%0.2f</tt:AspectRatio>", p_VideoOutput->AspectRatio);
    }
    
	return offset;
}

int build_GetVideoOutputs_rly_xml(char * p_buf, int mlen, const char * argv)
{
    int offset = 0;
	ONVIF_VideoOutput * p_output = g_onvif_cfg.v_output;
    
	offset += snprintf(p_buf+offset, mlen-offset, "<tmd:GetVideoOutputsResponse>");
	
	while (p_output)
	{
	    offset += snprintf(p_buf+offset, mlen-offset, 
	        "<tmd:VideoOutputs token=\"%s\">",
	        p_output->VideoOutput.token);

	    offset += build_VideoOutput_xml(p_buf+offset, mlen-offset, &p_output->VideoOutput);
	    
	    offset += snprintf(p_buf+offset, mlen-offset, "</tmd:VideoOutputs>");

	    p_output = p_output->next;
    }
    
    offset += snprintf(p_buf+offset, mlen-offset, "</tmd:GetVideoOutputsResponse>");            

	return offset;
}

int build_GetVideoOutputConfiguration_rly_xml(char * p_buf, int mlen, const char * argv)
{
    int offset = 0;
    GetVideoOutputConfiguration_REQ * p_req = (GetVideoOutputConfiguration_REQ *)argv;
    ONVIF_VideoOutputConfiguration * p_cfg = onvif_find_VideoOutputConfiguration_by_OutputToken(p_req->VideoOutputToken);
    if (NULL == p_cfg)
    {
    	return ONVIF_ERR_NoVideoOutput;
    }

	offset += snprintf(p_buf+offset, mlen-offset, 
	    "<tmd:GetVideoOutputConfigurationResponse>"
	        "<tmd:VideoOutputConfiguration token=\"%s\">"
                "<tt:Name>%s</tt:Name>"
                "<tt:UseCount>%d</tt:UseCount>"
                "<tt:OutputToken>%s</tt:OutputToken>"
            "</tmd:VideoOutputConfiguration>"
	    "<tmd:GetVideoOutputConfigurationResponse>",
	    p_cfg->Configuration.token,
	    p_cfg->Configuration.Name,
	    p_cfg->Configuration.UseCount,
	    p_cfg->Configuration.OutputToken);
	    
    return offset;
}

int build_SetVideoOutputConfiguration_rly_xml(char * p_buf, int mlen, const char * argv)
{
    int offset = 0;
	offset += snprintf(p_buf+offset, mlen-offset, "<tmd:SetVideoOutputConfigurationResponse />");
	return offset;
}

int build_GetVideoOutputConfigurationOptions_rly_xml(char * p_buf, int mlen, const char * argv)
{
    int offset = 0;
    GetVideoOutputConfigurationOptions_REQ * p_req = (GetVideoOutputConfigurationOptions_REQ *)argv;
    ONVIF_VideoOutput * p_output = onvif_find_VideoOutput(p_req->VideoOutputToken);
    if (NULL == p_output)
    {
        return ONVIF_ERR_NoVideoOutput;
    }
    
	offset += snprintf(p_buf+offset, mlen-offset, 
	    "<tmd:GetVideoOutputConfigurationOptionsResponse>"
	        "<tt:VideoOutputConfigurationOptions />"
        "</tmd:GetVideoOutputConfigurationOptionsResponse>");
	return offset;
}

int build_trt_GetAudioOutputs_rly_xml(char * p_buf, int mlen, const char * argv)
{
    int offset = 0;
    ONVIF_AudioOutput * p_output = g_onvif_cfg.a_output;

	offset += snprintf(p_buf+offset, mlen-offset, "<trt:GetAudioOutputsResponse>");

	while (p_output)
	{
	    offset += snprintf(p_buf+offset, mlen-offset, 
	        "<trt:AudioOutputs token=\"%s\" />",
	        p_output->AudioOutput.token);

        p_output = p_output->next;
	}
	        
	offset += snprintf(p_buf+offset, mlen-offset, "</trt:GetAudioOutputsResponse>");

	return offset;
}

int build_AddAudioOutputConfiguration_rly_xml(char * p_buf, int mlen, const char * argv)
{
    int offset = 0;
	offset += snprintf(p_buf+offset, mlen-offset, "<trt:AddAudioOutputConfigurationResponse />");
	return offset;
}

int build_RemoveAudioOutputConfiguration_rly_xml(char * p_buf, int mlen, const char * argv)
{
    int offset = 0;
	offset += snprintf(p_buf+offset, mlen-offset, "<trt:RemoveAudioOutputConfigurationResponse />");
	return offset;
}

int build_GetAudioOutputs_rly_xml(char * p_buf, int mlen, const char * argv)
{
    int offset = 0;
    ONVIF_AudioOutput * p_output = g_onvif_cfg.a_output;

	offset += snprintf(p_buf+offset, mlen-offset, "<tmd:GetAudioOutputsResponse>");

	while (p_output)
	{
	    offset += snprintf(p_buf+offset, mlen-offset, 
	        "<tmd:Token>%s</tmd:Token>",
	        p_output->AudioOutput.token);

        p_output = p_output->next;
	}
	        
	offset += snprintf(p_buf+offset, mlen-offset, "</tmd:GetAudioOutputsResponse>");

	return offset;
}

int build_AudioOutputConfiguration_xml(char * p_buf, int mlen, onvif_AudioOutputConfiguration * p_req)
{
    int offset = 0;
    
    offset += snprintf(p_buf+offset, mlen-offset, 
        "<tt:Name>%s</tt:Name>"
        "<tt:UseCount>%d</tt:UseCount>"
        "<tt:OutputToken>%s</tt:OutputToken>",
        p_req->Name,
        p_req->UseCount,
        p_req->OutputToken);

    if (p_req->SendPrimacyFlag)
    {
        offset += snprintf(p_buf+offset, mlen-offset, 
            "<tt:SendPrimacy>%s</tt:SendPrimacy>",
            p_req->SendPrimacy);
    }
    offset += snprintf(p_buf+offset, mlen-offset, 
        "<tt:OutputLevel>%d</tt:OutputLevel>",
        p_req->OutputLevel);

    return offset;
}

int build_GetAudioOutputConfigurations_rly_xml(char * p_buf, int mlen, const char * argv)
{
    int offset = 0;
    ONVIF_AudioOutputConfiguration * p_cfg = g_onvif_cfg.a_output_cfg;

	offset += snprintf(p_buf+offset, mlen-offset, "<trt:GetAudioOutputConfigurationsResponse>");

    while (p_cfg)
    {
        offset += snprintf(p_buf+offset, mlen-offset, 
            "<trt:Configurations token=\"%s\">", 
            p_cfg->Configuration.token);
        
        offset += build_AudioOutputConfiguration_xml(p_buf+offset, mlen-offset, &p_cfg->Configuration);
        
        offset += snprintf(p_buf+offset, mlen-offset, "</trt:Configurations>");
            
        p_cfg = p_cfg->next;
    }
	
	offset += snprintf(p_buf+offset, mlen-offset, "</trt:GetAudioOutputConfigurationsResponse>");

	return offset;
}

int build_GetCompatibleAudioOutputConfigurations_rly_xml(char * p_buf, int mlen, const char * argv)
{
    int offset = 0;
    GetCompatibleAudioOutputConfigurations_REQ * p_req = (GetCompatibleAudioOutputConfigurations_REQ *)argv;
    ONVIF_AudioOutputConfiguration * p_cfg = g_onvif_cfg.a_output_cfg;
    ONVIF_PROFILE * p_profile = onvif_find_profile(p_req->ProfileToken);
    if (NULL == p_profile)
    {
        return ONVIF_ERR_NoProfile;
    }

	offset += snprintf(p_buf+offset, mlen-offset, "<trt:GetCompatibleAudioOutputConfigurationsResponse>");

    while (p_cfg)
    {
        offset += snprintf(p_buf+offset, mlen-offset, 
            "<trt:Configurations token=\"%s\">", 
            p_cfg->Configuration.token);
        
        offset += build_AudioOutputConfiguration_xml(p_buf+offset, mlen-offset, &p_cfg->Configuration);
        
        offset += snprintf(p_buf+offset, mlen-offset, "</trt:Configurations>");
            
        p_cfg = p_cfg->next;
    }
	
	offset += snprintf(p_buf+offset, mlen-offset, "</trt:GetCompatibleAudioOutputConfigurationsResponse>");

	return offset;
}

int build_trt_GetAudioOutputConfiguration_rly_xml(char * p_buf, int mlen, const char * argv)
{
    int offset = 0;
    trt_GetAudioOutputConfiguration_REQ * p_req = (trt_GetAudioOutputConfiguration_REQ *)argv; 
    ONVIF_AudioOutputConfiguration * p_cfg = onvif_find_AudioOutputConfiguration(p_req->ConfigurationToken);
    if (NULL == p_cfg)
    {
        return ONVIF_ERR_NoConfig;
    }

    offset += snprintf(p_buf+offset, mlen-offset, "<trt:GetAudioOutputConfigurationResponse>");
	offset += snprintf(p_buf+offset, mlen-offset, 
	    "<trt:Configuration token=\"%s\">", 
	    p_cfg->Configuration.token);
    offset += build_AudioOutputConfiguration_xml(p_buf+offset, mlen-offset, &p_cfg->Configuration);
    offset += snprintf(p_buf+offset, mlen-offset, "</trt:Configuration>");

	offset += snprintf(p_buf+offset, mlen-offset, "</trt:GetAudioOutputConfigurationResponse>");

	return offset;
}

int build_GetAudioOutputConfiguration_rly_xml(char * p_buf, int mlen, const char * argv)
{
    int offset = 0;
    GetAudioOutputConfiguration_REQ * p_req = (GetAudioOutputConfiguration_REQ *)argv; 
    ONVIF_AudioOutputConfiguration * p_cfg = onvif_find_AudioOutputConfiguration_by_OutputToken(p_req->AudioOutputToken);
    if (NULL == p_cfg)
    {
        return ONVIF_ERR_NoAudioOutput;
    }

    offset += snprintf(p_buf+offset, mlen-offset, "<tmd:GetAudioOutputConfigurationResponse>");
	offset += snprintf(p_buf+offset, mlen-offset, 
	    "<tmd:AudioOutputConfiguration token=\"%s\">", 
	    p_cfg->Configuration.token);
    offset += build_AudioOutputConfiguration_xml(p_buf+offset, mlen-offset, &p_cfg->Configuration);
    offset += snprintf(p_buf+offset, mlen-offset, "</tmd:AudioOutputConfiguration>");

	offset += snprintf(p_buf+offset, mlen-offset, "</tmd:GetAudioOutputConfigurationResponse>");

	return offset;
}

int build_SetAudioOutputConfiguration_rly_xml(char * p_buf, int mlen, const char * argv)
{
    int offset = 0;
	offset += snprintf(p_buf+offset, mlen-offset, "<trt:SetAudioOutputConfigurationResponse />");
	return offset;
}

int build_AudioOutputConfigurationOptions_xml(char * p_buf, int mlen, onvif_AudioOutputConfigurationOptions * p_req)
{
    int i, offset = 0;
    
    for (i = 0; i < p_req->sizeOutputTokensAvailable; i++)
    {
        offset += snprintf(p_buf+offset, mlen-offset, 
            "<tt:OutputTokensAvailable>%s</tt:OutputTokensAvailable>",
            p_req->OutputTokensAvailable[i]);
    }

    for (i = 0; i < p_req->sizeSendPrimacyOptions; i++)
    {
        offset += snprintf(p_buf+offset, mlen-offset, 
            "<tt:SendPrimacyOptions>%s</tt:SendPrimacyOptions>",
            p_req->SendPrimacyOptions[i]);
    }

    offset += snprintf(p_buf+offset, mlen-offset, 
        "<tt:OutputLevelRange>"
            "<tt:Min>%d</tt:Min>"
            "<tt:Max>%d</tt:Max>"
        "</tt:OutputLevelRange>",
        p_req->OutputLevelRange.Min,
        p_req->OutputLevelRange.Max); 

    return offset;        
}

int build_trt_GetAudioOutputConfigurationOptions_rly_xml(char * p_buf, int mlen, const char * argv)
{
    int offset = 0;
    ONVIF_AudioOutputConfiguration * p_cfg = NULL;
    trt_GetAudioOutputConfigurationOptions_REQ * p_req = (trt_GetAudioOutputConfigurationOptions_REQ *)argv; 
    if (p_req->ConfigurationTokenFlag)
    {
        p_cfg = onvif_find_AudioOutputConfiguration(p_req->ConfigurationToken);
        if (NULL == p_cfg)
        {
            return ONVIF_ERR_NoConfig;
        }
    }

    if (p_req->ProfileTokenFlag)
    {
        ONVIF_PROFILE * p_profile = onvif_find_profile(p_req->ProfileToken);
        if (NULL == p_profile)
        {
            return ONVIF_ERR_NoProfile;
        }
    }

    if (NULL == p_cfg)
    {
        p_cfg = g_onvif_cfg.a_output_cfg;
    }

	offset += snprintf(p_buf+offset, mlen-offset, "<trt:GetAudioOutputConfigurationOptionsResponse>");
	offset += snprintf(p_buf+offset, mlen-offset, "<trt:Options>");

	if (p_cfg)
	{
        offset += build_AudioOutputConfigurationOptions_xml(p_buf+offset, mlen-offset, &p_cfg->Options);
    }
    
	offset += snprintf(p_buf+offset, mlen-offset, "</trt:Options>");
	offset += snprintf(p_buf+offset, mlen-offset, "</trt:GetAudioOutputConfigurationOptionsResponse>");

	return offset;
}

int build_GetAudioOutputConfigurationOptions_rly_xml(char * p_buf, int mlen, const char * argv)
{
    int offset = 0;
    GetAudioOutputConfigurationOptions_REQ * p_req = (GetAudioOutputConfigurationOptions_REQ *)argv; 
    ONVIF_AudioOutputConfiguration * p_cfg = onvif_find_AudioOutputConfiguration_by_OutputToken(p_req->AudioOutputToken);
    if (NULL == p_cfg)
    {
        return ONVIF_ERR_NoAudioOutput;
    }

	offset += snprintf(p_buf+offset, mlen-offset, "<tmd:GetAudioOutputConfigurationOptionsResponse>");
	offset += snprintf(p_buf+offset, mlen-offset, "<tmd:AudioOutputOptions>");

    offset += build_AudioOutputConfigurationOptions_xml(p_buf+offset, mlen-offset, &p_cfg->Options);   
	
	offset += snprintf(p_buf+offset, mlen-offset, "</tmd:AudioOutputOptions>");
	offset += snprintf(p_buf+offset, mlen-offset, "</tmd:GetAudioOutputConfigurationOptionsResponse>");

	return offset;
}

int build_RelayOutput_xml(char * p_buf, int mlen, onvif_RelayOutput * p_req)
{
    int offset = 0;

    offset += snprintf(p_buf+offset, mlen-offset, 
        "<tt:Properties>"
            "<tt:Mode>%s</tt:Mode>"
            "<tt:DelayTime>PT%dS</tt:DelayTime>"
            "<tt:IdleState>%s</tt:IdleState>"
        "</tt:Properties>",
        onvif_RelayModeToString(p_req->Properties.Mode),
        p_req->Properties.DelayTime,
        onvif_RelayIdleStateToString(p_req->Properties.IdleState));

    return offset;        
}

int build_tmd_GetRelayOutputs_rly_xml(char * p_buf, int mlen, const char * argv)
{
    int offset = 0;
    ONVIF_RelayOutput * p_output = g_onvif_cfg.relay_output;
    
	offset += snprintf(p_buf+offset, mlen-offset, "<tmd:GetRelayOutputsResponse>");
	
	while (p_output)
	{
	    offset += snprintf(p_buf+offset, mlen-offset, 
	        "<tmd:RelayOutputs token=\"%s\">",
	        p_output->RelayOutput.token);

	    offset += build_RelayOutput_xml(p_buf+offset, mlen-offset, &p_output->RelayOutput);
	    
	    offset += snprintf(p_buf+offset, mlen-offset, "</tmd:RelayOutputs>");

	    p_output = p_output->next;
    }
    
    offset += snprintf(p_buf+offset, mlen-offset, "</tmd:GetRelayOutputsResponse>");            

	return offset;
}

int build_tds_GetRelayOutputs_rly_xml(char * p_buf, int mlen, const char * argv)
{
    int offset = 0;
    ONVIF_RelayOutput * p_output = g_onvif_cfg.relay_output;
    
	offset += snprintf(p_buf+offset, mlen-offset, "<tds:GetRelayOutputsResponse>");
	
	while (p_output)
	{
	    offset += snprintf(p_buf+offset, mlen-offset, 
	        "<tds:RelayOutputs token=\"%s\">",
	        p_output->RelayOutput.token);

	    offset += build_RelayOutput_xml(p_buf+offset, mlen-offset, &p_output->RelayOutput);
	    
	    offset += snprintf(p_buf+offset, mlen-offset, "</tds:RelayOutputs>");

	    p_output = p_output->next;
    }
    
    offset += snprintf(p_buf+offset, mlen-offset, "</tds:GetRelayOutputsResponse>");            

	return offset;
}

int build_GetRelayOutputOptions_rly_xml(char * p_buf, int mlen, const char * argv)
{
	int offset = 0;
    onvif_RelayOutputOptions * p_option = NULL;
    GetRelayOutputOptions_REQ * p_req = (GetRelayOutputOptions_REQ *)argv;
    
    if (p_req->RelayOutputTokenFlag)
    {
        ONVIF_RelayOutput * p_output = onvif_find_RelayOutput(p_req->RelayOutputToken);
        if (p_output)
        {
            p_option = &p_output->Options;
        }
    }

    if (NULL == p_option)
    {
        p_option = &g_onvif_cfg.RelayOutputOptions;
    }

	offset += snprintf(p_buf+offset, mlen-offset, "<tmd:GetRelayOutputOptionsResponse>");

    offset += snprintf(p_buf+offset, mlen-offset, 
        "<tmd:RelayOutputOptions token=\"%s\">",
        p_option->token);

    if (p_option->RelayMode_MonostableFlag)
    {
        offset += snprintf(p_buf+offset, mlen-offset, "<tmd:Mode>Monostable</tmd:Mode>");
    }
    if (p_option->RelayMode_BistableFlag)
    {
        offset += snprintf(p_buf+offset, mlen-offset, "<tmd:Mode>Bistable</tmd:Mode>");
    }
    if (p_option->DelayTimesFlag)
    {
        offset += snprintf(p_buf+offset, mlen-offset, 
            "<tmd:DelayTimes>%s</tmd:DelayTimes>",
            p_option->DelayTimes);
    }
    if (p_option->DiscreteFlag)
    {
        offset += snprintf(p_buf+offset, mlen-offset, 
            "<tmd:Discrete>%s</tmd:Discrete>",
            p_option->Discrete ? "true" : "false");
    }
    
    offset += snprintf(p_buf+offset, mlen-offset, "</tmd:RelayOutputOptions>");
    
	offset += snprintf(p_buf+offset, mlen-offset, "</tmd:GetRelayOutputOptionsResponse>");            

	return offset;
}

int build_tmd_SetRelayOutputSettings_rly_xml(char * p_buf, int mlen, const char * argv)
{
    int offset = 0;
	offset += snprintf(p_buf+offset, mlen-offset, "<tmd:SetRelayOutputSettingsResponse />");
	return offset;
}

int build_tds_SetRelayOutputSettings_rly_xml(char * p_buf, int mlen, const char * argv)
{
    int offset = 0;
	offset += snprintf(p_buf+offset, mlen-offset, "<tds:SetRelayOutputSettingsResponse />");
	return offset;
}

int build_SetRelayOutputState_rly_xml(char * p_buf, int mlen, const char * argv)
{
    int offset = 0;
	offset += snprintf(p_buf+offset, mlen-offset, "<tds:SetRelayOutputStateResponse />");
	return offset;
}

int build_tmd_SetRelayOutputState_rly_xml(char * p_buf, int mlen, const char * argv)
{
    int offset = 0;
	offset += snprintf(p_buf+offset, mlen-offset, "<tmd:SetRelayOutputStateResponse />");
	return offset;
}

int build_GetDigitalInputs_rly_xml(char * p_buf, int mlen, const char * argv)
{
    int offset = 0;
    ONVIF_DigitalInput * p_input = g_onvif_cfg.digit_input;
    
	offset += snprintf(p_buf+offset, mlen-offset, "<tmd:GetDigitalInputsResponse>");
	
	while (p_input)
	{
	    offset += snprintf(p_buf+offset, mlen-offset, 
	        "<tmd:DigitalInputs token=\"%s\"",
	        p_input->DigitalInput.token);
	        
        if (p_input->DigitalInput.IdleStateFlag)
        {
	        offset += snprintf(p_buf+offset, mlen-offset, 
	            " IdleState=\"%s\"",
	            onvif_DigitalIdleStateToString(p_input->DigitalInput.IdleState));
        }
        
	    offset += snprintf(p_buf+offset, mlen-offset,">");    
	    offset += snprintf(p_buf+offset, mlen-offset, "</tmd:DigitalInputs>");

	    p_input = p_input->next;
    }
    
    offset += snprintf(p_buf+offset, mlen-offset, "</tmd:GetDigitalInputsResponse>");            

	return offset;
}

int build_GetDigitalInputConfigurationOptions_rly_xml(char * p_buf, int mlen, const char * argv)
{
    int offset = 0;
    onvif_DigitalInputConfigurationInputOptions * p_option = NULL;
    GetDigitalInputConfigurationOptions_REQ * p_req = (GetDigitalInputConfigurationOptions_REQ *)argv;
    
    if (p_req->TokenFlag)
    {
        ONVIF_DigitalInput * p_input = onvif_find_DigitalInput(p_req->Token);
        if (p_input)
        {
            p_option = &p_input->Options;
        }
    }

    if (NULL == p_option)
    {
        p_option = &g_onvif_cfg.DigitalInputConfigurationInputOptions;
    }

	offset += snprintf(p_buf+offset, mlen-offset, "<tmd:GetDigitalInputConfigurationOptionsResponse>");
    offset += snprintf(p_buf+offset, mlen-offset, "<tmd:DigitalInputOptions>");

    if (p_option->DigitalIdleState_closedFlag)
    {
        offset += snprintf(p_buf+offset, mlen-offset, "<tmd:IdleState>closed</tmd:IdleState>");
    }
    if (p_option->DigitalIdleState_openFlag)
    {
        offset += snprintf(p_buf+offset, mlen-offset, "<tmd:IdleState>open</tmd:IdleState>");
    }    
    
    offset += snprintf(p_buf+offset, mlen-offset, "</tmd:DigitalInputOptions>");    
	offset += snprintf(p_buf+offset, mlen-offset, "</tmd:GetDigitalInputConfigurationOptionsResponse>");            

	return offset;
}

int build_SetDigitalInputConfigurations_rly_xml(char * p_buf, int mlen, const char * argv)
{
    int offset = 0;
	offset += snprintf(p_buf+offset, mlen-offset, "<tmd:SetDigitalInputConfigurationsResponse />");
	return offset;
}

int build_GetSerialPorts_rly_xml(char * p_buf, int mlen, const char * argv)
{
    int offset = 0;
    ONVIF_SerialPort * p_port = g_onvif_cfg.serial_port;
    
	offset += snprintf(p_buf+offset, mlen-offset, "<tmd:GetSerialPortsResponse>");
	
	while (p_port)
	{
	    offset += snprintf(p_buf+offset, mlen-offset, 
	        "<tmd:SerialPort token=\"%s\"></tmd:SerialPort>",
	        p_port->SerialPort.token);

	    p_port = p_port->next;
    }
    
    offset += snprintf(p_buf+offset, mlen-offset, "</tmd:GetSerialPortsResponse>");            

	return offset;
}

int build_GetSerialPortConfiguration_rly_xml(char * p_buf, int mlen, const char * argv)
{
    int offset = 0;
    GetSerialPortConfiguration_REQ * p_req = (GetSerialPortConfiguration_REQ *)argv;

    ONVIF_SerialPort * p_port = onvif_find_SerialPort(p_req->SerialPortToken);
    if (NULL == p_port)
    {
        return ONVIF_ERR_InvalidSerialPort;
    }

    offset += snprintf(p_buf+offset, mlen-offset,
        "<tmd:GetSerialPortConfigurationResponse>"
            "<tmd:SerialPortConfiguration token=\"%s\" type=\"%s\">"
                "<tmd:BaudRate>%d</tmd:BaudRate>"
                "<tmd:ParityBit>%s</tmd:ParityBit>"
                "<tmd:CharacterLength>%d</tmd:CharacterLength>"
                "<tmd:StopBit>%0.1f</tmd:StopBit>"
                "</tmd:SerialPortConfiguration>"
            "</tmd:SerialPortConfiguration>"
        "</tmd:GetSerialPortConfigurationResponse>",
        p_port->Configuration.token,
        onvif_SerialPortTypeToString(p_port->Configuration.type),
        p_port->Configuration.BaudRate,
        onvif_ParityBitToString(p_port->Configuration.ParityBit),
        p_port->Configuration.CharacterLength,
        p_port->Configuration.StopBit);

	return offset;    
}

int build_ParityBitList_xml(char * p_buf, int mlen, onvif_ParityBitList * p_req)
{
    int i, offset = 0;

    for (i = 0; i < p_req->sizeItems; i++)
    {
        offset += snprintf(p_buf+offset, mlen-offset,
            "<tt:Items>%s</tt:Items>", onvif_ParityBitToString(p_req->Items[i]));
    }       

    return offset;
}

int build_GetSerialPortConfigurationOptions_rly_xml(char * p_buf, int mlen, const char * argv)
{
    int offset = 0;
    GetSerialPortConfigurationOptions_REQ * p_req = (GetSerialPortConfigurationOptions_REQ *)argv;

    ONVIF_SerialPort * p_port = onvif_find_SerialPort(p_req->SerialPortToken);
    if (NULL == p_port)
    {
        return ONVIF_ERR_InvalidSerialPort;
    }

    offset += snprintf(p_buf+offset, mlen-offset,
        "<tmd:GetSerialPortConfigurationOptionsResponse>"
            "<tmd:SerialPortOptions token=\"%s\">",
        p_port->Options.token);

    offset += snprintf(p_buf+offset, mlen-offset, "<tmd:BaudRateList>");
    offset += build_IntList_xml(p_buf+offset, mlen-offset, &p_port->Options.BaudRateList);
    offset += snprintf(p_buf+offset, mlen-offset, "</tmd:BaudRateList>");

    offset += snprintf(p_buf+offset, mlen-offset, "<tmd:ParityBitList>");
    offset += build_ParityBitList_xml(p_buf+offset, mlen-offset, &p_port->Options.ParityBitList);
    offset += snprintf(p_buf+offset, mlen-offset, "</tmd:ParityBitList>");

    offset += snprintf(p_buf+offset, mlen-offset, "<tmd:CharacterLengthList>");
    offset += build_IntList_xml(p_buf+offset, mlen-offset, &p_port->Options.CharacterLengthList);
    offset += snprintf(p_buf+offset, mlen-offset, "</tmd:CharacterLengthList>");

    offset += snprintf(p_buf+offset, mlen-offset, "<tmd:StopBitList>");
    offset += build_FloatList_xml(p_buf+offset, mlen-offset, &p_port->Options.StopBitList);
    offset += snprintf(p_buf+offset, mlen-offset, "</tmd:StopBitList>");
    
    offset += snprintf(p_buf+offset, mlen-offset,            
            "</tmd:SerialPortOptions>"
        "</tmd:GetSerialPortConfigurationOptionsResponse>");        

	return offset; 
}

int build_SetSerialPortConfiguration_rly_xml(char * p_buf, int mlen, const char * argv)
{
    int offset = 0;
	offset += snprintf(p_buf+offset, mlen-offset, "<tmd:SetSerialPortConfigurationResponse />");
	return offset;
}

int build_SendReceiveSerialCommand_rly_xml(char * p_buf, int mlen, const char * argv)
{
    int offset = 0;
    SendReceiveSerialCommand_RES * p_res = (SendReceiveSerialCommand_RES *)argv;

	offset += snprintf(p_buf+offset, mlen-offset, "<tmd:SendReceiveSerialCommandResponse>");

	if (p_res->SerialDataFlag)
	{
	    offset += snprintf(p_buf+offset, mlen-offset, "<tmd:SerialData>");
	    if (p_res->SerialData._union_SerialData == 0)
	    {
	        offset += snprintf(p_buf+offset, mlen-offset, 
	            "<tmd:Binary>%s</tmd:Binary>",
	            p_res->SerialData.union_SerialData.Binary);
	    }
	    else
	    {
	        offset += snprintf(p_buf+offset, mlen-offset, 
	            "<tmd:String>%s</tmd:String>",
	            p_res->SerialData.union_SerialData.String);
	    }
	    offset += snprintf(p_buf+offset, mlen-offset, "</tmd:SerialData>");
	}

	offset += snprintf(p_buf+offset, mlen-offset, "</tmd:SendReceiveSerialCommandResponse>");

	return offset;
}

int build_tmd_GetAudioSources_rly_xml(char * p_buf, int mlen, const char * argv)
{
    int offset = 0;
    
#ifdef AUDIO_SUPPORT    
    ONVIF_AudioSource * p_a_src = g_onvif_cfg.a_src;
    
	offset += snprintf(p_buf+offset, mlen-offset, "<tmd:GetAudioSourcesResponse>");
	
	while (p_a_src)
	{
	    offset += snprintf(p_buf+offset, mlen-offset, 
	    	"<tmd:Token>%s</tmd:Token>", 
	    	p_a_src->AudioSource.token);
	    
	    p_a_src = p_a_src->next;
	}
	
	offset += snprintf(p_buf+offset, mlen-offset, "</tmd:GetAudioSourcesResponse>");
#else
    offset += snprintf(p_buf+offset, mlen-offset, "<tmd:GetAudioSourcesResponse>");
    offset += snprintf(p_buf+offset, mlen-offset, "</tmd:GetAudioSourcesResponse>");
#endif

	return offset;
}

#endif // end of DEVICEIO_SUPPORT

#ifdef MEDIA2_SUPPORT

BOOL find_ConfigurationType(tr2_GetProfiles_REQ * p_req, const char * type)
{
    int i;

    for (i = 0; i < p_req->sizeType; i++)
    {
        if (strcasecmp(p_req->Type[i], type) == 0 || 
            strcasecmp(p_req->Type[i], "all") == 0)
        {
            return TRUE;
        }
    }

    return FALSE;
}

int build_ColorspaceRange_xml(char * p_buf, int mlen, onvif_ColorspaceRange * p_req)
{
    int offset = 0;

    offset += snprintf(p_buf+offset, mlen-offset, 
        "<tt:X>"
            "<tt:Min>%0.2f</tt:Min>"
            "<tt:Max>%0.2f</tt:Max>"
        "</tt:X>"
        "<tt:Y>"
            "<tt:Min>%0.2f</tt:Min>"
            "<tt:Max>%0.2f</tt:Max>"
        "</tt:Y>"
        "<tt:Z>"
            "<tt:Min>%0.2f</tt:Min>"
            "<tt:Max>%0.2f</tt:Max>"
        "</tt:Z>"
        "<tt:Colorspace>%s</tt:Colorspace>",
        p_req->X.Min, p_req->X.Max,
        p_req->Y.Min, p_req->Y.Max,
        p_req->Z.Min, p_req->Z.Max,
        p_req->Colorspace);

    return offset;

}

int build_ColorOptions_xml(char * p_buf, int mlen, onvif_ColorOptions * p_req)
{
    int i;
    int offset = 0;

    for (i = 0; i < p_req->sizeColorList; i++)
    {
        offset += snprintf(p_buf+offset, mlen-offset, 
            "<tt:ColorList X=\"%0.2f\" Y=\"%0.2f\" Z=\"%0.2f\"", 
            p_req->ColorList[i].X, p_req->ColorList[i].Y, p_req->ColorList[i].Z);

        if (p_req->ColorList[i].ColorspaceFlag)
        {
            offset += snprintf(p_buf+offset, mlen-offset, 
                " Colorspace=\"%s\"", p_req->ColorList[i].Colorspace);
        }

        offset += snprintf(p_buf+offset, mlen-offset, " />");
    }

    for (i = 0; i < p_req->sizeColorspaceRange; i++)
    {
        offset += snprintf(p_buf+offset, mlen-offset, "<tt:ColorspaceRange>");
        offset += build_ColorspaceRange_xml(p_buf+offset, mlen-offset, &p_req->ColorspaceRange[i]);
        offset += snprintf(p_buf+offset, mlen-offset, "</tt:ColorspaceRange>");
    }

    return offset;
}

int build_OSDColorOptions_xml(char * p_buf, int mlen, onvif_OSDColorOptions * p_req)
{
    int offset = 0;

    if (p_req->ColorFlag)
    {
        offset += snprintf(p_buf+offset, mlen-offset, "<tt:Color>");
        offset += build_ColorOptions_xml(p_buf+offset, mlen-offset, &p_req->Color);
        offset += snprintf(p_buf+offset, mlen-offset, "</tt:Color>");
    }

    if (p_req->TransparentFlag)
    {
        offset += snprintf(p_buf+offset, mlen-offset, 
			"<tt:Transparent>"
				"<tt:Min>%d</tt:Min>"				
				"<tt:Max>%d</tt:Max>"				
			"</tt:Transparent>", 
			p_req->Transparent.Min,
			p_req->Transparent.Max);
    }

    return offset;    
}

int build_OSDTextOptions_xml(char * p_buf, int mlen, onvif_OSDTextOptions * p_req)
{
    int i;
    int offset = 0;
    
    offset += snprintf(p_buf+offset, mlen-offset, "<tt:TextOption>");
		
	if (p_req->OSDTextType_Plain)
	{
		offset += snprintf(p_buf+offset, mlen-offset, "<tt:Type>%s</tt:Type>", 
			onvif_OSDTextTypeToString(OSDTextType_Plain));
	}
	if (p_req->OSDTextType_Date)
	{
		offset += snprintf(p_buf+offset, mlen-offset, "<tt:Type>%s</tt:Type>", 
			onvif_OSDTextTypeToString(OSDTextType_Date));
	}
	if (p_req->OSDTextType_Time)
	{
		offset += snprintf(p_buf+offset, mlen-offset, "<tt:Type>%s</tt:Type>", 
			onvif_OSDTextTypeToString(OSDTextType_Time));
	}
	if (p_req->OSDTextType_DateAndTime)
	{
		offset += snprintf(p_buf+offset, mlen-offset, "<tt:Type>%s</tt:Type>", 
			onvif_OSDTextTypeToString(OSDTextType_DateAndTime));
	}

	if (p_req->FontSizeRangeFlag)
	{
		offset += snprintf(p_buf+offset, mlen-offset, 
			"<tt:FontSizeRange>"
				"<tt:Min>%d</tt:Min>"				
				"<tt:Max>%d</tt:Max>"				
			"</tt:FontSizeRange>", 
			p_req->FontSizeRange.Min,
			p_req->FontSizeRange.Max);
	}

	for (i = 0; i < p_req->DateFormatSize; i++)
	{
		offset += snprintf(p_buf+offset, mlen-offset, 
		    "<tt:DateFormat>%s</tt:DateFormat>",
			p_req->DateFormat[i]);
	}
	
	for (i = 0; i < p_req->TimeFormatSize; i++)
	{
		offset += snprintf(p_buf+offset, mlen-offset, 
		    "<tt:TimeFormat>%s</tt:TimeFormat>",
			p_req->TimeFormat[i]);
	}

	if (p_req->FontColorFlag)
	{
	    offset += snprintf(p_buf+offset, mlen-offset, "<tt:FontColor>");
	    offset += build_OSDColorOptions_xml(p_buf+offset, mlen-offset, &p_req->FontColor);
	    offset += snprintf(p_buf+offset, mlen-offset, "</tt:FontColor>");
	}

	if (p_req->BackgroundColorFlag)
	{
	    offset += snprintf(p_buf+offset, mlen-offset, "<tt:BackgroundColor>");
	    offset += build_OSDColorOptions_xml(p_buf+offset, mlen-offset, &p_req->BackgroundColor);
	    offset += snprintf(p_buf+offset, mlen-offset, "</tt:BackgroundColor>");
	}

	offset += snprintf(p_buf+offset, mlen-offset, "</tt:TextOption>");

	return offset;
}

int build_OSDImgOptions_xml(char * p_buf, int mlen, onvif_OSDImgOptions * p_req)
{
    int i;
    int offset = 0;
    
    offset += snprintf(p_buf+offset, mlen-offset, "<tt:ImageOption>");

	for (i = 0; i < p_req->ImagePathSize; i++)
	{
		offset += snprintf(p_buf+offset, mlen-offset, 
		    "<tt:ImagePath>%s</tt:ImagePath>",
			p_req->ImagePath[i]);
	}
	
	offset += snprintf(p_buf+offset, mlen-offset, "</tt:ImageOption>");

	return offset;
}

int build_Polygon_xml(char * p_buf, int mlen, onvif_Polygon * p_req)
{
    int i;
    int offset = 0;

    offset += snprintf(p_buf+offset, mlen-offset, "<tt:Polygon>");

    for (i = 0; i < p_req->sizePoint; i++)
    {
        offset += snprintf(p_buf+offset, mlen-offset, 
            "<tt:Point x=\"%0.2f\" y=\"%0.2f\" />",
            p_req->Point[i].x, p_req->Point[i].y);
    }

    offset += snprintf(p_buf+offset, mlen-offset, "</tt:Polygon>");
    
    return offset;
}

int build_Color_xml(char * p_buf, int mlen, onvif_Color * p_req)
{
    int offset = 0;

    offset += snprintf(p_buf+offset, mlen-offset, 
        "<tt:Color X=\"%0.2f\" Y=\"%0.2f\" Z=\"%0.2f\"", 
        p_req->X, p_req->Y, p_req->Z);

    if (p_req->ColorspaceFlag)
    {
        offset += snprintf(p_buf+offset, mlen-offset, 
            " Colorspace=\"%s\"", p_req->Colorspace);
    }

    offset += snprintf(p_buf+offset, mlen-offset, " />");

    return offset;
}

int build_Mask_xml(char * p_buf, int mlen, onvif_Mask * p_req)
{
    int offset = 0;

    offset += snprintf(p_buf+offset, mlen-offset, 
        "<tr2:ConfigurationToken>%s</tr2:ConfigurationToken>",
        p_req->ConfigurationToken);

    offset += build_Polygon_xml(p_buf+offset, mlen-offset, &p_req->Polygon);

    offset += snprintf(p_buf+offset, mlen-offset, "<tr2:Type>%s</tr2:Type>", p_req->Type);

    if (p_req->ColorFlag)
    {
        offset += build_Color_xml(p_buf+offset, mlen-offset, &p_req->Color);
    }
    
    offset += snprintf(p_buf+offset, mlen-offset, 
        "<tr2:Enabled>%s</tr2:Enabled>", 
        p_req->Enabled ? "true" : "false");
    
	return offset;    
}

int build_VideoEncoder2Configuration_xml(char * p_buf, int mlen, onvif_VideoEncoder2Configuration * p_req)
{
    int offset = 0;
    
    offset += snprintf(p_buf+offset, mlen-offset, 
		"<tt:Name>%s</tt:Name>"
		"<tt:UseCount>%d</tt:UseCount>"
	    "<tt:Encoding>%s</tt:Encoding>"
	    "<tt:Resolution>"
	    	"<tt:Width>%d</tt:Width>"
	    	"<tt:Height>%d</tt:Height>"
	    "</tt:Resolution>",
	    p_req->Name, 
	    p_req->UseCount, 
	    p_req->Encoding, 
	    p_req->Resolution.Width, 
	    p_req->Resolution.Height);

	if (p_req->RateControlFlag)
	{
	    offset += snprintf(p_buf+offset, mlen-offset, "<tt:RateControl");
	    if (p_req->RateControl.ConstantBitRateFlag)
	    {
	        offset += snprintf(p_buf+offset, mlen-offset, " ConstantBitRate=\"%s\"",
	            p_req->RateControl.ConstantBitRate ? "true" : "false");
	    }
	    offset += snprintf(p_buf+offset, mlen-offset, ">");
		    
		offset += snprintf(p_buf+offset, mlen-offset,  
		    	"<tt:FrameRateLimit>%0.1f</tt:FrameRateLimit>"
		    	"<tt:BitrateLimit>%d</tt:BitrateLimit>"
		    "</tt:RateControl>",		    
		    p_req->RateControl.FrameRateLimit,
		    p_req->RateControl.BitrateLimit);
	}

    if (p_req->MulticastFlag)
    {
	    offset += build_MulticastConfiguration_xml(p_buf+offset, mlen-offset, &p_req->Multicast);
	}

	offset += snprintf(p_buf+offset, mlen-offset, "<tt:Quality>%0.2f</tt:Quality>", p_req->Quality);

	return offset;  
}

int build_VideoEncoder2ConfigurationOptions_xml(char * p_buf, int mlen, onvif_VideoEncoder2ConfigurationOptions * p_req)
{
    int i;
    int offset = 0;

    offset += snprintf(p_buf+offset, mlen-offset, 
        "<tt:Encoding>%s</tt:Encoding>"
		"<tt:QualityRange>"
			"<tt:Min>%d</tt:Min>"
			"<tt:Max>%d</tt:Max>"
		"</tt:QualityRange>",
		p_req->Encoding,
		p_req->QualityRange.Min, 
		p_req->QualityRange.Max);
    
    for (i = 0; i < ARRAY_SIZE(p_req->ResolutionsAvailable); i++)
    {
        if (p_req->ResolutionsAvailable[i].Width == 0 || p_req->ResolutionsAvailable[i].Height == 0)
        {
            continue;
        }
        
        offset += build_VideoResolution_xml(p_buf+offset, mlen-offset, &p_req->ResolutionsAvailable[i]);
    }
	
	offset += snprintf(p_buf+offset, mlen-offset, 
		"<tt:BitrateRange>"
			"<tt:Min>%d</tt:Min>"
			"<tt:Max>%d</tt:Max>"
		"</tt:BitrateRange>",
		p_req->BitrateRange.Min, 
		p_req->BitrateRange.Max);

    return offset;
}

int build_AudioEncoder2Configuration_xml(char * p_buf, int mlen, onvif_AudioEncoder2Configuration * p_req)
{
    int offset = 0;
	
	offset += snprintf(p_buf+offset, mlen-offset, 
		"<tt:Name>%s</tt:Name>"
		"<tt:UseCount>%d</tt:UseCount>"
		"<tt:Encoding>%s</tt:Encoding>"
		"<tt:Bitrate>%d</tt:Bitrate>"
		"<tt:SampleRate>%d</tt:SampleRate>", 
		p_req->Name, 
    	p_req->UseCount, 
    	p_req->Encoding, 
	    p_req->Bitrate, 
	    p_req->SampleRate); 

    if (p_req->MulticastFlag)
    {
	    offset += build_MulticastConfiguration_xml(p_buf+offset, mlen-offset, &p_req->Multicast);
    }
		
	return offset;   
}

int build_AudioEncoder2ConfigurationOptions_xml(char * p_buf, int mlen, onvif_AudioEncoder2ConfigurationOptions * p_req)
{
    int offset = 0;

    offset += snprintf(p_buf+offset, mlen-offset, 
        "<tt:Encoding>%s</tt:Encoding>",	
		p_req->Encoding);
    
    offset += snprintf(p_buf+offset, mlen-offset, "<tt:BitrateList>");
	offset += build_IntList_xml(p_buf+offset, mlen-offset, &p_req->BitrateList);
	offset += snprintf(p_buf+offset, mlen-offset, "</tt:BitrateList>");
	
	offset += snprintf(p_buf+offset, mlen-offset, "<tt:SampleRateList>");
	offset += build_IntList_xml(p_buf+offset, mlen-offset, &p_req->SampleRateList);
	offset += snprintf(p_buf+offset, mlen-offset, "</tt:SampleRateList>");

    return offset;
}

int build_tr2_GetVideoEncoderConfigurations_rly_xml(char * p_buf, int mlen, const char * argv)
{
    BOOL loopflag = 0;
    int offset = 0;
    tr2_GetVideoEncoderConfigurations_REQ * p_req = (tr2_GetVideoEncoderConfigurations_REQ *)argv;
    ONVIF_VideoEncoder2Configuration * p_v_enc_cfg = NULL;

    if (p_req->GetConfiguration.ConfigurationTokenFlag)
    {
        p_v_enc_cfg = onvif_find_VideoEncoderConfiguration(p_req->GetConfiguration.ConfigurationToken);
        if (NULL == p_v_enc_cfg)
        {
            return ONVIF_ERR_NoConfig;
        }
    }

    if (p_req->GetConfiguration.ProfileTokenFlag)
    {
        ONVIF_PROFILE * p_profile = onvif_find_profile(p_req->GetConfiguration.ProfileToken);
        if (NULL == p_profile)
        {
            return ONVIF_ERR_NoProfile;
        }
    }

    if (NULL == p_v_enc_cfg)
    {
        loopflag = TRUE;
        p_v_enc_cfg = g_onvif_cfg.v_enc_cfg;
    }

	offset += snprintf(p_buf+offset, mlen-offset, "<tr2:GetVideoEncoderConfigurationsResponse>");

    while (p_v_enc_cfg)
	{
	    offset += snprintf(p_buf+offset, mlen-offset, "<tr2:Configurations token=\"%s\"", p_v_enc_cfg->Configuration.token);
        if (p_v_enc_cfg->Configuration.GovLengthFlag)
        {
            offset += snprintf(p_buf+offset, mlen-offset, " GovLength=\"%d\"", p_v_enc_cfg->Configuration.GovLength);
        }
	    if (p_v_enc_cfg->Configuration.ProfileFlag)
        {
            offset += snprintf(p_buf+offset, mlen-offset, " Profile=\"%s\"", p_v_enc_cfg->Configuration.Profile);
        }
	    offset += snprintf(p_buf+offset, mlen-offset, ">");
    	offset += build_VideoEncoder2Configuration_xml(p_buf+offset, mlen-offset, &p_v_enc_cfg->Configuration);
    	offset += snprintf(p_buf+offset, mlen-offset, "</tr2:Configurations>");

    	if (loopflag)
    	{
    	    p_v_enc_cfg = p_v_enc_cfg->next;
    	}
    	else
    	{
    	    break;
    	}
	}
	
	offset += snprintf(p_buf+offset, mlen-offset, "</tr2:GetVideoEncoderConfigurationsResponse>");
    
    return offset;
}

int build_tr2_GetVideoEncoderConfigurationOptions_rly_xml(char * p_buf, int mlen, const char * argv)
{
    int offset = 0;
    tr2_GetVideoEncoderConfigurationOptions_REQ * p_req = (tr2_GetVideoEncoderConfigurationOptions_REQ *)argv;
    ONVIF_VideoEncoder2Configuration * p_v_enc_cfg = NULL;
    ONVIF_VideoEncoder2ConfigurationOptions * p_option;
    
    if (p_req->GetConfiguration.ConfigurationTokenFlag)
    {
        p_v_enc_cfg = onvif_find_VideoEncoderConfiguration(p_req->GetConfiguration.ConfigurationToken);
        if (NULL == p_v_enc_cfg)
        {
            return ONVIF_ERR_NoConfig;
        }
    }

    if (p_req->GetConfiguration.ProfileTokenFlag)
    {
        ONVIF_PROFILE * p_profile = onvif_find_profile(p_req->GetConfiguration.ProfileToken);
        if (NULL == p_profile)
        {
            return ONVIF_ERR_NoProfile;
        }
    }

    p_option = g_onvif_cfg.v_enc_cfg_opt;

	offset += snprintf(p_buf+offset, mlen-offset, "<tr2:GetVideoEncoderConfigurationOptionsResponse>");

    while (p_option)
	{
	    offset += snprintf(p_buf+offset, mlen-offset, "<tr2:Options");
        if (p_option->Options.GovLengthRangeFlag)
        {
            offset += snprintf(p_buf+offset, mlen-offset, " GovLengthRange=\"%s\"", 
                p_option->Options.GovLengthRange);
        }
	    if (p_option->Options.FrameRatesSupportedFlag)
        {
            offset += snprintf(p_buf+offset, mlen-offset, " FrameRatesSupported=\"%s\"", 
                p_option->Options.FrameRatesSupported);
        }
        if (p_option->Options.ProfilesSupportedFlag)
        {
            offset += snprintf(p_buf+offset, mlen-offset, " ProfilesSupported=\"%s\"", 
                p_option->Options.ProfilesSupported);
        }
        if (p_option->Options.ConstantBitRateSupportedFlag)
        {
            offset += snprintf(p_buf+offset, mlen-offset, " ConstantBitRateSupported=\"%s\"", 
                p_option->Options.ConstantBitRateSupported ? "true" : "false");
        }
	    offset += snprintf(p_buf+offset, mlen-offset, ">");
    	offset += build_VideoEncoder2ConfigurationOptions_xml(p_buf+offset, mlen-offset, &p_option->Options);
    	offset += snprintf(p_buf+offset, mlen-offset, "</tr2:Options>");

	    p_option = p_option->next;
	}
	
	offset += snprintf(p_buf+offset, mlen-offset, "</tr2:GetVideoEncoderConfigurationOptionsResponse>");
    
    return offset;
}

int build_tr2_SetVideoEncoderConfiguration_rly_xml(char * p_buf, int mlen, const char * argv)
{
    int offset = 0;
	offset += snprintf(p_buf+offset, mlen-offset, "<tr2:SetVideoEncoderConfigurationResponse />");		    
	return offset;
}

int build_tr2_CreateProfile_rly_xml(char * p_buf, int mlen, const char * argv)
{
    int offset = 0;
    tr2_CreateProfile_RES * p_res = (tr2_CreateProfile_RES *)argv;

	offset += snprintf(p_buf+offset, mlen-offset, 
	    "<tr2:CreateProfileResponse>"
	        "<tr2:Token>%s</tr2:Token>"
	    "</tr2:CreateProfileResponse>",
	    p_res->Token);		    
	
	return offset;
}

int build_tr2_Profile_xml(char * p_buf, int mlen, tr2_GetProfiles_REQ * p_req, ONVIF_PROFILE * p_profile)
{
    int offset = 0;
	
	if (p_profile->v_src_cfg && find_ConfigurationType(p_req, "VideoSource"))
    {
        offset += snprintf(p_buf+offset, mlen-offset, 
        	"<tr2:VideoSource token=\"%s\">", 
            p_profile->v_src_cfg->Configuration.token);            
        offset += build_VideoSourceConfiguration_xml(p_buf+offset, mlen-offset, &p_profile->v_src_cfg->Configuration);    
        offset += snprintf(p_buf+offset, mlen-offset, "</tr2:VideoSource>");	            
    }

#ifdef AUDIO_SUPPORT
    if (p_profile->a_src_cfg && find_ConfigurationType(p_req, "AudioSource"))
    {
        offset += snprintf(p_buf+offset, mlen-offset, 
            "<tr2:AudioSource token=\"%s\">",
            p_profile->a_src_cfg->Configuration.token);
        offset += build_AudioSourceConfiguration_xml(p_buf+offset, mlen-offset, &p_profile->a_src_cfg->Configuration);
        offset += snprintf(p_buf+offset, mlen-offset, "</tr2:AudioSource>");	            
    }
#endif

    if (p_profile->v_enc_cfg && find_ConfigurationType(p_req, "VideoEncoder"))
    {
        offset += snprintf(p_buf+offset, mlen-offset, 
        	"<tr2:VideoEncoder token=\"%s\"", p_profile->v_enc_cfg->Configuration.token);
        if (p_profile->v_enc_cfg->Configuration.GovLengthFlag)
        {
            offset += snprintf(p_buf+offset, mlen-offset, 
        	    " GovLength=\"%d\"", p_profile->v_enc_cfg->Configuration.GovLength);
        }
        if (p_profile->v_enc_cfg->Configuration.ProfileFlag)
        {
            offset += snprintf(p_buf+offset, mlen-offset, 
        	    " Profile=\"%s\"", onvif_MediaProfile2Media2Profile(p_profile->v_enc_cfg->Configuration.Profile));
        }
        offset += snprintf(p_buf+offset, mlen-offset, ">");
		offset += build_VideoEncoder2Configuration_xml(p_buf+offset, mlen-offset, &p_profile->v_enc_cfg->Configuration);        	    
        offset += snprintf(p_buf+offset, mlen-offset, "</tr2:VideoEncoder>");	            
    }

#ifdef AUDIO_SUPPORT
    if (p_profile->a_enc_cfg && find_ConfigurationType(p_req, "AudioEncoder"))
    {
        offset += snprintf(p_buf+offset, mlen-offset, 
            "<tr2:AudioEncoder token=\"%s\">", 
            p_profile->a_enc_cfg->Configuration.token);
		offset += build_AudioEncoder2Configuration_xml(p_buf+offset, mlen-offset, &p_profile->a_enc_cfg->Configuration);
        offset += snprintf(p_buf+offset, mlen-offset, "</tr2:AudioEncoder>");	            
    }
#endif

#ifdef VIDEO_ANALYTICS
    if (p_profile->va_cfg && find_ConfigurationType(p_req, "Analytics"))
    {
        offset += snprintf(p_buf+offset, mlen-offset, "<tr2:Analytics token=\"%s\">", p_profile->va_cfg->Configuration.token);
		offset += build_VideoAnalyticsConfiguration_xml(p_buf+offset, mlen-offset, &p_profile->va_cfg->Configuration);
		offset += snprintf(p_buf+offset, mlen-offset, "</tr2:Analytics>");
    }
#endif

#ifdef PTZ_SUPPORT
    if (p_profile->ptz_cfg && find_ConfigurationType(p_req, "PTZ"))
    {
        offset += snprintf(p_buf+offset, mlen-offset, 
            "<tr2:PTZ token=\"%s\" MoveRamp=\"%d\" PresetRamp=\"%d\" PresetTourRamp=\"%d\">", 
            p_profile->ptz_cfg->Configuration.token, p_profile->ptz_cfg->Configuration.MoveRamp,
            p_profile->ptz_cfg->Configuration.PresetRamp, p_profile->ptz_cfg->Configuration.PresetTourRamp);                       
    	offset += build_PTZConfiguration_xml(p_buf+offset, mlen-offset, p_profile->ptz_cfg);
    	offset += snprintf(p_buf+offset, mlen-offset, "</tr2:PTZ>");
    }
#endif

    if (p_profile->metadata_cfg && find_ConfigurationType(p_req, "Metadata"))
    {
    	offset += snprintf(p_buf+offset, mlen-offset, 
            "<tr2:Metadata token=\"%s\">", 
            p_profile->metadata_cfg->Configuration.token);
        offset += build_MetadataConfiguration_xml(p_buf+offset, mlen-offset, &p_profile->metadata_cfg->Configuration);
        offset += snprintf(p_buf+offset, mlen-offset, "</tr2:Metadata>");    
    }

#ifdef DEVICEIO_SUPPORT
    if (p_profile->a_output_cfg && find_ConfigurationType(p_req, "AudioOutput"))
    {
        offset += snprintf(p_buf+offset, mlen-offset, 
            "<tr2:AudioOutput token=\"%s\">", 
            p_profile->a_output_cfg->Configuration.token);
        offset += build_AudioOutputConfiguration_xml(p_buf+offset, mlen-offset, &p_profile->a_output_cfg->Configuration);
        offset += snprintf(p_buf+offset, mlen-offset, "</tr2:AudioOutput>");
    }
#endif

#ifdef AUDIO_SUPPORT
    if (p_profile->a_dec_cfg && find_ConfigurationType(p_req, "AudioDecoder"))
    {
        offset += snprintf(p_buf+offset, mlen-offset, 
            "<tr2:AudioDecoder token=\"%s\">", 
            p_profile->a_dec_cfg->Configuration.token);
        offset += build_AudioDecoderConfiguration_xml(p_buf+offset, mlen-offset, &p_profile->a_dec_cfg->Configuration);
        offset += snprintf(p_buf+offset, mlen-offset, "</tr2:AudioDecoder>");
    }
#endif    
    
    return offset;
}

int build_tr2_GetProfiles_rly_xml(char * p_buf, int mlen, const char * argv)
{
    BOOL loopflag = 0;
    int offset = 0;
    tr2_GetProfiles_REQ * p_req = (tr2_GetProfiles_REQ *)argv;
    ONVIF_PROFILE * p_profile = NULL;

    if (p_req->TokenFlag)
    {
        p_profile = onvif_find_profile(p_req->Token);
        if (NULL == p_profile)
        {
            return ONVIF_ERR_NoProfile;
        }
    }

    if (NULL == p_profile)
    {
        loopflag = TRUE;
        p_profile = g_onvif_cfg.profiles;
    }

	offset += snprintf(p_buf+offset, mlen-offset, "<tr2:GetProfilesResponse>");

    while (p_profile)
	{
	    offset += snprintf(p_buf+offset, mlen-offset, 
	        "<tr2:Profiles token=\"%s\" fixed=\"%s\"><tr2:Name>%s</tr2:Name><tr2:Configurations>",
	        p_profile->token, p_profile->fixed ? "true" : "false", p_profile->name);

	    offset += build_tr2_Profile_xml(p_buf+offset, mlen-offset, p_req, p_profile);
	    
	    offset += snprintf(p_buf+offset, mlen-offset, "</tr2:Configurations></tr2:Profiles>");

    	if (loopflag)
    	{
    	    p_profile = p_profile->next;
    	}
    	else
    	{
    	    break;
    	}
	}    

	offset += snprintf(p_buf+offset, mlen-offset, "</tr2:GetProfilesResponse>");
    
    return offset;
}

int build_tr2_DeleteProfile_rly_xml(char * p_buf, int mlen, const char * argv)
{
    int offset = 0;
	offset += snprintf(p_buf+offset, mlen-offset, "<tr2:DeleteProfileResponse />");		    
	return offset;
}

int build_tr2_AddConfiguration_rly_xml(char * p_buf, int mlen, const char * argv)
{
    int offset = 0;
	offset += snprintf(p_buf+offset, mlen-offset, "<tr2:AddConfigurationResponse />");		    
	return offset;
}

int build_tr2_RemoveConfiguration_rly_xml(char * p_buf, int mlen, const char * argv)
{
    int offset = 0;
	offset += snprintf(p_buf+offset, mlen-offset, "<tr2:RemoveConfigurationResponse />");		    
	return offset;
}

int build_tr2_GetVideoSourceConfigurations_rly_xml(char * p_buf, int mlen, const char * argv)
{
    BOOL loopflag = 0;
    int offset = 0;
    tr2_GetVideoSourceConfigurations_REQ * p_req = (tr2_GetVideoSourceConfigurations_REQ *)argv;
    ONVIF_VideoSourceConfiguration * p_v_src_cfg = NULL;

    if (p_req->GetConfiguration.ConfigurationTokenFlag)
    {
        p_v_src_cfg = onvif_find_VideoSourceConfiguration(p_req->GetConfiguration.ConfigurationToken);
        if (NULL == p_v_src_cfg)
        {
            return ONVIF_ERR_NoConfig;
        }
    }

    if (p_req->GetConfiguration.ProfileTokenFlag)
    {
        ONVIF_PROFILE * p_profile = onvif_find_profile(p_req->GetConfiguration.ProfileToken);
        if (NULL == p_profile)
        {
            return ONVIF_ERR_NoProfile;
        }
    }

    if (NULL == p_v_src_cfg)
    {
        loopflag = TRUE;
        p_v_src_cfg = g_onvif_cfg.v_src_cfg;
    }

	offset += snprintf(p_buf+offset, mlen-offset, "<tr2:GetVideoSourceConfigurationsResponse>");

    while (p_v_src_cfg)
	{
	    offset += snprintf(p_buf+offset, mlen-offset, "<tr2:Configurations token=\"%s\">", p_v_src_cfg->Configuration.token);
    	offset += build_VideoSourceConfiguration_xml(p_buf+offset, mlen-offset, &p_v_src_cfg->Configuration);
    	offset += snprintf(p_buf+offset, mlen-offset, "</tr2:Configurations>");

    	if (loopflag)
    	{
    	    p_v_src_cfg = p_v_src_cfg->next;
    	}
    	else
    	{
    	    break;
    	}
	}
	
	offset += snprintf(p_buf+offset, mlen-offset, "</tr2:GetVideoSourceConfigurationsResponse>");
    
    return offset;
}

int build_tr2_GetMetadataConfigurations_rly_xml(char * p_buf, int mlen, const char * argv)
{
    BOOL loopflag = 0;
    int offset = 0;
    tr2_GetMetadataConfigurations_REQ * p_req = (tr2_GetMetadataConfigurations_REQ *)argv;
    ONVIF_MetadataConfiguration * p_metadata_cfg = NULL;

    if (p_req->GetConfiguration.ConfigurationTokenFlag)
    {
        p_metadata_cfg = onvif_find_MetadataConfiguration(p_req->GetConfiguration.ConfigurationToken);
        if (NULL == p_metadata_cfg)
        {
            return ONVIF_ERR_NoConfig;
        }
    }

    if (p_req->GetConfiguration.ProfileTokenFlag)
    {
        ONVIF_PROFILE * p_profile = onvif_find_profile(p_req->GetConfiguration.ProfileToken);
        if (NULL == p_profile)
        {
            return ONVIF_ERR_NoProfile;
        }
    }

    if (NULL == p_metadata_cfg)
    {
        loopflag = TRUE;
        p_metadata_cfg = g_onvif_cfg.metadata_cfg;
    }

	offset += snprintf(p_buf+offset, mlen-offset, "<tr2:GetMetadataConfigurationsResponse>");

    while (p_metadata_cfg)
	{
	    offset += snprintf(p_buf+offset, mlen-offset, "<tr2:Configurations token=\"%s\">", p_metadata_cfg->Configuration.token);
    	offset += build_MetadataConfiguration_xml(p_buf+offset, mlen-offset, &p_metadata_cfg->Configuration);
    	offset += snprintf(p_buf+offset, mlen-offset, "</tr2:Configurations>");

    	if (loopflag)
    	{
    	    p_metadata_cfg = p_metadata_cfg->next;
    	}
    	else
    	{
    	    break;
    	}
	}
	
	offset += snprintf(p_buf+offset, mlen-offset, "</tr2:GetMetadataConfigurationsResponse>");
    
    return offset;
}

int build_tr2_SetVideoSourceConfiguration_rly_xml(char * p_buf, int mlen, const char * argv)
{
    int offset = 0;
	offset += snprintf(p_buf+offset, mlen-offset, "<tr2:SetVideoSourceConfigurationResponse />");		    
	return offset;
}

int build_tr2_SetMetadataConfiguration_rly_xml(char * p_buf, int mlen, const char * argv)
{
    int offset = 0;
	offset += snprintf(p_buf+offset, mlen-offset, "<tr2:SetMetadataConfigurationResponse />");		    
	return offset;
}

int build_tr2_SetAudioSourceConfiguration_rly_xml(char * p_buf, int mlen, const char * argv)
{
    int offset = 0;
	offset += snprintf(p_buf+offset, mlen-offset, "<tr2:SetAudioSourceConfigurationResponse />");		    
	return offset;
}

int build_tr2_GetVideoSourceConfigurationOptions_rly_xml(char * p_buf, int mlen, const char * argv)
{
    int offset = 0;
	ONVIF_VideoSourceConfiguration * p_v_src_cfg = NULL;
	
	tr2_GetVideoSourceConfigurationOptions_REQ * p_req = (tr2_GetVideoSourceConfigurationOptions_REQ *)argv;
	if (p_req->GetConfiguration.ProfileTokenFlag && p_req->GetConfiguration.ProfileToken[0] != '\0')
	{
		ONVIF_PROFILE * p_profile = onvif_find_profile(p_req->GetConfiguration.ProfileToken);
		if (NULL == p_profile)
		{
			return ONVIF_ERR_NoProfile;
		}

		p_v_src_cfg = p_profile->v_src_cfg;
	}

	if (p_req->GetConfiguration.ConfigurationTokenFlag && p_req->GetConfiguration.ConfigurationToken[0] != '\0')
	{
		p_v_src_cfg = onvif_find_VideoSourceConfiguration(p_req->GetConfiguration.ConfigurationToken);
		if (NULL == p_v_src_cfg)
		{
			return ONVIF_ERR_NoConfig;
		}
	}
	
	offset += snprintf(p_buf+offset, mlen-offset, "<tr2:GetVideoSourceConfigurationOptionsResponse>");
    offset += snprintf(p_buf+offset, mlen-offset, "<tr2:Options>");
    
    offset += build_VideoSourceConfigurationOptions_xml(p_buf+offset, mlen-offset, &g_onvif_cfg.VideoSourceConfigurationOptions);
    
	offset += snprintf(p_buf+offset, mlen-offset, "</tr2:Options>");
	offset += snprintf(p_buf+offset, mlen-offset, "</tr2:GetVideoSourceConfigurationOptionsResponse>");
	
	return offset;
}

int build_tr2_GetMetadataConfigurationOptions_rly_xml(char * p_buf, int mlen, const char * argv)
{
    int offset = 0;
	ONVIF_PROFILE * p_profile = NULL;
	tr2_GetMetadataConfigurationOptions_REQ * p_req = (tr2_GetMetadataConfigurationOptions_REQ *) argv;

	if (p_req->GetConfiguration.ProfileTokenFlag)
	{
		p_profile = onvif_find_profile(p_req->GetConfiguration.ProfileToken);
		if (NULL == p_profile)
		{
			return ONVIF_ERR_NoProfile;
		}
	}

	offset += snprintf(p_buf+offset, mlen-offset, "<tr2:GetMetadataConfigurationOptionsResponse>");
	offset += snprintf(p_buf+offset, mlen-offset, "<tr2:Options>");

	offset += build_MetadataConfigurationOptions_xml(p_buf+offset, mlen-offset, &g_onvif_cfg.MetadataConfigurationOptions);
	
	offset += snprintf(p_buf+offset, mlen-offset, "</tr2:Options>");
	offset += snprintf(p_buf+offset, mlen-offset, "</tr2:GetMetadataConfigurationOptionsResponse>");
	
	return offset;
}

int build_tr2_GetVideoEncoderInstances_rly_xml(char * p_buf, int mlen, const char * argv)
{
    int i;
    int offset = 0;
    tr2_GetVideoEncoderInstances_RES * p_res = (tr2_GetVideoEncoderInstances_RES *)argv;

	offset += snprintf(p_buf+offset, mlen-offset, "<tr2:GetVideoEncoderInstancesResponse>");
	offset += snprintf(p_buf+offset, mlen-offset, "<tr2:Info>");

    for (i = 0; i < p_res->Info.sizeCodec; i++)
    {
        offset += snprintf(p_buf+offset, mlen-offset, 
            "<tr2:Codec>"
                "<tr2:Encoding>%s</tr2:Encoding>"
                "<tr2:Number>%d</tr2:Number>"
            "</tr2:Codec>",
            p_res->Info.Codec[i].Encoding,
            p_res->Info.Codec[i].Number);
    }
    
    offset += snprintf(p_buf+offset, mlen-offset, "<tr2:Total>%d</tr2:Total>", p_res->Info.Total);
    
	offset += snprintf(p_buf+offset, mlen-offset, "</tr2:Info>");
	offset += snprintf(p_buf+offset, mlen-offset, "</tr2:GetVideoEncoderInstancesResponse>");
	
	return offset;
}

int build_tr2_GetStreamUri_rly_xml(char * p_buf, int mlen, const char * argv)
{
    int offset = 0;
	tr2_GetStreamUri_RES * p_res = (tr2_GetStreamUri_RES *)argv;
    
    offset += snprintf(p_buf+offset, mlen-offset, 
    	"<tr2:GetStreamUriResponse>"
		    "<tr2:Uri>%s</tr2:Uri>"
	    "</tr2:GetStreamUriResponse>", p_res->Uri);

    // onvif_print("======== build_tr2_GetStreamUri_rly_xml | rtspuri : %s ==========\n", p_res->Uri);
    
	return offset;
}

int build_tr2_SetSynchronizationPoint_rly_xml(char * p_buf, int mlen, const char * argv)
{
    int offset = 0;
	offset += snprintf(p_buf+offset, mlen-offset, "<tr2:SetSynchronizationPointResponse />");		    
	return offset;
}

int build_tr2_GetVideoSourceModes_rly_xml(char * p_buf, int mlen, const char * argv)
{
    int offset = 0;
    tr2_GetVideoSourceModes_REQ * p_req = (tr2_GetVideoSourceModes_REQ *)argv;

    ONVIF_VideoSource * p_v_src = onvif_find_VideoSource(p_req->VideoSourceToken);
    if (NULL == p_v_src)
    {
        return ONVIF_ERR_NoVideoSource;
    }

    offset += snprintf(p_buf+offset, mlen-offset, "<tr2:GetVideoSourceModesResponse>");
    offset += snprintf(p_buf+offset, mlen-offset, "<tr2:VideoSourceModes token=\"%s\" Enabled=\"%s\">",
        p_v_src->VideoSourceMode.token, p_v_src->VideoSourceMode.Enabled ? "true" : "false");

    offset += snprintf(p_buf+offset, mlen-offset, 
        "<tr2:MaxFramerate>%0.1f</tr2:MaxFramerate>"
        "<tr2:MaxResolution>"
            "<tt:Width>%d</tt:Width>"
            "<tt:Height>%d</tt:Height>"
        "</tr2:MaxResolution>"
        "<tr2:Encodings>%s</tr2:Encodings>"
        "<tr2:Reboot>%s</tr2:Reboot>",
        p_v_src->VideoSourceMode.MaxFramerate,
        p_v_src->VideoSourceMode.MaxResolution.Width,
        p_v_src->VideoSourceMode.MaxResolution.Height,
        p_v_src->VideoSourceMode.Encodings,
        p_v_src->VideoSourceMode.Reboot ? "true" : "false");

    if (p_v_src->VideoSourceMode.DescriptionFlag)
    {
        offset += snprintf(p_buf+offset, mlen-offset, 
            "<tr2:Description>%s</tr2:Description>",
            p_v_src->VideoSourceMode.Description);
    }

    offset += snprintf(p_buf+offset, mlen-offset, "</tr2:VideoSourceModes>");
    offset += snprintf(p_buf+offset, mlen-offset, "</tr2:GetVideoSourceModesResponse>");

	return offset;
}

int build_tr2_SetVideoSourceMode_rly_xml(char * p_buf, int mlen, const char * argv)
{
    int offset = 0;
    tr2_SetVideoSourceMode_RES * p_res = (tr2_SetVideoSourceMode_RES *) argv;
    
    offset += snprintf(p_buf+offset, mlen-offset, 
        "<tr2:SetVideoSourceModeResponse>"
            "<tr2:Reboot>%s</tr2:Reboot>"
        "</tr2:SetVideoSourceModeResponse>",
        p_res->Reboot ? "true" : "false");

	return offset;
}

int build_tr2_GetSnapshotUri_rly_xml(char * p_buf, int mlen, const char * argv)
{
    int offset = 0;
	tr2_GetSnapshotUri_RES * p_res = (tr2_GetSnapshotUri_RES *) argv;
    
	offset += snprintf(p_buf+offset, mlen-offset, 
        "<tr2:GetSnapshotUriResponse>"
            "<tr2:Uri>%s</tr2:Uri>"
        "</tr2:GetSnapshotUriResponse>",
        p_res->Uri);
	
	return offset;
}

int build_tr2_SetOSD_rly_xml(char * p_buf, int mlen, const char * argv)
{
    int offset = 0;
	offset += snprintf(p_buf+offset, mlen-offset, "<tr2:SetOSDResponse />");
	return offset;
}

int build_tr2_GetOSDOptions_rly_xml(char * p_buf, int mlen, const char * argv)
{
	int offset = 0;
	onvif_OSDConfigurationOptions * p_req = &g_onvif_cfg.OSDConfigurationOptions;

	offset += snprintf(p_buf+offset, mlen-offset, "<tr2:GetOSDOptionsResponse><tr2:OSDOptions>");

	offset += snprintf(p_buf+offset, mlen-offset, "<tt:MaximumNumberOfOSDs Total=\"%d\"",
		p_req->MaximumNumberOfOSDs.Total);
	if (p_req->MaximumNumberOfOSDs.ImageFlag)
	{
		offset += snprintf(p_buf+offset, mlen-offset, " Image=\"%d\"", 
			p_req->MaximumNumberOfOSDs.Image);
	}
	if (p_req->MaximumNumberOfOSDs.PlainTextFlag)
	{
		offset += snprintf(p_buf+offset, mlen-offset, " PlainText=\"%d\"", 
			p_req->MaximumNumberOfOSDs.PlainText);
	}
	if (p_req->MaximumNumberOfOSDs.DateFlag)
	{
		offset += snprintf(p_buf+offset, mlen-offset, " Date=\"%d\"", 
			p_req->MaximumNumberOfOSDs.Date);
	}
	if (p_req->MaximumNumberOfOSDs.TimeFlag)
	{
		offset += snprintf(p_buf+offset, mlen-offset, " Time=\"%d\"", 
			p_req->MaximumNumberOfOSDs.Time);
	}
	if (p_req->MaximumNumberOfOSDs.DateAndTimeFlag)
	{
		offset += snprintf(p_buf+offset, mlen-offset, " DateAndTime=\"%d\"", 
			p_req->MaximumNumberOfOSDs.DateAndTime);
	}
	offset += snprintf(p_buf+offset, mlen-offset, "></tt:MaximumNumberOfOSDs>");

	if (p_req->OSDType_Text)
	{
		offset += snprintf(p_buf+offset, mlen-offset, "<tt:Type>%s</tt:Type>", 
			onvif_OSDTypeToString(OSDType_Text));
	}
	if (p_req->OSDType_Image)
	{
		offset += snprintf(p_buf+offset, mlen-offset, "<tt:Type>%s</tt:Type>", 
			onvif_OSDTypeToString(OSDType_Image));
	}
	if (p_req->OSDType_Extended)
	{
		offset += snprintf(p_buf+offset, mlen-offset, "<tt:Type>%s</tt:Type>", 
			onvif_OSDTypeToString(OSDType_Extended));
	}

	if (p_req->OSDPosType_LowerLeft)
	{
		offset += snprintf(p_buf+offset, mlen-offset, "<tt:PositionOption>%s</tt:PositionOption>", 
			onvif_OSDPosTypeToString(OSDPosType_LowerLeft));
	}
	if (p_req->OSDPosType_LowerRight)
	{
		offset += snprintf(p_buf+offset, mlen-offset, "<tt:PositionOption>%s</tt:PositionOption>", 
			onvif_OSDPosTypeToString(OSDPosType_LowerRight));
	}
	if (p_req->OSDPosType_UpperLeft)
	{
		offset += snprintf(p_buf+offset, mlen-offset, "<tt:PositionOption>%s</tt:PositionOption>", 
			onvif_OSDPosTypeToString(OSDPosType_UpperLeft));
	}
	if (p_req->OSDPosType_UpperRight)
	{
		offset += snprintf(p_buf+offset, mlen-offset, "<tt:PositionOption>%s</tt:PositionOption>", 
			onvif_OSDPosTypeToString(OSDPosType_UpperRight));
	}
	if (p_req->OSDPosType_Custom)
	{
		offset += snprintf(p_buf+offset, mlen-offset, "<tt:PositionOption>%s</tt:PositionOption>", 
			onvif_OSDPosTypeToString(OSDPosType_Custom));
	}

	if (p_req->TextOptionFlag)
	{
	    offset += build_OSDTextOptions_xml(p_buf+offset, mlen-offset, &p_req->TextOption);		
	}

	if (p_req->ImageOptionFlag)
	{
	    offset += build_OSDImgOptions_xml(p_buf+offset, mlen-offset, &p_req->ImageOption);
	}
	
	offset += snprintf(p_buf+offset, mlen-offset, "</tr2:OSDOptions></tr2:GetOSDOptionsResponse>");

	return offset;
}

int build_tr2_GetOSDs_rly_xml(char * p_buf, int mlen, const char * argv)
{
    BOOL loopflag = 0;
    int offset = 0;
    tr2_GetOSDs_REQ * p_req = (tr2_GetOSDs_REQ *)argv;
    ONVIF_OSDConfiguration * p_osd = NULL;

    if (p_req->OSDTokenFlag)
    {
        p_osd = onvif_find_OSDConfiguration(p_req->OSDToken);
    	if (NULL == p_osd)
    	{
    		return ONVIF_ERR_NoConfig;
	    }
    }

    if (p_req->ConfigurationTokenFlag)
    {
        ONVIF_VideoSourceConfiguration * p_v_src_cfg = onvif_find_VideoSourceConfiguration(p_req->ConfigurationToken);
        if (NULL == p_v_src_cfg)
        {
        }
    }

    if (NULL == p_osd)
    {
        loopflag = TRUE;
        p_osd = g_onvif_cfg.OSDs;
    }	

	offset += snprintf(p_buf+offset, mlen-offset, "<tr2:GetOSDsResponse>");

    while (p_osd)
	{
	    offset += snprintf(p_buf+offset, mlen-offset, "<tr2:OSDs token=\"%s\">", p_osd->OSD.token);
		offset += build_OSD_xml(p_buf+offset, mlen-offset, p_osd);
		offset += snprintf(p_buf+offset, mlen-offset, "</tr2:OSDs>");

    	if (loopflag)
    	{
    	    p_osd = p_osd->next;
    	}
    	else
    	{
    	    break;
    	}
	}
	
	offset += snprintf(p_buf+offset, mlen-offset, "</tr2:GetOSDsResponse>");
    
    return offset;
}

int build_tr2_CreateOSD_rly_xml(char * p_buf, int mlen, const char * argv)
{
    int offset = 0;

	offset += snprintf(p_buf+offset, mlen-offset, 
	    "<tr2:CreateOSDResponse><tr2:OSDToken>%s</tr2:OSDToken></tr2:CreateOSDResponse>", argv);

	return offset;
}

int build_tr2_DeleteOSD_rly_xml(char * p_buf, int mlen, const char * argv)
{
    int offset = 0;

	offset += snprintf(p_buf+offset, mlen-offset, "<tr2:DeleteOSDResponse />");

	return offset;
}

int build_tr2_CreateMask_rly_xml(char * p_buf, int mlen, const char * argv)
{
    int offset = 0;

	offset += snprintf(p_buf+offset, mlen-offset, 
	    "<tr2:CreateMaskResponse><tr2:Token>%s</tr2:Token></tr2:CreateMaskResponse>", argv);

	return offset;
}

int build_tr2_DeleteMask_rly_xml(char * p_buf, int mlen, const char * argv)
{
    int offset = 0;
	offset += snprintf(p_buf+offset, mlen-offset, "<tr2:DeleteMaskResponse />");
	return offset;
}

int build_tr2_GetMasks_rly_xml(char * p_buf, int mlen, const char * argv)
{
    BOOL loopflag = 0;
    int offset = 0;
    tr2_GetMasks_REQ * p_req = (tr2_GetMasks_REQ *)argv;
    ONVIF_Mask * p_mask = NULL;

    if (p_req->TokenFlag)
    {
        p_mask = onvif_find_Mask(p_req->Token);
    	if (NULL == p_mask)
    	{
    		return ONVIF_ERR_NoConfig;
	    }
    }

    if (p_req->ConfigurationTokenFlag)
    {
        ONVIF_VideoSourceConfiguration * p_v_src_cfg = onvif_find_VideoSourceConfiguration(p_req->ConfigurationToken);
        if (NULL == p_v_src_cfg)
        {
        }
    }

    if (NULL == p_mask)
    {
        loopflag = TRUE;
        p_mask = g_onvif_cfg.mask;
    }	

	offset += snprintf(p_buf+offset, mlen-offset, "<tr2:GetMasksResponse>");

    while (p_mask)
	{
	    offset += snprintf(p_buf+offset, mlen-offset, "<tr2:Masks token=\"%s\">", p_mask->Mask.token);
		offset += build_Mask_xml(p_buf+offset, mlen-offset, &p_mask->Mask);
		offset += snprintf(p_buf+offset, mlen-offset, "</tr2:Masks>");

    	if (loopflag)
    	{
    	    p_mask = p_mask->next;
    	}
    	else
    	{
    	    break;
    	}
	}
	
	offset += snprintf(p_buf+offset, mlen-offset, "</tr2:GetMasksResponse>");
    
    return offset;
}

int build_tr2_SetMask_rly_xml(char * p_buf, int mlen, const char * argv)
{
    int offset = 0;
	offset += snprintf(p_buf+offset, mlen-offset, "<tr2:SetMaskResponse />");
	return offset;
}

int build_tr2_GetMaskOptions_rly_xml(char * p_buf, int mlen, const char * argv)
{
    int i;
    int offset = 0;
	onvif_MaskOptions * p_req = &g_onvif_cfg.MaskOptions;

	offset += snprintf(p_buf+offset, mlen-offset, "<tr2:GetMaskOptionsResponse>");
	
	offset += snprintf(p_buf+offset, mlen-offset, 
	    "<tr2:Options RectangleOnly=\"%s\" SingleColorOnly=\"%s\">",
	    p_req->RectangleOnly ? "true" : "false",
	    p_req->SingleColorOnly ? "true" : "false");
	    
    offset += snprintf(p_buf+offset, mlen-offset, 
        "<tr2:MaxMasks>%d</tr2:MaxMasks>", p_req->MaxMasks);
        
    offset += snprintf(p_buf+offset, mlen-offset, 
        "<tr2:MaxPoints>%d</tr2:MaxPoints>", p_req->MaxPoints);

    for (i = 0; i < p_req->sizeTypes; i++)
    {
        offset += snprintf(p_buf+offset, mlen-offset, 
            "<tr2:Types>%s</tr2:Types>", p_req->Types[i]);
    }

    offset += snprintf(p_buf+offset, mlen-offset, "<tr2:Color>");
    offset += build_ColorOptions_xml(p_buf+offset, mlen-offset, &p_req->Color);
    offset += snprintf(p_buf+offset, mlen-offset, "</tr2:Color>");

    offset += snprintf(p_buf+offset, mlen-offset, "</tr2:Options>");
    offset += snprintf(p_buf+offset, mlen-offset, "</tr2:GetMaskOptionsResponse>");
    
    return offset;        
}

#ifdef AUDIO_SUPPORT

int build_tr2_GetAudioEncoderConfigurations_rly_xml(char * p_buf, int mlen, const char * argv)
{
    BOOL loopflag = 0;
    int offset = 0;
    tr2_GetAudioEncoderConfigurations_REQ * p_req = (tr2_GetAudioEncoderConfigurations_REQ *)argv;
    ONVIF_AudioEncoder2Configuration * p_a_enc_cfg = NULL;

    if (p_req->GetConfiguration.ConfigurationTokenFlag)
    {
        p_a_enc_cfg = onvif_find_AudioEncoderConfiguration(p_req->GetConfiguration.ConfigurationToken);
        if (NULL == p_a_enc_cfg)
        {
            return ONVIF_ERR_NoConfig;
        }
    }

    if (p_req->GetConfiguration.ProfileTokenFlag)
    {
        ONVIF_PROFILE * p_profile = onvif_find_profile(p_req->GetConfiguration.ProfileToken);
        if (NULL == p_profile)
        {
            return ONVIF_ERR_NoProfile;
        }
    }

    if (NULL == p_a_enc_cfg)
    {
        loopflag = TRUE;
        p_a_enc_cfg = g_onvif_cfg.a_enc_cfg;
    }

	offset += snprintf(p_buf+offset, mlen-offset, "<tr2:GetAudioEncoderConfigurationsResponse>");

    while (p_a_enc_cfg)
	{
	    offset += snprintf(p_buf+offset, mlen-offset, "<tr2:Configurations token=\"%s\">", p_a_enc_cfg->Configuration.token);
    	offset += build_AudioEncoder2Configuration_xml(p_buf+offset, mlen-offset, &p_a_enc_cfg->Configuration);
    	offset += snprintf(p_buf+offset, mlen-offset, "</tr2:Configurations>");

    	if (loopflag)
    	{
    	    p_a_enc_cfg = p_a_enc_cfg->next;
    	}
    	else
    	{
    	    break;
    	}
	}
	
	offset += snprintf(p_buf+offset, mlen-offset, "</tr2:GetAudioEncoderConfigurationsResponse>");
    
    return offset;
}

int build_tr2_GetAudioSourceConfigurations_rly_xml(char * p_buf, int mlen, const char * argv)
{
    BOOL loopflag = 0;
    int offset = 0;
    tr2_GetAudioSourceConfigurations_REQ * p_req = (tr2_GetAudioSourceConfigurations_REQ *)argv;
    ONVIF_AudioSourceConfiguration * p_a_src_cfg = NULL;

    if (p_req->GetConfiguration.ConfigurationTokenFlag)
    {
        p_a_src_cfg = onvif_find_AudioSourceConfiguration(p_req->GetConfiguration.ConfigurationToken);
        if (NULL == p_a_src_cfg)
        {
            return ONVIF_ERR_NoConfig;
        }
    }

    if (p_req->GetConfiguration.ProfileTokenFlag)
    {
        ONVIF_PROFILE * p_profile = onvif_find_profile(p_req->GetConfiguration.ProfileToken);
        if (NULL == p_profile)
        {
            return ONVIF_ERR_NoProfile;
        }
    }

    if (NULL == p_a_src_cfg)
    {
        loopflag = TRUE;
        p_a_src_cfg = g_onvif_cfg.a_src_cfg;
    }

	offset += snprintf(p_buf+offset, mlen-offset, "<tr2:GetAudioSourceConfigurationsResponse>");

    while (p_a_src_cfg)
	{
	    offset += snprintf(p_buf+offset, mlen-offset, "<tr2:Configurations token=\"%s\">", p_a_src_cfg->Configuration.token);
    	offset += build_AudioSourceConfiguration_xml(p_buf+offset, mlen-offset, &p_a_src_cfg->Configuration);
    	offset += snprintf(p_buf+offset, mlen-offset, "</tr2:Configurations>");

    	if (loopflag)
    	{
    	    p_a_src_cfg = p_a_src_cfg->next;
    	}
    	else
    	{
    	    break;
    	}
	}
	
	offset += snprintf(p_buf+offset, mlen-offset, "</tr2:GetAudioSourceConfigurationsResponse>");
    
    return offset;
}

int build_tr2_GetAudioSourceConfigurationOptions_rly_xml(char * p_buf, int mlen, const char * argv)
{
    int offset = 0;
	ONVIF_AudioSourceConfiguration * p_a_src_cfg = NULL;
	
	tr2_GetAudioSourceConfigurationOptions_REQ * p_req = (tr2_GetAudioSourceConfigurationOptions_REQ *)argv;
	if (p_req->GetConfiguration.ProfileTokenFlag && p_req->GetConfiguration.ProfileToken[0] != '\0')
	{
		ONVIF_PROFILE * p_profile = onvif_find_profile(p_req->GetConfiguration.ProfileToken);
		if (NULL == p_profile)
		{
			return ONVIF_ERR_NoProfile;
		}

		p_a_src_cfg = p_profile->a_src_cfg;
	}

	if (p_req->GetConfiguration.ConfigurationTokenFlag && p_req->GetConfiguration.ConfigurationToken[0] != '\0')
	{
		p_a_src_cfg = onvif_find_AudioSourceConfiguration(p_req->GetConfiguration.ConfigurationToken);
		if (NULL == p_a_src_cfg)
		{
			return ONVIF_ERR_NoConfig;
		}
	}
	
	offset += snprintf(p_buf+offset, mlen-offset, "<tr2:GetAudioSourceConfigurationOptionsResponse>");
	offset += snprintf(p_buf+offset, mlen-offset, "<tr2:Options>");

	offset += build_AudioSourceConfigurationOptions_xml(p_buf+offset, mlen-offset);

    offset += snprintf(p_buf+offset, mlen-offset, "</tr2:Options>");
	offset += snprintf(p_buf+offset, mlen-offset, "</tr2:GetAudioSourceConfigurationOptionsResponse>");
	
	return offset;
}

int build_tr2_GetAudioEncoderConfigurationOptions_rly_xml(char * p_buf, int mlen, const char * argv)
{
    int offset = 0;
    tr2_GetAudioEncoderConfigurationOptions_REQ * p_req = (tr2_GetAudioEncoderConfigurationOptions_REQ *)argv;
    ONVIF_AudioEncoder2Configuration * p_a_enc_cfg = NULL;
    ONVIF_AudioEncoder2ConfigurationOptions * p_option;

    if (p_req->GetConfiguration.ConfigurationTokenFlag)
    {
        p_a_enc_cfg = onvif_find_AudioEncoderConfiguration(p_req->GetConfiguration.ConfigurationToken);
        if (NULL == p_a_enc_cfg)
        {
            return ONVIF_ERR_NoConfig;
        }
    }

    if (p_req->GetConfiguration.ProfileTokenFlag)
    {
        ONVIF_PROFILE * p_profile = onvif_find_profile(p_req->GetConfiguration.ProfileToken);
        if (NULL == p_profile)
        {
            return ONVIF_ERR_NoProfile;
        }
    }

    p_option = g_onvif_cfg.a_enc_cfg_opt;

	offset += snprintf(p_buf+offset, mlen-offset, "<tr2:GetAudioEncoderConfigurationOptionsResponse>");

    while (p_option)
	{
	    offset += snprintf(p_buf+offset, mlen-offset, "<tr2:Options>");        
    	offset += build_AudioEncoder2ConfigurationOptions_xml(p_buf+offset, mlen-offset, &p_option->Options);
    	offset += snprintf(p_buf+offset, mlen-offset, "</tr2:Options>");

	    p_option = p_option->next;
	}
	
	offset += snprintf(p_buf+offset, mlen-offset, "</tr2:GetAudioEncoderConfigurationOptionsResponse>");
    
    return offset;
}

int build_tr2_SetAudioEncoderConfiguration_rly_xml(char * p_buf, int mlen, const char * argv)
{
    int offset = 0;
	offset += snprintf(p_buf+offset, mlen-offset, "<tr2:SetAudioEncoderConfigurationResponse />");		    
	return offset;
}

int build_tr2_GetAudioDecoderConfigurations_rly_xml(char * p_buf, int mlen, const char * argv)
{
    BOOL loopflag = 0;
    int offset = 0;
    tr2_GetAudioDecoderConfigurations_REQ * p_req = (tr2_GetAudioDecoderConfigurations_REQ *)argv;
    ONVIF_AudioDecoderConfiguration * p_a_dec_cfg = NULL;

    if (p_req->GetConfiguration.ConfigurationTokenFlag)
    {
        p_a_dec_cfg = onvif_find_AudioDecoderConfiguration(p_req->GetConfiguration.ConfigurationToken);
        if (NULL == p_a_dec_cfg)
        {
            return ONVIF_ERR_NoConfig;
        }
    }

    if (p_req->GetConfiguration.ProfileTokenFlag)
    {
        ONVIF_PROFILE * p_profile = onvif_find_profile(p_req->GetConfiguration.ProfileToken);
        if (NULL == p_profile)
        {
            return ONVIF_ERR_NoProfile;
        }
    }

    if (NULL == p_a_dec_cfg)
    {
        loopflag = TRUE;
        p_a_dec_cfg = g_onvif_cfg.a_dec_cfg;
    }

	offset += snprintf(p_buf+offset, mlen-offset, "<tr2:GetAudioDecoderConfigurationsResponse>");

    while (p_a_dec_cfg)
	{
	    offset += snprintf(p_buf+offset, mlen-offset, 
            "<tr2:Configurations token=\"%s\">", 
            p_a_dec_cfg->Configuration.token);
        
        offset += build_AudioDecoderConfiguration_xml(p_buf+offset, mlen-offset, &p_a_dec_cfg->Configuration);
        
        offset += snprintf(p_buf+offset, mlen-offset, "</tr2:Configurations>");            

    	if (loopflag)
    	{
    	    p_a_dec_cfg = p_a_dec_cfg->next;
    	}
    	else
    	{
    	    break;
    	}
	}
	
	offset += snprintf(p_buf+offset, mlen-offset, "</tr2:GetAudioDecoderConfigurationsResponse>");
    
    return offset;
}

int build_tr2_SetAudioDecoderConfiguration_rly_xml(char * p_buf, int mlen, const char * argv)
{
    int offset = 0;
	offset += snprintf(p_buf+offset, mlen-offset, "<tr2:SetAudioDecoderConfigurationResponse />");
	return offset;
}

int build_tr2_GetAudioDecoderConfigurationOptions_rly_xml(char * p_buf, int mlen, const char * argv)
{
    int offset = 0;
    tr2_GetAudioDecoderConfigurationOptions_REQ * p_req = (tr2_GetAudioDecoderConfigurationOptions_REQ *)argv;
    ONVIF_AudioDecoderConfiguration * p_a_dec_cfg = NULL;
    ONVIF_AudioEncoder2ConfigurationOptions * p_option;

    if (p_req->GetConfiguration.ConfigurationTokenFlag)
    {
        p_a_dec_cfg = onvif_find_AudioDecoderConfiguration(p_req->GetConfiguration.ConfigurationToken);
        if (NULL == p_a_dec_cfg)
        {
            return ONVIF_ERR_NoConfig;
        }
    }

    if (p_req->GetConfiguration.ProfileTokenFlag)
    {
        ONVIF_PROFILE * p_profile = onvif_find_profile(p_req->GetConfiguration.ProfileToken);
        if (NULL == p_profile)
        {
            return ONVIF_ERR_NoProfile;
        }
    }

    p_option = g_onvif_cfg.a_dec2_cfg_opt;

	offset += snprintf(p_buf+offset, mlen-offset, "<tr2:GetAudioDecoderConfigurationOptionsResponse>");

    while (p_option)
	{
	    offset += snprintf(p_buf+offset, mlen-offset, "<tr2:Options>");        
    	offset += build_AudioEncoder2ConfigurationOptions_xml(p_buf+offset, mlen-offset, &p_option->Options);
    	offset += snprintf(p_buf+offset, mlen-offset, "</tr2:Options>");

	    p_option = p_option->next;
	}
	
	offset += snprintf(p_buf+offset, mlen-offset, "</tr2:GetAudioDecoderConfigurationOptionsResponse>");
    
    return offset;
}

#endif // end of AUDIO_SUPPORT

#ifdef DEVICEIO_SUPPORT

int build_tr2_GetAudioOutputConfigurationOptions_rly_xml(char * p_buf, int mlen, const char * argv)
{
    int offset = 0;
    ONVIF_AudioOutputConfiguration * p_cfg = NULL;
    trt_GetAudioOutputConfigurationOptions_REQ * p_req = (trt_GetAudioOutputConfigurationOptions_REQ *)argv; 
    if (p_req->ConfigurationTokenFlag)
    {
        p_cfg = onvif_find_AudioOutputConfiguration(p_req->ConfigurationToken);
        if (NULL == p_cfg)
        {
            return ONVIF_ERR_NoConfig;
        }
    }

    if (p_req->ProfileTokenFlag)
    {
        ONVIF_PROFILE * p_profile = onvif_find_profile(p_req->ProfileToken);
        if (NULL == p_profile)
        {
            return ONVIF_ERR_NoProfile;
        }
    }

    if (NULL == p_cfg)
    {
        p_cfg = g_onvif_cfg.a_output_cfg;
    }

	offset += snprintf(p_buf+offset, mlen-offset, "<tr2:GetAudioOutputConfigurationOptionsResponse>");
	offset += snprintf(p_buf+offset, mlen-offset, "<tr2:Options>");

    offset += build_AudioOutputConfigurationOptions_xml(p_buf+offset, mlen-offset, &p_cfg->Options);       
	
	offset += snprintf(p_buf+offset, mlen-offset, "</tr2:Options>");
	offset += snprintf(p_buf+offset, mlen-offset, "</tr2:GetAudioOutputConfigurationOptionsResponse>");

	return offset;
}

int build_tr2_GetAudioOutputConfigurations_rly_xml(char * p_buf, int mlen, const char * argv)
{
    BOOL loopflag = 0;
    int offset = 0;
    tr2_GetAudioOutputConfigurations_REQ * p_req = (tr2_GetAudioOutputConfigurations_REQ *)argv;
    ONVIF_AudioOutputConfiguration * p_a_output_cfg = NULL;

    if (p_req->GetConfiguration.ConfigurationTokenFlag)
    {
        p_a_output_cfg = onvif_find_AudioOutputConfiguration(p_req->GetConfiguration.ConfigurationToken);
        if (NULL == p_a_output_cfg)
        {
            return ONVIF_ERR_NoConfig;
        }
    }

    if (p_req->GetConfiguration.ProfileTokenFlag)
    {
        ONVIF_PROFILE * p_profile = onvif_find_profile(p_req->GetConfiguration.ProfileToken);
        if (NULL == p_profile)
        {
            return ONVIF_ERR_NoProfile;
        }
    }

    if (NULL == p_a_output_cfg)
    {
        loopflag = TRUE;
        p_a_output_cfg = g_onvif_cfg.a_output_cfg;
    }

	offset += snprintf(p_buf+offset, mlen-offset, "<tr2:GetAudioOutputConfigurationsResponse>");

    while (p_a_output_cfg)
	{
	    offset += snprintf(p_buf+offset, mlen-offset, 
            "<tr2:Configurations token=\"%s\">", 
            p_a_output_cfg->Configuration.token);
        
        offset += build_AudioOutputConfiguration_xml(p_buf+offset, mlen-offset, &p_a_output_cfg->Configuration);
        
        offset += snprintf(p_buf+offset, mlen-offset, "</tr2:Configurations>");            

    	if (loopflag)
    	{
    	    p_a_output_cfg = p_a_output_cfg->next;
    	}
    	else
    	{
    	    break;
    	}
	}
	
	offset += snprintf(p_buf+offset, mlen-offset, "</tr2:GetAudioOutputConfigurationsResponse>");
    
    return offset;
}

int build_tr2_SetAudioOutputConfiguration_rly_xml(char * p_buf, int mlen, const char * argv)
{
    int offset = 0;
	offset += snprintf(p_buf+offset, mlen-offset, "<tr2:SetAudioOutputConfigurationResponse />");
	return offset;
}

#endif // end of DEVICEIO_SUPPORT

#ifdef VIDEO_ANALYTICS

int build_tr2_GetAnalyticsConfigurations_rly_xml(char * p_buf, int mlen, const char * argv)
{
    BOOL loopflag = 0;
    int offset = 0;
    tr2_GetAnalyticsConfigurations_REQ * p_req = (tr2_GetAnalyticsConfigurations_REQ *)argv;
    ONVIF_VideoAnalyticsConfiguration * p_va_cfg = NULL;

    if (p_req->GetConfiguration.ConfigurationTokenFlag)
    {
        p_va_cfg = onvif_find_VideoAnalyticsConfiguration(p_req->GetConfiguration.ConfigurationToken);
        if (NULL == p_va_cfg)
        {
            return ONVIF_ERR_NoConfig;
        }
    }

    if (p_req->GetConfiguration.ProfileTokenFlag)
    {
        ONVIF_PROFILE * p_profile = onvif_find_profile(p_req->GetConfiguration.ProfileToken);
        if (NULL == p_profile)
        {
            return ONVIF_ERR_NoProfile;
        }
    }

    if (NULL == p_va_cfg)
    {
        loopflag = TRUE;
        p_va_cfg = g_onvif_cfg.va_cfg;
    }

	offset += snprintf(p_buf+offset, mlen-offset, "<tr2:GetAnalyticsConfigurationsResponse>");

    while (p_va_cfg)
	{
	    offset += snprintf(p_buf+offset, mlen-offset, "<tr2:Configurations token=\"%s\">", p_va_cfg->Configuration.token);
    	offset += build_VideoAnalyticsConfiguration_xml(p_buf+offset, mlen-offset, &p_va_cfg->Configuration);
    	offset += snprintf(p_buf+offset, mlen-offset, "</tr2:Configurations>");

    	if (loopflag)
    	{
    	    p_va_cfg = p_va_cfg->next;
    	}
    	else
    	{
    	    break;
    	}
	}
	
	offset += snprintf(p_buf+offset, mlen-offset, "</tr2:GetAnalyticsConfigurationsResponse>");
    
    return offset;
}

#endif // end of VIDEO_ANALYTICS

#endif // end of MEDIA2_SUPPORT

#ifdef THERMAL_SUPPORT

int build_ColorPalette_xml(char * p_buf, int mlen, onvif_ColorPalette * p_req)
{
    int offset = 0;
    
    offset += snprintf(p_buf+offset, mlen-offset, 
        "<tth:ColorPalette token=\"%s\" Type=\"%s\">"
        "<tth:Name>%s</tth:Name>"
        "</tth:ColorPalette>",
        p_req->token, 
        p_req->Type,
        p_req->Name);

    return offset;        
}

int build_NUCTable_xml(char * p_buf, int mlen, onvif_NUCTable * p_req)
{
    int offset = 0;
    
    offset += snprintf(p_buf+offset, mlen-offset, "<tth:NUCTable token=\"%s\"", p_req->token);
    if (p_req->LowTemperatureFlag)
    {
        offset += snprintf(p_buf+offset, mlen-offset, " LowTemperature=\"%f\"", p_req->LowTemperature);
    }
    if (p_req->HighTemperatureFlag)
    {
        offset += snprintf(p_buf+offset, mlen-offset, " HighTemperature=\"%f\"", p_req->HighTemperature);
    }
    offset += snprintf(p_buf+offset, mlen-offset, ">");
    offset += snprintf(p_buf+offset, mlen-offset, "<tth:Name>%s</tth:Name>", p_req->Name);
    offset += snprintf(p_buf+offset, mlen-offset, "</tth:NUCTable>"); 

    return offset;        
}

int build_ThermalConfiguration_xml(char * p_buf, int mlen, onvif_ThermalConfiguration * p_req)
{
    int offset = 0;
    
    offset += build_ColorPalette_xml(p_buf+offset, mlen-offset, &p_req->ColorPalette);

    offset += snprintf(p_buf+offset, mlen-offset, 
        "<tth:Polarity>%s</tth:Polarity>",
        onvif_PolarityToString(p_req->Polarity));

    if (p_req->NUCTableFlag)
    {
        offset += build_NUCTable_xml(p_buf+offset, mlen-offset, &p_req->NUCTable);     
    }

    if (p_req->CoolerFlag)
    {
        offset += snprintf(p_buf+offset, mlen-offset, "<tth:Cooler>");
        offset += snprintf(p_buf+offset, mlen-offset, "<tth:Enabled>%s</tth:Enabled>", p_req->Cooler.Enabled ? "true" : "false");
        if (p_req->Cooler.RunTimeFlag)
        {
            offset += snprintf(p_buf+offset, mlen-offset, "<tth:RunTime>%f</tth:RunTime>", p_req->Cooler.RunTime);
        }
        offset += snprintf(p_buf+offset, mlen-offset, "</tth:Cooler>");
    }

    return offset;
}

int build_RadiometryGlobalParameters_xml(char * p_buf, int mlen, onvif_RadiometryGlobalParameters * p_req)
{
    int offset = 0;
    
    offset += snprintf(p_buf+offset, mlen-offset, 
        "<tth:ReflectedAmbientTemperature>%f</tth:ReflectedAmbientTemperature>"
        "<tth:Emissivity>%f</tth:Emissivity>"
        "<tth:DistanceToObject>%f</tth:DistanceToObject>",
        p_req->ReflectedAmbientTemperature, 
        p_req->Emissivity,
        p_req->DistanceToObject);

    if (p_req->RelativeHumidityFlag)
    {
        offset += snprintf(p_buf+offset, mlen-offset, 
            "<tth:RelativeHumidity>%f</tth:RelativeHumidity>",
            p_req->RelativeHumidity);
    }

    if (p_req->AtmosphericTemperatureFlag)
    {
        offset += snprintf(p_buf+offset, mlen-offset, 
            "<tth:AtmosphericTemperature>%f</tth:AtmosphericTemperature>",
            p_req->AtmosphericTemperature);
    }

    if (p_req->AtmosphericTransmittanceFlag)
    {
        offset += snprintf(p_buf+offset, mlen-offset, 
            "<tth:AtmosphericTransmittance>%f</tth:AtmosphericTransmittance>",
            p_req->AtmosphericTransmittance);
    }

    if (p_req->ExtOpticsTemperatureFlag)
    {
        offset += snprintf(p_buf+offset, mlen-offset, 
            "<tth:ExtOpticsTemperature>%f</tth:ExtOpticsTemperature>",
            p_req->ExtOpticsTemperature);
    }

    if (p_req->ExtOpticsTransmittanceFlag)
    {
        offset += snprintf(p_buf+offset, mlen-offset, 
            "<tth:ExtOpticsTransmittance>%f</tth:ExtOpticsTransmittance>",
            p_req->ExtOpticsTransmittance);
    }
    
    return offset;
}

int build_RadiometryConfiguration_xml(char * p_buf, int mlen, onvif_RadiometryConfiguration * p_req)
{
    int offset = 0;

    if (p_req->RadiometryGlobalParametersFlag)
    {
        offset += snprintf(p_buf+offset, mlen-offset, "<tth:RadiometryGlobalParameters>");
        offset += build_RadiometryGlobalParameters_xml(p_buf+offset, mlen-offset, &p_req->RadiometryGlobalParameters);
        offset += snprintf(p_buf+offset, mlen-offset, "</tth:RadiometryGlobalParameters>");
    }
    
    return offset;
}

int build_RadiometryGlobalParameterOptions_xml(char * p_buf, int mlen, onvif_RadiometryGlobalParameterOptions * p_req)
{
    int offset = 0;

    offset += snprintf(p_buf+offset, mlen-offset, "<tth:ReflectedAmbientTemperature>");
    offset += build_FloatRange_xml(p_buf+offset, mlen-offset, &p_req->ReflectedAmbientTemperature);
    offset += snprintf(p_buf+offset, mlen-offset, "</tth:ReflectedAmbientTemperature>");

    offset += snprintf(p_buf+offset, mlen-offset, "<tth:Emissivity>");
    offset += build_FloatRange_xml(p_buf+offset, mlen-offset, &p_req->Emissivity);
    offset += snprintf(p_buf+offset, mlen-offset, "</tth:Emissivity>");

    offset += snprintf(p_buf+offset, mlen-offset, "<tth:DistanceToObject>");
    offset += build_FloatRange_xml(p_buf+offset, mlen-offset, &p_req->DistanceToObject);
    offset += snprintf(p_buf+offset, mlen-offset, "</tth:DistanceToObject>");

    if (p_req->RelativeHumidityFlag)
    {
        offset += snprintf(p_buf+offset, mlen-offset, "<tth:RelativeHumidity>");
        offset += build_FloatRange_xml(p_buf+offset, mlen-offset, &p_req->RelativeHumidity);
        offset += snprintf(p_buf+offset, mlen-offset, "</tth:RelativeHumidity>");
    }

    if (p_req->AtmosphericTemperatureFlag)
    {
        offset += snprintf(p_buf+offset, mlen-offset, "<tth:AtmosphericTemperature>");
        offset += build_FloatRange_xml(p_buf+offset, mlen-offset, &p_req->AtmosphericTemperature);
        offset += snprintf(p_buf+offset, mlen-offset, "</tth:AtmosphericTemperature>");
    }

    if (p_req->AtmosphericTransmittanceFlag)
    {
        offset += snprintf(p_buf+offset, mlen-offset, "<tth:AtmosphericTransmittance>");
        offset += build_FloatRange_xml(p_buf+offset, mlen-offset, &p_req->AtmosphericTransmittance);
        offset += snprintf(p_buf+offset, mlen-offset, "</tth:AtmosphericTransmittance>");
    }

    if (p_req->ExtOpticsTemperatureFlag)
    {
        offset += snprintf(p_buf+offset, mlen-offset, "<tth:ExtOpticsTemperature>");
        offset += build_FloatRange_xml(p_buf+offset, mlen-offset, &p_req->ExtOpticsTemperature);
        offset += snprintf(p_buf+offset, mlen-offset, "</tth:ExtOpticsTemperature>");
    }

    if (p_req->ExtOpticsTransmittanceFlag)
    {
        offset += snprintf(p_buf+offset, mlen-offset, "<tth:ExtOpticsTransmittance>");
        offset += build_FloatRange_xml(p_buf+offset, mlen-offset, &p_req->ExtOpticsTransmittance);
        offset += snprintf(p_buf+offset, mlen-offset, "</tth:ExtOpticsTransmittance>");
    }
    
    return offset;
}

int build_tth_GetConfigurations_rly_xml(char * p_buf, int mlen, const char * argv)
{
    int offset = 0;
    ONVIF_VideoSource * p_v_src = g_onvif_cfg.v_src;

	offset += snprintf(p_buf+offset, mlen-offset, "<tth:GetConfigurationsResponse>");
	
    while (p_v_src)
    {
        if (p_v_src->ThermalSupport)
        {
            offset += snprintf(p_buf+offset, mlen-offset, "<tth:Configurations token=\"%s\"><tth:Configuration>", p_v_src->VideoSource.token);
            offset += build_ThermalConfiguration_xml(p_buf+offset, mlen-offset, &p_v_src->ThermalConfiguration);
            offset += snprintf(p_buf+offset, mlen-offset, "</tth:Configuration></tth:Configurations>");
        }

        p_v_src = p_v_src->next;
    }

    offset += snprintf(p_buf+offset, mlen-offset, "</tth:GetConfigurationsResponse>");

	return offset;
}

int build_tth_GetConfiguration_rly_xml(char * p_buf, int mlen, const char * argv)
{
    int offset = 0;
    tth_GetConfiguration_REQ * p_req = (tth_GetConfiguration_REQ *)argv;
    ONVIF_VideoSource * p_v_src = onvif_find_VideoSource(p_req->VideoSourceToken);
    if (NULL == p_v_src)
    {
        return ONVIF_ERR_NoScope;
    }

    if (FALSE == p_v_src->ThermalSupport)
    {
        return ONVIF_ERR_NoThermalForSource;
    }

	offset += snprintf(p_buf+offset, mlen-offset, "<tth:GetConfigurationResponse><tth:Configuration>");
	offset += build_ThermalConfiguration_xml(p_buf+offset, mlen-offset, &p_v_src->ThermalConfiguration);
	offset += snprintf(p_buf+offset, mlen-offset, "</tth:Configuration></tth:GetConfigurationResponse>");
    
    return offset;
}

int build_tth_SetConfiguration_rly_xml(char * p_buf, int mlen, const char * argv)
{
    int offset = 0;
	offset += snprintf(p_buf+offset, mlen-offset, "<tth:SetConfigurationResponse />");
	return offset;
}

int build_tth_GetConfigurationOptions_rly_xml(char * p_buf, int mlen, const char * argv)
{
    int offset = 0;
    ONVIF_ColorPalette * p_ColorPalette;
    ONVIF_NUCTable * p_NUCTable;
    tth_GetConfigurationOptions_REQ * p_req = (tth_GetConfigurationOptions_REQ *) argv;
    ONVIF_VideoSource * p_v_src = onvif_find_VideoSource(p_req->VideoSourceToken);
    if (NULL == p_v_src)
    {
        return ONVIF_ERR_NoSource;
    }

    if (FALSE == p_v_src->ThermalSupport)
    {
        return ONVIF_ERR_NoThermalForSource;
    }

	offset += snprintf(p_buf+offset, mlen-offset, "<tth:GetConfigurationOptionsResponse><tth:ConfigurationOptions>");

    p_ColorPalette = p_v_src->ThermalConfigurationOptions.ColorPalette;
    while (p_ColorPalette)
    {
		offset += build_ColorPalette_xml(p_buf+offset, mlen-offset, &p_ColorPalette->ColorPalette);

		p_ColorPalette = p_ColorPalette->next;
    }

	p_NUCTable = p_v_src->ThermalConfigurationOptions.NUCTable;
	while (p_NUCTable)
	{
		offset += build_NUCTable_xml(p_buf+offset, mlen-offset, &p_NUCTable->NUCTable);

		p_NUCTable = p_NUCTable->next;
	}

    if (p_v_src->ThermalConfigurationOptions.CoolerOptionsFlag)
    {
        offset += snprintf(p_buf+offset, mlen-offset, 
            "<tth:CoolerOptions>"
                "<tth:Enabled>%s</tth:Enabled>"
            "</tth:CoolerOptions>",
            p_v_src->ThermalConfigurationOptions.CoolerOptions.Enabled ? "true" : "false");
    }
	
	offset += snprintf(p_buf+offset, mlen-offset, "</tth:ConfigurationOptions></tth:GetConfigurationOptionsResponse>");
    
    return offset;
}

int build_tth_GetRadiometryConfiguration_rly_xml(char * p_buf, int mlen, const char * argv)
{
    int offset = 0;
    tth_GetRadiometryConfiguration_REQ * p_req = (tth_GetRadiometryConfiguration_REQ *)argv;
    ONVIF_VideoSource * p_v_src = onvif_find_VideoSource(p_req->VideoSourceToken);
    if (NULL == p_v_src)
    {
        return ONVIF_ERR_NoSource;
    }

    if (FALSE == p_v_src->ThermalSupport)
    {
        return ONVIF_ERR_NoRadiometryForSource;
    }

	offset += snprintf(p_buf+offset, mlen-offset, "<tth:GetRadiometryConfigurationResponse><tth:Configuration>");
	offset += build_RadiometryConfiguration_xml(p_buf+offset, mlen-offset, &p_v_src->RadiometryConfiguration);
	offset += snprintf(p_buf+offset, mlen-offset, "</tth:Configuration></tth:GetRadiometryConfigurationResponse>");
    
    return offset;
}

int build_tth_SetRadiometryConfiguration_rly_xml(char * p_buf, int mlen, const char * argv)
{
    int offset = 0;
	offset += snprintf(p_buf+offset, mlen-offset, "<tth:SetRadiometryConfigurationResponse />");
	return offset;
}

int build_tth_GetRadiometryConfigurationOptions_rly_xml(char * p_buf, int mlen, const char * argv)
{
    int offset = 0;
    tth_GetRadiometryConfigurationOptions_REQ * p_req = (tth_GetRadiometryConfigurationOptions_REQ *) argv;
    ONVIF_VideoSource * p_v_src = onvif_find_VideoSource(p_req->VideoSourceToken);
    if (NULL == p_v_src)
    {
        return ONVIF_ERR_NoSource;
    }

    if (FALSE == p_v_src->ThermalSupport)
    {
        return ONVIF_ERR_NoRadiometryForSource;
    }

	offset += snprintf(p_buf+offset, mlen-offset, "<tth:GetRadiometryConfigurationOptionsResponse><tth:ConfigurationOptions>");

    if (p_v_src->RadiometryConfigurationOptions.RadiometryGlobalParameterOptionsFlag)
    {
        offset += snprintf(p_buf+offset, mlen-offset, "<tth:RadiometryGlobalParameterOptions>");
        offset += build_RadiometryGlobalParameterOptions_xml(p_buf+offset, mlen-offset, 
            &p_v_src->RadiometryConfigurationOptions.RadiometryGlobalParameterOptions);
        offset += snprintf(p_buf+offset, mlen-offset, "</tth:RadiometryGlobalParameterOptions>");            
    }
	
	offset += snprintf(p_buf+offset, mlen-offset, "</tth:ConfigurationOptions></tth:GetRadiometryConfigurationOptionsResponse>");
    
    return offset;
}

#endif // end of THERMAL_SUPPORT

#ifdef CREDENTIAL_SUPPORT

int build_CredentialInfo_xml(char * p_buf, int mlen, onvif_CredentialInfo * p_res)
{
    int offset = 0;

    if (p_res->DescriptionFlag)
    {
        offset += snprintf(p_buf+offset, mlen-offset, 
            "<tcr:Description>%s</tcr:Description>", 
            p_res->Description);
    }

    offset += snprintf(p_buf+offset, mlen-offset, 
            "<tcr:CredentialHolderReference>%s</tcr:CredentialHolderReference>", 
            p_res->CredentialHolderReference);

    if (p_res->ValidFromFlag)
    {
        offset += snprintf(p_buf+offset, mlen-offset, 
            "<tcr:ValidFrom>%s</tcr:ValidFrom>", 
            p_res->ValidFrom);
    }

    if (p_res->ValidToFlag)
    {
        offset += snprintf(p_buf+offset, mlen-offset, 
            "<tcr:ValidTo>%s</tcr:ValidTo>", 
            p_res->ValidTo);
    }
    
    return offset;
}

int build_CredentialIdentifierType_xml(char * p_buf, int mlen, onvif_CredentialIdentifierType * p_res)
{
    int offset = 0;

    offset += snprintf(p_buf+offset, mlen-offset, 
        "<tcr:Name>%s</tcr:Name>"
        "<tcr:FormatType>%s</tcr:FormatType>",
        p_res->Name,
        p_res->FormatType);

    return offset;
}

int build_CredentialIdentifier_xml(char * p_buf, int mlen, onvif_CredentialIdentifier * p_res)
{
    int offset = 0;

    offset += snprintf(p_buf+offset, mlen-offset, "<tcr:Type>");
    offset += build_CredentialIdentifierType_xml(p_buf+offset, mlen-offset, &p_res->Type);   
    offset += snprintf(p_buf+offset, mlen-offset, "</tcr:Type>");

    offset += snprintf(p_buf+offset, mlen-offset, 
        "<tcr:ExemptedFromAuthentication>%s</tcr:ExemptedFromAuthentication>"
        "<tcr:Value>%s</tcr:Value>",
        p_res->ExemptedFromAuthentication ? "true" : "false",
        p_res->Value);

    return offset;
}

int build_CredentialAccessProfile_xml(char * p_buf, int mlen, onvif_CredentialAccessProfile * p_res)
{
    int offset = 0;

    offset += snprintf(p_buf+offset, mlen-offset, 
        "<tcr:AccessProfileToken>%s</tcr:AccessProfileToken>",
        p_res->AccessProfileToken);

    if (p_res->ValidFromFlag)
    {
        offset += snprintf(p_buf+offset, mlen-offset, 
            "<tcr:ValidFrom>%s</tcr:ValidFrom>", 
            p_res->ValidFrom);
    }

    if (p_res->ValidToFlag)
    {
        offset += snprintf(p_buf+offset, mlen-offset, 
            "<tcr:ValidTo>%s</tcr:ValidTo>", 
            p_res->ValidTo);
    }
    
    return offset;        
}

int build_Credential_xml(char * p_buf, int mlen, onvif_Credential * p_res)
{
    int i;
    int offset = 0;

    if (p_res->DescriptionFlag)
    {
        offset += snprintf(p_buf+offset, mlen-offset, 
            "<tcr:Description>%s</tcr:Description>", 
            p_res->Description);
    }

    offset += snprintf(p_buf+offset, mlen-offset, 
            "<tcr:CredentialHolderReference>%s</tcr:CredentialHolderReference>", 
            p_res->CredentialHolderReference);

    if (p_res->ValidFromFlag)
    {
        offset += snprintf(p_buf+offset, mlen-offset, 
            "<tcr:ValidFrom>%s</tcr:ValidFrom>", 
            p_res->ValidFrom);
    }

    if (p_res->ValidToFlag)
    {
        offset += snprintf(p_buf+offset, mlen-offset, 
            "<tcr:ValidTo>%s</tcr:ValidTo>", 
            p_res->ValidTo);
    }

    for (i = 0; i < p_res->sizeCredentialIdentifier; i++)
    {
        offset += snprintf(p_buf+offset, mlen-offset, "<tcr:CredentialIdentifier>");
        offset += build_CredentialIdentifier_xml(p_buf+offset, mlen-offset, &p_res->CredentialIdentifier[i]);
        offset += snprintf(p_buf+offset, mlen-offset, "</tcr:CredentialIdentifier>");
    }

    for (i = 0; i < p_res->sizeCredentialAccessProfile; i++)
    {
        offset += snprintf(p_buf+offset, mlen-offset, "<tcr:CredentialAccessProfile>");
        offset += build_CredentialAccessProfile_xml(p_buf+offset, mlen-offset, &p_res->CredentialAccessProfile[i]);
        offset += snprintf(p_buf+offset, mlen-offset, "</tcr:CredentialAccessProfile>");
    }

    for (i = 0; i < p_res->sizeAttribute; i++)
    {
        offset += snprintf(p_buf+offset, mlen-offset, 
            "<tcr:Attribute  Name=\"%s\"",
            p_res->Attribute[i].Name);

        if (p_res->Attribute[i].ValueFlag)
        {
            offset += snprintf(p_buf+offset, mlen-offset, 
                " Value=\"%s\"",
                p_res->Attribute[i].Value);
        }

        offset += snprintf(p_buf+offset, mlen-offset, "/>");
    }
    
    return offset;
}

int build_CredentialIdentifierFormatTypeInfo_xml(char * p_buf, int mlen, onvif_CredentialIdentifierFormatTypeInfo * p_res)
{
    int offset = 0;

    offset += snprintf(p_buf+offset, mlen-offset, 
        "<tcr:FormatType>%s</tcr:FormatType>"
        "<tcr:Description>%s</tcr:Description>",
        p_res->FormatType,
        p_res->Description);

    return offset;
}

int build_CredentialState_xml(char * p_buf, int mlen, onvif_CredentialState * p_res)
{
    int offset = 0;
    
    offset += snprintf(p_buf+offset, mlen-offset, 
	    "<tcr:Enabled>%s</tcr:Enabled>", 
	    p_res->Enabled ? "true" : "false");

	if (p_res->ReasonFlag)
	{
	    offset += snprintf(p_buf+offset, mlen-offset, "<tcr:Reason>%s</tcr:Reason>", p_res->Reason);
	}
	
	if (p_res->AntipassbackStateFlag)
	{
	    offset += snprintf(p_buf+offset, mlen-offset, 
	        "<tcr:AntipassbackState>"
	            "<tcr:AntipassbackViolated>%s</tcr:AntipassbackViolated>"
	        "</tcr:AntipassbackState>", 
	        p_res->AntipassbackState.AntipassbackViolated ? "true" : "false");
	}

	return offset;
}

int build_tcr_GetCredentialInfo_rly_xml(char * p_buf, int mlen, const char * argv)
{
    int i;
    int offset = 0;
    tcr_GetCredentialInfo_RES * p_res = (tcr_GetCredentialInfo_RES *)argv;

	offset += snprintf(p_buf+offset, mlen-offset, "<tcr:GetCredentialInfoResponse>");

	for (i = 0; i < p_res->sizeCredentialInfo; i++)
	{
        offset += snprintf(p_buf+offset, mlen-offset, "<tcr:CredentialInfo token=\"%s\">", p_res->CredentialInfo[i].token);
        offset += build_CredentialInfo_xml(p_buf+offset, mlen-offset, &p_res->CredentialInfo[i]);
        offset += snprintf(p_buf+offset, mlen-offset, "</tcr:CredentialInfo>");
    }
    
    offset += snprintf(p_buf+offset, mlen-offset, "</tcr:GetCredentialInfoResponse>");
    
    return offset;
}

int build_tcr_GetCredentialInfoList_rly_xml(char * p_buf, int mlen, const char * argv)
{
	int i;
    int offset = 0;
    tcr_GetCredentialInfoList_RES * p_res = (tcr_GetCredentialInfoList_RES *)argv;

	offset += snprintf(p_buf+offset, mlen-offset, "<tcr:GetCredentialInfoListResponse>");

    if (p_res->NextStartReferenceFlag)
    {
        offset += snprintf(p_buf+offset, mlen-offset, 
            "<tcr:NextStartReference>%s</tcr:NextStartReference>",
            p_res->NextStartReference);
    }
    
	for (i = 0; i < p_res->sizeCredentialInfo; i++)
	{
        offset += snprintf(p_buf+offset, mlen-offset, "<tcr:CredentialInfo token=\"%s\">", p_res->CredentialInfo[i].token);
        offset += build_CredentialInfo_xml(p_buf+offset, mlen-offset, &p_res->CredentialInfo[i]);
        offset += snprintf(p_buf+offset, mlen-offset, "</tcr:CredentialInfo>");
    }
    
    offset += snprintf(p_buf+offset, mlen-offset, "</tcr:GetCredentialInfoListResponse>");
    
    return offset;
}

int build_tcr_GetCredentials_rly_xml(char * p_buf, int mlen, const char * argv)
{
    int i;
    int offset = 0;
    tcr_GetCredentials_RES * p_res = (tcr_GetCredentials_RES *)argv;

	offset += snprintf(p_buf+offset, mlen-offset, "<tcr:GetCredentialsResponse>");
    
	for (i = 0; i < p_res->sizeCredential; i++)
	{
        offset += snprintf(p_buf+offset, mlen-offset, "<tcr:Credential token=\"%s\">", p_res->Credential[i].token);
        offset += build_Credential_xml(p_buf+offset, mlen-offset, &p_res->Credential[i]);
        offset += snprintf(p_buf+offset, mlen-offset, "</tcr:Credential>");
    }
    
    offset += snprintf(p_buf+offset, mlen-offset, "</tcr:GetCredentialsResponse>");
    
    return offset;
}

int build_tcr_GetCredentialList_rly_xml(char * p_buf, int mlen, const char * argv)
{
    int i;
    int offset = 0;
    tcr_GetCredentialList_RES * p_res = (tcr_GetCredentialList_RES *)argv;

	offset += snprintf(p_buf+offset, mlen-offset, "<tcr:GetCredentialListResponse>");

    if (p_res->NextStartReferenceFlag)
    {
        offset += snprintf(p_buf+offset, mlen-offset, 
            "<tcr:NextStartReference>%s</tcr:NextStartReference>",
            p_res->NextStartReference);
    }
    
	for (i = 0; i < p_res->sizeCredential; i++)
	{
        offset += snprintf(p_buf+offset, mlen-offset, "<tcr:Credential token=\"%s\">", p_res->Credential[i].token);
        offset += build_Credential_xml(p_buf+offset, mlen-offset, &p_res->Credential[i]);
        offset += snprintf(p_buf+offset, mlen-offset, "</tcr:Credential>");
    }
    
    offset += snprintf(p_buf+offset, mlen-offset, "</tcr:GetCredentialListResponse>");
    
    return offset;
}

int build_tcr_CreateCredential_rly_xml(char * p_buf, int mlen, const char * argv)
{
    int offset = 0;
    tcr_CreateCredential_RES * p_res = (tcr_CreateCredential_RES *)argv;

	offset += snprintf(p_buf+offset, mlen-offset, 
	    "<tcr:CreateCredentialResponse>"
	        "<tcr:Token>%s</tcr:Token>"
	    "</tcr:CreateCredentialResponse>",
	    p_res->Token);
	    
    return offset;
}

int build_tcr_ModifyCredential_rly_xml(char * p_buf, int mlen, const char * argv)
{
    int offset = 0;
	offset += snprintf(p_buf+offset, mlen-offset, "<tcr:ModifyCredentialResponse />");
    return offset;
}

int build_tcr_DeleteCredential_rly_xml(char * p_buf, int mlen, const char * argv)
{
    int offset = 0;
	offset += snprintf(p_buf+offset, mlen-offset, "<tcr:DeleteCredentialResponse />");
    return offset;
}

int build_tcr_GetCredentialState_rly_xml(char * p_buf, int mlen, const char * argv)
{
    int offset = 0;
    tcr_GetCredentialState_RES * p_res = (tcr_GetCredentialState_RES *)argv;

	offset += snprintf(p_buf+offset, mlen-offset, "<tcr:GetCredentialStateResponse>");
	
	offset += snprintf(p_buf+offset, mlen-offset, "<tcr:State>");
    offset += build_CredentialState_xml(p_buf+offset, mlen-offset, &p_res->State);	
	offset += snprintf(p_buf+offset, mlen-offset, "</tcr:State>");
	
	offset += snprintf(p_buf+offset, mlen-offset, "</tcr:GetCredentialStateResponse>");
    
    return offset;
}

int build_tcr_EnableCredential_rly_xml(char * p_buf, int mlen, const char * argv)
{
    int offset = 0;
	offset += snprintf(p_buf+offset, mlen-offset, "<tcr:EnableCredentialResponse />");

    return offset;
}

int build_tcr_DisableCredential_rly_xml(char * p_buf, int mlen, const char * argv)
{
    int offset = 0;
	offset += snprintf(p_buf+offset, mlen-offset, "<tcr:DisableCredentialResponse />");
    return offset;
}

int build_tcr_ResetAntipassbackViolation_rly_xml(char * p_buf, int mlen, const char * argv)
{
    int offset = 0;
	offset += snprintf(p_buf+offset, mlen-offset, "<tcr:ResetAntipassbackViolationResponse />");
    return offset;
}

int build_tcr_GetSupportedFormatTypes_rly_xml(char * p_buf, int mlen, const char * argv)
{
    int i;
    int offset = 0;
    tcr_GetSupportedFormatTypes_RES * p_res = (tcr_GetSupportedFormatTypes_RES *)argv;

	offset += snprintf(p_buf+offset, mlen-offset, "<tcr:GetSupportedFormatTypesResponse>");

	for (i = 0; i < p_res->sizeFormatTypeInfo; i++)
	{
	    offset += snprintf(p_buf+offset, mlen-offset, "<tcr:FormatTypeInfo>");
	    offset += build_CredentialIdentifierFormatTypeInfo_xml(p_buf+offset, mlen-offset, &p_res->FormatTypeInfo[i]);
	    offset += snprintf(p_buf+offset, mlen-offset, "</tcr:FormatTypeInfo>");
	}
	
	offset += snprintf(p_buf+offset, mlen-offset, "</tcr:GetSupportedFormatTypesResponse>");
	    
    return offset;
}

int build_tcr_GetCredentialIdentifiers_rly_xml(char * p_buf, int mlen, const char * argv)
{
    int i;
    int offset = 0;
    tcr_GetCredentialIdentifiers_RES * p_res = (tcr_GetCredentialIdentifiers_RES *)argv;

	offset += snprintf(p_buf+offset, mlen-offset, "<tcr:GetCredentialIdentifiersResponse>");

	for (i = 0; i < p_res->sizeCredentialIdentifier; i++)
	{
	    offset += snprintf(p_buf+offset, mlen-offset, "<tcr:CredentialIdentifier>");
	    offset += build_CredentialIdentifier_xml(p_buf+offset, mlen-offset, &p_res->CredentialIdentifier[i]);
	    offset += snprintf(p_buf+offset, mlen-offset, "</tcr:CredentialIdentifier>");
	}
	
	offset += snprintf(p_buf+offset, mlen-offset, "</tcr:GetCredentialIdentifiersResponse>");
    
    return offset;
}

int build_tcr_SetCredentialIdentifier_rly_xml(char * p_buf, int mlen, const char * argv)
{
    int offset = 0;
	offset += snprintf(p_buf+offset, mlen-offset, "<tcr:SetCredentialIdentifierResponse />");
    return offset;
}

int build_tcr_DeleteCredentialIdentifier_rly_xml(char * p_buf, int mlen, const char * argv)
{
    int offset = 0;
	offset += snprintf(p_buf+offset, mlen-offset, "<tcr:DeleteCredentialIdentifierResponse />");
    return offset;
}

int build_tcr_GetCredentialAccessProfiles_rly_xml(char * p_buf, int mlen, const char * argv)
{
    int i;
    int offset = 0;
    tcr_GetCredentialAccessProfiles_RES * p_res = (tcr_GetCredentialAccessProfiles_RES *)argv;

	offset += snprintf(p_buf+offset, mlen-offset, "<tcr:GetCredentialAccessProfilesResponse>");

	for (i = 0; i < p_res->sizeCredentialAccessProfile; i++)
	{
	    offset += snprintf(p_buf+offset, mlen-offset, "<tcr:CredentialAccessProfile>");
	    offset += build_CredentialAccessProfile_xml(p_buf+offset, mlen-offset, &p_res->CredentialAccessProfile[i]);
	    offset += snprintf(p_buf+offset, mlen-offset, "</tcr:CredentialAccessProfile>");
	}
	
	offset += snprintf(p_buf+offset, mlen-offset, "</tcr:GetCredentialAccessProfilesResponse>");
	    
    return offset;
}

int build_tcr_SetCredentialAccessProfiles_rly_xml(char * p_buf, int mlen, const char * argv)
{
    int offset = 0;
	offset += snprintf(p_buf+offset, mlen-offset, "<tcr:SetCredentialAccessProfilesResponse />");
    return offset;
}

int build_tcr_DeleteCredentialAccessProfiles_rly_xml(char * p_buf, int mlen, const char * argv)
{
    int offset = 0;
	offset += snprintf(p_buf+offset, mlen-offset, "<tcr:DeleteCredentialAccessProfilesResponse />");
    return offset;
}

#endif // end of CREDENTIAL_SUPPORT

#ifdef ACCESS_RULES

int build_AccessProfileInfo_xml(char * p_buf, int mlen, onvif_AccessProfileInfo * p_res)
{
    int offset = 0;

    offset += snprintf(p_buf+offset, mlen-offset, "<tar:Name>%s</tar:Name>", p_res->Name);

    if (p_res->DescriptionFlag)
    {
        offset += snprintf(p_buf+offset, mlen-offset, 
            "<tar:Description>%s</tar:Description>", 
            p_res->Description);
    }

    return offset;
}

int build_AccessPolicy_xml(char * p_buf, int mlen, onvif_AccessPolicy * p_res)
{
    int offset = 0;

    offset += snprintf(p_buf+offset, mlen-offset, 
        "<tar:ScheduleToken>%s</tar:ScheduleToken>"
        "<tar:Entity>%s</tar:Entity>", 
        p_res->ScheduleToken,
        p_res->Entity);

    if (p_res->EntityTypeFlag)
    {
        offset += snprintf(p_buf+offset, mlen-offset, 
            "<tar:EntityType>%s</tar:EntityType>",
            p_res->EntityType);
    }

    return offset;
}

int build_AccessProfile_xml(char * p_buf, int mlen, onvif_AccessProfile * p_res)
{
    int i;
    int offset = 0;

    offset += snprintf(p_buf+offset, mlen-offset, "<tar:Name>%s</tar:Name>", p_res->Name);

    if (p_res->DescriptionFlag)
    {
        offset += snprintf(p_buf+offset, mlen-offset, 
            "<tar:Description>%s</tar:Description>", 
            p_res->Description);
    }

    for (i = 0; i < p_res->sizeAccessPolicy; i++)
    {
        offset += snprintf(p_buf+offset, mlen-offset, "<tar:AccessPolicy>");
        offset += build_AccessPolicy_xml(p_buf+offset, mlen-offset, &p_res->AccessPolicy[i]);
        offset += snprintf(p_buf+offset, mlen-offset, "</tar:AccessPolicy>");
    }

    return offset;
}

int build_tar_GetAccessProfileInfo_rly_xml(char * p_buf, int mlen, const char * argv)
{
    int i;
    int offset = 0;
    tar_GetAccessProfileInfo_RES * p_res = (tar_GetAccessProfileInfo_RES *)argv;

	offset += snprintf(p_buf+offset, mlen-offset, "<tar:GetAccessProfileInfoResponse>");

	for (i = 0; i < p_res->sizeAccessProfileInfo; i++)
	{
        offset += snprintf(p_buf+offset, mlen-offset, "<tar:AccessProfileInfo token=\"%s\">", p_res->AccessProfileInfo[i].token);
        offset += build_AccessProfileInfo_xml(p_buf+offset, mlen-offset, &p_res->AccessProfileInfo[i]);
        offset += snprintf(p_buf+offset, mlen-offset, "</tar:AccessProfileInfo>");
    }
    
    offset += snprintf(p_buf+offset, mlen-offset, "</tar:GetAccessProfileInfoResponse>");
    
    return offset;
}

int build_tar_GetAccessProfileInfoList_rly_xml(char * p_buf, int mlen, const char * argv)
{
    int i;
    int offset = 0;
    tar_GetAccessProfileInfoList_RES * p_res = (tar_GetAccessProfileInfoList_RES *)argv;

	offset += snprintf(p_buf+offset, mlen-offset, "<tar:GetAccessProfileInfoListResponse>");

    if (p_res->NextStartReferenceFlag)
    {
        offset += snprintf(p_buf+offset, mlen-offset, 
            "<tar:NextStartReference>%s</tar:NextStartReference>",
            p_res->NextStartReference);
    }
    
	for (i = 0; i < p_res->sizeAccessProfileInfo; i++)
	{
        offset += snprintf(p_buf+offset, mlen-offset, "<tar:AccessProfileInfo token=\"%s\">", p_res->AccessProfileInfo[i].token);
        offset += build_AccessProfileInfo_xml(p_buf+offset, mlen-offset, &p_res->AccessProfileInfo[i]);
        offset += snprintf(p_buf+offset, mlen-offset, "</tar:AccessProfileInfo>");
    }
    
    offset += snprintf(p_buf+offset, mlen-offset, "</tar:GetAccessProfileInfoListResponse>");
    
    return offset;
}

int build_tar_GetAccessProfiles_rly_xml(char * p_buf, int mlen, const char * argv)
{
    int i;
    int offset = 0;
    tar_GetAccessProfiles_RES * p_res = (tar_GetAccessProfiles_RES *)argv;

	offset += snprintf(p_buf+offset, mlen-offset, "<tar:GetAccessProfilesResponse>");
    
	for (i = 0; i < p_res->sizeAccessProfile; i++)
	{
        offset += snprintf(p_buf+offset, mlen-offset, "<tar:AccessProfile token=\"%s\">", p_res->AccessProfile[i].token);
        offset += build_AccessProfile_xml(p_buf+offset, mlen-offset, &p_res->AccessProfile[i]);
        offset += snprintf(p_buf+offset, mlen-offset, "</tar:AccessProfile>");
    }
    
    offset += snprintf(p_buf+offset, mlen-offset, "</tar:GetAccessProfilesResponse>");
    
    return offset;
}

int build_tar_GetAccessProfileList_rly_xml(char * p_buf, int mlen, const char * argv)
{
    int i;
    int offset = 0;
    tar_GetAccessProfileList_RES * p_res = (tar_GetAccessProfileList_RES *)argv;

	offset += snprintf(p_buf+offset, mlen-offset, "<tar:GetAccessProfileListResponse>");

    if (p_res->NextStartReferenceFlag)
    {
        offset += snprintf(p_buf+offset, mlen-offset, 
            "<tar:NextStartReference>%s</tar:NextStartReference>",
            p_res->NextStartReference);
    }
    
	for (i = 0; i < p_res->sizeAccessProfile; i++)
	{
        offset += snprintf(p_buf+offset, mlen-offset, "<tar:AccessProfile token=\"%s\">", p_res->AccessProfile[i].token);
        offset += build_AccessProfile_xml(p_buf+offset, mlen-offset, &p_res->AccessProfile[i]);
        offset += snprintf(p_buf+offset, mlen-offset, "</tar:AccessProfile>");
    }
    
    offset += snprintf(p_buf+offset, mlen-offset, "</tar:GetAccessProfileListResponse>");
    
    return offset;
}

int build_tar_CreateAccessProfile_rly_xml(char * p_buf, int mlen, const char * argv)
{
    int offset = 0;
    tar_CreateAccessProfile_RES * p_res = (tar_CreateAccessProfile_RES *)argv;

	offset += snprintf(p_buf+offset, mlen-offset, 
	    "<tar:CreateAccessProfileResponse>"
	        "<tar:Token>%s</tar:Token>"
	    "</tar:CreateAccessProfileResponse>",
	    p_res->Token);
	    
    return offset;
}

int build_tar_ModifyAccessProfile_rly_xml(char * p_buf, int mlen, const char * argv)
{
    int offset = 0;
	offset += snprintf(p_buf+offset, mlen-offset, "<tar:ModifyAccessProfileResponse />");
    return offset;
}

int build_tar_DeleteAccessProfile_rly_xml(char * p_buf, int mlen, const char * argv)
{
    int offset = 0;
	offset += snprintf(p_buf+offset, mlen-offset, "<tar:DeleteAccessProfileResponse />");
    return offset;
}

#endif // end of ACCESS_RULES

#ifdef SCHEDULE_SUPPORT

int build_ScheduleInfo_xml(char * p_buf, int mlen, onvif_ScheduleInfo * p_res)
{
    int offset = 0;

    offset += snprintf(p_buf+offset, mlen-offset, "<tsc:Name>%s</tsc:Name>", p_res->Name);

    if (p_res->DescriptionFlag)
    {
        offset += snprintf(p_buf+offset, mlen-offset, "<tsc:Description>%s</tsc:Description>", p_res->Description);
    }

    return offset;
}

int build_TimePeriod_xml(char * p_buf, int mlen, onvif_TimePeriod * p_res)
{
    int offset = 0;

    offset += snprintf(p_buf+offset, mlen-offset, "<tsc:From>%s</tsc:From>", p_res->From);

    if (p_res->UntilFlag)
    {
        offset += snprintf(p_buf+offset, mlen-offset, "<tsc:Until>%s</tsc:Until>", p_res->Until);
    }
    
    return offset;
}

int build_SpecialDaysSchedule_xml(char * p_buf, int mlen, onvif_SpecialDaysSchedule * p_res)
{
    int i;
    int offset = 0;

    offset += snprintf(p_buf+offset, mlen-offset, "<tsc:GroupToken>%s</tsc:GroupToken>", p_res->GroupToken);

    for (i = 0; i < p_res->sizeTimeRange; i++)
    {
        offset += snprintf(p_buf+offset, mlen-offset, "<tsc:TimeRange>");
        offset += build_TimePeriod_xml(p_buf+offset, mlen-offset, &p_res->TimeRange[i]);
        offset += snprintf(p_buf+offset, mlen-offset, "</tsc:TimeRange>");
    }
    
    return offset;
}

int build_Schedule_xml(char * p_buf, int mlen, onvif_Schedule * p_res)
{
    int i;
    int offset = 0;

    offset += snprintf(p_buf+offset, mlen-offset, "<tsc:Name>%s</tsc:Name>", p_res->Name);

    if (p_res->DescriptionFlag)
    {
        offset += snprintf(p_buf+offset, mlen-offset, "<tsc:Description>%s</tsc:Description>", p_res->Description);
    }

    offset += snprintf(p_buf+offset, mlen-offset, "<tsc:Standard>%s</tsc:Standard>", p_res->Standard);

    for (i = 0; i < p_res->sizeSpecialDays; i++)
    {
        offset += snprintf(p_buf+offset, mlen-offset, "<tsc:SpecialDays>");
        offset += build_SpecialDaysSchedule_xml(p_buf+offset, mlen-offset, &p_res->SpecialDays[i]);
        offset += snprintf(p_buf+offset, mlen-offset, "</tsc:SpecialDays>");
    }
    
    return offset;
}

int build_SpecialDayGroupInfo_xml(char * p_buf, int mlen, onvif_SpecialDayGroupInfo * p_res)
{
    int offset = 0;

    offset += snprintf(p_buf+offset, mlen-offset, "<tsc:Name>%s</tsc:Name>", p_res->Name);

    if (p_res->DescriptionFlag)
    {
        offset += snprintf(p_buf+offset, mlen-offset, "<tsc:Description>%s</tsc:Description>", p_res->Description);
    }

    return offset;
}

int build_SpecialDayGroup_xml(char * p_buf, int mlen, onvif_SpecialDayGroup * p_res)
{
    int offset = 0;

    offset += snprintf(p_buf+offset, mlen-offset, "<tsc:Name>%s</tsc:Name>", p_res->Name);

    if (p_res->DescriptionFlag)
    {
        offset += snprintf(p_buf+offset, mlen-offset, "<tsc:Description>%s</tsc:Description>", p_res->Description);
    }

    if (p_res->DaysFlag)
    {
        offset += snprintf(p_buf+offset, mlen-offset, "<tsc:Days>%s</tsc:Days>", p_res->Days);
    }

    return offset;
}

int build_tsc_GetScheduleInfo_rly_xml(char * p_buf, int mlen, const char * argv)
{
    int i;
    int offset = 0;
    tsc_GetScheduleInfo_RES * p_res = (tsc_GetScheduleInfo_RES *)argv;

	offset += snprintf(p_buf+offset, mlen-offset, "<tsc:GetScheduleInfoResponse>");

	for (i = 0; i < p_res->sizeScheduleInfo; i++)
	{
        offset += snprintf(p_buf+offset, mlen-offset, "<tsc:ScheduleInfo token=\"%s\">", p_res->ScheduleInfo[i].token);
        offset += build_ScheduleInfo_xml(p_buf+offset, mlen-offset, &p_res->ScheduleInfo[i]);
        offset += snprintf(p_buf+offset, mlen-offset, "</tsc:ScheduleInfo>");
    }
    
    offset += snprintf(p_buf+offset, mlen-offset, "</tsc:GetScheduleInfoResponse>");
    
    return offset;
}

int build_tsc_GetScheduleInfoList_rly_xml(char * p_buf, int mlen, const char * argv)
{
    int i;
    int offset = 0;
    tsc_GetScheduleInfoList_RES * p_res = (tsc_GetScheduleInfoList_RES *)argv;

	offset += snprintf(p_buf+offset, mlen-offset, "<tsc:GetScheduleInfoListResponse>");

    if (p_res->NextStartReferenceFlag)
    {
        offset += snprintf(p_buf+offset, mlen-offset, 
            "<tsc:NextStartReference>%s</tsc:NextStartReference>",
            p_res->NextStartReference);
    }
    
	for (i = 0; i < p_res->sizeScheduleInfo; i++)
	{
        offset += snprintf(p_buf+offset, mlen-offset, "<tsc:ScheduleInfo token=\"%s\">", p_res->ScheduleInfo[i].token);
        offset += build_ScheduleInfo_xml(p_buf+offset, mlen-offset, &p_res->ScheduleInfo[i]);
        offset += snprintf(p_buf+offset, mlen-offset, "</tsc:ScheduleInfo>");
    }
    
    offset += snprintf(p_buf+offset, mlen-offset, "</tsc:GetScheduleInfoListResponse>");
    
    return offset;
}

int build_tsc_GetSchedules_rly_xml(char * p_buf, int mlen, const char * argv)
{
    int i;
    int offset = 0;
    tsc_GetSchedules_RES * p_res = (tsc_GetSchedules_RES *)argv;

	offset += snprintf(p_buf+offset, mlen-offset, "<tsc:GetSchedulesResponse>");

	for (i = 0; i < p_res->sizeSchedule; i++)
	{
        offset += snprintf(p_buf+offset, mlen-offset, "<tsc:Schedule token=\"%s\">", p_res->Schedule[i].token);
        offset += build_Schedule_xml(p_buf+offset, mlen-offset, &p_res->Schedule[i]);
        offset += snprintf(p_buf+offset, mlen-offset, "</tsc:Schedule>");
    }
    
    offset += snprintf(p_buf+offset, mlen-offset, "</tsc:GetSchedulesResponse>");
    
    return offset;
}

int build_tsc_GetScheduleList_rly_xml(char * p_buf, int mlen, const char * argv)
{
    int i;
    int offset = 0;
    tsc_GetScheduleList_RES * p_res = (tsc_GetScheduleList_RES *)argv;

	offset += snprintf(p_buf+offset, mlen-offset, "<tsc:GetScheduleListResponse>");

    if (p_res->NextStartReferenceFlag)
    {
        offset += snprintf(p_buf+offset, mlen-offset, 
            "<tsc:NextStartReference>%s</tsc:NextStartReference>",
            p_res->NextStartReference);
    }
    
	for (i = 0; i < p_res->sizeSchedule; i++)
	{
        offset += snprintf(p_buf+offset, mlen-offset, "<tsc:Schedule token=\"%s\">", p_res->Schedule[i].token);
        offset += build_Schedule_xml(p_buf+offset, mlen-offset, &p_res->Schedule[i]);
        offset += snprintf(p_buf+offset, mlen-offset, "</tsc:Schedule>");
    }
    
    offset += snprintf(p_buf+offset, mlen-offset, "</tsc:GetScheduleListResponse>");
    
    return offset;
}

int build_tsc_CreateSchedule_rly_xml(char * p_buf, int mlen, const char * argv)
{
    int offset = 0;
    tsc_CreateSchedule_RES * p_res = (tsc_CreateSchedule_RES *)argv;

	offset += snprintf(p_buf+offset, mlen-offset, 
	    "<tsc:CreateScheduleResponse>"
	        "<tsc:Token>%s</tsc:Token>"
	    "</tsc:CreateScheduleResponse>",
	    p_res->Token);
    
    return offset;
}

int build_tsc_ModifySchedule_rly_xml(char * p_buf, int mlen, const char * argv)
{
    int offset = 0;
	offset += snprintf(p_buf+offset, mlen-offset, "<tsc:ModifyScheduleResponse />");
    return offset;
}

int build_tsc_DeleteSchedule_rly_xml(char * p_buf, int mlen, const char * argv)
{
    int offset = 0;
	offset += snprintf(p_buf+offset, mlen-offset, "<tsc:DeleteScheduleResponse />");
    return offset;
}

int build_tsc_GetSpecialDayGroupInfo_rly_xml(char * p_buf, int mlen, const char * argv)
{
    int i;
    int offset = 0;
    tsc_GetSpecialDayGroupInfo_RES * p_res = (tsc_GetSpecialDayGroupInfo_RES *)argv;

	offset += snprintf(p_buf+offset, mlen-offset, "<tsc:GetSpecialDayGroupInfoResponse>");

	for (i = 0; i < p_res->sizeSpecialDayGroupInfo; i++)
	{
        offset += snprintf(p_buf+offset, mlen-offset, "<tsc:SpecialDayGroupInfo token=\"%s\">", p_res->SpecialDayGroupInfo[i].token);
        offset += build_SpecialDayGroupInfo_xml(p_buf+offset, mlen-offset, &p_res->SpecialDayGroupInfo[i]);
        offset += snprintf(p_buf+offset, mlen-offset, "</tsc:SpecialDayGroupInfo>");
    }
    
    offset += snprintf(p_buf+offset, mlen-offset, "</tsc:GetSpecialDayGroupInfoResponse>");
    
    return offset;
}

int build_tsc_GetSpecialDayGroupInfoList_rly_xml(char * p_buf, int mlen, const char * argv)
{
    int i;
    int offset = 0;
    tsc_GetSpecialDayGroupInfoList_RES * p_res = (tsc_GetSpecialDayGroupInfoList_RES *)argv;

	offset += snprintf(p_buf+offset, mlen-offset, "<tsc:GetSpecialDayGroupInfoListResponse>");

    if (p_res->NextStartReferenceFlag)
    {
        offset += snprintf(p_buf+offset, mlen-offset, 
            "<tsc:NextStartReference>%s</tsc:NextStartReference>",
            p_res->NextStartReference);
    }
    
	for (i = 0; i < p_res->sizeSpecialDayGroupInfo; i++)
	{
        offset += snprintf(p_buf+offset, mlen-offset, "<tsc:SpecialDayGroupInfo token=\"%s\">", p_res->SpecialDayGroupInfo[i].token);
        offset += build_SpecialDayGroupInfo_xml(p_buf+offset, mlen-offset, &p_res->SpecialDayGroupInfo[i]);
        offset += snprintf(p_buf+offset, mlen-offset, "</tsc:SpecialDayGroupInfo>");
    }
    
    offset += snprintf(p_buf+offset, mlen-offset, "</tsc:GetSpecialDayGroupInfoListResponse>");
    
    return offset;
}

int build_tsc_GetSpecialDayGroups_rly_xml(char * p_buf, int mlen, const char * argv)
{
    int i;
    int offset = 0;
    tsc_GetSpecialDayGroups_RES * p_res = (tsc_GetSpecialDayGroups_RES *)argv;

	offset += snprintf(p_buf+offset, mlen-offset, "<tsc:GetSpecialDayGroupsResponse>");

	for (i = 0; i < p_res->sizeSpecialDayGroup; i++)
	{
        offset += snprintf(p_buf+offset, mlen-offset, "<tsc:SpecialDayGroup token=\"%s\">", p_res->SpecialDayGroup[i].token);
        offset += build_SpecialDayGroup_xml(p_buf+offset, mlen-offset, &p_res->SpecialDayGroup[i]);
        offset += snprintf(p_buf+offset, mlen-offset, "</tsc:SpecialDayGroup>");
    }
    
    offset += snprintf(p_buf+offset, mlen-offset, "</tsc:GetSpecialDayGroupsResponse>");
    
    return offset;
}

int build_tsc_GetSpecialDayGroupList_rly_xml(char * p_buf, int mlen, const char * argv)
{
    int i;
    int offset = 0;
    tsc_GetSpecialDayGroupList_RES * p_res = (tsc_GetSpecialDayGroupList_RES *)argv;

	offset += snprintf(p_buf+offset, mlen-offset, "<tsc:GetSpecialDayGroupListResponse>");

    if (p_res->NextStartReferenceFlag)
    {
        offset += snprintf(p_buf+offset, mlen-offset, 
            "<tsc:NextStartReference>%s</tsc:NextStartReference>",
            p_res->NextStartReference);
    }
    
	for (i = 0; i < p_res->sizeSpecialDayGroup; i++)
	{
        offset += snprintf(p_buf+offset, mlen-offset, "<tsc:SpecialDayGroup token=\"%s\">", p_res->SpecialDayGroup[i].token);
        offset += build_SpecialDayGroup_xml(p_buf+offset, mlen-offset, &p_res->SpecialDayGroup[i]);
        offset += snprintf(p_buf+offset, mlen-offset, "</tsc:SpecialDayGroup>");
    }
    
    offset += snprintf(p_buf+offset, mlen-offset, "</tsc:GetSpecialDayGroupListResponse>");
    
    return offset;
}

int build_tsc_CreateSpecialDayGroup_rly_xml(char * p_buf, int mlen, const char * argv)
{
    int offset = 0;
    tsc_CreateSpecialDayGroup_RES * p_res = (tsc_CreateSpecialDayGroup_RES *)argv;

	offset += snprintf(p_buf+offset, mlen-offset, 
	    "<tsc:CreateSpecialDayGroupResponse>"
	        "<tsc:Token>%s</tsc:Token>"
	    "</tsc:CreateSpecialDayGroupResponse>",
	    p_res->Token);
    
    return offset;
}

int build_tsc_ModifySpecialDayGroup_rly_xml(char * p_buf, int mlen, const char * argv)
{
    int offset = 0;
	offset += snprintf(p_buf+offset, mlen-offset, "<tsc:ModifySpecialDayGroupResponse />");
    return offset;
}

int build_tsc_DeleteSpecialDayGroup_rly_xml(char * p_buf, int mlen, const char * argv)
{
    int offset = 0;
	offset += snprintf(p_buf+offset, mlen-offset, "<tsc:DeleteSpecialDayGroupResponse />");
    return offset;
}

int build_tsc_GetScheduleState_rly_xml(char * p_buf, int mlen, const char * argv)
{
    int offset = 0;
    tsc_GetScheduleState_RES * p_res = (tsc_GetScheduleState_RES *)argv;

	offset += snprintf(p_buf+offset, mlen-offset, "<tsc:GetScheduleStateResponse><ScheduleState>");
    offset += snprintf(p_buf+offset, mlen-offset, "<tsc:Active>%s</tsc:Active>", p_res->ScheduleState.Active ? "true" : "false");
    
    if (p_res->ScheduleState.SpecialDayFlag)
    {
        offset += snprintf(p_buf+offset, mlen-offset, "<tsc:SpecialDay>%s</tsc:SpecialDay>", 
            p_res->ScheduleState.SpecialDay ? "true" : "false");
    }
    
	offset += snprintf(p_buf+offset, mlen-offset, "</ScheduleState></tsc:GetScheduleStateResponse>");
	    
    return offset;
}

#endif // end of SCHEDULE_SUPPORT

#ifdef RECEIVER_SUPPORT

int build_trv_ReceiverConfiguration_xml(char * p_buf, int mlen, onvif_ReceiverConfiguration * p_res)
{
    int offset = 0;

    offset += snprintf(p_buf+offset, mlen-offset, 
        "<tt:Mode>%s</tt:Mode>"
        "<tt:MediaUri>%s</tt:MediaUri>", 
        onvif_ReceiverModeToString(p_res->Mode), 
        p_res->MediaUri);

    offset += snprintf(p_buf+offset, mlen-offset, "<tt:StreamSetup>");
    offset += build_StreamSetup_xml(p_buf+offset, mlen-offset, &p_res->StreamSetup);
    offset += snprintf(p_buf+offset, mlen-offset, "</tt:StreamSetup>");
    
    return offset;        
}

int build_trv_Receiver_xml(char * p_buf, int mlen, onvif_Receiver * p_res)
{
    int offset = 0;

    offset += snprintf(p_buf+offset, mlen-offset, "<tt:Token>%s</tt:Token>", p_res->Token);

    offset += snprintf(p_buf+offset, mlen-offset, "<tt:Configuration>");
    offset += build_trv_ReceiverConfiguration_xml(p_buf+offset, mlen-offset, &p_res->Configuration);
    offset += snprintf(p_buf+offset, mlen-offset, "</tt:Configuration>");

    return offset;
}

int build_trv_GetReceivers_rly_xml(char * p_buf, int mlen, const char * argv)
{
    int offset = 0;
    trv_GetReceivers_RES * p_res = (trv_GetReceivers_RES *)argv;
    ONVIF_Receiver * p_Receiver;

    p_Receiver = p_res->Receivers;
    
	offset += snprintf(p_buf+offset, mlen-offset, "<trv:GetReceiversResponse>");

    while (p_Receiver)
    {
        offset += snprintf(p_buf+offset, mlen-offset, "<trv:Receivers>");
        offset += build_trv_Receiver_xml(p_buf+offset, mlen-offset, &p_Receiver->Receiver);
        offset += snprintf(p_buf+offset, mlen-offset, "</trv:Receivers>");
        
        p_Receiver = p_Receiver->next;
    }
	
	offset += snprintf(p_buf+offset, mlen-offset, "</trv:GetReceiversResponse>");
	    
    return offset;
}

int build_trv_GetReceiver_rly_xml(char * p_buf, int mlen, const char * argv)
{
    int offset = 0;
    trv_GetReceiver_RES * p_res = (trv_GetReceiver_RES *)argv;

    offset += snprintf(p_buf+offset, mlen-offset, "<trv:GetReceiverResponse><trv:Receiver>");
    offset += build_trv_Receiver_xml(p_buf+offset, mlen-offset, &p_res->Receiver);
    offset += snprintf(p_buf+offset, mlen-offset, "</trv:Receiver></trv:GetReceiverResponse>");

    return offset;
}

int build_trv_CreateReceiver_rly_xml(char * p_buf, int mlen, const char * argv)
{
    int offset = 0;
    trv_CreateReceiver_RES * p_res = (trv_CreateReceiver_RES *)argv;

    offset += snprintf(p_buf+offset, mlen-offset, "<trv:CreateReceiverResponse><trv:Receiver>");
    offset += build_trv_Receiver_xml(p_buf+offset, mlen-offset, &p_res->Receiver);
    offset += snprintf(p_buf+offset, mlen-offset, "</trv:Receiver></trv:CreateReceiverResponse>");

    return offset;
}

int build_trv_DeleteReceiver_rly_xml(char * p_buf, int mlen, const char * argv)
{
    int offset = 0;
    offset += snprintf(p_buf+offset, mlen-offset, "<trv:DeleteReceiverResponse />");
    return offset;
}

int build_trv_ConfigureReceiver_rly_xml(char * p_buf, int mlen, const char * argv)
{
    int offset = 0;
    offset += snprintf(p_buf+offset, mlen-offset, "<trv:ConfigureReceiverResponse />");
    return offset;
}

int build_trv_SetReceiverMode_rly_xml(char * p_buf, int mlen, const char * argv)
{
    int offset = 0;
    offset += snprintf(p_buf+offset, mlen-offset, "<trv:SetReceiverModeResponse />");
    return offset;
}

int build_trv_GetReceiverState_rly_xml(char * p_buf, int mlen, const char * argv)
{
    int offset = 0;
    trv_GetReceiverState_RES * p_res = (trv_GetReceiverState_RES *)argv;
    
    offset += snprintf(p_buf+offset, mlen-offset, 
        "<trv:GetReceiverStateResponse>"
            "<trv:ReceiverState>"
                "<tt:State>%s</tt:State>"
                "<tt:AutoCreated>%s</tt:AutoCreated>"
            "</trv:ReceiverState>"
        "</trv:GetReceiverStateResponse>",
        onvif_ReceiverStateToString(p_res->ReceiverState.State),
        p_res->ReceiverState.AutoCreated ? "true" : "false");

    return offset;                
}

#endif // end of RECEIVER_SUPPORT


