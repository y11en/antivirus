#include "all_func_def.h"
#include <aclapi.h>

NtCreateKeyFunc NtCreateKey = 0;
NtSetValueKeyFunc NtSetValueKey = 0;
NtDeleteKeyFunc NtDeleteKey = 0;
NtMapViewOfSectionFunc NtMapViewOfSection = 0;

RtlAnsiStringToUnicodeStringFunc RtlAnsiStringToUnicodeString = 0;
RtlInitAnsiStringFunc RtlInitAnsiString = 0;
RtlFreeUnicodeStringFunc RtlFreeUnicodeString = 0;
NtOpenSectionFunc NtOpenSection = 0;

///////////////////////////////////////////////////////////////
//	service func to init kernel funcs
///////////////////////////////////////////////////////////////
bool LocateNTDLLFunctions()
{
	if (NtOpenSection)
		return true;

	HMODULE hntdll;
	hntdll = GetModuleHandle("ntdll.dll");
	if (hntdll == 0)
		return false;

	if (!(NtCreateKey = (NtCreateKeyFunc)GetProcAddress(hntdll, "NtCreateKey"))) {
		printf("Could not find NtCreateKey entry point in NTDLL.DLL\n");
		return false;
	}
	if (!(NtDeleteKey = (NtDeleteKeyFunc)GetProcAddress(hntdll, "NtDeleteKey"))) {
		printf("Could not find NtDeleteKey entry point in NTDLL.DLL\n");
		return false;
	}
	if (!(NtSetValueKey = (NtSetValueKeyFunc)GetProcAddress(hntdll, "NtSetValueKey"))) {
		printf("Could not find NtSetValueKey entry point in NTDLL.DLL\n");
		return false;
	}
	if (!(NtMapViewOfSection = (NtMapViewOfSectionFunc)GetProcAddress(hntdll, "NtMapViewOfSection"))) {
		printf("Could not find NtMapViewOfSection entry point in NTDLL.DLL\n");
		return false;
	}
		

	if (!(RtlAnsiStringToUnicodeString = (RtlAnsiStringToUnicodeStringFunc)GetProcAddress(hntdll, "RtlAnsiStringToUnicodeString"))) {
		printf("Could not find NtSetValueKey entry point in NTDLL.DLL\n");
		return false;
	}
	if (!(RtlInitAnsiString = (RtlInitAnsiStringFunc)GetProcAddress(hntdll, "RtlInitAnsiString"))) {
		printf("Could not find RtlAnsiStringToUnicodeString entry point in NTDLL.DLL\n");
		return false;
	}
	if (!(RtlFreeUnicodeString = (RtlFreeUnicodeStringFunc)GetProcAddress(hntdll, "RtlFreeUnicodeString"))) {
		printf("Could not find RtlFreeUnicodeString entry point in NTDLL.DLL\n");
		return false;
	}
	if (!(NtOpenSection = (NtOpenSectionFunc)GetProcAddress(hntdll, "NtOpenSection"))) {
		printf("Could not find NtOpenSection entry point in NTDLL.DLL\n");
		return false;
	}
	return true;
}
///////////////////////////////////////////////////////////////
//	service function to access to physical memory
///////////////////////////////////////////////////////////////
HANDLE openPhyMem()
{
	LocateNTDLLFunctions();
	HANDLE hPhyMem;
	OBJECT_ATTRIBUTES oAttr;

	ANSI_STRING aStr;
		
	RtlInitAnsiString(&aStr, "\\device\\physicalmemory");
						
	UNICODE_STRING uStr;

	if(RtlAnsiStringToUnicodeString(&uStr, &aStr, TRUE) != STATUS_SUCCESS)
	{		
		return INVALID_HANDLE_VALUE;	
	}

    oAttr.Length = sizeof(OBJECT_ATTRIBUTES);
    oAttr.RootDirectory = NULL;
    oAttr.Attributes = OBJ_CASE_INSENSITIVE;
    oAttr.ObjectName = &uStr;
    oAttr.SecurityDescriptor = NULL;
    oAttr.SecurityQualityOfService = NULL;

	ULONG uRes = 0;
	if((uRes = NtOpenSection(&hPhyMem, SECTION_MAP_READ | SECTION_MAP_WRITE, &oAttr)) != STATUS_SUCCESS)
	{		
		printf("NtOpenSection fail (uRes = %d), err = %d\n", uRes, GetLastError());
		return INVALID_HANDLE_VALUE;
	}

	return hPhyMem;
}
///////////////////////////////////////////////////////////////
//	service func 
//	not used
///////////////////////////////////////////////////////////////
BOOL assignACL(void)
{
	HANDLE hPhyMem;
	OBJECT_ATTRIBUTES oAttr;
	BOOL result = FALSE;

	ANSI_STRING aStr;
		
	RtlInitAnsiString(&aStr, "\\device\\physicalmemory");
						
	UNICODE_STRING uStr;

	if(RtlAnsiStringToUnicodeString(&uStr, &aStr, TRUE) != STATUS_SUCCESS)
	{		
		return FALSE;
	}

    oAttr.Length = sizeof(OBJECT_ATTRIBUTES);
    oAttr.RootDirectory = NULL;
    oAttr.Attributes = OBJ_CASE_INSENSITIVE;
    oAttr.ObjectName = &uStr;
    oAttr.SecurityDescriptor = NULL;
    oAttr.SecurityQualityOfService = NULL;

	if(NtOpenSection(&hPhyMem, READ_CONTROL | WRITE_DAC, &oAttr ) != STATUS_SUCCESS)
	{		
		return FALSE;
	}
	else
	{
		PACL dacl;
		PSECURITY_DESCRIPTOR sd;
		
		if(GetSecurityInfo(hPhyMem, SE_KERNEL_OBJECT, DACL_SECURITY_INFORMATION, NULL, NULL,
						&dacl, NULL, &sd) == ERROR_SUCCESS)
		{
			EXPLICIT_ACCESS ea;
			char userName[MAX_PATH];
			DWORD userNameSize = MAX_PATH-1;

			GetUserName(userName, &userNameSize);
			ea.grfAccessPermissions = SECTION_MAP_WRITE;
			ea.grfAccessMode = GRANT_ACCESS;
			ea.grfInheritance = NO_INHERITANCE;
			ea.Trustee.pMultipleTrustee = NULL;
			ea.Trustee.MultipleTrusteeOperation = NO_MULTIPLE_TRUSTEE;
			ea.Trustee.TrusteeForm = TRUSTEE_IS_NAME;
			ea.Trustee.TrusteeType = TRUSTEE_IS_USER;
			ea.Trustee.ptstrName = userName;

			PACL newDacl;
			if(SetEntriesInAcl(1, &ea, dacl, &newDacl) == ERROR_SUCCESS)
			{
				if(SetSecurityInfo(hPhyMem, SE_KERNEL_OBJECT, DACL_SECURITY_INFORMATION, NULL, NULL,
								newDacl, NULL) == ERROR_SUCCESS)
				{		
					result = TRUE;
				}

				LocalFree(newDacl);
			}
		}
	}

	return result;	
}


