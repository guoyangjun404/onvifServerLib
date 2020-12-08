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
#include "onvif_cm.h"
#include "onvif_cfg.h"
#include "xml_node.h"
#include "onvif_utils.h"

#include "set_config.h"		// add by xieqingpu


/***************************************************************************************/
extern ONVIF_CFG g_onvif_cfg;
extern ONVIF_CLS g_onvif_cls;


/***************************************************************************************/
void onvif_parse_information_cfg(XMLN * p_node)
{
	XMLN * p_Manufacturer;
	XMLN * p_Model;
	XMLN * p_FirmwareVersion;
	XMLN * p_SerialNumber;
	XMLN * p_HardwareId;

	p_Manufacturer = xml_node_get(p_node, "Manufacturer");
	if (p_Manufacturer && p_Manufacturer->data)
	{
		strncpy(g_onvif_cfg.DeviceInformation.Manufacturer, p_Manufacturer->data, sizeof(g_onvif_cfg.DeviceInformation.Manufacturer)-1);
	}

	p_Model = xml_node_get(p_node, "Model");
	if (p_Model && p_Model->data)
	{
		strncpy(g_onvif_cfg.DeviceInformation.Model, p_Model->data, sizeof(g_onvif_cfg.DeviceInformation.Model)-1);
	}
	
	p_FirmwareVersion = xml_node_get(p_node, "FirmwareVersion");
	if (p_FirmwareVersion && p_FirmwareVersion->data)
	{
		strncpy(g_onvif_cfg.DeviceInformation.FirmwareVersion, p_FirmwareVersion->data, sizeof(g_onvif_cfg.DeviceInformation.FirmwareVersion)-1);
	}

	p_SerialNumber = xml_node_get(p_node, "SerialNumber");
	if (p_SerialNumber && p_SerialNumber->data)
	{
		strncpy(g_onvif_cfg.DeviceInformation.SerialNumber, p_SerialNumber->data, sizeof(g_onvif_cfg.DeviceInformation.SerialNumber)-1);
	}

	p_HardwareId = xml_node_get(p_node, "HardwareId");
	if (p_HardwareId && p_HardwareId->data)
	{
		strncpy(g_onvif_cfg.DeviceInformation.HardwareId, p_HardwareId->data, sizeof(g_onvif_cfg.DeviceInformation.HardwareId));
	}
}

////// add by xieqingpu
void onvif_set_devinfo(CONFIG_Information * p_devInfo)
{
	if (p_devInfo->manufacturer[0] != '\0'){
		strncpy(g_onvif_cfg.DeviceInformation.Manufacturer, p_devInfo->manufacturer, sizeof(g_onvif_cfg.DeviceInformation.Manufacturer)-1);
	}

	if (p_devInfo->model[0] != '\0'){
		strncpy(g_onvif_cfg.DeviceInformation.Model, p_devInfo->model, sizeof(g_onvif_cfg.DeviceInformation.Model)-1);
	}

	if (p_devInfo->firmware_version[0] != '\0'){
		strncpy(g_onvif_cfg.DeviceInformation.FirmwareVersion, p_devInfo->firmware_version, sizeof(g_onvif_cfg.DeviceInformation.FirmwareVersion)-1);
	}

	if (p_devInfo->serial_number[0] != '\0'){
		strncpy(g_onvif_cfg.DeviceInformation.SerialNumber, p_devInfo->serial_number, sizeof(g_onvif_cfg.DeviceInformation.SerialNumber)-1);
	}

	if (p_devInfo->hardware_id[0] != '\0'){
		strncpy(g_onvif_cfg.DeviceInformation.HardwareId ,p_devInfo->hardware_id, sizeof(g_onvif_cfg.DeviceInformation.HardwareId)-1);
	}
}

/////

