//
// ConfigDlg.h
//

#ifndef __CONFIGDLG_H__
#define __CONFIGDLG_H__

#pragma warning(disable : 4786)

#include <windows.h>
#include <map>

//--------------------------- CConfigDlg ----------------------------

class CConfigDlg
{
    typedef std::map<HWND, CConfigDlg*> DlgMap_t ;

public:
    CConfigDlg(HWND a_hParentWnd = NULL) ;

    void Run() ;

private:
    BOOL OnOK() ;

    BOOL OnCancel() ;

    BOOL OnInitDialog() ;

    BOOL IsWindow() ;

    BOOL EndDialog(int a_nResult) ;
    
    BOOL OnCommand(BOOL a_blMessageFromAccelerator, WORD a_wItemID, HWND a_hControl) ;
    
    INT_PTR WndProc(UINT a_uMsg, WPARAM a_wParam, LPARAM a_lParam) ;

private:
    static INT_PTR CALLBACK WndProcS(HWND a_hwndDlg, UINT a_uMsg, WPARAM a_wParam, LPARAM a_lParam) ;

private:
    HWND m_hParentWnd ;
    HWND m_hWnd ;

private:
    static DlgMap_t s_mapDlg ;
};

//-------------------------------------------------------------------

#endif // __CONFIGDLG_H__