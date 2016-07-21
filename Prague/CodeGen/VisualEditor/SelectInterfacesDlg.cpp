// SelectInterfacesDlg.cpp : implementation file
//

#include "stdafx.h"
#include <stuff/layout.h>
#include <stuffio/ioutil.h>
#include "selectinterfacesdlg.h"
#include "visualeditor.h"
#include "visualeditordlg.h"
#include "codegentools.h"
#include <serialize/kldtser.h>
#include <avpcontrols/dirdialog.h>
#include "PVE_DT_UTILS.H"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

//---------------------------------------------------------------------------//
// ---						   
struct OIDescriptor 
{
	IPragueCodeGen *m_pInterface;
	CLSID           m_CLSID;
	OIDescriptor( const IPragueCodeGen *pInterface, const CLSID &rCLSID );
};

// ---
OIDescriptor::OIDescriptor( const IPragueCodeGen *pInterface, const CLSID &rCLSID ) {
	m_pInterface = const_cast<IPragueCodeGen *>(pInterface);
	m_CLSID = rCLSID;
}

//---------------------------------------------------------------------------//
#define CGPPD(item) ((OIDescriptor *)item->GetData())
#define COM_INITDIR_REG_VALUE			_T("COMInitialDir")

void PrepareFilterIndex( OPENFILENAME &pOfn, const char *pDefExt );

//---------------------------------------------------------------------------//
// ---

// ---
static DWORD g_nObjectsHeaderIDTable[] = {
	IDS_CG_OD_DESCRIPTION,
	IDS_CG_OD_VENDOR			,
	IDS_CG_OD_VERSION		
};

static int g_nObjectsHeaderIDTableCount = sizeof(g_nObjectsHeaderIDTable) / sizeof(g_nObjectsHeaderIDTable[0]);

// ---
static UINT g_StrIDTable[][2] = {
	{ IDS_CGO_DEFEXTENTION, IDS_CGO_FILTER },
	{ IDS_HGO_DEFEXTENTION, IDS_HGO_FILTER },
};

// ---
char *g_RegValueNameTable[][5] = {
	{ "RecentSrcOptSession", "RecentSrcOptFile", IMP_INITDIR_REG_VALUE, IMP_INITDIR_REG_VALUE, "RecentHdrPrivateDir" },
	{ "RecentHdrOptSession", "RecentHdrOptFile", "RecentHdrOutputDir" , PRT_INITDIR_REG_VALUE, "RecentHdrPrivateDir" },
};

// ---
static int GetDataSelectedDatasCount( HDATA hData ) {
	int nCount = 0;
	
	HPROP hSelectedProp = ::DATA_Find_Prop( hData, NULL, VE_PID_IF_SELECTED );
	if ( hSelectedProp ) 
		nCount += !!::GetPropNumericValueAsDWord( hSelectedProp );
	
	if ( hData ) {
		HDATA hCurrData = ::DATA_Get_First( hData, NULL );
		while ( hCurrData ) {
			hSelectedProp = ::DATA_Find_Prop( hCurrData, NULL, VE_PID_IF_SELECTED );
			if ( hSelectedProp ) 
				nCount += !!::GetPropNumericValueAsDWord( hSelectedProp );
			hCurrData = ::DATA_Get_Next( hCurrData, NULL );
		}
	}
	return nCount;
}

/////////////////////////////////////////////////////////////////////////////
// CSelectInterfacesDlg dialog
CSelectInterfacesDlg::CSelectInterfacesDlg(
	HDATA            hImpData, 
	AVP_dword        idSelectedIFace, 
	HDATA&           hRestoredData, 
	CString&         rcFileName, 
	CString&         rcPublicDirName,
	CString&         rcPrivateDir,
	int              nRegime, 
	IPragueCodeGen*& pInterface, 
	CLSID&           rCLSID, 
	CWnd&            rcFrame, 
	CWnd*            pParent /*=NULL*/
)
	: CDialog(CSelectInterfacesDlg::IDD, pParent),
	m_hImpData( hImpData ),
	m_idSelectedIFace( idSelectedIFace ),
	m_hRestoredData( hRestoredData ),
	m_rcFileName( rcFileName ),
	m_rcPublicDir( rcPublicDirName ),
	m_rcPrivateDir( rcPrivateDir ),
	m_RegStorage( CFG_REG_KEY ),
	m_nRegime( nRegime ),
	m_rcFrame( rcFrame ),
	m_pInterface( pInterface ),
	m_rCLSID( rCLSID )
{
	//{{AFX_DATA_INIT(CSelectInterfacesDlg)
	m_FileName = _T("");
	m_bNewSession = TRUE;
	m_LoadStatus = _T("");
	m_PublicDir = m_rcPublicDir;
	m_PrivateDir = _T("");
	//}}AFX_DATA_INIT

	CAPointer<char> pName = new char[_MAX_PATH];
	*pName = 0;
	
	if ( !m_RegStorage.GetValue(g_RegValueNameTable[m_nRegime][2], pName, _MAX_PATH) ) 
		m_RegStorage.GetValue( g_RegValueNameTable[m_nRegime][3], pName, _MAX_PATH );
	
	m_PublicDir = m_rcPublicDir = pName;
	
	*pName = 0;
	m_RegStorage.GetValue(g_RegValueNameTable[m_nRegime][4], pName, _MAX_PATH);
	
	if(!IsProtoMode())
		m_PrivateDir = m_rcPrivateDir = pName;

	m_CTTControl.SetEnableAutoScanForEnable( false );

	m_pInterface = NULL;
	m_nSelected  = 0;
	
	m_Objects.SetShowHeaderOnEmpty( true );
	m_Objects.SetUseCheckSemantic( false );
	m_Objects.SetShowSelectionAlways( true );
	
	m_RegStorage.GetValue("LastCodeGenChoosenClassId", (char*) &m_rCLSID, sizeof(m_rCLSID));
}

