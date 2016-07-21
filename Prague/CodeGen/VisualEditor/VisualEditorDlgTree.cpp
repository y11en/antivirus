// ------------------------------------------------------------------------------------------

#include "stdafx.h"
#include "VisualEditorDlg.h"
#include "PVE_DT_UTILS.H"
#include "CodePages.h"
#include <hashutil/hashutil.h>

// ------------------------------------------------------------------------------------------

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

// ------------------------------------------------------------------------------------------

#define DISPLAY_PREFIX_NULL 1

// ------------------------------------------------------------------------------------------

AVP_dword FindNewPropertyID( HDATA hData, int i_nIdentProp, BOOL i_bIsPrototype );

// ------------------------------------------------------------------------------------------
MD::ETreeNodeType PropGetDefaultDisplayType(HDATA,AVP_dword,MD::ETreeNodeType i_eDefType)
{
	return i_eDefType;
}

MD::ETreeNodeType PropGetPropDefValueDisplayType(HDATA i_hData,AVP_dword,MD::ETreeNodeType i_eDefType)
{
	AVP_dword dwDTType = ::GetPropDTTypeByPragueType( i_hData );
	switch ( dwDTType ) 
	{
	case avpt_date : return MD::eDate;
	case avpt_time : return MD::eTime;
	case avpt_datetime : return MD::eDatetime;
	}
	return i_eDefType;
}

MD::ETreeNodeType PropGetPropNameDisplayType(HDATA i_hData,AVP_dword,MD::ETreeNodeType i_eDefType)
{
	if(!IsBoolPropSet(i_hData, VE_PID_IFP_HARDPREDEFINED))
		return MD::eStrDDList;
	return i_eDefType;
}

// ------------------------------------------------------------------------------------------
HPROP PropCreateCommon(HDATA i_hData, AVP_dword i_dwPropId, AVP_dword i_dwDefVal, BOOL& o_bModified)
{
	o_bModified = FALSE;
	HPROP hRetVal =	::DATA_Find_Prop( i_hData, NULL, i_dwPropId );
	if ( NULL == hRetVal )
	{
		hRetVal = ::DATA_Add_Prop(i_hData, NULL, i_dwPropId, i_dwDefVal, 0 );
		o_bModified = TRUE;
	}
	return hRetVal;
}

HPROP PropCreateCommonDate(HDATA i_hData, AVP_dword i_dwPropId, AVP_dword i_dwDefVal, BOOL& o_bModified)
{
	o_bModified = FALSE;

	AVP_date nEmptyValue;
	::ZeroMemory( &nEmptyValue, sizeof(nEmptyValue) );

	HPROP hRetVal =	::DATA_Find_Prop( i_hData, NULL, i_dwPropId );
	if ( NULL == hRetVal )
	{
		hRetVal = ::DATA_Add_Prop(i_hData, NULL, i_dwPropId, 0, 0 );
		::PROP_Set_Val( hRetVal, AVP_dword(&nEmptyValue), 0 );
		o_bModified = TRUE;
	}

	AVP_date nValue;
	::PROP_Get_Val( hRetVal, &nValue, sizeof(nValue) );

	if ( 0 == memcmp(&nValue, &nEmptyValue, sizeof(AVP_date)) ) 
	{
		SYSTEMTIME rcSysTime;
		CTime::GetCurrentTime().GetAsSystemTime( rcSysTime );
		::PROP_Set_Val( hRetVal, AVP_dword(&rcSysTime), 0 );
		o_bModified = TRUE;
	}

	return hRetVal;
}

HPROP PropCreateIdent(HDATA i_hData, AVP_dword i_dwPropId, AVP_dword i_dwDefVal, BOOL& o_bModified)
{
	o_bModified = FALSE;
	HPROP hRetVal =	::DATA_Find_Prop( i_hData, NULL, i_dwPropId );
	if ( NULL == hRetVal )
	{
		BOOL bIsPrototype = gpMainDlg->IsPrototype();
		AVP_dword nIDId = FindNewPropertyID( i_hData, i_dwPropId, bIsPrototype);
		hRetVal = ::DATA_Add_Prop( i_hData, NULL, i_dwPropId, AVP_dword(nIDId), 0 );
		o_bModified = TRUE;
	}
	return hRetVal;
}

HPROP PropCreatePropDigId(HDATA i_hData, AVP_dword, AVP_dword, BOOL& o_bModified)
{
	o_bModified = FALSE;

	BOOL bPredefinedProp		= IsBoolPropSet(i_hData, VE_PID_IFP_PREDEFINED);
	BOOL bHardPredefinedProp	= IsBoolPropSet(i_hData, VE_PID_IFP_HARDPREDEFINED);

	AVP_dword nIDId; 

	if ( !bPredefinedProp ) 
	{
		BOOL bIsPrototype = gpMainDlg->IsPrototype();
		nIDId =  FindNewPropertyID( i_hData, VE_PID_IFP_DIGITALID, bIsPrototype);
	}
	else
	{
		HPROP hProp = ::DATA_Find_Prop( i_hData, NULL, VE_PID_IFP_PROPNAME );
		CAPointer<char> pValue;
		if ( hProp ) 
		{
			pValue = ::GetPropValueAsString( hProp, NULL );
			nIDId = ::PropName2PropID( pValue, bHardPredefinedProp ? true:false );
		}
	}

	HPROP hProp = ::DATA_Find_Prop( i_hData, NULL, VE_PID_IFP_DIGITALID );

	if(hProp)
	{
		if(bPredefinedProp)
		{
			AVP_dword nOldIDId = ::GetPropNumericValueAsDWord( hProp );
			if(nOldIDId != nIDId)
			{
				::PROP_Set_Val( hProp, AVP_dword(nIDId), 0 );
				o_bModified = TRUE;
			}
		}
	}
	else
	{
		hProp = ::DATA_Add_Prop( i_hData, NULL, VE_PID_IFP_DIGITALID, AVP_dword(nIDId), 0 );
		o_bModified = TRUE;
	}

	return hProp;
}

