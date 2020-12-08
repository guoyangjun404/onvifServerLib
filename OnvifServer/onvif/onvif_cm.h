
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

#ifndef ONVIF_COMM_H
#define ONVIF_COMM_H

#ifdef LIBICAL
#include "icalvcal.h"
#include "vcc.h"
#endif

/***************************************************************************************/
#define ONVIF_TOKEN_LEN   	    100
#define ONVIF_NAME_LEN    	    100
#define ONVIF_URI_LEN     	    300

// #define MAX_PTZ_PRESETS         100 
#define MAX_PTZ_PRESETS         128
#define MAX_DNS_SERVER		    2
#define MAX_SEARCHDOMAIN	    4
#define MAX_NTP_SERVER		    2
#define MAX_SERVER_PORT		    4
#define MAX_GATEWAY			    2
#define MAX_RES_NUMS		    8
#define MAX_SCOPE_NUMS		    100
#define MAX_USERS			    10
#define MAX_SERVERS		        8

#define XADDR_LEN               500

/* Floating point precision */
#define FPP					    0.01

#define ACCESS_CTRL_MAX_LIMIT   10
#define DOOR_CTRL_MAX_LIMIT     10
#define CREDENTIAL_MAX_LIMIT    10
#define ACCESSRULES_MAX_LIMIT   10
#define SCHEDULE_MAX_LIMIT      10

/***************************************************************************************/
typedef enum 
{
	ONVIF_OK = 0,
	ONVIF_ERR_InvalidIPv4Address = -1,
	ONVIF_ERR_InvalidIPv6Address = -2,
	ONVIF_ERR_InvalidDnsName = -3,
	ONVIF_ERR_ServiceNotSupported = -4,
	ONVIF_ERR_PortAlreadyInUse = -5,
	ONVIF_ERR_InvalidGatewayAddress = -6,
	ONVIF_ERR_InvalidHostname = -7,
	ONVIF_ERR_MissingAttribute = -8,
	ONVIF_ERR_InvalidDateTime = -9,
	ONVIF_ERR_InvalidTimeZone = -10,
	ONVIF_ERR_ProfileExists = -11,
	ONVIF_ERR_MaxNVTProfiles = -12,
	ONVIF_ERR_NoProfile = -13,
	ONVIF_ERR_DeletionOfFixedProfile = -14,
	ONVIF_ERR_NoConfig = -15,
	ONVIF_ERR_NoPTZProfile = -16,
	ONVIF_ERR_NoHomePosition = -17,
	ONVIF_ERR_NoToken = - 18,
	ONVIF_ERR_PresetExist = -19,
	ONVIF_ERR_TooManyPresets = -20,
	ONVIF_ERR_MovingPTZ = -21,
	ONVIF_ERR_NoEntity = -22,
	ONVIF_ERR_InvalidNetworkInterface = -23, 
	ONVIF_ERR_InvalidMtuValue = -24,
	ONVIF_ERR_ConfigModify = -25,
	ONVIF_ERR_ConfigurationConflict = -26,
	ONVIF_ERR_InvalidPosition = -27,
	ONVIF_ERR_TooManyScopes = -28,
	ONVIF_ERR_FixedScope = -29,
	ONVIF_ERR_NoScope = -30,
	ONVIF_ERR_ScopeOverwrite = -31,
	ONVIF_ERR_ResourceUnknownFault = -32,
	ONVIF_ERR_NoSource = -33,
	ONVIF_ERR_CannotOverwriteHome = -34,
	ONVIF_ERR_SettingsInvalid = -35,
	ONVIF_ERR_NoImagingForSource = -36,
	ONVIF_ERR_UsernameClash = -37,
	ONVIF_ERR_PasswordTooLong = -38,
	ONVIF_ERR_UsernameTooLong = -39,
	ONVIF_ERR_Password = -40,
	ONVIF_ERR_TooManyUsers = -41,
	ONVIF_ERR_AnonymousNotAllowed = -42,
	ONVIF_ERR_UsernameTooShort = -43,
	ONVIF_ERR_UsernameMissing = -44,
	ONVIF_ERR_FixedUser = -45,
	ONVIF_ERR_MaxOSDs = -46,
	ONVIF_ERR_InvalidStreamSetup = -47,
	ONVIF_ERR_BadConfiguration = -48,
	ONVIF_ERR_MaxRecordings = -49,
	ONVIF_ERR_NoRecording = -50,
	ONVIF_ERR_CannotDelete = -51,
	ONVIF_ERR_MaxTracks = -52,
	ONVIF_ERR_NoTrack = -53,
	ONVIF_ERR_MaxRecordingJobs = -54,
	ONVIF_ERR_MaxReceivers = -55,                           // The maximum supported number of receivers has been reached
	ONVIF_ERR_NoRecordingJob = -56,
	ONVIF_ERR_BadMode = -57,
	ONVIF_ERR_InvalidToken = -58,
	
	ONVIF_ERR_InvalidRule = -59,
	ONVIF_ERR_RuleAlreadyExistent = -60,
	ONVIF_ERR_TooManyRules = -61,
	ONVIF_ERR_RuleNotExistent = -63,
	ONVIF_ERR_NameAlreadyExistent = -64,
	ONVIF_ERR_TooManyModules = -65,
	ONVIF_ERR_InvalidModule = -66,
	ONVIF_ERR_NameNotExistent = -67, 

	ONVIF_ERR_InvalidFilterFault = -68,
	ONVIF_ERR_InvalidTopicExpressionFault = -69,
	ONVIF_ERR_TopicNotSupportedFault = -70,
	ONVIF_ERR_InvalidMessageContentExpressionFault = -71,

	ONVIF_ERR_InvalidStartReference = -72,                  // StartReference is invalid or has timed out. Client needs to start fetching from the beginning
	ONVIF_ERR_TooManyItems = -73,                           // Too many items were requested, see MaxLimit capability
	ONVIF_ERR_NotFound = -74,                               // AccessPoint is not found 
	ONVIF_ERR_NotSupported = -75,                           // The operation is not supported
	ONVIF_ERR_Failure = -76,                                // Failed to go to Accessed state and unlock the door

	ONVIF_ERR_NoVideoOutput = -77,                          // The requested VideoOutput indicated with VideoOutputToken does not exist
    ONVIF_ERR_NoAudioOutput = -78,                          // The requested AudioOutput indicated with AudioOutputToken does not exist
    ONVIF_ERR_RelayToken = -79,                             // Unknown relay token reference
    ONVIF_ERR_ModeError = -80,                              // Monostable delay time not valid
    ONVIF_ERR_InvalidSerialPort = -81,                      // The supplied serial port token does not exist
    ONVIF_ERR_DataLengthOver = -82,                         // Number of available bytes exceeded
    ONVIF_ERR_DelimiterNotSupported = -83,                  // Sequence of character (delimiter) is not supported

    ONVIF_ERR_InvalidDot11 = -84,                           // IEEE 802.11 configuration is not supported
    ONVIF_ERR_NotDot11 = -85,                               // The interface is not an IEEE 802.11 interface
    ONVIF_ERR_NotConnectedDot11 = -86,                      // IEEE 802.11 network is not connected
    ONVIF_ERR_NotScanAvailable = -87,                       // ScanAvailableDot11Networks is not supported

    ONVIF_ERR_NotRemoteUser = -88,                          // Remote User handling is not supported

    ONVIF_ERR_NoVideoSource = -89,                          // The requested video source does not exist
    ONVIF_ERR_NoVideoSourceMode = -90,                      // The requested video source mode does not exist

    ONVIF_ERR_NoThermalForSource = -91,                     // The requested VideoSource does not support thermal settings
    ONVIF_ERR_NoRadiometryForSource = -92,                  // The requested VideoSource does not support radiometry settings
    ONVIF_ERR_InvalidConfiguration = -93,                   // The requested configuration is incorrect

    ONVIF_ERR_MaxAccessProfilesPerCredential = -94,         // There are too many access profiles per credential
    ONVIF_ERR_CredentialValiditySupported = -95,            // Credential validity is not supported by device
    ONVIF_ERR_CredentialAccessProfileValiditySupported = -96,// Credential access profile validity is not supported by the device
    ONVIF_ERR_SupportedIdentifierType = -97,                // Specified identifier type is not supported by device
    ONVIF_ERR_DuplicatedIdentifierType = -98,               // The same identifier type was used more than once
    ONVIF_ERR_InvalidFormatType = -99,                      // Specified identifier format type is not supported by the device
    ONVIF_ERR_InvalidIdentifierValue = -100,                // Specified identifier value is not as per FormatType definition
    ONVIF_ERR_DuplicatedIdentifierValue = -101,             // The same combination of identifier type, format and value was used more than once
    ONVIF_ERR_ReferenceNotFound = -102,                     // A referred entity token is not found
    ONVIF_ERR_ExemptFromAuthenticationSupported = -103,     // Exempt from authentication is not supported by the device
    ONVIF_ERR_MaxCredentials = -104,                        // There is not enough space to create a new credential
    ONVIF_ERR_ReferenceInUse = -105,                        // Failed to delete, credential token is in use
    ONVIF_ERR_MinIdentifiersPerCredential = -106,           // At least one credential identifier is required
    ONVIF_ERR_InvalidArgs = -107,

    ONVIF_ERR_MaxAccessProfiles = -108,                     // There is not enough space to add new AccessProfile, see the MaxAccessProfiles capability
    ONVIF_ERR_MaxAccessPoliciesPerAccessProfile = -109,     // There are too many AccessPolicies in anAccessProfile, see MaxAccessPoliciesPerAccessProfile capability
    ONVIF_ERR_MultipleSchedulesPerAccessPointSupported = -110, // Multiple AccessPoints are not supported for the same schedule, see MultipleSchedulesPerAccessPointSupported capability
    ONVIF_ERR_InvalidArgVal = -111,    

    ONVIF_ERR_MaxSchedules = -112,                          // There is not enough space to add new schedule, see MaxSchedules capability
    ONVIF_ERR_MaxSpecialDaysSchedules = -113,               // There are too many SpecialDaysSchedule entities referred in this schedule, see MaxSpecialDaysSchedules capability
    ONVIF_ERR_MaxTimePeriodsPerDay = -114,                  // There are too many time periods in a day schedule, see MaxTimePeriodsPerDay capability
    ONVIF_ERR_MaxSpecialDayGroups = -115,                   // There is not enough space to add new SpecialDayGroup items, see the MaxSpecialDayGroups capabilit
    ONVIF_ERR_MaxDaysInSpecialDayGroup = -116,              // There are too many special days in a SpecialDayGroup, see MaxDaysInSpecialDayGroup capability

    ONVIF_ERR_UnknownToken = -117,                          // The receiver indicated by ReceiverToken does not exist
    ONVIF_ERR_CannotDeleteReceiver = -118,                  // It is not possible to delete the specified receiver, for example because it is currently in use    

    ONVIF_ERR_MaxMasks = -119,

    ONVIF_ERR_IPFilterListIsFull  = -120,                   // It is not possible to add more IP filters since the IP filter list is full. 
    ONVIF_ERR_NoIPv4Address = -121,                         // The IPv4 address to be removed does not exist
    ONVIF_ERR_NoIPv6Address = -122,                         // The IPv6 address to be removed does not exist
    
} ONVIF_RET;

/***************************************************************************************/

typedef enum 
{
	CapabilityCategory_Invalid = -1,
	CapabilityCategory_All = 0, 
	CapabilityCategory_Analytics = 1, 
	CapabilityCategory_Device = 2, 
	CapabilityCategory_Events = 3, 
	CapabilityCategory_Imaging = 4, 
	CapabilityCategory_Media = 5, 
	CapabilityCategory_PTZ = 6,
	CapabilityCategory_Recording = 7,
	CapabilityCategory_Search = 8,
	CapabilityCategory_Replay = 9,
	CapabilityCategory_AccessControl = 10,
	CapabilityCategory_DoorControl = 11,
	CapabilityCategory_DeviceIO = 12,
	CapabilityCategory_Media2 = 13,
	CapabilityCategory_Thermal = 14,
	CapabilityCategory_Credential = 15,
	CapabilityCategory_AccessRules = 16,
	CapabilityCategory_Schedule = 17,
	CapabilityCategory_Receiver = 18,
} onvif_CapabilityCategory;

/////    add by xieqingpu
typedef enum
{
	NO  = 0,
	WhiteFever,
	BlackFever,
	BlueRedYellow,
	PurplishRedYellow,
	BlueGreenRed,
	Rainbow1,
	Rainbow2,
	BlackRed,
	BlackGreenRed,
	BlueGreenRed_powder,
	Mixed_colors,
	RedHead
} onvif_UserPalette;

typedef enum
{
	WDynamicOFF= 0,
	WDynamicNO = 1
} onvif_WideDynamic;

typedef enum
{
	OrgData0= 0,
	OrgData1 = 1
} onvif_OrgData;
/////

typedef enum 
{
	FactoryDefaultType_Hard = 0, 
	FactoryDefaultType_Soft = 1
} onvif_FactoryDefaultType;

typedef enum 
{
	SystemLogType_System = 0,
	SystemLogType_Access = 1
} onvif_SystemLogType;

typedef enum  
{
    VideoEncoding_Unknown = -1,
	VideoEncoding_JPEG = 0, 
	VideoEncoding_MPEG4 = 1, 
	VideoEncoding_H264 = 2,
	VideoEncoding_H265 = 3
} onvif_VideoEncoding;

typedef enum  
{
    AudioEncoding_Unknown = -1,
	AudioEncoding_G711 = 0, 
	AudioEncoding_G726 = 1, 
	AudioEncoding_AAC = 2
} onvif_AudioEncoding;

typedef enum H264Profile 
{
	H264Profile_Baseline = 0, 
	H264Profile_Main = 1, 
	H264Profile_Extended = 2, 
	H264Profile_High = 3
} onvif_H264Profile;

typedef enum  
{
	Mpeg4Profile_SP = 0,
	Mpeg4Profile_ASP = 1
} onvif_Mpeg4Profile;

typedef enum  
{
	UserLevel_Administrator = 0, 
	UserLevel_Operator = 1,
	UserLevel_User = 2, 
	UserLevel_Anonymous = 3,
	UserLevel_Extended = 4
} onvif_UserLevel;

typedef enum 
{
    IPAddressFilterType_Allow = 0, 
    IPAddressFilterType_Deny = 1
} onvif_IPAddressFilterType;

typedef enum MoveStatus 
{
	MoveStatus_IDLE = 0, 
	MoveStatus_MOVING = 1, 
	MoveStatus_UNKNOWN = 2
} onvif_MoveStatus;

// OSD type
typedef enum 
{
	OSDType_Text = 0,
	OSDType_Image = 1,
	OSDType_Extended =2
} onvif_OSDType;

// OSD position type
typedef enum
{
	OSDPosType_UpperLeft = 0,
	OSDPosType_UpperRight = 1,
	OSDPosType_LowerLeft = 2,
	OSDPosType_LowerRight = 3,
	OSDPosType_Custom = 4
} onvif_OSDPosType;

typedef enum
{
	OSDTextType_Plain,								    // The Plain type means the OSD is shown as a text string which defined in the "PlainText" item
	OSDTextType_Date,								    // The Date type means the OSD is shown as a date, format of which should be present in the "DateFormat" item
	OSDTextType_Time,								    // The Time type means the OSD is shown as a time, format of which should be present in the "TimeFormat" item
	OSDTextType_DateAndTime,						    // The DateAndTime type means the OSD is shown as date and time, format of which should be present in the "DateFormat" and the "TimeFormat" item
} onvif_OSDTextType;

// BacklightCompensation mode
typedef enum 
{
	BacklightCompensationMode_OFF = 0, 				    // Backlight compensation is disabled
	BacklightCompensationMode_ON = 1				    // Backlight compensation is enabled
} onvif_BacklightCompensationMode;

// Exposure mode
typedef enum  
{
	ExposureMode_AUTO = 0, 
	ExposureMode_MANUAL = 1
} onvif_ExposureMode;

// Exposure Priority
typedef enum  
{
	ExposurePriority_LowNoise = 0, 
	ExposurePriority_FrameRate = 1
} onvif_ExposurePriority;

// AutoFocus Mode
typedef enum 
{
	AutoFocusMode_AUTO = 0, 
	AutoFocusMode_MANUAL = 1
} onvif_AutoFocusMode;

typedef enum  
{
	WideDynamicMode_OFF = 0, 
	WideDynamicMode_ON = 1
} onvif_WideDynamicMode;

typedef enum  
{
	IrCutFilterMode_ON = 0,
	IrCutFilterMode_OFF = 1, 
	IrCutFilterMode_AUTO = 2
} onvif_IrCutFilterMode;

typedef enum WhiteBalanceMode 
{
	WhiteBalanceMode_AUTO = 0, 
	WhiteBalanceMode_MANUAL = 1
} onvif_WhiteBalanceMode;

typedef enum onvif_EFlipMode 
{
	EFlipMode_OFF = 0, 
	EFlipMode_ON = 1, 
	EFlipMode_Extended = 2
} onvif_EFlipMode;

typedef enum 
{
	ReverseMode_OFF = 0, 
	ReverseMode_ON = 1, 
	ReverseMode_AUTO = 2, 
	ReverseMode_Extended = 3
} onvif_ReverseMode;

typedef enum  
{
	DiscoveryMode_Discoverable = 0, 
	DiscoveryMode_NonDiscoverable = 1
} onvif_DiscoveryMode;

typedef enum  
{
	SetDateTimeType_Manual = 0,                         // Indicates that the date and time are set manually
	SetDateTimeType_NTP = 1                             // Indicates that the date and time are set through NTP
} onvif_SetDateTimeType;

typedef enum  
{
	StreamType_Invalid = -1,
	StreamType_RTP_Unicast = 0,
	StreamType_RTP_Multicast = 1
} onvif_StreamType;

typedef enum  
{
	TransportProtocol_Invalid = -1,
	TransportProtocol_UDP = 0, 
	TransportProtocol_TCP = 1, 
	TransportProtocol_RTSP = 2, 
	TransportProtocol_HTTP = 3
} onvif_TransportProtocol;

typedef enum
{
	TrackType_Invalid = -1,
	TrackType_Video = 0, 
	TrackType_Audio = 1, 
	TrackType_Metadata = 2,
	TrackType_Extended = 3
} onvif_TrackType;

typedef enum
{
	DynamicDNSType_NoUpdate = 0, 
	DynamicDNSType_ClientUpdates = 1, 
	DynamicDNSType_ServerUpdates = 2
} onvif_DynamicDNSType;

typedef enum
{
	PropertyOperation_Invalid = -1,
	PropertyOperation_Initialized = 0,
	PropertyOperation_Deleted = 1, 
	PropertyOperation_Changed = 2
} onvif_PropertyOperation;

typedef enum 
{ 
	RecordingStatus_Initiated = 0, 
	RecordingStatus_Recording = 1, 
	RecordingStatus_Stopped = 2, 
	RecordingStatus_Removing = 3, 
	RecordingStatus_Removed = 4, 
	RecordingStatus_Unknown = 5 
} onvif_RecordingStatus;

typedef enum
{
	SearchState_Queued = 0, 						    // The search is queued and not yet started.
	SearchState_Searching = 1,						    // The search is underway and not yet completed
	SearchState_Completed = 2,						    // The search has been completed and no new results will be found
	SearchState_Unknown = 3							    // The state of the search is unknown. (This is not a valid response from GetSearchState.)
} onvif_SearchState;

typedef enum 
{
    RotateMode_OFF = 0,                                 // Enable the Rotate feature. Degree of rotation is specified Degree parameter
    RotateMode_ON = 1,                                  // Disable the Rotate feature
    RotateMode_AUTO = 2                                 // Rotate feature is automatically activated by the device
} onvif_RotateMode;

typedef enum
{
    ScopeDefinition_Fixed = 0, 
    ScopeDefinition_Configurable = 1
} onvif_ScopeDefinition;

// The physical state of a Door
typedef enum  
{
    DoorPhysicalState_Unknown = 0,                      // Value is currently unknown (possibly due to initialization or monitors not giving a conclusive result)
    DoorPhysicalState_Open = 1,                         // Door is open
    DoorPhysicalState_Closed = 2,                       // Door is closed
    DoorPhysicalState_Fault = 3                         // Door monitor fault is detected
} onvif_DoorPhysicalState;

// The physical state of a Lock (including Double Lock)
typedef enum  
{
    LockPhysicalState_Unknown = 0,                      // Value is currently not known
    LockPhysicalState_Locked = 1,                       // Lock is activated
    LockPhysicalState_Unlocked = 2,                     // Lock is not activated
    LockPhysicalState_Fault = 3                         // Lock fault is detected
} onvif_LockPhysicalState;

// Describes the state of a Door with regard to alarms
typedef enum  
{
    DoorAlarmState_Normal = 0,                          // No alarm
    DoorAlarmState_DoorForcedOpen = 1,                  // Door is forced open
    DoorAlarmState_DoorOpenTooLong = 2                  // Door is held open too long
} onvif_DoorAlarmState;

// Describes the state of a Tamper detector
typedef enum  
{
    DoorTamperState_Unknown = 0,                        // Value is currently not known
    DoorTamperState_NotInTamper = 1,                    // No tampering is detected
    DoorTamperState_TamperDetected = 2                  // Tampering is detected
} onvif_DoorTamperState;

// Describes the state of a Door fault
typedef enum  
{
    DoorFaultState_Unknown = 0,                         // Fault state is unknown
    DoorFaultState_NotInFault = 1,                      // No fault is detected
    DoorFaultState_FaultDetected = 2                    // Fault is detected
} onvif_DoorFaultState;

// DoorMode parameters describe current Door mode from a logical perspective
typedef enum  
{
    DoorMode_Unknown = 0,                               // The Door is in an Unknown state
    DoorMode_Locked = 1,                                // The Door is in a Locked state. In this mode the device shall provide momentary access using the AccessDoor method if supported by the Door instance
    DoorMode_Unlocked = 2,                              // The Door is in an Unlocked (Permanent Access) state. Alarms related to door timing operations such as open too long or forced are masked in this mode
    DoorMode_Accessed = 3,                              // The Door is in an Accessed state (momentary/temporary access). Alarms related to timing operations such as "door forced" are masked in this mode
    DoorMode_Blocked = 4,                               // The Door is in a Blocked state (Door is locked, and AccessDoor requests are ignored, i.e., it is not possible for door to go to Accessed state)
    DoorMode_LockedDown = 5,                            // The Door is in a LockedDown state (Door is locked) until released using the LockDownReleaseDoor command. AccessDoor, LockDoor, UnlockDoor, BlockDoor and 
                                                        //  LockOpenDoor requests are ignored, i.e., it is not possible for door to go to Accessed, Locked, Unlocked, Blocked or LockedOpen state    
    DoorMode_LockedOpen = 6,                            // The Door is in a LockedOpen state (Door is unlocked) until released using the LockOpenReleaseDoor command. AccessDoor, LockDoor, UnlockDoor, BlockDoor and 
                                                        //  LockDownDoor requests are ignored, i.e., it is not possible for door to go to Accessed, Locked, Unlocked, Blocked or LockedDown state
    DoorMode_DoubleLocked = 7                           // The Door is in a Double Locked state - for doors with multiple locks. If the door does not have any DoubleLock, this shall be treated as a normal Locked mode. 
                                                        //  When changing to an Unlocked mode from the DoubleLocked mode, the door may first go to Locked state before unlocking
} onvif_DoorMode;

typedef enum
{
    RelayMode_Monostable = 0,                           // After setting the state, the relay returns to its idle state after the specified time
    RelayMode_Bistable = 1,                             // After setting the state, the relay remains in this state
} onvif_RelayMode;

typedef enum
{
    RelayIdleState_closed = 0,                          // means that the relay is closed when the relay state is set to 'inactive' through the trigger command and open when the state is set to 'active' through the same command
    RelayIdleState_open = 1,                            // means that the relay is open when the relay state is set to 'inactive' through the trigger command and closed when the state is set to 'active' through the same command    
} onvif_RelayIdleState;

typedef enum  
{
    RelayLogicalState_active = 0,                       // 
    RelayLogicalState_inactive = 1,                     // 
} onvif_RelayLogicalState;

typedef enum  
{
    DigitalIdleState_closed = 0, 
    DigitalIdleState_open = 1,
} onvif_DigitalIdleState;

typedef enum  
{
    ParityBit_None = 0,
    ParityBit_Even = 1, 
    ParityBit_Odd = 2, 
    ParityBit_Mark = 3, 
    ParityBit_Space = 4, 
    ParityBit_Extended = 5
} onvif_ParityBit;

typedef enum  
{
    SerialPortType_RS232 = 0, 
    SerialPortType_RS422HalfDuplex = 1, 
    SerialPortType_RS422FullDuplex = 2, 
    SerialPortType_RS485HalfDuplex = 3, 
    SerialPortType_RS485FullDuplex = 4, 
    SerialPortType_Generic = 5
} onvif_SerialPortType;

typedef enum 
{
    PTZPresetTourOperation_Start = 0, 
    PTZPresetTourOperation_Stop = 1, 
    PTZPresetTourOperation_Pause = 2, 
    PTZPresetTourOperation_Extended = 3
} onvif_PTZPresetTourOperation;

typedef enum 
{
    PTZPresetTourState_Idle = 0, 
    PTZPresetTourState_Touring = 1,
    PTZPresetTourState_Paused = 2, 
    PTZPresetTourState_Extended = 3
} onvif_PTZPresetTourState;

typedef enum 
{
    PTZPresetTourDirection_Forward = 0,
    PTZPresetTourDirection_Backward = 1,
    PTZPresetTourDirection_Extended = 2
} onvif_PTZPresetTourDirection;

typedef enum 
{
    Dot11AuthAndMangementSuite_None = 0, 
    Dot11AuthAndMangementSuite_Dot1X = 1, 
    Dot11AuthAndMangementSuite_PSK = 2, 
    Dot11AuthAndMangementSuite_Extended = 3
} onvif_Dot11AuthAndMangementSuite;

typedef enum 
{
    Dot11Cipher_CCMP = 0, 
    Dot11Cipher_TKIP = 1, 
    Dot11Cipher_Any = 2, 
    Dot11Cipher_Extended = 3
} onvif_Dot11Cipher;

typedef enum  
{
    Dot11SignalStrength_None = 0, 
    Dot11SignalStrength_VeryBad = 1, 
    Dot11SignalStrength_Bad = 2, 
    Dot11SignalStrength_Good = 3, 
    Dot11SignalStrength_VeryGood = 4, 
    Dot11SignalStrength_Extended = 5
} onvif_Dot11SignalStrength;

typedef enum 
{
    Dot11StationMode_Ad_hoc = 0, 
    Dot11StationMode_Infrastructure = 1, 
    Dot11StationMode_Extended = 2
} onvif_Dot11StationMode;

typedef enum 
{
    Dot11SecurityMode_None = 0, 
    Dot11SecurityMode_WEP = 1, 
    Dot11SecurityMode_PSK = 2, 
    Dot11SecurityMode_Dot1X = 3, 
    Dot11SecurityMode_Extended = 4
} onvif_Dot11SecurityMode;

typedef enum 
{
    ReceiverMode_AutoConnect = 0,                       // The receiver connects on demand, as required by consumers of the media streams
    ReceiverMode_AlwaysConnect = 1,                     // The receiver attempts to maintain a persistent connection to the configured endpoint
    ReceiverMode_NeverConnect = 2,                      // The receiver does not attempt to connect
    ReceiverMode_Unknown = 3                            // This case should never happen
} onvif_ReceiverMode;

typedef enum 
{
    ReceiverState_NotConnected = 0,                     // The receiver is not connected
    ReceiverState_Connecting = 1,                       // The receiver is attempting to connect
    ReceiverState_Connected = 2,                        // The receiver is connected
    ReceiverState_Unknown = 3                           // This case should never happen
} onvif_ReceiverState;


/***************************************************************************************/
//////	add by xieqingpu
typedef struct
{
	onvif_UserPalette		UserPalette;		//色板
	onvif_WideDynamic		WideDynamic;		//宽动态
	onvif_OrgData		OrgData;			//数据源
	unsigned int		Actime;			//自动校正时间
} ThermalSettings1;

typedef struct
{
	float		Emissivity;			//发射率
	float		Distance;			//距离
	float		Humidity;		//湿度
	float		Correction;    //修正
	float		Reflection;		//反射温度
	float		Amb;		//环境温度 
} ThermalSettings2;

typedef struct
{
	uint32	ThermalSet_ext1Flag 			: 1; 				   
	uint32	ThermalSet_ext2Flag 			: 1; 				 
    uint32  Reserved            : 30;                   

	ThermalSettings1	ThermalSet1;
	ThermalSettings2	ThermalSet2;
} onvif_ThermalSettings;


typedef struct
{
	int focal;
    float lens;
    float distance;

    int dula_model; 
    int16_t x;
    int16_t y;
    float scale;
} onvif_DulaInformation;
//////

typedef struct 
{
    float 	Min;									    // required
    float 	Max;									    // required
} onvif_FloatRange;

typedef struct
{
	float	x;										    // required
	float	y;										    // required
} onvif_Vector;

typedef struct 
{
	int     sizeItems;
	int     Items[10];	                                // optional
} onvif_IntList;

typedef struct 
{
	int     sizeItems;
	float   Items[10];	                                // optional
} onvif_FloatList;

