// VisualEditor.cpp : Defines the class behaviors for the application.
//

#include "stdafx.h"
#include <serialize/kldtser.h>
#include <Stuff\PathStr.h>
#include <Stuff\OCXReg.h>
#include <StuffIO\IOUtil.h>
#include "VisualEditor.h"
#include "VisualEditorDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

static void RegisterExt();

// ---
struct CRTableItem {
	const TCHAR *pszCLSID;
	const TCHAR *pszFileName;
	DWORD dwNotRegisteredID;
	DWORD dwRegistrationID;
	DWORD dwTitleID;
	DWORD dwFilerID;
	BOOL  bOCX;
};

// ---
static CRTableItem gCRTable[] = {
	{_T("{D482802A-8152-11D3-96B0-00104B5B66AA}"), _T("AvpSPL.ocx")  , IDS_ERR_OCXNOTREGISTERED, IDS_ERR_OCXREGISTRATION, IDS_ERR_OPENOCXTITLE, IDS_OCX_FILTER, TRUE},
	{_T("{D2B34AE8-B9CF-11D2-96B0-00104B5B66AA}"), _T("AvpCTT.ocx")  , IDS_ERR_OCXNOTREGISTERED, IDS_ERR_OCXREGISTRATION, IDS_ERR_OPENOCXTITLE, IDS_OCX_FILTER, TRUE},
	{_T("{60FE9D12-06EA-11D4-96B1-00D0B71DDDF6}"), _T("IDServer.dll"), IDS_ERR_COMNOTREGISTERED, IDS_ERR_COMREGISTRATION, IDS_ERR_OPENCOMTITLE, IDS_COM_FILTER, FALSE},
	{_T("{661A3E92-B16B-11D4-96B2-444553540000}"), _T("PGIServer.dll"),IDS_ERR_COMNOTREGISTERED, IDS_ERR_COMREGISTRATION, IDS_ERR_OPENCOMTITLE, IDS_COM_FILTER, FALSE},
};

static int gCRTableCount = sizeof(gCRTable) / sizeof(gCRTable[0]);

// ---
static BOOL RegisterRequiredCOM() {
	CRInfoStruct rcInfo;
	rcInfo.dwStructSize = sizeof(CRInfoStruct);
	rcInfo.hWndOwner = NULL;
	rcInfo.bLeaveLoaded	= TRUE;

	CString rcCaption;
	rcCaption.LoadString( IDS_MAINTITLE );
	rcInfo.pszMessageTitle = rcCaption;

	for ( int i=0; i<gCRTableCount; i++ ) {
		rcInfo.pszCLSID = gCRTable[i].pszCLSID;
		rcInfo.pszFileName = gCRTable[i].pszFileName;

		CString rcNotRegistered;
		rcNotRegistered.LoadString( gCRTable[i].dwNotRegisteredID );
		rcInfo.pszNotRegisteredErrorTxt = rcNotRegistered;

		CString rcRegistration;
		rcRegistration.LoadString( gCRTable[i].dwRegistrationID );
		rcInfo.pszRegistrationErrorTxt = rcRegistration;

		CString rcOpenTitle;
		rcOpenTitle.LoadString( gCRTable[i].dwTitleID );
		rcInfo.pszOpenDlgTitle = rcOpenTitle;

		CString rcOpenFilter;
		rcOpenFilter.LoadString( gCRTable[i].dwFilerID );
		rcInfo.pszOpenDlgFilter = rcOpenFilter;

		if ( (gCRTable[i].bOCX && !::CheckAndRegisterRequiredOCX(&rcInfo)) ||
				 (!gCRTable[i].bOCX && !::CheckAndRegisterRequiredCOM(&rcInfo))	)
			return FALSE;
	}
	return TRUE;
}

/////////////////////////////////////////////////////////////////////////////
// CVisualEditorApp

BEGIN_MESSAGE_MAP(CVisualEditorApp, CWinApp)
	//{{AFX_MSG_MAP(CVisualEditorApp)
	//}}AFX_MSG_MAP
	ON_COMMAND(ID_HELP, CWinApp::OnHelp)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CVisualEditorApp construction

CVisualEditorApp::CVisualEditorApp() {
}

/////////////////////////////////////////////////////////////////////////////
// The one and only CVisualEditorApp object

CVisualEditorApp theApp;

