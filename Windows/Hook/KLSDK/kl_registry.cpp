#include "kl_registry.h"


#define MAX_KEYVALUENAME_LEN	1024


#ifndef ISWIN9X
//  ----------------------------------------------
//	-------   WinNT Section ----------------------
//  ----------------------------------------------
CKl_Reg::CKl_Reg()
{
	m_hKey = NULL;
}

CKl_Reg::~CKl_Reg()
{
	if (m_hKey)
	{
		ExFreePool(m_Base_KeyPath);
		ZwClose(m_hKey);
	}
}

KLSTATUS CKl_Reg::OpenKey(PWCHAR KeyPath)
{
	if (m_hKey)
		return KL_ALREADYOPENED;

	ULONG KeyPath_Len = (ULONG)wcslen(KeyPath);
	if (KeyPath_Len+2 > MAX_KEYVALUENAME_LEN)
		return KL_PATHNOTFOUND;

	UNICODE_STRING cntdKeyPath;
	RtlInitUnicodeString(&cntdKeyPath, KeyPath);
	OBJECT_ATTRIBUTES Key_OA;
	InitializeObjectAttributes(	&Key_OA, &cntdKeyPath,
		OBJ_CASE_INSENSITIVE | OBJ_KERNEL_HANDLE, NULL, NULL);

	HANDLE hKey;
	if (!NT_SUCCESS(ZwOpenKey(&hKey, KEY_READ | KEY_WRITE | DELETE, &Key_OA)))
		return KL_PATHNOTFOUND;

	m_Base_KeyPath = (PWCHAR)ExAllocatePoolWithTag(PagedPool, (KeyPath_Len+2)*sizeof(WCHAR), '0000');
	if (!m_Base_KeyPath)
	{
		ZwClose(hKey);
		return KL_UNSUCCESSFUL;
	}
	wcscpy(m_Base_KeyPath, KeyPath);
	wcscat(m_Base_KeyPath, L"\\");

	m_hKey = hKey;

	return KL_SUCCESS;
}

void CKl_Reg::CloseKey()
{
	if (m_hKey)
	{
		ExFreePool(m_Base_KeyPath);
		ZwClose(m_hKey);
	}

	m_hKey = NULL;
}

KLSTATUS CKl_Reg::Delete()
{
    if ( m_hKey )
    {
        NTSTATUS ntStatus = ZwDeleteKey( m_hKey );
        return ( ntStatus == STATUS_SUCCESS ) ? KL_SUCCESS : KL_UNSUCCESSFUL;
    }
    return KL_UNSUCCESSFUL;
}

KLSTATUS CKl_Reg::CreateSubKey(PWCHAR SubKey)
{
	if (!m_hKey)
		return KL_NOTOPENED;

	ULONG Full_PathLen = (ULONG)(wcslen(m_Base_KeyPath)+wcslen(SubKey));
	if (Full_PathLen+1 > MAX_KEYVALUENAME_LEN)
		return KL_UNSUCCESSFUL;
	PWCHAR Full_KeyPath = (PWCHAR)ExAllocatePoolWithTag(PagedPool, (Full_PathLen+1)*sizeof(WCHAR), '0000');
	if (!Full_KeyPath)
		return KL_UNSUCCESSFUL;
	wcscpy(Full_KeyPath, m_Base_KeyPath);
	wcscat(Full_KeyPath, SubKey);

	UNICODE_STRING cntdFull_KeyPath;
	RtlInitUnicodeString(&cntdFull_KeyPath, Full_KeyPath);
	OBJECT_ATTRIBUTES Key_OA;
	InitializeObjectAttributes(&Key_OA, &cntdFull_KeyPath,
		OBJ_CASE_INSENSITIVE | OBJ_KERNEL_HANDLE, NULL, NULL);

	HANDLE hCreatedKey = m_hKey;
	if (!NT_SUCCESS(ZwCreateKey(&hCreatedKey, 0, &Key_OA, 0, NULL, REG_OPTION_NON_VOLATILE, NULL)))
	{
		ExFreePool(Full_KeyPath);
		return KL_UNSUCCESSFUL;
	}

	ExFreePool(Full_KeyPath);
	ZwClose(hCreatedKey);

	return KL_SUCCESS;
}

