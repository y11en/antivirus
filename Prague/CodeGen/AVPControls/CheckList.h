#if !defined(AFX_CHECKLIST_H__D8037F25_ABAF_11D2_96B0_00104B5B66AA__INCLUDED_)
#define AFX_CHECKLIST_H__D8037F25_ABAF_11D2_96B0_00104B5B66AA__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// CheckList.h : header file
//

#include <afxcmn.h>
#include <Stuff\Thread.h>
#include <Stuff\PSynArr.h>
#include <Stuff\SArray.h>
#include <Stuff\PArray.h>
#include <Stuff\StdDefs.h>
#include <StuffMFC\TrackToolTip.h>

#if (_WIN32_IE < 0x0500)
#define COMCTL32_VERSION  5
#define CCM_SETVERSION          (CCM_FIRST + 0x7)
#define CCM_GETVERSION          (CCM_FIRST + 0x8)
#define CCM_SETNOTIFYWINDOW     (CCM_FIRST + 0x9) // wParam == hwndParent.
#endif // (_WIN32_IE < 0x0500)  

#define CLIS_BOLD      (0x00000001)
#define CLIS_ITALIC    (0x00000002)
#define CLIS_UNDERLINE (0x00000004)
#define CLIS_STRIKEOUT (0x00000008)


// ---
struct CCLFontCacheItem {
	HFONT  m_hFont;
	DWORD  m_dwStyle;
	CCLFontCacheItem( HFONT hFont, DWORD dwStyle ) :
		m_hFont( hFont ),
		m_dwStyle( dwStyle ) {}
};


// ---
class CCLFontCache : public CSArray<CCLFontCacheItem> {
public :
	CCLFontCache() : CSArray<CCLFontCacheItem> (0, 1) {}
	HFONT Find( DWORD dwStyle );
	void  Flush();
};

class CCheckList;

// Дескриптор узла листа
// ---
class CCheckListItem {
protected :
  CString			m_Text;             // Текст узла
	CStringList m_SubItemsTexts;    // Тексты Subitems
	int					m_ImageIndex;				// Индекс в Image-листе
	int					m_CurrImageIndex;		// Текущий индекс в Image-листе
	HFONT       m_Font;             // Шрифт отображения узла
	void			 *m_Data;

  uint8				fChecked    : 2;    // Включен/выключен/немного включен
	uint8				fEnabled		: 1;	  // Разрешен
  uint8				fNode       : 1;    // Является узлом
	uint8				fOwnsData   : 1;    // Владеет данными (сам удаляет)

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

		CCheckListItem();
    virtual ~CCheckListItem();

		virtual bool operator == ( const CCheckListItem &rItem );
		virtual bool operator != ( const CCheckListItem &rItem ) { return !operator == ( rItem ); }

		virtual CCheckListItem *operator = ( const CCheckListItem &rItem ) { Assign( rItem ); return this; }
		virtual void Assign( const CCheckListItem &rItem );

		virtual CCheckListItem *Duplicate();

		void       SetFont( HFONT hFont )										 { m_Font = hFont; }
		HFONT      GetFont()																 { return m_Font; }

    CieChecked GetChecked() const 										   { return CieChecked(fChecked); }
    bool       IsChecked() const 										     { return !!fChecked; }
    void 			 Toggle()         				  						   { fChecked = !fChecked; }
    void 			 SetCheck( CieChecked check = ie_Checked ) { fChecked = uint8(check); }
    void 			 Check( bool check = true ) 							 { fChecked = uint8(check ? ie_Checked : ie_Unchecked); }

		void       SetEnabled( bool set = true )						 { fEnabled = set; }
    bool       IsEnabled()															 { return !!fEnabled; } 

    void       SetNode( bool set = true )								 { fNode = set; SetImageIndex( set ? ii_NodeUnchecked : ii_LeafUnchecked ); }
    bool       IsNode()																	 { return !!fNode; } 

    void 			 SetText( const CString &setText );
    CString	  &GetText();

		void       SetData( void *pData, bool bOwnsData )		 { m_Data = pData; SetOwnsData( bOwnsData ); }
		void      *GetData()																 { return m_Data; }	

		void       SetOwnsData( bool set = true )						 { fOwnsData = set; }
		bool       IsOwnsData()															 { return !!fOwnsData; } 

    void 			   SetSubItemsTexts( const CStringList &setText );
    CStringList	&GetSubItemsTexts();

    const TCHAR  *GetSubItemText( int nIndex );
		int          GetSubItemsCount();

		virtual int		GetImageIndex();
		void			 SetImageIndex( int iImageIndex )					 { m_ImageIndex = iImageIndex; }

