// CodeGenWizard.cpp : implementation file
//

#include "stdafx.h"
#include <serialize/kldtser.h>
#include "VisualEditor.h"
#include "VisualEditorDlg.h"
#include "CodeGenWizard.h"
#include "CodeGenTools.h"

#include "SelectInterfacesDlg.h"
#include "GeneralOptionsDlg.h"
#include "GeneratorOptionsDlg.h"
#include "GeneratorSummaryDlg.h"
#include <stuff/layout.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

// ---
struct CCGWBackButton : public CFAutoParam {
	CButton &m_rcButton;

	CCGWBackButton( CButton &rcButton );
	virtual void   SetState( int s );
};

// ---
struct CCGWNextButton : public CFAutoParam {
	CButton &m_rcButton;

	CCGWNextButton( CButton &rcButton );
	virtual void   SetState( int s );
};

// ---
struct CCGWFinishButton : public CFAutoParam {
	CButton &m_rcButton;

	CCGWFinishButton( CButton &rcButton );
	virtual void   SetState( int s );
};

// ---
CCGWBackButton::CCGWBackButton( CButton &rcButton ) :
	m_rcButton( rcButton ) {
}

// ---
void CCGWBackButton::SetState( int s ) {
	CFAutoParam::SetState( s );
	switch( m_State ) {
		case ps_nrm :
			m_rcButton.EnableWindow( TRUE );
			break;
		case ps_grd :
		case ps_und :
			m_rcButton.EnableWindow( FALSE );
			break;
	}
}

// ---
CCGWNextButton::CCGWNextButton( CButton &rcButton ) :
	m_rcButton( rcButton ) {
}

// ---
void CCGWNextButton::SetState( int s ) {
	CFAutoParam::SetState( s );
	CString rBtnName;
	switch( m_State ) {
		case ps_nrm :
			m_rcButton.EnableWindow( TRUE );
			break;
		case ps_grd :
		case ps_und :
			m_rcButton.EnableWindow( FALSE );
			break;
	}
}

// ---
CCGWFinishButton::CCGWFinishButton( CButton &rcButton ) :
	m_rcButton( rcButton ){
}

// ---
void CCGWFinishButton::SetState( int s ) {
	CFAutoParam::SetState( s );
	CString rBtnName;
	switch( m_State ) {
		case ps_nrm :
			m_rcButton.EnableWindow( TRUE );
			break;
		case ps_und :
			m_rcButton.EnableWindow( FALSE );
			break;
	}
}

// ---
static int  PS[][6] = {
//  Back		Next		Finish  SelInt  Opt		  Sum
	{ ps_grd, ps_nrm, ps_und, ps_nrm, ps_und, ps_und },	 // 0
	{ ps_nrm, ps_nrm, ps_nrm, ps_und, ps_nrm, ps_und },	 // 2
	{ ps_nrm, ps_grd, ps_nrm, ps_und, ps_und, ps_nrm },	 // 3
};


// ---
static int JT[][3] = {
	{ NO, 1,  3 }, // 0
	{  0, 2,  3 }, // 1
	{  1, 3,  3 }, // 2
};


// ---
PCFAState *CCodeGenWizard::GetStatesTable() {
	PCFAState *SA = new PCFAState[6];
	SA[0] = new CFAState(st_normal , PFEnter(&Enter0), PFEnter(&Leave0), PCFAPState(&PS[0]), (int *)&JT[0]), // SelInt
	SA[1] = new CFAState(st_normal , PFEnter(&Enter1), PFEnter(&Leave1), PCFAPState(&PS[1]), (int *)&JT[1]), // Opt
	SA[2] = new CFAState(st_normal , NULL           , NULL					 , PCFAPState(&PS[2]), (int *)&JT[2]), // Summary
	SA[3] = new CFAState(st_reduced, NULL           , NULL					 , PCFAPState(0)     , (int *)0     ),
	SA[4] = END_STATE_TABLE;
	return SA;
};

