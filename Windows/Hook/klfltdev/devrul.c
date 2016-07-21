#include "devrul.h"
#include "struct.h"
#include "ioctl.h"
#include "tags.h"

#include "../mklif/mtypes.h"
#include "../hook/funcs.h"
#include "../hook/avpgcom.h"
#include "../kldefs.h"
#include "../hook/HookSpec.h"
#include "../mklif/struct.h"
#include "debug.h"
#include "../mklif/inc/hips_tools.h"

#include "devrul.tmh"


LIST_ENTRY	gDevRulList;
ERESOURCE	gDevRulListLock;

BOOLEAN 
AcquireResourceExclusive ( __inout PERESOURCE Resource )
{
	BOOLEAN ret;
	KeEnterCriticalRegion();
	ret = ExAcquireResourceExclusiveLite( Resource, TRUE );
	KeLeaveCriticalRegion();
	return ret;
}

VOID ReleaseResource( __inout PERESOURCE Resource )
{
	KeEnterCriticalRegion();
	ExReleaseResourceLite( Resource );
	KeLeaveCriticalRegion();
}

void
InitializeDevRulListEx( 
	__in PLIST_ENTRY pDevRulHead 
	)
{
	InitializeListHead(pDevRulHead);
}

void
InitializeDevRulList()
{
	InitializeDevRulListEx(&gDevRulList);
}


NTSTATUS
InsertDevRulHeadListEx(
	__in PLIST_ENTRY pDevRulHead,
	__in UNICODE_STRING usGuid,
	__in UNICODE_STRING usDeviceType,
	__in ULONG mask
	)
{
	PDEV_RUL_ENTRY pdev_rul_entry;
	ULONG	AppPathLen = 0;
	ULONG	FilePathLen = 0;
	NTSTATUS status = STATUS_UNSUCCESSFUL;

	__try
	{
		pdev_rul_entry = ExAllocatePoolWithTag( PagedPool, sizeof (DEV_RUL_ENTRY), tag_devrul1 );
		if (!pdev_rul_entry)
		{
			status = STATUS_NO_MEMORY;
			__leave;
		}

		memset( pdev_rul_entry, 0, sizeof (DEV_RUL_ENTRY) );

		if (usDeviceType.Length==0 )
		{
			status = STATUS_INVALID_PARAMETER_4;
			__leave;
		}

		if (usGuid.Length==0)
		{
			status = STATUS_INVALID_PARAMETER_3;
			__leave;
		}

		if ( usGuid.Length>0 )
		{
			PWCHAR pwchNativeFilePath = ExAllocatePoolWithTag( PagedPool, usGuid.Length, tag_devrulC );
			if ( !pwchNativeFilePath )
			{
				status = STATUS_NO_MEMORY;
				__leave;
			}

			RtlInitEmptyUnicodeString( &pdev_rul_entry->dev_rul.usGuid, 
				pwchNativeFilePath, 
				usGuid.Length );

			RtlUpcaseUnicodeString( &pdev_rul_entry->dev_rul.usGuid, &usGuid, FALSE );
		}

		if ( usDeviceType.Length>0 )
		{
			PWCHAR pwchNativeFilePath = ExAllocatePoolWithTag( PagedPool, usDeviceType.Length, tag_devrul2 );
			if ( !pwchNativeFilePath )
			{
				status = STATUS_NO_MEMORY;
				__leave;
			}

			RtlInitEmptyUnicodeString( &pdev_rul_entry->dev_rul.usDeviceType, 
				pwchNativeFilePath, 
				usDeviceType.Length );

			RtlUpcaseUnicodeString( &pdev_rul_entry->dev_rul.usDeviceType, &usDeviceType, FALSE );
		}
		
		pdev_rul_entry->dev_rul.mask = mask;
		
		InsertHeadList( 
			pDevRulHead, 
			&(pdev_rul_entry->ListEntry) 
			);

		status = STATUS_SUCCESS;
	}
	__finally
	{
		if (!NT_SUCCESS( status ) )
		{
			if (pdev_rul_entry)
			{
				if ( pdev_rul_entry->dev_rul.usGuid.Buffer )
					ExFreePool ( pdev_rul_entry->dev_rul.usGuid.Buffer );

				if ( pdev_rul_entry->dev_rul.usDeviceType.Buffer )
					ExFreePool ( pdev_rul_entry->dev_rul.usDeviceType.Buffer );

				ExFreePool( pdev_rul_entry );
			}
		}
	}
	
	DoTraceEx ( 
		TRACE_LEVEL_INFORMATION, DC_HIPS,  
		"KLFltDev: Add device rule in driver status = 0x%08X",
		status
		);

	DoTraceEx ( 
		TRACE_LEVEL_INFORMATION, DC_HIPS,  
		"KLFltDev: Guid = %wZ",
		&usGuid
		);

	DoTraceEx ( 
		TRACE_LEVEL_INFORMATION, DC_HIPS,  
		"KLFltDev: DevType = %wZ",
		&usDeviceType
		);

	DoTraceEx ( 
		TRACE_LEVEL_INFORMATION, DC_HIPS,  
		"KLFltDev: AccessMask = %d (isAllow - %s needLog - %s) ",
		mask,
		( HIPS_GET_AM ( mask, HIPS_POS_KLFLTDEV ) == HIPS_FLAG_ALLOW)?"TRUE":"FALSE",
		( HIPS_GET_LOG ( mask, HIPS_POS_KLFLTDEV ) == HIPS_LOG_ON)?"TRUE":"FALSE"
		);

	return status;
}


