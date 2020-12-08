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
#include "onvif.h"
#include "onvif_media.h"
#include "onvif_utils.h"

/***************************************************************************************/
extern ONVIF_CFG g_onvif_cfg;
extern ONVIF_CLS g_onvif_cls;


/***************************************************************************************/
ONVIF_RET onvif_CreateProfile(const char * lip, uint32 lport, CreateProfile_REQ * p_req)
{
	ONVIF_PROFILE * p_profile = NULL;

	if (p_req->TokenFlag && p_req->Token[0] != '\0')
	{
		p_profile = onvif_find_profile(p_req->Token);
		if (p_profile)
		{
			return ONVIF_ERR_ProfileExists;
		}
	}
	
	p_profile = onvif_add_profile(FALSE);
	if (p_profile)
	{
		strcpy(p_profile->name, p_req->Name);
		if (p_req->TokenFlag && p_req->Token[0] != '\0')
		{
			strcpy(p_profile->token, p_req->Token);
		}
		else
		{
			strcpy(p_req->Token, p_profile->token);
		}

		// setup the new profile stream uri	
		if (g_onvif_cfg.profiles)
		{
			strcpy(p_profile->stream_uri, g_onvif_cfg.profiles->stream_uri);
		}		
	}
	else 
	{
		return ONVIF_ERR_MaxNVTProfiles;
	}
	
	return ONVIF_OK;
}

ONVIF_RET onvif_DeleteProfile(DeleteProfile_REQ * p_req)
{
    ONVIF_PROFILE * p_prev;
	ONVIF_PROFILE * p_profile = onvif_find_profile(p_req->ProfileToken);
	if (NULL == p_profile)
	{
		return ONVIF_ERR_NoProfile;
	}

	if (p_profile->fixed)
	{
		return ONVIF_ERR_DeletionOfFixedProfile;
	}

	p_prev = g_onvif_cfg.profiles;
	if (p_profile == p_prev)
	{
		g_onvif_cfg.profiles = p_profile->next;
	}
	else
	{
		while (p_prev->next)
		{
			if (p_prev->next == p_profile)
			{
				break;
			}

			p_prev = p_prev->next;
		}

		p_prev->next = p_profile->next;
	}

	if (p_profile->v_src_cfg && p_profile->v_src_cfg->Configuration.UseCount > 0)
	{
		p_profile->v_src_cfg->Configuration.UseCount--;
	}
	
	if (p_profile->v_enc_cfg && p_profile->v_enc_cfg->Configuration.UseCount > 0)
	{
		p_profile->v_enc_cfg->Configuration.UseCount--;
	}

#ifdef AUDIO_SUPPORT
	if (p_profile->a_src_cfg && p_profile->a_src_cfg->Configuration.UseCount > 0)
	{
		p_profile->a_src_cfg->Configuration.UseCount--;
	}

	if (p_profile->a_enc_cfg && p_profile->a_enc_cfg->Configuration.UseCount > 0)
	{
		p_profile->a_enc_cfg->Configuration.UseCount--;
	}
#endif

#ifdef PTZ_SUPPORT
	if (p_profile->ptz_cfg && p_profile->ptz_cfg->Configuration.UseCount > 0)
	{
		p_profile->ptz_cfg->Configuration.UseCount--;
	}
#endif

	if (p_profile->multicasting)
	{
		// todo : stop multicast streaming ...
	}

	free(p_profile);
	
	return ONVIF_OK;
}

ONVIF_RET onvif_AddVideoSourceConfiguration(AddVideoSourceConfiguration_REQ * p_req)
{
    ONVIF_VideoSourceConfiguration * p_v_src_cfg;
	ONVIF_PROFILE * p_profile = onvif_find_profile(p_req->ProfileToken);
	if (NULL == p_profile)
	{
		return ONVIF_ERR_NoProfile;
	}

	p_v_src_cfg = onvif_find_VideoSourceConfiguration(p_req->ConfigurationToken);
	if (NULL == p_v_src_cfg)
	{
		return ONVIF_ERR_NoConfig;
	}

	if (p_profile->v_src_cfg != p_v_src_cfg)
	{
		if (p_profile->v_src_cfg && p_profile->v_src_cfg->Configuration.UseCount > 0)
		{
			p_profile->v_src_cfg->Configuration.UseCount--;
		}
		
		p_v_src_cfg->Configuration.UseCount++;
		p_profile->v_src_cfg = p_v_src_cfg;
	}

	return ONVIF_OK;
}

