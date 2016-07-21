#include "osspec.h"
#include "tags.h"
#include "debug.h"

#include "mtypes.h"
#include "..\hook\avpgcom.h"
#include "resolve.h"
#include "hook.h"

#include "osspec.tmh"

#define SYSNAME    "System"
ULONG gProcessNameOffset = 0;

#ifndef _WIN64
	NTKERNELAPI BOOLEAN KeAddSystemServiceTable(
			IN PULONG_PTR Base,
			IN PULONG Count OPTIONAL,
			IN ULONG Limit,
			IN PUCHAR Number,
			IN ULONG Index
			);


	PVOID gBaseOfNtDllDll = 0;
	PVOID gBaseOfNtOsKrnl = 0;

PVOID
GetNativeBase (
	PCHAR DllName
	)
{
	ULONG		BufLen;
	ULONG		i;
	PVOID		ret = NULL;
	PULONG		qBuff;
	PSYSTEM_MODULE_INFORMATION Mod;
	NTSTATUS status = ZwQuerySystemInformation( 11, &BufLen, 0, &BufLen );//

	if (STATUS_INFO_LENGTH_MISMATCH != status || !BufLen)
		return NULL;

	qBuff = ExAllocatePoolWithTag( PagedPool, BufLen, tag_tmp_buf );
	if(!qBuff)
		return NULL;

	status = ZwQuerySystemInformation( 11, qBuff, BufLen, NULL );
	if(NT_SUCCESS( status ))
	{
		Mod = (PSYSTEM_MODULE_INFORMATION)( qBuff + 1 );
		for(i = 0; i < *qBuff; i++)
		{
			if(!_stricmp( Mod[i].ImageName + Mod[i].ModNameOffset, DllName ))
			{
				ret = Mod[i].Base;
				break;
			}
		}
	}

	ExFreePool( qBuff );

	return ret;
}

#endif

VOID
SleepImp (
	__int64 ReqInterval
	)
{
	LARGE_INTEGER	Interval;
	*(__int64*)&Interval=-(ReqInterval*10000000L);
	KeDelayExecutionThread( KernelMode, FALSE, &Interval );
}

VOID
SleepImpM (
	__int64 ReqIntervalMSec
	)
{
	LARGE_INTEGER	Interval;
	*(__int64*)&Interval=-(ReqIntervalMSec*10000L);
	KeDelayExecutionThread( KernelMode, FALSE, &Interval );
}


VOID
WaitInterlockedObj (
	PLONG pInterlocked
	)
{
	while (1 != InterlockedIncrement( pInterlocked ))
	{
		InterlockedDecrement( pInterlocked );

		SleepImp( 1 );
	}
}

PERESOURCE
InitKlgResourse (
	)
{
	PERESOURCE pMutex = (PERESOURCE) ExAllocatePoolWithTag(NonPagedPool, sizeof(ERESOURCE), 'MboS');
	if (pMutex != NULL)
		ExInitializeResourceLite(pMutex);

	return pMutex;
}

void
FreeKlgResourse (
	PERESOURCE *pMutex
	)
{
	if (pMutex != NULL)
	{
		if (*pMutex != NULL)
		{
			ExDeleteResourceLite(*pMutex);
			ExFreePool(*pMutex);
			*pMutex = NULL;
		}
	}
}

BOOLEAN
IsTimeExpired (
	__int64 ExpirationTime
	)
{
	__int64	SysTime;

	KeQuerySystemTime((PLARGE_INTEGER)&SysTime);
	if(SysTime > ExpirationTime)
	{
		//		DbPrint(DC_FILTER,DL_SPAM, ("Expired cobject found\n"));
		return TRUE;
	}

	return FALSE;
}

__int64
GetTimeShift (
	ULONG TimeShift
	)
{
	__int64	SysTime;
	KeQuerySystemTime((PLARGE_INTEGER)&SysTime);
	SysTime += TimeShift * 10000L;		// in Sec/1000 intervals

	return SysTime;
}

void _UpperStringZeroTerninatedA (
	PCHAR pchin,
	PCHAR pchout
	)
{
	if (pchin == NULL || pchout == NULL)
		return;
	while (*pchin != 0)
	{
		*pchout = RtlUpperChar(*pchin);
		pchin++;
		pchout++;
	}
}