typedef struct 
{
	int     sizeItems;
	
	onvif_ParityBit Items[10];	                        // optional
} onvif_ParityBitList;

/* device capabilities */
typedef struct
{
	// network capabilities
	uint32	IPFilter 			: 1; 				    // Indicates support for IP filtering
	uint32	ZeroConfiguration 	: 1; 				    // Indicates support for zeroconf
	uint32	IPVersion6			: 1; 				    // Indicates support for IPv6
	uint32 	DynDNS 				: 1; 				    // Indicates support for dynamic DNS configuration
	uint32  Dot11Configuration  : 1; 				    // Indicates support for IEEE 802.11 configuration
	uint32  HostnameFromDHCP    : 1; 				    // Indicates support for retrieval of hostname from DHCP
	uint32  DHCPv6              : 1; 				    // Indicates support for Stateful IPv6 DHCP

	// system capabilities
	uint32 	DiscoveryResolve 	: 1; 				    // Indicates support for WS Discovery resolve requests
	uint32 	DiscoveryBye 		: 1; 				    // Indicates support for WS-Discovery Bye
	uint32 	RemoteDiscovery 	: 1; 				    // Indicates support for remote discovery
	uint32 	SystemBackup 		: 1; 				    // Indicates support for system backup through MTOM
	uint32 	SystemLogging 		: 1; 				    // Indicates support for retrieval of system logging through MTOM
	uint32 	FirmwareUpgrade 	: 1; 				    // Indicates support for firmware upgrade through MTOM
	uint32  HttpFirmwareUpgrade : 1; 				    // Indicates support for system backup through MTOM
	uint32  HttpSystemBackup    : 1; 				    // Indicates support for system backup through HTTP
	uint32  HttpSystemLogging   : 1; 				    // Indicates support for retrieval of system logging through HTTP
	uint32  HttpSupportInformation : 1; 			    // Indicates support for retrieving support information through HTTP
    uint32  StorageConfiguration: 1;                    // Indicates support for storage configuration interfaces

    // scurity capabilities
    uint32  TLS10               : 1; 				    // Indicates support for TLS 1.0
    uint32  TLS11               : 1; 				    // Indicates support for TLS 1.1
    uint32  TLS12               : 1; 				    // Indicates support for TLS 1.2
    uint32  OnboardKeyGeneration: 1; 				    // Indicates support for onboard key generation
    uint32  AccessPolicyConfig  : 1; 				    // Indicates support for access policy configuration
    uint32  DefaultAccessPolicy : 1; 				    // Indicates support for the ONVIF default access policy
    uint32  Dot1X               : 1; 				    // Indicates support for IEEE 802.1X configuration
    uint32  RemoteUserHandling  : 1; 				    // Indicates support for remote user configuration. Used when accessing another device
    uint32  X509Token           : 1; 				    // Indicates support for WS-Security X.509 token
    uint32  SAMLToken           : 1; 				    // Indicates support for WS-Security SAML token
    uint32  KerberosToken       : 1; 				    // Indicates support for WS-Security Kerberos token
    uint32  UsernameToken       : 1; 				    // Indicates support for WS-Security Username token
    uint32  HttpDigest          : 1; 				    // Indicates support for WS over HTTP digest authenticated communication layer
    uint32  RELToken            : 1; 				    // Indicates support for WS-Security REL token

    uint32  Auxiliary           : 1;                    // 
    uint32  Reserved            : 31;                   
    
    // IO 
    int     InputConnectors;	                        // optional, Number of input connectors
	int     RelayOutputs;	                            // optional, Number of relay outputs
	
	int     Dot1XConfigurations;    				    // Indicates the maximum number of Dot1X configurations supported by the device
	int     NTP;                    				    // Maximum number of NTP servers supported by the devices SetNTP command
	
	int     SupportedEAPMethods;    				    // EAP Methods supported by the device. 
	                                				    // The int values refer to the <a href="http://www.iana.org/assignments/eap-numbers/eap-numbers.xhtml">IANA EAP Registry</a>
	int     MaxUsers;               				    // The maximum number of users that the device supports
    int     MaxUserNameLength;                          // Maximum number of characters supported for the username by CreateUsers
    int     MaxPasswordLength;                          // Maximum number of characters supported for the password by CreateUsers and SetUser

    int     MaxStorageConfigurations;                   // Indicates maximum number of storage configurations supported
    int     GeoLocationEntries;                         // If present signals support for geo location. The value signals the supported number of entries
    char    AutoGeo[256];                               // List of supported automatic GeoLocation adjustment supported by the device. Valid items are defined by tds:AutoGeoMode
                                                        //  Location:Automatic adjustment of the device location
                                                        //  Heading:Automatic adjustment of the device orientation relative to the compass also called yaw
                                                        //  Leveling:Automatic adjustment of the deviation from the horizon also called pitch and roll
    char    StorageTypesSupported[256];                 // Enumerates the supported StorageTypes, see tds:StorageType:
                                                        //  NFS,CIFS,CDMI,FTP
    
	// misc capabilities
    int     sizeAuxiliaryCommands;
    char    AuxiliaryCommands[10][64];                  // Lists of commands supported by SendAuxiliaryCommand
    
    char 	XAddr[XADDR_LEN];
} onvif_DevicesCapabilities;

/* media capabilities */
typedef struct
{
    uint32  SnapshotUri         : 1; 				    // Indicates if GetSnapshotUri is supported
    uint32  Rotation            : 1; 				    // Indicates whether or not Rotation feature is supported
    uint32  VideoSourceMode     : 1; 				    // Indicates the support for changing video source mode
    uint32  OSD                 : 1; 				    // Indicates if OSD is supported
    uint32  TemporaryOSDText    : 1; 				    // Indicates if TemporaryOSDText is supported
    uint32  EXICompression      : 1;                    // Indicates the support for the Efficient XML Interchange (EXI) binary XML format
	uint32	RTPMulticast 		: 1; 				    // Indicates support for RTP multicast
	uint32	RTP_TCP				: 1; 				    // Indicates support for RTP over TCP
	uint32	RTP_RTSP_TCP		: 1; 				    // Indicates support for RTP/RTSP/TCP
	uint32  NonAggregateControl : 1; 				    // Indicates support for non aggregate RTSP control
	uint32  NoRTSPStreaming     : 1; 				    // Indicates the device does not support live media streaming via RTSP
	uint32  support				: 1; 				    // Indication if the device supports media service
	uint32	reserved			: 20;

	int     MaximumNumberOfProfiles; 				    // Maximum number of profiles supported
	
    char 	XAddr[XADDR_LEN];
} onvif_MediaCapabilities;

/* PTZ capabilities */
typedef struct
{
    uint32  EFlip               : 1;	 			    // Indicates whether or not EFlip is supported
    uint32  Reverse             : 1; 				    // Indicates whether or not reversing of PT control direction is supported
    uint32  GetCompatibleConfigurations : 1; 		    // Indicates support for the GetCompatibleConfigurations command    
    uint32  MoveStatus          : 1;                    // Indicates that the PTZVector includes MoveStatus information
    uint32  StatusPosition      : 1;                    // Indicates that the PTZVector includes Position information
    uint32  support				: 1; 				    // Indication if the device supports ptz service
	uint32	reserved 			: 26;
	
    char 	XAddr[XADDR_LEN];
} onvif_PTZCapabilities;

/* event capabilities */
typedef struct
{
	uint32	WSSubscriptionPolicySupport	: 1; 		    // Indicates that the WS Subscription policy is supported
	uint32	WSPullPointSupport 	: 1; 				    // Indicates that the WS Pull Point is supported
	uint32	WSPausableSubscriptionManagerInterfaceSupport : 1; // Indicates that the WS Pausable Subscription Manager Interface is supported
	uint32  PersistentNotificationStorage : 1; 		    // Indication if the device supports persistent notification storage
	uint32  support				: 1; 				    // Indication if the device supports events service
	uint32	reserved 			: 27;

	int     MaxNotificationProducers; 				    // Maximum number of supported notification producers as defined by WS-BaseNotification
	int     MaxPullPoints;            				    // Maximum supported number of notification pull points
	
    char 	XAddr[XADDR_LEN];
} onvif_EventCapabilities;

/* image capabilities */
typedef struct
{
    uint32  ImageStabilization  : 1; 				    // Indicates whether or not Image Stabilization feature is supported
    uint32  Presets             : 1;                    // Indicates whether or not Presets feature is supported
	uint32  support				: 1; 				    // Indication if the device supports image service
	uint32	reserved 			: 29;
	
    char 	XAddr[XADDR_LEN];
} onvif_ImagingCapabilities;

/* analytics capabilities*/
typedef struct
{
	uint32  RuleSupport			: 1;				    // Indication that the device supports the rules interface and the rules syntax
	uint32  AnalyticsModuleSupport : 1;				    // Indication that the device supports the scene analytics module interface
	uint32  CellBasedSceneDescriptionSupported : 1;	    // Indication that the device produces the cell based scene description
	uint32  RuleOptionsSupported: 1;	                // Indication that the device produces the cell based scene description
	uint32  AnalyticsModuleOptionsSupported : 1;	    // Indication that the device produces the cell based scene description
	uint32  support				: 1; 				    // Indication if the device supports Analytics service
	uint32	reserved 			: 26;

	char 	XAddr[XADDR_LEN];
} onvif_AnalyticsCapabilities;

/* recording capabilities */
typedef struct
{
    uint32  ReceiverSource      : 1;
    uint32  MediaProfileSource  : 1;
    uint32  DynamicRecordings   : 1; 				    // Indication if the device supports dynamic creation and deletion of recordings
    uint32  DynamicTracks       : 1; 				    // Indication if the device supports dynamic creation and deletion of tracks
    uint32  Options             : 1; 				    // Indication if the device supports the GetRecordingOptions command
    uint32  MetadataRecording   : 1; 				    // Indication if the device supports recording metadata
    uint32  JPEG                : 1; 				    // Indication if supports JPEG encoding
    uint32  MPEG4               : 1; 				    // Indication if supports MPEG4 encoding
    uint32  H264                : 1; 				    // Indication if supports H264 encoding
    uint32  G711                : 1; 				    // Indication if supports G711 encoding
    uint32  G726                : 1; 				    // Indication if supports G726 encoding
    uint32  AAC                 : 1; 				    // Indication if supports AAC encoding
    uint32  support				: 1; 				    // Indication if the device supports recording service
	uint32	reserved 			: 19;

	uint32  MaxStringLength;
	float   MaxRate;            					    // Maximum supported bit rate for all tracks of a recording in kBit/s
	float   MaxTotalRate;       					    // Maximum supported bit rate for all recordings in kBit/s.
    int     MaxRecordings;      					    // Maximum number of recordings supported.
    int     MaxRecordingJobs;   					    // Maximum total number of supported recording jobs by the device
	char    SupportedExportFileFormats[100];            // 
	
	char 	XAddr[XADDR_LEN];
} onvif_RecordingCapabilities;

/* search capabilities */
typedef struct
{
    uint32  MetadataSearch      : 1;
    uint32  GeneralStartEvents  : 1; 				    // Indicates support for general virtual property events in the FindEvents method
    uint32  support				: 1; 				    // Indication if the device supports search service
	uint32	reserved 			: 29;
	
	char 	XAddr[XADDR_LEN];
} onvif_SearchCapabilities;

/* replay capabilities */
typedef struct
{
    uint32  ReversePlayback     : 1; 				    // Indicator that the Device supports reverse playback as defined in the ONVIF Streaming Specification
    uint32  RTP_RTSP_TCP        : 1; 				    // Indicates support for RTP/RTSP/TCP
    uint32  support				: 1; 				    // Indication if the device supports replay service
	uint32	reserved 			: 29;

	onvif_FloatRange	SessionTimeoutRange; 		    // The minimum and maximum valid values supported as session timeout in seconds

	char    RTSPWebSocketUri[256];                      // 
	
	char 	XAddr[XADDR_LEN];
} onvif_ReplayCapabilities;

/* accesscontrol capabilities */
typedef struct
{
    uint32  support				: 1; 				    // Indication if the device supports accesscontrol service
    uint32  ClientSuppliedTokenSupported : 1;           // Indicates that the client is allowed to supply the token when creating access points and areas
                                                        //  To enable the use of the commands SetAccessPoint and SetArea, the value must be set to true
	uint32	reserved 			: 30;
	
    int     MaxLimit;                                   // The maximum number of entries returned by a single GetList request. 
                                                        //  The device shall never return more than this number of entities in a single response
    int     MaxAccessPoints;                            // Indicates the maximum number of access points supported by the device
    int     MaxAreas;                                   // Indicates the maximum number of areas supported by the device
    
	
	char 	XAddr[XADDR_LEN];
} onvif_AccessControlCapabilities;

/* doorcontrol capabilities */
typedef struct
{
    uint32  support				: 1; 				    // Indication if the device supports doorcontrol service
    uint32  ClientSuppliedTokenSupported : 1;           // Indicates that the client is allowed to supply the token when creating doors.
                                                        //  To enable the use of the command SetDoor, the value must be set to true
	uint32	reserved 			: 30;
	
    int     MaxLimit;                                   // The maximum number of entries returned by a single Get<Entity>List or Get<Entity> request. 
                                                        //  The device shall never return more than this number of entities in a single response
    int     MaxDoors;                                   // Indicates the maximum number of doors supported by the device    
	
	char 	XAddr[XADDR_LEN];
} onvif_DoorControlCapabilities;

typedef struct 
{
	uint32  VideoSourcesFlag	: 1; 				    // Indicates whether the field VideoSources is valid
	uint32  VideoOutputsFlag	: 1; 				    // Indicates whether the field VideoSources is valid
	uint32  AudioSourcesFlag	: 1; 				    // Indicates whether the field VideoSources is valid
	uint32  AudioOutputsFlag	: 1; 				    // Indicates whether the field VideoSources is valid
	uint32  RelayOutputsFlag	: 1; 				    // Indicates whether the field VideoSources is valid
	uint32  SerialPortsFlag		: 1; 				    // Indicates whether the field VideoSources is valid
	uint32  DigitalInputsFlag	: 1; 				    // Indicates whether the field VideoSources is valid
	uint32  DigitalInputOptionsFlag	: 1; 				// Indicates whether the field VideoSources is valid
	uint32  support				: 1; 				    // Indication if the device supports deviceIO service
	uint32	reserved 			: 23;
	
	int 	VideoSources;								// optional, Number of video sources (defaults to none)
	int 	VideoOutputs;								// optional, Number of video outputs (defaults to none)
	int 	AudioSources;								// optional, Number of audio sources (defaults to none)
	int 	AudioOutputs;								// optional, Number of audio outputs (defaults to none)
	int 	RelayOutputs;								// optional, Number of relay outputs (defaults to none)
	int 	SerialPorts;								// optional, Number of serial ports (defaults to none)
	int 	DigitalInputs;								// optional, Number of digital inputs (defaults to none)
	BOOL 	DigitalInputOptions;						// optional, Indicates support for DigitalInput configuration of the idle state (defaults to false)

	char 	XAddr[XADDR_LEN];
} onvif_DeviceIOCapabilities;

typedef struct 
{
    uint32  MaximumNumberOfProfilesFlag : 1;            // Indicates whether the field MaximumNumberOfProfiles is valid
    uint32  ConfigurationsSupportedFlag : 1;            // Indicates whether the field ConfigurationsSupported is valid
    uint32  Reserved                    : 30;
    
	int     MaximumNumberOfProfiles;	                // optional, Maximum number of profiles supported
	char    ConfigurationsSupported[256];               // optional, Enumerates the configurations supported
} onvif_ProfileCapabilities;

typedef struct 
{
    uint32  RTSPStreamingFlag               : 1;        // Indicates whether the field RTSPStreaming is valid
    uint32  RTPMulticastFlag                : 1;        // Indicates whether the field RTPMulticast is valid
    uint32  RTP_USCORERTSP_USCORETCPFlag    : 1;        // Indicates whether the field RTP_USCORERTSP_USCORETCP is valid
    uint32  NonAggregateControlFlag         : 1;        // Indicates whether the field NonAggregateControl is valid
    uint32  RTSPWebSocketUriFlag            : 1;        // Indicates whether the field RTSPWebSocketUri is valid
    uint32  Reserved                        : 27;
    
	BOOL    RTSPStreaming;	                            // optional, Indicates support for live media streaming via RTSP
	BOOL    RTPMulticast;	                            // optional, Indicates support for RTP multicast
	BOOL    RTP_USCORERTSP_USCORETCP;	                // optional, Indicates support for RTP/RTSP/TCP
	BOOL    NonAggregateControl;	                    // optional, Indicates support for non aggregate RTSP control
    char    RTSPWebSocketUri[256];	                    // optional, If streaming over websocket supported, RTSP websocket URI is provided. 
                                                        //  The scheme and IP part shall match the one used in the request (e.g. the GetServices request)
    BOOL    AutoStartMulticast;                         // optional,                                                         
} onvif_StreamingCapabilities;

typedef struct 
{
	BOOL    TKIP;	                                    // required
	BOOL    ScanAvailableNetworks;	                    // required
	BOOL    MultipleConfiguration;	                    // required
	BOOL    AdHocStationMode;	                        // required
	BOOL    WEP;	                                    // required
} onvif_Dot11Capabilities;

typedef struct 
{
    uint32  SnapshotUriFlag     : 1;                    // Indicates whether the field SnapshotUri is valid
    uint32  RotationFlag        : 1;                    // Indicates whether the field Rotation is valid
    uint32  VideoSourceModeFlag : 1;                    // Indicates whether the field VideoSourceMode is valid
    uint32  OSDFlag             : 1;                    // Indicates whether the field OSD is valid
    uint32  TemporaryOSDTextFlag: 1;                    // Indicates whether the field TemporaryOSDText is valid
    uint32  MaskFlag            : 1;                    // Indicates whether the field Mask is valid
    uint32  SourceMaskFlag      : 1;                    // Indicates whether the field SourceMask is valid
    uint32  support				: 1; 				    // Indication if the device supports media service2
    uint32  Reserved            : 24;
    
	onvif_ProfileCapabilities   ProfileCapabilities;	// required, Media profile capabilities
	onvif_StreamingCapabilities StreamingCapabilities;	// required, Streaming capabilities

	BOOL    SnapshotUri;	                            // optional, Indicates if GetSnapshotUri is supported
	BOOL    Rotation;	                                // optional, Indicates whether or not Rotation feature is supported
	BOOL    VideoSourceMode;	                        // optional, Indicates the support for changing video source mode
	BOOL    OSD;	                                    // optional, Indicates if OSD is supported
	BOOL    TemporaryOSDText;                           // optional, Indicates if TemporaryOSDText is supported
    BOOL    Mask;                                       // optional, Indicates if Masking is supported, Indicates support for mask configuration
    BOOL    SourceMask;                                 // optional, Indicates if SourceMask is supported, 
                                                        //  Indicates that privacy masks are only supported at the video source level 
                                                        //  and not the video source configuration level. If this is true any addition, 
                                                        //  deletion or change of a privacy mask done for one video source configuration 
                                                        //  will automatically be applied by the device to a corresponding privacy mask 
                                                        //  for all other video source configuration associated with the same video source.

	char 	XAddr[XADDR_LEN];
} onvif_MediaCapabilities2;

typedef struct 
{
    uint32  Radiometry  : 1;                            // Indicates whether or not radiometric thermal measurements are supported by the thermal devic
    uint32  support     : 1;                            // Indication if the device supports thermal service
    uint32  Reserved    : 30;

    char 	XAddr[XADDR_LEN];
} onvif_ThermalCapabilities;

typedef struct
{
	int     sizeSupportedExemptionType;	                // sequence of elements <SupportedExemptionType>
	char    SupportedExemptionType[10][32];	            // optional, A list of exemptions that the device supports. Supported exemptions starting with the
								                        //  prefix pt: are reserved to define PACS specific exemption types and these reserved
								                        //  exemption types shall all share "pt:<Name>" syntax
								                        //  pt:ExemptFromAuthentication Supports ExemptedFromAuthentication
} onvif_CredentialCapabilitiesExtension;

typedef struct
{
    uint32  ExtensionFlag               : 1;
    uint32  CredentialValiditySupported : 1;            // required, Indicates that the device supports credential validity
    uint32  CredentialAccessProfileValiditySupported: 1;// required, Indicates that the device supports validity on the association 
                                                        //  between a credential and an access profile    
    uint32  ValiditySupportsTimeValue   : 1;	        // required, Indicates that the device supports both date and time value for validity. 
                                                        //  If set to false, then the time value is ignored
    uint32  ResetAntipassbackSupported  : 1;	        // required, Indicates the device supports resetting of anti-passback violations 
                                                        //  and notifying on anti-passback violations
    uint32  ClientSuppliedTokenSupported : 1;           // Indicates that the client is allowed to supply the token when creating credentials.
                                                        //  To enable the use of the command SetCredential, the value must be set to true                                                        
    uint32  support				        : 1; 		    // Indication if the device supports credential service
    uint32  Reserved                    : 25;
    
	int     sizeSupportedIdentifierType;	            // sequence of elements <SupportedIdentifierType>
	char    SupportedIdentifierType[10][32];	        // required, A list of identifier types that the device supports. Supported identifiers starting with
								                        //  the prefix pt: are reserved to define PACS specific identifier types and these reserved
								                        //  identifier types shall all share the "pt:<Name>" syntax
								                        //  o pt:Card Supports Card identifier type
                                                        //  o pt:PIN Supports PIN identifier type
                                                        //  o pt:Fingerprint Supports Fingerprint biometric identifier type
                                                        //  o pt:Face Supports Face biometric identifier type
                                                        //  o pt:Iris Supports Iris biometric identifier type
                                                        //  o pt:Vein Supports Vein biometric identifier type
	uint32  MaxLimit;	                                // required, The maximum number of entries returned by a single request.
							                            //  The device shall never return more than this number of entities in a single response
	uint32  MaxCredentials;	                            // required, The maximum number of credential supported by the device
	uint32  MaxAccessProfilesPerCredential;	            // required, The maximum number of access profiles for a credential

    char    DefaultCredentialSuspensionDuration[20];    // The default time period that the credential will temporary be suspended (e.g. by using the wrong PIN a predetermined number of times).
                                                        //  The time period is defined as an [ISO 8601] duration string (e.g. PT5M).
    
	onvif_CredentialCapabilitiesExtension   Extension;	// optional

	char 	XAddr[XADDR_LEN];
}  onvif_CredentialCapabilities;

typedef struct 
{
    uint32  MultipleSchedulesPerAccessPointSupported: 1;// required, Indicates whether or not several access policies can refer to the same access point in
                                                        //  an access profile
    uint32  ClientSuppliedTokenSupported : 1;           // Indicates that the client is allowed to supply the token when creating access profiles. 
                                                        //  To enable the use of the command SetAccessProfile, the value must be set to true                                        
    uint32  support				         : 1; 		    // Indication if the device supports access rules service
    uint32  Reserved                     : 29;
    
	uint32  MaxLimit;	                                // required, The maximum number of entries returned by a single Get<Entity>List or Get<Entity>
                                                        //  request. The device shall never return more than this number of entities in a single response
	uint32  MaxAccessProfiles;	                        // required, Indicates the maximum number of access profiles supported by the device
	uint32  MaxAccessPoliciesPerAccessProfile;	        // required, Indicates the maximum number of access policies per access profile supported by the device

	char 	XAddr[XADDR_LEN];
} onvif_AccessRulesCapabilities;

typedef struct
{
    uint32  ExtendedRecurrenceSupported  : 1;           // required, If this capability is supported, then all iCalendar recurrence types shall be supported by the device
    uint32  SpecialDaysSupported         : 1;           // required, If this capability is supported, then the device shall support special days
    uint32  StateReportingSupported      : 1;           // required, If this capability is set to true, the device shall implement the GetScheduleState command, 
                                                        //  and shall notify subscribing clients whenever schedules become active or inactive
    uint32  ClientSuppliedTokenSupported : 1;           // Indicates that the client is allowed to supply the token when creating schedules and special day groups.
                                                        //  To enable the use of the commands SetSchedule and SetSpecialDayGroup, the value must be set to true
    uint32  support				         : 1; 		    // Indication if the device supports schedule service
    uint32  Reserved                     : 27;
    
    uint32  MaxLimit;	                                // required, 
	uint32  MaxSchedules;	                            // required, 
	uint32  MaxTimePeriodsPerDay;	                    // required, 
	uint32  MaxSpecialDayGroups;	                    // required, 
	uint32  MaxDaysInSpecialDayGroup;	                // required, 
	uint32  MaxSpecialDaysSchedules;	                // required, 
	
    char 	XAddr[XADDR_LEN];
} onvif_ScheduleCapabilities;

typedef struct 
{
	uint32  RTP_USCOREMulticast         : 1;	        // required, Indicates that the device can receive RTP multicast streams
	uint32  RTP_USCORETCP               : 1;	        // required, Indicates that the device can receive RTP/TCP streams
	uint32  RTP_USCORERTSP_USCORETCP    : 1;	        // required, Indicates that the device can receive RTP/RTSP/TCP streams
	uint32  support				        : 1; 		    // Indication if the device supports receiver service
    uint32  Reserved                    : 28;

	int     SupportedReceivers;	                        // required, The maximum number of receivers supported by the device
	int     MaximumRTSPURILength;	                    // required, The maximum allowed length for RTSP URIs (Minimum and default value is 128 octet)

	char 	XAddr[XADDR_LEN];
} onvif_ReceiverCapabilities;

typedef struct
{
	onvif_DevicesCapabilities	    device;     	    // The capabilities for the device service is returned in the Capabilities element
    onvif_MediaCapabilities		    media;      	    // The capabilities for the media service is returned in the Capabilities element    
#ifdef MEDIA2_SUPPORT
    onvif_MediaCapabilities2        media2;             // The capabilities for the media service2 is returned in the Capabilities element
#endif
	onvif_EventCapabilities		    events;     	    // The capabilities for the event service is returned in the Capabilities element
	onvif_ImagingCapabilities	    image;      	    // The capabilities for the imaging service is returned in the Capabilities element

#ifdef PTZ_SUPPORT
    onvif_PTZCapabilities		    ptz;        	    // The capabilities for the PTZ service is returned in the Capabilities element
#endif

#ifdef VIDEO_ANALYTICS
	onvif_AnalyticsCapabilities	    analytics;		    // The capabilities for the analytics service is returned in the Capabilities element
#endif

#ifdef PROFILE_G_SUPPORT
	onvif_RecordingCapabilities	    recording;  	    // The capabilities for the recording service is returned in the Capabilities element
	onvif_SearchCapabilities	    search;     	    // The capabilities for the search service is returned in the Capabilities element
	onvif_ReplayCapabilities	    replay;     	    // The capabilities for the replay service is returned in the Capabilities element
#endif	

#ifdef PROFILE_C_SUPPORT
    onvif_AccessControlCapabilities accesscontrol;      // The capabilities for the accesscontrol service is returned in the Capabilities element
    onvif_DoorControlCapabilities   doorcontrol;        // The capabilities for the doorcontrol service is returned in the Capabilities element
#endif

#ifdef DEVICEIO_SUPPORT
	onvif_DeviceIOCapabilities	    deviceIO;		    // The capabilities for the deviceIO service is returned in the Capabilities element
#endif

    onvif_Dot11Capabilities         dot11;              // The capabilities for the dot11

#ifdef THERMAL_SUPPORT
    onvif_ThermalCapabilities       thermal;            // The capabilities for the thermal service is returned in the Capabilities element
#endif

#ifdef CREDENTIAL_SUPPORT
    onvif_CredentialCapabilities    credential;         // The capabilities for the credential service is returned in the Capabilities element
#endif

#ifdef ACCESS_RULES
    onvif_AccessRulesCapabilities   accessrules;        // The capabilities for the access rules service is returned in the Capabilities element
#endif

#ifdef SCHEDULE_SUPPORT
    onvif_ScheduleCapabilities      schedule;           // The capabilities for the schedule service is returned in the Capabilities element
#endif

#ifdef RECEIVER_SUPPORT
    onvif_ReceiverCapabilities      receiver;           // The capabilities for the receiver service is returned in the Capabilities element
#endif
} onvif_Capabilities;

typedef struct
{
	char 	Manufacturer[64];						    // required, The manufactor of the device
	char 	Model[64];								    // required, The device model
	char 	FirmwareVersion[64];					    // required, The firmware version in the device
	char 	SerialNumber[64];						    // required, The serial number of the device
	char 	HardwareId[64];							    // required, The hardware ID of the device
} onvif_DeviceInformation;

typedef struct 
{
	int 	Width;									    // required
	int 	Height;									    // required
} onvif_VideoResolution;

typedef struct
{
	int		Min;									    // required								
	int 	Max;									    // required
} onvif_IntRange;

typedef struct
{
	int     x;	                                        // required 
	int     y;	                                        // required 
	int     width;	                                    // required 
	int     height;	                                    // required 
} onvif_IntRectangle;

typedef struct
{
	float   bottom;	                                    // required
	float   top;	                                    // required
	float   right;	                                    // required
	float   left;	                                    // required
} onvif_Rectangle;

typedef struct
{
	uint32	LevelFlag	: 1;						    // Indicates whether the field Level is valid
	uint32  Reserved	: 31;
	
    onvif_BacklightCompensationMode	Mode;               // required, Backlight compensation mode (on/off)
    float   Level;                                      // optional, Optional level parameter (unit unspecified)
} onvif_BacklightCompensation;

