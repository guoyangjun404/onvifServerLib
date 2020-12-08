
#include "sys_inc.h"
#include "onvif_media2.h"
#include "onvif_media.h"
#include "onvif_event.h"


#ifdef MEDIA2_SUPPORT

/******************************************************************/
extern ONVIF_CLS g_onvif_cls;
extern ONVIF_CFG g_onvif_cfg;

/******************************************************************/

void onvif_MediaProfileChangedNotify(ONVIF_PROFILE * p_profile)
{
    ONVIF_NotificationMessage * p_message = onvif_add_NotificationMessage(NULL);
	if (p_message)
	{
	    ONVIF_SimpleItem * p_simpleitem;
	    
		strcpy(p_message->NotificationMessage.Dialect, "http://www.onvif.org/ver10/tev/topicExpression/ConcreteSet");
		strcpy(p_message->NotificationMessage.Topic, "tns1:Media/ProfileChanged");
		p_message->NotificationMessage.Message.PropertyOperationFlag = 1;
		p_message->NotificationMessage.Message.PropertyOperation = PropertyOperation_Changed;
		p_message->NotificationMessage.Message.UtcTime = time(NULL)+1;

        p_message->NotificationMessage.Message.SourceFlag = 1;
        
		p_simpleitem = onvif_add_SimpleItem(&p_message->NotificationMessage.Message.Source.SimpleItem);
		if (p_simpleitem)
		{		    
			strcpy(p_simpleitem->SimpleItem.Name, "Token");
			strcpy(p_simpleitem->SimpleItem.Value, p_profile->token);
		}

		onvif_put_NotificationMessage(p_message);
	}
}

void onvif_MediaConfigurationChangedNotify(const char * token, const char * type)
{
    ONVIF_NotificationMessage * p_message = onvif_add_NotificationMessage(NULL);
	if (p_message)
	{
	    ONVIF_SimpleItem * p_simpleitem;
	    
		strcpy(p_message->NotificationMessage.Dialect, "http://www.onvif.org/ver10/tev/topicExpression/ConcreteSet");
		strcpy(p_message->NotificationMessage.Topic, "tns1:Media/ConfigurationChanged");
		p_message->NotificationMessage.Message.PropertyOperationFlag = 1;
		p_message->NotificationMessage.Message.PropertyOperation = PropertyOperation_Changed;
		p_message->NotificationMessage.Message.UtcTime = time(NULL)+1;

        p_message->NotificationMessage.Message.SourceFlag = 1;
        
		p_simpleitem = onvif_add_SimpleItem(&p_message->NotificationMessage.Message.Source.SimpleItem);
		if (p_simpleitem)
		{		    
			strcpy(p_simpleitem->SimpleItem.Name, "Token");
			strcpy(p_simpleitem->SimpleItem.Value, token);
		}

		p_simpleitem = onvif_add_SimpleItem(&p_message->NotificationMessage.Message.Source.SimpleItem);
		if (p_simpleitem)
		{		    
			strcpy(p_simpleitem->SimpleItem.Name, "Type");
			strcpy(p_simpleitem->SimpleItem.Value, type);
		}

		onvif_put_NotificationMessage(p_message);
	}
}


ONVIF_RET onvif_tr2_SetVideoEncoderConfiguration(tr2_SetVideoEncoderConfiguration_REQ * p_req)
{
	int i = 0;
	ONVIF_VideoEncoder2Configuration * p_v_enc_cfg;
    ONVIF_VideoEncoder2ConfigurationOptions * p_option;
    
	p_v_enc_cfg = onvif_find_VideoEncoderConfiguration(p_req->Configuration.token);
	if (NULL == p_v_enc_cfg)
	{
		return ONVIF_ERR_NoConfig;
	}

    p_option = onvif_find_VideoEncoder2ConfigurationOptions(p_req->Configuration.Encoding);
    if (p_option)
    {
    	if (p_req->Configuration.Quality < p_option->Options.QualityRange.Min || 
    		p_req->Configuration.Quality > p_option->Options.QualityRange.Max )
    	{
    		return ONVIF_ERR_ConfigModify;
    	}

    	if (p_req->Configuration.GovLengthFlag)
    	{
    		// todo : check the govlength ...
    	}

    	for (i = 0; i < ARRAY_SIZE(p_option->Options.ResolutionsAvailable); i++)
    	{
    		if (p_option->Options.ResolutionsAvailable[i].Width == p_req->Configuration.Resolution.Width && 
    			p_option->Options.ResolutionsAvailable[i].Height == p_req->Configuration.Resolution.Height)
    		{
    			break;
    		}
    	}

    	if (i == ARRAY_SIZE(p_option->Options.ResolutionsAvailable))
    	{
    		return ONVIF_ERR_ConfigModify;
    	}
	}

	p_v_enc_cfg->Configuration.Resolution.Width = p_req->Configuration.Resolution.Width;
	p_v_enc_cfg->Configuration.Resolution.Height = p_req->Configuration.Resolution.Height;
	p_v_enc_cfg->Configuration.Quality = p_req->Configuration.Quality;
	strcpy(p_v_enc_cfg->Configuration.Encoding, p_req->Configuration.Encoding);
	strcpy(p_v_enc_cfg->Configuration.Name, p_req->Configuration.Name);

    if (strcasecmp(p_req->Configuration.Encoding, "JPEG"))
    {
        p_req->Configuration.VideoEncoding = VideoEncoding_JPEG;
    }
    else if (strcasecmp(p_req->Configuration.Encoding, "H264"))
    {
        p_req->Configuration.VideoEncoding = VideoEncoding_H264;
    }
    else if (strcasecmp(p_req->Configuration.Encoding, "H265"))
    {
        p_req->Configuration.VideoEncoding = VideoEncoding_H265;
    }
    else if (strcasecmp(p_req->Configuration.Encoding, "MPV4-ES"))
    {
        p_req->Configuration.VideoEncoding = VideoEncoding_MPEG4;
    }
    
    if (p_req->Configuration.GovLengthFlag)
    {
	    p_v_enc_cfg->Configuration.GovLength = p_req->Configuration.GovLength;
	}

	if (strcasecmp(p_req->Configuration.Encoding, "JPEG") == 0)
	{
	    p_v_enc_cfg->Configuration.GovLengthFlag = 0;
	}
	else
	{
	    p_v_enc_cfg->Configuration.GovLengthFlag = 1;
	}	
	
	if (p_req->Configuration.ProfileFlag)
	{
	    strcpy(p_v_enc_cfg->Configuration.Profile, p_req->Configuration.Profile);
    }
    
	if (p_req->Configuration.RateControlFlag)
	{
		p_v_enc_cfg->Configuration.RateControl.FrameRateLimit = p_req->Configuration.RateControl.FrameRateLimit;		
		p_v_enc_cfg->Configuration.RateControl.BitrateLimit = p_req->Configuration.RateControl.BitrateLimit;

        if (p_req->Configuration.RateControl.ConstantBitRateFlag)
        {
		    p_v_enc_cfg->Configuration.RateControl.ConstantBitRate = p_req->Configuration.RateControl.ConstantBitRate;
		}
	}

	if (p_req->Configuration.MulticastFlag)
	{
		memcpy(&p_v_enc_cfg->Configuration.Multicast, &p_req->Configuration.Multicast, sizeof(onvif_MulticastConfiguration));
	}

    onvif_MediaConfigurationChangedNotify(p_req->Configuration.token, "VideoEncoder");
	
	// todo : here add your handler code ...
	
	
	return ONVIF_OK;
}

