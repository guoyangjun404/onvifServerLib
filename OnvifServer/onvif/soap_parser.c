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
#include "onvif.h"
#include "soap_parser.h"
#include "onvif_utils.h"
#include "util.h"

/***************************************************************************************/

BOOL parse_Bool(const char * pdata)
{    
    if (strcasecmp(pdata, "true") == 0)
    {
        return TRUE;
    }
    else
    {
        return FALSE;
    }
}

BOOL parse_XSDDatetime(const char * s, time_t * p)
{
	if (s)
	{ 
		char zone[32];
		struct tm T;
		const char *t;
		
		*zone = '\0';
		memset(&T, 0, sizeof(T));
		
		if (strchr(s, '-'))
		{
			t = "%d-%d-%dT%d:%d:%d%31s";
		}	
		else if (strchr(s, ':'))
		{
			t = "%4d%2d%2dT%d:%d:%d%31s";
		}	
		else /* parse non-XSD-standard alternative ISO 8601 format */
		{
			t = "%4d%2d%2dT%2d%2d%2d%31s";
		}
		
		if (sscanf(s, t, &T.tm_year, &T.tm_mon, &T.tm_mday, &T.tm_hour, &T.tm_min, &T.tm_sec, zone) < 6)
		{
			return FALSE;
		}
		
		if (T.tm_year == 1)
		{
			T.tm_year = 70;
		}	
		else
		{
			T.tm_year -= 1900;
		}
		
		T.tm_mon--;
		
		if (*zone == '.')
		{ 
			for (s = zone + 1; *s; s++)
			{
				if (*s < '0' || *s > '9')
				{
					break;
				}	
			}	
		}
    	else
    	{
      		s = zone;
      	}
      	
		if (*s)
		{
			if (*s == '+' || *s == '-')
			{ 
				int h = 0, m = 0;
				if (s[3] == ':')
				{ 
					/* +hh:mm */
					sscanf(s, "%d:%d", &h, &m);
					if (h < 0)
						m = -m;
				}
				else /* +hhmm */
				{
					m = (int)strtol(s, NULL, 10);
					h = m / 100;
					m = m % 100;
				}
				
				T.tm_min -= m;
				T.tm_hour -= h;
				/* put hour and min in range */
				T.tm_hour += T.tm_min / 60;
				T.tm_min %= 60;
				
				if (T.tm_min < 0)
				{ 
					T.tm_min += 60;
					T.tm_hour--;
				}
				
				T.tm_mday += T.tm_hour / 24;
				T.tm_hour %= 24;
				
				if (T.tm_hour < 0)
				{
					T.tm_hour += 24;
					T.tm_mday--;
				}
				/* note: day of the month may be out of range, timegm() handles it */
			}

			*p = onvif_timegm(&T);
		}
		else /* no UTC or timezone, so assume we got a localtime */
		{ 
			T.tm_isdst = -1;
			*p = mktime(&T);
		}
	}
	
	return TRUE;
}

BOOL parse_XSDDuration(const char *s, int *a)
{ 
	int sign = 1, Y = 0, M = 0, D = 0, H = 0, N = 0, S = 0;
	float f = 0;
	*a = 0;
	if (s)
	{ 
		if (*s == '-')
		{ 
			sign = -1;
			s++;
		}
		if (*s++ != 'P')
			return FALSE;
			
		/* date part */
		while (s && *s)
		{ 
			int n;
			char k;
			if (*s == 'T')
			{ 
				s++;
				break;
			}
			
			if (sscanf(s, "%d%c", &n, &k) != 2)
				return FALSE;
				
			s = strchr(s, k);
			if (!s)
				return FALSE;
				
			switch (k)
			{ 
			case 'Y':
				Y = n;
				break;
				
			case 'M':
				M = n;
				break;
				
			case 'D':
				D = n;
				break;
				
			default:
				return FALSE;
			}
			
			s++;
		}
		
	    /* time part */
	    while (s && *s)
		{ 
			int n;
			char k;
			if (sscanf(s, "%d%c", &n, &k) != 2)
				return FALSE;
				
			s = strchr(s, k);
			if (!s)
				return FALSE;
				
			switch (k)
			{ 
			case 'H':
				H = n;
				break;
				
			case 'M':
				N = n;
				break;
				
			case '.':
				S = n;
				if (sscanf(s, "%g", &f) != 1)
					return FALSE;
				s = NULL;
				continue;
				
			case 'S':
				S = n;
				break;
				
			default:
				return FALSE;
			}
			
			s++;
		}
	    /* convert Y-M-D H:N:S.f to signed int */
	    *a = sign * ((((((((((Y * 12) + M) * 30) + D) * 24) + H) * 60) + N) * 60) + S);
	}

	return TRUE;
}


/***************************************************************************************/
ONVIF_RET parse_GetServices(XMLN * p_node, GetServices_REQ * p_req)
{
    XMLN * p_IncludeCapability;

    p_IncludeCapability = xml_node_soap_get(p_node, "IncludeCapability");
    if (p_IncludeCapability && p_IncludeCapability->data)
    {
        p_req->IncludeCapability = parse_Bool(p_IncludeCapability->data);
    }

    return ONVIF_OK;
}

ONVIF_RET parse_MulticastConfiguration(XMLN * p_node, onvif_MulticastConfiguration * p_req)
{
	XMLN * p_Multicast;
	XMLN * p_Address;
	XMLN * p_Port;
	XMLN * p_TTL;
	XMLN * p_AutoStart;
	
	p_Multicast = xml_node_soap_get(p_node, "Multicast");
    if (NULL == p_Multicast)
    {
        return ONVIF_ERR_MissingAttribute;
    }

    p_Address = xml_node_soap_get(p_Multicast, "Address");
    if (p_Address)
    {
    	XMLN * p_IPv4Address;
    	
        p_IPv4Address = xml_node_soap_get(p_Address, "IPv4Address");
	    if (p_IPv4Address && p_IPv4Address->data)
	    {
	        strncpy(p_req->IPv4Address, p_IPv4Address->data, sizeof(p_req->IPv4Address)-1);
	    }
    }

    p_Port = xml_node_soap_get(p_Multicast, "Port");
    if (p_Port && p_Port->data)
    {
    	p_req->Port = atoi(p_Port->data);
    }

    p_TTL = xml_node_soap_get(p_Multicast, "TTL");
    if (p_TTL && p_TTL->data)
    {
    	p_req->TTL = atoi(p_TTL->data);
    }

    p_AutoStart = xml_node_soap_get(p_Multicast, "AutoStart");
    if (p_AutoStart && p_AutoStart->data)
    {
    	p_req->AutoStart = parse_Bool(p_AutoStart->data);
    }

    return ONVIF_OK;
}

ONVIF_RET parse_SetVideoEncoderConfiguration(XMLN * p_node, SetVideoEncoderConfiguration_REQ * p_req)
{
    XMLN * p_Configuration;
	XMLN * p_Name;
	XMLN * p_UseCount;
	XMLN * p_Encoding;
	XMLN * p_Resolution;
	XMLN * p_Quality;
	XMLN * p_RateControl;
	XMLN * p_SessionTimeout;
	XMLN * p_ForcePersistence;
	const char * token;

	p_Configuration = xml_node_soap_get(p_node, "Configuration");
    if (NULL == p_Configuration)
    {
        return ONVIF_ERR_MissingAttribute;
    }
    
    token = xml_attr_get(p_Configuration, "token");
    if (token)
    {
        strncpy(p_req->Configuration.token, token, sizeof(p_req->Configuration.token)-1);
    }    

    p_Name = xml_node_soap_get(p_Configuration, "Name");
    if (p_Name && p_Name->data)
    {
        strncpy(p_req->Configuration.Name, p_Name->data, sizeof(p_req->Configuration.Name)-1);
    }

    p_UseCount = xml_node_soap_get(p_Configuration, "UseCount");
    if (p_UseCount && p_UseCount->data)
    {
        p_req->Configuration.UseCount = atoi(p_UseCount->data);
    }

    p_Encoding = xml_node_soap_get(p_Configuration, "Encoding");
    if (p_Encoding && p_Encoding->data)
    {
		p_req->Configuration.Encoding = onvif_StringToVideoEncoding(p_Encoding->data);
    }

    p_Resolution = xml_node_soap_get(p_Configuration, "Resolution");
    if (p_Resolution)
    {
        XMLN * p_Width;
		XMLN * p_Height;

		p_Width = xml_node_soap_get(p_Resolution, "Width");
	    if (p_Width && p_Width->data)
	    {
	        p_req->Configuration.Resolution.Width = atoi(p_Width->data);
	    }

	    p_Height = xml_node_soap_get(p_Resolution, "Height");
	    if (p_Height && p_Height->data)
	    {
	        p_req->Configuration.Resolution.Height = atoi(p_Height->data);
	    }
    }

    p_Quality = xml_node_soap_get(p_Configuration, "Quality");
    if (p_Quality && p_Quality->data)
    {
        p_req->Configuration.Quality = atoi(p_Quality->data);
    }

    p_RateControl = xml_node_soap_get(p_Configuration, "RateControl");
    if (p_RateControl)
    {
    	XMLN * p_FrameRateLimit;
		XMLN * p_EncodingInterval;
		XMLN * p_BitrateLimit;
		
		p_req->Configuration.RateControlFlag = 1;
    	
        p_FrameRateLimit = xml_node_soap_get(p_RateControl, "FrameRateLimit");
	    if (p_FrameRateLimit && p_FrameRateLimit->data)
	    {
	        p_req->Configuration.RateControl.FrameRateLimit = atoi(p_FrameRateLimit->data);
	    }

	    p_EncodingInterval = xml_node_soap_get(p_RateControl, "EncodingInterval");
	    if (p_EncodingInterval && p_EncodingInterval->data)
	    {
	        p_req->Configuration.RateControl.EncodingInterval = atoi(p_EncodingInterval->data);
	    }

	    p_BitrateLimit = xml_node_soap_get(p_RateControl, "BitrateLimit");
	    if (p_BitrateLimit && p_BitrateLimit->data)
	    {
	        p_req->Configuration.RateControl.BitrateLimit = atoi(p_BitrateLimit->data);
	    }
    }
    
    if (p_req->Configuration.Encoding == VideoEncoding_H264)
    {
    	XMLN * p_H264 = xml_node_soap_get(p_Configuration, "H264");
    	if (p_H264)
    	{
    		XMLN * p_GovLength;
			XMLN * p_H264Profile;
			
    		p_req->Configuration.H264Flag = 1;
    		
    		p_GovLength = xml_node_soap_get(p_H264, "GovLength");
		    if (p_GovLength && p_GovLength->data)
		    {
		        p_req->Configuration.H264.GovLength = atoi(p_GovLength->data);
		    }

		    p_H264Profile = xml_node_soap_get(p_H264, "H264Profile");
		    if (p_H264Profile && p_H264Profile->data)
		    {
				p_req->Configuration.H264.H264Profile = onvif_StringToH264Profile(p_H264Profile->data);
		    }
    	}
    }
    else if (p_req->Configuration.Encoding == VideoEncoding_MPEG4)
    {
    	XMLN * p_MPEG4 = xml_node_soap_get(p_Configuration, "MPEG4");
    	if (p_MPEG4)
    	{
    		XMLN * p_GovLength;
			XMLN * p_Mpeg4Profile;
			
    		p_req->Configuration.MPEG4Flag = 1;
    		
    		p_GovLength = xml_node_soap_get(p_MPEG4, "GovLength");
		    if (p_GovLength && p_GovLength->data)
		    {
		        p_req->Configuration.MPEG4.GovLength = atoi(p_GovLength->data);
		    }

		    p_Mpeg4Profile = xml_node_soap_get(p_MPEG4, "Mpeg4Profile");
		    if (p_Mpeg4Profile && p_Mpeg4Profile->data)
		    {
				p_req->Configuration.MPEG4.Mpeg4Profile = onvif_StringToMpeg4Profile(p_Mpeg4Profile->data);
		    }
    	}
    }	

	parse_MulticastConfiguration(p_Configuration, &p_req->Configuration.Multicast);
	
	p_SessionTimeout = xml_node_soap_get(p_Configuration, "SessionTimeout");
	if (p_SessionTimeout && p_SessionTimeout->data)
	{
		parse_XSDDuration(p_SessionTimeout->data, &p_req->Configuration.SessionTimeout);
	}

	p_ForcePersistence = xml_node_soap_get(p_node, "ForcePersistence");
	if (p_ForcePersistence && p_ForcePersistence->data)
	{
		p_req->ForcePersistence = parse_Bool(p_ForcePersistence->data);
	}
    	
    return ONVIF_OK;
}

ONVIF_RET parse_SetSynchronizationPoint(XMLN * p_node, SetSynchronizationPoint_REQ * p_req)
{
    XMLN * p_ProfileToken;

    p_ProfileToken = xml_node_soap_get(p_node, "ProfileToken");
    if (p_ProfileToken && p_ProfileToken->data)
    {
        strncpy(p_req->ProfileToken, p_ProfileToken->data, sizeof(p_req->ProfileToken)-1);
    }
    
    return ONVIF_OK;
}

ONVIF_RET parse_GetSystemLog(XMLN * p_node, GetSystemLog_REQ * p_req)
{
    XMLN * p_LogType;

    p_LogType = xml_node_soap_get(p_node, "LogType");
    if (p_LogType && p_LogType->data)
    {
        p_req->LogType = onvif_StringToSystemLogType(p_LogType->data);
    }

    return ONVIF_OK;
}

ONVIF_RET parse_SetSystemDateAndTime(XMLN * p_node, SetSystemDateAndTime_REQ * p_req)
{
    XMLN * p_DateTimeType;
	XMLN * p_DaylightSavings;
	XMLN * p_TimeZone;
	XMLN * p_UTCDateTime;

	p_DateTimeType = xml_node_soap_get(p_node, "DateTimeType");
    if (p_DateTimeType && p_DateTimeType->data)
    {
        p_req->SystemDateTime.DateTimeType = onvif_StringToSetDateTimeType(p_DateTimeType->data);
    }    

    p_DaylightSavings = xml_node_soap_get(p_node, "DaylightSavings");
    if (p_DaylightSavings && p_DaylightSavings->data)
    {
        p_req->SystemDateTime.DaylightSavings = parse_Bool(p_DaylightSavings->data);
    }

    p_TimeZone = xml_node_soap_get(p_node, "TimeZone");
    if (p_TimeZone)
    {
    	XMLN * p_TZ;
		
    	p_req->SystemDateTime.TimeZoneFlag = 1;
    	
        p_TZ = xml_node_soap_get(p_TimeZone, "TZ");
		if (p_TZ && p_TZ->data)
		{
			strncpy(p_req->SystemDateTime.TimeZone.TZ, p_TZ->data, sizeof(p_req->SystemDateTime.TimeZone.TZ)-1);
		}		
    }    

    p_UTCDateTime = xml_node_soap_get(p_node, "UTCDateTime");
    if (p_UTCDateTime)
    {
    	XMLN * p_Time;
		XMLN * p_Hour;
		XMLN * p_Minute;
		XMLN * p_Second;
		XMLN * p_Date;
		XMLN * p_Year;
		XMLN * p_Month;
		XMLN * p_Day;
		
    	p_req->UTCDateTimeFlag = 1;
    	
        p_Time = xml_node_soap_get(p_UTCDateTime, "Time");
	    if (!p_Time)
	    {
	        return ONVIF_ERR_MissingAttribute;
	    }

	    p_Hour = xml_node_soap_get(p_Time, "Hour");
	    if (!p_Hour || !p_Hour->data)
	    {
	        return ONVIF_ERR_MissingAttribute;
	    }
	    p_req->UTCDateTime.Time.Hour = atoi(p_Hour->data);

	    p_Minute = xml_node_soap_get(p_Time, "Minute");
	    if (!p_Minute || !p_Minute->data)
	    {
	        return ONVIF_ERR_MissingAttribute;
	    }
	    p_req->UTCDateTime.Time.Minute = atoi(p_Minute->data);

	    p_Second = xml_node_soap_get(p_Time, "Second");
	    if (!p_Second || !p_Second->data)
	    {
	        return ONVIF_ERR_MissingAttribute;
	    }
	    p_req->UTCDateTime.Time.Second = atoi(p_Second->data);

	    p_Date = xml_node_soap_get(p_UTCDateTime, "Date");
	    if (!p_Date)
	    {
	        return ONVIF_ERR_MissingAttribute;
	    }

	    p_Year = xml_node_soap_get(p_Date, "Year");
	    if (!p_Year || !p_Year->data)
	    {
	        return ONVIF_ERR_MissingAttribute;
	    }
	    p_req->UTCDateTime.Date.Year = atoi(p_Year->data);

	    p_Month = xml_node_soap_get(p_Date, "Month");
	    if (!p_Month || !p_Month->data)
	    {
	        return ONVIF_ERR_MissingAttribute;
	    }
	    p_req->UTCDateTime.Date.Month = atoi(p_Month->data);

	    p_Day = xml_node_soap_get(p_Date, "Day");
	    if (!p_Day || !p_Day->data)
	    {
	        return ONVIF_ERR_MissingAttribute;
	    }
	    p_req->UTCDateTime.Date.Day = atoi(p_Day->data);
    }    

    return ONVIF_OK;
}

ONVIF_RET parse_AddScopes(XMLN * p_node, AddScopes_REQ * p_req)
{
	int i = 0;
	
	XMLN * p_ScopeItem = xml_node_soap_get(p_node, "ScopeItem");
	while (p_ScopeItem && soap_strcmp(p_ScopeItem->name, "ScopeItem") == 0)
	{
		if (i < ARRAY_SIZE(p_req->ScopeItem))
		{
			strncpy(p_req->ScopeItem[i], p_ScopeItem->data, sizeof(p_req->ScopeItem[i])-1);

			++i;
		}
		else
		{
			return ONVIF_ERR_TooManyScopes;
		}
		
		p_ScopeItem = p_ScopeItem->next;
	}

	return ONVIF_OK;
}

ONVIF_RET parse_SetScopes(XMLN * p_node, SetScopes_REQ * p_req)
{
	int i = 0;
	
	XMLN * p_Scopes = xml_node_soap_get(p_node, "Scopes");
	while (p_Scopes && soap_strcmp(p_Scopes->name, "Scopes") == 0)
	{
		if (i < ARRAY_SIZE(p_req->Scopes))
		{
			strncpy(p_req->Scopes[i], p_Scopes->data, sizeof(p_req->Scopes[i])-1);

			++i;
		}
		else
		{
			return ONVIF_ERR_TooManyScopes;
		}
		
		p_Scopes = p_Scopes->next;
	}

	return ONVIF_OK;
}

ONVIF_RET parse_RemoveScopes(XMLN * p_node, RemoveScopes_REQ * p_req)
{
    int i = 0;
	
	XMLN * p_ScopeItem = xml_node_soap_get(p_node, "ScopeItem");
	while (p_ScopeItem && soap_strcmp(p_ScopeItem->name, "ScopeItem") == 0)
	{
		if (i < ARRAY_SIZE(p_req->ScopeItem))
		{
			strncpy(p_req->ScopeItem[i], p_ScopeItem->data, sizeof(p_req->ScopeItem[i])-1);

			++i;
		}
		else
		{
			return ONVIF_ERR_TooManyScopes;
		}
		
		p_ScopeItem = p_ScopeItem->next;
	}

	return ONVIF_OK;
}

ONVIF_RET parse_SetDiscoveryMode(XMLN * p_node, SetDiscoveryMode_REQ * p_req)
{
	XMLN * p_DiscoveryMode = xml_node_soap_get(p_node, "DiscoveryMode");
	if (p_DiscoveryMode && p_DiscoveryMode->data)
	{
		p_req->DiscoveryMode = onvif_StringToDiscoveryMode(p_DiscoveryMode->data);	
	}

	return ONVIF_OK;
}

ONVIF_RET parse_Filter(XMLN * p_node, ONVIF_FILTER * p_req)
{
    int i = 0;
    XMLN * p_TopicExpression;
    XMLN * p_MessageContent;

    p_TopicExpression = xml_node_soap_get(p_node, "TopicExpression");
    while (p_TopicExpression && soap_strcmp(p_TopicExpression->name, "TopicExpression") == 0)
    {
        if (p_TopicExpression->data && i < ARRAY_SIZE(p_req->TopicExpression))
        {
            strncpy(p_req->TopicExpression[i], p_TopicExpression->data, sizeof(p_req->TopicExpression[i])-1);
            i++;
        }

        p_TopicExpression = p_TopicExpression->next;
    }

    i = 0;

    p_MessageContent = xml_node_soap_get(p_node, "MessageContent");
    while (p_MessageContent && soap_strcmp(p_MessageContent->name, "MessageContent") == 0)
    {
        if (p_MessageContent->data && i < ARRAY_SIZE(p_req->MessageContent))
        {
            strncpy(p_req->MessageContent[i], p_MessageContent->data, sizeof(p_req->MessageContent[i])-1);
            i++;
        }

        p_MessageContent = p_MessageContent->next;
    }

    return ONVIF_OK;
}

ONVIF_RET parse_Subscribe(XMLN * p_node, Subscribe_REQ * p_req)
{
	XMLN * p_ConsumerReference;
	XMLN * p_Address;
	XMLN * p_InitialTerminationTime;
	XMLN * p_Filter;
	
	p_ConsumerReference = xml_node_soap_get(p_node, "ConsumerReference");
	if (NULL == p_ConsumerReference)
	{
		return ONVIF_ERR_MissingAttribute;		
	}

	p_Address = xml_node_soap_get(p_ConsumerReference, "Address");
	if (p_Address && p_Address->data)
	{
		strncpy(p_req->ConsumerReference, p_Address->data, sizeof(p_req->ConsumerReference)-1);
	}	

	p_InitialTerminationTime = xml_node_soap_get(p_node, "InitialTerminationTime");
	if (p_InitialTerminationTime && p_InitialTerminationTime->data)
	{
		p_req->InitialTerminationTimeFlag = 1;
		parse_XSDDuration(p_InitialTerminationTime->data, &p_req->InitialTerminationTime);
	}

    p_Filter = xml_node_soap_get(p_node, "Filter");
	if (p_Filter)
	{
	    p_req->FiltersFlag = 1;
		parse_Filter(p_Filter, &p_req->Filters);
	}
    
	return ONVIF_OK;
}

ONVIF_RET parse_Renew(XMLN * p_node, Renew_REQ * p_req)
{
	XMLN * p_TerminationTime;
	
	p_TerminationTime = xml_node_soap_get(p_node, "TerminationTime");
	if (p_TerminationTime && p_TerminationTime->data)
	{
	    if (p_TerminationTime->data[0] == 'P' || 
	        (p_TerminationTime->data[0] == '-' && p_TerminationTime->data[1] == 'P'))
	    {
	        p_req->TerminationTimeType = 1;
	        
		    parse_XSDDuration(p_TerminationTime->data, (int *)&p_req->TerminationTime);
	    }
	    else
	    {
	        p_req->TerminationTimeType = 0;
	        
		    parse_XSDDatetime(p_TerminationTime->data, &p_req->TerminationTime);
		}    
	}
	else
	{
		return ONVIF_ERR_MissingAttribute;
	}

	return ONVIF_OK;
}

ONVIF_RET parse_CreatePullPointSubscription(XMLN * p_node, CreatePullPointSubscription_REQ * p_req)
{
	XMLN * p_InitialTerminationTime;
	XMLN * p_Filter;

	p_InitialTerminationTime = xml_node_soap_get(p_node, "InitialTerminationTime");
	if (p_InitialTerminationTime && p_InitialTerminationTime->data)
	{
		p_req->InitialTerminationTimeFlag = 1;
		parse_XSDDuration(p_InitialTerminationTime->data, &p_req->InitialTerminationTime);
	}

    p_Filter = xml_node_soap_get(p_node, "Filter");
	if (p_Filter)
	{
	    p_req->FiltersFlag = 1;
		parse_Filter(p_Filter, &p_req->Filters);
	}
	
	return ONVIF_OK;
}

ONVIF_RET parse_PullMessages(XMLN * p_node, PullMessages_REQ * p_req)
{
	XMLN * p_Timeout;
	XMLN * p_MessageLimit;

	p_Timeout = xml_node_soap_get(p_node, "Timeout");
	if (p_Timeout && p_Timeout->data)
	{
		parse_XSDDuration(p_Timeout->data, &p_req->Timeout);
	}

	p_MessageLimit = xml_node_soap_get(p_node, "MessageLimit");
	if (p_MessageLimit && p_MessageLimit->data)
	{
		p_req->MessageLimit = atoi(p_MessageLimit->data);
	}

	return ONVIF_OK;
}

ONVIF_RET parse_Vector(XMLN * p_node, onvif_Vector * p_req)
{
	const char * p_x;
	const char * p_y;
	
	p_x = xml_attr_get(p_node, "x");
	if (p_x)
	{			
		p_req->x = (float)atof(p_x);
	}

	p_y = xml_attr_get(p_node, "y");
	if (p_y)
	{
		p_req->y = (float)atof(p_y);
	}

	return ONVIF_OK;
}

ONVIF_RET parse_Vector1D(XMLN * p_node, onvif_Vector1D * p_req)
{
	const char * p_x;
	
	p_x = xml_attr_get(p_node, "x");
	if (p_x)
	{			
		p_req->x = (float)atof(p_x);
	}

	return ONVIF_OK;
}

ONVIF_RET parse_SetDNS(XMLN * p_node, SetDNS_REQ * p_req)
{
	int i = 0;
	XMLN * p_FromDHCP;
	XMLN * p_SearchDomain;
	XMLN * p_DNSManual;
	
	assert(p_node);

	p_FromDHCP = xml_node_soap_get(p_node, "FromDHCP");
	if (p_FromDHCP && p_FromDHCP->data)
	{
		p_req->DNSInformation.FromDHCP = parse_Bool(p_FromDHCP->data);
	}	
	
	p_SearchDomain = xml_node_soap_get(p_node, "SearchDomain");
	while (p_SearchDomain && soap_strcmp(p_SearchDomain->name, "SearchDomain") == 0)
	{
		p_req->DNSInformation.SearchDomainFlag = 1;
		
		if (p_SearchDomain->data && i < ARRAY_SIZE(p_req->DNSInformation.SearchDomain))
		{
			strncpy(p_req->DNSInformation.SearchDomain[i], p_SearchDomain->data, sizeof(p_req->DNSInformation.SearchDomain[i])-1);
			++i;
		}

		p_SearchDomain = p_SearchDomain->next;
	}

	i = 0;
	
	p_DNSManual = xml_node_soap_get(p_node, "DNSManual");
	while (p_DNSManual && soap_strcmp(p_DNSManual->name, "DNSManual") == 0)
	{
		XMLN * p_Type;
		XMLN * p_IPv4Address;
		
		p_Type = xml_node_soap_get(p_DNSManual, "Type");
		if (p_Type && p_Type->data)
		{
			if (strcasecmp(p_Type->data, "IPv4") != 0) // todo : now only support ipv4
			{
				p_DNSManual = p_DNSManual->next;
				continue;
			}
		}

		p_IPv4Address = xml_node_soap_get(p_DNSManual, "IPv4Address");
		if (p_IPv4Address && p_IPv4Address->data)
		{
			if (is_ip_address(p_IPv4Address->data) == FALSE)
			{
				return ONVIF_ERR_InvalidIPv4Address;
			}
			else if (i < ARRAY_SIZE(p_req->DNSInformation.DNSServer))
			{
				strncpy(p_req->DNSInformation.DNSServer[i], p_IPv4Address->data, sizeof(p_req->DNSInformation.DNSServer[i])-1);
				++i;
			}
		}
		
		p_DNSManual = p_DNSManual->next;
	}

	return ONVIF_OK;	
}

ONVIF_RET parse_SetNTP(XMLN * p_node, SetNTP_REQ * p_req)
{
	int i = 0;
	XMLN * p_FromDHCP;
	XMLN * p_NTPManual;
	
	assert(p_node);

	p_FromDHCP = xml_node_soap_get(p_node, "FromDHCP");
	if (p_FromDHCP && p_FromDHCP->data)
	{
		p_req->NTPInformation.FromDHCP = parse_Bool(p_FromDHCP->data);
	}	
	
	p_NTPManual = xml_node_soap_get(p_node, "NTPManual");
	while (p_NTPManual && soap_strcmp(p_NTPManual->name, "NTPManual") == 0)
	{
		XMLN * p_Type;
		XMLN * p_IPv4Address;
		XMLN * p_DNSname;
		
		p_Type = xml_node_soap_get(p_NTPManual, "Type");
		if (p_Type && p_Type->data)
		{
			if (strcasecmp(p_Type->data, "IPv4") != 0 && strcasecmp(p_Type->data, "DNS") != 0) // todo : now only support ipv4
			{
				p_NTPManual = p_NTPManual->next;
				continue;
			}
		}

		p_IPv4Address = xml_node_soap_get(p_NTPManual, "IPv4Address");
		if (p_IPv4Address && p_IPv4Address->data)
		{
			if (is_ip_address(p_IPv4Address->data) == FALSE)
			{
				return ONVIF_ERR_InvalidIPv4Address;
			}
			else if (i < ARRAY_SIZE(p_req->NTPInformation.NTPServer))
			{
				strncpy(p_req->NTPInformation.NTPServer[i], p_IPv4Address->data, sizeof(p_req->NTPInformation.NTPServer[i])-1);
				++i;
			}
		}

		p_DNSname = xml_node_soap_get(p_NTPManual, "DNSname");
		if (p_DNSname && p_DNSname->data)
		{
			if (i < ARRAY_SIZE(p_req->NTPInformation.NTPServer))
			{
				strncpy(p_req->NTPInformation.NTPServer[i], p_DNSname->data, sizeof(p_req->NTPInformation.NTPServer[i])-1);
				++i;
			}
		}
		
		p_NTPManual = p_NTPManual->next;
	}

	return ONVIF_OK;
}

ONVIF_RET parse_SetZeroConfiguration(XMLN * p_node, SetZeroConfiguration_REQ * p_req)
{
    XMLN * p_InterfaceToken;
    XMLN * p_Enabled;

    p_InterfaceToken = xml_node_soap_get(p_node, "InterfaceToken");
	if (p_InterfaceToken && p_InterfaceToken->data)
	{
	    strncpy(p_req->InterfaceToken, p_InterfaceToken->data, sizeof(p_req->InterfaceToken)-1);
	}

	p_Enabled = xml_node_soap_get(p_node, "Enabled");
	if (p_Enabled && p_Enabled->data)
	{
	    p_req->Enabled = parse_Bool(p_Enabled->data);
	}

	return ONVIF_OK;
}

ONVIF_RET parse_GetDot11Status(XMLN * p_node, GetDot11Status_REQ * p_req)
{
    XMLN * p_InterfaceToken;

    p_InterfaceToken = xml_node_soap_get(p_node, "InterfaceToken");
	if (p_InterfaceToken && p_InterfaceToken->data)
	{
	    strncpy(p_req->InterfaceToken, p_InterfaceToken->data, sizeof(p_req->InterfaceToken)-1);
	}

	return ONVIF_OK;
}

ONVIF_RET parse_ScanAvailableDot11Networks(XMLN * p_node, ScanAvailableDot11Networks_REQ * p_req)
{
    XMLN * p_InterfaceToken;

    p_InterfaceToken = xml_node_soap_get(p_node, "InterfaceToken");
	if (p_InterfaceToken && p_InterfaceToken->data)
	{
	    strncpy(p_req->InterfaceToken, p_InterfaceToken->data, sizeof(p_req->InterfaceToken)-1);
	}

	return ONVIF_OK;
}

ONVIF_RET parse_SetNetworkProtocols(XMLN * p_node, SetNetworkProtocols_REQ * p_req)
{
	char name[32];
	BOOL enable;
	int  port[MAX_SERVER_PORT];
	XMLN * p_NetworkProtocols;

	assert(p_node);
	
	p_NetworkProtocols = xml_node_soap_get(p_node, "NetworkProtocols");
	while (p_NetworkProtocols && soap_strcmp(p_NetworkProtocols->name, "NetworkProtocols") == 0)
	{
		int i = 0;
		XMLN * p_Name;
		XMLN * p_Enabled;
		XMLN * p_Port;
		
		enable = FALSE;
		memset(name, 0, sizeof(name));
		memset(port, 0, sizeof(int)*MAX_SERVER_PORT);
		
		p_Name = xml_node_soap_get(p_NetworkProtocols, "Name");
		if (p_Name && p_Name->data)
		{
			strncpy(name, p_Name->data, sizeof(name)-1);
		}

		p_Enabled = xml_node_soap_get(p_NetworkProtocols, "Enabled");
		if (p_Enabled && p_Enabled->data)
		{
			if (strcasecmp(p_Enabled->data, "true") == 0)
			{
				enable = TRUE;
			}
		}		
		
		p_Port = xml_node_soap_get(p_NetworkProtocols, "Port");
		while (p_Port && p_Port->data && soap_strcmp(p_Port->name, "Port") == 0)
		{
			if (i < ARRAY_SIZE(port))
			{
				port[i++] = atoi(p_Port->data);
			}
			
			p_Port = p_Port->next;
		}

		if (strcasecmp(name, "HTTP") == 0)
		{
			p_req->NetworkProtocol.HTTPFlag = 1;
			p_req->NetworkProtocol.HTTPEnabled = enable;
			memcpy(p_req->NetworkProtocol.HTTPPort, port, sizeof(p_req->NetworkProtocol.HTTPPort));
		}
		else if (strcasecmp(name, "HTTPS") == 0)
		{
			p_req->NetworkProtocol.HTTPSFlag = 1;
			p_req->NetworkProtocol.HTTPSEnabled = enable;
			memcpy(p_req->NetworkProtocol.HTTPSPort, port, sizeof(p_req->NetworkProtocol.HTTPSPort));
		}
		else if (strcasecmp(name, "RTSP") == 0)
		{
			p_req->NetworkProtocol.RTSPFlag = 1;
			p_req->NetworkProtocol.RTSPEnabled = enable;
			memcpy(p_req->NetworkProtocol.RTSPPort, port, sizeof(p_req->NetworkProtocol.RTSPPort));
		}
		else
		{
			return ONVIF_ERR_ServiceNotSupported;
		}

		p_NetworkProtocols = p_NetworkProtocols->next;
	}
	
	return ONVIF_OK;
}

ONVIF_RET parse_SetNetworkDefaultGateway(XMLN * p_node, SetNetworkDefaultGateway_REQ * p_req)
{
	int i = 0;
	XMLN * p_IPv4Address;
	
	assert(p_node);	
	
	p_IPv4Address = xml_node_soap_get(p_node, "IPv4Address");
	while (p_IPv4Address && p_IPv4Address->data && soap_strcmp(p_IPv4Address->name, "IPv4Address") == 0)
	{
		if (is_ip_address(p_IPv4Address->data) == FALSE)
		{
			return ONVIF_ERR_InvalidIPv4Address;
		}

		if (i < ARRAY_SIZE(p_req->IPv4Address))
		{
			strncpy(p_req->IPv4Address[i++], p_IPv4Address->data, sizeof(p_req->IPv4Address[0])-1);
		}

		p_IPv4Address = p_IPv4Address->next;
	}

	return ONVIF_OK;
}

ONVIF_RET parse_GetCapabilities(XMLN * p_node, GetCapabilities_REQ * p_req)
{
    XMLN * p_Category;

    p_Category = xml_node_soap_get(p_node, "Category");
	if (p_Category && p_Category->data)
	{
		p_req->Category = onvif_StringToCapabilityCategory(p_Category->data);
	}

	return ONVIF_OK;
}

ONVIF_RET parse_GetProfile(XMLN * p_node, GetProfile_REQ * p_req)
{
    XMLN * p_ProfileToken;

    p_ProfileToken = xml_node_soap_get(p_node, "ProfileToken");
	if (p_ProfileToken && p_ProfileToken->data)
	{
		strncpy(p_req->ProfileToken, p_ProfileToken->data, sizeof(p_req->ProfileToken)-1);
	}

	return ONVIF_OK;
}

ONVIF_RET parse_CreateProfile(XMLN * p_node, CreateProfile_REQ * p_req)
{
	XMLN * p_Name;
	XMLN * p_Token;
	
	assert(p_node);

	p_Name = xml_node_soap_get(p_node, "Name");
	if (p_Name && p_Name->data)
	{
		strncpy(p_req->Name, p_Name->data, sizeof(p_req->Name)-1);
	}
	
	p_Token = xml_node_soap_get(p_node, "Token");
	if (p_Token && p_Token->data)
	{
		p_req->TokenFlag = 1;
		strncpy(p_req->Token, p_Token->data, sizeof(p_req->Token)-1);
	}

	return ONVIF_OK;
}

ONVIF_RET parse_DeleteProfile(XMLN * p_node, DeleteProfile_REQ * p_req)
{
    XMLN * p_ProfileToken;

    p_ProfileToken = xml_node_soap_get(p_node, "ProfileToken");
	if (p_ProfileToken && p_ProfileToken->data)
	{
		strncpy(p_req->ProfileToken, p_ProfileToken->data, sizeof(p_req->ProfileToken)-1);
	}

	return ONVIF_OK;
}