typedef struct
{
	uint32	PriorityFlag		: 1;				    // Indicates whether the field Priority is valid
	uint32	MinExposureTimeFlag	: 1;				    // Indicates whether the field MinExposureTime is valid
	uint32	MaxExposureTimeFlag	: 1;				    // Indicates whether the field MaxExposureTime is valid
	uint32	MinGainFlag			: 1;				    // Indicates whether the field MinGain is valid
	uint32	MaxGainFlag			: 1;				    // Indicates whether the field MaxGain is valid
	uint32	MinIrisFlag			: 1;				    // Indicates whether the field MinIris is valid
	uint32	MaxIrisFlag			: 1;				    // Indicates whether the field MaxIris is valid
	uint32	ExposureTimeFlag	: 1;				    // Indicates whether the field ExposureTime is valid
	uint32	GainFlag			: 1;				    // Indicates whether the field Gain is valid
	uint32	IrisFlag			: 1;				    // Indicates whether the field Iris is valid
	uint32  Reserved			: 22;
	
    onvif_ExposureMode		Mode;                       // required, Auto - Enabled the exposure algorithm on the device; Manual - Disabled exposure algorithm on the device
    onvif_ExposurePriority 	Priority;                   // optional, The exposure priority mode (low noise/framerate)
    onvif_Rectangle         Window;                     // required, 
    
    float   MinExposureTime;	                        // optional, Minimum value of exposure time range allowed to be used by the algorithm
	float   MaxExposureTime;	                        // optional, Maximum value of exposure time range allowed to be used by the algorithm
	float   MinGain;	                                // optional, Minimum value of the sensor gain range that is allowed to be used by the algorithm
	float   MaxGain;	                                // optional, Maximum value of the sensor gain range that is allowed to be used by the algorithm
	float   MinIris;	                                // optional, Minimum value of the iris range allowed to be used by the algorithm
	float   MaxIris;	                                // optional, Maximum value of the iris range allowed to be used by the algorithm
	float   ExposureTime;	                            // optional, The fixed exposure time used by the image sensor
	float   Gain;	                                    // optional, The fixed gain used by the image sensor (dB)
	float   Iris;	                                    // optional, The fixed attenuation of input light affected by the iris (dB). 0dB maps to a fully opened iris
} onvif_Exposure;

typedef struct
{
	uint32	DefaultSpeedFlag	: 1;				    // Indicates whether the field DefaultSpeed is valid
	uint32	NearLimitFlag		: 1;				    // Indicates whether the field NearLimit is valid
	uint32	FarLimitFlag		: 1;				    // Indicates whether the field FarLimit is valid	
	uint32  Reserved			: 29;
	
    onvif_AutoFocusMode	AutoFocusMode;                  // required, Mode of auto fucus
    
    float   DefaultSpeed;	                            // optional, 
	float   NearLimit;	                                // optional, Parameter to set autofocus near limit (unit: meter)
	float   FarLimit;	                                // optional, Parameter to set autofocus far limit (unit: meter)
} onvif_FocusConfiguration;

typedef struct
{
	uint32	LevelFlag	: 1;						    // Indicates whether the field Level is valid
	uint32  Reserved	: 31;
	
    onvif_WideDynamicMode	Mode;                       // required, Wide dynamic range mode (on/off), 0-OFF, 1-ON
    float   Level;	                                    // optional, Optional level parameter (unit unspecified)
} onvif_WideDynamicRange; 

typedef struct
{
	uint32	CrGainFlag	: 1;						    // Indicates whether the field CrGain is valid
	uint32	CbGainFlag	: 1;						    // Indicates whether the field CbGain is valid
	uint32  Reserved	: 30;
	
    onvif_WhiteBalanceMode	Mode;	                    // required, 'AUTO' or 'MANUAL'
    
	float   CrGain;	                                    // optional, Rgain (unitless)
	float   CbGain;	                                    // optional, Bgain (unitless)
} onvif_WhiteBalance; 

typedef struct
{
	uint32	BacklightCompensationFlag	: 1;		    // Indicates whether the field BacklightCompensation is valid
	uint32	BrightnessFlag				: 1;		    // Indicates whether the field Brightness is valid
	uint32	ColorSaturationFlag			: 1;		    // Indicates whether the field ColorSaturation is valid
	uint32	ContrastFlag				: 1;		    // Indicates whether the field Contrast is valid
	uint32	ExposureFlag				: 1;		    // Indicates whether the field Exposure is valid
	uint32	FocusFlag					: 1;		    // Indicates whether the field Focus is valid
	uint32	IrCutFilterFlag				: 1;		    // Indicates whether the field IrCutFilter is valid
	uint32	SharpnessFlag				: 1;		    // Indicates whether the field Sharpness is valid
	uint32	WideDynamicRangeFlag		: 1;		    // Indicates whether the field WideDynamicRange is valid
	uint32	WhiteBalanceFlag			: 1;		    // Indicates whether the field WhiteBalance is valid
   ////extension根据项目扩展  add by xieqingpu
	uint32  ThermalSettings_extFlag				: 1;	
	uint32  DulaInformationFlag			:1;
	uint32  Reserved					: 20;
	
	onvif_BacklightCompensation BacklightCompensation;  // optional, Enabled/disabled BLC mode (on/off)
	float   Brightness;                                 // optional, Image brightness (unit unspecified)
	float   ColorSaturation;                            // optional, Color saturation of the image (unit unspecified)
	float   Contrast;                                   // optional, Contrast of the image (unit unspecified)
	onvif_Exposure Exposure;                            // optional, Exposure mode of the device
	onvif_FocusConfiguration 	Focus;                  // optional, Focus configuration
    onvif_IrCutFilterMode    	IrCutFilter;            // optional, Infrared Cutoff Filter settings	
	float   Sharpness;                                  // optional, Sharpness of the Video image
	onvif_WideDynamicRange 		WideDynamicRange;       // optional, WDR settings
	onvif_WhiteBalance 			WhiteBalance;           // optional, White balance settings	

   ////extension根据项目扩展  add by xieqingpu
	onvif_ThermalSettings		ThermalSettings;
	onvif_DulaInformation		DulaInfo;
} onvif_ImagingSettings;

typedef struct
{
    uint32	Mode_ON	    : 1;                		    // Indicates whether mode ON is valid
	uint32 	Mode_OFF	: 1;                		    // Indicates whether mode OFF is valid
	uint32  LevelFlag	: 1;						    // Indicates whether the field LevelFlag is valid
	uint32 	Reserved    : 29;
	
    onvif_FloatRange	Level;                   	    // optional, Level range of BacklightCompensation
} onvif_BacklightCompensationOptions;

typedef struct
{
    uint32	Mode_AUTO           : 1;        		    // Indicates whether mode AUTO is valid
	uint32	Mode_MANUAL         : 1;        		    // Indicates whether mode Manual is valid
    uint32	Priority_LowNoise   : 1;       	 		    // Indicates whether Priority LowNoise is valid
	uint32	Priority_FrameRate  : 1;        		    // Indicates whether Priority FrameRate is valid
	uint32	MinExposureTimeFlag	: 1;				    // Indicates whether the field MinExposureTime is valid
	uint32	MaxExposureTimeFlag	: 1;				    // Indicates whether the field MaxExposureTime is valid
	uint32	MinGainFlag			: 1;				    // Indicates whether the field MinGain is valid
	uint32	MaxGainFlag			: 1;				    // Indicates whether the field MaxGain is valid
	uint32	MinIrisFlag			: 1;				    // Indicates whether the field MinIris is valid
	uint32	MaxIrisFlag			: 1;				    // Indicates whether the field MaxIris is valid
	uint32	ExposureTimeFlag	: 1;				    // Indicates whether the field ExposureTime is valid
	uint32	GainFlag			: 1;				    // Indicates whether the field Gain is valid
	uint32	IrisFlag			: 1;				    // Indicates whether the field Iris is valid
	uint32	Reserved            : 19;

	onvif_FloatRange	MinExposureTime;        	    // optional, Valid range of the Minimum ExposureTime
    onvif_FloatRange	MaxExposureTime;          	    // optional, Valid range of the Maximum ExposureTime
    onvif_FloatRange	MinGain;                  	    // optional, Valid range of the Minimum Gain
    onvif_FloatRange	MaxGain;                 	    // optional, Valid range of the Maximum Gain
    onvif_FloatRange	MinIris;                 	    // optional, Valid range of the Minimum Iris
    onvif_FloatRange	MaxIris;                 	    // optional, Valid range of the Maximum Iris
    onvif_FloatRange	ExposureTime;            	    // optional, Valid range of the ExposureTime
    onvif_FloatRange	Gain;                   	    // optional, Valid range of the Gain
    onvif_FloatRange	Iris;                    	    // optional, Valid range of the Iris
} onvif_ExposureOptions;

typedef struct
{
    uint32	AutoFocusModes_AUTO	    : 1;    		    // Indicates whether mode aUTO is valid
	uint32 	AutoFocusModes_MANUAL   : 1;   	 		    // Indicates whether mode Manual is valid
	uint32	DefaultSpeedFlag		: 1;			    // Indicates whether the field DefaultSpeed is valid
	uint32	NearLimitFlag			: 1;			    // Indicates whether the field NearLimit is valid
	uint32	FarLimitFlag			: 1;			    // Indicates whether the field FarLimit is valid
	uint32 	Reserved                : 27;

	onvif_FloatRange	DefaultSpeed;          		    // optional, Valid range of DefaultSpeed
	onvif_FloatRange	NearLimit;              	    // optional, Valid range of NearLimit
	onvif_FloatRange	FarLimit;               	    // optional, Valid range of FarLimit
} onvif_FocusOptions;

typedef struct
{
    uint32 	Mode_ON	    : 1;                		    // Indicates whether mode ON is valid
	uint32 	Mode_OFF	: 1;                		    // Indicates whether mode OFF is valid
	uint32	LevelFlag	: 1;						    // Indicates whether the field Level is valid
	uint32 	Reserved    : 29;
	
    onvif_FloatRange	Level;                 		    // optional, Valid range of Level
} onvif_WideDynamicRangeOptions;

typedef struct
{
    uint32	Mode_AUTO 	: 1;        				    // Indicates whether mode AUDO is valid
	uint32	Mode_MANUAL	: 1;        				    // Indicates whether mode Manual is valid
	uint32	YrGainFlag	: 1;						    // Indicates whether the field CrGain is valid
	uint32	YbGainFlag	: 1;						    // Indicates whether the field CbGain is valid
	uint32 	Reserved	: 28;

	onvif_FloatRange	YrGain;                 	    // optional, Valid range of YrGain
	onvif_FloatRange	YbGain;                  	    // optional, Valid range of YbGain
} onvif_WhiteBalanceOptions;

typedef struct
{
	uint32	IrCutFilterMode_ON	    	: 1; 		    // Indicates whether IrCutFilter mode ON is valid
	uint32 	IrCutFilterMode_OFF			: 1; 		    // Indicates whether IrCutFilter mode OFF is valid
	uint32 	IrCutFilterMode_AUTO		: 1; 		    // Indicates whether IrCutFilter mode AUTO is valid
	uint32	BacklightCompensationFlag	: 1;		    // Indicates whether the field BacklightCompensation is valid	
	uint32	BrightnessFlag				: 1;		    // Indicates whether the field Brightness is valid
	uint32  ColorSaturationFlag			: 1;		    // Indicates whether the field ColorSaturation is valid
	uint32  ContrastFlag				: 1;		    // Indicates whether the field Contrast is valid
	uint32  ExposureFlag				: 1;		    // Indicates whether the field Exposure is valid
	uint32  FocusFlag					: 1;		    // Indicates whether the field Focus is valid
	uint32  SharpnessFlag				: 1;		    // Indicates whether the field Sharpness is valid
	uint32  WideDynamicRangeFlag		: 1;		    // Indicates whether the field WideDynamicRange is valid
	uint32  WhiteBalanceFlag			: 1;		    // Indicates whether the field WhiteBalance is valid
	uint32 	Reserved    				: 20;
	
    onvif_BacklightCompensationOptions  BacklightCompensation;  // optional, Valid range of Backlight Compensation
    
    onvif_FloatRange      	Brightness;          	    // optional, Valid range of Brightness
    onvif_FloatRange      	ColorSaturation;    	    // optional, alid range of Color Saturation
    onvif_FloatRange      	Contrast;            	    // optional, Valid range of Contrast

    onvif_ExposureOptions   Exposure;           	    // optional, Valid range of Exposure    
    onvif_FocusOptions      Focus;              	    // optional, Valid range of Focus

    onvif_FloatRange      	Sharpness;           	    // optional, Valid range of Sharpness
    
    onvif_WideDynamicRangeOptions   WideDynamicRange;   // optional, Valid range of WideDynamicRange
    onvif_WhiteBalanceOptions		WhiteBalance;	    // optional, Valid range of WhiteBalance
} onvif_ImagingOptions;

typedef struct 
{
	uint32	ErrorFlag	: 1; 						    // Indicates whether the field Error is valid
	uint32	Reserved 	: 31;
	
	float 	Position;								    // required, Status of focus position
	onvif_MoveStatus	MoveStatus;					    // required, Status of focus MoveStatus
	char 	Error[100];								    // optional, Error status of focus
} onvif_FocusStatus;

typedef struct 
{
	uint32	FocusStatusFlag	: 1; 					    // Indicates whether the field FocusStatus is valid
	uint32	Reserved 		: 31;
	
	onvif_FocusStatus	FocusStatus;				    // optional, Status of focus
} onvif_ImagingStatus;

typedef struct 
{
    uint32  SpeedFlag   : 1;
    uint32  Reserved    : 31;
    
	onvif_FloatRange    Position;	                    // required, Valid ranges of the position
	onvif_FloatRange    Speed;	                        // optional, Valid ranges of the speed
} onvif_AbsoluteFocusOptions;

typedef struct 
{
    uint32  SpeedFlag   : 1;
    uint32  Reserved    : 31;
    
	onvif_FloatRange    Distance;	                    // required, valid ranges of the distance
	onvif_FloatRange    Speed;	                        // optional, Valid ranges of the speed
} onvif_RelativeFocusOptions20;

typedef struct 
{
	onvif_FloatRange    Speed;	                        // required, Valid ranges of the speed
} onvif_ContinuousFocusOptions;

typedef struct 
{
    uint32  AbsoluteFlag    : 1;
    uint32  RelativeFlag    : 1;
    uint32  ContinuousFlag  : 1;
    uint32  Reserved        : 29;
    
	onvif_AbsoluteFocusOptions      Absolute;	        // optional, Valid ranges for the absolute control
	onvif_RelativeFocusOptions20    Relative;	        // optional, Valid ranges for the relative control
	onvif_ContinuousFocusOptions    Continuous;	        // optional, Valid ranges for the continuous control
} onvif_MoveOptions20;

typedef struct
{	
	uint32  PasswordFlag	: 1;		    		    // Indicates whether the field Password is valid
	uint32	Reserved		: 31;
	
	BOOL	fixed;										// used by onvif server

	char 	Username[ONVIF_NAME_LEN];				    // required 
	char 	Password[ONVIF_NAME_LEN];				    // optional
	
	onvif_UserLevel UserLevel;						    // required 
} onvif_User;

typedef struct 
{
    uint32  PasswordFlag	: 1;		    		    // Indicates whether the field Password is valid
	uint32	Reserved		: 31;
	
	char    Username[ONVIF_NAME_LEN];	                // required
	char    Password[ONVIF_NAME_LEN];	                // optional
	
	BOOL    UseDerivedPassword;	                        // required 
} onvif_RemoteUser;

typedef struct
{
	char    Address[100];	                            // required
	int     PrefixLength;	                            // required
} onvif_PrefixedIPAddress;

typedef struct
{
	onvif_IPAddressFilterType   Type;	                // required
	onvif_PrefixedIPAddress     IPv4Address[20];	    // optional
	onvif_PrefixedIPAddress     IPv6Address[20];	    // optional
} onvif_IPAddressFilter;

typedef struct
{
	uint32	ImagingSettingsFlag	: 1; 				    // Indicates whether the field ImagingSettings is valid
	uint32	Reserved 			: 31;
	
    char    token[ONVIF_TOKEN_LEN];	                    // required
	float   Framerate;	                                // required, Frame rate in frames per second
	
	onvif_VideoResolution   Resolution;	                // required, Horizontal and vertical resolution 
	onvif_ImagingSettings	ImagingSettings;		    // optional
} onvif_VideoSource;

typedef struct
{
	uint32  DescriptionFlag		: 1;				    // Indicates whether the field Description is valid
	uint32  Enabled				: 1;				    //optional, Indication of whether this mode is active. If active this value is true. In case of non-indication, it means as false. 
													    //	The value of true shall be had by only one video source mode		
	uint32 	Reboot				: 1;				    // required, After setting the mode if a device starts to reboot this value is true. If a device change the mode without rebooting this value is false.
													    //	If true, configured parameters may not be guaranteed by the device after rebooting													
	uint32  Reserved			: 29;													
	
	float 	MaxFramerate;							    // required, Max frame rate in frames per second for this video source mode

	char    Encodings[32];                              // required, Indication which encodings are supported for this video source.
	char 	Description[128];						    // optional, Informative description of this video source mode. This field should be described in English
	char 	token[ONVIF_TOKEN_LEN];					    // required, Indicate token for video source mode

	onvif_VideoResolution	MaxResolution;			    // required, Max horizontal and vertical resolution for this video source mode
} onvif_VideoSourceMode;

typedef struct
{
    uint32  DegreeFlag  : 1;				            // Indicates whether the field Degree is valid
    uint32  Reserved    : 31;
    
	onvif_RotateMode    Mode;	                        // required, Parameter to enable/disable Rotation feature
	
	int     Degree;	                                    // optional, Optional parameter to configure how much degree of clockwise rotation of image  for On mode. Omitting this parameter for On mode means 180 degree rotation
} onvif_Rotate;

typedef struct 
{
    uint32  RotateFlag  : 1;				            // Indicates whether the field Rotate is valid
    uint32  Reserved    : 31;
    
	onvif_Rotate    Rotate;	                            // optional, Optional element to configure rotation of captured image
} onvif_VideoSourceConfigurationExtension;

typedef struct
{
    uint32  ExtensionFlag		: 1;				    // Indicates whether the field Extension is valid
    uint32  Reserved			: 31;
    
	char 	Name[ONVIF_NAME_LEN];					    // required, User readable name. Length up to 64 characters
	int 	UseCount;								    // required, Number of internal references currently using this configuration. This parameter is read-only and cannot be changed by a set request
	char 	token[ONVIF_TOKEN_LEN];					    // required, Token that uniquely refernces this configuration. Length up to 64 characters
	char 	SourceToken[ONVIF_TOKEN_LEN];			    // required, Reference to the physical input

	onvif_IntRectangle	Bounds;					        // required, Rectangle specifying the Video capturing area. The capturing area shall not be larger than the whole Video source area	
	onvif_VideoSourceConfigurationExtension Extension;  // optional
} onvif_VideoSourceConfiguration;


typedef struct
{
	onvif_IntRange  XRange;	                            // required
	onvif_IntRange  YRange;	                            // required
	onvif_IntRange  WidthRange;	                        // required
	onvif_IntRange  HeightRange;	                    // required
} onvif_IntRectangleRange;

typedef struct 
{
    uint32	RotateMode_OFF	: 1; 				        // Indicates whether the mode RotateMode_OFF is valid
    uint32	RotateMode_ON	: 1; 				        // Indicates whether the mode RotateMode_ON is valid
    uint32	RotateMode_AUTO : 1; 				        // Indicates whether the mode RotateMode_AUTO is valid
    uint32	DegreeListFlag  : 1; 				        // Indicates whether the field DegreeListFlag is valid
	uint32	Reserved 		: 28;

	int     sizeDegreeList;
	int     DegreeList[10];	                            // optional 
} onvif_RotateOptions;

typedef struct
{
    uint32	RotateFlag	: 1; 				            // Indicates whether the field Rotate is valid
	uint32	Reserved    : 31;
	
	onvif_RotateOptions     Rotate;	                    // optional 
} onvif_VideoSourceConfigurationOptionsExtension;

typedef struct
{
    uint32	ExtensionFlag	: 1; 				        // Indicates whether the field Extension is valid
	uint32	Reserved        : 31;
	
	onvif_IntRectangleRange BoundsRange;	            // required 

	int     sizeVideoSourceTokensAvailable;
	char    VideoSourceTokensAvailable[10][ONVIF_TOKEN_LEN];    // required

	onvif_VideoSourceConfigurationOptionsExtension  Extension;	// optional
} onvif_VideoSourceConfigurationOptions;

typedef struct
{
    uint32	ConstantBitRateFlag	: 1; 			        // Indicates whether the field ConstantBitRate is valid
	uint32	Reserved            : 31;
    
	int		FrameRateLimit;							    // required, Maximum output framerate in fps. If an EncodingInterval is provided the resulting encoded framerate will be reduced by the given factor
	int		EncodingInterval;						    // required, Interval at which images are encoded and transmitted. (A value of 1 means that every frame is encoded, a value of 2 means that every 2nd frame is encoded ...)
	int		BitrateLimit;							    // required, the maximum output bitrate in kbps
	BOOL    ConstantBitRate;                            // optional, Enforce constant bitrate
} onvif_VideoRateControl;

typedef struct
{
	int		GovLength;								    // required, Determines the interval in which the I-Frames will be coded. An entry of 1 indicates I-Frames are continuously generated. 
													    //	An entry of 2 indicates that every 2nd image is an I-Frame, and 3 only every 3rd frame, etc. The frames in between are coded as P or B Frames.
	onvif_Mpeg4Profile	Mpeg4Profile;				    // required, the Mpeg4 profile, either simple profile (SP) or advanced simple profile (ASP)													
} onvif_Mpeg4Configuration;

typedef struct
{
	int		GovLength;								    // required, Group of Video frames length. Determines typically the interval in which the I-Frames will be coded. An entry of 1 indicates I-Frames are continuously generated. 
													    //	An entry of 2 indicates that every 2nd image is an I-Frame, and 3 only every 3rd frame, etc. The frames in between are coded as P or B Frames
	onvif_H264Profile	H264Profile;				    // required, the H.264 profile, either baseline, main, extended or high
} onvif_H264Configuration;

typedef struct
{
	char	IPv4Address[32];						    // required, The multicast address
	int		Port;									    // required, The RTP mutlicast destination port. A device may support RTCP. In this case the port value shall be even to allow the corresponding RTCP stream to be mapped
													    //	to the next higher (odd) destination port number as defined in the RTSP specification
	int		TTL;									    // required, In case of IPv6 the TTL value is assumed as the hop limit. Note that for IPV6 and administratively scoped IPv4 multicast the primary use for hop limit / TTL is 
													    //    to prevent packets from (endlessly) circulating and not limiting scope. In these cases the address contains the scope
	BOOL	AutoStart;								    // required, Read only property signalling that streaming is persistant. Use the methods StartMulticastStreaming and StopMulticastStreaming to switch its state													
} onvif_MulticastConfiguration;

typedef struct
{
	uint32	RateControlFlag	: 1;					    // Indicates whether the field RateControl is valid
	uint32	MPEG4Flag		: 1;					    // Indicates whether the field MPEG4 is valid
	uint32	H264Flag		: 1;					    // Indicates whether the field H264 is valid
	uint32 	Reserved		: 29;
	
	char 	Name[ONVIF_NAME_LEN];					    // required, User readable name. Length up to 64 characters
	int 	UseCount;								    // required, Number of internal references currently using this configuration. This parameter is read-only and cannot be changed by a set request
	char 	token[ONVIF_TOKEN_LEN];					    // required, Token that uniquely refernces this configuration. Length up to 64 characters

	onvif_VideoEncoding		    Encoding;		        // required, Used video codec, either Jpeg, H.264 or Mpeg4
	onvif_VideoResolution	    Resolution;		        // required, Configured video resolution

	int		Quality;								    // required, Relative value for the video quantizers and the quality of the video. A high value within supported quality range means higher quality

	onvif_VideoRateControl	    RateControl;	        // optional, Optional element to configure rate control related parameters
	onvif_Mpeg4Configuration	MPEG4;			        // optional, Optional element to configure Mpeg4 related parameters
	onvif_H264Configuration	    H264;			        // optional, Optional element to configure H.264 related parameters
	
	onvif_MulticastConfiguration    Multicast;	        // required, Defines the multicast settings that could be used for video streaming
	
	int		SessionTimeout;							    // required, The rtsp session timeout for the related video stream, unit is second
} onvif_VideoEncoderConfiguration;

typedef struct 
{
	char    token[ONVIF_TOKEN_LEN];	            	    // required
	int     Channels;	                                // required, number of available audio channels. (1: mono, 2: stereo)
} onvif_AudioSource;

typedef struct
{
	char 	Name[ONVIF_NAME_LEN];					    // required, User readable name. Length up to 64 characters
	int 	UseCount;								    // required, Number of internal references currently using this configuration. This parameter is read-only and cannot be changed by a set request
	char 	token[ONVIF_TOKEN_LEN];					    // required, Token that uniquely refernces this configuration. Length up to 64 characters
	
	char 	SourceToken[ONVIF_TOKEN_LEN];			    // required, Token of the Audio Source the configuration applies to
} onvif_AudioSourceConfiguration;


typedef struct
{
	char 	Name[ONVIF_NAME_LEN];					    // required, User readable name. Length up to 64 characters
	int 	UseCount;								    // required, Number of internal references currently using this configuration. This parameter is read-only and cannot be changed by a set request
	char 	token[ONVIF_TOKEN_LEN];					    // required, Token that uniquely refernces this configuration. Length up to 64 characters
	
	onvif_AudioEncoding Encoding;					    // required, Audio codec used for encoding the audio input (either G.711, G.726 or AAC)

	int		Bitrate;								    // required, The output bitrate in kbps
	int		SampleRate;								    // required, The output sample rate in kHz

	onvif_MulticastConfiguration    Multicast;	        // required, Defines the multicast settings that could be used for video streaming

	int		SessionTimeout;							    // required, The rtsp session timeout for the related audio stream, unit is second
} onvif_AudioEncoderConfiguration;

typedef struct 
{
	onvif_AudioEncoding Encoding;	                    // required, The enoding used for audio data (either G.711, G.726 or AAC)
	
	onvif_IntList   BitrateList;	                    // required, List of supported bitrates in kbps for the specified Encoding
	onvif_IntList   SampleRateList;	                    // required, List of supported Sample Rates in kHz for the specified Encoding
} onvif_AudioEncoderConfigurationOption;

typedef struct
{
    int     sizeOptions;                                // required, valid Options numbers                           
    onvif_AudioEncoderConfigurationOption   Options[3]; // optional, list of supported AudioEncoderConfigurations
} onvif_AudioEncoderConfigurationOptions;

typedef struct
{
	onvif_VideoResolution	ResolutionsAvailable[MAX_RES_NUMS];	// required, List of supported image sizes

	onvif_IntRange	FrameRateRange;					    // required, Supported frame rate in fps (frames per second)
	onvif_IntRange	EncodingIntervalRange;			    // required, Supported encoding interval range. The encoding interval corresponds to the number of frames devided by the encoded frames. An encoding interval value of "1" means that all frames are encoded
} onvif_JpegOptions;

typedef struct
{	
	uint32 	Mpeg4Profile_SP 	: 1;				    // required, Indicates whether the SP profile is valid
	uint32 	Mpeg4Profile_ASP 	: 1;				    // required, Indicates whether the ASP profile is valid
	uint32 	Reserverd			: 30;

	onvif_VideoResolution	ResolutionsAvailable[MAX_RES_NUMS];	// required, List of supported image sizes

	onvif_IntRange	GovLengthRange;					    // required, Supported group of Video frames length. This value typically corresponds to the I-Frame distance
	onvif_IntRange	FrameRateRange;					    // required, Supported frame rate in fps (frames per second)
	onvif_IntRange	EncodingIntervalRange;			    // required, Supported encoding interval range. The encoding interval corresponds to the number of frames devided by the encoded frames. An encoding interval value of "1" means that all frames are encoded	
} onvif_Mpeg4Options;

typedef struct
{
	uint32 	H264Profile_Baseline	: 1;			    // required, Indicates whether the Baseline profile is valid				
	uint32 	H264Profile_Main 		: 1;			    // required, Indicates whether the Main profile is valid	
	uint32 	H264Profile_Extended 	: 1;			    // required, Indicates whether the Extended profile is valid	
	uint32 	H264Profile_High		: 1;			    // required, Indicates whether the High profile is valid	
	uint32 	Reserverd				: 28;

	onvif_VideoResolution	ResolutionsAvailable[MAX_RES_NUMS];	// required, List of supported image sizes
	
	onvif_IntRange	GovLengthRange;					    // required, Supported group of Video frames length. This value typically corresponds to the I-Frame distance
	onvif_IntRange	FrameRateRange;					    // required, Supported frame rate in fps (frames per second)
	onvif_IntRange	EncodingIntervalRange;			    // required, Supported encoding interval range. The encoding interval corresponds to the number of frames devided by the encoded frames. An encoding interval value of "1" means that all frames are encoded	
} onvif_H264Options;

typedef struct
{
    onvif_JpegOptions   JpegOptions;                    // required 
	onvif_IntRange      BitrateRange;	                // required 
} onvif_JpegOptions2;   

typedef struct
{
    onvif_Mpeg4Options  Mpeg4Options;                   // required
	onvif_IntRange      BitrateRange;	                // required 
} onvif_Mpeg4Options2;

