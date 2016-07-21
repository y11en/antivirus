#ifndef __KLG_H
#define __KLG_H

#define _AVPGNAME_		KLIF
#define AVPGNAME		"KLIF"
#define AVPGNAME_W		L"KLIF"

#define HOOK_INTERFACE_NUM 12			// This is AP INTERFACE NUMBER!!! Change if IOCTL struct changed

#define HOOK_REGESTRY_INTERFACE_NUM 10	// This is AP INTERFACE NUMBER!!! Change if changed regestry save data structures

#define PROCNAMELEN				32
#define MAXPATHLEN				270
#define MAX_MSG_LEN				512
#define MAXREQUEST				0x7FFFFFFFL
#define DISKNAMEMAXLEN			1024	// in WCHAR
#define DISKNAMEMAXLEN_OLD		64	// in WCHAR
// Gruzdev --------------------------------------------------
#define MAX_REGPATH_LEN			4096	// WCHARs
#define MAX_KEYVALUENAME_LEN	256		// ------
//-----------------------------------------------------------

#define __SID_LENGTH			32

#define _FILE_MAX_PATHLEN	540 * sizeof(WCHAR)	// запас буфера для обычной операции
#define _VERY_LONG_NAME		0xfff0				// максимальное имя объекта

#define _SYSTEM_APPLICATION	-1
#define _DRIVER_SPECIFIC_ID	0

#define INVALID_GROUP_ID		-1

#define ACTIVITY_WATCHDOG_TIMEOUT	3/*min*/*60/*(sec)*/ // 1 minutes in second
#define DEADLOCKWDOG_TIMEOUT			3 // in sec

#define _DIRECT_VOLUME_ACCESS_NAME		" ?dva|"
#define _DIRECT_VOLUME_ACCESS_NAME_W	L" ?dva|"
#define _PIPE_NAME							"pipe:"
#define _PIPE_NAME_W							L"pipe:"

#define _INETSWIFT_PREFIX			"INETSWIFT"
#define _INETSWIFT_PREFIX_BLEN		(sizeof(_INETSWIFT_PREFIX) - sizeof(CHAR))

#define _INETSWIFT_PREFIX_FULL		"INETSWIFT00000000"
#define _INETSWIFT_PREFIX_FULL_BLEN	(sizeof(_INETSWIFT_PREFIX_FULL) - sizeof(CHAR))


/*
Рарезервировать диапазоны приоритетов
1 - 99 - инофрмационные приложения. НЕ рекомендуется запрещать запросы
100 - 899 - стандартные(блокирующие) приложения
900 - 999 - "непрозрачные" фильтры (FLT_A_DENYPASS2LOWPR)
*/
#define AVPG_INFOPRIORITY				50
#define AVPG_STANDARTRIORITY			500
#define AVPG_INVISIBLEPRIORITY		950

// hard code idetificators
#define AVPG_Driver_Specific   	0
#define AVPG_KLGuard    			1
#define AVPG_StandAlone    		2
#define AVPG_MPraguePlugin			3
#define AVPG_Debug					4		// you have to use this value for pass debugers and other development tools

#define KL_PDM						10		// you have to use this value for pass debugers and other development tools

#define KL_Guard_Dumps				50		// you have to use this value for pass debugers and other development tools
#define KL_Guard_Dumps_End			99		// you have to use this value for pass debugers and other development tools

#define PersonalFireWall			100		// 
#define PersonalFireWall_End		199		// 

#define MailChecker					200
#define MailChecker_End             299

#define MailChecker_PPP             301

#define RebootDefender				401

#define VirLabProtection			501
#define InstallerProtection			909

// -----------------------------------------------------------------------------------------
/*
non popup -> deny, log

                             Popup Client                   Non Popup Client	
App state\ Filter type  popup			non popup	|   popup             non popup
-----------------------------------------------------------------------------------------
Not registered          pass			pass		|	verdict				verdict
Active                  depend			verdict		|	depend              verdict
Sleep                   pass			pass		|   pass				pass
Dead					pass			pass		|	verdict + log		verdict
-----------------------------------------------------------------------------------------

                               Popup Client                   Non Popup Client	
App state\ Filter type    info       notify        |    info               notify
-----------------------------------------------------------------------------------------
Not registered                                     |                               
Active                     +           +           |     +                   +      
Sleep                                              |                            
Dead                                               |                              
-----------------------------------------------------------------------------------------
*/
// -----------------------------------------------------------------------------------------
#define _CLIENT_FLAG_NONE							0x0000
#define _CLIENT_FLAG_SAVE_FILTERS				0x00001
#define _CLIENT_FLAG_USE_DRIVER_CACHE			0x00002
#define _CLIENT_FLAG_USE_BLUE_SCREEN_REQUEST	0x00004	// enable processing during deadlock
#define _CLIENT_FLAG_POPUP_TYPE					0x00008	// popup - if not resident
#define _CLIENT_FLAG_CACHDEADLOCK				0x00100	// place deadlock to cache if possible
#define _CLIENT_FLAG_WITHOUTWATCHDOG			0x00200	// this client doesn't have WatchDog thread
#define _CLIENT_FLAG_WITHOUTCASCADE				0x00400	// this client doesn't want to use cascade processing
#define _CLIENT_FLAG_WANTIMPERSONATE			0x00800	// this client want save security information for impersonation
#define _CLIENT_FLAG_DONTPROCEEDDEADLOCK		0x01000	// this client don't want save in queue deadlock event
#define _CLIENT_FLAG_PAUSEONREGISTER			0x02000	// with this flag client will be in pause after restore from save
#define _CLIENT_FLAG_PARALLEL					0x04000	// allow parallel working severals clients (load dispatching)
#define _CLIENT_FLAG_PAUSED						0x08000 // client paused
#define _CLIENT_FLAG_WATCHDOG_USEPAUSE			0x10000 // allow pause by watchdog thread when deadlock timeout expired
#define _CLIENT_FLAG_STRONG_PROCESSING			0x20000 // reassign deadlock events on other subclient

#include <pshpack1.h>

// driver specific log_icon id
typedef enum _LOG_ICON
{
	LogIcon_Default,	// equivalence to _VERDICT
	LogIcon_Pass,		//
	LogIcon_Discard,	//
	LogIcon_Kill,		//
	LogIcon_WDog,
	LogIcon_SessionStarted,
	LogIcon_DriverShutDowned,
	LogIcon_DriverUnloaded,
	LogIcon_AppRegistered,
	LogIcon_AppSessionStarted,
	LogIcon_AppUnLoaded,
	LogIcon_AppIsDead,
	LogIcon_StateChanged_Speep,
	LogIcon_StateChanged_Active,
}LOG_ICON;

// -----------------------------------------------------------------------------------------
typedef struct _APP_REGISTRATION
{
	ULONG		m_InterfaceVersion;			// you MUST fill this value current interface version (HOOK_INTERFACE_NUM)
	ULONG		m_AppID;					//!=_SYSTEM_APPLICATION, if AppID = _DRIVER_SPECIFIC_ID - diver assigned internal value
	ULONG		m_Priority;					// Better if bigger :) If priority equivalence will be select application with latest registration
	DWORD		m_ClientFlags;				// see _CLIENT_FLAG_???
	CHAR		m_WhistleUp[PROCNAMELEN];	// Win 9x: HANDLE opened event, Win NT: semaphore's name
	CHAR		m_WFChanged[PROCNAMELEN];	//  Important! len<PROCNAMELEN / If you not need an any signalyou may placing 0
	ULONG		m_CurProcId;				// Client! Fill this field GetCurrentProcessId()
	CHAR		m_LogFileName[MAXPATHLEN];	// if "\0" than driver log not required
	ULONG		m_CacheSize;				// driver cache size
	DWORD		m_BlueScreenTimeout;		// timeout for detecting deadlock
	DWORD		m_SyncAddr;					// in 9x windows application MUST fill this filed Win16 mutex address
											// you can obtain value by Kernel32.dll funcion ordinal-93
} APP_REGISTRATION,*PAPP_REGISTRATION;

