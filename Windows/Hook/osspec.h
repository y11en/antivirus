#ifndef __OSSPEC_H
#define __OSSPEC_H

/*!
*		
*		
*		(C) 2001-3 Kaspersky Lab 
*		
*		\file	osspec.h
*		\brief	cистемные и вспомогательные функции
*		
*		\author Sergey Belov, Andrew Sobko
*		\date	12.09.2003 12:35:06
*		
*		
*		
*		
*/		



#include "defs.h"
#include "drvtypes.h"

#define SYSNAME    "System"
#define MakePtr( cast, ptr, addValue ) (cast)( (DWORD)(ptr)+(DWORD)(addValue))

//+ ------------------------------------------------------------------------------------------
#define BADP2LMAPPING		(ULONG*)0xFFFFFFFF
#define CR4_PSE	0x10
#define CR4_PAE	0x20 
#define PDE_PS	0x80		// 1000 0000
#define PDE_P		1		//present
#define PDE_W		2		//writable
#define PDE_U		4		//UserEnabled
#define PDE_TRN 0x800		//transition


#define MapP2L_phys			0xfffff000
#define Cr3PAE4k_phys		(~0x1f)
//+ ------------------------------------------------------------------------------------------
//+ for windows 9x platform
#ifdef _HOOK_VXD_

#define _STRING_DEF	"%s"

extern BOOLEAN bUpperTableInited;

extern PCHAR RegParams;

extern ULONG W16mtxOThOffset;

extern PCHAR VxDGetFunctionStr(PCHAR StrBuff, ULONG Hook_ID, ULONG FunctionMj, ULONG FunctionMi,...);
#define _GetFunctionStr VxDGetFunctionStr

#define DOS_YEAR(t)   (((t & 0xFE00) >> 9) + 1980)
#define DOS_MONTH(t)  ((t & 0x01E0) >> 5)
#define DOS_DAY(t)    (t & 0x001F)
#define DOS_HOUR(t)   ((t & 0xF800) >> 11)
#define DOS_MINUTE(t) ((t & 0x07E0) >> 5)
#define DOS_SECOND(t) ((t & 0x001F) << 1)
// -----------------------------------------------------------------------------------------
#define DISPATCH_LEVEL							2
#define KeGetCurrentIrql()						0

#define RtlZeroMemory(_p, _s)					memset(_p, 0, _s);

#define PSID									PVOID
#define PMDL									PVOID
#define FAST_MUTEX								MUTEXHANDLE
#define ERESOURCE								MUTEXHANDLE
#define PERESOURCE								MUTEXHANDLE*
#define KeEnterCriticalRegion()
#define KeLeaveCriticalRegion()
#define KeSetAffinityThread(Thread,AffinityMask) 0

#define ExInitializeResourceLite(pMutex)				*pMutex=CreateMutex(0,0)
#define ExDeleteResourceLite(pMutex)					DestroyMutex(*pMutex)		
#define ExAcquireResourceExclusiveLite(pMutex, pBool)	EnterMutex(*pMutex,BLOCK_SVC_INTS)
#define ExAcquireResourceSharedLite(pMutex, pBool)		EnterMutex(*pMutex,BLOCK_SVC_INTS)
#define ExReleaseResourceLite(pMutex)					LeaveMutex(*pMutex)
// -----------------------------------------------------------------------------------------
#define ExInitializeFastMutex(pMutex)			*pMutex=CreateMutex(0,0)
#define ExAcquireFastMutex(pMutex)				EnterMutex(*pMutex,BLOCK_SVC_INTS)
#define ExReleaseFastMutex(pMutex)				LeaveMutex(*pMutex)
#define ExDestroyFastMutex(pMutex)				DestroyMutex(*pMutex)
#define ExAllocatePoolWithTag(Flags,Size,Tag)	HeapAllocate(Size,0)
#define ExFreePool(Address)						HeapFree(Address,0)
#define KEVENT											SEMHANDLE
#define PKEVENT										PVOID
#define KeSetEvent(pEvent,PriorInc,Wait)		Signal_Semaphore_No_Switch(*pEvent)
#define KeInitializeEvent(pEvent,Type,State)	*pEvent=Create_Semaphore(State)
#define KeDestroyEvent(pEvent)					Destroy_Semaphore(*pEvent)
#define KeWaitForSingleObject(Object,WaitReason,WaitMode,Alertable,Timeout) Wait_Semaphore(*Object,BLOCK_SVC_INTS)
#define InitializeObjectAttributes(IniAttr,ObjName,Attr,RootDir,Secur)
#define KEY_VALUE_PARTIAL_INFORMATION			int
#define InterlockedIncrement(x)					(*x)++
#define InterlockedDecrement(x)					(*x)--
#define InterlockedExchange(_px, _y)			(*_px) = _y
#define PsGetCurrentThreadId Get_Cur_Thread_Handle
#define PsGetCurrentProcess	VWIN32_GetCurrentProcessHandle
#define PsGetCurrentProcessId VWIN32_GetCurrentProcessHandle
#define DbgPrint nprintf


