#include "SysIO.h"
#include "runproc.h"
#include "..\lists.h"
#include "..\md5.h"

#include "fidbox.h"

FAST_MUTEX g_fmInternalSync;

#define OBJECT_TYPE_PROCESS		0x03
#define OBJECT_TYPE_FO			0x05
#define OBJECT_TYPE_THREAD		0x06
#define OBJECT_TYPE_REGKEY		0x32


void
SysMon_Init();

void
SysMon_Check();


BOOLEAN
ProbeForReadImp(
    IN CONST VOID  *Address,
    IN SIZE_T  Length,
    IN ULONG  Alignment
    )
{
	char* start = (char*)Address;
	char* end = start + Length;

	if (!MmIsAddressValid( (PVOID) Address ))
	{
		DbgBreakPoint();
		return FALSE;
	}

	if (Alignment)
	{
		if ((DWORD)Address % Alignment)
			return FALSE;
	}

	if (Length)
		end--;
	__try
	{
		int probe;
		*(DWORD*)&start &= 0xFFFFF000; // round start address to the page boundary
		while (start < end)
		{
			probe = *start;  // probe read
			start += 0x1000; // move to next page
		}
	}
	__except(EXCEPTION_EXECUTE_HANDLER)
	{
		return FALSE;
	}
	
	return TRUE;
}

PVOID
CopyUserBuffer (
	PVOID pContext,
	ULONG Size
	)
{
	PVOID pBuffer = NULL;
	BOOLEAN bKernel = FALSE;
	KPROCESSOR_MODE kMode;
	
	PMDL pmdl = NULL;

	if ( !pContext || !Size)
		return NULL;

	kMode = ExGetPreviousMode();

	if (KernelMode == kMode)
	{
		pBuffer = ExAllocatePoolWithTag( PagedPool, Size, 'zbos' );
		if (pBuffer)
		{
			memcpy( pBuffer, pContext, Size );
			return pBuffer;
		}

		return NULL;
	}

	if (!MmIsAddressValid( pContext ))
		return NULL;

	if (_MmUserProbeAddress)
	{
		if ( pContext > (PVOID) _MmUserProbeAddress )
			return NULL;
	}
	else
	{
		if ( pContext > (PVOID)MM_USER_PROBE_ADDRESS )
			return NULL;
	}

	pmdl = IoAllocateMdl( pContext, Size, FALSE, FALSE, NULL);
	if (!pmdl)
		return NULL;

	__try
	{
		MmProbeAndLockPages( pmdl, KernelMode, IoReadAccess );
	}
	__except(EXCEPTION_EXECUTE_HANDLER)
	{
		IoFreeMdl( pmdl );
		return NULL;
	}

	pBuffer = ExAllocatePoolWithTag( PagedPool, Size, 'zbos' );

	if (pBuffer)
	{
		__try
		{
			memcpy( pBuffer, pContext, Size );
		}
		__except(EXCEPTION_EXECUTE_HANDLER)
		{
			ExFreePool( pBuffer );
			pBuffer = NULL;
		}
	}

	if ( pmdl )
	{
		MmUnlockPages( pmdl );
		IoFreeMdl( pmdl );
	}

	return pBuffer;
}

PUNICODE_STRING
GetUserUniStr (
	PUNICODE_STRING pUserUniStr
	)
{
	PUNICODE_STRING pUniStr = NULL;
	PUNICODE_STRING pUniStrTmp = CopyUserBuffer( pUserUniStr, sizeof(UNICODE_STRING));
	if (pUniStrTmp)
	{
		USHORT Len = pUniStrTmp->Length;
		if (Len && pUniStrTmp->Buffer)
		{
			PVOID pBuffer = CopyUserBuffer( pUniStrTmp->Buffer, Len);
			if (pBuffer)
			{
				pUniStr = ExAllocatePoolWithTag( PagedPool, Len + sizeof(UNICODE_STRING), 'zbos' );
				if (pUniStr)
				{
					pUniStr->Length = Len;
					pUniStr->MaximumLength = Len;
					pUniStr->Buffer = (PWSTR)((char*) pUniStr + sizeof(UNICODE_STRING));

					memcpy( pUniStr->Buffer, pBuffer, Len );
				}

				ExFreePool( pBuffer );
			}
			else
			{
				DbgBreakPoint();
			}
		}
		
		ExFreePool( pUniStrTmp );
	}
	else
	{
		DbgBreakPoint();
	}

	return pUniStr;
}

//+ ------------------------------------------------------------------------------------------
NTSTATUS (__stdcall *__PsSetLoadImageNotifyRoutine)(IN PLOAD_IMAGE_NOTIFY_ROUTINE NotifyRoutine);

NTSTATUS (__stdcall *NtOpenProcess)(OUT PHANDLE ProcessHandle, IN ACCESS_MASK DesiredAccess, IN POBJECT_ATTRIBUTES ObjectAttributes, IN PCLIENT_ID ClientId OPTIONAL);
NTSTATUS (__stdcall *NtTerminateProcess)(IN HANDLE ProcessHandle, IN ULONG ProcessExitCode);
NTSTATUS (__stdcall *NtCreateProcess)(OUT PHANDLE ProcessHandle, IN ACCESS_MASK DesiredAccess, IN POBJECT_ATTRIBUTES ObjectAttributes OPTIONAL, IN HANDLE ParentProcess, IN BOOLEAN InheritObjectTable, IN HANDLE SectionHandle OPTIONAL, IN HANDLE DebugPort OPTIONAL, IN HANDLE ExceptionPort OPTIONAL);
NTSTATUS (__stdcall *NtCreateProcessEx)(OUT PHANDLE ProcessHandle, IN ACCESS_MASK DesiredAccess, IN POBJECT_ATTRIBUTES ObjectAttributes OPTIONAL, IN HANDLE ParentProcess, IN BOOLEAN InheritObjectTable, IN HANDLE SectionHandle OPTIONAL, IN HANDLE DebugPort OPTIONAL, IN HANDLE ExceptionPort OPTIONAL, IN ULONG UnknowDword OPTIONAL);


NTSTATUS (__stdcall *NtResumeThread)(IN PHANDLE ThreadHandle,OUT PULONG SuspendCount OPTIONAL);
NTSTATUS (__stdcall *NtSuspendThread)(IN PHANDLE ThreadHandle, OUT PULONG SuspendCount OPTIONAL);

NTSTATUS  (__stdcall *NtCreateThread)(OUT PHANDLE ThreadHandle, IN ACCESS_MASK DesiredAccess, 
		   IN POBJECT_ATTRIBUTES ObjectAttributes OPTIONAL, IN HANDLE ProcessHandle, OUT PCLIENT_ID ClientID, 
		   IN PCONTEXT Context, IN PUSER_STACK StackInfo, IN BOOLEAN CreateSuspended);
NTSTATUS (__stdcall *NtSetContextThread)(IN HANDLE hThread, IN const CONTEXT* lpContext);

NTSTATUS (__stdcall *NtClose)(IN HANDLE Handle);
NTSTATUS (__stdcall *NtCreateSection)(OUT PHANDLE SectionHandle, IN ACCESS_MASK DesiredAccess, IN POBJECT_ATTRIBUTES ObjectAttributes OPTIONAL, IN PLARGE_INTEGER MaximumSize OPTIONAL, IN ULONG SectionPageProtection, IN ULONG AllocationAttributes, IN HANDLE FileHandle OPTIONAL);

NTSTATUS (__stdcall *NtOpenSection)(OUT PHANDLE SectionHandle, IN ACCESS_MASK DesiredAccess, IN POBJECT_ATTRIBUTES ObjectAttributes);

NTSTATUS (__stdcall *NtCreateSymbolicLinkObject)(OUT PHANDLE ObjectHandle, IN ACCESS_MASK DesiredAccess, 
												 IN POBJECT_ATTRIBUTES ObjectAttributes, 
												 IN PUNICODE_STRING SubstituteString);
NTSTATUS (__stdcall *NtSetSystemInformation)(
	IN ULONG	SystemInformationClass,
    IN PVOID	SystemInformation,
    IN ULONG	Length
);

BOOLEAN (__stdcall *_pfFsRtlCheckLockForReadAccess)(IN PFILE_LOCK FileLock, IN PIRP Irp);

//NTSTATUS (__stdcall *NtTerminateThread)(IN HANDLE ThreadHandle, IN ULONG ThreadExitCode);

NTSTATUS (__stdcall *NtWriteVirtualMemory)(IN HANDLE ProcessHandle, IN PVOID StartAddress, IN PVOID Buffer, IN ULONG BytesToWrite, OUT PULONG BytesWritten OPTIONAL);

NTSTATUS (__stdcall *NtDuplicateObject)(IN HANDLE SourceProcessHandle,IN HANDLE SourceHandle, 
										IN HANDLE TargetProcessHandle, OUT PHANDLE TargetHandle OPTIONAL, 
										IN ACCESS_MASK DesiredAccess, IN ULONG Attributes, IN ULONG Options);

HANDLE (NTAPI *NtUserSetWindowsHookEx)(HANDLE Mod, PUNICODE_STRING ModuleName, DWORD ThreadId, 
									   int HookId, PVOID HookProc, BOOL Ansi);

// Gruzdev ----------------------------------------------------------------------------------------
typedef enum _WINDOWINFOCLASS {
    WindowProcess = 0,
    WindowThread,
    WindowActiveWindow,
    WindowFocusWindow,
    WindowIsHung,
    WindowClientBase,
    WindowIsForegroundThread,
} WINDOWINFOCLASS; 

HANDLE	(NTAPI *NtUserQueryWindow)(HANDLE hWnd, WINDOWINFOCLASS WindowInfo);

BOOLEAN (NTAPI *NtUserPostMessage)(
	HANDLE hWnd,
    ULONG Msg,
    ULONG wParam,
    ULONG lParam
	);

#include <pshpack4.h>

typedef struct  tagPOINT
    {
    LONG x;
    LONG y;
    }	POINT;

typedef struct tagPOINT *PPOINT;
typedef struct tagPOINT *LPPOINT;

#include <poppack.h>

HANDLE (NTAPI *NtUserWindowFromPoint)(POINT point);

NTSTATUS
(NTAPI
*NtSetInformationFile)(
	IN HANDLE FileHandle,
	OUT PIO_STATUS_BLOCK IoStatusBlock,
	IN PVOID FileInformation,
	IN ULONG FileInformationLength,
	IN FILE_INFORMATION_CLASS FileInformationClass
	);

SHORT (NTAPI *NtUserGetAsyncKeyState)(IN int vKey);
SHORT (NTAPI *NtUserGetKeyState)(IN int vKey);
BOOL ( NTAPI *NtUserGetKeyboardState ) ( OUT PBYTE pb	);
HANDLE (NTAPI *NtUserGetForegroundWindow)(VOID);

NTSTATUS
(NTAPI *NtSetSecurityObject)(
    IN HANDLE Handle,
    IN SECURITY_INFORMATION SecurityInformation,
    IN PSECURITY_DESCRIPTOR SecurityDescriptor
    );

#include <pshpack4.h>

#define INPUT_MOUSE     0
#define INPUT_KEYBOARD  1
#define INPUT_HARDWARE  2

typedef struct tagMOUSEINPUT {
    LONG    dx;
    LONG    dy;
    DWORD   mouseData;
    DWORD   dwFlags;
    DWORD   time;
    DWORD   dwExtraInfo;
} MOUSEINPUT, *PMOUSEINPUT, *LPMOUSEINPUT;

typedef struct tagKEYBDINPUT {
    WORD    wVk;
    WORD    wScan;
    DWORD   dwFlags;
    DWORD   time;
    DWORD   dwExtraInfo;
} KEYBDINPUT, *PKEYBDINPUT, *LPKEYBDINPUT;

typedef struct tagHARDWAREINPUT {
    DWORD   uMsg;
    WORD    wParamL;
    WORD    wParamH;
} HARDWAREINPUT, *PHARDWAREINPUT, *LPHARDWAREINPUT;

typedef struct tagINPUT {
    DWORD   type;

    union
    {
        MOUSEINPUT      mi;
        KEYBDINPUT      ki;
        HARDWAREINPUT   hi;
    };
} INPUT, *PINPUT, *LPINPUT;

#include <poppack.h>

UINT (NTAPI *NtUserSendInput)(
    IN UINT    cInputs,
    IN CONST INPUT *pInputs,
    IN int     cbSize);

HANDLE (NTAPI *NtUserFindWindowEx)(
    IN HANDLE hwndParent,
    IN HANDLE hwndChild,
    IN PUNICODE_STRING pstrClassName,
    IN PUNICODE_STRING pstrWindowName,
	IN ULONG dwType);


NTSTATUS
(NTAPI *NtLoadDriver)(
    IN PUNICODE_STRING DriverServiceName
    );

typedef HANDLE HDC;

BOOL (NTAPI *NtGdiBitBlt)(
    HDC    hdcDst,
    int    x,
    int    y,
    int    cx,
    int    cy,
    HDC    hdcSrc,
    int    xSrc,
    int    ySrc,
    DWORD  rop4,
    DWORD  crBackColor,
	DWORD  Unknown
	);

typedef enum _DEBUG_CONTROL_CODE {
	DebugGetTraceInformation = 1,
	DebugSetInternalBreakpoint,
	DebugSetSpecialCall,
	DebugClearSpecialCalls,
	DebugQuerySpecialCalls,
	DebugDbgBreakPoint
} DEBUG_CONTROL_CODE;


NTSTATUS
(NTAPI *NtSystemDebugControl)(
	IN DEBUG_CONTROL_CODE ControlCode,
	IN PVOID InputBuffer OPTIONAL,
	IN ULONG InputBufferLength,
	OUT PVOID OutputBuffer OPTIONAL,
	IN ULONG OutputBufferLength,
	OUT PULONG ReturnLength OPTIONAL
	);

typedef USHORT ATOM, RTL_ATOM, *PATOM, *PRTL_ATOM;

typedef HANDLE HCONV;

typedef enum _ATOM_INFORMATION_CLASS {
    AtomBasicInformation = 0,
    AtomTableInformation
} ATOM_INFORMATION_CLASS;

#include <pshpack1.h>

typedef struct _ATOM_BASIC_INFORMATION {
    USHORT UsageCount;
    USHORT Flags;
    USHORT NameLength;
    WCHAR Name[1];
} ATOM_BASIC_INFORMATION, *PATOM_BASIC_INFORMATION;

#include <poppack.h>

NTSYSAPI
NTSTATUS
NtQueryInformationAtom(
    IN RTL_ATOM Atom,
    IN ATOM_INFORMATION_CLASS AtomInformationClass,
    OUT PVOID AtomInformation,
    IN ULONG AtomInformationLength,
    OUT PULONG ReturnLength OPTIONAL
    );

/*
#include <pshpack1.h>

// код, эквивалентный near jump на FuncPtr
typedef struct _NEARJUMP
{
	UCHAR		m_JmpNearRel_Opcode;	// jump near relative = 0xe9
	LONG		m_FuncOffset;			// смещение на ф-и, на которою надо перейти, от m_JmpNearRel_Opcode
} NEARJUMP, *PNEARJUMP;

#define INIT_NEARJUMP(NearJump, pJumpPlace, pWhereToJump)													\
{																											\
	NearJump.m_JmpNearRel_Opcode = 0xe9;																	\
	NearJump.m_FuncOffset = (LONG)pWhereToJump-(LONG)&((PNEARJUMP)pJumpPlace)->m_JmpNearRel_Opcode-5;		\
}

#define MAX_PROLOGUE_SIZE		20

typedef struct _ORIG_FUNC_CALL
{
	UCHAR		m_Nops[MAX_PROLOGUE_SIZE];	// NOP'ы под пролог ф-и
	UCHAR		m_JmpNearRel_Opcode;		// jump near relative = 0xe9
	LONG		m_FuncOffset;				// указатель на ф-ю
} ORIG_FUNC_CALL, *PORIG_FUNC_CALL;			// смещение на ф-и, на которою надо перейти, от m_JmpNearRel_Opcode
*/
//#define INIT_ORIG_FUNC_CALL(OrigFuncCall, pOrigFuncCallPlace, pOrigFunc)												\
//{																														\
//	memset(OrigFuncCall.m_Nops, 0x90/*NOP opcode*/, MAX_PROLOGUE_SIZE);													\
//	OrigFuncCall.m_JmpNearRel_Opcode = 0xe9;																			\
//	OrigFuncCall.m_FuncOffset = (LONG)pOrigFunc-(LONG)&((PORIG_FUNC_CALL)pOrigFuncCallPlace)->m_JmpNearRel_Opcode-5;	\
//}
/*
#include <poppack.h>

typedef
NTSTATUS (NTAPI *f_IoAttachDevice)(IN PDEVICE_OBJECT  SourceDevice, IN PUNICODE_STRING  TargetDevice,
								   OUT PDEVICE_OBJECT  *AttachedDevice);
typedef
NTSTATUS (NTAPI *f_IoAttachDeviceByPointer)(IN PDEVICE_OBJECT  SourceDevice, IN PDEVICE_OBJECT  TargetDevice);
typedef
PDEVICE_OBJECT (NTAPI *f_IoAttachDeviceToDeviceStack)(IN PDEVICE_OBJECT  SourceDevice, IN PDEVICE_OBJECT  TargetDevice);
typedef
NTSTATUS (NTAPI *f_IoAttachDeviceToDeviceStackSafe)(IN PDEVICE_OBJECT  SourceDevice, IN PDEVICE_OBJECT  TargetDevice,
													IN OUT PDEVICE_OBJECT  *AttachedToDeviceObject);

typedef
struct _CHECK_FUNC_PROLOGUE
{
	UCHAR	m_Prologue[MAX_PROLOGUE_SIZE];
	ULONG	m_PrologueSize;
} CHECK_FUNC_PROLOGUE, *PCHECK_FUNC_PROLOGUE;

#define MAX_PROLOGUE_VARIANTS	20

typedef
struct _SPLICED_FUNC
{
	CHECK_FUNC_PROLOGUE	m_CheckFuncPrologues[MAX_PROLOGUE_VARIANTS];
	ULONG				m_NumPrologueVariants;
	ORIG_FUNC_CALL		m_rFuncCall;
	NEARJUMP			m_rFuncPrologue;
} SPLICED_FUNC, *PSPLICED_FUNC;

// /SECTION:.exdata,ERW
#pragma data_seg(".exdata")


__declspec(allocate(".exdata"))
SPLICED_FUNC gIoAttachDevice_Splice =					{
															{
																{ {	0x8b, 0xff, 0x55, 0x8b, 0xec	}, 5	}
															},
															1
														};
__declspec(allocate(".exdata"))
SPLICED_FUNC gIoAttachDeviceByPointer_Splice =			{
															{
																{ {	0x8b, 0xff, 0x55, 0x8b, 0xec	}, 5	}
															},
															1
														};
__declspec(allocate(".exdata"))
SPLICED_FUNC gIoAttachDeviceToDeviceStack_Splice =		{
															{
																{ {	0x8b, 0xff, 0x55, 0x8b, 0xec	}, 5	}
															},
															1
														};
__declspec(allocate(".exdata"))
SPLICED_FUNC gIoAttachDeviceToDeviceStackSafe_Splice =	{
															{
																{ {	0x8b, 0xff, 0x55, 0x8b, 0xec	}, 5	}
															},
															1
														};
*/

ULONG (NTAPI *NtUserMessageCall)(
    IN HANDLE hwnd,
    IN ULONG msg,
    IN ULONG wParam,
    IN ULONG lParam,
    IN PVOID xParam,
    IN ULONG xpfnProc,
    IN ULONG bAnsi);

ULONG (NTAPI *NtUserSendNotifyMessage)(
    IN HANDLE hwnd,
    IN ULONG msg,
    IN ULONG wParam,
    IN ULONG lParam OPTIONAL);

ULONG (NTAPI *NtUserPostThreadMessage)(
    IN ULONG id,
    IN ULONG msg,
    IN ULONG wParam,
    IN ULONG lParam);

ULONG (NTAPI *NtUserSetWinEventHook)(
    IN ULONG           eventMin,
    IN ULONG           eventMax,
    IN HANDLE          hmodWinEventProc,
    IN PUNICODE_STRING pstrLib OPTIONAL,
    IN PVOID           pfnWinEventProc,
    IN ULONG           idEventProcess,
    IN ULONG           idEventThread,
    IN ULONG           dwFlags);

//+ LPC/COM stuff ---------------------------------------------------------------------------------
#define RPC_MAPPER_PORT		L"\\RPC Control\\epmapper"

typedef PVOID PPORT_VIEW;
typedef PVOID PREMOTE_PORT_VIEW;

NTSTATUS
(NTAPI *NtConnectPort) (
			   OUT PHANDLE PortHandle,
			   IN PUNICODE_STRING PortName,
			   IN PSECURITY_QUALITY_OF_SERVICE SecurityQos,
			   IN OUT PPORT_VIEW ClientView OPTIONAL,
			   IN OUT PREMOTE_PORT_VIEW ServerView OPTIONAL,
			   OUT PULONG MaxMessageLength OPTIONAL,
			   IN OUT PVOID ConnectionInformation OPTIONAL,
			   IN OUT PULONG ConnectionInformationLength OPTIONAL
			   ) = NULL;
NTSTATUS
(NTAPI *NtRequestWaitReplyPort) (
						IN HANDLE PortHandle,
						IN PPORT_MESSAGE RequestMessage,
						OUT PPORT_MESSAGE ReplyMessage
						) = NULL;


//- LPC/COM stuff ---------------------------------------------------------------------------------
// AttachThreadInputApi
BOOL
(NTAPI *NtUserAttachThreadInput) (
							 IN DWORD idAttach,
							 IN DWORD idAttachTo,
							 IN BOOL fAttach
							 );
					 

#ifdef _WIN64
typedef ULONGLONG ULONG_PTR;
#else
typedef ULONG ULONG_PTR;
#endif

#include <pshpack1.h>

// list entry for InLoadOrderModuleList
typedef struct _LDR_MODULE_3
{
	LIST_ENTRY              InLoadOrderModuleList;
	LIST_ENTRY              InMemoryOrderModuleList;
	LIST_ENTRY              InInitializationOrderModuleList;
	PVOID                   BaseAddress;
	PVOID                   EntryPoint;
	ULONG_PTR               SizeOfImage;
	UNICODE_STRING          FullDllName;
	UNICODE_STRING          BaseDllName;
	ULONG                   Flags;
	SHORT                   LoadCount;
	SHORT                   TlsIndex;
	LIST_ENTRY              HashTableEntry;
	ULONG                   TimeDateStamp;
} LDR_MODULE_3, *PLDR_MODULE_3;

// list entry for InMemoryOrderModuleList
typedef struct _LDR_MODULE_2
{
	LIST_ENTRY              InMemoryOrderModuleList;
	LIST_ENTRY              InInitializationOrderModuleList;
	PVOID                   BaseAddress;
	PVOID                   EntryPoint;
	ULONG_PTR				SizeOfImage;
	UNICODE_STRING          FullDllName;
	UNICODE_STRING          BaseDllName;
	ULONG                   Flags;
	SHORT                   LoadCount;
	SHORT                   TlsIndex;
	LIST_ENTRY              HashTableEntry;
	ULONG                   TimeDateStamp;
} LDR_MODULE_2, *PLDR_MODULE_2;

// list entry for InInitializationOrderModuleList
typedef struct _LDR_MODULE_1
{
	LIST_ENTRY              InInitializationOrderModuleList;
	PVOID                   BaseAddress;
	PVOID                   EntryPoint;
	ULONG                   SizeOfImage;
	UNICODE_STRING          FullDllName;
	UNICODE_STRING          BaseDllName;
	ULONG_PTR               Flags;
	SHORT                   LoadCount;
	SHORT                   TlsIndex;
	LIST_ENTRY              HashTableEntry;
	ULONG                   TimeDateStamp;
} LDR_MODULE_1, *PLDR_MODULE_1;

typedef struct _PEB_LDR_DATA
{
	ULONG			Length;
	BOOLEAN			Initialized;
	UCHAR			Filler[3];
	HANDLE			SsHandle;
	LIST_ENTRY		InLoadOrderModuleList;
	LIST_ENTRY		InMemoryOrderModuleList;
	LIST_ENTRY		InInitializationOrderModuleList;
	PVOID			EntryInProgress;
} PEB_LDR_DATA, *PPEB_LDR_DATA;

typedef struct _intPEB
{
	BOOLEAN			InheritedAddressSpace;
	BOOLEAN			ReadImageFileExecOptions;
	BOOLEAN			BeingDebugged;
	UCHAR			BitField;
#ifdef _WIN64
	ULONG			Filler;
#endif
	PRKMUTANT		Mutant;
	PVOID			ImageBaseAddress;
	PPEB_LDR_DATA	Ldr;
	PVOID			ProcessParameters;
//	UCHAR			Filler2[0x22c];
} intPEB, *PintPEB;

#include <poppack.h>

//- -------------------------------------------------------------------------------------------------

NTSTATUS (__stdcall *_ZwQueryInformationFile_)(IN HANDLE FileHandle, OUT PIO_STATUS_BLOCK  IoStatusBlock, OUT PVOID  FileInformation, IN ULONG Length, IN FILE_INFORMATION_CLASS  FileInformationClass);
NTSTATUS (__stdcall *_ZwQuerySystemInformation)(IN ULONG InfoClass,OUT PVOID SysInfo,IN ULONG SysInfoLen,OUT PULONG LenReturned OPTIONAL);


//+ ------------------------------------------------------------------------------------------
PKLG_SYS_NAME_CACHE gpSysContext = NULL;

NPAGED_LOOKASIDE_LIST gPSysFileNames;
PNPAGED_LOOKASIDE_LIST gpPSysFileNames = &gPSysFileNames;

#define _SYSQUERY_INFO_LEN	0x4000

NPAGED_LOOKASIDE_LIST gPL0x140;
PNPAGED_LOOKASIDE_LIST gpPL0x140 = &gPL0x140;

NPAGED_LOOKASIDE_LIST gPL0x1000;
PNPAGED_LOOKASIDE_LIST gpPL0x1000 = &gPL0x1000;


NPAGED_LOOKASIDE_LIST gPL0x4000;
PNPAGED_LOOKASIDE_LIST gpPL0x4000 = &gPL0x4000;

// Gruzdev
PAGED_LOOKASIDE_LIST g_PLL_AsyncKeyReaders;
//+ ------------------------------------------------------------------------------------------
#define _countof(_arr) (sizeof(_arr) / sizeof(_arr[0]))

typedef struct _tProcInfoItem
{
	ULONG m_ImagesLoadedCount;
	BOOLEAN m_bKernel32Hooked;
	PWCHAR m_pwchmainImage;
}ProcInfoItem;

ProcInfoItem ProcIdtoHandle[0x1000];

//+ ------------------------------------------------------------------------------------------

typedef struct _tSYS_CACHEITEM
{
	ULONG m_Id;
	WCHAR m_Name[0];
}SYS_CACHEITEM, *PSYS_CACHEITEM;

typedef struct _tSYS_NOTIFY_PROCESS_CONTEXT
{
	HANDLE m_ParentId;
	HANDLE m_ProcessId;
}SYS_NOTIFY_PROCESS_CONTEXT, *PSYS_NOTIFY_PROCESS_CONTEXT;

typedef struct _tSYS_NOTIFY_THREAD_CONTEXT
{
	ULONG m_ProcessId;
	ULONG m_ThreadId;
}SYS_NOTIFY_THREAD_CONTEXT, *PSYS_NOTIFY_THREAD_CONTEXT;

typedef struct _tSYS_OPEN_PROCESS_CONTEXT
{
	ACCESS_MASK m_DesiredAccess;
	ULONG m_ProcessClientId;
	NTSTATUS m_Status;
} SYS_OPEN_PROCESS_CONTEXT, *PSYS_OPEN_PROCESS_CONTEXT;

typedef struct _tSYS_CREATE_PROCESS_CONTEXT
{
	PHANDLE m_ProcessHandle;
	ACCESS_MASK m_DesiredAccess;
	PWCHAR m_pFileName;
	
	NTSTATUS m_Status;
} SYS_CREATE_PROCESS_CONTEXT, *PSYS_CREATE_PROCESS_CONTEXT;

typedef struct _tSYS_CREATE_SECTION_CONTEXT
{
	PWCHAR m_pFileName;
	ACCESS_MASK m_DesiredAccess;

	ULONG m_ObjAttributes;
	ULONG m_SectionPageProtection;
	
	NTSTATUS m_Status;
} SYS_CREATE_SECTION_CONTEXT, *PSYS_CREATE_SECTION_CONTEXT;

typedef struct _tSYS_TERMINATE_PROCESS
{
	ULONG m_ProcessId;		//current
	ULONG m_DestProcId;			//kogo
	ULONG m_AccessAttributes;
	CHAR m_ProcName[KLMAX_PROCNAME];
	NTSTATUS m_Status;
}SYS_TERMINATE_PROCESS, *PSYS_TERMINATE_PROCESS;

typedef struct _tSYS_WRITE_PROCESS_MEMORY_CONTEXT
{
	ULONG m_DestProcId;			//kogo
	PVOID m_StartAddress;
	HANDLE	ProcessHandle;
	ULONG m_BytesToWrite;
	PVOID m_Buffer;
} SYS_WRITE_PROCESS_MEMORY_CONTEXT, *PSYS_WRITE_PROCESS_MEMORY_CONTEXT;

// Gruzdev
typedef
struct _ASYNC_KEY_READER
{
	LIST_ENTRY		m_List;

	HANDLE			m_PID;
	LARGE_INTEGER	m_LastReadTime;
	ULONG			m_FastReadCount;
} ASYNC_KEY_READER, *PASYNC_KEY_READER;

LIST_ENTRY gAsyncKeyReaders_Head;
FAST_MUTEX g_fmAsyncKeyLock;
//+ ------------------------------------------------------------------------------------------
typedef struct _DRVNAME_LISTITEM
{
    IMPLIST_ENTRY	m_List;
	DWORD			m_Hash;
    PWCHAR			m_DrvName;
} DRVNAME_LISTITEM, *PDRVNAME_LISTITEM;

IMPLIST_ENTRY	gDrvNamesList;
ULONG gDrvInList = 0;
#define _MaxDrvInList	256

//+ Gruzdev -----------------------------------------------------------------------------------
#define GEN_CACHE_SIGNATURE				0x21081979
// hash func
#define VMEM_HASH_FUNC( __x__, __s__ )	( (ULONG) ( __x__ ) & ( __s__ - 1 ) )

typedef struct _GEN_CACHE_ITEM
{
	LIST_ENTRY		m_List;
	ULONG			m_Signature;
	HANDLE			m_Key;
	UCHAR			m_Data[1];
} GEN_CACHE_ITEM, *PGEN_CACHE_ITEM;

typedef struct _GEN_CACHE
{
	ULONG			m_HashSize;
	ULONG			m_DataSize;
	FAST_MUTEX		m_CacheLock;		// global cache lock
	LIST_ENTRY		m_HashTable[1];
} GEN_CACHE, *PGEN_CACHE;

BOOLEAN GenCacheInit(
			IN ULONG HashSize,
			IN ULONG DataSize,
			OUT PGEN_CACHE *p_pGenCache
		)
{
	PGEN_CACHE pCache;
	ULONG i;

	pCache = (PGEN_CACHE) ExAllocatePoolWithTag(
		PagedPool,
		FIELD_OFFSET( GEN_CACHE, m_HashTable ) + HashSize * sizeof( LIST_ENTRY ),
		'CGrg'
		);
	if ( !pCache )
		return FALSE;

	pCache->m_HashSize = HashSize;
	pCache->m_DataSize = DataSize;
	ExInitializeFastMutex( &pCache->m_CacheLock );

	for ( i = 0; i < HashSize; i++ )
		InitializeListHead( &pCache->m_HashTable[i] );

	*p_pGenCache = pCache;
	
	return TRUE;
}

VOID GenCacheDone( IN OUT PGEN_CACHE *p_pCache )
{
	PGEN_CACHE pCache;
	ULONG Hash;

	ASSERT( p_pCache );
	pCache = *p_pCache;
	ASSERT( pCache );

	ExAcquireFastMutex( &pCache->m_CacheLock );

	for ( Hash = 0; Hash < pCache->m_HashSize; Hash++ )
	{
		PLIST_ENTRY pListHead;
		PGEN_CACHE_ITEM pCacheItem;

		pListHead = &pCache->m_HashTable[Hash];

		pCacheItem = (PGEN_CACHE_ITEM) pListHead->Flink;
		while ( pCacheItem != (PGEN_CACHE_ITEM) pListHead )
		{
			PGEN_CACHE_ITEM pTmpCacheItem;

			ASSERT( GEN_CACHE_SIGNATURE == pCacheItem->m_Signature );	

			pTmpCacheItem = (PGEN_CACHE_ITEM) pCacheItem->m_List.Flink;
			ExFreePool( pCacheItem );
			pCacheItem = pTmpCacheItem;
		}
	}

	ExReleaseFastMutex( &pCache->m_CacheLock );

	*p_pCache = NULL;
}

PVOID GenCacheGet(
		IN PGEN_CACHE pCache,
		IN HANDLE Key,
		BOOLEAN bCreateNew,
		PBOOLEAN p_bNewReturned
		)
{
	ULONG Hash;
	PLIST_ENTRY pListHead;
	PGEN_CACHE_ITEM pCacheItem;
	PGEN_CACHE_ITEM pRetItem = NULL;
	BOOLEAN bNewReturned = FALSE;

	ASSERT( pCache );
	ASSERT( p_bNewReturned );

	Hash = VMEM_HASH_FUNC( Key, pCache->m_HashSize );

	ExAcquireFastMutex( &pCache->m_CacheLock );

	pListHead = &pCache->m_HashTable[Hash];

	for ( pCacheItem = (PGEN_CACHE_ITEM) pListHead->Flink;
		pCacheItem != (PGEN_CACHE_ITEM) pListHead;
		pCacheItem = (PGEN_CACHE_ITEM) pCacheItem->m_List.Flink 
		)
	{
		ASSERT( GEN_CACHE_SIGNATURE == pCacheItem->m_Signature );	

		if ( Key == pCacheItem->m_Key )
		{
			pRetItem = pCacheItem;
			break;
		}	
	}

	if ( !pRetItem && bCreateNew )
	{
		pRetItem = (PGEN_CACHE_ITEM) ExAllocatePoolWithTag(
			PagedPool,
			FIELD_OFFSET( GEN_CACHE_ITEM, m_Data ) + pCache->m_DataSize,
			'ICrg'
			);
		if ( pRetItem )
		{
			pRetItem->m_Signature = GEN_CACHE_SIGNATURE;
			pRetItem->m_Key = Key;

			InsertTailList( pListHead, &pRetItem->m_List );

			bNewReturned = TRUE;
		}
	}

	ExReleaseFastMutex( &pCache->m_CacheLock );

	*p_bNewReturned = bNewReturned;

	return pRetItem ? &pRetItem->m_Data : NULL;
}

VOID GenCacheFree(
		IN PGEN_CACHE pCache,
		IN HANDLE Key
		)
{
	ULONG Hash;
	PLIST_ENTRY pListHead;
	PGEN_CACHE_ITEM pCacheItem;
	PGEN_CACHE_ITEM pFoundItem = NULL;

	ASSERT( pCache );

	Hash = VMEM_HASH_FUNC( Key, pCache->m_HashSize );

	ExAcquireFastMutex( &pCache->m_CacheLock );
	
	pListHead = &pCache->m_HashTable[Hash];
	
	for (	pCacheItem = (PGEN_CACHE_ITEM) pListHead->Flink;
			pCacheItem != (PGEN_CACHE_ITEM) pListHead;
			pCacheItem = (PGEN_CACHE_ITEM) pCacheItem->m_List.Flink 
		)
	{
		ASSERT( GEN_CACHE_SIGNATURE == pCacheItem->m_Signature );	

		if ( Key == pCacheItem->m_Key )
		{
			pFoundItem = pCacheItem;
			break;
		}	
	}
	
	if ( pFoundItem )	
	{		
		RemoveEntryList( &pFoundItem->m_List );		
		ExFreePool( pFoundItem );
	}
	
	ExReleaseFastMutex( &pCache->m_CacheLock );

}

PGEN_CACHE g_pRPCMapperPortsCache = NULL;
PGEN_CACHE g_pAttachedInputThreadCache = NULL;
//- -----------------------------------------------------------------------------------------------

ULONG Get_Window_Owner_PID(IN HANDLE hWnd);

volatile PDEVICE_OBJECT g_pi8042prtDevice = NULL;
volatile PDEVICE_OBJECT g_pKbdClassDevice = NULL;

// получаем и референсим базовый девайс..
NTSTATUS ReferenceBaseDeviceByName(IN PUNICODE_STRING DevicePath, OUT PDEVICE_OBJECT *p_pDeviceObject)
{
	OBJECT_ATTRIBUTES OA;
	NTSTATUS ntStatus;
	HANDLE hFile;
	IO_STATUS_BLOCK ioStatus;
	PFILE_OBJECT pFileObject;
	PDEVICE_OBJECT pDeviceObject;

	InitializeObjectAttributes(&OA, DevicePath, OBJ_CASE_INSENSITIVE, NULL, NULL);

	ntStatus = ZwOpenFile(&hFile, 0,  &OA, &ioStatus, FILE_SHARE_READ | FILE_SHARE_WRITE, FILE_NON_DIRECTORY_FILE);
	if (NT_SUCCESS(ntStatus ))
	{
		ntStatus = ObReferenceObjectByHandle(hFile, 0, *IoFileObjectType, KernelMode, &pFileObject, NULL);
		if (NT_SUCCESS(ntStatus))
		{
			pDeviceObject = IoGetBaseFileSystemDeviceObject(pFileObject);
			if (pDeviceObject)
			{
				ntStatus = ObReferenceObjectByPointer(pDeviceObject, 0, NULL, KernelMode);
				if (NT_SUCCESS(ntStatus))
					*p_pDeviceObject = pDeviceObject;
			}

			ObDereferenceObject(pFileObject);
		}

		ZwClose(hFile);
	}

	return ntStatus;
}

