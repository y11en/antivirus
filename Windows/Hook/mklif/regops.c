#include "pch.h"

typedef
NTSTATUS
(NTAPI
*t_fNtOpenKey)(
		  __out PHANDLE KeyHandle,
		  __in ACCESS_MASK DesiredAccess,
		  __in POBJECT_ATTRIBUTES ObjectAttributes
		  );

typedef
NTSTATUS
(NTAPI
*t_fNtQueryValueKey)(
				__in HANDLE KeyHandle,
				__in PUNICODE_STRING ValueName,
				__in KEY_VALUE_INFORMATION_CLASS KeyValueInformationClass,
				__out_bcount_opt(Length) PVOID KeyValueInformation,
				__in ULONG Length,
				__out PULONG ResultLength
				);

// dynamicly resolved
static t_fNtOpenKey NtOpenKey = NULL;
static t_fNtQueryValueKey NtQueryValueKey = NULL;

static BOOLEAN g_bInited = FALSE;

static BOOLEAN g_bFail = FALSE;

//__inline NTSTATUS Check_RegPath(IN PWCHAR KeyPath);
//__inline NTSTATUS Check_ValueName(IN PWCHAR ValueName);

// OutBuffer_Size >= sizeof(REG_OP_RESULT)
NTSTATUS Reg_CreateKey(IN PREG_OP_REQUEST pRegOp_Request, OUT PVOID pOutBuffer,
						IN ULONG OutBuffer_Size,OUT ULONG_PTR *pRet_Size);
// OutBuffer_Size >= sizeof(REG_OP_RESULT)
NTSTATUS Reg_DeleteKey(IN PREG_OP_REQUEST pRegOp_Request, OUT PVOID pOutBuffer,
						IN ULONG OutBuffer_Size,OUT ULONG_PTR *pRet_Size);
// OutBuffer_Size >= sizeof(REG_OP_RESULT)+sizeof(REG_ENUM_RESULT)+string data length
NTSTATUS Reg_EnumKey(IN PREG_OP_REQUEST pRegOp_Request, OUT PVOID pOutBuffer,
						IN ULONG OutBuffer_Size,OUT ULONG_PTR *pRet_Size);
// OutBuffer_Size >= sizeof(REG_OP_RESULT)
NTSTATUS Reg_CheckKeyPresent(IN PREG_OP_REQUEST pRegOp_Request, OUT PVOID pOutBuffer,
								IN ULONG OutBuffer_Size,OUT ULONG_PTR *pRet_Size);
// OutBuffer_Size >= sizeof(REG_OP_RESULT)+sizeof(REG_ENUM_RESULT)+string data length
NTSTATUS Reg_EnumValue(IN PREG_OP_REQUEST pRegOp_Request, OUT PVOID pOutBuffer,
						IN ULONG OutBuffer_Size,OUT ULONG_PTR *pRet_Size);
// OutBuffer_Size >= sizeof(REG_OP_RESULT)
NTSTATUS Reg_CheckValuePresent(IN PREG_OP_REQUEST pRegOp_Request, OUT PVOID pOutBuffer,
								IN ULONG OutBuffer_Size,OUT ULONG_PTR *pRet_Size);
// OutBuffer_Size >= sizeof(REG_OP_RESULT)+sizeof(REG_QUERY_RESULT)+string data length
NTSTATUS Reg_QueryValue(IN PREG_OP_REQUEST pRegOp_Request, OUT PVOID pOutBuffer,
						IN ULONG OutBuffer_Size,OUT ULONG_PTR *pRet_Size);
// OutBuffer_Size >= sizeof(REG_OP_RESULT)
NTSTATUS Reg_SetValue(IN PREG_OP_REQUEST pRegOp_Request, OUT PVOID pOutBuffer,
						IN ULONG OutBuffer_Size,OUT ULONG_PTR *pRet_Size);
// OutBuffer_Size >= sizeof(REG_OP_RESULT)
NTSTATUS Reg_DeleteValue(IN PREG_OP_REQUEST pRegOp_Request, OUT PVOID pOutBuffer,
						IN ULONG OutBuffer_Size,OUT ULONG_PTR *pRet_Size);


__inline NTSTATUS
Check_RegPath (
	IN PWCHAR KeyPath )
{
	__try
	{
		if (wcslen(KeyPath) > MAX_REGPATH_LEN)
			return STATUS_INVALID_PARAMETER;
	}
	__except(CheckException())
	{
		return STATUS_INVALID_PARAMETER;
	}

	return STATUS_SUCCESS;
}

__inline NTSTATUS
Check_ValueName (
	IN PWCHAR ValueName )
{
	__try
	{
		if (wcslen(ValueName) > MAX_KEYVALUENAME_LEN)
			return STATUS_INVALID_PARAMETER;
	}
	__except(CheckException())
	{
		return STATUS_INVALID_PARAMETER;
	}

	return STATUS_SUCCESS;
}

