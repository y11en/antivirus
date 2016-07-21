#ifndef __SYMBHOOKS_JS_H
#define __SYMBHOOKS_JS

#include "../../windows/hook/r3hook/hookbase64.h"

HRESULT __stdcall JS_EvalHook(PVOID pSession, VARIANT* pGlobalBinder, VARIANT* pUnknown, DWORD dwParamCount, VARIANT* pParams);

SYM_HOOK JsHooks[] = {
	L"eval", 4, 0x00081E68, JS_EvalHook, 0, 0, 0, 0,
};

#endif // __SYMBHOOKS_JS