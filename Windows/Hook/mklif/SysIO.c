#include "SysIO.h"
#include "inc/commdata.h"
#include "osspec_asm.h"
#include "osspec.h"
#include "gencache.h"

#include "sysio.tmh"

#pragma warning(disable:4995)

LONG g_SysEnters = 0;

ULONG gHashFuncVersion = 1;	// 1- md5, 2 - smart fidbox2

#define _inicode_prefix_system		L"\\??\\"
#define _inicode_prefix_system_len	(sizeof(_inicode_prefix_system) - sizeof(WCHAR))

#define _inicode_prefix				L"\\\\?\\"
#define _inicode_prefix_len			(sizeof(_inicode_prefix) - sizeof(WCHAR))

//+ function prototypes -----------------------------------------------------------------
VOID TryInitShadowTablePtr();
BOOLEAN KbdAttachCheck_ReInit();
//- function prototypes -----------------------------------------------------------------

NTSYSAPI POBJECT_TYPE IoDriverObjectType;

#ifndef _WIN64

PGEN_CACHE g_pKeyReaderCache = NULL;
PGEN_CACHE g_pAttachedInputThreadCache = NULL;
PGEN_CACHE g_pVMemCache = NULL;
PGEN_CACHE g_pRPCMapperPortsCache = NULL;
PGEN_CACHE g_pDNSResolverPortCache = NULL;
PGEN_CACHE g_pProtectedStorageHANDLECache = NULL;

#define KBDREAD_PROLOGUE_SIZE	16

PVOID g_fKbdReadDispatch = NULL;
PVOID *g_pfKbdReadDispatch = NULL;
UCHAR g_rKbdReadDispatch_Prologue[KBDREAD_PROLOGUE_SIZE];
BOOLEAN g_bKbdSpliceCheck_Inited = FALSE;

#endif

PDEVICE_OBJECT g_pKbdClassDevice = NULL;
PDEVICE_OBJECT g_pKbdPortDevice = NULL;
BOOLEAN g_bKbdAttachCheck_Inited = FALSE;

//+ -----------------------------------------------------------------------------------------------
// this virtual memory allocation cache is made for performance reasons
// NtAllocateVirtualMemory works very slow ((
// we will have a page of user-mode memory allocated(on demand) for each process

// process ID hash size
#define VMEM_HASH_SIZE			0x1000

typedef struct _VMEM_CACHE_ITEM
{
	PVOID			m_UserPagePtr;
	FAST_MUTEX		m_UserPageLock;		// memory use lock
} VMEM_CACHE_ITEM, *PVMEM_CACHE_ITEM;

VOID VMemCacheItemAlloc(
						__in ULONGLONG Key,
						__in PVOID pData
						)
{
	NTSTATUS status;
	PVMEM_CACHE_ITEM pItem = (PVMEM_CACHE_ITEM) pData;
	PVOID BaseAddress = NULL;
	SIZE_T RegionSize = 0x1000;

	UNREFERENCED_PARAMETER( Key );

	RtlZeroMemory(
		pItem,
		sizeof(*pItem)
		);

	status = ZwAllocateVirtualMemory( 
		NtCurrentProcess(),
		&BaseAddress,
		0,
		&RegionSize,
		MEM_COMMIT,
		PAGE_EXECUTE_READWRITE
		);
	if (!NT_SUCCESS( status ))
	{
		_dbg_break_;
	}
	else
	{
		pItem->m_UserPagePtr = BaseAddress;				
		ExInitializeFastMutex( &pItem->m_UserPageLock );
	}
}

VOID VMemCacheItemFree(
					   __in ULONGLONG Key,
					   __in PVOID pData
					   )
{
	NTSTATUS status;
	PVMEM_CACHE_ITEM pCacheItem = (PVMEM_CACHE_ITEM) pData;
	PVOID BaseAddress = pCacheItem->m_UserPagePtr;
	SIZE_T RegionSize = 0;
	HANDLE hProcess;
	CLIENT_ID Cid;
	OBJECT_ATTRIBUTES ProcOA;

	// item was not properly inited
	if (!pCacheItem->m_UserPagePtr)
		return;

	Cid.UniqueProcess = (HANDLE) Key;
	Cid.UniqueThread = 0;
	InitializeObjectAttributes(
		&ProcOA,
		NULL,
		OBJ_KERNEL_HANDLE,
		NULL,
		NULL
		);
	status = ZwOpenProcess(
		&hProcess,
		PROCESS_VM_OPERATION,
		&ProcOA,
		&Cid
		);
	if ( NT_SUCCESS( status ) )
	{
		ExAcquireFastMutex( &pCacheItem->m_UserPageLock );

		status = ZwFreeVirtualMemory(
			hProcess,
			&BaseAddress,
			&RegionSize,
			MEM_RELEASE
			);
		if ( !NT_SUCCESS( status ) )
			_dbg_break_;

		ExReleaseFastMutex( &pCacheItem->m_UserPageLock );

		ZwClose( hProcess );
	}
}

BOOLEAN VMemCacheInit(
					  __out PGEN_CACHE *p_pVMemCache
					  )
{
	return GenCacheInit(
		VMEM_HASH_SIZE,
		sizeof(VMEM_CACHE_ITEM),
		VMemCacheItemAlloc,
		VMemCacheItemFree,
		p_pVMemCache
		);
}

VOID VMemCacheDone(
				   __in PGEN_CACHE *p_pVMemCache
				   )
{
	GenCacheDone(
		p_pVMemCache
		);
}

// get cache item by current process
PVMEM_CACHE_ITEM VMemCacheGet(
							  __in PGEN_CACHE pVMemCache,
							  __in HANDLE ProcessId
							  )
{
	BOOLEAN bNewReturned;

	return GenCacheGet(
		pVMemCache,
		(ULONGLONG) ProcessId,
		TRUE,
		&bNewReturned
		);
}

// free cache item for current process
VOID VMemCacheFree(
				   __in PGEN_CACHE pVMemCache,
				   __in HANDLE ProcessId
				   )
{
	GenCacheFree(
		pVMemCache,
		(ULONGLONG) ProcessId
		);
}
//- -----------------------------------------------------------------------------------------------

HANDLE (__stdcall *_pfPsGetThreadId)(PKTHREAD pkTh) = 0; //ETHREAD*
HANDLE (__stdcall *_pfPsGetThreadProcessId)(PKTHREAD pkTh) = 0;	//ETHREAD*

NTSTATUS
Sys_GetProcessIdByHandle (
						  HANDLE ProcessHandle,
						  PHANDLE pProcessId
						  )
{
	NTSTATUS status;
	PROCESS_BASIC_INFORMATION ProcInfo;

	if (!ProcessHandle || UlongToHandle( -1 ) == ProcessHandle)
		return STATUS_UNSUCCESSFUL;

	status = ZwQueryInformationProcess (
		ProcessHandle,
		ProcessBasicInformation,
		&ProcInfo,
		sizeof(ProcInfo),
		NULL
		);

	if (NT_SUCCESS( status ))
	{
#ifdef _WIN64
		*pProcessId = (HANDLE) ProcInfo.UniqueProcessId;
#else
		*pProcessId = ULongToHandle( ProcInfo.UniqueProcessId );
#endif
	}

	return status;
}

PVOID
Sys_CopyUserBuffer (
					PVOID pUserBuffer,
					ULONG UserBufferSize
					)
{
	PVOID pRetBuf;
	BOOLEAN bSucceeded = FALSE;

	if ( !pUserBuffer )
		return NULL;

	// user-mode buffers only
	if ( MmUserProbeAddress <= (ULONG_PTR) pUserBuffer )
		return NULL;

	pRetBuf = ExAllocatePoolWithTag(
		PagedPool,
		UserBufferSize,
		tag_user_buf_probe
		);
	if ( !pRetBuf )
		return NULL;

	// check for no binary wrapping
	if ( IsSafeAddress( pUserBuffer, UserBufferSize ) )
	{
		PMDL pMdl;

		pMdl = IoAllocateMdl(
			pUserBuffer,
			UserBufferSize,
			FALSE,
			FALSE,
			NULL
			);
		if ( pMdl )
		{
			__try
			{
				PVOID pSysMappedBuffer;

				// lock pages
				MmProbeAndLockPages(
					pMdl,
					UserMode,
					IoReadAccess
					);
				// map to system VA not to let it go away
				pSysMappedBuffer = MmGetSystemAddressForMdlSafe(
					pMdl,
					NormalPagePriority
					);
				if ( pSysMappedBuffer )
				{
					memcpy(
						pRetBuf,
						pSysMappedBuffer,
						UserBufferSize
						);
					bSucceeded = TRUE;
				}

				MmUnlockPages( pMdl );
			}
			__except(CheckException())
			{
			}

			IoFreeMdl( pMdl );
		}
	}

	if ( !bSucceeded )
	{
		_dbg_break_;

		ExFreePool( pRetBuf );
		pRetBuf = NULL;
	}

	return pRetBuf;
}

PUNICODE_STRING GetUserUniStr(
							  PUNICODE_STRING pUserUniStr
							  )
{
	PUNICODE_STRING pUniStr = NULL;
	PUNICODE_STRING pUniStrTmp;

	pUniStrTmp = Sys_CopyUserBuffer(
		pUserUniStr,
		sizeof(UNICODE_STRING)
		);
	if ( pUniStrTmp )
	{
		USHORT Len = pUniStrTmp->Length;

		if ( Len && pUniStrTmp->Buffer )
		{
			PVOID pBuffer;

			pBuffer = Sys_CopyUserBuffer( pUniStrTmp->Buffer, Len );
			if ( pBuffer )
			{
				pUniStr = ExAllocatePoolWithTag(
					PagedPool,
					sizeof(UNICODE_STRING) + Len,
					'zbos'
					);
				if ( pUniStr )
				{
					pUniStr->Length = Len;
					pUniStr->MaximumLength = Len;
					pUniStr->Buffer = (PWCHAR)( (PCHAR) pUniStr + sizeof(UNICODE_STRING) );

					memcpy( pUniStr->Buffer, pBuffer, Len );
				}

				ExFreePool( pBuffer );
			}
		}

		ExFreePool( pUniStrTmp );
	}

	return pUniStr;
}

PWCHAR GetUserWideStr(
					  __in PWCHAR wcUserStr,
					  __in ULONG MaxBufferSize
					  )
{
	PWCHAR wcRetStr;
	PWCHAR wcSource;
	ULONG MaxChars = MaxBufferSize / sizeof(WCHAR);
	ULONG i;

	if ( MmUserProbeAddress <= (ULONG_PTR) wcUserStr )
		return NULL;

	if ( sizeof(WCHAR) > MaxBufferSize )
		return NULL;

	wcRetStr = (PWCHAR) ExAllocatePoolWithTag(
		PagedPool,
		MaxBufferSize,
		'SGrg'
		);
	if ( !wcRetStr )
		return NULL;

	RtlZeroMemory(
		wcRetStr,
		MaxBufferSize
		);

	wcSource = wcUserStr;
	i = 0;
	__try
	{
		while ( ( MmUserProbeAddress > (ULONG_PTR) wcSource + sizeof(WCHAR) ) && i < MaxChars )
		{
			wcRetStr[i] = *wcSource;

			if ( 0 == wcRetStr[i] )
				break;

			wcSource++;
			i++;
		}
	}
	__except( EXCEPTION_EXECUTE_HANDLER )
	{
	}
	wcRetStr[MaxChars-1] = 0;

	return wcRetStr;
}

PCHAR GetUserStr(
				 __in PCHAR cUserStr,
				 __in ULONG MaxBufferSize
				 )
{
	PCHAR cRetStr;
	PCHAR cSource;
	ULONG MaxChars = MaxBufferSize / sizeof(CHAR);
	ULONG i;

	if ( MmUserProbeAddress <= (ULONG_PTR) cUserStr )
		return NULL;

	if ( sizeof(CHAR) > MaxBufferSize )
		return NULL;

	cRetStr = (PCHAR) ExAllocatePoolWithTag(
		PagedPool,
		MaxBufferSize,
		'SGrg'
		);
	if ( !cRetStr )
		return NULL;

	RtlZeroMemory(
		cRetStr,
		MaxBufferSize
		);

	cSource = cUserStr;
	i = 0;
	__try
	{
		while ( ( MmUserProbeAddress > (ULONG_PTR) cSource + sizeof(CHAR) ) && i < MaxChars )
		{
			cRetStr[i] = *cSource;

			if ( 0 == cRetStr[i] )
				break;

			cSource++;
			i++;
		}
	}
	__except( EXCEPTION_EXECUTE_HANDLER )
	{
	}
	cRetStr[MaxChars-1] = 0;

	return cRetStr;
}

PWCHAR GetUserWideStrFromUni(
	__in PUNICODE_STRING p_usUserStr
	)
{
	PUNICODE_STRING p_usSafeStr;
	PWCHAR wcRetStr = NULL;

	p_usSafeStr = (PUNICODE_STRING) Sys_CopyUserBuffer(
		p_usUserStr,
		sizeof(*p_usUserStr)
		);
	if ( p_usSafeStr )
	{
		if ( p_usSafeStr->Buffer && p_usSafeStr->Length )
		{
			PWCHAR wcSafeStr;

			wcSafeStr = (PWCHAR) Sys_CopyUserBuffer(
				p_usSafeStr->Buffer,
				p_usSafeStr->Length
				);
			if ( wcSafeStr )
			{
				wcRetStr = (PWCHAR) ExAllocatePoolWithTag(
					PagedPool,
					p_usSafeStr->Length + sizeof(WCHAR),
					'WUrg'
					);
				if ( wcRetStr )
				{
					memcpy(
						wcRetStr,
						wcSafeStr,
						p_usSafeStr->Length
						);
					wcRetStr[p_usSafeStr->Length / sizeof(WCHAR)] = 0;
				}

				ExFreePool( wcSafeStr );
			}
		}

		ExFreePool( p_usSafeStr );
	}

	return wcRetStr;
}

VOID
ReparsePath (
			 PWCHAR* ppwchReparsedPath,
			 PUSHORT puReparsedLen
			 )
{
	PWCHAR pwchPath = *ppwchReparsedPath;
	USHORT len = *puReparsedLen;

	if (len > _inicode_prefix_system_len)
	{
		if (!memcmp( pwchPath, _inicode_prefix_system, _inicode_prefix_system_len ))
		{
			pwchPath += _inicode_prefix_system_len / sizeof(WCHAR);
			len -= _inicode_prefix_system_len;
		}
	}

	if (len > _inicode_prefix_len)
	{
		if (!memcmp( pwchPath, _inicode_prefix, _inicode_prefix_len ))
		{
			pwchPath += _inicode_prefix_len / sizeof(WCHAR);
			len -= _inicode_prefix_len;
		}
	}

	*ppwchReparsedPath = pwchPath;
	*puReparsedLen = len;
}

PWCHAR
Sys_BuildName (
	PWCHAR pwchPath,
	USHORT PathLen
	)
{
	NTSTATUS status = STATUS_SUCCESS;
	PWCHAR pwchBuf = NULL;

	ULONG UncPrefSize = 0;

	if (!pwchPath || PathLen < sizeof(WCHAR) * 4)
		return NULL;

	if (L'\\' == pwchPath[0] && 0 != pwchPath[1])
	{
		if (L'?' != pwchPath[1] && L'\\' != pwchPath[1])
		{
			pwchPath += 1;
			PathLen -= sizeof(WCHAR);
		}
	}

#define _SysRootPattern L"SystemRoot\\"
#define _SysRootPattern_LenBNZ (sizeof(_SysRootPattern) - sizeof(WCHAR))

	if (PathLen > _SysRootPattern_LenBNZ)
	{
		UNICODE_STRING us1, us2;
		RtlInitEmptyUnicodeString( &us1, _SysRootPattern, _SysRootPattern_LenBNZ );
		us1.Length = _SysRootPattern_LenBNZ;
		RtlInitEmptyUnicodeString( &us2, pwchPath, _SysRootPattern_LenBNZ );
		us2.Length = _SysRootPattern_LenBNZ;

		if (!RtlCompareUnicodeString( &us1, &us2, FALSE ))
		{
			UNICODE_STRING usRootPath;
			OBJECT_ATTRIBUTES oa;
			IO_STATUS_BLOCK ioStatus;
			HANDLE sysRootHandle = NULL;

			RtlInitUnicodeString( &usRootPath, L"\\SystemRoot" );
			InitializeObjectAttributes( &oa, &usRootPath, OBJ_CASE_INSENSITIVE | OBJ_KERNEL_HANDLE, NULL, NULL );
			status = ZwCreateFile (
				&sysRootHandle,
				FILE_TRAVERSE,
				&oa,
				&ioStatus,
				NULL,
				0,
				FILE_SHARE_READ | FILE_SHARE_WRITE | FILE_SHARE_DELETE,
				FILE_OPEN,
				0,
				NULL,
				0
				);

			if (NT_SUCCESS( status ))
			{
				PFILE_OBJECT pFileObject = NULL;

				status = ObReferenceObjectByHandle( sysRootHandle, 0, *IoFileObjectType, KernelMode, &pFileObject, NULL );
				if (NT_SUCCESS( status ))
				{
					PMKAV_VOLUME_CONTEXT pVolumeContext = GetVolumeContextFromFO( pFileObject );
					if (pVolumeContext)
					{
						PFLT_FILE_NAME_INFORMATION pNameInfo = NULL;

						status = FltGetFileNameInformationUnsafe (
							pFileObject,
							pVolumeContext->m_pFltInstance,
							FLT_FILE_NAME_QUERY_DEFAULT | FLT_FILE_NAME_NORMALIZED,
							&pNameInfo
							);

						if(NT_SUCCESS( status ))
						{
							ULONG TotalLen = 0;

							FltParseFileNameInformation( pNameInfo );
#define _sys_c	L"\\??\\C:\\"
							TotalLen = sizeof(_sys_c) + pNameInfo->FinalComponent.Length + PathLen - sizeof(_SysRootPattern) + sizeof(WCHAR);

							pwchBuf = ExAllocatePoolWithTag( PagedPool, TotalLen, tag_sysio1 );
							if (pwchBuf)
							{
								status = RtlStringCbCopyW( pwchBuf, TotalLen, _sys_c );
								if (NT_SUCCESS( status ))
									status = RtlStringCbCatNW( pwchBuf, TotalLen, pNameInfo->FinalComponent.Buffer, pNameInfo->FinalComponent.Length );
								if (NT_SUCCESS( status ))
								{
									ULONG LeftBytes = PathLen - sizeof(_SysRootPattern) + sizeof(WCHAR);
									PWCHAR pwchTmp = pwchPath + (sizeof(L"SystemRoot") - sizeof(WCHAR)) / sizeof(WCHAR);
									status = RtlStringCbCatNW( pwchBuf, TotalLen, pwchTmp, LeftBytes );
								}

								if (!NT_SUCCESS( status ))
								{
									ExFreePool( pwchBuf );
									pwchBuf = NULL;
								}
							}

							ReleaseFileNameInfo( pNameInfo );
						}

						ReleaseFltContext( &pVolumeContext );
					}

					ObDereferenceObject( pFileObject );
				}

				ZwClose( sysRootHandle );
			}
		}
	}

	if (pwchBuf) // ready?
		return pwchBuf;

	if (L'\\' == pwchPath[0] && L'\\' == pwchPath[1]
	&& L'\\' != pwchPath[2] && L'?' != pwchPath[2])
	{
		ULONG TotalLen = 0;
		UncPrefSize = sizeof(L"\\??\\unc") - sizeof(WCHAR);

		TotalLen = PathLen + UncPrefSize + sizeof(WCHAR);

		pwchBuf = ExAllocatePoolWithTag( PagedPool, TotalLen, tag_sysio1 );
		if (pwchBuf)
		{
			status = RtlStringCbCopyW( pwchBuf, TotalLen, L"\\??\\unc" );
			if (NT_SUCCESS( status ))
				status = RtlStringCbCatNW( pwchBuf, TotalLen, pwchPath + 1, PathLen - 2);
		}
	}
	else
	{
		UncPrefSize = sizeof(L"\\??\\") - sizeof(WCHAR);
#ifdef _WIN64
		if (FltIs32bitProcess( NULL ))
		{
			UNICODE_STRING us;
			UNICODE_STRING usp;
#define _SysPattern L"*:\\WINDOWS\\SYSTEM32\\*"
			PWCHAR pwchSysPattern = _SysPattern;

			RtlInitEmptyUnicodeString( &us, pwchPath, PathLen );
			us.Length = PathLen;

			RtlInitEmptyUnicodeString( &usp, pwchSysPattern, sizeof(_SysPattern) - sizeof(WCHAR) );
			usp.Length = sizeof(_SysPattern) - sizeof(WCHAR);

			if (FsRtlIsNameInExpression( &usp, &us, TRUE, NULL ))
			{
				ULONG Wow64Prefix = sizeof(L"SysWOW64\\") - sizeof(WCHAR);
				ULONG TotalLen = PathLen + UncPrefSize + Wow64Prefix + sizeof(WCHAR);

				pwchBuf = ExAllocatePoolWithTag( PagedPool, TotalLen, tag_sysio1 );
				if (pwchBuf)
				{
					status = RtlStringCbCopyW( pwchBuf, TotalLen, L"\\??\\" );

					if (NT_SUCCESS( status ))
						status = RtlStringCbCatNW( pwchBuf, TotalLen, pwchPath,
						(sizeof(L"c:\\windows") - sizeof(WCHAR)));

					if (NT_SUCCESS( status ))
						status = RtlStringCbCatNW( pwchBuf, TotalLen, L"\\SysWOW64\\", sizeof(L"\\SysWOW64\\") );

					if (NT_SUCCESS( status ))
					{
						PWCHAR pwchLeftString = pwchPath + sizeof(L"c:\\windows\\system32") / sizeof(WCHAR);
						ULONG LeftBytes = PathLen - sizeof(L"c:\\windows\\system32");
						status = RtlStringCbCatNW( pwchBuf, TotalLen, pwchLeftString, LeftBytes );
					}
				}
			}
		}
#endif //_WIN64
		if (!pwchBuf)
		{
			ULONG TotalLen = PathLen + UncPrefSize + sizeof(WCHAR);
			pwchBuf = ExAllocatePoolWithTag( PagedPool, TotalLen, tag_sysio1 );
			if (pwchBuf)
			{
				status = RtlStringCbCopyW( pwchBuf, TotalLen, L"\\??\\" );
				if (NT_SUCCESS( status ))
					status = RtlStringCbCatNW( pwchBuf, TotalLen, pwchPath, PathLen );
			}
		}
	}

	if (!pwchBuf)
		return NULL;

	if (!NT_SUCCESS( status ))
	{
		_dbg_break_;
		ExFreePool( pwchBuf );

		return NULL;
	}

	return pwchBuf;
}

NTSTATUS
Sys_BuildNativePath (
	PWCHAR pwchPath,
	USHORT PathLen,
	PUNICODE_STRING pusNativePath
	)
{
	NTSTATUS status = STATUS_UNSUCCESSFUL;

	PFILE_OBJECT pFileObject = NULL;
	PFLT_FILE_NAME_INFORMATION pNameInfo = NULL;

	PMKAV_VOLUME_CONTEXT pVolumeContext = NULL;
	PMKAV_STREAM_CONTEXT pContext = NULL;

	HANDLE hFile = NULL;
	PWCHAR pwchName = NULL;

	if (!pwchPath || !PathLen)
		return STATUS_INVALID_PARAMETER;
		
	 pwchName = Sys_BuildName( pwchPath, PathLen );
	if (pwchName)
	{
		OBJECT_ATTRIBUTES oa;
		UNICODE_STRING us;
		IO_STATUS_BLOCK ioStatus;

		RtlInitUnicodeString( &us, pwchName );
		InitializeObjectAttributes( &oa, &us, OBJ_CASE_INSENSITIVE | OBJ_KERNEL_HANDLE, NULL, NULL );

		DoTraceEx( TRACE_LEVEL_VERBOSE, DC_SYSTEM, "BuildNativePath: income '%wZ' len %d", &us, us.Length );

		status = FltCreateFile (
			Globals.m_Filter,
			NULL,
			&hFile,
			SYNCHRONIZE | FILE_READ_DATA,
			&oa,
			&ioStatus,
			NULL,
			FILE_ATTRIBUTE_NORMAL,
			FILE_SHARE_READ | FILE_SHARE_WRITE,
			FILE_OPEN,
			FILE_RANDOM_ACCESS,
			NULL,
			0,
			IO_IGNORE_SHARE_ACCESS_CHECK
		);

		/*status = ZwCreateFile (
			&hFile,
			SYNCHRONIZE | FILE_ANY_ACCESS,
			&oa,
			&ioStatus,
			NULL,
			0,
			FILE_SHARE_READ | FILE_SHARE_WRITE,
			FILE_OPEN,
			FILE_SYNCHRONOUS_IO_NONALERT,
			NULL,
			0
			);*/

		ExFreePool( pwchName );
		pwchName = NULL;

		if(!NT_SUCCESS( status ))
		{
			RtlInitEmptyUnicodeString( &us, pwchPath, PathLen );
			us.Length = PathLen;

			status = FltCreateFile (
				Globals.m_Filter,
				NULL,
				&hFile,
				SYNCHRONIZE | FILE_READ_DATA,
				&oa,
				&ioStatus,
				NULL,
				FILE_ATTRIBUTE_NORMAL,
				FILE_SHARE_READ | FILE_SHARE_WRITE,
				FILE_OPEN,
				FILE_RANDOM_ACCESS,
				NULL,
				0,
				IO_IGNORE_SHARE_ACCESS_CHECK
				);

			/*status = ZwCreateFile (
				&hFile,
				SYNCHRONIZE | FILE_ANY_ACCESS,
				&oa,
				&ioStatus,
				NULL,
				0,
				FILE_SHARE_READ | FILE_SHARE_WRITE,
				FILE_OPEN,
				FILE_SYNCHRONOUS_IO_NONALERT,
				NULL,
				0
				);*/

			if(!NT_SUCCESS( status ))
			{
				_dbg_break_;
			}
		}
	}

	DoTraceEx( TRACE_LEVEL_VERBOSE, DC_SYSTEM, "BuildNativePath: open result %!STATUS!", status );

	if(NT_SUCCESS( status ))
	{
		status = ObReferenceObjectByHandle (
			hFile,
			0,
			*IoFileObjectType,
			KernelMode,
			&pFileObject,
			NULL
			);

		DoTraceEx( TRACE_LEVEL_VERBOSE, DC_SYSTEM, "BuildNativePath: reference %!STATUS!", status );

		if(!NT_SUCCESS( status ))
			pFileObject = NULL;

		FltClose( hFile );
		//ZwClose( hFile );
	}

	if (!pFileObject)
		return STATUS_UNSUCCESSFUL;

	status = STATUS_UNSUCCESSFUL;
	pVolumeContext = GetVolumeContextFromFO( pFileObject );
	if (pVolumeContext)
	{
		pContext = GetStreamContext( pVolumeContext->m_pFltInstance, pFileObject );
		if (pContext)
			pNameInfo = LockFileNameInfo( pContext );
		else
		{
			DoTraceEx( TRACE_LEVEL_ERROR, DC_SYSTEM, "BuildNativePath: no volume context" );

			status = FltGetFileNameInformationUnsafe (
				pFileObject,
				pVolumeContext->m_pFltInstance,
				FLT_FILE_NAME_QUERY_DEFAULT | FLT_FILE_NAME_NORMALIZED,
				&pNameInfo
				);

			DoTraceEx( TRACE_LEVEL_ERROR, DC_SYSTEM, "BuildNativePath: get noncached name result %!STATUS!", status );

			if(NT_SUCCESS( status ))
				FltParseFileNameInformation( pNameInfo );
			else
				pNameInfo = NULL;
		}

		ReleaseFltContext( &pVolumeContext );
	}

	if (pNameInfo)
	{
		if (pNameInfo->Name.Length)
		{
			PWCHAR pwchReparsedName = ExAllocatePoolWithTag( PagedPool, pNameInfo->Name.Length, tag_name_norm );
			if (pwchReparsedName)
			{
				RtlInitEmptyUnicodeString( pusNativePath, pwchReparsedName, pNameInfo->Name.Length );
				RtlCopyUnicodeString( pusNativePath, &pNameInfo->Name );

				DoTraceEx( TRACE_LEVEL_WARNING, DC_SYSTEM, "BuildNativePath: result '%wZ'",
					pusNativePath );

				status = STATUS_SUCCESS;
			}
		}
	}

	if (pContext)
	{
		if (pNameInfo)
			ReleaseFileNameInfo( pNameInfo );

		ReleaseFltContext( &pContext );
	}
	else
	{
		if (pNameInfo)
			FltReleaseFileNameInformation( pNameInfo );
	}

	ObDereferenceObject( pFileObject );

	return status;
}

//////////////////////////////////////////////////////////////////////////
RTL_GENERIC_TABLE	gTableModules;
static ERESOURCE	gfmgTables;

typedef struct _tLIMAGEINFO
{
	ULONG			m_Flags;
	LARGE_INTEGER	m_Hash;
	UNICODE_STRING	m_Volume;
	UNICODE_STRING	m_FileName;
}LIMAGEINFO, *PLIMAGEINFO;

RTL_GENERIC_COMPARE_RESULTS
NTAPI
Modules_Compare (
				 PRTL_GENERIC_TABLE pTable,
				 PVOID  FirstStruct,
				 PVOID  SecondStruct
				 )
{
	PLIMAGEINFO pModule1 = (PLIMAGEINFO) FirstStruct;
	PLIMAGEINFO pModule2 = (PLIMAGEINFO) SecondStruct;

	LONG compare = 0;

	UNREFERENCED_PARAMETER( pTable );

	compare = RtlCompareUnicodeString( &pModule1->m_Volume, &pModule2->m_Volume, FALSE );
	if (0 > compare)
		return GenericLessThan;
	else if (0 < compare)
		return GenericGreaterThan;

	compare = RtlCompareUnicodeString( &pModule1->m_FileName, &pModule2->m_FileName, FALSE );

	if (0 > compare)
		return GenericLessThan;
	else if (0 < compare)
		return GenericGreaterThan;

	return GenericEqual;
}

//-----------------------------------------------------------------

#ifdef WIN2K
#define ExAllocateFromPagedLookasideList200 ExAllocatePoolWithTag( PagedPool, 200, tag_pl_200 )
#define ExFreeToPagedLookasideList200( _p ) ExFreePool( _p )

#define ExInitializePagedLookasideList200
#define ExDeletePagedLookasideList200
#else
PAGED_LOOKASIDE_LIST gPL0x200;
PPAGED_LOOKASIDE_LIST gpPL0x200 = &gPL0x200;

#define ExInitializePagedLookasideList200	ExInitializePagedLookasideList(gpPL0x200, NULL, NULL, 0, 0x1000, tag_pl_200, 0);
#define ExDeletePagedLookasideList200		ExDeletePagedLookasideList( gpPL0x200 );

#define ExAllocateFromPagedLookasideList200 ExAllocateFromPagedLookasideList( gpPL0x200 )
#define ExFreeToPagedLookasideList200( _p ) ExFreeToPagedLookasideList(gpPL0x200, _p)
#endif // WIN2K

typedef struct _tSYS_NOTIFY_PROCESS_CONTEXT
{
	HANDLE m_ParentId;
	HANDLE m_ProcessId;
}SYS_NOTIFY_PROCESS_CONTEXT, *PSYS_NOTIFY_PROCESS_CONTEXT;

typedef struct _tSYS_NOTIFY_THREAD_CONTEXT
{
	HANDLE m_ProcessId;
	HANDLE m_ThreadId;
	PIO_WORKITEM m_pWorkItem;
}SYS_NOTIFY_THREAD_CONTEXT, *PSYS_NOTIFY_THREAD_CONTEXT;

/*typedef struct _PEB_LDR_DATA
{
ULONG					m_Length;
ULONG					m_Initialized;//      : UChar?
PVOID					m_SsHandle;
LIST_ENTRY				m_InLoadOrderModuleList;
LIST_ENTRY				m_InMemoryOrderModuleList;
LIST_ENTRY				m_InInitializationOrderModuleList;
PVOID					m_EntryInProgress;//  : Ptr64 Void

}PEB_LDR_DATA, *PPEB_LDR_DATA;*/

#ifdef _WIN64
#include <pshpack8.h>
#endif

typedef struct _LDR_DATA_TABLE_ENTRY {
	LIST_ENTRY				m_InLoadOrderLinks;
	LIST_ENTRY				m_InMemoryOrderLinks;
	LIST_ENTRY				m_InInitializationOrderLinks;
	PVOID					m_DllBase;
	PVOID					m_EntryPoint;
	ULONG					m_SizeOfImage;
	UNICODE_STRING			m_FullDllName;
	UNICODE_STRING			m_BaseDllName;
	ULONG					m_Flags;
	USHORT					m_LoadCount;				// Uint2B
	USHORT					m_TlsIndex;					// Uint2B
	union {
		LIST_ENTRY				m_HashLinks;
		struct {
			PVOID				m_SectionPointer;
			ULONG				m_CheckSum;
		};
	};
	union {
		ULONG					m_TimeDateStamp;
		PVOID					m_LoadedImports;
	};
	PVOID					m_EntryPointActivationContext;	// Ptr64 _ACTIVATION_CONTEXT
	PVOID					m_PatchInformation;
}LDR_DATA_TABLE_ENTRY, *PLDR_DATA_TABLE_ENTRY;

#ifdef _WIN64
#include <poppack.h>
#endif

typedef struct _PROCESS_INFO
{
	IMPLIST_ENTRY			m_List;

	ULONG					m_RefCount;
	HANDLE					m_ProcessId;
	HANDLE					m_ParentProcessId;
	BOOLEAN					m_bActive;
	LARGE_INTEGER			m_StartTime;

	PWCHAR					m_pwchPath;
	USHORT					m_PathLen;

	PWCHAR					m_pwchCmdLine;
	USHORT					m_CmdLineLen;

	PWCHAR					m_pwchCurrDir;
	USHORT					m_CurrDirLen;

	UNICODE_STRING			m_usNativePath;
	md5_byte_t				m_digest[16];
} PROCESS_INFO, *PPROCESS_INFO;

IMPLIST_ENTRY	gProcessInfoList;
ERESOURCE		gProcessInfoListLock;


//////////////////////////////////////////////////////////////////////////

NTSTATUS
FillPidList (
			 __in PLIST_ENTRY pPidHead 
			 )
{
	NTSTATUS status = STATUS_SUCCESS;

	PPID_LIST_ENTRY ppid_entry;
	PIMPLIST_ENTRY Flink;
	PPROCESS_INFO pProcessInfo;

	AcquireResourceExclusive( &gProcessInfoListLock );

	Flink = gProcessInfoList.Flink;

	while(Flink != &gProcessInfoList)
	{
		pProcessInfo = CONTAINING_RECORD( Flink, PROCESS_INFO, m_List );

		if (!pProcessInfo->m_usNativePath.Length)
		{
			// unknown process path!
			//_dbg_break_;
		}
		else
		{
			ppid_entry = ExAllocatePoolWithTag( PagedPool, sizeof (PID_LIST_ENTRY), tag_sysio2 );
			if (ppid_entry)
			{
				RtlZeroMemory( ppid_entry, sizeof(PID_LIST_ENTRY) );

				ppid_entry->pid = pProcessInfo->m_ProcessId;
				memcpy(ppid_entry->digest, pProcessInfo->m_digest, sizeof(md5_byte_t) * 16 );

				ppid_entry->usNativePath.Buffer = ExAllocatePoolWithTag (
					PagedPool,
					pProcessInfo->m_usNativePath.Length,
					tag_sysio3
					);

				if (ppid_entry->usNativePath.Buffer)
				{
					RtlZeroMemory( ppid_entry->usNativePath.Buffer, pProcessInfo->m_usNativePath.Length );
					ppid_entry->usNativePath.Length = 0;
					ppid_entry->usNativePath.MaximumLength = pProcessInfo->m_usNativePath.Length;
					RtlCopyUnicodeString( &ppid_entry->usNativePath, &pProcessInfo->m_usNativePath );
				}

				InsertHeadList(pPidHead, &ppid_entry->ListEntry);
			}
		}

		Flink = Flink->Flink;
	}

	ReleaseResource( &gProcessInfoListLock );

	return status;
}

NTSTATUS
RemovePidList (
			   __in PLIST_ENTRY pPidHead 
			   )
{
	PLIST_ENTRY Flink;
	PPID_LIST_ENTRY ppid_entry;

	if (IsListEmpty( pPidHead ))
		return	STATUS_SUCCESS;

	Flink = pPidHead->Flink;
	while ( Flink != pPidHead )
	{
		ppid_entry = CONTAINING_RECORD( Flink, PID_LIST_ENTRY, ListEntry );

		Flink = Flink->Flink;
		RemoveEntryList( Flink->Blink );

		ExFreePool( ppid_entry->usNativePath.Buffer );

		ExFreePool( ppid_entry );
	}

	return STATUS_SUCCESS;
}
//////////////////////////////////////////////////////////////////////////

