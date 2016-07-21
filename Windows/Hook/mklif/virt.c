#include "virt.h"
#include "../hook/Funcs.h"

#include "./inc/virt_tools.h"

#include "apprul.tmh"

LIST_ENTRY	gVirtAppList;
LIST_ENTRY	gVirtSandBoxList;
ERESOURCE	gVirtAppListLock;
ERESOURCE	gVirtSandBoxListLock;

PPID_TREE gpVirtPidTree = NULL;
ULONG	  gVirtPidTreeRemCnt = 0;

LONGLONG gSandBoxID = 0;

#define IS_DESIRED_READ_ONLY(da) \
	!BooleanFlagOn( (da), DELETE ) && \
	!BooleanFlagOn( (da), FILE_WRITE_DATA ) && \
	!BooleanFlagOn( (da), FILE_WRITE_ATTRIBUTES ) && \
	!BooleanFlagOn( (da), FILE_WRITE_EA ) && \
	!BooleanFlagOn( (da), FILE_APPEND_DATA ) && \
	!BooleanFlagOn( (da), WRITE_DAC ) && \
	!BooleanFlagOn( (da), WRITE_OWNER )

typedef enum _FILE_TYPE
{
	FT_Unknown		=	0,
	FT_File			=	1,
	FT_Directory	=	2
}FILE_TYPE;

#define IS_DESIRED_READ_ONLY_REG(da) \
	!BooleanFlagOn( (da), DELETE ) && \
	!BooleanFlagOn( (da), KEY_SET_VALUE ) && \
	!BooleanFlagOn( (da), KEY_CREATE_SUB_KEY )



NTSTATUS
Virt_InsertApplToListEx (
	__in PLIST_ENTRY pAppListHead,
	__in UNICODE_STRING usNativeAppPath,
	__in UNICODE_STRING usNativeSBPath,
	__in UNICODE_STRING usNativeSBVolumeName,
	__in ULONG logMask
)
{
	PVIRT_APPL_LIST_ENTRY papp_list_entry = NULL;
	ULONG	AppPathLen = 0;
	ULONG	FilePathLen = 0;
	NTSTATUS status = STATUS_UNSUCCESSFUL;

	__try
	{
		papp_list_entry = ExAllocatePoolWithTag( PagedPool, sizeof (VIRT_APPL_LIST_ENTRY), tag_virt1 );
		if (!papp_list_entry)
		{
			status = STATUS_NO_MEMORY;
			__leave;
		}

		RtlZeroMemory( papp_list_entry, sizeof (VIRT_APPL_LIST_ENTRY) );

		if (!usNativeAppPath.Length)
		{
			status = STATUS_INVALID_PARAMETER_2;
			__leave;
		}
		if (!usNativeSBPath.Length)
		{
			status = STATUS_INVALID_PARAMETER_3;
			__leave;
		}

		if (!usNativeSBVolumeName.Length)
		{
			status = STATUS_INVALID_PARAMETER_4;
			__leave;
		}

		if ( usNativeAppPath.Length > 0 )
		{
			PWCHAR pwchNativeAppPath = ExAllocatePoolWithTag( PagedPool, usNativeAppPath.Length, tag_virt2 );
			if (!pwchNativeAppPath)
			{
				status = STATUS_NO_MEMORY;
				__leave;
			}

			RtlInitEmptyUnicodeString (
				&papp_list_entry->appl.usAppPath,
				pwchNativeAppPath,
				usNativeAppPath.Length
				);

			status = RtlUpcaseUnicodeString( &papp_list_entry->appl.usAppPath, &usNativeAppPath,FALSE );
			if (!NT_SUCCESS( status ))
				__leave;
		}

		if (usNativeSBPath.Length > 0)
		{
			PWCHAR pwchNativeSBPath = ExAllocatePoolWithTag( PagedPool, usNativeSBPath.Length, tag_virt3 );
			if ( !pwchNativeSBPath )
			{
				status = STATUS_NO_MEMORY;
				__leave;
			}

			RtlInitEmptyUnicodeString (
				&papp_list_entry->appl.usSBPath,
				pwchNativeSBPath,
				usNativeAppPath.Length
				);

			status = RtlUpcaseUnicodeString( &papp_list_entry->appl.usSBPath, &usNativeSBPath,FALSE );
			if (!NT_SUCCESS( status ))
				__leave;
		}
		
		if (usNativeSBVolumeName.Length > 0)
		{
			PWCHAR pwchNativeSBVolumeName = ExAllocatePoolWithTag( PagedPool, usNativeSBVolumeName.Length, tag_virt9 );
			if ( !pwchNativeSBVolumeName )
			{
				status = STATUS_NO_MEMORY;
				__leave;
			}

			RtlInitEmptyUnicodeString (
				&papp_list_entry->appl.usSBVolumeName,
				pwchNativeSBVolumeName,
				usNativeSBVolumeName.Length
				);

			status = RtlUpcaseUnicodeString( &papp_list_entry->appl.usSBVolumeName, &usNativeSBVolumeName, FALSE );
			if (!NT_SUCCESS( status ))
				__leave;
		}

		papp_list_entry->appl.logMask = logMask;
		
		InsertHeadList (
			pAppListHead,
			&papp_list_entry->ListEntry
			);

		status = STATUS_SUCCESS;
	}
	__finally
	{
		if (!NT_SUCCESS( status ))
		{
			if (papp_list_entry)
			{
				if ( papp_list_entry->appl.usAppPath.Buffer )
					ExFreePool ( papp_list_entry->appl.usAppPath.Buffer );

				if ( papp_list_entry->appl.usSBPath.Buffer )
					ExFreePool ( papp_list_entry->appl.usSBPath.Buffer );

				if ( papp_list_entry->appl.usSBVolumeName.Buffer )
					ExFreePool ( papp_list_entry->appl.usSBVolumeName.Buffer );

				ExFreePool( papp_list_entry );
			}
		}
	}

	return status;
}

NTSTATUS
Virt_InsertApplToList( 
	__in UNICODE_STRING usNativeAppPath,
	__in UNICODE_STRING usNativeSBPath,
	__in UNICODE_STRING usNativeSBVolumeName,
	__in ULONG logMask
)
{
	NTSTATUS status;

	AcquireResourceExclusive( &gVirtAppListLock );

	

	status = Virt_InsertApplToListEx (
		&gVirtAppList,
		usNativeAppPath,
		usNativeSBPath,
		usNativeSBVolumeName,
		logMask
		);

	ReleaseResource( &gVirtAppListLock );

	return status;
}


