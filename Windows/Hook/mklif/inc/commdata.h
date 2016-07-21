#ifndef __commdata_h
#define __commdata_h

#pragma warning (disable : 4200)

#define KLIF_COMM_PORT					L"\\KlifComm"
#define DEFAULT_DRV_NAME				L"\\KLIF"

#define MKLIF_API_VERSION				1
#define MKLIF_SMALL_EVENT_SIZE			0x900
//#define MKLIF_SMALL_EVENT_SIZE			0x10

typedef enum _MKLIF_COMMAND {
	mkc_Undefined				= 0,
	mkc_GetVersion				= 1,
	mkc_RegisterClient			= 2,
	mkc_AddFilter				= 3,
	mkc_ClientStateRequest		= 4,
	mkc_GetEventPart			= 5,
	mkc_ObjectRequest			= 6,
	mkc_AddInvThread			= 7,
	mkc_DelInvThread			= 8,
	mkc_IsInvisibleThread		= 9,
	mkc_QueryDrvFlags			= 10,
	mkc_DelFilter				= 11,
	mkc_DelAllFilters			= 12,
	mkc_QueryProcessesInfo		= 13,
	mkc_QueryModulesInfo		= 14,
	mkc_ReadProcessMemory		= 15,
	mkc_WriteProcessMemory		= 16,
	mkc_SaveFilters				= 17,
	mkc_AddInvProcess			= 18,
	mkc_AddInvProcessRecursive	= 19,
	mkc_ReleaseDrvFiles			= 20,
	mkc_AddInvThreadByHandle	= 21,
	mkc_DelInvThreadByHandle	= 22,
	mkc_IsImageActive			= 23,
	mkc_QueryActiveImages		= 24,
	mkc_ClientResetCache		= 25,
	mkc_AllowUnload				= 26,
	mkc_DetachDrvFiles			= 27,
	mkc_QueryProcessHash		= 28,
	mkc_BreakConnection			= 29,
	mkc_ClientSync				= 30,
	mkc_QueryFiltersCount		= 31,
	mkc_QueryClientCounter		= 32,
	mkc_FidBoxGet_ByHandle		= 33,
	mkc_FidBoxSet_ByHandle		= 34,
	mkc_LLD_QueryNames			= 35,
	mkc_LLD_GetInfo				= 36,
	mkc_LLD_GetGeometry			= 37,
	mkc_LLD_Read				= 38,
	mkc_LLD_GetDiskId			= 39,
	mkc_LLD_Reserved2			= 40,
	mkc_QueryFileHash			= 41,
	mkc_LLD_Write				= 42,
	mkc_AddApplRule				= 43,
	mkc_ResetClientRules		= 44,
	mkc_ApplyRules				= 45,
	mkc_QueryFileNativePath		= 46,
	mkc_SpecialFileRequest		= 47,
	mkc_FilterGetSize			= 48,
	mkc_FilterGet				= 49,
	mkc_FilterGetNextId			= 50,
	mkc_GetHashVersion			= 51,
	mkc_PreCreateDisable		= 52,
	mkc_PreCreateEnable			= 53,
	mkc_GetHashSize				= 54,
	mkc_QueryFileHandleHash		= 55,
	mkc_FilterEvent				= 56,
	mkc_ChangeFilterParam		= 57,
	mkc_FidBox2Get_ByHandle		= 58,
	mkc_KLFltDev_SetRule		= 59,
	mkc_KLFltDev_GetRulesSize	= 60,
	mkc_KLFltDev_GetRules		= 61,
	mkc_KLFltDev_ApplyRules		= 62,
	mkc_AddInvProcessByPid		= 63,
	mkc_Virt_AddApplToSB		= 64,
	mkc_Virt_Reset				= 65,
	mkc_Virt_Apply				= 66,
	mkc_TerminateProcess		= 67,
	mkc_IsInvisibleProcess		= 68,
	mkc_DisconnectAllClients	= 69,
	mkc_GetStatCounter			= 70,
	mkc_GetFidBox2				= 71,
	mkc_CreateFile				= 72,
	mkc_ReadFile				= 73,
	mkc_CloseFile				= 74,
	mkc_TimingGet				= 75,
} MKLIF_COMMAND;

