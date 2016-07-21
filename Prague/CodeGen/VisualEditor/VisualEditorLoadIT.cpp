// ------------------------------------------------------------------------------------------

#include "stdafx.h"
#include <datatreeutils/dtutils.h>
#include <StuffIO\Utils.h>
#include <Stuff\CPointer.h>
#include "resource.h"
#include "VisualEditorDlg.h"
#include <avpprpid.h>
#include <hashutil/hashutil.h>

// ------------------------------------------------------------------------------------------

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

// ------------------------------------------------------------------------------------------
void CVisualEditorDlg::OnLoadData(HDATA i_hData, BOOL i_bLinkedToFile)
{
	if(IsPrototype() && IsNativeNode(i_hData) && !i_bLinkedToFile)
	{
		if (!GetDwordProp(i_hData, VE_PID_NATIVE))
		{
			SetDwordProp(i_hData, VE_PID_NATIVE, TRUE);
			SetModified(TRUE);
		}
	}
}

// ------------------------------------------------------------------------------------------
void SetOneItemExpanded( CControlTreeItem *item, HDATA hData ) 
{
	AVP_bool nValue = item->IsExpanded();

	HPROP hExpandedProp = ::DATA_Find_Prop( hData, NULL, VE_PID_NODEEXPANDED );
	if ( hExpandedProp ) 
		::PROP_Get_Val( hExpandedProp, &nValue, sizeof(nValue) );
	
	item->SetExpanded( !!nValue );
}

// ------------------------------------------------------------------------------------------
void CVisualEditorDlg::LoadInterfaceDescriptionNodes( HDATA hRootData ) 
{
	int nFirstItemLevel = 1;
	CCTItemsArray &newItems = *new CCTItemsArray( true );

	if ( hRootData ) 
	{
		if(IsImplementation())
		{
			LoadOnePluginStuff( hRootData, newItems, nFirstItemLevel );
			++nFirstItemLevel;
		}

		HDATA hCurrData = ::DATA_Get_First( hRootData, NULL );
		while ( hCurrData ) 
		{
			if(!IsPluginStaticInterfaceRoot(hCurrData))
				LoadOneInterfaceStuff( hCurrData, newItems, nFirstItemLevel );

			hCurrData = ::DATA_Get_Next( hCurrData, NULL );
		}

		LoadObjectsTable( hRootData );
	}

	if ( IsPrototype() ) 
		CreateStringNode( newItems, nFirstItemLevel, NULL, IDS_NEWINTERFACE, TRUE, FALSE, sdt_Interface);

	m_IDescriptions.SetEnableAutoScanForEnable( true );
	m_Interfaces.SetEnableAutoScanForEnable( false );
	m_Interfaces.SetItemsArray( &newItems );
}

// ------------------------------------------------------------------------------------------
void CVisualEditorDlg::LoadOnePluginStuff( HDATA hPluginData, CCTItemsArray &newItems, int nFirstItemLevel ) 
{
	if ( !::DATA_Find_Prop(hPluginData, NULL, VE_PID_IF_LASTINTERFACESLEVEL) ) 
	{
		::DATA_Add_Prop( hPluginData, NULL, VE_PID_IF_LASTINTERFACESLEVEL, 0, 0 );
		SetModified( true );
	}

	// plugin name
	CreatePropertyNode(newItems, nFirstItemLevel, hPluginData, VE_PID_PL_PLUGINNAME, TRUE, FALSE, sdt_Plugin);
	CCheckTreeItem* pItem = newItems[newItems.GetSize()-1];
	pItem->SetExpanded();

	++nFirstItemLevel;

	CreateStringNode(newItems, nFirstItemLevel, hPluginData, 
		_T("Plugin properties"), FALSE);

	++nFirstItemLevel;

	CreatePropertyNode(newItems, nFirstItemLevel, hPluginData, VE_PID_PL_MNEMONICID);
	CreatePropertyNode(newItems, nFirstItemLevel, hPluginData, VE_PID_PL_DIGITALID);
	CreatePropertyNode(newItems, nFirstItemLevel, hPluginData, VE_PID_PL_VERSIONID);
	CreatePropertyNode(newItems, nFirstItemLevel, hPluginData, VE_PID_PL_CODEPAGEID);
	CreatePropertyNode(newItems, nFirstItemLevel, hPluginData, VE_PID_PL_DATEFORMATID );
	CreatePropertyNode(newItems, nFirstItemLevel, hPluginData, VE_PID_PL_PROJECTNAME );
	CreatePropertyNode(newItems, nFirstItemLevel, hPluginData, VE_PID_PL_SUBPROJECTNAME );
	
	LoadShortCommentStuff( hPluginData, newItems, nFirstItemLevel, false );
	LoadLargeCommentStuff( hPluginData, newItems, nFirstItemLevel, false );
	LoadBehaviorCommentStuff( hPluginData, newItems, nFirstItemLevel, false );

	CreatePropertyNode(newItems, nFirstItemLevel, hPluginData, VE_PID_PL_AUTHORNAME );
	CreatePropertyNode(newItems, nFirstItemLevel, hPluginData, VE_PID_PL_VENDORID, FALSE );
}

// ------------------------------------------------------------------------------------------
void CVisualEditorDlg::LoadOneInterfaceStuff( 
	HDATA hInterfaceData, CCTItemsArray &newItems, int nFirstItemLevel ) 
{
	int nItemLevel = nFirstItemLevel;
	BOOL bLinkedToFile = !!::IsLinkedToFile( hInterfaceData );
	BOOL bEditable = !bLinkedToFile;

	if(IsImplementation() && bLinkedToFile)
		--nItemLevel;

	CControlTreeItem *pItem;
	if(IsImplementation() && !bLinkedToFile)
		pItem = new CVEControlTreeItem;
	else
		pItem = new CControlTreeItem;

	STreeNodeInfo oTreeNodeInfo( newItems, nItemLevel, hInterfaceData, 
		VE_PID_IF_INTERFACENAME, bEditable, TRUE, sdt_Interface );
	oTreeNodeInfo.m_pExportedItem = pItem;

	CreateTreeNode(oTreeNodeInfo);

	HPROP hProp = ::DATA_Find_Prop( hInterfaceData, NULL, VE_PID_IF_INTERFACENAME );
	hProp = ::DATA_Find_Prop( hInterfaceData, 0, 0 );

	::AddHeaderGuidProp( hInterfaceData );

	++nFirstItemLevel;

	LoadInterfaceMnemonicIDStuff( hInterfaceData, newItems, nFirstItemLevel, bLinkedToFile );
	LoadInterfaceDigitalIDStuff( hInterfaceData, newItems, nFirstItemLevel, bLinkedToFile );
	LoadInterfaceRevisionIDStuff( hInterfaceData, newItems, nFirstItemLevel, bLinkedToFile );
	LoadInterfaceCreationDateStuff( hInterfaceData, newItems, nFirstItemLevel, bLinkedToFile );
	LoadInterfaceObjectTypeStuff( hInterfaceData, newItems, nFirstItemLevel, bLinkedToFile );
	LoadInterfaceIncludeFileStuff( hInterfaceData, newItems, nFirstItemLevel, bLinkedToFile );

	if (IsImplementation() && !bLinkedToFile )
		LoadInterfaceOutputFileStuff( hInterfaceData, newItems, nFirstItemLevel, bLinkedToFile );

	LoadShortCommentStuff( hInterfaceData, newItems, nFirstItemLevel, bLinkedToFile );
	LoadLargeCommentStuff( hInterfaceData, newItems, nFirstItemLevel, bLinkedToFile );

	LoadInterfaceAutorNameStuff( hInterfaceData, newItems, nFirstItemLevel, bLinkedToFile );
	LoadInterfaceVendorIDStuff( hInterfaceData, newItems, nFirstItemLevel, bLinkedToFile );

	ProcessObjectTable( hInterfaceData, VE_PID_IF_TYPES, VE_PID_IFT_TYPENAME );
}

// ------------------------------------------------------------------------------------------
void CVisualEditorDlg::LoadInterfaceMnemonicIDStuff( 
	HDATA hData, CCTItemsArray &newItems, int nFirstItemLevel, BOOL bLinkedToFile ) 
{
	BOOL bEditable = IsPrototype() && !bLinkedToFile;
	CreatePropertyNode(newItems, nFirstItemLevel, hData, VE_PID_IF_MNEMONICID, bEditable); 
}

// ---
void CVisualEditorDlg::LoadInterfaceDigitalIDStuff( 
	HDATA hData, CCTItemsArray &newItems, int nFirstItemLevel, BOOL bLinkedToFile ) 
{
	BOOL bEditable = IsPrototype() && !bLinkedToFile;
	CreatePropertyNode(newItems, nFirstItemLevel, hData, VE_PID_IF_DIGITALID, bEditable); 
}

// ---
void CVisualEditorDlg::LoadInterfaceRevisionIDStuff( 
	HDATA hData, CCTItemsArray &newItems, int nFirstItemLevel, BOOL bLinkedToFile ) 
{
	BOOL bEditable = IsPrototype() && !bLinkedToFile;
	CreatePropertyNode(newItems, nFirstItemLevel, hData, VE_PID_IF_REVISIONID, bEditable); 
}

