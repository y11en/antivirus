//==============================================================================
// PRODUCT: COMMON
//==============================================================================
#ifndef __COMMON_DIALOG_H
#define __COMMON_DIALOG_H

#include "defs.h"
#include "TrackTooltip.h"
#include "util.h"

struct CCMapWrapper;

const int DIALOG_FLAGS_MOVE_CONTEXT = 1;
const int DIALOG_FLAGS_GRIPPER = 2;
const int DIALOG_FLAGS_MAKE_TOPMOST = 4;
const int DIALOG_FLAGS_PLACEMENT_NOSIZE = 8;

#define DECLARE_DIALOG_BASED()                                                  \
    void ShowDlgCtrl ( int nID, int nCmdShow )                                  \
    {                                                                           \
        CWnd * pWnd = GetDlgItem ( nID );                                       \
                                                                                \
        if   ( pWnd )                                                           \
        {                                                                       \
            pWnd -> ShowWindow ( nCmdShow );                                    \
        }                                                                       \
    }                                                                           \
                                                                                \
    void SetTextDlgCtrl ( int nID, LPCTSTR pszText )                            \
    {                                                                           \
        CWnd * pWnd = GetDlgItem ( nID );                                       \
                                                                                \
        if   ( pWnd )                                                           \
        {                                                                       \
            pWnd -> SetWindowText ( pszText );                                  \
        }                                                                       \
    }                                                                           \
                                                                                \
    void SetTextDlgCtrl ( int nID, const CString & text )                       \
    {                                                                           \
        CWnd * pWnd = GetDlgItem ( nID );                                       \
                                                                                \
        if   ( pWnd )                                                           \
        {                                                                       \
            pWnd -> SetWindowText ( text );                                     \
        }                                                                       \
    }                                                                           \
                                                                                \
    void EnableDlgCtrl ( int nID, bool bEnable )                                \
    {                                                                           \
        CWnd * pWnd = GetDlgItem ( nID );                                       \
                                                                                \
        if  ( pWnd )                                                            \
        {                                                                       \
            pWnd -> EnableWindow ( bEnable );                                   \
        }                                                                       \
    }                                                                           \
                                                                                \
    bool IsChecked ( UINT id )                                                  \
    {                                                                           \
        CButton * pButton = ( CButton * ) GetDlgItem ( id );                    \
                                                                                \
        if  ( pButton )                                                         \
        {                                                                       \
            return ( pButton -> GetState () == 1 );                             \
        }                                                                       \
        return false;                                                           \
    }                                                                           \
                                                                                \
    bool SetCheck ( UINT id, int nCheck = 1 )                                   \
    {                                                                           \
        CButton * pButton = ( CButton * ) GetDlgItem ( id );                    \
                                                                                \
        if  ( pButton )                                                         \
        {                                                                       \
            pButton -> SetCheck ( nCheck );                                     \
                                                                                \
            return true;                                                        \
        }                                                                       \
        return false;                                                           \
    }                                                                           \
                                                                                \
    BOOL CreateBoldFont ( UINT CtrlId, CFont & font )                           \
    {                                                                           \
        CWnd * pItem = GetDlgItem ( CtrlId );                                   \
                                                                                \
        if  ( pItem )                                                           \
        {                                                                       \
            CFont * pCurrFont = pItem -> GetFont ();                            \
                                                                                \
            if  ( pCurrFont )                                                   \
            {                                                                   \
                LOGFONT lf;                                                     \
                pCurrFont -> GetLogFont ( &lf );                                \
                lf.lfWeight = FW_BOLD;                                          \
                                                                                \
                return font.CreateFontIndirect ( &lf );                         \
            }                                                                   \
        }                                                                       \
        return FALSE;                                                           \
    }                                                                           \
                                                                                \
    void SetSmallFont ( UINT CtrlId, CFont & font )                             \
    {                                                                           \
        CWnd *pWnd = GetDlgItem(CtrlId);		                                \
        if(pWnd)																\
        {                                                                       \
			if(CreateKAVFont(pWnd, font, NULL, false, FW_NORMAL, KAV_FONT_SIZE_SMALL, false, NULL, OUT_OUTLINE_PRECIS)) \
				pWnd->SetFont(&font);											\
        }                                                                       \
    }                                                                           \
	void ShowAllCtrls (int nCmdShow)											\
	{																			\
		EnumChildWindows ( GetSafeHwnd (), EnumChildCtrls, ( LPARAM )nCmdShow );\
	}																			\
	static BOOL CALLBACK EnumChildCtrls(HWND hwnd, LPARAM lParam)				\
	{																			\
		::ShowWindow ( hwnd, (int)lParam );										\
		return TRUE;															\
	}																			\

