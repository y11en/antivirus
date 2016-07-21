#include "stdafx.h"
#include <tchar.h>
#include "ControlTreeIPEdit.h"
#include "WASCRes.rh"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

// Style bits for the individual edit controls
const int WS_EDIT = WS_CHILD | WS_VISIBLE | ES_CENTER | ES_MULTILINE;

static const UINT mGetEncloseRect = ::RegisterWindowMessage( _T("GetEncloseRect") ); 
static const UINT mEditKillFocus  = ::RegisterWindowMessage( _T("EditKillFocus") ); 
static const UINT mGetIPAddress	  = ::RegisterWindowMessage( _T("GetIPAddress") ); 
static const UINT mChildInvalid	  = ::RegisterWindowMessage( _T("ChildInvalid") ); 


static const UINT nLoadLocalisedStringMessage = ::RegisterWindowMessage(_T("AVPControlsLoadLocalisedStringMessage"));

// ---
struct InfalidMessInfo {
	CEdit		*m_pChild;
	CString  m_Text;
};

// ---
static CString LoadLocString(CWnd *pLocWnd, UINT uStrID ) {
	CString str;
	str.LoadString(uStrID);
	TCHAR *pStr = str.GetBuffer(_MAX_PATH);
	pLocWnd->SendMessage(nLoadLocalisedStringMessage, uStrID, LPARAM(pStr) );
	str.ReleaseBuffer();
	return str;
}
/*
// ---
static HWND GetParentOwner( HWND hWnd ) {
	return (::GetWindowLong(hWnd, GWL_STYLE) & WS_CHILD) ?
		::GetParent(hWnd) : ::GetWindow(hWnd, GW_OWNER);
}

// ---
static HWND GetTopLevelParent( HWND hWnd ) {
	HWND hWndParent = hWnd;
	while ((hWnd = ::GetParentOwner(hWndParent)) != NULL)
		hWndParent = hWnd;

	return hWndParent;
}
*/

/////////////////////////////////////////////////////////////////////////////
// CControlTreeIPCtl

CControlTreeIPCtl::CControlTreeIPCtl( HTREEITEM hItem, CWnd *pLocWnd ) :
	m_bESC( false ),
	m_hItem( hItem ),
	m_pLocWnd( pLocWnd )
{
	m_bEnabled = TRUE;								// Window enabled flag (TRUE by default)
	m_bReadOnly = FALSE;							// Read only flag (FALSE by default)
	m_bNoValidate = FALSE;							// Don't do immediate field validation on input
}

CControlTreeIPCtl::~CControlTreeIPCtl()
{
}


// ---
void CControlTreeIPCtl::EndWorkingProcessing() {
	CString str;

	IPA_ADDR rcAddr;
	if ( OnGetAddress(!m_bESC, (LONG)&rcAddr) || m_bESC) {
		str.Format( _T("%u.%u.%u.%u"), rcAddr.nAddr1, rcAddr.nAddr2, rcAddr.nAddr3, rcAddr.nAddr4 );

		// Send Notification to parent of ListView ctrl
		TV_DISPINFO dispinfo;
		dispinfo.hdr.hwndFrom = GetParent()->GetSafeHwnd();
		dispinfo.hdr.idFrom = GetDlgCtrlID();
		dispinfo.hdr.code = TVN_ENDLABELEDIT;

		dispinfo.item.mask = TVIF_TEXT;
		dispinfo.item.hItem = m_hItem;
		dispinfo.item.pszText = m_bESC ? NULL : LPTSTR((LPCTSTR)str);
		dispinfo.item.cchTextMax = str.GetLength();

		GetParent()->GetParent()->SendMessage( WM_NOTIFY, GetParent()->GetDlgCtrlID(), (LPARAM)&dispinfo );

		PostMessage( WM_CLOSE );
	}
}

