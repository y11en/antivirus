#include "stdafx.h"
#include "CKAHUMTESTUI2.h"
#include "Utils.h"
#include "PortDlg.h"
#include "AddressDlg.h"

using namespace CKAHFW;
using namespace CKAHUM;

void RefreshPortList (CListBox &ListCtrl, CKAHFW::HRULEPORTS hPorts)
{
	int nSel = ListCtrl.GetCurSel ();
	ListCtrl.SetRedraw (FALSE);
	ListCtrl.ResetContent ();

	int n = RulePorts_GetCount (hPorts);

	for (int i = 0; i < n; ++i)
	{
		HRULEPORT hPort = NULL;

		if (RulePorts_GetItem (hPorts, i, &hPort) != srOK)
			continue;

		RulePortType port_type;
		if (RulePort_GetType (hPort, &port_type) == srOK)
		{
			CString PortName;

			if (port_type == rptSingle)
			{
				USHORT val;
				if (RulePort_GetSinglePort (hPort, &val) == srOK)
					PortName.Format (_T("%d"), val);
			}
			else if (port_type == rptRange)
			{
				USHORT loval, hival;
				if (RulePort_GetPortRange (hPort, &loval, &hival) == srOK)
					PortName.Format (_T("%d - %d"), loval, hival);
			}
			else
			{
				ASSERT (0);
				continue;
			}

			if (!PortName.IsEmpty ())
				ListCtrl.AddString (PortName);
			else
				ListCtrl.AddString (_T("<no name>"));
		}

		RulePort_Delete (hPort);
	}
	ListCtrl.SetRedraw (TRUE);
	ListCtrl.SetFocus ();
	ListCtrl.SetCurSel (nSel);
}

void AddPort (CListBox &ListCtrl, CKAHFW::HRULEPORTS hPorts, CWnd *pParent)
{
	HRULEPORT hPort = RulePort_Create (rptSingle);

	CPortDlg PortDlg (hPort, pParent);

	if (PortDlg.DoModal () == IDOK)
	{
		RulePorts_AddItemToBack (hPorts, PortDlg.m_hPort);
		RefreshPortList (ListCtrl, hPorts);
	}

	RulePort_Delete (hPort);
}

void EditPort (CListBox &ListCtrl, CKAHFW::HRULEPORTS hPorts, CWnd *pParent)
{
	int nSel = ListCtrl.GetCurSel ();

	if (nSel != LB_ERR)
	{
		HRULEPORT hPort = NULL;

		if (RulePorts_GetItem (hPorts, nSel, &hPort) != srOK)
			return;

		CPortDlg PortDlg (hPort, pParent);

		if (PortDlg.DoModal () == IDOK)
		{
			RulePorts_DeleteItem (hPorts, nSel);
			RulePorts_InsertItem (hPorts, nSel, PortDlg.m_hPort);
			RefreshPortList (ListCtrl, hPorts);
		}

		RulePort_Delete (hPort);
	}
}

void DeletePort (CListBox &ListCtrl, CKAHFW::HRULEPORTS hPorts)
{
	int nSel = ListCtrl.GetCurSel ();

	if (nSel != LB_ERR)
	{
		RulePorts_DeleteItem (hPorts, nSel);
		RefreshPortList (ListCtrl, hPorts);
	}		
}

//////////////////////////////////////////////////////////////////////////

void RefreshAddressList (CListBox &ListCtrl, CKAHFW::HRULEADDRESSES hAddresses)
{
	int nSel = ListCtrl.GetCurSel ();
	ListCtrl.SetRedraw (FALSE);
	ListCtrl.ResetContent ();

	int n = RuleAddresses_GetCount (hAddresses);

	for (int i = 0; i < n; ++i)
	{
		HRULEADDRESS hAddress = NULL;

		if (RuleAddresses_GetItem (hAddresses, i, &hAddress) != srOK)
			continue;

		CString AddressName;

		ULONG nSize = RuleAddress_GetName (hAddress, NULL, 0);
		if (nSize)
		{
			WCHAR *pName = new WCHAR[nSize];
			if (pName && RuleAddress_GetName (hAddress, pName, nSize) == nSize - 1)
				AddressName = CString (pName);

			delete []pName;
		}

		if (!AddressName.IsEmpty ())
			ListCtrl.AddString (AddressName);
		else
			ListCtrl.AddString (_T("<no name>"));

		RuleAddress_Delete (hAddress);
	}
	ListCtrl.SetRedraw (TRUE);
	ListCtrl.SetFocus ();
	ListCtrl.SetCurSel (nSel);
}

void AddAddress (CListBox &ListCtrl, CKAHFW::HRULEADDRESSES hAddresses, CWnd *pParent)
{
	HRULEADDRESS hAddress = RuleAddress_Create (ratHost);

	CAddressDlg AddressDlg (hAddress, pParent);

	if (AddressDlg.DoModal () == IDOK)
	{
		RuleAddresses_AddItemToBack (hAddresses, AddressDlg.m_hAddress);
		RefreshAddressList (ListCtrl, hAddresses);
	}

	RuleAddress_Delete (hAddress);
}