//******************************************************************************
// class CCButton
//******************************************************************************
class COMMONEXPORT CCDialog : public CDialog
{
	DECLARE_DYNAMIC(CCDialog)
public:
    CCDialog ( UINT nIDTemplate, CWnd * pParent = NULL, _dword Flags = 0, LPCTSTR pszSectionName = NULL, CSize InitialSize = CSize(0, 0) );
	~CCDialog();
    
    virtual WINDOWPLACEMENT* LoadWindowPlacement ();
    virtual void SaveWindowPlacement ();
	
	UINT GetProfileInt( LPCTSTR lpszEntry, int nDefault);
	BOOL WriteProfileInt( LPCTSTR lpszEntry, int nValue);

    CString GetProfileString( LPCTSTR lpszEntry, LPCTSTR lpszDefault = NULL);
	BOOL WriteProfileString( LPCTSTR lpszEntry, LPCTSTR lpszValue);

    BOOL GetProfileBinary( LPCTSTR lpszEntry, LPBYTE* ppData, UINT* pBytes);
	BOOL WriteProfileBinary( LPCTSTR lpszEntry, LPBYTE pData, UINT nBytes);

    DECLARE_DIALOG_BASED()

    void MinRect ( CRect & rect )
    {
        m_MinRect = rect;
    }
    
    CRect MinRect ()
    {
        return m_MinRect;
    }

	virtual void OnSaveSettings(){}

	virtual int DoModal();
	virtual bool DoModalAsync();
	
	void NormalizeDlgFocus(CWnd* pWindow2Disable);

	void SetModalStyle ( bool bModalStyle )
    {
        m_bModalStyle = bModalStyle;
    }

	void EndDialog(int nResult);

	void SetCtrlTooltip(int nID, LPCTSTR szTooltip);
	void SetCtrlTooltip(HWND hWndCtrl, LPCTSTR szTooltip);


    //{{AFX_DATA(CCDialog)
    //}}AFX_DATA
    
    //{{AFX_VIRTUAL(CCDialog)
	public:
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	//}}AFX_VIRTUAL
    
protected:
    void DrawGripper ( HDC hDC );
    
    //{{AFX_MSG(CCDialog)
    afx_msg void OnLButtonDown ( UINT nFlags, CPoint point );
    afx_msg LRESULT OnNcHitTest ( CPoint point );
    virtual BOOL OnInitDialog();
    afx_msg void OnDestroy();
    afx_msg void OnPaint();
	afx_msg void OnGetMinMaxInfo(MINMAXINFO FAR* lpMMI);
	afx_msg void OnClose();
	afx_msg void OnTimer(UINT nIDEvent);
	//}}AFX_MSG
    DECLARE_MESSAGE_MAP()
		
	virtual LRESULT OnSetWindowPlacement(WPARAM, LPARAM);
	void DestroyModelessDialog();
	void OnOK();
	void OnCancel();
	void PostNcDestroy ();

protected:
    _dword m_Flags;

    CString m_SectionName;

    static const char * s_pszWindowPlacementKey;
	
	CSize m_InitialSize;

	HWND m_hWndParent;
	BOOL m_bEnableParent;
	HGLOBAL m_hAsyncDialogTemplate;

	bool m_bModalStyle;
	bool m_bModalAsync;
	
	bool m_bShow;
    
private:
    CRect m_MinRect;

//tooltips
	CCMapWrapper* m_pTooltipMap;

	CTitleTip m_TitleTip;
	CPoint m_ptMouseClientPos;
	HWND m_hwndChild;
	UINT TOOLTIP_TIMER_ID;
	std::basic_string<TCHAR> m_szTooltipNext;
};

#endif // __COMMON_DIALOG_H

//==============================================================================