BOOL onvif_parse_user(XMLN * p_node, onvif_User * p_user)
{
	XMLN * p_username;
    XMLN * p_password;
    XMLN * p_userlevel;
    
	p_username = xml_node_get(p_node, "username");
	if (p_username && p_username->data)
	{
		strncpy(p_user->Username, p_username->data, sizeof(p_user->Username)-1);
	}
	else
	{
		return FALSE;
	}

	p_password = xml_node_get(p_node, "password");
	if (p_password && p_password->data)
	{
		strncpy(p_user->Password, p_password->data, sizeof(p_user->Password)-1);
	}

	p_userlevel = xml_node_get(p_node, "userlevel");
	if (p_userlevel && p_userlevel->data)
	{
		p_user->UserLevel = onvif_StringToUserLevel(p_userlevel->data);
	}

	return TRUE;
}

void onvif_parse_h264_options(XMLN * p_video_encoder, ONVIF_VideoEncoder2Configuration * p_v_enc_cfg)
{
	XMLN * p_h264;
	XMLN * p_gov_length;
	XMLN * p_h264_profile;

	p_h264 = xml_node_get(p_video_encoder, "h264");
	if (NULL == p_h264)
	{
		return;
	}
	
	p_gov_length = xml_node_get(p_h264, "gov_length");
	if (p_gov_length && p_gov_length->data)
	{
	    p_v_enc_cfg->Configuration.GovLengthFlag = 1;
		p_v_enc_cfg->Configuration.GovLength = atoi(p_gov_length->data);
	}

	p_h264_profile = xml_node_get(p_h264, "h264_profile");
	if (p_h264_profile && p_h264_profile->data)
	{
	    p_v_enc_cfg->Configuration.ProfileFlag = 1;
		strncpy(p_v_enc_cfg->Configuration.Profile, p_h264_profile->data, sizeof(p_v_enc_cfg->Configuration.Profile)-1);
	}
}

#ifdef MEDIA2_SUPPORT
void onvif_parse_h265_options(XMLN * p_video_encoder, ONVIF_VideoEncoder2Configuration * p_v_enc_cfg)
{
	XMLN * p_h265;
	XMLN * p_gov_length;
	XMLN * p_h265_profile;

	p_h265 = xml_node_get(p_video_encoder, "h265");
	if (NULL == p_h265)
	{
		return;
	}
	
	p_gov_length = xml_node_get(p_h265, "gov_length");
	if (p_gov_length && p_gov_length->data)
	{
	    p_v_enc_cfg->Configuration.GovLengthFlag = 1;
		p_v_enc_cfg->Configuration.GovLength = atoi(p_gov_length->data);
	}

	p_h265_profile = xml_node_get(p_h265, "h265_profile");
	if (p_h265_profile && p_h265_profile->data)
	{
	    p_v_enc_cfg->Configuration.ProfileFlag = 1;
		strncpy(p_v_enc_cfg->Configuration.Profile, p_h265_profile->data, sizeof(p_v_enc_cfg->Configuration.Profile)-1);
	}
}
#endif

void onvif_parse_mpeg4_options(XMLN * p_video_encoder, ONVIF_VideoEncoder2Configuration * p_v_enc_cfg)
{
	XMLN * p_mpeg4;
	XMLN * p_gov_length;
	XMLN * p_mpeg4_profile;

	p_mpeg4 = xml_node_get(p_video_encoder, "mpeg4");
	if (NULL == p_mpeg4)
	{
		return;
	}
	
	p_gov_length = xml_node_get(p_mpeg4, "gov_length");
	if (p_gov_length && p_gov_length->data)
	{
	    p_v_enc_cfg->Configuration.GovLengthFlag = 1;
		p_v_enc_cfg->Configuration.GovLength = atoi(p_gov_length->data);
	}

	p_mpeg4_profile = xml_node_get(p_mpeg4, "mpeg4_profile");
	if (p_mpeg4_profile && p_mpeg4_profile->data)
	{
	    p_v_enc_cfg->Configuration.ProfileFlag = 1;
		strncpy(p_v_enc_cfg->Configuration.Profile, p_mpeg4_profile->data, sizeof(p_v_enc_cfg->Configuration.Profile)-1);
	}
}

