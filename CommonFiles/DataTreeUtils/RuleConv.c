////////////////////////////////////////////////////////////////////
//
//  RuleConv.c
//  Convert data tree to string array
//  AVP generic purpose stuff
//  Victor Matiouchenkov [victor@avp.ru], Kaspersky Labs. 1999
//  Copyright (c) Kaspersky Labs
//
////////////////////////////////////////////////////////////////////

#include <stdio.h>
#include <string.h>
#include <tchar.h>
#include <datatreeutils/dtutils.h>
#include <avpprpid.h>
#include <avpnstid.h>
#include <AVPPort.h>
#include <WASCRes.rh>
#include <Stuff/Enc2Text.h>

extern void* (* DTUAllocator)(AVP_size_t);
extern void  (* DTULiberator)(void*);

#if defined(_UNICODE) || defined(UNICODE)
#define DTU_TCSCLEN(p) (p ? _tcsclen(p) : 0)
#else
#define DTU_TCSCLEN(p) (p ? strlen(p) : 0)
#endif

#define SIZEOF_TCHAR sizeof(TCHAR)

static DTUT_LocalizeString g_pfLocCallBack = 0;

#define CNV_PID_ROOT					MAKE_AVP_PID(1000,AVP_AID_SYSTEM,avpt_str,1)
#define PASSWORD_STR_LEN      10

static AVP_bool g_bINILook = FALSE;
static TCHAR *g_pSelectedText = 0;
static TCHAR *g_pYesText = 0;
static TCHAR *g_pNoText = 0;

#define TEXT_PID (!g_bINILook ? PP_PID_DISPLAY_STR : PP_PID_MNEMONICID)

#define BOBJECT _T("\"")
#define EOBJECT _T("\"")
#define BGROUP _T("[")
#define EGROUP _T("]")
#define VALUEB (!g_bINILook ? _T("  ") : _T(""))
#define VALUED (!g_bINILook ? _T(" = ")	: _T("="))
#define BVALUEY _T("Yes")
#define BVALUEN _T("No")
#define BSECT _T("[")
#define ESECT _T("]")
#define COMMENT _T(";")
#define PREFIX _T("; ")
#define ENABLETEXT _T("Enable processing")

#define OBJECT_EXTRA (DTU_TCSCLEN(BOBJECT) + DTU_TCSCLEN(EOBJECT) + 1)
#define GROUP_EXTRA (DTU_TCSCLEN(BGROUP) + DTU_TCSCLEN(EGROUP) + 1)
#define VALUE_EXTRA (DTU_TCSCLEN(VALUEB) + DTU_TCSCLEN(VALUED) + 1)
#define BVALUE_EXTRA (DTU_TCSCLEN(VALUED) + (g_pYesText ? DTU_TCSCLEN(g_pYesText) : DTU_TCSCLEN(BVALUEY)) + (g_pNoText ? DTU_TCSCLEN(g_pNoText) : DTU_TCSCLEN(BVALUEN)) + 1)
#define PREFIX_EXTRA (DTU_TCSCLEN(PREFIX) + 1)
#define SECT_EXTRA (DTU_TCSCLEN(BSECT) + DTU_TCSCLEN(ESECT) + 1)

#define CACHE_PID(a)  MAKE_AVP_PID(a,AVP_AID_SYSTEM,avpt_str,0)

static HDATA g_hCacheRoot = 0;

// ---
static TCHAR *CacheGet( AVP_dword dwStrID ) {
	TCHAR *pResult = 0;
	if ( g_hCacheRoot ) {
		HDATA hFoundData;
		MAKE_ADDR1( nAddr, CACHE_PID(dwStrID) );
		hFoundData = DATA_Find( g_hCacheRoot, nAddr );
		if ( hFoundData ) {
			HPROP hProp = DATA_Find_Prop( hFoundData, 0, 0 );
			if ( hProp ) 
				pResult = DTUT_GetPropValueAsString( hProp, NULL );
		}
	}
	return pResult;
}

// ---
static void CacheSet( AVP_dword dwStrID, TCHAR *pStr ) {
	if ( g_hCacheRoot ) {
		HDATA hFoundData;
		MAKE_ADDR1( nAddr, CACHE_PID(dwStrID) );
		hFoundData = DATA_Find( g_hCacheRoot, nAddr );
		if ( hFoundData ) {
			HPROP hProp = DATA_Find_Prop( hFoundData, 0, 0 );
			if ( hProp ) 
				PROP_Set_Val( hProp, (AVP_size_t)pStr, 0 );
		}
		else
			DATA_Add( g_hCacheRoot, 0, CACHE_PID(dwStrID), (AVP_size_t)pStr, 0 );
	}
}


// ---
static void QueryLocalisedString( TCHAR **pString, UINT nStrID ) {
	if ( g_pfLocCallBack ) 
		g_pfLocCallBack( nStrID, pString );
}


