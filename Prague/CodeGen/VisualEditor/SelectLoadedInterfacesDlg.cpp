// SelectLoadedInterfacesDlg.cpp : implementation file
//

#include "stdafx.h"
#include <Stuff\Layout.h>
#include <Stuff\CPointer.h>
#include <Stuff\PathStr.h>


#include "VisualEditor.h"
#include "VisualEditorDlg.h"
#include "SelectLoadedInterfacesDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

// ---
static BOOL IsSelectedPresented( HDATA hRootData ) {
	HDATA hCurrData = ::DATA_Get_First( hRootData, NULL );
	while ( hCurrData ) {
		HDATA hCheckData = hCurrData;
		hCurrData = ::DATA_Get_Next( hCurrData, NULL );

		HPROP hCheckedProp = ::DATA_Find_Prop( hCheckData, NULL, VE_PID_IF_SELECTED );
		if ( hCheckedProp && ::GetPropNumericValueAsDWord(hCheckedProp) )
			return TRUE;
	}
	return FALSE;
}

// ---
static void ProcessDataForSelected( HDATA hRootData ) {
	HDATA hCurrData = ::DATA_Get_First( hRootData, NULL );
	while ( hCurrData ) {
		HDATA hCheckData = hCurrData;
		hCurrData = ::DATA_Get_Next( hCurrData, NULL );

		HPROP hCheckedProp = ::DATA_Find_Prop( hCheckData, NULL, VE_PID_IF_SELECTED );
		if ( !hCheckedProp || !::GetPropNumericValueAsDWord(hCheckedProp) )
			::DATA_Remove( hCheckData, NULL );
		else
			if ( hCheckedProp )
				::SetPropNumericValue( hCheckedProp, FALSE );
	}
}

// ---
struct CInterfaceSortInfo {
	CAPointer<TCHAR> m_pIntName;
	HDATA            m_hIntData;
};


// ---
static CPArray<CInterfaceSortInfo> *g_pSortIntInfo;



// ---
static int CompareFunc( DWORD pA, DWORD pB ) {
	if ( g_pSortIntInfo )
		return lstrcmp( (*g_pSortIntInfo)[pA]->m_pIntName, (*g_pSortIntInfo)[pB]->m_pIntName );
	return 0;
}


// ---
template <class T>
inline void swap( T& a, T& b ) { a^=b; b^=a; a^=b; }

// ---
static inline void SwapItems( DWORD *pA, DWORD *pB ) {
	swap( *pA, *pB );
}


// ---
static void QuickSortRecursive( DWORD *pBase, int nLow, int nHight ) {
	//	nCount++;
	
	int   i,j;
	DWORD str;
	
	i = nHight;
	j = nLow;
	
	str = pBase[((int) ((nLow+nHight) / 2))];
	
	do {
		
		while ( ::CompareFunc(pBase[j], str) < 0 ) j++;
		while ( ::CompareFunc(pBase[i], str) > 0 ) i--;
		
		if ( i >= j ) {
			
			if ( i != j ) 
				SwapItems( &pBase[i], &pBase[j] );
			
			i--;
			j++;
		}
	} while (j <= i);
	
	if (nLow < i		 ) ::QuickSortRecursive( pBase, nLow, i );
	if (j		 < nHight) ::QuickSortRecursive( pBase, j,    nHight );
}

// ---
static void SortIndexArray( DWORD *pBase, int nHight ) {
	::QuickSortRecursive( pBase, 0, nHight - 1 );
}

// ---
static void SortInterfaceNames( CPArray<CInterfaceSortInfo> &rcSrcInterfaces, CPArray<CInterfaceSortInfo> &rcDstInterfaces ) {
	if ( rcSrcInterfaces.Count() ) {
		g_pSortIntInfo = &rcSrcInterfaces;
		
		int i, c;
		CAPointer<DWORD> nIndexArray = new DWORD[rcSrcInterfaces.Count()];
		for( i=0,c=rcSrcInterfaces.Count(); i<c; i++ )
			nIndexArray[i] = i;
		
		::SortIndexArray( nIndexArray, rcSrcInterfaces.Count() );
		
		for( i=0,c=rcSrcInterfaces.Count(); i<c; i++ )
			rcDstInterfaces.Add( rcSrcInterfaces[nIndexArray[i]] );
		
		g_pSortIntInfo = NULL;
	}
}



