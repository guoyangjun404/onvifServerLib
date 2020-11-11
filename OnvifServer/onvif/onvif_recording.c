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

#include "onvif.h"
#include "onvif_recording.h"
#include "xml_node.h"
#include "onvif_utils.h"
#include "onvif_event.h"
#include "onvif_pkt.h"

#ifdef PROFILE_G_SUPPORT

/***************************************************************************************/
extern ONVIF_CFG g_onvif_cfg;
extern ONVIF_CLS g_onvif_cls;


/***************************************************************************************/

void onvif_CreateTrackNotify(ONVIF_Recording * p_recording, ONVIF_Track * p_track)
{
    ONVIF_SimpleItem * p_simpleitem;
	ONVIF_NotificationMessage * p_message;
	
    p_message = onvif_add_NotificationMessage(NULL);
	if (p_message)
	{
		strcpy(p_message->NotificationMessage.Dialect, "http://www.onvif.org/ver10/tev/topicExpression/ConcreteSet");
		strcpy(p_message->NotificationMessage.Topic, "tns1:RecordingConfig/CreateTrack");
		p_message->NotificationMessage.Message.PropertyOperationFlag = 1;
		p_message->NotificationMessage.Message.PropertyOperation = PropertyOperation_Initialized;
		p_message->NotificationMessage.Message.UtcTime = time(NULL)+1;

        p_message->NotificationMessage.Message.SourceFlag = 1;
        
		p_simpleitem = onvif_add_SimpleItem(&p_message->NotificationMessage.Message.Source.SimpleItem);
		if (p_simpleitem)
		{
			strcpy(p_simpleitem->SimpleItem.Name, "RecordingToken");
			strcpy(p_simpleitem->SimpleItem.Value, p_recording->Recording.RecordingToken);
		}

		p_simpleitem = onvif_add_SimpleItem(&p_message->NotificationMessage.Message.Source.SimpleItem);
		if (p_simpleitem)
		{
			strcpy(p_simpleitem->SimpleItem.Name, "TrackToken");
			strcpy(p_simpleitem->SimpleItem.Value, p_track->Track.TrackToken);
		}

		onvif_put_NotificationMessage(p_message);
	}
}

void onvif_DeleteTrackNotify(ONVIF_Recording * p_recording, ONVIF_Track * p_track)
{
    ONVIF_SimpleItem * p_simpleitem;
	ONVIF_NotificationMessage * p_message;
	
    p_message = onvif_add_NotificationMessage(NULL);
	if (p_message)
	{
		strcpy(p_message->NotificationMessage.Dialect, "http://www.onvif.org/ver10/tev/topicExpression/ConcreteSet");
		strcpy(p_message->NotificationMessage.Topic, "tns1:RecordingConfig/DeleteTrack");
		p_message->NotificationMessage.Message.PropertyOperationFlag = 1;
		p_message->NotificationMessage.Message.PropertyOperation = PropertyOperation_Deleted;
		p_message->NotificationMessage.Message.UtcTime = time(NULL)+1;

        p_message->NotificationMessage.Message.SourceFlag = 1;
        
		p_simpleitem = onvif_add_SimpleItem(&p_message->NotificationMessage.Message.Source.SimpleItem);
		if (p_simpleitem)
		{
			strcpy(p_simpleitem->SimpleItem.Name, "RecordingToken");
			strcpy(p_simpleitem->SimpleItem.Value, p_recording->Recording.RecordingToken);
		}

		p_simpleitem = onvif_add_SimpleItem(&p_message->NotificationMessage.Message.Source.SimpleItem);
		if (p_simpleitem)
		{
			strcpy(p_simpleitem->SimpleItem.Name, "TrackToken");
			strcpy(p_simpleitem->SimpleItem.Value, p_track->Track.TrackToken);
		}

		onvif_put_NotificationMessage(p_message);
	}
}

void onvif_CreateRecordingNotify(ONVIF_Recording * p_recording)
{
    ONVIF_SimpleItem * p_simpleitem;
	ONVIF_NotificationMessage * p_message;

	p_message = onvif_add_NotificationMessage(NULL);
	if (p_message)
	{
		strcpy(p_message->NotificationMessage.Dialect, "http://www.onvif.org/ver10/tev/topicExpression/ConcreteSet");
		strcpy(p_message->NotificationMessage.Topic, "tns1:RecordingConfig/CreateRecording");
		p_message->NotificationMessage.Message.PropertyOperationFlag = 1;
		p_message->NotificationMessage.Message.PropertyOperation = PropertyOperation_Initialized;
		p_message->NotificationMessage.Message.UtcTime = time(NULL)+1;

        p_message->NotificationMessage.Message.SourceFlag = 1;
        
		p_simpleitem = onvif_add_SimpleItem(&p_message->NotificationMessage.Message.Source.SimpleItem);
		if (p_simpleitem)
		{
			strcpy(p_simpleitem->SimpleItem.Name, "RecordingToken");
			strcpy(p_simpleitem->SimpleItem.Value, p_recording->Recording.RecordingToken);
		}

		onvif_put_NotificationMessage(p_message);
	}
}

void onvif_DeleteRecordingNotify(ONVIF_Recording * p_recording)
{
    ONVIF_SimpleItem * p_simpleitem;
	ONVIF_NotificationMessage * p_message;

	p_message = onvif_add_NotificationMessage(NULL);
	if (p_message)
	{
		strcpy(p_message->NotificationMessage.Dialect, "http://www.onvif.org/ver10/tev/topicExpression/ConcreteSet");
		strcpy(p_message->NotificationMessage.Topic, "tns1:RecordingConfig/DeleteRecording");
		p_message->NotificationMessage.Message.PropertyOperationFlag = 1;
		p_message->NotificationMessage.Message.PropertyOperation = PropertyOperation_Deleted;
		p_message->NotificationMessage.Message.UtcTime = time(NULL)+1;

        p_message->NotificationMessage.Message.SourceFlag = 1;
        
		p_simpleitem = onvif_add_SimpleItem(&p_message->NotificationMessage.Message.Source.SimpleItem);
		if (p_simpleitem)
		{
			strcpy(p_simpleitem->SimpleItem.Name, "RecordingToken");
			strcpy(p_simpleitem->SimpleItem.Value, p_recording->Recording.RecordingToken);
		}

		onvif_put_NotificationMessage(p_message);
	}	
}


