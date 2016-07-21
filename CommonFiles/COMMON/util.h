//==============================================================================
// PRODUCT: COMMON
//==============================================================================
#ifndef __COMMON_UTIL_H
#define __COMMON_UTIL_H

#include "defs.h"
#include "FileDlg.h"
#include "CRestrictEdit.h"
#include "FontDefs.h"

#define IsCommonKeyDown( vk )	( (::GetKeyState(vk) & (1 << (sizeof(SHORT)*8-1))) != 0 )
#define IsCtrlDown()	IsCommonKeyDown(VK_CONTROL)
#define IsShiftDown()	IsCommonKeyDown(VK_SHIFT)
#define IsAltDown()		IsCommonKeyDown(VK_MENU)

//------------------------------------------------------------------------------
// 
//------------------------------------------------------------------------------
COMMONEXPORT void AddStrToCombo ( CComboBox & ComboBox, UINT StrID, DWORD Value, int & CurSel, DWORD DefaultValue );


//------------------------------------------------------------------------------
// 
//------------------------------------------------------------------------------
COMMONEXPORT int FindComboItemByData ( CComboBox & ComboBox, DWORD Value );

//------------------------------------------------------------------------------
// 
//------------------------------------------------------------------------------
#define PACKVERSION(major,minor) MAKELONG(minor,major)

COMMONEXPORT DWORD GetDllVersion(LPCTSTR lpszDllName);

//------------------------------------------------------------------------------
// 
//------------------------------------------------------------------------------
template < typename T >
bool GetCurSelComboValue ( CComboBox & ComboBox, T & value )
{
    int CurSel = ComboBox.GetCurSel ();
    
    if  ( CurSel != CB_ERR )
    {
        DWORD ItemData = ComboBox.GetItemData ( CurSel );
        
        if  ( ItemData != CB_ERR )
        {
            value = ( T ) ItemData;
            return true;
        }
    }
    return false;
}


//------------------------------------------------------------------------------
// 
//------------------------------------------------------------------------------
inline BOOL SetForegroundWindowEx ( HWND hWnd )
{
    HWND hCurrWnd = ::GetForegroundWindow ();
    int iTID = ::GetWindowThreadProcessId ( hWnd, 0 );
    int iCurrTID = ::GetWindowThreadProcessId ( hCurrWnd, 0 );
    
    AttachThreadInput ( iTID, iCurrTID, TRUE );
    
    BOOL bRet = ::SetForegroundWindow ( hWnd );
    
    AttachThreadInput ( iTID, iCurrTID, FALSE );
    
    BringWindowToTop ( hWnd );
    
    return bRet;
}

//------------------------------------------------------------------------------
// 
//------------------------------------------------------------------------------
inline BOOL SetForegroundWindowEx ( CWnd * pWnd )
{
    return SetForegroundWindowEx ( pWnd -> GetSafeHwnd () );
}

//------------------------------------------------------------------------------
// 
//------------------------------------------------------------------------------
inline void ChangeHeight ( CWnd * pWnd, int nNewHeight = 2 )
{	
    if  ( pWnd )
    {
        CRect rect;
        pWnd -> GetWindowRect ( &rect );	
        pWnd -> SetWindowPos ( NULL, 0, 0, rect.Width (), nNewHeight, SWP_NOMOVE | SWP_NOZORDER );
    }
}

inline CString LoadAndGetCString(DWORD dwID)
{
	CString szRet;
	VERIFY ( szRet.LoadString  ( dwID ) );
	return szRet;
}

inline void PumpMessagesFromQueue ()
{
	MSG msg;
	
	while (::PeekMessage(&msg, 0, 0, 0, PM_REMOVE))
	{
	   	::TranslateMessage(&msg);
		::DispatchMessage(&msg);
	}
}

class COMMONEXPORT CCFileDialogEx : public CCFileDialog
{
public:
	CCFileDialogEx ( 
			BOOL bOpenFileDialog, // TRUE for FileOpen, FALSE for FileSaveAs
			LPCTSTR lpszDefExt = NULL,
			LPCTSTR lpszFileName = NULL,
			DWORD dwFlags = OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT,
			LPCTSTR lpszFilter = NULL,
			CWnd * pParentWnd = NULL);

protected:
	virtual void OnInitDone();
};

namespace AutoStartProduct
{
	enum SetResult
	{
		asOK,
		asAccessDenied,
		asGeneralFail,
	};
	
	SetResult COMMONEXPORT SetAutoStartProductOption (bool bAutoStart);
}

void COMMONEXPORT BrowseFile ( CWnd * pParentWnd,
							   CString & strText,
                               LPCTSTR pszInitialDir, 
							   DWORD dwFlags = OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT | OFN_NOVALIDATE,
							   LPCTSTR szTitle = NULL );

void COMMONEXPORT BrowseFileMulti (CWnd * pParentWnd, 
									std::vector <std::string> &strFiles,
									DWORD dwFlags = OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT,
									LPCTSTR szTitle = NULL,
									LPCTSTR szInitialDir = NULL);
#define COD_EDITBOX			0x1
#define COD_FILEANDFOLDERS	0x2
#define COD_INITIALEMPTY	0x4

BOOL COMMONEXPORT BrowseFolder (CWnd *pParentWnd, CStdStringW &strText, UINT nHelpId, 
									LPCTSTR szTitle = NULL, LPCTSTR szNote = NULL, 
									CInputRestriction *pRestriction = NULL,
									long nType = 0,
									CStdStringW &szInitialPath = CStdStringW());

BOOL COMMONEXPORT BrowseSaveFile ( CWnd * pParentWnd,
									CString & strText,
									DWORD dwFlags = OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT | OFN_NOVALIDATE,
									LPCTSTR szTitle = NULL );

BYTE COMMONEXPORT GetCharsetByCodePage (int CodePage);

bool COMMONEXPORT CanUserStartServicePart ();
bool COMMONEXPORT CanRunProcessWithLogon();

bool COMMONEXPORT IsVersionXP();

bool COMMONEXPORT IsWin9x ();

bool COMMONEXPORT IsFontAvailable(HDC hDC, LPCTSTR lpszFamily);

void COMMONEXPORT RemoveSecondsFromDateTimeCtrl(HWND hDT);

BOOL COMMONEXPORT CreateKAVFont(CWnd *pWnd, CFont &font, CDC *pDC = NULL,
								bool bUnderline = false,
								LONG nWeight = FW_NORMAL,
								double fHeightPt = KAV_FONT_SIZE_DEFAULT,
								bool bItalic = false,
								LPCTSTR szFaceName = NULL,
								BYTE nPrecision = OUT_DEFAULT_PRECIS);

LPCTSTR COMMONEXPORT GetKAVFontFaceName();

#endif // __COMMON_UTIL_H

//==============================================================================
