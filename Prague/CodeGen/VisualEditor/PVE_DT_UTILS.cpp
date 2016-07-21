//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//

#include "stdafx.h"
#include "visualeditordlg.h"
#include "PVE_DT_UTILS.h"
#include <DataTreeUtils/DTUtils.h>
#include "../AVPPveID.h"

//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//
// find interface with no interfacename property and with pluginstaticinterface property
// if not found - create one
HDATA GetPluginStaticInterfaceRoot(HDATA i_hDataPlugin, BOOL& o_bBeenAdded)
{
	ASSERT(i_hDataPlugin);

	HDATA hData = ::DATA_Get_First( i_hDataPlugin, NULL );
	
	while(hData)
	{
		if(IsPluginStaticInterfaceRoot(hData))
			break;
		hData = ::DATA_Get_Next( hData, NULL );
	}

	o_bBeenAdded = !hData;

	// create if not found
	if(!hData)
	{
		hData = ::DATA_Add( i_hDataPlugin, NULL, VE_PID_INTERFACE(::CreateDataId(i_hDataPlugin)), 0, 0 );

		::DATA_Add_Prop( hData, NULL, VE_PID_NODETYPE, AVP_dword(VE_NT_INTERFACE), 0 );
		::DATA_Add_Prop( hData, 0, VE_PID_IF_PLUGIN_STATIC, 0, 0);
		::DATA_Add_Prop( hData, 0, VE_PID_IF_LASTINTERFACESLEVEL, 0, 0);
		

		::AddHeaderGuidProp( hData );
		::SetNodeUID( hData );

		// add empty methods node - without it current node will be treated as alias 
		// and thus removed on loading

		::DATA_Add( hData, NULL, VE_PID_IF_METHODS, 0, 0 );
	}

	// fix old data
	if(NULL == ::DATA_Find_Prop( hData, NULL, VE_PID_IF_LASTINTERFACESLEVEL ))
	{
		o_bBeenAdded = TRUE;
		::DATA_Add_Prop( hData, 0, VE_PID_IF_LASTINTERFACESLEVEL, 0, 0);
	}
	
	ASSERT(hData);

	return hData;
}

//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//
// this is true if node has not interfacename property and has pluginstatic property
BOOL	IsPluginStaticInterfaceRoot(HDATA i_hDataInterface)
{
	if(!i_hDataInterface)
		return FALSE;

	HPROP hPropInerfaceName = ::DATA_Find_Prop(i_hDataInterface, NULL, VE_PID_IF_INTERFACENAME);
	HPROP hPropPluginStatic = ::DATA_Find_Prop(i_hDataInterface, NULL, VE_PID_IF_PLUGIN_STATIC);
	
	return !hPropInerfaceName && hPropPluginStatic;
}


static const UINT arrNatives[] = 
{
	VE_PID_IFP_NATIVE,
	VE_PID_IFT_NATIVE,
	VE_PID_IFC_NATIVE,
	VE_PID_IFE_NATIVE,
	VE_PID_IFMC_NATIVE,
	VE_PID_IFM_NATIVE,
	VE_PID_NATIVE,
};

//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//
BOOL	IsNativeNode(HDATA i_hData)
{
	if(NULL == i_hData)
		return FALSE;

	if (GetDwordProp( i_hData, VE_PID_NODETYPE ))
		return TRUE;

	return FALSE;
}

//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//
BOOL	IsNativeData(HDATA i_hData)
{
	if(NULL == i_hData)
		return FALSE;

	int nArrSize = sizeof(arrNatives) / sizeof(arrNatives[0]);
	for(int i=0; i<nArrSize; i++ )
	{
		HPROP hNative = ::DATA_Find_Prop( i_hData, NULL, arrNatives[i] );
		if(hNative && ::GetPropNumericValueAsDWord(hNative))
		{
			i = GetDwordProp( i_hData, VE_PID_IFP_NATIVE );
			i = GetDwordProp( i_hData, VE_PID_IFT_NATIVE );
			i = GetDwordProp( i_hData, VE_PID_IFC_NATIVE );
			i = GetDwordProp( i_hData, VE_PID_IFE_NATIVE );
			i = GetDwordProp( i_hData, VE_PID_IFMC_NATIVE );
			i = GetDwordProp( i_hData, VE_PID_IFM_NATIVE );
			i = GetDwordProp( i_hData, VE_PID_NATIVE );
			i = GetDwordProp( i_hData, VE_PID_NODETYPE );
			return TRUE;
		}
	}
	return FALSE;
}