/**
 The possible return values:
 	ONVIF_ERR_BadConfiguration,
	ONVIF_ERR_MaxRecordings,
 */
ONVIF_RET onvif_CreateRecording(CreateRecording_REQ * p_req)
{
	ONVIF_Track * p_track;	
	ONVIF_Recording * p_recording = onvif_add_Recording();
	if (NULL == p_recording)
	{
		return ONVIF_ERR_MaxRecordings;
	}

	memcpy(&p_recording->Recording.Configuration, &p_req->RecordingConfiguration, sizeof(onvif_RecordingConfiguration));
	
	strcpy(p_req->RecordingToken, p_recording->Recording.RecordingToken);

	// send CreateRecording event
    onvif_CreateRecordingNotify(p_recording);
    
	// create three tracks
	p_track = onvif_add_Track(&p_recording->Recording.Tracks);
	if (p_track)
	{
		strcpy(p_track->Track.TrackToken, "VIDEO001");
		p_track->Track.Configuration.TrackType = TrackType_Video;

        // send CreateTrack event
		onvif_CreateTrackNotify(p_recording, p_track);
	}	
	
	p_track = onvif_add_Track(&p_recording->Recording.Tracks);
	if (p_track)
	{
		strcpy(p_track->Track.TrackToken, "AUDIO001");
		p_track->Track.Configuration.TrackType = TrackType_Audio;

		// send CreateTrack event
		onvif_CreateTrackNotify(p_recording, p_track);
	}
	
	p_track = onvif_add_Track(&p_recording->Recording.Tracks);
	if (p_track)
	{
		strcpy(p_track->Track.TrackToken, "META001");
		p_track->Track.Configuration.TrackType = TrackType_Metadata;

		// send CreateTrack event
		onvif_CreateTrackNotify(p_recording, p_track);
	}
	
	return ONVIF_OK;
}

/**
 The possible return values:
 	ONVIF_ERR_NoRecording,
	ONVIF_ERR_CannotDelete,
 */
ONVIF_RET onvif_DeleteRecording(const char * p_RecordingToken)
{
	ONVIF_Track	* p_track;	
	ONVIF_Recording * p_recording = onvif_find_Recording(p_RecordingToken);
	if (NULL == p_recording)
	{
		return ONVIF_ERR_NoRecording;
	}

    // todo : add delete recording code ...

    p_track = p_recording->Recording.Tracks;
    while (p_track)
    {
        // send DeleteTrack event
    	onvif_DeleteTrackNotify(p_recording, p_track);

    	p_track = p_track->next;
	}

    // send DeleteRecording event
    onvif_DeleteRecordingNotify(p_recording);	

	onvif_free_Recording(p_recording);
	
	return ONVIF_OK;
}

/**
 The possible return values:
 	ONVIF_ERR_BadConfiguration,
	ONVIF_ERR_NoRecording,
 */
ONVIF_RET onvif_SetRecordingConfiguration(SetRecordingConfiguration_REQ * p_req)
{
    ONVIF_SimpleItem * p_simpleitem;
    ONVIF_ElementItem * p_elementitem;
	ONVIF_NotificationMessage * p_message;
	
	ONVIF_Recording * p_recording = onvif_find_Recording(p_req->RecordingToken);
	if (NULL == p_recording)
	{
		return ONVIF_ERR_NoRecording;
	}

	// todo : add set recording configuration code ...

	memcpy(&p_recording->Recording.Configuration, &p_req->RecordingConfiguration, sizeof(onvif_RecordingConfiguration));

    // send RecordingConfiguration event
    
	p_message = onvif_add_NotificationMessage(NULL);
	if (p_message)
	{
		strcpy(p_message->NotificationMessage.Dialect, "http://www.onvif.org/ver10/tev/topicExpression/ConcreteSet");
		strcpy(p_message->NotificationMessage.Topic, "tns1:RecordingConfig/RecordingConfiguration");
		p_message->NotificationMessage.Message.PropertyOperationFlag = 1;
		p_message->NotificationMessage.Message.PropertyOperation = PropertyOperation_Changed;
		p_message->NotificationMessage.Message.UtcTime = time(NULL)+1;

        p_message->NotificationMessage.Message.SourceFlag = 1;
        
		p_simpleitem = onvif_add_SimpleItem(&p_message->NotificationMessage.Message.Source.SimpleItem);
		if (p_simpleitem)
		{
			strcpy(p_simpleitem->SimpleItem.Name, "RecordingToken");
			strcpy(p_simpleitem->SimpleItem.Value, p_recording->Recording.RecordingToken);
		}

		p_message->NotificationMessage.Message.DataFlag = 1;
		
		p_elementitem = onvif_add_ElementItem(&p_message->NotificationMessage.Message.Data.ElementItem);
		if (p_elementitem)
		{
		    strcpy(p_elementitem->ElementItem.Name, "Configuration");
		    
		    p_elementitem->ElementItem.Any = (char *)malloc(2048);
		    if (p_elementitem->ElementItem.Any)
		    {
		        int offset = 0;
		        int buflen = 2048;
		        
		        memset(p_elementitem->ElementItem.Any, 0, buflen);
		        
		        p_elementitem->ElementItem.AnyFlag = 1;
		        
	            offset += snprintf(p_elementitem->ElementItem.Any+offset, buflen-offset, "<tt:RecordingConfiguration>");
	            offset += build_RecordingConfiguration_xml(p_elementitem->ElementItem.Any+offset, buflen-offset, &p_recording->Recording.Configuration);
	            offset += snprintf(p_elementitem->ElementItem.Any+offset, buflen-offset, "</tt:RecordingConfiguration>");
		    }
		}

		onvif_put_NotificationMessage(p_message);
	}
	
	return ONVIF_OK;
}

/**
 The possible return values:
 	ONVIF_ERR_BadConfiguration,
	ONVIF_ERR_NoRecording,
	ONVIF_ERR_MaxTracks
 */
