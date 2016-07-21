//////////////////////////////////////////////////////////////////////////////////////
// Agent interface implementation

#include "stdafx.h"
#include <stdio.h>
#include "AVP32.h"
#include "AVP32dlg.h"
#include "PID.h"
#include "agentint.h"

extern BOOL boProccessAndExit;

CAvp32Dlg* pwndMain;
HIPCSERVER hServer = NULL;
UINT LM_UPDATE_DATA = RegisterWindowMessage("LM_UPDATE_DATA");

DWORD g_dwTaskState = 0;

//HIPCSERVER hServer = NULL;

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
			boProccessAndExit = TRUE;

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
	pwndMain->CancelProcess();
	pwndMain->PostMessage(WM_CLOSE,0,0);
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
//	CCClientRegisterClientCallback(CCCC_TASK_STATE, (LPFNCCCLIENTCALLBACK)IPC_CommandGetTaskState);

    pwndMain = (CAvp32Dlg*)AfxGetApp()->m_pMainWnd;
    if (pwndMain)
    {
		CCClientInit(dwID, NULL);
		return TRUE;
	}

	return FALSE;
}

//////////////////////////////////////////////////////////////////////////////////////
// Agent interface implementation

int LoadSettings(LPPOLICY lpPolicy)
{
    if (lpPolicy)
    {
		LPPROPERTY p;
        // Locations ...................................................
        pwndMain->PathArray.RemoveAll();
		pwndMain->tPage1.SetPaths(pwndMain->PathArray);

		int i;
					p = GetPropertyByID(lpPolicy, PROPID_DW_LOCALDISKS);
		i=2;
		while ( i-- && (p) && ((int)*p->Data != ((CButton*)pwndMain->tPage1.GetDlgItem(IDC_LOCAL))->GetCheck()))
					pwndMain->tPage1.GetDlgItem(IDC_LOCAL)->SendMessage(BM_CLICK, 0, 0);
		p = GetPropertyByID(lpPolicy, PROPID_DW_FLOPPYDISKS);
		i=2;
		while ( i-- && (p) && ((int)*p->Data != ((CButton*)pwndMain->tPage1.GetDlgItem(IDC_FLOPPY))->GetCheck()))
					pwndMain->tPage1.GetDlgItem(IDC_FLOPPY)->SendMessage(BM_CLICK, 0, 0);
		p = GetPropertyByID(lpPolicy, PROPID_DW_NETDISKS);
		i=2;
		while ( i-- && (p) && ((int)*p->Data != ((CButton*)pwndMain->tPage1.GetDlgItem(IDC_NETWORK))->GetCheck()))
					pwndMain->tPage1.GetDlgItem(IDC_NETWORK)->SendMessage(BM_CLICK, 0, 0);
		
		pwndMain->tPage1.GetPaths(pwndMain->PathArray);

        p = GetPropertyByID(lpPolicy,PROPID_STR_DISKS);
        while(p)
        {
                pwndMain->PathArray.Add(p->Data);
                p = GetNextPropertyByID(p,PROPID_STR_DISKS);
        }
		pwndMain->tPage1.SetPaths(pwndMain->PathArray);

        // Objects .....................................................
        p = GetPropertyByID(lpPolicy,PROPID_DW_SCANMEM);
        if (p) pwndMain->tPage2.m_Memory = *((DWORD*)p->Data);
        p = GetPropertyByID(lpPolicy,PROPID_DW_SCANSECT);
        if (p) pwndMain->tPage2.m_Sectors = *((DWORD*)p->Data);
        p = GetPropertyByID(lpPolicy,PROPID_DW_SCANFILES);
        if (p) pwndMain->tPage2.m_Files = *((DWORD*)p->Data);
        p = GetPropertyByID(lpPolicy,PROPID_DW_SCANPACK);
        if (p) pwndMain->tPage2.m_Packed = *((DWORD*)p->Data);
        p = GetPropertyByID(lpPolicy,PROPID_DW_SCANARCH);
        if (p) pwndMain->tPage2.m_Archives = *((DWORD*)p->Data);
        p = GetPropertyByID(lpPolicy,PROPID_DW_SCANMAIL);
        if (p) pwndMain->tPage2.m_MailBases = *((DWORD*)p->Data);
        p = GetPropertyByID(lpPolicy,PROPID_DW_SCANMAILPLAIN);
        if (p) pwndMain->tPage2.m_MailPlain = *((DWORD*)p->Data);
        pwndMain->tPage2.m_Mask = 0;
        p = GetPropertyByID(lpPolicy,PROPID_DW_MASKPROG);
        if (p)
                if (*((DWORD*)p->Data) != 0) pwndMain->tPage2.m_Mask = 1;
        p = GetPropertyByID(lpPolicy,PROPID_DW_MASKALL);
        if (p)
                if (*((DWORD*)p->Data) != 0) pwndMain->tPage2.m_Mask = 2;
        p = GetPropertyByID(lpPolicy,PROPID_DW_MASKUSER);
        if (p)
                if (*((DWORD*)p->Data) != 0) pwndMain->tPage2.m_Mask = 3;
        p = GetPropertyByID(lpPolicy,PROPID_ST_MASKINC);
        if (p) pwndMain->tPage2.m_UserMask = p->Data;

        p = GetPropertyByID(lpPolicy,PROPID_DW_MASKEXC);
        if (p) pwndMain->tPage2.m_Exclude = *((DWORD*)p->Data);

        p = GetPropertyByID(lpPolicy,PROPID_ST_MASKEXC);
        if (p) pwndMain->tPage2.m_EditExclude = p->Data;

        // Actions ......................................................
        pwndMain->tPage6.m_Action = 0;
        p = GetPropertyByID(lpPolicy,PROPID_DW_DISDLG);
        if (p)
                if (*((DWORD*)p->Data) != 0) pwndMain->tPage6.m_Action = 1;
        p = GetPropertyByID(lpPolicy,PROPID_DW_DISAUTO);
        if (p)
                if (*((DWORD*)p->Data) != 0) pwndMain->tPage6.m_Action = 2;

        p = GetPropertyByID(lpPolicy,PROPID_DW_COPYINF);
        if (p) pwndMain->tPage6.m_CopyInfected = *((DWORD*)p->Data);
        p = GetPropertyByID(lpPolicy,PROPID_DW_COPYSUS);
        if (p) pwndMain->tPage6.m_CopySuspicion = *((DWORD*)p->Data);

        p = GetPropertyByID(lpPolicy,PROPID_ST_INFFOLDER);
        if (p) pwndMain->tPage6.m_sInfectFolder = p->Data;
        p = GetPropertyByID(lpPolicy,PROPID_ST_SUSFOLDER);
        if (p) pwndMain->tPage6.m_sSuspFolder = p->Data;
        // Options ......................................................
        p = GetPropertyByID(lpPolicy,PROPID_DW_WARNINGS);
        if (p) pwndMain->tPage3.m_Warnings = *((DWORD*)p->Data);
        p = GetPropertyByID(lpPolicy,PROPID_DW_CA);
        if (p) pwndMain->tPage3.m_CA = *((DWORD*)p->Data);
        p = GetPropertyByID(lpPolicy,PROPID_DW_REDUNDANT);
        if (p) pwndMain->tPage3.m_Redundant = *((DWORD*)p->Data);
        p = GetPropertyByID(lpPolicy,PROPID_DW_SHOWOK);
        if (p) pwndMain->tPage3.m_ShowOK = *((DWORD*)p->Data);
        p = GetPropertyByID(lpPolicy,PROPID_DW_SHOWPACK);
        if (p) pwndMain->tPage3.m_ShowPack = *((DWORD*)p->Data);
        p = GetPropertyByID(lpPolicy,PROPID_DW_SOUND);
        if (p) pwndMain->cust.m_Sound = *((DWORD*)p->Data);
        p = GetPropertyByID(lpPolicy,PROPID_DW_TRACK);
        if (p) pwndMain->m_Track = *((DWORD*)p->Data);

        p = GetPropertyByID(lpPolicy,PROPID_DW_REPORT);
        if (p) pwndMain->tPage3.m_Report = *((DWORD*)p->Data);
        p = GetPropertyByID(lpPolicy,PROPID_ST_REPORTFILE);
        if (p) pwndMain->tPage3.m_Reportfile = p->Data;
        p = GetPropertyByID(lpPolicy,PROPID_DW_APPEND);
        if (p) pwndMain->tPage3.m_Append = *((DWORD*)p->Data);

        p = GetPropertyByID(lpPolicy,PROPID_DW_LIMIT);
        if (p) pwndMain->tPage3.m_Limit = *((DWORD*)p->Data);
        p = GetPropertyByID(lpPolicy,PROPID_ST_LIMITSIZE);
        if (p) pwndMain->tPage3.m_Size = atol(p->Data);
        p = GetPropertyByID(lpPolicy,PROPID_DW_PRIORITY);
        if (p) pwndMain->tPage3.m_Priority = *(DWORD*)p->Data;


        p = GetPropertyByID(lpPolicy,PROPID_DW_REF_BYTIMER);
        if (p) propByTimer = *((DWORD*)p->Data);
        p = GetPropertyByID(lpPolicy,PROPID_DW_REF_BYEVENT);
        if (p) propByEvent = *((DWORD*)p->Data);
        p = GetPropertyByID(lpPolicy,PROPID_DW_EVENT);
        if (p) propEvent = *((DWORD*)p->Data);
        p = GetPropertyByID(lpPolicy,PROPID_DW_REF_INTERVAL);
        if (p) propInterval = *((DWORD*)p->Data);

        pwndMain->SendMessage(LM_UPDATE_DATA, 0, 0);

//		pwndMain->LoadKey(pwndMain->AvpKey);

	pwndMain->PostMessage(WM_COMMAND,ID_SCAN_NOW,0);

    }

        return 0;
}

