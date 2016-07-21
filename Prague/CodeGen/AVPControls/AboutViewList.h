#if !defined(AFX_ABOUTVIEWLIST_H__056927C1_32C5_11D3_96B0_00104B5B66AA__INCLUDED_)
#define AFX_ABOUTVIEWLIST_H__056927C1_32C5_11D3_96B0_00104B5B66AA__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// AboutViewList.h : header file
//

#include <Stuff\CPointer.h>
#include "CheckList.h"
#include "IconCache.h"
#include "ODMenu.h"        // CODMenu class declaration

// Дескриптор узла листа
// ---
class CAboutViewListItem : public CCheckListItem {
private :
	HICON		m_SmallIcon;
	int			m_SmallIconIndex;
	void   *m_Data;

	uint8   fOwnsData        : 1;  // Владеет данными (сам удаляет)
	uint8   fOwnsIcons       : 1;  // Владеет данными (сам удаляет)

public :
  CAboutViewListItem();

	virtual ~CAboutViewListItem();

	void	SetIcons( HICON hSmallIcon )     { m_SmallIcon = hSmallIcon; }
	void	SetIconIndexes( int iSmall )		 { m_SmallIconIndex = iSmall; }

	HICON GetSmallIcon() const { return m_SmallIcon; }
	
	int   GetSmallIconIndex() const { return m_SmallIconIndex; }

  void       SetOwnsData( bool set = true )						 { fOwnsData = set; }
  bool       IsOwnsData()															 { return !!fOwnsData; } 

  void       SetOwnsIcons( bool set = true )						 { fOwnsIcons = set; }
  bool       IsOwnsIcons()															 { return !!fOwnsIcons; } 

	void       SetData( void *pData, bool bOwnsData )		 { m_Data = pData; SetOwnsData( bOwnsData ); }
	void      *GetData()																 { return m_Data; }	

	void       Empty();

	virtual int		GetImageIndex();

	friend class CAboutViewList; 
};


/////////////////////////////////////////////////////////////////////////////
// CAboutViewList window

class CAboutViewList : public CCheckList {
protected :
  CImageList			m_SmallImageList;
	CHIconCache			m_SmallIconCache;
	CSize           m_nSmallImageSize;

	CString         m_FindContext;

	uint8           fUseItemImages		   :	1; // Использовать иконы на элементах

// Construction
public:
	CAboutViewList();

// Attributes
public:

	void SetUseItemImages( bool set = true )   { fUseItemImages = set; }
  bool IsUseItemImages()                     { return !!fUseItemImages; } 

// Operations
public:

	virtual void  AddNewItem( CCheckListItem *pItem, bool bEnsureVisible );

					void  FindFirstItem( const char *pFindContext );
					void  FindNextItem();
// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CAboutViewList)
	public:
	virtual COLORREF SetBkColor( COLORREF crColor );
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CAboutViewList();

	// Generated message map functions
protected:

					void	CreateStandartImageList();
	virtual void	CreateImageList();
	virtual void	AddItem( int iItemIndex );
					void	SetItemIcons( int iItemIndex );
									
					bool	CreatePopupMenu( CPoint& point, CPointer<CODMenu> &menu, int &nItem );
					void	CreateAndTrackPopupMenu( CPoint& point );

					void	FindItem();

	//{{AFX_MSG(CAboutViewList)
	afx_msg void OnDestroy();
	afx_msg void OnContextMenu(CWnd* pWnd, CPoint point);
	afx_msg void OnFindItem();
	afx_msg void OnFindNextItem();
	afx_msg void OnItemChanging(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnItemChanged(NMHDR* pNMHDR, LRESULT* pResult);
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_REPORTVIEWLIST_H__056927C1_32C5_11D3_96B0_00104B5B66AA__INCLUDED_)
