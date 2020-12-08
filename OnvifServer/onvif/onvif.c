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
#include "onvif_device.h"
#include "xml_node.h"
#include "onvif_event.h"
#include "onvif_utils.h"
#include "onvif_cfg.h"
#include "util.h"
#include <math.h>

#include "set_config.h"		//add by xieqingpu
#include "utils_log.h"
#ifdef LIBICAL
#include "icalvcal.h"
#include "vcc.h"   
#endif

/***************************************************************************************/
ONVIF_CFG g_onvif_cfg;
ONVIF_CLS g_onvif_cls;


/***************************************************************************************/

ONVIF_PROFILE * onvif_find_profile(const char * token)
{
    ONVIF_PROFILE * p_tmp = g_onvif_cfg.profiles;
    
    if (NULL == token)
    {
        return NULL;
    }

    while (p_tmp)
    {
        if (strcasecmp(token, p_tmp->token) == 0)
        {
            return p_tmp;
        }
        
        p_tmp = p_tmp->next;
    }

    return NULL;
}

ONVIF_VideoSource * onvif_find_VideoSource(const char * token)
{
    ONVIF_VideoSource * p_tmp = g_onvif_cfg.v_src;
    
    if (NULL == token)
    {
        return NULL;
    }

    while (p_tmp)
    {
        if (strcasecmp(token, p_tmp->VideoSource.token) == 0)
        {
            return p_tmp;
        }
        
        p_tmp = p_tmp->next;
    }

    return NULL;
}

ONVIF_VideoSource * onvif_find_VideoSource_by_size(int w, int h)
{
	ONVIF_VideoSource * p_tmp = g_onvif_cfg.v_src;
    while (p_tmp)
    {
        if (p_tmp->VideoSource.Resolution.Width == w && p_tmp->VideoSource.Resolution.Height == h)
        {
            return p_tmp;
        }
        
        p_tmp = p_tmp->next;
    }

    return NULL;
}

ONVIF_VideoSource * onvif_add_VideoSource(int w, int h)
{
	ONVIF_VideoSource * p_tmp;
	ONVIF_VideoSource * p_new = (ONVIF_VideoSource *) malloc(sizeof(ONVIF_VideoSource));
	if (NULL == p_new)
	{
		return NULL;
	}

	memset(p_new, 0, sizeof(ONVIF_VideoSource));

	p_new->VideoSource.Framerate = 25;
	p_new->VideoSource.Resolution.Width = w;
	p_new->VideoSource.Resolution.Height = h;

	snprintf(p_new->VideoSource.token, ONVIF_TOKEN_LEN, "V_SRC_00%d", g_onvif_cls.v_src_idx);

    // init video source mode
    p_new->VideoSourceMode.Enabled = 1;
    p_new->VideoSourceMode.Reboot = 1;
    p_new->VideoSourceMode.MaxFramerate = 30.0f;
    p_new->VideoSourceMode.MaxResolution.Width = w;
    p_new->VideoSourceMode.MaxResolution.Height = h;
    snprintf(p_new->VideoSourceMode.token, ONVIF_TOKEN_LEN, "V_SRC_MODE_00%d", g_onvif_cls.v_src_idx);
    strcpy(p_new->VideoSourceMode.Encodings, "H264 MP4 MJPEG");

#ifdef THERMAL_SUPPORT
    p_new->ThermalSupport = onvif_init_Thermal(p_new);    
#endif
    
    p_tmp = g_onvif_cfg.v_src;
	if (NULL == p_tmp)
	{
		g_onvif_cfg.v_src = p_new;
	}
	else
	{
		while (p_tmp && p_tmp->next) p_tmp = p_tmp->next;

		p_tmp->next = p_new;
	}

	return p_new;
}

ONVIF_VideoSourceConfiguration * onvif_find_VideoSourceConfiguration(const char * token)
{
    ONVIF_VideoSourceConfiguration * p_tmp = g_onvif_cfg.v_src_cfg;
    
    if (NULL == token)
    {
        return NULL;
    }

    while (p_tmp)
    {
        if (strcasecmp(token, p_tmp->Configuration.token) == 0)
        {
            return p_tmp;
        }
        
        p_tmp = p_tmp->next;
    }

    return NULL;
}


ONVIF_VideoEncoder2Configuration * onvif_find_VideoEncoderConfiguration(const char * token)
{
    ONVIF_VideoEncoder2Configuration * p_tmp = g_onvif_cfg.v_enc_cfg;
    
    if (NULL == token)
    {
        return NULL;
    }

    while (p_tmp)
    {
        if (strcasecmp(token, p_tmp->Configuration.token) == 0)
        {
            return p_tmp;
        }
        
        p_tmp = p_tmp->next;
    }

    return NULL;
}


ONVIF_VideoSourceConfiguration * onvif_find_VideoSourceConfiguration_by_size(int w, int h)
{
	ONVIF_VideoSourceConfiguration * p_tmp = g_onvif_cfg.v_src_cfg;
	while (p_tmp)
	{
		if (p_tmp->Configuration.Bounds.width == w && 
			p_tmp->Configuration.Bounds.height == h)
		{
			break;
		}

		p_tmp = p_tmp->next;
	}

	return p_tmp;
}

ONVIF_VideoSourceConfiguration * onvif_add_VideoSourceConfiguration(int w, int h)
{
    ONVIF_VideoSource * p_v_src;
    ONVIF_VideoSourceConfiguration * p_tmp;
	ONVIF_VideoSourceConfiguration * p_new = (ONVIF_VideoSourceConfiguration *) malloc(sizeof(ONVIF_VideoSourceConfiguration));
	if (NULL == p_new)
	{
		return NULL;
	}

	memset(p_new, 0, sizeof(ONVIF_VideoSourceConfiguration));

	p_new->Configuration.Bounds.x = 0;
	p_new->Configuration.Bounds.y = 0;
	p_new->Configuration.Bounds.width = w;
	p_new->Configuration.Bounds.height = h;

	p_new->Configuration.UseCount = 0;

	snprintf(p_new->Configuration.Name, ONVIF_NAME_LEN, "V_SRC_CFG_00%d", g_onvif_cls.v_src_idx);
    snprintf(p_new->Configuration.token, ONVIF_TOKEN_LEN, "V_SRC_CFG_00%d", g_onvif_cls.v_src_idx);

    p_v_src = onvif_find_VideoSource_by_size(w, h);
	if (NULL == p_v_src)
	{
		p_v_src = onvif_add_VideoSource(w, h);
	}
	
	strcpy(p_new->Configuration.SourceToken, p_v_src->VideoSource.token);

    g_onvif_cls.v_src_idx++;

	p_tmp = g_onvif_cfg.v_src_cfg;
	if (NULL == p_tmp)
	{
		g_onvif_cfg.v_src_cfg = p_new;
	}
	else
	{
		while (p_tmp && p_tmp->next) p_tmp = p_tmp->next;

		p_tmp->next = p_new;
	}

	return p_new;
}

ONVIF_VideoEncoder2Configuration * onvif_find_VideoEncoderConfiguration_by_param(ONVIF_VideoEncoder2Configuration * p_v_enc_cfg)
{
	ONVIF_VideoEncoder2Configuration * p_tmp = g_onvif_cfg.v_enc_cfg;
	while (p_tmp)
	{
		if (p_tmp->Configuration.Resolution.Width == p_v_enc_cfg->Configuration.Resolution.Width && 
			p_tmp->Configuration.Resolution.Height == p_v_enc_cfg->Configuration.Resolution.Height && 
			fabs(p_tmp->Configuration.Quality - p_v_enc_cfg->Configuration.Quality) < 0.1 && 
			p_tmp->Configuration.SessionTimeout == p_v_enc_cfg->Configuration.SessionTimeout && 
			fabs(p_tmp->Configuration.RateControl.FrameRateLimit - p_v_enc_cfg->Configuration.RateControl.FrameRateLimit) < 0.1 && 
			p_tmp->Configuration.RateControl.EncodingInterval == p_v_enc_cfg->Configuration.RateControl.EncodingInterval && 
			p_tmp->Configuration.RateControl.BitrateLimit == p_v_enc_cfg->Configuration.RateControl.BitrateLimit && 
			strcmp(p_tmp->Configuration.Encoding, p_v_enc_cfg->Configuration.Encoding) == 0)
		{
			break;
		}

		p_tmp = p_tmp->next;
	}

	return p_tmp;
}

void onvif_init_multicast_cfg(onvif_MulticastConfiguration * p_cfg)
{
	p_cfg->Port = 32002;
	p_cfg->TTL = 2;
	p_cfg->AutoStart = FALSE;
	strcpy(p_cfg->IPv4Address, "239.0.1.0");
}

void onvif_init_VideoEncoder2ConfigurationOptions
(
onvif_VideoEncoder2ConfigurationOptions * p_option,
const char * Encoding
)
{
    strcpy(p_option->Encoding, Encoding);
    if (strcasecmp(Encoding, "JPEG") == 0)
    {
        p_option->VideoEncoding = VideoEncoding_JPEG;
    }
    else if (strcasecmp(Encoding, "MPV4-ES") == 0)
    {
        p_option->VideoEncoding = VideoEncoding_MPEG4;

        p_option->GovLengthRangeFlag = 1;
        strcpy(p_option->GovLengthRange, "1 60");

        p_option->ProfilesSupportedFlag = 1;
	    strcpy(p_option->ProfilesSupported, "Simple AdvancedSimple");
    }
    else if (strcasecmp(Encoding, "H264") == 0)
    {
        p_option->VideoEncoding = VideoEncoding_H264;

        p_option->GovLengthRangeFlag = 1;
        strcpy(p_option->GovLengthRange, "1 60");

        p_option->ProfilesSupportedFlag = 1;
	    strcpy(p_option->ProfilesSupported, "Baseline Main High");
    }
    else if (strcasecmp(Encoding, "H265") == 0)
    {
        p_option->VideoEncoding = VideoEncoding_Unknown;

        p_option->GovLengthRangeFlag = 1;
        strcpy(p_option->GovLengthRange, "1 60");

        p_option->ProfilesSupportedFlag = 1;
	    strcpy(p_option->ProfilesSupported, "Main Main10");
    }

    p_option->FrameRatesSupportedFlag = 1;
    sprintf(p_option->FrameRatesSupported, "30 29 25 23 20 19 18 15 12 10 8 5");

    p_option->QualityRange.Min = 0;
    p_option->QualityRange.Max = 100;

    p_option->BitrateRange.Min = 64;
    p_option->BitrateRange.Max = 4096;

    p_option->ResolutionsAvailable[0].Width = 1920;
	p_option->ResolutionsAvailable[0].Height = 1080;
	p_option->ResolutionsAvailable[1].Width = 1280;
	p_option->ResolutionsAvailable[1].Height = 720;
	p_option->ResolutionsAvailable[2].Width = 640;
	p_option->ResolutionsAvailable[2].Height = 480;
	p_option->ResolutionsAvailable[3].Width = 352;
	p_option->ResolutionsAvailable[3].Height = 288;
	p_option->ResolutionsAvailable[4].Width = 320;
	p_option->ResolutionsAvailable[4].Height = 240;
    
}

ONVIF_VideoEncoder2Configuration * onvif_add_VideoEncoderConfiguration(ONVIF_VideoEncoder2Configuration * p_v_enc_cfg)
{
    ONVIF_VideoEncoder2Configuration * p_tmp;
	ONVIF_VideoEncoder2Configuration * p_new = (ONVIF_VideoEncoder2Configuration *) malloc(sizeof(ONVIF_VideoEncoder2Configuration));
	if (NULL == p_new)
	{
		return NULL;
	}

	memset(p_new, 0, sizeof(ONVIF_VideoEncoder2Configuration));
	
	memcpy(&p_new->Configuration, &p_v_enc_cfg->Configuration, sizeof(onvif_VideoEncoder2Configuration));

	snprintf(p_new->Configuration.Name, ONVIF_NAME_LEN, "V_ENC_00%d", g_onvif_cls.v_enc_idx);
    snprintf(p_new->Configuration.token, ONVIF_TOKEN_LEN, "V_ENC_00%d", g_onvif_cls.v_enc_idx);

	onvif_init_multicast_cfg(&p_new->Configuration.Multicast);	
	
    g_onvif_cls.v_enc_idx++;

	p_tmp = g_onvif_cfg.v_enc_cfg;
	if (NULL == p_tmp)
	{
		g_onvif_cfg.v_enc_cfg = p_new;
	}
	else
	{
		while (p_tmp && p_tmp->next) p_tmp = p_tmp->next;

		p_tmp->next = p_new;
	}
    
	return p_new;
}

ONVIF_PROFILE * onvif_add_profile(BOOL fixed)
{
    ONVIF_PROFILE * p_tmp;
	ONVIF_PROFILE * p_new = (ONVIF_PROFILE *) malloc(sizeof(ONVIF_PROFILE));
	if (NULL == p_new)
	{
		return NULL;
	}

	memset(p_new, 0, sizeof(ONVIF_PROFILE));

	p_new->fixed = fixed;

	snprintf(p_new->name, ONVIF_NAME_LEN, "PROFILE_00%d", g_onvif_cls.profile_idx);
    snprintf(p_new->token, ONVIF_TOKEN_LEN, "PROFILE_00%d", g_onvif_cls.profile_idx);

    g_onvif_cls.profile_idx++;

	p_tmp = g_onvif_cfg.profiles;
	if (NULL == p_tmp)
	{
		g_onvif_cfg.profiles = p_new;
	}
	else
	{
		while (p_tmp && p_tmp->next) p_tmp = p_tmp->next;

		p_tmp->next = p_new;
	}

	return p_new;
}

BOOL onvif_is_scope_exist(const char * scope)
{
    int i;
	for (i = 0; i < ARRAY_SIZE(g_onvif_cfg.scopes); i++)
	{
		if (strcmp(scope, g_onvif_cfg.scopes[i].ScopeItem) == 0)
		{
			return TRUE;
		}
	}

	return FALSE;	
}

onvif_Scope * onvif_find_scope(const char * scope)
{
    int i;
	for (i = 0; i < ARRAY_SIZE(g_onvif_cfg.scopes); i++)
	{
		if (strcmp(g_onvif_cfg.scopes[i].ScopeItem, scope) == 0)
		{
			return &g_onvif_cfg.scopes[i];
		}
	}

	return NULL;
}

onvif_Scope * onvif_get_idle_scope()
{
    int i;
	for (i = 0; i < ARRAY_SIZE(g_onvif_cfg.scopes); i++)
	{
		if (g_onvif_cfg.scopes[i].ScopeItem[0] == '\0')
		{
			return &g_onvif_cfg.scopes[i];
		}
	}

	return NULL;
}

ONVIF_RET onvif_add_scope(const char * scope, BOOL fixed)
{
    onvif_Scope * p_scope;
    
	if (onvif_is_scope_exist(scope) == TRUE)
	{
		return ONVIF_ERR_ScopeOverwrite;
	}

	p_scope = onvif_get_idle_scope();
	if (p_scope)
	{
		p_scope->ScopeDef = fixed ? ScopeDefinition_Fixed : ScopeDefinition_Configurable;
		strncpy(p_scope->ScopeItem, scope, sizeof(p_scope->ScopeItem)-1);
		return ONVIF_OK;
	}

	return ONVIF_ERR_TooManyScopes;
}

BOOL onvif_is_user_exist(const char * username)
{
    int i;
	for (i = 0; i < ARRAY_SIZE(g_onvif_cfg.users); i++)
	{
		if (strcmp(username, g_onvif_cfg.users[i].Username) == 0)
		{
			return TRUE;
		}
	}

	return FALSE;	
}


//// add by xieqingpu
ONVIF_RET add_to_Gusers(void)
{
	// g_onvif_cfg.users[MAX_USERS] = {0};
	if (readUsers(g_onvif_cfg.users, ARRAY_SIZE(g_onvif_cfg.users)) != 0)	  //从文件User.dat读取用户
	{
		printf(" read user faile.\n");
		return -1;
	}else{
		/* for(int i = 0; i <ARRAY_SIZE(g_onvif_cfg.users); i++)
		{
			printf("xxx  read user[%d],name:%s password:%s level:%d\n",i, g_onvif_cfg.users[i].Username, g_onvif_cfg.users[i].Password ,g_onvif_cfg.users[i].UserLevel);
		} */
		return ONVIF_OK;
	}
}

////


ONVIF_RET onvif_add_user(onvif_User * p_user)
{
    onvif_User * p_idle_user;
    
	if (onvif_is_user_exist(p_user->Username) == TRUE)
	{
		return ONVIF_ERR_UsernameClash;
	}

	p_idle_user = onvif_get_idle_user();	// g_onvif_cfg.users[i]
	if (p_idle_user)
	{
		memcpy(p_idle_user, p_user, sizeof(onvif_User));
		return ONVIF_OK;
	}

	return ONVIF_ERR_TooManyUsers;
}

onvif_User * onvif_find_user(const char * username)
{
    int i;
	for (i = 0; i < ARRAY_SIZE(g_onvif_cfg.users); i++)
	{
		if (strcmp(g_onvif_cfg.users[i].Username, username) == 0)
		{
			return &g_onvif_cfg.users[i];
		}
	}

	return NULL;
}

onvif_User * onvif_get_idle_user()
{
    int i;
	for (i = 0; i < ARRAY_SIZE(g_onvif_cfg.users); i++)
	{
		if (g_onvif_cfg.users[i].Username[0] == '\0')
		{
			return &g_onvif_cfg.users[i];
		}
	}

	return NULL;
}

const char * onvif_get_user_pass(const char * username)
{
    onvif_User * p_user;
    
    if (NULL == username || strlen(username) == 0)
    {
        return NULL;
    }
    
    p_user = onvif_find_user(username);
	if (NULL != p_user)
	{
	    return p_user->Password;
	}

	return NULL;	
}

ONVIF_NetworkInterface * onvif_find_NetworkInterface(const char * token)
{
    ONVIF_NetworkInterface * p_tmp = g_onvif_cfg.network.interfaces;

    while (p_tmp)
    {
        if (strcmp(p_tmp->NetworkInterface.token, token) == 0)
        {
            break;
        }

        p_tmp = p_tmp->next;
    }

    return p_tmp;
}

ONVIF_OSDConfiguration * onvif_add_OSDConfiguration()
{
    ONVIF_OSDConfiguration * p_tmp;
	ONVIF_OSDConfiguration * p_new = (ONVIF_OSDConfiguration *) malloc(sizeof(ONVIF_OSDConfiguration));
	if (NULL == p_new)
	{
		return NULL;
	}

	memset(p_new, 0, sizeof(ONVIF_OSDConfiguration));

	p_tmp = g_onvif_cfg.OSDs;
	if (NULL == p_tmp)
	{
		g_onvif_cfg.OSDs = p_new;
	}
	else
	{
		while (p_tmp && p_tmp->next) p_tmp = p_tmp->next;

		p_tmp->next = p_new;
	}

	return p_new;
}

ONVIF_OSDConfiguration * onvif_find_OSDConfiguration(const char * token)
{
	ONVIF_OSDConfiguration * p_tmp = g_onvif_cfg.OSDs;
	while (p_tmp)
	{
		if (strcmp(p_tmp->OSD.token, token) == 0)
		{
			break;
		}

		p_tmp = p_tmp->next;
	}

	return p_tmp;
}

ONVIF_MetadataConfiguration * onvif_find_MetadataConfiguration(const char * token)
{
	ONVIF_MetadataConfiguration * p_tmp = g_onvif_cfg.metadata_cfg;
	while (p_tmp)
	{
		if (strcmp(p_tmp->Configuration.token, token) == 0)
		{
			break;
		}

		p_tmp = p_tmp->next;
	}

	return p_tmp;
}

ONVIF_NotificationMessage * onvif_add_NotificationMessage(ONVIF_NotificationMessage ** p_head)
{
	ONVIF_NotificationMessage * p_tmp;
	ONVIF_NotificationMessage * p_new = (ONVIF_NotificationMessage *) malloc(sizeof(ONVIF_NotificationMessage));
	if (NULL == p_new)
	{
		return NULL;
	}

	memset(p_new, 0, sizeof(ONVIF_NotificationMessage));

	if (p_head)
	{
		p_tmp = *p_head;
		if (NULL == p_tmp)
		{
			*p_head = p_new;
		}
		else
		{
			while (p_tmp && p_tmp->next) p_tmp = p_tmp->next;

			p_tmp->next = p_new;
		}	
	}

	p_new->refcnt++;

	return p_new;
}

void onvif_free_NotificationMessage(ONVIF_NotificationMessage * p_message)
{
	if (p_message)
	{
	    p_message->refcnt--;
	    if (p_message->refcnt <= 0)
	    {
	        onvif_free_SimpleItems(&p_message->NotificationMessage.Message.Source.SimpleItem);
    		onvif_free_SimpleItems(&p_message->NotificationMessage.Message.Key.SimpleItem);
    		onvif_free_SimpleItems(&p_message->NotificationMessage.Message.Data.SimpleItem);

    		onvif_free_ElementItems(&p_message->NotificationMessage.Message.Source.ElementItem);
    		onvif_free_ElementItems(&p_message->NotificationMessage.Message.Key.ElementItem);
    		onvif_free_ElementItems(&p_message->NotificationMessage.Message.Data.ElementItem);

    		free(p_message);
	    }
	}
}

void onvif_free_NotificationMessages(ONVIF_NotificationMessage ** p_head)
{
	ONVIF_NotificationMessage * p_next;
	ONVIF_NotificationMessage * p_tmp = *p_head;

	while (p_tmp)
	{
		p_next = p_tmp->next;

		onvif_free_NotificationMessage(p_tmp);
		
		p_tmp = p_next;
	}

	*p_head = NULL;
}

ONVIF_SimpleItem * onvif_add_SimpleItem(ONVIF_SimpleItem ** p_head)
{
	ONVIF_SimpleItem * p_tmp;
	ONVIF_SimpleItem * p_new = (ONVIF_SimpleItem *) malloc(sizeof(ONVIF_SimpleItem));
	if (NULL == p_new)
	{
		return NULL;
	}

	memset(p_new, 0, sizeof(ONVIF_SimpleItem));

	p_tmp = *p_head;
	if (NULL == p_tmp)
	{
		*p_head = p_new;
	}
	else
	{
		while (p_tmp && p_tmp->next) p_tmp = p_tmp->next;

		p_tmp->next = p_new;
	}	

	return p_new;
}

void onvif_free_SimpleItems(ONVIF_SimpleItem ** p_head)
{
    ONVIF_SimpleItem * p_next;
	ONVIF_SimpleItem * p_tmp = *p_head;

	while (p_tmp)
	{
		p_next = p_tmp->next;
		
		free(p_tmp);
		p_tmp = p_next;
	}

	*p_head = NULL;
}

ONVIF_ElementItem * onvif_add_ElementItem(ONVIF_ElementItem ** p_head)
{
	ONVIF_ElementItem * p_tmp;
	ONVIF_ElementItem * p_new = (ONVIF_ElementItem *) malloc(sizeof(ONVIF_ElementItem));
	if (NULL == p_new)
	{
		return NULL;
	}

	memset(p_new, 0, sizeof(ONVIF_ElementItem));

	p_tmp = *p_head;
	if (NULL == p_tmp)
	{
		*p_head = p_new;
	}
	else
	{
		while (p_tmp && p_tmp->next) p_tmp = p_tmp->next;

		p_tmp->next = p_new;
	}	

	return p_new;
}

void onvif_free_ElementItems(ONVIF_ElementItem ** p_head)
{
    ONVIF_ElementItem * p_next;
	ONVIF_ElementItem * p_tmp = *p_head;

	while (p_tmp)
	{
		p_next = p_tmp->next;

		if (p_tmp->ElementItem.Any)
		{
		    free(p_tmp->ElementItem.Any);
		}
		
		free(p_tmp);
		p_tmp = p_next;
	}

	*p_head = NULL;
}

ONVIF_VideoEncoder2ConfigurationOptions * onvif_add_VideoEncoder2ConfigurationOptions(ONVIF_VideoEncoder2ConfigurationOptions ** p_head)
{
    ONVIF_VideoEncoder2ConfigurationOptions * p_tmp;
	ONVIF_VideoEncoder2ConfigurationOptions * p_new = (ONVIF_VideoEncoder2ConfigurationOptions *) malloc(sizeof(ONVIF_VideoEncoder2ConfigurationOptions));
	if (NULL == p_new)
	{
		return NULL;
	}

	memset(p_new, 0, sizeof(ONVIF_VideoEncoder2ConfigurationOptions));

	p_tmp = *p_head;
	if (NULL == p_tmp)
	{
		*p_head = p_new;
	}
	else
	{
		while (p_tmp && p_tmp->next) p_tmp = p_tmp->next;

		p_tmp->next = p_new;
	}	

	return p_new;
}

ONVIF_VideoEncoder2ConfigurationOptions * onvif_find_VideoEncoder2ConfigurationOptions(const char * Encoding)
{
    ONVIF_VideoEncoder2ConfigurationOptions * p_tmp = g_onvif_cfg.v_enc_cfg_opt;
	while (p_tmp)
	{
		if (strcmp(p_tmp->Options.Encoding, Encoding) == 0)
		{
			break;
		}

		p_tmp = p_tmp->next;
	}

	return p_tmp;
}

