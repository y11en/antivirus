//------------------------------------------------------------------------------------------------//

#include "stdafx.h"
#include "VisualEditorDlg.h"

//------------------------------------------------------------------------------------------------//

#define MAX_LARGE_LEN						50
#define ADJUST_METHOD_STRING				120
#define ADJUST_PROPERTY_STRING				120
#define ADJUST_ERRORCODE_STRING				120
#define ADJUST_CONSTANT_STRING				180

#define ADJUST_METHOD_RESULT_STRING			100
#define ADJUST_CONSTANT_VALUE				100
#define ADJUST_ERRCODE_RESULT_STRING		100

#define ADJUST_CONSTANT_COMMENT				100

// ---
static int GetTextSize( LPCTSTR pText, HDC hDC ) {
  return LOWORD(::GetTabbedTextExtent(hDC, pText, lstrlen(pText), 0, NULL));
}

// ---
void AjustDisplayString( CPathStr &pDispStr, int nAjustLen ) 
{
	CDC *dc = gpMainDlg->m_IDescriptions.GetDC();
	dc->SelectObject( gpMainDlg->m_IDescriptions.GetFont() );

	int nBaseLen = ::GetTextSize( pDispStr, *dc );
	int nSpaceLen = ::GetTextSize( __TEXT(" "), *dc );

	while (	nBaseLen < nAjustLen ) {
		pDispStr += " ";
		nBaseLen += nSpaceLen;
	}

	gpMainDlg->m_IDescriptions.ReleaseDC( dc );
}

char *	GetConstantNameDisplayString( HDATA hData, HPROP hProp, int, int i_nPrefixId)
{
	return ::GetConstantNameDisplayString(hData, hProp, AVP_dword(0), i_nPrefixId ? FALSE : TRUE);
}

// ---
void GetConstantNameDisplayString( CTTMessageInfo *info, HDATA hData, HPROP hProp, AVP_dword dwWorkMode ) {
	delete [] info->m_Value.m_pItemText;
	info->m_Value.m_pItemText = GetConstantNameDisplayString( hData, hProp, dwWorkMode );
}

// ---
char *GetConstantNameDisplayString( HDATA hData, HPROP hProp, AVP_dword dwWorkMode, BOOL i_bForEditing ) 
{
	CAPointer<TCHAR>	pValue = ::GetPropValueAsString( hProp, NULL );
	CPathStr pDispStr( _MAX_PATH );
	
	if ( ::IsSeparator(hData) ) 
		::GetSeparatorDisplayString( pDispStr, pValue );
	else 
	{
		pDispStr = pValue;

		if(FALSE == i_bForEditing)
		{
			::AjustDisplayString( pDispStr, ADJUST_CONSTANT_STRING );
			
			pDispStr += "<";
			pValue = ::GetDataPropertyString( hData, VE_PID_IFC_BASETYPENAME );
			pDispStr += pValue;
			pDispStr += ">";
			
			::AjustDisplayString( pDispStr, ADJUST_CONSTANT_STRING + ADJUST_CONSTANT_VALUE );
			
			pDispStr += "<";
			hProp = ::DATA_Find_Prop( hData, NULL, VE_PID_IFC_VALUE );
			if ( hProp ) {
				pValue = ::GetPropValueAsString( hProp, "%#x" );
				pDispStr += pValue;
			}
			pDispStr += ">";
			
			hProp = ::DATA_Find_Prop( hData, NULL, VE_PID_SHORTCOMMENT );
			if ( hProp ) {
				::AjustDisplayString( pDispStr, ADJUST_CONSTANT_STRING + ADJUST_CONSTANT_VALUE  + ADJUST_CONSTANT_COMMENT);
				
				pDispStr += "<";
				pValue = ::GetPropValueAsString( hProp, "%#x" );
				pDispStr += pValue;
				pDispStr += ">";
			}
		}
	}

	return pDispStr.Relinquish();
}