void EditAddress (CListBox &ListCtrl, CKAHFW::HRULEADDRESSES hAddresses, CWnd *pParent)
{
	int nSel = ListCtrl.GetCurSel ();

	if (nSel != LB_ERR)
	{
		HRULEADDRESS hAddress = NULL;

		if (RuleAddresses_GetItem (hAddresses, nSel, &hAddress) != srOK)
			return;

		CAddressDlg AddressDlg (hAddress, pParent);

		if (AddressDlg.DoModal () == IDOK)
		{
			RuleAddresses_DeleteItem (hAddresses, nSel);
			RuleAddresses_InsertItem (hAddresses, nSel, AddressDlg.m_hAddress);
			RefreshAddressList (ListCtrl, hAddresses);
		}

		RuleAddress_Delete (hAddress);
	}
}

void DeleteAddress (CListBox &ListCtrl, CKAHFW::HRULEADDRESSES hAddresses)
{
	int nSel = ListCtrl.GetCurSel ();

	if (nSel != LB_ERR)
	{
		RuleAddresses_DeleteItem (hAddresses, nSel);
		RefreshAddressList (ListCtrl, hAddresses);
	}		
}

static BOOL _SetForegroundWindowEx ( HWND hWnd ) // handle to window
{
	HWND hCurrWnd;
	int iTID;
	int iCurrTID;
	
	// determine current foreground window
	hCurrWnd  = ::GetForegroundWindow();

	if ( hCurrWnd == NULL )
	{
		TRACE0 ("PopupWindow: no foreground window\n");

		if ( hCurrWnd == NULL )
		{
			return FALSE;
		}
	}

	// check whether it is able to pump messages to avoid deadlocks
	DWORD dwResult = 0;
	if ( SendMessageTimeout ( hCurrWnd, WM_NULL, 0, 0, SMTO_NORMAL, 12000, &dwResult ) == 0 )
	{
		DWORD dwError = GetLastError ();
		TRACE0 ("PopupWindow: SendMessageTimeout failed\n");
		return FALSE;
	}

	iTID      = ::GetWindowThreadProcessId( hWnd,0);
	iCurrTID  = ::GetWindowThreadProcessId( hCurrWnd,0);
	
	AttachThreadInput( iTID, iCurrTID, TRUE );
	
	TRACE0 ("PopupWindow: AttachThreadInput 1\n");

	BOOL bRet = ::SetForegroundWindow( hWnd );
	
	if ( bRet )
	{
		TRACE0 ("PopupWindow: SetForegroundWindow succeeded\n");
	}
	else
	{
		TRACE0 ("PopupWindow: SetForegroundWindow failed\n");
	}

	AttachThreadInput( iTID, iCurrTID, FALSE );
	
	TRACE0 ("PopupWindow: AttachThreadInput 2\n");

	return bRet;
}

void PopupWindow (CWnd *pWnd, bool bMakeTopmostOnFail)
{
	if(!pWnd || !IsWindow(pWnd->GetSafeHwnd()))
		return;

	TRACE0 ("PopupWindow: -------------- popup window 1 ---------------\n");

	if (!_SetForegroundWindowEx ( pWnd->GetSafeHwnd() ))
	{
		TRACE0 ("PopupWindow: _SetForegroundWindowEx failed\n");

		if ( pWnd->SetForegroundWindow () )
		{
			TRACE0 ("PopupWindow: SetForegroundWindow succeeded\n");
		}
		else
		{
			TRACE0 ("PopupWindow: SetForegroundWindow failed\n");
		}

		if ( bMakeTopmostOnFail )
		{
			TRACE0 ("PopupWindow: setting window topmost\n");
			pWnd->SetWindowPos(&CWnd::wndTopMost, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE);
		}
	}

    pWnd->BringWindowToTop ();
	pWnd->FlashWindow (TRUE);

	TRACE0 ("PopupWindow: -------------- popup window 2 ---------------\n");
}

DWORD GetLongPathName (CStdStringW strShortPath, CStdStringW &strLongPath)
{
	int iFound = strShortPath.ReverseFind (L'\\');

	if (iFound > 1)
	{
		// recurse to peel off components
		//
		if (GetLongPathName(strShortPath.Left (iFound), strLongPath) > 0)
		{
			strLongPath += L'\\';
				
				if (strShortPath.Right(1) != L"\\")
				{
					HANDLE hFind;

					if (GetVersion() < 0x80000000)
					{
						WIN32_FIND_DATAW findData;
						
						// append the long component name to the path
						//
						if (INVALID_HANDLE_VALUE != (hFind = ::FindFirstFileW (strShortPath, &findData)))
						{
							strLongPath += findData.cFileName;
							::FindClose (hFind);
						}
						else
						{
							// if FindFirstFile fails, return the error code
							//
							strLongPath.Empty();
							return 0;
						}
					}
					else
					{
						WIN32_FIND_DATAA findData;
						
						// append the long component name to the path
						//
						if (INVALID_HANDLE_VALUE != (hFind = ::FindFirstFileA (CStdStringA (strShortPath), &findData)))
						{
							strLongPath += findData.cFileName;
							::FindClose (hFind);
						}
						else
						{
							// if FindFirstFile fails, return the error code
							//
							strLongPath.Empty();
							return 0;
						}
					}
				}
		}
	}
	else
	{
		strLongPath = strShortPath;
	}
	
	return strLongPath.GetLength();
}
