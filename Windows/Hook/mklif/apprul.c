#include "apprul.h"
#include "../hook/Funcs.h"

#include "./inc/hips_tools.h"

#include "apprul.tmh"

LIST_ENTRY	gAppRulList;
ERESOURCE	gAppRulListLock;

PPID_TREE gpPidTree = NULL;
ULONG	  gPidTreeRemCnt = 0;

LONGLONG gRulID = 0;

//находим последний элемент блока
PLIST_ENTRY
FindLastBockElemetRulHeadList (
	__in PLIST_ENTRY pAppRulHead,
	__in ULONG blockID
	)
{
	PLIST_ENTRY Flink;
	PAPP_RUL_ENTRY papp_rul_entry;
	
	if ( IsListEmpty( pAppRulHead ) )
		return	pAppRulHead;

	Flink = pAppRulHead->Flink;
	while ( Flink != pAppRulHead )
	{
		papp_rul_entry = CONTAINING_RECORD( Flink, APP_RUL_ENTRY, ListEntry );
				
		if ( papp_rul_entry->app_rul.rul_info.blockID > blockID )
			break;
		
		Flink = Flink->Flink;
	}

	return Flink->Blink;
}

NTSTATUS
InsertAppRulHeadListEx (
	__in PLIST_ENTRY pAppRulHead,
	__in ULONG clientID,
	__in UNICODE_STRING usNativeAppPath,
	__in UNICODE_STRING usNativeFilePath,
	__in UNICODE_STRING usValueName,
	__in PVOID	pHash,
	__in ULONG	HashSize,
	__in ULONG AccessMask,
	__in LONGLONG RulID,
	__in ULONG blockID
	)
{
	PAPP_RUL_ENTRY papp_rul_entry = NULL;
	ULONG	AppPathLen = 0;
	ULONG	FilePathLen = 0;
	ULONG writeRight = 0, readRight = 0, enumRight = 0, deleteRight = 0;
	ULONG writeLog = 0, readLog = 0, enumLog = 0, deleteLog = 0;
	NTSTATUS status = STATUS_UNSUCCESSFUL;

	__try
	{
		papp_rul_entry = ExAllocatePoolWithTag( PagedPool, sizeof (APP_RUL_ENTRY), tag_apprul2 );
		if (!papp_rul_entry)
		{
			status = STATUS_NO_MEMORY;
			__leave;
		}

		RtlZeroMemory( papp_rul_entry, sizeof (APP_RUL_ENTRY) );

		if (!usNativeFilePath.Length)
		{
			status = STATUS_INVALID_PARAMETER_4;
			__leave;
		}

		if (!usNativeAppPath.Length && (!pHash || !HashSize))
		{
			status = STATUS_UNSUCCESSFUL;
			__leave;
		}

		if (!pHash && HashSize > 0)
		{
			status = STATUS_INVALID_PARAMETER_5;
			__leave;
		}

		if (HashSize > 0 &&  HashSize != sizeof (md5_byte_t) * 16)
		{
			status = STATUS_INVALID_PARAMETER_6;
			__leave;
		}

		if (usNativeAppPath.Length > 0)
		{
			PWCHAR pwchNativeAppPath = ExAllocatePoolWithTag( PagedPool, usNativeAppPath.Length, tag_apprul9 );
			if (!pwchNativeAppPath)
			{
				status = STATUS_NO_MEMORY;
				__leave;
			}
	
			RtlInitEmptyUnicodeString (
				&papp_rul_entry->app_rul.appl_info.usPath,
				pwchNativeAppPath,
				usNativeAppPath.Length
				);

			RtlUpcaseUnicodeString( &papp_rul_entry->app_rul.appl_info.usPath, &usNativeAppPath,FALSE );
		}

		if (usNativeFilePath.Length > 0)
		{
			PWCHAR pwchNativeFilePath = ExAllocatePoolWithTag( PagedPool, usNativeFilePath.Length, tag_apprulA );
			if ( !pwchNativeFilePath )
			{
				status = STATUS_NO_MEMORY;
				__leave;
			}

			RtlInitEmptyUnicodeString (
				&papp_rul_entry->app_rul.rul_info.usDeviceName, 
				pwchNativeFilePath, 
				usNativeFilePath.Length
				);

			RtlUpcaseUnicodeString( &papp_rul_entry->app_rul.rul_info.usDeviceName, &usNativeFilePath, FALSE );
		}

		if (usValueName.Length > 0)
		{
			PWCHAR pwchValueName = ExAllocatePoolWithTag( PagedPool, usValueName.Length, tag_apprulD );
			if (!pwchValueName)
			{
				status = STATUS_NO_MEMORY;
				__leave;
			}
			
			RtlInitEmptyUnicodeString (
				&papp_rul_entry->app_rul.rul_info.usValueName,
				pwchValueName,
				usValueName.Length
				);

			RtlUpcaseUnicodeString( &papp_rul_entry->app_rul.rul_info.usValueName, &usValueName, FALSE );
		}

		papp_rul_entry->clientID = clientID;

		if (HashSize > 0 && pHash)
			memcpy( papp_rul_entry->app_rul.appl_info.digest, pHash, HashSize );

		papp_rul_entry->app_rul.rul_info.mask = AccessMask;
		papp_rul_entry->app_rul.rul_info.blockID = blockID;
		papp_rul_entry->RulID = RulID;

		InsertHeadList (
			FindLastBockElemetRulHeadList( pAppRulHead, blockID ),
			&papp_rul_entry->ListEntry
			);

		DoTraceEx (
			TRACE_LEVEL_INFORMATION, DC_HIPS,
			"InsertAppRulHeadListEx ApplicationPath = '%wZ'",
			&usNativeAppPath
			);

		DoTraceEx (
			TRACE_LEVEL_INFORMATION, DC_HIPS,
			"InsertAppRulHeadListEx FilePath = '%wZ'",
			&usNativeFilePath
			);

		if (usValueName.Length > 0)
		{
			DoTraceEx (
				TRACE_LEVEL_INFORMATION, DC_HIPS,
				"InsertAppRulHeadListEx ValueName = '%wZ'",
				&usValueName
				);
		}

		DoTraceEx (
			TRACE_LEVEL_INFORMATION, DC_HIPS,
			"InsertAppRulHeadListEx rulId = %I64x, blockID = %d",
			RulID,
			blockID
			);
		
		writeRight = HIPS_GET_AM ( AccessMask, HIPS_POS_WRITE );
		readRight = HIPS_GET_AM ( AccessMask, HIPS_POS_READ );
		enumRight = HIPS_GET_AM ( AccessMask, HIPS_POS_ENUM );
		deleteRight = HIPS_GET_AM ( AccessMask, HIPS_POS_DELETE );

		writeLog = HIPS_GET_LOG ( AccessMask, HIPS_POS_WRITE );
		readLog = HIPS_GET_LOG ( AccessMask, HIPS_POS_READ );
		enumLog = HIPS_GET_LOG ( AccessMask, HIPS_POS_ENUM );
		deleteLog = HIPS_GET_LOG ( AccessMask, HIPS_POS_DELETE );

		DoTraceEx (
			TRACE_LEVEL_INFORMATION, DC_HIPS,
			"InsertAppRulHeadListEx AccessMask =%d (wR=%d, rR=%d, eR=%d, dR=%d)",
			AccessMask,
			writeRight,
			readRight,
			enumRight,
			deleteRight
			);

		DoTraceEx (
			TRACE_LEVEL_INFORMATION, DC_HIPS,
			"InsertAppRulHeadListEx LogMask (wR=%d, rR=%d, eR=%d, dR=%d)",
			writeLog,
			readLog,
			enumLog,
			deleteLog
			);
		
		status = STATUS_SUCCESS;
	}
	__finally
	{
		if (!NT_SUCCESS( status ))
		{
			if (papp_rul_entry)
			{
				if (papp_rul_entry->app_rul.appl_info.usPath.Buffer)
					ExFreePool ( papp_rul_entry->app_rul.appl_info.usPath.Buffer );

				if (papp_rul_entry->app_rul.rul_info.usDeviceName.Buffer)
					ExFreePool ( papp_rul_entry->app_rul.rul_info.usDeviceName.Buffer );

				if (papp_rul_entry->app_rul.rul_info.usValueName.Buffer)
					ExFreePool ( papp_rul_entry->app_rul.rul_info.usValueName.Buffer );

				ExFreePool( papp_rul_entry );
			}
		}
	}
	
	return status;
}