typedef struct
{
    onvif_H264Options   H264Options;                    // required
    onvif_IntRange      BitrateRange;	                // required 	
} onvif_H264Options2;

typedef struct 
{
    uint32	JPEGFlag 	    : 1;				        // Indicates whether the field JPEG is valid
	uint32	MPEG4Flag 	    : 1;				        // Indicates whether the field MPEG4 is valid
	uint32	H264Flag 	    : 1;				        // Indicates whether the field H264 is valid
	uint32	Reserved 	    : 29;
	
    onvif_JpegOptions2  JPEG;	                        // optional 
	onvif_Mpeg4Options2 MPEG4;	                        // optional 
	onvif_H264Options2  H264;	                        // optional 
} onvif_VideoEncoderOptionsExtension;

typedef struct 
{
	uint32	JPEGFlag 	    : 1;				        // Indicates whether the field JPEG is valid
	uint32	MPEG4Flag 	    : 1;				        // Indicates whether the field MPEG4 is valid
	uint32	H264Flag 	    : 1;				        // Indicates whether the field H264 is valid
	uint32	ExtensionFlag 	: 1;				        // Indicates whether the field Extension is valid
	uint32	Reserved 	    : 28;				
	
	onvif_IntRange		QualityRange;		            // required, Range of the quality values. A high value means higher quality
	
	onvif_JpegOptions 	JPEG;				            // optional, Optional JPEG encoder settings ranges
	onvif_Mpeg4Options	MPEG4;				            // optional, Optional MPEG-4 encoder settings ranges
	onvif_H264Options	H264;				            // optional, Optional H.264 encoder settings ranges	

	onvif_VideoEncoderOptionsExtension  Extension;	    // optional 
} onvif_VideoEncoderConfigurationOptions;

typedef struct 
{
	BOOL 	Status;									    // required, True if the metadata stream shall contain the PTZ status (IDLE, MOVING or UNKNOWN)
	BOOL 	Position;								    // required, True if the metadata stream shall contain the PTZ position 
} onvif_PTZFilter;

typedef struct 
{
    char    Dialect[1024];                              // Dialect
	char    TopicExpression[256];                       // TopicExpression
} onvif_EventSubscription;

typedef struct 
{
	uint32	AnalyticsFlag 	: 1;				        // Indicates whether the field Analytics is valid
	uint32	PTZStatusFlag 	: 1;				        // Indicates whether the field PTZStatus is valid
	uint32	EventsFlag 	    : 1;				        // Indicates whether the field Events is valid
	uint32	Reserved 		: 29;
	
	char 	Name[ONVIF_NAME_LEN];					    // required , User readable name. Length up to 64 characters
	int 	UseCount;								    // required, Number of internal references currently using this configuration. This parameter is read-only and cannot be changed by a set request 
	char 	token[ONVIF_TOKEN_LEN];					    // required, Token that uniquely refernces this configuration. Length up to 64 characters 
	BOOL 	Analytics;								    // optional, Defines whether the streamed metadata will include metadata from the analytics engines (video, cell motion, audio etc.) 
	int 	SessionTimeout;							    // required, The rtsp session timeout for the related audio stream, unit is second
	
	onvif_PTZFilter 		PTZStatus;				    // optional, optional element to configure which PTZ related data is to include in the metadata stream 
	onvif_EventSubscription Events;	                    // optional, 
	onvif_MulticastConfiguration Multicast;			    // required, efines the multicast settings that could be used for video streaming
} onvif_MetadataConfiguration;

typedef struct 
{
	uint32	PanTiltPositionSupportedFlag	: 1;	    // Indicates whether the field PanTiltPositionSupported is valid
	uint32	ZoomPositionSupportedFlag 		: 1;	    // Indicates whether the field ZoomPositionSupported is valid
	uint32	Reserved 						: 30;
	
	BOOL 	PanTiltStatusSupported;					    // required, True if the device is able to stream pan or tilt status information
	BOOL 	ZoomStatusSupported;					    // required, True if the device is able to stream zoom status inforamtion
	BOOL 	PanTiltPositionSupported;				    // optional, True if the device is able to stream the pan or tilt position
	BOOL 	ZoomPositionSupported;					    // optional, True if the device is able to stream zoom position information
} onvif_PTZStatusFilterOptions;

typedef struct 
{
	onvif_PTZStatusFilterOptions PTZStatusFilterOptions;// required, This message contains the metadata configuration options. If a metadata configuration is specified, 
													    //   the options shall concern that particular configuration. If a media profile is specified, the options shall be compatible with that media profile. 
													    //   If no tokens are specified, the options shall be considered generic for the device
} onvif_MetadataConfigurationOptions;

typedef struct
{
	uint32	PosFlag		: 1;						    // Indicates whether the field Pos is valid
	uint32  Reserved	: 31;
	
	onvif_OSDPosType	Type;						    // required, For OSD position type

	onvif_Vector		Pos;						    // Optional, when Type is Custom, this field is valid
} onvif_OSDPosConfiguration;


typedef struct
{
	uint32  ColorspaceFlag  : 1;					    // Indicates whether the field Colorspace is valid
	uint32	TransparentFlag	: 1;					    // Indicates whether the field Transparent is valid
	uint32 	Reserved		: 30;
	
	float	X;										    // required, 
	float	Y;										    // required, 
	float	Z;										    // required, 

	int		Transparent;							    // Optional, The value range of "Transparent" could be defined by vendors only should follow this rule: the minimum value means non-transparent and the maximum value maens fully transparent
	char 	Colorspace[256];						    // Optional, support the following colorspace
													    // 	http://www.onvif.org/ver10/colorspace/YCbCr
													    //	http://www.onvif.org/ver10/colorspace/CIELUV
													    // 	http://www.onvif.org/ver10/colorspace/CIELAB 
													    // 	http://www.onvif.org/ver10/colorspace/HSV
} onvif_OSDColor;

typedef struct
{
	uint32	DateFormatFlag		: 1;				    // Indicates whether the field DateFormat is valid
	uint32	TimeFormatFlag		: 1;				    // Indicates whether the field TimeFormat is valid
	uint32	FontSizeFlag		: 1;				    // Indicates whether the field FontSize is valid
	uint32	FontColorFlag		: 1;				    // Indicates whether the field FontColor is valid
	uint32	BackgroundColorFlag	: 1;				    // Indicates whether the field BackgroundColor is valid
	uint32	PlainTextFlag		: 1;				    // Indicates whether the field PlainText is valid
	uint32 	Reserved			: 26;
	
	onvif_OSDTextType	Type;						    // required, 
	
	char	DateFormat[64];							    // Optional, List of supported OSD date formats. This element shall be present when the value of Type field has Date or DateAndTime. The following DateFormat are defined:
													    /*
    														M/d/yyyy - e.g. 3/6/2013
    														MM/dd/yyyy - e.g. 03/06/2013
    														dd/MM/yyyy - e.g. 06/03/2013
    														yyyy/MM/dd - e.g. 2013/03/06
    														yyyy-MM-dd - e.g. 2013-06-03
    														dddd, MMMM dd, yyyy - e.g. Wednesday, March 06, 2013
    														MMMM dd, yyyy - e.g. March 06, 2013
    														dd MMMM, yyyy - e.g. 06 March, 2013
													    */
	char	TimeFormat[64];							    // Optional, List of supported OSD time formats. This element shall be present when the value of Type field has Time or DateAndTime. The following TimeFormat are defined:
													    /*
    														h:mm:ss tt - e.g. 2:14:21 PM
    														hh:mm:ss tt - e.g. 02:14:21 PM
    														H:mm:ss - e.g. 14:14:21
    														HH:mm:ss - e.g. 14:14:21
													    */

	int		FontSize;								    // Optional, Font size of the text in pt
	
	onvif_OSDColor	FontColor;						    // Optional, Font color of the text
	onvif_OSDColor	BackgroundColor;				    // Optional, Background color of the text
	
	char	PlainText[256];							    // Optional, The content of text to be displayed	
} onvif_OSDTextConfiguration;

typedef struct
{
	char	ImgPath[256];							    // required, The URI of the image which to be displayed
} onvif_OSDImgConfiguration;

typedef struct
{
	uint32	TextStringFlag	: 1;					    // Indicates whether the field TextString is valid
	uint32	ImageFlag		: 1;					    // Indicates whether the field Image is valid
	uint32 	Reserved		: 30;
	
	char	token[ONVIF_TOKEN_LEN];					    // required, OSD config token
	char	VideoSourceConfigurationToken[ONVIF_TOKEN_LEN];	// required, Reference to the video source configuration

	onvif_OSDType	Type;							    // required, Type of OSD
	
	onvif_OSDPosConfiguration	Position;			    // required, Position configuration of OSD
	onvif_OSDTextConfiguration 	TextString;			    // Optional, Text configuration of OSD. It shall be present when the value of Type field is Text
	onvif_OSDImgConfiguration	Image;				    // Optional, Image configuration of OSD. It shall be present when the value of Type field is Image
} onvif_OSDConfiguration;

typedef struct 
{
	uint32 	ImageFlag		: 1;					    // Indicates whether the field Image is valid
	uint32 	PlainTextFlag	: 1;					    // Indicates whether the field PlainText is valid
	uint32 	DateFlag		: 1;					    // Indicates whether the field Date is valid
	uint32 	TimeFlag		: 1;					    // Indicates whether the field Time is valid
	uint32 	DateAndTimeFlag	: 1;					    // Indicates whether the field DateAndTime is valid
	uint32 	Reserved		: 27;
	
	int 	Total;									    // required 
	int 	Image;									    // optional
	int 	PlainText;								    // optional
	int 	Date;									    // optional
	int 	Time;									    // optional
	int 	DateAndTime;							    // optional
} onvif_MaximumNumberOfOSDs;

typedef struct 
{
	uint32  ColorspaceFlag	: 1;					    // Indicates whether the field Colorspace is valid
	uint32  Reserved		: 31;					
	
	float 	X;										    // required, 
	float 	Y;										    // required, 
	float 	Z;										    // required, 
	
	char 	Colorspace[128];						    // optional, The following values are acceptable for Colourspace attribute
	                                                    //  http://www.onvif.org/ver10/colorspace/YCbCr
				                                        //  http://www.onvif.org/ver10/colorspace/CIELUV
                                        				//  http://www.onvif.org/ver10/colorspace/CIELAB
                                        				//  http://www.onvif.org/ver10/colorspace/HSV
} onvif_Color;

typedef struct
{
	onvif_FloatRange 	X;							    // required
	onvif_FloatRange 	Y;							    // required
	onvif_FloatRange 	Z;							    // required 

	char 	Colorspace[128];						    // required, The following values are acceptable for Colourspace attribute
	                                                    //  http://www.onvif.org/ver10/colorspace/YCbCr
				                                        //  http://www.onvif.org/ver10/colorspace/CIELUV
                                        				//  http://www.onvif.org/ver10/colorspace/CIELAB
                                        				//  http://www.onvif.org/ver10/colorspace/HSV
} onvif_ColorspaceRange;

typedef struct 
{
	int				sizeColorList;	
	onvif_Color 	ColorList[10];					    // optional, List the supported color

	int						sizeColorspaceRange;
	onvif_ColorspaceRange	ColorspaceRange[10];	    // optional, Define the rang of color supported
} onvif_ColorOptions;

typedef struct 
{
	uint32  ColorFlag		: 1;					    // Indicates whether the field Color is valid
	uint32  TransparentFlag	: 1;					    // Indicates whether the field Transparent is valid
	uint32  Reserved		: 30;
	
	onvif_ColorOptions 	Color;						    // optional, Optional list of supported colors 
	onvif_IntRange 		Transparent;				    // optional, Range of the transparent level. Larger means more tranparent
} onvif_OSDColorOptions;

typedef struct 
{
	uint32	OSDTextType_Plain		: 1;			    // Indicates whether support OSD text type plain
	uint32  OSDTextType_Date		: 1;			    // Indicates whether support OSD text type date
	uint32  OSDTextType_Time		: 1;			    // Indicates whether support OSD text type time
	uint32  OSDTextType_DateAndTime	: 1;			    // Indicates whether support OSD text type dateandtime
	uint32  FontSizeRangeFlag		: 1;			    // Indicates whether the field FontSizeRange is valid
	uint32  FontColorFlag			: 1;			    // Indicates whether the field FontColor is valid	
	uint32  BackgroundColorFlag		: 1;			    // Indicates whether the field BackgroundColor is valid
	uint32  Reserved				: 25;
	
	onvif_IntRange	FontSizeRange;					    // optional, range of the font size value

	int		DateFormatSize;
	char 	DateFormat[10][64];						    // optional, List of supported date format

	int		TimeFormatSize;
	char 	TimeFormat[10][64];						    // optional, List of supported time format
	
	onvif_OSDColorOptions	FontColor;				    // optional, List of supported font color
	onvif_OSDColorOptions 	BackgroundColor;		    // optional, List of supported background color
} onvif_OSDTextOptions;

typedef struct 
{
	int 	ImagePathSize;	
	char 	ImagePath[10][256];						    // required, List of avaiable uris of image
} onvif_OSDImgOptions;

typedef struct 
{
	uint32	OSDType_Text			: 1;			    // Indicates whether support OSD text type
	uint32  OSDType_Image			: 1;			    // Indicates whether support OSD image type
	uint32  OSDType_Extended		: 1;			    // Indicates whether support OSD extended type
	uint32  OSDPosType_UpperLeft	: 1;			    // Indicates whether support OSD position UpperLeft type
	uint32  OSDPosType_UpperRight	: 1;			    // Indicates whether support OSD position UpperRight type
	uint32 	OSDPosType_LowerLeft	: 1;			    // Indicates whether support OSD position LowerLeft type
	uint32  OSDPosType_LowerRight	: 1;			    // Indicates whether support OSD position LowerRight type
	uint32  OSDPosType_Custom		: 1;			    // Indicates whether support OSD position Custom type
	uint32  TextOptionFlag			: 1;			    // Indicates whether the field TextOption is valid
	uint32	ImageOptionFlag			: 1;			    // Indicates whether the field ImageOption is valid
	uint32 	Reserved				: 22;
	
	onvif_MaximumNumberOfOSDs MaximumNumberOfOSDs;	    // required, The maximum number of OSD configurations supported for the specificate video source configuration. 
													    //   If a device limits the number of instances by OSDType, it should indicate the supported number via the related attribute
	onvif_OSDTextOptions	TextOption;				    // optional, Option of the OSD text configuration. This element shall be returned if the device is signaling the support for Text
	onvif_OSDImgOptions 	ImageOption;			    // optional, Option of the OSD image configuration. This element shall be returned if the device is signaling the support for Image
} onvif_OSDConfigurationOptions;

typedef struct
{
	float	x;										    // required
} onvif_Vector1D;

typedef struct 
{
	uint32	PanTiltFlag	: 1;						    // Indicates whether the field PanTilt is valid
	uint32	ZoomFlag	: 1;						    // Indicates whether the field Zoom is valid
	uint32 	Reserved	: 30;
	
	onvif_Vector 	PanTilt;						    // optional, Pan and tilt position. The x component corresponds to pan and the y component to tilt
	onvif_Vector1D	Zoom;							    // optional, A zoom position
} onvif_PTZVector;

typedef struct 
{
	uint32	PanTiltFlag	: 1;						    // Indicates whether the field PanTilt is valid
	uint32	ZoomFlag	: 1;						    // Indicates whether the field Zoom is valid
	uint32 	Reserved	: 30;
	
	onvif_Vector 	PanTilt;						    // optional, Pan and tilt speed. The x component corresponds to pan and the y component to tilt. If omitted in a request, the current (if any) PanTilt movement should not be affected
	onvif_Vector1D	Zoom;							    // optional, A zoom speed. If omitted in a request, the current (if any) Zoom movement should not be affected
} onvif_PTZSpeed;

typedef struct 
{
	uint32	PTZPositionFlag	: 1;					    // Indicates whether the field PTZPosition is valid
	uint32 	Reserved		: 31;
	
	char 	Name[ONVIF_NAME_LEN];					    // required, A list of preset position name
	char 	token[ONVIF_TOKEN_LEN];					    // required

	onvif_PTZVector	PTZPosition;					    // optional, A list of preset position
} onvif_PTZPreset;

typedef struct 
{
	onvif_FloatRange 	XRange;						    // required 
	onvif_FloatRange	YRange;						    // required 
} onvif_PanTiltLimits;

typedef struct 
{
	onvif_FloatRange 	XRange;						    // required 
} onvif_ZoomLimits;

typedef struct onvif_PTControlDirection
{
	uint32	EFlipFlag	: 1;						    // Indicates whether the field EFlip is valid
	uint32	ReverseFlag	: 1;						    // Indicates whether the field Reverse is valid
	uint32 	Reserved	: 30;
	
	onvif_EFlipMode		EFlip;						    // optional, Optional element to configure related parameters for E-Flip
	onvif_ReverseMode	Reverse;					    // optional, Optional element to configure related parameters for reversing of PT Control Direction
} onvif_PTControlDirection;

typedef struct 
{
	uint32	PTControlDirectionFlag	: 1;			    // Indicates whether the field PTControlDirection is valid
	uint32 	Reserved				: 31;
	
	onvif_PTControlDirection	PTControlDirection;	    // optional, Optional element to configure PT Control Direction related features
} onvif_PTZConfigurationExtension;

typedef struct
{
	uint32	DefaultPTZSpeedFlag		: 1;			    // Indicates whether the field DefaultPTZSpeed is valid
	uint32	DefaultPTZTimeoutFlag	: 1;			    // Indicates whether the field DefaultPTZTimeout is valid
	uint32	PanTiltLimitsFlag		: 1;			    // Indicates whether the field PanTiltLimits is valid
	uint32	ZoomLimitsFlag			: 1;			    // Indicates whether the field ZoomLimits is valid
	uint32	ExtensionFlag			: 1;			    // Indicates whether the field Extension is valid
	uint32	MoveRampFlag		    : 1;			    // Indicates whether the field MoveRamp is valid
	uint32	PresetRampFlag			: 1;			    // Indicates whether the field PresetRamp is valid
	uint32	PresetTourRampFlag	    : 1;			    // Indicates whether the field PresetTourRamp is valid
	uint32 	Reserved				: 24;
	
	char 	Name[ONVIF_NAME_LEN];					    // required 
	int 	UseCount;								    // required 
	char 	token[ONVIF_TOKEN_LEN];					    // required 
	char 	NodeToken[ONVIF_TOKEN_LEN];				    // required, A mandatory reference to the PTZ Node that the PTZ Configuration belongs to
	
	onvif_PTZSpeed		DefaultPTZSpeed;			    // optional, If the PTZ Node supports absolute or relative PTZ movements, it shall specify corresponding default Pan/Tilt and Zoom speeds
	int					DefaultPTZTimeout;			    // optional, If the PTZ Node supports continuous movements, it shall specify a default timeout, after which the movement stops 
	onvif_PanTiltLimits	PanTiltLimits;				    // optional, The Pan/Tilt limits element should be present for a PTZ Node that supports an absolute Pan/Tilt. If the element is present it signals the support for configurable Pan/Tilt limits. 
													    //	If limits are enabled, the Pan/Tilt movements shall always stay within the specified range. The Pan/Tilt limits are disabled by setting the limits to �CINF or +INF 
	onvif_ZoomLimits 	ZoomLimits;					    // optional, The Zoom limits element should be present for a PTZ Node that supports absolute zoom. If the element is present it signals the supports for configurable Zoom limits. 
													    //	If limits are enabled the zoom movements shall always stay within the specified range. The Zoom limits are disabled by settings the limits to -INF and +INF
	
	onvif_PTZConfigurationExtension	Extension;		    // optional 

	int     MoveRamp;	                                // optional, The optional acceleration ramp used by the device when moving
	int     PresetRamp;	                                // optional, The optional acceleration ramp used by the device when recalling presets
	int     PresetTourRamp;	                            // optional, The optional acceleration ramp used by the device when executing PresetTours
} onvif_PTZConfiguration;

typedef struct 
{
	// Indicates which preset tour operations are available for this PTZ Node
	
	uint32	PTZPresetTourOperation_Start	: 1;
	uint32	PTZPresetTourOperation_Stop		: 1;
	uint32	PTZPresetTourOperation_Pause	: 1;
	uint32	PTZPresetTourOperation_Extended	: 1;
	uint32	Reserved						: 28;	
	
	int 	MaximumNumberOfPresetTours;				    // required, Indicates number of preset tours that can be created. Required preset tour operations shall be available for this PTZ Node if one or more preset tour is supported
} onvif_PTZPresetTourSupported;

typedef struct 
{
	uint32	SupportedPresetTourFlag	: 1;			    // Indicates whether the field SupportedPresetTour is valid
	uint32 	Reserved				: 31;
	
	onvif_PTZPresetTourSupported	SupportedPresetTour;// optional, Detail of supported Preset Tour feature
} onvif_PTZNodeExtension;

typedef struct 
{
	onvif_FloatRange 	XRange;						    // required 
	onvif_FloatRange	YRange;						    // required 
} onvif_Space2DDescription;

typedef struct 
{
	onvif_FloatRange 	XRange;						    // required 
} onvif_Space1DDescription;

typedef struct 
{
    uint32	AbsolutePanTiltPositionSpaceFlag	: 1;			// Indicates whether the field AbsolutePanTiltPositionSpace is valid
    uint32	AbsoluteZoomPositionSpaceFlag	    : 1;			// Indicates whether the field AbsoluteZoomPositionSpace is valid
    uint32	RelativePanTiltTranslationSpaceFlag	: 1;			// Indicates whether the field RelativePanTiltTranslationSpace is valid
    uint32	RelativeZoomTranslationSpaceFlag	: 1;			// Indicates whether the field RelativeZoomTranslationSpace is valid
    uint32	ContinuousPanTiltVelocitySpaceFlag	: 1;			// Indicates whether the field ContinuousPanTiltVelocitySpace is valid
    uint32	ContinuousZoomVelocitySpaceFlag	    : 1;			// Indicates whether the field ContinuousZoomVelocitySpace is valid
    uint32	PanTiltSpeedSpaceFlag	            : 1;			// Indicates whether the field PanTiltSpeedSpace is valid
    uint32	ZoomSpeedSpaceFlag	                : 1;			// Indicates whether the field ZoomSpeedSpace is valid
	uint32 	Reserved				            : 24;
	
	onvif_Space2DDescription	AbsolutePanTiltPositionSpace;	// optional, The Generic Pan/Tilt Position space is provided by every PTZ node that supports absolute Pan/Tilt, since it does not relate to a specific physical range. 
																//	Instead, the range should be defined as the full range of the PTZ unit normalized to the range -1 to 1 resulting in the following space description
	onvif_Space1DDescription	AbsoluteZoomPositionSpace;		// optional, The Generic Zoom Position Space is provided by every PTZ node that supports absolute Zoom, since it does not relate to a specific physical range. 
																//	Instead, the range should be defined as the full range of the Zoom normalized to the range 0 (wide) to 1 (tele). 
																//	There is no assumption about how the generic zoom range is mapped to magnification, FOV or other physical zoom dimension
	onvif_Space2DDescription	RelativePanTiltTranslationSpace;// optional, The Generic Pan/Tilt translation space is provided by every PTZ node that supports relative Pan/Tilt, since it does not relate to a specific physical range. 
																//	Instead, the range should be defined as the full positive and negative translation range of the PTZ unit normalized to the range -1 to 1, 
																//	where positive translation would mean clockwise rotation or movement in right/up direction resulting in the following space description 
	onvif_Space1DDescription	RelativeZoomTranslationSpace;	// optional, The Generic Zoom Translation Space is provided by every PTZ node that supports relative Zoom, since it does not relate to a specific physical range. 
																//	Instead, the corresponding absolute range should be defined as the full positive and negative translation range of the Zoom normalized to the range -1 to1, 
																//	where a positive translation maps to a movement in TELE direction. The translation is signed to indicate direction (negative is to wide, positive is to tele). 
																//	There is no assumption about how the generic zoom range is mapped to magnification, FOV or other physical zoom dimension. This results in the following space description
	onvif_Space2DDescription	ContinuousPanTiltVelocitySpace;	// optional, The generic Pan/Tilt velocity space shall be provided by every PTZ node, since it does not relate to a specific physical range. 
																//	Instead, the range should be defined as a range of the PTZ unit's speed normalized to the range -1 to 1, where a positive velocity would map to clockwise 
																//	rotation or movement in the right/up direction. A signed speed can be independently specified for the pan and tilt component resulting in the following space description 
	onvif_Space1DDescription	ContinuousZoomVelocitySpace;	// optional, The generic zoom velocity space specifies a zoom factor velocity without knowing the underlying physical model. The range should be normalized from -1 to 1, 
																//	where a positive velocity would map to TELE direction. A generic zoom velocity space description resembles the following
	onvif_Space1DDescription	PanTiltSpeedSpace;				// optional, The speed space specifies the speed for a Pan/Tilt movement when moving to an absolute position or to a relative translation. 
																//	In contrast to the velocity spaces, speed spaces do not contain any directional information. The speed of a combined Pan/Tilt 
																//	movement is represented by a single non-negative scalar value
	onvif_Space1DDescription	ZoomSpeedSpace;					// optional, The speed space specifies the speed for a Zoom movement when moving to an absolute position or to a relative translation. 
																//	In contrast to the velocity spaces, speed spaces do not contain any directional information 
} onvif_PTZSpaces;

typedef struct 
{
	uint32  NameFlag				: 1;			    // Indicates whether the field Name is valid
	uint32	ExtensionFlag			: 1;			    // Indicates whether the field Extension is valid
	uint32	FixedHomePositionFlag	: 1;			    // Indicates whether the field FixedHomePosition is valid
	uint32	Reserved				: 29;	
	
	char 	token[ONVIF_TOKEN_LEN];					    // required 
	char 	Name[ONVIF_NAME_LEN];					    // optional, A unique identifier that is used to reference PTZ Nodes
	
	onvif_PTZSpaces 		SupportedPTZSpaces;		    // required, A list of Coordinate Systems available for the PTZ Node. For each Coordinate System, the PTZ Node MUST specify its allowed range
	
	int 	MaximumNumberOfPresets;					    // required, All preset operations MUST be available for this PTZ Node if one preset is supported 
	BOOL 	HomeSupported;							    // required, A boolean operator specifying the availability of a home position. If set to true, the Home Position Operations MUST be available for this PTZ Node 
	
	onvif_PTZNodeExtension 	Extension;				    // optional 
	
	BOOL 	FixedHomePosition;						    // optional, Indication whether the HomePosition of a Node is fixed or it can be changed via the SetHomePosition command
} onvif_PTZNode;


typedef struct 
{
	// Supported options for EFlip feature	
    uint32  EFlipMode_OFF           : 1;
	uint32  EFlipMode_ON            : 1;
	uint32  EFlipMode_Extended      : 1;

	// Supported options for Reverse feature
	uint32  ReverseMode_OFF         : 1;
	uint32  ReverseMode_ON          : 1;
	uint32  ReverseMode_AUTO        : 1;
	uint32  ReverseMode_Extended    : 1;
	uint32  Reserved                : 25;
} onvif_PTControlDirectionOptions;

typedef struct 
{
    uint32  PTControlDirectionFlag  : 1;		        // Indicates whether the field PTControlDirection is valid
	uint32	Reserved				: 31;

	onvif_PTZSpaces Spaces;							    // required, 
	onvif_IntRange  PTZTimeout;	               		    // required, A timeout Range within which Timeouts are accepted by the PTZ Node
	onvif_PTControlDirectionOptions PTControlDirection;	// optional,  
} onvif_PTZConfigurationOptions;

typedef struct 
{
	uint32  PanTiltFlag	: 1;		    			    // Indicates whether the field PanTilt is valid
	uint32  ZoomFlag  	: 1;		    			    // Indicates whether the field Zoom is valid
	uint32	Reserved	: 30;
	
	onvif_MoveStatus 	PanTilt;					    // optional 
	onvif_MoveStatus 	Zoom;						    // optional 
} onvif_PTZMoveStatus;

typedef struct 
{
	uint32  PositionFlag	: 1;		    		    // Indicates whether the field Position is valid
	uint32  MoveStatusFlag	: 1;		    		    // Indicates whether the field MoveStatus is valid
	uint32  ErrorFlag		: 1;		    		    // Indicates whether the field MoveStatus is valid
	uint32	Reserved		: 29;
	
	onvif_PTZVector 	Position;					    // optional, Specifies the absolute position of the PTZ unit together with the Space references. The default absolute spaces of the corresponding PTZ configuration MUST be referenced within the Position element
	onvif_PTZMoveStatus MoveStatus;					    // optional, Indicates if the Pan/Tilt/Zoom device unit is currently moving, idle or in an unknown state
	
	char 	Error[100];								    // optional, States a current PTZ error
	time_t 	UtcTime;								    // required, Specifies the UTC time when this status was generated 
} onvif_PTZStatus;


typedef struct 
{
    uint32  PresetTokenFlag	: 1;		    		    // Indicates whether the field PresetToken is valid
	uint32  HomeFlag	    : 1;		    		    // Indicates whether the field Home is valid
	uint32  PTZPositionFlag : 1;		    		    // Indicates whether the field PTZPosition is valid
	uint32	Reserved		: 29;
	
	char    PresetToken[ONVIF_TOKEN_LEN];               // optional, Option to specify the preset position with Preset Token defined in advance
	BOOL    Home;                                       // optional, Option to specify the preset position with the home position of this PTZ Node. "False" to this parameter shall be treated as an invalid argument
	
	onvif_PTZVector PTZPosition;                        // optional, Option to specify the preset position with vector of PTZ node directly    
} onvif_PTZPresetTourPresetDetail;

