////////////////////////////////////////////////////////////////////
//
//  ControlTreePIDEdit.cpp
//  DT PID Custom Control Implementation
//  AVP Custom controls
//  Victor Matiouchenkov [victor@avp.ru], Kaspersky Labs. 2000
//  Copyright (c) Kaspersky Labs
//
////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include <tchar.h>
#include <property/property.h>
#include "PropertySetGet.h"
#include "ControlTreePIDEdit.h"
#include "WASCRes.rh"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

// Style bits for the individual edit controls
const int WS_EDIT = WS_CHILD | WS_VISIBLE | /*ES_CENTER | */ES_MULTILINE;

static const UINT mGetEncloseRect = ::RegisterWindowMessage( _T("GetEncloseRect") ); 
static const UINT mEditKillFocus  = ::RegisterWindowMessage( _T("EditKillFocus") ); 
static const UINT mGetPIDValue	  = ::RegisterWindowMessage( _T("GetPIDValue") ); 
static const UINT mChildInvalid	  = ::RegisterWindowMessage( _T("ChildInvalid") ); 
static const UINT mSpinStep				= ::RegisterWindowMessage( _T("SpinStep") ); 

static const UINT nLoadLocalisedStringMessage = ::RegisterWindowMessage(_T("AVPControlsLoadLocalisedStringMessage"));

// ---
static int SizeValueInDecChars( DWORD nValue ) {
	int nSizeOfDWord = 10;
	int nDigBase     = 1000000000;
	while( !(nValue / nDigBase) ) {
		nDigBase /= 10;
		nSizeOfDWord--;
	}
	return nSizeOfDWord;
}


// ---
static int gMAXTable[] = {
  GET_AVP_PID_ID_MAX_VALUE,
  GET_AVP_PID_APP_MAX_VALUE,
  GET_AVP_PID_TYPE_MAX_VALUE,
  GET_AVP_PID_ARR_MAX_VALUE,
};

// ---
static int gLIMITTable[] = {
	::SizeValueInDecChars(GET_AVP_PID_ID_MAX_VALUE),
	::SizeValueInDecChars(GET_AVP_PID_APP_MAX_VALUE),
	::TypeNameMaxLength(),//::SizeValueInDecChars(GET_AVP_PID_TYPE_MAX_VALUE),
	::SizeValueInDecChars(GET_AVP_PID_ARR_MAX_VALUE),
};

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

/////////////////////////////////////////////////////////////////////////////
// CControlTreePIDCtl

CControlTreePIDCtl::CControlTreePIDCtl( HTREEITEM hItem, CWnd *pLocWnd ) :
	m_bESC( false ),
	m_hItem( hItem ),
	m_pLocWnd( pLocWnd )
{
	m_bEnabled = TRUE;								// Window enabled flag (TRUE by default)
	m_bReadOnly = FALSE;							// Read only flag (FALSE by default)
	m_bNoValidate = FALSE;							// Don't do immediate field validation on input
	m_pSpinForEdit = NULL;

	m_PIDEdits[0] = new CControlTreePIDEdit();
	m_PIDEdits[1] = new CControlTreePIDEdit();
	m_PIDEdits[2] = new CControlTreePIDTypeEdit();
	m_PIDEdits[3] = new CControlTreePIDEdit();
}

CControlTreePIDCtl::~CControlTreePIDCtl()
{
	delete m_pSpinForEdit;
	delete m_PIDEdits[0];
	delete m_PIDEdits[1];
	delete m_PIDEdits[2];
	delete m_PIDEdits[3];
}


