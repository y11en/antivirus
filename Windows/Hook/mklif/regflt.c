#include "pch.h"
#include <wdm.h>
#include "../hook/Funcs.h"

#include "regflt.tmh"

LONG g_RegEnters = 0;

typedef struct _tREGKEY_CONTEXT
{
	ACCESS_MASK  m_DesiredAccess;
}REGKEY_CONTEXT, *PREGKEY_CONTEXT;

#if (NTDDI_VERSION < NTDDI_LONGHORN)

#pragma message ("----------------------- REG_LOAD_KEY_INFORMATION not defined - using handmade struct")
typedef struct _REG_LOAD_KEY_INFORMATION {
	PVOID               Object;
	PUNICODE_STRING     KeyName;
	PUNICODE_STRING     SourceFile;
}REG_LOAD_KEY_INFORMATION, *PREG_LOAD_KEY_INFORMATION;

#pragma message ("----------------------- REG_CALLBACK_CONTEXT_CLEANUP_INFORMATION not defined - using handmade struct")
typedef struct _REG_CALLBACK_CONTEXT_CLEANUP_INFORMATION {
	PVOID  Object;
	PVOID  ObjectContext;  
	PVOID  Reserved;     
} REG_CALLBACK_CONTEXT_CLEANUP_INFORMATION, *PREG_CALLBACK_CONTEXT_CLEANUP_INFORMATION;

#endif // reg structs for vista or higher

ULONG gRegHookMajorVersion = 0;

typedef struct _REG_VALUE
{
	ULONG	m_Type;
	ULONG	m_Size;
	PVOID	m_pData;
} REG_VALUE, *PREG_VALUE;

typedef struct _KEY_INFO
{
	PUNICODE_STRING	p_cntdRelKeyName;
	PVOID			pRootObject;
	PVOID			pKeyObject;
	ACCESS_MASK		m_DesiredAccess;
} KEY_INFO, *PKEY_INFO;

BOOLEAN			gbRegFltInited = FALSE;
LARGE_INTEGER	gRegCallback_Cookie = {0, 0};

#define INIT_KEYINFO( ki, keyname, rootobject, keyobject )	\
{	\
	ki.p_cntdRelKeyName = keyname;	\
	ki.pRootObject = rootobject;	\
	ki.pKeyObject = keyobject;	\
	ki.m_DesiredAccess = 0;\
}

extern BOOLEAN g_bTransactionOperationsSupported;
extern t_fCmGetBoundTransaction g_pfCmGetBoundTransaction;

//+ context functions
RTL_GENERIC_TABLE	gTableRegSetValue;
FAST_MUTEX			gfmTableRegSetValue;

typedef struct _tThreadSetInfo
{
	HANDLE m_hProcess;
	HANDLE m_hThread;
	PREG_SET_VALUE_KEY_INFORMATION m_pSetInfo;
}ThreadSetInfo, *PThreadSetInfo;

RTL_GENERIC_COMPARE_RESULTS
NTAPI
TSetValue_Compare (
	PRTL_GENERIC_TABLE pTable,
	PVOID  FirstStruct,
	PVOID  SecondStruct
	)
{
	PThreadSetInfo pInfo1 = (PThreadSetInfo) FirstStruct;
	PThreadSetInfo pInfo2 = (PThreadSetInfo) SecondStruct;

	LONG compare = 0;

	UNREFERENCED_PARAMETER( pTable );
	
	if (pInfo1->m_hThread > pInfo2->m_hThread)
		return GenericLessThan;
	else if (pInfo1->m_hThread < pInfo2->m_hThread)
		return GenericGreaterThan;

	return GenericEqual;
}

VOID
TSetValue_DeleteElement (
	PThreadSetInfo pTInfo
	)
{
	PVOID freeptr = NULL;

	freeptr = pTInfo->m_pSetInfo;

	RtlDeleteElementGenericTable( &gTableRegSetValue, pTInfo );
	if (freeptr)
		ExFreePool( freeptr );
}

VOID
Reg_ThreadSetPreInfo (
	PREG_SET_VALUE_KEY_INFORMATION pInfo
	)
{
	ThreadSetInfo TInfo;
	PThreadSetInfo pTInfo;
	
	BOOLEAN NewElement = FALSE;

	if(gRegHookMajorVersion)
		return; // vista - use pre struct in post!

	if (!pInfo->ValueName)
		return;

	if (!pInfo->ValueName->Length)
		return;

	TInfo.m_hProcess = PsGetCurrentProcessId();
	TInfo.m_hThread = PsGetCurrentThreadId();
	TInfo.m_pSetInfo = NULL;

	ExAcquireFastMutex( &gfmTableRegSetValue );

	pTInfo = RtlLookupElementGenericTable( &gTableRegSetValue, &TInfo );
	if (pTInfo)
		TSetValue_DeleteElement( pTInfo );

	pTInfo = RtlInsertElementGenericTable( &gTableRegSetValue, &TInfo, sizeof(ThreadSetInfo), &NewElement );

	if (!pTInfo || !NewElement)
	{
	}
	else
	{
		PBYTE ptr;
		ULONG ItemSize = 0;
		ULONG DataSize = pInfo->DataSize;

		if (DataSize > 0x10000)
		{
			//_dbg_break_;
			DoTraceEx( TRACE_LEVEL_WARNING, DC_DRIVER, "huge registry value, size 0x%x", DataSize );
			DataSize = 0;
		}

		ItemSize = sizeof(REG_SET_VALUE_KEY_INFORMATION) + pInfo->ValueName->Length + sizeof(UNICODE_STRING) + DataSize;

		pTInfo->m_pSetInfo = ExAllocatePoolWithTag( PagedPool, ItemSize, tag_table_reg_ptr );
		if (pTInfo->m_pSetInfo)
		{
			RtlCopyMemory( pTInfo->m_pSetInfo, pInfo, sizeof(REG_SET_VALUE_KEY_INFORMATION) );
			
			ptr = (PBYTE)pTInfo->m_pSetInfo;
			ptr += sizeof(REG_SET_VALUE_KEY_INFORMATION);
			pTInfo->m_pSetInfo->ValueName = (PUNICODE_STRING) ptr;
			
			pTInfo->m_pSetInfo->ValueName->Length = pInfo->ValueName->Length;
			pTInfo->m_pSetInfo->ValueName->MaximumLength = pTInfo->m_pSetInfo->ValueName->Length;

			ptr += sizeof(UNICODE_STRING);
			pTInfo->m_pSetInfo->ValueName->Buffer = (PWSTR) ptr;
			RtlCopyMemory (
				pTInfo->m_pSetInfo->ValueName->Buffer,
				pInfo->ValueName->Buffer,
				pTInfo->m_pSetInfo->ValueName->Length
				);

			ptr += pTInfo->m_pSetInfo->ValueName->Length;
			pTInfo->m_pSetInfo->Data = ptr;
			
			if (DataSize)
			{
				try {
					 ProbeForRead( pInfo->Data, DataSize, 1 );
					 RtlCopyMemory( pTInfo->m_pSetInfo->Data, pInfo->Data, DataSize );

				} __except(CheckException()) {
					DataSize = 0;
				}
			}

			if (!DataSize)
				pTInfo->m_pSetInfo->Data = NULL;
		}
	}

	ExReleaseFastMutex( &gfmTableRegSetValue );
}