BOOLEAN IsLowerDeviceOf(IN PDEVICE_OBJECT pDeviceLow, IN PDEVICE_OBJECT pDeviceHigh)
{
	PDEVICE_OBJECT pDevObj;

	pDevObj = pDeviceLow->AttachedDevice;
	while (pDevObj)
	{
		if (pDeviceHigh == pDevObj)
			return TRUE;

		pDevObj = pDevObj->AttachedDevice;
	}

	return FALSE;
}

/*
BOOLEAN IsDeviceInKbdStack(IN PDEVICE_OBJECT pDeviceObject OPTIONAL, IN PUNICODE_STRING DeviceName OPTIONAL)
{
	PDEVICE_OBJECT pTargetDevice;
	PFILE_OBJECT pFileObject;
	NTSTATUS ntStatus;
	BOOLEAN bResult = FALSE;
	PDEVICE_OBJECT pCurrDevice;

	if (pDeviceObject == NULL && DeviceName == NULL)
		return FALSE;

	pTargetDevice = pDeviceObject;
	if (!pDeviceObject)
	{
		if (!DeviceName)
			return FALSE;

		ntStatus = IoGetDeviceObjectPointer(DeviceName, 0, &pFileObject, &pTargetDevice);
		if (!NT_SUCCESS(ntStatus))
			return FALSE;
		ObDereferenceObject(pFileObject);
	}
	ObReferenceObject(pTargetDevice);

	if (g_pKbdClassDevice)
	{
		pCurrDevice = g_pKbdClassDevice;

	// в предположении, что pTargetDevice выше(или на том же уровне) по стеку, чем \Device\KeyboardClass0
		do
		{
			if (pCurrDevice == pTargetDevice)
			{
				bResult = TRUE;
				break;
			}

			pCurrDevice = pCurrDevice->AttachedDevice;
		} while (pCurrDevice);

	// в предположении, что pTargetDevice ниже по стеку, чем \Device\KeyboardClass0
		if (!bResult)
		{
			pCurrDevice = pTargetDevice;			
			do
			{
				if (pCurrDevice == g_pKbdClassDevice)
				{
					bResult = TRUE;
					break;
				}

				pCurrDevice = pCurrDevice->AttachedDevice;
			} while (pCurrDevice);
		}
	}

	ObDereferenceObject(pTargetDevice);

	return bResult;
}
*/
// освобождать буфер надо с помощью ExFreeToNPagedLookasideList(gpPL0x1000, ...);
POBJECT_NAME_INFORMATION QueryObjectName(IN PVOID pObject)
{
	POBJECT_NAME_INFORMATION pDeviceInfo;
	NTSTATUS ntStatus;

	if (pObject == NULL)
		return NULL;

	pDeviceInfo = (POBJECT_NAME_INFORMATION)ExAllocateFromNPagedLookasideList(gpPL0x1000);
	if (pDeviceInfo)
	{
		ULONG RetLen;

		ntStatus = ObQueryNameString(pObject, pDeviceInfo, 0x1000, &RetLen);
		if (NT_SUCCESS(ntStatus) && RetLen && pDeviceInfo->Name.Length && pDeviceInfo->Name.Buffer)
			return pDeviceInfo;
		
		ExFreeToNPagedLookasideList(gpPL0x1000, pDeviceInfo);
	}

	return NULL;
}
/*
VERDICT CommonAttachKbdDevice(IN PDEVICE_OBJECT pSourceDeviceObject, IN PDEVICE_OBJECT pTargetDeviceObject OPTIONAL,
			  IN PUNICODE_STRING TargetDeviceName OPTIONAL)
{
	VERDICT Verdict = Verdict_Default;
	NTSTATUS ntStatus;

	if (pSourceDeviceObject == NULL || (pTargetDeviceObject == NULL && TargetDeviceName == NULL))
		return Verdict_Default;

	if (IsDeviceInKbdStack(pTargetDeviceObject, TargetDeviceName))
	{
*///		if (IsNeedFilterEvent(FLTTYPE_SYSTEM, 0x14/*MJ_SYSTEM_KEYLOGGER_DETECTED*/, 0))
/*		{
			PFILTER_EVENT_PARAM	pParam = (PFILTER_EVENT_PARAM)ExAllocateFromNPagedLookasideList(gpPL0x1000);
			
			if (pParam)
			{
				PSINGLE_PARAM pSingleParam;
				PWCHAR	DriverName,
						_DriverName;
*/
//				FILTER_PARAM_COMMONINIT(pParam, FLTTYPE_SYSTEM, 0x14/*MJ_SYSTEM_KEYLOGGER_DETECTED*/, 0, PostProcessing, 3);
/*				pSingleParam = (PSINGLE_PARAM)pParam->Params;

				SINGLE_PARAM_INIT_SID(pSingleParam);

				SINGLE_PARAM_SHIFT(pSingleParam);
				SINGLE_PARAM_INIT_ULONG(pSingleParam, _PARAM_OBJECT_KEYLOGGERID, KEYLOGGER_ATTACHKBD);

				SINGLE_PARAM_SHIFT(pSingleParam);
				SINGLE_PARAM_INIT_NONE(pSingleParam, _PARAM_OBJECT_URL_W);
				((PWCHAR)pSingleParam->ParamValue)[0] = 0;
				pSingleParam->ParamSize = sizeof(WCHAR);
				DriverName = QueryObjectName(pSourceDeviceObject->DriverObject);
				if (DriverName)
				{
					wcscpy((PWCHAR)pSingleParam->ParamValue, DriverName);
					pSingleParam->ParamSize = wcslen(DriverName)*sizeof(WCHAR);

					ExFreeToNPagedLookasideList(gpPL0x1000, DriverName);
				}
				_DriverName = (PWCHAR)pSingleParam->ParamValue;

				Verdict = FilterEvent(pParam, NULL);

				ExFreeToNPagedLookasideList(gpPL0x1000, pParam);

				DbPrint(DC_SYS, DL_IMPORTANT, ("CommonAttachKbdDevice: attaching device 0x%08x of driver \"%S\" to keyboard stack\n",
					pSourceDeviceObject, _DriverName));
			}
		}			
	}

	return Verdict;
}

NTSTATUS NTAPI KLHookIoAttachDevice(
    IN PDEVICE_OBJECT  SourceDevice,
    IN PUNICODE_STRING  TargetDevice,
    OUT PDEVICE_OBJECT  *AttachedDevice
    )
{
	NTSTATUS ntStatus;

	ntStatus = ((f_IoAttachDevice)&gIoAttachDevice_Splice.m_rFuncCall)(SourceDevice, TargetDevice, AttachedDevice);

	DbPrint(DC_SYS, DL_IMPORTANT, ("KLHookIoAttachDevice: attaching device 0x%08x to device \"%S\", Status=0x%08x\n",
		SourceDevice, TargetDevice, ntStatus));

	if (NT_SUCCESS(ntStatus))
		CommonAttachKbdDevice(SourceDevice, NULL, TargetDevice);

	return ntStatus;
}

NTSTATUS NTAPI KLHookIoAttachDeviceByPointer(
	IN PDEVICE_OBJECT  SourceDevice,
    IN PDEVICE_OBJECT  TargetDevice
	)
{
	NTSTATUS ntStatus;

	ntStatus = ((f_IoAttachDeviceByPointer)&gIoAttachDeviceByPointer_Splice.m_rFuncCall)(SourceDevice, TargetDevice);

	DbPrint(DC_SYS, DL_IMPORTANT, ("KLHookIoAttachDeviceByPointer: attaching device 0x%08x to device 0x%08x, Status=0x%08x\n",
		SourceDevice, TargetDevice, ntStatus));

	if (NT_SUCCESS(ntStatus))
		CommonAttachKbdDevice(SourceDevice, TargetDevice, NULL);

	return ntStatus;
}

PDEVICE_OBJECT NTAPI KLHookIoAttachDeviceToDeviceStack(
    IN PDEVICE_OBJECT  SourceDevice,
    IN PDEVICE_OBJECT  TargetDevice
    )
{
	PDEVICE_OBJECT RetValue;

	RetValue =  ((f_IoAttachDeviceToDeviceStack)&gIoAttachDeviceToDeviceStack_Splice.m_rFuncCall)(SourceDevice, TargetDevice);

	DbPrint(DC_SYS, DL_IMPORTANT, ("KLHookIoAttachDeviceToDeviceStack: attaching device 0x%08x to device 0x%08x, RetVal=0x%08x\n",
		SourceDevice, TargetDevice, RetValue));

	if (RetValue)
		CommonAttachKbdDevice(SourceDevice, TargetDevice, NULL);

	return RetValue;
}

NTSTATUS NTAPI KLHookIoAttachDeviceToDeviceStackSafe(
    IN PDEVICE_OBJECT  SourceDevice,
    IN PDEVICE_OBJECT  TargetDevice,
    IN OUT PDEVICE_OBJECT  *AttachedToDeviceObject 
    )
{
	NTSTATUS ntStatus;

	ntStatus = ((f_IoAttachDeviceToDeviceStackSafe)&gIoAttachDeviceToDeviceStackSafe_Splice.m_rFuncCall)(SourceDevice, TargetDevice, AttachedToDeviceObject);

	DbPrint(DC_SYS, DL_IMPORTANT, ("KLHookIoAttachDeviceToDeviceStackSafe: attaching device 0x%08x to device 0x%08x, Status=0x%08x\n",
		SourceDevice, TargetDevice, ntStatus));

	if (NT_SUCCESS(ntStatus))
		CommonAttachKbdDevice(SourceDevice, TargetDevice, NULL);

	return ntStatus;
}



// установка сплайсеров..
BOOLEAN SetSplice(IN PVOID OrigFunc, IN PVOID HookFunc, IN ULONG FuncPrologueSize,
				OUT PORIG_FUNC_CALL pOrigFuncCall, OUT PNEARJUMP pOrigFuncPrologue)
{
	NEARJUMP Jump;
	BOOLEAN OldProtection;

	INIT_ORIG_FUNC_CALL((*pOrigFuncCall), pOrigFuncCall, ((PCHAR)OrigFunc+FuncPrologueSize));
	memcpy(&pOrigFuncCall->m_Nops, OrigFunc, FuncPrologueSize);
	memcpy(pOrigFuncPrologue, OrigFunc, sizeof(NEARJUMP));

	INIT_NEARJUMP(Jump, OrigFunc, HookFunc);

	if (MyVirtualProtect(OrigFunc, TRUE, &OldProtection))
	{
// здесь собственно устанавливается сплайс
		memcpy(OrigFunc, &Jump, sizeof(NEARJUMP));
		MyVirtualProtect(OrigFunc, OldProtection, &OldProtection);

		return TRUE;
	}

	return FALSE;
}

BOOLEAN RemoveSplice(IN PVOID OrigFunc, IN PNEARJUMP pOrigFuncPrologue)
{
	BOOLEAN OldProtection;

	if (MyVirtualProtect(OrigFunc, TRUE, &OldProtection))
	{
		memcpy(OrigFunc, pOrigFuncPrologue, sizeof(NEARJUMP));
		MyVirtualProtect(OrigFunc, OldProtection, &OldProtection);

		return TRUE;
	}

	return FALSE;
}

ULONG SelectFuncPrologue(IN PVOID OrigFunc, IN PCHECK_FUNC_PROLOGUE pCheckFuncPrologues, IN ULONG NumVariants)
{
	ULONG i;

	for (i = 0; i < NumVariants; i++)
	{
		if (memcmp(OrigFunc, pCheckFuncPrologues[i].m_Prologue, pCheckFuncPrologues[i].m_PrologueSize) == 0)
			return i;
	}

	return -1L;
}

BOOLEAN SpliceFunc(PVOID OrigFunc, PVOID HookFunc, PSPLICED_FUNC pSplice)
{
	ULONG PrologueVariant;

	PrologueVariant = SelectFuncPrologue(OrigFunc, pSplice->m_CheckFuncPrologues, pSplice->m_NumPrologueVariants);
	if (PrologueVariant != -1L)
	{
		BOOLEAN bResult;

		bResult = SetSplice(OrigFunc, HookFunc, pSplice->m_CheckFuncPrologues[PrologueVariant].m_PrologueSize, 
							&pSplice->m_rFuncCall, &pSplice->m_rFuncPrologue);
		return bResult;
	}

	return FALSE;
}

static PVOID pIoAttachDevice = NULL;
static PVOID pIoAttachDeviceByPointer = NULL;
static PVOID pIoAttachDeviceToDeviceStack = NULL;
static PVOID pIoAttachDeviceToDeviceStackSafe = NULL;

BOOLEAN g_bKbdClassQueried = FALSE;
BOOLEAN g_bIoAttachDevice_Set = FALSE,
		g_bIoAttachDeviceByPointer_Set = FALSE,
		g_bIoAttachDeviceToDeviceStack_Set = FALSE,
		g_bIoAttachDeviceToDeviceStackSafe_Set = FALSE;

VOID RemoveSplicers()
{
	if (g_bIoAttachDevice_Set)
		RemoveSplice(IoAttachDevice, &gIoAttachDevice_Splice.m_rFuncPrologue);

	if (g_bIoAttachDeviceByPointer_Set)
		RemoveSplice(IoAttachDeviceByPointer, &gIoAttachDeviceByPointer_Splice.m_rFuncPrologue);

	if (g_bIoAttachDeviceToDeviceStack_Set)
		RemoveSplice(IoAttachDeviceToDeviceStack, &gIoAttachDeviceToDeviceStack_Splice.m_rFuncPrologue);

	if (g_bIoAttachDeviceToDeviceStackSafe_Set && pIoAttachDeviceToDeviceStackSafe)
		RemoveSplice(pIoAttachDeviceToDeviceStackSafe, &gIoAttachDeviceToDeviceStackSafe_Splice.m_rFuncPrologue);
}

BOOLEAN SetSplicers()
{
	pIoAttachDevice = GetExport(BaseOfNtOsKrnl, "IoAttachDevice", NULL);
	pIoAttachDeviceByPointer = GetExport(BaseOfNtOsKrnl, "IoAttachDeviceByPointer", NULL);
	pIoAttachDeviceToDeviceStack = GetExport(BaseOfNtOsKrnl, "IoAttachDeviceToDeviceStack", NULL);
	pIoAttachDeviceToDeviceStackSafe = GetExport(BaseOfNtOsKrnl, "IoAttachDeviceToDeviceStackSafe", NULL);

// получаем указатель на \Device\KeyboardClass0
	if (!g_bKbdClassQueried)
	{
		NTSTATUS ntStatus;
		UNICODE_STRING cntdKbdClassStr;
		PDEVICE_OBJECT pKbdClassDevice;

		RtlInitUnicodeString(&cntdKbdClassStr, L"\\Device\\KeyboardClass0");
		ntStatus = ReferenceBaseDeviceByName(&cntdKbdClassStr, &pKbdClassDevice);
		if (!NT_SUCCESS(ntStatus))
			return FALSE;

		g_pKbdClassDevice = pKbdClassDevice;
		g_bKbdClassQueried = TRUE;
	}

	do
	{	
		if (pIoAttachDevice)
		{
			g_bIoAttachDevice_Set = SpliceFunc(pIoAttachDevice, KLHookIoAttachDevice, &gIoAttachDevice_Splice);
			if (!g_bIoAttachDevice_Set)
				break;
		}

		if (pIoAttachDeviceByPointer)
		{
			g_bIoAttachDeviceByPointer_Set = SpliceFunc(pIoAttachDeviceByPointer, KLHookIoAttachDeviceByPointer, &gIoAttachDeviceByPointer_Splice);
			if (!g_bIoAttachDeviceByPointer_Set)
				break;
		}

		if (pIoAttachDeviceToDeviceStack)
		{
			g_bIoAttachDeviceToDeviceStack_Set = SpliceFunc(pIoAttachDeviceToDeviceStack, KLHookIoAttachDeviceToDeviceStack,
				&gIoAttachDeviceToDeviceStack_Splice);
			if (!g_bIoAttachDeviceToDeviceStack_Set)
				break;
		}

		if (pIoAttachDeviceToDeviceStackSafe)
		{
			g_bIoAttachDeviceToDeviceStackSafe_Set = SpliceFunc(pIoAttachDeviceToDeviceStackSafe, KLHookIoAttachDeviceToDeviceStackSafe,
				&gIoAttachDeviceToDeviceStackSafe_Splice);
		}
	} while(FALSE);

	if ( !(g_bIoAttachDevice_Set && g_bIoAttachDeviceByPointer_Set && g_bIoAttachDeviceToDeviceStack_Set &&
		(g_bIoAttachDeviceToDeviceStackSafe_Set || *NtBuildNumber < 2600)) )
	{
		RemoveSplicers();
		return FALSE;
	}

	return TRUE;
}
*/

PVOID GetPEBAddress()
{
	NTSTATUS ntStatus;
	PROCESS_BASIC_INFORMATION ProcInfo;
	ULONG RetLen;

	ntStatus = ZwQueryInformationProcess(NtCurrentProcess(), ProcessBasicInformation, &ProcInfo, sizeof(ProcInfo), &RetLen);
	if (NT_SUCCESS(ntStatus))
		return ProcInfo.PebBaseAddress;

	return NULL;
}

// lookup module with known HMODULE in current process
BOOLEAN LookupModuleNameByBase(IN PVOID ModuleBase, OUT PWCHAR FullName OPTIONAL, IN ULONG cbFullName,
							OUT PWCHAR BaseName OPTIONAL, IN ULONG cbBaseName)
{
	PintPEB pPEB;

	pPEB = (PintPEB)GetPEBAddress();
	if (pPEB)
	{
		__try
		{
			PPEB_LDR_DATA pLdrData = pPEB->Ldr;

			if (pLdrData)
			{
				PLDR_MODULE_2 pInMemoryEntry = (PLDR_MODULE_2)pLdrData->InMemoryOrderModuleList.Flink;

				while (pInMemoryEntry && (PLIST_ENTRY)pInMemoryEntry != &pLdrData->InMemoryOrderModuleList)
				{
					if (ModuleBase == pInMemoryEntry->BaseAddress)
					{
						BOOLEAN bResult = FALSE;

						if (FullName)
						{
							if (cbFullName >= pInMemoryEntry->FullDllName.Length+sizeof(WCHAR))
							{
								memcpy(FullName, pInMemoryEntry->FullDllName.Buffer, pInMemoryEntry->FullDllName.Length);
								FullName[pInMemoryEntry->FullDllName.Length/sizeof(WCHAR)] = 0;

								bResult = TRUE;
							}
						}

						if (BaseName)
						{
							if (cbBaseName >= pInMemoryEntry->BaseDllName.Length+sizeof(WCHAR))
							{
								memcpy(BaseName, pInMemoryEntry->BaseDllName.Buffer, pInMemoryEntry->BaseDllName.Length);
								FullName[pInMemoryEntry->BaseDllName.Length/sizeof(WCHAR)] = 0;

								bResult = TRUE;
							}
						}

						return bResult;
					}

					pInMemoryEntry = (PLDR_MODULE_2)pInMemoryEntry->InMemoryOrderModuleList.Flink;
				}
			}
		}
		__except(EXCEPTION_EXECUTE_HANDLER)
		{
		}
	}

	return FALSE;
}

#define ASYNC_KEY_READ_TIMEOUT		1000000L	// 100 msecs
#define ASYNC_KEY_READ_THRESHOLD	20L			// 

VERDICT PreAsyncKeyRead()
{
	HANDLE PID;
	PLIST_ENTRY pList;
	BOOLEAN bItemFound = FALSE;
	LARGE_INTEGER CurrTime;
	BOOLEAN bNeedsEvent = FALSE;
	VERDICT Verdict = Verdict_Default;

	KeQueryTickCount(&CurrTime);
	CurrTime.QuadPart *= KeQueryTimeIncrement();

	PID = PsGetCurrentProcessId();	

	ExAcquireFastMutex(&g_fmAsyncKeyLock);

	pList = gAsyncKeyReaders_Head.Flink;
	while (pList != &gAsyncKeyReaders_Head)
	{
		PASYNC_KEY_READER pListItem = (PASYNC_KEY_READER)pList;

		if (pListItem->m_PID == PID)
		{
			if (CurrTime.QuadPart-pListItem->m_LastReadTime.QuadPart < ASYNC_KEY_READ_TIMEOUT)
				pListItem->m_FastReadCount++;
			else
// если превышен таймаут...
				pListItem->m_FastReadCount = 1;

			pListItem->m_LastReadTime = CurrTime;

			bItemFound = TRUE;

			if (pListItem->m_FastReadCount > ASYNC_KEY_READ_THRESHOLD)
			{
// надо сгенерировать событие..
				bNeedsEvent = TRUE;
// сбрасываем счетчик
				pListItem->m_FastReadCount = 1;
			}
		}
		
		if (CurrTime.QuadPart-pListItem->m_LastReadTime.QuadPart > 100000000L)
		{
			PLIST_ENTRY pOldEntry = pList;

// 10 сек не было активности - удаляем
			RemoveEntryList(pList);
			pList = pList->Blink;
			ExFreeToPagedLookasideList(&g_PLL_AsyncKeyReaders, pOldEntry);
		}

		pList = pList->Flink;
	}

	if (!bItemFound)
	{
		PASYNC_KEY_READER pListItem;
		
		pListItem = (PASYNC_KEY_READER)ExAllocateFromPagedLookasideList(&g_PLL_AsyncKeyReaders);
		if (pListItem)
		{
// новый процесс - потенциальный кейлоггер
			pListItem->m_LastReadTime = CurrTime;
			pListItem->m_FastReadCount = 1;
			pListItem->m_PID = PID;
			InsertTailList(&gAsyncKeyReaders_Head, (PLIST_ENTRY)pListItem);
		}
	}

	ExReleaseFastMutex(&g_fmAsyncKeyLock);

	if (bNeedsEvent)
	{
		HANDLE hForegroundWindow;
		ULONG ForegroundWindowPID;

		hForegroundWindow = NtUserGetForegroundWindow();
		if (hForegroundWindow)
		{
			ULONG ForegroundWindowPID;
// если текущий процесс не имеет foreground-окна			
			ForegroundWindowPID = Get_Window_Owner_PID(hForegroundWindow);
			if (ForegroundWindowPID && ForegroundWindowPID != (ULONG)PsGetCurrentProcessId())
			{
				DbPrint(DC_SYS, DL_SPAM, ("potential keylogger(Get(Async)KeyState) detected, PID=%d\n", PID));

				if (IsNeedFilterEvent(FLTTYPE_SYSTEM, 0x14/*MJ_SYSTEM_KEYLOGGER_DETECTED*/, 0))
				{
					PFILTER_EVENT_PARAM	pParam = (PFILTER_EVENT_PARAM)ExAllocateFromNPagedLookasideList(gpPL0x140);
			
					if (pParam)
					{
						PSINGLE_PARAM pSingleParam;

						FILTER_PARAM_COMMONINIT(pParam, FLTTYPE_SYSTEM, 0x14/*MJ_SYSTEM_KEYLOGGER_DETECTED*/, 0, PostProcessing, 3);
						pSingleParam = (PSINGLE_PARAM)pParam->Params;

						SINGLE_PARAM_INIT_SID(pSingleParam);

						SINGLE_PARAM_SHIFT(pSingleParam);
						SINGLE_PARAM_INIT_ULONG(pSingleParam, _PARAM_OBJECT_KEYLOGGERID, KEYLOGGER_GETASYNCKEYSTATE);

						SINGLE_PARAM_SHIFT(pSingleParam);
						SINGLE_PARAM_INIT_ULONG(pSingleParam, _PARAM_OBJECT_SOURCE_ID, (ULONG)PID);
			
						Verdict = FilterEvent(pParam, NULL);

						ExFreeToNPagedLookasideList(gpPL0x140, pParam);
					}
				}
			}
		}
	}

	return Verdict;
}

SHORT KLHookUserGetAsyncKeyState(IN int vKey)
{
	if (vKey < '0'
		|| vKey > 'Z' 
		|| _PASS_VERDICT(PreAsyncKeyRead()))
	{
		return NtUserGetAsyncKeyState(vKey);
	}

	return 0;
}

SHORT KLHookUserGetKeyState(IN int vKey)
{
	if (vKey < '0'
		|| vKey > 'Z' 
		|| _PASS_VERDICT(PreAsyncKeyRead()))
		return NtUserGetKeyState(vKey);

	return 0;
}

BOOL KLHookUserGetKeyboardState( OUT PBYTE pb )
{
	BOOLEAN bNewItem;
	PVOID pRet;

	pRet = GenCacheGet(
		g_pAttachedInputThreadCache,
		PsGetCurrentThreadId(),
		FALSE,
		&bNewItem
		);
	if ( pRet )
	{
// thread has attached to another thread input
		if ( !_PASS_VERDICT( PreAsyncKeyRead() ) )
			return FALSE;
	}

	return NtUserGetKeyboardState( pb );
}

NTSTATUS NTAPI KLHookSetSecurityObject(
    IN HANDLE Handle,
    IN SECURITY_INFORMATION SecurityInformation,
    IN PSECURITY_DESCRIPTOR SecurityDescriptor
    )
{
	VERDICT Verdict = Verdict_NotFiltered;

	NTSTATUS ntStatus;
	PVOID pObject;

	ntStatus = ObReferenceObjectByHandle(Handle, 0, NULL, KernelMode, &pObject, NULL);
	if (NT_SUCCESS(ntStatus) && pObject)
	{
		UCHAR Type = ((DISPATCHER_HEADER *)pObject)->Type;

		DbPrint(DC_SYS, DL_IMPORTANT, ("KLHookSetSecurityObject - type 0x%x\n", Type ));
		
		if (OBJECT_TYPE_REGKEY == Type
			|| OBJECT_TYPE_FO == Type)
		{
			POBJECT_NAME_INFORMATION pObjInfo;

			pObjInfo = (POBJECT_NAME_INFORMATION)ExAllocateFromNPagedLookasideList(gpPL0x1000);
			if (pObjInfo)
			{
				ULONG RetLen;

				ntStatus = ObQueryNameString(pObject, pObjInfo, 0x1000, &RetLen);
				if (NT_SUCCESS(ntStatus) && pObjInfo->Name.Length && pObjInfo->Name.Buffer)
				{
					PFILTER_EVENT_PARAM pParam;
					PSINGLE_PARAM pSingleParam;
					ULONG Mj = 0; 
					
					if (OBJECT_TYPE_REGKEY == Type)
						Mj = 0x16 ; //MJ_SYSTEM_SETREGSECURITY
					else
						Mj = 0x1d; //MJ_SYSTEM_SETFOSECURITY

					DbPrint(DC_SYS, DL_IMPORTANT, ("KLHookSetSecurityObject - setting security on %S\n",
						pObjInfo->Name.Buffer));

					if (IsNeedFilterEvent(FLTTYPE_SYSTEM, Mj, 0))
					{
						pParam = (PFILTER_EVENT_PARAM)ExAllocateFromNPagedLookasideList(gpPL0x1000);
						if (pParam)
						{
							LUID Luid = {0, 0};

							FILTER_PARAM_COMMONINIT(pParam, FLTTYPE_SYSTEM, Mj, 0, PreProcessing, 2);
							pSingleParam = (PSINGLE_PARAM)pParam->Params;

							SINGLE_PARAM_INIT_SID(pSingleParam);
							
							if (SeGetLuid(&Luid))
							{
								SINGLE_PARAM_SHIFT(pSingleParam);
								SINGLE_PARAM_INIT_NONE(pSingleParam, _PARAM_OBJECT_LUID);
								pSingleParam->ParamSize = sizeof(LUID);
								memcpy(pSingleParam->ParamValue, &Luid, sizeof(LUID));
								pParam->ParamsCount++;
							}

							SINGLE_PARAM_SHIFT(pSingleParam);
							SINGLE_PARAM_INIT_NONE(pSingleParam, _PARAM_OBJECT_URL_W);
							memcpy(pSingleParam->ParamValue, pObjInfo->Name.Buffer, pObjInfo->Name.Length);
							((PCHAR)pSingleParam->ParamValue)[pObjInfo->Name.Length] = 0;
							((PCHAR)pSingleParam->ParamValue)[pObjInfo->Name.Length + 1] = 0;
							pSingleParam->ParamSize = pObjInfo->Name.Length + sizeof(WCHAR);

							Verdict = FilterEvent(pParam, NULL);

							ExFreeToNPagedLookasideList(gpPL0x1000, pParam);
						}
					}
				}

				ExFreeToNPagedLookasideList(gpPL0x1000, pObjInfo);
			}
		}

		ObDereferenceObject(pObject);
	}
	
	if (_PASS_VERDICT(Verdict))
		return NtSetSecurityObject(Handle, SecurityInformation, SecurityDescriptor);

	DbgBreakPoint();
	return STATUS_ACCESS_DENIED;
}

VERDICT CommonUserSendInput(IN PINPUT pInputs, IN ULONG nInputs)
{
	VERDICT Verdict = Verdict_NotFiltered;
	PFILTER_EVENT_PARAM pParam;
	PSINGLE_PARAM pSingleParam;
	ULONG cbParam;
	ULONG i;
	ULONG bInvThreadTarget;

	cbParam = sizeof(INPUT)*nInputs;

	if (!ProbeForReadImp(pInputs, cbParam, 1))
		return Verdict;

	bInvThreadTarget = FALSE;
	for (i = 0; i < nInputs; i++)
	{
		PINPUT pInput = &pInputs[i];

		switch (pInput->type)
		{
		case INPUT_KEYBOARD:
			{
				HANDLE hForegroundWindow = NtUserGetForegroundWindow();

				if (hForegroundWindow)
				{
					HANDLE ForegroundWindowTID = NtUserQueryWindow(hForegroundWindow, WindowThread);

					if (ForegroundWindowTID && IsInvisibleThread((ULONG)ForegroundWindowTID))
						bInvThreadTarget = TRUE;
				}
				break;
			}
		}
	}

	DbPrint(DC_SYS, DL_IMPORTANT, ("send input: source PID=%d, bInvThreadTarget=%d\n",
		PsGetCurrentProcessId(), bInvThreadTarget));

	if (!IsNeedFilterEvent(FLTTYPE_SYSTEM, 0x15/*MJ_SYSTEM_SENDINPUT*/, 0))
		return Verdict_NotFiltered;

	if (cbParam+0x100 > 0x1000)
		return Verdict;

	pParam = (PFILTER_EVENT_PARAM)ExAllocateFromNPagedLookasideList(gpPL0x1000);
	if (!pParam)
		return Verdict;

	FILTER_PARAM_COMMONINIT(pParam, FLTTYPE_SYSTEM, 0x15/*MJ_SYSTEM_SENDINPUT*/, 0, PreProcessing, 4);
	pSingleParam = (PSINGLE_PARAM)pParam->Params;

	SINGLE_PARAM_INIT_SID(pSingleParam);
	SINGLE_PARAM_SHIFT(pSingleParam);

	SINGLE_PARAM_INIT_ULONG(pSingleParam, _PARAM_OBJECT_PARAMCOUNT, nInputs);
	SINGLE_PARAM_SHIFT(pSingleParam);

	SINGLE_PARAM_INIT_NONE(pSingleParam, _PARAM_OBJECT_BINARYDATA);
	memcpy(pSingleParam->ParamValue, pInputs, cbParam);
	pSingleParam->ParamSize = cbParam;
	SINGLE_PARAM_SHIFT(pSingleParam);

	SINGLE_PARAM_INIT_ULONG(pSingleParam, _PARAM_OBJECT_INVIS_TARGET, bInvThreadTarget);

	Verdict = FilterEvent(pParam, NULL);

	ExFreeToNPagedLookasideList(gpPL0x1000, pParam);

	return Verdict;
}

UINT NTAPI KLHookUserSendInput(
    IN UINT    cInputs,
    IN INPUT *pInputs,
    IN int     cbSize)
{
	VERDICT Verdict = Verdict_NotFiltered;
	PINPUT pSafeInputs;

	pSafeInputs = (PINPUT) CopyUserBuffer( pInputs, sizeof(INPUT) * cInputs );
	if ( pSafeInputs )
	{
		Verdict = CommonUserSendInput(pSafeInputs, cInputs);
		ExFreePool( pSafeInputs );
	}
	
	if (_PASS_VERDICT(Verdict))
		return NtUserSendInput(cInputs, pInputs, cbSize);

	return 0;
}

#define WM_SETTEXT                      0x000C

VERDICT PreInterProcMessage(IN HANDLE hWnd OPTIONAL, IN HANDLE TargetProcessID, IN HANDLE TargetThreadID,
							IN ULONG Msg, IN ULONG wParam, IN ULONG lParam)
{
	VERDICT Verdict = Verdict_NotFiltered;
	ULONG bInvisibleTarget;

	if (!TargetThreadID)
		return Verdict_Default;

	bInvisibleTarget = IsInvisibleThread((ULONG)TargetThreadID)?1L:0L;

// костыль на лочку при нескольких сессиях с запущенным ГУЕм
	if (Msg >= 0x400)
		return Verdict_NotFiltered;

	DbPrint(DC_SYS, DL_NOTIFY, ("interprocess message: target hWnd=0x%08X, target PID=%d, target thread ID=%d, source PID=%d, Msg=0x%04X, wParam=0x%08X, lParam= 0x%08X\n",
		hWnd, TargetProcessID, TargetThreadID, PsGetCurrentProcessId(), Msg, wParam, lParam));

	if (IsNeedFilterEvent(FLTTYPE_SYSTEM, 0x17/*MJ_SYSTEM_INTERPROC_MESSAGE*/, 0))
	{
		PFILTER_EVENT_PARAM	pParam = (PFILTER_EVENT_PARAM)ExAllocateFromNPagedLookasideList(gpPL0x4000);
			
		if (pParam)
		{
			PSINGLE_PARAM pSingleParam;

			FILTER_PARAM_COMMONINIT(pParam, FLTTYPE_SYSTEM, 0x17/*MJ_SYSTEM_INTERPROC_MESSAGE*/, 0, PreProcessing, 9);
			pSingleParam = (PSINGLE_PARAM)pParam->Params;

			SINGLE_PARAM_INIT_SID(pSingleParam);

			SINGLE_PARAM_SHIFT(pSingleParam);
			SINGLE_PARAM_INIT_ULONG(pSingleParam, _PARAM_OBJECT_SOURCE_ID, (ULONG)PsGetCurrentProcessId());
			
			SINGLE_PARAM_SHIFT(pSingleParam);
			SINGLE_PARAM_INIT_ULONG(pSingleParam, _PARAM_OBJECT_OBJECTHANDLE, (ULONG)hWnd);

			SINGLE_PARAM_SHIFT(pSingleParam);
			SINGLE_PARAM_INIT_ULONG(pSingleParam, _PARAM_OBJECT_DEST_ID, (ULONG)TargetProcessID);

			SINGLE_PARAM_SHIFT(pSingleParam);
			SINGLE_PARAM_INIT_ULONG(pSingleParam, _PARAM_OBJECT_CLIENTID1, (ULONG)TargetThreadID);

			SINGLE_PARAM_SHIFT(pSingleParam);
			SINGLE_PARAM_INIT_ULONG(pSingleParam, _PARAM_OBJECT_MSG_ID, Msg);

			SINGLE_PARAM_SHIFT(pSingleParam);
			SINGLE_PARAM_INIT_ULONG(pSingleParam, _PARAM_OBJECT_WPARAM, wParam);

			SINGLE_PARAM_SHIFT(pSingleParam);
			SINGLE_PARAM_INIT_ULONG(pSingleParam, _PARAM_OBJECT_LPARAM, lParam);

			SINGLE_PARAM_SHIFT(pSingleParam);
			SINGLE_PARAM_INIT_ULONG(pSingleParam, _PARAM_OBJECT_INVIS_TARGET, bInvisibleTarget);

			if (WM_SETTEXT == Msg && lParam)
			{
				PWCHAR UnicodeText;

				pParam->ParamsCount++;

				SINGLE_PARAM_SHIFT(pSingleParam);
				SINGLE_PARAM_INIT_NONE(pSingleParam, _PARAM_OBJECT_URL_W);

				pSingleParam->ParamSize = sizeof(WCHAR);
				*(PWCHAR)pSingleParam->ParamValue = 0;

				UnicodeText = (PWCHAR)lParam;
				__try
				{
					NTSTATUS ntStatus;

					if (((PCHAR)lParam)[1] != 0)
					{
// ANSI string
						PCHAR AnsiText = (PCHAR)lParam;
						ANSI_STRING cntdAnsiText;
						UNICODE_STRING cntdUnicodeText;

						RtlInitAnsiString(&cntdAnsiText, AnsiText);
						cntdUnicodeText.Buffer = ExAllocatePoolWithTag(PagedPool, 0x1000, 'ipMS');
						UnicodeText = cntdUnicodeText.Buffer;
						if (UnicodeText)
						{
							cntdUnicodeText.Length = 0;
							cntdUnicodeText.MaximumLength = 0x1000-sizeof(WCHAR);
							ntStatus = RtlAnsiStringToUnicodeString(&cntdUnicodeText, &cntdAnsiText, FALSE);
							if (NT_SUCCESS(ntStatus))
								cntdUnicodeText.Buffer[cntdUnicodeText.Length/sizeof(WCHAR)] = 0;
							else
							{
								ExFreePool(UnicodeText);
								UnicodeText = NULL;
							}

						}

					}

					if (UnicodeText)
					{
						ULONG TextLen = wcslen(UnicodeText);
						
						TextLen = min(TextLen, 0x1000);

						pSingleParam->ParamSize = sizeof(WCHAR)*TextLen;
						memcpy(pSingleParam->ParamValue, UnicodeText, pSingleParam->ParamSize);
						((PWCHAR)pSingleParam->ParamValue)[TextLen] = 0;
						pSingleParam->ParamSize += sizeof(WCHAR);
					}

				}
				__except(EXCEPTION_EXECUTE_HANDLER)
				{
				}

				if (UnicodeText && UnicodeText != (PWCHAR)lParam)
					ExFreePool(UnicodeText);
			}

			Verdict = FilterEvent(pParam, NULL);
			
			ExFreeToNPagedLookasideList(gpPL0x4000, pParam);
		}
	}

	return Verdict;
}