// ---
static void QueryLocalisedStandardString() {
	if ( g_hCacheRoot ) {
		DATA_Remove( g_hCacheRoot, 0 );
		g_hCacheRoot = 0;
	}

	if ( !g_pSelectedText ) {
		g_pSelectedText = DTUAllocator( SIZEOF_TCHAR * (DTU_TCSCLEN(ENABLETEXT) + 1) );
		_tcscpy( g_pSelectedText, ENABLETEXT );
		QueryLocalisedString( &g_pSelectedText, IDS_WAS_SELECTED_FOR_PROCESSING );
	}

	if ( !g_pYesText ) {
		g_pYesText = DTUAllocator( SIZEOF_TCHAR * (DTU_TCSCLEN(BVALUEY) + 1) );
		_tcscpy( g_pYesText, BVALUEY );
		QueryLocalisedString( &g_pYesText, IDS_WAS_YES );
	}

	if ( !g_pNoText ) {
		g_pNoText = DTUAllocator( SIZEOF_TCHAR * (DTU_TCSCLEN(BVALUEN) + 1) );
		_tcscpy( g_pNoText, BVALUEN );
		QueryLocalisedString( &g_pNoText, IDS_WAS_NO );
	}
}

// ---
static void FreeLocalisedStandardString() {
	if ( g_pSelectedText ) {
		DTULiberator( g_pSelectedText );
		g_pSelectedText = 0;
	}
	if ( g_pYesText ) {
		DTULiberator( g_pYesText );
		g_pYesText = 0;
	}
	if ( g_pNoText ) {
		DTULiberator( g_pNoText );
		g_pNoText = 0;
	}

	if ( g_hCacheRoot ) {
		DATA_Remove( g_hCacheRoot, 0 );
		g_hCacheRoot = 0;
	}
}


// ---
static void QueryAndChangeSingleLocalised( HDATA hData, UINT nResID, UINT nPropID ) {
	HPROP hResIDProp = DATA_Find_Prop( hData, NULL, nResID );
	if ( hResIDProp ) {
		HPROP hValueProp = DATA_Find_Prop( hData, NULL, nPropID );
		if ( hValueProp ) {
			AVP_dword nPID   = PROP_Get_Id( hValueProp );
			AVP_dword nType  = GET_AVP_PID_TYPE( nPID );
			AVP_bool  bArray = GET_AVP_PID_ARR( nPID );

			if ( nType == PP_TYPE_STRING && !bArray ) {
				AVP_dword	nStrID = DTUT_GetPropNumericValueAsDWord( hResIDProp );
				TCHAR *pText = CacheGet( nStrID );
				if ( !pText ) {
					pText = DTUT_GetPropValueAsString( hValueProp, NULL );
					QueryLocalisedString( &pText, nStrID );
					CacheSet( nStrID, pText );
				}
				DTUT_SetPropValue( hValueProp, pText, NULL );
				DTULiberator( pText );
			}
		}
		DATA_Remove_Prop_H( hData, NULL, hResIDProp );
	}
}

// ---
static void QueryAndChangeArrayLocalised( HDATA hData, UINT nResID, UINT nPropID ) {
	HPROP hResIDProp = DATA_Find_Prop( hData, NULL, nResID );
	if ( hResIDProp ) {
		HPROP hValueProp = DATA_Find_Prop( hData, NULL, nPropID );
		if ( hValueProp ) {
			AVP_dword nPID   = PROP_Get_Id( hValueProp );
			AVP_dword nType  = GET_AVP_PID_TYPE( nPID );
			AVP_bool  bArray = GET_AVP_PID_ARR( nPID );

			if ( nType == PP_TYPE_STRING && bArray ) {
				AVP_dword nCount = PROP_Arr_Count( hValueProp );
				AVP_dword i;

				for ( i=0; i<nCount; i++ ) {
					AVP_dword	nStrID = DTUT_GetPropArrayNumericValueAsDWord( hResIDProp, i );
					TCHAR *pText = CacheGet( nStrID );
					if ( !pText ) {
						pText = DTUT_GetPropValueAsString( hValueProp, NULL );
						QueryLocalisedString( &pText, nStrID );
						CacheSet( nStrID, pText );
					}
					DTUT_SetPropArrayValue( hValueProp, i, pText, NULL );
					DTULiberator( pText );
				}
			}
		}
		DATA_Remove_Prop_H( hData, NULL, hResIDProp );
	}
}


// ---
static void QueryAndChangeLocalisedStrings( HDATA hData ) {
	if ( g_pfLocCallBack ) {
		AVP_bool  bShouldLocalize = TRUE;
		HPROP hShouldLocalizeProp;

		QueryAndChangeSingleLocalised( hData, PP_PID_DISPSTR_RESID, PP_PID_DISPLAY_STR );
		QueryAndChangeArrayLocalised( hData, PP_PID_LISTCONT_RESID, PP_PID_LISTCONTENTS );
		QueryAndChangeArrayLocalised( hData, PP_PID_COMBOCONT_RESID, PP_PID_COMBOCONTENTS );

		hShouldLocalizeProp = DATA_Find_Prop( hData, NULL, PP_PID_SHOULDLOCALIZE );
		if ( hShouldLocalizeProp ) 
			PROP_Get_Val( hShouldLocalizeProp, &bShouldLocalize, sizeof(bShouldLocalize) );
		if ( bShouldLocalize )
			QueryAndChangeSingleLocalised( hData, PP_PID_STRVAL_RESID, 0 );
	}
}