/////////////////////////////////////////////////////////////////////////////
// CCodeGenWizard dialog
CCodeGenWizard::CCodeGenWizard(HDATA hImpData, AVP_dword idSelectedIFace, BOOL i_bImplementation, CWnd* pParent /*=NULL*/)
	: CDialog(CCodeGenWizard::IDD, pParent),
	CFAutomaton( GetStatesTable() )
{
	//{{AFX_DATA_INIT(CCodeGenWizard)
	//}}AFX_DATA_INIT

	m_hImpData = hImpData ? ::DATA_Copy( 0, 0, hImpData, DATA_IF_ROOT_INCLUDE ) : NULL;
	m_hIntOptionsPattern = NULL;
	m_hWorkOptTree = NULL;
	m_hWorkIntRoot = NULL;
	m_nCurrOptIndex = -1;
	m_pPage = NULL;
	m_pInterface = NULL;
	m_bInSingleFile = FALSE;
	m_bOldInSingleFile = FALSE;
	m_nDirection = 0;
	m_hOptFileData = NULL;

	m_bImplementation = i_bImplementation;

	memset( &m_rOldCLSID, 0xff, sizeof(m_rOldCLSID) );
	memset( &m_rCLSID, 0x0, sizeof(m_rCLSID) );

	int nRegime = m_bImplementation ? 0:1;

	AddParam( new CCGWBackButton(m_BackBtn) );
	AddParam( new CCGWNextButton(m_NextBtn) );
	AddParam( new CCGWFinishButton(m_FinishBtn) );
	AddParam( new CSelectInterfacesDlg(m_hImpData, idSelectedIFace, m_hOptFileData, m_rcOptFileName, m_rcPublicDirName, m_rcPrivateDirName, nRegime, m_pInterface, m_rCLSID, m_ChildFrame, this) );
	AddParam( new CGeneratorOptionsDlg(m_pInterface, m_hIntOptionsPattern, m_hWorkIntRoot, m_ChildFrame, this ) );
	AddParam( new CGeneratorSummary(m_hImpData, m_hWorkIntRoot, m_hIntOptionsPattern, m_rcOptFileName, m_rCLSID, m_ChildFrame, nRegime, this ) );
}

// ---
CCodeGenWizard::~CCodeGenWizard() {
	if ( m_pPage ) {				
		delete m_pPage;
		m_pPage = NULL;
	}

	if ( m_hImpData ) 
		::DATA_Remove( m_hImpData, NULL );

	if ( m_hIntOptionsPattern )
		::DATA_Remove( m_hIntOptionsPattern, NULL );

	if ( m_hWorkOptTree ) 
		::DATA_Remove( m_hWorkOptTree, NULL );

	if ( m_hWorkIntRoot ) 
		::DATA_Remove( m_hWorkIntRoot, NULL );
	
	if ( m_pInterface ) 
		m_pInterface->Release();

	CGT::s_OptFile.Close();

	::CoFreeUnusedLibraries();
}

// ---
BOOL CCodeGenWizard::DestroyWindow() {
	Stop();
	return CDialog::DestroyWindow();
}

// ---
void CCodeGenWizard::DoDataExchange( CDataExchange* pDX ) {
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CCodeGenWizard)
	DDX_Control(pDX, IDC_CGW_FINISH, m_FinishBtn);
	DDX_Control(pDX, IDC_CGW_DLGFRAME, m_ChildFrame);
	DDX_Control(pDX, IDC_CGW_NEXT, m_NextBtn);
	DDX_Control(pDX, IDC_CGW_BACK, m_BackBtn);
	//}}AFX_DATA_MAP
}

// ---
bool CCodeGenWizard::Reduce() {
	((CGeneratorSummary *)m_rcPars[CGT::EAutoSummaryDlg])->PerformAutoSave();
	PrepareAndLaunchGenerator();
	CDialog::OnOK();
	return false;
}

// ---
void CCodeGenWizard::Stop() {
	for ( int i=0,c=m_rcPars.Count(); i<c; i++ ) {
		m_rcPars[i]->SetState( ps_und );
	}

	CFAutomaton::Stop();
}

// ---
bool CCodeGenWizard::Enter0( int &rcNewState ) {

	return true;
}