NTSTATUS
Reg_CreateKey (
	IN PREG_OP_REQUEST pRegOp_Request,
	OUT PVOID pOutBuffer,
	IN ULONG OutBuffer_Size,
	OUT ULONG_PTR *pRet_Size )
{
	NTSTATUS ntStatus = STATUS_SUCCESS;
	PWCHAR				Base_KeyPath,
						Slash_Position;
	HANDLE				hBaseKey,
						hCreatedKey;
	OBJECT_ATTRIBUTES	BaseKey_OA,
						Key_OA;
	UNICODE_STRING		cntdBase_KeyPath,
						cntdFull_KeyPath;
	ULONG				CreatedKey_Disposition;
	PVOID				pObject;

	if (OutBuffer_Size < sizeof(REG_OP_RESULT))
		return STATUS_BUFFER_TOO_SMALL;

	*pRet_Size = 0;

	ntStatus = Check_RegPath(pRegOp_Request->m_KeyPath);
	if (!NT_SUCCESS(ntStatus))
		return ntStatus;

	Base_KeyPath = ExAllocatePoolWithTag(PagedPool, MAX_REGPATH_LEN, 'Gbos');
	if (!Base_KeyPath)
		return STATUS_UNSUCCESSFUL;

	do
	{
		RtlStringCbCopyW( Base_KeyPath, MAX_REGPATH_LEN, pRegOp_Request->m_KeyPath );
// find last '\' character to identify key name
		Slash_Position = wcsrchr(Base_KeyPath, L'\\');
		if (!Slash_Position)
		{
			ntStatus = STATUS_INVALID_PARAMETER;
			break;
		}
// terminate base key path substring	
		*Slash_Position = 0;

// open base key handle to create subkey
		RtlInitUnicodeString(&cntdBase_KeyPath, Base_KeyPath);
		InitializeObjectAttributes(&BaseKey_OA, &cntdBase_KeyPath,
			OBJ_CASE_INSENSITIVE | OBJ_KERNEL_HANDLE, NULL, NULL);
		
		ntStatus = ZwOpenKey(&hBaseKey, KEY_CREATE_SUB_KEY, &BaseKey_OA);
// here is \perflib\* workaround
		if (!(	NT_SUCCESS(ntStatus) && 
				NT_SUCCESS(ObReferenceObjectByHandle(
							hBaseKey,
							0,
							NULL,
							KernelMode,
							&pObject,
							NULL
				))
		))
			break;
		ObDereferenceObject(pObject);

		hCreatedKey = hBaseKey;
		RtlInitUnicodeString(&cntdFull_KeyPath, pRegOp_Request->m_KeyPath);
		InitializeObjectAttributes(&Key_OA, &cntdFull_KeyPath,
			OBJ_CASE_INSENSITIVE | OBJ_KERNEL_HANDLE, NULL, NULL);
		
		ntStatus = ZwCreateKey(&hCreatedKey, 0, &Key_OA, 0, NULL, REG_OPTION_NON_VOLATILE, &CreatedKey_Disposition);
		ZwClose(hBaseKey);
		
		if (!NT_SUCCESS(ntStatus))
			break;

		ZwClose(hCreatedKey);

		((PREG_OP_RESULT)pOutBuffer)->m_Status = CreatedKey_Disposition == REG_CREATED_NEW_KEY?
												RegOp_StatusSuccess : RegOp_StatusAlreadyExisting;
		((PREG_OP_RESULT)pOutBuffer)->m_BufferLen = 0;
		*pRet_Size = sizeof(REG_OP_RESULT);

	} while (FALSE);

	ExFreePool(Base_KeyPath);

	return NT_SUCCESS(ntStatus)?STATUS_SUCCESS:ntStatus;
}

NTSTATUS
Reg_DeleteKey (
	IN PREG_OP_REQUEST pRegOp_Request,
	OUT PVOID pOutBuffer,
	IN ULONG OutBuffer_Size,
	OUT ULONG_PTR *pRet_Size )
{
	NTSTATUS			ntStatus = STATUS_SUCCESS;
	UNICODE_STRING		cntdKeyPath;
	OBJECT_ATTRIBUTES	Key_OA;
	HANDLE				hKey;
	PVOID				pObject;

	if (OutBuffer_Size < sizeof(REG_OP_RESULT))
		return STATUS_BUFFER_TOO_SMALL;

	*pRet_Size = 0;

	ntStatus = Check_RegPath(pRegOp_Request->m_KeyPath);
	if (!NT_SUCCESS(ntStatus))
		return ntStatus;

	RtlInitUnicodeString(&cntdKeyPath, pRegOp_Request->m_KeyPath);
	InitializeObjectAttributes(&Key_OA, &cntdKeyPath, OBJ_CASE_INSENSITIVE | OBJ_KERNEL_HANDLE,
		NULL, NULL);

	ntStatus = ZwOpenKey(&hKey, DELETE, &Key_OA);
// here is \perflib\* workaround
	if (!(	NT_SUCCESS(ntStatus) && 
			NT_SUCCESS(ObReferenceObjectByHandle(
						hKey,
						0,
						NULL,
						KernelMode,
						&pObject,
						NULL
			))
	))
		return ntStatus;
	ObDereferenceObject(pObject);

	ntStatus = ZwDeleteKey(hKey);
	ZwClose(hKey);

	if (!NT_SUCCESS(ntStatus))
		return ntStatus;

	((PREG_OP_RESULT)pOutBuffer)->m_Status = RegOp_StatusSuccess;
	((PREG_OP_RESULT)pOutBuffer)->m_BufferLen = 0;
	*pRet_Size = sizeof(REG_OP_RESULT);

	return STATUS_SUCCESS;
}