void
Sys_FreeProcInfo (
				  PPROCESS_INFO pProcessInfo
				  )
{
	if (pProcessInfo->m_pwchPath)
		ExFreePool( pProcessInfo->m_pwchPath );
	if (pProcessInfo->m_pwchCmdLine)
		ExFreePool( pProcessInfo->m_pwchCmdLine );
	if (pProcessInfo->m_pwchCurrDir)
		ExFreePool( pProcessInfo->m_pwchCurrDir );
	if (pProcessInfo->m_usNativePath.Length)
		ExFreePool( pProcessInfo->m_usNativePath.Buffer );

	ExFreePool( pProcessInfo );
}

HANDLE GetParent (
				  HANDLE ProcessId
				  )
{
	HANDLE hParentProcess = NULL;

	AcquireResourceExclusive( &gProcessInfoListLock );

	if (!_impIsListEmpty( &gProcessInfoList ))
	{
		PPROCESS_INFO pProcessInfo;
		PIMPLIST_ENTRY Flink;

		Flink = gProcessInfoList.Flink;

		while(Flink != &gProcessInfoList)
		{
			pProcessInfo = CONTAINING_RECORD(Flink, PROCESS_INFO, m_List);
			Flink = Flink->Flink;

			if (pProcessInfo->m_ProcessId == ProcessId 
				&& pProcessInfo->m_bActive)
			{
				hParentProcess = pProcessInfo->m_ParentProcessId;
				break;
			}
		}
	}

	ReleaseResource( &gProcessInfoListLock );

	return hParentProcess;
}

PWCHAR
SkipUnicodePrefix (
				   PWCHAR pwcsPath
				   )
{
	PWCHAR p = pwcsPath;

	if (NULL == p)
		return NULL;

	if (p[0] == '\\' && p[1] == '\\' && p[2] == '?' && p[3] == '\\')
		return p+4;

	if (p[0] == '\\' && p[1] == '?' && p[2] == '?' && p[3] == '\\')
		return p+4;

	return p;
}

VOID
RegisterProcessPath (
					 HANDLE ProcessId,
					 HANDLE ParentProcessId,
					 PWCHAR pwchPath,
					 USHORT PathLen,

					 PWCHAR pwchCmdLine,
					 USHORT CmdLineLen,

					 PWCHAR pwchCurrDir,
					 USHORT CurrDirLen
					 )
{
	NTSTATUS status = STATUS_UNSUCCESSFUL;
	USHORT NativePathLen = 0;

	PPROCESS_INFO pProcessInfo = ExAllocatePoolWithTag( PagedPool, sizeof(PROCESS_INFO), tag_proc_info );
	if (!pProcessInfo)
		return;

	memset( pProcessInfo, 0, sizeof(PROCESS_INFO) );

	pProcessInfo->m_RefCount = 0;
	pProcessInfo->m_ProcessId = ProcessId;
	pProcessInfo->m_ParentProcessId = ParentProcessId;
	pProcessInfo->m_bActive = TRUE;

	KeQuerySystemTime( &pProcessInfo->m_StartTime );

	pProcessInfo->m_pwchPath = ExAllocatePoolWithTag( PagedPool, PathLen, tag_proc_info );
	if (pProcessInfo->m_pwchPath)
	{
		pProcessInfo->m_PathLen = PathLen;
		memcpy( pProcessInfo->m_pwchPath, pwchPath, PathLen );
	}

	status = Sys_BuildNativePath( pProcessInfo->m_pwchPath, PathLen, &pProcessInfo->m_usNativePath );
	if (!NT_SUCCESS( status ))
		RtlInitEmptyUnicodeString( &pProcessInfo->m_usNativePath, NULL, 0 );
	else
	{
		if (FlagOn( Globals.m_DrvFlags, _DRV_FLAG_SUPPORT_PROC_HASH ))
		{
			ULONG digest_size = sizeof(pProcessInfo->m_digest);
			Sys_CalcFileHashByName( &pProcessInfo->m_usNativePath, pProcessInfo->m_digest, &digest_size );
		}
	}

	if (CmdLineLen)
	{
		pProcessInfo->m_pwchCmdLine = ExAllocatePoolWithTag( PagedPool, CmdLineLen, tag_proc_info );
		if (pProcessInfo->m_pwchCmdLine)
		{
			pProcessInfo->m_CmdLineLen = CmdLineLen;
			memcpy( pProcessInfo->m_pwchCmdLine, pwchCmdLine, CmdLineLen );
		}
	}

	if (CurrDirLen)
	{
		pProcessInfo->m_pwchCurrDir = ExAllocatePoolWithTag( PagedPool, CurrDirLen, tag_proc_info );
		if (pProcessInfo->m_pwchCurrDir)
		{
			pProcessInfo->m_CurrDirLen = CurrDirLen;
			memcpy( pProcessInfo->m_pwchCurrDir, pwchCurrDir, CurrDirLen );
		}
	}

	AcquireResourceExclusive( &gProcessInfoListLock );

	_impInsertTailList( &gProcessInfoList, &pProcessInfo->m_List );

	ReleaseResource( &gProcessInfoListLock );
}

VOID
UnRegisterProcessPath (
					   HANDLE ProcessId
					   )
{
	AcquireResourceExclusive( &gProcessInfoListLock );

	if (!_impIsListEmpty( &gProcessInfoList ))
	{
		PPROCESS_INFO pProcessInfo;
		PIMPLIST_ENTRY Flink;

		Flink = gProcessInfoList.Flink;

		while(Flink != &gProcessInfoList)
		{
			pProcessInfo = CONTAINING_RECORD(Flink, PROCESS_INFO, m_List);
			Flink = Flink->Flink;

			if (pProcessInfo->m_ProcessId == ProcessId)
			{
				if (!pProcessInfo->m_RefCount)
				{
					_impRemoveEntryList( &pProcessInfo->m_List );

					Sys_FreeProcInfo( pProcessInfo );
				}
				else
				{
					if (pProcessInfo->m_bActive)
						pProcessInfo->m_bActive = FALSE;
				}
			}
		}
	}

	ReleaseResource( &gProcessInfoListLock );
}

VOID
GlobalSystemDataInit (
					  )
{
	ExInitializeResourceLite( &gProcessInfoListLock );
	_impInitializeListHead( &gProcessInfoList );

	ExInitializePagedLookasideList200;

	RtlInitializeGenericTable( &gTableModules, Modules_Compare, Tables_Allocate, Tables_Free, NULL);
	ExInitializeResourceLite( &gfmgTables );

	//виртуализация
	//VirtInit();
	InitAppRul();
}

VOID
GlobalSystemDataDone (
					  )
{
	PVOID pTableEntry = NULL;
	WaitInterlockedObj( &g_SysEnters );

	AcquireResourceExclusive( &gProcessInfoListLock );

	if (!_impIsListEmpty( &gProcessInfoList ))
	{
		PPROCESS_INFO pProcessInfo;
		PIMPLIST_ENTRY Flink;

		Flink = gProcessInfoList.Flink;

		while(Flink != &gProcessInfoList)
		{
			pProcessInfo = CONTAINING_RECORD(Flink, PROCESS_INFO, m_List);
			Flink = Flink->Flink;

			if (!pProcessInfo->m_RefCount)
			{
				_impRemoveEntryList( &pProcessInfo->m_List );
				Sys_FreeProcInfo( pProcessInfo );
			}
			else
			{
				_dbg_break_;
			}
		}
	}

	AcquireResourceExclusive( &gfmgTables );

	//+ free entries..
	pTableEntry = RtlGetElementGenericTable( &gTableModules, 0 );
	while(pTableEntry)
	{
		PLIMAGEINFO pImageInfo = (PLIMAGEINFO) pTableEntry;
		PVOID freeptr = pImageInfo->m_FileName.Buffer;
		RtlDeleteElementGenericTable( &gTableModules, pTableEntry );
		if (freeptr)
			ExFreePool( freeptr );

		pTableEntry = RtlGetElementGenericTable( &gTableModules, 0 );
	};

	//- free entries..
	ReleaseResource( &gfmgTables );

	ReleaseResource( &gProcessInfoListLock );

	ExDeleteResourceLite( &gProcessInfoListLock );
	ExDeleteResourceLite( &gfmgTables );
	
	//виртуализация
	//VirtDone();
	DoneAppRul();

	ExDeletePagedLookasideList200;
}

PWCHAR
QueryProcessPath (
				  HANDLE ProcessId,
				  PULONG pProcessPathLenB
				  )
{
	PWCHAR pwchPath = NULL;
	AcquireResourceExclusive( &gProcessInfoListLock );

	if (!_impIsListEmpty( &gProcessInfoList ))
	{
		PPROCESS_INFO pProcessInfo;
		PIMPLIST_ENTRY Flink;

		Flink = gProcessInfoList.Flink;

		while(Flink != &gProcessInfoList)
		{
			pProcessInfo = CONTAINING_RECORD(Flink, PROCESS_INFO, m_List);
			Flink = Flink->Flink;

			if (pProcessInfo->m_ProcessId == ProcessId
				&& pProcessInfo->m_bActive)
			{
				pwchPath = pProcessInfo->m_pwchPath;

				*pProcessPathLenB = pProcessInfo->m_PathLen;
				pProcessInfo->m_RefCount++;

				break;
			}
		}
	}

	ReleaseResource( &gProcessInfoListLock );

	return pwchPath;
}

VOID
ReleaseProcessPath (
					PWCHAR pwchProcessPath
					)
{
	AcquireResourceExclusive( &gProcessInfoListLock );

	if (!_impIsListEmpty( &gProcessInfoList ))
	{
		PPROCESS_INFO pProcessInfo;
		PIMPLIST_ENTRY Flink;

		Flink = gProcessInfoList.Flink;

		while(Flink != &gProcessInfoList)
		{
			pProcessInfo = CONTAINING_RECORD(Flink, PROCESS_INFO, m_List);
			Flink = Flink->Flink;

			if (pProcessInfo->m_pwchPath == pwchProcessPath)
			{
				pProcessInfo->m_RefCount--;

				if (pProcessInfo->m_RefCount && !pProcessInfo->m_bActive)
				{
					_dbg_break_;

					_impRemoveEntryList( &pProcessInfo->m_List );
					ExFreePool( pProcessInfo );
				}

				break;
			}
		}
	}

	ReleaseResource( &gProcessInfoListLock );
}

NTSTATUS
Sys_QueryProcessHash (
					  HANDLE ProcessId,
					  PVOID pBuffer,
					  ULONG BufferSize
					  )
{
	NTSTATUS status = STATUS_NOT_FOUND;

	md5_byte_t digest[16];

	if (BufferSize != sizeof(digest))
		return STATUS_BUFFER_TOO_SMALL;

	RtlZeroMemory( digest, BufferSize );

	AcquireResourceExclusive( &gProcessInfoListLock );

	if (!_impIsListEmpty( &gProcessInfoList ))
	{
		PPROCESS_INFO pProcessInfo;
		PIMPLIST_ENTRY Flink;

		Flink = gProcessInfoList.Flink;

		while(Flink != &gProcessInfoList)
		{
			pProcessInfo = CONTAINING_RECORD(Flink, PROCESS_INFO, m_List);
			Flink = Flink->Flink;

			if (pProcessInfo->m_ProcessId == ProcessId
				&& pProcessInfo->m_bActive)
			{
				if (BufferSize == RtlCompareMemory( pProcessInfo->m_digest, digest, BufferSize ))
					status = STATUS_DATA_ERROR;
				else
				{
					status = STATUS_SUCCESS;
					RtlCopyMemory( pBuffer, pProcessInfo->m_digest, BufferSize );
				}

				break;
			}
		}
	}

	ReleaseResource( &gProcessInfoListLock );

	return status;
}

//+ ------------------------------------------------------------------------------------------
#ifdef _WIN64
#define _peb_process_ldr_offset			0x18
#define _peb_process_parameters_offset	0x20
#else
#define _peb_process_ldr_offset			0x0c
#define _peb_process_parameters_offset	0x10
#endif

typedef struct _impCURDIR
{
	UNICODE_STRING DosPath;
	HANDLE Handle;
}impCURDIR;

#ifdef _WIN64
#include <pshpack8.h>
#endif

typedef struct _impPROCESS_PARAMETERS
{
	ULONG AllocationSize;
	ULONG Size;
	ULONG Flags;
	ULONG DebugFlags;
#ifdef _WIN64
	HANDLE ConsoleHandle;
#endif
	LONG Console;
#ifndef _WIN64
	ULONG ProcessGroup;
#endif
	HANDLE hStdInput;
	HANDLE hStdOutput;
	HANDLE hStdError;
	impCURDIR CurrentDirectory;
	UNICODE_STRING DllPath;
	UNICODE_STRING ImageFile;
	UNICODE_STRING CommandLine;
	PWSTR Environment;
	ULONG dwX;
	ULONG dwY;
	ULONG dwXSize;
	ULONG dwYSize;
	ULONG dwXCountChars;
	ULONG dwYCountChars;
	ULONG dwFillAttribite;
	ULONG dwFlags;
	ULONG wShowWindow;
	UNICODE_STRING WindowTitle;
	UNICODE_STRING Desktop;
	UNICODE_STRING reserved2;
	UNICODE_STRING reserved3;
}impPROCESS_PARAMETERS;

#ifdef _WIN64
#include <poppack.h>
#endif

VOID
GetUnicodeString (
				  PVOID pPrtFix,
				  PUNICODE_STRING pusString,
				  PWCHAR* ppwchDestString,
				  PUSHORT pDestLen
				  )
{
	*ppwchDestString = NULL;
	*pDestLen = 0;

	try {
		if (pusString->Length && pusString->Buffer)
		{
			PWSTR Buffer = pusString->Buffer;
			USHORT Len = pusString->Length;

			if (Buffer <= (PWSTR) 0x1000)
			{
				PCHAR tmp = (PCHAR) pPrtFix + *(ULONG*) &Buffer;
				Buffer = (PWSTR) tmp;
			}

			*ppwchDestString = ExAllocatePoolWithTag( PagedPool, Len + sizeof(WCHAR), tag_get_ustring );
			if (*ppwchDestString)
			{
				memcpy( *ppwchDestString, Buffer, Len );
				(*ppwchDestString)[Len / sizeof(WCHAR)] = 0;

				*pDestLen = Len + sizeof(WCHAR);
			}
		}
	} except( CheckException() ) {
		if (*ppwchDestString)
		{
			ExFreePool( *ppwchDestString );
			*ppwchDestString = NULL;
			*pDestLen = 0;
		}
	}
}

BOOLEAN
GetProcessInfo (
				__in HANDLE ProcessId,
				__in PEPROCESS pProcess,
				__out PWCHAR* ppwchFileName,
				__out PUSHORT pFileNameLen,
				__out PWCHAR* ppwchCmdLine,
				__out PUSHORT pCmdLineLen,
				__out PWCHAR* ppwchCurrDir,
				__out PUSHORT pCurrDirLen
				)
{
	NTSTATUS status = STATUS_UNSUCCESSFUL;

	HANDLE hProcess = NULL;

	CLIENT_ID			Cid;
	OBJECT_ATTRIBUTES   ObjectAttributes;

	if (!ProcessId || !pProcess)
		return FALSE;

	*ppwchFileName = NULL;
	*pFileNameLen = 0;

	*ppwchCmdLine = NULL;
	*pCmdLineLen = 0;

	Cid.UniqueProcess = ProcessId;
	Cid.UniqueThread = 0;

	InitializeObjectAttributes(&ObjectAttributes, NULL, OBJ_KERNEL_HANDLE, NULL, NULL);

	status = ZwOpenProcess( &hProcess, 0, &ObjectAttributes, &Cid );
	if (NT_SUCCESS( status ) && hProcess)
	{
		PROCESS_BASIC_INFORMATION	ProcInfo;
		status = ZwQueryInformationProcess( hProcess, ProcessBasicInformation, &ProcInfo, sizeof(ProcInfo), NULL );

		if (NT_SUCCESS( status ) && ProcInfo.PebBaseAddress)
		{
			KAPC_STATE  ApcState;

			impPROCESS_PARAMETERS* pProcParams = NULL;
			/*if (*NtBuildNumber >= 5456) // vistab2
			{
			}*/

			KeStackAttachProcess( pProcess, &ApcState );

			try {
				pProcParams = *(impPROCESS_PARAMETERS**) ((PCHAR) ProcInfo.PebBaseAddress + _peb_process_parameters_offset);
			} except( CheckException() ) {
				pProcParams = NULL;
			}

			if (pProcParams)
			{
				GetUnicodeString( pProcParams, &pProcParams->ImageFile, ppwchFileName, pFileNameLen );
				GetUnicodeString( pProcParams, &pProcParams->CommandLine, ppwchCmdLine, pCmdLineLen );
				GetUnicodeString( pProcParams, &pProcParams->CurrentDirectory.DosPath, ppwchCurrDir, pCurrDirLen );
			}

			KeUnstackDetachProcess( &ApcState );
		}

		ZwClose( hProcess );
	}

	return TRUE;
}

VOID
ProcessCrNotify (
				 IN HANDLE ParentId,
				 IN HANDLE ProcessId,
				 IN BOOLEAN Create
				 )
{
	InterlockedIncrement( &g_SysEnters );
	//	DbPrint(DC_SYS, DL_SPAM, ("ProcessCrNotify ParentId=%p ProcessId=%p Create=%#x\n", ParentId, ProcessId, Create));
	// if delete - current process, otherwise no

	if (Create)
	{
		PEPROCESS pProcess = NULL;		

		{
			static int ProcessesStarted = 0;
			if (3 == ProcessesStarted)
				GlobalFidBoxInit();

			ProcessesStarted++;
		}

		CheckProc_RemoveFromList( ProcessId );
		InvProc_CheckNewChild( ParentId, ProcessId );

		if (NT_SUCCESS(PsLookupProcessByProcessId( ProcessId, &pProcess )))
		{
			PFILTER_EVENT_PARAM	pParam = NULL;
			ULONG params_size;

			PWCHAR pwchProcessFileName = NULL;
			USHORT ProcessFileNameLen = 0;

			PWCHAR pwchProcessCmdLine = NULL;
			USHORT ProcessCmdLineLen = 0;

			PWCHAR pwchProcessCurrDir = NULL;
			USHORT ProcessCurrDirLen = 0;

			GetProcessInfo( ProcessId, pProcess,
				&pwchProcessFileName, &ProcessFileNameLen,
				&pwchProcessCmdLine, &ProcessCmdLineLen,
				&pwchProcessCurrDir, &ProcessCurrDirLen
				);

			params_size = sizeof(FILTER_EVENT_PARAM);
			params_size += sizeof(SINGLE_PARAM) + sizeof(ULONG); // _PARAM_OBJECT_SOURCE_ID

			if (pwchProcessFileName && ProcessFileNameLen)
				params_size += sizeof(SINGLE_PARAM) + ProcessFileNameLen;

			if (pwchProcessCmdLine && ProcessCmdLineLen)
				params_size += sizeof(SINGLE_PARAM) + ProcessCmdLineLen;

			if (pwchProcessCurrDir && ProcessCurrDirLen)
				params_size += sizeof(SINGLE_PARAM) + ProcessCurrDirLen;

			pParam = (PFILTER_EVENT_PARAM) ExAllocatePoolWithTag( PagedPool, params_size, tag_sys_params );

			if (pParam)
			{
				PSINGLE_PARAM pSingleParam;

				ULONG ProcessIdTmp = HandleToUlong( ProcessId );

				if (pwchProcessFileName)
				{
					PWCHAR pwchReparsedPath = pwchProcessFileName;
					USHORT uReparsedLen = ProcessFileNameLen;

					ReparsePath( &pwchReparsedPath, &uReparsedLen );

					RegisterProcessPath ( 
						ProcessId,
						ParentId,
						pwchReparsedPath,
						uReparsedLen,
						pwchProcessCmdLine,
						ProcessCmdLineLen,
						pwchProcessCurrDir,
						ProcessCurrDirLen
						);
				}

				FILTER_PARAM_COMMONINIT( pParam, FLTTYPE_SYSTEM, 0xe/*MJ_CREATE_PROCESS_NOTIFY*/, 0, PostProcessing, 1 );
				pParam->ProcessID = *(ULONG*) &ParentId;
				pParam->UnsafeValue = 1;

				pSingleParam = (PSINGLE_PARAM) pParam->Params;
				SINGLE_PARAM_INIT_ULONG(pSingleParam, _PARAM_OBJECT_SOURCE_ID, ProcessIdTmp);

				if (pwchProcessFileName && ProcessFileNameLen)
				{
					pParam->ParamsCount++;

					SINGLE_PARAM_SHIFT( pSingleParam );
					SINGLE_PARAM_INIT_NONE( pSingleParam, _PARAM_OBJECT_URL_W );
					memcpy( pSingleParam->ParamValue, pwchProcessFileName, ProcessFileNameLen );
					pSingleParam->ParamSize = ProcessFileNameLen;

					DoTraceEx( TRACE_LEVEL_INFORMATION, DC_SYSTEM, "ProcessCrNotify: new process %d (0x%0x) started from '%S'",
						ProcessIdTmp, ProcessIdTmp, pwchProcessFileName );
				}
				else
				{
					DoTraceEx( TRACE_LEVEL_WARNING, DC_SYSTEM, "ProcessCrNotify: new process %d (0x%0x) - path not resolved",
						ProcessIdTmp, ProcessIdTmp );
				}

				if (pwchProcessCmdLine && ProcessCmdLineLen)
				{
					pParam->ParamsCount++;

					SINGLE_PARAM_SHIFT( pSingleParam );
					SINGLE_PARAM_INIT_NONE( pSingleParam, _PARAM_OBJECT_STARTUP_STR );
					memcpy( pSingleParam->ParamValue, pwchProcessCmdLine, ProcessCmdLineLen );
					pSingleParam->ParamSize = ProcessCmdLineLen;

					DoTraceEx( TRACE_LEVEL_INFORMATION, DC_SYSTEM, "ProcessCrNotify: new process %d (0x%0x) cmd line '%S'",
						ProcessIdTmp, ProcessIdTmp, pwchProcessCmdLine );
				}

				if (pwchProcessCurrDir && ProcessCurrDirLen)
				{
					pParam->ParamsCount++;

					SINGLE_PARAM_SHIFT( pSingleParam );
					SINGLE_PARAM_INIT_NONE( pSingleParam, _PARAM_OBJECT_BASEPATH );
					memcpy( pSingleParam->ParamValue, pwchProcessCurrDir, ProcessCurrDirLen );
					pSingleParam->ParamSize = ProcessCurrDirLen;

					DoTraceEx( TRACE_LEVEL_INFORMATION, DC_SYSTEM, "ProcessCrNotify: new process %d (0x%0x) base path '%S'",
						ProcessIdTmp, ProcessIdTmp, pwchProcessCurrDir );
				}

				FilterEventImp( pParam, NULL, 0 );

				ExFreePool( pParam );
			}

			ObDereferenceObject( pProcess );

			if (pwchProcessFileName)
				ExFreePool( pwchProcessFileName );

			if (pwchProcessCmdLine)
				ExFreePool( pwchProcessCmdLine );

			if (pwchProcessCurrDir)
				ExFreePool( pwchProcessCurrDir );

		}

#ifndef _WIN64
		// try to init shadow table ptr
		TryInitShadowTablePtr();
#endif
	}
	else
	{
		ULONG ProcessIdTmp = HandleToULong( ProcessId );

		DisconnectClientByProcID(ProcessId);//pClient->m_ProcessId
		InvProc_RemoveFromList( ProcessId );

		{
			BYTE *RequestData = ExAllocateFromPagedLookasideList200;

			if (RequestData != NULL)
			{
				PFILTER_EVENT_PARAM	pParam = (PFILTER_EVENT_PARAM) RequestData;
				PSINGLE_PARAM pSingleParam;

				FILTER_PARAM_COMMONINIT(pParam, FLTTYPE_SYSTEM, 5/*MJ_EXIT_PROCESS*/, 0, PostProcessing, 2);
				pParam->UnsafeValue = 1;
				pSingleParam = (PSINGLE_PARAM) pParam->Params;

				SINGLE_PARAM_INIT_SID(pSingleParam);

				SINGLE_PARAM_SHIFT(pSingleParam);
				SINGLE_PARAM_INIT_ULONG(pSingleParam, _PARAM_OBJECT_SOURCE_ID, ProcessIdTmp);

				FilterEventImp( pParam, NULL, 0 );

				ExFreeToPagedLookasideList200( RequestData );
			}
		}

		// delete context
		UnRegisterProcessPath( ProcessId );

#ifndef _WIN64
		// free XXXKeyRead item
		GenCacheFree(
			g_pKeyReaderCache,
			(ULONGLONG) ProcessId
			);
		GenCacheCleanupByKeyMask(
			g_pAttachedInputThreadCache,
			(ULONGLONG) ProcessId,
			HANDLE_CACHE_PID_MASK
			);
		VMemCacheFree(
			g_pVMemCache,
			ProcessId
			);
		GenCacheCleanupByKeyMask(
			g_pRPCMapperPortsCache,
			(ULONGLONG) ProcessId,
			HANDLE_CACHE_PID_MASK
			);
		GenCacheCleanupByKeyMask(
			g_pDNSResolverPortCache,
			(ULONGLONG) ProcessId,
			HANDLE_CACHE_PID_MASK
			);
#endif
	}

// виртуализация
// 	if (Create)
// 	{
// 		Virt_RebuildPidTree();
// 		if (Virt_IsProcessInSB(ProcessId))
// 		{
// 			PFILTER_EVENT_PARAM pEvent;
// 			pEvent = Virt_ProcEvContext( Create, ProcessId );
// 			if (pEvent)
// 			{
// 				FilterEventImp( pEvent, NULL, 0 );
// 				ExFreePool(pEvent);
// 			}
// 		}
// 	}
// 	else
// 	{
// 		if (Virt_IsProcessInSB(ProcessId))
// 		{
// 			PFILTER_EVENT_PARAM pEvent;
// 			pEvent = Virt_ProcEvContext( Create, ProcessId );
// 			if (pEvent)
// 			{
// 				FilterEventImp( pEvent, NULL, 0 );
// 				ExFreePool(pEvent);
// 			}
// 		}
// 		
// 		Virt_RebuildPidTree();
// 	}
	
	
	
	RebuildPidTree();
	InterlockedDecrement( &g_SysEnters );
}

VOID
ThreadNotify (
			  SYS_NOTIFY_THREAD_CONTEXT* pThreadContext,
			  ULONG Mj
			  )
{
	tefVerdict Verdict = efVerdict_Default;
	BYTE *RequestData;

	InterlockedIncrement( &g_SysEnters );

	RequestData = ExAllocateFromPagedLookasideList200;
	if (RequestData)
	{
		PFILTER_EVENT_PARAM	pParam = (PFILTER_EVENT_PARAM) RequestData;
		PSINGLE_PARAM pSingleParam;

		FILTER_PARAM_COMMONINIT(pParam, FLTTYPE_SYSTEM, Mj, 0, PostProcessing, 3);
		pSingleParam = (PSINGLE_PARAM) pParam->Params;

		SINGLE_PARAM_INIT_SID(pSingleParam);

		SINGLE_PARAM_SHIFT(pSingleParam);
		SINGLE_PARAM_INIT_ULONG(pSingleParam, _PARAM_OBJECT_CLIENTID1, *(ULONG*) &pThreadContext->m_ProcessId);

		SINGLE_PARAM_SHIFT(pSingleParam);
		SINGLE_PARAM_INIT_ULONG(pSingleParam, _PARAM_OBJECT_CLIENTID2, *(ULONG*) &pThreadContext->m_ThreadId);

		// -----------------------------------------------------------------------------------------
		Verdict = FilterEventImp( pParam, NULL, 0 );

		ExFreeToPagedLookasideList200( RequestData );
	}

	InterlockedDecrement( &g_SysEnters );

	return;
}

IO_WORKITEM_ROUTINE Sys_ThreadTerminationWorkProc;

VOID
Sys_ThreadTerminationWorkProc (
							   __in PDEVICE_OBJECT DeviceObject,
							   __in PVOID pItem
							   )
{
	PSYS_NOTIFY_THREAD_CONTEXT pThreadContext = (PSYS_NOTIFY_THREAD_CONTEXT) pItem;

	UNREFERENCED_PARAMETER( DeviceObject );

	ThreadNotify( pThreadContext, 0xa/*MJ_SYSTEM_TERMINATE_THREAD_NOTIFY*/ );

	IoFreeWorkItem( pThreadContext->m_pWorkItem );
	ExFreeToPagedLookasideList200( pThreadContext );

	InterlockedDecrement( &g_SysEnters );
}

VOID
ThreadCrNotify (
				IN HANDLE ProcessId,
				IN HANDLE ThreadId,
				IN BOOLEAN Create
				)
{
	SYS_NOTIFY_THREAD_CONTEXT ThreadContext;

	//	DbPrint(DC_SYS, DL_SPAM, ("ThreadCrNotify ProcessId=%p ThreadId=%p Create=%d CurThID=%p\n", 
	//		ProcessId, ThreadId, Create, (HANDLE) PsGetCurrentThreadId()));
	// if delete - current thread, otherwise no

	ThreadContext.m_ProcessId = ProcessId;
	ThreadContext.m_ThreadId = ThreadId;

	if (Create)
	{
		if (PsGetCurrentProcessId() != ProcessId)
		{
			DoTraceEx( TRACE_LEVEL_INFORMATION, DC_SYSTEM, "process %d create remote thread %d in process %d",
				HandleToULong( PsGetCurrentProcessId() ),
				HandleToULong( ThreadId ),
				HandleToULong( ProcessId )
				);
		}

		ThreadNotify( &ThreadContext, 0x7/*MJ_SYSTEM_CREATE_THREAD*/ );
	}
	else
	{
#ifndef _WIN64
		ULONGLONG Key;
#endif
		// use workitem to use other thread (on current thread will fail comm_send)
		PSYS_NOTIFY_THREAD_CONTEXT pThreadContext = NULL;

		DisconnectClientByThreadID(ThreadId);
		DelInvisibleThread(ThreadId, TRUE);

		pThreadContext = ExAllocateFromPagedLookasideList200;
		if (pThreadContext)
		{
			pThreadContext->m_pWorkItem = IoAllocateWorkItem( Globals.m_FilterControlDeviceObject );
			if (!pThreadContext->m_pWorkItem)
			{
				ExFreeToPagedLookasideList200( pThreadContext );
			}
			else
			{
				pThreadContext->m_ProcessId = ThreadContext.m_ProcessId;
				pThreadContext->m_ThreadId = ThreadContext.m_ThreadId;

				InterlockedIncrement( &g_SysEnters );
				IoQueueWorkItem( pThreadContext->m_pWorkItem, Sys_ThreadTerminationWorkProc, DelayedWorkQueue, pThreadContext );
			}
		}

#ifndef _WIN64
		// clear AttachThreadInput cache item
		Key  = HANDLE_CACHE_KEY_FUNC(
			ProcessId,
			ThreadId
			);
		GenCacheFree(
			g_pAttachedInputThreadCache,
			Key
			);
#endif
	}
}

VOID
LoadImageNotify (
				 IN PUNICODE_STRING FullImageName,
				 IN HANDLE ProcessId,
				 IN PIMAGE_INFO ImageInfo
				 )
{
	UNREFERENCED_PARAMETER( FullImageName );
	UNREFERENCED_PARAMETER( ProcessId );
	UNREFERENCED_PARAMETER( ImageInfo );
}

//+ ------------------------------------------------------------------------------------------

NTSTATUS (__stdcall *klNtOpenProcess) (
									   __out PHANDLE ProcessHandle,
									   __in ACCESS_MASK DesiredAccess,
									   __in POBJECT_ATTRIBUTES ObjectAttributes,
									   __in_opt PCLIENT_ID ClientId
									   );

NTSTATUS
__stdcall
klhOpenProcess (
				__out PHANDLE ProcessHandle,
				__in ACCESS_MASK DesiredAccess,
				__in POBJECT_ATTRIBUTES ObjectAttributes,
				__in_opt PCLIENT_ID ClientId
				)
{
	NTSTATUS status;

	tefVerdict Verdict = efVerdict_Default;
	PCLIENT_ID pClientId = Sys_CopyUserBuffer( ClientId, sizeof(CLIENT_ID) );
	PFILTER_EVENT_PARAM pEvent = EvContext_OpenProcess( DesiredAccess, pClientId );

	if (pClientId)
		ExFreePool( pClientId );

	if (pEvent)
		Verdict = FilterEventImp( pEvent, NULL, 0 );

	if (efIsDeny( Verdict ))
		status = STATUS_ACCESS_DENIED;
	else
		status = klNtOpenProcess( ProcessHandle, DesiredAccess, ObjectAttributes, ClientId );

	if (pEvent)
		ExFreePool( pEvent );

	return status;
}

//+ ------------------------------------------------------------------------------------------
NTSTATUS (__stdcall *klNtTerminateProcess)( 
	__in HANDLE ProcessHandle,
	__in ULONG ProcessExitCode
	);

NTSTATUS
__stdcall
klhTerminateProcess (
					 __in HANDLE ProcessHandle,
					 __in ULONG ProcessExitCode
					 )
{
	NTSTATUS status;
	tefVerdict Verdict = efVerdict_Default;
	PFILTER_EVENT_PARAM pEvent = NULL;

	if (ProcessHandle && ProcessHandle != UlongToHandle(-1))
	{
		HANDLE ProcessId;
		if (NT_SUCCESS( Sys_GetProcessIdByHandle( ProcessHandle, &ProcessId ) ))
			pEvent = EvContext_TerminateProcess( ProcessId );
	}

	if (pEvent)
		Verdict = FilterEventImp( pEvent, NULL, 0 );

	if (efIsDeny( Verdict ))
		status = STATUS_ACCESS_DENIED;
	else
		status = klNtTerminateProcess( ProcessHandle, ProcessExitCode );

	if (pEvent)
		ExFreePool( pEvent );

	return status;
}

//+ ------------------------------------------------------------------------------------------
typedef struct _CONTROL_AREA
{
	PVOID Segment;					//_SEGMENT
	LIST_ENTRY DereferenceList;
	ULONG NumberOfSectionReferences;
	ULONG NumberOfPfnReferences;
	ULONG NumberOfMappedViews;
	USHORT NumberOfSubsections;
	USHORT FlushInProgressCount;
	ULONG NumberOfUserReferences;
	ULONG u;						// __unnamed
	PFILE_OBJECT FilePointer;
	PVOID WaitingForDeletion;		//_EVENT_COUNTER
	USHORT ModifiedWriteCount;
	USHORT NumberOfSystemCacheViews;
}CONTROL_AREA, *PCONTROL_AREA;

typedef struct _SUBSECTION
{
	PCONTROL_AREA ControlArea;
	ULONG u;						//__unnamed
	ULONG StartingSector;
	ULONG NumberOfFullSectors;
	PVOID SubsectionBase;			// _MMPTE
	ULONG UnusedPtes;
	ULONG PtesInSubsection;			//
	struct _SUBSECTION* NextSubsection;
}SUBSECTION, *PSUBSECTION;

typedef struct _SEGMENT_OBJECT
{
	PVOID BaseAddress;
	ULONG TotalNumberOfPtes;
	LARGE_INTEGER SizeOfSegment;
	ULONG NonExtendedPtes;
	ULONG ImageCommitment;
	PCONTROL_AREA* ControlArea;
	PSUBSECTION Subsection;
	PVOID LargeControlArea;			// _LARGE_CONTROL_AREA
	PVOID MmSectionFlags;			// _MMSECTION_FLAGS
	PVOID MmSubSectionFlags;		// _MMSUBSECTION_FLAGS
}SEGMENT_OBJECT, *PSEGMENT_OBJECT;

typedef struct _SECTION_OBJECT
{
	PVOID StartingVa;
	PVOID EndingVa;
	PVOID Parent;
	PVOID LeftChild;
	PVOID RightChild;
	PSEGMENT_OBJECT Segment;
}SECTION_OBJECT, *PSECTION_OBJECT;

extern POBJECT_TYPE *MmSectionObjectType;

PFLT_FILE_NAME_INFORMATION
Sys_QueryProcessFileName (
						  __in HANDLE SectionHandle
						  )
{
	PFLT_FILE_NAME_INFORMATION pFileNameInfo = NULL;
	NTSTATUS status;
	PVOID Object = NULL;

	if (!SectionHandle)
		return NULL;

	status = ObReferenceObjectByHandle( SectionHandle, STANDARD_RIGHTS_REQUIRED, *MmSectionObjectType, UserMode, &Object, NULL );
	if (NT_SUCCESS( status ))
	{
		PSECTION_OBJECT pSection = Object;
		PFILE_OBJECT pFileObject = NULL;
		if (pSection->Segment && pSection->Segment->Subsection && pSection->Segment->Subsection->ControlArea)
			pFileObject = pSection->Segment->Subsection->ControlArea->FilePointer;
		if (pFileObject)
		{
			PMKAV_VOLUME_CONTEXT pVolumeContext = GetVolumeContextFromFO( pFileObject );
			if (pVolumeContext)
			{
				status = FltGetFileNameInformationUnsafe (
					pFileObject,
					pVolumeContext->m_pFltInstance,
					FLT_FILE_NAME_QUERY_DEFAULT | FLT_FILE_NAME_NORMALIZED,
					&pFileNameInfo
					);

				if(NT_SUCCESS( status ))
					FltParseFileNameInformation( pFileNameInfo );
				else
					pFileNameInfo = NULL;
			}

			ReleaseFltContext( &pVolumeContext );
		}

		ObDereferenceObject( Object );
	}

	return pFileNameInfo;
}