//+ DDE stuff ----------------------------------------------------------------------
/* DDE window messages */

#define WM_DDE_FIRST	    0x03E0
#define WM_DDE_INITIATE     (WM_DDE_FIRST)
#define WM_DDE_TERMINATE    (WM_DDE_FIRST+1)
#define WM_DDE_ADVISE	    (WM_DDE_FIRST+2)
#define WM_DDE_UNADVISE     (WM_DDE_FIRST+3)
#define WM_DDE_ACK	        (WM_DDE_FIRST+4)
#define WM_DDE_DATA	        (WM_DDE_FIRST+5)
#define WM_DDE_REQUEST	    (WM_DDE_FIRST+6)
#define WM_DDE_POKE	        (WM_DDE_FIRST+7)
#define WM_DDE_EXECUTE	    (WM_DDE_FIRST+8)
#define WM_DDE_LAST	        (WM_DDE_FIRST+8)


typedef struct
{
	PVOID pMem;
} *_HGLOBAL;

ULONG Get_Window_Owner_PID(IN HANDLE hWnd)
{
	if (NtUserQueryWindow)
		return (ULONG)NtUserQueryWindow(hWnd, WindowProcess);

	return 0;
}

_CRTIMP int    __cdecl mbtowc(wchar_t *, const char *, size_t);

VERDICT PreDdeExecute(IN HANDLE hWnd, IN ULONG lParam)
{
	VERDICT		Verdict = Verdict_NotFiltered;
	_HGLOBAL	hDDE_Command;
	PCHAR		cDDE_Command = NULL;
	PWCHAR		wcDDE_Command = NULL;
	PWCHAR		DDE_Command = NULL;		// converted to Unicode
	ULONG		DDE_Command_Len;
	ULONG		Target_PID;

	do 
	{
		hDDE_Command = (_HGLOBAL)lParam;
		if (!(hDDE_Command && MmIsAddressValid(hDDE_Command) &&
			hDDE_Command->pMem && MmIsAddressValid(hDDE_Command->pMem)))
		{
			break;
		}

		cDDE_Command = hDDE_Command->pMem;
		wcDDE_Command = hDDE_Command->pMem;
		if (cDDE_Command[0] == 0)
		{
			break;
		}
// assuming that there are no one-char commands
// check what version of string we have - ANSI or Unicode
		if (cDDE_Command[1] != 0)
		{
// ANSI version
			PVOID SafeDDECommand = NULL;

			__try
			{
				DDE_Command_Len = strlen(cDDE_Command);
			}
			__except(EXCEPTION_EXECUTE_HANDLER)
			{
				break;
			}


// allocate buffer for converted string
			SafeDDECommand = CopyUserBuffer(cDDE_Command, DDE_Command_Len + 1);
			if ( !SafeDDECommand )
				break;

			DDE_Command = (PWCHAR) ExAllocatePoolWithTag( PagedPool, sizeof(WCHAR) * (DDE_Command_Len + 1), 'klDD');
			if ( !DDE_Command )
			{
				ExFreePool( SafeDDECommand );
				break;
			}

			mbtowc(DDE_Command, SafeDDECommand, DDE_Command_Len + 1);

			ExFreePool( SafeDDECommand );
		}
		else
		{
// Unicode version
			__try
			{
				DDE_Command_Len = wcslen(wcDDE_Command);
			}
			__except(EXCEPTION_EXECUTE_HANDLER)
			{
				break;
			}

			DDE_Command = CopyUserBuffer(wcDDE_Command, sizeof(WCHAR) * (DDE_Command_Len + 1));
		}

		Target_PID = Get_Window_Owner_PID(hWnd);

		if (IsNeedFilterEvent(FLTTYPE_SYSTEM, 0x11/*MJ_SYSTEM_USER_POST_MESSAGE*/, 0))
		{
			BYTE *RequestData;

			RequestData = ExAllocateFromNPagedLookasideList(gpPL0x1000);
			if (RequestData != NULL)
			{
				PFILTER_EVENT_PARAM	pParam = (PFILTER_EVENT_PARAM) RequestData;
				PSINGLE_PARAM pSingleParam;

				PUNICODE_STRING pModule_Name;

				FILTER_PARAM_COMMONINIT(pParam, FLTTYPE_SYSTEM, 0x11/*MJ_SYSTEM_USER_POST_MESSAGE*/, 0, PreProcessing, 4);
				pSingleParam = (PSINGLE_PARAM) pParam->Params;

				SINGLE_PARAM_INIT_SID(pSingleParam);

// target PID--------------------------------------------------------------------------
				SINGLE_PARAM_SHIFT(pSingleParam);
				SINGLE_PARAM_INIT_ULONG(pSingleParam, _PARAM_OBJECT_MSG_ID, WM_DDE_EXECUTE);
// target PID--------------------------------------------------------------------------
				SINGLE_PARAM_SHIFT(pSingleParam);
				SINGLE_PARAM_INIT_ULONG(pSingleParam, _PARAM_OBJECT_CLIENTID1, Target_PID);
// Command ----------------------------------------------------------------------------
				SINGLE_PARAM_SHIFT(pSingleParam);
				SINGLE_PARAM_INIT_NONE(pSingleParam, _PARAM_OBJECT_URL_W);

				memcpy(pSingleParam->ParamValue, DDE_Command, sizeof(WCHAR) * (DDE_Command_Len + 1));
				pSingleParam->ParamSize = sizeof(WCHAR) * (DDE_Command_Len + 1);


				DbPrint(DC_SYS, DL_IMPORTANT, ("PostMessage(WM_DDE_EXECUTE): Pid 0x%x, TargetPid 0x%x, Command = %S \n", PsGetCurrentProcessId(), Target_PID, (WCHAR *)pSingleParam->ParamValue));
//-------------------------------------------------------------------------------------

				Verdict = FilterEvent(pParam, NULL);

				ExFreeToNPagedLookasideList(gpPL0x1000, RequestData);
			}
		}	
	}
	while (FALSE);

// if buffer was allocated
	if ( DDE_Command )
		ExFreePool( DDE_Command );

	return Verdict;

}


PWCHAR GetAtomName(ATOM nAtom)
{
	NTSTATUS ntStatus;
	ULONG RegionSize;
	PATOM_BASIC_INFORMATION pAtomInfo = NULL;
	PWCHAR wcAtomName = NULL;

	RegionSize = 0x1000;;
	ntStatus = ZwAllocateVirtualMemory(NtCurrentProcess(), &pAtomInfo, 0, &RegionSize, MEM_COMMIT, PAGE_READWRITE);
	if (NT_SUCCESS(ntStatus) && pAtomInfo)
	{
		ntStatus = NtQueryInformationAtom(nAtom, AtomBasicInformation, pAtomInfo, 0x1000, NULL);
		if (NT_SUCCESS(ntStatus))
		{
			wcAtomName = (PWCHAR)ExAllocatePoolWithTag(PagedPool, pAtomInfo->NameLength+sizeof(WCHAR), 'grAT');
			if (wcAtomName)
			{
				memcpy(wcAtomName, pAtomInfo->Name, pAtomInfo->NameLength);
				wcAtomName[pAtomInfo->NameLength/sizeof(WCHAR)] = 0;
			}
		}

		ZwFreeVirtualMemory(NtCurrentProcess(), &pAtomInfo, &RegionSize, MEM_RELEASE);
	}

	return wcAtomName;
}

VERDICT PreDdeInitiate(IN HANDLE hWnd, IN ULONG lParam)
{
	VERDICT Verdict = Verdict_NotFiltered;
	ATOM aApplication, aTopic;

	aApplication = (USHORT)lParam;
	aTopic = (USHORT)(lParam >> 16);

// certain application
	if (aApplication)
	{
		PWCHAR wcAppName = GetAtomName(aApplication);
		if (wcAppName)
		{
			PWCHAR wcTopicName;
			ULONG TargetPID;

			wcTopicName = GetAtomName(aTopic);

			TargetPID = Get_Window_Owner_PID(hWnd);

			if (IsNeedFilterEvent(FLTTYPE_SYSTEM, 0x11/*MJ_SYSTEM_USER_POST_MESSAGE*/, 0))
			{
				PFILTER_EVENT_PARAM pParam;

				pParam = (PFILTER_EVENT_PARAM)ExAllocateFromNPagedLookasideList(gpPL0x1000);
				if (pParam)
				{
					PSINGLE_PARAM pSingleParam;

					RtlZeroMemory(pParam, sizeof(*pParam));	
					FILTER_PARAM_COMMONINIT(pParam, FLTTYPE_SYSTEM, 0x11/*MJ_SYSTEM_USER_POST_MESSAGE*/, 0, PreProcessing, 4);
					pSingleParam = (PSINGLE_PARAM) pParam->Params;

// sid --------------------------------------------------------------------------------
					SINGLE_PARAM_INIT_SID(pSingleParam);
					SINGLE_PARAM_SHIFT(pSingleParam);
// msg --------------------------------------------------------------------------------
					SINGLE_PARAM_INIT_ULONG(pSingleParam, _PARAM_OBJECT_MSG_ID, WM_DDE_INITIATE);
					SINGLE_PARAM_SHIFT(pSingleParam);
// target PID--------------------------------------------------------------------------
					SINGLE_PARAM_INIT_ULONG(pSingleParam, _PARAM_OBJECT_CLIENTID1, TargetPID);
					SINGLE_PARAM_SHIFT(pSingleParam);
// service name------------------------------------------------------------------------
					SINGLE_PARAM_INIT_NONE(pSingleParam, _PARAM_OBJECT_URL_W);
					wcscpy((PWCHAR)pSingleParam->ParamValue, wcAppName);
					pSingleParam->ParamSize = sizeof(WCHAR)*(wcslen(wcAppName)+1);
					SINGLE_PARAM_SHIFT(pSingleParam);
// topic name--------------------------------------------------------------------------
					if (wcTopicName)
					{
						SINGLE_PARAM_INIT_NONE(pSingleParam, _PARAM_OBJECT_URL_PARAM_W);
						wcscpy((PWCHAR)pSingleParam->ParamValue, wcTopicName);
						pSingleParam->ParamSize = sizeof(WCHAR)*(wcslen(wcTopicName)+1);
						SINGLE_PARAM_SHIFT(pSingleParam);
						pParam->ParamsCount++;
					}
//-------------------------------------------------------------------------------------

					Verdict = FilterEvent(pParam, NULL);

					ExFreeToNPagedLookasideList(gpPL0x1000, pParam);
				}
			}	

			DbPrint(DC_SYS, DL_IMPORTANT, ("PostMessage(WM_DDE_INITIATE): Pid 0x%x, TargetPid 0x%x, service = %S topic = %S, verdict = %d\n",
				PsGetCurrentProcessId(),
				TargetPID,
				wcAppName,
				wcTopicName,
				Verdict
				));

			if (wcTopicName)
				ExFreePool(wcTopicName);

			ExFreePool(wcAppName);
		}
	}

	return Verdict;
}
//- DDE stuff ----------------------------------------------------------------------

// обработчик на SendMessage/PostMessage
VERDICT PreSendPostMessage(IN HANDLE hWnd, IN ULONG Msg, IN ULONG wParam, IN ULONG lParam)
{
	HANDLE TargetProcessID;
	HANDLE TargetThreadID;
	VERDICT Verdict = Verdict_Default;

	TargetProcessID = NtUserQueryWindow(hWnd, WindowProcess);
// если посылаем в чужой процесс..
	if (TargetProcessID && TargetProcessID != PsGetCurrentProcessId())
	{
		TargetThreadID = NtUserQueryWindow(hWnd, WindowThread);
		Verdict = PreInterProcMessage(hWnd, TargetProcessID, TargetThreadID, Msg, wParam, lParam);
	}

	if (_PASS_VERDICT(Verdict))
	{
		switch (Msg)
		{
		case WM_DDE_EXECUTE:
			Verdict = PreDdeExecute(hWnd, lParam);
			break;
		case WM_DDE_INITIATE:
			Verdict = PreDdeInitiate(hWnd, lParam);
			break;
		}
	}

	return Verdict;
}
//+ Gruzdev -----------------------------------------------------------------------------------------
#define THREAD_QUERY_INFORMATION       (0x0040) 

NTSYSAPI
NTSTATUS
NTAPI
ZwOpenThread(
	OUT PHANDLE ThreadHandle,
	IN ACCESS_MASK DesiredAccess,
	IN POBJECT_ATTRIBUTES ObjectAttributes,
	IN PCLIENT_ID ClientId
);
//- Gruzdev -----------------------------------------------------------------------------------------
VERDICT PrePostThreadMessage(IN ULONG ThreadID, IN ULONG Msg, IN ULONG wParam, IN ULONG lParam)
{
	VERDICT Verdict = Verdict_Default;
	HANDLE hTargetThread;
	OBJECT_ATTRIBUTES ThreadOA;
	CLIENT_ID CID;
	NTSTATUS ntStatus;
	PTHREAD_BASIC_INFORMATION pThreadInfo;
	ULONG RegionSize;

	InitializeObjectAttributes(&ThreadOA, NULL, 0, NULL, NULL);
	CID.UniqueProcess = 0;
	CID.UniqueThread = (HANDLE)ThreadID;
	ntStatus = ZwOpenThread(&hTargetThread, THREAD_QUERY_INFORMATION, &ThreadOA, &CID);
	if (!NT_SUCCESS(ntStatus))
	{
		DbPrint(DC_SYS, DL_IMPORTANT, ("PrePostThreadMessage: failed to open thread CID=%d, ntStatus=0x%08X\n",
			ThreadID, ntStatus));
		return Verdict_Default;
	}

// выделяем юзеровую память, потомушта PreviousMode будет == UserMode
	pThreadInfo = (PTHREAD_BASIC_INFORMATION)NULL;
	RegionSize = sizeof(THREAD_BASIC_INFORMATION);
	ntStatus = ZwAllocateVirtualMemory(NtCurrentProcess(), &pThreadInfo, 0, &RegionSize, MEM_COMMIT, PAGE_READWRITE);
	if (NT_SUCCESS(ntStatus))
	{
// получаем ID процесса с этой ниткой..
		ntStatus = NtQueryInformationThread(hTargetThread, ThreadBasicInformation, pThreadInfo,
			sizeof(THREAD_BASIC_INFORMATION), NULL);
		if (NT_SUCCESS(ntStatus))
		{
			HANDLE TargetProcessID;

			TargetProcessID = pThreadInfo->ClientId.UniqueProcess;
// если посылаем в чужой процесс..
			if (TargetProcessID && TargetProcessID != PsGetCurrentProcessId())
				Verdict = PreInterProcMessage(NULL, TargetProcessID, (HANDLE)ThreadID, Msg, wParam, lParam);
		}
		else
			DbPrint(DC_SYS, DL_IMPORTANT, ("PrePostThreadMessage: failed to query thread information, CID=%d, ntStatus=0x%08X\n",
			ThreadID, ntStatus));

		RegionSize = 0;
		ZwFreeVirtualMemory(NtCurrentProcess(), &pThreadInfo, &RegionSize, MEM_RELEASE);
	}
	else
		DbPrint(DC_SYS, DL_IMPORTANT, ("PrePostThreadMessage: failed to allocate virtual memory for PID=%d, ntStatus=0x%08X\n",
			PsGetCurrentProcessId(), ntStatus));


	ZwClose(hTargetThread);

	return Verdict;
}

ULONG NTAPI KLHookUserMessageCall(
    IN HANDLE hwnd,
    IN ULONG msg,
    IN ULONG wParam,
    IN ULONG lParam,
    IN PVOID xParam,
    IN ULONG xpfnProc,
    IN ULONG bAnsi)
{
	ULONG RetVal = 0;
	VERDICT Verdict;

// вердикт на посылку межпроцессного сообщения на окно (send и post)
	Verdict = PreSendPostMessage(hwnd, msg, wParam, lParam);
	if (!_PASS_VERDICT(Verdict))
		return 0;

	return NtUserMessageCall(hwnd, msg, wParam, lParam, xParam, xpfnProc, bAnsi);
}

ULONG KLHookUserSendNotifyMessage(
    IN HANDLE hwnd,
    IN ULONG msg,
    IN ULONG wParam,
    IN ULONG lParam OPTIONAL)
{
	ULONG RetVal = 0;
	VERDICT Verdict;

// вердикт на посылку межпроцессного сообщения на окно (send и post)
	Verdict = PreSendPostMessage(hwnd, msg, wParam, lParam);
	if (!_PASS_VERDICT(Verdict))
		return 0;

	return NtUserSendNotifyMessage(hwnd, msg, wParam, lParam);
}

ULONG NTAPI KLHookUserPostThreadMessage(
    IN ULONG id,
    IN ULONG msg,
    IN ULONG wParam,
    IN ULONG lParam)
{
	VERDICT Verdict;

	Verdict = PrePostThreadMessage(id, msg, wParam, lParam);

	if (_PASS_VERDICT(Verdict))
		return NtUserPostThreadMessage(id, msg, wParam, lParam);

	return 0;
}

HANDLE KLHookUserFindWindowEx(
    IN HANDLE hwndParent,
    IN HANDLE hwndChild,
    IN PUNICODE_STRING pstrClassName,
    IN PUNICODE_STRING pstrWindowName,
	IN ULONG dwType)
{
	VERDICT Verdict = Verdict_NotFiltered;
	HANDLE hResult = NULL;
	BOOLEAN bNeedsEvent = FALSE;

	if (pstrClassName || pstrWindowName)
	{
		if ( bNeedsEvent = IsNeedFilterEvent(FLTTYPE_SYSTEM, 0x18/*MJ_SYSTEM_FINDWINDOW*/, 0) )
		{
			PFILTER_EVENT_PARAM	pParam = (PFILTER_EVENT_PARAM)ExAllocateFromNPagedLookasideList(gpPL0x1000);
			ULONG cbSingleParam;
			
			if (pParam)
			{
				PSINGLE_PARAM pSingleParam;
				PWCHAR ClassName = NULL;
				PWCHAR WindowName = NULL;

				FILTER_PARAM_COMMONINIT(pParam, FLTTYPE_SYSTEM, 0x18/*MJ_SYSTEM_MJ_SYSTEM_FINDWINDOW*/, 0, PreProcessing, 4);
				pSingleParam = (PSINGLE_PARAM)pParam->Params;

				SINGLE_PARAM_INIT_SID(pSingleParam);
				SINGLE_PARAM_SHIFT(pSingleParam);

				SINGLE_PARAM_INIT_ULONG(pSingleParam, _PARAM_OBJECT_SOURCE_ID, (ULONG)PsGetCurrentProcessId());
				SINGLE_PARAM_SHIFT(pSingleParam);
			
				SINGLE_PARAM_INIT_NONE(pSingleParam, _PARAM_OBJECT_URL_W);
				cbSingleParam = 0;
				__try
				{
					if ( pstrClassName && MmIsAddressValid(pstrClassName) &&
						pstrClassName->Buffer && MmIsAddressValid(pstrClassName->Buffer) &&
						pstrClassName->Length != 0)
					{
						PVOID pSafeCopy;

						pSafeCopy = CopyUserBuffer( pstrClassName->Buffer, pstrClassName->Length);
						if ( pSafeCopy )
						{
							cbSingleParam = pstrClassName->Length;
							memcpy(pSingleParam->ParamValue, pSafeCopy, cbSingleParam);

							ExFreePool( pSafeCopy );
						}
					}
				}
				__except(EXCEPTION_EXECUTE_HANDLER)
				{
					cbSingleParam = 0;
				}
				((PWCHAR)pSingleParam->ParamValue)[cbSingleParam / sizeof(WCHAR)] = 0;
				pSingleParam->ParamSize = cbSingleParam + sizeof(WCHAR);
				ClassName = (PWCHAR)pSingleParam->ParamValue;
				SINGLE_PARAM_SHIFT(pSingleParam);


				SINGLE_PARAM_INIT_NONE(pSingleParam, _PARAM_OBJECT_URL_PARAM_W);
				cbSingleParam = 0;
				__try
				{
					if (pstrWindowName && MmIsAddressValid( pstrWindowName ) &&
						pstrWindowName->Buffer && MmIsAddressValid( pstrWindowName->Buffer ) &&
						pstrWindowName->Length != 0)
					{
						PVOID pSafeCopy;

						pSafeCopy = CopyUserBuffer( pstrWindowName->Buffer, pstrWindowName->Length );
						if (pSafeCopy)
						{
							cbSingleParam = pstrWindowName->Length;
							memcpy(pSingleParam->ParamValue, pstrWindowName->Buffer, cbSingleParam);

							ExFreePool( pSafeCopy );
						}
					}
				}
				__except(EXCEPTION_EXECUTE_HANDLER)
				{
					cbSingleParam = 0;
				}
				((PWCHAR)pSingleParam->ParamValue)[cbSingleParam / sizeof(WCHAR)] = 0;
				pSingleParam->ParamSize = cbSingleParam + sizeof(WCHAR);
				WindowName = (PWCHAR)pSingleParam->ParamValue;

				Verdict = FilterEvent(pParam, NULL);

				DbPrint(DC_SYS, DL_IMPORTANT, ("process Cid=%d trying to FindWindow: ClassName=%S WindowName=%S\n",
					PsGetCurrentProcessId(), ClassName, WindowName));

				ExFreeToNPagedLookasideList(gpPL0x1000, pParam);
			}
		}
	}

	if (!_PASS_VERDICT(Verdict))
		return NULL;

	hResult = NtUserFindWindowEx(hwndParent, hwndChild, pstrClassName, pstrWindowName, dwType);

	if ( bNeedsEvent )
	{
		PFILTER_EVENT_PARAM	pParam = (PFILTER_EVENT_PARAM)ExAllocateFromNPagedLookasideList(gpPL0x140);
		
		if (pParam)
		{
			PSINGLE_PARAM pSingleParam;

			FILTER_PARAM_COMMONINIT(pParam, FLTTYPE_SYSTEM, 0x18/*MJ_SYSTEM_MJ_SYSTEM_FINDWINDOW*/, 0, PostProcessing, 2);
			pSingleParam = (PSINGLE_PARAM)pParam->Params;

			SINGLE_PARAM_INIT_SID(pSingleParam);
			SINGLE_PARAM_SHIFT(pSingleParam);

			SINGLE_PARAM_INIT_ULONG(pSingleParam, _PARAM_OBJECT_OBJECTHANDLE, (ULONG)hResult);
			SINGLE_PARAM_SHIFT(pSingleParam);

			if (hResult)
			{
				ULONG bInvThreadTarget = 0;
				HANDLE TargetTid = NtUserQueryWindow(hResult, WindowThread);
				if (TargetTid && IsInvisibleThread((ULONG)TargetTid))
					bInvThreadTarget = TRUE;

				SINGLE_PARAM_INIT_ULONG(pSingleParam, _PARAM_OBJECT_INVIS_TARGET, bInvThreadTarget);
				SINGLE_PARAM_SHIFT(pSingleParam);
				pParam->ParamsCount++;
			}

			Verdict =  FilterEvent(pParam, NULL);

			ExFreeToNPagedLookasideList(gpPL0x140, pParam);
		}
	}

	if ( !_PASS_VERDICT(Verdict) )
		return NULL;

	return hResult;
}

NTSTATUS
NTAPI KLHookLoadDriver(
    IN PUNICODE_STRING DriverServiceName
					   )
{
	NTSTATUS StatusRet = STATUS_NOT_FOUND;
	VERDICT Verdict = Verdict_NotFiltered;

	__try
	{
		if (DriverServiceName && MmIsAddressValid(DriverServiceName) &&
			DriverServiceName->Buffer && MmIsAddressValid(DriverServiceName->Buffer) &&
			DriverServiceName->Length)
		{
			PWCHAR SafeServiceName;

			SafeServiceName = (PWCHAR) CopyUserBuffer(DriverServiceName->Buffer, DriverServiceName->Length);
			if ( SafeServiceName )
			{
				if (IsNeedFilterEvent(FLTTYPE_SYSTEM, 0x1a/*MJ_SYSTEM_LOADDRIVER*/, 0))
				{
					PFILTER_EVENT_PARAM	pParam = (PFILTER_EVENT_PARAM)ExAllocateFromNPagedLookasideList(gpPL0x1000);
					
					if (pParam)
					{
						PSINGLE_PARAM pSingleParam;
						ULONG cbCopy;

						RtlZeroMemory(pParam, 0x1000);
						FILTER_PARAM_COMMONINIT(pParam, FLTTYPE_SYSTEM, 0x1a/*MJ_SYSTEM_LOADDRIVER*/, 0, PreProcessing, 2);

						pSingleParam = (PSINGLE_PARAM)pParam->Params;

						SINGLE_PARAM_INIT_SID(pSingleParam);
						SINGLE_PARAM_SHIFT(pSingleParam);

						SINGLE_PARAM_INIT_NONE(pSingleParam, _PARAM_OBJECT_URL_W);
						cbCopy = min(0x500, DriverServiceName->Length);
						pSingleParam->ParamSize = cbCopy+sizeof(WCHAR);
						memcpy(pSingleParam->ParamValue, SafeServiceName, cbCopy);
						((PWCHAR)pSingleParam->ParamValue)[cbCopy/sizeof(WCHAR)] = 0;
						SINGLE_PARAM_SHIFT(pSingleParam);

						Verdict = FilterEvent(pParam, NULL);

						ExFreeToNPagedLookasideList(gpPL0x1000, pParam);
					}
				}

				ExFreePool( SafeServiceName );
			}


		}
	}
	__except(EXCEPTION_EXECUTE_HANDLER)
	{
	}

	if (_PASS_VERDICT(Verdict))
		StatusRet = NtLoadDriver(DriverServiceName);

	return StatusRet;
		
}

BOOL NTAPI KLHookBitBlt(
    HDC    hdcDst,
    int    x,
    int    y,
    int    cx,
    int    cy,
    HDC    hdcSrc,
    int    xSrc,
    int    ySrc,
    DWORD  rop4,
    DWORD  crBackColor,
	DWORD  Unknown
)
{
	VERDICT Verdict = Verdict_NotFiltered;

	if (IsNeedFilterEvent(FLTTYPE_SYSTEM, 0x1b/*MJ_SYSTEM_BITBLT*/, 0))
	{
		PFILTER_EVENT_PARAM	pParam = (PFILTER_EVENT_PARAM) ExAllocateFromNPagedLookasideList(gpPL0x140);
		
		if (pParam)
		{
			PSINGLE_PARAM pSingleParam;

			RtlZeroMemory(pParam, 0x140);

			FILTER_PARAM_COMMONINIT(pParam, FLTTYPE_SYSTEM, 0x1b/*MJ_SYSTEM_BITBLT*/, 0, PreProcessing, 1);

			pSingleParam = (PSINGLE_PARAM)pParam->Params;

			SINGLE_PARAM_INIT_SID(pSingleParam);
			SINGLE_PARAM_SHIFT(pSingleParam);

			Verdict = FilterEvent(pParam, NULL);

			ExFreeToNPagedLookasideList(gpPL0x140, pParam);
		}	

	}

	if (_PASS_VERDICT(Verdict))
		return NtGdiBitBlt(
			hdcDst,
			x,
			y,
			cx,
			cy,
			hdcSrc,
			xSrc,
			ySrc,
			rop4,
			crBackColor,
			Unknown
		);

	return FALSE;
}

NTSTATUS
NTAPI
KLHookSystemDebugControl(
	IN DEBUG_CONTROL_CODE ControlCode,
	IN PVOID InputBuffer OPTIONAL,
	IN ULONG InputBufferLength,
	OUT PVOID OutputBuffer OPTIONAL,
	IN ULONG OutputBufferLength,
	OUT PULONG ReturnLength OPTIONAL
)
{
	VERDICT Verdict = Verdict_NotFiltered;

	if (IsNeedFilterEvent(FLTTYPE_SYSTEM, 0x1c/*MJ_SYSTEM_DEBUGCONTROL*/, 0))
	{
		PFILTER_EVENT_PARAM	pParam = (PFILTER_EVENT_PARAM) ExAllocateFromNPagedLookasideList(gpPL0x140);
		
		if (pParam)
		{
			PSINGLE_PARAM pSingleParam;

			RtlZeroMemory(pParam, 0x140);

			FILTER_PARAM_COMMONINIT(pParam, FLTTYPE_SYSTEM, 0x1c/*MJ_SYSTEM_DEBUGCONTROL*/, 0, PreProcessing, 2);

			pSingleParam = (PSINGLE_PARAM)pParam->Params;

			SINGLE_PARAM_INIT_SID(pSingleParam);
			SINGLE_PARAM_SHIFT(pSingleParam);

			SINGLE_PARAM_INIT_ULONG(pSingleParam, _PARAM_OBJECT_INTERNALID, ControlCode);
			SINGLE_PARAM_SHIFT(pSingleParam);

			Verdict = FilterEvent(pParam, NULL);

			ExFreeToNPagedLookasideList(gpPL0x140, pParam);
		}	

	}

	DbPrint(DC_SYS, DL_IMPORTANT, ("process Cid=%d is trying to ZwSystemDebugControl: ControlCode=%d, Verdict=%d\n",
		PsGetCurrentProcessId(), ControlCode, Verdict));

	if ( _PASS_VERDICT(Verdict) )
		return NtSystemDebugControl(
			ControlCode,
			InputBuffer,
			InputBufferLength,
			OutputBuffer,
			OutputBufferLength,
			ReturnLength
		);

	return STATUS_ACCESS_DENIED;
}

/*
NTSTATUS
NTAPI KLHookConnectPort (
						OUT PHANDLE PortHandle,
						IN PUNICODE_STRING PortName,
						IN PSECURITY_QUALITY_OF_SERVICE SecurityQos,
						IN OUT PPORT_VIEW ClientView OPTIONAL,
						IN OUT PREMOTE_PORT_VIEW ServerView OPTIONAL,
						OUT PULONG MaxMessageLength OPTIONAL,
						IN OUT PVOID ConnectionInformation OPTIONAL,
						IN OUT PULONG ConnectionInformationLength OPTIONAL
						)
{
	NTSTATUS ntStatus;

	ntStatus = NtConnectPort(
		PortHandle,
		PortName,
		SecurityQos,
		ClientView,
		ServerView,
		MaxMessageLength,
		ConnectionInformation,
		ConnectionInformationLength
		);
	if ( NT_SUCCESS( ntStatus ) )
	{
		UNICODE_STRING usPortMapper;
		PUNICODE_STRING p_usSafePortName;

		RtlInitUnicodeString(
			&usPortMapper,
			RPC_MAPPER_PORT
			);

		p_usSafePortName = GetUserUniStr( PortName );
		if ( p_usSafePortName )
		{
			LONG lResult;


			lResult = RtlCompareUnicodeString(
				&usPortMapper,
				p_usSafePortName,
				TRUE
				);
			if ( 0 == lResult )
			{
				// connection to RPC port mapper here

				if ( PortHandle )
				{
					PHANDLE p_hPort = CopyUserBuffer(
						PortHandle,
						sizeof( *PortHandle )
						);
					if ( p_hPort )
					{
						HANDLE hPort = *p_hPort;
						BOOLEAN bNewItem;

						GenCacheGet(
							g_pRPCMapperPortsCache,
							hPort,
							TRUE,
							&bNewItem
							);
						if ( !bNewItem )
							DbPrint( DC_SYS, DL_ERROR, ( "error: ununique LPC port handles\n" ) );

						ExFreePool( p_hPort );
					}
				}
			}

			ExFreePool( p_usSafePortName );
		}
	}

	return ntStatus;
}

// CoCreateObject on LOCAL_SERVER through LPC
VERDICT CommonLPCCreateObject(
							  IN PPORT_MESSAGE RequestMessage
							  )
{
#define LPC_MAX_DATA_SIZE_OLE_INTEREST 0x240
#define LPC_OLE_MAGIC_4_4 0x00040002
#define LPC_OLE_MAGIC_2_4 LPC_OLE_MAGIC_4_4
#define LPC_OLE_MAGIC_4_4_MASK 0xFFFF00FF
#define LPC_OLE_MAGIC_2_4_MASK LPC_OLE_MAGIC_4_4_MASK
#define LPC_OLE_MAGIC_STACK_0_4_W2K 0x00060005
#define LPC_OLE_MAGIC_STACK_0_4_XPVISTA 0x00070005
	typedef struct LPC_OLE_DATA_STACK
	{
		ULONG magic1; // 0x00
		ULONG unknown1[8]; // 0x04
		ULONG thread_id; // 0x24
		ULONG unknown2[23]; // 0x28
		ULONG offset; // 0x84
		ULONG unknown3[22]; // 0x88
		UCHAR data[ANYSIZE_ARRAY]; // 0xE0
	} LPC_OLE_DATA_STACK,*PLPC_OLE_DATA_STACK;
	typedef struct LPC_OLE_DATA
	{
		ULONG unknown; // 0x00
		PLPC_OLE_DATA_STACK stack; // 0x04
		ULONG stack_size; // 0x08
	} LPC_OLE_DATA,*PLPC_OLE_DATA;

	// a) the message must have some minimum size
	if ( RequestMessage->u1.s1.TotalLength >= sizeof( PORT_MESSAGE ) )
	{
		PUCHAR pData;
		ULONG DataSize;
		ULONG Data_2_4;
		ULONG Data_4_4;

		DataSize = min( RequestMessage->u1.s1.DataLength, LPC_MAX_DATA_SIZE_OLE_INTEREST );
		pData = CopyUserBuffer( RequestMessage + 1, DataSize );
		if ( pData )
		{
			// Windows 2000 SP4 & Windows XP SP2
			Data_2_4 = * (PULONG) ( pData + 2 );
			Data_4_4 = * (PULONG) ( pData + 4 );
		}

		// b) VirtualRangesOffset must be non-zero and must not point too far in the data part
		if ( RequestMessage->u2.s2.DataInfoOffset &&
			 sizeof( PORT_MESSAGE ) + RequestMessage->u2.s2.DataInfoOffset + sizeof() <= RequestMessage->u1.s1.TotalLength )
			)
		{
			// c) message magics (type) must match, W2k magic starts at offset 2 in data,
			// WXP magic starts at offset 4 in data
			if ( 
				 ( ( 2195 == *NtBuildNumber )&&
				   ( LPC_OLE_MAGIC_2_4 == ( Data_2_4 & LPC_OLE_MAGIC_2_4_MASK ) )
				 ) ||
				 ( ( 2600 == *NtBuildNumber ) &&
				   ( LPC_OLE_MAGIC_4_4 == ( Data_4_4 & LPC_OLE_MAGIC_4_4_MASK ) )
				 )

			{
				// message was recognized, interesting data start at VirtualRangesOffset
				PLPC_OLE_DATA packet=(PVOID) &Data[RequestMessage->u2.s2.DataInfoOffset];

				// there is a pointer to "real data" stored inside caller's process memory
				// d) the size of "real data" must be at least sizeof(LPC_OLE_DATA_STACK)
				if (packet->stack_size>=sizeof(LPC_OLE_DATA_STACK))
				{
					PLPC_OLE_DATA_STACK remote_stack=packet->stack;
					LPC_OLE_DATA_STACK stack;
					memcpy(&stack,remote_stack,sizeof(stack));
					// e) "real data" magics must match and the GUID offset must point inside the data
					if ((((osversion==OS_WINDOWS_2000_SP4)
						&& (stack.magic1==LPC_OLE_MAGIC_STACK_0_4_W2K))
						|| ((osversion>OS_WINDOWS_2000_SP4)
						&& (stack.magic1==LPC_OLE_MAGIC_STACK_0_4_XPVISTA)))
						&& (stack.offset+FIELD_OFFSET(LPC_OLE_DATA_STACK,offset)+sizeof(GUID)
						< packet->stack_size))
					{
						GUID clsid;
						memcpy(&clsid,&stack.data[stack.offset],sizeof(clsid));
						if (!verify_access_ole(&clsid))
						{
							call_org=FALSE;
							status=STATUS_ACCESS_DENIED;
						}
					}
				}
			}
		}
}


NTSTATUS
NTAPI KLHookRequestWaitReplyPort (
								 IN HANDLE PortHandle,
								 IN PPORT_MESSAGE RequestMessage,
								 OUT PPORT_MESSAGE ReplyMessage
								 )
{
	PVOID pData;
	BOOLEAN bCreated;
	VERDICT Verdict = Verdict_NotFiltered;

	pData = GenCacheGet(
		g_pRPCMapperPortsCache,
		PortHandle,
		FALSE,
		&bCreated
		);
	if ( pData )
	{
		// this is rpc mapper port
		Verdict = CommonLPCCreateObject(
			RequestMessage
			);
	}

	return NtRequestWaitReplyPort(
		PortHandle,
		RequestMessage,
		ReplyMessage
		);
}
*/

// maintain cache of threads that attached their inputs to other threads..
BOOL NTAPI KLHookUserAttachThreadInput (
								  IN DWORD idAttach,
								  IN DWORD idAttachTo,
								  IN BOOL fAttach
								  )
{
	BOOL bResult;

	bResult = NtUserAttachThreadInput(
		idAttach,
		idAttachTo,
		fAttach
		);
	if ( fAttach && bResult && idAttach != idAttachTo )
	{
		BOOLEAN bNewItem;

		GenCacheGet(
			g_pAttachedInputThreadCache,
			(HANDLE) idAttach,
			TRUE,
			&bNewItem
			);
	}

	return bResult;		
}