PREG_SET_VALUE_KEY_INFORMATION
Reg_ThreadGetPreInfo (
	PREG_POST_OPERATION_INFORMATION pInfo
	)
{
	ThreadSetInfo TInfo;
	PThreadSetInfo pTInfo = NULL;

	if(gRegHookMajorVersion)
		return pInfo->PreInformation;

	TInfo.m_hProcess = PsGetCurrentProcessId();
	TInfo.m_hThread = PsGetCurrentThreadId();

	ExAcquireFastMutex( &gfmTableRegSetValue );

	pTInfo = RtlLookupElementGenericTable( &gTableRegSetValue, &TInfo );

	ExReleaseFastMutex( &gfmTableRegSetValue );

	if (pTInfo)
		return pTInfo->m_pSetInfo;

	return NULL;
}

VOID
Reg_ThreadReleasePreInfo (
	)
{
	ThreadSetInfo TInfo;
	PThreadSetInfo pTInfo = NULL;

	if(gRegHookMajorVersion)
		return;

	TInfo.m_hProcess = PsGetCurrentProcessId();
	TInfo.m_hThread = PsGetCurrentThreadId();

	ExAcquireFastMutex( &gfmTableRegSetValue );

	pTInfo = RtlLookupElementGenericTable( &gTableRegSetValue, &TInfo );
	if (pTInfo)
		TSetValue_DeleteElement( pTInfo );
	
	ExReleaseFastMutex( &gfmTableRegSetValue );
}

//- context functions

BOOLEAN
IsSkipRegKey (
	PUNICODE_STRING pusKeyName
	)
{
	UNREFERENCED_PARAMETER( pusKeyName );
#define _REGKEY_HARDWARE_DEVICEMAP		L"\\REGISTRY\\MACHINE\\HARDWARE\\DEVICE"
#define _REGKEY_HARDWARE_DEVICEMAP_LEN (sizeof(_REGKEY_HARDWARE_DEVICEMAP) - sizeof(WCHAR))

#define _REGKEY_RNG		L"\\REGISTRY\\MACHINE\\SOFTWARE\\Microsoft\\Cryptography\\RNG"
#define _REGKEY_RNG_LEN (sizeof(_REGKEY_RNG) - sizeof(WCHAR))

	return FALSE;
}

NTSTATUS
QueryObjectName (
	__in PVOID pObject,
	__out PUNICODE_STRING pusObjectName
	)
{
	NTSTATUS					status;
	POBJECT_NAME_INFORMATION	pObjectInfo;
	ULONG						RetLen;
	PWCHAR						KeyName = NULL;

	if (!pObject)
		return STATUS_UNSUCCESSFUL;

	pObjectInfo = ExAllocatePoolWithTag( PagedPool, sizeof(WCHAR) * MAX_REGPATH_LEN, tag_regkname );
	if(!pObjectInfo)
		return STATUS_UNSUCCESSFUL;
	try {
		status = ObQueryNameString( pObject, pObjectInfo, sizeof(WCHAR) * MAX_REGPATH_LEN, &RetLen );
	} __except(CheckException()) {
		ExFreePool( pObjectInfo );
		return STATUS_UNSUCCESSFUL;
	}
	
	if(NT_SUCCESS( status ))
	{
		if (!pObjectInfo->Name.Length)
			status = STATUS_UNSUCCESSFUL;
		else
		{
			pusObjectName->Buffer = ExAllocatePoolWithTag( PagedPool, pObjectInfo->Name.Length, tag_regkname );
			if (!pusObjectName->Buffer)
				status = STATUS_UNSUCCESSFUL;
			else
			{
				pusObjectName->Length =  pObjectInfo->Name.Length;
				pusObjectName->MaximumLength =  pObjectInfo->Name.Length;
				memcpy( pusObjectName->Buffer, pObjectInfo->Name.Buffer, pObjectInfo->Name.Length );
			}
		}
	}

	ExFreePool( pObjectInfo );

	return status;
}

