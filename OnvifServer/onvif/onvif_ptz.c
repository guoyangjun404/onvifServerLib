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
#include "onvif_ptz.h"
#include "onvif_utils.h"

#ifdef MEDIA2_SUPPORT
#include "onvif_media2.h"
#endif

// #include "ptz.h"   ///
#include "set_config.h"

#ifdef PTZ_SUPPORT

/***************************************************************************************/
extern ONVIF_CLS g_onvif_cls;
extern ONVIF_CFG g_onvif_cfg;

/***************************************************************************************/
ONVIF_RET onvif_ptz_GetStatus(ONVIF_PROFILE * p_profile, onvif_PTZStatus * p_ptz_status)
{
	// todo : add get ptz status code ...
	
	p_ptz_status->PositionFlag = 1;
	p_ptz_status->Position.PanTiltFlag = 1;
	p_ptz_status->Position.PanTilt.x = 0;
	p_ptz_status->Position.PanTilt.y = 0;
	p_ptz_status->Position.ZoomFlag = 1;
	p_ptz_status->Position.Zoom.x = 0;
	
	p_ptz_status->MoveStatusFlag = 1;
	p_ptz_status->MoveStatus.PanTiltFlag = 1;
	p_ptz_status->MoveStatus.PanTilt = MoveStatus_IDLE;
	p_ptz_status->MoveStatus.ZoomFlag = 1;
	p_ptz_status->MoveStatus.Zoom = MoveStatus_IDLE;

	p_ptz_status->ErrorFlag = 0;
    p_ptz_status->UtcTime = time(NULL);	
	
	return ONVIF_OK;
}

ONVIF_RET onvif_ContinuousMove(ContinuousMove_REQ * p_req)
{
	ONVIF_PROFILE * p_profile = onvif_find_profile(p_req->ProfileToken);
	if (NULL == p_profile)
	{
		return ONVIF_ERR_NoProfile;
	}
    else if (NULL == g_onvif_cfg.ptz_node)
	{
		return ONVIF_ERR_NoPTZProfile;
	}
	
	if (p_req->Velocity.PanTiltFlag)
	{
	
		if (p_req->Velocity.PanTilt.x - g_onvif_cfg.ptz_node->PTZNode.SupportedPTZSpaces.ContinuousPanTiltVelocitySpace.XRange.Min < -FPP || 
		 	p_req->Velocity.PanTilt.x - g_onvif_cfg.ptz_node->PTZNode.SupportedPTZSpaces.ContinuousPanTiltVelocitySpace.XRange.Max > FPP)
		{
			return ONVIF_ERR_InvalidPosition;
		}

		if (p_req->Velocity.PanTilt.y - g_onvif_cfg.ptz_node->PTZNode.SupportedPTZSpaces.ContinuousPanTiltVelocitySpace.YRange.Min < -FPP || 
			p_req->Velocity.PanTilt.y - g_onvif_cfg.ptz_node->PTZNode.SupportedPTZSpaces.ContinuousPanTiltVelocitySpace.YRange.Max > FPP)
		{
			return ONVIF_ERR_InvalidPosition;
		}
	}
	
	if (p_req->Velocity.ZoomFlag && 
		(p_req->Velocity.Zoom.x - g_onvif_cfg.ptz_node->PTZNode.SupportedPTZSpaces.ContinuousZoomVelocitySpace.XRange.Min < -FPP ||     //Min = -1.0
		 p_req->Velocity.Zoom.x - g_onvif_cfg.ptz_node->PTZNode.SupportedPTZSpaces.ContinuousZoomVelocitySpace.XRange.Max > FPP))		//Max = 1.0
	{
		return ONVIF_ERR_InvalidPosition;
	}	

	// todo : add continuous move code ... 
	// printf("\nPTZvelocity :  x=%f , y = %f , z = %f \n",p_req->Velocity.PanTilt.x, p_req->Velocity.PanTilt.y, p_req->Velocity.Zoom.x);
	float x = p_req->Velocity.PanTilt.x;
	float y = p_req->Velocity.PanTilt.y;
	float z = p_req->Velocity.Zoom.x;

	controlPtzPos(x, y, z);

 /* 	if (p_req->Velocity.PanTilt.x  > 0 && p_req->Velocity.PanTilt.y > 0) {   //右上
		printf("####### right_up ########\n");
		
		pelco_right_up(speed);
	}
	else if (p_req->Velocity.PanTilt.x  > 0 && p_req->Velocity.PanTilt.y < 0){	//右下
		printf("####### right_down ########\n");
		pelco_right_down(speed);
	}
	else if (p_req->Velocity.PanTilt.x  < 0 && p_req->Velocity.PanTilt.y > 0){	//左上
		printf("####### left_up ########\n");
		pelco_left_up(speed);
	}
	else if  (p_req->Velocity.PanTilt.x < 0 && p_req->Velocity.PanTilt.y < 0){	    // 左下
		printf("####### left_down ########\n");
		pelco_left_down(speed);
	}
 */

    return ONVIF_OK;
}

