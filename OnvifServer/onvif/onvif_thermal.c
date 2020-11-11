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
#include "onvif_thermal.h"

#ifdef THERMAL_SUPPORT

/**
 * The possible return values:
 *	ONVIF_ERR_NoSource
 *	ONVIF_ERR_NoThermalForSource
 *	ONVIF_ERR_InvalidConfiguration
**/ 
ONVIF_RET onvif_tth_SetConfiguration(tth_SetConfiguration_REQ * p_req)
{
    ONVIF_VideoSource * p_v_src = onvif_find_VideoSource(p_req->VideoSourceToken);
    if (NULL == p_v_src)
    {
        return ONVIF_ERR_NoSource;
    }

    if (p_v_src->ThermalSupport == FALSE)
    {
        return ONVIF_ERR_NoThermalForSource;
    }

    strcpy(p_v_src->ThermalConfiguration.ColorPalette.token, p_req->Configuration.ColorPalette.token);
    strcpy(p_v_src->ThermalConfiguration.ColorPalette.Name, p_req->Configuration.ColorPalette.Name);
    strcpy(p_v_src->ThermalConfiguration.ColorPalette.Type, p_req->Configuration.ColorPalette.Type);
    p_v_src->ThermalConfiguration.Polarity = p_req->Configuration.Polarity;

    if (p_req->Configuration.NUCTableFlag)
    {
        strcpy(p_v_src->ThermalConfiguration.NUCTable.token, p_req->Configuration.NUCTable.token);
        strcpy(p_v_src->ThermalConfiguration.NUCTable.Name, p_req->Configuration.NUCTable.Name);
        
        if (p_req->Configuration.NUCTable.LowTemperatureFlag)
        {
            p_v_src->ThermalConfiguration.NUCTable.LowTemperature = p_req->Configuration.NUCTable.LowTemperature;
        }

        if (p_req->Configuration.NUCTable.HighTemperatureFlag)
        {
            p_v_src->ThermalConfiguration.NUCTable.HighTemperature = p_req->Configuration.NUCTable.HighTemperature;
        }
    }

    if (p_req->Configuration.CoolerFlag)
    {
        p_v_src->ThermalConfiguration.Cooler.Enabled = p_req->Configuration.Cooler.Enabled;
    }

    // todo : add handler code ...
    
    return ONVIF_OK;
}

