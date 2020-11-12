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
#include "onvif_image.h"

#include "set_config.h"  // add by xieqingpu


/***************************************************************************************/
extern ONVIF_CFG g_onvif_cfg;

/***************************************************************************************/
ONVIF_RET onvif_SetImagingSettings(SetImagingSettings_REQ * p_req)
{
	ONVIF_VideoSource * p_v_src = onvif_find_VideoSource(p_req->VideoSourceToken);
	if (NULL == p_v_src)
	{
		return ONVIF_ERR_NoSource;
	}

	/* valid param value */
	
    if (p_req->ImagingSettings.BacklightCompensationFlag && p_req->ImagingSettings.BacklightCompensation.LevelFlag && 
    	(p_req->ImagingSettings.BacklightCompensation.Level - g_onvif_cfg.ImagingOptions.BacklightCompensation.Level.Min < -FPP || 
		 p_req->ImagingSettings.BacklightCompensation.Level - g_onvif_cfg.ImagingOptions.BacklightCompensation.Level.Max > FPP))
	{
		return ONVIF_ERR_SettingsInvalid;
	}
    
	if (p_req->ImagingSettings.BrightnessFlag && 
		(p_req->ImagingSettings.Brightness - g_onvif_cfg.ImagingOptions.Brightness.Min < -FPP || 
		 p_req->ImagingSettings.Brightness - g_onvif_cfg.ImagingOptions.Brightness.Max > FPP))
	{
		return ONVIF_ERR_SettingsInvalid;
	}

	if (p_req->ImagingSettings.ColorSaturationFlag && 
		(p_req->ImagingSettings.ColorSaturation - g_onvif_cfg.ImagingOptions.ColorSaturation.Min < -FPP || 
		 p_req->ImagingSettings.ColorSaturation - g_onvif_cfg.ImagingOptions.ColorSaturation.Max > FPP))
	{
		return ONVIF_ERR_SettingsInvalid;
	}

	if (p_req->ImagingSettings.ContrastFlag && 
		(p_req->ImagingSettings.Contrast - g_onvif_cfg.ImagingOptions.Contrast.Min < -FPP || 
		 p_req->ImagingSettings.Contrast - g_onvif_cfg.ImagingOptions.Contrast.Max > FPP))
	{
		return ONVIF_ERR_SettingsInvalid;
	}

	if (p_req->ImagingSettings.ExposureFlag && p_req->ImagingSettings.Exposure.MinExposureTimeFlag && 
		(p_req->ImagingSettings.Exposure.MinExposureTime - g_onvif_cfg.ImagingOptions.Exposure.MinExposureTime.Min < -FPP || 
		 p_req->ImagingSettings.Exposure.MinExposureTime - g_onvif_cfg.ImagingOptions.Exposure.MinExposureTime.Max > FPP))
	{
		return ONVIF_ERR_SettingsInvalid;
	}

	if (p_req->ImagingSettings.ExposureFlag && p_req->ImagingSettings.Exposure.MaxExposureTimeFlag && 
		(p_req->ImagingSettings.Exposure.MaxExposureTime - g_onvif_cfg.ImagingOptions.Exposure.MaxExposureTime.Min < -FPP|| 
		 p_req->ImagingSettings.Exposure.MaxExposureTime - g_onvif_cfg.ImagingOptions.Exposure.MaxExposureTime.Max > FPP))
	{
		return ONVIF_ERR_SettingsInvalid;
	}

	if (p_req->ImagingSettings.ExposureFlag && p_req->ImagingSettings.Exposure.MinGainFlag && 
		(p_req->ImagingSettings.Exposure.MinGain - g_onvif_cfg.ImagingOptions.Exposure.MinGain.Min < -FPP || 
		 p_req->ImagingSettings.Exposure.MinGain - g_onvif_cfg.ImagingOptions.Exposure.MinGain.Max > FPP))
	{
		return ONVIF_ERR_SettingsInvalid;
	}

	if (p_req->ImagingSettings.ExposureFlag && p_req->ImagingSettings.Exposure.MaxGainFlag && 
		(p_req->ImagingSettings.Exposure.MaxGain - g_onvif_cfg.ImagingOptions.Exposure.MaxGain.Min < -FPP || 
		 p_req->ImagingSettings.Exposure.MaxGain - g_onvif_cfg.ImagingOptions.Exposure.MaxGain.Max > FPP))
	{
		return ONVIF_ERR_SettingsInvalid;
	}

	if (p_req->ImagingSettings.ExposureFlag && p_req->ImagingSettings.Exposure.MinIrisFlag && 
		(p_req->ImagingSettings.Exposure.MinIris - g_onvif_cfg.ImagingOptions.Exposure.MinIris.Min < -FPP || 
		 p_req->ImagingSettings.Exposure.MinIris - g_onvif_cfg.ImagingOptions.Exposure.MinIris.Max > FPP))
	{
		return ONVIF_ERR_SettingsInvalid;
	}

	if (p_req->ImagingSettings.ExposureFlag && p_req->ImagingSettings.Exposure.MaxIrisFlag && 
		(p_req->ImagingSettings.Exposure.MaxIris - g_onvif_cfg.ImagingOptions.Exposure.MaxIris.Min < -FPP || 
		 p_req->ImagingSettings.Exposure.MaxIris - g_onvif_cfg.ImagingOptions.Exposure.MaxIris.Max > FPP))
	{
		return ONVIF_ERR_SettingsInvalid;
	}

	if (p_req->ImagingSettings.ExposureFlag && p_req->ImagingSettings.Exposure.ExposureTimeFlag && 
		(p_req->ImagingSettings.Exposure.ExposureTime - g_onvif_cfg.ImagingOptions.Exposure.ExposureTime.Min < -FPP || 
		 p_req->ImagingSettings.Exposure.ExposureTime - g_onvif_cfg.ImagingOptions.Exposure.ExposureTime.Max > FPP))
	{
		return ONVIF_ERR_SettingsInvalid;
	}

	if (p_req->ImagingSettings.ExposureFlag && p_req->ImagingSettings.Exposure.GainFlag && 
		(p_req->ImagingSettings.Exposure.Gain - g_onvif_cfg.ImagingOptions.Exposure.Gain.Min < -FPP || 
		 p_req->ImagingSettings.Exposure.Gain - g_onvif_cfg.ImagingOptions.Exposure.Gain.Max > FPP))
	{
		return ONVIF_ERR_SettingsInvalid;
	}

	if (p_req->ImagingSettings.ExposureFlag && p_req->ImagingSettings.Exposure.IrisFlag && 
		(p_req->ImagingSettings.Exposure.Iris - g_onvif_cfg.ImagingOptions.Exposure.Iris.Min < -FPP || 
		 p_req->ImagingSettings.Exposure.Iris - g_onvif_cfg.ImagingOptions.Exposure.Iris.Max > FPP))
	{
		return ONVIF_ERR_SettingsInvalid;
	}
	
	if (p_req->ImagingSettings.SharpnessFlag && 
		(p_req->ImagingSettings.Sharpness - g_onvif_cfg.ImagingOptions.Sharpness.Min < -FPP || 
		 p_req->ImagingSettings.Sharpness - g_onvif_cfg.ImagingOptions.Sharpness.Max > FPP))
	{
		return ONVIF_ERR_SettingsInvalid;
	}

	if (p_req->ImagingSettings.WideDynamicRangeFlag && p_req->ImagingSettings.WideDynamicRange.LevelFlag && 
		(p_req->ImagingSettings.WideDynamicRange.Level - g_onvif_cfg.ImagingOptions.WideDynamicRange.Level.Min < -FPP || 
		 p_req->ImagingSettings.WideDynamicRange.Level - g_onvif_cfg.ImagingOptions.WideDynamicRange.Level.Max > FPP))
	{
		return ONVIF_ERR_SettingsInvalid;
	}

	if (p_req->ImagingSettings.WhiteBalanceFlag && p_req->ImagingSettings.WhiteBalance.CrGainFlag && 
		(p_req->ImagingSettings.WhiteBalance.CrGain - g_onvif_cfg.ImagingOptions.WhiteBalance.YrGain.Min < -FPP || 
		 p_req->ImagingSettings.WhiteBalance.CrGain - g_onvif_cfg.ImagingOptions.WhiteBalance.YrGain.Max > FPP))
	{
		return ONVIF_ERR_SettingsInvalid;
	}

	if (p_req->ImagingSettings.WhiteBalanceFlag && p_req->ImagingSettings.WhiteBalance.CbGainFlag && 
		(p_req->ImagingSettings.WhiteBalance.CbGain - g_onvif_cfg.ImagingOptions.WhiteBalance.YbGain.Min < -FPP || 
		 p_req->ImagingSettings.WhiteBalance.CbGain - g_onvif_cfg.ImagingOptions.WhiteBalance.YbGain.Max > FPP))
	{
		return ONVIF_ERR_SettingsInvalid;
	}
	
	// todo : add the image setting code ...
	///   add by xieqingpu
	if (p_req->ImagingSettings.ThermalSettings_extFlag != 1 && p_req->ImagingSettings.DulaInformationFlag != 1){
		// printf("Brightness 亮度= %0.2f ,ColorSaturation 饱和度 = %0.2f , Contrast 对比度 = %0.2f , Sharpness 锐度= %0.2f\n",p_req->ImagingSettings.Brightness, p_req->ImagingSettings.ColorSaturation,
		// p_req->ImagingSettings.Contrast, p_req->ImagingSettings.Sharpness); 
		if (p_req->ImagingSettings.BrightnessFlag !=1)
			p_req->ImagingSettings.Brightness = -1;

		 if (p_req->ImagingSettings.ColorSaturationFlag != 1)
			p_req->ImagingSettings.ColorSaturation = -1;

		 if (p_req->ImagingSettings.ContrastFlag != 1)
			p_req->ImagingSettings.Contrast = -1;

		 if (p_req->ImagingSettings.SharpnessFlag != 1)
			p_req->ImagingSettings.Sharpness = -1;
		
		ImgParam_t  imgParams;
		memset(&imgParams, 0, sizeof(ImgParam_t));
		
		imgParams.brightness =  p_req->ImagingSettings.Brightness;
		imgParams.saturation =  p_req->ImagingSettings.ColorSaturation;
		imgParams.contrast =  p_req->ImagingSettings.Contrast;
		imgParams.sharp =  p_req->ImagingSettings.Sharpness;

		setImgParam(&imgParams);
	}

	 if ( p_req->ImagingSettings.ThermalSettings.ThermalSet_ext1Flag == 1 )
	{
		/* 	printf("UserPalette 色板:%d (1-12)\n", p_req->ImagingSettings.ThermalSettings.ThermalSet1.UserPalette);
		printf("WideDynamic 宽动态:%d (0：关 or 1：开)\n", p_req->ImagingSettings.ThermalSettings.ThermalSet1.WideDynamic);
		printf("OrgData 数据源:%d (0：原始数据 or 1：YUV数据)\n", p_req->ImagingSettings.ThermalSettings.ThermalSet1.OrgData);	
		printf("Actime 自动校正间隔:%d \n", p_req->ImagingSettings.ThermalSettings.ThermalSet1.Actime); 
 		*/
	 	ThermalParam1_t thermalParam_1;
	    memset(&thermalParam_1, 0, sizeof(ThermalParam1_t));

		thermalParam_1.userPalette =  p_req->ImagingSettings.ThermalSettings.ThermalSet1.UserPalette;
		thermalParam_1.wideDynamic =  p_req->ImagingSettings.ThermalSettings.ThermalSet1.WideDynamic;
		thermalParam_1.orgData =  p_req->ImagingSettings.ThermalSettings.ThermalSet1.OrgData;
		thermalParam_1.actime =  p_req->ImagingSettings.ThermalSettings.ThermalSet1.Actime;

		setThermalParam1(&thermalParam_1);
	}
	else if (p_req->ImagingSettings.ThermalSettings.ThermalSet_ext2Flag == 1) {
	   	/* 
		printf("Emissivity 发射率:%0.2f \n", p_req->ImagingSettings.ThermalSettings.ThermalSet2.Emissivity);
		printf("Distance 距离:%0.2f \n", p_req->ImagingSettings.ThermalSettings.ThermalSet2.Distance);
		printf("Humidity 湿度:%0.2f \n", p_req->ImagingSettings.ThermalSettings.ThermalSet2.Humidity);
		printf("Correction 修正:%0.2f \n", p_req->ImagingSettings.ThermalSettings.ThermalSet2.Correction);
		printf("Reflection 反射温度:%0.2f \n", p_req->ImagingSettings.ThermalSettings.ThermalSet2.Reflection);
		printf("Amb 环境温度:%0.2f \n", p_req->ImagingSettings.ThermalSettings.ThermalSet2.Amb); */
		ThermalParam2_t thermalParam_2;
		memset(&thermalParam_2, 0, sizeof(ThermalParam2_t));

		thermalParam_2.emissivity =	p_req->ImagingSettings.ThermalSettings.ThermalSet2.Emissivity;
		thermalParam_2.distance =	p_req->ImagingSettings.ThermalSettings.ThermalSet2.Distance;
		thermalParam_2.humidity =	p_req->ImagingSettings.ThermalSettings.ThermalSet2.Humidity;
		thermalParam_2.correction =	p_req->ImagingSettings.ThermalSettings.ThermalSet2.Correction;
		thermalParam_2.reflection =	p_req->ImagingSettings.ThermalSettings.ThermalSet2.Reflection;
		thermalParam_2.amb =	p_req->ImagingSettings.ThermalSettings.ThermalSet2.Amb;

		setThermalParam2(&thermalParam_2);
	}
			
	if ( p_req->ImagingSettings.DulaInformationFlag == 1 )
	{
		// onvif_DulaInformation  onvifDulaInfo;
		DulaInformation_t onvifDulaInfo;
	    memset(&onvifDulaInfo, 0, sizeof(DulaInformation_t));

		onvifDulaInfo.focal = p_req->ImagingSettings.DulaInfo.focal;
		onvifDulaInfo.lens  =  p_req->ImagingSettings.DulaInfo.lens;
		onvifDulaInfo.distance = p_req->ImagingSettings.DulaInfo.distance;
		onvifDulaInfo.dula_model = 	p_req->ImagingSettings.DulaInfo.dula_model;
		onvifDulaInfo.x  = p_req->ImagingSettings.DulaInfo.x;
		onvifDulaInfo.y  = p_req->ImagingSettings.DulaInfo.y;
		onvifDulaInfo.scale = p_req->ImagingSettings.DulaInfo.scale;
		/* printf("focal:%d, lens:%0.2f, distance:%0.2f, dula_model:%d, x:%d, y:%d, xscale:%0.2f\n", onvifDulaInfo.focal, onvifDulaInfo.lens,
						onvifDulaInfo.distance, onvifDulaInfo.dula_model, onvifDulaInfo.x, onvifDulaInfo.y, onvifDulaInfo.scale); */

		setDulaParam(&onvifDulaInfo);
	}


	// save image setting
	if (p_req->ImagingSettings.BacklightCompensationFlag)
	{
		g_onvif_cfg.ImagingSettings.BacklightCompensation.Mode = p_req->ImagingSettings.BacklightCompensation.Mode;
		
		if (p_req->ImagingSettings.BacklightCompensation.LevelFlag)
		{
			g_onvif_cfg.ImagingSettings.BacklightCompensation.Level = p_req->ImagingSettings.BacklightCompensation.Level;
		}
	}

	if (p_req->ImagingSettings.BrightnessFlag)
	{
		g_onvif_cfg.ImagingSettings.Brightness = p_req->ImagingSettings.Brightness;
	}

	if (p_req->ImagingSettings.ColorSaturationFlag)
	{
		g_onvif_cfg.ImagingSettings.ColorSaturation = p_req->ImagingSettings.ColorSaturation;
	}

	if (p_req->ImagingSettings.ContrastFlag)
	{
		g_onvif_cfg.ImagingSettings.Contrast = p_req->ImagingSettings.Contrast;
	}

	if (p_req->ImagingSettings.ExposureFlag)
	{
		g_onvif_cfg.ImagingSettings.Exposure.Mode = p_req->ImagingSettings.Exposure.Mode;

		if (p_req->ImagingSettings.Exposure.PriorityFlag)
		{
			g_onvif_cfg.ImagingSettings.Exposure.Priority = p_req->ImagingSettings.Exposure.Priority;
		}

		if (p_req->ImagingSettings.Exposure.MinExposureTimeFlag)
		{
			g_onvif_cfg.ImagingSettings.Exposure.MinExposureTime = p_req->ImagingSettings.Exposure.MinExposureTime;
		}

		if (p_req->ImagingSettings.Exposure.MaxExposureTimeFlag)
		{
			g_onvif_cfg.ImagingSettings.Exposure.MaxExposureTime = p_req->ImagingSettings.Exposure.MaxExposureTime;
		}

		if (p_req->ImagingSettings.Exposure.MinGainFlag)
		{
			g_onvif_cfg.ImagingSettings.Exposure.MinGain = p_req->ImagingSettings.Exposure.MinGain;
		}

		if (p_req->ImagingSettings.Exposure.MaxGainFlag)
		{
			g_onvif_cfg.ImagingSettings.Exposure.MaxGain = p_req->ImagingSettings.Exposure.MaxGain;
		}

		if (p_req->ImagingSettings.Exposure.MinIrisFlag)
		{
			g_onvif_cfg.ImagingSettings.Exposure.MinIris = p_req->ImagingSettings.Exposure.MinIris;
		}

		if (p_req->ImagingSettings.Exposure.MaxIrisFlag)
		{
			g_onvif_cfg.ImagingSettings.Exposure.MaxIris = p_req->ImagingSettings.Exposure.MaxIris;
		}

		if (p_req->ImagingSettings.Exposure.ExposureTimeFlag)
		{
			g_onvif_cfg.ImagingSettings.Exposure.ExposureTime = p_req->ImagingSettings.Exposure.ExposureTime;
		}

		if (p_req->ImagingSettings.Exposure.GainFlag)
		{
			g_onvif_cfg.ImagingSettings.Exposure.Gain = p_req->ImagingSettings.Exposure.Gain;
		}

		if (p_req->ImagingSettings.Exposure.IrisFlag)
		{
			g_onvif_cfg.ImagingSettings.Exposure.Iris = p_req->ImagingSettings.Exposure.Iris;
		}
	}

	if (p_req->ImagingSettings.FocusFlag)
	{
		g_onvif_cfg.ImagingSettings.Focus.AutoFocusMode = p_req->ImagingSettings.Focus.AutoFocusMode;

		if (p_req->ImagingSettings.Focus.DefaultSpeedFlag)
		{
			g_onvif_cfg.ImagingSettings.Focus.DefaultSpeed = p_req->ImagingSettings.Focus.DefaultSpeed;
		}

		if (p_req->ImagingSettings.Focus.NearLimitFlag)
		{
			g_onvif_cfg.ImagingSettings.Focus.NearLimit = p_req->ImagingSettings.Focus.NearLimit;
		}

		if (p_req->ImagingSettings.Focus.FarLimitFlag)
		{
			g_onvif_cfg.ImagingSettings.Focus.FarLimit = p_req->ImagingSettings.Focus.FarLimit;
		}
	}

	if (p_req->ImagingSettings.IrCutFilterFlag)
	{
		g_onvif_cfg.ImagingSettings.IrCutFilter = p_req->ImagingSettings.IrCutFilter;
	}

	if (p_req->ImagingSettings.SharpnessFlag)
	{
		g_onvif_cfg.ImagingSettings.Sharpness = p_req->ImagingSettings.Sharpness;
	}

	if (p_req->ImagingSettings.WideDynamicRangeFlag)
	{
		g_onvif_cfg.ImagingSettings.WideDynamicRange.Mode = p_req->ImagingSettings.WideDynamicRange.Mode;

		if (p_req->ImagingSettings.WideDynamicRange.LevelFlag)
		{
			g_onvif_cfg.ImagingSettings.WideDynamicRange.Level = p_req->ImagingSettings.WideDynamicRange.Level;
		}
	}

	if (p_req->ImagingSettings.WhiteBalanceFlag)
	{
		g_onvif_cfg.ImagingSettings.WhiteBalance.Mode = p_req->ImagingSettings.WhiteBalance.Mode;

		if (p_req->ImagingSettings.WhiteBalance.CrGainFlag)
		{
			g_onvif_cfg.ImagingSettings.WhiteBalance.CrGain = p_req->ImagingSettings.WhiteBalance.CrGain;
		}

		if (p_req->ImagingSettings.WhiteBalance.CbGainFlag)
		{
			g_onvif_cfg.ImagingSettings.WhiteBalance.CbGain = p_req->ImagingSettings.WhiteBalance.CbGain;
		}
	}

	//// add by xieqingpu
	if (p_req->ImagingSettings.ThermalSettings_extFlag)
	{
		g_onvif_cfg.ImagingSettings.ThermalSettings_extFlag = p_req->ImagingSettings.ThermalSettings_extFlag;
		//
		if ( p_req->ImagingSettings.ThermalSettings.ThermalSet_ext1Flag == 1 ){
			g_onvif_cfg.ImagingSettings.ThermalSettings.ThermalSet_ext1Flag =  p_req->ImagingSettings.ThermalSettings.ThermalSet_ext1Flag;

			g_onvif_cfg.ImagingSettings.ThermalSettings.ThermalSet1.UserPalette =  p_req->ImagingSettings.ThermalSettings.ThermalSet1.UserPalette;
			g_onvif_cfg.ImagingSettings.ThermalSettings.ThermalSet1.WideDynamic =  p_req->ImagingSettings.ThermalSettings.ThermalSet1.WideDynamic;
			g_onvif_cfg.ImagingSettings.ThermalSettings.ThermalSet1.OrgData =  p_req->ImagingSettings.ThermalSettings.ThermalSet1.OrgData;
			g_onvif_cfg.ImagingSettings.ThermalSettings.ThermalSet1.Actime =  p_req->ImagingSettings.ThermalSettings.ThermalSet1.Actime;
		}

		if(p_req->ImagingSettings.ThermalSettings.ThermalSet_ext2Flag == 1){
			g_onvif_cfg.ImagingSettings.ThermalSettings.ThermalSet_ext2Flag =  p_req->ImagingSettings.ThermalSettings.ThermalSet_ext2Flag;

			g_onvif_cfg.ImagingSettings.ThermalSettings.ThermalSet2.Emissivity = p_req->ImagingSettings.ThermalSettings.ThermalSet2.Emissivity;
			g_onvif_cfg.ImagingSettings.ThermalSettings.ThermalSet2.Distance = p_req->ImagingSettings.ThermalSettings.ThermalSet2.Distance;
			g_onvif_cfg.ImagingSettings.ThermalSettings.ThermalSet2.Humidity = p_req->ImagingSettings.ThermalSettings.ThermalSet2.Humidity;
			g_onvif_cfg.ImagingSettings.ThermalSettings.ThermalSet2.Correction = p_req->ImagingSettings.ThermalSettings.ThermalSet2.Correction;
			g_onvif_cfg.ImagingSettings.ThermalSettings.ThermalSet2.Reflection = p_req->ImagingSettings.ThermalSettings.ThermalSet2.Reflection;
			g_onvif_cfg.ImagingSettings.ThermalSettings.ThermalSet2.Amb = p_req->ImagingSettings.ThermalSettings.ThermalSet2.Amb;
		}
	}

	if ( p_req->ImagingSettings.DulaInformationFlag == 1 ){
		g_onvif_cfg.ImagingSettings.DulaInformationFlag =  p_req->ImagingSettings.DulaInformationFlag;

		g_onvif_cfg.ImagingSettings.DulaInfo.focal = p_req->ImagingSettings.DulaInfo.focal;
		g_onvif_cfg.ImagingSettings.DulaInfo.lens  =  p_req->ImagingSettings.DulaInfo.lens;
		g_onvif_cfg.ImagingSettings.DulaInfo.distance = p_req->ImagingSettings.DulaInfo.distance;
		g_onvif_cfg.ImagingSettings.DulaInfo.dula_model = 	p_req->ImagingSettings.DulaInfo.dula_model;
		g_onvif_cfg.ImagingSettings.DulaInfo.x  = p_req->ImagingSettings.DulaInfo.x;
		g_onvif_cfg.ImagingSettings.DulaInfo.y  = p_req->ImagingSettings.DulaInfo.y;
		g_onvif_cfg.ImagingSettings.DulaInfo.scale = p_req->ImagingSettings.DulaInfo.scale;
	}
	////
	
	return ONVIF_OK;
}

