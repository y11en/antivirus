//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//

#ifndef AFX_LEFTVIEW_H__E27F8183_CBD3_40B7_8AB0_216367217918__INCLUDED_
#define AFX_LEFTVIEW_H__E27F8183_CBD3_40B7_8AB0_216367217918__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//

#include "CtrlExt.h"

class CRegEditDoc;
class CLeftView : public CTreeView
{
protected: 
	CLeftView();
	DECLARE_DYNCREATE(CLeftView)

public:
	CRegEditDoc* GetDocument();

	//{{AFX_VIRTUAL(CLeftView)
	public:
	virtual void OnDraw(CDC* pDC);  
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	protected:
	virtual void OnInitialUpdate(); 
	//}}AFX_VIRTUAL

public:

	cRegistry* reg();
	BOOL RegGetKeyCount		   ( tDWORD& o_dwKeyCount, tRegKey i_oKey);
	BOOL RegOpenKeyByIndex	 ( tRegKey i_oParentKey, tRegKey& o_oKey, tDWORD i_dwIndex);
	BOOL RegGetKeyNameByIndex( tRegKey i_oKey, tDWORD i_dwIndex, CString& o_strKeyName );
	BOOL RegCloseKey		( tRegKey i_oKey );
	BOOL RegDeleteKey		( tRegKey i_oKey, LPCTSTR i_strSubKey );
	BOOL RegCopyKey			( tRegKey i_oFrom, tRegKey o_oTo );

	tRegKey openKey( CTreeCursor& node );
	BOOL    createSubKey( tRegKey i_oParentKey, tRegKey* sub_key, LPCTSTR i_strNewKeyName, bool create_def_val );
	void OnNewKey();

	virtual ~CLeftView();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:
	CTreeCursor m_tRoot;
	CTreeCursor	m_tCurrent;
	CTreeCursor	m_tPrevious;

	CImageList  m_lstImage;

	int			m_nCreateCount;

	bool    GetPath( CTreeCursor& node, CString& path );
	BOOL    PopulateSubTree( CTreeCursor& node );
	void    UpdateStatus();
	BOOL    IsCurrentRoot();
	BOOL    IsCurrentEditable();

	BOOL RenameItem(LPCTSTR i_strNewName);
	void DeleteItem();
	void AppendNewItem();

	typedef BOOL (*pfnFindFirst)(CTreeCursor i_tCur, void* i_pData);	
	CTreeCursor FindFirst(pfnFindFirst i_pfn, void* i_pData, CTreeCursor i_tStartFrom, BOOL i_bProcessParent=TRUE);

	//{{AFX_MSG(CLeftView)
	afx_msg void OnSelChanged(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg void OnExpand( NMHDR* pNMHDR, LRESULT* pResult );
	afx_msg void OnBeginLabelEdit(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnEndLabelEdit(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnUpdateRenameItem(CCmdUI* pCmdUI);
	afx_msg void OnRenameItem();
	afx_msg void OnUpdateDeleteItem(CCmdUI* pCmdUI);
	afx_msg void OnDeleteItem();
	afx_msg void OnUpdateNewKey(CCmdUI* pCmdUI);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

#ifndef _DEBUG  // debug version in LeftView.cpp
inline CRegEditDoc* CLeftView::GetDocument()
   { return (CRegEditDoc*)m_pDocument; }
#endif

//{{AFX_INSERT_LOCATION}}
//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//

#endif 