NTSTATUS
Reg_InitKeyName (
	PUNICODE_STRING pusKeyName,
	PVOID pRootObject,
	PVOID pKeyObject,
	PUNICODE_STRING p_cntdRelKeyName
	)
{
	NTSTATUS status;
	UNICODE_STRING usRootName;
	UNICODE_STRING usSubName;
	UNICODE_STRING usSubNameAlloc;
	
	RtlInitEmptyUnicodeString( pusKeyName, NULL, 0 );
	RtlInitEmptyUnicodeString( &usRootName, NULL, 0 );
	RtlInitEmptyUnicodeString( &usSubName, NULL, 0 );
	RtlInitEmptyUnicodeString( &usSubNameAlloc, NULL, 0 );

	if (p_cntdRelKeyName)
		usSubName = *p_cntdRelKeyName;
	else
	{
		status = QueryObjectName( pKeyObject, &usSubNameAlloc );
		if (NT_SUCCESS( status ))
			usSubName = usSubNameAlloc;
	}
	
	QueryObjectName( pRootObject, &usRootName );

	status = STATUS_UNSUCCESSFUL;
	if (usRootName.Length && usSubName.Length)
	{
		pusKeyName->Length = usRootName.Length + usSubName.Length + sizeof(WCHAR);
		pusKeyName->Buffer = ExAllocatePoolWithTag( PagedPool, pusKeyName->Length, tag_regkname );
		if (pusKeyName->Buffer)
		{
			memcpy( pusKeyName->Buffer, usRootName.Buffer, usRootName.Length );
			memcpy( (__int8*)pusKeyName->Buffer + usRootName.Length, L"\\", sizeof(L'\\') );
			memcpy( (__int8*)pusKeyName->Buffer + usRootName.Length + sizeof(L'\\'), usSubName.Buffer, usSubName.Length );
			
			status = STATUS_SUCCESS;
		}
	}
	else if (usRootName.Length)
	{
		pusKeyName->Length = usRootName.Length;
		pusKeyName->Buffer = ExAllocatePoolWithTag( PagedPool, pusKeyName->Length, tag_regkname );
		if (pusKeyName->Buffer)
		{
			memcpy( pusKeyName->Buffer, usRootName.Buffer, usRootName.Length );
			status = STATUS_SUCCESS;
		}
	}
	else if (usSubName.Length)
	{
		pusKeyName->Length = usSubName.Length;
		pusKeyName->Buffer = ExAllocatePoolWithTag( PagedPool, pusKeyName->Length, tag_regkname );
		if (pusKeyName->Buffer)
		{
			memcpy( pusKeyName->Buffer, usSubName.Buffer, usSubName.Length );
			status = STATUS_SUCCESS;
		}
	}

	if (usRootName.Buffer)
		ExFreePool( usRootName.Buffer );

	if (usSubNameAlloc.Buffer)
		ExFreePool( usSubNameAlloc.Buffer );

	pusKeyName->MaximumLength = pusKeyName->Length;

	return status;
}

tefVerdict
RegOpNotify_Common (
	ULONG FltCode,
	PKEY_INFO pKeyInfo,
	PUNICODE_STRING ValueName,
	PREG_VALUE pRegValue,
	NTSTATUS RetStatus,
	BOOLEAN HasStatus,
	PROCESSING_TYPE ProcessingType
	)
{
	tefVerdict		Verdict = efVerdict_Default;
	NTSTATUS		status = STATUS_SUCCESS;
	UNICODE_STRING	usKeyName;
	PSID			psid = NULL;
	ULONG           desiredMask = 0;

	RtlInitEmptyUnicodeString( &usKeyName, NULL, 0 );

	if (HasStatus)
	{
		if (!NT_SUCCESS( RetStatus ))
			pKeyInfo->pKeyObject = NULL;
	}

	status = Reg_InitKeyName( &usKeyName, pKeyInfo->pRootObject, pKeyInfo->pKeyObject, pKeyInfo->p_cntdRelKeyName );
	if (!NT_SUCCESS( status ))
		return efVerdict_Default;

	if (IsSkipRegKey( &usKeyName ) )
	{
		DoTraceEx( TRACE_LEVEL_WARNING, DC_DRIVER, "skip key '%wZ'", &usKeyName );

		ExFreePool( usKeyName.Buffer );
		return efVerdict_Default;
	}

	if ( !NT_SUCCESS( SeGetUserSid ( NULL, &psid ) ) )
		psid = NULL;

	if (FltCode == Interceptor_CreateKey ||
		FltCode == Interceptor_SetValueKey ||
		FltCode == Interceptor_SaveKey ||
		FltCode == Interceptor_SetInformationKey)
	{
		SetFlag( desiredMask, FILE_OP_WRITE );
	}
	
	if (FltCode == Interceptor_QueryKey || FltCode == Interceptor_QueryValueKey)
	{
		SetFlag( desiredMask, FILE_OP_READ );
	}

	if (FltCode == Interceptor_DeleteKey || FltCode == Interceptor_DeleteValueKey || FltCode == Interceptor_RenameKey)
	{
		SetFlag( desiredMask, FILE_OP_DELETE );
	}

	Verdict = IsAllowAccessReg(
				psid,
				PsGetCurrentProcessId(),
				usKeyName,
				ValueName,
				desiredMask
				);

	if ( efIsAllow( Verdict ) )
	if (IsNeedFilterEvent( FLTTYPE_REGS, FltCode, 0 ))
	{
		ULONG				ParamBuffer_Size = 0;
		ULONG				ParamCount = 0;
		PSINGLE_PARAM		pSingleParam = NULL;

		PFILTER_EVENT_PARAM	pParam = NULL;
		BOOLEAN				ValueName_Present = FALSE;
		BOOLEAN				ValueData_Present = FALSE;

		try {
			ParamCount = 2;
			ParamBuffer_Size = sizeof(FILTER_EVENT_PARAM) + 2 * sizeof(SINGLE_PARAM)+
				__SID_LENGTH + usKeyName.Length + 2;
			if (ValueName)
			{
				ParamBuffer_Size += sizeof(SINGLE_PARAM) + ValueName->Length + sizeof(WCHAR);
				ParamCount++;
				ValueName_Present = TRUE;
			}
			if (pRegValue && pRegValue->m_pData)
			{
				ParamBuffer_Size += 2 * sizeof(SINGLE_PARAM) + sizeof(ULONG) + pRegValue->m_Size;
				ParamCount += 2;
				ValueData_Present = TRUE;
			}
			if (HasStatus)
			{
				ParamBuffer_Size += sizeof(SINGLE_PARAM) + sizeof(ULONG);
				ParamCount++;
			}
			
			if (pKeyInfo->m_DesiredAccess)
			{
				ParamBuffer_Size += sizeof(SINGLE_PARAM) + sizeof(ULONG);
				ParamCount++;
			}

			pParam = ExAllocatePoolWithTag( PagedPool, ParamBuffer_Size, 'rbos' );
			if (!pParam)
			{
				ExFreePool( usKeyName.Buffer );
				if (psid)
					ExFreePool( psid );
				return efVerdict_Default;
			}
			
			FILTER_PARAM_COMMONINIT( pParam, FLTTYPE_REGS, FltCode, 0, ProcessingType, ParamCount );

			pSingleParam = (PSINGLE_PARAM)pParam->Params;
			SINGLE_PARAM_INIT_SID( pSingleParam );

			SINGLE_PARAM_SHIFT( pSingleParam );
			SINGLE_PARAM_INIT_NONE( pSingleParam, _PARAM_OBJECT_URL_W );
			pSingleParam->ParamSize = usKeyName.Length + 2;
			memcpy( pSingleParam->ParamValue, usKeyName.Buffer, usKeyName.Length );
			((__int8*)pSingleParam->ParamValue)[usKeyName.Length] = 0;
			((__int8*)pSingleParam->ParamValue)[usKeyName.Length + 1] = 0;
			
			if (ValueName_Present)
			{
				SINGLE_PARAM_SHIFT( pSingleParam );
				SINGLE_PARAM_INIT_NONE( pSingleParam, _PARAM_OBJECT_URL_PARAM_W );

				pSingleParam->ParamSize = ValueName->Length + sizeof(WCHAR);
				((PWCHAR)pSingleParam->ParamValue)[ValueName->Length/sizeof(WCHAR)] = 0;
				try {
					memcpy( pSingleParam->ParamValue, ValueName->Buffer, ValueName->Length );
				} __except(CheckException()) {
					_dbg_break_;
					pSingleParam->ParamSize = 0;
				}
			}

			if (ValueData_Present)
			{
				SINGLE_PARAM_SHIFT( pSingleParam );
				SINGLE_PARAM_INIT_ULONG( pSingleParam, _PARAM_OBJECT_SOURCE_ID, pRegValue->m_Type );

				SINGLE_PARAM_SHIFT( pSingleParam );
				SINGLE_PARAM_INIT_NONE( pSingleParam, _PARAM_OBJECT_BINARYDATA );
				pSingleParam->ParamSize = pRegValue->m_Size;
				try {
					memcpy( pSingleParam->ParamValue, pRegValue->m_pData, pRegValue->m_Size );
				} __except(CheckException()) {
					_dbg_break_;
					pSingleParam->ParamSize = 0;
				}
			}
			
			if (HasStatus)
			{
				SINGLE_PARAM_SHIFT( pSingleParam );
				SINGLE_PARAM_INIT_ULONG( pSingleParam, _PARAM_RET_STATUS, RetStatus );
			}

			if (pKeyInfo->m_DesiredAccess)
			{
				SINGLE_PARAM_SHIFT( pSingleParam );
				SINGLE_PARAM_INIT_ULONG( pSingleParam, _PARAM_OBJECT_ACCESSATTR, pKeyInfo->m_DesiredAccess );
			}

			try {
				Verdict = FilterEventImp( pParam, NULL, 0 );
			} __except(CheckException()) {
				DoTraceEx( TRACE_LEVEL_ERROR, DC_DRIVER, "exception in RegOpNotify_Common - FilterEvent" );
				_dbg_break_;
			}
		} __except(CheckException()) {
			DoTraceEx( TRACE_LEVEL_ERROR, DC_DRIVER, "exception in RegOpNotify_Common" );
			_dbg_break_;
		}

		if (pParam)
			ExFreePool( pParam );
	}

	ExFreePool( usKeyName.Buffer );

	if ( psid )
		ExFreePool( psid );

	return Verdict;
}

