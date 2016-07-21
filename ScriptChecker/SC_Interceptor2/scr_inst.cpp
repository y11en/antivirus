#include <windows.h>
#include "debug.h"
#include "scr_inst.h"
#include "siteinfo.h"

SCRIPT_ENGINE_INSTANCE_DATA* g_pScriptEngineInstanceDataFirst = NULL;

CRITICAL_SECTION g_CriticalSec;

void DumpEnginesList()
{
#ifdef _DEBUG
	SCRIPT_ENGINE_INSTANCE_DATA* pScriptEngineInstanceData = NULL;
	ODS(("--- DumpEnginesList -----------------------------"));
	EnterCriticalSection(&g_CriticalSec);
	pScriptEngineInstanceData = g_pScriptEngineInstanceDataFirst;
	ODS(("Data      IActiveScript  IActiveScriptParse  IActiveScriptSite  IActiveScriptSiteDebug  Deny  Name"));
	while (pScriptEngineInstanceData != NULL)
	{
		ODS(("%08X  %08X       %08X            %08X           %08X                %s     %S",
			pScriptEngineInstanceData,
			pScriptEngineInstanceData->pActiveScript, 
			pScriptEngineInstanceData->pActiveScriptParse, 
			pScriptEngineInstanceData->pActiveScriptSite,
			pScriptEngineInstanceData->pActiveScriptSiteDebug,
			(pScriptEngineInstanceData->bDenyExecution ? "T" : "F"),
			(pScriptEngineInstanceData->pwcsLanguageName == NULL ? L"NULL" : pScriptEngineInstanceData->pwcsLanguageName)
			));
		pScriptEngineInstanceData = pScriptEngineInstanceData->pNext;
	}
	LeaveCriticalSection(&g_CriticalSec);
	ODS(("-------------------------------------------------"));
#endif
}

void ScriptEngineListInit()
{
	InitializeCriticalSection(&g_CriticalSec);
}

void ScriptEngineListDone()
{
	DumpEnginesList();
	if (g_pScriptEngineInstanceDataFirst != NULL)
	{
		ODS(("??? ScriptEngineDone: g_pScriptEngineInstanceDataFirst != NULL"));
	}
	DeleteCriticalSection(&g_CriticalSec);
}

SC_THREAD_DATA* GetSCThreadData()
{
	if (g_TlsSCThreadData == TLS_OUT_OF_INDEXES)
		return NULL;
	SC_THREAD_DATA* pThreadData = (SC_THREAD_DATA*)TlsGetValue(g_TlsSCThreadData);
	if (!pThreadData)
	{
		pThreadData = (SC_THREAD_DATA*)HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, sizeof(SC_THREAD_DATA));
		TlsSetValue(g_TlsSCThreadData, pThreadData);
	}
	return pThreadData;

}

void FreeSCThreadData()
{
	if (g_TlsSCThreadData == TLS_OUT_OF_INDEXES)
		return;
	SC_THREAD_DATA* pThreadData = (SC_THREAD_DATA*)TlsGetValue(g_TlsSCThreadData);
	if (pThreadData)
	{
		TlsSetValue(g_TlsSCThreadData, NULL);
		HeapFree(GetProcessHeap(), 0, pThreadData);
	}
	return;
}

SCRIPT_ENGINE_INSTANCE_DATA* GetScriptEngineInstanceData(PVOID ptr)
{
	SCRIPT_ENGINE_INSTANCE_DATA* pScriptEngineInstanceData = NULL;

	if (ptr == NULL)
		return NULL;
	
	EnterCriticalSection(&g_CriticalSec);

	pScriptEngineInstanceData = g_pScriptEngineInstanceDataFirst;
	while (pScriptEngineInstanceData != NULL)
	{
		if (pScriptEngineInstanceData->pActiveScript == ptr || 
			pScriptEngineInstanceData->pActiveScriptParse == ptr ||
			pScriptEngineInstanceData->pActiveScriptSite == ptr ||
			pScriptEngineInstanceData == ptr)
				break;
		pScriptEngineInstanceData = pScriptEngineInstanceData->pNext;
	}

	LeaveCriticalSection(&g_CriticalSec);

	return pScriptEngineInstanceData;
}

SCRIPT_ENGINE_INSTANCE_DATA* AddScriptEngineInstanceData()
{
	SCRIPT_ENGINE_INSTANCE_DATA* pScriptEngineInstanceData= NULL;

	pScriptEngineInstanceData = (SCRIPT_ENGINE_INSTANCE_DATA*)HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, sizeof(SCRIPT_ENGINE_INSTANCE_DATA));
	ODS(("AddScriptEngineInstanceData: new pScriptEngineInstanceData=%08X", pScriptEngineInstanceData));
	
	EnterCriticalSection(&g_CriticalSec);

	pScriptEngineInstanceData->pNext = g_pScriptEngineInstanceDataFirst;
	g_pScriptEngineInstanceDataFirst = pScriptEngineInstanceData;
	
	DumpEnginesList();

	LeaveCriticalSection(&g_CriticalSec);
	return pScriptEngineInstanceData;
}

