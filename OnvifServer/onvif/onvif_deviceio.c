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

#ifdef DEVICEIO_SUPPORT

#include "sys_inc.h"
#include "onvif_deviceio.h"
#include "onvif_event.h"

extern ONVIF_CFG g_onvif_cfg;

/***************************************************************************************/

void onvif_DigitalInputStateNotify(ONVIF_DigitalInput * p_req)
{
    ONVIF_NotificationMessage * p_message = onvif_add_NotificationMessage(NULL);
	if (p_message)
	{
	    ONVIF_SimpleItem * p_simpleitem;
	    
		strcpy(p_message->NotificationMessage.Dialect, "http://www.onvif.org/ver10/tev/topicExpression/ConcreteSet");
		strcpy(p_message->NotificationMessage.Topic, "tns1:Device/Trigger/DigitalInput");
		p_message->NotificationMessage.Message.PropertyOperationFlag = 1;
		p_message->NotificationMessage.Message.PropertyOperation = PropertyOperation_Changed;
		p_message->NotificationMessage.Message.UtcTime = time(NULL)+1;

        p_message->NotificationMessage.Message.SourceFlag = 1;
        
		p_simpleitem = onvif_add_SimpleItem(&p_message->NotificationMessage.Message.Source.SimpleItem);
		if (p_simpleitem)
		{		    
			strcpy(p_simpleitem->SimpleItem.Name, "InputToken");
			strcpy(p_simpleitem->SimpleItem.Value, p_req->DigitalInput.token);
		}

		p_message->NotificationMessage.Message.DataFlag = 1;

		p_simpleitem = onvif_add_SimpleItem(&p_message->NotificationMessage.Message.Data.SimpleItem);
		if (p_simpleitem)
		{		    
			strcpy(p_simpleitem->SimpleItem.Name, "LogicalState");
			// Digital Input LogicalState can be either set at "true" to represent the circuit in the closed state
            //    or set at "false" to represent the circuit in the open state
			strcpy(p_simpleitem->SimpleItem.Value, (p_req->DigitalInput.IdleState == DigitalIdleState_closed) ? "true" : "false");
		}

		onvif_put_NotificationMessage(p_message);
	}
}

void onvif_RelayOutputStateNotify(ONVIF_RelayOutput * p_req, int state)
{
    ONVIF_NotificationMessage * p_message = onvif_add_NotificationMessage(NULL);
	if (p_message)
	{
	    ONVIF_SimpleItem * p_simpleitem;
	    
		strcpy(p_message->NotificationMessage.Dialect, "http://www.onvif.org/ver10/tev/topicExpression/ConcreteSet");
		strcpy(p_message->NotificationMessage.Topic, "tns1:Device/Trigger/Relay");
		p_message->NotificationMessage.Message.PropertyOperationFlag = 1;
		p_message->NotificationMessage.Message.PropertyOperation = (onvif_PropertyOperation)state;
		p_message->NotificationMessage.Message.UtcTime = time(NULL)+1;

        p_message->NotificationMessage.Message.SourceFlag = 1;
        
		p_simpleitem = onvif_add_SimpleItem(&p_message->NotificationMessage.Message.Source.SimpleItem);
		if (p_simpleitem)
		{		    
			strcpy(p_simpleitem->SimpleItem.Name, "RelayToken");
			strcpy(p_simpleitem->SimpleItem.Value, p_req->RelayOutput.token);
		}

		p_message->NotificationMessage.Message.DataFlag = 1;

		p_simpleitem = onvif_add_SimpleItem(&p_message->NotificationMessage.Message.Data.SimpleItem);
		if (p_simpleitem)
		{		    
			strcpy(p_simpleitem->SimpleItem.Name, "LogicalState");
			strcpy(p_simpleitem->SimpleItem.Value, onvif_RelayLogicalStateToString(p_req->RelayLogicalState));
		}

		onvif_put_NotificationMessage(p_message);
	}
}

ONVIF_RET onvif_tmd_GetRelayOutputs()
{
    ONVIF_RelayOutput * p_output = g_onvif_cfg.relay_output;
    while (p_output)
    {
        onvif_RelayOutputStateNotify(p_output, PropertyOperation_Initialized);

        p_output = p_output->next;
    }

    return ONVIF_OK;
}

/**
 * possible retrun value:
 *
 *  ONVIF_ERR_NoVideoOutput
 *  ONVIF_ERR_ConfigModify
 */
ONVIF_RET onvif_SetVideoOutputConfiguration(SetVideoOutputConfiguration_REQ * p_req)
{
    ONVIF_VideoOutput * p_output;
    ONVIF_VideoOutputConfiguration * p_cfg = onvif_find_VideoOutputConfiguration(p_req->Configuration.token);
    if (NULL == p_cfg)
    {
        return ONVIF_ERR_NoVideoOutput;
    }

    p_output = onvif_find_VideoOutput(p_req->Configuration.OutputToken);
    if (NULL == p_output)
    {
        return ONVIF_ERR_NoVideoOutput;
    }

    // todo : here add process code ...

    
    strcpy(p_cfg->Configuration.Name, p_req->Configuration.Name);
    strcpy(p_cfg->Configuration.OutputToken, p_req->Configuration.OutputToken);
    
    return ONVIF_OK;
}

