#ifndef __HOOKS_H
#define __HOOKS_H
//-------------------------------------------------------------

typedef struct _HOOK_CONTEXT
{
	PVOID*		m_pApiPtr;
	PVOID		m_OldApiPtr;
} HOOK_CONTEXT, *PHOOK_CONTEXT;

BOOL HookBaseInit();
VOID HookBaseDone();
BOOL SetDispatchHooks();
VOID RemoveDispatchHooks();
extern "C" __declspec(dllexport) VOID WINAPI SetClientVerdict(VERDICT Verdict);

#endif
