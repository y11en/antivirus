// HeaderGenWizard.cpp : implementation file
//

#include "stdafx.h"
#include "resource.h"
#include <avpcontrols/propertysetget.h>
#include "../avppveid.h"
#include <serialize/kldtser.h>
#include <stuff/cpointer.h>
#include <stuff/comutils.h>
#include <stuff/pathstr.h>
#include <datatreeutils/dtutils.h>
#include "visualeditor.h"
#include "visualeditordlg.h"
#include "visualeditor.h"
#include "headergenwizard.h"
#include "generatoroptionsfiledlg.h"
#include "selectinterfacesdlg.h"
#include "selectgeneratordlg.h"
#include "generatoroptionsdlg.h"
#include "generatorsummarydlg.h"
#include "codegentools.h"
#include <stuff/layout.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#define CGW_PAGESCOUNT		2
#define CGW_FIRSTOPTPAGE	3
#define CGW_OPTPAGESCOUNT (m_dwInterfaceCount)


// ---
struct CHGWBackButton : public CFAutoParam {
	CButton &m_rcButton;

	CHGWBackButton( CButton &rcButton );
	virtual void   SetState( int s );
};

// ---
struct CHGWNextButton : public CFAutoParam {
	CButton &m_rcButton;

	CHGWNextButton( CButton &rcButton );
	virtual void   SetState( int s );
};

// ---
struct CHGWFinishButton : public CFAutoParam {
	CButton &m_rcButton;
	BOOL		&m_bDirectEnable; 

	CHGWFinishButton( CButton &rcButton, BOOL &bDirectEnable );
	virtual void   SetState( int s );
};

// ---
CHGWBackButton::CHGWBackButton( CButton &rcButton ) :
	m_rcButton( rcButton ) {
}