ONVIF_RET onvif_AddVideoEncoderConfiguration(AddVideoEncoderConfiguration_REQ * p_req)
{
    ONVIF_VideoEncoder2Configuration * p_v_enc_cfg;
	ONVIF_PROFILE * p_profile = onvif_find_profile(p_req->ProfileToken);
	if (NULL == p_profile)
	{
		return ONVIF_ERR_NoProfile;
	}

	p_v_enc_cfg = onvif_find_VideoEncoderConfiguration(p_req->ConfigurationToken);
	if (NULL == p_v_enc_cfg)
	{
		return ONVIF_ERR_NoConfig;
	}

	if (p_profile->v_enc_cfg != p_v_enc_cfg)
	{
		if (p_profile->v_enc_cfg && p_profile->v_enc_cfg->Configuration.UseCount > 0)
		{
			p_profile->v_enc_cfg->Configuration.UseCount--;
		}
		
		p_v_enc_cfg->Configuration.UseCount++;
		p_profile->v_enc_cfg = p_v_enc_cfg;
	}
	
	return ONVIF_OK;
}

ONVIF_RET onvif_RemoveVideoEncoderConfiguration(RemoveVideoEncoderConfiguration_REQ * p_req)
{
	ONVIF_PROFILE * p_profile = onvif_find_profile(p_req->ProfileToken);
	if (NULL == p_profile)
	{
		return ONVIF_ERR_NoProfile;
	}

	if (p_profile->v_enc_cfg && p_profile->v_enc_cfg->Configuration.UseCount > 0)
	{
		p_profile->v_enc_cfg->Configuration.UseCount--;
	}
	
	p_profile->v_enc_cfg = NULL;

	return ONVIF_OK;
}

ONVIF_RET onvif_RemoveVideoSourceConfiguration(RemoveVideoSourceConfiguration_REQ * p_req)
{
	ONVIF_PROFILE * p_profile = onvif_find_profile(p_req->ProfileToken);
	if (NULL == p_profile)
	{
		return ONVIF_ERR_NoProfile;
	}

	if (p_profile->v_src_cfg && p_profile->v_src_cfg->Configuration.UseCount > 0)
	{
		p_profile->v_src_cfg->Configuration.UseCount--;
	}
	
	p_profile->v_src_cfg = NULL;

	return ONVIF_OK;
}

/************************************************************************************
 *  	
 * Possible error:
 * 	ONVIF_ERR_ConfigModify
 *	ONVIF_ERR_NoConfig
 * 	ONVIF_ERR_ConfigurationConflict
 *
*************************************************************************************/
ONVIF_RET onvif_SetVideoEncoderConfiguration(SetVideoEncoderConfiguration_REQ * p_req)
{
    int i = 0;
    onvif_VideoResolution * p_VideoResolution;
	ONVIF_VideoEncoder2Configuration * p_v_enc_cfg;

	p_v_enc_cfg = onvif_find_VideoEncoderConfiguration(p_req->Configuration.token);
	if (NULL == p_v_enc_cfg)
	{
		return ONVIF_ERR_NoConfig;
	}

	if (p_req->Configuration.Quality < g_onvif_cfg.VideoEncoderConfigurationOptions.QualityRange.Min || 
		p_req->Configuration.Quality > g_onvif_cfg.VideoEncoderConfigurationOptions.QualityRange.Max )
	{
		return ONVIF_ERR_ConfigModify;
	}

	if (VideoEncoding_MPEG4 == p_req->Configuration.Encoding)
	{
		if (p_req->Configuration.MPEG4.GovLength < g_onvif_cfg.VideoEncoderConfigurationOptions.MPEG4.GovLengthRange.Min ||
			p_req->Configuration.MPEG4.GovLength > g_onvif_cfg.VideoEncoderConfigurationOptions.MPEG4.GovLengthRange.Max)
		{
			return ONVIF_ERR_ConfigModify;
		}

		p_VideoResolution = g_onvif_cfg.VideoEncoderConfigurationOptions.MPEG4.ResolutionsAvailable;
	}
	else if (VideoEncoding_H264 == p_req->Configuration.Encoding)
	{
		if (p_req->Configuration.H264.GovLength < g_onvif_cfg.VideoEncoderConfigurationOptions.H264.GovLengthRange.Min ||
			p_req->Configuration.H264.GovLength > g_onvif_cfg.VideoEncoderConfigurationOptions.H264.GovLengthRange.Max)
		{
			return ONVIF_ERR_ConfigModify;
		}

		p_VideoResolution = g_onvif_cfg.VideoEncoderConfigurationOptions.H264.ResolutionsAvailable;
	}
	else
	{
		p_VideoResolution = g_onvif_cfg.VideoEncoderConfigurationOptions.JPEG.ResolutionsAvailable;
	}

	for (i = 0; i < MAX_RES_NUMS; i++)
	{
		if (p_VideoResolution[i].Width == p_req->Configuration.Resolution.Width && 
			p_VideoResolution[i].Height == p_req->Configuration.Resolution.Height)
		{
			break;
		}
	}

	if (i == MAX_RES_NUMS)
	{
		return ONVIF_ERR_ConfigModify;
	}

	p_v_enc_cfg->Configuration.Resolution.Width = p_req->Configuration.Resolution.Width;
	p_v_enc_cfg->Configuration.Resolution.Height = p_req->Configuration.Resolution.Height;
	p_v_enc_cfg->Configuration.Quality = (float)p_req->Configuration.Quality;
	p_v_enc_cfg->Configuration.SessionTimeout = p_req->Configuration.SessionTimeout;
	p_v_enc_cfg->Configuration.VideoEncoding = p_req->Configuration.Encoding;

	if (VideoEncoding_MPEG4 == p_req->Configuration.Encoding)
	{
	    strcpy(p_v_enc_cfg->Configuration.Encoding, "MPV4-ES");
		p_v_enc_cfg->Configuration.GovLength = p_req->Configuration.MPEG4.GovLength;
		strcpy(p_v_enc_cfg->Configuration.Profile, onvif_Mpeg4ProfileToString(p_req->Configuration.MPEG4.Mpeg4Profile));
	}
	else if (VideoEncoding_H264 == p_req->Configuration.Encoding)
	{
	    strcpy(p_v_enc_cfg->Configuration.Encoding, "H264");
		p_v_enc_cfg->Configuration.GovLength = p_req->Configuration.H264.GovLength;
		strcpy(p_v_enc_cfg->Configuration.Profile, onvif_H264ProfileToString(p_req->Configuration.H264.H264Profile));
	}	
	else if (VideoEncoding_JPEG == p_req->Configuration.Encoding)
	{
	    strcpy(p_v_enc_cfg->Configuration.Encoding, "JPEG");
	}

	if (p_req->Configuration.RateControlFlag)
	{
		p_v_enc_cfg->Configuration.RateControl.FrameRateLimit = (float)p_req->Configuration.RateControl.FrameRateLimit;
		p_v_enc_cfg->Configuration.RateControl.EncodingInterval = p_req->Configuration.RateControl.EncodingInterval;
		p_v_enc_cfg->Configuration.RateControl.BitrateLimit = p_req->Configuration.RateControl.BitrateLimit;
	}

	memcpy(&p_v_enc_cfg->Configuration.Multicast, &p_req->Configuration.Multicast, sizeof(onvif_MulticastConfiguration));

	// todo : here add your handler code ...
	
	
	return ONVIF_OK;
}