// ---
void CControlTreePIDCtl::EndWorkingProcessing() {
	CString str;

	PID_VALUE rcPID;
	if ( OnGetPIDValue(!m_bESC, (LONG)&rcPID) || m_bESC) {
		str.Format( _T("%u,%u,%s,%u"), rcPID.nPIDField1, rcPID.nPIDField2, ::TypeID2TypeName(rcPID.nPIDField3), rcPID.nPIDField4 );

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
void CControlTreePIDCtl::GetEncloseRect( CRect &rcEncloseRect ) {
	GetWindowRect( &rcEncloseRect );
	GetParent()->ScreenToClient( &rcEncloseRect );

	CDC*	pDC = GetDC();
	pDC->SelectObject( GetFont() );
	CSize	szDot = pDC->GetTextExtent(_T(","), 1);
	CSize	szNum = pDC->GetTextExtent(_T("0"), 1);

	int nXBorder = ::GetSystemMetrics( SM_CXBORDER );
	int nYBorder = ::GetSystemMetrics( SM_CXBORDER );

//	rcEncloseRect.bottom = rcEncloseRect.top + szNum.cy + nYBorder * 2;

	int nSize = 0;
	for ( int i=0; i<4; i++ )
		nSize += szNum.cx * gLIMITTable[i];

	rcEncloseRect.right = rcEncloseRect.left + szDot.cx * 4 + nSize + nXBorder * 2;

	if ( m_pSpinForEdit ) {
		CRect spinRect;
		m_pSpinForEdit->GetWindowRect( spinRect );
		GetParent()->ScreenToClient( &spinRect );

		rcEncloseRect.right += spinRect.Width();
	}
	ReleaseDC(pDC);	
}


BEGIN_MESSAGE_MAP(CControlTreePIDCtl, CWnd)
	//{{AFX_MSG_MAP(CControlTreePIDCtl)
	ON_WM_CREATE()
	ON_WM_NCDESTROY()
	ON_WM_SIZE()
	ON_WM_SETFOCUS()
	ON_WM_PAINT()
	ON_WM_ENABLE()
	ON_WM_ERASEBKGND()
	ON_MESSAGE(WM_SETFONT, OnSetFont)
	ON_MESSAGE(PIDM_GETPIDVALUE, OnGetPIDValue)
	ON_MESSAGE(PIDM_SETPIDVALUE, OnSetPIDValue)
	ON_MESSAGE(PIDM_SETREADONLY, OnSetReadOnly)
	ON_REGISTERED_MESSAGE(mGetEncloseRect, OnGetEncloseRect)
	ON_REGISTERED_MESSAGE(mEditKillFocus, OnEditKillFocus)
	ON_REGISTERED_MESSAGE(mGetPIDValue, OnGetPIDValue)
	ON_REGISTERED_MESSAGE(mChildInvalid, OnChildInvalid)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// CControlTreePIDCtl message handlers

BOOL CControlTreePIDCtl::Create(DWORD dwStyle, const RECT& rect, CWnd* pParentWnd, UINT nID, DWORD dwExStyle/*=0*/)
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

int CControlTreePIDCtl::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if (CWnd::OnCreate(lpCreateStruct) == -1)	
		return -1;

	// Save the "no immediate validation on input" style setting
	m_bNoValidate = (lpCreateStruct->style & PIDS_NOVALIDATE);

	// Set the styles for the parent control
	ModifyStyleEx(0, WS_EX_NOPARENTNOTIFY);

	// Create the four edit controls used to obtain the four parts of the IP address (size
	// of controls gets set during OnSize)
	for (int ii = 0; ii < 4; ii++)
	{
		if (m_PIDEdits[ii]->GetSafeHwnd() == NULL)
		{
			m_PIDEdits[ii]->Create(WS_EDIT, CRect(0,0,0,0), this, IDC_PIDEDIT1 + ii);
			m_PIDEdits[ii]->LimitText(gLIMITTable[ii]);
			m_PIDEdits[ii]->SetParent(this);
		}
	}
	
	// Set the proper font
	CFont* font = GetParent()->GetFont();
	SetFont( font );

	return 0;
}

// ---
LRESULT CControlTreePIDCtl::OnEditKillFocus( WPARAM wParam, LPARAM lParam ) {
	HWND hFocusWnd = (HWND)lParam;
	if ( !hFocusWnd || !::IsChild(*this, hFocusWnd) ) {
		EndWorkingProcessing();
	}
	return 0;
}


// ---
LRESULT CControlTreePIDCtl::OnGetEncloseRect( WPARAM wParam, LPARAM lParam ) {
	GetEncloseRect( *(CRect *)lParam );
	return 0;
}

// ---
LRESULT CControlTreePIDCtl::OnGetPID( WPARAM wParam, LPARAM lParam ) {
	OnGetPIDValue( wParam, lParam );
	return TRUE;
}


// ---
void CControlTreePIDCtl::OnNcDestroy() 
{
	CWnd::OnNcDestroy();
	
	// Make sure the window was destroyed
	ASSERT(NULL == m_hWnd);

	// Destroy this object since it won't be destroyed otherwise
	delete this;
}

void CControlTreePIDCtl::OnSize(UINT nType, int cx, int cy) 
{
	CWnd::OnSize(nType, cx, cy);

	// Get the width of a "." drawn in the control
	CDC*	pDC = GetDC();
	pDC->SelectObject( GetFont() );
	CSize	szDot = pDC->GetTextExtent(_T(","), 1);
	CSize	szNum = pDC->GetTextExtent(_T("0"), 1);
	int nDotWidth = szDot.cx;
	ReleaseDC(pDC);	

	// Based on the size of the parent window, compute the width & height of the edit
	// controls.  Leave room for the three "." which will be drawn on the parent window
	// to separate the four parts of the IP address.
	CRect	rcClient;
	GetClientRect(&rcClient);
	int nEditHeight = rcClient.Height();
	int cyEdge = 1;//::GetSystemMetrics(SM_CYEDGE);

	// Compute rectangles for the edit controls, then move the controls into place
	int nEditWidth = szNum.cx * gLIMITTable[0];
	CRect rect = CRect(0, cyEdge, nEditWidth, nEditHeight);
	int ii;
	for (ii = 0; ii < 4; ii++)
	{
		nEditWidth = szNum.cx * gLIMITTable[ii];
		rect.right = rect.left + nEditWidth;
		m_rcPIDRects[ii] = rect;
		m_PIDEdits[ii]->MoveWindow(rect);
		rect.OffsetRect(nEditWidth + nDotWidth, 0);
	}

//	nEditWidth = szNum.cx * gLIMITTable[0];
//	rect = CRect(nEditWidth, 0, nEditWidth + nDotWidth, nEditHeight);
	rect = CRect(0, 0, nDotWidth, nEditHeight);
	for (ii = 0; ii < 3; ii++)
	{
		nEditWidth = szNum.cx * gLIMITTable[ii];
		rect.OffsetRect(nEditWidth, 0);
		m_rcDot[ii] = rect;
		rect.OffsetRect(nDotWidth, 0);
	}

	static bool bOnSizeProcessing = false;
	if ( !bOnSizeProcessing ) {
		bOnSizeProcessing = true;

		SetWindowPos( NULL, 0, 0, cx + ::GetSystemMetrics(SM_CXVSCROLL), cy, SWP_NOMOVE | SWP_NOZORDER | SWP_NOACTIVATE );
		
		if ( !m_pSpinForEdit ) {
			CWnd *parent = GetParent();

			m_pSpinForEdit = new CControlTreePIDSpinButton();
			m_pSpinForEdit->SetParent( this );
			
			CRect rect;
			GetWindowRect( &rect );
			parent->ScreenToClient( &rect );
			
			//rect.left = rect.right;
			rect.right = rect.left + ::GetSystemMetrics( SM_CXVSCROLL );
			
			m_pSpinForEdit->Create( WS_VISIBLE | WS_CHILD | WS_BORDER | UDS_ARROWKEYS | UDS_SETBUDDYINT | UDS_ALIGNRIGHT | UDS_NOTHOUSANDS, rect, parent, 100 );

//			m_pSpinForEdit->SetBuddy( m_PIDEdits[0] );
//			m_pSpinForEdit->SetRange32( 0, gMAXTable[0] );
		}

		m_pSpinForEdit->SetBuddy( this );

		bOnSizeProcessing = false;
	}
}

void CControlTreePIDCtl::OnSetFocus(CWnd* pOldWnd) 
{
	CWnd::OnSetFocus(pOldWnd);
	
	m_PIDEdits[0]->SetFocus();							// Set focus to first edit control
	m_PIDEdits[0]->SetSel(0, -1);						// Select entire contents
}

// Protected function called by the edit control (friend class) when it receives a
// character which should be processed by the parent
void CControlTreePIDCtl::OnChildChar( UINT nChar, UINT nRepCnt, UINT nFlags, CControlTreePIDEdit& child ) {
	UINT nIDC = child.GetDlgCtrlID();			// Get control ID of the edit control
	switch ( nChar )	{
		case L',':										// Dot means advance to next edit control (if in first 3)
		case VK_RIGHT:									// Ditto for right arrow at end of text
		case L' ':										// Ditto for space
			{
			if (nIDC < IDC_PIDEDIT4)						// Move focus to appropriate edit control and select entire contents
			{
				m_PIDEdits[nIDC - IDC_PIDEDIT1 + 1]->SetFocus();
				if (VK_RIGHT != nChar)					// Re-select text unless arrow key entered
					m_PIDEdits[nIDC - IDC_PIDEDIT1 + 1]->SetSel(0, -1);
			}		
			break;
			}

		case VK_LEFT:									// Left arrow means move to previous edit control (if in last 3)
			{
			if (nIDC > IDC_PIDEDIT1)						// Move focus to appropriate edit control
				m_PIDEdits[nIDC - IDC_PIDEDIT1 - 1]->SetFocus();
			break;
			}

		case VK_ESCAPE :
			m_bESC = true;
		case VK_TAB:									// Tab moves between controls in the dialog
		case VK_RETURN:									// Return implies default pushbutton press
			GetParent()->SetFocus();
			break;

		case L'-':										// "Field full" indication
			// Validate the contents for proper values (unless suppressed)
			if (!m_bNoValidate)							// If not suppressing immediate validation
			{
				CString		szText;

				int nIndex = nIDC - IDC_PIDEDIT1;
				child.GetWindowText(szText);			// Get text from edit control
				int n = child.GetPosition();					// Get numeric value from edit control
				if (n < 0 || n > gMAXTable[nIndex])					// If out of range, notify parent
				{
					CString szFormat = ::LoadLocString(m_pLocWnd, IDS_WAS_ERR_OUTOFRANGE_D);
					szText.Format(szFormat, n, 0, gMAXTable[nIndex]);

					InfalidMessInfo *pInfo = new InfalidMessInfo;
					pInfo->m_pChild = &child;
					pInfo->m_Text = szText;
					GetParent()->PostMessage( mChildInvalid, WPARAM(GetSafeHwnd()), LPARAM(pInfo) );
					return;
				}
			}

			// Advance to next field
			OnChildChar(L',', 0, nFlags, child);
			break;

		default:
			TRACE(_T("Unexpected call to CControlTreePIDCtl::OnChildChar!\n"));
	}
}

void CControlTreePIDCtl::OnPaint() 
{
	CPaintDC dc(this); // device context for painting

	// Save mode and set to transparent (so background remains)
	int nOldMode = dc.SetBkMode(TRANSPARENT);
	dc.SelectObject( GetFont() );

	// If disabled, set text color to COLOR_GRAYTEXT, else use COLOR_WINDOWTEXT
	COLORREF crText;
	if (m_bEnabled)
		crText = ::GetSysColor(COLOR_WINDOWTEXT);
	else
		crText = ::GetSysColor(COLOR_GRAYTEXT);
	COLORREF crOldText = dc.SetTextColor(crText);

	// Draw the three "." which separate the four edit controls
	for (int ii = 0; ii < 3; ii++)
		dc.DrawText(_T(","), 1, m_rcDot[ii], DT_CENTER | DT_SINGLELINE | DT_BOTTOM);

	// Restore old mode and color
	dc.SetBkMode(nOldMode);
	dc.SetTextColor(crOldText);

	// Do not call CWnd::OnPaint() for painting messages
}

BOOL CControlTreePIDCtl::OnEraseBkgnd(CDC* pDC) 
{
	CRect	rcClient;
	GetClientRect(&rcClient);

	if (m_bEnabled && !m_bReadOnly)
		::FillRect(pDC->m_hDC, rcClient, (HBRUSH) (COLOR_WINDOW+1));
	else
		::FillRect(pDC->m_hDC, rcClient, (HBRUSH) (COLOR_BTNFACE+1));

	return TRUE;
}

void CControlTreePIDCtl::OnEnable(BOOL bEnable) 
{
	CWnd::OnEnable(bEnable);
	
	// Nothing to do unless the window state has changed
	if (bEnable != m_bEnabled)
	{
		// Save new state
		m_bEnabled = bEnable;

		// Adjust child controls appropriately
		for (int ii = 0; ii < 4; ii++)
			m_PIDEdits[ii]->EnableWindow(bEnable);
	}

	Invalidate();
}

LONG CControlTreePIDCtl::OnSetFont(UINT wParam, LONG lParam)
{
	// Note: font passed on to children, but we don't
	// use it, the system font is much nicer for printing
	// the dots, since they show up much better

	for (int ii = 0; ii < 4; ii++)
		m_PIDEdits[ii]->SendMessage(WM_SETFONT, wParam, lParam);

	return 0;
}

LONG CControlTreePIDCtl::OnGetPIDValue(UINT wParam, LONG lParam)
{
	BOOL bStatus;
	int	i, nPIDField[4], nInError = 0;

	BOOL bPrintErrors = (BOOL) wParam;				// Cast wParam as a flag
	PID_VALUE* lpIPAddr = (PID_VALUE*) lParam;		// Cast lParam as an PID_VALUE structure
	if (NULL == lpIPAddr)							// If it's a bad pointer, return an error
		return FALSE;
	memset(lpIPAddr, 0, sizeof(PID_VALUE));			// Zero out the returned data

	// Parse the fields and return an error indication if something bad was detected
	for (i = 0; i < 4; i++)
	{
		bStatus = ParsePIDPart(i, *m_PIDEdits[i], nPIDField[i]);
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
			if (nPIDField[i] < 0)
				szText = ::LoadLocString(m_pLocWnd, IDS_IPE_MISSINGVALUE);
			else {
				szFormat = ::LoadLocString(m_pLocWnd, IDS_WAS_ERR_OUTOFRANGE_D);
				szText.Format(szFormat, nPIDField[i], 0, gMAXTable[i]);
			}

			InfalidMessInfo *pInfo = new InfalidMessInfo;
			pInfo->m_pChild = m_PIDEdits[i];
			pInfo->m_Text = szText;
			GetParent()->PostMessage( mChildInvalid, WPARAM(GetSafeHwnd()), LPARAM(pInfo) );
		}
		return FALSE;								// Return an error
	}

	lpIPAddr->nPIDField1 = nPIDField[0];					// Return the pieces to the caller
	lpIPAddr->nPIDField2 = nPIDField[1];
	lpIPAddr->nPIDField3 = nPIDField[2];
	lpIPAddr->nPIDField4 = nPIDField[3];
	lpIPAddr->nInError = 0;							// No error to report
	return TRUE;									// Return success
}