NTSTATUS (__stdcall *klNtCreateProcess) (
	__out PHANDLE ProcessHandle,
	__in ACCESS_MASK DesiredAccess,
	__in_opt POBJECT_ATTRIBUTES ObjectAttributes,
	__in HANDLE ParentProcess,
	__in BOOLEAN InheritObjectTable,
	__in_opt HANDLE SectionHandle,
	__in_opt HANDLE DebugPort,
	__in_opt HANDLE ExceptionPort
	);

NTSTATUS
__stdcall
klhCreateProcess (
				  __out PHANDLE ProcessHandle,
				  __in ACCESS_MASK DesiredAccess,
				  __in_opt POBJECT_ATTRIBUTES ObjectAttributes,
				  __in HANDLE ParentProcess,
				  __in BOOLEAN InheritObjectTable,
				  __in_opt HANDLE SectionHandle,
				  __in_opt HANDLE DebugPort,
				  __in_opt HANDLE ExceptionPort
				  )
{
	NTSTATUS status = STATUS_SUCCESS;
	PFLT_FILE_NAME_INFORMATION pFileNameInfo = Sys_QueryProcessFileName( SectionHandle );
	tefVerdict Verdict = efVerdict_Default;
	PFILTER_EVENT_PARAM pEvent = NULL;

	if (pFileNameInfo)
	{
		HANDLE ProcessId = 0;
		if (ParentProcess && UlongToHandle(-1) != ParentProcess)
		{
			if (!NT_SUCCESS( Sys_GetProcessIdByHandle( ProcessHandle, &ProcessId ) ))
				ProcessId = 0;
		}

		pEvent = EvContext_CreateProcess( pFileNameInfo, DesiredAccess, ProcessId );
		FltReleaseFileNameInformation( pFileNameInfo );
		pFileNameInfo = NULL;
	}

	if (pEvent)
		Verdict = FilterEventImp( pEvent, NULL, 0 );

	if (efIsDeny( Verdict ))
		status = STATUS_ACCESS_DENIED;
	else
		status = klNtCreateProcess( ProcessHandle, DesiredAccess, ObjectAttributes, ParentProcess,
		InheritObjectTable, SectionHandle, DebugPort, ExceptionPort);

	return status;
}

NTSTATUS (__stdcall *klNtCreateProcessEx) (
	__out PHANDLE ProcessHandle,
	__in ACCESS_MASK DesiredAccess,
	__in_opt POBJECT_ATTRIBUTES ObjectAttributes,
	__in HANDLE ParentProcess,
	__in BOOLEAN InheritObjectTable,
	__in_opt HANDLE SectionHandle,
	__in_opt HANDLE DebugPort,
	__in_opt HANDLE ExceptionPort,
	__in_opt ULONG UnknownDword
	);

NTSTATUS
__stdcall
klhCreateProcessEx (
					__out PHANDLE ProcessHandle,
					__in ACCESS_MASK DesiredAccess,
					__in_opt POBJECT_ATTRIBUTES ObjectAttributes,
					__in HANDLE ParentProcess,
					__in BOOLEAN InheritObjectTable,
					__in_opt HANDLE SectionHandle,
					__in_opt HANDLE DebugPort,
					__in_opt HANDLE ExceptionPort,
					__in_opt ULONG UnknownDword
					)
{
	NTSTATUS status = STATUS_SUCCESS;
	PFLT_FILE_NAME_INFORMATION pFileNameInfo = Sys_QueryProcessFileName( SectionHandle );
	tefVerdict Verdict = efVerdict_Default;
	PFILTER_EVENT_PARAM pEvent = NULL;

	if (pFileNameInfo)
	{
		HANDLE ProcessId = 0;
		if (ParentProcess && UlongToHandle(-1) != ParentProcess)
		{
			if (!NT_SUCCESS( Sys_GetProcessIdByHandle( ProcessHandle, &ProcessId ) ))
				ProcessId = 0;
		}

		pEvent = EvContext_CreateProcess( pFileNameInfo, DesiredAccess, ProcessId );
		FltReleaseFileNameInformation( pFileNameInfo );
		pFileNameInfo = NULL;
	}

	if (pEvent)
		Verdict = FilterEventImp( pEvent, NULL, 0 );

	if (efIsDeny( Verdict ))
		status = STATUS_ACCESS_DENIED;
	else
		status = klNtCreateProcessEx( ProcessHandle, DesiredAccess, ObjectAttributes, ParentProcess,
		InheritObjectTable, SectionHandle, DebugPort, ExceptionPort, UnknownDword );

	return status;
}

//+ ------------------------------------------------------------------------------------------
NTSTATUS (__stdcall *klNtWriteVirtualMemory) (
	__in HANDLE ProcessHandle,
	__in PVOID StartAddress,
	__in PVOID Buffer,
	__in ULONG BytesToWrite,
	__out_opt PULONG BytesWritten
	);

NTSTATUS
__stdcall
klhWriteVirtualMemory (
					   __in HANDLE ProcessHandle,
					   __in PVOID StartAddress,
					   __in PVOID Buffer,
					   __in ULONG BytesToWrite,
					   __out_opt PULONG BytesWritten
					   )
{

	NTSTATUS status;
	tefVerdict Verdict = efVerdict_Default;
	PFILTER_EVENT_PARAM pEvent = NULL;

	if (ProcessHandle && ProcessHandle != UlongToHandle(-1) && BytesToWrite)
	{
		if (BytesToWrite > 0x2000)
		{
			//?
		}
		else
		{
			HANDLE ProcessId;
			if (NT_SUCCESS( Sys_GetProcessIdByHandle( ProcessHandle, &ProcessId ) ))
			{
				PVOID pBuffer = Sys_CopyUserBuffer( Buffer, BytesToWrite );
				if (pBuffer)
				{
					pEvent = EvContext_WriteProcessMemory( ProcessId, StartAddress, pBuffer, BytesToWrite );
					ExFreePool( pBuffer );
				}
			}
		}
	}

	if (pEvent)
		Verdict = FilterEventImp( pEvent, NULL, 0 );

	if (efIsDeny( Verdict ))
		status = STATUS_ACCESS_DENIED;
	else
		status = klNtWriteVirtualMemory( ProcessHandle, StartAddress, Buffer, BytesToWrite, BytesWritten );

	if (pEvent)
		ExFreePool( pEvent );

	return status;
}

/*#include <pshpack4.h>
typedef struct _USER_STACK {
PVOID FixedStackBase;
PVOID FixedStackLimit;
PVOID ExpandableStackBase;
PVOID ExpandableStackLimit;
PVOID ExpandableStackBottom;
} USER_STACK, *PUSER_STACK;
#include <poppack.h>
*/
NTSTATUS (__stdcall *klCreateThread) (
									  __out PHANDLE ThreadHandle,
									  __in ACCESS_MASK DesiredAccess, 
									  __in_opt POBJECT_ATTRIBUTES ObjectAttributes,
									  __in HANDLE ProcessHandle,
									  __out PCLIENT_ID ClientID, 
									  __in PCONTEXT Context,
									  __in /*PUSER_STACK*/PVOID StackInfo,
									  __in BOOLEAN CreateSuspended
									  );

NTSTATUS
__stdcall
klhCreateThread (
				 __out PHANDLE ThreadHandle,
				 __in ACCESS_MASK DesiredAccess, 
				 __in_opt POBJECT_ATTRIBUTES ObjectAttributes,
				 __in HANDLE ProcessHandle,
				 __out PCLIENT_ID ClientID, 
				 __in PCONTEXT Context,
				 __in /*PUSER_STACK*/PVOID StackInfo,
				 __in BOOLEAN CreateSuspended
				 )
{
	NTSTATUS status;

	HANDLE hDestProcessId;
	PCONTEXT pContextTmp = NULL;
	tefVerdict Verdict = efVerdict_Default;
	PFILTER_EVENT_PARAM pEvent = NULL;

	UNREFERENCED_PARAMETER( ClientID );
	UNREFERENCED_PARAMETER( StackInfo );

	if (!ProcessHandle || (-1 == HandleToUlong( ProcessHandle )))
		return klCreateThread( ThreadHandle, DesiredAccess, ObjectAttributes, ProcessHandle, ClientID, Context, StackInfo, CreateSuspended );

	if (!NT_SUCCESS( Sys_GetProcessIdByHandle( ProcessHandle, &hDestProcessId ) ))
	{
		_dbg_break_;
		hDestProcessId = ProcessHandle; // skip
	}

	if (ProcessHandle == hDestProcessId)
		return klCreateThread( ThreadHandle, DesiredAccess, ObjectAttributes, ProcessHandle, ClientID, Context, StackInfo, CreateSuspended );

	pContextTmp = Sys_CopyUserBuffer( Context, sizeof(CONTEXT) );
	if (pContextTmp)
	{
		pEvent = EvContext_CreateThread( hDestProcessId, DesiredAccess, pContextTmp, CreateSuspended );
		ExFreePool( pContextTmp );
	}

	if (pEvent)
		Verdict = FilterEventImp( pEvent, NULL, 0 );

	if (efIsDeny( Verdict ))
		status = STATUS_ACCESS_DENIED;
	else
		status = klCreateThread( ThreadHandle, DesiredAccess, ObjectAttributes, ProcessHandle, ClientID, Context, StackInfo, CreateSuspended );

	if (NT_SUCCESS( status ))
	{
		if (pEvent)
		{
			pEvent->ProcessingType = PostProcessing;
			FilterEventImp( pEvent, NULL, 0 );
		}
	}

	if (pEvent)
		ExFreePool( pEvent );

	return status;
}

NTSTATUS (__stdcall *klSetContextThread) (
	__in HANDLE hThread,
	__in CONTEXT* Context
	);

NTSTATUS
__stdcall
klhSetContextThread (
					 __in HANDLE hThread,
					 __in CONTEXT* Context
					 )
{
	NTSTATUS status = STATUS_SUCCESS;
	tefVerdict Verdict = efVerdict_Default;

	HANDLE DestProcessId = 0;
	HANDLE DestThreadID = 0;
	PFILTER_EVENT_PARAM pEvent = NULL;

	if (hThread)
	{
		PKTHREAD pkTh;

		status = ObReferenceObjectByHandle( hThread, STANDARD_RIGHTS_REQUIRED, *PsThreadType, KernelMode, &pkTh, NULL);
		if (NT_SUCCESS( status ))
		{
			DestProcessId =  _pfPsGetThreadProcessId( pkTh );
			DestThreadID = _pfPsGetThreadId( pkTh );
			ObDereferenceObject( pkTh );

			if (DestProcessId != PsGetCurrentProcessId())
			{
				PCONTEXT pContextTmp = Sys_CopyUserBuffer( Context, sizeof(CONTEXT) );
				if (pContextTmp)
				{
					pEvent = EvContext_SetContextThread( DestProcessId, DestThreadID, pContextTmp );
					ExFreePool( pContextTmp );
				}

				if (pEvent)
					Verdict = FilterEventImp( pEvent, NULL, 0 );
			}
		}
	}

	if (efIsDeny( Verdict ))
		status = STATUS_ACCESS_DENIED;
	else
		status = klSetContextThread( hThread, Context );

	if (pEvent)
		ExFreePool( pEvent );

	return status;
}

#ifndef _WIN64

NTSTATUS ( NTAPI *klNtClose )(
							  __in HANDLE  Handle
							  ) = NULL;

NTSTATUS  NTAPI klhClose(
							__in HANDLE  Handle
							)
{
	ULONGLONG Key;

	// clear LPC port cache for this handle
	Key  = HANDLE_CACHE_KEY_FUNC(
		PsGetCurrentProcessId(),
		Handle
		);
	GenCacheFree(
		g_pRPCMapperPortsCache,
		Key
		);
	GenCacheFree(
		g_pDNSResolverPortCache,
		Key
		);
	GenCacheFree(
		g_pProtectedStorageHANDLECache,
		Key
		);

	return klNtClose( Handle );
}

NTSTATUS ( NTAPI *klNtQueryInformationThread )(
	__in HANDLE ThreadHandle,
	__in THREADINFOCLASS ThreadInformationClass,
	__out PVOID ThreadInformation,
	__in ULONG ThreadInformationLength,
	__out_opt PULONG ReturnLength
	) = NULL;

//+ win32k hooks ------------------------------------------------------------------------------------
typedef ULONG BOOL;
typedef HANDLE HWND;

#define INVALID_HANDLE_VALUE ( (HANDLE)(ULONG_PTR) (-1) )

// ++ window hooks ---------------------------------------------------------------------------------
HANDLE ( NTAPI *klNtUserSetWindowsHookEx ) (
	__in HANDLE Mod,
	__in PUNICODE_STRING ModuleName,
	__in HANDLE ThreadId,
	__in int HookId,
	__in PVOID HookProc,
	__in BOOL Ansi
	) = NULL;

HANDLE NTAPI klhUserSetWindowsHookEx(
										__in HANDLE Mod,
										__in PUNICODE_STRING ModuleName,
										__in HANDLE ThreadId,
										__in int HookId,
										__in PVOID HookProc,
										__in BOOL Ansi
										)
{
	PUNICODE_STRING p_usModuleName;
	tefVerdict Verdict = efVerdict_Default;
	PWCHAR wcSafeModuleName = NULL;
	PFILTER_EVENT_PARAM pParam;
	ULONG cbModuleName = 0;

	p_usModuleName = (PUNICODE_STRING) Sys_CopyUserBuffer(
		ModuleName,
		sizeof(*ModuleName)
		);
	if ( p_usModuleName )
	{
		wcSafeModuleName = (PWCHAR) Sys_CopyUserBuffer(
			p_usModuleName->Buffer,
			p_usModuleName->Length + sizeof(WCHAR)
			);
		if ( wcSafeModuleName )
		{
			wcSafeModuleName[p_usModuleName->Length / sizeof(WCHAR)] = 0;
			cbModuleName = p_usModuleName->Length + sizeof(WCHAR);
		}

		ExFreePool( p_usModuleName );
	}

	pParam = (PFILTER_EVENT_PARAM) ExAllocatePoolWithTag(
		PagedPool,
		0x200 + cbModuleName,
		tag_event
		);
	if ( pParam )
	{
		PSINGLE_PARAM pSingleParam;

		FILTER_PARAM_COMMONINIT(
			pParam,
			FLTTYPE_SYSTEM,
			0x10/*MJ_SYSTEM_USER_SET_WINDOWS_HOOK_EX*/,
			0,
			PreProcessing,
			( wcSafeModuleName ? 1 : 0 ) + 3
			);

		pSingleParam = (PSINGLE_PARAM) pParam->Params;

		SINGLE_PARAM_INIT_SID( pSingleParam );
		SINGLE_PARAM_SHIFT( pSingleParam );

		SINGLE_PARAM_INIT_ULONG(
			pSingleParam,
			_PARAM_OBJECT_CLIENTID1,
			HookId
			);
		SINGLE_PARAM_SHIFT( pSingleParam );

		SINGLE_PARAM_INIT_ULONG(
			pSingleParam,
			_PARAM_OBJECT_CLIENTID2,
			(ULONG) ThreadId
			);
		SINGLE_PARAM_SHIFT( pSingleParam );

		if ( wcSafeModuleName )
		{
			SINGLE_PARAM_INIT_NONE(
				pSingleParam,
				_PARAM_OBJECT_URL_W
				);
			memcpy(
				pSingleParam->ParamValue,
				wcSafeModuleName,
				cbModuleName
				);
			pSingleParam->ParamSize = cbModuleName;
		}

		Verdict = FilterEventImp(
			pParam,
			NULL,
			0
			);

		ExFreePool( pParam );
	}

	DoTraceEx(
		TRACE_LEVEL_INFORMATION,
		DC_SYSTEM,
		"NtUserSetWindowsHookEx HookId = %d on thread Cid = %!HANDLE!, module name = %S, Verdict = %d",
		HookId,
		ThreadId,
		wcSafeModuleName,
		Verdict
		);

	if ( wcSafeModuleName )
		ExFreePool( wcSafeModuleName );

	if (efIsAllow( Verdict ))
	{
		return
			klNtUserSetWindowsHookEx(
			Mod,
			ModuleName,
			ThreadId,
			HookId,
			HookProc,
			Ansi
			);
	}

	return INVALID_HANDLE_VALUE;
}

/*
* dwFlags for SetWinEventHook
*/
#define WINEVENT_OUTOFCONTEXT   0x0000  // Events are ASYNC
#define WINEVENT_SKIPOWNTHREAD  0x0001  // Don't call back for events on installer's thread
#define WINEVENT_SKIPOWNPROCESS 0x0002  // Don't call back for events on installer's process
#define WINEVENT_INCONTEXT      0x0004  // Events are SYNC, this causes your dll to be injected into every process

ULONG ( NTAPI *klNtUserSetWinEventHook )(
	__in ULONG					eventMin,
	__in ULONG					eventMax,
	__in HANDLE					hmodWinEventProc,
	__in_opt PUNICODE_STRING	pstrLib,
	__in PVOID					pfnWinEventProc,
	__in HANDLE					idEventProcess,
	__in HANDLE					idEventThread,
	__in ULONG					dwFlags
	) = NULL;

ULONG NTAPI klhUserSetWinEventHook(
									  __in ULONG					eventMin,
									  __in ULONG					eventMax,
									  __in HANDLE					hmodWinEventProc,
									  __in_opt PUNICODE_STRING	pstrLib,
									  __in PVOID					pfnWinEventProc,
									  __in HANDLE					idEventProcess,
									  __in HANDLE					idEventThread,
									  __in ULONG					dwFlags
									  )
{
	tefVerdict Verdict = efVerdict_Default;

	if ( ( dwFlags & WINEVENT_INCONTEXT ) && hmodWinEventProc )
	{
		BOOLEAN bResult;
		WCHAR FullModuleName[260];

		// trying to inject DLL...
		bResult = LookupModuleNameByBase(
			hmodWinEventProc,
			FullModuleName,
			sizeof(FullModuleName),
			NULL,
			0
			);
		if ( bResult )
		{
			PFILTER_EVENT_PARAM pParam;
			ULONG cbFullModuleName;

			cbFullModuleName = sizeof(WCHAR) * ( wcslen( FullModuleName ) + 1 );

			pParam = (PFILTER_EVENT_PARAM) ExAllocatePoolWithTag(
				PagedPool,
				0x200 + cbFullModuleName,
				tag_event
				);
			if ( pParam )
			{
				PSINGLE_PARAM pSingleParam;

				FILTER_PARAM_COMMONINIT(
					pParam,
					FLTTYPE_SYSTEM,
					0x19/*MJ_SYSTEM_USER_SETWINEVENTHOOK*/,
					0,
					PreProcessing,
					2;
				);
				pSingleParam = (PSINGLE_PARAM) pParam->Params;

				SINGLE_PARAM_INIT_SID( pSingleParam );
				SINGLE_PARAM_SHIFT( pSingleParam );

				SINGLE_PARAM_INIT_NONE(
					pSingleParam,
					_PARAM_OBJECT_URL_W
					);
				pSingleParam->ParamSize = cbFullModuleName;
				memcpy(
					pSingleParam->ParamValue,
					FullModuleName,
					pSingleParam->ParamSize
					);

				Verdict = FilterEventImp(
					pParam,
					NULL,
					0
					);

				ExFreePool( pParam );

				DoTraceEx(
					TRACE_LEVEL_INFORMATION,
					DC_SYSTEM,
					"NtUserSetWinEventHook module name = %S, Verdict = %d",
					FullModuleName,
					Verdict
					);

			}
		}
	}

	if (efIsAllow( Verdict ))
	{
		return klNtUserSetWinEventHook(
			eventMin,
			eventMax,
			hmodWinEventProc,
			pstrLib,
			pfnWinEventProc,
			idEventProcess,
			idEventThread,
			dwFlags
			);
	}

	return 0;
}
// -- window hooks ---------------------------------------------------------------------------------

typedef enum _WINDOWINFOCLASS {
	WindowProcess = 0,
	WindowThread,
	WindowActiveWindow,
	WindowFocusWindow,
	WindowIsHung,
	WindowClientBase,
	WindowIsForegroundThread,
} WINDOWINFOCLASS; 

// ++ XXXKeyRead keyloggers -------------------------------------------------------------------------
HANDLE ( NTAPI *klNtUserQueryWindow )(
									  HANDLE hWnd,
									  WINDOWINFOCLASS WindowInfo
									  ) = NULL;
HANDLE ( NTAPI *klNtUserGetForegroundWindow )() = NULL;

SHORT ( NTAPI *klNtUserGetAsyncKeyState )(
	__in int vKey
	) = NULL;
SHORT ( NTAPI *klNtUserGetKeyState )(
									 __in int vKey
									 ) = NULL;
BOOL ( NTAPI *klNtUserGetKeyboardState ) (
	__out PBYTE pb
	) = NULL;
BOOL ( NTAPI *klNtUserAttachThreadInput ) (
	__in HANDLE idAttach,
	__in HANDLE idAttachTo,
	__in BOOL fAttach
	) = NULL;

typedef struct _KEY_READER
{
	LARGE_INTEGER	m_LastReadTime;
	ULONG			m_FastReadCount;
} KEY_READER, *PKEY_READER;

#define ASYNC_KEY_READ_TIMEOUT		1000000L	// 100 msecs
#define ASYNC_KEY_READ_THRESHOLD	100L		// increased from 20 - fix bug 30479

tefVerdict
PreKeyRead()
{
	HANDLE PID;
	HANDLE hForegroundWindow;
	BOOLEAN bNeedProcessing = FALSE;
	LARGE_INTEGER CurrTime;
	PKEY_READER pReaderCacheItem;
	BOOLEAN bNeedsEvent = FALSE;
	tefVerdict Verdict = efVerdict_Default;
	BOOLEAN bNewCreated;

	PID = PsGetCurrentProcessId();	

	hForegroundWindow = klNtUserGetForegroundWindow();
	if ( hForegroundWindow )
	{
		HANDLE ForegroundWindowPID;

		// если текущий процесс не имеет foreground-окна			
		ForegroundWindowPID = klNtUserQueryWindow(
			hForegroundWindow,
			WindowProcess
			);
		if ( ForegroundWindowPID && PID != ForegroundWindowPID )
			bNeedProcessing = TRUE;
	}

	if ( !bNeedProcessing )
		return efVerdict_Default;

	KeQueryTickCount( &CurrTime );
	CurrTime.QuadPart *= KeQueryTimeIncrement();

	pReaderCacheItem = (PKEY_READER) GenCacheGet(
		g_pKeyReaderCache,
		(ULONGLONG) PID,
		TRUE,
		&bNewCreated
		);
	if ( pReaderCacheItem )
	{
		if ( !bNewCreated )
		{
			if ( ASYNC_KEY_READ_TIMEOUT > CurrTime.QuadPart - pReaderCacheItem->m_LastReadTime.QuadPart )
				pReaderCacheItem->m_FastReadCount++;
			else
			{
				// out of timeout...
				pReaderCacheItem->m_FastReadCount = 1;
			}

			pReaderCacheItem->m_LastReadTime = CurrTime;

			if ( ASYNC_KEY_READ_THRESHOLD < pReaderCacheItem->m_FastReadCount )
			{
				// we need event..
				bNeedsEvent = TRUE;
				// reset counter
				pReaderCacheItem->m_FastReadCount = 1;
			}
		}
		else
		{
			// new process - potentially a keylogger
			pReaderCacheItem->m_LastReadTime = CurrTime;
			pReaderCacheItem->m_FastReadCount = 1;
		}
	}

	if ( bNeedsEvent )
	{
		PFILTER_EVENT_PARAM	pParam;

		pParam = (PFILTER_EVENT_PARAM) ExAllocatePoolWithTag(
			PagedPool,
			0x200,
			tag_event
			);
		if ( pParam )
		{
			PSINGLE_PARAM pSingleParam;

			FILTER_PARAM_COMMONINIT(
				pParam,
				FLTTYPE_SYSTEM,
				0x14/*MJ_SYSTEM_KEYLOGGER_DETECTED*/,
				0,
				PostProcessing,
				2
				);
			pSingleParam = (PSINGLE_PARAM)pParam->Params;

			SINGLE_PARAM_INIT_SID( pSingleParam );
			SINGLE_PARAM_SHIFT( pSingleParam );

			SINGLE_PARAM_INIT_ULONG(
				pSingleParam,
				_PARAM_OBJECT_KEYLOGGERID,
				KEYLOGGER_GETASYNCKEYSTATE
				);
			SINGLE_PARAM_SHIFT(pSingleParam);

			Verdict = FilterEventImp(
				pParam,
				NULL,
				0
				);

			DoTraceEx(
				TRACE_LEVEL_INFORMATION,
				DC_SYSTEM,
				"keylogger event: key poll"
				);

			ExFreePool( pParam );
		}
	}

	return Verdict;
}

SHORT NTAPI klhUserGetAsyncKeyState(
									   __in int vKey
									   )
{
	if ( vKey < '0'	|| vKey > 'Z' || efIsAllow( PreKeyRead() ) )
		return klNtUserGetAsyncKeyState( vKey );

	return 0;
}

SHORT NTAPI klhUserGetKeyState(
								  __in int vKey
								  )
{
	if ( vKey < '0' || vKey > 'Z' || efIsAllow( PreKeyRead() ) )
		return klNtUserGetKeyState( vKey );

	return 0;
}

BOOL NTAPI klhUserGetKeyboardState(
									  __out PBYTE pb
									  )
{
	BOOLEAN bNewItem;
	ULONGLONG Key;
	PVOID pRet;

	Key = HANDLE_CACHE_KEY_FUNC(
		PsGetCurrentProcessId(),
		PsGetCurrentThreadId()
		);
	pRet = GenCacheGet(
		g_pAttachedInputThreadCache,
		Key,
		FALSE,
		&bNewItem
		);
	if ( pRet )
	{
		// thread has attached to another thread input
		if (efIsDeny( PreKeyRead() ))
			return FALSE;
	}

	return klNtUserGetKeyboardState( pb );
}

// maintain cache of threads that attached their inputs to other threads..
BOOL NTAPI klhUserAttachThreadInput (
										__in HANDLE idAttach,
										__in HANDLE idAttachTo,
										__in BOOL fAttach
										)
{
	BOOLEAN bResult;

	bResult = (BOOLEAN) klNtUserAttachThreadInput(
		idAttach,
		idAttachTo,
		fAttach
		);
	if ( fAttach && bResult && idAttach != idAttachTo )
	{
		ULONGLONG Key;
		BOOLEAN bNewItem;

		Key = HANDLE_CACHE_KEY_FUNC(
			PsGetCurrentProcessId(),
			idAttach
			);

		GenCacheGet(
			g_pAttachedInputThreadCache,
			Key,
			TRUE,
			&bNewItem
			);

		DoTraceEx(
			TRACE_LEVEL_INFORMATION,
			DC_SYSTEM,
			"NtUserAttachThreadInput from Cid = %!HANDLE! to Cid = %!HANDLE!, result = %!BOOLEAN!, new = %!BOOLEAN!",
			idAttach,
			idAttachTo,
			bResult,
			bNewItem
			);
	}

	return bResult;		
}
// -- XXXKeyRead keyloggers -------------------------------------------------------------------------

// ++ SendInput/FindWindow --------------------------------------------------------------------------
#define INPUT_MOUSE     0
#define INPUT_KEYBOARD  1
#define INPUT_HARDWARE  2

#include <pshpack1.h>

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

UINT ( NTAPI *klNtUserSendInput )(
	__in UINT    cInputs,
	__in CONST INPUT *pInputs,
	__in int     cbSize
	) = NULL;

HANDLE ( NTAPI *klNtUserFindWindowEx )(
	__in HANDLE hwndParent,
	__in HANDLE hwndChild,
	__in PUNICODE_STRING pstrClassName,
	__in PUNICODE_STRING pstrWindowName,
	__in ULONG dwType
	) = NULL;

tefVerdict
CommonUserSendInput(
	__in PINPUT pInputs,
	__in ULONG nInputs
	)
{
	tefVerdict Verdict = efVerdict_Default;
	ULONG nRealInputs;
	PINPUT pSafeInputs;
	ULONG cbInputs;
	ULONG i;
	ULONG bInvThreadTarget = FALSE;

	ASSERT( klNtUserGetForegroundWindow );
	ASSERT( klNtUserQueryWindow );

	nRealInputs = min( 1024, nInputs );
	cbInputs = sizeof(INPUT) * nRealInputs;

	pSafeInputs = Sys_CopyUserBuffer(
		pInputs,
		cbInputs
		);
	if ( pSafeInputs )
	{
		PFILTER_EVENT_PARAM pParam;
		BOOLEAN bHasKeyboardEntries;

		bHasKeyboardEntries = FALSE;
		for ( i = 0; i < nRealInputs; i++ )
		{
			PINPUT pSafeInput = &pSafeInputs[i];

			switch ( pSafeInput->type )
			{
			case INPUT_KEYBOARD:
				bHasKeyboardEntries = TRUE;
				break;
			}
		}

		if ( bHasKeyboardEntries )
		{
			HANDLE hForegroundWindow = klNtUserGetForegroundWindow();

			if ( hForegroundWindow )
			{
				HANDLE ForegroundWindowTID;

				ForegroundWindowTID = klNtUserQueryWindow(
					hForegroundWindow,
					WindowThread
					);

				if ( ForegroundWindowTID && IsInvisible( ForegroundWindowTID, NULL ) )
					bInvThreadTarget = TRUE;
			}
		}

		pParam = (PFILTER_EVENT_PARAM) ExAllocatePoolWithTag(
			PagedPool,
			0x200 + cbInputs,
			tag_event
			);
		if ( pParam )
		{
			PSINGLE_PARAM pSingleParam;

			FILTER_PARAM_COMMONINIT(
				pParam,
				FLTTYPE_SYSTEM,
				0x15/*MJ_SYSTEM_SENDINPUT*/,
				0,
				PreProcessing,
				4
				);
			pSingleParam = (PSINGLE_PARAM) pParam->Params;

			SINGLE_PARAM_INIT_SID( pSingleParam );
			SINGLE_PARAM_SHIFT( pSingleParam );

			SINGLE_PARAM_INIT_ULONG(
				pSingleParam,
				_PARAM_OBJECT_PARAMCOUNT,
				nRealInputs
				);
			SINGLE_PARAM_SHIFT(pSingleParam);

			SINGLE_PARAM_INIT_NONE(
				pSingleParam,
				_PARAM_OBJECT_BINARYDATA
				);
			pSingleParam->ParamSize = cbInputs;
			memcpy(
				pSingleParam->ParamValue,
				pSafeInputs,
				pSingleParam->ParamSize
				);
			SINGLE_PARAM_SHIFT( pSingleParam );

			SINGLE_PARAM_INIT_ULONG(
				pSingleParam,
				_PARAM_OBJECT_INVIS_TARGET,
				bInvThreadTarget
				);
			SINGLE_PARAM_SHIFT( pSingleParam );

			Verdict = FilterEventImp(
				pParam,
				NULL,
				0
				);

			ExFreePool( pParam );
		}

		ExFreePool( pSafeInputs );
	}

	DoTraceEx(
		TRACE_LEVEL_INFORMATION,
		DC_SYSTEM,
		"send input: source Pid = %!HANDLE!, bInvThreadTarget = %!BOOLEAN!, Verdict = %d",
		PsGetCurrentProcessId(),
		(BOOLEAN) bInvThreadTarget,
		Verdict
		);

	return Verdict;
}

UINT NTAPI klhUserSendInput(
	__in UINT cInputs,
	__in PINPUT pInputs,
	__in int cbSize
	)
{
	tefVerdict Verdict = efVerdict_Default;

	Verdict = CommonUserSendInput(
		pInputs,
		cInputs
		);

	if (efIsAllow( Verdict ))
	{
		return klNtUserSendInput(
			cInputs,
			pInputs,
			cbSize
			);
	}

	return 0;
}

HANDLE klhUserFindWindowEx(
	__in HANDLE hwndParent,
	__in HANDLE hwndChild,
	__in PUNICODE_STRING pstrClassName,
	__in PUNICODE_STRING pstrWindowName,
	__in ULONG dwType
	)
{
	tefVerdict Verdict = efVerdict_Default;
	HANDLE hResult;
	PWCHAR wcClassName = NULL;
	PWCHAR wcWindowName = NULL;
	ULONG cbClassName = 0;
	ULONG cbWindowName = 0;
	PFILTER_EVENT_PARAM pParam;

	if ( pstrClassName )
		wcClassName = GetUserWideStrFromUni( pstrClassName );
	if ( pstrWindowName )
		wcWindowName = GetUserWideStrFromUni( pstrWindowName );

	if ( wcClassName )
		cbClassName = sizeof(WCHAR) * ( wcslen( wcClassName ) + 1 );
	if ( wcWindowName )
		cbWindowName = sizeof(WCHAR) * ( wcslen( wcWindowName ) + 1 );

	if ( pstrClassName || pstrWindowName )
	{
		pParam = (PFILTER_EVENT_PARAM) ExAllocatePoolWithTag(
			PagedPool,
			0x200 + cbClassName + cbWindowName,
			tag_event
			);
		if  ( pParam )
		{
			PSINGLE_PARAM pSingleParam;

			FILTER_PARAM_COMMONINIT(
				pParam,
				FLTTYPE_SYSTEM,
				0x18/*MJ_SYSTEM_MJ_SYSTEM_FINDWINDOW*/,
				0,
				PreProcessing,
				( wcClassName ? 1 : 0 ) + ( wcWindowName ? 1 : 0 ) + 1
				);
			pSingleParam = (PSINGLE_PARAM) pParam->Params;

			SINGLE_PARAM_INIT_SID( pSingleParam );
			SINGLE_PARAM_SHIFT( pSingleParam );

			if ( wcClassName )
			{
				SINGLE_PARAM_INIT_NONE(
					pSingleParam,
					_PARAM_OBJECT_URL_W
					);
				pSingleParam->ParamSize = cbClassName;
				memcpy(
					pSingleParam->ParamValue,
					wcClassName,
					pSingleParam->ParamSize
					);
				SINGLE_PARAM_SHIFT( pSingleParam );
			}

			if ( wcWindowName )
			{
				SINGLE_PARAM_INIT_NONE(
					pSingleParam,
					_PARAM_OBJECT_URL_PARAM_W
					);
				pSingleParam->ParamSize = cbWindowName;
				memcpy(
					pSingleParam->ParamValue,
					wcWindowName,
					pSingleParam->ParamSize
					);
				SINGLE_PARAM_SHIFT( pSingleParam );
			}


			Verdict = FilterEventImp(
				pParam,
				NULL,
				0
				);

			ExFreePool( pParam );
			pParam = NULL;
		}
	}

	DoTraceEx(
		TRACE_LEVEL_INFORMATION,
		DC_SYSTEM,
		"NtUserFindWindow: Pid = %!HANDLE! ClassName = %S WindowName = %S, Verdict pre = %d",
		PsGetCurrentProcessId(),
		wcClassName,
		wcWindowName,
		Verdict
		);

	if ( wcClassName )
		ExFreePool( wcClassName );
	if ( wcWindowName )
		ExFreePool( wcWindowName );

	if (efIsDeny( Verdict ))
		return NULL;

	hResult = klNtUserFindWindowEx(
		hwndParent,
		hwndChild,
		pstrClassName,
		pstrWindowName,
		dwType
		);

	pParam = (PFILTER_EVENT_PARAM) ExAllocatePoolWithTag(
		PagedPool,
		0x200,
		tag_event
		);
	if ( pParam )
	{
		PSINGLE_PARAM pSingleParam;

		FILTER_PARAM_COMMONINIT(
			pParam,
			FLTTYPE_SYSTEM,
			0x18/*MJ_SYSTEM_MJ_SYSTEM_FINDWINDOW*/,
			0,
			PostProcessing,
			( hResult ? 1 : 0 ) + 2
			);
		pSingleParam = (PSINGLE_PARAM) pParam->Params;

		SINGLE_PARAM_INIT_SID( pSingleParam );
		SINGLE_PARAM_SHIFT( pSingleParam );

		SINGLE_PARAM_INIT_ULONG(
			pSingleParam,
			_PARAM_OBJECT_OBJECTHANDLE,
			(ULONG) hResult
			);
		SINGLE_PARAM_SHIFT( pSingleParam );

		if ( hResult )
		{
			HANDLE TargetTid;
			ULONG bInvThreadTarget = 0;

			TargetTid = klNtUserQueryWindow(
				hResult,
				WindowThread
				);
			if ( TargetTid && IsInvisible( TargetTid, NULL ) )
				bInvThreadTarget = TRUE;

			SINGLE_PARAM_INIT_ULONG(
				pSingleParam,
				_PARAM_OBJECT_INVIS_TARGET,
				bInvThreadTarget
				);
			SINGLE_PARAM_SHIFT( pSingleParam );
		}

		Verdict =  FilterEventImp(
			pParam,
			NULL,
			0
			);

		ExFreePool( pParam );

		DoTraceEx(
			TRACE_LEVEL_INFORMATION,
			DC_SYSTEM,
			"NtUserFindWindow: Pid = %!HANDLE! Verdict post = %d",
			PsGetCurrentProcessId(),
			Verdict
			);
	}

	if ( efIsDeny(Verdict) )
		return NULL;

	return hResult;
}
// -- SendInput/FindWindow --------------------------------------------------------------------------

