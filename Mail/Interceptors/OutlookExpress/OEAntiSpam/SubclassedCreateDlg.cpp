// SubclassedCreateDlg.cpp: implementation of the CSubclassedCreateDlg class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "SubclassedCreateDlg.h"
#include "Tracing.h"

namespace
{
	const int OE_IDC_FOLDERNAME = 0x3e8;
}

void CSubclassedCreateDlg::AfterInitDialog()
{
	ASTRACE(_T("CreateFolderDialog:InitDialog"));
	
	HWND hItem = ::GetDlgItem(m_hWnd, OE_IDC_FOLDERNAME);
	if (!hItem)
	{
		ASTRACE(_T("CFD: wrong dialog"));
		return;
	}

	tstring msg(_T("Setting folder name: '"));
	msg += m_strDesiredFolderName + _T("'");
	ASTRACE(msg.c_str());
	ASTRACE(_T(""));

	::SetWindowText(hItem, m_strDesiredFolderName.c_str());
	PostMessage(WM_COMMAND, IDOK, NULL);
}