ONVIF_RET onvif_tr2_CreateProfile(const char * lip, uint32 lport, tr2_CreateProfile_REQ * p_req, tr2_CreateProfile_RES * p_res)
{
	int i;
	ONVIF_PROFILE * p_profile = NULL;
	
	p_profile = onvif_add_profile(FALSE);
	if (p_profile)
	{
		strcpy(p_profile->name, p_req->Name);		
		strcpy(p_res->Token, p_profile->token);

		// add configuration to the new profile
		for (i = 0; i < p_req->sizeConfiguration; i++)
		{                                         
			if (strcasecmp(p_req->Configuration[i].Type, "all") == 0)
			{
			    ONVIF_PROFILE * p_refer = NULL;
    			if (p_req->Configuration[i].TokenFlag)
    			{
    				p_refer = onvif_find_profile(p_req->Configuration[i].Token);
    			}
    			else
    			{
    				p_refer = g_onvif_cfg.profiles;
    			}

    			if (NULL == p_refer)
    			{
    				continue;
    			}
			
				p_profile->v_src_cfg = p_refer->v_src_cfg;
				if (p_profile->v_src_cfg)
				{
					p_profile->v_src_cfg->Configuration.UseCount++;
				}
				
				p_profile->v_enc_cfg = p_refer->v_enc_cfg;
				if (p_profile->v_enc_cfg)
				{
					p_profile->v_enc_cfg->Configuration.UseCount++;
				}	
				
				p_profile->metadata_cfg = p_refer->metadata_cfg;
				if (p_profile->metadata_cfg)
				{
					p_profile->metadata_cfg->Configuration.UseCount++;
				}	
				
#ifdef AUDIO_SUPPORT				
				p_profile->a_src_cfg = p_refer->a_src_cfg;
				if (p_profile->a_src_cfg)
				{
					p_profile->a_src_cfg->Configuration.UseCount++;
				}
				
				p_profile->a_enc_cfg = p_refer->a_enc_cfg;
				if (p_profile->a_enc_cfg)
				{
					p_profile->a_enc_cfg->Configuration.UseCount++;
				}

				p_profile->a_dec_cfg = p_refer->a_dec_cfg;
				if (p_profile->a_dec_cfg)
				{
					p_profile->a_dec_cfg->Configuration.UseCount++;
				}
#endif		

#ifdef DEVICEIO_SUPPORT
				p_profile->a_output_cfg = p_refer->a_output_cfg;
				if (p_profile->a_output_cfg)
				{
					p_profile->a_output_cfg->Configuration.UseCount++;
				}
#endif

#ifdef VIDEO_ANALYTICS
				p_profile->va_cfg = p_refer->va_cfg;
				if (p_profile->va_cfg)
				{
					p_profile->va_cfg->Configuration.UseCount++;
				}
#endif

#ifdef PTZ_SUPPORT
				p_profile->ptz_cfg = p_refer->ptz_cfg;
				if (p_profile->ptz_cfg)
				{
					p_profile->ptz_cfg->Configuration.UseCount++;
				}
#endif
			}
			else if (strcasecmp(p_req->Configuration[i].Type, "VideoSource") == 0)
			{
			    ONVIF_VideoSourceConfiguration * p_refer = NULL;
    			if (p_req->Configuration[i].TokenFlag)
    			{
    				p_refer = onvif_find_VideoSourceConfiguration(p_req->Configuration[i].Token);
    			}
    			else
    			{
    				p_refer = g_onvif_cfg.v_src_cfg;
    			}

    			if (NULL == p_refer)
    			{
    				continue;
    			}
    			
				p_profile->v_src_cfg = p_refer;
				if (p_profile->v_src_cfg)
				{
					p_profile->v_src_cfg->Configuration.UseCount++;
				}
			}
			else if (strcasecmp(p_req->Configuration[i].Type, "VideoEncoder") == 0)
			{
			    ONVIF_VideoEncoder2Configuration * p_refer = NULL;
    			if (p_req->Configuration[i].TokenFlag)
    			{
    				p_refer = onvif_find_VideoEncoderConfiguration(p_req->Configuration[i].Token);
    			}
    			else
    			{
    				p_refer = g_onvif_cfg.v_enc_cfg;
    			}

    			if (NULL == p_refer)
    			{
    				continue;
    			}
    			
				p_profile->v_enc_cfg = p_refer;
				if (p_profile->v_enc_cfg)
				{
					p_profile->v_enc_cfg->Configuration.UseCount++;
				}
			}
			else if (strcasecmp(p_req->Configuration[i].Type, "AudioSource") == 0)
			{
#ifdef AUDIO_SUPPORT
                ONVIF_AudioSourceConfiguration * p_refer = NULL;
    			if (p_req->Configuration[i].TokenFlag)
    			{
    				p_refer = onvif_find_AudioSourceConfiguration(p_req->Configuration[i].Token);
    			}
    			else
    			{
    				p_refer = g_onvif_cfg.a_src_cfg;
    			}

    			if (NULL == p_refer)
    			{
    				continue;
    			}
    			
				p_profile->a_src_cfg = p_refer;
				if (p_profile->a_src_cfg)
				{
					p_profile->a_src_cfg->Configuration.UseCount++;
				}
#endif
			}
			else if (strcasecmp(p_req->Configuration[i].Type, "AudioEncoder") == 0)
			{
#ifdef AUDIO_SUPPORT
                ONVIF_AudioEncoder2Configuration * p_refer = NULL;
    			if (p_req->Configuration[i].TokenFlag)
    			{
    				p_refer = onvif_find_AudioEncoderConfiguration(p_req->Configuration[i].Token);
    			}
    			else
    			{
    				p_refer = g_onvif_cfg.a_enc_cfg;
    			}

    			if (NULL == p_refer)
    			{
    				continue;
    			}
    			
				p_profile->a_enc_cfg = p_refer;
				if (p_profile->a_enc_cfg)
				{
					p_profile->a_enc_cfg->Configuration.UseCount++;
				}
#endif			
			}
			else if (strcasecmp(p_req->Configuration[i].Type, "AudioOutput") == 0)
			{
#ifdef DEVICEIO_SUPPORT
                ONVIF_AudioOutputConfiguration * p_refer = NULL;
    			if (p_req->Configuration[i].TokenFlag)
    			{
    				p_refer = onvif_find_AudioOutputConfiguration(p_req->Configuration[i].Token);
    			}
    			else
    			{
    				p_refer = g_onvif_cfg.a_output_cfg;
    			}

    			if (NULL == p_refer)
    			{
    				continue;
    			}
    			
				p_profile->a_output_cfg = p_refer;
				if (p_profile->a_output_cfg)
				{
					p_profile->a_output_cfg->Configuration.UseCount++;
				}
#endif			
			}
			else if (strcasecmp(p_req->Configuration[i].Type, "AudioDecoder") == 0)
			{
#ifdef AUDIO_SUPPORT		
                ONVIF_AudioDecoderConfiguration * p_refer = NULL;
    			if (p_req->Configuration[i].TokenFlag)
    			{
    				p_refer = onvif_find_AudioDecoderConfiguration(p_req->Configuration[i].Token);
    			}
    			else
    			{
    				p_refer = g_onvif_cfg.a_dec_cfg;
    			}

    			if (NULL == p_refer)
    			{
    				continue;
    			}
    			
    			p_profile->a_dec_cfg = p_refer;
    			if (p_profile->a_dec_cfg)
    			{
    				p_profile->a_dec_cfg->Configuration.UseCount++;
    			}
#endif	
			}
			else if (strcasecmp(p_req->Configuration[i].Type, "Metadata") == 0)
			{
			    ONVIF_MetadataConfiguration * p_refer = NULL;
    			if (p_req->Configuration[i].TokenFlag)
    			{
    				p_refer = onvif_find_MetadataConfiguration(p_req->Configuration[i].Token);
    			}
    			else
    			{
    				p_refer = g_onvif_cfg.metadata_cfg;
    			}

    			if (NULL == p_refer)
    			{
    				continue;
    			}
    			
				p_profile->metadata_cfg = p_refer;
				if (p_profile->metadata_cfg)
				{
					p_profile->metadata_cfg->Configuration.UseCount++;
				}
			}
			else if (strcasecmp(p_req->Configuration[i].Type, "Analytics") == 0)
			{
#ifdef VIDEO_ANALYTICS
                ONVIF_VideoAnalyticsConfiguration * p_refer = NULL;
    			if (p_req->Configuration[i].TokenFlag)
    			{
    				p_refer = onvif_find_VideoAnalyticsConfiguration(p_req->Configuration[i].Token);
    			}
    			else
    			{
    				p_refer = g_onvif_cfg.va_cfg;
    			}

    			if (NULL == p_refer)
    			{
    				continue;
    			}
    			
				p_profile->va_cfg = p_refer;
				if (p_profile->va_cfg)
				{	
					p_profile->va_cfg->Configuration.UseCount++;
				}
#endif
			}
			else if (strcasecmp(p_req->Configuration[i].Type, "PTZ") == 0)
			{
#ifdef PTZ_SUPPORT
                ONVIF_PTZConfiguration * p_refer = NULL;
    			if (p_req->Configuration[i].TokenFlag)
    			{
    				p_refer = onvif_find_PTZConfiguration(p_req->Configuration[i].Token);
    			}
    			else
    			{
    				p_refer = g_onvif_cfg.ptz_cfg;
    			}

    			if (NULL == p_refer)
    			{
    				continue;
    			}
    			
				p_profile->ptz_cfg = p_refer;
				if (p_profile->ptz_cfg)
				{
					p_profile->ptz_cfg->Configuration.UseCount++;
				}
#endif			
			}
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

    onvif_MediaProfileChangedNotify(p_profile);
	
	return ONVIF_OK;
}

ONVIF_RET onvif_tr2_DeleteProfile(tr2_DeleteProfile_REQ * p_req)
{
    DeleteProfile_REQ req;
    ONVIF_PROFILE * p_profile = onvif_find_profile(p_req->Token);
	if (NULL == p_profile)
	{
		return ONVIF_ERR_NoProfile;
	}
	
    onvif_MediaProfileChangedNotify(p_profile);

    strcpy(req.ProfileToken, p_req->Token);
    
	return onvif_DeleteProfile(&req);
}

ONVIF_RET onvif_tr2_AddConfiguration(tr2_AddConfiguration_REQ * p_req)
{
	int i;
	ONVIF_PROFILE * p_profile = onvif_find_profile(p_req->ProfileToken);
	if (NULL == p_profile)
	{
		return ONVIF_ERR_NoProfile;
	}

	if (p_req->NameFlag)
	{
		strcpy(p_profile->name, p_req->Name);
	}

	// add configuration to the new profile
	for (i = 0; i < p_req->sizeConfiguration; i++)
	{                                         
		if (strcasecmp(p_req->Configuration[i].Type, "all") == 0)
		{
		    ONVIF_PROFILE * p_refer = NULL;
			if (p_req->Configuration[i].TokenFlag)
			{
				p_refer = onvif_find_profile(p_req->Configuration[i].Token);
			}
			else
			{
				p_refer = g_onvif_cfg.profiles;
			}

			if (NULL == p_refer)
			{
				continue;
			}
		
			p_profile->v_src_cfg = p_refer->v_src_cfg;
			if (p_profile->v_src_cfg)
			{
				p_profile->v_src_cfg->Configuration.UseCount++;
			}
			
			p_profile->v_enc_cfg = p_refer->v_enc_cfg;
			if (p_profile->v_enc_cfg)
			{
				p_profile->v_enc_cfg->Configuration.UseCount++;
			}	
			
			p_profile->metadata_cfg = p_refer->metadata_cfg;
			if (p_profile->metadata_cfg)
			{
				p_profile->metadata_cfg->Configuration.UseCount++;
			}	
			
#ifdef AUDIO_SUPPORT				
			p_profile->a_src_cfg = p_refer->a_src_cfg;
			if (p_profile->a_src_cfg)
			{
				p_profile->a_src_cfg->Configuration.UseCount++;
			}
			
			p_profile->a_enc_cfg = p_refer->a_enc_cfg;
			if (p_profile->a_enc_cfg)
			{
				p_profile->a_enc_cfg->Configuration.UseCount++;
			}

			p_profile->a_dec_cfg = p_refer->a_dec_cfg;
			if (p_profile->a_dec_cfg)
			{
				p_profile->a_dec_cfg->Configuration.UseCount++;
			}
#endif		

#ifdef DEVICEIO_SUPPORT
			p_profile->a_output_cfg = p_refer->a_output_cfg;
			if (p_profile->a_output_cfg)
			{
				p_profile->a_output_cfg->Configuration.UseCount++;
			}
#endif

#ifdef VIDEO_ANALYTICS
			p_profile->va_cfg = p_refer->va_cfg;
			if (p_profile->va_cfg)
			{
				p_profile->va_cfg->Configuration.UseCount++;
			}
#endif

#ifdef PTZ_SUPPORT
			p_profile->ptz_cfg = p_refer->ptz_cfg;
			if (p_profile->ptz_cfg)
			{
				p_profile->ptz_cfg->Configuration.UseCount++;
			}
#endif
		}
		else if (strcasecmp(p_req->Configuration[i].Type, "VideoSource") == 0)
		{
		    ONVIF_VideoSourceConfiguration * p_refer = NULL;
			if (p_req->Configuration[i].TokenFlag)
			{
				p_refer = onvif_find_VideoSourceConfiguration(p_req->Configuration[i].Token);
			}
			else
			{
				p_refer = g_onvif_cfg.v_src_cfg;
			}

			if (NULL == p_refer)
			{
				continue;
			}
			
			p_profile->v_src_cfg = p_refer;
			if (p_profile->v_src_cfg)
			{
				p_profile->v_src_cfg->Configuration.UseCount++;
			}
		}
		else if (strcasecmp(p_req->Configuration[i].Type, "VideoEncoder") == 0)
		{
		    ONVIF_VideoEncoder2Configuration * p_refer = NULL;
			if (p_req->Configuration[i].TokenFlag)
			{
				p_refer = onvif_find_VideoEncoderConfiguration(p_req->Configuration[i].Token);
			}
			else
			{
				p_refer = g_onvif_cfg.v_enc_cfg;
			}

			if (NULL == p_refer)
			{
				continue;
			}
			
			p_profile->v_enc_cfg = p_refer;
			if (p_profile->v_enc_cfg)
			{
				p_profile->v_enc_cfg->Configuration.UseCount++;
			}
		}
		else if (strcasecmp(p_req->Configuration[i].Type, "AudioSource") == 0)
		{
#ifdef AUDIO_SUPPORT
            ONVIF_AudioSourceConfiguration * p_refer = NULL;
			if (p_req->Configuration[i].TokenFlag)
			{
				p_refer = onvif_find_AudioSourceConfiguration(p_req->Configuration[i].Token);
			}
			else
			{
				p_refer = g_onvif_cfg.a_src_cfg;
			}

			if (NULL == p_refer)
			{
				continue;
			}
			
			p_profile->a_src_cfg = p_refer;
			if (p_profile->a_src_cfg)
			{
				p_profile->a_src_cfg->Configuration.UseCount++;
			}
#endif
		}
		else if (strcasecmp(p_req->Configuration[i].Type, "AudioEncoder") == 0)
		{
#ifdef AUDIO_SUPPORT
            ONVIF_AudioEncoder2Configuration * p_refer = NULL;
			if (p_req->Configuration[i].TokenFlag)
			{
				p_refer = onvif_find_AudioEncoderConfiguration(p_req->Configuration[i].Token);
			}
			else
			{
				p_refer = g_onvif_cfg.a_enc_cfg;
			}

			if (NULL == p_refer)
			{
				continue;
			}
			
			p_profile->a_enc_cfg = p_refer;
			if (p_profile->a_enc_cfg)
			{
				p_profile->a_enc_cfg->Configuration.UseCount++;
			}
#endif			
		}
		else if (strcasecmp(p_req->Configuration[i].Type, "AudioOutput") == 0)
		{
#ifdef DEVICEIO_SUPPORT
            ONVIF_AudioOutputConfiguration * p_refer = NULL;
			if (p_req->Configuration[i].TokenFlag)
			{
				p_refer = onvif_find_AudioOutputConfiguration(p_req->Configuration[i].Token);
			}
			else
			{
				p_refer = g_onvif_cfg.a_output_cfg;
			}

			if (NULL == p_refer)
			{
				continue;
			}
			
			p_profile->a_output_cfg = p_refer;
			if (p_profile->a_output_cfg)
			{
				p_profile->a_output_cfg->Configuration.UseCount++;
			}
#endif			
		}
		else if (strcasecmp(p_req->Configuration[i].Type, "AudioDecoder") == 0)
		{
#ifdef AUDIO_SUPPORT		
            ONVIF_AudioDecoderConfiguration * p_refer = NULL;
			if (p_req->Configuration[i].TokenFlag)
			{
				p_refer = onvif_find_AudioDecoderConfiguration(p_req->Configuration[i].Token);
			}
			else
			{
				p_refer = g_onvif_cfg.a_dec_cfg;
			}

			if (NULL == p_refer)
			{
				continue;
			}
			
			p_profile->a_dec_cfg = p_refer;
			if (p_profile->a_dec_cfg)
			{
				p_profile->a_dec_cfg->Configuration.UseCount++;
			}
#endif			
		}
		else if (strcasecmp(p_req->Configuration[i].Type, "Metadata") == 0)
		{
		    ONVIF_MetadataConfiguration * p_refer = NULL;
			if (p_req->Configuration[i].TokenFlag)
			{
				p_refer = onvif_find_MetadataConfiguration(p_req->Configuration[i].Token);
			}
			else
			{
				p_refer = g_onvif_cfg.metadata_cfg;
			}

			if (NULL == p_refer)
			{
				continue;
			}
			
			p_profile->metadata_cfg = p_refer;
			if (p_profile->metadata_cfg)
			{
				p_profile->metadata_cfg->Configuration.UseCount++;
			}
		}
		else if (strcasecmp(p_req->Configuration[i].Type, "Analytics") == 0)
		{
#ifdef VIDEO_ANALYTICS
            ONVIF_VideoAnalyticsConfiguration * p_refer = NULL;
			if (p_req->Configuration[i].TokenFlag)
			{
				p_refer = onvif_find_VideoAnalyticsConfiguration(p_req->Configuration[i].Token);
			}
			else
			{
				p_refer = g_onvif_cfg.va_cfg;
			}

			if (NULL == p_refer)
			{
				continue;
			}
			
			p_profile->va_cfg = p_refer;
			if (p_profile->va_cfg)
			{	
				p_profile->va_cfg->Configuration.UseCount++;
			}
#endif
		}
		else if (strcasecmp(p_req->Configuration[i].Type, "PTZ") == 0)
		{
#ifdef PTZ_SUPPORT
            ONVIF_PTZConfiguration * p_refer = NULL;
			if (p_req->Configuration[i].TokenFlag)
			{
				p_refer = onvif_find_PTZConfiguration(p_req->Configuration[i].Token);
			}
			else
			{
				p_refer = g_onvif_cfg.ptz_cfg;
			}

			if (NULL == p_refer)
			{
				continue;
			}
			
			p_profile->ptz_cfg = p_refer;
			if (p_profile->ptz_cfg)
			{
				p_profile->ptz_cfg->Configuration.UseCount++;
			}
#endif			
		}
	}

    onvif_MediaProfileChangedNotify(p_profile);
    
	return ONVIF_OK;
}

ONVIF_RET onvif_tr2_RemoveConfiguration(tr2_RemoveConfiguration_REQ * p_req)
{
	int i;
	ONVIF_PROFILE * p_profile = onvif_find_profile(p_req->ProfileToken);
	if (NULL == p_profile)
	{
		return ONVIF_ERR_NoProfile;
	}

	for (i = 0; i < p_req->sizeConfiguration; i++)
	{
		if (strcasecmp(p_req->Configuration[i].Type, "all") == 0)
		{
			if (p_profile->v_src_cfg && p_profile->v_src_cfg->Configuration.UseCount > 0)
			{
				p_profile->v_src_cfg->Configuration.UseCount--;
				p_profile->v_src_cfg = NULL;
			}

			if (p_profile->v_enc_cfg && p_profile->v_enc_cfg->Configuration.UseCount > 0)
			{
				p_profile->v_enc_cfg->Configuration.UseCount--;
				p_profile->v_enc_cfg = NULL;
			}

			if (p_profile->metadata_cfg && p_profile->metadata_cfg->Configuration.UseCount > 0)
			{
				p_profile->metadata_cfg->Configuration.UseCount--;
				p_profile->metadata_cfg = NULL;
			}
			
#ifdef AUDIO_SUPPORT	
			if (p_profile->a_src_cfg && p_profile->a_src_cfg->Configuration.UseCount > 0)
			{
				p_profile->a_src_cfg->Configuration.UseCount--;
				p_profile->a_src_cfg = NULL;
			}

			if (p_profile->a_enc_cfg && p_profile->a_enc_cfg->Configuration.UseCount > 0)
			{
				p_profile->a_enc_cfg->Configuration.UseCount--;
				p_profile->a_enc_cfg = NULL;
			}
#endif		

#ifdef DEVICEIO_SUPPORT
			if (p_profile->a_output_cfg && p_profile->a_output_cfg->Configuration.UseCount > 0)
			{
				p_profile->a_output_cfg->Configuration.UseCount--;
				p_profile->a_output_cfg = NULL;
			}
#endif

#ifdef VIDEO_ANALYTICS
			if (p_profile->va_cfg && p_profile->va_cfg->Configuration.UseCount > 0)
			{
				p_profile->va_cfg->Configuration.UseCount--;
				p_profile->va_cfg = NULL;
			}
#endif

#ifdef PTZ_SUPPORT
			if (p_profile->ptz_cfg && p_profile->ptz_cfg->Configuration.UseCount > 0)
			{
				p_profile->ptz_cfg->Configuration.UseCount--;
				p_profile->ptz_cfg = NULL;
			}
#endif
		}
		else if (strcasecmp(p_req->Configuration[i].Type, "VideoSource") == 0)
		{
			if (p_profile->v_src_cfg && p_profile->v_src_cfg->Configuration.UseCount > 0)
			{
				p_profile->v_src_cfg->Configuration.UseCount--;
				p_profile->v_src_cfg = NULL;
			}
		}
		else if (strcasecmp(p_req->Configuration[i].Type, "VideoEncoder") == 0)
		{
			if (p_profile->v_enc_cfg && p_profile->v_enc_cfg->Configuration.UseCount > 0)
			{
				p_profile->v_enc_cfg->Configuration.UseCount--;
				p_profile->v_enc_cfg = NULL;
			}
		}
		else if (strcasecmp(p_req->Configuration[i].Type, "AudioSource") == 0)
		{
#ifdef AUDIO_SUPPORT				
			if (p_profile->a_src_cfg && p_profile->a_src_cfg->Configuration.UseCount > 0)
			{
				p_profile->a_src_cfg->Configuration.UseCount--;
				p_profile->a_src_cfg = NULL;
			}
#endif
		}
		else if (strcasecmp(p_req->Configuration[i].Type, "AudioEncoder") == 0)
		{
#ifdef AUDIO_SUPPORT
			if (p_profile->a_enc_cfg && p_profile->a_enc_cfg->Configuration.UseCount > 0)
			{
				p_profile->a_enc_cfg->Configuration.UseCount--;
				p_profile->a_enc_cfg = NULL;
			}
#endif			
		}
		else if (strcasecmp(p_req->Configuration[i].Type, "AudioOutput") == 0)
		{
#ifdef DEVICEIO_SUPPORT
			if (p_profile->a_output_cfg && p_profile->a_output_cfg->Configuration.UseCount > 0)
			{
				p_profile->a_output_cfg->Configuration.UseCount--;
				p_profile->a_output_cfg = NULL;
			}
#endif			
		}
		else if (strcasecmp(p_req->Configuration[i].Type, "AudioDecoder") == 0)
		{
#ifdef AUDIO_SUPPORT		
		    if (p_profile->a_dec_cfg && p_profile->a_dec_cfg->Configuration.UseCount > 0)
			{
				p_profile->a_dec_cfg->Configuration.UseCount--;
				p_profile->a_dec_cfg = NULL;
			}
#endif			
		}
		else if (strcasecmp(p_req->Configuration[i].Type, "Metadata") == 0)
		{
			if (p_profile->metadata_cfg && p_profile->metadata_cfg->Configuration.UseCount > 0)
			{
				p_profile->metadata_cfg->Configuration.UseCount--;
				p_profile->metadata_cfg = NULL;
			}
		}
		else if (strcasecmp(p_req->Configuration[i].Type, "Analytics") == 0)
		{
#ifdef VIDEO_ANALYTICS
			if (p_profile->va_cfg && p_profile->va_cfg->Configuration.UseCount > 0)
			{
				p_profile->va_cfg->Configuration.UseCount--;
				p_profile->va_cfg = NULL;
			}
#endif
		}
		else if (strcasecmp(p_req->Configuration[i].Type, "PTZ") == 0)
		{
#ifdef PTZ_SUPPORT
			if (p_profile->ptz_cfg && p_profile->ptz_cfg->Configuration.UseCount > 0)
			{
				p_profile->ptz_cfg->Configuration.UseCount--;
				p_profile->ptz_cfg = NULL;
			}
#endif			
		}
	}

    onvif_MediaProfileChangedNotify(p_profile);
    
	return ONVIF_OK;
}

ONVIF_RET onvif_tr2_SetVideoSourceConfiguration(tr2_SetVideoSourceConfiguration_REQ * p_req)
{
    ONVIF_RET ret;
	SetVideoSourceConfiguration_REQ req;

	memcpy(&req.Configuration, &p_req->Configuration, sizeof(onvif_VideoSourceConfiguration));
	req.ForcePersistence = TRUE;
    
	ret = onvif_SetVideoSourceConfiguration(&req);

    if (ONVIF_OK == ret)
    {
	    onvif_MediaConfigurationChangedNotify(req.Configuration.token, "VideoSource");
	}

    // todo : here add your handler code ...

    
	return ret;
}

ONVIF_RET onvif_tr2_SetMetadataConfiguration(tr2_SetMetadataConfiguration_REQ * p_req)
{
    ONVIF_RET ret;
	SetMetadataConfiguration_REQ req;

	memcpy(&req.Configuration, &p_req->Configuration, sizeof(onvif_MetadataConfiguration));
	req.ForcePersistence = TRUE;

	ret = onvif_SetMetadataConfiguration(&req);

    if (ONVIF_OK == ret)
    {
	    onvif_MediaConfigurationChangedNotify(p_req->Configuration.token, "Metadata");
	}

    // todo : here add your handler code ...

    
	return ret;
}

ONVIF_RET onvif_tr2_GetVideoEncoderInstances(tr2_GetVideoEncoderInstances_REQ * p_req, tr2_GetVideoEncoderInstances_RES * p_res)
{
    int total = 0;
    ONVIF_PROFILE * p_profile;
	ONVIF_VideoSourceConfiguration * p_v_src = onvif_find_VideoSourceConfiguration(p_req->ConfigurationToken);
	if (NULL == p_v_src)
	{
		return ONVIF_ERR_NoSource;
	}

	// todo : modify the p_res ...

    // get the stream nums
    p_profile = g_onvif_cfg.profiles;
    while (p_profile)
    {
        if (p_profile->v_src_cfg && strcmp(p_profile->v_src_cfg->Configuration.token, p_req->ConfigurationToken) == 0)
        {
            total++;
        }
        
        p_profile = p_profile->next;
    }
    
	p_res->Info.Total = total;
	
	return ONVIF_OK;
}

ONVIF_RET onvif_tr2_SetSynchronizationPoint(tr2_SetSynchronizationPoint_REQ * p_req)
{
	ONVIF_PROFILE * p_profile = onvif_find_profile(p_req->ProfileToken);
	if (NULL == p_profile)
	{
		return ONVIF_ERR_NoProfile;
	}

	// todo : here add your handler code ...


	return ONVIF_OK;
}

ONVIF_RET onvif_tr2_SetVideoSourceMode(tr2_SetVideoSourceMode_REQ * p_req, tr2_SetVideoSourceMode_RES * p_res)
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

    // todo : here add your handler code ...
    

    return ONVIF_OK;
}

ONVIF_RET onvif_tr2_GetSnapshotUri(const char * lip, uint32 lport, tr2_GetSnapshotUri_REQ * p_req, tr2_GetSnapshotUri_RES * p_res)
{
    ONVIF_PROFILE * p_profile = onvif_find_profile(p_req->ProfileToken);
    if (NULL == p_profile)
    {
        return ONVIF_ERR_InvalidToken;
    }

    // set the media uri
    
    sprintf(p_res->Uri, "%s://%s:%d/snapshot/%s", 
        g_onvif_cfg.https_enable ? "https" : "http", lip, lport, p_profile->token);

    return ONVIF_OK;
}

ONVIF_RET onvif_tr2_GetStreamUri(const char * lip, uint32 lport, tr2_GetStreamUri_REQ * p_req, tr2_GetStreamUri_RES * p_res)
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
	    int len = sizeof(p_res->Uri);

	    if (stricmp(p_req->Protocol, "RtspOverHttp") == 0)
	    {
	        offset += snprintf(p_res->Uri, len, "http://%s/live.sdp", lip);
	    }
	    else
	    {
	        offset += snprintf(p_res->Uri, len, "rtsp://%s/live.sdp", lip);
	    }

	 /*    if (strcasecmp(p_req->Protocol, "RtspUnicast") == 0)
	    {
	        offset += snprintf(p_res->Uri+offset, len-offset, "&amp;t=%s", "unicast");
	        offset += snprintf(p_res->Uri+offset, len-offset, "&amp;p=%s", "udp");
	    }
	    else if (strcasecmp(p_req->Protocol, "RtspMulticast") == 0)
	    {
	        offset += snprintf(p_res->Uri+offset, len-offset, "&amp;t=%s", "multicase");
	        offset += snprintf(p_res->Uri+offset, len-offset, "&amp;p=%s", "udp");
	    }
	    else if (strcasecmp(p_req->Protocol, "RTSP") == 0)
	    {
	        offset += snprintf(p_res->Uri+offset, len-offset, "&amp;t=%s", "unicast");
	        offset += snprintf(p_res->Uri+offset, len-offset, "&amp;p=%s", "rtsp");
	    }
	    else if (strcasecmp(p_req->Protocol, "RtspOverHttp") == 0)
	    {
	        offset += snprintf(p_res->Uri+offset, len-offset, "&amp;t=%s", "unicast");
	        offset += snprintf(p_res->Uri+offset, len-offset, "&amp;p=%s", "http");
	    }

        if (p_profile->v_enc_cfg)
        {            
	        offset += snprintf(p_res->Uri+offset, len-offset, "&amp;ve=%s&amp;w=%d&amp;h=%d", 
	            p_profile->v_enc_cfg->Configuration.Encoding,
	            p_profile->v_enc_cfg->Configuration.Resolution.Width,
	            p_profile->v_enc_cfg->Configuration.Resolution.Height);
	        
	    }

#ifdef AUDIO_SUPPORT
	    if (p_profile->a_enc_cfg)
        {            
	        offset += snprintf(p_res->Uri+offset, len-offset, "&amp;ae=%s&amp;sr=%d", 
	            p_profile->a_enc_cfg->Configuration.Encoding,
	            p_profile->a_enc_cfg->Configuration.SampleRate * 1000);
	        
	    }
#endif	 
		*/
	}
	else
	{
	    strcpy(p_res->Uri, p_profile->stream_uri);
	}

    return ONVIF_OK;
}