// Request application status
typedef enum _APPSTATE_REQUEST
{
	_AS_DontChange				= 0,	// get current state
	_AS_GoSleep					= 1,	// Don't filter any events (switch in SleepMode)
	_AS_GoActive				= 2,	// Activate filters (switch in ActiveMode)
	_AS_Unload					= 3,	// Unload application (Application MUST inform driver befor exit)
	_AS_UnRegisterStayResident	= 4,	// Unload and stay resident (Application inform that interface is closing but driver can proceed further)
	_AS_IncreaseUnsafe			= 5,	// Increase client's safe value
	_AS_DecreaseUnsafe			= 6,	// Decrease client's safe value
	_AS_SetPauseOnStart			= 7,	// Set flag _CLIENT_FLAG_PAUSEONREGISTER on client. On register from regestry client will be in pause
	_AS_ResetPauseOnStart		= 8,	// Reset flag _CLIENT_FLAG_PAUSEONREGISTER on client
	_AS_SaveFilters				= 9,	// Save filters to regestry
} APPSTATE_REQUEST, *PAPPSTATE_REQUEST;

// answers for APPSTATE_REQUEST
typedef enum _APPSTATE_					// current application status
{		
	_AS_Dead,								// is dead
	_AS_Sleep,								// is in SleepMode
	_AS_Active,								// is active
	_AS_NotRegistered,					// not registered
} APPSTATE, *PAPPSTATE;

// check client event state
typedef struct _HDSTATE
{
	ULONG AppID;							// 
	ULONG Activity;						// APPSTATE_REQUEST on call, APPSTATE on answer. if request Increase/Descrease Unsafe - on exit Activity == Client's safe value
	ULONG QLen;								// Lenght of queue (for selected AppID)
	ULONG QUnmarkedLen;					// Lenght of nonmarked queue (for selected AppID)
} HDSTATE,*PHDSTATE;

// current client info
typedef struct _CLIENT_INFO
{
	ULONG		m_Priority;					// 
	DWORD		m_ClientFlags;				// see _CLIENT_FLAG_???
	ULONG		m_CacheSize;				// 
	DWORD		m_AppID;		// 
} CLIENT_INFO,*PCLIENT_INFO;

typedef struct _FLOWTIMEOUT			// Current Timeout = (MaxT - MinT) / 2
{	
	ULONG m_AppID;							// 
	ULONG m_MinT;							// in mSec
	ULONG m_MaxT;							// MaxT - MinT
	ULONG m_DeltaT;						// Step = (MaxT - MinT) / DeltaT
	BOOLEAN m_Sign;						// +- delta
} FLOWTIMEOUT,*PFLOWTIMEOUT;
// -----------------------------------------------------------------------------------------
// verdict values
typedef enum _VERDICT
{
	Verdict_Default						= 0,
	Verdict_Pass						= 1,
	Verdict_Discard						= 2,
	Verdict_Kill						= 3,
	Verdict_WDog						= 4,
	Verdict_Continue					= 5,
	Verdict_UserRequest					= 6,	// this verdict (and next) is using for Event with ProcessingType == CheckProcessing 
	Verdict_NotFiltered					= 7,
	Verdict_Debug						= 8, // only in debug version - make breakpoint after filtering. changed on default in release
} VERDICT, *PVERDICT;

#define _PASS_VERDICT(ver) ((ver == Verdict_NotFiltered) || (ver == Verdict_Pass) || (ver == Verdict_Default))

#pragma warning (disable : 4200)
typedef struct _SET_VERDICT
{
	ULONG		m_AppID;
	VERDICT	m_Verdict;
	ULONG		m_Mark;
	ULONG		m_ExpTime;	//in Sec/1000 time form answer till expiration
} SET_VERDICT,*PSET_VERDICT;

#pragma warning (default : 4200)

// Filters ---------------------------------------------------------------------------------

#define _INVALID_FILTER_ID		0
// Flags------------------------------------------------------------------------------------
/*
 Рассмотреть комбинацию флагов
	FLT_A_INFO и FLT_A_NOTIFY не могут быть использованы с типом FLT_A_POPUP
	FLT_A_INFO, FLT_A_NOTIFY активны только при статусе приложения AS_Active
*/

#define FLT_A_SYSTEM				0x80000000	// This filter can't be deleted
#define FLT_A_DEFAULT				0x00000000	// Allow (default)
#define FLT_A_POPUP					0x00000001	// Show request (popup)
#define FLT_A_LOG					0x00000002	// Write to log file
#define FLT_A_DENY					0x00000004  // Deny by default
#define FLT_A_DENYPASS2LOWPR		0x00000010  // не пропускать к более низкоуровневым задачам для фильтрации

#define FLT_A_NOTIFY				0x00000020  // посигналить WhistlerUp и пропустить дальше
#define FLT_A_INFO					0x00000040  // поместить в очередь событий чтобы морда замаркировала и удалила, на ожидание ответа от морды не ставить(Pass immediately)
#define FLT_A_USERLOG				0x00000080  // user want to make own log. this flag sets flag _EVENT_FLAG_LOGTHIS in EVENT_TRANSMIT

#define FLT_A_DROPCACHE				0x00000100  // drop cache value
#define FLT_A_SAVE2CACHE			0x00000200  // save answer in cache (if pass)	// on event will be setted _EVENT_FLAG_SAVECACHE_AVAILABLE
#define FLT_A_USECACHE				0x00000400  // check cache
#define FLT_A_RESETCACHE			0x00000800  // reset all cache value

#define FLT_A_MAPORIGINALPACKET		0x00001000  // поместить пакет из перехватчика в общую память //? first byte is mask for buffer - first bit sign that data changed in r3
#define FLT_A_TOUCH					0x00002000  // 'touch filter' - filter after generate info will be deleted  //this flag sets flag _EVENT_FLAG_TOUCH in EVENT_TRANSMIT
#define FLT_A_DISABLED				0x00004000  // this filter is disabled (see FLTCTL_???)
#define FLT_A_PROCESSCACHEABLE		0x00008000  // process name can be used in calc CacheID

#define FLT_A_CHECKNEXTFILTER		0x00010000  // after check cache try to search next filter...
#define FLT_A_PASS					0x00020000  // default answer is pass
#define FLT_A_STOPPROCESSING		0x00040000  // stop filtering for current client (not filtered result)
#define FLT_A_DELETE_ON_MARK		0x00080000  // if filter is info - event will be deleted on mark

#define FLT_A_ALERT1				0x00100000  // user ALERT1
#define FLT_A_DBG_BREAK				0x00200000  // break after verdict // only in dbg driver version

#define FLT_A_SKIPPARAMS			0x01000000  // dont' check param

#define FLT_A_HAVE_MAPPED_PARAMS	0x02000000  // driver set this flag for filter if it have params with flag _FILTER_PARAM_FLAG_MAP_TO_EVENT
#define FLT_A_DISPATCH_MIRROR		0x04000000  // this filter has mirror filter on dispatch level


//Flags for Param field
//#define FLT_PARAM				0xff000000
//#define FLT_PARAM_DONT_CHECK	0x00000000	// don't check this param

//!!check params on set
typedef enum _FLT_PARAM_OPERATION
{
// string
	FLT_PARAM_WILDCARD		= 0,	// param MUST be zero-terminated
	FLT_PARAM_WILDCARDSENS	= 1,	// Case sensivity. param MUST be zero-terminated 
//  byte per byte
	FLT_PARAM_EUA				= 2,				
	FLT_PARAM_AND				= 3,
	FLT_PARAM_ABV				= 4,
	FLT_PARAM_BEL				= 5,
	FLT_PARAM_NOP				= 6,		// without operation
// invers comparing
	FLT_PARAM_MORE				= 7,
	FLT_PARAM_LESS				= 8,
// mask comparing 
	FLT_PARAM_MASK				= 9,	// m_ParamValue (FILTER_PARAM) must contain value_to_compare as first
										// param and mask immediately beyond one
										// m_ParamSize = sizeof(value_to_compare) + sizeof(mask) = 2*sizeof(value_to_compare)

	FLT_PARAM_EQU_LIST			= 10,	// m_ParamValue (FILTER_PARAM) must contain values
										// m_ParamSize = sizeof(value_to_compare) * items_count
	FLT_PARAM_MASK_LIST			= 11,	// m_ParamValue (FILTER_PARAM) must contain values
										// m_ParamSize = SUM(strlen(value_to_compare) + 1) + common zero terminations == multi-string
	FLT_PARAM_MASKUNSENS_LIST	= 12,	// m_ParamValue (FILTER_PARAM) must contain values
										// m_ParamSize = SUM(strlen(value_to_compare) + 1) + common zero terminations == multi-string
	FLT_PARAM_MASKUNSENS_LISTW	= 13,	// m_ParamValue (FILTER_PARAM) must contain values
										// m_ParamSize = SUM(wstrlen(value_to_compare) + sizeof(WCHAR)) + common zero terminations (2 bytes) == unicode multi-string
	FLT_PARAM_MASKSENS_LISTW	= 14,	// m_ParamValue (FILTER_PARAM) must contain values
										// m_ParamSize = SUM(wstrlen(value_to_compare) + sizeof(WCHAR)) + common zero terminations (2 bytes) == unicode multi-string
	FLT_PARAM_CHECK_SID			= 15,	
}FLT_PARAM_OPERATION, *PFLT_PARAM_OPERATION;

