#include "stdafx.h"
#include <io.h>
#include "globals.h"
#include <direct.h>

///////////////////////////////////////////////////////////////////////////
// Global variables
CString sErrDlgTitle = "Error", 
		sInfoDlgTitle = "Info";
HINSTANCE hLocalizationLibrary = NULL; // handle of localization dll
BOOL boManualMode = TRUE;
int STR_ID_BASE = 0;

///////////////////////////////////////////////////////////////////////////
// Global function implementations


//////////////////////////////////////////////////////////////////////////////////////
// Localization functions

HINSTANCE LoadLocLibrary(CString sAVPLocDLLPath, CString sLocName)
{
	CString S = ComposeFileName("Configuration", "LocFile", sAVPLocDLLPath, sLocName);
	hLocalizationLibrary = LoadLibrary(S);

	return hLocalizationLibrary;
}

BOOL FreeLocLibrary()
{
	if (hLocalizationLibrary) 
	{
		FreeLibrary(hLocalizationLibrary);
		hLocalizationLibrary = NULL;
	}

	return TRUE;
}

// Load string from localization library if it exists else from current module
CString LoadStr(int nID)
{
	CString S;
	char szS[1024];

	if ((hLocalizationLibrary) && (LoadString(hLocalizationLibrary,STR_ID_BASE + nID, szS,1024) != 0)) 
		S = szS;
	else 
		S.LoadString(nID);

	return S;
}

//////////////////////////////////////////////////////////////////////////////
// Message boxes

void LocalizeMsgBoxTitles(int nErrorDlgTitle, int nInfoDlgTitle)
{
	sErrDlgTitle = LoadStr(nErrorDlgTitle);
	sInfoDlgTitle = LoadStr(nInfoDlgTitle);
}

CString PrepareMessage(LONG resMsg, CString sParam)
{
	CString sMsg;
	
	if (resMsg == 0) 
	{
		sMsg = sParam;
	}
	else
	{
		CString S = LoadStr(resMsg);
		sMsg.Format(S, sParam);
	} 

	return sMsg;		
}

// display error message box if boManualMode == true
int ErrorMessageBox(HWND hWnd, LONG resMsg, CString sParam)
{
	if (boManualMode) 
	{
		if (!hWnd)
			if (AfxGetApp()->m_pMainWnd) hWnd = AfxGetApp()->m_pMainWnd->m_hWnd;

		return MessageBox(hWnd, PrepareMessage(resMsg, sParam), sErrDlgTitle, MB_ICONSTOP | MB_OK);
	}

	return MB_OK;
}

// display Info message box if boManualMode == true
int InfoMessageBox(HWND hWnd, LONG resMsg, CString sParam)
{
	if (boManualMode) 
	{
		if (!hWnd)
			if (AfxGetApp()->m_pMainWnd) hWnd = AfxGetApp()->m_pMainWnd->m_hWnd;

		return MessageBox(hWnd, PrepareMessage(resMsg, sParam), sInfoDlgTitle, MB_ICONINFORMATION | MB_OK);
	}
	return MB_OK;
}

///////////////////////////////////////////////////////////////////////////////
// String manipulations

///////////////////////////////////////////////////////////////////////////////
// GetFileTitle - returns file name without extentions from file name 
// ----------------------------------------------------------------------------
// CString sFileName	- full name without path
//
// Returns : file name if successfull or "" otherwise
CString GetFileTitle(CString sFileName)
{
	int n = sFileName.Find('.');
	if (n == -1) return "";

	return sFileName.Left(n);
}

///////////////////////////////////////////////////////////////////////////////
// GetPath - returns path from full file name
// ----------------------------------------------------------------------------
// CString sFileName	- full name with path
//
// Returns : path if successfull or "" otherwise
CString GetPath(CString sFileName)
{
	CString S;
	int n = sFileName.ReverseFind('\\');
	if (n != -1) return sFileName.Left(n + 1);

	return "";
}