ONVIF_RET onvif_tr2_CreateMask(tr2_CreateMask_REQ * p_req)
{
	ONVIF_Mask * p_mask = onvif_add_Mask();
	if (NULL == p_mask)
	{
		return ONVIF_ERR_MaxMasks;
	}

    if (p_req->Mask.ConfigurationToken[0] == '\0' || p_req->Mask.Polygon.sizePoint <= 0)
    {
        return ONVIF_ERR_InvalidArgVal;
    }

	memcpy(&p_mask->Mask, &p_req->Mask, sizeof(onvif_Mask));

	snprintf(p_mask->Mask.token, ONVIF_TOKEN_LEN, "MASK_00%d", g_onvif_cls.mask_idx++);

    // return the token
    strcpy(p_req->Mask.token, p_mask->Mask.token);
    
    // todo : here add your handler code ... 
    
	
	return ONVIF_OK;
}

ONVIF_RET onvif_tr2_DeleteMask(tr2_DeleteMask_REQ * p_req)
{
    ONVIF_Mask * p_prev;
	ONVIF_Mask * p_mask = onvif_find_Mask(p_req->Token);
	if (NULL == p_mask)
	{
		return ONVIF_ERR_NoConfig;
	}

	// todo : here add your handler code ...


	p_prev = g_onvif_cfg.mask;
	if (p_mask == p_prev)
	{
		g_onvif_cfg.mask = p_mask->next;
	}
	else
	{
		while (p_prev->next)
		{
			if (p_prev->next == p_mask)
			{
				break;
			}

			p_prev = p_prev->next;
		}

		p_prev->next = p_mask->next;
	}

	free(p_mask);
	
	return ONVIF_OK;
}