ONVIF_RET parse_AddVideoSourceConfiguration(XMLN * p_node, AddVideoSourceConfiguration_REQ * p_req)
{
	XMLN * p_ProfileToken;
	XMLN * p_ConfigurationToken;
	
	assert(p_node);

	p_ProfileToken = xml_node_soap_get(p_node, "ProfileToken");
	if (p_ProfileToken && p_ProfileToken->data)
	{
		strncpy(p_req->ProfileToken, p_ProfileToken->data, sizeof(p_req->ProfileToken)-1);
	}
	
	p_ConfigurationToken = xml_node_soap_get(p_node, "ConfigurationToken");
	if (p_ConfigurationToken && p_ConfigurationToken->data)
	{
		strncpy(p_req->ConfigurationToken, p_ConfigurationToken->data, sizeof(p_req->ConfigurationToken)-1);
	}
	
	return ONVIF_OK;
}

ONVIF_RET parse_RemoveVideoSourceConfiguration(XMLN * p_node, RemoveVideoSourceConfiguration_REQ * p_req)
{
    XMLN * p_ProfileToken;
    
    p_ProfileToken = xml_node_soap_get(p_node, "ProfileToken");
	if (p_ProfileToken && p_ProfileToken->data)
	{
		strncpy(p_req->ProfileToken, p_ProfileToken->data, sizeof(p_req->ProfileToken)-1);
	}

	return ONVIF_OK;
}

ONVIF_RET parse_AddVideoEncoderConfiguration(XMLN * p_node, AddVideoEncoderConfiguration_REQ * p_req)
{
	XMLN * p_ProfileToken;
	XMLN * p_ConfigurationToken;
	
	assert(p_node);

	p_ProfileToken = xml_node_soap_get(p_node, "ProfileToken");
	if (p_ProfileToken && p_ProfileToken->data)
	{
		strncpy(p_req->ProfileToken, p_ProfileToken->data, sizeof(p_req->ProfileToken)-1);
	}
	
	p_ConfigurationToken = xml_node_soap_get(p_node, "ConfigurationToken");
	if (p_ConfigurationToken && p_ConfigurationToken->data)
	{
		strncpy(p_req->ConfigurationToken, p_ConfigurationToken->data, sizeof(p_req->ConfigurationToken)-1);
	}
	
	return ONVIF_OK;
}

ONVIF_RET parse_RemoveVideoEncoderConfiguration(XMLN * p_node, RemoveVideoEncoderConfiguration_REQ * p_req)
{
    XMLN * p_ProfileToken;
    
    p_ProfileToken = xml_node_soap_get(p_node, "ProfileToken");
	if (p_ProfileToken && p_ProfileToken->data)
	{
		strncpy(p_req->ProfileToken, p_ProfileToken->data, sizeof(p_req->ProfileToken)-1);
	}

	return ONVIF_OK;
}

ONVIF_RET parse_StreamSetup(XMLN * p_node, onvif_StreamSetup * p_req)
{
	XMLN * p_Stream;
	XMLN * p_Transport;

	p_Stream = xml_node_soap_get(p_node, "Stream");
	if (p_Stream && p_Stream->data)
	{
		p_req->Stream = onvif_StringToStreamType(p_Stream->data);
		if (StreamType_Invalid == p_req->Stream)
		{
			return ONVIF_ERR_InvalidStreamSetup;
		}
	}

	p_Transport = xml_node_soap_get(p_node, "Transport");
	if (p_Transport)
	{
		XMLN * p_Protocol = xml_node_soap_get(p_Transport, "Protocol");
		if (p_Protocol && p_Protocol->data)
		{
			p_req->Transport.Protocol = onvif_StringToTransportProtocol(p_Protocol->data);
			if (TransportProtocol_Invalid == p_req->Transport.Protocol)
			{
				return ONVIF_ERR_InvalidStreamSetup;
			}
		}
	}

	return ONVIF_OK;
}

ONVIF_RET parse_GetStreamUri(XMLN * p_node, GetStreamUri_REQ * p_req)
{
	ONVIF_RET ret = ONVIF_OK;
	XMLN * p_StreamSetup;
	XMLN * p_ProfileToken;

	p_StreamSetup = xml_node_soap_get(p_node, "StreamSetup");
	if (p_StreamSetup)
	{
		ret = parse_StreamSetup(p_StreamSetup, &p_req->StreamSetup);		
	}
	
	p_ProfileToken = xml_node_soap_get(p_node, "ProfileToken");
	if (p_ProfileToken && p_ProfileToken->data)
	{
		strncpy(p_req->ProfileToken, p_ProfileToken->data, sizeof(p_req->ProfileToken)-1);
	}
	
	return ret;
}

BOOL parse_Dot11Configuration(XMLN * p_node, onvif_Dot11Configuration * p_req)
{
    XMLN * p_SSID;
    XMLN * p_Mode;
    XMLN * p_Alias;
    XMLN * p_Priority;
    XMLN * p_Security;

    p_SSID = xml_node_soap_get(p_node, "SSID");
	if (p_SSID && p_SSID->data)
	{
	    strncpy(p_req->SSID, p_SSID->data, sizeof(p_req->SSID)-1);
	}

	p_Mode = xml_node_soap_get(p_node, "Mode");
	if (p_Mode && p_Mode->data)
	{
	    p_req->Mode = onvif_StringToDot11StationMode(p_Mode->data);
	}

	p_Alias = xml_node_soap_get(p_node, "Alias");
	if (p_Alias && p_Alias->data)
	{
	    strncpy(p_req->Alias, p_Alias->data, sizeof(p_req->Alias)-1);
	}

	p_Priority = xml_node_soap_get(p_node, "Priority");
	if (p_Priority && p_Priority->data)
	{
	    p_req->Priority = atoi(p_Priority->data);
	}

	p_Security = xml_node_soap_get(p_node, "Security");
    if (p_Security)
    {
        XMLN * p_Mode;
        XMLN * p_Algorithm;
        XMLN * p_PSK;
        XMLN * p_Dot1X;

        p_Mode = xml_node_soap_get(p_Security, "Mode");
    	if (p_Mode && p_Mode->data)
    	{
    	    p_req->Security.Mode = onvif_StringToDot11SecurityMode(p_Mode->data);
    	}

    	p_Algorithm = xml_node_soap_get(p_Security, "Algorithm");
    	if (p_Algorithm && p_Algorithm->data)
    	{
    	    p_req->Security.AlgorithmFlag = 1;
    	    p_req->Security.Algorithm = onvif_StringToDot11Cipher(p_Algorithm->data);
    	}

    	p_PSK = xml_node_soap_get(p_Security, "PSK");
    	if (p_PSK)
    	{
    	    XMLN * p_Key;
            XMLN * p_Passphrase;

            p_req->Security.PSKFlag = 1;
            
            p_Key = xml_node_soap_get(p_PSK, "Key");
        	if (p_Key && p_Key->data)
        	{
        	    p_req->Security.PSK.KeyFlag = 1;
        	    strncpy(p_req->Security.PSK.Key, p_Key->data, sizeof(p_req->Security.PSK.Key)-1);
        	}

        	p_Passphrase = xml_node_soap_get(p_PSK, "Passphrase");
        	if (p_Passphrase && p_Passphrase->data)
        	{
        	    p_req->Security.PSK.PassphraseFlag = 1;
        	    strncpy(p_req->Security.PSK.Passphrase, p_Passphrase->data, sizeof(p_req->Security.PSK.Passphrase)-1);
        	}
    	}

    	p_Dot1X = xml_node_soap_get(p_Security, "Dot1X");
    	if (p_Dot1X && p_Dot1X->data)
    	{
    	    p_req->Security.Dot1XFlag = 1;
    	    strncpy(p_req->Security.Dot1X, p_Dot1X->data, sizeof(p_req->Security.Dot1X)-1);
    	}
    }

	return TRUE;
}

ONVIF_RET parse_GetSnapshotUri(XMLN * p_node, GetSnapshotUri_REQ * p_req)
{
    XMLN * p_ProfileToken;

    p_ProfileToken = xml_node_soap_get(p_node, "ProfileToken");
	if (p_ProfileToken && p_ProfileToken->data)
	{
	    strncpy(p_req->ProfileToken, p_ProfileToken->data, sizeof(p_req->ProfileToken)-1);
	}

	return ONVIF_OK;
}

ONVIF_RET parse_SetNetworkInterfaces(XMLN * p_node, SetNetworkInterfaces_REQ * p_req)
{
    XMLN * p_InterfaceToken;
	XMLN * p_NetworkInterface;

	p_InterfaceToken = xml_node_soap_get(p_node, "InterfaceToken");
	if (p_InterfaceToken && p_InterfaceToken->data)
	{
	    strncpy(p_req->NetworkInterface.token, p_InterfaceToken->data, sizeof(p_req->NetworkInterface.token)-1);
	}
	else
	{
	    return ONVIF_ERR_MissingAttribute;
	}

	p_NetworkInterface = xml_node_soap_get(p_node, "NetworkInterface");
	if (p_NetworkInterface)
	{
		XMLN * p_Enabled;
		XMLN * p_MTU;
		XMLN * p_IPv4;
		XMLN * p_Extension;
		
	    p_req->NetworkInterface.Enabled = TRUE;
	    
	    p_Enabled = xml_node_soap_get(p_NetworkInterface, "Enabled");
	    if (p_Enabled && p_Enabled->data)
	    {
	        p_req->NetworkInterface.Enabled = parse_Bool(p_Enabled->data);
	    }

	    p_MTU = xml_node_soap_get(p_NetworkInterface, "MTU");
	    if (p_MTU && p_MTU->data)
	    {
	    	p_req->NetworkInterface.InfoFlag = 1;
	    	p_req->NetworkInterface.Info.MTUFlag = 1;
	        p_req->NetworkInterface.Info.MTU = atoi(p_MTU->data);
	    }

	    p_IPv4 = xml_node_soap_get(p_NetworkInterface, "IPv4");
	    if (p_IPv4)
	    {
	    	XMLN * p_Enabled;
			XMLN * p_DHCP;
			
	        p_req->NetworkInterface.IPv4Flag = 1;
	        
	        p_Enabled = xml_node_soap_get(p_IPv4, "Enabled");
	        if (p_Enabled && p_Enabled->data)
    	    {
    	        p_req->NetworkInterface.IPv4.Enabled = parse_Bool(p_Enabled->data);
    	    }

    	    p_DHCP = xml_node_soap_get(p_IPv4, "DHCP");
	        if (p_DHCP && p_DHCP->data)
	        {
	            p_req->NetworkInterface.IPv4.Config.DHCP = parse_Bool(p_DHCP->data);
	        }

	        if (p_req->NetworkInterface.IPv4.Config.DHCP == FALSE)
	        {
	            XMLN * p_Manual = xml_node_soap_get(p_IPv4, "Manual");
	            if (p_Manual)
	            {
	                XMLN * p_Address;
					XMLN * p_PrefixLength;

					p_Address = xml_node_soap_get(p_Manual, "Address");
	                if (p_Address && p_Address->data)
	                {
	                    strncpy(p_req->NetworkInterface.IPv4.Config.Address, p_Address->data, sizeof(p_req->NetworkInterface.IPv4.Config.Address)-1);
	                }

	                p_PrefixLength = xml_node_soap_get(p_Manual, "PrefixLength");
	                if (p_PrefixLength && p_PrefixLength->data)
	                {
	                    p_req->NetworkInterface.IPv4.Config.PrefixLength = atoi(p_PrefixLength->data);
	                }
	            }
	        }
	    }

	    p_Extension = xml_node_soap_get(p_NetworkInterface, "Extension");
	    if (p_Extension)
	    {
	        XMLN * p_InterfaceType;
	        XMLN * p_Dot11;

	        p_req->NetworkInterface.ExtensionFlag = 1;

	        p_InterfaceType = xml_node_soap_get(p_Extension, "InterfaceType");
            if (p_InterfaceType && p_InterfaceType->data)
            {
                p_req->NetworkInterface.Extension.InterfaceType = atoi(p_InterfaceType->data);
            }

	        p_Dot11 = xml_node_soap_get(p_Extension, "Dot11");
	        while (p_Dot11 && soap_strcmp(p_Dot11->name, "Dot11") == 0)
	        {
	            int idx = p_req->NetworkInterface.Extension.sizeDot11;

                parse_Dot11Configuration(p_Dot11, &p_req->NetworkInterface.Extension.Dot11[idx]);
                
	            p_req->NetworkInterface.Extension.sizeDot11++;
	            if (p_req->NetworkInterface.Extension.sizeDot11 >= ARRAY_SIZE(p_req->NetworkInterface.Extension.Dot11))
	            {
	                break;
	            }

	            p_Dot11 = p_Dot11->next;
	        }
	    }
	}
	
	return ONVIF_OK;
}

ONVIF_RET parse_GetVideoSourceConfigurationOptions(XMLN * p_node, GetVideoSourceConfigurationOptions_REQ * p_req)
{
	XMLN * p_ConfigurationToken;
	XMLN * p_ProfileToken;
	
	assert(p_node);

	p_ConfigurationToken = xml_node_soap_get(p_node, "ConfigurationToken");
	if (p_ConfigurationToken && p_ConfigurationToken->data)
	{
		p_req->ConfigurationTokenFlag = 1;
		strncpy(p_req->ConfigurationToken, p_ConfigurationToken->data, sizeof(p_req->ConfigurationToken)-1);
	}
	
	p_ProfileToken = xml_node_soap_get(p_node, "ProfileToken");
	if (p_ProfileToken && p_ProfileToken->data)
	{
		p_req->ProfileTokenFlag = 1;
		strncpy(p_req->ProfileToken, p_ProfileToken->data, sizeof(p_req->ProfileToken)-1);
	}
	
	return ONVIF_OK;
}

ONVIF_RET parse_VideoSourceConfiguration(XMLN * p_node, onvif_VideoSourceConfiguration * p_req)
{
    XMLN * p_Configuration;
	XMLN * p_Name;
	XMLN * p_UseCount;
	XMLN * p_SourceToken;
	XMLN * p_Bounds;

	p_Configuration = xml_node_soap_get(p_node, "Configuration");
	if (p_Configuration)
	{
		const char * token = xml_attr_get(p_Configuration, "token");
		if (token)
		{
			strncpy(p_req->token, token, sizeof(p_req->token)-1);
		}
		else
		{
			return ONVIF_ERR_MissingAttribute;
		}
	}
	else
	{
		return ONVIF_ERR_MissingAttribute;
	}

	p_Name = xml_node_soap_get(p_Configuration, "Name");
	if (p_Name && p_Name->data)
	{
		strncpy(p_req->Name, p_Name->data, sizeof(p_req->Name)-1);
	}

	p_UseCount = xml_node_soap_get(p_Configuration, "UseCount");
	if (p_UseCount && p_UseCount->data)
	{
		p_req->UseCount = atoi(p_UseCount->data);
	}

	p_SourceToken = xml_node_soap_get(p_Configuration, "SourceToken");
	if (p_SourceToken && p_SourceToken->data)
	{
		strncpy(p_req->SourceToken, p_SourceToken->data, sizeof(p_req->SourceToken)-1);
	}

	p_Bounds = xml_node_soap_get(p_Configuration, "Bounds");
	if (p_Bounds)
	{
		const char * p_x;
		const char * p_y;
		const char * p_width;
		const char * p_height;

		p_x = xml_attr_get(p_Bounds, "x");
		if (p_x)
		{
			p_req->Bounds.x = atoi(p_x);
		}

		p_y = xml_attr_get(p_Bounds, "y");
		if (p_y)
		{
			p_req->Bounds.y = atoi(p_y);
		}

		p_width = xml_attr_get(p_Bounds, "width");
		if (p_width)
		{
			p_req->Bounds.width = atoi(p_width);
		}

		p_height = xml_attr_get(p_Bounds, "height");
		if (p_height)
		{
			p_req->Bounds.height = atoi(p_height);
		}
	}

	return ONVIF_OK;
}

ONVIF_RET parse_SetVideoSourceConfiguration(XMLN * p_node, SetVideoSourceConfiguration_REQ * p_req)
{
	XMLN * p_ForcePersistence;

    parse_VideoSourceConfiguration(p_node, &p_req->Configuration);
    
	p_ForcePersistence = xml_node_soap_get(p_node, "ForcePersistence");
	if (p_ForcePersistence && p_ForcePersistence->data)
	{
		p_req->ForcePersistence = parse_Bool(p_ForcePersistence->data);
	}	
	
	return ONVIF_OK;
}

ONVIF_RET parse_GetVideoEncoderConfigurationOptions(XMLN * p_node, GetVideoEncoderConfigurationOptions_REQ * p_req)
{
	XMLN * p_ConfigurationToken;
	XMLN * p_ProfileToken;
	
	assert(p_node);

	p_ConfigurationToken = xml_node_soap_get(p_node, "ConfigurationToken");
	if (p_ConfigurationToken && p_ConfigurationToken->data)
	{
		p_req->ConfigurationTokenFlag = 1;
		strncpy(p_req->ConfigurationToken, p_ConfigurationToken->data, sizeof(p_req->ConfigurationToken)-1);
	}
	
	p_ProfileToken = xml_node_soap_get(p_node, "ProfileToken");
	if (p_ProfileToken && p_ProfileToken->data)
	{
		p_req->ProfileTokenFlag = 1;
		strncpy(p_req->ProfileToken, p_ProfileToken->data, sizeof(p_req->ProfileToken)-1);
	}
	
	return ONVIF_OK;
}

ONVIF_RET parse_SetImagingSettings(XMLN * p_node, SetImagingSettings_REQ * p_req)
{
	XMLN * p_VideoSourceToken;
	XMLN * p_ImagingSettings;
	XMLN * p_BacklightCompensation;
	XMLN * p_Brightness;
	XMLN * p_ColorSaturation;
	XMLN * p_Contrast;
	XMLN * p_Exposure;
	XMLN * p_Focus;
	XMLN * p_IrCutFilter;
	XMLN * p_Sharpness;
	XMLN * p_WideDynamicRange;
	XMLN * p_WhiteBalance;
	XMLN * p_ForcePersistence;

	////    add by xieqingpu
	XMLN * p_Extension;
	XMLN * p_ThermalSettings;
	XMLN * p_DulaInfoSettings;
	
	p_VideoSourceToken = xml_node_soap_get(p_node, "VideoSourceToken");
    if (p_VideoSourceToken && p_VideoSourceToken->data)
    {
        strncpy(p_req->VideoSourceToken, p_VideoSourceToken->data, sizeof(p_req->VideoSourceToken)-1);
    }
    else
    {
    	return ONVIF_ERR_MissingAttribute;
    }

    p_ImagingSettings = xml_node_soap_get(p_node, "ImagingSettings"); 
    if (NULL == p_ImagingSettings)
    {
    	return ONVIF_ERR_MissingAttribute;
    }

    p_BacklightCompensation = xml_node_soap_get(p_ImagingSettings, "BacklightCompensation");
    if (p_BacklightCompensation)
    {
    	XMLN * p_Mode;
		XMLN * p_Level;

		p_req->ImagingSettings.BacklightCompensationFlag = 1;
		
		p_Mode = xml_node_soap_get(p_BacklightCompensation, "Mode");
    	if (p_Mode && p_Mode->data)
    	{
    		p_req->ImagingSettings.BacklightCompensation.Mode = onvif_StringToBacklightCompensationMode(p_Mode->data);
    	}

    	p_Level = xml_node_soap_get(p_BacklightCompensation, "Level");
    	if (p_Level && p_Level->data)
    	{
    		p_req->ImagingSettings.BacklightCompensation.LevelFlag = 1;
    	    p_req->ImagingSettings.BacklightCompensation.Level = (float)atof(p_Level->data);
    	}
    }

    p_Brightness = xml_node_soap_get(p_ImagingSettings, "Brightness");			//亮度
    if (p_Brightness && p_Brightness->data)
    {
    	p_req->ImagingSettings.BrightnessFlag = 1;
    	p_req->ImagingSettings.Brightness = (float)atof(p_Brightness->data);
    }

    p_ColorSaturation = xml_node_soap_get(p_ImagingSettings, "ColorSaturation");		//饱和度
    if (p_ColorSaturation && p_ColorSaturation->data)
    {
    	p_req->ImagingSettings.ColorSaturationFlag = 1;
    	p_req->ImagingSettings.ColorSaturation = (float)atof(p_ColorSaturation->data);
    }

    p_Contrast = xml_node_soap_get(p_ImagingSettings, "Contrast");		//对比度
    if (p_Contrast && p_Contrast->data)
    {
    	p_req->ImagingSettings.ContrastFlag = 1;
    	p_req->ImagingSettings.Contrast = (float)atof(p_Contrast->data);
    }

	    p_Sharpness = xml_node_soap_get(p_ImagingSettings, "Sharpness");		//锐度
    if (p_Sharpness && p_Sharpness->data)
    {
    	p_req->ImagingSettings.SharpnessFlag = 1;
    	p_req->ImagingSettings.Sharpness = (float)atof(p_Sharpness->data);
    }

    p_Exposure = xml_node_soap_get(p_ImagingSettings, "Exposure");
    if (p_Exposure)
    {
    	XMLN * p_Mode;
		XMLN * p_Priority;
		XMLN * p_Window;
		XMLN * p_MinExposureTime;
		XMLN * p_MaxExposureTime;
		XMLN * p_MinGain;
		XMLN * p_MaxGain;
		XMLN * p_MinIris;
		XMLN * p_MaxIris;
		XMLN * p_ExposureTime;
		XMLN * p_Gain;
		XMLN * p_Iris;

		p_req->ImagingSettings.ExposureFlag = 1;
		
		p_Mode = xml_node_soap_get(p_Exposure, "Mode");
    	if (p_Mode && p_Mode->data)
    	{
    		p_req->ImagingSettings.Exposure.Mode = onvif_StringToExposureMode(p_Mode->data);
    	}

        p_Priority = xml_node_soap_get(p_Exposure, "Priority");
    	if (p_Priority && p_Priority->data)
    	{
    		p_req->ImagingSettings.Exposure.PriorityFlag = 1;
    		p_req->ImagingSettings.Exposure.Priority = onvif_StringToExposurePriority(p_Priority->data);
    	}

    	p_Window = xml_node_soap_get(p_Exposure, "Window");
    	if (p_Window)
    	{
    		const char * p_bottom;
    		const char * p_top;
    		const char * p_right;
    		const char * p_left;

    	    p_bottom = xml_attr_get(p_Window, "bottom");
    	    if (p_bottom)
    	    {
    	        p_req->ImagingSettings.Exposure.Window.bottom = (float)atof(p_bottom);
    	    }

    	    p_top = xml_attr_get(p_Window, "top");
    	    if (p_top)
    	    {
    	        p_req->ImagingSettings.Exposure.Window.top = (float)atof(p_top);
    	    }

    	    p_right = xml_attr_get(p_Window, "right");
    	    if (p_right)
    	    {
    	        p_req->ImagingSettings.Exposure.Window.right = (float)atof(p_right);
    	    }

    	    p_left = xml_attr_get(p_Window, "left");
    	    if (p_left)
    	    {
    	        p_req->ImagingSettings.Exposure.Window.left = (float)atof(p_left);
    	    }
    	}
    	
    	p_MinExposureTime = xml_node_soap_get(p_Exposure, "MinExposureTime");
    	if (p_MinExposureTime && p_MinExposureTime->data)
    	{
    		p_req->ImagingSettings.Exposure.MinExposureTimeFlag = 1;
    		p_req->ImagingSettings.Exposure.MinExposureTime = (float)atof(p_MinExposureTime->data);
    	}

    	p_MaxExposureTime = xml_node_soap_get(p_Exposure, "MaxExposureTime");
    	if (p_MaxExposureTime && p_MaxExposureTime->data)
    	{
    		p_req->ImagingSettings.Exposure.MaxExposureTimeFlag = 1;
    		p_req->ImagingSettings.Exposure.MaxExposureTime = (float)atof(p_MaxExposureTime->data);
    	}

    	p_MinGain = xml_node_soap_get(p_Exposure, "MinGain");
    	if (p_MinGain && p_MinGain->data)
    	{
    		p_req->ImagingSettings.Exposure.MinGainFlag = 1;
    		p_req->ImagingSettings.Exposure.MinGain = (float)atof(p_MinGain->data);
    	}

    	p_MaxGain = xml_node_soap_get(p_Exposure, "MaxGain");
    	if (p_MaxGain && p_MaxGain->data)
    	{
    		p_req->ImagingSettings.Exposure.MaxGainFlag = 1;
    		p_req->ImagingSettings.Exposure.MaxGain = (float)atof(p_MaxGain->data);
    	}

    	p_MinIris = xml_node_soap_get(p_Exposure, "MinIris");
    	if (p_MinIris && p_MinIris->data)
    	{
    		p_req->ImagingSettings.Exposure.MinIrisFlag = 1;
    		p_req->ImagingSettings.Exposure.MinIris = (float)atof(p_MinIris->data);
    	}

    	p_MaxIris = xml_node_soap_get(p_Exposure, "MaxIris");
    	if (p_MaxIris && p_MaxIris->data)
    	{
    		p_req->ImagingSettings.Exposure.MaxIrisFlag = 1;
    		p_req->ImagingSettings.Exposure.MaxIris = (float)atof(p_MaxIris->data);
    	}

    	p_ExposureTime = xml_node_soap_get(p_Exposure, "ExposureTime");
    	if (p_ExposureTime && p_ExposureTime->data)
    	{
    		p_req->ImagingSettings.Exposure.ExposureTimeFlag = 1;
    		p_req->ImagingSettings.Exposure.ExposureTime = (float)atof(p_ExposureTime->data);
    	}

    	p_Gain = xml_node_soap_get(p_Exposure, "Gain");
    	if (p_Gain && p_Gain->data)
    	{
    		p_req->ImagingSettings.Exposure.GainFlag = 1;
    		p_req->ImagingSettings.Exposure.Gain = (float)atof(p_Gain->data);
    	}

    	p_Iris = xml_node_soap_get(p_Exposure, "Iris");
    	if (p_Iris && p_Iris->data)
    	{
    		p_req->ImagingSettings.Exposure.IrisFlag = 1;
    		p_req->ImagingSettings.Exposure.Iris = (float)atof(p_Iris->data);
    	}
    }

    p_Focus = xml_node_soap_get(p_ImagingSettings, "Focus");
    if (p_Focus)
    {
        XMLN * p_AutoFocusMode;
		XMLN * p_DefaultSpeed;
		XMLN * p_NearLimit;
		XMLN * p_FarLimit;

		p_req->ImagingSettings.FocusFlag = 1;
		
		p_AutoFocusMode = xml_node_soap_get(p_Focus, "AutoFocusMode");
    	if (p_AutoFocusMode && p_AutoFocusMode->data)
    	{
    		p_req->ImagingSettings.Focus.AutoFocusMode = onvif_StringToAutoFocusMode(p_AutoFocusMode->data);
    	}

    	p_DefaultSpeed = xml_node_soap_get(p_Focus, "DefaultSpeed");
    	if (p_DefaultSpeed && p_DefaultSpeed->data)
    	{
    		p_req->ImagingSettings.Focus.DefaultSpeedFlag = 1;
    	    p_req->ImagingSettings.Focus.DefaultSpeed = (float)atof(p_DefaultSpeed->data);
    	}

    	p_NearLimit = xml_node_soap_get(p_Focus, "NearLimit");
    	if (p_NearLimit && p_NearLimit->data)
    	{
    		p_req->ImagingSettings.Focus.NearLimitFlag = 1;
    	    p_req->ImagingSettings.Focus.NearLimit = (float)atof(p_NearLimit->data);
    	}

    	p_FarLimit = xml_node_soap_get(p_Focus, "FarLimit");
    	if (p_FarLimit && p_FarLimit->data)
    	{
    		p_req->ImagingSettings.Focus.FarLimitFlag = 1;
    	    p_req->ImagingSettings.Focus.FarLimit = (float)atof(p_FarLimit->data);
    	}
    }
    
    p_IrCutFilter = xml_node_soap_get(p_ImagingSettings, "IrCutFilter");
    if (p_IrCutFilter && p_IrCutFilter->data)
    {
    	p_req->ImagingSettings.IrCutFilterFlag = 1;
    	p_req->ImagingSettings.IrCutFilter = onvif_StringToIrCutFilterMode(p_IrCutFilter->data);
    }

    /* p_Sharpness = xml_node_soap_get(p_ImagingSettings, "Sharpness");		//锐度
    if (p_Sharpness && p_Sharpness->data)
    {
    	p_req->ImagingSettings.SharpnessFlag = 1;
    	p_req->ImagingSettings.Sharpness = (float)atof(p_Sharpness->data);
    } */

    p_WideDynamicRange = xml_node_soap_get(p_ImagingSettings, "WideDynamicRange");
    if (p_WideDynamicRange)
    {
    	XMLN * p_Mode;
		XMLN * p_Level;

		p_req->ImagingSettings.WideDynamicRangeFlag = 1;
		
		p_Mode = xml_node_soap_get(p_WideDynamicRange, "Mode");
    	if (p_Mode && p_Mode->data)
    	{
    		p_req->ImagingSettings.WideDynamicRange.Mode = onvif_StringToWideDynamicMode(p_Mode->data);
    	}

    	p_Level = xml_node_soap_get(p_WideDynamicRange, "Level");
    	if (p_Level && p_Level->data)
    	{
    		p_req->ImagingSettings.WideDynamicRange.LevelFlag = 1;
    		p_req->ImagingSettings.WideDynamicRange.Level = (float)atof(p_Level->data);
    	}
    }

    p_WhiteBalance = xml_node_soap_get(p_ImagingSettings, "WhiteBalance");
    if (p_WhiteBalance)
    {
    	XMLN * p_Mode;
		XMLN * p_CrGain;
		XMLN * p_CbGain;

		p_req->ImagingSettings.WhiteBalanceFlag = 1;
		
		p_Mode = xml_node_soap_get(p_WhiteBalance, "Mode");
    	if (p_Mode && p_Mode->data)
    	{
    		p_req->ImagingSettings.WhiteBalance.Mode = onvif_StringToWhiteBalanceMode(p_Mode->data);
    	}

    	p_CrGain = xml_node_soap_get(p_WhiteBalance, "CrGain");
    	if (p_CrGain && p_CrGain->data)
    	{
    		p_req->ImagingSettings.WhiteBalance.CrGainFlag = 1;
    	    p_req->ImagingSettings.WhiteBalance.CrGain = (float)atof(p_CrGain->data);
    	}

    	p_CbGain = xml_node_soap_get(p_WhiteBalance, "CbGain");
    	if (p_CbGain && p_CbGain->data)
    	{
    		p_req->ImagingSettings.WhiteBalance.CbGainFlag = 1;
    	    p_req->ImagingSettings.WhiteBalance.CbGain = (float)atof(p_CbGain->data);
    	}
    }

	/////  扩展			add by xieqingpu
    p_Extension = xml_node_soap_get(p_ImagingSettings, "Extension");
	if (NULL == p_Extension)
    {
    	return ONVIF_OK;
    }
	
	p_ThermalSettings = xml_node_soap_get(p_Extension, "ThermalSettings");
	if (p_ThermalSettings)
    {
			//ThermalSet1
			XMLN * p_UserPalette; 			//色板
			XMLN * p_WideDynamic; 	   //宽动态
			XMLN * p_OrgData; 		//数据源
			XMLN * p_Actime; 		 //自动校正间隔
			//ThermalSet2
			XMLN * p_Emissivity; 		//发射率
			XMLN * p_Distance; 	   		 //距离
			XMLN * p_Humidity; 			//湿度
			XMLN * p_Correction;  	   //修正
			XMLN * p_Reflection; 		//反射温度 Amb
			XMLN * p_Amb; 		       	     //环境温度 

			p_req->ImagingSettings.ThermalSettings_extFlag = 1;    ////

			 /* ************ ThermalSet1  *************/
			p_UserPalette = xml_node_soap_get(p_ThermalSettings, "UserPalette");		//色板（1-12种色板）
			if (p_UserPalette && p_UserPalette->data)		
			{
				// printf("UserPalette != NULL\n");
				p_req->ImagingSettings.ThermalSettings.ThermalSet_ext1Flag = 1;			////
				p_req->ImagingSettings.ThermalSettings.ThermalSet1.UserPalette = (int)atoi(p_UserPalette->data);
			}
			else{
				// printf("UserPalette == NULL\n");
				p_req->ImagingSettings.ThermalSettings.ThermalSet_ext1Flag = 0;			////
			}
			
			p_WideDynamic = xml_node_soap_get(p_ThermalSettings, "WideDynamic");	//宽动态（开关）
			if (p_WideDynamic && p_WideDynamic->data)
			{
				// BOOL dynamic  = parse_Bool(p_WideDynamic->data);
				// printf("WideDynamic 宽动态:%d (0：关 or 1：开)\n", dynamic);
				p_req->ImagingSettings.ThermalSettings.ThermalSet1.WideDynamic = (int)atoi(p_WideDynamic->data);
			}
			p_OrgData = xml_node_soap_get(p_ThermalSettings, "OrgData");		//数据源(0：原始数据，1：YUV数据)
			if (p_OrgData && p_OrgData->data)
			{
				p_req->ImagingSettings.ThermalSettings.ThermalSet1.OrgData = (int)atoi(p_OrgData->data);
			}
			p_Actime = xml_node_soap_get(p_ThermalSettings, "Actime");			//自动校正间隔
			if (p_Actime && p_Actime->data)
			{
				p_req->ImagingSettings.ThermalSettings.ThermalSet1.Actime = (int)atoi(p_Actime->data);
			}

			 /* ************ ThermalSet2  *************/
			p_Emissivity = xml_node_soap_get(p_ThermalSettings, "Emissivity");			//发射率
			if (p_Emissivity && p_Emissivity->data)
			{
				// printf("p_Emissivity != NULL\n");
				p_req->ImagingSettings.ThermalSettings.ThermalSet_ext2Flag = 1;		////
				p_req->ImagingSettings.ThermalSettings.ThermalSet2.Emissivity = (float)atof(p_Emissivity->data);
			}
			else{
				// printf("p_Emissivity == NULL\n");
				p_req->ImagingSettings.ThermalSettings.ThermalSet_ext2Flag = 0 ;		////
			}
			p_Distance = xml_node_soap_get(p_ThermalSettings, "Distance");			//距离
			if (p_Distance && p_Distance->data)
			{
				// printf("Distance 距离:%0.2f \n", (float)atof(p_Distance->data));
				p_req->ImagingSettings.ThermalSettings.ThermalSet2.Distance = (float)atof(p_Distance->data);
			}
			p_Humidity = xml_node_soap_get(p_ThermalSettings, "Humidity");			//湿度
			if (p_Humidity && p_Humidity->data)
			{
				p_req->ImagingSettings.ThermalSettings.ThermalSet2.Humidity = (float)atof(p_Humidity->data);
			}
			p_Correction = xml_node_soap_get(p_ThermalSettings, "Correction");		//修正
			if (p_Correction && p_Correction->data)
			{
				p_req->ImagingSettings.ThermalSettings.ThermalSet2.Correction = (float)atof(p_Correction->data);
			}
			p_Reflection = xml_node_soap_get(p_ThermalSettings, "Reflection");		//反射温度
			if (p_Reflection && p_Reflection->data)
			{
				p_req->ImagingSettings.ThermalSettings.ThermalSet2.Reflection = (float)atof(p_Reflection->data);
			}
			p_Amb = xml_node_soap_get(p_ThermalSettings, "Amb");		//环境温度
			if (p_Amb && p_Amb->data)
			{
				p_req->ImagingSettings.ThermalSettings.ThermalSet2.Amb = (float)atof(p_Amb->data);
			}
    }

	p_DulaInfoSettings = xml_node_soap_get(p_Extension, "DulaInfoSettings");
	if (p_DulaInfoSettings)
	{
		XMLN * p_Focal; 	
		XMLN * p_Lens; 	
		XMLN * p_Distance; 	
		XMLN * p_DulaModel; 	
		XMLN * p_X; 	
		XMLN * p_Y; 	
		XMLN * p_Scale; 	

		p_req->ImagingSettings.DulaInformationFlag = 1;    ////

		p_Focal = xml_node_soap_get(p_DulaInfoSettings, "Focal");
		if (p_Focal && p_Focal->data)
		{
			p_req->ImagingSettings.DulaInfo.focal = (int)atoi(p_Focal->data);
		}else{
			p_req->ImagingSettings.DulaInfo.focal = -1;
		}
		p_Lens = xml_node_soap_get(p_DulaInfoSettings, "Lens");
		if (p_Lens && p_Lens->data)
		{
			p_req->ImagingSettings.DulaInfo.lens = (float)atof(p_Lens->data);
		}else{
			p_req->ImagingSettings.DulaInfo.lens = -1;
		}
		p_Distance = xml_node_soap_get(p_DulaInfoSettings, "Distance");
		if (p_Distance && p_Distance->data)
		{
			p_req->ImagingSettings.DulaInfo.distance = (float)atof(p_Distance->data);
		}else{
			p_req->ImagingSettings.DulaInfo.distance = -1;
		}
		p_DulaModel = xml_node_soap_get(p_DulaInfoSettings, "DulaModel");
		if (p_DulaModel && p_DulaModel->data)
		{
			p_req->ImagingSettings.DulaInfo.dula_model = (int)atoi(p_DulaModel->data);
		}else{
			p_req->ImagingSettings.DulaInfo.dula_model = -1;
		}

		p_X = xml_node_soap_get(p_DulaInfoSettings, "X");
		if (p_X && p_X->data)
		{
			p_req->ImagingSettings.DulaInfo.x = (signed short int)atoi(p_X->data);
		}else{
			p_req->ImagingSettings.DulaInfo.x = -1;
		}
		p_Y = xml_node_soap_get(p_DulaInfoSettings, "Y");
		if (p_Y && p_Y->data)
		{
			p_req->ImagingSettings.DulaInfo.y = (signed short int)atoi(p_Y->data);
		}else{
			p_req->ImagingSettings.DulaInfo.y = -1;
		}
		p_Scale = xml_node_soap_get(p_DulaInfoSettings, "Scale");
		if (p_Scale && p_Scale->data)
		{
			p_req->ImagingSettings.DulaInfo.scale = (float)atof(p_Scale->data);
		}else{
			p_req->ImagingSettings.DulaInfo.scale = -1;
		}
	}

   /////end 扩展  

    p_ForcePersistence = xml_node_soap_get(p_node, "ForcePersistence");
    if (p_ForcePersistence && p_ForcePersistence->data)
    {
    	p_req->ForcePersistenceFlag = 1;
    	p_req->ForcePersistence = parse_Bool(p_ForcePersistence->data);
    }
    
	return ONVIF_OK;
}