HPROP PropCreatePropType(HDATA i_hData, AVP_dword, AVP_dword, BOOL& o_bModified)
{
	o_bModified = FALSE;

	BOOL bPredefinedProp		= IsBoolPropSet(i_hData, VE_PID_IFP_PREDEFINED);
	BOOL bHardPredefinedProp	= IsBoolPropSet(i_hData, VE_PID_IFP_HARDPREDEFINED);

	DWORD dwType				= DEFAULT_VAR_TYPE;
	AVP_dword dwTypeName		= AVP_dword(::PropTypeID2PropTypeName(dwType));

	if ( bPredefinedProp ) 
	{
		CAPointer<char> pValue;
		HPROP hProp = ::DATA_Find_Prop( i_hData, NULL, VE_PID_IFP_PROPNAME );
		if ( hProp ) 
		{
			pValue = ::GetPropValueAsString( hProp, NULL );
			dwTypeName = AVP_dword(::PropName2PropTypeName(pValue, bHardPredefinedProp ? true:false));
			dwType = ::PropName2PropType( pValue, bHardPredefinedProp ? true:false);
		}
	}

	HPROP hProp = ::DATA_Find_Prop( i_hData, NULL, VE_PID_IFP_TYPENAME );

	if(hProp)
	{
		if(bPredefinedProp)
		{
			CAPointer<char> strOldTypeName = 
				::GetPropValueAsString( hProp, NULL );

			if(0 != lstrcmp((char*)dwTypeName, strOldTypeName))
			{
				::PROP_Set_Val( hProp, dwTypeName, 0 );
				o_bModified = TRUE;
			}
		}
	}
	else
	{
		hProp = ::DATA_Add_Prop( i_hData, NULL, VE_PID_IFP_TYPENAME, dwTypeName, 0 );
		o_bModified = TRUE;
	}

	if ( !::DATA_Find_Prop(i_hData, NULL, VE_PID_IFP_TYPE) ) 
	{
		::DATA_Add_Prop( i_hData, NULL, VE_PID_IFP_TYPE, AVP_dword(dwType), 0 );
		o_bModified = TRUE;
	}

	return hProp;
}

HPROP PropCreatePluginName(HDATA i_hData, AVP_dword i_dwPropId, AVP_dword i_dwDefVal, BOOL& o_bModified)
{
	o_bModified = FALSE;
	HPROP hRetVal =	::DATA_Find_Prop( i_hData, NULL, i_dwPropId );
	if ( NULL == hRetVal )
	{
		CString strDefName;
		strDefName.LoadString( IDS_PLUGIN );

		hRetVal = ::DATA_Add_Prop( i_hData, NULL, i_dwPropId, AVP_dword(LPCSTR(strDefName)), 0 );
		::DATA_Add_Prop( i_hData, NULL, VE_PID_NODETYPE, AVP_dword(VE_NT_PLUGIN), 0 );

		o_bModified = TRUE;
	}
	return hRetVal;
}

HPROP PropCreatePluginMnemId(HDATA i_hData, AVP_dword i_dwPropId, AVP_dword i_dwDefVal, BOOL& o_bModified)
{
	o_bModified = FALSE;
	HPROP hRetVal =	::DATA_Find_Prop( i_hData, NULL, i_dwPropId );
	if ( NULL == hRetVal )
	{
		CAPointer<char> pLabel;
		pLabel  = ::CreatePluginMnemonicIDLabel( i_hData );
		hRetVal = ::DATA_Add_Prop( i_hData, NULL, i_dwPropId, AVP_dword(LPCSTR(pLabel)), 0 );
		o_bModified = TRUE;
	}
	return hRetVal;
}

HPROP PropCreateIntMnemId(HDATA i_hData, AVP_dword i_dwPropId, AVP_dword i_dwDefVal, BOOL& o_bModified)
{
	o_bModified = FALSE;
	HPROP hRetVal =	::DATA_Find_Prop( i_hData, NULL, i_dwPropId );
	if ( NULL == hRetVal )
	{
		CAPointer<char> pLabel;
		pLabel  = ::CreateMnemonicIDLabel( i_hData );
		hRetVal = ::DATA_Add_Prop( i_hData, NULL, i_dwPropId, AVP_dword(LPCSTR(pLabel)), 0 );
		o_bModified = TRUE;
	}
	return hRetVal;
}

HPROP PropCreateIntIncludeFile(HDATA i_hData, AVP_dword i_dwPropId, AVP_dword i_dwDefVal, BOOL& o_bModified)
{
	o_bModified = FALSE;
	HPROP hRetVal =	::DATA_Find_Prop( i_hData, NULL, i_dwPropId );
	if ( NULL == hRetVal )
	{
		CAPointer<char> pLabel;
		pLabel  = ::CreateIncludeFileLabel( i_hData );
		hRetVal = ::DATA_Add_Prop( i_hData, NULL, i_dwPropId, AVP_dword(LPCSTR(pLabel)), 0 );
		o_bModified = TRUE;
	}
	return hRetVal;
}

HPROP PropCreateIntObjType(HDATA i_hData, AVP_dword i_dwPropId, AVP_dword i_dwDefVal, BOOL& o_bModified)
{
	o_bModified = FALSE;
	HPROP hRetVal =	::DATA_Find_Prop( i_hData, NULL, i_dwPropId );
	if ( NULL == hRetVal )
	{
		CAPointer<char> pLabel;
		pLabel  = ::CreateObjectTypeLabel( i_hData );
		hRetVal = ::DATA_Add_Prop( i_hData, NULL, i_dwPropId, AVP_dword(LPCSTR(pLabel)), 0 );
		o_bModified = TRUE;
	}
	return hRetVal;
}

static AVP_dword GetIIDServerID(PgIDCategories i_eIdCategory)
{
	AVP_dword nIDId = -1;
	if ( gpMainDlg->m_pIIDServer ) 
	{
		HRESULT hr = gpMainDlg->m_pIIDServer->GetID( i_eIdCategory, &nIDId );

		if ( !SUCCEEDED(hr) )
			gpMainDlg->DisplayPIDSError( hr );
	}
	return nIDId;
}

HPROP PropCreateIntDigId(HDATA i_hData, AVP_dword i_dwPropId, AVP_dword, BOOL& o_bModified)
{
	o_bModified = FALSE;
	HPROP hRetVal =	::DATA_Find_Prop( i_hData, NULL, i_dwPropId );
	if ( NULL == hRetVal ) {
		AVP_dword nIDId = GetIIDServerID(pic_Interface);
		if ( nIDId != -1 ) {
			hRetVal = ::DATA_Add_Prop( i_hData, NULL, i_dwPropId, AVP_dword(nIDId), 0 );
			o_bModified = TRUE;
		}
	}
	return hRetVal;
}

HPROP PropCreatePluginDigId(HDATA i_hData, AVP_dword i_dwPropId, AVP_dword i_dwDefVal, BOOL& o_bModified)
{
	o_bModified = FALSE;
	HPROP hRetVal =	::DATA_Find_Prop( i_hData, NULL, i_dwPropId );
	if ( NULL == hRetVal )
	{
		AVP_dword nIDId = GetIIDServerID(pic_Plugin);
		if ( nIDId != -1 ) {
			hRetVal = ::DATA_Add_Prop( i_hData, NULL, i_dwPropId, AVP_dword(nIDId), 0 );
			o_bModified = TRUE;
		}
	}
	return hRetVal;
}