//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//
void CheckNativeProp(HDATA i_hData, void* i_pData)
{
	if(IsNativeData(i_hData))
		return;
	::DATA_Add_Prop(i_hData, NULL, VE_PID_NATIVE, TRUE, 0 );
	if(i_pData)
		*((BOOL*)i_pData) = TRUE;
}

//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//
BOOL	IsBoolPropSet(HDATA i_hData, AVP_dword i_dwPropId)
{
	if(NULL == i_hData)
		return FALSE;

	HPROP hProp = ::DATA_Find_Prop( i_hData, NULL, i_dwPropId );

	if(NULL == hProp)
		return FALSE;

	return !!::GetPropNumericValueAsDWord( hProp );
}

//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//
BOOL	CheckPropSet(HDATA i_hData, AVP_dword i_dwPropId, AVP_dword i_dwDefVal)
{
	if(NULL == i_hData)
		return FALSE;

	BOOL bRet = FALSE;

	if(::DATA_Find_Prop( i_hData, NULL, i_dwPropId ))
		return FALSE;

	::DATA_Add_Prop( i_hData, NULL, i_dwPropId, i_dwDefVal, 0 );
	return TRUE;
}

//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//
BOOL SetDwordProp(HDATA i_hData, AVP_dword i_dwPropId, AVP_dword i_dwDefVal)
{
	BOOL bRetVal = FALSE;
	if(NULL == i_hData)
		return bRetVal;
	HPROP hProp = ::DATA_Find_Prop( i_hData, NULL, i_dwPropId );
	if ( hProp ) 
	{
		AVP_dword dwOldVal;
		::PROP_Get_Val( hProp, &dwOldVal, sizeof(dwOldVal) );
		if(dwOldVal != i_dwDefVal)
		{
			bRetVal = TRUE;
			::PROP_Set_Val( hProp, i_dwDefVal, 0 );
		}
	}
	else
	{
		bRetVal = TRUE;
		::DATA_Add_Prop( i_hData, NULL, i_dwPropId, AVP_dword(i_dwDefVal), 0 );
	}
	return bRetVal;
}

//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//
AVP_dword GetDwordProp(HDATA i_hData, AVP_dword i_dwPropId)
{
	AVP_dword dwRetVal = 0;
	HPROP hProp = ::DATA_Find_Prop( i_hData, NULL, i_dwPropId );
	if ( hProp ) 
		::PROP_Get_Val( hProp, &dwRetVal, sizeof(dwRetVal) );

	return dwRetVal;
}

// ---
static AVP_dword g_dwCondemnedTreesTable[] = {
	VE_PID_IF_DATASTRUCTURES,
	VE_PID_IF_DATASTRUCTUREMEMBERS,
	VE_PID_IF_TYPES,
	VE_PID_IF_CONSTANTS,
	VE_PID_IF_ERRORCODES,
};


static int g_dwCondemnedTreesTableCount = sizeof(g_dwCondemnedTreesTable) / sizeof(g_dwCondemnedTreesTable[0]);

