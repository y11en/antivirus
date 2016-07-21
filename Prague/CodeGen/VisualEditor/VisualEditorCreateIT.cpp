// -------------------------------------------------------------------------------------------

#include "stdafx.h"
#include <datatreeutils/dtutils.h>
#include <serialize/kldtser.h>
#include <avpprpid.h>
#include <Pr_prop.h>
#include <Stuff\CPointer.h>
#include <StuffIO\Utils.h>
#include <Stuff\PSynArr.h>

#include "VisualEditor.h"
#include "VisualEditorDlg.h"
#include "SelectLoadedInterfacesDlg.h"

// -------------------------------------------------------------------------------------------

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

// -------------------------------------------------------------------------------------------
HDATA DataCreateCommon(HDATA i_hParent, UINT i_uiTypeProp, UINT i_uiNameProp, CHAR* i_strName)
{
	ASSERT(NULL != i_hParent);

	UINT nDataId = MAKE_AVP_PID(::CreateDataId(i_hParent), VE_APP_ID, avpt_nothing, 0);
	HDATA hData = ::DATA_Add( i_hParent, NULL, nDataId, 0, 0 );

	::SetSeparator( hData, i_strName);
	::DATA_Add_Prop( hData, NULL, i_uiNameProp, AVP_dword(LPCSTR(i_strName)), 0 );
	::DATA_Add_Prop( hData, NULL, VE_PID_NODETYPE, AVP_dword(i_uiTypeProp), 0 );

	return hData;
}

HDATA DataCreateErrorCode(HDATA i_hParent, UINT i_uiTypeProp, UINT i_uiNameProp, CHAR* i_strName)
{
	HDATA hData = ::DataCreateCommon( i_hParent, i_uiTypeProp, i_uiNameProp, i_strName );

	HPROP hMaxValueProp = ::DATA_Find_Prop( i_hParent, NULL, VE_PID_IFE_ERRORCODEVALUE );
	if ( hMaxValueProp ) 
	{
		AVP_dword dwMaxValue = ::GetPropNumericValueAsDWord( hMaxValueProp ) + 1;
		SetDwordProp(hData, VE_PID_IFE_ERRORCODEVALUE, dwMaxValue);
		SetDwordProp(i_hParent, VE_PID_IFE_ERRORCODEVALUE, dwMaxValue);
	}

	return hData;
}

HDATA DataCreateProperty(HDATA i_hParent, UINT i_uiTypeProp, UINT i_uiNameProp, CHAR* i_strName)
{
	HDATA hData = ::DataCreateCommon( i_hParent, i_uiTypeProp, i_uiNameProp, i_strName );

	bool bPredefinedProp = !!::PropName2PropID( i_strName );
	::DATA_Add_Prop( hData, NULL, VE_PID_IFP_PREDEFINED, AVP_dword(bPredefinedProp), 0 );

	if(gpMainDlg->IsPrototype())
		::DATA_Add_Prop( hData, NULL, VE_PID_IFP_HARDPREDEFINED, AVP_dword(FALSE), 0 );

	gpMainDlg->PreparePredefinedProp( gSoftProps, i_strName, hData );

	return hData;
}

HDATA DataCreateInterface(HDATA i_hParent, UINT i_uiTypeProp, UINT i_uiNameProp, CHAR* i_strName)
{
	HDATA hData = ::DataCreateCommon( i_hParent, i_uiTypeProp, i_uiNameProp, i_strName );

	if(!::IsSeparator( hData ))
		SetDwordProp(hData , VE_PID_IF_LASTINTERFACESLEVEL, 0);

	return hData;
}

HDATA DataCreateMethod(HDATA i_hParent, UINT i_uiTypeProp, UINT , CHAR* i_strName)
{
	ASSERT(NULL != i_hParent);

	UINT nDataId = MAKE_AVP_PID(::CreateDataId(i_hParent), VE_APP_ID, avpt_nothing, 0);
	HDATA hData = ::DATA_Add( i_hParent, NULL, nDataId, 0, 0 );

	if(gpMainDlg->IsImplementation())
		SetDwordProp(hData, VE_PID_IFM_EXTENDED, TRUE);

	::SetSeparator( hData, i_strName );

	CPubMethodInfo method( hData, false );
	method.SetName( i_strName );
	method.SetDWAttr( VE_PID_NODETYPE, i_uiTypeProp );

	return hData;
}