NTSTATUS
RemoveAlDevRulEx(
	__in PLIST_ENTRY pDevRulHead 
	)
{
	PLIST_ENTRY Flink;
	PDEV_RUL_ENTRY pdev_rul_entry;
	
	if ( !pDevRulHead ) return STATUS_INVALID_PARAMETER;
	
	if ( IsListEmpty( pDevRulHead ) )
		return	STATUS_SUCCESS;


	Flink=pDevRulHead->Flink;
	while ( Flink!=pDevRulHead )
	{
		pdev_rul_entry=CONTAINING_RECORD( Flink, DEV_RUL_ENTRY, ListEntry );

		Flink=Flink->Flink;
		RemoveEntryList( Flink->Blink );
		
		
		if ( pdev_rul_entry->dev_rul.usGuid.Buffer )
			ExFreePool ( pdev_rul_entry->dev_rul.usGuid.Buffer );


		if ( pdev_rul_entry->dev_rul.usDeviceType.Buffer )
			ExFreePool( pdev_rul_entry->dev_rul.usDeviceType.Buffer );

		
		if ( pdev_rul_entry )
			ExFreePool( pdev_rul_entry );
	}

	return STATUS_SUCCESS;
}


NTSTATUS
RemoveAllDevRul()
{
	NTSTATUS status;
	AcquireResourceExclusive( &gDevRulListLock );
	status = RemoveAlDevRulEx( &gDevRulList );
	ReleaseResource( &gDevRulListLock );
	return status;
}

NTSTATUS
GetDevRulSizeEx (
	__in PLIST_ENTRY pDevRulHead,
	__out PULONG psize
)
{
	
	PLIST_ENTRY Flink;
	PDEV_RUL_ENTRY pdev_rul_entry;
	
	if (!psize)
		return STATUS_UNSUCCESSFUL;

	if ( !pDevRulHead ) 
	{
		*psize = 0;
		return STATUS_UNSUCCESSFUL;
	}

	*psize = sizeof(KLFLTDEV_RULES);
	if ( IsListEmpty( pDevRulHead ) )
			return STATUS_SUCCESS;
	
	Flink=pDevRulHead->Flink;
	while ( Flink!=pDevRulHead )
	{
		pdev_rul_entry=CONTAINING_RECORD( Flink, DEV_RUL_ENTRY, ListEntry );
		
		*psize += sizeof (ULONG)+											// место под размер каждого правила (m_ItemSize) 
			sizeof (ULONG)+													// размер маски (m_mask)	
			pdev_rul_entry->dev_rul.usGuid.Length + sizeof (ULONG)+			// размер guid + место дл€ указани€ размера строки (m_Guid + m_GuidSize)
			pdev_rul_entry->dev_rul.usDeviceType.Length + sizeof (ULONG);   // размер DeviceType + место дл€ указани€ размера строки ( m_DevType + m_DevTypeSize)
				
		Flink=Flink->Flink;
	}
	
	return STATUS_SUCCESS;
}


NTSTATUS
GetDevRulSize (
	__out PULONG psize
)
{
	NTSTATUS status;
	AcquireResourceExclusive( &gDevRulListLock );
	status = GetDevRulSizeEx ( &gDevRulList , psize );
	ReleaseResource( &gDevRulListLock );
	return status;
}


