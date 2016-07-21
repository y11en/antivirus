// MailWeb.cpp: implementation of the MailWeb class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "common.h"
#include "MailWeb.h"
#include "MapiWrapper.h"
#include <sign/sign.h>
#include "../KLUtil/SysInfo.h"
#include "../KLUtil/FormatTimeString.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

#define SEND_MAIL_EXE_NAME	_T("kavsend.exe")

static std::string GetProdInfo (const SendMailProdInfo *pProdInfo)
{
	CStdString prod_info;

	prod_info.Format (_T("Product information: %s (build %d.%d.%d)\n")
		, pProdInfo->strProduct
		, pProdInfo->dwMajorVersion
		, pProdInfo->dwMinorVersion
		, pProdInfo->dwBuildNumber);

	prod_info += SYS_INFO_SEP;

	prod_info += _T("Anti-virus database release date:\n\t");
	prod_info += pProdInfo->tmLastUpdate.GetDateTimeString();

	prod_info.AppendFormat (_T("\nAnti-virus database record count:\n\t%d")
		, pProdInfo->dwKnownVirusBody);
	
	if (pProdInfo->bHasKey)
	{
		prod_info.AppendFormat (_T("\nLicense number:\n\t%04x-%06x-%08x")
			, pProdInfo->dwKeyMemberId, pProdInfo->dwKeyAppId, pProdInfo->dwKeySerNum);
		prod_info.AppendFormat (_T("\nLicense owner:\n\t%s")
			, pProdInfo->strKeyInfo.c_str ());
		prod_info.AppendFormat (_T("\nLicense expiration date:\n\t%s")
			, pProdInfo->tmKeyExpiration.GetDateTimeString().c_str());
	}
	else
	{
		prod_info += _T("\nLicense info:\n\tNo license key found");
	}

	return prod_info;
}

bool SendMail (MapiMessage *pNote, HWND hWnd, bool bAddSysInfoToBody, const SendMailProdInfo *pProdInfo)
{
	if (pNote == NULL)
		return false;

	TCHAR szBuffer[MAX_PATH];
	LPTSTR lpFileStart;
	
	if (!SearchPath (NULL,
		SEND_MAIL_EXE_NAME,
		NULL,
		sizeof(szBuffer) / sizeof(szBuffer[0]),
		szBuffer,
		&lpFileStart))
	{
		CString szMsg;
		szMsg.Format  (IDS_COMMON_CANNOT_FIND_KAVSEND, SEND_MAIL_EXE_NAME);
		MsgBoxEx(hWnd, (LPCTSTR)szMsg, NULL, MB_OK | MB_ICONERROR);
		return false;
	}

#ifndef _DEBUG
	if ( sign_check_file (szBuffer, 1, 0, 0, 0) != SIGN_OK )
	{
		CString szMsg;
		szMsg.Format  (IDS_COMMON_KAVSEND_CORRUPTED, szBuffer);
		MsgBoxEx(hWnd, (LPCTSTR)szMsg, NULL, MB_OK | MB_ICONERROR);
		return false;
	}
#endif
	
	std::string szFullPath;
	if ( !GetAutoDelTempFileName( szFullPath ) )
	{
		MsgBoxEx(hWnd, IDS_COMMON_CANNOT_SEND_MAIL, -1, MB_OK | MB_ICONERROR);
		return false;
	}
		
	
	CFileStorage fs;
	if ( !fs.attach_(szFullPath.c_str (), false) )
	{
		MsgBoxEx(hWnd, IDS_COMMON_CANNOT_SEND_MAIL, -1, MB_OK | MB_ICONERROR);
		return false;
	}
		
	
	std::basic_string<TCHAR> szModulePath = szBuffer;
	
	STARTUPINFO si={sizeof(STARTUPINFO)};
	PROCESS_INFORMATION pi={0};
	
	std::basic_string<TCHAR> szwCommandLine = (_T("\"") + szModulePath + _T("\" \"")+ szFullPath + _T("\""));
	
	if(::CreateProcess(
		NULL,
		(LPTSTR)(szwCommandLine.c_str()),
		NULL,
		NULL,
		FALSE,
		CREATE_DEFAULT_ERROR_MODE | CREATE_SUSPENDED,
		NULL,
		NULL,
		&si,
		&pi))
	{
		std::string strBody (pNote->lpszNoteText);
		LPSTR lpszOldNoteText = pNote->lpszNoteText;

		if (pProdInfo)
		{
			strBody += "\n\n";
			strBody += GetProdInfo (pProdInfo);
		}

		if (bAddSysInfoToBody)
		{
			strBody += "\n\n";
			strBody += KLUTIL::GetSysInfo ();

			pNote->lpszNoteText = (LPSTR)strBody.c_str ();
		}
		
		MapiMessageSer ser = *pNote;
		DWORD dwMagic = MAPI_SERIALIZE_FILE_MAGIC_NUMBER;
		std::string strErr;

		if ( ser.CopyAttaches(strErr) )
		{
			fs.write_  (&dwMagic, 4);
			
			dwMagic = MAPI_SEND;
			fs.write_  (&dwMagic, 4);
			
			PoliType pt (fs, false);
			pt<<ser;
		}
		else
		{
			fs.write_  (&dwMagic, 4);
			
			dwMagic = MAPI_DONTPROCESS;
			fs.write_  (&dwMagic, 4);

			CString szErrMsg;
			szErrMsg.Format  ( IDS_COMMON_CANNOT_FIND_ATTACH, strErr.c_str () );

			MsgBoxEx(hWnd, (LPCTSTR)szErrMsg, NULL, MB_OK | MB_ICONERROR);
		}
		
		pNote->lpszNoteText = lpszOldNoteText;

		fs.detach_  ();

		if ( ResumeThread  (pi.hThread) == -1 )
		{
			DeleteFile( szFullPath.c_str() );
			ser.DeleteIfCopied();
			TerminateProcess(pi.hProcess, 0);

			MsgBoxEx(hWnd, IDS_COMMON_CANNOT_SEND_MAIL, -1, MB_OK | MB_ICONERROR);
		}
		else
		{
			CloseHandle(pi.hThread);
			CloseHandle(pi.hProcess);
		}

		return true;
	}
	else
	{
		fs.detach_  ();
		DeleteFile( szFullPath.c_str() );
		return false;
	}
}

