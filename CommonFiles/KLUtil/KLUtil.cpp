#include "stdafx.h"
#include <locale.h>
#include "KLUtil.h"
#include "../Stuff/StdString.h"

HINSTANCE g_hKLUtilRes;

namespace KLUTIL
{
	void InitializeLog();
	void FreeLogThreadName();
	void SetLogThreadName(LPCTSTR pszThreadName);
}

using namespace KLUTIL;


BOOL APIENTRY DllMain( HANDLE hModule, 
                       DWORD  ul_reason_for_call, 
                       LPVOID lpReserved
					 )
{
	switch (ul_reason_for_call)
	{
	case DLL_PROCESS_ATTACH:
		{
			/*	locale должно выставл€ть приложение, а так мы 
				можем испортить locale не известно кому, например SrciptChecker
				загрузитс€ в чужой процесс и там накас€чит.

			TCHAR szACP[9] = {_T('.')};
			_itot(GetACP(), szACP + 1, 10);
			_tsetlocale(LC_ALL, szACP);
			*/
			g_hKLUtilRes = LoadLibraryEx(_T("UTLOC.dll"), NULL, LOAD_LIBRARY_AS_DATAFILE);
			if (g_hKLUtilRes == NULL)
			{
				g_hKLUtilRes = (HINSTANCE) hModule;
			}
			
			CStdString::SetResourceHandle(g_hKLUtilRes);
			KLUTIL::InitializeLog();
		}
	// fall through (дл€ потока, вызвавшего DLL_PROCESS_ATTACH DLL_THREAD_ATTACH не вызываетс€, см. MSDN)
	case DLL_THREAD_ATTACH:
		{
			SetLogThreadName(NULL);
		}
		break;
	case DLL_PROCESS_DETACH:
		{
			if (g_hKLUtilRes != hModule)
			{
				CStdString::SetResourceHandle((HINSTANCE) hModule);
				FreeLibrary(g_hKLUtilRes);
				g_hKLUtilRes = (HINSTANCE) hModule;
			}
		}
	// fall through (дл€ главного потока DLL_THREAD_DETACH не вызываетс€ (?))
	case DLL_THREAD_DETACH:
		{
			FreeLogThreadName();
		}
		break;
	}

    return TRUE;
}