///////////////////////////////////////////////////////////////////////////////
// GetExt - returns extention from file name
// ----------------------------------------------------------------------------
// CString sFileName	- file name
//
// Returns : extention if successfull or "" otherwise
CString GetExt(CString sFileName)
{
	CString S;
	int n = sFileName.ReverseFind('.');
	if (n != -1) return sFileName.Mid(n + 1);

	return "";
}


///////////////////////////////////////////////////////////////////////////////
// GetName - returns file name with extention from full file name
// ----------------------------------------------------------------------------
// CString sFileName	- full file name with path
//
// Returns : file name if successfull or "" otherwise
CString GetName(CString sFileName)
{
	CString S;

	int nSlash = sFileName.ReverseFind('\\');
	int nBackSlash = sFileName.ReverseFind('/');
	int n = __max(nSlash, nBackSlash);
	if (n != -1)
	{
		S = sFileName.Mid(n + 1);
	}
	else S = sFileName;

	return S;
}

// searches sSubStr in S in upper case
int Pos(CString S, CString sSubStr)
{
	sSubStr.MakeUpper();
	S.MakeUpper();
	return S.Find(sSubStr);
}
// checks S for valid number
BOOL IsValidNumber(CString S)
{
	CString S1 = S.SpanIncluding("0123456789");
	if (S1.GetLength() < S.GetLength()) return FALSE;
	else return TRUE;
}

CString ClosePath(CString& S, char cSlash)
{
	if (S != "")
	{
		char C = S[S.GetLength() - 1];
		if (C != cSlash) S += cSlash;
	}

	return S;
}

CString FormatStr(int nFormat, CString sParam)
{
	CString S;
	
	S.Format(LoadStr(nFormat), sParam);

	return S;
}

LPTSTR GetEnclosedStr(LPCTSTR szSrc, LPTSTR szDst, char cBeg, char cEnd)
{
	szDst[0] = 0;
	char *pe, *pb = strchr(szSrc, cBeg);

	if (pb != NULL) 
	{
		pb++;

		pe = strchr(pb, cEnd);
		if (pe == NULL)	pe = (char*)&szSrc[strlen(szSrc)];
		int n = pe - pb;
		strncpy(szDst,pb,n);
		szDst[n] = 0;
	}

	return szDst;
}

BOOL CutEnclosedStr(CString& sSrc, CString& sDst, char cBeg, char cEnd)
{
	CString S;
	sDst = "";

	int n = sSrc.Find(cBeg);
	if (n != -1)
	{
		S = sSrc.Mid(n + 1);
		n = S.Find(cEnd);
		if (n != -1)
		{
			sDst = S.Left(n);
			sSrc = S.Mid(n + 1);
			return TRUE;
		}
	}

	return FALSE;
}

/////////////////////////////////////////////////////////////////////////////
// CheckStrForValidChars - Checks source string for valid characters
// --------------------------------------------------------------------------
// CString sSrc				- string to check
// CString sValidChars		- string which contains valid characters
// return					TRUE - if string is valid, FALSE - otherwise
BOOL CheckStrForValidChars(CString sSrc, CString sValidChars)
{
	for (int i = 0;i < sSrc.GetLength();i++)
	{
		if (sValidChars.Find(sSrc[i]) == -1) return FALSE;
	}

	return TRUE;
}

// Deletes all chars not including in sMask
CString SpanIncluding(CString& sSrc, CString sMask)
{
	CString sDst = "";
	char c;

	for (int i = 0;i < sSrc.GetLength();i++)
	{
		c = sSrc[i];
		if (sMask.Find(c) != -1) sDst += c;
	}
	sSrc = sDst;

	return sSrc;
}

DWORD HexStrToDWORD(LPCTSTR str, int len)
{
   DWORD l=0;
   int i;
   
   char s[100];
   strcpy(s, str);
   _strupr(s);
   for(i = len - 1;i >= 0;i--)
   {
      if (s[i]>='0' && s[i]<='9')
         l+=((DWORD) (s[i]-'0'))<<(4*(len-i-1));
	  else
      if (s[i]>='A' && s[i]<='F')
         l+=((DWORD) (s[i]-'A'+10))<<(4*(len-i-1));
	  else
		  return 0;
   }

   return l;
}

