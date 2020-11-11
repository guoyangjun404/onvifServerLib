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
#include "onvif_schedule.h"
#include "onvif.h"
#include "onvif_event.h"

#ifdef LIBICAL
#include "icalvcal.h"
#include "vcc.h"

// link to libical dynamic library
#pragma comment(lib, "libical.lib")
#pragma comment(lib, "libicalvcal.lib")
#endif

#ifdef SCHEDULE_SUPPORT

/********************************************************************************/
extern ONVIF_CFG g_onvif_cfg;
extern ONVIF_CLS g_onvif_cls;

/********************************************************************************/

/*
 * If the StateReportingSupported capability is set to true then the service/device shall be
 * capable of generating the following event whenever a schedule or its special days becomes
 * active or inactive based on the device time. It¡¯s a property event that indicates if the schedule
 * is active or not
 */
void onvif_ScheduleStateNotify(ONVIF_Schedule * p_req)
{
    ONVIF_NotificationMessage * p_message = onvif_add_NotificationMessage(NULL);
	if (p_message)
	{
	    ONVIF_SimpleItem * p_simpleitem;
	    
		strcpy(p_message->NotificationMessage.Dialect, "http://www.onvif.org/ver10/tev/topicExpression/ConcreteSet");
		strcpy(p_message->NotificationMessage.Topic, "tns1:Schedule/State/Active");
		p_message->NotificationMessage.Message.PropertyOperationFlag = 1;
		p_message->NotificationMessage.Message.PropertyOperation = PropertyOperation_Changed;
		p_message->NotificationMessage.Message.UtcTime = time(NULL)+1;

        p_message->NotificationMessage.Message.SourceFlag = 1;
        
		p_simpleitem = onvif_add_SimpleItem(&p_message->NotificationMessage.Message.Source.SimpleItem);
		if (p_simpleitem)
		{		    
			strcpy(p_simpleitem->SimpleItem.Name, "ScheduleToken");
			strcpy(p_simpleitem->SimpleItem.Value, p_req->Schedule.token);
		}

		p_simpleitem = onvif_add_SimpleItem(&p_message->NotificationMessage.Message.Source.SimpleItem);
		if (p_simpleitem)
		{		    
			strcpy(p_simpleitem->SimpleItem.Name, "Name");
			strcpy(p_simpleitem->SimpleItem.Value, p_req->Schedule.Name);
		}

		p_message->NotificationMessage.Message.DataFlag = 1;

		p_simpleitem = onvif_add_SimpleItem(&p_message->NotificationMessage.Message.Data.SimpleItem);
		if (p_simpleitem)
		{		    
			strcpy(p_simpleitem->SimpleItem.Name, "Active");
			strcpy(p_simpleitem->SimpleItem.Value, p_req->ScheduleState.Active ? "true" : "false");
		}

		p_simpleitem = onvif_add_SimpleItem(&p_message->NotificationMessage.Message.Data.SimpleItem);
		if (p_simpleitem)
		{		    
			strcpy(p_simpleitem->SimpleItem.Name, "SpecialDay");
			strcpy(p_simpleitem->SimpleItem.Value, p_req->ScheduleState.SpecialDay ? "true" : "false");
		}

		onvif_put_NotificationMessage(p_message);
	}
}

/*
 * Whenever the configuration data for a schedule is changed (including SpecialDaysSchedule)
 * or if a schedule is added,  the device shall provide the following event
 */