// ---
void CVisualEditorDlg::LoadInterfaceCreationDateStuff( 
	HDATA hData, CCTItemsArray &newItems, int nFirstItemLevel, BOOL bLinkedToFile ) 
{
	BOOL bEditable = IsPrototype() && !bLinkedToFile;
	CreatePropertyNode(newItems, nFirstItemLevel, hData, VE_PID_IF_CREATIONDATE, bEditable); 
}

// ---
void CVisualEditorDlg::LoadInterfaceObjectTypeStuff( 
	HDATA hData, CCTItemsArray &newItems, int nFirstItemLevel, BOOL bLinkedToFile ) 
{
	CAPointer<char> pLabel;
	
	HPROP hProp = ::DATA_Find_Prop( hData, NULL, VE_PID_IF_OBJECTTYPE );
	if ( hProp ) 
		pLabel = ::GetPropValueAsString( hProp, NULL );

	CIObject *pObject = m_IOTable.Add( pLabel );
	if ( pObject ) 
	{
		pObject->m_bInterface = TRUE;
		HPROP hIDProp = ::DATA_Find_Prop( hData, NULL, VE_PID_IF_DIGITALID );
		if ( hIDProp ) 
		{
			AVP_dword dwID;
			::PROP_Get_Val( hIDProp, &dwID, sizeof(dwID) );
			pObject->m_dwObjectID = dwID;
		}
		HPROP hIncludeProp = ::DATA_Find_Prop( hData, NULL, VE_PID_IF_INCLUDEFILE );
		if ( hIncludeProp ) 
		{
			CAPointer<char> pValue = ::GetPropValueAsString( hIncludeProp, NULL );
			pObject->m_rcIncludeName = pValue;
		}

		::GetNodeUID( hData, pObject->m_UID );
	}

	BOOL bEditable = IsPrototype() && !bLinkedToFile;
	CreatePropertyNode(newItems, nFirstItemLevel, hData, VE_PID_IF_OBJECTTYPE, bEditable); 
}

// ---
void CVisualEditorDlg::LoadInterfaceIncludeFileStuff( 
	HDATA hData, CCTItemsArray &newItems, int nFirstItemLevel, BOOL bLinkedToFile ) 
{		
	CAPointer<char> pLabel;
	
	HPROP hProp = ::DATA_Find_Prop( hData, NULL, VE_PID_IF_INCLUDEFILE );
	if ( hProp  ) 
	{
		pLabel = ::GetPropValueAsString( hProp, NULL );
		SetObjectIncludeName( hData, pLabel );
	}

	BOOL bEditable = IsPrototype() && !bLinkedToFile;
	CreatePropertyNode(newItems, nFirstItemLevel, hData, VE_PID_IF_INCLUDEFILE, bEditable); 
}

// ---
void CVisualEditorDlg::LoadInterfaceOutputFileStuff( 
	HDATA hData, CCTItemsArray &newItems, int nFirstItemLevel, BOOL bLinkedToFile ) 
{
	BOOL bEditable = IsImplementation() || !bLinkedToFile;
	CreatePropertyNode(newItems, nFirstItemLevel, hData, VE_PID_IF_OUTPUTFILE, bEditable); 
}

// ---
void CVisualEditorDlg::LoadInterfaceAutorNameStuff( 
	HDATA hData, CCTItemsArray &newItems, int nFirstItemLevel, BOOL bLinkedToFile ) 
{
	BOOL bEditable = IsPrototype() && !bLinkedToFile;
	CreatePropertyNode(newItems, nFirstItemLevel, hData, VE_PID_IF_AUTORNAME, bEditable); 
}

// ---
void CVisualEditorDlg::LoadInterfaceVendorIDStuff( 
	HDATA hData, CCTItemsArray &newItems, int nFirstItemLevel, BOOL bLinkedToFile ) 
{
	CreatePropertyNode(newItems, nFirstItemLevel, hData, VE_PID_IF_VENDORID, FALSE); 
}

// ------------------------------------------------------------------------------------------
void CVisualEditorDlg::LoadShortCommentStuff( 
	HDATA hData, CCTItemsArray &newItems, int nFirstItemLevel, BOOL bLinkedToFile ) 
{
	BOOL bEditable = TRUE;

	if(bLinkedToFile)
		bEditable = FALSE;
	else
	if(IsNativeData( hData ) && IsImplementation())
		bEditable = FALSE;

	CreatePropertyNode(newItems, nFirstItemLevel, hData, VE_PID_SHORTCOMMENT, bEditable);
}

// ---
void CVisualEditorDlg::LoadLargeCommentStuff( 
	HDATA hData, CCTItemsArray &newItems, int nFirstItemLevel, BOOL bLinkedToFile ) 
{
	CreatePropertyNode(newItems, nFirstItemLevel, hData, VE_PID_LARGECOMMENT);
}

// ---
void CVisualEditorDlg::LoadBehaviorCommentStuff( 
	HDATA hData, CCTItemsArray &newItems, int nFirstItemLevel, BOOL bLinkedToFile ) 
{
	CreatePropertyNode(newItems, nFirstItemLevel, hData, VE_PID_BEHAVIORCOMMENT);
}

// ---
void CVisualEditorDlg::LoadValueCommentStuff( 
	HDATA hData, CCTItemsArray &newItems, int nFirstItemLevel, BOOL bLinkedToFile ) 
{
	CreatePropertyNode(newItems, nFirstItemLevel, hData, VE_PID_VALUECOMMENT);
}

// ---
void	CVisualEditorDlg::LoadImplCommentStuff( 
	HDATA hData, CCTItemsArray &newItems, int nFirstItemLevel, BOOL bLinkedToFile )
{
	// if is not native node or prototype mode - do not show implementation comments
	if(IsNativeData(hData) && IsImplementation())
		CreatePropertyNode(newItems, nFirstItemLevel, hData, VE_PID_IMPLCOMMENT);
}

// ---
void CVisualEditorDlg::CreateShortCommentStuff( HDATA hData, CCTItemsArray &newItems, int nFirstItemLevel ) 
{
	LoadShortCommentStuff(hData, newItems, nFirstItemLevel, FALSE);
}

// ---
void CVisualEditorDlg::CreateLargeCommentStuff( HDATA hData, CCTItemsArray &newItems, int nFirstItemLevel ) 
{
	LoadLargeCommentStuff(hData, newItems, nFirstItemLevel, false);
}

// ---
void CVisualEditorDlg::CreateBehaviorCommentStuff( HDATA hData, CCTItemsArray &newItems, int nFirstItemLevel ) 
{
	LoadBehaviorCommentStuff(hData, newItems, nFirstItemLevel, false);
}

// ---
void CVisualEditorDlg::CreateValueCommentStuff( HDATA hData, CCTItemsArray &newItems, int nFirstItemLevel ) 
{
	LoadValueCommentStuff(hData, newItems, nFirstItemLevel, false);
}

// ------------------------------------------------------------------------------------------
// loading interface or plugin descriptions
void CVisualEditorDlg::LoadInterfaceDescriptionTreeNodes( HDATA hRootData ) 
{
	int nFirstItemLevel = 1;
	BOOL bLinkedToFile = !!::IsLinkedToFile( hRootData );
	CCTItemsArray &newItems = *new CCTItemsArray( true );

	if(IsImplementation())
	{
		if ( !bLinkedToFile ) 
		{
			LoadInterfaceSubTypeStuff( hRootData, newItems, nFirstItemLevel );
			LoadInterfaceFlagsStuff( hRootData, newItems, nFirstItemLevel );
		}

		LoadInterfaceIncludesNodes( hRootData, newItems, nFirstItemLevel, bLinkedToFile );
	}

	LoadInterfaceTypesNodes( hRootData, newItems, nFirstItemLevel, bLinkedToFile );
	LoadInterfaceConstantsNodes( hRootData, newItems, nFirstItemLevel, bLinkedToFile );
	LoadInterfaceMessagesNodes(hRootData, newItems, nFirstItemLevel, bLinkedToFile);
	LoadInterfaceErrorCodeNodes( hRootData, newItems, nFirstItemLevel, bLinkedToFile );

	if(IsImplementation())
		LoadInterfaceDataStructuresNodes( hRootData, newItems, nFirstItemLevel, bLinkedToFile );

	LoadInterfacePropertiesNodes( hRootData, newItems, nFirstItemLevel, bLinkedToFile );
	LoadInterfaceMethodsNodes( hRootData, newItems, nFirstItemLevel, bLinkedToFile );

	if(IsImplementation() && !bLinkedToFile)
		LoadInterfaceSystemMethodsNodes( hRootData, newItems, nFirstItemLevel, bLinkedToFile );
	
	m_IDescriptions.SetItemsArray( &newItems );

	EnableEditableStuff();
}

// ------------------------------------------------------------------------------------------
void CVisualEditorDlg::LoadInterfaceSubTypeStuff( 
	HDATA hData, CCTItemsArray &newItems, int nFirstItemLevel ) 
{
	CreatePropertyNode(newItems, nFirstItemLevel, hData, VE_PID_IF_SUBTYPEID);
}