LPTSTR RemoveSpaces(LPTSTR str)
{
 LPTSTR from=str,to=str;

 while(*str)
 {
  if(*str!=' ')
  {
   *to=*from;
   if(from!=to) *from=' ';
   to++;
  }
  from++; str++;
 }
 *to=0;

 return to;
}

CString GetCmdLineParam(CString sCommandLine, CString sParamName)
{
	CString sParam;

	int n = sCommandLine.Find(sParamName);
	if (n != -1)
	{
		sParam = sCommandLine.Mid(n);
		n = sParam.Find(_T("\""));
		if (n != -1)	// Name enclosed in '\'
		{
			sParam = sParam.Mid(n + 1);
			n = sParam.Find(_T("\""));
			if (n != -1)
			{
				sParam = sParam.Left(n);
				return sParam;
			}
		}
		else			// Name 'as is'
		{
			n = sParam.Find(_T("="));
			if (n != -1)
			{
				sParam = sParam.Mid(n + 1);
				n = sParam.Find(_T(" "));
				if (n != -1)
				{
					sParam = sParam.Left(n);
					return sParam;
				}
			}
		}
	}
	
	return "";
}

////////////////////////////////////////////////////////////////////////////
// Registry functions
int GetStrRegVal(HKEY hKey, CString sName, CString& S)
{
	DWORD n = 1024;
	char Buf[1024];

	Buf[0] = 0;

	if (RegQueryValueEx(hKey, sName, NULL, NULL, (LPBYTE)Buf, &n) == ERROR_SUCCESS)	
	{
		S = Buf;
		return ERROR_SUCCESS;
	}

	return 1;
}		

int GetDWORDRegVal(HKEY hKey, CString sName, DWORD& N)
{
	DWORD n = 4;

	return RegQueryValueEx(hKey, sName, NULL, NULL, (LPBYTE)&N, &n);
}		

////////////////////////////////////////////////////////////////////////////
// Other functions
CString ComposeFileName(CString sSection, CString sKey, CString sDefFolder, CString sDefName)
{
	CString S = AfxGetApp()->GetProfileString(sSection, sKey, "");

	if (S != "")
	{
		if (_access(sDefFolder + S,0) == 0) return S;
		if (_access(S,0) == 0) return S;
	}
	else
	{
		S = sDefFolder + sDefName;
		if (_access(S,0) == 0) return S;
	}
	return "";
}
/////////////////////////////////////////////////////////////////////////////////////
// CopyFile - copies ffile with progress
// ----------------------------------------------------------------------------------
// return : 0 - if successful, 1 - otherwise
int CopyFile(CFile* lpSrc, CString sDst,DWORD dwBufSize, LPFNPROGRESS Progress)
{
	int rc = 1;
	char *pBuffer = NULL;
	int nBytesReaded;

	try
	{
		SetFileAttributes(sDst, FILE_ATTRIBUTE_NORMAL);

		CFile fDest(sDst, CFile::modeCreate | CFile::modeWrite);	
		pBuffer = (char*)malloc(dwBufSize);
		if (pBuffer)
		{
			while (TRUE)
			{
				nBytesReaded = lpSrc->Read(pBuffer, dwBufSize);
				if (nBytesReaded == 0) break;
				if (Progress) Progress(nBytesReaded);
				fDest.Write(pBuffer, nBytesReaded);
			}

			rc = 0;
		}

		fDest.Close();
	}
	catch(CFileException* /*pEX*/)
	{
	}

	if (pBuffer) free(pBuffer);

	return rc;
}

void HandleEvents()
{
	MSG Msg;
	if (PeekMessage(&Msg, 0, 0, 0, PM_REMOVE))
	{
		TranslateMessage(&Msg);
		DispatchMessage(&Msg);
	}
}

