// ZwSetSystemInformation.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"

#include <stdio.h>
#include <windows.h>
#include <winternl.h>

#define NT_SUCCESS(__status__)	(((LONG)(__status__)) >= 0L)

typedef struct _SYSTEM_GLOBAL_FLAG { // Information Class 9
	ULONG GlobalFlag;
} SYSTEM_GLOBAL_FLAG, *PSYSTEM_GLOBAL_FLAG;

typedef struct _SYSTEM_LOAD_AND_CALL_IMAGE { // Information Class 38
	UNICODE_STRING ModuleName;
} SYSTEM_LOAD_AND_CALL_IMAGE, *PSYSTEM_LOAD_AND_CALL_IMAGE;


// typedef enum _SYSTEM_INFORMATION_CLASS {
// 	SystemGlobalFlag = 9, // 9 Y Y
// 	SystemLoadAndCallImage = 38,
// } SYSTEM_INFORMATION_CLASS;

//#endif

typedef NTSTATUS (NTAPI* _MY_ZwQuerySystemInformation)(
	IN SYSTEM_INFORMATION_CLASS SystemInformationClass,
	IN OUT PVOID SystemInformation,
	IN ULONG SystemInformationLength,
	OUT PULONG ReturnLength OPTIONAL
	);

typedef NTSTATUS (NTAPI* _MY_ZwSetSystemInformation)(
	IN SYSTEM_INFORMATION_CLASS SystemInformationClass,
	IN OUT PVOID SystemInformation,
	IN ULONG SystemInformationLength
	);

_MY_ZwQuerySystemInformation myZwQuerySystemInformation;
_MY_ZwSetSystemInformation myZwSetSystemInformation;

BOOL EnablePrivilege( LPCWSTR privilege_name  )
{
	HANDLE hToken; 
	TOKEN_PRIVILEGES tkp; 

	// Get a token for this process. 

	if (!OpenProcessToken(GetCurrentProcess(), 
		TOKEN_ADJUST_PRIVILEGES | TOKEN_QUERY, &hToken)) 
		return( FALSE ); 

	// Get the LUID for the shutdown privilege. 

	LookupPrivilegeValue(NULL, privilege_name, 
		&tkp.Privileges[0].Luid); 

	tkp.PrivilegeCount = 1;  // one privilege to set    
	tkp.Privileges[0].Attributes = SE_PRIVILEGE_ENABLED; 

	// Get the shutdown privilege for this process. 

	AdjustTokenPrivileges(hToken, FALSE, &tkp, 0, 
		(PTOKEN_PRIVILEGES)NULL, 0); 

	if ( GetLastError() != ERROR_SUCCESS ) 
		return FALSE; 

	// Shut down the system and force all applications to close. 

	return TRUE;
}

HRESULT LoadAndCallImage(PWCHAR ServiceRegPath)
{
	if (!myZwSetSystemInformation)	
		return E_FAIL;

	SYSTEM_LOAD_AND_CALL_IMAGE sys_load_call_image;
	UNICODE_STRING usServiceRegPath;
	usServiceRegPath.Buffer = ServiceRegPath;
	usServiceRegPath.Length = (USHORT)(sizeof(WCHAR)*wcslen(ServiceRegPath));
	usServiceRegPath.MaximumLength = usServiceRegPath.Length+sizeof(WCHAR);

	memcpy( &sys_load_call_image.ModuleName, &usServiceRegPath, sizeof(UNICODE_STRING) );
	
	NTSTATUS ntStatus;
	ntStatus = myZwSetSystemInformation(
		(SYSTEM_INFORMATION_CLASS)38, //SystemLoadAndCallImage,
		&sys_load_call_image,
		sizeof(sys_load_call_image)
		);
	
	if (NT_SUCCESS(ntStatus))
		return S_OK;

	return E_FAIL;
}


int _tmain(int argc, _TCHAR* argv[])
{
	if (argc < 2)
	{
		printf("usage example:\tloaddriver \\registry\\machine\\system\\currentcontrolset\\services\\klif\n");
		return -1;
	}

	HMODULE hinstLib=LoadLibrary(L"ntdll.dll");

	if (hinstLib == NULL) 
		goto END;

	
	myZwSetSystemInformation = (_MY_ZwSetSystemInformation)GetProcAddress(hinstLib, "ZwSetSystemInformation");
	if (!myZwSetSystemInformation)
	{
		printf("GetProcAddress ZwSetSystemInformation - error!\n");
		goto END;
	}

	PWCHAR wcServiceRegPath = argv[1];

	printf("ZwSetSystemInformation SystemLoadAndCallImage(""%S"")..\n", wcServiceRegPath);
	HRESULT hResult;
	hResult = LoadAndCallImage(wcServiceRegPath);
	if (SUCCEEDED(hResult))
	{
		printf("succeeded!\n");
	}
	else
		printf("failed!\n");
	
END:
	if (hinstLib)
		FreeLibrary(hinstLib); 

	return 0;
}