void onvif_ScheduleChangedNotify(ONVIF_Schedule * p_req)
{
    ONVIF_NotificationMessage * p_message = onvif_add_NotificationMessage(NULL);
	if (p_message)
	{
	    ONVIF_SimpleItem * p_simpleitem;
	    
		strcpy(p_message->NotificationMessage.Dialect, "http://www.onvif.org/ver10/tev/topicExpression/ConcreteSet");
		strcpy(p_message->NotificationMessage.Topic, "tns1:Configuration/Schedule/Changed");
		p_message->NotificationMessage.Message.PropertyOperationFlag = 1;
		p_message->NotificationMessage.Message.PropertyOperation = PropertyOperation_Changed;
		p_message->NotificationMessage.Message.UtcTime = time(NULL)+1;

        p_message->NotificationMessage.Message.SourceFlag = 1;
        
		p_simpleitem = onvif_add_SimpleItem(&p_message->NotificationMessage.Message.Source.SimpleItem);
		if (p_simpleitem)
		{		    
			strcpy(p_simpleitem->SimpleItem.Name, "ScheduleToken");
			strcpy(p_simpleitem->SimpleItem.Value, p_req->Schedule.token);
		}

		onvif_put_NotificationMessage(p_message);
	}
}

/*
 * Whenever a schedule is removed, the device shall provide the following event
 */
void onvif_ScheduleRemovedNotify(ONVIF_Schedule * p_req)
{
    ONVIF_NotificationMessage * p_message = onvif_add_NotificationMessage(NULL);
	if (p_message)
	{
	    ONVIF_SimpleItem * p_simpleitem;
	    
		strcpy(p_message->NotificationMessage.Dialect, "http://www.onvif.org/ver10/tev/topicExpression/ConcreteSet");
		strcpy(p_message->NotificationMessage.Topic, "tns1:Configuration/Schedule/Removed");
		p_message->NotificationMessage.Message.PropertyOperationFlag = 1;
		p_message->NotificationMessage.Message.PropertyOperation = PropertyOperation_Deleted;
		p_message->NotificationMessage.Message.UtcTime = time(NULL)+1;

        p_message->NotificationMessage.Message.SourceFlag = 1;
        
		p_simpleitem = onvif_add_SimpleItem(&p_message->NotificationMessage.Message.Source.SimpleItem);
		if (p_simpleitem)
		{		    
			strcpy(p_simpleitem->SimpleItem.Name, "ScheduleToken");
			strcpy(p_simpleitem->SimpleItem.Value, p_req->Schedule.token);
		}

		onvif_put_NotificationMessage(p_message);
	}
}

/*
 * Whenever the configuration data for a SpecialDays item is changed or added, the device shall
 * provide the following event
 */
void onvif_SpecialDayChangedNotify(ONVIF_SpecialDayGroup * p_req)
{
    ONVIF_NotificationMessage * p_message = onvif_add_NotificationMessage(NULL);
	if (p_message)
	{
	    ONVIF_SimpleItem * p_simpleitem;
	    
		strcpy(p_message->NotificationMessage.Dialect, "http://www.onvif.org/ver10/tev/topicExpression/ConcreteSet");
		strcpy(p_message->NotificationMessage.Topic, "tns1:Configuration/SpecialDays/Changed");
		p_message->NotificationMessage.Message.PropertyOperationFlag = 1;
		p_message->NotificationMessage.Message.PropertyOperation = PropertyOperation_Changed;
		p_message->NotificationMessage.Message.UtcTime = time(NULL)+1;

        p_message->NotificationMessage.Message.SourceFlag = 1;
        
		p_simpleitem = onvif_add_SimpleItem(&p_message->NotificationMessage.Message.Source.SimpleItem);
		if (p_simpleitem)
		{		    
			strcpy(p_simpleitem->SimpleItem.Name, "SpecialDaysToken");
			strcpy(p_simpleitem->SimpleItem.Value, p_req->SpecialDayGroup.token);
		}

		onvif_put_NotificationMessage(p_message);
	}
}

/*
 * Whenever a SpecialDays item is removed, the device shall provide the following event
 */