// ---
bool CCodeGenWizard::Leave0( int &rcNewState ) {

	// check VE_PID_CGIOINSINGLEFILE property in options set
	HDATA hOpts = GetOptFileOptions(); 

	if(hOpts){
		HPROP hInSingleFileProp = ::DATA_Find_Prop( hOpts, NULL, VE_PID_CGIOINSINGLEFILE );
		if ( hInSingleFileProp )
			m_bInSingleFile = FALSE;
		// In several files always !!::GetPropNumericValueAsDWord( hInSingleFileProp );
	}

	PrepareCodeGenOptions();
	m_nCurrOptIndex = -1;
	
	return true;
}

// ---
bool CCodeGenWizard::Enter1( int &rcNewState ) {
	m_nCurrOptIndex += m_nDirection;
	return true;
}

// ---
bool CCodeGenWizard::Leave1( int &rcNewState ) {
	m_nCurrOptIndex += m_nDirection;

	if(3 == rcNewState)
		((CGeneratorOptionsDlg*)m_rcPars[CGT::EAutoOptionsDlg])->GetGeneratorUpdatedOptions();
	
	return true;
}

// ---
// make options tree, it comes from opt file or from options pattern
void CCodeGenWizard::GenerateWorkOptTree() {

	if ( m_hWorkIntRoot ) {
		::DATA_Remove( m_hWorkIntRoot, NULL );
		m_hWorkIntRoot = NULL;
	}

	HDATA hOpts = GetOptFileOptions();

	if(!hOpts)
		hOpts = m_hIntOptionsPattern;

	ASSERT(NULL != hOpts);

	m_hWorkIntRoot = ::DATA_Copy(0, 0, hOpts, DATA_IF_ROOT_INCLUDE);
}

// ---
HDATA CCodeGenWizard::GetOptFileOptions(){

	HDATA hOpts = NULL;

	if(m_bImplementation){
		CString strPluginName = CGT::GetPluginName(m_hImpData);
		hOpts = CGT::s_OptFile.GetPluginOptions(m_rCLSID, strPluginName);
	}
	else{
		hOpts = CGT::s_OptFile.GetCodeGenOptions(m_rCLSID);
		hOpts = ::DATA_Get_First(hOpts, NULL);
	}


	return hOpts;
}


// ---
void CCodeGenWizard::PrepareCodeGenOptions() {

	BOOL bMustRegenerateOptions = FALSE;

	// we must check code gen rotation 
	// and preserve edited options
	// and process new session state

	// load rotated generator and its options pattern
	if(m_rOldCLSID != m_rCLSID){
		
		if(m_pInterface)
			m_pInterface->Release();
		
		m_pInterface = CGT::LoadGenerator(m_rCLSID);

		LoadCodeGenOptionsPattern();

		// if generator changed - anyway regenerate options
		bMustRegenerateOptions = TRUE;
	}	

	// new session
	if(! (HDATA)CGT::s_OptFile)
		bMustRegenerateOptions = TRUE;
	
	if(bMustRegenerateOptions)
		GenerateWorkOptTree();
	else
		// othewise leave the same options
		ASSERT(NULL != m_hWorkIntRoot);
	
	m_rOldCLSID = m_rCLSID;
}

// ---
void CCodeGenWizard::LoadCodeGenOptionsPattern(){

	if(!m_pInterface){
		ASSERT(FALSE);
		return;
	}

	if ( m_hIntOptionsPattern ){
		::DATA_Remove( m_hIntOptionsPattern, NULL );
		m_hIntOptionsPattern = NULL;
	}
	
	DWORD dwSize = 0;
	BYTE *pBuffer = NULL; 
  if ( m_bImplementation ) {
    if ( FAILED(m_pInterface->GetImplementationOptionTemplate(&dwSize, &pBuffer)) ) 
      return;
  }
  else if ( FAILED(m_pInterface->GetPrototypeOptionTemplate(&dwSize, &pBuffer)) )
    return;

	m_hIntOptionsPattern = ::DeserializeDTree( pBuffer, dwSize );
	::CoTaskMemFree( pBuffer );
}

