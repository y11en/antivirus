#include <windows.h>
#include "scripting/activscp.h"
#include "debug.h"
#include "../../windows/hook/r3hook/hookbase64.h"
#include "scr_inst.h"
#include "ChkScript.h"

HRESULT __stdcall VBS_ExecuteHook(VARIANT* pResult, DWORD dwNameLen, VARIANT* pVarScriptCode)
{
	HRESULT hRes = S_OK;
	void* OrigFuncAddr = HookGetOrigFunc();

	if (pVarScriptCode->vt == VT_BSTR) 
	{
		if (FAILED(CheckScriptText(GetCurEngineInstanceTls(), (WCHAR*)pVarScriptCode->pbstrVal)))
		{
			ODS(("VBS EXECUTE script blocked"));
			return E_FAIL;
		}
		ODS(("VBS EXECUTE passed."));
	}
	else
	{
		ODS(("VBS EXECUTE parameter has unsupported type %X, passed.", pVarScriptCode->vt));
	}
	
	hRes = ((HRESULT (__stdcall *)(VARIANT*, DWORD, VARIANT*))OrigFuncAddr)(pResult, dwNameLen, pVarScriptCode);

	return hRes;
}