NTSTATUS
InsertAppRuleToList (
	__in ULONG clientID,
	__in UNICODE_STRING usNativeAppPath,
	__in UNICODE_STRING usNativeFilePath,
	__in UNICODE_STRING usValueName,
	__in PVOID	pHash,
	__in ULONG	HashSize,
	__in ULONG AccessMask,
	__inout PLONGLONG pRulID,
	__in ULONG blockID
	)
{
	NTSTATUS status;
	
	AcquireResourceExclusive( &gAppRulListLock );
	
	gRulID++;

	status = InsertAppRulHeadListEx (
		&gAppRulList,
		clientID,
		usNativeAppPath,
		usNativeFilePath,
		usValueName,
		pHash,
		HashSize,
		AccessMask,
		gRulID,
		blockID
		);
	
	if (pRulID)
		*pRulID = gRulID;
	
	ReleaseResource( &gAppRulListLock );
	
	return status;
}


NTSTATUS
RemoveAllAppRulEx (
	__in PLIST_ENTRY pAppRulHead
	)
{
	PLIST_ENTRY Flink;
	PAPP_RUL_ENTRY papp_rul_entry = NULL;
	
	if (!pAppRulHead)
		return STATUS_INVALID_PARAMETER;
	
	if (IsListEmpty( pAppRulHead ))
		return STATUS_SUCCESS;

	Flink = pAppRulHead->Flink;
	while (Flink != pAppRulHead)
	{
		papp_rul_entry = CONTAINING_RECORD( Flink, APP_RUL_ENTRY, ListEntry );

		Flink = Flink->Flink;
		RemoveEntryList( Flink->Blink );
		
		if (papp_rul_entry->app_rul.appl_info.usPath.Buffer)
			ExFreePool( papp_rul_entry->app_rul.appl_info.usPath.Buffer );
		
		if (papp_rul_entry->app_rul.rul_info.usDeviceName.Buffer)
			ExFreePool( papp_rul_entry->app_rul.rul_info.usDeviceName.Buffer );

		if (papp_rul_entry->app_rul.rul_info.usValueName.Buffer)
			ExFreePool( papp_rul_entry->app_rul.rul_info.usValueName.Buffer );

		if (papp_rul_entry)
			ExFreePool( papp_rul_entry );
	}

	return STATUS_SUCCESS;
}

NTSTATUS
RemoveAllAppRul (
	)
{
	NTSTATUS status;

	AcquireResourceExclusive( &gAppRulListLock );
	status = RemoveAllAppRulEx( &gAppRulList );
	ReleaseResource( &gAppRulListLock );
	
	return status;
}

NTSTATUS
RemoveAppRulHeadListIDEx (
	__in PLIST_ENTRY pAppRulHead,
	__in ULONG clientID
	)
{
	PLIST_ENTRY Flink;
	PAPP_RUL_ENTRY papp_rul_entry = NULL;
	
	if (!pAppRulHead)
		return STATUS_INVALID_PARAMETER;

	if (IsListEmpty( pAppRulHead ))
		return STATUS_SUCCESS;

	Flink = pAppRulHead->Flink;
	while (Flink != pAppRulHead)
	{
		papp_rul_entry = CONTAINING_RECORD( Flink, APP_RUL_ENTRY, ListEntry );

		if (papp_rul_entry->clientID == clientID)
		{
			Flink = Flink->Flink;
			RemoveEntryList( Flink->Blink );
			
			if (papp_rul_entry->app_rul.appl_info.usPath.Buffer)
				ExFreePool ( papp_rul_entry->app_rul.appl_info.usPath.Buffer );
			
			if (papp_rul_entry->app_rul.rul_info.usDeviceName.Buffer)
				ExFreePool ( papp_rul_entry->app_rul.rul_info.usDeviceName.Buffer );

			if (papp_rul_entry->app_rul.rul_info.usValueName.Buffer)
				ExFreePool( papp_rul_entry->app_rul.rul_info.usValueName.Buffer );

			if (papp_rul_entry)
				ExFreePool( papp_rul_entry );
		}
		else
			Flink = Flink->Flink;
	}

	return STATUS_SUCCESS;
}

