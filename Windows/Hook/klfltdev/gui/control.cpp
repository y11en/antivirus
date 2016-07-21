#include "stdafx.h"
#include "GuidConst.h"
#include "winioctl.h"
#include "..\ioctl.h"

HRESULT
GetRulesListSize ( PULONG psize )
{
	WCHAR	dos_device_name[256];
	HANDLE	device;
	ULONG	nread;
	KLFLTDEV_RULES_SIZE klfltdev_rules_size;
	
	klfltdev_rules_size.m_size = 0;
	*psize = 0;

	wsprintf(dos_device_name, L"\\\\.\\%s", (WCHAR*)SYMBOLIC_NAME_STRING+(sizeof(L"\\DosDevices\\")-sizeof(WCHAR))/sizeof(WCHAR) );

	device = CreateFileW(
		dos_device_name,
		GENERIC_READ | GENERIC_WRITE,
		FILE_SHARE_READ | FILE_SHARE_WRITE,
		NULL,
		OPEN_EXISTING,
		FILE_ATTRIBUTE_NORMAL,
		NULL
		);

	if (device == INVALID_HANDLE_VALUE)
	{
// 		WCHAR buf[50];
// 		wsprintf (buf , L"CreateFileW Error %d", GetLastError() );
// 		MessageBox( buf, L"Error", MB_OK );

		return E_FAIL;
	}

	if (!DeviceIoControl(
		device,
		IOCTL_KLFLTDEV_GETRULES_SIZE,
		NULL,
		0,
		&klfltdev_rules_size, sizeof(KLFLTDEV_RULES_SIZE),
		&nread,
		NULL
		))
	{
		CloseHandle(device);

// 		WCHAR buf[50];
// 		wsprintf (buf , L"DeviceIoControl Error %d", GetLastError() );
// 		MessageBox( buf, L"Error", MB_OK );

		return E_FAIL;
	}
	
	*psize = klfltdev_rules_size.m_size;

	CloseHandle(device);
	return S_OK;
}

HRESULT
GetRulesList ( ULONG max_size,  PKLFLTDEV_RULES pdev_rules )
{
	WCHAR	dos_device_name[256];
	HANDLE	device;
	ULONG	nread;
	
	memset( pdev_rules, 0, max_size );

	wsprintf(dos_device_name, L"\\\\.\\%s", (WCHAR*)SYMBOLIC_NAME_STRING+(sizeof(L"\\DosDevices\\")-sizeof(WCHAR))/sizeof(WCHAR) );

	device = CreateFileW(
		dos_device_name,
		GENERIC_READ | GENERIC_WRITE,
		FILE_SHARE_READ | FILE_SHARE_WRITE,
		NULL,
		OPEN_EXISTING,
		FILE_ATTRIBUTE_NORMAL,
		NULL
		);

	if (device == INVALID_HANDLE_VALUE)
	{
		// 		WCHAR buf[50];
		// 		wsprintf (buf , L"CreateFileW Error %d", GetLastError() );
		// 		MessageBox( buf, L"Error", MB_OK );

		return E_FAIL;
	}

	if (!DeviceIoControl(
		device,
		IOCTL_KLFLTDEV_GETRULES,
		NULL,
		0,
		pdev_rules, max_size,
		&nread,
		NULL
		))
	{
		CloseHandle(device);

		// 		WCHAR buf[50];
		// 		wsprintf (buf , L"DeviceIoControl Error %d", GetLastError() );
		// 		MessageBox( buf, L"Error", MB_OK );

		return E_FAIL;
	}

	
	CloseHandle(device);
	return S_OK;
}


