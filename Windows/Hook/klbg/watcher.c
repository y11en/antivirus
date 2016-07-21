#define __WATCHER_C

#include <fltkernel.h>
#include <ntstrsafe.h>
#include "md5.h"

#include "watcher.h"
#include "main.h"

extern PFLT_FILTER g_pFilterObj;
extern LONG g_SysEnters;

ERESOURCE g_erServiceListLock;
LIST_ENTRY g_ServiceListHead;

PWCHAR SkipUnicodePrefix( 
						 __in PWCHAR wcPathStr,
						 __in ULONG cbInput,
						 __out PULONG p_cbOutput
						 )
{
	if ( p_cbOutput )
		*p_cbOutput = cbInput;

	if ( cbInput < sizeof(WCHAR) * 4 )
		return wcPathStr;

	if ( 0 == wcsncmp( wcPathStr, L"\\\\?\\", 4 ) )
	{
		if ( p_cbOutput )
			*p_cbOutput = cbInput - sizeof(WCHAR) * 4;
		return wcPathStr + 4;
	}

	return wcPathStr;
}

BOOLEAN ProcessServiceRecord(
	  __in HANDLE hRoot,
	  __in PKEY_BASIC_INFORMATION pKeyInfo
	  )
{
	BOOLEAN bResult = FALSE;
	NTSTATUS status;
	UNICODE_STRING usKeyName;
	OBJECT_ATTRIBUTES KeyOA;
	HANDLE hKey;
	UNICODE_STRING usValueName;
	PKEY_VALUE_PARTIAL_INFORMATION QueryBuffer = NULL;

	usKeyName.Buffer = pKeyInfo->Name;
	usKeyName.Length = (USHORT)pKeyInfo->NameLength;
	usKeyName.MaximumLength = usKeyName.Length;
	InitializeObjectAttributes(
		&KeyOA,
		&usKeyName,
		OBJ_CASE_INSENSITIVE | OBJ_KERNEL_HANDLE,
		hRoot,
		NULL
		);
	status = ZwOpenKey(&hKey, KEY_QUERY_VALUE, &KeyOA);
	if ( !NT_SUCCESS(status))
		return FALSE;

	QueryBuffer = (PKEY_VALUE_PARTIAL_INFORMATION) ExAllocatePoolWithTag(
		PagedPool,
		0x1000,
		'BQrg');
	if ( QueryBuffer )
	{
		ULONG ulRetLen;
		ULONG Type = -1L, Start = -1L;
		ULONG cbImagePath = 0;
		PWCHAR wcImagePath = NULL;
		PSERVICE_ENUM_ENTRY pEntry = NULL;

		RtlInitUnicodeString(&usValueName, L"Type");
		status = ZwQueryValueKey(
			hKey,
			&usValueName,
			KeyValuePartialInformation,
			(PVOID)QueryBuffer,
			0x1000,
			&ulRetLen
			);
		if ( NT_SUCCESS(status) && REG_DWORD == QueryBuffer->Type )
			Type = *(PULONG)(QueryBuffer->Data);

		RtlInitUnicodeString(&usValueName, L"Start");
		status = ZwQueryValueKey(
			hKey,
			&usValueName,
			KeyValuePartialInformation,
			(PVOID)QueryBuffer,
			0x1000,
			&ulRetLen
			);
		if ( NT_SUCCESS(status) && REG_DWORD == QueryBuffer->Type )
			Start = *(PULONG)(QueryBuffer->Data);

		RtlInitUnicodeString(&usValueName, L"ImagePath");
		status = ZwQueryValueKey(
			hKey,
			&usValueName,
			KeyValuePartialInformation,
			(PVOID)QueryBuffer,
			0x1000,
			&ulRetLen
			);
		if ( NT_SUCCESS(status) && (REG_SZ == QueryBuffer->Type || REG_EXPAND_SZ == QueryBuffer->Type) )
		{
			cbImagePath = QueryBuffer->DataLength;
			wcImagePath = (PWCHAR)QueryBuffer->Data;
			SkipUnicodePrefix( wcImagePath, cbImagePath, &cbImagePath);
		}

		pEntry = (PSERVICE_ENUM_ENTRY) ExAllocatePoolWithTag(
			PagedPool,
			sizeof(SERVICE_ENUM_ENTRY) + usKeyName.Length + sizeof(WCHAR) + cbImagePath + sizeof(WCHAR),
			'ESrg');
		if (pEntry)
		{
			PWCHAR wcTargetImagePath = NULL;

			RtlZeroMemory( 
				pEntry,
				sizeof( SERVICE_ENUM_ENTRY )
				);
			pEntry->m_Type = Type;
			pEntry->m_Start = Start;
			memcpy(
				pEntry->m_ServiceName,
				usKeyName.Buffer,
				usKeyName.Length);
			pEntry->m_ServiceName[usKeyName.Length / sizeof(WCHAR)] = 0;
			pEntry->m_ImagePathOffset = usKeyName.Length + sizeof(WCHAR);
			wcTargetImagePath = (PWCHAR) ((PCHAR) pEntry->m_ServiceName + pEntry->m_ImagePathOffset);
			if (wcImagePath)
			{
				memcpy(
					wcTargetImagePath,
					wcImagePath,
					cbImagePath);
			}
			wcTargetImagePath[cbImagePath / sizeof(WCHAR)] = 0;
			cbImagePath += sizeof(WCHAR);

			pEntry->m_cbServiceName = pEntry->m_ImagePathOffset + cbImagePath;

			AcquireResourceExclusive(&g_erServiceListLock);
			InsertTailList(&g_ServiceListHead, (PLIST_ENTRY)pEntry);
			ReleaseResource(&g_erServiceListLock);

			bResult = TRUE;
		}

		ExFreePool(QueryBuffer);
	}

	ZwClose(hKey);

	return bResult;
}