VOID
AttachObjectContext (
	PVOID		Object,
	NTSTATUS	status_op,
	ACCESS_MASK	DesiredAccess
	)
{
	if (!pfCmSetCallbackObjectContext)
		return;

	if (NT_SUCCESS( status_op ) && Object && DesiredAccess && gRegHookMajorVersion)
	{
		PREGKEY_CONTEXT pKeyContext = ExAllocatePoolWithTag (
			PagedPool,
			sizeof(REGKEY_CONTEXT),
			'rbos'
			);

		if (pKeyContext)
		{
			NTSTATUS status = STATUS_UNSUCCESSFUL;

			pKeyContext->m_DesiredAccess = DesiredAccess;

			status = pfCmSetCallbackObjectContext (
				Object,
				&gRegCallback_Cookie,
				pKeyContext,
				NULL
				);

			if (!NT_SUCCESS( status ))
			{
				// what this status mean?
				if (STATUS_PREDEFINED_HANDLE != status_op)
					_dbg_break_;
				
				ExFreePool( pKeyContext );
			}
		}
	}
}

VOID
TransEnlistKey (
	__in PVOID pKeyObject,
	__in PMKAV_TRANSACTION_CONTEXT pTransactionContext
	)
{
	POBJECT_NAME_INFORMATION pKeyInfo;

	pKeyInfo = (POBJECT_NAME_INFORMATION) ExAllocatePoolWithTag( PagedPool, 0x300, 'KBos' );
	if (pKeyInfo)
	{
		ULONG RetLen = 0;
		NTSTATUS status;

		status = ObQueryNameString(pKeyObject, pKeyInfo, 0x300, &RetLen);
		if (NT_SUCCESS(status) && pKeyInfo->Name.Buffer)
		{
			PVOID pResult;
			PMKAV_ENLISTED_KEY pKeyEntry;

			ULONG item_size = sizeof(MKAV_ENLISTED_FILE) + pKeyInfo->Name.Length + sizeof(WCHAR);

			pKeyEntry = (PMKAV_ENLISTED_KEY) ExAllocatePoolWithTag( PagedPool, item_size, 'TbOs' );
			if (pKeyEntry)
			{
				// prepare zero-terminated string
				pKeyEntry->m_ProcessId = HandleToUlong( PsGetCurrentProcessId() );
				memcpy(pKeyEntry->m_KeyPath, pKeyInfo->Name.Buffer, pKeyInfo->Name.Length);
				pKeyEntry->m_KeyPath[pKeyInfo->Name.Length/sizeof(WCHAR)] = 0;

				pKeyEntry->m_NameLen = pKeyInfo->Name.Length + sizeof(WCHAR);

				AcquireResourceExclusive( &pTransactionContext->m_RegLock );
				
				pResult = RtlInsertElementGenericTable (
					&pTransactionContext->m_EnlistedKeysTable,
					pKeyEntry,
					item_size,
					NULL
					);
				
				ReleaseResource( &pTransactionContext->m_RegLock );

				DoTraceEx( TRACE_LEVEL_INFORMATION, DC_DRIVER, "enlisted registry key \'%S\' into transaction %p",
					pKeyEntry->m_KeyPath, pTransactionContext->m_pTransObj) ;

				ExFreePool( pKeyEntry );
			}
		}

		ExFreePool( pKeyInfo );
	}
}

