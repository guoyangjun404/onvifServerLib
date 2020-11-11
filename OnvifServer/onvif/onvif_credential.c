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
#include "onvif_credential.h"
#include "onvif.h"
#include "onvif_event.h"


#ifdef CREDENTIAL_SUPPORT

/********************************************************************************/
extern ONVIF_CFG g_onvif_cfg;
extern ONVIF_CLS g_onvif_cls;

/********************************************************************************/

void onvif_CredentialRemovedNotify(ONVIF_Credential * p_req)
{
    ONVIF_NotificationMessage * p_message = onvif_add_NotificationMessage(NULL);
	if (p_message)
	{
	    ONVIF_SimpleItem * p_simpleitem;
	    
		strcpy(p_message->NotificationMessage.Dialect, "http://www.onvif.org/ver10/tev/topicExpression/ConcreteSet");
		strcpy(p_message->NotificationMessage.Topic, "tns1:Configuration/Credential/Removed");
		p_message->NotificationMessage.Message.PropertyOperationFlag = 1;
		p_message->NotificationMessage.Message.PropertyOperation = PropertyOperation_Deleted;
		p_message->NotificationMessage.Message.UtcTime = time(NULL)+1;

        p_message->NotificationMessage.Message.SourceFlag = 1;
        
		p_simpleitem = onvif_add_SimpleItem(&p_message->NotificationMessage.Message.Source.SimpleItem);
		if (p_simpleitem)
		{		    
			strcpy(p_simpleitem->SimpleItem.Name, "CredentialToken");
			strcpy(p_simpleitem->SimpleItem.Value, p_req->Credential.token);
		}

		onvif_put_NotificationMessage(p_message);
	}
}

void onvif_CredentialStateNotify(ONVIF_Credential * p_req, BOOL ClientUpdated)
{
    ONVIF_NotificationMessage * p_message = onvif_add_NotificationMessage(NULL);
	if (p_message)
	{
	    ONVIF_SimpleItem * p_simpleitem;
	    
		strcpy(p_message->NotificationMessage.Dialect, "http://www.onvif.org/ver10/tev/topicExpression/ConcreteSet");
		strcpy(p_message->NotificationMessage.Topic, "tns1:Credential/State/Enabled");
		p_message->NotificationMessage.Message.PropertyOperationFlag = 1;
		p_message->NotificationMessage.Message.PropertyOperation = PropertyOperation_Changed;
		p_message->NotificationMessage.Message.UtcTime = time(NULL)+1;

        p_message->NotificationMessage.Message.SourceFlag = 1;
        
		p_simpleitem = onvif_add_SimpleItem(&p_message->NotificationMessage.Message.Source.SimpleItem);
		if (p_simpleitem)
		{		    
			strcpy(p_simpleitem->SimpleItem.Name, "CredentialToken");
			strcpy(p_simpleitem->SimpleItem.Value, p_req->Credential.token);
		}

		p_message->NotificationMessage.Message.DataFlag = 1;

		p_simpleitem = onvif_add_SimpleItem(&p_message->NotificationMessage.Message.Data.SimpleItem);
		if (p_simpleitem)
		{		    
			strcpy(p_simpleitem->SimpleItem.Name, "State");
			strcpy(p_simpleitem->SimpleItem.Value, p_req->State.Enabled ? "true" : "false");
		}

		p_simpleitem = onvif_add_SimpleItem(&p_message->NotificationMessage.Message.Data.SimpleItem);
		if (p_simpleitem)
		{		    
			strcpy(p_simpleitem->SimpleItem.Name, "Reason");
			strcpy(p_simpleitem->SimpleItem.Value, p_req->State.Reason);
		}

		p_simpleitem = onvif_add_SimpleItem(&p_message->NotificationMessage.Message.Data.SimpleItem);
		if (p_simpleitem)
		{		    
			strcpy(p_simpleitem->SimpleItem.Name, "ClientUpdated");
			strcpy(p_simpleitem->SimpleItem.Value, ClientUpdated ? "true" : "false");
		}

		onvif_put_NotificationMessage(p_message);
	}
}