ONVIF_RET parse_Move(XMLN * p_node, Move_REQ * p_req)
{
	XMLN * p_VideoSourceToken;
	XMLN * p_Focus;
	XMLN * p_Absolute;
	XMLN * p_Relative;
	XMLN * p_Continuous;
	
	p_VideoSourceToken = xml_node_soap_get(p_node, "VideoSourceToken");
    if (p_VideoSourceToken && p_VideoSourceToken->data)
    {
        strncpy(p_req->VideoSourceToken, p_VideoSourceToken->data, sizeof(p_req->VideoSourceToken)-1);
    }

    p_Focus = xml_node_soap_get(p_node, "Focus");
    if (NULL == p_Focus)
    {
    	return ONVIF_ERR_MissingAttribute;
    }

    p_Absolute = xml_node_soap_get(p_Focus, "Absolute");
    if (p_Absolute)
    {
    	XMLN * p_Position;
		XMLN * p_Speed;
		
    	p_req->Focus.AbsoluteFlag = 1;
    	
    	p_Position = xml_node_soap_get(p_Absolute, "Position");
    	if (p_Position && p_Position->data)
    	{
    		p_req->Focus.Absolute.Position = (float)atof(p_Position->data);
    	}

    	p_Speed = xml_node_soap_get(p_Absolute, "Speed");
    	if (p_Speed && p_Speed->data)
    	{
    		p_req->Focus.Absolute.SpeedFlag = 1;
    		p_req->Focus.Absolute.Speed = (float)atof(p_Speed->data);
    	}
    }
    
	p_Relative = xml_node_soap_get(p_Focus, "Relative");
	if (p_Relative)
	{
		XMLN * p_Distance;
		XMLN * p_Speed;
		
		p_req->Focus.RelativeFlag = 1;
		
		p_Distance = xml_node_soap_get(p_Relative, "Distance");
		if (p_Distance && p_Distance->data)
		{
			p_req->Focus.Relative.Distance = (float)atof(p_Distance->data);
		}

		p_Speed = xml_node_soap_get(p_Relative, "Speed");
		if (p_Speed && p_Speed->data)
		{
			p_req->Focus.Relative.SpeedFlag = 1;			
			p_req->Focus.Relative.Speed = (float)atof(p_Speed->data);
		}
	}

	p_Continuous = xml_node_soap_get(p_Focus, "Continuous");
	if (p_Continuous)
	{
		XMLN * p_Speed;
		
		p_req->Focus.ContinuousFlag = 1;
		
		p_Speed = xml_node_soap_get(p_Continuous, "Speed");
		if (p_Speed && p_Speed->data)
		{
			p_req->Focus.Continuous.Speed = (float)atof(p_Speed->data);
		}
	}

	return ONVIF_OK;
}

ONVIF_RET prase_User(XMLN * p_node, onvif_User * p_req)
{
    XMLN * p_Username;
	XMLN * p_Password;
	XMLN * p_UserLevel;

	p_Username = xml_node_soap_get(p_node, "Username");
	if (p_Username && p_Username->data)
	{
		strncpy(p_req->Username, p_Username->data, sizeof(p_req->Username)-1);
	}

	p_Password = xml_node_soap_get(p_node, "Password");
	if (p_Password && p_Password->data)
	{
		if (strlen(p_Password->data) >= sizeof(p_req->Password))
		{
			return ONVIF_ERR_PasswordTooLong;
		}
		
		strncpy(p_req->Password, p_Password->data, sizeof(p_req->Password)-1);
	}

	p_UserLevel = xml_node_soap_get(p_node, "UserLevel");
	if (p_UserLevel && p_UserLevel->data)
	{
		p_req->UserLevel = onvif_StringToUserLevel(p_UserLevel->data);
	}

	return ONVIF_OK;
}

ONVIF_RET parse_CreateUsers(XMLN * p_node, CreateUsers_REQ * p_req)
{
	int i = 0;
	ONVIF_RET ret;
	
	XMLN * p_User = xml_node_soap_get(p_node, "User");
	while (p_User)
	{
		if (i < ARRAY_SIZE(p_req->User))
		{
		    ret = prase_User(p_User, &p_req->User[i]);
			if (ONVIF_OK != ret)
			{
			    return ret;
			}

			++i;
		}
		else
		{
			return ONVIF_ERR_TooManyUsers;
		}
		
		p_User = p_User->next;
	}

	return ONVIF_OK;
}

ONVIF_RET parse_DeleteUsers(XMLN * p_node, DeleteUsers_REQ * p_req)
{
	int i = 0;
	
	XMLN * p_Username = xml_node_soap_get(p_node, "Username");
	while (p_Username)
	{
		if (i < ARRAY_SIZE(p_req->Username))
		{
			strncpy(p_req->Username[i], p_Username->data, sizeof(p_req->Username[i])-1);

			++i;
		}
		else
		{
			break;
		}
		
		p_Username = p_Username->next;
	}

	return ONVIF_OK;
}

ONVIF_RET parse_SetUser(XMLN * p_node, SetUser_REQ * p_req)
{
    int i = 0;
	ONVIF_RET ret;
	
	XMLN * p_User = xml_node_soap_get(p_node, "User");
	while (p_User)
	{
		if (i < ARRAY_SIZE(p_req->User))
		{
		    ret = prase_User(p_User, &p_req->User[i]);
			if (ONVIF_OK != ret)
			{
			    return ret;
			}

			++i;
		}
		else
		{
			return ONVIF_ERR_TooManyUsers;
		}
		
		p_User = p_User->next;
	}

	return ONVIF_OK;
}

ONVIF_RET parse_SetRemoteUser(XMLN * p_node, SetRemoteUser_REQ * p_req)
{
	XMLN * p_RemoteUser = xml_node_soap_get(p_node, "RemoteUser");
	if (p_RemoteUser)
	{
		XMLN * p_Username;
		XMLN * p_Password;
		XMLN * p_UseDerivedPassword;

		p_req->RemoteUserFlag = 1;

		p_Username = xml_node_soap_get(p_RemoteUser, "Username");
    	if (p_Username && p_Username->data)
    	{
    	    strncpy(p_req->RemoteUser.Username, p_Username->data, sizeof(p_req->RemoteUser.Username)-1);
    	}

    	p_Password = xml_node_soap_get(p_RemoteUser, "Password");
    	if (p_Password && p_Password->data)
    	{
    	    p_req->RemoteUser.PasswordFlag = 1;
    	    strncpy(p_req->RemoteUser.Password, p_Password->data, sizeof(p_req->RemoteUser.Password)-1);
    	}

    	p_UseDerivedPassword = xml_node_soap_get(p_RemoteUser, "UseDerivedPassword");
    	if (p_UseDerivedPassword && p_UseDerivedPassword->data)
    	{
    	    p_req->RemoteUser.UseDerivedPassword = parse_Bool(p_UseDerivedPassword->data);
    	}
	}

	return ONVIF_OK;
}

#ifdef IPFILTER_SUPPORT

BOOL parse_PrefixedIPAddress(XMLN * p_node, onvif_PrefixedIPAddress * p_req)
{
    XMLN * p_Address;
    XMLN * p_PrefixLength;

    p_Address = xml_node_soap_get(p_node, "Address");
	if (p_Address && p_Address->data)
	{
	    strncpy(p_req->Address, p_Address->data, sizeof(p_req->Address)-1);
	}

	p_PrefixLength = xml_node_soap_get(p_node, "PrefixLength");
	if (p_PrefixLength && p_PrefixLength->data)
	{
	    p_req->PrefixLength = atoi(p_PrefixLength->data);
	}

	return TRUE;
}

ONVIF_RET parse_IPAddressFilter(XMLN * p_node, onvif_IPAddressFilter * p_req)
{
    int idx = 0;
    XMLN * p_Type;
    XMLN * p_IPv4Address;
    XMLN * p_IPv6Address;

    p_Type = xml_node_soap_get(p_node, "Type");
	if (p_Type && p_Type->data)
	{
	    p_req->Type = onvif_StringToIPAddressFilterType(p_Type->data);
	}

    idx = 0;
    
	p_IPv4Address = xml_node_soap_get(p_node, "IPv4Address");
	while (p_IPv4Address && soap_strcmp(p_IPv4Address->name, "IPv4Address") == 0)
	{
	    parse_PrefixedIPAddress(p_IPv4Address, &p_req->IPv4Address[idx]);

        if (++idx >= ARRAY_SIZE(p_req->IPv4Address))
        {
            break;
        }
        
	    p_IPv4Address = p_IPv4Address->next;
	}

    idx = 0;
    
	p_IPv6Address = xml_node_soap_get(p_node, "IPv6Address");
	while (p_IPv6Address && soap_strcmp(p_IPv6Address->name, "IPv6Address") == 0)
	{
	    parse_PrefixedIPAddress(p_IPv6Address, &p_req->IPv6Address[idx]);

        if (++idx >= ARRAY_SIZE(p_req->IPv6Address))
        {
            break;
        }
        
	    p_IPv6Address = p_IPv6Address->next;
	}

    return ONVIF_OK;
}

ONVIF_RET parse_SetIPAddressFilter(XMLN * p_node, SetIPAddressFilter_REQ * p_req)
{
    ONVIF_RET ret = ONVIF_OK;
    XMLN * p_IPAddressFilter;

    p_IPAddressFilter = xml_node_soap_get(p_node, "IPAddressFilter");
	if (p_IPAddressFilter)
	{
	    ret = parse_IPAddressFilter(p_IPAddressFilter, &p_req->IPAddressFilter);
	}

	return ret;
}

ONVIF_RET parse_AddIPAddressFilter(XMLN * p_node, AddIPAddressFilter_REQ * p_req)
{
    ONVIF_RET ret = ONVIF_OK;
    XMLN * p_IPAddressFilter;

    p_IPAddressFilter = xml_node_soap_get(p_node, "IPAddressFilter");
	if (p_IPAddressFilter)
	{
	    ret = parse_IPAddressFilter(p_IPAddressFilter, &p_req->IPAddressFilter);
	}

	return ret;
}

ONVIF_RET parse_RemoveIPAddressFilter(XMLN * p_node, RemoveIPAddressFilter_REQ * p_req)
{
    ONVIF_RET ret = ONVIF_OK;
    XMLN * p_IPAddressFilter;

    p_IPAddressFilter = xml_node_soap_get(p_node, "IPAddressFilter");
	if (p_IPAddressFilter)
	{
	    ret = parse_IPAddressFilter(p_IPAddressFilter, &p_req->IPAddressFilter);
	}

	return ret;
}

#endif // end of #ifdef IPFILTER_SUPPORT

ONVIF_RET parse_GetOSDs(XMLN * p_node, GetOSDs_REQ * p_req)
{
	XMLN * p_ConfigurationToken = xml_node_soap_get(p_node, "ConfigurationToken");
	if (p_ConfigurationToken && p_ConfigurationToken->data)
	{
		p_req->ConfigurationTokenFlag = 1;
		strncpy(p_req->ConfigurationToken, p_ConfigurationToken->data, sizeof(p_req->ConfigurationToken)-1);
	}

	return ONVIF_OK;
}

ONVIF_RET parse_GetOSD(XMLN * p_node, GetOSD_REQ * p_req)
{
	XMLN * p_OSDToken = xml_node_soap_get(p_node, "OSDToken");
	if (p_OSDToken && p_OSDToken->data)
	{
		strncpy(p_req->OSDToken, p_OSDToken->data, sizeof(p_req->OSDToken)-1);
	}

	return ONVIF_OK;
}

ONVIF_RET parse_OSDColor(XMLN * p_node, onvif_OSDColor * p_req)
{
	XMLN * p_Color;
	const char * p_Transparent;

	p_Transparent = xml_attr_get(p_node, "Transparent");
	if (p_Transparent)
	{
		p_req->TransparentFlag = 1;
		p_req->Transparent = atoi(p_Transparent);
	}

	p_Color = xml_node_soap_get(p_node, "Color");
	if (p_Color)
	{
		const char * p_X;
		const char * p_Y;
		const char * p_Z;

		p_X = xml_attr_get(p_Color, "X");
		if (p_X)
		{
			p_req->X = (float) atof(p_X);
		}

		p_Y = xml_attr_get(p_Color, "Y");
		if (p_X)
		{
			p_req->Y = (float) atof(p_Y);
		}

		p_Z = xml_attr_get(p_Color, "Z");
		if (p_Z)
		{
			p_req->Z = (float) atof(p_Z);
		}
	}

	return ONVIF_OK;
}

ONVIF_RET parse_OSDConfiguration(XMLN * p_node, onvif_OSDConfiguration * p_req)
{
	XMLN * p_OSD;
	XMLN * p_VideoSourceConfigurationToken;
	XMLN * p_Type;
	XMLN * p_Position;
	XMLN * p_TextString;
	XMLN * p_Image;
	const char * p_token;

	p_OSD = xml_node_soap_get(p_node, "OSD");
	if (NULL == p_OSD)
	{
		return ONVIF_ERR_MissingAttribute;
	}

	p_token = xml_attr_get(p_OSD, "token");
	if (p_token)
	{
		strncpy(p_req->token, p_token, sizeof(p_req->token)-1);
	}

	p_VideoSourceConfigurationToken = xml_node_soap_get(p_OSD, "VideoSourceConfigurationToken");
	if (p_VideoSourceConfigurationToken && p_VideoSourceConfigurationToken->data)
	{
		strncpy(p_req->VideoSourceConfigurationToken, p_VideoSourceConfigurationToken->data, sizeof(p_req->VideoSourceConfigurationToken)-1);
	}

	p_Type = xml_node_soap_get(p_OSD, "Type");
	if (p_Type && p_Type->data)
	{
		p_req->Type = onvif_StringToOSDType(p_Type->data);
	}

	p_Position = xml_node_soap_get(p_OSD, "Position");
	if (p_Position)
	{
		XMLN * p_Type;
		XMLN * p_Pos;

		p_Type = xml_node_soap_get(p_Position, "Type");
		if (p_Type && p_Type->data)
		{
			p_req->Position.Type = onvif_StringToOSDPosType(p_Type->data);
		}

		p_Pos = xml_node_soap_get(p_Position, "Pos");
		if (p_Pos)
		{
			p_req->Position.PosFlag = 1;
			parse_Vector(p_Pos, &p_req->Position.Pos);
		}
	}

	p_TextString = xml_node_soap_get(p_OSD, "TextString");
	if (p_TextString)
	{
		XMLN * p_Type;
		XMLN * p_DateFormat;
		XMLN * p_TimeFormat;
		XMLN * p_FontSize;
		XMLN * p_FontColor;
		XMLN * p_BackgroundColor;
		XMLN * p_PlainText;
		
		p_req->TextStringFlag = 1;
		
		p_Type = xml_node_soap_get(p_TextString, "Type");
		if (p_Type && p_Type->data)
		{
			p_req->TextString.Type = onvif_StringToOSDTextType(p_Type->data);
		}

		p_DateFormat = xml_node_soap_get(p_TextString, "DateFormat");
		if (p_DateFormat && p_DateFormat->data)
		{
			p_req->TextString.DateFormatFlag = 1;
			strncpy(p_req->TextString.DateFormat, p_DateFormat->data, sizeof(p_req->TextString.DateFormat)-1);
		}

		p_TimeFormat = xml_node_soap_get(p_TextString, "TimeFormat");
		if (p_TimeFormat && p_TimeFormat->data)
		{
			p_req->TextString.TimeFormatFlag = 1;
			strncpy(p_req->TextString.TimeFormat, p_TimeFormat->data, sizeof(p_req->TextString.TimeFormat)-1);
		}

		p_FontSize = xml_node_soap_get(p_TextString, "FontSize");
		if (p_FontSize && p_FontSize->data)
		{
			p_req->TextString.FontSizeFlag = 1;
			p_req->TextString.FontSize = atoi(p_FontSize->data);
		}

		p_FontColor = xml_node_soap_get(p_TextString, "FontColor");
		if (p_FontColor)
		{
			p_req->TextString.FontColorFlag = 1;
			
			parse_OSDColor(p_FontColor, &p_req->TextString.FontColor);
		}

		p_BackgroundColor = xml_node_soap_get(p_TextString, "BackgroundColor");
		if (p_BackgroundColor)
		{
			p_req->TextString.BackgroundColorFlag = 1;
			
			parse_OSDColor(p_BackgroundColor, &p_req->TextString.BackgroundColor);
		}

		p_PlainText = xml_node_soap_get(p_TextString, "PlainText");
		if (p_PlainText && p_PlainText->data)
		{
			p_req->TextString.PlainTextFlag = 1;
			strncpy(p_req->TextString.PlainText, p_PlainText->data, sizeof(p_req->TextString.PlainText)-1);
		}
	}

	p_Image = xml_node_soap_get(p_OSD, "Image");
	if (p_Image)
	{
		XMLN * p_ImgPath;
		
		p_req->ImageFlag = 1;
		
		p_ImgPath = xml_node_soap_get(p_Image, "ImgPath");
		if (p_ImgPath && p_ImgPath->data)
		{
			strncpy(p_req->Image.ImgPath, p_ImgPath->data, sizeof(p_req->Image.ImgPath)-1);
		}
	}

	return ONVIF_OK;
}

ONVIF_RET parse_SetOSD(XMLN * p_node, SetOSD_REQ * p_req)
{
	return parse_OSDConfiguration(p_node, &p_req->OSD);
}

ONVIF_RET parse_CreateOSD(XMLN * p_node, CreateOSD_REQ * p_req)
{
	return parse_OSDConfiguration(p_node, &p_req->OSD);
}

ONVIF_RET parse_DeleteOSD(XMLN * p_node, DeleteOSD_REQ * p_req)
{
	XMLN * p_OSDToken = xml_node_soap_get(p_node, "OSDToken");
	if (p_OSDToken && p_OSDToken->data)
	{
		strncpy(p_req->OSDToken, p_OSDToken->data, sizeof(p_req->OSDToken)-1);
	}

	return ONVIF_OK;
}

ONVIF_RET parse_FloatRange(XMLN * p_node, onvif_FloatRange * p_req)
{
	XMLN * p_Min;
	XMLN * p_Max;

	p_Min = xml_node_soap_get(p_node, "Min");
	if (p_Min && p_Min->data)
	{
		p_req->Min = (float)atof(p_Min->data);
	}
	
	p_Max = xml_node_soap_get(p_node, "Max");
	if (p_Max && p_Max->data)
	{
		p_req->Max = (float)atof(p_Max->data);
	}

	return ONVIF_OK;
}

ONVIF_RET parse_GetMetadataConfigurationOptions(XMLN * p_node, GetMetadataConfigurationOptions_REQ * p_req)
{
	XMLN * p_ConfigurationToken;
	XMLN * p_ProfileToken;

	p_ConfigurationToken = xml_node_soap_get(p_node, "ConfigurationToken");
	if (p_ConfigurationToken && p_ConfigurationToken->data)
	{
		p_req->ConfigurationTokenFlag = 1;
		strncpy(p_req->ConfigurationToken, p_ConfigurationToken->data, sizeof(p_req->ConfigurationToken)-1);
	}

	p_ProfileToken = xml_node_soap_get(p_node, "ProfileToken");
	if (p_ProfileToken && p_ProfileToken->data)
	{
		p_req->ProfileTokenFlag = 1;
		strncpy(p_req->ProfileToken, p_ProfileToken->data, sizeof(p_req->ProfileToken)-1);
	}

	return ONVIF_OK;
}

ONVIF_RET parse_MetadataConfiguration(XMLN * p_node, onvif_MetadataConfiguration * p_req)
{
    XMLN * p_Configuration;
	XMLN * p_Name;
	XMLN * p_PTZStatus;
	XMLN * p_Events;
	XMLN * p_Analytics;
	XMLN * p_SessionTimeout;
	const char * token;
	
	p_Configuration = xml_node_soap_get(p_node, "Configuration");
	if (NULL == p_Configuration)
	{
		return ONVIF_ERR_MissingAttribute;
	}
	
	token = xml_attr_get(p_Configuration, "token");
	if (token)
	{
		strncpy(p_req->token, token, sizeof(p_req->token)-1);
	}

	p_Name = xml_node_soap_get(p_Configuration, "Name");
	if (p_Name && p_Name->data)
	{
		strncpy(p_req->Name, p_Name->data, sizeof(p_req->Name)-1);
	}

	p_PTZStatus = xml_node_soap_get(p_Configuration, "PTZStatus");
	if (p_PTZStatus)
	{
		XMLN * p_Status;
		XMLN * p_Position;

		p_req->PTZStatusFlag = 1;
		
		p_Status = xml_node_soap_get(p_PTZStatus, "Status");
		if (p_Status && p_Status->data)
		{
			p_req->PTZStatus.Status = parse_Bool(p_Status->data);
		}

		p_Position = xml_node_soap_get(p_PTZStatus, "Position");
		if (p_Position && p_Position->data)
		{
			p_req->PTZStatus.Position = parse_Bool(p_Position->data);
		}
	}

    p_Events = xml_node_soap_get(p_Configuration, "Events");
	if (p_Events)
	{
	    XMLN * p_Filter;
	    
	    p_req->EventsFlag = 1;
        
	    p_Filter = xml_node_soap_get(p_Events, "Filter");
		if (p_Filter)
		{
		    XMLN * p_TopicExpression;
		    
		    p_TopicExpression = xml_node_soap_get(p_Filter, "TopicExpression");
    		if (p_TopicExpression && p_TopicExpression->data)
    		{
    		    const char * p_Dialect;

    		    p_Dialect = xml_attr_get(p_TopicExpression, "Dialect");
    		    if (p_Dialect)
    		    {
    		        strncpy(p_req->Events.Dialect, p_Dialect, sizeof(p_req->Events.Dialect)-1);
    		    }
    		    
			    strncpy(p_req->Events.TopicExpression, p_TopicExpression->data, sizeof(p_req->Events.TopicExpression)-1);
			}
		}
	}
	
	p_Analytics = xml_node_soap_get(p_Configuration, "Analytics");
	if (p_Analytics && p_Analytics->data)
	{
		p_req->AnalyticsFlag = 1;
		p_req->Analytics = parse_Bool(p_Analytics->data);
	}

	parse_MulticastConfiguration(p_Configuration, &p_req->Multicast);

	p_SessionTimeout = xml_node_soap_get(p_Configuration, "SessionTimeout");
	if (p_SessionTimeout && p_SessionTimeout->data)
	{
		parse_XSDDuration(p_SessionTimeout->data, &p_req->SessionTimeout);
	}
	
	return ONVIF_OK;
}

ONVIF_RET parse_SetMetadataConfiguration(XMLN * p_node, SetMetadataConfiguration_REQ * p_req)
{
	XMLN * p_ForcePersistence;

	parse_MetadataConfiguration(p_node, &p_req->Configuration);

	p_ForcePersistence = xml_node_soap_get(p_node, "ForcePersistence");
	if (p_ForcePersistence && p_ForcePersistence->data)
	{
		p_req->ForcePersistence = parse_Bool(p_ForcePersistence->data);
	}
	
	return ONVIF_OK;
}

ONVIF_RET parse_AddMetadataConfiguration(XMLN * p_node, AddMetadataConfiguration_REQ * p_req)
{
	XMLN * p_ProfileToken;
	XMLN * p_ConfigurationToken;

	p_ProfileToken = xml_node_soap_get(p_node, "ProfileToken");
	if (p_ProfileToken && p_ProfileToken->data)
	{
		strncpy(p_req->ProfileToken, p_ProfileToken->data, sizeof(p_req->ProfileToken)-1);
	}
	
	p_ConfigurationToken = xml_node_soap_get(p_node, "ConfigurationToken");
	if (p_ConfigurationToken && p_ConfigurationToken->data)
	{
		strncpy(p_req->ConfigurationToken, p_ConfigurationToken->data, sizeof(p_req->ConfigurationToken)-1);
	}
	
	return ONVIF_OK;
}

ONVIF_RET parse_AudioSourceConfiguration(XMLN * p_node, onvif_AudioSourceConfiguration * p_req)
{
	XMLN * p_Configuration;
	XMLN * p_Name;
	XMLN * p_UseCount;
	XMLN * p_SourceToken;
	
	p_Configuration = xml_node_soap_get(p_node, "Configuration");
	if (p_Configuration)
	{
		const char * token = xml_attr_get(p_Configuration, "token");
		if (token)
		{
			strncpy(p_req->token, token, sizeof(p_req->token)-1);
		}
		else
		{
			return ONVIF_ERR_MissingAttribute;
		}
	}
	else
	{
		return ONVIF_ERR_MissingAttribute;
	}

	p_Name = xml_node_soap_get(p_Configuration, "Name");
	if (p_Name && p_Name->data)
	{
		strncpy(p_req->Name, p_Name->data, sizeof(p_req->Name)-1);
	}

	p_UseCount = xml_node_soap_get(p_Configuration, "UseCount");
	if (p_UseCount && p_UseCount->data)
	{
		p_req->UseCount = atoi(p_UseCount->data);
	}

	p_SourceToken = xml_node_soap_get(p_Configuration, "SourceToken");
	if (p_SourceToken && p_SourceToken->data)
	{
		strncpy(p_req->SourceToken, p_SourceToken->data, sizeof(p_req->SourceToken)-1);
	}
	
	return ONVIF_OK;
}

ONVIF_RET parse_GetVideoSourceModes(XMLN * p_node, GetVideoSourceModes_REQ * p_req)
{
    XMLN * p_VideoSourceToken;

    p_VideoSourceToken = xml_node_soap_get(p_node, "VideoSourceToken");
	if (p_VideoSourceToken && p_VideoSourceToken->data)
	{
		strncpy(p_req->VideoSourceToken, p_VideoSourceToken->data, sizeof(p_req->VideoSourceToken)-1);
	}

	return ONVIF_OK;
}

ONVIF_RET parse_SetVideoSourceMode(XMLN * p_node, SetVideoSourceMode_REQ * p_req)
{
    XMLN * p_VideoSourceToken;
    XMLN * p_VideoSourceModeToken;

    p_VideoSourceToken = xml_node_soap_get(p_node, "VideoSourceToken");
	if (p_VideoSourceToken && p_VideoSourceToken->data)
	{
		strncpy(p_req->VideoSourceToken, p_VideoSourceToken->data, sizeof(p_req->VideoSourceToken)-1);
	}

	p_VideoSourceModeToken = xml_node_soap_get(p_node, "VideoSourceModeToken");
	if (p_VideoSourceModeToken && p_VideoSourceModeToken->data)
	{
		strncpy(p_req->VideoSourceModeToken, p_VideoSourceModeToken->data, sizeof(p_req->VideoSourceModeToken)-1);
	}

	return ONVIF_OK;
}



#ifdef AUDIO_SUPPORT

ONVIF_RET parse_AddAudioSourceConfiguration(XMLN * p_node, AddAudioSourceConfiguration_REQ * p_req)
{
	XMLN * p_ProfileToken;
	XMLN * p_ConfigurationToken;
	
	assert(p_node);

	p_ProfileToken = xml_node_soap_get(p_node, "ProfileToken");
	if (p_ProfileToken && p_ProfileToken->data)
	{
		strncpy(p_req->ProfileToken, p_ProfileToken->data, sizeof(p_req->ProfileToken)-1);
	}
	else
	{
		return ONVIF_ERR_MissingAttribute;
	}
	
	p_ConfigurationToken = xml_node_soap_get(p_node, "ConfigurationToken");
	if (p_ConfigurationToken && p_ConfigurationToken->data)
	{
		strncpy(p_req->ConfigurationToken, p_ConfigurationToken->data, sizeof(p_req->ConfigurationToken)-1);
	}
	else
	{
		return ONVIF_ERR_MissingAttribute;
	}
	
	return ONVIF_OK;
}

ONVIF_RET parse_AddAudioEncoderConfiguration(XMLN * p_node, AddAudioEncoderConfiguration_REQ * p_req)
{
	XMLN * p_ProfileToken;
	XMLN * p_ConfigurationToken;
	
	assert(p_node);

	p_ProfileToken = xml_node_soap_get(p_node, "ProfileToken");
	if (p_ProfileToken && p_ProfileToken->data)
	{
		strncpy(p_req->ProfileToken, p_ProfileToken->data, sizeof(p_req->ProfileToken)-1);
	}
	else
	{
		return ONVIF_ERR_MissingAttribute;
	}
	
	p_ConfigurationToken = xml_node_soap_get(p_node, "ConfigurationToken");
	if (p_ConfigurationToken && p_ConfigurationToken->data)
	{
		strncpy(p_req->ConfigurationToken, p_ConfigurationToken->data, sizeof(p_req->ConfigurationToken)-1);
	}
	else
	{
		return ONVIF_ERR_MissingAttribute;
	}
	
	return ONVIF_OK;
}

ONVIF_RET parse_GetAudioSourceConfigurationOptions(XMLN * p_node, GetAudioSourceConfigurationOptions_REQ * p_req)
{
	XMLN * p_ConfigurationToken;
	XMLN * p_ProfileToken;
	
	assert(p_node);

	p_ConfigurationToken = xml_node_soap_get(p_node, "ConfigurationToken");
	if (p_ConfigurationToken && p_ConfigurationToken->data)
	{
		p_req->ConfigurationTokenFlag = 1;
		strncpy(p_req->ConfigurationToken, p_ConfigurationToken->data, sizeof(p_req->ConfigurationToken)-1);
	}
	
	p_ProfileToken = xml_node_soap_get(p_node, "ProfileToken");
	if (p_ProfileToken && p_ProfileToken->data)
	{
		p_req->ProfileTokenFlag = 1;
		strncpy(p_req->ProfileToken, p_ProfileToken->data, sizeof(p_req->ProfileToken)-1);
	}
	
	return ONVIF_OK;
}

ONVIF_RET parse_SetAudioSourceConfiguration(XMLN * p_node, SetAudioSourceConfiguration_REQ * p_req)
{
	XMLN * p_ForcePersistence;
	
	parse_AudioSourceConfiguration(p_node, &p_req->Configuration);

	p_ForcePersistence = xml_node_soap_get(p_node, "ForcePersistence");
	if (p_ForcePersistence && p_ForcePersistence->data)
	{
		p_req->ForcePersistence = parse_Bool(p_ForcePersistence->data);
	}	
	
	return ONVIF_OK;
}

ONVIF_RET parse_GetAudioEncoderConfigurationOptions(XMLN * p_node, GetAudioEncoderConfigurationOptions_REQ * p_req)
{
	XMLN * p_ConfigurationToken;
	XMLN * p_ProfileToken;
	
	assert(p_node);

	p_ConfigurationToken = xml_node_soap_get(p_node, "ConfigurationToken");
	if (p_ConfigurationToken && p_ConfigurationToken->data)
	{
		p_req->ConfigurationTokenFlag = 1;
		strncpy(p_req->ConfigurationToken, p_ConfigurationToken->data, sizeof(p_req->ConfigurationToken)-1);
	}
	
	p_ProfileToken = xml_node_soap_get(p_node, "ProfileToken");
	if (p_ProfileToken && p_ProfileToken->data)
	{
		p_req->ProfileTokenFlag = 1;
		strncpy(p_req->ProfileToken, p_ProfileToken->data, sizeof(p_req->ProfileToken)-1);
	}
	
	return ONVIF_OK;
}

ONVIF_RET parse_SetAudioEncoderConfiguration(XMLN * p_node, SetAudioEncoderConfiguration_REQ * p_req)
{
	XMLN * p_Configuration;
	XMLN * p_Name;
	XMLN * p_UseCount;
	XMLN * p_Encoding;
	XMLN * p_Bitrate;
	XMLN * p_SampleRate;
	XMLN * p_SessionTimeout;
	XMLN * p_ForcePersistence;
	const char * token;

	p_Configuration = xml_node_soap_get(p_node, "Configuration");
    if (!p_Configuration)
    {
        return ONVIF_ERR_MissingAttribute;
    }
    
    token = xml_attr_get(p_Configuration, "token");
    if (token)
    {
        strncpy(p_req->Configuration.token, token, sizeof(p_req->Configuration.token)-1);
    }    

    p_Name = xml_node_soap_get(p_Configuration, "Name");
    if (p_Name && p_Name->data)
    {
        strncpy(p_req->Configuration.Name, p_Name->data, sizeof(p_req->Configuration.Name)-1);
    }

    p_UseCount = xml_node_soap_get(p_Configuration, "UseCount");
    if (p_UseCount && p_UseCount->data)
    {
        p_req->Configuration.UseCount = atoi(p_UseCount->data);
    }

    p_Encoding = xml_node_soap_get(p_Configuration, "Encoding");
    if (p_Encoding && p_Encoding->data)
    {
		p_req->Configuration.Encoding = onvif_StringToAudioEncoding(p_Encoding->data);
    }    

    p_Bitrate = xml_node_soap_get(p_Configuration, "Bitrate");
    if (p_Bitrate && p_Bitrate->data)
    {
        p_req->Configuration.Bitrate = atoi(p_Bitrate->data);
    }

    p_SampleRate = xml_node_soap_get(p_Configuration, "SampleRate");
    if (p_SampleRate && p_SampleRate->data)
    {
        p_req->Configuration.SampleRate = atoi(p_SampleRate->data);
    }    

	parse_MulticastConfiguration(p_Configuration, &p_req->Configuration.Multicast);
	
	p_SessionTimeout = xml_node_soap_get(p_Configuration, "SessionTimeout");
	if (p_SessionTimeout && p_SessionTimeout->data)
	{
		parse_XSDDuration(p_SessionTimeout->data, &p_req->Configuration.SessionTimeout);
	}

	p_ForcePersistence = xml_node_soap_get(p_node, "ForcePersistence");
	if (p_ForcePersistence && p_ForcePersistence->data)
	{
		p_req->ForcePersistence = parse_Bool(p_ForcePersistence->data);
	}
    	
    return ONVIF_OK;
}

ONVIF_RET parse_AddAudioDecoderConfiguration(XMLN * p_node, AddAudioDecoderConfiguration_REQ * p_req)
{
	XMLN * p_ProfileToken;
	XMLN * p_ConfigurationToken;

    p_ProfileToken = xml_node_soap_get(p_node, "ProfileToken");
	if (p_ProfileToken && p_ProfileToken->data)
	{
		strncpy(p_req->ProfileToken, p_ProfileToken->data, sizeof(p_req->ProfileToken)-1);
	}
	
	p_ConfigurationToken = xml_node_soap_get(p_node, "ConfigurationToken");
	if (p_ConfigurationToken && p_ConfigurationToken->data)
	{
		strncpy(p_req->ConfigurationToken, p_ConfigurationToken->data, sizeof(p_req->ConfigurationToken)-1);
	}
	
	return ONVIF_OK;
}

ONVIF_RET parse_RemoveAudioDecoderConfiguration(XMLN * p_node, RemoveAudioDecoderConfiguration_REQ * p_req)
{
    XMLN * p_ProfileToken;

    p_ProfileToken = xml_node_soap_get(p_node, "ProfileToken");
	if (p_ProfileToken && p_ProfileToken->data)
	{
		strncpy(p_req->ProfileToken, p_ProfileToken->data, sizeof(p_req->ProfileToken)-1);
	}
	
	return ONVIF_OK;
}