#define	_FILTER_PARAM_FLAG_NONE					0x0000
#define	_FILTER_PARAM_FLAG_CACHABLE				0x0001		// this parameter used for calcing CasheID
#define	_FILTER_PARAM_FLAG_INVERS_OP			0x0002		// operation result will be = !result
#define	_FILTER_PARAM_FLAG_CACHEUPREG			0x0004		// this parameter used for calcing CasheID with up register
#define	_FILTER_PARAM_FLAG_MUSTEXIST			0x0008		// this parameter MUST exist in Event
#define	_FILTER_PARAM_FLAG_MAP_TO_EVENT			0x0010		// copy this param to event (see _SINGLE_PARAM_FLAG_MAPPED_FILTER_PARAM)

#define	_FILTER_PARAM_FLAG_DEBUG_BREAK			0x0100		// in debug driver make a breakpoint
#define _FILTER_PARAM_FLAG_SPECIAL_THISISPROCID	0x0200		// this parameter is special -> process id. size = sizeof (ULONG)
#define _FILTER_PARAM_FLAG_SPECIAL_THISISTHREADID 0x0400		// this parameter is special -> thread id. size = sizeof (ULONG)


#define	_FILTER_PARAM_FLAG_SINGLEID				0x2000		// don't search such id in filter (optimize). todo: auto set this flag
#define	_FILTER_PARAM_INTERNAL_OPTIMIZER1		0x1000		// used in driver for optimizing 

/* remarks for _FILTER_PARAM_FLAG_???
	if filter param has _FILTER_PARAM_FLAG_MAP_TO_EVENT | _FILTER_PARAM_FLAG_CACHABLE than CachID will be calculated on this param data too (not only event data)
end remarks */

#pragma warning (disable : 4200)
typedef struct _FILTER_PARAM
{
	ULONG						m_ParamID;			// event specific
	DWORD						m_ParamFlags;		// see _FILTER_PARAM_FLAG_???
	FLT_PARAM_OPERATION	m_ParamFlt;					// see FLT_PARAM_OPERATION_??? Value will be compared with FLT_PARAM_DW_??? operation
	ULONG						m_ParamSize;		// length binary data
	BYTE						m_ParamValue[0];	//
} FILTER_PARAM, *PFILTER_PARAM;

// Comment
#define FLT_ID_NOTDEF		0xffffffff
// Если произошла перестройка списка в онлайне то ставится соответстующий event
// и навигацию FLTCTL_FIRST, FLTCTL_NEXT нужно начать с начала.
#define FLTCTL_FIRST			0					// get first filter in list
//_GetFirst STATUS_SUCCESS | STATUS_UNSUCCESSFUL
#define FLTCTL_NEXT				1				// get next filter in list
//_GetNext заполнить структуру + STATUS_SUCCESS | STATUS_UNSUCCESSFUL | STATUS_BUFFER_OVERFLOW
#define FLTCTL_FIND				3				// find filter in list
// _IsFiltered заполнить (FilterID !!!) STATUS_SUCCESS | STATUS_UNSUCCESSFUL
#define FLTCTL_DEL				4				// Delete filter by id. FilterID field must be specified.
// _DelFilter STATUS_SUCCESS | STATUS_UNSUCCESSFUL
#define FLTCTL_ADD				5				// Add filter at the end of filter's queue
//_AddFilter заполнить (FilterID !!!) STATUS_SUCCESS | STATUS_UNSUCCESSFUL
#define FLTCTL_ADD2TOP			6				// Add filter at the top of filter's queue
//_AddFilter заполнить (FilterID !!!) STATUS_SUCCESS | STATUS_UNSUCCESSFUL
#define FLTCTL_ADD2POSITION		7			// Add filter after specified filter. FilterID must be specified.
//_AddFilter заполнить (FilterID !!!) STATUS_SUCCESS | STATUS_UNSUCCESSFUL
#define FLTCTL_DISABLE_FILTER		8			// DisableFilter
//_AddFilter заполнить (FilterID !!!) STATUS_SUCCESS | STATUS_UNSUCCESSFUL
#define FLTCTL_ENABLE_FILTER		9			// EnableFilter
//_AddFilter заполнить (FilterID !!!) STATUS_SUCCESS | STATUS_UNSUCCESSFUL
#define FLTCTL_SET_FILTER_GROUP_ID		10			// заполнить FilterID & m_HookID == GroupID STATUS_SUCCESS | STATUS_UNSUCCESSFUL

//_AddFilter заполнить (FilterID !!!) STATUS_SUCCESS | STATUS_UNSUCCESSFUL
#define FLTCTL_CHANGE_FILTER_PARAM		11			// заполнить FilterID & fill ONE filter_param

#define FLTCTL_RESET_FILTERS		100			// Delete all filters


#define _SINGLE_PARAM_FLAG_NONE					0x000	//
#define _SINGLE_PARAM_FLAG_POINTER				0x001	// this mean that ParamValue is (dword) pointer on memory block
#define _SINGLE_PARAM_FLAG_MAPPED_FILTER_PARAM	0x002	// this parameter from FILTER_TRANSMIT (see _FILTER_PARAM_FLAG_MAP_TO_EVENT). This event param contain _FILTER_PARAM as is in filter
#define _SINGLE_PARAM_FLAG_INFORMATION			0x004	// this parameter is ignored by cache

typedef struct _SINGLE_PARAM
{
	ULONG ParamSize;								// length binary data
	ULONG ParamID;									// see hook specific describes
	DWORD ParamFlags;								// see _SINGLE_PARAM_FLAG_???
	BYTE  ParamValue[0];							// 
}SINGLE_PARAM, *PSINGLE_PARAM;

typedef enum _PROCESSING_TYPE
{
	PreProcessing,									// before
	PostProcessing,									// after
	AnyProcessing,									// pre- or post-
	CheckProcessing,								// if exist filter
	PreDispatch,									// filter in dispatch 
}PROCESSING_TYPE;

typedef struct _FILTER_TRANSMIT
{
	ULONG	m_FltCtl;								// for exchange with driver (see FLTCTL_???)
	ULONG	m_AppID;									// client id
	ULONG	m_FilterID;								// unique filter id
	ULONG	m_HookID;								// 
	ULONG	m_FunctionMj;							// 
	ULONG	m_FunctionMi;							// 
	PROCESSING_TYPE m_ProcessingType;		// see PROCESSING_TYPE
	ULONG	m_Flags;									// see FLT_A_???
	ULONG	m_Timeout;								// timeout (request window life time)
	__int64 m_Expiration;						// filter time life
	CHAR	m_ProcName[PROCNAMELEN];			// Wildcard "*" possible
	ULONG	m_ParamsCount;							// parameters value
	BYTE	m_Params[0];							// FILTER_PARAM 
} FILTER_TRANSMIT,*PFILTER_TRANSMIT;		// Filter apply if all fields are congruent only


// -----------------------------------------------------------------------------------------
#define INVCTL_ADD			0						// remove from invisible list (calling thread)
#define INVCTL_DEL			1						// add to invisible list (calling thread)
#define INVCTL_RESET		2							// reset invisible list (all thread)
#define INVCTL_DELBYID		3						// delete from invisible list by ID
#define INVCTL_PROCADD		4						// 
#define INVCTL_GETCOUNTER	5
#define INVCTL_ADDBYID		6						// 

typedef struct
{
	ULONG m_InvCtl;								// for exchange with driver (see INVCTL_???)
	ULONG m_AppID;									// идентификатор приложения / во время работы приложения
	DWORD m_ThreadID;								// идентификатор потока, валидно на выходе при INVCTL_ADD, INVCTL_DEL и на входе при INVCTL_DELBYID
}INVISIBLE_TRANSMIT, *PINVISIBLE_TRANSMIT;

// -----------------------------------------------------------------------------------------
typedef struct _GET_EVENT{
	ULONG	m_AppID;									//
	ULONG	Mark;										// (уникальный маркер, hWnd is good enought)
} GET_EVENT,*PGET_EVENT;