void _UpperStringZeroTerninatedW (
	PWCHAR pchin,
	PWCHAR pchout
	)
{
	if (pchin == NULL || pchout == NULL)
		return;
	while (*pchin != 0)
	{
		*pchout = RtlUpcaseUnicodeChar(*pchin);
		pchin++;
		pchout++;
	}
}

ULONG
Uwcslen (
	PWCHAR pwchName,
	ULONG BufferLengthInWChars
	)
{
	ULONG cou = 0;

	while (cou < BufferLengthInWChars)
	{
		if (pwchName[cou] == 0)
			return cou;

		cou++;
	}

	if (cou == 0)
		return 0;

	return BufferLengthInWChars;
}

PWCHAR
Uwcsncpy (
	PWCHAR Dst,
	ULONG MaxDstLenInChars,
	PWCHAR Src,
	ULONG SrcLenInBytes)
{
	ULONG CopyLen;
	CopyLen = SrcLenInBytes/sizeof(WCHAR);
	if(CopyLen > MaxDstLenInChars)
		CopyLen = MaxDstLenInChars - 1;
	memcpy(Dst,Src,CopyLen*sizeof(WCHAR));
	Dst[CopyLen] = 0;
	
	return Dst;
}


ULONG
GetProcessNameOffset (
	)
{
	int i;
	PEPROCESS pSystemProcess = IoGetCurrentProcess();

	for(i = 0;i < 0x3000; i++)
	{
		if(!strncmp(SYSNAME, (PCHAR)pSystemProcess + i, strlen(SYSNAME)))
			return i;
	}

	return 0;
}

PCHAR
GetShortProcName (
	PCHAR ProcessName,
	PEPROCESS requestor_proc
	)
{
	PEPROCESS curproc;
	if(requestor_proc)
		curproc = requestor_proc;
	else
		curproc = IoGetCurrentProcess();
	__try
	{
		if(curproc && gProcessNameOffset)
		{
			strncpy(ProcessName,(PCHAR)curproc + gProcessNameOffset, PROCNAMELEN - 1);
			return ProcessName;
		}
	}
	__except ( CheckException() )
	{
		memset(ProcessName, 0, PROCNAMELEN);
	}

	return ProcessName;
}

//+ ------------------------------------------------------------------------------------------

VOID
GlobalOsSpecInit (
	)
{
	gProcessNameOffset = GetProcessNameOffset();

#ifndef _WIN64
	if (FlagOn( Globals.m_DrvFlags, _DRV_FLAG_HAS_SYSPATCH ))
	{
		ULONG _os_min_base = 0x80000000;
		DWORD OsBase = (DWORD)IoAllocateIrp;;

		gBaseOfNtDllDll = GetNativeBase( "NTDLL.DLL" );

		OsBase &= ~0xFFF;
		while (TRUE)
		{
			__try
			{
				if(*(short*)OsBase == 'ZM')
					break;

				OsBase -= PAGE_SIZE;
				if(OsBase <= _os_min_base)
				{
					OsBase = 0;
					break;
				}

			} __except(EXCEPTION_EXECUTE_HANDLER)
			{
				OsBase = 0;
				break;
			}
		}

		if (OsBase)
		{
			if((GetExport((PVOID) OsBase,"IoCallDriver", NULL, NULL)))
				gBaseOfNtOsKrnl = (PVOID)OsBase;
		}
	}
#endif
}

//+ ------------------------------------------------------------------------------------------
ULONG
CheckException (
	)
{
	return EXCEPTION_EXECUTE_HANDLER;
}

#if WINVER<0x0501

typedef __nullterminated char* NTSTRSAFE_PSTR;
typedef __nullterminated const char* NTSTRSAFE_PCSTR;
typedef __nullterminated wchar_t* NTSTRSAFE_PWSTR;
typedef __nullterminated const wchar_t* NTSTRSAFE_PCWSTR;
typedef __nullterminated const wchar_t UNALIGNED* NTSTRSAFE_PCUWSTR;

#define NTSTRSAFE_MAX_CCH     2147483647  // max buffer size, in characters, that we support (same as INT_MAX)
#define NTSTRSAFE_MAX_LENGTH  (NTSTRSAFE_MAX_CCH - 1)   // max buffer length, in characters, that we support