LONG CControlTreePIDCtl::OnSetPIDValue(UINT wParam, LONG lParam)
{
	CString		szText;

	PID_VALUE* lpIPAddr = (PID_VALUE*) lParam;		// Cast lParam as an PID_VALUE structure

	m_PIDEdits[0]->SetPosition( lpIPAddr->nPIDField1 );
	m_PIDEdits[1]->SetPosition( lpIPAddr->nPIDField2 );
	m_PIDEdits[2]->SetPosition( lpIPAddr->nPIDField3 );
	m_PIDEdits[3]->SetPosition( lpIPAddr->nPIDField4 );
/*
	// Format their data and load the edit controls
	szText.Format(_T("%u"), lpIPAddr->nPIDField1);
	m_PIDEdits[0].SetWindowText(szText);
	szText.Format(_T("%u"), lpIPAddr->nPIDField2);
	m_PIDEdits[1].SetWindowText(szText);
	szText.Format(_T("%u"), lpIPAddr->nPIDField3);
	m_PIDEdits[2].SetWindowText(szText);
	szText.Format(_T("%u"), lpIPAddr->nPIDField4);
	m_PIDEdits[3].SetWindowText(szText);
*/
	return TRUE;
}

LONG CControlTreePIDCtl::OnSetReadOnly(UINT wParam, LONG lParam)
{
	m_bReadOnly = (BOOL) wParam;

	for (int ii = 0; ii < 4; ii++)
		m_PIDEdits[ii]->SetReadOnly(m_bReadOnly);

	Invalidate();
	return TRUE;
}