HDATA DataCreateStructure(HDATA i_hParent, UINT i_uiTypeProp, UINT i_uiNameProp, CHAR* i_strName)
{
	HDATA hData = ::DataCreateCommon( i_hParent, i_uiTypeProp, i_uiNameProp, i_strName );

	HDATA hChildData;

	hChildData = ::DATA_Add( hData, NULL, VE_PID_IF_DATASTRUCTURES, 0, 0 );
	gpMainDlg->OnCreateData(hChildData);  

	hChildData = ::DATA_Add( hData, NULL, VE_PID_IF_DATASTRUCTUREMEMBERS, 0, 0 );
	gpMainDlg->OnCreateData(hChildData);  

	return hData;
}

// -------------------------------------------------------------------------------------------
MD::SDataMetadata CVisualEditorDlg::s_arrDataMetadata[] = 
{
	// prop type		// name prop				// tree data mark	// new node str id	// data constructor
	{VE_NT_TYPE,		VE_PID_IFT_TYPENAME,		sdt_Type,			IDS_NEWTYPE, 		DataCreateCommon},	
	{VE_NT_ERRORCODE,	VE_PID_IFE_ERRORCODENAME,	sdt_ErrorCode,		IDS_NEWERRORCODE, 	DataCreateErrorCode},	
	{VE_NT_CONSTANT,	VE_PID_IFC_CONSTANTNAME,	sdt_Constant,		IDS_NEWCONSTANT, 	DataCreateCommon},	
	{VE_NT_DATASTRUCTUREMEMBER,VE_PID_IFDM_MEMBERNAME,sdt_DataStructureMember,IDS_NEWDATASRUCTUREMEMBER,DataCreateCommon},	
	{VE_NT_DATASTRUCTURE,VE_PID_IFD_DATASTRUCTURENAME,sdt_DataStructure,IDS_NEWDATASRUCTURE,DataCreateStructure},	
	{VE_NT_MESSAGECLASS,VE_PID_IFM_MESSAGECLASSNAME,sdt_MessageClass,	IDS_NEW_MESSAGE_CLASS,DataCreateCommon},	
	{VE_NT_MESSAGE,		VE_PID_IFM_MESSAGENAME,		sdt_Message,		IDS_NEW_MESSAGE,	DataCreateCommon},	
	{VE_NT_PROPERTY,	VE_PID_IFP_PROPNAME,		sdt_Property,		IDS_NEWPROPERTY,	DataCreateProperty},	
	{VE_NT_INTERFACE,	VE_PID_IF_INTERFACENAME,	sdt_Interface,		IDS_NEWINTERFACE,	DataCreateInterface},	
	{VE_NT_METHODPARAM,	VE_PID_IFMP_PARAMETERNAME,	sdt_MethodParam,	IDS_NEWMETHODPARAMETER,	DataCreateCommon},	
	{VE_NT_METHOD,		0,							sdt_Method,			IDS_NEWMETHOD,		DataCreateMethod},	
};

// -------------------------------------------------------------------------------------------
MD::SDataMetadata*	CVisualEditorDlg::FindDataMetadata(UINT i_nTypeId)
{
	int iTblSize = sizeof(s_arrDataMetadata) / sizeof(s_arrDataMetadata[0]);
	for(int i=0; i<iTblSize; i++)
	{
		if(s_arrDataMetadata[i].m_uiPropertyType == i_nTypeId)
			return &s_arrDataMetadata[i];
	}

	ASSERT(FALSE);
	return NULL;
}