//- Gruzdev -----------------------------------------------------------------------------------

void
StoreDriverName (
	PUNICODE_STRING pFullImageName
	)
{
	md5_byte_t digest[16];
	md5_state_t md5st;
	DWORD* ptmpd;

	BOOLEAN bSkip = FALSE;
	PIMPLIST_ENTRY Flink;
	PDRVNAME_LISTITEM pDrvNameTmp;

	if (gDrvInList > _MaxDrvInList)
		return;

	md5_init(&md5st);
	md5_append(&md5st, (md5_byte_t*) pFullImageName->Buffer, pFullImageName->Length);
	md5_finish(&md5st, digest);
	ptmpd = (DWORD*) digest;
	ptmpd[0] ^= ptmpd[2];
	ptmpd[1] ^= ptmpd[3];

	ExAcquireFastMutex(&g_fmInternalSync);

	if (!IsListEmpty(&gDrvNamesList))
	{
		Flink = gDrvNamesList.Flink;
	
		while(Flink != &gDrvNamesList)
		{
			pDrvNameTmp = CONTAINING_RECORD(Flink, DRVNAME_LISTITEM, m_List);
			
			Flink = Flink->Flink;
			
			if ((pDrvNameTmp->m_Hash == *ptmpd))
			{
				bSkip = TRUE;
				break;
			}
		}
	}
	
	if (!bSkip)
	{
		DWORD len = sizeof(DRVNAME_LISTITEM) + pFullImageName->Length;
		if (pFullImageName->Buffer[pFullImageName->Length / sizeof(WCHAR) - 1])
			len += sizeof(WCHAR);
		else
		{
			DbgBreakPoint();
		}

		pDrvNameTmp = ExAllocatePoolWithTag(PagedPool, len, 'DBoS');
		if (pDrvNameTmp)
		{
#ifdef __DBG__
			memset(pDrvNameTmp, 'fe', len);
#endif
			pDrvNameTmp->m_DrvName = (PWCHAR)(pDrvNameTmp + 1);
			memcpy(pDrvNameTmp->m_DrvName, pFullImageName->Buffer, pFullImageName->Length);
			((char*)pDrvNameTmp)[len - 1] = 0;
			((char*)pDrvNameTmp)[len - 2] = 0;

			pDrvNameTmp->m_Hash = *ptmpd;

			_impInsertTailList(&gDrvNamesList, &pDrvNameTmp->m_List);

			gDrvInList++;
		}
	}
	
	ExReleaseFastMutex(&g_fmInternalSync);
}

LARGE_INTEGER	*gpExecuteHashes = NULL;
LARGE_INTEGER	gExecuteLastHash;
ULONG			gExecutePos = 0;
#define			_execute_max_count 4000

void StoreExecuteName(PWCHAR pwchName)
{
	PWCHAR pwchNameImp = NULL;
	LARGE_INTEGER hash;
	md5_byte_t digest[16];
	md5_state_t md5st;

	DWORD* tmpd;

	int len = (wcslen(pwchName) + 1) * sizeof(WCHAR);
	
	if (!len)
		return;

	pwchNameImp = ExAllocatePoolWithTag(PagedPool, len, 'sBOs');
	if (!pwchNameImp)
		return;

	memcpy(pwchNameImp, pwchName, len);
	_UpperStringZeroTerninatedW(pwchNameImp, pwchNameImp);

	md5_init(&md5st);
	md5_append(&md5st, (md5_byte_t*) pwchNameImp, len - sizeof(WCHAR));
	md5_finish(&md5st, digest);
	tmpd = (DWORD*) digest;
	tmpd[0] ^= tmpd[2];
	tmpd[1] ^= tmpd[3];

	hash = *(LARGE_INTEGER*) tmpd;
	
	ExAcquireFastMutex(&g_fmInternalSync);

	if (gExecuteLastHash.QuadPart == hash.QuadPart)
	{
	}
	else
	{
		if (!gpExecuteHashes)
		{
			gpExecuteHashes = ExAllocatePoolWithTag(PagedPool, sizeof(LARGE_INTEGER) * _execute_max_count, 'EBOS');
			gExecutePos = 0;
			gExecuteLastHash.QuadPart = 0;

			memset(gpExecuteHashes, 0, sizeof(LARGE_INTEGER) * _execute_max_count);
		}

		if (gpExecuteHashes)
		{
			BOOLEAN bFound = FALSE;
			ULONG curpos = 0;
			LARGE_INTEGER*	pExecuteHashes = gpExecuteHashes;
			while (pExecuteHashes[curpos].QuadPart)
			{
				if (pExecuteHashes[curpos].QuadPart == hash.QuadPart)
				{
					bFound = TRUE;
					break;
				}
				
				curpos++;
				if (curpos == _execute_max_count)
					break;
			}

			if (!bFound)
			{
				DbPrint(DC_SYS, DL_NOTIFY, ("Store cache %S 0x%I64x\n", pwchName, hash));	

				gpExecuteHashes[gExecutePos] = hash;
				gExecuteLastHash = hash;

				gExecutePos++;
				if (_execute_max_count == gExecutePos)
					gExecutePos = 0;
			}
		}
	}

	ExReleaseFastMutex(&g_fmInternalSync);

	ExFreePool(pwchNameImp);
}

NTSTATUS
SysFindInSectionHashes (
	LARGE_INTEGER hash )
{
	NTSTATUS ntStatus = STATUS_NOT_FOUND;

	ExAcquireFastMutex(&g_fmInternalSync);
	if (gpExecuteHashes)
	{
		ULONG curpos = 0;
		LARGE_INTEGER*	pExecuteHashes = gpExecuteHashes;
		while (pExecuteHashes[curpos].QuadPart)
		{
			if (pExecuteHashes[curpos].QuadPart == hash.QuadPart)
			{
				ntStatus = STATUS_SUCCESS;
				break;
			}
			
			curpos++;
			if (curpos == _execute_max_count)
				break;
		}
	}

	ExReleaseFastMutex(&g_fmInternalSync);

	return ntStatus;
}


//+ ------------------------------------------------------------------------------------------

VOID
ParamPlaceValueStrW(PWCHAR pwstrSource, PWCHAR pwstrDest, ULONG* pParamSize)
{
	ULONG len;
	if (!pwstrSource || ((ULONG) pwstrSource == -1))
	{
		*pParamSize = 0;
		return;
	}
	
	__try
	{
		len = (wcslen(pwstrSource) + 1) * sizeof(WCHAR);
	}
	__except(EXCEPTION_EXECUTE_HANDLER)
	{
		DbgBreakPoint();
		*pParamSize = 0;
		return;
	}

	if (len  > *pParamSize)
	{
		pwstrDest[0] = 0;
		*pParamSize = sizeof(WCHAR);
	}
	else
	{
		memcpy(pwstrDest, pwstrSource, len);
		*pParamSize = len;
	}
}

VOID
ParamPlaceValueStr(PCHAR pstrSource, PCHAR pstrDest, ULONG* pParamSize)
{
	strncpy(pstrDest, pstrSource, *pParamSize);
	*pParamSize = strlen(pstrDest) + 1;
}

//+ ------------------------------------------------------------------------------------------
ProcInfoItem* GetProcInfoByProcId(ULONG ProcessId)
{
	if (ProcessId < _countof(ProcIdtoHandle))
		return &ProcIdtoHandle[ProcessId];

	return NULL;
}

PFILE_OBJECT QueryStdhandle(HANDLE hIo)
{
	PFILE_OBJECT pFileObject = NULL;
	NTSTATUS ntStatus;
	
	if (((ULONG)hIo > 0x10) && ((ULONG)hIo < 0xffff))
	{
		ntStatus = ObReferenceObjectByHandle(hIo, FILE_READ_DATA, *IoFileObjectType, KernelMode, (PVOID*) &pFileObject, NULL);
		if (NT_SUCCESS(ntStatus))
			return pFileObject;
	}

	return NULL;
}

void CreateProcessNotify(ULONG CreateFlags)
{
	//_PARAM_OBJECT_URL_PARAM_W
	//_PARAM_OBJECT_CONTEXT_FLAGS
	VERDICT Verdict = Verdict_NotFiltered;
	BYTE *RequestData;

	RequestData = ExAllocateFromNPagedLookasideList(gpPL0x4000);
	if (RequestData == NULL)
		return;
	else
	{
		PFILTER_EVENT_PARAM	pParam = (PFILTER_EVENT_PARAM) RequestData;
		PSINGLE_PARAM pSingleParam;

		FILTER_PARAM_COMMONINIT(pParam, FLTTYPE_SYSTEM, 0x6/*MJ_CREATE_PROCESS_NOTIFY_EXTEND*/, 0, PreProcessing, 2);
		pSingleParam = (PSINGLE_PARAM) pParam->Params;

		SINGLE_PARAM_INIT_SID(pSingleParam);

		SINGLE_PARAM_SHIFT(pSingleParam);
		SINGLE_PARAM_INIT_ULONG(pSingleParam, _PARAM_OBJECT_CONTEXT_FLAGS, CreateFlags);

		// -----------------------------------------------------------------------------------------
		Verdict = FilterEvent(pParam, NULL);
	}

	ExFreeToNPagedLookasideList(gpPL0x4000, RequestData);
	return;
}

void TerminateThreadNotify(SYS_NOTIFY_THREAD_CONTEXT* pThreadContext)
{
	VERDICT Verdict = Verdict_NotFiltered;
	BYTE *RequestData;

	if (!IsNeedFilterEvent(FLTTYPE_SYSTEM, 0xa/*MJ_SYSTEM_TERMINATE_THREAD_NOTIFY*/, 0))
		return;

	RequestData = ExAllocateFromNPagedLookasideList(gpPL0x140);
	if (RequestData == NULL)
		return;
	else
	{
		PFILTER_EVENT_PARAM	pParam = (PFILTER_EVENT_PARAM) RequestData;
		PSINGLE_PARAM pSingleParam;

		FILTER_PARAM_COMMONINIT(pParam, FLTTYPE_SYSTEM, 0xa/*MJ_SYSTEM_TERMINATE_THREAD_NOTIFY*/, 0, PostProcessing, 3);
		pSingleParam = (PSINGLE_PARAM) pParam->Params;

		SINGLE_PARAM_INIT_SID(pSingleParam);

		SINGLE_PARAM_SHIFT(pSingleParam);
		SINGLE_PARAM_INIT_ULONG(pSingleParam, _PARAM_OBJECT_CLIENTID1, pThreadContext->m_ProcessId);

		SINGLE_PARAM_SHIFT(pSingleParam);
		SINGLE_PARAM_INIT_ULONG(pSingleParam, _PARAM_OBJECT_CLIENTID2, pThreadContext->m_ThreadId);

		// -----------------------------------------------------------------------------------------
		Verdict = FilterEvent(pParam, NULL);
	}

	ExFreeToNPagedLookasideList(gpPL0x140, RequestData);
	return;
}

void ProcessCheckParams(HANDLE hProcess)
{
	NTSTATUS ntStatus;
	PROCESS_BASIC_INFORMATION ProcInfo;
	ULONG Flags = CREATE_PROCESS_NOTIFY_FLAG_NONE;	// this first parameter

	if (!IsNeedFilterEvent(FLTTYPE_SYSTEM, 0x6/*MJ_CREATE_PROCESS_NOTIFY_EXTEND*/, 0))
		return;

	ntStatus = ZwQueryInformationProcess(hProcess, ProcessBasicInformation, &ProcInfo, sizeof(ProcInfo), NULL);

	if (STATUS_SUCCESS == ntStatus)
	{
		if (ProcInfo.PebBaseAddress != 0)
		{
			impPROCESS_PARAMETERS** ppProcessParameters = (impPROCESS_PARAMETERS**) ((PBYTE)ProcInfo.PebBaseAddress + 0x10);
			if (*ppProcessParameters != NULL)
			{
				PFILE_OBJECT pFileObjectStd;
				impPROCESS_PARAMETERS* pProcessParameters = *ppProcessParameters;

				pFileObjectStd = QueryStdhandle(pProcessParameters->hStdInput);
				if (pFileObjectStd != NULL)
				{
					if (pFileObjectStd->DeviceObject != NULL)
					{
						if (pFileObjectStd->DeviceObject->DeviceType == FILE_DEVICE_NAMED_PIPE)
							Flags |= CREATE_PROCESS_NOTIFY_FLAG_STDIN_REMOTE;
					}
					ObDereferenceObject(pFileObjectStd);
				}

				pFileObjectStd = QueryStdhandle(pProcessParameters->hStdOutput);
				if (pFileObjectStd != NULL)
				{
					if (pFileObjectStd->DeviceObject != NULL)
					{
						if (pFileObjectStd->DeviceObject->DeviceType == FILE_DEVICE_NAMED_PIPE)
							Flags |= CREATE_PROCESS_NOTIFY_FLAG_STDOUT_REMOTE;
					}
					ObDereferenceObject(pFileObjectStd);
				}
			}
		}
	}

	CreateProcessNotify(Flags);
}

//+ ------------------------------------------------------------------------------------------

VOID
ObjectListCleanupCommonByOwner(ULONG Owner)
{
	SysNameCacheCleanupByProcInternal(gpSysContext, Owner);
	
	NameCacheCleanupByProc((PVOID) Owner);
}

ULONG gProcessesStart = 0;

VOID
ProcessCrNotify(IN HANDLE ParentId, IN HANDLE ProcessId, IN BOOLEAN Create)
{
	SYS_NOTIFY_PROCESS_CONTEXT ProcessContext;
	DbPrint(DC_SYS, DL_SPAM, ("ProcessCrNotify ParentId=%#x ProcessId=%#x Create=%#x\n",ParentId, ProcessId, Create));
	// if delete - current process, otherwise no

	ProcessContext.m_ParentId = ParentId;
	ProcessContext.m_ProcessId = ProcessId;

	if (Create)
	{
		DbPrint(DC_SYS, DL_IMPORTANT, ("Create process notify ParentId=%#x ProcessId=%#x\n",ParentId, ProcessId));

		CheckProc_RemoveFromList((ULONG) ProcessContext.m_ProcessId);
		InvProc_CheckNewChild(ParentId, ProcessId);

		if (IsNeedFilterEvent(FLTTYPE_SYSTEM, 0xe/*MJ_CREATE_PROCESS_NOTIFY*/, 0))
		{
			BYTE *RequestData = ExAllocateFromNPagedLookasideList(gpPL0x140);
			
			if (RequestData != NULL)
			{
				PFILTER_EVENT_PARAM	pParam = (PFILTER_EVENT_PARAM) RequestData;
				PSINGLE_PARAM pSingleParam;

				FILTER_PARAM_COMMONINIT(pParam, FLTTYPE_SYSTEM, 0xe/*MJ_CREATE_PROCESS_NOTIFY*/, 0, PostProcessing, 2);
				pParam->UnsafeValue = 1;
				pSingleParam = (PSINGLE_PARAM) pParam->Params;

				SINGLE_PARAM_INIT_SID(pSingleParam);

				SINGLE_PARAM_SHIFT(pSingleParam);
				SINGLE_PARAM_INIT_ULONG(pSingleParam, _PARAM_OBJECT_SOURCE_ID, (ULONG) ProcessId);
			
				FilterEvent(pParam, NULL);

				ExFreeToNPagedLookasideList(gpPL0x140, RequestData);
			}
		}

		gProcessesStart++;
	}
	else
	{
		TSPProcCrash(ProcessId);
		DisconnectClientByProcID((ULONG) ProcessId);//pClient->m_ProcessId
		ObjectListCleanupCommonByOwner((ULONG) ProcessId);

		InvProc_RemoveFromList(ProcessId);

		if ((ULONG) ProcessId < _countof(ProcIdtoHandle))
		{
			ProcInfoItem* pProcInfo = &ProcIdtoHandle[(ULONG) ProcessId];
			pProcInfo->m_ImagesLoadedCount = 0;
			pProcInfo->m_bKernel32Hooked = FALSE;
			
			_RESOURCE_LOCK_W(&gpSysContext->m_CacheResource);
			if (pProcInfo->m_pwchmainImage)
			{
				ExFreePool(pProcInfo->m_pwchmainImage);
				pProcInfo->m_pwchmainImage = 0;
			}
			_RESOURCE_UNLOCK(&gpSysContext->m_CacheResource);
		}

		if (IsNeedFilterEvent(FLTTYPE_SYSTEM, 5/*MJ_EXIT_PROCESS*/, 0))
		{
			BYTE *RequestData = ExAllocateFromNPagedLookasideList(gpPL0x140);
			
			if (RequestData != NULL)
			{
				PFILTER_EVENT_PARAM	pParam = (PFILTER_EVENT_PARAM) RequestData;
				PSINGLE_PARAM pSingleParam;

				FILTER_PARAM_COMMONINIT(pParam, FLTTYPE_SYSTEM, 5/*MJ_EXIT_PROCESS*/, 0, PostProcessing, 1);
				pParam->UnsafeValue = 1;
				pSingleParam = (PSINGLE_PARAM) pParam->Params;

				SINGLE_PARAM_INIT_ULONG(pSingleParam, _PARAM_OBJECT_SOURCE_ID, (ULONG) ProcessId);
			
				FilterEvent(pParam, NULL);

				ExFreeToNPagedLookasideList(gpPL0x140, RequestData);
			}
		}
	}
}

VOID
ThreadCrNotify(IN HANDLE ProcessId, IN HANDLE ThreadId, IN BOOLEAN Create)
{
	SYS_NOTIFY_THREAD_CONTEXT ThreadContext;

	DbPrint(DC_SYS, DL_SPAM, ("ThreadCrNotify ProcessId=%#x ThreadId=%#x Create=%#x CurThID=%#x\n", 
		ProcessId, ThreadId, Create, PsGetCurrentThreadId()));
	// if delete - current thread, otherwise no
	
	ThreadContext.m_ProcessId = (ULONG) ProcessId;
	ThreadContext.m_ThreadId = (ULONG) ThreadId;

	if (!Create)
	{
		TSPThreadCrash((PVOID)ThreadId);
		DisconnectClientByThreadID((ULONG) ThreadId);
		DelInvisibleThread((ULONG) ThreadId, TRUE);
	
		TerminateThreadNotify(&ThreadContext);

// clear AttachThreadInput cache for this thread
		GenCacheFree(
			g_pAttachedInputThreadCache,
			ThreadId
			);
	}
	else
	{
		// create thread
		//ProcIdtoThread[(ULONG)ThreadId]
	}
}

void LoadImageNotifyCallback (
	IN PUNICODE_STRING FullImageName, 
	IN HANDLE ProcessId)
{
	PFILTER_EVENT_PARAM pParam = NULL;
	ULONG params_size;
	
	params_size = sizeof(EVENT_TRANSMIT);
	params_size += sizeof(SINGLE_PARAM) * 2;
	if (FullImageName->Length)			// _PARAM_OBJECT_URL_W + \0
		params_size += FullImageName->Length + sizeof(WCHAR);
	params_size += __SID_LENGTH;			//_PARAM_OBJECT_SID 
	
	DbPrint(DC_PARAMS, DL_SPAM, ("LoadImageNotify: size of params %d\n", params_size));

	pParam = ExAllocatePoolWithTag(PagedPool, params_size, 'Pbos');
	if (pParam)
	{
		PSINGLE_PARAM pSingleParam;
		FILTER_PARAM_COMMONINIT(pParam, FLTTYPE_SYSTEM, 0xc /*MJ_SYSTEM_LOAD_IMAGE_NOTIFY*/, 0, PostProcessing, 2);

		pParam->ProcessID = (ULONG) ProcessId;

		pSingleParam = (PSINGLE_PARAM) pParam->Params;
		SINGLE_PARAM_INIT_NONE(pSingleParam, _PARAM_OBJECT_URL_W);

		memcpy(pSingleParam->ParamValue, FullImageName->Buffer, FullImageName->Length);
		((PCHAR)pSingleParam->ParamValue)[FullImageName->Length] = 0;
		((PCHAR)pSingleParam->ParamValue)[FullImageName->Length + 1] = 0;
		pSingleParam->ParamSize = FullImageName->Length + sizeof(WCHAR);

		SINGLE_PARAM_SHIFT(pSingleParam);
		SINGLE_PARAM_INIT_SID(pSingleParam);

		FilterEvent(pParam, NULL);

		ExFreePool(pParam);
	}
}

//Andrey Gruzdev 19.08.05-----------------------------------------------------------------------
static const WCHAR Novell_Driver_Names[][KLMAX_PROCNAME]={L"nwfs.sys",L"hgfs.sys"};
static const LONG Num_Novell_Drivers=2;

#ifndef min
	#define min(x,y)	(x<y?x:y)
#endif

BOOLEAN isNovellDriver(IN PUNICODE_STRING Full_ImageName)
{
	ULONG Full_ImageName_Len;

	LONG iChar;
	WCHAR Curr_Char;
	LONG iName_Start;
	
	WCHAR Driver_Name[KLMAX_PROCNAME];
	ULONG Driver_Name_Len;

	LONG iNovell_Driver;

	UNICODE_STRING	cntdDriver_Name,
					cntdNovell_Driver_Name;

	Full_ImageName_Len = Full_ImageName->Length / sizeof(WCHAR);

	if (!Full_ImageName_Len)
		return FALSE;

// выделяем имя драйвера
	iName_Start = 0;
	for (iChar = Full_ImageName_Len-1; iChar>=0; iChar--)
	{
		Curr_Char = Full_ImageName->Buffer[iChar];

		if (Curr_Char==L'\\' || Curr_Char==L'/')
		{
			iName_Start = iChar + 1;
			break;
		}
	}

	Driver_Name_Len=min(Full_ImageName_Len-iName_Start,KLMAX_PROCNAME-1);

// копируем подстроку с именем драйвера без пути
	RtlCopyMemory(Driver_Name,Full_ImageName->Buffer+iName_Start,Driver_Name_Len*sizeof(WCHAR));
	Driver_Name[Driver_Name_Len]=0;
	
// ищем в таблице Novell_Driver_Names драйвер с нашим именем
	RtlInitUnicodeString(&cntdDriver_Name,Driver_Name);
	for (iNovell_Driver=0;iNovell_Driver<Num_Novell_Drivers;iNovell_Driver++)
	{
		RtlInitUnicodeString(&cntdNovell_Driver_Name,Novell_Driver_Names[iNovell_Driver]);

		if (RtlEqualString((PSTRING)&cntdDriver_Name,(PSTRING)&cntdNovell_Driver_Name,TRUE))
			return TRUE;
	}

	return FALSE;
}

VOID
LoadImageNotify(IN PUNICODE_STRING FullImageName, IN HANDLE ProcessId, IN PIMAGE_INFO ImageInfo)
{
	WCHAR DllName[KLMAX_PROCNAME];
	LONG	i,j,finLen;

	ProcInfoItem* pProcInfo = NULL;

	DbPrint(DC_SYS, DL_INFO, ("LoadImageNotify ProcId %x Base %08x Len %08x\n",ProcessId,ImageInfo->ImageBase,ImageInfo->ImageSize));	

	if (FullImageName == 0 )
		return;
	if(FullImageName->Length==0)
		return;

	if (ProcessId == 0)
	{
		gFSNeedRescan++; // loading new driver! :(
		StoreDriverName(FullImageName);
	}

// Andrey Gruzdev 19.08.05-----------------------------------------------------------------
	if (ProcessId == 0)
	{
		if (*NtBuildNumber > 2195)
		{
			if (isNovellDriver(FullImageName))	// Novell driver loaded - need to rescan drivers
				gFSNeedRescan=2;				// *********************************************
		}
		else
			gFSNeedRescan++; // loading new driver! :(

	}
//------------------------------------------------------------------------------------------

	LoadImageNotifyCallback(FullImageName, ProcessId);
	
	finLen = FullImageName->Length/sizeof(WCHAR);
	
	for(i = finLen-1; i > 0; i--)
	{
		if(FullImageName->Buffer[i] == L'\\' || FullImageName->Buffer[i] == L'/')
		{
			i++;
			break;
		}
	}
	
	for(j = 0; i < finLen && j < KLMAX_PROCNAME - 1; i++, j++)
	{
		DllName[j] = FullImageName->Buffer[i];
	}
	
	DllName[j] = 0;
	DbPrint(DC_SYS, DL_INFO, ("LoadImageNotify ProcId %x Base %x Len %x\n", ProcessId, ImageInfo->ImageBase, ImageInfo->ImageSize));

	pProcInfo = GetProcInfoByProcId((ULONG) ProcessId);

	if (pProcInfo != NULL)
	{
		if(!pProcInfo->m_bKernel32Hooked)
			pProcInfo->m_bKernel32Hooked = R3Hook(DllName,ImageInfo->ImageBase);
		if (ProcessId && (!pProcInfo->m_ImagesLoadedCount))
		{
			ULONG lentmp;
			_RESOURCE_LOCK_W(&gpSysContext->m_CacheResource);
			if (pProcInfo->m_pwchmainImage)
			{
				ExFreePool(pProcInfo->m_pwchmainImage);
				pProcInfo->m_pwchmainImage = 0;
			}

			lentmp = Uwcslen(FullImageName->Buffer, FullImageName->Length / sizeof(WCHAR)) * sizeof(WCHAR);
			lentmp += sizeof(WCHAR);

			pProcInfo->m_pwchmainImage = ExAllocatePoolWithTag(NonPagedPool, lentmp, 'SOBl');
			if (pProcInfo->m_pwchmainImage)
			{
				memset(pProcInfo->m_pwchmainImage, 0, lentmp);
				Uwcsncpy(pProcInfo->m_pwchmainImage, lentmp / sizeof(WCHAR), FullImageName->Buffer, FullImageName->Length);
			}

			_RESOURCE_UNLOCK(&gpSysContext->m_CacheResource);
		}

		pProcInfo->m_ImagesLoadedCount++;

		if (pProcInfo->m_ImagesLoadedCount == 4)	
		{
			//pochemu 4? main image + ntdll.dll + kernel32.dll + current
			// только к этому моменету startup parameters полностью инициализированны!!!
			ProcessCheckParams(NtCurrentProcess());
		}
	}
}

//+ ------------------------------------------------------------------------------------------

VERDICT
OpenProcessCallback(IN PSYS_OPEN_PROCESS_CONTEXT ProcessContext)
{
	BYTE *RequestData;
	VERDICT Verdict = Verdict_NotFiltered;

//	if (!IsNeedFilterEvent(FLTTYPE_SYSTEM, 3/*MJ_SYSTEM_OPEN_PROCESS*/, 0))
//		return Verdict;
	RequestData = ExAllocateFromNPagedLookasideList(gpPL0x140);
	if (RequestData == NULL)
		return Verdict;

	{
		DWORD ContextFlags = 0;
		PFILTER_EVENT_PARAM	pParam = (PFILTER_EVENT_PARAM) RequestData;
		PSINGLE_PARAM pSingleParam;

		FILTER_PARAM_COMMONINIT(pParam, FLTTYPE_SYSTEM, 3/*MJ_SYSTEM_OPEN_PROCESS*/, 0, PreProcessing, 4);
		pParam->ProcessID = (ULONG) PsGetCurrentProcessId();

		if (ProcessContext->m_ProcessClientId == pParam->ProcessID)
			ContextFlags = _CONTEXT_OBJECT_FLAG_OPERATION_ON_SELF;

		pSingleParam = (PSINGLE_PARAM) pParam->Params;
		SINGLE_PARAM_INIT_SID(pSingleParam);
		SINGLE_PARAM_SHIFT(pSingleParam);
		SINGLE_PARAM_INIT_ULONG(pSingleParam, _PARAM_OBJECT_ACCESSATTR, ProcessContext->m_DesiredAccess);
		SINGLE_PARAM_SHIFT(pSingleParam);
		SINGLE_PARAM_INIT_ULONG(pSingleParam, _PARAM_OBJECT_DEST_ID, ProcessContext->m_ProcessClientId);

		SINGLE_PARAM_SHIFT(pSingleParam);
		SINGLE_PARAM_INIT_ULONG(pSingleParam, _PARAM_OBJECT_CONTEXT_FLAGS, ContextFlags);
	
		// -----------------------------------------------------------------------------------------
		Verdict = FilterEvent(pParam, NULL);
	}

	ExFreeToNPagedLookasideList(gpPL0x140, RequestData);

	return Verdict;
}

VERDICT
WriteProcessMemoryCallback(IN PSYS_WRITE_PROCESS_MEMORY_CONTEXT pWriteProcessContext, PROCESSING_TYPE ProcessingType)
{
	BYTE *RequestData;
	VERDICT Verdict = Verdict_NotFiltered;

	if (IsNeedFilterEvent(FLTTYPE_SYSTEM, 0xb/*MJ_SYSTEM_WRITE_PROCESS_MEMORY*/, 0))
	{
		BYTE *RequestData = ExAllocateFromNPagedLookasideList(gpPL0x4000);
		if (RequestData != NULL)
		{
			LUID Luid = {0, 0};
			PFILTER_EVENT_PARAM	pParam = (PFILTER_EVENT_PARAM) RequestData;
			PSINGLE_PARAM pSingleParam;

			FILTER_PARAM_COMMONINIT(pParam, FLTTYPE_SYSTEM, 0xb/*MJ_SYSTEM_WRITE_PROCESS_MEMORY*/, 0, ProcessingType, 5);
			pSingleParam = (PSINGLE_PARAM) pParam->Params;

			SINGLE_PARAM_INIT_ULONG(pSingleParam, _PARAM_OBJECT_CLIENTID1, pWriteProcessContext->m_DestProcId);
			
			SINGLE_PARAM_SHIFT(pSingleParam);
			SINGLE_PARAM_INIT_SID(pSingleParam);

			SINGLE_PARAM_SHIFT(pSingleParam);
			SINGLE_PARAM_INIT_ULONG(pSingleParam, _PARAM_OBJECT_OBJECTHANDLE, (ULONG) pWriteProcessContext->ProcessHandle);

			SINGLE_PARAM_SHIFT(pSingleParam);
			SINGLE_PARAM_INIT_ULONG(pSingleParam, _PARAM_OBJECT_SECTOR, (ULONG) pWriteProcessContext->m_StartAddress);

			if (SeGetLuid(&Luid))
			{
				SINGLE_PARAM_SHIFT(pSingleParam);
				SINGLE_PARAM_INIT_NONE(pSingleParam, _PARAM_OBJECT_LUID);
				pSingleParam->ParamSize = sizeof(LUID);
				memcpy(pSingleParam->ParamValue, &Luid, sizeof(LUID));
				pParam->ParamsCount++;
			}

			SINGLE_PARAM_SHIFT(pSingleParam);
			SINGLE_PARAM_INIT_ULONG(pSingleParam, _PARAM_OBJECT_DATALEN, pWriteProcessContext->m_BytesToWrite);

			if (pWriteProcessContext->m_Buffer)
			{
				try
				{
					SINGLE_PARAM_SHIFT(pSingleParam);
					SINGLE_PARAM_INIT_NONE(pSingleParam, _PARAM_OBJECT_BINARYDATA);
					pSingleParam->ParamSize = pWriteProcessContext->m_BytesToWrite;
					memcpy(pSingleParam->ParamValue, pWriteProcessContext->m_Buffer, pWriteProcessContext->m_BytesToWrite);
					pParam->ParamsCount++;
				}
				except(EXCEPTION_EXECUTE_HANDLER)
				{
				}
			}
		
			Verdict = FilterEvent(pParam, NULL);

			ExFreeToNPagedLookasideList(gpPL0x4000, RequestData);
		}
	}
	
	return Verdict;
}


NTSTATUS __stdcall
KLHookOpenProcess (
	OUT PHANDLE ProcessHandle,
	IN ACCESS_MASK DesiredAccess,
	IN POBJECT_ATTRIBUTES ObjectAttributes,
	IN PCLIENT_ID ClientId OPTIONAL
	)
{
	KIRQL CurrIrql;
	NTSTATUS ntStatusRet;
	VERDICT	Verdict = Verdict_Default;

	SYS_OPEN_PROCESS_CONTEXT ProcessContext;

	DbPrint(DC_SYS, DL_SPAM, ("OpenProcess ProcessHandle=%#x DesiredAccess=%#x\n",ProcessHandle, DesiredAccess));
	// if delete - current process, otherwise no

	CurrIrql = KeGetCurrentIrql();
	if (CurrIrql < DISPATCH_LEVEL)
	{
		PCLIENT_ID pClientId = NULL;
		ProcessContext.m_DesiredAccess = DesiredAccess;
		ProcessContext.m_Status = 0;
		ProcessContext.m_ProcessClientId = 0;

		pClientId = CopyUserBuffer( ClientId, sizeof(CLIENT_ID) );
		if (pClientId)
		{
			ProcessContext.m_ProcessClientId = (ULONG) pClientId->UniqueProcess;
			ExFreePool( pClientId );
		}

		Verdict = OpenProcessCallback(&ProcessContext);
	}

	if (!_PASS_VERDICT(Verdict))
		ntStatusRet = STATUS_ACCESS_DENIED;
	else
	{
		ntStatusRet = NtOpenProcess(ProcessHandle, DesiredAccess, ObjectAttributes, ClientId);
		ProcessContext.m_Status = ntStatusRet;
		if (CurrIrql < DISPATCH_LEVEL)
		{
		//	ProcessCallbacks(KLYS_OPEN_PROCESS, 0, &CallbackData, FALSE, &VerdictEx);
		}
	}
	
	return ntStatusRet;
}

VERDICT
KLHookCreateProcessCommon(PSYS_CREATE_PROCESS_CONTEXT pContext, ULONG Mj, PROCESSING_TYPE ProcessingType)
{
	VERDICT Verdict = Verdict_NotFiltered;
	BYTE *RequestData;
	ULONG ProcessId;

	if (!IsNeedFilterEvent(FLTTYPE_SYSTEM, Mj, 0))
		return Verdict;

	RequestData = ExAllocateFromNPagedLookasideList(gpPL0x4000);
	if (RequestData == NULL)
		return Verdict;
	else
	{
		PFILTER_EVENT_PARAM	pParam = (PFILTER_EVENT_PARAM) RequestData;
		PSINGLE_PARAM pSingleParam;

		FILTER_PARAM_COMMONINIT(pParam, FLTTYPE_SYSTEM, Mj, 0, ProcessingType, 5);
		pSingleParam = (PSINGLE_PARAM) pParam->Params;

		SINGLE_PARAM_INIT_SID(pSingleParam);

		SINGLE_PARAM_SHIFT(pSingleParam);
		SINGLE_PARAM_INIT_ULONG(pSingleParam, _PARAM_OBJECT_SOURCE_ID, (ULONG) PsGetCurrentProcessId());

		SINGLE_PARAM_SHIFT(pSingleParam);
		ProcessId = pContext->m_ProcessHandle == 0 ? 0 : GetProcessIDByHandle(pContext->m_ProcessHandle, NULL);
		SINGLE_PARAM_INIT_ULONG(pSingleParam, _PARAM_OBJECT_DEST_ID, ProcessId);

		SINGLE_PARAM_SHIFT(pSingleParam);
		SINGLE_PARAM_INIT_ULONG(pSingleParam, _PARAM_OBJECT_OBJECTHANDLE, (ULONG) pContext->m_ProcessHandle);

		SINGLE_PARAM_SHIFT(pSingleParam);
		SINGLE_PARAM_INIT_NONE(pSingleParam, _PARAM_OBJECT_URL_W);

		pSingleParam->ParamSize = 0x3d00;
		ParamPlaceValueStrW(pContext->m_pFileName, (PWCHAR) pSingleParam->ParamValue, &pSingleParam->ParamSize);

		// -----------------------------------------------------------------------------------------
		Verdict = FilterEvent(pParam, NULL);


		ExFreeToNPagedLookasideList(gpPL0x4000, RequestData);
	}

	return Verdict;
}

ULONG gnCreateCount = 0;

NTSTATUS __stdcall
KLHookCreateProcess (
	OUT PHANDLE ProcessHandle,
	IN ACCESS_MASK DesiredAccess,
	IN POBJECT_ATTRIBUTES ObjectAttributes OPTIONAL,
	IN HANDLE ParentProcess,
	IN BOOLEAN InheritObjectTable,
	IN HANDLE SectionHandle OPTIONAL,
	IN HANDLE DebugPort OPTIONAL,
	IN HANDLE ExceptionPort OPTIONAL
	)
{
	NTSTATUS	ntStatusRet;
	VERDICT	Verdict = Verdict_Default;

	SYS_CREATE_PROCESS_CONTEXT CreateContext;
	PSYS_CACHEITEM pCacheItem = NULL;

	DbPrint(DC_SYS, DL_SPAM, ("CreateProcess SectionHandle = %#x DesiredAccess=%#x\n", SectionHandle, DesiredAccess));

	CreateContext.m_ProcessHandle = 0;
	CreateContext.m_DesiredAccess = DesiredAccess;
	CreateContext.m_pFileName = NULL;

	if (SectionHandle)
	{
		pCacheItem = ExAllocateFromNPagedLookasideList(gpPSysFileNames);
		if (pCacheItem != NULL)
		{
			BOOLEAN bOverBuf = FALSE;
			if (SysNameCacheGet(gpSysContext, (ULONG) SectionHandle, pCacheItem, _SYSQUERY_INFO_LEN, &bOverBuf))
				CreateContext.m_pFileName = pCacheItem->m_Name;
		}
	}
	
	/*if (CreateContext.m_pFileName)
	{
		DbPrint(DC_SYS, DL_IMPORTANT, ("Creating process %S\n", CreateContext.m_pFileName));
	}*/

	CreateContext.m_Status = 0;

	Verdict = KLHookCreateProcessCommon(&CreateContext, 1/*MJ_SYSTEM_CREATE_PROCESS*/, PreProcessing);
	if (!_PASS_VERDICT(Verdict))
		ntStatusRet = STATUS_ACCESS_DENIED;
	else
	{
		PHANDLE ProcessHandleTmp = NULL;
		ntStatusRet = NtCreateProcess(ProcessHandle, DesiredAccess, ObjectAttributes, ParentProcess, InheritObjectTable, SectionHandle, DebugPort, ExceptionPort);

		ProcessHandleTmp = CopyUserBuffer( ProcessHandle, sizeof(HANDLE) );

		if (ProcessHandleTmp)
		{
			CreateContext.m_ProcessHandle = *ProcessHandleTmp;
			ExFreePool( ProcessHandleTmp );
		}

		if (NT_SUCCESS(ntStatusRet))
		{
			gnCreateCount++;
			if (gnCreateCount > 3)
				FidBox_Allow();
		}
		
		CreateContext.m_Status = ntStatusRet;
		
		Verdict = KLHookCreateProcessCommon(&CreateContext, 1/*MJ_SYSTEM_CREATE_PROCESS*/, PostProcessing);
	}

	if (pCacheItem != NULL)
		ExFreeToNPagedLookasideList(gpPSysFileNames, pCacheItem);
	
	return ntStatusRet;
}

