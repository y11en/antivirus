// PropMemberDlg.cpp : implementation file
//

#include "stdafx.h"
#include "../AVPPveID.h"
#include <Stuff\CPointer.h>
#include <Stuff\PArray.h>
#include "VisualEditor.h"
#include "VisualEditorDlg.h"
#include "PropMemberDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

char *CreateDisplayLabelSTR( DWORD dwPrefixID, LPCTSTR lpszStr );
void SetOneItemExpanded( CControlTreeItem *item, HDATA hData );

// ---
void DeleteData( CTreeWnd &owner, HTREEITEM cur, int ind, void *par ) {
	CCTItemsArray &items = *((CCheckTree &)owner).GetItemsArray();
	CCheckTreeItem *item = items[ind];
	delete CPPD(item);
}


/////////////////////////////////////////////////////////////////////////////
// CPropMemberDlg dialog


CPropMemberDlg::CPropMemberDlg(CString &rcMemberText, HDATA hStructTree, CWnd* pParent /*=NULL*/)
	: CDialog(CPropMemberDlg::IDD, pParent),
	m_rcMemberText( rcMemberText ),
	m_hStructTree( hStructTree )
{
	//{{AFX_DATA_INIT(CPropMemberDlg)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}


// ---
BOOL CPropMemberDlg::DestroyWindow() {
	m_ControlTree.ForEach( ::DeleteData, NULL, 0 );
	return CDialog::DestroyWindow();
}

// ---
void CPropMemberDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CPropMemberDlg)
	DDX_Control(pDX, IDC_PM_STRUCTTREE, m_ControlTree);
	//}}AFX_DATA_MAP
}


// ---
void CPropMemberDlg::LoadInterfaceDataStructureMemberShortCommentStuff( HDATA hData, HPROP hProp, CCTItemsArray &newItems, int nFirstItemLevel ) {
	CControlTreeItem *item;
	CString rcString;
	CPPDescriptor *pDescr;

	CAPointer<char> pLabel;
	HPROP hValueProp;

	if ( hValueProp = ::DATA_Find_Prop( hData, NULL, VE_PID_SHORTCOMMENT ) ) {
		pLabel = ::GetPropValueAsString( hValueProp, NULL );
		pLabel = ::CreateDisplayLabelSTR( IDS_SHORTCOMMENT_PREFIX, pLabel );
	}

	rcString = pLabel;

	item = new CControlTreeItem;
	item->SetText( rcString );
	item->SetLevel( nFirstItemLevel );
	item->SetCanBeRemoved( false );
	item->SetSimpleStringAlias();
	
	pDescr = new CPPDescriptor( hData, hValueProp );
	item->SetData( (void *)pDescr, false );
	
	newItems.Add( item );
}


// ---
void CPropMemberDlg::LoadInterfaceDataStructureMemberTypeStuff( HDATA hData, HPROP hProp, CCTItemsArray &newItems, int nFirstItemLevel ) {
	CControlTreeItem *item;
	CString rcString;
	CPPDescriptor *pDescr;

	CAPointer<char> pLabel;
	HPROP hValueProp;

	if ( hValueProp = ::DATA_Find_Prop( hData, NULL, VE_PID_IFDM_TYPENAME ) ) {
		pLabel = ::GetPropValueAsString( hValueProp, NULL );
		pLabel = ::CreateDisplayLabelSTR( IDS_DSMTYPENAME_PREFIX, pLabel );
	}

	rcString = pLabel;

	item = new CControlTreeItem;
	item->SetText( rcString );
	item->SetLevel( nFirstItemLevel );
	item->SetCanBeRemoved( false );
	item->SetSimpleStringAlias();
	
	pDescr = new CPPDescriptor( hData, hValueProp );
	item->SetData( (void *)pDescr, false );
	
	newItems.Add( item );
}



// ---
void CPropMemberDlg::LoadInterfaceDataStructureMemberStuff( HDATA hData, HPROP hProp, CCTItemsArray &newItems, int nFirstItemLevel ) {
	LoadInterfaceDataStructureMemberTypeStuff( hData, hProp, newItems, nFirstItemLevel );
	LoadInterfaceDataStructureMemberShortCommentStuff( hData, hProp, newItems, nFirstItemLevel );
}