// ------------------------------------------------------------------------------------------
void CVisualEditorDlg::LoadInterfaceFlagsStuff( 
	HDATA hData, CCTItemsArray &newItems, int nFirstItemLevel ) 
{
	CreateStringNode(newItems, nFirstItemLevel, hData, IDS_INTERFACEFLAGS, FALSE);
	
	++nFirstItemLevel;

	CreatePropertyNode(newItems, nFirstItemLevel, hData, VE_PID_IF_STATIC);
	CreatePropertyNode(newItems, nFirstItemLevel, hData, VE_PID_IF_SYSTEM);
	CreatePropertyNode(newItems, nFirstItemLevel, hData, VE_PID_IF_TRANSFERPROPUP);
	CreatePropertyNode(newItems, nFirstItemLevel, hData, VE_PID_IF_NONSWAPABLE);

	CControlTreeItem* pItem;
	CString rcString;

	CreatePropertyNode(newItems, nFirstItemLevel, hData, VE_PID_IF_PROTECTED_BY, TRUE, FALSE, 0, 
		VE_IFF_PROTECTED_BY_NONE, IDS_IFACE_FLAG_SYNCHRONIZED);

	HPROP hProp = ::DATA_Find_Prop(hData, NULL, VE_PID_IF_PROTECTED_BY);
	AVP_dword dwValue = ::GetPropNumericValueAsDWord( hProp );

	pItem = (CControlTreeItem*) newItems[newItems.GetSize()-1];
	pItem->SetCheckAlias();
	pItem->Check( !!dwValue  );
	CPPD(pItem)->m_nArrayIndex = 0;

	++nFirstItemLevel;

	CreatePropertyNode(newItems, nFirstItemLevel, hData, VE_PID_IF_PROTECTED_BY, TRUE, FALSE, 0, 
		VE_IFF_PROTECTED_BY_CS, IDS_IFACE_FLAG_CS);

	pItem = (CControlTreeItem*) newItems[newItems.GetSize()-1];
	CPPD(pItem)->m_nArrayIndex = 1;

	CreatePropertyNode(newItems, nFirstItemLevel, hData, VE_PID_IF_PROTECTED_BY, TRUE, FALSE, 0, 
		VE_IFF_PROTECTED_BY_MUTEX, IDS_IFACE_FLAG_MUTEX);

	pItem = (CControlTreeItem*) newItems[newItems.GetSize()-1];
	CPPD(pItem)->m_nArrayIndex = 2;
}

// ------------------------------------------------------------------------------------------
void CVisualEditorDlg::LoadInterfaceIncludesNodes( 
	HDATA hRootData, CCTItemsArray &newItems, int nFirstItemLevel, BOOL bLinkedToFile ) 
{
	CreatePropertyNode(newItems, nFirstItemLevel, hRootData, VE_PID_IF_INCLUDENAMES, FALSE, FALSE, sdt_Includes);

	++nFirstItemLevel;

	HPROP hProp = ::DATA_Find_Prop( hRootData, NULL, VE_PID_IF_INCLUDENAMES );
	int nCount = ::PROP_Arr_Count( hProp );

	STreeNodeInfo oIncludeNodeInfo
		(newItems, nFirstItemLevel, hRootData, 
		VE_PID_IF_INCLUDENAMES, !bLinkedToFile, !bLinkedToFile);

	oIncludeNodeInfo.m_nPrefixId = -1;

	int i;
	for(i=0; i<nCount; i++)
	{
		oIncludeNodeInfo.m_nArrayPosition = i+1;
		CreateTreeNode(oIncludeNodeInfo);
	}

	if(!bLinkedToFile)
	{
		oIncludeNodeInfo.m_nArrayPosition = i+1;
		CreateTreeNode(oIncludeNodeInfo);
	}
}

// ------------------------------------------------------------------------------------------
// interface types
void CVisualEditorDlg::LoadInterfaceTypesNodes( 
	HDATA hRootData, CCTItemsArray &newItems, int nFirstItemLevel, BOOL bLinkedToFile ) 
{
	HDATA hData;

	MAKE_ADDR1(nAddr1, VE_PID_IF_TYPES);
	if ( !(hData = ::DATA_Find(hRootData, nAddr1)) ) 
	{
		hData = ::DATA_Add( hRootData, NULL, VE_PID_IF_TYPES, 0, 0 );
		OnCreateData(hData);
	}

	CreateStringNode(newItems, nFirstItemLevel, hData, IDS_TYPES, FALSE);

	++nFirstItemLevel;

	if ( hData ) 
	{
		HDATA hCurrData = ::DATA_Get_First( hData, NULL );
		while ( hCurrData ) 
		{
			LoadOneInterfaceTypeStuff( hCurrData, newItems, nFirstItemLevel, bLinkedToFile );
			hCurrData = ::DATA_Get_Next( hCurrData, NULL );
		}
	}

	if(!bLinkedToFile)
		CreateStringNode(newItems, nFirstItemLevel, NULL, IDS_NEWTYPE, TRUE, FALSE, sdt_Type);
}

// ---
void CVisualEditorDlg::LoadOneInterfaceTypeStuff( 
	HDATA hTypeData, CCTItemsArray &newItems, int nFirstItemLevel, BOOL bLinkedToFile ) 
{
	OnLoadData(hTypeData, bLinkedToFile );
	BOOL bEditable = IsEditableData(hTypeData, bLinkedToFile);

	CreatePropertyNode(newItems, nFirstItemLevel, hTypeData, VE_PID_IFT_TYPENAME, bEditable, bEditable, sdt_Type);
	++nFirstItemLevel;

	BOOL bSeparator = ::IsSeparator(hTypeData);
	if(!bSeparator)
	{
		CControlTreeItem *pItem = (CControlTreeItem*) newItems[newItems.GetSize()-1];
		CIObject *pObject = m_IOTable.Add( pItem->GetText() );
		if ( pObject ) 
		{
			AVP_dword dwID = ::DATA_Get_Id( hTypeData, NULL );
			pObject->m_dwObjectID = dwID;
			HPROP hValueProp;
			if ( hValueProp = ::DATA_Find_Prop( m_hCCTree, NULL, VE_PID_IF_INCLUDEFILE ) ) 
			{
				CAPointer<char> pValue = ::GetPropValueAsString( hValueProp, NULL );
				pObject->m_rcIncludeName = pValue;
			}
			::GetNodeUID( m_hCCTree, pObject->m_UID );
		}

		if ( !::DATA_Find_Prop(hTypeData, NULL, VE_PID_IFT_BASETYPENAME)) 
		{
			::DATA_Add_Prop( hTypeData, NULL, VE_PID_IFT_BASETYPE, AVP_dword(DEFAULT_VAR_TYPE), 0 );
			::DATA_Add_Prop( hTypeData, NULL, VE_PID_IFT_BASETYPENAME, AVP_dword(::GTypeID2TypeName(DEFAULT_VAR_TYPE)), 0 );
			SetModified( IsModified() | !bLinkedToFile );
		}
			
		CreatePropertyNode(newItems, nFirstItemLevel, hTypeData, VE_PID_IFT_BASETYPENAME, bEditable);
	}

	LoadShortCommentStuff( hTypeData, newItems, nFirstItemLevel, bLinkedToFile );
	LoadLargeCommentStuff( hTypeData, newItems, nFirstItemLevel, bLinkedToFile );

	if(!bSeparator)
	{
		LoadBehaviorCommentStuff( hTypeData, newItems, nFirstItemLevel, bLinkedToFile );
		LoadImplCommentStuff(hTypeData, newItems, nFirstItemLevel, bLinkedToFile);
	}
}

// ------------------------------------------------------------------------------------------
void CVisualEditorDlg::LoadInterfaceConstantsNodes( 
	HDATA hRootData, CCTItemsArray &newItems, int nFirstItemLevel, BOOL bLinkedToFile ) 
{
	MAKE_ADDR1(nAddr1, VE_PID_IF_CONSTANTS);

	HDATA hData;
	if ( !(hData = ::DATA_Find(hRootData, nAddr1)) ) 
	{
		hData = ::DATA_Add( hRootData, NULL, VE_PID_IF_CONSTANTS, 0, 0 );
		OnCreateData(hData);
	}

	CreateStringNode(newItems, nFirstItemLevel, hData, IDS_CONSTANTS, FALSE);

	++ nFirstItemLevel;

	if ( hData ) 
	{
		HDATA hCurrData = ::DATA_Get_First( hData, NULL );
		while ( hCurrData ) 
		{
			LoadOneInterfaceConstantStuff( hCurrData, newItems, nFirstItemLevel, bLinkedToFile );
			hCurrData = ::DATA_Get_Next( hCurrData, NULL );
		}
	}
	
	if ( !bLinkedToFile ) 
		CreateStringNode(newItems, nFirstItemLevel, NULL, IDS_NEWCONSTANT, TRUE, FALSE, sdt_Constant);
}