void onvif_CredentialChangedNotify(ONVIF_Credential * p_req)
{
    ONVIF_NotificationMessage * p_message = onvif_add_NotificationMessage(NULL);
	if (p_message)
	{
	    ONVIF_SimpleItem * p_simpleitem;
	    
		strcpy(p_message->NotificationMessage.Dialect, "http://www.onvif.org/ver10/tev/topicExpression/ConcreteSet");
		strcpy(p_message->NotificationMessage.Topic, "tns1:Configuration/Credential/Changed");
		p_message->NotificationMessage.Message.PropertyOperationFlag = 1;
		p_message->NotificationMessage.Message.PropertyOperation = PropertyOperation_Changed;
		p_message->NotificationMessage.Message.UtcTime = time(NULL)+1;

        p_message->NotificationMessage.Message.SourceFlag = 1;
        
		p_simpleitem = onvif_add_SimpleItem(&p_message->NotificationMessage.Message.Source.SimpleItem);
		if (p_simpleitem)
		{		    
			strcpy(p_simpleitem->SimpleItem.Name, "CredentialToken");
			strcpy(p_simpleitem->SimpleItem.Value, p_req->Credential.token);
		}

		onvif_put_NotificationMessage(p_message);
	}
}

void onvif_CredentialApbViolationNotify(ONVIF_Credential * p_req, BOOL ClientUpdated)
{
    ONVIF_NotificationMessage * p_message = onvif_add_NotificationMessage(NULL);
	if (p_message)
	{
	    ONVIF_SimpleItem * p_simpleitem;
	    
		strcpy(p_message->NotificationMessage.Dialect, "http://www.onvif.org/ver10/tev/topicExpression/ConcreteSet");
		strcpy(p_message->NotificationMessage.Topic, "tns1:Credential/State/ApbViolation");
		p_message->NotificationMessage.Message.PropertyOperationFlag = 1;
		p_message->NotificationMessage.Message.PropertyOperation = PropertyOperation_Changed;
		p_message->NotificationMessage.Message.UtcTime = time(NULL)+1;

        p_message->NotificationMessage.Message.SourceFlag = 1;
        
		p_simpleitem = onvif_add_SimpleItem(&p_message->NotificationMessage.Message.Source.SimpleItem);
		if (p_simpleitem)
		{		    
			strcpy(p_simpleitem->SimpleItem.Name, "CredentialToken");
			strcpy(p_simpleitem->SimpleItem.Value, p_req->Credential.token);
		}

		p_message->NotificationMessage.Message.DataFlag = 1;

        p_simpleitem = onvif_add_SimpleItem(&p_message->NotificationMessage.Message.Data.SimpleItem);
		if (p_simpleitem)
		{		    
			strcpy(p_simpleitem->SimpleItem.Name, "ApbViolation");
			strcpy(p_simpleitem->SimpleItem.Value, p_req->State.AntipassbackState.AntipassbackViolated ? "true" : "false");
		}
		
		p_simpleitem = onvif_add_SimpleItem(&p_message->NotificationMessage.Message.Data.SimpleItem);
		if (p_simpleitem)
		{		    
			strcpy(p_simpleitem->SimpleItem.Name, "ClientUpdated");
			strcpy(p_simpleitem->SimpleItem.Value, ClientUpdated ? "true" : "false");
		}

		onvif_put_NotificationMessage(p_message);
	}
}


/**
 * The possible return values:
 *	ONVIF_ERR_TooManyItems
 **/ 
ONVIF_RET onvif_tcr_GetCredentialInfo(tcr_GetCredentialInfo_REQ * p_req, tcr_GetCredentialInfo_RES * p_res)
{
    int i;
    int idx;
    ONVIF_Credential * p_tmp;
    
    for (i = 0; i< p_req->sizeToken; i++)
    {
        p_tmp = onvif_find_Credential(p_req->Token[i]);
        if (p_tmp)
        {
            idx = p_res->sizeCredentialInfo;

            p_res->CredentialInfo[idx].DescriptionFlag = p_tmp->Credential.DescriptionFlag;
            p_res->CredentialInfo[idx].ValidFromFlag = p_tmp->Credential.ValidFromFlag;            
            p_res->CredentialInfo[idx].ValidToFlag = p_tmp->Credential.ValidToFlag;            

            strcpy(p_res->CredentialInfo[idx].ValidFrom, p_tmp->Credential.ValidFrom);
            strcpy(p_res->CredentialInfo[idx].ValidTo, p_tmp->Credential.ValidTo);
            strcpy(p_res->CredentialInfo[idx].token, p_tmp->Credential.token);
            strcpy(p_res->CredentialInfo[idx].Description, p_tmp->Credential.Description);
            strcpy(p_res->CredentialInfo[idx].CredentialHolderReference, p_tmp->Credential.CredentialHolderReference);
            
            p_res->sizeCredentialInfo++;
        }
    }

    return ONVIF_OK;
}