int DeleteThread(CWinThread* lpThread)
{
	if (lpThread != NULL)
	{
		lpThread->SuspendThread();
		delete lpThread;

		return 0;
	}
	return 1;
}

BOOL MakeFolder(CString sFolder)
{
	char c;
	CString sTmpFolder;

	for (int i = 0;i < sFolder.GetLength();i++)
	{
		c = sFolder[i];
		if (c == '\\')
		{
			if (_access(sTmpFolder, 0))
			{
				if (_mkdir(sTmpFolder)) return FALSE;
			}
		}
		
		sTmpFolder += sFolder[i];
	}

	return TRUE;
}

BOOL DeleteLockedFile(CString sFile)
{
	char szFileToDelete[MAX_PATH], szWININITFileName[MAX_PATH];
	HANDLE hDst;
	DWORD n;
	char szBuffer[1024];

	strcpy(szFileToDelete, sFile);

	if (!MoveFileEx(szFileToDelete, NULL, MOVEFILE_DELAY_UNTIL_REBOOT))
	{
		if (GetLastError() == ERROR_CALL_NOT_IMPLEMENTED)
		{
			GetShortPathName(szFileToDelete,szFileToDelete,MAX_PATH);

			GetWindowsDirectory(szWININITFileName, MAX_PATH);
			strcat(szWININITFileName, "\\WININIT.INI");
			hDst = CreateFile(szWININITFileName,GENERIC_WRITE,FILE_SHARE_READ, NULL, OPEN_ALWAYS, 0, NULL);
			if (hDst)
			{
				n = SetFilePointer(hDst, 0, 0, FILE_END);
	
				if (n == 0)
				{
					strcpy(szBuffer,"[rename]\r\n"); 
					WriteFile(hDst, szBuffer, strlen(szBuffer), &n, NULL);
				}
				strcpy(szBuffer,"NUL="); 
				strcat(szBuffer,szFileToDelete); 
				strcat(szBuffer,"\r\n"); 
				WriteFile(hDst, szBuffer, strlen(szBuffer), &n, NULL);
				CloseHandle(hDst);
			}
		}
	}
	return TRUE;
}

//////////////////////////////////////////////////////////////////////////////////////
// HANDLE hToken          access token handle
// LPCTSTR lpszPrivilege  name of privilege to enable/disable
// BOOL bEnablePrivilege  to enable or disable privilege
BOOL SetPrivilege(HANDLE hToken, LPCTSTR lpszPrivilege, BOOL bEnablePrivilege) 
{
	TOKEN_PRIVILEGES tp;
	LUID luid;

	if (!LookupPrivilegeValue(
			NULL,            // lookup privilege on local system
			lpszPrivilege,   // privilege to lookup 
			&luid)) 
	{      // receives LUID of privilege
		return FALSE; 
	}

	tp.PrivilegeCount = 1;
	tp.Privileges[0].Luid = luid;
	if (bEnablePrivilege)
		tp.Privileges[0].Attributes = SE_PRIVILEGE_ENABLED;
	else
		tp.Privileges[0].Attributes = 0;

	// Enable the privilege or disable all privileges.
	AdjustTokenPrivileges(hToken, FALSE, &tp, sizeof(TOKEN_PRIVILEGES), (PTOKEN_PRIVILEGES)NULL,(PDWORD)NULL); 
 
	// Call GetLastError to determine whether the function succeeded.
	if (GetLastError() != ERROR_SUCCESS) return FALSE; 

	return TRUE;
}
 
BOOL RebootWindows(DWORD dwFlag)
{
	HANDLE hProcess = OpenProcess(PROCESS_ALL_ACCESS, FALSE, GetCurrentProcessId());
	HANDLE hToken;
	OpenProcessToken(hProcess, TOKEN_ADJUST_PRIVILEGES, &hToken);
   
	SetPrivilege(hToken, SE_SHUTDOWN_NAME, TRUE);
	CloseHandle(hProcess);

	return ExitWindowsEx(dwFlag, 0);
}