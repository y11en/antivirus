////////////////////////////////////////////////////////////////////
//
//  PrepareDBTree.cpp
//  Prepare given tree for generating description DB
//  Prague visual editor
//  Victor Matiouchenkov [victor@avp.ru], Kaspersky Labs. 2000
//  Copyright (c) Kaspersky Labs
//
////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include <datatreeutils/dtutils.h>
#include "../AVPPveID.h"

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
	//VE_PID_UNIQUEID_ID,
	//VE_PID_PL_PLUGINNAME_ID,
	VE_PID_PL_DIGITALID_ID,
	VE_PID_PL_VERSIONID_ID,
	//VE_PID_PL_MNEMONICID_ID,
	VE_PID_PL_VENDORID_ID,
	//VE_PID_PL_VENDORMNEMONICID_ID,
	//VE_PID_PL_VENDORNAME_ID,
	VE_PID_PL_CODEPAGEID_ID,
	VE_PID_PL_DATEFORMATID_ID,
	//VE_PID_PL_AUTHORNAME_ID,
	//VE_PID_PL_PROJECTNAME_ID,
	//VE_PID_PL_SUBPROJECTNAME_ID,
	VE_PID_PL_AUTOSTART_ID,
	//VE_PID_IF_MNEMONICID_ID,
	VE_PID_IF_DIGITALID_ID,
	VE_PID_IF_REVISIONID_ID,
	VE_PID_IF_CREATIONDATE_ID,
	//VE_PID_IF_OBJECTTYPE_ID,
	VE_PID_IF_SUBTYPEID_ID,
	//VE_PID_IF_AUTORNAME_ID,
	VE_PID_IF_STATIC_ID,
  VE_PID_IF_SYSTEM_ID,
  //VE_PID_IF_TRANSFERPROPUP_ID,
	VE_PID_IF_NONSWAPABLE_ID,
  VE_PID_IF_PROTECTED_BY_ID,
	VE_PID_IF_VENDORID_ID,
	//VE_PID_IF_SUBTYPESTRID_ID,
	VE_PID_IFP_DIGITALID_ID,
	VE_PID_IFP_TYPE_ID,
	VE_PID_IFP_NATIVE_ID,
	VE_PID_IFP_SCOPE_ID,
	VE_PID_IFPSS_VARTYPIFIEDDEFVALUE_ID,
	VE_PID_IFM_RESULTTYPE_ID,
	//VE_PID_IFM_RESULTDEFVALUE_ID,
	VE_PID_IFMP_TYPE_ID,
	VE_PID_IFMP_ISPOINTER_ID,
	VE_PID_IFMP_ISCONST_ID,
  VE_PID_IFM_MESSAGECLASSID_ID,
  VE_PID_IFM_MESSAGEID_ID,
  VE_PID_IFM_METHOD_ID_ID,
  VE_PID_NODETYPETYPE_ID,
};

static int g_dwEnabledPropsTableCount = sizeof(g_dwEnabledPropsTable) / sizeof(g_dwEnabledPropsTable[0]);

// ---
static AVP_dword g_dwCommentsPropsTable[] = {
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
	::ScanTreeNodeForEnabled( hData, false /*true*/ );

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
		::DATA_Set_Val( hResultData, NULL, 0, 2, 0 );

		// Сканировать PROPs узла (разрешить комментарии)
		::ScanTreeNodeForEnabled( hResultData, false /*true*/ );

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