KLSTATUS CKl_Reg::QueryValue(PWCHAR ValueName, PULONG pType, PVOID Data, PULONG pDataSize)
{
	if (!m_hKey)
		return KL_NOTOPENED;

	UNICODE_STRING cntdValueName;
	RtlInitUnicodeString(&cntdValueName, ValueName);

	ULONG Dummy;
	ULONG BufferLen;
	NTSTATUS ntStatus = ZwQueryValueKey(
		m_hKey,
		&cntdValueName,
		KeyValuePartialInformation,
		&Dummy,
		0,
		&BufferLen
		);
	if (!(NT_SUCCESS(ntStatus) || ntStatus == STATUS_BUFFER_OVERFLOW ||
		ntStatus == STATUS_BUFFER_TOO_SMALL))
		return KL_UNSUCCESSFUL;


	PKEY_VALUE_PARTIAL_INFORMATION pValueInfo = 
		(PKEY_VALUE_PARTIAL_INFORMATION)ExAllocatePoolWithTag(PagedPool, BufferLen, '0000');
	if (!pValueInfo)
		return KL_UNSUCCESSFUL;

	ULONG RetSize;
	if (!NT_SUCCESS(ZwQueryValueKey(
		m_hKey,
		&cntdValueName,
		KeyValuePartialInformation,
		pValueInfo,
		BufferLen,
		&RetSize
		)))
	{
		ExFreePool(pValueInfo);
		return KL_UNSUCCESSFUL;
	}
	
	ULONG Provided_BufferLen = *pDataSize;

	*pDataSize = pValueInfo->DataLength;
	if (Provided_BufferLen < pValueInfo->DataLength)
	{
		ExFreePool(pValueInfo);
		return KL_BUFFER_TOO_SMALL;
	}

	*pType = pValueInfo->Type;
	memcpy(Data, pValueInfo->Data, pValueInfo->DataLength);

	ExFreePool(pValueInfo);
	
	return KL_SUCCESS;
}

KLSTATUS CKl_Reg::SetValue(PWCHAR ValueName, ULONG Type, PVOID Data, ULONG DataSize)
{
	if (!m_hKey)
		return KL_NOTOPENED;

	UNICODE_STRING cntdValueName;
	RtlInitUnicodeString(&cntdValueName, ValueName);

	if (!NT_SUCCESS(ZwSetValueKey(
		m_hKey,
		&cntdValueName,
		0,
		Type,
		Data,
		DataSize
		)))
		return KL_UNSUCCESSFUL;

	return KL_SUCCESS;
}


KLSTATUS CKl_Reg::EnumValues(ULONG EnumIndex, PWCHAR ValueName, PULONG pValueName_Len/* in bytes*/)
{
	if (!m_hKey)
		return KL_NOTOPENED;

	ULONG ValueInfo_Len = sizeof(KEY_VALUE_BASIC_INFORMATION)+MAX_KEYVALUENAME_LEN*sizeof(WCHAR);
	PKEY_VALUE_BASIC_INFORMATION pValueInfo = 
		(PKEY_VALUE_BASIC_INFORMATION)ExAllocatePoolWithTag(PagedPool, ValueInfo_Len, '0000');
	if (!pValueInfo)
		return KL_UNSUCCESSFUL;


	ULONG ResultLen;
	NTSTATUS ntStatus = ZwEnumerateValueKey(
		m_hKey,
		EnumIndex,
		KeyValueBasicInformation,
		pValueInfo,
		ValueInfo_Len,
		&ResultLen
		);
	if (!NT_SUCCESS(ntStatus))
	{
		ExFreePool(pValueInfo);

		if (ntStatus == STATUS_NO_MORE_ENTRIES)
			return KL_NOMOREENTRIES;
		return KL_UNSUCCESSFUL;
	}

	ULONG Provided_BufferLen = *pValueName_Len;
	*pValueName_Len = pValueInfo->NameLength+sizeof(WCHAR);
	if (Provided_BufferLen < pValueInfo->NameLength+sizeof(WCHAR))
	{
		ExFreePool(pValueInfo);
		return KL_BUFFER_TOO_SMALL;
	}

	memcpy(ValueName, pValueInfo->Name, pValueInfo->NameLength);
	ValueName[pValueInfo->NameLength/sizeof(WCHAR)] = 0;

	ExFreePool(pValueInfo);

	return KL_SUCCESS;
}

#else
//  ----------------------------------------------
//	-------   Win9x Section ----------------------
//  ----------------------------------------------

#define REG_MACHINE_PATH	"\\registry\\machine\\"
#define REG_USER_PATH		"\\registry\\user\\"


CKl_Reg::CKl_Reg()
{
	m_hKey = NULL;
}