// ---
void CHGWBackButton::SetState( int s ) {
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
CHGWNextButton::CHGWNextButton( CButton &rcButton ) :
	m_rcButton( rcButton ) {
}


// ---
void CHGWNextButton::SetState( int s ) {
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
CHGWFinishButton::CHGWFinishButton( CButton &rcButton, BOOL &bDirectEnable ) :
	m_rcButton( rcButton ),
	m_bDirectEnable( bDirectEnable ) {
}


// ---
void CHGWFinishButton::SetState( int s ) {
	CFAutoParam::SetState( s );
	CString rBtnName;
	switch( m_State ) {
		case ps_nrm :
			m_rcButton.EnableWindow( TRUE );
			break;
		case ps_grd :
			m_rcButton.EnableWindow( m_bDirectEnable );
			break;
		case ps_und :
			m_rcButton.EnableWindow( FALSE );
			break;
	}
}



// ---
static int GetDataSelectedDatasCount( HDATA hData ) {
	int nCount = 0;
	if ( hData ) {
		HDATA hCurrData = ::DATA_Get_First( hData, NULL );
		while ( hCurrData ) {
			HPROP hSelectedProp = ::DATA_Find_Prop( hCurrData, NULL, VE_PID_IF_SELECTED );
			if ( hSelectedProp ) 
				nCount += !!::GetPropNumericValueAsDWord( hSelectedProp );
			hCurrData = ::DATA_Get_Next( hCurrData, NULL );
		}
	}
	return nCount;
}

// ---
static int  PS[][6] = {
//  Back		Next		Finish  SelInt  Opt		  Sum
	{ ps_grd, ps_nrm, ps_und, ps_nrm, ps_und, ps_und },	 // 0
	{ ps_nrm, ps_nrm, ps_grd, ps_und, ps_nrm, ps_und },	 // 1
	{ ps_nrm, ps_nrm, ps_grd, ps_und, ps_und, ps_nrm },	 // 2
};


// ---
static int JT[][3] = {
	{ NO, 1,  3 }, // 0
	{  0, 2,  3 }, // 1
	{  1, 3,  3 }, // 2
};

// ---
PCFAState *CHeaderGenWizard::GetStatesTable() {

	PCFAState *SA = new PCFAState[5];
	SA[0] = new CFAState(st_normal , PFEnter(&Enter0), PFEnter(&Leave0), PCFAPState(&PS[0]), (int *)&JT[0]), // SelInt
	SA[1] = new CFAState(st_normal , PFEnter(&Enter1), PFEnter(&Leave1), PCFAPState(&PS[1]), (int *)&JT[1]), // Opt
	SA[2] = new CFAState(st_normal , NULL           , NULL					 , PCFAPState(&PS[2]), (int *)&JT[2]), // Summary
	SA[3] = new CFAState(st_reduced, NULL           , NULL					 , PCFAPState(0)     , (int *)0     ),
	SA[4] = END_STATE_TABLE;
	return SA;
};




/////////////////////////////////////////////////////////////////////////////
// CHeaderGenWizard dialog


CHeaderGenWizard::CHeaderGenWizard(HDATA hImpData, AVP_dword idSelectedIFace, CWnd* pParent /*=NULL*/)
	: CDialog(CHeaderGenWizard::IDD, pParent),
	CFAutomaton( GetStatesTable() )
{
	//{{AFX_DATA_INIT(CHeaderGenWizard)
	//}}AFX_DATA_INIT

	m_hImpData = hImpData ? ::DATA_Copy( 0, 0, hImpData, DATA_IF_ROOT_INCLUDE ) : NULL;
	m_hIntOptionsPattern = NULL;
	m_hWorkOptTree = NULL;
	m_hWorkIntRoot = NULL;
	m_nCurrOptIndex = -1;
	m_pPage = NULL;
	m_pInterface = NULL;
	m_nDirection = 0;
	m_hOptFileData = NULL;
	m_bFinishEnable = TRUE;

	memset( &m_rOldCLSID, 0xff, sizeof(m_rOldCLSID) );
	memset( &m_rCLSID, 0x0, sizeof(m_rCLSID) );

	m_dwInterfaceCount = ::GetDataSelectedDatasCount( m_hImpData );
	m_dwPagesCount = CGW_PAGESCOUNT + CGW_OPTPAGESCOUNT;

	AddParam( new CHGWBackButton(m_BackBtn) );
	AddParam( new CHGWNextButton(m_NextBtn) );
	AddParam( new CHGWFinishButton(m_FinishBtn, m_bFinishEnable) );
	AddParam( new CSelectInterfacesDlg(m_hImpData, idSelectedIFace, m_hOptFileData, m_rcOptFileName, m_rcPublicDirName, m_rcPrivateDirName, 1, m_pInterface, m_rCLSID, m_ChildFrame, this) );
	AddParam( new CGeneratorOptionsDlg( m_pInterface, m_hIntOptionsPattern, m_hWorkIntRoot, m_ChildFrame, this ) );
	AddParam( new CGeneratorSummary( m_hImpData, m_hWorkOptTree, m_hIntOptionsPattern, m_rcOptFileName, m_rCLSID, m_ChildFrame, 1, this ) );
}


void CHeaderGenWizard::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CHeaderGenWizard)
	DDX_Control(pDX, IDC_HGW_NEXT, m_NextBtn);
	DDX_Control(pDX, IDC_HGW_BACK, m_BackBtn);
	DDX_Control(pDX, IDC_HGW_FINISH, m_FinishBtn);
	DDX_Control(pDX, IDC_HGW_DLGFRAME, m_ChildFrame);
	//}}AFX_DATA_MAP
}

// ---
CHeaderGenWizard::~CHeaderGenWizard() {
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

	if ( m_hOptFileData )
		::DATA_Remove( m_hOptFileData, NULL );

	if ( m_pInterface ) 
		m_pInterface->Release();

	::CoFreeUnusedLibraries();
}

// ---
BOOL CHeaderGenWizard::DestroyWindow() {
	return CDialog::DestroyWindow();
}

// ---
bool CHeaderGenWizard::Reduce() {
	GetInterfaceOptionsPattern();
	((CGeneratorSummary *)m_rcPars[5])->PerformAutoSave();
	PrepareAndLaunchGenerator();
	CDialog::OnOK();
	return false;
}


// ---
bool CHeaderGenWizard::Enter0( int &rcNewState ) {
	if ( m_pInterface ) 
		m_pInterface->Release();
	m_pInterface = NULL;
	return true;
}