NTSTATUS
Reg_EnumKey (
	IN PREG_OP_REQUEST pRegOp_Request,
	OUT PVOID pOutBuffer,
	IN ULONG OutBuffer_Size,
	OUT ULONG_PTR *pRet_Size )
{
	NTSTATUS				ntStatus;
	UNICODE_STRING			cntdKeyPath;
	OBJECT_ATTRIBUTES		Key_OA;
	HANDLE					hKey;
	ULONG					Key_Info_Len,
							RegOpBuffer_Len,
							Result_Len;
	PKEY_BASIC_INFORMATION	pKey_Info;
	PREG_OP_RESULT			pRegOp_Result;
	PVOID					pObject;

	if (OutBuffer_Size < sizeof(REG_OP_RESULT))
		return STATUS_BUFFER_TOO_SMALL;

	*pRet_Size = 0;

	pRegOp_Result = (PREG_OP_RESULT)pOutBuffer;

	ntStatus = Check_RegPath(pRegOp_Request->m_KeyPath);
	if (!NT_SUCCESS(ntStatus))
		return ntStatus;

	RtlInitUnicodeString(&cntdKeyPath, pRegOp_Request->m_KeyPath);
	InitializeObjectAttributes(&Key_OA, &cntdKeyPath, OBJ_CASE_INSENSITIVE | OBJ_KERNEL_HANDLE,
		NULL, NULL);

	ntStatus = NtOpenKey(&hKey, KEY_ENUMERATE_SUB_KEYS, &Key_OA);
// here is \perflib\* workaround
	if (!(	NT_SUCCESS(ntStatus) && 
			NT_SUCCESS(ObReferenceObjectByHandle(
						hKey,
						0,
						NULL,
						KernelMode,
						&pObject,
						NULL
			))
	))
		return ntStatus;
	ObDereferenceObject(pObject);

	Key_Info_Len = sizeof(KEY_BASIC_INFORMATION)+MAX_KEYVALUENAME_LEN*sizeof(WCHAR);
	pKey_Info = (PKEY_BASIC_INFORMATION)ExAllocatePoolWithTag(PagedPool, Key_Info_Len, '0000');
	
	if (!pKey_Info)
	{
		ZwClose(hKey);
		return STATUS_UNSUCCESSFUL;
	}

	ntStatus = ZwEnumerateKey(
		hKey,
		pRegOp_Request->m_EnumIndex,
		KeyBasicInformation,
		pKey_Info,
		Key_Info_Len,
		&Result_Len
		);
	
	if (!NT_SUCCESS(ntStatus))
	{
		ZwClose(hKey);
		ExFreePool(pKey_Info);

		if (ntStatus == STATUS_NO_MORE_ENTRIES)
		{
			pRegOp_Result->m_Status = RegOp_StatusNoMoreEntries;
			return STATUS_SUCCESS;
		}

		return ntStatus;
	}

	RegOpBuffer_Len = sizeof(REG_ENUM_RESULT)+pKey_Info->NameLength+sizeof(WCHAR);
	if (OutBuffer_Size < sizeof(REG_OP_RESULT)+RegOpBuffer_Len)
	{
		pRegOp_Result->m_Status = RegOp_StatusBufferTooSmall;
		pRegOp_Result->m_BufferLen = RegOpBuffer_Len;
		ZwClose(hKey);
		ExFreePool(pKey_Info);
		
		return STATUS_SUCCESS;
	}

	pRegOp_Result->m_Status = RegOp_StatusSuccess;
	pRegOp_Result->m_BufferLen = RegOpBuffer_Len;

	memcpy(((PREG_ENUM_RESULT)pRegOp_Result->m_Buffer)->m_Result, pKey_Info->Name,
		pKey_Info->NameLength);
	((PREG_ENUM_RESULT)pRegOp_Result->m_Buffer)->m_Result[pKey_Info->NameLength/sizeof(WCHAR)] = 0;

	*pRet_Size = sizeof(REG_OP_RESULT)+pRegOp_Result->m_BufferLen;

	ExFreePool(pKey_Info);
	ZwClose(hKey);

	return STATUS_SUCCESS;
}

NTSTATUS
Reg_CheckKeyPresent (
	IN PREG_OP_REQUEST pRegOp_Request,
	OUT PVOID pOutBuffer,
	IN ULONG OutBuffer_Size,
	OUT ULONG_PTR *pRet_Size )
{
	NTSTATUS				ntStatus;
	UNICODE_STRING			cntdKeyPath;
	OBJECT_ATTRIBUTES		Key_OA;
	HANDLE					hKey;
	PREG_OP_RESULT			pRegOp_Result;
	PVOID					pObject;

	if (OutBuffer_Size < sizeof(REG_OP_RESULT))
		return STATUS_BUFFER_TOO_SMALL;

	*pRet_Size = 0;

	pRegOp_Result = (PREG_OP_RESULT)pOutBuffer;

	ntStatus = Check_RegPath(pRegOp_Request->m_KeyPath);
	if (!NT_SUCCESS(ntStatus))
		return ntStatus;

	RtlInitUnicodeString(&cntdKeyPath, pRegOp_Request->m_KeyPath);
	InitializeObjectAttributes(&Key_OA, &cntdKeyPath, OBJ_CASE_INSENSITIVE | OBJ_KERNEL_HANDLE,
		NULL, NULL);
	
	ntStatus = ZwOpenKey(&hKey, KEY_ENUMERATE_SUB_KEYS, &Key_OA);

	pRegOp_Result->m_Status = RegOp_StatusNotFound;
	pRegOp_Result->m_BufferLen = 0;
	
// here is \perflib\* workaround
	if (	NT_SUCCESS(ntStatus) && 
			NT_SUCCESS(ObReferenceObjectByHandle(
						hKey,
						0,
						NULL,
						KernelMode,
						&pObject,
						NULL
			))
	)
	{
		ObDereferenceObject(pObject);

		pRegOp_Result->m_Status = RegOp_StatusSuccess;
		ZwClose(hKey);	
	}	

	*pRet_Size = sizeof(REG_OP_RESULT);

	return STATUS_SUCCESS;
}