BOOL DeleteScriptEngineInstanceData(SCRIPT_ENGINE_INSTANCE_DATA* pCurScriptEngineInstanceData)
{
	BOOL bRes = FALSE;
	SCRIPT_ENGINE_INSTANCE_DATA* pScriptEngineInstanceData = NULL;
	SCRIPT_ENGINE_INSTANCE_DATA* pScriptEngineInstanceDataPrev = NULL;

	DumpEnginesList();
	ODS(("DeleteScriptEngineInstanceData(%08X)", pCurScriptEngineInstanceData));

	EnterCriticalSection(&g_CriticalSec);	
	
	pScriptEngineInstanceData = g_pScriptEngineInstanceDataFirst;
	while (pScriptEngineInstanceData != NULL)
	{
		if (pScriptEngineInstanceData == pCurScriptEngineInstanceData)
			break;
		pScriptEngineInstanceDataPrev = pScriptEngineInstanceData;
		pScriptEngineInstanceData = pScriptEngineInstanceData->pNext;
	}
	
	if (pScriptEngineInstanceData != NULL)
	{
		if (pScriptEngineInstanceDataPrev == NULL) // first instance was found
		{
			g_pScriptEngineInstanceDataFirst = g_pScriptEngineInstanceDataFirst->pNext;
		}
		else
		{
			pScriptEngineInstanceDataPrev->pNext = pScriptEngineInstanceData->pNext;
		}

		if (pScriptEngineInstanceData->pwcsLanguageName != NULL)
			HeapFree(GetProcessHeap(), 0, pScriptEngineInstanceData->pwcsLanguageName);
		if (pScriptEngineInstanceData->wcsScriptText != NULL)
			HeapFree(GetProcessHeap(), 0, pScriptEngineInstanceData->wcsScriptText);
		if (pScriptEngineInstanceData->wcsURL != NULL)
			SysFreeString(pScriptEngineInstanceData->wcsURL);
		HeapFree(GetProcessHeap(), 0, pScriptEngineInstanceData);

		bRes = TRUE;
	}

	LeaveCriticalSection(&g_CriticalSec);
	DumpEnginesList();
	return bRes;
}

/*
IActiveScript* GetCurEngineInstance()
{
	IActiveScript* pCurEngine = NULL;
	SCRIPT_ENGINE_INSTANCE_DATA* pScriptEngineInstanceData;
	SCRIPT_ENGINE_INSTANCE_DATA* pScriptEngineInstanceData2;

	EnterCriticalSection(&g_CriticalSec);	
	pScriptEngineInstanceData = g_pScriptEngineInstanceDataFirst;
	while (pScriptEngineInstanceData != NULL)
	{
		if (pScriptEngineInstanceData->pActiveScript != NULL)
		{
			__try
			{
				DWORD dwWin32ThreadID = GetCurrentThreadId();
				SCRIPTTHREADID stidThread;
				if (S_OK == pScriptEngineInstanceData->pActiveScript->GetScriptThreadID(dwWin32ThreadID, &stidThread))
				{
					ODS(("CurrentWin32ThreadID=%x CurrentScriptThreadID=%x", dwWin32ThreadID, stidThread));
					pScriptEngineInstanceData2 = g_pScriptEngineInstanceDataFirst;
					while (pScriptEngineInstanceData2 != NULL && pScriptEngineInstanceData2->pActiveScript != NULL)
					{
						__try
						{
							SCRIPTTHREADID stidThread2;
							if (S_OK == pScriptEngineInstanceData2->pActiveScript->GetCurrentScriptThreadID(&stidThread2))
							{
								ODS(("	pActiveScript=%08X CurrentScriptThreadID=%x", pScriptEngineInstanceData2->pActiveScript, stidThread2));
								if (stidThread == stidThread2)
								{
									pCurEngine = pScriptEngineInstanceData2->pActiveScript;
									//break;
								}
							}
						}
						__except(EXCEPTION_EXECUTE_HANDLER)
						{
							ODS(("ERROR: GetCurEngineInstance caused exception on calling %08X", pScriptEngineInstanceData2->pActiveScript));
						}
						pScriptEngineInstanceData2 = pScriptEngineInstanceData2->pNext;
					}

				}
			}
			__except(EXCEPTION_EXECUTE_HANDLER)
			{
				ODS(("ERROR: GetCurEngineInstance caused exception on calling %08X", pScriptEngineInstanceData->pActiveScript));
			}
		}
		//if (pCurEngine != NULL)
			break;
		pScriptEngineInstanceData = pScriptEngineInstanceData->pNext;
	}
	
	LeaveCriticalSection(&g_CriticalSec);
	return pCurEngine;
}
*/
/*
BOOL SetCurEngineInstanceTls(PVOID pScriptEngineInstance)
{
	SC_THREAD_DATA* pThreadData = GetSCThreadData();
	SCRIPT_ENGINE_INSTANCE_DATA* pScriptEngineInstanceData = GetScriptEngineInstanceData(pScriptEngineInstance);
	if (NULL == pThreadData)
		return FALSE;
#ifdef _DEBUG
	if (pScriptEngineInstance != NULL && pScriptEngineInstanceData == NULL)
	{
		DumpEnginesList();
//		ODS(("SetCurEngineInstanceTls for %08X failed, CurInstance=%08X", pScriptEngineInstance, TlsGetValue(g_TlsCurEngineInstance)));
		DBG_STOP;
	}
	else
	{
//		if (pScriptEngineInstanceData != NULL)
//			ODS(("SetCurEngineInstanceTls instance %08X language %S", pScriptEngineInstance, pScriptEngineInstanceData->pwcsLanguageName));
//		else
//			ODS(("SetCurEngineInstanceTls instance NULL"));
	}
#endif
	pThreadData->pCurrentScriptEngineInstance = pScriptEngineInstance;
	return TRUE;
}
*/

