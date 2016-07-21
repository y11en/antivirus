#if !defined(AFX_TREEWND_H__788BF3C5_94F2_11D2_96B0_00104B5B66AA__INCLUDED_)
#define AFX_TREEWND_H__788BF3C5_94F2_11D2_96B0_00104B5B66AA__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#include <afxcmn.h>
#include <Stuff\PArray.h>
#include <Stuff\PSynArr.h>
#include <Stuff\SArray.h>
#include <Stuff\StdDefs.h>

#if (_WIN32_IE < 0x0500)
#define COMCTL32_VERSION  5
#define CCM_SETVERSION          (CCM_FIRST + 0x7)
#define CCM_GETVERSION          (CCM_FIRST + 0x8)
#define CCM_SETNOTIFYWINDOW     (CCM_FIRST + 0x9) // wParam == hwndParent.
#endif // (_WIN32_IE < 0x0500)  

// TreeWnd.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CTreeWnd window

class CTreeWnd;

typedef void (*TvActionFunc)( CTreeWnd &owner, HTREEITEM cur, int ind, void *par );
typedef bool (*TvCondFunc)( CTreeWnd &owner, HTREEITEM cur, int ind, void *par );

// ---
struct CTreeItemInfo : public TV_INSERTSTRUCT {
	int level;
	CTreeItemInfo( LPCTSTR lpszItem, int nLevel ) :
		level( nLevel ) {

		item.mask = TVIF_TEXT;
		item.pszText = (LPTSTR)lpszItem;
	}
};

// ---
class CTreeWnd : public CTreeCtrl {
  HTREEITEM lastAddNode;
	int       notRedrawCount;
  int       lastAddLevel;
	uint8     m_bDeleteCollapsedSubTree : 1;
	uint8			m_bPossibleRedraw : 1;

// Construction
public:
	CTreeWnd();

// Attributes
public:

// Operations
public:

	void      SetDeleteCollapsed( bool set = true ) { m_bDeleteCollapsedSubTree = set; }
	bool      IsDeleteCollapsed()										{ return m_bDeleteCollapsedSubTree; }

	void      SetPossibleRedraw( bool set = true )  { m_bPossibleRedraw = set; }
	bool      IsPossibleRedraw()										{ return m_bPossibleRedraw; }

	HTREEITEM AddNode( CTreeItemInfo &newNode, HTREEITEM addAfter = NULL ); // Добавить узел (автоматически размещается на заданном уровне)
	HTREEITEM InsertNode( CTreeItemInfo &newNode, HTREEITEM addAfter );
  void      DeleteSubTree( HTREEITEM node, bool bRootIncluded = false );	// Удалить поддерево данного узла
	void 			ExpandNode( int ind );                   // Раскрыть узел по индексу
	void			CollapseNode( int ind );    						 // Закрыть узел по индексу    
	void 			SelectNode( int ind );									 // Селектировать узел по индексу
	int  			GetSelectedIndex();											 // Выдать индекс селектированного узла
	bool 			IsExpanded( int ind );                   // Раскрыт ли узел с заданным индексом
	bool 			IsExpanded( HTREEITEM node );            // Раскрыт ли узел
  bool 			IsNodeVisible( int ind );                // Видим ли узел с заданным индексом
	void 			SetNodeData( uint32 data, int ind );     // Установить данные узла
	uint32 		GetNodeData( int ind );                  // Выдать данные узла
  bool    	IsLeaf( int ind );                                  // Является ли узел конечным
  bool    	IsLeaf( HTREEITEM node );                           // Является ли узел конечным
  HTREEITEM ItemFromMousePos();                                 // Выдать узел под курсором мыши

  void      ForEach( TvActionFunc action, void *param = NULL, HTREEITEM fromNode = NULL ); // Итератор "по всем"
  HTREEITEM FirstThat( TvCondFunc action, void *param = NULL, HTREEITEM fromNode = NULL ); // Итератор "до первого кто"

	void      NewLoadSession() { lastAddNode = NULL; lastAddLevel = 0; }

	virtual int				IndexNode( HTREEITEM node );                        // Выдать индекс данного узла
	virtual HTREEITEM NodeByIndex( int ind );                             // Выдать узел по заданному индексу