		void			SetCurrImageIndex( int iCurrImageIndex )		{ m_CurrImageIndex = iCurrImageIndex; }
		int 			GetCurrImageIndex()                       	{ return m_CurrImageIndex; }

    friend class CCheckList; 
};


// ---
class CCLItemsArray : public CPSyncArray<CCheckListItem> {
public :
	CCLItemsArray( bool owns = true );
	~CCLItemsArray();

	void SetOwns( bool owns = true ) { OwnsElem(owns); }
	bool GetOwns() const { return OwnsElem(); }

	void RemoveAt( int nIndex );
	int  FindIt( CCheckListItem *item );

	void InsertAt( int nIndex, CCLItemsArray *pNewArray );
	void InsertAt( int nIndex, CCheckListItem *pNewItem );

	int  GetSize() const { return Count(); }
	int  GetUpperBound() const { return MaxIndex(); }

	bool operator == ( const CCLItemsArray &rItems );
	bool operator != ( const CCLItemsArray &rItems ) { return !operator == ( rItems ); }
};


// Дескриптор колонки
// ---
class CCheckListColumn {
public :
	typedef enum {
		ca_Left		= LVCFMT_LEFT,
		ca_Right	= LVCFMT_RIGHT,
		ca_Center = LVCFMT_CENTER
	} CieAlingment;

protected :
	CString				m_Name;
	CieAlingment	m_Alignment;
	int           m_WidthPercent;          
	uint8         fSortAscending		: 1; 
	uint8         fExternalPercent	: 1;

public :
	CCheckListColumn();

	bool operator == ( const CCheckListColumn &rItem );
	bool operator != ( const CCheckListColumn &rItem ) { return !operator == ( rItem ); }

	void SetName( const CString &rName );
	CString &GetName();

	void SetAlignment( CieAlingment ieAlign ) { m_Alignment = ieAlign; }
	CieAlingment GetAlignment()								{ return m_Alignment; }

	void SetSortAscending( bool set = true )  { fSortAscending = set; }
	bool IsSortAscending()										{ return fSortAscending; }

	void SetExternalPercent( bool set = true )  { fExternalPercent = set; }
	bool IsExternalPercent()										{ return fExternalPercent; }

	void SetWidthPercent( int set )						{ m_WidthPercent = set; }
	int  GetWidthPercent()									  { return m_WidthPercent; }
};


// ---
class CCLColumnsArray : public CPSyncArray<CCheckListColumn> {
public :
	CCLColumnsArray( bool owns = true ) : CPSyncArray<CCheckListColumn>( 0, 1, owns ) {}
	~CCLColumnsArray();

	void SetOwns( bool owns = true ) { OwnsElem(owns); }
	bool GetOwns() const { return OwnsElem(); }

	void RemoveAt( int nIndex );
	int  FindIt( CCheckListColumn *item );

	void InsertAt( int nIndex, CCLColumnsArray *pNewArray );
	void InsertAt( int nIndex, CCheckListColumn *pNewItem );

	int  GetSize() const { return Count(); }
	int  GetUpperBound() const { return MaxIndex(); }

	bool operator == ( const CCLColumnsArray &rItems );
	bool operator != ( const CCLColumnsArray &rItems ) { return !operator == ( rItems ); }
};

#define E_SIZEOSCILLATE				0
#define E_DINAMICRESIZEHEADER 1
#define E_COLUMNWIDTHSETUP		2
#define E_TRACKINPROGRESS			3
#define E_COUNT								4


/////////////////////////////////////////////////////////////////////////////
// CCheckList window

class CCheckList : public CListCtrl {
protected :
	HACCEL					 m_hAccelTable;
	bool             m_bAccelTableShouldBeDestroyed;
	CWnd					  *m_MsgReceiver;
	CCLItemsArray		*m_Items;
  CImageList			 m_ImageList;
	CCLColumnsArray	*m_Columns;
	CTitleTip				 m_TitleTip;
	CCLFontCache		 m_FontCache;
	HFONT            m_hOldFont;
	bool             m_bInterfaceLocked;
	int              m_nInterfaceLockCount;
	int              m_nLastSelectedInd;

	CPArray<CEventSemaphore> m_pEvents;
	HANDLE                  m_hEvents[E_COUNT];