/************************************************************************************
 *  	
 * Possible error:
 * 	ONVIF_ERR_ConfigModify
 *	ONVIF_ERR_NoConfig
 * 	ONVIF_ERR_ConfigurationConflict
 *
*************************************************************************************/
ONVIF_RET onvif_SetVideoSourceConfiguration(SetVideoSourceConfiguration_REQ * p_req)
{
	ONVIF_VideoSource * p_v_src;
	ONVIF_VideoSourceConfiguration * p_v_src_cfg = onvif_find_VideoSourceConfiguration(p_req->Configuration.token);
	if (NULL == p_v_src_cfg)
	{
		return ONVIF_ERR_NoConfig;
	}
	
	p_v_src = onvif_find_VideoSource(p_req->Configuration.SourceToken);
	if (NULL == p_v_src)
	{
		return ONVIF_ERR_NoConfig;
	}

	if (/*p_req->Configuration.Bounds.x < g_onvif_cfg.VideoSourceConfigurationOptions.BoundsRange.XRange.Min || */
		p_req->Configuration.Bounds.x > g_onvif_cfg.VideoSourceConfigurationOptions.BoundsRange.XRange.Max ||
		/*p_req->Configuration.Bounds.y < g_onvif_cfg.VideoSourceConfigurationOptions.BoundsRange.YRange.Min || */
		p_req->Configuration.Bounds.y > g_onvif_cfg.VideoSourceConfigurationOptions.BoundsRange.YRange.Max ||
		/*p_req->Configuration.Bounds.width < g_onvif_cfg.VideoSourceConfigurationOptions.BoundsRange.WidthRange.Min || */
		p_req->Configuration.Bounds.width > g_onvif_cfg.VideoSourceConfigurationOptions.BoundsRange.WidthRange.Max ||
		/*p_req->Configuration.Bounds.height < g_onvif_cfg.VideoSourceConfigurationOptions.BoundsRange.HeightRange.Min || */
		p_req->Configuration.Bounds.height > g_onvif_cfg.VideoSourceConfigurationOptions.BoundsRange.HeightRange.Max)
	{
		return ONVIF_ERR_ConfigModify;
	}

	p_v_src_cfg->Configuration.Bounds.x = p_req->Configuration.Bounds.x;
	p_v_src_cfg->Configuration.Bounds.y = p_req->Configuration.Bounds.y;
	p_v_src_cfg->Configuration.Bounds.width = p_req->Configuration.Bounds.width;
	p_v_src_cfg->Configuration.Bounds.height = p_req->Configuration.Bounds.height;

	strcpy(p_v_src_cfg->Configuration.Name, p_req->Configuration.Name);

	return ONVIF_OK;
}

