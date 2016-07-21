#ifndef _SC_PLUGIN_H
#define _SC_PLUGIN_H

#include <windows.h>
#include <activscp.h>

#ifdef __cplusplus
extern "C" {
#endif

#define KL_REG_KEY  "Software\\KasperskyLab"
#define COMPONENTS_REG_KEY  KL_REG_KEY "\\Components"
#define SC_REG_SUBKEY       "SC"
#define SC_REG_KEY          COMPONENTS_REG_KEY "\\" SC_REG_SUBKEY
#define SC_REG_PLUGINS_SUBKEY "Plugins"
#define SC_REG_PLUGINS      SC_REG_KEY "\\" SC_REG_PLUGINS_SUBKEY
#define SC_REGVAL_PRIORITY  "Priority"
#define SC_REGVAL_ENABLED   "Enabled"
#define SC_REGVAL_RELOAD    "Reload"

typedef BOOL (__stdcall *tIsEnabled)(void);
typedef BOOL (__stdcall *tProcessScript)(WCHAR* pwcsScript, WCHAR* pwcsLanguage, WCHAR* pwcsURL, IActiveScript* pActiveScript, DWORD* pdwFlags);
typedef BOOL (__stdcall *tCheckPopupURLCallbackFn)(LPVOID pCallbackCtx, DWORD dwAction, WCHAR* pwcsSourceURL);
typedef BOOL (__stdcall *tCheckPopupURL)(DWORD dwAction, WCHAR* pwcsSourceURL, WCHAR* pwcsDestinationURL, DWORD* pdwFlags, tCheckPopupURLCallbackFn pCallbackFn, LPVOID pCallbackCtx);
typedef BOOL (__stdcall *tIsDispatchEnabled)(DWORD dwActionId);
typedef HRESULT (__stdcall *tDispatch)(DWORD dwActionId, void* pActionDataStruct, DWORD dwActionDataSize, DWORD* pdwFlags);

#define SCP_BLOCK           (DWORD)0x01
#define SCP_BLOCK_FURTHER   (DWORD)0x02
#define SCP_WHITELISTED     (DWORD)0x04

#define SCP_ACTION_POPUP_CHECK_DENY           (DWORD)0x01
#define SCP_ACTION_POPUP_TEMPORARY_ALLOWED    (DWORD)0x02
#define SCP_ACTION_POPUP_ADD_TRUSTED_SITE     (DWORD)0x03


#ifdef __cplusplus
} // extern "C" 
#endif


#endif _SC_PLUGIN_H