// ---
void CVisualEditorDlg::LoadOneInterfaceConstantStuff( 
	HDATA hTypeData, CCTItemsArray &newItems, int nFirstItemLevel, BOOL bLinkedToFile ) 
{
	OnLoadData(hTypeData, bLinkedToFile );
	BOOL bEditable = IsEditableData(hTypeData, bLinkedToFile);

	CreatePropertyNode(newItems, nFirstItemLevel, hTypeData, VE_PID_IFC_CONSTANTNAME, bEditable, bEditable, sdt_Constant);

	++nFirstItemLevel;

	BOOL bSeparator = ::IsSeparator(hTypeData);
	if ( !bSeparator  ) 
	{
		if ( !::DATA_Find_Prop(hTypeData, NULL, VE_PID_IFC_BASETYPENAME) ) 
		{
			::DATA_Add_Prop( hTypeData, NULL, VE_PID_IFC_BASETYPE, AVP_dword(DEFAULT_VAR_TYPE), 0 );
			::DATA_Add_Prop( hTypeData, NULL, VE_PID_IFC_BASETYPENAME, AVP_dword(::GTypeID2TypeName(DEFAULT_VAR_TYPE)), 0 );
			SetModified( IsModified() | !bLinkedToFile );
		}

		CreatePropertyNode(newItems, nFirstItemLevel, hTypeData, VE_PID_IFC_BASETYPENAME, bEditable);
		CreatePropertyNode(newItems, nFirstItemLevel, hTypeData, VE_PID_IFC_VALUE, bEditable);
	}

	LoadShortCommentStuff( hTypeData, newItems, nFirstItemLevel, bLinkedToFile );
	LoadLargeCommentStuff( hTypeData, newItems, nFirstItemLevel, bLinkedToFile );

	if (!bSeparator) 
	{
		LoadBehaviorCommentStuff( hTypeData, newItems, nFirstItemLevel, bLinkedToFile );
		LoadImplCommentStuff(hTypeData, newItems, nFirstItemLevel, bLinkedToFile);
	}
}

// ------------------------------------------------------------------------------------------
void CVisualEditorDlg::LoadInterfaceMessagesNodes( 
	HDATA hRootData, CCTItemsArray &newItems, int nFirstItemLevel, BOOL bLinkedToFile ) 
{
	MAKE_ADDR1(nAddr1, VE_PID_PL_MESSAGES);

	// add 'messages classes' parent node
	HDATA hData = ::DATA_Find(hRootData, nAddr1);
	if ( !hData ) 
	{
		hData = ::DATA_Add( hRootData, NULL, VE_PID_PL_MESSAGES, 0, 0 );
		OnCreateData(hData);
	}
	
	CreateStringNode(newItems, nFirstItemLevel, hData, IDS_CLASSES_OF_MESSAGES, FALSE);

	// add class props and all messages in class 
	if ( hData ) 
	{
		HDATA hCurrData = ::DATA_Get_First( hData, NULL );
		while ( hCurrData ) 
		{
			LoadOneInterfaceMessagesClassStuff( hCurrData, newItems, nFirstItemLevel+1, bLinkedToFile );
			hCurrData = ::DATA_Get_Next( hCurrData, NULL );
		}
	}
	
	// and finally add 'new message class' node
	if ( !bLinkedToFile ) 
		CreateStringNode(newItems, nFirstItemLevel+1, NULL, IDS_NEW_MESSAGE_CLASS, TRUE, FALSE, sdt_MessageClass);
}

// ---
void CVisualEditorDlg::LoadOneInterfaceMessagesClassStuff( 
	HDATA i_hTypeData, CCTItemsArray &i_oNewItems, int i_nFirstItemLevel, BOOL i_bLinkedToFile ) 
{
	OnLoadData(i_hTypeData, i_bLinkedToFile );
	BOOL bEditable = IsEditableData(i_hTypeData, i_bLinkedToFile);

	//-----------------------------------------------------------
	// adding class name
	CreatePropertyNode(i_oNewItems, i_nFirstItemLevel, i_hTypeData, 
		VE_PID_IFM_MESSAGECLASSNAME, bEditable, bEditable, sdt_MessageClass);

	//-----------------------------------------------------------
	// adding message class scalar props

	// class identificator
	CreatePropertyNode(i_oNewItems, i_nFirstItemLevel+1, i_hTypeData, VE_PID_IFM_MESSAGECLASSID, bEditable);

	LoadShortCommentStuff( i_hTypeData, i_oNewItems, i_nFirstItemLevel+1, i_bLinkedToFile );
	LoadLargeCommentStuff( i_hTypeData, i_oNewItems, i_nFirstItemLevel+1, i_bLinkedToFile );
	LoadImplCommentStuff(i_hTypeData, i_oNewItems, i_nFirstItemLevel+1, i_bLinkedToFile);
	
	++i_nFirstItemLevel;

	CreateStringNode(i_oNewItems, i_nFirstItemLevel, i_hTypeData, IDS_MESSAGES, FALSE);

	++i_nFirstItemLevel;

	// add all messages in class 
	HDATA hCurrData = NULL;
	if ( i_hTypeData ) 
		hCurrData = ::DATA_Get_First( i_hTypeData, NULL );

	while ( hCurrData ) 
	{
		LoadOneInterfaceMessagesClassMessagesStuff(hCurrData, i_oNewItems, i_nFirstItemLevel, bEditable, i_bLinkedToFile);
		hCurrData = ::DATA_Get_Next( hCurrData, NULL );
	}
	
	// and finally add 'new message' node
	CreateStringNode(i_oNewItems, i_nFirstItemLevel, NULL, IDS_NEW_MESSAGE, TRUE, FALSE, sdt_Message);
}

// ---
void CVisualEditorDlg::LoadOneInterfaceMessagesClassMessagesStuff( 
	HDATA i_hData, CCTItemsArray &i_oNewItems, int i_nFirstItemLevel, BOOL bEditable, BOOL i_bLinkedToFile)
{
	OnLoadData(i_hData, i_bLinkedToFile );
	bEditable = IsEditableData(i_hData, i_bLinkedToFile);

	CreatePropertyNode(i_oNewItems, i_nFirstItemLevel, i_hData, VE_PID_IFM_MESSAGENAME, 
		bEditable, bEditable, sdt_Message);
	
	++i_nFirstItemLevel;

	CreatePropertyNode(i_oNewItems, i_nFirstItemLevel, i_hData, VE_PID_IFM_MESSAGEID, bEditable);
	CreatePropertyNode(i_oNewItems, i_nFirstItemLevel, i_hData, VE_PID_IFM_MESSAGE_SEND_POINT, bEditable);
	CreatePropertyNode(i_oNewItems, i_nFirstItemLevel, i_hData, VE_PID_IFM_MESSAGECONTEXT, bEditable);
	CreatePropertyNode(i_oNewItems, i_nFirstItemLevel, i_hData, VE_PID_IFM_MESSAGEDATA, bEditable);
	
	LoadShortCommentStuff( i_hData, i_oNewItems, i_nFirstItemLevel, false );
	LoadLargeCommentStuff( i_hData, i_oNewItems, i_nFirstItemLevel, false );
	LoadImplCommentStuff(i_hData, i_oNewItems, i_nFirstItemLevel, false);
}

// ------------------------------------------------------------------------------------------
// we do not check error code values uniqueness here
static BOOL CheckErrorCodesValues(HDATA hRootData)
{
	HPROP hProp = ::DATA_Find_Prop( hRootData, NULL, VE_PID_IFE_ERRORCODEVALUE );

	if(NULL == hProp)
		return FALSE;

	AVP_dword dwMaxValue = ::GetPropNumericValueAsDWord( hProp );
	
	for(HDATA hData = ::DATA_Get_First( hRootData, NULL );
		hData;
		hData = ::DATA_Get_Next( hData, NULL ))
	{
		HPROP hProp = ::DATA_Find_Prop( hData, NULL, VE_PID_IFE_ERRORCODEVALUE );
		if(NULL == hProp)
			return FALSE;

		if(dwMaxValue < ::GetPropNumericValueAsDWord( hProp ))
			return FALSE;
	}

	return TRUE;
}

static BOOL CollectInterfaceErrorCodes( HDATA hRootData ) 
{
	if(CheckErrorCodesValues(hRootData))
		return FALSE;

	HDATA hData = ::DATA_Get_First( hRootData, NULL );
	AVP_dword dwMaxValue;
	for(dwMaxValue = 1; hData; dwMaxValue++)
	{
		SetDwordProp(hData, VE_PID_IFE_ERRORCODEVALUE, dwMaxValue);
		hData = ::DATA_Get_Next( hData, NULL ); 
	}

	if(1 == dwMaxValue)
		dwMaxValue = 0;

	SetDwordProp(hRootData, VE_PID_IFE_ERRORCODEVALUE, dwMaxValue);
	return TRUE;
}

// ---
void CVisualEditorDlg::LoadInterfaceErrorCodeNodes( 
	HDATA hRootData, CCTItemsArray &newItems, int nFirstItemLevel, BOOL bLinkedToFile ) 
{
	HDATA hData;

	MAKE_ADDR1(nAddr1, VE_PID_IF_ERRORCODES);
	if ( !(hData = ::DATA_Find(hRootData, nAddr1)) ) 
	{
		hData = ::DATA_Add( hRootData, NULL, VE_PID_IF_ERRORCODES, 0, 0 );
		SetDwordProp(hData, VE_PID_IFE_ERRORCODEVALUE, 0);
		OnCreateData(hData);
	}
	else 
	{
		if(CollectInterfaceErrorCodes( hData ))
			SetModified( true );
	}

	CreateStringNode(newItems, nFirstItemLevel, hData, IDS_ERRORCODES, FALSE);

	++nFirstItemLevel;
	if ( hData ) 
	{
		HDATA hCurrData = ::DATA_Get_First( hData, NULL );
		while ( hCurrData ) {
			LoadOneInterfaceErrorCodeStuff( hCurrData, newItems, nFirstItemLevel, bLinkedToFile );
			hCurrData = ::DATA_Get_Next( hCurrData, NULL );
		}
	}

	if(!bLinkedToFile)
		CreateStringNode(newItems, nFirstItemLevel, NULL, IDS_NEWERRORCODE, TRUE, FALSE, sdt_ErrorCode);
}