ONVIF_RET parse_AudioDecoderConfiguration(XMLN * p_node, onvif_AudioDecoderConfiguration * p_req)
{
    XMLN * p_Name;
    XMLN * p_UseCount;
    const char * p_token;

    p_token = xml_attr_get(p_node, "token");
    if (p_token)
    {
        strncpy(p_req->token, p_token, sizeof(p_req->token)-1);
    }

    p_Name = xml_node_soap_get(p_node, "Name");
	if (p_Name && p_Name->data)
	{
		strncpy(p_req->Name, p_Name->data, sizeof(p_req->Name)-1);
	}

	p_UseCount = xml_node_soap_get(p_node, "UseCount");
	if (p_UseCount && p_UseCount->data)
	{
		p_req->UseCount = atoi(p_UseCount->data);
	}
	
	return ONVIF_OK;
}

ONVIF_RET parse_SetAudioDecoderConfiguration(XMLN * p_node, SetAudioDecoderConfiguration_REQ * p_req)
{
    XMLN * p_Configuration;
	XMLN * p_ForcePersistence;

	p_Configuration = xml_node_soap_get(p_node, "Configuration");
	if (p_Configuration)
	{
	    parse_AudioDecoderConfiguration(p_Configuration, &p_req->Configuration);
	}

	p_ForcePersistence = xml_node_soap_get(p_node, "ForcePersistence");
	if (p_ForcePersistence && p_ForcePersistence->data)
	{
	    p_req->ForcePersistence = parse_Bool(p_ForcePersistence->data);
	}

	return ONVIF_OK;
}

ONVIF_RET parse_GetAudioDecoderConfigurationOptions(XMLN * p_node, GetAudioDecoderConfigurationOptions_REQ * p_req)
{
    XMLN * p_ConfigurationToken;
	XMLN * p_ProfileToken;

	p_ConfigurationToken = xml_node_soap_get(p_node, "ConfigurationToken");
	if (p_ConfigurationToken && p_ConfigurationToken->data)
	{
		p_req->ConfigurationTokenFlag = 1;
		strncpy(p_req->ConfigurationToken, p_ConfigurationToken->data, sizeof(p_req->ConfigurationToken)-1);
	}
	
	p_ProfileToken = xml_node_soap_get(p_node, "ProfileToken");
	if (p_ProfileToken && p_ProfileToken->data)
	{
		p_req->ProfileTokenFlag = 1;
		strncpy(p_req->ProfileToken, p_ProfileToken->data, sizeof(p_req->ProfileToken)-1);
	}
	
	return ONVIF_OK;
}

#endif // end of AUDIO_SUPPORT

#ifdef PTZ_SUPPORT

ONVIF_RET parse_AddPTZConfiguration(XMLN * p_node, AddPTZConfiguration_REQ * p_req)
{
	XMLN * p_ProfileToken;
	XMLN * p_ConfigurationToken;
	
	assert(p_node);

	p_ProfileToken = xml_node_soap_get(p_node, "ProfileToken");
	if (p_ProfileToken && p_ProfileToken->data)
	{
		strncpy(p_req->ProfileToken, p_ProfileToken->data, sizeof(p_req->ProfileToken)-1);
	}
	else
	{
		return ONVIF_ERR_MissingAttribute;
	}
	
	p_ConfigurationToken = xml_node_soap_get(p_node, "ConfigurationToken");
	if (p_ConfigurationToken && p_ConfigurationToken->data)
	{
		strncpy(p_req->ConfigurationToken, p_ConfigurationToken->data, sizeof(p_req->ConfigurationToken)-1);
	}
	else
	{
		return ONVIF_ERR_MissingAttribute;
	}
	
	return ONVIF_OK;
}

ONVIF_RET parse_PTZConfiguration(XMLN * p_node, onvif_PTZConfiguration * p_req)
{
	XMLN * p_Name;
	XMLN * p_UseCount;
	XMLN * p_NodeToken;
	XMLN * p_DefaultPTZSpeed;
	XMLN * p_DefaultPTZTimeout;
	XMLN * p_PanTiltLimits;
	XMLN * p_ZoomLimits;
	XMLN * p_Extension;
	
	p_Name = xml_node_soap_get(p_node, "Name");
	if (p_Name && p_Name->data)
	{
		strncpy(p_req->Name, p_Name->data, sizeof(p_req->Name)-1);
	}

	p_UseCount = xml_node_soap_get(p_node, "UseCount");
	if (p_UseCount && p_UseCount->data)
	{
		p_req->UseCount = atoi(p_UseCount->data);
	}

	p_NodeToken = xml_node_soap_get(p_node, "NodeToken");
	if (p_NodeToken && p_NodeToken->data)
	{
		strncpy(p_req->NodeToken, p_NodeToken->data, sizeof(p_req->NodeToken)-1);
	}

	p_DefaultPTZSpeed = xml_node_soap_get(p_node, "DefaultPTZSpeed");
	if (p_DefaultPTZSpeed)
	{
		XMLN * p_PanTilt;
		XMLN * p_Zoom;

		p_req->DefaultPTZSpeedFlag = 1;
		
		p_PanTilt = xml_node_soap_get(p_DefaultPTZSpeed, "PanTilt");
		if (p_PanTilt)
		{
			p_req->DefaultPTZSpeed.PanTiltFlag = 1;
			parse_Vector(p_PanTilt, &p_req->DefaultPTZSpeed.PanTilt);
		}

		p_Zoom = xml_node_soap_get(p_DefaultPTZSpeed, "Zoom");
		if (p_Zoom)
		{
			p_req->DefaultPTZSpeed.ZoomFlag = 1;
			parse_Vector1D(p_Zoom, &p_req->DefaultPTZSpeed.Zoom);
		}
	}

	p_DefaultPTZTimeout = xml_node_soap_get(p_node, "DefaultPTZTimeout");
	if (p_DefaultPTZTimeout && p_DefaultPTZTimeout->data)
	{
		p_req->DefaultPTZTimeoutFlag = parse_XSDDuration(p_DefaultPTZTimeout->data, &p_req->DefaultPTZTimeout);
	}

	p_PanTiltLimits = xml_node_soap_get(p_node, "PanTiltLimits");
	if (p_PanTiltLimits)
	{
		XMLN * p_Range;
		
		p_req->PanTiltLimitsFlag = 1;
		
		p_Range = xml_node_soap_get(p_PanTiltLimits, "Range");
		if (p_Range)
		{
			XMLN * p_XRange;
			XMLN * p_YRange;

			p_XRange = xml_node_soap_get(p_Range, "XRange");
			if (p_XRange)
			{
				parse_FloatRange(p_XRange, &p_req->PanTiltLimits.XRange);
			}	

			p_YRange = xml_node_soap_get(p_Range, "YRange");
			if (p_YRange)
			{
				parse_FloatRange(p_YRange, &p_req->PanTiltLimits.YRange);
			}
		}
	}

	p_ZoomLimits = xml_node_soap_get(p_node, "ZoomLimits");
	if (p_ZoomLimits)
	{
		XMLN * p_Range;
		
		p_req->ZoomLimitsFlag = 1;
		
		p_Range = xml_node_soap_get(p_ZoomLimits, "Range");
		if (p_Range)
		{
			XMLN * p_XRange;

			p_XRange = xml_node_soap_get(p_Range, "XRange");
			if (p_XRange)
			{
				parse_FloatRange(p_XRange, &p_req->ZoomLimits.XRange);
			}
		}
	}

	p_Extension = xml_node_soap_get(p_node, "Extension");
	if (p_Extension)
	{
		XMLN * p_PTControlDirection;
		XMLN * p_EFlip;
		XMLN * p_Reverse;
		XMLN * p_Mode;
		
		p_req->ExtensionFlag = 1;
		
		p_PTControlDirection = xml_node_soap_get(p_Extension, "PTControlDirection");
		if (p_PTControlDirection)
		{
			p_req->Extension.PTControlDirectionFlag = 1;
			
			p_EFlip = xml_node_soap_get(p_PTControlDirection, "EFlip");
			if (p_EFlip)
			{
				p_req->Extension.PTControlDirection.EFlipFlag = 1;

				p_Mode = xml_node_soap_get(p_EFlip, "Mode");
				if (p_Mode && p_Mode->data)
				{
					p_req->Extension.PTControlDirection.EFlip = onvif_StringToEFlipMode(p_Mode->data);
				}
			}

			p_Reverse = xml_node_soap_get(p_PTControlDirection, "Reverse");
			if (p_Reverse)
			{
				p_req->Extension.PTControlDirection.ReverseFlag = 1;

				p_Mode = xml_node_soap_get(p_Reverse, "Mode");
				if (p_Mode && p_Mode->data)
				{
					p_req->Extension.PTControlDirection.Reverse = onvif_StringToReverseMode(p_Mode->data);
				}
			}
		}
	}
	
	return ONVIF_OK;
}

ONVIF_RET parse_GetCompatibleConfigurations(XMLN * p_node, GetCompatibleConfigurations_REQ * p_req)
{
    XMLN * p_ProfileToken;	    

	p_ProfileToken = xml_node_soap_get(p_node, "ProfileToken");
	if (p_ProfileToken && p_ProfileToken->data)
	{
		strncpy(p_req->ProfileToken, p_ProfileToken->data, sizeof(p_req->ProfileToken)-1);
	}

	return ONVIF_OK;
}

ONVIF_RET parse_SetConfiguration(XMLN * p_node, SetConfiguration_REQ * p_req)
{
	ONVIF_RET ret;
	const char * p_token;
	const char * p_MoveRamp;
	const char * p_PresetRamp;
	const char * p_PresetTourRamp;
	XMLN * p_PTZConfiguration;
	XMLN * p_ForcePersistence;	    

	p_PTZConfiguration = xml_node_soap_get(p_node, "PTZConfiguration");
	if (NULL == p_PTZConfiguration)
	{
		return ONVIF_ERR_MissingAttribute;
	}

	p_token = xml_attr_get(p_PTZConfiguration, "token");
	if (p_token)
	{
		strncpy(p_req->PTZConfiguration.token, p_token, sizeof(p_req->PTZConfiguration.token)-1);
	}

	p_MoveRamp = xml_attr_get(p_PTZConfiguration, "MoveRamp");
    if (p_MoveRamp)
    {
        p_req->PTZConfiguration.MoveRampFlag = 1;
        p_req->PTZConfiguration.MoveRamp = atoi(p_MoveRamp);
    }
    
    p_PresetRamp = xml_attr_get(p_PTZConfiguration, "PresetRamp");
    if (p_PresetRamp)
    {
        p_req->PTZConfiguration.PresetRampFlag = 1;
        p_req->PTZConfiguration.PresetRamp = atoi(p_PresetRamp);
    }
    
    p_PresetTourRamp = xml_attr_get(p_PTZConfiguration, "PresetTourRamp");
    if (p_PresetTourRamp)
    {
        p_req->PTZConfiguration.PresetTourRampFlag = 1;
        p_req->PTZConfiguration.PresetTourRamp = atoi(p_PresetTourRamp);
    }

	ret = parse_PTZConfiguration(p_PTZConfiguration, &p_req->PTZConfiguration);
	if (ONVIF_OK != ret)
	{
		return ret;
	}

	p_ForcePersistence = xml_node_soap_get(p_node, "ForcePersistence");
	if (p_ForcePersistence && p_ForcePersistence->data)
	{
		p_req->ForcePersistence = parse_Bool(p_ForcePersistence->data);		
	}

	return ONVIF_OK;
}

ONVIF_RET parse_PTZSpeed(XMLN * p_node, onvif_PTZSpeed * p_req)
{
	XMLN * p_PanTilt;
	XMLN * p_Zoom;

	p_PanTilt = xml_node_soap_get(p_node, "PanTilt");
	if (p_PanTilt)
	{
		p_req->PanTiltFlag = 1;		
		parse_Vector(p_PanTilt, &p_req->PanTilt);
	}

	p_Zoom = xml_node_soap_get(p_node, "Zoom");
	if (p_Zoom)
	{
		p_req->ZoomFlag = 1;		
		parse_Vector1D(p_Zoom, &p_req->Zoom);
	}

	return ONVIF_OK;
}

ONVIF_RET parse_PTZVector(XMLN * p_node, onvif_PTZVector * p_req)
{
	XMLN * p_PanTilt;
	XMLN * p_Zoom;

	p_PanTilt = xml_node_soap_get(p_node, "PanTilt");
	if (p_PanTilt)
	{	
		p_req->PanTiltFlag = 1;		
		parse_Vector(p_PanTilt, &p_req->PanTilt);
	}

	p_Zoom = xml_node_soap_get(p_node, "Zoom");
	if (p_Zoom)
	{	
		p_req->ZoomFlag = 1;		
		parse_Vector1D(p_Zoom, &p_req->Zoom);
	}

	return ONVIF_OK;
}


ONVIF_RET parse_ContinuousMove(XMLN * p_node, ContinuousMove_REQ * p_req)
{
	XMLN * p_ProfileToken;
	XMLN * p_Velocity;
	XMLN * p_Timeout;
	
	assert(p_node);

	p_ProfileToken = xml_node_soap_get(p_node, "ProfileToken");
	if (p_ProfileToken && p_ProfileToken->data)
	{
		strncpy(p_req->ProfileToken, p_ProfileToken->data, sizeof(p_req->ProfileToken)-1);
	}
	
	p_Velocity = xml_node_soap_get(p_node, "Velocity");
	if (p_Velocity)
	{	
		parse_PTZSpeed(p_Velocity, &p_req->Velocity);
	}

	p_Timeout = xml_node_soap_get(p_node, "Timeout");
	if (p_Timeout && p_Timeout->data)
	{
		p_req->TimeoutFlag = parse_XSDDuration(p_Timeout->data, &p_req->Timeout);
	}
	
	return ONVIF_OK;
}

ONVIF_RET parse_ptz_Stop(XMLN * p_node, PTZ_Stop_REQ * p_req)
{
	XMLN * p_ProfileToken;
	XMLN * p_PanTilt;
	XMLN * p_Zoom;
	
	assert(p_node);

	p_ProfileToken = xml_node_soap_get(p_node, "ProfileToken");
	if (p_ProfileToken && p_ProfileToken->data)
	{
		strncpy(p_req->ProfileToken, p_ProfileToken->data, sizeof(p_req->ProfileToken)-1);
	}
	
	p_PanTilt = xml_node_soap_get(p_node, "PanTilt");
	if (p_PanTilt && p_PanTilt->data)
	{
		p_req->PanTiltFlag = 1;
		p_req->PanTilt = parse_Bool(p_PanTilt->data);
	}

	p_Zoom = xml_node_soap_get(p_node, "Zoom");
	if (p_Zoom && p_Zoom->data)
	{
		p_req->ZoomFlag = 1;
		p_req->Zoom = parse_Bool(p_Zoom->data);
	}

	return ONVIF_OK;
}

ONVIF_RET parse_AbsoluteMove(XMLN * p_node, AbsoluteMove_REQ * p_req)
{
	XMLN * p_ProfileToken;
	XMLN * p_Position;
	XMLN * p_Speed;
	
    assert(p_node);

	p_ProfileToken = xml_node_soap_get(p_node, "ProfileToken");
	if (p_ProfileToken && p_ProfileToken->data)
	{
		strncpy(p_req->ProfileToken, p_ProfileToken->data, sizeof(p_req->ProfileToken)-1);
	}

    p_Position = xml_node_soap_get(p_node, "Position");
	if (p_Position)
	{	
	    parse_PTZVector(p_Position, &p_req->Position);
	}

	p_Speed = xml_node_soap_get(p_node, "Speed");
	if (p_Speed)
	{	
		p_req->SpeedFlag = 1;		
	    parse_PTZSpeed(p_Speed, &p_req->Speed);
	}

	return ONVIF_OK;
}

ONVIF_RET parse_RelativeMove(XMLN * p_node, RelativeMove_REQ * p_req)
{
	XMLN * p_ProfileToken;
	XMLN * p_Translation;
	XMLN * p_Speed;
	
    assert(p_node);

	p_ProfileToken = xml_node_soap_get(p_node, "ProfileToken");
	if (p_ProfileToken && p_ProfileToken->data)
	{
		strncpy(p_req->ProfileToken, p_ProfileToken->data, sizeof(p_req->ProfileToken)-1);
	}

    p_Translation = xml_node_soap_get(p_node, "Translation");
	if (p_Translation)
	{	
	    parse_PTZVector(p_Translation, &p_req->Translation);
	}

	p_Speed = xml_node_soap_get(p_node, "Speed");
	if (p_Speed)
	{	
		p_req->SpeedFlag = 1;		
	    parse_PTZSpeed(p_Speed, &p_req->Speed);
	}

	return ONVIF_OK;
}

ONVIF_RET parse_SetPreset(XMLN * p_node, SetPreset_REQ * p_req)
{
	XMLN * p_ProfileToken;
	XMLN * p_PresetName;
	XMLN * p_PresetToken;
	
    assert(p_node);

	p_ProfileToken = xml_node_soap_get(p_node, "ProfileToken");
	if (p_ProfileToken && p_ProfileToken->data)
	{
		strncpy(p_req->ProfileToken, p_ProfileToken->data, sizeof(p_req->ProfileToken)-1);
	}

	p_PresetName = xml_node_soap_get(p_node, "PresetName");
	if (p_PresetName && p_PresetName->data)
	{
		p_req->PresetNameFlag = 1;
		strncpy(p_req->PresetName, p_PresetName->data, sizeof(p_req->PresetName)-1);
	}

	p_PresetToken = xml_node_soap_get(p_node, "PresetToken");
	if (p_PresetToken && p_PresetToken->data)
	{
		p_req->PresetTokenFlag = 1;
		strncpy(p_req->PresetToken, p_PresetToken->data, sizeof(p_req->PresetToken)-1);
	}

	return ONVIF_OK;
}

ONVIF_RET parse_RemovePreset(XMLN * p_node, RemovePreset_REQ * p_req)
{
	XMLN * p_ProfileToken;
	XMLN * p_PresetToken;
	
	assert(p_node);

    p_ProfileToken = xml_node_soap_get(p_node, "ProfileToken");
	if (p_ProfileToken && p_ProfileToken->data)
	{
		strncpy(p_req->ProfileToken, p_ProfileToken->data, sizeof(p_req->ProfileToken)-1);
	}

	p_PresetToken = xml_node_soap_get(p_node, "PresetToken");
	if (p_PresetToken && p_PresetToken->data)
	{
		strncpy(p_req->PresetToken, p_PresetToken->data, sizeof(p_req->PresetToken)-1);
	}

	return ONVIF_OK;
}

ONVIF_RET parse_GotoPreset(XMLN * p_node, GotoPreset_REQ * p_req)
{
	XMLN * p_ProfileToken;
	XMLN * p_PresetToken;
	XMLN * p_Speed;
	
	assert(p_node);

    p_ProfileToken = xml_node_soap_get(p_node, "ProfileToken");
	if (p_ProfileToken && p_ProfileToken->data)
	{
		strncpy(p_req->ProfileToken, p_ProfileToken->data, sizeof(p_req->ProfileToken)-1);
	}

	p_PresetToken = xml_node_soap_get(p_node, "PresetToken");
	if (p_PresetToken && p_PresetToken->data)
	{
		strncpy(p_req->PresetToken, p_PresetToken->data, sizeof(p_req->PresetToken)-1);
	}

	p_Speed = xml_node_soap_get(p_node, "Speed");
	if (p_Speed)
	{	
		p_req->SpeedFlag = 1;		
	    parse_PTZSpeed(p_Speed, &p_req->Speed);
	}

	return ONVIF_OK;
}

ONVIF_RET parse_GotoHomePosition(XMLN * p_node, GotoHomePosition_REQ * p_req)
{
	XMLN * p_ProfileToken;
	XMLN * p_Speed;
	
    assert(p_node);

    p_ProfileToken = xml_node_soap_get(p_node, "ProfileToken");
	if (p_ProfileToken && p_ProfileToken->data)
	{
		strncpy(p_req->ProfileToken, p_ProfileToken->data, sizeof(p_req->ProfileToken)-1);
	}

	p_Speed = xml_node_soap_get(p_node, "Speed");
	if (p_Speed)
	{	
		p_req->SpeedFlag = 1;
	    parse_PTZSpeed(p_Speed, &p_req->Speed);
	}

	return ONVIF_OK;
}

#endif // PTZ_SUPPORT

#ifdef PROFILE_G_SUPPORT

ONVIF_RET parse_RecordingConfiguration(XMLN * p_node, onvif_RecordingConfiguration * p_req)
{
	XMLN * p_Source;
	XMLN * p_Content;
	XMLN * p_MaximumRetentionTime;

	p_Source = xml_node_soap_get(p_node, "Source");
	if (p_Source)
	{
		XMLN * p_SourceId;
		XMLN * p_Name;
		XMLN * p_Location;
		XMLN * p_Description;
		XMLN * p_Address;

		p_SourceId = xml_node_soap_get(p_Source, "SourceId");
		if (p_SourceId && p_SourceId->data)
		{
			strncpy(p_req->Source.SourceId, p_SourceId->data, sizeof(p_req->Source.SourceId)-1);
		}

		p_Name = xml_node_soap_get(p_Source, "Name");
		if (p_Name && p_Name->data)
		{
			strncpy(p_req->Source.Name, p_Name->data, sizeof(p_req->Source.Name)-1);
		}

		p_Location = xml_node_soap_get(p_Source, "Location");
		if (p_Location && p_Location->data)
		{
			strncpy(p_req->Source.Location, p_Location->data, sizeof(p_req->Source.Location)-1);
		}

		p_Description = xml_node_soap_get(p_Source, "Description");
		if (p_Description && p_Description->data)
		{
			strncpy(p_req->Source.Description, p_Description->data, sizeof(p_req->Source.Description)-1);
		}

		p_Address = xml_node_soap_get(p_Source, "Address");
		if (p_Address && p_Address->data)
		{
			strncpy(p_req->Source.Address, p_Address->data, sizeof(p_req->Source.Address)-1);
		}
	}

	p_Content = xml_node_soap_get(p_node, "Content");
	if (p_Content && p_Content->data)
	{
		strncpy(p_req->Content, p_Content->data, sizeof(p_req->Content)-1);
	}

	p_MaximumRetentionTime = xml_node_soap_get(p_node, "MaximumRetentionTime");
	if (p_MaximumRetentionTime && p_MaximumRetentionTime->data)
	{
		p_req->MaximumRetentionTimeFlag = parse_XSDDuration(p_MaximumRetentionTime->data, (int*)&p_req->MaximumRetentionTime);
	}

	return ONVIF_OK;
}

ONVIF_RET parse_CreateRecording(XMLN * p_node, CreateRecording_REQ * p_req)
{
	XMLN * p_RecordingConfiguration;

	p_RecordingConfiguration = xml_node_soap_get(p_node, "RecordingConfiguration");
	if (NULL == p_RecordingConfiguration)
	{
		return ONVIF_ERR_BadConfiguration;
	}
	else
	{
		return parse_RecordingConfiguration(p_RecordingConfiguration, &p_req->RecordingConfiguration);
	}
}

ONVIF_RET parse_SetRecordingConfiguration(XMLN * p_node, SetRecordingConfiguration_REQ * p_req)
{
	XMLN * p_RecordingToken;
	XMLN * p_RecordingConfiguration;

	p_RecordingToken = xml_node_soap_get(p_node, "RecordingToken");
	if (p_RecordingToken && p_RecordingToken->data)
	{
		strncpy(p_req->RecordingToken, p_RecordingToken->data, sizeof(p_req->RecordingToken)-1);
	}
	
	p_RecordingConfiguration = xml_node_soap_get(p_node, "RecordingConfiguration");
	if (p_RecordingConfiguration)
	{
		return parse_RecordingConfiguration(p_RecordingConfiguration, &p_req->RecordingConfiguration);
	}
	else
	{
		return ONVIF_ERR_BadConfiguration;
	}
}

ONVIF_RET parse_TrackConfiguration(XMLN * p_node, onvif_TrackConfiguration * p_req)
{
	XMLN * p_TrackType;
	XMLN * p_Description;

	p_TrackType = xml_node_soap_get(p_node, "TrackType");
	if (p_TrackType && p_TrackType->data)
	{
		p_req->TrackType = onvif_StringToTrackType(p_TrackType->data);
		if (TrackType_Invalid == p_req->TrackType)
		{
			return ONVIF_ERR_BadConfiguration;
		}
	}

	p_Description = xml_node_soap_get(p_node, "Description");
	if (p_Description && p_Description->data)
	{
		strncpy(p_req->Description, p_Description->data, sizeof(p_req->Description)-1);
	}

	return ONVIF_OK;
}

ONVIF_RET parse_CreateTrack(XMLN * p_node, CreateTrack_REQ * p_req)
{
	XMLN * p_RecordingToken;
	XMLN * p_TrackConfiguration;

	p_RecordingToken = xml_node_soap_get(p_node, "RecordingToken");
	if (p_RecordingToken && p_RecordingToken->data)
	{
		strncpy(p_req->RecordingToken, p_RecordingToken->data, sizeof(p_req->RecordingToken)-1);
	}

	p_TrackConfiguration = xml_node_soap_get(p_node, "TrackConfiguration");
	if (p_TrackConfiguration)
	{
		return parse_TrackConfiguration(p_TrackConfiguration, &p_req->TrackConfiguration);
	}

	return ONVIF_OK;
}

ONVIF_RET parse_DeleteTrack(XMLN * p_node, DeleteTrack_REQ * p_req)
{
	XMLN * p_RecordingToken;
	XMLN * p_TrackToken;

	p_RecordingToken = xml_node_soap_get(p_node, "RecordingToken");
	if (p_RecordingToken && p_RecordingToken->data)
	{
		strncpy(p_req->RecordingToken, p_RecordingToken->data, sizeof(p_req->RecordingToken)-1);
	}

	p_TrackToken = xml_node_soap_get(p_node, "TrackToken");
	if (p_TrackToken && p_TrackToken->data)
	{
		strncpy(p_req->TrackToken, p_TrackToken->data, sizeof(p_req->TrackToken)-1);
	}

	return ONVIF_OK;
}

ONVIF_RET parse_GetTrackConfiguration(XMLN * p_node, GetTrackConfiguration_REQ * p_req)
{
	XMLN * p_RecordingToken;
	XMLN * p_TrackToken;

	p_RecordingToken = xml_node_soap_get(p_node, "RecordingToken");
	if (p_RecordingToken && p_RecordingToken->data)
	{
		strncpy(p_req->RecordingToken, p_RecordingToken->data, sizeof(p_req->RecordingToken)-1);
	}

	p_TrackToken = xml_node_soap_get(p_node, "TrackToken");
	if (p_TrackToken && p_TrackToken->data)
	{
		strncpy(p_req->TrackToken, p_TrackToken->data, sizeof(p_req->TrackToken)-1);
	}

	return ONVIF_OK;
}

ONVIF_RET parse_SetTrackConfiguration(XMLN * p_node, SetTrackConfiguration_REQ * p_req)
{
	XMLN * p_RecordingToken;
	XMLN * p_TrackToken;
	XMLN * p_TrackConfiguration;

	p_RecordingToken = xml_node_soap_get(p_node, "RecordingToken");
	if (p_RecordingToken && p_RecordingToken->data)
	{
		strncpy(p_req->RecordingToken, p_RecordingToken->data, sizeof(p_req->RecordingToken)-1);
	}

	p_TrackToken = xml_node_soap_get(p_node, "TrackToken");
	if (p_TrackToken && p_TrackToken->data)
	{
		strncpy(p_req->TrackToken, p_TrackToken->data, sizeof(p_req->TrackToken)-1);
	}

	p_TrackConfiguration = xml_node_soap_get(p_node, "TrackConfiguration");
	if (p_TrackConfiguration)
	{
		return parse_TrackConfiguration(p_TrackConfiguration, &p_req->TrackConfiguration);
	}

	return ONVIF_OK;
}

ONVIF_RET parse_JobConfiguration(XMLN * p_node, onvif_RecordingJobConfiguration * p_req)
{
	XMLN * p_RecordingToken;
	XMLN * p_Mode;
	XMLN * p_Priority;
	XMLN * p_Source;

	p_RecordingToken = xml_node_soap_get(p_node, "RecordingToken");
	if (p_RecordingToken && p_RecordingToken->data)
	{
		strncpy(p_req->RecordingToken, p_RecordingToken->data, sizeof(p_req->RecordingToken)-1);
	}

	p_Mode = xml_node_soap_get(p_node, "Mode");
	if (p_Mode && p_Mode->data)
	{
		strncpy(p_req->Mode, p_Mode->data, sizeof(p_req->Mode)-1);
	}

	p_Priority = xml_node_soap_get(p_node, "Priority");
	if (p_Priority && p_Priority->data)
	{
		p_req->Priority = atoi(p_Priority->data);
	}

	p_Source = xml_node_soap_get(p_node, "Source");
	while (p_Source && soap_strcmp(p_Source->name, "Source") == 0)
	{
		int i = p_req->sizeSource;
		XMLN * p_SourceToken;
		XMLN * p_AutoCreateReceiver;
		XMLN * p_Tracks;

		p_SourceToken = xml_node_soap_get(p_Source, "SourceToken");
		if (p_SourceToken)
		{
			const char * p_Type;
			XMLN * p_Token;

			p_req->Source[i].SourceTokenFlag = 1;
			
			p_Type = xml_attr_get(p_SourceToken, "Type");
			if (p_Type)
			{
				p_req->Source[i].SourceToken.TypeFlag = 1;
				strncpy(p_req->Source[i].SourceToken.Type, p_Type, sizeof(p_req->Source[i].SourceToken.Type)-1);
			}

			p_Token = xml_node_soap_get(p_SourceToken, "Token");
			if (p_Token && p_Token->data)
			{
				strncpy(p_req->Source[i].SourceToken.Token, p_Token->data, sizeof(p_req->Source[i].SourceToken.Token)-1);
			}
		}

		p_AutoCreateReceiver = xml_node_soap_get(p_Source, "AutoCreateReceiver");
		if (p_AutoCreateReceiver && p_AutoCreateReceiver->data)
		{
			p_req->Source[i].AutoCreateReceiverFlag = 1;
			p_req->Source[i].AutoCreateReceiver = parse_Bool(p_AutoCreateReceiver->data);
		}

		p_Tracks = xml_node_soap_get(p_Source, "Tracks");
		while (p_Tracks && soap_strcmp(p_Tracks->name, "Tracks") == 0)
		{
			int j = p_req->Source[i].sizeTracks;
			XMLN * p_SourceTag;
			XMLN * p_Destination;

			p_SourceTag = xml_node_soap_get(p_Tracks, "SourceTag");
			if (p_SourceTag && p_SourceTag->data)
			{
				strncpy(p_req->Source[i].Tracks[j].SourceTag, p_SourceTag->data, sizeof(p_req->Source[i].Tracks[j].SourceTag)-1);
			}

			p_Destination = xml_node_soap_get(p_Tracks, "Destination");
			if (p_Destination && p_Destination->data)
			{
				strncpy(p_req->Source[i].Tracks[j].Destination, p_Destination->data, sizeof(p_req->Source[i].Tracks[j].Destination)-1);
			}

			p_req->Source[i].sizeTracks++;
			if (p_req->Source[i].sizeTracks >= ARRAY_SIZE(p_req->Source[i].Tracks))
			{
				break;
			}

			p_Tracks = p_Tracks->next;
		}
		
		p_req->sizeSource++;
		if (p_req->sizeSource >= ARRAY_SIZE(p_req->Source))
		{
			break;
		}

		p_Source = p_Source->next;
	}

	return ONVIF_OK;
}

ONVIF_RET parse_CreateRecordingJob(XMLN * p_node, CreateRecordingJob_REQ * p_req)
{
	XMLN * p_JobConfiguration;
	
	p_JobConfiguration = xml_node_soap_get(p_node, "JobConfiguration");
	if (p_JobConfiguration)
	{
		return parse_JobConfiguration(p_JobConfiguration, &p_req->JobConfiguration);
	}
	
	return ONVIF_OK;
}

ONVIF_RET parse_SetRecordingJobConfiguration(XMLN * p_node, SetRecordingJobConfiguration_REQ * p_req)
{
	XMLN * p_JobToken;
	XMLN * p_JobConfiguration;

	p_JobToken = xml_node_soap_get(p_node, "JobToken");
	if (p_JobToken && p_JobToken->data)
	{
		strncpy(p_req->JobToken, p_JobToken->data, sizeof(p_req->JobToken)-1);
	}
	
	p_JobConfiguration = xml_node_soap_get(p_node, "JobConfiguration");
	if (p_JobConfiguration)
	{
		return parse_JobConfiguration(p_JobConfiguration, &p_req->JobConfiguration);
	}
	
	return ONVIF_OK;
}

ONVIF_RET parse_SetRecordingJobMode(XMLN * p_node, SetRecordingJobMode_REQ * p_req)
{
	XMLN * p_JobToken;
	XMLN * p_Mode;

	p_JobToken = xml_node_soap_get(p_node, "JobToken");
	if (p_JobToken && p_JobToken->data)
	{
		strncpy(p_req->JobToken, p_JobToken->data, sizeof(p_req->JobToken)-1);
	}
	
	p_Mode = xml_node_soap_get(p_node, "Mode");
	if (p_Mode && p_Mode->data)
	{
		strncpy(p_req->Mode, p_Mode->data, sizeof(p_req->Mode));
	}
	
	return ONVIF_OK;
}

ONVIF_RET parse_GetRecordingInformation(XMLN * p_node, GetRecordingInformation_REQ * p_req)
{
    XMLN * p_RecordingToken;

    p_RecordingToken = xml_node_soap_get(p_node, "RecordingToken");
	if (p_RecordingToken && p_RecordingToken->data)
	{
		strncpy(p_req->RecordingToken, p_RecordingToken->data, sizeof(p_req->RecordingToken)-1);
	}

	return ONVIF_OK;
}

ONVIF_RET parse_GetMediaAttributes(XMLN * p_node, GetMediaAttributes_REQ * p_req)
{
	int idx;
	XMLN * p_RecordingTokens;
	XMLN * p_Time;

	p_RecordingTokens = xml_node_soap_get(p_node, "RecordingTokens");
	while (p_RecordingTokens && p_RecordingTokens->data && soap_strcmp(p_RecordingTokens->name, "RecordingTokens") == 0)
	{
		idx = p_req->sizeRecordingTokens;
		strncpy(p_req->RecordingTokens[idx], p_RecordingTokens->data, sizeof(p_req->RecordingTokens[idx])-1);
		
		p_req->sizeRecordingTokens++;
		if (p_req->sizeRecordingTokens >= ARRAY_SIZE(p_req->RecordingTokens))
		{
			break;
		}

		p_RecordingTokens = p_RecordingTokens->next;
	}

	p_Time = xml_node_soap_get(p_node, "Time");
	if (p_Time && p_Time->data)
	{
		parse_XSDDatetime(p_Time->data, &p_req->Time);
	}

	return ONVIF_OK;
}

ONVIF_RET parse_SearchScope(XMLN * p_node, onvif_SearchScope * p_req)
{
	XMLN * p_IncludedSources;
	XMLN * p_IncludedRecordings;
	XMLN * p_RecordingInformationFilter;

	p_IncludedSources = xml_node_soap_get(p_node, "IncludedSources");
	while (p_IncludedSources && soap_strcmp(p_IncludedSources->name, "IncludedSources") == 0)
	{
		int idx = p_req->sizeIncludedSources;
		const char * p_Type;
		XMLN * p_Token;

		p_Type = xml_attr_get(p_IncludedSources, "Type");
		if (p_Type)
		{
			p_req->IncludedSources[idx].TypeFlag = 1;
			strncpy(p_req->IncludedSources[idx].Type, p_Type, sizeof(p_req->IncludedSources[idx].Type));			
		}

		p_Token = xml_node_soap_get(p_IncludedSources, "Token");
		if (p_Token && p_Token->data)
		{
			strncpy(p_req->IncludedSources[idx].Token, p_Token->data, sizeof(p_req->IncludedSources[idx].Token));
		}
		
		p_req->sizeIncludedSources++;
		if (p_req->sizeIncludedSources >= ARRAY_SIZE(p_req->IncludedSources))
		{
			break;
		}

		p_IncludedSources = p_IncludedSources->next;
	}

	p_IncludedRecordings = xml_node_soap_get(p_node, "IncludedRecordings");
	while (p_IncludedRecordings && p_IncludedRecordings->data && soap_strcmp(p_IncludedRecordings->name, "IncludedRecordings") == 0)
	{
		int idx = p_req->sizeIncludedRecordings;
		
		strncpy(p_req->IncludedRecordings[idx], p_IncludedRecordings->data, sizeof(p_req->IncludedRecordings[idx])-1);

		p_req->sizeIncludedRecordings++;
		if (p_req->sizeIncludedRecordings >= ARRAY_SIZE(p_req->IncludedRecordings))
		{
			break;
		}

		p_IncludedRecordings = p_IncludedRecordings->next;
	}

	p_RecordingInformationFilter = xml_node_soap_get(p_node, "RecordingInformationFilter");
	if (p_RecordingInformationFilter && p_RecordingInformationFilter->data)
	{
		strncpy(p_req->RecordingInformationFilter, p_RecordingInformationFilter->data, sizeof(p_req->RecordingInformationFilter));
	}

	return ONVIF_OK;
}

