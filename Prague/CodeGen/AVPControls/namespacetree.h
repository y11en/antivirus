#if !defined(AFX_NAMESPACELIST_H__7115D621_A950_11D2_96B0_00104B5B66AA__INCLUDED_)
#define AFX_NAMESPACELIST_H__7115D621_A950_11D2_96B0_00104B5B66AA__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "TreeWnd.h"
#include "IconCache.h"

// NameSpaceList.h : header file
//


class CNameSpaceTree;

// Дескриптор узла листа
// ---
class CNameSpaceTreeItem : public CCheckTreeItem {
private :
	HICON		m_NormalIcon;
	HICON		m_SelectedIcon;
	int			m_NormalIconIndex;
	int			m_SelectedIconIndex;
	void   *m_AdditionData;

	uint8   fStrict											 : 1; // Strict/Non Strict
	uint8		fEnabled										 : 1; //	Разрешен
	uint8   fOwnsAdditionData						 : 1; // Владеет данными (сам удаляет)
	uint8   fDummy											 : 1; // Пустышка для указания узла имеющего детей, но еще не загруженного
	uint8   fHasCheckedDescendants			 : 1; // Имеет включенных потомков
	uint8   fHasStrictDescendants			   : 1; // Имеет Strict потомков
	uint8   fHasStrictCheckedDescendants : 1; // Имеет строго включенных потомков
	uint8   fManuallyChecked						 : 2;	// Включен вручную
	uint8   fManuallyStrict 						 : 2;	// Strict вручную
	uint8   fHasNoCheckButton						 : 1; // Не имеет check button
	uint8   fOwnsIcons									 : 1;  // Владеет данными (сам удаляет)

public :
  typedef enum {
    ie_NonStrict = 0,
    ie_Strict, 
  } CieStrict;

  typedef enum {
		iti_Unchecked = 0,		// 0
		iti_Checked,					// 1
		iti_LastIndex,				// 2
	} CieTImageIndex;

	 CNameSpaceTreeItem();

	virtual ~CNameSpaceTreeItem();

	virtual bool operator == ( const CCheckTreeItem &rItem );

	virtual void Assign( const CCheckTreeItem &rItem );

	virtual CCheckTreeItem *Duplicate();

	void	SetIcons( HICON hNormal, HICON hSelected )   { m_NormalIcon = hNormal; m_SelectedIcon = hSelected; }
	void	SetIconIndexes( int iNormal, int iSelected ) { m_NormalIconIndex = iNormal; m_SelectedIconIndex = iSelected; }

  void       SetOwnsIcons( bool set = true )						 { fOwnsIcons = set; }
  bool       IsOwnsIcons()															 { return !!fOwnsIcons; } 

	HICON GetNormalIcon() const { return m_NormalIcon; }
	HICON GetSelectedIcon() const { return m_SelectedIcon; }
	
	int   GetNormalIconIndex() const { return m_NormalIconIndex; }
	int   GetSelectedIconIndex() const { return m_SelectedIconIndex; }	

  CieStrict  GetStrict() const 										     { return CieStrict(fStrict); }
  bool       IsStrict() const 										     { return !!fStrict; }
  void 			 ToggleStrict()    				  						   { fStrict = !fStrict; }
  void 			 SetStrict( CieStrict strict = ie_Strict ) { fStrict = uint8(strict); }
  void 			 Strict( bool strict = true ) 						 { fStrict = uint8(strict ? ie_Strict : ie_NonStrict); }

  void       SetEnabled( bool set = true )						 { fEnabled = set; }
  bool       IsEnabled()															 { return !!fEnabled; } 

  void       SetOwnsAdditionData( bool set = true )		 { fOwnsAdditionData = set; }
  bool       IsOwnsAdditionData()											 { return !!fOwnsAdditionData; } 

  void       SetDummy()																 { fDummy = true; SetMarkable( true );}
  bool       IsDummy()																 { return !!fDummy; } 

  void       SetHasCheckedDescendants( bool set = true )				{ fHasCheckedDescendants = set; }
  bool       IsHasCheckedDescendants()													{ return !!fHasCheckedDescendants; } 

  void       SetHasStrictDescendants( bool set = true )				{ fHasStrictDescendants = set; }
  bool       IsHasStrictDescendants()													{ return !!fHasStrictDescendants; } 

  void       SetHasStrictCheckedDescendants( bool set = true )	{ fHasStrictCheckedDescendants = set; }
  bool       IsHasStrictCheckedDescendants()										{ return !!fHasStrictCheckedDescendants; } 

  void       SetManuallyChecked( bool set = true )		 { fManuallyChecked = set ? 2 : fManuallyChecked ? --fManuallyChecked : 0; }
  bool       IsManuallyChecked()											 { return !!fManuallyChecked; } 

  void       SetManuallyStrict( bool set = true )			 { fManuallyStrict = set ? 2 : fManuallyStrict ? --fManuallyStrict : 0; }
  bool       IsManuallyStrict()												 { return !!fManuallyStrict; } 

  void       SetHasNoCheckButton( bool set = true )							{ fHasNoCheckButton = set; }
  bool       IsHasNoCheckButton()																{ return !!fHasNoCheckButton; } 