#define DRIVE_UNKNOWN     0
#define DRIVE_NO_ROOT_DIR 1
#define DRIVE_REMOVABLE   2
#define DRIVE_FIXED       3
#define DRIVE_REMOTE      4
#define DRIVE_CDROM       5
#define DRIVE_RAMDISK     6


BYTE _UpperCharA(BYTE ch);
#define _UpperCharW(wIn) (WCHAR)_UpperCharA((BYTE)wIn)
unsigned int wcslen(PWCHAR wcs);

#define AcquireResource(pResource, bExclusive) EnterMutex(*pResource,BLOCK_SVC_INTS)
#define ReleaseResource(pResource)	LeaveMutex(* pResource)

extern ULONG Obsfucator;

typedef struct _WIN16MUTEX {
	ULONG	field_0;
	ULONG	OwnCnt;
	ULONG	OwnThrd;
	ULONG	field;
	ULONG	max;
	ULONG pKern32_crstLstMgr;
} WIN16MUTEX,*PWIN16MUTEX;

extern PWIN16MUTEX	pWin16Mutex;

#define  PMDL									PVOID


//NT specific
#define PAGE_SIZE											0x1000L
#define NTSTATUS											LONG
#define NT_SUCCESS(Status)							((NTSTATUS)(Status) == 0)
#define STATUS_SUCCESS									ERROR_SUCCESS
#define STATUS_INVALID_PARAMETER				ERROR_INVALID_PARAMETER
#define STATUS_INSUFFICIENT_RESOURCES		ERROR_NO_SYSTEM_RESOURCES
#define STATUS_MEMORY_NOT_ALLOCATED			ERROR_NOT_ENOUGH_MEMORY
#define STATUS_UNSUCCESSFUL							ERROR_TOO_MANY_SECRETS
#define STATUS_NOT_FOUND								ERROR_FILE_NOT_FOUND
#define STATUS_NO_MEMORY								ERROR_OUTOFMEMORY
#define STATUS_NOT_IMPLEMENTED					ERROR_CALL_NOT_IMPLEMENTED
#define STATUS_ACCESS_DENIED						ERROR_ACCESS_DENIED
#define STATUS_INVALID_BLOCK_LENGTH			ERROR_INVALID_BLOCK_LENGTH
#define STATUS_BUFFER_TOO_SMALL					ERROR_INSUFFICIENT_BUFFER
#define STATUS_OBJECT_NAME_NOT_FOUND		ERROR_FILE_NOT_FOUND
#define STATUS_NOT_SUPPORTED             ERROR_NOT_SUPPORTED

ULONG
IsWaitingSafe(VOID);

void
SetObsfucator(DWORD obsf);

PVOID GetExport(PVOID hModule,PCHAR FuncName,DWORD *ExportAddr);

#endif // _HOOK_VXD_

//+ ------------------------------------------------------------------------------------------
//+ for windows nt platform
#ifdef _HOOK_NT_

//_________________  MS MD  _______________________ 
// These defines don't doubled in ntifs.h
#ifndef IoSetNextIrpStackLocation
#define IoSetNextIrpStackLocation( Irp ) {      \
    (Irp)->CurrentLocation--;                   \
