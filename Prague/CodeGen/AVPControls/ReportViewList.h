#if !defined(AFX_REPORTVIEWLIST_H__056927C1_32C5_11D3_96B0_00104B5B66AA__INCLUDED_)
#define AFX_REPORTVIEWLIST_H__056927C1_32C5_11D3_96B0_00104B5B66AA__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// ReportViewList.h : header file
//

#include <CheckList.h>
#include <IconCache.h>
#include <Stuff\CPointer.h>
#include <Stuff\SArray.h>
#include <ODMenu.h>        // CODMenu class declaration

// Дескриптор узла листа
// ---
class CReportViewListItem : public CCheckListItem {
private :
	HICON		m_SmallIcon;
	int			m_SmallIconIndex;

	uint8   fManuallyChecked : 1;  // Включен вручную
	uint8   fOwnsIcons       : 1;  // Владеет данными (сам удаляет)

public :
  CReportViewListItem();

	virtual ~CReportViewListItem();

	void	SetIcons( HICON hSmallIcon )     { m_SmallIcon = hSmallIcon; }
	void	SetIconIndexes( int iSmall )		 { m_SmallIconIndex = iSmall; }

  void       SetOwnsIcons( bool set = true )						 { fOwnsIcons = set; }
  bool       IsOwnsIcons()															 { return !!fOwnsIcons; } 

	HICON GetSmallIcon() const { return m_SmallIcon; }
	
	int   GetSmallIconIndex() const { return m_SmallIconIndex; }

  void       SetManuallyChecked( bool set = true )		 { fManuallyChecked = set; }
  bool       IsManuallyChecked()											 { return !!fManuallyChecked; } 

	void       Empty();

	virtual int		GetImageIndex();

	friend class CReportViewList; 
};


// ---
struct CReportViewListCacheItem {
	int  m_nIndex;
	bool m_bUsed;
	CReportViewListCacheItem( int nIndex ) :
		m_nIndex( nIndex ),
		m_bUsed( true ) {}
};

/////////////////////////////////////////////////////////////////////////////
// CReportViewList window

class CReportViewList : public CCheckList {
protected :
	int             m_nLastSelectedSubItem;

  CImageList			m_SmallImageList;
	CHIconCache			m_SmallIconCache;
	CSize           m_nSmallImageSize;

	CString         m_FindContext;
	bool            m_bItemFound;
	int             m_bItemFoundInd;
	
	CSArray<CReportViewListCacheItem>	m_ItemsCache;

	uint8           fUseItemImages		   :	1; // Использовать иконы на элементах
	uint8           fDontAddItemToList	 :	1; // Не добавлать элемент в лист немедленно
	uint8           fExternalFillingItem :	1; // Данные элементов хранятся где-то снаружи

// Construction
public:
	CReportViewList();

// Attributes
public:

	void SetUseItemImages( bool set = true )   { fUseItemImages = set; }
  bool IsUseItemImages()                     { return !!fUseItemImages; } 

	void SetDontAddItemToList( bool set = true )				{ fDontAddItemToList = set; }
  bool IsDontAddItemToList()													{ return !!fDontAddItemToList; } 

	void SetExternalFillingItem( bool set = true )			{ fExternalFillingItem = set; }
  bool IsExternalFillingItem()												{ return !!fExternalFillingItem; } 

// Operations
public:

					void  SetSortByColumns( bool bSet );

	virtual void  AddNewItem( CCheckListItem *pItem, bool bEnsureVisible, bool bAddToList = true );
	virtual void  AddItemsToList( bool bEnsureVisible, bool bInternal = false );

					void  FindFirstItem( const TCHAR *pFindContext );
					void  FindNextItem();
					bool  IsItemFound() { return m_bItemFound; }

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CReportViewList)
	public:
	virtual COLORREF SetBkColor( COLORREF crColor );
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CReportViewList();

	// Generated message map functions
protected:

	// Check/Uncheck Item
	virtual	void	CheckItem( int ind );

					void	CreateStandartImageList();
	virtual void	CreateImageList();
	virtual void	AddItem( int iItemIndex );
					void	SetItemIcons( int iItemIndex );
									
					bool	CreatePopupMenu( CPoint& point, CPointer<CODMenu> &menu, int &nItem, int &nSubItem );
					void	CreateAndTrackPopupMenu( CPoint& point );

					void	FindItem();

					void	FlushItemCache();
					bool	PrepareCachedItem( int nItemIndex );
					void	PrepareItemsCache( int nIndexFrom, int nIndexTo );

					void	DrawCellFocusRect( CPoint &point );

	//{{AFX_MSG(CReportViewList)
	afx_msg void		OnGetDispInfo(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void		OnDestroy();
	afx_msg void		OnColumnClick(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void		OnContextMenu(CWnd* pWnd, CPoint point);
	afx_msg void	  OnItemChanging(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void	  OnItemChanged(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void		OnFindItem();
	afx_msg void		OnFindNextItem();
	afx_msg void		OnOdCacheHint(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void		OnSetFocus(CWnd* pOldWnd);
	afx_msg void		OnKillFocus(CWnd* pNewWnd);
	afx_msg LRESULT OnGetMenuItemBmp( WPARAM wParam, LPARAM lParam );
	afx_msg void		OnLButtonDown( UINT nFlags, CPoint point );
	afx_msg void		OnRButtonDown( UINT nFlags, CPoint point );
	afx_msg void		OnPaint();
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_REPORTVIEWLIST_H__056927C1_32C5_11D3_96B0_00104B5B66AA__INCLUDED_)
