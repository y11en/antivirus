//
// ConfigDlg.h
//

#pragma warning(disable : 4786)

#include "ConfigDlg.h"

#include "resource.h"

//--------------------------- CConfigDlg ----------------------------

CConfigDlg::DlgMap_t CConfigDlg::s_mapDlg ;

CConfigDlg::CConfigDlg(HWND a_hParentWnd /*= NULL*/)
    : m_hParentWnd(a_hParentWnd),
      m_hWnd(NULL)
{
}

void CConfigDlg::Run()
{
    HRSRC hResource = ::FindResource(::GetModuleHandle(NULL), MAKEINTRESOURCE(IDD_CONFIGURE), RT_DIALOG) ;
    if (!hResource)
    {
        return ;
    }
    
    LPDLGTEMPLATE pTemplate = (LPDLGTEMPLATE)::LoadResource(::GetModuleHandle(NULL), hResource) ;
    if (!pTemplate)
    {
        return ;
    }
    
    ::DialogBoxIndirectParam(::GetModuleHandle(NULL), pTemplate, m_hParentWnd, WndProcS, (LPARAM)this) ;
}

BOOL CConfigDlg::OnOK()
{
    return EndDialog(IDOK) ;
}

BOOL CConfigDlg::OnCancel()
{
    return EndDialog(IDCANCEL) ;
}

BOOL CConfigDlg::OnInitDialog()
{
    return TRUE ;
}

BOOL CConfigDlg::IsWindow()
{
    return ::IsWindow(m_hWnd) ;
}

BOOL CConfigDlg::EndDialog(int a_nResult)
{
    if (!IsWindow())
    {
        return FALSE ;
    }

    return ::EndDialog(m_hWnd, a_nResult) ;
}

BOOL CConfigDlg::OnCommand(BOOL a_blMessageFromAccelerator, WORD a_wItemID, HWND a_hControl)
{
    switch (a_wItemID)
    {
    case IDOK:
        return OnOK() ;

    case IDCANCEL:
        return OnCancel() ;
    }

    return FALSE ;
}

INT_PTR CConfigDlg::WndProc(UINT a_uMsg, WPARAM a_wParam, LPARAM a_lParam)
{
    switch (a_uMsg)
    {
    case WM_COMMAND:
        return OnCommand(HIWORD(a_wParam), LOWORD(a_wParam), (HWND)a_lParam) ;

    case WM_INITDIALOG:
        return OnInitDialog() ;
    }
    
    return FALSE ;
}

INT_PTR CConfigDlg::WndProcS(HWND a_hwndDlg, UINT a_uMsg, WPARAM a_wParam, LPARAM a_lParam)
{
    switch (a_uMsg)
    {
    case WM_INITDIALOG:
        {
            CConfigDlg* pWnd = reinterpret_cast<CConfigDlg*>((LPVOID)a_lParam) ;
            if (pWnd)
            {
                s_mapDlg[a_hwndDlg] = pWnd ;
                pWnd->m_hWnd = a_hwndDlg ;
                return pWnd->WndProc(a_uMsg, a_wParam, a_lParam) ;
            }
        }
        break ;

    case WM_DESTROY:
    default:
        {
            DlgMap_t::iterator It = s_mapDlg.find(a_hwndDlg) ;
            if (It != s_mapDlg.end())
            {
                INT_PTR nResult = (*It).second->WndProc(a_uMsg, a_wParam, a_lParam) ;
                if (a_uMsg == WM_NCDESTROY)
                {
                    s_mapDlg.erase(It) ;
                }

                return nResult ;
            }
        }
        break ;
    }

    return FALSE ;
}

//-------------------------------------------------------------------