NTSTATUS
Virt_RemoveAllAppFromListEx (
	__in PLIST_ENTRY pAppListHead
)
{
	PLIST_ENTRY Flink;
	PVIRT_APPL_LIST_ENTRY papp_list_entry = NULL;

	if (!pAppListHead)
		return STATUS_INVALID_PARAMETER;

	if (IsListEmpty( pAppListHead ))
		return STATUS_SUCCESS;

	Flink = pAppListHead->Flink;
	while (Flink != pAppListHead)
	{
		papp_list_entry = CONTAINING_RECORD( Flink, VIRT_APPL_LIST_ENTRY, ListEntry );

		Flink = Flink->Flink;
		RemoveEntryList( Flink->Blink );

		if ( papp_list_entry->appl.usAppPath.Buffer )
			ExFreePool ( papp_list_entry->appl.usAppPath.Buffer );

		if ( papp_list_entry->appl.usSBPath.Buffer )
			ExFreePool ( papp_list_entry->appl.usSBPath.Buffer );

		if ( papp_list_entry->appl.usSBVolumeName.Buffer )
			ExFreePool ( papp_list_entry->appl.usSBVolumeName.Buffer );

		if ( papp_list_entry )
			ExFreePool( papp_list_entry );
	}

	return STATUS_SUCCESS;
}

NTSTATUS
Virt_RemoveAllApp(
)
{
	NTSTATUS status;

	AcquireResourceExclusive( &gVirtAppListLock );
	status = Virt_RemoveAllAppFromListEx( &gVirtAppList );
	ReleaseResource( &gVirtAppListLock );

	return status;
}

//////////////////////////////////////////////////////////////////////////
/////////деревья////////