typedef struct _ORIGINAL_PACKET				// for _PARAM_OBJECT_ORIGINAL_PACKET
{				
	ULONG	m_Size;
	PVOID	m_OriginalBufferPtr;			// r0
	PVOID	m_UserOffset;					// r3
	ULONG	m_Flags;
}ORIGINAL_PACKET, *PORIGINAL_PACKET;

typedef struct _EXEC_ON_HOLD
{
	ULONG	m_FunctionID;							// see hookspec.h
	BYTE	m_ExecParams[0];						// function specific
}EXEC_ON_HOLD, *PEXEC_ON_HOLD;

typedef struct _YELD_EVENT
{
	ULONG	m_AppID;								//
	ULONG	Mark;									// 
	ULONG	Timeout;								// new timeout
	ULONG	YeldParameterSize;						// sizeof YeldParams
	BYTE	YeldParams[0];							// EXEC_ON_HOLD
} YELD_EVENT,*PYELD_EVENT;

#define _EVENT_FLAG_NONE					0x00000		//_EVENT_FLAG_PREPROCESSING
#define _EVENT_FLAG_POSTPROCESSING			0x00001
#define _EVENT_FLAG_DEADLOCK_DETECTED		0x00002		// не было ни одного GetEvent (event не был помаркирован)
#define _EVENT_FLAG_RESCHEDULED				0x00004		// помаркированный евент (но отвалившийся на Yeld) помешен ПОВТОРНО в очередь как информационный для клиента
#define _EVENT_FLAG_LOGTHIS					0x00008		// client have to log this event
#define _EVENT_FLAG_TOUCH					0x00010		// touch filter fired
#define _EVENT_FLAG_SAVECACHE_AVAILABLE		0x00020		// if filter hase FLT_A_SAVE2CACHE this flag will be setted on event
#define _EVENT_FLAG_POPUP					0x00040		// thread stopped
#define _EVENT_FLAG_OBJECT_ATTENDED			0x00080		// event object saved in event queue
#define _EVENT_FLAG_WAITINGUNSAFE			0x00100		// unsafe detected
#define _EVENT_FLAG_DELETED_ON_MARK			0x00200		// event was deleted on mark (see FLT_A_DELETE_ON_MARK)
#define _EVENT_FLAG_64BIT					0x00400		// 64 bit op
#define _EVENT_FLAG_ALERT1					0x01000		// event generated filter with FLT_A_ALERT1 flag

#define _EVENT_FLAG_AUTO_VERDICT_DEFAULT	0x10000		// for filter with Verdict == Verdict_Default
#define _EVENT_FLAG_AUTO_VERDICT_PASS		0x20000		// for filter with Verdict == Verdict_Pass
#define _EVENT_FLAG_AUTO_VERDICT_DISCARD	0x40000		// for filter with Verdict == Verdict_Discard
//#define _EVENT_FLAG_AUTO_VERDICT_KILL		0x80000		// for filter with Verdict == Verdict_Kill

typedef struct _EVENT_TRANSMIT
{
	ULONG	m_HookID;								// 
	ULONG	m_FunctionMj;							// 
	ULONG	m_FunctionMi;							// 
	ULONG	m_FilterID;								// идентификатор фильтра который вызвал событие
	DWORD	m_EventFlags;							// bitmask see _EVENT_FLAG_???
	ULONG	m_Timeout;
	CHAR	m_ProcName[PROCNAMELEN];
	ULONG	m_ProcessID;							// process id
	ULONG	m_ParamsCount;							// число параметров (лежат после структуры)
	BYTE	m_Params[0];							// SINGLE_PARAM
} EVENT_TRANSMIT,*PEVENT_TRANSMIT;

// -----------------------------------------------------------------------------------------

typedef struct _FILTER_EVENT_PARAM
{
	ULONG HookID;									// FLTTYPE_???
	ULONG FunctionMj;								// fun. major
	ULONG FunctionMi;								// fun. minor
	ULONG ThreadID;
	CHAR  ProcName[PROCNAMELEN];				// 
	ULONG ProcessID;
	PROCESSING_TYPE ProcessingType;			// 
	DWORD UnsafeValue;							// safety value - 0 if ReplyWaiting absolutly safe (deadlock free)
	ULONG ParamsCount;
	BYTE Params[0];								// SINGLE_PARAM 
}FILTER_EVENT_PARAM, *PFILTER_EVENT_PARAM;


// -----------------------------------------------------------------------------------------
// driver additional status
#define _DRV_FLAG_NONE						0x000000
#define _DRV_FLAG_DEBUG						0x000001
#define _DRV_FLAG_TSP_OK					0x000002
#define _DRV_FLAG_DIRECTSYSCALL				0x000004
#define _DRV_FLAG_NO_ISTREAM_SUPPORT		0x000008
#define _DRV_FLAG_MINIFILTER				0x000010
#define _DRV_FLAG_HAS_DISPATCH_FLT			0x000020
#define _DRV_FLAG_HAS_SYSPATCH				0x000040
#define _DRV_FLAG_SUPPORT_NET_SAFESTREAMS	0x000100
#define _DRV_FLAG_SUPPORT_UNLOAD			0x000200
#define _DRV_FLAG_SUPPORT_PROC_HASH			0x000400
#define _DRV_FLAG_ISWIFT_DISABLED			0x001000
#define _DRV_FLAG_INETSWIFT_DISABLED		0x002000
#define _DRV_FLAG_FILEID_ID					0x010000
#define _DRV_FLAG_FILEID_INDEX				0x020000
#define _DRV_FLAG_TIMING					0x040000
#define _DRV_FLAG_PRECREATE_DISABLED		0x100000

typedef struct _ADDITIONALSTATE
{
	ULONG ReplyWaiterEnters;
	ULONG DeadlockCount;
	ULONG FilterEventEntersCount;
	ULONG DrvFlags;								// see _DRV_FLAG_???
	ULONG DirectSysCallStartId;   
} ADDITIONALSTATE,*PADDITIONALSTATE;

//--------------------------------------------------------------------------------------------
#define DeclareW32Service(service) _##service,
enum AVPG_W32_Services {
	DeclareW32Service(W32ServGetVersion)
	DeclareW32Service(W32ServFilterEvent)
	DeclareW32Service(W32ServIsNeedFltEvAndNotInvisTh)
	DeclareW32Service(W32ServActivity)
	DeclareW32Service(W32ServGetEvent)
	DeclareW32Service(W32ServImpersonateThread)
	DeclareW32Service(W32ServYieldEvent)
	DeclareW32Service(W32ServSetVerdict)
	DeclareW32Service(W32ServInvisibleTransmit)
	DeclareW32Service(W32ServFilterTransmit)
	DeclareW32Service(W32ServExternalDrvRequest)
	DeclareW32Service(W32ServEventRequest)
	DeclareW32Service(W32ServTSPRequest)
};

// Memory managment
typedef struct _COMMON_MEMORY
{
	ULONG m_AppID;									//
	PVOID m_BaseAdress;							// returned value. 
	ULONG m_Size;									// 0 if you want free memory, other value - size to allocate
														// if memory has been already allocated -> error
}COMMON_MEMORY, *PCOMMON_MEMORY;
// -----------------------------------------------------------------------------------------
// перключеие страниц для threads на ходу

#define  TSP_NO_COPY_ORIG				1
#define  TSP_KRNL_MEMSPACE_SHADOW		2
#define  TSP_UNLINK_ON_SWAPOUT			0x80000000 // for internal use only !!!
#define  TSP_INIT_ON_SWAPIN				0x40000000 // for internal use only !!!

/*
typedef struct _PAGE_REGION
{
	PVOID	m_OrigAddr;// MUST be aligned on PAGE bound (divisible by PAGE_SIZE) and locked by FSDrvVirtualLock (see IOCTLHOOK_LOCKMEMORY)
	// All pages in the specified region must be committed. Memory protected with PAGE_NOACCESS cannot be locked 
	// Applications that need to lock larger numbers of pages must first call the SetProcessWorkingSetSize 
	// function to increase their minimum and maximum working set sizes. The maximum number of pages that a process 
	// can lock is equal to the number of pages in its minimum working set minus a small overhead) 
	DWORD	m_RegionSize;		// in pages!
	DWORD	m_Flags;			// see TSP_??? bitfields
}PAGE_REGION, *PPAGE_REGION;
*/