HPROP PropCreateIntVendorId(HDATA i_hData, AVP_dword i_dwPropId, AVP_dword i_dwDefVal, BOOL& o_bModified)
{
	o_bModified = FALSE;
	HPROP hRetVal =	::DATA_Find_Prop( i_hData, NULL, i_dwPropId );
	if ( NULL == hRetVal )
	{
		AVP_dword nIDId = GetIIDServerID(pic_Vendor);
		if ( nIDId != -1 ) {
			hRetVal = ::DATA_Add_Prop( i_hData, NULL, i_dwPropId, AVP_dword(nIDId), 0 );
			o_bModified = TRUE;
		}
	}
	return hRetVal;
}

HPROP PropCreatePluginVendor(HDATA i_hData, AVP_dword i_dwPropId, AVP_dword i_dwDefVal, BOOL& o_bModified)
{
	o_bModified = FALSE;
	HPROP hRetVal =	::DATA_Find_Prop( i_hData, NULL, i_dwPropId );
	if ( NULL == hRetVal )
	{
		TCHAR pName[_MAX_PATH] = {0};

		if ( gpMainDlg->m_pIIDServer ) 
		{
			HRESULT hr = gpMainDlg->
				m_pIIDServer->GetVendorName( sizeof(pName), (BYTE *)pName );
			if ( !SUCCEEDED(hr) )
				gpMainDlg->DisplayPIDSError( hr );
		}

		hRetVal = ::DATA_Add_Prop( i_hData, NULL, i_dwPropId, AVP_dword(LPCSTR(pName)), 0 );
		o_bModified = TRUE;

		if(!::DATA_Find_Prop( i_hData, NULL, VE_PID_PL_VENDORNAME ))
			::DATA_Add_Prop( i_hData, NULL, VE_PID_PL_VENDORNAME, AVP_dword(pName), 0 );
	}
	return hRetVal;
}

HPROP PropCreatePluginVendorId(HDATA i_hData, AVP_dword i_dwPropId, AVP_dword i_dwDefVal, BOOL& o_bModified)
{
	o_bModified = FALSE;

	if(!::DATA_Find_Prop( i_hData, NULL, VE_PID_PL_VENDORMNEMONICID ))
	{
		TCHAR pMnemonicID[_MAX_PATH] = {0};

		if ( gpMainDlg->m_pIIDServer ) 
		{
			HRESULT hr = 
				gpMainDlg->
					m_pIIDServer->GetVendorMnemonicID( sizeof(pMnemonicID), (BYTE *)pMnemonicID );
			if ( !SUCCEEDED(hr) )
				gpMainDlg->DisplayPIDSError( hr );
		}
		::DATA_Add_Prop( i_hData, NULL, VE_PID_PL_VENDORMNEMONICID, AVP_dword(pMnemonicID), 0 );
		o_bModified = TRUE;
	}

	HPROP hRetVal =	::DATA_Find_Prop( i_hData, NULL, i_dwPropId );
	if ( NULL == hRetVal )
	{
		AVP_dword nIDId = 1;

		if ( gpMainDlg->m_pIIDServer ) 
		{
			HRESULT hr = gpMainDlg->
					m_pIIDServer->GetID( pic_Vendor, &nIDId );
			if ( !SUCCEEDED(hr) )
				gpMainDlg->
					DisplayPIDSError( hr );
		}

		hRetVal = ::DATA_Add_Prop( i_hData, NULL, i_dwPropId, AVP_dword(nIDId), 0 );
		o_bModified = TRUE;
	}
	return hRetVal;
}

HPROP PropCreateOutputFileLabel(HDATA i_hData, AVP_dword i_dwPropId, AVP_dword i_dwDefVal, BOOL& o_bModified)
{
	o_bModified = FALSE;
	HPROP hRetVal =	::DATA_Find_Prop( i_hData, NULL, i_dwPropId );
	if ( NULL == hRetVal )
	{
		CAPointer<char> pLabel;
		pLabel  = ::CreateOutputFileLabel( i_hData );
		hRetVal = ::DATA_Add_Prop( i_hData, NULL, i_dwPropId, AVP_dword(LPCSTR(pLabel)), 0 );
		o_bModified = TRUE;
	}
	return hRetVal;
}

HPROP PropCreateMethodResultType(HDATA i_hData, AVP_dword i_dwPropId, AVP_dword i_dwDefVal, BOOL& o_bModified)
{
	o_bModified = FALSE;
	HPROP hRetVal =	::DATA_Find_Prop( i_hData, NULL, i_dwPropId );
	if ( NULL == hRetVal )
	{
		::DATA_Add_Prop( i_hData, NULL, VE_PID_IFM_RESULTTYPE, AVP_dword(i_dwDefVal), 0 );
		hRetVal = ::DATA_Add_Prop( i_hData, NULL, i_dwPropId, AVP_dword(::GTypeID2TypeName(i_dwDefVal)), 0 );
		o_bModified = TRUE;
	}
	return hRetVal;
}

HPROP PropCreateMethodParType(HDATA i_hData, AVP_dword i_dwPropId, AVP_dword i_dwDefVal, BOOL& o_bModified)
{
	o_bModified = FALSE;
	HPROP hRetVal =	::DATA_Find_Prop( i_hData, NULL, i_dwPropId );
	if ( NULL == hRetVal )
	{
		SetDwordProp(i_hData, VE_PID_IFMP_TYPE, i_dwDefVal);
		hRetVal = 
			::DATA_Add_Prop( i_hData, NULL, i_dwPropId, AVP_dword(::GTypeID2TypeName(i_dwDefVal)), 0 );
		o_bModified = TRUE;
	}
	return hRetVal;
}

// ------------------------------------------------------------------------------------------
TCHAR* PropDisplayNameStr(HDATA , HPROP i_hProp, int i_nArrayPosition, int i_nPrefixId)
{
	CAPointer<TCHAR> pLabel;

	if(0 == i_nArrayPosition)
		pLabel = ::GetPropValueAsString( i_hProp, NULL );
	else
	{
		int nCount = ::PROP_Arr_Count( i_hProp );
		if(i_nArrayPosition <= nCount)
			pLabel = ::GetPropArrayValueAsString( i_hProp, i_nArrayPosition-1, NULL );
		else
		{
			TCHAR * strNewValue = new TCHAR[25];
			lstrcpy(strNewValue, _T("New Value"));
			pLabel = strNewValue;
		}
	}

	if(i_nPrefixId)
		pLabel = ::CreateDisplayLabelSTR( i_nPrefixId, pLabel );

	return pLabel.Relinquish();
}

