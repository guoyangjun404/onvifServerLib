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

#ifndef HTTP_AUTH_H
#define HTTP_AUTH_H

#include "rfc_md5.h"

#define IN
#define OUT

#ifdef __cplusplus
extern "C" {
#endif

void DigestCalcHA1(
	IN const char * pszAlg,
	IN const char * pszUserName,
	IN const char * pszRealm,
	IN const char * pszPassword,
	IN const char * pszNonce,
	IN const char * pszCNonce,
	OUT HASHHEX SessionKey
);

void DigestCalcResponseHash(
	IN HASHHEX HA1, /* H(A1) */
	IN const char * pszNonce, /* nonce from server */
	IN const char * pszNonceCount, /* 8 hex digits */
	IN const char * pszCNonce, /* client nonce */
	IN const char * pszQop, /* qop-value: "", "auth", "auth-int" */
	IN const char * pszMethod, /* method from the request */
	IN const char * pszDigestUri, /* requested URL */
	IN HASHHEX HEntity, /* H(entity body) if qop="auth-int" */
	OUT HASH RespHash /* request-digest or response-digest */
);

void DigestCalcResponse(
	IN HASHHEX HA1, /* H(A1) */
	IN const char * pszNonce, /* nonce from server */
	IN const char * pszNonceCount, /* 8 hex digits */
	IN const char * pszCNonce, /* client nonce */
	IN const char * pszQop, /* qop-value: "", "auth", "auth-int" */
	IN const char * pszMethod, /* method from the request */
	IN const char * pszDigestUri, /* requested URL */
	IN HASHHEX HEntity, /* H(entity body) if qop="auth-int" */
	OUT HASHHEX Response /* request-digest or response-digest */
);

BOOL DigestAuthProcess(HD_AUTH_INFO * p_auth, HD_AUTH_INFO * p_lauth, const char * method, int oplevel);

#ifdef __cplusplus
}
#endif

#endif // HTTP_AUTH_H