/**
 * The possible return values:
 *	ONVIF_ERR_NoSource
 *	ONVIF_ERR_NoRadiometryForSource
 *	ONVIF_ERR_InvalidConfiguration
**/ 
ONVIF_RET onvif_tth_SetRadiometryConfiguration(tth_SetRadiometryConfiguration_REQ * p_req)
{
    ONVIF_VideoSource * p_v_src = onvif_find_VideoSource(p_req->VideoSourceToken);
    if (NULL == p_v_src)
    {
        return ONVIF_ERR_NoSource;
    }

    if (p_v_src->ThermalSupport == FALSE)
    {
        return ONVIF_ERR_NoRadiometryForSource;
    }

    if (p_req->Configuration.RadiometryGlobalParametersFlag && p_v_src->RadiometryConfigurationOptions.RadiometryGlobalParameterOptionsFlag)
    {
        if (p_req->Configuration.RadiometryGlobalParameters.ReflectedAmbientTemperature < 
            p_v_src->RadiometryConfigurationOptions.RadiometryGlobalParameterOptions.ReflectedAmbientTemperature.Min ||
            p_req->Configuration.RadiometryGlobalParameters.ReflectedAmbientTemperature > 
            p_v_src->RadiometryConfigurationOptions.RadiometryGlobalParameterOptions.ReflectedAmbientTemperature.Max)
        {
            return ONVIF_ERR_InvalidConfiguration;
        }

        if (p_req->Configuration.RadiometryGlobalParameters.Emissivity < 
            p_v_src->RadiometryConfigurationOptions.RadiometryGlobalParameterOptions.Emissivity.Min ||
            p_req->Configuration.RadiometryGlobalParameters.Emissivity > 
            p_v_src->RadiometryConfigurationOptions.RadiometryGlobalParameterOptions.Emissivity.Max)
        {
            return ONVIF_ERR_InvalidConfiguration;
        }

        if (p_req->Configuration.RadiometryGlobalParameters.DistanceToObject < 
            p_v_src->RadiometryConfigurationOptions.RadiometryGlobalParameterOptions.DistanceToObject.Min ||
            p_req->Configuration.RadiometryGlobalParameters.DistanceToObject > 
            p_v_src->RadiometryConfigurationOptions.RadiometryGlobalParameterOptions.DistanceToObject.Max)
        {
            return ONVIF_ERR_InvalidConfiguration;
        }

        if (p_req->Configuration.RadiometryGlobalParameters.RelativeHumidityFlag && 
            p_v_src->RadiometryConfigurationOptions.RadiometryGlobalParameterOptions.RelativeHumidityFlag)
        {            
            if (p_req->Configuration.RadiometryGlobalParameters.RelativeHumidity < 
                p_v_src->RadiometryConfigurationOptions.RadiometryGlobalParameterOptions.RelativeHumidity.Min ||
                p_req->Configuration.RadiometryGlobalParameters.RelativeHumidity > 
                p_v_src->RadiometryConfigurationOptions.RadiometryGlobalParameterOptions.RelativeHumidity.Max)
            {
                return ONVIF_ERR_InvalidConfiguration;
            }
        }

        if (p_req->Configuration.RadiometryGlobalParameters.AtmosphericTemperatureFlag && 
            p_v_src->RadiometryConfigurationOptions.RadiometryGlobalParameterOptions.AtmosphericTemperatureFlag)
        {            
            if (p_req->Configuration.RadiometryGlobalParameters.AtmosphericTemperature < 
                p_v_src->RadiometryConfigurationOptions.RadiometryGlobalParameterOptions.AtmosphericTemperature.Min ||
                p_req->Configuration.RadiometryGlobalParameters.AtmosphericTemperature > 
                p_v_src->RadiometryConfigurationOptions.RadiometryGlobalParameterOptions.AtmosphericTemperature.Max)
            {
                return ONVIF_ERR_InvalidConfiguration;
            }
        }

        if (p_req->Configuration.RadiometryGlobalParameters.AtmosphericTransmittanceFlag && 
            p_v_src->RadiometryConfigurationOptions.RadiometryGlobalParameterOptions.AtmosphericTransmittanceFlag)
        {            
            if (p_req->Configuration.RadiometryGlobalParameters.AtmosphericTransmittance < 
                p_v_src->RadiometryConfigurationOptions.RadiometryGlobalParameterOptions.AtmosphericTransmittance.Min ||
                p_req->Configuration.RadiometryGlobalParameters.AtmosphericTransmittance > 
                p_v_src->RadiometryConfigurationOptions.RadiometryGlobalParameterOptions.AtmosphericTransmittance.Max)
            {
                return ONVIF_ERR_InvalidConfiguration;
            }
        }

        if (p_req->Configuration.RadiometryGlobalParameters.ExtOpticsTemperatureFlag && 
            p_v_src->RadiometryConfigurationOptions.RadiometryGlobalParameterOptions.ExtOpticsTemperatureFlag)
        {            
            if (p_req->Configuration.RadiometryGlobalParameters.ExtOpticsTemperature < 
                p_v_src->RadiometryConfigurationOptions.RadiometryGlobalParameterOptions.ExtOpticsTemperature.Min ||
                p_req->Configuration.RadiometryGlobalParameters.ExtOpticsTemperature > 
                p_v_src->RadiometryConfigurationOptions.RadiometryGlobalParameterOptions.ExtOpticsTemperature.Max)
            {
                return ONVIF_ERR_InvalidConfiguration;
            }
        }

        if (p_req->Configuration.RadiometryGlobalParameters.ExtOpticsTransmittanceFlag && 
            p_v_src->RadiometryConfigurationOptions.RadiometryGlobalParameterOptions.ExtOpticsTransmittanceFlag)
        {            
            if (p_req->Configuration.RadiometryGlobalParameters.ExtOpticsTransmittance < 
                p_v_src->RadiometryConfigurationOptions.RadiometryGlobalParameterOptions.ExtOpticsTransmittance.Min ||
                p_req->Configuration.RadiometryGlobalParameters.ExtOpticsTransmittance > 
                p_v_src->RadiometryConfigurationOptions.RadiometryGlobalParameterOptions.ExtOpticsTransmittance.Max)
            {
                return ONVIF_ERR_InvalidConfiguration;
            }
        }

        memcpy(&p_v_src->RadiometryConfiguration, &p_req->Configuration, sizeof(onvif_RadiometryConfiguration));
    }

    // todo : add handler code ...
    
    return ONVIF_OK;
}

#endif // end of THERMAL_SUPPORT



