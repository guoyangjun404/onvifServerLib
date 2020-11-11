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

#ifndef ONVIF_RECORDING_H
#define ONVIF_RECORDING_H

#include "sys_inc.h"
#include "onvif_cm.h"


#define SEARCH_TYPE_EVENTS      1
#define SEARCH_TYPE_RECORDING   2
#define SEARCH_TYPE_PTZPOS      3
#define SEARCH_TYPE_METADATA    4

typedef struct
{
    // if type is SEARCH_TYPE_EVENTS, req -> FindEvents_REQ
    // if type is SEARCH_TYPE_RECORDING, req -> FindRecordings_REQ
    // if type is SEARCH_TYPE_PTZPOS, req -> FindPTZPosition_REQ
    int     type; 
    void *  req;
    char    token[ONVIF_TOKEN_LEN];
} SUA;

typedef struct 
{
	char	RecordingToken[ONVIF_TOKEN_LEN];					// filled by onvif server, return to client
	
	onvif_RecordingConfiguration	RecordingConfiguration;		// required, Initial configuration for the recording
} CreateRecording_REQ;

typedef struct
{
	char	RecordingToken[ONVIF_TOKEN_LEN];					// required, Token of the recording that shall be changed
	
	onvif_RecordingConfiguration	RecordingConfiguration;		// required, The new configuration
} SetRecordingConfiguration_REQ;

typedef struct
{
	char	RecordingToken[ONVIF_TOKEN_LEN];					// required, Identifies the recording to which a track shall be added
	char	TrackToken[ONVIF_TOKEN_LEN];						// filled by onvif server, return to client
	
	onvif_TrackConfiguration	TrackConfiguration;				// required, The configuration of the new track
} CreateTrack_REQ;

typedef struct
{
	char 	RecordingToken[ONVIF_TOKEN_LEN];					// required, Token of the recording the track belongs to
	char 	TrackToken[ONVIF_TOKEN_LEN];						// required, Token of the track to be deleted
} DeleteTrack_REQ;

typedef struct
{
	char 	RecordingToken[ONVIF_TOKEN_LEN];					// required, Token of the recording the track belongs to
	char 	TrackToken[ONVIF_TOKEN_LEN];						// required, Token of the track
} GetTrackConfiguration_REQ;

typedef struct
{
	char 	RecordingToken[ONVIF_TOKEN_LEN];					// required, Token of the recording the track belongs to
	char 	TrackToken[ONVIF_TOKEN_LEN];						// required, Token of the track to be modified
	
	onvif_TrackConfiguration	TrackConfiguration;				// required, New configuration for the track
} SetTrackConfiguration_REQ;

typedef struct
{
	char	JobToken[ONVIF_TOKEN_LEN];							// filled by onvif server, return to client
	
	onvif_RecordingJobConfiguration	JobConfiguration;			// required, The initial configuration of the new recording job
} CreateRecordingJob_REQ;

typedef struct
{
	char	JobToken[ONVIF_TOKEN_LEN];							// required, Token of the job to be modified
	
	onvif_RecordingJobConfiguration	JobConfiguration;			// required, New configuration of the recording job
} SetRecordingJobConfiguration_REQ;

typedef struct
{
	char 	JobToken[ONVIF_TOKEN_LEN];							// required, Token of the recording job
	char 	Mode[16];											// required, The new mode for the recording job, The only valid values for Mode shall be "Idle" or "Active"
} SetRecordingJobMode_REQ;

typedef struct 
{
	onvif_RecordingSummary	Summary;							// required, 
} GetRecordingSummary_RES;

typedef struct
{
    char    RecordingToken[ONVIF_TOKEN_LEN];                    // required, 
} GetRecordingInformation_REQ;

typedef struct
{
	onvif_RecordingInformation	RecordingInformation;			// required, 
} GetRecordingInformation_RES;

typedef struct
{
	int 	sizeRecordingTokens;
	char 	RecordingTokens[10][ONVIF_TOKEN_LEN];				// optional, 
	
	time_t 	Time;												// required, 
} GetMediaAttributes_REQ;

typedef struct
{
	int 	sizeMediaAttributes;	
	onvif_MediaAttributes	MediaAttributes[10];				// optional, 
} GetMediaAttributes_RES;