/////////////////////////////////////////////////////////////////////////////
// CSelectLoadedInterfacesDlg dialog

// ---
CSelectLoadedInterfacesDlg::CSelectLoadedInterfacesDlg(HDATA hLoadedContainer, CVisualEditorDlg *pParent, BOOL bFlushSelected /*TRUE*/)	: 
   CDialog(CSelectLoadedInterfacesDlg::IDD, pParent),
	 m_hLoadedContainer( hLoadedContainer ),
	 m_pHost( pParent ),
	 m_bFlushSelected( bFlushSelected )
{
	//{{AFX_DATA_INIT(CSelectLoadedInterfacesDlg)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT

	m_CTTControl.SetEnableAutoScanForEnable( false );
}

// ---
void CSelectLoadedInterfacesDlg::DoDataExchange(CDataExchange* pDX) {
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CSelectLoadedInterfacesDlg)
	DDX_Control(pDX, IDC_SID_CTTTREE, m_CTTControl);
	//}}AFX_DATA_MAP
}

// ---
BOOL CSelectLoadedInterfacesDlg::DestroyWindow() {
	WINDOWPLACEMENT rcPlacement;
	rcPlacement.length = sizeof(rcPlacement);

	GetWindowPlacement( &rcPlacement );
	CRegStorage rcRegStorage( CFG_REG_KEY );
	rcRegStorage.PutValue( SLIPLACEMENT_REG_VALUE, (BYTE *)&rcPlacement, sizeof(rcPlacement) );

	return CDialog::DestroyWindow();
}


// ---
void CSelectLoadedInterfacesDlg::LoadInterfaceStuff( HDATA hData, HPROP hProp, CCTItemsArray &newItems, int nFirstItemLevel ) {
	m_pHost->LoadInterfaceMnemonicIDStuff( hData, newItems, nFirstItemLevel, true );
	m_pHost->LoadInterfaceDigitalIDStuff( hData, newItems, nFirstItemLevel, true );
	m_pHost->LoadInterfaceRevisionIDStuff( hData, newItems, nFirstItemLevel, true );
	m_pHost->LoadInterfaceCreationDateStuff( hData, newItems, nFirstItemLevel, true );
	m_pHost->LoadInterfaceObjectTypeStuff( hData, newItems, nFirstItemLevel, true );
	m_pHost->LoadInterfaceIncludeFileStuff( hData, newItems, nFirstItemLevel, true );

	m_pHost->LoadShortCommentStuff( hData, newItems, nFirstItemLevel, true );
	m_pHost->LoadLargeCommentStuff( hData, newItems, nFirstItemLevel, true );

	m_pHost->LoadInterfaceAutorNameStuff( hData, newItems, nFirstItemLevel, true );
	m_pHost->LoadInterfaceVendorIDStuff( hData, newItems, nFirstItemLevel, true );
}