void onvif_init_ImagingSettings()
{
	// init image setting	
	// note : Optional field flag is set to 0, this option will not appear

	g_onvif_cfg.ImagingSettings.BacklightCompensationFlag = 1;
	g_onvif_cfg.ImagingSettings.BacklightCompensation.Mode = BacklightCompensationMode_OFF;
	g_onvif_cfg.ImagingSettings.BacklightCompensation.LevelFlag = 1;
	g_onvif_cfg.ImagingSettings.BacklightCompensation.Level = 10;
	
	g_onvif_cfg.ImagingSettings.BrightnessFlag = 1;
	// g_onvif_cfg.ImagingSettings.Brightness = 50;
	
	g_onvif_cfg.ImagingSettings.ColorSaturationFlag = 1;
	// g_onvif_cfg.ImagingSettings.ColorSaturation = 50;
	
	g_onvif_cfg.ImagingSettings.ContrastFlag = 1;
	// g_onvif_cfg.ImagingSettings.Contrast = 50;
	
	g_onvif_cfg.ImagingSettings.ExposureFlag = 1;
	g_onvif_cfg.ImagingSettings.Exposure.Mode = ExposureMode_AUTO;
	
	g_onvif_cfg.ImagingSettings.Exposure.PriorityFlag = 1;
	g_onvif_cfg.ImagingSettings.Exposure.Priority = ExposurePriority_LowNoise;

	g_onvif_cfg.ImagingSettings.Exposure.Window.bottom = 1;
	g_onvif_cfg.ImagingSettings.Exposure.Window.top = 0;
	g_onvif_cfg.ImagingSettings.Exposure.Window.right = 1;
	g_onvif_cfg.ImagingSettings.Exposure.Window.left = 0;
	
	g_onvif_cfg.ImagingSettings.Exposure.MinExposureTimeFlag = 1;
	g_onvif_cfg.ImagingSettings.Exposure.MinExposureTime = 10;
	
	g_onvif_cfg.ImagingSettings.Exposure.MaxExposureTimeFlag = 1;
	g_onvif_cfg.ImagingSettings.Exposure.MaxExposureTime = 40000;
	
	g_onvif_cfg.ImagingSettings.Exposure.MinGainFlag = 1;
	g_onvif_cfg.ImagingSettings.Exposure.MinGain = 0;
	
	g_onvif_cfg.ImagingSettings.Exposure.MaxGainFlag = 1;
	g_onvif_cfg.ImagingSettings.Exposure.MaxGain = 100;
	
	g_onvif_cfg.ImagingSettings.Exposure.MinIrisFlag = 1;
	g_onvif_cfg.ImagingSettings.Exposure.MinIris = 0;
	
	g_onvif_cfg.ImagingSettings.Exposure.MaxIrisFlag = 1;
	g_onvif_cfg.ImagingSettings.Exposure.MaxIris = 10;
	
	g_onvif_cfg.ImagingSettings.Exposure.ExposureTimeFlag = 1;
    g_onvif_cfg.ImagingSettings.Exposure.ExposureTime = 4000;
    
    g_onvif_cfg.ImagingSettings.Exposure.GainFlag = 1;
	g_onvif_cfg.ImagingSettings.Exposure.Gain = 100;
	
	g_onvif_cfg.ImagingSettings.Exposure.IrisFlag = 1;
	g_onvif_cfg.ImagingSettings.Exposure.Iris = 10;
	
	g_onvif_cfg.ImagingSettings.FocusFlag = 1;
	g_onvif_cfg.ImagingSettings.Focus.AutoFocusMode = AutoFocusMode_AUTO;
	
	g_onvif_cfg.ImagingSettings.Focus.DefaultSpeedFlag = 1;
	g_onvif_cfg.ImagingSettings.Focus.DefaultSpeed = 100;
	
	g_onvif_cfg.ImagingSettings.Focus.NearLimitFlag = 1;
	g_onvif_cfg.ImagingSettings.Focus.NearLimit = 100;
	
	g_onvif_cfg.ImagingSettings.Focus.FarLimitFlag = 1;
	g_onvif_cfg.ImagingSettings.Focus.FarLimit = 1000;
	
	g_onvif_cfg.ImagingSettings.IrCutFilterFlag = 1;
	g_onvif_cfg.ImagingSettings.IrCutFilter = IrCutFilterMode_AUTO;
	
	g_onvif_cfg.ImagingSettings.SharpnessFlag = 1;
	// g_onvif_cfg.ImagingSettings.Sharpness = 50;
	
	g_onvif_cfg.ImagingSettings.WideDynamicRangeFlag = 1;
	g_onvif_cfg.ImagingSettings.WideDynamicRange.Mode = WideDynamicMode_OFF;
	
	g_onvif_cfg.ImagingSettings.WideDynamicRange.LevelFlag = 1;
	g_onvif_cfg.ImagingSettings.WideDynamicRange.Level = 50;
	
	g_onvif_cfg.ImagingSettings.WhiteBalanceFlag = 1;
	g_onvif_cfg.ImagingSettings.WhiteBalance.Mode = WhiteBalanceMode_AUTO;
	
	g_onvif_cfg.ImagingSettings.WhiteBalance.CbGainFlag = 1;
	g_onvif_cfg.ImagingSettings.WhiteBalance.CbGain = 10;
	
	g_onvif_cfg.ImagingSettings.WhiteBalance.CrGainFlag = 1;
	g_onvif_cfg.ImagingSettings.WhiteBalance.CrGain = 10;


	//  add xieqingpu
	ImgParam_t imgParam;
	memset(&imgParam, 0, sizeof(ImgParam_t));

	if (getImgParam(&imgParam) != 0)
		printf("get img param.\n");
	g_onvif_cfg.ImagingSettings.Brightness = imgParam.brightness;
	g_onvif_cfg.ImagingSettings.Contrast = imgParam.contrast;
	g_onvif_cfg.ImagingSettings.ColorSaturation = imgParam.saturation;
	g_onvif_cfg.ImagingSettings.Sharpness = imgParam.sharp;

	//// 扩展 add xieqingpu
	g_onvif_cfg.ImagingSettings.ThermalSettings_extFlag = 1;
	if (g_onvif_cfg.ImagingSettings.ThermalSettings_extFlag)
	{
		//热成像第一个设置
		ThermalParam1_t thermalParam1;
		memset(&thermalParam1, 0, sizeof(ThermalParam1_t));

		if (getThermalParam1(&thermalParam1) != 0)
			printf("get thermal paeam1 faile.\n");
		g_onvif_cfg.ImagingSettings.ThermalSettings.ThermalSet_ext1Flag = 1;
		g_onvif_cfg.ImagingSettings.ThermalSettings.ThermalSet1.UserPalette = thermalParam1.userPalette;
		g_onvif_cfg.ImagingSettings.ThermalSettings.ThermalSet1.WideDynamic = thermalParam1.wideDynamic;
		g_onvif_cfg.ImagingSettings.ThermalSettings.ThermalSet1.OrgData = thermalParam1.orgData;
		g_onvif_cfg.ImagingSettings.ThermalSettings.ThermalSet1.Actime = thermalParam1.actime;

		//热成像第二个设置
		ThermalParam2_t thermalParam2;
		memset(&thermalParam2, 0, sizeof(ThermalParam2_t));

		if ( getThermalParam2(&thermalParam2) != 0)
			printf("get thermal param2 faile.\n");
		g_onvif_cfg.ImagingSettings.ThermalSettings.ThermalSet_ext2Flag = 1;
		g_onvif_cfg.ImagingSettings.ThermalSettings.ThermalSet2.Emissivity = thermalParam2.emissivity;
		g_onvif_cfg.ImagingSettings.ThermalSettings.ThermalSet2.Distance = thermalParam2.distance;
		g_onvif_cfg.ImagingSettings.ThermalSettings.ThermalSet2.Humidity = thermalParam2.humidity;
		g_onvif_cfg.ImagingSettings.ThermalSettings.ThermalSet2.Correction = thermalParam2.correction;
		g_onvif_cfg.ImagingSettings.ThermalSettings.ThermalSet2.Reflection = thermalParam2.reflection;
		g_onvif_cfg.ImagingSettings.ThermalSettings.ThermalSet2.Amb = thermalParam2.amb;

		//
		DulaInformation_t dulaInfomation;
		memset(&dulaInfomation, 0, sizeof(DulaInformation_t));

		if (getDulaParam(&dulaInfomation) != 0)
			printf("get dula faile.\n");
		g_onvif_cfg.ImagingSettings.DulaInformationFlag = 1;
		g_onvif_cfg.ImagingSettings.DulaInfo.focal = dulaInfomation.focal;
		g_onvif_cfg.ImagingSettings.DulaInfo.lens = dulaInfomation.lens;
		g_onvif_cfg.ImagingSettings.DulaInfo.distance = dulaInfomation.distance;
		g_onvif_cfg.ImagingSettings.DulaInfo.dula_model = dulaInfomation.dula_model;
		g_onvif_cfg.ImagingSettings.DulaInfo.x = dulaInfomation.x;
		g_onvif_cfg.ImagingSettings.DulaInfo.y = dulaInfomation.y;
		g_onvif_cfg.ImagingSettings.DulaInfo.scale = dulaInfomation.scale;
	}
	/////
}

void onvif_init_ImagingOptions()
{
	// init image config options
	// note : Optional field flag is set to 0, this option will not appear

	g_onvif_cfg.ImagingOptions.BacklightCompensationFlag = 1;
	g_onvif_cfg.ImagingOptions.BacklightCompensation.Mode_OFF = 1;
	g_onvif_cfg.ImagingOptions.BacklightCompensation.Mode_ON = 1;
	g_onvif_cfg.ImagingOptions.BacklightCompensation.LevelFlag = 1;
	g_onvif_cfg.ImagingOptions.BacklightCompensation.Level.Min = 0;
	g_onvif_cfg.ImagingOptions.BacklightCompensation.Level.Max = 100;
	
	g_onvif_cfg.ImagingOptions.BrightnessFlag = 1;
	g_onvif_cfg.ImagingOptions.Brightness.Min = 0;
	g_onvif_cfg.ImagingOptions.Brightness.Max = 100;
	
	g_onvif_cfg.ImagingOptions.ColorSaturationFlag = 1;
	g_onvif_cfg.ImagingOptions.ColorSaturation.Min = 0;
	g_onvif_cfg.ImagingOptions.ColorSaturation.Max = 100;
	
	g_onvif_cfg.ImagingOptions.ContrastFlag = 1;
	g_onvif_cfg.ImagingOptions.Contrast.Min = 0;
	g_onvif_cfg.ImagingOptions.Contrast.Max = 100;
	
	g_onvif_cfg.ImagingOptions.ExposureFlag = 1;
	g_onvif_cfg.ImagingOptions.Exposure.Mode_AUTO = 1;
	g_onvif_cfg.ImagingOptions.Exposure.Mode_MANUAL = 1;
	g_onvif_cfg.ImagingOptions.Exposure.Priority_LowNoise = 1;
	g_onvif_cfg.ImagingOptions.Exposure.Priority_FrameRate = 1;
	
	g_onvif_cfg.ImagingOptions.Exposure.MinExposureTimeFlag = 1;
	g_onvif_cfg.ImagingOptions.Exposure.MinExposureTime.Min = 10;
	g_onvif_cfg.ImagingOptions.Exposure.MinExposureTime.Max = 10;
	
	g_onvif_cfg.ImagingOptions.Exposure.MaxExposureTimeFlag = 1;
	g_onvif_cfg.ImagingOptions.Exposure.MaxExposureTime.Min = 10;
	g_onvif_cfg.ImagingOptions.Exposure.MaxExposureTime.Max = 320000;

	g_onvif_cfg.ImagingOptions.Exposure.MinGainFlag = 1;
	g_onvif_cfg.ImagingOptions.Exposure.MinGain.Min = 0;
	g_onvif_cfg.ImagingOptions.Exposure.MinGain.Max = 0;

	g_onvif_cfg.ImagingOptions.Exposure.MaxGainFlag = 1;
	g_onvif_cfg.ImagingOptions.Exposure.MaxGain.Min = 0;
	g_onvif_cfg.ImagingOptions.Exposure.MaxGain.Max = 100;

	g_onvif_cfg.ImagingOptions.Exposure.MinIrisFlag = 1;
	g_onvif_cfg.ImagingOptions.Exposure.MinIris.Min = 0;
	g_onvif_cfg.ImagingOptions.Exposure.MinIris.Max = 10;

	g_onvif_cfg.ImagingOptions.Exposure.MaxIrisFlag = 1;
	g_onvif_cfg.ImagingOptions.Exposure.MaxIris.Min = 0;
	g_onvif_cfg.ImagingOptions.Exposure.MaxIris.Max = 10;

	g_onvif_cfg.ImagingOptions.Exposure.ExposureTimeFlag = 1;
	g_onvif_cfg.ImagingOptions.Exposure.ExposureTime.Min = 0;
	g_onvif_cfg.ImagingOptions.Exposure.ExposureTime.Max = 40000;

	g_onvif_cfg.ImagingOptions.Exposure.GainFlag = 1;
	g_onvif_cfg.ImagingOptions.Exposure.Gain.Min = 0;
	g_onvif_cfg.ImagingOptions.Exposure.Gain.Max = 100;

	g_onvif_cfg.ImagingOptions.Exposure.IrisFlag = 1;
	g_onvif_cfg.ImagingOptions.Exposure.Iris.Min = 0;
	g_onvif_cfg.ImagingOptions.Exposure.Iris.Max = 100;

	g_onvif_cfg.ImagingOptions.FocusFlag = 1;
    g_onvif_cfg.ImagingOptions.Focus.AutoFocusModes_AUTO = 1;
    g_onvif_cfg.ImagingOptions.Focus.AutoFocusModes_MANUAL = 1;

    g_onvif_cfg.ImagingOptions.Focus.DefaultSpeedFlag = 1;
	g_onvif_cfg.ImagingOptions.Focus.DefaultSpeed.Min = 0;
	g_onvif_cfg.ImagingOptions.Focus.DefaultSpeed.Max = 100;

	g_onvif_cfg.ImagingOptions.Focus.NearLimitFlag = 1;
	g_onvif_cfg.ImagingOptions.Focus.NearLimit.Min = 0;
	g_onvif_cfg.ImagingOptions.Focus.NearLimit.Max = 100;

	g_onvif_cfg.ImagingOptions.Focus.FarLimitFlag = 1;
	g_onvif_cfg.ImagingOptions.Focus.FarLimit.Min = 0;
	g_onvif_cfg.ImagingOptions.Focus.FarLimit.Max = 1000;	
	
	g_onvif_cfg.ImagingOptions.IrCutFilterMode_ON = 1;
	g_onvif_cfg.ImagingOptions.IrCutFilterMode_OFF = 1;
	g_onvif_cfg.ImagingOptions.IrCutFilterMode_AUTO = 1;

	g_onvif_cfg.ImagingOptions.SharpnessFlag = 1;
	g_onvif_cfg.ImagingOptions.Sharpness.Min = 0;
	g_onvif_cfg.ImagingOptions.Sharpness.Max = 100;

	g_onvif_cfg.ImagingOptions.WideDynamicRangeFlag = 1;	
	g_onvif_cfg.ImagingOptions.WideDynamicRange.Mode_OFF = 1;
	g_onvif_cfg.ImagingOptions.WideDynamicRange.Mode_ON = 1;

	g_onvif_cfg.ImagingOptions.WideDynamicRange.LevelFlag = 1;
	g_onvif_cfg.ImagingOptions.WideDynamicRange.Level.Min = 0;
	g_onvif_cfg.ImagingOptions.WideDynamicRange.Level.Max = 100;

	g_onvif_cfg.ImagingOptions.WhiteBalanceFlag = 1;
	g_onvif_cfg.ImagingOptions.WhiteBalance.Mode_AUTO = 1;
	g_onvif_cfg.ImagingOptions.WhiteBalance.Mode_MANUAL = 1;

	g_onvif_cfg.ImagingOptions.WhiteBalance.YrGainFlag = 1;
    g_onvif_cfg.ImagingOptions.WhiteBalance.YrGain.Min = 0;
	g_onvif_cfg.ImagingOptions.WhiteBalance.YrGain.Max = 100;

	g_onvif_cfg.ImagingOptions.WhiteBalance.YbGainFlag = 1;
	g_onvif_cfg.ImagingOptions.WhiteBalance.YbGain.Min = 0;
	g_onvif_cfg.ImagingOptions.WhiteBalance.YbGain.Max = 100;
}

void onvif_init_xaddr(char * addr, int addrlen, const char * suffix)
{
	int i;
    BOOL first = TRUE;
    int offset = 0;

    for (i = 0; i < g_onvif_cfg.servs_num; i++)
    {
        if (first)
	    {
	        first = FALSE;
	    }
	    else
	    {
	        offset += snprintf(addr+offset, addrlen-offset, " ");
	    }
    	    
#ifdef HTTPS
    	if (g_onvif_cfg.https_enable)
    	{
        	offset += snprintf(addr+offset, addrlen-offset,  
        	    "https://%s:%d%s", g_onvif_cfg.servs[i].serv_ip, g_onvif_cfg.servs[i].serv_port, suffix);
        }
        else
        {
            offset += snprintf(addr+offset, addrlen-offset,  
        	    "http://%s:%d%s", g_onvif_cfg.servs[i].serv_ip, g_onvif_cfg.servs[i].serv_port, suffix);
        }
#else
    	offset += snprintf(addr+offset, addrlen-offset,  
    	    "http://%s:%d%s", g_onvif_cfg.servs[i].serv_ip, g_onvif_cfg.servs[i].serv_port, suffix);
#endif
    }
}

void onvif_init_capabilities()
{
	// network capabilities
	
#ifdef IPFILTER_SUPPORT	
	g_onvif_cfg.Capabilities.device.IPFilter = 1;
#else
    g_onvif_cfg.Capabilities.device.IPFilter = 0;
#endif
	g_onvif_cfg.Capabilities.device.ZeroConfiguration = 1;
	g_onvif_cfg.Capabilities.device.IPVersion6 = 0;
	g_onvif_cfg.Capabilities.device.DynDNS = 1;
	g_onvif_cfg.Capabilities.device.Dot11Configuration = 1;
	g_onvif_cfg.Capabilities.device.HostnameFromDHCP = 1;
	g_onvif_cfg.Capabilities.device.DHCPv6 = 0;

	// system capabilities
	g_onvif_cfg.Capabilities.device.DiscoveryResolve = 1;
	g_onvif_cfg.Capabilities.device.DiscoveryBye = 1;
	g_onvif_cfg.Capabilities.device.RemoteDiscovery = 0;
	g_onvif_cfg.Capabilities.device.SystemBackup = 1;
	g_onvif_cfg.Capabilities.device.SystemLogging = 1;
	g_onvif_cfg.Capabilities.device.FirmwareUpgrade = 1;
	g_onvif_cfg.Capabilities.device.HttpFirmwareUpgrade = 1;
	g_onvif_cfg.Capabilities.device.HttpSystemBackup = 1;
	g_onvif_cfg.Capabilities.device.HttpSystemLogging = 1;
	g_onvif_cfg.Capabilities.device.HttpSupportInformation = 1;
	g_onvif_cfg.Capabilities.device.StorageConfiguration = 0;

    // scurity capabilities
#ifdef HTTPS
	if (g_onvif_cfg.https_enable)
	{
		g_onvif_cfg.Capabilities.device.TLS10 = 1;
    	g_onvif_cfg.Capabilities.device.TLS11 = 1;
    	g_onvif_cfg.Capabilities.device.TLS12 = 1;
    }
    else
    {
    	g_onvif_cfg.Capabilities.device.TLS10 = 0;
    	g_onvif_cfg.Capabilities.device.TLS11 = 0;
    	g_onvif_cfg.Capabilities.device.TLS12 = 0;
    }
#else
	g_onvif_cfg.Capabilities.device.TLS10 = 0;
	g_onvif_cfg.Capabilities.device.TLS11 = 0;
	g_onvif_cfg.Capabilities.device.TLS12 = 0;
#endif
    g_onvif_cfg.Capabilities.device.OnboardKeyGeneration = 0;
    g_onvif_cfg.Capabilities.device.AccessPolicyConfig = 1;
    g_onvif_cfg.Capabilities.device.DefaultAccessPolicy = 1;
    g_onvif_cfg.Capabilities.device.Dot1X = 1;
    g_onvif_cfg.Capabilities.device.RemoteUserHandling = 1;
    g_onvif_cfg.Capabilities.device.X509Token = 0;
    g_onvif_cfg.Capabilities.device.SAMLToken = 0;
    g_onvif_cfg.Capabilities.device.KerberosToken = 0;
    g_onvif_cfg.Capabilities.device.UsernameToken = 1;
    g_onvif_cfg.Capabilities.device.HttpDigest = 1;
    g_onvif_cfg.Capabilities.device.RELToken = 0;

    g_onvif_cfg.Capabilities.device.Dot1XConfigurations = 1;
	g_onvif_cfg.Capabilities.device.NTP = MAX_NTP_SERVER;
	g_onvif_cfg.Capabilities.device.SupportedEAPMethods = 0;
	g_onvif_cfg.Capabilities.device.MaxUsers = MAX_USERS;
	g_onvif_cfg.Capabilities.device.MaxUserNameLength = 32;
	g_onvif_cfg.Capabilities.device.MaxPasswordLength = 32;

#ifdef DEVICEIO_SUPPORT
    g_onvif_cfg.Capabilities.device.InputConnectors = 0;
    g_onvif_cfg.Capabilities.device.RelayOutputs = 1;
#endif

    onvif_init_xaddr(g_onvif_cfg.Capabilities.device.XAddr, XADDR_LEN-1, "/onvif/device_service");

	// media capabilities
    g_onvif_cfg.Capabilities.media.SnapshotUri = 1;
    g_onvif_cfg.Capabilities.media.Rotation = 0;
    g_onvif_cfg.Capabilities.media.VideoSourceMode = 1;
    g_onvif_cfg.Capabilities.media.OSD = 1;
    g_onvif_cfg.Capabilities.media.EXICompression = 0;
	g_onvif_cfg.Capabilities.media.RTPMulticast = 0;
	g_onvif_cfg.Capabilities.media.RTP_TCP = 1;
	g_onvif_cfg.Capabilities.media.RTP_RTSP_TCP = 1;
	g_onvif_cfg.Capabilities.media.NonAggregateControl = 0;
	g_onvif_cfg.Capabilities.media.NoRTSPStreaming = 0;
	g_onvif_cfg.Capabilities.media.support = 1;

	g_onvif_cfg.Capabilities.media.MaximumNumberOfProfiles = 10;

    onvif_init_xaddr(g_onvif_cfg.Capabilities.media.XAddr, XADDR_LEN-1, "/onvif/media_service");

#ifdef MEDIA2_SUPPORT
    // media capabilities
    g_onvif_cfg.Capabilities.media2.StreamingCapabilities.RTP_USCORERTSP_USCORETCPFlag = 1;
    g_onvif_cfg.Capabilities.media2.StreamingCapabilities.RTP_USCORERTSP_USCORETCP = 1;
    g_onvif_cfg.Capabilities.media2.StreamingCapabilities.RTSPStreamingFlag = 1;
    g_onvif_cfg.Capabilities.media2.StreamingCapabilities.RTSPStreaming = 1;
    g_onvif_cfg.Capabilities.media2.StreamingCapabilities.RTPMulticastFlag = 1;
    g_onvif_cfg.Capabilities.media2.StreamingCapabilities.RTPMulticast = 1;
    g_onvif_cfg.Capabilities.media2.StreamingCapabilities.AutoStartMulticast = 0;
    g_onvif_cfg.Capabilities.media2.SnapshotUriFlag = 1;
    g_onvif_cfg.Capabilities.media2.SnapshotUri = 1;
    g_onvif_cfg.Capabilities.media2.Rotation = 0;
    g_onvif_cfg.Capabilities.media2.VideoSourceMode = 1;
    g_onvif_cfg.Capabilities.media2.OSDFlag = 1;
    g_onvif_cfg.Capabilities.media2.OSD = 1;
    g_onvif_cfg.Capabilities.media2.TemporaryOSDText = 1;
    g_onvif_cfg.Capabilities.media2.Mask = 1;
    g_onvif_cfg.Capabilities.media2.SourceMask = 1;
    g_onvif_cfg.Capabilities.media2.ProfileCapabilities.MaximumNumberOfProfilesFlag = 1;
    g_onvif_cfg.Capabilities.media2.ProfileCapabilities.MaximumNumberOfProfiles = 10;
    g_onvif_cfg.Capabilities.media2.ProfileCapabilities.ConfigurationsSupportedFlag = 1;

    strcpy(g_onvif_cfg.Capabilities.media2.ProfileCapabilities.ConfigurationsSupported, "VideoSource VideoEncoder Metadata");

#ifdef PTZ_SUPPORT
    strcat(g_onvif_cfg.Capabilities.media2.ProfileCapabilities.ConfigurationsSupported, " PTZ");
#endif
#ifdef AUDIO_SUPPORT
    strcat(g_onvif_cfg.Capabilities.media2.ProfileCapabilities.ConfigurationsSupported, " AudioSource AudioEncoder");
#endif
#ifdef DEVICEIO_SUPPORT
    strcat(g_onvif_cfg.Capabilities.media2.ProfileCapabilities.ConfigurationsSupported, " AudioOutput AudioDecoder");
#endif
#ifdef VIDEO_ANALYTICS
    strcat(g_onvif_cfg.Capabilities.media2.ProfileCapabilities.ConfigurationsSupported, " Analytics");
#endif

	g_onvif_cfg.Capabilities.media2.support = 1;

    onvif_init_xaddr(g_onvif_cfg.Capabilities.media2.XAddr, XADDR_LEN-1, "/onvif/media2_service");

#endif // end of MEDIA2_SUPPORT

	// event capabilities
    g_onvif_cfg.Capabilities.events.WSSubscriptionPolicySupport = 1;
	g_onvif_cfg.Capabilities.events.WSPullPointSupport = 1;
	g_onvif_cfg.Capabilities.events.WSPausableSubscriptionManagerInterfaceSupport = 0;
	g_onvif_cfg.Capabilities.events.PersistentNotificationStorage = 0;
	g_onvif_cfg.Capabilities.events.support = 1;

	g_onvif_cfg.Capabilities.events.MaxNotificationProducers = 10;
	g_onvif_cfg.Capabilities.events.MaxPullPoints = 10;

    onvif_init_xaddr(g_onvif_cfg.Capabilities.events.XAddr, XADDR_LEN-1, "/onvif/event_service");

	// image capabilities
    g_onvif_cfg.Capabilities.image.ImageStabilization = 0;
    g_onvif_cfg.Capabilities.image.Presets = 0;
	g_onvif_cfg.Capabilities.image.support = 1;

    onvif_init_xaddr(g_onvif_cfg.Capabilities.image.XAddr, XADDR_LEN-1, "/onvif/image_service");

#ifdef PTZ_SUPPORT
	// ptz capabilities
	g_onvif_cfg.Capabilities.ptz.EFlip = 1;
    g_onvif_cfg.Capabilities.ptz.Reverse = 1;
    g_onvif_cfg.Capabilities.ptz.GetCompatibleConfigurations = 1;
    g_onvif_cfg.Capabilities.ptz.MoveStatus = 1;
    g_onvif_cfg.Capabilities.ptz.StatusPosition = 1;
    g_onvif_cfg.Capabilities.ptz.support = 1;

    onvif_init_xaddr(g_onvif_cfg.Capabilities.ptz.XAddr, XADDR_LEN-1, "/onvif/ptz_service");

#endif // end of PTZ_SUPPORT

#ifdef VIDEO_ANALYTICS
	// analytics capabilities
	g_onvif_cfg.Capabilities.analytics.RuleSupport = 1;
	g_onvif_cfg.Capabilities.analytics.AnalyticsModuleSupport = 1;
	g_onvif_cfg.Capabilities.analytics.CellBasedSceneDescriptionSupported = 0;
	g_onvif_cfg.Capabilities.analytics.support = 1;

    onvif_init_xaddr(g_onvif_cfg.Capabilities.analytics.XAddr, XADDR_LEN-1, "/onvif/analytics_service");

#endif // end of VIDEO_ANALYTICS

#ifdef PROFILE_G_SUPPORT
	// record capabilities
    g_onvif_cfg.Capabilities.recording.ReceiverSource = 0;
    g_onvif_cfg.Capabilities.recording.MediaProfileSource = 1;
    g_onvif_cfg.Capabilities.recording.DynamicRecordings = 1;
    g_onvif_cfg.Capabilities.recording.DynamicTracks = 1;
    g_onvif_cfg.Capabilities.recording.Options = 1;
    g_onvif_cfg.Capabilities.recording.MetadataRecording = 1;
    g_onvif_cfg.Capabilities.recording.JPEG = 1;
    g_onvif_cfg.Capabilities.recording.MPEG4 = 1;
    g_onvif_cfg.Capabilities.recording.H264 = 1;
#ifdef AUDIO_SUPPORT    
    g_onvif_cfg.Capabilities.recording.G711 = 1;
    g_onvif_cfg.Capabilities.recording.G726 = 1;
    g_onvif_cfg.Capabilities.recording.AAC = 1;
#else
    g_onvif_cfg.Capabilities.recording.G711 = 0;
    g_onvif_cfg.Capabilities.recording.G726 = 0;
    g_onvif_cfg.Capabilities.recording.AAC = 0;
#endif
    g_onvif_cfg.Capabilities.recording.support = 1;

	g_onvif_cfg.Capabilities.recording.MaxStringLength = 256;
	g_onvif_cfg.Capabilities.recording.MaxRate = 200;
	g_onvif_cfg.Capabilities.recording.MaxTotalRate = 2000;
    g_onvif_cfg.Capabilities.recording.MaxRecordings = 5;
    g_onvif_cfg.Capabilities.recording.MaxRecordingJobs = 5;

    onvif_init_xaddr(g_onvif_cfg.Capabilities.recording.XAddr, XADDR_LEN-1, "/onvif/recording_service");

	// search capabilities
	g_onvif_cfg.Capabilities.search.MetadataSearch = 0;
    g_onvif_cfg.Capabilities.search.GeneralStartEvents = 0;
    g_onvif_cfg.Capabilities.search.support = 1;

    onvif_init_xaddr(g_onvif_cfg.Capabilities.search.XAddr, XADDR_LEN-1, "/onvif/search_service");

	// replay capabilities
	g_onvif_cfg.Capabilities.replay.ReversePlayback = 0;
    g_onvif_cfg.Capabilities.replay.RTP_RTSP_TCP = 1;
    g_onvif_cfg.Capabilities.replay.support = 1;

	g_onvif_cfg.Capabilities.replay.SessionTimeoutRange.Min = 10;
	g_onvif_cfg.Capabilities.replay.SessionTimeoutRange.Max = 100;

    onvif_init_xaddr(g_onvif_cfg.Capabilities.replay.XAddr, XADDR_LEN-1, "/onvif/replay_service");

#endif // end of PROFILE_G_SUPPORT

#ifdef PROFILE_C_SUPPORT
    // accesscontrol capabilities
    g_onvif_cfg.Capabilities.accesscontrol.support = 1;
    g_onvif_cfg.Capabilities.accesscontrol.MaxLimit = ACCESS_CTRL_MAX_LIMIT;
    g_onvif_cfg.Capabilities.accesscontrol.MaxAccessPoints = ACCESS_CTRL_MAX_LIMIT;
    g_onvif_cfg.Capabilities.accesscontrol.MaxAreas = ACCESS_CTRL_MAX_LIMIT;
    g_onvif_cfg.Capabilities.accesscontrol.ClientSuppliedTokenSupported = 0;

    onvif_init_xaddr(g_onvif_cfg.Capabilities.accesscontrol.XAddr, XADDR_LEN-1, "/onvif/accesscontrol_service");

    // doorcontrol capabilities
    g_onvif_cfg.Capabilities.doorcontrol.support = 1;
    g_onvif_cfg.Capabilities.doorcontrol.MaxLimit = DOOR_CTRL_MAX_LIMIT;
    g_onvif_cfg.Capabilities.doorcontrol.MaxDoors = DOOR_CTRL_MAX_LIMIT;
    g_onvif_cfg.Capabilities.doorcontrol.ClientSuppliedTokenSupported = 0;

    onvif_init_xaddr(g_onvif_cfg.Capabilities.doorcontrol.XAddr, XADDR_LEN-1, "/onvif/doorcontrol_service");

#endif // end of PROFILE_C_SUPPORT

#ifdef DEVICEIO_SUPPORT
    // deviceIO capabilities
    g_onvif_cfg.Capabilities.deviceIO.support = 1;
    g_onvif_cfg.Capabilities.deviceIO.VideoSourcesFlag = 1;
    g_onvif_cfg.Capabilities.deviceIO.VideoSources = 1;
    g_onvif_cfg.Capabilities.deviceIO.VideoOutputsFlag = 0;
#ifdef AUDIO_SUPPORT    
    g_onvif_cfg.Capabilities.deviceIO.AudioSourcesFlag = 1;
    g_onvif_cfg.Capabilities.deviceIO.AudioSources = 1;
    g_onvif_cfg.Capabilities.deviceIO.AudioOutputsFlag = 1;
    g_onvif_cfg.Capabilities.deviceIO.AudioOutputs = 1;
#endif    
    g_onvif_cfg.Capabilities.deviceIO.RelayOutputsFlag = 1;
    g_onvif_cfg.Capabilities.deviceIO.RelayOutputs = 1;
    g_onvif_cfg.Capabilities.deviceIO.SerialPortsFlag = 1;
    g_onvif_cfg.Capabilities.deviceIO.SerialPorts = 1;
    g_onvif_cfg.Capabilities.deviceIO.DigitalInputsFlag = 1;
    g_onvif_cfg.Capabilities.deviceIO.DigitalInputs = 1;
    g_onvif_cfg.Capabilities.deviceIO.DigitalInputOptionsFlag = 1;
    g_onvif_cfg.Capabilities.deviceIO.DigitalInputOptions = TRUE;

    onvif_init_xaddr(g_onvif_cfg.Capabilities.deviceIO.XAddr, XADDR_LEN-1, "/onvif/deviceIO_service");

#endif // end of DEVICEIO_SUPPORT

    // dot11 capabilities
    g_onvif_cfg.Capabilities.dot11.TKIP = 1;
    g_onvif_cfg.Capabilities.dot11.ScanAvailableNetworks = 1;
    g_onvif_cfg.Capabilities.dot11.MultipleConfiguration = 0;
    g_onvif_cfg.Capabilities.dot11.AdHocStationMode = 1;
    g_onvif_cfg.Capabilities.dot11.WEP = 1;

#ifdef THERMAL_SUPPORT
    // thermal capabilities
    g_onvif_cfg.Capabilities.thermal.support = 1;
    g_onvif_cfg.Capabilities.thermal.Radiometry = 1;

    onvif_init_xaddr(g_onvif_cfg.Capabilities.thermal.XAddr, XADDR_LEN-1, "/onvif/thermal_service");
#endif // end of THERMAL_SUPPORT

#ifdef CREDENTIAL_SUPPORT
    // credential capabilities
    g_onvif_cfg.Capabilities.credential.support = 1;
    g_onvif_cfg.Capabilities.credential.CredentialValiditySupported = 1;
    g_onvif_cfg.Capabilities.credential.CredentialAccessProfileValiditySupported = 1;
    g_onvif_cfg.Capabilities.credential.ValiditySupportsTimeValue = 1;
    g_onvif_cfg.Capabilities.credential.ResetAntipassbackSupported = 1;
    g_onvif_cfg.Capabilities.credential.ClientSuppliedTokenSupported = 0;

    g_onvif_cfg.Capabilities.credential.MaxLimit = CREDENTIAL_MAX_LIMIT;
    g_onvif_cfg.Capabilities.credential.MaxCredentials = CREDENTIAL_MAX_LIMIT;
    g_onvif_cfg.Capabilities.credential.MaxAccessProfilesPerCredential = 1;

    g_onvif_cfg.Capabilities.credential.sizeSupportedIdentifierType = 3;
    strcpy(g_onvif_cfg.Capabilities.credential.SupportedIdentifierType[0], "pt:Card");
    strcpy(g_onvif_cfg.Capabilities.credential.SupportedIdentifierType[1], "pt:PIN");
    strcpy(g_onvif_cfg.Capabilities.credential.SupportedIdentifierType[2], "pt:Fingerprint");

    strcpy(g_onvif_cfg.Capabilities.credential.DefaultCredentialSuspensionDuration, "PT5M");

    g_onvif_cfg.Capabilities.credential.ExtensionFlag = 1;
    g_onvif_cfg.Capabilities.credential.Extension.sizeSupportedExemptionType = 0;
    
    onvif_init_xaddr(g_onvif_cfg.Capabilities.credential.XAddr, XADDR_LEN-1, "/onvif/credential_service");
#endif // end of CREDENTIAL_SUPPORT

#ifdef ACCESS_RULES
    // access rules capabilities
    g_onvif_cfg.Capabilities.accessrules.support = 1;
    g_onvif_cfg.Capabilities.accessrules.MaxLimit = ACCESSRULES_MAX_LIMIT;
    g_onvif_cfg.Capabilities.accessrules.MaxAccessProfiles = ACCESSRULES_MAX_LIMIT;
    g_onvif_cfg.Capabilities.accessrules.MaxAccessPoliciesPerAccessProfile = 1;
    g_onvif_cfg.Capabilities.accessrules.MultipleSchedulesPerAccessPointSupported = 1;
    g_onvif_cfg.Capabilities.accessrules.ClientSuppliedTokenSupported = 0;

    onvif_init_xaddr(g_onvif_cfg.Capabilities.accessrules.XAddr, XADDR_LEN-1, "/onvif/accessrules_service");
#endif // end of ACCESS_RULES

#ifdef SCHEDULE_SUPPORT
    g_onvif_cfg.Capabilities.schedule.support = 1;
    g_onvif_cfg.Capabilities.schedule.MaxLimit = SCHEDULE_MAX_LIMIT;
    g_onvif_cfg.Capabilities.schedule.MaxSchedules = SCHEDULE_MAX_LIMIT;
    g_onvif_cfg.Capabilities.schedule.MaxTimePeriodsPerDay = SCHEDULE_MAX_LIMIT;
    g_onvif_cfg.Capabilities.schedule.MaxSpecialDayGroups = SCHEDULE_MAX_LIMIT;
    g_onvif_cfg.Capabilities.schedule.MaxDaysInSpecialDayGroup = SCHEDULE_MAX_LIMIT;
    g_onvif_cfg.Capabilities.schedule.MaxSpecialDaysSchedules = SCHEDULE_MAX_LIMIT;
    g_onvif_cfg.Capabilities.schedule.ExtendedRecurrenceSupported = 1;
    g_onvif_cfg.Capabilities.schedule.SpecialDaysSupported = 1;
    g_onvif_cfg.Capabilities.schedule.StateReportingSupported = 0;
    g_onvif_cfg.Capabilities.schedule.ClientSuppliedTokenSupported = 0;

    onvif_init_xaddr(g_onvif_cfg.Capabilities.schedule.XAddr, XADDR_LEN-1, "/onvif/schedule_service");
#endif // end of SCHEDULE_SUPPORT

#ifdef RECEIVER_SUPPORT
    g_onvif_cfg.Capabilities.receiver.support = 1;
    g_onvif_cfg.Capabilities.receiver.RTP_USCOREMulticast = 1;
    g_onvif_cfg.Capabilities.receiver.RTP_USCORETCP = 1;
    g_onvif_cfg.Capabilities.receiver.RTP_USCORERTSP_USCORETCP = 1;

    g_onvif_cfg.Capabilities.receiver.SupportedReceivers = 10;
    g_onvif_cfg.Capabilities.receiver.MaximumRTSPURILength = 256;

    onvif_init_xaddr(g_onvif_cfg.Capabilities.receiver.XAddr, XADDR_LEN-1, "/onvif/receiver_service");
#endif // end of RECEIVER_SUPPORT
}

