/*!
*		
*		
*		(C) 2003 Kaspersky Lab 
*		
*		\file	ToolsOptionsDlg.cpp
*		\brief	Tools.Options Property Page
*		
*		\author Nick Lebedev
*		\date	19.06.2003 16:11:36
*		
*		Example:	
*		
*		
*		
*/		

#include "stdafx.h"
#include "ToolsOptionsDlg.h"
#include "mcou_resource.h"
#include "mcou.h"
#include "util.h"
#include "Hyperlinks.h"
//#include <AVPRegID.h>
//#include <Stuff\RegStorage.h>
#include <uxtheme.h>
//#include "..\build\klcsc.h"
#include <structs/s_profiles.h>
#include <structs/s_gui.h>
#include "initterm.h"

void DlgCtrlSizeToContent ( HWND hDlg, ULONG ulCtrlID, int cxBlank = 0 )
{
	TCHAR szStrBuffer[256];

	if ( GetDlgItemText ( hDlg, ulCtrlID, szStrBuffer, sizeof(szStrBuffer) ) == 0 )
		return;

	HDC hDC = GetDC ( hDlg );
	HFONT hFont = (HFONT)::SendMessage(hDlg, WM_GETFONT, 0, 0);
	HFONT hOldFont = SelectFont ( hDC, hFont );
	SIZE sz;
	GetTextExtentPoint32 ( hDC, szStrBuffer, _tcslen(szStrBuffer), &sz );
	SelectFont ( hDC, hOldFont );
	ReleaseDC ( hDlg, hDC );
	HWND hCtrlWnd = GetDlgItem ( hDlg, ulCtrlID );
	SetWindowPos ( hCtrlWnd, NULL, 0, 0, sz.cx + cxBlank, sz.cy, SWP_NOMOVE | SWP_NOZORDER );
}

static HFONT g_hDlgFont = NULL;
static UINT g_TimerId;

static struct _GUIRegParam
{
	tstring sWindowClassName;
	tstring sMutexUserMode;
	// window message names
	UINT	msgShowSettings;
	UINT	msgEnableMailChecker;
}
g_GUIRegParam;

BOOL CALLBACK EnumChildWndProc (HWND hwnd, LPARAM lParam)
{
	::SendMessage (hwnd, WM_SETFONT, (WPARAM)lParam, (LPARAM)TRUE);
	
	return TRUE;
}

static void UpdateStatusAndSettings ( HWND hDlg )
{
	TCHAR szStrBuffer[256];

	bool bEnableSettings = true, bShowStatus2 = false, bShowStatus34 = false;
	
	// state
	MCOU::MCState mcState = MCOU::CheckMCState ();
	switch ( mcState )
	{
	case MCOU::mcsNoRTP:
		::LoadString ( MCOU::g_hResInstance, IDS_MCOU_CheckIsOff, szStrBuffer, sizeof(szStrBuffer) );
		bEnableSettings = false;
		bShowStatus2 = true;
		break;
	case MCOU::mcsOPEnabledDuplex:
	case MCOU::mcsOPEnabledIncoming:
		::LoadString ( MCOU::g_hResInstance, IDS_MCOU_CheckIsOn, szStrBuffer, sizeof(szStrBuffer) );
		bShowStatus34 = true;
		break;
	case MCOU::mcsOPDisabled:
		::LoadString ( MCOU::g_hResInstance, IDS_MCOU_CheckIsOff, szStrBuffer, sizeof(szStrBuffer) );
		bShowStatus34 = true;
		break;
	default:
		szStrBuffer[0] = _T('\0');
		break;				
	}

	// Если нет связи с BL - скрываем все настройки
	if ( NULL == GetBL() ) 
	{
		bEnableSettings = false;
		bShowStatus34 = false;
	}

	SetDlgItemText ( hDlg, IDC_MCOU_TOOLSOPTIONS_STATUS1, szStrBuffer );
	ShowWindow ( GetDlgItem ( hDlg, IDC_MCOU_TOOLSOPTIONS_STATUS2 ), bShowStatus2 ? SW_SHOW : SW_HIDE );

	if ( bShowStatus34 )
	{
		::LoadString ( MCOU::g_hResInstance,
			(mcState == MCOU::mcsOPEnabledDuplex || mcState == MCOU::mcsOPEnabledIncoming) ? IDS_MCOU_TurnMailCheckingOff : IDS_MCOU_TurnMailCheckingOn,
			szStrBuffer, sizeof(szStrBuffer) );
		SetDlgItemText ( hDlg, IDC_MCOU_TOOLSOPTIONS_STATUS4, szStrBuffer);
		DlgCtrlSizeToContent ( hDlg, IDC_MCOU_TOOLSOPTIONS_STATUS4, 14 );
	}
	ShowWindow ( GetDlgItem ( hDlg, IDC_MCOU_TOOLSOPTIONS_STATUS3 ), bShowStatus34 ? SW_SHOW : SW_HIDE );
	ShowWindow ( GetDlgItem ( hDlg, IDC_MCOU_TOOLSOPTIONS_STATUS4 ), bShowStatus34 ? SW_SHOW : SW_HIDE );

	// settings
//	EnableWindow ( GetDlgItem ( hDlg, IDD_MCOU_TOOLSOPTIONS_SECTION3 ), bEnableSettings );
	EnableWindow ( GetDlgItem ( hDlg, IDC_MCOU_TOOLSOPTIONS_SETTINGS1 ), bEnableSettings );
	EnableWindow ( GetDlgItem ( hDlg, IDC_MCOU_TOOLSOPTIONS_SETTINGS2 ), bEnableSettings );

	HWND hItem = GetDlgItem(hDlg, IDC_WWKS);
	if (hItem)
		EnableWindow(hItem, bEnableSettings);
}

