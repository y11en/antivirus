#ifndef __osspec_h
#define __osspec_h

#include <fltKernel.h>

VOID
SleepImp (
	__int64 ReqInterval
	);

VOID
SleepImpM (
	__int64 ReqIntervalMSec
	);


VOID
WaitInterlockedObj (
	PLONG pInterlocked
	);

PERESOURCE
InitKlgResourse ();

void
FreeKlgResourse (
	PERESOURCE *pMutex
	);


BOOLEAN
IsTimeExpired (
	__int64 ExpirationTime
	);

__int64
GetTimeShift (
	ULONG TimeShift
	);

void
_UpperStringZeroTerninatedW (
	PWCHAR pchin,
	PWCHAR pchout
	);

void
_UpperStringZeroTerninatedA (
	PCHAR pchin,
	PCHAR pchout
	);

ULONG
Uwcslen (
	PWCHAR pwchName,
	ULONG BufferLengthInWChars
	);

PWCHAR
Uwcsncpy (
	PWCHAR Dst,
	ULONG MaxDstLenInChars,
	PWCHAR Src,
	ULONG SrcLenInBytes
	);
//+ ------------------------------------------------------------------------------------------

PCHAR
GetShortProcName (
	PCHAR ProcessName,
	PEPROCESS requestor_proc
	);

VOID
GlobalOsSpecInit ();

#ifndef _WIN64
ULONG
GetProcessIDByHandle(
	__in HANDLE ProcessHandle, 
	__in PPEB* ppPeb
	);
#endif

BOOLEAN
HookNtFunc (
	__out PULONG pInterceptedFuncAddress,
	__in ULONG NewFuncAddress,
	__in PCHAR FuncName
	);

BOOLEAN
LinkKrnlFunc (
	__out PULONG pLinkedFunc,
	__in PCHAR FuncName
	);

PVOID
GetNtFunc (
		   __in PCHAR cFuncName
		   );

//+ ------------------------------------------------------------------------------------------
ULONG
CheckException (
	);


#if WINVER<0x0501

_CRTIMP int __cdecl swprintf(wchar_t *, const wchar_t *, ...);

NTSTATUS
RtlStringCbCopyA(
    PSTR pszDest,
    size_t cbDest,
    PCSTR pszSrc
	);

NTSTATUS
RtlStringCbPrintfW(
    PWSTR pszDest,
    size_t cbDest,
    PCWSTR pszFormat,
    ...);

NTSTATUS
RtlStringCbCopyW(
    PWSTR pszDest,
    size_t cbDest,
    PCWSTR pszSrc
	);

NTSTATUS
RtlStringCbCopyNW(
	PWSTR pszDest,
	size_t cbDest,
	PCWSTR pszSrc,
	size_t cbToCopy);

NTSTATUS
RtlStringCbCatW(
    PWSTR pszDest,
    size_t cbDest,
    PCWSTR pszSrc
	);

NTSTATUS
RtlStringCbCatNW(
    PWSTR pszDest,
    size_t cbDest,
    PCWSTR pszSrc,
    size_t cbToAppend
	);

NTSTATUS
RtlStringCbCopyUnicodeString(
    PWSTR pszDest,
    size_t cbDest,
    PCUNICODE_STRING SourceString
	);

#endif // win2k

PVOID
NTAPI
Tables_Allocate (
	PRTL_GENERIC_TABLE pTable,
	CLONG  ByteSize
	);

VOID
NTAPI
Tables_Free (
	PRTL_GENERIC_TABLE pTable,
	PVOID  Buffer
	);

PVOID
CopyUserBuffer (
	PVOID pContext,
	ULONG Size,
	PIRP Irp
	);

#ifndef _WIN64

#include <pshpack1.h>

typedef struct ServiceDescriptorEntry {
	unsigned int *ServiceTableBase;
	unsigned int *ServiceCounterTableBase; //Used only in checked build
	unsigned int NumberOfServices;
	unsigned char *ParamTableBase;
} ServiceDescriptorTableEntry_t, *PServiceDescriptorTableEntry_t;

#include <poppack.h>

