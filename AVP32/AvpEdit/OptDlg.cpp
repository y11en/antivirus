// OptDlg.cpp : implementation file
//

#include "stdafx.h"
#include "AvpEdit.h"
#include "OptDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// COptDlg dialog


COptDlg::COptDlg(CWnd* pParent /*=NULL*/)
	: CDialog(COptDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(COptDlg)
	m_LinkConfirm = FALSE;
	m_EscSaveRec = FALSE;
	m_SetFile = _T("");
	m_ShowSourceFile = FALSE;
	m_SepLine = _T("");
	m_SaveBackup = FALSE;
	m_SkipPag = FALSE;
	m_FontName = _T("");
	m_FontSize = 0;
	m_MakeFullName = TRUE;
#if (defined (KL_MULTIPLATFORM_LINKS))
	m_LocalCVSRoot = _T("");
#endif
	//}}AFX_DATA_INIT
}


void COptDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(COptDlg)
	DDX_Check(pDX, IDC_LINKCONF, m_LinkConfirm);
	DDX_Check(pDX, IDC_SAVEREC, m_EscSaveRec);
	DDX_Text(pDX, IDC_SETFILE, m_SetFile);
	DDV_MaxChars(pDX, m_SetFile, 255);
	DDX_Check(pDX, IDC_SHOWSOURCE, m_ShowSourceFile);
	DDX_Text(pDX, IDC_SEPLINE, m_SepLine);
	DDV_MaxChars(pDX, m_SepLine, 50);
	DDX_Check(pDX, IDC_SAVEBACKUP, m_SaveBackup);
	DDX_Check(pDX, IDC_SKIP_PAG, m_SkipPag);
	DDX_Text(pDX, IDC_FONT_NAME, m_FontName);
	DDX_Text(pDX, IDC_FONT_SIZE, m_FontSize);
	DDX_Check(pDX, IDC_MAKE_FULL_NAME, m_MakeFullName);
#if (defined (KL_MULTIPLATFORM_LINKS))
	DDX_Text(pDX, IDC_LOCAL_CVSROOT, m_LocalCVSRoot);
	DDV_MaxChars(pDX, m_LocalCVSRoot, 256);
#endif
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(COptDlg, CDialog)
	//{{AFX_MSG_MAP(COptDlg)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// COptDlg message handlers