// ++ screenshot protection -------------------------------------------------------------------------
#define DCPT_DCORG				6

#define DDW_ISMEMDC				7

typedef HANDLE HDC;
typedef HANDLE HBITMAP;

#include <pshpack1.h>

typedef struct tagPOINT
{
	LONG x;
	LONG y;
} POINT, *PPOINT, *LPPOINT;

#include <poppack.h>

typedef unsigned int UINT;

BOOL ( NTAPI *klNtGdiGetDCPoint )(
								  HDC     hdc,
								  UINT    iPoint,
								  PPOINT pptOut
								  ) = NULL;

BOOL ( NTAPI *klNtGdiGetDCDword )(
								  HDC hdc,
								  UINT u,
								  DWORD *pdwResult
								  ) = NULL;
HANDLE ( NTAPI *klNtUserWindowFromPoint )(
	POINT point
	) = NULL;

BOOL ( NTAPI *klNtGdiBitBlt )(
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
							  ) = NULL;
BOOL ( NTAPI *klNtGdiStretchBlt )(
								  HDC   hdcDst,
								  int   xDst,
								  int   yDst,
								  int   cxDst,
								  int   cyDst,
								  HDC   hdcSrc,
								  int   xSrc,
								  int   ySrc,
								  int   cxSrc,
								  int   cySrc,
								  DWORD dwRop,
								  DWORD dwBackColor
								  ) = NULL;
BOOL ( NTAPI *klNtGdiMaskBlt )(
							   HDC     hdc,
							   int     xDst,
							   int     yDst,
							   int     cx,
							   int     cy,
							   HDC     hdcSrc,
							   int     xSrc,
							   int     ySrc,
							   HBITMAP hbmMask,
							   int     xMask,
							   int     yMask,
							   DWORD   dwRop4,
							   DWORD   crBackColor
							   ) = NULL;
BOOL ( NTAPI *klNtGdiPlgBlt )(
							  HDC     hdcTrg,
							  LPPOINT pptlTrg,
							  HDC     hdcSrc,
							  int     xSrc,
							  int     ySrc,
							  int     cxSrc,
							  int     cySrc,
							  HBITMAP hbmMask,
							  int     xMask,
							  int     yMask,
							  DWORD   crBackColor
							  ) = NULL;

BOOLEAN
Gdi32_GetDCOrgEx (
	__in HDC hdc,
	__out PPOINT pptl
	)
{
	BOOLEAN bResult;
	PPOINT pOrgDC = NULL;
	PVMEM_CACHE_ITEM pCacheItem;

	ASSERT( klNtGdiGetDCPoint );

	// allocate from user-space  - NtGdiGetDCPoint works only with r3-buffers
	pCacheItem = VMemCacheGet(
		g_pVMemCache,
		PsGetCurrentProcessId()
		);
	if ( !pCacheItem )
		return FALSE;

	ExAcquireFastMutex( &pCacheItem->m_UserPageLock );

	bResult = (BOOLEAN) klNtGdiGetDCPoint(
		hdc,
		DCPT_DCORG,
		pCacheItem->m_UserPagePtr
		);
	if ( bResult )
		*pptl = *(PPOINT) pCacheItem->m_UserPagePtr;

	ExReleaseFastMutex( &pCacheItem->m_UserPageLock );

	return bResult;
}

BOOLEAN
IsMemDC (
	__in HDC hDC,
	__out PBOOLEAN p_bIsMemDC
	)
{
	BOOL bResult;
	BOOLEAN bRet = FALSE;
	PVMEM_CACHE_ITEM pCacheItem;

	ASSERT( klNtGdiGetDCDword );

	// allocate from user-space  - NtGdiGetDCDword works only with r3-buffers
	pCacheItem = VMemCacheGet(
		g_pVMemCache,
		PsGetCurrentProcessId()
		);
	if ( !pCacheItem )
		return FALSE;

	ExAcquireFastMutex( &pCacheItem->m_UserPageLock );

	bResult = klNtGdiGetDCDword(
		hDC,
		DDW_ISMEMDC,
		pCacheItem->m_UserPagePtr
		);
	if ( bResult )
	{
		*p_bIsMemDC = !! *(PULONG) pCacheItem->m_UserPagePtr;
		bRet = TRUE;
	}

	ExReleaseFastMutex( &pCacheItem->m_UserPageLock );

	return bRet;
}

HANDLE
PidFromPoint (
	__in x,
	__in y
	)
{
	POINT p = { x, y };
	HANDLE hWnd;

	ASSERT( klNtUserWindowFromPoint );
	ASSERT( klNtUserQueryWindow );

	hWnd = klNtUserWindowFromPoint( p );
	if ( hWnd )
	{
		HANDLE Pid = klNtUserQueryWindow(
			hWnd,
			WindowProcess
			);
		return Pid;
	}

	return NULL;
}

tefVerdict
BltCommon (
	__in HDC hdcSrc,
	__in int xSrc,
	__in int ySrc,
	__in int cx,
	__in int cy
	)
{
	tefVerdict Verdict = efVerdict_Default;
	BOOLEAN bResult;
	BOOLEAN bIsMemDC;
	POINT OrgDC;
	PFILTER_EVENT_PARAM	pParam;

	bResult = IsMemDC(
		hdcSrc,
		&bIsMemDC
		);
	if ( !bResult )
		return efVerdict_Default;		// unknown hdc

	if ( bIsMemDC )						// we are interested only in non-memory DCs
		return efVerdict_Default;


	// get DC origin
	// we cannot do it in user-mode on other thread - device contexts are process-specific
	bResult = Gdi32_GetDCOrgEx(
		hdcSrc,
		&OrgDC
		);
	if ( bResult )
	{
		ULONG	x1 = OrgDC.x + xSrc,
			y1 = OrgDC.y + ySrc,
			x2 = x1 + cx,
			y2 = y1 + cy;
		BOOLEAN bOwnWindow = TRUE;
		HANDLE Pid, WndOwnerPid;
		BOOLEAN bIsExplorer = FALSE;
		CHAR ProcName[PROCNAMELEN];

		Pid = PsGetCurrentProcessId();

		GetShortProcName (
			ProcName,
			NULL
			);

		bIsExplorer = 0 == _stricmp( "explorer.exe", ProcName );

		// check weather source region to copy is completely within process own windows
		WndOwnerPid = PidFromPoint(
			x1,
			y1
			);
		if ( WndOwnerPid || !bIsExplorer )
			bOwnWindow = Pid == WndOwnerPid;

		WndOwnerPid = PidFromPoint(
			x2,
			y1
			);
		if ( WndOwnerPid || !bIsExplorer )
			bOwnWindow = bOwnWindow && (Pid == WndOwnerPid);

		WndOwnerPid = PidFromPoint(
			x2,
			y2
			);
		if ( WndOwnerPid || !bIsExplorer )
			bOwnWindow = bOwnWindow && (Pid == WndOwnerPid);

		WndOwnerPid = PidFromPoint(
			x1,
			y2
			);
		if ( WndOwnerPid || !bIsExplorer )
			bOwnWindow = bOwnWindow && (Pid == WndOwnerPid);

		if ( !bOwnWindow )		// trying to blit from window not belonging to current process - SCREENSHOOTER
		{
			pParam = (PFILTER_EVENT_PARAM) ExAllocatePoolWithTag(
				PagedPool,
				0x200,
				'CBrg');
			if ( pParam )
			{
				PSINGLE_PARAM pSingleParam;

				FILTER_PARAM_COMMONINIT(
					pParam,
					FLTTYPE_SYSTEM,
					0x1b/*MJ_SYSTEM_BITBLT*/,
					0,
					PreProcessing,
					1
					);

				pSingleParam = (PSINGLE_PARAM) pParam->Params;

				SINGLE_PARAM_INIT_SID( pSingleParam );
				SINGLE_PARAM_SHIFT( pSingleParam );

				Verdict = FilterEventImp(
					pParam,
					NULL,
					0
					);

				DoTraceEx(
					TRACE_LEVEL_INFORMATION,
					DC_SYSTEM,
					"SCREENSHOOTER:: Pid = %!HANDLE! x1 = %d, y1 = %d, x2 = %d, y2 = %d, Verdict = %d",
					PsGetCurrentProcessId(),
					x1,
					y1,
					x2,
					y2,
					Verdict
					);

				ExFreePool( pParam );
			}	
		}
	}

	return Verdict;
}

BOOL
NTAPI
klhGdiBitBlt (
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
	tefVerdict Verdict = BltCommon( hdcSrc, xSrc, ySrc, cx, cy );

	if (efIsAllow( Verdict ))
		return klNtGdiBitBlt( hdcDst, x, y, cx, cy, hdcSrc, xSrc, ySrc, rop4, crBackColor, Unknown );

	return FALSE;
}

BOOL
NTAPI
klhGdiStretchBlt (
	HDC   hdcDst,
	int   xDst,
	int   yDst,
	int   cxDst,
	int   cyDst,
	HDC   hdcSrc,
	int   xSrc,
	int   ySrc,
	int   cxSrc,
	int   cySrc,
	DWORD dwRop,
	DWORD dwBackColor
	)
{
	tefVerdict Verdict = BltCommon( hdcSrc, xSrc, ySrc, cxSrc, cySrc );

	if (efIsAllow( Verdict ))
		return klNtGdiStretchBlt( hdcDst, xDst, yDst, cxDst, cyDst, hdcSrc, xSrc, ySrc, cxSrc, cySrc, dwRop, dwBackColor );

	return FALSE;
}

BOOL
NTAPI
klhGdiMaskBlt (
	HDC     hdc,
	int     xDst,
	int     yDst,
	int     cx,
	int     cy,
	HDC     hdcSrc,
	int     xSrc,
	int     ySrc,
	HBITMAP hbmMask,
	int     xMask,
	int     yMask,
	DWORD   dwRop4,
	DWORD   crBackColor
	)
{
	tefVerdict Verdict = BltCommon( hdcSrc, xSrc, ySrc, cx, cy );

	if (efIsAllow( Verdict ))
		return klNtGdiMaskBlt( hdc, xDst, yDst, cx, cy, hdcSrc, xSrc, ySrc, hbmMask, xMask, yMask, dwRop4, crBackColor );

	return FALSE;
}

BOOL NTAPI klhGdiPlgBlt (
	HDC     hdcTrg,
	LPPOINT pptlTrg,
	HDC     hdcSrc,
	int     xSrc,
	int     ySrc,
	int     cxSrc,
	int     cySrc,
	HBITMAP hbmMask,
	int     xMask,
	int     yMask,
	DWORD   crBackColor
	)
{
	tefVerdict Verdict = BltCommon( hdcSrc, xSrc, ySrc, cxSrc, cySrc );

	if (efIsAllow( Verdict ))
		return klNtGdiPlgBlt( hdcTrg, pptlTrg, hdcSrc, xSrc, ySrc, cxSrc, cySrc, hbmMask, xMask, yMask, crBackColor );

	return FALSE;
}
// -- screenshot protection -------------------------------------------------------------------------

// ++ LPC/COM stuff ---------------------------------------------------------------------------------
#define PORT_NAME_TAPI		L"\\RPC Control\\tapsrvlpc"
#define PIPE_NAME_ROUTER	L"\\??\\PIPE\\ROUTER"

#define PIPE_NTSVCS			L"\\??\\PIPE\\ntsvcs"

NTSTATUS
( NTAPI *klNtOpenFile )(
		   __out PHANDLE  FileHandle,
		   __in ACCESS_MASK  DesiredAccess,
		   __in POBJECT_ATTRIBUTES  ObjectAttributes,
		   __out PIO_STATUS_BLOCK  IoStatusBlock,
		   __in ULONG  ShareAccess,
		   __in ULONG  OpenOptions
		   ) = NULL;

NTSTATUS
 NTAPI klhOpenFile (
	__out PHANDLE  FileHandle,
	__in ACCESS_MASK  DesiredAccess,
	__in POBJECT_ATTRIBUTES  ObjectAttributes,
	__out PIO_STATUS_BLOCK  IoStatusBlock,
	__in ULONG  ShareAccess,
	__in ULONG  OpenOptions
)
{
	NTSTATUS ntStatus;
	UNICODE_STRING usPipeNameRouter;
	PUNICODE_STRING p_usSafePipeNameRouter = NULL;
	PUNICODE_STRING PipeNameRouter = ObjectAttributes->ObjectName;
	
	// bypass kernel usage
	if ( KernelMode == ExGetPreviousMode() )
	{
		return klNtOpenFile (
			FileHandle,
			DesiredAccess,
			ObjectAttributes,
			IoStatusBlock,
			ShareAccess,
			OpenOptions
			);
	}

	do 
	{
		p_usSafePipeNameRouter = GetUserUniStr( PipeNameRouter );

		if (p_usSafePipeNameRouter)
		{
			RtlInitUnicodeString(&usPipeNameRouter, PIPE_NAME_ROUTER);

			//RasDial API
			if ( RtlCompareUnicodeString(&usPipeNameRouter, p_usSafePipeNameRouter, TRUE) == 0 )
			{
				tefVerdict Verdict = efVerdict_Default;
				PFILTER_EVENT_PARAM pEvent = NULL;
				pEvent = EvContext_RasDial ( );
				if (pEvent)
				{
					Verdict = FilterEventImp( pEvent, NULL, 0 );
					ExFreePool( pEvent );
				}

				if ( efIsDeny(Verdict) )
				{
					ntStatus = STATUS_ACCESS_DENIED;
					break;
				}
			}
		}

		ntStatus = klNtOpenFile (
			FileHandle,
			DesiredAccess,
			ObjectAttributes,
			IoStatusBlock,
			ShareAccess,
			OpenOptions
			);
		
		
		if ( !NT_SUCCESS(ntStatus) )
			break;
		
		if (p_usSafePipeNameRouter)
		{
			if ( FileHandle )
			{
				PHANDLE p_Handle = Sys_CopyUserBuffer(
					FileHandle,
					sizeof( *FileHandle )
					);
				if ( p_Handle )
				{
					LONG lResult;

					ULONGLONG Key = HANDLE_CACHE_KEY_FUNC(
						PsGetCurrentProcessId(),
						*p_Handle
						);
					BOOLEAN bNewItem;

					//Protected Storage
					if (g_bIsW2K)
					{
						UNICODE_STRING usPipeNtsvcs;
						RtlInitUnicodeString( &usPipeNtsvcs, PIPE_NTSVCS );
						lResult = RtlCompareUnicodeString(&usPipeNtsvcs, p_usSafePipeNameRouter, TRUE);
						if ( 0 == lResult )
						{
							// connection to RPC port mapper here												
							GenCacheGet(
								g_pProtectedStorageHANDLECache,
								Key,
								TRUE,
								&bNewItem
								);
							if ( !bNewItem )
							{
								_dbg_break_;

								DoTraceEx(
									TRACE_LEVEL_ERROR,
									DC_SYSTEM,
									"ununique RPC via LPC port handles"
									);
							}
						}
					}

					ExFreePool( p_Handle );
				}
			}
		}
	} while(FALSE);

	if (p_usSafePipeNameRouter)
		ExFreePool( p_usSafePipeNameRouter );


	return ntStatus;
}

NTSTATUS  
( NTAPI *klNtCreateFile)(
						 __out PHANDLE  FileHandle,
						 __in ACCESS_MASK  DesiredAccess,
						 __in POBJECT_ATTRIBUTES  ObjectAttributes,
						 __out PIO_STATUS_BLOCK  IoStatusBlock,
						 __in_opt PLARGE_INTEGER  AllocationSize,
						 __in ULONG  FileAttributes,
						 __in ULONG  ShareAccess,
						 __in ULONG  CreateDisposition,
						 __in ULONG  CreateOptions,
						 __in_opt PVOID  EaBuffer,
						 __in ULONG  EaLength
						 ) = NULL;
NTSTATUS
NTAPI klhCreateFile (
	__out PHANDLE  FileHandle,
	__in ACCESS_MASK  DesiredAccess,
	__in POBJECT_ATTRIBUTES  ObjectAttributes,
	__out PIO_STATUS_BLOCK  IoStatusBlock,
	__in_opt PLARGE_INTEGER  AllocationSize,
	__in ULONG  FileAttributes,
	__in ULONG  ShareAccess,
	__in ULONG  CreateDisposition,
	__in ULONG  CreateOptions,
	__in_opt PVOID  EaBuffer,
	__in ULONG  EaLength
)
{
	NTSTATUS ntStatus;
	UNICODE_STRING usPipeNameRouter;
	PUNICODE_STRING p_usSafePipeNameRouter = NULL;
	PUNICODE_STRING PipeNameRouter = ObjectAttributes->ObjectName;
	
	// bypass kernel usage
	if ( KernelMode == ExGetPreviousMode() )
	{
		return klNtCreateFile(
			FileHandle,
			DesiredAccess,
			ObjectAttributes,
			IoStatusBlock,
			AllocationSize,
			FileAttributes,
			ShareAccess,
			CreateDisposition,
			CreateOptions,
			EaBuffer,
			EaLength
			);
	}
	
	do 
	{
		p_usSafePipeNameRouter = GetUserUniStr( PipeNameRouter );
	
		if (p_usSafePipeNameRouter)
		{
			RtlInitUnicodeString(&usPipeNameRouter, PIPE_NAME_ROUTER);

			//RasDial API
			if ( RtlCompareUnicodeString(&usPipeNameRouter, p_usSafePipeNameRouter, TRUE) == 0 )
			{
				tefVerdict Verdict = efVerdict_Default;
				PFILTER_EVENT_PARAM pEvent = NULL;
				pEvent = EvContext_RasDial (  );
				if (pEvent)
				{
					Verdict = FilterEventImp( pEvent, NULL, 0 );
					ExFreePool( pEvent );
				}

				if ( efIsDeny(Verdict) )
				{
					ntStatus = STATUS_ACCESS_DENIED;
					break;
				}
			}
		}

		ntStatus = klNtCreateFile(
							FileHandle,
							DesiredAccess,
							ObjectAttributes,
							IoStatusBlock,
							AllocationSize,
							FileAttributes,
							ShareAccess,
							CreateDisposition,
							CreateOptions,
							EaBuffer,
							EaLength
							);

		
		if ( !NT_SUCCESS(ntStatus) )
			break;

		if (p_usSafePipeNameRouter)
		{
			if ( FileHandle )
			{
				PHANDLE p_Handle = Sys_CopyUserBuffer(
					FileHandle,
					sizeof( *FileHandle )
					);
				if ( p_Handle )
				{
					LONG lResult;

					ULONGLONG Key = HANDLE_CACHE_KEY_FUNC(
						PsGetCurrentProcessId(),
						*p_Handle
						);
					BOOLEAN bNewItem;

					//Protected Storage
					if (g_bIsW2K)
					{
						UNICODE_STRING usPipeNtsvcs;
						RtlInitUnicodeString( &usPipeNtsvcs, PIPE_NTSVCS );
						lResult = RtlCompareUnicodeString(&usPipeNtsvcs, p_usSafePipeNameRouter, TRUE);
						if ( 0 == lResult )
						{
							// connection to RPC port mapper here												
							GenCacheGet(
								g_pProtectedStorageHANDLECache,
								Key,
								TRUE,
								&bNewItem
								);
							if ( !bNewItem )
							{
								_dbg_break_;

								DoTraceEx(
									TRACE_LEVEL_ERROR,
									DC_SYSTEM,
									"ununique RPC via LPC port handles"
									);
							}
						}
					}

					ExFreePool( p_Handle );
				}
			}
		}

	} while(FALSE);

	if (p_usSafePipeNameRouter)
		ExFreePool( p_usSafePipeNameRouter );


	return ntStatus;
}


#define MAGIC_0_4 0x03000005
#define DEFINE_GUID2(name, l, w1, w2, b1, b2, b3, b4, b5, b6, b7, b8) \
	const GUID name \
	= { l, w1, w2, { b1, b2, b3, b4, b5, b6, b7, b8 } }
DEFINE_GUID2(GUID_PROTECTED_STORAGE, 0x8a078c30, 0x3755, 0x11d0, 0xa0, 0xbd, 0x0, 0xaa, 0x0,
			 0x61, 0x42, 0x6a);

//#pragma pack(1)
typedef struct NT_SERVICE_CONTROL_MSG
{
	ULONG Magic;		// 0x00
	ULONG Unknown1;		// 0x04
	ULONG PacketSize;	// 0x08
	ULONG MessageID;	// 0x0C
	ULONG Unknown2;		// 0x10
	ULONG Unknown3;		// 0x14
	GUID Guid;			// 0x18
} NT_SERVICE_CONTROL_MSG;
//#pragma pack()

NTSTATUS ( NTAPI *klNtFsControlFile)(
	__in HANDLE  FileHandle,
	__in HANDLE  Event OPTIONAL,
	__in_opt PIO_APC_ROUTINE  ApcRoutine,
	__in PVOID  ApcContext OPTIONAL,
	__out PIO_STATUS_BLOCK  IoStatusBlock,
	__in ULONG  FsControlCode,
	__in_opt PVOID  InputBuffer,
	__in ULONG  InputBufferLength,
	__out_opt PVOID  OutputBuffer,
	__in ULONG  OutputBufferLength
) = NULL;

NTSTATUS 
NTAPI klhFsControlFile(
	__in HANDLE  FileHandle,
	__in_opt HANDLE  Event,
	__in_opt PIO_APC_ROUTINE  ApcRoutine,
	__in_opt PVOID  ApcContext,
	__out PIO_STATUS_BLOCK  IoStatusBlock,
	__in ULONG  FsControlCode,
	__in_opt PVOID  InputBuffer,
	__in ULONG  InputBufferLength,
	__out_opt PVOID  OutputBuffer,
	__in ULONG  OutputBufferLength
)
{
	PVOID pData;
	BOOLEAN bCreated;
	ULONGLONG Key;
	VERDICT Verdict = Verdict_NotFiltered;
	NTSTATUS ntStatus;
	
	// bypass kernel usage
	if ( KernelMode == ExGetPreviousMode() )
	{
		return klNtFsControlFile(
			FileHandle,
			Event,
			ApcRoutine,
			ApcContext,
			IoStatusBlock,
			FsControlCode,
			InputBuffer,
			InputBufferLength,
			OutputBuffer,
			OutputBufferLength
			);
	}
	
	do 
	{
		if ( g_bIsW2K )
		{
			Key  = HANDLE_CACHE_KEY_FUNC( PsGetCurrentProcessId(), FileHandle );

			// check if PortHandle is "\RPC Control\epmapper" port
			pData = GenCacheGet(
				g_pProtectedStorageHANDLECache,
				Key,
				FALSE,
				&bCreated
				);
			if ( pData )
			{
				// this is Protected Storage handle
				// the packet must not be too small
				if (InputBufferLength>=sizeof(NT_SERVICE_CONTROL_MSG))
				{
					// proper verification of InputBuffer should be here
					// + make a local copy of the interesting part of the packet
					NT_SERVICE_CONTROL_MSG *pkMsg = Sys_CopyUserBuffer(
						InputBuffer,
						sizeof( NT_SERVICE_CONTROL_MSG )
						);

					// first 4 bytes must exactly match the magic value
					if (pkMsg)
					{
						if ((pkMsg->Magic == MAGIC_0_4))
						{
							// if GUID is the base storage provider ID
							if (RtlCompareMemory(&pkMsg->Guid, &GUID_PROTECTED_STORAGE, sizeof(GUID)) == sizeof(GUID))
							{
								tefVerdict Verdict = efVerdict_Default;
								PFILTER_EVENT_PARAM pEvent = NULL;
								pEvent = EvContext_ProtectedStorage ();
								if (pEvent)
								{
									Verdict = FilterEventImp( pEvent, NULL, 0 );
									ExFreePool( pEvent );
								}

								if ( efIsDeny(Verdict) )
								{
									ntStatus = STATUS_ACCESS_DENIED;
									break;
								}
							}
						}
						ExFreePool(pkMsg);
					}
				}
			}
		}

		ntStatus = klNtFsControlFile(
			FileHandle,
			Event,
			ApcRoutine,
			ApcContext,
			IoStatusBlock,
			FsControlCode,
			InputBuffer,
			InputBufferLength,
			OutputBuffer,
			OutputBufferLength
			);
	
	} while(FALSE);

	return ntStatus;
}

typedef struct tagRAWINPUTDEVICE {
		USHORT usUsagePage;
		USHORT usUsage;
		DWORD dwFlags;
		HWND hwndTarget;
} RAWINPUTDEVICE, *PRAWINPUTDEVICE, *LPRAWINPUTDEVICE;
typedef CONST RAWINPUTDEVICE* PCRAWINPUTDEVICE;

BOOL (NTAPI *klNtRegisterRawInputDevices)(
	PCRAWINPUTDEVICE pRawInputDevices,
	UINT uiNumDevices,
	UINT cbSize
) = NULL;

BOOL NTAPI klhRegisterRawInputDevices(
	PCRAWINPUTDEVICE pRawInputDevices,
	UINT uiNumDevices,
	UINT cbSize
	)
{
	BOOL ret;
	
	tefVerdict Verdict = efVerdict_Default;
	PFILTER_EVENT_PARAM pEvent = NULL;
	
	// bypass kernel usage
	if ( KernelMode == ExGetPreviousMode() )
	{
		return klhRegisterRawInputDevices(
			pRawInputDevices,
			uiNumDevices,
			cbSize
			);
	}
	
	pEvent = EvContext_RegRawInputDevices ();
	if (pEvent)
	{
		Verdict = FilterEventImp( pEvent, NULL, 0 );
		ExFreePool( pEvent );
	}

	if ( efIsDeny(Verdict) )
	{
		ret = FALSE;
	
	}
	else
	{
		ret = klNtRegisterRawInputDevices(
			pRawInputDevices,
			uiNumDevices,
			cbSize
			);
	}

	return ret;
}

ULONG_PTR (NTAPI *klNtUserCallOneParam)(
	IN ULONG_PTR dwParam,
	IN DWORD xpfnProc
) = NULL;


ULONG_PTR NTAPI klhUserCallOneParam(
	IN ULONG_PTR dwParam,
	IN DWORD xpfnProc
)
{
	ULONG_PTR ret = 0;
	tefVerdict Verdict = efVerdict_Default;
	PFILTER_EVENT_PARAM pEvent = NULL;
	ULONG SFI_PREPAREFORLOGOFF = 0x34;

	if (g_bIsW2K)
	{
 		SFI_PREPAREFORLOGOFF = 0x38;	
	}

	//KdPrint(("klhUserCallOneParam dwParam = %d, xpfnProc = %d\n", dwParam, xpfnProc));

	// bypass kernel usage
	if ( KernelMode == ExGetPreviousMode() )
	{
		return  klNtUserCallOneParam(
			dwParam,
			xpfnProc
			);
	}
	
	if (SFI_PREPAREFORLOGOFF == xpfnProc )
	{
		pEvent = EvContext_Shutdown ();
		if (pEvent)
		{
			Verdict = FilterEventImp( pEvent, NULL, 0 );
			ExFreePool( pEvent );
		}

		if ( efIsDeny(Verdict) )
		{
			ret = FALSE;
			return ret;
		}	
	}	
		
	ret =  klNtUserCallOneParam(
			dwParam,
			xpfnProc
			);
	
	return ret;
}

typedef struct _PORT_SECTION_WRITE {
	ULONG Length;
	HANDLE SectionHandle;
	ULONG SectionOffset;
	ULONG ViewSize;
	PVOID ViewBase;
	PVOID TargetViewBase;
} PORT_SECTION_WRITE, *PPORT_SECTION_WRITE;

typedef struct _PORT_SECTION_READ {
	ULONG Length;
	ULONG ViewSize;
	ULONG ViewBase;
} PORT_SECTION_READ, *PPORT_SECTION_READ;

#define PORT_NAME_PSTORE L"\\RPC Control\\protected_storage"

NTSTATUS ( NTAPI *klNtSecureConnectPort )(
	__out PHANDLE PortHandle,
	__in PUNICODE_STRING PortName,
	__in PSECURITY_QUALITY_OF_SERVICE SecurityQos,
	__inout_opt PPORT_SECTION_WRITE WriteSection,
	__in_opt PSID ServerSid, 
	__inout_opt PPORT_SECTION_READ ReadSection,
	__out_opt PULONG MaxMessageSize,
	__inout_opt PVOID ConnectData,
	__inout_opt PULONG ConnectDataLength
	) = NULL;

NTSTATUS
NTAPI klhSecureConnectPort(
	__out PHANDLE PortHandle,
	__in PUNICODE_STRING PortName,
	__in PSECURITY_QUALITY_OF_SERVICE SecurityQos,
	__inout_opt PPORT_SECTION_WRITE WriteSection,
	__in_opt PSID ServerSid, 
	__inout_opt PPORT_SECTION_READ ReadSection,
	__out_opt PULONG MaxMessageSize,
	__inout_opt PVOID ConnectData,
	__inout_opt PULONG ConnectDataLength
)
{
	NTSTATUS ntStatus;
	UNICODE_STRING usPortNameTapi;
	UNICODE_STRING usPortNamePstore;
	PUNICODE_STRING p_usSafePortName = NULL;
	
	// bypass kernel usage
	if ( KernelMode == ExGetPreviousMode() )
	{
		return klNtSecureConnectPort(
			PortHandle,
			PortName,
			SecurityQos,
			WriteSection,
			ServerSid, 
			ReadSection,
			MaxMessageSize,
			ConnectData,
			ConnectDataLength
			);
	}

	do 
	{
		p_usSafePortName = GetUserUniStr( PortName );

		if (p_usSafePortName)
		{
			RtlInitUnicodeString(&usPortNameTapi, PORT_NAME_TAPI);
			
			//KdPrint(("klhSecureConnectPort PortName = %wZ\n", p_usSafePortName ));

			//RasDial API
			if ( RtlCompareUnicodeString(&usPortNameTapi, p_usSafePortName, TRUE) == 0 )
			{
				tefVerdict Verdict = efVerdict_Default;
				PFILTER_EVENT_PARAM pEvent = NULL;
				pEvent = EvContext_RasDial ( );
				if (pEvent)
				{
					Verdict = FilterEventImp( pEvent, NULL, 0 );
					ExFreePool( pEvent );
				}

				if ( efIsDeny(Verdict) )
				{
					ntStatus = STATUS_ACCESS_DENIED;
					break;
				}
			}

			if (g_bIsWXP)
			{
				//Protected Storage
				RtlInitUnicodeString(&usPortNamePstore, PORT_NAME_PSTORE);

				if ( RtlCompareUnicodeString(&usPortNamePstore, p_usSafePortName, TRUE) == 0 )
				{
					tefVerdict Verdict = efVerdict_Default;
					PFILTER_EVENT_PARAM pEvent = NULL;
					pEvent = EvContext_ProtectedStorage ();
					if (pEvent)
					{
						Verdict = FilterEventImp( pEvent, NULL, 0 );
						ExFreePool( pEvent );
					}

					if ( efIsDeny(Verdict) )
					{
						ntStatus = STATUS_ACCESS_DENIED;
						break;
					}
				}
			}
		}
	
		ntStatus = klNtSecureConnectPort(
			PortHandle,
			PortName,
			SecurityQos,
			WriteSection,
			ServerSid, 
			ReadSection,
			MaxMessageSize,
			ConnectData,
			ConnectDataLength
			);

			
	} while(FALSE);

	if (p_usSafePortName)
		ExFreePool( p_usSafePortName );

	
	return ntStatus;
}

#define RPC_MAPPER_PORT		L"\\RPC Control\\epmapper"
#define DNS_RESOLVER_PORT	L"\\RPC Control\\DNSResolver"

NTSTATUS ( NTAPI *klNtConnectPort )(
	__out PHANDLE PortHandle,
	__in PUNICODE_STRING PortName,
	__in PSECURITY_QUALITY_OF_SERVICE SecurityQos,
	__inout_opt PPORT_VIEW ClientView,
	__inout_opt PREMOTE_PORT_VIEW ServerView,
	__out_opt PULONG MaxMessageLength,
	__inout_opt PVOID ConnectionInformation,
	__inout_opt PULONG ConnectionInformationLength
) = NULL;

NTSTATUS ( NTAPI *klNtRequestWaitReplyPort)(
	__in HANDLE PortHandle,
	__in PPORT_MESSAGE RequestMessage,
	__out PPORT_MESSAGE ReplyMessage
	) = NULL;

NTSTATUS
NTAPI klhConnectPort (
	__out PHANDLE PortHandle,
	__in PUNICODE_STRING PortName,
	__in PSECURITY_QUALITY_OF_SERVICE SecurityQos,
	__inout_opt PPORT_VIEW ClientView,
	__inout_opt PREMOTE_PORT_VIEW ServerView,
	__out_opt PULONG MaxMessageLength,
	__inout_opt PVOID ConnectionInformation,
	__inout_opt PULONG ConnectionInformationLength
)
{
	NTSTATUS ntStatus;
	UNICODE_STRING usPortNameTapi;
	UNICODE_STRING usPortNamePstore;
	PUNICODE_STRING p_usSafePortName = NULL;
	
	// bypass kernel usage
	if ( KernelMode == ExGetPreviousMode() )
	{
		return klNtConnectPort(
			PortHandle,
			PortName,
			SecurityQos,
			ClientView,
			ServerView,
			MaxMessageLength,
			ConnectionInformation,
			ConnectionInformationLength
			);
	}

	do 
	{
		p_usSafePortName = GetUserUniStr( PortName );
		
			

		if (p_usSafePortName)
		{
			RtlInitUnicodeString(&usPortNameTapi, PORT_NAME_TAPI);

			//KdPrint(("klhConnectPort PortName = %wZ\n", p_usSafePortName ));

			//RasDial API
			if ( RtlCompareUnicodeString(&usPortNameTapi, p_usSafePortName, TRUE) == 0 )
			{
				tefVerdict Verdict = efVerdict_Default;
				PFILTER_EVENT_PARAM pEvent = NULL;
				pEvent = EvContext_RasDial ( );
				if (pEvent)
				{
					Verdict = FilterEventImp( pEvent, NULL, 0 );
					ExFreePool( pEvent );
				}
				
				if ( efIsDeny(Verdict) )
				{
					ntStatus = STATUS_ACCESS_DENIED;
					break;
				}
			}

			if (g_bIsWXP)
			{
				//Protected Storage
				RtlInitUnicodeString(&usPortNamePstore, PORT_NAME_PSTORE);

				if ( RtlCompareUnicodeString(&usPortNamePstore, p_usSafePortName, TRUE) == 0 )
				{
					tefVerdict Verdict = efVerdict_Default;
					PFILTER_EVENT_PARAM pEvent = NULL;
					pEvent = EvContext_ProtectedStorage ();
					if (pEvent)
					{
						Verdict = FilterEventImp( pEvent, NULL, 0 );
						ExFreePool( pEvent );
					}

					if ( efIsDeny(Verdict) )
					{
						ntStatus = STATUS_ACCESS_DENIED;
						break;
					}
				}
			}
		}
		
		ntStatus = klNtConnectPort(
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
			UNICODE_STRING usDNSResolver;
			//PUNICODE_STRING p_usSafePortName;

			RtlInitUnicodeString(
				&usPortMapper,
				RPC_MAPPER_PORT
				);
			RtlInitUnicodeString(
				&usDNSResolver,
				DNS_RESOLVER_PORT
				);

			if ( p_usSafePortName )
			{
				LONG lResult;

				if ( PortHandle )
				{
					PHANDLE p_hPort = Sys_CopyUserBuffer(
						PortHandle,
						sizeof( *PortHandle )
						);
					if ( p_hPort )
					{
						ULONGLONG Key = HANDLE_CACHE_KEY_FUNC(
							PsGetCurrentProcessId(),
							*p_hPort
							);
						BOOLEAN bNewItem;

						lResult = RtlCompareUnicodeString(
							&usPortMapper,
							p_usSafePortName,
							TRUE
							);
						if ( 0 == lResult )
						{
							// connection to RPC port mapper here												
							GenCacheGet(
								g_pRPCMapperPortsCache,
								Key,
								TRUE,
								&bNewItem
								);
							if ( !bNewItem )
							{
								_dbg_break_;

								DoTraceEx(
									TRACE_LEVEL_ERROR,
									DC_SYSTEM,
									"ununique RPC via LPC port handles"
									);
							}
						}
						else
						{
							lResult = RtlCompareUnicodeString(
								&usDNSResolver,
								p_usSafePortName,
								TRUE
								);
							if ( 0 == lResult )
							{
								// connection to DNS resolver port
								GenCacheGet(
									g_pDNSResolverPortCache,
									Key,
									TRUE,
									&bNewItem
									);
								if ( !bNewItem )
								{
									_dbg_break_;

									DoTraceEx(
										TRACE_LEVEL_ERROR,
										DC_SYSTEM,
										"ununique DNS resolver port handles"
										);
								}
							}
						}
						
						ExFreePool( p_hPort );
					}
				}


			}
		}
	} while(FALSE);

	if (p_usSafePortName)
			ExFreePool( p_usSafePortName );

	return ntStatus;
}