NTSTATUS __stdcall
KLHookCreateProcessEx(OUT PHANDLE ProcessHandle, IN ACCESS_MASK DesiredAccess, IN POBJECT_ATTRIBUTES ObjectAttributes OPTIONAL, IN HANDLE ParentProcess, IN BOOLEAN InheritObjectTable, IN HANDLE SectionHandle OPTIONAL, IN HANDLE DebugPort OPTIONAL, IN HANDLE ExceptionPort OPTIONAL, IN ULONG UnknowDword OPTIONAL)
{
	NTSTATUS	ntStatusRet;
	VERDICT	Verdict = Verdict_Default;

	SYS_CREATE_PROCESS_CONTEXT CreateContext;
	PSYS_CACHEITEM pCacheItem = NULL;

	DbPrint(DC_SYS, DL_SPAM, ("CreateProcessEx SectionHandle = %#x DesiredAccess=%#x. Owner %#x\n", SectionHandle, DesiredAccess, PsGetCurrentProcessId()));

	CreateContext.m_ProcessHandle = 0;
	CreateContext.m_DesiredAccess = DesiredAccess;
	CreateContext.m_pFileName = NULL;

	if (SectionHandle != NULL)
	{
		pCacheItem = ExAllocateFromNPagedLookasideList(gpPSysFileNames);
		if (pCacheItem != NULL)
		{
			BOOLEAN bOverBuf = FALSE;
			if (SysNameCacheGet(gpSysContext, (ULONG) SectionHandle, pCacheItem, _SYSQUERY_INFO_LEN, &bOverBuf))
				CreateContext.m_pFileName = pCacheItem->m_Name;
		}
	}

	CreateContext.m_Status = 0;

	Verdict = KLHookCreateProcessCommon(&CreateContext, 2/*MJ_SYSTEM_CREATE_PROCESS_EX*/, PreProcessing);
	if (!_PASS_VERDICT(Verdict))
		ntStatusRet = STATUS_ACCESS_DENIED;
	else
	{
		PHANDLE ProcessHandleTmp = NULL;
		ntStatusRet = NtCreateProcessEx(ProcessHandle, DesiredAccess, ObjectAttributes, ParentProcess, InheritObjectTable, SectionHandle, DebugPort, ExceptionPort, UnknowDword);

		ProcessHandleTmp = CopyUserBuffer( ProcessHandle, sizeof(HANDLE) );

		if (ProcessHandleTmp)
		{
			CreateContext.m_ProcessHandle = *ProcessHandleTmp;
			ExFreePool( ProcessHandleTmp );
		}

		CreateContext.m_Status = ntStatusRet;

		Verdict = KLHookCreateProcessCommon(&CreateContext, 2/*MJ_SYSTEM_CREATE_PROCESS_EX*/, PostProcessing);
	}

	if (pCacheItem != NULL)
		ExFreeToNPagedLookasideList(gpPSysFileNames, pCacheItem);
	
	return ntStatusRet;
}

NTSTATUS __stdcall
KLHookClose(IN HANDLE Handle)
{
	NTSTATUS ntStatusRet;
	NTSTATUS ntStatus;
	PFILE_OBJECT pFileObj = NULL;
	BOOLEAN bOverBuf;

	if(Handle)
	{
		DbPrint(DC_SYS, DL_SPAM, ("CloseHandle Handle = %#x\n", Handle));
		SysNameCacheFree(gpSysContext, (ULONG) Handle);
	}
	else
	{
		DbPrint(DC_SYS, DL_SPAM, ("Closing NULL handle...\n"));
	}

	ntStatusRet = NtClose(Handle);

// clear LPC port cache for this handle
//	GenCacheFree(
//		g_pRPCMapperPortsCache,
//		Handle
//		);

	return ntStatusRet;
}

/*NTSTATUS __stdcall
KLTerminateThread(IN HANDLE ThreadHandle, IN ULONG ThreadExitCode)
{
	NTSTATUS ntStatusRet;

	if (ThreadHandle != 0)
	{
		NTSTATUS ntStatus;
		PKTHREAD pkTh = NULL;

		ntStatus = ObReferenceObjectByHandle(ThreadHandle, STANDARD_RIGHTS_REQUIRED, NULL, KernelMode, &pkTh, NULL);
		if (NT_SUCCESS(ntStatus))
		{
			PVOID pTeb = *(PVOID*)((PBYTE) pkTh + 0x20);
			if (pTeb != NULL)
			{
				CLIENT_ID* pClient = (CLIENT_ID*)((PBYTE) pTeb + 0x20);

  pClient - sometimes bad pointer
				if (pClient != NULL)
				{
					DbPrint(DC_SYS, DL_IMPORTANT, ("Terminating thread %d:%d\n", pClient->UniqueProcess, pClient->UniqueThread));
				}
			}
			ObDereferenceObject(pkTh);
		}
	}

	
	ntStatusRet = NtTerminateThread(ThreadHandle, ThreadExitCode);
	
	return ntStatusRet;
}*/

typedef struct _MEMORY_BASIC_INFORMATION {
    PVOID BaseAddress;
    PVOID AllocationBase;
    ULONG AllocationProtect;
    ULONG RegionSize;
    ULONG State;
    ULONG Protect;
    ULONG Type;
} MEMORY_BASIC_INFORMATION, *PMEMORY_BASIC_INFORMATION;

NTSTATUS __stdcall
KLWriteVirtualMemory (
	IN HANDLE ProcessHandle,
	IN PVOID StartAddress,
	IN PVOID Buffer,
	IN ULONG BytesToWrite,
	OUT PULONG BytesWritten OPTIONAL
	)
{
	NTSTATUS ntStatusRet;
	VERDICT Verdict = Verdict_NotFiltered;

	BOOLEAN bSkip = FALSE;

	PPEB pPeb = NULL;

	SYS_WRITE_PROCESS_MEMORY_CONTEXT WriteProcessContext;
	WriteProcessContext.m_Buffer = NULL;

	if (BytesToWrite <= 0x1000)
	{
		if (MmIsAddressValid(Buffer) && ProbeForReadImp(Buffer, BytesToWrite, sizeof(UCHAR)))
			WriteProcessContext.m_Buffer = Buffer;
	}

	WriteProcessContext.m_BytesToWrite = BytesToWrite;

	WriteProcessContext.m_DestProcId = 0;
	if ((ProcessHandle > (HANDLE)0x10) && (ProcessHandle < (HANDLE) 0xffff))
		WriteProcessContext.m_DestProcId = GetProcessIDByHandle(ProcessHandle, &pPeb);

//	if (((DWORD) pPeb & 0xfffff000) == ((DWORD) StartAddress & 0xfffff000))
//		bSkip = TRUE;
// даем писать в PEB
	if (pPeb && StartAddress)
	{
		ULONG PEBSize = 0;

// win 2k/xp
		if (*NtBuildNumber >= 2195)
		{
// win xp +
			if (*NtBuildNumber >= 2600)
			{
				if (*NtBuildNumber >= 3790)
				{
// win 2003 srv
					PEBSize = 0x230;
				}
				else
				{
// win xp
					PEBSize = 0x20c;
				}
			}
			else
			{
// win 2k
				PEBSize = 0x1e0;
			}
		}
// win nt 4
		else
		{
// не нашел размер PEB'а в NT4, это последние ненулевые байты, которые я нашел, система похоже пишет только в 0x10
			PEBSize = 0x124;
		}

		if ((ULONG)StartAddress >= (ULONG)pPeb &&
			(ULONG)StartAddress < (ULONG)pPeb+PEBSize &&
			(ULONG)StartAddress+BytesToWrite <= (ULONG)pPeb+PEBSize)
			bSkip = TRUE;
	}

	WriteProcessContext.m_StartAddress = StartAddress;
	WriteProcessContext.ProcessHandle = ProcessHandle;

	DbPrint(DC_SYS, DL_NOTIFY, ("WriteVirtualMemory CurrentProcId %#x to ProcId %#x Start address %p (size %d)\n", 
		PsGetCurrentProcessId(), WriteProcessContext.m_DestProcId, StartAddress, BytesToWrite));

	if (!bSkip)
		Verdict = WriteProcessMemoryCallback(&WriteProcessContext, PreProcessing);

	if (!_PASS_VERDICT(Verdict))
		ntStatusRet = STATUS_ACCESS_DENIED;
	else
	{
		ntStatusRet = NtWriteVirtualMemory(ProcessHandle, StartAddress, Buffer, BytesToWrite, BytesWritten);
		if (!bSkip)
		{
			if (NT_SUCCESS(ntStatusRet))
			{
				WriteProcessContext.m_Buffer = NULL; 			// do not filter memory block again

				WriteProcessMemoryCallback(&WriteProcessContext, PostProcessing);
			}
		}
	}
	
	return ntStatusRet;
}

// added by Andrey Gruzdev------------------------------------------------------------------------
#define PROCESS_PSEUDO_HANDLE	((HANDLE)-1)
#define THREAD_PSEUDO_HANDLE	((HANDLE)-2)

NTSTATUS
NTAPI
KLHookDuplicateObject(
	IN HANDLE SourceProcessHandle,
	IN HANDLE SourceHandle,
	IN HANDLE TargetProcessHandle,
	OUT PHANDLE TargetHandle OPTIONAL,
	IN ACCESS_MASK DesiredAccess,
	IN ULONG Attributes,
	IN ULONG Options
	)
{
	NTSTATUS	ntStatus,
				DuplicateHandle_Status;
	HANDLE		Internal_TargetHandle = 0;
	BOOLEAN		Standard_Processing = FALSE;
	VERDICT		Verdict = Verdict_NotFiltered;

	DbPrint(DC_SYS, DL_NOTIFY, ("Entered KLHookDuplicateObject, SourceProcessHandle=%0x, SourceHandle=%0x, TargetProcessHandle=%0x\n",
		SourceProcessHandle, SourceHandle, TargetHandle));

	if (KeGetCurrentIrql() != PASSIVE_LEVEL || !TargetHandle)
	{
		return NtDuplicateObject(SourceProcessHandle, SourceHandle, TargetProcessHandle,
			TargetHandle, DesiredAccess, Attributes, Options);
	}
	
//----------------------------------------------------------------------------------------------------

	DuplicateHandle_Status = NtDuplicateObject(SourceProcessHandle, SourceHandle, TargetProcessHandle,
		TargetHandle, DesiredAccess, Attributes, Options);

	do
	{
		PVOID pObject;

		if (!NT_SUCCESS(DuplicateHandle_Status))
		{
			Standard_Processing = TRUE;
			break;
		}

// если SourceProcessHandle - псевдо-хэндл, то дуплицировать всегда можно
		if (PROCESS_PSEUDO_HANDLE == SourceProcessHandle)
		{
			Standard_Processing = TRUE;
			break;
		}

// если SourceHandle - псевдо-хэндл процесса или потока, то дуплицировать тоже всегда можно
		if ((PROCESS_PSEUDO_HANDLE == SourceHandle) || (THREAD_PSEUDO_HANDLE == SourceHandle))
		{
			Standard_Processing = TRUE;
			break;
		}

// получаем по хэндлу объект
		if (((ULONG)SourceHandle <= 0x10) || ((ULONG)SourceHandle >= 0xffff))
		{
			Standard_Processing = TRUE;
			break;
		}

		__try
		{
			Internal_TargetHandle = *TargetHandle;
		}
		__except(EXCEPTION_EXECUTE_HANDLER)
		{
			Standard_Processing = TRUE;
		}

		if (Standard_Processing)
			break;

		ntStatus = ObReferenceObjectByHandle(Internal_TargetHandle, MAXIMUM_ALLOWED, NULL, KernelMode, &pObject,NULL);

		if (!NT_SUCCESS(ntStatus))
		{
			if (STATUS_INVALID_HANDLE == ntStatus)
			{
				DbPrint(DC_SYS, DL_NOTIFY, ("KLHookDuplicateObject:: ObReferenceObjectByHandle(...) failed - STATUS_INVALID_HANDLE\n"));
			}
			else
			{
				DbPrint(DC_SYS, DL_IMPORTANT, ("KLHookDuplicateObject:: ObReferenceObjectByHandle(...) failed, ntStatus = %0x\n", ntStatus));
			}
			
			Standard_Processing = TRUE;
			break;
		}

		DbPrint(DC_SYS, DL_NOTIFY, ("KLHookDuplicateObject:: DISPATCHER_HEADER::Type==%d\n",((DISPATCHER_HEADER *)pObject)->Type));

// у нас хэндл процесса?
		if (OBJECT_TYPE_PROCESS !=  ((DISPATCHER_HEADER *)pObject)->Type)
		{
			ObDereferenceObject(pObject);
			Standard_Processing = TRUE;
			break;
		}
		ObDereferenceObject(pObject);
	} while (FALSE);



// если фильтровать событие не надо
	if (Standard_Processing)
		return DuplicateHandle_Status;
	else
	{
		DbPrint(DC_SYS, DL_NOTIFY, ("Duplicate process handle: SrcPID=%d DstPID=%d ObjPID=%d\n",
				GetProcessIDByHandle(SourceProcessHandle, NULL),
				GetProcessIDByHandle(TargetProcessHandle, NULL),
				GetProcessIDByHandle(Internal_TargetHandle, NULL)));
	}

	if (IsNeedFilterEvent(FLTTYPE_SYSTEM, 0xf/*MJ_SYSTEM_DUPLICATE_OBJECT*/, 0))
	{
		BYTE *RequestData;

		RequestData = ExAllocateFromNPagedLookasideList(gpPL0x140);
		if (RequestData != NULL)
		{
			PFILTER_EVENT_PARAM	pParam = (PFILTER_EVENT_PARAM) RequestData;
			PSINGLE_PARAM pSingleParam;

			FILTER_PARAM_COMMONINIT(pParam, FLTTYPE_SYSTEM, 0xf/*MJ_SYSTEM_DUPLICATE_OBJECT*/, 0, PreProcessing, 6);
			pSingleParam = (PSINGLE_PARAM) pParam->Params;

			SINGLE_PARAM_INIT_SID(pSingleParam);

// source process id
			SINGLE_PARAM_SHIFT(pSingleParam);
			SINGLE_PARAM_INIT_ULONG(pSingleParam, _PARAM_OBJECT_SOURCE_ID, GetProcessIDByHandle(SourceProcessHandle, NULL));

// target process id
			SINGLE_PARAM_SHIFT(pSingleParam);
			SINGLE_PARAM_INIT_ULONG(pSingleParam, _PARAM_OBJECT_DEST_ID, GetProcessIDByHandle(TargetProcessHandle, NULL));

// source handle
			SINGLE_PARAM_SHIFT(pSingleParam);
			SINGLE_PARAM_INIT_ULONG(pSingleParam, _PARAM_OBJECT_OBJECTHANDLE, (ULONG) SourceHandle);

// source handle type (всегда 3 для процесса)
			SINGLE_PARAM_SHIFT(pSingleParam);
			SINGLE_PARAM_INIT_ULONG(pSingleParam, _PARAM_OBJECT_TYPE, OBJECT_TYPE_PROCESS);

// source handle - хэндл процесса и здесь PID этого процесса
			SINGLE_PARAM_SHIFT(pSingleParam);
			SINGLE_PARAM_INIT_ULONG(pSingleParam, _PARAM_OBJECT_CLIENTID1, GetProcessIDByHandle(Internal_TargetHandle, NULL));
				// -----------------------------------------------------------------------------------------
			Verdict = FilterEvent(pParam, NULL);
	
			ExFreeToNPagedLookasideList(gpPL0x140, RequestData);
		}
	}

	if (_PASS_VERDICT(Verdict))
		return DuplicateHandle_Status;
	
	ZwClose(Internal_TargetHandle);

	return STATUS_ACCESS_DENIED;
}


//+ ------------------------------------------------------------------------------------------
VERDICT
CreateSectionCallbackPre(PSYS_CREATE_SECTION_CONTEXT SectionContext)
{
	VERDICT Verdict = Verdict_Default;
	BYTE *RequestData;
	
	if (!IsNeedFilterEvent(FLTTYPE_SYSTEM, 4/*MJ_SYSTEM_CREATE_SECTION*/, 0))
		return Verdict;

	RequestData = ExAllocateFromNPagedLookasideList(gpPL0x4000);
	if (RequestData == NULL)
		return Verdict;

	{
		PFILTER_EVENT_PARAM	pParam = (PFILTER_EVENT_PARAM) RequestData;
		PSINGLE_PARAM pSingleParam;

		FILTER_PARAM_COMMONINIT(pParam, FLTTYPE_SYSTEM, 4/*MJ_SYSTEM_CREATE_SECTION*/, 0, PreProcessing, 5);

		pSingleParam = (PSINGLE_PARAM) pParam->Params;

		SINGLE_PARAM_INIT_SID(pSingleParam);

		SINGLE_PARAM_SHIFT(pSingleParam);
		SINGLE_PARAM_INIT_ULONG(pSingleParam, _PARAM_OBJECT_ACCESSATTR, SectionContext->m_DesiredAccess);

		SINGLE_PARAM_SHIFT(pSingleParam);
		SINGLE_PARAM_INIT_NONE(pSingleParam, _PARAM_OBJECT_URL_W);
		pSingleParam->ParamSize = 0x3d00;
		
		ParamPlaceValueStrW(SectionContext->m_pFileName, (PWCHAR) pSingleParam->ParamValue, &pSingleParam->ParamSize);

		SINGLE_PARAM_SHIFT(pSingleParam);
		SINGLE_PARAM_INIT_ULONG(pSingleParam, _PARAM_OBJECT_ATTRIB, SectionContext->m_ObjAttributes);

		SINGLE_PARAM_SHIFT(pSingleParam);
		SINGLE_PARAM_INIT_ULONG(pSingleParam, _PARAM_OBJECT_PAGEPROTECTION, SectionContext->m_SectionPageProtection);

		Verdict = FilterEvent(pParam, NULL);
	}

	ExFreeToNPagedLookasideList(gpPL0x4000, RequestData);

	return Verdict;
}

NTSTATUS __stdcall
KLHookNtCreateSymbolicLinkObject (
	OUT PHANDLE ObjectHandle,
	IN ACCESS_MASK DesiredAccess, 
	IN POBJECT_ATTRIBUTES ObjectAttributes, 
	IN PUNICODE_STRING SubstituteString
	)
{
	NTSTATUS ntStatusRet;
	VERDICT Verdict = Verdict_Default;

	POBJECT_ATTRIBUTES ObjectAttributesTmp = CopyUserBuffer( ObjectAttributes, sizeof(OBJECT_ATTRIBUTES) );

	if (ObjectAttributesTmp)
	{
		if (ObjectAttributesTmp->Length == sizeof(OBJECT_ATTRIBUTES))
		{
			UNICODE_STRING uni;
			PUNICODE_STRING uni_p;
			uni.Buffer = L"\\device\\physicalmemory";
			uni.Length = sizeof(L"\\device\\physicalmemory") - sizeof(WCHAR);
			uni.MaximumLength = sizeof(L"\\device\\physicalmemory");

			uni_p = CopyUserBuffer( ObjectAttributesTmp->ObjectName, sizeof(UNICODE_STRING) );
			if (uni_p)
			{
				uni_p->MaximumLength = uni_p->Length;
				uni_p->Buffer = CopyUserBuffer( uni_p->Buffer, uni_p->Length );
				if (uni_p->Buffer)
				{
					if (!RtlCompareUnicodeString(uni_p, &uni, TRUE))
					{
						if (IsNeedFilterEvent(FLTTYPE_SYSTEM, 0x13/*MJ_SYSTEM_ACCESS_PHYSICAL_MEM*/, 0))
						{
							BYTE *RequestData = ExAllocateFromNPagedLookasideList(gpPL0x140);
							
							if (RequestData != NULL)
							{
								PFILTER_EVENT_PARAM	pParam = (PFILTER_EVENT_PARAM) RequestData;
								PSINGLE_PARAM pSingleParam;
								
								FILTER_PARAM_COMMONINIT(pParam, FLTTYPE_SYSTEM, 0x13/*MJ_SYSTEM_ACCESS_PHYSICAL_MEM*/, 0, PreProcessing, 2);
								pSingleParam = (PSINGLE_PARAM) pParam->Params;
								
								SINGLE_PARAM_INIT_SID(pSingleParam);

								SINGLE_PARAM_SHIFT(pSingleParam);
								SINGLE_PARAM_INIT_ULONG(pSingleParam, _PARAM_OBJECT_ACCESSATTR, DesiredAccess);

								
								Verdict = FilterEvent(pParam, NULL);
								
								ExFreeToNPagedLookasideList(gpPL0x140, RequestData);
							}
						}
					}

					ExFreePool( uni_p->Buffer );
				}
				
				ExFreePool( uni_p );
			}
		}

		ExFreePool( ObjectAttributesTmp );
	}

	if (!_PASS_VERDICT(Verdict))
		ntStatusRet = STATUS_ACCESS_DENIED;
	else
		ntStatusRet = NtCreateSymbolicLinkObject(ObjectHandle, DesiredAccess, ObjectAttributes, SubstituteString);

	return ntStatusRet;
}

#define SystemLoadAndCallImage				0x26 
typedef struct _SYSTEM_LOAD_AND_CALL_IMAGE
{
	UNICODE_STRING ModuleName;
} SYSTEM_LOAD_AND_CALL_IMAGE, *PSYSTEM_LOAD_AND_CALL_IMAGE;


NTSTATUS __stdcall
KLHookNtSetSystemInformation (
	IN ULONG SystemInformationClass,
    IN PVOID SystemInformation,
    IN ULONG Length
	)
{
	NTSTATUS ntStatusRet;
	VERDICT Verdict = Verdict_Default;

	if (SystemLoadAndCallImage == SystemInformationClass)
	{
		PUNICODE_STRING pModuleName = GetUserUniStr( (PUNICODE_STRING) SystemInformation );

		if (pModuleName)
		{
			BYTE *RequestData = NULL;

			if (pModuleName->Length < 0x900)
				RequestData = ExAllocateFromNPagedLookasideList(gpPL0x1000);

			if (RequestData)
			{
				PFILTER_EVENT_PARAM	pParam = (PFILTER_EVENT_PARAM) RequestData;
				PSINGLE_PARAM pSingleParam;

				FILTER_PARAM_COMMONINIT(pParam, FLTTYPE_SYSTEM, 0x1f/*MJ_SYSTEM_LOADANDCALL*/, 0, PreProcessing, 2);
				pSingleParam = (PSINGLE_PARAM) pParam->Params;

				SINGLE_PARAM_INIT_SID(pSingleParam);

				SINGLE_PARAM_SHIFT(pSingleParam);
				SINGLE_PARAM_INIT_NONE(pSingleParam, _PARAM_OBJECT_URL_W);

				memcpy(pSingleParam->ParamValue, pModuleName->Buffer, pModuleName->Length);
				((WCHAR*)pSingleParam->ParamValue)[pModuleName->Length / sizeof(WCHAR)] = 0;
				pSingleParam->ParamSize = pModuleName->Length + sizeof(WCHAR);

				Verdict = FilterEvent(pParam, NULL);
	
				ExFreeToNPagedLookasideList(gpPL0x1000, RequestData);
			}

			ExFreePool( pModuleName );	
		}
	}

	if (!_PASS_VERDICT(Verdict))
		ntStatusRet = STATUS_ACCESS_DENIED;
	else
		ntStatusRet = NtSetSystemInformation( SystemInformationClass, SystemInformation, Length );

	return ntStatusRet;
}

NTSTATUS __stdcall
KLHookOpenSection(OUT PHANDLE SectionHandle, IN ACCESS_MASK DesiredAccess, IN POBJECT_ATTRIBUTES ObjectAttributes)
{
	NTSTATUS ntStatusRet;
	VERDICT Verdict = Verdict_Default;

	KPROCESSOR_MODE PreviousMode = ExGetPreviousMode();

	POBJECT_ATTRIBUTES ObjectAttributesTmp = NULL;

	if (UserMode == PreviousMode)
		ObjectAttributesTmp = CopyUserBuffer( ObjectAttributes, sizeof(OBJECT_ATTRIBUTES) );

	if (ObjectAttributesTmp)
	{
		if ((DesiredAccess & SECTION_MAP_WRITE) && 
			ObjectAttributesTmp->Length == sizeof(OBJECT_ATTRIBUTES))
		{
			UNICODE_STRING uni;
			PUNICODE_STRING uni_p;
			uni.Buffer = L"\\device\\physicalmemory";
			uni.Length = sizeof(L"\\device\\physicalmemory") - sizeof(WCHAR);
			uni.MaximumLength = sizeof(L"\\device\\physicalmemory");
			uni_p = CopyUserBuffer( ObjectAttributesTmp->ObjectName, sizeof(UNICODE_STRING) );
			if (uni_p)
			{
				uni_p->MaximumLength = uni_p->Length;
				uni_p->Buffer = CopyUserBuffer( uni_p->Buffer, uni_p->Length );
				if (uni_p->Buffer)
				{
					if (!RtlCompareUnicodeString(uni_p, &uni, TRUE))
					{
						if (IsNeedFilterEvent(FLTTYPE_SYSTEM, 0x13/*MJ_SYSTEM_ACCESS_PHYSICAL_MEM*/, 0))
						{
							BYTE *RequestData = ExAllocateFromNPagedLookasideList(gpPL0x140);
							
							if (RequestData != NULL)
							{
								PFILTER_EVENT_PARAM	pParam = (PFILTER_EVENT_PARAM) RequestData;
								PSINGLE_PARAM pSingleParam;
								
								FILTER_PARAM_COMMONINIT(pParam, FLTTYPE_SYSTEM, 0x13/*MJ_SYSTEM_ACCESS_PHYSICAL_MEM*/, 0, PreProcessing, 1);
								pSingleParam = (PSINGLE_PARAM) pParam->Params;
								
								SINGLE_PARAM_INIT_SID(pSingleParam);
								
								Verdict = FilterEvent(pParam, NULL);
								
								ExFreeToNPagedLookasideList(gpPL0x140, RequestData);
							}
						}
					}
					ExFreePool( uni_p->Buffer );
				}
				ExFreePool( uni_p );
			}
		}

		ExFreePool( ObjectAttributesTmp );
	}

	if (!_PASS_VERDICT(Verdict))
		ntStatusRet = STATUS_ACCESS_DENIED;
	else
		ntStatusRet = NtOpenSection(SectionHandle, DesiredAccess, ObjectAttributes);

	return ntStatusRet;
}

PWCHAR NameShift(PWCHAR fullPathName);

NTSTATUS __stdcall
KLHookCreateSection (
	OUT PHANDLE SectionHandle,
	IN ACCESS_MASK DesiredAccess,
	IN POBJECT_ATTRIBUTES ObjectAttributes OPTIONAL, 
	IN PLARGE_INTEGER MaximumSize OPTIONAL,
	IN ULONG SectionPageProtection,
	IN ULONG AllocationAttributes,
	IN HANDLE FileHandle OPTIONAL
	)
{
	NTSTATUS ntStatusRet;
	NTSTATUS ntStatus;
	PFILE_OBJECT pFileObj = NULL;

	PSYS_CACHEITEM pCacheItem = NULL;

	VERDICT	Verdict = Verdict_Default;
	SYS_CREATE_SECTION_CONTEXT SectionContext;

	POBJECT_ATTRIBUTES ObjectAttributesTmp = NULL;

	SectionContext.m_pFileName = NULL;
	SectionContext.m_ObjAttributes = 0;
	SectionContext.m_SectionPageProtection = SectionPageProtection;

	DbPrint(DC_SYS, DL_SPAM, ("CreateSection FileHandle %#x DesiredAccess %#x\n", FileHandle, DesiredAccess));

	ObjectAttributesTmp = CopyUserBuffer( ObjectAttributes, sizeof(OBJECT_ATTRIBUTES) );
	if (ObjectAttributesTmp)
	{
		SectionContext.m_ObjAttributes = ObjectAttributesTmp->Attributes;
		ExFreePool( ObjectAttributesTmp );
	}

	if(FileHandle)
	{
		ntStatus = ObReferenceObjectByHandle(FileHandle, STANDARD_RIGHTS_REQUIRED, *IoFileObjectType, KernelMode, (VOID**)&pFileObj, NULL);
		if (NT_SUCCESS(ntStatus)) 
		{
			BOOLEAN bNameResolved = FALSE;
			pCacheItem = ExAllocateFromNPagedLookasideList(gpPSysFileNames);
			if(pCacheItem)
			{
				if (pFileObj->DeviceObject)
				{
					WCHAR DriveLetter;
					ULONG NameSize;
					IO_STATUS_BLOCK IoStatusBlock;
					PFILE_NAME_INFORMATION pFileNameInformation;

					DriveLetter = GetDeviceLetter(pFileObj->DeviceObject);
					if (DriveLetter == 0)
					{
						DriveLetter = L'?';
						DbgBreakPoint();
					}

					pFileNameInformation = (PFILE_NAME_INFORMATION) pCacheItem->m_Name;
					ntStatus = ZwQueryInformationFile(FileHandle, &IoStatusBlock, pFileNameInformation, _SYSQUERY_INFO_LEN - sizeof(SYS_CACHEITEM), FileNameInformation);
					if(NT_SUCCESS(ntStatus))
					{
						if (pFileNameInformation->FileNameLength != 0)
						{
							if ((_SYSQUERY_INFO_LEN - sizeof(SYS_CACHEITEM)) <= pFileNameInformation->FileNameLength)
							{
								pFileNameInformation->FileNameLength = Uwcslen(pFileNameInformation->FileName, 
									pFileNameInformation->FileNameLength / sizeof(WCHAR)) * sizeof(WCHAR);
							}
							pFileNameInformation->FileName[pFileNameInformation->FileNameLength / sizeof(WCHAR)] = 0;
							
							pCacheItem->m_Name[0] = DriveLetter;
							pCacheItem->m_Name[1] = L':';

							NameShift(pCacheItem->m_Name);

							if (DesiredAccess & SECTION_MAP_EXECUTE)
							{
								DbPrint(DC_SYS, DL_NOTIFY, ("CreateSection FileHandle %#x '%S' DesiredAccess %#x, Attr %#x, Protection %#x\n", 
									FileHandle, pCacheItem->m_Name, DesiredAccess, AllocationAttributes, SectionPageProtection));
							}

							bNameResolved = TRUE;
						}
					}
				}

				if (bNameResolved == FALSE)
				{
					DbgBreakPoint();
					ExFreeToNPagedLookasideList(gpPSysFileNames, pCacheItem);
					pCacheItem = NULL;
				}
			}

			ObDereferenceObject(pFileObj);
		}
	}

	if(FileHandle)
	{
		SectionContext.m_DesiredAccess = DesiredAccess;
		if (pCacheItem)
			SectionContext.m_pFileName = pCacheItem->m_Name;
		SectionContext.m_Status = 0;

		Verdict = CreateSectionCallbackPre(&SectionContext);
	}
	
	if (!_PASS_VERDICT(Verdict))
		ntStatusRet = STATUS_ACCESS_DENIED;
	else
	{
		ntStatusRet = NtCreateSection(SectionHandle, DesiredAccess, ObjectAttributes, MaximumSize, SectionPageProtection, AllocationAttributes, FileHandle);
		if (NT_SUCCESS(ntStatusRet) && (pCacheItem != NULL))
		{
			ProcInfoItem* pProcInfo = GetProcInfoByProcId((ULONG) PsGetCurrentProcessId());
			pCacheItem->m_Id = 0;
			if (pProcInfo)
				pCacheItem->m_Id = pProcInfo->m_ImagesLoadedCount;

			if (NT_SUCCESS( ntStatusRet ))
			{
				PHANDLE pHandleTmp = CopyUserBuffer( SectionHandle, sizeof(HANDLE) );
				if (pHandleTmp)
				{
					if (*pHandleTmp)
						SysNameCacheStore(gpSysContext, (ULONG) *pHandleTmp, pCacheItem, sizeof(pCacheItem) + (wcslen(pCacheItem->m_Name) + 1) * sizeof(WCHAR), '1boS');
					
					ExFreePool( pHandleTmp );
				}
			}
			DbPrint(DC_SYS, DL_SPAM, ("CreateSection SetionHandle %#x stored. Owner %#x\n", FileHandle, (ULONG) *SectionHandle, PsGetCurrentProcessId()));
		}

		if (pCacheItem && (DesiredAccess & SECTION_MAP_EXECUTE) && (AllocationAttributes & SEC_IMAGE))
			StoreExecuteName(pCacheItem->m_Name);

		SectionContext.m_Status = ntStatusRet;
		//ProcessCallbacks(KLYS_CREATE_SECTION, 0, &CallbackData, FALSE, &VerdictEx);

		if (!_PASS_VERDICT(Verdict))
		{
			DbgBreakPoint();
			ntStatusRet = STATUS_ACCESS_DENIED;
			ZwClose(*SectionHandle);
			*SectionHandle = NULL;
		}
	}

	if (pCacheItem != NULL)
		ExFreeToNPagedLookasideList(gpPSysFileNames, pCacheItem);
	
	return ntStatusRet;
}
//+ ------------------------------------------------------------------------------------------
VERDICT
TerminateProcessCallback(PSYS_TERMINATE_PROCESS pTerminateProcessContext, PROCESSING_TYPE ProcessingType)
{
	VERDICT Verdict = Verdict_Default;
	BYTE *RequestData;

	if (!IsNeedFilterEvent(FLTTYPE_SYSTEM, 0/*MJ_SYSTEM_TERMINATE_PROCESS*/, 0))
		return Verdict;

	RequestData = ExAllocateFromNPagedLookasideList(gpPL0x140);
	if (RequestData == NULL)
		return Verdict;

	{
		PFILTER_EVENT_PARAM	pParam = (PFILTER_EVENT_PARAM) RequestData;
		PSINGLE_PARAM pSingleParam;

		FILTER_PARAM_COMMONINIT(pParam, FLTTYPE_SYSTEM, 0/*MJ_SYSTEM_TERMINATE_PROCESS*/, 0, ProcessingType, 5);
		pSingleParam = (PSINGLE_PARAM) pParam->Params;

		SINGLE_PARAM_INIT_NONE(pSingleParam, _PARAM_OBJECT_URL);
		pSingleParam->ParamSize = KLMAX_PROCNAME;

		ParamPlaceValueStr(pTerminateProcessContext->m_ProcName, pSingleParam->ParamValue, &pSingleParam->ParamSize);
		
		SINGLE_PARAM_SHIFT(pSingleParam);
		SINGLE_PARAM_INIT_SID(pSingleParam);

		SINGLE_PARAM_SHIFT(pSingleParam);
		SINGLE_PARAM_INIT_ULONG(pSingleParam, _PARAM_OBJECT_ACCESSATTR, pTerminateProcessContext->m_AccessAttributes);
	
		SINGLE_PARAM_SHIFT(pSingleParam);
		SINGLE_PARAM_INIT_ULONG(pSingleParam, _PARAM_OBJECT_SOURCE_ID, pTerminateProcessContext->m_ProcessId);
	
		SINGLE_PARAM_SHIFT(pSingleParam);
		SINGLE_PARAM_INIT_ULONG(pSingleParam, _PARAM_OBJECT_DEST_ID, pTerminateProcessContext->m_DestProcId);

		Verdict = FilterEvent(pParam, NULL);
	}

	ExFreeToNPagedLookasideList(gpPL0x140, RequestData);
	
	return Verdict;
}