/**
 * The possible return values:
 *	ONVIF_ERR_InvalidStartReference
 **/
ONVIF_RET onvif_tcr_GetCredentialInfoList(tcr_GetCredentialInfoList_REQ * p_req, tcr_GetCredentialInfoList_RES * p_res)
{
    int idx;
    int nums = 0;
    ONVIF_Credential * p_tmp = g_onvif_cfg.credential;
    
    if (p_req->StartReferenceFlag)
    {
        p_tmp = onvif_find_Credential(p_req->StartReference);
        if (NULL == p_tmp)
        {
            return ONVIF_ERR_InvalidStartReference;
        }
    }

    while (p_tmp)
    {
        idx = p_res->sizeCredentialInfo;

        p_res->CredentialInfo[idx].DescriptionFlag = p_tmp->Credential.DescriptionFlag;
        p_res->CredentialInfo[idx].ValidFromFlag = p_tmp->Credential.ValidFromFlag;
        p_res->CredentialInfo[idx].ValidToFlag = p_tmp->Credential.ValidToFlag;

        strcpy(p_res->CredentialInfo[idx].ValidFrom, p_tmp->Credential.ValidFrom);
        strcpy(p_res->CredentialInfo[idx].ValidTo, p_tmp->Credential.ValidTo);
        strcpy(p_res->CredentialInfo[idx].token, p_tmp->Credential.token);
        strcpy(p_res->CredentialInfo[idx].Description, p_tmp->Credential.Description);
        strcpy(p_res->CredentialInfo[idx].CredentialHolderReference, p_tmp->Credential.CredentialHolderReference);
            
        p_res->sizeCredentialInfo++;

        p_tmp = p_tmp->next;
        
        nums++;
        if (p_req->LimitFlag && nums >= p_req->Limit)
        {
            break;
        }
        else if (nums >= ARRAY_SIZE(p_res->CredentialInfo))
        {
            break;
        }
    }

    if (p_tmp)
    {
        p_res->NextStartReferenceFlag = 1;
        strcpy(p_res->NextStartReference, p_tmp->Credential.token);
    }    
    
    return ONVIF_OK;
}

/**
 * The possible return values:
 *	ONVIF_ERR_TooManyItems
 **/
ONVIF_RET onvif_tcr_GetCredentials(tcr_GetCredentials_REQ * p_req, tcr_GetCredentials_RES * p_res)
{
    int i;
    int idx;
    ONVIF_Credential * p_tmp;
    
    for (i = 0; i< p_req->sizeToken; i++)
    {
        p_tmp = onvif_find_Credential(p_req->Token[i]);
        if (p_tmp)
        {
            idx = p_res->sizeCredential;

            memcpy(&p_res->Credential[idx], &p_tmp->Credential, sizeof(onvif_Credential));
            
            p_res->sizeCredential++;
        }
    }

    return ONVIF_OK;
}

/**
 * The possible return values:
 *	ONVIF_ERR_InvalidStartReference
 **/
ONVIF_RET onvif_tcr_GetCredentialList(tcr_GetCredentialList_REQ * p_req, tcr_GetCredentialList_RES * p_res)
{
    int idx;
    int nums = 0;
    ONVIF_Credential * p_tmp = g_onvif_cfg.credential;
    
    if (p_req->StartReferenceFlag)
    {
        p_tmp = onvif_find_Credential(p_req->StartReference);
        if (NULL == p_tmp)
        {
            return ONVIF_ERR_InvalidStartReference;
        }
    }

    while (p_tmp)
    {
        idx = p_res->sizeCredential;

        memcpy(&p_res->Credential[idx], &p_tmp->Credential, sizeof(onvif_Credential));
        
        p_res->sizeCredential++;        

        p_tmp = p_tmp->next;
        
        nums++;
        if (p_req->LimitFlag && nums >= p_req->Limit)
        {
            break;
        }
        else if (nums >= ARRAY_SIZE(p_res->Credential))
        {
            break;
        }
    }

    if (p_tmp)
    {
        p_res->NextStartReferenceFlag = 1;
        strcpy(p_res->NextStartReference, p_tmp->Credential.token);
    }    
    
    return ONVIF_OK;
}

