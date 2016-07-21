#ifndef WIN32API_BASED_HYPERLINK_CLASS_EGFUDIOYR
#define WIN32API_BASED_HYPERLINK_CLASS_EGFUDIOYR

const TCHAR g_cszHyperlinkClassName[] = _T("kavhyperlink_ctrl_78swdkg");//

// wParam - DWORD Color index from GetSysColor, lParam - should be always TRUE
static UINT nSetBkColor = WM_USER + 5;


#define RGB_COLOR_LINK_TEXT		RGB(0,0,175)
#define RGB_COLOR_LINK_TEXT_HOVER RGB(0,0,255)

//nSetBkColor indexes
#define COLOR_BACKTEXT_WHITE	COLOR_3DHILIGHT
#define COLOR_DIALOG_BACKGR		COLOR_3DFACE

typedef struct tagHYPERLINKINFO
{
	HWND hParent;
	RECT ClientRect;
	DWORD dwStyle;

	// attributes
	HFONT hFont;
	COLORREF crTextColor;
	COLORREF crTextColorHover;

	// internal attributes
	HINSTANCE hInst;
	HINSTANCE hModule;

} HYPERLINKINF, *LPHYPERLINKINF;


#define STATE_MOUSE_LEAVE		0
#define STATE_MOUSE_ENTER		1
#define STATE_KEYBOARD_FOCUS	(1<<1)
#define STATE_MOUSE_LBTN_DOWN	(1<<2)
#define STATE_MOUSE_LBTN_UP		(1<<3)

// define timer IDs
#define TIMER_CHECK_VIEW		101

class CHyperLink;


// Function name	: CreateHyperlinkControl
// Description	    : 
// Return type		: HWND Control HWND
// Argument         : HWND hwndParent		- Parent window handle,
// Argument         : int nControlID		- control ID
// Argument         : int nControlTextID	- control internal text ID
// Argument         : HINSTANCE hLoc		- instance where to load string
// Argument         : HINSTANCE hModule		- module HINSTANCE where Hyperlink is placed
// Argument         : int nRectID			- rectID
// Argument         : HFONT hFont = NULL	- font 
CHyperLink* CreateHyperlinkControl(HWND hwndParent, int nControlID, int nControlTextID, HINSTANCE hLoc, HINSTANCE hModule, int nRectID, HFONT hFont = NULL);

class CHyperLink
{
public:
	HWND m_hWnd;
	LPHYPERLINKINF m_pParam;
	static BOOL WINAPI WndProc (HWND hwndDlg, UINT uMsg, WPARAM wParam, LPARAM lParam);
	CHyperLink();
	~CHyperLink();
	BOOL Create(LPHYPERLINKINF pInfo, int nCtrlID);
	BOOL SetText(LPARAM lParam);
	int GetText(PTCHAR szCopyTo, int nSize);
	int DrawItem(HDC hDC, LPRECT lpRect);
	void SetState(DWORD dwState);
	DWORD GetState();
	void RemoveState(DWORD dwState);
	int SendParentPress(HWND hwndDlg);
	void RedrawText(HWND hwndDlg);

	void SetDrawTestFormat(UINT nDrawTextFormat);

	
	RECT m_sFocusedRect;// size of focused rectangle
	HCURSOR m_hPrevCursor;

private:
	ATOM MyRegisterClass(LPHYPERLINKINF pHLI, PTCHAR pszWindowClass);
	int DrawText(HDC hDC, LPRECT lpRect);
	DWORD m_dwState;
	UINT m_nDrawTextFormat;
	PTCHAR m_pszText;
	DWORD m_dwBkColor;
};


#endif