#define LPC_MAX_DATA_SIZE_OLE_INTEREST 0x240
#define LPC_OLE_MAGIC_4_4 0x00040002
#define LPC_OLE_MAGIC_2_4 LPC_OLE_MAGIC_4_4
#define LPC_OLE_MAGIC_4_4_MASK 0xFFFF00FF
#define LPC_OLE_MAGIC_2_4_MASK LPC_OLE_MAGIC_4_4_MASK
#define LPC_OLE_MAGIC_STACK_0_4_W2K 0x00060005
#define LPC_OLE_MAGIC_STACK_0_4_XPVISTA 0x00070005

typedef PVOID LPC_PVOID;

#include <pshpack1.h>

typedef struct _PORT_DATA_ENTRY {
	LPC_PVOID Base;
	ULONG Size;
} PORT_DATA_ENTRY, *PPORT_DATA_ENTRY;

typedef struct _PORT_DATA_INFORMATION {
	ULONG CountDataEntries;
	PORT_DATA_ENTRY DataEntries[1];
} PORT_DATA_INFORMATION, *PPORT_DATA_INFORMATION;

#define PORT_MESSAGE_SIZE		sizeof(PORT_MESSAGE)

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

#include <poppack.h>

// CoCreateObject on LOCAL_SERVER through LPC
tefVerdict
CommonLPCCreateObject (
	__in PPORT_MESSAGE RequestMessage
	)
{
	tefVerdict Verdict = efVerdict_Default;
	PPORT_MESSAGE pHeader;

	pHeader = (PPORT_MESSAGE) Sys_CopyUserBuffer(
		RequestMessage,
		sizeof(PORT_MESSAGE)
		);
	if ( !pHeader )
		return efVerdict_Default;

	// a) the message must have some minimum size
	if ( pHeader->u1.s1.TotalLength >= PORT_MESSAGE_SIZE )
	{
		PUCHAR pData;
		ULONG DataSize;

		DataSize = min( pHeader->u1.s1.DataLength, LPC_MAX_DATA_SIZE_OLE_INTEREST );
		pData = Sys_CopyUserBuffer(
			(PUCHAR) RequestMessage + PORT_MESSAGE_SIZE,
			DataSize
			);
		if ( pData )
		{
			// b) VirtualRangesOffset must be non-zero and must not point too far in the data part
			if ( pHeader->u2.s2.DataInfoOffset &&
				pHeader->u2.s2.DataInfoOffset + sizeof( PORT_DATA_INFORMATION ) <= (ULONG) pHeader->u1.s1.TotalLength
				)
			{
				ULONG Data_2_4;
				ULONG Data_4_4;
				BOOLEAN b2kMagic;
				BOOLEAN bXPMagic;

				// Windows 2000 SP4 & Windows XP SP2
				Data_2_4 = * (PULONG) ( pData + 2 );
				Data_4_4 = * (PULONG) ( pData + 4 );

				// c) message magics (type) must match, W2k magic starts at offset 2 in data,
				// WXP magic starts at offset 4 in data
				b2kMagic = g_bIsW2K && ( LPC_OLE_MAGIC_2_4 == ( Data_2_4 & LPC_OLE_MAGIC_2_4_MASK ) );
				bXPMagic = g_bIsWXP && ( LPC_OLE_MAGIC_4_4 == ( Data_4_4 & LPC_OLE_MAGIC_4_4_MASK ) );
				if ( b2kMagic || bXPMagic)
				{
					PPORT_DATA_INFORMATION pPortDataInfo;

					pPortDataInfo = ( PPORT_DATA_INFORMATION ) ( pData + ( pHeader->u2.s2.DataInfoOffset - PORT_MESSAGE_SIZE ) );

					if ( pPortDataInfo->CountDataEntries >= 1 )
					{
						PPORT_DATA_ENTRY pDataEntry;

						pDataEntry = &pPortDataInfo->DataEntries[0];
						// there is a pointer to "real data" stored inside caller's process memory
						// d) the size of "real data" must be at least sizeof(LPC_OLE_DATA_STACK)
						if ( pDataEntry->Size >= sizeof(LPC_OLE_DATA_STACK) )
						{
							PLPC_OLE_DATA_STACK pLocalStackCopy;

							pLocalStackCopy = (PLPC_OLE_DATA_STACK) Sys_CopyUserBuffer(
								pDataEntry->Base,
								pDataEntry->Size
								);
							if ( pLocalStackCopy )
							{
								BOOLEAN bInRange;

								// e) "real data" magics must match and the GUID offset must point inside the data
								b2kMagic = g_bIsW2K && ( LPC_OLE_MAGIC_STACK_0_4_W2K == pLocalStackCopy->magic1 );
								bXPMagic = g_bIsWXP && ( LPC_OLE_MAGIC_STACK_0_4_XPVISTA == pLocalStackCopy->magic1 );

								bInRange = pLocalStackCopy->offset + FIELD_OFFSET( LPC_OLE_DATA_STACK, data ) + sizeof(GUID) <= pDataEntry->Size;

								if ( ( b2kMagic || bXPMagic ) && bInRange )
								{
									GUID Guid;
									PFILTER_EVENT_PARAM pParam;

									memcpy(
										&Guid,
										pLocalStackCopy->data + pLocalStackCopy->offset,
										sizeof(GUID)
										);

									pParam = (PFILTER_EVENT_PARAM) ExAllocatePoolWithTag(
										PagedPool,
										0x200,
										'OCrg');
									if ( pParam )
									{
										PSINGLE_PARAM pSingleParam;

										FILTER_PARAM_COMMONINIT(
											pParam,
											FLTTYPE_SYSTEM,
											0x20/*MJ_SYSTEM_COCREATEINSTANCE*/,
											0,
											PreProcessing,
											2
											);
										pSingleParam = (PSINGLE_PARAM) pParam->Params;

										SINGLE_PARAM_INIT_SID( pSingleParam );
										SINGLE_PARAM_SHIFT( pSingleParam );

										SINGLE_PARAM_INIT_NONE(
											pSingleParam,
											_PARAM_OBJECT_CLSID
											);
										memcpy(
											pSingleParam->ParamValue,
											&Guid,
											sizeof(Guid)
											);
										pSingleParam->ParamSize = sizeof(Guid);
										SINGLE_PARAM_SHIFT(pSingleParam);

										Verdict = FilterEventImp(
											pParam,
											NULL,
											0
											);

										ExFreePool( pParam );

										DoTraceEx(
											TRACE_LEVEL_INFORMATION,
											DC_SYSTEM,
											"CoCreateInstance:: Pid = %!HANDLE!, GUID = %!GUID!, Verdict = %d",
											PsGetCurrentProcessId(),
											&Guid,
											(ULONG) Verdict
											);
									}
								}

								ExFreePool( pLocalStackCopy );
							}

						}

					}

				}

			}	

			ExFreePool( pData );
		}
	}

	ExFreePool( pHeader );

	return Verdict;
}

#include <pshpack1.h>

typedef struct _LPC_DNS_DATA_W2K
{
	ULONG unknown1; // 0x00
	USHORT RpcOperationNumber; // 0x04
	USHORT unknown2; // 0x06
	ULONG unknown3[7]; // 0x08
	ULONG HostNameLength; // 0x24
	ULONG unknown4; // 0x28
	ULONG HostNameLength2; // 0x2C
	WCHAR HostName[ANYSIZE_ARRAY]; // 0x32
} LPC_DNS_DATA_W2K,*PLPC_DNS_DATA_W2K;

typedef struct _LPC_DNS_DATA2_W2K
{
	USHORT wType; // 0x00
	USHORT unknown1; // 0x02
	USHORT fOptions; // 0x04
	USHORT unknown2[5]; // 0x06
	//USHORT AlignmentToFour[1] OPTIONAL; // 0x08
} LPC_DNS_DATA2_W2K,*PLPC_DNS_DATA2_W2K;

typedef struct _LPC_DNS_DATA_WXP
{
	ULONG unknown1; // 0x00
	USHORT unknown2; // 0x04
	USHORT RpcOperationNumber; // 0x06
	ULONG unknown3[8]; // 0x08
	ULONG HostNameLength; // 0x28
	ULONG unknown4; // 0x2C
	ULONG HostNameLength2; // 0x30
	WCHAR HostName[ANYSIZE_ARRAY]; // 0x34
} LPC_DNS_DATA_WXP,*PLPC_DNS_DATA_WXP;

typedef struct _LPC_DNS_DATA2_WXP
{
	USHORT wType; // 0x00
	USHORT unknown1; // 0x02
	USHORT fOptions; // 0x04
	USHORT unknown2[5]; // 0x06
	//USHORT AlignmentToFour[1] OPTIONAL; // 0x08
} LPC_DNS_DATA2_WXP,*PLPC_DNS_DATA2_WXP;

#include <poppack.h>

#define LPC_MAX_DATA_SIZE_DNS			0x260
#define LPC_MIN_DATA_SIZE_DNS_W2K		( sizeof(LPC_DNS_DATA_W2K) + sizeof(LPC_DNS_DATA2_W2K) )
#define LPC_MIN_DATA_SIZE_DNS_WXP		( sizeof(LPC_DNS_DATA_WXP) + sizeof(LPC_DNS_DATA2_WXP) )

#define DNS_MAX_NAME_LEN				256
#define LPC_DNSQUERY_OPNUMBER_W2K		0x6
#define LPC_DNSQUERY_OPNUMBER_WXP		0x9

// DNS query to DNS cache service
tefVerdict
CommonLPCDNSRequest(
	__in PPORT_MESSAGE RequestMessage
	)
{
	tefVerdict Verdict = efVerdict_Default;
	PPORT_MESSAGE pHeader;
	ULONG DataSizeMin;

	pHeader = (PPORT_MESSAGE) Sys_CopyUserBuffer(
		RequestMessage,
		sizeof(PORT_MESSAGE)
		);
	if ( !pHeader )
		return efVerdict_Default;

	//right max sizes
	DataSizeMin = LPC_MIN_DATA_SIZE_DNS_WXP;
	if ( g_bIsW2K )
		DataSizeMin = LPC_MIN_DATA_SIZE_DNS_W2K;

	// a) the message must have some minimum size
	if ( (ULONG) pHeader->u1.s1.TotalLength >= PORT_MESSAGE_SIZE &&
		(ULONG) pHeader->u1.s1.DataLength >= DataSizeMin &&
		(ULONG) pHeader->u1.s1.DataLength <= LPC_MAX_DATA_SIZE_DNS
		)
	{
		PUCHAR pData;

		pData = Sys_CopyUserBuffer(
			(PUCHAR) RequestMessage + PORT_MESSAGE_SIZE,
			pHeader->u1.s1.DataLength
			);
		if ( pData )
		{
			BOOLEAN bMatch = FALSE;
			WCHAR HostName[DNS_MAX_NAME_LEN + 1];
			ULONG HostNameLen = 0;

			if ( g_bIsW2K )
			{
				// win2k
				PLPC_DNS_DATA_W2K pDNSData = (PLPC_DNS_DATA_W2K) pData;

				if ( LPC_DNSQUERY_OPNUMBER_W2K == pDNSData->RpcOperationNumber &&
					pDNSData->HostNameLength == pDNSData->HostNameLength2 &&
					pDNSData->HostNameLength < DNS_MAX_NAME_LEN 
					)
				{
					bMatch = TRUE;
					memcpy(
						HostName,
						pDNSData->HostName,
						sizeof(WCHAR) * pDNSData->HostNameLength
						);
					HostNameLen = pDNSData->HostNameLength;
				}
			}
			else
			{
				// winxp
				PLPC_DNS_DATA_WXP pDNSData = (PLPC_DNS_DATA_WXP) pData;

				if ( LPC_DNSQUERY_OPNUMBER_WXP == pDNSData->RpcOperationNumber &&
					pDNSData->HostNameLength == pDNSData->HostNameLength2 &&
					pDNSData->HostNameLength < DNS_MAX_NAME_LEN 
					)
				{
					bMatch = TRUE;
					memcpy(
						HostName,
						pDNSData->HostName,
						sizeof(WCHAR) * pDNSData->HostNameLength
						);
					HostNameLen = pDNSData->HostNameLength;
				}
			}

			if ( bMatch )
			{
				PFILTER_EVENT_PARAM pParam;

				HostName[HostNameLen] = 0;

				pParam = (PFILTER_EVENT_PARAM) ExAllocatePoolWithTag(
					PagedPool,
					0x200,
					'NDrg'
					);
				if ( pParam )
				{
					PSINGLE_PARAM pSingleParam;

					FILTER_PARAM_COMMONINIT(
						pParam,
						FLTTYPE_SYSTEM,
						0x21/*MJ_SYSTEM_DNSQUERY*/,
						0,
						PreProcessing,
						2
						);
					pSingleParam = (PSINGLE_PARAM) pParam->Params;

					SINGLE_PARAM_INIT_SID( pSingleParam );
					SINGLE_PARAM_SHIFT( pSingleParam );

					SINGLE_PARAM_INIT_NONE(
						pSingleParam,
						_PARAM_OBJECT_URL_W
						);
					pSingleParam->ParamSize = sizeof(WCHAR) * ( HostNameLen + 1 );
					memcpy(
						pSingleParam->ParamValue,
						HostName,
						pSingleParam->ParamSize
						);

					Verdict = FilterEventImp(
						pParam,
						NULL,
						0
						);

					ExFreePool( pParam );

					DoTraceEx(
						TRACE_LEVEL_INFORMATION,
						DC_SYSTEM,
						"DnsQuer:: Pid = %!HANDLE!, host = %S, Verdict = %d",
						PsGetCurrentProcessId(),
						HostName,
						Verdict
						);
				}
			}

			ExFreePool( pData );
		}
	}

	return Verdict;
}