	uint8						 fStateImageType						:	1; // Image типа State
	uint8            fOwnsItemsArray						: 1; // Владеет (сам уничтожает) массив элементов
	uint8            fOwnsColumnsArray					: 1; // Владеет (сам уничтожает) массив заголовков
	uint8            fHeaderWasChanged 					: 1; // Состояние Header изменено
	uint8            fSetupImagesAtLoad					: 1; // Устанавливать иконы при загрузке
	uint8            fUseCheckSemantic					:	1; // Использовать семантику Check-листа
	uint8						 fEnableChangeItemsState		: 1; // Разрешено изменение состояния элементов
	uint8						 fShowSelectionAlways				: 1; // Удерживать селектирование
	uint8						 fShowHeaderOnEmpty  				: 1; // Удерживать Header на пустом листе
	uint8						 fProportionalResizeHeader	: 1; // Пропорциональное изменение ширины колонок Header
	uint8						 fChangeColumnTextOnly			: 1; // Только замена текста колонок Header
	uint8						 fUseTitleTip								: 1; // Использовать TitleTip
	uint8						 fDisplayNoItemsString			: 1; // Использовать строку "There are no items to show in this view"
	uint8						 fForceAntiFlickering			  : 1; // Использовать технологию "анти-бликирования"
	

	uint8            m_bSizeOscillate						:	1;
	uint8            m_bDynamicResizeHeader			: 1;
	uint8            m_bColumnWidthSetup				: 1;
	uint8            m_bTrackInProgress    			: 1;
	uint8						 m_bDefferedSetColumns			: 1;
	uint8            m_bFirstChanceSetSize			: 1;
	uint8            m_bDestroyed							  : 1;
	
// Construction
public:
	CCheckList();

// Attributes
public:

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CCheckList)
	public:
	virtual COLORREF SetBkColor( COLORREF crColor );
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	protected:
	virtual void PreSubclassWindow();
	virtual BOOL OnNotify(WPARAM wParam, LPARAM lParam, LRESULT* pResult);
	virtual BOOL OnCommand(WPARAM wParam, LPARAM lParam);
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CCheckList();

	void SetStateImageType( bool set = true )    { fStateImageType = set; }
  bool IsStateImageType()                      { return !!fStateImageType; } 

	void SetOwnsItemsArray( bool set = true )    { fOwnsItemsArray = set; }
  bool IsOwnsItemsArray()                      { return !!fOwnsItemsArray; } 

	void SetOwnsColumnsArray( bool set = true )  { fOwnsColumnsArray = set; }
  bool IsOwnsColumnsArray()                    { return !!fOwnsColumnsArray; } 

	void SetSetupImagesAtLoad( bool set = true )   { fSetupImagesAtLoad = set; }
  bool IsSetupImagesAtLoad()                     { return !!fSetupImagesAtLoad; } 

	void SetUseCheckSemantic( bool set = true )   { fUseCheckSemantic = set; }
  bool IsUseCheckSemantic()                     { return !!fUseCheckSemantic; } 

	void SetEnableChangeItemsState( bool set = true ) { fEnableChangeItemsState = set; }
	bool IsEnableChangeItemsState()								    { return !!fEnableChangeItemsState; } 

	void SetShowSelectionAlways( bool set = true );
	bool GetShowSelectionAlways()								 { return fShowSelectionAlways; }

	void SetShowHeaderOnEmpty( bool set = true )	{ fShowHeaderOnEmpty = set; }
	bool IsShowHeaderOnEmpty()								    { return !!fShowHeaderOnEmpty; } 

	void SetProportionalResizeHeader( bool set = true );
	bool IsProportionalResizeHeader()								    { return !!fProportionalResizeHeader; } 

	void SetChangeColumnTextOnly( bool set = true ) { fChangeColumnTextOnly = set; }
	bool IsChangeColumnTextOnly()								    { return !!fChangeColumnTextOnly; } 

	void SetUseTitleTip( bool set = true );
	bool IsUseTitleTip()								    { return !!fUseTitleTip; } 
	
	void SetDisplayNoItemsString( bool set = true ) {	fDisplayNoItemsString = set; }
	bool IsDisplayNoItemsString()								    { return !!fDisplayNoItemsString; } 
	
	void SetForceAntiFlickering( bool set = true ) {	fForceAntiFlickering = set; }
	bool IsForceAntiFlickering()								    { return !!fForceAntiFlickering; } 
	

	void SetItemsArray( CCLItemsArray *itemsArray, CCLColumnsArray *headerNames = NULL );// Установить новый массив дескрипторов
	void SetColumnsArray( CCLColumnsArray *headerNames );// Установить новый массив заголовков
	void UpdateItemsArray( int fromIndex, CCLItemsArray &itemsArray );// Установить новый массив дескрипторов, начиная с заданного индекса
	
	CCLItemsArray *GetItemsArray() { return m_Items; }
	CCLColumnsArray *GetColumnsArray() { return m_Columns; }

	virtual void SetItemChecked( int iItemIndex, CCheckListItem::CieChecked fHowToCheck, bool bSendMessage = true );

	void SetItemSelected( int ind );
  
	// Получить индекс селектированного элемента
	int	 GetSelectedIndex();

	// Выполнить стартовое инициирование
	void  StartupInitiation();

	// Установить тип отображения
	bool	 SetViewType( DWORD dwViewType );
	uint32 GetViewType();

	CWnd  *GetMessageReceiver();
	void   SetMessageReceiver( CWnd *pReceiver ) { m_MsgReceiver = pReceiver; }

	virtual void   AddNewItem( CCheckListItem *pItem, bool bEnsureVisible, bool bAddToList = true );
	virtual void   RemoveItems( int iItemFromIndex, int nRemoveCount = 1 );
	virtual void   AddItemsToList( bool bEnsureVisible );

	// Установить размеры колонок
					void	 SetColumnsWidth();
					void	 CorrectColumnsWidth();

					void   SetItemText( int nIndex, CString &rcItemText );
					void   SetItemSubTexts( int nIndex, CStringList &rcStrList );

					void	 Synchronize();

					void   SetAccelTable( HACCEL	hAccelTable, bool bShouldBeDestroyed = false );

					void	 SetNodeFont( CCheckListItem *item, HFONT hFont ) { item->SetFont(hFont); }
					void	 SetNodeStyle( CCheckListItem *item, DWORD dwStyle );

					void	 LockInterface( bool bLock );
