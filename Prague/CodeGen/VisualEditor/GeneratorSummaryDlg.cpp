// GeneratorSummary.cpp : implementation file
//

#include "stdafx.h"
#include <Stuff\Layout.h>
#include <datatreeutils/dtutils.h>
#include <StuffIO\IOUtil.h>
#include <serialize/kldtser.h>
#include "VisualEditor.h"
#include "VisualEditorDlg.h"
#include "CodeGenWizard.h"
#include "GeneratorSummaryDlg.h"
#include "CodeGenTools.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

// ---
static char *g_RegValueNameTable[][3] = {
	{ "RecentSrcOptSession", "RecentSrcOptFile",  "RecentSrcAutoSaveOpt" },
	{ "RecentHdrOptSession", "RecentHdrOptFile",  "RecentSrcAutoSaveOpt" },
};

/////////////////////////////////////////////////////////////////////////////
// CGeneratorSummary dialog

CGeneratorSummary::CGeneratorSummary(
	HDATA hImpData,
	HDATA &hOptionsData, HDATA &hOptionsPattern, CString &rcOptFileName, 
	GUID& quidClsId, CWnd &rcFrame, int nRegime, CWnd* pParent /*=NULL*/)
	: CDialog(CGeneratorSummary::IDD, pParent),
	m_hImpData(hImpData),
	m_rcFrame( rcFrame ),
	m_hOptionsData( hOptionsData ),
	m_hOptionsPattern( hOptionsPattern ),
	m_rcOptFileName( rcOptFileName ),
	m_RegStorage( CFG_REG_KEY ),
	m_quidClassId(quidClsId),
	m_nRegime( nRegime ),
	m_bAutoSave( TRUE ),
	m_bSaved( FALSE )
{
	//{{AFX_DATA_INIT(CGeneratorSummary)
	m_FileName = _T("");
	//}}AFX_DATA_INIT
}


void CGeneratorSummary::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CGeneratorSummary)
	DDX_Control(pDX, IDC_CGS_EDITVIEW, m_EditView);
	DDX_Text(pDX, IDC_CGS_EDITFILENAME, m_FileName);
	//}}AFX_DATA_MAP
}

// ---
BOOL CGeneratorSummary::DestroyWindow() {
	return CDialog::DestroyWindow();
}


// ---
void  CGeneratorSummary::SetState( int s ) {
	CFAutoParam::SetState( s );
	switch ( m_State ) {
		case ps_nrm :
			if ( !GetSafeHwnd() ) {
				Create( CGeneratorSummary::IDD, m_pParentWnd );

				CRect rcFrameRect;
				m_rcFrame.GetWindowRect( &rcFrameRect );
				::MapWindowPoints( NULL, *this, (POINT *)&rcFrameRect, 2 );

				SetWindowPos( NULL, rcFrameRect.left, rcFrameRect.top, 
														rcFrameRect.Width(), rcFrameRect.Height(), SWP_NOACTIVATE | SWP_NOZORDER );
				ShowWindow( SW_SHOW );
			}
			break;
		case ps_grd :
		case ps_und :
			if ( GetSafeHwnd() ) {
				DestroyWindow();
			}
			break;
	}
}

// ---
void  CGeneratorSummary::PerformAutoSave() {
	if ( m_bAutoSave && !m_bSaved )
		OnSaveOptions();
}