NTSTATUS __stdcall
KLHookTerminateProcess(IN HANDLE ProcessHandle, IN ULONG ProcessExitCode)
{
	NTSTATUS ntStatusRet;
	NTSTATUS ntStatus;

	SYS_TERMINATE_PROCESS TerminateProcessContext;
	VERDICT	Verdict = Verdict_Default;
	
	PEPROCESS	pProcObj = NULL;
	if (ProcessHandle == NULL)
		return NtTerminateProcess(ProcessHandle, ProcessExitCode);

	TerminateProcessContext.m_ProcessId = (ULONG) PsGetCurrentProcessId();
	if (ProcessHandle == NtCurrentProcess())
		TerminateProcessContext.m_AccessAttributes= -1;
	else
		TerminateProcessContext.m_AccessAttributes = 0;

	TerminateProcessContext.m_DestProcId = GetProcessIDByHandle(ProcessHandle, NULL);
	TerminateProcessContext.m_Status = 0;

	ntStatus = ObReferenceObjectByHandle(ProcessHandle, STANDARD_RIGHTS_REQUIRED, NULL, KernelMode, (VOID**)&pProcObj, NULL);
	if (NT_SUCCESS(ntStatus))
	{
		if (OBJECT_TYPE_PROCESS !=  ((DISPATCHER_HEADER *)pProcObj)->Type)
			strncpy(TerminateProcessContext.m_ProcName, (PCHAR)pProcObj + ProcessNameOffset, sizeof(TerminateProcessContext.m_ProcName));
		else
			strncpy(TerminateProcessContext.m_ProcName, "notprocess", sizeof(TerminateProcessContext.m_ProcName));
		
		ObDereferenceObject(pProcObj);
	}

	//если суицайдер мы и остались регионы (триды) то вызвать на эти триды терминате трид а потом идти дальше
	// проверить что терминате трид не ассинхронный

	if ((ULONG) ProcessHandle == (ULONG) -1)
	{
		if (TspExistRegisterdProcess((PVOID) TerminateProcessContext.m_DestProcId))
		{
			DbPrint(DC_SYS, DL_IMPORTANT, ("KLIF: Exists regions in TSP!!!\n"));
		}
	}
	
	ReleasePopupEventsForProcess(TerminateProcessContext.m_DestProcId, FLTTYPE_SYSTEM);

	Verdict = TerminateProcessCallback(&TerminateProcessContext, PreProcessing);
	
	if (!_PASS_VERDICT(Verdict))
		ntStatusRet = STATUS_ACCESS_DENIED;
	else
	{
		if (TerminateProcessContext.m_DestProcId < _countof(ProcIdtoHandle))
		{
			ProcInfoItem* pProcInfo = &ProcIdtoHandle[TerminateProcessContext.m_DestProcId];
			pProcInfo->m_ImagesLoadedCount = 0;
			pProcInfo->m_bKernel32Hooked = FALSE;
			
			_RESOURCE_LOCK_W(&gpSysContext->m_CacheResource);
			if (pProcInfo->m_pwchmainImage)
			{
				ExFreePool(pProcInfo->m_pwchmainImage);
				pProcInfo->m_pwchmainImage = 0;
			}
			_RESOURCE_UNLOCK(&gpSysContext->m_CacheResource);
		}
		
		DisconnectClientByProcID(TerminateProcessContext.m_DestProcId);
		CheckProc_RemoveFromList(TerminateProcessContext.m_DestProcId);

		ntStatusRet = NtTerminateProcess(ProcessHandle, ProcessExitCode);
		TerminateProcessContext.m_Status = ntStatusRet;
		Verdict = TerminateProcessCallback(&TerminateProcessContext, PostProcessing);
	}

	return ntStatusRet;
}

//+ ------------------------------------------------------------------------------------------
typedef enum {
	StateInitialized,
		StateReady,
		StateRunning,
		StateStandby,
		StateTerminated,
		StateWait,
		StateTransition,
		StateUnknown
} THREAD_STATE;

typedef struct _VM_COUNTERS {
	ULONG PeakVirtualSize;
	ULONG VirtualSize;
	ULONG PageFaultCount;
	ULONG PeakWorkingSetSize;
	ULONG WorkingSetSize;
	ULONG QuotaPeakPagedPoolUsage;
	ULONG QuotaPagedPoolUsage;
	ULONG QuotaPeakNonPagedPoolUsage;
	ULONG QuotaNonPagedPoolUsage;
	ULONG PagefileUsage;
	ULONG PeakPagefileUsage;
} VM_COUNTERS, *PVM_COUNTERS;

typedef struct _IO_COUNTERS {
	LARGE_INTEGER ReadOperationCount;
	LARGE_INTEGER WriteOperationCount;
	LARGE_INTEGER OtherOperationCount;
	LARGE_INTEGER ReadTransferCount;
	LARGE_INTEGER WriteTransferCount;
	LARGE_INTEGER OtherTransferCount;
} IO_COUNTERS, *PIO_COUNTERS;

typedef struct _SYSTEM_THREADS {
	LARGE_INTEGER KernelTime;
	LARGE_INTEGER UserTime;
	LARGE_INTEGER CreateTime;
	ULONG WaitTime;
	PVOID StartAddress;
	CLIENT_ID ClientId;
	KPRIORITY Priority;
	KPRIORITY BasePriority;
	ULONG ContextSwitchCount;
	THREAD_STATE State;
	KWAIT_REASON WaitReason;
} SYSTEM_THREADS, *PSYSTEM_THREADS;

typedef struct _SYSTEM_PROCESSES { // Information Class 5
	ULONG NextEntryDelta;
	ULONG ThreadCount;
	ULONG Reserved1[6];
	LARGE_INTEGER CreateTime;
	LARGE_INTEGER UserTime;
	LARGE_INTEGER KernelTime;
	UNICODE_STRING ProcessName;
	KPRIORITY BasePriority;
	ULONG ProcessId;
	ULONG InheritedFromProcessId;
	ULONG HandleCount;
	ULONG Reserved2[2];
	VM_COUNTERS VmCounters;
	IO_COUNTERS IoCounters; // Windows 2000 only
	SYSTEM_THREADS Threads[1];
} SYSTEM_PROCESSES, *PSYSTEM_PROCESSES;

//+ ------------------------------------------------------------------------------------------
#include <pshpack1.h>

typedef struct __PREFETCH_BLOCK
{
	ULONG	BlockID;
	ULONG	Unknown[2];
	PVOID	PrefetchData;
	ULONG	PrefetchDataSize;
} PREFETCH_BLOCK, *PPREFETCH_BLOCK;

#include <poppack.h>

typedef struct tag_PREFETCH_DATA {
	DWORD dwVersion;           // 0x00 // = 0x11
	DWORD dwMagic;             // 0x04 // = 0x43435341 = 'ASCC'
	DWORD dwDataSize;          // 0x08 // total size
	WCHAR wcImageName[30];     // 0x0C // application image name, "NTOSBOOT" for boot process
	DWORD dwPathHash;          // 0x48 // hash or 0xB00DFAAD for boot process
	DWORD dwUnknown1;          // 0x4C // 1?
	DWORD dwEndOfNamesBlock;   // 0x50 // 
	DWORD dwUnknown2;          // 0x54 // names count?
	DWORD dwNamesOffset;       // 0x58 // start of names block
	DWORD dwNamesCount;        // 0x5C
	DWORD dwVolumesInfoOffset; // 0x60
	DWORD dwVolumesInfoCount;  // 0x64
	DWORD dwUnknown6;          // 0x68 // 0x8E=142
	DWORD dwUnknown7;          // 0x6C // 0
	DWORD dwUnknown8;          // 0x70 // 0xF8D8F200
	DWORD dwUnknown9;          // 0x74 // 0xFFFFFFFF
	DWORD dwUnknown10;         // 0x78 // 0x5650
	DWORD dwUnknown11;         // 0x7C // 0x59FC
	DWORD dwUnknown12;         // 0x80 // 0x1937
	DWORD dwUnknown13;         // 0x84 // 0x3FE
	DWORD dwUnknown14;         // 0x88 // 0x539
	DWORD dwUnknown15;         // 0x8C // 0
	DWORD dwUnknown16;         // 0x90 // 0
	DWORD dwUnknown17;         // 0x94 // 0
	DWORD dwUnknown18;         // 0x98 // 0
	DWORD dwUnknown19;         // 0x9C // 0
	ULONGLONG ftTimeStamp;      // 0xA0
} PREFETCH_DATA;

typedef struct tag_PREFETCH_NAME_DATA {
	WORD wSize;
	WORD wFlags;
	WCHAR wcName[1];
} PREFETCH_NAME_DATA;

DWORD GetPrefetchSize(PREFETCH_DATA* pPrefecth, DWORD BlockSize)
{
	DWORD sizeRet = 0;

	if (BlockSize < sizeof(PREFETCH_DATA))
		return 0;

	if (pPrefecth->dwEndOfNamesBlock > BlockSize)
		return 0;

	sizeRet = pPrefecth->dwEndOfNamesBlock;

	return sizeRet;
}


// Gruzdev
PVOID g_PrefetchData = NULL;
ULONG g_PrefetchDataSize = 0;

NTSTATUS __stdcall
KLHookQuerySystemInformation(IN ULONG InfoClass,OUT PVOID SysInfo,IN ULONG SysInfoLen,OUT PULONG LenReturned OPTIONAL)
{
	NTSTATUS ntStatusRet;

	ntStatusRet = _ZwQuerySystemInformation(InfoClass, SysInfo, SysInfoLen, LenReturned);

	//5 -> SystemProcessesAndThreadsInformation
	if ((5 == InfoClass) && NT_SUCCESS(ntStatusRet))
	{
		if (IsNeedFilterEvent(FLTTYPE_SYSTEM, 0x9, 0))		// MJ_SYSTEM_QUERY_INFO
		{
			VERDICT Verdict = Verdict_NotFiltered;
			BYTE *RequestData;

			RequestData = ExAllocateFromNPagedLookasideList(gpPL0x140);
			if (RequestData != NULL)
			{
				BOOLEAN done = FALSE;
				PSYSTEM_PROCESSES sp = (PSYSTEM_PROCESSES) SysInfo;
				PSYSTEM_PROCESSES p = sp;

				PFILTER_EVENT_PARAM	pParam = (PFILTER_EVENT_PARAM) RequestData;
				PSINGLE_PARAM pSingleParam;

				FILTER_PARAM_COMMONINIT(pParam, FLTTYPE_SYSTEM, 0x9, 0, PreProcessing, 1);
				pSingleParam = (PSINGLE_PARAM) pParam->Params;

				__try
				{
					for (; !done; p = (PSYSTEM_PROCESSES)((PCHAR) p + p->NextEntryDelta))
					{
						SINGLE_PARAM_INIT_ULONG(pSingleParam, _PARAM_OBJECT_CLIENTID1, (ULONG) p->ProcessId);
						Verdict = FilterEvent(pParam, NULL);
						if (!_PASS_VERDICT(Verdict))
						{
							p->ThreadCount = 0;
						}
						
						done = p->NextEntryDelta == 0;		
					}
				}
				__except(EXCEPTION_EXECUTE_HANDLER)
				{
				}


				ExFreeToNPagedLookasideList(gpPL0x140, RequestData);
			}
		}
	}
	else if ((56 == InfoClass) && NT_SUCCESS(ntStatusRet) && !g_PrefetchData && KeGetCurrentIrql() < DISPATCH_LEVEL) // prefetch information
	{
		// Gruzdev
		PPREFETCH_BLOCK PrefetchBlock = (PPREFETCH_BLOCK)SysInfo;
		#define NTOSBOOT_STR	L"NTOSBOOT"

		ExAcquireFastMutex(&g_fmInternalSync);

		if (g_PrefetchData)
		{
			ExFreePool(g_PrefetchData);
			g_PrefetchData = NULL;
		}
		__try
		{
			if (PrefetchBlock->BlockID == 0x00000011)
			{
				PVOID PrefetchData = PrefetchBlock->PrefetchData;

				if ( memcmp((PCHAR)PrefetchData + 12, NTOSBOOT_STR, sizeof(WCHAR)*wcslen(NTOSBOOT_STR)) == 0 )
				{
					ULONG PrefetchDataSize = PrefetchBlock->PrefetchDataSize;

					if (LenReturned)
						PrefetchDataSize = *LenReturned;
					// recalc size

					PrefetchDataSize = GetPrefetchSize( (PREFETCH_DATA*) PrefetchData, PrefetchDataSize);
					if (PrefetchDataSize)
						g_PrefetchData = ExAllocatePoolWithTag(PagedPool, PrefetchDataSize, 'PBoS');
					
					if (g_PrefetchData)
					{
						memcpy(g_PrefetchData, PrefetchData, PrefetchDataSize);
						g_PrefetchDataSize = PrefetchDataSize;

						DbPrint(DC_SYS, DL_IMPORTANT, ("NtQuerySystemInformation: found NTOSBOOT prefetch block, Size = %d\n", PrefetchDataSize));
					}
					else
						DbPrint(DC_SYS, DL_ERROR, ("NtQuerySystemInformation: failed to allocate prefetch info buffer, Size = %d\n", PrefetchDataSize));
				}
			}
		}
		__except(EXCEPTION_EXECUTE_HANDLER)
		{
			if (g_PrefetchData)
			{
				ExFreePool(g_PrefetchData);
				g_PrefetchData = NULL;
			}
			
			DbPrint(DC_SYS, DL_ERROR, ("NtQuerySystemInformation: exception raised in prefetch info processing block\n"));
		}

		ExReleaseFastMutex(&g_fmInternalSync);
	}

	return ntStatusRet;
}

//+ ------------------------------------------------------------------------------------------

NTSTATUS
KLTerminateProcess(IN HANDLE ProcessHandle, IN ULONG ProcessExitCode)
{
	if (NtTerminateProcess != NULL)
		return NtTerminateProcess(ProcessHandle, ProcessExitCode);
	
	return STATUS_UNSUCCESSFUL;
}

//+ ----------------------------------------------------------------------------------------
NTSTATUS KLHookSuspendThread(IN PHANDLE ThreadHandle, OUT PULONG SuspendCount OPTIONAL)
{
	if (ThreadHandle != 0)
	{
		NTSTATUS ntStatus;
		PKTHREAD pkTh = NULL;

		ntStatus = ObReferenceObjectByHandle(ThreadHandle, STANDARD_RIGHTS_REQUIRED, NULL, KernelMode, &pkTh, NULL);
		if (NT_SUCCESS(ntStatus))
		{
			SuspendSubClient((ULONG) pkTh);
			ObDereferenceObject(pkTh);
		}
	}

	return NtSuspendThread(ThreadHandle, SuspendCount);
}

NTSTATUS KLHookResumeThread(PHANDLE ThreadHandle, PULONG SuspendCount)
{
	if (ThreadHandle != 0)
	{
		NTSTATUS ntStatus;
		PKTHREAD pkTh = NULL;

		ntStatus = ObReferenceObjectByHandle(ThreadHandle, STANDARD_RIGHTS_REQUIRED, NULL, KernelMode, &pkTh, NULL);
		if (NT_SUCCESS(ntStatus))
		{
			ResumeSubClient((ULONG) pkTh);
			ObDereferenceObject(pkTh);
		}
	}

	return NtResumeThread(ThreadHandle, SuspendCount);
}

/*void TryResolveProcessParameters(HANDLE ProcessHandle)
{
	NTSTATUS ntStatus;
	PROCESS_BASIC_INFORMATION ProcInfo;
	if (!_pfKeStackAttachProcess)
		return;

	if (NT_SUCCESS(ZwQueryInformationProcess(ProcessHandle, ProcessBasicInformation, &ProcInfo, sizeof(ProcInfo), NULL)))
	{
		PEPROCESS ProcObj = 0;
		if (ProcInfo.PebBaseAddress != 0)
		{
			impPROCESS_PARAMETERS** ppProcessParameters = (impPROCESS_PARAMETERS**) ((PBYTE)ProcInfo.PebBaseAddress + 0x10);

			ntStatus = ObReferenceObjectByHandle(ProcessHandle, 0, NULL, KernelMode, &ProcObj, NULL);
			if (NT_SUCCESS(ntStatus))
			{
				ULONG apc_state;
				_pfKeStackAttachProcess((PKPROCESS) ProcObj, &apc_state);

				DbgBreakPoint();
				
				_pfKeUnstackDetachProcess(&apc_state);
				ObDereferenceObject(ProcObj);
			}
		}
	}
}*/


BOOLEAN Late_System_Notifiers_Set = FALSE;

NTSTATUS KLHookCreateThread (
	OUT PHANDLE ThreadHandle,
	IN ACCESS_MASK DesiredAccess, 
	IN POBJECT_ATTRIBUTES ObjectAttributes OPTIONAL,
	IN HANDLE ProcessHandle,
	OUT PCLIENT_ID ClientID, 
	IN PCONTEXT Context,
	IN PUSER_STACK StackInfo,
	IN BOOLEAN CreateSuspended
	)
{
	NTSTATUS ntStatus = STATUS_SUCCESS;
	PVOID pEPROCESS;

	if (((ULONG)-1 != (ULONG) ProcessHandle) && ProcessHandle)
	{
		ULONG CurrentProcessId, DestProcId;
		CurrentProcessId = (ULONG) PsGetCurrentProcessId();
		DestProcId = GetProcessIDByHandle(ProcessHandle, NULL);
		
		if (DestProcId != CurrentProcessId)
		{
			DbPrint(DC_SYS, DL_NOTIFY, ("Create remote thread from %d (%#x) in %d (%#x)\n", CurrentProcessId, CurrentProcessId, DestProcId, DestProcId));
			
//			TryResolveProcessParameters(ProcessHandle);

			if (IsNeedFilterEvent(FLTTYPE_SYSTEM, 0x8, 0))		// MJ_SYSTEM_CREATE_REMOTE_THREAD
			{
				VERDICT Verdict = Verdict_NotFiltered;
				BYTE *RequestData;

				RequestData = ExAllocateFromNPagedLookasideList(gpPL0x140);
				if (RequestData != NULL)
				{
					PCONTEXT ContextTmp = NULL;
					PFILTER_EVENT_PARAM	pParam = (PFILTER_EVENT_PARAM) RequestData;
					PSINGLE_PARAM pSingleParam;
					ULONG ThreadEIP = 0;
					ULONG ThreadBaseAddr = 0;
					
					ContextTmp = CopyUserBuffer( Context, sizeof(CONTEXT) );
 					if (ContextTmp)
					{
						ThreadEIP = (ULONG) ContextTmp->Eip;
						ThreadBaseAddr = (ULONG) ContextTmp->Eax;

						ExFreePool( ContextTmp );
					}

					FILTER_PARAM_COMMONINIT(pParam, FLTTYPE_SYSTEM, 0x8, 0, PreProcessing, 4);		//MJ_SYSTEM_CREATE_REMOTE_THREAD
					//pParam->UnsafeValue = 1; // this is unsafe request
					pSingleParam = (PSINGLE_PARAM) pParam->Params;

					SINGLE_PARAM_INIT_SID(pSingleParam);

					SINGLE_PARAM_SHIFT(pSingleParam);
					SINGLE_PARAM_INIT_ULONG(pSingleParam, _PARAM_OBJECT_CLIENTID1, (ULONG) DestProcId);

					SINGLE_PARAM_SHIFT(pSingleParam);
					SINGLE_PARAM_INIT_ULONG(pSingleParam, _PARAM_OBJECT_ADDR1, ThreadEIP);

					SINGLE_PARAM_SHIFT(pSingleParam);
					SINGLE_PARAM_INIT_ULONG(pSingleParam, _PARAM_OBJECT_ADDR2, ThreadBaseAddr);

					// -----------------------------------------------------------------------------------------
					Verdict = FilterEvent(pParam, NULL);
				
					ExFreeToNPagedLookasideList(gpPL0x140, RequestData);

					if (!_PASS_VERDICT(Verdict))
					{
						DbgBreakPoint();
						ntStatus = STATUS_ACCESS_DENIED;
					}
				}
			}
		}
	}

	if (NT_SUCCESS(ntStatus))
	{
		ntStatus = NtCreateThread(ThreadHandle, DesiredAccess, ObjectAttributes, ProcessHandle, ClientID, 
			   Context, StackInfo, CreateSuspended);
	}

	if (NT_SUCCESS(ntStatus) && ClientID != 0)
	{
		PCLIENT_ID ClientIDTmp = CopyUserBuffer( ClientID, sizeof(CLIENT_ID));
		if (ClientIDTmp)
		{
			if (IsNeedFilterEvent(FLTTYPE_SYSTEM, 0x7/*MJ_SYSTEM_CREATE_THREAD*/, 0))
			{
				VERDICT Verdict = Verdict_NotFiltered;
				BYTE *RequestData;

				RequestData = ExAllocateFromNPagedLookasideList(gpPL0x140);
				if (RequestData != NULL)
				{
					PFILTER_EVENT_PARAM	pParam = (PFILTER_EVENT_PARAM) RequestData;
					PSINGLE_PARAM pSingleParam;

					FILTER_PARAM_COMMONINIT(pParam, FLTTYPE_SYSTEM, 0x7/*MJ_SYSTEM_CREATE_THREAD*/, 0, PostProcessing, 4);
					pSingleParam = (PSINGLE_PARAM) pParam->Params;

					SINGLE_PARAM_INIT_SID(pSingleParam);

					SINGLE_PARAM_SHIFT(pSingleParam);
					SINGLE_PARAM_INIT_ULONG(pSingleParam, _PARAM_OBJECT_CLIENTID1, (ULONG) ClientIDTmp->UniqueProcess);

					SINGLE_PARAM_SHIFT(pSingleParam);
					SINGLE_PARAM_INIT_ULONG(pSingleParam, _PARAM_OBJECT_CLIENTID2, (ULONG) ClientIDTmp->UniqueThread);

					SINGLE_PARAM_SHIFT(pSingleParam);
					SINGLE_PARAM_INIT_ULONG(pSingleParam, _PARAM_OBJECT_OBJECTHANDLE, (ULONG) ProcessHandle);

					// -----------------------------------------------------------------------------------------
					Verdict = FilterEvent(pParam, NULL);
				
					ExFreeToNPagedLookasideList(gpPL0x140, RequestData);
				}
			}

			ExFreePool( ClientIDTmp );
		}
	}

// Gruzdev---------------------------------------------------------------------------------------	
	pEPROCESS=PsGetCurrentProcess();
	if (pEPROCESS)
	{
// если это процесс csrss.exe (а он с user32.dll) и еще не прошел ни один удачный SetSystemNotifiers_Late()
		if (!Late_System_Notifiers_Set && _stricmp((PCHAR)pEPROCESS + ProcessNameOffset,"csrss.exe"))
			Late_System_Notifiers_Set = SetSystemNotifiers_Late();
	}
	else
		DbgPrint("KLHookCreateProcess:: PsGetCurrentProcess() failed\n");
//-----------------------------------------------------------------------------------------------	

	return ntStatus;
}

NTSTATUS
KLSetThreadContext (
	IN HANDLE hThread,
	IN const CONTEXT* lpContext
	)
{
	NTSTATUS ntStatusRet = STATUS_SUCCESS;
	VERDICT Verdict = Verdict_NotFiltered;

	BYTE *RequestData = NULL;
	
	ULONG DestProcessId = 0;
	ULONG DestThreadID = 0;

	if (hThread)
	{
		NTSTATUS ntStatus;
		PKTHREAD pkTh = NULL;

		ntStatus = ObReferenceObjectByHandle(hThread, STANDARD_RIGHTS_REQUIRED, NULL, KernelMode, &pkTh, NULL);
		if (NT_SUCCESS(ntStatus))
		{
			DestProcessId = (ULONG) _pfPsGetThreadProcessId(pkTh);
			DestThreadID = (ULONG) _pfPsGetThreadId(pkTh);
			ObDereferenceObject(pkTh);
		}
	}

	if (DestProcessId && DestThreadID)
	{
		RequestData = ExAllocateFromNPagedLookasideList(gpPL0x140);
		if (RequestData)
		{
			PCONTEXT ContextTmp = NULL;
			PFILTER_EVENT_PARAM	pParam = (PFILTER_EVENT_PARAM) RequestData;
			PSINGLE_PARAM pSingleParam;

			ULONG ThreadEIP = 0;
			ULONG ThreadBaseAddr = 0;
			
			ContextTmp = CopyUserBuffer( (PVOID) lpContext, sizeof(CONTEXT) );
			if (ContextTmp)
			{
				ThreadEIP = (ULONG) ContextTmp->Eip;
				ThreadBaseAddr = (ULONG) ContextTmp->Ebx;

				ExFreePool( ContextTmp );
			}

			
			FILTER_PARAM_COMMONINIT(pParam, FLTTYPE_SYSTEM, 0xd/*MJ_SYSTEM_SET_THREAD_CONTEXT*/, 0, PreProcessing, 5);
			pSingleParam = (PSINGLE_PARAM) pParam->Params;
			
			SINGLE_PARAM_INIT_SID(pSingleParam);
			
			SINGLE_PARAM_SHIFT(pSingleParam);
			SINGLE_PARAM_INIT_ULONG(pSingleParam, _PARAM_OBJECT_CLIENTID1, DestProcessId);

			SINGLE_PARAM_SHIFT(pSingleParam);
			SINGLE_PARAM_INIT_ULONG(pSingleParam, _PARAM_OBJECT_CLIENTID2, DestThreadID);

			SINGLE_PARAM_SHIFT(pSingleParam);
			SINGLE_PARAM_INIT_ULONG(pSingleParam, _PARAM_OBJECT_ADDR1, ThreadEIP);

			SINGLE_PARAM_SHIFT(pSingleParam);
			SINGLE_PARAM_INIT_ULONG(pSingleParam, _PARAM_OBJECT_ADDR2, ThreadBaseAddr);
			
			// -----------------------------------------------------------------------------------------
			Verdict = FilterEvent(pParam, NULL);

			ExFreeToNPagedLookasideList(gpPL0x140, RequestData);
		}
	}

	if (!_PASS_VERDICT(Verdict))
		ntStatusRet = STATUS_ACCESS_DENIED;
	else
		ntStatusRet = NtSetContextThread(hThread, lpContext);

	return ntStatusRet;
}

#define INVALID_HANDLE_VALUE ((HANDLE)-1)

HANDLE
NTAPI
KLHookUserSetWindowsHookEx(
	HANDLE Mod,
	PUNICODE_STRING ModuleName,
	DWORD ThreadId,
	int HookId,
	PVOID HookProc,
	BOOL Ansi
	)
{
	NTSTATUS ntStatus;
	BOOLEAN Standard_Processing = FALSE;

	VERDICT Verdict = Verdict_NotFiltered;

	do
	{
		ULONG cbModule_Name;
		PWCHAR wcSafeModuleName = NULL;

		if ( !ModuleName || !MmIsAddressValid(ModuleName) || !ModuleName->Buffer || !MmIsAddressValid(ModuleName->Buffer) )
		{
			Standard_Processing = TRUE;
			break;
		}
		
		if (!ModuleName->Length)
		{
			Standard_Processing = TRUE;
			break;
		}

		wcSafeModuleName = (PWCHAR) CopyUserBuffer( ModuleName->Buffer, ModuleName->Length );
		if ( !wcSafeModuleName )
		{
			Standard_Processing = TRUE;
			break;
		}

		if (IsNeedFilterEvent(FLTTYPE_SYSTEM, 0x10/*MJ_SYSTEM_USER_SET_WINDOWS_HOOK_EX*/, 0))
		{
			BYTE *RequestData;

			RequestData = ExAllocateFromNPagedLookasideList(gpPL0x1000);
			if (RequestData != NULL)
			{
				PFILTER_EVENT_PARAM	pParam = (PFILTER_EVENT_PARAM) RequestData;
				PSINGLE_PARAM pSingleParam;

				PUNICODE_STRING pModule_Name;

				FILTER_PARAM_COMMONINIT(pParam, FLTTYPE_SYSTEM, 0x10/*MJ_SYSTEM_USER_SET_WINDOWS_HOOK_EX*/, 0, PreProcessing, 4);
				pSingleParam = (PSINGLE_PARAM) pParam->Params;

				SINGLE_PARAM_INIT_SID(pSingleParam);

				SINGLE_PARAM_SHIFT(pSingleParam);
				SINGLE_PARAM_INIT_ULONG(pSingleParam, _PARAM_OBJECT_CLIENTID1, HookId);

				SINGLE_PARAM_SHIFT(pSingleParam);
				SINGLE_PARAM_INIT_ULONG(pSingleParam, _PARAM_OBJECT_CLIENTID2, ThreadId);

// имя модуля hModule----------------------------------------------------------------------------
				SINGLE_PARAM_SHIFT(pSingleParam);
				SINGLE_PARAM_INIT_NONE(pSingleParam, _PARAM_OBJECT_URL_W);

				cbModule_Name = min(0x940, ModuleName->Length);
				memcpy(pSingleParam->ParamValue, wcSafeModuleName, cbModule_Name);
				((WCHAR*)pSingleParam->ParamValue)[cbModule_Name / sizeof(WCHAR)] = 0;
				cbModule_Name += sizeof(WCHAR);
				pSingleParam->ParamSize = cbModule_Name;

				DbPrint(DC_SYS, DL_IMPORTANT, ("SetWindowsHookEx: Pid 0x%x, Module = %S \n", PsGetCurrentProcessId(), (WCHAR*)pSingleParam->ParamValue));
//-----------------------------------------------------------------------------------------------

				Verdict = FilterEvent(pParam, NULL);
	
				ExFreeToNPagedLookasideList(gpPL0x1000, RequestData);
			}

			ExFreePool( wcSafeModuleName );
		}
	} while (FALSE);

	if (Standard_Processing || _PASS_VERDICT(Verdict))
		return NtUserSetWindowsHookEx(Mod, ModuleName, ThreadId, HookId, HookProc, Ansi);
	else
		return INVALID_HANDLE_VALUE;

}

// Gruzdev ----------------------------------------------------------------------------------------
BOOLEAN
NTAPI
KLHookUserPostMessage(
	IN HANDLE hWnd,
    IN ULONG Msg,
    IN ULONG wParam,
    IN ULONG lParam
	)
{
	VERDICT Verdict = Verdict_NotFiltered;

//+ Gruzdev
// вердикт на межпроцессное сообщение
	Verdict = PreSendPostMessage(hWnd, Msg, wParam, lParam);
	if (!_PASS_VERDICT(Verdict))
		return FALSE;

	return NtUserPostMessage(
			hWnd,
			Msg,
			wParam,
			lParam
			);
}

/*
 * dwFlags for SetWinEventHook
 */
#define WINEVENT_OUTOFCONTEXT   0x0000  // Events are ASYNC
#define WINEVENT_SKIPOWNTHREAD  0x0001  // Don't call back for events on installer's thread
#define WINEVENT_SKIPOWNPROCESS 0x0002  // Don't call back for events on installer's process
#define WINEVENT_INCONTEXT      0x0004  // Events are SYNC, this causes your dll to be injected into every process

ULONG NTAPI KLHookUserSetWinEventHook(
    IN ULONG           eventMin,
    IN ULONG           eventMax,
    IN HANDLE          hmodWinEventProc,
    IN PUNICODE_STRING pstrLib OPTIONAL,
    IN PVOID           pfnWinEventProc,
    IN ULONG           idEventProcess,
    IN ULONG           idEventThread,
    IN ULONG           dwFlags)
{
	VERDICT Verdict = Verdict_NotFiltered;

	if ((dwFlags & WINEVENT_INCONTEXT) && hmodWinEventProc)
	{
		BOOLEAN bResult;
		WCHAR FullModuleName[260];

// trying to inject DLL...
		bResult = LookupModuleNameByBase(hmodWinEventProc, FullModuleName, sizeof(FullModuleName), NULL, 0);
		if (bResult)
		{
			if (IsNeedFilterEvent(FLTTYPE_SYSTEM, 0x19/*MJ_SYSTEM_USER_SETWINEVENTHOOK*/, 0))
			{
				PFILTER_EVENT_PARAM pParam;

				pParam = (PFILTER_EVENT_PARAM)ExAllocateFromNPagedLookasideList(gpPL0x1000);
				if (pParam)
				{
					PSINGLE_PARAM pSingleParam;

					FILTER_PARAM_COMMONINIT(pParam, FLTTYPE_SYSTEM, 0x19/*MJ_SYSTEM_USER_SETWINEVENTHOOK*/, 0, PreProcessing, 2);
					pSingleParam = (PSINGLE_PARAM)pParam->Params;

					SINGLE_PARAM_INIT_SID(pSingleParam);

					SINGLE_PARAM_SHIFT(pSingleParam);
					SINGLE_PARAM_INIT_NONE(pSingleParam, _PARAM_OBJECT_URL_W);
					pSingleParam->ParamSize = sizeof(WCHAR)*wcslen(FullModuleName);
					memcpy(pSingleParam->ParamValue, FullModuleName, pSingleParam->ParamSize);
					((PWCHAR)pSingleParam->ParamValue)[pSingleParam->ParamSize/sizeof(WCHAR)] = 0;
					pSingleParam->ParamSize += sizeof(WCHAR);

					Verdict = FilterEvent(pParam, NULL);

					ExFreeToNPagedLookasideList(gpPL0x1000, pParam);
				}

			}
		}

	}

	if (_PASS_VERDICT(Verdict))
		return NtUserSetWinEventHook(
					eventMin,
					eventMax,
					hmodWinEventProc,
					pstrLib,
					pfnWinEventProc,
					idEventProcess,
					idEventThread,
					dwFlags);


	return 0;
}

NTSTATUS
NTAPI
KLHookSetInformationFile(
						 IN HANDLE FileHandle,
						 OUT PIO_STATUS_BLOCK IoStatusBlock,
						 IN PVOID FileInformation,
						 IN ULONG FileInformationLength,
						 IN FILE_INFORMATION_CLASS FileInformationClass
						 )
{
	NTSTATUS			ntStatus;
	
	ntStatus = NtSetInformationFile(
		FileHandle,
		IoStatusBlock,
		FileInformation,
		FileInformationLength,
		FileInformationClass
		);
	
	if (NT_SUCCESS(ntStatus) && FileInformationClass == FileBasicInformation && 
		((PFILE_BASIC_INFORMATION)FileInformation)->LastWriteTime.QuadPart &&
		// AVP 5 correction
		!IsInvisibleThread((ULONG)PsGetCurrentThreadId()))
	{
		FIDBOX_REQUEST_GET	Fidbox_Request;
		FIDBOX_REQUEST_DATA Fidbox_Result;
		ULONG				Ret_Size;
		
		Fidbox_Request.m_hFile = (ULONG)FileHandle;
		// if file indicated by FileHandle is not found in the fidbox - initial count is 0 (m_Buffer
		// is filled with zeros)
		if (!NT_SUCCESS(FidBox2_GetData(&Fidbox_Request, &Fidbox_Result, &Ret_Size)))
			return ntStatus;
		
		DbPrint(DC_SYS, DL_NOTIFY, ("Modification time setting hooked: FileHandle == 0x%x, Count == %d\n", 
			FileHandle, *((PULONG)Fidbox_Result.m_Buffer)));
		//---------------------------------------------------------------------------------------------
		
		// increment the counter
		(*((PULONG)Fidbox_Result.m_Buffer))++;
		
		FidBox2_SetData(&Fidbox_Result);
	}
	
	return ntStatus;
}

//--------------------------------------------------------------------------------------------

//-------------------------------------------------------------------------------------------------

//+ ------------------------------------------------------------------------------------------
#define lcase(_wc) (_wc >= 'A' ? (_wc <= 'Z' ? _wc | 0x20 : _wc) : _wc)
int __cdecl my_wcsincmp(const wchar_t * src, const wchar_t * dst, int size)
{
	int ret = 0 ;
	while (size)
	{
		wchar_t c1 = lcase(*src);
		wchar_t c2 = lcase(*dst);
		if (c1 != c2)
			return 1;
		size--;

		src++;
		dst++;
	}

	return 0;
}

DWORD LINPatchRet;
ULONG PatchedCodeLIN[]={0x90909090,0x90909090,0x25ff9090,(ULONG)&LINPatchRet};
PVOID PatchedCodeLINF=PatchedCodeLIN;

void __stdcall CheckLoadImage(PUNICODE_STRING pFileName, PVOID BaseAddress)
{
	if (pFileName && pFileName->Buffer && pFileName->Length >= 24)
	{
		if (!my_wcsincmp(L"kernel32.dll", pFileName->Buffer + (pFileName->Length / sizeof(WCHAR)) - 12, 12))
			R3Hook(L"kernel32.dll", BaseAddress);
	}
}

VOID __declspec(naked) LINCallback(VOID)
{
	__asm
	{
		pushfd;
		pushad;
		
	}
	__asm
	{
		mov eax, [ebp + 0x8]	// control area
		mov eax, [eax + 0x24]	// fileobject
		add eax, 0x030			// filename unicode string (check 0)

		mov ebx, [ebp + 0x10]	// pointer on base address
		mov ebx, [ebx]			// base address

		push ebx
		push eax

		call CheckLoadImage
	}
	
	__asm
	{
		popad;
		popfd;
		jmp [PatchedCodeLINF];
	}
}

