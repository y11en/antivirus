// ThNewProcess.cpp: implementation of the CThNewProcess class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "ThNewProcess.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//+ ------------------------------------------------------------------------------------------
DWORD WINAPI WndFunc_NewProcess(LPVOID lpThreadParameter)
{
	ThreadSync* pThreadSync = (ThreadSync*) lpThreadParameter;
	if (pThreadSync == NULL)
		return -1;

	CThNewProcess* pThreadParam = (CThNewProcess*) pThreadSync->m_pThreadParam;

	HANDLE hStopEvent = pThreadSync->m_hStopEvent;

	if (!pThreadParam->Init())
		return -1;

	pThreadSync->SetStartSucceed();

	HANDLE handles[1];
	handles[0] = hStopEvent;

	DWORD dwResult = WAIT_TIMEOUT;

	bool bExit = false;

	MSG msg;
	HWND hWnd = pThreadParam->GetHWND();
	while(GetMessage(&msg, hWnd, 0, 0)) 
	{
		if (!bExit)
			dwResult = WaitForMultipleObjects(sizeof(handles) / sizeof(handles[0]), handles, FALSE, 0);

		switch (dwResult)
		{
		case WAIT_OBJECT_0:
			pThreadParam->BeforeExit();
			bExit = true;
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


LRESULT CALLBACK RequestDlgProc_NewProcess(HWND hWnd, UINT wMsg, WPARAM wParam, LPARAM lParam)
{
	Request_StartProcess* pRequest;
	switch (wMsg)
	{ 
	case WM_INITDIALOG:
		SetWindowLong(hWnd, DWL_USER, 0);
		SetTimer(hWnd, _TIMER_NEWPROCESS_WND, 1000, NULL);
		break;
	case WM_TIMER:
		pRequest = (Request_StartProcess*) GetWindowLong(hWnd, DWL_USER);
		if (pRequest != NULL)
		{
			CTime Current = Current.GetCurrentTime();
			CTimeSpan spanCurrent(Current.GetTime());

			if (pRequest->m_StartTime <= Current)
				PostMessage(hWnd, WM_COMMAND, IDOK, 0);
			else
			{
				CTimeSpan span(pRequest->m_StartTime.GetTime());

				CString strtmp;
				span -= spanCurrent;
				strtmp = span.Format("time elapse %M min %S (sec)");
				
				SetDlgItemText(hWnd, IDC_STATIC_TIME_ELAPSE, strtmp);
			}
		}
		return TRUE;
		break;
	case WM_CLOSE:
		break;
	case WM_DESTROY:
		KillTimer(hWnd, _TIMER_NEWPROCESS_WND);
		break;
	case WM_CONTEXTMENU:
		break;
	case WM_PAINT:
		pRequest = (Request_StartProcess*) GetWindowLong(hWnd, DWL_USER);
		if (pRequest != NULL)
		{
			PAINTSTRUCT paint;
			HDC hdc = BeginPaint(hWnd, &paint);
			HICON hIcon = pRequest->wininfo.GetIcon(true);
			if (hIcon != NULL)
			{
				RECT rect;
				GetWindowRect(GetDlgItem(hWnd, IDC_STATIC_FILEICON), &rect);
				MapWindowPoints(0, hWnd, (LPPOINT) &rect, 2);
				DrawIcon(hdc, rect.left, rect.top, hIcon);
			}
			EndPaint(hWnd, &paint);
		}
		return TRUE;
		break;
	case WU_REQUESTUPDATE:
		SetWindowLong(hWnd, DWL_USER, wParam);
		SendMessage(hWnd, WM_TIMER, _TIMER_NEWPROCESS_WND, 0);

		pRequest = (Request_StartProcess*) wParam;
		{
			CString strInfo;
			CString strtmp;

			strInfo = *pRequest->m_pstrInfo;
			strInfo += _T("\r\n\r\n");

			if (pRequest->wininfo.QueryInfo(pRequest->m_pwchImagePath))
			{
				strtmp.Format(_T("Image path: %s\r\nVersion: <unknown>\r\nDescription: <not present>"), 
					pRequest->m_pwchImagePath);
			}
			else
			{
				strtmp.Format(_T("Image path: %s\r\nVersion: %s\r\nDescription: %s"), 
					pRequest->m_pwchImagePath, 
					pRequest->wininfo.GetProductVersion(),
					pRequest->wininfo.GetFileDescription());
			}

			strInfo += strtmp;

			SetDlgItemText(hWnd, IDC_EDIT_INFO, strInfo);

			if (pRequest->m_bSandBoxed)
				EnableWindow(GetDlgItem(hWnd, IDOK), FALSE);
		}
		
		ShowWindow(hWnd, SW_SHOW);
		SetForegroundWindow(hWnd);
		SetFocus(hWnd);
		UpdateWindow(hWnd);

		return TRUE;
		break;
	case WM_COMMAND:
		{
			switch (LOWORD (wParam)) 
			{
			case IDCANCEL:
				{
					ShowWindow(hWnd, SW_HIDE);

					pRequest = (Request_StartProcess*) GetWindowLong(hWnd, DWL_USER);
					if (pRequest != 0)
					{
						pRequest->m_Verdict = _start_disabled;
						SetWindowLong(hWnd, DWL_USER, 0);
						SetEvent(pRequest->m_hEventComplete);
					}
				}
				return TRUE;
			case IDOK:
				{
					ShowWindow(hWnd, SW_HIDE);

					pRequest = (Request_StartProcess*) GetWindowLong(hWnd, DWL_USER);
					if (pRequest != 0)
					{
						pRequest->m_Verdict = _start_allowed;
						SetWindowLong(hWnd, DWL_USER, 0);
						SetEvent(pRequest->m_hEventComplete);
					}
				}
				return TRUE;
			case IDC_BUTTON_ALLOWONCE:
				{
					ShowWindow(hWnd, SW_HIDE);
					
					pRequest = (Request_StartProcess*) GetWindowLong(hWnd, DWL_USER);
					pRequest->m_Verdict = _start_allowed_once;
					SetWindowLong(hWnd, DWL_USER, 0);
					SetEvent(pRequest->m_hEventComplete);
				}
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

CThNewProcess::CThNewProcess()
{
}

CThNewProcess::~CThNewProcess()
{
}

bool CThNewProcess::StartLoop()
{
	ThreadSync ThSync(this);
	m_hThread = ThSync.StartThread(WndFunc_NewProcess, m_hStopEvent);
	if (m_hThread == INVALID_HANDLE_VALUE)
		return false;

	return true;
}

void  CThNewProcess::StopLoop()
{
	SetEvent(m_hStopEvent);
	PostMessage(m_hWnd, WM_QUIT, 0, 0);
}

bool CThNewProcess::Init()
{
	m_hWnd = CreateDialogParam(AfxGetInstanceHandle(), MAKEINTRESOURCE(IDD_DIALOG_REQUEST_PROCESS_ALERT), NULL, 
		(DLGPROC) RequestDlgProc_NewProcess, 0);

	if (m_hWnd == NULL)
		return false;

	return true;
}

void CThNewProcess::BeforeExit()
{
	if (m_hWnd != NULL)
		DestroyWindow(m_hWnd);
}

_eStartNewProcess_Verdict CThNewProcess::IsAllowStart(CString *pStrInfo, PWCHAR pwchImagePath, bool bSandBoxed)
{
	Request_StartProcess Request;
	Request.m_Verdict = _start_allowed_once;
	Request.m_pstrInfo = pStrInfo;
	Request.m_pwchImagePath = pwchImagePath;
	Request.m_bSandBoxed = bSandBoxed;
	Request.m_hEventComplete = CreateEvent(NULL, FALSE, FALSE, NULL);
	if (Request.m_hEventComplete != NULL)
	{
		CTimeSpan span(0, 0, 0, _TIMEOUT_SHOWNEWPROCESS_WND);
		Request.m_StartTime = Request.m_StartTime.GetCurrentTime();
		Request.m_StartTime += span;

		PostMessage(m_hWnd, WU_REQUESTUPDATE, (WPARAM) &Request, 0);
		
		WaitForSingleObject(Request.m_hEventComplete, INFINITE);
	}

	return Request.m_Verdict;
}
