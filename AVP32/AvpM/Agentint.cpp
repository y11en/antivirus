//////////////////////////////////////////////////////////////////////////////////////
// Agent interface implementation

#include "stdafx.h"
#include <stdio.h>
#include "agentint.h"

//UINT LM_UPDATE_DATA = RegisterWindowMessage("LM_UPDATE_DATA");
DWORD g_dwTaskState = 0;

extern HWND hMainDlg;
extern int LoadSettings(LPPOLICY lpPolicy);

/////////////////////////////////////////////////////////////////////////////
//
// CC Client Callback functions
//
/////////////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////////////
// IPC_CommandTaskStart - Load new settings from CC and start task execution
// -------------------------------------------------------------------------
// return:				TRUE - if successfull, FALSE - otherwise.
BOOL CALLBACK IPC_CommandTaskStart(HIPCCONNECTION hConnection, LPPOLICY pPolicy)
{
	LPPROPERTY p = GetPropertyByID((LPPOLICY)pPolicy, 1002);
	if (p)
	{
		if (LoadSettings((LPPOLICY)p->Data) == 0)
		{
			g_dwTaskState = TS_RUNNING;
			CCClientSendStatus(TS_RUNNING);

			return TRUE;
		}
	}

	return FALSE;
}

/////////////////////////////////////////////////////////////////////////////
// IPC_CommandTaskStop - Stop task execution and unload component from memory
// -------------------------------------------------------------------------
// return:				TRUE - if successfull, FALSE - otherwise.
BOOL CALLBACK IPC_CommandTaskStop(HIPCCONNECTION hConnection, LPPOLICY pPolicy)
{
    g_dwTaskState = TS_ABORTED;
    LPPROPERTY p = GetPropertyByID((LPPOLICY)pPolicy, 1001);
	if(hMainDlg)
		PostMessage(hMainDlg, WM_CLOSE, 0, 0);
    return TRUE;
}

/////////////////////////////////////////////////////////////////////////////
// IPC_CommandTaskPause - Pause task execution (not implemented)
// -------------------------------------------------------------------------
// return:				TRUE - if successfull, FALSE - otherwise.
BOOL CALLBACK IPC_CommandTaskPause(HIPCCONNECTION hConnection, LPPOLICY pPolicy)
{
	return FALSE;
}

/////////////////////////////////////////////////////////////////////////////
// IPC_CommandTaskResume - Resume task execution (not implemented)
// -------------------------------------------------------------------------
// return:				TRUE - if successfull, FALSE - otherwise.
BOOL CALLBACK IPC_CommandTaskResume(HIPCCONNECTION hConnection, LPPOLICY pPolicy)
{
	return FALSE;
}

/////////////////////////////////////////////////////////////////////////////
// IPC_CommandReloadBases - Reload bases
// -------------------------------------------------------------------------
// return:				TRUE - if successfull, FALSE - otherwise.
extern UINT WM_AVPUPD;
BOOL CALLBACK IPC_CommandReloadBases(HIPCCONNECTION hConnection, LPPOLICY pPolicy)
{
	PostMessage(hMainDlg,WM_AVPUPD,0,0);
	return TRUE;
}

/////////////////////////////////////////////////////////////////////////////
// IPC_CommandGetTaskState - Get current state of the task
// -------------------------------------------------------------------------
// return:				TRUE - if successfull, FALSE - otherwise.
/*
BOOL CALLBACK IPC_CommandGetTaskState(HIPCCONNECTION hConnection, LPPOLICY pPolicy)
{
	int rc = FALSE;

	HPOLICY hPolicy = CreatePolicy(PID_IPC_COMMAND_TASK_STATE, 0, PROP_SIZE_DWORD);
	CreatePropertyDWORD(hPolicy, 1001, g_dwTaskState);
	LPPOLICY pTmpPolicy = ClosePolicy(hPolicy, NULL);
	if (pTmpPolicy)
	{
		switch (CCClientSendDataToCC(hConnection, pTmpPolicy, pTmpPolicy->Size, 5000)) {
			case -1:
			case -2:
			break;
			default:
				rc = TRUE;
		}

		free(pTmpPolicy);
	}

	return rc;
}
*/
/////////////////////////////////////////////////////////////////////////////////////////
// InitCCClientInterface - Publics 
BOOL InitCCClientInterface(DWORD dwID)
{
	CCClientRegisterClientCallback(CCCC_TASK_START, (LPFNCCCLIENTCALLBACK)IPC_CommandTaskStart);
	CCClientRegisterClientCallback(CCCC_TASK_STOP, (LPFNCCCLIENTCALLBACK)IPC_CommandTaskStop);
	CCClientRegisterClientCallback(CCCC_TASK_PAUSE, (LPFNCCCLIENTCALLBACK)IPC_CommandTaskPause);
	CCClientRegisterClientCallback(CCCC_TASK_RESUME, (LPFNCCCLIENTCALLBACK)IPC_CommandTaskResume);
	CCClientRegisterClientCallback(CCCC_TASK_RELOAD_BASES, (LPFNCCCLIENTCALLBACK)IPC_CommandReloadBases);

//    if (hMainDlg)
    {
		CCClientInit(dwID, "AVPM");
		return TRUE;
	}

	return FALSE;
}




