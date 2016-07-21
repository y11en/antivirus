// ControlListEdit.cpp : implementation file
//

#include "stdafx.h"
#include <tchar.h>
#include "ControlTreeEdit.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


/////////////////////////////////////////////////////////////////////////////
// CControlTreeEdit
// ---
CControlTreeEdit::CControlTreeEdit() :
	CEdit() {
}


BEGIN_MESSAGE_MAP(CControlTreeEdit, CEdit)
	//{{AFX_MSG_MAP(CControlTreeEdit)
	ON_WM_NCDESTROY()
	ON_MESSAGE(WM_PASTE,OnPaste)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CControlTreeEdit message handlers
// ---
LRESULT CControlTreeEdit::OnPaste( WPARAM wParam, LPARAM lParam ) {
	if ( OpenClipboard() )  {
		HANDLE hMem = ::GetClipboardData( CF_UNICODETEXT );
		
		if ( hMem ) {
			void *pMem  = ::GlobalLock( hMem );
			DWORD dwLength = ::GlobalSize( hMem );
			
			if ( ::IsTextUnicode(pMem, dwLength, NULL) ) {
				int nSize = ::WideCharToMultiByte( CP_ACP, 0/*MB_COMPOSITE*/, (wchar_t *)pMem, -1, NULL, 0, NULL, NULL );
				HANDLE hMBMem = ::GlobalAlloc( GMEM_MOVEABLE | GMEM_DDESHARE, nSize );
				char *pMStr = (char *)::GlobalLock( hMBMem );
				::WideCharToMultiByte( CP_ACP, 0/*MB_COMPOSITE*/, (wchar_t *)pMem, -1, pMStr, nSize, NULL, NULL );
				::SetClipboardData( CF_TEXT, hMBMem );
				::GlobalUnlock( hMBMem );
			}
			
			::GlobalUnlock( hMem );
		}
		
		CloseClipboard();
	}
	return Default();
}

// ---
void CControlTreeEdit::OnNcDestroy() {
	CWnd::OnNcDestroy();
	
	// Make sure the window was destroyed
	ASSERT(NULL == m_hWnd);
	
	// Destroy this object since it won't be destroyed otherwise
	delete this;
}