// ---
void CCodeGenWizard::ShowGeneratorError( HRESULT hResult, CString &rError ) {
	UCHAR *pBuffer = NULL;
	if ( SUCCEEDED(m_pInterface->GetErrorString( hResult, &pBuffer)) ) {
		rError += "\r\n";
		rError += (TCHAR*)pBuffer;
	}

	::CoTaskMemFree( pBuffer );
	::DlgMessageBox( GetParent(), rError );
}

// ---
HRESULT CCodeGenWizard::LaunchGenerator( HDATA hImpData, HDATA hOptData ) {

	DWORD nImpSSize = 0;
	BYTE *pImpBuffer = (BYTE *)::SerializeDTree( hImpData, nImpSSize );

	DWORD nOptSSize = 0;
	BYTE *pOptBuffer = 0;

	if ( hOptData )
		pOptBuffer = (BYTE *)::SerializeDTree( hOptData, nOptSSize );

	HRESULT hResult = 
		m_bImplementation ? 
		m_pInterface->ImplementationGenerate( nImpSSize, pImpBuffer, nOptSSize, pOptBuffer ) :
		m_pInterface->PrototypeGenerate( nImpSSize, pImpBuffer, nOptSSize, pOptBuffer );

	::CoTaskMemFree( pImpBuffer );
	::CoTaskMemFree( pOptBuffer );

	return hResult;
}

// ---
HRESULT CCodeGenWizard::LoadTypeTable() {
	HRESULT hResult = S_OK;
	CAPointer<TCHAR> pStartup = ::GetStartupDirectory();
	CPathStr pFileName = pStartup;
	pFileName ^= "PTypeTbl.ptt";

	HDATA hTypesData = NULL;
	if ( ::KLDT_DeserializeUsingSWM(pFileName, &hTypesData) && hTypesData ) {
		DWORD nSSize = 0;
		BYTE *pBuffer = (BYTE *)::SerializeDTree( hTypesData, nSSize );
		hResult = m_pInterface->SetObjectTypes( nSSize, pBuffer );
		::CoTaskMemFree( pBuffer );
		::DATA_Remove( hTypesData, NULL );
	}
	return hResult;
}

// ---
bool CCodeGenWizard::PrepareAndSetGeneratorProjectInfo() {

	DWORD nImpSSize = 0;
	BYTE *pImpBuffer = NULL;
	bool bResult = true;

	HDATA hRootData = ::DATA_Add( NULL, NULL, VE_PID_CGPI_ROOT, 0, 0 );
	::DATA_Add_Prop( hRootData, NULL, VE_PID_CGPI_PUBLICDIR, AVP_dword(LPCTSTR(m_rcPublicDirName)), 0 );
	::DATA_Add_Prop( hRootData, NULL, VE_PID_CGPI_PRIVATEDIR, AVP_dword(LPCTSTR(m_rcPrivateDirName)), 0 );
		
	pImpBuffer = (BYTE *)::SerializeDTree( hRootData, nImpSSize );
	::DATA_Remove( hRootData, NULL );

	HRESULT hResult = m_pInterface->StartProject( nImpSSize, pImpBuffer );
	if ( hResult != S_OK ) {
		CString rError;
		rError.Format( IDS_CGW_ERR_INITFAILED );
		ShowGeneratorError( hResult, rError );
		bResult = false;
	}
	
	::CoTaskMemFree( pImpBuffer );

	return bResult;
}

// ---
void CCodeGenWizard::PrepareAndLaunchGenerator() {

	if(!m_pInterface || !m_hWorkIntRoot ){
		ASSERT(FALSE);
		return;
	}

	CWaitCursor rWait;

	CString rError;
	HRESULT hRes = LoadTypeTable();

	if ( FAILED(hRes) ) {
		rError.LoadString( IDS_CGW_ERR_SETTYPES );
		ShowGeneratorError( hRes, rError );
		return;
	}

	if ( PrepareAndSetGeneratorProjectInfo() ) {

		hRes = LaunchGenerator( m_hImpData, m_hWorkIntRoot);
		
		if ( FAILED(hRes) )
      rError.Format( IDS_CGW_ERR_GENERATION, _T("---") );
    else if ( hRes == S_FALSE )
			rError.Format(IDS_CGW_ERR_GENERATION_FALSE, _T("---"));
    else
      rError.Empty();
    
		if( S_OK != hRes )
			ShowGeneratorError( hRes, rError );
	}

	m_pInterface->EndProject();
}

