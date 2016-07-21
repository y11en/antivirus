//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//

#include "stdafx.h"
#include "RegEdit.h"
#include "RegEditDoc.h"

//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//
// CRegEditDoc

IMPLEMENT_DYNCREATE(CRegEditDoc, CDocument)

BEGIN_MESSAGE_MAP(CRegEditDoc, CDocument)
	//{{AFX_MSG_MAP(CRegEditDoc)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//
CRegEditDoc::CRegEditDoc()
{
	m_pReg = NULL;
}

CRegEditDoc::~CRegEditDoc()
{
}

//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//
BOOL CRegEditDoc::OnNewDocument()
{
	if (!CDocument::OnNewDocument())
		return FALSE;

	return TRUE;
}

//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//
BOOL CRegEditDoc::OnOpenDocument(LPCTSTR lpszPathName) 
{
	m_strRegKey = lpszPathName;
	m_pReg = GlobalGetApp()->OpenRegistry(m_strRegKey, m_bWinReg, m_bReadOnly);
	//return m_pReg ? TRUE : FALSE;
	SetTitle( lpszPathName );

	if(NULL == m_pReg)
		m_strRegKey.Empty();
	return TRUE;
}



// ---
void CRegEditDoc::SetTitle( LPCTSTR title ) {
	if ( m_bWinReg )
		m_strTitle = "reg(";
	else
		m_strTitle = "file(";
	m_strTitle += title;
	m_strTitle += ")";
}


//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//
void CRegEditDoc::SetPathName(LPCTSTR lpszPathName, BOOL bAddToMRU) 
{
	if (lstrlen(lpszPathName) && bAddToMRU && FALSE == m_bWinReg)
		AfxGetApp()->AddToRecentFileList(lpszPathName);
}

//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//
void CRegEditDoc::Serialize(CArchive& ar)
{
	if (ar.IsStoring())
	{
	}
	else
	{
	}
}


void CRegEditDoc::OnNewKey() {
}



BOOL CRegEditDoc::SaveModified() 
{
	if (!IsModified() || m_bWinReg)
		return TRUE;        // ok to continue

	// get name/title of document
	CString prompt;
	AfxFormatString1(prompt, AFX_IDP_ASK_TO_SAVE, m_strRegKey);
	switch (AfxMessageBox(prompt, MB_YESNOCANCEL, AFX_IDP_ASK_TO_SAVE))
	{
	case IDCANCEL:
		return FALSE;       // don't continue

	case IDYES:
		// If so, either Save or Update, as appropriate
		if (!SetSaveOnCloseProp())
			return FALSE;       // don't continue
		break;

	case IDNO:
		// If not saving changes, revert the document
		break;

	default:
		ASSERT(FALSE);
		break;
	}
	return TRUE;    // keep going
}

//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//
BOOL CRegEditDoc::SetSaveOnCloseProp()
{
	tERROR	nErr = m_pReg->set_pgSAVE_RESULTS_ON_CLOSE( cTRUE );  
	if(PR_FAIL(nErr))
	{
		GlobalGetApp()->ReportPragueError(_T("Cannot set pgSAVE_RESULTS_ON_CLOSE property value."), nErr);
		return FALSE;
	}
	return TRUE;
}

//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//
// CRegEditDoc commands


//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//
// CRegEditDoc diagnostics

#ifdef _DEBUG
void CRegEditDoc::AssertValid() const
{
	CDocument::AssertValid();
}

void CRegEditDoc::Dump(CDumpContext& dc) const
{
	CDocument::Dump(dc);
}
#endif //_DEBUG

//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//


