//*************************************************************************
// ODMenu.h : header file
// Version : 2.4
// Date : Feb 11, 1999
// Author : Brent Corkum
// 
// Portions of code supplied by:
// Ben Ashley,Girish Bharadwaj,Jean-Edouard Lachand-Robert,
// Robert Edward Caldecott,Kenny Goers,Leonardo Zide,
// Stefan Kuhr
//
// Bug Fixes:
// Stefan Kuhr,Martin Vladic,Kim Yoo Chul
//
// You are free to use/modify this code but leave this header intact.
//

#ifndef __ODMENU_H__
#define __ODMENU_H__

#include <Stuff\PArray.h>
#include <Stuff\SArray.h>

// CODMenuData class. Fill this class structure to define a single menu item:

class CODMenuData
{
	wchar_t *m_szMenuText;
public:
	CODMenuData () {menuIconNormal=-1;xoffset=-1;bitmap=NULL;
	nFlags=0;nID=0;syncflag=0;m_szMenuText=NULL;};
	void SetAnsiString(LPCSTR szAnsiString);
	void SetWideString(const wchar_t *szWideString);
	const wchar_t *GetWideString(void) {return m_szMenuText;};
	~CODMenuData ();
	CString GetString(void);//returns the menu text in ANSI or UNICODE
	int xoffset;
	int menuIconNormal;
	UINT nFlags,nID,syncflag;
	CImageList *bitmap;
};

//struct CMenuItemInfo : public MENUITEMINFO {
struct CMenuItemInfo : public 
//MENUITEMINFO 
#ifndef UNICODE   //SK: this fixes warning C4097: typedef-name 'MENUITEMINFO' used as synonym for class-name 'tagMENUITEMINFOA'
tagMENUITEMINFOA
#else
tagMENUITEMINFOW
#endif
{
	CMenuItemInfo()
	{ memset(this, 0, sizeof(MENUITEMINFO));
	cbSize = sizeof(MENUITEMINFO);
	}
};




typedef enum {Normal,TextOnly} HIGHLIGHTSTYLE;

#ifndef UNICODE
#define AppendODMenu AppendODMenuA
#define ModifyODMenu ModifyODMenuA
#else
#define AppendODMenu AppendODMenuW
#define ModifyODMenu ModifyODMenuW
#endif

class CODMenu;

// ---
class CODMenuDataArray : public CPArray<CODMenuData> {
public :
	CODMenuDataArray( bool owns = false ) : CPArray<CODMenuData>( 0, 1, owns ) {}
	int  GetUpperBound() const { return MaxIndex(); }
	int  Append( const CODMenuDataArray &other );
};


// ---
class CODMenuArray : public CPArray<CODMenu> {
public :
	CODMenuArray( bool owns = false ) : CPArray<CODMenu>( 0, 1, owns ) {}
	int  GetUpperBound() const { return MaxIndex(); }
};


class CODMenu : public CMenu  // Derived from CMenu
{
	// Construction
public:
	CODMenu(CWnd *pParentWnd = NULL); 
	// Attributes
protected:
	CODMenuDataArray m_MenuList;  // Stores list of menu items 
	// When loading an owner-drawn menu using a Resource, CODMenu must keep track of
	// the popup menu's that it creates. Warning, this list *MUST* be destroyed
	// last item first :)
	