// ---
void CControlTreeIPCtl::GetEncloseRect( CRect &rcEncloseRect ) {
	GetWindowRect( &rcEncloseRect );
	GetParent()->ScreenToClient( &rcEncloseRect );

	CDC*	pDC = GetDC();
	pDC->SelectObject( GetFont() );
	CSize	szDot = pDC->GetTextExtent(_T("."), 1);
	CSize	szNum = pDC->GetTextExtent(_T("0"), 1);

	int nXBorder = ::GetSystemMetrics( SM_CXBORDER );
	int nYBorder = ::GetSystemMetrics( SM_CXBORDER );

	rcEncloseRect.bottom = rcEncloseRect.top + szNum.cy + nYBorder * 2;
	rcEncloseRect.right = rcEncloseRect.left + szDot.cx * 3 + szNum.cx * 3 * 4 + nXBorder * 2;

	ReleaseDC(pDC);	
}


BEGIN_MESSAGE_MAP(CControlTreeIPCtl, CWnd)
	//{{AFX_MSG_MAP(CControlTreeIPCtl)
	ON_WM_CREATE()
	ON_WM_NCDESTROY()
	ON_WM_SIZE()
	ON_WM_SETFOCUS()
	ON_WM_PAINT()
	ON_WM_ENABLE()
	ON_WM_ERASEBKGND()
	ON_MESSAGE(WM_SETFONT, OnSetFont)
	ON_MESSAGE(IPAM_GETADDRESS, OnGetAddress)
	ON_MESSAGE(IPAM_SETADDRESS, OnSetAddress)
	ON_MESSAGE(IPAM_SETREADONLY, OnSetReadOnly)
	ON_REGISTERED_MESSAGE(mGetEncloseRect, OnGetEncloseRect)
	ON_REGISTERED_MESSAGE(mEditKillFocus, OnEditKillFocus)
	ON_REGISTERED_MESSAGE(mGetIPAddress, OnGetIPAddress)
	ON_REGISTERED_MESSAGE(mChildInvalid, OnChildInvalid)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// CControlTreeIPCtl message handlers

BOOL CControlTreeIPCtl::Create(DWORD dwStyle, const RECT& rect, CWnd* pParentWnd, UINT nID, DWORD dwExStyle/*=0*/)
{
	// Create a window class that has the properties we want
	CString szWndClass = AfxRegisterWndClass(CS_GLOBALCLASS | CS_VREDRAW | CS_HREDRAW,
		::LoadCursor(NULL, IDC_IBEAM), (HBRUSH) COLOR_WINDOW+1);

	// Create using the extended window style
#if _MSC_VER >= 1100
	// Original VC 5.0 stuff
	return CWnd::CreateEx(dwExStyle, szWndClass, NULL, dwStyle, rect, pParentWnd, nID);
#else
	// Back ported to VC 4.2
	return CWnd::CreateEx(dwExStyle, szWndClass, NULL, dwStyle,
		rect.left, rect.top, rect.right-rect.left, rect.bottom-rect.top,
		pParentWnd->GetSafeHwnd(), (HMENU) nID);
#endif
}

int CControlTreeIPCtl::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if (CWnd::OnCreate(lpCreateStruct) == -1)	
		return -1;

	// Save the "no immediate validation on input" style setting
	m_bNoValidate = (lpCreateStruct->style & IPAS_NOVALIDATE);

	// Set the styles for the parent control
	ModifyStyleEx(0, WS_EX_NOPARENTNOTIFY);

	// Create the four edit controls used to obtain the four parts of the IP address (size
	// of controls gets set during OnSize)
	for (int ii = 0; ii < 4; ii++)
	{
		if (m_Addr[ii].GetSafeHwnd() == NULL)
		{
			m_Addr[ii].Create(WS_EDIT, CRect(0,0,0,0), this, IDC_ADDR1 + ii);
			m_Addr[ii].LimitText(3);
			m_Addr[ii].SetParent(this);
		}
	}
	
	// Set the proper font
	CFont* font = GetParent()->GetFont();
	SetFont( font );

	return 0;
}

