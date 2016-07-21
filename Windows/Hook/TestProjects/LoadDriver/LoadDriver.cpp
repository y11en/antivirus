// LoadDriver.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"

#define NT_SUCCESS(__status__)	(((LONG)(__status__)) >= 0L)

typedef
NTSTATUS
(WINAPI *t_fZwLoadDriver)(
	PUNICODE_STRING DriverName
	);

t_fZwLoadDriver g_pfZwLoadDriver = NULL;

HRESULT LoadDriver(PWCHAR ServiceRegPath)
{
	if (!g_pfZwLoadDriver)	
		return E_FAIL;

	UNICODE_STRING usServiceRegPath;
	usServiceRegPath.Buffer = ServiceRegPath;
	usServiceRegPath.Length = (USHORT)(sizeof(WCHAR)*wcslen(ServiceRegPath));
	usServiceRegPath.MaximumLength = usServiceRegPath.Length+sizeof(WCHAR);

	NTSTATUS ntStatus;
	ntStatus = g_pfZwLoadDriver(&usServiceRegPath);

	if (NT_SUCCESS(ntStatus))
		return S_OK;

	return E_FAIL;
}

int wmain(int argc, PWCHAR argv[])
{
	if (argc < 2)
	{
		printf("usage example:\tloaddriver \\registry\\machine\\system\\currentcontrolset\\services\\klif\n");
		return -1;
	}

	HMODULE hNtdll = GetModuleHandleW(L"ntdll.dll");
	if (hNtdll)
		g_pfZwLoadDriver = (t_fZwLoadDriver)GetProcAddress(hNtdll, "ZwLoadDriver");

	PWCHAR wcServiceRegPath = argv[1];

	printf("ZwLoadDriver(""%S"")..\n", wcServiceRegPath);
	HRESULT hResult;
	hResult = LoadDriver(wcServiceRegPath);
	if (SUCCEEDED(hResult))
	{
		printf("succeeded!\n");
		return 0;
	}

	printf("failed!\n");

	return 0;
}