CSelectInterfacesDlg::~CSelectInterfacesDlg()
{
	if(m_rCLSID.Data1 || m_rCLSID.Data2 || m_rCLSID.Data3 || m_rCLSID.Data4)
		m_RegStorage.PutValue("LastCodeGenChoosenClassId", (BYTE*) &m_rCLSID, sizeof(m_rCLSID));
}

void CSelectInterfacesDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CSelectInterfacesDlg)
	DDX_Control(pDX, IDC_HGG_CTTTREE, m_CTTControl);
	DDX_Control(pDX, IDC_CGOF_BROWSE, m_BrowseBtn);
	DDX_Control(pDX, IDC_CGOF_FILENAME, m_FileNameEdit);
	DDX_Text(pDX, IDC_CGOF_FILENAME, m_FileName);
	DDX_Text(pDX, IDC_CGOF_LOADSTATUS, m_LoadStatus);
	DDX_Text(pDX, IDC_CGOF_OUTPUTDIR, m_PublicDir);
	DDX_Text(pDX, IDC_CGOF_INCLUDEDIR, m_PrivateDir);
	DDX_Control(pDX, IDC_CG_OBJECTS, m_Objects);
	//}}AFX_DATA_MAP
}

// ---
void  CSelectInterfacesDlg::SetState( int s ) {
	CFAutoParam::SetState( s );
	switch ( m_State ) {
		case ps_nrm :
			if ( !GetSafeHwnd() ) {
				Create( CSelectInterfacesDlg::IDD, m_pParentWnd );

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
void CSelectInterfacesDlg::
LoadPluginStuff( HDATA hPluginData, CCTItemsArray &newItems, int nFirstItemLevel ) {

	CControlTreeItem *item = new CControlTreeItem;
	item->SetText( "files for module" );
	item->SetLevel( nFirstItemLevel );
	item->SetCanBeRemoved( false );
	item->SetCheckAlias();

	AVP_bool bChecked = FALSE;
	HPROP hCheckedProp = ::DATA_Find_Prop( hPluginData, NULL, VE_PID_IF_SELECTED );

	if ( hCheckedProp )
		bChecked = !!::GetPropNumericValueAsDWord( hCheckedProp );

	// in that case we assume that plugin selected
	if(!bChecked && !m_idSelectedIFace)
		bChecked = TRUE;

	item->Check( !!bChecked );

	if ( hCheckedProp )
		::PROP_Set_Val( hCheckedProp, AVP_dword(bChecked), 0 );
	else
		hCheckedProp = 
		::DATA_Add_Prop( hPluginData, NULL, VE_PID_IF_SELECTED, AVP_dword(bChecked), 0 );

	CPPDescriptor *pDescr = new CPPDescriptor( hPluginData, hCheckedProp );
	pDescr->m_nSpecialType = sdt_Plugin | sdt_Label;

	item->SetData( (void *)pDescr, false );
	item->SetExpanded(TRUE);
		
	newItems.Add( item );
}

// ---
void CSelectInterfacesDlg::LoadOneInterfaceStuff( HDATA hInterfaceData, CCTItemsArray &newItems, int nFirstItemLevel ) {
	bool bLinkedToFile = !!::IsLinkedToFile( hInterfaceData );
	
	if ( bLinkedToFile )
		return;
	
	HPROP hProp = ::DATA_Find_Prop( hInterfaceData, NULL, VE_PID_IF_INTERFACENAME );
	CAPointer<char> pValue;
	if ( hProp )
		pValue = ::GetPropValueAsString( hProp, NULL );
	
	CControlTreeItem *item;
	
	item = new CControlTreeItem;
	item->SetText( LPCSTR(pValue) );
	item->SetLevel( nFirstItemLevel );
	item->SetCanBeRemoved( false );
	
	if ( !bLinkedToFile ) {
		item->SetCheckAlias();
		
		AVP_bool bChecked = FALSE;
		HPROP hCheckedProp = ::DATA_Find_Prop( hInterfaceData, NULL, VE_PID_IF_SELECTED );
		if ( hCheckedProp )
			bChecked = !!::GetPropNumericValueAsDWord( hCheckedProp );
		if ( !bChecked && m_idSelectedIFace > 0 ) {
			AVP_dword dIFaceID = 0;
			HPROP hIDProp = ::DATA_Find_Prop( hInterfaceData, NULL, VE_PID_IF_DIGITALID );
			if ( hIDProp )
				dIFaceID = ::GetPropNumericValueAsDWord( hIDProp );
			bChecked = (dIFaceID == m_idSelectedIFace);
		}
		
		item->Check( !!bChecked );
		
		if ( hCheckedProp )
			::PROP_Set_Val( hCheckedProp, AVP_dword(bChecked), 0 );
		else
			::DATA_Add_Prop( hInterfaceData, NULL, VE_PID_IF_SELECTED, AVP_dword(bChecked), 0 );
	}
	else
		item->SetSimpleStringAlias();
	
	CPPDescriptor *pDescr = new CPPDescriptor( hInterfaceData, hProp );
	pDescr->m_nSpecialType = sdt_Interface | sdt_Label;
	
	item->SetData( (void *)pDescr, false );
	
	newItems.Add( item );
}

// ---
BOOL CSelectInterfacesDlg::DestroyWindow() {

	ClearGeneratorsListData();
	ClearInterfacesListData();
	::CoFreeUnusedLibraries();
	return CDialog::DestroyWindow();
}

// ---
void CSelectInterfacesDlg::FillGeneratorsList(){

	ClearGeneratorsListData();

	CGT::s_CodeGens.LoadCodeGenItemsCache();

	m_nSelected = 0;

	CCLItemsArray *pItems = new CCLItemsArray( true );

	// fill header for list with code generators
	for(BYTE i=0; i < CGT::s_CodeGens.m_nRealCodeGenItems; i++){

		CGT::SCodeGenItem& rItem = CGT::s_CodeGens.m_CodeGenItemsCache[i];

		CCheckListItem *item = new CCheckListItem;
		item->SetText( rItem.m_strDescription );
		item->SetSubItemsTexts( rItem.m_oAttrs );
		item->SetData( new OIDescriptor(NULL, rItem.m_oClsId), false );
		pItems->Add( item );
		
		if ( m_rCLSID == rItem.m_oClsId )
			m_nSelected = i;
	}
	
	m_Objects.SetItemsArray( pItems );

	if ( pItems->GetSize() )
		m_Objects.SetItemSelected( m_nSelected );
	else
		m_Objects.SetItemSelected( -1 );

	m_Objects.SetFocus();
}

// ---
void CSelectInterfacesDlg::ClearInterfacesListData(){
	
	CCTItemsArray *pItems = m_CTTControl.GetItemsArray();
	if ( !pItems ) 
		return;
	
	for(int i=0; i<pItems->GetSize(); i++){
		CCheckTreeItem *item = (*pItems)[i];
		CPPDescriptor* pData = ((CPPDescriptor*)item->GetData());
		if(pData)delete pData;
		item->SetData(NULL, FALSE);
	}
}

// ---
void CSelectInterfacesDlg::ClearGeneratorsListData(){

	CCLItemsArray *pItems = m_Objects.GetItemsArray();
	if ( !pItems ) 
		return;
	
	for(int i=0; i<pItems->GetSize(); i++){

		CCheckListItem *item = (*pItems)[i];
		OIDescriptor* pData = CGPPD(item);
		if(pData)delete pData;
		item->SetData(NULL, FALSE);
	}
}

// ---
bool CSelectInterfacesDlg::SelectGeneratorModule( CString &rcFileName ) {
	CString rcExtString;
	rcExtString.LoadString( IDS_CG_DEFEXTENTION );
	
	CString rcFltrString;
	rcFltrString.LoadString( IDS_CG_FILTER );
	
	CString rcInitDirRegValue = COM_INITDIR_REG_VALUE;
	
	CFileDialog rcOpenDlg( TRUE, rcExtString, NULL, OFN_HIDEREADONLY | OFN_PATHMUSTEXIST, rcFltrString, this );
	
	char pCurrDir[_MAX_PATH];
	char pFileName[_MAX_PATH];
	
	CRegStorage	rcRegStorage( CFG_REG_KEY );

	if ( rcFileName.IsEmpty() ) {
		if ( !rcRegStorage.GetValue(rcInitDirRegValue, pCurrDir, sizeof(pCurrDir)) ) 
			::IOSGetCurrentDirectory( pCurrDir, sizeof(pCurrDir) );
		
		rcOpenDlg.m_ofn.lpstrInitialDir = pCurrDir;
	}
	else {
		::IOSDirExtract( rcFileName, pCurrDir, sizeof(pCurrDir) );
		rcOpenDlg.m_ofn.lpstrInitialDir = pCurrDir;
		
		::IOSFileExtract( rcFileName, pFileName, _MAX_PATH );
		rcOpenDlg.m_ofn.lpstrFile = pFileName;
		rcOpenDlg.m_ofn.nMaxFile = sizeof(pFileName);
	}
	
	::PrepareFilterIndex( rcOpenDlg.m_ofn, rcExtString );
	
	bool bResult = rcOpenDlg.DoModal() == IDOK;
	
	if ( bResult ) {
		rcFileName = rcOpenDlg.GetPathName();
		
		::IOSDirExtract( rcFileName, pCurrDir, sizeof(pCurrDir) );
		rcRegStorage.PutValue( rcInitDirRegValue, pCurrDir );
	}
	
	return bResult;
}

//---
void  CSelectInterfacesDlg::OnNewOptionsFile( LPCTSTR i_strFileName ) {

	// this can be from new_session and save_options sequence, 
	BOOL bOptFileLoadedAlready = 
		CString(i_strFileName) == CString(CGT::s_OptFile.GetPathname());

	if(!bOptFileLoadedAlready){
		// set status - we are trying to load	
		m_FileName.Empty();
		m_LoadStatus.LoadString( IDS_CGOF_NOTLOADED );
		UpdateData( FALSE );
	
		// load optfile and check its proper structure
		BOOL bSucceededLoad = CGT::s_OptFile.Load(i_strFileName, !m_nRegime);
		if(!bSucceededLoad){
			CString rcString;
			rcString.Format( IDS_ERR_READFILE, i_strFileName );
			::DlgMessageBox(this, rcString );
			return;
		}
	}
	
	m_FileName = m_rcFileName = i_strFileName;
	m_LoadStatus.LoadString( IDS_CGOF_LOADED );
	
	m_RegStorage.PutValue( g_RegValueNameTable[m_nRegime][1], LPCSTR(m_FileName) );
	
	if(!bOptFileLoadedAlready)
		UpdateGeneratorsOutputDir();

	UpdateButtonsState();

	UpdateData(FALSE);
}

//---
void  CSelectInterfacesDlg::UpdateGeneratorsOutputDir() {

	TCHAR strDefOutput[1024];
	m_RegStorage.GetValue( g_RegValueNameTable[m_nRegime][2], strDefOutput, 1024 );

	CGT::s_CodeGens.UpdateGeneratorsOptions(CGT::s_OptFile, strDefOutput);
	UpdateCurrentGeneratorOutputDir();
}

//---
void CSelectInterfacesDlg::UpdateCurrentGeneratorOutputDir(){

	// change output dir for selected generator
	CGT::SCodeGenItem* pCacheItem = CGT::s_CodeGens.GetGenCacheItem(m_rCLSID);
	if(pCacheItem){

		BOOL bImplemetation = !m_nRegime;

		if(!bImplemetation)
		{
			m_rcPublicDir = m_PublicDir = pCacheItem->m_strPublicDir;
			m_rcPrivateDir = m_PrivateDir = pCacheItem->m_strPrivateDir;
		}
		else
		{
			CString strPluginName = CGT::GetPluginName(m_hImpData);
			CGT::SCodeGenPlugin* pPlugin = pCacheItem->FindPluginData(strPluginName);
			if(pPlugin)
			{
				m_rcPublicDir = m_PublicDir = pPlugin->m_strPublicDir;
				m_rcPrivateDir = m_PrivateDir = pPlugin->m_strPrivateDir;
			}
			else
			{
				m_rcPublicDir = m_PublicDir = _T("");
				m_rcPrivateDir = m_PrivateDir = _T("");
			}
		}

		UpdateData(FALSE);
	}
}

//---
void CSelectInterfacesDlg::UpdateButtonsState(){

	CFAutomaton *pAutomat = dynamic_cast<CFAutomaton *>(m_pParentWnd);
	if(!pAutomat)	return;
	CFAutoParamArray& rAuto = *pAutomat->GetParamArray();
	
	// we must check at least one interface selected
	DWORD dwSelectedEntities = ::GetDataSelectedDatasCount( m_hImpData );

	// and output dir entered
	BOOL bOutputDir = lstrlen(m_rcPublicDir);
	BYTE nCodeGensCount = CGT::s_CodeGens.m_nRealCodeGenItems;

	CFAPState eNextFinishState = ps_und;

	if(dwSelectedEntities && bOutputDir && nCodeGensCount)
		eNextFinishState = ps_nrm;

	rAuto[CGT::EAutoBackButton]->SetState( ps_und );
	rAuto[CGT::EAutoNextButton]->SetState( eNextFinishState );
	rAuto[CGT::EAutoFinishButton]->SetState( eNextFinishState );
}

//---
BOOL CSelectInterfacesDlg::IsProtoMode()
{
	return m_nRegime ? TRUE : FALSE;
}


//---
BEGIN_MESSAGE_MAP(CSelectInterfacesDlg, CDialog)
	//{{AFX_MSG_MAP(CSelectInterfacesDlg)
	ON_WM_SIZE()
	ON_MESSAGE( CTTM_WINDOWSHUTDOWN, OnCTTWindowShutDown )
	ON_MESSAGE( CTTM_CHECKEDCHANGING, OnCTTCheckedChanging )
	ON_BN_CLICKED(IDC_CGOF_NEWSESSION_RBTN, OnNewSession)
	ON_BN_CLICKED(IDC_CGOF_USEFILE_RBTN, OnUseFile)
	ON_BN_CLICKED(IDC_CGOF_BROWSE, OnBrowse)
	ON_BN_CLICKED(IDC_CGOF_OUTPUTBROWSE, OnOutputBrowse)
	ON_BN_CLICKED(IDC_CGOF_INCLUDEBROWSE, OnIncludeBrowse)
	ON_WM_DESTROY()
	ON_EN_CHANGE(IDC_CGOF_OUTPUTDIR, OnChangeOutputDir)
	ON_EN_CHANGE(IDC_CGOF_INCLUDEDIR, OnChangePrivateDir)
	ON_NOTIFY(LVN_ITEMCHANGED, IDC_CG_OBJECTS, OnItemChanged)
	ON_BN_CLICKED(IDC_CG_ADDGENERATOR, OnAddGenerator)
	ON_BN_CLICKED(IDC_CG_REMOVE_GENERATOR, OnUnregGenerator)
	ON_BN_CLICKED(IDC_CG_UPDATE_GENERATOR_LIST, OnCgUpdateGeneratorList)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CSelectInterfacesDlg message handlers

// ---
BOOL CSelectInterfacesDlg::OnInitDialog() {

	BOOL bResult = CDialog::OnInitDialog();
	
	int nFirstItemLevel = 1;
	CCTItemsArray &newItems = *new CCTItemsArray( true );

	if(!m_nRegime)
		LoadPluginStuff(m_hImpData, newItems, nFirstItemLevel);

	HDATA hCurrData = ::DATA_Get_First( m_hImpData, NULL );
	while ( hCurrData ) {

		if(!IsPluginStaticInterfaceRoot(hCurrData))
			LoadOneInterfaceStuff( hCurrData, newItems, nFirstItemLevel );
		hCurrData = ::DATA_Get_Next( hCurrData, NULL );
	}

	m_idSelectedIFace = -1;

	m_CTTControl.SetItemsArray( &newItems );
	
	m_RegStorage.GetValue( g_RegValueNameTable[m_nRegime][0], (DWORD &)m_bNewSession );
	
	DWORD dwStyle = ListView_GetExtendedListViewStyle( m_Objects );
	dwStyle |= LVS_EX_FULLROWSELECT;// | LVS_EX_GRIDLINES;
	ListView_SetExtendedListViewStyle( m_Objects, dwStyle );

	// set generators list header
	CCLColumnsArray *pColumnsArray = new CCLColumnsArray( true );
	for ( int i=0; i<g_nObjectsHeaderIDTableCount; i++ ) {
		CCheckListColumn *pColumn = new CCheckListColumn();
		CString rString;
		rString.LoadString( g_nObjectsHeaderIDTable[i] );
		pColumn->SetName( rString );
		pColumn->SetAlignment( CCheckListColumn::ca_Left );
		pColumnsArray->Add( pColumn );
	}
	
	m_Objects.SetColumnsArray( pColumnsArray );

	FillGeneratorsList();

	if ( m_bNewSession ) {
			CheckDlgButton( IDC_CGOF_NEWSESSION_RBTN, BST_CHECKED );
			OnNewSession();
	}
	else {
			CheckDlgButton( IDC_CGOF_USEFILE_RBTN, BST_CHECKED );
			OnUseFile();
	}

	if(IsProtoMode())
	{
		GetDlgItem(IDC_CGOF_INCLUDEDIR)->EnableWindow(FALSE);	
		GetDlgItem(IDC_CGOF_INCLUDEBROWSE)->EnableWindow(FALSE);	
	}

	return bResult;  
}


// ---
void CSelectInterfacesDlg::OnSize(UINT nType, int cx, int cy) {
	CDialog::OnSize(nType, cx, cy);
	
  static RULE rules[] = {
    // Action    Act-on                                 Relative-to                      Offset
    // ------    ------                                 -----------                      ------
//  {  lSTRETCH, lBOTTOM(IDC_HGG_CTTTREE),              lTOP(IDC_G_OBJECTS),                -4  },
    
    {  lMOVE,    lBOTTOM (IDC_FOLDER_BOX),               lBOTTOM(lPARENT),                   -2  },
    {  lMOVE,    lBOTTOM (IDC_OPTIONS_BOX),              lTOP   (IDC_FOLDER_BOX),            -2  },
    {  lMOVE,    lBOTTOM (IDC_G_OBJECTS),                lTOP   (IDC_OPTIONS_BOX),           -2  },
    {  lSTRETCH, lBOTTOM (IDC_HGG_CTTTREE),              lTOP   (IDC_G_OBJECTS),             -2  },

    {  lSTRETCH, lRIGHT  (IDC_FOLDER_BOX),               lRIGHT(lPARENT),                    -2  },
    {  lSTRETCH, lRIGHT  (IDC_OPTIONS_BOX),              lRIGHT(lPARENT),                    -2  },
    {  lSTRETCH, lRIGHT  (IDC_G_OBJECTS),                lRIGHT(lPARENT),                    -2  },
    {  lSTRETCH, lRIGHT  (IDC_HGG_CTTTREE),              lRIGHT(lPARENT),                    -2  },
    
    {  lMOVE,    lTOP    (IDC_CG_ADDGENERATOR),          lTOP(IDC_G_OBJECTS),                +10 },
    {  lMOVE,    lTOP    (IDC_CG_REMOVE_GENERATOR),      lBOTTOM(IDC_CG_ADDGENERATOR),       +2  },
    {  lMOVE,    lTOP    (IDC_CG_UPDATE_GENERATOR_LIST), lBOTTOM(IDC_CG_REMOVE_GENERATOR),   +2  },
    {  lMOVE,    lTOP    (IDC_CG_OBJECTS),               lTOP(IDC_G_OBJECTS),                +4  },
    {  lSTRETCH, lBOTTOM (IDC_CG_OBJECTS),               lBOTTOM(IDC_G_OBJECTS),             -2  },
    {  lSTRETCH, lRIGHT  (IDC_CG_OBJECTS),               lRIGHT(IDC_G_OBJECTS),              -2  },
    
    {  lMOVE,    lTOP    (IDC_CGOF_USEFILE_RBTN),        lTOP (IDC_OPTIONS_BOX),             +10 },
    {  lMOVE,    lTOP    (IDC_CGOF_FILENAME),            lTOP(IDC_CGOF_USEFILE_RBTN),        -2  },
    {  lMOVE,    lTOP    (IDC_CGOF_BROWSE),              lTOP(IDC_CGOF_USEFILE_RBTN),        -2  },
    {  lMOVE,    lTOP    (IDC_CGOF_NEWSESSION_RBTN),     lBOTTOM(IDC_CGOF_USEFILE_RBTN),     +6  },
    {  lMOVE,    lTOP    (IDC_CGOF_LOADSTATUS),          lTOP(IDC_CGOF_NEWSESSION_RBTN),     +0  },

    {  lMOVE,    lRIGHT  (IDC_CGOF_BROWSE),              lRIGHT(IDC_OPTIONS_BOX),            -2  },
    {  lSTRETCH, lRIGHT  (IDC_CGOF_FILENAME),            lLEFT(IDC_CGOF_BROWSE),             -2  },
    {  lSTRETCH, lRIGHT  (IDC_CGOF_LOADSTATUS),          lRIGHT(IDC_OPTIONS_BOX),            -2  },
    
    {  lMOVE,    lTOP    (IDC_S_OUTPUT),                 lTOP(IDC_FOLDER_BOX),               +10 },
    {  lMOVE,    lTOP    (IDC_CGOF_OUTPUTDIR),           lTOP(IDC_S_OUTPUT),                 -2  },
    {  lMOVE,    lTOP    (IDC_CGOF_OUTPUTBROWSE),        lTOP(IDC_S_OUTPUT),                 -2  },
    {  lMOVE,    lTOP    (IDC_S_INCLUDE),                lBOTTOM(IDC_S_OUTPUT),              +6  },
    {  lMOVE,    lTOP    (IDC_CGOF_INCLUDEDIR),          lTOP(IDC_S_INCLUDE),                +2  },
    {  lMOVE,    lTOP    (IDC_CGOF_INCLUDEBROWSE),       lTOP(IDC_S_INCLUDE),                +2  },
    
    {  lMOVE,    lRIGHT  (IDC_CGOF_OUTPUTBROWSE),        lRIGHT(IDC_FOLDER_BOX),             -2  },
    {  lSTRETCH, lRIGHT  (IDC_CGOF_OUTPUTDIR),           lLEFT(IDC_CGOF_OUTPUTBROWSE),       -2  },
    {  lMOVE,    lRIGHT  (IDC_CGOF_INCLUDEBROWSE),       lRIGHT(IDC_FOLDER_BOX),             -2  },
    {  lSTRETCH, lRIGHT  (IDC_CGOF_INCLUDEDIR),          lLEFT(IDC_CGOF_INCLUDEBROWSE),      -2  },
    
    {  lEND                                                                                      }
  };

  Layout_ComputeLayout( GetSafeHwnd(), rules );
}

// ---
void CSelectInterfacesDlg::OnDestroy() {

	m_RegStorage.PutValue( g_RegValueNameTable[m_nRegime][4], m_PrivateDir );
	m_RegStorage.PutValue( g_RegValueNameTable[m_nRegime][2], m_PublicDir );

	CDialog::OnDestroy();
}

// ---
LRESULT CSelectInterfacesDlg::OnCTTWindowShutDown( WPARAM wParam, LPARAM lParam ) {

	ClearGeneratorsListData();

	CCTItemsArray &newItems = *new CCTItemsArray( true );
	m_CTTControl.SetItemsArray( &newItems );

	return 0;
}

// —ообщение об намерении установить Checked
// WPARAM		- LPBOOL - указатель на результат - не используетс€
// LPARAM		- указатель на экземпл€р структуры CTTMessageInfo
//						m_Value.m_bSetRemove - TRUE - установить, FALSE - сн€ть
// LRESULT	- не используетс€
// ---
LRESULT CSelectInterfacesDlg::OnCTTCheckedChanging( WPARAM wParam, LPARAM lParam ) {

	CTTMessageInfo *info = (CTTMessageInfo *)lParam;

	CCTItemsArray &items = *m_CTTControl.GetItemsArray();
	
	int nItemInd         = info->m_nItemIndex;
 	CCheckTreeItem *item = items[nItemInd];
	HDATA hData = CPPD(item)->m_hData;
	HPROP hProp = CPPD(item)->m_hProp;

	// Check InterfaceName
	if ( ::IsIDProlog(m_CTTControl, nItemInd, VE_PID_IF_INTERFACENAME_ID) ) {
		HPROP hCheckedProp = ::DATA_Find_Prop( hData, NULL, VE_PID_IF_SELECTED );
		if ( hCheckedProp )
			::PROP_Set_Val( hCheckedProp, AVP_dword(info->m_Value.m_bSetRemove), 0 );
		else
			::DATA_Add_Prop( hData, NULL, VE_PID_IF_SELECTED, AVP_dword(info->m_Value.m_bSetRemove), 0 );

		UpdateButtonsState();
		
		return 0;
	}

	// plugin
	if( ::IsIDProlog(m_CTTControl, nItemInd, VE_PID_IF_SELECTED_ID) ){
		HPROP hCheckedProp = ::DATA_Find_Prop( hData, NULL, VE_PID_IF_SELECTED );
		if ( hCheckedProp )
			::PROP_Set_Val( hCheckedProp, AVP_dword(info->m_Value.m_bSetRemove), 0 );
		else
			::DATA_Add_Prop( hData, NULL, VE_PID_IF_SELECTED, AVP_dword(info->m_Value.m_bSetRemove), 0 );

		UpdateButtonsState();

		return 0;
	}

	return 0;
}

// ---
void CSelectInterfacesDlg::OnAddGenerator() {

	CString rcFileName;
	if ( SelectGeneratorModule(rcFileName) && CGT::RegisterCOM(rcFileName) ) {
		CGT::s_CodeGens.LoadCodeGenItemsCache(TRUE);
		FillGeneratorsList();
		UpdateButtonsState();
	}
}

// ---
void CSelectInterfacesDlg::OnUnregGenerator() {
	CCLItemsArray* pItems = m_Objects.GetItemsArray();
	
	if ( !pItems || (m_nSelected >= pItems->GetSize()) ) 
		return;
	
	CCheckListItem *item = (*pItems)[m_nSelected];
	if ( !item )
		return;
	
	LPOLESTR wstr;
	::StringFromCLSID( CGPPD(item)->m_CLSID, &wstr );
	
	char pFileName[_MAX_PATH];
	char skey[MAX_PATH];
	
	::lstrcpy( skey, _T("CLSID\\") );
	DWORD len = ::lstrlen( skey );
	::wcstombs( skey+len, wstr, sizeof(skey)-len );
	::CoTaskMemFree( wstr );
	::lstrcat( skey, "\\InprocServer32" );
	
	CRegStorage	rcRegStorage( HKEY_CLASSES_ROOT, skey );
	len = sizeof(pFileName);
	if ( !rcRegStorage.GetValue(0,pFileName,len) )
		return;
	
	if ( !CGT::UnregisterCOM(pFileName) )
		return;
	
	CGT::s_CodeGens.LoadCodeGenItemsCache(TRUE);
	FillGeneratorsList();
	UpdateButtonsState();
}

// ---
void CSelectInterfacesDlg::OnCgUpdateGeneratorList() {

	CGT::s_CodeGens.LoadCodeGenItemsCache(TRUE);
	FillGeneratorsList();
	UpdateButtonsState();
}

// ---
void CSelectInterfacesDlg::OnItemChanged(NMHDR* pNMHDR, LRESULT* pResult) {

	*pResult = 0;
	
	NM_LISTVIEW* pNMListView = (NM_LISTVIEW*)pNMHDR;
	if ( !(pNMListView->uChanged & LVIF_STATE) || !(pNMListView->uNewState & LVIS_FOCUSED) ||
		   pNMListView->uOldState ) 
		return;
	
	m_nSelected = pNMListView->iItem;
	
	CCLItemsArray *pItems = m_Objects.GetItemsArray();
	if ( !pItems || m_nSelected >= pItems->GetSize() ) 
		return;

	// change current generator
	CCheckListItem *item = (*pItems)[m_nSelected];
	m_rCLSID = CGPPD(item)->m_CLSID;
	
	// change output dir for selected generator
	UpdateCurrentGeneratorOutputDir();
	UpdateButtonsState();
}

// ---
void CSelectInterfacesDlg::OnChangeOutputDir() {
	UpdateData( TRUE );
	m_rcPublicDir = m_PublicDir;
	
	CGT::SCodeGenItem* pCacheItem = CGT::s_CodeGens.GetGenCacheItem(m_rCLSID);
	ASSERT(NULL != pCacheItem);

	BOOL bImplementation = !m_nRegime;

	if(!bImplementation)
	{
		pCacheItem->m_strPublicDir = m_rcPublicDir;
		pCacheItem->m_strPublicDir = m_rcPrivateDir;
	}
	else
	{
		CString strPluginName = CGT::GetPluginName(m_hImpData);
		CGT::SCodeGenPlugin* pPlugin = pCacheItem->FindPluginData(strPluginName);
		if(!pPlugin)
		{
			pPlugin = pCacheItem->AddNewPluginData();
			pPlugin->m_strPluginName = strPluginName;
		}

		pPlugin->m_strPublicDir = m_rcPublicDir = m_PublicDir;
		pPlugin->m_strPrivateDir = m_rcPrivateDir = m_PrivateDir;
	}

	UpdateButtonsState();
}

// ---
void CSelectInterfacesDlg::OnChangePrivateDir() {
	OnChangeOutputDir();
}

// ---
void CSelectInterfacesDlg::OnIncludeBrowse() {
	CString rcOutput = m_PublicDir;
	rcOutput += _T("\\");
	
	CString rcText = m_PublicDir;
	TCHAR *pBuffer = rcText.GetBuffer(_MAX_PATH);
	
	::IOSFullPathFromRelativePath( rcOutput, m_PrivateDir, pBuffer, _MAX_PATH );
	
	rcText.ReleaseBuffer();
	
	CDirDialog rcDlg( rcText, NULL, this );
	
	char pCurrDir[_MAX_PATH];
	::GetModuleFileName( NULL, pCurrDir, sizeof(pCurrDir)	);
	
	if ( rcText.IsEmpty() ) {
		rcDlg.m_ofn.lpstrInitialDir = pCurrDir;
	}
	else {
		::IOSFullPathFromRelativePath( pCurrDir, rcText, pCurrDir, sizeof(pCurrDir) );
		rcDlg.m_ofn.lpstrInitialDir = pCurrDir;
	}
	
	if ( rcDlg.DoModal() == IDOK ) {
		rcText = rcDlg.GetPath();
		rcText.TrimRight( L'\\' );
		TCHAR *pBuffer = rcText.GetBuffer(_MAX_PATH);
		
		::IOSRelativePathFromFullPath( rcOutput, rcText, pBuffer, _MAX_PATH );
		
		rcText.ReleaseBuffer();
		m_PrivateDir = m_rcPrivateDir = rcText;
		
		UpdateData( FALSE );
	}
}

// ---
void CSelectInterfacesDlg::OnOutputBrowse() {
	CString rcText = m_PublicDir;
	CDirDialog rcDlg( rcText, NULL, this );
	
	char pCurrDir[_MAX_PATH];
	::GetModuleFileName( NULL, pCurrDir, sizeof(pCurrDir));
	
	if ( rcText.IsEmpty() ) 
		rcDlg.m_ofn.lpstrInitialDir = pCurrDir;
	else{
		::IOSFullPathFromRelativePath( pCurrDir, rcText, pCurrDir, sizeof(pCurrDir) );
		rcDlg.m_ofn.lpstrInitialDir = pCurrDir;
	}
	
	if ( rcDlg.DoModal() == IDOK ) {
		rcText = rcDlg.GetPath();
		rcText.TrimRight( L'\\' );
		m_PublicDir = m_rcPublicDir = rcText;
	
		UpdateData( FALSE );
	}

	OnChangeOutputDir();
}

// ---
void CSelectInterfacesDlg::OnBrowse() {

	CString rcExtString;
	rcExtString.LoadString( g_StrIDTable[m_nRegime][0] );
	
	CString rcFltrString;
	rcFltrString.LoadString( g_StrIDTable[m_nRegime][1] );
	
	CFileDialog rcOpenDlg( TRUE, rcExtString, NULL, OFN_HIDEREADONLY, rcFltrString, this );
	
	if ( m_FileName.IsEmpty() ) {
		char pCurrDir[_MAX_PATH];
		::IOSGetCurrentDirectory( pCurrDir, sizeof(pCurrDir) );
		rcOpenDlg.m_ofn.lpstrInitialDir = pCurrDir;
	}
	else {
		char pCurrDir[_MAX_PATH];
		::IOSDirExtract( m_FileName, pCurrDir, sizeof(pCurrDir) );
		rcOpenDlg.m_ofn.lpstrInitialDir = pCurrDir;
		
		char pFileName[_MAX_PATH];
		::IOSFileExtract( m_FileName, pFileName, _MAX_PATH );
		rcOpenDlg.m_ofn.lpstrFile = pFileName;
		rcOpenDlg.m_ofn.nMaxFile = sizeof(pFileName);
	}
	
	::PrepareFilterIndex( rcOpenDlg.m_ofn, rcExtString );
	
	if ( rcOpenDlg.DoModal() == IDOK ) 
		OnNewOptionsFile( rcOpenDlg.GetPathName() );
}

// ---
void CSelectInterfacesDlg::OnNewSession() {

	m_FileName = m_rcFileName = "";
	CGT::s_OptFile.Close();

	m_FileNameEdit.EnableWindow( FALSE );
	m_BrowseBtn.EnableWindow( FALSE );
	m_LoadStatus = _T("");
	
	UpdateData( FALSE );
	
	CFAutomaton *pAutomat = dynamic_cast<CFAutomaton *>(m_pParentWnd);
	if ( pAutomat )
		(*pAutomat->GetParamArray())[2]->SetState( !m_bNewSession ? ps_nrm : ps_und );
	
	m_bNewSession = TRUE;
	m_RegStorage.PutValue( g_RegValueNameTable[m_nRegime][0], DWORD(m_bNewSession) );

	// clear output dirs
	CGT::s_CodeGens.UpdateGeneratorsOptions(CGT::s_OptFile, "");
	UpdateCurrentGeneratorOutputDir();

	UpdateButtonsState();
}

// ---
void CSelectInterfacesDlg::OnUseFile() {

	TCHAR strName[_MAX_PATH] = {0};
	m_RegStorage.GetValue( g_RegValueNameTable[m_nRegime][1], strName, _MAX_PATH );
	
	m_FileNameEdit.EnableWindow( TRUE );
	m_BrowseBtn.EnableWindow( TRUE );
	
	if ( lstrlen(strName) )
		OnNewOptionsFile( strName );
	
	m_bNewSession = FALSE;
	m_RegStorage.PutValue( g_RegValueNameTable[m_nRegime][0], DWORD(m_bNewSession) );

	UpdateButtonsState();
}