void onvif_init_VideoEncoderConfigurationOptions()
{
#ifdef MEDIA2_SUPPORT
    ONVIF_VideoEncoder2ConfigurationOptions * p_option;

    p_option = onvif_add_VideoEncoder2ConfigurationOptions(&g_onvif_cfg.v_enc_cfg_opt);

    onvif_init_VideoEncoder2ConfigurationOptions(&p_option->Options, "JPEG");

    p_option = onvif_add_VideoEncoder2ConfigurationOptions(&g_onvif_cfg.v_enc_cfg_opt);

    onvif_init_VideoEncoder2ConfigurationOptions(&p_option->Options, "MPV4-ES");

    p_option = onvif_add_VideoEncoder2ConfigurationOptions(&g_onvif_cfg.v_enc_cfg_opt);

    onvif_init_VideoEncoder2ConfigurationOptions(&p_option->Options, "H264");

    p_option = onvif_add_VideoEncoder2ConfigurationOptions(&g_onvif_cfg.v_enc_cfg_opt);

    onvif_init_VideoEncoder2ConfigurationOptions(&p_option->Options, "H265");
#endif

	// video encoder config options
	g_onvif_cfg.VideoEncoderConfigurationOptions.JPEGFlag = 1;
	g_onvif_cfg.VideoEncoderConfigurationOptions.MPEG4Flag = 1;
	g_onvif_cfg.VideoEncoderConfigurationOptions.H264Flag = 1;
	g_onvif_cfg.VideoEncoderConfigurationOptions.QualityRange.Min = 0;
	g_onvif_cfg.VideoEncoderConfigurationOptions.QualityRange.Max = 100;	

	// jpeg config options
	g_onvif_cfg.VideoEncoderConfigurationOptions.JPEG.ResolutionsAvailable[0].Width = 1920;
	g_onvif_cfg.VideoEncoderConfigurationOptions.JPEG.ResolutionsAvailable[0].Height = 1080;
	g_onvif_cfg.VideoEncoderConfigurationOptions.JPEG.ResolutionsAvailable[1].Width = 1280;
	g_onvif_cfg.VideoEncoderConfigurationOptions.JPEG.ResolutionsAvailable[1].Height = 720;
	g_onvif_cfg.VideoEncoderConfigurationOptions.JPEG.ResolutionsAvailable[2].Width = 640;
	g_onvif_cfg.VideoEncoderConfigurationOptions.JPEG.ResolutionsAvailable[2].Height = 480;
	g_onvif_cfg.VideoEncoderConfigurationOptions.JPEG.ResolutionsAvailable[3].Width = 352;
	g_onvif_cfg.VideoEncoderConfigurationOptions.JPEG.ResolutionsAvailable[3].Height = 288;
	g_onvif_cfg.VideoEncoderConfigurationOptions.JPEG.ResolutionsAvailable[4].Width = 320;
	g_onvif_cfg.VideoEncoderConfigurationOptions.JPEG.ResolutionsAvailable[4].Height = 240;
	g_onvif_cfg.VideoEncoderConfigurationOptions.JPEG.FrameRateRange.Min = 1;
	g_onvif_cfg.VideoEncoderConfigurationOptions.JPEG.FrameRateRange.Max = 30;
	g_onvif_cfg.VideoEncoderConfigurationOptions.JPEG.EncodingIntervalRange.Min = 5;
	g_onvif_cfg.VideoEncoderConfigurationOptions.JPEG.EncodingIntervalRange.Max = 60;

	// mpeg4 config options
	g_onvif_cfg.VideoEncoderConfigurationOptions.MPEG4.ResolutionsAvailable[0].Width = 1920;
	g_onvif_cfg.VideoEncoderConfigurationOptions.MPEG4.ResolutionsAvailable[0].Height = 1080;
	g_onvif_cfg.VideoEncoderConfigurationOptions.MPEG4.ResolutionsAvailable[1].Width = 1280;
	g_onvif_cfg.VideoEncoderConfigurationOptions.MPEG4.ResolutionsAvailable[1].Height = 720;
	g_onvif_cfg.VideoEncoderConfigurationOptions.MPEG4.ResolutionsAvailable[2].Width = 640;
	g_onvif_cfg.VideoEncoderConfigurationOptions.MPEG4.ResolutionsAvailable[2].Height = 480;
	g_onvif_cfg.VideoEncoderConfigurationOptions.MPEG4.ResolutionsAvailable[3].Width = 352;
	g_onvif_cfg.VideoEncoderConfigurationOptions.MPEG4.ResolutionsAvailable[3].Height = 288;
	g_onvif_cfg.VideoEncoderConfigurationOptions.MPEG4.ResolutionsAvailable[4].Width = 320;
	g_onvif_cfg.VideoEncoderConfigurationOptions.MPEG4.ResolutionsAvailable[4].Height = 240;
	g_onvif_cfg.VideoEncoderConfigurationOptions.MPEG4.Mpeg4Profile_SP = 1;
	g_onvif_cfg.VideoEncoderConfigurationOptions.MPEG4.GovLengthRange.Min = 10;
	g_onvif_cfg.VideoEncoderConfigurationOptions.MPEG4.GovLengthRange.Max = 60;
	g_onvif_cfg.VideoEncoderConfigurationOptions.MPEG4.FrameRateRange.Min = 1;
	g_onvif_cfg.VideoEncoderConfigurationOptions.MPEG4.FrameRateRange.Max = 30;
	g_onvif_cfg.VideoEncoderConfigurationOptions.MPEG4.EncodingIntervalRange.Min = 5;
	g_onvif_cfg.VideoEncoderConfigurationOptions.MPEG4.EncodingIntervalRange.Max = 60;

	// h264 config options
	g_onvif_cfg.VideoEncoderConfigurationOptions.H264.ResolutionsAvailable[0].Width = 1920;
	g_onvif_cfg.VideoEncoderConfigurationOptions.H264.ResolutionsAvailable[0].Height = 1080;
	g_onvif_cfg.VideoEncoderConfigurationOptions.H264.ResolutionsAvailable[1].Width = 1280;
	g_onvif_cfg.VideoEncoderConfigurationOptions.H264.ResolutionsAvailable[1].Height = 720;
	g_onvif_cfg.VideoEncoderConfigurationOptions.H264.ResolutionsAvailable[2].Width = 640;
	g_onvif_cfg.VideoEncoderConfigurationOptions.H264.ResolutionsAvailable[2].Height = 480;
	g_onvif_cfg.VideoEncoderConfigurationOptions.H264.ResolutionsAvailable[3].Width = 352;
	g_onvif_cfg.VideoEncoderConfigurationOptions.H264.ResolutionsAvailable[3].Height = 288;
	g_onvif_cfg.VideoEncoderConfigurationOptions.H264.ResolutionsAvailable[4].Width = 320;
	g_onvif_cfg.VideoEncoderConfigurationOptions.H264.ResolutionsAvailable[4].Height = 240;
	g_onvif_cfg.VideoEncoderConfigurationOptions.H264.H264Profile_Baseline = 1;
	g_onvif_cfg.VideoEncoderConfigurationOptions.H264.H264Profile_Main = 1;
	g_onvif_cfg.VideoEncoderConfigurationOptions.H264.GovLengthRange.Min = 10;
	g_onvif_cfg.VideoEncoderConfigurationOptions.H264.GovLengthRange.Max = 60;
	g_onvif_cfg.VideoEncoderConfigurationOptions.H264.FrameRateRange.Min = 1;
	g_onvif_cfg.VideoEncoderConfigurationOptions.H264.FrameRateRange.Max = 30;
	g_onvif_cfg.VideoEncoderConfigurationOptions.H264.EncodingIntervalRange.Min = 5;
	g_onvif_cfg.VideoEncoderConfigurationOptions.H264.EncodingIntervalRange.Max = 60;
}

/*
 * Initialize the video source
 * 
 */
void onvif_init_VideoSource()
{
	// todo : here init one video source (1280*720*25)
	
	g_onvif_cfg.v_src = (ONVIF_VideoSource *) malloc(sizeof(ONVIF_VideoSource));
	if (NULL == g_onvif_cfg.v_src)
	{
		return;
	}

	memset(g_onvif_cfg.v_src, 0, sizeof(ONVIF_VideoSource));

	g_onvif_cfg.v_src->VideoSource.Framerate = 25;
	g_onvif_cfg.v_src->VideoSource.Resolution.Width = 1280;
	g_onvif_cfg.v_src->VideoSource.Resolution.Height = 720;
	strcpy(g_onvif_cfg.v_src->VideoSource.token, "VideoSourceToken");
}

/*
 * Initialize the video source configuration options
 * 
 */
void onvif_init_VideoSourceConfigurationOptions()
{
	// Specify the range can be configured to  the video source
	
	g_onvif_cfg.VideoSourceConfigurationOptions.BoundsRange.XRange.Min = 0;
	g_onvif_cfg.VideoSourceConfigurationOptions.BoundsRange.XRange.Max = 100;

	g_onvif_cfg.VideoSourceConfigurationOptions.BoundsRange.YRange.Min = 0;
	g_onvif_cfg.VideoSourceConfigurationOptions.BoundsRange.YRange.Max = 100;

	g_onvif_cfg.VideoSourceConfigurationOptions.BoundsRange.WidthRange.Min = 320;
	g_onvif_cfg.VideoSourceConfigurationOptions.BoundsRange.WidthRange.Max = 1280;

	g_onvif_cfg.VideoSourceConfigurationOptions.BoundsRange.HeightRange.Min = 240;
	g_onvif_cfg.VideoSourceConfigurationOptions.BoundsRange.HeightRange.Max = 720;	
}

void onvif_init_SystemDateTime()
{
    int ret = -1;
    ret = GetSystemDateTime(&g_onvif_cfg.SystemDateTime);
	if (ret < 0) {
		g_onvif_cfg.SystemDateTime.DateTimeType = SetDateTimeType_Manual;
	    g_onvif_cfg.SystemDateTime.DaylightSavings = FALSE;
	    g_onvif_cfg.SystemDateTime.TimeZoneFlag = 1;
		strcpy(g_onvif_cfg.SystemDateTime.TimeZone.TZ, "CST-8:00:00");
	}

	UTIL_INFO("TZ=%s", g_onvif_cfg.SystemDateTime.TimeZone.TZ);
}

void onvif_init_MetadataConfiguration()
{
	g_onvif_cfg.metadata_cfg = (ONVIF_MetadataConfiguration *) malloc(sizeof(ONVIF_MetadataConfiguration));
	if (NULL == g_onvif_cfg.metadata_cfg)
	{
		return;
	}

	memset(g_onvif_cfg.metadata_cfg, 0, sizeof(ONVIF_MetadataConfiguration));

	g_onvif_cfg.metadata_cfg->Configuration.AnalyticsFlag = 1;
	g_onvif_cfg.metadata_cfg->Configuration.Analytics = FALSE;

	g_onvif_cfg.metadata_cfg->Configuration.SessionTimeout = 60;
	g_onvif_cfg.metadata_cfg->Configuration.PTZStatusFlag = 1;
	g_onvif_cfg.metadata_cfg->Configuration.PTZStatus.Status = 1;
	g_onvif_cfg.metadata_cfg->Configuration.PTZStatus.Position = 0;

	strcpy(g_onvif_cfg.metadata_cfg->Configuration.Name, "MetadataConfiguration");
	strcpy(g_onvif_cfg.metadata_cfg->Configuration.token, "MetadataToken");

	onvif_init_multicast_cfg(&g_onvif_cfg.metadata_cfg->Configuration.Multicast);
}

void onvif_init_MetadataConfigurationOptions()
{
	g_onvif_cfg.MetadataConfigurationOptions.PTZStatusFilterOptions.PanTiltPositionSupportedFlag = 0;
	g_onvif_cfg.MetadataConfigurationOptions.PTZStatusFilterOptions.PanTiltPositionSupported = FALSE;

	g_onvif_cfg.MetadataConfigurationOptions.PTZStatusFilterOptions.ZoomPositionSupportedFlag = 0;
	g_onvif_cfg.MetadataConfigurationOptions.PTZStatusFilterOptions.ZoomPositionSupported = FALSE;

	g_onvif_cfg.MetadataConfigurationOptions.PTZStatusFilterOptions.PanTiltStatusSupported = TRUE;
	g_onvif_cfg.MetadataConfigurationOptions.PTZStatusFilterOptions.ZoomStatusSupported = TRUE;
}

void onvif_init_OSDConfigurationOptions()
{	
	g_onvif_cfg.OSDConfigurationOptions.OSDType_Text = 1;
	g_onvif_cfg.OSDConfigurationOptions.OSDType_Image = 0;
	g_onvif_cfg.OSDConfigurationOptions.OSDType_Extended = 0;
	g_onvif_cfg.OSDConfigurationOptions.OSDPosType_UpperLeft = 1;
	g_onvif_cfg.OSDConfigurationOptions.OSDPosType_UpperRight = 1;
	g_onvif_cfg.OSDConfigurationOptions.OSDPosType_LowerLeft = 1;
	g_onvif_cfg.OSDConfigurationOptions.OSDPosType_LowerRight = 1;
	g_onvif_cfg.OSDConfigurationOptions.OSDPosType_Custom = 1;
	g_onvif_cfg.OSDConfigurationOptions.TextOptionFlag = 1;
	g_onvif_cfg.OSDConfigurationOptions.ImageOptionFlag = 0;
	
	g_onvif_cfg.OSDConfigurationOptions.MaximumNumberOfOSDs.ImageFlag = 0;
	g_onvif_cfg.OSDConfigurationOptions.MaximumNumberOfOSDs.PlainTextFlag = 1;
	g_onvif_cfg.OSDConfigurationOptions.MaximumNumberOfOSDs.DateFlag = 1;
	g_onvif_cfg.OSDConfigurationOptions.MaximumNumberOfOSDs.TimeFlag = 1;
	g_onvif_cfg.OSDConfigurationOptions.MaximumNumberOfOSDs.DateAndTimeFlag = 1;

	g_onvif_cfg.OSDConfigurationOptions.MaximumNumberOfOSDs.Total = 5;
	g_onvif_cfg.OSDConfigurationOptions.MaximumNumberOfOSDs.Image = 0;
	g_onvif_cfg.OSDConfigurationOptions.MaximumNumberOfOSDs.PlainText = 4;
	g_onvif_cfg.OSDConfigurationOptions.MaximumNumberOfOSDs.Date = 1;
	g_onvif_cfg.OSDConfigurationOptions.MaximumNumberOfOSDs.Time = 1;
	g_onvif_cfg.OSDConfigurationOptions.MaximumNumberOfOSDs.DateAndTime = 1;
	
	g_onvif_cfg.OSDConfigurationOptions.TextOption.OSDTextType_Plain = 1;
	g_onvif_cfg.OSDConfigurationOptions.TextOption.OSDTextType_Date = 1;
	g_onvif_cfg.OSDConfigurationOptions.TextOption.OSDTextType_Time = 1;
	g_onvif_cfg.OSDConfigurationOptions.TextOption.OSDTextType_DateAndTime = 1;
	g_onvif_cfg.OSDConfigurationOptions.TextOption.FontSizeRangeFlag = 1;
	g_onvif_cfg.OSDConfigurationOptions.TextOption.FontColorFlag = 0;
	g_onvif_cfg.OSDConfigurationOptions.TextOption.BackgroundColorFlag = 0;

	g_onvif_cfg.OSDConfigurationOptions.TextOption.FontSizeRange.Min = 16;
	g_onvif_cfg.OSDConfigurationOptions.TextOption.FontSizeRange.Max = 64;

	g_onvif_cfg.OSDConfigurationOptions.TextOption.DateFormatSize = 4;
	strcpy(g_onvif_cfg.OSDConfigurationOptions.TextOption.DateFormat[0], "MM/dd/yyyy");
	strcpy(g_onvif_cfg.OSDConfigurationOptions.TextOption.DateFormat[1], "dd/MM/yyyy");
	strcpy(g_onvif_cfg.OSDConfigurationOptions.TextOption.DateFormat[2], "yyyy/MM/dd");
	strcpy(g_onvif_cfg.OSDConfigurationOptions.TextOption.DateFormat[3], "yyyy-MM-dd");

	g_onvif_cfg.OSDConfigurationOptions.TextOption.TimeFormatSize = 2;
	strcpy(g_onvif_cfg.OSDConfigurationOptions.TextOption.TimeFormat[0], "hh:mm:ss tt");
	strcpy(g_onvif_cfg.OSDConfigurationOptions.TextOption.TimeFormat[1], "HH:mm:ss");
}

ONVIF_NetworkInterface * onvif_add_NetworkInterface()
{
    ONVIF_NetworkInterface * p_tmp;
	ONVIF_NetworkInterface * p_net_inf = (ONVIF_NetworkInterface *) malloc(sizeof(ONVIF_NetworkInterface));
	if (NULL == p_net_inf)
	{
		return NULL;
	}

	memset(p_net_inf, 0, sizeof(ONVIF_NetworkInterface));

    snprintf(p_net_inf->NetworkInterface.token, ONVIF_TOKEN_LEN, "eth%d", g_onvif_cls.netinf_idx);

    g_onvif_cls.netinf_idx++;

	p_tmp = g_onvif_cfg.network.interfaces;
	if (NULL == p_tmp)
	{
		g_onvif_cfg.network.interfaces = p_net_inf;
	}
	else
	{
		while (p_tmp && p_tmp->next) p_tmp = p_tmp->next;

		p_tmp->next = p_net_inf;
	}

	return p_net_inf;
}