	void      SetRedraw( BOOL bRedraw );
// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CTreeWnd)
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CTreeWnd();

	// Generated message map functions
protected:
					int 			GetNodeLevel( int ind );                            // Выдать уровень узла по индексу
					int 			GetNodeLevel( HTREEITEM node );                     // Выдать уровень узла
					bool			HasStyle( uint32 mask );
					HTREEITEM AddItemChild( HTREEITEM node, CTreeItemInfo &newNode ); // Добавить item в дерево
					HTREEITEM InsertItemChild( HTREEITEM hParentNode, HTREEITEM hAfterNode, CTreeItemInfo &newNode );

	//{{AFX_MSG(CTreeWnd)
	afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg void OnChar(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg void OnRButtonDown(UINT nFlags, CPoint point);
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
private:
	HTREEITEM RecourseEnumeration ( HTREEITEM node, int &index );
	bool			RecourseEnumeration ( HTREEITEM node, HTREEITEM find, int &index );
	bool			RecourseEnumeration ( HTREEITEM node, int findInd, int &index, int &level );
	HTREEITEM ForEachEnumeration  ( HTREEITEM node, TvActionFunc action, void *param, int &index );
	HTREEITEM FirstThatEnumeration( HTREEITEM node, TvCondFunc action, void *param, int &index, bool &result );
};

/////////////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////////////
// CCheckTree window

#define CTIS_BOLD      (0x00000001)
#define CTIS_ITALIC    (0x00000002)
#define CTIS_UNDERLINE (0x00000004)
#define CTIS_STRIKEOUT (0x00000008)

// ---
struct CCTFontCacheItem {
	HFONT  m_hFont;
	DWORD  m_dwStyle;
	CCTFontCacheItem( HFONT hFont, DWORD dwStyle ) :
		m_hFont( hFont ),
		m_dwStyle( dwStyle ) {}
};


// ---
class CCTFontCache : public CSArray<CCTFontCacheItem> {
public :
	CCTFontCache() : CSArray<CCTFontCacheItem> (0, 1) {}
	HFONT Find( DWORD dwStyle );
	void  Flush();
};

class CCheckTree;

// Дескриптор узла листа
// ---
class CCheckTreeItem {
protected :
  CString   m_Text;									// Текст узла
	HTREEITEM m_TreeItem;							// Узел дерева
	HFONT     m_Font;                 // Шрифт отображения узла
  int       m_Level;								// Уровень узла
  int       m_NumItems;							// Количество подэлементов в узле
  int       m_NumItemsChecked;			// Количество включенных подэлементов в узле
  int       m_NumItemsSomeChecked;  // Количество немного включенных подэлементов в узле
	int       m_ImageIndex;						// Индекс в Image-листе
	int       m_CurrImageIndex;				// Текущий индекс в Image-листе
	void		 *m_Data;									// Данные узла
  uint8     fChecked					: 2;	// Включен/выключен/немного включен
	uint8			fEnabled					: 1;	// Разрешен
  uint8     fNode							: 1;	// Является узлом
  uint8     fExpanded					: 1;	// Раскрыт
  uint8     fSelected					: 1;	// Селектирован
  uint8     fFirstVisible			: 1;	// Первый видимый
	uint8     fMarkable					: 1;	// Может включаться/выключаться
	uint8			fOwnsData					: 1;  // Владеет данными (сам удаляет)
	uint8     fStrictlyDisabled : 1;	// Строго запрещен

public:
    typedef enum {
      ie_Unchecked = 0,
      ie_Checked,
      ie_SomeChecked
    } CieChecked;

    typedef enum {
			ii_LeafUnchecked = 0,		// 0
			ii_LeafChecked,					// 1
			ii_NodeUnchecked,				// 2
			ii_NodeChecked,					// 3
			ii_NodeSomeChecked,			// 4
			ii_LastIndex,						// 5
		} CieImageIndex;

		CCheckTreeItem();
    virtual ~CCheckTreeItem();

		virtual bool operator == ( const CCheckTreeItem &rItem );
		virtual bool operator != ( const CCheckTreeItem &rItem ) { return !operator == ( rItem ); }

		virtual CCheckTreeItem *operator = ( const CCheckTreeItem &rItem ) { Assign( rItem ); return this; }
		virtual void Assign( const CCheckTreeItem &rItem );