// ---
static AVP_dword g_dwEnabledPropsTable[] = {
	VE_PID_PL_PLUGINNAME_ID,
	VE_PID_PL_DIGITALID_ID,
	VE_PID_PL_VERSIONID_ID,
	VE_PID_PL_MNEMONICID_ID,
	VE_PID_PL_VENDORID_ID,
	VE_PID_PL_VENDORMNEMONICID_ID,
	VE_PID_PL_VENDORNAME_ID,
	VE_PID_PL_CODEPAGEID_ID,
	VE_PID_PL_DATEFORMATID_ID,
	VE_PID_PL_AUTHORNAME_ID,
	VE_PID_PL_PROJECTNAME_ID,
	VE_PID_PL_SUBPROJECTNAME_ID,
	VE_PID_IF_MNEMONICID_ID,
	VE_PID_IF_DIGITALID_ID,
	VE_PID_IF_REVISIONID_ID,
	VE_PID_IF_CREATIONDATE_ID,
	VE_PID_IF_OBJECTTYPE_ID,
	VE_PID_IF_SUBTYPEID_ID,
	VE_PID_IF_AUTORNAME_ID,
	VE_PID_IF_STATIC_ID,
	VE_PID_IF_SYSTEM_ID,
	VE_PID_IF_TRANSFERPROPUP_ID,
	VE_PID_IF_NONSWAPABLE_ID,
	VE_PID_IF_PROTECTED_BY_ID,
	VE_PID_IF_VENDORID_ID,
	VE_PID_IFP_DIGITALID_ID,
	VE_PID_IFP_TYPE_ID,
	VE_PID_IFP_NATIVE_ID,
	VE_PID_IFP_SCOPE_ID,
	VE_PID_IFPSS_VARTYPIFIEDDEFVALUE_ID,
	VE_PID_IFM_RESULTTYPE_ID,
	VE_PID_IFM_RESULTDEFVALUE_ID,
	VE_PID_IFMP_TYPE_ID,
	VE_PID_IFMP_ISPOINTER_ID,
	VE_PID_IFMP_ISCONST_ID,
	VE_PID_IFM_MESSAGECLASSID_ID,
	VE_PID_IFM_MESSAGEID_ID,
};

static int g_dwEnabledPropsTableCount = sizeof(g_dwEnabledPropsTable) / sizeof(g_dwEnabledPropsTable[0]);

// ---
static AVP_dword g_dwCommentsPropsTable[] = 
{
	VE_PID_SHORTCOMMENT_ID,
	VE_PID_LARGECOMMENT_ID,
	VE_PID_BEHAVIORCOMMENT_ID,
	VE_PID_VALUECOMMENT_ID,
};

static int g_dwCommentsPropsTableCount = sizeof(g_dwCommentsPropsTable) / sizeof(g_dwCommentsPropsTable[0]);

// ---
struct EnabledNodeDsc {
	AVP_dword m_dwPID;
	AVP_dword m_dwValue;
};


static EnabledNodeDsc g_EnabledNodeTable[] = {
	{ VE_PID_IFP_SCOPE		, VE_IFP_SHARED },
	{ VE_PID_IFM_SELECTED	, TRUE					}
};

static int g_EnabledNodeTableCount = sizeof(g_EnabledNodeTable)	/ sizeof(g_EnabledNodeTable[0]);


// ---
static bool IsExistsInTable( AVP_dword *pdwTable, int nCount, AVP_dword dwID ) {
	for ( int i=0; i<nCount; i++ ) {
		if ( pdwTable[i] == dwID ) 
			return true;
	}
	return false;
}


// ---
static bool IsNodeEnabled( HDATA hData ) {
	for ( int i=0; i<g_EnabledNodeTableCount; i++ ) {
		HPROP hProp = ::DATA_Find_Prop( hData, NULL, g_EnabledNodeTable[i].m_dwPID );
		if ( hProp ) {
			AVP_dword dwValue = ::DTUT_GetPropNumericValueAsDWord( hProp );
			if ( dwValue == g_EnabledNodeTable[i].m_dwValue )
				return true;
			return false;
		}
	}
	return true;
}

// ---
static BOOL IsSeparator( HDATA hData ) {
	return hData ? !!::DATA_Find_Prop( hData, NULL, VE_PID_SEPARATOR ) : FALSE;
}

