// CheckWindow.cpp: implementation of the CCheckWindow class.
//
//////////////////////////////////////////////////////////////////////
#include "CheckWindow.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

typedef struct _tCheckWindowsContext
{
	_tCheckWindowsContext()
	{
		m_ProcessId = 0;
		m_HasVisibleWindows = true;
		m_HasInvisibleWindows = false;
	}
	
	ULONG m_ProcessId;
	bool m_HasVisibleWindows;
	bool m_HasInvisibleWindows;
}CheckWindowsContext, *PCheckWindowsContext;
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
BOOL CALLBACK EnumWindowsProc(HWND hwnd, LPARAM lParam)
{
	PCheckWindowsContext pContext = (PCheckWindowsContext) lParam;

	ULONG ProcId;
	
	GetWindowThreadProcessId(hwnd, &ProcId);
	if (ProcId != pContext->m_ProcessId)
		return 1;

	HWND hWndParent = hwnd;
	do
	{
		hWndParent = GetParent(hwnd);
		if (hWndParent == NULL)
			break;
		hwnd = hWndParent;

	} while(true);

	
	RECT rect;
	if (GetWindowRect(hwnd, &rect))
	{
		if ((rect.left == rect.right) || (rect.top == rect.bottom))
			pContext->m_HasInvisibleWindows = true;
		else
		{
			if (IsWindowVisible(hwnd))
				pContext->m_HasVisibleWindows = true;
		}
	}
	
	return 1;
}

CCheckWindow::CCheckWindow()
{

}

CCheckWindow::~CCheckWindow()
{

}

void CCheckWindow::AlertInvisilbe(ULONG ProcessId)
{
	//empty func, to inherit
}

void CCheckWindow::CheckWindows(ULONG ProcessId)
{
	CheckWindowsContext Context;
	Context.m_ProcessId = ProcessId;
	Context.m_HasInvisibleWindows = false;

	EnumWindows(EnumWindowsProc, (LPARAM) &Context);
	// EnumDisplayMonitors

	if (!Context.m_HasVisibleWindows && Context.m_HasInvisibleWindows)
		AlertInvisilbe(ProcessId);
}

bool CCheckWindow::HasVisibleWindow(ULONG ProcessId)
{
	CheckWindowsContext Context;
	Context.m_ProcessId = ProcessId;

	EnumWindows(EnumWindowsProc, (LPARAM) &Context);
	return Context.m_HasVisibleWindows;
}