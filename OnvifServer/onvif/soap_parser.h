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

#ifndef _SOAP_PARSER_H_
#define _SOAP_PARSER_H_

/***************************************************************************************/
#include "xml_node.h"
#include "onvif.h"
#include "onvif_ptz.h"
#include "onvif_device.h"
#include "onvif_media.h"
#include "onvif_event.h"
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

#ifdef __cplusplus
extern "C" {
#endif

/***************************************************************************************/
BOOL parse_Bool(const char * pdata);

ONVIF_RET parse_GetServices(XMLN * p_node, GetServices_REQ * p_req);
ONVIF_RET parse_AddScopes(XMLN * p_AddScopes, AddScopes_REQ * p_req);
ONVIF_RET parse_SetScopes(XMLN * p_AddScopes, SetScopes_REQ * p_req);
ONVIF_RET parse_RemoveScopes(XMLN * p_AddScopes, RemoveScopes_REQ * p_req);
ONVIF_RET parse_SetDiscoveryMode(XMLN * p_SetDiscoveryMode, SetDiscoveryMode_REQ * p_req);
ONVIF_RET parse_Subscribe(XMLN * p_node, Subscribe_REQ * p_req);
ONVIF_RET parse_Renew(XMLN * p_node, Renew_REQ * p_req);
ONVIF_RET parse_CreatePullPointSubscription(XMLN * p_node, CreatePullPointSubscription_REQ * p_req);
ONVIF_RET parse_PullMessages(XMLN * p_node, PullMessages_REQ * p_req);
ONVIF_RET parse_SetDNS(XMLN * p_node, SetDNS_REQ * p_req);
ONVIF_RET parse_SetNTP(XMLN * p_node, SetNTP_REQ * p_req);
ONVIF_RET parse_SetZeroConfiguration(XMLN * p_node, SetZeroConfiguration_REQ * p_req);
ONVIF_RET parse_GetDot11Status(XMLN * p_node, GetDot11Status_REQ * p_req);
ONVIF_RET parse_GetSnapshotUri(XMLN * p_node, GetSnapshotUri_REQ * p_req);
ONVIF_RET parse_ScanAvailableDot11Networks(XMLN * p_node, ScanAvailableDot11Networks_REQ * p_req);
ONVIF_RET parse_SetNetworkProtocols(XMLN * p_node, SetNetworkProtocols_REQ * p_req);
ONVIF_RET parse_SetNetworkDefaultGateway(XMLN * p_node, SetNetworkDefaultGateway_REQ * p_req);
ONVIF_RET parse_GetSystemLog(XMLN * p_node, GetSystemLog_REQ * p_req);
ONVIF_RET parse_SetSystemDateAndTime(XMLN * p_node, SetSystemDateAndTime_REQ * p_req);
ONVIF_RET parse_GetCapabilities(XMLN * p_node, GetCapabilities_REQ * p_req);
ONVIF_RET parse_GetProfile(XMLN * p_node, GetProfile_REQ * p_req);
ONVIF_RET parse_CreateProfile(XMLN * p_node, CreateProfile_REQ * p_req);
ONVIF_RET parse_DeleteProfile(XMLN * p_node, DeleteProfile_REQ * p_req);
ONVIF_RET parse_GetStreamUri(XMLN * p_node, GetStreamUri_REQ * p_req);
ONVIF_RET parse_SetNetworkInterfaces(XMLN * p_node, SetNetworkInterfaces_REQ * p_req);
ONVIF_RET parse_AddVideoSourceConfiguration(XMLN * p_node, AddVideoSourceConfiguration_REQ * p_req);
ONVIF_RET parse_RemoveVideoSourceConfiguration(XMLN * p_node, RemoveVideoSourceConfiguration_REQ * p_req);
ONVIF_RET parse_AddVideoEncoderConfiguration(XMLN * p_node, AddVideoEncoderConfiguration_REQ * p_req);
ONVIF_RET parse_RemoveVideoEncoderConfiguration(XMLN * p_node, RemoveVideoEncoderConfiguration_REQ * p_req);
ONVIF_RET parse_GetVideoSourceConfigurationOptions(XMLN * p_node, GetVideoSourceConfigurationOptions_REQ * p_req);
ONVIF_RET parse_SetVideoSourceConfiguration(XMLN * p_node, SetVideoSourceConfiguration_REQ * p_req);
ONVIF_RET parse_GetVideoEncoderConfigurationOptions(XMLN * p_node, GetVideoEncoderConfigurationOptions_REQ * p_req);
ONVIF_RET parse_SetVideoEncoderConfiguration(XMLN * p_SetVideoEncoderConfiguration, SetVideoEncoderConfiguration_REQ * p_req);
ONVIF_RET parse_SetImagingSettings(XMLN * p_node, SetImagingSettings_REQ * p_req);
ONVIF_RET parse_Move(XMLN * p_node, Move_REQ * p_req);
ONVIF_RET parse_CreateUsers(XMLN * p_AddScopes, CreateUsers_REQ * p_req);
ONVIF_RET parse_DeleteUsers(XMLN * p_node, DeleteUsers_REQ * p_req);
ONVIF_RET parse_SetUser(XMLN * p_node, SetUser_REQ * p_req);
ONVIF_RET parse_SetRemoteUser(XMLN * p_node, SetRemoteUser_REQ * p_req);
ONVIF_RET parse_GetOSDs(XMLN * p_node, GetOSDs_REQ * p_req);
ONVIF_RET parse_GetOSD(XMLN * p_node, GetOSD_REQ * p_req);
ONVIF_RET parse_SetOSD(XMLN * p_node, SetOSD_REQ * p_req);
ONVIF_RET parse_CreateOSD(XMLN * p_node, CreateOSD_REQ * p_req);
ONVIF_RET parse_DeleteOSD(XMLN * p_node, DeleteOSD_REQ * p_req);
ONVIF_RET parse_GetMetadataConfigurationOptions(XMLN * p_node, GetMetadataConfigurationOptions_REQ * p_req);
ONVIF_RET parse_SetMetadataConfiguration(XMLN * p_node, SetMetadataConfiguration_REQ * p_req);
ONVIF_RET parse_AddMetadataConfiguration(XMLN * p_node, AddMetadataConfiguration_REQ * p_req);
ONVIF_RET parse_GetVideoSourceModes(XMLN * p_node, GetVideoSourceModes_REQ * p_req);
ONVIF_RET parse_SetVideoSourceMode(XMLN * p_node, SetVideoSourceMode_REQ * p_req);
ONVIF_RET parse_SetSynchronizationPoint(XMLN * p_node, SetSynchronizationPoint_REQ * p_req);

#ifdef IPFILTER_SUPPORT

ONVIF_RET parse_SetIPAddressFilter(XMLN * p_node, SetIPAddressFilter_REQ * p_req);
ONVIF_RET parse_AddIPAddressFilter(XMLN * p_node, AddIPAddressFilter_REQ * p_req);
ONVIF_RET parse_RemoveIPAddressFilter(XMLN * p_node, RemoveIPAddressFilter_REQ * p_req);

#endif // end of IPFILTER_SUPPORT

#ifdef AUDIO_SUPPORT

ONVIF_RET parse_AddAudioSourceConfiguration(XMLN * p_node, AddAudioSourceConfiguration_REQ * p_req);
ONVIF_RET parse_AddAudioEncoderConfiguration(XMLN * p_node, AddAudioEncoderConfiguration_REQ * p_req);
ONVIF_RET parse_GetAudioSourceConfigurationOptions(XMLN * p_node, GetAudioSourceConfigurationOptions_REQ * p_req);
ONVIF_RET parse_SetAudioSourceConfiguration(XMLN * p_node, SetAudioSourceConfiguration_REQ * p_req);
ONVIF_RET parse_GetAudioEncoderConfigurationOptions(XMLN * p_node, GetAudioEncoderConfigurationOptions_REQ * p_req);
ONVIF_RET parse_SetAudioEncoderConfiguration(XMLN * p_node, SetAudioEncoderConfiguration_REQ * p_req);
ONVIF_RET parse_AddAudioDecoderConfiguration(XMLN * p_node, AddAudioDecoderConfiguration_REQ * p_req);
ONVIF_RET parse_RemoveAudioDecoderConfiguration(XMLN * p_node, RemoveAudioDecoderConfiguration_REQ * p_req);
ONVIF_RET parse_SetAudioDecoderConfiguration(XMLN * p_node, SetAudioDecoderConfiguration_REQ * p_req);
ONVIF_RET parse_GetAudioDecoderConfigurationOptions(XMLN * p_node, GetAudioDecoderConfigurationOptions_REQ * p_req);

#endif // end of AUDIO_SUPPORT

#ifdef PTZ_SUPPORT

ONVIF_RET parse_AddPTZConfiguration(XMLN * p_node, AddPTZConfiguration_REQ * p_req);
ONVIF_RET parse_GetCompatibleConfigurations(XMLN * p_node, GetCompatibleConfigurations_REQ * p_req);
ONVIF_RET parse_SetConfiguration(XMLN * p_node, SetConfiguration_REQ * p_req);
ONVIF_RET parse_ContinuousMove(XMLN * p_node, ContinuousMove_REQ * p_req);
ONVIF_RET parse_ptz_Stop(XMLN * p_node, PTZ_Stop_REQ * p_req);
ONVIF_RET parse_AbsoluteMove(XMLN * p_node, AbsoluteMove_REQ * p_req);
ONVIF_RET parse_RelativeMove(XMLN * p_node, RelativeMove_REQ * p_req);
ONVIF_RET parse_SetPreset(XMLN * p_node, SetPreset_REQ * p_req);
ONVIF_RET parse_RemovePreset(XMLN * p_node, RemovePreset_REQ * p_req);
ONVIF_RET parse_GotoPreset(XMLN * p_node, GotoPreset_REQ * p_req);
ONVIF_RET parse_GotoHomePosition(XMLN * p_node, GotoHomePosition_REQ * p_req);

#endif // end of PTZ_SUPPORT

#ifdef PROFILE_G_SUPPORT

ONVIF_RET parse_CreateRecording(XMLN * p_node, CreateRecording_REQ * p_req);
ONVIF_RET parse_SetRecordingConfiguration(XMLN * p_node, SetRecordingConfiguration_REQ * p_req);
ONVIF_RET parse_CreateTrack(XMLN * p_node, CreateTrack_REQ * p_req);
ONVIF_RET parse_DeleteTrack(XMLN * p_node, DeleteTrack_REQ * p_req);
ONVIF_RET parse_GetTrackConfiguration(XMLN * p_node, GetTrackConfiguration_REQ * p_req);
ONVIF_RET parse_SetTrackConfiguration(XMLN * p_node, SetTrackConfiguration_REQ * p_req);
ONVIF_RET parse_CreateRecordingJob(XMLN * p_node, CreateRecordingJob_REQ * p_req);
ONVIF_RET parse_SetRecordingJobConfiguration(XMLN * p_node, SetRecordingJobConfiguration_REQ * p_req);
ONVIF_RET parse_SetRecordingJobMode(XMLN * p_node, SetRecordingJobMode_REQ * p_req);
ONVIF_RET parse_GetRecordingInformation(XMLN * p_node, GetRecordingInformation_REQ * p_req);
ONVIF_RET parse_GetMediaAttributes(XMLN * p_node, GetMediaAttributes_REQ * p_req);
ONVIF_RET parse_FindRecordings(XMLN * p_node, FindRecordings_REQ * p_req);
ONVIF_RET parse_GetRecordingSearchResults(XMLN * p_node, GetRecordingSearchResults_REQ * p_req);
ONVIF_RET parse_FindEvents(XMLN * p_node, FindEvents_REQ * p_req);
ONVIF_RET parse_GetEventSearchResults(XMLN * p_node, GetEventSearchResults_REQ * p_req);
ONVIF_RET parse_FindMetadata(XMLN * p_node, FindMetadata_REQ * p_req);
ONVIF_RET parse_GetMetadataSearchResults(XMLN * p_node, GetMetadataSearchResults_REQ * p_req);
ONVIF_RET parse_FindPTZPosition(XMLN * p_node, FindPTZPosition_REQ * p_req);
ONVIF_RET parse_GetPTZPositionSearchResults(XMLN * p_node, GetPTZPositionSearchResults_REQ * p_req);
ONVIF_RET parse_EndSearch(XMLN * p_node, EndSearch_REQ * p_req);
ONVIF_RET parse_GetSearchState(XMLN * p_node, GetSearchState_REQ * p_req);
ONVIF_RET parse_GetReplayUri(XMLN * p_node, GetReplayUri_REQ * p_req);
ONVIF_RET parse_SetReplayConfiguration(XMLN * p_node, SetReplayConfiguration_REQ * p_req);

#endif	// end of PROFILE_G_SUPPORT

#ifdef VIDEO_ANALYTICS

ONVIF_RET parse_GetSupportedRules(XMLN * p_node, GetSupportedRules_REQ * p_req);
ONVIF_RET parse_CreateRules(XMLN * p_node, CreateRules_REQ * p_req);
ONVIF_RET parse_DeleteRules(XMLN * p_node, DeleteRules_REQ * p_req);
ONVIF_RET parse_GetRules(XMLN * p_node, GetRules_REQ * p_req);
ONVIF_RET parse_ModifyRules(XMLN * p_node, ModifyRules_REQ * p_req);
ONVIF_RET parse_CreateAnalyticsModules(XMLN * p_node, CreateAnalyticsModules_REQ * p_req);
ONVIF_RET parse_DeleteAnalyticsModules(XMLN * p_node, DeleteAnalyticsModules_REQ * p_req);
ONVIF_RET parse_GetAnalyticsModules(XMLN * p_node, GetAnalyticsModules_REQ * p_req);
ONVIF_RET parse_ModifyAnalyticsModules(XMLN * p_node, ModifyAnalyticsModules_REQ * p_req);

ONVIF_RET parse_AddVideoAnalyticsConfiguration(XMLN * p_node, AddVideoAnalyticsConfiguration_REQ * p_req);
ONVIF_RET parse_GetVideoAnalyticsConfiguration(XMLN * p_node, GetVideoAnalyticsConfiguration_REQ * p_req);
ONVIF_RET parse_RemoveVideoAnalyticsConfiguration(XMLN * p_node, RemoveVideoAnalyticsConfiguration_REQ * p_req);
ONVIF_RET parse_SetVideoAnalyticsConfiguration(XMLN * p_node, SetVideoAnalyticsConfiguration_REQ * p_req);
ONVIF_RET parse_GetRuleOptions(XMLN * p_node, GetRuleOptions_REQ * p_req);
ONVIF_RET parse_GetSupportedAnalyticsModules(XMLN * p_node, GetSupportedAnalyticsModules_REQ * p_req);
ONVIF_RET parse_GetAnalyticsModuleOptions(XMLN * p_node, GetAnalyticsModuleOptions_REQ * p_req);

#endif	// end of VIDEO_ANALYTICS

#ifdef PROFILE_C_SUPPORT

ONVIF_RET parse_tac_GetAccessPointInfoList(XMLN * p_node, tac_GetAccessPointInfoList_REQ * p_req);
ONVIF_RET parse_tac_GetAccessPointInfo(XMLN * p_node, tac_GetAccessPointInfo_REQ * p_req);
ONVIF_RET parse_tac_GetAreaInfoList(XMLN * p_node, tac_GetAreaInfoList_REQ * p_req);
ONVIF_RET parse_tac_GetAreaInfo(XMLN * p_node, tac_GetAreaInfo_REQ * p_req);
ONVIF_RET parse_tac_GetAccessPointState(XMLN * p_node, tac_GetAccessPointState_REQ * p_req);
ONVIF_RET parse_tac_EnableAccessPoint(XMLN * p_node, tac_EnableAccessPoint_REQ * p_req);
ONVIF_RET parse_tac_DisableAccessPoint(XMLN * p_node, tac_DisableAccessPoint_REQ * p_req);

ONVIF_RET parse_tdc_GetDoorInfoList(XMLN * p_node, tdc_GetDoorInfoList_REQ * p_req);
ONVIF_RET parse_tdc_GetDoorInfo(XMLN * p_node, tdc_GetDoorInfo_REQ * p_req);
ONVIF_RET parse_tdc_GetDoorState(XMLN * p_node, tdc_GetDoorState_REQ * p_req);
ONVIF_RET parse_tdc_AccessDoor(XMLN * p_node, tdc_AccessDoor_REQ * p_req);
ONVIF_RET parse_tdc_LockDoor(XMLN * p_node, tdc_LockDoor_REQ * p_req);
ONVIF_RET parse_tdc_UnlockDoor(XMLN * p_node, tdc_UnlockDoor_REQ * p_req);
ONVIF_RET parse_tdc_DoubleLockDoor(XMLN * p_node, tdc_DoubleLockDoor_REQ * p_req);
ONVIF_RET parse_tdc_BlockDoor(XMLN * p_node, tdc_BlockDoor_REQ * p_req);
ONVIF_RET parse_tdc_LockDownDoor(XMLN * p_node, tdc_LockDownDoor_REQ * p_req);
ONVIF_RET parse_tdc_LockDownReleaseDoor(XMLN * p_node, tdc_LockDownReleaseDoor_REQ * p_req);
ONVIF_RET parse_tdc_LockOpenDoor(XMLN * p_node, tdc_LockOpenDoor_REQ * p_req);
ONVIF_RET parse_tdc_LockOpenReleaseDoor(XMLN * p_node, tdc_LockOpenReleaseDoor_REQ * p_req);

#endif  // end of PROFILE_C_SUPPORT

#ifdef DEVICEIO_SUPPORT

ONVIF_RET parse_GetVideoOutputConfiguration(XMLN * p_node, GetVideoOutputConfiguration_REQ * p_req);
ONVIF_RET parse_SetVideoOutputConfiguration(XMLN * p_node, SetVideoOutputConfiguration_REQ * p_req);
ONVIF_RET parse_GetVideoOutputConfigurationOptions(XMLN * p_node, GetVideoOutputConfigurationOptions_REQ * p_req);
ONVIF_RET parse_trt_GetAudioOutputConfiguration(XMLN * p_node, trt_GetAudioOutputConfiguration_REQ * p_req);
ONVIF_RET parse_GetAudioOutputConfiguration(XMLN * p_node, GetAudioOutputConfiguration_REQ * p_req);
ONVIF_RET parse_SetAudioOutputConfiguration(XMLN * p_node, SetAudioOutputConfiguration_REQ * p_req);
ONVIF_RET parse_GetAudioOutputConfigurationOptions(XMLN * p_node, GetAudioOutputConfigurationOptions_REQ * p_req);
ONVIF_RET parse_trt_GetAudioOutputConfigurationOptions(XMLN * p_node, trt_GetAudioOutputConfigurationOptions_REQ * p_req);
ONVIF_RET parse_GetRelayOutputOptions(XMLN * p_node, GetRelayOutputOptions_REQ * p_req);
ONVIF_RET parse_SetRelayOutputSettings(XMLN * p_node, SetRelayOutputSettings_REQ * p_req);
ONVIF_RET parse_SetRelayOutputState(XMLN * p_node, SetRelayOutputState_REQ * p_req);
ONVIF_RET parse_GetDigitalInputConfigurationOptions(XMLN * p_node, GetDigitalInputConfigurationOptions_REQ * p_req);
ONVIF_RET parse_SetDigitalInputConfigurations(XMLN * p_node, SetDigitalInputConfigurations_REQ * p_req);
ONVIF_RET parse_GetSerialPortConfiguration(XMLN * p_node, GetSerialPortConfiguration_REQ * p_req);
ONVIF_RET parse_GetSerialPortConfigurationOptions(XMLN * p_node, GetSerialPortConfigurationOptions_REQ * p_req);
ONVIF_RET parse_SetSerialPortConfiguration(XMLN * p_node, SetSerialPortConfiguration_REQ * p_req);
ONVIF_RET parse_SendReceiveSerialCommand(XMLN * p_node, SendReceiveSerialCommand_REQ * p_req);
ONVIF_RET parse_GetCompatibleAudioOutputConfigurations(XMLN * p_node, GetCompatibleAudioOutputConfigurations_REQ * p_req);
ONVIF_RET parse_AddAudioOutputConfiguration(XMLN * p_node, AddAudioOutputConfiguration_REQ * p_req);
ONVIF_RET parse_RemoveAudioOutputConfiguration(XMLN * p_node, RemoveAudioOutputConfiguration_REQ * p_req);
ONVIF_RET parse_trt_SetRelayOutputSettings(XMLN * p_node, SetRelayOutputSettings_REQ * p_req);

#endif // end of DEVICEIO_SUPPORT

#ifdef MEDIA2_SUPPORT

ONVIF_RET parse_tr2_GetConfiguration(XMLN * p_node, tr2_GetConfiguration * p_req);
ONVIF_RET parse_tr2_SetVideoEncoderConfiguration(XMLN * p_node, tr2_SetVideoEncoderConfiguration_REQ * p_req);
ONVIF_RET parse_tr2_CreateProfile(XMLN * p_node, tr2_CreateProfile_REQ * p_req);
ONVIF_RET parse_tr2_GetProfiles(XMLN * p_node, tr2_GetProfiles_REQ * p_req);
ONVIF_RET parse_tr2_DeleteProfile(XMLN * p_node, tr2_DeleteProfile_REQ * p_req);
ONVIF_RET parse_tr2_AddConfiguration(XMLN * p_node, tr2_AddConfiguration_REQ * p_req);
ONVIF_RET parse_tr2_RemoveConfiguration(XMLN * p_node, tr2_RemoveConfiguration_REQ * p_req);
ONVIF_RET parse_tr2_SetVideoSourceConfiguration(XMLN * p_node, tr2_SetVideoSourceConfiguration_REQ * p_req);
ONVIF_RET parse_tr2_SetAudioEncoderConfiguration(XMLN * p_node, tr2_SetAudioEncoderConfiguration_REQ * p_req);
ONVIF_RET parse_tr2_SetMetadataConfiguration(XMLN * p_node, tr2_SetMetadataConfiguration_REQ * p_req);
ONVIF_RET parse_tr2_SetAudioSourceConfiguration(XMLN * p_node, tr2_SetAudioSourceConfiguration_REQ * p_req);
ONVIF_RET parse_tr2_GetVideoEncoderInstances(XMLN * p_node, tr2_GetVideoEncoderInstances_REQ * p_req);
ONVIF_RET parse_tr2_GetStreamUri(XMLN * p_node, tr2_GetStreamUri_REQ * p_req);
ONVIF_RET parse_tr2_SetSynchronizationPoint(XMLN * p_node, tr2_SetSynchronizationPoint_REQ * p_req);
ONVIF_RET parse_tr2_GetVideoSourceModes(XMLN * p_node, tr2_GetVideoSourceModes_REQ * p_req);
ONVIF_RET parse_tr2_SetVideoSourceMode(XMLN * p_node, tr2_SetVideoSourceMode_REQ * p_req);
ONVIF_RET parse_tr2_GetSnapshotUri(XMLN * p_node, tr2_GetSnapshotUri_REQ * p_req);
ONVIF_RET parse_tr2_GetOSDs(XMLN * p_node, tr2_GetOSDs_REQ * p_req);
ONVIF_RET parse_tr2_GetAudioOutputConfigurations(XMLN * p_node, tr2_GetAudioOutputConfigurations_REQ * p_req);
ONVIF_RET parse_tr2_SetAudioOutputConfiguration(XMLN * p_node, tr2_SetAudioOutputConfiguration_REQ * p_req);
ONVIF_RET parse_tr2_SetAudioDecoderConfiguration(XMLN * p_node, tr2_SetAudioDecoderConfiguration_REQ * p_req);
ONVIF_RET parse_tr2_CreateMask(XMLN * p_node, tr2_CreateMask_REQ * p_req);
ONVIF_RET parse_tr2_DeleteMask(XMLN * p_node, tr2_DeleteMask_REQ * p_req);
ONVIF_RET parse_tr2_GetMasks(XMLN * p_node, tr2_GetMasks_REQ * p_req);
ONVIF_RET parse_tr2_SetMask(XMLN * p_node, tr2_SetMask_REQ * p_req);

#endif // end of MEDIA2_SUPPORT

#ifdef THERMAL_SUPPORT

ONVIF_RET parse_tth_GetConfiguration(XMLN * p_node, tth_GetConfiguration_REQ * p_req);
ONVIF_RET parse_tth_SetConfiguration(XMLN * p_node, tth_SetConfiguration_REQ * p_req);
ONVIF_RET parse_tth_GetConfigurationOptions(XMLN * p_node, tth_GetConfigurationOptions_REQ * p_req);
ONVIF_RET parse_tth_GetRadiometryConfiguration(XMLN * p_node, tth_GetRadiometryConfiguration_REQ * p_req);
ONVIF_RET parse_tth_SetRadiometryConfiguration(XMLN * p_node, tth_SetRadiometryConfiguration_REQ * p_req);
ONVIF_RET parse_tth_GetRadiometryConfigurationOptions(XMLN * p_node, tth_GetRadiometryConfigurationOptions_REQ * p_req);

#endif // end of THERMAL_SUPPORT

#ifdef CREDENTIAL_SUPPORT

ONVIF_RET parse_tcr_GetCredentialInfo(XMLN * p_node, tcr_GetCredentialInfo_REQ * p_req);
ONVIF_RET parse_tcr_GetCredentialInfoList(XMLN * p_node, tcr_GetCredentialInfoList_REQ * p_req);
ONVIF_RET parse_tcr_GetCredentials(XMLN * p_node, tcr_GetCredentials_REQ * p_req);
ONVIF_RET parse_tcr_GetCredentialList(XMLN * p_node, tcr_GetCredentialList_REQ * p_req);
ONVIF_RET parse_tcr_CreateCredential(XMLN * p_node, tcr_CreateCredential_REQ * p_req);
ONVIF_RET parse_tcr_ModifyCredential(XMLN * p_node, tcr_ModifyCredential_REQ * p_req);
ONVIF_RET parse_tcr_DeleteCredential(XMLN * p_node, tcr_DeleteCredential_REQ * p_req);
ONVIF_RET parse_tcr_GetCredentialState(XMLN * p_node, tcr_GetCredentialState_REQ * p_req);
ONVIF_RET parse_tcr_EnableCredential(XMLN * p_node, tcr_EnableCredential_REQ * p_req);
ONVIF_RET parse_tcr_DisableCredential(XMLN * p_node, tcr_DisableCredential_REQ * p_req);
ONVIF_RET parse_tcr_ResetAntipassbackViolation(XMLN * p_node, tcr_ResetAntipassbackViolation_REQ * p_req);
ONVIF_RET parse_tcr_GetSupportedFormatTypes(XMLN * p_node, tcr_GetSupportedFormatTypes_REQ * p_req);
ONVIF_RET parse_tcr_GetCredentialIdentifiers(XMLN * p_node, tcr_GetCredentialIdentifiers_REQ * p_req);
ONVIF_RET parse_tcr_SetCredentialIdentifier(XMLN * p_node, tcr_SetCredentialIdentifier_REQ * p_req);
ONVIF_RET parse_tcr_DeleteCredentialIdentifier(XMLN * p_node, tcr_DeleteCredentialIdentifier_REQ * p_req);
ONVIF_RET parse_tcr_GetCredentialAccessProfiles(XMLN * p_node, tcr_GetCredentialAccessProfiles_REQ * p_req);
ONVIF_RET parse_tcr_SetCredentialAccessProfiles(XMLN * p_node, tcr_SetCredentialAccessProfiles_REQ * p_req);
ONVIF_RET parse_tcr_DeleteCredentialAccessProfiles(XMLN * p_node, tcr_DeleteCredentialAccessProfiles_REQ * p_req);

#endif // end of CREDENTIAL_SUPPORT

#ifdef ACCESS_RULES

ONVIF_RET parse_tar_GetAccessProfileInfo(XMLN * p_node, tar_GetAccessProfileInfo_REQ * p_req);
ONVIF_RET parse_tar_GetAccessProfileInfoList(XMLN * p_node, tar_GetAccessProfileInfoList_REQ * p_req);
ONVIF_RET parse_tar_GetAccessProfiles(XMLN * p_node, tar_GetAccessProfiles_REQ * p_req);
ONVIF_RET parse_tar_GetAccessProfileList(XMLN * p_node, tar_GetAccessProfileList_REQ * p_req);
ONVIF_RET parse_tar_CreateAccessProfile(XMLN * p_node, tar_CreateAccessProfile_REQ * p_req);
ONVIF_RET parse_tar_ModifyAccessProfile(XMLN * p_node, tar_ModifyAccessProfile_REQ * p_req);
ONVIF_RET parse_tar_DeleteAccessProfile(XMLN * p_node, tar_DeleteAccessProfile_REQ * p_req);

#endif // end of ACCESS_RULES

#ifdef SCHEDULE_SUPPORT

ONVIF_RET parse_tsc_GetScheduleInfo(XMLN * p_node, tsc_GetScheduleInfo_REQ * p_req);
ONVIF_RET parse_tsc_GetScheduleInfoList(XMLN * p_node, tsc_GetScheduleInfoList_REQ * p_req);
ONVIF_RET parse_tsc_GetSchedules(XMLN * p_node, tsc_GetSchedules_REQ * p_req);
ONVIF_RET parse_tsc_GetScheduleList(XMLN * p_node, tsc_GetScheduleList_REQ * p_req);
ONVIF_RET parse_tsc_CreateSchedule(XMLN * p_node, tsc_CreateSchedule_REQ * p_req);
ONVIF_RET parse_tsc_ModifySchedule(XMLN * p_node, tsc_ModifySchedule_REQ * p_req);
ONVIF_RET parse_tsc_DeleteSchedule(XMLN * p_node, tsc_DeleteSchedule_REQ * p_req);
ONVIF_RET parse_tsc_GetSpecialDayGroupInfo(XMLN * p_node, tsc_GetSpecialDayGroupInfo_REQ * p_req);
ONVIF_RET parse_tsc_GetSpecialDayGroupInfoList(XMLN * p_node, tsc_GetSpecialDayGroupInfoList_REQ * p_req);
ONVIF_RET parse_tsc_GetSpecialDayGroups(XMLN * p_node, tsc_GetSpecialDayGroups_REQ * p_req);
ONVIF_RET parse_tsc_GetSpecialDayGroupList(XMLN * p_node, tsc_GetSpecialDayGroupList_REQ * p_req);
ONVIF_RET parse_tsc_CreateSpecialDayGroup(XMLN * p_node, tsc_CreateSpecialDayGroup_REQ * p_req);
ONVIF_RET parse_tsc_ModifySpecialDayGroup(XMLN * p_node, tsc_ModifySpecialDayGroup_REQ * p_req);
ONVIF_RET parse_tsc_DeleteSpecialDayGroup(XMLN * p_node, tsc_DeleteSpecialDayGroup_REQ * p_req);
ONVIF_RET parse_tsc_GetScheduleState(XMLN * p_node, tsc_GetScheduleState_REQ * p_req);

#endif // end of SCHEDULE_SUPPORT

#ifdef RECEIVER_SUPPORT

ONVIF_RET parse_trv_GetReceiver(XMLN * p_node, trv_GetReceiver_REQ * p_req);
ONVIF_RET parse_trv_CreateReceiver(XMLN * p_node, trv_CreateReceiver_REQ * p_req);
ONVIF_RET parse_trv_DeleteReceiver(XMLN * p_node, trv_DeleteReceiver_REQ * p_req);
ONVIF_RET parse_trv_ConfigureReceiver(XMLN * p_node, trv_ConfigureReceiver_REQ * p_req);
ONVIF_RET parse_trv_SetReceiverMode(XMLN * p_node, trv_SetReceiverMode_REQ * p_req);
ONVIF_RET parse_trv_GetReceiverState(XMLN * p_node, trv_GetReceiverState_REQ * p_req);

#endif // end of RECEIVER_SUPPORT


#ifdef __cplusplus
}
#endif

#endif