/*
typedef struct _LOCKMEMREGION
{
	PVOID	m_Addr;// both MUST be aligned on PAGE bound (divisible by PAGE_SIZE)
	DWORD	m_RegionSize;		// in pages!
	BOOLEAN Lock;			// FALSE for unlock
}LOCKMEMREGION, *PLOCKMEMREGION;
*/
/*
typedef struct _THREAD_PAGE_SET
{
	ULONG		m_AppID;						//
	ULONG		m_PageCount;					// page count. if count == 0 - then delete assigned to thread list
	PAGE_REGION	m_PageParams[0];				// dword array of pages ()
}THREAD_PAGE_SET,*PTHREAD_PAGE_SET;
*/

typedef void* ORIG_HANDLE; //базовый адрес в третьем кольце

typedef enum _TSP_REQUEST_CLASS
{
	TSPRC_Register,
	TSPRC_Unregister,
	TSPRC_Lock,
	TSPRC_ThreadEnter,
	TSPRC_ThreadLeave,
	TSPRC_SetMinShadow
}TSP_REQUEST_CLASS;

typedef struct _TSPRS_REGISTER
{
	TSP_REQUEST_CLASS ReqClass;
	ULONG	MaxSize;	// In bytes
}TSPRS_REGISTER,*PTSPRS_REGISTER;

typedef struct _TSPRS_COMMON
{
	TSP_REQUEST_CLASS ReqClass;
	ORIG_HANDLE hOrig;
}TSPRS_COMMON,*PTSPRS_COMMON;

typedef struct _TSPRS_LOCK
{
	TSP_REQUEST_CLASS ReqClass;
	ORIG_HANDLE hOrig;
	ULONG real_size;	// In bytes
	ULONG min_number_of_shadows;
}TSPRS_LOCK,*PTSPRS_LOCK;

// -----------------------------------------------------------------------------------------
//
/*
phisical (13h)  0, 1- floppy, 0x80, 0x81... - hards
logical (25h) Zero-based driveletter the operation is to be done on (zero = A, 1 = B,...). 	
*/
typedef struct _DISK_IO
{
	BYTE Func;	//0x25,0x26,0x13,0x14 -- Int13_WRITE_ ;-)
	BYTE Drive;
	BYTE Head;	//Didn't used for 25/26
	BYTE Nifiga;//this field for alignment only
	ULONG Sector; 	
	ULONG Cylinder;
	ULONG SectorCount;	//число секторов
	ULONG SizeOfBuffer;	//размер буфера
	CHAR Buffer[0];// Для операций чтения будет использоваться OutBuffer
} DISK_IO,*PDISK_IO;

// -----------------------------------------------------------------------------------------

typedef struct _DISK_IO_REQUEST
{
	ULONG	m_Tag;												// must be -1
	WCHAR	m_DriveName[DISKNAMEMAXLEN];						//for 9x - first byte is drive letter
	__int64 m_ByteOffset;
	ULONG	m_DataSize;
	ULONG	m_SizeOfBuffer;
	BYTE	m_Buffer[0];
} DISK_IO_REQUEST,*PDISK_IO_REQUEST;

typedef struct _DISK_IO_REQUEST_OLD
{
	WCHAR	m_DriveName[DISKNAMEMAXLEN_OLD];						//for 9x - first byte is drive letter
	__int64 m_ByteOffset;
	ULONG	m_DataSize;
	ULONG	m_SizeOfBuffer;
	BYTE	m_Buffer[0];
} DISK_IO_REQUEST_OLD,*PDISK_IO_REQUEST_OLD;

typedef struct _DISK_ID_INFO
{
	CHAR    m_ModelNumber[64];
	CHAR    m_FirmwareRev[16];
	CHAR    m_SerialNumber[32];
	WCHAR   m_PartitionName[64];
}DISK_ID_INFO, *PDISK_ID_INFO;

// Gruzdev -----------------------------------------------------------------------
typedef enum _REG_OP_STATUS
{
	RegOp_StatusSuccess,								// success, or key/value found in CHECK_XXX_PRESENT
														// request
	RegOp_StatusAlreadyExisting,						// existing key found in a CREATEKEY request
	RegOp_StatusNotFound,								// key/value not found in CHECK_XXX_PRESENT
														// request
	RegOp_StatusNoMoreEntries,							// enumeration index too high
	RegOp_StatusBufferTooSmall							// buffer provided is too small, 
														// REG_OP_RESULT.m_BufferLen contains
														// needed REG_OP_RESULT.m_Buffer[] length
} REG_OP_STATUS;

typedef struct _REG_OP_REQUEST
{
	WCHAR			m_KeyPath[MAX_REGPATH_LEN];			// path to key (\Registry\Machine/User...)
	WCHAR			m_ValueName[MAX_KEYVALUENAME_LEN];	// value name

	union
	{
		ULONG		m_EnumIndex;						// index for enumerations
		struct
		{
			ULONG	m_Type;								// value type
			ULONG	m_ValueSize;						// value data size
			CHAR	m_ValueData[1];						// value data
		};
	};
} REG_OP_REQUEST, *PREG_OP_REQUEST;

typedef struct _REG_OP_RESULT
{
	REG_OP_STATUS	m_Status;							// registry operation status

	ULONG			m_BufferLen;						// needed m_Buffer[] length
	CHAR			m_Buffer[1];						// result buffer
} REG_OP_RESULT, *PREG_OP_RESULT;

typedef struct _REG_ENUM_RESULT
{
	WCHAR			m_Result[1];						// enumeration result
} REG_ENUM_RESULT, *PREG_ENUM_RESULT;

typedef struct _REG_QUERY_RESULT
{
	ULONG			m_Type;								// value type
	ULONG			m_ResultLen;						// size of result data
	CHAR			m_Result[1];						// query value result
} REG_QUERY_RESULT, *PREG_QUERY_RESULT;

typedef struct _REG_QUERY_KEY_RESULT
{
	LARGE_INTEGER	m_LastWriteTime;					// last modification time
	ULONG			m_ClassLen;							// length of key's class string
	ULONG			m_NumSubKeys;						// count of subkeys
	ULONG			m_MaxKeyNameLen;					// max subkey name length
	ULONG			m_MaxClassLen;						// max subkey class string length
	ULONG			m_NumValues;						// count of values
	ULONG			m_MaxValueNameLen;					// max value name length
	ULONG			m_MaxValueDataLen;					// max value data length
	WCHAR			m_Class[1];
} REG_QUERY_KEY_RESULT, *PREG_QUERY_KEY_RESULT;
//--------------------------------------------------------------------------------

// in EXTERNAL_DRV_REQUEST
// for read/write -> m_BufferSize >= sizeof(DISK_IO_REQUEST)
// for read/write -> m_Buffer == PDISK_IO_REQUEST
// for _AVPG_IOCTL_DISK_GET_GEOMETRY or _AVPG_IOCTL_DISK_GET_PARTITION_INFO -> m_Buffer[0] == drive letter (WCHAR - nt, CHAR 9x)
// -----------------------------------------------------------------------------------------

typedef struct _IMPERSONATE_REQUEST
{
	ULONG		m_AppID;
	ULONG		m_Mark;
	DWORD		m_Reserved1;
	DWORD		m_Reserved2;
} IMPERSONATE_REQUEST, *PIMPERSONATE_REQUEST;


typedef struct _EXTERNAL_DRV_REQUEST
{
	ULONG		m_DrvID;
	ULONG		m_IOCTL;
	DWORD		m_AppID;
	DWORD		m_BufferSize;						
	BYTE		m_Buffer[0];						
} EXTERNAL_DRV_REQUEST, *PEXTERNAL_DRV_REQUEST;

//+ ------------------------------------------------------------------------------------------
typedef struct _FIDBOX_REQUEST_GET
{
	ULONG		m_hFile;
} FIDBOX_REQUEST_GET, *PFIDBOX_REQUEST_GET;

typedef struct _FIDBOX_REQUEST_SET
{
	ULONG		m_hFile;
	DWORD		m_BufferSize;						
	BYTE		m_Buffer[0];						
} FIDBOX_REQUEST_SET, *PFIDBOX_REQUEST_SET;

typedef struct _FIDBOX_REQUEST_DATA
{
	DWORD		m_DataFormatId;				// must be 0
	BYTE		m_VolumeID[16];
	BYTE		m_FileID[16];
	BYTE		m_Buffer[28];
} FIDBOX_REQUEST_DATA, *PFIDBOX_REQUEST_DATA;

// Gruzdev ---------------------------------------------------------------------------------
typedef struct _FIDBOX2_REQUEST_GET
{
	ULONG			m_hFile;
} FIDBOX2_REQUEST_GET, *PFIDBOX2_REQUEST_GET;

