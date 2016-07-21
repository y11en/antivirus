// LargeCommentDlg.cpp : implementation file
//

#include "stdafx.h"
#include <Stuff\Layout.h>
#include <Stuff\RegStorage.h>

#include "VisualEditorDlg.h"
#include "LargeCommentDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CLargeCommentDlg dialog


CLargeCommentDlg::CLargeCommentDlg(CString &rcText, DWORD dwTitleID, BOOL bReadOnly, CWnd* pParent /*=NULL*/)
	: CDialog(CLargeCommentDlg::IDD, pParent),
	m_rcSourceText(rcText),
	m_dwTitleID( dwTitleID )
{
	//{{AFX_DATA_INIT(CLargeCommentDlg)
	m_Comment = m_rcSourceText;
	m_bReadOnly = bReadOnly;
	//}}AFX_DATA_INIT
}

// ---
void CLargeCommentDlg::DoDataExchange( CDataExchange* pDX ) {
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CLargeCommentDlg)
	DDX_Control(pDX, IDC_LARGECOMMENT_EDIT, m_CommentEdit);
	DDX_Text(pDX, IDC_LARGECOMMENT_EDIT, m_Comment);
	//}}AFX_DATA_MAP
}

// ---
BOOL CLargeCommentDlg::DestroyWindow() {
	WINDOWPLACEMENT rcPlacement;
	rcPlacement.length = sizeof(rcPlacement);

	GetWindowPlacement( &rcPlacement );
	CRegStorage rcRegStorage( CFG_REG_KEY );
	rcRegStorage.PutValue( LCPLACEMENT_REG_VALUE, (BYTE *)&rcPlacement, sizeof(rcPlacement) );

	return CDialog::DestroyWindow();
}

BEGIN_MESSAGE_MAP(CLargeCommentDlg, CDialog)
	//{{AFX_MSG_MAP(CLargeCommentDlg)
	ON_WM_SIZE()
	ON_BN_CLICKED(IDC_LARGECOMMENT_FONT_BUTTON, OnFontButton)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CLargeCommentDlg message handlers
// ---
BOOL CLargeCommentDlg::OnInitDialog() {
	CDialog::OnInitDialog();
	
	CString rcTitle;
	rcTitle.LoadString( m_dwTitleID );
	SetWindowText( rcTitle );

	m_CommentEdit.SetReadOnly( m_bReadOnly );

	CRegStorage rcRegStorage( CFG_REG_KEY );

	WINDOWPLACEMENT rcPlacement;
	rcPlacement.length = sizeof(rcPlacement);

	DWORD dwSize = sizeof(rcPlacement);
	if ( rcRegStorage.GetValue(LCPLACEMENT_REG_VALUE, (BYTE *)&rcPlacement, dwSize) )
		SetWindowPlacement( &rcPlacement );

	LOGFONT rcLogFont;
	dwSize = sizeof(rcLogFont);
	if ( rcRegStorage.GetValue(LCFONT_REG_VALUE, (BYTE *)&rcLogFont, dwSize) ) {
		m_EditFont.CreateFontIndirect( &rcLogFont );
		m_CommentEdit.SetFont( &m_EditFont );
	}

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}
// ---
void CLargeCommentDlg::OnOK() {
	UpdateData();
	
	m_rcSourceText = m_Comment;

	CDialog::OnOK();
}

#define IDC_EDIT IDC_LARGECOMMENT_EDIT 
#define IDC_FONT IDC_LARGECOMMENT_FONT_BUTTON 

// ---
void CLargeCommentDlg::OnSize( UINT nType, int cx, int cy ) {
	CDialog::OnSize(nType, cx, cy);
	
  static RULE rules[] = {
    // Action    Act-on                   Relative-to           Offset
    // ------    ------                   -----------           ------
    {  lMOVE,    lRIGHT (IDC_FONT),						lRIGHT(lPARENT)	  					,-5  },
    {  lMOVE,    lBOTTOM(IDC_FONT),						lBOTTOM(lPARENT)						,-5  },

    {  lMOVE,    lRIGHT (IDCANCEL),						lLEFT(IDC_FONT)						  ,-5  },
    {  lMOVE,    lBOTTOM(IDCANCEL),						lBOTTOM(IDC_FONT)						,-0  },

    {  lMOVE,    lRIGHT (IDOK),								lLEFT(IDCANCEL)							,-5  },
    {  lMOVE,    lBOTTOM(IDOK),								lBOTTOM(IDCANCEL)						,-0  },

    {  lSTRETCH, lRIGHT(IDC_EDIT),						lRIGHT(lPARENT)							,-5  },
    {  lSTRETCH, lBOTTOM(IDC_EDIT),						lTOP(IDOK)									,-5  },

    {  lEND																																		 }
 };

 Layout_ComputeLayout( GetSafeHwnd(), rules );
}

// ---
void CLargeCommentDlg::OnFontButton() {
	CFont *rcSelfFont = m_CommentEdit.GetFont();

	LOGFONT rcLogFont;
	rcSelfFont->GetLogFont( &rcLogFont );

	CFontDialog rcDlg( &rcLogFont, /*CF_EFFECTS | */CF_SCREENFONTS, NULL, this );

	if ( rcDlg.DoModal() == IDOK ) {
		m_EditFont.Detach();
		m_EditFont.CreateFontIndirect( &rcLogFont );
		m_CommentEdit.SetFont( &m_EditFont );
		
		CRegStorage rcRegStorage( CFG_REG_KEY );

		DWORD dwSize = sizeof(rcLogFont);
		rcRegStorage.PutValue( LCFONT_REG_VALUE, (BYTE *)&rcLogFont, dwSize );
	}

	m_CommentEdit.SetFocus();
}