TCHAR* PropDisplayNameDWORD(HDATA , HPROP i_hProp, int, int i_nPrefixId)
{
	CAPointer<TCHAR> pLabel;
	AVP_dword dwVal = ::GetPropNumericValueAsDWord( i_hProp );
	pLabel = ::CreateDisplayLabelINT( i_nPrefixId, dwVal );
	return pLabel.Relinquish();
}

TCHAR* PropDisplayNameDWORDDec(HDATA , HPROP i_hProp, int, int i_nPrefixId)
{
	CAPointer<TCHAR> pLabel;
	AVP_dword dwVal = ::GetPropNumericValueAsDWord( i_hProp );
	pLabel = ::CreateDisplayLabelINT( i_nPrefixId, dwVal, FALSE );
	return pLabel.Relinquish();
}

TCHAR* PropDisplayNameAsPrefix(HDATA , HPROP , int, int i_nPrefixId)
{
	CString str;
	str.LoadString(i_nPrefixId);
	TCHAR* strRetVal = new TCHAR[str.GetLength()+1];
	lstrcpy(strRetVal, str);
	return strRetVal;
}

TCHAR* PropDisplayNameConstantDropDown(HDATA , HPROP i_Prop, int, int i_nPrefixId)
{
	CAPointer<char> strPropValue = ::GetPropValueAsString( i_Prop, NULL );
	CAPointer<char> strConstValue = GetConstantNameOnValue(strPropValue);
	CAPointer<char> pLabel;
	if(strConstValue)
		pLabel = ::CreateDisplayLabelSTR( i_nPrefixId, strConstValue);
	else
		pLabel = ::CreateDisplayLabelSTR( i_nPrefixId, strPropValue);
	return pLabel.Relinquish();
}

TCHAR* PropDisplayNamePropDefValue(HDATA i_hData, HPROP i_Prop, int, int i_nPrefixId)
{
	gpMainDlg->SetPropTypifiedDefValue( i_hData );

	CAPointer<char> pLabel;
	pLabel = ::GetPropValueAsString( i_Prop, NULL );
	pLabel = ::CreateDisplayLabelSTR( i_nPrefixId, pLabel );
	return pLabel.Relinquish();
}

TCHAR* PropDisplayNamePropertyName(HDATA iData, HPROP i_Prop, int, int i_nPrefixId)
{
	AVP_dword nWorkMode = 
		gpMainDlg->m_nWorkMode;

	return ::GetPropertyNameDisplayString(iData, i_Prop, nWorkMode, i_nPrefixId ? FALSE : TRUE);
}

TCHAR* PropPluginCPDisplayString(HDATA iData, HPROP i_Prop, int, int i_nPrefixId)
{
	AVP_dword nIDId = ::GetPropNumericValueAsDWord( i_Prop );

	// patch old data
	if(cCP_NULL==nIDId)
	{
		nIDId = cCP_ANSI;
		::DATA_Set_Val( iData, NULL, VE_PID_PL_CODEPAGEID, nIDId, 0 );
	}

	CCodePages oCP;
	CString strCodePageName = oCP.GetCodePageById(nIDId);
	CAPointer<char> pLabel = 
		::CreateDisplayLabelSTR( i_nPrefixId, strCodePageName );

	return pLabel.Relinquish();
}

TCHAR* PropIntMnemDisplayString(HDATA i_iData, HPROP i_hProp, int, int i_nPrefixId)
{
	CAPointer<char> pLabel;
	pLabel = ::GetPropValueAsString( i_hProp, NULL );

	// add interface id to the global constans list
	HPROP hProp = ::DATA_Find_Prop( i_iData, NULL, VE_PID_IF_DIGITALID );
	if(hProp)
	{
		TCHAR* strVal = ::GetPropValueAsString( hProp, _T("%d"));
		AddGlobalConst(avpt_int, _strdup(pLabel), strVal);
	}

	pLabel = ::CreateDisplayLabelSTR( i_nPrefixId, pLabel );
	return pLabel.Relinquish();
}

TCHAR* PropMethodDisplayName(HDATA i_iData, HPROP i_hProp, int, int i_nPrefixId)
{
	CPubMethodInfo method( i_iData, false );
	
	if ( !method.MethodID() ) 
	{
		const char* name = method.Name();
		method.SetMethodID( ::iCountCRC32str(name) );
		//SetModified( TRUE );
	}
	
	CAPointer<char> pLabel = GetMethodNameDisplayString( method, i_nPrefixId ? FALSE : TRUE );
	return pLabel.Relinquish();
}

TCHAR* PropSysMethodName(HDATA i_iData, HPROP i_hProp, int, int i_nPrefixId)
{
	CIntMethodInfo method( i_iData, false );
	CAPointer<char> pLabel = GetMethodNameDisplayString( method );
	return pLabel.Relinquish();
}

TCHAR* PropDisplayNameStrArray(HDATA i_iData, HPROP i_hProp, int i_nArrayPosition, int i_nPrefixId)
{
	if(i_nArrayPosition)
		return PropDisplayNameStr(NULL, i_hProp, i_nArrayPosition, 0);
	else
		return PropDisplayNameAsPrefix(NULL, NULL, 0, i_nPrefixId);
}

// ------------------------------------------------------------------------------------------
#define STR_PROP(id,pref,init,fnCreate,fnGetDisplayName,fnGetDisplayType) \
	{ id,pref,MD::eString,AVP_dword(init),fnCreate,fnGetDisplayName,fnGetDisplayType},

#define STR_PROP2(id,pref,init,fnCreate,fnGetDisplayName) \
	STR_PROP(id,pref,init,fnCreate,fnGetDisplayName,PropGetDefaultDisplayType)

#define CMN_STR_PROP(id,pref,init) \
	STR_PROP(id,pref,init,PropCreateCommon,PropDisplayNameStr,PropGetDefaultDisplayType)

#define EXT_EDIT_PROP(id,pref,init,fnCreate,fnGetDisplayName,fnGetDisplayType) \
	{ id,pref,MD::eStrExtEdit,AVP_dword(init),fnCreate,fnGetDisplayName,fnGetDisplayType},

#define CMN_EXT_EDIT_PROP(id,pref) \
	EXT_EDIT_PROP(id,pref,0,PropCreateCommon,PropDisplayNameStr,PropGetDefaultDisplayType)

#define DDLIST_PROP(id,pref,init,fnCreate,fnGetDisplayName,fnGetDisplayType) \
	{ id,pref,MD::eDDList,AVP_dword(init),fnCreate,fnGetDisplayName,fnGetDisplayType},

#define DDLIST_PROP2(id,pref,init,fnCreate,fnGetDisplayName) \
	DDLIST_PROP(id,pref,init,fnCreate,fnGetDisplayName,PropGetDefaultDisplayType)