ONVIF_RET onvif_ptz_Stop(PTZ_Stop_REQ * p_req)
{
	ONVIF_PROFILE * p_profile = onvif_find_profile(p_req->ProfileToken);
	if (NULL == p_profile)
	{
		return ONVIF_ERR_NoProfile;
	}
	else if (NULL == g_onvif_cfg.ptz_node)
	{
		return ONVIF_ERR_NoPTZProfile;
	}

	// todo : add stop PTZ moving code ... 
	if(ptzStop() != 0){
		return -1;
	}

	
    return ONVIF_OK;
}

ONVIF_RET onvif_AbsoluteMove(AbsoluteMove_REQ * p_req)
{	
	ONVIF_PROFILE * p_profile = onvif_find_profile(p_req->ProfileToken);
	if (NULL == p_profile)
	{
		return ONVIF_ERR_NoProfile;
	}
	else if (NULL == g_onvif_cfg.ptz_node)
	{
		return ONVIF_ERR_NoPTZProfile;
	}

	if (p_req->Position.PanTiltFlag)
	{
		if (p_req->Position.PanTilt.x - g_onvif_cfg.ptz_node->PTZNode.SupportedPTZSpaces.AbsolutePanTiltPositionSpace.XRange.Min < -FPP || 
		 	p_req->Position.PanTilt.x - g_onvif_cfg.ptz_node->PTZNode.SupportedPTZSpaces.AbsolutePanTiltPositionSpace.XRange.Max > FPP)
		{
			return ONVIF_ERR_InvalidPosition;
		}

		if (p_req->Position.PanTilt.y - g_onvif_cfg.ptz_node->PTZNode.SupportedPTZSpaces.AbsolutePanTiltPositionSpace.YRange.Min < -FPP || 
			p_req->Position.PanTilt.y - g_onvif_cfg.ptz_node->PTZNode.SupportedPTZSpaces.AbsolutePanTiltPositionSpace.YRange.Max > FPP)
		{
			return ONVIF_ERR_InvalidPosition;
		}
	}

	if (p_req->Position.ZoomFlag && 
		(p_req->Position.Zoom.x - g_onvif_cfg.ptz_node->PTZNode.SupportedPTZSpaces.AbsoluteZoomPositionSpace.XRange.Min < -FPP || 
		 p_req->Position.Zoom.x - g_onvif_cfg.ptz_node->PTZNode.SupportedPTZSpaces.AbsoluteZoomPositionSpace.XRange.Max > FPP))
	{
		return ONVIF_ERR_InvalidPosition;
	}	
	
	
	// todo : add absolute move code ...
	
    return ONVIF_OK;
}

