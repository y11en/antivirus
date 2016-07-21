#include "devlist.h"
#include "tags.h"


LIST_ENTRY	gDeviceList;
ERESOURCE	gDeviceListLock;

extern BOOLEAN AcquireResourceExclusive ( __inout PERESOURCE Resource );
extern VOID ReleaseResource( __inout PERESOURCE Resource );

void
InitializeDeviceListEx( 
	__in PLIST_ENTRY pDevRulHead 
)
{
	InitializeListHead(pDevRulHead);
}

void
InitializeDeviceList()
{
	InitializeDeviceListEx(&gDeviceList);
}


NTSTATUS
InsertDeviceListEx(
	__in PLIST_ENTRY pListHead,
	__in PDEVICE_NAME pDevName
)
{
	PDEVICE_LIST_ENTRY pdev_list_entry;
	NTSTATUS status = STATUS_UNSUCCESSFUL;

	__try
	{
		pdev_list_entry = ExAllocatePoolWithTag( PagedPool, sizeof (DEVICE_LIST_ENTRY), tag_devlist1 );
		if (!pdev_list_entry)
		{
			status = STATUS_NO_MEMORY;
			__leave;
		}

		memset( pdev_list_entry, 0, sizeof (DEVICE_LIST_ENTRY) );

		if (!pDevName )
		{
			status = STATUS_INVALID_PARAMETER_4;
			__leave;
		}

		
		pdev_list_entry->pDevName = pDevName;
		
		InsertHeadList( 
			pListHead, 
			&(pdev_list_entry->ListEntry) 
			);

		
		status = STATUS_SUCCESS;
	}
	__finally
	{
		if (!NT_SUCCESS( status ) )
		{
			if (pdev_list_entry)
			{
				ExFreePool( pdev_list_entry );
			}
		}
	}

	return status;
}


NTSTATUS
InsertDeviceList( 
	__in PDEVICE_NAME pDevName
)
{
	NTSTATUS status;
	AcquireResourceExclusive( &gDeviceListLock );

	status=InsertDeviceListEx(
		&gDeviceList,
		pDevName
		);

	ReleaseResource( &gDeviceListLock );
	return status;
}

NTSTATUS
RemAllDeviceListEx(
	__in PLIST_ENTRY pDevRulHead 
)
{
	PLIST_ENTRY Flink;
	PDEVICE_LIST_ENTRY pdev_list_entry;

	if ( !pDevRulHead ) return STATUS_INVALID_PARAMETER;

	if ( IsListEmpty( pDevRulHead ) )
		return	STATUS_SUCCESS;


	Flink=pDevRulHead->Flink;
	while ( Flink!=pDevRulHead )
	{
		pdev_list_entry=CONTAINING_RECORD( Flink, DEVICE_LIST_ENTRY, ListEntry );

		Flink=Flink->Flink;
		RemoveEntryList( Flink->Blink );


		if ( pdev_list_entry )
			ExFreePool( pdev_list_entry );
	}

	return STATUS_SUCCESS;
}


NTSTATUS
RemAllDeviceList()
{
	NTSTATUS status;
	AcquireResourceExclusive( &gDeviceListLock );
	status = RemAllDeviceListEx( &gDeviceList );
	ReleaseResource( &gDeviceListLock );
	return status;
}

NTSTATUS
RemItemFromDeviceListEx(
	__in PLIST_ENTRY pDevRulHead, 
	__in PDEVICE_NAME pDevName
)
{
	PLIST_ENTRY Flink;
	PDEVICE_LIST_ENTRY pdev_rul_entry;

	if (!pDevRulHead) return STATUS_INVALID_PARAMETER;

	if ( IsListEmpty( pDevRulHead ) )
		return	STATUS_SUCCESS;


	Flink=pDevRulHead->Flink;
	while ( Flink != pDevRulHead )
	{

		pdev_rul_entry=CONTAINING_RECORD( Flink, DEVICE_LIST_ENTRY, ListEntry );

		if (pdev_rul_entry->pDevName == pDevName)
		{
			Flink = Flink->Flink;
			RemoveEntryList(Flink->Blink);

			if ( pdev_rul_entry )
				ExFreePool (pdev_rul_entry);
		}
		else
			Flink=Flink->Flink;


	}

	return STATUS_SUCCESS;
}

NTSTATUS
RemItemFromDeviceList(
	__in PDEVICE_NAME pDevName
)
{
	NTSTATUS status;
	AcquireResourceExclusive( &gDeviceListLock );
	status = RemItemFromDeviceListEx( &gDeviceList, pDevName );
	ReleaseResource( &gDeviceListLock );
	return status;
}


BOOLEAN
IsAttachedDeviceEx (
	__in PLIST_ENTRY pDevRulHead, 
	__in PUNICODE_STRING pusGuid,
	__in PUNICODE_STRING pusDeviceType
)
{
	PLIST_ENTRY Flink;
	PDEVICE_LIST_ENTRY pdev_rul_entry;

	if (!pDevRulHead) return FALSE;

	if ( IsListEmpty( pDevRulHead ) )
		return	FALSE;


	Flink=pDevRulHead->Flink;
	while ( Flink != pDevRulHead )
	{

		pdev_rul_entry=CONTAINING_RECORD( Flink, DEVICE_LIST_ENTRY, ListEntry );

		if ( RtlCompareUnicodeString( &pdev_rul_entry->pDevName->usGuid,pusGuid, TRUE ) == 0 &&
			 RtlCompareUnicodeString( &pdev_rul_entry->pDevName->usDeviceType,pusDeviceType, TRUE ) == 0
			)
			return TRUE;
		
		 Flink=Flink->Flink;

	}

	return FALSE;
}


BOOLEAN
IsAttachedDevice(
	__in PUNICODE_STRING pusGuid,
	__in PUNICODE_STRING pusDeviceType
)
{
	BOOLEAN ret;
	AcquireResourceExclusive( &gDeviceListLock );
	ret = IsAttachedDeviceEx (
		&gDeviceList, 
		pusGuid,
		pusDeviceType
		);
	ReleaseResource( &gDeviceListLock );
	return ret;
}


//должна запускаться при инициализации драйвера
void GlobalInitDeviceList()
{
	InitializeDeviceList();
	ExInitializeResourceLite( &gDeviceListLock );
}


// запуск при выгрузке драйвера
void GlobalDoneDeviceList()
{
	RemAllDeviceList();
	ExDeleteResourceLite(&gDeviceListLock);
}