void onvif_init_NetworkInterface(onvif_NetworkInterface	*pNetworkInterface)
{
#if __WINDOWS_OS__

	IP_ADAPTER_ADDRESSES addr[16], *paddr;
	DWORD len = sizeof(addr);
	
	if (NO_ERROR == GetAdaptersAddresses(AF_INET, 0, 0, addr, &len) && len >= sizeof(IP_ADAPTER_ADDRESSES))
	{
		paddr = addr;
		while (paddr)
		{
		    ONVIF_NetworkInterface * p_net_inf;
		    IP_ADAPTER_UNICAST_ADDRESS * p_ipaddr;
		    
			if (paddr->IfType & IF_TYPE_SOFTWARE_LOOPBACK)
			{
				paddr = paddr->Next;
				continue;
			}

            // not 169.xxx.xxx.xxx
			p_ipaddr = paddr->FirstUnicastAddress;
			while (p_ipaddr)
			{
			    if (p_ipaddr->Address.lpSockaddr->sa_family != AF_INET)
			    {
			        goto NEXT;
			    }
			    else
			    {
			        struct sockaddr_in * p_inaddr = (struct sockaddr_in *)p_ipaddr->Address.lpSockaddr;

			        if (p_inaddr->sin_addr.s_net == 169)
			        {
			            goto NEXT;
			        }
			    }

			    p_ipaddr = p_ipaddr->Next;
			}
			
			p_net_inf = onvif_add_NetworkInterface();
			if (NULL == p_net_inf)
			{
				return;
			}

			p_net_inf->NetworkInterface.Enabled = TRUE;
			p_net_inf->NetworkInterface.InfoFlag = 1;			
			p_net_inf->NetworkInterface.Info.MTUFlag = 1;
			p_net_inf->NetworkInterface.IPv4Flag = 1;			
			
			sprintf(p_net_inf->NetworkInterface.Info.HwAddress, "%02X:%02X:%02X:%02X:%02X:%02X", 
				paddr->PhysicalAddress[0], paddr->PhysicalAddress[1], paddr->PhysicalAddress[2], 
				paddr->PhysicalAddress[3], paddr->PhysicalAddress[4], paddr->PhysicalAddress[5]);

			p_net_inf->NetworkInterface.Info.MTU = paddr->Mtu;
			p_net_inf->NetworkInterface.IPv4.Enabled = (paddr->Flags & IP_ADAPTER_IPV4_ENABLED);
			p_net_inf->NetworkInterface.IPv4.Config.DHCP = (paddr->Flags & IP_ADAPTER_DHCP_ENABLED);

			p_ipaddr = paddr->FirstUnicastAddress;
			while (p_ipaddr)
			{
				if (p_ipaddr->Address.lpSockaddr->sa_family == AF_INET)
				{
					struct sockaddr_in * p_inaddr = (struct sockaddr_in *)p_ipaddr->Address.lpSockaddr;

					strcpy(p_net_inf->NetworkInterface.IPv4.Config.Address, inet_ntoa(p_inaddr->sin_addr));
					if (p_ipaddr->OnLinkPrefixLength > 0 && p_ipaddr->OnLinkPrefixLength <= 32)
					{
						p_net_inf->NetworkInterface.IPv4.Config.PrefixLength = p_ipaddr->OnLinkPrefixLength;
					}
					else
					{
						p_net_inf->NetworkInterface.IPv4.Config.PrefixLength = 24;
					}
					break;
				}

				p_ipaddr = p_ipaddr->Next;
			}

NEXT:		
			paddr = paddr->Next;
		}
	}

#elif __LINUX_OS__

    int i;    
	int socket_fd;
	struct ifreq *ifr;
	struct ifconf conf;
	struct ifreq ifs[8];
	int num;
	
	socket_fd = socket(AF_INET, SOCK_DGRAM, 0);
	
	conf.ifc_len = sizeof(ifs);
	conf.ifc_req = ifs;
	
	ioctl(socket_fd, SIOCGIFCONF, &conf);
	
	num = conf.ifc_len / sizeof(struct ifreq);
	ifr = conf.ifc_req;
	
	for (i=0; i<num; i++)
	{		
		struct sockaddr_in *sin = (struct sockaddr_in *)(&ifr->ifr_addr);

		if (ifr->ifr_addr.sa_family != AF_INET)
		{
			ifr++;
			continue;
		}
		
		if (ioctl(socket_fd, SIOCGIFFLAGS, ifr) == 0 && (ifr->ifr_flags & IFF_LOOPBACK) == 0) // not loopback interface
		{
			ONVIF_NetworkInterface * p_net_inf = onvif_add_NetworkInterface();
			if (NULL == p_net_inf)
			{
				break;
			}
			
			p_net_inf->NetworkInterface.Enabled = TRUE;
			p_net_inf->NetworkInterface.InfoFlag = 1;			
			p_net_inf->NetworkInterface.Info.NameFlag = 1;
			p_net_inf->NetworkInterface.IPv4Flag = 1;
			p_net_inf->NetworkInterface.IPv4.Enabled = TRUE;
            if (pNetworkInterface) {
				p_net_inf->NetworkInterface.IPv4.Config.DHCP = pNetworkInterface->IPv4.Config.DHCP;
            }
			else {
				p_net_inf->NetworkInterface.IPv4.Config.DHCP = FALSE;
			}

			strcpy(p_net_inf->NetworkInterface.IPv4.Config.Address, inet_ntoa(sin->sin_addr));
			strncpy(p_net_inf->NetworkInterface.Info.Name, ifr->ifr_name, sizeof(p_net_inf->NetworkInterface.Info.Name)-1); 

			// get netmask
			if (ioctl(socket_fd, SIOCGIFNETMASK, ifr) == 0)
			{
				sin = (struct sockaddr_in *)(&ifr->ifr_netmask);
				p_net_inf->NetworkInterface.IPv4.Config.PrefixLength = get_prefix_len_by_mask(inet_ntoa(sin->sin_addr));
			}
			else
			{
				p_net_inf->NetworkInterface.IPv4.Config.PrefixLength = 24;
			}

			// get mtu
			if (ioctl(socket_fd, SIOCGIFMTU, ifr) == 0)
			{
				p_net_inf->NetworkInterface.Info.MTUFlag = 1;
				p_net_inf->NetworkInterface.Info.MTU = ifr->ifr_mtu;
			}

			// get hwaddr
	        if (ioctl(socket_fd, SIOCGIFHWADDR, ifr) == 0) 
	        {
	        	snprintf(p_net_inf->NetworkInterface.Info.HwAddress, sizeof(p_net_inf->NetworkInterface.Info.HwAddress), "%02X:%02X:%02X:%02X:%02X:%02X", 
					(unsigned char)ifr->ifr_hwaddr.sa_data[0], (unsigned char)ifr->ifr_hwaddr.sa_data[1], (unsigned char)ifr->ifr_hwaddr.sa_data[2],
					(unsigned char)ifr->ifr_hwaddr.sa_data[3], (unsigned char)ifr->ifr_hwaddr.sa_data[4], (unsigned char)ifr->ifr_hwaddr.sa_data[5]);	
	        }			
		}
		
		ifr++;
	}

	closesocket(socket_fd);
	
#endif
	
}

void onvif_build_gateway(char *ipAddr)
{
	char *temp = NULL;
	
	ONVIF_NetworkInterface * p_net_inf = g_onvif_cfg.network.interfaces;
	if (p_net_inf && strlen(p_net_inf->NetworkInterface.IPv4.Config.Address) > 0) {
		strcpy(ipAddr, p_net_inf->NetworkInterface.IPv4.Config.Address);
		temp = strrchr(ipAddr, '.');
		ipAddr[temp-ipAddr+1] = '1';
		ipAddr[temp-ipAddr+2] = '\0';
	}
	else {
		strcpy(ipAddr, "192.168.1.1");
	}
}


void onvif_init_net()
{
    const char * dns;
    const char * gw;
    int ret = -1;

    g_onvif_cfg.network.DiscoveryMode = DiscoveryMode_Discoverable;

	// 1.init network interface
    onvif_NetworkInterface	pNetworkInterface;	
    ret = GetNetworkInterfaces(&pNetworkInterface);
	if (ret == 0) {
		SetNetworkInterfaces(&pNetworkInterface, FALSE);
		onvif_init_NetworkInterface(&pNetworkInterface);
	}
	else {
		//system_ex("killall udhcpc; udhcpc -i eth0");
		onvif_init_NetworkInterface(NULL);
	}

    if (g_onvif_cfg.network.interfaces) {
        // init zero configuration
        strcpy(g_onvif_cfg.network.ZeroConfiguration.InterfaceToken, g_onvif_cfg.network.interfaces->NetworkInterface.token);
    }
	
	// init host name
    g_onvif_cfg.network.HostnameInformation.FromDHCP = FALSE;
    g_onvif_cfg.network.HostnameInformation.RebootNeeded = FALSE;
    g_onvif_cfg.network.HostnameInformation.NameFlag = 1;
    gethostname(g_onvif_cfg.network.HostnameInformation.Name, sizeof(g_onvif_cfg.network.HostnameInformation.Name));

	// init dns setting
	ret = GetDNSInformation(&g_onvif_cfg.network.DNSInformation);
	if (ret < 0) {
	    g_onvif_cfg.network.DNSInformation.SearchDomainFlag = 1;
	    g_onvif_cfg.network.DNSInformation.FromDHCP = FALSE;
	    
	    dns = get_dns_server();
	    if (dns && strlen(dns) > 0){
	    	strcpy(g_onvif_cfg.network.DNSInformation.DNSServer[0], dns);
	    }
		else {
			onvif_build_gateway(g_onvif_cfg.network.DNSInformation.DNSServer[0]);
			SetDNSInformation(&g_onvif_cfg.network.DNSInformation, FALSE);
		}
		UTIL_INFO("dns==%s", g_onvif_cfg.network.DNSInformation.DNSServer[0]);
	}
	else {
		SetDNSInformation(&g_onvif_cfg.network.DNSInformation, FALSE);
	}

    // init ntp settting
    ret = GetNTPInformation(&g_onvif_cfg.network.NTPInformation);
	if (ret < 0) {
	    g_onvif_cfg.network.NTPInformation.FromDHCP = FALSE;
	    strcpy(g_onvif_cfg.network.NTPInformation.NTPServer[0], "ntp1.aliyun.com");
		SetNTPInformation(&g_onvif_cfg.network.NTPInformation, TRUE);
	}

    // init default gateway
    ret = GetNetworkGateway(&g_onvif_cfg.network.NetworkGateway);
	if (ret < 0) {
	    gw = get_default_gateway();
	    if (gw && strlen(gw) > 0) {
	    	strcpy(g_onvif_cfg.network.NetworkGateway.IPv4Address[0], gw);
	    }
		else {
			onvif_build_gateway(g_onvif_cfg.network.NetworkGateway.IPv4Address[0]);
			SetNetworkGateway(&g_onvif_cfg.network.NetworkGateway, FALSE);
		}
		
		UTIL_INFO("gw==%s", g_onvif_cfg.network.NetworkGateway.IPv4Address[0]);
	}
	else {
		SetNetworkGateway(&g_onvif_cfg.network.NetworkGateway, FALSE);
	}
	
	// init network protocol
	ret = GetNetworkProtocols(&g_onvif_cfg.network.NetworkProtocol);
	if (ret < 0) {
		g_onvif_cfg.network.NetworkProtocol.HTTPFlag = 1;
		g_onvif_cfg.network.NetworkProtocol.HTTPEnabled = 1;
#ifdef HTTPS    
		g_onvif_cfg.network.NetworkProtocol.HTTPSFlag = 1;
		g_onvif_cfg.network.NetworkProtocol.HTTPSEnabled = g_onvif_cfg.https_enable;
#else
		g_onvif_cfg.network.NetworkProtocol.HTTPSFlag = 0;
		g_onvif_cfg.network.NetworkProtocol.HTTPSEnabled = 0;
#endif
		g_onvif_cfg.network.NetworkProtocol.RTSPFlag = 1;
		g_onvif_cfg.network.NetworkProtocol.RTSPEnabled = 1;
		
		g_onvif_cfg.network.NetworkProtocol.HTTPPort[0] = 80;
		g_onvif_cfg.network.NetworkProtocol.HTTPSPort[0] = 443;
		g_onvif_cfg.network.NetworkProtocol.RTSPPort[0] = 554;
		SetNetworkProtocols(&g_onvif_cfg.network.NetworkProtocol, TRUE);
	}
	
}

#ifdef MEDIA2_SUPPORT

ONVIF_Mask * onvif_add_Mask()
{
    ONVIF_Mask * p_tmp;
	ONVIF_Mask * p_new = (ONVIF_Mask *) malloc(sizeof(ONVIF_Mask));
	if (NULL == p_new)
	{
		return NULL;
	}

	memset(p_new, 0, sizeof(ONVIF_Mask));

	p_tmp = g_onvif_cfg.mask;
	if (NULL == p_tmp)
	{
		g_onvif_cfg.mask = p_new;
	}
	else
	{
		while (p_tmp && p_tmp->next) p_tmp = p_tmp->next;

		p_tmp->next = p_new;
	}

	return p_new;
}

ONVIF_Mask * onvif_find_Mask(const char * token)
{
    ONVIF_Mask * p_tmp = g_onvif_cfg.mask;

    if (NULL == token)
    {
        return NULL;
    }
    
    while (p_tmp)
    {
        if (strcasecmp(token, p_tmp->Mask.token) == 0)
        {
            return p_tmp;
        }
        
        p_tmp = p_tmp->next;
    }

    return NULL;
}

void onvif_free_Masks(ONVIF_Mask ** p_head)
{
    ONVIF_Mask * p_next;
	ONVIF_Mask * p_tmp = *p_head;

	while (p_tmp)
	{
		p_next = p_tmp->next;
        
		free(p_tmp);
		p_tmp = p_next;
	}

	*p_head = NULL;
}

void onvif_init_MaskOptions()
{
    g_onvif_cfg.MaskOptions.MaxMasks = 10;
    g_onvif_cfg.MaskOptions.MaxPoints = 10;

    g_onvif_cfg.MaskOptions.sizeTypes = 3;
    strcpy(g_onvif_cfg.MaskOptions.Types[0], "Color");
    strcpy(g_onvif_cfg.MaskOptions.Types[1], "Pixelated");
    strcpy(g_onvif_cfg.MaskOptions.Types[2], "Blurred");

    g_onvif_cfg.MaskOptions.Color.sizeColorList = 1;
    g_onvif_cfg.MaskOptions.Color.ColorList[0].X = 100;
    g_onvif_cfg.MaskOptions.Color.ColorList[0].Y = 100;
    g_onvif_cfg.MaskOptions.Color.ColorList[0].Z = 100;
    g_onvif_cfg.MaskOptions.Color.ColorList[0].ColorspaceFlag = 1;
    strcpy(g_onvif_cfg.MaskOptions.Color.ColorList[0].Colorspace, "http://www.onvif.org/ver10/colorspace/YCbCr");
    g_onvif_cfg.MaskOptions.Color.sizeColorspaceRange = 0;

    g_onvif_cfg.MaskOptions.RectangleOnly = TRUE;
    g_onvif_cfg.MaskOptions.SingleColorOnly = FALSE;
}

#endif

#ifdef AUDIO_SUPPORT

ONVIF_AudioSource * onvif_find_AudioSource(const char * token)
{
    ONVIF_AudioSource * p_tmp = g_onvif_cfg.a_src;

    if (NULL == token)
    {
        return NULL;
    }
    
    while (p_tmp)
    {
        if (strcasecmp(token, p_tmp->AudioSource.token) == 0)
        {
            return p_tmp;
        }
        
        p_tmp = p_tmp->next;
    }

    return NULL;
}

ONVIF_AudioSourceConfiguration * onvif_find_AudioSourceConfiguration(const char * token)
{
	ONVIF_AudioSourceConfiguration * p_tmp = g_onvif_cfg.a_src_cfg;

	if (NULL == token)
    {
        return NULL;
    }
    
    while (p_tmp)
    {
        if (strcasecmp(token, p_tmp->Configuration.token) == 0)
        {
            return p_tmp;
        }
        
        p_tmp = p_tmp->next;
    }

    return NULL;
}

ONVIF_AudioEncoder2Configuration * onvif_find_AudioEncoderConfiguration(const char * token)
{
    ONVIF_AudioEncoder2Configuration * p_tmp = g_onvif_cfg.a_enc_cfg;

    if (NULL == token)
    {
        return NULL;
    }
    
    while (p_tmp)
    {
        if (strcasecmp(token, p_tmp->Configuration.token) == 0)
        {
            return p_tmp;
        }
        
        p_tmp = p_tmp->next;
    }

    return NULL;
}

ONVIF_AudioSourceConfiguration * onvif_add_AudioSourceConfiguration()
{
    ONVIF_AudioSourceConfiguration * p_tmp;
	ONVIF_AudioSourceConfiguration * p_new = (ONVIF_AudioSourceConfiguration *) malloc(sizeof(ONVIF_AudioSourceConfiguration));
	if (NULL == p_new)
	{
		return NULL;
	}

	memset(p_new, 0, sizeof(ONVIF_AudioSourceConfiguration));

	snprintf(p_new->Configuration.Name, ONVIF_NAME_LEN, "A_SRC_CFG_00%d", g_onvif_cls.a_src_idx);
    snprintf(p_new->Configuration.token, ONVIF_TOKEN_LEN, "A_SRC_CFG_00%d", g_onvif_cls.a_src_idx);

    assert(g_onvif_cfg.a_src);
    strcpy(p_new->Configuration.SourceToken, g_onvif_cfg.a_src->AudioSource.token);

    g_onvif_cls.a_src_idx++;

	p_tmp = g_onvif_cfg.a_src_cfg;
	if (NULL == p_tmp)
	{
		g_onvif_cfg.a_src_cfg = p_new;
	}
	else
	{
		while (p_tmp && p_tmp->next) p_tmp = p_tmp->next;

		p_tmp->next = p_new;
	}

	return p_new;
}

ONVIF_AudioEncoder2Configuration * onvif_find_AudioEncoderConfiguration_by_param(ONVIF_AudioEncoder2Configuration * p_a_enc_cfg)
{
	ONVIF_AudioEncoder2Configuration * p_tmp = g_onvif_cfg.a_enc_cfg;
	while (p_tmp)
	{
		if (p_tmp->Configuration.SessionTimeout == p_a_enc_cfg->Configuration.SessionTimeout &&
			p_tmp->Configuration.SampleRate == p_a_enc_cfg->Configuration.SampleRate && 
			p_tmp->Configuration.Bitrate == p_a_enc_cfg->Configuration.Bitrate && 
			strcmp(p_tmp->Configuration.Encoding, p_a_enc_cfg->Configuration.Encoding) == 0)
		{
			break;
		}

		p_tmp = p_tmp->next;
	}

	return p_tmp;
}

ONVIF_AudioEncoder2ConfigurationOptions * onvif_add_AudioEncoder2ConfigurationOptions(ONVIF_AudioEncoder2ConfigurationOptions ** p_head)
{
    ONVIF_AudioEncoder2ConfigurationOptions * p_tmp;
	ONVIF_AudioEncoder2ConfigurationOptions * p_new = (ONVIF_AudioEncoder2ConfigurationOptions *) malloc(sizeof(ONVIF_AudioEncoder2ConfigurationOptions));
	if (NULL == p_new)
	{
		return NULL;
	}

	memset(p_new, 0, sizeof(ONVIF_AudioEncoder2ConfigurationOptions));

	p_tmp = *p_head;
	if (NULL == p_tmp)
	{
		*p_head = p_new;
	}
	else
	{
		while (p_tmp && p_tmp->next) p_tmp = p_tmp->next;

		p_tmp->next = p_new;
	}	

	return p_new;
}

ONVIF_AudioEncoder2ConfigurationOptions * onvif_find_AudioEncoder2ConfigurationOptions(const char * Encoding)
{
    ONVIF_AudioEncoder2ConfigurationOptions * p_tmp = g_onvif_cfg.a_enc_cfg_opt;
	while (p_tmp)
	{
		if (strcmp(p_tmp->Options.Encoding, Encoding) == 0)
		{
			break;
		}

		p_tmp = p_tmp->next;
	}

	return p_tmp;
}

ONVIF_AudioDecoderConfiguration * onvif_add_AudioDecoderConfiguration()
{
    ONVIF_AudioDecoderConfiguration * p_tmp;
	ONVIF_AudioDecoderConfiguration * p_new = (ONVIF_AudioDecoderConfiguration *) malloc(sizeof(ONVIF_AudioDecoderConfiguration));
	if (NULL == p_new)
	{
		return NULL;
	}

	memset(p_new, 0, sizeof(ONVIF_AudioDecoderConfiguration));

	snprintf(p_new->Configuration.Name, ONVIF_NAME_LEN, "A_DEC_00%d", g_onvif_cls.a_dec_idx);
    snprintf(p_new->Configuration.token, ONVIF_TOKEN_LEN, "A_DEC_00%d", g_onvif_cls.a_dec_idx);
	
    g_onvif_cls.a_dec_idx++;

	p_tmp = g_onvif_cfg.a_dec_cfg;
	if (NULL == p_tmp)
	{
		g_onvif_cfg.a_dec_cfg = p_new;
	}
	else
	{
		while (p_tmp && p_tmp->next) p_tmp = p_tmp->next;

		p_tmp->next = p_new;
	}
	
	return p_new;
}

ONVIF_AudioDecoderConfiguration * onvif_find_AudioDecoderConfiguration(const char * token)
{
    ONVIF_AudioDecoderConfiguration * p_tmp = g_onvif_cfg.a_dec_cfg;
	while (p_tmp)
	{
		if (strcmp(p_tmp->Configuration.token, token) == 0)
		{
			break;
		}

		p_tmp = p_tmp->next;
	}

	return p_tmp;
}

void onvif_init_AudioEncoder2ConfigurationOptions
(
onvif_AudioEncoder2ConfigurationOptions * p_option,
const char * Encoding
)
{
    strcpy(p_option->Encoding, Encoding);
    if (strcasecmp(Encoding, "PCMU") == 0)
    {
        p_option->AudioEncoding = AudioEncoding_G711;
    }
    else if (strcasecmp(Encoding, "G726") == 0)
    {
        p_option->AudioEncoding = AudioEncoding_G726;
    }
    else if (strcasecmp(Encoding, "MP4A-LATM") == 0)
    {
        p_option->AudioEncoding = AudioEncoding_AAC;
    }

    p_option->BitrateList.sizeItems = 5;
	p_option->BitrateList.Items[0] = 64;
	p_option->BitrateList.Items[1] = 80;
	p_option->BitrateList.Items[2] = 128;
	p_option->BitrateList.Items[3] = 256;
	p_option->BitrateList.Items[4] = 320;
	
	// specify the supported samplerate
	p_option->SampleRateList.sizeItems = 5;
	p_option->SampleRateList.Items[0] = 8;
	p_option->SampleRateList.Items[1] = 12;
	p_option->SampleRateList.Items[2] = 25;
	p_option->SampleRateList.Items[3] = 32;
	p_option->SampleRateList.Items[4] = 48;
}

ONVIF_AudioEncoder2Configuration * onvif_add_AudioEncoderConfiguration(ONVIF_AudioEncoder2Configuration * p_a_enc_cfg)
{
    ONVIF_AudioEncoder2Configuration * p_tmp;
	ONVIF_AudioEncoder2Configuration * p_new = (ONVIF_AudioEncoder2Configuration *) malloc(sizeof(ONVIF_AudioEncoder2Configuration));
	if (NULL == p_new)
	{
		return NULL;
	}

	p_new->next = NULL;
	memcpy(&p_new->Configuration, &p_a_enc_cfg->Configuration, sizeof(onvif_AudioEncoder2Configuration));

	snprintf(p_new->Configuration.Name, ONVIF_NAME_LEN, "A_ENC_00%d", g_onvif_cls.a_enc_idx);
    snprintf(p_new->Configuration.token, ONVIF_TOKEN_LEN, "A_ENC_00%d", g_onvif_cls.a_enc_idx);

	onvif_init_multicast_cfg(&p_new->Configuration.Multicast);
	
    g_onvif_cls.a_enc_idx++;

	p_tmp = g_onvif_cfg.a_enc_cfg;
	if (NULL == p_tmp)
	{
		g_onvif_cfg.a_enc_cfg = p_new;
	}
	else
	{
		while (p_tmp && p_tmp->next) p_tmp = p_tmp->next;

		p_tmp->next = p_new;
	}
	
	return p_new;
}

/*
 * Initialize the audio source
 * 
 */
void onvif_init_AudioSource()
{
	// todo : here init one audio source (2 channels)
	
	g_onvif_cfg.a_src = (ONVIF_AudioSource *) malloc(sizeof(ONVIF_AudioSource));
	if (NULL == g_onvif_cfg.a_src)
	{
		return;
	}

	memset(g_onvif_cfg.a_src, 0, sizeof(ONVIF_AudioSource));

	g_onvif_cfg.a_src->AudioSource.Channels = 2;
	strcpy(g_onvif_cfg.a_src->AudioSource.token, "AudioSourceToken");
}

/*
 * Initialize the audio encoder configuration options
 * 
 */
void onvif_init_AudioEncoderConfigurationOptions()
{
    ONVIF_AudioEncoder2ConfigurationOptions * p_option;

    p_option = onvif_add_AudioEncoder2ConfigurationOptions(&g_onvif_cfg.a_enc_cfg_opt);

    onvif_init_AudioEncoder2ConfigurationOptions(&p_option->Options, "PCMU");

    p_option = onvif_add_AudioEncoder2ConfigurationOptions(&g_onvif_cfg.a_enc_cfg_opt);

    onvif_init_AudioEncoder2ConfigurationOptions(&p_option->Options, "G726");

    p_option = onvif_add_AudioEncoder2ConfigurationOptions(&g_onvif_cfg.a_enc_cfg_opt);

    onvif_init_AudioEncoder2ConfigurationOptions(&p_option->Options, "MP4A-LATM");    
}

void onvif_init_AudioDecoderConfigurations()
{
    ONVIF_AudioDecoderConfiguration * p_a_dec_cfg = onvif_add_AudioDecoderConfiguration();
}