#define SYSTEMSERVICE_BY_FUNC_ID(_func_id)  KeServiceDescriptorTable.ServiceTableBase[_func_id]
__declspec(dllimport)  ServiceDescriptorTableEntry_t KeServiceDescriptorTable;

PServiceDescriptorTableEntry_t GetAddrOfShadowTable();

#endif -----------------------------------------------------------------------

#include <pshpack1.h>

typedef struct _SYSTEM_MODULE_INFORMATION {
	ULONG		Reserved[2];
	PVOID		Base;
	ULONG		Size;
	ULONG		Flags;
	USHORT		Index;
	USHORT		Unknown;
	USHORT		LoadCount;
	USHORT		ModNameOffset;
	CHAR		ImageName[256];
} SYSTEM_MODULE_INFORMATION, *PSYSTEM_MODULE_INFORMATION;

typedef struct _SYS_MOD_INFO {
	ULONG	NumberOfModules;
	SYSTEM_MODULE_INFORMATION Module[1];
} SYS_MOD_INFO, *PSYS_MOD_INFO;

#include <poppack.h>

int __cdecl mbtowc(
	__out wchar_t * _DstCh,
	__in const char * _SrcCh,
	__in size_t _SrcSizeInBytes
	);

NTSYSAPI
NTSTATUS NTAPI ObReferenceObjectByName(
	__in PUNICODE_STRING ObjectPath,
	__in ULONG Attributes,
	__in_opt PACCESS_STATE PassedAccessState,
	__in_opt ACCESS_MASK DesiredAccess,
	__in POBJECT_TYPE ObjectType,
	__in KPROCESSOR_MODE AccessMode,
	__inout_opt PVOID ParseContext,
	__out PVOID *ObjectPtr
	);  

typedef unsigned int UINT;

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
	__in RTL_ATOM Atom,
	__in ATOM_INFORMATION_CLASS AtomInformationClass,
	__out PVOID AtomInformation,
	__in ULONG AtomInformationLength,
	__out_opt PULONG ReturnLength
	);

#define THREAD_QUERY_INFORMATION       (0x0040) 

#include <pshpack1.h>

typedef struct _THREAD_BASIC_INFORMATION {
	NTSTATUS ExitStatus;
	PNT_TIB TebBaseAddress;
	CLIENT_ID ClientId;
	KAFFINITY AffinityMask;
	KPRIORITY Priority;
	KPRIORITY BasePriority;
} THREAD_BASIC_INFORMATION, *PTHREAD_BASIC_INFORMATION;

#include <poppack.h>

NTSYSAPI
NTSTATUS
NTAPI
ZwOpenThread(
			 OUT PHANDLE ThreadHandle,
			 IN ACCESS_MASK DesiredAccess,
			 IN POBJECT_ATTRIBUTES ObjectAttributes,
			 IN PCLIENT_ID ClientId
			 );


PVOID GetNtFuncByID(
					__in ULONG ServiceID,
					__out_opt PVOID **p_pfServiceFunc
					);
BOOLEAN HookNtFuncByID(
					   __in ULONG ServiceID,
					   __in PVOID fHookFunc,
					   __out PVOID *p_fServiceFunc,
					   __out_opt PVOID **p_pfServiceFunc
					   );

// returnes first occurence!
// returned PROCESS object should be dereferenced after use!
PEPROCESS LookupProcessByName(
							  __in PWCHAR ProcessName
							  );
PVOID GetPEBAddress();
// lookup module with known HMODULE in current process
BOOLEAN LookupModuleNameByBase(
	__in PVOID ModuleBase,
	__out_opt PWCHAR FullName,
	__in ULONG cbFullName,
	__out_opt PWCHAR BaseName,
	__in ULONG cbBaseName
	);
NTSTATUS ReferenceBaseDeviceByName(
	__in PUNICODE_STRING DevicePath,
	__out PDEVICE_OBJECT *p_pDeviceObject
								   );
// free returned string with ExFreePool
PWCHAR Sys_QueryObjectName(
	__in PVOID pObject
					   );
BOOLEAN IsSafeAddress(
	__in PVOID Address,
	__in ULONG Size
	);

#endif __osspec_h

