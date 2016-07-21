//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//
#include "stdafx.h"

#include "SynchroTools.h"

#include <datatreeutils/dtutils.h>
#include "VisualEditor.h"
#include "VisualEditorDlg.h"


//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//
inline HDATA FindDataWithGuid( HDATA i_hWhereToFind, HDATA i_hWhatToFind, bool i_bNative = TRUE ) 
{
	if(	NULL == i_hWhereToFind ||
		NULL == i_hWhatToFind)
		return NULL;

	CNodeUID rcGuid;
	if ( ::GetNodeUID(i_hWhatToFind, rcGuid) ) 
		return ::FindEntityByUID( i_hWhereToFind, rcGuid, i_bNative );
	
	return NULL;
}

//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//
static void MakePropFullName( HDATA hPropData, CString &rcName, AVP_dword dwNamePID ) {
	HDATA hIntData = ::DATA_Get_Dad( ::DATA_Get_Dad(hPropData, NULL), NULL );
	if ( hIntData ) {
		HPROP hProp = ::DATA_Find_Prop( hIntData, NULL, VE_PID_IF_INTERFACENAME );
		if ( hProp ) {
			CAPointer<char> pValue = ::GetPropValueAsString( hProp, NULL );
			rcName += pValue;
			rcName += _TEXT(".");
		}
	}
	HPROP hProp = ::DATA_Find_Prop( hPropData, NULL, dwNamePID );
	if ( hProp ) {
		CAPointer<char> pValue = ::GetPropValueAsString( hProp, NULL );
		rcName += pValue;
	}
}

//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//
void MergeCheckPropType( HDATA hDefData, HDATA hImpData ) {
	HPROP hDefProp = ::DATA_Find_Prop( hDefData, NULL, VE_PID_IFP_TYPE );
	if ( hDefProp ) {
		AVP_dword dwDefType = ::GetPropNumericValueAsDWord( hDefProp );
		HPROP hImpProp = ::DATA_Find_Prop( hImpData, NULL, VE_PID_IFP_TYPE );
		if ( hImpProp ) {
			AVP_dword dwImpType = ::GetPropNumericValueAsDWord( hImpProp );
			if ( dwDefType != dwImpType ) {
				HPROP hValueProp;
				if ( hValueProp = ::DATA_Find_Prop(hImpData, NULL, VE_PID_IFPSS_VARIABLEDEFVALUE) ) {
					CString rcName;
					::MakePropFullName( hDefData, rcName, VE_PID_IFP_PROPNAME );

					CString rcText;
					rcText.Format( IDS_ERR_MERGEDISCARDDEFVALUE, LPCTSTR(rcName) );
					::DlgMessageBox( AfxGetMainWnd(), rcText, MB_OK );

					::SetPropValue( hValueProp, "", NULL );
					( (CVisualEditorDlg*) AfxGetMainWnd())->
						SetPropTypifiedDefValue( hImpData );
				}
			}
		}
	}
}

//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//
void MergeCheckType( HDATA hDefData, HDATA hImpData ) {
	bool bDifferent = false;
	HPROP hDefProp = ::DATA_Find_Prop( hDefData, NULL, VE_PID_IFT_BASETYPENAME );
	if ( hDefProp ) {
		CAPointer<char> pDefValue = ::GetPropValueAsString( hDefProp, NULL );
		HPROP hImpProp = ::DATA_Find_Prop( hImpData, NULL, VE_PID_IFT_BASETYPENAME );
		if ( hImpProp ) {
			CAPointer<char> pImpValue = ::GetPropValueAsString( hImpProp, NULL );
			bDifferent |= !!lstrcmp( pDefValue, pImpValue );
		}
	}
	hDefProp = ::DATA_Find_Prop( hDefData, NULL, VE_PID_IFT_BASETYPE );
	if ( hDefProp ) {
		AVP_dword dwDefType = ::GetPropNumericValueAsDWord( hDefProp );
		HPROP hImpProp = ::DATA_Find_Prop( hImpData, NULL, VE_PID_IFT_BASETYPE );
		if ( hImpProp ) {
			AVP_dword dwImpType = ::GetPropNumericValueAsDWord( hImpProp );
			bDifferent = (dwDefType != dwImpType);
		}
	}
	
	if ( bDifferent ) {
		CString rcName;
		::MakePropFullName( hDefData, rcName, VE_PID_IFC_CONSTANTNAME );
		CString rcText;
		rcText.Format( IDS_ERR_MERGEDISCARDTYPE, LPCTSTR(rcName) );
		::DlgMessageBox( AfxGetMainWnd(), rcText, MB_OK );
	}
}

