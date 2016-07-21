#ifndef _SC_PLUGINS_H
#define _SC_PLUGINS_H

#include <windows.h>
#include "scripting/activscp.h"
#include "scplugin.h"

#ifdef __cplusplus
extern "C" {
#endif

BOOL ScPluginsIsEnabled();
BOOL ScPluginsProcessScript(WCHAR* pwcsScript, WCHAR* pwcsLanguage, WCHAR* pwcsURL, IActiveScript* pActiveScript, DWORD* pdwFlags);
BOOL ScPluginsCheckPopupURL(DWORD dwAction, WCHAR* pwcsSourceURL, WCHAR* pwcsDestinationURL, DWORD* pdwFlags, tCheckPopupURLCallbackFn pCallbackFn, LPVOID pCallbackCtx);

BOOL ScPluginsIsDispatchEnabled(DWORD dwActionId);
BOOL ScPluginsDispatch(DWORD dwActionId, void* pActionDataStruct, DWORD dwActionDataSize, DWORD* pdwFlags);

void ScPluginsUnload();

#ifdef __cplusplus
} // extern "C" 
#endif


#endif _SC_PLUGINS_H