PVOID GetCurEngineInstanceTls()
{
	SC_THREAD_DATA* pThreadData = GetSCThreadData();
	if (NULL == pThreadData)
		return NULL;
	if (pThreadData->dwCurEngineIndex >= MAX_CUR_ENGINES )
		return NULL;
	LPVOID pScriptEngineInstance = pThreadData->arrCurEngines[pThreadData->dwCurEngineIndex];
#ifdef _DEBUG
	if (GetScriptEngineInstanceData(pScriptEngineInstance) == NULL)
	{
		DumpEnginesList();
		ODS(("GetCurEngineInstanceTls returned unknown instance %08X", pScriptEngineInstance));
		DBG_STOP;
	}
#endif
	return pScriptEngineInstance;
}



HRESULT GetScriptThreadID(
    DWORD dwWin32ThreadID,       // Win32 thread identifier
    SCRIPTTHREADID *pstidThread  // receives scripting thread identifier
);


WCHAR* AddEngineScriptStr(LPVOID pEngineInstance, WCHAR* pScriptText)
{
	DWORD dwScriptTextLen;
	SCRIPT_ENGINE_INSTANCE_DATA* pScriptEngineInstanceData;
	WCHAR* pJoinedScriptText = NULL;
	DWORD  dwJoinedScriptTextSize;

//#define _TIMING_
	//	ODS(("AddCurEngineScriptStr"));
	
	if (pEngineInstance == NULL)
	{
		ODS(("??? GetCurEngineInstanceTls failed"));
		return NULL;
	}

	EnterCriticalSection(&g_CriticalSec);

	pScriptEngineInstanceData = GetScriptEngineInstanceData(pEngineInstance);
	if (pScriptEngineInstanceData == NULL)
	{
		ODS(("??? GetScriptEngineInstanceData failed"));
		LeaveCriticalSection(&g_CriticalSec);
		return NULL;
	}

	if (pScriptEngineInstanceData != NULL)
	{
		pJoinedScriptText = pScriptEngineInstanceData->wcsScriptText;
		dwJoinedScriptTextSize = pScriptEngineInstanceData->dwScriptTextSize;
		
		if (pScriptText == NULL)
		{
			LeaveCriticalSection(&g_CriticalSec);
			return pJoinedScriptText;
		}

		dwScriptTextLen = (DWORD)wcslen(pScriptText);
		if (pJoinedScriptText != NULL)
		{
			// realloc block
#if defined(_TIMING_) && defined(_DEBUG)
			// Init timing
			__int64 t;
			__int64 t1;
			static __int64 t2=0;
			static DWORD __cnt=0;
			//    SetPriorityClass(GetCurrentProcess(), REALTIME_PRIORITY_CLASS);
			//    SetThreadPriority(GetCurrentThread(), THREAD_PRIORITY_TIME_CRITICAL);
			QueryPerformanceCounter((LARGE_INTEGER*)&t);
#endif
			//DWORD oldLen = wcslen(pJoinedScriptText);
#if defined(_TIMING_) && defined(_DEBUG)
			// Calculate timing
			QueryPerformanceCounter((LARGE_INTEGER*)&t1);
			t=t1-t;
			t2+=t;
			ODS(("AddCurEngineScriptStr=%u %d", (DWORD)t2, ++__cnt));
			
			//    __int64 t2;
			
			//    QueryPerformanceFrequency ((LARGE_INTEGER*)&t);
			//        t2 = t2 / t.LowPart;
			//    printf(" = %f second(s)\n", t2);
			
#endif 
			DWORD newLen = dwJoinedScriptTextSize*2 + dwScriptTextLen*2 + 0x10;
			if (newLen > pScriptEngineInstanceData->dwScriptTextSizeAllocated)
			{
				newLen += 0x1000;
				pJoinedScriptText = (WCHAR*)HeapReAlloc(GetProcessHeap(), 0, pJoinedScriptText, newLen);
				pScriptEngineInstanceData->dwScriptTextSizeAllocated = newLen;
			}
			if (pJoinedScriptText != NULL)
			{
//				pJoinedScriptText[dwJoinedScriptTextSize] = L'\n';
				wcscpy(pJoinedScriptText+dwJoinedScriptTextSize, pScriptText);
				dwJoinedScriptTextSize += dwScriptTextLen;
			}
		}
		else
		{
			// alloc new block
			pScriptEngineInstanceData->dwScriptTextSizeAllocated = dwScriptTextLen*2 + 0x200;
			pJoinedScriptText = (WCHAR*)HeapAlloc(GetProcessHeap(), 0, pScriptEngineInstanceData->dwScriptTextSizeAllocated);
			if (pJoinedScriptText != NULL)
			{
				wcscpy(pJoinedScriptText, pScriptText);
				dwJoinedScriptTextSize = dwScriptTextLen;
			}
		}
		if (pJoinedScriptText != NULL)
		{
			//ODS(("AddScriptStr on ThreadID=%08X", GetCurrentThreadId()));
			pScriptEngineInstanceData->wcsScriptText = pJoinedScriptText;
			pScriptEngineInstanceData->dwScriptTextSize = dwJoinedScriptTextSize;
		}
		else
		{
			ODS(("AddCurEngineScriptStr on ThreadID=%08X failed!!!", GetCurrentThreadId()));
			pScriptEngineInstanceData->dwScriptTextSize = 0;
			pScriptEngineInstanceData->dwScriptTextSizeAllocated = 0;
		}
	}

	LeaveCriticalSection(&g_CriticalSec);
	return pJoinedScriptText;
}