(Irp)->Tail.Overlay.CurrentStackLocation--; }
#endif

#ifndef IoCopyCurrentIrpStackLocationToNext
#define IoCopyCurrentIrpStackLocationToNext( Irp ) { \
    PIO_STACK_LOCATION irpSp; \
    PIO_STACK_LOCATION nextIrpSp; \
    irpSp = IoGetCurrentIrpStackLocation( (Irp) ); \
    nextIrpSp = IoGetNextIrpStackLocation( (Irp) ); \
    RtlCopyMemory( nextIrpSp, irpSp, FIELD_OFFSET(IO_STACK_LOCATION, CompletionRoutine)); \
	nextIrpSp->Control = 0; }
#endif

#ifndef	IoSkipCurrentIrpStackLocation
#define IoSkipCurrentIrpStackLocation( Irp ) {\
	(Irp)->CurrentLocation++; \
	(Irp)->Tail.Overlay.CurrentStackLocation++;}
#endif

extern BOOLEAN (__stdcall* _pfKeInsertQueueApc)(PKAPC Apc, PVOID SystemArgument1, PVOID SystemArgument2, UCHAR Mode);

extern NTSTATUS (__stdcall* _pfZwFreeVirtualMemory)(IN PHANDLE ProcessHandle, IN OUT PVOID *BaseAddress, 
				IN OUT PULONG RegionSize, IN ULONG FreeType);

//+ ------------------------------------------------------------------------------------------

#include <pshpack4.h>
typedef struct _USER_STACK {
	PVOID FixedStackBase;
	PVOID FixedStackLimit;
	PVOID ExpandableStackBase;
	PVOID ExpandableStackLimit;
	PVOID ExpandableStackBottom;
} USER_STACK, *PUSER_STACK;
#include <poppack.h>

//+ ------------------------------------------------------------------------------------------

extern NTSTATUS (__stdcall* _pfIoReadPartitionTable)(IN PDEVICE_OBJECT  DeviceObject,
				IN ULONG  SectorSize,
				IN BOOLEAN  ReturnRecognizedPartitions,
				OUT struct _DRIVE_LAYOUT_INFORMATION  **PartitionBuffer);

//+ ------------------------------------------------------------------------------------------
extern NTSTATUS (__stdcall* _pfIoReadPartitionTableEx)(IN PDEVICE_OBJECT  DeviceObject,
				IN /*struct _DRIVE_LAYOUT_INFORMATION_EX*/VOID  **PartitionBuffer); 

//+ ------------------------------------------------------------------------------------------

typedef enum _MM_PAGE_PRIORITY {
    LowPagePriority,
    NormalPagePriority = 16,
    HighPagePriority = 32
} MM_PAGE_PRIORITY;

typedef enum _MEMORYINFOCLASS {
MemoryBasicInformation,
MemoryEntryInformation
} MEMORYINFOCLASS;

extern PVOID (__stdcall *_pfMmMapLockedPages)(IN PMDL  MemoryDescriptorList, IN KPROCESSOR_MODE  AccessMode);

extern NTSTATUS (__stdcall *_pfRtlGetAce)(IN PACL Acl, ULONG AceIndex, OUT PVOID *Ace);
extern NTSTATUS (__stdcall *_pfZwSetSecurityObject)(IN HANDLE  Handle, IN SECURITY_INFORMATION  SecurityInformation, 
						IN PSECURITY_DESCRIPTOR  SecurityDescriptor);

extern NTSTATUS (__stdcall *_pfZwFsControlFile)(IN HANDLE  FileHandle, IN HANDLE  Event OPTIONAL, 
										 IN PIO_APC_ROUTINE  ApcRoutine OPTIONAL, IN PVOID  ApcContext OPTIONAL, 
										 OUT PIO_STATUS_BLOCK  IoStatusBlock, IN ULONG  FsControlCode, 
										 IN PVOID  InputBuffer OPTIONAL, IN ULONG  InputBufferLength,
										 OUT PVOID  OutputBuffer OPTIONAL, IN ULONG  OutputBufferLength);