typedef struct 
{
	uint32  MaxMatchesFlag	: 1;								// Indicates whether the field MaxMatches is valid
	uint32  Reserved		: 31;
	
	onvif_SearchScope	Scope;									// required, Scope defines the dataset to consider for this search
	
	int 	MaxMatches;											// optional, The search will be completed after this many matches. If not specified, the search will continue until reaching the endpoint or until the session expires
	int 	KeepAliveTime;										// required, The time the search session will be kept alive after responding to this and subsequent requests. A device shall support at least values up to ten seconds
} FindRecordings_REQ;

typedef struct 
{
	char 	SearchToken[ONVIF_TOKEN_LEN];						// required
} FindRecordings_RES;

typedef struct 
{
	uint32  MinResultsFlag	: 1;								// Indicates whether the field MinResults is valid
	uint32  MaxResultsFlag	: 1;								// Indicates whether the field MaxResults is valid
	uint32  WaitTimeFlag	: 1;								// Indicates whether the field WaitTime is valid
	uint32  Reserved		: 29;
	
	char 	SearchToken[ONVIF_TOKEN_LEN];						// required, The search session to get results from
	int 	MinResults;											// optional, The minimum number of results to return in one response
	int 	MaxResults;											// optional, The maximum number of results to return in one response
	int	 	WaitTime;											// optional, The maximum time before responding to the request, even if the MinResults parameter is not fulfilled
} GetRecordingSearchResults_REQ;

typedef struct
{
	onvif_FindRecordingResultList	ResultList;					// required
} GetRecordingSearchResults_RES;

typedef struct
{
	uint32  EndPointFlag		: 1;							// Indicates whether the field EndPoint is valid
	uint32  MaxMatchesFlag		: 1;							// Indicates whether the field MaxMatches is valid
	uint32  KeepAliveTimeFlag	: 1;							// Indicates whether the field KeepAliveTime is valid
	uint32  Reserved			: 29;
	
	time_t 	StartPoint;											// required, The point of time where the search will start
	time_t	EndPoint;											// optional, The point of time where the search will stop. This can be a time before the StartPoint, in which case the search is performed backwards in time

	onvif_SearchScope	Scope;									// required,

	BOOL 	IncludeStartState;									// required, 
	int 	MaxMatches;											// optional, The search will be completed after this many matches. If not specified, the search will continue until reaching the endpoint or until the session expires
	int	 	KeepAliveTime;										// optional, The time the search session will be kept alive after responding to this and subsequent requests. A device shall support at least values up to ten seconds
} FindEvents_REQ;

typedef struct 
{
	char	SearchToken[ONVIF_TOKEN_LEN];						// required, A unique reference to the search session created by this request
} FindEvents_RES;

typedef struct 
{
	uint32  MinResultsFlag	: 1;								// Indicates whether the field MinResults is valid
	uint32  MaxResultsFlag	: 1;								// Indicates whether the field MaxResults is valid
	uint32  WaitTimeFlag	: 1;								// Indicates whether the field WaitTime is valid
	uint32  Reserved		: 29;
	
	char 	SearchToken[ONVIF_TOKEN_LEN];						// required, The search session to get results from
	int 	MinResults;											// optional, The minimum number of results to return in one response
	int 	MaxResults;											// optional, The maximum number of results to return in one response
	int	 	WaitTime;											// optional, The maximum time before responding to the request, even if the MinResults parameter is not fulfilled
} GetEventSearchResults_REQ;

typedef struct 
{
	onvif_FindEventResultList	ResultList;						// required
} GetEventSearchResults_RES;

typedef struct
{
    uint32  EndPointFlag		: 1;							// Indicates whether the field EndPoint is valid
	uint32  MaxMatchesFlag		: 1;							// Indicates whether the field MaxMatches is valid
	uint32  KeepAliveTimeFlag	: 1;							// Indicates whether the field KeepAliveTime is valid
	uint32  Reserved			: 29;
	
	time_t 	StartPoint;											// required, The point of time where the search will start
	time_t	EndPoint;											// optional, The point of time where the search will stop. This can be a time before the StartPoint, in which case the search is performed backwards in time

	onvif_SearchScope	    Scope;						        // required,
    onvif_PTZPositionFilter SearchFilter;                       // required,

	int 	MaxMatches;											// optional, The search will be completed after this many matches. If not specified, the search will continue until reaching the endpoint or until the session expires
	int	 	KeepAliveTime;										// optional, The time the search session will be kept alive after responding to this and subsequent requests. A device shall support at least values up to ten seconds
} FindPTZPosition_REQ;