	CODMenuArray     m_SubMenus;  // Stores list of sub-menus 
	// Operations
public: 
	// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CCustomMenu)
	//}}AFX_VIRTUAL 
	// Implementation
public:
	void SetParentWnd(CWnd* pParentWnd);
	CWnd* GetParentWnd();
	static BOOL IsNewShell(void);
	void SetBitmapBackground(COLORREF color);
	void SetDisableOldStyle(void);
	void UnSetDisableOldStyle(void);
	BOOL GetDisableOldStyle(void);
	void UnSetBitmapBackground(void);
	int AddBitmapToImageList(CImageList *list,UINT nResourceID);
	int AddBitmapToImageList(CImageList *bmplist,HBITMAP hbmp);
	BOOL LoadFromToolBar(UINT nID,UINT nToolBar,int& xoffset);
	void InsertSpaces(void);
	static LRESULT FindKeyboardShortcut(UINT nChar,UINT nFlags,CMenu *pMenu);
	static void UpdateMenu(CMenu *pmenu);
	BOOL IsMenu(CMenu *submenu);
	void DrawCheckMark(CDC *pDC,int x,int y,COLORREF color);
	void DrawRadioDot(CDC *pDC,int x,int y,COLORREF color);
	CODMenu *FindMenuOption(int nId,int& nLoc);
	CODMenuData *FindMenuOption(wchar_t *lpstrText);
	BOOL GetMenuText(UINT id,CString &string);
	CImageList *checkmaps;
	BOOL checkmapsshare;
	int m_selectcheck;
	int m_unselectcheck;
	void LoadCheckmarkBitmap(int unselect,int select);
	void DitherBlt(HDC hdcDest, int nXDest, int nYDest, int nWidth, 
		int nHeight, HBITMAP hbm, int nXSrc, int nYSrc);
	void DitherBlt2(CDC *drawdc, int nXDest, int nYDest, int nWidth, 
		int nHeight, CBitmap &bmp, int nXSrc, int nYSrc);
	void DitherBlt( CBitmap &srcBmp, CBitmap &dstBmp );
	HBITMAP LoadSysColorBitmap(int nResourceId);
	
	virtual ~CODMenu();  // Virtual Destructor 
	// Drawing: 
	virtual void DrawItem( LPDRAWITEMSTRUCT);  // Draw an item
	virtual void MeasureItem( LPMEASUREITEMSTRUCT );  // Measure an item
	
	// Customizing:
	
	void SetIconSize (int, int);  // Set icon size
	
	BOOL AppendODMenuA(LPCSTR lpstrText, 
		UINT nFlags = MF_OWNERDRAW,
		UINT nID = 0,
		int nIconNormal = -1); // Owner-Drawn Append 
	
	BOOL AppendODMenuW(wchar_t *lpstrText, 
		UINT nFlags = MF_OWNERDRAW,
		UINT nID = 0,
		int nIconNormal = -1); // Owner-Drawn Append 
	
	CODMenu* GetSubMenu( int nPos ) const {
		return (CODMenu*)CMenu::GetSubMenu(nPos);
	}

	BOOL ModifyODMenuA(const char *lpstrText,UINT nID=0,int nIconNormal=-1);
	BOOL ModifyODMenuA(const char *lpstrText,const char *OptionText,int nIconNormal);
	BOOL ModifyODMenuW(wchar_t *lpstrText,UINT nID=0,int nIconNormal=-1);
	BOOL ModifyODMenuW(wchar_t *lpstrText,wchar_t *OptionText,int nIconNormal);
	BOOL ModifyODMenuA(const char * lpstrText,UINT nID,HBITMAP hIconNormal);
	BOOL ModifyODMenuW(wchar_t *lpstrText,UINT nID,HBITMAP hIconNormal);
	CODMenuData *NewODMenu(UINT pos,UINT nFlags,UINT nID,CString string);
	void SynchronizeMenu(void);
	void SynchronizeIcons(void);
	void InitializeMenuList(int value);
	void DeleteMenuList(void);
	CODMenuData *FindMenuList(UINT nID);
	virtual BOOL LoadMenu(LPCTSTR lpszResourceName);  // Load a menu
	virtual BOOL LoadMenu(int nResource);  // ... 
	void AddFromToolBar(CToolBar* pToolBar, int nResourceID);
	BOOL Draw3DCheckmark(CDC *dc, const CRect& rc,BOOL bSelected,
		HBITMAP hbmCheck);
	BOOL LoadToolbar(UINT nToolBar);
	BOOL LoadToolbars(const UINT *arID,int n);
	
	// Destoying:
	
	virtual BOOL DestroyMenu();
	
	// Generated message map functions
protected:
	int m_iconX,m_iconY;
	COLORREF m_bitmapBackground;
	BOOL m_bitmapBackgroundFlag;
	BOOL disable_old_style;
	CWnd* m_pParentWnd;
}; 

#endif // __CJMENU_H__

//*************************************************************************