// ---
void CPropMemberDlg::LoadOneInterfaceDataStructureMemberStuff(HDATA hDataStructureMemberData, CCTItemsArray &newItems, int nFirstItemLevel) {
	HPROP hProp = ::DATA_Find_Prop( hDataStructureMemberData, NULL, VE_PID_IFDM_MEMBERNAME );
	if ( hProp ) {
		CAPointer<char> pValue;
		pValue = ::GetPropValueAsString( hProp, NULL );
		
		CControlTreeItem *item;

		item = new CControlTreeItem;
		item->SetText( LPCSTR(pValue) );
		item->SetLevel( nFirstItemLevel );
		item->SetSimpleStringAlias();
		item->SetCanBeRemoved( false );
		
		CPPDescriptor *pDescr = new CPPDescriptor( hDataStructureMemberData, hProp );
		pDescr->m_nSpecialType = sdt_DataStructureMember | sdt_Label;

		item->SetData( (void *)pDescr, false );
			
		::SetOneItemExpanded( item, hDataStructureMemberData );

		newItems.Add( item );
/*
		CString rcString;

		rcString.LoadString( IDS_DESCRIPTION );

		item = new CControlTreeItem;
		item->SetText( rcString );
		item->SetLevel( nFirstItemLevel + 1 );
		item->SetSimpleStringAlias();

		pDescr = new CPPDescriptor( NULL );
		pDescr->m_nSpecialType = sdt_Desctription;
		item->SetData( (void *)pDescr, false );
			
		newItems.Add( item );
*/
		LoadInterfaceDataStructureMemberStuff( hDataStructureMemberData, hProp, newItems, nFirstItemLevel + 1 );
	}
}

// ---
void CPropMemberDlg::LoadOneInterfaceDataStructureStuff(HDATA hDataStructureData, CCTItemsArray &newItems, int nFirstItemLevel) {
	HPROP hProp = ::DATA_Find_Prop( hDataStructureData, NULL, VE_PID_IFD_DATASTRUCTURENAME );
	if ( hProp ) {
		CString rcString;

		CAPointer<char> pValue;
		pValue = ::GetPropValueAsString( hProp, NULL );
		
		CControlTreeItem *item;

		item = new CControlTreeItem;
		item->SetText( LPCSTR(pValue) );
		item->SetLevel( nFirstItemLevel );
		// Who knows what it's for? item->SetSimpleEditAlias();
		item->SetSimpleStringAlias();
		item->SetCanBeRemoved( true );
		
		CPPDescriptor *pDescr = new CPPDescriptor( hDataStructureData, hProp );
		pDescr->m_nSpecialType = sdt_DataStructure | sdt_Label;

		item->SetData( (void *)pDescr, false );

		::SetOneItemExpanded( item, hDataStructureData );

		newItems.Add( item );

		MAKE_ADDR1(nStructAddr, VE_PID_IF_DATASTRUCTURES);
		HDATA hStructuresData = ::DATA_Find( hDataStructureData, nStructAddr );
		if ( hStructuresData ) {
			rcString.LoadString( IDS_DATASTRUCTURE );
			item = new CControlTreeItem;

			item->SetText( rcString );
			item->SetLevel( nFirstItemLevel + 1 );
			item->SetSimpleStringAlias();
			item->SetCanBeRemoved( false );

			pDescr = new CPPDescriptor( hStructuresData );
			item->SetData( (void *)pDescr, false );

			newItems.Add( item );

			HDATA hCurrData = ::DATA_Get_First( hStructuresData, NULL );
			while ( hCurrData ) {
				LoadOneInterfaceDataStructureStuff( hCurrData, newItems, nFirstItemLevel + 2 );
				hCurrData = ::DATA_Get_Next( hCurrData, NULL );
			}
		}
		
		MAKE_ADDR1(nMembersAddr, VE_PID_IF_DATASTRUCTUREMEMBERS);
		HDATA hMembersData = ::DATA_Find( hDataStructureData, nMembersAddr );
		if ( hMembersData ) {
			rcString.LoadString( IDS_DATASTRUCTUREMEMBERS );
			item = new CControlTreeItem;

			item->SetText( rcString );
			item->SetLevel( nFirstItemLevel + 1 );
			item->SetSimpleStringAlias();
			item->SetCanBeRemoved( false );

			pDescr = new CPPDescriptor( hMembersData );
			item->SetData( (void *)pDescr, false );

			newItems.Add( item );

			HDATA hCurrData = ::DATA_Get_First( hMembersData, NULL );
			while ( hCurrData ) {
				LoadOneInterfaceDataStructureMemberStuff( hCurrData, newItems, nFirstItemLevel + 2 );
				hCurrData = ::DATA_Get_Next( hCurrData, NULL );
			}
		}
	}
}


