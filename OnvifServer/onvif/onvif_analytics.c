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
#include "onvif_analytics.h"

#ifdef VIDEO_ANALYTICS

/***************************************************************************************/

/**
 The possible return value
 	ONVIF_ERR_NoProfile
 	ONVIF_ERR_NoConfig
*/
ONVIF_RET onvif_AddVideoAnalyticsConfiguration(AddVideoAnalyticsConfiguration_REQ * p_req)
{
	ONVIF_VideoAnalyticsConfiguration * p_va_cfg;
	ONVIF_PROFILE * p_profile = onvif_find_profile(p_req->ProfileToken);
	if (NULL == p_profile)
	{
		return ONVIF_ERR_NoProfile;
	}

	p_va_cfg = onvif_find_VideoAnalyticsConfiguration(p_req->ConfigurationToken);
	if (NULL == p_va_cfg)
	{
		return ONVIF_ERR_NoConfig;
	}

	if (p_profile->va_cfg != p_va_cfg)
	{
		if (p_profile->va_cfg && p_profile->va_cfg->Configuration.UseCount > 0)
		{
			p_profile->va_cfg->Configuration.UseCount--;
		}
		
		p_va_cfg->Configuration.UseCount++;
		p_profile->va_cfg = p_va_cfg;
	}

	// todo : add video analytics configuration code ...
	
	return ONVIF_OK;
}

/**
 The possible return value
 	ONVIF_ERR_NoProfile
*/
ONVIF_RET onvif_RemoveVideoAnalyticsConfiguration(RemoveVideoAnalyticsConfiguration_REQ * p_req)
{
	ONVIF_PROFILE * p_profile = onvif_find_profile(p_req->ProfileToken);
	if (NULL == p_profile)
	{
		return ONVIF_ERR_NoProfile;
	}

	if (p_profile->va_cfg && p_profile->va_cfg->Configuration.UseCount > 0)
	{
		p_profile->va_cfg->Configuration.UseCount--;
	}
	
	p_profile->va_cfg = NULL;

	// todo : remove video analytics configuration code ...
	
	return ONVIF_OK;
}

/**
 The possible return value
 	ONVIF_ERR_NoConfig
 	ONVIF_ERR_ConfigModify
 	ONVIF_ERR_ConfigurationConflict
*/
ONVIF_RET onvif_SetVideoAnalyticsConfiguration(SetVideoAnalyticsConfiguration_REQ * p_req)
{
	ONVIF_VideoAnalyticsConfiguration * p_va_cfg;

	p_va_cfg = onvif_find_VideoAnalyticsConfiguration(p_req->Configuration.token);
	if (NULL == p_va_cfg)
	{
		return ONVIF_ERR_NoConfig;
	}

	// check the configuration parameters ...

	// save the video analytics configuration 
	strcpy(p_va_cfg->Configuration.Name, p_req->Configuration.Name);
	
	onvif_free_Configs(&p_va_cfg->Configuration.AnalyticsEngineConfiguration.AnalyticsModule);
	onvif_free_Configs(&p_va_cfg->Configuration.RuleEngineConfiguration.Rule);

	p_va_cfg->Configuration.AnalyticsEngineConfiguration.AnalyticsModule = p_req->Configuration.AnalyticsEngineConfiguration.AnalyticsModule;
	p_va_cfg->Configuration.RuleEngineConfiguration.Rule = p_req->Configuration.RuleEngineConfiguration.Rule;

	// todo : set video analytics configuration code ...

	
	return ONVIF_OK;
}

/**
 The possible return value
 	ONVIF_ERR_NoConfig
*/
ONVIF_RET onvif_GetSupportedRules(GetSupportedRules_REQ * p_req, GetSupportedRules_RES * p_res)
{
	ONVIF_VideoAnalyticsConfiguration * p_va_cfg;

	p_va_cfg = onvif_find_VideoAnalyticsConfiguration(p_req->ConfigurationToken);
	if (NULL == p_va_cfg)
	{
		return ONVIF_ERR_NoConfig;
	}

	memcpy(&p_res->SupportedRules, &p_va_cfg->SupportedRules, sizeof(onvif_SupportedRules));
	
	return ONVIF_OK;
}

/**
 The possible return value
 	ONVIF_ERR_NoConfig
 	ONVIF_ERR_InvalidRule,
	ONVIF_ERR_RuleAlreadyExistent
	ONVIF_ERR_TooManyRules
	ONVIF_ERR_ConfigurationConflict
*/
ONVIF_RET onvif_CreateRules(CreateRules_REQ * p_req)
{
	ONVIF_VideoAnalyticsConfiguration * p_va_cfg;
	ONVIF_Config * p_config;

	if (NULL == p_req->Rule)
	{
		return ONVIF_ERR_InvalidRule;
	}
	
	p_va_cfg = onvif_find_VideoAnalyticsConfiguration(p_req->ConfigurationToken);
	if (NULL == p_va_cfg)
	{
		return ONVIF_ERR_NoConfig;
	}

	p_config = onvif_find_Config(&p_va_cfg->Configuration.RuleEngineConfiguration.Rule, p_req->Rule->Config.Name);
	if (NULL != p_config)
	{
		return ONVIF_ERR_RuleAlreadyExistent;
	}
	
	// check rule configuration pararmeters ...

	p_config = p_va_cfg->Configuration.RuleEngineConfiguration.Rule;
	if (NULL == p_config)
	{
		p_va_cfg->Configuration.RuleEngineConfiguration.Rule = p_req->Rule;
	}
	else
	{
		while (p_config && p_config->next) p_config = p_config->next;

		p_config->next = p_va_cfg->Configuration.RuleEngineConfiguration.Rule;
	}
	
	return ONVIF_OK;
}