ONVIF_RET parse_FindRecordings(XMLN * p_node, FindRecordings_REQ * p_req)
{
	XMLN * p_Scope;
	XMLN * p_MaxMatches;
	XMLN * p_KeepAliveTime;

	p_Scope = xml_node_soap_get(p_node, "Scope");
	if (p_Scope)
	{
		parse_SearchScope(p_Scope, &p_req->Scope);
	}

	p_MaxMatches = xml_node_soap_get(p_node, "MaxMatches");
	if (p_MaxMatches && p_MaxMatches->data)
	{
		p_req->MaxMatchesFlag = 1;
		p_req->MaxMatches = atoi(p_MaxMatches->data);
	}

	p_KeepAliveTime = xml_node_soap_get(p_node, "KeepAliveTime");
	if (p_KeepAliveTime && p_KeepAliveTime->data)
	{
		parse_XSDDuration(p_KeepAliveTime->data, &p_req->KeepAliveTime);
	}

	return ONVIF_OK;
}

ONVIF_RET parse_GetRecordingSearchResults(XMLN * p_node, GetRecordingSearchResults_REQ * p_req)
{
	XMLN * p_SearchToken;
	XMLN * p_MinResults;
	XMLN * p_MaxResults;
	XMLN * p_WaitTime;

	p_SearchToken = xml_node_soap_get(p_node, "SearchToken");
	if (p_SearchToken && p_SearchToken->data)
	{
		strncpy(p_req->SearchToken, p_SearchToken->data, sizeof(p_req->SearchToken)-1);
	}

	p_MinResults = xml_node_soap_get(p_node, "MinResults");
	if (p_MinResults && p_MinResults->data)
	{
		p_req->MinResultsFlag = 1;
		p_req->MinResults = atoi(p_MinResults->data);
	}

	p_MaxResults = xml_node_soap_get(p_node, "MaxResults");
	if (p_MaxResults && p_MaxResults->data)
	{
		p_req->MaxResultsFlag = 1;
		p_req->MaxResults = atoi(p_MaxResults->data);
	}

	p_WaitTime = xml_node_soap_get(p_node, "WaitTime");
	if (p_WaitTime && p_WaitTime->data)
	{
		p_req->WaitTimeFlag = 1;
		parse_XSDDuration(p_WaitTime->data, &p_req->WaitTime);
	}
	
	return ONVIF_OK;
}

ONVIF_RET parse_FindEvents(XMLN * p_node, FindEvents_REQ * p_req)
{
	XMLN * p_StartPoint;
	XMLN * p_EndPoint;
	XMLN * p_Scope;
	XMLN * p_IncludeStartState;
	XMLN * p_MaxMatches;
	XMLN * p_KeepAliveTime;

	p_StartPoint = xml_node_soap_get(p_node, "StartPoint");
	if (p_StartPoint && p_StartPoint->data)
	{
		parse_XSDDatetime(p_StartPoint->data, &p_req->StartPoint);
	}

	p_EndPoint = xml_node_soap_get(p_node, "EndPoint");
	if (p_EndPoint && p_EndPoint->data)
	{
		p_req->EndPointFlag = 1;
		parse_XSDDatetime(p_EndPoint->data, &p_req->EndPoint);
	}

	p_Scope = xml_node_soap_get(p_node, "Scope");
	if (p_Scope)
	{
		parse_SearchScope(p_Scope, &p_req->Scope);
	}

	p_IncludeStartState = xml_node_soap_get(p_node, "IncludeStartState");
	if (p_IncludeStartState && p_IncludeStartState->data)
	{
		p_req->IncludeStartState = parse_Bool(p_IncludeStartState->data);
	}

	p_MaxMatches = xml_node_soap_get(p_node, "MaxMatches");
	if (p_MaxMatches && p_MaxMatches->data)
	{
		p_req->MaxMatchesFlag = 1;
		p_req->MaxMatches = atoi(p_MaxMatches->data);
	}

	p_KeepAliveTime = xml_node_soap_get(p_node, "KeepAliveTime");
	if (p_KeepAliveTime && p_KeepAliveTime->data)
	{
		p_req->KeepAliveTimeFlag = 1;
		parse_XSDDuration(p_KeepAliveTime->data, &p_req->KeepAliveTime);
	}
	
	return ONVIF_OK;
}

ONVIF_RET parse_GetEventSearchResults(XMLN * p_node, GetEventSearchResults_REQ * p_req)
{
	XMLN * p_SearchToken;
	XMLN * p_MinResults;
	XMLN * p_MaxResults;
	XMLN * p_WaitTime;

	p_SearchToken = xml_node_soap_get(p_node, "SearchToken");
	if (p_SearchToken && p_SearchToken->data)
	{
		strncpy(p_req->SearchToken, p_SearchToken->data, sizeof(p_req->SearchToken)-1);
	}

	p_MinResults = xml_node_soap_get(p_node, "MinResults");
	if (p_MinResults && p_MinResults->data)
	{
		p_req->MinResultsFlag = 1;
		p_req->MinResults = atoi(p_MinResults->data);
	}

	p_MaxResults = xml_node_soap_get(p_node, "MaxResults");
	if (p_MaxResults && p_MaxResults->data)
	{
		p_req->MaxResultsFlag = 1;
		p_req->MaxResults = atoi(p_MaxResults->data);
	}

	p_WaitTime = xml_node_soap_get(p_node, "WaitTime");
	if (p_WaitTime && p_WaitTime->data)
	{
		p_req->WaitTimeFlag = 1;
		parse_XSDDuration(p_WaitTime->data, &p_req->WaitTime);
	}
	
	return ONVIF_OK;
}

ONVIF_RET parse_FindMetadata(XMLN * p_node, FindMetadata_REQ * p_req)
{
    XMLN * p_StartPoint;
	XMLN * p_EndPoint;
	XMLN * p_Scope;
	XMLN * p_MetadataFilter;
	XMLN * p_MaxMatches;
	XMLN * p_KeepAliveTime;

	p_StartPoint = xml_node_soap_get(p_node, "StartPoint");
	if (p_StartPoint && p_StartPoint->data)
	{
		parse_XSDDatetime(p_StartPoint->data, &p_req->StartPoint);
	}

	p_EndPoint = xml_node_soap_get(p_node, "EndPoint");
	if (p_EndPoint && p_EndPoint->data)
	{
		p_req->EndPointFlag = 1;
		parse_XSDDatetime(p_EndPoint->data, &p_req->EndPoint);
	}

	p_Scope = xml_node_soap_get(p_node, "Scope");
	if (p_Scope)
	{
		parse_SearchScope(p_Scope, &p_req->Scope);
	}

	p_MetadataFilter = xml_node_soap_get(p_node, "MetadataFilter");
	if (p_MetadataFilter)
	{
	    XMLN * p_MetadataStreamFilter;

	    p_MetadataStreamFilter = xml_node_soap_get(p_MetadataFilter, "MetadataStreamFilter");
	    if (p_MetadataStreamFilter && p_MetadataStreamFilter->data)
	    {
	        strncpy(p_req->MetadataFilter.MetadataStreamFilter, p_MetadataStreamFilter->data, 
	            sizeof(p_req->MetadataFilter.MetadataStreamFilter)-1);
	    }
	}

	p_MaxMatches = xml_node_soap_get(p_node, "MaxMatches");
	if (p_MaxMatches && p_MaxMatches->data)
	{
		p_req->MaxMatchesFlag = 1;
		p_req->MaxMatches = atoi(p_MaxMatches->data);
	}

	p_KeepAliveTime = xml_node_soap_get(p_node, "KeepAliveTime");
	if (p_KeepAliveTime && p_KeepAliveTime->data)
	{
		p_req->KeepAliveTimeFlag = 1;
		parse_XSDDuration(p_KeepAliveTime->data, &p_req->KeepAliveTime);
	}
	
	return ONVIF_OK;
}

ONVIF_RET parse_GetMetadataSearchResults(XMLN * p_node, GetMetadataSearchResults_REQ * p_req)
{
    XMLN * p_SearchToken;
	XMLN * p_MinResults;
	XMLN * p_MaxResults;
	XMLN * p_WaitTime;

	p_SearchToken = xml_node_soap_get(p_node, "SearchToken");
	if (p_SearchToken && p_SearchToken->data)
	{
		strncpy(p_req->SearchToken, p_SearchToken->data, sizeof(p_req->SearchToken)-1);
	}

	p_MinResults = xml_node_soap_get(p_node, "MinResults");
	if (p_MinResults && p_MinResults->data)
	{
		p_req->MinResultsFlag = 1;
		p_req->MinResults = atoi(p_MinResults->data);
	}

	p_MaxResults = xml_node_soap_get(p_node, "MaxResults");
	if (p_MaxResults && p_MaxResults->data)
	{
		p_req->MaxResultsFlag = 1;
		p_req->MaxResults = atoi(p_MaxResults->data);
	}

	p_WaitTime = xml_node_soap_get(p_node, "WaitTime");
	if (p_WaitTime && p_WaitTime->data)
	{
		p_req->WaitTimeFlag = 1;
		parse_XSDDuration(p_WaitTime->data, &p_req->WaitTime);
	}
	
	return ONVIF_OK;
}


#ifdef PTZ_SUPPORT

ONVIF_RET parse_PTZPositionFilter(XMLN * p_node, onvif_PTZPositionFilter * p_req)
{
    XMLN * p_MinPosition;
	XMLN * p_MaxPosition;
	XMLN * p_EnterOrExit;

	p_MinPosition = xml_node_soap_get(p_node, "MinPosition");
	if (p_MinPosition)
	{
		parse_PTZVector(p_MinPosition, &p_req->MinPosition);
	}

	p_MaxPosition = xml_node_soap_get(p_node, "MaxPosition");
	if (p_MaxPosition)
	{
		parse_PTZVector(p_MaxPosition, &p_req->MaxPosition);
	}

    p_EnterOrExit = xml_node_soap_get(p_node, "EnterOrExit");
	if (p_EnterOrExit && p_EnterOrExit->data)
	{
		p_req->EnterOrExit = parse_Bool(p_EnterOrExit->data);
	}
	
	return ONVIF_OK;
}

ONVIF_RET parse_FindPTZPosition(XMLN * p_node, FindPTZPosition_REQ * p_req)
{
    XMLN * p_StartPoint;
	XMLN * p_EndPoint;
	XMLN * p_Scope;
	XMLN * p_SearchFilter;
	XMLN * p_MaxMatches;
	XMLN * p_KeepAliveTime;

	p_StartPoint = xml_node_soap_get(p_node, "StartPoint");
	if (p_StartPoint && p_StartPoint->data)
	{
		parse_XSDDatetime(p_StartPoint->data, &p_req->StartPoint);
	}

	p_EndPoint = xml_node_soap_get(p_node, "EndPoint");
	if (p_EndPoint && p_EndPoint->data)
	{
		p_req->EndPointFlag = 1;
		parse_XSDDatetime(p_EndPoint->data, &p_req->EndPoint);
	}

	p_Scope = xml_node_soap_get(p_node, "Scope");
	if (p_Scope)
	{
		parse_SearchScope(p_Scope, &p_req->Scope);
	}

	p_SearchFilter = xml_node_soap_get(p_node, "SearchFilter");
	if (p_SearchFilter && p_SearchFilter->data)
	{
	    parse_PTZPositionFilter(p_SearchFilter, &p_req->SearchFilter);
	}

	p_MaxMatches = xml_node_soap_get(p_node, "MaxMatches");
	if (p_MaxMatches && p_MaxMatches->data)
	{
		p_req->MaxMatchesFlag = 1;
		p_req->MaxMatches = atoi(p_MaxMatches->data);
	}

	p_KeepAliveTime = xml_node_soap_get(p_node, "KeepAliveTime");
	if (p_KeepAliveTime && p_KeepAliveTime->data)
	{
		p_req->KeepAliveTimeFlag = 1;
		parse_XSDDuration(p_KeepAliveTime->data, &p_req->KeepAliveTime);
	}
	
	return ONVIF_OK;
}

ONVIF_RET parse_GetPTZPositionSearchResults(XMLN * p_node, GetPTZPositionSearchResults_REQ * p_req)
{
    XMLN * p_SearchToken;
	XMLN * p_MinResults;
	XMLN * p_MaxResults;
	XMLN * p_WaitTime;

	p_SearchToken = xml_node_soap_get(p_node, "SearchToken");
	if (p_SearchToken && p_SearchToken->data)
	{
		strncpy(p_req->SearchToken, p_SearchToken->data, sizeof(p_req->SearchToken)-1);
	}

	p_MinResults = xml_node_soap_get(p_node, "MinResults");
	if (p_MinResults && p_MinResults->data)
	{
		p_req->MinResultsFlag = 1;
		p_req->MinResults = atoi(p_MinResults->data);
	}

	p_MaxResults = xml_node_soap_get(p_node, "MaxResults");
	if (p_MaxResults && p_MaxResults->data)
	{
		p_req->MaxResultsFlag = 1;
		p_req->MaxResults = atoi(p_MaxResults->data);
	}

	p_WaitTime = xml_node_soap_get(p_node, "WaitTime");
	if (p_WaitTime && p_WaitTime->data)
	{
		p_req->WaitTimeFlag = 1;
		parse_XSDDuration(p_WaitTime->data, &p_req->WaitTime);
	}
	
	return ONVIF_OK;
}

#endif

ONVIF_RET parse_EndSearch(XMLN * p_node, EndSearch_REQ * p_req)
{
	XMLN * p_SearchToken;

	p_SearchToken = xml_node_soap_get(p_node, "SearchToken");
	if (p_SearchToken && p_SearchToken->data)
	{
		strncpy(p_req->SearchToken, p_SearchToken->data, sizeof(p_req->SearchToken)-1);
	}

	return ONVIF_OK;
}

ONVIF_RET parse_GetSearchState(XMLN * p_node, GetSearchState_REQ * p_req)
{
	XMLN * p_SearchToken;

	p_SearchToken = xml_node_soap_get(p_node, "SearchToken");
	if (p_SearchToken && p_SearchToken->data)
	{
		strncpy(p_req->SearchToken, p_SearchToken->data, sizeof(p_req->SearchToken)-1);
	}

	return ONVIF_OK;
}

ONVIF_RET parse_GetReplayUri(XMLN * p_node, GetReplayUri_REQ * p_req)
{
	ONVIF_RET ret = ONVIF_OK;
	XMLN * p_StreamSetup;
	XMLN * p_RecordingToken;

	p_StreamSetup = xml_node_soap_get(p_node, "StreamSetup");
	if (p_StreamSetup)
	{
		ret = parse_StreamSetup(p_StreamSetup, &p_req->StreamSetup);
	}

	p_RecordingToken = xml_node_soap_get(p_node, "RecordingToken");
	if (p_RecordingToken && p_RecordingToken->data)
	{
		strncpy(p_req->RecordingToken, p_RecordingToken->data, sizeof(p_req->RecordingToken)-1);
	}

	return ret;
}

ONVIF_RET parse_SetReplayConfiguration(XMLN * p_node, SetReplayConfiguration_REQ * p_req)
{
	XMLN * p_Configuration;

	p_Configuration = xml_node_soap_get(p_node, "Configuration");
	if (p_Configuration)
	{
		XMLN * p_SessionTimeout;
		
		p_SessionTimeout = xml_node_soap_get(p_Configuration, "SessionTimeout");
		if (p_SessionTimeout && p_SessionTimeout->data)
		{
			parse_XSDDuration(p_SessionTimeout->data, &p_req->SessionTimeout);
		}
	}

	return ONVIF_OK;
}

#endif	// end of PROFILE_G_SUPPORT

#ifdef VIDEO_ANALYTICS

ONVIF_RET parse_GetSupportedRules(XMLN * p_node, GetSupportedRules_REQ * p_req)
{
	XMLN * p_ConfigurationToken;

	p_ConfigurationToken = xml_node_soap_get(p_node, "ConfigurationToken");
	if (p_ConfigurationToken && p_ConfigurationToken->data)
	{
		strncpy(p_req->ConfigurationToken, p_ConfigurationToken->data, sizeof(p_req->ConfigurationToken)-1);
	}

	return ONVIF_OK;
}

ONVIF_RET parse_SimpleItem(XMLN * p_node, onvif_SimpleItem * p_req)
{
	const char * p_Name;
	const char * p_Value;

	p_Name = xml_attr_get(p_node, "Name");
	if (p_Name)
	{
		strncpy(p_req->Name, p_Name, sizeof(p_req->Name));
	}

	p_Value = xml_attr_get(p_node, "Value");
	if (p_Value)
	{
		strncpy(p_req->Value, p_Value, sizeof(p_req->Value));
	}

	return ONVIF_OK;
}

ONVIF_RET parse_ElementItem(XMLN * p_node, onvif_ElementItem * p_req)
{
	const char * p_Name;

	p_Name = xml_attr_get(p_node, "Name");
	if (p_Name)
	{
		strncpy(p_req->Name, p_Name, sizeof(p_req->Name));

		if (p_node->f_child)
		{
		    p_req->AnyFlag = 1;
		    p_req->Any = (char *) malloc(1024);
		    
		    xml_nwrite_buf(p_node->f_child, p_req->Any, 1024);
		}
	}

	return ONVIF_OK;
}

ONVIF_RET parse_Config(XMLN * p_node, onvif_Config * p_req)
{
	XMLN * p_Parameters;	
	const char * p_Name;
	const char * p_Type;
	ONVIF_RET ret;

	p_Name = xml_attr_get(p_node, "Name");
	if (p_Name)
	{
		strncpy(p_req->Name, p_Name, sizeof(p_req->Name));
	}

	p_Type = xml_attr_get(p_node, "Type");
	if (p_Type)
	{
		strncpy(p_req->Type, p_Type, sizeof(p_req->Type));

		if (p_node->l_attrib)
		{
		    p_req->attrFlag = 1;
			snprintf(p_req->attr, sizeof(p_req->attr)-1, "%s=\"%s\"", p_node->l_attrib->name, p_node->l_attrib->data);
		}
	}

	p_Parameters = xml_node_soap_get(p_node, "Parameters");
	if (p_Parameters)
	{	
		XMLN * p_SimpleItem;
		XMLN * p_ElementItem;

		p_SimpleItem = xml_node_soap_get(p_Parameters, "SimpleItem");
		while (p_SimpleItem && soap_strcmp(p_SimpleItem->name, "SimpleItem") == 0)
		{
			ONVIF_SimpleItem * p_simple_item = onvif_add_SimpleItem(&p_req->Parameters.SimpleItem);
			if (p_simple_item)
			{
				ret = parse_SimpleItem(p_SimpleItem, &p_simple_item->SimpleItem);
				if (ONVIF_OK != ret)
				{
					onvif_free_SimpleItems(&p_req->Parameters.SimpleItem);
					break;
				}
			}
			
			p_SimpleItem = p_SimpleItem->next;
		}

		p_ElementItem = xml_node_soap_get(p_Parameters, "ElementItem");
		while (p_ElementItem && soap_strcmp(p_ElementItem->name, "ElementItem") == 0)
		{
			ONVIF_ElementItem * p_element_item = onvif_add_ElementItem(&p_req->Parameters.ElementItem);
			if (p_element_item)
			{
				ret = parse_ElementItem(p_ElementItem, &p_element_item->ElementItem);
				if (ONVIF_OK != ret)
				{
					onvif_free_ElementItems(&p_req->Parameters.ElementItem);
					break;
				}
			}
			
			p_ElementItem = p_ElementItem->next;
		}
	}

	return ONVIF_OK;
}

ONVIF_RET parse_CreateRules(XMLN * p_node, CreateRules_REQ * p_req)
{
	XMLN * p_ConfigurationToken;
	XMLN * p_Rule;
	ONVIF_RET ret;

	p_ConfigurationToken = xml_node_soap_get(p_node, "ConfigurationToken");
	if (p_ConfigurationToken && p_ConfigurationToken->data)
	{
		strncpy(p_req->ConfigurationToken, p_ConfigurationToken->data, sizeof(p_req->ConfigurationToken)-1);
	}

	p_Rule = xml_node_soap_get(p_node, "Rule");
	while (p_Rule && soap_strcmp(p_Rule->name, "Rule") == 0)
	{
		ONVIF_Config * p_config = onvif_add_Config(&p_req->Rule);
		if (p_config)
		{
			ret = parse_Config(p_Rule, &p_config->Config);
			if (ONVIF_OK != ret)
			{
				onvif_free_Configs(&p_req->Rule);
				return ret;
			}
		}
		
		p_Rule = p_Rule->next;
	}
	
	return ONVIF_OK;
}

ONVIF_RET parse_DeleteRules(XMLN * p_node, DeleteRules_REQ * p_req)
{
	XMLN * p_ConfigurationToken;
	XMLN * p_RuleName;

	p_ConfigurationToken = xml_node_soap_get(p_node, "ConfigurationToken");
	if (p_ConfigurationToken && p_ConfigurationToken->data)
	{
		strncpy(p_req->ConfigurationToken, p_ConfigurationToken->data, sizeof(p_req->ConfigurationToken)-1);
	}

	p_RuleName = xml_node_soap_get(p_node, "RuleName");
	while (p_RuleName && p_RuleName->data && soap_strcmp(p_RuleName->name, "RuleName") == 0)
	{	
		int idx = p_req->sizeRuleName;
		
		strncpy(p_req->RuleName[idx], p_RuleName->data, sizeof(p_req->RuleName[idx])-1);
		
		p_req->sizeRuleName++;
		if (p_req->sizeRuleName >= ARRAY_SIZE(p_req->RuleName))
		{
			break;
		}

		p_RuleName = p_RuleName->next;
	}
	
	return ONVIF_OK;
}

ONVIF_RET parse_GetRules(XMLN * p_node, GetRules_REQ * p_req)
{
	XMLN * p_ConfigurationToken;

	p_ConfigurationToken = xml_node_soap_get(p_node, "ConfigurationToken");
	if (p_ConfigurationToken && p_ConfigurationToken->data)
	{
		strncpy(p_req->ConfigurationToken, p_ConfigurationToken->data, sizeof(p_req->ConfigurationToken)-1);
	}

	return ONVIF_OK;
}

ONVIF_RET parse_ModifyRules(XMLN * p_node, ModifyRules_REQ * p_req)
{
	XMLN * p_ConfigurationToken;
	XMLN * p_Rule;
	ONVIF_RET ret;

	p_ConfigurationToken = xml_node_soap_get(p_node, "ConfigurationToken");
	if (p_ConfigurationToken && p_ConfigurationToken->data)
	{
		strncpy(p_req->ConfigurationToken, p_ConfigurationToken->data, sizeof(p_req->ConfigurationToken)-1);
	}

	p_Rule = xml_node_soap_get(p_node, "Rule");
	while (p_Rule && soap_strcmp(p_Rule->name, "Rule") == 0)
	{
		ONVIF_Config * p_config = onvif_add_Config(&p_req->Rule);
		if (p_config)
		{
			ret = parse_Config(p_Rule, &p_config->Config);
			if (ONVIF_OK != ret)
			{
				onvif_free_Configs(&p_req->Rule);
				return ret;
			}
		}
		
		p_Rule = p_Rule->next;
	}
	
	return ONVIF_OK;
}

ONVIF_RET parse_CreateAnalyticsModules(XMLN * p_node, CreateAnalyticsModules_REQ * p_req)
{
	XMLN * p_ConfigurationToken;
	XMLN * p_AnalyticsModule;
	ONVIF_RET ret;

	p_ConfigurationToken = xml_node_soap_get(p_node, "ConfigurationToken");
	if (p_ConfigurationToken && p_ConfigurationToken->data)
	{
		strncpy(p_req->ConfigurationToken, p_ConfigurationToken->data, sizeof(p_req->ConfigurationToken)-1);
	}

	p_AnalyticsModule = xml_node_soap_get(p_node, "AnalyticsModule");
	while (p_AnalyticsModule && soap_strcmp(p_AnalyticsModule->name, "AnalyticsModule") == 0)
	{
		ONVIF_Config * p_config = onvif_add_Config(&p_req->AnalyticsModule);
		if (p_config)
		{
			ret = parse_Config(p_AnalyticsModule, &p_config->Config);
			if (ONVIF_OK != ret)
			{
				onvif_free_Configs(&p_req->AnalyticsModule);
				return ret;
			}
		}
		
		p_AnalyticsModule = p_AnalyticsModule->next;
	}
	
	return ONVIF_OK;
}

ONVIF_RET parse_DeleteAnalyticsModules(XMLN * p_node, DeleteAnalyticsModules_REQ * p_req)
{
	XMLN * p_ConfigurationToken;
	XMLN * p_AnalyticsModuleName;

	p_ConfigurationToken = xml_node_soap_get(p_node, "ConfigurationToken");
	if (p_ConfigurationToken && p_ConfigurationToken->data)
	{
		strncpy(p_req->ConfigurationToken, p_ConfigurationToken->data, sizeof(p_req->ConfigurationToken)-1);
	}

	p_AnalyticsModuleName = xml_node_soap_get(p_node, "AnalyticsModuleName");
	while (p_AnalyticsModuleName && p_AnalyticsModuleName->data && soap_strcmp(p_AnalyticsModuleName->name, "AnalyticsModuleName") == 0)
	{	
		int idx = p_req->sizeAnalyticsModuleName;
		
		strncpy(p_req->AnalyticsModuleName[idx], p_AnalyticsModuleName->data, sizeof(p_req->AnalyticsModuleName[idx])-1);
		
		p_req->sizeAnalyticsModuleName++;
		if (p_req->sizeAnalyticsModuleName >= ARRAY_SIZE(p_req->AnalyticsModuleName))
		{
			break;
		}

		p_AnalyticsModuleName = p_AnalyticsModuleName->next;
	}
	
	return ONVIF_OK;
}

ONVIF_RET parse_GetAnalyticsModules(XMLN * p_node, GetAnalyticsModules_REQ * p_req)
{
	XMLN * p_ConfigurationToken;

	p_ConfigurationToken = xml_node_soap_get(p_node, "ConfigurationToken");
	if (p_ConfigurationToken && p_ConfigurationToken->data)
	{
		strncpy(p_req->ConfigurationToken, p_ConfigurationToken->data, sizeof(p_req->ConfigurationToken)-1);
	}

	return ONVIF_OK;
}

ONVIF_RET parse_ModifyAnalyticsModules(XMLN * p_node, ModifyAnalyticsModules_REQ * p_req)
{
	XMLN * p_ConfigurationToken;
	XMLN * p_AnalyticsModule;
	ONVIF_RET ret;

	p_ConfigurationToken = xml_node_soap_get(p_node, "ConfigurationToken");
	if (p_ConfigurationToken && p_ConfigurationToken->data)
	{
		strncpy(p_req->ConfigurationToken, p_ConfigurationToken->data, sizeof(p_req->ConfigurationToken)-1);
	}

	p_AnalyticsModule = xml_node_soap_get(p_node, "AnalyticsModule");
	while (p_AnalyticsModule && soap_strcmp(p_AnalyticsModule->name, "AnalyticsModule") == 0)
	{
		ONVIF_Config * p_config = onvif_add_Config(&p_req->AnalyticsModule);
		if (p_config)
		{
			ret = parse_Config(p_AnalyticsModule, &p_config->Config);
			if (ONVIF_OK != ret)
			{
				onvif_free_Configs(&p_req->AnalyticsModule);
				return ret;
			}
		}
		
		p_AnalyticsModule = p_AnalyticsModule->next;
	}
	
	return ONVIF_OK;
}

ONVIF_RET parse_AddVideoAnalyticsConfiguration(XMLN * p_node, AddVideoAnalyticsConfiguration_REQ * p_req)
{
	XMLN * p_ProfileToken;
	XMLN * p_ConfigurationToken;

	p_ProfileToken = xml_node_soap_get(p_node, "ProfileToken");
	if (p_ProfileToken && p_ProfileToken->data)
	{
		strncpy(p_req->ProfileToken, p_ProfileToken->data, sizeof(p_req->ProfileToken)-1);
	}

	p_ConfigurationToken = xml_node_soap_get(p_node, "ConfigurationToken");
	if (p_ConfigurationToken && p_ConfigurationToken->data)
	{
		strncpy(p_req->ConfigurationToken, p_ConfigurationToken->data, sizeof(p_req->ConfigurationToken)-1);
	}

	return ONVIF_OK;
}

ONVIF_RET parse_GetVideoAnalyticsConfiguration(XMLN * p_node, GetVideoAnalyticsConfiguration_REQ * p_req)
{
	XMLN * p_ConfigurationToken;

	p_ConfigurationToken = xml_node_soap_get(p_node, "ConfigurationToken");
	if (p_ConfigurationToken && p_ConfigurationToken->data)
	{
		strncpy(p_req->ConfigurationToken, p_ConfigurationToken->data, sizeof(p_req->ConfigurationToken)-1);
	}

	return ONVIF_OK;
}

ONVIF_RET parse_RemoveVideoAnalyticsConfiguration(XMLN * p_node, RemoveVideoAnalyticsConfiguration_REQ * p_req)
{
	XMLN * p_ProfileToken;

	p_ProfileToken = xml_node_soap_get(p_node, "ProfileToken");
	if (p_ProfileToken && p_ProfileToken->data)
	{
		strncpy(p_req->ProfileToken, p_ProfileToken->data, sizeof(p_req->ProfileToken)-1);
	}

	return ONVIF_OK;
}

ONVIF_RET parse_AnalyticsEngineConfiguration(XMLN * p_node, onvif_AnalyticsEngineConfiguration * p_req)
{
	XMLN * p_AnalyticsModule;
	ONVIF_Config * p_config;

	p_AnalyticsModule = xml_node_soap_get(p_node, "AnalyticsModule");
	while (p_AnalyticsModule && soap_strcmp(p_AnalyticsModule->name, "AnalyticsModule") == 0)
	{
		p_config = onvif_add_Config(&p_req->AnalyticsModule);
		if (p_config)
		{
			parse_Config(p_AnalyticsModule, &p_config->Config);
		}
		
		p_AnalyticsModule = p_AnalyticsModule->next;
	}

	return ONVIF_OK;
}

ONVIF_RET parse_RuleEngineConfiguration(XMLN * p_node, onvif_RuleEngineConfiguration * p_req)
{
	XMLN * p_Rule;
	ONVIF_Config * p_config;

	p_Rule = xml_node_soap_get(p_node, "Rule");
	while (p_Rule && soap_strcmp(p_Rule->name, "Rule") == 0)
	{
		p_config = onvif_add_Config(&p_req->Rule);
		if (p_config)
		{
			parse_Config(p_Rule, &p_config->Config);
		}
		
		p_Rule = p_Rule->next;
	}

	return ONVIF_OK;
}

ONVIF_RET parse_VideoAnalyticsConfiguration(XMLN * p_node, onvif_VideoAnalyticsConfiguration * p_req)
{
	XMLN * p_Name;
	XMLN * p_UseCount;
	XMLN * p_AnalyticsEngineConfiguration;
	XMLN * p_RuleEngineConfiguration;	
	const char * p_token;
	ONVIF_RET ret = ONVIF_OK;

	p_token = xml_attr_get(p_node, "token");
	if (p_token)
	{
		strncpy(p_req->token, p_token, sizeof(p_req->token)-1);
	}
	
	p_Name = xml_node_soap_get(p_node, "Name");
	if (p_Name && p_Name->data)
	{
		strncpy(p_req->Name, p_Name->data, sizeof(p_req->Name)-1);
	}

	p_UseCount = xml_node_soap_get(p_node, "UseCount");
	if (p_UseCount && p_UseCount->data)
	{
		p_req->UseCount = atoi(p_UseCount->data);
	}

	p_AnalyticsEngineConfiguration = xml_node_soap_get(p_node, "AnalyticsEngineConfiguration");
	if (p_AnalyticsEngineConfiguration)
	{
		ret = parse_AnalyticsEngineConfiguration(p_AnalyticsEngineConfiguration, &p_req->AnalyticsEngineConfiguration);
		if (ONVIF_OK != ret)
		{
			return ret;
		}
	}

	p_RuleEngineConfiguration = xml_node_soap_get(p_node, "RuleEngineConfiguration");
	if (p_RuleEngineConfiguration)
	{
		ret = parse_RuleEngineConfiguration(p_RuleEngineConfiguration, &p_req->RuleEngineConfiguration);
	}
	
	return ret;
}

ONVIF_RET parse_SetVideoAnalyticsConfiguration(XMLN * p_node, SetVideoAnalyticsConfiguration_REQ * p_req)
{
	XMLN * p_Configuration;
	XMLN * p_ForcePersistence;
	ONVIF_RET ret = ONVIF_ERR_MissingAttribute;

	p_Configuration = xml_node_soap_get(p_node, "Configuration");
	if (p_Configuration)
	{
		ret = parse_VideoAnalyticsConfiguration(p_Configuration, &p_req->Configuration);
	}

	p_ForcePersistence = xml_node_soap_get(p_node, "ForcePersistence");
	if (p_ForcePersistence && p_ForcePersistence->data)
	{
		p_req->ForcePersistence = parse_Bool(p_ForcePersistence->data);
	}
	
	return ret;
}

ONVIF_RET parse_GetRuleOptions(XMLN * p_node, GetRuleOptions_REQ * p_req)
{
    XMLN * p_RuleType;
	XMLN * p_ConfigurationToken;

	p_RuleType = xml_node_soap_get(p_node, "RuleType");
	if (p_RuleType && p_RuleType->data)
	{
		strncpy(p_req->RuleType, p_RuleType->data, sizeof(p_req->RuleType));
	}

	p_ConfigurationToken = xml_node_soap_get(p_node, "ConfigurationToken");
	if (p_ConfigurationToken && p_ConfigurationToken->data)
	{
		strncpy(p_req->ConfigurationToken, p_ConfigurationToken->data, sizeof(p_req->ConfigurationToken));
	}

	return ONVIF_OK;
}

ONVIF_RET parse_GetSupportedAnalyticsModules(XMLN * p_node, GetSupportedAnalyticsModules_REQ * p_req)
{
	XMLN * p_ConfigurationToken;
	
	p_ConfigurationToken = xml_node_soap_get(p_node, "ConfigurationToken");
	if (p_ConfigurationToken && p_ConfigurationToken->data)
	{
		strncpy(p_req->ConfigurationToken, p_ConfigurationToken->data, sizeof(p_req->ConfigurationToken));
	}

	return ONVIF_OK;
}

ONVIF_RET parse_GetAnalyticsModuleOptions(XMLN * p_node, GetAnalyticsModuleOptions_REQ * p_req)
{
    XMLN * p_Type;
	XMLN * p_ConfigurationToken;

	p_Type = xml_node_soap_get(p_node, "Type");
	if (p_Type && p_Type->data)
	{
		strncpy(p_req->Type, p_Type->data, sizeof(p_req->Type));
	}

	p_ConfigurationToken = xml_node_soap_get(p_node, "ConfigurationToken");
	if (p_ConfigurationToken && p_ConfigurationToken->data)
	{
		strncpy(p_req->ConfigurationToken, p_ConfigurationToken->data, sizeof(p_req->ConfigurationToken));
	}

	return ONVIF_OK;
}


#endif	// end of VIDEO_ANALYTICS

#ifdef PROFILE_C_SUPPORT

ONVIF_RET parse_tac_GetAccessPointInfoList(XMLN * p_node, tac_GetAccessPointInfoList_REQ * p_req)
{
    XMLN * p_Limit;
	XMLN * p_StartReference;

	p_Limit = xml_node_soap_get(p_node, "Limit");
	if (p_Limit && p_Limit->data)
	{
		p_req->LimitFlag = 1;
		p_req->Limit = atoi(p_Limit->data);
	}

	p_StartReference = xml_node_soap_get(p_node, "StartReference");
	if (p_StartReference && p_StartReference->data)
	{
		p_req->StartReferenceFlag = 1;
		strncpy(p_req->StartReference, p_StartReference->data, sizeof(p_req->StartReference)-1);
	}

	return ONVIF_OK;
}

ONVIF_RET parse_tac_GetAccessPointInfo(XMLN * p_node, tac_GetAccessPointInfo_REQ * p_req)
{
    int idx = 0;
    XMLN * p_Token;

    p_Token = xml_node_soap_get(p_node, "Token");
    while (p_Token && p_Token->data && soap_strcmp(p_Token->name, "Token") == 0)
    {
        strncpy(p_req->token[idx], p_Token->data, sizeof(p_req->token[idx])-1);

        if (++idx >= ARRAY_SIZE(p_req->token))
        {
            return ONVIF_ERR_TooManyItems;
        }
        
        p_Token = p_Token->next;
    }

    return ONVIF_OK;
}

ONVIF_RET parse_tac_GetAreaInfoList(XMLN * p_node, tac_GetAreaInfoList_REQ * p_req)
{
    XMLN * p_Limit;
	XMLN * p_StartReference;

	p_Limit = xml_node_soap_get(p_node, "Limit");
	if (p_Limit && p_Limit->data)
	{
		p_req->LimitFlag = 1;
		p_req->Limit = atoi(p_Limit->data);
	}

	p_StartReference = xml_node_soap_get(p_node, "StartReference");
	if (p_StartReference && p_StartReference->data)
	{
		p_req->StartReferenceFlag = 1;
		strncpy(p_req->StartReference, p_StartReference->data, sizeof(p_req->StartReference)-1);
	}

	return ONVIF_OK;
}