//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//
void MergeCheckConstant( HDATA hDefData, HDATA hImpData ) {
	bool bDifferent = false;
	HPROP hDefProp = ::DATA_Find_Prop( hDefData, NULL, VE_PID_IFC_BASETYPE );
	if ( hDefProp ) {
		AVP_dword dwDefType = ::GetPropNumericValueAsDWord( hDefProp );
		HPROP hImpProp = ::DATA_Find_Prop( hImpData, NULL, VE_PID_IFC_BASETYPE );
		if ( hImpProp ) {
			AVP_dword dwImpType = ::GetPropNumericValueAsDWord( hImpProp );
			bDifferent = (dwDefType != dwImpType);
		}
	}
	hDefProp = ::DATA_Find_Prop( hDefData, NULL, VE_PID_IFC_VALUE );
	if ( hDefProp ) {
		CAPointer<char> pDefValue = ::GetPropValueAsString( hDefProp, NULL );
		HPROP hImpProp = ::DATA_Find_Prop( hImpData, NULL, VE_PID_IFC_VALUE );
		if ( hImpProp ) {
			CAPointer<char> pImpValue = ::GetPropValueAsString( hImpProp, NULL );
			bDifferent |= !!lstrcmp( pDefValue, pImpValue );
		}
	}
	
	if ( bDifferent ) {
		CString rcName;
		::MakePropFullName( hDefData, rcName, VE_PID_IFC_CONSTANTNAME );
		CString rcText;
		rcText.Format( IDS_ERR_MERGEDISCARDCONSTANT, LPCTSTR(rcName) );
		::DlgMessageBox( AfxGetMainWnd(), rcText, MB_OK );
	}
}

//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//
void MergeCheckErrorCode( HDATA hDefData, HDATA hImpData ) 
{
	bool bDifferent = false;
	HPROP hDefProp = ::DATA_Find_Prop( hDefData, NULL, VE_PID_IFE_ERRORCODEVALUE );
	if ( hDefProp ) {
		AVP_dword dwDefValue = ::GetPropNumericValueAsDWord( hDefProp );
		HPROP hImpProp = ::DATA_Find_Prop( hImpData, NULL, VE_PID_IFE_ERRORCODEVALUE );
		if ( hImpProp ) {
			AVP_dword dwImpValue = ::GetPropNumericValueAsDWord( hImpProp );
			bDifferent = (dwDefValue != dwImpValue);
		}
	}
	hDefProp = ::DATA_Find_Prop( hDefData, NULL, VE_PID_IFE_ISWARNING );
	if ( hDefProp ) {
		AVP_dword dwDefValue = ::GetPropNumericValueAsDWord( hDefProp );
		HPROP hImpProp = ::DATA_Find_Prop( hImpData, NULL, VE_PID_IFE_ISWARNING );
		if ( hImpProp ) {
			AVP_dword dwImpValue = ::GetPropNumericValueAsDWord( hImpProp );
			bDifferent = (dwDefValue != dwImpValue);
		}
	}
	
	if ( bDifferent ) {
		CString rcName;
		::MakePropFullName( hDefData, rcName, VE_PID_IFE_ERRORCODENAME );
		CString rcText;
		rcText.Format( IDS_ERR_MERGEDISCARDERRCODE, LPCTSTR(rcName) );
		::DlgMessageBox( AfxGetMainWnd(), rcText, MB_OK );
	}
}

//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//
void MergeMessageClass( HDATA hDefData, HDATA hImpData ) 
{
	/*
	if(!hDefData || !hImpData)
		return;

	bool bDifferent	= false;
	
	HDATA hCurDefClass, hCurImpClass;
	HDATA hCurDefMsg, hCurImpMsg;

	hCurDefClass = ::DATA_Get_First( hDefData, NULL );
	while ( hCurDefClass ) 
	{
		hCurImpClass = FindDataWithGuid(hImpData, hCurDefClass, true);
		if(hCurImpClass)
		{
		}

		hCurDefClass = ::DATA_Get_Next( hCurDefClass, NULL );
	}
	
	if ( bDifferent ) 
	{
		CString rcName;
		::MakePropFullName( hDefData, rcName, VE_PID_IFE_ERRORCODENAME );
		CString rcText;
		rcText.Format( IDS_ERR_MERGEDISCARDERRCODE, LPCTSTR(rcName) );
		::DlgMessageBox( AfxGetMainWnd(), rcText, MB_OK );
	}
	*/
}