// ---
static BOOL IsLinkedToFile( HDATA hData ) {
	return !!::DATA_Find_Prop( hData, NULL, VE_PID_IF_IMPORTFILENAME ) ||
		     !!::DATA_Find_Prop( hData, NULL, VE_PID_IF_LINKEDBYINTERFACE );
}

// ---
static void ScanTreeNodeForEnabled( HDATA hData, bool bEnableComments = false ) {
	HPROP hCurrProp = ::DATA_Get_First_Prop( hData, NULL );
	while ( hCurrProp ) {
		HPROP hNextProp = ::PROP_Get_Next( hCurrProp );
		
		AVP_dword dwID = GET_AVP_PID_ID(::PROP_Get_Id(hCurrProp));
		if ( ::DATA_Find_Prop(hData, NULL, NULL) != hCurrProp && 
			  !::IsExistsInTable(g_dwEnabledPropsTable, g_dwEnabledPropsTableCount, dwID) ) {

			if ( !bEnableComments || 
				   !::IsExistsInTable(g_dwCommentsPropsTable, g_dwCommentsPropsTableCount, dwID) )
				::DATA_Remove_Prop_H( hData, NULL, hCurrProp );
		}
		hCurrProp = hNextProp;
	}
}

// ---
static void ScanSubTreeForEnabled( HDATA hData ) {
	// Сканировать описание интерфейса

	// Сканировать PROPs узла (запретить комментарии)
	::ScanTreeNodeForEnabled( hData );

	HDATA hCurrData = ::DATA_Get_First( hData, NULL );
	while ( hCurrData ) {
		HDATA hNextData = ::DATA_Get_Next( hCurrData, NULL );

		if ( ::IsSeparator(hCurrData) || !::IsNodeEnabled(hCurrData) )
			::DATA_Remove( hCurrData, NULL );
		else 
			::ScanSubTreeForEnabled( hCurrData );

		hCurrData = hNextData;
	}
}


// ---
static void ScanInterfaceTreeForEnabled( HDATA hData ) {
	// Сканировать интерфейс

	// Сканировать PROPs узла (разрешить комментарии)
	::ScanTreeNodeForEnabled( hData, true );

	// Сканировать поддеревья описания интерфейса
	HDATA hCurrData = ::DATA_Get_First( hData, NULL );
	while ( hCurrData ) {
		HDATA hNextData = ::DATA_Get_Next( hCurrData, NULL );

		AVP_dword dwID = ::DATA_Get_Id( hCurrData, NULL );
		if ( ::IsExistsInTable(g_dwCondemnedTreesTable, g_dwCondemnedTreesTableCount, dwID) ) 
			// Если данное поддерево не разрешено - удалить
			::DATA_Remove( hCurrData, NULL );
		else 
			::ScanSubTreeForEnabled( hCurrData );

		hCurrData = hNextData;
	}
}


// ---
HDATA PrepareDBTree( HDATA hRootData ) {
	// Сканировать поддерево Plugin
	HDATA hResultData = ::DATA_Copy( NULL, NULL, hRootData, DATA_IF_ROOT_INCLUDE );
	if ( hResultData ) {
		// Сканировать PROPs узла (разрешить комментарии)
		::ScanTreeNodeForEnabled( hResultData, true );

		// Сканировать интерфейсы Plugin
		HDATA hCurrData = ::DATA_Get_First( hResultData, NULL );
		while ( hCurrData ) {
			HDATA hNextData = ::DATA_Get_Next( hCurrData, NULL );
			 
			if ( ::IsLinkedToFile(hCurrData) ) 
				// Если интерфейс импортирован - удалить
				::DATA_Remove( hCurrData, NULL );
			else 
				::ScanInterfaceTreeForEnabled( hCurrData );

			hCurrData = hNextData;
		}
	}
	return hResultData;
}

//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//
BOOL	IsCommentPropId(AVP_dword i_dwPropId)
{
	return IsExistsInTable(g_dwCommentsPropsTable, g_dwCommentsPropsTableCount, i_dwPropId);
}