// ---
LRESULT CControlTreeIPCtl::OnEditKillFocus( WPARAM wParam, LPARAM lParam ) {
	HWND hFocusWnd = (HWND)lParam;
	if ( !hFocusWnd || !::IsChild(*this, hFocusWnd) ) {
		EndWorkingProcessing();
	}
	return 0;
}


// ---
LRESULT CControlTreeIPCtl::OnGetEncloseRect( WPARAM wParam, LPARAM lParam ) {
	GetEncloseRect( *(CRect *)lParam );
	return 0;
}

// ---
LRESULT CControlTreeIPCtl::OnGetIPAddress( WPARAM wParam, LPARAM lParam ) {
	OnGetAddress( wParam, lParam );
	return TRUE;
}


// ---
void CControlTreeIPCtl::OnNcDestroy() {
	CWnd::OnNcDestroy();
	
	// Make sure the window was destroyed
	ASSERT(NULL == m_hWnd);

	// Destroy this object since it won't be destroyed otherwise
	delete this;
}

void CControlTreeIPCtl::OnSize(UINT nType, int cx, int cy) 
{
	CWnd::OnSize(nType, cx, cy);

	// Get the width of a "." drawn in the control
	CDC*	pDC = GetDC();
	CSize	szDot = pDC->GetTextExtent(_T("."), 1);
	int nDotWidth = szDot.cx;
	ReleaseDC(pDC);	

	// Based on the size of the parent window, compute the width & height of the edit
	// controls.  Leave room for the three "." which will be drawn on the parent window
	// to separate the four parts of the IP address.
	CRect	rcClient;
	GetClientRect(&rcClient);
	int nEditWidth = (rcClient.Width() - (3 * nDotWidth)) / 4;
	int nEditHeight = rcClient.Height();
	int cyEdge = ::GetSystemMetrics(SM_CYEDGE);

	// Compute rectangles for the edit controls, then move the controls into place
	CRect rect = CRect(0, cyEdge, nEditWidth, nEditHeight);
	int ii;
	for (ii = 0; ii < 4; ii++)
	{
		m_rcAddr[ii] = rect;
		m_Addr[ii].MoveWindow(rect);
		rect.OffsetRect(nEditWidth + nDotWidth, 0);
	}

	rect = CRect(nEditWidth, 0, nEditWidth + nDotWidth, nEditHeight);
	for (ii = 0; ii < 3; ii++)
	{
		m_rcDot[ii] = rect;
		rect.OffsetRect(nEditWidth + nDotWidth, 0);
	}
}

void CControlTreeIPCtl::OnSetFocus(CWnd* pOldWnd) 
{
	CWnd::OnSetFocus(pOldWnd);
	
	m_Addr[0].SetFocus();							// Set focus to first edit control
	m_Addr[0].SetSel(0, -1);						// Select entire contents
}