ONVIF_VideoSourceConfiguration * onvif_parse_video_source_cfg(XMLN * p_video_source)
{
	int w = 0, h = 0;
	XMLN * p_width;
	XMLN * p_height;
	ONVIF_VideoSourceConfiguration * p_v_src_cfg = NULL;
	
	p_width = xml_node_get(p_video_source, "width");
	if (p_width && p_width->data)
	{
		w = atoi(p_width->data);
	}

	p_height = xml_node_get(p_video_source, "height");
	if (p_height && p_height->data)
	{
		h = atoi(p_height->data);
	}

	if (w == 0 || h == 0)
	{
		return NULL;
	}
	
	p_v_src_cfg = onvif_find_VideoSourceConfiguration_by_size(w, h);
	if (p_v_src_cfg)
	{
		return p_v_src_cfg;
	}

	p_v_src_cfg = onvif_add_VideoSourceConfiguration(w, h);

	return p_v_src_cfg;
}

ONVIF_VideoEncoder2Configuration * onvif_parse_video_encoder_cfg(XMLN * p_video_encoder)
{
    XMLN * p_width;
    XMLN * p_height;
    XMLN * p_quality;
    XMLN * p_session_timeout;
    XMLN * p_framerate;
    XMLN * p_encoding_interval;
    XMLN * p_bitrate_limit;
    XMLN * p_encoding;
    ONVIF_VideoEncoder2Configuration * p_v_enc_cfg;
	ONVIF_VideoEncoder2Configuration v_enc_cfg;
	
	memset(&v_enc_cfg, 0, sizeof(v_enc_cfg));
	
	p_width = xml_node_get(p_video_encoder, "width");
	if (p_width && p_width->data)
	{
		v_enc_cfg.Configuration.Resolution.Width = atoi(p_width->data);
	}

	p_height = xml_node_get(p_video_encoder, "height");
	if (p_height && p_height->data)
	{
		v_enc_cfg.Configuration.Resolution.Height = atoi(p_height->data);
	}

	p_quality = xml_node_get(p_video_encoder, "quality");
	if (p_quality && p_quality->data)
	{
		v_enc_cfg.Configuration.Quality = (float)atof(p_quality->data);
	}

	p_session_timeout = xml_node_get(p_video_encoder, "session_timeout");
	if (p_session_timeout && p_session_timeout->data)
	{
		v_enc_cfg.Configuration.SessionTimeout = atoi(p_session_timeout->data);
	}

	v_enc_cfg.Configuration.RateControlFlag = 1;
	
	p_framerate = xml_node_get(p_video_encoder, "framerate");
	if (p_framerate && p_framerate->data)
	{
		v_enc_cfg.Configuration.RateControl.FrameRateLimit = (float)atof(p_framerate->data);
	}

	p_encoding_interval = xml_node_get(p_video_encoder, "encoding_interval");
	if (p_encoding_interval && p_encoding_interval->data)
	{
		v_enc_cfg.Configuration.RateControl.EncodingInterval = atoi(p_encoding_interval->data);
	}

	p_bitrate_limit = xml_node_get(p_video_encoder, "bitrate_limit");
	if (p_bitrate_limit && p_bitrate_limit->data)
	{
		v_enc_cfg.Configuration.RateControl.BitrateLimit = atoi(p_bitrate_limit->data);
	}

	p_encoding = xml_node_get(p_video_encoder, "encoding");
	if (p_encoding && p_encoding->data)
	{
		strncpy(v_enc_cfg.Configuration.Encoding, p_encoding->data, sizeof(v_enc_cfg.Configuration.Encoding)-1);
		v_enc_cfg.Configuration.VideoEncoding = onvif_StringToVideoEncoding(p_encoding->data);
	}

	if (strcasecmp(v_enc_cfg.Configuration.Encoding, "MPEG4") == 0 || 
	    strcasecmp(v_enc_cfg.Configuration.Encoding, "MPV4-ES") == 0)
	{
	    strcpy(v_enc_cfg.Configuration.Encoding, "MPV4-ES");
	    v_enc_cfg.Configuration.VideoEncoding = VideoEncoding_MPEG4;
	    
		onvif_parse_mpeg4_options(p_video_encoder, &v_enc_cfg);
	}
	else if (strcasecmp(v_enc_cfg.Configuration.Encoding, "H264") == 0)
	{
		onvif_parse_h264_options(p_video_encoder, &v_enc_cfg);
	}
#ifdef MEDIA2_SUPPORT	
	else if (strcasecmp(v_enc_cfg.Configuration.Encoding, "H265") == 0)
	{
		onvif_parse_h265_options(p_video_encoder, &v_enc_cfg);
	}	
#endif
    else if (strcasecmp(v_enc_cfg.Configuration.Encoding, "JPEG"))
	{
	    return NULL;   
	}

	p_v_enc_cfg = onvif_find_VideoEncoderConfiguration_by_param(&v_enc_cfg);
	if (p_v_enc_cfg)
	{
		return p_v_enc_cfg;
	}

	p_v_enc_cfg = onvif_add_VideoEncoderConfiguration(&v_enc_cfg);

	return p_v_enc_cfg;
}