// -------------------------------------------------------------------------------------------
HDATA CVisualEditorDlg::CreateDataData(HDATA i_hData, UINT i_uiDataId, CHAR* i_strName)
{
	MD::SDataMetadata* pMD = FindDataMetadata(i_uiDataId);
	HDATA hData = 
		pMD->m_eDataConstructor(i_hData, pMD->m_uiPropertyType, pMD->m_uiNamePropId, i_strName);
	OnCreateData(hData);
	return hData;				 
}

// -------------------------------------------------------------------------------------------
static int GetItemParentIndex(CCTItemsArray& items, int nItemIndex)
{
	if ( nItemIndex >= 0 ) 
	{
		int lastLevel  = items[nItemIndex]->GetLevel();
		for (	nItemIndex--; 
				nItemIndex >= 0 && items[nItemIndex]->GetLevel() >= lastLevel; nItemIndex-- );
		return nItemIndex;
	}
	return -1;
}

void	CVisualEditorDlg::InitCreationParams(CCTItemsArray& items, int nItemInd, 
	HDATA& o_hParent, int& o_nParentIndex, CString& o_strItemText, int& o_nItemLevel)
{
	o_hParent = NULL;
	o_nParentIndex = 0;
	o_strItemText.Empty();
	o_nItemLevel = 0;

	o_nParentIndex = GetItemParentIndex( items, nItemInd );
	if ( o_nParentIndex >= 0 )
		o_hParent = CPPD(items[o_nParentIndex])->m_hData;

	if ( !o_hParent )
	{
		if(&items == m_Interfaces.GetItemsArray())
			o_hParent = m_hTree;
		else
			o_hParent = m_hCCTree;
	}

	CControlTreeItem *item = (CControlTreeItem *)items[nItemInd];
	o_strItemText = item->GetText().GetBuffer( 0 );
	item->GetText().ReleaseBuffer();
	delete item->GetData();

	o_nItemLevel = items[nItemInd]->GetLevel();
}

// -------------------------------------------------------------------------------------------
void CVisualEditorDlg::CreateData(CCTItemsArray& items, int nItemInd, UINT i_nDataType)
{
	HDATA hParentData;
	int nParentIndex;
	CString strItemText;
	int nItemLevel;

	InitCreationParams(items, nItemInd, hParentData, nParentIndex, strItemText, nItemLevel);

	MD::SDataMetadata* pMD = FindDataMetadata(i_nDataType);
	ASSERT(NULL != pMD);

	HDATA hData = pMD->m_eDataConstructor(hParentData, 
		pMD->m_uiPropertyType, pMD->m_uiNamePropId, (LPTSTR)(LPCTSTR)strItemText);

	OnCreateData(hData);

	CVEControlTreeBase* poCtrl = NULL;

	if(&items == m_Interfaces.GetItemsArray())
		poCtrl = &m_Interfaces;
	else
	if(&items == m_IDescriptions.GetItemsArray())
		poCtrl = &m_IDescriptions;
	else
		ASSERT(FALSE);

	CCTItemsArray newItems( false );
	PopulateData(hData, newItems, nItemLevel);
	poCtrl->UpdateItemsArray( nItemInd, newItems, true );

	newItems.RemoveAll();

	MD::ETreeNodeType eNewNodeType = MD::eString;

	if(i_nDataType == VE_NT_PROPERTY)
		eNewNodeType = MD::eStrDDList;

	CreateStringNode(newItems, nItemLevel, NULL, pMD->m_nStrNewNodeId, TRUE, FALSE, pMD->m_eTreeDataMark, eNewNodeType);
	poCtrl->AddItemToParent( newItems[0], nParentIndex );
}

