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

#ifndef ONVIF_ANALYTICS_H
#define ONVIF_ANALYTICS_H


/***************************************************************************************/

typedef struct 
{
	char 	ProfileToken[ONVIF_TOKEN_LEN];					// required, Reference to the profile where the configuration should be added
	char 	ConfigurationToken[ONVIF_TOKEN_LEN];			// required, Contains a reference to the VideoAnalyticsConfiguration to add
} AddVideoAnalyticsConfiguration_REQ;

typedef struct 
{
	char 	ConfigurationToken[ONVIF_TOKEN_LEN];			// required, The requested video analytics configuration
} GetVideoAnalyticsConfiguration_REQ;

typedef struct 
{
	char 	ProfileToken[ONVIF_TOKEN_LEN];					// required, Contains a reference to the media profile from which the VideoAnalyticsConfiguration shall be removed
} RemoveVideoAnalyticsConfiguration_REQ;

typedef struct
{
	onvif_VideoAnalyticsConfiguration	Configuration;		// required, Contains the modified video analytics configuration. The configuration shall exist in the device

	BOOL 	ForcePersistence;								// required, The ForcePersistence element is obsolete and should always be assumed to be true
} SetVideoAnalyticsConfiguration_REQ;

typedef struct
{
	char 	ConfigurationToken[ONVIF_TOKEN_LEN];			// required, References an existing Video Analytics configuration. The list of available tokens can be obtained
															//	via the Media service GetVideoAnalyticsConfigurations method
} GetSupportedRules_REQ;

typedef struct
{
	onvif_SupportedRules 	SupportedRules;					// required 
} GetSupportedRules_RES;

typedef struct
{
	char 	ConfigurationToken[ONVIF_TOKEN_LEN];			// required, Reference to an existing VideoAnalyticsConfiguration

	ONVIF_Config * Rule;									// required
} CreateRules_REQ;

typedef struct 
{
	char 	ConfigurationToken[ONVIF_TOKEN_LEN];			// required, Reference to an existing VideoAnalyticsConfiguration

	int 	sizeRuleName;
	char 	RuleName[10][ONVIF_NAME_LEN];					// required, References the specific rule to be deleted (e.g. "MyLineDetector"). 
} DeleteRules_REQ;

typedef struct 
{
	char	ConfigurationToken[ONVIF_TOKEN_LEN];			// required, Reference to an existing VideoAnalyticsConfiguration
} GetRules_REQ;

typedef struct 
{
	ONVIF_Config * Rule;									// optional
} GetRules_RES;

typedef struct 
{
	char 	ConfigurationToken[ONVIF_TOKEN_LEN];			// required, Reference to an existing VideoAnalyticsConfiguration

	ONVIF_Config * Rule;									// required 
} ModifyRules_REQ;

typedef struct 
{
	char 	ConfigurationToken[ONVIF_TOKEN_LEN];			// required, Reference to an existing VideoAnalyticsConfiguration

	ONVIF_Config * AnalyticsModule;							// required 
} CreateAnalyticsModules_REQ;

typedef struct 
{
	char 	ConfigurationToken[ONVIF_TOKEN_LEN];			// required, Reference to an existing Video Analytics configuration

	int 	sizeAnalyticsModuleName;
	char 	AnalyticsModuleName[10][ONVIF_NAME_LEN];		//required, name of the AnalyticsModule to be deleted
} DeleteAnalyticsModules_REQ;

typedef struct 
{
	char 	ConfigurationToken[ONVIF_TOKEN_LEN];			// required, Reference to an existing VideoAnalyticsConfiguration
} GetAnalyticsModules_REQ;

typedef struct
{
	ONVIF_Config * AnalyticsModule;							// optional
} GetAnalyticsModules_RES;

typedef struct 
{
	char 	ConfigurationToken[ONVIF_TOKEN_LEN];			// required, Reference to an existing VideoAnalyticsConfiguration
	
	ONVIF_Config * AnalyticsModule;							// required 
} ModifyAnalyticsModules_REQ;

typedef struct 
{
	char    RuleType[100];	                                // optional, Reference to an SupportedRule Type returned from GetSupportedRules
	char    ConfigurationToken[ONVIF_TOKEN_LEN];	        // required, Reference to an existing analytics configuration
} GetRuleOptions_REQ;

typedef struct 
{
    char    ConfigurationToken[ONVIF_TOKEN_LEN];	        // required, Reference to an existing VideoAnalyticsConfiguration
} GetSupportedAnalyticsModules_REQ;

typedef struct
{
    char    Type[128];	                                    // required, Reference to an SupportedAnalyticsModule Type returned from GetSupportedAnalyticsModules
	char    ConfigurationToken[ONVIF_TOKEN_LEN];	        // required, Reference to an existing AnalyticsConfiguration
} GetAnalyticsModuleOptions_REQ;


#ifdef __cplusplus
extern "C" {
#endif

ONVIF_RET onvif_AddVideoAnalyticsConfiguration(AddVideoAnalyticsConfiguration_REQ * p_req);
ONVIF_RET onvif_RemoveVideoAnalyticsConfiguration(RemoveVideoAnalyticsConfiguration_REQ * p_req);
ONVIF_RET onvif_SetVideoAnalyticsConfiguration(SetVideoAnalyticsConfiguration_REQ * p_req);

ONVIF_RET onvif_GetSupportedRules(GetSupportedRules_REQ * p_req, GetSupportedRules_RES * p_res);
ONVIF_RET onvif_CreateRules(CreateRules_REQ * p_req);
ONVIF_RET onvif_DeleteRules(DeleteRules_REQ * p_req);
ONVIF_RET onvif_GetRules(GetRules_REQ * p_req, GetRules_RES * p_res);
ONVIF_RET onvif_ModifyRules(ModifyRules_REQ * p_req);
ONVIF_RET onvif_CreateAnalyticsModules(CreateAnalyticsModules_REQ * p_req);
ONVIF_RET onvif_DeleteAnalyticsModules(DeleteAnalyticsModules_REQ * p_req);
ONVIF_RET onvif_GetAnalyticsModules(GetAnalyticsModules_REQ * p_req, GetAnalyticsModules_RES * p_res);
ONVIF_RET onvif_ModifyAnalyticsModules(ModifyAnalyticsModules_REQ * p_req);

#ifdef __cplusplus
}
#endif

#endif 	// end of ONVIF_ANALYTICS_H