typedef struct _MKLIF_COMMAND_MESSAGE {
	ULONG			m_ApiVersion;
	MKLIF_COMMAND	m_Command;
	ULONG			m_Reserved1;
	ULONG			m_Reserved2;
	UCHAR			m_Data[];
} MKLIF_COMMAND_MESSAGE, *PMKLIF_COMMAND_MESSAGE;

#include <pshpack1.h>
typedef struct _CLIENT_REGISTRATION {
	ULONG			m_AppID;
	ULONG			m_Priority;	
	ULONG			m_ClientFlags;
	ULONG			m_CacheSize;
	ULONG			m_BlueScreenTimeout;
	ULONG			m_Reserved1;
	ULONG			m_Reserved2;
	ULONG			m_Reserved3;
} CLIENT_REGISTRATION, *PCLIENT_REGISTRATION;

typedef struct _MKLIF_EVENT_HDR {
	ULONG			m_DrvMark;
    ULONG			m_EventSize;
	ULONG			m_HookID;
	ULONG			m_FunctionMj;
	ULONG			m_FunctionMi;
	ULONG			m_FilterID;
	ULONG			m_EventFlags;
	ULONG			m_Timeout;
	ULONG			m_ProcessId;
	ULONG			m_ThreadId;
	ULONG			m_ParamsCount;
} MKLIF_EVENT_HDR, *PMKLIF_EVENT_HDR;

typedef struct _MKLIF_EVENT {
	MKLIF_EVENT_HDR	m_EventHdr;
	UCHAR			m_Content[MKLIF_SMALL_EVENT_SIZE];
} MKLIF_EVENT, *PMKLIF_EVENT;

typedef struct _MKLIF_EVENT_TRANSMIT {
	MKLIF_EVENT_HDR	m_EventHdr;
	UCHAR			m_SingleParams[];
}MKLIF_EVENT_TRANSMIT, *PMKLIF_EVENT_TRANSMIT;

#define tefVerdict		           ULONG
#define efVerdict_Default          0x0000000
#define efVerdict_Pending          0x0000001
#define efVerdict_Allow            0x0000002
#define efVerdict_Deny             0x0000004
#define efVerdict_TerminateProcess 0x0000008
#define efVerdict_TerminateThread  0x0000010
#define efVerdict_Quarantine	   0x0000020
#define efVerdict_ContinueSearch   0x0000040
#define efVerdict_Cacheable		   0x0001000
#define efVerdict_ClientRequest	   0x0010000
#define efVerdict_NotFiltered	   0x1000000

#define efIsDeny( _verdict ) (FlagOn( _verdict, efVerdict_Deny | efVerdict_TerminateProcess | efVerdict_TerminateThread ))
#define efIsAllow( _verdict ) (!efIsDeny( _verdict ))

typedef struct _MKLIF_REPLY_EVENT {
	ULONG			m_Reserved;				// reserved for future
	ULONG			m_VerdictFlags;			// see efVerdict...
	ULONG			m_ExpTime;
	ULONG			m_Reserved1;
} MKLIF_REPLY_EVENT, *PMKLIF_REPLY_EVENT;

typedef struct _MKLIF_GET_EVENT_PART {
	ULONG				m_DrvMark;
	ULONG				m_Offset;
} MKLIF_GET_EVENT_PART, *PMKLIF_GET_EVENT_PART;