NTSTATUS
Reg_EnumValue (
	IN PREG_OP_REQUEST pRegOp_Request,
	OUT PVOID pOutBuffer,
	IN ULONG OutBuffer_Size,
	OUT ULONG_PTR *pRet_Size )
{
	NTSTATUS						ntStatus;
	UNICODE_STRING					cntdKeyPath;
	OBJECT_ATTRIBUTES				Key_OA;
	HANDLE							hKey;
	ULONG							Value_Info_Len,
									RegOpBuffer_Len,
									Result_Len;
	PKEY_VALUE_BASIC_INFORMATION	pValue_Info;
	PREG_OP_RESULT					pRegOp_Result;
	PVOID							pObject;

	if (OutBuffer_Size < sizeof(REG_OP_RESULT))
		return STATUS_BUFFER_TOO_SMALL;

	*pRet_Size = 0;

	pRegOp_Result = (PREG_OP_RESULT)pOutBuffer;

	ntStatus = Check_RegPath(pRegOp_Request->m_KeyPath);
	if (!NT_SUCCESS(ntStatus))
		return ntStatus;

	RtlInitUnicodeString(&cntdKeyPath, pRegOp_Request->m_KeyPath);
	InitializeObjectAttributes(&Key_OA, &cntdKeyPath, OBJ_CASE_INSENSITIVE | OBJ_KERNEL_HANDLE,
		NULL, NULL);

	ntStatus = ZwOpenKey(&hKey, KEY_ENUMERATE_SUB_KEYS, &Key_OA);
// here is \perflib\* workaround
	if (!(	NT_SUCCESS(ntStatus) && 
			NT_SUCCESS(ObReferenceObjectByHandle(
						hKey,
						0,
						NULL,
						KernelMode,
						&pObject,
						NULL
			))
	))
		return ntStatus;
	ObDereferenceObject(pObject);

	Value_Info_Len = sizeof(KEY_VALUE_BASIC_INFORMATION)+MAX_KEYVALUENAME_LEN*sizeof(WCHAR);
	pValue_Info = (PKEY_VALUE_BASIC_INFORMATION)ExAllocatePoolWithTag(PagedPool, Value_Info_Len, '0000');
	
	if (!pValue_Info)
	{
		ZwClose(hKey);
		return STATUS_UNSUCCESSFUL;
	}

	ntStatus = ZwEnumerateValueKey(
		hKey,
		pRegOp_Request->m_EnumIndex,
		KeyValueBasicInformation,
		pValue_Info,
		Value_Info_Len,
		&Result_Len
		);
	
	if (!NT_SUCCESS(ntStatus))
	{
		ZwClose(hKey);
		ExFreePool(pValue_Info);

		if (ntStatus == STATUS_NO_MORE_ENTRIES)
		{
			pRegOp_Result->m_Status = RegOp_StatusNoMoreEntries;
			*pRet_Size = sizeof(REG_OP_RESULT);
			return STATUS_SUCCESS;
		}

		return ntStatus;
	}

	RegOpBuffer_Len = sizeof(REG_ENUM_RESULT)+pValue_Info->NameLength+sizeof(WCHAR);
	if (OutBuffer_Size < sizeof(REG_OP_RESULT)+RegOpBuffer_Len)
	{
		pRegOp_Result->m_Status = RegOp_StatusBufferTooSmall;
		pRegOp_Result->m_BufferLen = RegOpBuffer_Len;
		*pRet_Size = sizeof(REG_OP_RESULT);

		ZwClose(hKey);
		ExFreePool(pValue_Info);
		
		return STATUS_SUCCESS;
	}

	pRegOp_Result->m_Status = RegOp_StatusSuccess;
	pRegOp_Result->m_BufferLen = RegOpBuffer_Len;

	memcpy(((PREG_ENUM_RESULT)pRegOp_Result->m_Buffer)->m_Result, pValue_Info->Name,
		pValue_Info->NameLength);
	((PREG_ENUM_RESULT)pRegOp_Result->m_Buffer)->m_Result[pValue_Info->NameLength/sizeof(WCHAR)] = 0;

	*pRet_Size = sizeof(REG_OP_RESULT)+pRegOp_Result->m_BufferLen;

	ExFreePool(pValue_Info);
	ZwClose(hKey);

	return STATUS_SUCCESS;
}