ONVIF_RET onvif_CreateTrack(CreateTrack_REQ * p_req)
{
	ONVIF_Track * p_track;	
	ONVIF_Recording * p_recording = onvif_find_Recording(p_req->RecordingToken);
	if (NULL == p_recording)
	{
		return ONVIF_ERR_NoRecording;
	}
	
	p_track = onvif_add_Track(&p_recording->Recording.Tracks);
	if (NULL == p_track)
	{
		return ONVIF_ERR_MaxTracks;
	}

	memcpy(&p_track->Track.Configuration, &p_req->TrackConfiguration, sizeof(onvif_TrackConfiguration));
	strcpy(p_req->TrackToken, p_track->Track.TrackToken);

	// send CreateTrack event    
	onvif_CreateTrackNotify(p_recording, p_track);
	
	return ONVIF_OK;
}

/**
 The possible return values:
	ONVIF_ERR_NoRecording,
	ONVIF_ERR_NoTrack,
	ONVIF_ERR_CannotDelete
 */
ONVIF_RET onvif_DeleteTrack(DeleteTrack_REQ * p_req)
{
	ONVIF_Track * p_track;	
	ONVIF_Recording * p_recording = onvif_find_Recording(p_req->RecordingToken);
	if (NULL == p_recording)
	{
		return ONVIF_ERR_NoRecording;
	}

	p_track = onvif_find_Track(p_recording->Recording.Tracks, p_req->TrackToken);
	if (NULL == p_track)
	{
		return ONVIF_ERR_NoTrack;
	}

	// todo : add delete track code ...


    // send DeleteTrack event    
	onvif_DeleteTrackNotify(p_recording, p_track);
	
	onvif_free_Track(&p_recording->Recording.Tracks, p_track);
	
	return ONVIF_OK;
}

/**
 The possible return values:
	ONVIF_ERR_NoRecording,
	ONVIF_ERR_NoTrack,
	ONVIF_ERR_BadConfiguration
 */
ONVIF_RET onvif_SetTrackConfiguration(SetTrackConfiguration_REQ * p_req)
{
    ONVIF_SimpleItem * p_simpleitem;
    ONVIF_ElementItem * p_elementitem;
	ONVIF_NotificationMessage * p_message;
	
	ONVIF_Track * p_track;
	ONVIF_Recording * p_recording = onvif_find_Recording(p_req->RecordingToken);
	if (NULL == p_recording)
	{
		return ONVIF_ERR_NoRecording;
	}

	p_track = onvif_find_Track(p_recording->Recording.Tracks, p_req->TrackToken);
	if (NULL == p_track)
	{
		return ONVIF_ERR_NoTrack;
	}

	// todo : add set track configuration code ...

	memcpy(&p_track->Track.Configuration, &p_req->TrackConfiguration, sizeof(onvif_TrackConfiguration));

    // send TrackConfiguration event
    
	p_message = onvif_add_NotificationMessage(NULL);
	if (p_message)
	{
		strcpy(p_message->NotificationMessage.Dialect, "http://www.onvif.org/ver10/tev/topicExpression/ConcreteSet");
		strcpy(p_message->NotificationMessage.Topic, "tns1:RecordingConfig/TrackConfiguration");
		p_message->NotificationMessage.Message.PropertyOperationFlag = 1;
		p_message->NotificationMessage.Message.PropertyOperation = PropertyOperation_Changed;
		p_message->NotificationMessage.Message.UtcTime = time(NULL)+1;

        p_message->NotificationMessage.Message.SourceFlag = 1;
        
		p_simpleitem = onvif_add_SimpleItem(&p_message->NotificationMessage.Message.Source.SimpleItem);
		if (p_simpleitem)
		{
			strcpy(p_simpleitem->SimpleItem.Name, "RecordingToken");
			strcpy(p_simpleitem->SimpleItem.Value, p_recording->Recording.RecordingToken);
		}

		p_simpleitem = onvif_add_SimpleItem(&p_message->NotificationMessage.Message.Source.SimpleItem);
		if (p_simpleitem)
		{
			strcpy(p_simpleitem->SimpleItem.Name, "TrackToken");
			strcpy(p_simpleitem->SimpleItem.Value, p_track->Track.TrackToken);
		}

		p_message->NotificationMessage.Message.DataFlag = 1;
		
		p_elementitem = onvif_add_ElementItem(&p_message->NotificationMessage.Message.Data.ElementItem);
		if (p_elementitem)
		{
		    strcpy(p_elementitem->ElementItem.Name, "Configuration");
		    
		    p_elementitem->ElementItem.Any = (char *)malloc(2048);
		    if (p_elementitem->ElementItem.Any)
		    {
		        int offset = 0;
		        int buflen = 2048;
		        
		        memset(p_elementitem->ElementItem.Any, 0, buflen);
		        
		        p_elementitem->ElementItem.AnyFlag = 1;
		        
	            offset += snprintf(p_elementitem->ElementItem.Any+offset, buflen-offset, "<tt:TrackConfiguration>");
	            offset += build_TrackConfiguration_xml(p_elementitem->ElementItem.Any+offset, buflen-offset, &p_track->Track.Configuration);
	            offset += snprintf(p_elementitem->ElementItem.Any+offset, buflen-offset, "</tt:TrackConfiguration>");
		    }
		}

		onvif_put_NotificationMessage(p_message);
	}
	
	return ONVIF_OK;
}

