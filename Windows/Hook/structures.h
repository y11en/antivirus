#ifndef __STRUCTURES_H
#define __STRUCTURES_H

/*!
*		
*		
*		(C) 2001-3 Kaspersky Lab 
*		
*		\file	structures.h
*		\brief	внутренние структуры данных
*		
*		\author Andrew Sobko
*		\date	12.09.2003 14:11:49
*		
*		
*		
*		
*/		


#include "hook/avpgcom.h"
#include "osspec.h"

#define MY_PAGE_SIZE PAGE_SIZE-0x10
#define __SID_LENGTH 32

//+ коды возврата из функции ReplyWaiter
typedef enum _eReplyWaiter
{
	ReplyWaiter_SendFailed	= 0,
	ReplyWaiter_WaitFailed	= 1,
	ReplyWaiter_Succeed		= 2,
}eReplyWaiter, *peReplyWaiter;

//+ общий объект синхронизации с Ring3
typedef struct _KLG_EVENT
{
	PKEVENT			pWhistle; // Attention, this filed may be equ 0 (if nafig ne nugni :) )
	HANDLE			hWhistle;
}KLG_EVENT, *PKLG_EVENT;

//+ описание для маппирования данных из фильтра в событие
typedef struct _MAPPED_PARAMS
{
	DWORD			m_ParamsCount;
	DWORD			m_Param_Size;
	BYTE			m_SingleParams[0];					//FILTER_PARAM
}MAPPED_PARAMS, *PMAPPED_PARAMS;

//+ запись в лог
typedef struct _LOGENTRY
{
	struct _LOGENTRY *Next;
	PCHAR BuffPos;
	CHAR MsgBuff[MY_PAGE_SIZE - sizeof(PVOID) - sizeof(PCHAR) - 1];
} LOGENTRY, *PLOGENTRY;

//+ лог
typedef struct _LOGSTRUCT
{
	CHAR m_LogFileName[MAXPATHLEN];// interface must to place value in registry
	HANDLE m_hLog;
	PLOGENTRY pLogEntry;
} LOGSTRUCT, *PLOGSTRUCT;

// структура записи описаний клиентов в реестр
typedef struct _CLIENT_SAV
{
	CHAR		LogFileName[MAXPATHLEN];
	ULONG		Priority;
	DWORD		ClientFlags;
	ULONG		CacheSize;
	DWORD		BlueScreenTimeout;
}CLIENT_SAV, *PCLIENT_SAV;

//+ ------------------------------------------------------------------------------------------
//+ флажки описания текущего статуса обработки security
#define		_SecurityFlagsNone					0x000
#define		_SecurityFlagsCaptured				0x001
#define		_SecurityFlagsLocked					0x002
#define		_SecurityFlagsClientOk				0x004
#define		_SecurityFlags_Impersonated		0x010

//+ ------------------------------------------------------------------------------------------
//+ структура события
typedef struct _QELEM {
	struct _QELEM *m_pNext;
	struct _QELEM *m_pPrev;
	ULONG		m_Mark;
	ULONG		m_MarkerProcID;				// id маркирующего процесса
	ULONG		m_FilterID;
	KEVENT		m_KEvent;
	VERDICT		m_Verdict;
	DWORD		m_dwCacheValue;
	ULONG		m_Timeout;
	CHAR		m_ProcName[PROCNAMELEN];
	ULONG		m_ProcessID;
	ULONG		m_ThreadID;
	ULONG		m_HookID;					// 
	ULONG		m_FunctionMj;				// 
	ULONG		m_FunctionMi;				// 
	DWORD		m_EventFlags;	//
	ULONG		m_Flags;					// 
	
	BOOLEAN		m_bVerdict;
	ULONG		m_YeldTimeout;

#ifdef _HOOK_NT_	
	// no hold executions
#else
	KEVENT		m_HoldKEvent;
	PCHAR		m_pInBuffer;
	ULONG		m_InBufferSize;
	ULONG		m_OutBufferSize;
	PCHAR		m_pOutBuffer;
#endif

#ifdef _HOOK_IMPERSONATE_	
	PETHREAD	m_StoppedThread;
	DWORD		m_SecurityFlag;
	SECURITY_CLIENT_CONTEXT m_ClientContext;
#endif

	PEVENT_OBJECT_INFO	m_pEventObject;

//! Фильтр может быт убит к окнчанию обработки события
	ULONG		m_ParamsCount;		// parameters value
	BYTE		m_Params[0];				// SINGLE_PARAM 
} QELEM, *PQELEM;

//+ ------------------------------------------------------------------------------------------
//+ очередь событий
typedef struct _EVENT_LIST
{
	PERESOURCE		m_pEventMutex;			// to protect event's queue
	PQELEM			m_pEvQueue;
	PQELEM			m_pEvQueue_FirstUnmarked;
	ULONG			m_QLen;
	ULONG			m_QUnmarkedLen;
	ULONG			m_MaxQueueLen;
}EVENT_LIST, *PEVENT_LIST;

// -----------------------------------------------------------------------------------------
// cache 
typedef struct _DRIVER_CACHE
{
	__int64 m_ExpirationTime;
	DWORD	m_dwVal;
	__int8 m_bDirtyFlag;
	__int8 m_Verdict;		//was VERDICT 
}DRIVER_CACHE, *PDRIVER_CACHE;

typedef struct _CACHE_OBJ {
	DRIVER_CACHE		m_CacheObj;				// object identifier (must be unique)
	struct _CACHE_OBJ*	m_pNext;				// pointer to next object
	struct _CACHE_OBJ*	m_pPrev;				// pointer to next object
} CACHE_OBJ, *PCACHE_OBJ;