NTSTATUS
ResetClientRulesFromList (
	__in ULONG clientID
	)
{
	NTSTATUS status;
	AcquireResourceExclusive( &gAppRulListLock );
	status = RemoveAppRulHeadListIDEx( &gAppRulList, clientID );
	ReleaseResource( &gAppRulListLock );
	
	return status;
}

NTSTATUS
InsertRulTailList (
	__in PLIST_ENTRY pRulHead,
	__in PRUL_INFO prul_info,
	__in ULONG clientID,
	__in LONGLONG RulID
	)
{
	PRUL_ENTRY prul_entry = NULL;
	NTSTATUS status = STATUS_UNSUCCESSFUL;
	
	__try
	{
		prul_entry = ExAllocatePoolWithTag( PagedPool,sizeof (RUL_ENTRY), tag_apprul3 );
		if (!prul_entry)
		{
			status = STATUS_NO_MEMORY;
			__leave;
		}
		
		RtlZeroMemory( prul_entry, sizeof (RUL_ENTRY) );
		
		prul_entry->rul_info.usDeviceName.Buffer = ExAllocatePoolWithTag (
			PagedPool,
			prul_info->usDeviceName.MaximumLength,
			tag_apprul4
			);

		if (!prul_entry->rul_info.usDeviceName.Buffer)
		{
			status = STATUS_NO_MEMORY;
			__leave;
		}
		
		if (prul_info->usValueName.Length)
		{
			prul_entry->rul_info.usValueName.Buffer = ExAllocatePoolWithTag (
				PagedPool,
				prul_info->usValueName.MaximumLength,
				tag_apprulF
				);

			if (!prul_entry->rul_info.usValueName.Buffer)
			{
				status = STATUS_NO_MEMORY;
				__leave;
			}
		}

		//DeviceName, FilePath, RegKey
		RtlZeroMemory( prul_entry->rul_info.usDeviceName.Buffer, prul_info->usDeviceName.MaximumLength );
		prul_entry->rul_info.usDeviceName.Length = 0;
		prul_entry->rul_info.usDeviceName.MaximumLength = prul_info->usDeviceName.MaximumLength;
		RtlCopyUnicodeString( &prul_entry->rul_info.usDeviceName, &prul_info->usDeviceName );

		//ValueName for Registry
		RtlZeroMemory( prul_entry->rul_info.usValueName.Buffer, prul_info->usValueName.MaximumLength );
		prul_entry->rul_info.usValueName.Length = 0;
		prul_entry->rul_info.usValueName.MaximumLength = prul_info->usValueName.MaximumLength;
		RtlCopyUnicodeString( &prul_entry->rul_info.usValueName, &prul_info->usValueName );

		prul_entry->rul_info.mask = prul_info->mask;
		prul_entry->rul_info.blockID = prul_info->blockID;

		prul_entry->clientID = clientID;
		prul_entry->RulID = RulID;

		InsertTailList( pRulHead, &(prul_entry->ListEntry) );
		status = STATUS_SUCCESS;
	}
	__finally
	{
		if ( !NT_SUCCESS( status ) )
		{
			if ( prul_entry )
			{
				if (prul_entry->rul_info.usDeviceName.Buffer)
					ExFreePool( prul_entry->rul_info.usDeviceName.Buffer );

				if (prul_entry->rul_info.usValueName.Buffer)
					ExFreePool( prul_entry->rul_info.usValueName.Buffer );

				ExFreePool( prul_entry );
			}
		}
	}
	
	return status;
}

//! 

NTSTATUS
RemoveRulHeadList(
	__in PLIST_ENTRY pRulHead 
	)
{
	PLIST_ENTRY Flink;
	PRUL_ENTRY prul_entry;
	
	if (!pRulHead) return STATUS_INVALID_PARAMETER;
	
	if ( IsListEmpty( pRulHead ) )
		return	STATUS_SUCCESS;

	Flink = pRulHead->Flink;
	while ( Flink != pRulHead )
	{
		prul_entry = CONTAINING_RECORD( Flink, RUL_ENTRY, ListEntry );

		Flink = Flink->Flink;
		RemoveEntryList( Flink->Blink );
		
		if ( prul_entry->rul_info.usDeviceName.Buffer ) 
			ExFreePool( prul_entry->rul_info.usDeviceName.Buffer );
		
		if ( prul_entry->rul_info.usValueName.Buffer )
			ExFreePool( prul_entry->rul_info.usValueName.Buffer );

		if ( prul_entry )
			ExFreePool( prul_entry );
	}

	return STATUS_SUCCESS;
}


//////////////////////////////////////////////////////////////////////////
/////////деревья////////

RTL_GENERIC_COMPARE_RESULTS
NTAPI
PidTree_Compare (
	__in PRTL_GENERIC_TABLE pTable,
	__in PVOID  FirstStruct,
	__in PVOID  SecondStruct
	)
{
	PPID_TREE_ENTRY pTrEntry1 = (PPID_TREE_ENTRY) FirstStruct;
	PPID_TREE_ENTRY pTrEntry2 = (PPID_TREE_ENTRY) SecondStruct;

	LONG compare = 0;

	UNREFERENCED_PARAMETER( pTable );

	if ( pTrEntry1->pid > pTrEntry2->pid )
		return GenericGreaterThan;
	else if( pTrEntry1->pid < pTrEntry2->pid )
		return GenericLessThan;
	
	return GenericEqual;
}


PVOID
NTAPI
PidTree_Allocate (
	__in PRTL_GENERIC_TABLE pTable,
	__in CLONG  ByteSize
	)
{
	PVOID ptr = NULL;
	UNREFERENCED_PARAMETER( pTable );

	ptr = ExAllocatePoolWithTag( PagedPool, ByteSize, tag_apprul5 );

	return ptr;
}

VOID
NTAPI
PidTree_Free (
	__in PRTL_GENERIC_TABLE pTable,
	__in PVOID  Buffer
	)
{
	UNREFERENCED_PARAMETER( pTable );

	ExFreePool( Buffer );
}