void onvif_init_AudioDecoderConfigurationOptions()
{
    g_onvif_cfg.a_dec_cfg_opt.AACDecOptionsFlag = 1;
    g_onvif_cfg.a_dec_cfg_opt.AACDecOptions.Bitrate.sizeItems = 6;
    g_onvif_cfg.a_dec_cfg_opt.AACDecOptions.Bitrate.Items[0] = 8;
    g_onvif_cfg.a_dec_cfg_opt.AACDecOptions.Bitrate.Items[1] = 12;
    g_onvif_cfg.a_dec_cfg_opt.AACDecOptions.Bitrate.Items[2] = 20;
    g_onvif_cfg.a_dec_cfg_opt.AACDecOptions.Bitrate.Items[3] = 25;
    g_onvif_cfg.a_dec_cfg_opt.AACDecOptions.Bitrate.Items[4] = 32;
    g_onvif_cfg.a_dec_cfg_opt.AACDecOptions.Bitrate.Items[5] = 40;
    g_onvif_cfg.a_dec_cfg_opt.AACDecOptions.SampleRateRange.sizeItems = 5;
    g_onvif_cfg.a_dec_cfg_opt.AACDecOptions.SampleRateRange.Items[0] = 8;
    g_onvif_cfg.a_dec_cfg_opt.AACDecOptions.SampleRateRange.Items[1] = 12;
    g_onvif_cfg.a_dec_cfg_opt.AACDecOptions.SampleRateRange.Items[2] = 24;
    g_onvif_cfg.a_dec_cfg_opt.AACDecOptions.SampleRateRange.Items[3] = 32;
    g_onvif_cfg.a_dec_cfg_opt.AACDecOptions.SampleRateRange.Items[4] = 48;

    g_onvif_cfg.a_dec_cfg_opt.G711DecOptionsFlag= 1;
    g_onvif_cfg.a_dec_cfg_opt.G711DecOptions.Bitrate.sizeItems = 6;
    g_onvif_cfg.a_dec_cfg_opt.G711DecOptions.Bitrate.Items[0] = 8;
    g_onvif_cfg.a_dec_cfg_opt.G711DecOptions.Bitrate.Items[1] = 12;
    g_onvif_cfg.a_dec_cfg_opt.G711DecOptions.Bitrate.Items[2] = 20;
    g_onvif_cfg.a_dec_cfg_opt.G711DecOptions.Bitrate.Items[3] = 25;
    g_onvif_cfg.a_dec_cfg_opt.G711DecOptions.Bitrate.Items[4] = 32;
    g_onvif_cfg.a_dec_cfg_opt.G711DecOptions.Bitrate.Items[5] = 40;
    g_onvif_cfg.a_dec_cfg_opt.G711DecOptions.SampleRateRange.sizeItems = 5;
    g_onvif_cfg.a_dec_cfg_opt.G711DecOptions.SampleRateRange.Items[0] = 8;
    g_onvif_cfg.a_dec_cfg_opt.G711DecOptions.SampleRateRange.Items[1] = 12;
    g_onvif_cfg.a_dec_cfg_opt.G711DecOptions.SampleRateRange.Items[2] = 24;
    g_onvif_cfg.a_dec_cfg_opt.G711DecOptions.SampleRateRange.Items[3] = 32;
    g_onvif_cfg.a_dec_cfg_opt.G711DecOptions.SampleRateRange.Items[4] = 48;

    g_onvif_cfg.a_dec_cfg_opt.G726DecOptionsFlag = 1;
    g_onvif_cfg.a_dec_cfg_opt.G726DecOptions.Bitrate.sizeItems = 6;
    g_onvif_cfg.a_dec_cfg_opt.G726DecOptions.Bitrate.Items[0] = 8;
    g_onvif_cfg.a_dec_cfg_opt.G726DecOptions.Bitrate.Items[1] = 12;
    g_onvif_cfg.a_dec_cfg_opt.G726DecOptions.Bitrate.Items[2] = 20;
    g_onvif_cfg.a_dec_cfg_opt.G726DecOptions.Bitrate.Items[3] = 25;
    g_onvif_cfg.a_dec_cfg_opt.G726DecOptions.Bitrate.Items[4] = 32;
    g_onvif_cfg.a_dec_cfg_opt.G726DecOptions.Bitrate.Items[5] = 40;
    g_onvif_cfg.a_dec_cfg_opt.G726DecOptions.SampleRateRange.sizeItems = 5;
    g_onvif_cfg.a_dec_cfg_opt.G726DecOptions.SampleRateRange.Items[0] = 8;
    g_onvif_cfg.a_dec_cfg_opt.G726DecOptions.SampleRateRange.Items[1] = 12;
    g_onvif_cfg.a_dec_cfg_opt.G726DecOptions.SampleRateRange.Items[2] = 24;
    g_onvif_cfg.a_dec_cfg_opt.G726DecOptions.SampleRateRange.Items[3] = 32;
    g_onvif_cfg.a_dec_cfg_opt.G726DecOptions.SampleRateRange.Items[4] = 48;
}

#ifdef MEDIA2_SUPPORT

void onvif_init_AudioDecoder2ConfigurationOptions()
{
    ONVIF_AudioEncoder2ConfigurationOptions * p_option;

    p_option = onvif_add_AudioEncoder2ConfigurationOptions(&g_onvif_cfg.a_dec2_cfg_opt);

    onvif_init_AudioEncoder2ConfigurationOptions(&p_option->Options, "PCMU");

    p_option = onvif_add_AudioEncoder2ConfigurationOptions(&g_onvif_cfg.a_dec2_cfg_opt);

    onvif_init_AudioEncoder2ConfigurationOptions(&p_option->Options, "G726");

    p_option = onvif_add_AudioEncoder2ConfigurationOptions(&g_onvif_cfg.a_dec2_cfg_opt);

    onvif_init_AudioEncoder2ConfigurationOptions(&p_option->Options, "MP4A-LATM");
}

#endif //  MEDIA2_SUPPORT

#endif // end of AUDIO_SUPPORT

#ifdef PTZ_SUPPORT

ONVIF_PTZNode * onvif_find_PTZNode(const char * token)
{
	ONVIF_PTZNode * p_tmp = g_onvif_cfg.ptz_node;
	while (p_tmp)
	{
		if (strcmp(p_tmp->PTZNode.token, token) == 0)
		{
			break;
		}

		p_tmp = p_tmp->next;
	}

	return p_tmp;
}

ONVIF_PTZConfiguration * onvif_find_PTZConfiguration(const char * token)
{
	ONVIF_PTZConfiguration * p_tmp = g_onvif_cfg.ptz_cfg;
	while (p_tmp)
	{
		if (strcmp(p_tmp->Configuration.token, token) == 0)
		{
			break;
		}

		p_tmp = p_tmp->next;
	}

	return p_tmp;
}

ONVIF_PTZPreset * onvif_find_PTZPreset(const char * profile_token, const char  * preset_token)
{
    int i;
    ONVIF_PROFILE * p_profile = onvif_find_profile(profile_token);
    if (NULL == p_profile)
    {
        return NULL;
    }

    for (i = 0; i < ARRAY_SIZE(p_profile->presets); i++)
    {
        if (strcmp(preset_token, p_profile->presets[i].PTZPreset.token) == 0)
        {
            break;
        }
    }

    if (i == ARRAY_SIZE(p_profile->presets))
    {
        return NULL;
    }

    return &p_profile->presets[i];
}

ONVIF_PTZPreset * onvif_get_idle_PTZPreset(const char * profile_token)
{
    int i;
    ONVIF_PROFILE * p_profile = onvif_find_profile(profile_token);
    if (NULL == p_profile)
    {
        return NULL;
    }

    for (i = 0; i < ARRAY_SIZE(p_profile->presets); i++)
    {
        if (p_profile->presets[i].UsedFlag == 0)
        {
            break;
        }
    }

    if (i == ARRAY_SIZE(p_profile->presets))
    {
        return NULL;
    }

    return &p_profile->presets[i];
}


/**
 * init PTZ node
 */
void onvif_init_PTZNode()
{
    // todo : init one ptz node
    
	g_onvif_cfg.ptz_node = (ONVIF_PTZNode *) malloc(sizeof(ONVIF_PTZNode));
	if (NULL == g_onvif_cfg.ptz_node)
	{
		return;
	}

	memset(g_onvif_cfg.ptz_node, 0, sizeof(ONVIF_PTZNode));

	strcpy(g_onvif_cfg.ptz_node->PTZNode.Name, "PTZNODE_000");
    strcpy(g_onvif_cfg.ptz_node->PTZNode.token, "PTZNODE_000");

    g_onvif_cfg.ptz_node->PTZNode.NameFlag = 1;
    
    g_onvif_cfg.ptz_node->PTZNode.SupportedPTZSpaces.AbsolutePanTiltPositionSpaceFlag = 1;
    g_onvif_cfg.ptz_node->PTZNode.SupportedPTZSpaces.AbsolutePanTiltPositionSpace.XRange.Min = -1.0;
    g_onvif_cfg.ptz_node->PTZNode.SupportedPTZSpaces.AbsolutePanTiltPositionSpace.XRange.Max = 1.0;
    g_onvif_cfg.ptz_node->PTZNode.SupportedPTZSpaces.AbsolutePanTiltPositionSpace.YRange.Min = -1.0;
    g_onvif_cfg.ptz_node->PTZNode.SupportedPTZSpaces.AbsolutePanTiltPositionSpace.YRange.Max = 1.0;
    
    g_onvif_cfg.ptz_node->PTZNode.SupportedPTZSpaces.AbsoluteZoomPositionSpaceFlag = 1;
    g_onvif_cfg.ptz_node->PTZNode.SupportedPTZSpaces.AbsoluteZoomPositionSpace.XRange.Min = 0.0;
    g_onvif_cfg.ptz_node->PTZNode.SupportedPTZSpaces.AbsoluteZoomPositionSpace.XRange.Max = 1.0;

    g_onvif_cfg.ptz_node->PTZNode.SupportedPTZSpaces.RelativePanTiltTranslationSpaceFlag = 1;
    g_onvif_cfg.ptz_node->PTZNode.SupportedPTZSpaces.RelativePanTiltTranslationSpace.XRange.Min = -1.0;
    g_onvif_cfg.ptz_node->PTZNode.SupportedPTZSpaces.RelativePanTiltTranslationSpace.XRange.Max = 1.0;
    g_onvif_cfg.ptz_node->PTZNode.SupportedPTZSpaces.RelativePanTiltTranslationSpace.YRange.Min = -1.0;
    g_onvif_cfg.ptz_node->PTZNode.SupportedPTZSpaces.RelativePanTiltTranslationSpace.YRange.Max = 1.0;

    g_onvif_cfg.ptz_node->PTZNode.SupportedPTZSpaces.RelativeZoomTranslationSpaceFlag = 1;
    g_onvif_cfg.ptz_node->PTZNode.SupportedPTZSpaces.RelativeZoomTranslationSpace.XRange.Min = -1.0;
    g_onvif_cfg.ptz_node->PTZNode.SupportedPTZSpaces.RelativeZoomTranslationSpace.XRange.Max = 1.0;

    g_onvif_cfg.ptz_node->PTZNode.SupportedPTZSpaces.ContinuousPanTiltVelocitySpaceFlag = 1;
    g_onvif_cfg.ptz_node->PTZNode.SupportedPTZSpaces.ContinuousPanTiltVelocitySpace.XRange.Min = -1.0;
    g_onvif_cfg.ptz_node->PTZNode.SupportedPTZSpaces.ContinuousPanTiltVelocitySpace.XRange.Max = 1.0;
    g_onvif_cfg.ptz_node->PTZNode.SupportedPTZSpaces.ContinuousPanTiltVelocitySpace.YRange.Min = -1.0;
    g_onvif_cfg.ptz_node->PTZNode.SupportedPTZSpaces.ContinuousPanTiltVelocitySpace.YRange.Max = 1.0;

    g_onvif_cfg.ptz_node->PTZNode.SupportedPTZSpaces.ContinuousZoomVelocitySpaceFlag = 1;
    g_onvif_cfg.ptz_node->PTZNode.SupportedPTZSpaces.ContinuousZoomVelocitySpace.XRange.Min = -1.0;
    g_onvif_cfg.ptz_node->PTZNode.SupportedPTZSpaces.ContinuousZoomVelocitySpace.XRange.Max = 1.0;

    g_onvif_cfg.ptz_node->PTZNode.SupportedPTZSpaces.PanTiltSpeedSpaceFlag = 1;
    g_onvif_cfg.ptz_node->PTZNode.SupportedPTZSpaces.PanTiltSpeedSpace.XRange.Min = 0.0;
    g_onvif_cfg.ptz_node->PTZNode.SupportedPTZSpaces.PanTiltSpeedSpace.XRange.Max = 1.0;

    g_onvif_cfg.ptz_node->PTZNode.SupportedPTZSpaces.ZoomSpeedSpaceFlag = 1;
    g_onvif_cfg.ptz_node->PTZNode.SupportedPTZSpaces.ZoomSpeedSpace.XRange.Min = 0.0;
    g_onvif_cfg.ptz_node->PTZNode.SupportedPTZSpaces.ZoomSpeedSpace.XRange.Max = 1.0;

    g_onvif_cfg.ptz_node->PTZNode.MaximumNumberOfPresets = MAX_PTZ_PRESETS;
    g_onvif_cfg.ptz_node->PTZNode.HomeSupported = TRUE;
    
    g_onvif_cfg.ptz_node->PTZNode.ExtensionFlag = 1;
    g_onvif_cfg.ptz_node->PTZNode.Extension.SupportedPresetTourFlag = 1;
    g_onvif_cfg.ptz_node->PTZNode.Extension.SupportedPresetTour.MaximumNumberOfPresetTours = 1;
    g_onvif_cfg.ptz_node->PTZNode.Extension.SupportedPresetTour.PTZPresetTourOperation_Start = 1;
    g_onvif_cfg.ptz_node->PTZNode.Extension.SupportedPresetTour.PTZPresetTourOperation_Stop = 1;
    g_onvif_cfg.ptz_node->PTZNode.Extension.SupportedPresetTour.PTZPresetTourOperation_Pause = 1;
    g_onvif_cfg.ptz_node->PTZNode.Extension.SupportedPresetTour.PTZPresetTourOperation_Extended = 0;

    g_onvif_cfg.ptz_node->PTZNode.FixedHomePositionFlag = 1;
    g_onvif_cfg.ptz_node->PTZNode.FixedHomePosition = FALSE;
}

/**
 * init ptz configuration
 */
void onvif_init_PTZConfiguration()
{
    g_onvif_cfg.ptz_cfg = (ONVIF_PTZConfiguration *) malloc(sizeof(ONVIF_PTZConfiguration));
	if (NULL == g_onvif_cfg.ptz_cfg)
	{
		return;
	}

	memset(g_onvif_cfg.ptz_cfg, 0, sizeof(ONVIF_PTZConfiguration));

	strcpy(g_onvif_cfg.ptz_cfg->Configuration.Name, "PTZCFG_000");
    strcpy(g_onvif_cfg.ptz_cfg->Configuration.token, "PTZCFG_000");

    assert(g_onvif_cfg.ptz_node);
    strcpy(g_onvif_cfg.ptz_cfg->Configuration.NodeToken, g_onvif_cfg.ptz_node->PTZNode.token);
    
    g_onvif_cfg.ptz_cfg->Configuration.DefaultPTZSpeedFlag = 1;
    g_onvif_cfg.ptz_cfg->Configuration.DefaultPTZSpeed.PanTiltFlag = 1;
    g_onvif_cfg.ptz_cfg->Configuration.DefaultPTZSpeed.PanTilt.x = 0.5;
    g_onvif_cfg.ptz_cfg->Configuration.DefaultPTZSpeed.PanTilt.y = 0.5;
    g_onvif_cfg.ptz_cfg->Configuration.DefaultPTZSpeed.ZoomFlag = 1;
    g_onvif_cfg.ptz_cfg->Configuration.DefaultPTZSpeed.Zoom.x = 0.5;

    g_onvif_cfg.ptz_cfg->Configuration.DefaultPTZTimeoutFlag = 1;
    g_onvif_cfg.ptz_cfg->Configuration.DefaultPTZTimeout = 5;

    g_onvif_cfg.ptz_cfg->Configuration.PanTiltLimitsFlag = 1;
    g_onvif_cfg.ptz_cfg->Configuration.PanTiltLimits.XRange.Min = -1.0;
    g_onvif_cfg.ptz_cfg->Configuration.PanTiltLimits.XRange.Max = 1.0;
    g_onvif_cfg.ptz_cfg->Configuration.PanTiltLimits.YRange.Min = -1.0;
    g_onvif_cfg.ptz_cfg->Configuration.PanTiltLimits.YRange.Max = 1.0;

    g_onvif_cfg.ptz_cfg->Configuration.ZoomLimitsFlag = 1;
    g_onvif_cfg.ptz_cfg->Configuration.ZoomLimits.XRange.Min = 0;
    g_onvif_cfg.ptz_cfg->Configuration.ZoomLimits.XRange.Max = 1.0;

    g_onvif_cfg.ptz_cfg->Configuration.ExtensionFlag = 1;
    g_onvif_cfg.ptz_cfg->Configuration.Extension.PTControlDirectionFlag = 1;
    g_onvif_cfg.ptz_cfg->Configuration.Extension.PTControlDirection.EFlipFlag = 1;
    g_onvif_cfg.ptz_cfg->Configuration.Extension.PTControlDirection.EFlip = EFlipMode_OFF;
    g_onvif_cfg.ptz_cfg->Configuration.Extension.PTControlDirection.ReverseFlag = 1;
    g_onvif_cfg.ptz_cfg->Configuration.Extension.PTControlDirection.Reverse= ReverseMode_OFF;
}

void onvif_init_PTZConfigurationOptions()
{
    g_onvif_cfg.PTZConfigurationOptions.PTZTimeout.Min = 1;
    g_onvif_cfg.PTZConfigurationOptions.PTZTimeout.Max = 100;

    g_onvif_cfg.PTZConfigurationOptions.PTControlDirectionFlag = 1;
    g_onvif_cfg.PTZConfigurationOptions.PTControlDirection.EFlipMode_OFF = 1;
    g_onvif_cfg.PTZConfigurationOptions.PTControlDirection.EFlipMode_ON = 1;
    g_onvif_cfg.PTZConfigurationOptions.PTControlDirection.ReverseMode_OFF = 1;
    g_onvif_cfg.PTZConfigurationOptions.PTControlDirection.ReverseMode_ON = 1;
    g_onvif_cfg.PTZConfigurationOptions.PTControlDirection.ReverseMode_AUTO = 1;
}

void onvif_init_ptz()
{
    onvif_init_PTZNode();

    onvif_init_PTZConfiguration();

    onvif_init_PTZConfigurationOptions();
}

#endif // end of PTZ_SUPPORT

#ifdef VIDEO_ANALYTICS

ONVIF_Config * onvif_add_Config(ONVIF_Config ** p_head)
{
	ONVIF_Config * p_tmp;
	ONVIF_Config * p_new = (ONVIF_Config *) malloc(sizeof(ONVIF_Config));
	if (NULL == p_new)
	{
		return NULL;
	}

	memset(p_new, 0, sizeof(ONVIF_Config));

	p_tmp = *p_head;
	if (NULL == p_tmp)
	{
		*p_head = p_new;
	}
	else
	{
		while (p_tmp && p_tmp->next) p_tmp = p_tmp->next;

		p_tmp->next = p_new;
	}	

	return p_new;
}

void onvif_free_Config(ONVIF_Config * p_config)
{
	onvif_free_SimpleItems(&p_config->Config.Parameters.SimpleItem);
	onvif_free_ElementItems(&p_config->Config.Parameters.ElementItem);
}

void onvif_free_Configs(ONVIF_Config ** p_head)
{
	ONVIF_Config * p_next;
	ONVIF_Config * p_tmp = *p_head;

	while (p_tmp)
	{
		p_next = p_tmp->next;

		onvif_free_Config(p_tmp);
		
		free(p_tmp);
		p_tmp = p_next;
	}

	*p_head = NULL;
}

ONVIF_Config * onvif_find_Config(ONVIF_Config ** p_head, const char * name)
{
	ONVIF_Config * p_tmp = *p_head;
	while (p_tmp)
	{
		if (strcmp(p_tmp->Config.Name, name) == 0)
		{
			break;
		}

		p_tmp = p_tmp->next;
	}

	return p_tmp;
}

void onvif_remove_Config(ONVIF_Config ** p_head, ONVIF_Config * p_remove)
{
	BOOL found = FALSE;
	ONVIF_Config * p_prev = NULL;
	ONVIF_Config * p_cfg = *p_head;	
	
	while (p_cfg)
	{
		if (p_cfg == p_remove)
		{
			found = TRUE;
			break;
		}

		p_prev = p_cfg;
		p_cfg = p_cfg->next;
	}

	if (found)
	{
		if (NULL == p_prev)
		{
			*p_head = p_cfg->next;
		}
		else
		{
			p_prev->next = p_cfg->next;
		}

		onvif_free_Config(p_cfg);
		free(p_cfg);
	}
}

ONVIF_Config * onvif_get_prev_Config(ONVIF_Config ** p_head, ONVIF_Config * p_found)
{
	ONVIF_Config * p_prev = *p_head;
	
	if (p_found == *p_head)
	{
		return NULL;
	}

	while (p_prev)
	{
		if (p_prev->next == p_found)
		{
			break;
		}
		
		p_prev = p_prev->next;
	}

	return p_prev;
}

ONVIF_ConfigDescription * onvif_add_ConfigDescription(ONVIF_ConfigDescription ** p_head)
{
    ONVIF_ConfigDescription * p_tmp;
	ONVIF_ConfigDescription * p_new = (ONVIF_ConfigDescription *) malloc(sizeof(ONVIF_ConfigDescription));
	if (NULL == p_new)
	{
		return NULL;
	}

	memset(p_new, 0, sizeof(ONVIF_ConfigDescription));
   
	p_tmp = *p_head;
	if (NULL == p_tmp)
	{
		*p_head = p_new;
	}
	else
	{
		while (p_tmp && p_tmp->next) p_tmp = p_tmp->next;

		p_tmp->next = p_new;
	}	

	return p_new;
}

void onvif_free_ConfigDescriptions(ONVIF_ConfigDescription ** p_head)
{
    ONVIF_ConfigDescription * p_next;
	ONVIF_ConfigDescription * p_tmp = *p_head;

	while (p_tmp)
	{
		p_next = p_tmp->next;

        onvif_free_SimpleItemDescriptions(&p_tmp->ConfigDescription.Parameters.SimpleItemDescription);
        onvif_free_SimpleItemDescriptions(&p_tmp->ConfigDescription.Parameters.ElementItemDescription);
        
        onvif_free_ConfigDescription_Messages(&p_tmp->ConfigDescription.Messages);

        onvif_free_ConfigOptions(&p_tmp->RuleOptions);
        
		free(p_tmp);
		p_tmp = p_next;
	}

	*p_head = NULL;
}

ONVIF_ConfigDescription_Messages * onvif_add_ConfigDescription_Message(ONVIF_ConfigDescription_Messages ** p_head)
{
    ONVIF_ConfigDescription_Messages * p_tmp;
	ONVIF_ConfigDescription_Messages * p_new = (ONVIF_ConfigDescription_Messages *) malloc(sizeof(ONVIF_ConfigDescription_Messages));
	if (NULL == p_new)
	{
		return NULL;
	}

	memset(p_new, 0, sizeof(ONVIF_ConfigDescription_Messages));
   
	p_tmp = *p_head;
	if (NULL == p_tmp)
	{
		*p_head = p_new;
	}
	else
	{
		while (p_tmp && p_tmp->next) p_tmp = p_tmp->next;

		p_tmp->next = p_new;
	}	

	return p_new;
}

void onvif_free_ConfigDescription_Messages(ONVIF_ConfigDescription_Messages ** p_head)
{
    ONVIF_ConfigDescription_Messages * p_next;
	ONVIF_ConfigDescription_Messages * p_tmp = *p_head;

	while (p_tmp)
	{
		p_next = p_tmp->next;

		free(p_tmp);
		p_tmp = p_next;
	}

	*p_head = NULL;
}

ONVIF_ConfigOptions * onvif_add_ConfigOptions(ONVIF_ConfigOptions ** p_head)
{
    ONVIF_ConfigOptions * p_tmp;
	ONVIF_ConfigOptions * p_new = (ONVIF_ConfigOptions *) malloc(sizeof(ONVIF_ConfigOptions));
	if (NULL == p_new)
	{
		return NULL;
	}

	memset(p_new, 0, sizeof(ONVIF_ConfigOptions));
   
	p_tmp = *p_head;
	if (NULL == p_tmp)
	{
		*p_head = p_new;
	}
	else
	{
		while (p_tmp && p_tmp->next) p_tmp = p_tmp->next;

		p_tmp->next = p_new;
	}	

	return p_new;
}

void onvif_free_ConfigOptions(ONVIF_ConfigOptions ** p_head)
{
    ONVIF_ConfigOptions * p_next;
	ONVIF_ConfigOptions * p_tmp = *p_head;

	while (p_tmp)
	{
		p_next = p_tmp->next;

		free(p_tmp);
		p_tmp = p_next;
	}

	*p_head = NULL;
}

ONVIF_SimpleItemDescription * onvif_add_SimpleItemDescription(ONVIF_SimpleItemDescription ** p_head)
{
    ONVIF_SimpleItemDescription * p_tmp;
	ONVIF_SimpleItemDescription * p_new = (ONVIF_SimpleItemDescription *) malloc(sizeof(ONVIF_SimpleItemDescription));
	if (NULL == p_new)
	{
		return NULL;
	}

	memset(p_new, 0, sizeof(ONVIF_SimpleItemDescription));
   
	p_tmp = *p_head;
	if (NULL == p_tmp)
	{
		*p_head = p_new;
	}
	else
	{
		while (p_tmp && p_tmp->next) p_tmp = p_tmp->next;

		p_tmp->next = p_new;
	}	

	return p_new;
}

void onvif_free_SimpleItemDescriptions(ONVIF_SimpleItemDescription ** p_head)
{
    ONVIF_SimpleItemDescription * p_next;
	ONVIF_SimpleItemDescription * p_tmp = *p_head;

	while (p_tmp)
	{
		p_next = p_tmp->next;

		free(p_tmp);
		p_tmp = p_next;
	}

	*p_head = NULL;
}

ONVIF_VideoAnalyticsConfiguration * onvif_add_VideoAnalyticsConfiguration(ONVIF_VideoAnalyticsConfiguration ** p_head)
{
    ONVIF_VideoAnalyticsConfiguration * p_tmp;
	ONVIF_VideoAnalyticsConfiguration * p_new = (ONVIF_VideoAnalyticsConfiguration *) malloc(sizeof(ONVIF_VideoAnalyticsConfiguration));
	if (NULL == p_new)
	{
		return NULL;
	}

	memset(p_new, 0, sizeof(ONVIF_VideoAnalyticsConfiguration));

    snprintf(p_new->Configuration.token, ONVIF_TOKEN_LEN, "VideoAnalyticsToken%d", g_onvif_cls.va_idx);
	g_onvif_cls.va_idx++;
	
	p_tmp = *p_head;
	if (NULL == p_tmp)
	{
		*p_head = p_new;
	}
	else
	{
		while (p_tmp && p_tmp->next) p_tmp = p_tmp->next;

		p_tmp->next = p_new;
	}	

	return p_new;
}

ONVIF_VideoAnalyticsConfiguration * onvif_find_VideoAnalyticsConfiguration(const char * token)
{
	ONVIF_VideoAnalyticsConfiguration * p_tmp = g_onvif_cfg.va_cfg;
	while (p_tmp)
	{
		if (strcmp(p_tmp->Configuration.token, token) == 0)
		{
			break;
		}

		p_tmp = p_tmp->next;
	}

	return p_tmp;
}

void onvif_free_VideoAnalyticsConfigurations(ONVIF_VideoAnalyticsConfiguration ** p_head)
{
    ONVIF_VideoAnalyticsConfiguration * p_next;
	ONVIF_VideoAnalyticsConfiguration * p_tmp = *p_head;

	while (p_tmp)
	{
		p_next = p_tmp->next;

        onvif_free_Configs(&p_tmp->Configuration.AnalyticsEngineConfiguration.AnalyticsModule);
        onvif_free_Configs(&p_tmp->Configuration.RuleEngineConfiguration.Rule);

        onvif_free_ConfigDescriptions(&p_tmp->SupportedRules.RuleDescription);

		free(p_tmp);
		p_tmp = p_next;
	}

	*p_head = NULL;
}

