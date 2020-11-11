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

#ifndef __ONVIF_PKT_H__
#define __ONVIF_PKT_H__


#ifdef __cplusplus
extern "C" {
#endif

int build_err_rly_xml(char * p_buf, int mlen, const char * code, const char * subcode, const char * subcode_ex, const char * reason, const char * action);

int build_GetDeviceInformation_rly_xml(char * p_buf, int mlen, const char * argv);
int build_GetSystemUris_rly_xml(char * p_buf, int mlen, const char * argv);
int build_GetProfiles_rly_xml(char * p_buf, int mlen, const char * argv);
int build_GetProfile_rly_xml(char * p_buf, int mlen, const char * argv);
int build_CreateProfile_rly_xml(char * p_buf, int mlen, const char * argv);
int build_DeleteProfile_rly_xml(char * p_buf, int mlen, const char * argv);
int build_AddVideoSourceConfiguration_rly_xml(char * p_buf, int mlen, const char * argv);
int build_RemoveVideoSourceConfiguration_rly_xml(char * p_buf, int mlen, const char * argv);
int build_AddVideoEncoderConfiguration_rly_xml(char * p_buf, int mlen, const char * argv);
int build_RemoveVideoEncoderConfiguration_rly_xml(char * p_buf, int mlen, const char * argv);
int build_GetStreamUri_rly_xml(char * p_buf, int mlen, const char * argv);
int build_GetCapabilities_rly_xml(char * p_buf, int mlen, const char * argv);
int build_GetNetworkInterfaces_rly_xml(char * p_buf, int mlen, const char * argv);
int build_SetNetworkInterfaces_rly_xml(char * p_buf, int mlen, const char * argv);
int build_GetVideoEncoderConfigurations_rly_xml(char * p_buf, int mlen, const char * argv);
int build_GetCompatibleVideoEncoderConfigurations_rly_xml(char * p_buf, int mlen, const char * argv);
int build_GetVideoSourceConfigurations_rly_xml(char * p_buf, int mlen, const char * argv);
int build_SetVideoSourceConfiguration_rly_xml(char * p_buf, int mlen, const char * argv);
int build_GetVideoSourceConfigurationOptions_rly_xml(char * p_buf, int mlen, const char * argv);
int build_GetVideoEncoderConfigurationOptions_rly_xml(char * p_buf, int mlen, const char * argv);
int build_SetSynchronizationPoint_rly_xml(char * p_buf, int mlen, const char * argv);
int build_SystemReboot_rly_xml(char * p_buf, int mlen, const char * argv);
int build_SetSystemFactoryDefault_rly_xml(char * p_buf, int mlen, const char * argv);
int build_GetSystemLog_rly_xml(char * p_buf, int mlen, const char * argv);
int build_SetVideoEncoderConfiguration_rly_xml(char * p_buf, int mlen, const char * argv);
int build_GetVideoSources_rly_xml(char * p_buf, int mlen, const char * argv);
int build_GetVideoEncoderConfiguration_rly_xml(char * p_buf, int mlen, const char * token);
int build_GetSystemDateAndTime_rly_xml(char * p_buf, int mlen, const char * argv);
int build_SetSystemDateAndTime_rly_xml(char * p_buf, int mlen, const char * argv);
int build_GetServices_rly_xml(char * p_buf, int mlen, const char * argv);
int build_GetSnapshotUri_rly_xml(char * p_buf, int mlen, const char * profile_token);
int build_GetVideoSourceConfiguration_rly_xml(char * p_buf, int mlen, const char * token);
int build_GetCompatibleVideoSourceConfigurations_rly_xml(char * p_buf, int mlen, const char * token);
int build_GetScopes_rly_xml(char * p_buf, int mlen, const char * argv);
int build_AddScopes_rly_xml(char * p_buf, int mlen, const char * argv);
int build_SetScopes_rly_xml(char * p_buf, int mlen, const char * argv);
int build_RemoveScopes_rly_xml(char * p_buf, int mlen, const char * argv);
int build_GetHostname_rly_xml(char * p_buf, int mlen, const char * argv);
int build_SetHostname_rly_xml(char * p_buf, int mlen, const char * argv);
int build_SetHostnameFromDHCP_rly_xml(char * p_buf, int mlen, const char * argv);
int build_GetNetworkProtocols_rly_xml(char * p_buf, int mlen, const char * argv);
int build_SetNetworkProtocols_rly_xml(char * p_buf, int mlen, const char * argv);
int build_GetNetworkDefaultGateway_rly_xml(char * p_buf, int mlen, const char * argv);
int build_SetNetworkDefaultGateway_rly_xml(char * p_buf, int mlen, const char * argv);
int build_GetDiscoveryMode_rly_xml(char * p_buf, int mlen, const char * argv);
int build_SetDiscoveryMode_rly_xml(char * p_buf, int mlen, const char * argv);
int build_GetDNS_rly_xml(char * p_buf, int mlen, const char * argv);
int build_SetDNS_rly_xml(char * p_buf, int mlen, const char * argv);
int build_GetNTP_rly_xml(char * p_buf, int mlen, const char * argv);
int build_SetNTP_rly_xml(char * p_buf, int mlen, const char * argv);
int build_GetZeroConfiguration_rly_xml(char * p_buf, int mlen, const char * argv);
int build_SetZeroConfiguration_rly_xml(char * p_buf, int mlen, const char * argv);
int build_GetDot11Capabilities_rly_xml(char * p_buf, int mlen, const char * argv);
int build_GetDot11Status_rly_xml(char * p_buf, int mlen, const char * argv);
int build_ScanAvailableDot11Networks_rly_xml(char * p_buf, int mlen, const char * argv);
int build_GetServiceCapabilities_rly_xml(char * p_buf, int mlen, const char * argv);
int build_GetEventProperties_rly_xml(char * p_buf, int mlen, const char * argv);
int build_Subscribe_rly_xml(char * p_buf, int mlen, const char * argv);
int build_Unsubscribe_rly_xml(char * p_buf, int mlen, const char * argv);
int build_Renew_rly_xml(char * p_buf, int mlen, const char * argv);
int build_CreatePullPointSubscription_rly_xml(char * p_buf, int mlen, const char * argv);
int build_PullMessages_rly_xml(char * p_buf, int mlen, const char * argv);
int build_tev_SetSynchronizationPoint_rly_xml(char * p_buf, int mlen, const char * argv);
int build_Notify_xml(char * p_buf, int mlen, const char * argv);
int build_GetWsdlUrl_rly_xml(char * p_buf, int mlen, const char * argv);
int build_GetEndpointReference_rly_xml(char * p_buf, int mlen, const char * argv);
int build_GetGuaranteedNumberOfVideoEncoderInstances_rly_xml(char * p_buf, int mlen, const char * argv);
int build_GetImagingSettings_rly_xml(char * p_buf, int mlen, const char * argv);
int build_GetOptions_rly_xml(char * p_buf, int mlen, const char * argv);
int build_SetImagingSettings_rly_xml(char * p_buf, int mlen, const char * argv);
int build_GetMoveOptions_rly_xml(char * p_buf, int mlen, const char * argv);
int build_Move_rly_xml(char * p_buf, int mlen, const char * argv);
int build_img_GetStatus_rly_xml(char * p_buf, int mlen, const char * argv);
int build_img_Stop_rly_xml(char * p_buf, int mlen, const char * argv);
int build_GetUsers_rly_xml(char * p_buf, int mlen, const char * argv);
int build_CreateUsers_rly_xml(char * p_buf, int mlen, const char * argv);
int build_DeleteUsers_rly_xml(char * p_buf, int mlen, const char * argv);
int build_SetUser_rly_xml(char * p_buf, int mlen, const char * argv);
int build_GetRemoteUser_rly_xml(char * p_buf, int mlen, const char * argv);
int build_SetRemoteUser_rly_xml(char * p_buf, int mlen, const char * argv);
int build_StartFirmwareUpgrade_rly_xml(char * p_buf, int mlen, const char * argv);
int build_StartSystemRestore_rly_xml(char * p_buf, int mlen, const char * argv);
int build_GetOSDs_rly_xml(char * p_buf, int mlen, const char * argv);
int build_GetOSD_rly_xml(char * p_buf, int mlen, const char * argv);
int build_SetOSD_rly_xml(char * p_buf, int mlen, const char * argv);
int build_CreateOSD_rly_xml(char * p_buf, int mlen, const char * argv);
int build_DeleteOSD_rly_xml(char * p_buf, int mlen, const char * argv);
int build_GetOSDOptions_rly_xml(char * p_buf, int mlen, const char * argv);
int build_SetConfiguration_rly_xml(char * p_buf, int mlen, const char * argv);
int build_StartMulticastStreaming_rly_xml(char * p_buf, int mlen, const char * argv);
int build_StopMulticastStreaming_rly_xml(char * p_buf, int mlen, const char * argv);
int build_GetMetadataConfigurations_rly_xml(char * p_buf, int mlen, const char * argv);
int build_GetMetadataConfiguration_rly_xml(char * p_buf, int mlen, const char * argv);
int build_GetCompatibleMetadataConfigurations_rly_xml(char * p_buf, int mlen, const char * argv);
int build_GetMetadataConfigurationOptions_rly_xml(char * p_buf, int mlen, const char * argv);
int build_SetMetadataConfiguration_rly_xml(char * p_buf, int mlen, const char * argv);
int build_AddMetadataConfiguration_rly_xml(char * p_buf, int mlen, const char * argv);
int build_RemoveMetadataConfiguration_rly_xml(char * p_buf, int mlen, const char * argv);
int build_GetVideoSourceModes_rly_xml(char * p_buf, int mlen, const char * argv);
int build_SetVideoSourceMode_rly_xml(char * p_buf, int mlen, const char * argv);

#ifdef IPFILTER_SUPPORT	

int build_GetIPAddressFilter_rly_xml(char * p_buf, int mlen, const char * argv);
int build_SetIPAddressFilter_rly_xml(char * p_buf, int mlen, const char * argv);
int build_AddIPAddressFilter_rly_xml(char * p_buf, int mlen, const char * argv);
int build_RemoveIPAddressFilter_rly_xml(char * p_buf, int mlen, const char * argv);

#endif // end of IPFILTER_SUPPORT

#ifdef AUDIO_SUPPORT

int build_AddAudioSourceConfiguration_rly_xml(char * p_buf, int mlen, const char * argv);
int build_RemoveAudioSourceConfiguration_rly_xml(char * p_buf, int mlen, const char * argv);
int build_AddAudioEncoderConfiguration_rly_xml(char * p_buf, int mlen, const char * argv);
int build_RemoveAudioEncoderConfiguration_rly_xml(char * p_buf, int mlen, const char * argv);
int build_GetAudioSources_rly_xml(char * p_buf, int mlen, const char * argv);
int build_GetAudioEncoderConfigurations_rly_xml(char * p_buf, int mlen, const char * argv);
int build_GetAudioSourceConfigurations_rly_xml(char * p_buf, int mlen, const char * argv);
int build_GetAudioEncoderConfiguration_rly_xml(char * p_buf, int mlen, const char * token);
int build_GetAudioEncoderConfigurationOptions_rly_xml(char * p_buf, int mlen, const char * argv);
int build_GetAudioSourceConfiguration_rly_xml(char * p_buf, int mlen, const char * token);
int build_GetCompatibleAudioSourceConfigurations_rly_xml(char * p_buf, int mlen, const char * argv);
int build_GetAudioSourceConfigurationOptions_rly_xml(char * p_buf, int mlen, const char * argv);
int build_SetAudioSourceConfiguration_rly_xml(char * p_buf, int mlen, const char * argv);
int build_GetCompatibleAudioEncoderConfigurations_rly_xml(char * p_buf, int mlen, const char * argv);
int build_SetAudioEncoderConfiguration_rly_xml(char * p_buf, int mlen, const char * argv);
int build_AddAudioDecoderConfiguration_rly_xml(char * p_buf, int mlen, const char * argv);
int build_GetAudioDecoderConfigurations_rly_xml(char * p_buf, int mlen, const char * argv);
int build_GetAudioDecoderConfiguration_rly_xml(char * p_buf, int mlen, const char * argv);
int build_RemoveAudioDecoderConfiguration_rly_xml(char * p_buf, int mlen, const char * argv);
int build_SetAudioDecoderConfiguration_rly_xml(char * p_buf, int mlen, const char * argv);
int build_GetAudioDecoderConfigurationOptions_rly_xml(char * p_buf, int mlen, const char * argv);
int build_GetCompatibleAudioDecoderConfigurations_rly_xml(char * p_buf, int mlen, const char * argv);

#endif // end of AUDIO_SUPPORT

#ifdef PTZ_SUPPORT

int build_GetNodes_rly_xml(char * p_buf, int mlen, const char * argv);
int build_GetNode_rly_xml(char * p_buf, int mlen, const char * argv);
int build_GetConfigurations_rly_xml(char * p_buf, int mlen, const char * argv);
int build_GetCompatibleConfigurations_rly_xml(char * p_buf, int mlen, const char * argv);
int build_GetConfiguration_rly_xml(char * p_buf, int mlen, const char * argv);
int build_AddPTZConfiguration_rly_xml(char * p_buf, int mlen, const char * argv);
int build_RemovePTZConfiguration_rly_xml(char * p_buf, int mlen, const char * argv);
int build_GetConfigurationOptions_rly_xml(char * p_buf, int mlen, const char * argv);
int build_ptz_GetStatus_rly_xml(char * p_buf, int mlen, const char * argv);
int build_ContinuousMove_rly_xml(char * p_buf, int mlen, const char * argv);
int build_ptz_Stop_rly_xml(char * p_buf, int mlen, const char * argv);
int build_AbsoluteMove_rly_xml(char * p_buf, int mlen, const char * argv);
int build_RelativeMove_rly_xml(char * p_buf, int mlen, const char * argv);
int build_SetPreset_rly_xml(char * p_buf, int mlen, const char * argv);
int build_GetPresets_rly_xml(char * p_buf, int mlen, const char * argv);
int build_RemovePreset_rly_xml(char * p_buf, int mlen, const char * argv);
int build_GotoPreset_rly_xml(char * p_buf, int mlen, const char * argv);
int build_GotoHomePosition_rly_xml(char * p_buf, int mlen, const char * argv);
int build_SetHomePosition_rly_xml(char * p_buf, int mlen, const char * argv);   

#endif // end of PTZ_SUPPORT


#ifdef PROFILE_G_SUPPORT

int build_GetRecordingSummary_rly_xml(char * p_buf, int mlen, const char * argv);
int build_GetRecordingInformation_rly_xml(char * p_buf, int mlen, const char * argv);
int build_GetMediaAttributes_rly_xml(char * p_buf, int mlen, const char * argv);
int build_FindRecordings_rly_xml(char * p_buf, int mlen, const char * argv);
int build_GetRecordingSearchResults_rly_xml(char * p_buf, int mlen, const char * argv);
int build_FindEvents_rly_xml(char * p_buf, int mlen, const char * argv);
int build_GetEventSearchResults_rly_xml(char * p_buf, int mlen, const char * argv);
int build_FindMetadata_rly_xml(char * p_buf, int mlen, const char * argv);
int build_GetMetadataSearchResults_rly_xml(char * p_buf, int mlen, const char * argv);
int build_FindPTZPosition_rly_xml(char * p_buf, int mlen, const char * argv);
int build_GetPTZPositionSearchResults_rly_xml(char * p_buf, int mlen, const char * argv);
int build_EndSearch_rly_xml(char * p_buf, int mlen, const char * argv);
int build_GetSearchState_rly_xml(char * p_buf, int mlen, const char * argv);
int build_CreateRecording_rly_xml(char * p_buf, int mlen, const char * argv);
int build_DeleteRecording_rly_xml(char * p_buf, int mlen, const char * argv);
int build_RecordingConfiguration_xml(char * p_buf, int mlen, onvif_RecordingConfiguration * p_req);
int build_GetRecordings_rly_xml(char * p_buf, int mlen, const char * argv);
int build_SetRecordingConfiguration_rly_xml(char * p_buf, int mlen, const char * argv);
int build_GetRecordingConfiguration_rly_xml(char * p_buf, int mlen, const char * argv);
int build_CreateTrack_rly_xml(char * p_buf, int mlen, const char * argv);
int build_DeleteTrack_rly_xml(char * p_buf, int mlen, const char * argv);
int build_TrackConfiguration_xml(char * p_buf, int mlen, onvif_TrackConfiguration * p_req);
int build_GetTrackConfiguration_rly_xml(char * p_buf, int mlen, const char * argv);
int build_SetTrackConfiguration_rly_xml(char * p_buf, int mlen, const char * argv);
int build_CreateRecordingJob_rly_xml(char * p_buf, int mlen, const char * argv);
int build_DeleteRecordingJob_rly_xml(char * p_buf, int mlen, const char * argv);
int build_RecordingJobConfiguration_xml(char * p_buf, int mlen, onvif_RecordingJobConfiguration * p_req);
int build_GetRecordingJobs_rly_xml(char * p_buf, int mlen, const char * argv);
int build_SetRecordingJobConfiguration_rly_xml(char * p_buf, int mlen, const char * argv);
int build_GetRecordingJobConfiguration_rly_xml(char * p_buf, int mlen, const char * argv);
int build_SetRecordingJobMode_rly_xml(char * p_buf, int mlen, const char * argv);
int build_RecordingJobStateInformation_xml(char * p_buf, int mlen, onvif_RecordingJobStateInformation * p_res);
int build_GetRecordingJobState_rly_xml(char * p_buf, int mlen, const char * argv);
int build_GetRecordingOptions_rly_xml(char * p_buf, int mlen, const char * argv);

int build_GetReplayUri_rly_xml(char * p_buf, int mlen, const char * argv);
int build_GetReplayConfiguration_rly_xml(char * p_buf, int mlen, const char * argv);
int build_SetReplayConfiguration_rly_xml(char * p_buf, int mlen, const char * argv);

#endif	// end of PROFILE_G_SUPPORT

#ifdef VIDEO_ANALYTICS

int build_GetVideoAnalyticsConfigurations_rly_xml(char * p_buf, int mlen, const char * argv);
int build_AddVideoAnalyticsConfiguration_rly_xml(char * p_buf, int mlen, const char * argv);
int build_GetVideoAnalyticsConfiguration_rly_xml(char * p_buf, int mlen, const char * argv);
int build_RemoveVideoAnalyticsConfiguration_rly_xml(char * p_buf, int mlen, const char * argv);
int build_SetVideoAnalyticsConfiguration_rly_xml(char * p_buf, int mlen, const char * argv);

int build_GetSupportedRules_rly_xml(char * p_buf, int mlen, const char * argv);
int build_CreateRules_rly_xml(char * p_buf, int mlen, const char * argv);
int build_DeleteRules_rly_xml(char * p_buf, int mlen, const char * argv);
int build_GetRules_rly_xml(char * p_buf, int mlen, const char * argv);
int build_ModifyRules_rly_xml(char * p_buf, int mlen, const char * argv);
int build_CreateAnalyticsModules_rly_xml(char * p_buf, int mlen, const char * argv);
int build_DeleteAnalyticsModules_rly_xml(char * p_buf, int mlen, const char * argv);
int build_GetAnalyticsModules_rly_xml(char * p_buf, int mlen, const char * argv);
int build_ModifyAnalyticsModules_rly_xml(char * p_buf, int mlen, const char * argv);
int build_GetAnalyticsConfigurations_rly_xml(char * p_buf, int mlen, const char * argv);
int build_GetRuleOptions_rly_xml(char * p_buf, int mlen, const char * argv);
int build_GetSupportedAnalyticsModules_rly_xml(char * p_buf, int mlen, const char * argv);
int build_GetAnalyticsModuleOptions_rly_xml(char * p_buf, int mlen, const char * argv);

#endif	// end of VIDEO_ANALYTICS

#ifdef PROFILE_C_SUPPORT

int build_tac_GetAccessPointInfoList_rly_xml(char * p_buf, int mlen, const char * argv);
int build_tac_GetAccessPointInfo_rly_xml(char * p_buf, int mlen, const char * argv);
int build_tac_GetAreaInfoList_rly_xml(char * p_buf, int mlen, const char * argv);
int build_tac_GetAreaInfo_rly_xml(char * p_buf, int mlen, const char * argv);
int build_tac_GetAccessPointState_rly_xml(char * p_buf, int mlen, const char * argv);
int build_tac_EnableAccessPoint_rly_xml(char * p_buf, int mlen, const char * argv);
int build_tac_DisableAccessPoint_rly_xml(char * p_buf, int mlen, const char * argv);

int build_tdc_GetDoorInfoList_rly_xml(char * p_buf, int mlen, const char * argv);
int build_tdc_GetDoorInfo_rly_xml(char * p_buf, int mlen, const char * argv);
int build_tdc_GetDoorState_rly_xml(char * p_buf, int mlen, const char * argv);
int build_tdc_AccessDoor_rly_xml(char * p_buf, int mlen, const char * argv);
int build_tdc_LockDoor_rly_xml(char * p_buf, int mlen, const char * argv);
int build_tdc_UnlockDoor_rly_xml(char * p_buf, int mlen, const char * argv);
int build_tdc_DoubleLockDoor_rly_xml(char * p_buf, int mlen, const char * argv);
int build_tdc_BlockDoor_rly_xml(char * p_buf, int mlen, const char * argv);
int build_tdc_LockDownDoor_rly_xml(char * p_buf, int mlen, const char * argv);
int build_tdc_LockDownReleaseDoor_rly_xml(char * p_buf, int mlen, const char * argv);
int build_tdc_LockOpenDoor_rly_xml(char * p_buf, int mlen, const char * argv);
int build_tdc_LockOpenReleaseDoor_rly_xml(char * p_buf, int mlen, const char * argv);

#endif // end of PROFILE_C_SUPPORT

#ifdef DEVICEIO_SUPPORT

int build_tmd_GetVideoSources_rly_xml(char * p_buf, int mlen, const char * argv);
int build_GetVideoOutputs_rly_xml(char * p_buf, int mlen, const char * argv);
int build_GetVideoOutputConfiguration_rly_xml(char * p_buf, int mlen, const char * argv);
int build_SetVideoOutputConfiguration_rly_xml(char * p_buf, int mlen, const char * argv);
int build_GetVideoOutputConfigurationOptions_rly_xml(char * p_buf, int mlen, const char * argv);
int build_trt_GetAudioOutputs_rly_xml(char * p_buf, int mlen, const char * argv);
int build_GetAudioOutputs_rly_xml(char * p_buf, int mlen, const char * argv);
int build_GetAudioOutputConfigurations_rly_xml(char * p_buf, int mlen, const char * argv);
int build_trt_GetAudioOutputConfiguration_rly_xml(char * p_buf, int mlen, const char * argv);
int build_GetAudioOutputConfiguration_rly_xml(char * p_buf, int mlen, const char * argv);
int build_SetAudioOutputConfiguration_rly_xml(char * p_buf, int mlen, const char * argv);
int build_GetAudioOutputConfigurationOptions_rly_xml(char * p_buf, int mlen, const char * argv);
int build_trt_GetAudioOutputConfigurationOptions_rly_xml(char * p_buf, int mlen, const char * argv);
int build_tmd_GetRelayOutputs_rly_xml(char * p_buf, int mlen, const char * argv);
int build_tds_GetRelayOutputs_rly_xml(char * p_buf, int mlen, const char * argv);
int build_GetRelayOutputOptions_rly_xml(char * p_buf, int mlen, const char * argv);
int build_tmd_SetRelayOutputSettings_rly_xml(char * p_buf, int mlen, const char * argv);
int build_tds_SetRelayOutputSettings_rly_xml(char * p_buf, int mlen, const char * argv);
int build_SetRelayOutputState_rly_xml(char * p_buf, int mlen, const char * argv);
int build_tmd_SetRelayOutputState_rly_xml(char * p_buf, int mlen, const char * argv);
int build_GetDigitalInputs_rly_xml(char * p_buf, int mlen, const char * argv);
int build_GetDigitalInputConfigurationOptions_rly_xml(char * p_buf, int mlen, const char * argv);
int build_SetDigitalInputConfigurations_rly_xml(char * p_buf, int mlen, const char * argv);
int build_GetSerialPorts_rly_xml(char * p_buf, int mlen, const char * argv);
int build_GetSerialPortConfiguration_rly_xml(char * p_buf, int mlen, const char * argv);
int build_GetSerialPortConfigurationOptions_rly_xml(char * p_buf, int mlen, const char * argv);
int build_SetSerialPortConfiguration_rly_xml(char * p_buf, int mlen, const char * argv);
int build_SendReceiveSerialCommand_rly_xml(char * p_buf, int mlen, const char * argv);
int build_GetCompatibleAudioOutputConfigurations_rly_xml(char * p_buf, int mlen, const char * argv);
int build_AddAudioOutputConfiguration_rly_xml(char * p_buf, int mlen, const char * argv);
int build_RemoveAudioOutputConfiguration_rly_xml(char * p_buf, int mlen, const char * argv);
int build_tmd_GetAudioSources_rly_xml(char * p_buf, int mlen, const char * argv);

#endif // end of DEVICEIO_SUPPORT

#ifdef MEDIA2_SUPPORT

int build_tr2_GetVideoEncoderConfigurations_rly_xml(char * p_buf, int mlen, const char * argv);
int build_tr2_GetVideoEncoderConfigurationOptions_rly_xml(char * p_buf, int mlen, const char * argv);
int build_tr2_SetVideoEncoderConfiguration_rly_xml(char * p_buf, int mlen, const char * argv);
int build_tr2_CreateProfile_rly_xml(char * p_buf, int mlen, const char * argv);
int build_tr2_GetProfiles_rly_xml(char * p_buf, int mlen, const char * argv);
int build_tr2_DeleteProfile_rly_xml(char * p_buf, int mlen, const char * argv);
int build_tr2_AddConfiguration_rly_xml(char * p_buf, int mlen, const char * argv);
int build_tr2_RemoveConfiguration_rly_xml(char * p_buf, int mlen, const char * argv);
int build_tr2_GetVideoSourceConfigurations_rly_xml(char * p_buf, int mlen, const char * argv);
int build_tr2_GetAudioEncoderConfigurations_rly_xml(char * p_buf, int mlen, const char * argv);
int build_tr2_GetAudioSourceConfigurations_rly_xml(char * p_buf, int mlen, const char * argv);
int build_tr2_GetMetadataConfigurations_rly_xml(char * p_buf, int mlen, const char * argv);
int build_tr2_SetVideoSourceConfiguration_rly_xml(char * p_buf, int mlen, const char * argv);
int build_tr2_SetAudioEncoderConfiguration_rly_xml(char * p_buf, int mlen, const char * argv);
int build_tr2_SetMetadataConfiguration_rly_xml(char * p_buf, int mlen, const char * argv);
int build_tr2_SetAudioSourceConfiguration_rly_xml(char * p_buf, int mlen, const char * argv);
int build_tr2_GetVideoSourceConfigurationOptions_rly_xml(char * p_buf, int mlen, const char * argv);
int build_tr2_GetAudioSourceConfigurationOptions_rly_xml(char * p_buf, int mlen, const char * argv);
int build_tr2_GetAudioEncoderConfigurationOptions_rly_xml(char * p_buf, int mlen, const char * argv);
int build_tr2_GetMetadataConfigurationOptions_rly_xml(char * p_buf, int mlen, const char * argv);
int build_tr2_GetVideoEncoderInstances_rly_xml(char * p_buf, int mlen, const char * argv);
int build_tr2_GetStreamUri_rly_xml(char * p_buf, int mlen, const char * argv);
int build_tr2_SetSynchronizationPoint_rly_xml(char * p_buf, int mlen, const char * argv);
int build_tr2_GetVideoSourceModes_rly_xml(char * p_buf, int mlen, const char * argv);
int build_tr2_SetVideoSourceMode_rly_xml(char * p_buf, int mlen, const char * argv);
int build_tr2_GetSnapshotUri_rly_xml(char * p_buf, int mlen, const char * argv);
int build_tr2_SetOSD_rly_xml(char * p_buf, int mlen, const char * argv);
int build_tr2_GetOSDOptions_rly_xml(char * p_buf, int mlen, const char * argv);
int build_tr2_GetOSDs_rly_xml(char * p_buf, int mlen, const char * argv);
int build_tr2_CreateOSD_rly_xml(char * p_buf, int mlen, const char * argv);
int build_tr2_DeleteOSD_rly_xml(char * p_buf, int mlen, const char * argv);
int build_tr2_GetAudioOutputConfigurationOptions_rly_xml(char * p_buf, int mlen, const char * argv);
int build_tr2_GetAudioOutputConfigurations_rly_xml(char * p_buf, int mlen, const char * argv);
int build_tr2_SetAudioOutputConfiguration_rly_xml(char * p_buf, int mlen, const char * argv);
int build_tr2_GetAudioDecoderConfigurations_rly_xml(char * p_buf, int mlen, const char * argv);
int build_tr2_SetAudioDecoderConfiguration_rly_xml(char * p_buf, int mlen, const char * argv);
int build_tr2_GetAudioDecoderConfigurationOptions_rly_xml(char * p_buf, int mlen, const char * argv);
int build_tr2_GetAnalyticsConfigurations_rly_xml(char * p_buf, int mlen, const char * argv);
int build_tr2_CreateMask_rly_xml(char * p_buf, int mlen, const char * argv);
int build_tr2_DeleteMask_rly_xml(char * p_buf, int mlen, const char * argv);
int build_tr2_GetMasks_rly_xml(char * p_buf, int mlen, const char * argv);
int build_tr2_SetMask_rly_xml(char * p_buf, int mlen, const char * argv);
int build_tr2_GetMaskOptions_rly_xml(char * p_buf, int mlen, const char * argv);

#endif // end of MEDIA2_SUPPORT

#ifdef THERMAL_SUPPORT

int build_tth_GetConfigurations_rly_xml(char * p_buf, int mlen, const char * argv);
int build_tth_GetConfiguration_rly_xml(char * p_buf, int mlen, const char * argv);
int build_tth_SetConfiguration_rly_xml(char * p_buf, int mlen, const char * argv);
int build_tth_GetConfigurationOptions_rly_xml(char * p_buf, int mlen, const char * argv);
int build_tth_GetRadiometryConfiguration_rly_xml(char * p_buf, int mlen, const char * argv);
int build_tth_SetRadiometryConfiguration_rly_xml(char * p_buf, int mlen, const char * argv);
int build_tth_GetRadiometryConfigurationOptions_rly_xml(char * p_buf, int mlen, const char * argv);

#endif // end of THERMAL_SUPPORT

#ifdef CREDENTIAL_SUPPORT

int build_tcr_GetCredentialInfo_rly_xml(char * p_buf, int mlen, const char * argv);
int build_tcr_GetCredentialInfoList_rly_xml(char * p_buf, int mlen, const char * argv);
int build_tcr_GetCredentials_rly_xml(char * p_buf, int mlen, const char * argv);
int build_tcr_GetCredentialList_rly_xml(char * p_buf, int mlen, const char * argv);
int build_tcr_CreateCredential_rly_xml(char * p_buf, int mlen, const char * argv);
int build_tcr_ModifyCredential_rly_xml(char * p_buf, int mlen, const char * argv);
int build_tcr_DeleteCredential_rly_xml(char * p_buf, int mlen, const char * argv);
int build_tcr_GetCredentialState_rly_xml(char * p_buf, int mlen, const char * argv);
int build_tcr_EnableCredential_rly_xml(char * p_buf, int mlen, const char * argv);
int build_tcr_DisableCredential_rly_xml(char * p_buf, int mlen, const char * argv);
int build_tcr_ResetAntipassbackViolation_rly_xml(char * p_buf, int mlen, const char * argv);
int build_tcr_GetSupportedFormatTypes_rly_xml(char * p_buf, int mlen, const char * argv);
int build_tcr_GetCredentialIdentifiers_rly_xml(char * p_buf, int mlen, const char * argv);
int build_tcr_SetCredentialIdentifier_rly_xml(char * p_buf, int mlen, const char * argv);
int build_tcr_DeleteCredentialIdentifier_rly_xml(char * p_buf, int mlen, const char * argv);
int build_tcr_GetCredentialAccessProfiles_rly_xml(char * p_buf, int mlen, const char * argv);
int build_tcr_SetCredentialAccessProfiles_rly_xml(char * p_buf, int mlen, const char * argv);
int build_tcr_DeleteCredentialAccessProfiles_rly_xml(char * p_buf, int mlen, const char * argv);

#endif // end of CREDENTIAL_SUPPORT

#ifdef ACCESS_RULES

int build_tar_GetAccessProfileInfo_rly_xml(char * p_buf, int mlen, const char * argv);
int build_tar_GetAccessProfileInfoList_rly_xml(char * p_buf, int mlen, const char * argv);
int build_tar_GetAccessProfiles_rly_xml(char * p_buf, int mlen, const char * argv);
int build_tar_GetAccessProfileList_rly_xml(char * p_buf, int mlen, const char * argv);
int build_tar_CreateAccessProfile_rly_xml(char * p_buf, int mlen, const char * argv);
int build_tar_ModifyAccessProfile_rly_xml(char * p_buf, int mlen, const char * argv);
int build_tar_DeleteAccessProfile_rly_xml(char * p_buf, int mlen, const char * argv);

#endif // end of ACCESS_RULES

#ifdef SCHEDULE_SUPPORT

int build_tsc_GetScheduleInfo_rly_xml(char * p_buf, int mlen, const char * argv);
int build_tsc_GetScheduleInfoList_rly_xml(char * p_buf, int mlen, const char * argv);
int build_tsc_GetSchedules_rly_xml(char * p_buf, int mlen, const char * argv);
int build_tsc_GetScheduleList_rly_xml(char * p_buf, int mlen, const char * argv);
int build_tsc_CreateSchedule_rly_xml(char * p_buf, int mlen, const char * argv);
int build_tsc_ModifySchedule_rly_xml(char * p_buf, int mlen, const char * argv);
int build_tsc_DeleteSchedule_rly_xml(char * p_buf, int mlen, const char * argv);
int build_tsc_GetSpecialDayGroupInfo_rly_xml(char * p_buf, int mlen, const char * argv);
int build_tsc_GetSpecialDayGroupInfoList_rly_xml(char * p_buf, int mlen, const char * argv);
int build_tsc_GetSpecialDayGroups_rly_xml(char * p_buf, int mlen, const char * argv);
int build_tsc_GetSpecialDayGroupList_rly_xml(char * p_buf, int mlen, const char * argv);
int build_tsc_CreateSpecialDayGroup_rly_xml(char * p_buf, int mlen, const char * argv);
int build_tsc_ModifySpecialDayGroup_rly_xml(char * p_buf, int mlen, const char * argv);
int build_tsc_DeleteSpecialDayGroup_rly_xml(char * p_buf, int mlen, const char * argv);
int build_tsc_GetScheduleState_rly_xml(char * p_buf, int mlen, const char * argv);

#endif // end of SCHEDULE_SUPPORT

#ifdef RECEIVER_SUPPORT

int build_trv_GetReceivers_rly_xml(char * p_buf, int mlen, const char * argv);
int build_trv_GetReceiver_rly_xml(char * p_buf, int mlen, const char * argv);
int build_trv_CreateReceiver_rly_xml(char * p_buf, int mlen, const char * argv);
int build_trv_DeleteReceiver_rly_xml(char * p_buf, int mlen, const char * argv);
int build_trv_ConfigureReceiver_rly_xml(char * p_buf, int mlen, const char * argv);
int build_trv_SetReceiverMode_rly_xml(char * p_buf, int mlen, const char * argv);
int build_trv_GetReceiverState_rly_xml(char * p_buf, int mlen, const char * argv);

#endif // end of RECEIVER_SUPPORT

#ifdef __cplusplus
}
#endif

#endif 


