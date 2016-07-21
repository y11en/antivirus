// ThCMD_Remote.cpp: implementation of the CThCMD_Remote class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "ThCMD_Remote.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//+ ------------------------------------------------------------------------------------------
DWORD WINAPI WndFunc_CMD_Remote(LPVOID lpThreadParameter)
{
	ThreadSync* pThreadSync = (ThreadSync*) lpThreadParameter;
	if (pThreadSync == NULL)
		return -1;

	CThCMD_Remote* pThreadParam = (CThCMD_Remote*) pThreadSync->m_pThreadParam;

	HANDLE hStopEvent = pThreadSync->m_hStopEvent;

	if (!pThreadParam->Init())
		return -1;

	pThreadSync->SetStartSucceed();

	HANDLE handles[2];
	handles[0] = hStopEvent;
	handles[1] = pThreadParam->GetWarningEvent();

	DWORD dwResult = WAIT_TIMEOUT;

	bool bExit = false;

	MSG msg;
	HWND hWnd = pThreadParam->GetHWND();
	while(GetMessage(&msg, hWnd, 0, 0))
	{
		if (!bExit)
			dwResult = WaitForMultipleObjects(sizeof(handles) / sizeof(handles[0]), handles, FALSE, 0);
		else
			break;

		switch (dwResult)
		{
		case WAIT_OBJECT_0:
			pThreadParam->BeforeExit();
			bExit = true;
			OutputDebugString(_T("before exit!\n"));
			break;
		case WAIT_OBJECT_0 + 1:
		default:
			if (!IsWindowVisible(hWnd))
			{
				CString *pstrtmp = new CString;
				if (pstrtmp == NULL)
				{
					//! low resource
					pThreadParam->PopEvent(pstrtmp);
					delete pstrtmp;
				}
				else
				{
					if (!pThreadParam->PopEvent(pstrtmp))
						delete pstrtmp;
					else
					{
						SetWindowLong(hWnd, DWL_USER, (LONG) pstrtmp);
						ShowWindow(hWnd, SW_SHOW);
						SetForegroundWindow(hWnd);
					}
				}
			}
			break;
		}

		if (!IsDialogMessage(hWnd, &msg))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
	}

	return 0;
}


LRESULT CALLBACK RequestDlgProc_CMD_Remoter(HWND hWnd, UINT wMsg, WPARAM wParam, LPARAM lParam)
{
	switch (wMsg)
	{ 
	case WM_INITDIALOG:
		SetWindowLong(hWnd, DWL_USER, 0);
		SetTimer(hWnd, _TIMER_CMDREMOTE, 1000, NULL);
		break;
	case WM_CLOSE:
		break;
	case WM_DESTROY:
		KillTimer(hWnd, _TIMER_CMDREMOTE);
		break;
	case WM_TIMER:
		break;
	case WM_CONTEXTMENU:
		break;
	case WM_SHOWWINDOW:
		if (wParam == TRUE)
		{
			CString *pstrtmp = (CString*) GetWindowLong(hWnd, DWL_USER);
			if (pstrtmp == NULL)
			{
				ShowWindow(hWnd, SW_HIDE);
			}
			else
			{
				SetDlgItemText(hWnd, IDC_EDIT_WARNING, *pstrtmp);
			}
		}
		break;
	case WM_COMMAND:
		{
			switch (LOWORD (wParam)) 
			{
			case IDCANCEL:
			case IDOK:
				{
					CString *pstrtmp = (CString*) GetWindowLong(hWnd, DWL_USER);
					if (pstrtmp != NULL)
					{
						delete pstrtmp;
						SetWindowLong(hWnd, DWL_USER, 0);
					}
				}
				ShowWindow(hWnd, SW_HIDE);
				return TRUE;
			}
			break;
		}
		break;
	}
	return FALSE;
}

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
CThCMD_Remote::CThCMD_Remote()
{
	m_hWarningEvent = CreateEvent(NULL, FALSE, FALSE, NULL);
}

CThCMD_Remote::~CThCMD_Remote()
{
	if (m_hWarningEvent)
		CloseHandle(m_hWarningEvent);
}

bool CThCMD_Remote::StartLoop()
{
	ThreadSync ThSync(this);

	if (!m_hWarningEvent)
		return false;

	m_hThread = ThSync.StartThread(WndFunc_CMD_Remote, m_hStopEvent);
	if (m_hThread == INVALID_HANDLE_VALUE)
		return false;

	return true;
}

void  CThCMD_Remote::StopLoop()
{
	SetEvent(m_hStopEvent);
	PostMessage(m_hWnd, WM_QUIT, 0, 0);
}

bool CThCMD_Remote::Init()
{
	m_hWnd = CreateDialogParam(AfxGetInstanceHandle(), MAKEINTRESOURCE(IDD_DIALOG_CMD_REMOTE), NULL, 
		(DLGPROC) RequestDlgProc_CMD_Remoter, 0);

	if (m_hWnd == NULL)
		return false;

	return true;
}

void CThCMD_Remote::BeforeExit()
{
	if (m_hWnd != NULL)
		DestroyWindow(m_hWnd);
}

void CThCMD_Remote::ShowWarning(CString *pWarning)
{
	m_Warnings.PushEvent(pWarning);
	SetEvent(m_hWarningEvent);
}