// ---
char* GetErrorCodeNameDisplayString(HDATA hData, HPROP hProp, int, int i_nPrefixId  ) 
{
	return GetErrorCodeNameDisplayString(hData, hProp, i_nPrefixId ? FALSE : TRUE);
}

void GetErrorCodeNameDisplayString( CTTMessageInfo *info, HDATA hData, HPROP hProp ) {
	delete [] info->m_Value.m_pItemText;
	info->m_Value.m_pItemText = GetErrorCodeNameDisplayString( hData, hProp );
}

// ---
char* GetErrorCodeNameDisplayString( HDATA hData, HPROP hProp, BOOL i_bForEditing ) {
	CAPointer<TCHAR>	pValue = ::GetPropValueAsString( hProp, NULL );
	CPathStr pDispStr( _MAX_PATH );

	if ( ::IsSeparator(hData) ) 
		::GetSeparatorDisplayString( pDispStr, pValue );
	else {
		pDispStr += pValue;

		if(FALSE == i_bForEditing)
		{
			::AjustDisplayString( pDispStr, ADJUST_ERRORCODE_STRING );
			
			AVP_dword dwIsWarning = FALSE;
			HPROP hValueProp;
			if ( hValueProp = ::DATA_Find_Prop(hData, NULL, VE_PID_IFE_ISWARNING) ) {
				dwIsWarning = ::GetPropNumericValueAsDWord( hValueProp );
			}
			
//			pDispStr += "<";
//			hProp = ::DATA_Find_Prop( hData, NULL, VE_PID_IFE_ERRORCODEVALUE );
//			if ( hProp ) {
//				AVP_dword dwValue = ::GetPropNumericValueAsDWord( hProp );
//				TCHAR	pBuff[100];
//				if ( dwIsWarning )
//					wsprintf( pBuff, "errUSER_DEFINED_WARN + %#x", dwValue );
//				else
//					wsprintf( pBuff, "errUSER_DEFINED + %#x", dwValue );
//				pDispStr += pBuff;
//			}
//			pDispStr += ">";
//			
//			::AjustDisplayString( pDispStr, ADJUST_METHOD_STRING + ADJUST_ERRCODE_RESULT_STRING );
//			
//			pDispStr += " <";
//			if ( hProp ) {
//				AVP_dword dwValue = ::GetPropNumericValueAsDWord( hProp ) + (dwIsWarning ? errUSER_DEFINED_WARN : errUSER_DEFINED);
//				TCHAR	pBuff[100];
//				wsprintf( pBuff, "%#08x", dwValue );
//				pDispStr += pBuff;
//			}
//			pDispStr += ">";
		}
	}
	return pDispStr.Relinquish();
}

// ---
char *GetMethodNameDisplayString( HDATA hData ) {
  CMethodInfo method( hData, ifPubMethod, false );
  return GetMethodNameDisplayString( method );
}

// ---
void GetMethodNameDisplayString( CTTMessageInfo *info, HDATA hData ) {
  CMethodInfo method( hData, ifPubMethod, false );
  delete [] info->m_Value.m_pItemText;
	info->m_Value.m_pItemText = GetMethodNameDisplayString( method );
}