VOID
PostCreateKeyTransProcessing(
	__in PREG_POST_OPERATION_INFORMATION pRegInfo
	)
{
	NTSTATUS status = STATUS_UNSUCCESSFUL;

	PVOID pBoundTransaction = NULL;
	PVOID pSysVolInstance = NULL;

	PMKAV_TRANSACTION_CONTEXT pTransactionContext = NULL;

	KIRQL kIrql = KeGetCurrentIrql();

	if (APC_LEVEL < kIrql)
	{
		_dbg_break_;
		return;
	}

	if (!g_bTransactionOperationsSupported)
		return;

	if (!pRegInfo || !NT_SUCCESS(pRegInfo->Status) || !pRegInfo->Object)
		return;

	pBoundTransaction = g_pfCmGetBoundTransaction( &gRegCallback_Cookie, pRegInfo->Object );

	if (!pBoundTransaction)
		return;
	
	pSysVolInstance = GetSystemVolumeInstance();

	if (!pSysVolInstance)
		return;

	status = g_pfFltGetTransactionContext( pSysVolInstance, pBoundTransaction, (PFLT_CONTEXT) &pTransactionContext );
	
	if (!NT_SUCCESS(status))
		pTransactionContext = NULL;

	if (!pTransactionContext)
	{
		// no context allocated for this transaction, allocate one
		status = FltAllocateContext (
			Globals.m_Filter,
			FLT_TRANSACTION_CONTEXT,
			sizeof(MKAV_TRANSACTION_CONTEXT),
			NonPagedPool,
			(PFLT_CONTEXT*) &pTransactionContext
			);
		
		if (!NT_SUCCESS( status ))
		{
			_dbg_break_;
			pTransactionContext = NULL;
		}
		else
		{
			status = InitTransactionContext( pTransactionContext, pBoundTransaction );

			if (NT_SUCCESS(status))
			{
				status = g_pfFltSetTransactionContext (
					pSysVolInstance,
					pBoundTransaction,
					FLT_SET_CONTEXT_KEEP_IF_EXISTS,
					(PFLT_CONTEXT) pTransactionContext,
					NULL
					);
			}

			if (!NT_SUCCESS(status))
			{
				_dbg_break_;
				ReleaseFltContext( &pTransactionContext );
			}
		}
	}

	if (pTransactionContext)
	{
		g_pfFltEnlistInTransaction (
			pSysVolInstance,
			pBoundTransaction,
			pTransactionContext,
			FLT_MAX_TRANSACTION_NOTIFICATIONS
			);
		
		// enlist key into transaction
		TransEnlistKey( pRegInfo->Object, pTransactionContext );

		ReleaseFltContext( &pTransactionContext );
	}

	FltObjectDereference( pSysVolInstance );
}