// ---
void CVisualEditorDlg::LoadOneInterfaceErrorCodeStuff( 
	HDATA hErrorCodeData, CCTItemsArray &newItems, int nFirstItemLevel, BOOL bLinkedToFile ) 
{
	OnLoadData(hErrorCodeData, bLinkedToFile );
	BOOL bEditable = IsEditableData(hErrorCodeData, bLinkedToFile);
	CreatePropertyNode(newItems, nFirstItemLevel, hErrorCodeData, VE_PID_IFE_ERRORCODENAME, bEditable, bEditable, sdt_ErrorCode);

	++nFirstItemLevel;

	if ( !::IsSeparator(hErrorCodeData)) 
		CreatePropertyNode(newItems, nFirstItemLevel, hErrorCodeData, VE_PID_IFE_ISWARNING, bEditable );

	LoadShortCommentStuff( hErrorCodeData, newItems, nFirstItemLevel, bLinkedToFile );
	LoadLargeCommentStuff( hErrorCodeData, newItems, nFirstItemLevel, bLinkedToFile );

	if ( !::IsSeparator(hErrorCodeData)) 
	{
		LoadBehaviorCommentStuff( hErrorCodeData, newItems, nFirstItemLevel, bLinkedToFile );
		LoadImplCommentStuff(hErrorCodeData, newItems, nFirstItemLevel, bLinkedToFile);
	}
}

// ------------------------------------------------------------------------------------------
void CVisualEditorDlg::LoadInterfaceDataStructuresNodes( 
	HDATA hRootData, CCTItemsArray &newItems, int nFirstItemLevel, BOOL bLinkedToFile ) 
{
	HDATA hData;
	MAKE_ADDR1(nAddr1, VE_PID_IF_DATASTRUCTURES);
	if ( !(hData = ::DATA_Find(hRootData, nAddr1)) ) 
	{
		hData = ::DATA_Add( hRootData, NULL, VE_PID_IF_DATASTRUCTURES, 0, 0 );
		OnCreateData(hData);
	}

	CreateStringNode(newItems, nFirstItemLevel, hData, IDS_DATASTRUCTURE, FALSE);

	++nFirstItemLevel;

	hData = ::DATA_Get_First( hData, NULL );
	while ( hData ) 
	{
		LoadOneInterfaceDataStructureStuff( hData, newItems, nFirstItemLevel );
		hData = ::DATA_Get_Next( hData, NULL );
	}

	CreateStringNode(newItems, nFirstItemLevel, NULL, IDS_NEWDATASRUCTURE, TRUE, FALSE, sdt_DataStructure);
}

// ---
void CVisualEditorDlg::LoadOneInterfaceDataStructureStuff(
	HDATA hDataStructureData, CCTItemsArray &newItems, int nFirstItemLevel) 
{
	if ( ::DATA_Find_Prop( hDataStructureData, NULL, VE_PID_IFD_DATASTRUCTURENAME ) ) 
	{
		OnLoadData(hDataStructureData, FALSE );

		CreatePropertyNode(newItems, nFirstItemLevel, hDataStructureData, 
			VE_PID_IFD_DATASTRUCTURENAME, TRUE, TRUE, sdt_DataStructure);

		MAKE_ADDR1(nStructAddr, VE_PID_IF_DATASTRUCTURES);
		HDATA hStructuresData = ::DATA_Find( hDataStructureData, nStructAddr );
		if ( hStructuresData ) 
		{
	  		CreateStringNode(newItems, nFirstItemLevel+1, hStructuresData, IDS_DATASTRUCTURE, FALSE);

			HDATA hCurrData = ::DATA_Get_First( hStructuresData, NULL );
			while ( hCurrData ) 
			{
				LoadOneInterfaceDataStructureStuff( hCurrData, newItems, nFirstItemLevel + 2 );
				hCurrData = ::DATA_Get_Next( hCurrData, NULL );
			}

			CreateStringNode(newItems, nFirstItemLevel+2, NULL, IDS_NEWDATASRUCTURE, TRUE, FALSE, sdt_DataStructure);
		}
		
		MAKE_ADDR1(nMembersAddr, VE_PID_IF_DATASTRUCTUREMEMBERS);
		HDATA hMembersData = ::DATA_Find( hDataStructureData, nMembersAddr );
		if ( hMembersData ) 
		{
	  		CreateStringNode(newItems, nFirstItemLevel+1, hMembersData, IDS_DATASTRUCTUREMEMBERS, FALSE);

			HDATA hCurrData = ::DATA_Get_First( hMembersData, NULL );
			while ( hCurrData ) 
			{
				LoadOneInterfaceDataStructureMemberStuff( hCurrData, newItems, nFirstItemLevel + 2 );
				hCurrData = ::DATA_Get_Next( hCurrData, NULL );
			}

	  		CreateStringNode(newItems, nFirstItemLevel+2, NULL, IDS_NEWDATASRUCTUREMEMBER, TRUE, FALSE, sdt_DataStructureMember);
		}
	}
}

// ---
void CVisualEditorDlg::LoadOneInterfaceDataStructureMemberStuff(
	HDATA hDataStructureMemberData, CCTItemsArray &newItems, int nFirstItemLevel) 
{
	HPROP hProp = ::DATA_Find_Prop( hDataStructureMemberData, NULL, VE_PID_IFDM_MEMBERNAME );
	if ( hProp ) 
	{
		CreatePropertyNode(newItems, nFirstItemLevel, hDataStructureMemberData, 
			VE_PID_IFDM_MEMBERNAME, TRUE, TRUE, sdt_DataStructureMember);

		++nFirstItemLevel;

		CreatePropertyNode(newItems, nFirstItemLevel, hDataStructureMemberData, VE_PID_IFDM_TYPENAME);
		CreatePropertyNode(newItems, nFirstItemLevel, hDataStructureMemberData, VE_PID_IFDM_ISARRAY);
		CreatePropertyNode(newItems, nFirstItemLevel+1, hDataStructureMemberData, VE_PID_IFDM_ARRAYBOUND);

		LoadShortCommentStuff( hDataStructureMemberData, newItems, nFirstItemLevel, false );
	}
}

// ------------------------------------------------------------------------------------------
void CVisualEditorDlg::LoadInterfacePropertiesNodes( 
	HDATA hRootData, CCTItemsArray &newItems, int nFirstItemLevel, BOOL bLinkedToFile ) 
{
	HDATA hData;
	MAKE_ADDR1(nAddr1, VE_PID_IF_PROPERTIES);
	if ( !(hData = ::DATA_Find(hRootData, nAddr1)) ) 
	{
		hData = ::DATA_Add( hRootData, NULL, VE_PID_IF_PROPERTIES, 0, 0 );
		OnCreateData(hData);
	}

	CreateStringNode(newItems, nFirstItemLevel, hData, IDS_PROPERTIES, FALSE);

	++nFirstItemLevel;

	if (IsImplementation() && !bLinkedToFile ) 
	{
		HDATA hDad = ::DATA_Get_Dad(hData, NULL);
		CreatePropertyNode(newItems, nFirstItemLevel, hDad, VE_PID_IF_LASTCALLPROP);
		CreatePropertyNode(newItems, nFirstItemLevel+1, hDad, VE_PID_IFLCP_READFUNC);
		CreatePropertyNode(newItems, nFirstItemLevel+1, hDad, VE_PID_IFLCP_WRITEFUNC);
	}

	LoadNewHardPropertiesDescriptionTreeStuff( hData, newItems, nFirstItemLevel, bLinkedToFile );
	
	HDATA hCurrData = hData ? ::DATA_Get_First( hData, NULL ) : NULL;

	while ( hCurrData ) 
	{
		if(!IsBoolPropSet(hCurrData, VE_PID_IFP_HARDPREDEFINED))
			LoadOneInterfacePropertyStuff( hCurrData, newItems, nFirstItemLevel, bLinkedToFile );
		hCurrData = ::DATA_Get_Next( hCurrData, NULL );
	}

	if ( !bLinkedToFile ) 
		CreateStringNode(newItems, nFirstItemLevel, NULL, IDS_NEWPROPERTY, TRUE, FALSE, sdt_Property, MD::eStrDDList);
}