NTSTATUS
Reg_CheckValuePresent (
	IN PREG_OP_REQUEST pRegOp_Request,
	OUT PVOID pOutBuffer, 
	IN ULONG OutBuffer_Size,
	OUT ULONG_PTR *pRet_Size )
{
	NTSTATUS						ntStatus;
	UNICODE_STRING					cntdKeyPath,
									cntdValueName;
	OBJECT_ATTRIBUTES				Key_OA;
	HANDLE							hKey;
	ULONG							BufferLen,
									Dummy;
	PREG_OP_RESULT					pRegOp_Result;
	PVOID							pObject;

	if (OutBuffer_Size < sizeof(REG_OP_RESULT))
		return STATUS_BUFFER_TOO_SMALL;

	*pRet_Size = 0;

	pRegOp_Result = (PREG_OP_RESULT)pOutBuffer;

	ntStatus = Check_RegPath(pRegOp_Request->m_KeyPath);
	if (!NT_SUCCESS(ntStatus))
		return ntStatus;

	ntStatus = Check_ValueName(pRegOp_Request->m_ValueName);
	if (!NT_SUCCESS(ntStatus))
		return ntStatus;

	RtlInitUnicodeString(&cntdKeyPath, pRegOp_Request->m_KeyPath);
	InitializeObjectAttributes(&Key_OA, &cntdKeyPath, OBJ_CASE_INSENSITIVE | OBJ_KERNEL_HANDLE,
		NULL, NULL);

	ntStatus = ZwOpenKey(&hKey, KEY_ENUMERATE_SUB_KEYS, &Key_OA);
// here is \perflib\* workaround
	if (!(	NT_SUCCESS(ntStatus) && 
			NT_SUCCESS(ObReferenceObjectByHandle(
						hKey,
						0,
						NULL,
						KernelMode,
						&pObject,
						NULL
			))
	))
		return ntStatus;
	ObDereferenceObject(pObject);

	RtlInitUnicodeString(&cntdValueName, pRegOp_Request->m_ValueName);

// check whether the given value is present
	ntStatus = ZwQueryValueKey(
		hKey,
		&cntdValueName,
		KeyValuePartialInformation,
		&Dummy,
		0,
		&BufferLen
		);

	pRegOp_Result->m_Status = RegOp_StatusNotFound;
	pRegOp_Result->m_BufferLen = 0;
	if (NT_SUCCESS(ntStatus) || ntStatus == STATUS_BUFFER_OVERFLOW ||
		ntStatus == STATUS_BUFFER_TOO_SMALL)
		pRegOp_Result->m_Status = RegOp_StatusSuccess;

	ZwClose(hKey);

	*pRet_Size = sizeof(REG_OP_RESULT);

	return STATUS_SUCCESS;
}

NTSTATUS
Reg_QueryValue (
	IN PREG_OP_REQUEST pRegOp_Request,
	OUT PVOID pOutBuffer,
	IN ULONG OutBuffer_Size,
	OUT ULONG_PTR *pRet_Size )
{
	NTSTATUS						ntStatus;
	UNICODE_STRING					cntdKeyPath,
									cntdValueName;
	OBJECT_ATTRIBUTES				Key_OA;
	HANDLE							hKey;
	ULONG							BufferLen,
									RegOpBuffer_Len,
									Dummy;
	PKEY_VALUE_PARTIAL_INFORMATION	pValueInfo;
	PREG_OP_RESULT					pRegOp_Result;
	PREG_QUERY_RESULT				pRegQuery_Result;
	PVOID							pObject;

	if (OutBuffer_Size < sizeof(REG_OP_RESULT))
		return STATUS_BUFFER_TOO_SMALL;

	*pRet_Size = 0;

	pRegOp_Result = (PREG_OP_RESULT)pOutBuffer;

	ntStatus = Check_RegPath(pRegOp_Request->m_KeyPath);
	if (!NT_SUCCESS(ntStatus))
		return ntStatus;

	ntStatus = Check_ValueName(pRegOp_Request->m_ValueName);
	if (!NT_SUCCESS(ntStatus))
		return ntStatus;

	RtlInitUnicodeString(&cntdKeyPath, pRegOp_Request->m_KeyPath);
	InitializeObjectAttributes(&Key_OA, &cntdKeyPath, OBJ_CASE_INSENSITIVE | OBJ_KERNEL_HANDLE,
		NULL, NULL);

	ntStatus = NtOpenKey(&hKey, KEY_ENUMERATE_SUB_KEYS | KEY_QUERY_VALUE, &Key_OA);		// ***
// here is \perflib\* workaround
	if (!(	NT_SUCCESS(ntStatus) && 
			NT_SUCCESS(ObReferenceObjectByHandle(
						hKey,
						0,
						NULL,
						KernelMode,
						&pObject,
						NULL
			))
	))
		return ntStatus;
	ObDereferenceObject(pObject);

	RtlInitUnicodeString(&cntdValueName, pRegOp_Request->m_ValueName);

// get buffer size, allocate buffer
	ntStatus = NtQueryValueKey(		// ***
		hKey,
		&cntdValueName,
		KeyValuePartialInformation,
		&Dummy,
		sizeof(Dummy),
		&BufferLen
		);
	
	if (!(NT_SUCCESS(ntStatus) || ntStatus == STATUS_BUFFER_OVERFLOW ||
		ntStatus == STATUS_BUFFER_TOO_SMALL))
	{
		ZwClose(hKey);
		return ntStatus;
	}
	pValueInfo = (PKEY_VALUE_PARTIAL_INFORMATION)ExAllocatePoolWithTag(PagedPool, BufferLen, '0000');
	if (!pValueInfo)
	{
		ZwClose(hKey);
		return STATUS_UNSUCCESSFUL;
	}

// query value data
	ntStatus = NtQueryValueKey(		// ***
		hKey,
		&cntdValueName,
		KeyValuePartialInformation,
		pValueInfo,
		BufferLen,
		&BufferLen
		);
	
// *** - changed Zw to Nt to set KePreviousMode == UserMode

	if (!NT_SUCCESS(ntStatus))
	{
		ZwClose(hKey);
		ExFreePool(pValueInfo);
		return ntStatus;
	}

	RegOpBuffer_Len = sizeof(REG_QUERY_RESULT)+pValueInfo->DataLength;
	if (OutBuffer_Size < sizeof(REG_OP_RESULT)+RegOpBuffer_Len)
	{
		pRegOp_Result->m_Status = RegOp_StatusBufferTooSmall;
		pRegOp_Result->m_BufferLen = RegOpBuffer_Len;
		*pRet_Size = sizeof(REG_OP_RESULT);

		ZwClose(hKey);
		ExFreePool(pValueInfo);
		return STATUS_SUCCESS;
	}

	pRegOp_Result->m_Status = RegOp_StatusSuccess;
	pRegOp_Result->m_BufferLen = RegOpBuffer_Len;

	pRegQuery_Result = (PREG_QUERY_RESULT)pRegOp_Result->m_Buffer;

	pRegQuery_Result->m_Type = pValueInfo->Type;
	pRegQuery_Result->m_ResultLen = pValueInfo->DataLength;
	memcpy(pRegQuery_Result->m_Result, pValueInfo->Data, pValueInfo->DataLength);

	*pRet_Size = sizeof(REG_OP_RESULT)+pRegOp_Result->m_BufferLen;

	ExFreePool(pValueInfo);
	ZwClose(hKey);

	return STATUS_SUCCESS;
}

