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

#ifndef ONVIF_ACCESSRULES_H
#define ONVIF_ACCESSRULES_H

#include "sys_inc.h"
#include "onvif_cm.h"


typedef struct 
{
	int     sizeToken;	                                // sequence of elements <Token>
	char    Token[ACCESSRULES_MAX_LIMIT][ONVIF_TOKEN_LEN];  // required, Tokens of CredentialInfo items to get
} tar_GetAccessProfileInfo_REQ;

typedef struct 
{
	int     sizeAccessProfileInfo;	                    // sequence of elements <AccessProfileInfo>

	onvif_AccessProfileInfo AccessProfileInfo[ACCESSRULES_MAX_LIMIT];   // optional, List of AccessProfileInfo items
} tar_GetAccessProfileInfo_RES;

typedef struct 
{
    uint32  LimitFlag           : 1;                    // Indicates whether the field Limit is valid
    uint32  StartReferenceFlag  : 1;                    // Indicates whether the field StartReference is valid
    uint32  Reserved            : 30;
    
	int     Limit;	                                    // optional, Maximum number of entries to return. If not specified, less than one or higher 
	                                                    //  than what the device supports, the number of items is determined by the device
	char    StartReference[ONVIF_TOKEN_LEN];	        // optional, Start returning entries from this start reference. If not specified,
									                    //  entries shall start from the beginning of the dataset
} tar_GetAccessProfileInfoList_REQ;

typedef struct 
{
    uint32  NextStartReferenceFlag  : 1;                // Indicates whether the field NextStartReference is valid
    uint32  Reserved                : 31;
    
	char    NextStartReference[ONVIF_TOKEN_LEN];	    // optional, 
	int     sizeAccessProfileInfo;	                    // sequence of elements <AccessProfileInfo>

	onvif_AccessProfileInfo     AccessProfileInfo[ACCESSRULES_MAX_LIMIT];   // optional, List of AccessProfileInfo items
} tar_GetAccessProfileInfoList_RES;

typedef struct 
{
	int     sizeToken;	                                // sequence of elements <Token>
	char    Token[ACCESSRULES_MAX_LIMIT][ONVIF_TOKEN_LEN];  // required, Tokens of CredentialInfo items to get
} tar_GetAccessProfiles_REQ;

typedef struct 
{
	int     sizeAccessProfile;	                        // sequence of elements <AccessProfile>

	onvif_AccessProfile AccessProfile[ACCESSRULES_MAX_LIMIT];   // optional, List of Access Profile items
} tar_GetAccessProfiles_RES;

typedef struct 
{
    uint32  LimitFlag           : 1;                    // Indicates whether the field Limit is valid
    uint32  StartReferenceFlag  : 1;                    // Indicates whether the field StartReference is valid
    uint32  Reserved            : 30;
    
	int     Limit;	                                    // optional, Maximum number of entries to return. If not specified, less than one or higher 
	                                                    //  than what the device supports, the number of items is determined by the device
	char    StartReference[ONVIF_TOKEN_LEN];	        // optional, Start returning entries from this start reference. If not specified,
									                    //  entries shall start from the beginning of the dataset
} tar_GetAccessProfileList_REQ;

typedef struct 
{
    uint32  NextStartReferenceFlag  : 1;                // Indicates whether the field NextStartReference is valid
    uint32  Reserved                : 31;
    
	char    NextStartReference[ONVIF_TOKEN_LEN];	    // optional, 
	int     sizeAccessProfile;	                        // sequence of elements <AccessProfile>

	onvif_AccessProfile AccessProfile[ACCESSRULES_MAX_LIMIT];   // optional, List of Access Profile items
} tar_GetAccessProfileList_RES;

typedef struct 
{
	onvif_AccessProfile AccessProfile;	                // required, The AccessProfile to create
} tar_CreateAccessProfile_REQ;

typedef struct 
{
	char    Token[ONVIF_TOKEN_LEN];	                    // required, The Token of created AccessProfile
} tar_CreateAccessProfile_RES;

typedef struct 
{
	onvif_AccessProfile AccessProfile;	                // required, The details of Access Profile
} tar_ModifyAccessProfile_REQ;

typedef struct 
{
	char    Token[ONVIF_TOKEN_LEN];	                    // required, The token of the access profile to delete
} tar_DeleteAccessProfile_REQ;


#ifdef __cplusplus
extern "C" {
#endif

ONVIF_RET onvif_tar_GetAccessProfileInfo(tar_GetAccessProfileInfo_REQ * p_req, tar_GetAccessProfileInfo_RES * p_res);
ONVIF_RET onvif_tar_GetAccessProfileInfoList(tar_GetAccessProfileInfoList_REQ * p_req, tar_GetAccessProfileInfoList_RES * p_res);
ONVIF_RET onvif_tar_GetAccessProfiles(tar_GetAccessProfiles_REQ * p_req, tar_GetAccessProfiles_RES * p_res);
ONVIF_RET onvif_tar_GetAccessProfileList(tar_GetAccessProfileList_REQ * p_req, tar_GetAccessProfileList_RES * p_res);
ONVIF_RET onvif_tar_CreateAccessProfile(tar_CreateAccessProfile_REQ * p_req, tar_CreateAccessProfile_RES * p_res);
ONVIF_RET onvif_tar_ModifyAccessProfile(tar_ModifyAccessProfile_REQ * p_req);
ONVIF_RET onvif_tar_DeleteAccessProfile(tar_DeleteAccessProfile_REQ * p_req);


#ifdef __cplusplus
}
#endif

#endif // ONVIF_ACCESSRULES_H