void onvif_SpecialDayRemovedNotify(ONVIF_SpecialDayGroup * p_req)
{
    ONVIF_NotificationMessage * p_message = onvif_add_NotificationMessage(NULL);
	if (p_message)
	{
	    ONVIF_SimpleItem * p_simpleitem;
	    
		strcpy(p_message->NotificationMessage.Dialect, "http://www.onvif.org/ver10/tev/topicExpression/ConcreteSet");
		strcpy(p_message->NotificationMessage.Topic, "tns1:Configuration/SpecialDays/Removed");
		p_message->NotificationMessage.Message.PropertyOperationFlag = 1;
		p_message->NotificationMessage.Message.PropertyOperation = PropertyOperation_Deleted;
		p_message->NotificationMessage.Message.UtcTime = time(NULL)+1;

        p_message->NotificationMessage.Message.SourceFlag = 1;
        
		p_simpleitem = onvif_add_SimpleItem(&p_message->NotificationMessage.Message.Source.SimpleItem);
		if (p_simpleitem)
		{		    
			strcpy(p_simpleitem->SimpleItem.Name, "SpecialDaysToken");
			strcpy(p_simpleitem->SimpleItem.Value, p_req->SpecialDayGroup.token);
		}

		onvif_put_NotificationMessage(p_message);
	}
}


/**
 * The possible return values:
 *  ONVIF_ERR_TooManyItems
 **/ 
ONVIF_RET onvif_tsc_GetScheduleInfo(tsc_GetScheduleInfo_REQ * p_req, tsc_GetScheduleInfo_RES * p_res)
{
    int i;
    int idx;
    ONVIF_Schedule * p_tmp;
    
    for (i = 0; i< p_req->sizeToken; i++)
    {
        p_tmp = onvif_find_Schedule(p_req->Token[i]);
        if (p_tmp)
        {
            idx = p_res->sizeScheduleInfo;

            p_res->ScheduleInfo[idx].DescriptionFlag = p_tmp->Schedule.DescriptionFlag;
            
            strcpy(p_res->ScheduleInfo[idx].token, p_tmp->Schedule.token);
            strcpy(p_res->ScheduleInfo[idx].Name, p_tmp->Schedule.Name);
            strcpy(p_res->ScheduleInfo[idx].Description, p_tmp->Schedule.Description);            
            
            p_res->sizeScheduleInfo++;
        }
    }

    return ONVIF_OK;
}

/**
 * The possible return values:
 *  ONVIF_ERR_InvalidStartReference
 **/ 
ONVIF_RET onvif_tsc_GetScheduleInfoList(tsc_GetScheduleInfoList_REQ * p_req, tsc_GetScheduleInfoList_RES * p_res)
{
    int idx;
    int nums = 0;
    ONVIF_Schedule * p_tmp = g_onvif_cfg.schedule;
    
    if (p_req->StartReferenceFlag)
    {
        p_tmp = onvif_find_Schedule(p_req->StartReference);
        if (NULL == p_tmp)
        {
            return ONVIF_ERR_InvalidStartReference;
        }
    }

    while (p_tmp)
    {
        idx = p_res->sizeScheduleInfo;

        p_res->ScheduleInfo[idx].DescriptionFlag = p_tmp->Schedule.DescriptionFlag;
        
        strcpy(p_res->ScheduleInfo[idx].token, p_tmp->Schedule.token);
        strcpy(p_res->ScheduleInfo[idx].Name, p_tmp->Schedule.Name);
        strcpy(p_res->ScheduleInfo[idx].Description, p_tmp->Schedule.Description);  
            
        p_res->sizeScheduleInfo++;
        
        p_tmp = p_tmp->next;

        nums++;
        if (p_req->LimitFlag && nums >= p_req->Limit)
        {
            break;
        }
        else if (nums >= ARRAY_SIZE(p_res->ScheduleInfo))
        {
            break;
        }
    }

    if (p_tmp)
    {
        p_res->NextStartReferenceFlag = 1;
        strcpy(p_res->NextStartReference, p_tmp->Schedule.token);
    }    
    
    return ONVIF_OK;
}

/**
 * The possible return values:
 *  ONVIF_ERR_TooManyItems
 **/