#define CMN_DDLIST_PROP(id,pref,init) \
	DDLIST_PROP(id,pref,init,PropCreateCommon,PropDisplayNameStr,PropGetDefaultDisplayType)

#define STRDDLIST_PROP(id,pref,init,fnCreate,fnGetDisplayName,fnGetDisplayType) \
	{ id,pref,MD::eStrDDList,AVP_dword(init),fnCreate,fnGetDisplayName,fnGetDisplayType},

#define STRDDLIST_PROP2(id,pref,init,fnCreate,fnGetDisplayName) \
	STRDDLIST_PROP(id,pref,init,fnCreate,fnGetDisplayName,PropGetDefaultDisplayType)

#define CMN_STRDDLIST_PROP(id,pref,init) \
	DDLIST_PROP(id,pref,init,PropCreateCommon,PropDisplayNameStr,PropGetDefaultDisplayType)

#define CHECK_PROP(id,pref,init,fnCreate,fnGetDisplayName,fnGetDisplayType) \
	{ id,pref,MD::eCheck,AVP_dword(init),fnCreate,fnGetDisplayName,fnGetDisplayType},

#define CHECK_PROP2(id,pref,init,fnCreate,fnGetDisplayName) \
	CHECK_PROP(id,pref,init,fnCreate,fnGetDisplayName,PropGetDefaultDisplayType)

#define CMN_CHECK_PROP(id,pref,init) \
	CHECK_PROP(id,pref,init,PropCreateCommon,PropDisplayNameAsPrefix,PropGetDefaultDisplayType)

#define RADIO_PROP(id,pref,init,fnCreate,fnGetDisplayName,fnGetDisplayType) \
	{ id,pref,MD::eRadio,AVP_dword(init),fnCreate,fnGetDisplayName,fnGetDisplayType},

#define RADIO_PROP2(id,pref,init,fnCreate,fnGetDisplayName) \
	RADIO_PROP(id,pref,init,fnCreate,fnGetDisplayName,PropGetDefaultDisplayType)

#define CMN_RADIO_PROP(id,init) \
	RADIO_PROP(id,0,init,PropCreateCommon,PropDisplayNameAsPrefix,PropGetDefaultDisplayType)

#define DATE_PROP(id,pref,init,fnCreate,fnGetDisplayName,fnGetDisplayType) \
	{ id,pref,MD::eDate,AVP_dword(init),fnCreate,fnGetDisplayName,fnGetDisplayType},

#define DATE_PROP2(id,pref,init,fnCreate,fnGetDisplayName) \
	DATE_PROP(id,pref,init,fnCreate,fnGetDisplayName,PropGetDefaultDisplayType)

#define CMN_DATE_PROP(id,pref) \
	DATE_PROP(id,pref,0,PropCreateCommonDate,PropDisplayNameStr,PropGetDefaultDisplayType)

// ------------------------------------------------------------------------------------------

