// MainWindow.h: interface for the CMainWindow class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_MAINWINDOW_H__6BCACF01_4EF7_4A7B_A6EA_21B664003799__INCLUDED_)
#define AFX_MAINWINDOW_H__6BCACF01_4EF7_4A7B_A6EA_21B664003799__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "../../TheBatAntispam/EnsureCleanup.h"
#include "OETricks.h"
#include "OESettings.h"
#include "RebarPos.h"
#include "ProgressDlg.h"

const UINT WM_AFTER_SUBCLASS	= WM_APP + 1;
const UINT ID_ISSPAM			= WM_APP + 2;
const UINT ID_ISNOTSPAM			= WM_APP + 3;
const UINT ID_CONFIG			= WM_APP + 4;
const UINT WM_CREATEFOLDER_DONE = WM_APP + 5;
const UINT WM_MSGPROP_DONE		= WM_APP + 6;

namespace
{
	typedef CWinTraits<WS_OVERLAPPEDWINDOW, 0>	CMainWndTraits;
}

class CMainWindow : public CWindowImpl<CMainWindow, CWindow, CMainWndTraits>
{
public:
	CMainWindow();
	virtual ~CMainWindow();

	DECLARE_WND_CLASS(TEXT("25D73C30-4F71-4a97-BC9D-54618809E085"))

	BEGIN_MSG_MAP(CMainWindow)
		MESSAGE_HANDLER(WM_TIMER, OnTimer)
		MESSAGE_HANDLER(WM_AFTER_SUBCLASS, OnAfterSubclass)
		COMMAND_HANDLER(OE_WM_CREATEFOLDER, 0, OnOECreateFolder)
		COMMAND_HANDLER(OE_WM_PROPERTIES, 0, OnOEProperties)
		COMMAND_HANDLER(OE_WM_COPYTOFOLDER, 0, OnOECopyMove)
		COMMAND_HANDLER(OE_WM_MOVETOFOLDER, 0, OnOECopyMove)
		COMMAND_HANDLER(ID_ISSPAM, 0, OnIsSpam)
		COMMAND_HANDLER(ID_ISNOTSPAM, 0, OnIsNotSpam)
		COMMAND_HANDLER(ID_CONFIG, 0, OnConfigure)
		MESSAGE_HANDLER(WM_CREATEFOLDER_DONE, OnCreateFolderDone)
		MESSAGE_HANDLER(WM_MSGPROP_DONE, OnMsgPropDone)
		MESSAGE_HANDLER(WM_DESTROY, OnDestroy)
		MESSAGE_HANDLER(WM_CLOSE, OnClose)
	END_MSG_MAP()

	bool m_bHasRebar;

	void PrepareToCopyMoveMessages()
	{
		m_bShouldProcessCopyMoveDialog = true;
	}

private:
	// the main initialization function
	LRESULT OnAfterSubclass(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	LRESULT OnTimer(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	LRESULT OnDestroy(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	LRESULT OnClose(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	LRESULT OnCreateFolderDone(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	LRESULT OnMsgPropDone(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	
	LRESULT OnIsSpam(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled);
	LRESULT OnIsNotSpam(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled);
	LRESULT OnConfigure(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled);
	LRESULT OnOECreateFolder(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled);
	LRESULT OnOEProperties(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled);
	LRESULT OnOECopyMove(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled);

	bool m_bShouldProcessCopyMoveDialog;

	HWND m_hToolbar;
	COETricks m_OETricks;
	COESettings m_OESettings;
	bool m_bAntiSpamIsRunning;
	LONG m_nStyle;
	CRebarPos m_RebarPos;

	// creates Spam-folders and default mail rules
	bool _CanInstallNow();
	bool _InstallPlugin();
	bool m_bIsInstalling;
	bool m_bIsConfigShown;

	bool _DestroyOurToolband();
	void _EnableButtons(bool bEnable);
	
	CProgressDlg m_ProgressDlg;
	CDummyDlg m_Dummy;
};

#endif // !defined(AFX_MAINWINDOW_H__6BCACF01_4EF7_4A7B_A6EA_21B664003799__INCLUDED_)
