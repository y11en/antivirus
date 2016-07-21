#include <windows.h>
#include "debug.h"
#include "../../windows/hook/r3hook/hookbase64.h"
#include "scr_inst.h"
#include "ChkScript.h"

// Script specific variant types
#define VT_SCRIPT_OBJECT	0x49 
#define VT_SCRIPT_DISPATCH	0x4A
#define VT_SCRIPT_UNKNOWN	0x400C
#define VT_SCRIPT_BSTR		0x4B
// IE7 changes - THEY CHANGED INTERNAL VARIANT TYPES TO AVOID CONFLICT WITH NEW STANDARD TYPES (VT_VERSIONED_STREAM)
#define VT_SCRIPT_OBJECT2	0x80
#define VT_SCRIPT_DISPATCH2	0x81
#define VT_SCRIPT_BSTR2		0x82	

BOOL JS_Eval_NeedToCheck(WCHAR* wcEvalString)
{
//	int nStrLen = wcslen(wcEvalString);
	DWORD i;
	BOOL bNeedCheck = FALSE;

/*
	// Check for simple scripts (just for optimization)
	if (nStrLen > 0x80) 
		bNeedCheck = TRUE;
	else if (nStrLen < 30)
		bNeedCheck = FALSE;
	else
	{
		bNeedCheck = FALSE;
		int i;
		if (nStrLen > 30) // for very short scripts only
		{
			for (i=0; i<nStrLen; i++)
			{
			
//				if (pScriptText[i] >= 'A' && pScriptText[i] <= 'Z')
//					continue;
//				if (pScriptText[i] >= 'a' && pScriptText[i] <= 'z')
//					continue;
//				if (pScriptText[i] >= '0' && pScriptText[i] <= '9')
//					continue;
//				if (pScriptText[i] == '[' || pScriptText[i] == ']' || pScriptText[i] == '_')
//					continue;
				
				if (pScriptText[i] != '.') //&& pScriptText[i] != '(' && pScriptText[i] != ')'
					continue;
				bNeedCheck = TRUE;
				break;
			}
		}
	}
*/
	i=0;
	while (wcEvalString[i] != 0)
	{
		if ((wcEvalString[i]|0x20)  == L's')  // .Send
		{
			if ((wcEvalString[i+1]!=0 && (wcEvalString[i+1]|0x20)==L'e') 
				&& (wcEvalString[i+2]!=0 && (wcEvalString[i+2]|0x20)==L'n') 
				&& (wcEvalString[i+3]!=0 && (wcEvalString[i+3]|0x20)==L'd'))
			{
				bNeedCheck = TRUE;	
				break;
			}
		}
		i++;
	}
	if (wcEvalString[0] == '\'' && 
		wcEvalString[1] == 'X' && 
		wcEvalString[2] == '5' && 
		wcEvalString[3] == 'O')
		bNeedCheck = TRUE;	

	return bNeedCheck;
}

HRESULT __stdcall JS_EvalHook(PVOID pSession, VARIANT* pGlobalBinder, VARIANT* pResult, DWORD dwParamCount, VARIANT* pParams)
{
	void* OrigFuncAddr = HookGetOrigFunc();

//#define _TIMING_
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

	HRESULT hRes = S_OK;
	WCHAR* pScriptText;
	VARIANT* pVarArg = NULL;

	if (dwParamCount>0)  
	{
		pVarArg = &pParams[dwParamCount-1];
		if (pVarArg->vt == VT_SCRIPT_OBJECT || pVarArg->vt == VT_SCRIPT_OBJECT2)
			pVarArg = pVarArg->pvarVal;
		
		if (pVarArg->vt == VT_BSTR || pVarArg->vt == VT_SCRIPT_BSTR || pVarArg->vt == VT_SCRIPT_BSTR2 )
		{
			pScriptText = pVarArg->bstrVal;
			//ODS(("JS EVAL script: %S", pbstrScriptCode));
			
			if (pScriptText != NULL)
			{
				if (JS_Eval_NeedToCheck(pScriptText) == FALSE)
				{
					//ODS(("JS EVAL need no check."));
					// AddEngineScriptStr(GetCurEngineInstanceTls(), (WCHAR*)pScriptText);
				}
				else
				{
					if (FAILED(CheckScriptText(GetCurEngineInstanceTls(), pScriptText)))
					{
						ODS(("JS EVAL script blocked"));
						return E_FAIL;
					}
				}
				
			}
		}
		else
		{
			ODS(("JS EVAL parameter has unsupported type %X, passed.", pParams[dwParamCount-1].vt));
		}
	}
	else
	{
		ODS(("JS EVAL has no parameters, passed."));
	}
//	ODS(("JS EVAL passed."));
#if defined(_TIMING_) && defined(_DEBUG)
	// Calculate timing
	QueryPerformanceCounter((LARGE_INTEGER*)&t1);
	t=t1-t;
	t2+=t;
	ODS(("JS_EvalHook=%u %d", (DWORD)t2, ++__cnt));
	
	//    __int64 t2;
	
	//    QueryPerformanceFrequency ((LARGE_INTEGER*)&t);
	//        t2 = t2 / t.LowPart;
	//    printf(" = %f second(s)\n", t2);
	
#endif 
	hRes = ((HRESULT (__stdcall *)(PVOID, VARIANT*, VARIANT*, DWORD, VARIANT*))OrigFuncAddr)(pSession, pGlobalBinder, pResult, dwParamCount, pParams);


	return hRes;
}