NTSTATUS EnumServices()
{
	NTSTATUS status;
	HANDLE hServices;
	UNICODE_STRING usServicesPath;
	OBJECT_ATTRIBUTES ServicesOA;
	PKEY_BASIC_INFORMATION pEnumInfo = NULL;
	ULONG ulRetLen = 0;
	ULONG i;
	
	RtlInitUnicodeString(&usServicesPath, L"\\registry\\machine\\system\\currentcontrolset\\services");
	InitializeObjectAttributes(
		&ServicesOA,
		&usServicesPath,
		OBJ_CASE_INSENSITIVE | OBJ_KERNEL_HANDLE,
		NULL,
		NULL);
	status = ZwOpenKey(
		&hServices,
		KEY_ENUMERATE_SUB_KEYS,
		&ServicesOA
		);
	if ( !NT_SUCCESS(status) )
		return status;

	pEnumInfo = (PKEY_BASIC_INFORMATION) ExAllocatePoolWithTag(
		PagedPool,
		0x300,
		'SErg');
	if ( pEnumInfo )
	{
		status = STATUS_SUCCESS;
		for ( i = 0; status != STATUS_NO_MORE_ENTRIES; i++ )
		{
			status = ZwEnumerateKey(
				hServices,
				i,
				KeyBasicInformation,
				pEnumInfo,
				0x300,
				&ulRetLen);
			if ( !NT_SUCCESS(status) )
			{
				if ( status != STATUS_NO_MORE_ENTRIES )
					_dbg_break_;
			}
			else
			{
				// ok
				ProcessServiceRecord(hServices, pEnumInfo);
			}
		}

		ExFreePool(pEnumInfo);
	}

	ZwClose(hServices);

	if ( STATUS_NO_MORE_ENTRIES == status )
		status = STATUS_SUCCESS;

	return status;
}

VOID ServiceEnumInit()
{
	InitializeListHead( &g_ServiceListHead );
	ExInitializeResourceLite( &g_erServiceListLock );
}

VOID ServiceEnumCleanup()
{
	PLIST_ENTRY pEntry = NULL;

	pEntry = g_ServiceListHead.Flink;
	while ( pEntry != &g_ServiceListHead )
	{
		PLIST_ENTRY pNext = pEntry->Flink;
		ExFreePool(pEntry);
		pEntry = pNext;
	}

	InitializeListHead( &g_ServiceListHead );

	ExDeleteResourceLite( &g_erServiceListLock );
}

BOOLEAN TranslateImagePathService(
	__in ULONG ServiceType,
	__in PWCHAR OrigImagePath,
	__out PWCHAR TranslatedPath,
	__in ULONG cchBuffer
	)
{
	NTSTATUS status;

	TranslatedPath[0] = 0;

	if ( ServiceType < 0x10 )
	{
		// driver
		// system32\drivers\... -->> \systemroot\system32\drivers\...
		// \??\c:\drivers\... -->> \??\c:\drivers\...
		// c:\drivers\... not valid
		if ( L'\\' != OrigImagePath[0] )
		{
			status = RtlStringCchCopyW(TranslatedPath, cchBuffer, L"\\SystemRoot\\");
			if ( !NT_SUCCESS(status))
				return FALSE;
		}

		status = RtlStringCchCatW(TranslatedPath, cchBuffer, OrigImagePath);
		if ( !NT_SUCCESS(status) )
			return FALSE;

		return TRUE;
	}
	else
	{
		// exe service
		status = RtlStringCchCopyW(TranslatedPath, cchBuffer, OrigImagePath);
		if ( !NT_SUCCESS(status) )
			return FALSE;

		return TRUE;
	}

	return FALSE;
}