//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//
HDATA MergeDefinitionAndImplementation( HDATA hDefinitionData, HDATA hImplementationData ) {
	if ( hDefinitionData && hImplementationData ) {
		HDATA hCurrIntData	= ::DATA_Get_First( hDefinitionData, NULL );
		while ( hCurrIntData ) {
			MAKE_ADDR1(nPropsAddr, VE_PID_IF_PROPERTIES);
			HDATA hCurrPropData = ::DATA_Get_First( hCurrIntData, nPropsAddr );
			while ( hCurrPropData ) {
				AVP_dword *pnPropAddr = NULL;
				::DATA_Make_Sequence( hCurrPropData, NULL, NULL, &pnPropAddr );

				if ( pnPropAddr ) {
					HDATA hIntPropData = ::DATA_Find( hImplementationData, pnPropAddr );
					if ( hIntPropData ) {
						MergeCheckPropType( hCurrPropData, hIntPropData ); 
					}
					::DATA_Remove_Sequence( pnPropAddr );
				}
				hCurrPropData = ::DATA_Get_Next( hCurrPropData, NULL );
			}
			hCurrIntData	= ::DATA_Get_Next( hCurrIntData, NULL );
		}
	}
	HDATA hResultData = NULL;
	::DTUT_MergeDataTreeAndPattern( hImplementationData, hDefinitionData, &hResultData );
	return hResultData;
}

//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//
HDATA FindInterfaceByInterface(  HDATA hRootData, HDATA hIntToFind ) 
{
	HDATA hRet = FindDataWithGuid(hRootData, hIntToFind, FALSE);
	if(NULL == hRet)
		hRet = FindDataWithGuid(hRootData, hIntToFind, TRUE);
	return hRet;
}

//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//
HDATA CopyAndAttachData( HDATA hRootData, HDATA hData) 
{
	HDATA hNewData = 
		::DATA_Copy( NULL, NULL, hData, DATA_IF_ROOT_INCLUDE);

	AVP_dword dwNewID = ::CreateDataId( hRootData );
	::DATA_Replace_ID( hNewData, NULL, AVP_word(dwNewID) );
	::DATA_Attach( hRootData, NULL, hNewData, DATA_IF_ROOT_INCLUDE );

	return hNewData;
}

//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//
BOOL CompareTreeUniqueIDs( HDATA hFData, HDATA hSData ) 
{
	CNodeUID rcFGuid;
	CNodeUID rcSGuid;
	if ( ::GetNodeUID(hFData, rcFGuid) && ::GetNodeUID(hSData, rcSGuid) ) 
		return ::CompareNodesUID( rcFGuid, rcSGuid );
	return FALSE;
}

//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//
static AVP_word g_dwImpPropTable[] = 
{
	// properties 
	VE_PID_IFP_SCOPE_ID,										

	VE_PID_IFPLS_WRITABLEDURINGINIT_ID,			
	VE_PID_IFPLS_REQUIRED_ID,								

	VE_PID_IFPLS_MEMBER_ID,									
	VE_PID_IFPLS_MEMBER_MODE_ID,						

	VE_PID_IFPLS_MODE_ID,										
	VE_PID_IFPLS_READFUNC_ID,								
	VE_PID_IFPLS_WRITEFUNC_ID,							

	VE_PID_IFPSS_VARIABLENAME_ID,						
	VE_PID_IFPSS_VARIABLEDEFVALUE_ID,

	// impl comment
	VE_PID_IMPLCOMMENT_ID,

	VE_PID_UNIQUEID_ID,
	VE_PID_IFP_PREDEFINED_ID,								
	VE_PID_IFP_HARDPREDEFINED_ID,						
	VE_PID_IFPSS_VARTYPIFIEDDEFVALUE_ID,	
};

void ImplPropsPreserver(HDATA i_hProtoData, HDATA i_hImplData, void*)
{
	if(	NULL == i_hProtoData ||
		NULL == i_hImplData)
		return;

	int dwImpPropTableCount = 
		sizeof(g_dwImpPropTable) / sizeof(g_dwImpPropTable[0]);

	for ( int i=0; i<dwImpPropTableCount; i++ ) 
	{
		UINT uiPropId = g_dwImpPropTable[i];

		HPROP hImplProp = ::FindPropByID( i_hImplData, uiPropId );
		if ( !hImplProp ) 
			continue;

		HPROP hProtoProp = ::FindPropByID( i_hProtoData, uiPropId );
		if(NULL == hProtoProp)
			hProtoProp = ::DATA_Add_Prop(i_hProtoData, NULL, ::PROP_Get_Id(hImplProp), 0, 0 );

		DTUT_ExchangePropValue(hImplProp, hProtoProp);
	}
}