typedef struct _FIDBOX2_REQUEST_DATA
{
	DWORD			m_DataFormatId;				// must be 0
	BYTE			m_VolumeID[16];
	BYTE			m_FileID[16];
	ULONG			m_Counter;
	LARGE_INTEGER	m_ModificationTime;
} FIDBOX2_REQUEST_DATA, *PFIDBOX2_REQUEST_DATA;
//------------------------------------------------------------------------------------------

// -----------------------------------------------------------------------------------------
typedef enum _EVENT_OBJECT_REQUEST_TYPE
{
	_event_request_read						= 0,	// not implemented yet
	_event_request_write					= 1,	// not implemented yet
	_event_requst_delete					= 2,	// not implemented yet
	_event_update_cache						= 3,
	_event_add_cache						= 4,	// not implemented yet
	_event_delsame_in_queue_by_cacheid		= 5,	// del all unmarked events in queue by CacheID
	_event_request_check					= 6,	// check event object and lock it
	_event_request_get_size					= 7,	// get event object size
	_event_request_release_object			= 8,	// release associated object
	_event_request_set_preverdict			= 9,	// set pre verdict on event

	_event_request_queue_set_max_len		= 10,	// set global params for client -> m_Offset== max_len, m_Verdict - default verdict when overflow
	_event_request_get_thread_id			= 11,	// get thread id for event
	_event_request_get_param				= 12,	// get any param by request
	_event_request_get_param_size			= 13,	// get param size in bytes
	_event_request_get_granularity			= 14,	// get align(?) size
	_event_request_get_basicinfo			= 15,
	_event_request_get_fidbox				= 16,
	_event_request_set_fidbox				= 17,
}EVENT_OBJECT_REQUEST_TYPE;

#define EVENT_OBJECT_REQUEST_FLAG_NONE					0x0000
#define EVENT_OBJECT_REQUEST_FLAG_OFFSET				0x0001		// use set offset before operation
#define EVENT_OBJECT_REQUEST_FLAG_USECOMMON_MEM		0x0002		// use m_CommonMemoryPtr instead of m_Buffer	// not implemented yet
#define EVENT_OBJECT_REQUEST_FLAG_TIMEOUT				0x0004		// m_Timeout is valid

typedef struct _EVENT_OBJECT_REQUEST
{
	ULONG								m_AppID;
	ULONG								m_Mark;
	EVENT_OBJECT_REQUEST_TYPE			m_RequestType;
	DWORD								m_Flags;				//EVENT_OBJECT_REQUEST_FLAG_???
	DWORD								m_Offset;
	VERDICT								m_Verdict;
	ULONG								m_Timeout;
	DWORD								m_OffsetHi;
	DWORD								m_RequestBufferSize;
	ULONG								m_Reserved;	// offset from BaseAddr // not used yet
	CHAR								m_Buffer[0];
}EVENT_OBJECT_REQUEST, *PEVENT_OBJECT_REQUEST;

typedef struct _DIRECT_FILE_REQUEST
{
	HANDLE								m_hFile;
	LARGE_INTEGER						m_Offset;
	DWORD								m_RequestBufferSize;
	CHAR								m_Buffer[0];
}DIRECT_FILE_REQUEST, *PDIRECT_FILE_REQUEST;

// -----------------------------------------------------------------------------------------

//???_IOCTL_START_FILTER
typedef BOOLEAN(__stdcall *IS_NEED_FILTER_EVENT)(ULONG /*HookID*/, ULONG/*FuncMj*/, ULONG/*FuncMi*/);
typedef BOOLEAN(__stdcall *IS_NEED_FILTER_EVENT_EX)(ULONG /*HookID*/, ULONG/*FuncMj*/, ULONG/*FuncMi*/, HANDLE /*TreadID*/);
typedef VERDICT(__stdcall *FILTER_EVENT_FUNC)(PFILTER_EVENT_PARAM, PVOID/*PEVENT_OBJECT_INFO*/);
typedef ULONG(__stdcall *EVENT_REQUEST_FUNC)(PEVENT_OBJECT_REQUEST, PVOID/*PEVENT_OBJECT_INFO*/, PVOID /*pOutRequest*/, ULONG /*OutputBufferLength*/, PULONG /*pRetSuze*/);


typedef enum _EVENT_OBJECT_TYPE
{
	_EVENT_UNKNOWN_OBJECT	= 0,
		_EVENT_FILE_OBJECT			= 1,
		_EVENT_VOLUME_OBJECT		= 2,
}EVENT_OBJECT_TYPE;

#define _EVENT_OBJECT_INFO_FLAG_NONE			0x000
#define _EVENT_OBJECT_INFO_FLAG_DYNALLOCATED	0x001			// object allocated dynamically

typedef struct _EVENT_OBJECT_INFO
{
	EVENT_OBJECT_TYPE	m_ObjectType;
	HANDLE				m_Object;
	HANDLE				m_DevObj;
	DWORD				m_Flags;						// see _EVENT_OBJECT_INFO_FLAG_???
	EVENT_REQUEST_FUNC	m_pCallback;
}EVENT_OBJECT_INFO, *PEVENT_OBJECT_INFO;


typedef struct _EXT_START_FILTERING
{
	ULONG					m_FltVersion;				// filtering protocol version
	ULONG					m_HookID;					//
	IS_NEED_FILTER_EVENT	m_pIsNeedFilterEvent;		// Ring0 address IsNeedFilterEvent
	FILTER_EVENT_FUNC		m_pFilterEvent;				// Ring0 address FilterEvent
	IS_NEED_FILTER_EVENT_EX m_pIsNeedFilterEventEx;
}EXT_START_FILTERING, *PEXT_START_FILTERING;


#define PROPROT_FLAG_NONE			0x0000
#define PROPROT_FLAG_OPEN			0x0001
#define PROPROT_FLAG_TERMINATE		0x0002
#define PROPROT_FLAG_DEBUG			0x0004
#define PROPROT_FLAG_NOTIFY			0x0100
#define PROPROT_FLAG_AUTOACTIVATE	0x2000

typedef struct _SINGLE_REGION
{
	__int64					m_RegionOffset;
	ULONG					m_RegionSize;
}SINGLE_REGION, *PSINGLE_REGION;

typedef struct _PREFETCH_REQUEST
{
	DWORD			m_Object;
	DWORD			m_Reserved1;
	DWORD			m_Reserved2;
	ULONG			m_RegionCount;
	SINGLE_REGION	m_Regions[0];
}PREFETCH_REQUEST, *PPREFETCH_REQUEST;

//+ ------------------------------------------------------------------------------------------
// Special file operations
typedef enum _eSpecFile_RequestType
{
	_sfr_add,
	_sfr_get,
	_sfr_flush,
	_sfr_proceed_container,
}SpecFile_RequestType;

typedef enum _eSpecFile_RequestFunc
{
	_sfop_delete = 0,
	_sfop_rename,
}SpecFile_RequestFunc;

typedef enum _eSpecFile_FuncResult
{
	_sfopr_new_request = 0,				// действия над файлом ещё не выполнялись
	_sfopr_ok,
	_sfopr_notok,
	_sfopr_file_not_found,
	_sfopr_access_denied,
	_sfopr_not_enough_space,
}SpecFile_FuncResult;

typedef struct _SPECFILEFUNC_FILEBLOCK
{
	SpecFile_RequestFunc	m_FileOp;			// ignored if m_Request == _sfr_get
	SpecFile_FuncResult		m_Result;
	ULONG					m_ContainerID;		// reserved -> must be 0
	BYTE					m_FileNames[0];
}SPECFILEFUNC_FILEBLOCK, *PSPECFILEFUNC_FILEBLOCK;

typedef struct _SPECFILEFUNC_REQUEST
{
	SpecFile_RequestType	m_Request;
	ULONG					m_Idx;					// block idx
	SPECFILEFUNC_FILEBLOCK	m_Data;
}SPECFILEFUNC_REQUEST, *PSPECFILEFUNC_REQUEST;

typedef struct _OBJ_BASIC_INFO {
    LARGE_INTEGER CreationTime;
    LARGE_INTEGER LastAccessTime;
    LARGE_INTEGER LastWriteTime;
    LARGE_INTEGER ChangeTime;
    ULONG ObjAttributes;
} OBJ_BASIC_INFO, *POBJ_BASIC_INFO;

//+ ------------------------------------------------------------------------------------------


#pragma warning (default : 4200)

//--IOCTLs----------------------------------------------------------------------------------------------
//#undef CTL_CODE
//#define CTL_CODE( DeviceType, Function, Method, Access ) (((ULONG)(DeviceType) << 16) + ((ULONG)(Access) << 14) + ((ULONG)(Function) << 2) + ((ULONG)Method))