		virtual CCheckTreeItem *Duplicate();

		void       SetFont( HFONT hFont )										 { m_Font = hFont; }
		HFONT      GetFont()																 { return m_Font; }

    CieChecked GetChecked() const 										   { return CieChecked(fChecked); }
    bool       IsChecked() const 										     { return !!fChecked; }
    void 			 Toggle()         				  						   { fChecked = !fChecked; }
    void 			 SetCheck( CieChecked check = ie_Checked ) { fChecked = uint8(check); }
    void 			 Check( bool check = true ) 							 { fChecked = uint8(check ? ie_Checked : ie_Unchecked); }

    void 			 SetLevel( int lvl )        							 { m_Level   = lvl; }
    int  			 GetLevel() const           							 { return m_Level; }

    void 			 SetText( const CString &setText );
    CString	  &GetText();

    void 			 SetTreeItem( HTREEITEM hItem )    				 { m_TreeItem   = hItem; }
    HTREEITEM  GetTreeItem() const           						 { return m_TreeItem; }

		void       SetEnabled( bool set = true )						 { fEnabled = set; }
    bool       IsEnabled()															 { return !!fEnabled; } 

    void       SetNode( bool set = true )								 { fNode = set; if ( IsMarkable() ) SetImageIndex( set ? ii_NodeUnchecked : ii_LeafUnchecked ); }
    bool       IsNode()																	 { return !!fNode; } 

    void       SetExpanded( bool set = true )						 { fExpanded = set; }
    bool       IsExpanded()															 { return !!fExpanded; } 

    void       SetSelected( bool set = true )						 { fSelected = set; }
    bool       IsSelected()															 { return !!fSelected; } 

    void       SetFirstVisible( bool set = true )				 { fFirstVisible = set; }
    bool       IsFirstVisible()													 { return !!fFirstVisible; } 

    void       SetMarkable( bool set = true )						 { fMarkable = set; }
    bool       IsMarkable()															 { return !!fMarkable; } 

    int        GetNumItems()														 { return m_NumItems; }
    void       SetNumItems( int _numItems )							 { m_NumItems = _numItems; }

    int        GetNumItemsChecked()											 { return m_NumItemsChecked; }
    void       SetNumItemsChecked( int _numItems )			 { m_NumItemsChecked = _numItems; }

    int        GetNumItemsSomeChecked()									 { return m_NumItemsSomeChecked; }
    void       SetNumItemsSomeChecked( int _numItems )	 { m_NumItemsSomeChecked = _numItems; }

		virtual int		GetImageIndex();
		void			 SetImageIndex( int iImageIndex )					 { m_ImageIndex = iImageIndex; }

		void			 SetCurrImageIndex( int iCurrImageIndex )		{ m_CurrImageIndex = iCurrImageIndex; }
		int 			 GetCurrImageIndex()                       	{ return m_CurrImageIndex; }

		void       SetData( void *pData, bool bOwnsData )		 { m_Data = pData; SetOwnsData( bOwnsData ); }
		void      *GetData()																 { return m_Data; }	

		void       SetOwnsData( bool set = true )						 { fOwnsData = set; }
		bool       IsOwnsData()															 { return !!fOwnsData; } 

		void       SetStrictlyDisabled( bool set = true )		 { fStrictlyDisabled = set; }
		bool       IsStrictlyDisabled()											 { return !!fStrictlyDisabled; } 

		virtual bool CanChangeChecked()												{ return true; } 

    friend class CCheckTree; 
};


// ---
class CCTItemsArray : public CPSyncArray<CCheckTreeItem> {
public :
	CCTItemsArray( bool owns = true );
	~CCTItemsArray();

	void SetOwns( bool owns = true ) { OwnsElem(owns); }
	bool GetOwns() const { return OwnsElem(); }

	void RemoveAt( int nIndex );
	int  FindIt( CCheckTreeItem *item );

	void InsertAt( int nIndex, CCTItemsArray *pNewArray );
	void InsertAt( int nIndex, CCheckTreeItem *pNewItem );

	int  GetSize() const { return Count(); }
	int  GetUpperBound() const { return MaxIndex(); }