BOOLEAN FileComputeMd5(__in HANDLE hFile, __in PFLT_INSTANCE pInstance, __out UCHAR Md5Out[16])
{
	BOOLEAN bResult = FALSE;
	NTSTATUS status;
	PCHAR pMd5Buffer;

#define MD5_BLOCK_SIZE	0x10000

	pMd5Buffer = (PCHAR) ExAllocatePoolWithTag(
		PagedPool,
		MD5_BLOCK_SIZE,
		'DMrg');

	if ( pMd5Buffer )
	{
		PFILE_OBJECT pFileObject;

		status = ObReferenceObjectByHandle(
			hFile,
			0,
			*IoFileObjectType,
			KernelMode,
			(PVOID *) &pFileObject,
			NULL
			);
		if ( NT_SUCCESS(status) )
		{
			ULONG cbRead;
			md5_state_t md5state;

			md5_init(&md5state);

			do 
			{
				status = FltReadFile(
					pInstance,
					pFileObject,
					NULL,
					MD5_BLOCK_SIZE,
					pMd5Buffer,
					0,
					&cbRead,
					NULL,
					NULL
					);
				if ( NT_SUCCESS(status) && cbRead )
					md5_append( &md5state, pMd5Buffer, cbRead );

			} while( NT_SUCCESS(status) && MD5_BLOCK_SIZE == cbRead );

			md5_finish(&md5state, Md5Out);

			bResult = NT_SUCCESS(status);

			ObDereferenceObject( pFileObject );
		}

		ExFreePool(pMd5Buffer);
	}

	return bResult;
}

VOID ComputeMd5OnServiceImages()
{
	PLIST_ENTRY _pEntry = NULL;

	AcquireResourceShared(&g_erServiceListLock);

	_pEntry = g_ServiceListHead.Flink;
	while ( _pEntry != &g_ServiceListHead )
	{
		PSERVICE_ENUM_ENTRY pEntry = (PSERVICE_ENUM_ENTRY) _pEntry;
		PWCHAR wcOrigImagePath = (PWCHAR)((PCHAR)pEntry->m_ServiceName + pEntry->m_ImagePathOffset);
		
		if ( !pEntry->m_Md5Computed )
		{
			if ( 0 != wcOrigImagePath[0] && -1L != pEntry->m_Type )
			{
				PWCHAR wcTranslatedPath;

				wcTranslatedPath = (PWCHAR) ExAllocatePoolWithTag(PagedPool, 0x300, 'BTrg');
				if ( wcTranslatedPath )
				{
					BOOLEAN bResult;

					bResult = TranslateImagePathService(
						pEntry->m_Type,
						wcOrigImagePath,
						wcTranslatedPath,
						0x300 / sizeof(WCHAR)
						);
					if (bResult)
					{
						NTSTATUS status;
						UNICODE_STRING usServiceFile;
						PFLT_INSTANCE pInstance;

						RtlInitUnicodeString(
							&usServiceFile,
							wcTranslatedPath);

						pInstance = GetInstanceForFile( &usServiceFile );
						if ( pInstance )
						{
							OBJECT_ATTRIBUTES ServiceFileOA;
							IO_STATUS_BLOCK ioStatus;
							HANDLE hServiceFile = NULL;

							InitializeObjectAttributes(
								&ServiceFileOA,
								&usServiceFile,
								OBJ_CASE_INSENSITIVE | OBJ_KERNEL_HANDLE,
								NULL,
								NULL);

							status = FltCreateFile(
								g_pFilterObj,
								pInstance,
								&hServiceFile,
								SYNCHRONIZE | FILE_READ_DATA,
								&ServiceFileOA,
								&ioStatus,
								NULL,
								FILE_ATTRIBUTE_NORMAL,
								FILE_SHARE_READ | FILE_SHARE_WRITE | FILE_SHARE_DELETE,
								FILE_OPEN,
								FILE_SYNCHRONOUS_IO_NONALERT,
								NULL,
								0,
								0
								);
							if ( NT_SUCCESS(status) )
							{
								UCHAR Md5[16];

								bResult = FileComputeMd5(hServiceFile, pInstance, Md5);
								if ( bResult )
								{
									ExReleaseResourceLite( &g_erServiceListLock );
									ExAcquireResourceExclusiveLite( &g_erServiceListLock, TRUE );

									memcpy( pEntry->m_Md5Hash, Md5, sizeof(Md5) );
									pEntry->m_Md5Computed = TRUE;

									ExConvertExclusiveToSharedLite( &g_erServiceListLock );
								}



								FltClose( hServiceFile );
							}

							FltObjectDereference( pInstance );
						}
					}

					ExFreePool(wcTranslatedPath);
				}
			}
		}

		_pEntry = _pEntry->Flink;
	}

	ReleaseResource(&g_erServiceListLock);
}