ONVIF_RET onvif_tsc_GetSchedules(tsc_GetSchedules_REQ * p_req, tsc_GetSchedules_RES * p_res)
{
    int i;
    int idx;
    ONVIF_Schedule * p_tmp;
    
    for (i = 0; i< p_req->sizeToken; i++)
    {
        p_tmp = onvif_find_Schedule(p_req->Token[i]);
        if (p_tmp)
        {
            idx = p_res->sizeSchedule;

            memcpy(&p_res->Schedule[idx], &p_tmp->Schedule, sizeof(onvif_Schedule));          
            
            p_res->sizeSchedule++;
        }
    }

    return ONVIF_OK;
}

/**
 * The possible return values:
 *  ONVIF_ERR_InvalidStartReference
 **/
ONVIF_RET onvif_tsc_GetScheduleList(tsc_GetScheduleList_REQ * p_req, tsc_GetScheduleList_RES * p_res)
{
    int idx;
    int nums = 0;
    ONVIF_Schedule * p_tmp = g_onvif_cfg.schedule;
    
    if (p_req->StartReferenceFlag)
    {
        p_tmp = onvif_find_Schedule(p_req->StartReference);
        if (NULL == p_tmp)
        {
            return ONVIF_ERR_InvalidStartReference;
        }
    }

    while (p_tmp)
    {
        idx = p_res->sizeSchedule;

        memcpy(&p_res->Schedule[idx], &p_tmp->Schedule, sizeof(onvif_Schedule));  
            
        p_res->sizeSchedule++;
        
        p_tmp = p_tmp->next;

        nums++;
        if (p_req->LimitFlag && nums >= p_req->Limit)
        {
            break;
        }
        else if (nums >= ARRAY_SIZE(p_res->Schedule))
        {
            break;
        }
    }

    if (p_tmp)
    {
        p_res->NextStartReferenceFlag = 1;
        strcpy(p_res->NextStartReference, p_tmp->Schedule.token);
    }    
    
    return ONVIF_OK;
}

/**
 * The possible return values:
 *  ONVIF_ERR_MaxSchedules
 *  ONVIF_ERR_MaxSpecialDaysSchedules
 *  ONVIF_ERR_MaxTimePeriodsPerDay
 *  ONVIF_ERR_ReferenceNotFound
 **/
ONVIF_RET onvif_tsc_CreateSchedule(tsc_CreateSchedule_REQ * p_req, tsc_CreateSchedule_RES * p_res)
{
    int i, j;
    ONVIF_Schedule * p_tmp;
#ifdef LIBICAL
    char * p;
    VObject * vcal = NULL;
    icalcomponent *comp = NULL;
#endif

    // parameter check
    if (strlen(p_req->Schedule.token) > 0)
    {
        return ONVIF_ERR_InvalidArgVal;
    }

#ifdef LIBICAL
    // replace "&#xD;" with "\n"
    p = strstr(p_req->Schedule.Standard, "&#xD;");
    while (p)
	{
		memmove(p+1, p+5, strlen(p+5));
		*p = '\n';
		p = strstr(p+5, "&#xD;");
		p_req->Schedule.Standard[strlen(p_req->Schedule.Standard)-4] = '\0';
	}
	
    vcal = Parse_MIME(p_req->Schedule.Standard, strlen(p_req->Schedule.Standard));
    if (vcal)
    {
        int cnt;
        
        comp = icalvcal_convert(vcal);
        
        cnt = icalcomponent_count_components(comp, ICAL_VEVENT_COMPONENT);
        if (cnt > SCHEDULE_MAX_LIMIT)
        {
            icalcomponent_free(comp);
            return ONVIF_ERR_MaxTimePeriodsPerDay;
        }
    }
#endif // end of LIBICAL

    for (i = 0; i < p_req->Schedule.sizeSpecialDays; i++)
    {
        for (j = 0; j < p_req->Schedule.SpecialDays[i].sizeTimeRange; j++)
        {
            if (p_req->Schedule.SpecialDays[i].TimeRange[j].UntilFlag && 
                strcmp(p_req->Schedule.SpecialDays[i].TimeRange[j].Until, p_req->Schedule.SpecialDays[i].TimeRange[j].From) <= 0)
            {
                return ONVIF_ERR_InvalidArgVal;
            }
        }
    }

    // add new Schedule
    p_tmp = onvif_add_Schedule();
    if (NULL == p_tmp)
    {
        return ONVIF_ERR_MaxSchedules;
    }

    memcpy(&p_tmp->Schedule, &p_req->Schedule, sizeof(onvif_Schedule));

    sprintf(p_tmp->Schedule.token, "ScheduleToken%d", g_onvif_cls.schedule_idx++);
    strcpy(p_res->Token, p_tmp->Schedule.token);

#ifdef LIBICAL
    p_tmp->comp = comp;
#endif

    // here add hander code ...

    // send event notify
    onvif_ScheduleChangedNotify(p_tmp);
    
    return ONVIF_OK;
}