WCHAR* GetEngineScriptStr(LPVOID pEngineInstance)
{
	SCRIPT_ENGINE_INSTANCE_DATA* pScriptEngineInstanceData;
	WCHAR* pJoinedScriptText = NULL;

	ODS(("GetCurEngineScriptStr"));
	
	EnterCriticalSection(&g_CriticalSec);

	pScriptEngineInstanceData = GetScriptEngineInstanceData(pEngineInstance);

	if (pScriptEngineInstanceData != NULL)
		pJoinedScriptText = pScriptEngineInstanceData->wcsScriptText;

	LeaveCriticalSection(&g_CriticalSec);

	return pJoinedScriptText;
}

void ScriptEngineReinit(LPVOID pEngineInstance)
{
	SCRIPT_ENGINE_INSTANCE_DATA* pScriptEngineInstanceData;
//	WCHAR* pJoinedScriptText = NULL;

	ODS(("FreeCurEngineScriptStr"));
	
	EnterCriticalSection(&g_CriticalSec);

	pScriptEngineInstanceData = GetScriptEngineInstanceData(pEngineInstance);

	if (pScriptEngineInstanceData != NULL)
	{
		FreeEngineScriptStr(pEngineInstance);
		if (pScriptEngineInstanceData->wcsURL != NULL)
		{
			SysFreeString(pScriptEngineInstanceData->wcsURL);
			pScriptEngineInstanceData->wcsURL = NULL;
		}
	}

	LeaveCriticalSection(&g_CriticalSec);

	return;
}



void FreeEngineScriptStr(LPVOID pEngineInstance)
{
	SCRIPT_ENGINE_INSTANCE_DATA* pScriptEngineInstanceData;
//	WCHAR* pJoinedScriptText = NULL;

	ODS(("FreeCurEngineScriptStr"));
	
	EnterCriticalSection(&g_CriticalSec);

	pScriptEngineInstanceData = GetScriptEngineInstanceData(pEngineInstance);

	if (pScriptEngineInstanceData != NULL)
	{
		if (pScriptEngineInstanceData->wcsScriptText != NULL)
		{
			HeapFree(GetProcessHeap(), 0, pScriptEngineInstanceData->wcsScriptText);
			pScriptEngineInstanceData->wcsScriptText = NULL;
			pScriptEngineInstanceData->dwScriptTextSize = 0;
			pScriptEngineInstanceData->dwScriptTextSizeAllocated = 0;
		}
	}

	LeaveCriticalSection(&g_CriticalSec);

	return;
}