/**
 The possible return value
 	ONVIF_ERR_NoConfig
	ONVIF_ERR_RuleNotExistent
	ONVIF_ERR_ConfigurationConflict
*/
ONVIF_RET onvif_DeleteRules(DeleteRules_REQ * p_req)
{
	int i;
	ONVIF_VideoAnalyticsConfiguration * p_va_cfg;
	ONVIF_Config * p_config;
	
	p_va_cfg = onvif_find_VideoAnalyticsConfiguration(p_req->ConfigurationToken);
	if (NULL == p_va_cfg)
	{
		return ONVIF_ERR_NoConfig;
	}

	for (i = 0; i < p_req->sizeRuleName; i++)
	{
		p_config = onvif_find_Config(&p_va_cfg->Configuration.RuleEngineConfiguration.Rule, p_req->RuleName[i]);
		if (NULL == p_config)
		{
			return ONVIF_ERR_RuleNotExistent;
		}

		onvif_remove_Config(&p_va_cfg->Configuration.RuleEngineConfiguration.Rule, p_config);
	}
	
	return ONVIF_OK;
}

/**
 The possible return value
 	ONVIF_ERR_NoConfig
*/
ONVIF_RET onvif_GetRules(GetRules_REQ * p_req, GetRules_RES * p_res)
{
	ONVIF_VideoAnalyticsConfiguration * p_va_cfg;
	
	p_va_cfg = onvif_find_VideoAnalyticsConfiguration(p_req->ConfigurationToken);
	if (NULL == p_va_cfg)
	{
		return ONVIF_ERR_NoConfig;
	}

	p_res->Rule = p_va_cfg->Configuration.RuleEngineConfiguration.Rule;
	
	return ONVIF_OK;
}

/**
 The possible return value
 	ONVIF_ERR_NoConfig
 	ONVIF_ERR_InvalidRule,
	ONVIF_ERR_RuleNotExistent
	ONVIF_ERR_TooManyRules
	ONVIF_ERR_ConfigurationConflict
*/
ONVIF_RET onvif_ModifyRules(ModifyRules_REQ * p_req)
{
	ONVIF_VideoAnalyticsConfiguration * p_va_cfg;
	ONVIF_Config * p_config;
	ONVIF_Config * p_tmp;
	ONVIF_Config * p_prev;

	if (NULL == p_req->Rule)
	{
		return ONVIF_ERR_InvalidRule;
	}
	
	p_va_cfg = onvif_find_VideoAnalyticsConfiguration(p_req->ConfigurationToken);
	if (NULL == p_va_cfg)
	{
		return ONVIF_ERR_NoConfig;
	}

	p_tmp = p_req->Rule;
	while (p_tmp)
	{
		// check rule configuration parameters ...

		p_config = onvif_find_Config(&p_va_cfg->Configuration.RuleEngineConfiguration.Rule, p_tmp->Config.Name);
		if (NULL == p_config)
		{
			onvif_free_Configs(&p_tmp);	// free resource
			
			return ONVIF_ERR_RuleNotExistent;
		}

		p_prev = onvif_get_prev_Config(&p_va_cfg->Configuration.RuleEngineConfiguration.Rule, p_config);
		if (NULL == p_prev)
		{
			p_va_cfg->Configuration.RuleEngineConfiguration.Rule = p_tmp;
			p_tmp->next = p_config->next;
		}
		else
		{
			p_prev->next = p_tmp;
			p_tmp->next = p_config->next;
		}

		onvif_free_Config(p_config);
		free(p_config);

		p_tmp = p_tmp->next;
	}
	
	return ONVIF_OK;
}