// ---
static void FormatAndWriteStringDivider( HPROP hResultProp ) {
	DTUT_SetPropArrayValue( hResultProp, PROP_ARR_LAST, _T(""), 0 );
}


// ---
static void FormatAndWriteObject( const TCHAR *pValue, HPROP hResultProp ) {
	TCHAR *pString = DTUAllocator( SIZEOF_TCHAR * (DTU_TCSCLEN(pValue) + OBJECT_EXTRA) );
	if ( pString ) {
		*pString = 0;
		_tcscat( pString, BOBJECT );
		_tcscat( pString, pValue );
		_tcscat( pString, EOBJECT );
		DTUT_SetPropArrayValue( hResultProp, PROP_ARR_LAST, pString, 0 );
		DTULiberator( pString );
	}
}

// ---
static void FormatAndWriteGroup( const TCHAR *pValue, HPROP hResultProp ) {
/*
	TCHAR *pString = DTUAllocator( SIZEOF_TCHAR * (DTU_TCSCLEN(pValue) + GROUP_EXTRA) );
	if ( pString ) {
		*pString = 0;
		_tcscat( pString, BGROUP );
		_tcscat( pString, pValue );
		_tcscat( pString, EGROUP );
		DTUT_SetPropArrayValue( hResultProp, PROP_ARR_LAST, pString, 0 );
		DTULiberator( pString );
	}
*/
}

// ---
static void FormatAndWriteValue( const TCHAR *pName, const TCHAR *pValue, HPROP hResultProp ) {
	TCHAR *pString = DTUAllocator( SIZEOF_TCHAR * (DTU_TCSCLEN(pName) + (pValue ? DTU_TCSCLEN(pValue) : 0) + VALUE_EXTRA) );
	if ( pString ) {
		*pString = 0;
		_tcscat( pString, VALUEB );
		_tcscat( pString, pName );
		if ( pValue ) {
			_tcscat( pString, VALUED );
			_tcscat( pString, pValue );
		}
		DTUT_SetPropArrayValue( hResultProp, PROP_ARR_LAST, pString, 0 );
		DTULiberator( pString );
	}
}

// ---
static void FormatAndWriteBoolValue( const TCHAR *pName, AVP_bool bValue, HPROP hResultProp ) {
	TCHAR *pString = DTUAllocator( SIZEOF_TCHAR * (DTU_TCSCLEN(pName) + BVALUE_EXTRA) );
	if ( pString ) {
		*pString = 0;
		_tcscat( pString, VALUEB );
		_tcscat( pString, pName );
		_tcscat( pString, VALUED );
		_tcscat( pString, bValue ? (g_pYesText ? g_pYesText : BVALUEY) : (g_pNoText ? g_pNoText : BVALUEN) );
		DTUT_SetPropArrayValue( hResultProp, PROP_ARR_LAST, pString, 0 );
		DTULiberator( pString );
	}
}

// ---
static void FormatAndWriteSection( const TCHAR *pValue, HPROP hResultProp ) {
	TCHAR *pString;
	DTUT_SetPropArrayValue( hResultProp, PROP_ARR_LAST, _T(""), 0 );
	pString = DTUAllocator( SIZEOF_TCHAR * (DTU_TCSCLEN(pValue) + SECT_EXTRA) );
	if ( pString ) {
		*pString = 0;
		_tcscat( pString, BSECT );
		_tcscat( pString, pValue );
		_tcscat( pString, ESECT );
		DTUT_SetPropArrayValue( hResultProp, PROP_ARR_LAST, pString, 0 );
		DTULiberator( pString );
	}
}

// ---
static TCHAR *EncryptPassword( TCHAR *pValue ) {
	AVP_size_t nSLength = DTU_TCSCLEN( pValue ) + 1;
	AVP_size_t nDLength = E2T_LENGTH(nSLength) + 1;
	TCHAR *pEncrypted = DTUAllocator( SIZEOF_TCHAR * nDLength );
	*pEncrypted = 0;
	nDLength = EncryptToText( pValue, (AVP_uint)nSLength, pEncrypted, (AVP_uint)nDLength );
	pEncrypted[nDLength] = 0;
	//DecryptToText( pEncrypted, nDLength, pValue, nSLength );
	return pEncrypted;
}