#ifdef AUDIO_SUPPORT

ONVIF_AudioSourceConfiguration * onvif_parse_audio_source_cfg(XMLN * p_audio_source)
{	
	ONVIF_AudioSourceConfiguration * p_a_src_cfg = g_onvif_cfg.a_src_cfg;
	if (p_a_src_cfg)
	{
		return p_a_src_cfg;
	}

	p_a_src_cfg = onvif_add_AudioSourceConfiguration();

	return p_a_src_cfg;
}

ONVIF_AudioEncoder2Configuration * onvif_parse_audio_encoder_cfg(XMLN * p_audio_encoder)
{
    XMLN * p_session_timeout;
    XMLN * p_sample_rate;
    XMLN * p_bitrate;
    XMLN * p_encoding;
    ONVIF_AudioEncoder2Configuration * p_a_enc_cfg;    
	ONVIF_AudioEncoder2Configuration a_enc_cfg;
	
	memset(&a_enc_cfg, 0, sizeof(ONVIF_AudioEncoder2Configuration));
	
	p_session_timeout = xml_node_get(p_audio_encoder, "session_timeout");
	if (p_session_timeout && p_session_timeout->data)
	{
		a_enc_cfg.Configuration.SessionTimeout = atoi(p_session_timeout->data);
	}

	p_sample_rate = xml_node_get(p_audio_encoder, "sample_rate");
	if (p_sample_rate && p_sample_rate->data)
	{
		a_enc_cfg.Configuration.SampleRate = atoi(p_sample_rate->data);
	}

	p_bitrate = xml_node_get(p_audio_encoder, "bitrate");
	if (p_bitrate && p_bitrate->data)
	{
		a_enc_cfg.Configuration.Bitrate = atoi(p_bitrate->data);
	}	

	p_encoding = xml_node_get(p_audio_encoder, "encoding");
	if (p_encoding && p_encoding->data)
	{
		strncpy(a_enc_cfg.Configuration.Encoding, p_encoding->data, sizeof(a_enc_cfg.Configuration.Encoding)-1);
		a_enc_cfg.Configuration.AudioEncoding = onvif_StringToAudioEncoding(p_encoding->data);
	}

#ifdef MEDIA2_SUPPORT
    if (strcasecmp(a_enc_cfg.Configuration.Encoding, "PCMU") == 0 || 
	    strcasecmp(a_enc_cfg.Configuration.Encoding, "G711") == 0)
	{
	    strcpy(a_enc_cfg.Configuration.Encoding, "PCMU");
	    a_enc_cfg.Configuration.AudioEncoding = AudioEncoding_G711;
	}
	else if (strcasecmp(a_enc_cfg.Configuration.Encoding, "AAC") == 0 || 
	    strcasecmp(a_enc_cfg.Configuration.Encoding, "MP4A-LATM") == 0)
	{
	    strcpy(a_enc_cfg.Configuration.Encoding, "MP4A-LATM");
	    a_enc_cfg.Configuration.AudioEncoding = AudioEncoding_AAC;
	}
#endif

	p_a_enc_cfg = onvif_find_AudioEncoderConfiguration_by_param(&a_enc_cfg);
	if (p_a_enc_cfg)
	{
		return p_a_enc_cfg;
	}

	p_a_enc_cfg = onvif_add_AudioEncoderConfiguration(&a_enc_cfg);

	return p_a_enc_cfg;
}