NTSTATUS
RegOpNotify (
	__in PVOID  CallbackContext,
	__in PVOID  Argument1,
	__in PVOID  Argument2 
	)
{
	REG_NOTIFY_CLASS	RegNotifyClass = (REG_NOTIFY_CLASS)Argument1;
	NTSTATUS RetStatus = STATUS_SUCCESS;
	tefVerdict Verdict = efVerdict_Default;
	KEY_INFO KeyInfo;

	UNREFERENCED_PARAMETER( CallbackContext );

	if (!Argument2)
		return STATUS_SUCCESS;

	if ( KernelMode == ExGetPreviousMode() )
		return STATUS_SUCCESS;

	if (IsInvisible( PsGetCurrentThreadId(), 0 ))
		return STATUS_SUCCESS;

	InterlockedIncrement( &g_RegEnters );

	try
	{
		switch (RegNotifyClass)
		{
		case RegNtPreCreateKey:
			{
				PREG_PRE_CREATE_KEY_INFORMATION pRegInfo = (PREG_PRE_CREATE_KEY_INFORMATION) Argument2;
				INIT_KEYINFO( KeyInfo, pRegInfo->CompleteName, NULL, NULL );
				Verdict = RegOpNotify_Common( Interceptor_CreateKey, &KeyInfo, NULL, NULL, 0, FALSE, PreProcessing );
			}
			break;

		case RegNtPreCreateKeyEx:
			{
				PREG_CREATE_KEY_INFORMATION pRegInfo = (PREG_CREATE_KEY_INFORMATION) Argument2;
				INIT_KEYINFO( KeyInfo, pRegInfo->CompleteName, pRegInfo->RootObject, NULL );
				KeyInfo.m_DesiredAccess = pRegInfo->DesiredAccess;

				Verdict = RegOpNotify_Common( Interceptor_CreateKey, &KeyInfo, NULL, NULL, 0, FALSE, PreProcessing );
			}
			break;

		case RegNtPostOpenKeyEx:
			{
				PREG_POST_OPERATION_INFORMATION pRegInfo = (PREG_POST_OPERATION_INFORMATION) Argument2;

				if (gRegHookMajorVersion)
				{
					PREG_CREATE_KEY_INFORMATION pRegInfoPre = (PREG_CREATE_KEY_INFORMATION) pRegInfo->PreInformation;
					if (pRegInfoPre)
						AttachObjectContext( pRegInfo->Object, pRegInfo->Status, pRegInfoPre->DesiredAccess);
				}

				PostCreateKeyTransProcessing(pRegInfo);
			}

			break;

		case RegNtPostCreateKeyEx:
			{
				PREG_POST_OPERATION_INFORMATION pRegInfo = (PREG_POST_OPERATION_INFORMATION) Argument2;
				PREG_CREATE_KEY_INFORMATION pRegInfoPre = NULL;
				
				if (gRegHookMajorVersion)
				{
					pRegInfoPre = pRegInfo->PreInformation;
					if (pRegInfoPre)
						AttachObjectContext( pRegInfo->Object, pRegInfo->Status, pRegInfoPre->DesiredAccess);
				}
				
				INIT_KEYINFO( KeyInfo, NULL, NULL, pRegInfo->Object );

				if (pRegInfoPre)
					KeyInfo.m_DesiredAccess = pRegInfoPre->DesiredAccess;

				Verdict = RegOpNotify_Common( Interceptor_CreateKey, &KeyInfo, NULL, NULL, pRegInfo->Status,
					TRUE, PostProcessing );

				PostCreateKeyTransProcessing(pRegInfo);
			}
			break;

		case RegNtPreDeleteKey:
			{
				PREG_DELETE_KEY_INFORMATION pRegInfo = (PREG_DELETE_KEY_INFORMATION) Argument2;
				PREGKEY_CONTEXT pRegContext = NULL;
				
				if (gRegHookMajorVersion)
					pRegContext = pRegInfo->ObjectContext;

				INIT_KEYINFO( KeyInfo, NULL, NULL, pRegInfo->Object );

				if (pRegContext)
					KeyInfo.m_DesiredAccess = pRegContext->m_DesiredAccess;

				Verdict = RegOpNotify_Common( Interceptor_DeleteKey, &KeyInfo, NULL, NULL, 0, FALSE,
					PreProcessing );
			}
			break;

		case RegNtPostDeleteKey:
			{
				// this event is fired only if delete operation is unsuccessful
				PREG_POST_OPERATION_INFORMATION pRegInfo = (PREG_POST_OPERATION_INFORMATION) Argument2;
				PREGKEY_CONTEXT pRegContext = NULL;

				if (gRegHookMajorVersion)
					pRegContext = pRegInfo->ObjectContext;

				INIT_KEYINFO( KeyInfo, NULL, NULL, pRegInfo->Object );
				
				if (pRegContext)
					KeyInfo.m_DesiredAccess = pRegContext->m_DesiredAccess;

				Verdict = RegOpNotify_Common( Interceptor_DeleteKey, &KeyInfo,	NULL, NULL,
					pRegInfo->Status, TRUE, PostProcessing );
			}
			break;

		case RegNtPreDeleteValueKey:
			{
				PREG_DELETE_VALUE_KEY_INFORMATION pRegInfo = (PREG_DELETE_VALUE_KEY_INFORMATION) Argument2;
				PREGKEY_CONTEXT pRegContext = NULL;

				if (gRegHookMajorVersion)
					pRegContext = pRegInfo->ObjectContext;

				INIT_KEYINFO( KeyInfo, NULL, NULL, pRegInfo->Object );

				if (pRegContext)
					KeyInfo.m_DesiredAccess = pRegContext->m_DesiredAccess;

				Verdict = RegOpNotify_Common( Interceptor_DeleteValueKey, &KeyInfo, pRegInfo->ValueName, NULL,
					0, FALSE, PreProcessing );
			}
			break;

		case RegNtPostDeleteValueKey:
			{
				PREG_POST_OPERATION_INFORMATION pRegInfo = (PREG_POST_OPERATION_INFORMATION) Argument2;
				PREGKEY_CONTEXT pRegContext = NULL;

				if (gRegHookMajorVersion)
					pRegContext = pRegInfo->ObjectContext;

				INIT_KEYINFO( KeyInfo, NULL, NULL, pRegInfo->Object );

				if (pRegContext)
					KeyInfo.m_DesiredAccess = pRegContext->m_DesiredAccess;

				Verdict = RegOpNotify_Common( Interceptor_DeleteValueKey, &KeyInfo, NULL, NULL,
					pRegInfo->Status, TRUE, PostProcessing );
			}
			break;

		case RegNtPreQueryValueKey:
			{
				PREG_QUERY_VALUE_KEY_INFORMATION pRegInfo = (PREG_QUERY_VALUE_KEY_INFORMATION) Argument2;
				PREGKEY_CONTEXT pRegContext = NULL;

				if (gRegHookMajorVersion)
					pRegContext = pRegInfo->ObjectContext;

				INIT_KEYINFO( KeyInfo, NULL, NULL, pRegInfo->Object );

				if (pRegContext)
					KeyInfo.m_DesiredAccess = pRegContext->m_DesiredAccess;

				Verdict = RegOpNotify_Common( Interceptor_QueryValueKey, &KeyInfo,	pRegInfo->ValueName,
					NULL, 0, FALSE, PreProcessing );
			}
			break;

		case RegNtPostQueryValueKey:
			{
				PREG_POST_OPERATION_INFORMATION pRegInfo = (PREG_POST_OPERATION_INFORMATION) Argument2;
				PREGKEY_CONTEXT pRegContext = NULL;

				if (gRegHookMajorVersion)
					pRegContext = pRegInfo->ObjectContext;

				INIT_KEYINFO( KeyInfo, NULL, NULL, pRegInfo->Object );

				if (pRegContext)
					KeyInfo.m_DesiredAccess = pRegContext->m_DesiredAccess;

				Verdict = RegOpNotify_Common( Interceptor_QueryValueKey, &KeyInfo,	NULL,
					NULL, pRegInfo->Status, TRUE, PostProcessing );
			}
			break;

		case RegNtPreSetValueKey:
			{
				PREG_SET_VALUE_KEY_INFORMATION pRegInfo = (PREG_SET_VALUE_KEY_INFORMATION) Argument2;
				REG_VALUE RegValue;

				PREGKEY_CONTEXT pRegContext = NULL;

				if ((PVOID) MmUserProbeAddress < pRegInfo->Data)
				{
					// skip address kernel access
				}
				else
				{
					if (gRegHookMajorVersion)
						pRegContext = pRegInfo->ObjectContext;

					INIT_KEYINFO( KeyInfo, NULL, NULL, pRegInfo->Object );

					if (pRegContext)
						KeyInfo.m_DesiredAccess = pRegContext->m_DesiredAccess;

					RegValue.m_Type = pRegInfo->Type;
					RegValue.m_Size = pRegInfo->DataSize;
					RegValue.m_pData = pRegInfo->Data;

					if (pRegContext)
						KeyInfo.m_DesiredAccess = pRegContext->m_DesiredAccess;

					Verdict = RegOpNotify_Common( Interceptor_SetValueKey, &KeyInfo, pRegInfo->ValueName,
						&RegValue, 0, FALSE, PreProcessing );

	#ifdef WIN2K
					if (efIsAllow( Verdict))
					{
						RegOpNotify_Common( Interceptor_SetValueKey, &KeyInfo, pRegInfo->ValueName,
							&RegValue, 0, FALSE, PostProcessing );
					}
	#else
					if (efIsAllow( Verdict ))
						Reg_ThreadSetPreInfo( pRegInfo );
	#endif // w2k
				}
			}
			break;

		case RegNtPostSetValueKey:
			{
				PREG_POST_OPERATION_INFORMATION pRegInfo = (PREG_POST_OPERATION_INFORMATION) Argument2;
				PREG_SET_VALUE_KEY_INFORMATION	pRegInfoPre = NULL;
				PREGKEY_CONTEXT pRegContext = NULL;
				PUNICODE_STRING pValueName = NULL;
				REG_VALUE RegValue;
				PREG_VALUE pRegValue = NULL;

				if (gRegHookMajorVersion)
					pRegContext = pRegInfo->ObjectContext;

				INIT_KEYINFO( KeyInfo, NULL, NULL, pRegInfo->Object );

				if (pRegContext)
					KeyInfo.m_DesiredAccess = pRegContext->m_DesiredAccess;

				pRegInfoPre = Reg_ThreadGetPreInfo( pRegInfo );
				if (pRegInfoPre)
				{
					pValueName = pRegInfoPre->ValueName;
					RegValue.m_Type = pRegInfoPre->Type;
					RegValue.m_Size = pRegInfoPre->DataSize;
					RegValue.m_pData = pRegInfoPre->Data;
					pRegValue = &RegValue;
				}

				RegOpNotify_Common( Interceptor_SetValueKey, &KeyInfo, pValueName,
					pRegValue, pRegInfo->Status, TRUE, PostProcessing );

				Reg_ThreadReleasePreInfo();
			}
			break;

		case RegNtPreQueryMultipleValueKey:
			{
				PREG_QUERY_MULTIPLE_VALUE_KEY_INFORMATION pRegInfo = (PREG_QUERY_MULTIPLE_VALUE_KEY_INFORMATION) Argument2;
				PREGKEY_CONTEXT pRegContext = NULL;

				if (gRegHookMajorVersion)
					pRegContext = pRegInfo->ObjectContext;

				INIT_KEYINFO( KeyInfo, NULL, NULL, pRegInfo->Object );

				if (pRegContext)
					KeyInfo.m_DesiredAccess = pRegContext->m_DesiredAccess;

				Verdict = RegOpNotify_Common( Interceptor_QueryMultipleValueKey, &KeyInfo,
					NULL, NULL, 0, FALSE, PreProcessing );
			}
			break;

		case RegNtPostQueryMultipleValueKey:
			{
				PREG_POST_OPERATION_INFORMATION pRegInfo = (PREG_POST_OPERATION_INFORMATION) Argument2;
				PREGKEY_CONTEXT pRegContext = NULL;

				if (gRegHookMajorVersion)
					pRegContext = pRegInfo->ObjectContext;

				INIT_KEYINFO( KeyInfo, NULL, NULL, pRegInfo->Object );

				if (pRegContext)
					KeyInfo.m_DesiredAccess = pRegContext->m_DesiredAccess;

				Verdict = RegOpNotify_Common( Interceptor_QueryMultipleValueKey, &KeyInfo,	NULL,
					NULL, pRegInfo->Status, TRUE, PostProcessing );
			}
			break;


		case RegNtPreRenameKey:
			{
				PREG_RENAME_KEY_INFORMATION pRegInfo = (PREG_RENAME_KEY_INFORMATION) Argument2;
				PREGKEY_CONTEXT pRegContext = NULL;

				if (gRegHookMajorVersion)
					pRegContext = pRegInfo->ObjectContext;

				INIT_KEYINFO( KeyInfo, NULL, NULL, pRegInfo->Object );

				if (pRegContext)
					KeyInfo.m_DesiredAccess = pRegContext->m_DesiredAccess;

				Verdict = RegOpNotify_Common( Interceptor_RenameKey, &KeyInfo, pRegInfo->NewName,
					NULL, 0, TRUE, PreProcessing );
			}
			break;

		case RegNtPreLoadKey:
			break;

		/*case RegNtPreLoadKey:
			{
				NTSTATUS status_tmp;
				PREG_LOAD_KEY_INFORMATION  pRegInfo = (PREG_LOAD_KEY_INFORMATION) Argument2;
				PREGKEY_CONTEXT pRegContext = NULL;

				if (gRegHookMajorVersion)
					pRegContext = pRegInfo->ObjectContext;

				LUID luid;

				status_tmp = SeGetUserLuid( &luid );
				if (NT_SUCCESS( status_tmp ))
				{
					LUID SystemLuid = SYSTEM_LUID;
					if (sizeof(LUID) == RtlCompareMemory( &SystemLuid, &luid, sizeof(LUID) ))
					{
						// system load, skip
					}
					else
					{
						INIT_KEYINFO( KeyInfo, NULL, NULL, pRegInfo->Object );

						if (pRegContext)
							KeyInfo.m_DesiredAccess = pRegContext->m_DesiredAccess;

						_dbg_break_;

						Verdict = RegOpNotify_Common( Interceptor_LoadKey2, &KeyInfo, pRegInfo->KeyName,
							NULL, 0, TRUE, PreProcessing );
					}
				}
			}
			break;*/
		case RegNtCallbackObjectContextCleanup:
			{
				PREG_CALLBACK_CONTEXT_CLEANUP_INFORMATION pRegInfo = (PREG_CALLBACK_CONTEXT_CLEANUP_INFORMATION) Argument2;
				if (pRegInfo->ObjectContext)
				{
					PREGKEY_CONTEXT pKeyContext = (PREGKEY_CONTEXT) pRegInfo->ObjectContext;
					ExFreePool( pKeyContext );
				}
			}
			break;
		
		default:
			break;
		}
	}
	__except(CheckException())
	{
		DoTraceEx( TRACE_LEVEL_ERROR, DC_DRIVER, "common exception in RegOpNotify_Common" );
	}

	RetStatus = STATUS_ACCESS_DENIED;
	if (efIsAllow( Verdict))
		RetStatus = STATUS_SUCCESS;

	InterlockedDecrement( &g_RegEnters );

	return RetStatus;
}