void onvif_RecordingJobStateNotify(ONVIF_RecordingJob * p_recordingjob)
{
    ONVIF_SimpleItem * p_simpleitem;
	ONVIF_ElementItem * p_elementitem;
	ONVIF_NotificationMessage * p_message;
	
    p_message = onvif_add_NotificationMessage(NULL);
	if (p_message)
	{
		strcpy(p_message->NotificationMessage.Dialect, "http://www.onvif.org/ver10/tev/topicExpression/ConcreteSet");
		strcpy(p_message->NotificationMessage.Topic, "tns1:RecordingConfig/JobState");
		p_message->NotificationMessage.Message.PropertyOperationFlag = 1;
		p_message->NotificationMessage.Message.PropertyOperation = PropertyOperation_Changed;
		p_message->NotificationMessage.Message.UtcTime = time(NULL)+1;

        p_message->NotificationMessage.Message.SourceFlag = 1;
        
		p_simpleitem = onvif_add_SimpleItem(&p_message->NotificationMessage.Message.Source.SimpleItem);
		if (p_simpleitem)
		{		    
			strcpy(p_simpleitem->SimpleItem.Name, "RecordingJobToken");
			strcpy(p_simpleitem->SimpleItem.Value, p_recordingjob->RecordingJob.JobToken);
		}

        p_message->NotificationMessage.Message.DataFlag = 1;
        
		p_simpleitem = onvif_add_SimpleItem(&p_message->NotificationMessage.Message.Data.SimpleItem);
		if (p_simpleitem)
		{		    
			strcpy(p_simpleitem->SimpleItem.Name, "State");
			strcpy(p_simpleitem->SimpleItem.Value, p_recordingjob->RecordingJob.JobConfiguration.Mode);
		}
		
		p_elementitem = onvif_add_ElementItem(&p_message->NotificationMessage.Message.Data.ElementItem);
		if (p_elementitem)
		{
		    strcpy(p_elementitem->ElementItem.Name, "Information");
		    
		    p_elementitem->ElementItem.Any = (char *)malloc(1024);
		    if (p_elementitem->ElementItem.Any)
		    {
				onvif_RecordingJobStateInformation res;
				memset(&res, 0, sizeof(res));

		        memset(p_elementitem->ElementItem.Any, 0, 1024);
		        
		        p_elementitem->ElementItem.AnyFlag = 1;                
                
		        if (ONVIF_OK == onvif_GetRecordingJobState(p_recordingjob->RecordingJob.JobToken, &res))
		        {
		            int offset = 0;
		            offset += snprintf(p_elementitem->ElementItem.Any+offset, 1024-offset, "<tt:RecordingJobStateInformation>");
		            offset += build_RecordingJobStateInformation_xml(p_elementitem->ElementItem.Any+offset, 1024-offset, &res);
		            offset += snprintf(p_elementitem->ElementItem.Any+offset, 1024-offset, "</tt:RecordingJobStateInformation>");
		        }
		    }
		}

		onvif_put_NotificationMessage(p_message);
	}
}

/**
 The possible return values:
	ONVIF_ERR_MaxRecordingJobs,
	ONVIF_ERR_BadConfiguration,
	ONVIF_ERR_MaxReceivers
	ONVIF_ERR_NoRecording
 */
ONVIF_RET onvif_CreateRecordingJob(CreateRecordingJob_REQ  * p_req)
{
	int i = 0;
	ONVIF_Recording * p_recording;	
	ONVIF_RecordingJob * p_tmp;
	ONVIF_RecordingJob * p_recordingjob;

	p_recording = onvif_find_Recording(p_req->JobConfiguration.RecordingToken);
	if (NULL == p_recording)
	{
		return ONVIF_ERR_NoRecording;
	}
	
	p_recordingjob = onvif_add_RecordingJob();
	if (NULL == p_recordingjob)
	{
		return ONVIF_ERR_MaxRecordingJobs;
	}

    memcpy(&p_recordingjob->RecordingJob.JobConfiguration, &p_req->JobConfiguration, sizeof(onvif_RecordingJobConfiguration));
	
	strcpy(p_req->JobToken, p_recordingjob->RecordingJob.JobToken);
	
	// auto create recording source
	for (i = 0; i < p_req->JobConfiguration.sizeSource; i++)
	{
		if (p_req->JobConfiguration.Source[i].AutoCreateReceiverFlag && p_req->JobConfiguration.Source[i].AutoCreateReceiver)
		{
			p_req->JobConfiguration.Source[i].SourceTokenFlag = 1;
			p_req->JobConfiguration.Source[i].SourceToken.TypeFlag = 1;
			strcpy(p_req->JobConfiguration.Source[i].SourceToken.Type, "http://www.onvif.org/ver10/schema/Profile");

			if (g_onvif_cfg.profiles)
			{
				strcpy(p_req->JobConfiguration.Source[i].SourceToken.Token, g_onvif_cfg.profiles->token);
			}
		}
		else if (p_req->JobConfiguration.Source[i].SourceTokenFlag)
		{
			if (strcmp(p_req->JobConfiguration.Source[i].SourceToken.Type, "http://www.onvif.org/ver10/schema/Profile") == 0)
			{
				if (p_req->JobConfiguration.Source[i].SourceToken.Token[0] == '\0')
				{
					if (g_onvif_cfg.profiles)
					{
						strcpy(p_req->JobConfiguration.Source[i].SourceToken.Token, g_onvif_cfg.profiles->token);
					}
				}
			}
		}
	}

    // Adjust the status according to priority
    if (strcmp(p_recordingjob->RecordingJob.JobConfiguration.Mode, "Active") == 0)
    {
        p_tmp = g_onvif_cfg.recording_jobs;
        while (p_tmp)
        {
            if (p_tmp != p_recordingjob)
            {
                if (strcmp(p_tmp->RecordingJob.JobConfiguration.Mode, "Active") == 0)
                {
                    if (p_tmp->RecordingJob.JobConfiguration.Priority <= p_recordingjob->RecordingJob.JobConfiguration.Priority)
                    {
                        strcpy(p_tmp->RecordingJob.JobConfiguration.Mode, "Idle");

                        onvif_RecordingJobStateNotify(p_tmp);
                    }
                    else
                    {
                        strcpy(p_recordingjob->RecordingJob.JobConfiguration.Mode, "Idle");
                    }
                }
            }
            
            p_tmp = p_tmp->next;
        }
    }

	// if job state changed, send job state event notify
	onvif_RecordingJobStateNotify(p_recordingjob);
	
	return ONVIF_OK;
}

/**
 The possible return values:
	ONVIF_ERR_NoRecordingJob
 */