/**
 * The possible return values:
 *  ONVIF_ERR_NotFound
 *  ONVIF_ERR_MaxSpecialDaysSchedules
 *  ONVIF_ERR_MaxTimePeriodsPerDay
 *  ONVIF_ERR_ReferenceNotFound
 **/
ONVIF_RET onvif_tsc_ModifySchedule(tsc_ModifySchedule_REQ * p_req)
{
    int i, j;
#ifdef LIBICAL
    char * p;
    VObject * vcal = NULL;
    icalcomponent *comp = NULL;
#endif

    ONVIF_Schedule * p_tmp = onvif_find_Schedule(p_req->Schedule.token);
    if (NULL == p_tmp)
    {
        return ONVIF_ERR_NotFound;
    }
    
    // parameter check
#ifdef LIBICAL
    // replace "&#xD;" with "\n"
    p = strstr(p_req->Schedule.Standard, "&#xD;");
    while (p)
    {
        memmove(p+1, p+5, strlen(p+5));
        *p = '\n';
        p = strstr(p+5, "&#xD;");
        p_req->Schedule.Standard[strlen(p_req->Schedule.Standard)-4] = '\0';
    }
    
    vcal = Parse_MIME(p_req->Schedule.Standard, strlen(p_req->Schedule.Standard));
    if (vcal)
    {
        int cnt;
        
        comp = icalvcal_convert(vcal);
        
        cnt = icalcomponent_count_components(comp, ICAL_VEVENT_COMPONENT);
        if (cnt > SCHEDULE_MAX_LIMIT)
        {
            icalcomponent_free(comp);
            return ONVIF_ERR_MaxTimePeriodsPerDay;
        }
    }
#endif // end of LIBICAL

    for (i = 0; i < p_req->Schedule.sizeSpecialDays; i++)
    {
        for (j = 0; j < p_req->Schedule.SpecialDays[i].sizeTimeRange; j++)
        {
            if (p_req->Schedule.SpecialDays[i].TimeRange[j].UntilFlag && 
                strcmp(p_req->Schedule.SpecialDays[i].TimeRange[j].Until, p_req->Schedule.SpecialDays[i].TimeRange[j].From) <= 0)
            {
                return ONVIF_ERR_InvalidArgVal;
            }
        }
    }

#ifdef LIBICAL
    if (p_tmp->comp)
    {
        icalcomponent_free(p_tmp->comp);
    }
    p_tmp->comp = comp;
#endif

    // here add hander code ...
    
    // modify the Schedule
    memcpy(&p_tmp->Schedule, &p_req->Schedule, sizeof(onvif_Schedule));

    // send event notify
    onvif_ScheduleChangedNotify(p_tmp);
            
    return ONVIF_OK;
}

/**
 * The possible return values:
 *  ONVIF_ERR_NotFound
 *  ONVIF_ERR_ReferenceInUse
 **/
