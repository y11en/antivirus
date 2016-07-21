//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//

#ifndef AFX_MAINFRM_H__12BCAAB0_519F_45B7_86DC_62FCB03A6C97__INCLUDED_
#define AFX_MAINFRM_H__12BCAAB0_519F_45B7_86DC_62FCB03A6C97__INCLUDED_

//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//

class CRegEditView;

class CMainFrame : public CFrameWnd
{
	
protected: 
	CMainFrame();
	DECLARE_DYNCREATE(CMainFrame)

	CSplitterWnd	m_wndSplitter;
	CSize			m_szOnePane;
	CSize			m_szTwoPane;

	CString			GetSplitterInfo();				
	void			RestoreSplitterInfo(CString str);

	CString			m_strStatusIdleMsg;
	virtual void GetMessageString(UINT nID, CString& rMessage) const;

public:

	void		SavePlacement		(); 
	void		RestorePlacement	(int& cmdShow); 
	void		SetStatusIdleMsg	(LPCTSTR i_strMsg);

	//{{AFX_VIRTUAL(CMainFrame)
	public:
	virtual BOOL OnCreateClient(LPCREATESTRUCT lpcs, CCreateContext* pContext);
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	//}}AFX_VIRTUAL

public:
	virtual ~CMainFrame();
	CRegEditView* GetRightPane();
	void OnNewKey();

#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:  
	CStatusBar  m_wndStatusBar;
	CToolBar    m_wndToolBar;

protected:
	//{{AFX_MSG(CMainFrame)
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnClose();
	//}}AFX_MSG
	afx_msg void OnUpdateViewStyles(CCmdUI* pCmdUI);
	afx_msg void OnViewStyle(UINT nCommandID);
	DECLARE_MESSAGE_MAP()
};

//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//

CMainFrame* GlobalGetMainWnd();

//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//

//{{AFX_INSERT_LOCATION}}

#endif 