NTSTATUS
RegFltInit (
	)
{
	NTSTATUS status = STATUS_SUCCESS;
#ifdef WIN2K
	#pragma message ("----------------------- w2k compatibility: no CmRegisterCallback")
#else
	ExInitializeFastMutex( &gfmTableRegSetValue );
	RtlInitializeGenericTable( &gTableRegSetValue, TSetValue_Compare, Tables_Allocate, Tables_Free, NULL);

	if (pfCmGetCallbackVersion)
		pfCmGetCallbackVersion( &gRegHookMajorVersion, NULL );

	if (pfCmRegisterCallbackEx)
	{
		UNICODE_STRING usAltitude;
		
		status = GetAltitude ( &usAltitude );
		if ( NT_SUCCESS(status) )
		{
			status = pfCmRegisterCallbackEx (
				RegOpNotify,
				&usAltitude,
				Globals.m_FilterDriverObject,
				NULL,
				&gRegCallback_Cookie,
				NULL
				);
			ExFreePool( usAltitude.Buffer );
		}
	}
	else
		status = CmRegisterCallback( RegOpNotify, NULL, &gRegCallback_Cookie );

	DoTraceEx( TRACE_LEVEL_WARNING, DC_DRIVER, "RegFlt init status 0x%x, reg cookie 0x%x:0x%x",
		status, gRegCallback_Cookie.HighPart,  gRegCallback_Cookie.LowPart );

	if (NT_SUCCESS( status ))
		gbRegFltInited = TRUE;
#endif // WIN2K

	return status;
}