void	CGeneratorSummary::SetOptionsText(){

	ASSERT(NULL != m_hOptionsData);
	if (!m_hOptionsData ) 
		return;

	CString rText;

	if(!m_nRegime)
	{
		CString strPluginName = CGT::GetPluginName(m_hImpData);
		rText.Format("Plugin - %s\r\n\r\n", strPluginName);
	}

	HDATA hResultData = NULL;
	if ( ::DTUT_ConvertKnownObjectRulesTreeToStringsTree
				( m_hOptionsData, m_hOptionsPattern, &hResultData, NULL, FALSE ) ) {

		HPROP hResultProp	= ::DATA_Find_Prop( hResultData, 0, 0 );
		AVP_dword nCount = ::PROP_Arr_Count( hResultProp );
		for ( AVP_dword i=0; i<nCount; i++ ) {
			CAPointer<char> pStr = ::GetPropArrayValueAsString( hResultProp, i, NULL );
			rText += pStr;
			rText += "\r\n";
		}
		rText += "\r\n";
						
		::DATA_Remove( hResultData, NULL );
	}
				
	rText += "\r\n";
	m_EditView.SetWindowText( rText );

	UpdateData( FALSE );
}


BEGIN_MESSAGE_MAP(CGeneratorSummary, CDialog)
	//{{AFX_MSG_MAP(CGeneratorSummary)
	ON_WM_SIZE()
	ON_BN_CLICKED(IDC_CGS_SAVEBUTTON, OnSaveOptions)
	ON_BN_CLICKED(IDC_CGS_AUTOSAVE, OnAutoSave)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CGeneratorSummary message handlers

BOOL CGeneratorSummary::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	SetOptionsText();

	m_FileName = CGT::s_OptFile.GetPathname();
	UpdateData( FALSE );

	m_RegStorage.GetValue( g_RegValueNameTable[m_nRegime][2], (DWORD &)m_bAutoSave );
	CheckDlgButton( IDC_CGS_AUTOSAVE, m_bAutoSave );

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

// ---
void CGeneratorSummary::OnSize(UINT nType, int cx, int cy) {

	CDialog::OnSize(nType, cx, cy);
	
  static RULE rules[] = {
    // Action    Act-on                   Relative-to           Offset
    // ------    ------                   -----------           ------
		{  lMOVE   , lRIGHT (IDC_CGS_AUTOSAVE),lRIGHT(lPARENT)						,-0  },
		{  lMOVE   , lBOTTOM(IDC_CGS_AUTOSAVE),lBOTTOM(lPARENT)						,-0  },

		{  lMOVE   , lRIGHT (IDC_CGS_SAVEBUTTON),lRIGHT(lPARENT)						,-0  },
		{  lMOVE   , lBOTTOM(IDC_CGS_SAVEBUTTON),lTOP(IDC_CGS_AUTOSAVE)			,-5  },

		{  lSTRETCH, lRIGHT (IDC_CGS_EDITFILENAME),lLEFT(IDC_CGS_SAVEBUTTON),-5  },
		{  lMOVE   , lBOTTOM(IDC_CGS_EDITFILENAME),lBOTTOM(IDC_CGS_SAVEBUTTON),-0  },
    
		{  lSTRETCH, lRIGHT (IDC_CGS_EDITVIEW)  ,lRIGHT(lPARENT)						,-0  },
    {  lSTRETCH, lBOTTOM(IDC_CGS_EDITVIEW)  ,lTOP(IDC_CGS_SAVEBUTTON)		,-5  },

    {  lEND																																	 }
  };

  Layout_ComputeLayout( GetSafeHwnd(), rules );
}

// ---
void CGeneratorSummary::OnSaveOptions() { 

	CString strPluginName = CGT::GetPluginName(m_hImpData);
	if( CGT::s_OptFile.SaveOptionsData(m_nRegime ? FALSE : TRUE, m_hOptionsData, m_quidClassId, strPluginName) ){

		CRegStorage oRegStorage( CFG_REG_KEY );
		oRegStorage.PutValue( g_RegValueNameTable[m_nRegime][1], CGT::s_OptFile.GetPathname() );
		oRegStorage.PutValue( g_RegValueNameTable[m_nRegime][0], DWORD(FALSE) );
	}
}

// ---
void CGeneratorSummary::OnAutoSave() {
	m_bAutoSave = !m_bAutoSave;
	m_RegStorage.PutValue( g_RegValueNameTable[m_nRegime][2], m_bAutoSave );
}