ONVIF_RET onvif_DeleteRecordingJob(const char * p_JobToken)
{
    ONVIF_RecordingJob * p_highPriority = NULL;
	ONVIF_RecordingJob * p_recordingjob = onvif_find_RecordingJob(p_JobToken);
	if (NULL == p_recordingjob)
	{
		return ONVIF_ERR_NoRecordingJob;
	}

	// todo : add delete recording job code ...


    // Adjust the status according to priority
    if (strcmp(p_recordingjob->RecordingJob.JobConfiguration.Mode, "Active") == 0)
    {
        ONVIF_RecordingJob * p_tmp = g_onvif_cfg.recording_jobs;
        while (p_tmp)
        {
            if (p_tmp != p_recordingjob)
            {
                if (NULL == p_highPriority)
                {
                    p_highPriority = p_tmp;
                }
                else if (p_highPriority->RecordingJob.JobConfiguration.Priority <= p_tmp->RecordingJob.JobConfiguration.Priority)
                {
                    p_highPriority = p_tmp;
                }
            }
            
            p_tmp = p_tmp->next;
        }

        if (p_highPriority)
        {
            strcpy(p_highPriority->RecordingJob.JobConfiguration.Mode, "Active");
            
            onvif_RecordingJobStateNotify(p_highPriority);
        }
    }

	onvif_free_RecordingJob(p_recordingjob);
	
	return ONVIF_OK;
}

/**
 The possible return values:
	ONVIF_ERR_NoRecordingJob
	ONVIF_ERR_BadConfiguration
	ONVIF_ERR_MaxReceivers
 */
ONVIF_RET onvif_SetRecordingJobConfiguration(SetRecordingJobConfiguration_REQ * p_req)
{
	int i;
	ONVIF_SimpleItem * p_simpleitem;
    ONVIF_ElementItem * p_elementitem;
	ONVIF_NotificationMessage * p_message;
	
	ONVIF_Recording * p_recording;
	ONVIF_RecordingJob * p_recordingjob = onvif_find_RecordingJob(p_req->JobToken);
	if (NULL == p_recordingjob)
	{
		return ONVIF_ERR_NoRecordingJob;
	}

	p_recording = onvif_find_Recording(p_req->JobConfiguration.RecordingToken);
	if (NULL == p_recording)
	{
		return ONVIF_ERR_BadConfiguration;
	}

	// auto create recording source
	for (i = 0; i < p_req->JobConfiguration.sizeSource; i++)
	{
		if (p_req->JobConfiguration.Source[i].AutoCreateReceiverFlag && p_req->JobConfiguration.Source[i].AutoCreateReceiver)
		{
			p_req->JobConfiguration.Source[i].SourceTokenFlag = 1;
			p_req->JobConfiguration.Source[i].SourceToken.TypeFlag = 1;
			strcpy(p_req->JobConfiguration.Source[i].SourceToken.Type, "http://www.onvif.org/ver10/schema/Profile");

			if (g_onvif_cfg.profiles)
			{
				strcpy(p_req->JobConfiguration.Source[i].SourceToken.Token, g_onvif_cfg.profiles->token);
			}
		}
	}
	
	memcpy(&p_recordingjob->RecordingJob.JobConfiguration, &p_req->JobConfiguration, sizeof(onvif_RecordingJobConfiguration));

	// send RecordingJobConfiguration event
    
	p_message = onvif_add_NotificationMessage(NULL);
	if (p_message)
	{
		strcpy(p_message->NotificationMessage.Dialect, "http://www.onvif.org/ver10/tev/topicExpression/ConcreteSet");
		strcpy(p_message->NotificationMessage.Topic, "tns1:RecordingConfig/RecordingJobConfiguration");
		p_message->NotificationMessage.Message.PropertyOperationFlag = 1;
		p_message->NotificationMessage.Message.PropertyOperation = PropertyOperation_Changed;
		p_message->NotificationMessage.Message.UtcTime = time(NULL)+1;

        p_message->NotificationMessage.Message.SourceFlag = 1;
        
		p_simpleitem = onvif_add_SimpleItem(&p_message->NotificationMessage.Message.Source.SimpleItem);
		if (p_simpleitem)
		{
			strcpy(p_simpleitem->SimpleItem.Name, "RecordingJobToken");
			strcpy(p_simpleitem->SimpleItem.Value, p_recordingjob->RecordingJob.JobToken);
		}

		p_message->NotificationMessage.Message.DataFlag = 1;
		
		p_elementitem = onvif_add_ElementItem(&p_message->NotificationMessage.Message.Data.ElementItem);
		if (p_elementitem)
		{
		    strcpy(p_elementitem->ElementItem.Name, "Configuration");
		    
		    p_elementitem->ElementItem.Any = (char *)malloc(2048);
		    if (p_elementitem->ElementItem.Any)
		    {
		        int offset = 0;
		        int buflen = 2048;
		        
		        memset(p_elementitem->ElementItem.Any, 0, buflen);
		        
		        p_elementitem->ElementItem.AnyFlag = 1;
		        
	            offset += snprintf(p_elementitem->ElementItem.Any+offset, buflen-offset, "<tt:RecordingJobConfiguration>");
	            offset += build_RecordingJobConfiguration_xml(p_elementitem->ElementItem.Any+offset, buflen-offset, 
	                        &p_recordingjob->RecordingJob.JobConfiguration);
	            offset += snprintf(p_elementitem->ElementItem.Any+offset, buflen-offset, "</tt:RecordingJobConfiguration>");
		    }
		}

		onvif_put_NotificationMessage(p_message);
	}
	
	return ONVIF_OK;
}

/**
 The possible return values:
	ONVIF_ERR_NoRecordingJob
	ONVIF_ERR_BadMode
 */
ONVIF_RET onvif_SetRecordingJobMode(SetRecordingJobMode_REQ * p_req)
{	
	ONVIF_RecordingJob * p_recordingjob = onvif_find_RecordingJob(p_req->JobToken);
	if (NULL == p_recordingjob)
	{
		return ONVIF_ERR_NoRecordingJob;
	}

	if (strcmp(p_req->Mode, "Idle") && strcmp(p_req->Mode, "Active"))
	{
		return ONVIF_ERR_BadMode;
	}

    if (strcmp(p_req->Mode, p_recordingjob->RecordingJob.JobConfiguration.Mode) == 0)
    {
        return ONVIF_OK;
    }

	// todo : add set recording job mode code ...

	strcpy(p_recordingjob->RecordingJob.JobConfiguration.Mode, p_req->Mode);

    // if job state changed, send job state event notify
	onvif_RecordingJobStateNotify(p_recordingjob);
	
	return ONVIF_OK;
}

/**
 The possible return values:
	ONVIF_ERR_NoRecordingJob
 */