static long GetRegKey(HKEY hKey, LPCTSTR subkey, LPTSTR retdata)
{
	HKEY hSubKey;
	long retval = RegOpenKeyEx(hKey, subkey, 0, KEY_QUERY_VALUE, &hSubKey);
	
	if (retval == ERROR_SUCCESS) 
	{
		long datasize = MAX_PATH;
		TCHAR data[MAX_PATH];
		RegQueryValue(hSubKey, NULL, data, &datasize);
		_tcscpy(retdata,data);
		RegCloseKey(hSubKey);
	}
	
	return retval;
}

void OpenURL( LPCTSTR szURL )
{
	CWaitCursor wait;

/*	TCHAR key[MAX_PATH + MAX_PATH];

	if (GetRegKey(HKEY_CLASSES_ROOT, _T(".htm"), key) == ERROR_SUCCESS) 
	{
		lstrcat(key, _T("\\shell\\open\\command"));
		
		if (GetRegKey(HKEY_CLASSES_ROOT,key,key) == ERROR_SUCCESS) 
		{
			TCHAR *pos;
			pos = _tcsstr(key, _T("\"%1\""));
			if (pos == NULL) {                     // No quotes found
				pos = _tcsstr(key, _T("%1"));       // Check for %1, without quotes 
				if (pos == NULL)                   // No parameter at all...
					pos = key+_tcslen(key)-1;
				else
					*pos = _T('\0');                   // Remove the parameter
			}
			else
				*pos = _T('\0');                       // Remove the parameter
			
			lstrcat(pos, _T(" "));
			lstrcat(pos, szURL);
			if (WinExec(key, SW_SHOW) >= HINSTANCE_ERROR) //all ok
				return;
		}
	}
*/
	if ( int(ShellExecute (NULL, _T ("open"), szURL, NULL, NULL, SW_SHOW)) < HINSTANCE_ERROR )
	{
		CString szText;
		szText.Format  ("Failed to invoke URL: %s\n", szURL);
		MsgBoxEx ( NULL, szText );
	}
		
	
}