// ---
char *GetMethodNameDisplayString( CMethodInfo& method, BOOL i_bForEditing ) 
{
	CAPointer<TCHAR>	pValue = ::GetPropValueAsString( method.FindProp(VE_PID_IFM_METHODNAME), NULL );
	CPathStr pDispStr( _MAX_PATH );

	if ( method.IsSeparator() ) 
		::GetSeparatorDisplayString( pDispStr, pValue );
	else 
	{
		pDispStr += pValue;

		if(FALSE == i_bForEditing)
		{
			::AjustDisplayString( pDispStr, ADJUST_METHOD_STRING );

			pDispStr += " <";

			pValue = ::GetDataPropertyString( method.Data(), VE_PID_IFM_RESULTTYPENAME );
			pDispStr += pValue;
			pDispStr += ">";

			::AjustDisplayString( pDispStr, ADJUST_METHOD_STRING + ADJUST_METHOD_RESULT_STRING );
			
			pDispStr += "<";
			
			int nIndex = 0;
			HDATA hCurrData = ::DATA_Get_First( method.Data(), NULL );
			while ( hCurrData ) {
				if ( nIndex )
					pDispStr += ", ";
				
				nIndex++;
				
				CAPointer<TCHAR>	pParamName = ::GetDataPropertyString( hCurrData, VE_PID_IFMP_PARAMETERNAME );
				if ( !::IsEllipsis(pParamName) ) {
					AVP_dword dwPointerValue = FALSE;
					AVP_dword dwNativePointerValue = FALSE;
					AVP_dword dwConstValue = FALSE;
					HPROP hProp = ::DATA_Find_Prop( hCurrData, NULL, VE_PID_IFMP_ISPOINTER );
					if ( hProp )
						dwPointerValue = ::GetPropNumericValueAsDWord( hProp );
					if ( !dwPointerValue ) {
						hProp = ::DATA_Find_Prop( hCurrData, NULL, VE_PID_IFMP_TYPE );
						dwNativePointerValue = ::GetPropNumericValueAsDWord( hProp );
						dwNativePointerValue = TYPE_IS_POINTER(dwNativePointerValue);
					}
					
					if ( dwPointerValue || dwNativePointerValue ) {
						hProp = ::DATA_Find_Prop( hCurrData, NULL, VE_PID_IFMP_ISCONST );
						if ( hProp )
							dwConstValue = ::GetPropNumericValueAsDWord( hProp );
					}
					
					if ( dwConstValue )
						pDispStr += "const ";
					
					pValue = ::GetDataPropertyString( hCurrData, VE_PID_IFMP_TYPENAME );
					pDispStr += pValue;
					pDispStr += " ";
					
					if ( dwPointerValue ) {
						hProp = ::DATA_Find_Prop( hCurrData, NULL, VE_PID_IFMP_IS_DOUBLE_POINTER );
						if ( hProp && ::GetPropNumericValueAsDWord(hProp) )
							pDispStr += "** ";
						else
							pDispStr += "* ";
					}
				}
				pDispStr += pParamName;
				
				hCurrData = ::DATA_Get_Next( hCurrData, NULL );
			}
			
			pDispStr += ">";
		}
	}

	return pDispStr.Relinquish();
}

// ---
void GetPropertyNameDisplayString( CTTMessageInfo *info, HDATA hData, HPROP hProp, AVP_dword dwWorkMode ) 
{
	delete [] info->m_Value.m_pItemText;
	info->m_Value.m_pItemText = GetPropertyNameDisplayString( hData, hProp, dwWorkMode );
}

// ---
char *GetPropertyNameDisplayString( HDATA hData, HPROP hProp, AVP_dword dwWorkMode, BOOL i_bForEditing ) 
{
	CAPointer<TCHAR>	pValue = ::GetPropValueAsString( hProp, NULL );
	CPathStr pDispStr( _MAX_PATH );
	
	if ( ::IsSeparator(hData) ) 
		::GetSeparatorDisplayString( pDispStr, pValue );
	else {
		pDispStr = pValue;

		if(FALSE == i_bForEditing)
		{
			::AjustDisplayString( pDispStr, ADJUST_PROPERTY_STRING );
			
			pDispStr += "<";
			
			pValue = ::GetDataPropertyString( hData, VE_PID_IFP_TYPENAME );
			pDispStr += pValue;
			
			if ( dwWorkMode == wmt_IImplementation ) {
				AVP_dword dwRequiredValue = FALSE;
				AVP_dword dwWritableDuringInitValue = FALSE;
				HPROP hProp = ::DATA_Find_Prop( hData, NULL, VE_PID_IFPLS_REQUIRED );
				if ( hProp )
					dwRequiredValue = ::GetPropNumericValueAsDWord( hProp );
				
				hProp = ::DATA_Find_Prop( hData, NULL, VE_PID_IFPLS_WRITABLEDURINGINIT );
				if ( hProp )
					dwWritableDuringInitValue = ::GetPropNumericValueAsDWord( hProp );
				
				if ( dwRequiredValue ) {
					pDispStr += ", R";
				}
				
				if ( dwWritableDuringInitValue ) {
					pDispStr += ", WI";
				}
			}
			
			pDispStr += ">";
		}
	}
	return pDispStr.Relinquish();
}