extern VOID (__stdcall *_pfKeStackAttachProcess)(PKPROCESS Process, OUT ULONG* /*PRKAPC_STATE*/ ApcState);

extern VOID (__stdcall *_pfKeUnstackDetachProcess)(/*PRKAPC_STATE*/ ULONG* ApcState);

extern PVOID (__stdcall *_pfMmMapLockedPagesSpecifyCache)(
	IN PMDL  MemoryDescriptorList,
    IN KPROCESSOR_MODE  AccessMode,
    IN MEMORY_CACHING_TYPE  CacheType,
    IN PVOID  BaseAddress,
    IN ULONG  BugCheckOnFailure,
    IN MM_PAGE_PRIORITY  Priority
    );

#define __MmGetSystemAddressForMdl(MDL)                                  \
     (((MDL)->MdlFlags & (MDL_MAPPED_TO_SYSTEM_VA |                    \
                        MDL_SOURCE_IS_NONPAGED_POOL)) ?                \
                             ((MDL)->MappedSystemVa) :                 \
                             (_pfMmMapLockedPages((MDL),KernelMode)))

#define __MmGetSystemAddressForMdlSafe(MDL, PRIORITY)                    \
     (((MDL)->MdlFlags & (MDL_MAPPED_TO_SYSTEM_VA |                    \
                        MDL_SOURCE_IS_NONPAGED_POOL)) ?                \
                             ((MDL)->MappedSystemVa) :                 \
                             (_pfMmMapLockedPagesSpecifyCache((MDL),   \
                                                           KernelMode, \
                                                           MmCached,   \
                                                           NULL,       \
                                                           FALSE,      \
                                                           (PRIORITY))))

//+ ------------------------------------------------------------------------------------------



extern POBJECT_TYPE* IoDeviceObjectType;

#ifndef IRP_MN_KERNEL_CALL
#define IRP_MN_KERNEL_CALL  0x04
#endif

#ifndef FILE_DEVICE_DFS_FILE_SYSTEM
#define FILE_DEVICE_DFS_FILE_SYSTEM     0x00000035
#endif

#ifndef FILE_DEVICE_DFS_VOLUME
#define FILE_DEVICE_DFS_VOLUME 0x00000036
#endif

#define _STRING_DEF	"%S"

extern UNICODE_STRING RegParams;
extern PVOID _MmHighestUserAddress;
extern PVOID _MmUserProbeAddress;

extern PCHAR NTGetFunctionStr(PCHAR StrBuff, ULONG Hook_ID, ULONG FunctionMj, ULONG FunctionMi,...);
#define _GetFunctionStr NTGetFunctionStr

#define ExDestroyFastMutex(pMutex)
#define KeDestroyEvent(pEvent)
#define _UpperCharW				RtlUpcaseUnicodeChar

#define ERROR_SUCCESS                    0L
#define ERROR_ACCESS_DENIED              5L

#define HKEY HANDLE
typedef int BOOL;
typedef BOOL *PBOOL;
typedef int INT;
typedef unsigned int UINT;

typedef BYTE* PBYTE;

typedef VOID (*PENUM_DRIVERS_FUNC)(PDRIVER_OBJECT pDrv, PVOID pUserValue);

#define QUERY_DIRECTORY_BUFF_SIZE 0x200

#define _UpperCharA(ch) RtlUpperChar(ch)

#define AcquireResource(pResource, bExclusive) \
{ \
	KeEnterCriticalRegion();	\
	if(bExclusive) \
	ExAcquireResourceExclusiveLite(pResource, TRUE);\
	else \
	ExAcquireResourceSharedLite(pResource, TRUE); \
}

#define ReleaseResource(pResource) {ExReleaseResourceLite(pResource); KeLeaveCriticalRegion();}

#define IsWaitingSafe() 0

NTSTATUS
EnumDrivers(PENUM_DRIVERS_FUNC EnumFunc, PVOID pUserValue);

PWCHAR
Uwcsncpy(PWCHAR Dst, ULONG MaxDstLenInChars, PWCHAR Src, ULONG SrcLenInBytes);