void
InitPidTree(
	__in PRTL_GENERIC_TABLE pPidTree
	)
{
	RtlInitializeGenericTable(
		pPidTree,
		PidTree_Compare,
		PidTree_Allocate,
		PidTree_Free,
		NULL
		);
}


NTSTATUS
InsertElementInPidTree(
	__in PRTL_GENERIC_TABLE pPidTree, 
	__in HANDLE pid,
	__in ULONG clientID,
	__in LONGLONG RulID,
	__in PRUL_INFO prul_info
)
{
	ULONG writeRight = 0, readRight = 0, enumRight = 0, deleteRight = 0;
	ULONG writeLog = 0, readLog = 0, enumLog = 0, deleteLog = 0;

	NTSTATUS status = STATUS_SUCCESS;
	PID_TREE_ENTRY treeEntry;
	PID_TREE_ENTRY *ptreeEntry;
	BOOLEAN NewElement = FALSE;
	
	memset( &treeEntry, 0, sizeof(PID_TREE_ENTRY) );
	
	treeEntry.pid = pid;
	
	ptreeEntry = RtlLookupElementGenericTable(
		pPidTree,
		&treeEntry
		);
	
	if ( !ptreeEntry )
	{
		treeEntry.pid = pid;
		treeEntry.readers_cnt = 0;
		treeEntry.pRulHead=NULL;
		
		treeEntry.pRulHead = ExAllocatePoolWithTag( PagedPool, sizeof(LIST_ENTRY), tag_apprulB );
		if (!treeEntry.pRulHead)
			status=STATUS_NO_MEMORY;
		
		if (NT_SUCCESS(status))
		{
			ptreeEntry = RtlInsertElementGenericTable( pPidTree, &treeEntry, sizeof(PID_TREE_ENTRY), &NewElement );
			if (ptreeEntry && NewElement)
				status = STATUS_SUCCESS;
			else
				status = STATUS_UNSUCCESSFUL;
		}
		
		if ( NT_SUCCESS(status) )
			InitializeListHead( ptreeEntry->pRulHead );
		
		if ( !NT_SUCCESS(status) )
		{
			if ( treeEntry.pRulHead )
				ExFreePool( treeEntry.pRulHead );
		}

	}
	
	if ( NT_SUCCESS(status) )
		status = InsertRulTailList( ptreeEntry->pRulHead, prul_info, clientID, RulID);	
		
	DoTraceEx ( 
		TRACE_LEVEL_INFORMATION, DC_HIPS,  
		"InsertElementInPidTree pid=%p FilePath=%wZ status=0x%x",
		pid,
		&prul_info->usDeviceName,
		status
		);
	DoTraceEx ( 
		TRACE_LEVEL_INFORMATION, DC_HIPS,  
		"InsertElementInPidTree clientID=%d RulID=%I64x",
		clientID,
		RulID
		);

	writeRight = HIPS_GET_AM ( prul_info->mask, HIPS_POS_WRITE );
	readRight = HIPS_GET_AM ( prul_info->mask, HIPS_POS_READ );
	enumRight = HIPS_GET_AM ( prul_info->mask, HIPS_POS_ENUM );
	deleteRight = HIPS_GET_AM ( prul_info->mask, HIPS_POS_DELETE );

	writeLog = HIPS_GET_LOG ( prul_info->mask, HIPS_POS_WRITE );
	readLog = HIPS_GET_LOG ( prul_info->mask, HIPS_POS_READ );
	enumLog = HIPS_GET_LOG ( prul_info->mask, HIPS_POS_ENUM );
	deleteLog = HIPS_GET_LOG ( prul_info->mask, HIPS_POS_DELETE );

	DoTraceEx (
		TRACE_LEVEL_INFORMATION, DC_HIPS,
		"InsertElementInPidTree AccessMask =%d (wR=%d, rR=%d, eR=%d, dR=%d)",
		prul_info->mask,
		writeRight,
		readRight,
		enumRight,
		deleteRight
		);

	DoTraceEx (
		TRACE_LEVEL_INFORMATION, DC_HIPS,
		"InsertElementInPidTree LogMask (wR=%d, rR=%d, eR=%d, dR=%d)",
		writeLog,
		readLog,
		enumLog,
		deleteLog
		);

	return status;
}

// 0 - одинаковы
int
ComparePidAndApplInfo( 
	__in PPID_LIST_ENTRY ppid_list_entry, 
	__in PAPPL_INFO pappl_info 
	)
{
	md5_byte_t null_digest[16];
	memset (null_digest,0, sizeof(null_digest));
	
	//если в APPL_INFO и PID_LIST_ENTRY есть хэши то сравниваем их
	if ( memcmp(pappl_info->digest,null_digest,sizeof(md5_byte_t)*16)!= 0 &&
		memcmp(ppid_list_entry->digest,null_digest,sizeof(md5_byte_t)*16)!= 0
		)
	{
		//если хэши не совпадают то дальнейшая проверка бессмысленна
		if (memcmp( pappl_info->digest, ppid_list_entry->digest, sizeof(md5_byte_t)*16 )!=0)
			return -1;
	}
	
	if ( pappl_info->usPath.Length > 0 &&
		ppid_list_entry->usNativePath.Length > 0 )
	{
		int ret = -1;
		
		if ( FsRtlIsNameInExpression( &(pappl_info->usPath), &(ppid_list_entry->usNativePath), TRUE, NULL ) )	
				ret = 0;

		return ret;
	}
	
	return 0;
}


