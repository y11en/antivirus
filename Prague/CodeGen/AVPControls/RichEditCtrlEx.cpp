////////////////////////////////////////////////////////////////////
//
//  RichEditCtrlEx.cpp
//  RichEditCtrlEx implementation file
//  AVP general purpose stuff
//  Victor Matiouchenkov [victor@avp.ru], Kaspersky Labs. 1999
//  Copyright (c) Kaspersky Labs
//
////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include <tchar.h>
#include "RichEditCtrlEx.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

_AFX_RICHEDITEX_STATE _afxRichEditStateEx;

// ---
_AFX_RICHEDITEX_STATE::_AFX_RICHEDITEX_STATE() {
	m_hInstRichEdit20 = NULL;
}

// ---
_AFX_RICHEDITEX_STATE::~_AFX_RICHEDITEX_STATE() {
	if( m_hInstRichEdit20 != NULL )	{
		::FreeLibrary( m_hInstRichEdit20 );
	}
}

// ---
_AFX_RICHEDITEX_STATE &_AFX_RICHEDITEX_STATE::GetState() {
	return _afxRichEditStateEx;
}

// ---
BOOL PASCAL AfxInitRichEditEx() {
	if( !::AfxInitRichEdit() ) {
		return FALSE;
	}

	_AFX_RICHEDITEX_STATE* l_pState = &_afxRichEditStateEx;

	if( l_pState->m_hInstRichEdit20 == NULL )	{
		l_pState->m_hInstRichEdit20 = LoadLibraryA( "RICHED20.DLL" ) ;
	}

	return l_pState->m_hInstRichEdit20 != NULL ;
}



/////////////////////////////////////////////////////////////////////////////
// CRichEditCtrlEx

CRichEditCtrlEx::CRichEditCtrlEx()
{
}

CRichEditCtrlEx::~CRichEditCtrlEx()
{
}

// ---
BOOL CRichEditCtrlEx::AutoURLDetect( BOOL bEnable ) {
  ASSERT(::IsWindow( m_hWnd ) ); 
	return ( BOOL )::SendMessage( m_hWnd, EM_AUTOURLDETECT, bEnable, 0 );
}

BEGIN_MESSAGE_MAP(CRichEditCtrlEx, CRichEditCtrl)
	//{{AFX_MSG_MAP(CRichEditCtrlEx)
		// NOTE - the ClassWizard will add and remove mapping macros here.
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CRichEditCtrlEx message handlers

BOOL CRichEditCtrlEx::Create( DWORD	dwStyle, const RECT& rcRect, CWnd* pParentWnd, UINT	nID ) {
	if ( _AFX_RICHEDITEX_STATE::GetState().Is20Activated() )
		return CWnd::Create( _T("RichEdit20A"), NULL, dwStyle, rcRect, pParentWnd, nID );
	else
		return CWnd::Create( _T("RICHEDIT"), NULL, dwStyle, rcRect, pParentWnd, nID );
}