typedef struct 
{
    uint32  SpeedFlag	    : 1;		    		    // Indicates whether the field Speed is valid
	uint32  StayTimeFlag    : 1;		    		    // Indicates whether the field StayTime is valid
	uint32	Reserved		: 30;
	
	onvif_PTZPresetTourPresetDetail PresetDetail;	    // required, Detail definition of preset position of the tour spot
	onvif_PTZSpeed  Speed;	                            // optional, Optional parameter to specify Pan/Tilt and Zoom speed on moving toward this tour spot
	
	int     StayTime;	                                // optional, Optional parameter to specify time duration of staying on this tour sport
} onvif_PTZPresetTourSpot;

typedef struct _ONVIF_PTZPresetTourSpot
{
    struct _ONVIF_PTZPresetTourSpot * next;

    onvif_PTZPresetTourSpot PTZPresetTourSpot;
} ONVIF_PTZPresetTourSpot;

typedef struct 
{
    uint32  CurrentTourSpotFlag : 1;		            // Indicates whether the field CurrentTourSpot is valid
	uint32	Reserved		    : 31;
	
	onvif_PTZPresetTourState    State;	                // required, Indicates state of this preset tour by Idle/Touring/Paused
	onvif_PTZPresetTourSpot     CurrentTourSpot;	    // optional, Indicates a tour spot currently staying
} onvif_PTZPresetTourStatus;

typedef struct 
{
    uint32  RecurringTimeFlag	    : 1;		        // Indicates whether the field RecurringTime is valid
	uint32  RecurringDurationFlag   : 1;		        // Indicates whether the field RecurringDuration is valid
	uint32  DirectionFlag		    : 1;		        // Indicates whether the field Direction is valid
	uint32  RandomPresetOrderFlag   : 1;		        // Indicates whether the field RandomPresetOrder is valid
	uint32	Reserved	            : 28;
	
	int     RecurringTime;	                            // optional, Optional parameter to specify how many times the preset tour is recurred
	int     RecurringDuration;	                        // optional, Optional parameter to specify how long time duration the preset tour is recurred

	onvif_PTZPresetTourDirection    Direction;	        // optional, Optional parameter to choose which direction the preset tour goes. Forward shall be chosen in case it is omitted

	BOOL RandomPresetOrder;	                            // optional, Execute presets in random order. If set to true and Direction is also present, Direction will be ignored and presets of the Tour will be recalled randomly
} onvif_PTZPresetTourStartingCondition;

typedef struct
{
    char    Name[ONVIF_NAME_LEN];	                    // optional, Readable name of the preset tour
    char    token[ONVIF_TOKEN_LEN];	                    // required, Unique identifier of this preset tour

    BOOL    AutoStart;                                  // required, Auto Start flag of the preset tour. True allows the preset tour to be activated always
    
	onvif_PTZPresetTourStatus   Status;	                // required, Read only parameters to indicate the status of the preset tour
		                            
	onvif_PTZPresetTourStartingCondition    StartingCondition;  // required, Parameters to specify the detail behavior of the preset tour

	ONVIF_PTZPresetTourSpot *   TourSpot;	            // optional, A list of detail of touring spots including preset positions
	
} onvif_PresetTour;

typedef struct
{
    int     Min;	                                    // required, unit is second
	int     Max;	                                    // required, unit is second
} onvif_DurationRange;

typedef struct 
{
    uint32  RecurringTimeFlag	            : 1;        // Indicates whether the field RecurringTime is valid
	uint32  RecurringDurationFlag           : 1;        // Indicates whether the field RecurringDuration is valid
    uint32  PTZPresetTourDirection_Forward  : 1;        // 
    uint32  PTZPresetTourDirection_Backward : 1;        // 
    uint32  PTZPresetTourDirection_Extended : 1;        // 
    uint32  Reserved                        : 27;
    
	onvif_IntRange          RecurringTime;	            // optional, Supported range of Recurring Time
	onvif_DurationRange     RecurringDuration;	        // optional, Supported range of Recurring Duration
} onvif_PTZPresetTourStartingConditionOptions;

typedef struct 
{
    uint32  HomeFlag	                : 1;            // Indicates whether the field Home is valid
	uint32  PanTiltPositionSpaceFlag    : 1;            // Indicates whether the field PanTiltPositionSpace is valid
    uint32  ZoomPositionSpaceFlag       : 1;            // Indicates whether the field ZoomPositionSpace is valid
    uint32  Reserved                    : 29;
    
	int     sizePresetToken;	
	char    PresetToken[10][ONVIF_TOKEN_LEN];	        // optional, A list of available Preset Tokens for tour spots
	
	BOOL    Home;	                                    // optional, An option to indicate Home postion for tour spots
	
	onvif_Space2DDescription    PanTiltPositionSpace;   // optional, Supported range of Pan and Tilt for tour spots
	onvif_Space1DDescription    ZoomPositionSpace;	    // optional, Supported range of Zoom for a tour spot
} onvif_PTZPresetTourPresetDetailOptions;

typedef struct 
{
	onvif_PTZPresetTourPresetDetailOptions  PresetDetail;   // required, Supported options for detail definition of preset position of the tour spot
	onvif_DurationRange     StayTime;	                // required, Supported range of stay time for a tour spot
} onvif_PTZPresetTourSpotOptions;

typedef struct
{
	BOOL    AutoStart;	                                // required, Indicates whether or not the AutoStart is supported
	
	onvif_PTZPresetTourStartingConditionOptions StartingCondition;  // required, Supported options for Preset Tour Starting Condition
	onvif_PTZPresetTourSpotOptions  TourSpot;	        // required, Supported options for Preset Tour Spot
} onvif_PTZPresetTourOptions;


typedef struct 
{
	uint32  NameFlag	: 1;		    			    // Indicates whether the field Name is valid
	uint32  MTUFlag		: 1;		    			    // Indicates whether the field MTU is valid
	uint32	Reserved	: 30;
	
	char 	Name[ONVIF_NAME_LEN];					    // optional, Network interface name, for example eth0
	char 	HwAddress[32];							    // required, Network interface MAC address
	int 	MTU;									    // optional, Maximum transmission unit
} onvif_NetworkInterfaceInfo;

typedef struct 
{
	char 	Address[32];							    // required 
	int 	PrefixLength;							    // required 
	
	BOOL 	DHCP;									    // required, Indicates whether or not DHCP is used
} onvif_IPv4Configuration;

typedef struct 
{
	BOOL 	Enabled;								    // required, Indicates whether or not IPv4 is enabled
	
	onvif_IPv4Configuration	Config;					    // required, IPv4 configuration
} onvif_IPv4NetworkInterface;

typedef struct 
{
    uint32  KeyFlag         : 1;                        // Indicates whether the field Key is valid
    uint32  PassphraseFlag  : 1;                        // Indicates whether the field Passphrase is valid
    uint32  Reserved        : 30;   
    
	char    Key[256];	                                // optional, hexBinary, 
	                                                    // According to IEEE802.11-2007 H.4.1 the RSNA PSK consists of 256 bits, 
	                                                    //  or 64 octets when represented in hex
					                                    // Either Key or Passphrase shall be given, 
					                                    //  if both are supplied Key shall be used by the device and Passphrase ignored.
	char    Passphrase[128];	                        // optional,
	                                                    // According to IEEE802.11-2007 H.4.1 a pass-phrase is 
	                                                    //  a sequence of between 8 and 63 ASCII-encoded characters and
					                                    //  each character in the pass-phrase must have an encoding in the range of 32 to 126 (decimal),inclusive.
					                                    //  If only Passpharse is supplied the Key shall be derived using the algorithm described in 
					                                    //  IEEE802.11-2007 section H.4
} onvif_Dot11PSKSet;

typedef struct 
{
    uint32  AlgorithmFlag   : 1;                        // Indicates whether the field Algorithm is valid
    uint32  PSKFlag         : 1;                        // Indicates whether the field PSK is valid
    uint32  Dot1XFlag       : 1;                        // // Indicates whether the field Dot1X is valid
    uint32  Reserved        : 29;
    
	onvif_Dot11SecurityMode Mode;	                    // required
	onvif_Dot11Cipher       Algorithm;	                // optional
	onvif_Dot11PSKSet       PSK;	                    // optional
	
	char    Dot1X[ONVIF_TOKEN_LEN];	                    // optional
} onvif_Dot11SecurityConfiguration;

typedef struct 
{
	char    SSID[32];	                                // required, hexBinary
	
	onvif_Dot11StationMode Mode;	                    // required
	
	char    Alias[32];	                                // required	
	int     Priority;	                                // required, range is 0-31
	
	onvif_Dot11SecurityConfiguration    Security;	    // required element of type ns2:Dot11SecurityConfiguration */
} onvif_Dot11Configuration;

typedef struct
{
    int     InterfaceType;	                            // required, tt:IANA-IfTypes, Integer indicating interface type, for example: 6 is ethernet
                                                        //  ieee80211(71)
                                                        //  For valid numbers, please refer to http://www.iana.org/assignments/ianaiftype-mib
	int     sizeDot11;	                                // sequence of elements <Dot11>
	onvif_Dot11Configuration    Dot11[4];	            // optional
} onvif_NetworkInterfaceExtension;

typedef struct 
{
	uint32  InfoFlag	    : 1;		    		    // Indicates whether the field Info is valid
	uint32  IPv4Flag	    : 1;		    	        // Indicates whether the field IPv4 is valid
	uint32  ExtensionFlag	: 1;		    		    // Indicates whether the field Extension is valid
	uint32	Reserved	    : 29;
	
	char 	token[ONVIF_TOKEN_LEN];					    // required 
	BOOL 	Enabled;								    // required, Indicates whether or not an interface is enabled
	
	onvif_NetworkInterfaceInfo	Info;				    // optional, Network interface information
	onvif_IPv4NetworkInterface 	IPv4;				    // optional, IPv4 network interface configuration
	onvif_NetworkInterfaceExtension Extension;	        // optional,
} onvif_NetworkInterface;

typedef struct
{
	BOOL 	HTTPFlag;								    // Indicates if the http protocol required
	BOOL 	HTTPEnabled;							    // Indicates if the http protocol is enabled or not
	BOOL 	HTTPSFlag;								    // Indicates if the https protocol required
	BOOL 	HTTPSEnabled;							    // Indicates if the https protocol is enabled or not
	BOOL 	RTSPFlag;								    // Indicates if the rtsp protocol required
	BOOL 	RTSPEnabled;							    // Indicates if the rtsp protocol is enabled or not

	int  	HTTPPort[MAX_SERVER_PORT];				    // The port that is used by the protocol
	int  	HTTPSPort[MAX_SERVER_PORT];				    // The port that is used by the protocol
	int  	RTSPPort[MAX_SERVER_PORT];				    // The port that is used by the protocol
} onvif_NetworkProtocol;

typedef struct 
{
	uint32  SearchDomainFlag	: 1;		    	    // Indicates whether the field Name is valid
	uint32	Reserved			: 31;
	
	BOOL 	FromDHCP;								    // required, Indicates whether or not DNS information is retrieved from DHCP 
	char 	SearchDomain[MAX_SEARCHDOMAIN][64];		    // optional, Search domain
	char	DNSServer[MAX_DNS_SERVER][32];			    // required
} onvif_DNSInformation;

typedef struct
{
	onvif_DynamicDNSType	Type;					    // required 
	
	char 	Name[ONVIF_NAME_LEN];					    // optional 
	int		TTL;									    // optional
} onvif_DynamicDNSInformation;

typedef struct 
{
	BOOL 	FromDHCP;								    // required, Indicates if NTP information is to be retrieved by using DHCP
	char	NTPServer[MAX_NTP_SERVER][32];			    // required
} onvif_NTPInformation;

typedef struct 
{
	uint32  NameFlag	: 1;		    			    // Indicates whether the field Name is valid
	uint32	Reserved	: 31;
	
	BOOL	FromDHCP;								    // required, Indicates whether the hostname is obtained from DHCP or not
	BOOL	RebootNeeded;							    // required, Indicates whether or not a reboot is required after configuration updates
	char 	Name[100];								    // optional, Indicates the hostname
} onvif_HostnameInformation;

typedef struct 
{
	char	IPv4Address[MAX_GATEWAY][32];			    // required, IPv4 address string	
} onvif_NetworkGateway;

typedef struct 
{
	char    InterfaceToken[ONVIF_TOKEN_LEN];	        // required, Unique identifier of network interface
	BOOL    Enabled;	                                // required, Indicates whether the zero-configuration is enabled or not
	int     sizeAddresses;	                            // sequence of elements <Addresses>
	char    Addresses[4][32];	                        // optional, The zero-configuration IPv4 address(es)
} onvif_NetworkZeroConfiguration;

typedef struct 
{
	char    Uri[256];	                                // required
	BOOL    InvalidAfterConnect;	                    // required
	BOOL    InvalidAfterReboot;	                        // required
	int     Timeout;	                                // required, unit is second
} onvif_MediaUri;

typedef struct 
{
    uint32  BSSIDFlag           : 1;
    uint32  PairCipherFlag      : 1;
    uint32  GroupCipherFlag     : 1;
    uint32  SignalStrengthFlag  : 1;
    uint32  Reserved            : 28;
    
	char    SSID[32];	                                // required, hexBinary
	char    BSSID[64];	                                // optional
	
	onvif_Dot11Cipher           PairCipher;	            // optional 
	onvif_Dot11Cipher           GroupCipher;	        // optional 
	onvif_Dot11SignalStrength   SignalStrength;	        // optional 
	
	char    ActiveConfigAlias[32];	                    // required 	
} onvif_Dot11Status;

typedef struct 
{
    uint32  BSSIDFlag           : 1;
    uint32  SignalStrengthFlag  : 1;
    uint32  Reserved            : 30;
    
	char    SSID[32];	                                // required, hexBinary
	char    BSSID[64];	                                // optional

	int     sizeAuthAndMangementSuite;	                // sequence of elements <AuthAndMangementSuite>
	onvif_Dot11AuthAndMangementSuite    AuthAndMangementSuite[4];	// optional

	int     sizePairCipher;	                            // sequence of elements <PairCipher> 
	onvif_Dot11Cipher   PairCipher[4];	                // optional

	int     sizeGroupCipher;	                        // sequence of elements <GroupCipher>
	onvif_Dot11Cipher   GroupCipher[4];	                // optional
	
	onvif_Dot11SignalStrength   SignalStrength;	        // optional
} onvif_Dot11AvailableNetworks;

typedef struct _ONVIF_Dot11AvailableNetworks
{
    struct _ONVIF_Dot11AvailableNetworks * next;

    onvif_Dot11AvailableNetworks Networks;
} ONVIF_Dot11AvailableNetworks;

typedef struct 
{
	char 	TZ[128];								    // required, Posix timezone string
} onvif_TimeZone;

typedef struct
{
	int		Hour;									    // Range is 0 to 23
	int		Minute;									    // Range is 0 to 59
	int		Second;									    // Range is 0 to 61 (typically 59)
} onvif_Time;

typedef struct
{
	int		Year;									    // 
	int		Month;									    // Range is 1 to 12
	int		Day;									    // Range is 1 to 31
} onvif_Date;

typedef struct
{
	onvif_Time	Time;								    // required 
	onvif_Date  Date;								    // required 
} onvif_DateTime;

typedef struct 
{
	uint32  TimeZoneFlag	: 1;		    		    // Indicates whether the field TimeZone is valid
	uint32	Reserved		: 31;
	
	BOOL 	DaylightSavings;						    // required, Informative indicator whether daylight savings is currently on/off
	
	onvif_SetDateTimeType 	DateTimeType;			    // required, Indicates if the time is set manully or through NTP	
	onvif_TimeZone			TimeZone;				    // optional, Timezone information in Posix format 
} onvif_SystemDateTime;

typedef struct 
{
	onvif_ScopeDefinition   ScopeDef;	                // required
	
	char    ScopeItem[128];	                            // required
} onvif_Scope;

typedef struct 
{
    uint32  lonFlag	        : 1;		    		    // Indicates whether the field lon is valid
    uint32  latFlag	        : 1;		    		    // Indicates whether the field lat is valid
    uint32  elevationFlag	: 1;		    		    // Indicates whether the field elevation is valid
	uint32	Reserved		: 29;
	
	double  lon;	                                    // optional, East west location as angle
	double  lat;	                                    // optional, North south location as angle
	float   elevation;	                                // optional, Hight in meters above sea level
} onvif_GeoLocation;

typedef struct 
{
    uint32  rollFlag    : 1;		    		        // Indicates whether the field roll is valid
    uint32  pitchFlag   : 1;		    		        // Indicates whether the field pitch is valid
    uint32  yawFlag	    : 1;		    		        // Indicates whether the field yaw is valid
	uint32	Reserved    : 29;
	
	float   roll;	                                    // optional, Rotation around the x axis
	float   pitch;	                                    // optional, Rotation around the y axis
	float   yaw;	                                    // optional, Rotation around the z axis
} onvif_GeoOrientation;

typedef struct 
{
    uint32  xFlag       : 1;		    		        // Indicates whether the field x is valid
    uint32  yFlag       : 1;		    		        // Indicates whether the field y is valid
    uint32  zFlag	    : 1;		    		        // Indicates whether the field z is valid
	uint32	Reserved    : 29;
	
	float   x;	                                        // optional, East west location as angle
	float   y;	                                        // optional, North south location as angle
	float   z;	                                        // optional, Offset in meters from the sea level
} onvif_LocalLocation;

typedef struct 
{
    uint32  panFlag     : 1;		    		        // Indicates whether the field pan is valid
    uint32  tiltFlag    : 1;		    		        // Indicates whether the field tilt is valid
    uint32  rollFlag    : 1;		    		        // Indicates whether the field roll is valid
	uint32	Reserved    : 29;
	
	float   pan;	                                    // optional, Rotation around the y axis
	float   tilt;	                                    // optional, Rotation around the z axis
	float   roll;	                                    // optional, Rotation around the x axis
} onvif_LocalOrientation;

typedef struct 
{
    uint32  GeoLocationFlag         : 1;		        // Indicates whether the field GeoLocation is valid
    uint32  GeoOrientationFlag      : 1;		        // Indicates whether the field GeoOrientation is valid
    uint32  LocalLocationFlag       : 1;		        // Indicates whether the field LocalLocation is valid
    uint32  LocalOrientationFlag    : 1;		        // Indicates whether the field LocalOrientation is valid
    uint32  EntityFlag              : 1;		        // Indicates whether the field Entity is valid
    uint32  TokenFlag               : 1;		        // Indicates whether the field Token is valid
    uint32  FixedFlag               : 1;		        // Indicates whether the field Fixed is valid
    uint32  GeoSourceFlag           : 1;		        // Indicates whether the field GeoSource is valid
    uint32  AutoGeoFlag             : 1;		        // Indicates whether the field AutoGeo is valid
	uint32	Reserved                : 23;
	
	onvif_GeoLocation       GeoLocation;	            // optional, Location on earth
	onvif_GeoOrientation    GeoOrientation;	            // optional, Orientation relative to earth
	onvif_LocalLocation     LocalLocation;	            // optional, Indoor location offset
	onvif_LocalOrientation  LocalOrientation;	        // optional, Indoor orientation offset
	
	char    Entity[200];                                // optional, Entity type the entry refers to as defined in tds:Entity
	char    Token[ONVIF_TOKEN_LEN];	                    // optional, Optional entity token
	BOOL    Fixed;	                                    // optional, If this value is true the entity cannot be deleted
	char    GeoSource[256];	                            // optional, Optional reference to the XAddr of another devices DeviceManagement service
	BOOL    AutoGeo;	                                // optional, If set the geo location is obtained internally
} onvif_LocationEntity;

typedef struct _ONVIF_LocationEntity
{
    struct _ONVIF_LocationEntity * next;

    onvif_LocationEntity Location;
} ONVIF_LocationEntity;

typedef struct 
{
	char  * ptr;                                        // required, need call free to free the buffer
	int     size;                                       // required, the ptr buffer length
} onvif_base64Binary;

typedef struct 
{
    uint32  contentTypeFlag : 1;		    	        // Indicates whether the field contentType is valid
	uint32	Reserved        : 31;
	
	onvif_base64Binary Data;	                        // required, base64 encoded binary data
	char    contentType[100];	                        // optional 
} onvif_BinaryData;

typedef struct 
{
    uint32  PasswordFlag    : 1;		    	        // Indicates whether the field Password is valid
	uint32	Reserved        : 31;
	
	char    UserName[64];	                            // required, User name
	char    Password[64];	                            // optional, optional password
} onvif_UserCredential;

typedef struct 
{
    uint32  LocalPathFlag   : 1;		    	        // Indicates whether the field LocalPath is valid
    uint32  StorageUriFlag  : 1;		    	        // Indicates whether the field StorageUri is valid
    uint32  UserFlag        : 1;		    	        // Indicates whether the field User is valid
	uint32	Reserved        : 29;
	
	char    LocalPath[256];	                            // optional, local path
	char    StorageUri[256];	                        // optional, Storage server address
	
	onvif_UserCredential    User;	                    // optional, User credential for the storage server
	
	char    type[100];	                                // required, StorageType lists the acceptable values for type attribute
} onvif_StorageConfigurationData;

typedef struct 
{
	char    token[ONVIF_TOKEN_LEN];	                    // required
	onvif_StorageConfigurationData  Data;	            // required
} onvif_StorageConfiguration;

typedef struct _ONVIF_StorageConfiguration
{
    struct _ONVIF_StorageConfiguration *next;

    onvif_StorageConfiguration Configuration;
} ONVIF_StorageConfiguration;

typedef struct 
{
	onvif_TransportProtocol Protocol;				    // required, Defines the network protocol for streaming, either UDP=RTP/UDP, RTSP=RTP/RTSP/TCP or HTTP=RTP/RTSP/HTTP/TCP 
} onvif_Transport;

typedef struct 
{
	onvif_StreamType	Stream;						    // required, Defines if a multicast or unicast stream is requested
	onvif_Transport 	Transport;					    // required 
} onvif_StreamSetup;

typedef struct
{
	char 	SourceId[128];							    // required, Identifier for the source chosen by the client that creates the structure.
													    //	This identifier is opaque to the device. Clients may use any type of URI for this field. A device shall support at least 128 characters
	char 	Name[64];								    // required, Informative user readable name of the source, e.g. "Camera23". A device shall support at least 20 characters
	char 	Location[100];							    // required, Informative description of the physical location of the source, e.g. the coordinates on a map
	char 	Description[128];						    // required, Informative description of the source
	char 	Address[128];							    // required, URI provided by the service supplying data to be recorded. A device shall support at least 128 characters
} onvif_RecordingSourceInformation;

typedef struct
{
	uint32  MaximumRetentionTimeFlag : 1;			    // Indicates whether the field MaximumRetentionTime is valid
	uint32  Reserved 				 : 31;
	
	onvif_RecordingSourceInformation	Source;		    // required, Information about the source of the recording
	char 	Content[256];							    // required, Informative description of the source
	uint32 	MaximumRetentionTime;					    // optional, specifies the maximum time that data in any track within the
													    //	recording shall be stored. The device shall delete any data older than the maximum retention
													    //	time. Such data shall not be accessible anymore. If the MaximumRetentionPeriod is set to 0,
													    //	the device shall not limit the retention time of stored data, except by resource constraints.
													    //	Whatever the value of MaximumRetentionTime, the device may automatically delete
													    //	recordings to free up storage space for new recordings.
													    //  unit is second
} onvif_RecordingConfiguration;

typedef struct 
{
	onvif_TrackType	TrackType;						    // required, Type of the track. It shall be equal to the strings "Video", "Audio" or "Metadata"
	
	char 	Description[100];						    // required, Informative description of the track
} onvif_TrackConfiguration;

typedef struct 
{
	uint32  TypeFlag : 1;							    // Indicates whether the field Type is valid
	uint32  Reserved : 31;
	
	char 	Token[ONVIF_TOKEN_LEN];					    // required,
	char 	Type[256];								    // optional, default is "http://www.onvif.org/ver10/schema/Receiver", "http://www.onvif.org/ver10/schema/Profile"
} onvif_SourceReference;

typedef struct 
{
	char 	SourceTag[64];							    // required, If the received RTSP stream contains multiple tracks of the same type, the
													    //	SourceTag differentiates between those Tracks. This field can be ignored in case of recording a local source
	char 	Destination[ONVIF_TOKEN_LEN];			    // required, The destination is the tracktoken of the track to which the device shall store the received data
} onvif_RecordingJobTrack;

typedef struct 
{
	uint32  SourceTokenFlag 		: 1;			    // Indicates whether the field SourceToken is valid
	uint32  AutoCreateReceiverFlag 	: 1;			    // Indicates whether the field AutoCreateReceiver is valid
	uint32  Reserved 				: 30;
	
	onvif_SourceReference	SourceToken;			    // optional, This field shall be a reference to the source of the data. The type of the source
													    //	is determined by the attribute Type in the SourceToken structure. If Type is
													    //	http://www.onvif.org/ver10/schema/Receiver, the token is a ReceiverReference. In this case
													    //	the device shall receive the data over the network. If Type is
													    //	http://www.onvif.org/ver10/schema/Profile, the token identifies a media profile, instructing the
													    //	device to obtain data from a profile that exists on the local device
	BOOL 	AutoCreateReceiver;						    // optional, If this field is TRUE, and if the SourceToken is omitted, the device
													    //	shall create a receiver object (through the receiver service) and assign the
													    //	ReceiverReference to the SourceToken field. When retrieving the RecordingJobConfiguration
													    //	from the device, the AutoCreateReceiver field shall never be present

	int 	sizeTracks;
	
	onvif_RecordingJobTrack	Tracks[5];				    // optional, List of tracks associated with the recording
} onvif_RecordingJobSource;

typedef struct 
{
	char 	RecordingToken[ONVIF_TOKEN_LEN];		    // required, Identifies the recording to which this job shall store the received data
	char 	Mode[16];								    // required, The mode of the job. If it is idle, nothing shall happen. If it is active, the device shall try to obtain data from the receivers. 
													    //	A client shall use GetRecordingJobState to determine if data transfer is really taking place
													    //   The only valid values for Mode shall be "Idle" or "Active"
	int 	Priority;								    // required, This shall be a non-negative number. If there are multiple recording jobs that store data to
													    //	the same track, the device will only store the data for the recording job with the highest
													    //	priority. The priority is specified per recording job, but the device shall determine the priority
													    //	of each track individually. If there are two recording jobs with the same priority, the device
													    //	shall record the data corresponding to the recording job that was activated the latest
	int 	sizeSource;
	
	onvif_RecordingJobSource	Source[5];			    // optional, Source of the recording
} onvif_RecordingJobConfiguration;

typedef struct 
{
	uint32  ErrorFlag	: 1;						    // Indicates whether the field Error is valid
	uint32  Reserved	: 31;
	
	char 	SourceTag[64];							    // required, Identifies the track of the data source that provides the data
	char 	Destination[ONVIF_TOKEN_LEN];			    // required, Indicates the destination track
	char 	Error[100];								    // optional, Optionally holds an implementation defined string value that describes the error. The string should be in the English language
	char 	State[16];								    // required, Provides the job state of the track. 
													    //	The valid values of state shall be "Idle", "Active" and "Error". If state equals "Error", the Error field may be filled in with an implementation defined value
} onvif_RecordingJobStateTrack;

typedef struct 
{
	onvif_SourceReference	SourceToken;			    // required, Identifies the data source of the recording job
	char 	State[16];								    // required, Holds the aggregated state over all substructures of RecordingJobStateSource
													    // 	Idle : All state values in sub-nodes are "Idle"
													    // 	PartiallyActive : The state of some sub-nodes are "active" and some sub-nodes are "idle"
													    //	Active : The state of all sub-nodes is "Active"
													    //	Error : At least one of the sub-nodes has state "Error"
	int 	sizeTrack;
	
	onvif_RecordingJobStateTrack	Track[5];		    // optional, 
} onvif_RecordingJobStateSource;

typedef struct 
{
	char 	RecordingToken[ONVIF_TOKEN_LEN];		    // required, Identification of the recording that the recording job records to
	char 	State[16];								    // required, Holds the aggregated state over the whole RecordingJobInformation structure
													    // 	Idle : All state values in sub-nodes are "Idle"
													    // 	PartiallyActive : The state of some sub-nodes are "active" and some sub-nodes are "idle"
													    //	Active : The state of all sub-nodes is "Active"
													    //	Error : At least one of the sub-nodes has state "Error"
	
	int 	sizeSources;
	
	onvif_RecordingJobStateSource	Sources[5];		    // optional, Identifies the data source of the recording job
} onvif_RecordingJobStateInformation;

typedef struct
{
	uint32  SpareFlag				: 1;			    // Indicates whether the field Spare is valid
	uint32  CompatibleSourcesFlag	: 1;			    // Indicates whether the field CompatibleSources is valid
	uint32  Reserved				: 30;
	
	int 	Spare;									    // optional, Number of spare jobs that can be created for the recording
	char 	CompatibleSources[160];					    // optional, A device that supports recording of a restricted set of Media Service Profiles returns the list of profiles that can be recorded on the given Recording
} onvif_JobOptions;

