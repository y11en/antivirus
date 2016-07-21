// KeyFileTest.cpp : Defines the entry point for the application.
//

#include "stdafx.h"
#include <stdlib.h>
#include "resource.h"
#include "../KeyFileDownloader for 6.0/keyfiledl.h"

BOOL CALLBACK DialogProc(HWND hwndDlg, UINT uMsg, WPARAM wParam, LPARAM lParam);
LPCTSTR GetMsgDesc(int i, BOOL& bExit);
BOOL CheckRetCode(int nRetCode);

int APIENTRY WinMain(HINSTANCE hInstance,
                     HINSTANCE hPrevInstance,
                     LPSTR     lpCmdLine,
                     int       nCmdShow)
{
 	// TODO: Place code here.
	


	DialogBox(
		GetModuleHandle(NULL), 
		MAKEINTRESOURCE(IDD_DIALOG_MAIN), 
		NULL, DialogProc
	);

	return 0;
}

BOOL CALLBACK DialogProc(HWND hwndDlg, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	TCHAR szBuf[MAX_PATH];
	BOOL bExit;
	int bResult;

	static bool bAuthSet = false;

	switch (uMsg)
	{
	case WM_INITDIALOG:
		EnableWindow(GetDlgItem(hwndDlg, ID_STOP), FALSE);
		SendMessage(GetDlgItem(hwndDlg, IDC_CHECK_IE), BM_SETCHECK, BST_CHECKED, 0);
		SetDlgItemText(hwndDlg, IDC_EDIT_URL, TEXT("http://activation.kaspersky.com/activate_product"));
		SetDlgItemText(hwndDlg, IDC_EDIT_DST, TEXT("e:\\keys"));
		SetDlgItemText(hwndDlg, IDC_EDIT_AGENT, TEXT("Key File Downloader"));
		SetDlgItemText(hwndDlg, IDC_EDIT_PID, TEXT("1165"));
		SetDlgItemText(hwndDlg, IDC_EDIT_PVER, TEXT("6.0.0.0"));
		SetDlgItemText(hwndDlg, IDC_EDIT_GUID, TEXT("T1JVS-NNMBD-K1QTN-SUBP8"));
		SetDlgItemText(hwndDlg, IDC_EDIT_COMPANY, TEXT("Kaspersky Lab"));
		SetDlgItemText(hwndDlg, IDC_EDIT_NAME, TEXT("Ivanov Ivan Ivanovitch"));
		SetDlgItemText(hwndDlg, IDC_EDIT_COUNTRY, TEXT("Russia"));
		SetDlgItemText(hwndDlg, IDC_EDIT_CITY, TEXT("Moscow"));
		SetDlgItemText(hwndDlg, IDC_EDIT_ADDR, TEXT("Geroev Panfilovtsev, 10"));
		SetDlgItemText(hwndDlg, IDC_EDIT_PHN, TEXT("797-87-00"));
		SetDlgItemText(hwndDlg, IDC_EDIT_FAX, TEXT("797-87-01"));
		SetDlgItemText(hwndDlg, IDC_EDIT_MAIL, TEXT("sales@kaspersky.com"));
		SetDlgItemText(hwndDlg, IDC_EDIT_WWW, TEXT("www.kaspersky.com"));
		SetDlgItemText(hwndDlg, IDC_PROXY_URL, TEXT("proxy.avp.ru:3128"));
		SetDlgItemText(hwndDlg, IDC_PROXY_USERNAME, TEXT("kl\\tester"));

		return TRUE;
		break;

	case WM_CLOSE:
		EndDialog(hwndDlg, wParam);

	case WM_COMMAND:
		switch(LOWORD(wParam))
		{
		case ID_START:
			{
				bAuthSet = false;

				wchar_t szUrl[MAX_PATH], szKeyNum[MAX_PATH], szCompany[MAX_PATH];
				wchar_t szName[MAX_PATH], szCountry[MAX_PATH], szCity[MAX_PATH];
				wchar_t szAddress[MAX_PATH], szPhone[MAX_PATH], szFax[MAX_PATH];
				wchar_t szEmail[MAX_PATH], szWww[MAX_PATH], szDst[MAX_PATH];
				wchar_t szAgent[MAX_PATH], szPver[MAX_PATH], szProxyUrl[MAX_PATH];

				EnableWindow(GetDlgItem(hwndDlg, ID_STOP), TRUE);
				EnableWindow(GetDlgItem(hwndDlg, ID_START), FALSE);
				
				CUserData data = {0};

				BOOL bTr;
				data.nProdId = (int) GetDlgItemInt(hwndDlg, IDC_EDIT_PID, &bTr, FALSE);
				if (!bTr)
					data.nProdId = 0;

				GetDlgItemTextW(hwndDlg, IDC_EDIT_PVER, szPver, MAX_PATH);
				GetDlgItemTextW(hwndDlg, IDC_EDIT_URL, szUrl, MAX_PATH);
				GetDlgItemTextW(hwndDlg, IDC_EDIT_AGENT, szAgent, MAX_PATH);
				GetDlgItemTextW(hwndDlg, IDC_EDIT_DST, szDst, MAX_PATH);
				GetDlgItemTextW(hwndDlg, IDC_EDIT_GUID, szKeyNum, MAX_PATH);
				GetDlgItemTextW(hwndDlg, IDC_EDIT_COMPANY, szCompany, MAX_PATH);
				GetDlgItemTextW(hwndDlg, IDC_EDIT_NAME, szName, MAX_PATH);
				GetDlgItemTextW(hwndDlg, IDC_EDIT_COUNTRY, szCountry, MAX_PATH);
				GetDlgItemTextW(hwndDlg, IDC_EDIT_CITY, szCity, MAX_PATH);
				GetDlgItemTextW(hwndDlg, IDC_EDIT_ADDR, szAddress, MAX_PATH);
				GetDlgItemTextW(hwndDlg, IDC_EDIT_PHN, szPhone, MAX_PATH);
				GetDlgItemTextW(hwndDlg, IDC_EDIT_FAX, szFax, MAX_PATH);
				GetDlgItemTextW(hwndDlg, IDC_EDIT_MAIL, szEmail, MAX_PATH);
				GetDlgItemTextW(hwndDlg, IDC_EDIT_WWW, szWww, MAX_PATH);
				GetDlgItemTextW(hwndDlg, IDC_PROXY_URL, szProxyUrl, MAX_PATH);
				
				data.lpszProdVer	= szPver;
				data.lpszUrl		= szUrl;
				data.lpszAgent		= szAgent;
				data.lpszDstFolder	= szDst;
				data.lpszKeyNum		= szKeyNum;
				data.lpszCompany	= szCompany;
				data.lpszName		= szName;
				data.lpszCountry	= szCountry;
				data.lpszCity		= szCity;
				data.lpszAddress	= szAddress;
				data.lpszTelephone	= szPhone;
				data.lpszFax		= szFax;
				data.lpszEmail		= szEmail;
				data.lpszWww		= szWww;
				data.bUseIeSettings	= (SendMessage(GetDlgItem(hwndDlg, IDC_CHECK_IE), BM_GETCHECK, 0, 0) == BST_CHECKED);
				data.bUseProxy		= (SendMessage(GetDlgItem(hwndDlg, IDC_CHECK_PROXY), BM_GETCHECK, 0, 0) == BST_CHECKED);
				data.bTryAutoAuth	= (SendMessage(GetDlgItem(hwndDlg, IDC_CHECK_AUTH), BM_GETCHECK, 0, 0) == BST_CHECKED);
				data.lpszProxyList	= szProxyUrl;
				data.lpszProxyBypass= NULL;

				bResult = StartDownloadKeyFile(hwndDlg, IDC_BTN_TEST, &data);

				if (CheckRetCode(bResult))
				{
					EnableWindow(GetDlgItem(hwndDlg, ID_STOP), FALSE);
					EnableWindow(GetDlgItem(hwndDlg, ID_START), TRUE);
				}
				return TRUE;
				break;
			}

		case ID_STOP:
			CheckRetCode(StopDownloadKeyFile());
			EnableWindow(GetDlgItem(hwndDlg, ID_STOP), FALSE);
			EnableWindow(GetDlgItem(hwndDlg, ID_START), TRUE);
			return TRUE;
			break;

		case ID_CLR:
			SendDlgItemMessage(hwndDlg, IDC_LIST_LOG, LB_RESETCONTENT, 0, 0);
			return TRUE;
			break;

		case ID_CANCEL:
			EndDialog(hwndDlg, wParam);
			return TRUE;
			break;

		case IDC_BTN_TEST:
			{
				GetDlgItemText(hwndDlg, IDC_BTN_TEST, szBuf, MAX_PATH);
				LPTSTR lpszTemp;
				int i = _tcstol(szBuf, &lpszTemp, 10);
				lstrcat(szBuf, GetMsgDesc(i, bExit));
				SendDlgItemMessage(hwndDlg, IDC_LIST_LOG, LB_ADDSTRING, 0, (LPARAM)szBuf);
				if (i == MSGERR_CLNT_PRXAUTHRQ)
				{
					if (bAuthSet)
					{
						StopDownloadKeyFile();
						return TRUE;
					}
					char szProxyLogin[MAX_PATH], szProxyPwd[MAX_PATH];
					GetDlgItemTextA(hwndDlg, IDC_PROXY_USERNAME, szProxyLogin, MAX_PATH);
					GetDlgItemTextA(hwndDlg, IDC_PROXY_PASSWORD, szProxyPwd, MAX_PATH);
					SetCredentialProxy(szProxyLogin, szProxyPwd);
					bAuthSet = true;
				}
				if (bExit)
				{
					TCHAR szFileNameW[MAX_PATH];
					char szFileNameA[MAX_PATH] = "<no_key_file>";
					int nLen = MAX_PATH;
		
					GetKeyFileName(szFileNameW, &nLen);

					//mbstowcs(szFileNameW, szFileNameA, MAX_PATH);
					SendDlgItemMessage(hwndDlg, IDC_LIST_LOG, LB_ADDSTRING, 0, (LPARAM) szFileNameW);
					EnableWindow(GetDlgItem(hwndDlg, ID_STOP), FALSE);
					EnableWindow(GetDlgItem(hwndDlg, ID_START), TRUE);
				}
				return TRUE;
			}
			break;
		case IDC_CHECK_IE:
			if (HIWORD(wParam) == BN_CLICKED)
			{
				BOOL bEnable = (SendMessage(GetDlgItem(hwndDlg, IDC_CHECK_IE), BM_GETCHECK, 0, 0) != BST_CHECKED);
				EnableWindow(GetDlgItem(hwndDlg, IDC_CHECK_PROXY), bEnable);
			}
		case IDC_CHECK_PROXY:
			if (HIWORD(wParam) == BN_CLICKED)
			{
				BOOL bEnable = 
					(SendMessage(GetDlgItem(hwndDlg, IDC_CHECK_PROXY), BM_GETCHECK, 0, 0) == BST_CHECKED) && 
						(SendMessage(GetDlgItem(hwndDlg, IDC_CHECK_IE), BM_GETCHECK, 0, 0) != BST_CHECKED);
				EnableWindow(GetDlgItem(hwndDlg, IDC_PROXY_URL), bEnable);
				EnableWindow(GetDlgItem(hwndDlg, IDC_PROXY_USERNAME), bEnable);
				EnableWindow(GetDlgItem(hwndDlg, IDC_PROXY_PASSWORD), bEnable);
			}
			break;

		default:
			return FALSE;
			break;
		}

	default:
		return FALSE;
		break;
	}
}