#endif // end of AUDIO_SUPPORT

void onvif_parse_profile(XMLN * p_profile)
{
    XMLN * p_video_source;
    XMLN * p_video_encoder;
#ifdef AUDIO_SUPPORT    
    XMLN * p_audio_source;
    XMLN * p_audio_encoder;
#endif    
    XMLN * p_stream_uri;    
	ONVIF_PROFILE * profile;

	profile = onvif_add_profile(TRUE);
	if (NULL == profile)
	{
		return;
	}
	
	p_video_source = xml_node_get(p_profile, "video_source");
	if (p_video_source)
	{
		profile->v_src_cfg = onvif_parse_video_source_cfg(p_video_source);
		profile->v_src_cfg->Configuration.UseCount++;
	}

	p_video_encoder = xml_node_get(p_profile, "video_encoder");
	if (p_video_encoder)
	{
		profile->v_enc_cfg = onvif_parse_video_encoder_cfg(p_video_encoder);
		if (profile->v_enc_cfg)
		{
		    profile->v_enc_cfg->Configuration.UseCount++;
		}
	}

#ifdef AUDIO_SUPPORT
	p_audio_source = xml_node_get(p_profile, "audio_source");
	if (p_audio_source)
	{
		profile->a_src_cfg = onvif_parse_audio_source_cfg(p_audio_source);
		profile->a_src_cfg->Configuration.UseCount++;
	}
	
	p_audio_encoder = xml_node_get(p_profile, "audio_encoder");
	if (p_audio_encoder)
	{
		profile->a_enc_cfg = onvif_parse_audio_encoder_cfg(p_audio_encoder);
		profile->a_enc_cfg->Configuration.UseCount++;
	}
#endif

	p_stream_uri = xml_node_get(p_profile, "stream_uri");
	if (p_stream_uri && p_stream_uri->data && strlen(p_stream_uri->data) > 0)
	{
		strncpy(profile->stream_uri, p_stream_uri->data, sizeof(profile->stream_uri));
	}	
}

void onvif_parse_event_cfg(XMLN * p_event)
{
	XMLN * p_renew_interval;
	XMLN * p_simulate_enable;
	XMLN * p_simulate_interval;

	p_renew_interval = xml_node_get(p_event, "renew_interval");
	if (p_renew_interval && p_renew_interval->data)
	{
		g_onvif_cfg.evt_renew_time = atoi(p_renew_interval->data);
	}

	p_simulate_enable = xml_node_get(p_event, "simulate_enable");
	if (p_simulate_enable && p_simulate_enable->data)
	{
		g_onvif_cfg.evt_sim_flag = atoi(p_simulate_enable->data);
	}
	
	p_simulate_interval = xml_node_get(p_event, "simulate_interval");
	if (p_simulate_interval && p_simulate_interval->data)
	{
		g_onvif_cfg.evt_sim_interval = atoi(p_simulate_interval->data);
	}
}

BOOL onvif_parse_server(XMLN * p_node, ONVIF_SRV * p_req)
{
    XMLN * p_serv_ip;	
	XMLN * p_serv_port;
	
    p_serv_ip = xml_node_get(p_node, "server_ip");
	if (NULL == p_serv_ip)
	{
	    return FALSE;
	}
	else if (p_serv_ip->data)
	{
		strncpy(p_req->serv_ip, p_serv_ip->data, sizeof(p_req->serv_ip)-1);
	}
	
	p_serv_port = xml_node_get(p_node, "server_port");
	if (NULL == p_serv_port || NULL == p_serv_port->data)
	{
	    return FALSE;
	}
	else
	{
		p_req->serv_port = atoi(p_serv_port->data);
	}

	return TRUE;
}

