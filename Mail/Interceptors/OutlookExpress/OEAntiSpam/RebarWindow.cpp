// RebarWindow.cpp: implementation of the CRebarWindow class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "RebarWindow.h"
#include "MainWindow.h"
#include "resource.h"

CRebarWindow::CRebarWindow() :
	m_bInited(false)
{
}

CRebarWindow::~CRebarWindow()
{
}

void CRebarWindow::Initialize()
{
	if(!m_bInited && _Module.m_GuiLoader.GetRoot())
	{
		m_strSpamTooltipW = _Module.LoadStringW("Report_Body.ReportDetails.antispam.Events.menu", "TrainAsSpam");
		m_strProbSpamTooltipW = _Module.LoadStringW("Report_Body.ReportDetails.antispam.Events.menu", "TrainAsHam");
		m_strConfigTooltipW = _Module.LoadStringW("Report_Body.ReportDetails.antispam.Events.menu", "ASConfig");
		m_strSpamTooltipA = _Module.LoadString("Report_Body.ReportDetails.antispam.Events.menu", "TrainAsSpam");
		m_strProbSpamTooltipA = _Module.LoadString("Report_Body.ReportDetails.antispam.Events.menu", "TrainAsHam");
		m_strConfigTooltipA = _Module.LoadString("Report_Body.ReportDetails.antispam.Events.menu", "ASConfig");
		m_bInited = true;
	}
}

LRESULT CRebarWindow::OnWmNotify(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	bHandled = FALSE;

	LPNMHDR lpnm = (LPNMHDR) lParam;

	Initialize();

	if (lpnm)
	{
		if(lpnm->code == TTN_NEEDTEXTA)
		{
			LPNMTTDISPINFOA nmdisp = (LPNMTTDISPINFOA)lParam;
			switch (lpnm->idFrom)
			{
			case ID_ISSPAM:
				nmdisp->hinst = NULL;
				nmdisp->lpszText = const_cast<LPSTR>(m_strSpamTooltipA.c_str());
				break;

			case ID_ISNOTSPAM:
				nmdisp->hinst = NULL;
				nmdisp->lpszText = const_cast<LPSTR>(m_strProbSpamTooltipA.c_str());
				break;

			case ID_CONFIG:
				nmdisp->hinst = NULL;
				nmdisp->lpszText = const_cast<LPSTR>(m_strConfigTooltipA.c_str());
				break;
			}
		}
		else if(lpnm->code == TTN_NEEDTEXTW)
		{
			LPNMTTDISPINFOW nmdisp = (LPNMTTDISPINFOW)lParam;
			switch (lpnm->idFrom)
			{
			case ID_ISSPAM:
				nmdisp->hinst = NULL;
				nmdisp->lpszText = const_cast<LPWSTR>(m_strSpamTooltipW.c_str());
				break;

			case ID_ISNOTSPAM:
				nmdisp->hinst = NULL;
				nmdisp->lpszText = const_cast<LPWSTR>(m_strProbSpamTooltipW.c_str());
				break;

			case ID_CONFIG:
				nmdisp->hinst = NULL;
				nmdisp->lpszText = const_cast<LPWSTR>(m_strConfigTooltipW.c_str());
				break;
			}
		}
	}

	return S_OK;
}