MD::SPropMetadata CVisualEditorDlg::s_arrPropMetadata[] = 
{	
	CMN_STR_PROP( VE_PID_SHORTCOMMENT,		IDS_SHORTCOMMENT_PREFIX,		0)
	CMN_STR_PROP( VE_PID_IFC_VALUE,			IDS_VALUE_PREFIX,				"0")
	CMN_STR_PROP( VE_PID_IFM_MESSAGENAME,	0,								0)
	CMN_STR_PROP( VE_PID_IFT_TYPENAME,		0,								"0")
	CMN_STR_PROP( VE_PID_IFM_MESSAGECLASSNAME,0,							"MsgClass")
	CMN_STR_PROP( VE_PID_IFD_DATASTRUCTURENAME,0,							"")		
	CMN_STR_PROP( VE_PID_IFDM_MEMBERNAME,	0,								"")		
	CMN_STR_PROP( VE_PID_IFDM_TYPENAME,		IDS_DSMTYPENAME_PREFIX,			"tDWORD")
	CMN_STR_PROP( VE_PID_IFDM_ARRAYBOUND,	IDS_DSMARRAYBOUND_PREFIX,		"1")		
	CMN_STR_PROP( VE_PID_IFLCP_READFUNC,	IDS_PLSREADFUNC_PREFIX,			0)		
	CMN_STR_PROP( VE_PID_IFLCP_WRITEFUNC,	IDS_PLSWRITEFUNC_PREFIX,		0)		
	CMN_STR_PROP( VE_PID_IFPLS_READFUNC,	IDS_PLSREADFUNC_PREFIX,			0)			
	CMN_STR_PROP( VE_PID_IFPLS_WRITEFUNC,	IDS_PLSWRITEFUNC_PREFIX,		0)		
	CMN_STR_PROP( VE_PID_IFPSS_VARIABLENAME,IDS_PROPVARIABLE_PREFIX,		0)			
	CMN_STR_PROP( VE_PID_PL_PROJECTNAME,	IDS_PLPROJECTNAME_PREFIX,		0)			
	CMN_STR_PROP( VE_PID_PL_SUBPROJECTNAME,	IDS_PLSUBPROJECTNAME_PREFIX,	0)			
	CMN_STR_PROP( VE_PID_IF_INTERFACENAME,	0,								"")			
	CMN_STR_PROP( VE_PID_IF_REVISIONID,		IDS_REVISIONID_PREFIX,			0)	
	CMN_STR_PROP( VE_PID_IF_AUTORNAME,		IDS_AUTORNAME_PREFIX,			0)	
	CMN_STR_PROP( VE_PID_IFM_RESULTDEFVALUE,IDS_RESULTDEFVALUE_PREFIX,		0)	
	CMN_STR_PROP( VE_PID_IFMP_PARAMETERNAME,0,								0)	
	
//	VE_PID_IFE_ERRORCODEVALUE

	STR_PROP2( VE_PID_IFC_CONSTANTNAME,		DISPLAY_PREFIX_NULL,		"Const",	PropCreateCommon,		GetConstantNameDisplayString)
	STR_PROP2( VE_PID_IFE_ERRORCODENAME,	DISPLAY_PREFIX_NULL,		"ErrCode",	PropCreateCommon,		GetErrorCodeNameDisplayString)
	STR_PROP2( VE_PID_IFP_DIGITALID,		IDS_DIGITALID_PREFIX,		0,			PropCreatePropDigId,	PropDisplayNameDWORD)
	STR_PROP2( VE_PID_IF_INCLUDENAMES,		IDS_INCLUDES,				0,			PropCreateCommon,		PropDisplayNameStrArray)
	STR_PROP2( VE_PID_IFM_MESSAGECLASSID,	IDS_IDENTIFICATOR_PREFIX,	0,			PropCreateIdent,		PropDisplayNameDWORD)
	STR_PROP2( VE_PID_IFM_MESSAGEID,		IDS_IDENTIFICATOR_PREFIX,	0,			PropCreateIdent,		PropDisplayNameDWORD)
	STR_PROP2( VE_PID_PL_DATEFORMATID,		IDS_PLDATEFORMAT_PREFIX,	0,			PropCreateCommon,		PropDisplayNameDWORD)

	STR_PROP2( VE_PID_PL_PLUGINNAME,		IDS_PLNAME_PREFIX,			0,			PropCreatePluginName,	PropDisplayNameStr)
	STR_PROP2( VE_PID_PL_MNEMONICID,		IDS_MNEMONICID_PREFIX,		0,			PropCreatePluginMnemId,	PropDisplayNameStr)
	STR_PROP2( VE_PID_PL_DIGITALID,			IDS_PLDIGITALID_PREFIX,		0,			PropCreatePluginDigId,	PropDisplayNameDWORDDec)
	STR_PROP2( VE_PID_IF_VENDORID,			IDS_PLVENDORID_PREFIX,		0,			PropCreateIntVendorId,	PropDisplayNameStr)
	STR_PROP2( VE_PID_PL_VERSIONID,			IDS_PLVERSION_PREFIX,		1,			PropCreateCommon,		PropDisplayNameDWORD)
	STR_PROP2( VE_PID_PL_AUTHORNAME,		IDS_PLAUTHORNAME_PREFIX,	0,			PropCreatePluginVendor,	PropDisplayNameStr)
	STR_PROP2( VE_PID_PL_VENDORID,			IDS_PLVENDORID_PREFIX,		0,			PropCreatePluginVendorId,PropDisplayNameDWORD)
	STR_PROP2( VE_PID_IF_MNEMONICID,		IDS_MNEMONICID_PREFIX,		"",			PropCreateIntMnemId,	PropIntMnemDisplayString)
	STR_PROP2( VE_PID_IF_INCLUDEFILE,		IDS_INCLUDEFILE_PREFIX,		0,			PropCreateIntIncludeFile,PropDisplayNameStr)
	STR_PROP2( VE_PID_IF_OBJECTTYPE,		IDS_OBJECTTYPE_PREFIX,		0,			PropCreateIntObjType,	PropDisplayNameStr)	

	STR_PROP2( VE_PID_IF_DIGITALID,			IDS_DIGITALID_PREFIX,		0,			PropCreateIntDigId,		PropDisplayNameDWORDDec)
	STR_PROP2( VE_PID_IF_OUTPUTFILE,		IDS_OUTPUTFILE_PREFIX,		0,			PropCreateOutputFileLabel,PropDisplayNameStr)
	STR_PROP2( VE_PID_IFM_METHODNAME,		DISPLAY_PREFIX_NULL,		"",			PropCreateCommon,		PropMethodDisplayName)
	STR_PROP2( VE_PID_IF_SUBTYPEID,			IDS_SUBTYPEID_PREFIX,		0,			PropCreateCommon,		PropDisplayNameDWORD)

	STR_PROP( VE_PID_IFP_PROPNAME,	DISPLAY_PREFIX_NULL, "PropName",		PropCreateCommon,	PropDisplayNamePropertyName,	PropGetPropNameDisplayType)

	CMN_EXT_EDIT_PROP( VE_PID_IFM_MESSAGE_SEND_POINT,IDS_MSG_SEND_POINT_PREFIX)
	CMN_EXT_EDIT_PROP( VE_PID_IFM_MESSAGECONTEXT,	IDS_MSG_CONTEXT_PREFIX)	
	CMN_EXT_EDIT_PROP( VE_PID_IFM_MESSAGEDATA,		IDS_MSG_DATA_PREFIX)	
	CMN_EXT_EDIT_PROP( VE_PID_IMPLCOMMENT,			IDS_IMPLCOMMENT_PREFIX)	
	CMN_EXT_EDIT_PROP( VE_PID_LARGECOMMENT,			IDS_LARGECOMMENT_PREFIX)
	CMN_EXT_EDIT_PROP( VE_PID_BEHAVIORCOMMENT,		IDS_BEHAVIORCOMMENT_PREFIX)
	CMN_EXT_EDIT_PROP( VE_PID_VALUECOMMENT,			IDS_VALUECOMMENT_PREFIX)
	CMN_EXT_EDIT_PROP( VE_PID_IFPLS_MEMBER,			IDS_PLSMEMBER_PREFIX)	
	
	CMN_DDLIST_PROP( VE_PID_IFT_BASETYPENAME,	IDS_BASETYPENAME_PREFIX,		0)
	CMN_DDLIST_PROP( VE_PID_IFC_BASETYPENAME,	IDS_CONSTTYPENAME_PREFIX,		0)
	DDLIST_PROP2( VE_PID_IFP_TYPENAME,			IDS_PROPTYPENAME_PREFIX,		0, 						PropCreatePropType,	PropDisplayNameStr)
	DDLIST_PROP2( VE_PID_PL_CODEPAGEID ,		IDS_PLCODEPAGE_PREFIX,			cCP_ANSI, 				PropCreateCommon,	PropPluginCPDisplayString)
	DDLIST_PROP2( VE_PID_IFM_RESULTTYPENAME ,	IDS_RESULTNAME_PREFIX,			DEFAULT_METHOD_TYPE,	PropCreateMethodResultType,	PropDisplayNameStr)

	STRDDLIST_PROP2( VE_PID_IFMP_TYPENAME,		IDS_PARAMTYPENAME_PREFIX,		DEFAULT_VAR_TYPE, 		PropCreateMethodParType,	PropDisplayNameStr)

	STRDDLIST_PROP ( VE_PID_IFPSS_VARIABLEDEFVALUE,	IDS_PROPDEFVALUE_PREFIX,	0,		PropCreateCommon,	PropDisplayNamePropDefValue, PropGetPropDefValueDisplayType)
	
	CMN_CHECK_PROP( VE_PID_IFE_ISWARNING,		IDS_ERRORISWARNING,		0)				
	CMN_CHECK_PROP( VE_PID_IF_STATIC,			IDS_STATIC,				0)						
	CMN_CHECK_PROP( VE_PID_IF_SYSTEM,			IDS_SYSTEM,				0)						
	CMN_CHECK_PROP( VE_PID_IF_TRANSFERPROPUP,	IDS_TRANSFERPROPUP,		0)				
	CMN_CHECK_PROP( VE_PID_IF_NONSWAPABLE,		IDS_NONSWAPABLE,		0)				
	CMN_CHECK_PROP( VE_PID_IFDM_ISARRAY,		IDS_MEMBERISARRAY,		0)				
	CMN_CHECK_PROP( VE_PID_IF_LASTCALLPROP,		IDS_LASTCALLPROP,		0)				
	CMN_CHECK_PROP( VE_PID_IFPLS_WRITABLEDURINGINIT,IDS_PROPWRITABLEDURINGINIT, 0)	
	CMN_CHECK_PROP( VE_PID_IFPLS_REQUIRED,		IDS_PROPREQUIRED,		0)				
	CMN_CHECK_PROP( VE_PID_IFMP_ISPOINTER,		IDS_POINTER,			0)				
	CMN_CHECK_PROP( VE_PID_IFMP_IS_DOUBLE_POINTER,IDS_POINTER,			0)				
	CMN_CHECK_PROP( VE_PID_IFMP_ISCONST,		IDS_CONST ,				0)				
	CMN_CHECK_PROP( VE_PID_PL_AUTOSTART,		IDS_AUTOSTART,			0)				

	CHECK_PROP2( VE_PID_IFM_SELECTED,			0,						0,		PropCreateCommon,	PropSysMethodName)				

	CMN_RADIO_PROP( VE_PID_IFP_SCOPE,			VE_IFP_LOCAL) 	
	CMN_RADIO_PROP( VE_PID_IFPLS_MEMBER_MODE,	VE_IFPLS_MEMBER_READ)
	CMN_RADIO_PROP( VE_PID_IFPLS_MODE,			VE_IFPLS_NONE)
	CMN_RADIO_PROP( VE_PID_IF_PROTECTED_BY,		VE_IFF_PROTECTED_BY_NONE)

	CMN_DATE_PROP( VE_PID_IF_CREATIONDATE,		IDS_CREATIONDATE_PREFIX )
};