// ---
static AVP_bool ConvertNodeContents( HDATA hData, HPROP hResultProp, AVP_dword *pnIndexInParent ) {
	AVP_bool bResult = TRUE;
	TCHAR *pItemText = 0;
	TCHAR *pItemValue = 0;
	TCHAR *pItemFormat = 0;
	AVP_byte  nTEDType = PP_TED_NONEDITABLE;
	AVP_bool  bPassword = FALSE;
	HPROP     hProp = NULL;

	HPROP hOutFormatProp = DATA_Find_Prop( hData, NULL, PP_PID_FORMAT_STR_OUT ); 
	if ( hOutFormatProp )	
		pItemFormat = DTUT_GetPropValueAsString( hOutFormatProp, NULL );

	if ( hProp = DATA_Find_Prop( hData, NULL, PP_PID_EDIT_TYPE ) ) 
		PROP_Get_Val( hProp, &nTEDType, sizeof(nTEDType) );

	switch ( nTEDType ) {
		case PP_TED_ORDINARY :
		case PP_TED_UPDOWN :
		case PP_TED_EDITABLECOMBO :
		case PP_TED_NONEDITABLECOMBO : 
		case PP_TED_EXTERNAL :
		case PP_TED_EXCLUSIVEEXTERNAL :
		case PP_TED_DATEORDINARY :
		case PP_TED_TIMEORDINARY :
		case PP_TED_PASSWORD :
		case PP_TED_IPADDRESS :
			pItemText = DTUT_GetDataPropertyString( hData, TEXT_PID );
			pItemValue = DTUT_GetPropValueAsStringCC( DATA_Find_Prop(hData, 0, 0), pItemFormat );

			if ( hProp = DATA_Find_Prop( hData, NULL, PP_PID_VALUEASPASSWORD ) ) 
				PROP_Get_Val( hProp, &bPassword, sizeof(bPassword) );

			if ( bPassword ) {
				if ( !g_bINILook ) {
					DTULiberator( pItemValue );
					pItemValue = DTUAllocator( SIZEOF_TCHAR * (PASSWORD_STR_LEN + 1) );
					_tcsnset( pItemValue, L'*', PASSWORD_STR_LEN );
					*_tcsninc(pItemValue, PASSWORD_STR_LEN) = 0;
	//				memset( (void *)(LPCSTR)pItemValue, L'*', DTU_TCSCLEN(pItemValue) );
				}
				else {
					TCHAR *pNewItemValue = EncryptPassword( pItemValue );
					DTULiberator( pItemValue );
					pItemValue = pNewItemValue;
				}
			}

			FormatAndWriteValue( pItemText, pItemValue, hResultProp );
			break;
		case PP_TED_NONEDITABLE  : {
			AVP_dword nID		 = DATA_Get_Id( hData, NULL );
			AVP_dword nType  = GET_AVP_PID_TYPE(nID);
			switch ( nType ) {
				case PP_TYPE_NOTHING     :
				case PP_TYPE_BINARY      : 
				default									 :
					pItemText = DTUT_GetDataPropertyString( hData, TEXT_PID );
					FormatAndWriteGroup( pItemText, hResultProp );
					break;
				case PP_TYPE_BOOL        : {
					AVP_byte  nSubType;
					if ( hProp = DATA_Find_Prop( hData, NULL, PP_PID_SUBTYPE ) ) {
						AVP_bool bValue;
						PROP_Get_Val( hProp, &nSubType, sizeof(nSubType) );
						switch ( nSubType ) {
							case PP_SYBTYPE_CHECKBTN :
								pItemText = DTUT_GetDataPropertyString( hData, TEXT_PID );
								bValue = !!DTUT_GetPropNumericValueAsDWord( DATA_Find_Prop(hData, 0, 0) );
								FormatAndWriteBoolValue( pItemText, bValue, hResultProp );
								bResult = bValue;
								break;
							case PP_SYBTYPE_RADIOBTN : {
									HDATA hParentData = DATA_Get_Dad( hData, NULL );
									AVP_dword nID		  = DATA_Get_Id( hParentData, NULL );
									AVP_dword nType		= GET_AVP_PID_TYPE(nID);
									if ( nType == PP_TYPE_RADIOGROUP ) {
										HPROP hProp = DATA_Find_Prop( hParentData, 0, 0 );
										AVP_dword nValue = DTUT_GetPropNumericValueAsDWord( hProp );
										if ( nValue == *pnIndexInParent ) {
											if ( !g_bINILook ) {
												HDATA hParentParentData = DATA_Get_Dad( hParentData, NULL );
												if ( hParentParentData )
													hParentData = hParentParentData;
												pItemText = DTUT_GetDataPropertyString( hParentData, TEXT_PID );
												pItemValue = DTUT_GetDataPropertyString( hData, PP_PID_DISPLAY_STR );
											}
											else {
												pItemText = DTUT_GetDataPropertyString( hParentData, TEXT_PID );
												pItemValue = DTUAllocator( SIZEOF_TCHAR * 20 );
												_stprintf( pItemValue, _T("%d"), nValue );
											}
											FormatAndWriteValue( pItemText, pItemValue, hResultProp );
										}
										else 
											bResult = FALSE;
										(*pnIndexInParent)++;
									}
									else {
										pItemText = DTUT_GetDataPropertyString( hData, TEXT_PID );
										bValue = !!DTUT_GetPropNumericValueAsDWord( DATA_Find_Prop(hData, 0, 0) );
										FormatAndWriteBoolValue( pItemText, bValue, hResultProp );
									}
							}
								break;
						}
					}
				}
					break;
			}
		}
			break;
		case PP_TED_NOTVISIBLE   :
			break;
	}

	DTULiberator( pItemText );
	DTULiberator( pItemValue );
	DTULiberator( pItemFormat );

	return bResult;
}