// ---
void CSelectLoadedInterfacesDlg::LoadOneInterfaceStuff( HDATA hData, CCTItemsArray &newItems, int nFirstItemLevel ) {
	HPROP hProp = ::DATA_Find_Prop( hData, NULL, VE_PID_IF_INTERFACENAME );
	CAPointer<char> pValue;
	CPathStr pLabel;
	if ( hProp )
		pValue = ::GetPropValueAsString( hProp, NULL );

	pLabel = pValue;

	CNodeUID rcUID;
	if ( ::GetNodeUID(hData, rcUID) ) {
		::AjustDisplayString( pLabel, 100 );
		pLabel += _T("<");
		pLabel += LPCTSTR(rcUID);
		pLabel += _T(">");
	}

	HPROP hContNameProp = ::DATA_Find_Prop( hData, NULL, VE_PID_IF_CONTAINERNAME );
	if ( hContNameProp ) {
		CAPointer<char> pContName = ::GetPropValueAsString( hContNameProp, NULL );
		pLabel += _T("  <");
		pLabel += pContName;
		pLabel += _T(">");
		::DATA_Remove_Prop_H( hData, NULL, hContNameProp );
	}

	CControlTreeItem *item;

	item = new CControlTreeItem;
	item->SetText( LPCSTR(pLabel) );
	item->SetLevel( nFirstItemLevel );
	item->SetCanBeRemoved( false );
	
	item->SetCheckAlias();

	HPROP hCheckedProp = ::DATA_Find_Prop( hData, NULL, VE_PID_IF_SELECTED );
	if ( !hCheckedProp )
		hCheckedProp = ::DATA_Add_Prop( hData, NULL, VE_PID_IF_SELECTED, AVP_dword(FALSE), 0 );

	if ( m_bFlushSelected && hCheckedProp ) 
		::PROP_Set_Val( hCheckedProp, AVP_dword(FALSE), 0 );

	BOOL bSelected = hCheckedProp ? ::GetPropNumericValueAsDWord(hCheckedProp) : FALSE;
	item->Check( !!bSelected );

	CPPDescriptor *pDescr = new CPPDescriptor( hData, hProp );
	pDescr->m_nSpecialType = sdt_Interface | sdt_Label;

	item->SetData( (void *)pDescr, false );
		
	newItems.Add( item );

	LoadInterfaceStuff( hData, hProp, newItems, nFirstItemLevel + 1 );
}

// ---
void CSelectLoadedInterfacesDlg::EnableDlgButtons() {
	BOOL bHasSelected = ::IsSelectedPresented( m_hLoadedContainer );
	if ( bHasSelected ) {
		::EnableWindow( ::GetDlgItem(*this, IDOK), TRUE );
		::SendMessage( *this, DM_SETDEFID, IDOK, 0 );
	}
	else {
		::EnableWindow( ::GetDlgItem(*this, IDOK), FALSE );
		::SendMessage( *this, DM_SETDEFID, IDCANCEL, 0 );
	}
}