BOOL CControlTreePIDCtl::ParsePIDPart(int nIndex, CControlTreePIDEdit& edit, int& n)
{
	CString		szText;

	edit.GetWindowText(szText);						// Get text from edit control
	if (szText.IsEmpty())							// Empty text is an error
	{
		n = -1;										// Return bogus value
		return FALSE;								// Return parse failure to caller
	}

	n = edit.GetPosition();								// Grab a decimal value from edit text
	if (n < 0 || n > gMAXTable[nIndex])							// If it is out of range, return an error
		return FALSE;

	return TRUE;									// Looks acceptable, return success
}

CControlTreePIDEdit* CControlTreePIDCtl::GetEditControl(int nIndex)
{
	if (nIndex < 1 || nIndex > 4)
		return NULL;
	return m_PIDEdits[nIndex - 1];
}

LONG CControlTreePIDCtl::OnChildInvalid(UINT wParam, LONG lParam) {
	CString szTitle = ::LoadLocString(m_pLocWnd, IDS_ERRTITLE);
	GetParent()->BeginModalState();
	::MessageBox(NULL, ((InfalidMessInfo *)lParam)->m_Text, szTitle, MB_OK | MB_ICONEXCLAMATION | MB_TASKMODAL );
	//GetTopLevelParent()->MessageBox(((InfalidMessInfo *)lParam)->m_Text, szTitle, MB_OK | MB_ICONEXCLAMATION | MB_TASKMODAL );
	GetParent()->EndModalState();

	((InfalidMessInfo *)lParam)->m_pChild->SetFocus();						// Set focus to offending byte
	((InfalidMessInfo *)lParam)->m_pChild->SetSel(0, -1);					// Select entire contents

	delete (InfalidMessInfo *)lParam;

	return 0;
}