BOOL CALLBACK ToolsOptionsDlgProc (HWND hDlg, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	BOOL bMsgResult = FALSE;

	switch (uMsg)
	{
	case WM_INITDIALOG:
		{
			MCOU::CWaitCur wc;

			// change font to Tahoma on > Win2k
			OSVERSIONINFO os;
			os.dwOSVersionInfoSize = sizeof (OSVERSIONINFO);
			if ( GetVersionEx ( &os ) && os.dwMajorVersion >= 5 )
			{

				HFONT hFont = (HFONT)::SendMessage (hDlg, WM_GETFONT, 0, 0);

				if ( hFont )
				{
					LOGFONT lf;
					::GetObject (hFont, sizeof(LOGFONT), &lf);
					
					_tcscpy ( lf.lfFaceName, _T("Tahoma") );

					if ( g_hDlgFont = CreateFontIndirect ( &lf ) )
					{
						EnumChildWindows ( hDlg, EnumChildWndProc, (LPARAM)g_hDlgFont );
					}
				}

				HINSTANCE hThemeLib = LoadLibrary("uxtheme.dll");
				if (hThemeLib)
				{
					typedef HRESULT (WINAPI * fnEnableThemeDialogTexture)(HWND hWnd, DWORD dwFlags);
					typedef BOOL (WINAPI * fnIsThemeActive)();
					typedef BOOL (WINAPI * fnIsAppThemed)();
					
					fnEnableThemeDialogTexture EnableThemeDialogTexture = 
							(fnEnableThemeDialogTexture)GetProcAddress(hThemeLib, "EnableThemeDialogTexture");
					fnIsThemeActive IsThemeActive =
							(fnIsThemeActive)GetProcAddress(hThemeLib, "IsThemeActive");
					fnIsAppThemed IsAppThemed =
							(fnIsThemeActive)GetProcAddress(hThemeLib, "IsAppThemed");
	
					if (IsAppThemed && IsAppThemed () &&
						IsThemeActive && IsThemeActive () &&
						EnableThemeDialogTexture)
					{
						EnableThemeDialogTexture (hDlg, ETDT_ENABLETAB);
					}

					FreeLibrary (hThemeLib);
				}
			}

			// size to content
			DlgCtrlSizeToContent ( hDlg, IDD_MCOU_TOOLSOPTIONS_SECTION1, 14 );
			DlgCtrlSizeToContent ( hDlg, IDD_MCOU_TOOLSOPTIONS_SECTION2, 14 );
			DlgCtrlSizeToContent ( hDlg, IDD_MCOU_TOOLSOPTIONS_SECTION3, 14 );
			DlgCtrlSizeToContent ( hDlg, IDD_MCOU_TOOLSOPTIONS_SECTION4, 14 );

			DlgCtrlSizeToContent ( hDlg, IDC_MCOU_TOOLSOPTIONS_SETTINGS2, 14 );
			ConvertStaticToHyperlink ( hDlg, IDC_MCOU_TOOLSOPTIONS_SETTINGS2 );

			DlgCtrlSizeToContent ( hDlg, IDC_MCOU_TOOLSOPTIONS_STATUS4, 14 );
			ConvertStaticToHyperlink ( hDlg, IDC_MCOU_TOOLSOPTIONS_STATUS4 );

			// status and settings
			UpdateStatusAndSettings ( hDlg );

			g_TimerId = SetTimer ( hDlg, 1, 1000, NULL );

			return FALSE;
		}

	case WM_DESTROY:
		
		if ( g_hDlgFont )
			DeleteObject ( g_hDlgFont );

		KillTimer ( hDlg, g_TimerId );

		return TRUE;

	case WM_TIMER:

		UpdateStatusAndSettings ( hDlg );

		break;

	case WM_COMMAND:
		{
			UINT CtrlID = LOWORD (wParam);
			UINT CmdCode = HIWORD (wParam);
			hTASKMANAGER pTM = (hTASKMANAGER)GetBL();

			if ( CmdCode == STN_CLICKED && pTM )
			{
				if ( 
					CtrlID == IDC_MCOU_TOOLSOPTIONS_SETTINGS2 ||
					CtrlID == IDC_MCOU_TOOLSOPTIONS_STATUS4
					)
				{
//					cERROR err = pTM->sysSendMsg(pmcASK_ACTION, cProfileAction::SHOW_SETTINGS,
//						NULL, &cProfileAction(AVP_PROFILE_MAILMONITOR), SER_SENDMSG_PSIZE);
					pTM->AskAction(
						TASKID_TM_ITSELF, 
						cAskGuiAction::SHOW_SETTINGS_WND, 
						&cProfileAction(AVP_PROFILE_MAILMONITOR)
						);
				}
			}
		}				

		break;

	case WM_NOTIFY:
		{
			LPNMHDR pnmhdr = (LPNMHDR)lParam;

			if ( pnmhdr->code == PSN_HELP )
			{
				MessageBox ( pnmhdr->hwndFrom,
						"Mail Checker",
						MCOU::g_sProductName.c_str (),
						MB_OK | MB_ICONINFORMATION );
			}
		}
	}
	
	return bMsgResult;
}