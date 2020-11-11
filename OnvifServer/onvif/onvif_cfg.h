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

#ifndef ONVIF_CONFIG_H
#define ONVIF_CONFIG_H

#ifdef __cplusplus
extern "C" {
#endif

void onvif_load_cfg();
BOOL onvif_read_device_uuid(char * buff, int len);
BOOL onvif_save_device_uuid(char * buff);

#ifdef PROFILE_Q_SUPPORT
int  onvif_read_device_state();
BOOL onvif_save_device_state(int state);
#endif


#ifdef __cplusplus
}
#endif

#endif