/////////////////////////////////////////////////////////////////////////////
// CControlTreePIDEdit

CControlTreePIDEdit::CControlTreePIDEdit() :
	m_nPosition( 0 )
{
}

CControlTreePIDEdit::~CControlTreePIDEdit()
{
}

void CControlTreePIDEdit::SetParent(CControlTreePIDCtl* pParent)
{
	m_pParent = pParent;							// Save pointer to parent control
}


// ---
int CControlTreePIDEdit::GetPosition() {
//	return m_nPosition;
	CString szText;
	GetWindowText( szText );	
	return _ttoi(szText);				
}


// ---
void CControlTreePIDEdit::SetPosition( int nPos ) {
	m_nPosition = nPos;
	TCHAR pBuff[20];
	_stprintf( pBuff, _T("%d"), nPos );
	SetWindowText( pBuff );
}

// ---
void CControlTreePIDEdit::StepPosition( int nStep ) {
	UINT nIDC = GetDlgCtrlID();
	int nIndex = nIDC - IDC_PIDEDIT1;

	int nPos = GetPosition();
	nPos += nStep;
	if ( nPos >= 0 && nPos <= gMAXTable[nIndex] ) 
		SetPosition( nPos );
}

BEGIN_MESSAGE_MAP(CControlTreePIDEdit, CEdit)
	//{{AFX_MSG_MAP(CControlTreePIDEdit)
	ON_WM_CHAR()
	ON_WM_KEYDOWN()
	ON_WM_KILLFOCUS()
	ON_WM_SETFOCUS()
	ON_REGISTERED_MESSAGE(mSpinStep, OnSpinStep)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CControlTreePIDEdit message handlers