// ---
char *CreateDisplayLabelSTR( DWORD dwPrefixID, LPCTSTR lpszStr ) 
{
	return CreateDisplayLabelSTRExt(dwPrefixID, lpszStr );
}

// ---
char *CreateDisplayLabelSTRExt( DWORD dwPrefixID, LPCTSTR lpszStr ) 
{
	int     plen, mlen;
	bool    ellipses = false;
	CString rcPrefix;
	BOOL bValidId = rcPrefix.LoadString( dwPrefixID );
	
	plen = rcPrefix.GetLength();
	if ( lpszStr ) {
		const char* crlf = strstr( lpszStr, "\r\n" );
		if ( crlf )
			mlen = crlf - lpszStr;
		else
			mlen = ::lstrlen(lpszStr);
		if ( mlen > MAX_LARGE_LEN ) 
		{
			mlen = MAX_LARGE_LEN;
			ellipses = true;
		}
	}
	else
		mlen = 0;
	
	char *pLabel = new char[plen+mlen+3*(ellipses+1)];
	memcpy( pLabel, (const char*)rcPrefix, plen );

	if(bValidId)
		*(pLabel+plen++) = '<';

	if ( mlen ) {
		memcpy( pLabel+plen, lpszStr, mlen );
		plen += mlen;
		if ( ellipses ) {
			memcpy( pLabel+plen, "...", 3 );
			plen += 3;
		}
	}

	if(bValidId)
		*(pLabel+plen++) = '>';

	*(pLabel+plen++) = 0;
	
	return pLabel;
}

// ---
char *CreateDisplayLabelINT( DWORD dwPrefixID, int nValue, BOOL i_bHex) 
{
	CString rcPrefix;
	BOOL bValidId = rcPrefix.LoadString( dwPrefixID );

	TCHAR *pLabel = new TCHAR[_MAX_PATH];

	if(i_bHex)
	{
		if(bValidId)	wsprintf( pLabel, "%s<%#x>", rcPrefix, nValue );
		else			wsprintf( pLabel, "%#x", nValue );
	}
	else
	{
		if(bValidId)	wsprintf( pLabel, "%s<%d>", rcPrefix, nValue );
		else			wsprintf( pLabel, "%d", nValue );
	}

	return pLabel;
}

// ---
char *CreateLargeCommentLabel( HDATA hData, AVP_dword dwCommentID ) {
	TCHAR *pMnemonicIDLabel = new TCHAR[_MAX_PATH];
	memset( pMnemonicIDLabel, 0, _MAX_PATH );

	CAPointer<TCHAR> pDisplayString = ::GetDataPropertyString( hData, dwCommentID );

	if ( pDisplayString && *pDisplayString ) {
		lstrcpyn( pMnemonicIDLabel, pDisplayString, MAX_LARGE_LEN );
		int nTermInd = strcspn( pMnemonicIDLabel, "\r\n" );
		bool bChanged = nTermInd != lstrlen( pMnemonicIDLabel );
		*(pMnemonicIDLabel + nTermInd) = 0;

		if ( (lstrlen(pDisplayString) - (bChanged ? 2 : 0)) > lstrlen(pMnemonicIDLabel) )
			lstrcat( pMnemonicIDLabel, "..." );
	}

	return pMnemonicIDLabel;
}