void onvif_init_VideoAnalyticsConfiguration()
{
    // todo : here init video analytics configurations ...
    
	ONVIF_Config * p_config;
	ONVIF_ConfigDescription * p_rule;
	ONVIF_ConfigOptions * p_options;
	ONVIF_SimpleItem * p_simpleitem;
	ONVIF_SimpleItemDescription * p_desc;
	ONVIF_ConfigDescription_Messages * p_message;
	
	ONVIF_VideoAnalyticsConfiguration * p_va_cfg = onvif_add_VideoAnalyticsConfiguration(&g_onvif_cfg.va_cfg);
	if (NULL == p_va_cfg)
	{
		return;
	}

	memset(p_va_cfg, 0, sizeof(ONVIF_VideoAnalyticsConfiguration));

	strcpy(p_va_cfg->Configuration.Name, "VideoAnalytics");
	p_va_cfg->Configuration.UseCount = 0;
	sprintf(p_va_cfg->Configuration.token, "AnalyticsToken%d", g_onvif_cls.va_idx++);

	// todo : here init analytics engine configuration ...
	p_config = onvif_add_Config(&p_va_cfg->Configuration.AnalyticsEngineConfiguration.AnalyticsModule);
	if (p_config)
	{
		memset(p_config, 0, sizeof(ONVIF_Config));

		strcpy(p_config->Config.Name, "MyMotionRegionDetector");
		strcpy(p_config->Config.Type, "tt:MotionRegionDetector");

		p_simpleitem = onvif_add_SimpleItem(&p_config->Config.Parameters.SimpleItem);
		if (p_simpleitem)
		{
		    strcpy(p_simpleitem->SimpleItem.Name, "Sensitivity");
		    strcpy(p_simpleitem->SimpleItem.Value, "60");
		}
	}

	// todo : here init rule engine configuration ...
	p_config = onvif_add_Config(&p_va_cfg->Configuration.RuleEngineConfiguration.Rule);
	if (p_config)
	{
		memset(p_config, 0, sizeof(ONVIF_Config));

		strcpy(p_config->Config.Name, "MyMotionRegionDetector");
		strcpy(p_config->Config.Type, "tt:MotionRegionDetector");

		p_simpleitem = onvif_add_SimpleItem(&p_config->Config.Parameters.SimpleItem);
		if (p_simpleitem)
		{
		    strcpy(p_simpleitem->SimpleItem.Name, "Sensitivity");
		    strcpy(p_simpleitem->SimpleItem.Value, "60");
		}
	}

	// todo : here init supported rules ...
	p_va_cfg->SupportedRules.sizeRuleContentSchemaLocation = 0;
	
	p_rule = onvif_add_ConfigDescription(&p_va_cfg->SupportedRules.RuleDescription);
	if (NULL == p_rule)
	{
	    return;
	}

    strcpy(p_rule->ConfigDescription.Name, "tt:MotionRegionDetector");

	p_desc = onvif_add_SimpleItemDescription(&p_rule->ConfigDescription.Parameters.ElementItemDescription);
	if (p_desc)
	{
	    strcpy(p_desc->SimpleItemDescription.Name, "MotionRegion");
	    strcpy(p_desc->SimpleItemDescription.Type, "axt:MotionRegionConfig");
	}

	p_message = onvif_add_ConfigDescription_Message(&p_rule->ConfigDescription.Messages);
	if (p_message)
	{
	    p_message->Messages.IsPropertyFlag = 1;
	    p_message->Messages.IsProperty = TRUE;
	    strcpy(p_message->Messages.ParentTopic, "tns1:RuleEngine/MotionRegionDetector/Motion");

	    p_message->Messages.SourceFlag = 1;
	    
	    p_desc = onvif_add_SimpleItemDescription(&p_message->Messages.Source.SimpleItemDescription);
	    if (p_desc)
	    {
	        strcpy(p_desc->SimpleItemDescription.Name, "VideoSource");
	        strcpy(p_desc->SimpleItemDescription.Type, "tt:ReferenceToken");
	    }

	    p_desc = onvif_add_SimpleItemDescription(&p_message->Messages.Source.SimpleItemDescription);
	    if (p_desc)
	    {
	        strcpy(p_desc->SimpleItemDescription.Name, "RuleName");
	        strcpy(p_desc->SimpleItemDescription.Type, "xs:string");
	    }

	    p_message->Messages.DataFlag = 1;
	    
	    p_desc = onvif_add_SimpleItemDescription(&p_message->Messages.Data.SimpleItemDescription);
	    if (p_desc)
	    {
	        strcpy(p_desc->SimpleItemDescription.Name, "State");
	        strcpy(p_desc->SimpleItemDescription.Type, "xs:boolean");
	    }
	}

	p_options = onvif_add_ConfigOptions(&p_rule->RuleOptions);
	if (p_options)
	{
	    strcpy(p_options->Options.RuleType, "axt:MotionRegionConfigOptions");
	    strcpy(p_options->Options.Name, "MotionRegion");
	    strcpy(p_options->Options.Type, "axt:MotionRegionConfigOptions");

	    p_options->Options.any = (char *) malloc(1024);

	    strcpy(p_options->Options.any, 
	        "<axt:MotionRegionConfigOptions>"
	        "</axt:MotionRegionConfigOptions>");
	}	
}

#endif	// end of VIDEO_ANALYTICS

#ifdef PROFILE_G_SUPPORT

ONVIF_Recording * onvif_add_Recording()
{
    ONVIF_Recording * p_tmp;
    ONVIF_Recording * p_new = (ONVIF_Recording *) malloc(sizeof(ONVIF_Recording));
	if (NULL == p_new)
	{
		return NULL;
	}

	memset(p_new, 0, sizeof(ONVIF_Recording));
	
	snprintf(p_new->Recording.RecordingToken, ONVIF_TOKEN_LEN, "RECORDING_00%d", g_onvif_cls.recording_idx);
	g_onvif_cls.recording_idx++;

	p_tmp = g_onvif_cfg.recordings;
	if (NULL == p_tmp)
	{
		g_onvif_cfg.recordings = p_new;
	}
	else
	{
		while (p_tmp && p_tmp->next) p_tmp = p_tmp->next;

		p_tmp->next = p_new;
	}	

	return p_new;
}

ONVIF_Recording * onvif_find_Recording(const char * token)
{
	ONVIF_Recording * p_tmp = g_onvif_cfg.recordings;
	while (p_tmp)
	{
		if (strcmp(p_tmp->Recording.RecordingToken, token) == 0)
		{
			break;
		}

		p_tmp = p_tmp->next;
	}

	return p_tmp;
}

void onvif_free_Recording(ONVIF_Recording * p_recording)
{
    ONVIF_Recording * p_prev;
    
    p_prev = g_onvif_cfg.recordings;
	if (p_recording == p_prev)
	{
		g_onvif_cfg.recordings = p_recording->next;
	}
	else
	{
		while (p_prev->next)
		{
			if (p_prev->next == p_recording)
			{
				break;
			}

			p_prev = p_prev->next;
		}

		p_prev->next = p_recording->next;
	}
	
	onvif_free_Tracks(&p_recording->Recording.Tracks);

	free(p_recording);
}

ONVIF_Track * onvif_add_Track(ONVIF_Track ** p_head)
{
	ONVIF_Track * p_tmp;
	ONVIF_Track * p_new = (ONVIF_Track *) malloc(sizeof(ONVIF_Track));
	if (NULL == p_new)
	{
		return NULL;
	}

	memset(p_new, 0, sizeof(ONVIF_Track));

    snprintf(p_new->Track.TrackToken, ONVIF_TOKEN_LEN, "TRACK00%d", g_onvif_cls.track_idx);
	g_onvif_cls.track_idx++;
	
	p_tmp = *p_head;
	if (NULL == p_tmp)
	{
		*p_head = p_new;
	}
	else
	{
		while (p_tmp && p_tmp->next) p_tmp = p_tmp->next;

		p_tmp->next = p_new;
	}	

	return p_new;
}

void onvif_free_Track(ONVIF_Track ** p_head, ONVIF_Track * p_track)
{
    ONVIF_Track * p_prev;
    
	p_prev = *p_head;
	if (p_track == p_prev)
	{
		*p_head = p_track->next;
	}
	else
	{
		while (p_prev->next)
		{
			if (p_prev->next == p_track)
			{
				break;
			}

			p_prev = p_prev->next;
		}

		p_prev->next = p_track->next;
	}

	free(p_track);
}

void onvif_free_Tracks(ONVIF_Track ** p_head)
{
	ONVIF_Track * p_next;
	ONVIF_Track * p_tmp = *p_head;

	while (p_tmp)
	{
		p_next = p_tmp->next;

		free(p_tmp);
		p_tmp = p_next;
	}

	*p_head = NULL;
}

ONVIF_Track * onvif_find_Track(ONVIF_Track * p_head, const char * token)
{
	ONVIF_Track * p_tmp = p_head;
	while (p_tmp)
	{
		if (strcmp(p_tmp->Track.TrackToken, token) == 0)
		{
			break;
		}

		p_tmp = p_tmp->next;
	}

	return p_tmp;
}

int	onvif_get_track_nums_by_type(ONVIF_Track * p_head, onvif_TrackType type)
{
	int nums = 0;
	
	ONVIF_Track * p_track = p_head;
	while (p_track)
	{
		if (p_track->Track.Configuration.TrackType == type)
		{
			nums++;
		}

		p_track = p_track->next;
	}

	return nums;
}

ONVIF_RecordingJob * onvif_add_RecordingJob()
{
    ONVIF_RecordingJob * p_tmp;
    ONVIF_RecordingJob * p_new;
    
    p_new = (ONVIF_RecordingJob *) malloc(sizeof(ONVIF_RecordingJob));
	if (NULL == p_new)
	{
		return NULL;
	}

	memset(p_new, 0, sizeof(ONVIF_RecordingJob));

	snprintf(p_new->RecordingJob.JobToken, ONVIF_TOKEN_LEN, "RECORDINGJOB_00%d", g_onvif_cls.recordingjob_idx);
	g_onvif_cls.recordingjob_idx++;

	p_tmp = g_onvif_cfg.recording_jobs;
	if (NULL == p_tmp)
	{
		g_onvif_cfg.recording_jobs = p_new;
	}
	else
	{
		while (p_tmp && p_tmp->next) p_tmp = p_tmp->next;

		p_tmp->next = p_new;
	}

	return p_new;
}

ONVIF_RecordingJob * onvif_find_RecordingJob(const char * token)
{
	ONVIF_RecordingJob * p_tmp = g_onvif_cfg.recording_jobs;
	while (p_tmp)
	{
		if (strcmp(p_tmp->RecordingJob.JobToken, token) == 0)
		{
			break;
		}

		p_tmp = p_tmp->next;
	}

	return p_tmp;
}

void onvif_free_RecordingJob(ONVIF_RecordingJob * p_head)
{
    ONVIF_RecordingJob * p_prev;
    
    if (NULL == p_head)
    {
        return;
    }
    
    p_prev = g_onvif_cfg.recording_jobs;
	if (p_head == p_prev)
	{
		g_onvif_cfg.recording_jobs = p_head->next;
	}
	else
	{
		while (p_prev->next)
		{
			if (p_prev->next == p_head)
			{
				break;
			}

			p_prev = p_prev->next;
		}

		p_prev->next = p_head->next;
	}

	free(p_head);
}

void onvif_init_Recording()
{    
    ONVIF_Recording * p_recording = onvif_add_Recording();
    if (p_recording)
    {
        ONVIF_Track * p_track;

        strcpy(p_recording->Recording.RecordingToken, "");
        
        strcpy(p_recording->Recording.Configuration.Source.SourceId, "http://localhost/sourceID");
        strcpy(p_recording->Recording.Configuration.Source.Name, "CameraName");
        strcpy(p_recording->Recording.Configuration.Source.Location, "LocationDescription");
        strcpy(p_recording->Recording.Configuration.Source.Description, "SourceDescription");
        strcpy(p_recording->Recording.Configuration.Source.Address, "http://localhost/address");

        strcpy(p_recording->Recording.Configuration.Content, "Recording from device");
        p_recording->Recording.Configuration.MaximumRetentionTimeFlag = 1;
        p_recording->Recording.Configuration.MaximumRetentionTime = 0;

        p_track = onvif_add_Track(&p_recording->Recording.Tracks);
    	if (p_track)
    	{
    		strcpy(p_track->Track.TrackToken, "VIDEO001");
    		p_track->Track.Configuration.TrackType = TrackType_Video;
    	}	
    	
    	p_track = onvif_add_Track(&p_recording->Recording.Tracks);
    	if (p_track)
    	{
    		strcpy(p_track->Track.TrackToken, "AUDIO001");
    		p_track->Track.Configuration.TrackType = TrackType_Audio;
    	}
    	
    	p_track = onvif_add_Track(&p_recording->Recording.Tracks);
    	if (p_track)
    	{
    		strcpy(p_track->Track.TrackToken, "META001");
    		p_track->Track.Configuration.TrackType = TrackType_Metadata;
    	}
    }
}

void onvif_init_RecordingJob()
{
    ONVIF_RecordingJob * p_recordingjob = onvif_add_RecordingJob();
    if (p_recordingjob)
    {
        strcpy(p_recordingjob->RecordingJob.JobConfiguration.Mode, "Active");
        p_recordingjob->RecordingJob.JobConfiguration.Priority = 1;

        if (g_onvif_cfg.profiles)
        {
            p_recordingjob->RecordingJob.JobConfiguration.sizeSource = 1;

            p_recordingjob->RecordingJob.JobConfiguration.Source[0].SourceTokenFlag = 1;
            
            p_recordingjob->RecordingJob.JobConfiguration.Source[0].SourceToken.TypeFlag = 1;
            strcpy(p_recordingjob->RecordingJob.JobConfiguration.Source[0].SourceToken.Type, "http://www.onvif.org/ver10/schema/Profile");
            strcpy(p_recordingjob->RecordingJob.JobConfiguration.Source[0].SourceToken.Token, g_onvif_cfg.profiles->token);

            p_recordingjob->RecordingJob.JobConfiguration.Source[0].sizeTracks = 1;
            strcpy(p_recordingjob->RecordingJob.JobConfiguration.Source[0].Tracks[0].SourceTag, "SourceTag");
            strcpy(p_recordingjob->RecordingJob.JobConfiguration.Source[0].Tracks[0].Destination, "VIDEO001");
        }
    }
}

ONVIF_RecordingInformation * onvif_add_RecordingInformation(ONVIF_RecordingInformation ** p_head)
{
    ONVIF_RecordingInformation * p_tmp;
	ONVIF_RecordingInformation * p_new = (ONVIF_RecordingInformation *) malloc(sizeof(ONVIF_RecordingInformation));
	if (NULL == p_new)
	{
		return NULL;
	}

	memset(p_new, 0, sizeof(ONVIF_RecordingInformation));

	p_tmp = *p_head;
	if (NULL == p_tmp)
	{
		*p_head = p_new;
	}
	else
	{
		while (p_tmp && p_tmp->next) p_tmp = p_tmp->next;

		p_tmp->next = p_new;
	}	

	return p_new;
}

void onvif_free_RecordingInformations(ONVIF_RecordingInformation ** p_head)
{
    ONVIF_RecordingInformation * p_next;
	ONVIF_RecordingInformation * p_tmp = *p_head;

	while (p_tmp)
	{
		p_next = p_tmp->next;

		free(p_tmp);
		p_tmp = p_next;
	}

	*p_head = NULL;
}

ONVIF_FindEventResult * onvif_add_FindEventResult(ONVIF_FindEventResult ** p_head)
{
    ONVIF_FindEventResult * p_tmp;
	ONVIF_FindEventResult * p_new = (ONVIF_FindEventResult *) malloc(sizeof(ONVIF_FindEventResult));
	if (NULL == p_new)
	{
		return NULL;
	}

	memset(p_new, 0, sizeof(ONVIF_FindEventResult));

	p_tmp = *p_head;
	if (NULL == p_tmp)
	{
		*p_head = p_new;
	}
	else
	{
		while (p_tmp && p_tmp->next) p_tmp = p_tmp->next;

		p_tmp->next = p_new;
	}	

	return p_new;
}

void onvif_free_FindEventResults(ONVIF_FindEventResult ** p_head)
{
    ONVIF_FindEventResult * p_next;
	ONVIF_FindEventResult * p_tmp = *p_head;

	while (p_tmp)
	{
		p_next = p_tmp->next;

		free(p_tmp);
		p_tmp = p_next;
	}

	*p_head = NULL;
}

ONVIF_FindMetadataResult * onvif_add_FindMetadataResult(ONVIF_FindMetadataResult ** p_head)
{
    ONVIF_FindMetadataResult * p_tmp;
	ONVIF_FindMetadataResult * p_new = (ONVIF_FindMetadataResult *) malloc(sizeof(ONVIF_FindMetadataResult));
	if (NULL == p_new)
	{
		return NULL;
	}

	memset(p_new, 0, sizeof(ONVIF_FindMetadataResult));

	p_tmp = *p_head;
	if (NULL == p_tmp)
	{
		*p_head = p_new;
	}
	else
	{
		while (p_tmp && p_tmp->next) p_tmp = p_tmp->next;

		p_tmp->next = p_new;
	}	

	return p_new;
}

void onvif_free_FindMetadataResults(ONVIF_FindMetadataResult ** p_head)
{
    ONVIF_FindMetadataResult * p_next;
	ONVIF_FindMetadataResult * p_tmp = *p_head;

	while (p_tmp)
	{
		p_next = p_tmp->next;

		free(p_tmp);
		p_tmp = p_next;
	}

	*p_head = NULL;
}

ONVIF_FindPTZPositionResult * onvif_add_FindPTZPositionResult(ONVIF_FindPTZPositionResult ** p_head)
{
    ONVIF_FindPTZPositionResult * p_tmp;
	ONVIF_FindPTZPositionResult * p_new = (ONVIF_FindPTZPositionResult *) malloc(sizeof(ONVIF_FindPTZPositionResult));
	if (NULL == p_new)
	{
		return NULL;
	}

	memset(p_new, 0, sizeof(ONVIF_FindPTZPositionResult));

	p_tmp = *p_head;
	if (NULL == p_tmp)
	{
		*p_head = p_new;
	}
	else
	{
		while (p_tmp && p_tmp->next) p_tmp = p_tmp->next;

		p_tmp->next = p_new;
	}	

	return p_new;
}

void onvif_free_FindPTZPositionResult(ONVIF_FindPTZPositionResult ** p_head)
{
    ONVIF_FindPTZPositionResult * p_next;
	ONVIF_FindPTZPositionResult * p_tmp = *p_head;

	while (p_tmp)
	{
		p_next = p_tmp->next;

		free(p_tmp);
		p_tmp = p_next;
	}

	*p_head = NULL;
}

#endif	// end of PROFILE_G_SUPPORT

#ifdef PROFILE_C_SUPPORT

ONVIF_AccessPoint * onvif_add_AccessPoint(ONVIF_AccessPoint ** p_head)
{
    ONVIF_AccessPoint * p_tmp;
	ONVIF_AccessPoint * p_new = (ONVIF_AccessPoint *) malloc(sizeof(ONVIF_AccessPoint));
	if (NULL == p_new)
	{
		return NULL;
	}

	memset(p_new, 0, sizeof(ONVIF_AccessPoint));

	p_tmp = *p_head;
	if (NULL == p_tmp)
	{
		*p_head = p_new;
	}
	else
	{
		while (p_tmp && p_tmp->next) p_tmp = p_tmp->next;

		p_tmp->next = p_new;
	}	

	return p_new;
}

ONVIF_AccessPoint * onvif_find_AccessPoint(const char * token)
{
    ONVIF_AccessPoint * p_tmp = g_onvif_cfg.access_points;  
    
    if (NULL == token)
    {
        return NULL;
    }

    while (p_tmp)
    {
        if (strcasecmp(token, p_tmp->AccessPointInfo.token) == 0)
        {
            return p_tmp;
        }
        
        p_tmp = p_tmp->next;
    }

    return NULL;
}

void onvif_free_AccessPoints(ONVIF_AccessPoint ** p_head)
{
    ONVIF_AccessPoint * p_next;
	ONVIF_AccessPoint * p_tmp = *p_head;

	while (p_tmp)
	{
		p_next = p_tmp->next;

		free(p_tmp);
		p_tmp = p_next;
	}

	*p_head = NULL;
}

ONVIF_Door * onvif_add_Door(ONVIF_Door ** p_head)
{
    ONVIF_Door * p_tmp;
	ONVIF_Door * p_new = (ONVIF_Door *) malloc(sizeof(ONVIF_Door));
	if (NULL == p_new)
	{
		return NULL;
	}

	memset(p_new, 0, sizeof(ONVIF_Door));

	p_tmp = *p_head;
	if (NULL == p_tmp)
	{
		*p_head = p_new;
	}
	else
	{
		while (p_tmp && p_tmp->next) p_tmp = p_tmp->next;

		p_tmp->next = p_new;
	}	

	return p_new;
}

ONVIF_Door * onvif_find_Door(const char * token)
{
    ONVIF_Door * p_tmp = g_onvif_cfg.doors; 
    
    if (NULL == token)
    {
        return NULL;
    }

    while (p_tmp)
    {
        if (strcasecmp(token, p_tmp->DoorInfo.token) == 0)
        {
            return p_tmp;
        }
        
        p_tmp = p_tmp->next;
    }

    return NULL;
}

void onvif_free_Doors(ONVIF_Door ** p_head)
{
    ONVIF_Door * p_next;
	ONVIF_Door * p_tmp = *p_head;

	while (p_tmp)
	{
		p_next = p_tmp->next;

		free(p_tmp);
		p_tmp = p_next;
	}

	*p_head = NULL;
}

ONVIF_AreaInfo * onvif_add_AreaInfo(ONVIF_AreaInfo ** p_head)
{
    ONVIF_AreaInfo * p_tmp;
	ONVIF_AreaInfo * p_new = (ONVIF_AreaInfo *) malloc(sizeof(ONVIF_AreaInfo));
	if (NULL == p_new)
	{
		return NULL;
	}

	memset(p_new, 0, sizeof(ONVIF_AreaInfo));

	p_tmp = *p_head;
	if (NULL == p_tmp)
	{
		*p_head = p_new;
	}
	else
	{
		while (p_tmp && p_tmp->next) p_tmp = p_tmp->next;

		p_tmp->next = p_new;
	}	

	return p_new;
}

ONVIF_AreaInfo * onvif_find_AreaInfo(const char * token)
{
    ONVIF_AreaInfo * p_tmp = g_onvif_cfg.area_info;

    while (p_tmp)
    {
        if (strcasecmp(token, p_tmp->AreaInfo.token) == 0)
        {
            return p_tmp;
        }
        
        p_tmp = p_tmp->next;
    }

    return NULL;
}

void onvif_free_AreaInfos(ONVIF_AreaInfo ** p_new)
{
    ONVIF_AreaInfo * p_next;
	ONVIF_AreaInfo * p_tmp = *p_new;

	while (p_tmp)
	{
		p_next = p_tmp->next;

		free(p_tmp);
		p_tmp = p_next;
	}

	*p_new = NULL;
}

void onvif_init_AccessPoint(ONVIF_AccessPoint * p_accesspoint, ONVIF_Door * p_door, ONVIF_AreaInfo * p_area)
{
    p_accesspoint->Enabled = TRUE;
    
    sprintf(p_accesspoint->AccessPointInfo.token, "AC_TOKEN_%d", ++g_onvif_cls.aceess_point_idx);
    sprintf(p_accesspoint->AccessPointInfo.Name, "AC_NAME_%d", g_onvif_cls.aceess_point_idx);

    p_accesspoint->AccessPointInfo.DescriptionFlag = 1;
    sprintf(p_accesspoint->AccessPointInfo.Description, "Access point %d", g_onvif_cls.aceess_point_idx);

    if (p_area)
    {
        p_accesspoint->AccessPointInfo.AreaFromFlag = 1;
        strcpy(p_accesspoint->AccessPointInfo.AreaFrom, p_area->AreaInfo.token);

        p_area = p_area->next;
    }

    if (p_area)
    {
        p_accesspoint->AccessPointInfo.AreaToFlag = 1;
        strcpy(p_accesspoint->AccessPointInfo.AreaTo, p_area->AreaInfo.token);
    }

    if (p_door)
    {
        strcpy(p_accesspoint->AccessPointInfo.Entity, p_door->DoorInfo.token);

        p_accesspoint->AccessPointInfo.EntityTypeFlag = 1;
        strcpy(p_accesspoint->AccessPointInfo.EntityType, "tdc:Door");
    }

    p_accesspoint->AccessPointInfo.Capabilities.DisableAccessPoint = TRUE;
    p_accesspoint->AccessPointInfo.Capabilities.Duress = TRUE;
    p_accesspoint->AccessPointInfo.Capabilities.AnonymousAccess = TRUE;
    p_accesspoint->AccessPointInfo.Capabilities.AccessTaken = TRUE;
    p_accesspoint->AccessPointInfo.Capabilities.ExternalAuthorization = FALSE;
}

void onvif_init_AccessPointList()
{
    // here, init two access point for two door ...

    ONVIF_Door * p_door = g_onvif_cfg.doors;
    ONVIF_AreaInfo * p_area = g_onvif_cfg.area_info;
    
    ONVIF_AccessPoint * p_accesspoint = onvif_add_AccessPoint(&g_onvif_cfg.access_points);
    if (p_accesspoint)
    {
        onvif_init_AccessPoint(p_accesspoint, p_door, p_area);

        if (p_door)
        {
            p_door = p_door->next;
        }

        if (p_area)
        {
            p_area = p_area->next;
        }

        if (p_area)
        {
            p_area = p_area->next;
        }
    }

    p_accesspoint = onvif_add_AccessPoint(&g_onvif_cfg.access_points);
    if (p_accesspoint)
    {
        onvif_init_AccessPoint(p_accesspoint, p_door, p_area);
    }
}

void onvif_init_Door(ONVIF_Door * p_door)
{
    sprintf(p_door->DoorInfo.token, "DOOR_TOKEN_%d", ++g_onvif_cls.door_idx);
    sprintf(p_door->DoorInfo.Name, "DOOR_NAME_%d", g_onvif_cls.door_idx);

    p_door->DoorInfo.DescriptionFlag = 1;
    sprintf(p_door->DoorInfo.Description, "Door %d", g_onvif_cls.door_idx);

    p_door->DoorInfo.Capabilities.Access = TRUE;
    p_door->DoorInfo.Capabilities.AccessTimingOverride = TRUE;
    p_door->DoorInfo.Capabilities.Lock = TRUE;
    p_door->DoorInfo.Capabilities.Unlock = TRUE;
    p_door->DoorInfo.Capabilities.Block = TRUE;
    p_door->DoorInfo.Capabilities.DoubleLock = FALSE;
    p_door->DoorInfo.Capabilities.LockDown = TRUE;
    p_door->DoorInfo.Capabilities.LockOpen = TRUE;
    p_door->DoorInfo.Capabilities.DoorMonitor = TRUE;
    p_door->DoorInfo.Capabilities.LockMonitor = TRUE;
    p_door->DoorInfo.Capabilities.DoubleLockMonitor = FALSE;
    p_door->DoorInfo.Capabilities.Alarm = TRUE;
    p_door->DoorInfo.Capabilities.Tamper = FALSE;
    p_door->DoorInfo.Capabilities.Fault = TRUE;

    p_door->DoorState.DoorPhysicalStateFlag = 1;
    p_door->DoorState.DoorPhysicalState = DoorPhysicalState_Closed;
    p_door->DoorState.LockPhysicalStateFlag = 1;
    p_door->DoorState.LockPhysicalState = LockPhysicalState_Locked;
    p_door->DoorState.AlarmFlag = 1;
    p_door->DoorState.Alarm = DoorAlarmState_Normal;
    p_door->DoorState.FaultFlag = 1;
    p_door->DoorState.Fault.State = DoorFaultState_NotInFault;
    p_door->DoorState.DoorMode = DoorMode_Locked;
}

