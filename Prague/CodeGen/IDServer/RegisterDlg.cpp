// RegisterDlg.cpp : implementation file
//

#include "stdafx.h"
#include <Stuff\RegStorage.h>
#include <Stuff\CPointer.h>
#include <datatreeutils/dtutils.h>
#include "resource.h"
#include "RegisterDlg.h"
#include "SerDeser.h"
#include "..\IDServerInterface.h"
#include "PragueIDServer.h"
#include "AVPPisID.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CRegisterDlg dialog


CRegisterDlg::CRegisterDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CRegisterDlg::IDD, pParent),
	m_hRootData( NULL )
{
	//{{AFX_DATA_INIT(CRegisterDlg)
	m_VName = _T("");
	m_IRFrom = 1;
	m_IRTo = 0xffff;
	m_PRFrom = 1;
	m_PRTo = 0xffff;
	m_VId = 1;
	m_VMnemID = _T("");
	//}}AFX_DATA_INIT

	TCHAR pName[_MAX_PATH];
	DWORD dwSize = sizeof( pName );
	if ( ::GetUserName(pName, &dwSize) ) {
		m_VName = pName;
		m_VMnemID = "VID_";
		m_VMnemID += m_VName;
		m_VMnemID.MakeUpper();
	}
}


void CRegisterDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CRegisterDlg)
	DDX_Control(pDX, IDC_VENDORID_SPIN, m_VIdSpin);
	DDX_Control(pDX, IDC_PRANGE_TO_SPIN, m_PRToSpin);
	DDX_Control(pDX, IDC_IRANGE_TO_SPIN, m_IRToSpin);
	DDX_Control(pDX, IDC_PRANGE_FROM_SPIN, m_PRFromSpin);
	DDX_Control(pDX, IDC_IRANGE_FROM_SPIN, m_IRFromSpin);
	DDX_Text(pDX, IDC_VENDORNAME_EDIT, m_VName);
	DDX_Text(pDX, IDC_IRANGE_FROM_EDIT, m_IRFrom);
	DDV_MinMaxUInt(pDX, m_IRFrom, 1, 65535);
	DDX_Text(pDX, IDC_IRANGE_TO_EDIT, m_IRTo);
	DDV_MinMaxUInt(pDX, m_IRTo, 1, 65535);
	DDX_Text(pDX, IDC_PRANGE_FROM_EDIT, m_PRFrom);
	DDV_MinMaxUInt(pDX, m_PRFrom, 1, 65535);
	DDX_Text(pDX, IDC_PRANGE_TO_EDIT, m_PRTo);
	DDV_MinMaxUInt(pDX, m_PRTo, 1, 65535);
	DDX_Text(pDX, IDC_VENDORID_EDIT, m_VId);
	DDV_MinMaxUInt(pDX, m_VId, 1, 65535);
	DDX_Text(pDX, IDC_VENDORMNEMID_EDIT, m_VMnemID);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CRegisterDlg, CDialog)
	//{{AFX_MSG_MAP(CRegisterDlg)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CRegisterDlg message handlers
// ---
BOOL CRegisterDlg::OnInitDialog() {
	CDialog::OnInitDialog();
	
	CRegStorage  rcRegStorage( PRAGUE_ID_SERVER_KEY );

	BYTE *pBuffer = NULL;
	DWORD dwSize = 0;
	if ( rcRegStorage.GetValue(PRAGUE_ID_CONTAINER_VALUE, (BYTE *)NULL, dwSize) ) {
		pBuffer = new BYTE[dwSize];
		if ( rcRegStorage.GetValue(PRAGUE_ID_CONTAINER_VALUE, pBuffer, dwSize) ) {
			m_hRootData = ::DeserializeDTree( pBuffer, dwSize );
		}
		delete [] pBuffer;
	}

	if ( m_hRootData ) {
		HPROP hProp;

		hProp = ::DATA_Find_Prop( m_hRootData, NULL, PI_PID_VENDORNAME );
		if ( hProp ) {
			CAPointer<char> pName = ::DTUT_GetPropValueAsString( hProp, NULL );
			m_VName = pName;
		}

		hProp = ::DATA_Find_Prop( m_hRootData, NULL, NULL );
		if ( hProp ) 
			m_VId = ::DTUT_GetPropNumericValueAsDWord( hProp );

		hProp = ::DATA_Find_Prop( m_hRootData, NULL, PI_PID_VENDORMNEMONICID );
		if ( hProp ) {
			CAPointer<char> pName = ::DTUT_GetPropValueAsString( hProp, NULL );
			m_VMnemID = pName;
		}
		else {
			m_VMnemID = m_VName;
			m_VMnemID.MakeUpper();
		}

		MAKE_ADDR1(nIAddr, PI_PID_INTERFACEID);
		HDATA hData = ::DATA_Find( m_hRootData, nIAddr );
		if ( hData ) {
			hProp = ::DATA_Find_Prop( hData, NULL, PI_PID_IDRANGE );
			if ( hProp ) {
				m_IRFrom = ::DTUT_GetPropArrayNumericValueAsDWord( hProp, 0 );
				m_IRTo   = ::DTUT_GetPropArrayNumericValueAsDWord( hProp, 1 );
			}
		}

		MAKE_ADDR1(nPAddr, PI_PID_PLUGINID);
		hData = ::DATA_Find( m_hRootData, nIAddr );
		if ( hData ) {
			hProp = ::DATA_Find_Prop( hData, NULL, PI_PID_IDRANGE );
			if ( hProp ) {
				m_PRFrom = ::DTUT_GetPropArrayNumericValueAsDWord( hProp, 0 );
				m_PRTo   = ::DTUT_GetPropArrayNumericValueAsDWord( hProp, 1 );
			}
		}
	}

	m_VIdSpin.SetRange32( m_VId, 0xffff );
	m_VIdSpin.SetPos( m_VId );

	m_IRFromSpin.SetRange32( m_IRFrom, m_IRTo );
	m_IRFromSpin.SetPos( m_IRFrom );

	m_IRToSpin.SetRange32( m_IRFrom, m_IRTo );
	m_IRToSpin.SetPos( m_IRTo );

	m_PRFromSpin.SetRange32( m_PRFrom, m_PRTo );
	m_PRFromSpin.SetPos( m_PRFrom );

	m_PRToSpin.SetRange32( m_PRFrom, m_PRTo );
	m_PRToSpin.SetPos( m_PRTo );

	UpdateData( FALSE );

	return TRUE; 
}