void CControlTreePIDEdit::OnChar(UINT nChar, UINT nRepCnt, UINT nFlags) 
{
	// Logic for this function:
	// Tab and dot are forwarded to the parent.  Tab (or shift-tab) operate
	// just like expected (focus moves to next control after the parent).
	// Dot or space causes the parent to set focus to the next child edit (if
	// focus is currently set to one of the first three edit controls).
	// Numerics (0..9) and control characters are forwarded to the standard
	// CEdit OnChar method; all other characters are dropped.
	if (VK_TAB == nChar ||
			L',' == nChar ||
			L' ' == nChar ||
			VK_RETURN == nChar ||
			VK_ESCAPE == nChar)
		m_pParent->OnChildChar(nChar, nRepCnt, nFlags, *this);
	else if ( _istdigit(nChar) || _istcntrl(nChar))
	{
		CEdit::OnChar(nChar, nRepCnt, nFlags);

		// Automatically advance to next child control if 5 characters were entered;
		// use "-" to indicate field was full to OnChildChar
		if (5 == GetWindowTextLength())
			m_pParent->OnChildChar(L'-', 0, nFlags, *this);
	}
	else
		::MessageBeep(0xFFFFFFFF);
}

void CControlTreePIDEdit::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags) 
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
void CControlTreePIDEdit::OnKillFocus( CWnd* pNewWnd ) {
	CEdit::OnKillFocus(pNewWnd);
  
	GetParent()->SendMessage( mEditKillFocus, 0, LPARAM(pNewWnd->GetSafeHwnd()) );
}