// ---
void CVisualEditorDlg::LoadNewHardPropertiesDescriptionTreeStuff( 
	HDATA hParentData, CCTItemsArray &newItems, int nFirstItemLevel, BOOL bLinkedToFile ) 
{
	for ( int i=0,c=gHardProps.Count(); i<c; i++ ) 
	{
		PredefinedProp *pProp = gHardProps[i];

		HDATA hData = ::FindPropDataByName( hParentData, pProp->m_pName );
		if ( !hData ) 
		{
			hData = CreateDataData(hParentData, VE_NT_PROPERTY, pProp->m_pName);
			SetModified( IsModified() | !bLinkedToFile );
		}

		HPROP hProp = ::DATA_Find_Prop( hData, NULL, VE_PID_IFP_PROPNAME );
		PreparePredefinedPropComments( pProp, hData );

		LoadNewHardPropertyDescriptionTreeStuff( hData, newItems, nFirstItemLevel, bLinkedToFile );
	}

	HDATA hCurrData = ::DATA_Get_First( hParentData, NULL );
	while ( hCurrData ) 
	{
		// skip hard predefined properties
		if (! ::CheckHardPropExists(hCurrData) && 
			IsBoolPropSet(hCurrData, VE_PID_IFP_HARDPREDEFINED)) 
		{
			LoadNewHardPropertyDescriptionTreeStuff( hCurrData, newItems, nFirstItemLevel, bLinkedToFile );
		}

		hCurrData = ::DATA_Get_Next( hCurrData, NULL );
	}
}

// ---
void CVisualEditorDlg::LoadNewHardPropertyDescriptionTreeStuff( 
	HDATA hData, CCTItemsArray &newItems, int nFirstItemLevel, BOOL bLinkedToFile ) 
{
	if(SetDwordProp(hData, VE_PID_IFP_PREDEFINED, TRUE))
		SetModified( IsModified() | !bLinkedToFile );

	if(SetDwordProp(hData, VE_PID_IFP_HARDPREDEFINED, TRUE))
		SetModified( IsModified() | !bLinkedToFile );

	CreatePropertyNode(newItems, nFirstItemLevel, hData, VE_PID_IFP_PROPNAME, FALSE, FALSE, sdt_Property);
	LoadInterfacePropertyStuff( hData, newItems, nFirstItemLevel + 1, TRUE, bLinkedToFile );
}

// ---
void CVisualEditorDlg::LoadOneInterfacePropertyStuff( 
HDATA hPropData, CCTItemsArray &newItems, int nFirstItemLevel, BOOL bLinkedToFile ) 
{
	OnLoadData(hPropData, FALSE );

	if(CheckPropSet(hPropData, VE_PID_IFP_PREDEFINED, FALSE) &&	!bLinkedToFile)
		SetModified( TRUE );

	BOOL bEditable = IsEditableData(hPropData, bLinkedToFile);
	if(bEditable)
	{
		BOOL bPredefinedProp		= IsBoolPropSet(hPropData, VE_PID_IFP_PREDEFINED);
		BOOL bHardPredefinedProp	= IsBoolPropSet(hPropData, VE_PID_IFP_HARDPREDEFINED);
		BOOL bEditable = FALSE;

		if (IsPrototype()) 
			bEditable = !bHardPredefinedProp && !bPredefinedProp;
		else
			bEditable = !bHardPredefinedProp;
	}

	CreatePropertyNode(newItems, nFirstItemLevel, hPropData, VE_PID_IFP_PROPNAME, 
		bEditable, bEditable, sdt_Property);
	
	LoadInterfacePropertyStuff( hPropData, newItems, nFirstItemLevel + 1, bEditable, bLinkedToFile );
}

// ---
void CVisualEditorDlg::LoadInterfacePropertyStuff( 
	HDATA hData, CCTItemsArray &newItems, int nFirstItemLevel, BOOL bEditable, BOOL bLinkedToFile ) 
{
	BOOL bSeparator = ::IsSeparator(hData);
	if ( !bSeparator ) 
	{
		CreatePropertyNode(newItems, nFirstItemLevel, hData, VE_PID_IFP_DIGITALID, bEditable);
		CreatePropertyNode(newItems, nFirstItemLevel, hData, VE_PID_IFP_TYPENAME, bEditable);

		if(IsImplementation() && !bLinkedToFile )
		{
			bEditable = TRUE;

			CreatePropertyNode(newItems, nFirstItemLevel, hData, VE_PID_IFP_SCOPE, bEditable, FALSE, 
				sdt_PropScopeLocal, VE_IFP_LOCAL, IDS_PROPLOCAL);
	
			// local scope stuff
			CreatePropertyNode(newItems, nFirstItemLevel+1, hData, VE_PID_IFPLS_WRITABLEDURINGINIT, bEditable);
			CreatePropertyNode(newItems, nFirstItemLevel+1, hData, VE_PID_IFPLS_REQUIRED, bEditable);

			LoadInterfacePropertyLocalMemberStuff( hData, newItems, nFirstItemLevel + 1, bEditable);
			LoadInterfacePropertyLocalModeStuff( hData, newItems, nFirstItemLevel + 1, bEditable);

			CreatePropertyNode(newItems, nFirstItemLevel, hData, VE_PID_IFP_SCOPE, bEditable, FALSE, 
				sdt_PropScopeShared, VE_IFP_SHARED, IDS_PROPSHARED);
							 
			// shared scope stuff
			CreatePropertyNode(newItems, nFirstItemLevel+1, hData, VE_PID_IFPSS_VARIABLENAME, bEditable);
			CreatePropertyNode(newItems, nFirstItemLevel+1, hData, VE_PID_IFPSS_VARIABLEDEFVALUE, bEditable);
		}
	}

	LoadShortCommentStuff( hData, newItems, nFirstItemLevel, bLinkedToFile );
	LoadLargeCommentStuff( hData, newItems, nFirstItemLevel, bLinkedToFile );

	if ( !bSeparator ) 
	{
		LoadBehaviorCommentStuff( hData, newItems, nFirstItemLevel, bLinkedToFile );
		LoadImplCommentStuff(hData, newItems, nFirstItemLevel, bLinkedToFile);
	}
}

// ---
void CVisualEditorDlg::LoadInterfacePropertyLocalMemberStuff( 
	HDATA hData, CCTItemsArray &newItems, int nFirstItemLevel, BOOL i_bEditable ) 
{
	CreatePropertyNode(newItems, nFirstItemLevel, hData, VE_PID_IFPLS_MEMBER, i_bEditable);

	++nFirstItemLevel;

	CreatePropertyNode(newItems, nFirstItemLevel, hData, VE_PID_IFPLS_MEMBER_MODE,
		i_bEditable, FALSE, sdt_PropLocalMemberRead, VE_IFPLS_MEMBER_READ, IDS_PROPREAD);

	CreatePropertyNode(newItems, nFirstItemLevel, hData, VE_PID_IFPLS_MEMBER_MODE,
		i_bEditable, FALSE, sdt_PropLocalMemberWrite, VE_IFPLS_MEMBER_WRITE, IDS_PROPWRITE);

	CreatePropertyNode(newItems, nFirstItemLevel, hData, VE_PID_IFPLS_MEMBER_MODE,
		i_bEditable, FALSE, sdt_PropLocalMemberReadWrite, VE_IFPLS_MEMBER_READWRITE, IDS_PROPREADWRITE);
}

// ---
void CVisualEditorDlg::LoadInterfacePropertyLocalModeStuff( 
	HDATA hData, CCTItemsArray &newItems, int nFirstItemLevel, BOOL i_bEditable ) 
{
	CreatePropertyNode(newItems, nFirstItemLevel, hData, VE_PID_IFPLS_MODE, i_bEditable, FALSE, 
		sdt_PropLocalNone, VE_IFPLS_NONE, IDS_PROPNONE);

	CreatePropertyNode(newItems, nFirstItemLevel, hData, VE_PID_IFPLS_MODE, i_bEditable, FALSE, 
		sdt_PropLocalRead, VE_IFPLS_READ, IDS_PROPREAD);

	CreatePropertyNode(newItems, nFirstItemLevel+1, hData, VE_PID_IFPLS_READFUNC, i_bEditable);

	CreatePropertyNode(newItems, nFirstItemLevel, hData, VE_PID_IFPLS_MODE, i_bEditable, FALSE, 
		sdt_PropLocalWrite, VE_IFPLS_WRITE, IDS_PROPWRITE);

	CreatePropertyNode(newItems, nFirstItemLevel+1, hData, VE_PID_IFPLS_WRITEFUNC, i_bEditable);

	CreatePropertyNode(newItems, nFirstItemLevel, hData, VE_PID_IFPLS_MODE, i_bEditable, FALSE, 
		sdt_PropLocalReadWrite, VE_IFPLS_READWRITE, IDS_PROPREADWRITE);

	CreatePropertyNode(newItems, nFirstItemLevel+1, hData, VE_PID_IFPLS_READFUNC, i_bEditable);
	CreatePropertyNode(newItems, nFirstItemLevel+1, hData, VE_PID_IFPLS_WRITEFUNC, i_bEditable);
}

