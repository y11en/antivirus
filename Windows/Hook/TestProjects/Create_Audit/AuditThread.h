#ifndef __AuditThread_
#define __AuditThread_

#include "windows.h"

#include "../../../hook/hook\FSDrvLib.h"
#include "../../../hook/hook\IDriver.h"
#include "../../../hook/hook\HookSpec.h"
#include "../../../hook/hook\Funcs.h"

#if defined( __cplusplus )
extern "C" 
{
#endif
	
typedef struct _THREAD_PARAM
{
	HWND				m_hWnd;
	HANDLE				m_hDevice;
	HANDLE				m_SyncObject_ClientStart;
	HANDLE				m_SyncObject_ClientExit;
	
	APP_REGISTRATION	m_AppReg;
	HANDLE				m_hWhistleup;
	HANDLE				m_hWFChanged;
	
	GET_EVENT			m_Event;
	SET_VERDICT			m_SetVerdict;
	OSVERSIONINFO*		m_pOSVer;
	BYTE				m_pEventData[4096];
}THREAD_PARAM, *PTHREAD_PARAM;

DWORD WINAPI EventWaiterThread(void* pParam);
//extern HANDLE g_hHeap;

HANDLE StartClient(PTHREAD_PARAM pThreadParam);
void StopClient(HANDLE hThread, PTHREAD_PARAM pThreadParam);

BOOL AddFilters(PTHREAD_PARAM pThreadParam);

#if defined( __cplusplus )
}
#endif

#endif //__AuditThread_