///////////////////////////////////////////////////////////////
//	service func 
///////////////////////////////////////////////////////////////
LPVOID NTAPI MapViewOfFileExWrap(HANDLE hProcess, HANDLE hFileMappingObject, DWORD dwDesiredAccess, DWORD dwFileOffsetHigh, 
        DWORD dwFileOffsetLow, DWORD dwNumberOfBytesToMap, LPVOID lpBaseAddress )   
{
	NTSTATUS Status;
	LARGE_INTEGER SectionOffset;
	ULONG ViewSize;
	ULONG Protect;
	LPVOID ViewBase;
	// Convert the offset
	SectionOffset.LowPart = dwFileOffsetLow;
	SectionOffset.HighPart = dwFileOffsetHigh;
	// Save the size and base
	ViewBase = lpBaseAddress;
	ViewSize = dwNumberOfBytesToMap;
	// Convert flags to NT Protection Attributes
	if (dwDesiredAccess & FILE_MAP_WRITE) {
		Protect  = PAGE_READWRITE;
	} else if (dwDesiredAccess & FILE_MAP_READ)	{
		Protect = PAGE_READONLY;
	} else if (dwDesiredAccess & FILE_MAP_COPY)	{
		Protect = PAGE_WRITECOPY;
	} else {
		Protect = PAGE_NOACCESS;
	}
	// Map the section
	Status = NtMapViewOfSection(hFileMappingObject,
								hProcess,
								&ViewBase,
								0,
								0,
								&SectionOffset,
								&ViewSize,
								ViewShare,
								0,
								Protect);
	if (!NT_SUCCESS(Status)) {
		// We failed
		return NULL;
	}
	// Return the base
    return ViewBase;
}