typedef struct
{
	uint32  SpareTotalFlag		: 1;				    // Indicates whether the field SpareTotal is valid
	uint32  SpareVideoFlag		: 1;				    // Indicates whether the field SpareVideo is valid
	uint32  SpareAudioFlag		: 1;				    // Indicates whether the field SpareAudio is valid
	uint32  SpareMetadataFlag	: 1;				    // Indicates whether the field SpareMetadata is valid
	uint32  Reserved			: 28;
	
	int 	SpareTotal;								    // optional, Total spare number of tracks that can be added to this recording
	int 	SpareVideo;								    // optional, Number of spare Video tracks that can be added to this recording
	int 	SpareAudio;								    // optional, Number of spare Aduio tracks that can be added to this recording
	int 	SpareMetadata;							    // optional, Number of spare Metadata tracks that can be added to this recording
} onvif_TrackOptions;

typedef struct
{
	onvif_JobOptions	Job;						    // required, 
	onvif_TrackOptions	Track;						    // required, 
} onvif_RecordingOptions;

typedef struct
{
	char 	TrackToken[ONVIF_TOKEN_LEN];			    // required
	
	onvif_TrackConfiguration 	Configuration;		    // required
} onvif_Track;

typedef struct _ONVIF_Track
{
	struct _ONVIF_Track * next;

	onvif_Track	Track;
} ONVIF_Track;

typedef struct 
{
	char 	RecordingToken[ONVIF_TOKEN_LEN];		    // required
	
	onvif_RecordingConfiguration 	Configuration;	    // required
	
	ONVIF_Track	* Tracks;
} onvif_Recording;

typedef struct
{
	char 	JobToken[ONVIF_TOKEN_LEN];				    // required
	
	onvif_RecordingJobConfiguration	JobConfiguration;	// required
} onvif_RecordingJob;

typedef struct 
{
	char 	Name[32];									// required, Item name
	char 	Value[ONVIF_TOKEN_LEN];						// required, Item value. The type is defined in the corresponding description
} onvif_SimpleItem;

typedef struct _ONVIF_SimpleItem
{
	struct _ONVIF_SimpleItem * next;

	onvif_SimpleItem SimpleItem;						// Value name pair as defined by the corresponding description
} ONVIF_SimpleItem;

typedef struct
{
    uint32  AnyFlag   : 1;
    uint32  Reserverd : 31;
    
	char 	Name[32];									// required, Item name
	char  * Any;                                        // optional
} onvif_ElementItem;

typedef struct _ONVIF_ElementItem
{
	struct _ONVIF_ElementItem * next;

	onvif_ElementItem ElementItem;						// Value name pair as defined by the corresponding description
} ONVIF_ElementItem;

typedef struct 
{
	ONVIF_SimpleItem	* SimpleItem;					// optional
	ONVIF_ElementItem	* ElementItem;					// optional
} onvif_ItemList;

typedef struct
{
	uint32  PropertyOperationFlag	: 1;				// Indicates whether the field PropertyOperation is valid
	uint32  SourceFlag				: 1;				// Indicates whether the field Source is valid
	uint32  KeyFlag					: 1;				// Indicates whether the field Key is valid
	uint32  DataFlag				: 1;				// Indicates whether the field Data is valid
	uint32  Reserved				: 28;
	
	time_t UtcTime;										// required
	
	onvif_ItemList	Source;								// optional, Token value pairs that triggered this message. Typically only one item is present
	onvif_ItemList	Key;								// optional element of type tt:ItemList */
	onvif_ItemList	Data;								// optional element of type tt:ItemList */
	
	onvif_PropertyOperation	PropertyOperation;			// optional 
} onvif_Message;

typedef struct
{
	char	Dialect[256];								// required, 
	char	Topic[256];									// required, 

	onvif_Message	Message;							// required
} onvif_NotificationMessage;

typedef struct 
{
	time_t 	DataFrom;									// required, The earliest point in time where there is recorded data on the device
	time_t 	DataUntil;									// required, The most recent point in time where there is recorded data on the device
	int 	NumberRecordings;							// required, The device contains this many recordings
} onvif_RecordingSummary;

typedef struct 
{
	char 	TrackToken[ONVIF_TOKEN_LEN];				// required, 
	
	onvif_TrackType TrackType;							// required, Type of the track: "Video", "Audio" or "Metadata".
														//	The track shall only be able to hold data of that type
	
	char 	Description[100];							// required, Informative description of the contents of the track
	time_t 	DataFrom;									// required, The start date and time of the oldest recorded data in the track
	time_t 	DataTo;										// required, The stop date and time of the newest recorded data in the track
} onvif_TrackInformation;

typedef struct 
{
	uint32  EarliestRecordingFlag	: 1;				// Indicates whether the field EarliestRecording is valid
	uint32  LatestRecordingFlag		: 1;				// Indicates whether the field LatestRecording is valid
	uint32  Reserved				: 30;
	
	char 	RecordingToken[ONVIF_TOKEN_LEN];			// required, 
	
	onvif_RecordingSourceInformation	Source;			// required, Information about the source of the recording
	
	time_t	EarliestRecording;							// optional, 
	time_t	LatestRecording;							// optional, 
	char	Content[256];								// required, 

	int 	sizeTrack;	
	onvif_TrackInformation	Track[5];					// optional, Basic information about the track. Note that a track may represent a single contiguous time span or consist of multiple slices
	
	onvif_RecordingStatus	RecordingStatus;			// required, 
} onvif_RecordingInformation;

typedef struct 
{
	uint32  BitrateFlag	: 1;							// Indicates whether the field Bitrate is valid
	uint32  Reserved	: 31;
	
	int 	Bitrate;									// optional, Average bitrate in kbps
	int 	Width;										// required, The width of the video in pixels
	int 	Height;										// required, The height of the video in pixels

	onvif_VideoEncoding Encoding;						// required, Used video codec, either Jpeg, H.264 or Mpeg4

	float 	Framerate;									// required, Average framerate in frames per second
} onvif_VideoAttributes;

typedef struct 
{
	uint32  BitrateFlag	: 1;							// Indicates whether the field Bitrate is valid
	uint32  Reserved	: 31;
	
	int 	Bitrate;									// optional, The bitrate in kbps

	onvif_AudioEncoding Encoding;						// required, Audio codec used for encoding the audio (either G.711, G.726 or AAC)

	int 	Samplerate;									// required, The sample rate in kHz
} onvif_AudioAttributes;

typedef struct 
{
	uint32  PtzSpacesFlag	: 1;						// Indicates whether the field PtzSpaces is valid
	uint32  Reserved		: 31;
	
	BOOL 	CanContainPTZ;								// required, Indicates that there can be PTZ data in the metadata track in the specified time interval
	BOOL 	CanContainAnalytics;						// required, Indicates that there can be analytics data in the metadata track in the specified time interval
	BOOL 	CanContainNotifications;					// required, Indicates that there can be notifications in the metadata track in the specified time interval
	char 	PtzSpaces[256];								// optional, List of all PTZ spaces active for recording. Note that events are only recorded on position changes and 
														//	the actual point of recording may not necessarily contain an event of the specified type
} onvif_MetadataAttributes;

typedef struct 
{
	uint32  VideoAttributesFlag		: 1;				// Indicates whether the field VideoAttributes is valid
	uint32  AudioAttributesFlag		: 1;				// Indicates whether the field AudioAttributes is valid
	uint32  MetadataAttributesFlag	: 1;				// Indicates whether the field MetadataAttributes is valid
	uint32  Reserved				: 29;
	
	onvif_TrackInformation		TrackInformation;		// required, The basic information about the track. Note that a track may represent a single contiguous time span or consist of multiple slices
	onvif_VideoAttributes		VideoAttributes;		// optional, If the track is a video track, exactly one of this structure shall be present and contain the video attributes
	onvif_AudioAttributes		AudioAttributes;		// optional, If the track is an audio track, exactly one of this structure shall be present and contain the audio attributes
	onvif_MetadataAttributes	MetadataAttributes;		// optional, If the track is an metadata track, exactly one of this structure shall be present and contain the metadata attributes
} onvif_TrackAttributes;

typedef struct _ONVIF_TrackAttributes
{
	struct _ONVIF_TrackAttributes * next;
	
	onvif_TrackAttributes	TrackAttributes;
} ONVIF_TrackAttributes;

typedef struct 
{
	char 	RecordingToken[ONVIF_TOKEN_LEN];			// required, A reference to the recording that has these attributes
	
	int 	sizeTrackAttributes;
	onvif_TrackAttributes	TrackAttributes[5];			// optional, A set of attributes for each track

	time_t 	From;										// required, The attributes are valid from this point in time in the recording
	time_t 	Until;										// required, The attributes are valid until this point in time in the recording. 
														//	Can be equal to 'From' to indicate that the attributes are only known to be valid for this particular point in time
} onvif_MediaAttributes;

typedef struct 
{
	uint32  RecordingInformationFilterFlag	: 1;		// Indicates whether the field RecordingInformationFilter is valid
	uint32  Reserved						: 31;
	
	int 	sizeIncludedSources;
	onvif_SourceReference	IncludedSources[10];		// optional, A list of sources that are included in the scope. If this list is included, only data from one of these sources shall be searched
	
	int 	sizeIncludedRecordings;
	char 	IncludedRecordings[10][ONVIF_TOKEN_LEN];	// optional, A list of recordings that are included in the scope. If this list is included, only data from one of these recordings shall be searched

	char 	RecordingInformationFilter[128];			// optional, An xpath expression used to specify what recordings to search. 
														//	Only those recordings with an RecordingInformation structure that matches the filter shall be searched
} onvif_SearchScope;

typedef struct 
{
	onvif_PTZVector MinPosition;	                    // required, 
	onvif_PTZVector MaxPosition;	                    // required, 
	
	BOOL    EnterOrExit;	                            // required, 
} onvif_PTZPositionFilter;

typedef struct
{
	char    MetadataStreamFilter[100];	                // required
} onvif_MetadataFilter;

typedef struct _ONVIF_RecordingInformation
{
	struct _ONVIF_RecordingInformation * next;

	onvif_RecordingInformation	RecordingInformation;
} ONVIF_RecordingInformation;

typedef struct 
{
	onvif_SearchState SearchState;						// required, The state of the search when the result is returned. Indicates if there can be more results, or if the search is completed
	
	ONVIF_RecordingInformation * RecordInformation;     // optional, A RecordingInformation structure for each found recording matching the search
} onvif_FindRecordingResultList;

typedef struct 
{
	char 	RecordingToken[ONVIF_TOKEN_LEN];			// required, The recording where this event was found. Empty string if no recording is associated with this event
	char 	TrackToken[ONVIF_TOKEN_LEN];				// required, A reference to the track where this event was found. Empty string if no track is associated with this event
	time_t 	Time;										// required, The time when the event occured

	onvif_NotificationMessage 	Event;					// required, The description of the event

	BOOL 	StartStateEvent;							// required, If true, indicates that the event is a virtual event generated for this particular search session to give the state of a property at the start time of the search
} onvif_FindEventResult;

typedef struct _ONVIF_FindEventResult
{
	struct _ONVIF_FindEventResult * next;

	onvif_FindEventResult	FindEventResult;
} ONVIF_FindEventResult;

typedef struct 
{
	onvif_SearchState SearchState;						// required, The state of the search when the result is returned. Indicates if there can be more results, or if the search is completed

	ONVIF_FindEventResult * Result;				        // optional
} onvif_FindEventResultList;

typedef struct 
{
	char 	RecordingToken[ONVIF_TOKEN_LEN];			// required, The recording where this event was found. Empty string if no recording is associated with this event
	char 	TrackToken[ONVIF_TOKEN_LEN];				// required, A reference to the track where this event was found. Empty string if no track is associated with this event
	time_t 	Time;										// required, The time when the event occured
} onvif_FindMetadataResult;

typedef struct _ONVIF_FindMetadataResult
{
    struct _ONVIF_FindMetadataResult * next;

	onvif_FindMetadataResult    Result;
} ONVIF_FindMetadataResult;

typedef struct 
{
    onvif_SearchState SearchState;	                    // required, The state of the search when the result is returned. Indicates if there can be more results, or if the search is completed

	ONVIF_FindMetadataResult * Result;	                // optional
} onvif_FindMetadataResultList;

typedef struct 
{
	char    RecordingToken[ONVIF_TOKEN_LEN];	        // required
	char    TrackToken[ONVIF_TOKEN_LEN];	            // required
	time_t  Time;	                                    // required

	onvif_PTZVector Position;	                        // required
} onvif_FindPTZPositionResult;

typedef struct _ONVIF_FindPTZPositionResult
{
    struct _ONVIF_FindPTZPositionResult * next;

    onvif_FindPTZPositionResult FindPTZPositionResult;
} ONVIF_FindPTZPositionResult;

typedef struct 
{
	onvif_SearchState SearchState;						// required, The state of the search when the result is returned. Indicates if there can be more results, or if the search is completed

	ONVIF_FindPTZPositionResult * Result;			    // optional
} onvif_FindPTZPositionResultList;


//////////////////////////////////////////////////////////////////////////
//	Video analytics struct defines
//////////////////////////////////////////////////////////////////////////

typedef struct 
{
    uint32   attrFlag : 1;
    uint32   reserved : 31;
    
	onvif_ItemList	Parameters;							// required
	
	char 	Name[32];									// required
	char 	Type[32];									// required
	char    attr[200];
} onvif_Config;

typedef struct _ONVIF_Config
{
	struct _ONVIF_Config * next;

	onvif_Config	Config;
} ONVIF_Config;

typedef struct 
{
	ONVIF_Config * AnalyticsModule;						// optional
} onvif_AnalyticsEngineConfiguration;

typedef struct 
{
	ONVIF_Config * Rule;								// optional
} onvif_RuleEngineConfiguration;

typedef struct 
{
	char 	Name[ONVIF_NAME_LEN];						// required
	int 	UseCount;									// required
	char 	token[ONVIF_TOKEN_LEN];						// required

	onvif_AnalyticsEngineConfiguration 	AnalyticsEngineConfiguration;	// required
	onvif_RuleEngineConfiguration 		RuleEngineConfiguration;		// required 
} onvif_VideoAnalyticsConfiguration;

typedef struct
{
	char 	Name[64];									// required
	char 	Type[64];									// required
} onvif_SimpleItemDescription;

typedef struct _ONVIF_SimpleItemDescription
{
	struct _ONVIF_SimpleItemDescription * next;

	onvif_SimpleItemDescription	SimpleItemDescription;
} ONVIF_SimpleItemDescription;

typedef struct 
{
	ONVIF_SimpleItemDescription	* SimpleItemDescription;
	ONVIF_SimpleItemDescription	* ElementItemDescription;
} onvif_ItemListDescription;

typedef struct 
{
	uint32  SourceFlag		: 1;						// Indicates whether the field Source is valid
	uint32  KeyFlag			: 1;						// Indicates whether the field Source is valid
	uint32  DataFlag		: 1;						// Indicates whether the field Source is valid
	uint32  IsPropertyFlag	: 1;						// Indicates whether the field Source is valid
	uint32  Reserved		: 28;
	
	onvif_ItemListDescription	Source;					// optional 
	onvif_ItemListDescription	Key;					// optional 
	onvif_ItemListDescription	Data;					// optional 

	BOOL	IsProperty;									// optional 
	char	ParentTopic[100];							// required 
} onvif_ConfigDescription_Messages;

typedef struct _ONVIF_ConfigDescription_Messages
{
	struct _ONVIF_ConfigDescription_Messages * next;

	onvif_ConfigDescription_Messages Messages;
} ONVIF_ConfigDescription_Messages;

typedef struct 
{
    uint32  fixedFlag		    : 1;				    // Indicates whether the field fixed is valid
	uint32  maxInstancesFlag    : 1;				    // Indicates whether the field maxInstances is valid
	uint32  Reserved		    : 30;
	
	onvif_ItemListDescription 	Parameters;				// required 

	ONVIF_ConfigDescription_Messages * Messages;		

	char 	Name[ONVIF_NAME_LEN];						// required 
	BOOL    fixed;                                      // optional
	int     maxInstances;                               // optional
} onvif_ConfigDescription;

typedef struct 
{
    uint32  RuleTypeFlag    : 1;			            // Indicates whether the field RuleType is valid
	uint32  Reserved        : 31;
	
    char    RuleType[100];	                            // optional, 
	char    Name[ONVIF_NAME_LEN];	                    // required, The Name of the SimpleItemDescription/ElementItemDescription
							                            //  the ConfigOptions applies to
	char    Type[64];	                                // required, Type of the Rule Options represented by a unique QName. 
							                            //  The Type defines the element contained in this structure
    char  * any;							                            
} onvif_ConfigOptions;

typedef struct _ONVIF_ConfigOptions
{
    struct _ONVIF_ConfigOptions	* next;

    onvif_ConfigOptions Options;
} ONVIF_ConfigOptions;

typedef struct _ONVIF_ConfigDescription
{
	struct _ONVIF_ConfigDescription * next;

	onvif_ConfigDescription	ConfigDescription;

	ONVIF_ConfigOptions * RuleOptions;
} ONVIF_ConfigDescription;

typedef struct 
{
	int 	sizeRuleContentSchemaLocation;
	char 	RuleContentSchemaLocation[10][256];			// optional

	ONVIF_ConfigDescription	* RuleDescription;
} onvif_SupportedRules;

typedef struct
{
    int     sizeAnalyticsModuleContentSchemaLocation;	// sequence of elements <AnalyticsModuleContentSchemaLocation>
	char    AnalyticsModuleContentSchemaLocation[10][128];  // It optionally contains a list of URLs that provide the location of schema files
	                                                        //  These schema files describe the types and elements used in the analytics module descriptions.
                                                            //  If the analytics module descriptions reference types or elements of the ONVIF schema file,
                                                            //  the ONVIF schema file MUST be explicitly listed
	
	ONVIF_ConfigDescription * AnalyticsModuleDescription;   // optional, 
} onvif_SupportedAnalyticsModules;

typedef struct 
{
	char    Type[128];	                                // required, Type of the Analytics Module Options represented by a unique QName. 
							                            //  The Type defines the element contained in this structure
} onvif_AnalyticsModuleConfigOptions;

typedef struct _ONVIF_AnalyticsModuleConfigOptions
{
    struct _ONVIF_AnalyticsModuleConfigOptions	* next;
    
    onvif_AnalyticsModuleConfigOptions  Options;
} ONVIF_AnalyticsModuleConfigOptions;

typedef struct
{
    char    Dialect[128];
    char    Expression[256];
} onvif_EventFilterItem;

typedef struct
{
    int     sizeTopicExpression;
    int     sizeMessageContent;
    
    onvif_EventFilterItem   TopicExpression[5];
    onvif_EventFilterItem   MessageContent[5];
} onvif_EventFilter;

// PROFILE C Define Begin

/**
 * The AccessPoint capabilities reflect optional functionality of a particular physical entity.
 * Different AccessPoint instances may have different set of capabilities. 
 * This information maychange during device operation, e.g. if hardware settings are changed.
 */
typedef struct 
{
	BOOL    DisableAccessPoint;	                        // required, Indicates whether or not this AccessPoint instance supports EnableAccessPoint and DisableAccessPoint commands
	BOOL    Duress;	                                    // optional, Indicates whether or not this AccessPoint instance supports generation of duress events
	BOOL    AnonymousAccess;	                        // optional, Indicates whether or not this AccessPoint has a REX switch or other input that allows anonymous access
	BOOL    AccessTaken;	                            // optional, Indicates whether or not this AccessPoint instance supports generation of AccessTaken and AccessNotTaken events.
	                                                    //  If AnonymousAccess and AccessTaken are both true, it indicates that the Anonymous versions of AccessTaken and AccessNotTaken are supported
	BOOL    ExternalAuthorization;	                    // optional, Indicates whether or not this AccessPoint instance supports the ExternalAuthorization operation and the generation of Request events. 
	                                                    //  If AnonymousAccess and ExternalAuthorization are both true, it indicates that the Anonymous version is supported as well
} onvif_AccessPointCapabilities;

/**
 * The AccessPointInfo structure contains basic information about an AccessPoint instance.
 * An AccessPoint defines an entity a Credential can be granted or denied access to. 
 * TheAccessPointInfo provides basic information on how access is controlled in one direction for adoor (from which area to which area).
 * door is the typical device involved, but other type ofdevices may be supported as well.
 * Multiple AccessPoints may cover the same Door.A typical case is one AccessPoint for entry and another for exit, both referencingthe same Door.
 * An ONVIF compliant device shall provide the following fields for each AccessPoint instance
 */
typedef struct 
{
    uint32  DescriptionFlag : 1;
    uint32  AreaFromFlag    : 1;
    uint32  AreaToFlag      : 1;
    uint32  EntityTypeFlag  : 1;
    uint32  Reserved        : 28;
    
    char    token[ONVIF_TOKEN_LEN];                     // required
    char    Name[ONVIF_NAME_LEN];                       // required, A user readable name. It shall be up to 64 characters
    char    Description[1024];                          // optional, Optional user readable description for the AccessPoint. It shall be up to 1024 characters
    char    AreaFrom[ONVIF_TOKEN_LEN];                  // optional, Optional reference to the Area from which access is requested
    char    AreaTo[ONVIF_TOKEN_LEN];                    // optional, Optional reference to the Area to which access is requested
    char    EntityType[100];                            // optional, Optional entity type; if missing, a Door type as defined by the ONVIF DoorControl service should be assumed. 
                                                        //  This can also be represented by the QName value "tdc:Door" - where tdc is the namespace of the Door Control service. 
                                                        //  This field is provided for future extensions; it will allow an AccessPoint being extended to cover entity types other than Doors as well
    char    Entity[ONVIF_TOKEN_LEN];                    // required, Reference to the entity used to control access; the entity type may be specified by the optional EntityType field explained below but is typically a Door

    onvif_AccessPointCapabilities   Capabilities;       // required, The capabilities for the AccessPoint
} onvif_AccessPointInfo;

typedef struct _ONVIF_AccessPoint
{
    struct _ONVIF_AccessPoint * next;

    BOOL    Enabled;                                    // Indicates that the AccessPoint is enabled. By default this field value shall be True, if the DisableAccessPoint capabilities is not supported
    
    onvif_AccessPointInfo   AccessPointInfo;
} ONVIF_AccessPoint;

typedef struct _ONVIF_AccessPointInfo
{
    struct _ONVIF_AccessPointInfo * next;
    
    onvif_AccessPointInfo   AccessPointInfo;
} ONVIF_AccessPointInfo;

/**
 * DoorCapabilities reflect optional functionality of a particular physical entity.
 * Different door instances may have different set of capabilities.
 * This information may change during device operation, e.g. if hardware settings are changed
 */
typedef struct 
{
	BOOL    Access;	                                    // optional, Indicates whether or not this Door instance supports AccessDoor command to perform momentary access
	BOOL    AccessTimingOverride;	                    // optional, Indicates that this Door instance supports overriding configured timing in the AccessDoor command
	BOOL    Lock;	                                    // optional, Indicates that this Door instance supports LockDoor command to lock the door
	BOOL    Unlock;	                                    // optional, Indicates that this Door instance supports UnlockDoor command to unlock the door
	BOOL    Block;	                                    // optional, Indicates that this Door instance supports BlockDoor command to block the door
	BOOL    DoubleLock;	                                // optional, Indicates that this Door instance supports DoubleLockDoor command to lock multiple locks on the door
	BOOL    LockDown;	                                // optional, Indicates that this Door instance supports LockDown (and LockDownRelease) commands to lock the door and put it in LockedDown mode
	BOOL    LockOpen;	                                // optional, Indicates that this Door instance supports LockOpen (and LockOpenRelease) commands to unlock the door and put it in LockedOpen mode
	BOOL    DoorMonitor;	                            // optional, Indicates that this Door instance has a DoorMonitor and supports the DoorPhysicalState event
	BOOL    LockMonitor;	                            // optional, Indicates that this Door instance has a LockMonitor and supports the LockPhysicalState event
	BOOL    DoubleLockMonitor;	                        // optional, Indicates that this Door instance has a DoubleLockMonitor and supports the DoubleLockPhysicalState event
	BOOL    Alarm;	                                    // optional, Indicates that this Door instance supports door alarm and the DoorAlarm event
	BOOL    Tamper;	                                    // optional, Indicates that this Door instance has a Tamper detector and supports the DoorTamper event
	BOOL    Fault;	                                    // optional, Indicates that this Door instance supports door fault and the DoorFault event
} onvif_DoorCapabilities;

// Tampering information for a Door
typedef struct 
{
    uint32  ReasonFlag : 1;
    uint32  Reserved   : 31;
    
	char    Reason[100];                                // optional, Optional field; Details describing tampering state change (e.g., reason, place and time).
	                                                    //  NOTE: All fields (including this one) which are designed to give end-user prompts can be localized to the customers's native language
	onvif_DoorTamperState State;	                    // required, State of the tamper detector
} onvif_DoorTamper;

// Fault information for a Door
typedef struct 
{
    uint32  ReasonFlag : 1;
    uint32  Reserved   : 31;
    
	char    Reason[100];	                            // optional, Optional reason for fault
	
	onvif_DoorFaultState State;	                        // required, Overall fault state for the door; it is of type DoorFaultState. If there are any faults, the value shall be: FaultDetected. 
	                                                    //  Details of the detected fault shall be found in the Reason field, and/or the various DoorState fields and/or in extensions to this structure
} onvif_DoorFault;

// The DoorState structure contains current aggregate runtime status of Door
typedef struct
{
    uint32  DoorPhysicalStateFlag       : 1;
    uint32  LockPhysicalStateFlag       : 1;
    uint32  DoubleLockPhysicalStateFlag : 1;
    uint32  AlarmFlag                   : 1;
    uint32  TamperFlag                  : 1;
    uint32  FaultFlag                   : 1;
    uint32  Reserved                    : 26;
    
	onvif_DoorPhysicalState DoorPhysicalState;	        // optional, Physical state of Door; it is of type DoorPhysicalState. 
	                                                    //  A device that signals support for DoorMonitor capability for a particular door instance shall provide this field 
	onvif_LockPhysicalState LockPhysicalState;	        // optional, Physical state of the Lock; it is of type LockPhysicalState. 
	                                                    //  A device that signals support for LockMonitor capability for a particular door instance shall provide this field
	onvif_LockPhysicalState DoubleLockPhysicalState;	// optional, Physical state of the DoubleLock; it is of type LockPhysicalState. 
	                                                    //  A device that signals support for DoubleLockMonitor capability for a particular door instance shall provide this field
	onvif_DoorAlarmState    Alarm;	                    // optional, Alarm state of the door; it is of type DoorAlarmState. 
	                                                    //  A device that signals support for Alarm capability for a particular door instance shall provide this field
	onvif_DoorTamper        Tamper;	                    // optional, Tampering state of the door; it is of type DoorTamper.
	                                                    //  A device that signals support for Tamper capability for a particular door instance shall provide this field
	onvif_DoorFault         Fault;	                    // optional, Fault information for door; it is of type DoorFault.
	                                                    //  A device that signals support for Fault capability for a particular door instance shall provide this field
	onvif_DoorMode          DoorMode;	                // required,  The logical operating mode of the door; it is of type DoorMode. An ONVIF compatible device shall report current operating mode in this field
} onvif_DoorState;

/**
 * The DoorInfo type represents the Door as a physical object.
 * The structure contains information and capabilities of a specific door instance
 */
typedef struct 
{
    uint32  DescriptionFlag : 1;
    uint32  Reserved        : 31;
    
	char    token[ONVIF_TOKEN_LEN];	                    // required
	char    Name[ONVIF_NAME_LEN];	                    // required, A user readable name. It shall be up to 64 characters
	char    Description[1024];	                        // optional, A user readable description. It shall be up to 1024 characters

	onvif_DoorCapabilities  Capabilities;	            // required, The capabilities of the Door
} onvif_DoorInfo;

typedef struct _ONVIF_Door
{
    struct _ONVIF_Door * next;

    onvif_DoorState DoorState;     
    onvif_DoorInfo  DoorInfo;
} ONVIF_Door;

typedef struct _ONVIF_DoorInfo
{
    struct _ONVIF_DoorInfo * next;
    
    onvif_DoorInfo  DoorInfo;
} ONVIF_DoorInfo;

/**
 * The AreaInfo structure contains basic information about an Area
 */
typedef struct 
{
    uint32  DescriptionFlag : 1;
    uint32  Reserved        : 31;
    
	char    token[ONVIF_TOKEN_LEN];	                    // required
	char    Name[ONVIF_NAME_LEN];	                    // required, A user readable name. It shall be up to 64 characters
	char    Description[1024];	                        // optional, A user readable description. It shall be up to 1024 characters
} onvif_AreaInfo;

typedef struct _ONVIF_AreaInfo
{
    struct _ONVIF_AreaInfo * next;

    onvif_AreaInfo   AreaInfo;
} ONVIF_AreaInfo;


// PROFILE C Define End

// DEVICEIO Define Begin

// A pane layout describes one Video window of a display. It links a pane configuration to a region of the screen
typedef struct 
{
	char 	Pane[ONVIF_TOKEN_LEN];						// required, Reference to the configuration of the streaming and coding parameters

	onvif_Rectangle 	Area;							// required, Describes the location and size of the area on the monitor. 
														//	The area coordinate values are espressed in normalized units [-1.0, 1.0]
} onvif_PaneLayout;