BOOLEAN
MakeLoadImageNotifyRoutine()
{
	DWORD SSPos;

	// one byte :) different
	static BYTE asmdata[] = {0x83,0x7D,0xD8,0x00, 0x0F,0x8C,0xF6,0x01,0x00,0x00,0x8B,0x45,0x08,0x8B,0x00,
							0x80,0x78,0x3E,0x00,0x74,0x22,0x66,0x8B,0x48,0x3C,0xB8,0x2C,0x00,0xFE,0x7F,
							0x66,0x3B,0x08,0x72,0x0A,0xB8,0x2E,0x00,0xFE,0x7F,0x66,0x3B,0x08,0x76,0x0A, 
							0xB8,0x0E,0x00,0x00,0x40};

	static BYTE asmdata_[] ={0x83,0x7D,0xD8,0x00,0x0F,0x8C,0xFF,0x01,0x00,0x00,0x8B,0x45,0x08,0x8B,0x00,
							0x80,0x78,0x3E,0x00,0x74,0x22,0x66,0x8B,0x48,0x3C,0xB8,0x2C,0x00,0xFE,0x7F,
							0x66,0x3B,0x08,0x72,0x0A,0xB8,0x2E,0x00,0xFE,0x7F,0x66,0x3B,0x08,0x76,0x0A,
							0xB8,0x0E,0x00,0x00,0x40};

	
	BYTE			PatchData[10];
	ULONG			PatchDataSize;
	ULONG			oldirql;
	BOOLEAN			OldProtect;
	PVOID			LoadImageCallbackPatchAddr;
	PIMAGE_DOS_HEADER pDosHeader;
	PIMAGE_NT_HEADERS pNTHeader;
	PIMAGE_SECTION_HEADER SectHeader;
	PVOID FirstSectStart;
	ULONG FirstSectSize;

	pDosHeader = (PIMAGE_DOS_HEADER)BaseOfNtOsKrnl;
	if(pDosHeader->e_magic != IMAGE_DOS_SIGNATURE)
	{
		DbPrint(DC_TSP,DL_ERROR, ("mz header not found\n"));
		return FALSE;
	}
	
	pNTHeader=MakePtr(PIMAGE_NT_HEADERS,BaseOfNtOsKrnl,pDosHeader->e_lfanew);
	if(pNTHeader->Signature != IMAGE_NT_SIGNATURE)
	{
		DbPrint(DC_TSP,DL_ERROR, ("PE header not found\n"));
		return FALSE;
	}
	
	SectHeader = IMAGE_FIRST_SECTION(pNTHeader);
	FirstSectStart=(BYTE*)BaseOfNtOsKrnl + SectHeader->VirtualAddress;
	FirstSectSize=SectHeader->SizeOfRawData;
	
	LoadImageCallbackPatchAddr = NULL;
	try
	{
		SSPos = GetSubstringPosBM((BYTE*) FirstSectStart, FirstSectSize, asmdata,sizeof(asmdata));
		if (-1 == SSPos)
		{
			DbgBreakPoint();
			SSPos = GetSubstringPosBM((BYTE*) FirstSectStart, FirstSectSize, asmdata_, sizeof(asmdata_));
		}

		if(-1 != SSPos)
		{
			LoadImageCallbackPatchAddr = (PVOID)((DWORD)FirstSectStart + SSPos);
			LoadImageCallbackPatchAddr = (BYTE*) LoadImageCallbackPatchAddr + sizeof(asmdata) + 8;
			PatchDataSize = 5;
		}
	}
	except(EXCEPTION_EXECUTE_HANDLER)
	{
	}
	if(LoadImageCallbackPatchAddr == NULL)
	{
		DbPrint(DC_TSP,DL_ERROR, ("GetSwapContext failed\n"));
		return FALSE;
	}
	
	memset(PatchData,0x90,sizeof(PatchData));
	PatchData[0] = 0xe9;
	*(ULONG*)(PatchData+1)=(DWORD)LINCallback - (DWORD)LoadImageCallbackPatchAddr - 5;
	LINPatchRet = (ULONG)LoadImageCallbackPatchAddr + PatchDataSize;
	memcpy(PatchedCodeLIN, LoadImageCallbackPatchAddr, PatchDataSize);
	
	if(MyVirtualProtect(LoadImageCallbackPatchAddr,TRUE,&OldProtect))
	{
		KIRQL OldIrql;
		KeRaiseIrql(DISPATCH_LEVEL, &OldIrql );

		memcpy(LoadImageCallbackPatchAddr,&PatchData,PatchDataSize);

		KeLowerIrql( OldIrql );

		MyVirtualProtect(LoadImageCallbackPatchAddr,OldProtect,&OldProtect);
		
		return TRUE;
	}
	return FALSE;
}

//////////////////////////////////////////////////////////////////////////
typedef struct _CHECKLOCK_LISTITEM
{
    IMPLIST_ENTRY	m_List;
    PFILE_OBJECT	m_FileObject;
	HANDLE			m_ThreadId;
} CHECKLOCK_LISTITEM, *PCHECKLOCK_LISTITEM;

IMPLIST_ENTRY	gCheckLockList;

BOOLEAN IsSkipLock(PIRP Irp)
{
	BOOLEAN bSkipLock = FALSE;
	PIO_STACK_LOCATION	currentIrpStack = IoGetCurrentIrpStackLocation(Irp);
	PFILE_OBJECT		fileObject      = currentIrpStack->FileObject;

	DbPrint(DC_SYS, DL_NOTIFY, ("IsSkipLock irp 0x%x, fo 0x%x\n", Irp, fileObject));

	if (fileObject)
	{
		if (!IsListEmpty(&gCheckLockList))
		{
			PCHECKLOCK_LISTITEM LockItemTmp;
			PIMPLIST_ENTRY Flink;
			ExAcquireFastMutex(&g_fmInternalSync);

			if (!IsListEmpty(&gCheckLockList))
			{
				Flink = gCheckLockList.Flink;
	
				while(Flink != &gCheckLockList)
				{
					LockItemTmp = CONTAINING_RECORD(Flink, CHECKLOCK_LISTITEM, m_List);
				
					Flink = Flink->Flink;

					if ((LockItemTmp->m_FileObject == fileObject) &&
						(LockItemTmp->m_ThreadId == PsGetCurrentThreadId()))
					{
						bSkipLock = TRUE;
						break;
					}
				}
			}

			ExReleaseFastMutex(&g_fmInternalSync);
		}
	}
	
	return bSkipLock;
}

VOID
AddSkipLockItem(PFILE_OBJECT pFileObject)
{
	PCHECKLOCK_LISTITEM pItem = ExAllocatePoolWithTag(NonPagedPool, sizeof(CHECKLOCK_LISTITEM), 'LbOS');
	if (pItem)
	{
		ExAcquireFastMutex(&g_fmInternalSync);

		pItem->m_FileObject = pFileObject;
		pItem->m_ThreadId = PsGetCurrentThreadId();
		_impInsertTailList(&gCheckLockList, &pItem->m_List);

		ExReleaseFastMutex(&g_fmInternalSync);
	}
}

VOID
DelSkipLockItem(PFILE_OBJECT pFileObject)
{
	ExAcquireFastMutex(&g_fmInternalSync);
	if (!IsListEmpty(&gCheckLockList))
	{
		PCHECKLOCK_LISTITEM LockItemTmp;
		PIMPLIST_ENTRY Flink;

		Flink = gCheckLockList.Flink;

		while(Flink != &gCheckLockList)
		{
			LockItemTmp = CONTAINING_RECORD(Flink, CHECKLOCK_LISTITEM, m_List);
		
			Flink = Flink->Flink;

			if (LockItemTmp->m_FileObject == pFileObject)
			{
				_impRemoveEntryList(&LockItemTmp->m_List);
				ExFreePool(LockItemTmp);
			}
		}
	}

	ExReleaseFastMutex(&g_fmInternalSync);
}

DWORD FSRtlCheckLockAccessReadNextOp = 0;
VOID __declspec(naked) NakedFSRtlCheckLockAccessRead(VOID)
{
	__asm
	{
		mov     edi,edi
		push    ebp
		mov     ebp,esp

		// call menia
		//esli return
		//{
			push [ebp+0xc]		;
			call IsSkipLock		;
			cmp al, 1			;
			jnz _chla_call_original					;

			pop ebp				;
			mov al, 1			;
			ret 8				;
		//}
_chla_call_original:
		jmp [FSRtlCheckLockAccessReadNextOp];
	}
}

// NakedFSRtlCheckLockAccessRead & NakedFSRtlCheckLockAccessRead2 mogno svernut v ondu function
VOID
MakeCheckLockAccess()
{
	BYTE PatchData[5];
	BYTE CheckData[5] = {0x8b, 0xff, 0x55, 0x8b, 0xec};

	DWORD* ptr = (DWORD*)(&PatchData[1]);
	DWORD pOriginalFunc = (DWORD) FsRtlCheckLockForReadAccess;
	DWORD pPatchFunc = (DWORD) NakedFSRtlCheckLockAccessRead;

	if (!memcmp((void*) pOriginalFunc, CheckData, sizeof(CheckData)))
	{
		BOOLEAN OldProtect;

		PatchData[0] = 0xe9;	// jmp
		*ptr = (DWORD)(pPatchFunc - pOriginalFunc - 5);
		FSRtlCheckLockAccessReadNextOp = pOriginalFunc + sizeof(PatchData);

		if (MyVirtualProtect((void*) pOriginalFunc, TRUE, &OldProtect))
		{
			KIRQL OldIrql;
			KeRaiseIrql(DISPATCH_LEVEL, &OldIrql );
			
			memcpy((void*) pOriginalFunc, PatchData, sizeof(PatchData));

			KeLowerIrql( OldIrql );

			MyVirtualProtect((void*) pOriginalFunc, OldProtect, &OldProtect);
		}
	}
}

//////////////////////////////////////////////////////////////////////////
typedef struct _PATCHNAME_LISTITEM
{
    IMPLIST_ENTRY	m_List;
	PUNICODE_STRING	m_UniNameOrig;
	PUNICODE_STRING	m_UniNameShadow;
} PATCHNAME_LISTITEM, *PPATCHNAME_LISTITEM;

IMPLIST_ENTRY	gPatchNameList;

void CheckNameAndPatch(PUNICODE_STRING pUniName)
{
	if (IsListEmpty(&gPatchNameList))
		return;

	ExAcquireFastMutex(&g_fmInternalSync);

	if (!IsListEmpty(&gPatchNameList))
	{
		PPATCHNAME_LISTITEM PatchNameTmp;
		PIMPLIST_ENTRY Flink;
	
		Flink = gPatchNameList.Flink;

		while(Flink != &gPatchNameList)
		{
			PatchNameTmp = CONTAINING_RECORD(Flink, PATCHNAME_LISTITEM, m_List);
			Flink = Flink->Flink;
			
			if (pUniName->Length == PatchNameTmp->m_UniNameShadow->Length)
			{
				if (!memcmp(pUniName->Buffer, PatchNameTmp->m_UniNameShadow->Buffer, pUniName->Length))
					memcpy(pUniName->Buffer, PatchNameTmp->m_UniNameOrig->Buffer, pUniName->Length);
			}
		}
	}

	ExReleaseFastMutex(&g_fmInternalSync);
}

NTSTATUS
DoDirectOpen (
	PHANDLE pHandle,
	PWSTR pOrigName,
	USHORT OrigNameLenInTchar
	)
{
	NTSTATUS ntStatus = STATUS_INVALID_PARAMETER;

	USHORT len;
	USHORT reallen;
	USHORT pos = 0;

	USHORT buflen;

	DWORD slashcount = 0;

	WCHAR timetmp;
	
	PUNICODE_STRING pUniOrig = NULL;
	PUNICODE_STRING pUniShadow = NULL;

	PPATCHNAME_LISTITEM pItem;

	if (!OrigNameLenInTchar)
		return ntStatus;
	
	len = (USHORT) Uwcslen(pOrigName, OrigNameLenInTchar);
	
	if (!len) // in TCHAR without 0
		return STATUS_INVALID_PARAMETER;
	
	while (pos < len)
	{
		if (pOrigName[pos++] == ':')
			break;
	}
	
	if (pos >= len)
	{
		DbgBreakPoint();
		return STATUS_INVALID_PARAMETER;
	}

	reallen = len - pos;
	buflen = (reallen) * sizeof(WCHAR);

	pUniOrig = ExAllocatePoolWithTag(PagedPool, sizeof(UNICODE_STRING) + buflen, 'SBOs');
	pUniShadow = ExAllocatePoolWithTag(PagedPool, sizeof(UNICODE_STRING) + buflen, 'SBOs');
	if (!pUniOrig || !pUniShadow)
	{
		if (pUniOrig)
			ExFreePool(pUniOrig);
		
		if (pUniShadow)
			ExFreePool(pUniShadow);

		return STATUS_MEMORY_NOT_ALLOCATED;
	}

	pUniOrig->Buffer = (PWSTR) (pUniOrig + 1);
	pUniOrig->MaximumLength = buflen;
	pUniOrig->Length = buflen;

	memcpy(pUniOrig->Buffer, pOrigName + pos, buflen);
	memcpy(pUniShadow, pUniOrig, sizeof(UNICODE_STRING) + buflen);
	pUniShadow->Buffer = (PWSTR) (pUniShadow + 1);

	len = len - pos - 1;
	while (len)
	{
		if (pUniShadow->Buffer[len] == '\\')
		{
			slashcount++;
			if (2 == slashcount)
				break;
		}
		else
		{
			timetmp = (WCHAR) GetCurTimeFast();
			pUniShadow->Buffer[len] = 'a' + (timetmp & 0x1f);
			
			if (pUniShadow->Buffer[len] > 'z')
				pUniShadow->Buffer[len] -= 15;
		}

		len--;
	}

	pItem = ExAllocatePoolWithTag(PagedPool, sizeof(PATCHNAME_LISTITEM), 'lBOs');
	if (pItem)
	{
		pItem->m_UniNameOrig = pUniOrig;
		pItem->m_UniNameShadow = pUniShadow;

		ExAcquireFastMutex(&g_fmInternalSync);
		_impInsertTailList(&gPatchNameList, &pItem->m_List);
		ExReleaseFastMutex(&g_fmInternalSync);

		// create file
		{
			UNICODE_STRING		LogFileName;
			OBJECT_ATTRIBUTES	objectAttributes;
			IO_STATUS_BLOCK		ioStatus;
	
			memcpy(pOrigName + pos, pUniShadow->Buffer, reallen * sizeof(WCHAR));

			RtlInitUnicodeString(&LogFileName, pOrigName);
			InitializeObjectAttributes(&objectAttributes, &LogFileName, OBJ_CASE_INSENSITIVE, NULL, NULL);
			
			ntStatus = ZwCreateFile(pHandle, FILE_READ_DATA | SYNCHRONIZE, &objectAttributes, &ioStatus, NULL, 0, 
				FILE_SHARE_READ | FILE_SHARE_WRITE | FILE_SHARE_DELETE, FILE_OPEN, FILE_SYNCHRONOUS_IO_NONALERT, NULL, 0);

			if (!NT_SUCCESS(ntStatus))
			{
				if (pOrigName[1] == L'\\')
				{
					pOrigName[1] = L'?';
					ntStatus = ZwCreateFile(pHandle, FILE_READ_DATA | SYNCHRONIZE, &objectAttributes, &ioStatus, NULL, 0, 
						FILE_SHARE_READ | FILE_SHARE_WRITE | FILE_SHARE_DELETE, FILE_OPEN, FILE_SYNCHRONOUS_IO_NONALERT, NULL, 0);
				}
			}
		}
		
		if (NT_SUCCESS(ntStatus))
		{
			PFILE_OBJECT pFileObject = NULL;
			NTSTATUS ntStatusTmp = ObReferenceObjectByHandle(*pHandle, 0, *IoFileObjectType, KernelMode, &pFileObject, NULL);
			if (NT_SUCCESS(ntStatusTmp))
			{
				PDEVICE_OBJECT pDevice = IoGetRelatedDeviceObject(pFileObject);
				if (FILE_DEVICE_NETWORK_FILE_SYSTEM == pDevice->DeviceType)
				{
					ntStatus = STATUS_NOT_SUPPORTED;
				
					ZwClose(*pHandle);

					*pHandle = NULL;
				}

				ObDereferenceObject(pFileObject);
			}
		}

		ExAcquireFastMutex(&g_fmInternalSync);
		_impRemoveEntryList(&pItem->m_List);
		ExReleaseFastMutex(&g_fmInternalSync);

		ExFreePool(pItem);
	}

	ExFreePool(pUniOrig);
	ExFreePool(pUniShadow);
		
	return ntStatus;
}

extern DWORD NtfsStart;
extern DWORD NtfsSize;
void IoIsOperationSyncCheckPatch(DWORD RetAddr, PIRP Irp)
{
	PIO_STACK_LOCATION currentIrpStack;

	if ((RetAddr < NtfsStart) && (RetAddr >  NtfsStart + NtfsSize))
		return;

	if (!Irp)
		return;

	if (IO_TYPE_IRP != Irp->Type)
	{
		DbgBreakPoint();
		return;
	}

	currentIrpStack = IoGetCurrentIrpStackLocation(Irp);
	if (IRP_MJ_CREATE == currentIrpStack->MajorFunction)
	{
		PFILE_OBJECT fileObject = currentIrpStack->FileObject;

		if (fileObject && fileObject->FileName.Length && fileObject->FileName.Buffer)
		{
			CheckNameAndPatch(&fileObject->FileName);
		}
	}
}

DWORD callIoIsOperationSync = 0;

VOID
__declspec(naked) NakedIoIsOperationSync( VOID )
{
	__asm
	{
		mov     edi,edi
		push    ebp
		mov     ebp,esp

		push [ebp+0x8]
		push [ebp+0x4]
		call IoIsOperationSyncCheckPatch
		jmp [callIoIsOperationSync];
	}
}

VOID
MakeIoIsOperationSync()
{
	BYTE PatchData[5];
	BYTE CheckData[5] = {0x8b, 0xff, 0x55, 0x8b, 0xec};

	DWORD* ptr = (DWORD*)(&PatchData[1]);
	DWORD pOriginalFunc = (DWORD) IoIsOperationSynchronous;
	DWORD pPatchFunc = (DWORD) NakedIoIsOperationSync;

	if (!memcmp((void*) pOriginalFunc, CheckData, sizeof(CheckData)))
	{
		BOOLEAN OldProtect;

		PatchData[0] = 0xe9;	// jmp
		*ptr = (DWORD)(pPatchFunc - pOriginalFunc - 5);
		callIoIsOperationSync = pOriginalFunc + sizeof(PatchData);

		if (MyVirtualProtect((void*) pOriginalFunc, TRUE, &OldProtect))
		{
			KIRQL OldIrql;
			KeRaiseIrql(DISPATCH_LEVEL, &OldIrql );

			memcpy((void*) pOriginalFunc, PatchData, sizeof(PatchData));

			KeLowerIrql( OldIrql );

			MyVirtualProtect((void*) pOriginalFunc, OldProtect, &OldProtect);
		}
	}
}

/*
80420726 8b442404         mov     eax,[esp+0x4]     ss:0010:eb4335b4=857a7e28
8042072a 8b4860           mov     ecx,[eax+0x60]
*/

__declspec(naked) NakedIoIsOperationSync2k( VOID )
{
	__asm
	{
		mov     eax,[esp+0x4]
		mov     ecx,[eax+0x60]
		push	eax
		push	ecx


		push	eax
		push	[esp+0x10]
		call	IoIsOperationSyncCheckPatch
		
		pop		ecx
		pop		eax
		jmp [callIoIsOperationSync];
	}
}


VOID
MakeIoIsOperationSync2k()
{
	BYTE PatchData[] = {0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90};
	BYTE CheckData[] = {0x8b, 0x44, 0x24, 0x04, 0x8b, 0x48, 0x60};

	DWORD* ptr = (DWORD*)(&PatchData[1]);
	DWORD pOriginalFunc = (DWORD) IoIsOperationSynchronous;
	DWORD pPatchFunc = (DWORD) NakedIoIsOperationSync2k;

	if (!memcmp((void*) pOriginalFunc, CheckData, sizeof(CheckData)))
	{
		BOOLEAN OldProtect;

		PatchData[0] = 0xe9;	// jmp
		*ptr = (DWORD)(pPatchFunc - pOriginalFunc - 5);
		callIoIsOperationSync = pOriginalFunc + sizeof(CheckData);

		if (MyVirtualProtect((void*) pOriginalFunc, TRUE, &OldProtect))
		{
			KIRQL OldIrql;
			KeRaiseIrql(DISPATCH_LEVEL, &OldIrql );

			memcpy((void*) pOriginalFunc, PatchData, sizeof(PatchData));

			KeLowerIrql( OldIrql );

			MyVirtualProtect((void*) pOriginalFunc, OldProtect, &OldProtect);
		}
	}
}

//////////////////////////////////////////////////////////////////////////
BOOLEAN KbdAttachCheck_ReInit();
VOID KbdSpliceCheck_ReInit();

VOID
SetSystemNotifiers()
{
// initialize cache for threads that attached their inputs to another threads
	GenCacheInit(
		0x1000,
		0,
		&g_pAttachedInputThreadCache
		);
// initialize cache for RPC mapper ports
//	GenCacheInit(
//		0x1000,
//		0,
//		&g_pRPCMapperPortsCache
//		);

	ExInitializeFastMutex(&g_fmInternalSync);
// Gruzdev
	ExInitializeFastMutex(&g_fmAsyncKeyLock);
	
	_impInitializeListHead(&gCheckLockList);
	_impInitializeListHead(&gPatchNameList);
	_impInitializeListHead(&gDrvNamesList);
// Gruzdev
	InitializeListHead(&gAsyncKeyReaders_Head);

	SysNameCacheInit(&gpSysContext);
	
	SysMon_Init();

	RtlZeroMemory(ProcIdtoHandle, sizeof(ProcIdtoHandle));

	ExInitializeNPagedLookasideList(gpPSysFileNames, NULL, NULL, 0, _SYSQUERY_INFO_LEN, 'sBOS', 0);
	ExInitializeNPagedLookasideList(gpPL0x140, NULL, NULL, 0, 0x140, '2pOS', 0);
	ExInitializeNPagedLookasideList(gpPL0x1000, NULL, NULL, 0, 0x1000, '3pOS', 0);
	ExInitializeNPagedLookasideList(gpPL0x4000, NULL, NULL, 0, 0x4000, '4pOS', 0);
// Gruzdev
	ExInitializePagedLookasideList(&g_PLL_AsyncKeyReaders, NULL, NULL, 0, sizeof(ASYNC_KEY_READER), '0000', 0);

	if(!NT_SUCCESS(PsSetCreateProcessNotifyRoutine(ProcessCrNotify, FALSE)))
		DbPrint(DC_SYS, DL_ERROR, ("PsSetCreateProcessNotifyRoutine failed\n"));

	if(!NT_SUCCESS(PsSetCreateThreadNotifyRoutine(ThreadCrNotify)))
		DbPrint(DC_SYS, DL_ERROR, ("PsSetCreateThreadNotifyRoutine failed\n"));

	if(((PVOID) __PsSetLoadImageNotifyRoutine = GetExport(BaseOfNtOsKrnl, "PsSetLoadImageNotifyRoutine", NULL)))
	{
		if(!NT_SUCCESS(__PsSetLoadImageNotifyRoutine(LoadImageNotify)))
			DbPrint(DC_SYS,DL_WARNING, ("PsSetLoadImageNotifyRoutine failed\n"));
	}
	else
		DbPrint(DC_SYS,DL_WARNING, ("PsSetLoadImageNotifyRoutine not found\n"));


	DbPrint(DC_SYS, DL_IMPORTANT, ("Hook system function...\n"));	

	HookNtFunc((ULONG*) &NtTerminateProcess, (ULONG)KLHookTerminateProcess, "NtTerminateProcess");
	HookNtFunc((ULONG*) &NtOpenProcess, (ULONG)KLHookOpenProcess, "NtOpenProcess");	
	HookNtFunc((ULONG*) &NtCreateProcess, (ULONG)KLHookCreateProcess, "NtCreateProcess");
	HookNtFunc((ULONG*) &NtCreateProcessEx, (ULONG)KLHookCreateProcessEx, "NtCreateProcessEx");
	if (HookNtFunc((ULONG*) &NtResumeThread, (ULONG)KLHookResumeThread, "NtResumeThread"))
		HookNtFunc((ULONG*) &NtSuspendThread, (ULONG)KLHookSuspendThread, "NtSuspendThread");

	HookNtFunc((ULONG*) &NtCreateThread, (ULONG)KLHookCreateThread, "NtCreateThread");

	if (_pfPsGetThreadId && _pfPsGetThreadProcessId)
		HookNtFunc((ULONG*) &NtSetContextThread, (ULONG)KLSetThreadContext, "NtSetContextThread");
		
	if (HookNtFunc((ULONG*) &NtClose, (ULONG)KLHookClose, "NtClose"))
		HookNtFunc((ULONG*) &NtCreateSection, (ULONG)KLHookCreateSection, "NtCreateSection");

	HookNtFunc((ULONG*) &NtOpenSection, (ULONG)KLHookOpenSection, "NtOpenSection");

	HookNtFunc((ULONG*) &NtCreateSymbolicLinkObject, (ULONG)KLHookNtCreateSymbolicLinkObject, "NtCreateSymbolicLinkObject");
	
	HookNtFunc((ULONG*) &NtSetSystemInformation, (ULONG)KLHookNtSetSystemInformation, "NtSetSystemInformation");

	if (*NtBuildNumber >= 2600)
	{
		MakeCheckLockAccess();
		MakeIoIsOperationSync();
	}
	else if (*NtBuildNumber >= 2195)
	{
		MakeIoIsOperationSync2k();
	}

//	HookNtFunc((ULONG*) &NtTerminateThread, (ULONG)KLTerminateThread, "NtTerminateThread");

	HookNtFunc((ULONG*) &NtWriteVirtualMemory, (ULONG)KLWriteVirtualMemory, "NtWriteVirtualMemory");

	if (!HookNtFunc((ULONG*)&NtSetInformationFile, (ULONG)KLHookSetInformationFile, "NtSetInformationFile"))
		DbPrint(DC_SYS, DL_ERROR,("HookNtFunc(..) on NtSetInformationFile failed\n"));

	if (!HookNtFunc((ULONG*)&NtDuplicateObject,(ULONG)KLHookDuplicateObject,"NtDuplicateObject"))
	{
		DbPrint(DC_SYS, DL_ERROR,("HookNtFunc(..) on NtDuplicateObject failed\n"));
	}

	HookNtFunc((ULONG*) &_ZwQuerySystemInformation, (ULONG)KLHookQuerySystemInformation, "ZwQuerySystemInformation");
	

	if (1381 == *NtBuildNumber)
		MakeLoadImageNotifyRoutine();

	if (!HookNtFunc((ULONG *)&NtSetSecurityObject, (ULONG)KLHookSetSecurityObject, "NtSetSecurityObject"))
		DbPrint(DC_SYS, DL_ERROR,("HookNtFunc(..) on NtSetSecurityObject failed\n"));

	if (!HookNtFunc((ULONG *)&NtLoadDriver, (ULONG)KLHookLoadDriver, "NtLoadDriver"))
		DbPrint(DC_SYS, DL_ERROR,("HookNtFunc(..) on NtLoadDriver failed\n"));

// Gruzdev
//	SetSplicers();

	if (!HookNtFunc((ULONG *)&NtSystemDebugControl, (ULONG)KLHookSystemDebugControl, "NtSystemDebugControl"))
		DbPrint(DC_SYS, DL_ERROR,("HookNtFunc(..) on NtSystemDebugControl failed\n"));
	
	DbPrint(DC_SYS,DL_IMPORTANT, ("Hook system function - success!\n"));	
}

// Gruzdev---------------------------------------------------------------------------------------
#define SERVICEID_NOT_ASSIGNED		0xFFFF

volatile BOOLEAN gbNtUserSetWindowsHookEx = FALSE;
volatile BOOLEAN gbNtUserPostMessage = FALSE;
volatile BOOLEAN gbNtUserGetAsyncKeyState = FALSE;
volatile BOOLEAN gbNtUserGetKeyState = FALSE;
volatile BOOLEAN gbNtUserSendInput = FALSE;
volatile BOOLEAN gbNtUserMessageCall = FALSE;
volatile BOOLEAN gbNtUserSendNotifyMessage = FALSE;
volatile BOOLEAN gbNtUserPostThreadMessage = FALSE;
volatile BOOLEAN gbNtUserFindWindowEx = FALSE;
volatile BOOLEAN gbNtUserSetWinEventHook = FALSE;
volatile BOOLEAN gbNtGdiBitBlt = FALSE;
volatile BOOLEAN gbNtUserGetKeyboardState = FALSE;
volatile BOOLEAN gbNtUserAttachThreadInput = FALSE;

VOID ShadowTable_Init();

BOOLEAN gbInSetSystemNotifiers_Late = FALSE;

BOOLEAN
SetSystemNotifiers_Late()
{
	ULONG	Service_ID1 = SERVICEID_NOT_ASSIGNED,
			Service_ID2 = SERVICEID_NOT_ASSIGNED,
			Service_ID3 = SERVICEID_NOT_ASSIGNED,
			Service_ID4 = SERVICEID_NOT_ASSIGNED,
			Service_ID5 = SERVICEID_NOT_ASSIGNED,
			Service_ID6 = SERVICEID_NOT_ASSIGNED,
			Service_ID7 = SERVICEID_NOT_ASSIGNED,
			Service_ID8 = SERVICEID_NOT_ASSIGNED,
			Service_ID9 = SERVICEID_NOT_ASSIGNED,
			Service_ID10 = SERVICEID_NOT_ASSIGNED,
			Service_ID11 = SERVICEID_NOT_ASSIGNED,
			Service_ID12 = SERVICEID_NOT_ASSIGNED,
			Service_ID13 = SERVICEID_NOT_ASSIGNED,
			Service_ID14 = SERVICEID_NOT_ASSIGNED,
			Service_ID15 = SERVICEID_NOT_ASSIGNED;


	if (*NtBuildNumber < 2195)
	{
		// nt4 - crash!
		return FALSE;
	}

	ExAcquireFastMutex(&g_fmInternalSync);

	if (gbInSetSystemNotifiers_Late)
	{
		ExReleaseFastMutex(&g_fmInternalSync);
		return FALSE;
	}

	gbInSetSystemNotifiers_Late = TRUE;

	ExReleaseFastMutex(&g_fmInternalSync);

// win 2k/xp
	if (*NtBuildNumber >= 2195)
	{
// win xp +
		if (*NtBuildNumber >= 2600)
		{
			if (*NtBuildNumber >= 3790)
			{
// win 2003 srv
				Service_ID1 = 0x1221;
				Service_ID2 = 0x11DA;
				Service_ID3 = 0x11E1;
				Service_ID4 = 0x117E;
				Service_ID5 = 0x1193;
				Service_ID6 = 0x119F;
				Service_ID7 = 0x11F4;
				Service_ID8 = SERVICEID_NOT_ASSIGNED;
				Service_ID9 = SERVICEID_NOT_ASSIGNED;
				Service_ID10 = 0x1179;
				Service_ID13 = 0x100D;
			}
			else
			{
// win xp
				Service_ID1 = 0x1225;
				Service_ID2 = 0x11DB;
				Service_ID3 = 0x11E3;
				Service_ID4 = 0x117F;
				Service_ID5 = 0x1194;
				Service_ID6 = 0x11A0;
				Service_ID7 = 0x11F6;
				Service_ID8 = 0x11CC;
				Service_ID9 = 0x11DC;
				Service_ID10 = 0x117A;
				Service_ID12 = 0x1228;
				Service_ID13 = 0x100D;
				Service_ID14 = 0x119E;
				Service_ID15 = 0x1133;
			}
		}
		else
		{
// win 2k
			if (gProcessesStart < 5) // !!! зависает если меньше 4 на w2k двухпроцессорном железе
			{
				gbInSetSystemNotifiers_Late = FALSE;
				return FALSE;
			}

			Service_ID1 = 0x1212;
			Service_ID2 = 0x11CB;
			Service_ID3 = 0x11D2;
			Service_ID4 = 0x1175;
			Service_ID5 = 0x1189;
			Service_ID6 = 0x1195;
			Service_ID7 = 0x11E1;
			Service_ID8 = 0x11BC;
			Service_ID9 = 0x11CC;			
			Service_ID10 = 0x1170;
			Service_ID11 = 0x11E3;
			Service_ID12 = 0x1215;
			Service_ID13 = 0x100D;
			Service_ID14 = 0x1193;
			Service_ID15 = 0x1129;
		}
	}
// win nt 4
	else
	{
		Service_ID1 = 0x11F0;
		Service_ID3 = 0x11B7;
		Service_ID5 = 0x1179;
		Service_ID10 = 0x112D;
	}

	if (!gbNtUserSetWindowsHookEx)
	{
		gbNtUserSetWindowsHookEx = TRUE;
		if (!HookNtFuncByID((PVOID *)&NtUserSetWindowsHookEx, KLHookUserSetWindowsHookEx, Service_ID1))
		{
			DbPrint(DC_SYS,DL_ERROR,("HookNtFuncByID(...) on NtUserSetWindowsHookEx failed\n"));
			gbNtUserSetWindowsHookEx = FALSE;
			gbInSetSystemNotifiers_Late = FALSE;
			return FALSE;
		}
	}

	NtUserQueryWindow = GetNtFuncByID(Service_ID3, NULL);
	if (!NtUserQueryWindow)
	{
		DbPrint(DC_SYS,DL_ERROR,("GetNtFuncByID(...) on NtUserQueryWindow failed\n"));
		gbInSetSystemNotifiers_Late = FALSE;
		return FALSE;
	}

	NtUserGetForegroundWindow = GetNtFuncByID(Service_ID5, NULL);
	if (!NtUserGetForegroundWindow)
	{
		DbPrint(DC_SYS,DL_ERROR,("GetNtFuncByID(...) on NtUserGetForegroundWindow failed\n"));
		gbInSetSystemNotifiers_Late = FALSE;
		return FALSE;
	}


	if (!gbNtUserPostMessage && Service_ID2 != SERVICEID_NOT_ASSIGNED)
	{
		gbNtUserPostMessage = TRUE;
		if (!HookNtFuncByID((PVOID *)&NtUserPostMessage, KLHookUserPostMessage, Service_ID2))
		{
			DbPrint(DC_SYS,DL_ERROR,("HookNtFuncByID(...) on NtUserPostMessage failed\n"));
			gbNtUserPostMessage = FALSE;
			gbInSetSystemNotifiers_Late = FALSE;
			return FALSE;
		}
	
	}

	if (!gbNtUserGetAsyncKeyState && Service_ID4 != SERVICEID_NOT_ASSIGNED)
	{
		gbNtUserGetAsyncKeyState = TRUE;
		if (!HookNtFuncByID((PVOID *)&NtUserGetAsyncKeyState, KLHookUserGetAsyncKeyState, Service_ID4))
		{
			DbPrint(DC_SYS,DL_ERROR,("HookNtFuncByID(...) on NtUserGetAsyncKeyState failed\n"));
			gbNtUserGetAsyncKeyState = FALSE;
			gbInSetSystemNotifiers_Late = FALSE;
			return FALSE;
		}
	}

	if (!gbNtUserGetKeyState && Service_ID6 != SERVICEID_NOT_ASSIGNED)
	{
		gbNtUserGetKeyState = TRUE;
		if (!HookNtFuncByID((PVOID *)&NtUserGetKeyState, KLHookUserGetKeyState, Service_ID6))
		{
			DbPrint(DC_SYS,DL_ERROR,("HookNtFuncByID(...) on NtUserGetKeyState failed\n"));
			gbNtUserGetKeyState = FALSE;
			gbInSetSystemNotifiers_Late = FALSE;
			return FALSE;
		}
	}

	if (!gbNtUserSendInput && Service_ID7 != SERVICEID_NOT_ASSIGNED)
	{
		gbNtUserSendInput = TRUE;
		if (!HookNtFuncByID((PVOID *)&NtUserSendInput, KLHookUserSendInput, Service_ID7))
		{
			DbPrint(DC_SYS,DL_ERROR,("HookNtFuncByID(...) on NtUserSendInput failed\n"));
			gbNtUserSendInput = FALSE;
			gbInSetSystemNotifiers_Late = FALSE;
			return FALSE;
		}
	}

	if (Service_ID8 != SERVICEID_NOT_ASSIGNED && !gbNtUserMessageCall)
	{
		gbNtUserMessageCall = TRUE;
		if (!HookNtFuncByID((PVOID *)&NtUserMessageCall, KLHookUserMessageCall, Service_ID8))
		{
			DbPrint(DC_SYS,DL_ERROR,("HookNtFuncByID(...) on NtUserMessageCall failed\n"));
			gbNtUserMessageCall = FALSE;
			gbInSetSystemNotifiers_Late = FALSE;
			return FALSE;
		}
	}

	if (Service_ID9 != SERVICEID_NOT_ASSIGNED && !gbNtUserPostThreadMessage)
	{
		gbNtUserPostThreadMessage = TRUE;
		if (!HookNtFuncByID((PVOID *)&NtUserPostThreadMessage, KLHookUserPostThreadMessage, Service_ID9))
		{
			DbPrint(DC_SYS,DL_ERROR,("HookNtFuncByID(...) on NtUserPostThreadMessage failed\n"));
			gbNtUserPostThreadMessage = FALSE;
			gbInSetSystemNotifiers_Late = FALSE;
			return FALSE;
		}
	}


	if (Service_ID10 != SERVICEID_NOT_ASSIGNED && !gbNtUserFindWindowEx)
	{
		gbNtUserFindWindowEx = TRUE;
		if (!HookNtFuncByID((PVOID *)&NtUserFindWindowEx, KLHookUserFindWindowEx, Service_ID10))
		{
			DbPrint(DC_SYS,DL_ERROR,("HookNtFuncByID(...) on NtUserFindWindowEx failed\n"));
			gbNtUserFindWindowEx = FALSE;
			gbInSetSystemNotifiers_Late = FALSE;
			return FALSE;
		}
	}

// for 2k only
	if (Service_ID11 != SERVICEID_NOT_ASSIGNED && !gbNtUserSendNotifyMessage)
	{
		gbNtUserSendNotifyMessage = TRUE;
		if (!HookNtFuncByID((PVOID *)&NtUserSendNotifyMessage, KLHookUserSendNotifyMessage, Service_ID11))
		{
			DbPrint(DC_SYS,DL_ERROR,("HookNtFuncByID(...) on NtUserSendNotifyMessage failed\n"));
			gbNtUserSendNotifyMessage = FALSE;
			gbInSetSystemNotifiers_Late = FALSE;
			return FALSE;
		}
	}

	if (Service_ID12 != SERVICEID_NOT_ASSIGNED && !gbNtUserSetWinEventHook)
	{
		gbNtUserSetWinEventHook = TRUE;
		if (!HookNtFuncByID((PVOID *)&NtUserSetWinEventHook, KLHookUserSetWinEventHook, Service_ID12))
		{
			DbPrint(DC_SYS,DL_ERROR,("HookNtFuncByID(...) on NtUserSetWinEventHook failed\n"));
			gbNtUserSetWinEventHook = FALSE;
			gbInSetSystemNotifiers_Late = FALSE;
			return FALSE;
		}
	}

	if (Service_ID13 != SERVICEID_NOT_ASSIGNED && !gbNtGdiBitBlt)
	{
		gbNtGdiBitBlt = TRUE;
		if (!HookNtFuncByID((PVOID *)&NtGdiBitBlt, KLHookBitBlt, Service_ID13))
		{
			DbPrint(DC_SYS,DL_ERROR,("HookNtFuncByID(...) on NtGdiBitBlt failed\n"));
			gbNtGdiBitBlt = FALSE;
			gbInSetSystemNotifiers_Late = FALSE;
			return FALSE;
		}
	}

	if ( SERVICEID_NOT_ASSIGNED != Service_ID14 && !gbNtUserGetKeyboardState )
	{
		gbNtUserGetKeyboardState = TRUE;

		if ( !HookNtFuncByID( (PVOID *)&NtUserGetKeyboardState, KLHookUserGetKeyboardState, Service_ID14 ) )
		{
			DbPrint(DC_SYS,DL_ERROR,("HookNtFuncByID(...) on NtUserGetKeyboardState failed\n"));
			gbNtUserGetKeyboardState = FALSE;
			gbInSetSystemNotifiers_Late = FALSE;

			return FALSE;
		}
	}

	if ( SERVICEID_NOT_ASSIGNED != Service_ID15 && !gbNtUserAttachThreadInput )
	{
		gbNtUserAttachThreadInput = TRUE;
		
		if ( !HookNtFuncByID( (PVOID *)&NtUserAttachThreadInput, KLHookUserAttachThreadInput, Service_ID15 ) )
		{
			DbPrint(DC_SYS,DL_ERROR,("HookNtFuncByID(...) on NtUserAttachThreadInput failed\n"));
			gbNtUserAttachThreadInput = FALSE;
			gbInSetSystemNotifiers_Late = FALSE;
			
			return FALSE;
		}
	}

	gbInSetSystemNotifiers_Late = FALSE;

	return TRUE;
}