// Protected function called by the edit control (friend class) when it receives a
// character which should be processed by the parent
void CControlTreeIPCtl::OnChildChar( UINT nChar, UINT nRepCnt, UINT nFlags, CControlTreeIPEdit& child ) {
	switch ( nChar )	{
		case L'.':										// Dot means advance to next edit control (if in first 3)
		case VK_RIGHT:									// Ditto for right arrow at end of text
		case L' ':										// Ditto for space
			{
			UINT nIDC = child.GetDlgCtrlID();			// Get control ID of the edit control
			if (nIDC < IDC_ADDR4)						// Move focus to appropriate edit control and select entire contents
			{
				m_Addr[nIDC - IDC_ADDR1 + 1].SetFocus();
				if (VK_RIGHT != nChar)					// Re-select text unless arrow key entered
					m_Addr[nIDC - IDC_ADDR1 + 1].SetSel(0, -1);
			}		
			break;
			}

		case VK_LEFT:									// Left arrow means move to previous edit control (if in last 3)
			{
			UINT nIDC = child.GetDlgCtrlID();			// Get control ID of the edit control
			if (nIDC > IDC_ADDR1)						// Move focus to appropriate edit control
				m_Addr[nIDC - IDC_ADDR1 - 1].SetFocus();
			break;
			}

		case VK_ESCAPE :
			m_bESC = true;
		case VK_TAB:									// Tab moves between controls in the dialog
		case VK_RETURN:									// Return implies default pushbutton press
			GetParent()->SetFocus();
			break;

		case '-':										// "Field full" indication
			// Validate the contents for proper values (unless suppressed)
			if (!m_bNoValidate)							// If not suppressing immediate validation
			{
				CString		szText;

				child.GetWindowText(szText);			// Get text from edit control
				int n = _ttoi(szText);					// Get numeric value from edit control
				if (n < 0 || n > 255)					// If out of range, notify parent
				{
					CString szFormat = ::LoadLocString(m_pLocWnd, IDS_WAS_ERR_OUTOFRANGE_D);
					szText.Format(szFormat, n, 0, 255);

					InfalidMessInfo *pInfo = new InfalidMessInfo;
					pInfo->m_pChild = &child;
					pInfo->m_Text = szText;
					GetParent()->PostMessage( mChildInvalid, WPARAM(GetSafeHwnd()), LPARAM(pInfo) );
					return;
				}
			}

			// Advance to next field
			OnChildChar(L'.', 0, nFlags, child);
			break;

		default:
			TRACE(_T("Unexpected call to CControlTreeIPCtl::OnChildChar!\n"));
	}
}

void CControlTreeIPCtl::OnPaint() 
{
	CPaintDC dc(this); // device context for painting

	// Save mode and set to transparent (so background remains)
	int nOldMode = dc.SetBkMode(TRANSPARENT);

	// If disabled, set text color to COLOR_GRAYTEXT, else use COLOR_WINDOWTEXT
	COLORREF crText;
	if (m_bEnabled)
		crText = ::GetSysColor(COLOR_WINDOWTEXT);
	else
		crText = ::GetSysColor(COLOR_GRAYTEXT);
	COLORREF crOldText = dc.SetTextColor(crText);

	// Draw the three "." which separate the four edit controls
	for (int ii = 0; ii < 3; ii++)
		dc.DrawText(_T("."), 1, m_rcDot[ii], DT_CENTER | DT_SINGLELINE | DT_BOTTOM);

	// Restore old mode and color
	dc.SetBkMode(nOldMode);
	dc.SetTextColor(crOldText);

	// Do not call CWnd::OnPaint() for painting messages
}

BOOL CControlTreeIPCtl::OnEraseBkgnd(CDC* pDC) 
{
	CRect	rcClient;
	GetClientRect(&rcClient);

	if (m_bEnabled && !m_bReadOnly)
		::FillRect(pDC->m_hDC, rcClient, (HBRUSH) (COLOR_WINDOW+1));
	else
		::FillRect(pDC->m_hDC, rcClient, (HBRUSH) (COLOR_BTNFACE+1));

	return TRUE;
}

void CControlTreeIPCtl::OnEnable(BOOL bEnable) 
{
	CWnd::OnEnable(bEnable);
	
	// Nothing to do unless the window state has changed
	if (bEnable != m_bEnabled)
	{
		// Save new state
		m_bEnabled = bEnable;

		// Adjust child controls appropriately
		for (int ii = 0; ii < 4; ii++)
			m_Addr[ii].EnableWindow(bEnable);
	}

	Invalidate();
}

LONG CControlTreeIPCtl::OnSetFont(UINT wParam, LONG lParam)
{
	// Note: font passed on to children, but we don't
	// use it, the system font is much nicer for printing
	// the dots, since they show up much better

	for (int ii = 0; ii < 4; ii++)
		m_Addr[ii].SendMessage(WM_SETFONT, wParam, lParam);

	return 0;
}