NTSTATUS
GetDevRulesEx (
	__in PLIST_ENTRY pDevRulHead,
	__in ULONG max_size,
	__out PKLFLTDEV_RULES prules
	)
{
	NTSTATUS status = STATUS_UNSUCCESSFUL;
	ULONG size = 0;
	ULONG item_size = 0;

	ULONG count = 0;

	PLIST_ENTRY Flink;
	PDEV_RUL_ENTRY pdev_rul_entry;
	PULONG	pItemSize;
	PULONG	pmask;
	PULONG	pStrSize;
	PWCHAR	pStr;

	if ( !prules )
		return STATUS_UNSUCCESSFUL;

	if ( !pDevRulHead ) 
	{
		return STATUS_UNSUCCESSFUL;
	}

	status = GetDevRulSizeEx(pDevRulHead, &size );
	if (!NT_SUCCESS (status))
		return status;
	
	if ( size < max_size )
		return STATUS_BUFFER_TOO_SMALL;

	size = 0;

	memset (prules, 0, max_size);

	size = sizeof(KLFLTDEV_RULES);
	prules->m_ver = DEV_RUL_VERSION;
	prules->m_size = size;
	
	if ( IsListEmpty( pDevRulHead ) )
		return STATUS_SUCCESS;

	Flink=pDevRulHead->Flink;
	
	pItemSize = (PULONG)((char*)prules + sizeof(KLFLTDEV_RULES));
	pmask = (PULONG)(pItemSize + 1);
	pStrSize = (PULONG)(pmask + 1);
	pStr = (PWCHAR)(pStrSize + 1);
	while ( Flink!=pDevRulHead )
	{
		pdev_rul_entry=CONTAINING_RECORD( Flink, DEV_RUL_ENTRY, ListEntry );

		//размер правила
		item_size = sizeof (ULONG)+											// место под размер каждого правила (m_ItemSize) 
			sizeof (ULONG)+													// размер маски (m_mask)	
			pdev_rul_entry->dev_rul.usGuid.Length + sizeof (ULONG)+			// размер guid + место дл€ указани€ размера строки (m_Guid + m_GuidSize)
			pdev_rul_entry->dev_rul.usDeviceType.Length + sizeof (ULONG);   // размер DeviceType + место дл€ указани€ размера строки ( m_DevType + m_DevTypeSize)

		//заполн€ем пол€ данными
		//Guid
		*pItemSize = item_size;
		*pmask = pdev_rul_entry->dev_rul.mask;

		*pStrSize = pdev_rul_entry->dev_rul.usGuid.Length;
		memcpy ( pStr, pdev_rul_entry->dev_rul.usGuid.Buffer, *pStrSize );

		//DeviceType
		pStrSize = (PULONG) ((char*) pStr + *pStrSize);
		pStr = (PWCHAR)(pStrSize + 1);

		*pStrSize = pdev_rul_entry->dev_rul.usDeviceType.Length;
		memcpy ( pStr, pdev_rul_entry->dev_rul.usDeviceType.Buffer, *pStrSize );

		//смещаемс€ на позицию дл€ следующего правила
		pItemSize = (PULONG)((char*)pItemSize + item_size );
		pmask = (PULONG)(pItemSize + 1);
		pStrSize = (PULONG)(pmask + 1);
		pStr = (PWCHAR)(pStrSize + 1);

		count++;
		size += item_size;
		Flink=Flink->Flink;
	}
	
	prules->m_size = size;
	prules->m_RulesCount = count;
	
	return STATUS_SUCCESS;
}

NTSTATUS
GetDevRules (
	__in ULONG max_size,
	__out PKLFLTDEV_RULES prules
)
{
	NTSTATUS status;
	AcquireResourceExclusive( &gDevRulListLock );
	status = GetDevRulesEx (
		&gDevRulList ,
		max_size,
		prules
		);
	ReleaseResource( &gDevRulListLock );
	return status;
}


//////////////////////////////////////////////////////////////////////////