VOID
RemoveSystemNotifiers()
{
	SysNameCacheDone(&gpSysContext);

	ExDeleteNPagedLookasideList(gpPSysFileNames);
	ExDeleteNPagedLookasideList(gpPL0x140);
	ExDeleteNPagedLookasideList(gpPL0x1000);
	ExDeleteNPagedLookasideList(gpPL0x4000);
// Gruzdev
	ExDeletePagedLookasideList(&g_PLL_AsyncKeyReaders);
}

NTSTATUS
SysFillPIDList(PVOID pOutRequest, ULONG OutRequestSize, ULONG *pRetSize)
{
	NTSTATUS ntStatus = STATUS_BUFFER_TOO_SMALL;
	int cou;
	PULONG pPids = (PULONG) pOutRequest;

	*pRetSize = 0;
	for (cou = 0; cou < _countof(ProcIdtoHandle); cou++)
	{
		if (*pRetSize >= OutRequestSize)
			break;

		if (ProcIdtoHandle[cou].m_ImagesLoadedCount)
		{
			*pPids = cou;
			pPids++;
			*pRetSize += sizeof(ULONG);
		}
	}

	if (cou == _countof(ProcIdtoHandle))
		ntStatus = STATUS_SUCCESS;
	else
		*pRetSize = 0;

	return ntStatus;
}

NTSTATUS
SysFillDrvList (
	PVOID pOutRequest,
	ULONG OutRequestSize,
	ULONG *pRetSize
	)
{
	NTSTATUS ntStatus = STATUS_UNSUCCESSFUL;
	*pRetSize = 0;

	if (!pOutRequest)
		return STATUS_INVALID_PARAMETER;

	if (sizeof(ULONG) > OutRequestSize)
		return STATUS_INVALID_PARAMETER;

	ExAcquireFastMutex(&g_fmInternalSync);

	if (!IsListEmpty(&gDrvNamesList))
	{
		PIMPLIST_ENTRY Flink;
		PDRVNAME_LISTITEM pDrvNameTmp;
		ULONG len;

		ntStatus = STATUS_SUCCESS;

		Flink = gDrvNamesList.Flink;
	
		while(Flink != &gDrvNamesList)
		{
			pDrvNameTmp = CONTAINING_RECORD(Flink, DRVNAME_LISTITEM, m_List);
			
			Flink = Flink->Flink;

			len = (wcslen(pDrvNameTmp->m_DrvName) + 1) * sizeof(WCHAR);

			*pRetSize += len;
			if (OutRequestSize > sizeof(ULONG))
			{
				if (*pRetSize > OutRequestSize)
				{
					ntStatus = STATUS_BUFFER_TOO_SMALL;
					*pRetSize = 0;
					DbgBreakPoint();
					break;
				}

				memcpy(pOutRequest, pDrvNameTmp->m_DrvName, len);
				pOutRequest = (BYTE*) pOutRequest + len;
			}
		}
	}

	ExReleaseFastMutex(&g_fmInternalSync);

	if (sizeof(ULONG) == OutRequestSize)
	{
		*(ULONG*) pOutRequest = *pRetSize + 4096;
		*pRetSize = sizeof(ULONG);

		ntStatus = STATUS_BUFFER_TOO_SMALL;
	}

	return ntStatus;
}

NTSTATUS
SysFillSectionList(ULONG PID, PVOID pOutRequest, ULONG OutRequestSize, ULONG *pRetSize)
{
	NTSTATUS ntStatus = STATUS_SUCCESS;
	ULONG cou;

	*pRetSize = 0;

	if (!pOutRequest)
		return STATUS_INVALID_PARAMETER;

	_RESOURCE_LOCK_W(&gpSysContext->m_CacheResource);

	if (sizeof(ULONG) == OutRequestSize)
	{
		for (cou = 0; cou < _countof(ProcIdtoHandle); cou++)
		{
			if (!PID || (PID == cou))
			{
				if (ProcIdtoHandle[cou].m_pwchmainImage)
				{
					ULONG len = wcslen(ProcIdtoHandle[cou].m_pwchmainImage);
					if (len)
					{
						len = (len + 1) * sizeof(WCHAR);
						*pRetSize += len;
					}
				}
			}
		}
		
		*(ULONG*) pOutRequest = *pRetSize;
		*pRetSize = sizeof(ULONG);

		ntStatus = STATUS_BUFFER_TOO_SMALL;
	}
	else
	{
		for (cou = 0; cou < _countof(ProcIdtoHandle); cou++)
		{
			if (!PID || (PID == cou))
			{
				if (ProcIdtoHandle[cou].m_pwchmainImage)
				{
					ULONG len = wcslen(ProcIdtoHandle[cou].m_pwchmainImage);
					if (len)
					{
						len = (len + 1) * sizeof(WCHAR);
						if ((*pRetSize + len ) > OutRequestSize)
						{
							ntStatus = STATUS_BUFFER_TOO_SMALL;
							*pRetSize = 0;
							DbgBreakPoint();
							break;
						}

						memcpy(pOutRequest, ProcIdtoHandle[cou].m_pwchmainImage, len);
						pOutRequest = (BYTE*) pOutRequest + len;
						*pRetSize += len;
					}
				}
			}
		}
	}

	_RESOURCE_UNLOCK(&gpSysContext->m_CacheResource);

	return ntStatus;
}

//////////////////////////////////////////////////////////////////////////
BOOLEAN gbSysCheckInited = FALSE;

PVOID gpSysEnter = 0;
PVOID gpSysEnter_OnStart = 0;

void
SysMon_Init()
{
	BOOLEAN CheckSysEnterDisabled = FALSE;

	ULONG cpuid_result = 0;
	ULONG BaseFamily = 0;
	ULONG BaseModel = 0;
	ULONG Stepping = 0;

	if (*NtBuildNumber < 2600)
		return;

	// xp or higher
	//amd k6   - CPUID: Family 5, Model 7, Stepping 0
	//amd k6-2 - CPUID: Family 5, Model 8, Stepping 0
	//31:0 The 12 8-bit ASCII character codes to create the string “AuthenticAMD”.
	//EBX=6874_7541h “h t u A”, ECX=444D_4163h “D M A c”, EDX=6974_6E65h “i t n e”

	_asm
	{
		mov eax, 0
		cpuid

		cmp ebx, 68747541h
		jne _endcpucheck
		
		cmp ecx, 444D4163h
		jne _endcpucheck

		cmp edx, 69746E65h
		jne _endcpucheck

		mov eax, 1
		cpuid
		mov cpuid_result, eax

_endcpucheck:		; cpuid_result will be 0 if not amd
	}

	Stepping = cpuid_result & 0xf;
	cpuid_result = cpuid_result  >> 4;

	BaseModel = cpuid_result & 0xf;
	cpuid_result = cpuid_result >> 4;

	BaseFamily = cpuid_result & 0xf;
	cpuid_result = cpuid_result >> 4;

	//if ((5 == BaseFamily) && (7 <= BaseModel && 9 <= BaseModel))
	if (5 == BaseFamily)
	{
		CheckSysEnterDisabled = TRUE;
	}

	if (!CheckSysEnterDisabled)
	{
		_asm
		{
			mov ecx, 176h				; get sysenter addr
			rdmsr						;
			mov gpSysEnter_OnStart, eax		;
		}

		gpSysEnter = gpSysEnter_OnStart;
		gbSysCheckInited = TRUE;
	}
}

void
SysMon_Check()
{
	PVOID CurrentSysEnter = 0;

	if (!gbSysCheckInited)
		return;

	_asm
	{
		mov ecx, 176h				; get sysenter addr
		rdmsr						;
		mov CurrentSysEnter, eax		;
	}	
	
	if ((CurrentSysEnter != gpSysEnter) &&
		CurrentSysEnter != gpSysEnter_OnStart)
	{
		gpSysEnter = CurrentSysEnter;

		if (IsNeedFilterEvent(FLTTYPE_SYSTEM, 0x12/*MJ_SYSTEM_SYSENTER_CHANGED*/, 0))
		{
			BYTE *RequestData = ExAllocateFromNPagedLookasideList(gpPL0x140);
			
			if (RequestData != NULL)
			{
				PFILTER_EVENT_PARAM	pParam = (PFILTER_EVENT_PARAM) RequestData;
				PSINGLE_PARAM pSingleParam;

				FILTER_PARAM_COMMONINIT(pParam, FLTTYPE_SYSTEM, 0x12/*MJ_SYSTEM_SYSENTER_CHANGED*/, 0, PostProcessing, 3);
				pParam->UnsafeValue = 1;
				pSingleParam = (PSINGLE_PARAM) pParam->Params;

				SINGLE_PARAM_INIT_SID(pSingleParam);

				SINGLE_PARAM_SHIFT(pSingleParam);
				SINGLE_PARAM_INIT_ULONG(pSingleParam, _PARAM_OBJECT_ADDR1, (ULONG) gpSysEnter_OnStart);

				SINGLE_PARAM_SHIFT(pSingleParam);
				SINGLE_PARAM_INIT_ULONG(pSingleParam, _PARAM_OBJECT_ADDR2, (ULONG) CurrentSysEnter);
			
				FilterEvent(pParam, NULL);

				ExFreeToNPagedLookasideList(gpPL0x140, RequestData);
			}
		}
	}
}

NTSTATUS
SysGetPrefetch(PVOID pOutRequest, ULONG OutRequestSize, ULONG *pRetSize)
{
	NTSTATUS ntStatus;
	
	*pRetSize = g_PrefetchDataSize;

	if (!pOutRequest || (sizeof(ULONG) == OutRequestSize))
	{
		if (OutRequestSize == sizeof(ULONG))
		{
			*pRetSize = sizeof(ULONG);
			*(ULONG*) pOutRequest = g_PrefetchDataSize;
		}
	
		return STATUS_BUFFER_TOO_SMALL;
	}

	ExAcquireFastMutex(&g_fmInternalSync);

	if (g_PrefetchDataSize &&  (g_PrefetchDataSize <= OutRequestSize))
	{
		memcpy(pOutRequest, g_PrefetchData, g_PrefetchDataSize);
		*pRetSize = g_PrefetchDataSize;
		ntStatus = STATUS_SUCCESS;
	}
	else
	{
		*pRetSize = 0;
		ntStatus = STATUS_BUFFER_TOO_SMALL;
	}

	ExReleaseFastMutex(&g_fmInternalSync);

	return ntStatus;
}

//+ Gruzdev ---------------------------------------------------------------------------------
typedef
struct _MONITORED_SERVICE_ITEM
{
	PVOID	*m_pfServiceFunc;
	PVOID	m_fPointerOnStart;
} MONITORED_SERVICE_ITEM, *PMONITORED_SERVICE_ITEM;

MONITORED_SERVICE_ITEM gNtUserGetMessage_Service;
MONITORED_SERVICE_ITEM gNtUserPeekMessage_Service;

PEPROCESS g_pCSRSSProcess = NULL;

PVOID g_pWin32kBase = NULL;
ULONG gWin32kSize = 0;

volatile BOOLEAN g_bShadowTable_Inited = FALSE;


NTKERNELAPI
NTSTATUS
PsLookupProcessByProcessId(
    IN HANDLE ProcessId,
    OUT PEPROCESS *Process
    );

//-------------------------------------------------------------------------------------------------
// возвращает объект процесса по имени (напр. "csrss.exe"), предварительно зареференсив его
PEPROCESS LookupProcessByName(IN PWCHAR ProcessName)
{
	ULONG BufferSize = sizeof(SYSTEM_THREADS)*1000;
	PVOID pBuffer = NULL;
	NTSTATUS ntStatus;
	PSYSTEM_PROCESSES pSysProc;
	UNICODE_STRING cntdProcessName;
	ULONG Tmp;
	PEPROCESS pProcess = NULL;

	RtlInitUnicodeString(&cntdProcessName, ProcessName);

// получаем список процессов
	do
	{
		if (pBuffer)
			ExFreePool(pBuffer);

		pBuffer = ExAllocatePoolWithTag(PagedPool, BufferSize, '0000');
		if (!pBuffer)
			return NULL;

		ntStatus = ZwQuerySystemInformation(5/*processes and threads*/, pBuffer,
			BufferSize, &Tmp);
		if (ntStatus != STATUS_SUCCESS && ntStatus != STATUS_INFO_LENGTH_MISMATCH)
			break;

		BufferSize *= 2;
	} while(ntStatus != STATUS_SUCCESS && BufferSize < 1000000);

	if (ntStatus == STATUS_SUCCESS)
	{
		pSysProc = (PSYSTEM_PROCESSES)pBuffer;
		do
		{
// если это заданнный процесс
			if (RtlCompareUnicodeString(&pSysProc->ProcessName, &cntdProcessName, TRUE) == 0)
			{
				PEPROCESS pCurrProcess;

				ntStatus = PsLookupProcessByProcessId((HANDLE)pSysProc->ProcessId, &pCurrProcess);
				if (NT_SUCCESS(ntStatus))
					pProcess = pCurrProcess;
			}

			(PCHAR)pSysProc += pSysProc->NextEntryDelta;
		} while (pSysProc->NextEntryDelta && !pProcess);
	}

	ExFreePool(pBuffer);

	return pProcess;
}

VOID ShadowTable_Init()
{
	if (!g_bShadowTable_Inited)
	{
		PEPROCESS pCSRSSProcess;
		ULONG NtUserGetMessageID;
		ULONG NtUserPeekMessageID;
		PVOID NtUserGetMessage_OnStart;
		PVOID NtUserPeekMessage_OnStart;
		PVOID *p_fNtUserGetMessage;
		PVOID *p_fNtUserPeekMessage;

		pCSRSSProcess = LookupProcessByName(L"csrss.exe");
		if (pCSRSSProcess)
		{
// win 2k/xp
			if (*NtBuildNumber >= 2195)
			{
// win xp +
				if (*NtBuildNumber >= 2600)
				{
					if (*NtBuildNumber >= 3790)
					{
// win 2003 srv
						NtUserGetMessageID = 0x11A4;
						NtUserPeekMessageID = 0x11D9;
					}
					else
					{
// win xp
						NtUserGetMessageID = 0x11A5;
						NtUserPeekMessageID = 0x11DA;
					}
				}
				else
				{
// win 2k
					NtUserGetMessageID = 0x119A;
					NtUserPeekMessageID = 0x11CA;
				}
			}
// win nt 4
			else
			{
				NtUserGetMessageID = 0x1185;
				NtUserPeekMessageID = 0x118E;
			}

			KeAttachProcess(pCSRSSProcess);
//--------------------------------------------------------------------------------

			NtUserGetMessage_OnStart = GetNtFuncByID(NtUserGetMessageID, &p_fNtUserGetMessage);
			NtUserPeekMessage_OnStart = GetNtFuncByID(NtUserPeekMessageID, &p_fNtUserPeekMessage);

			if (NtUserGetMessage_OnStart && NtUserPeekMessage_OnStart)
			{
				PVOID pSysInfo;

				g_pCSRSSProcess = pCSRSSProcess;

				gNtUserGetMessage_Service.m_fPointerOnStart = NtUserGetMessage_OnStart;
				gNtUserGetMessage_Service.m_pfServiceFunc = p_fNtUserGetMessage;
				gNtUserPeekMessage_Service.m_fPointerOnStart = NtUserPeekMessage_OnStart;
				gNtUserPeekMessage_Service.m_pfServiceFunc = p_fNtUserPeekMessage;

				pSysInfo = ExAllocatePoolWithTag(PagedPool, 0x8000, 'grSI');
				if (pSysInfo)
				{
					NTSTATUS ntStatus;
					ULONG RetLen = 0;

					ntStatus = ZwQuerySystemInformation(11/*SystemModuleInformation*/, pSysInfo, 0x8000, &RetLen);
					if (ntStatus == STATUS_INFO_LENGTH_MISMATCH && RetLen)
					{
						ExFreePool(pSysInfo);
						pSysInfo = ExAllocatePoolWithTag(PagedPool, RetLen, 'grSI');
						if (pSysInfo)
							ntStatus = ZwQuerySystemInformation(11/*SystemModuleInformation*/, pSysInfo, RetLen, &RetLen);
					}
					if (NT_SUCCESS(ntStatus))
					{
						ULONG ModuleCount = *(PULONG)pSysInfo;
						PSYSTEM_MODULE_INFORMATION pModuleInfo = (PSYSTEM_MODULE_INFORMATION)((PCHAR)pSysInfo+sizeof(ULONG));

						if (ModuleCount != 0 && pModuleInfo)
						{
							ULONG i;

							for (i = 0; i < ModuleCount; i++)
							{
								PSYSTEM_MODULE_INFORMATION pCurrInfo = pModuleInfo+i;

								if (_stricmp((PCHAR)pCurrInfo->ImageName+pCurrInfo->ModNameOffset, "win32k.sys") == 0)
								{
									g_pWin32kBase = pCurrInfo->Base;
									gWin32kSize = pCurrInfo->Size;

									DbPrint(DC_SYS, DL_IMPORTANT, ("got win32k.sys base address..\n"));
								}
							}
						}
					}

					if (pSysInfo)
						ExFreePool(pSysInfo);
				}

				g_bShadowTable_Inited = TRUE;

				DbPrint(DC_SYS, DL_IMPORTANT, ("shadow table check initialized..\n"));
			}
//--------------------------------------------------------------------------------
			KeDetachProcess();
		}

	}
}

VOID ShadowTableEvent_Common(ULONG EventCode, ULONG KeyloggerID, PVOID ObjectAddress)
{
	PFILTER_EVENT_PARAM	pParam = (PFILTER_EVENT_PARAM)ExAllocateFromNPagedLookasideList(gpPL0x140);
		
	if (pParam)
	{
		PSINGLE_PARAM pSingleParam;

		FILTER_PARAM_COMMONINIT(pParam, FLTTYPE_SYSTEM, EventCode, 0, PostProcessing, 4);
		pParam->ProcessID = -1;
		pParam->ThreadID = -1;
		pSingleParam = (PSINGLE_PARAM)pParam->Params;

		SINGLE_PARAM_INIT_SID(pSingleParam);

		SINGLE_PARAM_SHIFT(pSingleParam);
		SINGLE_PARAM_INIT_ULONG(pSingleParam, _PARAM_OBJECT_KEYLOGGERID, KeyloggerID);

		SINGLE_PARAM_SHIFT(pSingleParam);
		SINGLE_PARAM_INIT_ULONG(pSingleParam, _PARAM_OBJECT_SOURCE_ID, (ULONG)PsGetCurrentProcessId());

		SINGLE_PARAM_SHIFT(pSingleParam);
		SINGLE_PARAM_INIT_ULONG(pSingleParam, _PARAM_OBJECT_ADDR1, (ULONG)ObjectAddress);
		
		FilterEvent(pParam, NULL);

		ExFreeToNPagedLookasideList(gpPL0x140, pParam);
	}
}

VOID ShadowTable_Check()
{
	if (g_bShadowTable_Inited)
	{
		BOOLEAN bGetMessagePatched = FALSE;
		BOOLEAN bPeekMessagePatched = FALSE;
		PVOID fNewGetMessage = NULL;
		PVOID fNewPeekMessage = NULL;

		KeAttachProcess(g_pCSRSSProcess);
//--------------------------------------------------------------------------------
		__try
		{
			bGetMessagePatched = *gNtUserGetMessage_Service.m_pfServiceFunc != gNtUserGetMessage_Service.m_fPointerOnStart ||
				(g_pWin32kBase &&
				((ULONG)*gNtUserGetMessage_Service.m_pfServiceFunc < (ULONG)g_pWin32kBase || (ULONG)*gNtUserGetMessage_Service.m_pfServiceFunc >= (ULONG)g_pWin32kBase+gWin32kSize));
			bPeekMessagePatched = *gNtUserPeekMessage_Service.m_pfServiceFunc != gNtUserPeekMessage_Service.m_fPointerOnStart ||
				(g_pWin32kBase &&
				((ULONG)*gNtUserPeekMessage_Service.m_pfServiceFunc < (ULONG)g_pWin32kBase || (ULONG)*gNtUserPeekMessage_Service.m_pfServiceFunc >= (ULONG)g_pWin32kBase+gWin32kSize));

			fNewGetMessage = *gNtUserGetMessage_Service.m_pfServiceFunc;
			fNewPeekMessage = *gNtUserPeekMessage_Service.m_pfServiceFunc;
		}
		__except(EXCEPTION_EXECUTE_HANDLER)
		{
			DbgBreakPoint();
			DbPrint(DC_SYS, DL_ERROR, ("memory access fault on GetMessage/PeekMessage\n"));
		}
//--------------------------------------------------------------------------------		
		KeDetachProcess();

		if (bGetMessagePatched)
		{
			ShadowTableEvent_Common(0x14/*MJ_SYSTEM_KEYLOGGER_DETECTED*/, KEYLOGGER_GETMESSAGE_PATCH, fNewGetMessage);
			DbPrint(DC_SYS, DL_IMPORTANT, ("potential keylogger: shadow table patched on NtUserGetMessage..\n"));
		}

		if (bPeekMessagePatched)
		{
			ShadowTableEvent_Common(0x14/*MJ_SYSTEM_KEYLOGGER_DETECTED*/, KEYLOGGER_PEEKMESSAGE_PATCH, fNewPeekMessage);
			DbPrint(DC_SYS, DL_IMPORTANT, ("potential keylogger: shadow table patched on NtUserPeekMessage..\n"));
		}
	}
}

#define KBDREAD_PROLOGUE_SIZE	16

PVOID g_pKbdReadDispatch = NULL;
PVOID g_pKbdReadDispatchPtr = NULL;
UCHAR g_rKbdReadDispatch_Prologue[KBDREAD_PROLOGUE_SIZE];

BOOLEAN g_bKbdSpliceCheck_Inited = FALSE;
BOOLEAN g_bKbdAttachCheck_Inited = FALSE;

PDEVICE_OBJECT TryGetActiveKbdDevices(IN PDRIVER_OBJECT pKbdPortDriver, IN PDEVICE_OBJECT *pKbdClassDevices,
									  IN ULONG NumKbdClassDevices, OUT PDEVICE_OBJECT *p_pPortDevice OPTIONAL)
{
	ULONG i;
	PDEVICE_OBJECT pDevice = pKbdPortDriver->DeviceObject;

	while (pDevice)
	{
		BOOLEAN bResult;

		for (i = 0; i < NumKbdClassDevices; i++)
		{
			if (pKbdClassDevices[i])
			{
				bResult = IsLowerDeviceOf(pDevice, pKbdClassDevices[i]);
				if (bResult)
				{
#ifdef _DEBUG
					WCHAR DriverName[128];

					if (pKbdPortDriver->DriverName.Buffer)
					{
						memcpy(DriverName, pKbdPortDriver->DriverName.Buffer, pKbdPortDriver->DriverName.Length);
						DriverName[pKbdPortDriver->DriverName.Length / sizeof(WCHAR)] = 0;
					}

					DbPrint(DC_SYS, DL_IMPORTANT, ("found kbd device objects on %S / \\Device\\KbdClass%d\n",
						DriverName, i));
#endif
	
					if (p_pPortDevice)
						*p_pPortDevice = pDevice;

					return pKbdClassDevices[i];
				}
			}
		}

		pDevice = pDevice->NextDevice;
	}

	return NULL;
}

VOID KbdSpliceCheck_ReInit()
{
	BOOLEAN bDevicesChanged;

	bDevicesChanged = KbdAttachCheck_ReInit();

	if (!g_bKbdAttachCheck_Inited)
	{
		g_bKbdSpliceCheck_Inited = FALSE;
		g_pKbdReadDispatch = NULL;
		g_pKbdReadDispatchPtr = NULL;
		RtlZeroMemory(g_rKbdReadDispatch_Prologue, KBDREAD_PROLOGUE_SIZE);

		return;
	}

	if (bDevicesChanged)
	{
		g_bKbdSpliceCheck_Inited = FALSE;
		g_pKbdReadDispatch = NULL;
		g_pKbdReadDispatchPtr = NULL;
		RtlZeroMemory(g_rKbdReadDispatch_Prologue, KBDREAD_PROLOGUE_SIZE);

		if (g_pKbdClassDevice->DriverObject && g_pKbdClassDevice->DriverObject->MajorFunction[IRP_MJ_READ])
		{
			g_pKbdReadDispatch = g_pKbdClassDevice->DriverObject->MajorFunction[IRP_MJ_READ];
			g_pKbdReadDispatchPtr = &(g_pKbdClassDevice->DriverObject->MajorFunction[IRP_MJ_READ]);
			memcpy(g_rKbdReadDispatch_Prologue, g_pKbdReadDispatch, KBDREAD_PROLOGUE_SIZE);

			g_bKbdSpliceCheck_Inited = TRUE;
		}

	}

	DbPrint(DC_SYS, DL_IMPORTANT, ("KbdSpliceCheck_ReInit: g_bKbdSpliceCheck_Inited=%d, ReInit=%d\n",
		g_bKbdSpliceCheck_Inited, bDevicesChanged));
}

VOID KbdSplice_Check()
{
	VERDICT Verdict = Verdict_Default;

	KbdSpliceCheck_ReInit();

	if (g_bKbdSpliceCheck_Inited)
	{
		PVOID ptr =  *(PVOID*)g_pKbdReadDispatchPtr;
		if ((ptr != g_pKbdReadDispatch) || memcmp(g_pKbdReadDispatch, g_rKbdReadDispatch_Prologue, KBDREAD_PROLOGUE_SIZE))
		{
			PFILTER_EVENT_PARAM	pParam = (PFILTER_EVENT_PARAM)ExAllocateFromNPagedLookasideList(gpPL0x140);
		
			if (pParam)
			{
				PSINGLE_PARAM pSingleParam;

				FILTER_PARAM_COMMONINIT(pParam, FLTTYPE_SYSTEM, 0x14/*MJ_SYSTEM_KEYLOGGER_DETECTED*/, 0, PostProcessing, 3);
				pParam->ProcessID = -1;
				pParam->ThreadID = -1;
				pSingleParam = (PSINGLE_PARAM)pParam->Params;

				SINGLE_PARAM_INIT_SID(pSingleParam);

				SINGLE_PARAM_SHIFT(pSingleParam);
				SINGLE_PARAM_INIT_ULONG(pSingleParam, _PARAM_OBJECT_KEYLOGGERID, KEYLOGGER_SPLICEONREADDISPATCH);

// копируем сплайс
				SINGLE_PARAM_SHIFT(pSingleParam);
				SINGLE_PARAM_INIT_NONE(pSingleParam, _PARAM_OBJECT_BINARYDATA);
				memcpy(pSingleParam->ParamValue, g_pKbdReadDispatch, KBDREAD_PROLOGUE_SIZE);
				pSingleParam->ParamSize = KBDREAD_PROLOGUE_SIZE;

				Verdict = FilterEvent(pParam, NULL);

				ExFreeToNPagedLookasideList(gpPL0x140, pParam);
			}

			DbPrint(DC_SYS, DL_IMPORTANT, ("kbdclass read dispatch handler has been spliced\n"));
		}
	}
}

NTSYSAPI POBJECT_TYPE IoDriverObjectType;

#ifdef _HOOK_NT_
__declspec(dllimport) int __cdecl swprintf(wchar_t *, const wchar_t *, ...);
#endif

// returnes true if devices really changed
BOOLEAN KbdAttachCheck_ReInit()
{
	NTSTATUS ntStatus;
	BOOLEAN bResult;
	ULONG i;
	PDEVICE_OBJECT pKbdClassDevices[10];
	UNICODE_STRING cntdi8042prtDriverName;
	PDRIVER_OBJECT pi8042prtDriver;
	PDEVICE_OBJECT OldKbdClassDevice = NULL;
	PDEVICE_OBJECT OldPortDevice = NULL;

	if (g_bKbdAttachCheck_Inited)
	{
		OldKbdClassDevice = g_pKbdClassDevice;
		OldPortDevice = g_pi8042prtDevice;

		ObDereferenceObject(g_pKbdClassDevice);
		ObDereferenceObject(g_pi8042prtDevice);

		g_bKbdAttachCheck_Inited = FALSE;
		g_pKbdClassDevice = NULL;
		g_pi8042prtDevice = NULL;
	}
		
// получаем указатели на \Device\KeyboardClassN
	RtlZeroMemory(pKbdClassDevices, sizeof(pKbdClassDevices));
	for (i = 0; i < 10; i++)
	{
		UNICODE_STRING cntdKbdClassStr;
		WCHAR wcKbdClassStr[128];
		PDEVICE_OBJECT pKbdClassDevice;

		swprintf(wcKbdClassStr, L"\\Device\\KeyboardClass%d", i);
		RtlInitUnicodeString(&cntdKbdClassStr, wcKbdClassStr);
		ntStatus = ReferenceBaseDeviceByName(&cntdKbdClassStr, &pKbdClassDevice);
		if (NT_SUCCESS(ntStatus))
			pKbdClassDevices[i] = pKbdClassDevice;
	}


	RtlInitUnicodeString(&cntdi8042prtDriverName, L"\\Driver\\i8042prt");
	ntStatus = ObReferenceObjectByName(&cntdi8042prtDriverName, OBJ_CASE_INSENSITIVE, NULL, 0, IoDriverObjectType,
		KernelMode, NULL, &pi8042prtDriver);
	if (NT_SUCCESS(ntStatus))
	{
		PDEVICE_OBJECT pKbdClassDevice;
		PDEVICE_OBJECT pPortDevice;

		pKbdClassDevice = TryGetActiveKbdDevices(pi8042prtDriver, pKbdClassDevices, 10, &pPortDevice);
		if (pKbdClassDevice)
		{
			g_pKbdClassDevice = pKbdClassDevice;
			g_pi8042prtDevice = pPortDevice;

			g_bKbdAttachCheck_Inited = TRUE;

		}

		ObDereferenceObject(pi8042prtDriver);
	}
	

	if (!g_bKbdAttachCheck_Inited)
	{
		UNICODE_STRING cntdKbdHidDriverName;
		PDRIVER_OBJECT pKbdHidDriver;

		RtlInitUnicodeString(&cntdKbdHidDriverName, L"\\Driver\\kbdhid");
		ntStatus = ObReferenceObjectByName(&cntdKbdHidDriverName, OBJ_CASE_INSENSITIVE, NULL, 0, IoDriverObjectType,
			KernelMode, NULL, &pKbdHidDriver);
		if (NT_SUCCESS(ntStatus))
		{
			PDEVICE_OBJECT pKbdClassDevice;
			PDEVICE_OBJECT pPortDevice;

			pKbdClassDevice = TryGetActiveKbdDevices(pKbdHidDriver, pKbdClassDevices, 10, &pPortDevice);
			if (pKbdClassDevice)
			{
				g_pKbdClassDevice = pKbdClassDevice;
				g_pi8042prtDevice = pPortDevice;

				g_bKbdAttachCheck_Inited = TRUE;

			}

			ObDereferenceObject(pKbdHidDriver);
		}
	}

	if (g_bKbdAttachCheck_Inited)
	{
		ObReferenceObject(g_pKbdClassDevice);
		ObReferenceObject(g_pi8042prtDevice);
	}

// deref all found KbdClassN
	for (i = 0; i < 10; i++)
	{
		if (pKbdClassDevices[i])
			ObDereferenceObject(pKbdClassDevices[i]);
	}

	bResult = OldKbdClassDevice != g_pKbdClassDevice || OldPortDevice != g_pi8042prtDevice;

	DbPrint(DC_SYS, DL_IMPORTANT,
		("KbdAttachCheck_ReInit: g_bKbdAttachCheck_Inited=%d, ReInit=%d\n", g_bKbdAttachCheck_Inited, bResult));

	return bResult;
}


VOID KbdAttach_BuildEvent(IN OUT PFILTER_EVENT_PARAM pParam, IN PDEVICE_OBJECT pDeviceObject, IN PDEVICE_OBJECT pLowerDevice)
{
	PSINGLE_PARAM pSingleParam;
	POBJECT_NAME_INFORMATION pDriverNameInfo;
	PWCHAR _DriverName;

	FILTER_PARAM_COMMONINIT(pParam, FLTTYPE_SYSTEM, 0x14/*MJ_SYSTEM_KEYLOGGER_DETECTED*/, 0, PostProcessing, 3);
	pParam->ProcessID = -1;
	pParam->ThreadID = -1;
	pSingleParam = (PSINGLE_PARAM)pParam->Params;

	SINGLE_PARAM_INIT_SID(pSingleParam);

	SINGLE_PARAM_SHIFT(pSingleParam);
	SINGLE_PARAM_INIT_ULONG(pSingleParam, _PARAM_OBJECT_KEYLOGGERID, KEYLOGGER_ATTACHKBD);

	SINGLE_PARAM_SHIFT(pSingleParam);
	SINGLE_PARAM_INIT_NONE(pSingleParam, _PARAM_OBJECT_URL_W);
	((PWCHAR)pSingleParam->ParamValue)[0] = 0;
	pSingleParam->ParamSize = sizeof(WCHAR);
	pDriverNameInfo = QueryObjectName(pDeviceObject->DriverObject);
	if (pDriverNameInfo)
	{
		memcpy(pSingleParam->ParamValue, pDriverNameInfo->Name.Buffer, pDriverNameInfo->Name.Length);
		((PWCHAR)pSingleParam->ParamValue)[pDriverNameInfo->Name.Length/sizeof(WCHAR)] = 0;
		pSingleParam->ParamSize = pDriverNameInfo->Name.Length+sizeof(WCHAR);

		ExFreeToNPagedLookasideList(gpPL0x1000, pDriverNameInfo);
	}
	_DriverName = (PWCHAR)pSingleParam->ParamValue;

	DbPrint(DC_SYS, DL_IMPORTANT, ("KbdAttach_BuildEvent: device 0x%08x of driver \"%S\" was attached to keyboard stack\n",
		pDeviceObject, _DriverName));
}

VOID KbdAttach_Check()
{
	PFILTER_EVENT_PARAM	pParam;
	PDEVICE_OBJECT pCurrDevice;

	KbdAttachCheck_ReInit();

	if (!g_bKbdAttachCheck_Inited)
		return;

	pParam = (PFILTER_EVENT_PARAM)ExAllocateFromNPagedLookasideList(gpPL0x1000);
	if (!pParam)
		return;

// идем от девайса i8042prt вверх по стеку до kbdclass
	pCurrDevice = g_pi8042prtDevice;
	while (pCurrDevice->AttachedDevice && pCurrDevice->AttachedDevice != g_pKbdClassDevice)
	{
		KbdAttach_BuildEvent(pParam, pCurrDevice->AttachedDevice, pCurrDevice);
		FilterEvent(pParam, NULL);

		pCurrDevice = pCurrDevice->AttachedDevice;
	}

// идем от девайса kbdclass0 вверх по стеку до вершины
	pCurrDevice = g_pKbdClassDevice;
	while (pCurrDevice->AttachedDevice)
	{
		KbdAttach_BuildEvent(pParam, pCurrDevice->AttachedDevice, pCurrDevice);
		FilterEvent(pParam, NULL);

		pCurrDevice = pCurrDevice->AttachedDevice;
	}

	ExFreeToNPagedLookasideList(gpPL0x1000, pParam);
}
//- Gruzdev ---------------------------------------------------------------------------------

VOID
KeyLoggers_CommonCheck()
{
	DbPrint(DC_SYS, DL_IMPORTANT, ("KeyLoggers_CommonCheck...\n"));
	
	ShadowTable_Check();
	KbdSplice_Check();
	KbdAttach_Check();

	DbPrint(DC_SYS, DL_IMPORTANT, ("KeyLoggers_CommonCheck complete\n"));
}

NTSTATUS
SysGetAppPath (
	ULONG ProcessId,
	PWCHAR pOutRequest,
	ULONG OutRequestSize,
	PULONG pRetSize
	)
{
	ProcInfoItem* pProcInfo = GetProcInfoByProcId( ProcessId );	
	ULONG retsize = OutRequestSize;
	
	*pRetSize = 0;

	if (!pProcInfo)
		return STATUS_NOT_FOUND;

	if (!pProcInfo->m_pwchmainImage)
		return STATUS_NOT_FOUND;

	__try
	{
		ParamPlaceValueStrW(pProcInfo->m_pwchmainImage, pOutRequest, &retsize);

		if (retsize < sizeof(WCHAR))
			return STATUS_NOT_FOUND;


	}
	__except(EXCEPTION_EXECUTE_HANDLER)
	{
		return STATUS_NOT_FOUND;
	}

	*pRetSize = retsize;

	return STATUS_SUCCESS;
}