LONG CControlTreeIPCtl::OnGetAddress(UINT wParam, LONG lParam)
{
	BOOL bStatus;
	int	i, nAddr[4], nInError = 0;

	BOOL bPrintErrors = (BOOL) wParam;				// Cast wParam as a flag
	IPA_ADDR* lpIPAddr = (IPA_ADDR*) lParam;		// Cast lParam as an IPA_ADDR structure
	if (NULL == lpIPAddr)							// If it's a bad pointer, return an error
		return FALSE;
	memset(lpIPAddr, 0, sizeof(IPA_ADDR));			// Zero out the returned data

	// Parse the fields and return an error indication if something bad was detected
	for (i = 0; i < 4; i++)
	{
		bStatus = ParseAddressPart(m_Addr[i], nAddr[i]);
		if (!bStatus)								// If it failed to parse, quit now
		{
			nInError = i + 1;						// Remember which address part was in error
			break;									// Break out of for loop
		}
	}
	if (!bStatus)									// Error detected during parse?
	{
		lpIPAddr->nInError = nInError;				// Show where it occurred
		if (bPrintErrors)							// If they want us to print error messages
		{
			CString	szText;
			CString szFormat;
			if (nAddr[i] < 0)
				szText = ::LoadLocString(m_pLocWnd, IDS_IPE_MISSINGVALUE);
			else {
				szFormat = ::LoadLocString(m_pLocWnd, IDS_WAS_ERR_OUTOFRANGE_D);
				szText.Format(szFormat, nAddr[i], 0, 255);
			}

			InfalidMessInfo *pInfo = new InfalidMessInfo;
			pInfo->m_pChild = &m_Addr[i];
			pInfo->m_Text = szText;
			GetParent()->PostMessage( mChildInvalid, WPARAM(GetSafeHwnd()), LPARAM(pInfo) );
		}
		return FALSE;								// Return an error
	}

	lpIPAddr->nAddr1 = nAddr[0];					// Return the pieces to the caller
	lpIPAddr->nAddr2 = nAddr[1];
	lpIPAddr->nAddr3 = nAddr[2];
	lpIPAddr->nAddr4 = nAddr[3];
	lpIPAddr->nInError = 0;							// No error to report
	return TRUE;									// Return success
}

LONG CControlTreeIPCtl::OnSetAddress(UINT wParam, LONG lParam)
{
	CString		szText;

	IPA_ADDR* lpIPAddr = (IPA_ADDR*) lParam;		// Cast lParam as an IPA_ADDR structure

	// Format their data and load the edit controls
	szText.Format(_T("%u"), lpIPAddr->nAddr1);
	m_Addr[0].SetWindowText(szText);
	szText.Format(_T("%u"), lpIPAddr->nAddr2);
	m_Addr[1].SetWindowText(szText);
	szText.Format(_T("%u"), lpIPAddr->nAddr3);
	m_Addr[2].SetWindowText(szText);
	szText.Format(_T("%u"), lpIPAddr->nAddr4);
	m_Addr[3].SetWindowText(szText);
	return TRUE;
}

LONG CControlTreeIPCtl::OnSetReadOnly(UINT wParam, LONG lParam)
{
	m_bReadOnly = (BOOL) wParam;

	for (int ii = 0; ii < 4; ii++)
		m_Addr[ii].SetReadOnly(m_bReadOnly);

	Invalidate();
	return TRUE;
}

BOOL CControlTreeIPCtl::ParseAddressPart(CEdit& edit, int& n)
{
	CString		szText;

	edit.GetWindowText(szText);						// Get text from edit control
	if (szText.IsEmpty())							// Empty text is an error
	{
		n = -1;										// Return bogus value
		return FALSE;								// Return parse failure to caller
	}

	n = _ttoi(szText);								// Grab a decimal value from edit text
	if (n < 0 || n > 255)							// If it is out of range, return an error
		return FALSE;

	return TRUE;									// Looks acceptable, return success
}

CControlTreeIPEdit* CControlTreeIPCtl::GetEditControl(int nIndex)
{
	if (nIndex < 1 || nIndex > 4)
		return NULL;
	return &m_Addr[nIndex - 1];
}