// ------------------------------------------------------------------------------------------
void CVisualEditorDlg::LoadInterfaceMethodsNodes( 
	HDATA hRootData, CCTItemsArray &newItems, int nFirstItemLevel, BOOL bLinkedToFile ) 
{
	HDATA hData;

	MAKE_ADDR1(nAddr2, VE_PID_IF_METHODS);
	if ( !(hData = ::DATA_Find(hRootData, nAddr2)) ) 
	{
		hData = ::DATA_Add( hRootData, NULL, VE_PID_IF_METHODS, 0, 0 );
		OnCreateData(hData);
	}

	CreateStringNode(newItems, nFirstItemLevel, hData, IDS_METHODS, FALSE);

	++nFirstItemLevel;

	HDATA hCurrData = NULL;
	if ( hData ) 
		hCurrData = ::DATA_Get_First( hData, NULL );

	while ( hCurrData ) 
	{
		LoadOneInterfaceMethodStuff( hCurrData, newItems, nFirstItemLevel, bLinkedToFile);
		hCurrData = ::DATA_Get_Next( hCurrData, NULL );
	}

	CreateStringNode(newItems, nFirstItemLevel, NULL, IDS_NEWMETHOD, TRUE, FALSE, sdt_Method);
}

// ---
// load method name, its parameters
void CVisualEditorDlg::LoadOneInterfaceMethodStuff( 
	HDATA hMethod, CCTItemsArray &newItems, int nFirstItemLevel, BOOL bLinkedToFile) 
{
	OnLoadData(hMethod, bLinkedToFile );
	BOOL bEditable = IsEditableData(hMethod, bLinkedToFile);

	if(IsBoolPropSet(hMethod, VE_PID_IFM_EXTENDED))
		bEditable = TRUE;

	CreatePropertyNode(newItems, nFirstItemLevel, hMethod, VE_PID_IFM_METHODNAME, bEditable, bEditable, sdt_Method);

	BOOL bSeparator = ::IsSeparator(hMethod);

	if ( !bSeparator ) 
	{
		LoadInterfaceMethodResultNameStuff( hMethod, newItems, nFirstItemLevel + 1, bLinkedToFile, bEditable);
		LoadInterfaceMethodResultDefValueStuff( hMethod, newItems, nFirstItemLevel + 1, bLinkedToFile, bEditable );
	}
	
	LoadShortCommentStuff( hMethod, newItems, nFirstItemLevel + 1, bLinkedToFile );
	LoadLargeCommentStuff( hMethod, newItems, nFirstItemLevel + 1, bLinkedToFile );
	
	if ( !bSeparator ) 
	{
		LoadBehaviorCommentStuff( hMethod, newItems, nFirstItemLevel + 1, bLinkedToFile );
		LoadValueCommentStuff( hMethod, newItems, nFirstItemLevel + 1, bLinkedToFile );
		LoadImplCommentStuff(hMethod, newItems, nFirstItemLevel+1, bLinkedToFile);
	}
	
	// load method parameters
	if ( !bSeparator ) 
		LoadInterfaceMethodParametersDescriptionNodes( hMethod, newItems, nFirstItemLevel, bLinkedToFile, bEditable );
}

// ---
void CVisualEditorDlg::LoadInterfaceMethodResultNameStuff( 
	HDATA hData, CCTItemsArray &newItems, int nFirstItemLevel, BOOL bLinkedToFile, BOOL i_bEditable ) 
{
	CreatePropertyNode(newItems, nFirstItemLevel, hData, VE_PID_IFM_RESULTTYPENAME, i_bEditable);
}

// ---
void CVisualEditorDlg::LoadInterfaceMethodResultDefValueStuff( 
	HDATA hData, CCTItemsArray &newItems, int nFirstItemLevel, BOOL bLinkedToFile, BOOL i_bEditable ) 
{
	CreatePropertyNode(newItems, nFirstItemLevel, hData, VE_PID_IFM_RESULTDEFVALUE, i_bEditable);
}

// ---
void CVisualEditorDlg::LoadInterfaceMethodParametersDescriptionNodes( 
	HDATA hRootData, CCTItemsArray &newItems, int nFirstItemLevel, BOOL bLinkedToFile, BOOL i_bEditable ) 
{
	CreateStringNode(newItems, (++nFirstItemLevel)++, hRootData, IDS_METHODPARAMETERS, FALSE, FALSE, sdt_Parameters);

	HDATA hCurrData = NULL;
	if ( hRootData ) 
		hCurrData = ::DATA_Get_First( hRootData, NULL );

	while ( hCurrData ) 
	{
		LoadOneInterfaceMethodParameterStuff( hCurrData, newItems, nFirstItemLevel, bLinkedToFile, i_bEditable);
		hCurrData = ::DATA_Get_Next( hCurrData, NULL );
	}

	if(i_bEditable)
		CreateStringNode(newItems, nFirstItemLevel, NULL, IDS_NEWMETHODPARAMETER, TRUE, FALSE, sdt_MethodParam);
}

// ---
// parameter properties loading
void CVisualEditorDlg::LoadOneInterfaceMethodParameterStuff(
	HDATA hParameterData, CCTItemsArray &newItems, int nFirstItemLevel, BOOL bLinkedToFile, BOOL i_bEditable ) 
{
	CreatePropertyNode(newItems, nFirstItemLevel, hParameterData, VE_PID_IFMP_PARAMETERNAME, 
		i_bEditable, i_bEditable, sdt_MethodParam);

	HPROP hProp = ::DATA_Find_Prop( hParameterData, NULL, VE_PID_IFMP_PARAMETERNAME );
	CAPointer<char> pValue;
	if ( hProp )
		pValue = ::GetPropValueAsString( hProp, NULL );

	if ( !::IsEllipsis(pValue) )
	{
		++nFirstItemLevel;

		CreatePropertyNode(newItems, nFirstItemLevel, hParameterData, VE_PID_IFMP_ISPOINTER, i_bEditable);
		CreatePropertyNode(newItems, nFirstItemLevel+1, hParameterData, VE_PID_IFMP_IS_DOUBLE_POINTER, i_bEditable);
		LoadInterfaceMethodParamConstStuff( hParameterData, hProp, newItems, nFirstItemLevel, bLinkedToFile, i_bEditable );
		CreatePropertyNode(newItems, nFirstItemLevel, hParameterData, VE_PID_IFMP_TYPENAME, i_bEditable);

		LoadShortCommentStuff( hParameterData, newItems, nFirstItemLevel, bLinkedToFile );
		LoadLargeCommentStuff( hParameterData, newItems, nFirstItemLevel, bLinkedToFile );
		LoadBehaviorCommentStuff( hParameterData, newItems, nFirstItemLevel, bLinkedToFile );
		LoadImplCommentStuff(hParameterData, newItems, nFirstItemLevel, bLinkedToFile);
	}
}

// ---
void CVisualEditorDlg::LoadInterfaceMethodParamConstStuff( 
	HDATA hData, HPROP , CCTItemsArray &newItems, int nFirstItemLevel, BOOL bLinkedToFile , BOOL i_bEditable ) 
{
	// check if const property value is valid

	BOOL bPointer = FALSE;
	HPROP hProp = ::DATA_Find_Prop( hData, NULL, VE_PID_IFMP_ISPOINTER );
	if ( hProp ) 
		bPointer = ::GetPropNumericValueAsDWord( hProp );

	// let's see if the type is pointer type
	if ( !bPointer )
	{
		hProp = ::DATA_Find_Prop(hData,NULL,VE_PID_IFMP_TYPE);
		if(hProp)
		{
			AVP_dword val = ::GetPropNumericValueAsDWord( hProp );
			bPointer = TYPE_IS_POINTER(val);
		}
	}

	// only none-const values are allowed in that case
	if(!bPointer)
	{
		hProp = ::DATA_Find_Prop(hData,NULL,VE_PID_IFMP_ISCONST);

		if(NULL == hProp)
		{
			hProp = CreateDataProp(hData, VE_PID_IFMP_ISCONST);
			SetModified(true);
		}

		AVP_dword dwOldConstVal = ::GetPropNumericValueAsDWord( hProp );

		if(dwOldConstVal)
		{
			::PROP_Set_Val( hProp, AVP_dword(0), 0 );
			SetModified(TRUE);
		}
	}

	CreatePropertyNode(newItems, nFirstItemLevel, hData, VE_PID_IFMP_ISCONST, i_bEditable);
}

// ------------------------------------------------------------------------------------------
static BOOL CompareDataProps ( HDATA hFData, HDATA hSData ) {
	BOOL bResult = TRUE;
	HPROP hFCurrProp = ::DATA_Get_First_Prop( hFData, NULL );
	HPROP hSCurrProp = ::DATA_Get_First_Prop( hSData, NULL );
	while( hFCurrProp && hSCurrProp && bResult ) {
		CAPointer<char> pFValue = ::GetPropValueAsString( hFCurrProp, NULL );
		CAPointer<char> pSValue = ::GetPropValueAsString( hSCurrProp, NULL );

		if ( LPCSTR(pFValue) && LPCSTR(pSValue) )
			bResult = !lstrcmp( pFValue, pSValue );

		hFCurrProp = ::PROP_Get_Next( hFCurrProp );
		hSCurrProp = ::PROP_Get_Next( hSCurrProp );
	}
	return bResult;
}