ULONG
Uwcslen(PWCHAR pwchName, ULONG BufferLengthInWChars);

PVOID
GetNativeBase(PCHAR DllName, ULONG ValidateAddress);

PVOID
GetExport(PVOID hModule, PCHAR FuncName, DWORD *ExportAddr);

ULONG
GetProcessIDByHandle(HANDLE ProcessHandle, PPEB* ppPeb);

BOOLEAN
HookNtFunc(ULONG* InterceptFunc, ULONG NewHook, PCHAR Description);

PVOID
GetNtFuncByID(IN ULONG Service_ID, PVOID **p_pfServiceFunc OPTIONAL);

BOOLEAN
HookNtFuncByID(PVOID* InterceptFunc, PVOID NewHook, ULONG NativeID);

WCHAR
GetDeviceLetter(PDEVICE_OBJECT pDevice);

PWCHAR
GetDeviceName(PDEVICE_OBJECT pDevice, PWCHAR pDeviceName,ULONG BuffLen);

ULONG
GetProcessNameOffset();

//+ ------------------------------------------------------------------------------------------

BOOLEAN
MyVirtualProtect(PVOID VAddr,BOOLEAN Protect,BOOLEAN *OldProtect);

//+ ------------------------------------------------------------------------------------------

typedef enum _THREADINFOCLASS {
    ThreadBasicInformation,
    ThreadTimes,
    ThreadPriority,
    ThreadBasePriority,
    ThreadAffinityMask,
    ThreadImpersonationToken,
    ThreadDescriptorTableEntry,
    ThreadEnableAlignmentFaultFixup,
    ThreadEventPair,
    ThreadQuerySetWin32StartAddress,
    ThreadZeroTlsCell,
    ThreadPerformanceCount,
    ThreadAmILastThread,
    ThreadIdealProcessor,
    ThreadPriorityBoost,
    ThreadSetTlsArrayAddress,
    MaxThreadInfoClass
    } THREADINFOCLASS;


typedef struct _THREAD_BASIC_INFORMATION {
	NTSTATUS ExitStatus;
	PNT_TIB TebBaseAddress;
	CLIENT_ID ClientId;
	KAFFINITY AffinityMask;
	KPRIORITY Priority;
	KPRIORITY BasePriority;
} THREAD_BASIC_INFORMATION, *PTHREAD_BASIC_INFORMATION;
NTSTATUS (__stdcall *NtQueryInformationThread)(IN HANDLE ThreadHandle, IN THREADINFOCLASS ThreadInformationClass, OUT PVOID ThreadInformation, IN ULONG ThreadInformationLength, OUT PULONG ReturnLength OPTIONAL);

//NTSTATUS (__stdcall *NtQueryInformationThread)(IN HANDLE ThreadHandle, IN THREADINFOCLASS ThreadInformationClass, OUT PVOID ThreadInformation, IN ULONG ThreadInformationLength, OUT PULONG ReturnLength OPTIONAL);
//

NTSTATUS ZwQuerySystemInformation(IN ULONG InfoClass,OUT PVOID SysInfo,IN ULONG SysInfoLen,OUT PULONG LenReturned OPTIONAL);
NTSYSAPI NTSTATUS NTAPI ZwQueryInformationProcess(IN HANDLE ProcessHandle, IN ULONG ProcessInformationClass, OUT PVOID ProcessInformation, IN ULONG ProcessInformationLength, OUT PULONG ReturnLength OPTIONAL);

NTSYSAPI NTSTATUS NTAPI ZwOpenDirectoryObject(OUT PHANDLE DirectoryHandle,IN ACCESS_MASK DesiredAccess,IN POBJECT_ATTRIBUTES ObjectAttributes);
NTSTATUS (__stdcall *NtQueryDirectoryObject)(IN HANDLE DirectoryHandle,OUT PVOID Buffer,IN ULONG BuffLength,IN BOOLEAN ReturnSingleEntry,IN BOOLEAN RestartScan,IN OUT PULONG Context,OUT PULONG ReturnLength OPTIONAL);
NTSTATUS (__stdcall *NtReadVirtualMemory)(IN HANDLE ProcessHandle, IN PVOID BaseAddress, OUT PVOID Buffer, IN ULONG BufferLength, OUT PULONG ReturnLength OPTIONAL);