/////////////////////////////////////////////////////////////////////////////
// CVisualEditorApp initialization
// ---
BOOL CVisualEditorApp::InitInstance() {
	CoInitialize( 0 );
	AfxEnableControlContainer();

	// Standard initialization
	// If you are not using these features and wish to reduce the size
	//  of your final executable, you should remove from the following
	//  the specific initialization routines you do not need.

	if ( ::RegisterRequiredCOM() ) {

	#ifdef _AFXDLL
		//Enable3dControls();			// Call this when using MFC in a shared DLL
	#else
		//Enable3dControlsStatic();	// Call this when linking to MFC statically
	#endif

		RegisterExt();
		
		CVisualEditorDlg dlg;
		m_pMainWnd = &dlg;

		ProcessCommandLine();
		
		int nResponse = dlg.DoModal();
		if (nResponse == IDOK)
		{
			// TODO: Place code here to handle when the dialog is
			//  dismissed with OK
		}
		else if (nResponse == IDCANCEL)
		{
			// TODO: Place code here to handle when the dialog is
			//  dismissed with Cancel
		}
	}

	// Since the dialog has been closed, return FALSE so that we exit the
	//  application, rather than start the application's message pump.
	return FALSE;
}

void CVisualEditorApp::ProcessCommandLine(){

	CString strCmdLine(m_lpCmdLine);
	int pos;
	if(-1 != (pos = strCmdLine.Find(_T("-w"))) || -1 != (pos = strCmdLine.Find(_T("/w")))){

		CVisualEditorDlg* pDlg = (CVisualEditorDlg*)m_pMainWnd;
		pDlg->m_RegStorage.PutValue(TECH_EDITING_MODE_REG_VALUE, 1);

		// remove key from command line
		lstrcpy(m_lpCmdLine+pos, m_lpCmdLine+pos+2);
	}
}

#define PRT_EXT ".prt"
#define IMP_EXT ".imp"

static char *lpszExtNameTable[]  = {
	PRT_EXT,
	IMP_EXT
};

static char *lpszKeyNameTable[] = {
	"AVPPragueIntDefinitionFile",
	"AVPPragueIntImplementationFile"
};

static char *lpszDefNameTable[] = {
	"AVP Prague Interface Definition File",
	"AVP Prague Interface Implementation File"
};


// ---
static void RegisterExtInd( int nInd ) {

  DWORD dwDisposition = 0;
  HKEY  hKey = 0;

  char lpszCommand[_MAX_PATH];
	CPathStr lpszCommKeyName( lpszKeyNameTable[nInd] );
	lpszCommKeyName ^= "Shell\\Open\\Command";

  ::GetModuleFileName( NULL, lpszCommand, sizeof(lpszCommand) );
  _strlwr( lpszCommand );
  lstrcat( lpszCommand, " \"%1\"" );

  LONG res = ::RegCreateKeyEx( HKEY_CLASSES_ROOT, lpszExtNameTable[nInd], 0, REG_NONE, REG_OPTION_NON_VOLATILE, KEY_READ|KEY_WRITE, 0, &hKey, &dwDisposition );
  if ( ERROR_SUCCESS == res ) {

    ERROR_SUCCESS == ::RegSetValueEx( hKey, "", 0, REG_SZ, (BYTE*)lpszKeyNameTable[nInd], lstrlen(lpszKeyNameTable[nInd]) + 1 ) &&
    ERROR_SUCCESS == ::RegCloseKey( hKey ) &&
    ERROR_SUCCESS == ::RegCreateKeyEx( HKEY_CLASSES_ROOT, lpszKeyNameTable[nInd], 0, REG_NONE, REG_OPTION_NON_VOLATILE, KEY_READ|KEY_WRITE, 0, &hKey, &dwDisposition ) &&
    ERROR_SUCCESS == ::RegSetValueEx( hKey, "", 0, REG_SZ, (BYTE*)lpszDefNameTable[nInd], lstrlen(lpszDefNameTable[nInd]) + 1 ) &&
    ERROR_SUCCESS == ::RegCloseKey( hKey ) &&
    ERROR_SUCCESS == ::RegCreateKeyEx( HKEY_CLASSES_ROOT, lpszCommKeyName, 0, REG_NONE, REG_OPTION_NON_VOLATILE, KEY_READ|KEY_WRITE, 0, &hKey, &dwDisposition ) &&
    ERROR_SUCCESS == ::RegSetValueEx( hKey, "", 0, REG_SZ, (BYTE*)lpszCommand, lstrlen( lpszCommand ) + 1 );
    ::RegCloseKey( hKey );
  }
}

// ---
static void RegisterExt() {
	RegisterExtInd( 0 );
	RegisterExtInd( 1 );
}

