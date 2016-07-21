// SubclassedMsgSourceDlg.cpp: implementation of the CSubclassedMsgSourceDlg class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "SubclassedMsgSourceDlg.h"
#include <boost/scoped_array.hpp>
#include "Utility.h"
#include "../../TheBatAntispam/EnsureCleanup.h"
#include "Tracing.h"
#include "OETricks.h"
#include <richedit.h>

void CSubclassedMsgSourceDlg::AfterInitDialog()
{
	HWND hRichEdit = GetDlgItem(100);
	if (!hRichEdit)
	{
		ASTRACE(_T("MSD: wrong dialog"));
		PostMessage(WM_COMMAND, IDCANCEL, NULL);
		return;
	}

	LRESULT nBufLen = ::SendMessage(hRichEdit, WM_GETTEXTLENGTH, NULL, NULL) + 1;
	boost::scoped_array<TCHAR> arrBuf(new TCHAR[nBufLen + 1]);

	::SendMessage(hRichEdit, EM_SETSEL, 0, -1);

	LRESULT nCharsCopied = 0;
	if (COETricks::m_bVista)
	{
		// WinMail под Vista использует юникодный элемент управления
		boost::scoped_array<WCHAR> arrBufW(new WCHAR[nBufLen + 1]);
		nCharsCopied = ::SendMessage(hRichEdit, EM_GETSELTEXT, 0, (LPARAM) arrBufW.get());
		arrBufW[nCharsCopied] = 0;
		nCharsCopied = ::WideCharToMultiByte(CP_ACP, 0, arrBufW.get(), -1, arrBuf.get(), (int)((nBufLen + 1) * sizeof(TCHAR)), NULL, NULL);
	}
	else
	{
		nCharsCopied = ::SendMessage(hRichEdit, EM_GETSELTEXT, 0, (LPARAM) arrBuf.get());
		arrBuf[nCharsCopied] = _T('\0');
	}

	if (nCharsCopied > 0)
		_Module.TeachAntiSpam(m_bIsSpam, reinterpret_cast<LPBYTE>(arrBuf.get()), (UINT)nBufLen * sizeof(TCHAR));

	if (m_hFinishedEvent)
		SetEvent(m_hFinishedEvent);

	PostMessage(WM_SYSCOMMAND, SC_CLOSE, NULL);
}