// ---
void CRegisterDlg::OnOK() {
	UpdateData();

	if ( !m_hRootData ) {
		m_hRootData = ::DATA_Add( NULL, NULL, PI_PID_ROOT, 0, 0 );
		if ( m_hRootData ) {
			HDATA hData;
			::DATA_Add_Prop( m_hRootData, NULL, PI_PID_VENDORNAME, 0, 0 );
			::DATA_Add_Prop( m_hRootData, NULL, PI_PID_VENDORMNEMONICID, 0, 0 );

			hData = ::DATA_Add( m_hRootData, NULL, PI_PID_INTERFACEID, 0, 0 );
			::DATA_Add_Prop( hData, NULL, PI_PID_IDRANGE, 0, 0 );

			hData = ::DATA_Add( m_hRootData, NULL, PI_PID_PLUGINID, 0, 0 );
			::DATA_Add_Prop( hData, NULL, PI_PID_IDRANGE, 0, 0 );
		}
	}

	if ( m_hRootData ) {
		HPROP hProp;

		hProp = ::DATA_Find_Prop( m_hRootData, NULL, PI_PID_VENDORNAME );
		if ( hProp ) 
			::DTUT_SetPropValue( hProp, LPCTSTR(m_VName), NULL );

		hProp = ::DATA_Find_Prop( m_hRootData, NULL, PI_PID_VENDORMNEMONICID );
		if ( hProp ) 
			::DTUT_SetPropValue( hProp, LPCTSTR(m_VMnemID), NULL );

		hProp = ::DATA_Find_Prop( m_hRootData, NULL, NULL );
		if ( hProp ) 
			::DTUT_SetPropNumericValue( hProp, m_VId );

		MAKE_ADDR1(nIAddr, PI_PID_INTERFACEID);
		HDATA hData = ::DATA_Find( m_hRootData, nIAddr );
		if ( hData ) {
			hProp = ::DATA_Find_Prop( hData, NULL, PI_PID_IDRANGE );
			if ( hProp ) {
				::DTUT_SetPropArrayNumericValue( hProp, 0, m_IRFrom );
				::DTUT_SetPropArrayNumericValue( hProp, 1, m_IRTo );
			}
		}

		MAKE_ADDR1(nPAddr, PI_PID_PLUGINID);
		hData = ::DATA_Find( m_hRootData, nPAddr );
		if ( hData ) {
			hProp = ::DATA_Find_Prop( hData, NULL, PI_PID_IDRANGE );
			if ( hProp ) {
				::DTUT_SetPropArrayNumericValue( hProp, 0, m_PRFrom );
				::DTUT_SetPropArrayNumericValue( hProp, 1, m_PRTo );
			}
		}

		DWORD dwSize = 0;
		AVP_char *pBuffer = ::SerializeDTree( m_hRootData, dwSize );
		if ( pBuffer ) {
			CRegStorage  rcRegStorage( PRAGUE_ID_SERVER_KEY );
			rcRegStorage.PutValue( PRAGUE_ID_CONTAINER_VALUE, (BYTE *)pBuffer, dwSize );
			delete [] pBuffer;
		}
	}

	CDialog::OnOK();
}