ONVIF_RET parse_tac_GetAreaInfo(XMLN * p_node, tac_GetAreaInfo_REQ * p_req)
{
    int idx = 0;
    XMLN * p_Token;

    p_Token = xml_node_soap_get(p_node, "Token");
    while (p_Token && p_Token->data && soap_strcmp(p_Token->name, "Token") == 0)
    {
        strncpy(p_req->token[idx], p_Token->data, sizeof(p_req->token[idx])-1);

        if (++idx >= ARRAY_SIZE(p_req->token))
        {
            return ONVIF_ERR_TooManyItems;
        }
        
        p_Token = p_Token->next;
    }

    return ONVIF_OK;
}

ONVIF_RET parse_tac_GetAccessPointState(XMLN * p_node, tac_GetAccessPointState_REQ * p_req)
{
    XMLN * p_Token;

    p_Token = xml_node_soap_get(p_node, "Token");
    if (p_Token && p_Token->data)
    {
        strncpy(p_req->Token, p_Token->data, sizeof(p_req->Token)-1);
    }

    return ONVIF_OK;
}

ONVIF_RET parse_tac_EnableAccessPoint(XMLN * p_node, tac_EnableAccessPoint_REQ * p_req)
{
    XMLN * p_Token;

    p_Token = xml_node_soap_get(p_node, "Token");
    if (p_Token && p_Token->data)
    {
        strncpy(p_req->Token, p_Token->data, sizeof(p_req->Token)-1);
    }

    return ONVIF_OK;
}

ONVIF_RET parse_tac_DisableAccessPoint(XMLN * p_node, tac_DisableAccessPoint_REQ * p_req)
{
    XMLN * p_Token;

    p_Token = xml_node_soap_get(p_node, "Token");
    if (p_Token && p_Token->data)
    {
        strncpy(p_req->Token, p_Token->data, sizeof(p_req->Token)-1);
    }

    return ONVIF_OK;
}

ONVIF_RET parse_tdc_GetDoorInfoList(XMLN * p_node, tdc_GetDoorInfoList_REQ * p_req)
{
    XMLN * p_Limit;
	XMLN * p_StartReference;

	p_Limit = xml_node_soap_get(p_node, "Limit");
	if (p_Limit && p_Limit->data)
	{
		p_req->LimitFlag = 1;
		p_req->Limit = atoi(p_Limit->data);
	}

	p_StartReference = xml_node_soap_get(p_node, "StartReference");
	if (p_StartReference && p_StartReference->data)
	{
		p_req->StartReferenceFlag = 1;
		strncpy(p_req->StartReference, p_StartReference->data, sizeof(p_req->StartReference)-1);
	}

	return ONVIF_OK;
}

ONVIF_RET parse_tdc_GetDoorInfo(XMLN * p_node, tdc_GetDoorInfo_REQ * p_req)
{
    int idx = 0;
    XMLN * p_Token;

    p_Token = xml_node_soap_get(p_node, "Token");
    while (p_Token && p_Token->data && soap_strcmp(p_Token->name, "Token") == 0)
    {
        strncpy(p_req->token[idx], p_Token->data, sizeof(p_req->token[idx])-1);

        if (++idx >= ARRAY_SIZE(p_req->token))
        {
            return ONVIF_ERR_TooManyItems;
        }
        
        p_Token = p_Token->next;
    }

    return ONVIF_OK;
}

ONVIF_RET parse_tdc_GetDoorState(XMLN * p_node, tdc_GetDoorState_REQ * p_req)
{
    XMLN * p_Token;

    p_Token = xml_node_soap_get(p_node, "Token");
    if (p_Token && p_Token->data)
    {
        strncpy(p_req->Token, p_Token->data, sizeof(p_req->Token)-1);
    }

    return ONVIF_OK;
}

ONVIF_RET parse_tdc_AccessDoor(XMLN * p_node, tdc_AccessDoor_REQ * p_req)
{
    XMLN * p_Token;
    XMLN * p_UseExtendedTime;
    XMLN * p_AccessTime;
    XMLN * p_OpenTooLongTime;
    XMLN * p_PreAlarmTime;
    
    p_Token = xml_node_soap_get(p_node, "Token");
    if (p_Token && p_Token->data)
    {
        strncpy(p_req->Token, p_Token->data, sizeof(p_req->Token)-1);
    }

    p_UseExtendedTime = xml_node_soap_get(p_node, "UseExtendedTime");
    if (p_UseExtendedTime && p_UseExtendedTime->data)
    {
        p_req->UseExtendedTimeFlag = 1;
        p_req->UseExtendedTime = parse_Bool(p_UseExtendedTime->data);
    }

    p_AccessTime = xml_node_soap_get(p_node, "AccessTime");
    if (p_AccessTime && p_AccessTime->data)
    {
        p_req->AccessTimeFlag = 1;
        parse_XSDDuration(p_AccessTime->data, &p_req->AccessTime);
    }

    p_OpenTooLongTime = xml_node_soap_get(p_node, "AccessTime");
    if (p_OpenTooLongTime && p_OpenTooLongTime->data)
    {
        p_req->OpenTooLongTimeFlag = 1;
        parse_XSDDuration(p_OpenTooLongTime->data, &p_req->OpenTooLongTime);
    }

    p_PreAlarmTime = xml_node_soap_get(p_node, "PreAlarmTime");
    if (p_PreAlarmTime && p_PreAlarmTime->data)
    {
        p_req->PreAlarmTimeFlag = 1;
        parse_XSDDuration(p_PreAlarmTime->data, &p_req->PreAlarmTime);
    }

    return ONVIF_OK;    
}

ONVIF_RET parse_tdc_LockDoor(XMLN * p_node, tdc_LockDoor_REQ * p_req)
{
    XMLN * p_Token;
    
    p_Token = xml_node_soap_get(p_node, "Token");
    if (p_Token && p_Token->data)
    {
        strncpy(p_req->Token, p_Token->data, sizeof(p_req->Token)-1);
    }

    return ONVIF_OK;
}

ONVIF_RET parse_tdc_UnlockDoor(XMLN * p_node, tdc_UnlockDoor_REQ * p_req)
{
    XMLN * p_Token;
    
    p_Token = xml_node_soap_get(p_node, "Token");
    if (p_Token && p_Token->data)
    {
        strncpy(p_req->Token, p_Token->data, sizeof(p_req->Token)-1);
    }

    return ONVIF_OK;
}

ONVIF_RET parse_tdc_DoubleLockDoor(XMLN * p_node, tdc_DoubleLockDoor_REQ * p_req)
{
    XMLN * p_Token;
    
    p_Token = xml_node_soap_get(p_node, "Token");
    if (p_Token && p_Token->data)
    {
        strncpy(p_req->Token, p_Token->data, sizeof(p_req->Token)-1);
    }

    return ONVIF_OK;
}

ONVIF_RET parse_tdc_BlockDoor(XMLN * p_node, tdc_BlockDoor_REQ * p_req)
{
    XMLN * p_Token;
    
    p_Token = xml_node_soap_get(p_node, "Token");
    if (p_Token && p_Token->data)
    {
        strncpy(p_req->Token, p_Token->data, sizeof(p_req->Token)-1);
    }

    return ONVIF_OK;
}

ONVIF_RET parse_tdc_LockDownDoor(XMLN * p_node, tdc_LockDownDoor_REQ * p_req)
{
    XMLN * p_Token;
    
    p_Token = xml_node_soap_get(p_node, "Token");
    if (p_Token && p_Token->data)
    {
        strncpy(p_req->Token, p_Token->data, sizeof(p_req->Token)-1);
    }

    return ONVIF_OK;
}

ONVIF_RET parse_tdc_LockDownReleaseDoor(XMLN * p_node, tdc_LockDownReleaseDoor_REQ * p_req)
{
    XMLN * p_Token;
    
    p_Token = xml_node_soap_get(p_node, "Token");
    if (p_Token && p_Token->data)
    {
        strncpy(p_req->Token, p_Token->data, sizeof(p_req->Token)-1);
    }

    return ONVIF_OK;
}

ONVIF_RET parse_tdc_LockOpenDoor(XMLN * p_node, tdc_LockOpenDoor_REQ * p_req)
{
    XMLN * p_Token;
    
    p_Token = xml_node_soap_get(p_node, "Token");
    if (p_Token && p_Token->data)
    {
        strncpy(p_req->Token, p_Token->data, sizeof(p_req->Token)-1);
    }

    return ONVIF_OK;
}

ONVIF_RET parse_tdc_LockOpenReleaseDoor(XMLN * p_node, tdc_LockOpenReleaseDoor_REQ * p_req)
{
    XMLN * p_Token;
    
    p_Token = xml_node_soap_get(p_node, "Token");
    if (p_Token && p_Token->data)
    {
        strncpy(p_req->Token, p_Token->data, sizeof(p_req->Token)-1);
    }

    return ONVIF_OK;
}

#endif  // end of PROFILE_C_SUPPORT

#ifdef DEVICEIO_SUPPORT

ONVIF_RET parse_GetVideoOutputConfiguration(XMLN * p_node, GetVideoOutputConfiguration_REQ * p_req)
{
    XMLN * p_VideoOutputToken;

    p_VideoOutputToken = xml_node_soap_get(p_node, "VideoOutputToken");
    if (p_VideoOutputToken && p_VideoOutputToken->data)
    {
        strncpy(p_req->VideoOutputToken, p_VideoOutputToken->data, sizeof(p_req->VideoOutputToken)-1);
    }

    return ONVIF_OK;
}

ONVIF_RET parse_SetVideoOutputConfiguration(XMLN * p_node, SetVideoOutputConfiguration_REQ * p_req)
{
    XMLN * p_Configuration;
    XMLN * p_ForcePersistence;

    p_Configuration = xml_node_soap_get(p_node, "Configuration");
    if (NULL == p_Configuration)
    {
        XMLN * p_Name;
        XMLN * p_UseCount;
        XMLN * p_OutputToken;
        const char * p_token;

        p_token = xml_attr_get(p_Configuration, "token");
        if (p_token)
        {
            strncpy(p_req->Configuration.token, p_token, sizeof(p_req->Configuration.token)-1);
        }

        p_Name = xml_node_soap_get(p_Configuration, "Name");
        if (p_Name && p_Name->data)
        {
            strncpy(p_req->Configuration.Name, p_Name->data, sizeof(p_req->Configuration.Name)-1);
        }

        p_UseCount = xml_node_soap_get(p_Configuration, "UseCount");
        if (p_UseCount && p_UseCount->data)
        {
            p_req->Configuration.UseCount = atoi(p_UseCount->data);
        }

        p_OutputToken = xml_node_soap_get(p_Configuration, "OutputToken");
        if (p_OutputToken && p_OutputToken->data)
        {
            strncpy(p_req->Configuration.OutputToken, p_OutputToken->data, sizeof(p_req->Configuration.OutputToken)-1);
        }        
    }

    p_ForcePersistence = xml_node_soap_get(p_node, "ForcePersistence");
    if (p_ForcePersistence && p_ForcePersistence->data)
    {
        p_req->ForcePersistence = parse_Bool(p_ForcePersistence->data);
    }
    
    return ONVIF_OK;
}

ONVIF_RET parse_GetVideoOutputConfigurationOptions(XMLN * p_node, GetVideoOutputConfigurationOptions_REQ * p_req)
{
    XMLN * p_VideoOutputToken;

    p_VideoOutputToken = xml_node_soap_get(p_node, "VideoOutputToken");
    if (p_VideoOutputToken && p_VideoOutputToken->data)
    {
        strncpy(p_req->VideoOutputToken, p_VideoOutputToken->data, sizeof(p_req->VideoOutputToken)-1);
    }

    return ONVIF_OK;
}

ONVIF_RET parse_trt_GetAudioOutputConfiguration(XMLN * p_node, trt_GetAudioOutputConfiguration_REQ * p_req)
{
    XMLN * p_ConfigurationToken;

    p_ConfigurationToken = xml_node_soap_get(p_node, "ConfigurationToken");
    if (p_ConfigurationToken && p_ConfigurationToken->data)
    {
        strncpy(p_req->ConfigurationToken, p_ConfigurationToken->data, sizeof(p_req->ConfigurationToken)-1);
    }

    return ONVIF_OK;
}

ONVIF_RET parse_GetAudioOutputConfiguration(XMLN * p_node, GetAudioOutputConfiguration_REQ * p_req)
{
    XMLN * p_AudioOutputToken;

    p_AudioOutputToken = xml_node_soap_get(p_node, "AudioOutputToken");
    if (p_AudioOutputToken && p_AudioOutputToken->data)
    {
        strncpy(p_req->AudioOutputToken, p_AudioOutputToken->data, sizeof(p_req->AudioOutputToken)-1);
    }

    return ONVIF_OK;
}

ONVIF_RET parse_AudioOutputConfiguration(XMLN * p_node, onvif_AudioOutputConfiguration * p_req)
{
    XMLN * p_Name;
    XMLN * p_UseCount;
    XMLN * p_OutputToken;
    XMLN * p_SendPrimacy;
    XMLN * p_OutputLevel;
    const char * p_token;

    p_token = xml_attr_get(p_node, "token");
    if (p_token)
    {
        strncpy(p_req->token, p_token, sizeof(p_req->token)-1);
    }

    p_Name = xml_node_soap_get(p_node, "Name");
    if (p_Name && p_Name->data)
    {
        strncpy(p_req->Name, p_Name->data, sizeof(p_req->Name)-1);
    }

    p_UseCount = xml_node_soap_get(p_node, "UseCount");
    if (p_UseCount && p_UseCount->data)
    {
        p_req->UseCount = atoi(p_UseCount->data);
    }

    p_OutputToken = xml_node_soap_get(p_node, "OutputToken");
    if (p_OutputToken && p_OutputToken->data)
    {
        strncpy(p_req->OutputToken, p_OutputToken->data, sizeof(p_req->OutputToken)-1);
    }

    p_SendPrimacy = xml_node_soap_get(p_node, "SendPrimacy");
    if (p_SendPrimacy && p_SendPrimacy->data)
    {
        p_req->SendPrimacyFlag = 1;
        strncpy(p_req->SendPrimacy, p_SendPrimacy->data, sizeof(p_req->SendPrimacy)-1);
    }

    p_OutputLevel = xml_node_soap_get(p_node, "OutputLevel");
    if (p_OutputLevel && p_OutputLevel->data)
    {
        p_req->OutputLevel = atoi(p_OutputLevel->data);
    }

    return ONVIF_OK;
}

ONVIF_RET parse_SetAudioOutputConfiguration(XMLN * p_node, SetAudioOutputConfiguration_REQ * p_req)
{
    XMLN * p_Configuration;
    XMLN * p_ForcePersistence;

    p_Configuration = xml_node_soap_get(p_node, "Configuration");
    if (p_Configuration)
    {
        parse_AudioOutputConfiguration(p_Configuration, &p_req->Configuration);
    }

    p_ForcePersistence = xml_node_soap_get(p_node, "ForcePersistence");
    if (p_ForcePersistence && p_ForcePersistence->data)
    {
        p_req->ForcePersistence = parse_Bool(p_ForcePersistence->data);
    }

    return ONVIF_OK;
}

ONVIF_RET parse_GetAudioOutputConfigurationOptions(XMLN * p_node, GetAudioOutputConfigurationOptions_REQ * p_req)
{
    XMLN * p_AudioOutputToken;

    p_AudioOutputToken = xml_node_soap_get(p_node, "AudioOutputToken");
    if (p_AudioOutputToken && p_AudioOutputToken->data)
    {
        strncpy(p_req->AudioOutputToken, p_AudioOutputToken->data, sizeof(p_req->AudioOutputToken)-1);
    }

    return ONVIF_OK;
}

ONVIF_RET parse_trt_GetAudioOutputConfigurationOptions(XMLN * p_node, trt_GetAudioOutputConfigurationOptions_REQ * p_req)
{
    XMLN * p_ConfigurationToken;
    XMLN * p_ProfileToken;

    p_ConfigurationToken = xml_node_soap_get(p_node, "ConfigurationToken");
    if (p_ConfigurationToken && p_ConfigurationToken->data)
    {
        p_req->ConfigurationTokenFlag = 1;
        strncpy(p_req->ConfigurationToken, p_ConfigurationToken->data, sizeof(p_req->ConfigurationToken)-1);
    }

    p_ProfileToken = xml_node_soap_get(p_node, "ProfileToken");
    if (p_ProfileToken && p_ProfileToken->data)
    {
        p_req->ProfileTokenFlag = 1;
        strncpy(p_req->ProfileToken, p_ProfileToken->data, sizeof(p_req->ProfileToken)-1);
    }

    return ONVIF_OK;
}

ONVIF_RET parse_GetRelayOutputOptions(XMLN * p_node, GetRelayOutputOptions_REQ * p_req)
{
    XMLN * p_RelayOutputToken;

    p_RelayOutputToken = xml_node_soap_get(p_node, "RelayOutputToken");
    if (p_RelayOutputToken && p_RelayOutputToken->data)
    {
        p_req->RelayOutputTokenFlag = 1;
        strncpy(p_req->RelayOutputToken, p_RelayOutputToken->data, sizeof(p_req->RelayOutputToken)-1);
    }

    return ONVIF_OK;
}

ONVIF_RET parse_RelayOutputSettings(XMLN * p_node, onvif_RelayOutputSettings * p_req)
{
    XMLN * p_Mode;
    XMLN * p_DelayTime;
    XMLN * p_IdleState;

    p_Mode = xml_node_soap_get(p_node, "Mode");
    if (p_Mode && p_Mode->data)
    {
        p_req->Mode = onvif_StringToRelayMode(p_Mode->data);
    }

    p_DelayTime = xml_node_soap_get(p_node, "DelayTime");
    if (p_DelayTime && p_DelayTime->data)
    {
        parse_XSDDuration(p_DelayTime->data, (int*)&p_req->DelayTime);
    }

    p_IdleState = xml_node_soap_get(p_node, "IdleState");
    if (p_IdleState && p_IdleState->data)
    {
        p_req->IdleState = onvif_StringToRelayIdleState(p_IdleState->data);
    }

    return ONVIF_OK;
}

ONVIF_RET parse_SetRelayOutputSettings(XMLN * p_node, SetRelayOutputSettings_REQ * p_req)
{
    XMLN * p_RelayOutput;
    
    p_RelayOutput = xml_node_soap_get(p_node, "RelayOutput");
    if (p_RelayOutput)
    {
        XMLN * p_Properties;
        const char * p_token;

        p_token = xml_attr_get(p_RelayOutput, "token");
        if (p_token)
        {
            strncpy(p_req->RelayOutput.token, p_token, sizeof(p_req->RelayOutput.token)-1);
        }
        
        p_Properties = xml_node_soap_get(p_RelayOutput, "Properties");
        if (p_Properties)
        {
            parse_RelayOutputSettings(p_Properties, &p_req->RelayOutput.Properties);
        }
    }

    return ONVIF_OK;    
}

ONVIF_RET parse_trt_SetRelayOutputSettings(XMLN * p_node, SetRelayOutputSettings_REQ * p_req)
{
    XMLN * p_RelayOutputToken;
    XMLN * p_Properties;

    p_RelayOutputToken = xml_node_soap_get(p_node, "RelayOutputToken");
    if (p_RelayOutputToken && p_RelayOutputToken->data)
    {
        strncpy(p_req->RelayOutput.token, p_RelayOutputToken->data, sizeof(p_req->RelayOutput.token)-1);
    }    

    p_Properties = xml_node_soap_get(p_node, "Properties");
    if (p_Properties)
    {    
        parse_RelayOutputSettings(p_Properties, &p_req->RelayOutput.Properties); 
    }

    return ONVIF_OK;
}

ONVIF_RET parse_SetRelayOutputState(XMLN * p_node, SetRelayOutputState_REQ * p_req)
{
    XMLN * p_RelayOutputToken;
    XMLN * p_LogicalState;

    p_RelayOutputToken = xml_node_soap_get(p_node, "RelayOutputToken");
    if (p_RelayOutputToken && p_RelayOutputToken->data)
    {
        strncpy(p_req->RelayOutputToken, p_RelayOutputToken->data, sizeof(p_req->RelayOutputToken)-1);
    }

    p_LogicalState = xml_node_soap_get(p_node, "LogicalState");
    if (p_LogicalState && p_LogicalState->data)
    {
        p_req->LogicalState = onvif_StringToRelayLogicalState(p_LogicalState->data);
    }

    return ONVIF_OK;
}

ONVIF_RET parse_GetDigitalInputConfigurationOptions(XMLN * p_node, GetDigitalInputConfigurationOptions_REQ * p_req)
{
    XMLN * p_Token;

    p_Token = xml_node_soap_get(p_node, "Token");
    if (p_Token && p_Token->data)
    {
        p_req->TokenFlag = 1;
        strncpy(p_req->Token, p_Token->data, sizeof(p_req->Token)-1);
    }

    return ONVIF_OK;
}

ONVIF_RET parse_SetDigitalInputConfigurations(XMLN * p_node, SetDigitalInputConfigurations_REQ * p_req)
{
    XMLN * p_DigitalInputs;

    p_DigitalInputs = xml_node_soap_get(p_node, "DigitalInputs");
    while (p_DigitalInputs && soap_strcmp(p_DigitalInputs->name, "DigitalInputs") == 0)
    {
        ONVIF_DigitalInput * p_input = onvif_add_DigitalInput(&p_req->DigitalInputs);
        if (p_input)
        {
            const char * p_token;
            const char * p_IdleState;

            p_token = xml_attr_get(p_DigitalInputs, "token");
            if (p_token)
            {
                strncpy(p_input->DigitalInput.token, p_token, sizeof(p_input->DigitalInput.token)-1);
            }

            p_IdleState = xml_attr_get(p_DigitalInputs, "IdleState");
            if (p_IdleState)
            {
                p_input->DigitalInput.IdleStateFlag = 1;
                p_input->DigitalInput.IdleState = onvif_StringToDigitalIdleState(p_IdleState);
            }
        }
        
        p_DigitalInputs = p_DigitalInputs->next;
    }

    return ONVIF_OK;
}

ONVIF_RET parse_GetSerialPortConfiguration(XMLN * p_node, GetSerialPortConfiguration_REQ * p_req)
{
    XMLN * p_SerialPortToken;

    p_SerialPortToken = xml_node_soap_get(p_node, "SerialPortToken");
    if (p_SerialPortToken && p_SerialPortToken->data)
    {
        strncpy(p_req->SerialPortToken, p_SerialPortToken->data, sizeof(p_req->SerialPortToken)-1);
    }

    return ONVIF_OK;
}

ONVIF_RET parse_GetSerialPortConfigurationOptions(XMLN * p_node, GetSerialPortConfigurationOptions_REQ * p_req)
{
    XMLN * p_SerialPortToken;

    p_SerialPortToken = xml_node_soap_get(p_node, "SerialPortToken");
    if (p_SerialPortToken && p_SerialPortToken->data)
    {
        strncpy(p_req->SerialPortToken, p_SerialPortToken->data, sizeof(p_req->SerialPortToken)-1);
    }

    return ONVIF_OK;
}

ONVIF_RET parse_SetSerialPortConfiguration(XMLN * p_node, SetSerialPortConfiguration_REQ * p_req)
{
    XMLN * p_SerialPortConfiguration;
    XMLN * p_ForcePersistance;

    p_SerialPortConfiguration = xml_node_soap_get(p_node, "SerialPortConfiguration");
    if (p_SerialPortConfiguration)
    {
        XMLN * p_BaudRate;
        XMLN * p_ParityBit;
        XMLN * p_CharacterLength;
        XMLN * p_StopBit;
        const char * p_token;
        const char * p_type;

        p_token = xml_attr_get(p_SerialPortConfiguration, "token");
        if (p_token)
        {
            strncpy(p_req->SerialPortConfiguration.token, p_token, sizeof(p_req->SerialPortConfiguration.token)-1);
        }

        p_type = xml_attr_get(p_SerialPortConfiguration, "type");
        if (p_type)
        {
            p_req->SerialPortConfiguration.type = onvif_StringToSerialPortType(p_type);
        }

        p_BaudRate = xml_node_soap_get(p_SerialPortConfiguration, "BaudRate");
        if (p_BaudRate && p_BaudRate->data)
        {
            p_req->SerialPortConfiguration.BaudRate = atoi(p_BaudRate->data);
        }

        p_ParityBit = xml_node_soap_get(p_SerialPortConfiguration, "ParityBit");
        if (p_ParityBit && p_ParityBit->data)
        {
            p_req->SerialPortConfiguration.ParityBit = onvif_StringToParityBit(p_ParityBit->data);
        }

        p_CharacterLength = xml_node_soap_get(p_SerialPortConfiguration, "CharacterLength");
        if (p_CharacterLength && p_CharacterLength->data)
        {
            p_req->SerialPortConfiguration.CharacterLength = atoi(p_CharacterLength->data);
        }

        p_StopBit = xml_node_soap_get(p_SerialPortConfiguration, "StopBit");
        if (p_StopBit && p_StopBit->data)
        {
            p_req->SerialPortConfiguration.StopBit = (float)atof(p_StopBit->data);
        }
    }

    p_ForcePersistance = xml_node_soap_get(p_node, "ForcePersistance");
    if (p_ForcePersistance && p_ForcePersistance->data)
    {
        p_req->ForcePersistance = parse_Bool(p_ForcePersistance->data);
    }
    
    return ONVIF_OK;
}

ONVIF_RET parse_SendReceiveSerialCommand(XMLN * p_node, SendReceiveSerialCommand_REQ * p_req)
{
    XMLN * p_token;
    XMLN * p_SerialData;
    XMLN * p_TimeOut;
    XMLN * p_DataLength;
    XMLN * p_Delimiter;

    p_token = xml_node_soap_get(p_node, "token");
    if (p_token && p_token->data)
    {
        strncpy(p_req->token, p_token->data, sizeof(p_req->token)-1);
    }
    
    p_SerialData = xml_node_soap_get(p_node, "SerialData");
    if (p_SerialData)
    {
        XMLN * p_Binary;
        XMLN * p_String;

        p_Binary = xml_node_soap_get(p_SerialData, "Binary");
        if (p_Binary && p_Binary->data)
        {
            p_req->Command.SerialData._union_SerialData = 0;

            onvif_malloc_SerialData(&p_req->Command.SerialData, 0, strlen(p_Binary->data)+1);
            strcpy(p_req->Command.SerialData.union_SerialData.Binary, p_Binary->data);            
        }

        p_String = xml_node_soap_get(p_SerialData, "String");
        if (p_String && p_String->data)
        {
            p_req->Command.SerialData._union_SerialData = 1;

            onvif_malloc_SerialData(&p_req->Command.SerialData, 1, strlen(p_String->data)+1);
            strcpy(p_req->Command.SerialData.union_SerialData.String, p_String->data);                                    
        }
        
        p_req->Command.SerialDataFlag = 1;
    }

    p_TimeOut = xml_node_soap_get(p_node, "TimeOut");
    if (p_TimeOut && p_TimeOut->data)
    {
        p_req->Command.TimeOutFlag = 1;
        parse_XSDDuration(p_TimeOut->data, (int *)&p_req->Command.TimeOut);
    }

    p_DataLength = xml_node_soap_get(p_node, "DataLength");
    if (p_DataLength && p_DataLength->data)
    {
        p_req->Command.DataLengthFlag = 1;
        p_req->Command.DataLength = atoi(p_DataLength->data);
    }

    p_Delimiter = xml_node_soap_get(p_node, "Delimiter");
    if (p_Delimiter && p_Delimiter->data)
    {
        p_req->Command.DelimiterFlag = 1;
        strncpy(p_req->Command.Delimiter, p_Delimiter->data, sizeof(p_req->Command.Delimiter)-1);
    }
    
    return ONVIF_OK;
}

ONVIF_RET parse_GetCompatibleAudioOutputConfigurations(XMLN * p_node, GetCompatibleAudioOutputConfigurations_REQ * p_req)
{
    XMLN * p_ProfileToken;

    p_ProfileToken = xml_node_soap_get(p_node, "ProfileToken");
    if (p_ProfileToken && p_ProfileToken->data)
    {
        strncpy(p_req->ProfileToken, p_ProfileToken->data, sizeof(p_req->ProfileToken)-1);
    }
    
    return ONVIF_OK;
}

ONVIF_RET parse_AddAudioOutputConfiguration(XMLN * p_node, AddAudioOutputConfiguration_REQ * p_req)
{
    XMLN * p_ProfileToken;
    XMLN * p_ConfigurationToken;

    p_ProfileToken = xml_node_soap_get(p_node, "ProfileToken");
    if (p_ProfileToken && p_ProfileToken->data)
    {
        strncpy(p_req->ProfileToken, p_ProfileToken->data, sizeof(p_req->ProfileToken)-1);
    }

    p_ConfigurationToken = xml_node_soap_get(p_node, "ConfigurationToken");
    if (p_ConfigurationToken && p_ConfigurationToken->data)
    {
        strncpy(p_req->ConfigurationToken, p_ConfigurationToken->data, sizeof(p_req->ConfigurationToken)-1);
    }
    
    return ONVIF_OK;
}

ONVIF_RET parse_RemoveAudioOutputConfiguration(XMLN * p_node, RemoveAudioOutputConfiguration_REQ * p_req)
{
    XMLN * p_ProfileToken;

    p_ProfileToken = xml_node_soap_get(p_node, "ProfileToken");
    if (p_ProfileToken && p_ProfileToken->data)
    {
        strncpy(p_req->ProfileToken, p_ProfileToken->data, sizeof(p_req->ProfileToken)-1);
    }
    
    return ONVIF_OK;
}

#endif // end of DEVICEIO_SUPPORT

#ifdef MEDIA2_SUPPORT

ONVIF_RET parse_ConfigurationRef(XMLN * p_node, onvif_ConfigurationRef * p_req)
{
    XMLN * p_Type;
    XMLN * p_Token;

    p_Type = xml_node_soap_get(p_node, "Type");
    if (p_Type && p_Type->data)
    {
        strncpy(p_req->Type, p_Type->data, sizeof(p_req->Type)-1);
    }

    p_Token = xml_node_soap_get(p_node, "Token");
    if (p_Token && p_Token->data)
    {
        p_req->TokenFlag = 1;
        strncpy(p_req->Token, p_Token->data, sizeof(p_req->Token)-1);
    }

    return ONVIF_OK;
}

BOOL parse_Polygon(XMLN * p_node, onvif_Polygon * p_req)
{
    XMLN * p_Point;

    p_Point = xml_node_soap_get(p_node, "Point");
    while (p_Point && soap_strcmp(p_Point->name, "Point") == 0)
    {
        int idx = p_req->sizePoint;
        
        parse_Vector(p_Point, &p_req->Point[idx]);

        p_req->sizePoint++;
        if (p_req->sizePoint >= ARRAY_SIZE(p_req->Point))
        {
            break;
        }
        
        p_Point = p_Point->next;
    }

    return TRUE;
}

BOOL parse_Color(XMLN * p_node, onvif_Color * p_req)
{
    const char * p_X;
    const char * p_Y;
    const char * p_Z;
    const char * p_Colorspace;

    p_X = xml_attr_get(p_node, "X");
    if (p_X)
    {
        p_req->X = (float)atof(p_X);
    }

    p_Y = xml_attr_get(p_node, "Y");
    if (p_Y)
    {
        p_req->Y = (float)atof(p_Y);
    }

    p_Z = xml_attr_get(p_node, "Z");
    if (p_Z)
    {
        p_req->Z = (float)atof(p_Z);
    }

    p_Colorspace = xml_attr_get(p_node, "Colorspace");
    if (p_Colorspace)
    {
        p_req->ColorspaceFlag = 1;
        strncpy(p_req->Colorspace, p_Colorspace, sizeof(p_req->Colorspace)-1);
    }

    return TRUE;
}

ONVIF_RET parse_Mask(XMLN * p_node, onvif_Mask * p_req)
{
    XMLN * p_ConfigurationToken;
    XMLN * p_Polygon;
    XMLN * p_Type;
    XMLN * p_Color;
    XMLN * p_Enabled;
    const char * p_token;

    p_token = xml_attr_get(p_node, "token");
    if (p_token)
    {
        strncpy(p_req->token, p_token, sizeof(p_req->token)-1);
    }

    p_ConfigurationToken = xml_node_soap_get(p_node, "ConfigurationToken");
    if (p_ConfigurationToken && p_ConfigurationToken->data)
    {
        strncpy(p_req->ConfigurationToken, p_ConfigurationToken->data, sizeof(p_req->ConfigurationToken)-1);
    }

    p_Polygon = xml_node_soap_get(p_node, "Polygon");
    if (p_Polygon)
    {
        parse_Polygon(p_Polygon, &p_req->Polygon);
    }

    p_Type = xml_node_soap_get(p_node, "Type");
    if (p_Type && p_Type->data)
    {
        strncpy(p_req->Type, p_Type->data, sizeof(p_req->Type)-1);
    }

    p_Color = xml_node_soap_get(p_node, "Color");
    if (p_Color)
    {
        p_req->ColorFlag = parse_Color(p_Color, &p_req->Color);
    }

    p_Enabled = xml_node_soap_get(p_node, "Enabled");
    if (p_Enabled && p_Enabled->data)
    {
        p_req->Enabled = parse_Bool(p_Enabled->data);
    }

    return ONVIF_OK;
}

ONVIF_RET parse_tr2_GetConfiguration(XMLN * p_node, tr2_GetConfiguration * p_req)
{
    XMLN * p_ConfigurationToken;
    XMLN * p_ProfileToken;

    p_ConfigurationToken = xml_node_soap_get(p_node, "ConfigurationToken");
    if (p_ConfigurationToken && p_ConfigurationToken->data)
    {
        p_req->ConfigurationTokenFlag = 1;
        strncpy(p_req->ConfigurationToken, p_ConfigurationToken->data, sizeof(p_req->ConfigurationToken)-1);
    }
    
    p_ProfileToken = xml_node_soap_get(p_node, "ProfileToken");
    if (p_ProfileToken && p_ProfileToken->data)
    {
        p_req->ProfileTokenFlag = 1;
        strncpy(p_req->ProfileToken, p_ProfileToken->data, sizeof(p_req->ProfileToken)-1);
    }
    
    return ONVIF_OK;
}

ONVIF_RET parse_tr2_SetVideoEncoderConfiguration(XMLN * p_node, tr2_SetVideoEncoderConfiguration_REQ * p_req)
{
    XMLN * p_Configuration;

    p_Configuration = xml_node_soap_get(p_node, "Configuration");
    if (p_Configuration)
    {
        XMLN * p_Name;
    	XMLN * p_UseCount;
    	XMLN * p_Encoding;
    	XMLN * p_Resolution;
    	XMLN * p_Quality;
    	XMLN * p_RateControl;
    	const char * p_token;
        const char * p_GovLength;
        const char * p_Profile;

        p_token = xml_attr_get(p_Configuration, "token");
        if (p_token)
        {
            strncpy(p_req->Configuration.token, p_token, sizeof(p_req->Configuration.token)-1);
        }

        p_GovLength = xml_attr_get(p_Configuration, "GovLength");
        if (p_GovLength)
        {
            p_req->Configuration.GovLengthFlag = 1;
            p_req->Configuration.GovLength = atoi(p_GovLength);
        }

        p_Profile = xml_attr_get(p_Configuration, "Profile");
        if (p_Profile)
        {
            p_req->Configuration.ProfileFlag = 1;
            strncpy(p_req->Configuration.Profile, p_Profile, sizeof(p_req->Configuration.Profile)-1);
        }
        
        p_Name = xml_node_soap_get(p_Configuration, "Name");
        if (p_Name && p_Name->data)
        {
            strncpy(p_req->Configuration.Name, p_Name->data, sizeof(p_req->Configuration.Name)-1);
        }

        p_UseCount = xml_node_soap_get(p_Configuration, "UseCount");
        if (p_UseCount && p_UseCount->data)
        {
            p_req->Configuration.UseCount = atoi(p_UseCount->data);
        }

        p_Encoding = xml_node_soap_get(p_Configuration, "Encoding");
        if (p_Encoding && p_Encoding->data)
        {
    		strncpy(p_req->Configuration.Encoding, p_Encoding->data, sizeof(p_req->Configuration.Encoding)-1);
        }

        p_Resolution = xml_node_soap_get(p_Configuration, "Resolution");
        if (p_Resolution)
        {
            XMLN * p_Width;
    		XMLN * p_Height;

    		p_Width = xml_node_soap_get(p_Resolution, "Width");
    	    if (p_Width && p_Width->data)
    	    {
    	        p_req->Configuration.Resolution.Width = atoi(p_Width->data);
    	    }

    	    p_Height = xml_node_soap_get(p_Resolution, "Height");
    	    if (p_Height && p_Height->data)
    	    {
    	        p_req->Configuration.Resolution.Height = atoi(p_Height->data);
    	    }
        }

        p_RateControl = xml_node_soap_get(p_Configuration, "RateControl");
        if (p_RateControl)
        {
        	XMLN * p_FrameRateLimit;
    		XMLN * p_BitrateLimit;
    		
    		p_req->Configuration.RateControlFlag = 1;
        	
            p_FrameRateLimit = xml_node_soap_get(p_RateControl, "FrameRateLimit");
    	    if (p_FrameRateLimit && p_FrameRateLimit->data)
    	    {
    	        p_req->Configuration.RateControl.FrameRateLimit = (float)atof(p_FrameRateLimit->data);
    	    }

    	    p_BitrateLimit = xml_node_soap_get(p_RateControl, "BitrateLimit");
    	    if (p_BitrateLimit && p_BitrateLimit->data)
    	    {
    	        p_req->Configuration.RateControl.BitrateLimit = atoi(p_BitrateLimit->data);
    	    }
        }

    	if (ONVIF_OK == parse_MulticastConfiguration(p_Configuration, &p_req->Configuration.Multicast))
    	{
    	    p_req->Configuration.MulticastFlag = 1;
    	}
    	
    	p_Quality = xml_node_soap_get(p_Configuration, "Quality");
        if (p_Quality && p_Quality->data)
        {
            p_req->Configuration.Quality = (float)atof(p_Quality->data);
        }
    }

    return ONVIF_OK;
}

