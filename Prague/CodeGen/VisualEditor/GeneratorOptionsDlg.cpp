// GeneratorOptionsDlg.cpp : implementation file
//

#include "stdafx.h"
#include <stuff/layout.h>
#include <datatreeutils/dtutils.h>
#include <stuffio/ioutil.h>
#include <stuff/cpointer.h>
#include "../avppveid.h"
#include <avpprpid.h>
#include <avpcontrols/propertysetget.h>
#include "codegenwizard.h"
#include "visualeditor.h"
#include "generatoroptionsdlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

void PrepareFilterIndex( OPENFILENAME &pOfn, const char *pDefExt );


/////////////////////////////////////////////////////////////////////////////
// CGeneratorOptionsDlg dialog


CGeneratorOptionsDlg::CGeneratorOptionsDlg( IPragueCodeGen	*&pInterface, 
   HDATA &hIntOptionsPattern, HDATA &hIntOptionsRoot, CWnd &rcFrame, CWnd* pParent /*=NULL*/)
	: CDialog(CGeneratorOptionsDlg::IDD, pParent),
	m_rcFrame( rcFrame ),
	m_hIntOptionsRoot( hIntOptionsRoot ),
	m_hIntOptionsPattern( hIntOptionsPattern ),
	m_pInterface( pInterface )
{
	//{{AFX_DATA_INIT(CGeneratorOptionsDlg)
	m_Title = _T("");
	//}}AFX_DATA_INIT
}


void CGeneratorOptionsDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CGeneratorOptionsDlg)
	DDX_Control(pDX, IDC_CGO_CTTCONTROL, m_CTTControl);
	DDX_Text(pDX, IDC_CGO_TITLE, m_Title);
	//}}AFX_DATA_MAP
}

// ---
BOOL CGeneratorOptionsDlg::DestroyWindow() {

	GetGeneratorUpdatedOptions();
	return CDialog::DestroyWindow();
}

// ---
void CGeneratorOptionsDlg::GetGeneratorUpdatedOptions(){
					    
	if ( m_hIntOptionsData ) {
		HDATA hIntOptData = NULL;
		
		if ( ::DTUT_ExtractValuesDataTree(m_hIntOptionsData, &hIntOptData, NULL) ) {
			
			if ( m_hIntOptionsRoot ) 
				::DATA_Remove( m_hIntOptionsRoot, NULL );
			
			m_hIntOptionsRoot = hIntOptData;
		}
		
		::DATA_Remove( m_hIntOptionsData, NULL );
		m_hIntOptionsData = NULL;
	}
}


// ---
void  CGeneratorOptionsDlg::SetState( int s ) {
	CFAutoParam::SetState( s );
	switch ( m_State ) {
		case ps_nrm :
			
			if ( !GetSafeHwnd() ) {
				Create( CGeneratorOptionsDlg::IDD, m_pParentWnd );

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

BEGIN_MESSAGE_MAP(CGeneratorOptionsDlg, CDialog)
	//{{AFX_MSG_MAP(CGeneratorOptionsDlg)
	ON_WM_SIZE()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CGeneratorOptionsDlg message handlers
// ---
BOOL CGeneratorOptionsDlg::OnInitDialog() {
	CDialog::OnInitDialog();	   
	
	::SetWindowLong( m_CTTControl, GWL_ID, IDC_CGO_CTTCONTROL );

	m_hIntOptionsData = m_hIntOptionsRoot;

	HDATA hIntOptData = NULL;
	if ( ::DTUT_MergeDataTreeAndPattern(m_hIntOptionsData, m_hIntOptionsPattern, &hIntOptData) ) {

		m_CTTControl.SetPropertyTree(	(OLE_HANDLE)hIntOptData, FALSE );
		m_hIntOptionsData = hIntOptData;
	}

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

// ---
void CGeneratorOptionsDlg::OnSize(UINT nType, int cx, int cy) {
	CDialog::OnSize(nType, cx, cy);
	
  static RULE rules[] = {
    // Action    Act-on                   Relative-to           Offset
    // ------    ------                   -----------           ------
    {  lSTRETCH, lRIGHT (IDC_CGO_CTTCONTROL),lRIGHT(lPARENT)						,-0  },
    {  lSTRETCH, lBOTTOM(IDC_CGO_CTTCONTROL),lBOTTOM(lPARENT)						,-0  },

    {  lEND																				 }
  };

  Layout_ComputeLayout( GetSafeHwnd(), rules );
}


BEGIN_EVENTSINK_MAP(CGeneratorOptionsDlg, CDialog)
    //{{AFX_EVENTSINK_MAP(CGeneratorOptionsDlg)
	ON_EVENT(CGeneratorOptionsDlg, IDC_CGO_CTTCONTROL, 12 /* CTTDoExternalEditingEx */, OnCTTDoExternalEditingEx, VTS_I4 VTS_PBSTR VTS_PBOOL)
	//}}AFX_EVENTSINK_MAP
END_EVENTSINK_MAP()

// ---
void CGeneratorOptionsDlg::OnCTTDoExternalEditingEx( long hData, BSTR FAR* pItemText, BOOL FAR* pbCompleted ) {

	*pbCompleted = FALSE;

	if ( m_pInterface ) {
		CString rString( *pItemText );
		::SysFreeString( *pItemText );

//		*pbCompleted = PerformFileDlg( HDATA(hData), rString );

		BeginModalState();

		DWORD dwSSize;
		BYTE *pBuffer = (BYTE *)::SerializeDTree( (HDATA)hData, dwSSize );
		if ( m_pInterface->EditNode(*this, &dwSSize, &pBuffer) == S_OK ) {
			hData = (long)::DeserializeDTree( pBuffer, dwSSize );
			HPROP hProp = ::DATA_Find_Prop( (HDATA)hData, NULL, NULL );

			CAPointer<char> pText = ::GetPropValueAsString( hProp, NULL );
			rString = pText;

			::DATA_Remove( (HDATA)hData, NULL );

			*pbCompleted = TRUE;
		}

		if ( pBuffer )
			::CoTaskMemFree( pBuffer );

		EndModalState();

		*pItemText = rString.AllocSysString();
	}
}