// ---
static void ConvertArrayContents( HDATA hData, HPROP hResultProp ) {
	TCHAR *pItemFormat = 0;

	HPROP hProp = DATA_Find_Prop( hData, 0, 0 );
	AVP_dword nCount = PROP_Arr_Count( hProp );
	AVP_dword i;

	HPROP hOutFormatProp = DATA_Find_Prop( hData, NULL, PP_PID_FORMAT_STR_OUT ); 
	if ( hOutFormatProp )	
		pItemFormat = DTUT_GetPropValueAsString( hOutFormatProp, NULL );

	for ( i=0; i<nCount; i++ ) {
		TCHAR *pItemText = DTUT_GetPropArrayValueAsStringCC( hProp, i, pItemFormat );
		FormatAndWriteValue( pItemText, 0, hResultProp );
		DTULiberator( pItemText );
	}

	DTULiberator( pItemFormat );
}


// ---
static AVP_bool ConvertArrayNodeContents( HDATA hData, HPROP hResultProp ) {
	TCHAR *pItemText = 0;
	pItemText = DTUT_GetDataPropertyString( hData, TEXT_PID );
	FormatAndWriteGroup( pItemText, hResultProp );
	DTULiberator( pItemText );

	ConvertArrayContents( hData, hResultProp );

	return TRUE;
}

// ---
static AVP_bool ConvertRuleTreeNode( HDATA hData, HPROP hResultProp, AVP_dword *nIndexInParent ) {
	AVP_byte  nTEDType = PP_TED_NONEDITABLE;
	if ( DATA_Find_Prop( hData, NULL, PP_PID_EDIT_TYPE ) ) 
		DATA_Get_Val( hData, NULL, PP_PID_EDIT_TYPE, &nTEDType, sizeof(nTEDType) );

	if ( nTEDType != PP_TED_NOTVISIBLE ) {
		AVP_dword nID		 = DATA_Get_Id( hData, NULL );
		AVP_bool  bArray = GET_AVP_PID_ARR(nID);
		
		if ( !bArray ) {
			AVP_dword nType = GET_AVP_PID_TYPE(nID);
			if ( nType == PP_TYPE_RADIOGROUP ) {
				HPROP hContentsProp = DATA_Find_Prop( hData, NULL, PP_PID_LISTCONTENTS ); 
				
				if ( hContentsProp ) {
					HPROP	hProp	= DATA_Find_Prop( hData, 0, 0 );

					AVP_dword nValue = DTUT_GetPropNumericValueAsDWord( hProp );

					AVP_dword nCount = PROP_Arr_Count( hContentsProp );
					AVP_dword i;

					for ( i=0; i<nCount; i++ ) {
						if ( i == nValue ) {
							TCHAR *pItemText = 0;
							TCHAR *pItemValue = 0;
							HDATA hParentData = DATA_Get_Dad( hData, NULL );
							if ( hParentData )
								hData = hParentData;
							pItemText = DTUT_GetDataPropertyString( hData, TEXT_PID );
							pItemValue = DTUT_GetPropArrayValueAsString( hContentsProp, i, 0 );
							FormatAndWriteValue( pItemText, pItemValue, hResultProp );
							DTULiberator( pItemText );
							DTULiberator( pItemValue );
							break;
						}
					}
				}
				return TRUE;
			}
			else 
				return ConvertNodeContents( hData, hResultProp, nIndexInParent );
		}
		else 
			return ConvertArrayNodeContents( hData, hResultProp );
	}
	return FALSE;
}

// ---
static void ConvertRuleSubTree( HDATA hSrcData, HPROP hResultProp, AVP_dword *nIndexInParent ) {
	HDATA hCurrData = DATA_Get_First( hSrcData, NULL );

	AVP_dword nIndex = 0;
	while ( hCurrData ) {
		QueryAndChangeLocalisedStrings( hCurrData );
		if ( ConvertRuleTreeNode(hCurrData, hResultProp, nIndexInParent) )
			ConvertRuleSubTree( hCurrData, hResultProp, &nIndex );
		hCurrData = DATA_Get_Next( hCurrData, NULL );
	}
}

	
// ---
static void ConvertRulesTree( HDATA hRulesData, HDATA hPatternData, HPROP hResultProp, AVP_bool bShowUnselected ) {
	HDATA hCurrData = DATA_Get_First( hRulesData, NULL );

	QueryLocalisedStandardString();

	while ( hCurrData ) {
		HDATA hRuleRoot;
		TCHAR *pValue;
		MAKE_ADDR1( nAddr, NS_PID_RULE_ROOT )

		hRuleRoot = DATA_Find( hCurrData, nAddr );
		if ( hRuleRoot ) {
			AVP_bool bSelected = !!DATA_Find_Prop( hRuleRoot, NULL, NS_PID_RULE_SELECTED );
			if ( bSelected || bShowUnselected ) {
				HPROP hPatternIDProp;

				FormatAndWriteStringDivider( hResultProp );

				pValue = DTUT_GetPropValueAsString( DATA_Find_Prop(hCurrData, 0, 0), 0 );
				FormatAndWriteObject( pValue, hResultProp );
				DTULiberator( pValue );

				if ( bShowUnselected && g_pSelectedText ) 
					FormatAndWriteBoolValue( g_pSelectedText, bSelected, hResultProp );
				
				hPatternIDProp = DATA_Find_Prop( hCurrData, NULL, NS_PID_PATTERN_ID );
				if ( hPatternIDProp ) {
					// Если нашли PROP - получить идентификатор и корневой узел нужного шаблона
					AVP_dword nObjectID = DTUT_GetPropNumericValueAsDWord( hPatternIDProp );
					if ( nObjectID ) {
						MAKE_ADDR1( nAddr, nObjectID )
						HDATA hNodePattern = DATA_Find( hPatternData, nAddr );
						if ( hNodePattern )	{
							HDATA hSrcData = 0;
							if ( DTUT_MergeDataTreeAndPattern(hRuleRoot, hNodePattern, &hSrcData) ) {
								AVP_dword nIndex = 0;
								ConvertRuleSubTree( hSrcData, hResultProp, &nIndex );
								DATA_Remove( hSrcData, 0 );
							}
						}
					}
				}
			}
		}
		hCurrData = DATA_Get_Next( hCurrData, NULL );
	}

	FreeLocalisedStandardString();
}