ONVIF_RET onvif_tr2_SetMask(tr2_SetMask_REQ * p_req)
{
    ONVIF_Mask * p_mask = onvif_find_Mask(p_req->Mask.token);
	if (NULL == p_mask)
	{
		return ONVIF_ERR_NoConfig;
	}

    if (p_req->Mask.ConfigurationToken[0] == '\0' || p_req->Mask.Polygon.sizePoint <= 0)
    {
        return ONVIF_ERR_InvalidArgVal;
    }
    
	// todo : here add your handler code ...


	memcpy(&p_mask->Mask, &p_req->Mask, sizeof(onvif_Mask));
	
	return ONVIF_OK;
}


#ifdef AUDIO_SUPPORT

ONVIF_RET onvif_tr2_SetAudioEncoderConfiguration(tr2_SetAudioEncoderConfiguration_REQ * p_req)
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
	strcpy(p_a_enc_cfg->Configuration.Name, p_req->Configuration.Name);
	strcpy(p_a_enc_cfg->Configuration.Encoding, p_req->Configuration.Encoding);

    if (strcasecmp(p_req->Configuration.Encoding, "PCMU"))
    {
        p_req->Configuration.AudioEncoding = AudioEncoding_G711;
    }
    else if (strcasecmp(p_req->Configuration.Encoding, "G726"))
    {
        p_req->Configuration.AudioEncoding = AudioEncoding_G726;
    }
    else if (strcasecmp(p_req->Configuration.Encoding, "MP4A-LATM"))
    {
        p_req->Configuration.AudioEncoding = AudioEncoding_AAC;
    }
    
	memcpy(&p_a_enc_cfg->Configuration.Multicast, &p_req->Configuration.Multicast, sizeof(onvif_MulticastConfiguration));

    onvif_MediaConfigurationChangedNotify(p_req->Configuration.token, "AudioEncoder");
    
	// todo : here add your handler code ...

	return ONVIF_OK;
}