/**
 * The possible return values:
 *  ONVIF_ERR_MaxAccessProfilesPerCredential
 *  ONVIF_ERR_CredentialValiditySupported
 *  ONVIF_ERR_CredentialAccessProfileValiditySupported
 *  ONVIF_ERR_SupportedIdentifierType
 *  ONVIF_ERR_DuplicatedIdentifierType
 *  ONVIF_ERR_InvalidFormatType
 *  ONVIF_ERR_InvalidIdentifierValue
 *  ONVIF_ERR_DuplicatedIdentifierValue
 *  ONVIF_ERR_ReferenceNotFound
 *  ONVIF_ERR_ExemptFromAuthenticationSupported
 *  ONVIF_ERR_MaxCredentials
 **/
ONVIF_RET onvif_tcr_CreateCredential(tcr_CreateCredential_REQ * p_req, tcr_CreateCredential_RES * p_res)
{
    int i;
    ONVIF_Credential * p_tmp;
    
    // parameter check
    if (strlen(p_req->Credential.token) > 0)
    {
        return ONVIF_ERR_InvalidArgVal;
    }

    // add new credential
    p_tmp = onvif_add_Credential();
    if (NULL == p_tmp)
    {
        return ONVIF_ERR_MaxCredentials;
    }

    memcpy(&p_tmp->Credential, &p_req->Credential, sizeof(onvif_Credential));
    memcpy(&p_tmp->State, &p_req->State, sizeof(onvif_CredentialState));

    sprintf(p_tmp->Credential.token, "CredentialToken%d", g_onvif_cls.credential_idx++);
    strcpy(p_res->Token, p_tmp->Credential.token);

    for (i = 0; i < p_req->Credential.sizeCredentialIdentifier; i++)
    {
        p_tmp->Credential.CredentialIdentifier[i].Used = 1;
    }

    for (i = 0; i < p_req->Credential.sizeCredentialAccessProfile; i++)
    {
        p_tmp->Credential.CredentialAccessProfile[i].Used = 1;
    }

    for (i = 0; i < p_req->Credential.sizeAttribute; i++)
    {
        p_tmp->Credential.Attribute[i].Used = 1;
    }
    
    // here add hander code ...

    // send event notify
    onvif_CredentialChangedNotify(p_tmp);
    
    return ONVIF_OK;
}

/**
 * The possible return values:
 *  ONVIF_ERR_NotFound
 *  ONVIF_ERR_CredentialValiditySupported
 *  ONVIF_ERR_CredentialAccessProfileValiditySupported
 *  ONVIF_ERR_SupportedIdentifierType
 *  ONVIF_ERR_DuplicatedIdentifierType
 *  ONVIF_ERR_InvalidFormatType
 *  ONVIF_ERR_InvalidIdentifierValue
 *  ONVIF_ERR_DuplicatedIdentifierValue
 *  ONVIF_ERR_ReferenceNotFound
 *  ONVIF_ERR_ExemptFromAuthenticationSupported
 **/
ONVIF_RET onvif_tcr_ModifyCredential(tcr_ModifyCredential_REQ * p_req)
{
    ONVIF_Credential * p_tmp = onvif_find_Credential(p_req->Credential.token);
    if (NULL == p_tmp)
    {
        return ONVIF_ERR_NotFound;
    }
    
    // parameter check

    // here add hander code ...
    
    // modify the credential
    memcpy(&p_tmp->Credential, &p_req->Credential, sizeof(onvif_Credential));

    // send event notify
    onvif_CredentialChangedNotify(p_tmp);
            
    return ONVIF_OK;
}

/**
 * The possible return values:
 *  ONVIF_ERR_NotFound
 *  ONVIF_ERR_ReferenceInUse
 **/
ONVIF_RET onvif_tcr_DeleteCredential(tcr_DeleteCredential_REQ * p_req)
{
    ONVIF_Credential * p_tmp = onvif_find_Credential(p_req->Token);
    if (NULL == p_tmp)
    {
        return ONVIF_ERR_NotFound;
    }

    // here add hander code ...

    // send event notify
    onvif_CredentialRemovedNotify(p_tmp);
        
    // delete the credential
    onvif_free_Credential(p_tmp);

    return ONVIF_OK;
}

/**
 * The possible return values:
 *  ONVIF_ERR_NotFound
 **/