LPCTSTR GetMsgDesc(int i, BOOL& bExit)
{
	switch(i)
	{
	case 101: bExit = FALSE; return TEXT(": Initializing");
	case 102: bExit = FALSE; return TEXT(": Posting Data");
	case 103: bExit = FALSE; return TEXT(": Saving to Temporary");
	case 104: bExit = FALSE; return TEXT(": Checking Key File");
	case 105: bExit = FALSE; return TEXT(": Retrying");
	case 106: bExit = FALSE; return TEXT(": Saving to Destination");
	case 107: bExit = FALSE; return TEXT(": Posting Status");
	case 108: bExit = TRUE;	return TEXT(": Completed");

	case 401: bExit = TRUE;	return TEXT(": Authentication required");
	case 404: bExit = TRUE;	return TEXT(": Resource not found");
	case 407: bExit = TRUE; return TEXT(": Proxy authentication required");

	case 701: bExit = FALSE; return TEXT(": Connecting");
	case 702: bExit = FALSE; return TEXT(": Resolving");
	case 703: bExit = FALSE; return TEXT(": Sending");
	case 704: bExit = FALSE; return TEXT(": Sent");
	case 705: bExit = FALSE; return TEXT(": Receiving");
	case 706: bExit = FALSE; return TEXT(": Received");
	case 707: bExit = FALSE; return TEXT(": Redirecting");

	case 600: bExit = TRUE; return TEXT(": Cancelled");
	case 601: bExit = TRUE; return TEXT(": Initialize Failed");
	case 602: bExit = TRUE; return TEXT(": Form Fill Data Failed");
	case 603: bExit = TRUE; return TEXT(": Sending Error");
	case 604: bExit = TRUE; return TEXT(": Bad Content Received");
	case 605: bExit = TRUE; return TEXT(": Unable to Get Reply Status");
	case 606: bExit = TRUE; return TEXT(": Temp Saving Failed");
	case 607: bExit = TRUE; return TEXT(": Destination Saving Failed");
	case 608: bExit = TRUE; return TEXT(": Wrong Key File");
	case 609: bExit = TRUE; return TEXT(": Corrupted Key File");
	case 610: bExit = TRUE; return TEXT(": Incorrect local date");
	case 612: bExit = TRUE; return TEXT(": Key is blocked");
	case 616: bExit = TRUE; return TEXT(": Wrong activation code");

	default: 
		if (IsActivationServerError(i))
		{
			bExit = FALSE; 
			return TEXT(": Activation server error");
		}
		else
		{
			bExit = TRUE; 
			return TEXT(": Unknown error");
		}
		break;
	}

}