VOID KeInitializeApc(PKAPC Apc,PKTHREAD Thread,UCHAR	StateIndex,PKKERNEL_ROUTINE	KernelRoutine,PKRUNDOWN_ROUTINE	RundownRoutine,PKNORMAL_ROUTINE	NormalRoutine,UCHAR	ApcMode,PVOID	NormalContext);

ULONG KeSetAffinityThread(PETHREAD Thread,ULONG AffinityMask);
NTSYSAPI NTSTATUS NTAPI ZwOpenSymbolicLinkObject(OUT PHANDLE LinkHandle,IN ACCESS_MASK DesiredAccess,IN POBJECT_ATTRIBUTES ObjectAttributes);
NTSYSAPI NTSTATUS NTAPI ZwQuerySymbolicLinkObject(IN HANDLE LinkHandle,IN OUT PUNICODE_STRING LinkTarget,OUT PULONG ReturnedLength OPTIONAL);

//+ ------------------------------------------------------------------------------------------
extern NTSTATUS (__stdcall *_pfPoCallDriver)(IN PDEVICE_OBJECT DeviceObject, IN OUT PIRP Irp);
extern VOID (__stdcall *_pfPoStartNextPowerIrp)(IN PIRP Irp);

extern HANDLE (__stdcall *_pfPsGetThreadId)(PKTHREAD pkTh);
extern HANDLE (__stdcall *_pfPsGetThreadProcessId)(PKTHREAD pkTh);


//_________________________________________________

//BOOLEAN ObFindHandleForObject(IN PVOID Object,IN ACCESS_MASK DesiredAccess,IN POBJECT_TYPE ObjectType,IN KPROCESSOR_MODE AccessMode,OUT PHANDLE pHandle);
//NTSTATUS ObOpenObjectByPointer(IN PVOID Object,IN ULONG HandleAttributes,IN PACCESS_STATE PassedAccessState,IN ACCESS_MASK DesiredAccess,IN POBJECT_TYPE ObjectType,IN KPROCESSOR_MODE AccessMode,OUT PHANDLE Handle);

NTSTATUS ObOpenObjectByName(IN POBJECT_ATTRIBUTES ObjAttr,IN POBJECT_TYPE ObjectType,KPROCESSOR_MODE  ProcMode,PACCESS_STATE AccessState OPTIONAL,IN ACCESS_MASK DesiredAccess,ULONG U1_0,OUT PHANDLE pHandle);
//NTSTATUS ObQueryNameString(PVOID Object,PUNICODE_STRING Name,ULONG MaximumLength,PULONG ActualLength);

NTSTATUS ObReferenceObjectByName(IN PUNICODE_STRING ObjectPath,IN ULONG Attributes,
								 IN PACCESS_STATE PassedAccessState OPTIONAL,IN ACCESS_MASK DesiredAccess OPTIONAL,
								 IN POBJECT_TYPE ObjectType,IN KPROCESSOR_MODE AccessMode,
								 IN OUT PVOID ParseContext OPTIONAL,OUT PVOID *ObjectPtr);  
//The first parameter, ObjectPath, points to a counted unicode string that holds the path of the object, e.g. \Driver\Serial
//Attributes may be zero or OBJ_CASE_INSENSITIVE, which indicates that the name lookup should be performed ignoring the case of the ObjectName.
//Set parameter AccessMode to KernelMode.
//Parameter ObjectType optionally points to a 'type' object, which can be used to limit the search for the object to a particular type.
//ParseContext is an optional pointer that is passed uninterpreted to any parse procedure that is called during the course of performing the name lookup. 
//Parameter ObjectPtr is the address of the variable that receives a pointer to the object if the object is found 
//This service is only callable at PASSIVE_LEVEL.

