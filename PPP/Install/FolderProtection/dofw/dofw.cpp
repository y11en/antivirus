// dofw.cpp : Defines the entry point for the DLL application.
//
/*
	Для установки полной (файл + реестр) защиты вызвать без параметров
	Для устновки защиты только файлов вызвать с Param1 == 3
	Для снятия защиты вызвать с параметрами Param1 == 4, Param2 == 4
 */

#include "stdafx.h"
#include <windows.h>
#include <shlobj.h>

#include "../../../../windows/Hook/hook/dofw_src.cpp"

HANDLE ghModule = NULL;
BOOL APIENTRY DllMain( HANDLE hModule, 
                       DWORD  ul_reason_for_call, 
                       LPVOID lpReserved
					 )
{
	switch (ul_reason_for_call)
	{
	case DLL_PROCESS_ATTACH:
		ghModule = hModule;
		break;
	default:
		break;
	}
    return TRUE;
}

DWORD __stdcall GoThis(DWORD Param1, DWORD Param2)
{
	DWORD Ret = 2;
	#define _UNI_MAX_PATH 32000
	PWCHAR pwchCurrentFN = (PWCHAR) HeapAlloc(GetProcessHeap(), 0, _UNI_MAX_PATH);
	if (!pwchCurrentFN)
		return 3; 						// low memory

	DWORD len = 0;
	if (Param2)
	{
		lstrcpyW(pwchCurrentFN, (PWCHAR) Param2);
		len = lstrlenW(pwchCurrentFN);
	}
	else
	{
		len = GetModuleFileNameW((HMODULE) ghModule, pwchCurrentFN, _UNI_MAX_PATH / sizeof(WCHAR));
		while (len)
		{
			WCHAR c = pwchCurrentFN[len];
			pwchCurrentFN[len] = 0;
			len--;
			if (c == L'\\')
				break;
		}
	}
	
	if (len)
		Ret = GoThisImp(pwchCurrentFN, Param1, TRUE);

	HeapFree(GetProcessHeap(), 0, pwchCurrentFN);

	return Ret;
}

DWORD __stdcall GoThisBack(DWORD Param1, DWORD Param2)
{
	if (Param1 != 4 && Param2 != 4)
		return 3;

	GoThisBackImp();

	return 0;
}

void __stdcall DisableFB(DWORD Param1, DWORD Param2)
{
	DisableFBImp();
}

DWORD __stdcall GoThisImp2(DWORD Param1, DWORD Param2)
{
	return GoProc(Param1, Param2);
}