NTSTATUS
RtlStringCopyWorkerW(
	 __out_ecount(cchDest) wchar_t* pszDest,
	 __in size_t cchDest,
	 __out_opt size_t* pcchNewDestLength,
	 __in NTSTRSAFE_PCWSTR pszSrc,
	 __in size_t cchToCopy
	 )
{
	NTSTATUS status = STATUS_SUCCESS;
	size_t cchNewDestLength = 0;

	// ASSERT(cchDest != 0);

	while (cchDest && cchToCopy && (*pszSrc != L'\0'))
	{
		*pszDest++ = *pszSrc++;
		cchDest--;
		cchToCopy--;

		cchNewDestLength++;
	}

	if (cchDest == 0)
	{
		// we are going to truncate pszDest
		pszDest--;
		cchNewDestLength--;

		status = STATUS_BUFFER_OVERFLOW;
	}

	*pszDest= L'\0';

	if (pcchNewDestLength)
	{
		*pcchNewDestLength = cchNewDestLength;
	}

	return status;
}

NTSTATUS
RtlStringLengthWorkerW (
	__in NTSTRSAFE_PCWSTR psz,
	__in size_t cchMax,
	__out_opt size_t* pcchLength
	)
{
	NTSTATUS status = STATUS_SUCCESS;
	size_t cchOriginalMax = cchMax;

	while (cchMax && (*psz != L'\0'))
	{
		psz++;
		cchMax--;
	}

	if (cchMax == 0)
	{
		// the string is longer than cchMax
		status = STATUS_INVALID_PARAMETER;
	}

	if (pcchLength)
	{
		if (NT_SUCCESS(status))
		{
			*pcchLength = cchOriginalMax - cchMax;
		}
		else
		{
			*pcchLength = 0;
		}
	}

	return status;
}

NTSTATUS
RtlStringValidateDestW (
	__in_ecount_opt(cchDest) NTSTRSAFE_PWSTR pszDest,
	__in size_t cchDest,
	__out_opt size_t* pcchDestLength,
	__in size_t cchMax
	)
{
	NTSTATUS status = STATUS_SUCCESS;

	if ((cchDest == 0) || (cchDest > cchMax))
	{
		status = STATUS_INVALID_PARAMETER;
	}

	if (pcchDestLength)
	{
		if (NT_SUCCESS(status))
		{
			status = RtlStringLengthWorkerW(pszDest, cchDest, pcchDestLength);
		}
		else
		{
			*pcchDestLength = 0;
		}
	}

	return status;
}

NTSTATUS
RtlStringCbCopyA (
				  PSTR pszDest,
				  size_t cbDest,
				  PCSTR pszSrc
				  )
{
	UNREFERENCED_PARAMETER( cbDest );
	strcpy( pszDest, pszSrc );

	return STATUS_SUCCESS;
}

NTSTATUS
RtlStringCbCopyW (
	PWSTR pszDest,
	size_t cbDest,
	PCWSTR pszSrc
	)
{
	NTSTATUS status;
	size_t cchDest = cbDest / sizeof(wchar_t);

	status = RtlStringValidateDestW(pszDest, cchDest, NULL, NTSTRSAFE_MAX_CCH);

	if (NT_SUCCESS(status))
	{
		status = RtlStringCopyWorkerW(pszDest,
			cchDest,
			NULL,
			pszSrc,
			NTSTRSAFE_MAX_LENGTH);
	}

	return status;
}

NTSTATUS
RtlStringCbCopyNW (
	PWSTR pszDest,
	size_t cbDest,
	PCWSTR pszSrc,
	size_t cbToCopy
	)
{
	NTSTATUS status;
	size_t cchDest = cbDest / sizeof(wchar_t);

	status = RtlStringValidateDestW(pszDest, cchDest, NULL, NTSTRSAFE_MAX_CCH);

	if (NT_SUCCESS(status))
	{
		size_t cchToCopy = cbToCopy / sizeof(wchar_t);

		if (cchToCopy > NTSTRSAFE_MAX_LENGTH)
		{
			status = STATUS_INVALID_PARAMETER;

			*pszDest = L'\0';
		}
		else
		{
			status = RtlStringCopyWorkerW(pszDest,
				cchDest,
				NULL,
				pszSrc,
				cchToCopy);
		}
	}

	return status;
}

