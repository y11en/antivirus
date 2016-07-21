// AvpEditView.h : interface of the CAvpEditView class
//
/////////////////////////////////////////////////////////////////////////////
#include "FindDlg.h"

class CAvpEditView : public CListView
{
protected: // create from serialization only
	CFindDlg FindDlg;
	int LastInsertType;
	CAvpEditView();
	DECLARE_DYNCREATE(CAvpEditView)


// Attributes
public:
	CAvpEditDoc* GetDocument();

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CAvpEditView)
	public:
	virtual void OnDraw(CDC* pDC);  // overridden to draw this view
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	protected:
	virtual void OnInitialUpdate(); // called first time after construct
	virtual BOOL OnPreparePrinting(CPrintInfo* pInfo);
	virtual void OnBeginPrinting(CDC* pDC, CPrintInfo* pInfo);
	virtual void OnEndPrinting(CDC* pDC, CPrintInfo* pInfo);
	virtual void OnUpdate(CView* pSender, LPARAM lHint, CObject* pHint);
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CAvpEditView();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:

// Generated message map functions
protected:
	//{{AFX_MSG(CAvpEditView)
	afx_msg void OnKeydown(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnEditInsertrecord();
	afx_msg void OnEditDeleterecord();
	afx_msg void OnEditEditrecord();
	afx_msg void OnDblclk(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnEditCopy();
	afx_msg void OnEditCut();
	afx_msg void OnEditPaste();
	afx_msg void OnUpdateEditPaste(CCmdUI* pCmdUI);
	afx_msg void OnUpdateEditCut(CCmdUI* pCmdUI);
	afx_msg void OnUpdateEditCopy(CCmdUI* pCmdUI);
	afx_msg void OnFilePack();
	afx_msg void OnGetdispinfo(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnWindowNext();
	afx_msg void OnFileSavereload();
	afx_msg void OnItemchanging(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnEditFind();
	afx_msg void OnEditFindagain();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

#ifndef _DEBUG  // debug version in AvpEditView.cpp
inline CAvpEditDoc* CAvpEditView::GetDocument()
   { return (CAvpEditDoc*)m_pDocument; }
#endif

/////////////////////////////////////////////////////////////////////////////