typedef struct
{
    char	SearchToken[ONVIF_TOKEN_LEN];						// required, A unique reference to the search session created by this request
} FindPTZPosition_RES;

typedef struct
{
    uint32  MinResultsFlag	: 1;								// Indicates whether the field MinResults is valid
	uint32  MaxResultsFlag	: 1;								// Indicates whether the field MaxResults is valid
	uint32  WaitTimeFlag	: 1;								// Indicates whether the field WaitTime is valid
	uint32  Reserved		: 29;
	
	char 	SearchToken[ONVIF_TOKEN_LEN];						// required, The search session to get results from
	int 	MinResults;											// optional, The minimum number of results to return in one response
	int 	MaxResults;											// optional, The maximum number of results to return in one response
	int	 	WaitTime;											// optional, The maximum time before responding to the request, even if the MinResults parameter is not fulfilled
} GetPTZPositionSearchResults_REQ;

typedef struct
{
    onvif_FindPTZPositionResultList	ResultList;				    // required
} GetPTZPositionSearchResults_RES;

typedef struct
{
    uint32  EndPointFlag		: 1;							// Indicates whether the field EndPoint is valid
	uint32  MaxMatchesFlag		: 1;							// Indicates whether the field MaxMatches is valid
	uint32  KeepAliveTimeFlag	: 1;							// Indicates whether the field KeepAliveTime is valid
	uint32  Reserved			: 29;
	
	time_t 	StartPoint;											// required, The point of time where the search will start
	time_t	EndPoint;											// optional, The point of time where the search will stop. This can be a time before the StartPoint, in which case the search is performed backwards in time

	onvif_SearchScope	    Scope;						        // required,
    onvif_MetadataFilter    MetadataFilter;                     // required,

	int 	MaxMatches;											// optional, The search will be completed after this many matches. If not specified, the search will continue until reaching the endpoint or until the session expires
	int	 	KeepAliveTime;										// optional, The time the search session will be kept alive after responding to this and subsequent requests. A device shall support at least values up to ten seconds
} FindMetadata_REQ;

typedef struct
{
    char	SearchToken[ONVIF_TOKEN_LEN];						// required, A unique reference to the search session created by this request
} FindMetadata_RES;

typedef struct
{
    uint32  MinResultsFlag	: 1;								// Indicates whether the field MinResults is valid
	uint32  MaxResultsFlag	: 1;								// Indicates whether the field MaxResults is valid
	uint32  WaitTimeFlag	: 1;								// Indicates whether the field WaitTime is valid
	uint32  Reserved		: 29;
	
	char 	SearchToken[ONVIF_TOKEN_LEN];						// required, The search session to get results from
	int 	MinResults;											// optional, The minimum number of results to return in one response
	int 	MaxResults;											// optional, The maximum number of results to return in one response
	int	 	WaitTime;											// optional, The maximum time before responding to the request, even if the MinResults parameter is not fulfilled
} GetMetadataSearchResults_REQ;

typedef struct
{
    onvif_FindMetadataResultList    ResultList;				    // required
} GetMetadataSearchResults_RES;

typedef struct
{
	char	SearchToken[ONVIF_TOKEN_LEN];						// required, The search session to end
} EndSearch_REQ;

typedef struct
{
	time_t 	Endpoint;											// required, The point of time the search had reached when it was ended. It is equal to the EndPoint specified in Find-operation if the search was completed
} EndSearch_RES;

typedef struct
{
	char	SearchToken[ONVIF_TOKEN_LEN];						// required, The search session to get the state from
} GetSearchState_REQ;

typedef struct
{
	onvif_SearchState	State;									// required, 
} GetSearchState_RES;

