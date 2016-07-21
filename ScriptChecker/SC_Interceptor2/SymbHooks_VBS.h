#ifndef __SYMBHOOKS_VBS_H
#define __SYMBHOOKS_VBS

HRESULT __stdcall VBS_ExecuteHook(VARIANT* pResult, DWORD dwNameLen, VARIANT* pScriptCode);

SYM_HOOK VbsHooks[] = {
	L"Execute",			7,	0x9BFFD703,	VBS_ExecuteHook, 0, 0, 0, 0,
	L"ExecuteGlobal",	13,	0x2CE456E4,	VBS_ExecuteHook, 0, 0, 0, 0,
	L"Eval",			4,	0x00081E68,	VBS_ExecuteHook, 0, 0, 0, 0,
};

#endif // __SYMBHOOKS_VBS