NTSTATUS
GeneratePidTree( 
	__in PRTL_GENERIC_TABLE pPidTree
)
{
	LIST_ENTRY PidList;
	PLIST_ENTRY pPidListFlink;
	PPID_LIST_ENTRY ppid_list_entry; 
	
	PLIST_ENTRY pAppRulFlink;
	PAPP_RUL_ENTRY papp_rul_entry;

	InitializeListHead( &PidList );
	FillPidList( &PidList );

	if ( IsListEmpty( &PidList ) )
		return	STATUS_SUCCESS;

	if ( IsListEmpty( &gAppRulList ) )
	{
		RemovePidList( &PidList );
		return	STATUS_SUCCESS;
	}

	AcquireResourceExclusive( &gAppRulListLock );

	pPidListFlink = PidList.Flink;
	while (pPidListFlink != &PidList)
	{
		ppid_list_entry = CONTAINING_RECORD( pPidListFlink,PID_LIST_ENTRY, ListEntry );
		
		pAppRulFlink = gAppRulList.Flink;
		while ( pAppRulFlink != &gAppRulList )
		{
			papp_rul_entry = CONTAINING_RECORD( pAppRulFlink, APP_RUL_ENTRY, ListEntry );

			// Если PIDы совпадают добавляем правило для него в наше дерево
			if ( ComparePidAndApplInfo( ppid_list_entry, &(papp_rul_entry->app_rul.appl_info) ) == 0 )
				if ( !NT_SUCCESS( InsertElementInPidTree(
									pPidTree,
									ppid_list_entry->pid,
									papp_rul_entry->clientID,
									papp_rul_entry->RulID,
									&(papp_rul_entry->app_rul.rul_info) ) )
					)
				{
					_dbg_break_;
					
					DoTraceEx ( 
						TRACE_LEVEL_ERROR, DC_HIPS, 
						"InsertElementInPidTree error pid=%p ApplicationPath=%wZ, FilePath=%wZ AccessMask = %d, rulId=%I64x blockID=%d", 
						ppid_list_entry->pid,
						&papp_rul_entry->app_rul.appl_info.usPath, 
						&papp_rul_entry->app_rul.rul_info.usDeviceName,
						papp_rul_entry->app_rul.rul_info.mask,
						papp_rul_entry->RulID,
						papp_rul_entry->app_rul.rul_info.blockID
						);

					
				}
			
			pAppRulFlink = pAppRulFlink->Flink;
		}
		
		pPidListFlink = pPidListFlink->Flink;
	}

	ReleaseResource( &gAppRulListLock );
	
	RemovePidList( &PidList );

	return STATUS_SUCCESS;
}


PPID_TREE_ENTRY
GetPidTreeEntryByPid(
	__in PRTL_GENERIC_TABLE pPidTree, 
	__in HANDLE pid
	)
{
	PID_TREE_ENTRY treeEntry;
	PID_TREE_ENTRY *ptreeEntry;
	
	memset( &treeEntry,0, sizeof(PID_TREE_ENTRY) );

	treeEntry.pid = pid;

	ptreeEntry = RtlLookupElementGenericTable(
		pPidTree,
		&treeEntry
		);

	if (ptreeEntry)
	{
		return ptreeEntry;
	}	

	return NULL;
}

PPID_TREE
GetPidTreePointer()
{
	return (PPID_TREE) InterlockedCompareExchangePointer( &gpPidTree, NULL, NULL );
}

PPID_TREE
SwitchPidTreePointer(
	__in_opt PPID_TREE newPidTreePointer
	)
{
	return (PPID_TREE)InterlockedExchangePointer( &gpPidTree, newPidTreePointer );
}

IO_WORKITEM_ROUTINE RemovePidTreeRoutine;

void RemovePidTreeRoutine( 
	__in PDEVICE_OBJECT	DeviceObject,
	__in PVOID  p 
  	)
{
	PPID_TREE pPidTree;
	PPID_TREE_ENTRY ppid_tree_entry;

	UNREFERENCED_PARAMETER( DeviceObject );

	pPidTree = (PPID_TREE)p;
	
	WaitInterlockedObj(&(pPidTree->readers_cnt));

	ppid_tree_entry = ( PPID_TREE_ENTRY ) RtlGetElementGenericTable(&(pPidTree->PidTree), 0);
	while ( ppid_tree_entry )
	{
		RemoveRulHeadList( ppid_tree_entry->pRulHead );
		
		if ( ppid_tree_entry->pRulHead )
			ExFreePool ( ppid_tree_entry->pRulHead );
		
		RtlDeleteElementGenericTable( &(pPidTree->PidTree), ppid_tree_entry );
				
		ppid_tree_entry = (PPID_TREE_ENTRY)RtlGetElementGenericTable(&(pPidTree->PidTree),0);
	}
	
	DoTraceEx( 
		TRACE_LEVEL_INFORMATION, DC_HIPS, 
		"RemovePidTreeRoutine: tree removed pPidTree=%p",	
		pPidTree 
		);

	ExFreePool (pPidTree);

	InterlockedDecrement( &gPidTreeRemCnt );
}


VOID
RemovePidTree( 
	__in PPID_TREE pPidTree 
	)
{
	PIO_WORKITEM pItem = IoAllocateWorkItem( Globals.m_FilterControlDeviceObject );

	if (!pItem)
	{
		_dbg_break_;
		DoTraceEx( 
			TRACE_LEVEL_ERROR, DC_HIPS, 
			"RemovePidTree IoAllocateWorkItem Error: pPidTree=%p",	
			pPidTree );
	}

	InterlockedIncrement( &gPidTreeRemCnt );

	IoQueueWorkItem( pItem, RemovePidTreeRoutine, DelayedWorkQueue, pPidTree );
}



//перестройка дерева PIDов:
// - если пришло новое правило
// - если запустили новый процесс
// - если удалили процесс
NTSTATUS
RebuildPidTree()
{
	PPID_TREE pNewPidTree;
	PPID_TREE pOldPidTree;
	
	NTSTATUS status=STATUS_SUCCESS;

	pNewPidTree=ExAllocatePoolWithTag(PagedPool,sizeof(PID_TREE), tag_apprul6);
	if( !pNewPidTree )
		return STATUS_NO_MEMORY;
	
	memset(pNewPidTree,0, sizeof(PID_TREE));
	
	InitPidTree(&(pNewPidTree->PidTree));

	status = GeneratePidTree(&(pNewPidTree->PidTree));
	
	//если дерево не создалось или оно пустое, то удаляем его
	if ( !NT_SUCCESS(status) || !RtlGetElementGenericTable(&(pNewPidTree->PidTree),0) )
	{
		ExFreePool (pNewPidTree);
		pNewPidTree=NULL;
	}

	pOldPidTree=SwitchPidTreePointer( pNewPidTree );
	
	if ( pOldPidTree )
		RemovePidTree(pOldPidTree);

	DoTraceEx (
		TRACE_LEVEL_INFORMATION, DC_HIPS, "RebuildPidTree: newPid=%p oldPid=%p, status=0x%x",
		pNewPidTree,
		pOldPidTree,
		status
		);

	return status;
}