ONVIF_RET onvif_GetStreamUri(const char * lip, uint32 lport, GetStreamUri_REQ * p_req, GetStreamUri_RES * p_res)
{
    ONVIF_PROFILE * p_profile = onvif_find_profile(p_req->ProfileToken);
    if (NULL == p_profile)
    {
        return ONVIF_ERR_NoProfile;
    }

    // set the media uri

	if (p_profile->stream_uri[0] == '\0')
	{
	    int offset = 0;
	    int len = sizeof(p_res->MediaUri.Uri);	    

	    if (p_req->StreamSetup.Transport.Protocol == TransportProtocol_HTTP)
	    {
	        offset += sprintf(p_res->MediaUri.Uri, "http://%s/live.sdp", lip);
	        // offset += sprintf(p_res->MediaUri.Uri, "http://%s/test.mp4", lip);
	    }
	    else
	    {
	        offset += sprintf(p_res->MediaUri.Uri, "rtsp://%s/live.sdp", lip);
	    }

	/*     if (StreamType_RTP_Unicast == p_req->StreamSetup.Stream)
	    {
	        offset += snprintf(p_res->MediaUri.Uri+offset, len-offset, "&amp;t=%s", "unicast");
	    }
	    else if (StreamType_RTP_Multicast == p_req->StreamSetup.Stream)
	    {
	        offset += snprintf(p_res->MediaUri.Uri+offset, len-offset, "&amp;t=%s", "multicase");
	    }

	    if (TransportProtocol_UDP == p_req->StreamSetup.Transport.Protocol)
	    {
	        offset += snprintf(p_res->MediaUri.Uri+offset, len-offset, "&amp;p=%s", "udp");
	    }
	    else if (TransportProtocol_TCP == p_req->StreamSetup.Transport.Protocol)
	    {
	        offset += snprintf(p_res->MediaUri.Uri+offset, len-offset, "&amp;p=%s", "tcp");
	    }
	    else if (TransportProtocol_RTSP == p_req->StreamSetup.Transport.Protocol)
	    {
	        offset += snprintf(p_res->MediaUri.Uri+offset, len-offset, "&amp;p=%s", "rtsp");
	    }
	    else if (TransportProtocol_HTTP == p_req->StreamSetup.Transport.Protocol)
	    {
	        offset += snprintf(p_res->MediaUri.Uri+offset, len-offset, "&amp;p=%s", "http");
	    }
 
        if (p_profile->v_enc_cfg)
        {            
	        offset += snprintf(p_res->MediaUri.Uri+offset, len-offset, "&amp;ve=%s&amp;w=%d&amp;h=%d", 
	            p_profile->v_enc_cfg->Configuration.Encoding,
	            p_profile->v_enc_cfg->Configuration.Resolution.Width,
	            p_profile->v_enc_cfg->Configuration.Resolution.Height);
	        
	    }
	

#ifdef AUDIO_SUPPORT
	    if (p_profile->a_enc_cfg)
        {            
	        offset += snprintf(p_res->MediaUri.Uri+offset, len-offset, "&amp;ae=%s&amp;sr=%d", 
	            p_profile->a_enc_cfg->Configuration.Encoding,
	            p_profile->a_enc_cfg->Configuration.SampleRate * 1000);
	        
	    }
#endif	    
		*/
	}
	else
	{
	    strcpy(p_res->MediaUri.Uri, p_profile->stream_uri);
	}

    p_res->MediaUri.InvalidAfterConnect = FALSE;
    p_res->MediaUri.InvalidAfterReboot = FALSE;
    p_res->MediaUri.Timeout = 60;

    return ONVIF_OK;
}

/**
 *
 * Get snapshot JPEG image data, 
 * rlen [in, out], [in] the buff size, [out] the image data size
 *
**/
ONVIF_RET onvif_GetSnapshot(char *buff, int * rlen, char * profile_token)
{
    int len = 0;
    FILE * fp = NULL;
    ONVIF_PROFILE * p_profile;
	char *p_bufs = NULL;	
	int tlen = 0;
	char * acFile = "/user/snapshot.jpg";
    
    onvif_print("onvif_GetSnapshot\r\n");
    p_profile = onvif_find_profile(profile_token);
    if (NULL == p_profile || NULL == p_profile->v_src_cfg)
    {
        return ONVIF_ERR_NoProfile;
    }

	if (NULL == buff) return ONVIF_ERR_ServiceNotSupported;
	
    p_bufs = buff;
    // here is the test code, just read the image data from file ...
	if (0 == Common_Venc_SnapProcess(acFile))
	{
	    fp = fopen(acFile, "rb");
		if (NULL == fp)
		{
			return ONVIF_ERR_ServiceNotSupported;
		}
		
		fseek(fp, 0, SEEK_END);
		len = ftell(fp);
		if (len <= 0)
		{
			fclose(fp);
			return ONVIF_ERR_ServiceNotSupported;
		}
		fseek(fp, 0, SEEK_SET);
		
		if (len > *rlen)
		{
		    fclose(fp);
			return ONVIF_ERR_ServiceNotSupported;
		}
		
		tlen = sprintf(p_bufs,	"HTTP/1.1 200 OK\r\n"
								"Server: hsoap/2.8\r\n"
								"Access-Control-Allow-Origin: *\r\n"
								"Access-Control-Allow-Methods: GET, POST, PUT, OPTIONS\r\n"
								"Access-Control-Allow-Headers: Content-Type, Authorization, X-Custom-Header\r\n"
								"Content-Type: image/jpeg\r\n"
								"Content-Length: %d\r\n"
								"Connection: close\r\n\r\n",
								len);

		tlen += fread(p_bufs+tlen, 1, len, fp);
		fclose(fp);
	}
	else {
		*rlen = 0;
		return ONVIF_ERR_ServiceNotSupported;
	}

	*rlen = tlen;
    
    return ONVIF_OK;
}