	bool operator == ( const CCTItemsArray &rItems );
	bool operator != ( const CCTItemsArray &rItems ) { return !operator == ( rItems ); }
	void operator = ( const CCTItemsArray &rItems );
};


// ---
class CCheckTree : public CTreeWnd {
protected :
	CWnd					 *m_MsgReceiver;
	HACCEL					m_hAccelTable;
	bool            m_bAccelTableShouldBeDestroyed;
	bool            m_bInterfaceLocked;
	int             m_nInterfaceLockCount;
	CCTItemsArray  *m_Items;
  CImageList			m_ImageList;
	int							m_nFirstVisibleInd;
	CCTFontCache		m_FontCache;
	HFONT           m_hOldFont;

	uint8           fSpreadMark							: 1; // Распространять маркировку вниз и вверх
	uint8						fStateImageType					: 1; // Image типа State
	uint8           fOwnsItemsArray					: 1; // Владеет (сам уничтожает) массив элементов
	uint8           fSetupImagesAtLoad			: 1; // Устанавливать иконы при загрузке
	uint8						fEnableOpOnDisabled			: 1; // Разрешены операции на запрещенном узле
	uint8						fEnableChangeItemsState	: 1; // Разрешено изменение состояния элементов
	uint8						fUseCheckSemantic				: 1; // Использовать семантику CheckTree
	uint8						fEnablePostLoadProcessing : 1; // Разрешить обработку после загрузки

// Construction
public:
	CCheckTree();

// Attributes
public:

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CCheckTree)
	public:
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	virtual COLORREF SetBkColor( COLORREF crColor );
	protected:
	virtual void PreSubclassWindow();
	virtual BOOL OnCommand(WPARAM wParam, LPARAM lParam);
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CCheckTree();

	void SetSpreadMark( bool set = true )        { fSpreadMark = set; }
  bool IsSpreadMark()                          { return !!fSpreadMark; } 

	void SetOwnsItemsArray( bool set = true )    { fOwnsItemsArray = set; }
  bool IsOwnsItemsArray()                      { return !!fOwnsItemsArray; } 

	void SetStateImageType( bool set = true )    { fStateImageType = set; }
  bool IsStateImageType()                      { return !!fStateImageType; } 

	void SetSetupImagesAtLoad( bool set = true )   { fSetupImagesAtLoad = set; }
  bool IsSetupImagesAtLoad()                     { return !!fSetupImagesAtLoad; } 

	void SetEnableOpOnDisabled( bool set = true ) { fEnableOpOnDisabled = set; }
	bool IsEnableOpOnDisabled()								    { return !!fEnableOpOnDisabled; } 

	void SetEnableChangeItemsState( bool set = true ) { fEnableChangeItemsState = set; }
	bool IsEnableChangeItemsState()								    { return !!fEnableChangeItemsState; } 

	void SetUseCheckSemantic( bool set = true ) { fUseCheckSemantic = set; }
	bool IsUseCheckSemantic()								    { return !!fUseCheckSemantic; } 

	void SetEnablePostLoadProcessing( bool set = true );
	bool IsEnablePostLoadProcessing();

	void	SetItemsArray( CCTItemsArray *itemsArray );									// Установить новый массив дескрипторов
	// Установить новый массив дескрипторов, начиная с заданного индекса
	void	UpdateItemsArray( int fromIndex, CCTItemsArray &itemsArray, bool bRootInserted = false );
	void	UpdateItemsArrayByRange( int fromIndex, int toIndex, CCTItemsArray &itemsArray );
	
	// Выполнить стартовое инициирование
	void  StartupInitiation();

	// Получить массив элементов
	CCTItemsArray *GetItemsArray() { return m_Items; }

	// Получиь индекс родительского элемента
	int  GetParentIndex( int nItemIndex );

	// Получиь индекс видимого элемента
	int  GetFirstVisibleIndex();

	// Получиь индекс первого дочернего элемента
	int  GetFirstChildIndex( int nItemIndex );

	// Получиь индекс следующего братского элемента
	int  GetNextSiblingIndex( int nItemIndex );

	virtual int				IndexNode( HTREEITEM node );                     // Выдать индекс данного узла
	        int				IndexNode( CCheckTreeItem *item );                        // Выдать индекс данного узла
	virtual HTREEITEM NodeByIndex( int ind );                          // Выдать узел по заданному индексу