// ---
int CPropMemberDlg::FindStructureMember( CPArray<char> &pNames, int nNameIndex, int nItemIndex ) {
	CCTItemsArray &items = *m_ControlTree.GetItemsArray();
	nItemIndex = m_ControlTree.GetFirstChildIndex( nItemIndex );
	while ( nItemIndex >= 0 ) {
		CCheckTreeItem *item = items[nItemIndex];
		if ( IS_NODE_LABEL(item) ) {
			CString &rcString = item->GetText();
			char *pName = pNames[nNameIndex];
			if ( !lstrcmp(item->GetText(), pNames[nNameIndex]) ) {
				++nNameIndex;
				if ( nNameIndex <= pNames.MaxIndex() )
					return FindStructureMember( pNames, nNameIndex, nItemIndex );
				else
					return nItemIndex;
			}
		}
		else {
			int nChildIndex = FindStructureMember( pNames, nNameIndex, nItemIndex );
			if ( nChildIndex >= 0 )
				return nChildIndex;
		}
		nItemIndex = m_ControlTree.GetNextSiblingIndex( nItemIndex );
	}
	return -1;
}


BEGIN_MESSAGE_MAP(CPropMemberDlg, CDialog)
	//{{AFX_MSG_MAP(CPropMemberDlg)
	ON_BN_CLICKED(IDC_PM_CLEANUP, OnCleanUp)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CPropMemberDlg message handlers
// ---
BOOL CPropMemberDlg::OnInitDialog() {
	CDialog::OnInitDialog();
	
	int nFirstItemLevel = 1;
	CCTItemsArray &newItems = *new CCTItemsArray( true );
	
	HDATA hCurrData = ::DATA_Get_First( m_hStructTree, NULL );
	while ( hCurrData ) {
		LoadOneInterfaceDataStructureStuff( hCurrData, newItems, nFirstItemLevel );
		hCurrData = ::DATA_Get_Next( hCurrData, NULL );
	}

	m_ControlTree.SetItemsArray( &newItems );

	int nSelected = 0;

	if ( !m_rcMemberText.IsEmpty() ) {
		CAPointer<char> pFullName = new char[m_rcMemberText.GetLength() + 1];
		lstrcpy( pFullName, m_rcMemberText );
		char *pName = pFullName;

		CPArray<char> pNames( 0, 1, false );
		pNames.Add( pName );

		pName = strchr( pName, '.' );
		while ( pName ) {
			*pName = 0;
			pName++;
			pNames.Add( pName );
			pName = strchr( pName, '.' );
		}

		int nShouldBeSelected = FindStructureMember( pNames, 0, -1 );

		if ( nShouldBeSelected >= 0 )
			nSelected = nShouldBeSelected;
	}

	m_ControlTree.SetFocus();
	m_ControlTree.SelectNode( nSelected );

	return FALSE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

// ---
void CPropMemberDlg::OnOK() {
	CCTItemsArray &items = *m_ControlTree.GetItemsArray();
	int nSelected = m_ControlTree.GetSelectedIndex();
	if ( nSelected >= 0 ) {
		CCheckTreeItem *item = items[nSelected];
		HDATA hData = CPPD(item)->m_hData;
		HPROP hProp = ::DATA_Find_Prop( hData, NULL, VE_PID_IFDM_MEMBERNAME );
		if ( hProp ) {
			CAPointer<char> pValue;
			pValue = ::GetPropValueAsString( hProp, NULL );
			
			m_rcMemberText = pValue;

			nSelected = m_ControlTree.GetParentIndex( nSelected );
			while( nSelected >= 0 ) {
				item = items[nSelected];
				hData = CPPD(item)->m_hData;
				hProp = ::DATA_Find_Prop( hData, NULL, VE_PID_IFD_DATASTRUCTURENAME );
				if ( hProp ) {
					pValue = ::GetPropValueAsString( hProp, NULL );
					m_rcMemberText.Insert( 0, L'.' );
					m_rcMemberText.Insert( 0, pValue );
				}
				nSelected = m_ControlTree.GetParentIndex( nSelected );
			}
		}
		else {
			::DlgMessageBox( this, IDS_ERR_NODENOTMEMBER );
			return;
		}
	}
	CDialog::OnOK();
}


// ---
void CPropMemberDlg::OnCleanUp() {
	m_rcMemberText = "";
	CDialog::OnOK();
}