#include <pshpack1.h>
typedef struct ServiceDescriptorEntry {
	unsigned int *ServiceTableBase;
	unsigned int *ServiceCounterTableBase; //Used only in checked build
	unsigned int NumberOfServices;
	unsigned char *ParamTableBase;
} ServiceDescriptorTableEntry_t, *PServiceDescriptorTableEntry_t;
#include <poppack.h>

__declspec(dllimport) _stdcall KeAddSystemServiceTable(PVOID, PVOID, PVOID, PVOID, PVOID);
__declspec(dllimport)  ServiceDescriptorTableEntry_t KeServiceDescriptorTable;

#define SYSTEMSERVICE_BY_FUNC_PTR(_function)  KeServiceDescriptorTable.ServiceTableBase[ *(PULONG)((PUCHAR)_function+1)]
#define SYSTEMSERVICE_BY_FUNC_ID(_func_id)  KeServiceDescriptorTable.ServiceTableBase[ _func_id]

extern DWORD ThreadIdOffsetInETHREAD;
extern ULONG ZwProtectVirtualMemoryNativeId;

BOOLEAN
GetNativeAddresses(VOID);

ULONG
GetRequestorThreadId(PIRP Irp);

BOOLEAN
GetThreadIdOffsetInETHREAD(VOID);

#define SetObsfucator(_obsf)

//+ ------------------------------------------------------------------------------------------
#endif // _HOOK_NT_

//+ ------------------------------------------------------------------------------------------
//+ common functions

#include "structures.h"

BOOLEAN
IsTimeExpired(__int64 ExpirationTime);

__int64
GetTimeShift(ULONG TimeShift);

PERESOURCE
InitKlgResourse(void);

PKLG_EVENT
InitKlgEvent(PCHAR WhistlerName);

void
DoneKlgEvent(PKLG_EVENT* pKlgEvent);

void
FreeKlgResourse(PERESOURCE *pMutex);

void
_UpperStringZeroTerninatedA(PCHAR pchin, PCHAR pchout);

void
_UpperStringZeroTerninatedW(PWCHAR pchin, PWCHAR pchout);

BOOLEAN
ExecutionOnHoldedThread(PVOID pInBuffer, DWORD dwInBufferSize, PVOID pOutBuffer, DWORD* pdwOutBufferSize);

BOOLEAN
IsLargePageNative(PVOID VirtAddr);

ULONG
GetNativeID(IN PVOID NativeBase, IN PSTR NativeName);

DWORD
GetSubstringPosBM(BYTE* Str, DWORD StrLen, BYTE* SubStr, DWORD SubStrLen);

//+ ------------------------------------------------------------------------------------------
typedef ULONGLONG* (*_tpGetPageDescriptor)(PVOID VirtAddr);
extern _tpGetPageDescriptor GetPageDescriptor;

typedef ULONGLONG*(*_tpGetPDEEntryNativeLite)(PVOID VirtAddr);
extern _tpGetPDEEntryNativeLite GetPDEEntryNativeLite;

typedef ULONGLONG*(*_tpGetPageDescriptorNativeLite)(PVOID VirtAddr);
extern _tpGetPageDescriptorNativeLite GetPageDescriptorNativeLite;

typedef ULONGLONG*(*_tpGetPageDescriptorNativeGeneric)(PVOID VirtAddr);
extern _tpGetPageDescriptorNativeGeneric GetPageDescriptorNative;

void DonePageDir(ULONG* PageDir);
//+ ------------------------------------------------------------------------------------------

BOOLEAN RecognizeFuncs(ULONG VirtAddr);

ULONG __fastcall
HalpAcquireHighLevelLock(ULONG *SpinLock);

VOID __fastcall
HalpReleaseHighLevelLock (ULONG *SpinLock,ULONG NewIrql);

PCHAR
GetProcName(PCHAR ProcessName, PVOID pIrp);

VOID __fastcall TouchPage(PVOID VAddr);

// статистика нагрузки
extern unsigned __int64 g_T;
extern unsigned __int64 g_TW;
extern LONG g_C;

#pragma warning (disable : 4035)
__inline unsigned __int64 GetCurTimeFast()
{
	// eax:edx
  __asm rdtsc
}
#pragma warning (default : 4035)


#endif // __OSSPEC_H