	void       SetAdditionData( void *pData, bool bOwnsData )		 { m_AdditionData = pData; SetOwnsAdditionData( bOwnsData ); }
	void      *GetAdditionData()																 { return m_AdditionData; }	

	virtual int		GetImageIndex();
	virtual bool  CanChangeChecked();

	friend class CNameSpaceTree; 
};


/////////////////////////////////////////////////////////////////////////////
// CNameSpaceTree window

class CNameSpaceTree : public CCheckTree {
	uint8 fShowSelectionAlways : 1;

protected :
  CImageList			m_NameImageList;
  //CImageList			m_DisabledImageList;
	CHIconCache			m_IconCache;
	CSize           m_nImageSize;

// Construction
public:
	CNameSpaceTree();

// Attributes
public:

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CNameSpaceTree)
	protected:
	virtual void		 PreSubclassWindow();
	public :
	virtual COLORREF SetBkColor( COLORREF crColor );
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CNameSpaceTree();

	void  SetShowSelectionAlways( bool bShowAlways );

	// Установить признак Strict на элемент
  void SetItemStrict( int iItemIndex, CNameSpaceTreeItem::CieStrict fHowToStrict, bool bSpreadAutomatically = false ); 

// Только для внутреннего использования
	// Включить всех родитетей данного узла
	virtual void CheckParents( HTREEITEM node );	
	// Включить всех детей данного узла
	virtual void CheckChildren( HTREEITEM node, int ind, CCheckTreeItem::CieChecked fHowToChecked, bool bSetCheck = true );
	// Установить Images узла
	virtual void SetNodeImage( CCheckTreeItem *item, HTREEITEM hNode );
	virtual void InternalSetItemChecked( CCheckTreeItem *item, CCheckTreeItem::CieChecked fHowToChecked );

					// Установить размеры ячеек ImageList
					void  SetImageListSize( SIZE nSize );
					// Установить иконы узла
					void	SetItemIcons( CNameSpaceTreeItem *nsItem, HTREEITEM hItem );
					// Удалить все иконы
					void  FlushItemImages();
					

	// Generated message map functions
protected:
					void			CreateStandartImageList();
	virtual void			CreateImageList();																	 // Создать ImageList
	virtual HTREEITEM AddItem( CCheckTreeItem *item, HTREEITEM addAfter ); // Добавить узел (автоматически размещается на заданном уровне)
	virtual void			CheckNode( HTREEITEM node );												 // Включить узел
					void			StrictNode( int iNodeIndex );                        // Установить Strict

	// Установить состояние детей узла
					void			CheckChildrenStrict( HTREEITEM hNode, int ind, CNameSpaceTreeItem::CieStrict fHowToStrict, bool bStrict = true);
	// Установить состояние родилей узла
					void			CheckParentsStrict( HTREEITEM hNode );

					void      InsideImageClickImitate( HTREEITEM hItemOnWhich );
					void			CreateAndTrackPopupMenu( CPoint& point );
	
					void			GetFullItemPath( CNameSpaceTreeItem *nsItem, HTREEITEM hNode, CString &stFullPath );

					int				HowShouldItemBeChecked( CNameSpaceTreeItem *nsItem );
					int				HowShouldItemBeStricted( CNameSpaceTreeItem *nsItem );

	virtual int				ItemCheckingAnalize( CCheckTreeItem *item );
					int				ItemStrictAnalize( CNameSpaceTreeItem *prItem );
					
					void			ScanChildrenAndSetChecked( HTREEITEM hParentNode, 
																							 CNameSpaceTreeItem *prIitem, 
																							 int parentIndex, 
																							 bool bCheckChildren = false,
																							 CNameSpaceTreeItem::CieChecked fHowToChecked = CNameSpaceTreeItem::ie_Unchecked );
					void			ScanChildrenAndSetStrict( HTREEITEM hParentNode, 
																							CNameSpaceTreeItem *prIitem, 
																							int parentIndex, 
																							 bool bCheckChildren = false,
																							 CNameSpaceTreeItem::CieStrict fHowToStrict = CNameSpaceTreeItem::ie_NonStrict );
					void			ScanChildrenForManuallyChecked( CNameSpaceTreeItem *prItem, 
																										int parentIndex );
					void			ScanChildrenForManuallyStrict( CNameSpaceTreeItem *prItem, 
 																									 int parentIndex );

	//{{AFX_MSG(CNameSpaceTree)
	afx_msg void		OnCustomDraw( NMHDR* pNMHDR, LRESULT* pResult );
	afx_msg void		OnItemExpanding(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void		OnItemExpanded(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void		OnRButtonDown(UINT nFlags, CPoint point);
	afx_msg void		OnContextMenu(CWnd* pWnd, CPoint point);
	afx_msg void		OnSelChanging(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void		OnSelChanged(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void		OnDestroy();
	afx_msg void		OnKillFocus(CWnd* pNewWnd);
	afx_msg void		OnSetFocus(CWnd* pOldWnd);
	afx_msg void		OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg int			OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg LRESULT OnAfterChangeState( WPARAM wParam, LPARAM lParam );
	afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_NAMESPACELIST_H__7115D621_A950_11D2_96B0_00104B5B66AA__INCLUDED_)