ONVIF_RET onvif_SetOSD(SetOSD_REQ * p_req)
{
	ONVIF_OSDConfiguration * p_osd = onvif_find_OSDConfiguration(p_req->OSD.token);
	if (NULL == p_osd)
	{
		return ONVIF_ERR_NoConfig;
	}

	// todo : here add your handler code ...


	memcpy(&p_osd->OSD, &p_req->OSD, sizeof(onvif_OSDConfiguration));
	
	return ONVIF_OK;
}

ONVIF_RET onvif_CreateOSD(CreateOSD_REQ * p_req)
{
    ONVIF_OSDConfiguration * p_osd = onvif_add_OSDConfiguration();
	if (NULL == p_osd)
	{
		return ONVIF_ERR_MaxOSDs;
	}

	memcpy(&p_osd->OSD, &p_req->OSD, sizeof(onvif_OSDConfiguration));

	snprintf(p_osd->OSD.token, ONVIF_TOKEN_LEN, "OSD_00%d", g_onvif_cls.osd_idx++);

    // return the token
    strcpy(p_req->OSD.token, p_osd->OSD.token);

    // todo : here add your handler code ... 
    
	
	return ONVIF_OK;
}

ONVIF_RET onvif_DeleteOSD(DeleteOSD_REQ * p_req)
{
    ONVIF_OSDConfiguration * p_prev;
	ONVIF_OSDConfiguration * p_osd = onvif_find_OSDConfiguration(p_req->OSDToken);
	if (NULL == p_osd)
	{
		return ONVIF_ERR_NoConfig;
	}

	// todo : here add your handler code ...


	p_prev = g_onvif_cfg.OSDs;
	if (p_osd == p_prev)
	{
		g_onvif_cfg.OSDs = p_osd->next;
	}
	else
	{
		while (p_prev->next)
		{
			if (p_prev->next == p_osd)
			{
				break;
			}

			p_prev = p_prev->next;
		}

		p_prev->next = p_osd->next;
	}

	free(p_osd);
	
	return ONVIF_OK;
}

ONVIF_RET onvif_StartMulticastStreaming(const char * token)
{
	ONVIF_PROFILE * p_profile = onvif_find_profile(token);
	if (NULL == p_profile)
	{
		return ONVIF_ERR_NoProfile;
	}

	// todo : start multicast streaming ...

	p_profile->multicasting = TRUE;

	return ONVIF_OK;
}

ONVIF_RET onvif_StopMulticastStreaming(const char * token)
{
	ONVIF_PROFILE * p_profile = onvif_find_profile(token);
	if (NULL == p_profile)
	{
		return ONVIF_ERR_NoProfile;
	}

	// todo : stop multicast streaming ...

	p_profile->multicasting = FALSE;
	
	return ONVIF_OK;
}

ONVIF_RET onvif_SetMetadataConfiguration(SetMetadataConfiguration_REQ * p_req)
{
	ONVIF_MetadataConfiguration * p_cfg = onvif_find_MetadataConfiguration(p_req->Configuration.token);
	if (NULL == p_cfg)
	{
		return ONVIF_ERR_NoConfig;
	}

	if (p_req->Configuration.SessionTimeout <= 0)
	{
		return ONVIF_ERR_ConfigModify;
	}

    strcpy(p_cfg->Configuration.Name, p_req->Configuration.Name);
    p_cfg->Configuration.SessionTimeout = p_req->Configuration.SessionTimeout;

    p_cfg->Configuration.AnalyticsFlag = p_req->Configuration.AnalyticsFlag;
    p_cfg->Configuration.Analytics = p_req->Configuration.Analytics;

    p_cfg->Configuration.PTZStatusFlag = p_req->Configuration.PTZStatusFlag;
    memcpy(&p_cfg->Configuration.PTZStatus, &p_req->Configuration.PTZStatus, sizeof(onvif_PTZFilter));

    p_cfg->Configuration.EventsFlag = p_req->Configuration.EventsFlag;
    memcpy(&p_cfg->Configuration.Events, &p_req->Configuration.Events, sizeof(onvif_EventSubscription));

    memcpy(&p_cfg->Configuration.Multicast, &p_req->Configuration.Multicast, sizeof(onvif_MulticastConfiguration));
	
	return ONVIF_OK;
}