PFILTER_EVENT_PARAM
DevRul_EvContext (
	__in ULONG evType,
	__in UNICODE_STRING usGuid,
	__in UNICODE_STRING usDeviceType,
	__in ULONG AccessMask,
	__out PULONG pParamsSize
)
{
	PFILTER_EVENT_PARAM pParam = NULL;

	ULONG ParamsCount = 0;
	ULONG params_size = sizeof(FILTER_EVENT_PARAM);

	ParamsCount++;											//1: usGuid + \0
	params_size += sizeof(SINGLE_PARAM) + usGuid.Length + sizeof(WCHAR);


	ParamsCount++;											//2: usDeviceType + \0
	params_size += sizeof(SINGLE_PARAM) + usDeviceType.Length + sizeof(WCHAR);	

	
	ParamsCount++;											//3: AccessMask
	params_size += sizeof(SINGLE_PARAM) + sizeof(ULONG);

	pParam = ExAllocatePoolWithTag( PagedPool, params_size, tag_devrulE );
	if (pParam)
	{
		PSINGLE_PARAM pSingleParam;
		FILTER_PARAM_COMMONINIT( pParam, FLTTYPE_SYSTEM, MJ_SYSTEM_KLFLTDEV, evType, PreProcessing, ParamsCount );

		pSingleParam = (PSINGLE_PARAM) pParam->Params;
		
		if (usGuid.Length)
		{
													//1: usGuid + \0
			SINGLE_PARAM_INIT_NONE( pSingleParam, _PARAM_OBJECT_URL_W );

			memcpy( pSingleParam->ParamValue, usGuid.Buffer, usGuid.Length );
			((PCHAR)pSingleParam->ParamValue)[usGuid.Length] = 0;
			((PCHAR)pSingleParam->ParamValue)[usGuid.Length + 1] = 0;
			pSingleParam->ParamSize = usGuid.Length + sizeof(WCHAR);
		}

		if ( usDeviceType.Length )
		{
			SINGLE_PARAM_SHIFT( pSingleParam );		//2: usDeviceType + \0
			SINGLE_PARAM_INIT_NONE( pSingleParam, _PARAM_OBJECT_VOLUME_NAME_W );

			memcpy( pSingleParam->ParamValue, usDeviceType.Buffer, usDeviceType.Length );
			((PCHAR)pSingleParam->ParamValue)[usDeviceType.Length] = 0;
			((PCHAR)pSingleParam->ParamValue)[usDeviceType.Length + 1] = 0;
			pSingleParam->ParamSize = usDeviceType.Length + sizeof(WCHAR);
		}

		SINGLE_PARAM_SHIFT( pSingleParam );			//3: AccessMask
		SINGLE_PARAM_INIT_ULONG( pSingleParam, _PARAM_OBJECT_ACCESSATTR, AccessMask );
	}

	if ( pParamsSize )
		*pParamsSize = params_size;

	return pParam;
}

NTSTATUS 
SendDevIoCtl2KLFltDevByName(
							__in ULONG IoControlCode, 
							__in PVOID InputBuffer,
							__in ULONG InputBufferLength,
							__out PVOID OutputBuffer,
							__in ULONG OutputBufferLength, 
							__in PCWSTR pDrvName, 
							__out ULONG* pRetSize
							)
{
	HANDLE						hExtDrv;
	OBJECT_ATTRIBUTES	ObjAttr;
	UNICODE_STRING		us;
	IO_STATUS_BLOCK		ioStatus;
	PDEVICE_OBJECT		DevObj;
	PFILE_OBJECT			fileObject;
	NTSTATUS					ntStatus;
	KEVENT						Event;
	PIRP							Irp;
	PIO_STACK_LOCATION irpSp;

	RtlInitUnicodeString(&us, pDrvName);
	InitializeObjectAttributes(&ObjAttr,&us,OBJ_CASE_INSENSITIVE,NULL,NULL);
	ntStatus=ZwCreateFile(&hExtDrv,SYNCHRONIZE|FILE_ANY_ACCESS,&ObjAttr,&ioStatus,NULL,0,FILE_SHARE_READ|FILE_SHARE_WRITE,FILE_OPEN,FILE_SYNCHRONOUS_IO_NONALERT,NULL,0);
	if(NT_SUCCESS(ntStatus)) {
		ntStatus=ObReferenceObjectByHandle(hExtDrv,FILE_READ_DATA,NULL,KernelMode, (PVOID*) &fileObject,NULL);
		if(NT_SUCCESS(ntStatus)) {
			if((DevObj=IoGetRelatedDeviceObject(fileObject))!=NULL) {
				KeInitializeEvent(&Event,NotificationEvent,FALSE);
				Irp=IoBuildDeviceIoControlRequest(IoControlCode,DevObj,InputBuffer,InputBufferLength,OutputBuffer,OutputBufferLength,FALSE,&Event,&ioStatus);
				if(Irp!=NULL) {
					irpSp=IoGetNextIrpStackLocation(Irp);
					irpSp->FileObject = fileObject;
					ntStatus=IoCallDriver(DevObj,Irp);
					if(ntStatus==STATUS_PENDING) {
						KeWaitForSingleObject(&Event,Executive,KernelMode,FALSE,(PLARGE_INTEGER)NULL);
						ntStatus = ioStatus.Status;
					}
					if (pRetSize != NULL)
						*pRetSize = (ULONG)ioStatus.Information;
				} else {
					ntStatus=STATUS_UNSUCCESSFUL;
				}
			} else {
				ntStatus=STATUS_UNSUCCESSFUL;
			}
			ObDereferenceObject(fileObject);
		} 
		ZwClose(hExtDrv);
	}

	return ntStatus;
}