typedef struct _ONVIF_PaneLayout
{
	struct _ONVIF_PaneLayout * next;

	onvif_PaneLayout	PaneLayout;						// required
} ONVIF_PaneLayout;

// A layout describes a set of Video windows that are displayed simultaniously on a display
typedef struct 
{
	ONVIF_PaneLayout * 	PaneLayout;						// required, List of panes assembling the display layout
} onvif_Layout;

// Representation of a physical video outputs
typedef struct 
{
	uint32  ResolutionFlag 	: 1;						// Indicates whether the field Resolution is valid
	uint32  RefreshRateFlag : 1;						// Indicates whether the field RefreshRate is valid
	uint32  AspectRatioFlag : 1;						// Indicates whether the field AspectRatio is valid
    uint32  Reserved        : 29;						
    
	char 	token[ONVIF_TOKEN_LEN];						// required, 

	onvif_Layout 			Layout;						// required, 
	onvif_VideoResolution 	Resolution;					// optional, Resolution of the display in Pixel

	float 	RefreshRate;								// optional, Refresh rate of the display in Hertz
	float 	AspectRatio;								// optional, Aspect ratio of the display as physical extent of width divided by height
} onvif_VideoOutput;

typedef struct _ONVIF_VideoOutput
{
	struct _ONVIF_VideoOutput * next;

	onvif_VideoOutput	VideoOutput;
} ONVIF_VideoOutput;

typedef struct 
{
	char 	Name[ONVIF_NAME_LEN];						// required
	int 	UseCount;									// required
	char 	token[ONVIF_TOKEN_LEN];						// required
	char 	OutputToken[ONVIF_TOKEN_LEN];				// required
} onvif_VideoOutputConfiguration;

typedef struct _ONVIF_VideoOutputConfiguration
{
    struct _ONVIF_VideoOutputConfiguration * next;

    onvif_VideoOutputConfiguration  Configuration;
} ONVIF_VideoOutputConfiguration;

typedef struct
{
    char 	token[ONVIF_TOKEN_LEN];						// required, 
} onvif_AudioOutput;

typedef struct _ONVIF_AudioOutput
{
    struct _ONVIF_AudioOutput  * next;
    
    onvif_AudioOutput   AudioOutput;
} ONVIF_AudioOutput;

typedef struct 
{
	int     sizeOutputTokensAvailable;	                
	char    OutputTokensAvailable[5][ONVIF_TOKEN_LEN];	// required, Tokens of the physical Audio outputs (typically one)
	int     sizeSendPrimacyOptions;
	char    SendPrimacyOptions[5][100];	                // optional, The following modes for the Send-Primacy are defined:
							                            //  www.onvif.org/ver20/HalfDuplex/Server
							                            //  www.onvif.org/ver20/HalfDuplex/Client
							                            //  www.onvif.org/ver20/HalfDuplex/Auto

	onvif_IntRange  OutputLevelRange;	                // required, Minimum and maximum level range supported for this Output
} onvif_AudioOutputConfigurationOptions;

typedef struct 
{
    uint32  SendPrimacyFlag : 1;						// Indicates whether the field SendPrimacy is valid
    uint32  Reserved        : 31;
    
	char    Name[ONVIF_NAME_LEN];	                    // required, 
	int     UseCount;	                                // required, 
	char    token[ONVIF_TOKEN_LEN];	                    // required, 
	char    OutputToken[ONVIF_TOKEN_LEN];	            // required, Token of the phsycial Audio output
	char    SendPrimacy[100];	                        // optional, The following modes for the Send-Primacy are defined:
	                                                    //  www.onvif.org/ver20/HalfDuplex/Server
	                                                    //  www.onvif.org/ver20/HalfDuplex/Client
	                                                    //  www.onvif.org/ver20/HalfDuplex/Auto
	int     OutputLevel;	                            // required, Volume setting of the output. The applicable range is defined via the option AudioOutputOptions.OutputLevelRange
} onvif_AudioOutputConfiguration;

typedef struct _ONVIF_AudioOutputConfiguration
{
    struct _ONVIF_AudioOutputConfiguration * next;

    onvif_AudioOutputConfigurationOptions   Options;
    onvif_AudioOutputConfiguration          Configuration;
} ONVIF_AudioOutputConfiguration;

typedef struct 
{
	onvif_RelayMode Mode;	                            // required, 'Bistable' or 'Monostable'
	uint32  DelayTime;	                                // external, Time after which the relay returns to its idle state if it is in monostable mode. 
	                                                    //  If the Mode field is set to bistable mode the value of the parameter can be ignored
	onvif_RelayIdleState IdleState;	                    // required, 'open' or 'closed'
} onvif_RelayOutputSettings;

typedef struct 
{
    uint32 RelayMode_BistableFlag   : 1;
    uint32 RelayMode_MonostableFlag : 1;    
    uint32 DelayTimesFlag           : 1;
    uint32 DiscreteFlag             : 1;
    uint32 Reserved                 : 28;
    
	char    DelayTimes[100];	                        // optional, Supported delay time range or discrete values in seconds. This element must be present if MonoStable mode is supported.
	BOOL    Discrete;	                                // optional, True if the relay only supports the exact values for the DelayTimes listed. Default is false
	char    token[ONVIF_TOKEN_LEN];	                    // required, Token of the relay output
} onvif_RelayOutputOptions;

typedef struct 
{
	char    token[ONVIF_TOKEN_LEN];	                    // required

	onvif_RelayOutputSettings   Properties;	            // required
} onvif_RelayOutput;

typedef struct _ONVIF_RelayOutput
{
    struct _ONVIF_RelayOutput * next;

    onvif_RelayOutputOptions    Options;
    onvif_RelayLogicalState     RelayLogicalState;     
    onvif_RelayOutput           RelayOutput;
} ONVIF_RelayOutput;

typedef struct 
{
    uint32 IdleStateFlag    : 1;
    uint32 Reserved         : 31;
    
	char    token[ONVIF_TOKEN_LEN];	                    // required

	onvif_DigitalIdleState  IdleState;	                // optional, Indicate the Digital IdleState status
} onvif_DigitalInput;

typedef struct 
{
    uint32  DigitalIdleState_closedFlag : 1;
    uint32  DigitalIdleState_openFlag   : 1;
    uint32  Reserved                    : 30;
} onvif_DigitalInputConfigurationInputOptions;

typedef struct _ONVIF_DigitalInput
{
    struct _ONVIF_DigitalInput * next;

    onvif_DigitalInputConfigurationInputOptions Options;
    
    onvif_DigitalInput  DigitalInput;
} ONVIF_DigitalInput;

typedef struct 
{
	int     BaudRate;	                                // required, The transfer bitrate	
	int     CharacterLength;	                        // required, The bit length for each character
	float   StopBit;	                                // required, The number of stop bits used to terminate each character
	char    token[ONVIF_TOKEN_LEN];	                    // required,

	onvif_ParityBit         ParityBit;	                // required, The parity for the data error detection
	onvif_SerialPortType    type;	                    // required,
} onvif_SerialPortConfiguration;

typedef struct 
{
	onvif_IntList           BaudRateList;	            // required, The list of configurable transfer bitrate
	onvif_ParityBitList     ParityBitList;	            // required, The list of configurable parity for the data error detection
	onvif_IntList           CharacterLengthList;        // required, The list of configurable bit length for each character
	onvif_FloatList         StopBitList;	            // required, The list of configurable number of stop bits used to terminate each character

	char    token[ONVIF_TOKEN_LEN];	                    // required, 
} onvif_SerialPortConfigurationOptions;

typedef struct 
{
	char    token[ONVIF_TOKEN_LEN];	                    // required
} onvif_SerialPort;

typedef struct _ONVIF_SerialPort
{
    struct _ONVIF_SerialPort * next;

    onvif_SerialPortConfiguration           Configuration;
    onvif_SerialPortConfigurationOptions    Options;
    onvif_SerialPort                        SerialPort;
} ONVIF_SerialPort;

typedef union 
{
	char   * Binary;
	char   * String;
} onvif_union_SerialData;

typedef struct 
{
    int     _union_SerialData;                          // 0 - Binary; 1 - String
    
	onvif_union_SerialData  union_SerialData;
} onvif_SerialData;

typedef struct 
{
    uint32  SerialDataFlag  : 1;
    uint32  TimeOutFlag     : 1;
    uint32  DataLengthFlag  : 1;
    uint32  DelimiterFlag   : 1;
    uint32  Reserved        : 28;
    
	onvif_SerialData    SerialData;	                    // optional, The serial port data

	uint32  TimeOut;	                                // optional, Indicates that the command should be responded back within the specified period of time
	int     DataLength;	                                // optional, This element may be put in the case that data length returned from the connected serial device is already determined as some fixed bytes length. 
	                                                    //  It indicates the length of received data which can be regarded as available
	char    Delimiter[100];	                            // optional, This element may be put in the case that the delimiter codes returned from the connected serial device is already known. 
	                                                    //  It indicates the termination data sequence of the responded data. In case the string has more than one character a device shall interpret the whole string as a single delimiter. 
	                                                    //  Furthermore a device shall return the delimiter character(s) to the client
} onvif_SendReceiveSerialCommand;


// DEVICEIO Define End

// MEDIA2 Define Begin

typedef struct 
{
    uint32  GovLengthRangeFlag           : 1;           // Indicates whether the field GovLengthRange is valid
    uint32  FrameRatesSupportedFlag      : 1;           // Indicates whether the field FrameRatesSupported is valid
    uint32  ProfilesSupportedFlag        : 1;           // Indicates whether the field ProfilesSupported is valid
    uint32  ConstantBitRateSupportedFlag : 1;           // Indicates whether the field ConstantBitRateSupported is valid
    uint32  Reserved                     : 28;
    
	char    Encoding[64];	                            // required, Mime name of the supported Video format
	                                                    //  JPEG, MPV4-ES, H264, H265

    onvif_VideoEncoding     VideoEncoding;              // media server 1 field

	onvif_IntRange          QualityRange;	            // required, Range of the quality values. A high value means higher quality
	onvif_VideoResolution	ResolutionsAvailable[MAX_RES_NUMS];	// required, List of supported image sizes
	onvif_IntRange          BitrateRange;	            // required, Supported range of encoded bitrate in kbps

	char    GovLengthRange[100];	                    // optional, Lower and Upper bounds for the supported group of Video frames length. 
	                                                    //  This value typically corresponds to the I-Frame distance
	char    FrameRatesSupported[100];	                // optional, List of supported target frame rates in fps (frames per second). 
	                                                    //  The list shall be sorted with highest values first
	char    ProfilesSupported[256];	                    // optional, List of supported encoder profiles
	                                                    //  Simple          <!-- MPEG4 SP -->
	                                                    //  AdvancedSimple  <!-- MPEG4 ASP -->
	                                                    //  Baseline        <!-- H264 Baseline -->
	                                                    //  Main            <!-- H264 Main, H.265 Main -->
	                                                    //  Main10          <!-- H265 Main 10 -->
	                                                    //  Extended        <!-- H264 Extended -->
	                                                    //  High            <!-- H264 High -->
	BOOL    ConstantBitRateSupported;	                // optional, Signal whether enforcing constant bitrate is supported
} onvif_VideoEncoder2ConfigurationOptions;

typedef struct _ONVIF_VideoEncoder2ConfigurationOptions
{
    struct _ONVIF_VideoEncoder2ConfigurationOptions * next;

    onvif_VideoEncoder2ConfigurationOptions Options;
} ONVIF_VideoEncoder2ConfigurationOptions;

typedef struct 
{
    uint32  ConstantBitRateFlag : 1;                    // Indicates whether the field ConstantBitRate is valid
    uint32  Reserved            : 31;
    
	float   FrameRateLimit;	                            // required, Desired frame rate in fps. The actual rate may be lower due to e.g. performance limitations
	int     BitrateLimit;	                            // required, the maximum output bitrate in kbps
	BOOL    ConstantBitRate;	                        // optional, Enforce constant bitrate

	int     EncodingInterval;                           // required, The media server field
} onvif_VideoRateControl2;

typedef struct 
{
    uint32  RateControlFlag : 1;                        // Indicates whether the field RateControl is valid
    uint32  MulticastFlag   : 1;                        // Indicates whether the field Multicast is valid
    uint32  GovLengthFlag   : 1;                        // Indicates whether the field GovLength is valid
    uint32  ProfileFlag     : 1;                        // Indicates whether the field Profile is valid
    uint32  Reserved        : 28;
    
	char    Name[ONVIF_NAME_LEN];	                    // required
	int     UseCount;	                                // required
	char    token[ONVIF_TOKEN_LEN];	                    // required
	char    Encoding[64];	                            // required, Mime name of the supported video format.
	                                                    //  JPEG, MPV4-ES, H264, H265
    onvif_VideoEncoding             VideoEncoding;      // required, the media service field
    
	onvif_VideoResolution           Resolution;	        // required, Configured video resolution
	onvif_VideoRateControl2         RateControl;	    // optional, Optional element to configure rate control related parameters
	onvif_MulticastConfiguration    Multicast;	        // optional, Defines the multicast settings that could be used for video streaming

	float   Quality;	                                // required, Relative value for the video quantizers and the quality of the video. 
	                                                    //  A high value within supported quality range means higher quality
	int     GovLength;	                                // optional, Group of Video frames length. Determines typically the interval in which the I-Frames will be coded. 
	                                                    //  An entry of 1 indicates I-Frames are continuously generated. An entry of 2 indicates that every 2nd image is an I-Frame, and 3 only every 3rd frame, etc. 
	                                                    //  The frames in between are coded as P or B Frames
	char    Profile[64];	                            // optional, The encoder profile
	                                                    //  Simple          <!-- MPEG4 SP -->
	                                                    //  AdvancedSimple  <!-- MPEG4 ASP -->
	                                                    //  Baseline        <!-- H264 Baseline -->
	                                                    //  Main            <!-- H264 Main, H.265 Main -->
	                                                    //  Main10          <!-- H265 Main 10 -->
	                                                    //  Extended        <!-- H264 Extended -->
	                                                    //  High            <!-- H264 High -->

	int     SessionTimeout;                             // required, the media service field
} onvif_VideoEncoder2Configuration;

typedef struct _ONVIF_VideoEncoder2Configuration
{
    struct _ONVIF_VideoEncoder2Configuration * next;

    onvif_VideoEncoder2Configuration        Configuration;
} ONVIF_VideoEncoder2Configuration;

typedef struct 
{
    uint32  MulticastFlag   : 1;                        // Indicates whether the field Multicast is valid
    uint32  Reserved        : 31;
    
	char    Name[ONVIF_NAME_LEN];	                    // required
	int     UseCount;	                                // required
	char    token[ONVIF_TOKEN_LEN];	                    // required
	char    Encoding[32];	                            // required, Mime name of the supported audio format
	                                                    //  PCMU, G726, MP4A-LATM(AAC)

	onvif_AudioEncoding             AudioEncoding;      // required, the media service field
	
	onvif_MulticastConfiguration    Multicast;	        // optional, Optional multicast configuration of the audio stream
	int     Bitrate;	                                // required, The output bitrate in kbps
	int     SampleRate;	                                // required, The output sample rate in kHz

	int     SessionTimeout;                             // required, the media service field
} onvif_AudioEncoder2Configuration;

typedef struct 
{
	char    Encoding[32];	                            // required, Mime name of the supported audio format
	                                                    //  PCMU, G726, MP4A-LATM(AAC)
    onvif_AudioEncoding AudioEncoding;                  // media server 1 field
    
	onvif_IntList   BitrateList;	                    // required, List of supported bitrates in kbps for the specified Encoding
	onvif_IntList   SampleRateList;	                    // required, List of supported Sample Rates in kHz for the specified Encoding
} onvif_AudioEncoder2ConfigurationOptions;

typedef struct _ONVIF_AudioEncoder2ConfigurationOptions
{
    struct _ONVIF_AudioEncoder2ConfigurationOptions * next;

    onvif_AudioEncoder2ConfigurationOptions Options;
} ONVIF_AudioEncoder2ConfigurationOptions;

typedef struct _ONVIF_AudioEncoder2Configuration
{
    struct _ONVIF_AudioEncoder2Configuration * next;

    onvif_AudioEncoder2Configuration        Configuration;
} ONVIF_AudioEncoder2Configuration;

typedef struct 
{
    uint32  TokenFlag   : 1;                            // Indicates whether the field Token is valid
    uint32  Reserved    : 31;
    
	char    Type[32];	                                // required, Type of the configuration
	                                                    //  All, VideoSource, VideoEncoder, AudioSource, AudioEncoder, 
	                                                    //  AudioOutput, AudioDecoder, Metadata, Analytics, PTZ
	char    Token[ONVIF_TOKEN_LEN];	                    // optional, Reference token of an existing configuration
} onvif_ConfigurationRef;

typedef struct 
{
	char    Encoding[32];	                            // required,
	int     Number;	                                    // required,
} onvif_EncoderInstance;

typedef struct 
{
	int     sizeCodec;  
	onvif_EncoderInstance   Codec[10];	                // optional, If a device limits the number of instances for respective Video Codecs the response
	                                                    //  contains the information how many streams can be set up at the same time per VideoSource
	
	int     Total;	                                    // required, The minimum guaranteed total number of encoder instances (applications) per VideoSourceConfiguration. 
	                                                    //  The device is able to deliver the Total number of streams
} onvif_EncoderInstanceInfo;

typedef struct 
{
	char    Name[ONVIF_NAME_LEN];	                    // required, 
	int     UseCount;	                                // required, 
	char    token[ONVIF_TOKEN_LEN];	                    // required, 
} onvif_ConfigurationEntity;

typedef struct 
{
	char    Name[ONVIF_NAME_LEN];	                    // required, 
	int     UseCount;	                                // required, 
	char    token[ONVIF_TOKEN_LEN];	                    // required, 
} onvif_AudioDecoderConfiguration;

typedef struct _ONVIF_AudioDecoderConfiguration
{
	struct _ONVIF_AudioDecoderConfiguration * next;

	onvif_AudioDecoderConfiguration Configuration;
} ONVIF_AudioDecoderConfiguration;

typedef struct 
{
	onvif_IntList   Bitrate;	                        // required
	onvif_IntList   SampleRateRange;	                // required
} onvif_AACDecOptions;

typedef struct 
{
	onvif_IntList   Bitrate;	                        // required
	onvif_IntList   SampleRateRange;	                // required
} onvif_G711DecOptions;

typedef struct 
{
	onvif_IntList   Bitrate;	                        // required
	onvif_IntList   SampleRateRange;	                // required
} onvif_G726DecOptions;

typedef struct
{
    uint32 AACDecOptionsFlag  : 1;                      // Indicates whether the field AACDecOptions is valid
    uint32 G711DecOptionsFlag : 1;                      // Indicates whether the field G711DecOptions is valid
    uint32 G726DecOptionsFlag : 1;                      // Indicates whether the field G726DecOptions is valid
    uint32 Reserved           : 29;
    
    onvif_AACDecOptions     AACDecOptions;	            // optional
	onvif_G711DecOptions    G711DecOptions;	            // optional
	onvif_G726DecOptions    G726DecOptions;	            // optional
} onvif_AudioDecoderConfigurationOptions;

typedef struct 
{
    uint32  VideoSourceFlag     : 1;                    // Indicates whether the field VideoSource is valid
    uint32  AudioSourceFlag     : 1;                    // Indicates whether the field AudioSource is valid
    uint32  VideoEncoderFlag    : 1;                    // Indicates whether the field VideoEncoder is valid
    uint32  AudioEncoderFlag    : 1;                    // Indicates whether the field AudioEncoder is valid
    uint32  AnalyticsFlag       : 1;                    // Indicates whether the field Analytics is valid
    uint32  PTZFlag             : 1;                    // Indicates whether the field PTZ is valid
    uint32  MetadataFlag        : 1;                    // Indicates whether the field Metadata is valid
    uint32  AudioOutputFlag     : 1;                    // Indicates whether the field AudioOutput is valid
    uint32  AudioDecoderFlag    : 1;                    // Indicates whether the field AudioDecoder is valid    
    uint32  Reserved            : 23;
    
	onvif_VideoSourceConfiguration      VideoSource;	// optional, Optional configuration of the Video input
	onvif_AudioSourceConfiguration      AudioSource;	// optional, Optional configuration of the Audio input
	onvif_VideoEncoder2Configuration    VideoEncoder;	// optional, Optional configuration of the Video encoder
	onvif_AudioEncoder2Configuration    AudioEncoder;	// optional, Optional configuration of the Audio encoder
	onvif_VideoAnalyticsConfiguration   Analytics;	    // optional, Optional configuration of the analytics module and rule engine
	onvif_PTZConfiguration              PTZ;	        // optional, Optional configuration of the pan tilt zoom unit
	onvif_MetadataConfiguration         Metadata;	    // optional, Optional configuration of the metadata stream
	onvif_AudioOutputConfiguration      AudioOutput;	// optional, Optional configuration of the Audio output
	onvif_AudioDecoderConfiguration     AudioDecoder;	// optional, Optional configuration of the Audio decoder
} onvif_ConfigurationSet;

typedef struct 
{
	char    Name[ONVIF_NAME_LEN];	                    // required, User readable name of the profile

	onvif_ConfigurationSet  Configurations;	            // required, The configurations assigned to the profile

	char    token[ONVIF_TOKEN_LEN];	                    // required, Unique identifier of the profile
	BOOL    fixed;	                                    // optional, A value of true signals that the profile cannot be deleted. Default is false
} onvif_MediaProfile;

typedef struct _ONVIF_MediaProfile
{
    struct _ONVIF_MediaProfile * next;

    onvif_MediaProfile  MediaProfile;
} ONVIF_MediaProfile;

typedef struct 
{
	int             sizePoint;                          // sequence of elements <Point>
	onvif_Vector    Point[100];	                        // required, 
} onvif_Polygon;

typedef struct 
{
    uint32  ColorFlag : 1;                              // Indicates whether the field Color is valid
    uint32  Reserved  : 31;
    
	char    ConfigurationToken[ONVIF_TOKEN_LEN];	    // required, Token of the VideoSourceConfiguration the Mask is associated with

	onvif_Polygon   Polygon;	                        // required, Geometric representation of the mask area

	char    Type[64];	                                // required, 
	                                                    //  Color - The masked area is colored with color defined by the Color field
								                        //  Pixelated - The masked area is filled in mosaic style to hide details
								                        //  Blurred - The masked area is low pass filtered to hide details
	onvif_Color Color;	                                // optional, Color of the masked area

	BOOL    Enabled;	                                // required, If set the mask will cover the image, otherwise it will be fully transparent
	char    token[ONVIF_TOKEN_LEN];	                    // required, Token of the mask
} onvif_Mask;

typedef struct
{    
	int     MaxMasks;	                                // required, Maximum supported number of masks per VideoSourceConfiguration
	int     MaxPoints;	                                // required, Maximum supported number of points per mask

	int     sizeTypes;	                                // sequence, 
	char    Types[10][64];	                            // required, Information which types of tr2:MaskType are supported. 
	                                                    //  Valid values are 'Color', 'Pixelated' and 'Blurred'

	onvif_ColorOptions  Color;	                        // required, Colors supported

	BOOL    RectangleOnly;	                            // optional, Information whether the polygon must have four points and a rectangular shape
	BOOL    SingleColorOnly;	                        // optional, Indicates the device capability of change in color of privacy mask for one video source 
	                                                    //  configuration will automatically be applied to all the privacy masks associated with the same
	                                                    //  video source configuration
} onvif_MaskOptions;

typedef struct _ONVIF_Mask
{
    struct _ONVIF_Mask * next;

    onvif_Mask  Mask;
} ONVIF_Mask;

// MEDIA2 Define End

// Thermal Define Begin

typedef struct
{
	char    Name[ONVIF_NAME_LEN];	                    // required, User readable Color Palette name
	char    token[ONVIF_TOKEN_LEN];	                    // required, Unique identifier of this Color Palette
	char    Type[32];                                   // required, Indicates Color Palette Type. Can use the following value:
	                                                    // Custom,Grayscale,BlackHot,WhiteHot,Sepia,Red,Iron,Rain,Rainbow,Isotherm
} onvif_ColorPalette;

typedef enum 
{
    Polarity_WhiteHot = 0, 
    Polarity_BlackHot = 1
} onvif_Polarity;

typedef struct 
{
    uint32  LowTemperatureFlag  : 1;                    // Indicates whether the field LowTemperature is valid
    uint32  HighTemperatureFlag : 1;                    // Indicates whether the field HighTemperature is valid
    uint32  Reserved            : 30;
    
	char    Name[ONVIF_NAME_LEN];	                    // required, User reabable name for the Non-Uniformity Correction (NUC) Table
	char    token[ONVIF_TOKEN_LEN];	                    // required, Unique identifier of this NUC Table
	float   LowTemperature;	                            // optional, Low Temperature limit for application of NUC Table, in Kelvin
	float   HighTemperature;	                        // optional, High Temperature limit for application of NUC Table, in Kelvin
} onvif_NUCTable;

typedef struct
{
    uint32  RunTimeFlag : 1;                           // Indicates whether the field RunTime is valid
    uint32  Reserved    : 31;
    
	BOOL    Enabled;	                                // required, Indicates whether the Cooler is enabled (running) or not
	float   RunTime;	                                // optional, Number of hours the Cooler has been running (unit: hours). Read-only
} onvif_Cooler;

typedef struct 
{
    uint32  NUCTableFlag : 1;                           // Indicates whether the field NUCTable is valid
    uint32  CoolerFlag   : 1;                           // Indicates whether the field Cooler is valid
    uint32  Reserved     : 30;

	onvif_ColorPalette  ColorPalette;	                // required, Current Color Palette in use by the Thermal Device
	onvif_Polarity      Polarity;	                    // required, Polarity configuration of the Thermal Device
	onvif_NUCTable      NUCTable;	                    // optional, Current Non-Uniformity Correction (NUC) Table in use by the Thermal Device
	onvif_Cooler        Cooler;	                        // optional, Cooler settings of the Thermal Device
} onvif_ThermalConfiguration;

typedef struct _ONVIF_ThermalConfiguration
{
    struct _ONVIF_ThermalConfiguration * next;

    
    char    token[ONVIF_TOKEN_LEN];                     // required, Reference token to the thermal VideoSource

    onvif_ThermalConfiguration Configuration;
} ONVIF_ThermalConfiguration;

typedef struct _ONVIF_ColorPalette
{
    struct _ONVIF_ColorPalette * next;

    onvif_ColorPalette ColorPalette;
} ONVIF_ColorPalette;

typedef struct _ONVIF_NUCTable
{
    struct _ONVIF_NUCTable * next;

    onvif_NUCTable  NUCTable;
} ONVIF_NUCTable;

typedef struct 
{
	BOOL    Enabled;	                                // optional, Indicates the Device allows cooler status to be changed from running (Enabled) to stopped (Disabled), and viceversa
} onvif_CoolerOptions;

typedef struct 
{
    uint32  CoolerOptionsFlag   : 1;                    // Indicates whether the field CoolerOptions is valid
    uint32  Reserved            : 31;
    
	ONVIF_ColorPalette* ColorPalette;	                // required, List of Color Palettes available for the requested Thermal VideoSource
	ONVIF_NUCTable    * NUCTable;	                    // optional, List of Non-Uniformity Correction (NUC) Tables available for the requested Thermal VideoSource
	onvif_CoolerOptions CoolerOptions;	                // optional, Specifies Cooler Options for cooled thermal devices
} onvif_ThermalConfigurationOptions;

typedef struct 
{
    uint32  RelativeHumidityFlag            : 1;        // Indicates whether the field RelativeHumidity is valid
    uint32  AtmosphericTemperatureFlag      : 1;        // Indicates whether the field AtmosphericTemperature is valid
    uint32  AtmosphericTransmittanceFlag    : 1;        // Indicates whether the field AtmosphericTransmittance is valid
    uint32  ExtOpticsTemperatureFlag        : 1;        // Indicates whether the field ExtOpticsTemperature is valid
    uint32  ExtOpticsTransmittanceFlag      : 1;        // Indicates whether the field ExtOpticsTransmittance is valid
    uint32  Reserved                        : 27;
    
	float   ReflectedAmbientTemperature;	            // required, Reflected Ambient Temperature for the environment in which the thermal device and the object being measured is located
	float   Emissivity;	                                // required, Emissivity of the surface of the object on which temperature is being measured
	float   DistanceToObject;	                        // required, Distance from the thermal device to the measured object
	float   RelativeHumidity;	                        // optional, Relative Humidity in the environment in which the measurement is located
	float   AtmosphericTemperature;	                    // optional, Temperature of the atmosphere between the thermal device and the object being measured
	float   AtmosphericTransmittance;	                // optional, Transmittance value for the atmosphere between the thermal device and the object being measured
	float   ExtOpticsTemperature;	                    // optional, Temperature of the optics elements between the thermal device and the object being measured
	float   ExtOpticsTransmittance;	                    // optional, Transmittance value for the optics elements between the thermal device and the object being measured
} onvif_RadiometryGlobalParameters;

typedef struct 
{
    uint32  RadiometryGlobalParametersFlag  : 1;        // Indicates whether the field RadiometryGlobalParameters is valid
    uint32  Reserved                        : 31;
    
	onvif_RadiometryGlobalParameters    RadiometryGlobalParameters;	// optional, Global Parameters for Radiometry Measurements. 
	                                                    // Shall exist if Radiometry Capability is reported, and Global Parameters are supported by the device
} onvif_RadiometryConfiguration;