protected:

					void  SetHeaderWasChanged( bool set = true )  { fHeaderWasChanged = set; }
					bool  IsHeaderWasChanged()                    { return !!fHeaderWasChanged; } 

	// Создать ImageList
	virtual void	CreateImageList();

	// Check/Uncheck Item
	virtual	void	CheckItem( int ind );

	// Добавить Item в List
	virtual void	AddItem( int iItemIndex );

	// Установить Images для Item
	virtual	void	SetItemImages( CCheckListItem *clItem, int index );

	// Загрузить List
					void	LoadListWindow(  bool bNeedReloadHeader );

	// Установить Normal Image для Item
					void	SetItemImage( int iItemIndex, int iItemImageIndex );
/*
	// Получить размер текста
					int		GetTextSize( LPCTSTR ptText );
*/
 // Убрать/восстановить Header
					void  ShowColumnHeader( bool bShow );

	// Загрузить Header
					void	LoadHeader( bool bNeedReloadHeader );

					void	SetColumnsByWidth( int iClientWidth );

					void	SetItemWidth( int iItemIndex, int iColumnWidth );
					void	SetHeaderItemWidth( CHeaderCtrl* pHeader, int iItemIndex, int iColumnWidth );
					void	SetHeaderItem( int iItemIndex, LPCTSTR pColumnText, int nFormat );
					void	NormalizeColumnWidth( int iClientWidth, int iItemIndex, int &iColumnWidth, int &iColumnPercent );

					bool	ProportionalResizeHeader( NMHEADER *pNMHeader );
					int		CorrectItemSize( int nItemIndex, int nItemSize );

					void	Lock( int nIndex );
					void	Unlock( int nIndex );

					int		GetRowColumnIndex( CPoint &rcPoint, RECT *pCellRect, int *pnColumn ) const;

					CString GetTrueItemText( int nRow, int nCol );
					int			GetTrueColumnWidth( int nCurrentPosition ) const;
					BOOL		CheckTextFitItsRect( int nRow, int nCol );

	virtual BOOL		CheckTextContentsShouldBeTipped( LPCTSTR pText );


	// Generated message map functions
	//{{AFX_MSG(CCheckList)
	afx_msg void		OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void		OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg void		OnKeyUp(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg void		OnChar(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg int			OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void		OnSetFocus(CWnd* pOldWnd);
	afx_msg void		OnSize(UINT nType, int cx, int cy);
	afx_msg void		OnWindowPosChanged(WINDOWPOS FAR* lpwndpos);
	afx_msg void		OnWindowPosChanging(WINDOWPOS FAR* lpwndpos);
	afx_msg void		OnDestroy();
	afx_msg LRESULT OnDefferedPosChanging( WPARAM wParam, LPARAM lParam );
	afx_msg LRESULT OnDefferedPosChanged( WPARAM wParam, LPARAM lParam );
	afx_msg void		OnItemChanging(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void		OnItemChanged(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void		OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void		OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	afx_msg BOOL		OnEraseBkgnd(CDC* pDC);
	afx_msg void		OnPaint();
		//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_CHECKLIST_H__D8037F25_ABAF_11D2_96B0_00104B5B66AA__INCLUDED_)