ONVIF_RET onvif_AddMetadataConfiguration(AddMetadataConfiguration_REQ * p_req)
{
	ONVIF_MetadataConfiguration * p_metadata_cfg;
	ONVIF_PROFILE * p_profile = onvif_find_profile(p_req->ProfileToken);
	if (NULL == p_profile)
	{
		return ONVIF_ERR_NoProfile;
	}

	p_metadata_cfg = onvif_find_MetadataConfiguration(p_req->ConfigurationToken);
	if (NULL == p_metadata_cfg)
	{
		return ONVIF_ERR_NoConfig;
	}

    if (p_profile->metadata_cfg != p_metadata_cfg)
	{
		if (p_profile->metadata_cfg && p_profile->metadata_cfg->Configuration.UseCount > 0)
		{
			p_profile->metadata_cfg->Configuration.UseCount--;
		}
		
		p_metadata_cfg->Configuration.UseCount++;
		p_profile->metadata_cfg = p_metadata_cfg;
	}

	return ONVIF_OK;
}

ONVIF_RET onvif_RemoveMetadataConfiguration(const char * profile_token)
{
	ONVIF_PROFILE * p_profile = onvif_find_profile(profile_token);
	if (NULL == p_profile)
	{
		return ONVIF_ERR_NoProfile;
	}

    if (p_profile->metadata_cfg && p_profile->metadata_cfg->Configuration.UseCount > 0)
	{
		p_profile->metadata_cfg->Configuration.UseCount--;
	}
	
	p_profile->metadata_cfg = NULL;

	return ONVIF_OK;
}

ONVIF_RET onvif_SetVideoSourceMode(SetVideoSourceMode_REQ * p_req, SetVideoSourceMode_RES * p_res)
{
    ONVIF_VideoSource * p_v_src = onvif_find_VideoSource(p_req->VideoSourceToken);
    if (NULL == p_v_src)
    {
        return ONVIF_ERR_NoVideoSource;
    }

    if (strcmp(p_v_src->VideoSourceMode.token, p_req->VideoSourceModeToken))
    {
        return ONVIF_ERR_NoVideoSourceMode;
    }

    // todo : handler set video source mode ...
    

    return ONVIF_OK;
}

ONVIF_RET onvif_GetSnapshotUri(const char * lip, uint32 lport, GetSnapshotUri_REQ * p_req, GetSnapshotUri_RES * p_res)
{
    ONVIF_PROFILE * p_profile = onvif_find_profile(p_req->ProfileToken);
    if (NULL == p_profile)
    {
        return ONVIF_ERR_InvalidToken;
    }

    // set the media uri
    
    sprintf(p_res->MediaUri.Uri, "%s://%s:%d/snapshot/%s", 
        g_onvif_cfg.https_enable ? "https" : "http", lip, lport, p_profile->token);

    p_res->MediaUri.InvalidAfterConnect = FALSE;
    p_res->MediaUri.InvalidAfterReboot = FALSE;
    p_res->MediaUri.Timeout = 60;

    return ONVIF_OK;
}

ONVIF_RET onvif_SetSynchronizationPoint(SetSynchronizationPoint_REQ * p_req)
{
    // todo : add handler code ...
    
    return ONVIF_OK;
}


#ifdef AUDIO_SUPPORT

ONVIF_RET onvif_AddAudioSourceConfiguration(AddAudioSourceConfiguration_REQ * p_req)
{
    ONVIF_AudioSourceConfiguration * p_a_src_cfg;
	ONVIF_PROFILE * p_profile = onvif_find_profile(p_req->ProfileToken);
	if (NULL == p_profile)
	{
		return ONVIF_ERR_NoProfile;
	}

	p_a_src_cfg = onvif_find_AudioSourceConfiguration(p_req->ConfigurationToken);
	if (NULL == p_a_src_cfg)
	{
		return ONVIF_ERR_NoConfig;
	}

	if (p_profile->a_src_cfg != p_a_src_cfg)
	{
		if (p_profile->a_src_cfg && p_profile->a_src_cfg->Configuration.UseCount > 0)
		{
			p_profile->a_src_cfg->Configuration.UseCount--;
		}
		
		p_a_src_cfg->Configuration.UseCount++;
		p_profile->a_src_cfg = p_a_src_cfg;
	}
	
	return ONVIF_OK;
}

ONVIF_RET onvif_AddAudioEncoderConfiguration(AddAudioEncoderConfiguration_REQ * p_req)
{
    ONVIF_AudioEncoder2Configuration * p_a_enc_cfg;
	ONVIF_PROFILE * p_profile = onvif_find_profile(p_req->ProfileToken);
	if (NULL == p_profile)
	{
		return ONVIF_ERR_NoProfile;
	}

	p_a_enc_cfg = onvif_find_AudioEncoderConfiguration(p_req->ConfigurationToken);
	if (NULL == p_a_enc_cfg)
	{
		return ONVIF_ERR_NoConfig;
	}

	if (p_profile->a_enc_cfg != p_a_enc_cfg)
	{
		if (p_profile->a_enc_cfg && p_profile->a_enc_cfg->Configuration.UseCount > 0)
		{
			p_profile->a_enc_cfg->Configuration.UseCount--;
		}
		
		p_a_enc_cfg->Configuration.UseCount++;
		p_profile->a_enc_cfg = p_a_enc_cfg;
	}
	
	return ONVIF_OK;
}