ONVIF_RET onvif_tsc_DeleteSchedule(tsc_DeleteSchedule_REQ * p_req)
{
    ONVIF_Schedule * p_tmp = onvif_find_Schedule(p_req->Token);
    if (NULL == p_tmp)
    {
        return ONVIF_ERR_NotFound;
    }

    // here add hander code ...

    // send event notify
    onvif_ScheduleRemovedNotify(p_tmp);
        
    // delete the Schedule
    onvif_free_Schedule(p_tmp);

    return ONVIF_OK;
}

/**
 * The possible return values:
 *  ONVIF_ERR_TooManyItems
 **/
ONVIF_RET onvif_tsc_GetSpecialDayGroupInfo(tsc_GetSpecialDayGroupInfo_REQ * p_req, tsc_GetSpecialDayGroupInfo_RES * p_res)
{
    int i;
    int idx;
    ONVIF_SpecialDayGroup * p_tmp;
    
    for (i = 0; i< p_req->sizeToken; i++)
    {
        p_tmp = onvif_find_SpecialDayGroup(p_req->Token[i]);
        if (p_tmp)
        {
            idx = p_res->sizeSpecialDayGroupInfo;

            p_res->SpecialDayGroupInfo[idx].DescriptionFlag = p_tmp->SpecialDayGroup.DescriptionFlag;
            
            strcpy(p_res->SpecialDayGroupInfo[idx].token, p_tmp->SpecialDayGroup.token);
            strcpy(p_res->SpecialDayGroupInfo[idx].Name, p_tmp->SpecialDayGroup.Name);
            strcpy(p_res->SpecialDayGroupInfo[idx].Description, p_tmp->SpecialDayGroup.Description);            
            
            p_res->sizeSpecialDayGroupInfo++;
        }
    }

    return ONVIF_OK;
}

/**
 * The possible return values:
 *  ONVIF_ERR_InvalidStartReference
 **/
ONVIF_RET onvif_tsc_GetSpecialDayGroupInfoList(tsc_GetSpecialDayGroupInfoList_REQ * p_req, tsc_GetSpecialDayGroupInfoList_RES * p_res)
{
    int idx;
    int nums = 0;
    ONVIF_SpecialDayGroup * p_tmp = g_onvif_cfg.specialdaygroup;
    
    if (p_req->StartReferenceFlag)
    {
        p_tmp = onvif_find_SpecialDayGroup(p_req->StartReference);
        if (NULL == p_tmp)
        {
            return ONVIF_ERR_InvalidStartReference;
        }
    }

    while (p_tmp)
    {
        idx = p_res->sizeSpecialDayGroupInfo;

        p_res->SpecialDayGroupInfo[idx].DescriptionFlag = p_tmp->SpecialDayGroup.DescriptionFlag;
        
        strcpy(p_res->SpecialDayGroupInfo[idx].token, p_tmp->SpecialDayGroup.token);
        strcpy(p_res->SpecialDayGroupInfo[idx].Name, p_tmp->SpecialDayGroup.Name);
        strcpy(p_res->SpecialDayGroupInfo[idx].Description, p_tmp->SpecialDayGroup.Description);  
            
        p_res->sizeSpecialDayGroupInfo++;
        
        p_tmp = p_tmp->next;

        nums++;
        if (p_req->LimitFlag && nums >= p_req->Limit)
        {
            break;
        }
        else if (nums >= ARRAY_SIZE(p_res->SpecialDayGroupInfo))
        {
            break;
        }
    }

    if (p_tmp)
    {
        p_res->NextStartReferenceFlag = 1;
        strcpy(p_res->NextStartReference, p_tmp->SpecialDayGroup.token);
    }    
    
    return ONVIF_OK;
}

/**
 * The possible return values:
 *  ONVIF_ERR_TooManyItems
 **/