// ---
// Convert Rules data tree to strings data tree
// hRulesData   - HDATA of rules tree to be converted
// hPatternData - HDATA of pattern tree to use for convert
// phResultData - pointer to result HDATA
// bShowUnselected - whether to show unselected rules
// Return	value - 1 on success, 0 on error occurs
DTUT_PROC AVP_bool DTUT_PARAM DTUT_ConvertRulesTreeToStringsTree( HDATA hRulesData, HDATA hPatternData, HDATA* phResultData, DTUT_LocalizeString pfLocalize, AVP_bool bShowUnselected ) {
	if ( !DTUAllocator || !DTULiberator ) {
    _RPT0( _CRT_ASSERT, _T("Library is not initialised") );
	}
	else 
		if ( !hRulesData || !hPatternData || !phResultData ) {
			_RPT0( _CRT_ASSERT, _T("DTUT_ConvertRulesTreeToStringsTree Wrong parameters") );
		}
		else {
			HDATA hResult = 0;
			g_bINILook = FALSE;
			if ( *phResultData && DATA_Get_Id(*phResultData, 0) == CNV_PID_ROOT ) 
				hResult = *phResultData;

			if ( !hResult ) 
				hResult = DATA_Add( 0, 0, CNV_PID_ROOT, 0, 0 );

			if ( hResult ) {
				g_pfLocCallBack = pfLocalize;
				
				ConvertRulesTree( hRulesData, hPatternData, DATA_Find_Prop(hResult, 0, 0), bShowUnselected );
				*phResultData =  hResult;
				
				g_pfLocCallBack = NULL;
				return 1;
			}
		}
	return 0;
}

// Convert known object Rules data tree to strings data tree
// hRulesData   - HDATA of rules tree to be converted
// hPatternData - HDATA of pattern tree to use for convert
// phResultData - pointer to result HDATA
// bShowUnselected - whether to show unselected rules
// Return	value - 1 on success, 0 on error occurs
DTUT_PROC AVP_bool DTUT_PARAM DTUT_ConvertKnownObjectRulesTreeToStringsTree( HDATA hRulesData, HDATA hPatternData, HDATA* phResultData, DTUT_LocalizeString pfLocalize, AVP_bool bShowUnselected ) {
	if ( !DTUAllocator || !DTULiberator ) {
    _RPT0( _CRT_ASSERT, _T("Library is not initialised") );
	}
	else 
		if ( !hRulesData || !hPatternData || !phResultData ) {
			_RPT0( _CRT_ASSERT, _T("DTUT_ConvertKnownObjectRulesTreeToStringsTree Wrong parameters") );
		}
		else {
			HDATA hResult = 0;
			g_bINILook = FALSE;
			if ( *phResultData && DATA_Get_Id(*phResultData, 0) == CNV_PID_ROOT ) 
				hResult = *phResultData;

			if ( !hResult ) 
				hResult = DATA_Add( 0, 0, CNV_PID_ROOT, 0, 0 );

			if ( hResult ) {
				HDATA hSrcData = 0;

				g_pfLocCallBack = pfLocalize;
				
				if ( DTUT_MergeDataTreeAndPattern(hRulesData, hPatternData, &hSrcData) ) {
					AVP_dword nIndex = 0;
					HPROP hResultProp = DATA_Find_Prop( hResult, 0, 0 );
					QueryLocalisedStandardString();

					PROP_Arr_Delta( hResultProp, 100 );

					ConvertRuleSubTree( hSrcData, hResultProp, &nIndex );
					DATA_Remove( hSrcData, 0 );

					FreeLocalisedStandardString();
				}
				*phResultData =  hResult;
				
				g_pfLocCallBack = NULL;
				return 1;
			}
		}
	return 0;
}