ONVIF_RET onvif_GetRecordingJobState(const char * p_JobToken, onvif_RecordingJobStateInformation * p_res)
{
	int i, j;
	ONVIF_RecordingJob * p_recordingjob = onvif_find_RecordingJob(p_JobToken);
	if (NULL == p_recordingjob)
	{
		return ONVIF_ERR_NoRecordingJob;
	}

	strcpy(p_res->RecordingToken, p_recordingjob->RecordingJob.JobConfiguration.RecordingToken);
	strcpy(p_res->State, p_recordingjob->RecordingJob.JobConfiguration.Mode);

	p_res->sizeSources = p_recordingjob->RecordingJob.JobConfiguration.sizeSource;
	
	for (i = 0; i < p_recordingjob->RecordingJob.JobConfiguration.sizeSource; i++)
	{
		p_res->Sources[i].SourceToken.TypeFlag = 1;
		strcpy(p_res->Sources[i].SourceToken.Token, p_recordingjob->RecordingJob.JobConfiguration.Source[i].SourceToken.Token);
		strcpy(p_res->Sources[i].SourceToken.Type, p_recordingjob->RecordingJob.JobConfiguration.Source[i].SourceToken.Type);

		strcpy(p_res->Sources[i].State, p_recordingjob->RecordingJob.JobConfiguration.Mode);

		p_res->Sources[i].sizeTrack = p_recordingjob->RecordingJob.JobConfiguration.Source[i].sizeTracks;
		
		for (j = 0; j < p_recordingjob->RecordingJob.JobConfiguration.Source[i].sizeTracks; j++)
		{
			strcpy(p_res->Sources[i].Track[j].SourceTag, p_recordingjob->RecordingJob.JobConfiguration.Source[i].Tracks[j].SourceTag);
			strcpy(p_res->Sources[i].Track[j].Destination, p_recordingjob->RecordingJob.JobConfiguration.Source[i].Tracks[j].Destination);
			strcpy(p_res->Sources[i].Track[j].State, p_recordingjob->RecordingJob.JobConfiguration.Mode);
		}
	}
	
	return ONVIF_OK;
}

/**
 The possible return values:
	ONVIF_ERR_NoRecording
 */
ONVIF_RET onvif_GetRecordingOptions(const char * p_RecordingToken, onvif_RecordingOptions * p_res)
{
	int video, audio, meta;
	ONVIF_PROFILE * p_profile;
	ONVIF_Recording * p_recording = onvif_find_Recording(p_RecordingToken);
	if (NULL == p_recording)
	{
		return ONVIF_ERR_NoRecording;
	}

	video = onvif_get_track_nums_by_type(p_recording->Recording.Tracks, TrackType_Video);
	audio = onvif_get_track_nums_by_type(p_recording->Recording.Tracks, TrackType_Audio);
	meta  = onvif_get_track_nums_by_type(p_recording->Recording.Tracks, TrackType_Metadata);

	p_res->Track.SpareVideoFlag = 1;
	p_res->Track.SpareVideo = 2 - video;	// max support two video track
	p_res->Track.SpareAudioFlag = 1;
	p_res->Track.SpareAudio = 2 - audio;	// max support two audio track
	p_res->Track.SpareMetadataFlag = 1;
	p_res->Track.SpareMetadata = 1 - meta;	// max support one metadata track
	p_res->Track.SpareTotalFlag = 1;
	p_res->Track.SpareTotal = 2 - video + 2 - audio + 1 - meta;
	
	p_res->Job.SpareFlag = 1;
	p_res->Job.Spare = 2;
	p_res->Job.CompatibleSourcesFlag = 1;

	p_profile = g_onvif_cfg.profiles;
	while (p_profile)
	{
	    strcat(p_res->Job.CompatibleSources, p_profile->token);
	    strcat(p_res->Job.CompatibleSources, " ");

		p_profile = p_profile->next;
	}
	
	return ONVIF_OK;
}

ONVIF_RET onvif_GetRecordingSummary(GetRecordingSummary_RES * p_summary)
{
	// todo : modify the information ...
	p_summary->Summary.DataFrom = time(NULL);
	p_summary->Summary.DataUntil = time(NULL);
	p_summary->Summary.NumberRecordings = 0;

	return ONVIF_OK;
}

/**
 The possible return values:
	ONVIF_ERR_InvalidToken
 */
ONVIF_RET onvif_GetRecordingInformation(const char * p_RecordingToken, GetRecordingInformation_RES * p_res)
{
	ONVIF_Track * p_track;
	ONVIF_Recording * p_recording = onvif_find_Recording(p_RecordingToken);
	if (NULL == p_recording)
	{
		return ONVIF_ERR_InvalidToken;
	}

	// todo : fill the recording information ...
	strcpy(p_res->RecordingInformation.RecordingToken, p_recording->Recording.RecordingToken);
	memcpy(&p_res->RecordingInformation.Source, &p_recording->Recording.Configuration.Source, sizeof(onvif_RecordingSourceInformation));
	p_res->RecordingInformation.EarliestRecordingFlag = 1;
	p_res->RecordingInformation.EarliestRecording = time(NULL);
	p_res->RecordingInformation.LatestRecordingFlag = 1;
	p_res->RecordingInformation.LatestRecording = time(NULL);

	strcpy(p_res->RecordingInformation.Content, p_recording->Recording.Configuration.Content);

	p_track = p_recording->Recording.Tracks;
	while (p_track)
	{
		int index = p_res->RecordingInformation.sizeTrack;
		
		strcpy(p_res->RecordingInformation.Track[index].TrackToken, p_track->Track.TrackToken);
		p_res->RecordingInformation.Track[index].TrackType = p_track->Track.Configuration.TrackType;
		strcpy(p_res->RecordingInformation.Track[index].Description, p_track->Track.Configuration.Description);
		p_res->RecordingInformation.Track[index].DataFrom = time(NULL);
		p_res->RecordingInformation.Track[index].DataTo = time(NULL);

		p_track = p_track->next;
		p_res->RecordingInformation.sizeTrack++;

		if (p_res->RecordingInformation.sizeTrack >= 5)
		{
			break;
		}
	}

	p_res->RecordingInformation.RecordingStatus = RecordingStatus_Recording;
	
	return ONVIF_OK;
}

/**
 The possible return values:
	ONVIF_ERR_InvalidToken
	ONVIF_ERR_NoRecording
 */