ONVIF_RET onvif_Move(Move_REQ * p_req)
{
	ONVIF_VideoSource * p_v_src = onvif_find_VideoSource(p_req->VideoSourceToken);
	if (NULL == p_v_src)
	{
		return ONVIF_ERR_NoSource;
	}

    if (p_req->Focus.AbsoluteFlag || p_req->Focus.RelativeFlag)
    {
        return ONVIF_ERR_NotSupported;
    }

    if (p_req->Focus.ContinuousFlag)
    {
        // check the parameter range 
        if (p_req->Focus.Continuous.Speed < -50.0f ||
            p_req->Focus.Continuous.Speed > 50.0f)
        {
            return ONVIF_ERR_SettingsInvalid;
        }
    }
	
	// todo : add move code ...
	printf("\nfocus : %0.2f\n", p_req->Focus.Continuous.Speed);
	float zSpeed = p_req->Focus.Continuous.Speed;
	focusMove(zSpeed);

    
	return ONVIF_OK;
}

ONVIF_RET onvif_img_Stop(const char * VideoSourceToken)
{
    ONVIF_VideoSource * p_v_src = onvif_find_VideoSource(VideoSourceToken);
	if (NULL == p_v_src)
	{
		return ONVIF_ERR_NoSource;
	}

    // todo : add stop move code ...
    
	return ONVIF_OK;
}

ONVIF_RET onvif_img_GetStatus(ONVIF_VideoSource * p_v_src, onvif_ImagingStatus * p_status)
{
	// todo : add get imaging status code ...

	p_status->FocusStatusFlag = 1;
	p_status->FocusStatus.Position = 0.0;
	p_status->FocusStatus.MoveStatus = MoveStatus_IDLE;

	return ONVIF_OK;
}

ONVIF_RET onvif_GetMoveOptions(ONVIF_VideoSource * p_v_src, onvif_MoveOptions20 * p_option)
{
    // todo : add get imaging move options code ...

    p_option->ContinuousFlag = 1;
    p_option->Continuous.Speed.Min = 1.0f;
    p_option->Continuous.Speed.Max = 5.0f;

    return ONVIF_OK;
}