NTSTATUS
Reg_SetValue (
	IN PREG_OP_REQUEST pRegOp_Request,
	OUT PVOID pOutBuffer,
	IN ULONG OutBuffer_Size,
	OUT ULONG_PTR *pRet_Size )
{
	NTSTATUS						ntStatus;
	UNICODE_STRING					cntdKeyPath,
									cntdValueName;
	OBJECT_ATTRIBUTES				Key_OA;
	HANDLE							hKey;
	PREG_OP_RESULT					pRegOp_Result;
	PVOID							pObject;

	*pRet_Size = 0;

	if (OutBuffer_Size < sizeof(REG_OP_RESULT))
		return STATUS_BUFFER_TOO_SMALL;

	pRegOp_Result = (PREG_OP_RESULT)pOutBuffer;

	ntStatus = Check_RegPath(pRegOp_Request->m_KeyPath);
	if (!NT_SUCCESS(ntStatus))
		return ntStatus;
	
	ntStatus = Check_ValueName(pRegOp_Request->m_ValueName);
	if (!NT_SUCCESS(ntStatus))
		return ntStatus;

	RtlInitUnicodeString(&cntdKeyPath, pRegOp_Request->m_KeyPath);
	InitializeObjectAttributes(&Key_OA, &cntdKeyPath, OBJ_CASE_INSENSITIVE | OBJ_KERNEL_HANDLE,
		NULL, NULL);
	
	ntStatus = ZwOpenKey(&hKey, KEY_SET_VALUE, &Key_OA);
// here is \perflib\* workaround
	if (!(	NT_SUCCESS(ntStatus) && 
			NT_SUCCESS(ObReferenceObjectByHandle(
						hKey,
						0,
						NULL,
						KernelMode,
						&pObject,
						NULL
			))
	))
		return ntStatus;
	ObDereferenceObject(pObject);

	RtlInitUnicodeString(&cntdValueName, pRegOp_Request->m_ValueName);

// set value data
	ntStatus = ZwSetValueKey(
		hKey,
		&cntdValueName,
		0,
		pRegOp_Request->m_Type,
		pRegOp_Request->m_ValueData,
		pRegOp_Request->m_ValueSize
		);
	if (!NT_SUCCESS(ntStatus))
	{
		ZwClose(hKey);
		return ntStatus;
	}

	pRegOp_Result->m_Status = RegOp_StatusSuccess;
	pRegOp_Result->m_BufferLen = 0;

	*pRet_Size = sizeof(REG_OP_RESULT);

	ZwClose(hKey);

	return STATUS_SUCCESS;
}