ONVIF_RET onvif_RelativeMove(RelativeMove_REQ * p_req)
{
	ONVIF_PROFILE * p_profile = onvif_find_profile(p_req->ProfileToken);
	if (NULL == p_profile)
	{
		return ONVIF_ERR_NoProfile;
	}
	else if (NULL == g_onvif_cfg.ptz_node)
	{
		return ONVIF_ERR_NoPTZProfile;
	}

	if (p_req->Translation.PanTiltFlag)
	{
		if (p_req->Translation.PanTilt.x - g_onvif_cfg.ptz_node->PTZNode.SupportedPTZSpaces.RelativePanTiltTranslationSpace.XRange.Min < -FPP || 
			p_req->Translation.PanTilt.x - g_onvif_cfg.ptz_node->PTZNode.SupportedPTZSpaces.RelativePanTiltTranslationSpace.XRange.Max > FPP)
		{
			return ONVIF_ERR_InvalidPosition;
		}

		if (p_req->Translation.PanTilt.y - g_onvif_cfg.ptz_node->PTZNode.SupportedPTZSpaces.RelativePanTiltTranslationSpace.YRange.Min < -FPP || 
			p_req->Translation.PanTilt.y - g_onvif_cfg.ptz_node->PTZNode.SupportedPTZSpaces.RelativePanTiltTranslationSpace.YRange.Max > FPP)
		{
			return ONVIF_ERR_InvalidPosition;
		}
	}
	
	if (p_req->Translation.ZoomFlag && 
		(p_req->Translation.Zoom.x - g_onvif_cfg.ptz_node->PTZNode.SupportedPTZSpaces.RelativeZoomTranslationSpace.XRange.Min < -FPP || 
		 p_req->Translation.Zoom.x - g_onvif_cfg.ptz_node->PTZNode.SupportedPTZSpaces.RelativeZoomTranslationSpace.XRange.Max > FPP))
	{
		return ONVIF_ERR_InvalidPosition;
	}
	
	// todo : add relative move code ...
	
    return ONVIF_OK;
}

//////
int  onvif_find_PTZPreset_index(const char * profile_token, const char  * preset_token)
{
    int i;
    ONVIF_PROFILE * p_profile = onvif_find_profile(profile_token);
    if (NULL == p_profile)
    {
        return  -1;
    }

    for (i = 0; i < ARRAY_SIZE(p_profile->presets); i++)
    {
        if (strcmp(preset_token, p_profile->presets[i].PTZPreset.token) == 0)
        {
            return i;
        }
    }

	return  -1;
}
//////


ONVIF_RET onvif_SetPreset(SetPreset_REQ * p_req)
{
    ONVIF_PTZPreset * p_preset = NULL;
	ONVIF_PROFILE * p_profile;

	p_profile = onvif_find_profile(p_req->ProfileToken);
	if (NULL == p_profile)
	{
		return ONVIF_ERR_NoProfile;
	}
	else if (NULL == g_onvif_cfg.ptz_node)
	{
		return ONVIF_ERR_NoPTZProfile;
	}
	
    if (p_req->PresetTokenFlag && p_req->PresetToken[0] != '\0')
    {
        p_preset = onvif_find_PTZPreset(p_req->ProfileToken, p_req->PresetToken);
        if (NULL == p_preset)
        {
        	return ONVIF_ERR_NoToken;
        }
    }
    else
    {
        p_preset = onvif_get_idle_PTZPreset(p_req->ProfileToken);
        if (NULL == p_preset)
        {
        	return ONVIF_ERR_TooManyPresets;
        }
    }

    if (p_req->PresetNameFlag && p_req->PresetName[0] != '\0')
    {
    	strcpy(p_preset->PTZPreset.Name, p_req->PresetName);
    }
    else
    {
    	sprintf(p_preset->PTZPreset.Name, "PRESET_%d", g_onvif_cls.preset_idx);
    	strcpy(p_req->PresetName, p_preset->PTZPreset.Name);
    	g_onvif_cls.preset_idx++;
    }
    
    if (p_req->PresetTokenFlag && p_req->PresetToken[0] != '\0')
    {
        strcpy(p_preset->PTZPreset.token, p_req->PresetToken);
    }
    else
    {
        sprintf(p_preset->PTZPreset.token, "PRESET_%d", g_onvif_cls.preset_idx);
        strcpy(p_req->PresetToken, p_preset->PTZPreset.token);
        g_onvif_cls.preset_idx++;
    }

    // todo : get PTZ current position ...
 ////
 	int index = onvif_find_PTZPreset_index(p_req->ProfileToken, p_req->PresetToken);
	// printf(" \ng_onvif_cls.preset_idx = %d\n", g_onvif_cls.preset_idx);
	// printf(" ####   idx= %d\n", index);

	short location=  index<0?0:index;
	// pelco_set_point(location);
	setPtzPreset(location);
 ////

    p_preset->PTZPreset.PTZPositionFlag = 1;
    p_preset->PTZPreset.PTZPosition.PanTiltFlag = 1;
    p_preset->PTZPreset.PTZPosition.PanTilt.x = 0;
    p_preset->PTZPreset.PTZPosition.PanTilt.y = 0;
    p_preset->PTZPreset.PTZPosition.ZoomFlag = 1;
    p_preset->PTZPreset.PTZPosition.Zoom.x = 0;

    p_preset->UsedFlag = 1;
    
    return ONVIF_OK;
}

