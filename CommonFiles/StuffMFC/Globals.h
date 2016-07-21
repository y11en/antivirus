////////////////////////////////////////////////////////////////////////////////////////
// global functions and variables declarations
#ifndef GLOBALS_H
#define GLOBALS_H

#define RC_OK           0
#define RC_ABORT        1
#define RC_RETRY        2
#define RC_FAIL         3
#define RC_ERROR        4

// registry entry
#define REG_SHARED "Software\\KasperskyLab\\SharedFiles"
#define REG_COMPONENTS "Software\\KasperskyLab\\Components"

typedef
        int (*LPFNPROGRESS)(DWORD dwCurPos);

union CONV {
	DWORD dw;
	struct {
		WORD wLo, wHi;
	};
	struct {
		BYTE bLoLo, bLoHi, bHiLo, bHiHi;
	};
};

extern int STR_ID_BASE;
extern CString sErrDlgTitle, sInfoDlgTitle;
extern BOOL boManualMode;
extern HINSTANCE hLocalizationLibrary; // handle of localization dll

////////////////////////////////////////////////////////////////////////////////////////
// global functions declarations
BOOL MakeFolder(CString sFolder);

CString ComposeFileName(CString sSection, CString sKey, CString sDefFolder, CString sDefName);
HINSTANCE LoadLocLibrary(CString sAVPLocDLLPath, CString sLocName);
BOOL FreeLocLibrary();
CString LoadStr(int nID);

CString ClosePath(CString& S, char cSlash);

///////////////////////////////////////////////////////////////////////////////
// GetPath - returns path from full file name
// ----------------------------------------------------------------------------
// CString sFileName	- full name with path
//
// Returns : path if successfull or "" otherwise
CString GetPath(CString sFileName);

///////////////////////////////////////////////////////////////////////////////
// GetName - returns file name from full file name
// ----------------------------------------------------------------------------
// CString sFileName	- full file name with path
//
// Returns : file name if successfull or "" otherwise
CString GetName(CString sFileName);

///////////////////////////////////////////////////////////////////////////////
// GetExt - returns extention from file name
// ----------------------------------------------------------------------------
// CString sFileName	- file name
//
// Returns : extention if successfull or "" otherwise
CString GetExt(CString sFileName);

///////////////////////////////////////////////////////////////////////////////
// GetFileTitle - returns file name without extentions from file name 
// ----------------------------------------------------------------------------
// CString sFileName	- full name without path
//
// Returns : file name if successfull or "" otherwise
CString GetFileTitle(CString sFileName);

int Process_ZIP(CString sSrcFileName, CString sDestFolder);
int CopyFile(CFile* lpSrc, CString sDst, DWORD dwBufSize, LPFNPROGRESS Progress = NULL);

void LocalizeMsgBoxTitles(int nErrorDlgTitle, int nInfoDlgTitle);
CString PrepareMessage(LONG resMsg, CString sParam);
int ErrorMessageBox(HWND hWnd, LONG resMsg, CString sParam = "");
int InfoMessageBox(HWND hWnd, LONG resMsg, CString sParam = "");

CString GetCmdLineParam(CString sCommandLine, CString sParamName);
CString FormatStr(int nFormat, CString sParam);
int Pos(CString S, CString sSubStr);
BOOL IsValidNumber(CString S);
DWORD HexStrToDWORD(LPCTSTR str, int len);
BOOL CutEnclosedStr(CString& sSrc, CString& sDst, char cBeg, char cEnd);
LPTSTR GetEnclosedStr(LPCTSTR szSrc, LPTSTR szDst, char cBeg, char cEnd);
LPTSTR RemoveSpaces(LPTSTR str);
CString SpanIncluding(CString& sSrc, CString sMask);

/////////////////////////////////////////////////////////////////////////////
// CheckStrForValidChars - Checks source string for valid characters
// --------------------------------------------------------------------------
// CString sSrc				- string to check
// CString sValidChars		- string which contains valid characters
// return					TRUE - if string is valid, FALSE - otherwise
BOOL CheckStrForValidChars(CString sSrc, CString sValidChars);

void HandleEvents();
void CALLBACK EXPORT TimerProc(HWND hWnd, UINT nMsg, UINT nIDEvent, DWORD dwTime);
int DeleteThread(CWinThread* lpThread);

int GetStrRegVal(HKEY hKey, CString sName, CString& S);
int GetDWORDRegVal(HKEY hKey, CString sName, DWORD& N);

BOOL DeleteLockedFile(CString sFile);

BOOL RebootWindows(DWORD dwFlag);

#endif