NTSTATUS
RtlStringCbCatW (
	PWSTR pszDest,
	size_t cbDest,
	PCWSTR pszSrc
	)
{
	NTSTATUS status;
	size_t cchDestLength;
	size_t cchDest = cbDest / sizeof(wchar_t);

	status = RtlStringValidateDestW(pszDest, cchDest, &cchDestLength, NTSTRSAFE_MAX_CCH);

	if (NT_SUCCESS(status))
	{
		status = RtlStringCopyWorkerW(pszDest + cchDestLength,
			cchDest - cchDestLength,
			NULL,
			pszSrc,
			NTSTRSAFE_MAX_CCH);
	}

	return status;
}

NTSTATUS
RtlStringCbCatNW (
	PWSTR pszDest,
	size_t cbDest,
	PCWSTR pszSrc,
	size_t cbToAppend
	)
{
	NTSTATUS status;
	size_t cchDest = cbDest / sizeof(wchar_t);
	size_t cchDestLength;

	status = RtlStringValidateDestW(pszDest, cchDest, &cchDestLength, NTSTRSAFE_MAX_CCH);

	if (NT_SUCCESS(status))
	{
		size_t cchToAppend = cbToAppend / sizeof(wchar_t);

		if (cchToAppend > NTSTRSAFE_MAX_LENGTH)
		{
			status = STATUS_INVALID_PARAMETER;
		}
		else
		{
			status = RtlStringCopyWorkerW(pszDest + cchDestLength,
				cchDest - cchDestLength,
				NULL,
				pszSrc,
				cchToAppend);
		}
	}

	return status;
}

NTSTATUS
RtlStringCbCopyUnicodeString (
	PWSTR pszDest,
	size_t cbDest,
	PCUNICODE_STRING SourceString
	)
{
	memset( pszDest, 0, cbDest );
	memcpy( pszDest, SourceString, SourceString->Length );

	return STATUS_SUCCESS;
}

#endif

PVOID
NTAPI
Tables_Allocate (
	PRTL_GENERIC_TABLE pTable,
	CLONG  ByteSize
	)
{
	PVOID ptr = NULL;
	UNREFERENCED_PARAMETER( pTable );

	ptr = ExAllocatePoolWithTag( PagedPool, ByteSize, tag_table_sys );

	return ptr;
}

VOID
NTAPI
Tables_Free (
	PRTL_GENERIC_TABLE pTable,
	PVOID  Buffer
	)
{
	UNREFERENCED_PARAMETER( pTable );

	ExFreePool( Buffer );
}