BOOL CheckRetCode(int nRetCode)
{
	if (nRetCode != ERR_SUCCESS)
	{
		LPCTSTR lpszMsg = NULL;
		switch (nRetCode)
		{
		case ERR_BAD_HWND:	lpszMsg = TEXT("Bad Dialog Handle"); break;
		case ERR_BAD_RID:	lpszMsg = TEXT("Bad Resource Id"); break;
		case ERR_BAD_KEY:	lpszMsg = TEXT("Bad Key Number"); break;
		case ERR_BAD_URL:	lpszMsg = TEXT("Bad URL"); break;
		case ERR_BAD_FOLDER:lpszMsg = TEXT("Bad Destination Folder"); break;
		case ERR_MEM_ALLOC:	lpszMsg = TEXT("Unable to Allocate Memory"); break;
		case ERR_CONV_ANSI:	lpszMsg = TEXT("Convertion Parameters to ANSI failed"); break;
		case ERR_THR_CREATE: lpszMsg = TEXT("Unable to Create Thread"); break;
		case ERR_THR_STARTED: lpszMsg = TEXT("Thread Already Started"); break;
		case ERR_THR_STOPPED: lpszMsg = TEXT("Thread Already Stopped"); break;
		}
		
		MessageBox(NULL, lpszMsg, TEXT("StartDownloadKeyFile()"), MB_OK);
		return TRUE;
	}

	return FALSE;
}