LONG CControlTreeIPCtl::OnChildInvalid(UINT wParam, LONG lParam) {
	CString szTitle = ::LoadLocString(m_pLocWnd, IDS_ERRTITLE);
	GetParent()->BeginModalState();
	::MessageBox(NULL, ((InfalidMessInfo *)lParam)->m_Text, szTitle, MB_OK | MB_ICONEXCLAMATION | MB_TASKMODAL );
//	GetTopLevelParent()->MessageBox(((InfalidMessInfo *)lParam)->m_Text, szTitle, MB_OK | MB_ICONEXCLAMATION);
	GetParent()->EndModalState();

	((InfalidMessInfo *)lParam)->m_pChild->SetFocus();						// Set focus to offending byte
	((InfalidMessInfo *)lParam)->m_pChild->SetSel(0, -1);					// Select entire contents

	delete (InfalidMessInfo *)lParam;

	return 0;
}


/////////////////////////////////////////////////////////////////////////////
// CControlTreeIPEdit

CControlTreeIPEdit::CControlTreeIPEdit()
{
}

CControlTreeIPEdit::~CControlTreeIPEdit()
{
}

void CControlTreeIPEdit::SetParent(CControlTreeIPCtl* pParent)
{
	m_pParent = pParent;							// Save pointer to parent control
}


BEGIN_MESSAGE_MAP(CControlTreeIPEdit, CEdit)
	//{{AFX_MSG_MAP(CControlTreeIPEdit)
	ON_WM_CHAR()
	ON_WM_KEYDOWN()
	ON_WM_KILLFOCUS()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CControlTreeIPEdit message handlers

void CControlTreeIPEdit::OnChar(UINT nChar, UINT nRepCnt, UINT nFlags) 
{
	// Logic for this function:
	// Tab and dot are forwarded to the parent.  Tab (or shift-tab) operate
	// just like expected (focus moves to next control after the parent).
	// Dot or space causes the parent to set focus to the next child edit (if
	// focus is currently set to one of the first three edit controls).
	// Numerics (0..9) and control characters are forwarded to the standard
	// CEdit OnChar method; all other characters are dropped.
	if (VK_TAB == nChar ||
			L'.' == nChar ||
			L' ' == nChar ||
		VK_RETURN == nChar ||
		VK_ESCAPE == nChar)
		m_pParent->OnChildChar(nChar, nRepCnt, nFlags, *this);
	else if ( _istdigit(nChar) || _istcntrl(nChar))
	{
		CEdit::OnChar(nChar, nRepCnt, nFlags);

		// Automatically advance to next child control if 3 characters were entered;
		// use "-" to indicate field was full to OnChildChar
		if (3 == GetWindowTextLength())
			m_pParent->OnChildChar(L'-', 0, nFlags, *this);
	}
	else
		::MessageBeep(0xFFFFFFFF);
}

void CControlTreeIPEdit::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags) 
{
	// Handle the left and right arrow keys.  If the left arrow key is pressed
	// with the caret at the left of the input text, shift focus to the previous
	// control (if in edit controls 2-4).  Likewise for the right arrow key.
	// This is done by calling the parent's OnChildChar method.
	// If not left or right arrow, or not at beginning or end, call default
	// OnKeyDown processor so key down gets passed to edit control.
	if (VK_LEFT == nChar || VK_RIGHT == nChar)
	{
		CPoint ptCaret = GetCaretPos();
		int nCharPos = LOWORD(CharFromPos(ptCaret));
		if ((VK_LEFT == nChar && nCharPos == 0) ||
			(VK_RIGHT == nChar && nCharPos == GetWindowTextLength()))
			m_pParent->OnChildChar(nChar, nRepCnt, nFlags, *this);
	}
	CEdit::OnKeyDown(nChar, nRepCnt, nFlags);
}

// ---
void CControlTreeIPEdit::OnKillFocus( CWnd* pNewWnd ) {
	CEdit::OnKillFocus(pNewWnd);
  
	GetParent()->SendMessage( mEditKillFocus, 0, LPARAM(pNewWnd->GetSafeHwnd()) );
}


