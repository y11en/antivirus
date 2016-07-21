// ThModifiedProcess.cpp: implementation of the CThModifiedProcess class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "ThModifiedProcess.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//+ ------------------------------------------------------------------------------------------
DWORD WINAPI WndFunc_ModifiedProcess(LPVOID lpThreadParameter)
{
	ThreadSync* pThreadSync = (ThreadSync*) lpThreadParameter;
	if (pThreadSync == NULL)
		return -1;

	CThModifiedProcess* pThreadParam = (CThModifiedProcess*) pThreadSync->m_pThreadParam;

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

LRESULT CALLBACK RequestDlgProc_ModifiedProcess(HWND hWnd, UINT wMsg, WPARAM wParam, LPARAM lParam)
{
	Request_StartModifiedProcess* pRequest;
	switch (wMsg)
	{ 
	case WM_INITDIALOG:
		SetWindowLong(hWnd, DWL_USER, 0);
		
		{
			HWND hOncebutton = GetDlgItem(hWnd, IDC_BUTTON_ALLOWONCE);
			EnableWindow(hOncebutton, FALSE);
			ShowWindow(hOncebutton, SW_HIDE);
		}

		SetTimer(hWnd, _TIMER_MODIFIED_WND, 1000, NULL);
		break;
	case WM_TIMER:
		pRequest = (Request_StartModifiedProcess*) GetWindowLong(hWnd, DWL_USER);
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
		KillTimer(hWnd, _TIMER_MODIFIED_WND);
		break;
	case WM_CONTEXTMENU:
		break;
	case WM_PAINT:
		pRequest = (Request_StartModifiedProcess*) GetWindowLong(hWnd, DWL_USER);
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
		SendMessage(hWnd, WM_TIMER, _TIMER_MODIFIED_WND, 0);

		pRequest = (Request_StartModifiedProcess*) wParam;
		{
			CString strInfo;
			CString strtmp;

			strInfo = "Known file was modified. Do you want to start process from modified file?";
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

					pRequest = (Request_StartModifiedProcess*) GetWindowLong(hWnd, DWL_USER);
					if (pRequest != 0)
					{
						pRequest->m_bVerdict = false;
						SetWindowLong(hWnd, DWL_USER, 0);
						SetEvent(pRequest->m_hEventComplete);
					}
				}
				return TRUE;
			case IDOK:
				{
					ShowWindow(hWnd, SW_HIDE);

					pRequest = (Request_StartModifiedProcess*) GetWindowLong(hWnd, DWL_USER);
					if (pRequest != 0)
					{
						pRequest->m_bVerdict = true;
						SetWindowLong(hWnd, DWL_USER, 0);
						SetEvent(pRequest->m_hEventComplete);
					}
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

CThModifiedProcess::CThModifiedProcess()
{
}

CThModifiedProcess::~CThModifiedProcess()
{
}

bool CThModifiedProcess::StartLoop()
{
	ThreadSync ThSync(this);
	m_hThread = ThSync.StartThread(WndFunc_ModifiedProcess, m_hStopEvent);
	if (m_hThread == INVALID_HANDLE_VALUE)
		return false;

	return true;
}

void  CThModifiedProcess::StopLoop()
{
	SetEvent(m_hStopEvent);
	PostMessage(m_hWnd, WM_QUIT, 0, 0);
}

bool CThModifiedProcess::Init()
{
	m_hWnd = CreateDialogParam(AfxGetInstanceHandle(), MAKEINTRESOURCE(IDD_DIALOG_REQUEST_PROCESS_ALERT), NULL, 
		(DLGPROC) RequestDlgProc_ModifiedProcess, 0);

	if (m_hWnd == NULL)
		return false;

	return true;
}

void CThModifiedProcess::BeforeExit()
{
	if (m_hWnd != NULL)
		DestroyWindow(m_hWnd);
}

bool CThModifiedProcess::IsAllowStart(PWCHAR pwchImagePath)
{
	Request_StartModifiedProcess Request;
	Request.m_bVerdict = true;
	Request.m_pwchImagePath = pwchImagePath;
	Request.m_hEventComplete = CreateEvent(NULL, FALSE, FALSE, NULL);
	if (Request.m_hEventComplete != NULL)
	{
		CTimeSpan span(0, 0, 0, _TIMEOUT_SHOWMODIFIED_WND);
		Request.m_StartTime = Request.m_StartTime.GetCurrentTime();
		Request.m_StartTime += span;

		PostMessage(m_hWnd, WU_REQUESTUPDATE, (WPARAM) &Request, 0);
		
		WaitForSingleObject(Request.m_hEventComplete, INFINITE);
	}

	return Request.m_bVerdict;
}