BEGIN_MESSAGE_MAP(CCodeGenWizard, CDialog)
	//{{AFX_MSG_MAP(CCodeGenWizard)
	ON_BN_CLICKED(IDC_CGW_NEXT, OnCgwNext)
	ON_BN_CLICKED(IDC_CGW_BACK, OnCgwBack)
	ON_BN_CLICKED(IDC_CGW_FINISH, OnCgwFinish)
	ON_COMMAND(IDOK, OnOK)
	ON_WM_SIZE()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CCodeGenWizard message handlers
// ---
BOOL CCodeGenWizard::OnInitDialog() {
	CDialog::OnInitDialog();
	
	if ( !m_hImpData ) {
    ::DlgMessageBox( GetParent(), IDS_CGW_ERR_NOINTERFACES );
    EndDialog( IDCANCEL );
    return FALSE;
  }
  
  Start();
  
  if ( m_pPage ) {
    HWND hWndNext = ::GetNextDlgTabItem( *m_pPage, NULL, FALSE );
    if ( hWndNext != NULL )	
      ::SetFocus( hWndNext );
  }
  
  return !m_pPage;  // return TRUE unless you set the focus to a control
  // EXCEPTION: OCX Property Pages should return FALSE
}

// ---
void CCodeGenWizard::OnOK() {
}

// ---
void CCodeGenWizard::OnCgwNext() {
  m_nDirection = 1;
  SetEvent( 1 );
}

// ---
void CCodeGenWizard::OnCgwBack() {
  m_nDirection = -1;
  SetEvent( 0 );
}

// ---
void CCodeGenWizard::OnCgwFinish() {
  SetEvent( 2 );
}

void CCodeGenWizard::OnSize(UINT nType, int cx, int cy) 
{
  CDialog::OnSize(nType, cx, cy);
  
  static RULE rules[] = {
    // Action    Act-on                                 Relative-to                      Offset
    // ------    ------                                 -----------                      ------
    //  {  lSTRETCH, lBOTTOM(IDC_HGG_CTTTREE),           lTOP(IDC_G_OBJECTS),                -4  },
    
    {  lMOVE,    lBOTTOM (IDC_CGW_BACK),                 lBOTTOM(lPARENT),                   -8  },
    {  lMOVE,    lBOTTOM (IDC_CGW_NEXT),                 lBOTTOM(lPARENT),                   -8  },
    {  lMOVE,    lBOTTOM (IDC_CGW_FINISH),               lBOTTOM(lPARENT),                   -8  },
    {  lMOVE,    lBOTTOM (IDCANCEL),                     lBOTTOM(lPARENT),                   -8  },
    
    {  lMOVE,    lRIGHT  (IDCANCEL),                     lRIGHT(lPARENT),                    -8  },
    {  lMOVE,    lRIGHT  (IDC_CGW_FINISH),               lLEFT (IDCANCEL),                   -2  },
    {  lMOVE,    lRIGHT  (IDC_CGW_BACK),                 lLEFT (IDC_CGW_FINISH),             -2  },
    {  lMOVE,    lRIGHT  (IDC_CGW_NEXT),                 lLEFT (IDC_CGW_BACK),               -2  },
    
    {  lSTRETCH, lBOTTOM (IDC_CGW_DLGFRAME),             lTOP(IDCANCEL),                     -10 },
    {  lSTRETCH, lRIGHT  (IDC_CGW_DLGFRAME),             lRIGHT(lPARENT),                    -8  },
    
    {  lEND                                                                                      }
  };
  
  Layout_ComputeLayout( GetSafeHwnd(), rules );
}