ONVIF_RET onvif_tcr_GetCredentialState(tcr_GetCredentialState_REQ * p_req, tcr_GetCredentialState_RES * p_res)
{
    ONVIF_Credential * p_tmp = onvif_find_Credential(p_req->Token);
    if (NULL == p_tmp)
    {
        return ONVIF_ERR_NotFound;
    }

    memcpy(&p_res->State, &p_tmp->State, sizeof(onvif_CredentialState));
    
    return ONVIF_OK;
}

/**
 * The possible return values:
 *  ONVIF_ERR_NotFound
 **/
ONVIF_RET onvif_tcr_EnableCredential(tcr_EnableCredential_REQ * p_req)
{
    ONVIF_Credential * p_tmp = onvif_find_Credential(p_req->Token);
    if (NULL == p_tmp)
    {
        return ONVIF_ERR_NotFound;
    }

    // here add hander code ... 

    // endable the credentail
    
    p_tmp->State.Enabled = TRUE;
    if (p_req->ReasonFlag)
    {
        p_tmp->State.ReasonFlag = 1;
        strcpy(p_tmp->State.Reason, p_req->Reason);
    }

    // send event notify
    onvif_CredentialStateNotify(p_tmp, TRUE);
    
    return ONVIF_OK;
}

/**
 * The possible return values:
 *  ONVIF_ERR_NotFound
 **/
ONVIF_RET onvif_tcr_DisableCredential(tcr_DisableCredential_REQ * p_req)
{
    ONVIF_Credential * p_tmp = onvif_find_Credential(p_req->Token);
    if (NULL == p_tmp)
    {
        return ONVIF_ERR_NotFound;
    }

    // here add hander code ... 

    // disable the credentail
    
    p_tmp->State.Enabled = FALSE;
    if (p_req->ReasonFlag)
    {
        p_tmp->State.ReasonFlag = 1;
        strcpy(p_tmp->State.Reason, p_req->Reason);
    }

    // send event notify
    onvif_CredentialStateNotify(p_tmp, TRUE);
    
    return ONVIF_OK;
}

/**
 * The possible return values:
 *  ONVIF_ERR_NotFound
 **/
ONVIF_RET onvif_tcr_ResetAntipassbackViolation(tcr_ResetAntipassbackViolation_REQ * p_req)
{
    ONVIF_Credential * p_tmp = onvif_find_Credential(p_req->CredentialToken);
    if (NULL == p_tmp)
    {
        return ONVIF_ERR_NotFound;
    }

    // here add hander code ... 

    // reset AntipassbackViolation

    if (p_tmp->State.AntipassbackStateFlag)
    {
        p_tmp->State.AntipassbackState.AntipassbackViolated = FALSE;
    }

    // send event notify
    onvif_CredentialApbViolationNotify(p_tmp, TRUE);
    
    return ONVIF_OK;
}

/**
 * The possible return values:
 *  ONVIF_ERR_NotFound
 **/
ONVIF_RET onvif_tcr_GetSupportedFormatTypes(tcr_GetSupportedFormatTypes_REQ * p_req, tcr_GetSupportedFormatTypes_RES * p_res)
{
    p_res->sizeFormatTypeInfo = 2;
    strcpy(p_res->FormatTypeInfo[0].FormatType, "GUID");
    strcpy(p_res->FormatTypeInfo[0].Description, "test");
    strcpy(p_res->FormatTypeInfo[1].FormatType, "CHUID");
    strcpy(p_res->FormatTypeInfo[1].Description, "test1");

    //strcpy(p_res->FormatTypeInfo[0].FormatType, "USER_PASSWORD");
    //strcpy(p_res->FormatTypeInfo[0].FormatType, "WIEGAND26");
    
    return ONVIF_OK;
}

/**
 * The possible return values:
 *  ONVIF_ERR_NotFound
 **/
ONVIF_RET onvif_tcr_GetCredentialIdentifiers(tcr_GetCredentialIdentifiers_REQ * p_req, tcr_GetCredentialIdentifiers_RES * p_res)
{
    int i;
    int idx = 0;
    ONVIF_Credential * p_tmp = onvif_find_Credential(p_req->CredentialToken);
    if (NULL == p_tmp)
    {
        return ONVIF_ERR_NotFound;
    }

    p_res->sizeCredentialIdentifier = p_tmp->Credential.sizeCredentialIdentifier;

    for (i = 0; i < ARRAY_SIZE(p_tmp->Credential.CredentialIdentifier); i++)
    {
        if (!p_tmp->Credential.CredentialIdentifier[i].Used)
        {
            continue;
        }

        memcpy(&p_res->CredentialIdentifier[idx++], &p_tmp->Credential.CredentialIdentifier[i], sizeof(onvif_CredentialIdentifier));
    }
    
    return ONVIF_OK;
}

