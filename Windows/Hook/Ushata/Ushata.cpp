// Ushata.cpp : Defines the entry point for the DLL application.
//

#include "stdafx.h"
#include "Ushata.h"
#include "exphooks.h"
#include <tchar.h>
#include <stdio.h>
#include "..\..\..\CommonFiles\Version\ver_kl.h"

#ifndef _DONT_REGISTER_THREADS_
#include "..\hook\FSSync.h"
#endif

static HMODULE s_hThisModule = NULL;
static BOOL s_bIsLoaderKLVerified = FALSE;

#ifdef _MANAGED
#pragma managed(push, off)
#endif

static void CheckLoaderKLVerified();

BOOL APIENTRY DllMain( HMODULE hModule,
                       DWORD  ul_reason_for_call,
                       LPVOID lpvReserved
					 )
{
	switch (ul_reason_for_call)
	{
	case DLL_PROCESS_ATTACH:
		CheckLoaderKLVerified();
		if (s_bIsLoaderKLVerified)
		{
			s_hThisModule = hModule;
		}
		break;
	case DLL_PROCESS_DETACH:
		if (s_bIsLoaderKLVerified)
		{
			DoneExportHooks();
#ifndef _DONT_REGISTER_THREADS_
			FSDrv_Done();
#endif
		}
		break;
#ifndef _DONT_REGISTER_THREADS_
	case DLL_THREAD_ATTACH:
		if (s_bIsLoaderKLVerified)
		{
			FSDrv_RegisterInvisibleThread();
		}
		break;
	case DLL_THREAD_DETACH:
		if (s_bIsLoaderKLVerified)
		{
			FSDrv_UnRegisterInvisibleThread();
		}
		break;
#endif
	}
    return TRUE;
}

#ifdef _MANAGED
#pragma managed(pop)
#endif

static BOOL bUshHook_Inited = FALSE;
static BOOL bUshFsdr_Inited = FALSE;

USHATA_API BOOLEAN Ushata(BOOLEAN bMakeHooks)
{
	if (bMakeHooks && !bUshHook_Inited)
	{
		bUshHook_Inited = TRUE;
		if (!InitExportHooks(s_hThisModule, bMakeHooks == 2))
		{
			bUshHook_Inited = FALSE;
			return FALSE;
		}
	}

	if (!bUshFsdr_Inited)
	{
		bUshFsdr_Inited = TRUE;
#ifndef _DONT_REGISTER_THREADS_
		if (!FSDrv_Init())
		{
			bUshFsdr_Inited = FALSE;
			return FALSE;
		}
		FSDrv_RegisterInvisibleThread();
#endif
	}

	return TRUE;
}

static void CheckLoaderKLVerified()
{
	s_bIsLoaderKLVerified = FALSE;

	HMODULE exeModule = GetModuleHandle(NULL);
	TCHAR fileName[MAX_PATH];
	GetModuleFileName(exeModule, fileName, MAX_PATH);

	DWORD dwHandle;
	DWORD dwVersionSize = ::GetFileVersionInfoSize(fileName, &dwHandle);
	if (dwVersionSize)
	{
		PVOID pVersionInfo = HeapAlloc(GetProcessHeap(), 0, dwVersionSize);
		if (pVersionInfo)
		{
			if (::GetFileVersionInfo(fileName, NULL, dwVersionSize, pVersionInfo))
			{
				UINT nLen = 0;
				LPDWORD pnLangCp;
				if (::VerQueryValue(pVersionInfo, _T("\\VarFileInfo\\Translation"), reinterpret_cast<LPVOID*>(&pnLangCp), &nLen))
				{
					#define __SWAPWORDS__(X) ( (X<<16) | (X>>16) )

					TCHAR sLangCp[20];
					_stprintf(sLangCp, _T("%08X"), __SWAPWORDS__(*pnLangCp));
					TCHAR sVerPath[2048];
					_sntprintf(sVerPath, sizeof(sVerPath)/sizeof(TCHAR), _T("\\StringFileInfo\\%s\\CompanyName"), sLangCp);

					UINT nLen = 0;
					LPTSTR pVerValue = NULL;
					::VerQueryValue(pVersionInfo, sVerPath, (LPVOID *)&pVerValue, &nLen);
					if (strcmp(pVerValue, VER_COMPANYNAME_STR) == 0)
						s_bIsLoaderKLVerified = TRUE;
				}
			}
			HeapFree(GetProcessHeap(), 0, pVersionInfo);
		}
	}
}