// Convert known object Rules data tree to strings data tree
// hRulesData   - HDATA of rules tree to be converted
// hPatternData - HDATA of pattern tree to use for convert
// phResultData - pointer to result HDATA
// pfLocalize   - pointer to string localize function
// bShowUnselected - whether to show unselected rules
// Return	value - 1 on success, 0 on error occurs
DTUT_PROC AVP_bool DTUT_PARAM DTUT_ConvertRulesTreeToStandardReportHeaderStringsTree( HDATA hRulesData, HDATA hPatternData, AVP_dword dwAppID, HDATA* phResultData, DTUT_LocalizeString pfLocalize, AVP_bool bShowUnselected ) {
	if ( !DTUAllocator || !DTULiberator ) {
    _RPT0( _CRT_ASSERT, _T("Library is not initialised") );
	}
	else 
		if ( !hRulesData || !hPatternData || !phResultData ) {
			_RPT0( _CRT_ASSERT, ("DTUT_ConvertRulesTreeToStandardReportHeaderStringsTree Wrong parameters") );
		}
		else {
			HDATA hResult = 0;
			g_bINILook = FALSE;
			if ( *phResultData && DATA_Get_Id(*phResultData, 0) == CNV_PID_ROOT ) 
				hResult = *phResultData;

			if ( !hResult ) 
				hResult = DATA_Add( 0, 0, CNV_PID_ROOT, 0, 0 );

			if ( hResult ) {
				HDATA hCfgData = NULL;
				HDATA hPtnData = NULL;
				HPROP hResultProp	= NULL;

				/*
				MAKE_ADDR2( nOIAddr, 
										MAKE_AVP_PID(dwAppID, AVP_APID_GLOBAL, avpt_nothing, 0),
										MAKE_AVP_PID(AVP_DC_OPTIONS_IMPLEMENTATION, AVP_APID_GLOBAL, avpt_nothing, 0) );

				MAKE_ADDR2( nOPAddr, 
										MAKE_AVP_PID(dwAppID, AVP_APID_GLOBAL, avpt_nothing, 0),
										MAKE_AVP_PID(AVP_DC_OPTIONS_PATTERN, AVP_APID_GLOBAL, avpt_nothing, 0) );

				MAKE_ADDR2( nCIAddr, 
										MAKE_AVP_PID(dwAppID, AVP_APID_GLOBAL, avpt_nothing, 0),
										MAKE_AVP_PID(AVP_DC_CONFIG_IMPLEMENTATION, AVP_APID_GLOBAL, avpt_nothing, 0) );

				MAKE_ADDR1( nCPAddr, MAKE_AVP_PID(dwAppID, AVP_APID_GLOBAL, avpt_nothing, 0) );
				*/

				AVP_dword nOIAddr[3], nOPAddr[3], nCIAddr[3], nCPAddr[3];

				DATA_Sequence( nOIAddr, 
											 //MAKE_AVP_PID(dwAppID, AVP_APID_GLOBAL, avpt_nothing, 0),
										   MAKE_AVP_PID(AVP_DC_OPTIONS_IMPLEMENTATION, AVP_APID_GLOBAL, avpt_nothing, 0),
											 0 );       
				DATA_Sequence( nOPAddr, 
											 MAKE_AVP_PID(dwAppID, AVP_APID_GLOBAL, avpt_nothing, 0),
										   MAKE_AVP_PID(AVP_DC_OPTIONS_PATTERN, AVP_APID_GLOBAL, avpt_nothing, 0),
											 0 );
				DATA_Sequence( nCIAddr, 
										   //MAKE_AVP_PID(dwAppID, AVP_APID_GLOBAL, avpt_nothing, 0),
										   MAKE_AVP_PID(AVP_DC_CONFIG_IMPLEMENTATION, AVP_APID_GLOBAL, avpt_nothing, 0),
											 0 );
				DATA_Sequence( nCPAddr, 
											 MAKE_AVP_PID(dwAppID, AVP_APID_GLOBAL, avpt_nothing, 0), 
										   MAKE_AVP_PID(AVP_DC_CONFIG_PATTERN, AVP_APID_GLOBAL, avpt_nothing, 0),
											 0 );

				g_pfLocCallBack = pfLocalize;

				hResultProp	= DATA_Find_Prop( hResult, 0, 0 );

				DTUT_SetPropArrayValue( hResultProp, -1, COMMENT, NULL );
				DTUT_SetPropArrayValue( hResultProp, -1, COMMENT, NULL );

				hCfgData = DATA_Find( hRulesData, nOIAddr );
				hPtnData = DATA_Find( hPatternData, nOPAddr );

				if ( hCfgData && hPtnData ) {
					HDATA hResultData = 0;

					if( DTUT_ConvertKnownObjectRulesTreeToStringsTree(hCfgData, hPtnData, &hResultData, pfLocalize, bShowUnselected) ) {

						AVP_dword i;
						HPROP hProp	= DATA_Find_Prop( hResultData, 0, 0 );
						AVP_dword nCount = PROP_Arr_Count( hProp );
						for ( i=0; i<nCount; i++ ) {
							TCHAR *pStr = DTUT_GetPropArrayValueAsString( hProp, (int)i, NULL );

							TCHAR *pCortege = DTUAllocator( SIZEOF_TCHAR * (PREFIX_EXTRA + DTU_TCSCLEN(pStr) + 1) );
							_tcscpy( pCortege, PREFIX );
							_tcscat( pCortege, pStr );

							DTUT_SetPropArrayValue( hResultProp, -1, pCortege, NULL );

							DTULiberator( pStr );
							DTULiberator( pCortege );
						}

						DTUT_SetPropArrayValue( hResultProp, -1, PREFIX, NULL );
						DTUT_SetPropArrayValue( hResultProp, -1, PREFIX, NULL );

						DATA_Remove( hResultData, 0 );
					}
				}


				hCfgData = DATA_Find( hRulesData, nCIAddr );
				hPtnData = DATA_Find( hPatternData, nCPAddr );

				if ( hCfgData && hPtnData ) {
					HDATA hResultData = 0;

					if( DTUT_ConvertRulesTreeToStringsTree(hCfgData, hPtnData, &hResultData, pfLocalize, bShowUnselected) ) {

						AVP_dword i;
						HPROP hProp	= DATA_Find_Prop( hResultData, 0, 0 );
						AVP_dword nCount = PROP_Arr_Count( hProp );
						for ( i=0; i<nCount; i++ ) {
							TCHAR *pStr = DTUT_GetPropArrayValueAsString( hProp, (int)i, NULL );

							TCHAR *pCortege = DTUAllocator( SIZEOF_TCHAR * (PREFIX_EXTRA + DTU_TCSCLEN(pStr) + 1) );
							_tcscpy( pCortege, PREFIX );
							_tcscat( pCortege, pStr );

							DTUT_SetPropArrayValue( hResultProp, -1, pCortege, NULL );

							DTULiberator( pStr );
							DTULiberator( pCortege );
						}

						DTUT_SetPropArrayValue( hResultProp, -1, PREFIX, NULL );
						DTUT_SetPropArrayValue( hResultProp, -1, PREFIX, NULL );

						DATA_Remove( hResultData, 0 );
					}
				}


				*phResultData =  hResult;
				
				g_pfLocCallBack = NULL;

				return 1;
			}
		}
	return 0;
}