void onvif_init_DoorList()
{
    // here, init two door ...
    
    ONVIF_Door * p_door = onvif_add_Door(&g_onvif_cfg.doors);
    if (p_door)
    {
        onvif_init_Door(p_door);
    }

    p_door = onvif_add_Door(&g_onvif_cfg.doors);
    if (p_door)
    {
        onvif_init_Door(p_door);
    }
}

void onvif_init_AreaInfo(ONVIF_AreaInfo * p_info)
{
    sprintf(p_info->AreaInfo.token, "AREA_TOKEN_%d", ++g_onvif_cls.area_idx);
    sprintf(p_info->AreaInfo.Name, "AREA_NAME_%d", g_onvif_cls.area_idx);

    p_info->AreaInfo.DescriptionFlag = 1;
    sprintf(p_info->AreaInfo.Description, "Area %d", g_onvif_cls.area_idx);
}

void onvif_init_AreaInfoList()
{
    // here, init four area for two door ...
    
    ONVIF_AreaInfo * p_info = onvif_add_AreaInfo(&g_onvif_cfg.area_info);
    if (p_info)
    {
        onvif_init_AreaInfo(p_info);
    }

    p_info = onvif_add_AreaInfo(&g_onvif_cfg.area_info);
    if (p_info)
    {
        onvif_init_AreaInfo(p_info);
    }

    p_info = onvif_add_AreaInfo(&g_onvif_cfg.area_info);
    if (p_info)
    {
        onvif_init_AreaInfo(p_info);
    }

    p_info = onvif_add_AreaInfo(&g_onvif_cfg.area_info);
    if (p_info)
    {
        onvif_init_AreaInfo(p_info);
    }
}

#endif // end of PROFILE_C_SUPPORT

#ifdef DEVICEIO_SUPPORT

ONVIF_PaneLayout * onvif_add_PaneLayout(ONVIF_PaneLayout ** p_head)
{
    ONVIF_PaneLayout * p_tmp;
	ONVIF_PaneLayout * p_new = (ONVIF_PaneLayout *) malloc(sizeof(ONVIF_PaneLayout));
	if (NULL == p_new)
	{
		return NULL;
	}

	memset(p_new, 0, sizeof(ONVIF_PaneLayout));

	p_tmp = *p_head;
	if (NULL == p_tmp)
	{
		*p_head = p_new;
	}
	else
	{
		while (p_tmp && p_tmp->next) p_tmp = p_tmp->next;

		p_tmp->next = p_new;
	}	

	return p_new;
}

void onvif_free_PaneLayouts(ONVIF_PaneLayout ** p_head)
{
    ONVIF_PaneLayout * p_next;
	ONVIF_PaneLayout * p_tmp = *p_head;

	while (p_tmp)
	{
		p_next = p_tmp->next;

		free(p_tmp);
		p_tmp = p_next;
	}

	*p_head = NULL;
}

ONVIF_VideoOutput * onvif_add_VideoOutput(ONVIF_VideoOutput ** p_head)
{
    ONVIF_VideoOutput * p_tmp;
	ONVIF_VideoOutput * p_new = (ONVIF_VideoOutput *) malloc(sizeof(ONVIF_VideoOutput));
	if (NULL == p_new)
	{
		return NULL;
	}

	memset(p_new, 0, sizeof(ONVIF_VideoOutput));

    snprintf(p_new->VideoOutput.token, ONVIF_TOKEN_LEN, "VOUT_00%d", g_onvif_cls.v_out_idx);

    g_onvif_cls.v_out_idx++;

	p_tmp = *p_head;
	if (NULL == p_tmp)
	{
		*p_head = p_new;
	}
	else
	{
		while (p_tmp && p_tmp->next) p_tmp = p_tmp->next;

		p_tmp->next = p_new;
	}

	return p_new;
}

ONVIF_VideoOutput * onvif_find_VideoOutput(const char * token)
{
    ONVIF_VideoOutput * p_tmp = g_onvif_cfg.v_output;  
    
    if (NULL == token)
    {
        return NULL;
    }

    while (p_tmp)
    {
        if (strcasecmp(token, p_tmp->VideoOutput.token) == 0)
        {
            return p_tmp;
        }
        
        p_tmp = p_tmp->next;
    }

    return NULL;
}

void onvif_free_VideoOutputs(ONVIF_VideoOutput ** p_head)
{
    ONVIF_VideoOutput * p_next;
	ONVIF_VideoOutput * p_tmp = *p_head;

	while (p_tmp)
	{
		p_next = p_tmp->next;

        onvif_free_PaneLayouts(&p_tmp->VideoOutput.Layout.PaneLayout);
        
		free(p_tmp);
		p_tmp = p_next;
	}

	*p_head = NULL;
}

ONVIF_VideoOutputConfiguration * onvif_add_VideoOutputConfiguration(ONVIF_VideoOutputConfiguration ** p_head)
{
    ONVIF_VideoOutputConfiguration * p_tmp;
	ONVIF_VideoOutputConfiguration * p_new = (ONVIF_VideoOutputConfiguration *) malloc(sizeof(ONVIF_VideoOutputConfiguration));
	if (NULL == p_new)
	{
		return NULL;
	}

	memset(p_new, 0, sizeof(ONVIF_VideoOutputConfiguration));

    snprintf(p_new->Configuration.Name, ONVIF_NAME_LEN, "VOUT_NAME_00%d", g_onvif_cls.v_out_cfg_idx);
    snprintf(p_new->Configuration.token, ONVIF_TOKEN_LEN, "VOUT_CFG_00%d", g_onvif_cls.v_out_cfg_idx);

    g_onvif_cls.v_out_cfg_idx++;

	p_tmp = *p_head;
	if (NULL == p_tmp)
	{
		*p_head = p_new;
	}
	else
	{
		while (p_tmp && p_tmp->next) p_tmp = p_tmp->next;

		p_tmp->next = p_new;
	}

	return p_new;
}

ONVIF_VideoOutputConfiguration * onvif_find_VideoOutputConfiguration(const char * token)
{
    ONVIF_VideoOutputConfiguration * p_tmp = g_onvif_cfg.v_output_cfg;  
    
    if (NULL == token)
    {
        return NULL;
    }

    while (p_tmp)
    {
        if (strcasecmp(token, p_tmp->Configuration.token) == 0)
        {
            return p_tmp;
        }
        
        p_tmp = p_tmp->next;
    }

    return NULL;
}

ONVIF_VideoOutputConfiguration * onvif_find_VideoOutputConfiguration_by_OutputToken(const char * token)
{
    ONVIF_VideoOutputConfiguration * p_tmp = g_onvif_cfg.v_output_cfg;  
    
    if (NULL == token)
    {
        return NULL;
    }

    while (p_tmp)
    {
        if (strcasecmp(token, p_tmp->Configuration.OutputToken) == 0)
        {
            return p_tmp;
        }
        
        p_tmp = p_tmp->next;
    }

    return NULL;
}

void onvif_free_VideoOutputConfigurations(ONVIF_VideoOutputConfiguration ** p_head)
{
    ONVIF_VideoOutputConfiguration * p_next;
	ONVIF_VideoOutputConfiguration * p_tmp = *p_head;

	while (p_tmp)
	{
		p_next = p_tmp->next;
        
		free(p_tmp);
		p_tmp = p_next;
	}

	*p_head = NULL;
}

ONVIF_AudioOutput * onvif_add_AudioOutput(ONVIF_AudioOutput ** p_head)
{
    ONVIF_AudioOutput * p_tmp;
	ONVIF_AudioOutput * p_new = (ONVIF_AudioOutput *) malloc(sizeof(ONVIF_AudioOutput));
	if (NULL == p_new)
	{
		return NULL;
	}

	memset(p_new, 0, sizeof(ONVIF_AudioOutput));

    snprintf(p_new->AudioOutput.token, ONVIF_TOKEN_LEN, "AOUT_00%d", g_onvif_cls.a_out_idx);

    g_onvif_cls.a_out_idx++;

	p_tmp = *p_head;
	if (NULL == p_tmp)
	{
		*p_head = p_new;
	}
	else
	{
		while (p_tmp && p_tmp->next) p_tmp = p_tmp->next;

		p_tmp->next = p_new;
	}

	return p_new;
}

ONVIF_AudioOutput * onvif_find_AudioOutput(const char * token)
{
    ONVIF_AudioOutput * p_tmp = g_onvif_cfg.a_output;  
    
    if (NULL == token)
    {
        return NULL;
    }

    while (p_tmp)
    {
        if (strcasecmp(token, p_tmp->AudioOutput.token) == 0)
        {
            return p_tmp;
        }
        
        p_tmp = p_tmp->next;
    }

    return NULL;
}

void onvif_free_AudioOutputs(ONVIF_AudioOutput ** p_head)
{
    ONVIF_AudioOutput * p_next;
	ONVIF_AudioOutput * p_tmp = *p_head;

	while (p_tmp)
	{
		p_next = p_tmp->next;
        
		free(p_tmp);
		p_tmp = p_next;
	}

	*p_head = NULL;
}

ONVIF_AudioOutputConfiguration * onvif_add_AudioOutputConfiguration(ONVIF_AudioOutputConfiguration ** p_head)
{
    ONVIF_AudioOutputConfiguration * p_tmp;
	ONVIF_AudioOutputConfiguration * p_new = (ONVIF_AudioOutputConfiguration *) malloc(sizeof(ONVIF_AudioOutputConfiguration));
	if (NULL == p_new)
	{
		return NULL;
	}

	memset(p_new, 0, sizeof(ONVIF_AudioOutputConfiguration));

    snprintf(p_new->Configuration.Name, ONVIF_NAME_LEN, "AOUT_NAME_00%d", g_onvif_cls.a_out_cfg_idx);
    snprintf(p_new->Configuration.token, ONVIF_TOKEN_LEN, "AOUT_CFG_00%d", g_onvif_cls.a_out_cfg_idx);

    g_onvif_cls.a_out_cfg_idx++;

	p_tmp = *p_head;
	if (NULL == p_tmp)
	{
		*p_head = p_new;
	}
	else
	{
		while (p_tmp && p_tmp->next) p_tmp = p_tmp->next;

		p_tmp->next = p_new;
	}

	return p_new;
}

ONVIF_AudioOutputConfiguration * onvif_find_AudioOutputConfiguration(const char * token)
{
    ONVIF_AudioOutputConfiguration * p_tmp = g_onvif_cfg.a_output_cfg;  
    
    if (NULL == token)
    {
        return NULL;
    }

    while (p_tmp)
    {
        if (strcasecmp(token, p_tmp->Configuration.token) == 0)
        {
            return p_tmp;
        }
        
        p_tmp = p_tmp->next;
    }

    return NULL;
}

ONVIF_AudioOutputConfiguration * onvif_find_AudioOutputConfiguration_by_OutputToken(const char * token)
{
    ONVIF_AudioOutputConfiguration * p_tmp = g_onvif_cfg.a_output_cfg;  
    
    if (NULL == token)
    {
        return NULL;
    }

    while (p_tmp)
    {
        if (strcasecmp(token, p_tmp->Configuration.OutputToken) == 0)
        {
            return p_tmp;
        }
        
        p_tmp = p_tmp->next;
    }

    return NULL;
}

void onvif_free_AudioOutputConfigurations(ONVIF_AudioOutputConfiguration ** p_head)
{
    ONVIF_AudioOutputConfiguration * p_next;
	ONVIF_AudioOutputConfiguration * p_tmp = *p_head;

	while (p_tmp)
	{
		p_next = p_tmp->next;
        
		free(p_tmp);
		p_tmp = p_next;
	}

	*p_head = NULL;
}

ONVIF_RelayOutput * onvif_add_RelayOutput(ONVIF_RelayOutput ** p_head)
{
    ONVIF_RelayOutput * p_tmp;
	ONVIF_RelayOutput * p_new = (ONVIF_RelayOutput *) malloc(sizeof(ONVIF_RelayOutput));
	if (NULL == p_new)
	{
		return NULL;
	}

	memset(p_new, 0, sizeof(ONVIF_RelayOutput));

    snprintf(p_new->RelayOutput.token, ONVIF_TOKEN_LEN, "RELAY_OUTPUT_00%d", g_onvif_cls.relay_idx);

    g_onvif_cls.relay_idx++;

	p_tmp = *p_head;
	if (NULL == p_tmp)
	{
		*p_head = p_new;
	}
	else
	{
		while (p_tmp && p_tmp->next) p_tmp = p_tmp->next;

		p_tmp->next = p_new;
	}

	return p_new;
}

ONVIF_RelayOutput * onvif_find_RelayOutput(const char * token)
{
    ONVIF_RelayOutput * p_tmp = g_onvif_cfg.relay_output;  
    
    if (NULL == token)
    {
        return NULL;
    }

    while (p_tmp)
    {
        if (strcasecmp(token, p_tmp->RelayOutput.token) == 0)
        {
            return p_tmp;
        }
        
        p_tmp = p_tmp->next;
    }

    return NULL;
}

void onvif_free_RelayOutputs(ONVIF_RelayOutput ** p_head)
{
    ONVIF_RelayOutput * p_next;
	ONVIF_RelayOutput * p_tmp = *p_head;

	while (p_tmp)
	{
		p_next = p_tmp->next;
        
		free(p_tmp);
		p_tmp = p_next;
	}

	*p_head = NULL;
}

ONVIF_DigitalInput * onvif_add_DigitalInput(ONVIF_DigitalInput ** p_head)
{
    ONVIF_DigitalInput * p_tmp;
	ONVIF_DigitalInput * p_new = (ONVIF_DigitalInput *) malloc(sizeof(ONVIF_DigitalInput));
	if (NULL == p_new)
	{
		return NULL;
	}

	memset(p_new, 0, sizeof(ONVIF_DigitalInput));

    snprintf(p_new->DigitalInput.token, ONVIF_TOKEN_LEN, "DIGIT_INPUT_00%d", g_onvif_cls.digit_input_idx);

    g_onvif_cls.digit_input_idx++;

	p_tmp = *p_head;
	if (NULL == p_tmp)
	{
		*p_head = p_new;
	}
	else
	{
		while (p_tmp && p_tmp->next) p_tmp = p_tmp->next;

		p_tmp->next = p_new;
	}

	return p_new;
}

ONVIF_DigitalInput * onvif_find_DigitalInput(const char * token)
{
    ONVIF_DigitalInput * p_tmp = g_onvif_cfg.digit_input;  
    
    if (NULL == token)
    {
        return NULL;
    }

    while (p_tmp)
    {
        if (strcasecmp(token, p_tmp->DigitalInput.token) == 0)
        {
            return p_tmp;
        }
        
        p_tmp = p_tmp->next;
    }

    return NULL;
}

void onvif_free_DigitalInputs(ONVIF_DigitalInput ** p_head)
{
    ONVIF_DigitalInput * p_next;
	ONVIF_DigitalInput * p_tmp = *p_head;

	while (p_tmp)
	{
		p_next = p_tmp->next;
        
		free(p_tmp);
		p_tmp = p_next;
	}

	*p_head = NULL;
}

ONVIF_SerialPort * onvif_add_SerialPort(ONVIF_SerialPort ** p_head)
{
    ONVIF_SerialPort * p_tmp;
	ONVIF_SerialPort * p_new = (ONVIF_SerialPort *) malloc(sizeof(ONVIF_SerialPort));
	if (NULL == p_new)
	{
		return NULL;
	}

	memset(p_new, 0, sizeof(ONVIF_SerialPort));

    snprintf(p_new->SerialPort.token, ONVIF_TOKEN_LEN, "SERIAL_PORT_00%d", g_onvif_cls.serial_port_idx);

    g_onvif_cls.serial_port_idx++;

	p_tmp = *p_head;
	if (NULL == p_tmp)
	{
		*p_head = p_new;
	}
	else
	{
		while (p_tmp && p_tmp->next) p_tmp = p_tmp->next;

		p_tmp->next = p_new;
	}

	return p_new;
}

ONVIF_SerialPort * onvif_find_SerialPort(const char * token)
{
    ONVIF_SerialPort * p_tmp = g_onvif_cfg.serial_port;  
    
    if (NULL == token)
    {
        return NULL;
    }

    while (p_tmp)
    {
        if (strcasecmp(token, p_tmp->SerialPort.token) == 0)
        {
            return p_tmp;
        }
        
        p_tmp = p_tmp->next;
    }

    return NULL;
}

ONVIF_SerialPort * onvif_find_SerialPort_by_ConfigurationToken(const char * token)
{
    ONVIF_SerialPort * p_tmp = g_onvif_cfg.serial_port;  
    
    if (NULL == token)
    {
        return NULL;
    }

    while (p_tmp)
    {
        if (strcasecmp(token, p_tmp->Configuration.token) == 0)
        {
            return p_tmp;
        }
        
        p_tmp = p_tmp->next;
    }

    return NULL;
}

void onvif_free_SerialPorts(ONVIF_SerialPort ** p_head)
{
    ONVIF_SerialPort * p_next;
	ONVIF_SerialPort * p_tmp = *p_head;

	while (p_tmp)
	{
		p_next = p_tmp->next;
        
		free(p_tmp);
		p_tmp = p_next;
	}

	*p_head = NULL;
}

void onvif_malloc_SerialData(onvif_SerialData * p_data, int union_SerialData, int size)
{
    if (NULL == p_data)
    {
        return;
    }
    
    if (union_SerialData == 0)
    {
        p_data->_union_SerialData = 0;
        p_data->union_SerialData.Binary = (char *)malloc(size);
        memset(p_data->union_SerialData.Binary, 0, size);
    }
    else
    {
        p_data->_union_SerialData = 1;
        p_data->union_SerialData.String = (char *)malloc(size);
        memset(p_data->union_SerialData.String, 0, size);
    }
}

void  onvif_free_SerialData(onvif_SerialData * p_data)
{
    if (NULL == p_data)
    {
        return;
    }

    if (p_data->_union_SerialData == 0)
    {
        if (p_data->union_SerialData.Binary)
        {
            free(p_data->union_SerialData.Binary);
            p_data->union_SerialData.Binary = NULL;
        }
    }
    else
    {
        if (p_data->union_SerialData.String)
        {
            free(p_data->union_SerialData.String);
            p_data->union_SerialData.String = NULL;
        }
    }
}

#ifdef AUDIO_SUPPORT
void onvif_init_AudioOutput()
{
    ONVIF_AudioOutput * p_output = onvif_add_AudioOutput(&g_onvif_cfg.a_output);
    ONVIF_AudioOutputConfiguration * p_cfg = onvif_add_AudioOutputConfiguration(&g_onvif_cfg.a_output_cfg);

    strcpy(p_cfg->Configuration.OutputToken, p_output->AudioOutput.token);
    p_cfg->Configuration.SendPrimacyFlag = 1;
    strcpy(p_cfg->Configuration.SendPrimacy, "www.onvif.org/ver20/HalfDuplex/Server");
    p_cfg->Configuration.OutputLevel = 100;

    p_cfg->Options.sizeOutputTokensAvailable = 1;
    strcpy(p_cfg->Options.OutputTokensAvailable[0], p_output->AudioOutput.token);
    p_cfg->Options.sizeSendPrimacyOptions = 3;
    strcpy(p_cfg->Options.SendPrimacyOptions[0], "www.onvif.org/ver20/HalfDuplex/Server");
    strcpy(p_cfg->Options.SendPrimacyOptions[1], "www.onvif.org/ver20/HalfDuplex/Client");
    strcpy(p_cfg->Options.SendPrimacyOptions[2], "www.onvif.org/ver20/HalfDuplex/Auto");

    p_cfg->Options.OutputLevelRange.Min = 0;
    p_cfg->Options.OutputLevelRange.Max = 100;
}
#endif

void onvif_init_RelayOutput()
{
    ONVIF_RelayOutput * p_output = onvif_add_RelayOutput(&g_onvif_cfg.relay_output);

    p_output->RelayOutput.Properties.Mode = RelayMode_Monostable;
    p_output->RelayOutput.Properties.DelayTime = 10;
    p_output->RelayOutput.Properties.IdleState = RelayIdleState_closed;

    p_output->RelayLogicalState = RelayLogicalState_inactive;

    strcpy(p_output->Options.token, p_output->RelayOutput.token);
    
    p_output->Options.RelayMode_BistableFlag = 1;
    p_output->Options.RelayMode_MonostableFlag = 0;

    p_output->Options.DelayTimesFlag = 1;
    strcpy(p_output->Options.DelayTimes, "1 120");

    memcpy(&g_onvif_cfg.RelayOutputOptions, &p_output->Options, sizeof(onvif_RelayOutputOptions));
}

void onvif_init_DigitInput()
{
    ONVIF_DigitalInput * p_input = onvif_add_DigitalInput(&g_onvif_cfg.digit_input);

    p_input->DigitalInput.IdleStateFlag = 1;
    p_input->DigitalInput.IdleState = DigitalIdleState_closed;

    p_input->Options.DigitalIdleState_openFlag = 1;
    p_input->Options.DigitalIdleState_closedFlag = 1;

    g_onvif_cfg.DigitalInputConfigurationInputOptions.DigitalIdleState_closedFlag = 1;
    g_onvif_cfg.DigitalInputConfigurationInputOptions.DigitalIdleState_openFlag = 1;
}

void onvif_init_SerialPort()
{
    ONVIF_SerialPort * p_port = onvif_add_SerialPort(&g_onvif_cfg.serial_port);

    strcpy(p_port->Configuration.token, "SERIAL_PORT_CFG_000");
    p_port->Configuration.BaudRate = 112500;
    p_port->Configuration.CharacterLength = 8;
    p_port->Configuration.StopBit = 1;
    p_port->Configuration.ParityBit = ParityBit_Odd;
    p_port->Configuration.type = SerialPortType_RS232;

    strcpy(p_port->Options.token, p_port->SerialPort.token);
    
    p_port->Options.BaudRateList.sizeItems = 2;
    p_port->Options.BaudRateList.Items[0] = 112500;
    p_port->Options.BaudRateList.Items[1] = 98000;

    p_port->Options.CharacterLengthList.sizeItems = 1;
    p_port->Options.CharacterLengthList.Items[0] = 8;

    p_port->Options.ParityBitList.sizeItems = 2;
    p_port->Options.ParityBitList.Items[0] = ParityBit_Odd;
    p_port->Options.ParityBitList.Items[1] = ParityBit_Even;

    p_port->Options.StopBitList.sizeItems = 1;
    p_port->Options.StopBitList.Items[0] = 1;    
}

#endif // end of DEVICEIO_SUPPORT

#ifdef THERMAL_SUPPORT
ONVIF_ColorPalette * onvif_add_ColorPalette(ONVIF_ColorPalette ** p_head)
{
    ONVIF_ColorPalette * p_tmp;
	ONVIF_ColorPalette * p_new = (ONVIF_ColorPalette *) malloc(sizeof(ONVIF_ColorPalette));
	if (NULL == p_new)
	{
		return NULL;
	}

	memset(p_new, 0, sizeof(ONVIF_ColorPalette));

	p_tmp = *p_head;
	if (NULL == p_tmp)
	{
		*p_head = p_new;
	}
	else
	{
		while (p_tmp && p_tmp->next) p_tmp = p_tmp->next;

		p_tmp->next = p_new;
	}	

	return p_new;
}

void onvif_free_ColorPalettes(ONVIF_ColorPalette ** p_head)
{
    ONVIF_ColorPalette * p_next;
	ONVIF_ColorPalette * p_tmp = *p_head;

	while (p_tmp)
	{
		p_next = p_tmp->next;
        
		free(p_tmp);
		p_tmp = p_next;
	}

	*p_head = NULL;
}

ONVIF_NUCTable * onvif_add_NUCTable(ONVIF_NUCTable ** p_head)
{
    ONVIF_NUCTable * p_tmp;
	ONVIF_NUCTable * p_new = (ONVIF_NUCTable *) malloc(sizeof(ONVIF_NUCTable));
	if (NULL == p_new)
	{
		return NULL;
	}

	memset(p_new, 0, sizeof(ONVIF_NUCTable));

	p_tmp = *p_head;
	if (NULL == p_tmp)
	{
		*p_head = p_new;
	}
	else
	{
		while (p_tmp && p_tmp->next) p_tmp = p_tmp->next;

		p_tmp->next = p_new;
	}	

	return p_new;
}

void onvif_free_NUCTables(ONVIF_NUCTable ** p_head)
{
    ONVIF_NUCTable * p_next;
	ONVIF_NUCTable * p_tmp = *p_head;

	while (p_tmp)
	{
		p_next = p_tmp->next;
        
		free(p_tmp);
		p_tmp = p_next;
	}

	*p_head = NULL;
}

void onvif_init_ThermalConfiguration(onvif_ThermalConfiguration * p_req)
{
    strcpy(p_req->ColorPalette.token, "CP_TOKEN");
    strcpy(p_req->ColorPalette.Name, "CP_NAME");
    strcpy(p_req->ColorPalette.Type, "WhiteHot");

    p_req->Polarity = Polarity_WhiteHot;

    p_req->NUCTableFlag = 1;
    strcpy(p_req->NUCTable.token, "NUC_TOKEN");
    strcpy(p_req->NUCTable.Name, "NUC_NAME");
    p_req->NUCTable.LowTemperatureFlag = 1;
    p_req->NUCTable.LowTemperature = 0;
    p_req->NUCTable.HighTemperatureFlag = 1;
    p_req->NUCTable.HighTemperature = 100;

    p_req->CoolerFlag = 1;
    p_req->Cooler.Enabled = TRUE;
    p_req->Cooler.RunTimeFlag = 1;
    p_req->Cooler.RunTime = 0;
}