// ---
bool CHeaderGenWizard::Leave0( int &rcNewState ) {
	m_dwInterfaceCount = ::GetDataSelectedDatasCount( m_hImpData );
	if ( !m_dwInterfaceCount ) {
		::DlgMessageBox( GetParent(), IDS_HGW_ERR_NOINTERFACES );
		rcNewState = m_nCurrState;
	}
	m_dwPagesCount = CGW_PAGESCOUNT + CGW_OPTPAGESCOUNT;
	
	if ( m_hOptFileData ) {
		HPROP hCGClsIDProp = ::DATA_Find_Prop( m_hOptFileData, NULL, VE_PID_CGIOGENERATORCLSID );
		if ( hCGClsIDProp )
			::PROP_Get_Val( hCGClsIDProp, &m_rCLSID, sizeof(m_rCLSID) );
	}
	m_bFinishEnable = !!m_hOptFileData;
	
	if ( m_nDirection > 0 )
		GetInterfaceOptionsPattern();
	
	return true;
}



// ---
bool CHeaderGenWizard::Enter1( int &rcNewState ) {
	m_nCurrOptIndex += m_nDirection;
	if ( m_nCurrState == 0 || m_nCurrState == 2 )
		m_hWorkIntRoot = NULL;
	else {
		m_rcPars[4]->SetState( ps_und );
		CopyCurrInterfaceData();
	}
	FindCurrInterfaceData( m_nDirection );
	return true;
}



// ---
bool CHeaderGenWizard::Leave1( int &rcNewState ) {
	int nCurrOptIndex = m_nCurrOptIndex + m_nDirection;
	if ( nCurrOptIndex < 0 )
		m_nCurrOptIndex = -1;
	else if ( rcNewState > 2 )
		;
	else if ( nCurrOptIndex >= int(CGW_OPTPAGESCOUNT) )
		m_nCurrOptIndex = nCurrOptIndex;
	else
		rcNewState = m_nCurrState;
	return true;
}



// ---
//bool CHeaderGenWizard::Leave2( int &rcNewState ) {
//	/*
//	if ( m_nDirection > 0 )
//		GetInterfaceOptionsPattern();
//	*/
//	return true;
//}

// ---
//bool CHeaderGenWizard::Leave4( int &rcNewState ) {
//	int nCurrOptIndex = m_nCurrOptIndex + m_nDirection;
//	if ( nCurrOptIndex < 0 )
//		rcNewState = 2;
//	else
//		if ( nCurrOptIndex >= int(CGW_OPTPAGESCOUNT) )
//			rcNewState = 4;
//		else
//			rcNewState = m_nCurrState;
//	return true;
//}


// ---
static void MergeFileOptionsWithOptTree( HDATA hNewData, HDATA hOptFileData ) {
	if ( hOptFileData ) {
		HPROP hIntNameProp = ::DATA_Find_Prop( hNewData, NULL , VE_PID_IF_INTERFACENAME );
		if ( hIntNameProp ) {
			CAPointer<char> pIntName = ::GetPropValueAsString( hIntNameProp, NULL );

			HDATA hCurrData = ::DATA_Get_First( hOptFileData, NULL );
			while ( hCurrData ) {
				hIntNameProp = ::DATA_Find_Prop( hCurrData, NULL , VE_PID_IF_INTERFACENAME );
				if ( hIntNameProp ) {
					CAPointer<char> pCurrIntName = ::GetPropValueAsString( hIntNameProp, NULL );
					if ( !lstrcmp(pCurrIntName, pIntName) ) {
						HDATA hIntOptionsData = ::DATA_Get_First( hCurrData, NULL );
						if ( hIntOptionsData ) {
							HDATA hData = ::DATA_Copy( 0, 0, hIntOptionsData, DATA_IF_ROOT_INCLUDE );
							::DATA_Attach( hNewData, NULL, hData, DATA_IF_ROOT_INCLUDE );
							return;
						}
					}
				}
				hCurrData = ::DATA_Get_Next( hCurrData, NULL );
			}
		}

		HDATA hIntData = ::DATA_Get_First( hOptFileData, NULL );
		HDATA hIntOptionsData = ::DATA_Get_First( hIntData, NULL );
		if ( hIntOptionsData ) {
			HDATA hData = ::DATA_Copy( 0, 0, hIntOptionsData, DATA_IF_ROOT_INCLUDE );
			if ( hData ) 
				::DATA_Attach( hNewData, NULL, hData, DATA_IF_ROOT_INCLUDE );
		}
	}
}