ONVIF_RET onvif_RemovePreset(RemovePreset_REQ * p_req)
{
    ONVIF_PROFILE * p_profile;
    ONVIF_PTZPreset * p_preset;

    p_profile = onvif_find_profile(p_req->ProfileToken);
	if (NULL == p_profile)
	{
		return ONVIF_ERR_NoProfile;
	}

	if (NULL == g_onvif_cfg.ptz_node)
	{
		return ONVIF_ERR_NoPTZProfile;
	}

    p_preset = onvif_find_PTZPreset(p_req->ProfileToken, p_req->PresetToken);
    if (NULL == p_preset)
    {
		onvif_find_PTZPreset(p_req->ProfileToken, p_req->PresetToken);
    }

    memset(p_preset, 0, sizeof(ONVIF_PTZPreset));

    return ONVIF_OK;
}

ONVIF_RET onvif_GotoPreset(GotoPreset_REQ * p_req)
{	
	ONVIF_PROFILE * p_profile;
    ONVIF_PTZPreset * p_preset;
    
	p_profile = onvif_find_profile(p_req->ProfileToken);
	if (NULL == p_profile)
	{
		return ONVIF_ERR_NoProfile;
	}

	if (NULL == g_onvif_cfg.ptz_node)
	{
		return ONVIF_ERR_NoPTZProfile;
	}

    p_preset = onvif_find_PTZPreset(p_req->ProfileToken, p_req->PresetToken);
    if (NULL == p_preset)
    {
        return ONVIF_ERR_NoToken;
    }

    // todo : add goto preset code ...
 ////
 		int index = onvif_find_PTZPreset_index(p_req->ProfileToken, p_req->PresetToken);

    // todo : get PTZ current position ...
	/* printf(" g_onvif_cls.preset_idx = %d\n", g_onvif_cls.preset_idx);
	printf(" ####   goto  idx= %d\n", index); */

	short location=  index<0?0:index;
	// pelco_get_point(location);
	gotoPtzPreset(location);
 ////


    return ONVIF_OK;
}

ONVIF_RET onvif_GotoHomePosition(GotoHomePosition_REQ * p_req)
{
    ONVIF_PROFILE * p_profile = onvif_find_profile(p_req->ProfileToken);
    if (NULL == p_profile)
    {
        return ONVIF_ERR_NoProfile;
    }

    if (NULL == g_onvif_cfg.ptz_node)
    {
    	return ONVIF_ERR_NoPTZProfile;
    }

    // todo : add goto home position code ...

    return ONVIF_OK;
}

ONVIF_RET onvif_SetHomePosition(const char * token)
{
	ONVIF_PROFILE * p_profile = onvif_find_profile(token);
    if (NULL == p_profile)
    {
        return ONVIF_ERR_NoProfile;
    }

    if (NULL == g_onvif_cfg.ptz_node)
    {
    	return ONVIF_ERR_NoPTZProfile;
    }

	if (g_onvif_cfg.ptz_node->PTZNode.FixedHomePositionFlag && 
	    g_onvif_cfg.ptz_node->PTZNode.FixedHomePosition)
	{
		return ONVIF_ERR_CannotOverwriteHome;
	}
	
    // todo : add set home position code ...

    return ONVIF_OK;
}