typedef struct 
{
    uint32  RelativeHumidityFlag            : 1;        // Indicates whether the field RelativeHumidity is valid
    uint32  AtmosphericTemperatureFlag      : 1;        // Indicates whether the field AtmosphericTemperature is valid
    uint32  AtmosphericTransmittanceFlag    : 1;        // Indicates whether the field AtmosphericTransmittance is valid
    uint32  ExtOpticsTemperatureFlag        : 1;        // Indicates whether the field ExtOpticsTemperature is valid
    uint32  ExtOpticsTransmittanceFlag      : 1;        // Indicates whether the field ExtOpticsTransmittance is valid
    uint32  Reserved                        : 27;
    
	onvif_FloatRange    ReflectedAmbientTemperature;	// required, Valid range of temperature values, in Kelvin
	onvif_FloatRange    Emissivity;	                    // required, Valid range of emissivity values for the objects to measure
	onvif_FloatRange    DistanceToObject;	            // required, Valid range of distance between camera and object for a valid temperature reading, in meters
	onvif_FloatRange    RelativeHumidity;	            // optional, Valid range of relative humidity values, in percentage
	onvif_FloatRange    AtmosphericTemperature;	        // optional, Valid range of temperature values, in Kelvin
	onvif_FloatRange    AtmosphericTransmittance;	    // optional, Valid range of atmospheric transmittance values
	onvif_FloatRange    ExtOpticsTemperature;	        // optional, Valid range of temperature values, in Kelvin
	onvif_FloatRange    ExtOpticsTransmittance;	        // optional, Valid range of external optics transmittance
} onvif_RadiometryGlobalParameterOptions;

typedef struct 
{
    uint32  RadiometryGlobalParameterOptionsFlag  : 1;  // Indicates whether the field RadiometryGlobalParameterOptions is valid
    uint32  Reserved                              : 31;
    
	onvif_RadiometryGlobalParameterOptions  RadiometryGlobalParameterOptions;   // optional, Specifies valid ranges and options for the global radiometry parameters 
	                                                    // used as default parameter values for temperature measurement modules (spots and boxes)
} onvif_RadiometryConfigurationOptions;

// Thermal Define End

// Credential define start

typedef struct 
{
    uint32  DescriptionFlag : 1;                        // Indicates whether the field Description is valid
    uint32  ValidFromFlag   : 1;                        // Indicates whether the field ValidFrom is valid
    uint32  ValidToFlag     : 1;                        // Indicates whether the field ValidTo is valid
    uint32  Reserved        : 29;
    
	char    token[ONVIF_TOKEN_LEN];	                    // required, 
	char    Description[1024];	                        // optional, User readable description for the credential. It shall be up to 1024 characters
	char    CredentialHolderReference[ONVIF_TOKEN_LEN];	// required, An external reference to a person holding this credential. 
	                                                    //  The reference is a username or used ID in an external system, such as a directory
										                //  service
	char    ValidFrom[64];	                            // optional, The start date/time validity of the credential. If the
										                //  ValiditySupportsTimeValue capability is set to false, then only date is
										                //  supported (time is ignored)
	char    ValidTo[64];                                // optional, The expiration date/time validity of the credential. If the
										                //  ValiditySupportsTimeValue capability is set to false, then only date is
										                //  supported (time is ignored)
} onvif_CredentialInfo;

typedef struct 
{
	char    Name[100];	                                // required, The name of the credential identifier type, such as pt:Card, pt:PIN, etc
	char    FormatType[100];                            // required, Specifies the format of the credential value for the specified identifier type name
} onvif_CredentialIdentifierType;

// A credential identifier is a card number, unique card information, PIN or
//  biometric information such as fingerprint, iris, vein, face recognition, that can be validated
//  in an access point
typedef struct 
{
    BOOL    Used;                                       // used flag
    
	onvif_CredentialIdentifierType  Type;	            // required, Contains the details of the credential identifier type. Is of type CredentialIdentifierType

	BOOL    ExemptedFromAuthentication;	                // required, If set to true, this credential identifier is not considered for authentication

	char    Value[2048];	                            // required, The value of the identifier in hexadecimal representation
} onvif_CredentialIdentifier;

// The association between a credential and an access profile
typedef struct 
{
    uint32  Used            : 1;                        // used flag
    uint32  ValidFromFlag   : 1;                        // Indicates whether the field ValidFrom is valid
    uint32  ValidToFlag     : 1;                        // Indicates whether the field ValidTo is valid
    uint32  Reserved        : 29;
    
	char    AccessProfileToken[ONVIF_TOKEN_LEN];	    // required, The reference token of the associated access profile
	char    ValidFrom[64];                              // optional, The start date/time of the validity for the association between the
								                        //  credential and the access profile. If the ValiditySupportsTimeValue capability is set to
								                        //  false, then only date is supported (time is ignored)
	char    ValidTo[64];                                // optional, The end date/time of the validity for the association between the
								                        //  credential and the access profile. If the ValiditySupportsTimeValue capability is set to
								                        //  false, then only date is supported (time is ignored)
} onvif_CredentialAccessProfile;

typedef struct 
{
    uint32  Used        : 1;                            // used flag
    uint32  ValueFlag   : 1;                            // Indicates whether the field Value is valid
    uint32  Reserved    : 30;
    
	char    Name[100];	                                // required, 
	char    Value[100];	                                // optional, 
} onvif_Attribute;

// A Credential is a physical/tangible object, a piece of knowledge, or a facet of a person's
//  physical being, that enables an individual access to a given physical facility or computer-based
//  information system. A credential holds one or more credential identifiers. To gain access one or
//  more identifiers may be required

typedef struct 
{
    uint32  DescriptionFlag : 1;                        // Indicates whether the field Description is valid
    uint32  ValidFromFlag   : 1;                        // Indicates whether the field ValidFrom is valid
    uint32  ValidToFlag     : 1;                        // Indicates whether the field ValidTo is valid
    uint32  Reserved        : 29;
    
	char    token[ONVIF_TOKEN_LEN];	                    // required, 
	char    Description[1024];	                        // optional, User readable description for the credential. It shall be up to 1024 characters
	char    CredentialHolderReference[ONVIF_TOKEN_LEN];	// required, An external reference to a person holding this credential. 
	                                                    //  The reference is a username or used ID in an external system, such as a directory
										                //  service
	char    ValidFrom[64];                              // optional, The start date/time validity of the credential. If the
										                //  ValiditySupportsTimeValue capability is set to false, then only date is
										                //  supported (time is ignored)
	char    ValidTo[64];                                // optional, The expiration date/time validity of the credential. If the
										                //  ValiditySupportsTimeValue capability is set to false, then only date is
										                //  supported (time is ignored)
										                
	int     sizeCredentialIdentifier;	                // sequence of elements <CredentialIdentifier>
	onvif_CredentialIdentifier  CredentialIdentifier[CREDENTIAL_MAX_LIMIT]; // required, A list of credential identifier structures. At least one
										                //  credential identifier is required. Maximum one credential identifier structure
										                //  per type is allowed
	
	int     sizeCredentialAccessProfile;	            // sequence of elements <CredentialAccessProfile>
	onvif_CredentialAccessProfile   CredentialAccessProfile[CREDENTIAL_MAX_LIMIT];  // optional, A list of credential access profile structures
	
	int     sizeAttribute;	                            // sequence of elements <Attribute>
	onvif_Attribute Attribute[CREDENTIAL_MAX_LIMIT];    // optional, A list of credential attributes as name value pairs. Key names
										                //  starting with the prefix pt: are reserved to define PACS specific attributes
										                //  following the "pt:<Name>" syntax
} onvif_Credential;

typedef struct 
{
	BOOL    AntipassbackViolated;	                    // required, Indicates if anti-passback is violated for the credential
} onvif_AntipassbackState;

// The CredentialState structure contains information about the state of the credential and
//  optionally the reason of why the credential was disabled

typedef struct 
{
    uint32  ReasonFlag              : 1;                // Indicates whether the field Reason is valid
    uint32  AntipassbackStateFlag   : 1;                // Indicates whether the field AntipassbackState is valid
    uint32  Reserved                : 30;
    
	BOOL    Enabled;	                                // required, True if the credential is enabled or false if the credential is disabled
	char    Reason[100];	                            // optional, Predefined ONVIF reasons. For any other reason, free	text can be used
                                                        // pt:CredentialLockedOut
                                                        //  Access is denied due to credential locked out.
                                                        // pt:CredentialBlocked
                                                        //  Access is denied because the credential has deliberately been blocked by the operator.
                                                        // pt:CredentialLost
                                                        //  Access is denied due to the credential being reported as lost.
                                                        // pt:CredentialStolen
                                                        //  Access is denied due to the credential being reported as stolen
                                                        // pt:CredentialDamaged
                                                        //  Access is denied due to the credential being reported as damaged.
                                                        // pt:CredentialDestroyed
                                                        //  Access is denied due to the credential being reported as destroyed
                                                        // pt:CredentialInactivity
                                                        //  Access is denied due to credential inactivity
                                                        // pt:CredentialExpired
                                                        //  Access is denied because the credential has expired
                                                        // pt:CredentialRenewalNeeded
                                                        //  Access is denied because the credential requires a renewal (e.g. new PIN or
                                                        //  fingerprint enrollment).

	onvif_AntipassbackState AntipassbackState;	        // optional, A structure indicating the anti-passback state. This field shall be
								                        //  supported if the ResetAntipassbackSupported capability is set to true
} onvif_CredentialState;

typedef struct 
{
	char    FormatType[100];	                        // required, A format type supported by the device. A list of supported format types is
								                        //  provided in [ISO 16484-5:2014-09 Annex P]. The BACnet type "CUSTOM" is not used. 
								                        //  Instead device manufacturers can define their own format types
	char    Description[1024];	                        // required, User readable description of the credential identifier format type. It
								                        //  shall be up to 1024 characters
} onvif_CredentialIdentifierFormatTypeInfo;

typedef struct _ONVIF_Credential
{
    struct _ONVIF_Credential * next;

    onvif_Credential        Credential;
    onvif_CredentialState   State;
} ONVIF_Credential;

// Credential define end

// Access Rules define begin

typedef struct 
{
    uint32  DescriptionFlag : 1;                        // Indicates whether the field Description is valid
    uint32  Reserved        : 31;
    
	char    token[ONVIF_TOKEN_LEN];	                    // required, 
	char    Name[ONVIF_NAME_LEN];	                    // required, A user readable name. It shall be up to 64 characters
	char    Description[1024];                          // optional, User readable description for the access profile. It shall be up to 1024 characters
} onvif_AccessProfileInfo;

typedef struct 
{
    uint32  EntityTypeFlag  : 1;                        // Indicates whether the field EntityType is valid
    uint32  Reserved        : 31;
    
	char    ScheduleToken[ONVIF_TOKEN_LEN];	            // required, Reference to the schedule used by the access policy
	char    Entity[ONVIF_TOKEN_LEN];	                // required, Reference to the entity used by the rule engine, 
	                                                    //  the entity type may be specified by the optional EntityType field 
	                                                    //  explained below but is typically an access point
	char    EntityType[64];	                            // optional, Optional entity type; if missing, an access point type as defined 
	                                                    //  by the ONVIF Access Control service should be assumed. 
	                                                    //  This can also be represented by the QName value tac:AccessPoint
	                                                    //  where tac is the namespace of Access Control Service Specification. 
	                                                    //  This field is provided for future extensions; 
	                                                    //  it will allow an access policy being extended to cover entity types 
	                                                    //  other than access points as well
} onvif_AccessPolicy;

typedef struct 
{
    uint32  DescriptionFlag : 1;                        // Indicates whether the field Description is valid
    uint32  Reserved        : 31;
    
	char    token[ONVIF_TOKEN_LEN];	                    // required, 
	char    Name[ONVIF_NAME_LEN];	                    // required, A user readable name. It shall be up to 64 characters
	char    Description[1024];                          // optional, User readable description for the access profile. It shall be up to 1024 characters

	int     sizeAccessPolicy;	                        // sequence of elements <AccessPolicy> 
	onvif_AccessPolicy  AccessPolicy[ACCESSRULES_MAX_LIMIT];    // optional, A list of access policy structures, 
	                                                    //  where each access policy defines during which schedule an access point can be accessed
} onvif_AccessProfile;

typedef struct _ONVIF_AccessProfile
{
    struct _ONVIF_AccessProfile * next;

    onvif_AccessProfile AccessProfile;
} ONVIF_AccessProfile;


// Access Rules define end

// Schedule define begin

typedef struct 
{
    uint32  DescriptionFlag : 1;                        // Indicates whether the field Description is valid
    uint32  Reserved        : 31;
    
	char    token[ONVIF_TOKEN_LEN];	                    // required
	char    Name[ONVIF_NAME_LEN];	                    // required, A user readable name. It shall be up to 64 characters
	char    Description[1024];	                        // optional, User readable description for the schedule. It shall be up to 1024 characters
} onvif_ScheduleInfo;

typedef struct 
{
    uint32  UntilFlag   : 1;                           // Indicates whether the field Until is valid
    uint32  Reserved    : 31;
    
	char    From[32];	                                // required, Indicates the start time
	char    Until[32];	                                // optional, Indicates the end time. Is optional, if omitted, the period ends at midnight.
								                        //  The end time is exclusive, meaning that that exact moment in time is not
								                        //  part of the period. To determine if a moment in time (t) is part of a time period,
								                        //  the formula StartTime &#8804; t &lt; EndTime is used.
} onvif_TimePeriod;

typedef struct 
{
	char    GroupToken[ONVIF_TOKEN_LEN];	            // required, Indicates the list of special days in a schedule
	
	int     sizeTimeRange;	                            // sequence of elements <TimeRange>
	
	onvif_TimePeriod TimeRange[SCHEDULE_MAX_LIMIT];	    // optional, Indicates the alternate time periods for the list of special days
								                        //  (overrides the regular schedule). For example, the regular schedule indicates
								                        //  that it is active from 8AM to 5PM on Mondays. However, this particular
								                        //  Monday is a special day, and the alternate time periods state that the
								                        //  schedule is active from 9 AM to 11 AM and 1 PM to 4 PM.
								                        //  If no time periods are defined, then no access is allowed.
								                        //  Is of type TimePeriod
} onvif_SpecialDaysSchedule;

typedef struct 
{
    uint32  DescriptionFlag : 1;                        // Indicates whether the field Description is valid
    uint32  Reserved        : 31;
    
	char    token[ONVIF_TOKEN_LEN];	                    // required
	char    Name[ONVIF_NAME_LEN];	                    // required, A user readable name. It shall be up to 64 characters
	char    Description[1024];	                        // optional, User readable description for the schedule. It shall be up to 1024 characters
	char    Standard[10*1024];	                        // required, An iCalendar structure that defines a number of events. Events
										                //  can be recurring or non-recurring. The events can, for instance,
										                //  be used to control when a camera should record or when a facility
										                //  is accessible. Some devices might not be able to fully support
										                //  all the features of iCalendar. Setting the service capability
										                //  ExtendedRecurrenceSupported to false will enable more devices
										                //  to be ONVIF compliant. Is of type string (but contains an iCalendar structure)
	int     sizeSpecialDays;	                        // sequence of elements <SpecialDays>
	
	onvif_SpecialDaysSchedule   SpecialDays[SCHEDULE_MAX_LIMIT];    // optional, For devices that are not able to support all the features of iCalendar,
										                //  supporting special days is essential. Each SpecialDaysSchedule
										                //  instance defines an alternate set of time periods that overrides
										                //  the regular schedule for a specified list of special days.
										                //  Is of type SpecialDaysSchedule
} onvif_Schedule;

typedef struct 
{
    uint32  DescriptionFlag : 1;                        // Indicates whether the field Description is valid
    uint32  Reserved        : 31;
    
	char    token[ONVIF_TOKEN_LEN];	                    // required
	char    Name[ONVIF_NAME_LEN];	                    // required, A user readable name. It shall be up to 64 characters
	char    Description[1024];	                        // optional, User readable description for the schedule. It shall be up to 1024 characters
} onvif_SpecialDayGroupInfo;

typedef struct 
{
	uint32  DescriptionFlag : 1;                        // Indicates whether the field Description is valid
	uint32  DaysFlag        : 1;                        // Indicates whether the field Days is valid
    uint32  Reserved        : 30;
    
	char    token[ONVIF_TOKEN_LEN];	                    // required
	char    Name[ONVIF_NAME_LEN];	                    // required, A user readable name. It shall be up to 64 characters
	char    Description[1024];	                        // optional, User readable description for the schedule. It shall be up to 1024 characters
	char    Days[4*1024];	                            // optional, An iCalendar structure that contains a group of special days.
										                //  Is of type string (containing an iCalendar structure)
} onvif_SpecialDayGroup;

typedef struct 
{
    uint32  SpecialDayFlag  : 1;                        // Indicates whether the field SpecialDay is valid
    uint32  Reserved        : 31;
    
	BOOL    Active;	                                    // required, Indicates that the current time is within the boundaries of the schedule
								                        //  or its special days schedules's time periods. For example, if this
								                        //  schedule is being used for triggering automatic recording on a video source,
								                        //  the Active flag will be true when the schedule-based recording is supposed to record
	BOOL    SpecialDay;	                                // optional, Indicates that the current time is within the boundaries of its special
								                        //  days schedules's time periods. For example, if this schedule is being used
								                        //  for recording at a lower frame rate on a video source during special days,
								                        //  the SpecialDay flag will be true. If special days are not supported by the device,
								                        //  this field may be omitted and interpreted as false by the client
} onvif_ScheduleState;

typedef struct _ONVIF_Schedule
{
    struct _ONVIF_Schedule * next;

    onvif_Schedule      Schedule;
    onvif_ScheduleState ScheduleState;

#ifdef LIBICAL
    icalcomponent     * comp;
#endif
} ONVIF_Schedule;

typedef struct _ONVIF_SpecialDayGroup
{
    struct _ONVIF_SpecialDayGroup * next;

    onvif_SpecialDayGroup SpecialDayGroup;

#ifdef LIBICAL
    icalcomponent       * comp;
#endif
} ONVIF_SpecialDayGroup;

// Schedule define end

// Receiver define begin

typedef struct 
{
	onvif_ReceiverMode  Mode;	                        // required, connection modes
	char                MediaUri[256];	                // required, Details of the URI to which the receiver should connect
	onvif_StreamSetup   StreamSetup;	                // required, Stream connection parameters
} onvif_ReceiverConfiguration;

typedef struct 
{
	char    Token[ONVIF_TOKEN_LEN];	                    // required, Unique identifier of the receiver
	
	onvif_ReceiverConfiguration Configuration;	        // required, Describes the configuration of the receiver
} onvif_Receiver;

typedef struct 
{
	onvif_ReceiverState State;	                        // required, The connection state of the receiver
	
	BOOL    AutoCreated;	                            // required, Indicates whether or not the receiver was created automatically
} onvif_ReceiverStateInformation;

typedef struct _ONVIF_Receiver
{
    struct _ONVIF_Receiver * next;

    onvif_Receiver  Receiver;
    onvif_ReceiverStateInformation  StateInformation;   
} ONVIF_Receiver;

// Receiver define end



#ifdef __cplusplus
extern "C" {
#endif


const char * 				    onvif_CapabilityCategoryToString(onvif_CapabilityCategory category);
onvif_CapabilityCategory 	    onvif_StringToCapabilityCategory(const char * str);

const char *                    onvif_FactoryDefaultTypeToString(onvif_FactoryDefaultType type);
onvif_FactoryDefaultType        onvif_StringToFactoryDefaultType(const char * str);

const char *                    onvif_SystemLogTypeToString(onvif_SystemLogType type);
onvif_SystemLogType             onvif_StringToSystemLogType(const char * str);

const char * 				    onvif_VideoEncodingToString(onvif_VideoEncoding encoding);
onvif_VideoEncoding 		    onvif_StringToVideoEncoding(const char * str);

const char * 				    onvif_AudioEncodingToString(onvif_AudioEncoding encoding);
onvif_AudioEncoding 		    onvif_StringToAudioEncoding(const char * str);

const char * 				    onvif_H264ProfileToString(onvif_H264Profile profile);
onvif_H264Profile 			    onvif_StringToH264Profile(const char * str);

const char *                    onvif_MediaProfile2Media2Profile(const char * profile);

const char * 				    onvif_Mpeg4ProfileToString(onvif_Mpeg4Profile profile);
onvif_Mpeg4Profile 			    onvif_StringToMpeg4Profile(const char * str);

const char * 				    onvif_UserLevelToString(onvif_UserLevel level);
onvif_UserLevel 			    onvif_StringToUserLevel(const char * str);

const char * 				    onvif_MoveStatusToString(onvif_MoveStatus status);
onvif_MoveStatus 			    onvif_StringToMoveStatus(const char * str);

const char * 				    onvif_OSDTypeToString(onvif_OSDType type);
onvif_OSDType 				    onvif_StringToOSDType(const char * type);

const char * 				    onvif_OSDPosTypeToString(onvif_OSDPosType type);
onvif_OSDPosType 			    onvif_StringToOSDPosType(const char * type);

const char * 				    onvif_OSDTextTypeToString(onvif_OSDTextType type);
onvif_OSDTextType 			    onvif_StringToOSDTextType(const char * type);

const char * 				    onvif_BacklightCompensationModeToString(onvif_BacklightCompensationMode mode);
onvif_BacklightCompensationMode onvif_StringToBacklightCompensationMode(const char * str);

const char * 				    onvif_ExposureModeToString(onvif_ExposureMode mode);
onvif_ExposureMode 			    onvif_StringToExposureMode(const char * str);

const char * 				    onvif_ExposurePriorityToString(onvif_ExposurePriority mode);
onvif_ExposurePriority 		    onvif_StringToExposurePriority(const char * str);

const char * 				    onvif_AutoFocusModeToString(onvif_AutoFocusMode mode);
onvif_AutoFocusMode 		    onvif_StringToAutoFocusMode(const char * str);

const char * 				    onvif_WideDynamicModeToString(onvif_WideDynamicMode mode);
onvif_WideDynamicMode 		    onvif_StringToWideDynamicMode(const char * str);

const char * 				    onvif_IrCutFilterModeToString(onvif_IrCutFilterMode mode);
onvif_IrCutFilterMode 		    onvif_StringToIrCutFilterMode(const char * str);

const char * 				    onvif_WhiteBalanceModeToString(onvif_WhiteBalanceMode mode);
onvif_WhiteBalanceMode 		    onvif_StringToWhiteBalanceMode(const char * str);

const char * 				    onvif_EFlipModeToString(onvif_EFlipMode mode);
onvif_EFlipMode 			    onvif_StringToEFlipMode(const char * str);

const char * 				    onvif_ReverseModeToString(onvif_ReverseMode mode);
onvif_ReverseMode 			    onvif_StringToReverseMode(const char * str);

const char *				    onvif_DiscoveryModeToString(onvif_DiscoveryMode mode);
onvif_DiscoveryMode			    onvif_StringToDiscoveryMode(const char * str);

const char * 				    onvif_SetDateTimeTypeToString(onvif_SetDateTimeType type);
onvif_SetDateTimeType		    onvif_StringToSetDateTimeType(const char * str);

const char * 				    onvif_StreamTypeToString(onvif_StreamType type);
onvif_StreamType			    onvif_StringToStreamType(const char * str);

const char * 				    onvif_TransportProtocolToString(onvif_TransportProtocol type);
onvif_TransportProtocol		    onvif_StringToTransportProtocol(const char * str);

const char *				    onvif_DynamicDNSTypeToString(onvif_DynamicDNSType type);
onvif_DynamicDNSType		    onvif_StringToDynamicDNSType(const char * str);

const char * 				    onvif_TrackTypeToString(onvif_TrackType type);
onvif_TrackType				    onvif_StringToTrackType(const char * str);

const char * 				    onvif_PropertyOperationToString(onvif_PropertyOperation type);
onvif_PropertyOperation		    onvif_StringToPropertyOperation(const char * str);

const char * 				    onvif_RecordingStatusToString(onvif_RecordingStatus status);
onvif_RecordingStatus		    onvif_StringToRecordingStatus(const char * str);

const char *				    onvif_SearchStateToString(onvif_SearchState state);
onvif_SearchState			    onvif_StringToSearchState(const char * str);

const char * 				    onvif_RotateModeToString(onvif_RotateMode mode);
onvif_RotateMode			    onvif_StringToRotateMode(const char * str);

const char *                    onvif_ScopeDefinitionToString(onvif_ScopeDefinition def);
onvif_ScopeDefinition           onvif_StringToScopeDefinition(const char * str);

const char *                    onvif_Dot11AuthAndMangementSuiteToString(onvif_Dot11AuthAndMangementSuite req);
onvif_Dot11AuthAndMangementSuite    onvif_StringToDot11AuthAndMangementSuite(const char * str);

const char *                    onvif_Dot11CipherToString(onvif_Dot11Cipher req);
onvif_Dot11Cipher               onvif_StringToDot11Cipher(const char * str);

const char *                    onvif_Dot11SignalStrengthToString(onvif_Dot11SignalStrength req);
onvif_Dot11SignalStrength       onvif_StringToDot11SignalStrength(const char * str);

const char *                    onvif_Dot11StationModeToString(onvif_Dot11StationMode req);
onvif_Dot11StationMode          onvif_StringToDot11StationMode(const char * str);

const char *                    onvif_Dot11SecurityModeToString(onvif_Dot11SecurityMode req);
onvif_Dot11SecurityMode         onvif_StringToDot11SecurityMode(const char * str);

const char *                    onvif_PTZPresetTourOperationToString(onvif_PTZPresetTourOperation op);
onvif_PTZPresetTourOperation    onvif_StringToPTZPresetTourOperation(const char * str);

const char *                    onvif_PTZPresetTourStateToString(onvif_PTZPresetTourState st);
onvif_PTZPresetTourState        onvif_StringToPTZPresetTourState(const char * str);

const char *                    onvif_PTZPresetTourDirectionToString(onvif_PTZPresetTourDirection dir);
onvif_PTZPresetTourDirection    onvif_StringToPTZPresetTourDirection(const char * str);

#ifdef PROFILE_C_SUPPORT

const char *				    onvif_DoorPhysicalStateToString(onvif_DoorPhysicalState state);
onvif_DoorPhysicalState			onvif_StringToDoorPhysicalState(const char * str);

const char *				    onvif_LockPhysicalStateToString(onvif_LockPhysicalState state);
onvif_LockPhysicalState			onvif_StringToLockPhysicalState(const char * str);

const char *				    onvif_DoorAlarmStateToString(onvif_DoorAlarmState state);
onvif_DoorAlarmState			onvif_StringToDoorAlarmState(const char * str);

const char *				    onvif_DoorTamperStateToString(onvif_DoorTamperState state);
onvif_DoorTamperState			onvif_StringToDoorTamperState(const char * str);

const char *				    onvif_DoorFaultStateToString(onvif_DoorFaultState state);
onvif_DoorFaultState			onvif_StringToDoorFaultState(const char * str);

const char *				    onvif_DoorModeToString(onvif_DoorMode mode);
onvif_DoorMode			        onvif_StringToDoorMode(const char * str);

#endif // end of PROFILE_C_SUPPORT

#ifdef DEVICEIO_SUPPORT

const char *                    onvif_RelayModeToString(onvif_RelayMode mode);
onvif_RelayMode                 onvif_StringToRelayMode(const char * str);

const char *                    onvif_RelayIdleStateToString(onvif_RelayIdleState state);
onvif_RelayIdleState            onvif_StringToRelayIdleState(const char * str);

const char *                    onvif_RelayLogicalStateToString(onvif_RelayLogicalState state);
onvif_RelayLogicalState         onvif_StringToRelayLogicalState(const char * str);

const char *                    onvif_DigitalIdleStateToString(onvif_DigitalIdleState state);
onvif_DigitalIdleState          onvif_StringToDigitalIdleState(const char * str);

const char *                    onvif_ParityBitToString(onvif_ParityBit type);
onvif_ParityBit                 onvif_StringToParityBit(const char * str);

const char *                    onvif_SerialPortTypeToString(onvif_SerialPortType type);
onvif_SerialPortType            onvif_StringToSerialPortType(const char * str);

#endif // end of DEVICEIO_SUPPORT

#ifdef THERMAL_SUPPORT
const char *                    onvif_PolarityToString(onvif_Polarity type);
onvif_Polarity                  onvif_StringToPolarity(const char * str);
#endif // end of THERMAL_SUPPORT

#ifdef RECEIVER_SUPPORT
const char *                    onvif_ReceiverModeToString(onvif_ReceiverMode mode);
onvif_ReceiverMode              onvif_StringToReceiverMode(const char * str);

const char *                    onvif_ReceiverStateToString(onvif_ReceiverState state);
onvif_ReceiverState             onvif_StringToReceiverState(const char * str);
#endif // end of RECEIVER_SUPPORT

#ifdef IPFILTER_SUPPORT
const char *                    onvif_IPAddressFilterTypeToString(onvif_IPAddressFilterType type);
onvif_IPAddressFilterType       onvif_StringToIPAddressFilterType(const char * str);
#endif // end of IPFILTER_SUPPORT


#ifdef __cplusplus
}
#endif

#endif	/* end of ONVIF_COMM_H */