void onvif_parse_cfg(char * xml_buff, int rlen)
{
	XMLN * p_node;
	XMLN * p_servers;
	XMLN * p_http_max_users;
#ifdef HTTPS
	XMLN * p_https_enable;
#endif
	XMLN * p_need_auth;
	XMLN * p_log_enable;
	XMLN * p_information;
	XMLN * p_user;
	XMLN * p_profile;
	XMLN * p_scope;
	XMLN * p_event;

	p_node = xxx_hxml_parse(xml_buff, rlen);
	if (NULL == p_node)
	{
		return;
	}

    if (onvif_parse_server(p_node, &g_onvif_cfg.servs[0]))
    {
        g_onvif_cfg.servs_num++;
    }

	p_servers = xml_node_get(p_node, "servers");
	while (p_servers && strcmp(p_servers->name, "servers") == 0)
	{
	    int idx = g_onvif_cfg.servs_num;

	    if (onvif_parse_server(p_servers, &g_onvif_cfg.servs[idx]))
	    {
	        g_onvif_cfg.servs_num++;

	        if (g_onvif_cfg.servs_num >= ARRAY_SIZE(g_onvif_cfg.servs))
	        {
	            break;
	        }
	    }
	    
	    p_servers = p_servers->next;
	}

	p_http_max_users = xml_node_get(p_node, "http_max_users");
	if (p_http_max_users && p_http_max_users->data)
	{
		g_onvif_cfg.http_max_users = atoi(p_http_max_users->data);
	}

#ifdef HTTPS
	p_https_enable = xml_node_get(p_node, "https_enable");
	if (p_https_enable && p_https_enable->data)
	{
		g_onvif_cfg.https_enable = atoi(p_https_enable->data);
	}
#endif

	p_need_auth = xml_node_get(p_node, "need_auth");
	if (p_need_auth && p_need_auth->data)
	{
		g_onvif_cfg.need_auth = atoi(p_need_auth->data);
	}

	/* p_log_enable = xml_node_get(p_node, "log_enable");
	if (p_log_enable && p_log_enable->data)
	{
		// g_onvif_cfg.log_enable = atoi(p_log_enable->data);
	} */
	
	/* p_information = xml_node_get(p_node, "information");	  //设备信息
	if (p_information)
	{
		onvif_parse_information_cfg(p_information);
	} */

////// add by xieqingpu
	int ret;
	CONFIG_Information devInfo;
	memset(&devInfo, 0, sizeof(CONFIG_Information));	//设备信息

    ret = onvif_get_devinfo(&devInfo);
	if ( ret == 0)
		onvif_set_devinfo(&devInfo);
/////


/* 	p_user = xml_node_get(p_node, "user");
	while (p_user && strcmp(p_user->name, "user") == 0)
	{
		onvif_User user;
		memset(&user, 0, sizeof(user));
		
    	if (onvif_parse_user(p_user, &user))
    	{
    		// user.fixed = TRUE;
    		onvif_add_user(&user);	//g_onvif_cfg.users
    	}

    	p_user = p_user->next;
	}
 */
	//// add by xieqingpu
	/* if (writeUsers(g_onvif_cfg.users, ARRAY_SIZE(g_onvif_cfg.users)) != 0)   //写用户到文件保存起来
		printf(" write user faile.\n"); */

	/* onvif_User p_users[10] = {0};
	if (readUsers(p_users, ARRAY_SIZE(g_onvif_cfg.users)) != 0)	// 读出用户打印看看是否正确//
		printf(" read user faile.\n");
	for(int i = 0; i <ARRAY_SIZE(g_onvif_cfg.users); i++)
	{
		printf("xxx user[%d],name:%s password:%s level:%d\n",i, p_users[i].Username, p_users[i].Password ,p_users[i].UserLevel);
	} */

	add_to_Gusers();    	 // 用户 。从文件读取用户复制给全区变量g_onvif_cfg.users，给soap_GetUsers显示在前端
	////

	
	p_profile = xml_node_get(p_node, "profile");
	while (p_profile && strcmp(p_profile->name, "profile") == 0)
	{
    	onvif_parse_profile(p_profile);

    	p_profile = p_profile->next;
	}

	p_scope = xml_node_get(p_node, "scope");
	while (p_scope && strcmp(p_profile->name, "scope") == 0)
	{
		if (p_scope->data)
		{
			onvif_add_scope(p_scope->data, TRUE);
		}

		p_scope = p_scope->next;
	}

	p_event = xml_node_get(p_node, "event");
	if (p_event)
	{
		onvif_parse_event_cfg(p_event);
	}

	xml_node_del(p_node);
}