ONVIF_RET onvif_RemoveAudioEncoderConfiguration(const char * token)
{
	ONVIF_PROFILE * p_profile = onvif_find_profile(token);
	if (NULL == p_profile)
	{
		return ONVIF_ERR_NoProfile;
	}

	if (p_profile->a_enc_cfg && p_profile->a_enc_cfg->Configuration.UseCount > 0)
	{
		p_profile->a_enc_cfg->Configuration.UseCount--;
	}
	
	p_profile->a_enc_cfg = NULL;

	return ONVIF_OK;
}

ONVIF_RET onvif_RemoveAudioSourceConfiguration(const char * token)
{
	ONVIF_PROFILE * p_profile = onvif_find_profile(token);
	if (NULL == p_profile)
	{
		return ONVIF_ERR_NoProfile;
	}

	if (p_profile->a_src_cfg && p_profile->a_src_cfg->Configuration.UseCount > 0)
	{
		p_profile->a_src_cfg->Configuration.UseCount--;
	}
	
	p_profile->a_src_cfg= NULL;

	return ONVIF_OK;
}

ONVIF_RET onvif_SetAudioSourceConfiguration(SetAudioSourceConfiguration_REQ * p_req)
{
	ONVIF_AudioSource * p_a_src;
	ONVIF_AudioSourceConfiguration * p_a_src_cfg = onvif_find_AudioSourceConfiguration(p_req->Configuration.token);
	if (NULL == p_a_src_cfg)
	{
		return ONVIF_ERR_NoConfig;
	}
	
	p_a_src = onvif_find_AudioSource(p_req->Configuration.SourceToken);
	if (NULL == p_a_src)
	{
		return ONVIF_ERR_NoConfig;
	}

	strcpy(p_a_src_cfg->Configuration.Name, p_req->Configuration.Name);

	return ONVIF_OK;
}

ONVIF_RET onvif_SetAudioEncoderConfiguration(SetAudioEncoderConfiguration_REQ * p_req)
{
	ONVIF_AudioEncoder2Configuration * p_a_enc_cfg = onvif_find_AudioEncoderConfiguration(p_req->Configuration.token);
	if (NULL == p_a_enc_cfg)
	{
		return ONVIF_ERR_NoConfig;
	}

	if (p_req->Configuration.SampleRate != 8  && 
		p_req->Configuration.SampleRate != 12 && 
		p_req->Configuration.SampleRate != 25 && 
		p_req->Configuration.SampleRate != 32 &&
		p_req->Configuration.SampleRate != 48)
	{
		return ONVIF_ERR_ConfigModify;
	}

	p_a_enc_cfg->Configuration.SessionTimeout = p_req->Configuration.SessionTimeout;
	p_a_enc_cfg->Configuration.Bitrate = p_req->Configuration.Bitrate;
	p_a_enc_cfg->Configuration.SampleRate = p_req->Configuration.SampleRate;
	p_a_enc_cfg->Configuration.AudioEncoding = p_req->Configuration.Encoding;

    if (AudioEncoding_G711 == p_req->Configuration.Encoding)
    {
        strcpy(p_a_enc_cfg->Configuration.Encoding, "PCMU");
    }
    else if (AudioEncoding_G726 == p_req->Configuration.Encoding)
    {
        strcpy(p_a_enc_cfg->Configuration.Encoding, "G726");
    }
    else if (AudioEncoding_AAC == p_req->Configuration.Encoding)
    {
        strcpy(p_a_enc_cfg->Configuration.Encoding, "MP4A-LATM");
    }

	memcpy(&p_a_enc_cfg->Configuration.Multicast, &p_req->Configuration.Multicast, sizeof(onvif_MulticastConfiguration));

	// todo : add set audio encoder code ...

	return ONVIF_OK;
}

ONVIF_RET onvif_AddAudioDecoderConfiguration(AddAudioDecoderConfiguration_REQ * p_req)
{
    ONVIF_AudioDecoderConfiguration * p_a_dec_cfg;
	ONVIF_PROFILE * p_profile = onvif_find_profile(p_req->ProfileToken);
	if (NULL == p_profile)
	{
		return ONVIF_ERR_NoProfile;
	}

	p_a_dec_cfg = onvif_find_AudioDecoderConfiguration(p_req->ConfigurationToken);
	if (NULL == p_a_dec_cfg)
	{
		return ONVIF_ERR_NoConfig;
	}

	if (p_profile->a_dec_cfg != p_a_dec_cfg)
	{
		if (p_profile->a_dec_cfg && p_profile->a_dec_cfg->Configuration.UseCount > 0)
		{
			p_profile->a_dec_cfg->Configuration.UseCount--;
		}
		
		p_a_dec_cfg->Configuration.UseCount++;
		p_profile->a_dec_cfg = p_a_dec_cfg;
	}
	
	return ONVIF_OK;
}

