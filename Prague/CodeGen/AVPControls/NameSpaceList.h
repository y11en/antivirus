#if !defined(AFX_NAMESPACELIST_H__D8037F24_ABAF_11D2_96B0_00104B5B66AA__INCLUDED_)
#define AFX_NAMESPACELIST_H__D8037F24_ABAF_11D2_96B0_00104B5B66AA__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// NameSpaceList.h : header file
//

#include <Stuff\CPointer.h>
#include <Stuff\PArray.h>
#include "CheckList.h"
#include "IconCache.h"
#include "ODMenu.h"        // CODMenu class declaration

// Дескриптор узла листа
// ---
class CNameSpaceListItem : public CCheckListItem {
private :
	HICON		m_LargeIcon;
	HICON		m_SmallIcon;
	int			m_LargeIconIndex;
	int			m_SmallIconIndex;

	uint8   fStrict          : 2;  // Strict/Non Strict/Some Strict 
	uint8   fManuallyChecked : 1;  // Включен вручную
	uint8   fOwnsIcons       : 1;  // Владеет данными (сам удаляет)

public :
  typedef enum {
    ie_NonStrict = 0,
    ie_Strict, 
    ie_SomeStrict  
  } CieStrict;

  typedef enum {
		iti_Unchecked = 0,		// 0
		iti_Checked,					// 1
		iti_LastIndex,				// 2
	} CieLImageIndex;

  CNameSpaceListItem();

	virtual ~CNameSpaceListItem();

	virtual bool operator == ( const CCheckListItem &rItem );

	virtual void Assign( const CCheckListItem &rItem );

	virtual CCheckListItem *Duplicate();

	void	SetIcons( HICON hSmallIcon, HICON hLargeIcon )     { m_LargeIcon = hLargeIcon; m_SmallIcon = hSmallIcon; }
	void	SetIconIndexes( int iSmall, int iLarge )					 { m_LargeIconIndex = iLarge; m_SmallIconIndex = iSmall; }

  void       SetOwnsIcons( bool set = true )						 { fOwnsIcons = set; }
  bool       IsOwnsIcons()															 { return !!fOwnsIcons; } 

	HICON GetLargeIcon() const { return m_LargeIcon; }
	HICON GetSmallIcon() const { return m_SmallIcon; }
	
	int   GetLargeIconIndex() const { return m_LargeIconIndex; }
	int   GetSmallIconIndex() const { return m_SmallIconIndex; }

  CieStrict  GetStrict() const 										     { return CieStrict(fStrict); }
  bool       IsStrict() const 										     { return !!fStrict; }
  void 			 ToggleStrict()    				  						   { fStrict = !fStrict; }
  void 			 SetStrict( CieStrict strict = ie_Strict ) { fStrict = uint8(strict); }
  void 			 Strict( bool strict = true ) 						 { fStrict = uint8(strict ? ie_Strict : ie_NonStrict); }

  void       SetManuallyChecked( bool set = true )		 { fManuallyChecked = set; }
  bool       IsManuallyChecked()											 { return !!fManuallyChecked; } 

	virtual int		GetImageIndex();

	friend class CNameSpaceList; 
};


/////////////////////////////////////////////////////////////////////////////
// CNameSpaceList window

class CNameSpaceList : public CCheckList {
protected :
  CImageList			m_NameLargeImageList;
  CImageList			m_NameSmallImageList;
	CHIconCache			m_LargeIconCache;
	CHIconCache			m_SmallIconCache;
	CSize           m_nLargeImageSize;
	CSize           m_nSmallImageSize;

// Construction
public:
	CNameSpaceList();

// Attributes
public:

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CNameSpaceList)
	protected:
	virtual void PreSubclassWindow();
	public :
	virtual COLORREF SetBkColor( COLORREF crColor );
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CNameSpaceList();
	
					// Установить признак Checked на элемент
	virtual void SetItemChecked( int iItemIndex, CCheckListItem::CieChecked fHowToCheck, bool bSendMessage = true );

					// Установить признак Strict на элемент
					void SetItemStrict( int iItemIndex, CNameSpaceListItem::CieStrict fHowToStrict, bool bSendMessage = true );

					// Установить Images для Item
	virtual	void	SetItemImages( CCheckListItem *clItem, int index );

					// Установить размеры ячеек ImageList
					void  SetImageListSize( SIZE nLargeSize, SIZE nSmallSize );
					// Установить иконы узла
					void	SetItemIcons( int iItemIndex );
					// Удалить все иконы
					void  FlushItemImages();

// Generated message map functions
protected:
					void	CreateStandartImageList();
	virtual void	CreateImageList();
	virtual	void	CheckItem( int ind );
					void  StrictItem( int iItemIndex, bool strict );

	virtual void	AddItem( int iItemIndex );
					void	InsideImageClickImitate( int nItemOnWhich );
					void  CreateAndTrackPopupMenu( CPoint& point );
					bool  CreatePopupMenu( CPoint& point, CPArray<CODMenu> &menus, int &nItem );
					void  CreateViewMenu( HMENU menu );

	//{{AFX_MSG(CNameSpaceList)
	afx_msg void	OnRButtonDown( UINT nFlags, CPoint point );
	afx_msg void	OnContextMenu(CWnd* pWnd, CPoint point);
	afx_msg void	OnItemChanging(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void	OnItemChanged(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void	OnCustomDraw( NMHDR* pNMHDR, LRESULT* pResult );
	afx_msg void	OnDestroy();
	afx_msg void	OnKillFocus(CWnd* pNewWnd);
	afx_msg void	OnSetFocus(CWnd* pOldWnd);
	afx_msg int		OnCreate(LPCREATESTRUCT lpCreateStruct);
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_NAMESPACELIST_H__D8037F24_ABAF_11D2_96B0_00104B5B66AA__INCLUDED_)