RTL_GENERIC_COMPARE_RESULTS
NTAPI
Virt_PidTree_Compare (
				 __in PRTL_GENERIC_TABLE pTable,
				 __in PVOID  FirstStruct,
				 __in PVOID  SecondStruct
				 )
{
	PVIRT_PID_TREE_ENTRY pTrEntry1 = (PVIRT_PID_TREE_ENTRY) FirstStruct;
	PVIRT_PID_TREE_ENTRY pTrEntry2 = (PVIRT_PID_TREE_ENTRY) SecondStruct;

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
Virt_PidTree_Allocate (
				  __in PRTL_GENERIC_TABLE pTable,
				  __in CLONG  ByteSize
				  )
{
	PVOID ptr = NULL;
	UNREFERENCED_PARAMETER( pTable );

	ptr = ExAllocatePoolWithTag( PagedPool, ByteSize, tag_virt6 );

	return ptr;
}

VOID
NTAPI
Virt_PidTree_Free (
			  __in PRTL_GENERIC_TABLE pTable,
			  __in PVOID  Buffer
			  )
{
	UNREFERENCED_PARAMETER( pTable );

	ExFreePool( Buffer );
}



void
Virt_InitPidTree(
			__in PRTL_GENERIC_TABLE pPidTree
			)
{
	RtlInitializeGenericTable(
		pPidTree,
		Virt_PidTree_Compare,
		Virt_PidTree_Allocate,
		Virt_PidTree_Free,
		NULL
		);
}



NTSTATUS
Virt_InsertElementInPidTree(
					   __in PRTL_GENERIC_TABLE pPidTree, 
					   __in HANDLE pid,
					   __in UNICODE_STRING usNativeAppPath,
					   __in UNICODE_STRING usNativeSBPath,
					   __in UNICODE_STRING usNativeSBVolumeName,
					   __in ULONG logMask
					   )
{
	NTSTATUS status = STATUS_SUCCESS;
	VIRT_PID_TREE_ENTRY treeEntry;
	PVIRT_PID_TREE_ENTRY *ptreeEntry;
	BOOLEAN NewElement = FALSE;

	memset( &treeEntry, 0, sizeof(VIRT_PID_TREE_ENTRY) );

	treeEntry.pid = pid;

	ptreeEntry = RtlLookupElementGenericTable(
		pPidTree,
		&treeEntry
		);

	
	__try
	{
		if ( !ptreeEntry )
		{
			treeEntry.pid = pid;
			treeEntry.appl.logMask = logMask;
			//treeEntry.appl.SandBoxId = SandBoxId;

			if ( !usNativeAppPath.Length )
			{
				status = STATUS_INVALID_PARAMETER_3;
				__leave;
			}
			
			if ( !usNativeSBPath.Length )
			{
				status = STATUS_INVALID_PARAMETER_4;
				__leave;
			}

			if ( !usNativeSBVolumeName.Length )
			{
				status = STATUS_INVALID_PARAMETER_5;
				__leave;
			}

			if ( usNativeAppPath.Length > 0 )
			{
				PWCHAR pwchNativeAppPath = ExAllocatePoolWithTag( PagedPool, usNativeAppPath.Length, tag_virt4 );
				if (!pwchNativeAppPath)
				{
					status = STATUS_NO_MEMORY;
					__leave;
				}

				RtlInitEmptyUnicodeString (
					&treeEntry.appl.usAppPath,
					pwchNativeAppPath,
					usNativeAppPath.Length
					);

				status = RtlUpcaseUnicodeString( &treeEntry.appl.usAppPath, &usNativeAppPath,FALSE );
				if ( !NT_SUCCESS( status ) )
					__leave;
			}

			if ( usNativeSBPath.Length > 0 )
			{
				PWCHAR pwchNativeSBPath = ExAllocatePoolWithTag( PagedPool, usNativeSBPath.Length, tag_virt5 );
				if ( !pwchNativeSBPath )
				{
					status = STATUS_NO_MEMORY;
					__leave;
				}

				RtlInitEmptyUnicodeString (
					&treeEntry.appl.usSBPath,
					pwchNativeSBPath,
					usNativeSBPath.Length
					);

				status = RtlUpcaseUnicodeString( &treeEntry.appl.usSBPath, &usNativeSBPath, FALSE );
				if ( !NT_SUCCESS( status ) )
					__leave;
			}

			if (usNativeSBVolumeName.Length > 0)
			{
				PWCHAR pwchNativeSBVolumeName = ExAllocatePoolWithTag( PagedPool, usNativeSBVolumeName.Length, tag_virtA );
				if ( !pwchNativeSBVolumeName )
				{
					status = STATUS_NO_MEMORY;
					__leave;
				}

				RtlInitEmptyUnicodeString (
					&treeEntry.appl.usSBVolumeName,
					pwchNativeSBVolumeName,
					usNativeSBVolumeName.Length
					);

				status = RtlUpcaseUnicodeString( &treeEntry.appl.usSBVolumeName, &usNativeSBVolumeName, FALSE );
				if (!NT_SUCCESS( status ))
					__leave;
			}

			if (NT_SUCCESS(status))
			{
				ptreeEntry = RtlInsertElementGenericTable( pPidTree, &treeEntry, sizeof(VIRT_PID_TREE_ENTRY), &NewElement );
				if (ptreeEntry && NewElement)
					status = STATUS_SUCCESS;
				else
					status = STATUS_UNSUCCESSFUL;
			}
		}
	}
	__finally
	{
		if (!NT_SUCCESS( status ) )
		{
			if ( treeEntry.appl.usAppPath.Buffer )
				ExFreePool( treeEntry.appl.usAppPath.Buffer );

			if ( treeEntry.appl.usSBPath.Buffer )
				ExFreePool( treeEntry.appl.usSBPath.Buffer );

			if ( treeEntry.appl.usSBVolumeName.Buffer )
				ExFreePool( treeEntry.appl.usSBVolumeName.Buffer );
		}
	}
	
	return status;
}


NTSTATUS
Virt_GeneratePidTree( 
				__in PRTL_GENERIC_TABLE pPidTree
				)
{
	LIST_ENTRY PidList;
	PLIST_ENTRY pPidListFlink;
	PPID_LIST_ENTRY ppid_list_entry; 

	PLIST_ENTRY pVirtAppListFlink;
	PVIRT_APPL_LIST_ENTRY pvirt_appl_list_entry;

	InitializeListHead( &PidList );
	FillPidList( &PidList );

	if ( IsListEmpty( &PidList ) )
		return	STATUS_SUCCESS;

	if ( IsListEmpty( &gVirtAppList ) )
	{
		RemovePidList( &PidList );
		return	STATUS_SUCCESS;
	}

	AcquireResourceExclusive( &gVirtAppListLock );

	pPidListFlink = PidList.Flink;
	while (pPidListFlink != &PidList)
	{
		ppid_list_entry = CONTAINING_RECORD( pPidListFlink, PID_LIST_ENTRY, ListEntry );

		pVirtAppListFlink = gVirtAppList.Flink;
		while ( pVirtAppListFlink != &gVirtAppList )
		{
			pvirt_appl_list_entry = CONTAINING_RECORD( pVirtAppListFlink, VIRT_APPL_LIST_ENTRY, ListEntry );

			// Если PIDы совпадают добавляем правило для него в наше дерево
			//if ( ComparePidAndApplInfo( ppid_list_entry, &(pvirt_appl_list_entry->app_rul.appl_info) ) == 0 )
			if ( RtlCompareUnicodeString( &ppid_list_entry->usNativePath, &pvirt_appl_list_entry->appl.usAppPath,TRUE ) == 0 )
				if ( !NT_SUCCESS( Virt_InsertElementInPidTree(
					pPidTree,
					ppid_list_entry->pid,
					pvirt_appl_list_entry->appl.usAppPath,
					pvirt_appl_list_entry->appl.usSBPath,
					pvirt_appl_list_entry->appl.usSBVolumeName,
					pvirt_appl_list_entry->appl.logMask) ) )
					
				{
					_dbg_break_;
				}

				pVirtAppListFlink = pVirtAppListFlink->Flink;
		}

		pPidListFlink = pPidListFlink->Flink;
	}

	ReleaseResource( &gVirtAppListLock );

	RemovePidList( &PidList );

	return STATUS_SUCCESS;
}


PVIRT_PID_TREE_ENTRY
Virt_GetPidTreeEntryByPid(
					 __in PRTL_GENERIC_TABLE pPidTree, 
					 __in HANDLE pid
					 )
{
	VIRT_PID_TREE_ENTRY treeEntry;
	VIRT_PID_TREE_ENTRY *ptreeEntry;

	memset( &treeEntry, 0, sizeof(VIRT_PID_TREE_ENTRY) );

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

PVIRT_PID_TREE
Virt_GetPidTreePointer()
{
	return (PVIRT_PID_TREE)InterlockedExchangePointer( &gpVirtPidTree, gpVirtPidTree );
}

PVIRT_PID_TREE
Virt_SwitchPidTreePointer(
					 __in_opt PVIRT_PID_TREE newVirtPidTreePointer
					 )
{
	return (PVIRT_PID_TREE)InterlockedExchangePointer( &gpVirtPidTree, newVirtPidTreePointer );
}

IO_WORKITEM_ROUTINE RemoveVirtPidTreeRoutine;

void Virt_RemovePidTreeRoutine( 
						  __in PDEVICE_OBJECT	DeviceObject,
						  __in PVOID  p 
						  )
{
	PVIRT_PID_TREE pVirtPidTree;
	PVIRT_PID_TREE_ENTRY pvirt_pid_tree_entry;

	UNREFERENCED_PARAMETER( DeviceObject );

	pVirtPidTree = (PVIRT_PID_TREE)p;

	WaitInterlockedObj(&(pVirtPidTree->readers_cnt));

	pvirt_pid_tree_entry = ( PVIRT_PID_TREE_ENTRY ) RtlGetElementGenericTable(&(pVirtPidTree->PidTree), 0);
	while ( pvirt_pid_tree_entry )
	{
		if ( pvirt_pid_tree_entry->appl.usAppPath.Buffer )
			ExFreePool ( pvirt_pid_tree_entry->appl.usAppPath.Buffer );

		if ( pvirt_pid_tree_entry->appl.usSBPath.Buffer )
			ExFreePool ( pvirt_pid_tree_entry->appl.usSBPath.Buffer );

		if ( pvirt_pid_tree_entry->appl.usSBVolumeName.Buffer )
			ExFreePool ( pvirt_pid_tree_entry->appl.usSBVolumeName.Buffer );

		RtlDeleteElementGenericTable( &(pVirtPidTree->PidTree), pvirt_pid_tree_entry );

		pvirt_pid_tree_entry = (PVIRT_PID_TREE_ENTRY)RtlGetElementGenericTable(&(pVirtPidTree->PidTree),0);
	}

	ExFreePool (pVirtPidTree);

	InterlockedDecrement( &gVirtPidTreeRemCnt );
}


VOID
Virt_RemovePidTree( 
			  __in PVIRT_PID_TREE pVirtPidTree 
			  )
{
	PIO_WORKITEM pItem = IoAllocateWorkItem( Globals.m_FilterControlDeviceObject );

	if (!pItem)
	{
		_dbg_break_;
	}

	InterlockedIncrement( &gVirtPidTreeRemCnt );

	IoQueueWorkItem( pItem, Virt_RemovePidTreeRoutine, DelayedWorkQueue, pVirtPidTree );
}


//перестройка дерева PIDов:
// - если пришло новое правило
// - если запустили новый процесс
// - если удалили процесс
NTSTATUS
Virt_RebuildPidTree()
{
	PVIRT_PID_TREE pNewVirtPidTree;
	PVIRT_PID_TREE pOldVirtPidTree;

	NTSTATUS status=STATUS_SUCCESS;

	pNewVirtPidTree=ExAllocatePoolWithTag( PagedPool,sizeof(VIRT_PID_TREE), tag_virt7 );
	if( !pNewVirtPidTree )
		return STATUS_NO_MEMORY;

	memset(pNewVirtPidTree,0, sizeof(PID_TREE));

	Virt_InitPidTree(&(pNewVirtPidTree->PidTree));

	status = Virt_GeneratePidTree(&(pNewVirtPidTree->PidTree));

	//если дерево не создалось или оно пустое, то удаляем его
	if ( !NT_SUCCESS(status) || !RtlGetElementGenericTable(&(pNewVirtPidTree->PidTree),0) )
	{
		ExFreePool (pNewVirtPidTree);
		pNewVirtPidTree=NULL;
	}

	pOldVirtPidTree=Virt_SwitchPidTreePointer( pNewVirtPidTree );

	if ( pOldVirtPidTree )
		Virt_RemovePidTree(pOldVirtPidTree);

// 	DoTraceEx (
// 		TRACE_LEVEL_INFORMATION, DC_HIPS, "RebuildPidTree: newPid=%p oldPid=%p, status=0x%x",
// 		pNewVirtPidTree,
// 		pOldVirtPidTree,
// 		status
// 		);

	return status;
}

//должна запускаться при инициализации драйвера
void VirtInit()
{
	Virt_SwitchPidTreePointer( NULL );
	InitializeListHead( &gVirtAppList );
	ExInitializeResourceLite( &gVirtAppListLock );
}

// запуск при выгрузке драйвера
void VirtDone()
{
	PVIRT_PID_TREE pOldVirtPidTree;
	pOldVirtPidTree = Virt_SwitchPidTreePointer( NULL );

	if (pOldVirtPidTree)
		Virt_RemovePidTree( pOldVirtPidTree );

	WaitInterlockedObj( &gVirtPidTreeRemCnt );

	Virt_RemoveAllApp();

	ExDeleteResourceLite( &gVirtAppListLock );
}

PFILTER_EVENT_PARAM
Virt_EvContext (
	__in_opt PSID	psid,
	__in ULONG	disiredOp,
	__in HANDLE pid,
	__in UNICODE_STRING usFilePath,
	__in UNICODE_STRING usVolumeName,
	__in UNICODE_STRING usVirtFilePath,
	__in UNICODE_STRING usVirtVolumeName
)
{
	PFILTER_EVENT_PARAM pParam = NULL;

	ULONG ParamsCount = 0;
	ULONG params_size = sizeof(FILTER_EVENT_PARAM);

	ParamsCount++;											//1: PID
	params_size += sizeof(SINGLE_PARAM) + sizeof(HANDLE);

	ParamsCount++;											//2: FilePath + \0
	params_size += sizeof(SINGLE_PARAM) + usFilePath.Length + sizeof(WCHAR);


	ParamsCount++;											//3: Volume + \0
	params_size += sizeof(SINGLE_PARAM) + usVolumeName.Length + sizeof(WCHAR);	

	ParamsCount++;											//4: VirtFilePath + \0
	params_size += sizeof(SINGLE_PARAM) + usVirtFilePath.Length + sizeof(WCHAR);


	ParamsCount++;											//5: VirtVolume + \0
	params_size += sizeof(SINGLE_PARAM) + usVirtVolumeName.Length + sizeof(WCHAR);	
	
	if ( psid )
	{
		ParamsCount++;										//6: SID
		params_size += sizeof(SINGLE_PARAM) + RtlLengthSid( psid );
	}

	ParamsCount++;											//7: desiredOp
	params_size += sizeof(SINGLE_PARAM) + sizeof(ULONG);

	pParam = ExAllocatePoolWithTag( PagedPool, params_size, tag_virtH );
	if (pParam)
	{
		PSINGLE_PARAM pSingleParam;
		FILTER_PARAM_COMMONINIT( pParam, FLTTYPE_SYSTEM, MJ_SYSTEM_VIRT, MI_SYSTEM_VIRT_ASK, PreProcessing, ParamsCount );

		pSingleParam = (PSINGLE_PARAM) pParam->Params;
		//1: PID
		SINGLE_PARAM_INIT_NONE(pSingleParam, _PARAM_OBJECT_CLIENTID1);
		pSingleParam->ParamSize=sizeof(HANDLE);
		*(HANDLE*)pSingleParam->ParamValue = pid;

		
		if (usFilePath.Length)
		{
			SINGLE_PARAM_SHIFT( pSingleParam );		//2: FilePath + \0
			SINGLE_PARAM_INIT_NONE( pSingleParam, _PARAM_OBJECT_URL_W );

			memcpy( pSingleParam->ParamValue, usFilePath.Buffer, usFilePath.Length );
			((PCHAR)pSingleParam->ParamValue)[usFilePath.Length] = 0;
			((PCHAR)pSingleParam->ParamValue)[usFilePath.Length + 1] = 0;
			pSingleParam->ParamSize = usFilePath.Length + sizeof(WCHAR);
		}

		if ( usVolumeName.Length )
		{
			SINGLE_PARAM_SHIFT( pSingleParam );		//3: Volume + \0
			SINGLE_PARAM_INIT_NONE( pSingleParam, _PARAM_OBJECT_VOLUME_NAME_W );

			memcpy( pSingleParam->ParamValue, usVolumeName.Buffer, usVolumeName.Length );
			((PCHAR)pSingleParam->ParamValue)[usVolumeName.Length] = 0;
			((PCHAR)pSingleParam->ParamValue)[usVolumeName.Length + 1] = 0;
			pSingleParam->ParamSize = usVolumeName.Length + sizeof(WCHAR);
		}

		if (usVirtFilePath.Length)
		{
			SINGLE_PARAM_SHIFT( pSingleParam );		//4: VirtFilePath + \0
			SINGLE_PARAM_INIT_NONE( pSingleParam, _PARAM_OBJECT_URL_DEST_W );

			memcpy( pSingleParam->ParamValue, usVirtFilePath.Buffer, usVirtFilePath.Length );
			((PCHAR)pSingleParam->ParamValue)[usVirtFilePath.Length] = 0;
			((PCHAR)pSingleParam->ParamValue)[usVirtFilePath.Length + 1] = 0;
			pSingleParam->ParamSize = usVirtFilePath.Length + sizeof(WCHAR);
		}

		if ( usVirtVolumeName.Length )
		{
			SINGLE_PARAM_SHIFT( pSingleParam );		//5: VirtVolume + \0
			SINGLE_PARAM_INIT_NONE( pSingleParam, _PARAM_OBJECT_VOLUME_NAME_DEST_W );

			memcpy( pSingleParam->ParamValue, usVirtVolumeName.Buffer, usVirtVolumeName.Length );
			((PCHAR)pSingleParam->ParamValue)[usVirtVolumeName.Length] = 0;
			((PCHAR)pSingleParam->ParamValue)[usVirtVolumeName.Length + 1] = 0;
			pSingleParam->ParamSize = usVirtVolumeName.Length + sizeof(WCHAR);
		}

		if (psid)
		{
			SINGLE_PARAM_SHIFT( pSingleParam );		//6 SID
			SINGLE_PARAM_INIT_NONE(pSingleParam, _PARAM_OBJECT_SID);
			pSingleParam->ParamSize = RtlLengthSid( psid );
			memcpy( pSingleParam->ParamValue, psid, RtlLengthSid( psid ) );
		}

		SINGLE_PARAM_SHIFT( pSingleParam );			//7: disiredOp
		SINGLE_PARAM_INIT_ULONG( pSingleParam, _PARAM_OBJECT_ACCESSATTR, disiredOp );
		
	}

	return pParam;
}

BOOLEAN
CheckExistFileAndGetType (
	__in PUNICODE_STRING FilePath,
	__in PFLT_INSTANCE Instance,
	__out FILE_TYPE *pFileType
)
{
	NTSTATUS status;
	HANDLE	FileHandle = NULL;
	OBJECT_ATTRIBUTES  ObjectAttributes;
	IO_STATUS_BLOCK  ioStatus;
	BOOLEAN NonDirFlag = FALSE;
	BOOLEAN isExist = FALSE;
	ULONG myCreateOptions;

	if ( !FilePath )
		return isExist;

	if ( !Instance )
		return isExist;
	
	if ( pFileType )
		*pFileType = FT_Unknown;

// 	if ( !pFileType )
// 		return STATUS_INVALID_PARAMETER_3;

	myCreateOptions = FILE_SYNCHRONOUS_IO_NONALERT;
	
	InitializeObjectAttributes( &ObjectAttributes, FilePath, OBJ_CASE_INSENSITIVE | OBJ_KERNEL_HANDLE, NULL, NULL );

	status = AddInvisibleThread(PsGetCurrentThreadId());
	if ( !NT_SUCCESS ( status ) )
	{
		return isExist;
	}

	status = FltCreateFile (
		Globals.m_Filter,
		Instance, 
		&FileHandle,
		/*FILE_READ_DATA |*/ SYNCHRONIZE,//DesiredAccess,
		&ObjectAttributes,
		&ioStatus,
		NULL,
		FILE_ATTRIBUTE_NORMAL,//FileAttributes,
		FILE_SHARE_READ | FILE_SHARE_WRITE | FILE_SHARE_DELETE ,//ShareAccess,
		FILE_OPEN, //CreateDisposition,
		myCreateOptions,
		NULL,
		0,
		0
		);

	if ( NT_SUCCESS ( status ) || status == STATUS_ACCESS_DENIED )
	{
		isExist = TRUE;

		if (FileHandle)
			FltClose( FileHandle );
	
		//если объект существует и нужно определить его тип
		if ( NT_SUCCESS ( status ) && pFileType )
		{
			FileHandle = NULL;

			SetFlag ( myCreateOptions, FILE_DIRECTORY_FILE );
			status = FltCreateFile (
				Globals.m_Filter,
				Instance, 
				&FileHandle,
				/*FILE_READ_DATA |*/ SYNCHRONIZE,//DesiredAccess,
				&ObjectAttributes,
				&ioStatus,
				NULL,
				FILE_ATTRIBUTE_NORMAL,//FileAttributes,
				FILE_SHARE_READ | FILE_SHARE_WRITE | FILE_SHARE_DELETE ,//ShareAccess,
				FILE_OPEN, //CreateDisposition,
				myCreateOptions,
				NULL,
				0,
				0
				);
			
			if ( NT_SUCCESS ( status ) || status == STATUS_ACCESS_DENIED )
			{
				*pFileType = FT_Directory;
			}
			else
			{
				*pFileType = FT_File;
			}

			if ( FileHandle )
				FltClose( FileHandle );
		}
	}
	
	DelInvisibleThread( PsGetCurrentThreadId(), FALSE );

	return isExist;	
}

BOOLEAN
CheckExistRegKeyValue (
	__in PUNICODE_STRING KeyPath,
	__in_opt PUNICODE_STRING ValueName
)
{
	NTSTATUS status;
	HANDLE	FileHandle = NULL;
	OBJECT_ATTRIBUTES  ObjectAttributes;
	BOOLEAN isExist = FALSE;
	ACCESS_MASK DesiredRight;
	
	DesiredRight = 0;

	if ( !KeyPath )
		return isExist;
	
	if ( ValueName )
		SetFlag( DesiredRight, KEY_QUERY_VALUE );
		
	
// 	if ( !pFileType )
// 		return STATUS_INVALID_PARAMETER_3;

	InitializeObjectAttributes( &ObjectAttributes, KeyPath, OBJ_CASE_INSENSITIVE | OBJ_KERNEL_HANDLE, NULL, NULL );

	status = AddInvisibleThread(PsGetCurrentThreadId());
	if ( !NT_SUCCESS ( status ) )
		return isExist;
	

	status = ZwOpenKey( &FileHandle,
		KEY_ALL_ACCESS,
		&ObjectAttributes );

	if ( status = STATUS_ACCESS_DENIED && !ValueName )
		isExist = TRUE;

	if ( NT_SUCCESS ( status ) )
	{
		if (ValueName)
		{
			isExist = FALSE	;
			if ( NT_SUCCESS( status ) )
			{
				PKEY_VALUE_BASIC_INFORMATION pkvbi = NULL;
				ULONG kvbi_size = sizeof(KEY_VALUE_BASIC_INFORMATION) + ValueName->Length;
				pkvbi = (PKEY_VALUE_BASIC_INFORMATION) ExAllocatePoolWithTag(PagedPool, kvbi_size, tag_virtJ );
				if (pkvbi)
				{
					ULONG retSize = 0;
					status = ZwQueryValueKey(
						FileHandle,
						ValueName,
						KeyValueBasicInformation,
						pkvbi,
						kvbi_size,
						&retSize
						);
					
					if ( NT_SUCCESS(status) || status == STATUS_ACCESS_DENIED )
					{
						isExist = TRUE;
					}

					ExFreePool(pkvbi);
				}
			
			}
		}
		
		ZwClose(FileHandle);
	}
	
	DelInvisibleThread( PsGetCurrentThreadId(), FALSE );

	return isExist;	
}

NTSTATUS
Virt_CreateKey( 
	__in PUNICODE_STRING KeyName
)
{
	NTSTATUS status = STATUS_UNSUCCESSFUL;
	OBJECT_ATTRIBUTES	attributes;
	HANDLE KeyHandle;
	
	InitializeObjectAttributes( &attributes,
		KeyName,
		OBJ_CASE_INSENSITIVE,
		NULL,
		NULL );

	status = ZwCreateKey(
		&KeyHandle,
		KEY_CREATE_SUB_KEY | KEY_SET_VALUE,
		&attributes,
		0,
		NULL,
		REG_OPTION_NON_VOLATILE,
		NULL
		);
	
	if ( NT_SUCCESS( status ) )
		ZwClose(KeyHandle);

	return status;
}

tefVerdict
Virt_GetActionRegistry (
	__in REG_NOTIFY_CLASS RegNotifyClass,
	__in ACCESS_MASK DesiredAccess,
	__in_opt PSID	psid,
	__in HANDLE pid,
	__in UNICODE_STRING usKeyPath,
	__in_opt UNICODE_STRING usValueName,
	__in UNICODE_STRING usVirtKeyPath	
)
{

	//NTSTATUS status;
	//BOOLEAN isDir;
	tefVerdict verdict = efVerdict_Default;
	
	UNREFERENCED_PARAMETER( psid );
	UNREFERENCED_PARAMETER( pid );

	switch( RegNotifyClass )
	{
	
	// Можно попробовать так:
	// проверяем есть ли уже такой раздел в реальном реестре 
	// есть:
	//	смотрим DesiredAccess - 
	//		 если запрашиваются права на модификацию, 
	//		 то создаем свой Key в SB и перенаправляем туда
	//		 нет - пропускаем
	// нет - перенаправляем в песочницу
	// Сейчас просто все попытки создания перенаправляются в SB
	case RegNtPreCreateKey:
		verdict = efVerdict_Allow;
		break;
	
    // непонятно нужно ли ее как-то обрабатывать
// 	case RegNtPreSetInformationKey:
// 
// 		break;

	
	// непонятно нужно ли ее как-то обрабатывать
	// предварительно пропускаем
// 	case RegNtPreRenameKey:
// 
// 		break;

	
	//смотрим DesiredAccess - 
	// если запрашиваются права на модификацию, 
	//	то создаем свой Key и перенаправляем туда
	case RegNtPreOpenKey:
		if ( !IS_DESIRED_READ_ONLY_REG (DesiredAccess) )
		{
			//если в реальном реестре уже есть такой ключ - прямо тут его и создать
			if ( CheckExistRegKeyValue(&usKeyPath, NULL) )
				Virt_CreateKey( &usVirtKeyPath );
			
			verdict = efVerdict_Allow;
		}
	break;

// 	пропускаем (пока)
// 	    	case RegNtPreQueryKey:
// 	    
// 	    	    break;
	
// 	case RegNtPreDeleteKey:
// 
// 	break;

	//если есть в SB перенаправляем туда
	//нет пропускаем
 	case RegNtPreQueryValueKey:
	if ( CheckExistRegKeyValue(&usVirtKeyPath, &usValueName) )
	{
			verdict = efVerdict_Allow;
	}

    break;
	
// 	перенаправляем в SB, но эта обработка не должна понадобиться т.к. OpenKey будет уже перенаправлен
//	case RegNtPreSetValueKey:
//		
//	break;

	//перенаправляем в SB
 	case RegNtPreDeleteValueKey:
		if ( CheckExistRegKeyValue(&usVirtKeyPath, &usValueName) )
			verdict = efVerdict_Allow;
		else
			verdict = efVerdict_Deny;
 		break;

	default :
		_dbg_break_;
	    break;
	}

	return verdict;
}

tefVerdict
Virt_GetAction (
	__in ACCESS_MASK DesiredAccess,
	__in ULONG  CreateOptions,
	__in ULONG  CreateDisposition,
	__in PFLT_INSTANCE Instance,
	__in_opt PSID	psid,
	__in HANDLE pid,
	__in UNICODE_STRING usFilePath,
	__in UNICODE_STRING usVolumeName,
	__in UNICODE_STRING usVirtFilePath,
	__in UNICODE_STRING usVirtVolumeName
)
{
	
	//NTSTATUS status;
	//BOOLEAN isDir;
	tefVerdict verdict = efVerdict_Default;
	PFILTER_EVENT_PARAM pEvent = NULL;
	
	FILE_TYPE	desiredType = FT_Unknown;
	
	FILE_TYPE	existType = FT_Unknown;
	FILE_TYPE	existVirtType = FT_Unknown;
	
	BOOLEAN existFile = FALSE;
	BOOLEAN existVirtFile = FALSE;

	if ( BooleanFlagOn( CreateOptions, FILE_NON_DIRECTORY_FILE ) )
		desiredType = FT_File;

	if ( BooleanFlagOn( CreateOptions, FILE_DIRECTORY_FILE ) )
		desiredType = FT_Directory;

	//if it is a exist directory and we open only, we should pass down it without a redirection in a sandbox
	//status = Virt_IsDir( &usFilePath, Instance, &isDir );
	existFile = CheckExistFileAndGetType ( &usFilePath, Instance, &existType );
	existVirtFile = CheckExistFileAndGetType ( &usVirtFilePath, Instance, &existVirtType );
	
	//если только открываем: директории в реальной ФС, файлы если есть в SB то в SB
	if ( CreateDisposition == FILE_OPEN  )
	{
		//если только открываем файл, доступный в SB
		if ( (desiredType == FT_File || desiredType == FT_Unknown) && 
			existVirtFile && 
			existVirtType == FT_File 
			)
		{
			verdict = efVerdict_Allow;
			return verdict;
		}

		//если только открываем директорию существующую в реальной ФС
// 		if ( (desiredType == FT_Directory || desiredType == FT_Unknown) && 
// 			existFile && 
// 			existType == FT_Directory 
// 			)
// 		{
// 			verdict = efVerdict_Default;
// 			return verdict;
// 		}

		if ( IS_DESIRED_READ_ONLY ( DesiredAccess ) )
		{	
			
			//если есть в реальной ФС или нет нигде то пропускаем запрос в реальную ФС
			// а если в реальной нет, но есть в SB  - перенаправляем в SB
			if ( existFile || ( !existFile && !existVirtFile ) )
			{
				verdict = efVerdict_Default;
				return verdict;
			}
		}
	}

	//если такой объект(файл/каталог) есть в SandBox
	if ( (desiredType == existVirtType || desiredType == FT_Unknown) && existVirtFile )
	{
		verdict = efVerdict_Allow;
		return verdict;
	}

	//если такой объект(файл/каталог) есть в реальной ФС
	if ( (desiredType == existType || desiredType == FT_Unknown) && existFile )
	{
		pEvent = Virt_EvContext (
			psid,
			(existType==FT_Directory) ? VA_MakeDir : VA_CopyFile,
			pid,
			usFilePath,
			usVolumeName,
			usVirtFilePath,
			usVirtVolumeName
			);

		if ( pEvent )
		{	
			verdict = FilterEventImp( pEvent, NULL, 0 );
			ExFreePool(pEvent);
		}

		return verdict;
	}

	
 	//если такого  объекта нет в реальной ФС, то надо создать 
	//промежуточные каталоги в SB и перенаправить в SB
	if ( !existFile || desiredType != existType )
	{
		UNICODE_STRING tmpusVirtFilePath;
		USHORT i = 0;

		memcpy (&tmpusVirtFilePath, &usVirtFilePath, sizeof(UNICODE_STRING) );
		
		i = tmpusVirtFilePath.Length/2 - 1;
		while ( i > 0 )
		{
			if (tmpusVirtFilePath.Buffer[ i ] == L'\\' )
				break;
			i--;
		}
		
		tmpusVirtFilePath.Length = 	i * 2 ;

		pEvent = Virt_EvContext (
			psid,
			VA_MakeDir,
			pid,
			usFilePath,
			usVolumeName,
			tmpusVirtFilePath,
			usVirtVolumeName
			);

		if ( pEvent )
		{	
			verdict = FilterEventImp( pEvent, NULL, 0 );
			ExFreePool(pEvent);
		}

	}
	
	return verdict;
}



tefVerdict
GetVirtualPathRegistryEx(
	__in PSID psid,
	__in HANDLE pid,
	__in PUNICODE_STRING pusKeyPath,
	__in_opt PUNICODE_STRING pusValueName,	
	__in ACCESS_MASK DesiredAccess,
	__in REG_NOTIFY_CLASS RegNotifyClass,
	__out PUNICODE_STRING pusNewKeyPath
)
{
	tefVerdict verdict = efVerdict_Default;
	VIRT_ACTION VirtAction = VA_Noting;
	PWCHAR wcNewPath = NULL;
	ULONG wcNewPathSize = 0;
	NTSTATUS status = STATUS_UNSUCCESSFUL;

	__try
	{
		//RtlZeroMemory( pusFilePath, sizeof(UNICODE_STRING) );

		PVIRT_PID_TREE pVirtPidTree;
		PVIRT_PID_TREE_ENTRY pVirtTreeEntry;
		PFILTER_EVENT_PARAM pEvent = NULL;
		BOOLEAN isDir = FALSE;



		pVirtPidTree = Virt_GetPidTreePointer();
		if(!pVirtPidTree)
		{
			status = STATUS_UNSUCCESSFUL;
			__leave;
		}

		InterlockedIncrement( &pVirtPidTree->readers_cnt);
		pVirtTreeEntry = Virt_GetPidTreeEntryByPid( &pVirtPidTree->PidTree, pid );

		if (!pVirtTreeEntry) 
		{		
			InterlockedDecrement( &pVirtPidTree->readers_cnt );
			status = STATUS_UNSUCCESSFUL;
			__leave;
		}

		//если запрос уже в Sandbox, то просто пропускаем
		if ( pusKeyPath->Length > pVirtTreeEntry->appl.usSBRegistry.Length )
		{
			if ( _wcsnicmp( 
				pusKeyPath->Buffer, 
				pVirtTreeEntry->appl.usSBRegistry.Buffer, 
				pVirtTreeEntry->appl.usSBRegistry.Length / sizeof(WCHAR) ) == 0 )
			{
				InterlockedDecrement( &pVirtPidTree->readers_cnt );
				verdict = efVerdict_Default;
				status = STATUS_SUCCESS;
				__leave;
			}

		}

		wcNewPathSize = pVirtTreeEntry->appl.usSBRegistry.Length + sizeof(WCHAR) + pusKeyPath->Length;
		wcNewPath = ExAllocatePoolWithTag( PagedPool, wcNewPathSize, tag_virtI );
		if ( !wcNewPath )
		{
			InterlockedDecrement( &pVirtPidTree->readers_cnt );
			status = STATUS_UNSUCCESSFUL;
			__leave;
		}

		RtlInitEmptyUnicodeString( pusNewKeyPath, wcNewPath, (USHORT)wcNewPathSize );

		status = RtlAppendUnicodeStringToString (
			pusNewKeyPath,
			&pVirtTreeEntry->appl.usSBRegistry
			);
		if (!NT_SUCCESS( status ) )
		{
			InterlockedDecrement( &pVirtPidTree->readers_cnt );
			__leave;
		}

		status = RtlAppendUnicodeStringToString (
			pusNewKeyPath,
			pusKeyPath
			);

		if (!NT_SUCCESS( status ) )
		{
			InterlockedDecrement( &pVirtPidTree->readers_cnt );
			__leave;
		}

		
 		verdict = Virt_GetActionRegistry (
 									RegNotifyClass,
									DesiredAccess,
 									psid,
 									pid,
 									*pusKeyPath,
									*pusValueName,
 									*pusNewKeyPath
 									);


		InterlockedDecrement( &pVirtPidTree->readers_cnt );
		//verdict = TRUE;
		status = STATUS_SUCCESS;
	}
	__finally
	{
		if ( !NT_SUCCESS( status ) || verdict == efVerdict_Default || efIsDeny( verdict ) )
		{
			if ( wcNewPath )
				ExFreePool( wcNewPath );

			RtlZeroMemory( pusNewKeyPath, sizeof(UNICODE_STRING) );
		}

	}

	return  verdict;
}

tefVerdict
GetVirtualPathEx(
	__in PSID psid,
	__in HANDLE pid,
	__in PFLT_INSTANCE Instance,
	__in PUNICODE_STRING pusFilePath,
	__in PUNICODE_STRING pusVolumeName,
	__in ACCESS_MASK DesiredAccess,
	__in ULONG  CreateOptions,
	__in ULONG  CreateDisposition,
	__out PUNICODE_STRING pusNewFilePath
)
{
	tefVerdict verdict = efVerdict_Default;
	VIRT_ACTION VirtAction = VA_Noting;
	PWCHAR wcNewPath = NULL;
	ULONG wcNewPathSize = 0;
	NTSTATUS status = STATUS_UNSUCCESSFUL;

	__try
	{
		//RtlZeroMemory( pusFilePath, sizeof(UNICODE_STRING) );

		PVIRT_PID_TREE pVirtPidTree;
		PVIRT_PID_TREE_ENTRY pVirtTreeEntry;
		PFILTER_EVENT_PARAM pEvent = NULL;
		BOOLEAN isDir = FALSE;
		
		

		pVirtPidTree = Virt_GetPidTreePointer();
		if(!pVirtPidTree)
		{
			status = STATUS_UNSUCCESSFUL;
			__leave;
		}

		InterlockedIncrement( &pVirtPidTree->readers_cnt);
		pVirtTreeEntry = Virt_GetPidTreeEntryByPid( &pVirtPidTree->PidTree, pid );

		if (!pVirtTreeEntry) 
		{		
			InterlockedDecrement( &pVirtPidTree->readers_cnt );
			status = STATUS_UNSUCCESSFUL;
			__leave;
		}
		
		//если запрос уже в Sandbox, то просто пропускаем
		if ( pusFilePath->Length > pVirtTreeEntry->appl.usSBPath.Length )
		{
			if ( _wcsnicmp( 
					pusFilePath->Buffer, 
					pVirtTreeEntry->appl.usSBPath.Buffer, 
					pVirtTreeEntry->appl.usSBPath.Length / sizeof(WCHAR) ) == 0 )
			{
				InterlockedDecrement( &pVirtPidTree->readers_cnt );
				verdict = efVerdict_Default;
				status = STATUS_SUCCESS;
				__leave;
			}

		}

		wcNewPathSize = pVirtTreeEntry->appl.usSBPath.Length + sizeof(WCHAR) + pusFilePath->Length;
		wcNewPath = ExAllocatePoolWithTag( PagedPool, wcNewPathSize, tag_virt8 );
		if ( !wcNewPath )
		{
			InterlockedDecrement( &pVirtPidTree->readers_cnt );
			status = STATUS_UNSUCCESSFUL;
			__leave;
		}
		
		RtlInitEmptyUnicodeString( pusNewFilePath, wcNewPath, (USHORT)wcNewPathSize );
		
		status = RtlAppendUnicodeStringToString (
			pusNewFilePath,
			&pVirtTreeEntry->appl.usSBPath
			);
		if (!NT_SUCCESS( status ) )
		{
			InterlockedDecrement( &pVirtPidTree->readers_cnt );
			__leave;
		}

		status = RtlAppendUnicodeStringToString (
			pusNewFilePath,
			pusFilePath
			);
		
		if (!NT_SUCCESS( status ) )
		{
			InterlockedDecrement( &pVirtPidTree->readers_cnt );
			__leave;
		}
		
		verdict = Virt_GetAction (
								DesiredAccess,
								CreateOptions,
								CreateDisposition,
								Instance,
								psid,
								pid,
								*pusFilePath,
								*pusVolumeName,
								*pusNewFilePath,
								pVirtTreeEntry->appl.usSBVolumeName
								);

		
		InterlockedDecrement( &pVirtPidTree->readers_cnt );
		//verdict = TRUE;
		status = STATUS_SUCCESS;
	}
	__finally
	{
		if ( !NT_SUCCESS( status ) || verdict == efVerdict_Default || efIsDeny( verdict ) )
		{
			if ( wcNewPath )
				ExFreePool( wcNewPath );
						
			RtlZeroMemory( pusNewFilePath, sizeof(UNICODE_STRING) );
		}
		
	}

	return  verdict;
}


tefVerdict
GetVirtualPathReg(
	__in HANDLE pid,
	__in PUNICODE_STRING pusKeyPath,
	__in_opt PUNICODE_STRING pusValueName,
	__in ACCESS_MASK DesiredAccess,
	__in REG_NOTIFY_CLASS RegNotifyClass,
	__out PUNICODE_STRING pusNewKeyPath
)
{
	tefVerdict verdict = efVerdict_Default;
	PSID psid = NULL;

	if ( !NT_SUCCESS( SeGetUserSid(NULL, &psid) ) )
		psid = NULL;

	verdict = GetVirtualPathRegistryEx(
		psid,
		pid,
		pusKeyPath,
		pusValueName,
		DesiredAccess,
		RegNotifyClass,
		pusNewKeyPath
		);

	if (psid)
		ExFreePool(psid);

	return verdict;

}

tefVerdict
GetVirtualPathFile(
	__in PFLT_CALLBACK_DATA Data,
	__in HANDLE pid,
	__in PFLT_INSTANCE Instance,
	__in PUNICODE_STRING pusFilePath,
	__in PUNICODE_STRING pusVolumeName,
	//__in PFLT_FILE_NAME_INFORMATION pFileNameInfo,
	__in ACCESS_MASK DesiredAccess,
	__in ULONG  CreateOptions,
	__in ULONG  CreateDisposition,
	__out PUNICODE_STRING pusNewFilePath
)
{
	tefVerdict verdict = efVerdict_Default;
	PSID psid = NULL;

	if ( !NT_SUCCESS( SeGetUserSid(Data, &psid) ) )
		psid = NULL;

	verdict = GetVirtualPathEx(
		psid,
		pid,
		Instance,
		pusFilePath,
		pusVolumeName,
		DesiredAccess,
		CreateOptions,
		CreateDisposition,
		pusNewFilePath
		);

	if (psid)
		ExFreePool(psid);

	return verdict;

}

BOOLEAN 
Virt_IsProcessInSB(
	__in HANDLE pid
)
{
	PVIRT_PID_TREE pVirtPidTree;
	PVIRT_PID_TREE_ENTRY pVirtTreeEntry;
	BOOLEAN ret = FALSE;

	pVirtPidTree = Virt_GetPidTreePointer();
	if(!pVirtPidTree)
	{
		ret = FALSE;
		return FALSE;
	}

	InterlockedIncrement( &pVirtPidTree->readers_cnt);
	pVirtTreeEntry = Virt_GetPidTreeEntryByPid( &pVirtPidTree->PidTree, pid );

	if (pVirtTreeEntry) 
		ret = TRUE;		
	
	InterlockedDecrement( &pVirtPidTree->readers_cnt );
	
	return ret;
}

PFILTER_EVENT_PARAM
Virt_ProcEvContext (
	__in ULONG isCreate,
	__in HANDLE pid
)
{
	PFILTER_EVENT_PARAM pParam = NULL;

	ULONG ParamsCount = 0;
	ULONG params_size = sizeof(FILTER_EVENT_PARAM);

	ParamsCount++;											//1: PID
	params_size += sizeof(SINGLE_PARAM) + sizeof(HANDLE);

	ParamsCount++;											//2: isCreate
	params_size += sizeof(SINGLE_PARAM) + sizeof(ULONG);
		
	pParam = ExAllocatePoolWithTag( PagedPool, params_size, tag_virtG );
	if (pParam)
	{
		PSINGLE_PARAM pSingleParam;
		FILTER_PARAM_COMMONINIT( pParam, FLTTYPE_SYSTEM, MJ_SYSTEM_VIRT, MI_SYSTEM_VIRT_LOG, PreProcessing, ParamsCount );

		pSingleParam = (PSINGLE_PARAM) pParam->Params;
		//1: PID
		SINGLE_PARAM_INIT_NONE(pSingleParam, _PARAM_OBJECT_CLIENTID1);
		pSingleParam->ParamSize=sizeof(HANDLE);
		*(HANDLE*)pSingleParam->ParamValue = pid;

		SINGLE_PARAM_SHIFT( pSingleParam );			//2: isCreate
		SINGLE_PARAM_INIT_ULONG( pSingleParam, _PARAM_OBJECT_ACCESSATTR, isCreate );

	}

	return pParam;
}