// -------------------------------------------------------------------------------------------
void CVisualEditorDlg::PopulateData(HDATA hData, CCTItemsArray &newItems, int nItemLevel)
{
	UINT nDataType = GetDwordProp(hData, VE_PID_NODETYPE);

	switch(nDataType)
	{
	case VE_NT_TYPE:
		LoadOneInterfaceTypeStuff(hData, newItems, nItemLevel, false);
	break;

	case VE_NT_CONSTANT:
		LoadOneInterfaceConstantStuff(hData, newItems, nItemLevel, false);
	break;

	case VE_NT_MESSAGECLASS:
		LoadOneInterfaceMessagesClassStuff(hData, newItems, nItemLevel, false);
	break;
	
	case VE_NT_MESSAGE:
		LoadOneInterfaceMessagesClassMessagesStuff(hData, newItems, nItemLevel, TRUE, FALSE);
	break;

	case VE_NT_ERRORCODE:
		LoadOneInterfaceErrorCodeStuff(hData, newItems, nItemLevel, false);
	break;

	case VE_NT_PROPERTY:
		LoadOneInterfacePropertyStuff(hData, newItems, nItemLevel, false);
	break;

	case VE_NT_INTERFACE:
		LoadOneInterfaceStuff(hData, newItems, nItemLevel);
	break;
	
	case VE_NT_METHODPARAM:
		LoadOneInterfaceMethodParameterStuff(hData, newItems, nItemLevel, false, true);
	break;
	   
	case VE_NT_METHOD:
		LoadOneInterfaceMethodStuff(hData, newItems, nItemLevel, false);
	break;

	case VE_NT_DATASTRUCTUREMEMBER:
		LoadOneInterfaceDataStructureMemberStuff(hData, newItems, nItemLevel);
	break;
	
	case VE_NT_DATASTRUCTURE:
		LoadOneInterfaceDataStructureStuff(hData, newItems, nItemLevel);
	break;

	default:
		ASSERT(FALSE);
	}

	// update all labels - they could change since new it's children was added
	int nCount = newItems.GetSize();
	for(int i=0; i<nCount; i++)
	{
		CControlTreeItem *pItem = (CControlTreeItem*) newItems[i];

		if(NULL == pItem)
			continue;

		if(NULL == CPPD(pItem))
			continue;

		if(NULL == CPPD(pItem)->m_hProp || NULL == CPPD(pItem)->m_hData)
			continue;

		HDATA hData = CPPD(pItem)->m_hData;
		HPROP hProp = CPPD(pItem)->m_hProp;

		CString strStr;
		if(GetPropDisplayString(strStr, hData, hProp, 0, TRUE))
			pItem->SetText(strStr);
	}
}

// -------------------------------------------------------------------------------------------
void CVisualEditorDlg::OnCreateData(HDATA i_hData)
{
	if(IsPrototype())
		SetDwordProp(i_hData, VE_PID_NATIVE, TRUE);

	::SetNodeUID( i_hData );
}

// -------------------------------------------------------------------------------------------
void CVisualEditorDlg::CreateInterfaceTypeNodeContents( CCTItemsArray &items, int nItemInd ) 
{
	CreateData(items, nItemInd, VE_NT_TYPE);
}

// ---
void CVisualEditorDlg::CreateInterfaceConstantNodeContents( CCTItemsArray &items, int nItemInd ) 
{
	CreateData(items, nItemInd, VE_NT_CONSTANT);
}

// ---
void CVisualEditorDlg::CreateInterfaceMessageClassNodeContents( CCTItemsArray &items, int nItemInd ) 
{
	CreateData(items, nItemInd, VE_NT_MESSAGECLASS);
}

// ---
void CVisualEditorDlg::CreateInterfaceMessageNodeContents( CCTItemsArray &items, int nItemInd ) 
{		
	CreateData(items, nItemInd, VE_NT_MESSAGE);
}

// -------------------------------------------------------------------------------------------
void CVisualEditorDlg::CreateInterfaceErrorCodeNodeContents( CCTItemsArray &items, int nItemInd ) 
{
	CreateData(items, nItemInd, VE_NT_ERRORCODE);
}

// -------------------------------------------------------------------------------------------
void CVisualEditorDlg::CreateInterfacePropertyNodeContents( CCTItemsArray &items, int nItemInd ) 
{
	CreateData(items, nItemInd, VE_NT_PROPERTY);
}

// -------------------------------------------------------------------------------------------
void CVisualEditorDlg::CreateInterfaceNodeContents( CCTItemsArray &items, int nItemInd ) 
{
	// interface description will be populated as a result of
	// the Selection Changing notification processing 
	// which is sent within UpdateItemsArray call
	CreateData(items, nItemInd, VE_NT_INTERFACE);
}