/*
typedef struct _FILTER_EVENT_PARAM
{
	ULONG HookID;									// FLTTYPE_SYSTEM
	ULONG FunctionMj;								// MJ_SYSTEM_HIPS
	ULONG FunctionMi;								// MI_SYSTEM_HIPS_LOG
	ULONG ThreadID;									// 0
	CHAR  ProcName[PROCNAMELEN];					// 0
	ULONG ProcessID;								// 0
	PROCESSING_TYPE ProcessingType;					// PreProcessing
	DWORD UnsafeValue;								// 0
	ULONG ParamsCount;								// 8+1 ( PID, FilePath, opt Volume, opt ValueName, mask, RulID, opt SID, desiredOp, opt answer )	
	BYTE Params[0];									// SINGLE_PARAM 
}FILTER_EVENT_PARAM, *PFILTER_EVENT_PARAM;
*/

PFILTER_EVENT_PARAM
EvContext_appRul (
	__in ULONG evType,
	__in_opt PSID	psid,
	__in ULONG	disiredOp,
	__in HANDLE pid,
	__in UNICODE_STRING FilePath,
	__in_opt PUNICODE_STRING pVolumeName,
	__in_opt PUNICODE_STRING pValueName,
	__in ULONG mask,
	__in LONGLONG RulID
)
{
	PFILTER_EVENT_PARAM pParam = NULL;
		
	ULONG ParamsCount = 0;
	ULONG params_size = sizeof(FILTER_EVENT_PARAM);

	ParamsCount++;											//1: PID
	params_size += sizeof(SINGLE_PARAM) + sizeof(HANDLE);

	ParamsCount++;											//2: FilePath + \0
	params_size += sizeof(SINGLE_PARAM) + FilePath.Length + sizeof(WCHAR);

	
	if ( pVolumeName && pVolumeName->Length )
	{
		ParamsCount++;											//3: Volume + \0
		params_size += sizeof(SINGLE_PARAM) + pVolumeName->Length + sizeof(WCHAR);	
	}

	if ( pValueName && pValueName->Length )
	{
		ParamsCount++;											//4: ValueName + \0
		params_size += sizeof(SINGLE_PARAM) + pValueName->Length + sizeof(WCHAR);	
	}

	ParamsCount++;											//5: mask
	params_size += sizeof(SINGLE_PARAM) + sizeof(ULONG);

	ParamsCount++;											//6: RulID
	params_size += sizeof(SINGLE_PARAM) + sizeof(LONGLONG);

	if (psid )
	{
		ParamsCount++;										//7: SID
		params_size += sizeof(SINGLE_PARAM) + RtlLengthSid( psid );
	}

	ParamsCount++;											//8: desiredOp
	params_size += sizeof(SINGLE_PARAM) + sizeof(ULONG);

	ParamsCount++;										//9: answer
	params_size += sizeof(SINGLE_PARAM) + sizeof(ULONG);

	pParam = ExAllocatePoolWithTag( PagedPool, params_size, tag_apprulC );
	if (pParam)
	{
		PSINGLE_PARAM pSingleParam;
		FILTER_PARAM_COMMONINIT( pParam, FLTTYPE_SYSTEM, MJ_SYSTEM_HIPS, evType, PreProcessing, ParamsCount );

		pSingleParam = (PSINGLE_PARAM) pParam->Params;
												//1: PID
		SINGLE_PARAM_INIT_NONE(pSingleParam, _PARAM_OBJECT_CLIENTID1);
		pSingleParam->ParamSize=sizeof(HANDLE);
		*(HANDLE*)pSingleParam->ParamValue = pid;

		SINGLE_PARAM_SHIFT( pSingleParam );		//2: FilePath + \0
		SINGLE_PARAM_INIT_NONE( pSingleParam, _PARAM_OBJECT_URL_W );
		if (FilePath.Length)
		{
			memcpy( pSingleParam->ParamValue, FilePath.Buffer, FilePath.Length );
			((PCHAR)pSingleParam->ParamValue)[FilePath.Length] = 0;
			((PCHAR)pSingleParam->ParamValue)[FilePath.Length + 1] = 0;
			pSingleParam->ParamSize = FilePath.Length + sizeof(WCHAR);
		}

		if ( pVolumeName && pVolumeName->Length )
		{
			SINGLE_PARAM_SHIFT( pSingleParam );		//3: Volume + \0
			SINGLE_PARAM_INIT_NONE( pSingleParam, _PARAM_OBJECT_VOLUME_NAME_W );
		
			memcpy( pSingleParam->ParamValue, pVolumeName->Buffer, pVolumeName->Length );
			((PCHAR)pSingleParam->ParamValue)[pVolumeName->Length] = 0;
			((PCHAR)pSingleParam->ParamValue)[pVolumeName->Length + 1] = 0;
			pSingleParam->ParamSize = pVolumeName->Length + sizeof(WCHAR);
		}

		if ( pValueName && pValueName->Length )
		{
			SINGLE_PARAM_SHIFT( pSingleParam );		//4: ValueName + \0
			SINGLE_PARAM_INIT_NONE( pSingleParam, _PARAM_OBJECT_URL_PARAM_W );

			memcpy( pSingleParam->ParamValue, pValueName->Buffer, pValueName->Length );
			((PCHAR)pSingleParam->ParamValue)[pValueName->Length] = 0;
			((PCHAR)pSingleParam->ParamValue)[pValueName->Length + 1] = 0;
			pSingleParam->ParamSize = pValueName->Length + sizeof(WCHAR);
		}


		SINGLE_PARAM_SHIFT( pSingleParam );		//5: mask
		SINGLE_PARAM_INIT_ULONG( pSingleParam, _PARAM_OBJECT_ATTRIB, mask );

		SINGLE_PARAM_SHIFT( pSingleParam );		//6: RulID
		SINGLE_PARAM_INIT_NONE(pSingleParam, _PARAM_OBJECT_SOURCE_ID);
		pSingleParam->ParamSize = sizeof(LONGLONG);
		*(LONGLONG*)pSingleParam->ParamValue = RulID;

		if (psid)
		{
			SINGLE_PARAM_SHIFT( pSingleParam );		//7 SID
			SINGLE_PARAM_INIT_NONE(pSingleParam, _PARAM_OBJECT_SID);
			pSingleParam->ParamSize = RtlLengthSid( psid );
			memcpy( pSingleParam->ParamValue, psid, RtlLengthSid( psid ) );
		}

		SINGLE_PARAM_SHIFT( pSingleParam );			//8: disiredOp
		SINGLE_PARAM_INIT_ULONG( pSingleParam, _PARAM_OBJECT_ACCESSATTR, disiredOp );

		SINGLE_PARAM_SHIFT( pSingleParam );		//9: answer
		SINGLE_PARAM_INIT_ULONG( pSingleParam, _PARAM_RET_STATUS, 0 );
	}
	
	return pParam;
}