BEGIN_MESSAGE_MAP(CSelectLoadedInterfacesDlg, CDialog)
	//{{AFX_MSG_MAP(CSelectLoadedInterfacesDlg)
	ON_WM_SIZE()
	ON_MESSAGE( CTTM_WINDOWSHUTDOWN, OnCTTWindowShutDown )
	ON_MESSAGE( CTTM_CHECKEDCHANGING, OnCTTCheckedChanging )
	ON_MESSAGE( CTTM_DOEXTERNALEDITING, OnCTTDoExternalEditing )
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CSelectLoadedInterfacesDlg message handlers
// ---
BOOL CSelectLoadedInterfacesDlg::OnInitDialog() {
	CDialog::OnInitDialog();
	
	int nFirstItemLevel = 1;

	CCTItemsArray &newItems = *new CCTItemsArray( true );

	BOOL bModified = m_pHost->IsModified();

	CPArray<CInterfaceSortInfo> rcSortIntInfo( 0, 1, false );

	HDATA hCurrData = ::DATA_Get_First( m_hLoadedContainer, NULL );
	while ( hCurrData ) {
		HPROP hProp = ::DATA_Find_Prop( hCurrData, NULL, VE_PID_IF_INTERFACENAME );
		if ( hProp ) {
			CInterfaceSortInfo *pSortInfo = new CInterfaceSortInfo();
			pSortInfo->m_pIntName = ::GetPropValueAsString( hProp, NULL );
			pSortInfo->m_hIntData = hCurrData;
			rcSortIntInfo.Add( pSortInfo );
		}

		hCurrData = ::DATA_Get_Next( hCurrData, NULL );
	}

	CPArray<CInterfaceSortInfo> rcNewSortIntInfo( rcSortIntInfo.Count(), 1, true );
	::SortInterfaceNames( rcSortIntInfo, rcNewSortIntInfo );

	for ( int i=0,c=rcNewSortIntInfo.Count(); i<c; i++ ) {
		LoadOneInterfaceStuff( rcNewSortIntInfo[i]->m_hIntData, newItems, nFirstItemLevel );
	}
		
	m_pHost->SetModified( bModified );

	m_CTTControl.SetItemsArray( &newItems );
	
	EnableDlgButtons();

	CRegStorage rcRegStorage( CFG_REG_KEY );

	WINDOWPLACEMENT rcPlacement;
	rcPlacement.length = sizeof(rcPlacement);

	DWORD dwSize = sizeof(rcPlacement);
	if ( rcRegStorage.GetValue(SLIPLACEMENT_REG_VALUE, (BYTE *)&rcPlacement, dwSize) )
		SetWindowPlacement( &rcPlacement );

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

// ---
void CSelectLoadedInterfacesDlg::OnSize(UINT nType, int cx, int cy) {
	CDialog::OnSize(nType, cx, cy);
	
  static RULE rules[] = {
    // Action    Act-on                   Relative-to           Offset
    // ------    ------                   -----------           ------
    {  lMOVE,    lRIGHT(IDCANCEL),				lRIGHT(lPARENT)				,-5  },
    {  lMOVE,    lBOTTOM(IDCANCEL),				lBOTTOM(lPARENT)			,-5  },

    {  lMOVE,    lRIGHT(IDOK),						lLEFT(IDCANCEL)				,-5  },
    {  lMOVE,    lBOTTOM(IDOK),						lBOTTOM(IDCANCEL)			,-0  },

    {  lSTRETCH, lRIGHT (IDC_SID_CTTTREE),lRIGHT(lPARENT)				,-5  },
    {  lSTRETCH, lBOTTOM(IDC_SID_CTTTREE),lTOP(IDOK)			      ,-5  },

    {  lEND																													 }
  };

  Layout_ComputeLayout( GetSafeHwnd(), rules );
}


// ---
void CSelectLoadedInterfacesDlg::OnOK() {
	::ProcessDataForSelected( m_hLoadedContainer );

	CDialog::OnOK();
}

// ---
LRESULT CSelectLoadedInterfacesDlg::OnCTTWindowShutDown( WPARAM wParam, LPARAM lParam ) {
	CTTMessageInfo *info = (CTTMessageInfo *)lParam;

	CCTItemsArray *items = m_CTTControl.GetItemsArray();

	if ( items ) {
		for ( int i=0; i<items->GetSize(); i++ )
			delete CPPD((*items)[i]);
	}

	CCTItemsArray &newItems = *new CCTItemsArray( true );
	m_CTTControl.SetItemsArray( &newItems );

	return 0;
}

// Сообщение об намерении установить Checked
// WPARAM		- LPBOOL - указатель на результат - не используется
// LPARAM		- указатель на экземпляр структуры CTTMessageInfo
//						m_Value.m_bSetRemove - TRUE - установить, FALSE - снять
// LRESULT	- не используется
// ---
LRESULT CSelectLoadedInterfacesDlg::OnCTTCheckedChanging( WPARAM wParam, LPARAM lParam ) {

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
	}

	EnableDlgButtons();

	return 0;
}

// Запрос к Parent на выполнение внешнего редактирования текста элемента
// WPARAM		- LPBOOL - указатель на результат - TRUE - если редактирование выполнено
//            По-умолчанию - FALSE
// LPARAM		- указатель на экземпляр CTTMessageInfo 
//            m_Value.m_pItemText - копия текста узла
// LRESULT	- не используется
// UINT const CTTM_DOEXTERNALEDITING   
LRESULT CSelectLoadedInterfacesDlg::OnCTTDoExternalEditing( WPARAM wParam, LPARAM lParam ) {
	CTTMessageInfo *info = (CTTMessageInfo *)lParam;

	int nItemInd         = info->m_nItemIndex;

	CCTItemsArray &items = *m_CTTControl.GetItemsArray();
	HDATA hData = CPPD(items[nItemInd])->m_hData;
	HPROP hProp = CPPD(items[nItemInd])->m_hProp;

	// Вход в редактирование LargeComment
	if ( ::IsIDProlog(m_CTTControl, nItemInd, VE_PID_LARGECOMMENT_ID) ) {
		*(reinterpret_cast<BOOL *>(wParam)) = m_pHost->EditLargeComment( info, IDS_LET_LARGECOMMENT, IDS_LARGECOMMENT_PREFIX, hData, hProp, TRUE, this );
		return 0;
	}

	return 0;
}