// ---
void CHeaderGenWizard::GenerateWorkOptTree() {
	if ( m_hWorkOptTree ) {
		::DATA_Remove( m_hWorkOptTree, NULL );
		m_hWorkOptTree = NULL;
	}

	HDATA hCodeGenData = CGT::s_OptFile.GetCodeGenOptions(m_rCLSID);

	m_hWorkOptTree = ::DATA_Add( NULL, NULL, VE_PID_IF_PROPERTY(::CreateDataId(m_hOptFileData)), 
		AVP_dword(wmt_CGOptions), 0 );

	m_hWorkIntRoot = NULL;

	if ( m_hWorkOptTree ) {
		HDATA hCurrData = ::DATA_Get_First( m_hImpData, NULL );
		while ( hCurrData ) {
			HDATA hNewData = ::DATA_Copy( 0, 0, hCurrData, DATA_IF_ROOT_INCLUDE | DATA_IF_DOESNOT_GO_DOWN );
			if ( hNewData ) {
				::MergeFileOptionsWithOptTree( hNewData, hCodeGenData );
				::DATA_Attach( m_hWorkOptTree, NULL, hNewData, DATA_IF_ROOT_INCLUDE );
			}
			hCurrData = ::DATA_Get_Next( hCurrData, NULL );
		}
	}
}

// ---
void CHeaderGenWizard::GenerateAndMergeWorkOptTree() {
	m_hWorkIntRoot = NULL;

	HDATA hWorkOptTree = ::DATA_Add( NULL, NULL, VE_PID_CGIOPTIONSROOT, AVP_dword(wmt_CGOptions), 0 );
	if ( hWorkOptTree ) {
		HDATA hCurrData = ::DATA_Get_First( m_hImpData, NULL );
		while ( hCurrData ) {
			HDATA hNewData = ::DATA_Copy( 0, 0, hCurrData, DATA_IF_ROOT_INCLUDE | DATA_IF_DOESNOT_GO_DOWN );
			if ( hNewData ) {
				::MergeFileOptionsWithOptTree( hNewData, m_hWorkOptTree );
				::DATA_Attach( hWorkOptTree, NULL, hNewData, DATA_IF_ROOT_INCLUDE );
			}
			hCurrData = ::DATA_Get_Next( hCurrData, NULL );
		}
	}

	if ( m_hWorkOptTree ) 
		::DATA_Remove( m_hWorkOptTree, NULL );
	m_hWorkOptTree = hWorkOptTree;
}


// ---
void CHeaderGenWizard::GetInterfaceOptionsPattern() {
	if ( m_rOldCLSID != m_rCLSID ) {
		if ( m_hIntOptionsPattern )
			::DATA_Remove( m_hIntOptionsPattern, NULL );
		m_hIntOptionsPattern = NULL;

		DWORD dwSize = 0;
		BYTE *pBuffer = NULL; 
		if ( m_pInterface && SUCCEEDED(m_pInterface->GetPrototypeOptionTemplate(&dwSize, &pBuffer)) ) {
			m_hIntOptionsPattern = ::DeserializeDTree( pBuffer, dwSize );
			::CoTaskMemFree( pBuffer );
		}

		GenerateWorkOptTree();

		::DATA_Add_Prop( m_hWorkOptTree, NULL, VE_PID_CGIOGENERATORCLSID, AVP_dword(&m_rCLSID), sizeof(m_rCLSID) );

		m_rOldCLSID = m_rCLSID;						    
	}
	else {
		m_hWorkIntRoot = NULL;
	}
}

// ---
static bool IsNodeSelected( HDATA hData ) {
	HPROP hSelectedProp = ::DATA_Find_Prop( hData, NULL, VE_PID_IF_SELECTED );
	if ( hSelectedProp ) 
		return !!::GetPropNumericValueAsDWord( hSelectedProp );
	return false;
}

// ---
static HDATA GetPrevSelectedData( HDATA hData ) {
	HDATA hPrevSelected = ::DATA_Get_Prev( hData, NULL );
	while ( hPrevSelected ) {
		if ( ::IsNodeSelected(hPrevSelected) )
			return hPrevSelected;
		hPrevSelected = ::DATA_Get_Prev( hPrevSelected, NULL );
	}
	return NULL;
}