CKl_Reg::~CKl_Reg()
{
	if (m_hKey)
	{
		HeapFree(m_ansiBase_KeyPath, 0);
		RegCloseKey(m_hKey);
	}
}

KLSTATUS CKl_Reg::OpenKey(PWCHAR uniKeyPath)
{
	if (m_hKey)
		return KL_ALREADYOPENED;

	ULONG KeyPath_Len = wcslen(uniKeyPath);
	if (KeyPath_Len+2 > MAX_KEYVALUENAME_LEN)
		return KL_PATHNOTFOUND;

	PCHAR ansiKeyPath = (PCHAR)HeapAllocate(KeyPath_Len+2, 0);
	if (!ansiKeyPath)
		return KL_UNSUCCESSFUL;

	Unicode2Ansi(ansiKeyPath, uniKeyPath);

	
	HKEY hKeyRoot;
	PCHAR ansiSubKey_Path;
	if (KL_SUCCESS != QualifyRegRoot(ansiKeyPath, &ansiSubKey_Path, &hKeyRoot))
	{
		HeapFree(ansiKeyPath, 0);
		return KL_PATHNOTFOUND;
	}

	HKEY hOpenedKey;
	if (ERROR_SUCCESS != RegOpenKey(hKeyRoot, ansiSubKey_Path, &hOpenedKey))
	{
		HeapFree(ansiKeyPath, 0);
		return KL_PATHNOTFOUND;
	}
	
	m_ansiBase_KeyPath = (PCHAR)HeapAllocate(KeyPath_Len+1, 0);
	if (!m_ansiBase_KeyPath)
	{
		HeapFree(ansiKeyPath, 0);
		return KL_UNSUCCESSFUL;
	}
	m_hBase_KeyRoot = hKeyRoot;
	strcpy(m_ansiBase_KeyPath, ansiSubKey_Path);
	strcat(m_ansiBase_KeyPath, "\\");
	m_hKey = hOpenedKey;

	HeapFree(ansiKeyPath, 0);

	return KL_SUCCESS;
}

void CKl_Reg::CloseKey()
{
	if (m_hKey)
	{
		HeapFree(m_ansiBase_KeyPath, 0);
		RegCloseKey(m_hKey);
	}

	m_hKey = NULL;
}

KLSTATUS CKl_Reg::Delete()
{
    if ( m_hKey )
    {
        NTSTATUS ntStatus = RegDeleteKey( m_hBase_KeyRoot, m_ansiBase_KeyPath );
        return ( ntStatus == STATUS_SUCCESS ) ? KL_SUCCESS : KL_UNSUCCESSFUL;
    }

    return KL_UNSUCCESSFUL;
}

KLSTATUS CKl_Reg::CreateSubKey(PWCHAR uniSubKey)
{
	if (!m_hKey)
		return KL_NOTOPENED;

	ULONG SubKey_Len = wcslen(uniSubKey);
	ULONG Full_KeyPath_Len = strlen(m_ansiBase_KeyPath)+SubKey_Len;
	if (Full_KeyPath_Len+1 > MAX_KEYVALUENAME_LEN)
		return KL_UNSUCCESSFUL;

	PCHAR ansiSubKey = (PCHAR)HeapAllocate(SubKey_Len+1, 0);
	if (!ansiSubKey)
		return KL_UNSUCCESSFUL;
	Unicode2Ansi(ansiSubKey, uniSubKey);

	PCHAR ansiFull_KeyPath = (PCHAR)HeapAllocate(Full_KeyPath_Len+1, 0);
	if (!ansiFull_KeyPath)
	{
		HeapFree(ansiSubKey, 0);
		return KL_UNSUCCESSFUL;
	}
	strcpy(ansiFull_KeyPath, m_ansiBase_KeyPath);
	strcat(ansiFull_KeyPath, ansiSubKey);
	HeapFree(ansiSubKey, 0);

	HKEY hCreatedKey;
	if (ERROR_SUCCESS != RegCreateKey(m_hBase_KeyRoot, ansiFull_KeyPath, &hCreatedKey))
	{
		HeapFree(ansiFull_KeyPath, 0);
		return KL_UNSUCCESSFUL;
	}
	RegCloseKey(hCreatedKey);

	return KL_SUCCESS;
}