// ---
void CControlTreePIDEdit::OnSetFocus(CWnd* pOldWnd) {
	CEdit::OnSetFocus(pOldWnd);
	
	CWnd *pParent = GetParent();

	if ( ((CControlTreePIDCtl *)pParent)->m_pSpinForEdit ) {
		UINT nIDC = GetDlgCtrlID();
		int nIndex = nIDC - IDC_PIDEDIT1;

		int nPos = GetPosition();

		((CControlTreePIDCtl *)pParent)->m_pSpinForEdit->SetBuddy( this );
		((CControlTreePIDCtl *)pParent)->m_pSpinForEdit->SetRange32( 0, gMAXTable[nIndex] );
		((CControlTreePIDCtl *)pParent)->m_pSpinForEdit->SetPos( nPos );
	}

}

// ---
LONG CControlTreePIDEdit::OnSpinStep(UINT wParam, LONG lParam) {
	StepPosition( (int)wParam );
	return 0;
}


/////////////////////////////////////////////////////////////////////////////
// CControlTreePIDTypeEdit
// ---
CControlTreePIDTypeEdit::CControlTreePIDTypeEdit() {
}

// ---
CControlTreePIDTypeEdit::~CControlTreePIDTypeEdit() {
}

// ---
int CControlTreePIDTypeEdit::GetPosition() {
//	return m_nPosition;
	CString szText;
	GetWindowText( szText );	
	return ::TypeName2TypeID( szText );
}