ONVIF_RET onvif_SetConfiguration(SetConfiguration_REQ * p_req)
{
	ONVIF_PTZConfiguration * p_ptz_cfg;
	ONVIF_PTZNode * p_ptz_node;

	p_ptz_cfg = onvif_find_PTZConfiguration(p_req->PTZConfiguration.token);
	if (NULL == p_ptz_cfg)
	{
		return ONVIF_ERR_NoConfig;
	}
	
	p_ptz_node = onvif_find_PTZNode(p_req->PTZConfiguration.NodeToken);
	if (NULL == p_ptz_node)
	{
		return ONVIF_ERR_ConfigModify;
	}

	if (p_req->PTZConfiguration.DefaultPTZTimeoutFlag)
	{
		if (p_req->PTZConfiguration.DefaultPTZTimeout < g_onvif_cfg.PTZConfigurationOptions.PTZTimeout.Min ||
			p_req->PTZConfiguration.DefaultPTZTimeout > g_onvif_cfg.PTZConfigurationOptions.PTZTimeout.Max)
		{
			return ONVIF_ERR_ConfigModify;
		}
	}

	// todo : add set ptz configuration code ...


    if (p_req->PTZConfiguration.MoveRampFlag)
    {
        p_ptz_cfg->Configuration.MoveRamp = p_req->PTZConfiguration.MoveRamp;
    }
    
    if (p_req->PTZConfiguration.PresetRampFlag)
    {
        p_ptz_cfg->Configuration.PresetRamp = p_req->PTZConfiguration.PresetRamp;
    }
    
    if (p_req->PTZConfiguration.PresetTourRampFlag)
    {
        p_ptz_cfg->Configuration.PresetTourRamp = p_req->PTZConfiguration.PresetTourRamp;
    }    

	strcpy(p_ptz_cfg->Configuration.Name, p_req->PTZConfiguration.Name);
	if (p_req->PTZConfiguration.DefaultPTZSpeedFlag)
	{
		if (p_req->PTZConfiguration.DefaultPTZSpeed.PanTiltFlag)
		{
			p_ptz_cfg->Configuration.DefaultPTZSpeed.PanTilt.x = p_req->PTZConfiguration.DefaultPTZSpeed.PanTilt.x;
			p_ptz_cfg->Configuration.DefaultPTZSpeed.PanTilt.y = p_req->PTZConfiguration.DefaultPTZSpeed.PanTilt.y;
		}

		if (p_req->PTZConfiguration.DefaultPTZSpeed.ZoomFlag)
		{
			p_ptz_cfg->Configuration.DefaultPTZSpeed.Zoom.x = p_req->PTZConfiguration.DefaultPTZSpeed.Zoom.x;
		}
	}

	if (p_req->PTZConfiguration.DefaultPTZTimeoutFlag)
	{
		p_ptz_cfg->Configuration.DefaultPTZTimeout = p_req->PTZConfiguration.DefaultPTZTimeout;
	}

	if (p_req->PTZConfiguration.PanTiltLimitsFlag)
	{
		memcpy(&p_ptz_cfg->Configuration.PanTiltLimits, &p_req->PTZConfiguration.PanTiltLimits, sizeof(onvif_PanTiltLimits));
	}

	if (p_req->PTZConfiguration.ZoomLimitsFlag)
	{
		memcpy(&p_ptz_cfg->Configuration.ZoomLimits, &p_req->PTZConfiguration.ZoomLimits, sizeof(onvif_ZoomLimits));
	}

	if (p_req->PTZConfiguration.ExtensionFlag)
	{
		if (p_req->PTZConfiguration.Extension.PTControlDirectionFlag)
		{
			if (p_req->PTZConfiguration.Extension.PTControlDirection.EFlipFlag)
			{
				p_ptz_cfg->Configuration.Extension.PTControlDirection.EFlip = p_req->PTZConfiguration.Extension.PTControlDirection.EFlip;
			}

			if (p_req->PTZConfiguration.Extension.PTControlDirection.ReverseFlag)
			{
				p_ptz_cfg->Configuration.Extension.PTControlDirection.Reverse = p_req->PTZConfiguration.Extension.PTControlDirection.Reverse;
			}
		}
	}
    
#ifdef MEDIA2_SUPPORT
    onvif_MediaConfigurationChangedNotify(p_req->PTZConfiguration.token, "PTZ");
#endif

	return ONVIF_OK;
}


#endif // PTZ_SUPPORT


