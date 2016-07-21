#ifndef __hook_h
#define __hook_h

#include <ntimage.h>

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

NTSYSAPI
NTSTATUS
NTAPI
ZwQueryInformationProcess (
	IN HANDLE ProcessHandle,
	IN ULONG ProcessInformationClass,
	OUT PVOID ProcessInformation,
	IN ULONG ProcessInformationLength,
	OUT PULONG ReturnLength OPTIONAL
	);

NTSYSAPI
NTSTATUS
NTAPI
ZwOpenProcess (
	OUT PHANDLE ProcessHandle,
	IN ACCESS_MASK DesiredAccess,
	IN POBJECT_ATTRIBUTES ObjectAttributes,
	IN PCLIENT_ID ClientId OPTIONAL
);

#include <pshpack1.h>

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

typedef struct _SYSTEM_THREADS {
	LARGE_INTEGER KernelTime;
	LARGE_INTEGER UserTime;
	LARGE_INTEGER CreateTime;
	ULONG WaitTime;
#ifdef _WIN64
	ULONG Filler[5];
#endif
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
#ifdef _WIN64
	ULONG Filler;
#endif
	HANDLE ProcessId;
	HANDLE InheritedFromProcessId;
	ULONG HandleCount;
	ULONG Reserved2;
#ifndef _WIN64
	ULONG Reserved3;
#endif
	VM_COUNTERS VmCounters;
	IO_COUNTERS IoCounters; // Windows 2000 only
	SYSTEM_THREADS Threads[1];
} SYSTEM_PROCESSES, *PSYSTEM_PROCESSES;


typedef enum _SYSTEM_INFORMATION_CLASS {
	SystemBasicInformation, // 0 Y N
	SystemProcessorInformation, // 1 Y N
	SystemPerformanceInformation, // 2 Y N
	SystemTimeOfDayInformation, // 3 Y N
	SystemNotImplemented1, // 4 Y N
	SystemProcessesAndThreadsInformation, // 5 Y N
	SystemCallCounts, // 6 Y N
	SystemConfigurationInformation, // 7 Y N
	SystemProcessorTimes, // 8 Y N
	SystemGlobalFlag, // 9 Y Y
	SystemNotImplemented2, // 10 Y N
	SystemModuleInformation, // 11 Y N
	SystemLockInformation, // 12 Y N
	SystemNotImplemented3, // 13 Y N
	SystemNotImplemented4, // 14 Y N
	SystemNotImplemented5, // 15 Y N
	SystemHandleInformation, // 16 Y N
	SystemObjectInformation, // 17 Y N
	SystemPagefileInformation, // 18 Y N
	SystemInstructionEmulationCounts, // 19 Y N
	SystemInvalidInfoClass1, // 20
	SystemCacheInformation, // 21 Y Y
	SystemPoolTagInformation, // 22 Y N
	SystemProcessorStatistics, // 23 Y N
	SystemDpcInformation, // 24 Y Y
	SystemNotImplemented6, // 25 Y N
	SystemLoadImage, // 26 N Y
	SystemUnloadImage, // 27 N Y
	SystemTimeAdjustment, // 28 Y Y
	SystemNotImplemented7, // 29 Y N
	SystemNotImplemented8, // 30 Y N
	SystemNotImplemented9, // 31 Y N
	SystemCrashDumpInformation, // 32 Y N
	SystemExceptionInformation, // 33 Y N
	SystemCrashDumpStateInformation, // 34 Y Y/N
	SystemKernelDebuggerInformation, // 35 Y N
	SystemContextSwitchInformation, // 36 Y N
	SystemRegistryQuotaInformation, // 37 Y Y
	SystemLoadAndCallImage, // 38 N Y
	SystemPrioritySeparation, // 39 N Y
	SystemNotImplemented10, // 40 Y N
	SystemNotImplemented11, // 41 Y N
	SystemInvalidInfoClass2, // 42
	SystemInvalidInfoClass3, // 43
	SystemTimeZoneInformation, // 44 Y N
	SystemLookasideInformation, // 45 Y N
	SystemSetTimeSlipEvent, // 46 N Y
	SystemCreateSession, // 47 N Y
	SystemDeleteSession, // 48 N Y
	SystemInvalidInfoClass4, // 49
	SystemRangeStartInformation, // 50 Y N
	SystemVerifierInformation, // 51 Y Y
	SystemAddVerifier, // 52 N Y
	SystemSessionProcessesInformation // 53 Y N
} SYSTEM_INFORMATION_CLASS;

// from winnt.h
#define PROCESS_TERMINATE                  (0x0001)  
#define PROCESS_CREATE_THREAD              (0x0002)  
#define PROCESS_SET_SESSIONID              (0x0004)  
#define PROCESS_VM_OPERATION               (0x0008)  
#define PROCESS_VM_READ                    (0x0010)  
#define PROCESS_VM_WRITE                   (0x0020)  
#define PROCESS_DUP_HANDLE                 (0x0040)  
#define PROCESS_CREATE_PROCESS             (0x0080)  
#define PROCESS_SET_QUOTA                  (0x0100)  
#define PROCESS_SET_INFORMATION            (0x0200)  
#define PROCESS_QUERY_INFORMATION          (0x0400)  
#define PROCESS_SUSPEND_RESUME             (0x0800)  
#define PROCESS_QUERY_LIMITED_INFORMATION  (0x1000)  

NTSYSAPI
NTSTATUS
NTAPI
ZwQuerySystemInformation(
	IN SYSTEM_INFORMATION_CLASS SystemInformationClass,
	IN OUT PVOID SystemInformation,
	IN ULONG SystemInformationLength,
	OUT PULONG ReturnLength OPTIONAL
);

#define VALID_RVA(__rva1__, __hdr1__)		(GetEnclosingSectionHeader(__rva1__, __hdr1__)?TRUE:FALSE)

#define PTR_FROM_RVA(__img_base__, __hdr__, __rva__)	(VALID_RVA(__rva__, __hdr__)?(PCHAR)__img_base__+(ULONG_PTR)__rva__:NULL)
#define RVA_FROM_PTR(__img_base__, __ptr__)				((ULONG)((ULONG_PTR)__ptr__-(ULONG_PTR)__img_base__))

#define IMG_DIR_ENTRY_RVA(__hdr__, __i__)	(__hdr__->OptionalHeader.DataDirectory[__i__].VirtualAddress)
#define IMG_DIR_ENTRY_SIZE(__hdr__, __i__)	(__hdr__->OptionalHeader.DataDirectory[__i__].Size)

#define PAGE_BASE(__ptr__)					((ULONG_PTR)__ptr__ & ~((ULONG_PTR)PAGE_SIZE-1LL))
#define PAGE_OFFSET(__ptr__)				((ULONG_PTR)__ptr__ & ((ULONG_PTR)PAGE_SIZE-1LL))

PIMAGE_SECTION_HEADER GetEnclosingSectionHeader(ULONG RVA, PIMAGE_NT_HEADERS pNtHeader);
PIMAGE_SECTION_HEADER GetFollowingSectionHeader(PIMAGE_NT_HEADERS pNtHeader, PIMAGE_SECTION_HEADER pSectionHeader);


PVOID GetExport(IN PVOID ImageBase, IN PCHAR NativeName, OUT PVOID *p_ExportAddr OPTIONAL, OUT PULONG pNativeSize OPTIONAL);

ULONG
GetNativeID (
	__in PVOID NativeBase,
	__in PSTR NativeName
	);

#endif __hook_h