PVOID
CopyUserBuffer (
	PVOID pContext,
	ULONG Size,
	PIRP Irp
	)
{
	PVOID pBuffer = NULL;

	PMDL pmdl = NULL;
	KPROCESSOR_MODE PreviousMode = UserMode;

	if (Irp)
		PreviousMode = Irp->RequestorMode;
	else
		PreviousMode = ExGetPreviousMode();

	if ( !pContext || !Size)
		return NULL;

	if (UserMode == PreviousMode)
	{
		if (!MmIsAddressValid( pContext ))
			return NULL;

		pmdl = IoAllocateMdl( pContext, Size, FALSE, FALSE, NULL);
		if (!pmdl)
			return NULL;

		__try
		{
			MmProbeAndLockPages( pmdl, UserMode, IoReadAccess );
		}
		__except(EXCEPTION_EXECUTE_HANDLER)
		{
			IoFreeMdl( pmdl );
			return NULL;
		}
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

#ifndef _WIN64
ULONG
GetProcessIDByHandle(
	__in HANDLE ProcessHandle, 
	__in PPEB* ppPeb
)
{
	NTSTATUS ntStatus;
	PROCESS_BASIC_INFORMATION ProcInfo;

	ntStatus = ZwQueryInformationProcess(ProcessHandle, ProcessBasicInformation, &ProcInfo, sizeof(ProcInfo), NULL);
	if (STATUS_SUCCESS == ntStatus)
	{
		if (ppPeb)
			*ppPeb = ProcInfo.PebBaseAddress;

		return ProcInfo.UniqueProcessId;
	}

	return 0;
}
#endif

BOOLEAN
HookNtFunc (
	__out PULONG pInterceptedFuncAddress,
	__in ULONG NewFuncAddress,
	__in PCHAR FuncName
	)
{
	BOOLEAN bPatched = FALSE;

#ifdef _WIN64
	UNREFERENCED_PARAMETER( pInterceptedFuncAddress );
	UNREFERENCED_PARAMETER( NewFuncAddress );
	UNREFERENCED_PARAMETER( FuncName );
#else
	if (!FlagOn( Globals.m_DrvFlags, _DRV_FLAG_HAS_SYSPATCH ))
		return FALSE;

	if (g_bIsW2003 || g_bIsW2K || g_bIsWXP)
	{
		// only for win2k/xp/2003
		ULONG NativeID = GetNativeID( gBaseOfNtDllDll, FuncName );

		if (NativeID)
		{
			*pInterceptedFuncAddress = SYSTEMSERVICE_BY_FUNC_ID( NativeID );

			__asm { // снимаем защиту памяти от записи 
				mov  ebx , cr0
					push ebx
					and  ebx , ~0x10000
					mov  cr0 , ebx
			}

			SYSTEMSERVICE_BY_FUNC_ID( NativeID ) = NewFuncAddress;

			__asm {	
				pop   ebx 	
					mov   cr0 , ebx    
			}

			bPatched = TRUE;
		}
	}

	DoTraceEx( TRACE_LEVEL_WARNING, DC_SYSTEM, "function %s %s", FuncName, bPatched ? "ok" : "failed" );
#endif
	return bPatched;
}

BOOLEAN
LinkKrnlFunc (
	__out PULONG pLinkedFunc,
	__in PCHAR FuncName
	)
{
#ifdef _WIN64
	UNREFERENCED_PARAMETER( pLinkedFunc );
	UNREFERENCED_PARAMETER( FuncName );

	return FALSE;
#else
	if (!FlagOn( Globals.m_DrvFlags, _DRV_FLAG_HAS_SYSPATCH ))
		return FALSE;

	if (g_bIsW2003 || g_bIsW2K || g_bIsWXP)
	{
		if (gBaseOfNtOsKrnl)
		{
			*pLinkedFunc = (ULONG) GetExport( gBaseOfNtOsKrnl, FuncName, NULL, NULL );
			if (*pLinkedFunc)
				return TRUE;
		}
	}
#endif

	return FALSE;
}

PVOID
GetNtFunc (
			__in PCHAR cFuncName
			)
{
#ifdef _WIN64
	UNREFERENCED_PARAMETER( cFuncName );
#else
	if (!FlagOn( Globals.m_DrvFlags, _DRV_FLAG_HAS_SYSPATCH ))
		return FALSE;

	if ( g_bIsW2003 || g_bIsW2K || g_bIsWXP )
	{
		// only for win2k/xp/2003
		ULONG NativeID;

		NativeID = GetNativeID( gBaseOfNtDllDll, cFuncName );
		if ( NativeID )
			return ( (PVOID) SYSTEMSERVICE_BY_FUNC_ID( NativeID ) );
	}
#endif

	return NULL;
}

BOOLEAN IsSafeAddress(
	__in PVOID Address,
	__in ULONG Size
	)
{
	BOOLEAN bResult = FALSE;
	ULONG_PTR ulpAddress;

	ulpAddress = (ULONG_PTR) Address;
	// non-NULL ptr
	if ( ulpAddress )
	{
		// no wrapping
		if ( ulpAddress + Size >= ulpAddress )
			bResult = TRUE;
	}

	// valid ptr
	bResult &= MmIsAddressValid( Address ) && MmIsAddressValid( (PCHAR) Address + Size - 1 );

	return bResult;
}

#ifndef _WIN64

PServiceDescriptorTableEntry_t GetAddrOfShadowTable()
{
	PCHAR Ptr = (PCHAR) KeAddSystemServiceTable;
	ULONG i;
	PServiceDescriptorTableEntry_t pShadowTableRet = NULL;

	// scan KeAddSystemServiceTable function body for pointers to shadow table
	for( i = 0; i < 0x1000; i++, Ptr++ )
	{
		if ( MmIsAddressValid( Ptr ) )
		{
			PVOID ProbeShadowTablePtr;

			ProbeShadowTablePtr = (PVOID) *(PULONG) Ptr;
			// preliminary address check / bypass main SDT ptr
			if ( IsSafeAddress( ProbeShadowTablePtr, 2 * sizeof(ServiceDescriptorTableEntry_t) ) &&
				&KeServiceDescriptorTable != ProbeShadowTablePtr )
			{
				// can't do MmGetSystemAddressForMdl safe here, it returnes invalid(unmapped) VA

				// shadow table first item equals to SDT
				if ( 0 == memcmp( ProbeShadowTablePtr, &KeServiceDescriptorTable, sizeof(ServiceDescriptorTableEntry_t) ) )
				{
					// address found
					pShadowTableRet = (PServiceDescriptorTableEntry_t) ProbeShadowTablePtr;
				}
			}
		}

		if ( pShadowTableRet )
			break;
	}

	return pShadowTableRet;
}

// returnes old CR0 contents
ULONG ClearWP()
{
	ULONG OldCR0;

	__asm { // снимаем защиту памяти от записи 
			mov		ebx, cr0
			mov		[OldCR0], ebx
			and		ebx, ~0x10000
			mov		cr0, ebx
	}

	return OldCR0;
}

VOID RestoreWP(
	__in ULONG OldCR0
	)
{
	__asm { // снимаем защиту памяти от записи 
			mov		ebx, [OldCR0]
			and		ebx, 0x10000
			mov		edx, cr0
			or		edx, ebx
			mov		cr0, edx
	}
}

extern volatile PServiceDescriptorTableEntry_t g_pShadowSDT;

PVOID GetNtFuncByID(
					__in ULONG ServiceID,
					__out_opt PVOID **p_pfServiceRet
					)
{
	if ( !FlagOn( Globals.m_DrvFlags, _DRV_FLAG_HAS_SYSPATCH ) )
		return NULL;
	// only for 2k/xp/2003srv
	if ( !( g_bIsW2003 || g_bIsW2K || g_bIsWXP ) )
		return NULL;

	// syscall to ntoskrnl.exe
	if ( !( ServiceID & 0x3000 ) )
	{
		if ( p_pfServiceRet )
			*p_pfServiceRet = (PVOID *) &( SYSTEMSERVICE_BY_FUNC_ID( ServiceID ) );

		return (PVOID) SYSTEMSERVICE_BY_FUNC_ID(ServiceID);
	}
	// syscall to win32k.sys
	else
	{
		if ( g_pShadowSDT )
		{
			PVOID *p_fService;
			ULONG WIN32K_ServiceID;
			PServiceDescriptorTableEntry_t pWIN32KEntry;

			pWIN32KEntry = g_pShadowSDT + 1;
			WIN32K_ServiceID = ServiceID & 0x0fff;

			p_fService = (PVOID *) ( (PULONG) pWIN32KEntry->ServiceTableBase + WIN32K_ServiceID );
			if ( pWIN32KEntry->ServiceTableBase && MmIsAddressValid( p_fService ) )
			{
				if ( p_pfServiceRet )
					*p_pfServiceRet = p_fService;

				return *p_fService;
			}
		}
	}

	return NULL;
}

BOOLEAN HookNtFuncByID(
					   __in ULONG ServiceID,
					   __in PVOID fHookFunc,
					   __out PVOID *p_fServiceRet,
					   __out_opt PVOID **p_pfServiceRet
					   )
{
	BOOLEAN bResult = FALSE;

	if ( !FlagOn( Globals.m_DrvFlags, _DRV_FLAG_HAS_SYSPATCH ) )
		return FALSE;
	// only for 2k/xp/2003srv
	if ( !( g_bIsW2003 || g_bIsW2K || g_bIsWXP ) )
		return FALSE;

	// syscall to ntoskrnl.exe
	if ( !( ServiceID & 0x3000 ) )
	{
		ULONG OldCR0;

		*p_fServiceRet = (PVOID) SYSTEMSERVICE_BY_FUNC_ID( ServiceID );
		if ( p_pfServiceRet )
			*p_pfServiceRet = (PVOID *) &SYSTEMSERVICE_BY_FUNC_ID( ServiceID );

		OldCR0 = ClearWP();
		(PVOID) SYSTEMSERVICE_BY_FUNC_ID( ServiceID ) = fHookFunc;
		RestoreWP( OldCR0 );


		bResult = TRUE;
	}
	// syscall to win32k.sys
	else
	{
		if ( g_pShadowSDT )
		{
			PVOID *p_fService;
			ULONG WIN32K_ServiceID;
			PServiceDescriptorTableEntry_t pWIN32KEntry;

			pWIN32KEntry = g_pShadowSDT + 1;
			WIN32K_ServiceID = ServiceID & 0x0fff;

			p_fService = (PVOID *) ( (PULONG) pWIN32KEntry->ServiceTableBase + WIN32K_ServiceID );
			if ( pWIN32KEntry->ServiceTableBase && MmIsAddressValid( p_fService ) )
			{
				ULONG OldCR0;

				*p_fServiceRet = *p_fService;
				if ( p_pfServiceRet )
					*p_pfServiceRet = p_fService;

				OldCR0 = ClearWP();
				*p_fService = fHookFunc;
				RestoreWP( OldCR0 );

				bResult = TRUE;
			}
		}
	}

	return bResult;
}

#endif

//-------------------------------------------------------------------------------------------------
// returnes first occurence!
// returned PROCESS object should be dereferenced after use!
PEPROCESS LookupProcessByName(
	__in PWCHAR ProcessName
	)
{
	ULONG BufferSize = 1000 * sizeof(SYSTEM_THREADS);
	PVOID pBuffer = NULL;
	NTSTATUS ntStatus;
	UNICODE_STRING usProcessName;
	ULONG Tmp;
	PEPROCESS pProcess = NULL;

	RtlInitUnicodeString(
		&usProcessName,
		ProcessName
		);

	do
	{
		if ( pBuffer )
			ExFreePool(pBuffer);

		pBuffer = ExAllocatePoolWithTag(
			PagedPool,
			BufferSize,
			'PQrg'
			);
		if ( !pBuffer )
			return NULL;

		ntStatus = ZwQuerySystemInformation(
			5/*processes and threads*/,
			pBuffer,
			BufferSize,
			&Tmp
			);
		if ( ntStatus != STATUS_SUCCESS && ntStatus != STATUS_INFO_LENGTH_MISMATCH )
			break;

		BufferSize *= 2;
	} while( ntStatus != STATUS_SUCCESS && BufferSize < 0x100000 );

	if ( ntStatus == STATUS_SUCCESS )
	{
		PSYSTEM_PROCESSES pSysProc;

		pSysProc = (PSYSTEM_PROCESSES) pBuffer;
		do
		{
			if ( 0 == RtlCompareUnicodeString( &pSysProc->ProcessName, &usProcessName, TRUE ) )
			{
				PEPROCESS pCurrProcess;

				ntStatus = PsLookupProcessByProcessId( (HANDLE) pSysProc->ProcessId, &pCurrProcess );
				if ( NT_SUCCESS(ntStatus) )
					pProcess = pCurrProcess;
			}

			(PCHAR)pSysProc += pSysProc->NextEntryDelta;
		} while ( pSysProc->NextEntryDelta && !pProcess );
	}

	ExFreePool( pBuffer );

	return pProcess;
}

PVOID GetPEBAddress()
{
	NTSTATUS ntStatus;
	PROCESS_BASIC_INFORMATION ProcInfo;
	ULONG RetLen;

	ntStatus = ZwQueryInformationProcess(
		NtCurrentProcess(),
		ProcessBasicInformation,
		&ProcInfo,
		sizeof(ProcInfo),
		&RetLen
		);
	if ( NT_SUCCESS( ntStatus ) )
		return ProcInfo.PebBaseAddress;

	return NULL;
}

// lookup module with known HMODULE in current process
BOOLEAN LookupModuleNameByBase(
	__in PVOID ModuleBase,
	__out_opt PWCHAR FullName,
	__in ULONG cbFullName,
	__out_opt PWCHAR BaseName,
	__in ULONG cbBaseName
	)
{
	PintPEB pPEB;

	pPEB = (PintPEB) GetPEBAddress();
	if ( pPEB )
	{
		__try
		{
			PPEB_LDR_DATA pLdrData = pPEB->Ldr;

			if ( pLdrData )
			{
				PLDR_MODULE_2 pInMemoryEntry = (PLDR_MODULE_2) pLdrData->InMemoryOrderModuleList.Flink;

				while ( pInMemoryEntry && (PLIST_ENTRY) pInMemoryEntry != &pLdrData->InMemoryOrderModuleList )
				{
					if ( ModuleBase == pInMemoryEntry->BaseAddress )
					{
						BOOLEAN bResult = FALSE;

						if ( FullName )
						{
							if ( cbFullName >= pInMemoryEntry->FullDllName.Length + sizeof(WCHAR) )
							{
								memcpy(
									FullName,
									pInMemoryEntry->FullDllName.Buffer,
									pInMemoryEntry->FullDllName.Length
									);
								FullName[pInMemoryEntry->FullDllName.Length / sizeof(WCHAR)] = 0;

								bResult = TRUE;
							}
						}

						if ( BaseName )
						{
							if ( cbBaseName >= pInMemoryEntry->BaseDllName.Length + sizeof(WCHAR) )
							{
								memcpy(
									BaseName,
									pInMemoryEntry->BaseDllName.Buffer,
									pInMemoryEntry->BaseDllName.Length
									);
								FullName[pInMemoryEntry->BaseDllName.Length / sizeof(WCHAR)] = 0;

								bResult = TRUE;
							}
						}

						return bResult;
					}

					pInMemoryEntry = (PLDR_MODULE_2) pInMemoryEntry->InMemoryOrderModuleList.Flink;
				}
			}
		}
		__except(EXCEPTION_EXECUTE_HANDLER)
		{
		}
	}

	return FALSE;
}

// получаем и референсим базовый девайс..
NTSTATUS ReferenceBaseDeviceByName(
	__in PUNICODE_STRING DevicePath,
	__out PDEVICE_OBJECT *p_pDeviceObject
	)
{
	OBJECT_ATTRIBUTES OA;
	NTSTATUS status;
	HANDLE hFile;
	IO_STATUS_BLOCK ioStatus;
	PFILE_OBJECT pFileObject;
	PDEVICE_OBJECT pDeviceObject;

	InitializeObjectAttributes(
		&OA,
		DevicePath,
		OBJ_CASE_INSENSITIVE,
		NULL,
		NULL
		);

	status = ZwOpenFile(
		&hFile,
		0,
		&OA,
		&ioStatus,
		FILE_SHARE_READ | FILE_SHARE_WRITE,
		FILE_NON_DIRECTORY_FILE
		);
	if ( NT_SUCCESS( status ) )
	{
		status = ObReferenceObjectByHandle(
			hFile,
			0,
			*IoFileObjectType,
			KernelMode,
			&pFileObject,
			NULL
			);
		if ( NT_SUCCESS( status ) )
		{
			pDeviceObject = IoGetBaseFileSystemDeviceObject( pFileObject );
			if ( pDeviceObject )
			{
				ObReferenceObject( pDeviceObject );
				*p_pDeviceObject = pDeviceObject;
			}

			ObDereferenceObject(pFileObject);
		}

		ZwClose(hFile);
	}

	return status;
}

// free returned string with ExFreePool
PWCHAR Sys_QueryObjectName(
	__in PVOID pObject
	)
{
	POBJECT_NAME_INFORMATION pDeviceInfo;
	NTSTATUS status;
	PWCHAR wcRetStr = NULL;

	if ( !pObject )
		return NULL;

	pDeviceInfo = (POBJECT_NAME_INFORMATION) ExAllocatePoolWithTag(
		PagedPool,
		0x1000,
		'OQrg'
		);
	if ( pDeviceInfo )
	{
		ULONG RetLen;

		status = ObQueryNameString(
			pObject,
			pDeviceInfo,
			0x1000,
			&RetLen
			);
		if ( NT_SUCCESS( status ) && RetLen && pDeviceInfo->Name.Length && pDeviceInfo->Name.Buffer )
		{
			wcRetStr = (PWCHAR) ExAllocatePoolWithTag(
				PagedPool,
				pDeviceInfo->Name.Length + sizeof(WCHAR),
				'SQgr'
				);
			if ( wcRetStr )
			{
				memcpy(
					wcRetStr,
					pDeviceInfo->Name.Buffer,
					pDeviceInfo->Name.Length
					);
				wcRetStr[pDeviceInfo->Name.Length / sizeof(WCHAR)] = 0;
			}
		}

		ExFreePool( pDeviceInfo );
	}

	return wcRetStr;
}
//-------------------------------------------------------------------------------------------------