typedef enum _MKLIF_OBJECT_REQUEST_TYPE
{
	_object_request_read					= 0,
	_object_request_write					= 1,
	_object_requst_delete					= 2,
	_object_request_get_size				= 3,
	_object_request_get_proc_name			= 4,
	_object_request_get_proc_path			= 5,
	_object_request_set_verdict				= 6,
	_object_request_impersonate_thread		= 7,
	_object_request_get_enlisted_objects	= 8,
	_object_request_get_basicinfo			= 9,
	_object_request_get_fidbox				= 10,
	_object_request_set_fidbox				= 11,
	_object_request_set_preverdict			= 12,
}MKLIF_OBJECT_REQUEST_TYPE;

typedef struct _MKLIF_OBJECT_REQUEST
{
	ULONG						m_DrvMark;
	MKLIF_OBJECT_REQUEST_TYPE	m_RequestType;
	LARGE_INTEGER				m_Offset;
	ULONG						m_RequestBufferSize;
	CHAR						m_Buffer[];
}MKLIF_OBJECT_REQUEST, *PMKLIF_OBJECT_REQUEST;

//mkc_QueryProcessesInfo 
typedef enum _MKLIF_INFO_TAGS {
	mkpt_end								= 0,
	mkpt_process_id							= 1,
	mkpt_parent_process_id					= 2,
	mkpt_filename							= 3,
	mkpt_curr_dir							= 4,
	mkpt_cmd_line							= 5,
	mkpt_start_time							= 6,
	mkpt_base_ptr							= 7,
	mkpt_size								= 8,
	mkpt_entry_ptr							= 9,
	mkpt_flags								= 10,
	mkpt_volumename							= 11,
	mkpt_hash								= 12,
	mkpt_keyname							= 13,
} MKLIF_INFO_TAGS, *PMKLIF_INFO_TAGS;

typedef struct _MKLIF_INFO_ITEM
{
	MKLIF_INFO_TAGS		m_Tag;
	ULONG				m_ValueSize;
	CHAR				m_Value[0];
}MKLIF_INFO_ITEM, *PMKLIF_INFO_ITEM;

#define MKLIF_INFO_ITEM_SIZE sizeof(MKLIF_INFO_ITEM)

typedef struct _MKLIF_SETVERDICT_RESULT
{
	ULONG				m_Status;
	ULONG				m_Reserved1;
	ULONG				m_Reserved2;
	ULONG				m_Reserved3;
}MKLIF_SETVERDICT_RESULT, *PMKLIF_SETVERDICT_RESULT;

typedef struct _MKLIF_EVENT_DESCRIBE
{
	PVOID						m_pClientContext;
	PVOID						m_pMessage;
	ULONG						m_MessageSize;
	ULONG						m_Reserved1;
	ULONG						m_Reserved2;
	ULONG						m_Reserved3;
	ULONG						m_Reserved4;
}MKLIF_EVENT_DESCRIBE, *PMKLIF_EVENT_DESCRIBE;

typedef enum _MKLIF_CLIENT_COUNTERS {
	mkcc_None				= 0,
	mkcc_SendFaults			= 1,
	mkcc_SendTimeoutFaults	= 2,
	mkcc_SendSubFaults		= 3,
} MKLIF_CLIENT_COUNTERS, *PMKLIF_CLIENT_COUNTERS;

typedef struct _MKLIF_FIDBOX_DATA
{
	DWORD		m_DataFormatId;
	BYTE		m_VolumeID[16];
	BYTE		m_FileID[16];
	BYTE		m_Buffer[0];
} MKLIF_FIDBOX_DATA, *PMKLIF_FIDBOX_DATA;

//+ LLD structures
#define _LLD_RESOLVED_TYPE				0x0001
#define _LLD_RESOLVED_SECTOR_SIZE		0x0002
#define _LLD_RESOLVED_PARTITION_TYPE	0x0004
#define _LLD_RESOLVED_LENGTH			0x0008
#define _LLD_RESOLVED_PART_NUMBER		0x0010