VOID
RegFltDone (
	)
{
#ifdef WIN2K
	#pragma message ("----------------------- w2k compatibility: no CmUnRegisterCallback")
#else
	if (gbRegFltInited)
	{
		PThreadSetInfo pTInfo = NULL;

		DoTraceEx( TRACE_LEVEL_WARNING, DC_DRIVER, "RegFlt unregistering..." );
		
		CmUnRegisterCallback( gRegCallback_Cookie );
		
		DoTraceEx( TRACE_LEVEL_WARNING, DC_DRIVER, "RegFlt unregistered" );

		WaitInterlockedObj( &g_RegEnters );

		//+ free entries..
		ExAcquireFastMutex( &gfmTableRegSetValue );
		
		pTInfo = RtlGetElementGenericTable( &gTableRegSetValue, 0 );
		while(pTInfo)
		{
			TSetValue_DeleteElement( pTInfo );

			pTInfo = RtlGetElementGenericTable( &gTableRegSetValue, 0 );
		};

		ExReleaseFastMutex( &gfmTableRegSetValue );

		//- free entries..
		
		DoTraceEx( TRACE_LEVEL_WARNING, DC_DRIVER, "RegFlt done" );
	}
#endif // WIN2K
}

	/*if (p_cntdRelKeyName && ProbeMemRead( p_cntdRelKeyName ))
	{
		PWCHAR RootPath = NULL;
		PWCHAR RelKeyName = NULL;
		size_t keyname_size = sizeof(WCHAR) * MAX_REGPATH_LEN;

		KeyName = ExAllocatePoolWithTag( PagedPool, keyname_size , 'rbos' );
		if (!KeyName)
		{
			if ( psid )
				ExFreePool( psid );

			return efVerdict_Default;
		}

		if (pKeyInfo->pRootObject)
			RootPath = QueryObjectName( pKeyInfo->pRootObject );

		KeyName[0] = 0;
		if (RootPath)
		{
			RtlStringCbCopyW( KeyName, keyname_size, RootPath );
			ExFreePool( RootPath );
			RootPath = NULL;
		}

		RelKeyName = ExAllocatePoolWithTag(PagedPool, p_cntdRelKeyName->Length + sizeof(WCHAR), 'rbos');
		if (!RelKeyName)
		{
			if ( psid )
				ExFreePool( psid );
			
			ExFreePool( KeyName );
			return efVerdict_Default;
		}

		memcpy( RelKeyName, p_cntdRelKeyName->Buffer, p_cntdRelKeyName->Length );
		RelKeyName[p_cntdRelKeyName->Length/sizeof(WCHAR)] = 0;

		RtlStringCbCatW( KeyName, keyname_size, L"\\" );
		RtlStringCbCatW( KeyName, keyname_size, RelKeyName );
		ExFreePool( RelKeyName );
		RelKeyName = NULL;

		KeyName_Len = wcslen( KeyName );
	}
	else
	{
		PVOID pKeyObject = pKeyInfo->pKeyObject;
		if (!pKeyObject || !ProbeMemRead( pKeyObject ))
		{
			if ( psid )
				ExFreePool( psid );

			return efVerdict_Default;
		}

		KeyName = QueryObjectName( pKeyObject );
		if (!KeyName)
		{
			if ( psid )
				ExFreePool( psid );

			return efVerdict_Default;
		}

		KeyName_Len = wcslen( KeyName );
		if (!KeyName_Len)
		{
			_dbg_break_;
		}
	}
	
	RtlInitUnicodeString( &usKeyName, KeyName );*/