// ---
void CControlTreePIDTypeEdit::SetPosition( int nPos ) {
	UINT nIDC = GetDlgCtrlID();
	int nIndex = nIDC - IDC_PIDEDIT1;

	if ( nPos >= int(avpt_nothing) && nPos <= int(avpt_bin) ) {
		m_nPosition = nPos;
		TCHAR *pTypeName = ::TypeID2TypeName( m_nPosition );
		while( !pTypeName && (m_nPosition >= int(avpt_nothing) && m_nPosition <= int(avpt_bin))) {
			m_nPosition++;
			pTypeName = ::TypeID2TypeName( m_nPosition );
		}
		SetWindowText( pTypeName );
	}
}

// ---
void CControlTreePIDTypeEdit::StepPosition( int nStep ) {
	UINT nIDC = GetDlgCtrlID();
	int nIndex = nIDC - IDC_PIDEDIT1;

	if ( (m_nPosition + nStep) >= int(avpt_nothing) && (m_nPosition + nStep) <= int(avpt_bin) ) {
		m_nPosition += nStep;
		TCHAR *pTypeName = ::TypeID2TypeName( m_nPosition );
		while( !pTypeName && (m_nPosition >= int(avpt_nothing) && m_nPosition <= int(avpt_bin))) {
			m_nPosition += nStep;
			pTypeName = ::TypeID2TypeName( m_nPosition );
		}
		SetWindowText( pTypeName );
	}
}


BEGIN_MESSAGE_MAP(CControlTreePIDTypeEdit, CControlTreePIDEdit)
	//{{AFX_MSG_MAP(CControlTreePIDTypeEdit)
	ON_WM_CHAR()
	ON_WM_KEYDOWN()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CControlTreePIDTypeEdit message handlers
// ---
void CControlTreePIDTypeEdit::OnChar(UINT nChar, UINT nRepCnt, UINT nFlags) {
	if (VK_TAB == nChar ||
			L',' == nChar ||
			L' ' == nChar ||
			VK_RETURN == nChar ||
			VK_ESCAPE == nChar)
		CControlTreePIDEdit::OnChar(nChar, nRepCnt, nFlags);
	else 
		::MessageBeep(0xFFFFFFFF);
}

// ---
void CControlTreePIDTypeEdit::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags) {
	// Handle the left and right arrow keys.  If the left arrow key is pressed
	// with the caret at the left of the input text, shift focus to the previous
	// control (if in edit controls 2-4).  Likewise for the right arrow key.
	// This is done by calling the parent's OnChildChar method.
	// If not left or right arrow, or not at beginning or end, call default
	// OnKeyDown processor so key down gets passed to edit control.
/*
	if (VK_LEFT == nChar || VK_RIGHT == nChar)
	{
		CPoint ptCaret = GetCaretPos();
		int nCharPos = LOWORD(CharFromPos(ptCaret));
		if ((VK_LEFT == nChar && nCharPos == 0) ||
			(VK_RIGHT == nChar && nCharPos == GetWindowTextLength()))
			m_pParent->OnChildChar(nChar, nRepCnt, nFlags, *this);
	}
*/
	CControlTreePIDEdit::OnKeyDown(nChar, nRepCnt, nFlags);
}

/////////////////////////////////////////////////////////////////////////////
// CControlTreePIDSpinButton window
// ---
CControlTreePIDSpinButton::CControlTreePIDSpinButton() {
}

// ---
void CControlTreePIDSpinButton::SetParent( CControlTreePIDCtl* pParent ) {
	m_pParent = pParent;							// Save pointer to parent control
}

BEGIN_MESSAGE_MAP(CControlTreePIDSpinButton, CSpinButtonCtrl)
	//{{AFX_MSG_MAP(CControlTreePIDSpinButton)
	ON_NOTIFY_REFLECT(UDN_DELTAPOS, OnDeltaPos)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

// ---
void CControlTreePIDSpinButton::OnDeltaPos( NMHDR* pNMHDR, LRESULT* pResult ) {
	NM_UPDOWN* pNMUpDown = (NM_UPDOWN*)pNMHDR;

	::SendMessage( ::GetFocus(), mSpinStep, pNMUpDown->iDelta, 0 );

	*pResult = 0;
}