// ---
int DlgMessageBox( CWnd *pWnd, LPCTSTR lpszText, UINT nType ) {
	CString rcWindowText;
	pWnd->GetWindowText( rcWindowText );

	// determine icon based on type specified
	if ((nType & MB_ICONMASK) == 0)	{
		switch (nType & MB_TYPEMASK)	{
			case MB_OK:
			case MB_OKCANCEL:
				nType |= MB_ICONEXCLAMATION;
				break;

			case MB_YESNO:
			case MB_YESNOCANCEL:
				nType |= MB_ICONEXCLAMATION;
				break;

			case MB_ABORTRETRYIGNORE:
			case MB_RETRYCANCEL:
				// No default icon for these types, since they are rarely used.
				// The caller should specify the icon.
				break;
		}
	}

	HWND hParent = NULL;
	if ( !pWnd || !(hParent = pWnd->GetSafeHwnd()) ) {
		HWND hWndTop;
		hParent = CWnd::GetSafeOwner_( NULL, &hWndTop );
	}

	return ::MessageBox( hParent, lpszText, rcWindowText, nType );
}

// ---
int DlgMessageBox( CWnd *pWnd, DWORD dwTextID, UINT nType ) {
	CString rcText;
	rcText.LoadString( dwTextID );

	return DlgMessageBox( pWnd, rcText, nType );
}


// ---
void *SerializeDTree( HDATA hData, DWORD &nSSize ) {
	nSSize = 0;
	if ( hData ) {
/*
		AVP_Serialize_Data rcSData;
		memset( &rcSData, 0, sizeof(rcSData) );

		::DATA_Serialize( hData, NULL, &rcSData, NULL, 0, &nSSize );

		void *pBuffer = (BYTE *)::CoTaskMemAlloc( sizeof(BYTE) * nSSize );
		::DATA_Serialize( hData, NULL, &rcSData, pBuffer, nSSize, &nSSize );
*/
		nSSize = ::KLDT_SerializeToMemoryUsingSWM( NULL, 0, hData, "" );
		void *pBuffer = (BYTE *)::CoTaskMemAlloc( sizeof(BYTE) * nSSize );
		::KLDT_SerializeToMemoryUsingSWM( (char *)pBuffer, nSSize, hData, "" );
		return pBuffer;
	}
	return NULL;
}

// ---
HDATA DeserializeDTree( void *pBuffer, DWORD nSize ) {
	if ( nSize ) {
		HDATA hData = NULL;
		if ( pBuffer ) {
			if ( *(DWORD *)pBuffer == AVP_SW_MAGIC ) 
				::KLDT_DeserializeFromMemoryUsingSWM( (AVP_char *)pBuffer, nSize, &hData );

			if ( !hData && *(WORD *)pBuffer == 0xadad ) {
				AVP_Serialize_Data rcSData;
				memset( &rcSData, 0, sizeof(rcSData) );
				hData = ::DATA_Deserialize( &rcSData, pBuffer, nSize, &nSize );
				//cout << "Input DTree deserialised to HDATA " << hex << hData << endl;
			}
		}
		return hData;
	}
	return NULL;
}


// ---
TCHAR *GetErrorString( HRESULT hError ) {
	TCHAR *pBuffer = new TCHAR[0x200];

  ::FormatMessage( FORMAT_MESSAGE_FROM_SYSTEM, 0, hError, 0, pBuffer, 0x200, 0 );

	return pBuffer;
}


//---
static void PathExtract( const TCHAR * pszFileName, TCHAR * pszDirName, int uicchBufferLen ) {
  if ( pszFileName && pszDirName ) {

		*pszDirName = NULL;

    char fDrive[_MAX_DRIVE];
    char fDir  [_MAX_DIR];

  // выделить диск и каталог
    _splitpath( pszFileName, fDrive, fDir, NULL, NULL);
    int dLen = lstrlen(fDir);
    if ( dLen > 1 && *(fDir + dLen - 1) == L'\\' )
      *((char*)fDir+dLen-1) = 0;

    if ( (lstrlen(fDrive) + lstrlen(fDir)) < (uicchBufferLen - 1) ) {
      lstrcpyn( pszDirName, fDrive, uicchBufferLen );
      int uichLen = lstrlen(pszDirName);
      pszDirName += uichLen;
      lstrcpyn( pszDirName, fDir, uicchBufferLen - uichLen );
    }  
  }
}


// ---
TCHAR *GetStartupDirectory() {
	CPathStr szPathName( _MAX_PATH );
	CPathStr szExeName( _MAX_PATH );
	::GetModuleFileName( NULL, szExeName, _MAX_PATH );

	::PathExtract( szExeName, szPathName, _MAX_PATH );

	return szPathName.Relinquish();
}