/**
 * possible retrun value:
 *
 *  ONVIF_ERR_NoAudioOutput
 *  ONVIF_ERR_ConfigModify
 */
ONVIF_RET onvif_SetAudioOutputConfiguration(SetAudioOutputConfiguration_REQ * p_req)
{
    ONVIF_AudioOutput * p_output;
    ONVIF_AudioOutputConfiguration * p_cfg = onvif_find_AudioOutputConfiguration(p_req->Configuration.token);
    if (NULL == p_cfg)
    {
        return ONVIF_ERR_NoAudioOutput;
    }

    p_output = onvif_find_AudioOutput(p_req->Configuration.OutputToken);
    if (NULL == p_output)
    {
        return ONVIF_ERR_NoAudioOutput;
    }

    if (p_req->Configuration.OutputLevel < p_cfg->Options.OutputLevelRange.Min || 
        p_req->Configuration.OutputLevel > p_cfg->Options.OutputLevelRange.Max)
    {
        return ONVIF_ERR_ConfigModify;
    }
    
    // todo : here add process code ...

    
    strcpy(p_cfg->Configuration.Name, p_req->Configuration.Name);
    strcpy(p_cfg->Configuration.OutputToken, p_req->Configuration.OutputToken);
    if (p_req->Configuration.SendPrimacyFlag)
    {
        strcpy(p_cfg->Configuration.SendPrimacy, p_req->Configuration.SendPrimacy);
    }
    p_cfg->Configuration.OutputLevel = p_req->Configuration.OutputLevel;

    return ONVIF_OK;
}

/**
 * possible retrun value:
 *
 *  ONVIF_ERR_RelayToken
 *  ONVIF_ERR_ModeError
 */
ONVIF_RET onvif_SetRelayOutputSettings(SetRelayOutputSettings_REQ * p_req)
{
    ONVIF_RelayOutput * p_output = onvif_find_RelayOutput(p_req->RelayOutput.token);
    if (NULL == p_output)
    {
        return ONVIF_ERR_RelayToken;
    }

    // todo : here add process code ...
    

    p_output->RelayOutput.Properties.Mode = p_req->RelayOutput.Properties.Mode;
    p_output->RelayOutput.Properties.DelayTime = p_req->RelayOutput.Properties.DelayTime;
    p_output->RelayOutput.Properties.IdleState = p_req->RelayOutput.Properties.IdleState;

    return ONVIF_OK;
}

ONVIF_RET onvif_SetRelayOutputState(SetRelayOutputState_REQ * p_req)
{
    ONVIF_RelayOutput * p_output = onvif_find_RelayOutput(p_req->RelayOutputToken);
    if (NULL == p_output)
    {
        return ONVIF_ERR_RelayToken;
    }

    // todo : here add process code ...


    if (p_output->RelayLogicalState != p_req->LogicalState)
    {
        p_output->RelayLogicalState = p_req->LogicalState;

        // send notify message    
        onvif_RelayOutputStateNotify(p_output, PropertyOperation_Changed);
    }
    
    return ONVIF_OK;
}

ONVIF_RET onvif_SetDigitalInputConfigurations(SetDigitalInputConfigurations_REQ * p_req)
{
    ONVIF_DigitalInput * p_input = p_req->DigitalInputs;

    // todo : here add process code ...

    
    while (p_input)
    {
        ONVIF_DigitalInput * p_tmp = onvif_find_DigitalInput(p_input->DigitalInput.token);
        if (p_tmp)
        {
            if (p_input->DigitalInput.IdleStateFlag)
            {
                if (p_tmp->DigitalInput.IdleState != p_input->DigitalInput.IdleState)
                {
                    p_tmp->DigitalInput.IdleState = p_input->DigitalInput.IdleState;

                    // send notify message    
                    onvif_DigitalInputStateNotify(p_tmp);
                }
            }
        }
        
        p_input = p_input->next;
    }

    return ONVIF_OK;
}

/**
 * possible retrun value:
 *
 *  ONVIF_ERR_ConfigModify
 *  ONVIF_ERR_InvalidSerialPort
 */
ONVIF_RET onvif_SetSerialPortConfiguration(SetSerialPortConfiguration_REQ * p_req)
{
    ONVIF_SerialPort * p_port = onvif_find_SerialPort_by_ConfigurationToken(p_req->SerialPortConfiguration.token);
    if (NULL == p_port)
    {
        return ONVIF_ERR_InvalidSerialPort;
    }

    // todo : here add process code ...


    memcpy(&p_port->Configuration, &p_req->SerialPortConfiguration, sizeof(onvif_SerialPortConfiguration));

    return ONVIF_OK;
}

/**
 * possible retrun value:
 *
 *  ONVIF_ERR_InvalidSerialPort
 *  ONVIF_ERR_DataLengthOver
 *  ONVIF_ERR_DelimiterNotSupported
 */
ONVIF_RET onvif_SendReceiveSerialCommandRx(SendReceiveSerialCommand_REQ * p_req, SendReceiveSerialCommand_RES * p_res)
{
    ONVIF_SerialPort * p_port = onvif_find_SerialPort_by_ConfigurationToken(p_req->token);
    if (NULL == p_port)
    {
        return ONVIF_ERR_InvalidSerialPort;
    }
    
    // todo : here add process code ...

    
    return ONVIF_OK;
}


#endif // DEVICEIO_SUPPORT