// ---
static AVP_bool ConvertRuleSubTreeToINI( HDATA hSrcData, HPROP hResultProp, AVP_dword *nIndexInParent ) {
	HDATA hCurrData = DATA_Get_First( hSrcData, NULL );

	AVP_bool bSectionChanged = FALSE;
	AVP_dword nIndex = 0;
	while ( hCurrData ) {
		AVP_byte  nTEDType = PP_TED_NONEDITABLE;
		QueryAndChangeLocalisedStrings( hCurrData );

		if ( DATA_Find_Prop( hCurrData, NULL, PP_PID_EDIT_TYPE ) ) 
			DATA_Get_Val( hCurrData, NULL, PP_PID_EDIT_TYPE, &nTEDType, sizeof(nTEDType) );

		if ( nTEDType != PP_TED_NOTVISIBLE ) {
			if ( ConvertRuleTreeNode(hCurrData, hResultProp, nIndexInParent) )
				bSectionChanged |= ConvertRuleSubTreeToINI( hCurrData, hResultProp, &nIndex );
		}
		else {
			TCHAR *pItemText = DTUT_GetDataPropertyString( hCurrData, TEXT_PID );
			FormatAndWriteSection( pItemText, hResultProp );
			bSectionChanged |= TRUE;
			
			if ( ConvertRuleSubTreeToINI(hCurrData, hResultProp, &nIndex) &&
			     DATA_Get_Next(hCurrData, NULL) )
				FormatAndWriteSection( pItemText, hResultProp );
			DTULiberator( pItemText );
		}
		hCurrData = DATA_Get_Next( hCurrData, NULL );
	}
	return bSectionChanged;
}


// Convert Rules data tree to INI strings data tree
// hRulesData   - HDATA of rules tree to be converted
// hPatternData - HDATA of pattern tree to use for convert
// phResultData - pointer to result HDATA
// Return	value - 1 on success, 0 on error occurs
DTUT_PROC AVP_bool DTUT_PARAM DTUT_ConvertRulesTreeToINIStringsTree( HDATA hRulesData, HDATA hPatternData, HDATA* phResultData, DTUT_LocalizeString pfLocalize ) {
	if ( !DTUAllocator || !DTULiberator ) {
    _RPT0( _CRT_ASSERT, _T("Library is not initialised") );
	}
	else 
		if ( !hRulesData || !phResultData ) {
			_RPT0( _CRT_ASSERT, _T("DTUT_ConvertRulesTreeToINIStringsTree Wrong parameters") );
		}
		else {
			HDATA hResult = 0;
			g_bINILook = TRUE;
			if ( *phResultData && DATA_Get_Id(*phResultData, 0) == CNV_PID_ROOT ) 
				hResult = *phResultData;

			if ( !hResult ) 
				hResult = DATA_Add( 0, 0, CNV_PID_ROOT, 0, 0 );

			if ( hResult ) {
				HDATA hSrcData = 0;

				g_pfLocCallBack = pfLocalize;
				
				if ( hPatternData )
					DTUT_MergeDataTreeAndPattern( hRulesData, hPatternData, &hSrcData );
				else
					hSrcData = DATA_Copy( 0, 0, hRulesData, DATA_IF_ROOT_INCLUDE );

				if ( hSrcData ) {
					AVP_dword nIndex = 0;
					ConvertRuleSubTreeToINI( hSrcData, DATA_Find_Prop(hResult, 0, 0), &nIndex );
					DATA_Remove( hSrcData, 0 );
				}

				*phResultData =  hResult;
				
				g_pfLocCallBack = NULL;
				return 1;
			}
		}
	return 0;
}