KLSTATUS CKl_Reg::QueryValue(PWCHAR uniValueName, PULONG pType, PVOID Data, PULONG pDataSize)
{
	if (!m_hKey)
		return KL_NOTOPENED;

	ULONG ValueName_Len = wcslen(uniValueName);
	if (ValueName_Len+1 > MAX_KEYVALUENAME_LEN)
		return KL_PATHNOTFOUND;

	PCHAR ansiValueName = (PCHAR)HeapAllocate(ValueName_Len+1, 0);
	if (!ansiValueName)
		return KL_UNSUCCESSFUL;
	Unicode2Ansi(ansiValueName, uniValueName);

	ULONG ErrorCode = RegQueryValueEx(m_hKey, ansiValueName, NULL, pType, (PBYTE)Data, pDataSize);
	if (ERROR_SUCCESS != ErrorCode)
	{
		HeapFree(ansiValueName, 0);

		if (ERROR_MORE_DATA == ErrorCode)
			return KL_BUFFER_TOO_SMALL;
		return KL_PATHNOTFOUND;
	}

	HeapFree(ansiValueName, 0);

	return KL_SUCCESS;
}

KLSTATUS CKl_Reg::SetValue(PWCHAR uniValueName, ULONG Type, PVOID Data, ULONG DataSize)
{
	if (!m_hKey)
		return KL_NOTOPENED;

	ULONG ValueName_Len = wcslen(uniValueName);
	if (ValueName_Len+1 > MAX_KEYVALUENAME_LEN)
		return KL_PATHNOTFOUND;

	PCHAR ansiValueName = (PCHAR)HeapAllocate(ValueName_Len+1, 0);
	if (!ansiValueName)
		return KL_UNSUCCESSFUL;
	Unicode2Ansi(ansiValueName, uniValueName);

	if (ERROR_SUCCESS != RegSetValueEx(m_hKey, ansiValueName, 0, Type, (PBYTE)Data, DataSize))
	{
		HeapFree(ansiValueName, 0);
		return KL_UNSUCCESSFUL;
	}

	HeapFree(ansiValueName, 0);

	return KL_SUCCESS;
}

KLSTATUS CKl_Reg::EnumValues(ULONG EnumIndex, PWCHAR uniValueName, PULONG pValueName_Len/* in bytes*/)
{
	if (!m_hKey)
		return KL_NOTOPENED;

	PCHAR ansiValueName = (PCHAR)HeapAllocate(MAX_KEYVALUENAME_LEN, 0);
	if (!ansiValueName)
		return KL_UNSUCCESSFUL;

	ULONG ValueName_Len = MAX_KEYVALUENAME_LEN;
	ULONG ErrorCode = RegEnumValue(m_hKey, EnumIndex, ansiValueName, &ValueName_Len, 0, NULL, NULL, NULL);
	if (ERROR_SUCCESS != ErrorCode)
	{
		HeapFree(ansiValueName, 0);		

		if (ERROR_MORE_DATA == ErrorCode) // The buffer passed in is not large enough to hold the entire value.
			return KL_BUFFER_TOO_SMALL;

		if ( ERROR_NO_MORE_ITEMS == ErrorCode ) // There are no more keys or values to enumerate
			return KL_NOMOREENTRIES;

		return KL_UNSUCCESSFUL;
	}

	ULONG Provided_BufferLen = *pValueName_Len;
	*pValueName_Len = ValueName_Len*sizeof(WCHAR);
	if (*pValueName_Len > Provided_BufferLen)
	{
		HeapFree(ansiValueName, 0);
		return KL_BUFFER_TOO_SMALL;
	}

	Ansi2Unicode(uniValueName, ansiValueName);

	HeapFree(ansiValueName, 0);

	return KL_SUCCESS;
}

KLSTATUS CKl_Reg::QualifyRegRoot(PCHAR ansiKeyPath, PCHAR *p_ansiSubKey_Path, PHKEY p_hKeyRoot)
{
	ULONG REG_MACHINE_PATH_Len = strlen(REG_MACHINE_PATH);
	if (strnicmp(ansiKeyPath, REG_MACHINE_PATH, REG_MACHINE_PATH_Len) == 0)
	{
		*p_hKeyRoot = HKEY_LOCAL_MACHINE;
		*p_ansiSubKey_Path = ansiKeyPath+REG_MACHINE_PATH_Len;
	}
	else
	{
		ULONG REG_USER_PATH_Len = strlen(REG_USER_PATH);
		if (strnicmp(ansiKeyPath, REG_USER_PATH, REG_USER_PATH_Len) == 0)
		{
			*p_hKeyRoot = HKEY_USERS;
			*p_ansiSubKey_Path = ansiKeyPath+REG_USER_PATH_Len;
		}
		else
			return KL_UNSUCCESSFUL;
	}

	return KL_SUCCESS;
}

#endif // ISWIN9X