typedef struct _CACHE_CL {
	PCACHE_OBJ		m_pCache;				// pointer to cache block
	PCACHE_OBJ		m_pCacheHead;			// ... root
} CACHE_CL, *PCACHE_CL;

#define _INVALID_VALUE (DWORD) -1

//_CL_CACHE_BITMASKSIZE - на сколько очередей будет поделен кэш
#define _CL_CACHE_BITMASKSIZE	7
#define _CL_CACHE_QUEUES		(1 << _CL_CACHE_BITMASKSIZE)
#define _CL_CACHE_ACCESSMASK	_CL_CACHE_QUEUES - 1
#define _CL_CAHCHE_MIN_SIZE		64 * _CL_CACHE_QUEUES


// common memory
typedef struct _MEMORY_MAPPING
{
	PVOID			m_MemBaseAdress;		// store memory adress for common block with r3
	PMDL			m_MempMdl;				// 
	ULONG			m_MemSize;				// memory size
	PVOID			m_MemUserAddr;
	PVOID			m_MemStartFreeAddr;		// 
	PVOID			m_StartBisyAddr;	
}MEMORY_MAPPING, *PMEMORY_MAPPING;

//+ ------------------------------------------------------------------------------------------
//+ подключение подклиентов для распределения нагрузки
typedef struct _QSUBCLIENT
{
	struct _QSUBCLIENT	*m_pNext;
	
	ULONG			m_ActivityState;		// see _INTERNAL_APP_STATE_???
	LONG			m_ActivityWatchDog;		// then < 0 client is dead
	DWORD			m_BlueScreenTimeout;	//
	PKLG_EVENT		m_pWhistleUp;			// for communicate
	PKLG_EVENT		m_pWhistlerChanged;		//

	ULONG			m_ProcessId;			// 
	ULONG			m_ThreadId;
	
	ULONG			m_pkThread;
	ULONG			m_SuspendCount;

	ULONG			m_Tasks;
	ULONG			m_Enters;				// locks number
}QSUBCLIENT, *PQSUBCLIENT;


//+ ------------------------------------------------------------------------------------------
//+ клиентская структура
typedef struct _QCLIENT 
{
	struct _QCLIENT	*m_pNext;
// options
	ULONG			m_AppID;
	ULONG			m_Priority;
	DWORD			m_ClientFlags;			// see _CLIENT_FLAG_???

// events
	EVENT_LIST		m_EventList;
	DWORD			m_FiltersCount;
// cache for clean objects -----------------------------------------------------------------
	ULONG			m_CacheSize;			// cache size
	PERESOURCE		m_pCacheMutex;			// to protect cache
	CACHE_CL		m_CacheCl[_CL_CACHE_QUEUES];
	
	VERDICT			m_DefaultVerdict;
// locks
	ULONG			m_Enters;				// locks number
	ULONG			m_ClientsSafeValue;

// sub clients -----------------------------------------------------------------------------
	
	PQSUBCLIENT		m_pSubClient;
}QCLIENT, *PQCLIENT;

//+ ------------------------------------------------------------------------------------------
//+ описание фильтра, сработавшего на событие (внутреннее использование)
typedef struct _FILTER_SUB
{
	ULONG			m_FilterID;
	ULONG			m_Timeout;
	DWORD			m_Flags;
	VERDICT			m_Verdict;
	DWORD			m_dwCacheValue;
	PQSUBCLIENT		m_pSubClient;
	PMAPPED_PARAMS	m_pMappedFilterParams;
}FILTER_SUB, *PFILTER_SUB;

//+ ------------------------------------------------------------------------------------------
//+ ассоциированный контекст к объекту
typedef struct _KLG_OBJECT_CONTEXT
{
	ULONG	m_Flags;						// см. _CONTEXT_OBJECT_FLAG_??? (hookspec.h)
#ifdef _HOOK_NT_
	LUID	m_Luid;
#endif
	BYTE	m_SID[__SID_LENGTH];
}KLG_OBJECT_CONTEXT, *PKLG_OBJECT_CONTEXT;

typedef struct _FIDBOX_REQUEST_DATA_EX
{
	DWORD		m_DataFormatId;				// must be 0
	BYTE		m_VolumeID[16];
	BYTE		m_FileID[16];
	BYTE		m_Buffer[0];
} FIDBOX_REQUEST_DATA_EX, *PFIDBOX_REQUEST_DATA_EX;

#define _INETSWIFT_VERSION_1	1
#define _INETSWIFT_VERSION_2	2
#define _INETSWIFT_VERSION_3	3


#define _INETSWIFT_TAG			0x7

typedef struct _INETSWIFT_DATA
{
	BYTE				m_Version;
	BYTE				m_Tag;		// _INETSWIFT_TAG
	BYTE				m_Reserved2;
	BYTE				m_Reserved3;
	FIDBOX_REQUEST_DATA m_Data;
}INETSWIFT_DATA, *PINETSWIFT_DATA;

typedef struct _INETSWIFT_DATA_EX
{
	BYTE					m_Version;
	BYTE					m_Tag;		// _INETSWIFT_TAG
	BYTE					m_Reserved2;
	BYTE					m_Reserved3;
	FIDBOX_REQUEST_DATA_EX	m_Data;
}INETSWIFT_DATA_EX, *PINETSWIFT_DATA_EX;


#endif //__STRUCTURES_H