/**
 * The possible return values:
 *  ONVIF_ERR_NotFound
 *  ONVIF_ERR_SupportedIdentifierType
 *  ONVIF_ERR_InvalidFormatType
 *  ONVIF_ERR_InvalidIdentifierValue
 *  ONVIF_ERR_ExemptFromAuthenticationSupported
 **/
ONVIF_RET onvif_tcr_SetCredentialIdentifier(tcr_SetCredentialIdentifier_REQ * p_req)
{
    int i;
    ONVIF_Credential * p_tmp = onvif_find_Credential(p_req->CredentialToken);
    if (NULL == p_tmp)
    {
        return ONVIF_ERR_NotFound;
    }

    for (i = 0; i < ARRAY_SIZE(p_tmp->Credential.CredentialIdentifier); i++)
    {
        if (!p_tmp->Credential.CredentialIdentifier[i].Used)
        {
            continue;
        }
        
        if (strcmp(p_tmp->Credential.CredentialIdentifier[i].Type.Name, p_req->CredentialIdentifier.Type.Name) == 0)
        {
            // update the credential identifier
            memcpy(&p_tmp->Credential.CredentialIdentifier[i], &p_req->CredentialIdentifier, sizeof(onvif_CredentialIdentifier));
			p_tmp->Credential.CredentialIdentifier[i].Used = TRUE; 

            // send event notify
            onvif_CredentialChangedNotify(p_tmp);

            // here add handler code ...

            return ONVIF_OK;
        }
    }

    for (i = 0; i < ARRAY_SIZE(p_tmp->Credential.CredentialIdentifier); i++)
    {
        if (p_tmp->Credential.CredentialIdentifier[i].Used)
        {
            continue;
        }
        
        // add the credential identifier
        p_tmp->Credential.sizeCredentialIdentifier++;               
        memcpy(&p_tmp->Credential.CredentialIdentifier[i], &p_req->CredentialIdentifier, sizeof(onvif_CredentialIdentifier));

        p_tmp->Credential.CredentialIdentifier[i].Used = TRUE; 

        // send event notify
        onvif_CredentialChangedNotify(p_tmp);
            
        // here add handler code ...     

        break;
    }

    return ONVIF_OK;
}

/**
 * The possible return values:
 *  ONVIF_ERR_NotFound
 *  ONVIF_ERR_MinIdentifiersPerCredential
 **/
ONVIF_RET onvif_tcr_DeleteCredentialIdentifier(tcr_DeleteCredentialIdentifier_REQ * p_req)
{
    int i;
    ONVIF_Credential * p_tmp = onvif_find_Credential(p_req->CredentialToken);
    if (NULL == p_tmp)
    {
        return ONVIF_ERR_NotFound;
    }

    for (i = 0; i < ARRAY_SIZE(p_tmp->Credential.CredentialIdentifier); i++)
    {
        if (!p_tmp->Credential.CredentialIdentifier[i].Used)
        {
            continue;
        }
        
        if (strcmp(p_tmp->Credential.CredentialIdentifier[i].Type.Name, p_req->CredentialIdentifierTypeName) == 0)
        {
            // delete the credential identifier
            
            if (p_tmp->Credential.sizeCredentialIdentifier > 1)
            {
                p_tmp->Credential.sizeCredentialIdentifier--;
                p_tmp->Credential.CredentialIdentifier[i].Used = FALSE;

                // send event notify
                onvif_CredentialChangedNotify(p_tmp);
            }
            else
            {
                return ONVIF_ERR_MinIdentifiersPerCredential;
            }

            // here add handler code ...

            
        }
    }

    return ONVIF_OK;
}

/**
 * The possible return values:
 *  ONVIF_ERR_NotFound
 **/