// ------------------------------------------------------------------------------------------
MD::SPropMetadata* CVisualEditorDlg::FindPropMetadata(int i_nPropId, BOOL i_bPropUniqueId)
{
	int iTblSize = sizeof(s_arrPropMetadata) / sizeof(s_arrPropMetadata[0]);
	for(int i=0; i<iTblSize; i++)
	{
		if(i_bPropUniqueId)
		{
			if(GET_AVP_PID_ID(s_arrPropMetadata[i].m_nPropId) == UINT(i_nPropId))
				return &s_arrPropMetadata[i];
		}
		else
			if(s_arrPropMetadata[i].m_nPropId == i_nPropId)
				return &s_arrPropMetadata[i];
	}

	ASSERT(FALSE);
	return NULL;
}

// ------------------------------------------------------------------------------------------
HPROP CVisualEditorDlg::CreateDataProp(HDATA i_hData, int i_nPropId)
{
	ASSERT(NULL != i_hData);
	HPROP hProp = ::DATA_Find_Prop(i_hData,NULL,i_nPropId);

	if(hProp)
		return hProp;

	AVP_dword dwRetVal = 0;
	MD::SPropMetadata* pPropMetadata = FindPropMetadata(i_nPropId);
	if(pPropMetadata)	
	{
		BOOL bModified;
		hProp = pPropMetadata->m_pfnPropConstructor
				(i_hData, i_nPropId, pPropMetadata->m_dwPropDefVal, bModified);
	}
	return hProp;
}

// ------------------------------------------------------------------------------------------
void CVisualEditorDlg::CreateTreeNode(STreeNodeInfo& i_oTreeNodeInfo)
{
	HPROP hValueProp = NULL;
	CString strLabel;
	MD::ETreeNodeType eNodeType = i_oTreeNodeInfo.m_eTreeNodeType;

	if(i_oTreeNodeInfo.m_nPropId)
	{
		CAPointer<TCHAR> pLabel;
		MD::SPropMetadata* pPropMetadata = 
			FindPropMetadata(i_oTreeNodeInfo.m_nPropId);
		hValueProp = ::DATA_Find_Prop( i_oTreeNodeInfo.m_hData, NULL, i_oTreeNodeInfo.m_nPropId );

		// if parent is separator - only comments are allowed
		if(	::IsSeparator(i_oTreeNodeInfo.m_hData) &&
			!IsCommentPropId(GET_AVP_PID_ID(i_oTreeNodeInfo.m_nPropId)))
		{
			if(hValueProp)
				pLabel = ::GetPropValueAsString( hValueProp, NULL );
			TCHAR strPropTitle[256];
			lstrcpy(strPropTitle, pLabel);
			CPathStr pDispStr( _MAX_PATH );
			::GetSeparatorDisplayString( pDispStr, strPropTitle );
			pLabel = pDispStr.Relinquish();	
		}
		else
		{
			BOOL bModified;
			hValueProp = pPropMetadata->m_pfnPropConstructor
				(i_oTreeNodeInfo.m_hData, i_oTreeNodeInfo.m_nPropId, pPropMetadata->m_dwPropDefVal, bModified);

			if ( bModified )
				SetModified(TRUE);

			int nPrefixId = pPropMetadata->m_nPrefixStrId;

			if(i_oTreeNodeInfo.m_nPrefixId)
				nPrefixId = i_oTreeNodeInfo.m_nPrefixId;

			pLabel = pPropMetadata->m_pfnGetDisplayNameHelper
				(i_oTreeNodeInfo.m_hData, hValueProp, i_oTreeNodeInfo.m_nArrayPosition, nPrefixId);
		}

		strLabel = pLabel;

		// node type determination
		eNodeType = pPropMetadata->m_eTreeNodeType;
		if(NULL != pPropMetadata->m_pfncPropGetDisplayType)
			eNodeType = pPropMetadata->m_pfncPropGetDisplayType(
				i_oTreeNodeInfo.m_hData, i_oTreeNodeInfo.m_nPropId, pPropMetadata->m_eTreeNodeType);
	}
	else
		strLabel = i_oTreeNodeInfo.m_strTitle;

	CControlTreeItem *pItem	= i_oTreeNodeInfo.m_pExportedItem;
		
	if(NULL == pItem)
		pItem = new CControlTreeItem;

	CPPDescriptor* pDesc = new CPPDescriptor( i_oTreeNodeInfo.m_hData, hValueProp, 
			i_oTreeNodeInfo.m_nSpecialTypeId ? (i_oTreeNodeInfo.m_nSpecialTypeId | sdt_Label) : 0 );

	if(0 < i_oTreeNodeInfo.m_nArrayPosition)
	{
		pDesc->m_nArrayIndex = i_oTreeNodeInfo.m_nArrayPosition;
		pDesc->m_nSpecialType = sdt_ScrollListValue;

		if(	NULL != hValueProp)
		{
			int nCount = int(::PROP_Arr_Count( hValueProp ));
			if(nCount <= i_oTreeNodeInfo.m_nArrayPosition)
				pDesc->m_nSpecialType = sdt_ScrollListNewValue;
		}
	}

	pItem->SetData( (void *) pDesc, false );
	pItem->SetText( strLabel );
	pItem->SetLevel( i_oTreeNodeInfo.m_nItemLevel );
	pItem->SetCanBeRemoved( i_oTreeNodeInfo.m_bCanBeRemoved ? true : false );

	switch(eNodeType)
	{
	case MD::eString: 
		if(i_oTreeNodeInfo.m_bEditable)
			pItem->SetSimpleEditAlias();
		else
			pItem->SetSimpleStringAlias();
		break;
	case MD::eDDList:
		if(i_oTreeNodeInfo.m_bEditable)
			pItem->SetDropDownListComboAlias();
		else
			pItem->SetSimpleStringAlias();
		break;
	case MD::eStrDDList:
		if(!i_oTreeNodeInfo.m_bEditable)
			pItem->SetSimpleStringAlias();
		else
			pItem->SetDropDownComboAlias();
		break;
	case MD::eStrExtEdit:
		pItem->SetExclusiveExternalEditingAlias();
		break;
	case MD::eCheck:
		pItem->SetStrictlyDisabled( !i_oTreeNodeInfo.m_bEditable );
		pItem->SetCheckAlias();
		if(hValueProp)
		{
			AVP_dword dwValue = ::GetPropNumericValueAsDWord( hValueProp );
			pItem->Check( !!dwValue );
		}
		break;
	case MD::eRadio:
		pItem->SetStrictlyDisabled( !i_oTreeNodeInfo.m_bEditable );
		pItem->SetRadioAlias();
		if(hValueProp)
		{
			AVP_dword dwValue =  ::GetPropNumericValueAsDWord( hValueProp );
			pItem->Check( dwValue == i_oTreeNodeInfo.m_dwInitData );
		}
		break;
	case MD::eDate:
		if(!i_oTreeNodeInfo.m_bEditable)
			pItem->SetSimpleStringAlias();
		else
			pItem->SetDateAlias();
		break;
	case MD::eTime:
		if(!i_oTreeNodeInfo.m_bEditable)
			pItem->SetSimpleStringAlias();
		else
			pItem->SetTimeAlias();
		break;
	case MD::eDatetime:
		if(!i_oTreeNodeInfo.m_bEditable)
			pItem->SetSimpleStringAlias();
		else
			pItem->SetDateTimeAlias();
		break;

	default: ASSERT(FALSE);
	}

	i_oTreeNodeInfo.m_newItems.Add(pItem);
}