#define _KLG_HOOK	0x00009000
#define _KLG_FUNC	0

#define IOCTLHOOK_GetVersion		CTL_CODE(_KLG_HOOK, _KLG_FUNC+0, METHOD_BUFFERED, FILE_ANY_ACCESS)
//#define IOCTLHOOK_GetVersion		(ULONG) (((ULONG)(_KLG_HOOK) << 16) + ((ULONG)(FILE_ANY_ACCESS) << 14) + ((ULONG)(0) << 2) + ((ULONG)METHOD_BUFFERED))
// OutBuffer=&BuildNum OutBufferSize=sizeof(ULONG)

#define IOCTLHOOK_RegistrApp		CTL_CODE(_KLG_HOOK, _KLG_FUNC+1, METHOD_BUFFERED, FILE_ANY_ACCESS)
//#define IOCTLHOOK_RegistrApp		(ULONG) (((ULONG)(_KLG_HOOK) << 16) + ((ULONG)(FILE_ANY_ACCESS) << 14) + ((ULONG)(1) << 2) + ((ULONG)METHOD_BUFFERED))
// InBuffer=&APP_REGISTRATION     InBufferSize=sizeof(APP_REGISTRATION)
// OutBuffer=&CLIENT_INFO    OutBufferSize=sizeof(CLIENT_INFO)

#define IOCTLHOOK_Activity			CTL_CODE(_KLG_HOOK, _KLG_FUNC+2, METHOD_BUFFERED, FILE_ANY_ACCESS)
//#define IOCTLHOOK_Activity			(ULONG) (((ULONG)(_KLG_HOOK) << 16) + ((ULONG)(FILE_ANY_ACCESS) << 14) + ((ULONG)(2) << 2) + ((ULONG)METHOD_BUFFERED))
// App MUST pereodically check Activity (at least every 10 seconds),
// otherwise after ACTIVITY_WATCHDOG_TIMEOUT it will be marked as Dead
// InBuffer=PHDSTATE InBufferSize=sizeof(HDSTATE)
// OutBufer=PHDSTATE OutBufferSize=sizeof(HDSTATE)

#define IOCTLHOOK_GetEvent			CTL_CODE(_KLG_HOOK, _KLG_FUNC+3, METHOD_BUFFERED, FILE_ANY_ACCESS)
//#define IOCTLHOOK_GetEvent			(ULONG) (((ULONG)(_KLG_HOOK) << 16) + ((ULONG)(FILE_ANY_ACCESS) << 14) + ((ULONG)(3) << 2) + ((ULONG)METHOD_BUFFERED))
// InBuffer=&GET_EVENT     InBufferSize=sizeof(GET_EVENT)
// OutBuffer=PEVENT_TRANSMIT      OutBufferSize>=Sizeof(EVENT_TRANSMIT)

#define IOCTLHOOK_GetEventSize	CTL_CODE(_KLG_HOOK, _KLG_FUNC+9, METHOD_BUFFERED, FILE_ANY_ACCESS)
//#define IOCTLHOOK_GetEventSize		(ULONG) (((ULONG)(_KLG_HOOK) << 16) + ((ULONG)(FILE_ANY_ACCESS) << 14) + ((ULONG)(9) << 2) + ((ULONG)METHOD_BUFFERED))
// InBuffer=&ULONG			InBufferSize=sizeof(ULONG) - AppID
// OutBuffer=DWORD*			OutBufferSize = sizeof(DWORD)

#define IOCTLHOOK_YeldEvent			CTL_CODE(_KLG_HOOK, _KLG_FUNC+8, METHOD_BUFFERED, FILE_ANY_ACCESS)
//#define IOCTLHOOK_YeldEvent			(ULONG) (((ULONG)(_KLG_HOOK) << 16) + ((ULONG)(FILE_ANY_ACCESS) << 14) + ((ULONG)(8) << 2) + ((ULONG)METHOD_BUFFERED))
// InBuffer=&YELD_EVENT  InBufferSize>=sizeof(YELD_EVENT)
// OutBuffer=&BUFFER_FOR_DATA OutBufferSize == BUFFER_FOR_DATA

#define IOCTLHOOK_SetVerdict		CTL_CODE(_KLG_HOOK, _KLG_FUNC+4, METHOD_BUFFERED, FILE_ANY_ACCESS)
//#define IOCTLHOOK_SetVerdict		(ULONG) (((ULONG)(_KLG_HOOK) << 16) + ((ULONG)(FILE_ANY_ACCESS) << 14) + ((ULONG)(4) << 2) + ((ULONG)METHOD_BUFFERED))
// InBuffer=PSET_VERDICT InBufferSize=sizeof(SET_VERDICT)
// or InBuffer=PSET_VERDICT_EX InBufferSize=>sizeof(SET_VERDICT_EX)

#define IOCTLHOOK_FiltersCtl		CTL_CODE(_KLG_HOOK, _KLG_FUNC+5, METHOD_BUFFERED, FILE_ANY_ACCESS)
//#define IOCTLHOOK_FiltersCtl		(ULONG) (((ULONG)(_KLG_HOOK) << 16) + ((ULONG)(FILE_ANY_ACCESS) << 14) + ((ULONG)(5) << 2) + ((ULONG)METHOD_BUFFERED))
// InBuffer>=PFILTER_TRANSMIT  InBufferSize>=sizeof(FILTER_TRANSMIT)
// OutBuffer>=PFILTER_TRANSMIT  OutBufferSize>=sizeof(FILTER_TRANSMIT)

#define IOCTLHOOK_InvisibleThreadOperations	CTL_CODE(_KLG_HOOK, _KLG_FUNC+6, METHOD_BUFFERED, FILE_ANY_ACCESS)
//#define IOCTLHOOK_InvisibleThreadOperations	(ULONG) (((ULONG)(_KLG_HOOK) << 16) + ((ULONG)(FILE_ANY_ACCESS) << 14) + ((ULONG)(6) << 2) + ((ULONG)METHOD_BUFFERED))
// InBuffer=PINVISIBLE_TRANSMIT  InBufferSize>=sizeof(INVISIBLE_TRANSMIT)
// OutBuffer=PINVISIBLE_TRANSMIT  InBufferSize>=sizeof(INVISIBLE_TRANSMIT)

#define IOCTLHOOK_ReadInt13		CTL_CODE(_KLG_HOOK, _KLG_FUNC+7, METHOD_BUFFERED, FILE_ANY_ACCESS)
// Obsolete и сейчас совершенно не модно... и даже более того, реализация убрана. Пользуйтесь IOCTLHOOK_DiskIO
//#define IOCTLHOOK_ReadInt13		(ULONG) (((ULONG)(_KLG_HOOK) << 16) + ((ULONG)(FILE_ANY_ACCESS) << 14) + ((ULONG)(7) << 2) + ((ULONG)METHOD_BUFFERED))
// InBuffer = PREAD_INT13 InBufferSize=sizeof(READ_INT13)
// OutBufferSize depends PREAD_INT13->Len*512

#define IOCTLHOOK_FilterEvent CTL_CODE(_KLG_HOOK, _KLG_FUNC+10, METHOD_BUFFERED, FILE_ANY_ACCESS)
//#define IOCTLHOOK_FilterEvent		(ULONG) (((ULONG)(_KLG_HOOK) << 16) + ((ULONG)(FILE_ANY_ACCESS) << 14) + ((ULONG)(10) << 2) + ((ULONG)METHOD_BUFFERED))
// InBuffer>=PFILTER_EVENT_PARAM  InBufferSize>=sizeof(FILTER_EVENT_PARAM)
// OutBuffer=PVERDICT  OutBufferSize=sizeof(VERDICT)

#define IOCTLHOOK_ResetCache	CTL_CODE(_KLG_HOOK, _KLG_FUNC+11, METHOD_BUFFERED, FILE_ANY_ACCESS)
//#define IOCTLHOOK_ResetCache		(ULONG) (((ULONG)(_KLG_HOOK) << 16) + ((ULONG)(FILE_ANY_ACCESS) << 14) + ((ULONG)(11) << 2) + ((ULONG)METHOD_BUFFERED))
// InBuffer>=ULONG  InBufferSize=sizeof(ULONG)
// OutBuffer=NULL  OutBufferSize=0