ONVIF_RET onvif_GetMediaAttributes(GetMediaAttributes_REQ * p_req, GetMediaAttributes_RES * p_res)
{
	int i, j;	
	ONVIF_Track * p_track;
	ONVIF_Recording * p_recording;
	
	for (i = 0; i < p_req->sizeRecordingTokens; i++)
	{
		p_recording = onvif_find_Recording(p_req->RecordingTokens[i]);
		if (NULL == p_recording)
		{
			return ONVIF_ERR_NoRecording;
		}

		strcpy(p_res->MediaAttributes[i].RecordingToken, p_recording->Recording.RecordingToken);

		p_res->MediaAttributes[i].From = time(NULL);
		p_res->MediaAttributes[i].Until = time(NULL);

		p_track = p_recording->Recording.Tracks;
		while (p_track)
		{
			j = p_res->MediaAttributes[i].sizeTrackAttributes;
			
			strcpy(p_res->MediaAttributes[i].TrackAttributes[j].TrackInformation.TrackToken, p_track->Track.TrackToken);
			p_res->MediaAttributes[i].TrackAttributes[j].TrackInformation.TrackType = p_track->Track.Configuration.TrackType;
			strcpy(p_res->MediaAttributes[i].TrackAttributes[j].TrackInformation.Description, p_track->Track.Configuration.Description);
			p_res->MediaAttributes[i].TrackAttributes[j].TrackInformation.DataFrom = time(NULL);
			p_res->MediaAttributes[i].TrackAttributes[j].TrackInformation.DataTo = time(NULL);

			// todo : fill video, audio, metadata information ...
			
			p_track = p_track->next;

			p_res->MediaAttributes[i].sizeTrackAttributes++;
			if (p_res->MediaAttributes[i].sizeTrackAttributes >= 5)
			{
				break;
			}
		}
		
	}
	
	return ONVIF_OK;
}

/*****************************************************************************/
BOOL onvif_AddSearch(SUA * p_sua)
{
    BOOL ret = FALSE;
    
    if (NULL == g_onvif_cls.search_list)
    {
        g_onvif_cls.search_list = h_list_create(TRUE);
    }

    if (g_onvif_cls.search_list)
    {
        ret = h_list_add_at_back(g_onvif_cls.search_list, p_sua);
    }

    return ret;
}

SUA * onvif_FindSearch(const char * token)
{
    SUA * p_sua = NULL;
    SUA * p_tmp = NULL;
    
    LINKED_NODE * p_node = h_list_lookup_start(g_onvif_cls.search_list);
    while (p_node)
    {
        p_tmp = (SUA *)p_node->p_data;
        if (p_tmp && strcmp(p_tmp->token, token) == 0)
        {
            p_sua = p_tmp;
            break;
        }
        
        p_node = h_list_lookup_next(g_onvif_cls.search_list, p_node);
    }
    h_list_lookup_end(g_onvif_cls.search_list);

    return p_sua;
}

void onvif_FreeSearch(SUA * p_sua)
{
    if (p_sua->req)
    {
        free(p_sua->req);
    }
    
    h_list_remove_data(g_onvif_cls.search_list, p_sua);
}

void onvif_freeSearchs()
{
    SUA * p_sua = NULL;
    
    LINKED_NODE * p_node = h_list_lookup_start(g_onvif_cls.search_list);
    while (p_node)
    {
        p_sua = (SUA *)p_node->p_data;
        if (p_sua && p_sua->req)
        {
            free(p_sua->req);
        }
        
        p_node = h_list_lookup_next(g_onvif_cls.search_list, p_node);
    }
    h_list_lookup_end(g_onvif_cls.search_list);

    h_list_free_container(g_onvif_cls.search_list);
    g_onvif_cls.search_list = NULL;
}

/*****************************************************************************/

ONVIF_RET onvif_FindRecordings(FindRecordings_REQ * p_req, FindRecordings_RES * p_res)
{
    SUA * p_sua = (SUA *)malloc(sizeof(SUA));
    if (p_sua)
    {
        p_sua->req = malloc(sizeof(FindRecordings_REQ));
        if (p_sua->req)
        {
            memcpy(p_sua->req, p_req, sizeof(FindRecordings_REQ));
        }

        p_sua->type = SEARCH_TYPE_RECORDING;
        sprintf(p_sua->token, "SearchToken%d", g_onvif_cls.search_idx++);
        strcpy(p_res->SearchToken, p_sua->token);
        
        onvif_AddSearch(p_sua);
    }
    
	return ONVIF_OK;
}

ONVIF_RET onvif_GetRecordingSearchResults(GetRecordingSearchResults_REQ * p_req, GetRecordingSearchResults_RES * p_res)
{
    int n = 0;
    ONVIF_Recording * p_rec;
    ONVIF_RecordingInformation * p_recinf;
    SUA * p_sua = onvif_FindSearch(p_req->SearchToken);
    if (NULL == p_sua)
    {
        return ONVIF_ERR_InvalidToken;
    }
    
    p_res->ResultList.SearchState = SearchState_Completed;

    // the following code is just for example ...
    
    p_rec = g_onvif_cfg.recordings;
    while (p_rec)
    {
        if (p_req->MaxResultsFlag && n >= p_req->MaxResults)
        {
            break;
        }

        // add a recording to the result
        p_recinf = onvif_add_RecordingInformation(&p_res->ResultList.RecordInformation);
        if (p_recinf)
        {
            if (g_onvif_cfg.recordings)
            {
                strcpy(p_recinf->RecordingInformation.RecordingToken, p_rec->Recording.RecordingToken);
                memcpy(&p_recinf->RecordingInformation.Source, &p_rec->Recording.Configuration.Source, sizeof(onvif_RecordingSourceInformation));
                strcpy(p_recinf->RecordingInformation.Content, p_rec->Recording.Configuration.Content);
                p_recinf->RecordingInformation.RecordingStatus = RecordingStatus_Stopped;
            }
            else
            {
                strcpy(p_recinf->RecordingInformation.RecordingToken, "RecordingToken");
                strcpy(p_recinf->RecordingInformation.Source.SourceId, "SourceId");
                strcpy(p_recinf->RecordingInformation.Source.Name, "Name");
                strcpy(p_recinf->RecordingInformation.Source.Location, "Location");
                strcpy(p_recinf->RecordingInformation.Source.Description, "Description");
                strcpy(p_recinf->RecordingInformation.Source.Address, "Address");
                strcpy(p_recinf->RecordingInformation.Content, "Content");
                p_recinf->RecordingInformation.RecordingStatus = RecordingStatus_Stopped;
            }
        }

        p_rec = p_rec->next;
    }
    
	return ONVIF_OK;
}