VERDICT
DevRulFilterEvent( PFILTER_EVENT_PARAM pEvent, ULONG size )
{
	VERDICT Verdict = Verdict_Default;
	NTSTATUS status = STATUS_SUCCESS;
//	UNICODE_STRING nameString;
	PWCHAR pDrvName = NULL;
	ULONG DrvNameSize = 0;
	ULONG retSize = 0;

	__try
	{
		DrvNameSize = sizeof (CONTROL_DEVICE_OBJECT_PREFIX) + 
			sizeof (CONTROL_DEVICE_OBJECT_NAME) + sizeof(WCHAR);

		pDrvName = ExAllocatePoolWithTag( PagedPool, DrvNameSize, tag_devrulF );
		if (!pDrvName)
			__leave;

 		RtlZeroMemory ( pDrvName, DrvNameSize );
// 		RtlInitEmptyUnicodeString(&nameString, pstr, (USHORT)strSize);
// 		status = RtlAppendUnicodeToString ( &nameString, CONTROL_DEVICE_OBJECT_PREFIX );
// 		if (NT_SUCCESS(status))
// 			status = RtlAppendUnicodeToString ( &nameString, CONTROL_DEVICE_OBJECT_NAME );
		
		memcpy ( 
			pDrvName, 
			CONTROL_DEVICE_OBJECT_PREFIX, 
			sizeof( CONTROL_DEVICE_OBJECT_PREFIX )- sizeof(WCHAR)  );
		
		memcpy ( 
			(char*)pDrvName + sizeof( CONTROL_DEVICE_OBJECT_PREFIX ) - sizeof(WCHAR), 
			CONTROL_DEVICE_OBJECT_NAME, 
			sizeof( CONTROL_DEVICE_OBJECT_NAME )  );
		
		SendDevIoCtl2KLFltDevByName(
			IOCTLHOOK_FilterEvent, 
			pEvent,
			size,
			&Verdict,
			sizeof(VERDICT), 
			pDrvName, 
			&retSize
			);
				
	}
	__finally
	{
		if (pDrvName)
			ExFreePool( pDrvName );
	}


	return Verdict;
}

BOOLEAN 
IsAllowAccessEx(
	__in PLIST_ENTRY pDevRulHead,
	__in UNICODE_STRING	usGuid,
	__in UNICODE_STRING	deviceType
)
{
	BOOLEAN isAllow = TRUE;
	BOOLEAN needLog = FALSE;
	ULONG	mask = 0;
	PLIST_ENTRY Flink;
	PDEV_RUL_ENTRY pdev_rul_entry;
	UNICODE_STRING usAllDevType;
	PFILTER_EVENT_PARAM pEvent = NULL;
	
	if ( !pDevRulHead ) return isAllow;

	if ( IsListEmpty( pDevRulHead ) )
		return	isAllow;

	RtlInitUnicodeString( &usAllDevType, DEV_TYPE_OTHER );

	Flink=pDevRulHead->Flink;
	while ( Flink!=pDevRulHead )
	{
		pdev_rul_entry=CONTAINING_RECORD( Flink, DEV_RUL_ENTRY, ListEntry );
		
		if ( RtlCompareUnicodeString( &pdev_rul_entry->dev_rul.usGuid, &usGuid, TRUE ) == 0 )
		{
			if ( RtlCompareUnicodeString( &pdev_rul_entry->dev_rul.usDeviceType, &deviceType, TRUE ) == 0 )
			{
				mask = pdev_rul_entry->dev_rul.mask;
				//isAllow = (BOOLEAN) ( ( (pdev_rul_entry->dev_rul.mask) >> ACCESS_OFFSET )&0x3 ) ;
				//needLog = (BOOLEAN) ( ( (pdev_rul_entry->dev_rul.mask) >> LOG_OFFSET )&0x3 ); 
				isAllow = ( HIPS_GET_AM ( mask, HIPS_POS_KLFLTDEV ) == HIPS_FLAG_ALLOW ) ? TRUE : FALSE;
				needLog = ( HIPS_GET_LOG ( mask, HIPS_POS_KLFLTDEV ) == HIPS_LOG_ON ) ? TRUE : FALSE;
								
				DoTraceEx ( 
					TRACE_LEVEL_INFORMATION, DC_HIPS,  
					"!!!KLFltDev: IsAllowAccessEx finded rule!!!" 
					);

				DoTraceEx ( 
					TRACE_LEVEL_INFORMATION, DC_HIPS,  
					"!!!KLFltDev: Guid = %wZ!!!",
					&pdev_rul_entry->dev_rul.usGuid
					);

				DoTraceEx ( 
					TRACE_LEVEL_INFORMATION, DC_HIPS,  
					"!!!KLFltDev: DevType = %wZ!!!",
					&pdev_rul_entry->dev_rul.usDeviceType
					);

				DoTraceEx ( 
					TRACE_LEVEL_INFORMATION, DC_HIPS,  
					"!!!KLFltDev: AccessMask = %d (isAllow - %s needLog - %s)!!!",
					mask,
					( isAllow == TRUE )? "TRUE" : "FALSE",
					( needLog == TRUE )? "TRUE" : "FALSE"
					);

				//если нужно записать в лог, то пишем
				if ( needLog )
				{ //
					ULONG eventSize;
					pEvent = DevRul_EvContext (
											0,
											usGuid,
											deviceType,
											mask,
											&eventSize
											);

					if ( pEvent )
					{
						DevRulFilterEvent( pEvent, eventSize );
						ExFreePool( pEvent );
					}
				}

				break;
			}
		}
		
		Flink=Flink->Flink;
	}
	
	return isAllow;
}