ONVIF_RET onvif_tr2_SetAudioSourceConfiguration(tr2_SetAudioSourceConfiguration_REQ * p_req)
{
    ONVIF_RET ret;
	SetAudioSourceConfiguration_REQ req;

	memcpy(&req.Configuration, &p_req->Configuration, sizeof(onvif_AudioSourceConfiguration));
	req.ForcePersistence = TRUE;

    ret = onvif_SetAudioSourceConfiguration(&req);
    
    if (ONVIF_OK == ret)
    {
	    onvif_MediaConfigurationChangedNotify(req.Configuration.token, "AudioSource");
	}

	// todo : here add your handler code ...
	
	return ret;
}

ONVIF_RET onvif_tr2_SetAudioDecoderConfiguration(tr2_SetAudioDecoderConfiguration_REQ * p_req)
{
    ONVIF_RET ret;
	SetAudioDecoderConfiguration_REQ req;

	memcpy(&req.Configuration, &p_req->Configuration, sizeof(onvif_AudioDecoderConfiguration));
	req.ForcePersistence = TRUE;

    ret = onvif_SetAudioDecoderConfiguration(&req);
    
    if (ONVIF_OK == ret)
    {
	    onvif_MediaConfigurationChangedNotify(req.Configuration.token, "AudioDecoder");
	}

	// todo : here add your handler code ...
	
	return ret;
}