ONVIF_RET onvif_FindEvents(FindEvents_REQ * p_req, FindEvents_RES * p_res)
{
    SUA * p_sua = (SUA *)malloc(sizeof(SUA));
    if (p_sua)
    {
        p_sua->req = malloc(sizeof(FindEvents_REQ));
        if (p_sua->req)
        {
            memcpy(p_sua->req, p_req, sizeof(FindEvents_REQ));
        }

        p_sua->type = SEARCH_TYPE_EVENTS;
        sprintf(p_sua->token, "SearchToken%d", g_onvif_cls.search_idx++);
        strcpy(p_res->SearchToken, p_sua->token);
        
        onvif_AddSearch(p_sua);
    }
    
	return ONVIF_OK;
}

ONVIF_RET onvif_GetEventSearchResults(GetEventSearchResults_REQ * p_req, GetEventSearchResults_RES * p_res)
{
    SUA * p_sua = onvif_FindSearch(p_req->SearchToken);
    if (NULL == p_sua)
    {
        return ONVIF_ERR_InvalidToken;
    }

    // todo : fill the event result ...
    
    p_res->ResultList.SearchState = SearchState_Completed;
    
	return ONVIF_OK;
}

ONVIF_RET onvif_FindMetadata(FindMetadata_REQ * p_req, FindMetadata_RES * p_res)
{
    SUA * p_sua = (SUA *)malloc(sizeof(SUA));
    if (p_sua)
    {
        p_sua->req = malloc(sizeof(FindMetadata_REQ));
        if (p_sua->req)
        {
            memcpy(p_sua->req, p_req, sizeof(FindMetadata_REQ));
        }

        p_sua->type = SEARCH_TYPE_METADATA;
        sprintf(p_sua->token, "SearchToken%d", g_onvif_cls.search_idx++);
        strcpy(p_res->SearchToken, p_sua->token);
        
        onvif_AddSearch(p_sua);
    }
    
    return ONVIF_OK;
}

ONVIF_RET onvif_GetMetadataSearchResults(GetMetadataSearchResults_REQ * p_req, GetMetadataSearchResults_RES * p_res)
{
    SUA * p_sua = onvif_FindSearch(p_req->SearchToken);
    if (NULL == p_sua)
    {
        return ONVIF_ERR_InvalidToken;
    }

    // todo : fill metadata result ...
    
    p_res->ResultList.SearchState = SearchState_Completed;
    
	return ONVIF_OK;
}

ONVIF_RET onvif_FindPTZPosition(FindPTZPosition_REQ * p_req, FindPTZPosition_RES * p_res)
{
    SUA * p_sua = (SUA *)malloc(sizeof(SUA));
    if (p_sua)
    {
        p_sua->req = malloc(sizeof(FindPTZPosition_REQ));
        if (p_sua->req)
        {
            memcpy(p_sua->req, p_req, sizeof(FindPTZPosition_REQ));
        }

        p_sua->type = SEARCH_TYPE_PTZPOS;
        sprintf(p_sua->token, "SearchToken%d", g_onvif_cls.search_idx++);
        strcpy(p_res->SearchToken, p_sua->token);
        
        onvif_AddSearch(p_sua);
    }
    
    return ONVIF_OK;
}

ONVIF_RET onvif_GetPTZPositionSearchResults(GetPTZPositionSearchResults_REQ * p_req, GetPTZPositionSearchResults_RES * p_res)
{
    SUA * p_sua = onvif_FindSearch(p_req->SearchToken);
    if (NULL == p_sua)
    {
        return ONVIF_ERR_InvalidToken;
    }

    // todo : fill ptz result ...
    
    p_res->ResultList.SearchState = SearchState_Completed;
    
	return ONVIF_OK;
}

ONVIF_RET onvif_EndSearch(EndSearch_REQ * p_req, EndSearch_RES * p_res)
{
    SUA * p_sua = onvif_FindSearch(p_req->SearchToken);
    if (NULL == p_sua)
    {
        return ONVIF_ERR_InvalidToken;
    }

    onvif_FreeSearch(p_sua);
    
	return ONVIF_OK;
}

ONVIF_RET onvif_GetSearchState(GetSearchState_REQ * p_req, GetSearchState_RES * p_res)
{
    SUA * p_sua = onvif_FindSearch(p_req->SearchToken);
    if (NULL == p_sua)
    {
        return ONVIF_ERR_InvalidToken;
    }

	// todo : set the search state ...

	p_res->State = SearchState_Completed;
    
	return ONVIF_OK;
}

ONVIF_RET onvif_GetReplayUri(const char * lip, uint32 lport, GetReplayUri_REQ * p_req, GetReplayUri_RES * p_res)
{
    int offset = 0;
    int len = sizeof(p_res->Uri);
	    
	ONVIF_Recording * p_recording = onvif_find_Recording(p_req->RecordingToken);
	if (NULL == p_recording)
	{
		return ONVIF_ERR_NoRecording;
	}

	// todo : fill the replay uri ...

	if (p_req->StreamSetup.Transport.Protocol == TransportProtocol_HTTP)
    {
        offset += sprintf(p_res->Uri, "http://%s/test.mp4", lip);
    }
    else
    {
        offset += sprintf(p_res->Uri, "rtsp://%s/test.mp4", lip);
    }

	return ONVIF_OK;
}

ONVIF_RET onvif_GetReplayConfiguration(GetReplayConfiguration_RES * p_res)
{
	p_res->SessionTimeout = g_onvif_cfg.replay_session_timeout;

	return ONVIF_OK;
}

ONVIF_RET onvif_SetReplayConfiguration(SetReplayConfiguration_REQ * p_req)
{
    g_onvif_cfg.replay_session_timeout = p_req->SessionTimeout;
    
	return ONVIF_OK;
}


#endif // end of PROFILE_G_SUPPORT