HRESULT
SetRule2 ( PKLFLTDEV_RULE prule, ULONG rule_size, PREPLUG_STATUS preplug_status )
{
	WCHAR	dos_device_name[256];
	HANDLE	device;
	ULONG	nread;

	if (!prule)
		return E_OUTOFMEMORY;


	wsprintf(dos_device_name, L"\\\\.\\%s", (WCHAR*)SYMBOLIC_NAME_STRING+(sizeof(L"\\DosDevices\\")-sizeof(WCHAR))/sizeof(WCHAR) );

	device = CreateFileW(
		dos_device_name,
		GENERIC_READ | GENERIC_WRITE,
		FILE_SHARE_READ | FILE_SHARE_WRITE,
		NULL,
		OPEN_EXISTING,
		FILE_ATTRIBUTE_NORMAL,
		NULL
		);

	if (device == INVALID_HANDLE_VALUE)
	{
		delete [] prule;
		// 		WCHAR buf[50];
		// 		wsprintf (buf , L"CreateFile Error %d", GetLastError() );
		// 		MessageBox( buf, L"Error", MB_OK );
		return E_FAIL;
	}

	if (!DeviceIoControl(
		device,
		IOCTL_KLFLTDEV_ADDRULE,
		prule,
		rule_size,
		preplug_status, 
		sizeof(REPLUG_STATUS),
		&nread,
		NULL
		))
	{
		CloseHandle(device);
		delete [] prule;

		// 		WCHAR buf[50];
		// 		wsprintf (buf , L"DeviceIoControl Error %d", GetLastError() );
		// 		MessageBox( buf, L"Error", MB_OK );

		return E_FAIL;
	}

	CloseHandle(device);

	return S_OK;
}

HRESULT
SetRule ( PWCHAR wcGuid, PWCHAR wcDevType, ULONG mask, PREPLUG_STATUS preplug_status )
{
	PKLFLTDEV_RULE prule;
	ULONG rule_size;
	
	PULONG pStrSize;
	PWCHAR pStr;
	ULONG  wcGuidSize;
	ULONG  wcDevTypeSize;


	wcGuidSize = lstrlen(wcGuid)*sizeof(WCHAR);
	wcDevTypeSize = lstrlen(wcDevType)*sizeof(WCHAR);


	rule_size = sizeof(KLFLTDEV_RULE) + 
					wcGuidSize + sizeof(ULONG) + 
					wcDevTypeSize + sizeof(ULONG);
		 	
	prule = (PKLFLTDEV_RULE) new char [ rule_size ];
	if (!prule)
		return E_OUTOFMEMORY;

	memset( prule, 0x00, rule_size );

	
	prule->m_mask = mask;
	
	//Guid
	pStrSize = (ULONG*)((char*) prule + sizeof(KLFLTDEV_RULE));
	*pStrSize = wcGuidSize;
	pStr = (PWCHAR) (pStrSize + 1);
	memcpy( pStr, wcGuid, wcGuidSize );


	//DevType
	pStrSize = (ULONG*)((char*) pStr + wcGuidSize );
	*pStrSize = wcDevTypeSize;
	pStr = (PWCHAR) (pStrSize + 1);
	memcpy( pStr, wcDevType, wcDevTypeSize );

	HRESULT hResult = SetRule2( prule, rule_size, preplug_status);

	delete [] prule;

	return hResult;
}


HRESULT
ApplyRules()
{

	WCHAR	dos_device_name[256];
	HANDLE	device;
	ULONG	nread;

	wsprintf(dos_device_name, L"\\\\.\\%s", (WCHAR*)SYMBOLIC_NAME_STRING+(sizeof(L"\\DosDevices\\")-sizeof(WCHAR))/sizeof(WCHAR) );


	device = CreateFileW(
		dos_device_name,
		GENERIC_READ | GENERIC_WRITE,
		FILE_SHARE_READ | FILE_SHARE_WRITE,
		NULL,
		OPEN_EXISTING,
		FILE_ATTRIBUTE_NORMAL,
		NULL
		);

	if (device == INVALID_HANDLE_VALUE)
	{
// 		WCHAR buf[50];
// 		wsprintf (buf , L"CreateFileW Error %d", GetLastError() );
// 		MessageBox( buf, L"Error", MB_OK );

		return E_FAIL;
	}

	if (!DeviceIoControl(
		device,
		IOCTL_KLFLTDEV_APPLYRULES,
		NULL,
		0,
		NULL, 
		0,
		&nread,
		NULL
		))
	{
		CloseHandle(device);

// 		WCHAR buf[50];
// 		wsprintf (buf , L"DeviceIoControl Error %d", GetLastError() );
// 		MessageBox( buf, L"Error", MB_OK );

		return E_FAIL;
	}

	CloseHandle(device);
	return S_OK;
}