// ---
static HDATA GetNextSelectedData( HDATA hData ) {
	HDATA hNextSelected = ::DATA_Get_Next( hData, NULL );
	while ( hNextSelected ) {
		if ( ::IsNodeSelected(hNextSelected) )
			return hNextSelected;
		hNextSelected = ::DATA_Get_Next( hNextSelected, NULL );
	}
	return NULL;
}

// ---
static HDATA GetLastSelectedData( HDATA hData ) {
	HDATA hLastSelected = ::DATA_Get_Last( hData, NULL );
	while ( hLastSelected ) {
		if ( ::IsNodeSelected(hLastSelected) )
			return hLastSelected;
		hLastSelected = ::DATA_Get_Prev( hLastSelected, NULL );
	}
	return NULL;
}

// ---
static HDATA GetFirstSelectedData( HDATA hData ) {
	HDATA hFirstSelected = ::DATA_Get_First( hData, NULL );
	while ( hFirstSelected ) {
		if ( ::IsNodeSelected(hFirstSelected) )
			return hFirstSelected;
		hFirstSelected = ::DATA_Get_Next( hFirstSelected, NULL );
	}
	return NULL;
}

// ---
void CHeaderGenWizard::FindCurrInterfaceData( int nDirect ) {
	if ( m_hWorkIntRoot ) {
		if ( nDirect < 0 ) 
			m_hWorkIntRoot = ::GetPrevSelectedData( m_hWorkIntRoot );
		else 
			m_hWorkIntRoot = ::GetNextSelectedData( m_hWorkIntRoot );
	}
	else
		if ( nDirect < 0 ) 
			m_hWorkIntRoot = ::GetLastSelectedData( m_hWorkOptTree );
		else 
			m_hWorkIntRoot = ::GetFirstSelectedData( m_hWorkOptTree );
}


// ---
void CHeaderGenWizard::CopyCurrInterfaceData() {
	if ( m_hWorkIntRoot ) {
		HDATA hCurrIntOptionsData = ::DATA_Get_First( m_hWorkIntRoot, NULL );
		if ( hCurrIntOptionsData ) {
			HDATA hIntRoot = ::DATA_Get_Next( m_hWorkIntRoot, NULL );
			while ( hIntRoot ) {
				if ( ::IsNodeSelected(hIntRoot) ) {
					HDATA hIntOptionsData = ::DATA_Get_First( hIntRoot, NULL );
					if ( !hIntOptionsData ) {
						hIntOptionsData = ::DATA_Copy( NULL, NULL, hCurrIntOptionsData, DATA_IF_ROOT_INCLUDE );
						::DATA_Attach( hIntRoot, NULL, hIntOptionsData, DATA_IF_ROOT_INCLUDE );
					}
				}
				hIntRoot = ::DATA_Get_Next( hIntRoot, NULL );
			}
		}
	}
}

// ---
void CHeaderGenWizard::PrepareImpTree( HDATA hImpData, HDATA &hIntData, HDATA hWorkData, CString &rIntName ) {
	if ( ::IsNodeSelected(hWorkData) ) {
		HPROP hIntNameProp = ::DATA_Find_Prop( hWorkData, NULL , VE_PID_IF_INTERFACENAME );
		if ( hIntNameProp ) {
			CAPointer<char> pIntName = ::GetPropValueAsString( hIntNameProp, NULL );

			HDATA hCurrData = ::DATA_Get_First( hImpData, NULL );
			while ( hCurrData ) {
				hIntNameProp = ::DATA_Find_Prop( hCurrData, NULL , VE_PID_IF_INTERFACENAME );
				if ( hIntNameProp ) {
					CAPointer<char> pCurrIntName = ::GetPropValueAsString( hIntNameProp, NULL );
					if ( !lstrcmp(pCurrIntName, pIntName) ) {
						rIntName = pCurrIntName;
						hIntData = hCurrData;
						break;
					}
				}
				hCurrData = ::DATA_Get_Next( hCurrData, NULL );
			}
		}
	}
}

