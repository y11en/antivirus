//==============================================================================
// PRODUCT: COMMON
//==============================================================================
#ifndef __COMMON_WHITEBAR_DLG_H
#define __COMMON_WHITEBAR_DLG_H

#include "defs.h"
#include "Dialog.h"
#include "TransparentIco.h"
#include "HyperStatic.h"
#include "HtmlHelpSup.h"
#include "layout.h"

#define IDC_WHITEBAR	0xCF01
#define IDC_HELP_ICON_WHITEBAR	0xCF02
#define IDC_HELP_LINK_WHITEBAR	0xCF03


const int DIALOG_FLAGS_MOVE_STD_BTNS = 0x100;
const int DIALOG_FLAGS_FORCE_RENAME_STD_BTNS = 0x200;
const int DIALOG_FLAGS_DISABLE_HELP = 0x400;

const int WHITEBAR_NO_HELP_ID = -1;

//******************************************************************************
// 
//******************************************************************************
class COMMONEXPORT CCWhiteBarDlg : public CCDialog
{
	DECLARE_DYNAMIC(CCWhiteBarDlg)
public:
	CCWhiteBarDlg(UINT nIDTemplate, UINT nHelpId, CWnd * pParent = NULL, _dword Flags = DIALOG_FLAGS_MOVE_CONTEXT | DIALOG_FLAGS_MOVE_STD_BTNS, LPCTSTR pszSectionName = NULL, CSize InitialSize = CSize (0, 0) );   // standard constructor
		

	//{{AFX_DATA(CCWhiteBarDlg)
	//}}AFX_DATA

	//{{AFX_VIRTUAL(CCWhiteBarDlg)
	protected:
	virtual void DoDataExchange ( CDataExchange * pDX );
	//}}AFX_VIRTUAL

public:
	void ExternalInit ();
	
protected:
	//{{AFX_MSG(CCWhiteBarDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP ()

	DECLARE_USE_WND_HTMLHELP ()

protected:
	void SetLayout ( RULE* pLayoutRule);
	
    
	void SetWhiteBar ();
	void GetButtonReverseList ( std::vector < HWND > & btnList );
	void InsertIfExist ( std::vector < HWND > & btnList, DWORD dwIDC );

	virtual void OnSetMetricsDLU ();
	void SetMetricsDLU ( UINT wbh, UINT rmw, UINT sbw, UINT sbh, UINT sbmw );

	UINT GetWhiteBar_Height ();
	UINT GetRightMargin_Width ();
	UINT GetStdBtn_Width ();
	UINT GetStdBtn_Hight ();
	UINT GetStdBtnMargin_Width ();

	void CreateLayoutRule();
	void SetStdButtons();
	void OnHelpPress (NMHDR * pNotifyHeader, LRESULT * pResult );
	virtual void OnHelp();
	void AddCustomButton(DWORD dwID);
	RULE* GetLayoutRules();
		
	CStatic m_wndWhiteBar;
	CTransparentIco m_wndHelpIcon;
	CHyperStatic m_wndHelpLink;
	std::vector<HWND> m_btnList;
	std::vector<RULE> m_LayoutRule;
	std::vector<DWORD>	m_btnCustomList;
	RULE* m_pUserLayoutRule;

	UINT m_nHelpId;

private:
	UINT nWHITEBAR_HEIGHT;
	UINT nRIGHT_MARGIN;
	UINT nSTANDARD_BTN_WIDTH;
	UINT nSTANDARD_BTN_HIGHT;
	UINT nSTANDARD_BTN_MARGIN;

	friend class CResizableSheet;
};

#endif // __COMMON_WHITEBAR_DLG_H

//==============================================================================