void onvif_init_ThermalConfigurationOptions(onvif_ThermalConfigurationOptions * p_req)
{
    ONVIF_ColorPalette * p_ColorPalette1;
    ONVIF_ColorPalette * p_ColorPalette2;
    ONVIF_NUCTable * p_NUCTable1;
    ONVIF_NUCTable * p_NUCTable2;
    
    p_ColorPalette1 = onvif_add_ColorPalette(&p_req->ColorPalette);
    memset(p_ColorPalette1, 0, sizeof(ONVIF_ColorPalette));
    strcpy(p_ColorPalette1->ColorPalette.token, "CP_TOKEN");
    strcpy(p_ColorPalette1->ColorPalette.Name, "CP_NAME");
    strcpy(p_ColorPalette1->ColorPalette.Type, "WhiteHot");

    p_ColorPalette2 = onvif_add_ColorPalette(&p_req->ColorPalette);
    memset(p_ColorPalette2, 0, sizeof(ONVIF_ColorPalette));
    strcpy(p_ColorPalette2->ColorPalette.token, "CP_TOKEN2");
    strcpy(p_ColorPalette2->ColorPalette.Name, "CP_NAME2");
    strcpy(p_ColorPalette2->ColorPalette.Type, "BlackHot");
	    
    p_NUCTable1 = onvif_add_NUCTable(&p_req->NUCTable);
    memset(p_NUCTable1, 0, sizeof(ONVIF_NUCTable));
    strcpy(p_NUCTable1->NUCTable.token, "NUC_TOKEN");
    strcpy(p_NUCTable1->NUCTable.Name, "NUC_NAME");
    p_NUCTable1->NUCTable.LowTemperatureFlag = 1;
    p_NUCTable1->NUCTable.LowTemperature = 0;
    p_NUCTable1->NUCTable.HighTemperatureFlag = 1;
    p_NUCTable1->NUCTable.HighTemperature = 100;

    p_NUCTable2 = onvif_add_NUCTable(&p_req->NUCTable);
    memset(p_NUCTable2, 0, sizeof(ONVIF_NUCTable));
    strcpy(p_NUCTable2->NUCTable.token, "NUC_TOKEN2");
    strcpy(p_NUCTable2->NUCTable.Name, "NUC_NAME2");
    p_NUCTable2->NUCTable.LowTemperatureFlag = 1;
    p_NUCTable2->NUCTable.LowTemperature = 0;
    p_NUCTable2->NUCTable.HighTemperatureFlag = 1;
    p_NUCTable2->NUCTable.HighTemperature = 100;

    p_req->CoolerOptionsFlag = 1;
    p_req->CoolerOptions.Enabled = TRUE;
}

void onvif_init_RadiometryConfiguration(onvif_RadiometryConfiguration * p_req)
{
    p_req->RadiometryGlobalParametersFlag = 1;
    p_req->RadiometryGlobalParameters.ReflectedAmbientTemperature = 10;
    p_req->RadiometryGlobalParameters.Emissivity = 10;
    p_req->RadiometryGlobalParameters.DistanceToObject = 10;
    p_req->RadiometryGlobalParameters.RelativeHumidityFlag = 1;
    p_req->RadiometryGlobalParameters.RelativeHumidity = 10;
    p_req->RadiometryGlobalParameters.AtmosphericTemperatureFlag = 1;
    p_req->RadiometryGlobalParameters.AtmosphericTemperature = 10;
    p_req->RadiometryGlobalParameters.AtmosphericTransmittanceFlag = 1;
    p_req->RadiometryGlobalParameters.AtmosphericTransmittance = 10;
    p_req->RadiometryGlobalParameters.ExtOpticsTemperatureFlag = 1;
    p_req->RadiometryGlobalParameters.ExtOpticsTemperature = 10;
    p_req->RadiometryGlobalParameters.ExtOpticsTransmittanceFlag = 1;
    p_req->RadiometryGlobalParameters.ExtOpticsTransmittance = 10;
}

void onvif_init_RadiometryConfigurationOptions(onvif_RadiometryConfigurationOptions * p_req)
{
    p_req->RadiometryGlobalParameterOptionsFlag = 1;
    p_req->RadiometryGlobalParameterOptions.ReflectedAmbientTemperature.Min = 0;
    p_req->RadiometryGlobalParameterOptions.ReflectedAmbientTemperature.Max = 100;
    p_req->RadiometryGlobalParameterOptions.Emissivity.Min = 0;
    p_req->RadiometryGlobalParameterOptions.Emissivity.Max = 100;
    p_req->RadiometryGlobalParameterOptions.DistanceToObject.Min = 0;
    p_req->RadiometryGlobalParameterOptions.DistanceToObject.Max = 100;
    p_req->RadiometryGlobalParameterOptions.RelativeHumidityFlag = 1;
    p_req->RadiometryGlobalParameterOptions.RelativeHumidity.Min = 0;
    p_req->RadiometryGlobalParameterOptions.RelativeHumidity.Max = 100;
    p_req->RadiometryGlobalParameterOptions.AtmosphericTemperatureFlag = 1;
    p_req->RadiometryGlobalParameterOptions.AtmosphericTemperature.Min = 0;
    p_req->RadiometryGlobalParameterOptions.AtmosphericTemperature.Max = 100;
    p_req->RadiometryGlobalParameterOptions.AtmosphericTransmittanceFlag = 1;
    p_req->RadiometryGlobalParameterOptions.AtmosphericTransmittance.Min = 0;
    p_req->RadiometryGlobalParameterOptions.AtmosphericTransmittance.Max = 100;
    p_req->RadiometryGlobalParameterOptions.ExtOpticsTemperatureFlag = 1;
    p_req->RadiometryGlobalParameterOptions.ExtOpticsTemperature.Min = 0;
    p_req->RadiometryGlobalParameterOptions.ExtOpticsTemperature.Max = 100;
    p_req->RadiometryGlobalParameterOptions.ExtOpticsTransmittanceFlag = 1;
    p_req->RadiometryGlobalParameterOptions.ExtOpticsTransmittance.Min = 0;
    p_req->RadiometryGlobalParameterOptions.ExtOpticsTransmittance.Max = 100;
}

BOOL onvif_init_Thermal(ONVIF_VideoSource * p_req)
{
    onvif_init_ThermalConfiguration(&p_req->ThermalConfiguration);
    onvif_init_ThermalConfigurationOptions(&p_req->ThermalConfigurationOptions);
    onvif_init_RadiometryConfiguration(&p_req->RadiometryConfiguration);
    onvif_init_RadiometryConfigurationOptions(&p_req->RadiometryConfigurationOptions);

    return TRUE;
}
#endif

#ifdef CREDENTIAL_SUPPORT
ONVIF_Credential * onvif_add_Credential()
{
    ONVIF_Credential * p_tmp;
	ONVIF_Credential * p_new = (ONVIF_Credential *) malloc(sizeof(ONVIF_Credential));
	if (NULL == p_new)
	{
		return NULL;
	}

	memset(p_new, 0, sizeof(ONVIF_Credential));

	p_tmp = g_onvif_cfg.credential;
	if (NULL == p_tmp)
	{
		g_onvif_cfg.credential = p_new;
	}
	else
	{
		while (p_tmp && p_tmp->next) p_tmp = p_tmp->next;

		p_tmp->next = p_new;
	}	

	return p_new;
}

ONVIF_Credential * onvif_find_Credential(const char * token)
{
    ONVIF_Credential * p_tmp = g_onvif_cfg.credential;  
    
    if (NULL == token)
    {
        return NULL;
    }

    while (p_tmp)
    {
        if (strcasecmp(token, p_tmp->Credential.token) == 0)
        {
            return p_tmp;
        }
        
        p_tmp = p_tmp->next;
    }

    return NULL;
}

void onvif_free_Credential(ONVIF_Credential * p_node)
{
    BOOL found = FALSE;
	ONVIF_Credential * p_prev = NULL;
	ONVIF_Credential * p_tmp = g_onvif_cfg.credential;	
	
	while (p_tmp)
	{
		if (p_tmp == p_node)
		{
			found = TRUE;
			break;
		}

		p_prev = p_tmp;
		p_tmp = p_tmp->next;
	}

	if (found)
	{
		if (NULL == p_prev)
		{
			g_onvif_cfg.credential = p_tmp->next;
		}
		else
		{
			p_prev->next = p_tmp->next;
		}

		free(p_tmp);
	}
}

void onvif_free_Credentials()
{
    ONVIF_Credential * p_next;
	ONVIF_Credential * p_tmp = g_onvif_cfg.credential;

	while (p_tmp)
	{
		p_next = p_tmp->next;
        
		free(p_tmp);
		p_tmp = p_next;
	}

	g_onvif_cfg.credential = NULL;
}

BOOL onvif_init_Credential()
{
    ONVIF_Credential * p_tmp = onvif_add_Credential();

    sprintf(p_tmp->Credential.token, "CredentialToken%d", g_onvif_cls.credential_idx++);
    strcpy(p_tmp->Credential.CredentialHolderReference, "testuser");

    p_tmp->Credential.sizeCredentialIdentifier = 1;
    p_tmp->Credential.CredentialIdentifier[0].Used = TRUE;
    p_tmp->Credential.CredentialIdentifier[0].ExemptedFromAuthentication = FALSE;
    strcpy(p_tmp->Credential.CredentialIdentifier[0].Type.Name, "pt:Card");
    strcpy(p_tmp->Credential.CredentialIdentifier[0].Type.FormatType, "GUID");
    strcpy(p_tmp->Credential.CredentialIdentifier[0].Value, "31343031303834323633000000000000");

    p_tmp->State.AntipassbackStateFlag = 1;
    
    return TRUE;
}
#endif // end of CREDENTIAL_SUPPORT

#ifdef ACCESS_RULES
ONVIF_AccessProfile * onvif_add_AccessProfile()
{
    ONVIF_AccessProfile * p_tmp;
	ONVIF_AccessProfile * p_new = (ONVIF_AccessProfile *) malloc(sizeof(ONVIF_AccessProfile));
	if (NULL == p_new)
	{
		return NULL;
	}

	memset(p_new, 0, sizeof(ONVIF_AccessProfile));

	p_tmp = g_onvif_cfg.access_rules;
	if (NULL == p_tmp)
	{
		g_onvif_cfg.access_rules = p_new;
	}
	else
	{
		while (p_tmp && p_tmp->next) p_tmp = p_tmp->next;

		p_tmp->next = p_new;
	}	

	return p_new;
}

ONVIF_AccessProfile * onvif_find_AccessProfile(const char * token)
{
    ONVIF_AccessProfile * p_tmp = g_onvif_cfg.access_rules;  
    
    if (NULL == token)
    {
        return NULL;
    }

    while (p_tmp)
    {
        if (strcasecmp(token, p_tmp->AccessProfile.token) == 0)
        {
            return p_tmp;
        }
        
        p_tmp = p_tmp->next;
    }

    return NULL;
}

void onvif_free_AccessProfile(ONVIF_AccessProfile * p_node)
{
    BOOL found = FALSE;
	ONVIF_AccessProfile * p_prev = NULL;
	ONVIF_AccessProfile * p_tmp = g_onvif_cfg.access_rules;	
	
	while (p_tmp)
	{
		if (p_tmp == p_node)
		{
			found = TRUE;
			break;
		}

		p_prev = p_tmp;
		p_tmp = p_tmp->next;
	}

	if (found)
	{
		if (NULL == p_prev)
		{
			g_onvif_cfg.access_rules = p_tmp->next;
		}
		else
		{
			p_prev->next = p_tmp->next;
		}

		free(p_tmp);
	}
}

void onvif_free_AccessProfiles()
{
    ONVIF_AccessProfile * p_next;
	ONVIF_AccessProfile * p_tmp = g_onvif_cfg.access_rules;

	while (p_tmp)
	{
		p_next = p_tmp->next;
        
		free(p_tmp);
		p_tmp = p_next;
	}

	g_onvif_cfg.access_rules = NULL;
}

BOOL onvif_init_AccessProfile()
{
    ONVIF_AccessProfile * p_tmp = onvif_add_AccessProfile();

    sprintf(p_tmp->AccessProfile.token, "AccessProfileToken%d", g_onvif_cls.credential_idx);
    sprintf(p_tmp->AccessProfile.Name, "AccessProfileName%d", g_onvif_cls.credential_idx++);
    p_tmp->AccessProfile.DescriptionFlag = 1;
    sprintf(p_tmp->AccessProfile.Description, "test");

    p_tmp->AccessProfile.sizeAccessPolicy = 1;
    strcpy(p_tmp->AccessProfile.AccessPolicy[0].ScheduleToken, "test");

#ifdef PROFILE_C_SUPPORT
    if (g_onvif_cfg.access_points)
    {
        strcpy(p_tmp->AccessProfile.AccessPolicy[0].Entity, g_onvif_cfg.access_points->AccessPointInfo.token);
    }
    else
    {
        strcpy(p_tmp->AccessProfile.AccessPolicy[0].Entity, "test");
    }
#else
    strcpy(p_tmp->AccessProfile.AccessPolicy[0].Entity, "test");
#endif

    return TRUE;
}
#endif // end of ACCESS_RULES

#ifdef SCHEDULE_SUPPORT
ONVIF_Schedule * onvif_add_Schedule()
{
    ONVIF_Schedule * p_tmp;
	ONVIF_Schedule * p_new = (ONVIF_Schedule *) malloc(sizeof(ONVIF_Schedule));
	if (NULL == p_new)
	{
		return NULL;
	}

	memset(p_new, 0, sizeof(ONVIF_Schedule));

	p_tmp = g_onvif_cfg.schedule;
	if (NULL == p_tmp)
	{
		g_onvif_cfg.schedule= p_new;
	}
	else
	{
		while (p_tmp && p_tmp->next) p_tmp = p_tmp->next;

		p_tmp->next = p_new;
	}	

	return p_new;
}

ONVIF_Schedule * onvif_find_Schedule(const char * token)
{
    ONVIF_Schedule * p_tmp = g_onvif_cfg.schedule;  
    
    if (NULL == token)
    {
        return NULL;
    }

    while (p_tmp)
    {
        if (strcasecmp(token, p_tmp->Schedule.token) == 0)
        {
            return p_tmp;
        }
        
        p_tmp = p_tmp->next;
    }

    return NULL;
}

void onvif_free_Schedule(ONVIF_Schedule * p_node)
{
    BOOL found = FALSE;
	ONVIF_Schedule * p_prev = NULL;
	ONVIF_Schedule * p_tmp = g_onvif_cfg.schedule;	
	
	while (p_tmp)
	{
		if (p_tmp == p_node)
		{
			found = TRUE;
			break;
		}

		p_prev = p_tmp;
		p_tmp = p_tmp->next;
	}

	if (found)
	{
		if (NULL == p_prev)
		{
			g_onvif_cfg.schedule = p_tmp->next;
		}
		else
		{
			p_prev->next = p_tmp->next;
		}

#ifdef LIBICAL
        if (p_tmp->comp)
        {
            icalcomponent_free(p_tmp->comp);
        }
#endif
		free(p_tmp);
	}
}

void onvif_free_Schedules()
{
    ONVIF_Schedule * p_next;
	ONVIF_Schedule * p_tmp = g_onvif_cfg.schedule;

	while (p_tmp)
	{
		p_next = p_tmp->next;

#ifdef LIBICAL
        if (p_tmp->comp)
        {
            icalcomponent_free(p_tmp->comp);
        }
#endif

		free(p_tmp);
		p_tmp = p_next;
	}

	g_onvif_cfg.schedule = NULL;
}

BOOL onvif_init_Schedule()
{
    ONVIF_Schedule * p_tmp = onvif_add_Schedule();

    sprintf(p_tmp->Schedule.token, "ScheduleToken%d", g_onvif_cls.schedule_idx);
    sprintf(p_tmp->Schedule.Name, "ScheduleName%d", g_onvif_cls.schedule_idx++);
    p_tmp->Schedule.DescriptionFlag = 1;
    sprintf(p_tmp->Schedule.Description, "test");
    sprintf(p_tmp->Schedule.Standard, 
        "BEGIN:VCALENDAR\r\nBEGIN:VEVENT\r\nDTSTART:20171125T200000\r\n"
        "DTEND:20171126T020000\r\nEND:VEVENT\r\nEND:VCALENDAR");

    p_tmp->Schedule.sizeSpecialDays = 1;
    strcpy(p_tmp->Schedule.SpecialDays[0].GroupToken, "test");
    
    return TRUE;
}

ONVIF_SpecialDayGroup * onvif_add_SpecialDayGroup()
{
    ONVIF_SpecialDayGroup * p_tmp;
	ONVIF_SpecialDayGroup * p_new = (ONVIF_SpecialDayGroup *) malloc(sizeof(ONVIF_SpecialDayGroup));
	if (NULL == p_new)
	{
		return NULL;
	}

	memset(p_new, 0, sizeof(ONVIF_SpecialDayGroup));

	p_tmp = g_onvif_cfg.specialdaygroup;
	if (NULL == p_tmp)
	{
		g_onvif_cfg.specialdaygroup= p_new;
	}
	else
	{
		while (p_tmp && p_tmp->next) p_tmp = p_tmp->next;

		p_tmp->next = p_new;
	}	

	return p_new;
}

ONVIF_SpecialDayGroup * onvif_find_SpecialDayGroup(const char * token)
{
    ONVIF_SpecialDayGroup * p_tmp = g_onvif_cfg.specialdaygroup;  
    
    if (NULL == token)
    {
        return NULL;
    }

    while (p_tmp)
    {
        if (strcasecmp(token, p_tmp->SpecialDayGroup.token) == 0)
        {
            return p_tmp;
        }
        
        p_tmp = p_tmp->next;
    }

    return NULL;
}

void onvif_free_SpecialDayGroup(ONVIF_SpecialDayGroup * p_node)
{
    BOOL found = FALSE;
	ONVIF_SpecialDayGroup * p_prev = NULL;
	ONVIF_SpecialDayGroup * p_tmp = g_onvif_cfg.specialdaygroup;	
	
	while (p_tmp)
	{
		if (p_tmp == p_node)
		{
			found = TRUE;
			break;
		}

		p_prev = p_tmp;
		p_tmp = p_tmp->next;
	}

	if (found)
	{
		if (NULL == p_prev)
		{
			g_onvif_cfg.specialdaygroup= p_tmp->next;
		}
		else
		{
			p_prev->next = p_tmp->next;
		}

#ifdef LIBICAL
        if (p_tmp->comp)
        {
            icalcomponent_free(p_tmp->comp);
        }
#endif

		free(p_tmp);
	}
}

void onvif_free_SpecialDayGroups()
{
    ONVIF_SpecialDayGroup * p_next;
	ONVIF_SpecialDayGroup * p_tmp = g_onvif_cfg.specialdaygroup;

	while (p_tmp)
	{
		p_next = p_tmp->next;

#ifdef LIBICAL
        if (p_tmp->comp)
        {
            icalcomponent_free(p_tmp->comp);
        }
#endif

		free(p_tmp);
		p_tmp = p_next;
	}

	g_onvif_cfg.specialdaygroup = NULL;
}

BOOL onvif_init_SpecialDayGroup()
{    
    return TRUE;
}
#endif // end of SCHEDULE_SUPPORT

#ifdef RECEIVER_SUPPORT
ONVIF_Receiver * onvif_add_Receiver()
{
    ONVIF_Receiver * p_tmp;
	ONVIF_Receiver * p_new = (ONVIF_Receiver *) malloc(sizeof(ONVIF_Receiver));
	if (NULL == p_new)
	{
		return NULL;
	}

	memset(p_new, 0, sizeof(ONVIF_Receiver));

	p_tmp = g_onvif_cfg.receiver;
	if (NULL == p_tmp)
	{
		g_onvif_cfg.receiver= p_new;
	}
	else
	{
		while (p_tmp && p_tmp->next) p_tmp = p_tmp->next;

		p_tmp->next = p_new;
	}	

	return p_new;
}

ONVIF_Receiver * onvif_find_Receiver(const char * token)
{
    ONVIF_Receiver * p_tmp = g_onvif_cfg.receiver;  
    
    if (NULL == token)
    {
        return NULL;
    }

    while (p_tmp)
    {
        if (strcasecmp(token, p_tmp->Receiver.Token) == 0)
        {
            return p_tmp;
        }
        
        p_tmp = p_tmp->next;
    }

    return NULL;
}

void onvif_free_Receiver(ONVIF_Receiver * p_node)
{
    BOOL found = FALSE;
	ONVIF_Receiver * p_prev = NULL;
	ONVIF_Receiver * p_tmp = g_onvif_cfg.receiver;	
	
	while (p_tmp)
	{
		if (p_tmp == p_node)
		{
			found = TRUE;
			break;
		}

		p_prev = p_tmp;
		p_tmp = p_tmp->next;
	}

	if (found)
	{
		if (NULL == p_prev)
		{
			g_onvif_cfg.receiver = p_tmp->next;
		}
		else
		{
			p_prev->next = p_tmp->next;
		}
		
		free(p_tmp);
	}
}

void onvif_free_Receivers()
{
    ONVIF_Receiver * p_next;
	ONVIF_Receiver * p_tmp = g_onvif_cfg.receiver;

	while (p_tmp)
	{
		p_next = p_tmp->next;

		free(p_tmp);
		p_tmp = p_next;
	}

	g_onvif_cfg.receiver = NULL;
}

int onvif_get_Receiver_nums()
{
    int nums = 0;
    ONVIF_Receiver * p_tmp = g_onvif_cfg.receiver;

	while (p_tmp)
	{
	    nums++;
		p_tmp = p_tmp->next;
	}

	return nums;
}
#endif // end of RECEIVER_SUPPORT

#ifdef IPFILTER_SUPPORT

BOOL onvif_is_ipaddr_filter_exist(onvif_PrefixedIPAddress * p_head, int size, onvif_PrefixedIPAddress * p_item)
{
    int i;
	for (i = 0; i < size; i++)
	{
		if (strcmp(p_item->Address, p_head[i].Address) == 0 && p_item->PrefixLength == p_head[i].PrefixLength)
		{
			return TRUE;
		}
	}

	return FALSE;	
}

onvif_PrefixedIPAddress * onvif_find_ipaddr_filter(onvif_PrefixedIPAddress * p_head, int size, onvif_PrefixedIPAddress * p_item)
{
    int i;
	for (i = 0; i < size; i++)
	{
		if (strcmp(p_item->Address, p_head[i].Address) == 0 && p_item->PrefixLength == p_head[i].PrefixLength)
		{
			return &p_head[i];
		}
	}

	return NULL;
}

onvif_PrefixedIPAddress * onvif_get_idle_ipaddr_filter(onvif_PrefixedIPAddress * p_head, int size)
{
    int i;
    
	for (i = 0; i < size; i++)
	{
		if (p_head[i].Address[0] == '\0')
		{
			return &p_head[i];
		}
	}

	return NULL;
}

ONVIF_RET onvif_add_ipaddr_filter(onvif_PrefixedIPAddress * p_head, int size, onvif_PrefixedIPAddress * p_item)
{
    onvif_PrefixedIPAddress * p_ipfilter;
    
	if (onvif_is_ipaddr_filter_exist(p_head, size, p_item) == TRUE)
	{
		return ONVIF_OK;
	}

	p_ipfilter = onvif_get_idle_ipaddr_filter(p_head, size);
	if (p_ipfilter)
	{
		p_ipfilter->PrefixLength = p_item->PrefixLength;
		strcpy(p_ipfilter->Address, p_item->Address);
		
		return ONVIF_OK;
	}

	return ONVIF_ERR_IPFilterListIsFull;
}

#endif // IPFILTER_SUPPORT


void onvif_init_cfg()
{
    if (onvif_read_device_uuid(g_onvif_cfg.EndpointReference, sizeof(g_onvif_cfg.EndpointReference)-1) == FALSE)
    {
	    strncpy(g_onvif_cfg.EndpointReference, onvif_uuid_create(), sizeof(g_onvif_cfg.EndpointReference)-1);

	    onvif_save_device_uuid(g_onvif_cfg.EndpointReference);
	}

#ifdef PROFILE_Q_SUPPORT
	g_onvif_cfg.device_state = onvif_read_device_state();
#endif

	g_onvif_cfg.evt_sim_interval = 60;
	g_onvif_cfg.evt_sim_flag = 1;
	g_onvif_cfg.evt_renew_time = 60;
	g_onvif_cfg.http_max_users = 32;

	onvif_init_SystemDateTime();

	// onvif_init_VideoSource();
	onvif_init_VideoSourceConfigurationOptions();
	onvif_init_VideoEncoderConfigurationOptions();	

#ifdef MEDIA2_SUPPORT
    onvif_init_MaskOptions();
#endif

#ifdef AUDIO_SUPPORT
	onvif_init_AudioSource();
	onvif_init_AudioEncoderConfigurationOptions();
	onvif_init_AudioDecoderConfigurations();
	onvif_init_AudioDecoderConfigurationOptions();
#ifdef MEDIA2_SUPPORT
    onvif_init_AudioDecoder2ConfigurationOptions();
#endif
#endif

	onvif_init_ImagingSettings();   ////
	onvif_init_ImagingOptions();

	onvif_init_MetadataConfiguration();
	onvif_init_MetadataConfigurationOptions();

	onvif_init_OSDConfigurationOptions();

#ifdef VIDEO_ANALYTICS
	onvif_init_VideoAnalyticsConfiguration();
#endif

#ifdef PROFILE_C_SUPPORT    
    onvif_init_DoorList();
    onvif_init_AreaInfoList();
    onvif_init_AccessPointList();
#endif

#ifdef DEVICEIO_SUPPORT
#ifdef AUDIO_SUPPORT
    onvif_init_AudioOutput();
#endif    
    onvif_init_RelayOutput();
    onvif_init_DigitInput();
    onvif_init_SerialPort();
#endif

	//读取config.xml文件
	onvif_load_cfg();

#ifdef PROFILE_G_SUPPORT
    onvif_init_Recording();
    onvif_init_RecordingJob();

    g_onvif_cfg.replay_session_timeout = 60;
#endif

#ifdef CREDENTIAL_SUPPORT
    onvif_init_Credential();
#endif

#ifdef ACCESS_RULES
    onvif_init_AccessProfile();
#endif
}

void onvif_chk_server_cfg()
{
	int i;

    for (i = 0; i < g_onvif_cfg.servs_num; i++)
    {
        // check the server ip and port configure
        
        if (g_onvif_cfg.servs[i].serv_port <= 0 || g_onvif_cfg.servs[i].serv_port >= 65535) 
    	{
#ifdef HTTPS
    		if (g_onvif_cfg.https_enable)
    		{
    			g_onvif_cfg.servs[i].serv_port = 443;
    		}
    		else
    		{
    			g_onvif_cfg.servs[i].serv_port = 8000;
    		}
#else
    		g_onvif_cfg.servs[i].serv_port = 8000;
#endif
    	}

    	if (g_onvif_cfg.servs[i].serv_ip[0] == '\0')
    	{
			const char * ip = onvif_get_local_ip();
    		if (ip)
    		{
    			strcpy(g_onvif_cfg.servs[i].serv_ip, ip);
    		}
    	}
    }    
}

void onvif_init()
{
#ifdef PTZ_SUPPORT
    ONVIF_PROFILE * p_profile;
#endif

	memset(&g_onvif_cfg, 0, sizeof(ONVIF_CFG));
	memset(&g_onvif_cls, 0, sizeof(ONVIF_CLS));
	
	onvif_init_cfg();

    onvif_init_net();    

    onvif_chk_server_cfg();
    
    // onvif_init_net();        // 放到 onvif_chk_server_cfg()  前面

    onvif_eua_init();
    
#ifdef PTZ_SUPPORT
	onvif_init_ptz();
	
	// add PTZ node to profile
	p_profile = g_onvif_cfg.profiles;
	while (p_profile)
	{
		p_profile->ptz_cfg = g_onvif_cfg.ptz_cfg;
		p_profile->ptz_cfg->Configuration.UseCount++;
		
		p_profile = p_profile->next;
	}
#endif

#ifdef MEDIA2_SUPPORT
    onvif_add_scope("onvif://www.onvif.org/Profile/T", TRUE);
#endif

#ifdef PROFILE_G_SUPPORT
    onvif_add_scope("onvif://www.onvif.org/Profile/G", TRUE);
#endif

#ifdef PROFILE_C_SUPPORT
    onvif_add_scope("onvif://www.onvif.org/Profile/C", TRUE);
#endif

#ifdef ACCESS_RULES
    onvif_add_scope("onvif://www.onvif.org/Profile/A", TRUE);
#endif

	onvif_init_capabilities();
}