ONVIF_RET onvif_tcr_GetCredentialAccessProfiles(tcr_GetCredentialAccessProfiles_REQ * p_req, tcr_GetCredentialAccessProfiles_RES * p_res)
{
    int i;
    int idx = 0;
    ONVIF_Credential * p_tmp = onvif_find_Credential(p_req->CredentialToken);
    if (NULL == p_tmp)
    {
        return ONVIF_ERR_NotFound;
    }

    p_res->sizeCredentialAccessProfile = p_tmp->Credential.sizeCredentialAccessProfile;

    for (i = 0; i < ARRAY_SIZE(p_tmp->Credential.CredentialAccessProfile); i++)
    {
        if (!p_tmp->Credential.CredentialAccessProfile[i].Used)
        {
            continue;
        }

        memcpy(&p_res->CredentialAccessProfile[idx++], &p_tmp->Credential.CredentialAccessProfile[i], sizeof(onvif_CredentialAccessProfile));
    }
    
    return ONVIF_OK;
}

/**
 * The possible return values:
 *  ONVIF_ERR_NotFound
 *  ONVIF_ERR_MaxAccessProfilesPerCredential
 *  ONVIF_ERR_CredentialAccessProfileValiditySupported
 *  ONVIF_ERR_ReferenceNotFound
 **/
ONVIF_RET onvif_tcr_SetCredentialAccessProfiles(tcr_SetCredentialAccessProfiles_REQ * p_req)
{
    int i, j;
    ONVIF_Credential * p_tmp = onvif_find_Credential(p_req->CredentialToken);
    if (NULL == p_tmp)
    {
        return ONVIF_ERR_NotFound;
    }

    for (j = 0; j < p_req->sizeCredentialAccessProfile; j++)
    {
        for (i = 0; i < ARRAY_SIZE(p_tmp->Credential.CredentialAccessProfile); i++)
        {
            if (!p_tmp->Credential.CredentialAccessProfile[i].Used)
            {
                continue;
            }
            
            if (strcmp(p_tmp->Credential.CredentialAccessProfile[i].AccessProfileToken, p_req->CredentialAccessProfile[j].AccessProfileToken) == 0)
            {
                // update the credential Access Profile
                memcpy(&p_tmp->Credential.CredentialAccessProfile[i], &p_req->CredentialAccessProfile[j], sizeof(onvif_CredentialAccessProfile));
                p_tmp->Credential.CredentialAccessProfile[i].Used = 1;

                // here add handler code ...

                // send event notify
                onvif_CredentialChangedNotify(p_tmp);
            
                return ONVIF_OK;
            }
        }

        for (i = 0; i < ARRAY_SIZE(p_tmp->Credential.CredentialAccessProfile); i++)
        {
            if (p_tmp->Credential.CredentialAccessProfile[i].Used)
            {
                continue;
            }
            
            // add the credential identifier
            if (p_tmp->Credential.sizeCredentialAccessProfile < ARRAY_SIZE(p_tmp->Credential.CredentialAccessProfile))
            {
                p_tmp->Credential.sizeCredentialAccessProfile++;                        
                memcpy(&p_tmp->Credential.CredentialAccessProfile[i], &p_req->CredentialAccessProfile[j], sizeof(onvif_CredentialAccessProfile));
                p_tmp->Credential.CredentialAccessProfile[i].Used = TRUE;
                
                // here add handler code ...

                // send event notify
                onvif_CredentialChangedNotify(p_tmp);
            }
            else
            {
                
            }            
            
            break;
        }
    }

    return ONVIF_OK;
}

/**
 * The possible return values:
 *  ONVIF_ERR_NotFound
 **/
ONVIF_RET onvif_tcr_DeleteCredentialAccessProfiles(tcr_DeleteCredentialAccessProfiles_REQ * p_req)
{
    int i, j;
    ONVIF_Credential * p_tmp = onvif_find_Credential(p_req->CredentialToken);
    if (NULL == p_tmp)
    {
        return ONVIF_ERR_NotFound;
    }

    for (j = 0; j < p_req->sizeAccessProfileToken; j++)
    {
        for (i = 0; i < ARRAY_SIZE(p_tmp->Credential.CredentialAccessProfile); i++)
        {
            if (!p_tmp->Credential.CredentialAccessProfile[i].Used)
            {
                continue;
            }
            
            if (strcmp(p_tmp->Credential.CredentialAccessProfile[i].AccessProfileToken, p_req->AccessProfileToken[j]) == 0)
            {
                // delete the credential Access Profile
                
                p_tmp->Credential.sizeCredentialAccessProfile--;
                p_tmp->Credential.CredentialAccessProfile[i].Used = FALSE;

                // here add handler code ...

                // send event notify
                onvif_CredentialChangedNotify(p_tmp);
            }
        }
    }

    return ONVIF_OK;
}


#endif // end of  CREDENTAIL_SUPPORT