ONVIF_RET onvif_RemoveAudioDecoderConfiguration(RemoveAudioDecoderConfiguration_REQ * p_req)
{
    ONVIF_PROFILE * p_profile = onvif_find_profile(p_req->ProfileToken);
	if (NULL == p_profile)
	{
		return ONVIF_ERR_NoProfile;
	}

	if (p_profile->a_dec_cfg && p_profile->a_dec_cfg->Configuration.UseCount > 0)
	{
		p_profile->a_dec_cfg->Configuration.UseCount--;
	}
	
	p_profile->a_dec_cfg = NULL;

	return ONVIF_OK;
}

ONVIF_RET onvif_SetAudioDecoderConfiguration(SetAudioDecoderConfiguration_REQ * p_req)
{
    ONVIF_AudioDecoderConfiguration * p_a_dec_cfg = onvif_find_AudioDecoderConfiguration(p_req->Configuration.token);
	if (NULL == p_a_dec_cfg)
	{
		return ONVIF_ERR_NoConfig;
	}

	// todo : add set audio decoder code ...

    strcpy(p_a_dec_cfg->Configuration.Name, p_req->Configuration.Name);
    
	return ONVIF_OK;
}

#endif // end of AUDIO_SUPPORT

#ifdef PTZ_SUPPORT

ONVIF_RET onvif_AddPTZConfiguration(AddPTZConfiguration_REQ * p_req)
{
    ONVIF_PTZConfiguration * p_ptz_cfg;
	ONVIF_PROFILE * p_profile = onvif_find_profile(p_req->ProfileToken);
	if (NULL == p_profile)
	{
		return ONVIF_ERR_NoProfile;
	}

	p_ptz_cfg = onvif_find_PTZConfiguration(p_req->ConfigurationToken);
	if (NULL == p_ptz_cfg)
	{
		return ONVIF_ERR_NoConfig;
	}

    if (p_profile->ptz_cfg != p_ptz_cfg)
	{
		if (p_profile->ptz_cfg && p_profile->ptz_cfg->Configuration.UseCount > 0)
		{
			p_profile->ptz_cfg->Configuration.UseCount--;
		}
		
		p_ptz_cfg->Configuration.UseCount++;
		p_profile->ptz_cfg = p_ptz_cfg;
	}

	return ONVIF_OK;
}

ONVIF_RET onvif_RemovePTZConfiguration(const char * token)
{
	ONVIF_PROFILE * p_profile = onvif_find_profile(token);
	if (NULL == p_profile)
	{
		return ONVIF_ERR_NoProfile;
	}

    if (p_profile->ptz_cfg && p_profile->ptz_cfg->Configuration.UseCount > 0)
	{
		p_profile->ptz_cfg->Configuration.UseCount--;
	}
	
	p_profile->ptz_cfg = NULL;

	return ONVIF_OK;
}

#endif // PTZ_SUPPORT

#ifdef DEVICEIO_SUPPORT

ONVIF_RET onvif_AddAudioOutputConfiguration(AddAudioOutputConfiguration_REQ * p_req)
{
    ONVIF_AudioOutputConfiguration * p_a_output_cfg;
	ONVIF_PROFILE * p_profile = onvif_find_profile(p_req->ProfileToken);
	if (NULL == p_profile)
	{
		return ONVIF_ERR_NoProfile;
	}

	p_a_output_cfg = onvif_find_AudioOutputConfiguration(p_req->ConfigurationToken);
	if (NULL == p_a_output_cfg)
	{
		return ONVIF_ERR_NoConfig;
	}

	if (p_profile->a_output_cfg != p_a_output_cfg)
	{
		if (p_profile->a_output_cfg && p_profile->a_output_cfg->Configuration.UseCount > 0)
		{
			p_profile->a_output_cfg->Configuration.UseCount--;
		}
		
		p_a_output_cfg->Configuration.UseCount++;
		p_profile->a_output_cfg = p_a_output_cfg;
	}
	
	return ONVIF_OK;
}

ONVIF_RET onvif_RemoveAudioOutputConfiguration(RemoveAudioOutputConfiguration_REQ * p_req)
{
    ONVIF_PROFILE * p_profile = onvif_find_profile(p_req->ProfileToken);
	if (NULL == p_profile)
	{
		return ONVIF_ERR_NoProfile;
	}

	if (p_profile->a_output_cfg && p_profile->a_output_cfg->Configuration.UseCount > 0)
	{
		p_profile->a_output_cfg->Configuration.UseCount--;
	}
	
	p_profile->a_output_cfg = NULL;

	return ONVIF_OK;
}

#endif // end of DEVICEIO_SUPPORT





