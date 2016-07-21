////////////////////////////////////////////////////////////////////
//
//  ACComboBox.h
//  The CComboBox derived class for automatically completion of text entered by user
//  AVP general purposes stuff
//  Victor Matiouchenkov [victor@avp.ru], Kaspersky Labs. 1999
//  Copyright (c) Kaspersky Labs
//
////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "ACComboBox.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

WNDPROC	CACComboBox::m_pOriginalEditProc = NULL;
BOOLEAN CACComboBox::m_bDisableAutoComplete = FALSE;

/////////////////////////////////////////////////////////////////////////////
// CACComboBox
// ---
CACComboBox::CACComboBox() {
}

// ---
CACComboBox::~CACComboBox() {
}

// ---
LRESULT APIENTRY CACComboBox::AutoCompleteProc( HWND hWndEdit, UINT uMsg, WPARAM wParam, LPARAM lParam ) {
	// If the user presses the backspace or delete key, disable the autocompletion
	// until another valid character is entered.  Then pass this and any other message
	// on to the original window procedure.
	switch (uMsg) {
		case WM_CHAR:
			if (wParam == VK_BACK) {
				m_bDisableAutoComplete = TRUE;
			}
			else {
				m_bDisableAutoComplete = FALSE;
			}
			break;
		case WM_KEYDOWN:
			if (wParam == VK_DELETE) {
				m_bDisableAutoComplete = TRUE;;
			}
			break;
	}
	
	return CallWindowProc(m_pOriginalEditProc, hWndEdit, uMsg, wParam, lParam);
}

BEGIN_MESSAGE_MAP(CACComboBox, CComboBox)
	//{{AFX_MSG_MAP(CACComboBox)
	ON_WM_CREATE()
	ON_CONTROL_REFLECT(CBN_EDITCHANGE, OnEditChange)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CACComboBox message handlers
// ---
int CACComboBox::OnCreate( LPCREATESTRUCT lpCreateStruct ) {
	if (CComboBox::OnCreate(lpCreateStruct) == -1)
		return -1;
	
	HWND hWndEdit = ::GetDlgItem( *this, 1001 );
	if (hWndEdit != NULL) {
		m_pOriginalEditProc = (WNDPROC)::GetWindowLong(hWndEdit, GWL_WNDPROC);
		if (m_pOriginalEditProc != NULL) {
			::SetWindowLong(hWndEdit,GWL_WNDPROC, (LONG)AutoCompleteProc);
		}
	}

	return 0;
}

// ---
void CACComboBox::OnEditChange() {
	DWORD		nTextLen = 0;
	DWORD		nMatchedTextLen = 0;
	_TCHAR	*pStrMatchedText = NULL;
	_TCHAR	*pEnteredText = NULL;
	DWORD		dwStartSel = 0; 
	DWORD		dwEndSel = 0;
	
	// Get the text length from the combobox, then copy it into a newly allocated buffer.
	nTextLen = ::SendMessage(*this, WM_GETTEXTLENGTH, NULL, NULL);
	pEnteredText = new _TCHAR[nTextLen + 1];
	::SendMessage(*this, WM_GETTEXT, (WPARAM)nTextLen + 1, (LPARAM)pEnteredText);
	::SendMessage(*this, CB_GETEDITSEL, (WPARAM)&dwStartSel, (LPARAM)&dwEndSel);

	// Check to make sure autocompletion isn't disabled due to a backspace or delete
	// (see AutoCompleteComboBoxProc). Also, the user must be typing at the end of the string,
	// not somewhere in the middle.
	if ( !m_bDisableAutoComplete && (dwStartSel == dwEndSel) && (dwStartSel == nTextLen) ) {
		// Try and find a string that matches the typed substring.  If one is found,
		// set the text of the combobox to that string and set the selection to mask off
		// the end of the matched string.
		int nMatch = ::SendMessage ( *this, CB_FINDSTRING, (WPARAM)-1, (LPARAM)pEnteredText );
		if (nMatch != CB_ERR) {
			nMatchedTextLen = ::SendMessage( *this, CB_GETLBTEXTLEN, (WPARAM)nMatch, 0 );
			if (nMatchedTextLen != CB_ERR) {
				// Since the user may be typing in the same string, but with different case (e.g. "/port --> /PORT")
				// we copy whatever the user has already typed into the beginning of the matched string,
				// then copy the whole shebang into the combobox.  We then set the selection to mask off
				// the inferred portion.
				pStrMatchedText = new _TCHAR[nMatchedTextLen + 1];
				::SendMessage( *this, CB_GETLBTEXT, (WPARAM)nMatch, (LPARAM)pStrMatchedText );				
				memcpy((void*)pStrMatchedText, (void*)pEnteredText, nTextLen * sizeof(_TCHAR) );
				::SendMessage( *this, WM_SETTEXT, 0, (WPARAM)pStrMatchedText );
				::SendMessage( *this, CB_SETEDITSEL, 0, MAKELPARAM(nTextLen, -1) );
				delete [] pStrMatchedText;			
			}
		}
	}

	delete [] pEnteredText;
}
