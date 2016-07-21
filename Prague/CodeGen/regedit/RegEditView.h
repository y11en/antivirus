//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//

#ifndef AFX_REGEDITVIEW_H__FF506D0E_7FA9_4D94_98CB_9BE831FBCA31__INCLUDED_
#define AFX_REGEDITVIEW_H__FF506D0E_7FA9_4D94_98CB_9BE831FBCA31__INCLUDED_

//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//

class CRegEditView : public CListView
{
protected: 
	CRegEditView();
	DECLARE_DYNCREATE(CRegEditView)

public:
	CRegEditDoc* GetDocument();
	CString GetColumnsInfo();

	//{{AFX_VIRTUAL(CRegEditView)
	public:
	virtual void OnDraw(CDC* pDC);  
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	protected:
	virtual void OnInitialUpdate(); 
	virtual void OnUpdate(CView* pSender, LPARAM lHint, CObject* pHint);
	//}}AFX_VIRTUAL

public:
	virtual ~CRegEditView();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:
	CImageList  m_lstImage;
	DWORD		m_dwCurKey;

	void UpdateKeyValues	(DWORD i_dwKey);

	void insert( CListCtrl& list, CString& name, DWORD ins, DWORD idx );
	BOOL GetValueCount		(tDWORD& o_dwCount);
	BOOL GetValueName		(tDWORD i_dwIdx, CString& o_strName);
	BOOL GetValueTypeAndVal	(tDWORD i_dwIdx, tTYPE_ID& o_nType, BYTE* o_pBuf, tDWORD& io_nBufSize);
	BOOL SetValueTypeAndVal	(tDWORD i_dwIdx, tTYPE_ID i_nType, BYTE* i_pBuf, tDWORD i_nBufSize);
	BOOL AppendValue		(LPCTSTR i_strValName, tTYPE_ID i_nType, BYTE* i_pBuf, tDWORD i_nBufSize);
	BOOL DeleteValue		(tDWORD i_dwIdx);
	BOOL AppendValue		(tTYPE_ID i_nType);
	
	int  GetSelectedItem();
	void EditCurrentValue();
	void AdjustItemDataOnDeletion(DWORD i_dwItemDeleted);
	void PerformRename(int i_nItem, LPCTSTR i_strNewName);

	//{{AFX_MSG(CRegEditView)
	afx_msg void OnLButtonDblClk(UINT nFlags, CPoint point);
	afx_msg void OnUpdateModifyValue(CCmdUI* pCmdUI);
	afx_msg void OnModifyValue();
	afx_msg void OnContextMenu(CWnd* pWnd, CPoint point);
	afx_msg void OnUpdateDeleteItem(CCmdUI* pCmdUI);
	afx_msg void OnDeleteItem();
	afx_msg void OnUpdateRenameItem(CCmdUI* pCmdUI);
	afx_msg void OnRenameItem();
	afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg void OnEndLabelEdit(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnUpdateNewStringItem(CCmdUI* pCmdUI);
	afx_msg void OnNewStringItem();
	afx_msg void OnUpdateNewDwordItem(CCmdUI* pCmdUI);
	afx_msg void OnNewDwordItem();
	afx_msg void OnUpdateNewBinaryItem(CCmdUI* pCmdUI);
	afx_msg void OnNewBinaryItem();
	//}}AFX_MSG
	afx_msg void OnStyleChanged(int nStyleType, LPSTYLESTRUCT lpStyleStruct);
	DECLARE_MESSAGE_MAP()
};

#ifndef _DEBUG  // debug version in RegEditView.cpp
inline CRegEditDoc* CRegEditView::GetDocument()
   { return (CRegEditDoc*)m_pDocument; }
#endif

//{{AFX_INSERT_LOCATION}}
//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//

#endif // 