tefVerdict
IsAllowAccessEx(
	__in_opt PSID psid,
	__in HANDLE pid,
	__in UNICODE_STRING Path,
	__in_opt PUNICODE_STRING pVolumeName,
	__in_opt PUNICODE_STRING pValueName,
	__in ULONG	desiredOp
	)
{
	PPID_TREE pPidTree;
	PPID_TREE_ENTRY ptreeEntry;

	PLIST_ENTRY	pRullistFlink;
	PRUL_ENTRY	prul_entry;

	tefVerdict verdict = efVerdict_Default;
	ULONG	mask = 0;
	ULONG writeRight = 0, readRight = 0, enumRight = 0, deleteRight = 0;
	ULONG writeLog = 0, readLog = 0, enumLog = 0, deleteLog = 0;
	LONGLONG RulID = 0;
	ULONG cur_clientID = 0;
	BOOLEAN nextClientID = FALSE;

	PFILTER_EVENT_PARAM pEvent = NULL;

	if (IsInvisible( PsGetCurrentThreadId(), 0 ))
		return efVerdict_Default;

	pPidTree = GetPidTreePointer();
	if(!pPidTree)
		return efVerdict_Default;

	InterlockedIncrement( &pPidTree->readers_cnt);
	ptreeEntry = GetPidTreeEntryByPid( &pPidTree->PidTree, pid );

	if (!ptreeEntry) 
	{		
		InterlockedDecrement( &pPidTree->readers_cnt );
		return verdict;
	}

	DoTraceEx ( 
		TRACE_LEVEL_INFORMATION, DC_HIPS,  
		"+++IsAllowAccessEx begin: pid=%p, FileName=%wZ", 
		pid,
		&Path
		);

	if ( pValueName )
	{
		DoTraceEx (	TRACE_LEVEL_INFORMATION, DC_HIPS,  "IsAllowAccessEx ValueName=%wZ", pValueName );
	}
	

	DoTraceEx ( 
		TRACE_LEVEL_INFORMATION, DC_HIPS,  
		"IsAllowAccessEx DesiredOpMask = %d ( wF=%d, rF=%d, eF=%d, dF=%d )", 
		desiredOp,
		BooleanFlagOn(desiredOp,FILE_OP_WRITE),
		BooleanFlagOn(desiredOp,FILE_OP_READ),
		BooleanFlagOn(desiredOp,FILE_OP_ENUM),
		BooleanFlagOn(desiredOp,FILE_OP_DELETE)
		);


	pRullistFlink = ptreeEntry->pRulHead->Flink;

	while ( pRullistFlink != ptreeEntry->pRulHead )
	{
		prul_entry = CONTAINING_RECORD( pRullistFlink, RUL_ENTRY, ListEntry );

		if (nextClientID)
		{
			if ( cur_clientID != prul_entry->clientID )
				nextClientID = FALSE;
		}
		
		if (!nextClientID)
		{
			PQCLIENT pClient = AcquireClient( prul_entry->clientID );
			if (pClient)
			{
				if (FlagOn( pClient->m_ClientFlags, _CLIENT_FLAG_PAUSED ))
					nextClientID = TRUE;

				ReleaseClient( pClient );
			}
		}

		if (!nextClientID)
			if ( FsRtlIsNameInExpression( &prul_entry->rul_info.usDeviceName, &Path, TRUE, NULL ) )
			{
				BOOLEAN isPasst = TRUE;
				
				if (prul_entry->rul_info.usValueName.Length > 0)
				{
					isPasst = FALSE;
					if ( pValueName && pValueName->Buffer && pValueName->Length)
					{
						if ( FsRtlIsNameInExpression( &prul_entry->rul_info.usValueName, pValueName, TRUE, NULL ) )
							isPasst = TRUE;
					}
				}	
				
				if ( isPasst )
				{
					mask = (prul_entry->rul_info.mask);
					cur_clientID = prul_entry->clientID;
					
					writeRight = HIPS_GET_AM ( mask, HIPS_POS_WRITE );
					readRight = HIPS_GET_AM ( mask, HIPS_POS_READ );
					enumRight = HIPS_GET_AM ( mask, HIPS_POS_ENUM );
					deleteRight = HIPS_GET_AM ( mask, HIPS_POS_DELETE );

					writeLog = HIPS_GET_LOG ( mask, HIPS_POS_WRITE );
					readLog = HIPS_GET_LOG ( mask, HIPS_POS_READ );
					enumLog = HIPS_GET_LOG ( mask, HIPS_POS_ENUM );
					deleteLog = HIPS_GET_LOG ( mask, HIPS_POS_DELETE );

					DoTraceEx ( 
						TRACE_LEVEL_WARNING, DC_HIPS,  
						"IsAllowAccessEx Finded Rule: RuleMask=%wZ", 
						&prul_entry->rul_info.usDeviceName
						);

					if (prul_entry->rul_info.usValueName.Length > 0)
					{
						DoTraceEx ( 
							TRACE_LEVEL_WARNING, DC_HIPS,  
							"IsAllowAccessEx : ValueMask=%wZ", 
							&prul_entry->rul_info.usValueName
							);
					}

					DoTraceEx ( 
						TRACE_LEVEL_WARNING, DC_HIPS,
						"IsAllowAccessEx RulID=%I64x, blockID=%d", 
						prul_entry->RulID,
						prul_entry->rul_info.blockID
						);

					DoTraceEx ( 
						TRACE_LEVEL_WARNING, DC_HIPS,  
						"IsAllowAccessEx AccessMask = %d (wR=%d, rR=%d, eR=%d, dR=%d)", 
						mask,
						writeRight,
						readRight,
						enumRight,
						deleteRight
						);

					DoTraceEx ( 
						TRACE_LEVEL_WARNING, DC_HIPS,  
						"IsAllowAccessEx LogMask = (wR=%d, rR=%d, eR=%d, dR=%d)", 
						writeLog,
						readLog,
						enumLog,
						deleteLog
						);


					// если в маске указано спросить у клиента - то спрашиваем, изменяем маску, идем дальше
					if ( ( writeRight == HIPS_FLAG_ASK && FlagOn(desiredOp,FILE_OP_WRITE) ) ||
						( readRight == HIPS_FLAG_ASK && FlagOn(desiredOp,FILE_OP_READ) ) ||
						( enumRight == HIPS_FLAG_ASK && FlagOn(desiredOp,FILE_OP_ENUM) ) ||
						( deleteRight == HIPS_FLAG_ASK && FlagOn(desiredOp,FILE_OP_DELETE) )
						)
					{
						//здесь спрашиваем 
						//bAllow = ask
						DoTraceEx ( 
							TRACE_LEVEL_WARNING, DC_HIPS,  
							"IsAllowAccessEx: need ask"
							);

						pEvent = EvContext_appRul (
							MI_SYSTEM_HIPS_ASK,
							psid,
							desiredOp,
							pid,
							Path,
							pVolumeName,
							pValueName,
							mask,
							prul_entry->RulID
							);

						if (pEvent)
							verdict = FilterEventImp( pEvent, NULL, prul_entry->clientID );
					}
					else
					{
						if (
							( writeRight == HIPS_FLAG_DENY && FlagOn(desiredOp,FILE_OP_WRITE) ) ||
							( readRight == HIPS_FLAG_DENY && FlagOn(desiredOp,FILE_OP_READ) ) ||
							( enumRight == HIPS_FLAG_DENY && FlagOn(desiredOp,FILE_OP_ENUM) ) ||
							( deleteRight == HIPS_FLAG_DENY && FlagOn(desiredOp,FILE_OP_DELETE) )
							)
							verdict = efVerdict_Deny;
					}


					//если нужно писать в лог - пишем
					if (
						( writeLog == HIPS_LOG_ON && FlagOn(desiredOp,FILE_OP_WRITE) ) ||
						( readLog == HIPS_LOG_ON && FlagOn(desiredOp,FILE_OP_READ) ) ||
						( enumLog == HIPS_LOG_ON && FlagOn(desiredOp,FILE_OP_ENUM) ) ||
						( deleteLog == HIPS_LOG_ON && FlagOn(desiredOp,FILE_OP_DELETE) )
					)
					{
						DoTraceEx ( 
							TRACE_LEVEL_WARNING, DC_HIPS,  
							"IsAllowAccessEx: need log" 
							);

						if (!pEvent)
							pEvent = EvContext_appRul (
							MI_SYSTEM_HIPS_LOG,
							psid,
							desiredOp,
							pid,
							Path,
							pVolumeName,
							pValueName,
							mask,
							prul_entry->RulID
							);
						else
							pEvent->FunctionMi = MI_SYSTEM_HIPS_LOG;

						{
							PSINGLE_PARAM pParam = GetSingleParamPtr( pEvent, _PARAM_RET_STATUS );
							*(ULONG*)pParam->ParamValue = verdict;
						}

						FilterEventImp( pEvent, NULL, prul_entry->clientID );
					}

					if (pEvent)
					{
						ExFreePool( pEvent );
						pEvent = NULL;
					}

					//если есть запрещающее правило то дальще искть нет смысла
					if ( efIsDeny( verdict))
					{
						DoTraceEx ( 
							TRACE_LEVEL_WARNING, DC_HIPS,  
							"IsAllowAccessEx: break verdict deny(2)" 
							);
						break;
					}

					nextClientID = TRUE;
				}

			}

			pRullistFlink=pRullistFlink->Flink;
	}

	InterlockedDecrement( &(pPidTree->readers_cnt) );

	DoTraceEx ( 
		TRACE_LEVEL_INFORMATION, DC_HIPS,  
		"---IsAllowAccessEx end: return verdict=%d", 
		verdict
		);


	return verdict;
}