/**
 The possible return value
 	ONVIF_ERR_NoConfig
 	ONVIF_ERR_NameAlreadyExistent
	ONVIF_ERR_TooManyModules
	ONVIF_ERR_InvalidModule
	ONVIF_ERR_ConfigurationConflict
*/
ONVIF_RET onvif_CreateAnalyticsModules(CreateAnalyticsModules_REQ * p_req)
{
	ONVIF_VideoAnalyticsConfiguration * p_va_cfg;
	ONVIF_Config * p_config;

	if (NULL == p_req->AnalyticsModule)
	{
		return ONVIF_ERR_InvalidModule;
	}
	
	p_va_cfg = onvif_find_VideoAnalyticsConfiguration(p_req->ConfigurationToken);
	if (NULL == p_va_cfg)
	{
		return ONVIF_ERR_NoConfig;
	}

	p_config = onvif_find_Config(&p_va_cfg->Configuration.AnalyticsEngineConfiguration.AnalyticsModule, 
	    p_req->AnalyticsModule->Config.Name);
	if (NULL != p_config)
	{
		return ONVIF_ERR_NameAlreadyExistent;
	}
	
	// check analytics module configuration pararmeters ...

	p_config = p_va_cfg->Configuration.AnalyticsEngineConfiguration.AnalyticsModule;
	if (NULL == p_config)
	{
		p_va_cfg->Configuration.AnalyticsEngineConfiguration.AnalyticsModule = p_req->AnalyticsModule;
	}
	else
	{
		while (p_config && p_config->next) p_config = p_config->next;

		p_config->next = p_req->AnalyticsModule;
	}
	
	return ONVIF_OK;
}

/**
 The possible return value
 	ONVIF_ERR_NoConfig
 	ONVIF_ERR_InvalidModule
	ONVIF_ERR_ConfigurationConflict
*/
ONVIF_RET onvif_DeleteAnalyticsModules(DeleteAnalyticsModules_REQ * p_req)
{
	int i;
	ONVIF_VideoAnalyticsConfiguration * p_va_cfg;
	ONVIF_Config * p_config;
	
	p_va_cfg = onvif_find_VideoAnalyticsConfiguration(p_req->ConfigurationToken);
	if (NULL == p_va_cfg)
	{
		return ONVIF_ERR_NoConfig;
	}

	for (i = 0; i < p_req->sizeAnalyticsModuleName; i++)
	{
		p_config = onvif_find_Config(&p_va_cfg->Configuration.AnalyticsEngineConfiguration.AnalyticsModule, 
		    p_req->AnalyticsModuleName[i]);
		if (NULL == p_config)
		{
			return ONVIF_ERR_InvalidModule;
		}

		onvif_remove_Config(&p_va_cfg->Configuration.AnalyticsEngineConfiguration.AnalyticsModule, p_config);
	}
	
	return ONVIF_OK;
}

/**
 The possible return value
 	ONVIF_ERR_NoConfig
*/
ONVIF_RET onvif_GetAnalyticsModules(GetAnalyticsModules_REQ * p_req, GetAnalyticsModules_RES * p_res)
{
	ONVIF_VideoAnalyticsConfiguration * p_va_cfg;
	
	p_va_cfg = onvif_find_VideoAnalyticsConfiguration(p_req->ConfigurationToken);
	if (NULL == p_va_cfg)
	{
		return ONVIF_ERR_NoConfig;
	}

	p_res->AnalyticsModule = p_va_cfg->Configuration.AnalyticsEngineConfiguration.AnalyticsModule;
	
	return ONVIF_OK;
}

/**
 The possible return value
 	ONVIF_ERR_NoConfig
 	ONVIF_ERR_InvalidModule
	ONVIF_ERR_TooManyModules
	ONVIF_ERR_InvalidModule
	ONVIF_ERR_ConfigurationConflict
*/
ONVIF_RET onvif_ModifyAnalyticsModules(ModifyAnalyticsModules_REQ * p_req)
{
	ONVIF_VideoAnalyticsConfiguration * p_va_cfg;
	ONVIF_Config * p_config;
	ONVIF_Config * p_tmp;
	ONVIF_Config * p_prev;

	if (NULL == p_req->AnalyticsModule)
	{
		return ONVIF_ERR_InvalidRule;
	}
	
	p_va_cfg = onvif_find_VideoAnalyticsConfiguration(p_req->ConfigurationToken);
	if (NULL == p_va_cfg)
	{
		return ONVIF_ERR_NoConfig;
	}

	p_tmp = p_req->AnalyticsModule;
	while (p_tmp)
	{
		// check rule configuration parameters ...

		p_config = onvif_find_Config(&p_va_cfg->Configuration.AnalyticsEngineConfiguration.AnalyticsModule, 
		    p_tmp->Config.Name);
		if (NULL == p_config)
		{
			onvif_free_Configs(&p_tmp);	// free resource
			
			return ONVIF_ERR_InvalidModule;
		}

		p_prev = onvif_get_prev_Config(&p_va_cfg->Configuration.AnalyticsEngineConfiguration.AnalyticsModule, p_config);
		if (NULL == p_prev)
		{
			p_va_cfg->Configuration.AnalyticsEngineConfiguration.AnalyticsModule = p_tmp;
			p_tmp->next = p_config->next;
		}
		else
		{
			p_prev->next = p_tmp;
			p_tmp->next = p_config->next;
		}

		onvif_free_Config(p_config);
		free(p_config);

		p_tmp = p_tmp->next;
	}
	
	return ONVIF_OK;
}


#endif	// end of VIDEO_ANALYTICS