void onvif_load_cfg()
{
	int len;
    int rlen;
    FILE * fp;
    char * xml_buff;

    // read config file
	fp = fopen("config.xml", "r");
	if (NULL == fp)
	{
		return;
	}
	
	fseek(fp, 0, SEEK_END);
	
	len = ftell(fp);
	if (len <= 0)
	{
		fclose(fp);
		return;
	}
	fseek(fp, 0, SEEK_SET);
	
	xml_buff = (char *) malloc(len + 1);	
	if (NULL == xml_buff)
	{
		fclose(fp);
		return;
	}

	rlen = fread(xml_buff, 1, len, fp);
	
	fclose(fp);

	onvif_parse_cfg(xml_buff, rlen);

	free(xml_buff);
}


BOOL onvif_read_device_uuid(char * buff, int bufflen)
{
    int len, rlen;
    FILE * fp;
    
	fp = fopen("uuid.txt", "r");
	if (NULL == fp)
	{
		return FALSE;
	}

	fseek(fp, 0, SEEK_END);

	// get file length
	len = ftell(fp);
	if (len <= 0)
	{
		fclose(fp);
		return FALSE;
	}
	fseek(fp, 0, SEEK_SET);

    if (bufflen <= len)
    {
        printf("filelen = %d, bufflen = %d\r\n", len, bufflen);
    }
    else
    {
    	rlen = fread(buff, 1, len, fp);

        // remove \r\n
    	while (rlen > 0 && (buff[rlen-1] == '\r' || buff[rlen-1] == '\n'))
    	{
    	    rlen--;
    	    buff[rlen] = '\0';
    	}
	}
	
	fclose(fp);

	return TRUE;
}

BOOL onvif_save_device_uuid(char * buff)
{
    FILE * fp;
	
	fp = fopen("uuid.txt", "w");
	if (NULL == fp)
	{
	    printf("open file uuid.txt failed\r\n");
		return FALSE;
	}

	fwrite(buff, 1, strlen(buff), fp);

	fclose(fp);

	return TRUE;
}

#ifdef PROFILE_Q_SUPPORT

int onvif_read_device_state()
{
    int state = 0, rlen;
    char buff[8] = {'\0'};
    FILE * fp;
    
	fp = fopen("devst.txt", "r");
	if (NULL == fp)
	{
		return state;
	}
	
	rlen = fread(buff, 1, 1, fp);
	if (1 == rlen)
	{
	    state = atoi(buff);
	}
	
	fclose(fp);

	return state;
}

BOOL onvif_save_device_state(int state)
{
    char buff[8] = {'\0'};
    FILE * fp;
	
	fp = fopen("devst.txt", "w");
	if (NULL == fp)
	{
	    printf("open file devst.txt failed\r\n");
		return FALSE;
	}

    sprintf(buff, "%d", state);
	fwrite(buff, 1, strlen(buff), fp);

	fclose(fp);

	return TRUE;
}

#endif // end of PROFILE_Q_SUPPORT