NTSTATUS
Reg_DeleteValue (
	IN PREG_OP_REQUEST pRegOp_Request,
	OUT PVOID pOutBuffer,
	IN ULONG OutBuffer_Size,
	OUT ULONG_PTR *pRet_Size )
{
	NTSTATUS			ntStatus = STATUS_SUCCESS;
	UNICODE_STRING		cntdKeyPath,
						cntdValueName;
	OBJECT_ATTRIBUTES	Key_OA;
	HANDLE hKey;
	PVOID pObject;

	if (OutBuffer_Size < sizeof(REG_OP_RESULT))
		return STATUS_BUFFER_TOO_SMALL;

	*pRet_Size = 0;

	ntStatus = Check_RegPath(pRegOp_Request->m_KeyPath);
	if (!NT_SUCCESS(ntStatus))
		return ntStatus;

	ntStatus = Check_ValueName(pRegOp_Request->m_ValueName);
	if (!NT_SUCCESS(ntStatus))
		return ntStatus;

	RtlInitUnicodeString(&cntdKeyPath, pRegOp_Request->m_KeyPath);
	InitializeObjectAttributes(&Key_OA, &cntdKeyPath, OBJ_CASE_INSENSITIVE | OBJ_KERNEL_HANDLE,
		NULL, NULL);
	
	ntStatus = ZwOpenKey(&hKey, KEY_SET_VALUE, &Key_OA);
// here is \perflib\* workaround
	if (!(	NT_SUCCESS(ntStatus) && 
			NT_SUCCESS(ObReferenceObjectByHandle(
						hKey,
						0,
						NULL,
						KernelMode,
						&pObject,
						NULL
			))
	))
		return ntStatus;
	ObDereferenceObject(pObject);

	RtlInitUnicodeString(&cntdValueName, pRegOp_Request->m_ValueName);
	ntStatus = ZwDeleteValueKey(hKey, &cntdValueName);
	ZwClose(hKey);
	
	if (!NT_SUCCESS(ntStatus))
		return ntStatus;

	((PREG_OP_RESULT)pOutBuffer)->m_Status = RegOp_StatusSuccess;
	((PREG_OP_RESULT)pOutBuffer)->m_BufferLen = 0;
	*pRet_Size = sizeof(REG_OP_RESULT);

	return STATUS_SUCCESS;
}

NTSTATUS
Reg_QueryInfoKey (
	IN PREG_OP_REQUEST pRegOp_Request,
	OUT PVOID pOutBuffer,
	IN ULONG OutBuffer_Size,
	OUT ULONG_PTR *pRet_Size )
{
	NTSTATUS						ntStatus;
	UNICODE_STRING					cntdKeyPath;
	OBJECT_ATTRIBUTES				Key_OA;
	HANDLE							hKey;
	ULONG							BufferLen,
									RegOpBuffer_Len,
									Dummy;
	PKEY_FULL_INFORMATION			pKeyInfo;
	PREG_OP_RESULT					pRegOp_Result;
	PREG_QUERY_KEY_RESULT			pRegQueryKey_Result;
	PVOID							pObject;

	if (OutBuffer_Size < sizeof(REG_OP_RESULT))
		return STATUS_BUFFER_TOO_SMALL;

	*pRet_Size = 0;

	pRegOp_Result = (PREG_OP_RESULT)pOutBuffer;

	ntStatus = Check_RegPath(pRegOp_Request->m_KeyPath);
	if (!NT_SUCCESS(ntStatus))
		return ntStatus;

	RtlInitUnicodeString(&cntdKeyPath, pRegOp_Request->m_KeyPath);
	InitializeObjectAttributes(&Key_OA, &cntdKeyPath, OBJ_CASE_INSENSITIVE | OBJ_KERNEL_HANDLE,
		NULL, NULL);

	ntStatus = ZwOpenKey(&hKey, KEY_QUERY_VALUE, &Key_OA);
// here is \perflib\* workaround
	if (!(	NT_SUCCESS(ntStatus) && 
			NT_SUCCESS(ObReferenceObjectByHandle(
						hKey,
						0,
						NULL,
						KernelMode,
						&pObject,
						NULL
			))
	))
		return ntStatus;
	ObDereferenceObject(pObject);

// get buffer size, allocate buffer
	ntStatus = ZwQueryKey(
		hKey,
		KeyFullInformation,
		&Dummy,
		0,
		&BufferLen
		);
	if (!(NT_SUCCESS(ntStatus) || ntStatus == STATUS_BUFFER_OVERFLOW ||
		ntStatus == STATUS_BUFFER_TOO_SMALL))
	{
		ZwClose(hKey);
		return ntStatus;
	}
	pKeyInfo = (PKEY_FULL_INFORMATION)ExAllocatePoolWithTag(PagedPool, BufferLen, '0000');
	if (!pKeyInfo)
	{
		ZwClose(hKey);
		return STATUS_UNSUCCESSFUL;
	}

// query key info
	ntStatus = ZwQueryKey(
		hKey,
		KeyFullInformation,
		pKeyInfo,
		BufferLen,
		&BufferLen
		);
	if (!NT_SUCCESS(ntStatus))
	{
		ZwClose(hKey);
		ExFreePool(pKeyInfo);
		return ntStatus;
	}

	RegOpBuffer_Len = sizeof(REG_QUERY_KEY_RESULT)+pKeyInfo->ClassLength+sizeof(WCHAR);
	if (OutBuffer_Size < sizeof(REG_OP_RESULT)+RegOpBuffer_Len)
	{
		pRegOp_Result->m_Status = RegOp_StatusBufferTooSmall;
		pRegOp_Result->m_BufferLen = RegOpBuffer_Len;
		*pRet_Size = sizeof(REG_OP_RESULT);

		ZwClose(hKey);
		ExFreePool(pKeyInfo);
		return STATUS_SUCCESS;
	}

	pRegOp_Result->m_Status = RegOp_StatusSuccess;
	pRegOp_Result->m_BufferLen = RegOpBuffer_Len;

	pRegQueryKey_Result = (PREG_QUERY_KEY_RESULT)pRegOp_Result->m_Buffer;

	pRegQueryKey_Result->m_LastWriteTime = pKeyInfo->LastWriteTime;
	pRegQueryKey_Result->m_ClassLen = pKeyInfo->ClassLength;
	pRegQueryKey_Result->m_NumSubKeys = pKeyInfo->SubKeys;
	pRegQueryKey_Result->m_MaxKeyNameLen = pKeyInfo->MaxNameLen;
	pRegQueryKey_Result->m_MaxClassLen = pKeyInfo->MaxClassLen; 
	pRegQueryKey_Result->m_NumValues = pKeyInfo->Values;
	pRegQueryKey_Result->m_MaxValueNameLen = pKeyInfo->MaxValueNameLen;
	pRegQueryKey_Result->m_MaxValueDataLen = pKeyInfo->MaxValueDataLen;
	memcpy(pRegQueryKey_Result->m_Class, pKeyInfo->Class, pKeyInfo->ClassLength);
	pRegQueryKey_Result->m_Class[pKeyInfo->ClassLength/sizeof(WCHAR)] = 0;

	*pRet_Size = sizeof(REG_OP_RESULT)+pRegOp_Result->m_BufferLen;

	ExFreePool(pKeyInfo);
	ZwClose(hKey);

	return STATUS_SUCCESS;
}