BOOLEAN 
IsAllowAccess(
	__in UNICODE_STRING	usGuid,
	__in UNICODE_STRING	deviceType
)
{
	BOOLEAN isAllow = TRUE;
	AcquireResourceExclusive( &gDevRulListLock );
	
	isAllow = IsAllowAccessEx(
		&gDevRulList,
		usGuid,
		deviceType
		);
	
	ReleaseResource( &gDevRulListLock );

	return isAllow;
}

//должна запускатьс€ при инициализации драйвера
void InitDevRul()
{
	InitializeDevRulList();
	ExInitializeResourceLite( &gDevRulListLock );
}


// запуск при выгрузке драйвера
void DoneDevRul()
{
	RemoveAllDevRul();
	
	ExDeleteResourceLite(&gDevRulListLock);
}


//Registry

NTSTATUS
SaveDevRuleToReg(
    __in PUNICODE_STRING RegistryPath,
	__in UNICODE_STRING usGuid,
	__in UNICODE_STRING usDeviceType,
	__in ULONG mask
)
{
	NTSTATUS status = STATUS_UNSUCCESSFUL;

	OBJECT_ATTRIBUTES	attributes;
	HANDLE				driverRegKey;
	UNICODE_STRING		usStr;
	PWCHAR				wcStr = NULL;
	ULONG				wsStrSize = 0;
	
	__try
	{
		wsStrSize = (ULONG) (RegistryPath->Length + sizeof(WCHAR)+ 
			wcslen(RUL_KEY_NAME)*sizeof(WCHAR) + sizeof(WCHAR) + 
			usGuid.Length + sizeof(WCHAR));
		wcStr = ExAllocatePoolWithTag( PagedPool, wsStrSize, tag_devrulD );
		if (!wcStr)
		{
			status = STATUS_NO_MEMORY; 
			__leave;
		}

		RtlInitEmptyUnicodeString(&usStr,wcStr, (USHORT)wsStrSize);

		RtlCopyUnicodeString(&usStr, RegistryPath );
		
		status = RtlAppendUnicodeToString (&usStr, L"\\");
		if ( !NT_SUCCESS( status ) )
			__leave;

		status = RtlAppendUnicodeToString (&usStr, RUL_KEY_NAME);
		if ( !NT_SUCCESS( status ) )
			__leave;

		status = RtlAppendUnicodeToString (&usStr, L"\\");
		if ( !NT_SUCCESS( status ) )
			__leave;

		status = RtlAppendUnicodeStringToString( &usStr, &usGuid );
		if ( !NT_SUCCESS( status ) )
			__leave;

		InitializeObjectAttributes( &attributes,
			&usStr,
			OBJ_CASE_INSENSITIVE,
			NULL,
			NULL );

		status = ZwCreateKey(
			&driverRegKey,
			KEY_CREATE_SUB_KEY | KEY_SET_VALUE,
			&attributes,
			0,
			NULL,
			REG_OPTION_NON_VOLATILE,
			NULL
			);
		
		if ( !NT_SUCCESS( status ) )
			__leave;
		
		status = ZwSetValueKey(
			driverRegKey,
			&usDeviceType,
			0,
			REG_DWORD,
			&mask,
			sizeof (ULONG)
			);

		if ( !NT_SUCCESS( status ) )
			__leave;
		
		
		
		status = STATUS_SUCCESS;
	}
	__finally
	{
		if ( wcStr )
			ExFreePool( wcStr );

		
		if ( driverRegKey )
			ZwClose( driverRegKey );
	}

	DoTraceEx ( 
		TRACE_LEVEL_INFORMATION, DC_HIPS,  
		"KLFltDev: Add device rule in Registry status = 0x%08X",
		status
		);

	DoTraceEx ( 
		TRACE_LEVEL_INFORMATION, DC_HIPS,  
		"KLFltDev: Guid = %wZ",
		&usGuid
		);

	DoTraceEx ( 
		TRACE_LEVEL_INFORMATION, DC_HIPS,  
		"KLFltDev: DevType = %wZ",
		&usDeviceType
		);

	DoTraceEx ( 
		TRACE_LEVEL_INFORMATION, DC_HIPS,  
		"KLFltDev: AccessMask = %d (isAllow - %s needLog - %s) ",
		mask,
		( HIPS_GET_AM ( mask, HIPS_POS_KLFLTDEV ) == HIPS_FLAG_ALLOW)?"TRUE":"FALSE",
		( HIPS_GET_LOG ( mask, HIPS_POS_KLFLTDEV ) == HIPS_LOG_ON)?"TRUE":"FALSE"
		);

	return status;
}