#define	_LLD_FLAG_NONE					0x0000
#define	_LLD_FLAG_IS_PARTITIONAL		0x0001
#define	_LLD_FLAG_IS_LOCKED				0x0002
#define	_LLD_FLAG_IS_WRITE_PROTECTED	0x0004
#define	_LLD_FLAG_IS_BOOTABLE			0x0008
#define	_LLD_FLAG_IS_REMOVABLE			0x0100
#define	_LLD_FLAG_IS_RECOGNIZED			0x1000

typedef struct _MKLIF_LLD_INFO
{
	ULONG			m_Resolved;
	ULONG			m_Type;
	ULONG			m_SectorSize;
	ULONG			m_PartitionalType;
	ULONG			m_PartitionNumber;
	ULONG			m_Flags;
	ULONG			m_Reserved3;
	ULONG			m_Reserved4;
	LARGE_INTEGER	m_Length;
} MKLIF_LLD_INFO, *PMKLIF_LLD_INFO;

typedef struct _PMKLIF_LLD_GEOMETRY
{
  ULONG				m_MediaType;
  LARGE_INTEGER		m_Cylinders;
  ULONG				m_TracksPerCylinder;
  ULONG				m_SectorsPerTrack;
} MKLIF_LLD_GEOMETRY, *PMKLIF_LLD_GEOMETRY;

//- LLD structures

// #define MKLIF_ALLOW		0x0 //00
// #define MKLIF_ASK		0x1 //01
// #define MKLIF_DENY		0x3	//11
// 
// #define MKLIF_WRITE_BIT_OFFSET		0x00 //00 00 00 11
// #define MKLIF_READ_BIT_OFFSET		0x02 //00 00 11 00	
// #define MKLIF_ENUM_BIT_OFFSET		0x04 //00 11 00 00
// #define MKLIF_DELETE_BIT_OFFSET		0x06 //11 00 00 00

typedef struct _MKLIF_APPL_RULE
{
	ULONG		m_AccessMask;
	ULONG		m_blockID;
	ULONG		m_HashSize;
	CHAR		m_Hash[];	
	//дополнително выделяется 
	// ULONG	AppPathLen
	// WCHAR	AppPath[AppPathLen] //не содержит нулевой символ
	// ULONG	FilePathLen 
	// WCHAR	FilePath[FilePathLen] //не содержит нулевой символ
} MKLIF_APPL_RULE, *PMKLIF_APPL_RULE;

typedef struct _MKLIF_VIRT_APPL
{
	ULONG	AppPathLen;
	WCHAR	AppPath[1];
	//дополнително выделяется 
	// ULONG	SBPathLen 
	// WCHAR	SBPath[ SBFilePathLen ] //не содержит нулевой символ
	// ULONG	SBVolNameLen 
	// WCHAR	SBVolName[ SBVolNameLen ] //не содержит нулевой символ
} MKLIF_VIRT_APPL, *PMKLIF_VIRT_APPL;

typedef struct _MKLIF_RW_STAT
{
	ULONGLONG	m_StatCounter;
	ULONGLONG	m_CurrTime;
}MKLIF_RW_STAT, *PMKLIF_RW_STAT;


typedef struct _MKLIF_CREATE_FILE
{
// пока открываем для чтения
// 	ACCESS_MASK		DesiredAccess;
// 	ULONG			FileAttributes;
// 	ULONG			ShareAccess;
// 	ULONG			CreateDisposition;
// 	ULONG			CreateOptions;
// 	ULONG			Flags;
	ULONG			NativeFileNameSize;	
	WCHAR			NativeFileName[1];
}MKLIF_CREATE_FILE, *PMKLIF_CREATE_FILE;

typedef struct _MKLIF_READ_FILE
{
	ULONG			Size;
	LONGLONG		ByteOffset;
	ULONG			FileHandleSize;
	char			FileHandle[1];
}MKLIF_READ_FILE, *PMKLIF_READ_FILE;

#include <poppack.h>

#pragma warning (default : 4200)

#endif // __commdata_h