ONVIF_RET parse_tr2_CreateProfile(XMLN * p_node, tr2_CreateProfile_REQ * p_req)
{
    XMLN * p_Name;
    XMLN * p_Configuration;

    p_Name = xml_node_soap_get(p_node, "Name");
    if (p_Name && p_Name->data)
    {
        strncpy(p_req->Name, p_Name->data, sizeof(p_req->Name)-1);
    }

    p_Configuration = xml_node_soap_get(p_node, "Configuration");
    while (p_Configuration && soap_strcmp(p_Configuration->name, "Configuration") == 0)
    {
        int idx = p_req->sizeConfiguration;

        parse_ConfigurationRef(p_Configuration, &p_req->Configuration[idx]);

        p_req->sizeConfiguration++;
        if (p_req->sizeConfiguration >= ARRAY_SIZE(p_req->Configuration))
        {
            break;
        }

        p_Configuration = p_Configuration->next;
    }

    return ONVIF_OK;
}

ONVIF_RET parse_tr2_GetProfiles(XMLN * p_node, tr2_GetProfiles_REQ * p_req)
{
    XMLN * p_Token;
    XMLN * p_Type;

    p_Token = xml_node_soap_get(p_node, "Token");
    if (p_Token && p_Token->data)
    {
        p_req->TokenFlag = 1;
        strncpy(p_req->Token, p_Token->data, sizeof(p_req->Token)-1);
    }

    p_Type = xml_node_soap_get(p_node, "Type");
    while (p_Type && p_Type->data && soap_strcmp(p_Type->name, "Type") == 0)
    {
        int idx = p_req->sizeType;
        
        strncpy(p_req->Type[idx], p_Type->data, sizeof(p_req->Type[idx])-1);        

        p_req->sizeType++;
        if (p_req->sizeType >= ARRAY_SIZE(p_req->Type))
        {
            break;
        }

        p_Type = p_Type->next;
    }

    return ONVIF_OK;
}

ONVIF_RET parse_tr2_DeleteProfile(XMLN * p_node, tr2_DeleteProfile_REQ * p_req)
{
    XMLN * p_Token;

    p_Token = xml_node_soap_get(p_node, "Token");
    if (p_Token && p_Token->data)
    {
        strncpy(p_req->Token, p_Token->data, sizeof(p_req->Token)-1);
    }

    return ONVIF_OK;
}

ONVIF_RET parse_tr2_AddConfiguration(XMLN * p_node, tr2_AddConfiguration_REQ * p_req)
{
    XMLN * p_ProfileToken;
    XMLN * p_Name;
    XMLN * p_Configuration;

    p_ProfileToken = xml_node_soap_get(p_node, "ProfileToken");
    if (p_ProfileToken && p_ProfileToken->data)
    {
        strncpy(p_req->ProfileToken, p_ProfileToken->data, sizeof(p_req->ProfileToken)-1);
    }

    p_Name = xml_node_soap_get(p_node, "Name");
    if (p_Name && p_Name->data)
    {
        p_req->NameFlag = 1;
        strncpy(p_req->Name, p_Name->data, sizeof(p_req->Name)-1);
    }

    p_Configuration = xml_node_soap_get(p_node, "Configuration");
    while (p_Configuration && soap_strcmp(p_Configuration->name, "Configuration") == 0)
    {
        int idx = p_req->sizeConfiguration;

        parse_ConfigurationRef(p_Configuration, &p_req->Configuration[idx]);

        p_req->sizeConfiguration++;
        if (p_req->sizeConfiguration >= ARRAY_SIZE(p_req->Configuration))
        {
            break;
        }

        p_Configuration = p_Configuration->next;
    }
    
    return ONVIF_OK;
}

ONVIF_RET parse_tr2_RemoveConfiguration(XMLN * p_node, tr2_RemoveConfiguration_REQ * p_req)
{
    XMLN * p_ProfileToken;
    XMLN * p_Configuration;

    p_ProfileToken = xml_node_soap_get(p_node, "ProfileToken");
    if (p_ProfileToken && p_ProfileToken->data)
    {
        strncpy(p_req->ProfileToken, p_ProfileToken->data, sizeof(p_req->ProfileToken)-1);
    }

    p_Configuration = xml_node_soap_get(p_node, "Configuration");
    while (p_Configuration && soap_strcmp(p_Configuration->name, "Configuration") == 0)
    {
        int idx = p_req->sizeConfiguration;

        parse_ConfigurationRef(p_Configuration, &p_req->Configuration[idx]);

        p_req->sizeConfiguration++;
        if (p_req->sizeConfiguration >= ARRAY_SIZE(p_req->Configuration))
        {
            break;
        }

        p_Configuration = p_Configuration->next;
    }
    
    return ONVIF_OK;
}

ONVIF_RET parse_tr2_SetVideoSourceConfiguration(XMLN * p_node, tr2_SetVideoSourceConfiguration_REQ * p_req)
{
    return parse_VideoSourceConfiguration(p_node, &p_req->Configuration);
}

ONVIF_RET parse_tr2_SetAudioEncoderConfiguration(XMLN * p_node, tr2_SetAudioEncoderConfiguration_REQ * p_req)
{
    XMLN * p_Configuration;

    p_Configuration = xml_node_soap_get(p_node, "Configuration");
    if (p_Configuration)
    {
        XMLN * p_Name;
    	XMLN * p_UseCount;
    	XMLN * p_Encoding;
    	XMLN * p_Bitrate;
    	XMLN * p_SampleRate;
    	const char * p_token;

        p_token = xml_attr_get(p_Configuration, "token");
        if (p_token)
        {
            strncpy(p_req->Configuration.token, p_token, sizeof(p_req->Configuration.token)-1);
        }
        
        p_Name = xml_node_soap_get(p_Configuration, "Name");
        if (p_Name && p_Name->data)
        {
            strncpy(p_req->Configuration.Name, p_Name->data, sizeof(p_req->Configuration.Name)-1);
        }

        p_UseCount = xml_node_soap_get(p_Configuration, "UseCount");
        if (p_UseCount && p_UseCount->data)
        {
            p_req->Configuration.UseCount = atoi(p_UseCount->data);
        }

        p_Encoding = xml_node_soap_get(p_Configuration, "Encoding");
        if (p_Encoding && p_Encoding->data)
        {
    		strncpy(p_req->Configuration.Encoding, p_Encoding->data, sizeof(p_req->Configuration.Encoding)-1);
        }

    	if (ONVIF_OK == parse_MulticastConfiguration(p_Configuration, &p_req->Configuration.Multicast))
    	{
    	    p_req->Configuration.MulticastFlag = 1;
    	}
    	
    	p_Bitrate = xml_node_soap_get(p_Configuration, "Bitrate");
        if (p_Bitrate && p_Bitrate->data)
        {
            p_req->Configuration.Bitrate = atoi(p_Bitrate->data);
        }

        p_SampleRate = xml_node_soap_get(p_Configuration, "SampleRate");
        if (p_SampleRate && p_SampleRate->data)
        {
            p_req->Configuration.SampleRate = atoi(p_SampleRate->data);
        }
    }

    return ONVIF_OK;
}

ONVIF_RET parse_tr2_SetMetadataConfiguration(XMLN * p_node, tr2_SetMetadataConfiguration_REQ * p_req)
{
    return parse_MetadataConfiguration(p_node, &p_req->Configuration);
}

ONVIF_RET parse_tr2_SetAudioSourceConfiguration(XMLN * p_node, tr2_SetAudioSourceConfiguration_REQ * p_req)
{
    return parse_AudioSourceConfiguration(p_node, &p_req->Configuration);
}

ONVIF_RET parse_tr2_GetVideoEncoderInstances(XMLN * p_node, tr2_GetVideoEncoderInstances_REQ * p_req)
{
    XMLN * p_ConfigurationToken;

    p_ConfigurationToken = xml_node_soap_get(p_node, "ConfigurationToken");
    if (p_ConfigurationToken && p_ConfigurationToken->data)
    {
        strncpy(p_req->ConfigurationToken, p_ConfigurationToken->data, sizeof(p_req->ConfigurationToken)-1);
    }

    return ONVIF_OK;
}

ONVIF_RET parse_tr2_GetStreamUri(XMLN * p_node, tr2_GetStreamUri_REQ * p_req)
{
    XMLN * p_Protocol;
    XMLN * p_ProfileToken;

    p_Protocol = xml_node_soap_get(p_node, "Protocol");
    if (p_Protocol && p_Protocol->data)
    {
        strncpy(p_req->Protocol, p_Protocol->data, sizeof(p_req->Protocol)-1);
    }

    p_ProfileToken = xml_node_soap_get(p_node, "ProfileToken");
    if (p_ProfileToken && p_ProfileToken->data)
    {
        strncpy(p_req->ProfileToken, p_ProfileToken->data, sizeof(p_req->ProfileToken)-1);
    }
    
    return ONVIF_OK;
}

ONVIF_RET parse_tr2_SetSynchronizationPoint(XMLN * p_node, tr2_SetSynchronizationPoint_REQ * p_req)
{
    XMLN * p_ProfileToken;

    p_ProfileToken = xml_node_soap_get(p_node, "ProfileToken");
    if (p_ProfileToken && p_ProfileToken->data)
    {
        strncpy(p_req->ProfileToken, p_ProfileToken->data, sizeof(p_req->ProfileToken)-1);
    }
    
    return ONVIF_OK;
}

ONVIF_RET parse_tr2_GetVideoSourceModes(XMLN * p_node, tr2_GetVideoSourceModes_REQ * p_req)
{
    XMLN * p_VideoSourceToken;

    p_VideoSourceToken = xml_node_soap_get(p_node, "VideoSourceToken");
	if (p_VideoSourceToken && p_VideoSourceToken->data)
	{
		strncpy(p_req->VideoSourceToken, p_VideoSourceToken->data, sizeof(p_req->VideoSourceToken)-1);
	}

	return ONVIF_OK;
}

ONVIF_RET parse_tr2_SetVideoSourceMode(XMLN * p_node, tr2_SetVideoSourceMode_REQ * p_req)
{
    XMLN * p_VideoSourceToken;
    XMLN * p_VideoSourceModeToken;

    p_VideoSourceToken = xml_node_soap_get(p_node, "VideoSourceToken");
	if (p_VideoSourceToken && p_VideoSourceToken->data)
	{
		strncpy(p_req->VideoSourceToken, p_VideoSourceToken->data, sizeof(p_req->VideoSourceToken)-1);
	}

	p_VideoSourceModeToken = xml_node_soap_get(p_node, "VideoSourceModeToken");
	if (p_VideoSourceModeToken && p_VideoSourceModeToken->data)
	{
		strncpy(p_req->VideoSourceModeToken, p_VideoSourceModeToken->data, sizeof(p_req->VideoSourceModeToken)-1);
	}

	return ONVIF_OK;
}

ONVIF_RET parse_tr2_GetSnapshotUri(XMLN * p_node, tr2_GetSnapshotUri_REQ * p_req)
{
    XMLN * p_ProfileToken;

    p_ProfileToken = xml_node_soap_get(p_node, "ProfileToken");
	if (p_ProfileToken && p_ProfileToken->data)
	{
	    strncpy(p_req->ProfileToken, p_ProfileToken->data, sizeof(p_req->ProfileToken)-1);
	}

	return ONVIF_OK;
}

ONVIF_RET parse_tr2_GetOSDs(XMLN * p_node, tr2_GetOSDs_REQ * p_req)
{
    XMLN * p_OSDToken;
    XMLN * p_ConfigurationToken;

    p_OSDToken = xml_node_soap_get(p_node, "OSDToken");
	if (p_OSDToken && p_OSDToken->data)
	{
	    p_req->OSDTokenFlag = 1;
	    strncpy(p_req->OSDToken, p_OSDToken->data, sizeof(p_req->OSDToken)-1);
	}

	p_ConfigurationToken = xml_node_soap_get(p_node, "ConfigurationToken");
	if (p_ConfigurationToken && p_ConfigurationToken->data)
	{
	    p_req->ConfigurationTokenFlag = 1;
	    strncpy(p_req->ConfigurationToken, p_ConfigurationToken->data, sizeof(p_req->ConfigurationToken)-1);
	}

	return ONVIF_OK;
}

ONVIF_RET parse_tr2_CreateMask(XMLN * p_node, tr2_CreateMask_REQ * p_req)
{
    XMLN * p_Mask;

    p_Mask = xml_node_soap_get(p_node, "Mask");
    if (p_Mask)
    {
        parse_Mask(p_Mask, &p_req->Mask);
    }

    return ONVIF_OK;    
}

ONVIF_RET parse_tr2_DeleteMask(XMLN * p_node, tr2_DeleteMask_REQ * p_req)
{
    XMLN * p_Token;
    
    p_Token = xml_node_soap_get(p_node, "Token");
	if (p_Token && p_Token->data)
	{
	    strncpy(p_req->Token, p_Token->data, sizeof(p_req->Token)-1);
	}

	return ONVIF_OK;
}

ONVIF_RET parse_tr2_GetMasks(XMLN * p_node, tr2_GetMasks_REQ * p_req)
{
    XMLN * p_Token;
    XMLN * p_ConfigurationToken;
    
    p_Token = xml_node_soap_get(p_node, "Token");
	if (p_Token && p_Token->data)
	{
	    p_req->TokenFlag = 1;
	    strncpy(p_req->Token, p_Token->data, sizeof(p_req->Token)-1);
	}

	p_ConfigurationToken = xml_node_soap_get(p_node, "ConfigurationToken");
	if (p_ConfigurationToken && p_ConfigurationToken->data)
	{
	    p_req->ConfigurationTokenFlag = 1;
	    strncpy(p_req->ConfigurationToken, p_ConfigurationToken->data, sizeof(p_req->ConfigurationToken)-1);
	}

	return ONVIF_OK;
}

ONVIF_RET parse_tr2_SetMask(XMLN * p_node, tr2_SetMask_REQ * p_req)
{
    XMLN * p_Mask;

    p_Mask = xml_node_soap_get(p_node, "Mask");
    if (p_Mask)
    {
        parse_Mask(p_Mask, &p_req->Mask);
    }

    return ONVIF_OK;
}


#ifdef DEVICEIO_SUPPORT

ONVIF_RET parse_tr2_GetAudioOutputConfigurations(XMLN * p_node, tr2_GetAudioOutputConfigurations_REQ * p_req)
{
    XMLN * p_ConfigurationToken;
    XMLN * p_ProfileToken;

    p_ConfigurationToken = xml_node_soap_get(p_node, "ConfigurationToken");
    if (p_ConfigurationToken && p_ConfigurationToken->data)
    {
        p_req->GetConfiguration.ConfigurationTokenFlag = 1;
        strncpy(p_req->GetConfiguration.ConfigurationToken, p_ConfigurationToken->data, sizeof(p_req->GetConfiguration.ConfigurationToken)-1);
    }

    p_ProfileToken = xml_node_soap_get(p_node, "ProfileToken");
    if (p_ProfileToken && p_ProfileToken->data)
    {
        p_req->GetConfiguration.ProfileTokenFlag = 1;
        strncpy(p_req->GetConfiguration.ProfileToken, p_ProfileToken->data, sizeof(p_req->GetConfiguration.ProfileToken)-1);
    }

    return ONVIF_OK;
}

ONVIF_RET parse_tr2_SetAudioOutputConfiguration(XMLN * p_node, tr2_SetAudioOutputConfiguration_REQ * p_req)
{
    XMLN * p_Configuration;

    p_Configuration = xml_node_soap_get(p_node, "Configuration");
    if (p_Configuration)
    {
        parse_AudioOutputConfiguration(p_Configuration, &p_req->Configuration);
    }

    return ONVIF_OK;
}

#endif // end of DEVICEIO_SUPPORT

#ifdef AUDIO_SUPPORT

ONVIF_RET parse_tr2_SetAudioDecoderConfiguration(XMLN * p_node, tr2_SetAudioDecoderConfiguration_REQ * p_req)
{
    XMLN * p_Configuration;

	p_Configuration = xml_node_soap_get(p_node, "Configuration");
	if (p_Configuration)
	{
	    parse_AudioDecoderConfiguration(p_Configuration, &p_req->Configuration);
	}

	return ONVIF_OK;
}

#endif // end of AUDIO_SUPPORT

#endif // end of MEDIA2_SUPPORT

#ifdef THERMAL_SUPPORT

ONVIF_RET parse_tth_GetConfiguration(XMLN * p_node, tth_GetConfiguration_REQ * p_req)
{
    XMLN * p_VideoSourceToken;

	p_VideoSourceToken = xml_node_soap_get(p_node, "VideoSourceToken");
	if (p_VideoSourceToken && p_VideoSourceToken->data)
	{
	    strncpy(p_req->VideoSourceToken, p_VideoSourceToken->data, sizeof(p_req->VideoSourceToken)-1);
	}

	return ONVIF_OK;
}

ONVIF_RET parse_ColorPalette(XMLN * p_node, onvif_ColorPalette * p_req)
{
    const char * p_token;
    const char * p_Type;
    XMLN * p_Name;

    p_token = xml_attr_get(p_node, "token");
    if (p_token)
    {
        strncpy(p_req->token, p_token, sizeof(p_req->token)-1);
    }

    p_Type = xml_attr_get(p_node, "Type");
    if (p_Type)
    {
        strncpy(p_req->Type, p_Type, sizeof(p_req->Type)-1);
    }

    p_Name = xml_node_soap_get(p_node, "Name");
	if (p_Name && p_Name->data)
	{
	    strncpy(p_req->Name, p_Name->data, sizeof(p_req->Name)-1);
	}

	return ONVIF_OK;
}

ONVIF_RET parse_NUCTable(XMLN * p_node, onvif_NUCTable * p_req)
{
    const char * p_token;
    const char * p_LowTemperature;
    const char * p_HighTemperature;
    XMLN * p_Name;

    p_token = xml_attr_get(p_node, "token");
    if (p_token)
    {
        strncpy(p_req->token, p_token, sizeof(p_req->token)-1);
    }

    p_LowTemperature = xml_attr_get(p_node, "LowTemperature");
    if (p_LowTemperature)
    {
        p_req->LowTemperatureFlag = 1;
        p_req->LowTemperature = (float) atof(p_LowTemperature);
    }

    p_HighTemperature = xml_attr_get(p_node, "HighTemperature");
    if (p_HighTemperature)
    {
        p_req->HighTemperatureFlag = 1;
        p_req->HighTemperature = (float) atof(p_HighTemperature);
    }

    p_Name = xml_node_soap_get(p_node, "Name");
	if (p_Name && p_Name->data)
	{
	    strncpy(p_req->Name, p_Name->data, sizeof(p_req->Name)-1);
	}

	return ONVIF_OK;
}

ONVIF_RET parse_Cooler(XMLN * p_node, onvif_Cooler * p_req)
{
    XMLN * p_Enabled;
    XMLN * p_RunTime;

    p_Enabled = xml_node_soap_get(p_node, "Enabled");
	if (p_Enabled && p_Enabled->data)
	{
	    p_req->Enabled = parse_Bool(p_Enabled->data);
	}

	p_RunTime = xml_node_soap_get(p_node, "RunTime");
	if (p_RunTime && p_RunTime->data)
	{
	    p_req->RunTime = (float) atof(p_RunTime->data);
	}

	return ONVIF_OK;
}

ONVIF_RET parse_ThermalConfiguration(XMLN * p_node, onvif_ThermalConfiguration * p_req)
{
    XMLN * p_ColorPalette;
    XMLN * p_Polarity;
    XMLN * p_NUCTable;
    XMLN * p_Cooler;

    p_ColorPalette = xml_node_soap_get(p_node, "ColorPalette");
	if (p_ColorPalette)
	{
	    parse_ColorPalette(p_ColorPalette, &p_req->ColorPalette);
	}

	p_Polarity = xml_node_soap_get(p_node, "Polarity");
	if (p_Polarity && p_Polarity->data)
	{
	    p_req->Polarity = onvif_StringToPolarity(p_Polarity->data);
	}

	p_NUCTable = xml_node_soap_get(p_node, "NUCTable");
	if (p_NUCTable)
	{
	    p_req->NUCTableFlag = 1;
	    parse_NUCTable(p_NUCTable, &p_req->NUCTable);
	}

	p_Cooler = xml_node_soap_get(p_node, "Cooler");
	if (p_Cooler)
	{
	    p_req->CoolerFlag = 1;
	    parse_Cooler(p_Cooler, &p_req->Cooler);
	}

	return ONVIF_OK;
}

ONVIF_RET parse_tth_SetConfiguration(XMLN * p_node, tth_SetConfiguration_REQ * p_req)
{
    XMLN * p_VideoSourceToken;
    XMLN * p_Configuration;

	p_VideoSourceToken = xml_node_soap_get(p_node, "VideoSourceToken");
	if (p_VideoSourceToken && p_VideoSourceToken->data)
	{
	    strncpy(p_req->VideoSourceToken, p_VideoSourceToken->data, sizeof(p_req->VideoSourceToken)-1);
	}

	p_Configuration = xml_node_soap_get(p_node, "Configuration");
	if (p_Configuration)
	{
	    parse_ThermalConfiguration(p_Configuration, &p_req->Configuration);
	}

	return ONVIF_OK;
}

ONVIF_RET parse_tth_GetConfigurationOptions(XMLN * p_node, tth_GetConfigurationOptions_REQ * p_req)
{
    XMLN * p_VideoSourceToken;

	p_VideoSourceToken = xml_node_soap_get(p_node, "VideoSourceToken");
	if (p_VideoSourceToken && p_VideoSourceToken->data)
	{
	    strncpy(p_req->VideoSourceToken, p_VideoSourceToken->data, sizeof(p_req->VideoSourceToken)-1);
	}

	return ONVIF_OK;
}

ONVIF_RET parse_tth_GetRadiometryConfiguration(XMLN * p_node, tth_GetRadiometryConfiguration_REQ * p_req)
{
    XMLN * p_VideoSourceToken;

	p_VideoSourceToken = xml_node_soap_get(p_node, "VideoSourceToken");
	if (p_VideoSourceToken && p_VideoSourceToken->data)
	{
	    strncpy(p_req->VideoSourceToken, p_VideoSourceToken->data, sizeof(p_req->VideoSourceToken)-1);
	}

	return ONVIF_OK;
}

ONVIF_RET parse_RadiometryGlobalParameters(XMLN * p_node, onvif_RadiometryGlobalParameters * p_req)
{
    XMLN * p_ReflectedAmbientTemperature;
    XMLN * p_Emissivity;
    XMLN * p_DistanceToObject;
    XMLN * p_RelativeHumidity;
    XMLN * p_AtmosphericTemperature;
    XMLN * p_AtmosphericTransmittance;
    XMLN * p_ExtOpticsTemperature;
    XMLN * p_ExtOpticsTransmittance;

    p_ReflectedAmbientTemperature = xml_node_soap_get(p_node, "ReflectedAmbientTemperature");
	if (p_ReflectedAmbientTemperature && p_ReflectedAmbientTemperature->data)
	{
	    p_req->ReflectedAmbientTemperature = (float) atof(p_ReflectedAmbientTemperature->data);
	}

	p_Emissivity = xml_node_soap_get(p_node, "Emissivity");
	if (p_Emissivity && p_Emissivity->data)
	{
	    p_req->Emissivity = (float) atof(p_Emissivity->data);
	}

	p_DistanceToObject = xml_node_soap_get(p_node, "DistanceToObject");
	if (p_DistanceToObject && p_DistanceToObject->data)
	{
	    p_req->DistanceToObject = (float) atof(p_DistanceToObject->data);
	}

	p_RelativeHumidity = xml_node_soap_get(p_node, "RelativeHumidity");
	if (p_RelativeHumidity && p_RelativeHumidity->data)
	{
	    p_req->RelativeHumidityFlag = 1;
	    p_req->RelativeHumidity = (float) atof(p_RelativeHumidity->data);
	}

    p_AtmosphericTemperature = xml_node_soap_get(p_node, "AtmosphericTemperature");
	if (p_AtmosphericTemperature && p_AtmosphericTemperature->data)
	{
	    p_req->AtmosphericTemperatureFlag = 1;
	    p_req->AtmosphericTemperature = (float) atof(p_AtmosphericTemperature->data);
	}

	p_AtmosphericTransmittance = xml_node_soap_get(p_node, "AtmosphericTransmittance");
	if (p_AtmosphericTransmittance && p_AtmosphericTransmittance->data)
	{
	    p_req->AtmosphericTransmittanceFlag = 1;
	    p_req->AtmosphericTransmittance = (float) atof(p_AtmosphericTransmittance->data);
	}

    p_ExtOpticsTemperature = xml_node_soap_get(p_node, "ExtOpticsTemperature");
	if (p_ExtOpticsTemperature && p_ExtOpticsTemperature->data)
	{
	    p_req->ExtOpticsTemperatureFlag = 1;
	    p_req->ExtOpticsTemperature = (float) atof(p_ExtOpticsTemperature->data);
	}

	p_ExtOpticsTransmittance = xml_node_soap_get(p_node, "ExtOpticsTransmittance");
	if (p_ExtOpticsTransmittance && p_ExtOpticsTransmittance->data)
	{
	    p_req->ExtOpticsTransmittanceFlag = 1;
	    p_req->ExtOpticsTransmittance = (float) atof(p_ExtOpticsTransmittance->data);
	}

	return ONVIF_OK;
}

ONVIF_RET parse_RadiometryConfiguration(XMLN * p_node, onvif_RadiometryConfiguration * p_req)
{
    XMLN * p_RadiometryGlobalParameters;

    p_RadiometryGlobalParameters = xml_node_soap_get(p_node, "RadiometryGlobalParameters");
	if (p_RadiometryGlobalParameters)
	{
	    p_req->RadiometryGlobalParametersFlag = 1;

	    parse_RadiometryGlobalParameters(p_RadiometryGlobalParameters, &p_req->RadiometryGlobalParameters);
	}

	return ONVIF_OK;
}

ONVIF_RET parse_tth_SetRadiometryConfiguration(XMLN * p_node, tth_SetRadiometryConfiguration_REQ * p_req)
{
    XMLN * p_VideoSourceToken;
    XMLN * p_Configuration;

	p_VideoSourceToken = xml_node_soap_get(p_node, "VideoSourceToken");
	if (p_VideoSourceToken && p_VideoSourceToken->data)
	{
	    strncpy(p_req->VideoSourceToken, p_VideoSourceToken->data, sizeof(p_req->VideoSourceToken)-1);
	}

	p_Configuration = xml_node_soap_get(p_node, "Configuration");
	if (p_Configuration)
	{
	    parse_RadiometryConfiguration(p_Configuration, &p_req->Configuration);
	}

	return ONVIF_OK;
}

ONVIF_RET parse_tth_GetRadiometryConfigurationOptions(XMLN * p_node, tth_GetRadiometryConfigurationOptions_REQ * p_req)
{
    XMLN * p_VideoSourceToken;

	p_VideoSourceToken = xml_node_soap_get(p_node, "VideoSourceToken");
	if (p_VideoSourceToken && p_VideoSourceToken->data)
	{
	    strncpy(p_req->VideoSourceToken, p_VideoSourceToken->data, sizeof(p_req->VideoSourceToken)-1);
	}

	return ONVIF_OK;
}

#endif // end of THERMAL_SUPPORT

#ifdef CREDENTIAL_SUPPORT

ONVIF_RET parse_CredentialIdentifierType(XMLN * p_node, onvif_CredentialIdentifierType * p_req)
{
    XMLN * p_Name;
    XMLN * p_FormatType;

    p_Name = xml_node_soap_get(p_node, "Name");
    if (p_Name && p_Name->data)
    {
        strncpy(p_req->Name, p_Name->data, sizeof(p_req->Name)-1);
    }

    p_FormatType = xml_node_soap_get(p_node, "FormatType");
    if (p_FormatType && p_FormatType->data)
    {
        strncpy(p_req->FormatType, p_FormatType->data, sizeof(p_req->FormatType)-1);
    }

    return ONVIF_OK;
}

ONVIF_RET parse_CredentialIdentifier(XMLN * p_node, onvif_CredentialIdentifier * p_req)
{
    XMLN * p_Type;
    XMLN * p_ExemptedFromAuthentication;
    XMLN * p_Value;

    p_Type = xml_node_soap_get(p_node, "Type");
    if (p_Type)
    {
        parse_CredentialIdentifierType(p_Type, &p_req->Type);
    }

    p_ExemptedFromAuthentication = xml_node_soap_get(p_node, "ExemptedFromAuthentication");
    if (p_ExemptedFromAuthentication && p_ExemptedFromAuthentication->data)
    {
        p_req->ExemptedFromAuthentication = parse_Bool(p_ExemptedFromAuthentication->data);
    }

    p_Value = xml_node_soap_get(p_node, "Value");
    if (p_Value && p_Value->data)
    {
        strncpy(p_req->Value, p_Value->data, sizeof(p_req->Value)-1);
    }

    return ONVIF_OK;
}

ONVIF_RET parse_CredentialAccessProfile(XMLN * p_node, onvif_CredentialAccessProfile * p_req)
{
    XMLN * p_AccessProfileToken;
    XMLN * p_ValidFrom;
    XMLN * p_ValidTo;

    p_AccessProfileToken = xml_node_soap_get(p_node, "AccessProfileToken");
    if (p_AccessProfileToken && p_AccessProfileToken->data)
    {
        strncpy(p_req->AccessProfileToken, p_AccessProfileToken->data, sizeof(p_req->AccessProfileToken)-1);
    }

    p_ValidFrom = xml_node_soap_get(p_node, "ValidFrom");
    if (p_ValidFrom && p_ValidFrom->data)
    {
        p_req->ValidFromFlag = 1;
        strncpy(p_req->ValidFrom, p_ValidFrom->data, sizeof(p_req->ValidFrom)-1);
    }

    p_ValidTo = xml_node_soap_get(p_node, "ValidTo");
    if (p_ValidTo && p_ValidTo->data)
    {
        p_req->ValidToFlag = 1;
        strncpy(p_req->ValidTo, p_ValidTo->data, sizeof(p_req->ValidFrom)-1);
    }

    return ONVIF_OK;
}

ONVIF_RET parse_Attribute(XMLN * p_node, onvif_Attribute * p_req)
{
    const char * p_Name;
    const char * p_Value;

    p_Name = xml_attr_get(p_node, "Name");
    if (p_Name)
    {
        strncpy(p_req->Name, p_Name, sizeof(p_req->Name)-1);
    }

    p_Value = xml_attr_get(p_node, "Value");
    if (p_Value)
    {
        p_req->ValueFlag = 1;
        strncpy(p_req->Value, p_Value, sizeof(p_req->Value)-1);
    }

    return ONVIF_OK;
}

ONVIF_RET parse_Credential(XMLN * p_node, onvif_Credential * p_req)
{
    XMLN * p_Description;
    XMLN * p_CredentialHolderReference;
    XMLN * p_ValidFrom;
    XMLN * p_ValidTo;
    XMLN * p_CredentialIdentifier;
    XMLN * p_CredentialAccessProfile;
    XMLN * p_Attribute;

    p_Description = xml_node_soap_get(p_node, "Description");
    if (p_Description && p_Description->data)
    {
        p_req->DescriptionFlag = 1;
        strncpy(p_req->Description, p_Description->data, sizeof(p_req->Description)-1);
    }

    p_CredentialHolderReference = xml_node_soap_get(p_node, "CredentialHolderReference");
    if (p_CredentialHolderReference && p_CredentialHolderReference->data)
    {
        strncpy(p_req->CredentialHolderReference, p_CredentialHolderReference->data, sizeof(p_req->CredentialHolderReference)-1);
    }

    p_ValidFrom = xml_node_soap_get(p_node, "ValidFrom");
    if (p_ValidFrom && p_ValidFrom->data)
    {
        p_req->ValidFromFlag = 1;
        strncpy(p_req->ValidFrom, p_ValidFrom->data, sizeof(p_req->ValidFrom)-1);
    }

    p_ValidTo = xml_node_soap_get(p_node, "ValidTo");
    if (p_ValidTo && p_ValidTo->data)
    {
        p_req->ValidToFlag = 1;
        strncpy(p_req->ValidTo, p_ValidTo->data, sizeof(p_req->ValidTo)-1);
    }

    p_CredentialIdentifier = xml_node_soap_get(p_node, "CredentialIdentifier");
    while (p_CredentialIdentifier && soap_strcmp(p_CredentialIdentifier->name, "CredentialIdentifier") == 0)
    {
        int idx = p_req->sizeCredentialIdentifier;

        parse_CredentialIdentifier(p_CredentialIdentifier, &p_req->CredentialIdentifier[idx]);

        p_req->sizeCredentialIdentifier++;
        if (p_req->sizeCredentialIdentifier >= ARRAY_SIZE(p_req->CredentialIdentifier))
        {
            break;
        }

        p_CredentialIdentifier = p_CredentialIdentifier->next;
    }

    p_CredentialAccessProfile = xml_node_soap_get(p_node, "CredentialAccessProfile");
    while (p_CredentialAccessProfile && soap_strcmp(p_CredentialAccessProfile->name, "CredentialAccessProfile") == 0)
    {
        int idx = p_req->sizeCredentialAccessProfile;

        parse_CredentialAccessProfile(p_CredentialAccessProfile, &p_req->CredentialAccessProfile[idx]);

        p_req->sizeCredentialAccessProfile++;
        if (p_req->sizeCredentialAccessProfile >= ARRAY_SIZE(p_req->CredentialAccessProfile))
        {
            break;
        }

        p_CredentialAccessProfile = p_CredentialAccessProfile->next;
    }

    p_Attribute = xml_node_soap_get(p_node, "Attribute");
    while (p_Attribute && soap_strcmp(p_Attribute->name, "Attribute") == 0)
    {
        int idx = p_req->sizeAttribute;

        parse_Attribute(p_Attribute, &p_req->Attribute[idx]);

        p_req->sizeAttribute++;
        if (p_req->sizeAttribute >= ARRAY_SIZE(p_req->Attribute))
        {
            break;
        }

        p_Attribute = p_Attribute->next;
    }

    return ONVIF_OK;
}

ONVIF_RET parse_CredentialState(XMLN * p_node, onvif_CredentialState * p_req)
{
    XMLN * p_Enabled;
    XMLN * p_Reason;
    XMLN * p_AntipassbackState;

    p_Enabled = xml_node_soap_get(p_node, "Enabled");
    if (p_Enabled && p_Enabled->data)
    {
        p_req->Enabled = parse_Bool(p_Enabled->data);
    }

    p_Reason = xml_node_soap_get(p_node, "Reason");
    if (p_Reason && p_Reason->data)
    {
        p_req->ReasonFlag = 1;
        strncpy(p_req->Reason, p_Reason->data, sizeof(p_req->Reason)-1);
    }

    p_AntipassbackState = xml_node_soap_get(p_node, "AntipassbackState");
    if (p_AntipassbackState)
    {
        XMLN * p_AntipassbackViolated;
        
        p_req->AntipassbackStateFlag = 1;

        p_AntipassbackViolated = xml_node_soap_get(p_AntipassbackState, "AntipassbackViolated");
        if (p_AntipassbackViolated && p_AntipassbackViolated->data)
        {
            p_req->AntipassbackState.AntipassbackViolated = parse_Bool(p_AntipassbackViolated->data);
        }
    }

    return ONVIF_OK;
}



ONVIF_RET parse_tcr_GetCredentialInfo(XMLN * p_node, tcr_GetCredentialInfo_REQ * p_req)
{
    XMLN * p_Token;

    p_Token = xml_node_soap_get(p_node, "Token");
    while (p_Token && soap_strcmp(p_Token->name, "Token") == 0)
    {
        int idx = p_req->sizeToken;

        strncpy(p_req->Token[idx], p_Token->data, sizeof(p_req->Token[idx])-1);

        p_req->sizeToken++;
        if (p_req->sizeToken >= ARRAY_SIZE(p_req->Token))
        {
            break;
        }

        p_Token = p_Token->next;
    }

    return ONVIF_OK;
}