//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//
HDATA FindDataByUID( HDATA hRootData, CNodeUID &pFindUID) 
{
	HDATA hCurrData = ::DATA_Get_First( hRootData, NULL );
	while ( hCurrData ) 
	{
		CNodeUID rcGuid;
		if ( ::GetNodeUID(hCurrData, rcGuid) && 
			 ::CompareNodesUID(rcGuid, pFindUID)) 
				return hCurrData;

		hCurrData = ::DATA_Get_Next( hCurrData, NULL );
	}
	return NULL;
}

//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//
HDATA FindEntityByUID( HDATA hRootData, CNodeUID &pFindUID, BOOL bShouldBeNative ) 
{
	HDATA hData = FindDataByUID(hRootData, pFindUID);
	
	if(!hData)
		return NULL;

	if(IsNativeData(hData) != bShouldBeNative)
		hData = NULL;

	return hData;
}


//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//
HPROP FindPropByID( HDATA hData, AVP_word dwID ) 
{
	HPROP hFoundProp = NULL;
	HPROP hProp = ::DATA_Get_First_Prop ( hData, NULL );
	while ( hProp ) 
	{
		if ( GET_AVP_PID_ID(::PROP_Get_Id(hProp)) == dwID ) 
		{
			hFoundProp = hProp;
			break;
		}
		hProp = ::PROP_Get_Next( hProp );
	}
	return hFoundProp;
}

//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//
void ForEach(HDATA i_hRoot, pfnForEachRoutine i_pfnForEachRoutine, void* pRotineData)
{
	ASSERT(NULL != i_hRoot);
	ASSERT(NULL != i_pfnForEachRoutine);

	if(i_hRoot)
		i_pfnForEachRoutine(i_hRoot, pRotineData);

	HDATA hData = NULL;
	if(i_hRoot)
		hData =	::DATA_Get_First( i_hRoot, NULL );
	while ( hData  ) 
	{
		i_pfnForEachRoutine(hData, pRotineData);
		// recusive call
		ForEach(hData, i_pfnForEachRoutine, pRotineData);
		hData  = ::DATA_Get_Next( hData, NULL );
	}
}

//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//
void ForEachSync(HDATA i_hRoot1, HDATA i_hRoot2, 
	pfnForEachSyncRoutine i_pfnForEachSyncRoutine, void* pSyncRotineData)
{
	ASSERT(NULL != i_hRoot1);
	ASSERT(NULL != i_hRoot2);
	ASSERT(NULL != i_pfnForEachSyncRoutine);

	if(i_hRoot1 && i_hRoot2)
		i_pfnForEachSyncRoutine(i_hRoot1, i_hRoot2, pSyncRotineData);

	HDATA hData1 = NULL, hData2 = NULL;

	if(i_hRoot1)
		hData1 = ::DATA_Get_First( i_hRoot1, NULL );

	while ( hData1  ) 
	{
		CNodeUID rcGuid;
		if ( ::GetNodeUID(hData1, rcGuid) ) 
		{
			HDATA hData2 = FindDataByUID(i_hRoot2, rcGuid);

			i_pfnForEachSyncRoutine(hData1, hData2, pSyncRotineData);

			// recusive call
			if(hData2)
				ForEachSync(hData1, hData2, i_pfnForEachSyncRoutine, pSyncRotineData);
		}

		hData1  = ::DATA_Get_Next( hData1, NULL );
	}
}

//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//
void DataAttachWithNextDataId(HDATA i_hParent, HDATA i_hAttached, AVP_dword i_uiFlags)
{
	if(NULL == i_hParent || NULL == i_hAttached)
		return;

	AVP_dword uiDataMinId = ::CreateDataId(i_hParent);
	DATA_Replace_ID(i_hAttached, NULL, AVP_word(uiDataMinId));

	::DATA_Attach( i_hParent, NULL, i_hAttached, i_uiFlags );
}

//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//