// ------------------------------------------------------------------------------------------
void CVisualEditorDlg::CreatePropertyNode
	(CCTItemsArray& o_newItems, int i_nFirstItemLevel, 
		HDATA i_hData, 	int i_nPropId, BOOL i_bEditable, BOOL i_bCanBeRemoved, int i_nSpecialTypeId, 
		DWORD i_dwInitData, int i_nPrefixId)
{
	ASSERT(0 != i_nPropId);

	STreeNodeInfo oTreeNodeInfo
		(	
			o_newItems,
			i_nFirstItemLevel,
			i_hData,
			i_nPropId,
			i_bEditable,
			i_bCanBeRemoved,
			i_nSpecialTypeId
		);

	oTreeNodeInfo.m_dwInitData = i_dwInitData;
	oTreeNodeInfo.m_nPrefixId = i_nPrefixId;

	CreateTreeNode(oTreeNodeInfo);
}

// ------------------------------------------------------------------------------------------
void CVisualEditorDlg::CreateStringNode(CCTItemsArray & o_newItems, int i_nFirstItemLevel, 
	HDATA i_hData, UINT i_StrResId, BOOL i_bEditable, BOOL i_bRemovable,  int i_nSpecialTypeId,
	MD::ETreeNodeType i_eTreeNodeType)
{
	CString strTitle;
	strTitle.LoadString(i_StrResId);
	CreateStringNode(o_newItems, i_nFirstItemLevel, i_hData, strTitle, 
		i_bEditable, i_bRemovable, i_nSpecialTypeId, i_eTreeNodeType);
}

// ------------------------------------------------------------------------------------------
void CVisualEditorDlg::CreateStringNode(CCTItemsArray & o_newItems, int i_nFirstItemLevel, 
	HDATA i_hData, LPCTSTR i_strTitle, BOOL i_bEditable, BOOL i_bRemovable,  int i_nSpecialTypeId,
	MD::ETreeNodeType i_eTreeNodeType)
{
	STreeNodeInfo oTreeNodeInfo
		(	
			o_newItems,
			i_nFirstItemLevel,
			i_hData,
			0,
			i_bEditable,
			i_bRemovable,
			i_nSpecialTypeId
		);

	oTreeNodeInfo.m_strTitle = i_strTitle;
	oTreeNodeInfo.m_eTreeNodeType = i_eTreeNodeType;

	CreateTreeNode(oTreeNodeInfo);
}

// ------------------------------------------------------------------------------------------
void CVisualEditorDlg::PrepareDisplayString
	(CTTMessageInfo* i_pInfo, HDATA i_hData, HPROP i_hProp, int i_nArrayPosition, BOOL i_bWithPrefix)
{
	if(NULL == i_hData || NULL == i_hProp)
		return;

	if(i_pInfo->m_Value.m_pItemText)
	{
		delete [] i_pInfo->m_Value.m_pItemText;
		i_pInfo->m_Value.m_pItemText = NULL;
	}

	CString strStr;
	if(GetPropDisplayString(strStr, i_hData, i_hProp, i_nArrayPosition, i_bWithPrefix))
	{
		i_pInfo->m_Value.m_pItemText = new TCHAR[strStr.GetLength() + 1];
		lstrcpy(i_pInfo->m_Value.m_pItemText, strStr);
	}
}


// ------------------------------------------------------------------------------------------
BOOL CVisualEditorDlg::GetPropDisplayString
	(CString& o_strStr, HDATA i_hData, HPROP i_hProp, int i_nArrayPosition, BOOL i_bWithPrefix)
{
	o_strStr.Empty();

	if(NULL == i_hData || NULL == i_hProp)
		return FALSE;

	AVP_dword dwPropId = ::PROP_Get_Id(i_hProp);
	MD::SPropMetadata* pPropMetadata = FindPropMetadata(dwPropId);

	if(NULL == pPropMetadata)	
		return FALSE;

	int nPrefixId = 0;
	if(i_bWithPrefix)
		nPrefixId = pPropMetadata->m_nPrefixStrId;

	TCHAR* strTmp = 
		pPropMetadata->m_pfnGetDisplayNameHelper(i_hData, i_hProp, i_nArrayPosition, nPrefixId);

	o_strStr = strTmp;

	if(strTmp)
		delete strTmp;

	return TRUE;
}

// ------------------------------------------------------------------------------------------