NTSTATUS
NTAPI klhRequestWaitReplyPort (
	__in HANDLE PortHandle,
	__in PPORT_MESSAGE RequestMessage,
	__out PPORT_MESSAGE ReplyMessage
	)
{
	PVOID pData;
	BOOLEAN bCreated;
	ULONGLONG Key;
	tefVerdict Verdict = efVerdict_Default;

	// bypass kernel usage
	if ( KernelMode == ExGetPreviousMode() )
	{
		return klNtRequestWaitReplyPort(
			PortHandle,
			RequestMessage,
			ReplyMessage
			);
	}

	Key  = HANDLE_CACHE_KEY_FUNC( PsGetCurrentProcessId(), PortHandle );

	// check if PortHandle is "\RPC Control\epmapper" port
	pData = GenCacheGet(
		g_pRPCMapperPortsCache,
		Key,
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

	if (efIsDeny( Verdict ))
		return STATUS_ACCESS_DENIED;

	// check if PortHandle is "\RPC Control\DNSResolver" port
	pData = GenCacheGet(
		g_pDNSResolverPortCache,
		Key,
		FALSE,
		&bCreated
		);
	if ( pData )
	{
		// this is dns resolver port
		Verdict = CommonLPCDNSRequest(
			RequestMessage
			);
	}

	if (efIsDeny( Verdict ))
		return STATUS_ACCESS_DENIED;

	return klNtRequestWaitReplyPort(
		PortHandle,
		RequestMessage,
		ReplyMessage
		);
}
// -- LPC/COM stuff ---------------------------------------------------------------------------------

PWCHAR GetUserWideStrFromUnknown(
								 __in PVOID UnknownStr
								 )
{
	PCHAR cFirstTwoChars;
	PWCHAR wcUnicodeText = NULL;

	cFirstTwoChars = Sys_CopyUserBuffer(
		UnknownStr,
		2
		);
	if ( cFirstTwoChars )
	{
		if ( 0 != cFirstTwoChars[1] )
		{
			// probably ANSI string
			PCHAR cAnsiText;

			cAnsiText = GetUserStr(
				(PVOID) UnknownStr,
				0x1000
				);
			if ( cAnsiText )
			{
				ULONG cchStr;

				cchStr = strlen( cAnsiText );

				wcUnicodeText = (PWCHAR) ExAllocatePoolWithTag(
					PagedPool,
					sizeof(WCHAR) * (cchStr + 1),
					'SUrg'
					);
				if ( wcUnicodeText )
				{
					int iResult;

					iResult = mbtowc(
						wcUnicodeText,
						cAnsiText,
						cchStr
						);
					if ( 0 == iResult || -1 == iResult )
					{
						ExFreePool( wcUnicodeText );
						wcUnicodeText = NULL;
					}
					else
						wcUnicodeText[cchStr] = 0;
				}

				ExFreePool( cAnsiText );
			}
		}
		else
		{
			wcUnicodeText = GetUserWideStr(
				UnknownStr,
				0x2000
				);
		}

		ExFreePool( cFirstTwoChars );
	}

	return wcUnicodeText;
}

#define WM_SETTEXT                      0x000C

tefVerdict
PreInterProcMessage (
	__in_opt HANDLE hWnd,
	__in HANDLE TargetProcessID,
	__in HANDLE TargetThreadID,
	__in ULONG Msg,
	__in ULONG wParam,
	__in ULONG lParam
	)
{
	tefVerdict Verdict = efVerdict_Default;
	ULONG bInvisibleTarget;
	PFILTER_EVENT_PARAM	pParam;
	PWCHAR wcUnicodeText = NULL;

	// костыль на лочку при нескольких сессиях с запущенным ГУЕм
	if ( 0x400 <= Msg )
		return efVerdict_Default;

	if ( !TargetThreadID )
		return efVerdict_Default;

	bInvisibleTarget = IsInvisible( TargetThreadID, 0 ) ? 1L : 0L;

	if ( WM_SETTEXT == Msg && lParam )
		wcUnicodeText = GetUserWideStrFromUnknown( (PVOID) lParam );

	pParam = (PFILTER_EVENT_PARAM) ExAllocatePoolWithTag(
		PagedPool,
		0x4000,
		'PIrg'
		);
	if ( pParam )
	{
		PSINGLE_PARAM pSingleParam;

		FILTER_PARAM_COMMONINIT(
			pParam,
			FLTTYPE_SYSTEM,
			0x17/*MJ_SYSTEM_INTERPROC_MESSAGE*/,
			0,
			PreProcessing,
			( wcUnicodeText ? 1 : 0 ) + 8
			);
		pSingleParam = (PSINGLE_PARAM)pParam->Params;

		SINGLE_PARAM_INIT_SID( pSingleParam );
		SINGLE_PARAM_SHIFT( pSingleParam );

		SINGLE_PARAM_INIT_ULONG(
			pSingleParam,
			_PARAM_OBJECT_OBJECTHANDLE,
			(ULONG) hWnd
			);
		SINGLE_PARAM_SHIFT( pSingleParam );

		SINGLE_PARAM_INIT_ULONG(
			pSingleParam,
			_PARAM_OBJECT_DEST_ID,
			(ULONG) TargetProcessID
			);
		SINGLE_PARAM_SHIFT( pSingleParam );

		SINGLE_PARAM_INIT_ULONG(
			pSingleParam,
			_PARAM_OBJECT_CLIENTID1,
			(ULONG) TargetThreadID
			);
		SINGLE_PARAM_SHIFT( pSingleParam );

		SINGLE_PARAM_INIT_ULONG(
			pSingleParam,
			_PARAM_OBJECT_MSG_ID,
			Msg
			);
		SINGLE_PARAM_SHIFT( pSingleParam );

		SINGLE_PARAM_INIT_ULONG(
			pSingleParam,
			_PARAM_OBJECT_WPARAM,
			wParam
			);
		SINGLE_PARAM_SHIFT( pSingleParam );

		SINGLE_PARAM_INIT_ULONG(
			pSingleParam,
			_PARAM_OBJECT_LPARAM,
			lParam
			);
		SINGLE_PARAM_SHIFT( pSingleParam );

		SINGLE_PARAM_INIT_ULONG(
			pSingleParam,
			_PARAM_OBJECT_INVIS_TARGET,
			bInvisibleTarget
			);
		SINGLE_PARAM_SHIFT( pSingleParam );

		if ( wcUnicodeText )
		{
			SINGLE_PARAM_INIT_NONE(
				pSingleParam,
				_PARAM_OBJECT_URL_W
				);
			pSingleParam->ParamSize = sizeof(WCHAR) * ( wcslen( wcUnicodeText ) + 1 );
			memcpy(
				pSingleParam->ParamValue,
				wcUnicodeText,
				pSingleParam->ParamSize
				);
		}

		Verdict = FilterEventImp(
			pParam,
			NULL,
			0
			);

		ExFreePool( pParam );

		DoTraceEx(
			TRACE_LEVEL_INFORMATION,
			DC_SYSTEM,
			"interproc message: sPid = %!HANDLE!, hWnd = %!HANDLE!, tPid = %!HANDLE!, tTid = %!HANDLE!, Msg = 0x%04x, wParam = 0x%08x, lParam = 0x%08x, invtar = %!BOOLEAN!, text = %S, Verdict = %d",
			PsGetCurrentProcessId(),
			hWnd,
			TargetProcessID,
			TargetThreadID,
			Msg,
			wParam,
			lParam,
			(BOOLEAN) bInvisibleTarget,
			wcUnicodeText,
			Verdict
			);
	}

	if ( wcUnicodeText )
		ExFreePool( wcUnicodeText );

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

tefVerdict
PreDdeExecute (
	__in HANDLE hWnd,
	__in ULONG lParam
	)
{
	tefVerdict Verdict = efVerdict_Default;
	_HGLOBAL hDDECommand;
	PWCHAR wcDDECommand = NULL;
	HANDLE TargetPid;
	PFILTER_EVENT_PARAM	pParam;
	PSINGLE_PARAM pSingleParam;

	TargetPid = klNtUserQueryWindow(
		hWnd,
		WindowProcess
		);

	hDDECommand = (_HGLOBAL) lParam;
	if ( hDDECommand )
	{
		_HGLOBAL hDDECommand_Safe;

		hDDECommand_Safe = Sys_CopyUserBuffer(
			hDDECommand,
			sizeof(*hDDECommand)
			);
		if ( hDDECommand_Safe )
		{
			PVOID UnknowStr = hDDECommand_Safe->pMem;

			wcDDECommand = GetUserWideStrFromUnknown( UnknowStr );

			ExFreePool( hDDECommand_Safe );
		}
	}

	pParam = (PFILTER_EVENT_PARAM) ExAllocatePoolWithTag(
		PagedPool,
		0x4000,
		'EDrg'
		);
	if ( pParam )
	{
		FILTER_PARAM_COMMONINIT(
			pParam,
			FLTTYPE_SYSTEM,
			0x11/*MJ_SYSTEM_USER_POST_MESSAGE*/,
			0,
			PreProcessing,
			( wcDDECommand ? 1 : 0 ) + 3
			);
		pSingleParam = (PSINGLE_PARAM) pParam->Params;

		SINGLE_PARAM_INIT_SID( pSingleParam );
		SINGLE_PARAM_SHIFT( pSingleParam );

		SINGLE_PARAM_INIT_ULONG(pSingleParam,
			_PARAM_OBJECT_MSG_ID,
			WM_DDE_EXECUTE
			);
		SINGLE_PARAM_SHIFT( pSingleParam );

		SINGLE_PARAM_INIT_ULONG(
			pSingleParam,
			_PARAM_OBJECT_CLIENTID1,
			(ULONG) TargetPid
			);
		SINGLE_PARAM_SHIFT(pSingleParam);

		if ( wcDDECommand )
		{
			ULONG cchDDECommand;

			cchDDECommand = wcslen( wcDDECommand );

			SINGLE_PARAM_INIT_NONE(
				pSingleParam,
				_PARAM_OBJECT_URL_W
				);
			pSingleParam->ParamSize = sizeof(WCHAR) * ( cchDDECommand + 1 );
			memcpy(
				pSingleParam->ParamValue,
				wcDDECommand,
				pSingleParam->ParamSize
				);
		}

		Verdict = FilterEventImp(
			pParam,
			NULL,
			0
			);

		ExFreePool( pParam );
	}

	DoTraceEx(
		TRACE_LEVEL_INFORMATION,
		DC_SYSTEM,
		"DDE Execute:: Pid = %!HANDLE!, tPid = %!HANDLE!, command = %S, Verdict = %d",
		PsGetCurrentProcessId(),
		TargetPid,
		wcDDECommand,
		Verdict
		);

	if ( wcDDECommand )
		ExFreePool( wcDDECommand );

	return Verdict;

}

PWCHAR GetAtomName(
				   __in ATOM nAtom
				   )
{
	NTSTATUS status;
	SIZE_T RegionSize;
	PATOM_BASIC_INFORMATION pAtomInfo = NULL;
	PWCHAR wcAtomName = NULL;

	RegionSize = 0x1000;
	status = ZwAllocateVirtualMemory(
		NtCurrentProcess(),
		&pAtomInfo,
		0,
		&RegionSize,
		MEM_COMMIT,
		PAGE_READWRITE
		);
	if ( NT_SUCCESS( status ) )
	{
		status = NtQueryInformationAtom(
			nAtom,
			AtomBasicInformation,
			pAtomInfo,
			0x1000,
			NULL
			);
		if ( NT_SUCCESS( status ) )
		{
			wcAtomName = (PWCHAR) ExAllocatePoolWithTag(
				PagedPool,
				pAtomInfo->NameLength + sizeof(WCHAR),
				'TArg');
			if ( wcAtomName )
			{
				memcpy(
					wcAtomName,
					pAtomInfo->Name,
					pAtomInfo->NameLength
					);
				wcAtomName[pAtomInfo->NameLength / sizeof(WCHAR)] = 0;
			}
		}

		ZwFreeVirtualMemory(
			NtCurrentProcess(),
			&pAtomInfo,
			&RegionSize,
			MEM_RELEASE
			);
	}

	return wcAtomName;
}

tefVerdict
PreDdeInitiate (
	__in HANDLE hWnd,
	__in ULONG lParam
	)
{
	tefVerdict Verdict = efVerdict_Default;
	ATOM aApplication, aTopic;
	PWCHAR wcAppName = NULL;
	PWCHAR wcTopicName = NULL;
	HANDLE TargetPID;
	PFILTER_EVENT_PARAM pParam;

	aApplication = (USHORT) lParam;
	aTopic = (USHORT) ( lParam >> 16 );

	// certain application
	if ( aApplication )
		wcAppName = GetAtomName( aApplication );
	if ( aTopic )
		wcTopicName = GetAtomName( aTopic );
	TargetPID = klNtUserQueryWindow(
		hWnd,
		WindowProcess
		);


	pParam = (PFILTER_EVENT_PARAM) ExAllocatePoolWithTag(
		PagedPool,
		0x4000,
		tag_event
		);
	if ( pParam )
	{
		PSINGLE_PARAM pSingleParam;

		FILTER_PARAM_COMMONINIT(
			pParam,
			FLTTYPE_SYSTEM,
			0x11/*MJ_SYSTEM_USER_POST_MESSAGE*/,
			0,
			PreProcessing,
			( wcAppName ? 1 : 0 ) + ( wcTopicName ? 1 : 0 ) + 3
			);
		pSingleParam = (PSINGLE_PARAM) pParam->Params;

		// sid --------------------------------------------------------------------------------
		SINGLE_PARAM_INIT_SID( pSingleParam );
		SINGLE_PARAM_SHIFT( pSingleParam );
		// msg --------------------------------------------------------------------------------
		SINGLE_PARAM_INIT_ULONG(
			pSingleParam,
			_PARAM_OBJECT_MSG_ID,
			WM_DDE_INITIATE
			);
		SINGLE_PARAM_SHIFT( pSingleParam );
		// target PID--------------------------------------------------------------------------
		SINGLE_PARAM_INIT_ULONG(
			pSingleParam,
			_PARAM_OBJECT_CLIENTID1,
			(ULONG) TargetPID
			);
		SINGLE_PARAM_SHIFT( pSingleParam );
		// service name------------------------------------------------------------------------
		if ( wcAppName )
		{
			SINGLE_PARAM_INIT_NONE(
				pSingleParam,
				_PARAM_OBJECT_URL_W
				);
			pSingleParam->ParamSize = sizeof(WCHAR) * ( wcslen( wcAppName ) + 1 );
			memcpy(
				pSingleParam->ParamValue,
				wcAppName,
				pSingleParam->ParamSize
				);
			SINGLE_PARAM_SHIFT( pSingleParam );
		}
		// topic name--------------------------------------------------------------------------
		if (wcTopicName)
		{
			SINGLE_PARAM_INIT_NONE(
				pSingleParam,
				_PARAM_OBJECT_URL_PARAM_W
				);
			pSingleParam->ParamSize = sizeof(WCHAR) * ( wcslen( wcTopicName ) + 1 );
			memcpy(
				pSingleParam->ParamValue,
				wcTopicName,
				pSingleParam->ParamSize
				);
			SINGLE_PARAM_SHIFT( pSingleParam );
		}
		//-------------------------------------------------------------------------------------

		Verdict = FilterEventImp(
			pParam,
			NULL,
			0
			);

		ExFreePool( pParam );
	}

	DoTraceEx(
		TRACE_LEVEL_INFORMATION,
		DC_SYSTEM,
		"DDE Initiate:: Pid = %!HANDLE!, tPid = %!HANDLE!, service = %S, topic = %S, verdict = %d\n",
		PsGetCurrentProcessId(),
		TargetPID,
		wcAppName,
		wcTopicName,
		Verdict
		);

	if ( wcAppName )
		ExFreePool( wcAppName );
	if ( wcTopicName )
		ExFreePool( wcTopicName );

	return Verdict;
}
//- DDE stuff ----------------------------------------------------------------------

// обработчик на SendMessage/PostMessage
tefVerdict
PreSendPostMessage (
	__in HANDLE hWnd,
	__in ULONG Msg,
	__in ULONG wParam,
	__in ULONG lParam
	)
{
	HANDLE TargetProcessID;
	tefVerdict Verdict = efVerdict_Default;

	TargetProcessID = klNtUserQueryWindow( hWnd, WindowProcess );
	// если посылаем в чужой процесс..
	if ( TargetProcessID && PsGetCurrentProcessId() != TargetProcessID )
	{
		HANDLE TargetThreadID;

		TargetThreadID = klNtUserQueryWindow(
			hWnd,
			WindowThread
			);
		Verdict = PreInterProcMessage(
			hWnd,
			TargetProcessID,
			TargetThreadID,
			Msg,
			wParam,
			lParam
			);
	}

	if (efIsAllow( Verdict ))
	{
		switch (Msg)
		{
		case WM_DDE_EXECUTE:
			Verdict = PreDdeExecute(
				hWnd,
				lParam
				);
			break;
		case WM_DDE_INITIATE:
			Verdict = PreDdeInitiate(
				hWnd,
				lParam
				);
			break;
		}
	}

	return Verdict;
}

tefVerdict
PrePostThreadMessage (
	__in HANDLE ThreadID,
	__in ULONG Msg,
	__in ULONG wParam,
	__in ULONG lParam
	)
{
	tefVerdict Verdict = efVerdict_Default;
	HANDLE hTargetThread;
	OBJECT_ATTRIBUTES ThreadOA;
	CLIENT_ID CID;
	NTSTATUS status;
	PTHREAD_BASIC_INFORMATION pThreadInfo;
	SIZE_T RegionSize;

	ASSERT( klNtQueryInformationThread );

	InitializeObjectAttributes(
		&ThreadOA,
		NULL,
		0,
		NULL,
		NULL
		);
	CID.UniqueProcess = 0;
	CID.UniqueThread = (HANDLE)ThreadID;
	status = ZwOpenThread(
		&hTargetThread,
		THREAD_QUERY_INFORMATION,
		&ThreadOA,
		&CID
		);
	if ( !NT_SUCCESS( status ) )
	{
		DoTraceEx(
			TRACE_LEVEL_INFORMATION,
			DC_SYSTEM,
			"PrePostThreadMessage: failed to open thread Cid = %!HANDLE!, status = %!STATUS!",
			ThreadID,
			status
			);
		return efVerdict_Default;
	}

	// выделяем юзеровую память, потомушта PreviousMode будет == UserMode
	pThreadInfo = NULL;
	RegionSize = sizeof(THREAD_BASIC_INFORMATION);
	status = ZwAllocateVirtualMemory(
		NtCurrentProcess(),
		&pThreadInfo,
		0,
		&RegionSize,
		MEM_COMMIT,
		PAGE_READWRITE
		);
	if ( NT_SUCCESS( status ) )
	{
		// получаем ID процесса с этой ниткой..
		status = klNtQueryInformationThread(
			hTargetThread,
			ThreadBasicInformation,
			pThreadInfo,
			sizeof(THREAD_BASIC_INFORMATION),
			NULL
			);
		if ( NT_SUCCESS( status ) )
		{
			HANDLE TargetProcessID;

			TargetProcessID = pThreadInfo->ClientId.UniqueProcess;
			// если посылаем в чужой процесс..
			if ( TargetProcessID && PsGetCurrentProcessId() != TargetProcessID )
			{
				Verdict = PreInterProcMessage(
					NULL,
					TargetProcessID,
					ThreadID,
					Msg,
					wParam,
					lParam
					);
			}
		}
		else
		{
			DoTraceEx(
				TRACE_LEVEL_INFORMATION,
				DC_SYSTEM,
				"PrePostThreadMessage: failed to query thread information, Cid = %!HANDLE!, status = %!STATUS!",
				ThreadID,
				status
				);
		}

		RegionSize = 0;
		ZwFreeVirtualMemory(
			NtCurrentProcess(),
			&pThreadInfo,
			&RegionSize,
			MEM_RELEASE
			);
	}
	else
	{
		DoTraceEx(
			TRACE_LEVEL_INFORMATION,
			DC_SYSTEM,
			"PrePostThreadMessage: failed to allocate virtual memory for Pid = %!HANDLE!, status = %!STATUS!",
			PsGetCurrentProcessId(),
			status
			);
	}

	ZwClose(hTargetThread);

	return Verdict;
}

ULONG 
( NTAPI *klNtUserMessageCall )(
							   __in HANDLE hwnd,
							   __in ULONG msg,
							   __in ULONG wParam,
							   __in ULONG lParam,
							   __in PVOID xParam,
							   __in ULONG xpfnProc,
							   __in ULONG bAnsi
							   ) = NULL;
ULONG
( NTAPI *klNtUserSendNotifyMessage)(
									__in HANDLE hwnd,
									__in ULONG msg,
									__in ULONG wParam,
									__in_opt ULONG lParam
									) = NULL;
BOOLEAN
( NTAPI *klNtUserPostMessage )(
							   __in HANDLE hWnd,
							   __in ULONG Msg,
							   __in ULONG wParam,
							   __in ULONG lParam
							   ) = NULL;
ULONG 
( NTAPI *klNtUserPostThreadMessage)(
									__in HANDLE id,
									__in ULONG msg,
									__in ULONG wParam,
									__in ULONG lParam
									) = NULL;

ULONG 
NTAPI klhUserMessageCall (
	__in HANDLE hwnd,
	__in ULONG msg,
	__in ULONG wParam,
	__in ULONG lParam,
	__in PVOID xParam,
	__in ULONG xpfnProc,
	__in ULONG bAnsi
	)
{
	ULONG RetVal = 0;
	// вердикт на посылку межпроцессного сообщения на окно (send и post)
	tefVerdict Verdict = PreSendPostMessage( hwnd, msg, wParam, lParam );
	if (efIsDeny( Verdict ))
		return 0;

	return klNtUserMessageCall( hwnd, msg, wParam, lParam, xParam, xpfnProc, bAnsi );
}

ULONG
NTAPI
klhUserSendNotifyMessage
(
	__in HANDLE hwnd,
	__in ULONG msg,
	__in ULONG wParam,
	__in_opt ULONG lParam
	)
{
	ULONG RetVal = 0;
	// вердикт на посылку межпроцессного сообщения на окно (send и post)
	tefVerdict Verdict = PreSendPostMessage( hwnd, msg, wParam, lParam );
	if (efIsDeny( Verdict ))
		return 0;

	return klNtUserSendNotifyMessage( hwnd, msg, wParam, lParam );
}

BOOLEAN
NTAPI
klhUserPostMessage (
	__in HANDLE hWnd,
	__in ULONG Msg,
	__in ULONG wParam,
	__in ULONG lParam
	)
{
	tefVerdict Verdict = efVerdict_Default;

	// вердикт на межпроцессное сообщение
	Verdict = PreSendPostMessage(
		hWnd,
		Msg,
		wParam,
		lParam
		);
	if (efIsDeny( Verdict ))
		return FALSE;

	return klNtUserPostMessage( hWnd, Msg, wParam, lParam );
}

ULONG 
NTAPI
klhUserPostThreadMessage (
	__in HANDLE id,
	__in ULONG msg,
	__in ULONG wParam,
	__in ULONG lParam
	)
{
	tefVerdict Verdict = PrePostThreadMessage( id, msg, wParam, lParam );

	if (efIsAllow( Verdict ))
		return klNtUserPostThreadMessage( id, msg, wParam, lParam );

	return 0;
}

typedef enum _DEBUG_CONTROL_CODE {
	DebugGetTraceInformation = 1,
	DebugSetInternalBreakpoint,
	DebugSetSpecialCall,
	DebugClearSpecialCalls,
	DebugQuerySpecialCalls,
	DebugDbgBreakPoint
} DEBUG_CONTROL_CODE;


NTSTATUS
(NTAPI *klNtSystemDebugControl)(
	IN DEBUG_CONTROL_CODE ControlCode,
	IN PVOID InputBuffer OPTIONAL,
	IN ULONG InputBufferLength,
	OUT PVOID OutputBuffer OPTIONAL,
	IN ULONG OutputBufferLength,
	OUT PULONG ReturnLength OPTIONAL
	);

NTSTATUS
NTAPI
klhSystemDebugControl(
	IN DEBUG_CONTROL_CODE ControlCode,
	IN PVOID InputBuffer OPTIONAL,
	IN ULONG InputBufferLength,
	OUT PVOID OutputBuffer OPTIONAL,
	IN ULONG OutputBufferLength,
	OUT PULONG ReturnLength OPTIONAL
	)
{
	tefVerdict Verdict = efVerdict_Default;

	if (IsNeedFilterEvent(FLTTYPE_SYSTEM, 0x1c/*MJ_SYSTEM_DEBUGCONTROL*/, 0))
	{
		PFILTER_EVENT_PARAM	pParam = EvContext_SystemDebugControl( ControlCode );
		if (pParam)
		{
			Verdict = FilterEventImp( pParam, NULL, 0 );
			ExFreePool( pParam );
		}
	}

// 	DbPrint(DC_SYS, DL_IMPORTANT, ("process Cid=%d is trying to ZwSystemDebugControl: ControlCode=%d, Verdict=%d\n",
// 		PsGetCurrentProcessId(), ControlCode, Verdict));

	if ( efIsAllow(Verdict) )
		return klNtSystemDebugControl(
		ControlCode,
		InputBuffer,
		InputBufferLength,
		OutputBuffer,
		OutputBufferLength,
		ReturnLength
		);

	return STATUS_ACCESS_DENIED;
}

NTSTATUS
(NTAPI *klNtLoadDriver)(
	IN PUNICODE_STRING DriverServiceName
	);

NTSTATUS
NTAPI klhLoadDriver(
	IN PUNICODE_STRING DriverServiceName
)
{
	NTSTATUS StatusRet = STATUS_NOT_FOUND;
	tefVerdict Verdict = efVerdict_Default;

	__try
	{
		if (DriverServiceName && MmIsAddressValid(DriverServiceName) &&
			DriverServiceName->Buffer && MmIsAddressValid(DriverServiceName->Buffer) &&
			DriverServiceName->Length)
		{
			PWCHAR SafeServiceName;

			SafeServiceName = (PWCHAR) CopyUserBuffer( DriverServiceName->Buffer, DriverServiceName->Length, NULL );
			if ( SafeServiceName )
			{
				if (IsNeedFilterEvent(FLTTYPE_SYSTEM, 0x1a/*MJ_SYSTEM_LOADDRIVER*/, 0))
				{
					UNICODE_STRING ServiceName;
					PFILTER_EVENT_PARAM	pParam;
					
					RtlInitEmptyUnicodeString(
						&ServiceName,
						SafeServiceName,
						DriverServiceName->Length
						);
					ServiceName.Length = DriverServiceName->Length;
					
					
					pParam = EvContext_LoadDriver( &ServiceName );
					if (pParam)
					{
						Verdict = FilterEventImp( pParam, NULL, 0 );
						ExFreePool( pParam );
					}
				}

				ExFreePool( SafeServiceName );
			}


		}
	}
	__except(EXCEPTION_EXECUTE_HANDLER)
	{
	}

	if ( efIsAllow(Verdict) )
		StatusRet = klNtLoadDriver(DriverServiceName);

	return StatusRet;

}

#define OBJECT_TYPE_PROCESS		0x03
#define OBJECT_TYPE_FO			0x05
#define OBJECT_TYPE_THREAD		0x06
#define OBJECT_TYPE_REGKEY		0x32

typedef PVOID PSECURITY_DESCRIPTOR;

NTSTATUS
(NTAPI *klNtSetSecurityObject)(
	IN HANDLE Handle,
	IN SECURITY_INFORMATION SecurityInformation,
	IN PSECURITY_DESCRIPTOR SecurityDescriptor
);

NTSTATUS NTAPI klhSetSecurityObject(
	IN HANDLE Handle,
	IN SECURITY_INFORMATION SecurityInformation,
	IN PSECURITY_DESCRIPTOR SecurityDescriptor
)
{
	tefVerdict Verdict = efVerdict_Default;

	NTSTATUS ntStatus;
	PVOID pObject;

	UNREFERENCED_PARAMETER (SecurityDescriptor);
	UNREFERENCED_PARAMETER (SecurityInformation);

	ntStatus = ObReferenceObjectByHandle(Handle, 0, NULL, KernelMode, &pObject, NULL);
	if (NT_SUCCESS(ntStatus) && pObject)
	{
		UCHAR Type = ((DISPATCHER_HEADER *)pObject)->Type;

		//DbPrint(DC_SYS, DL_IMPORTANT, ("KLHookSetSecurityObject - type 0x%x\n", Type ));

		if (OBJECT_TYPE_REGKEY == Type
			|| OBJECT_TYPE_FO == Type)
		{
			POBJECT_NAME_INFORMATION pObjInfo;

			pObjInfo = (POBJECT_NAME_INFORMATION)ExAllocatePoolWithTag( PagedPool, 0x1000, tag_set_sec_obj );
			if (pObjInfo)
			{
				ULONG RetLen;

				ntStatus = ObQueryNameString(pObject, pObjInfo, 0x1000, &RetLen);
				if (NT_SUCCESS(ntStatus) && pObjInfo->Name.Length && pObjInfo->Name.Buffer)
				{
					ULONG Mj = 0; 

					if (OBJECT_TYPE_REGKEY == Type)
						Mj = 0x16 ; //MJ_SYSTEM_SETREGSECURITY
					else
						Mj = 0x1d; //MJ_SYSTEM_SETFOSECURITY

// 					DbPrint(DC_SYS, DL_IMPORTANT, ("KLHookSetSecurityObject - setting security on %S\n",
// 						pObjInfo->Name.Buffer));

					if (IsNeedFilterEvent(FLTTYPE_SYSTEM, Mj, 0))
					{
						PFILTER_EVENT_PARAM pParam;
						pParam = EvContext_SetSecurityObject( &pObjInfo->Name, Mj );
						if (pParam)
						{
							Verdict = FilterEventImp( pParam, NULL, 0 );
							ExFreePool( pParam );
						}
					}
				}

				ExFreePool( pObjInfo );
			}
		}

		ObDereferenceObject(pObject);
	}

	if ( efIsAllow(Verdict) )
		return klNtSetSecurityObject( Handle, SecurityInformation, SecurityDescriptor );

	//_dbg_break_;
	return STATUS_ACCESS_DENIED;
}
/*
NTSTATUS (__stdcall *klNtCreateSection)(
	OUT PHANDLE SectionHandle, 
	IN ACCESS_MASK DesiredAccess, 
	IN POBJECT_ATTRIBUTES ObjectAttributes OPTIONAL, 
	IN PLARGE_INTEGER MaximumSize OPTIONAL, 
	IN ULONG SectionPageProtection, 
	IN ULONG AllocationAttributes, 
	IN HANDLE FileHandle OPTIONAL
);

typedef struct _tSYS_CREATE_SECTION_CONTEXT
{
	PWCHAR m_pFileName;
	ACCESS_MASK m_DesiredAccess;
	ULONG m_ObjAttributes;
	ULONG m_SectionPageProtection;

	NTSTATUS m_Status;
} SYS_CREATE_SECTION_CONTEXT, *PSYS_CREATE_SECTION_CONTEXT;

#define _SYSQUERY_INFO_LEN	0x4000

typedef struct _tSYS_CACHEITEM
{
	ULONG m_Id;
	WCHAR m_Name[0];
}SYS_CACHEITEM, *PSYS_CACHEITEM;

NTSTATUS __stdcall
klhCreateSection (
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

	tefVerdict Verdict = efVerdict_Default;
	SYS_CREATE_SECTION_CONTEXT SectionContext;

	POBJECT_ATTRIBUTES ObjectAttributesTmp = NULL;

	SectionContext.m_pFileName = NULL;
	SectionContext.m_ObjAttributes = 0;
	SectionContext.m_SectionPageProtection = SectionPageProtection;

	//DbPrint(DC_SYS, DL_SPAM, ("CreateSection FileHandle %#x DesiredAccess %#x\n", FileHandle, DesiredAccess));

	ObjectAttributesTmp = CopyUserBuffer( ObjectAttributes, sizeof(OBJECT_ATTRIBUTES), NULL );
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
			pCacheItem = ExAllocatePoolWithTag(NonPagedPool,  _SYSQUERY_INFO_LEN, tag_crt_sect );
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
// 								DbPrint(DC_SYS, DL_NOTIFY, ("CreateSection FileHandle %#x '%S' DesiredAccess %#x, Attr %#x, Protection %#x\n", 
// 									FileHandle, pCacheItem->m_Name, DesiredAccess, AllocationAttributes, SectionPageProtection));
							}

							bNameResolved = TRUE;
						}
					}
				}

				if (bNameResolved == FALSE)
				{
					DbgBreakPoint();
					ExFreePool( pCacheItem );
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

		//Verdict = CreateSectionCallbackPre(&SectionContext);
		if (IsNeedFilterEvent(
		FLTTYPE_SYSTEM, 
		4,//MJ_SYSTEM_CREATE_SECTION, 
		0
		)
		)
		{
			PFILTER_EVENT_PARAM pParam;
			UNICODE_STRING FileName;
			RtlInitUnicodeString( &FileName, SectionContext.m_pFileName );
			pParam = EvContext_CreateSection(
				&FileName,
				SectionContext.m_DesiredAccess,
				SectionContext.m_ObjAttributes,
				SectionContext.m_SectionPageProtection );
			
			if (pParam)
			{
				Verdict = FilterEventImp( pParam, NULL, 0 );
				ExFreePool( pParam );
			}
		}
	}
	efIsDeny
	if ( efIsDeny(Verdict) )
		ntStatusRet = STATUS_ACCESS_DENIED;
	else
	{
		ntStatusRet = klNtCreateSection(SectionHandle, DesiredAccess, ObjectAttributes, MaximumSize, SectionPageProtection, AllocationAttributes, FileHandle);
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

		if (efIsDeny(Verdict))
		{
			DbgBreakPoint();
			ntStatusRet = STATUS_ACCESS_DENIED;
			ZwClose(*SectionHandle);
			*SectionHandle = NULL;
		}
	}

	if (pCacheItem != NULL)
		ExFreePool( pCacheItem );

	return ntStatusRet;
}
*/

NTSTATUS (__stdcall *klNtCreateSymbolicLinkObject)(
	OUT PHANDLE ObjectHandle, IN ACCESS_MASK DesiredAccess, 
	IN POBJECT_ATTRIBUTES ObjectAttributes, 
	IN PUNICODE_STRING SubstituteString
	);

NTSTATUS __stdcall
klhCreateSymbolicLinkObject (
	__out PHANDLE ObjectHandle,
	__in ACCESS_MASK DesiredAccess, 
	__in POBJECT_ATTRIBUTES ObjectAttributes, 
	__in PUNICODE_STRING SubstituteString
)
{
	NTSTATUS ntStatusRet;
	tefVerdict Verdict = efVerdict_Default;

	POBJECT_ATTRIBUTES ObjectAttributesTmp = CopyUserBuffer( ObjectAttributes, sizeof(OBJECT_ATTRIBUTES), NULL );

	if ( ObjectAttributesTmp )
	{
		if (ObjectAttributesTmp->Length == sizeof(OBJECT_ATTRIBUTES))
		{
			UNICODE_STRING uni;
			PUNICODE_STRING uni_p;
			uni.Buffer = L"\\device\\physicalmemory";
			uni.Length = sizeof(L"\\device\\physicalmemory") - sizeof(WCHAR);
			uni.MaximumLength = sizeof(L"\\device\\physicalmemory");

			uni_p = CopyUserBuffer( ObjectAttributesTmp->ObjectName, sizeof(UNICODE_STRING), NULL );
			if (uni_p)
			{
				uni_p->MaximumLength = uni_p->Length;
				uni_p->Buffer = CopyUserBuffer( uni_p->Buffer, uni_p->Length, NULL );
				if (uni_p->Buffer)
				{
					if (!RtlCompareUnicodeString(uni_p, &uni, TRUE))
					{
						if (IsNeedFilterEvent(FLTTYPE_SYSTEM, 0x13/*MJ_SYSTEM_ACCESS_PHYSICAL_MEM*/, 0))
						{
							PFILTER_EVENT_PARAM pParam;
							pParam = EvContext_CreateSymbolicLinkObject(
								DesiredAccess
								);
							
							if (pParam)
							{
								Verdict = FilterEventImp( pParam, NULL, 0 );
								ExFreePool( pParam );
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

	if (efIsDeny(Verdict))
		ntStatusRet = STATUS_ACCESS_DENIED;
	else
		ntStatusRet = klNtCreateSymbolicLinkObject(ObjectHandle, DesiredAccess, ObjectAttributes, SubstituteString);

	return ntStatusRet;
}

NTSTATUS (__stdcall *klNtSetSystemInformation)(
	IN ULONG	SystemInformationClass,
	IN PVOID	SystemInformation,
	IN ULONG	Length
	);

NTSTATUS __stdcall
klhSetSystemInformation (
	IN ULONG SystemInformationClass,
	IN PVOID SystemInformation,
	IN ULONG Length
)
{
	NTSTATUS ntStatusRet;
	tefVerdict Verdict = efVerdict_Default;

	if (SystemLoadAndCallImage == SystemInformationClass)
	{
		PUNICODE_STRING pModuleName = GetUserUniStr( (PUNICODE_STRING) SystemInformation );

		if (pModuleName)
		{
			PFILTER_EVENT_PARAM pParam;
			pParam = EvContext_SetSystemInformation(
												pModuleName
												);
			if (pParam)
			{
				Verdict = FilterEventImp( pParam, NULL, 0 );
				ExFreePool( pParam );
			}

			ExFreePool( pModuleName );	
		}
	}

	if (efIsDeny(Verdict))
		ntStatusRet = STATUS_ACCESS_DENIED;
	else
		ntStatusRet = klNtSetSystemInformation( SystemInformationClass, SystemInformation, Length );

	return ntStatusRet;
}


NTSTATUS (__stdcall *klNtSuspendThread)(
	IN PHANDLE ThreadHandle, 
	OUT PULONG SuspendCount OPTIONAL
	);

NTSTATUS klhSuspendThread(
	IN PHANDLE ThreadHandle, 
	OUT PULONG SuspendCount OPTIONAL
)
{
	if (ThreadHandle != 0)
	{
		NTSTATUS ntStatus;
		PKTHREAD pkTh = NULL;

		ntStatus = ObReferenceObjectByHandle(ThreadHandle, STANDARD_RIGHTS_REQUIRED, NULL, KernelMode, &pkTh, NULL);
		if (NT_SUCCESS(ntStatus))
		{
			SuspendSubClient(pkTh);
			ObDereferenceObject(pkTh);
		}
	}

	return klNtSuspendThread(ThreadHandle, SuspendCount);
}

NTSTATUS (__stdcall *klNtResumeThread)(
	IN PHANDLE ThreadHandle,
	OUT PULONG SuspendCount OPTIONAL
	);


NTSTATUS klhResumeThread(
	PHANDLE ThreadHandle, 
	PULONG SuspendCount
)
{
	if (ThreadHandle != 0)
	{
		NTSTATUS ntStatus;
		PKTHREAD pkTh = NULL;

		ntStatus = ObReferenceObjectByHandle(ThreadHandle, STANDARD_RIGHTS_REQUIRED, NULL, KernelMode, &pkTh, NULL);
		if (NT_SUCCESS(ntStatus))
		{
			ResumeSubClient( pkTh);
			ObDereferenceObject(pkTh);
		}
	}

	return klNtResumeThread(ThreadHandle, SuspendCount);
}


NTSTATUS (__stdcall *klNtDuplicateObject)(
	IN HANDLE SourceProcessHandle,IN HANDLE SourceHandle, 
	IN HANDLE TargetProcessHandle, OUT PHANDLE TargetHandle OPTIONAL, 
	IN ACCESS_MASK DesiredAccess, IN ULONG Attributes, IN ULONG Options
	);

#define PROCESS_PSEUDO_HANDLE	((HANDLE)-1)
#define THREAD_PSEUDO_HANDLE	((HANDLE)-2)

NTSTATUS
NTAPI
klhDuplicateObject(
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
	tefVerdict Verdict = efVerdict_Default;

// 	DbPrint(DC_SYS, DL_NOTIFY, ("Entered KLHookDuplicateObject, SourceProcessHandle=%0x, SourceHandle=%0x, TargetProcessHandle=%0x\n",
// 		SourceProcessHandle, SourceHandle, TargetHandle));

	if (KeGetCurrentIrql() != PASSIVE_LEVEL || !TargetHandle)
	{
		return klNtDuplicateObject(SourceProcessHandle, SourceHandle, TargetProcessHandle,
			TargetHandle, DesiredAccess, Attributes, Options);
	}

	//----------------------------------------------------------------------------------------------------

	DuplicateHandle_Status = klNtDuplicateObject(SourceProcessHandle, SourceHandle, TargetProcessHandle,
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
			//	DbPrint(DC_SYS, DL_NOTIFY, ("KLHookDuplicateObject:: ObReferenceObjectByHandle(...) failed - STATUS_INVALID_HANDLE\n"));
			}
			else
			{
			//	DbPrint(DC_SYS, DL_IMPORTANT, ("KLHookDuplicateObject:: ObReferenceObjectByHandle(...) failed, ntStatus = %0x\n", ntStatus));
			}

			Standard_Processing = TRUE;
			break;
		}

		//DbPrint(DC_SYS, DL_NOTIFY, ("KLHookDuplicateObject:: DISPATCHER_HEADER::Type==%d\n",((DISPATCHER_HEADER *)pObject)->Type));

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
// 		DbPrint(DC_SYS, DL_NOTIFY, ("Duplicate process handle: SrcPID=%d DstPID=%d ObjPID=%d\n",
// 			GetProcessIDByHandle(SourceProcessHandle, NULL),
// 			GetProcessIDByHandle(TargetProcessHandle, NULL),
// 			GetProcessIDByHandle(Internal_TargetHandle, NULL)));
	}

	if (IsNeedFilterEvent(FLTTYPE_SYSTEM, 0xf/*MJ_SYSTEM_DUPLICATE_OBJECT*/, 0))
	{
		PFILTER_EVENT_PARAM pParam;
		pParam = EvContext_DuplicateObject(
				GetProcessIDByHandle(SourceProcessHandle, NULL),	// source process id
				GetProcessIDByHandle(TargetProcessHandle, NULL),	// target process id
				(ULONG) SourceHandle,								// source handle
				OBJECT_TYPE_PROCESS,								// source handle type
				GetProcessIDByHandle(Internal_TargetHandle, NULL) 	 // source handle
				);

		if (pParam)
		{
			Verdict = FilterEventImp( pParam, NULL, 0 );
			ExFreePool( pParam );
		}
	}

	if (efIsAllow(Verdict))
		return DuplicateHandle_Status;

	ZwClose(Internal_TargetHandle);

	return STATUS_ACCESS_DENIED;
}

//- win32k hooks ------------------------------------------------------------------------------------
#endif

//+ ------------------------------------------------------------------------------------------
BOOLEAN gbProcessNotify = FALSE;
BOOLEAN gbThreadNotify = FALSE;
BOOLEAN gbLoadImageNotify = FALSE;

NTSTATUS
SetSystemNotifiers ()
{
	NTSTATUS status;
#ifndef _WIN64
	BOOLEAN bResult;
#endif

#ifndef _WIN64
	bResult = GenCacheInit(
		0x1000,
		sizeof(KEY_READER),
		NULL,
		NULL,
		&g_pKeyReaderCache
		);
	if ( !bResult )
	{
		DoTraceEx(
			TRACE_LEVEL_ERROR,
			DC_SYSTEM,
			"SetSystemNotifiers:: failed to init g_pKeyReaderCache"
			);

		return STATUS_INSUFFICIENT_RESOURCES;
	}

	bResult = GenCacheInit(
		0x1000,
		0,
		NULL,
		NULL,
		&g_pAttachedInputThreadCache
		);
	if ( !bResult )
	{
		DoTraceEx(
			TRACE_LEVEL_ERROR,
			DC_SYSTEM,
			"SetSystemNotifiers:: failed to init g_pAttachedInputThreadCache"
			);

		GenCacheDone( &g_pKeyReaderCache );

		return STATUS_INSUFFICIENT_RESOURCES;
	}

	bResult = VMemCacheInit(
		&g_pVMemCache
		);
	if ( !bResult )
	{
		DoTraceEx(
			TRACE_LEVEL_ERROR,
			DC_SYSTEM,
			"SetSystemNotifiers:: failed to init g_pVMemCache"
			);

		GenCacheDone( &g_pAttachedInputThreadCache );
		GenCacheDone( &g_pKeyReaderCache );

		return STATUS_INSUFFICIENT_RESOURCES;

	}

	bResult = GenCacheInit(
		0x1000,
		0,
		NULL,
		NULL,
		&g_pRPCMapperPortsCache
		);
	if ( !bResult )
	{
		DoTraceEx(
			TRACE_LEVEL_ERROR,
			DC_SYSTEM,
			"SetSystemNotifiers:: failed to init g_pRPCMapperPortsCache"
			);

		VMemCacheDone( &g_pVMemCache );
		GenCacheDone( &g_pAttachedInputThreadCache );
		GenCacheDone( &g_pKeyReaderCache );

		return STATUS_INSUFFICIENT_RESOURCES;
	}

	bResult = GenCacheInit(
		0x1000,
		0,
		NULL,
		NULL,
		&g_pDNSResolverPortCache
		);
	if ( !bResult )
	{
		DoTraceEx(
			TRACE_LEVEL_ERROR,
			DC_SYSTEM,
			"SetSystemNotifiers:: failed to init g_pDNSResolverPortCache"
			);

		GenCacheDone( &g_pRPCMapperPortsCache );
		VMemCacheDone( &g_pVMemCache );
		GenCacheDone( &g_pAttachedInputThreadCache );
		GenCacheDone( &g_pKeyReaderCache );

		return STATUS_INSUFFICIENT_RESOURCES;
	}
	
	//g_pProtectedStorageHANDLECache
	bResult = GenCacheInit(
		0x1000,
		0,
		NULL,
		NULL,
		&g_pProtectedStorageHANDLECache
		);
	if ( !bResult )
	{
		DoTraceEx(
			TRACE_LEVEL_ERROR,
			DC_SYSTEM,
			"SetSystemNotifiers:: failed to init g_pProtectedStorageHANDLECache"
			);

		GenCacheDone( &g_pRPCMapperPortsCache );
		VMemCacheDone( &g_pVMemCache );
		GenCacheDone( &g_pAttachedInputThreadCache );
		GenCacheDone( &g_pKeyReaderCache );
		GenCacheDone( &g_pDNSResolverPortCache );

		return STATUS_INSUFFICIENT_RESOURCES;
	}
#endif

	status = PsSetCreateProcessNotifyRoutine(ProcessCrNotify, FALSE);
	if(NT_SUCCESS(status))
		gbProcessNotify = TRUE;

	if(NT_SUCCESS(status))
	{
		status = PsSetCreateThreadNotifyRoutine(ThreadCrNotify);
		if(NT_SUCCESS(status))
			gbThreadNotify = TRUE;
	}

	if(NT_SUCCESS(status))
	{
		status = PsSetLoadImageNotifyRoutine(LoadImageNotify);
		if(NT_SUCCESS(status))
			gbLoadImageNotify = TRUE;
	}

	if (NT_SUCCESS( status ))
	{
#ifndef _WIN64
		HookNtFunc( (ULONG*) &klNtOpenProcess, (ULONG) klhOpenProcess, "NtOpenProcess" );
		HookNtFunc( (ULONG*) &klNtTerminateProcess, (ULONG) klhTerminateProcess, "NtTerminateProcess");
//		HookNtFunc((ULONG*) &klNtCreateProcess, (ULONG) klhCreateProcess, "NtCreateProcess");
//		HookNtFunc((ULONG*) &klNtCreateProcessEx, (ULONG) klhCreateProcessEx, "NtCreateProcessEx");
		HookNtFunc((ULONG*) &klNtWriteVirtualMemory, (ULONG) klhWriteVirtualMemory, "NtWriteVirtualMemory");
		HookNtFunc((ULONG*) &klCreateThread, (ULONG) klhCreateThread, "NtCreateThread");

		LinkKrnlFunc( (ULONG*) &_pfPsGetThreadId, "PsGetThreadId");
		LinkKrnlFunc( (ULONG*) &_pfPsGetThreadProcessId, "PsGetThreadProcessId");

		if (_pfPsGetThreadId && _pfPsGetThreadProcessId)
			HookNtFunc((ULONG*) &klSetContextThread, (ULONG) klhSetContextThread, "NtSetContextThread");
				
		HookNtFunc( (ULONG*) &klNtConnectPort, (ULONG) klhConnectPort, "NtConnectPort");
		HookNtFunc( (ULONG*) &klNtRequestWaitReplyPort, (ULONG) klhRequestWaitReplyPort, "NtRequestWaitReplyPort");
		
		HookNtFunc( (ULONG*) &klNtSecureConnectPort, (ULONG) klhSecureConnectPort, "NtSecureConnectPort");
		HookNtFunc( (ULONG*) &klNtOpenFile, (ULONG) klhOpenFile, "NtOpenFile");
		HookNtFunc( (ULONG*) &klNtCreateFile, (ULONG) klhCreateFile, "NtCreateFile");
		HookNtFunc( (ULONG*) &klNtFsControlFile, (ULONG) klhFsControlFile, "NtFsControlFile");

		klNtQueryInformationThread = GetNtFunc( "NtQueryInformationThread" );

		if ( g_bIsWXP || g_bIsW2003 )
		{
			HookCheckLock();
		}
		
		HookNtFunc((ULONG *)&klNtSystemDebugControl, (ULONG)klhSystemDebugControl, "NtSystemDebugControl");
		HookNtFunc((ULONG *)&klNtLoadDriver, (ULONG)klhLoadDriver, "NtLoadDriver");
		HookNtFunc((ULONG *)&klNtSetSecurityObject, (ULONG)klhSetSecurityObject, "NtSetSecurityObject");

		HookNtFunc( (ULONG*) &klNtClose, (ULONG) klhClose, "NtClose");
		//HookNtFunc((ULONG*) &klNtCreateSection, (ULONG)klhCreateSection, "NtCreateSection");
		HookNtFunc((ULONG*) &klNtCreateSymbolicLinkObject, (ULONG)klhCreateSymbolicLinkObject, "NtCreateSymbolicLinkObject");
		HookNtFunc((ULONG*) &klNtSetSystemInformation, (ULONG)klhSetSystemInformation, "NtSetSystemInformation");

		if (HookNtFunc((ULONG*) &klNtResumeThread, (ULONG)klhResumeThread, "NtResumeThread"))
			HookNtFunc((ULONG*) &klNtSuspendThread, (ULONG)klhSuspendThread, "NtSuspendThread");

		HookNtFunc((ULONG*)&klNtDuplicateObject,(ULONG)klhDuplicateObject,"NtDuplicateObject");


#endif
	}

#ifndef _WIN64
	TryInitShadowTablePtr();
#endif

	return status;
}

VOID
RemoveSystemNotifiers ()
{
//	DbPrint(DC_SYS, DL_IMPORTANT, ("RemoveSystemNotifiers\n"));
#ifdef WIN2K
	#pragma message ("----------------------- w2k compatibility: no PsRemoveLoadImageNotifyRoutine")
	#pragma message ("----------------------- w2k compatibility: no PsRemoveCreateThreadNotifyRoutine")
	#pragma message ("----------------------- w2k compatibility: no PsSetCreateProcessNotifyRoutine (remove)")
#else
	if (gbLoadImageNotify)
		PsRemoveLoadImageNotifyRoutine( LoadImageNotify );

	if (gbThreadNotify)
		PsRemoveCreateThreadNotifyRoutine( ThreadCrNotify );

	if (gbProcessNotify)
		PsSetCreateProcessNotifyRoutine( ProcessCrNotify, TRUE );

#endif // WIN2K

#ifndef _WIN64
	if ( g_pKeyReaderCache )
		GenCacheDone( &g_pKeyReaderCache );
	if ( g_pAttachedInputThreadCache )
		GenCacheDone( &g_pAttachedInputThreadCache );
	if ( g_pVMemCache )
		VMemCacheDone( &g_pVMemCache );
	if ( g_pRPCMapperPortsCache )
		GenCacheDone( &g_pRPCMapperPortsCache );
	if ( g_pDNSResolverPortCache )
		GenCacheDone( &g_pDNSResolverPortCache );
	if ( g_pProtectedStorageHANDLECache )
		GenCacheDone( &g_pProtectedStorageHANDLECache );
#endif
}

NTSTATUS
Sys_GetFB2ByFO (
	PFILE_OBJECT pFileObject,
	PFIDBOX2_REQUEST_DATA pFB2
	)
{
	NTSTATUS status;
	md5_byte_t digest[16];
				
	RtlZeroMemory( pFB2, sizeof(FIDBOX2_REQUEST_DATA) );

	status = Sys_CalcHash( pFileObject, digest, sizeof(digest) );

	if (NT_SUCCESS( status ))
		memcpy( pFB2, digest, sizeof(digest) );

	return status;
}

NTSTATUS
DoSysRequest (
	IN PEXTERNAL_DRV_REQUEST pInRequest,
	OUT PVOID pOutBuffer, 
	IN ULONG OutBufferSize,
	OUT PULONG pRetSize
	)
{
	NTSTATUS status = STATUS_NOT_SUPPORTED;

	*pRetSize = 0;

	switch (pInRequest->m_IOCTL)
	{
	case _AVPG_IOCTL_FIDBOX_SET:

//		DoTraceEx( TRACE_LEVEL_INFORMATION, DC_SYSTEM, "fidbox_set" );

		if (pInRequest->m_BufferSize > sizeof(FIDBOX_REQUEST_DATA_EX))
		{
			ULONG ReqSize = pInRequest->m_BufferSize - sizeof(FIDBOX_REQUEST_DATA_EX);
			if (ReqSize > 0x1000)
				status = STATUS_NOT_SUPPORTED;
			else
			{
				PFIDBOX_REQUEST_DATA_EX pFidData = (PFIDBOX_REQUEST_DATA_EX) pInRequest->m_Buffer;

				status = FidBox_SetData( pFidData, ReqSize, FALSE, FALSE );
			}
		}
		break;

	case _AVPG_IOCTL_FIDBOX_DISABLE:
		GlobalFidBoxDone( TRUE );
		status = STATUS_SUCCESS;

		break;

	case _AVPG_IOCTL_FIDBOX_GET:
//		DoTraceEx( TRACE_LEVEL_INFORMATION, DC_SYSTEM, "fidbox_get" );

		if (pInRequest->m_BufferSize >= sizeof(FIDBOX_REQUEST_GET)
			&& OutBufferSize >= sizeof(FIDBOX_REQUEST_DATA))
		{
			ULONG ReqSize;
			PFIDBOX_REQUEST_GET pFidGet = (PFIDBOX_REQUEST_GET) pInRequest->m_Buffer;
			PFIDBOX_REQUEST_DATA_EX pFidData = (PFIDBOX_REQUEST_DATA_EX) pOutBuffer;

			ReqSize = OutBufferSize - sizeof(FIDBOX_REQUEST_DATA_EX);
			status = FidBox_GetData(pFidGet, pFidData, ReqSize, pRetSize);
			
			if (!NT_SUCCESS(status))
				*pRetSize = 0;
		}
		break;
		
// -----------------------------------------------------------------------------------------		
	case _AVPG_IOCTL_STAT_DISK:
		DoTraceEx( TRACE_LEVEL_INFORMATION, DC_SYSTEM, "request disk_stat not supported" );

		break;

	case _AVPG_IOCTL_GET_APP_HASH:
		status = STATUS_INVALID_PARAMETER;
		if (pInRequest->m_BufferSize >= sizeof(FIDBOX2_REQUEST_GET) &&
			OutBufferSize >= sizeof(FIDBOX2_REQUEST_DATA))
		{
			PFIDBOX2_REQUEST_GET pFidGet = (PFIDBOX2_REQUEST_GET) pInRequest->m_Buffer;

			md5_byte_t digest[16];

			status = Sys_QueryProcessHash( UlongToHandle( pFidGet->m_hFile ), digest, sizeof(digest) );
			if (NT_SUCCESS( status ))
			{
				PFIDBOX2_REQUEST_DATA pFidData = (PFIDBOX2_REQUEST_DATA) pOutBuffer;

				RtlZeroMemory( pFidData, sizeof(FIDBOX2_REQUEST_DATA) );
				memcpy( pFidData, digest, sizeof(digest) );
				*pRetSize = sizeof(FIDBOX2_REQUEST_DATA);
			}
		}
		break;

	case _AVPG_IOCTL_FIDBOX2_GET:
		DoTraceEx( TRACE_LEVEL_INFORMATION, DC_SYSTEM, "get app hash..." );
		{
			status = STATUS_INVALID_PARAMETER;
			
			if (pInRequest->m_BufferSize >= sizeof(FIDBOX2_REQUEST_GET) &&
				OutBufferSize >= sizeof(FIDBOX2_REQUEST_DATA))
			{
				PFIDBOX2_REQUEST_GET pFidGet = (PFIDBOX2_REQUEST_GET) pInRequest->m_Buffer;
				HANDLE hFile = UlongToHandle( pFidGet->m_hFile );

				if (pFidGet->m_hFile && (-1 != pFidGet->m_hFile))
				{
					PFILE_OBJECT pFileObject = NULL;
					status = ObReferenceObjectByHandle (
						   hFile,
						   0,
						   *IoFileObjectType,
						   UserMode,
						   &pFileObject,
						   NULL
						   );

					if (NT_SUCCESS( status ))
					{
						PFIDBOX2_REQUEST_DATA pFidData = (PFIDBOX2_REQUEST_DATA) pOutBuffer;

						status = Sys_GetFB2ByFO( pFileObject, pFidData );
						ObDereferenceObject( pFileObject );

						if (NT_SUCCESS( status ))
							*pRetSize = sizeof(FIDBOX2_REQUEST_DATA);
					}
				}
			}
		}
		break;

	case _AVPG_IOCTL_GET_PREFETCHINFO:
		DoTraceEx( TRACE_LEVEL_INFORMATION, DC_SYSTEM, "request get prefetch not supported" );
	
		break;

	case _AVPG_IOCTL_KEYLOGGERS_CHECK:
		DoTraceEx( TRACE_LEVEL_INFORMATION, DC_SYSTEM, "request  keyloggers check not supported" );

		break;

	case _AVPG_IOCTL_CHECK_ACTIVE_SECT:
		DoTraceEx( TRACE_LEVEL_INFORMATION, DC_SYSTEM, "request  check active not supported" );

		break;

	case _AVPG_IOCTL_GET_APP_PATH:
		DoTraceEx( TRACE_LEVEL_INFORMATION, DC_SYSTEM, "request  getapppath not supported" );

		break;

	default:
		DoTraceEx( TRACE_LEVEL_INFORMATION, DC_SYSTEM, "request 0x%x - not supported",
			pInRequest->m_IOCTL );

		_dbg_break_;

		break;
	}

	return status;
}

VOID
PutInfoItem (
	PMKLIF_INFO_ITEM *ppItem,
	MKLIF_INFO_TAGS Tag,
	PVOID pData,
	ULONG DataLen
	)
{
	PMKLIF_INFO_ITEM pItem = *ppItem;
	pItem->m_Tag = Tag;
	
	if (!pData)
		DataLen = 0;

	pItem->m_ValueSize = DataLen;
	
	if (DataLen)
		memcpy( pItem->m_Value, pData, DataLen );

	(CHAR*) pItem = ((CHAR*) pItem) + MKLIF_INFO_ITEM_SIZE + DataLen;

	*ppItem = pItem;
}

VOID
PutInfoItemZ (
	PMKLIF_INFO_ITEM *ppItem,
	MKLIF_INFO_TAGS Tag,
	PVOID pData,
	ULONG DataLen
	)
{
	PMKLIF_INFO_ITEM pItem = *ppItem;
	ULONG zlen = sizeof(L"");
	pItem->m_Tag = Tag;

	if (DataLen)
		memcpy( pItem->m_Value, pData, DataLen );
	memcpy( ((PCHAR)pItem->m_Value) + DataLen, L"", zlen );
	DataLen += zlen;
	
	pItem->m_ValueSize = DataLen;

	(CHAR*) pItem = ((CHAR*) pItem) + MKLIF_INFO_ITEM_SIZE + DataLen;

	*ppItem = pItem;
}

NTSTATUS
Sys_GetProcessesInfo (
	PVOID* ppProcesInfo,
	ULONG* pProcesInfoLen
	)
{
	ULONG size = 0;

	AcquireResourceExclusive( &gProcessInfoListLock );

	if (!_impIsListEmpty( &gProcessInfoList ))
	{
		PPROCESS_INFO pProcessInfo;
		PIMPLIST_ENTRY Flink;
		
		Flink = gProcessInfoList.Flink;

		while(Flink != &gProcessInfoList)
		{
			pProcessInfo = CONTAINING_RECORD(Flink, PROCESS_INFO, m_List);
			Flink = Flink->Flink;

			if (pProcessInfo->m_bActive)
			{
				size += MKLIF_INFO_ITEM_SIZE + sizeof(pProcessInfo->m_ProcessId);
				size += MKLIF_INFO_ITEM_SIZE + sizeof(pProcessInfo->m_ParentProcessId);
				size += MKLIF_INFO_ITEM_SIZE + pProcessInfo->m_PathLen;
				
				if (pProcessInfo->m_CmdLineLen)
					size += MKLIF_INFO_ITEM_SIZE + pProcessInfo->m_CmdLineLen;
				
				if (pProcessInfo->m_CurrDirLen)
					size += MKLIF_INFO_ITEM_SIZE + pProcessInfo->m_CurrDirLen;

				size += MKLIF_INFO_ITEM_SIZE + sizeof(pProcessInfo->m_StartTime);
			}
		}

		if (size)
		{
			size += MKLIF_INFO_ITEM_SIZE; // end tag

			*pProcesInfoLen = size;
			*ppProcesInfo = ExAllocatePoolWithTag( PagedPool, size, tag_proc_item );
			if (*ppProcesInfo)
			{
				PMKLIF_INFO_ITEM pItem = (PMKLIF_INFO_ITEM) *ppProcesInfo;
				Flink = gProcessInfoList.Flink;

				while(Flink != &gProcessInfoList)
				{
					pProcessInfo = CONTAINING_RECORD(Flink, PROCESS_INFO, m_List);
					Flink = Flink->Flink;

					if (pProcessInfo->m_bActive)
					{
						PutInfoItem (
							&pItem,
							mkpt_process_id,
							&pProcessInfo->m_ProcessId,
							sizeof(pProcessInfo->m_ProcessId)
							);

						PutInfoItem (
							&pItem,
							mkpt_parent_process_id,
							&pProcessInfo->m_ParentProcessId,
							sizeof(pProcessInfo->m_ParentProcessId)
							);

						PutInfoItem (
							&pItem,
							mkpt_filename,
							pProcessInfo->m_pwchPath,
							pProcessInfo->m_PathLen
							);

						if (pProcessInfo->m_CmdLineLen)
						{
							PutInfoItem (
								&pItem,
								mkpt_cmd_line,
								pProcessInfo->m_pwchCmdLine,
								pProcessInfo->m_CmdLineLen
								);
						}
						
						if (pProcessInfo->m_CurrDirLen)
						{
							PutInfoItem (
								&pItem,
								mkpt_curr_dir,
								pProcessInfo->m_pwchCurrDir,
								pProcessInfo->m_CurrDirLen
								);
						}

						PutInfoItem (
							&pItem,
							mkpt_start_time,
							&pProcessInfo->m_StartTime,
							sizeof(pProcessInfo->m_StartTime)
							);
					}
				}

				PutInfoItem (
					&pItem,
					mkpt_end,
					NULL,
					0
					);

				ReleaseResource( &gProcessInfoListLock );
				return STATUS_SUCCESS;
			}
		}
	}

	ReleaseResource( &gProcessInfoListLock );
	return STATUS_NOT_SUPPORTED;
}

NTSTATUS
QueryModulesInfo (
	PEB_LDR_DATA* pLdrData,
	PVOID* ppModulesInfo,
	PULONG pModulesInfoLen
	)
{
	NTSTATUS status = STATUS_INVALID_PARAMETER;

	PVOID pModulesInfo = NULL;
	ULONG ModulesInfoLen = 0;

	*pModulesInfoLen = 0;
	*ppModulesInfo = NULL;

	if (!pLdrData)
		return STATUS_INVALID_PARAMETER;

	try {
		if (!_impIsListEmpty( &pLdrData->InMemoryOrderModuleList ))
		{
			PLDR_DATA_TABLE_ENTRY pLdrEntry;
			PLIST_ENTRY Flink = pLdrData->InMemoryOrderModuleList.Flink;

			while(Flink != &pLdrData->InMemoryOrderModuleList)
			{
				pLdrEntry = CONTAINING_RECORD(Flink, LDR_DATA_TABLE_ENTRY, m_InMemoryOrderLinks);

				ModulesInfoLen += MKLIF_INFO_ITEM_SIZE + sizeof(pLdrEntry->m_DllBase);
				ModulesInfoLen += MKLIF_INFO_ITEM_SIZE + sizeof(pLdrEntry->m_SizeOfImage);
				ModulesInfoLen += MKLIF_INFO_ITEM_SIZE + sizeof(pLdrEntry->m_EntryPoint);

				ModulesInfoLen += MKLIF_INFO_ITEM_SIZE + pLdrEntry->m_FullDllName.Length + sizeof(WCHAR);
				
				Flink = Flink->Flink;
			}

			if (ModulesInfoLen)
			{
				ModulesInfoLen += MKLIF_INFO_ITEM_SIZE; // end tag
				ModulesInfoLen += 0x1000;
				pModulesInfo = ExAllocatePoolWithTag( PagedPool, ModulesInfoLen + 0x1000, tag_modinfo );
			}

			if (pModulesInfo)
			{
				PMKLIF_INFO_ITEM pItem = (PMKLIF_INFO_ITEM) pModulesInfo;
				ULONG RealSize = 0;

				Flink = pLdrData->InMemoryOrderModuleList.Flink;

				while(Flink != &pLdrData->InMemoryOrderModuleList)
				{
					pLdrEntry = CONTAINING_RECORD(Flink, LDR_DATA_TABLE_ENTRY, m_InMemoryOrderLinks);

					RealSize += MKLIF_INFO_ITEM_SIZE + sizeof(pLdrEntry->m_DllBase);
					RealSize += MKLIF_INFO_ITEM_SIZE + sizeof(pLdrEntry->m_SizeOfImage);
					RealSize += MKLIF_INFO_ITEM_SIZE + sizeof(pLdrEntry->m_EntryPoint);

					RealSize += MKLIF_INFO_ITEM_SIZE + pLdrEntry->m_FullDllName.Length + sizeof(WCHAR);
					
					if (RealSize > ModulesInfoLen)
					{
						_dbg_break_;
						break;
					}

					PutInfoItemZ (
						&pItem,
						mkpt_filename,
						pLdrEntry->m_FullDllName.Buffer,
						pLdrEntry->m_FullDllName.Length
						);

					PutInfoItem (
						&pItem,
						mkpt_base_ptr,
						&pLdrEntry->m_DllBase,
						sizeof(pLdrEntry->m_DllBase)
						);

					PutInfoItem (
						&pItem,
						mkpt_size,
						&pLdrEntry->m_SizeOfImage,
						sizeof(pLdrEntry->m_SizeOfImage)
						);

					PutInfoItem (
						&pItem,
						mkpt_entry_ptr,
						&pLdrEntry->m_EntryPoint,
						sizeof(pLdrEntry->m_EntryPoint)
						);

					Flink = Flink->Flink;
				}

				RealSize += MKLIF_INFO_ITEM_SIZE; // end tag

				PutInfoItem (
					&pItem,
					mkpt_end,
					NULL,
					0
					);

				
				ModulesInfoLen = RealSize;
			}
		}
	} except( CheckException() ) {
		if (pModulesInfo)
		{
			ExFreePool( pModulesInfo );
			pModulesInfo = NULL;
			ModulesInfoLen = 0;
		}
	}

	if (pModulesInfo)
	{
		*ppModulesInfo = pModulesInfo;
		*pModulesInfoLen = ModulesInfoLen;
		status = STATUS_SUCCESS;
	}

	return status;
}

NTSTATUS
Sys_GetModulesInfo (
	HANDLE ProcessId,
	PVOID* ppModulesInfo,
	ULONG* pModulesInfoLen
	)
{
	NTSTATUS status = STATUS_INVALID_PARAMETER;
	PEPROCESS pProcess = NULL;

	if (!ProcessId)
		return STATUS_INVALID_PARAMETER;

	status = PsLookupProcessByProcessId( ProcessId, &pProcess );
	if (NT_SUCCESS( status ))
	{
		HANDLE hProcess = NULL;

		CLIENT_ID			Cid;
		OBJECT_ATTRIBUTES   ObjectAttributes;

		Cid.UniqueProcess = ProcessId;
		Cid.UniqueThread = 0;

		InitializeObjectAttributes(&ObjectAttributes, NULL, OBJ_KERNEL_HANDLE, NULL, NULL);

		status = ZwOpenProcess( &hProcess, 0, &ObjectAttributes, &Cid );
		
		if (NT_SUCCESS( status ) && hProcess)
		{
			PROCESS_BASIC_INFORMATION	ProcInfo;
			status = ZwQueryInformationProcess (
				hProcess,
				ProcessBasicInformation,
				&ProcInfo,
				sizeof(ProcInfo),
				NULL
				);

			if (NT_SUCCESS( status ) && ProcInfo.PebBaseAddress)
			{
				KAPC_STATE  ApcState;

				PEB_LDR_DATA* pLdrData = NULL;
				KeStackAttachProcess( pProcess, &ApcState );

				try {
					pLdrData = *(PEB_LDR_DATA**) ((PCHAR) ProcInfo.PebBaseAddress + _peb_process_ldr_offset);
				} except( CheckException() ) {
					pLdrData = NULL;
				}

				status = QueryModulesInfo( pLdrData, ppModulesInfo, pModulesInfoLen );

				KeUnstackDetachProcess( &ApcState );
			}
			else
			{
				_dbg_break_;
				status = STATUS_NOT_SUPPORTED;
			}

			ZwClose( hProcess );
		}

		ObDereferenceObject( pProcess ); 
	}

	return status;
}

NTSTATUS
Sys_ReadProcessMemory (
	HANDLE ProcessId,
	LARGE_INTEGER Offset,
	PVOID pBuffer,
	ULONG BufferSize,
	PULONG pBytesRead
	)
{
	NTSTATUS status = STATUS_INVALID_PARAMETER;
	PEPROCESS pProcess = NULL;

	PCHAR Ptr = (PCHAR) Offset.QuadPart;

	if (!ProcessId)
		return STATUS_INVALID_PARAMETER;

	if (pBytesRead)
		*pBytesRead = 0;

	status = PsLookupProcessByProcessId( ProcessId, &pProcess );
	if (NT_SUCCESS( status ))
	{
		KAPC_STATE  ApcState;

		KeStackAttachProcess( pProcess, &ApcState );

		try {
			memcpy( pBuffer, Ptr, BufferSize);
			if (pBytesRead)
				*pBytesRead = BufferSize;
		} except( CheckException() ) {
			status = STATUS_UNSUCCESSFUL;
		}

		if (STATUS_UNSUCCESSFUL == status)
		{
			ULONG cou = 0;
		
			try {
				for (; cou < BufferSize; cou++)
				{
					Ptr += cou;
					*((PCHAR)pBuffer + cou) = Ptr[cou];
				}
			} except( CheckException() ) {
				if (cou)
				{
					status = STATUS_SUCCESS;
					if (pBytesRead)
						*pBytesRead = cou;
				}
				else
					status = STATUS_UNSUCCESSFUL;
			}
		}

		KeUnstackDetachProcess( &ApcState );

		ObDereferenceObject( pProcess ); 
	}

	return status;
}

VOID
Sys_RegisterExecutedModule (
	PUNICODE_STRING pVolume,
	PUNICODE_STRING pFileName
	)
{
	LIMAGEINFO ImageInfo;
	PLIMAGEINFO pImageInfo = NULL;
	BOOLEAN NewElement = FALSE;

	if (!pVolume || !pFileName)
		return;
	
	ImageInfo.m_Volume = *pVolume;
	ImageInfo.m_FileName = *pFileName;
	ImageInfo.m_Flags = 0;
	ImageInfo.m_Hash.QuadPart = 0;

	AcquireResourceExclusive( &gfmgTables );

	if (0x2000 > RtlNumberGenericTableElements( &gTableModules ))
	{
		CSHORT ItemSize = sizeof(LIMAGEINFO);
		pImageInfo = RtlInsertElementGenericTable( &gTableModules, &ImageInfo, ItemSize, &NewElement );
		if (pImageInfo && NewElement)
		{
			PWSTR pFileNameBuffer = (PWSTR) ExAllocatePoolWithTag( PagedPool, pFileName->Length, tag_table_vol_ptr );
			if (!pFileNameBuffer)
			{
				RtlInitEmptyUnicodeString( &pImageInfo->m_FileName, NULL, 0 );
				RtlInitEmptyUnicodeString( &pImageInfo->m_Volume, NULL, 0 );
			}
			else
			{
				RtlInitEmptyUnicodeString( &pImageInfo->m_FileName, pFileNameBuffer, pFileName->Length );
				pImageInfo->m_Volume.Buffer = pFileNameBuffer;
				RtlCopyUnicodeString( &pImageInfo->m_FileName, pFileName );
			}
		}
	}

	ReleaseResource( &gfmgTables );
}

NTSTATUS
Sys_IsImageActive (
	HANDLE hFile
	)
{
	NTSTATUS status_ret = STATUS_UNSUCCESSFUL;
	NTSTATUS status;

	PFILE_OBJECT pFileObject = NULL;
	PFLT_VOLUME pVolume = NULL;

	status = ObReferenceObjectByHandle (
		hFile,
		0,
		*IoFileObjectType,
		UserMode,
		&pFileObject,
		NULL
		);

	if (!NT_SUCCESS(status) )
		return STATUS_UNSUCCESSFUL;

	status = FltGetVolumeFromFileObject( Globals.m_Filter, pFileObject, &pVolume );
	if (NT_SUCCESS( status ))
	{
		PMKAV_VOLUME_CONTEXT pVolumeContext = NULL;

		status = FltGetVolumeContext( Globals.m_Filter, pVolume,  &pVolumeContext );
		if (NT_SUCCESS(status))
		{
			if (pVolumeContext->m_pFltInstance)
			{
				status = FltObjectReference( pVolumeContext->m_pFltInstance );
				if (NT_SUCCESS(status))
				{
					PFLT_FILE_NAME_INFORMATION pFileNameInfo = NULL;
					status = FltGetFileNameInformationUnsafe (
						pFileObject,
						pVolumeContext->m_pFltInstance,
						FLT_FILE_NAME_QUERY_DEFAULT | FLT_FILE_NAME_NORMALIZED,
						&pFileNameInfo
						);

					if (NT_SUCCESS( status ))
					{
						LIMAGEINFO ImageInfo;
						ImageInfo.m_Volume = pFileNameInfo->Volume;
						ImageInfo.m_FileName = pFileNameInfo->Name;
						
						AcquireResourceExclusive( &gfmgTables );

						if (RtlLookupElementGenericTable( &gTableModules, &ImageInfo ))
							status_ret = STATUS_SUCCESS;

						ReleaseResource( &gfmgTables );

						FltReleaseFileNameInformation( pFileNameInfo );
					}

					FltObjectDereference( pVolumeContext->m_pFltInstance );
				}
			}

			ReleaseFltContext( &pVolumeContext );
		}

		FltObjectDereference( pVolume );
	}

	ObDereferenceObject( pFileObject );

	return status_ret;
}

NTSTATUS
Sys_GetActiveImages (
	PVOID* ppActiveImages,
	ULONG* pActiveImagesLen
	)
{
	NTSTATUS status_ret = STATUS_NO_MEMORY;
	ULONG size = 0;

	PLIMAGEINFO pImageInfo = NULL;

	*ppActiveImages = NULL;
	*pActiveImagesLen = 0;

	AcquireResourceExclusive( &gfmgTables );

	for (pImageInfo = RtlEnumerateGenericTable ( &gTableModules, TRUE );
		pImageInfo != NULL;
		pImageInfo = RtlEnumerateGenericTable ( &gTableModules, FALSE ))
	{
		USHORT VolumeLen = pImageInfo->m_Volume.Length;

		size += MKLIF_INFO_ITEM_SIZE + sizeof(pImageInfo->m_Flags);
		size += MKLIF_INFO_ITEM_SIZE + sizeof(pImageInfo->m_Hash);
		size += MKLIF_INFO_ITEM_SIZE + pImageInfo->m_Volume.Length + sizeof(WCHAR);
		size += MKLIF_INFO_ITEM_SIZE + pImageInfo->m_FileName.Length + sizeof(WCHAR);
	}

	if (size)
	{
		PVOID pActiveImages = NULL;

		size += MKLIF_INFO_ITEM_SIZE; // end tag

		pActiveImages = ExAllocatePoolWithTag( PagedPool, size, tag_active_image );
		if (pActiveImages)
		{
			PMKLIF_INFO_ITEM pItem = (PMKLIF_INFO_ITEM) pActiveImages;

			status_ret = STATUS_SUCCESS;

			for (pImageInfo = RtlEnumerateGenericTable ( &gTableModules, TRUE );
				pImageInfo != NULL;
				pImageInfo = RtlEnumerateGenericTable ( &gTableModules, FALSE ))
			{
				PutInfoItemZ (
					&pItem,
					mkpt_filename,
					pImageInfo->m_FileName.Buffer,
					pImageInfo->m_FileName.Length
					);

				PutInfoItemZ (
					&pItem,
					mkpt_volumename,
					pImageInfo->m_Volume.Buffer,
					pImageInfo->m_Volume.Length
					);

				PutInfoItem (
					&pItem,
					mkpt_flags,
					&pImageInfo->m_Flags,
					sizeof(pImageInfo->m_Flags)
					);
				
				PutInfoItem (
					&pItem,
					mkpt_hash,
					&pImageInfo->m_Hash,
					sizeof(pImageInfo->m_Hash)
					);
			}

			if (NT_SUCCESS (status_ret))
			{
				PutInfoItem (
					&pItem,
					mkpt_end,
					NULL,
					0
					);

				*ppActiveImages = pActiveImages;
				*pActiveImagesLen = size;
			}
			else
				ExFreePool( pActiveImages );
		}
	}

	ReleaseResource( &gfmgTables );

	return status_ret;
}

VOID
Sys_FixStoredFileName (
	__in PFLT_FILE_NAME_INFORMATION pNameInfoNew,
	__in PFLT_FILE_NAME_INFORMATION pNameInfoOld
	)
{
	UNREFERENCED_PARAMETER( pNameInfoNew );
	UNREFERENCED_PARAMETER( pNameInfoOld );

	AcquireResourceExclusive( &gProcessInfoListLock );

	if (!_impIsListEmpty( &gProcessInfoList ))
	{
		PPROCESS_INFO pProcessInfo;
		PIMPLIST_ENTRY Flink;
		
		Flink = gProcessInfoList.Flink;

		while(Flink != &gProcessInfoList)
		{
			pProcessInfo = CONTAINING_RECORD(Flink, PROCESS_INFO, m_List);
			if (pProcessInfo->m_usNativePath.Length)
			{
				if (!RtlCompareUnicodeString( &pProcessInfo->m_usNativePath, &pNameInfoOld->Name, FALSE ))
				{
					USHORT newLength = pNameInfoNew->Name.Length;
					PWCHAR pwchReparsedName = ExAllocatePoolWithTag( PagedPool, newLength, tag_name_norm );

					if (pwchReparsedName)
					{
						ExFreePool( pProcessInfo->m_usNativePath.Buffer );

						RtlInitEmptyUnicodeString (
							&pProcessInfo->m_usNativePath,
							pwchReparsedName,
							newLength
							);

						RtlCopyUnicodeString( &pProcessInfo->m_usNativePath, &pNameInfoNew->Name );

						DoTraceEx( TRACE_LEVEL_WARNING, DC_SYSTEM, "Process %p changed name to'%wZ'",
							pProcessInfo->m_ProcessId,
							&pProcessInfo->m_usNativePath
							);
					}
				}
			}
			Flink = Flink->Flink;
		}
	}

	ReleaseResource( &gProcessInfoListLock );
}

NTSTATUS
Sys_QueryEnlistedObjects (
	PMKAV_TRANSACTION_CONTEXT pTransactionContext,
	PVOID* ppEnlistedObjects,
	ULONG* pEnlistedObjectsLen
	)
{
	PVOID pEnlistedObjects = NULL;
	PMKLIF_INFO_ITEM pItem = NULL;

	PMKAV_ENLISTED_FILE pFileEntry = NULL;
	PRTL_GENERIC_TABLE pFilesTable = NULL;
	PMKAV_ENLISTED_KEY pKeyEntry = NULL;
	PRTL_GENERIC_TABLE pKeysTable = NULL;

	ULONG size = 0;
	ULONG RealSize = 0;

	//+ calc size
	//+ enum files
	AcquireResourceExclusive( &pTransactionContext->m_FileLock );
	pFilesTable = &pTransactionContext->m_EnlistedFilesTable;

	for (pFileEntry = RtlEnumerateGenericTable ( pFilesTable , TRUE );
		pFileEntry != NULL;
		pFileEntry = RtlEnumerateGenericTable ( pFilesTable, FALSE ))
	{
		size += MKLIF_INFO_ITEM_SIZE + sizeof(pFileEntry->m_ProcessId);	//pid
		size += MKLIF_INFO_ITEM_SIZE + pFileEntry->m_NameLen;
	}

	ReleaseResource( &pTransactionContext->m_FileLock );
	//- enum files

	//+ enum keys
	AcquireResourceExclusive( &pTransactionContext->m_RegLock );
	pKeysTable = &pTransactionContext->m_EnlistedKeysTable;

	for (pKeyEntry = RtlEnumerateGenericTable ( pKeysTable , TRUE );
		pKeyEntry != NULL;
		pKeyEntry = RtlEnumerateGenericTable ( pKeysTable, FALSE ))
	{
		size += MKLIF_INFO_ITEM_SIZE + sizeof(pKeyEntry->m_ProcessId);	//pid
		size += MKLIF_INFO_ITEM_SIZE + pKeyEntry->m_NameLen;
	}

	ReleaseResource( &pTransactionContext->m_RegLock );
	//- enum keys

	if (!size)
	{
		_dbg_break_;
		return STATUS_NO_MORE_ENTRIES;
	}

	size += MKLIF_INFO_ITEM_SIZE; // end tag
	size += 0x1000; //reserv

	pEnlistedObjects = ExAllocatePoolWithTag( PagedPool, size, tag_enobj_info );
	if (!pEnlistedObjects)
		return STATUS_NO_MEMORY;

	pItem = (PMKLIF_INFO_ITEM) pEnlistedObjects;

	// put data
	AcquireResourceExclusive( &pTransactionContext->m_FileLock );
	pFilesTable = &pTransactionContext->m_EnlistedFilesTable;

	for (pFileEntry = RtlEnumerateGenericTable ( pFilesTable , TRUE );
		pFileEntry != NULL;
		pFileEntry = RtlEnumerateGenericTable ( pFilesTable, FALSE ))
	{
		RealSize += MKLIF_INFO_ITEM_SIZE + sizeof(pFileEntry->m_ProcessId);	//pid
		RealSize += MKLIF_INFO_ITEM_SIZE + pFileEntry->m_NameLen;

		if (RealSize > size)
			break;

		PutInfoItem (
			&pItem,
			mkpt_process_id,
			&pFileEntry->m_ProcessId,
			sizeof(pFileEntry->m_ProcessId)
			);

		PutInfoItem (
			&pItem,
			mkpt_filename,
			pFileEntry->m_FileName,
			pFileEntry->m_NameLen
			);
	}

	ReleaseResource( &pTransactionContext->m_FileLock );
	//- enum files

	//+ enum keys
	AcquireResourceExclusive( &pTransactionContext->m_RegLock );
	pKeysTable = &pTransactionContext->m_EnlistedKeysTable;

	for (pKeyEntry = RtlEnumerateGenericTable ( pKeysTable , TRUE );
		pKeyEntry != NULL;
		pKeyEntry = RtlEnumerateGenericTable ( pKeysTable, FALSE ))
	{
		RealSize += MKLIF_INFO_ITEM_SIZE + sizeof(pKeyEntry->m_ProcessId);	//pid
		RealSize += MKLIF_INFO_ITEM_SIZE + pKeyEntry->m_NameLen;

		if (RealSize > size)
			break;

		PutInfoItem (
			&pItem,
			mkpt_process_id,
			&pKeyEntry->m_ProcessId,
			sizeof(pFileEntry->m_ProcessId)
			);

		PutInfoItem (
			&pItem,
			mkpt_filename,
			pKeyEntry->m_KeyPath,
			pKeyEntry->m_NameLen
			);
	}

	ReleaseResource( &pTransactionContext->m_RegLock );
	//- enum keys

	if (RealSize > size)
	{
		ExFreePool( pEnlistedObjects );
		return STATUS_BUFFER_TOO_SMALL;
	}

	*ppEnlistedObjects = pEnlistedObjects;
	*pEnlistedObjectsLen = RealSize;

	return STATUS_SUCCESS;
}

// calc hash
typedef struct _sAsyncInfo
{
	IO_STATUS_BLOCK IoStatus;
	KEVENT Event;
}AsyncInfo, *PAsyncInfo;

void
cbFuncReadFile (
    __in PFLT_CALLBACK_DATA CallbackData,
    __in PFLT_CONTEXT Context
    )
{
	PAsyncInfo pInfo = (PAsyncInfo) Context;
	
	pInfo->IoStatus = CallbackData->IoStatus;

	KeSetEvent( &pInfo->Event, 0, FALSE );
}

NTSTATUS
Sys_CalcHash (
	__in PFILE_OBJECT pFileObject,
	__in PVOID pHashBuffer,
	__in ULONG HashBufferSize
	)
{
	NTSTATUS status = STATUS_UNSUCCESSFUL;
	ULONG _read_buf_size = 4096;
	PVOID pBuffer = NULL;

	AsyncInfo IoInfo;
	
	LARGE_INTEGER Offset = {0, 0};
	ULONG cbRead = 0;

	md5_byte_t digest[16];
	md5_state_t md5st;

	PMKAV_VOLUME_CONTEXT pVolumeContext = NULL;

	if (sizeof(digest) != HashBufferSize)
		return STATUS_INVALID_PARAMETER;

	pVolumeContext = GetVolumeContextFromFO( pFileObject );
	if (!pVolumeContext)
	{
		_dbg_break_;
		return STATUS_UNSUCCESSFUL;
	}

	pBuffer = ExAllocatePoolWithTag( PagedPool, _read_buf_size, tag_fid_request );
	if (!pBuffer)
		return STATUS_NO_MEMORY;

	KeInitializeEvent( &IoInfo.Event, SynchronizationEvent, FALSE );
	md5_init(&md5st);

	while(TRUE)
	{
		KeClearEvent( &IoInfo.Event );

		status = FltReadFile (
			pVolumeContext->m_pFltInstance,
			pFileObject,
			&Offset,
			_read_buf_size,
			pBuffer,
			0,
			&cbRead,
			cbFuncReadFile,
			&IoInfo
			);

		if (STATUS_PENDING == status)
		{
			KeWaitForSingleObject( &IoInfo.Event, Executive, KernelMode, FALSE, NULL );
			cbRead = (ULONG) IoInfo.IoStatus.Information;
			status = IoInfo.IoStatus.Status;
		}

		if (NT_SUCCESS(status))
		{
			DoTraceEx( TRACE_LEVEL_VERBOSE, DC_DRIVER, "read (hash) ok. ReadBytes %d", cbRead );
		}
		else
		{
			DoTraceEx( TRACE_LEVEL_WARNING, DC_DRIVER, "read (hash) result %!STATUS!. ReadBytes %d",
				status,
				cbRead
				);
		}

		if (NT_SUCCESS( status ))
		{
			if (cbRead)
			{
				Offset.QuadPart += cbRead;
				md5_append( &md5st, (md5_byte_t*) pBuffer, cbRead );
			}

			if (_read_buf_size != cbRead)
				break;
		}
		else
		{
			if (STATUS_END_OF_FILE == status)
				status = STATUS_SUCCESS;
			else
			{
				_dbg_break_;
			}

			break;
		}
	}

	md5_finish(&md5st, digest);

	if (NT_SUCCESS(status))
	{
		memcpy( pHashBuffer, digest, sizeof(digest) );

		DoTraceEx( TRACE_LEVEL_INFORMATION, DC_SYSTEM, "app hash 0x%x%x...%x%x",
			digest[0], digest[1], digest[14], digest[15]);
	}

	ExFreePool( pBuffer );

	ReleaseFltContext( &pVolumeContext );

	return status;
}

NTSTATUS
Sys_CalcFileHashByName (
	__in PUNICODE_STRING pusNativePath,
	__inout PVOID  pBuffer,
	__inout PULONG pBufferSize
	)
{
	NTSTATUS status = STATUS_UNSUCCESSFUL;

	HANDLE hFile = NULL;
	PFILE_OBJECT pFileObject = NULL;

	OBJECT_ATTRIBUTES oa;

	IO_STATUS_BLOCK ioStatus;

	if ( !pBufferSize || !pBuffer )
		return	STATUS_INVALID_PARAMETER;

	if ( *pBufferSize < sizeof(md5_byte_t) * 16 )
		return	STATUS_BUFFER_TOO_SMALL;

	InitializeObjectAttributes( &oa, pusNativePath, OBJ_CASE_INSENSITIVE | OBJ_KERNEL_HANDLE, NULL, NULL );

	status = FltCreateFile (
		Globals.m_Filter,
		NULL,
		&hFile,
		FILE_READ_DATA | SYNCHRONIZE,
		&oa,
		&ioStatus,
		NULL,
		FILE_ATTRIBUTE_NORMAL,
		FILE_SHARE_READ | FILE_SHARE_WRITE | FILE_SHARE_DELETE,
		FILE_OPEN,
		FILE_RANDOM_ACCESS | FILE_SYNCHRONOUS_IO_NONALERT,
		NULL,
		0,
		IO_IGNORE_SHARE_ACCESS_CHECK
		);

	DoTraceEx( TRACE_LEVEL_INFORMATION, DC_SYSTEM, "CreateFile %wZ for hash %!STATUS!",
		pusNativePath,
		status
		);

	if (NT_SUCCESS( status ))
	{
		status = ObReferenceObjectByHandle (
			hFile,
			0,
			*IoFileObjectType,
			KernelMode,
			&pFileObject,
			NULL
			);

		if(NT_SUCCESS( status ))
		{
			status = Sys_CalcHash( pFileObject, pBuffer, sizeof(md5_byte_t) * 16 );
			ObDereferenceObject( pFileObject );

			if (NT_SUCCESS( status ))
				*pBufferSize = sizeof(md5_byte_t)*16;
		}

		FltClose( hFile );
	}

	return status;
}
NTSTATUS
Sys_CalcFileHash(
	__in PWCHAR FileName,
	__in USHORT	FileNameLen,
	__inout PVOID  pBuffer,
	__inout PULONG pBufferSize
	)
{
	UNICODE_STRING usNativePath;
	NTSTATUS status = STATUS_UNSUCCESSFUL;

	HANDLE hFile = NULL;
	PFILE_OBJECT pFileObject = NULL;

	if ( !pBufferSize || !pBuffer )
		return	STATUS_INVALID_PARAMETER;

	if ( *pBufferSize< sizeof(md5_byte_t) * 16 )
		return	STATUS_BUFFER_TOO_SMALL;

	status = Sys_BuildNativePath( FileName, FileNameLen, &usNativePath );

	DoTraceEx( TRACE_LEVEL_VERBOSE, DC_SYSTEM, "Sys_BuildNativePath result %!STATUS!", status );

	if (!NT_SUCCESS( status ))
		return STATUS_UNSUCCESSFUL;

	status = Sys_CalcFileHashByName( &usNativePath, pBuffer, pBufferSize );

	ExFreePool(usNativePath.Buffer);

	return status;
}

#ifndef _WIN64
//------------------------------------------------------------------------------------------------
volatile LONG g_bInTryInitShadowTablePtr = FALSE;
volatile PServiceDescriptorTableEntry_t g_pShadowSDT = NULL;
volatile BOOLEAN g_bShadowSystemNotifiers_Inited = FALSE;
volatile PEPROCESS g_pCSRSSProc = NULL;

#define SERVICEID_NOT_ASSIGNED		0xffffL

VOID _impSetSystemNotifiers_Shadow()
{
	BOOLEAN bResult;
	
	ULONG	ServID_NtUserSetWindowsHookEx = SERVICEID_NOT_ASSIGNED;
	ULONG	ServID_NtUserSetWinEventHook = SERVICEID_NOT_ASSIGNED;

	ULONG	ServID_NtUserQueryWindow = SERVICEID_NOT_ASSIGNED;
	ULONG	ServID_NtUserGetForegroundWindow = SERVICEID_NOT_ASSIGNED;

	ULONG	ServID_NtUserGetAsyncKeyState = SERVICEID_NOT_ASSIGNED;
	ULONG	ServID_NtUserGetKeyState = SERVICEID_NOT_ASSIGNED;
	ULONG	ServID_NtUserGetKeyboardState = SERVICEID_NOT_ASSIGNED;
	ULONG	ServID_NtUserAttachThreadInput = SERVICEID_NOT_ASSIGNED;

	ULONG	ServID_NtUserSendInput = SERVICEID_NOT_ASSIGNED;
	ULONG	ServID_NtUserFindWindow = SERVICEID_NOT_ASSIGNED;

	ULONG	ServID_NtGdiGetDCPoint = SERVICEID_NOT_ASSIGNED;
	ULONG	ServID_NtGdiGetDCDword = SERVICEID_NOT_ASSIGNED;
	ULONG	ServID_NtUserWindowFromPoint = SERVICEID_NOT_ASSIGNED;

	ULONG	ServID_NtGdiBitBlt = SERVICEID_NOT_ASSIGNED;
	ULONG	ServID_NtGdiStretchBlt = SERVICEID_NOT_ASSIGNED;
	ULONG	ServID_NtGdiMaskBlt = SERVICEID_NOT_ASSIGNED;
	ULONG	ServID_NtGdiPlgBlt = SERVICEID_NOT_ASSIGNED;

	ULONG	ServID_NtUserMessageCall = SERVICEID_NOT_ASSIGNED;
	ULONG	ServID_NtUserSendNotifyMessage = SERVICEID_NOT_ASSIGNED;
	ULONG	ServID_NtUserPostMessage = SERVICEID_NOT_ASSIGNED;
	ULONG	ServID_NtUserPostThreadMessage = SERVICEID_NOT_ASSIGNED;

	ULONG	ServID_NtRegisterRawInputDevices = SERVICEID_NOT_ASSIGNED;

	ULONG	ServID_NtUserCallOneParam = SERVICEID_NOT_ASSIGNED;
	
	DoTraceEx(
		TRACE_LEVEL_INFORMATION,
		DC_SYSTEM,
		"_impSetSystemNotifiers_Shadow start..."
		);

	if ( g_bIsW2K )
	{
		// win 2k
		ServID_NtUserSetWindowsHookEx = 0x1212;
		ServID_NtUserSetWinEventHook = 0x1215;

		ServID_NtUserQueryWindow = 0x11d2;
		ServID_NtUserGetForegroundWindow = 0x1189;
		ServID_NtUserGetAsyncKeyState = 0x1175;
		ServID_NtUserGetKeyState = 0x1195;
		ServID_NtUserGetKeyboardState = 0x1193;
		ServID_NtUserAttachThreadInput = 0x1129;

		ServID_NtGdiGetDCPoint = 0x10a6;
		ServID_NtGdiGetDCDword = 0x10a3;
		ServID_NtUserWindowFromPoint = 0x1238;

		ServID_NtGdiBitBlt = 0x100d;
		ServID_NtGdiStretchBlt = 0x111a;
		ServID_NtGdiMaskBlt = 0x10db;
		ServID_NtGdiPlgBlt = 0x10e5;

		ServID_NtUserPostMessage = 0x11cb;
		ServID_NtUserPostThreadMessage = 0x11cc;
		ServID_NtUserMessageCall = 0x11bc;
		ServID_NtUserSendNotifyMessage = 0x11e3;

		ServID_NtUserSendInput = 0x11e1;
		ServID_NtUserFindWindow = 0x1170;

		ServID_NtUserCallOneParam = 0x1139;
	}
	if ( g_bIsWXP )
	{
		// win xp
		ServID_NtUserSetWindowsHookEx = 0x1225;
		ServID_NtUserSetWinEventHook = 0x1228;

		ServID_NtUserQueryWindow = 0x11e3;
		ServID_NtUserGetForegroundWindow = 0x1194;
		ServID_NtUserGetAsyncKeyState = 0x117f;
		ServID_NtUserGetKeyState = 0x11a0;
		ServID_NtUserGetKeyboardState = 0x119e;
		ServID_NtUserAttachThreadInput = 0x1133;

		ServID_NtGdiGetDCPoint = 0x10aa;
		ServID_NtGdiGetDCDword = 0x10a7;
		ServID_NtUserWindowFromPoint = 0x1250;

		ServID_NtGdiBitBlt = 0x100d;
		ServID_NtGdiStretchBlt = 0x1124;
		ServID_NtGdiMaskBlt = 0x10e3;
		ServID_NtGdiPlgBlt = 0x10ed;

		ServID_NtUserPostMessage = 0x11db;
		ServID_NtUserPostThreadMessage = 0x11dc;
		ServID_NtUserMessageCall = 0x11cc;

		ServID_NtUserSendInput = 0x11f6;
		ServID_NtUserFindWindow = 0x117a;

		ServID_NtRegisterRawInputDevices = 0x11eb;
		
		ServID_NtUserCallOneParam = 0x1143;
	}
	if ( g_bIsW2003 )
	{
		// win 2003 srv
		ServID_NtUserSetWindowsHookEx = 0x1221;
		ServID_NtUserQueryWindow = 0x11e1;
		ServID_NtUserGetForegroundWindow = 0x1193;
		ServID_NtUserGetAsyncKeyState = 0x117e;
		ServID_NtUserGetKeyState = 0x119f;

		ServID_NtUserPostMessage = 0x11da;
		ServID_NtUserSendInput = 0x11f4;

		ServID_NtUserFindWindow = 0x1179;
	}

	if ( SERVICEID_NOT_ASSIGNED != ServID_NtUserQueryWindow )
	{
		klNtUserQueryWindow = GetNtFuncByID(
			ServID_NtUserQueryWindow,
			NULL
			);
		if ( !klNtUserQueryWindow )
		{
			DoTraceEx(
				TRACE_LEVEL_ERROR,
				DC_SYSTEM,
				"_impSetSystemNotifiers_Shadow:: failed to get NtUserQueryWindow address"
				);
		}			
	}
	if ( SERVICEID_NOT_ASSIGNED != ServID_NtUserGetForegroundWindow  )
	{
		klNtUserGetForegroundWindow = GetNtFuncByID(
			ServID_NtUserGetForegroundWindow,
			NULL
			);
		if ( !klNtUserGetForegroundWindow )
		{
			DoTraceEx(
				TRACE_LEVEL_ERROR,
				DC_SYSTEM,
				"_impSetSystemNotifiers_Shadow:: failed to get NtUserGetForegroundWindow address"
				);
		}			
	}

	if ( SERVICEID_NOT_ASSIGNED != ServID_NtUserSetWindowsHookEx )
	{
		bResult = HookNtFuncByID(
			ServID_NtUserSetWindowsHookEx,
			klhUserSetWindowsHookEx,
			(PVOID *) &klNtUserSetWindowsHookEx,
			NULL
			);
		if ( !bResult )
		{
			DoTraceEx(
				TRACE_LEVEL_ERROR,
				DC_SYSTEM,
				"_impSetSystemNotifiers_Shadow:: failed to hook NtUserSetWindowsHookEx"
				);
		}
	}
	if ( SERVICEID_NOT_ASSIGNED != ServID_NtUserSetWinEventHook )
	{
		bResult = HookNtFuncByID(
			ServID_NtUserSetWinEventHook,
			klhUserSetWinEventHook,
			(PVOID *) &klNtUserSetWinEventHook,
			NULL
			);
		if ( !bResult )
		{
			DoTraceEx(
				TRACE_LEVEL_ERROR,
				DC_SYSTEM,
				"_impSetSystemNotifiers_Shadow:: failed to hook NtUserSetWinEventHook"
				);
		}
	}

	// setup XXXKeyRead keylogger & SendInput / FindWindow hooks
	if ( !( klNtUserQueryWindow && klNtUserGetForegroundWindow ) )
	{
		DoTraceEx(
			TRACE_LEVEL_ERROR,
			DC_SYSTEM,
			"_impSetSystemNotifiers_Shadow:: won't setup key poll hooks"
			);
	}
	else
	{
		if ( SERVICEID_NOT_ASSIGNED != ServID_NtUserGetAsyncKeyState )
		{
			bResult = HookNtFuncByID(
				ServID_NtUserGetAsyncKeyState,
				klhUserGetAsyncKeyState,
				(PVOID *) &klNtUserGetAsyncKeyState,
				NULL
				);
			if ( !bResult )
			{
				DoTraceEx(
					TRACE_LEVEL_ERROR,
					DC_SYSTEM,
					"_impSetSystemNotifiers_Shadow:: failed to hook NtUserGetAsyncKeyState"
					);
			}
		}
		if ( SERVICEID_NOT_ASSIGNED != ServID_NtUserGetKeyState )
		{
			bResult = HookNtFuncByID(
				ServID_NtUserGetKeyState,
				klhUserGetKeyState,
				(PVOID *) &klNtUserGetKeyState,
				NULL
				);
			if ( !bResult )
			{
				DoTraceEx(
					TRACE_LEVEL_ERROR,
					DC_SYSTEM,
					"_impSetSystemNotifiers_Shadow:: failed to hook NtUserGetKeyState"
					);
			}
		}
		if ( SERVICEID_NOT_ASSIGNED != ServID_NtUserGetKeyboardState )
		{
			bResult = HookNtFuncByID(
				ServID_NtUserGetKeyboardState,
				klhUserGetKeyboardState,
				(PVOID *) &klNtUserGetKeyboardState,
				NULL
				);
			if ( !bResult )
			{
				DoTraceEx(
					TRACE_LEVEL_ERROR,
					DC_SYSTEM,
					"_impSetSystemNotifiers_Shadow:: failed to hook NtUserGetKeyboardState"
					);
			}
		}
		if ( SERVICEID_NOT_ASSIGNED != ServID_NtUserAttachThreadInput )
		{
			bResult = HookNtFuncByID(
				ServID_NtUserAttachThreadInput,
				klhUserAttachThreadInput,
				(PVOID *) &klNtUserAttachThreadInput,
				NULL
				);
			if ( !bResult )
			{
				DoTraceEx(
					TRACE_LEVEL_ERROR,
					DC_SYSTEM,
					"_impSetSystemNotifiers_Shadow:: failed to hook NtUserAttachThreadInput"
					);
			}
		}
		if ( SERVICEID_NOT_ASSIGNED != ServID_NtUserSendInput )
		{
			bResult = HookNtFuncByID(
				ServID_NtUserSendInput,
				klhUserSendInput,
				(PVOID *) &klNtUserSendInput,
				NULL
				);
			if ( !bResult )
			{
				DoTraceEx(
					TRACE_LEVEL_ERROR,
					DC_SYSTEM,
					"_impSetSystemNotifiers_Shadow:: failed to hook NtUserSendInput"
					);
			}
		}
		if ( SERVICEID_NOT_ASSIGNED != ServID_NtUserFindWindow )
		{
			bResult = HookNtFuncByID(
				ServID_NtUserFindWindow,
				klhUserFindWindowEx,
				(PVOID *) &klNtUserFindWindowEx,
				NULL
				);
			if ( !bResult )
			{
				DoTraceEx(
					TRACE_LEVEL_ERROR,
					DC_SYSTEM,
					"_impSetSystemNotifiers_Shadow:: failed to hook NtUserFindWindow"
					);
			}
		}
	}

	if ( SERVICEID_NOT_ASSIGNED != ServID_NtGdiGetDCPoint )
	{
		klNtGdiGetDCPoint = GetNtFuncByID(
			ServID_NtGdiGetDCPoint,
			NULL
			);
		if ( !klNtGdiGetDCPoint )
		{
			DoTraceEx(
				TRACE_LEVEL_ERROR,
				DC_SYSTEM,
				"_impSetSystemNotifiers_Shadow:: failed to get NtGdiGetDCPoint address"
				);
		}			
	}
	if ( SERVICEID_NOT_ASSIGNED != ServID_NtGdiGetDCDword )
	{
		klNtGdiGetDCDword = GetNtFuncByID(
			ServID_NtGdiGetDCDword,
			NULL
			);
		if ( !klNtGdiGetDCDword )
		{
			DoTraceEx(
				TRACE_LEVEL_ERROR,
				DC_SYSTEM,
				"_impSetSystemNotifiers_Shadow:: failed to get NtGdiGetDCDword address"
				);
		}			
	}
	if ( SERVICEID_NOT_ASSIGNED != ServID_NtUserWindowFromPoint )
	{
		klNtUserWindowFromPoint = GetNtFuncByID(
			ServID_NtUserWindowFromPoint,
			NULL
			);
		if ( !klNtUserWindowFromPoint )
		{
			DoTraceEx(
				TRACE_LEVEL_ERROR,
				DC_SYSTEM,
				"_impSetSystemNotifiers_Shadow:: failed to get NtUserWindowFromPoint address"
				);
		}			
	}

	// setup XXXBlt screenshot hooks
	if ( !( klNtUserQueryWindow && klNtGdiGetDCPoint && klNtGdiGetDCDword && klNtUserWindowFromPoint ) )
	{
		DoTraceEx(
			TRACE_LEVEL_ERROR,
			DC_SYSTEM,
			"_impSetSystemNotifiers_Shadow:: won't setup screenshot hooks"
			);
	}
	else
	{
		if ( SERVICEID_NOT_ASSIGNED != ServID_NtGdiBitBlt )
		{
			bResult = HookNtFuncByID(
				ServID_NtGdiBitBlt,
				klhGdiBitBlt,
				(PVOID *) &klNtGdiBitBlt,
				NULL
				);
			if ( !bResult )
			{
				DoTraceEx(
					TRACE_LEVEL_ERROR,
					DC_SYSTEM,
					"_impSetSystemNotifiers_Shadow:: failed to hook NtGdiBitBlt"
					);
			}
		}
		if ( SERVICEID_NOT_ASSIGNED != ServID_NtGdiStretchBlt )
		{
			bResult = HookNtFuncByID(
				ServID_NtGdiStretchBlt,
				klhGdiStretchBlt,
				(PVOID *) &klNtGdiStretchBlt,
				NULL
				);
			if ( !bResult )
			{
				DoTraceEx(
					TRACE_LEVEL_ERROR,
					DC_SYSTEM,
					"_impSetSystemNotifiers_Shadow:: failed to hook NtGdiStretchBlt"
					);
			}
		}
		if ( SERVICEID_NOT_ASSIGNED != ServID_NtGdiMaskBlt )
		{
			bResult = HookNtFuncByID(
				ServID_NtGdiMaskBlt,
				klhGdiMaskBlt,
				(PVOID *) &klNtGdiMaskBlt,
				NULL
				);
			if ( !bResult )
			{
				DoTraceEx(
					TRACE_LEVEL_ERROR,
					DC_SYSTEM,
					"_impSetSystemNotifiers_Shadow:: failed to hook NtGdiMaskBlt"
					);
			}
		}
		if ( SERVICEID_NOT_ASSIGNED != ServID_NtGdiPlgBlt )
		{
			bResult = HookNtFuncByID(
				ServID_NtGdiPlgBlt,
				klhGdiPlgBlt,
				(PVOID *) &klNtGdiPlgBlt,
				NULL
				);
			if ( !bResult )
			{
				DoTraceEx(
					TRACE_LEVEL_ERROR,
					DC_SYSTEM,
					"_impSetSystemNotifiers_Shadow:: failed to hook NtGdiPlgBlt"
					);
			}
		}
	}

	if ( klNtUserQueryWindow )
	{
		if ( SERVICEID_NOT_ASSIGNED != ServID_NtUserMessageCall )
		{
			bResult = HookNtFuncByID(
				ServID_NtUserMessageCall,
				klhUserMessageCall,
				(PVOID *) &klNtUserMessageCall,
				NULL
				);
			if ( !bResult )
			{
				DoTraceEx(
					TRACE_LEVEL_ERROR,
					DC_SYSTEM,
					"_impSetSystemNotifiers_Shadow:: failed to hook NtUserMessageCall"
					);
			}
		}
		// for win2k only
		if ( SERVICEID_NOT_ASSIGNED != ServID_NtUserSendNotifyMessage )
		{
			bResult = HookNtFuncByID(
				ServID_NtUserSendNotifyMessage,
				klhUserSendNotifyMessage,
				(PVOID *) &klNtUserSendNotifyMessage,
				NULL
				);
			if ( !bResult )
			{
				DoTraceEx(
					TRACE_LEVEL_ERROR,
					DC_SYSTEM,
					"_impSetSystemNotifiers_Shadow:: failed to hook NtUserSendNotifyMessage"
					);
			}
		}
		if ( SERVICEID_NOT_ASSIGNED != ServID_NtUserPostMessage )
		{
			bResult = HookNtFuncByID(
				ServID_NtUserPostMessage,
				klhUserPostMessage,
				(PVOID *) &klNtUserPostMessage,
				NULL
				);
			if ( !bResult )
			{
				DoTraceEx(
					TRACE_LEVEL_ERROR,
					DC_SYSTEM,
					"_impSetSystemNotifiers_Shadow:: failed to hook NtUserPostMessage"
					);
			}
		}
		if ( SERVICEID_NOT_ASSIGNED != ServID_NtUserPostThreadMessage )
		{
			bResult = HookNtFuncByID(
				ServID_NtUserPostThreadMessage,
				klhUserPostThreadMessage,
				(PVOID *) &klNtUserPostThreadMessage,
				NULL
				);
			if ( !bResult )
			{
				DoTraceEx(
					TRACE_LEVEL_ERROR,
					DC_SYSTEM,
					"_impSetSystemNotifiers_Shadow:: failed to hook NtUserPostThreadMessage"
					);
			}
		}

		//RegisterRawInputDevices
		if ( SERVICEID_NOT_ASSIGNED != ServID_NtRegisterRawInputDevices )
		{
			bResult = HookNtFuncByID(
				ServID_NtRegisterRawInputDevices,
				klhRegisterRawInputDevices,
				(PVOID *) &klNtRegisterRawInputDevices,
				NULL
				);
			if ( !bResult )
			{
				DoTraceEx(
					TRACE_LEVEL_ERROR,
					DC_SYSTEM,
					"_impSetSystemNotifiers_Shadow:: failed to hook NtRegisterRawInputDevices"
					);
			}
		}

		//NtUserCallOneParam
		if ( SERVICEID_NOT_ASSIGNED != ServID_NtUserCallOneParam )
		{
			bResult = HookNtFuncByID(
				ServID_NtUserCallOneParam,
				klhUserCallOneParam,
				(PVOID *) &klNtUserCallOneParam,
				NULL
				);
			if ( !bResult )
			{
				DoTraceEx(
					TRACE_LEVEL_ERROR,
					DC_SYSTEM,
					"_impSetSystemNotifiers_Shadow:: failed to hook NtUserCallOneParam"
					);
			}
		}
	}

	DoTraceEx(
		  TRACE_LEVEL_INFORMATION,
		  DC_SYSTEM,
		  "_impSetSystemNotifiers_Shadow end."
		  );
}

typedef struct _MONITORED_SERVICE_ITEM
{
	PVOID	*m_pfServiceFunc;
	PVOID	m_fPointerOnStart;
} MONITORED_SERVICE_ITEM, *PMONITORED_SERVICE_ITEM;

MONITORED_SERVICE_ITEM g_NtUserGetMessage_Service = { NULL, NULL };
MONITORED_SERVICE_ITEM g_NtUserPeekMessage_Service = { NULL, NULL };

PVOID g_pWin32kBase = NULL;
ULONG g_Win32kSize = 0;

// should be called once in the address context of CSRSS process
VOID Win32kVerifier_Init()
{
	NTSTATUS status;
	PSYS_MOD_INFO pSysInfo;

	pSysInfo = (PSYS_MOD_INFO) ExAllocatePoolWithTag(
		PagedPool,
		0x8000,
		'ISrg');
	if ( pSysInfo )
	{
		ULONG RetLen = 0;

		status = ZwQuerySystemInformation(
			11/*SystemModuleInformation*/,
			pSysInfo,
			0x8000,
			&RetLen
			);
		if ( STATUS_INFO_LENGTH_MISMATCH == status && RetLen )
		{
			ExFreePool( pSysInfo );
			pSysInfo = ExAllocatePoolWithTag(
				PagedPool,
				RetLen,
				'ISrg');
			if ( pSysInfo )
			{
				status = ZwQuerySystemInformation(
					11/*SystemModuleInformation*/,
					pSysInfo,
					RetLen,
					&RetLen
					);
			}
		}
		if ( NT_SUCCESS( status ) )
		{
			ULONG ModuleCount = pSysInfo->NumberOfModules;
			PSYSTEM_MODULE_INFORMATION pModuleInfo = pSysInfo->Module;

			if ( 0 != ModuleCount && pModuleInfo )
			{
				ULONG i;

				for ( i = 0; i < ModuleCount; i++ )
				{
					PSYSTEM_MODULE_INFORMATION pCurrInfo = pModuleInfo + i;
					PCHAR cModName = (PCHAR)pCurrInfo->ImageName + pCurrInfo->ModNameOffset;

					if ( 0 == _stricmp( cModName, "win32k.sys") )
					{
						g_pWin32kBase = pCurrInfo->Base;
						g_Win32kSize = pCurrInfo->Size;

						DoTraceEx(
							TRACE_LEVEL_INFORMATION,
							DC_SYSTEM,
							"Win32kVerifier_Init:: inited."
							);

						break;
					}
				}
			}
		}

		ExFreePool( pSysInfo );
	}

	if ( !g_pWin32kBase )
	{
		DoTraceEx(
			TRACE_LEVEL_ERROR,
			DC_SYSTEM,
			"Win32kVerifier_Init:: failed."
			);
	}
}

// should be called once in the address context of CSRSS process
VOID ShadowTableVerifier_Init()
{
	ULONG NtUserGetMessageID;
	ULONG NtUserPeekMessageID;
	PVOID NtUserGetMessage_OnStart;
	PVOID NtUserPeekMessage_OnStart;
	PVOID *p_fNtUserGetMessage;
	PVOID *p_fNtUserPeekMessage;

	if ( g_bIsW2003 )
	{
		// win 2003 srv
		NtUserGetMessageID = 0x11a4;
		NtUserPeekMessageID = 0x11d9;
	}
	if ( g_bIsWXP )
	{
		// win xp
		NtUserGetMessageID = 0x11a5;
		NtUserPeekMessageID = 0x11da;
	}
	if ( g_bIsW2K )
	{
		// win 2k
		NtUserGetMessageID = 0x119a;
		NtUserPeekMessageID = 0x11ca;
	}

	NtUserGetMessage_OnStart = GetNtFuncByID(
		NtUserGetMessageID,
		&p_fNtUserGetMessage
		);
	NtUserPeekMessage_OnStart = GetNtFuncByID(
		NtUserPeekMessageID,
		&p_fNtUserPeekMessage
		);
	if ( !( NtUserGetMessage_OnStart && NtUserPeekMessage_OnStart ) )
	{
		DoTraceEx(
			TRACE_LEVEL_ERROR,
			DC_SYSTEM,
			"ShadowTableVerifier_Init:: failed."
			);
	}
	else
	{
		g_NtUserGetMessage_Service.m_fPointerOnStart = NtUserGetMessage_OnStart;
		g_NtUserGetMessage_Service.m_pfServiceFunc = p_fNtUserGetMessage;
		g_NtUserPeekMessage_Service.m_fPointerOnStart = NtUserPeekMessage_OnStart;
		g_NtUserPeekMessage_Service.m_pfServiceFunc = p_fNtUserPeekMessage;

		DoTraceEx(
			TRACE_LEVEL_INFORMATION,
			DC_SYSTEM,
			"ShadowTableVerifier_Init:: inited."
			);
	}
}

// set win32k.sys (shadow SDT) hooks
VOID SetSystemNotifiers_Shadow()
{
	PServiceDescriptorTableEntry_t pWIN32KEntry;
	KAPC_STATE APCState;

	ASSERT( g_pShadowSDT );

	if ( g_bShadowSystemNotifiers_Inited )
		return;

	KeStackAttachProcess(
		g_pCSRSSProc,
		&APCState
		);

	pWIN32KEntry = g_pShadowSDT + 1;
	if ( pWIN32KEntry->ServiceTableBase && MmIsAddressValid( pWIN32KEntry->ServiceTableBase ) )
	{
		_impSetSystemNotifiers_Shadow();
		ShadowTableVerifier_Init();
		Win32kVerifier_Init();

		g_bShadowSystemNotifiers_Inited = TRUE;
	}

	KeUnstackDetachProcess( &APCState );
}

VOID TryInitShadowTablePtr()
{
	// check if patching is enabled
	if ( !FlagOn( Globals.m_DrvFlags, _DRV_FLAG_HAS_SYSPATCH ) )
		return;
	// check OS version
	if ( !( g_bIsW2003 || g_bIsW2K || g_bIsWXP ) )
		return;

	// do not reenter
	if ( !InterlockedExchange( &g_bInTryInitShadowTablePtr, TRUE ) ) 
	{
		if ( !g_pShadowSDT )
		{
			PEPROCESS pCSRSSProc;

			pCSRSSProc = LookupProcessByName( L"csrss.exe" );
			if ( pCSRSSProc )
			{
				KAPC_STATE APCState;

				KeStackAttachProcess(
					pCSRSSProc,
					&APCState
					);

				g_pShadowSDT = GetAddrOfShadowTable();
				if ( g_pShadowSDT )
				{
					g_pCSRSSProc = pCSRSSProc;
					ObReferenceObject( g_pCSRSSProc );
				}

				KeUnstackDetachProcess( &APCState );

				ObDereferenceObject( pCSRSSProc );
			}
		}

		if ( g_pShadowSDT )
			SetSystemNotifiers_Shadow();

		g_bInTryInitShadowTablePtr = FALSE;
	}
}

VOID ShadowTableEvent_Common(
							 __in ULONG EventCode,
							 __in ULONG KeyloggerID
							 )
{
	PFILTER_EVENT_PARAM	pParam;

	pParam = (PFILTER_EVENT_PARAM) ExAllocatePoolWithTag(
		PagedPool,
		0x200,
		tag_event);
	if ( pParam )
	{
		PSINGLE_PARAM pSingleParam;

		FILTER_PARAM_COMMONINIT(
			pParam,
			FLTTYPE_SYSTEM,
			EventCode,
			0,
			PostProcessing,
			1
			);
		pParam->ProcessID = -1;
		pParam->ThreadID = -1;
		pSingleParam = (PSINGLE_PARAM)pParam->Params;

		SINGLE_PARAM_INIT_ULONG(
			pSingleParam,
			_PARAM_OBJECT_KEYLOGGERID,
			KeyloggerID
			);
		SINGLE_PARAM_SHIFT( pSingleParam );

		FilterEventImp(
			pParam,
			NULL,
			0
			);

		ExFreePool( pParam );
	}
}

VOID ShadowTable_Check()
{
	BOOLEAN bGetMessagePatched = FALSE;
	BOOLEAN bPeekMessagePatched = FALSE;
	PVOID fNewGetMessage = NULL;
	PVOID fNewPeekMessage = NULL;
	KAPC_STATE APCState;

	if ( !g_bShadowSystemNotifiers_Inited )
		return;

	KeStackAttachProcess(
		g_pCSRSSProc,
		&APCState
		);
	//--------------------------------------------------------------------------------
	__try
	{
		bGetMessagePatched = g_NtUserGetMessage_Service.m_fPointerOnStart != *g_NtUserGetMessage_Service.m_pfServiceFunc;
		bGetMessagePatched |= g_pWin32kBase &&
			( (ULONG_PTR) g_pWin32kBase > (ULONG_PTR) *g_NtUserGetMessage_Service.m_pfServiceFunc || ( (ULONG_PTR) g_pWin32kBase + g_Win32kSize ) <= (ULONG_PTR) *g_NtUserGetMessage_Service.m_pfServiceFunc );

		bPeekMessagePatched = g_NtUserPeekMessage_Service.m_fPointerOnStart != *g_NtUserPeekMessage_Service.m_pfServiceFunc;
		bPeekMessagePatched |= g_pWin32kBase &&
			( (ULONG_PTR) g_pWin32kBase > (ULONG_PTR) *g_NtUserPeekMessage_Service.m_pfServiceFunc || ( (ULONG_PTR) g_pWin32kBase + g_Win32kSize ) <= (ULONG_PTR) *g_NtUserPeekMessage_Service.m_pfServiceFunc );

		fNewGetMessage = *g_NtUserGetMessage_Service.m_pfServiceFunc;
		fNewPeekMessage = *g_NtUserPeekMessage_Service.m_pfServiceFunc;
	}
	__except(EXCEPTION_EXECUTE_HANDLER)
	{
		_dbg_break_;
		DoTraceEx(
			TRACE_LEVEL_ERROR,
			DC_SYSTEM,
			"memory access fault on GetMessage/PeekMessage."
			);
	}
	//--------------------------------------------------------------------------------		
	KeUnstackDetachProcess( &APCState );

	if ( bGetMessagePatched )
	{
		ShadowTableEvent_Common(
			0x14/*MJ_SYSTEM_KEYLOGGER_DETECTED*/,
			KEYLOGGER_GETMESSAGE_PATCH
			);
		DoTraceEx(
			TRACE_LEVEL_INFORMATION,
			DC_SYSTEM,
			"keylogger: shadow table patched on NtUserGetMessage."
			);
	}

	if ( bPeekMessagePatched )
	{
		ShadowTableEvent_Common(
			0x14/*MJ_SYSTEM_KEYLOGGER_DETECTED*/,
			KEYLOGGER_PEEKMESSAGE_PATCH
			);
		DoTraceEx(
			TRACE_LEVEL_INFORMATION,
			DC_SYSTEM,
			"keylogger: shadow table patched on NtUserPeekMessage."
			);
	}
}

VOID KbdSpliceCheck_ReInit()
{
	BOOLEAN bDevicesChanged;

	bDevicesChanged = KbdAttachCheck_ReInit();
	if ( !g_bKbdAttachCheck_Inited )
	{
		g_bKbdSpliceCheck_Inited = FALSE;
		g_fKbdReadDispatch = NULL;
		g_pfKbdReadDispatch = NULL;
		RtlZeroMemory(
			g_rKbdReadDispatch_Prologue,
			sizeof(g_rKbdReadDispatch_Prologue)
			);

		return;
	}

	if ( bDevicesChanged || !g_bKbdSpliceCheck_Inited )
	{
		g_bKbdSpliceCheck_Inited = FALSE;
		g_fKbdReadDispatch = NULL;
		g_pfKbdReadDispatch = NULL;
		RtlZeroMemory(
			g_rKbdReadDispatch_Prologue,
			sizeof(g_rKbdReadDispatch_Prologue)
			);

		if ( g_pKbdClassDevice->DriverObject && g_pKbdClassDevice->DriverObject->MajorFunction[IRP_MJ_READ] )
		{
			g_fKbdReadDispatch = g_pKbdClassDevice->DriverObject->MajorFunction[IRP_MJ_READ];
			g_pfKbdReadDispatch = (PVOID *) &g_pKbdClassDevice->DriverObject->MajorFunction[IRP_MJ_READ];
			memcpy(
				g_rKbdReadDispatch_Prologue,
				g_fKbdReadDispatch,
				KBDREAD_PROLOGUE_SIZE
				);

			g_bKbdSpliceCheck_Inited = TRUE;
		}
	}

	DoTraceEx(
		TRACE_LEVEL_INFORMATION,
		DC_SYSTEM,
		"KbdSpliceCheck_ReInit:: g_bKbdSpliceCheck_Inited = %!BOOLEAN!, ReInit = %!BOOLEAN!",
		g_bKbdSpliceCheck_Inited,
		bDevicesChanged
		);
}

VOID KbdSplice_Check()
{
	KbdSpliceCheck_ReInit();

	if ( g_bKbdSpliceCheck_Inited )
	{
		PVOID ptr = *g_pfKbdReadDispatch;

		if (  g_fKbdReadDispatch != ptr ||
			  0 != memcmp(
				g_fKbdReadDispatch,
				g_rKbdReadDispatch_Prologue,
				KBDREAD_PROLOGUE_SIZE
				)
			)
		{
			PFILTER_EVENT_PARAM	pParam;

			pParam = (PFILTER_EVENT_PARAM) ExAllocatePoolWithTag(
				PagedPool,
				0x200,
				tag_event
				);
			if ( pParam )
			{
				PSINGLE_PARAM pSingleParam;

				FILTER_PARAM_COMMONINIT(
					pParam,
					FLTTYPE_SYSTEM,
					0x14/*MJ_SYSTEM_KEYLOGGER_DETECTED*/,
					0,
					PostProcessing,
					1
					);
				pParam->ProcessID = -1;
				pParam->ThreadID = -1;
				pSingleParam = (PSINGLE_PARAM)pParam->Params;

				SINGLE_PARAM_INIT_ULONG(
					pSingleParam,
					_PARAM_OBJECT_KEYLOGGERID,
					KEYLOGGER_SPLICEONREADDISPATCH
					);
				SINGLE_PARAM_SHIFT( pSingleParam );

				FilterEventImp(
					pParam,
					NULL,
					0
					);

				ExFreePool( pParam );
			}

			DoTraceEx(
				TRACE_LEVEL_INFORMATION,
				DC_SYSTEM,
				"KbdSplice_Check:: kbdclass read dispatch handler is spliced"
				);
		}
	}
}
//------------------------------------------------------------------------------------------------
#endif

BOOLEAN KbdAttachCheck_ReInit()
{
	NTSTATUS status;
	ULONG i;
	UNICODE_STRING usPortDriverName;
	PDRIVER_OBJECT pi8042Driver = NULL;
	PDRIVER_OBJECT pHidDriver = NULL;
	PDEVICE_OBJECT pKbdPortDevice_Old;
	PDEVICE_OBJECT pKbdClassDevice_Old;

	if ( !fIoGetLowerDeviceObject )
		return FALSE;

	pKbdPortDevice_Old = g_pKbdPortDevice;
	pKbdClassDevice_Old = g_pKbdClassDevice;

	if ( g_pKbdPortDevice )
	{
		ObDereferenceObject( g_pKbdPortDevice );
		ObDereferenceObject( g_pKbdClassDevice);

		g_pKbdPortDevice = NULL;
		g_pKbdClassDevice = NULL;
	}

	RtlInitUnicodeString(
		&usPortDriverName,
		L"\\Driver\\i8042prt"
		);
	status = ObReferenceObjectByName(
		&usPortDriverName,
		OBJ_CASE_INSENSITIVE,
		NULL,
		0,
		IoDriverObjectType,
		KernelMode,
		NULL,
		&pi8042Driver
		);
	if ( !NT_SUCCESS( status ) )
		pi8042Driver = NULL;

	RtlInitUnicodeString(
		&usPortDriverName,
		L"\\Driver\\kbdhid"
		);
	status = ObReferenceObjectByName(
		&usPortDriverName,
		OBJ_CASE_INSENSITIVE,
		NULL,
		0,
		IoDriverObjectType,
		KernelMode,
		NULL,
		&pHidDriver
		);
	if ( !NT_SUCCESS( status ) )
		pHidDriver = NULL;

	for ( i = 0; i < 10; i++ )
	{
		UNICODE_STRING usKbdClassStr;
		WCHAR wcKbdClassStr[128];
		PDEVICE_OBJECT pKbdClassDevice;

		swprintf(
			wcKbdClassStr,
			L"\\Device\\KeyboardClass%d",
			i
			);
		RtlInitUnicodeString(
			&usKbdClassStr,
			wcKbdClassStr
			);
		status = ReferenceBaseDeviceByName(
			&usKbdClassStr,
			&pKbdClassDevice
			);
		if ( NT_SUCCESS( status ) )
		{
			PDEVICE_OBJECT pLowerDevice;

			pLowerDevice = fIoGetLowerDeviceObject( pKbdClassDevice );
			while ( pLowerDevice )
			{
				if ( pLowerDevice->DriverObject )
				{
					if ( ( pi8042Driver && pi8042Driver == pLowerDevice->DriverObject ) ||
						 ( pHidDriver && pHidDriver == pLowerDevice->DriverObject ) )
					{
						g_pKbdClassDevice = pKbdClassDevice;
						g_pKbdPortDevice = pLowerDevice;

						break;
					}
				}

				ObDereferenceObject( pLowerDevice );
				pLowerDevice = fIoGetLowerDeviceObject( pLowerDevice );
			}

			if ( g_pKbdClassDevice )
				break;
			else
				ObDereferenceObject( pKbdClassDevice );
		}
	}

	if ( pi8042Driver )
		ObDereferenceObject( pi8042Driver );
	if ( pHidDriver )
		ObDereferenceObject( pHidDriver );

	g_bKbdAttachCheck_Inited = FALSE;
	if ( !g_pKbdClassDevice )
	{
		DoTraceEx(
			TRACE_LEVEL_ERROR,
			DC_SYSTEM,
			"KbdAttachCheck_ReInit:: reinit failed."
			);
	}
	else
	{
		g_bKbdAttachCheck_Inited = TRUE;

		DoTraceEx(
			TRACE_LEVEL_INFORMATION,
			DC_SYSTEM,
			"KbdAttachCheck_ReInit:: reinited."
			);
	}

	if ( g_pKbdPortDevice && pKbdPortDevice_Old != g_pKbdPortDevice )
		return TRUE;
	if ( g_pKbdClassDevice && pKbdClassDevice_Old != g_pKbdClassDevice )
		return TRUE;

	return FALSE;
}


VOID KbdAttach_BuildEvent(
	__inout PFILTER_EVENT_PARAM pParam,
	__in PDEVICE_OBJECT pDeviceObject
	)
{
	PSINGLE_PARAM pSingleParam;
	PWCHAR wcDriverName = NULL;
	ULONG cbDriverName = 0;

	if ( pDeviceObject->DriverObject )
	{
		wcDriverName = Sys_QueryObjectName( pDeviceObject->DriverObject );
		if ( wcDriverName )
			cbDriverName = sizeof(WCHAR) * ( wcslen( wcDriverName ) + 1 );
	}

	FILTER_PARAM_COMMONINIT(
		pParam,
		FLTTYPE_SYSTEM,
		0x14/*MJ_SYSTEM_KEYLOGGER_DETECTED*/,
		0,
		PostProcessing,
		( wcDriverName ? 1 : 0 ) + 1
		);
	pParam->ProcessID = -1;
	pParam->ThreadID = -1;
	pSingleParam = (PSINGLE_PARAM) pParam->Params;

	SINGLE_PARAM_INIT_ULONG(
		pSingleParam,
		_PARAM_OBJECT_KEYLOGGERID,
		KEYLOGGER_ATTACHKBD
		);
	SINGLE_PARAM_SHIFT(pSingleParam);

	SINGLE_PARAM_INIT_NONE(
		pSingleParam,
		_PARAM_OBJECT_URL_W
		);
	pSingleParam->ParamSize = cbDriverName;
	memcpy(
		pSingleParam->ParamValue,
		wcDriverName,
		cbDriverName
		);

	DoTraceEx(
		TRACE_LEVEL_INFORMATION,
		DC_SYSTEM,
		"KbdAttach_BuildEvent:: driver name = %S",
		wcDriverName
		);

	if ( wcDriverName )
		ExFreePool( wcDriverName );
}

VOID KbdAttach_Check()
{
	PFILTER_EVENT_PARAM	pParam;
	PDEVICE_OBJECT pCurrDevice;

	KbdAttachCheck_ReInit();
	if ( !g_bKbdAttachCheck_Inited )
		return;

	pParam = (PFILTER_EVENT_PARAM) ExAllocatePoolWithTag(
		PagedPool,
		0x1000,
		tag_event
		);
	if ( !pParam )
		return;

	// идем от порт-девайса вверх по стеку до kbdclass
	pCurrDevice = g_pKbdPortDevice;
	while ( pCurrDevice->AttachedDevice && g_pKbdClassDevice != pCurrDevice->AttachedDevice )
	{
		KbdAttach_BuildEvent(
			pParam,
			pCurrDevice->AttachedDevice
			);
		FilterEventImp(
			pParam,
			NULL,
			0
			);

		pCurrDevice = pCurrDevice->AttachedDevice;
	}

	// идем от девайса kbdclass вверх по стеку до вершины
	pCurrDevice = g_pKbdClassDevice;
	while ( pCurrDevice->AttachedDevice )
	{
		KbdAttach_BuildEvent(
			pParam,
			pCurrDevice->AttachedDevice
			);
		FilterEventImp(
			pParam,
			NULL,
			0
			);

		pCurrDevice = pCurrDevice->AttachedDevice;
	}

	ExFreePool( pParam );
}

VOID KeyLoggers_CommonCheck()
{
	DoTraceEx(
		TRACE_LEVEL_INFORMATION,
		DC_SYSTEM,
		"KeyLoggers_CommonCheck:: start..."
		);

#ifndef _WIN64
	ShadowTable_Check();
	KbdSplice_Check();
#endif
	KbdAttach_Check();

	DoTraceEx(
		TRACE_LEVEL_INFORMATION,
		DC_SYSTEM,
		"KeyLoggers_CommonCheck:: complete."
		);
}