static BOOL CompareDataDatas( HDATA hFData, HDATA hSData ) {
	BOOL bResult = ::CompareDataProps( hFData, hSData );

	if ( bResult ) {
		HDATA hFCurrData = ::DATA_Get_First( hFData, NULL );
		HDATA hSCurrData = ::DATA_Get_First( hSData, NULL );
		while( hFCurrData && hSCurrData && bResult ) {
			bResult = ::CompareDataProps ( hFCurrData, hSCurrData );
			hFCurrData = ::DATA_Get_Next( hFCurrData, NULL );
			hSCurrData = ::DATA_Get_Next( hSCurrData, NULL );
		}

		if ( hFCurrData || hSCurrData )
			bResult = FALSE;
	}

	return bResult;
}

static BOOL CompareTwoSMData( HDATA hFData, HDATA hSData, BOOL &bNameTheSame ) {
	BOOL bResult = FALSE;
	if ( hFData && hFData ) {
		CAPointer<char> pFName;
		CAPointer<char> pSName;
		HPROP hNameProp = ::DATA_Find_Prop( hFData, NULL, VE_PID_IFM_METHODNAME );
		if ( hNameProp )
			pFName = ::GetPropValueAsString( hNameProp, NULL );

		hNameProp = ::DATA_Find_Prop( hSData, NULL, VE_PID_IFM_METHODNAME );
		if ( hNameProp )
			pSName = ::GetPropValueAsString( hNameProp, NULL );

		bNameTheSame = !lstrcmp( pFName, pSName );

		bResult = ::CompareDataDatas( hFData, hSData );
	}
	return bResult;
}

void CVisualEditorDlg::LoadInterfaceSystemMethodsNodes( 
	HDATA hRootData, CCTItemsArray &newItems, int nFirstItemLevel, BOOL bLinkedToFile ) 
{
	CreateStringNode(newItems, nFirstItemLevel, hRootData, IDS_SYSMETHODS, FALSE);

	++nFirstItemLevel;
	HDATA hSMData;

	BOOL bNewTree = false;
	MAKE_ADDR1(nAddr1, VE_PID_IF_SYSTEMMETHODS);
	if ( !(hSMData = ::DATA_Find(hRootData, nAddr1)) ) 
	{
		hSMData = ::DATA_Add( hRootData, NULL, VE_PID_IF_SYSTEMMETHODS, 0, 0 );
		OnCreateData(hSMData);
		SetModified( IsModified() | !::IsParentLinkedToFile(hRootData) );
		bNewTree = true;
	}

	if ( hSMData && ghSMRootData ) 
	{
		if ( bNewTree ) 
		{
			HDATA hCurrData = ::DATA_Get_First( ghSMRootData, NULL );
			while( hCurrData ) 
			{
				HDATA hNewMethodData = ::DATA_Copy( NULL, NULL, hCurrData, DATA_IF_ROOT_INCLUDE );
				if ( hNewMethodData ) 
				{
					::DATA_Attach( hSMData, NULL, hNewMethodData, DATA_IF_ROOT_INCLUDE );
					LoadOneInterfaceSystemMethodStuff( hNewMethodData, newItems, nFirstItemLevel );
				}
				hCurrData = ::DATA_Get_Next( hCurrData, NULL );
			}
		}
		else 
		{
			// Идти синхронно по двум деревьям
			HDATA hCurrSMData = ::DATA_Get_First( hSMData, NULL );
			HDATA hCurrGSMData = ::DATA_Get_First( ghSMRootData, NULL );
			while( hCurrSMData && hCurrGSMData ) 
			{
				// Взять следующего в локальном дереве
				HDATA hNextSMData = ::DATA_Get_Next( hCurrSMData, NULL );
				// Сравнить текущего локального и глобального
				BOOL bNameTheSame = FALSE;
				if ( !::CompareTwoSMData(hCurrSMData, hCurrGSMData, bNameTheSame) ) 
				{
					// Если не совпадают -
					AVP_dword bSelected = FALSE;
					if ( bNameTheSame ) 
					{
						// Если имена совпадают - скопировать SELECTED
						HPROP hCheckedProp = ::DATA_Find_Prop( hCurrSMData, NULL, VE_PID_IFM_SELECTED );
						if ( hCheckedProp ) 
							bSelected = ::GetPropNumericValueAsDWord( hCheckedProp );
					}

					// Удалить локального, сделать копию с глобального и добавить
					::DATA_Remove( hCurrSMData, NULL );

					HDATA hNewMethodData = ::DATA_Copy( NULL, NULL, hCurrGSMData, DATA_IF_ROOT_INCLUDE );
					if ( hNewMethodData ) 
					{
						HPROP hCheckedProp = ::DATA_Find_Prop( hNewMethodData, NULL, VE_PID_IFM_SELECTED );
						if ( !hCheckedProp ) 
							hCheckedProp = ::DATA_Add_Prop( hNewMethodData, NULL, VE_PID_IFM_SELECTED, 0, 0 );
						if ( hCheckedProp ) 
							::SetPropNumericValue( hCheckedProp, bSelected );

						hCurrSMData = hNextSMData
										? ::DATA_Insert( hNextSMData, NULL, hNewMethodData )
										: ::DATA_Attach( hSMData, NULL, hNewMethodData, DATA_IF_ROOT_INCLUDE );
					}

					SetModified( true );
				}

				// Загрузить Stuff
				LoadOneInterfaceSystemMethodStuff( hCurrSMData, newItems, nFirstItemLevel );

				// Следующая итерация
				hCurrSMData = hNextSMData;
				hCurrGSMData = ::DATA_Get_Next( hCurrGSMData, NULL );
			}

			// Если остались неоприходованные локальные - всех удалить
			while( hCurrSMData ) 
			{
				HDATA hNextSMData = ::DATA_Get_Next( hCurrSMData, NULL );
				::DATA_Remove( hCurrSMData, NULL );
				hCurrSMData = hNextSMData;
				SetModified( true );
			}

			// Если остались неоприходованные глобальные - добавить новые копии
			while( hCurrGSMData ) 
			{
				HDATA hNewMethodData = ::DATA_Copy( NULL, NULL, hCurrGSMData, DATA_IF_ROOT_INCLUDE );
				if ( hNewMethodData ) 
				{
					::DATA_Attach( hSMData, NULL, hNewMethodData, DATA_IF_ROOT_INCLUDE );
					LoadOneInterfaceSystemMethodStuff( hNewMethodData, newItems, nFirstItemLevel );
				}
				hCurrGSMData = ::DATA_Get_Next( hCurrGSMData, NULL );
				SetModified( true );
			}
		}
	}
}

// ---
void CVisualEditorDlg::LoadOneInterfaceSystemMethodStuff( 
	HDATA hMethodData, CCTItemsArray &newItems, int nFirstItemLevel ) 
{
	::SetNodeUID( hMethodData );
	CreatePropertyNode(newItems, nFirstItemLevel, hMethodData, VE_PID_IFM_SELECTED);

	BOOL bEditable = false;

	if(IsPrototype())
		bEditable = true;

	BOOL bSeparator = ::IsSeparator(hMethodData); 
	if ( !(bSeparator) ) 
	{
		LoadInterfaceMethodResultNameStuff( hMethodData, newItems, nFirstItemLevel+1, true, bEditable);
		LoadInterfaceMethodResultDefValueStuff( hMethodData, newItems, nFirstItemLevel+1, true, bEditable );
	}

	LoadShortCommentStuff( hMethodData, newItems, nFirstItemLevel+1, true );
	LoadLargeCommentStuff( hMethodData, newItems, nFirstItemLevel+1, true );

	if ( !bSeparator ) 
	{
		LoadBehaviorCommentStuff( hMethodData, newItems, nFirstItemLevel+1, true );
		LoadValueCommentStuff( hMethodData, newItems, nFirstItemLevel+1, true );
	}

	LoadInterfaceMethodParametersDescriptionNodes( hMethodData, newItems, nFirstItemLevel, true, false );
}

// ------------------------------------------------------------------------------------------
void CVisualEditorDlg::LoadPluginDescriptionTreeNodes( HDATA hRootData, BOOL bSetToWindow ) 
{
	CCTItemsArray& newItems = *new CCTItemsArray( true );
	
	CreatePropertyNode(newItems, 1, hRootData, VE_PID_PL_AUTOSTART);

	BOOL bJustBeenAdded;
	HDATA hData = 
		GetPluginStaticInterfaceRoot(hRootData, bJustBeenAdded);

	// find methods subtree
	MAKE_ADDR1(nAddr, VE_PID_IF_METHODS);
	hData = ::DATA_Find(hData, nAddr);

	if(bJustBeenAdded)
		SetModified(true);
	
	// Externalmethods label in plugin description panel
	CreateStringNode(newItems, 1, hData, IDS_METHODS, FALSE);

	CControlTreeItem *pItem = (CControlTreeItem*)newItems[newItems.GetSize()-1]; 
	pItem->SetExpanded();
	
	HDATA hCurrData = NULL;
	if ( hData ) 
		hCurrData = ::DATA_Get_First( hData, NULL );

	while ( hCurrData ) 
	{
		LoadOneInterfaceMethodStuff( hCurrData, newItems, 2, false);
		hCurrData = ::DATA_Get_Next( hCurrData, NULL );
	}

	CreateStringNode(newItems, 2, NULL, IDS_NEWMETHOD, TRUE, FALSE, sdt_Method);

	if ( bSetToWindow )
		m_IDescriptions.SetItemsArray( &newItems );
}

// ------------------------------------------------------------------------------------------