#define IOCTLHOOK_NewDebugInfoLevel	CTL_CODE(_KLG_HOOK, _KLG_FUNC+12, METHOD_BUFFERED, FILE_ANY_ACCESS)
//#define IOCTLHOOK_NewDebugInfoLevel		(ULONG) (((ULONG)(_KLG_HOOK) << 16) + ((ULONG)(FILE_ANY_ACCESS) << 14) + ((ULONG)(12) << 2) + ((ULONG)METHOD_BUFFERED))
// InBuffer=ULONG  InBufferSize=sizeof(ULONG) for setting new level
// OutBuffer=NULL  OutBufferSize=0
// OR (possible to set both buffers)
// InBuffer=NULL  InBufferSize=0 
// OutBuffer=ULONG  OutBufferSize=sizeof(ULONG) for getting current level

#define IOCTLHOOK_NewDebugCategoryMask	CTL_CODE(_KLG_HOOK, _KLG_FUNC+20, METHOD_BUFFERED, FILE_ANY_ACCESS)
//#define IOCTLHOOK_NewDebugInfoLevel		(ULONG) (((ULONG)(_KLG_HOOK) << 16) + ((ULONG)(FILE_ANY_ACCESS) << 14) + ((ULONG)(20) << 2) + ((ULONG)METHOD_BUFFERED))
// In/OutBuffer the same as IOCTLHOOK_NewDebugInfoLevel

#define IOCTLHOOK_SetFlowTimeout	CTL_CODE(_KLG_HOOK, _KLG_FUNC+13, METHOD_BUFFERED, FILE_ANY_ACCESS)
//#define IOCTLHOOK_SetFlowTimeout		(ULONG) (((ULONG)(_KLG_HOOK) << 16) + ((ULONG)(FILE_ANY_ACCESS) << 14) + ((ULONG)(13) << 2) + ((ULONG)METHOD_BUFFERED))
// InBuffer=PFLOWTIMEOUT InBufferSize=sizeof(FLOWTIMEOUT)
// OutBuffer=NULL  OutBufferSize=0

#define IOCTLHOOK_SkipEvent	CTL_CODE(_KLG_HOOK, _KLG_FUNC+17, METHOD_BUFFERED, FILE_ANY_ACCESS)
//#define IOCTLHOOK_SkipEvent		(ULONG) (((ULONG)(_KLG_HOOK) << 16) + ((ULONG)(FILE_ANY_ACCESS) << 14) + ((ULONG)(17) << 2) + ((ULONG)METHOD_BUFFERED))
// InBuffer=DWORD(APPID) InBufferSize=sizeof(DWORD)
// OutBuffer=NULL  OutBufferSize=0


#define IOCTLHOOK_ImpersonateThread	CTL_CODE(_KLG_HOOK, _KLG_FUNC+22, METHOD_BUFFERED, FILE_ANY_ACCESS)
// InBuffer=sizeof(_IMPERSONATE_REQUEST)
// OutBuffer=NULL  OutBufferSize=0

#define IOCTLHOOK_External_Drv_Request	CTL_CODE(_KLG_HOOK, _KLG_FUNC+23, METHOD_BUFFERED, FILE_ANY_ACCESS)
// InBuffer>=sizeof(EXTERNAL_DRV_REQUEST)

#define IOCTLHOOK_EventObject_Request	CTL_CODE(_KLG_HOOK, _KLG_FUNC+24, METHOD_BUFFERED, FILE_ANY_ACCESS)
// InBuffer>=sizeof(EVENT_OBJECT_REQUEST)



// -----------------------------------------------------------------------------------------
// Extended driver functions
// -----------------------------------------------------------------------------------------

//!! STATUS_NOT_IMPLEMENTED
#define IOCTLHOOK_ThreadSubProcesssing	CTL_CODE(_KLG_HOOK, _KLG_FUNC+14, METHOD_BUFFERED, FILE_ANY_ACCESS)
//#define IOCTLHOOK_ThreadSubProcesssing		(ULONG) (((ULONG)(_KLG_HOOK) << 16) + ((ULONG)(FILE_ANY_ACCESS) << 14) + ((ULONG)(14) << 2) + ((ULONG)METHOD_BUFFERED))
// InBuffer=PTHREAD_PAGE_SET InBufferSize>=sizeof(THREAD_PAGE_SET)
// REMARK -> if THREAD_PAGE_SET.m_PageCount == 0  reset page's list and stop thread subprocessing
// OutBuffer=NULL  OutBufferSize=0

#define IOCTLHOOK_TSPv2	CTL_CODE(_KLG_HOOK, _KLG_FUNC+25, METHOD_BUFFERED, FILE_ANY_ACCESS)
// InBuffer=PTSPRS_??? InBufferSize>=sizeof(TSPRS_COMMON) depends of TSP_REQUEST_CLASS
// OutBuffer=ORIG_HANDLE*  OutBufferSize=sizeof(ORIG_HANDLE) for TSP_REQUEST_CLASS -- TSPRC_Register
// OutBuffer=NULL  OutBufferSize=0 for other classes

#define IOCTLHOOK_GetDriverInfo		CTL_CODE(_KLG_HOOK, _KLG_FUNC+16, METHOD_BUFFERED, FILE_ANY_ACCESS)
//#define IOCTLHOOK_GetDriverInfo		(ULONG) (((ULONG)(_KLG_HOOK) << 16) + ((ULONG)(FILE_ANY_ACCESS) << 14) + ((ULONG)(16) << 2) + ((ULONG)METHOD_BUFFERED))
// InBuffer=NULL InBufferSize=0
// OutBuffer=PADDITIONALSTATE  OutBufferSize=sizeof(ADDITIONALSTATE)

#define IOCTLHOOK_DiskIO	CTL_CODE(_KLG_HOOK, _KLG_FUNC+18, METHOD_BUFFERED, FILE_ANY_ACCESS)
//#define IOCTLHOOK_DiskIO		(ULONG) (((ULONG)(_KLG_HOOK) << 16) + ((ULONG)(FILE_ANY_ACCESS) << 14) + ((ULONG)(18) << 2) + ((ULONG)METHOD_BUFFERED))
// InBuffer=PDISK_IO InBufferSize==sizeof(DISK_IO)
// OutBufferSize depends PDISK_IO->SizeOfBuffer для операций чтения и 0 для записи

#define IOCTLHOOK_COMMONMEMORY	CTL_CODE(_KLG_HOOK, _KLG_FUNC+19, METHOD_BUFFERED, FILE_ANY_ACCESS)
//#define IOCTLHOOK_COMMONMEMORY		(ULONG) (((ULONG)(_KLG_HOOK) << 16) + ((ULONG)(FILE_ANY_ACCESS) << 14) + ((ULONG)(19) << 2) + ((ULONG)METHOD_BUFFERED))
// InBuffer=PCOMMON_MEMORY InBufferSize==sizeof(COMMON_MEMORY)
// OutBuffer=PCOMMON_MEMORY OutBufferSize==sizeof(COMMON_MEMORY)

//!! STATUS_NOT_IMPLEMENTED
#define IOCTLHOOK_LOCKMEMORY	CTL_CODE(_KLG_HOOK, _KLG_FUNC+21, METHOD_BUFFERED, FILE_ANY_ACCESS)
// InBuffer=PLOCKMEMREGION InBufferSize==sizeof(LOCKMEMREGION)
// OutBuffer=NULL OutBufferSize=0

#define IOCTLHOOK_RELOADSETTINGS	CTL_CODE(_KLG_HOOK, _KLG_FUNC+26, METHOD_BUFFERED, FILE_ANY_ACCESS)
// InBuffer=PULONG InBufferSize==sizeof(ULONG)
// OutBuffer=NULL OutBufferSize=0

#define IOCTLHOOK_SPECFILEREQUEST	CTL_CODE(_KLG_HOOK, _KLG_FUNC+27, METHOD_BUFFERED, FILE_ANY_ACCESS)
// InBuffer=PSPECFILEFUNC_REQUEST InBufferSize>=sizeof(SPECFILEFUNC_REQUEST)
// OutBuffer=NULL OutBufferSize=0 or OutBuffer= buffer for SPECFILEFUNC_FILEBLOCK

#define IOCTLHOOK_RESTART_FWDRV		CTL_CODE(_KLG_HOOK, _KLG_FUNC+28, METHOD_BUFFERED, FILE_ANY_ACCESS)
#define IOCTLHOOK_GETHASHFUNCVER	CTL_CODE(_KLG_HOOK, _KLG_FUNC+29, METHOD_BUFFERED, FILE_ANY_ACCESS)

// ^^ 29 zanito (next free 30) -----------------------------------------------------------------------------------------------------------------
#include <poppack.h>

#endif