	// Установить признак Checked на элемент
  void SetItemChecked( int iItemIndex, CCheckTreeItem::CieChecked fHowToChecked, bool bSpreadAutomatically = false ); 
	virtual void InternalSetItemChecked( CCheckTreeItem *item, CCheckTreeItem::CieChecked fHowToChecked );
	
	// Добавить новый элемент по дескриптору, начиная с заданного индекса папы
	int AddItemToParent( CCheckTreeItem *pNewItem, int parentIndex = -1 );

	// Вставить новый элемент по дескриптору, начиная с заданного индекса папы перед заданным индексом
	int InsertItemToParent( CCheckTreeItem *pNewItem, int parentIndex, int indexBefore );

	// Удалить элемент со всем поддеревом, начиная с заданного индекса 
	void DeleteItemWithSubtree( int nFromIndex );

	// Установить текст на элемент
	void SetTreeItemText( int nItemIndex, const CString &rcText );

	// Сканирование/Поиск
  void      ForEach( TvActionFunc action, void *param = NULL, int indFromNode = -1 ); // Итератор "по всем"
  int       FirstThat( TvCondFunc action, void *param = NULL, int indFromNode = -1 ); // Итератор "до первого кто"

	CWnd *GetMessageReceiver();
	void  SetMessageReceiver( CWnd *pReceiver ) { m_MsgReceiver = pReceiver; }

	void   SetAccelTable( HACCEL	hAccelTable, bool bShouldBeDestroyed = false );

// Только для внутреннего использования
	// Включить всех родитетей данного узла
	virtual void CheckParents( HTREEITEM node );	
	// Включить всех детей данного узла
	virtual void CheckChildren( HTREEITEM node, int ind, CCheckTreeItem::CieChecked fHowToChecked, bool bSetCheck = true );
	// Установить Images узла
	virtual void SetNodeImage( CCheckTreeItem *item, HTREEITEM hNode );

	       	void SetupImages( HTREEITEM fromNode = NULL );

					void SetNodeFont( CCheckTreeItem *item, HFONT hFont ) { item->SetFont(hFont); }
					void SetNodeStyle( CCheckTreeItem *item, DWORD dwStyle );

					void LockInterface( bool bLock );

private :
	// Загрузить дерево после заданного узла
	void LoadTreeWindow(  HTREEITEM loadAfter = NULL, bool bRootInserted = false ); 
	void LoadTreeWindow( int fromIndex, int toIndex, HTREEITEM hParent, HTREEITEM addAfter );

protected :
	virtual void			CreateImageList();        // Создать ImageList
	virtual HTREEITEM AddItem( CCheckTreeItem *item, HTREEITEM addAfter ); // Добавить узел (автоматически размещается на заданном уровне)
	virtual HTREEITEM InsertItem( CCheckTreeItem *item, HTREEITEM addAfter );
	virtual void			PostLoadProcessing();		 // Дополнительная обработка после загрузки дерева
	virtual void			CheckNode( HTREEITEM node );										 // Включить узел
	virtual int				ItemCheckingAnalize( CCheckTreeItem *item );
	
	       	void SetupNodes( HTREEITEM fromNode = NULL );
					void SetupInternalInfo();


	// Generated message map functions
protected:
	//{{AFX_MSG(CCheckTree)
	afx_msg void		OnNcLButtonDown( UINT nFlags, CPoint point );
	afx_msg void		OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg void		OnChar(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg void		OnLButtonDblClk(UINT nFlags, CPoint point);
	afx_msg void		OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void		OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void		OnItemExpanded(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void		OnKeyUp(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg int			OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void		OnDestroy();
	afx_msg LRESULT OnAfterLoadExpanding( WPARAM wParam, LPARAM lParam );
	afx_msg void		OnSelChanging(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void		OnSelChanged(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg int			OnMouseActivate( CWnd* pDesktopWnd, UINT nHitTest, UINT message );
	afx_msg LRESULT OnSetRedraw( WPARAM wParam, LPARAM lParam );
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////
//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_TREEWND_H__788BF3C5_94F2_11D2_96B0_00104B5B66AA__INCLUDED_)
