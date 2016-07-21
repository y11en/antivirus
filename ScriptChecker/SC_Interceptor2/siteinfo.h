#ifndef __siteinfo_h__
#define __siteinfo_h__

#ifdef __cplusplus
extern "C" {
#endif

#include "scr_inst.h"
#include <exdisp.h>

HWND GetExplorerStatusBarHWnd(IUnknown* pUnknown, IWebBrowser2** ppWebBrowser2);
BSTR GetSourceURL(IActiveScriptSite* pActiveScriptSite);
BSTR GetEventName(IActiveScriptSite* pActiveScriptSite);

#ifdef __cplusplus
} // extern "C"
#endif

#endif // __siteinfo_h__
 