BOOLEAN InitRegOps()
{
	UNICODE_STRING usFuncName;

	RtlInitUnicodeString( &usFuncName, L"NtOpenKey" );
	NtOpenKey = (t_fNtOpenKey) MmGetSystemRoutineAddress( &usFuncName );
	RtlInitUnicodeString( &usFuncName, L"NtQueryValueKey" );
	NtQueryValueKey = (t_fNtQueryValueKey) MmGetSystemRoutineAddress( &usFuncName );

	return ( NtOpenKey && NtQueryValueKey );
}

NTSTATUS DoRegsRequest (
	PEXTERNAL_DRV_REQUEST pInRequest,
	ULONG InRequestSize,
	PVOID pOutBuffer, 
	ULONG OutBufferSize,
	PULONG pRetSize
	)
{
	NTSTATUS status = STATUS_NOT_SUPPORTED;
	PREG_OP_REQUEST pRegOp_Request;

	ULONG_PTR uRetSize = 0;

	PEXTERNAL_DRV_REQUEST pRequest = NULL;

	if ( g_bFail )
		return STATUS_UNSUCCESSFUL;

	if ( !g_bInited )
	{
		BOOLEAN bResult = InitRegOps();
		if ( bResult )
			g_bInited = TRUE;
		else
		{
			g_bFail = TRUE;
			return STATUS_UNSUCCESSFUL;
		}
	}

	if (OutBufferSize < sizeof(REG_OP_RESULT))
		return STATUS_INVALID_PARAMETER;

	pRequest = ExAllocatePoolWithTag( PagedPool, InRequestSize, 'EBos' );
	if (!pRequest)
		return STATUS_NO_MEMORY;

	try {
		RtlCopyMemory( pRequest, pInRequest, InRequestSize );
	} except( CheckException() ) {
		_dbg_break_;
		
		ExFreePool(pRequest);

		return STATUS_INVALID_PARAMETER;
	}

	if (pRequest->m_BufferSize < sizeof(REG_OP_REQUEST))
	{
		ExFreePool(pRequest);
		return STATUS_INVALID_PARAMETER;
	}

	pRegOp_Request = (PREG_OP_REQUEST)pRequest->m_Buffer;
	
	switch (pRequest->m_IOCTL)
	{
	case _AVPG_IOCTL_REG_CREATEKEY:
		status = Reg_CreateKey(pRegOp_Request, pOutBuffer, OutBufferSize, &uRetSize);
		break;

	case _AVPG_IOCTL_REG_DELETEKEY:
		status = Reg_DeleteKey(pRegOp_Request, pOutBuffer, OutBufferSize, &uRetSize);
		break;

	case _AVPG_IOCTL_REG_ENUMKEY:
		status = Reg_EnumKey(pRegOp_Request, pOutBuffer, OutBufferSize, &uRetSize);
		break;

	case _AVPG_IOCTL_REG_CHECKKEYPRESENT:
		status = Reg_CheckKeyPresent(pRegOp_Request, pOutBuffer, OutBufferSize, &uRetSize);
		break;

	case _AVPG_IOCTL_REG_ENUMVALUE:
		status = Reg_EnumValue(pRegOp_Request, pOutBuffer, OutBufferSize, &uRetSize);
		break;

	case _AVPG_IOCTL_REG_CHECKVALUEPRESENT:
		status = Reg_CheckValuePresent(pRegOp_Request, pOutBuffer, OutBufferSize, &uRetSize);
		break;

	case _AVPG_IOCTL_REG_QUERYVALUE:
		status = Reg_QueryValue(pRegOp_Request, pOutBuffer, OutBufferSize, &uRetSize);
		break;

	case _AVPG_IOCTL_REG_SETVALUE:
		status = Reg_SetValue(pRegOp_Request, pOutBuffer, OutBufferSize, &uRetSize);
		break;

	case _AVPG_IOCTL_REG_DELETEVALUE:
		status = Reg_DeleteValue(pRegOp_Request, pOutBuffer, OutBufferSize, &uRetSize);
		break;

	case _AVPG_IOCTL_REG_QUERYINFOKEY:
		status = Reg_QueryInfoKey(pRegOp_Request, pOutBuffer, OutBufferSize, &uRetSize);
		break;
	}

	ExFreePool(pRequest);

	*pRetSize = (ULONG) uRetSize;

	return status;
}