// -------------------------------------------------------------------------------------------
void CVisualEditorDlg::CreateInterfaceMethodParamNodeContents( CCTItemsArray &items, int nItemInd ) 
{
	CreateData(items, nItemInd, VE_NT_METHODPARAM);
}

// -------------------------------------------------------------------------------------------
void CVisualEditorDlg::CreateInterfaceMethodNodeContents( CCTItemsArray &items, int nItemInd ) 
{
	CreateData(items, nItemInd, VE_NT_METHOD);
}

// -------------------------------------------------------------------------------------------
void CVisualEditorDlg::CreateInterfaceDataStructureMemberNodeContents( CCTItemsArray &items, int nItemInd ) 
{
	CreateData(items, nItemInd, VE_NT_DATASTRUCTUREMEMBER);
}

// ------------------------------------------------------------------------------------------
void CVisualEditorDlg::CreateInterfaceDataStructureNodeContents( CCTItemsArray &items, int nItemInd ) 
{
	CreateData(items, nItemInd, VE_NT_DATASTRUCTURE);
}

// -------------------------------------------------------------------------------------------
void CVisualEditorDlg::CreateNewDescriptionTreeStuff() 
{
	int nFirstItemLevel = 1;

	CCTItemsArray &newItems = *new CCTItemsArray( true );

	LoadInterfaceTypesNodes(m_hCCTree, newItems, nFirstItemLevel, false );
	LoadInterfaceConstantsNodes(m_hCCTree, newItems, nFirstItemLevel, false );
	LoadInterfaceMessagesNodes(m_hCCTree, newItems, nFirstItemLevel, false );
	LoadInterfaceErrorCodeNodes(m_hCCTree, newItems, nFirstItemLevel, false);
	LoadInterfacePropertiesNodes(m_hCCTree, newItems, nFirstItemLevel, false);
	LoadInterfaceMethodsNodes(m_hCCTree, newItems, nFirstItemLevel, false);

	m_IDescriptions.SetItemsArray( &newItems );
}

// -------------------------------------------------------------------------------------------
void CVisualEditorDlg::CreateNewInterfaceTreeStuff() 
{
	if(IsPrototype())	
	{
		FlushDataTree( NULL );
		
		m_FileName.Empty();
		UpdateData( FALSE );
		SetWindowTitle();
		
		m_hTree = ::DATA_Add( 0, NULL, VE_PID_ROOT, m_nWorkMode, 0 );
		::SetNodeUID( m_hTree );
		
		CCTItemsArray &items = *new CCTItemsArray( true );
		CreateStringNode(items, 1, NULL, IDS_NEWINTERFACE, TRUE, FALSE, sdt_Interface);
		m_Interfaces.SetItemsArray( &items );
	}
	else
	{
		HDATA hResult = NULL;
		if ( EnumAndSelectInterfaces(&hResult) ) 
		{
			::DATA_Set_Val( hResult, NULL, 0, m_nWorkMode, 0 );
			
			CWaitCursor wait;
			
			CheckLinkedInterfaces( hResult, TRUE );
			LoadPluginDescriptionTreeNodes( hResult, false );
			
			m_FileName.Empty();
			UpdateData( FALSE );
			SetWindowTitle();
			
			InitNewTree( hResult );
			SetModified( true );
		}
	}
}

// ------------------------------------------------------------------------------------------


template <class Type>
class  CPSyncArrayEx : public CPSyncArray<Type> {
	friend void destroy_array( CPSyncArray<Type>& arr );
};


void destroy_array( CPSyncArray<CCheckTreeItem>& arr ) {
  uint i = 0;
	CPSyncArrayEx<CCheckTreeItem>& arr2 = (CPSyncArrayEx<CCheckTreeItem>&)arr;
  for( CPSyncArray<CCheckTreeItem>::TPtr* parr=arr2.parr; i<arr2.count; i++,parr++ ) {
    delete *parr;
    *parr = 0;
  }  
  arr2.count = 0;
}