ONVIF_RET parse_tcr_GetCredentialInfoList(XMLN * p_node, tcr_GetCredentialInfoList_REQ * p_req)
{
    XMLN * p_Limit;
    XMLN * p_StartReference;

    p_Limit = xml_node_soap_get(p_node, "Limit");
    if (p_Limit && p_Limit->data)
    {
        p_req->LimitFlag = 1;
        p_req->Limit = atoi(p_Limit->data);
    }

    p_StartReference = xml_node_soap_get(p_node, "StartReference");
    if (p_StartReference && p_StartReference->data)
    {
        p_req->StartReferenceFlag = 1;
        strncpy(p_req->StartReference, p_StartReference->data, sizeof(p_req->StartReference)-1);
    }

    return ONVIF_OK;
}

ONVIF_RET parse_tcr_GetCredentials(XMLN * p_node, tcr_GetCredentials_REQ * p_req)
{
    XMLN * p_Token;

    p_Token = xml_node_soap_get(p_node, "Token");
    while (p_Token && soap_strcmp(p_Token->name, "Token") == 0)
    {
        int idx = p_req->sizeToken;

        strncpy(p_req->Token[idx], p_Token->data, sizeof(p_req->Token[idx])-1);

        p_req->sizeToken++;
        if (p_req->sizeToken >= ARRAY_SIZE(p_req->Token))
        {
            break;
        }

        p_Token = p_Token->next;
    }

    return ONVIF_OK;
}

ONVIF_RET parse_tcr_GetCredentialList(XMLN * p_node, tcr_GetCredentialList_REQ * p_req)
{
    XMLN * p_Limit;
    XMLN * p_StartReference;

    p_Limit = xml_node_soap_get(p_node, "Limit");
    if (p_Limit && p_Limit->data)
    {
        p_req->LimitFlag = 1;
        p_req->Limit = atoi(p_Limit->data);
    }

    p_StartReference = xml_node_soap_get(p_node, "StartReference");
    if (p_StartReference && p_StartReference->data)
    {
        p_req->StartReferenceFlag = 1;
        strncpy(p_req->StartReference, p_StartReference->data, sizeof(p_req->StartReference)-1);
    }

    return ONVIF_OK;
}

ONVIF_RET parse_tcr_CreateCredential(XMLN * p_node, tcr_CreateCredential_REQ * p_req)
{
    XMLN * p_Credential;
    XMLN * p_State;

    p_Credential = xml_node_soap_get(p_node, "Credential");
    if (p_Credential)
    {
        const char * p_token;

        p_token = xml_attr_get(p_Credential, "token");
        if (p_token)
        {
            strncpy(p_req->Credential.token, p_token, sizeof(p_req->Credential.token)-1);
        }

        parse_Credential(p_Credential, &p_req->Credential);
    }

    p_State = xml_node_soap_get(p_node, "State");
    if (p_State)
    {
        parse_CredentialState(p_State, &p_req->State);
    }

    return ONVIF_OK;
}

ONVIF_RET parse_tcr_ModifyCredential(XMLN * p_node, tcr_ModifyCredential_REQ * p_req)
{
    XMLN * p_Credential;

    p_Credential = xml_node_soap_get(p_node, "Credential");
    if (p_Credential)
    {
        const char * p_token;

        p_token = xml_attr_get(p_Credential, "token");
        if (p_token)
        {
            strncpy(p_req->Credential.token, p_token, sizeof(p_req->Credential.token)-1);
        }

        parse_Credential(p_Credential, &p_req->Credential);
    }

    return ONVIF_OK;
}

ONVIF_RET parse_tcr_DeleteCredential(XMLN * p_node, tcr_DeleteCredential_REQ * p_req)
{
    XMLN * p_Token;

    p_Token = xml_node_soap_get(p_node, "Token");
    if (p_Token && p_Token->data)
    {
        strncpy(p_req->Token, p_Token->data, sizeof(p_req->Token)-1);
    }

    return ONVIF_OK;
}

ONVIF_RET parse_tcr_GetCredentialState(XMLN * p_node, tcr_GetCredentialState_REQ * p_req)
{
    XMLN * p_Token;

    p_Token = xml_node_soap_get(p_node, "Token");
    if (p_Token && p_Token->data)
    {
        strncpy(p_req->Token, p_Token->data, sizeof(p_req->Token)-1);
    }

    return ONVIF_OK;
}

ONVIF_RET parse_tcr_EnableCredential(XMLN * p_node, tcr_EnableCredential_REQ * p_req)
{
    XMLN * p_Token;
    XMLN * p_Reason;

    p_Token = xml_node_soap_get(p_node, "Token");
    if (p_Token && p_Token->data)
    {
        strncpy(p_req->Token, p_Token->data, sizeof(p_req->Token)-1);
    }

    p_Reason = xml_node_soap_get(p_node, "Reason");
    if (p_Reason && p_Reason->data)
    {
        p_req->ReasonFlag = 1;
        strncpy(p_req->Reason, p_Reason->data, sizeof(p_req->Reason)-1);
    }

    return ONVIF_OK;
}

ONVIF_RET parse_tcr_DisableCredential(XMLN * p_node, tcr_DisableCredential_REQ * p_req)
{
    XMLN * p_Token;
    XMLN * p_Reason;

    p_Token = xml_node_soap_get(p_node, "Token");
    if (p_Token && p_Token->data)
    {
        strncpy(p_req->Token, p_Token->data, sizeof(p_req->Token)-1);
    }

    p_Reason = xml_node_soap_get(p_node, "Reason");
    if (p_Reason && p_Reason->data)
    {
        p_req->ReasonFlag = 1;
        strncpy(p_req->Reason, p_Reason->data, sizeof(p_req->Reason)-1);
    }

    return ONVIF_OK;
}

ONVIF_RET parse_tcr_ResetAntipassbackViolation(XMLN * p_node, tcr_ResetAntipassbackViolation_REQ * p_req)
{
    XMLN * p_CredentialToken;

    p_CredentialToken = xml_node_soap_get(p_node, "CredentialToken");
    if (p_CredentialToken && p_CredentialToken->data)
    {
        strncpy(p_req->CredentialToken, p_CredentialToken->data, sizeof(p_req->CredentialToken)-1);
    }

    return ONVIF_OK;
}

ONVIF_RET parse_tcr_GetSupportedFormatTypes(XMLN * p_node, tcr_GetSupportedFormatTypes_REQ * p_req)
{
    XMLN * p_CredentialIdentifierTypeName;

    p_CredentialIdentifierTypeName = xml_node_soap_get(p_node, "CredentialIdentifierTypeName");
    if (p_CredentialIdentifierTypeName && p_CredentialIdentifierTypeName->data)
    {
        strncpy(p_req->CredentialIdentifierTypeName, p_CredentialIdentifierTypeName->data, sizeof(p_req->CredentialIdentifierTypeName)-1);
    }

    return ONVIF_OK;
}

ONVIF_RET parse_tcr_GetCredentialIdentifiers(XMLN * p_node, tcr_GetCredentialIdentifiers_REQ * p_req)
{
    XMLN * p_CredentialToken;

    p_CredentialToken = xml_node_soap_get(p_node, "CredentialToken");
    if (p_CredentialToken && p_CredentialToken->data)
    {
        strncpy(p_req->CredentialToken, p_CredentialToken->data, sizeof(p_req->CredentialToken)-1);
    }

    return ONVIF_OK;
}

ONVIF_RET parse_tcr_SetCredentialIdentifier(XMLN * p_node, tcr_SetCredentialIdentifier_REQ * p_req)
{
    XMLN * p_CredentialToken;
    XMLN * p_CredentialIdentifier;

    p_CredentialToken = xml_node_soap_get(p_node, "CredentialToken");
    if (p_CredentialToken && p_CredentialToken->data)
    {
        strncpy(p_req->CredentialToken, p_CredentialToken->data, sizeof(p_req->CredentialToken)-1);
    }

    p_CredentialIdentifier = xml_node_soap_get(p_node, "CredentialIdentifier");
    if (p_CredentialIdentifier)
    {
        parse_CredentialIdentifier(p_CredentialIdentifier, &p_req->CredentialIdentifier);
    }
    
    return ONVIF_OK;
}

ONVIF_RET parse_tcr_DeleteCredentialIdentifier(XMLN * p_node, tcr_DeleteCredentialIdentifier_REQ * p_req)
{
    XMLN * p_CredentialToken;
    XMLN * p_CredentialIdentifierTypeName;

    p_CredentialToken = xml_node_soap_get(p_node, "CredentialToken");
    if (p_CredentialToken && p_CredentialToken->data)
    {
        strncpy(p_req->CredentialToken, p_CredentialToken->data, sizeof(p_req->CredentialToken)-1);
    }

    p_CredentialIdentifierTypeName = xml_node_soap_get(p_node, "CredentialIdentifierTypeName");
    if (p_CredentialIdentifierTypeName && p_CredentialIdentifierTypeName->data)
    {
        strncpy(p_req->CredentialIdentifierTypeName, p_CredentialIdentifierTypeName->data, sizeof(p_req->CredentialIdentifierTypeName)-1);
    }

    return ONVIF_OK;
}

ONVIF_RET parse_tcr_GetCredentialAccessProfiles(XMLN * p_node, tcr_GetCredentialAccessProfiles_REQ * p_req)
{
    XMLN * p_CredentialToken;

    p_CredentialToken = xml_node_soap_get(p_node, "CredentialToken");
    if (p_CredentialToken && p_CredentialToken->data)
    {
        strncpy(p_req->CredentialToken, p_CredentialToken->data, sizeof(p_req->CredentialToken)-1);
    }

    return ONVIF_OK;
}

ONVIF_RET parse_tcr_SetCredentialAccessProfiles(XMLN * p_node, tcr_SetCredentialAccessProfiles_REQ * p_req)
{
    XMLN * p_CredentialToken;
    XMLN * p_CredentialAccessProfile;

    p_CredentialToken = xml_node_soap_get(p_node, "CredentialToken");
    if (p_CredentialToken && p_CredentialToken->data)
    {
        strncpy(p_req->CredentialToken, p_CredentialToken->data, sizeof(p_req->CredentialToken)-1);
    }

    p_CredentialAccessProfile = xml_node_soap_get(p_node, "CredentialAccessProfile");
    while (p_CredentialAccessProfile && soap_strcmp(p_CredentialAccessProfile->name, "CredentialAccessProfile") == 0)
    {
        int idx = p_req->sizeCredentialAccessProfile;

        parse_CredentialAccessProfile(p_CredentialAccessProfile, &p_req->CredentialAccessProfile[idx]);

        p_req->sizeCredentialAccessProfile++;
        if (p_req->sizeCredentialAccessProfile >= ARRAY_SIZE(p_req->CredentialAccessProfile))
        {
            break;
        }

        p_CredentialAccessProfile = p_CredentialAccessProfile->next;
    }

    return ONVIF_OK;
}

ONVIF_RET parse_tcr_DeleteCredentialAccessProfiles(XMLN * p_node, tcr_DeleteCredentialAccessProfiles_REQ * p_req)
{
    XMLN * p_CredentialToken;
    XMLN * p_AccessProfileToken;

    p_CredentialToken = xml_node_soap_get(p_node, "CredentialToken");
    if (p_CredentialToken && p_CredentialToken->data)
    {
        strncpy(p_req->CredentialToken, p_CredentialToken->data, sizeof(p_req->CredentialToken)-1);
    }

    p_AccessProfileToken = xml_node_soap_get(p_node, "AccessProfileToken");
    while (p_AccessProfileToken && p_AccessProfileToken->data && soap_strcmp(p_AccessProfileToken->name, "AccessProfileToken") == 0)
    {
        int idx = p_req->sizeAccessProfileToken;

        strncpy(p_req->AccessProfileToken[idx], p_AccessProfileToken->data, sizeof(p_req->AccessProfileToken[idx])-1);

        p_req->sizeAccessProfileToken++;
        if (p_req->sizeAccessProfileToken >= ARRAY_SIZE(p_req->AccessProfileToken))
        {
            break;
        }

        p_AccessProfileToken = p_AccessProfileToken->next;
    }

    return ONVIF_OK;
}

#endif // end of CREDENTIAL_SUPPORT

#ifdef ACCESS_RULES

ONVIF_RET parse_AccessPolicy(XMLN * p_node, onvif_AccessPolicy * p_req)
{
    XMLN * p_ScheduleToken;
    XMLN * p_Entity;    
    XMLN * p_EntityType;

    p_ScheduleToken = xml_node_soap_get(p_node, "ScheduleToken");
    if (p_ScheduleToken && p_ScheduleToken->data)
    {
        strncpy(p_req->ScheduleToken, p_ScheduleToken->data, sizeof(p_req->ScheduleToken)-1);
    }

    p_Entity = xml_node_soap_get(p_node, "Entity");
    if (p_Entity && p_Entity->data)
    {
        strncpy(p_req->Entity, p_Entity->data, sizeof(p_req->Entity)-1);
    }

    p_EntityType = xml_node_soap_get(p_node, "EntityType");
    if (p_EntityType && p_EntityType->data)
    {
        p_req->EntityTypeFlag = 1;
        strncpy(p_req->EntityType, p_EntityType->data, sizeof(p_req->EntityType)-1);
    }
    
    return ONVIF_OK;
}

ONVIF_RET parse_AccessProfile(XMLN * p_node, onvif_AccessProfile * p_req)
{
    XMLN * p_Name;
    XMLN * p_Description;
    XMLN * p_AccessPolicy;

    p_Name = xml_node_soap_get(p_node, "Name");
    if (p_Name && p_Name->data)
    {
        strncpy(p_req->Name, p_Name->data, sizeof(p_req->Name)-1);
    }

    p_Description = xml_node_soap_get(p_node, "Description");
    if (p_Description && p_Description->data)
    {
        p_req->DescriptionFlag = 1;
        strncpy(p_req->Description, p_Description->data, sizeof(p_req->Description)-1);
    }

    p_AccessPolicy = xml_node_soap_get(p_node, "AccessPolicy");
    while (p_AccessPolicy && soap_strcmp(p_AccessPolicy->name, "AccessPolicy") == 0)
    {
        int idx = p_req->sizeAccessPolicy;

        parse_AccessPolicy(p_AccessPolicy, &p_req->AccessPolicy[idx]);

        p_req->sizeAccessPolicy++;
        if (p_req->sizeAccessPolicy >= ARRAY_SIZE(p_req->AccessPolicy))
        {
            break;
        }

        p_AccessPolicy = p_AccessPolicy->next;
    }

    return ONVIF_OK;
}

ONVIF_RET parse_tar_GetAccessProfileInfo(XMLN * p_node, tar_GetAccessProfileInfo_REQ * p_req)
{
    XMLN * p_Token;

    p_Token = xml_node_soap_get(p_node, "Token");
    while (p_Token && soap_strcmp(p_Token->name, "Token") == 0)
    {
        int idx = p_req->sizeToken;

        strncpy(p_req->Token[idx], p_Token->data, sizeof(p_req->Token[idx])-1);

        p_req->sizeToken++;
        if (p_req->sizeToken >= ARRAY_SIZE(p_req->Token))
        {
            break;
        }

        p_Token = p_Token->next;
    }

    return ONVIF_OK;
}

ONVIF_RET parse_tar_GetAccessProfileInfoList(XMLN * p_node, tar_GetAccessProfileInfoList_REQ * p_req)
{
    XMLN * p_Limit;
    XMLN * p_StartReference;

    p_Limit = xml_node_soap_get(p_node, "Limit");
    if (p_Limit && p_Limit->data)
    {
        p_req->LimitFlag = 1;
        p_req->Limit = atoi(p_Limit->data);
    }

    p_StartReference = xml_node_soap_get(p_node, "StartReference");
    if (p_StartReference && p_StartReference->data)
    {
        p_req->StartReferenceFlag = 1;
        strncpy(p_req->StartReference, p_StartReference->data, sizeof(p_req->StartReference)-1);
    }

    return ONVIF_OK;
}

ONVIF_RET parse_tar_GetAccessProfiles(XMLN * p_node, tar_GetAccessProfiles_REQ * p_req)
{
    XMLN * p_Token;

    p_Token = xml_node_soap_get(p_node, "Token");
    while (p_Token && soap_strcmp(p_Token->name, "Token") == 0)
    {
        int idx = p_req->sizeToken;

        strncpy(p_req->Token[idx], p_Token->data, sizeof(p_req->Token[idx])-1);

        p_req->sizeToken++;
        if (p_req->sizeToken >= ARRAY_SIZE(p_req->Token))
        {
            break;
        }

        p_Token = p_Token->next;
    }

    return ONVIF_OK;
}

ONVIF_RET parse_tar_GetAccessProfileList(XMLN * p_node, tar_GetAccessProfileList_REQ * p_req)
{
    XMLN * p_Limit;
    XMLN * p_StartReference;

    p_Limit = xml_node_soap_get(p_node, "Limit");
    if (p_Limit && p_Limit->data)
    {
        p_req->LimitFlag = 1;
        p_req->Limit = atoi(p_Limit->data);
    }

    p_StartReference = xml_node_soap_get(p_node, "StartReference");
    if (p_StartReference && p_StartReference->data)
    {
        p_req->StartReferenceFlag = 1;
        strncpy(p_req->StartReference, p_StartReference->data, sizeof(p_req->StartReference)-1);
    }

    return ONVIF_OK;
}

ONVIF_RET parse_tar_CreateAccessProfile(XMLN * p_node, tar_CreateAccessProfile_REQ * p_req)
{
    XMLN * p_AccessProfile;

    p_AccessProfile = xml_node_soap_get(p_node, "AccessProfile");
    if (p_AccessProfile)
    {
        const char * p_token;

        p_token = xml_attr_get(p_AccessProfile, "token");
        if (p_token)
        {
            strncpy(p_req->AccessProfile.token, p_token, sizeof(p_req->AccessProfile.token)-1);
        }

        parse_AccessProfile(p_AccessProfile, &p_req->AccessProfile);
    }

    return ONVIF_OK;
}

ONVIF_RET parse_tar_ModifyAccessProfile(XMLN * p_node, tar_ModifyAccessProfile_REQ * p_req)
{
    XMLN * p_AccessProfile;

    p_AccessProfile = xml_node_soap_get(p_node, "AccessProfile");
    if (p_AccessProfile)
    {
        const char * p_token;

        p_token = xml_attr_get(p_AccessProfile, "token");
        if (p_token)
        {
            strncpy(p_req->AccessProfile.token, p_token, sizeof(p_req->AccessProfile.token)-1);
        }

        parse_AccessProfile(p_AccessProfile, &p_req->AccessProfile);
    }

    return ONVIF_OK;
}

ONVIF_RET parse_tar_DeleteAccessProfile(XMLN * p_node, tar_DeleteAccessProfile_REQ * p_req)
{
    XMLN * p_Token;

    p_Token = xml_node_soap_get(p_node, "Token");
    if (p_Token && p_Token->data)
    {
        strncpy(p_req->Token, p_Token->data, sizeof(p_req->Token)-1);
    }

    return ONVIF_OK;
}

#endif // end of ACCESS_RULES

#ifdef SCHEDULE_SUPPORT

ONVIF_RET parse_TimePeriod(XMLN * p_node, onvif_TimePeriod * p_req)
{
    XMLN * p_From;
    XMLN * p_Until;

    p_From = xml_node_soap_get(p_node, "From");
    if (p_From && p_From->data)
    {
        strncpy(p_req->From, p_From->data, sizeof(p_req->From)-1);
    }

    p_Until = xml_node_soap_get(p_node, "Until");
    if (p_Until && p_Until->data)
    {
        p_req->UntilFlag = 1;
        strncpy(p_req->Until, p_Until->data, sizeof(p_req->Until)-1);
    }

    return ONVIF_OK;
}

ONVIF_RET parse_SpecialDaysSchedule(XMLN * p_node, onvif_SpecialDaysSchedule * p_req)
{
    XMLN * p_GroupToken;
    XMLN * p_TimeRange;

    p_GroupToken = xml_node_soap_get(p_node, "GroupToken");
    if (p_GroupToken && p_GroupToken->data)
    {
        strncpy(p_req->GroupToken, p_GroupToken->data, sizeof(p_req->GroupToken)-1);
    }

    p_TimeRange = xml_node_soap_get(p_node, "TimeRange");
    while (p_TimeRange && soap_strcmp(p_TimeRange->name, "TimeRange") == 0)
    {
        int idx = p_req->sizeTimeRange;

        parse_TimePeriod(p_TimeRange, &p_req->TimeRange[idx]);

        p_req->sizeTimeRange++;
        if (p_req->sizeTimeRange >= ARRAY_SIZE(p_req->TimeRange))
        {
            break;
        }
        
        p_TimeRange = p_TimeRange->next;
    }

    return ONVIF_OK;
}

ONVIF_RET parse_Schedule(XMLN * p_node, onvif_Schedule * p_req)
{
    XMLN * p_Name;
    XMLN * p_Description;
    XMLN * p_Standard;
    XMLN * p_SpecialDays;

    p_Name = xml_node_soap_get(p_node, "Name");
    if (p_Name && p_Name->data)
    {
        strncpy(p_req->Name, p_Name->data, sizeof(p_req->Name)-1);
    }

    p_Description = xml_node_soap_get(p_node, "Description");
    if (p_Description && p_Description->data)
    {
        p_req->DescriptionFlag = 1;
        strncpy(p_req->Description, p_Description->data, sizeof(p_req->Description)-1);
    }

    p_Standard = xml_node_soap_get(p_node, "Standard");
    if (p_Standard && p_Standard->data)
    {
        strncpy(p_req->Standard, p_Standard->data, sizeof(p_req->Standard)-1);
    }

    p_SpecialDays = xml_node_soap_get(p_node, "SpecialDays");
    while (p_SpecialDays && soap_strcmp(p_SpecialDays->name, "SpecialDays") == 0)
    {
        int idx = p_req->sizeSpecialDays;

        parse_SpecialDaysSchedule(p_SpecialDays, &p_req->SpecialDays[idx]);

        p_req->sizeSpecialDays++;
        if (p_req->sizeSpecialDays >= ARRAY_SIZE(p_req->SpecialDays))
        {
            break;
        }
        
        p_SpecialDays = p_SpecialDays->next;
    }

    return ONVIF_OK;
}

ONVIF_RET parse_SpecialDayGroup(XMLN * p_node, onvif_SpecialDayGroup * p_req)
{
    XMLN * p_Name;
    XMLN * p_Description;
    XMLN * p_Days;

    p_Name = xml_node_soap_get(p_node, "Name");
    if (p_Name && p_Name->data)
    {
        strncpy(p_req->Name, p_Name->data, sizeof(p_req->Name)-1);
    }

    p_Description = xml_node_soap_get(p_node, "Description");
    if (p_Description && p_Description->data)
    {
        p_req->DescriptionFlag = 1;
        strncpy(p_req->Description, p_Description->data, sizeof(p_req->Description)-1);
    }

    p_Days = xml_node_soap_get(p_node, "Days");
    if (p_Days && p_Days->data)
    {
        p_req->DaysFlag = 1;
        strncpy(p_req->Days, p_Days->data, sizeof(p_req->Days)-1);
    }

    return ONVIF_OK;
}

ONVIF_RET parse_tsc_GetScheduleInfo(XMLN * p_node, tsc_GetScheduleInfo_REQ * p_req)
{
    XMLN * p_Token;

    p_Token = xml_node_soap_get(p_node, "Token");
    while (p_Token && soap_strcmp(p_Token->name, "Token") == 0)
    {
        int idx = p_req->sizeToken;

        strncpy(p_req->Token[idx], p_Token->data, sizeof(p_req->Token[idx])-1);

        p_req->sizeToken++;
        if (p_req->sizeToken >= ARRAY_SIZE(p_req->Token))
        {
            break;
        }

        p_Token = p_Token->next;
    }

    return ONVIF_OK;
}

ONVIF_RET parse_tsc_GetScheduleInfoList(XMLN * p_node, tsc_GetScheduleInfoList_REQ * p_req)
{
    XMLN * p_Limit;
    XMLN * p_StartReference;

    p_Limit = xml_node_soap_get(p_node, "Limit");
    if (p_Limit && p_Limit->data)
    {
        p_req->LimitFlag = 1;
        p_req->Limit = atoi(p_Limit->data);
    }

    p_StartReference = xml_node_soap_get(p_node, "StartReference");
    if (p_StartReference && p_StartReference->data)
    {
        p_req->StartReferenceFlag = 1;
        strncpy(p_req->StartReference, p_StartReference->data, sizeof(p_req->StartReference)-1);
    }

    return ONVIF_OK;
}

ONVIF_RET parse_tsc_GetSchedules(XMLN * p_node, tsc_GetSchedules_REQ * p_req)
{
    XMLN * p_Token;

    p_Token = xml_node_soap_get(p_node, "Token");
    while (p_Token && soap_strcmp(p_Token->name, "Token") == 0)
    {
        int idx = p_req->sizeToken;

        strncpy(p_req->Token[idx], p_Token->data, sizeof(p_req->Token[idx])-1);

        p_req->sizeToken++;
        if (p_req->sizeToken >= ARRAY_SIZE(p_req->Token))
        {
            break;
        }

        p_Token = p_Token->next;
    }

    return ONVIF_OK;
}

ONVIF_RET parse_tsc_GetScheduleList(XMLN * p_node, tsc_GetScheduleList_REQ * p_req)
{
    XMLN * p_Limit;
    XMLN * p_StartReference;

    p_Limit = xml_node_soap_get(p_node, "Limit");
    if (p_Limit && p_Limit->data)
    {
        p_req->LimitFlag = 1;
        p_req->Limit = atoi(p_Limit->data);
    }

    p_StartReference = xml_node_soap_get(p_node, "StartReference");
    if (p_StartReference && p_StartReference->data)
    {
        p_req->StartReferenceFlag = 1;
        strncpy(p_req->StartReference, p_StartReference->data, sizeof(p_req->StartReference)-1);
    }

    return ONVIF_OK;
}

ONVIF_RET parse_tsc_CreateSchedule(XMLN * p_node, tsc_CreateSchedule_REQ * p_req)
{
    XMLN * p_Schedule;

    p_Schedule = xml_node_soap_get(p_node, "Schedule");
    if (p_Schedule)
    {
        const char * p_token;

        p_token = xml_attr_get(p_Schedule, "token");
        if (p_token)
        {
            strncpy(p_req->Schedule.token, p_token, sizeof(p_req->Schedule.token)-1);
        }

        parse_Schedule(p_Schedule, &p_req->Schedule);
    }

    return ONVIF_OK;
}

ONVIF_RET parse_tsc_ModifySchedule(XMLN * p_node, tsc_ModifySchedule_REQ * p_req)
{
    XMLN * p_Schedule;

    p_Schedule = xml_node_soap_get(p_node, "Schedule");
    if (p_Schedule)
    {
        const char * p_token;

        p_token = xml_attr_get(p_Schedule, "token");
        if (p_token)
        {
            strncpy(p_req->Schedule.token, p_token, sizeof(p_req->Schedule.token)-1);
        }

        parse_Schedule(p_Schedule, &p_req->Schedule);
    }

    return ONVIF_OK;
}

ONVIF_RET parse_tsc_DeleteSchedule(XMLN * p_node, tsc_DeleteSchedule_REQ * p_req)
{
    XMLN * p_Token;

    p_Token = xml_node_soap_get(p_node, "Token");
    if (p_Token && p_Token->data)
    {
        strncpy(p_req->Token, p_Token->data, sizeof(p_req->Token)-1);
    }

    return ONVIF_OK;
}

ONVIF_RET parse_tsc_GetSpecialDayGroupInfo(XMLN * p_node, tsc_GetSpecialDayGroupInfo_REQ * p_req)
{
    XMLN * p_Token;

    p_Token = xml_node_soap_get(p_node, "Token");
    while (p_Token && soap_strcmp(p_Token->name, "Token") == 0)
    {
        int idx = p_req->sizeToken;

        strncpy(p_req->Token[idx], p_Token->data, sizeof(p_req->Token[idx])-1);

        p_req->sizeToken++;
        if (p_req->sizeToken >= ARRAY_SIZE(p_req->Token))
        {
            break;
        }

        p_Token = p_Token->next;
    }

    return ONVIF_OK;
}

ONVIF_RET parse_tsc_GetSpecialDayGroupInfoList(XMLN * p_node, tsc_GetSpecialDayGroupInfoList_REQ * p_req)
{
    XMLN * p_Limit;
    XMLN * p_StartReference;

    p_Limit = xml_node_soap_get(p_node, "Limit");
    if (p_Limit && p_Limit->data)
    {
        p_req->LimitFlag = 1;
        p_req->Limit = atoi(p_Limit->data);
    }

    p_StartReference = xml_node_soap_get(p_node, "StartReference");
    if (p_StartReference && p_StartReference->data)
    {
        p_req->StartReferenceFlag = 1;
        strncpy(p_req->StartReference, p_StartReference->data, sizeof(p_req->StartReference)-1);
    }

    return ONVIF_OK;
}

ONVIF_RET parse_tsc_GetSpecialDayGroups(XMLN * p_node, tsc_GetSpecialDayGroups_REQ * p_req)
{
    XMLN * p_Token;

    p_Token = xml_node_soap_get(p_node, "Token");
    while (p_Token && soap_strcmp(p_Token->name, "Token") == 0)
    {
        int idx = p_req->sizeToken;

        strncpy(p_req->Token[idx], p_Token->data, sizeof(p_req->Token[idx])-1);

        p_req->sizeToken++;
        if (p_req->sizeToken >= ARRAY_SIZE(p_req->Token))
        {
            break;
        }

        p_Token = p_Token->next;
    }

    return ONVIF_OK;
}

ONVIF_RET parse_tsc_GetSpecialDayGroupList(XMLN * p_node, tsc_GetSpecialDayGroupList_REQ * p_req)
{
    XMLN * p_Limit;
    XMLN * p_StartReference;

    p_Limit = xml_node_soap_get(p_node, "Limit");
    if (p_Limit && p_Limit->data)
    {
        p_req->LimitFlag = 1;
        p_req->Limit = atoi(p_Limit->data);
    }

    p_StartReference = xml_node_soap_get(p_node, "StartReference");
    if (p_StartReference && p_StartReference->data)
    {
        p_req->StartReferenceFlag = 1;
        strncpy(p_req->StartReference, p_StartReference->data, sizeof(p_req->StartReference)-1);
    }

    return ONVIF_OK;
}

ONVIF_RET parse_tsc_CreateSpecialDayGroup(XMLN * p_node, tsc_CreateSpecialDayGroup_REQ * p_req)
{
    XMLN * p_SpecialDayGroup;

    p_SpecialDayGroup = xml_node_soap_get(p_node, "SpecialDayGroup");
    if (p_SpecialDayGroup)
    {
        const char * p_token;

        p_token = xml_attr_get(p_SpecialDayGroup, "token");
        if (p_token)
        {
            strncpy(p_req->SpecialDayGroup.token, p_token, sizeof(p_req->SpecialDayGroup.token)-1);
        }

        parse_SpecialDayGroup(p_SpecialDayGroup, &p_req->SpecialDayGroup);
    }

    return ONVIF_OK;
}

ONVIF_RET parse_tsc_ModifySpecialDayGroup(XMLN * p_node, tsc_ModifySpecialDayGroup_REQ * p_req)
{
    XMLN * p_SpecialDayGroup;

    p_SpecialDayGroup = xml_node_soap_get(p_node, "SpecialDayGroup");
    if (p_SpecialDayGroup)
    {
        const char * p_token;

        p_token = xml_attr_get(p_SpecialDayGroup, "token");
        if (p_token)
        {
            strncpy(p_req->SpecialDayGroup.token, p_token, sizeof(p_req->SpecialDayGroup.token)-1);
        }

        parse_SpecialDayGroup(p_SpecialDayGroup, &p_req->SpecialDayGroup);
    }

    return ONVIF_OK;
}

ONVIF_RET parse_tsc_DeleteSpecialDayGroup(XMLN * p_node, tsc_DeleteSpecialDayGroup_REQ * p_req)
{
    XMLN * p_Token;

    p_Token = xml_node_soap_get(p_node, "Token");
    if (p_Token && p_Token->data)
    {
        strncpy(p_req->Token, p_Token->data, sizeof(p_req->Token)-1);
    }

    return ONVIF_OK;
}

ONVIF_RET parse_tsc_GetScheduleState(XMLN * p_node, tsc_GetScheduleState_REQ * p_req)
{
    XMLN * p_Token;

    p_Token = xml_node_soap_get(p_node, "Token");
    if (p_Token && p_Token->data)
    {
        strncpy(p_req->Token, p_Token->data, sizeof(p_req->Token)-1);
    }

    return ONVIF_OK;
}

#endif // end of SCHEDULE_SUPPORT

#ifdef RECEIVER_SUPPORT

ONVIF_RET parse_ReceiverConfiguration(XMLN * p_node, onvif_ReceiverConfiguration * p_req)
{
    ONVIF_RET ret = ONVIF_OK;
    XMLN * p_Mode;
    XMLN * p_MediaUri;
    XMLN * p_StreamSetup;

    p_Mode = xml_node_soap_get(p_node, "Mode");
    if (p_Mode && p_Mode->data)
    {
        p_req->Mode = onvif_StringToReceiverMode(p_Mode->data);
    }

    p_MediaUri = xml_node_soap_get(p_node, "MediaUri");
    if (p_MediaUri && p_MediaUri->data)
    {
        strncpy(p_req->MediaUri, p_MediaUri->data, sizeof(p_req->MediaUri)-1);
    }

    p_StreamSetup = xml_node_soap_get(p_node, "StreamSetup");
    if (p_StreamSetup)
    {
        ret = parse_StreamSetup(p_StreamSetup, &p_req->StreamSetup);
    }

    return ret;
}

ONVIF_RET parse_trv_GetReceiver(XMLN * p_node, trv_GetReceiver_REQ * p_req)
{
    XMLN * p_ReceiverToken;

    p_ReceiverToken = xml_node_soap_get(p_node, "ReceiverToken");
    if (p_ReceiverToken && p_ReceiverToken->data)
    {
        strncpy(p_req->ReceiverToken, p_ReceiverToken->data, sizeof(p_req->ReceiverToken)-1);
    }

    return ONVIF_OK;
}

ONVIF_RET parse_trv_CreateReceiver(XMLN * p_node, trv_CreateReceiver_REQ * p_req)
{
    ONVIF_RET ret = ONVIF_OK;
    XMLN * p_Configuration;

    p_Configuration = xml_node_soap_get(p_node, "Configuration");
    if (p_Configuration)
    {
        ret = parse_ReceiverConfiguration(p_Configuration, &p_req->Configuration);
    }

    return ret;
}

ONVIF_RET parse_trv_DeleteReceiver(XMLN * p_node, trv_DeleteReceiver_REQ * p_req)
{
    XMLN * p_ReceiverToken;

    p_ReceiverToken = xml_node_soap_get(p_node, "ReceiverToken");
    if (p_ReceiverToken && p_ReceiverToken->data)
    {
        strncpy(p_req->ReceiverToken, p_ReceiverToken->data, sizeof(p_req->ReceiverToken)-1);
    }

    return ONVIF_OK;
}

ONVIF_RET parse_trv_ConfigureReceiver(XMLN * p_node, trv_ConfigureReceiver_REQ * p_req)
{
    ONVIF_RET ret = ONVIF_OK;
    XMLN * p_ReceiverToken;
    XMLN * p_Configuration;

    p_ReceiverToken = xml_node_soap_get(p_node, "ReceiverToken");
    if (p_ReceiverToken && p_ReceiverToken->data)
    {
        strncpy(p_req->ReceiverToken, p_ReceiverToken->data, sizeof(p_req->ReceiverToken)-1);
    }

    p_Configuration = xml_node_soap_get(p_node, "Configuration");
    if (p_Configuration)
    {
        ret = parse_ReceiverConfiguration(p_Configuration, &p_req->Configuration);
    }
    
    return ret;
}

ONVIF_RET parse_trv_SetReceiverMode(XMLN * p_node, trv_SetReceiverMode_REQ * p_req)
{
    XMLN * p_ReceiverToken;
    XMLN * p_Mode;

    p_ReceiverToken = xml_node_soap_get(p_node, "ReceiverToken");
    if (p_ReceiverToken && p_ReceiverToken->data)
    {
        strncpy(p_req->ReceiverToken, p_ReceiverToken->data, sizeof(p_req->ReceiverToken)-1);
    }

    p_Mode = xml_node_soap_get(p_node, "Mode");
    if (p_Mode && p_Mode->data)
    {
        p_req->Mode = onvif_StringToReceiverMode(p_Mode->data);
    }
    
    return ONVIF_OK;
}

ONVIF_RET parse_trv_GetReceiverState(XMLN * p_node, trv_GetReceiverState_REQ * p_req)
{
    XMLN * p_ReceiverToken;

    p_ReceiverToken = xml_node_soap_get(p_node, "ReceiverToken");
    if (p_ReceiverToken && p_ReceiverToken->data)
    {
        strncpy(p_req->ReceiverToken, p_ReceiverToken->data, sizeof(p_req->ReceiverToken)-1);
    }

    return ONVIF_OK;
}

#endif // end of RECEIVER_SUPPORT