#endif // end of AUDIO_SUPPORT


#ifdef DEVICEIO_SUPPORT

ONVIF_RET onvif_tr2_SetAudioOutputConfiguration(tr2_SetAudioOutputConfiguration_REQ * p_req)
{
    ONVIF_AudioOutput * p_output;
    ONVIF_AudioOutputConfiguration * p_cfg = onvif_find_AudioOutputConfiguration(p_req->Configuration.token);
    if (NULL == p_cfg)
    {
        return ONVIF_ERR_NoAudioOutput;
    }

    p_output = onvif_find_AudioOutput(p_req->Configuration.OutputToken);
    if (NULL == p_output)
    {
        return ONVIF_ERR_NoAudioOutput;
    }

    if (p_req->Configuration.OutputLevel < p_cfg->Options.OutputLevelRange.Min || 
        p_req->Configuration.OutputLevel > p_cfg->Options.OutputLevelRange.Max)
    {
        return ONVIF_ERR_ConfigModify;
    }
    
    // todo : here add your handler code ...

    
    strcpy(p_cfg->Configuration.Name, p_req->Configuration.Name);
    strcpy(p_cfg->Configuration.OutputToken, p_req->Configuration.OutputToken);
    if (p_req->Configuration.SendPrimacyFlag)
    {
        strcpy(p_cfg->Configuration.SendPrimacy, p_req->Configuration.SendPrimacy);
    }
    p_cfg->Configuration.OutputLevel = p_req->Configuration.OutputLevel;

    onvif_MediaConfigurationChangedNotify(p_req->Configuration.token, "AudioOutput");

    return ONVIF_OK;
}

#endif // end of DEVICEIO_SUPPORT


#endif // MEDIA2_SUPPORT