typedef struct 
{
	onvif_StreamSetup	StreamSetup;							// required, Specifies the connection parameters to be used for the stream. The URI that is returned may depend on these parameters
	
	char 	RecordingToken[ONVIF_TOKEN_LEN];					// required, The identifier of the recording to be streamed
} GetReplayUri_REQ;

typedef struct 
{
	char 	Uri[256];											// required, The URI to which the client should connect in order to stream the recording
} GetReplayUri_RES;

typedef struct
{
	int 	SessionTimeout;										// required, The RTSP session timeout, unit is second
} GetReplayConfiguration_RES;

typedef struct
{
	int 	SessionTimeout;										// required, The RTSP session timeout, unit is second
} SetReplayConfiguration_REQ;


#ifdef __cplusplus
extern "C" {
#endif

ONVIF_RET onvif_CreateRecording(CreateRecording_REQ * p_req);
ONVIF_RET onvif_DeleteRecording(const char * p_RecordingToken);
ONVIF_RET onvif_SetRecordingConfiguration(SetRecordingConfiguration_REQ * p_req);
ONVIF_RET onvif_CreateTrack(CreateTrack_REQ * p_req);
ONVIF_RET onvif_DeleteTrack(DeleteTrack_REQ * p_req);
ONVIF_RET onvif_SetTrackConfiguration(SetTrackConfiguration_REQ * p_req);
ONVIF_RET onvif_CreateRecordingJob(CreateRecordingJob_REQ  * p_req);
ONVIF_RET onvif_DeleteRecordingJob(const char * p_JobToken);
ONVIF_RET onvif_SetRecordingJobConfiguration(SetRecordingJobConfiguration_REQ * p_req);
ONVIF_RET onvif_SetRecordingJobMode(SetRecordingJobMode_REQ * p_req);
ONVIF_RET onvif_GetRecordingJobState(const char * p_JobToken, onvif_RecordingJobStateInformation * p_res);
ONVIF_RET onvif_GetRecordingOptions(const char * p_RecordingToken, onvif_RecordingOptions * p_res);

ONVIF_RET onvif_GetRecordingSummary(GetRecordingSummary_RES * p_summary);
ONVIF_RET onvif_GetRecordingInformation(const char * p_RecordingToken, GetRecordingInformation_RES * p_res);
ONVIF_RET onvif_GetMediaAttributes(GetMediaAttributes_REQ * p_req, GetMediaAttributes_RES * p_res);
ONVIF_RET onvif_FindRecordings(FindRecordings_REQ * p_req, FindRecordings_RES * p_res);
ONVIF_RET onvif_GetRecordingSearchResults(GetRecordingSearchResults_REQ * p_req, GetRecordingSearchResults_RES * p_res);
ONVIF_RET onvif_FindEvents(FindEvents_REQ * p_req, FindEvents_RES * p_res);
ONVIF_RET onvif_GetEventSearchResults(GetEventSearchResults_REQ * p_req, GetEventSearchResults_RES * p_res);
ONVIF_RET onvif_FindMetadata(FindMetadata_REQ * p_req, FindMetadata_RES * p_res);
ONVIF_RET onvif_GetMetadataSearchResults(GetMetadataSearchResults_REQ * p_req, GetMetadataSearchResults_RES * p_res);
ONVIF_RET onvif_FindPTZPosition(FindPTZPosition_REQ * p_req, FindPTZPosition_RES * p_res);
ONVIF_RET onvif_GetPTZPositionSearchResults(GetPTZPositionSearchResults_REQ * p_req, GetPTZPositionSearchResults_RES * p_res);
ONVIF_RET onvif_EndSearch(EndSearch_REQ * p_req, EndSearch_RES * p_res);
ONVIF_RET onvif_GetSearchState(GetSearchState_REQ * p_req, GetSearchState_RES * p_res);
void      onvif_freeSearchs();

ONVIF_RET onvif_GetReplayUri(const char * lip, uint32 lport, GetReplayUri_REQ * p_req, GetReplayUri_RES * p_res);
ONVIF_RET onvif_GetReplayConfiguration(GetReplayConfiguration_RES * p_res);
ONVIF_RET onvif_SetReplayConfiguration(SetReplayConfiguration_REQ * p_req);

#ifdef __cplusplus
}
#endif


#endif	// end of ONVIF_RECORDING_H