//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//
// if impldata is not null and appropriate protodata is null
// simply copy impl data to proto data root
void ImplMsgPreserver(HDATA i_hImplData, HDATA i_hProtoData, void* i_pData)
{
	if(	NULL != i_hProtoData &&	NULL != i_hImplData)
		return;

	ASSERT(NULL != i_hImplData);

	HDATA hProtoMsgClassData = (HDATA)i_pData;
	ASSERT(NULL != hProtoMsgClassData);

	CopyAndAttachData(hProtoMsgClassData, i_hImplData);
}

//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//
typedef void (*pfnDatasIdentityChecker)(HDATA hOne, HDATA hTwo);
pfnDatasIdentityChecker GetDataIdentityChecker(UINT i_nDataId)
{
	switch(i_nDataId)
	{
	case VE_PID_IF_TYPES:		return MergeCheckType;
	case VE_PID_IF_CONSTANTS:	return MergeCheckConstant;
	case VE_PID_IF_ERRORCODES:	return MergeCheckErrorCode;
	case VE_PID_PL_MESSAGES:	return MergeMessageClass;
	case VE_PID_IF_DATASTRUCTURES: return NULL;
	case VE_PID_IF_PROPERTIES:	return MergeCheckPropType;
	case VE_PID_IF_METHODS:		return NULL;
	case VE_PID_IF_SYSTEMMETHODS: return NULL;
	default: 
		ASSERT(FALSE);
	}
	return NULL;
}

//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//
void	CVisualEditorDlg::SyncDatasCommon(
	HDATA io_hNewInt, HDATA i_hProtoInt, HDATA i_hImpInt, UINT i_nDataId)
{
	ASSERT(NULL != io_hNewInt);
	ASSERT(NULL != i_hProtoInt);
	ASSERT(NULL != i_nDataId);
	
	HDATA hImpData = NULL, hProtoData = NULL;

	MAKE_ADDR1( nDataAddr, i_nDataId);
	if(i_hImpInt) 
		hImpData = ::DATA_Find( i_hImpInt, nDataAddr );
	if(i_hProtoInt)
		hProtoData = ::DATA_Find( i_hProtoInt, nDataAddr );

	HDATA hNewImpData = ::DATA_Add( NULL, NULL, i_nDataId, 0, 0 );
	::DATA_Attach( io_hNewInt, NULL, hNewImpData, DATA_IF_ROOT_INCLUDE );
	
	HDATA hCurrImplData  = NULL;
	HDATA hCurrProtoData = NULL;

	//-----------------------------------------------------------------
	// Перебираем сначала в прототипе и копируем
	if(hProtoData)
		hCurrProtoData = ::DATA_Get_First( hProtoData, NULL );

	while ( hCurrProtoData ) 
	{
		// assure that in each proto data native prop is set
		::ForEach(hCurrProtoData, CheckNativeProp, NULL);

		// find corresponding node in implementation tree
		hCurrImplData = FindDataWithGuid(hImpData, hCurrProtoData);
		
		if ( hCurrImplData )
		{
			pfnDatasIdentityChecker pIdentityChecker
				= GetDataIdentityChecker(i_nDataId);
			if(pIdentityChecker)
				pIdentityChecker(hCurrProtoData, hCurrImplData);

			// move common properties from implementation
			// to prototype 
			// so ptototype data could be simply moved
			ForEachSync(hCurrProtoData, hCurrImplData, ImplPropsPreserver, NULL);

			if(VE_PID_PL_MESSAGES == i_nDataId)
				// move impl messages to the proto messages class
				ForEachSync(hCurrImplData, hCurrProtoData, ImplMsgPreserver, (void*)hCurrProtoData);
		}
		
		CopyAndAttachData( hNewImpData, hCurrProtoData );
		hCurrProtoData = ::DATA_Get_Next( hCurrProtoData, NULL );
	}

	//-----------------------------------------------------------------
	// Перебираем в Imp и копируем не-Native
	if(hImpData)
		hCurrImplData = ::DATA_Get_First( hImpData, NULL );
	while ( hCurrImplData ) 
	{
		if(!IsNativeData(hCurrImplData))
			if ( !::FindDataWithGuid( hNewImpData, hCurrImplData, false ) && 
				 !::FindDataWithGuid( hNewImpData, hCurrImplData, true ) ) 
				CopyAndAttachData( hNewImpData, hCurrImplData );

		hCurrImplData = ::DATA_Get_Next( hCurrImplData, NULL );
	}
}

//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//