/* 
«аносит правило в список из реестра
*/
NTSTATUS
InsertDevRuleByGuidFromReg( 
	__in PLIST_ENTRY pDevRulHead,
	__in PUNICODE_STRING RegistryPath, 
	__in PUNICODE_STRING pusGuid 
)
{
	NTSTATUS status = STATUS_UNSUCCESSFUL;

	OBJECT_ATTRIBUTES	attributes;
	HANDLE				driverRegKey;
	UNICODE_STRING		usStr;
	PWCHAR				wcStr = NULL;
	ULONG				wsStrSize = 0;
	char				*inf_buf = NULL;

	__try
	{
		wsStrSize = RegistryPath->Length + sizeof(WCHAR)+ pusGuid->Length + sizeof(WCHAR);
		wcStr = ExAllocatePoolWithTag( PagedPool, wsStrSize, tag_devrulB );
		if (!wcStr)
		{
			status = STATUS_NO_MEMORY; 
			__leave;
		}
		
		RtlInitEmptyUnicodeString(&usStr,wcStr, (USHORT)wsStrSize);

		RtlCopyUnicodeString(&usStr, RegistryPath );
		
		status = RtlAppendUnicodeToString (&usStr, L"\\");
		if ( !NT_SUCCESS( status ) )
			__leave;

		status = RtlAppendUnicodeStringToString(&usStr, pusGuid );
		if ( !NT_SUCCESS( status ) )
			__leave;

		InitializeObjectAttributes( &attributes,
			&usStr,
			OBJ_CASE_INSENSITIVE,
			NULL,
			NULL );

		status = ZwOpenKey( &driverRegKey,
			KEY_ALL_ACCESS,
			&attributes );

		if ( !NT_SUCCESS( status ) )
			__leave;

		ExFreePool(wcStr);
		wcStr = NULL;

		{
			ULONG Index = 0;
			ULONG inf_buf_size = 0;
			ULONG retSize;
			
			inf_buf_size = sizeof ( KEY_VALUE_FULL_INFORMATION ) + 
				REGVAR_NAME_MAX_SIZE*sizeof(WCHAR) + sizeof( ULONG );
			inf_buf = ExAllocatePoolWithTag(PagedPool, inf_buf_size, tag_devrul9 );
			if ( !inf_buf )
			{
				status = STATUS_NO_MEMORY; 
				__leave;
			}

			while (1)
			{

				memset(inf_buf, 0, inf_buf_size);
							
				status = ZwEnumerateValueKey(
					driverRegKey,
					Index,
					KeyValueFullInformation,
					inf_buf,
					inf_buf_size,
					&retSize
					);

				if ( status == STATUS_NO_MORE_ENTRIES )
					break;
				else if ( !NT_SUCCESS( status ) )
					__leave;
				
				
				if ( ((PKEY_VALUE_FULL_INFORMATION)inf_buf)->DataLength != sizeof(ULONG) )
				{
					status = STATUS_UNSUCCESSFUL;
					__leave;
				}

				{
					UNICODE_STRING usDevType;
					PULONG pmask;

					pmask = (PULONG)( (char*)inf_buf + ((PKEY_VALUE_FULL_INFORMATION)inf_buf)->DataOffset );
					
					RtlInitEmptyUnicodeString(
						&usDevType, 
						((PKEY_VALUE_FULL_INFORMATION)inf_buf)->Name, 
						(USHORT) (((PKEY_VALUE_FULL_INFORMATION)inf_buf)->NameLength) 
						);

					usDevType.Length = (USHORT) (((PKEY_VALUE_FULL_INFORMATION)inf_buf)->NameLength);
					
					InsertDevRulHeadListEx( pDevRulHead, *pusGuid, usDevType, *pmask );
				}
				
				Index++;
			}
		}

		status = STATUS_SUCCESS;
	}
	__finally
	{
		if ( wcStr )
			ExFreePool( wcStr );

		if (inf_buf)
			ExFreePool( inf_buf );

		if ( driverRegKey )
			ZwClose( driverRegKey );
	}

	return status;

}