tefVerdict
IsAllowAccessFile(
	__in PFLT_CALLBACK_DATA Data,
	__in HANDLE pid,
	__in PFLT_FILE_NAME_INFORMATION pFileNameInfo,
	__in ULONG	desiredOp
	)
{
	tefVerdict verdict = efVerdict_Default;
	PSID psid = NULL;
	
	if (!desiredOp)
		return efVerdict_Default;

	if (!NT_SUCCESS(SeGetUserSid(Data, &psid) ) )
		psid = NULL;

	verdict = IsAllowAccessEx(
							psid,
							pid,
							pFileNameInfo->Name,
							&pFileNameInfo->Volume,
							NULL,
							desiredOp
							);

	if (psid)
		ExFreePool(psid);
	
	return verdict;

}

tefVerdict
IsAllowAccessReg(
				 __in_opt PSID psid,
				 __in HANDLE pid,
				 __in UNICODE_STRING Path,
				 __in_opt PUNICODE_STRING pValueName,
				 __in ULONG	desiredOp
				 )
{
	
	tefVerdict verdict = efVerdict_Default;

	if (!desiredOp)
		return efVerdict_Default;
	
	verdict = IsAllowAccessEx(
		psid,
		pid,
		Path,
		NULL,
		pValueName,
		desiredOp
		);

	return verdict;
}



//должна запускаться при инициализации драйвера
void InitAppRul()
{
	SwitchPidTreePointer( NULL );
	InitializeListHead( &gAppRulList );
	ExInitializeResourceLite( &gAppRulListLock );
}

// запуск при выгрузке драйвера
void DoneAppRul()
{
	PPID_TREE pOldPidTree;
	pOldPidTree = SwitchPidTreePointer( NULL );
	
	if (pOldPidTree)
		RemovePidTree( pOldPidTree );
	
	WaitInterlockedObj( &gPidTreeRemCnt );

	RemoveAllAppRul();
	
	ExDeleteResourceLite( &gAppRulListLock );
}