// -------------------------------------------------------------------------------------------
AVP_dword  FindNewPropertyID( HDATA hData, int i_nIdentProp, BOOL i_bIsPrototype) 
{
	AVP_dword dwResID;
	
	if(i_bIsPrototype)
		dwResID = pUSER_MIN_PROP_ID - 1;
	else
		dwResID = (pUSER_MIN_PROP_ID << 1) - 1;

	HPROP hProp = ::DATA_Find_Prop( hData, NULL, i_nIdentProp );
	if(hProp)
		return ::GetPropNumericValueAsDWord( hProp );
			
	HDATA hParentData = ::DATA_Get_Dad( hData, NULL );
	if ( hParentData ) 
	{
		CSArray<AVP_dword> rcIDArray;
		HDATA hCurrData = ::DATA_Get_First( hParentData, NULL );
		while ( hCurrData ) 
		{
			HPROP hIDProp;
			if ( hIDProp = ::DATA_Find_Prop( hCurrData, NULL, i_nIdentProp) ) 
			{
				AVP_dword dwPropID = ::GetPropNumericValueAsDWord( hIDProp );
				if ( dwResID < dwPropID )
					dwResID = dwPropID;
			}
			hCurrData = ::DATA_Get_Next( hCurrData, NULL );
		}
		dwResID++;
	}
	else {
		AVP_dword nID = ::DATA_Get_Id( hData, NULL );
		dwResID = GET_AVP_PID_ID(nID);
	}
	return dwResID;
}

// ------------------------------------------------------------------------------------------
static void CheckMnemonicInvalidCharacter( char *pString) 
{
	for(char *pChar = pString; *pChar; pChar++)
		if ( !((*pChar >= 'a' && *pChar <= 'z') || 
			 (*pChar >= 'A' && *pChar <= 'Z') || 
			 (*pChar >= '0' && *pChar <= '9')) )
			*pChar = '_';
}

static char* CreateDataNameWithPrefix(HDATA hData, UINT i_uiPropId, UINT i_nPrefId, BOOL i_bUpper)
{
	CString strPrefix;
	strPrefix.LoadString( i_nPrefId );

	char *pBuf = new char[_MAX_PATH];
	lstrcpy(pBuf, strPrefix);

	char* strName = pBuf + lstrlen(pBuf);

	CAPointer<char> pPropString = ::GetDataPropertyString( hData, i_uiPropId );

	lstrcat( strName, pPropString );

	if ( i_bUpper )	
		_strupr( strName );
	else
		_strlwr( strName );

	CheckMnemonicInvalidCharacter(strName);

	return pBuf;
}

char *CreatePluginMnemonicIDLabel( HDATA hData ) 
{
	return CreateDataNameWithPrefix(hData, VE_PID_PL_PLUGINNAME, IDS_PLMNEMONIC_ID, TRUE);
}

char *CreateMnemonicIDLabel( HDATA hData ) 
{
	return CreateDataNameWithPrefix(hData, VE_PID_IF_INTERFACENAME, IDS_MNEMONIC_ID, TRUE);
}

char *CreateIncludeFileLabel( HDATA hData ) 
{
	return CreateDataNameWithPrefix(hData, VE_PID_IF_INTERFACENAME, IDS_INCLUDEFILE, FALSE);
}

char *CreateOutputFileLabel( HDATA hData ) 
{
	return CreateDataNameWithPrefix(hData, VE_PID_IF_INTERFACENAME, IDS_OUPUTFILE, FALSE);
}

char *CreateObjectTypeLabel( HDATA hData ) 
{
	return CreateDataNameWithPrefix(hData, VE_PID_IF_INTERFACENAME, IDS_OBJECTTYPE, TRUE);
}

// -------------------------------------------------------------------------------------------