ONVIF_RET onvif_tsc_GetSpecialDayGroups(tsc_GetSpecialDayGroups_REQ * p_req, tsc_GetSpecialDayGroups_RES * p_res)
{
    int i;
    int idx;
    ONVIF_SpecialDayGroup * p_tmp;
    
    for (i = 0; i< p_req->sizeToken; i++)
    {
        p_tmp = onvif_find_SpecialDayGroup(p_req->Token[i]);
        if (p_tmp)
        {
            idx = p_res->sizeSpecialDayGroup;

            memcpy(&p_res->SpecialDayGroup[idx], &p_tmp->SpecialDayGroup, sizeof(onvif_SpecialDayGroup));                 
            
            p_res->sizeSpecialDayGroup++;
        }
    }

    return ONVIF_OK;
}

/**
 * The possible return values:
 *  ONVIF_ERR_InvalidStartReference
 **/
ONVIF_RET onvif_tsc_GetSpecialDayGroupList(tsc_GetSpecialDayGroupList_REQ * p_req, tsc_GetSpecialDayGroupList_RES * p_res)
{
    int idx;
    int nums = 0;
    ONVIF_SpecialDayGroup * p_tmp = g_onvif_cfg.specialdaygroup;
    
    if (p_req->StartReferenceFlag)
    {
        p_tmp = onvif_find_SpecialDayGroup(p_req->StartReference);
        if (NULL == p_tmp)
        {
            return ONVIF_ERR_InvalidStartReference;
        }
    }

    while (p_tmp)
    {
        idx = p_res->sizeSpecialDayGroup;

        memcpy(&p_res->SpecialDayGroup[idx], &p_tmp->SpecialDayGroup, sizeof(onvif_SpecialDayGroup));  
            
        p_res->sizeSpecialDayGroup++;
        
        p_tmp = p_tmp->next;

        nums++;
        if (p_req->LimitFlag && nums >= p_req->Limit)
        {
            break;
        }
        else if (nums >= ARRAY_SIZE(p_res->SpecialDayGroup))
        {
            break;
        }
    }

    if (p_tmp)
    {
        p_res->NextStartReferenceFlag = 1;
        strcpy(p_res->NextStartReference, p_tmp->SpecialDayGroup.token);
    }    
    
    return ONVIF_OK;
}

/**
 * The possible return values:
 *  ONVIF_ERR_MaxSpecialDayGroups
 *  ONVIF_ERR_MaxDaysInSpecialDayGroup
 **/
ONVIF_RET onvif_tsc_CreateSpecialDayGroup(tsc_CreateSpecialDayGroup_REQ * p_req, tsc_CreateSpecialDayGroup_RES * p_res)
{
#ifdef LIBICAL
    char * p;
    VObject * vcal = NULL;
    icalcomponent *comp = NULL;
#endif
    ONVIF_SpecialDayGroup * p_tmp;
    
    // parameter check
    if (strlen(p_req->SpecialDayGroup.token) > 0)
    {
        return ONVIF_ERR_InvalidArgVal;
    }

#ifdef LIBICAL
    if (p_req->SpecialDayGroup.DaysFlag)
    {
        // replace "&#xD;" with "\n"
        p = strstr(p_req->SpecialDayGroup.Days, "&#xD;");
        while (p)
        {
            memmove(p+1, p+5, strlen(p+5));
            *p = '\n';
            p = strstr(p+5, "&#xD;");
            p_req->SpecialDayGroup.Days[strlen(p_req->SpecialDayGroup.Days)-4] = '\0';
        }
        
        vcal = Parse_MIME(p_req->SpecialDayGroup.Days, strlen(p_req->SpecialDayGroup.Days));
        if (vcal)
        {
            comp = icalvcal_convert(vcal);
        }
    }
#endif // end of LIBICAL

    // add new SpecialDayGroup
    p_tmp = onvif_add_SpecialDayGroup();
    if (NULL == p_tmp)
    {
        return ONVIF_ERR_MaxSpecialDayGroups;
    }

    memcpy(&p_tmp->SpecialDayGroup, &p_req->SpecialDayGroup, sizeof(onvif_SpecialDayGroup));

    sprintf(p_tmp->SpecialDayGroup.token, "SpecialDayGroupToken%d", g_onvif_cls.specialdaygroup_idx++);
    strcpy(p_res->Token, p_tmp->SpecialDayGroup.token);

#ifdef LIBICAL
    p_tmp->comp = comp;
#endif

    // here add hander code ...

    // send event notify
    onvif_SpecialDayChangedNotify(p_tmp);
    
    return ONVIF_OK;
}