// ---
void CHeaderGenWizard::ShowGeneratorError( HRESULT hResult, CString &rError ) {
	UCHAR *pBuffer = NULL;
	if ( SUCCEEDED(m_pInterface->GetErrorString( hResult, &pBuffer)) ) {
		rError += "\r\n";
		rError += (TCHAR*)pBuffer;
	}

	::CoTaskMemFree( pBuffer );

	::DlgMessageBox( GetParent(), rError );
}

// ---
bool CHeaderGenWizard::LaunchGenerator( HDATA hImpData, HDATA hOptData, CString &rIntName ) {
	bool bResult = true;
	if ( /*hImpData &&*/ ::IsNodeSelected(hOptData) ) {
		DWORD nImpSSize = 0;
		BYTE *pImpBuffer = 0;
		DWORD nOptSSize = 0;
		BYTE *pOptBuffer = 0;
		HDATA hIntOptionsData = ::DATA_Get_First( hOptData, NULL );
		
		pImpBuffer = (BYTE *)::SerializeDTree( hImpData, nImpSSize );
		if ( hIntOptionsData )
			pOptBuffer = (BYTE *)::SerializeDTree( hIntOptionsData, nOptSSize );

		HRESULT hResult = m_pInterface->PrototypeGenerate( nImpSSize, pImpBuffer, nOptSSize, pOptBuffer );
		if ( hResult != S_OK ) {
			CString rError;
			rError.Format( hResult == S_FALSE ? IDS_HGW_ERR_GENERATION_FALSE : IDS_HGW_ERR_GENERATION, LPCSTR(rIntName) );
			ShowGeneratorError( hResult, rError );
			bResult = false;
		}
		::CoTaskMemFree( pImpBuffer );
		::CoTaskMemFree( pOptBuffer );
	}
	return bResult;
}