/* 
«аносит правилј в список из реестра
RegistryPath - ѕ”“№ к драйверу (получаем в DriverEntry) \REGISTRY\MACHINE\SYSTEM\ControlSet001\Services\klfltdev
*/

NTSTATUS
RebuildDevRulListFromReg( __in PUNICODE_STRING RegistryPath )
{
	NTSTATUS status = STATUS_UNSUCCESSFUL;
	
	OBJECT_ATTRIBUTES	attributes;
	HANDLE				driverRegKey;
	UNICODE_STRING		usStr;
	PWCHAR				wcStr = NULL;
	ULONG				wsStrSize = 0;
	char				*inf_buf = NULL;
	
	AcquireResourceExclusive( &gDevRulListLock );

	__try
	{
		status = RemoveAlDevRulEx( &gDevRulList );
		if ( !NT_SUCCESS( status ) )
			__leave;

		wsStrSize = (ULONG)(RegistryPath->Length + sizeof(WCHAR)+ wcslen(RUL_KEY_NAME)*sizeof(WCHAR)+ sizeof(WCHAR));
		wcStr = ExAllocatePoolWithTag( PagedPool, wsStrSize, tag_devrul8 );
		if (!wcStr)
		{
			status = STATUS_NO_MEMORY; 
			__leave;
		}
		
		RtlInitEmptyUnicodeString(&usStr,wcStr, (USHORT)wsStrSize);
		
		RtlCopyUnicodeString(&usStr, RegistryPath );
		status = RtlAppendUnicodeToString (&usStr, L"\\");
		
		if ( !NT_SUCCESS( status ) )
			__leave;

		status = RtlAppendUnicodeToString (&usStr, RUL_KEY_NAME);
		if ( !NT_SUCCESS( status ) )
			__leave;


		InitializeObjectAttributes( &attributes,
			&usStr,
			OBJ_CASE_INSENSITIVE,
			NULL,
			NULL );

		status = ZwOpenKey( &driverRegKey,
			KEY_ALL_ACCESS,
			&attributes );

		if ( !NT_SUCCESS( status ) )
			__leave;

		{
			ULONG Index = 0;
			ULONG inf_buf_size = 0;
			ULONG retSize;
			UNICODE_STRING usGuidName;
			
			inf_buf_size = sizeof (KEY_BASIC_INFORMATION) + GUID_SIZE*sizeof(WCHAR);
			inf_buf = ExAllocatePoolWithTag(PagedPool, inf_buf_size, tag_devrul9 );
			if ( !inf_buf )
			{
				status = STATUS_NO_MEMORY; 
				__leave;
			}
			
			while (1)
			{
				
				memset(inf_buf, 0, inf_buf_size);
				status = ZwEnumerateKey(
					driverRegKey,
					Index,
					KeyBasicInformation,
					inf_buf,
					inf_buf_size,
					&retSize
					);
			
				if ( status == STATUS_NO_MORE_ENTRIES )
					break;
				else if ( !NT_SUCCESS( status ) )
					__leave;
								
				RtlInitEmptyUnicodeString( &usGuidName, ((PKEY_BASIC_INFORMATION)inf_buf)->Name, (USHORT)(((PKEY_BASIC_INFORMATION)inf_buf)->NameLength) );
				usGuidName.Length = (USHORT)(((PKEY_BASIC_INFORMATION)inf_buf)->NameLength);
				
				InsertDevRuleByGuidFromReg( &gDevRulList, &usStr , &usGuidName );
												
				Index++;
			}
		}

		status = STATUS_SUCCESS;
	}
	__finally
	{
		if ( wcStr )
			ExFreePool( wcStr );

		if (inf_buf)
			ExFreePool( inf_buf );

		
		if ( driverRegKey )
			ZwClose( driverRegKey );
	}
	
	ReleaseResource( &gDevRulListLock );
	return status;
}