/**
 * The possible return values:
 *  ONVIF_ERR_NotFound
 *  ONVIF_ERR_MaxDaysInSpecialDayGroup
 **/
ONVIF_RET onvif_tsc_ModifySpecialDayGroup(tsc_ModifySpecialDayGroup_REQ * p_req)
{
#ifdef LIBICAL
    char * p;
    VObject * vcal = NULL;
    icalcomponent *comp = NULL;
#endif

    ONVIF_SpecialDayGroup * p_tmp = onvif_find_SpecialDayGroup(p_req->SpecialDayGroup.token);
    if (NULL == p_tmp)
    {
        return ONVIF_ERR_NotFound;
    }
    
    // parameter check
#ifdef LIBICAL
    if (p_req->SpecialDayGroup.DaysFlag)
    {
        // replace "&#xD;" with "\n"
        p = strstr(p_req->SpecialDayGroup.Days, "&#xD;");
        while (p)
        {
            memmove(p+1, p+5, strlen(p+5));
            *p = '\n';
            p = strstr(p+5, "&#xD;");
            p_req->SpecialDayGroup.Days[strlen(p_req->SpecialDayGroup.Days)-4] = '\0';
        }
        
        vcal = Parse_MIME(p_req->SpecialDayGroup.Days, strlen(p_req->SpecialDayGroup.Days));
        if (vcal)
        {
            comp = icalvcal_convert(vcal);
            if (p_tmp->comp)
            {
                icalcomponent_free(p_tmp->comp);
            }
            p_tmp->comp = comp;
        }
    }
#endif // end of LIBICAL

    // here add hander code ...
    
    // modify the SpecialDayGroup
    memcpy(&p_tmp->SpecialDayGroup, &p_req->SpecialDayGroup, sizeof(onvif_SpecialDayGroup));

    // send event notify
    onvif_SpecialDayChangedNotify(p_tmp);
            
    return ONVIF_OK;
}

/**
 * The possible return values:
 *  ONVIF_ERR_NotFound
 *  ONVIF_ERR_ReferenceInUse
 **/
ONVIF_RET onvif_tsc_DeleteSpecialDayGroup(tsc_DeleteSpecialDayGroup_REQ * p_req)
{
    ONVIF_SpecialDayGroup * p_tmp = onvif_find_SpecialDayGroup(p_req->Token);
    if (NULL == p_tmp)
    {
        return ONVIF_ERR_NotFound;
    }

    // here add hander code ...

    // send event notify
    onvif_SpecialDayRemovedNotify(p_tmp);
        
    // delete the SpecialDayGroup
    onvif_free_SpecialDayGroup(p_tmp);

    return ONVIF_OK;
}

/**
 * The possible return values:
 *  ONVIF_ERR_NotFound
 **/
ONVIF_RET onvif_tsc_GetScheduleState(tsc_GetScheduleState_REQ * p_req, tsc_GetScheduleState_RES * p_res)
{
    ONVIF_Schedule * p_tmp = onvif_find_Schedule(p_req->Token);
    if (NULL == p_tmp)
    {
        return ONVIF_ERR_NotFound;
    }

    p_res->ScheduleState.Active = p_tmp->ScheduleState.Active;
    p_res->ScheduleState.SpecialDayFlag = p_tmp->ScheduleState.SpecialDayFlag;
    p_res->ScheduleState.SpecialDay= p_tmp->ScheduleState.SpecialDay;

    return ONVIF_OK;
}

#endif // end of SCHEDULE_SUPPORT