// ---
HRESULT CHeaderGenWizard::LoadTypeTable() {
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
bool CHeaderGenWizard::PrepareAndSetGeneratorProjectInfo() {
	DWORD nImpSSize = 0;
	BYTE *pImpBuffer = NULL;
	bool  bResult = true;

	HDATA hRootData = ::DATA_Add( NULL, NULL, VE_PID_CGPI_ROOT, 0, 0 );
	if ( hRootData ) {
		::DATA_Add_Prop( hRootData, NULL, VE_PID_CGPI_PUBLICDIR, AVP_dword(LPCTSTR(m_rcPublicDirName)), 0 );
		::DATA_Add_Prop( hRootData, NULL, VE_PID_CGPI_PRIVATEDIR, AVP_dword(LPCTSTR(m_rcPrivateDirName)), 0 );
		
		pImpBuffer = (BYTE *)::SerializeDTree( hRootData, nImpSSize );
		::DATA_Remove( hRootData, NULL );
	}

	HRESULT hResult = m_pInterface->StartProject( nImpSSize, pImpBuffer );
	if ( hResult != S_OK ) {
		CString rError;
		rError.Format( IDS_HGW_ERR_INITFAILED );
		ShowGeneratorError( hResult, rError );
		bResult = false;
	}
	
	::CoTaskMemFree( pImpBuffer );

	return bResult;
}

// ---
void CHeaderGenWizard::PrepareAndLaunchGenerator() {
	if ( m_pInterface ) {
		if ( m_hWorkOptTree ) {
			bool bError = false;
		
			CWaitCursor rWait;

			HRESULT hResult = LoadTypeTable();
			if ( FAILED(hResult) ) {
				CString rError;
				rError.LoadString( IDS_HGW_ERR_SETTYPES );
				ShowGeneratorError( hResult, rError );
				bError = true;
			}
			else {
				if ( PrepareAndSetGeneratorProjectInfo() ) {

					HDATA hCurrData = ::DATA_Get_First( m_hWorkOptTree, NULL );
					while ( hCurrData ) {
						CString rIntName;
						HDATA hIntData = NULL;
						PrepareImpTree( m_hImpData, hIntData, hCurrData, rIntName );
						bError |= !LaunchGenerator( hIntData, hCurrData, rIntName );
						hCurrData = ::DATA_Get_Next( hCurrData, NULL );
					}

					m_pInterface->EndProject();

					::DlgMessageBox( GetParent(), bError ? IDS_HGW_ERR_SOMEERROR : IDS_HGW_ERR_SUCCEEDED );
				}
			}
		}
	}
	else {
		if ( m_hOptFileData ) {
			HPROP hCGClsIDProp = ::DATA_Find_Prop( m_hOptFileData, NULL, VE_PID_CGIOGENERATORCLSID );
			if ( hCGClsIDProp ) {
				::PROP_Get_Val( hCGClsIDProp, &m_rCLSID, sizeof(m_rCLSID) );
				HRESULT hResult;
				m_pInterface = (IPragueCodeGen *)::COMLoadCOMObjectID( m_rCLSID, IID_IPragueCodeGen, &hResult );
				if ( m_pInterface ) {
					GetInterfaceOptionsPattern();
					PrepareAndLaunchGenerator();
				}
				else {
					CString rcError;
					TCHAR pBuffer[40];
					::StringFromGUID( m_rCLSID, pBuffer, sizeof(pBuffer) );
					rcError.Format( IDS_ERR_LOADINTERFACEFAILED, pBuffer );
					CAPointer<TCHAR> pErrText = ::GetErrorString( hResult );
					rcError += "\n\r";
					rcError += pErrText;

					::DlgMessageBox( this, rcError );
				}
			}
		}
	}
}

BEGIN_MESSAGE_MAP(CHeaderGenWizard, CDialog)
	//{{AFX_MSG_MAP(CHeaderGenWizard)
	ON_BN_CLICKED(IDC_HGW_NEXT, OnHgwNext)
	ON_BN_CLICKED(IDC_HGW_BACK, OnHgwBack)
	ON_BN_CLICKED(IDC_HGW_FINISH, OnHgwFinish)
	ON_COMMAND(IDOK, OnOK)
	ON_WM_SIZE()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CHeaderGenWizard message handlers
// ---
BOOL CHeaderGenWizard::OnInitDialog() {
	CDialog::OnInitDialog();
	
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
void CHeaderGenWizard::OnOK() {
}

// ---
void CHeaderGenWizard::OnHgwNext() {
	m_nDirection = 1;
  SetEvent( 1 );
}

// ---
void CHeaderGenWizard::OnHgwBack() {
  m_nDirection = -1;
  SetEvent( 0 );
}

// ---
void CHeaderGenWizard::OnHgwFinish() {
  SetEvent( 2 );
}


void CHeaderGenWizard::OnSize(UINT nType, int cx, int cy) 
{
  CDialog::OnSize(nType, cx, cy);
  
  static RULE rules[] = {
    // Action    Act-on                                 Relative-to                      Offset
    // ------    ------                                 -----------                      ------
    //  {  lSTRETCH, lBOTTOM(IDC_HGG_CTTTREE),           lTOP(IDC_G_OBJECTS),                -4  },
    
    {  lMOVE,    lBOTTOM (IDC_HGW_BACK),                 lBOTTOM(lPARENT),                   -8  },
    {  lMOVE,    lBOTTOM (IDC_HGW_NEXT),                 lBOTTOM(lPARENT),                   -8  },
    {  lMOVE,    lBOTTOM (IDC_HGW_FINISH),               lBOTTOM(lPARENT),                   -8  },
    {  lMOVE,    lBOTTOM (IDCANCEL),                     lBOTTOM(lPARENT),                   -8  },

    {  lMOVE,    lRIGHT  (IDCANCEL),                     lRIGHT(lPARENT),                    -8  },
    {  lMOVE,    lRIGHT  (IDC_HGW_FINISH),               lLEFT (IDCANCEL),                   -2  },
    {  lMOVE,    lRIGHT  (IDC_HGW_BACK),                 lLEFT (IDC_HGW_FINISH),             -2  },
    {  lMOVE,    lRIGHT  (IDC_HGW_NEXT),                 lLEFT (IDC_HGW_BACK),               -2  },

    {  lSTRETCH, lBOTTOM (IDC_HGW_DLGFRAME),             lTOP(IDCANCEL),                     -10 },
    {  lSTRETCH, lRIGHT  (IDC_HGW_DLGFRAME),             lRIGHT(lPARENT),                    -8  },
    
    {  lEND                                                                                      }
  };
  
  Layout_ComputeLayout( GetSafeHwnd(), rules );
}
