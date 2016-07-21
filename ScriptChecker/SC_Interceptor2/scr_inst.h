#ifndef __SRC_INST_H
#define __SRC_INST_H

#include "scripting/activscp.h"
#include "scripting/activdbg.h"
#include <mshtml.h>

#if defined(__cplusplus)
extern "C" {
#endif

typedef struct _SCRIPT_ENGINE_INSTANCE_DATA {
	IActiveScript* pActiveScript;
	IActiveScriptParse* pActiveScriptParse;
	IActiveScriptSite* pActiveScriptSite;
	IActiveScriptSiteDebug* pActiveScriptSiteDebug;
	HWND hWndStatusBar;
	WCHAR* wcsURL;
	DWORD  dwScriptsCount;
	DWORD  dwEntryCount;
	
	WCHAR* wcsScriptText;
	DWORD dwScriptTextSize;
	DWORD dwScriptTextSizeAllocated;
	IHTMLDocument2* pDelayedWriteDoc;
	LONG  bPassDocumentWrite;
	
	WCHAR* pwcsLanguageName;
	BOOL  bEncoded;
	BOOL  bDenyExecution;
	HRESULT bSupressErrorCode;
	_SCRIPT_ENGINE_INSTANCE_DATA* pNext;
} SCRIPT_ENGINE_INSTANCE_DATA, *PSCRIPT_ENGINE_INSTANCE_DATA;


#define MAX_CUR_ENGINES 0x10

typedef struct _SC_THREAD_DATA {
	LPVOID pCurrentScriptEngineInstance; // ActiveScriptSite or ActiveScriptParse
	BSTR   bstrEventName; 
	DWORD  dwEntryCount;
	BOOL   bNavigate2Script;
	LPVOID arrCurEngines[MAX_CUR_ENGINES];
	DWORD  dwCurEngineIndex;
} SC_THREAD_DATA;

SC_THREAD_DATA* GetSCThreadData();

extern SCRIPT_ENGINE_INSTANCE_DATA* g_pScriptEngineInstanceDataFirst;
//extern DWORD g_TlsCurEngineInstance;
extern DWORD g_TlsSCThreadData;

void   ScriptEngineListInit();
void   ScriptEngineListDone();

SCRIPT_ENGINE_INSTANCE_DATA* AddScriptEngineInstanceData();
SCRIPT_ENGINE_INSTANCE_DATA* GetScriptEngineInstanceData(PVOID pScriptEngineInstance);
BOOL   DeleteScriptEngineInstanceData(SCRIPT_ENGINE_INSTANCE_DATA* pCurScriptEngineInstanceData);

//BOOL   SetCurEngineInstanceTls(PVOID pScriptEngineInstance);
PVOID  GetCurEngineInstanceTls();

void ScriptEngineReinit(LPVOID pEngineInstance);
WCHAR* AddEngineScriptStr(LPVOID pEngineInstance, WCHAR* pScriptText);
WCHAR* GetEngineScriptStr(LPVOID pEngineInstance);
void   FreeEngineScriptStr(LPVOID pEngineInstance);

SC_THREAD_DATA* GetSCThreadData();
void FreeSCThreadData();

void DumpEnginesList();

#if defined(__cplusplus)
} // extern "C"
#endif

#endif // __SRC_INST_H