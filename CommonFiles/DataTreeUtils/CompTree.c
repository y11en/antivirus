////////////////////////////////////////////////////////////////////
//
//  CompTree.c
//  Compare data trees module
//  AVP generic purpose stuff
//  Victor Matiouchenkov [victor@avp.ru], Kaspersky Labs. 1999
//  Copyright (c) Kaspersky Labs
//
////////////////////////////////////////////////////////////////////

#include "datatreeutils/dtutils.h"
#include "AVPPort.h"

extern void* (* DTUAllocator)(AVP_uint);
extern void  (* DTULiberator)(void*);

// ---
typedef struct CDTInfoStruct {
	HDATA			m_hFirstData;
	HDATA			m_hSecondData;
	AVP_bool	m_bStrict;
} CDTInfoStruct;


// ---
static DTUT_PROC AVP_bool DTUT_PARAM DTUT_CompareDataTreeNodeProps( HPROP hFProp, HPROP hSProp, AVP_bool bCompareIDs ) {
	if ( !DTUAllocator || !DTULiberator ) {
    _RPT0( _CRT_ASSERT, "Library is not initialised" );
		return 0;
	}
	else {
		AVP_dword nPos;
		AVP_dword nFromID = PROP_Get_Id( hFProp );
		AVP_dword nToID   = PROP_Get_Id( hSProp );
		if ( !bCompareIDs || nFromID == nToID ) { 
			AVP_dword nType  = GET_AVP_PID_TYPE(nFromID);
			AVP_bool  bArray = GET_AVP_PID_ARR(nFromID);
			switch ( nType ) {
				case avpt_char   : 
					if ( !bArray ) {
						AVP_char nFValue;
						AVP_char nSValue;
						PROP_Get_Val( hFProp, &nFValue, sizeof(nFValue) );
						PROP_Get_Val( hSProp, &nSValue, sizeof(nSValue) );
						return nFValue == nSValue;
					}
					else {
						AVP_dword nFCount = PROP_Arr_Count( hFProp );
						AVP_dword nSCount = PROP_Arr_Count( hSProp );
						if ( nFCount == nSCount ) {
							for ( nPos=0; nPos<nFCount; nPos++ ) {
								AVP_char nFValue;
								AVP_char nSValue;
								PROP_Arr_Get_Items( hFProp, nPos, &nFValue, sizeof(nFValue) );
								PROP_Arr_Get_Items( hSProp, nPos, &nSValue, sizeof(nSValue) );
								if ( nFValue != nSValue )
									return 0;
							}
							return 1;
						}
						return 0;
					}
					break;
				case avpt_wchar  : 
					if ( !bArray ) {
						AVP_wchar pWFChar;
						AVP_wchar pWSChar;
						PROP_Get_Val( hFProp, &pWFChar, sizeof(pWFChar) );
						PROP_Get_Val( hSProp, &pWSChar, sizeof(pWSChar) );
						return pWFChar == pWSChar;
					}
					else {
						AVP_dword nFCount = PROP_Arr_Count( hFProp );
						AVP_dword nSCount = PROP_Arr_Count( hSProp );
						if ( nFCount == nSCount ) {
							for ( nPos=0; nPos<nFCount; nPos++ ) {
								AVP_wchar pWFChar;
								AVP_wchar pWSChar;
								PROP_Arr_Get_Items( hFProp, nPos, &pWFChar, sizeof(pWFChar) );
								PROP_Arr_Get_Items( hSProp, nPos, &pWSChar, sizeof(pWSChar) );
								if ( pWFChar != pWSChar )
									return 0;
							}
							return 1;
						}
						return 0;
					}
					break;
				case avpt_short  : 
					if ( !bArray ) {
						AVP_short nFValue;
						AVP_short nSValue;
						PROP_Get_Val( hFProp, &nFValue, sizeof(nFValue) );
						PROP_Get_Val( hSProp, &nSValue, sizeof(nSValue) );
						return nFValue == nSValue;
					}
					else {
						AVP_dword nFCount = PROP_Arr_Count( hFProp );
						AVP_dword nSCount = PROP_Arr_Count( hSProp );
						if ( nFCount == nSCount ) {
							for ( nPos=0; nPos<nFCount; nPos++ ) {
								AVP_short nFValue;
								AVP_short nSValue;
								PROP_Arr_Get_Items( hFProp, nPos, &nFValue, sizeof(nFValue) );
								PROP_Arr_Get_Items( hSProp, nPos, &nSValue, sizeof(nSValue) );
								if ( nFValue != nSValue )
									return 0;
							}
							return 1;
						}
						return 0;
					}
					break;
				case avpt_long   : 
					if ( !bArray ) {
						AVP_long nFValue;
						AVP_long nSValue;
						PROP_Get_Val( hFProp, &nFValue, sizeof(nFValue) );
						PROP_Get_Val( hSProp, &nSValue, sizeof(nSValue) );
						return nFValue == nSValue;
					}
					else {
						AVP_dword nFCount = PROP_Arr_Count( hFProp );
						AVP_dword nSCount = PROP_Arr_Count( hSProp );
						if ( nFCount == nSCount ) {
							for ( nPos=0; nPos<nFCount; nPos++ ) {
								AVP_long nFValue;
								AVP_long nSValue;
								PROP_Arr_Get_Items( hFProp, nPos, &nFValue, sizeof(nFValue) );
								PROP_Arr_Get_Items( hSProp, nPos, &nSValue, sizeof(nSValue) );
								if ( nFValue != nSValue )
									return 0;
							}
							return 1;
						}
						return 0;
					}
					break;
				case avpt_longlong   : 
					if ( !bArray ) {
						AVP_longlong nFValue;
						AVP_longlong nSValue;
						PROP_Get_Val( hFProp, &nFValue, sizeof(nFValue) );
						PROP_Get_Val( hSProp, &nSValue, sizeof(nSValue) );
						return nFValue == nSValue;
					}
					else {
						AVP_dword nFCount = PROP_Arr_Count( hFProp );
						AVP_dword nSCount = PROP_Arr_Count( hSProp );
						if ( nFCount == nSCount ) {
							for ( nPos=0; nPos<nFCount; nPos++ ) {
								AVP_longlong nFValue;
								AVP_longlong nSValue;
								PROP_Arr_Get_Items( hFProp, nPos, &nFValue, sizeof(nFValue) );
								PROP_Arr_Get_Items( hSProp, nPos, &nSValue, sizeof(nSValue) );
								if ( nFValue != nSValue )
									return 0;
							}
							return 1;
						}
						return 0;
					}
					break;
				case avpt_byte   : 
					if ( !bArray ) {
						AVP_byte nFValue;
						AVP_byte nSValue;
						PROP_Get_Val( hFProp, &nFValue, sizeof(nFValue) );
						PROP_Get_Val( hSProp, &nSValue, sizeof(nSValue) );
						return nFValue == nSValue;
					}
					else {
						AVP_dword nFCount = PROP_Arr_Count( hFProp );
						AVP_dword nSCount = PROP_Arr_Count( hSProp );
						if ( nFCount == nSCount ) {
							for ( nPos=0; nPos<nFCount; nPos++ ) {
								AVP_byte nFValue;
								AVP_byte nSValue;
								PROP_Arr_Get_Items( hFProp, nPos, &nFValue, sizeof(nFValue) );
								PROP_Arr_Get_Items( hSProp, nPos, &nSValue, sizeof(nSValue) );
								if ( nFValue != nSValue )
									return 0;
							}
							return 1;
						}
						return 0;
					}
					break;
				case avpt_word   : 
					if ( !bArray ) {
						AVP_word nFValue;
						AVP_word nSValue;
						PROP_Get_Val( hFProp, &nFValue, sizeof(nFValue) );
						PROP_Get_Val( hSProp, &nSValue, sizeof(nSValue) );
						return nFValue == nSValue;
					}
					else {
						AVP_dword nFCount = PROP_Arr_Count( hFProp );
						AVP_dword nSCount = PROP_Arr_Count( hSProp );
						if ( nFCount == nSCount ) {
							for ( nPos=0; nPos<nFCount; nPos++ ) {
								AVP_word nFValue;
								AVP_word nSValue;
								PROP_Arr_Get_Items( hFProp, nPos, &nFValue, sizeof(nFValue) );
								PROP_Arr_Get_Items( hSProp, nPos, &nSValue, sizeof(nSValue) );
								if ( nFValue != nSValue )
									return 0;
							}
							return 1;
						}
						return 0;
					}
					break;
				case avpt_dword  : 
					if ( !bArray ) {
						AVP_dword nFValue;
						AVP_dword nSValue;
						PROP_Get_Val( hFProp, &nFValue, sizeof(nFValue) );
						PROP_Get_Val( hSProp, &nSValue, sizeof(nSValue) );
						return nFValue == nSValue;
					}
					else {
						AVP_dword nFCount = PROP_Arr_Count( hFProp );
						AVP_dword nSCount = PROP_Arr_Count( hSProp );
						if ( nFCount == nSCount ) {
							for ( nPos=0; nPos<nFCount; nPos++ ) {
								AVP_dword nFValue;
								AVP_dword nSValue;
								PROP_Arr_Get_Items( hFProp, nPos, &nFValue, sizeof(nFValue) );
								PROP_Arr_Get_Items( hSProp, nPos, &nSValue, sizeof(nSValue) );
								if ( nFValue != nSValue )
									return 0;
							}
							return 1;
						}
						return 0;
					}
					break;
				case avpt_qword  : 
					if ( !bArray ) {
						AVP_qword nFValue;
						AVP_qword nSValue;
						PROP_Get_Val( hFProp, &nFValue, sizeof(nFValue) );
						PROP_Get_Val( hSProp, &nSValue, sizeof(nSValue) );
						return nFValue == nSValue;
					}
					else {
						AVP_dword nFCount = PROP_Arr_Count( hFProp );
						AVP_dword nSCount = PROP_Arr_Count( hSProp );
						if ( nFCount == nSCount ) {
							for ( nPos=0; nPos<nFCount; nPos++ ) {
								AVP_qword nFValue;
								AVP_qword nSValue;
								PROP_Arr_Get_Items( hFProp, nPos, &nFValue, sizeof(nFValue) );
								PROP_Arr_Get_Items( hSProp, nPos, &nSValue, sizeof(nSValue) );
								if ( nFValue != nSValue )
									return 0;
							}
							return 1;
						}
						return 0;
					}
					break;
				case avpt_int   : 
					if ( !bArray ) {
						AVP_int nFValue;
						AVP_int nSValue;
						PROP_Get_Val( hFProp, &nFValue, sizeof(nFValue) );
						PROP_Get_Val( hSProp, &nSValue, sizeof(nSValue) );
						return nFValue == nSValue;
					}
					else {
						AVP_dword nFCount = PROP_Arr_Count( hFProp );
						AVP_dword nSCount = PROP_Arr_Count( hSProp );
						if ( nFCount == nSCount ) {
							for ( nPos=0; nPos<nFCount; nPos++ ) {
								AVP_int nFValue;
								AVP_int nSValue;
								PROP_Arr_Get_Items( hFProp, nPos, &nFValue, sizeof(nFValue) );
								PROP_Arr_Get_Items( hSProp, nPos, &nSValue, sizeof(nSValue) );
								if ( nFValue != nSValue )
									return 0;
							}
							return 1;
						}
						return 0;
					}
					break;
				case avpt_uint   : 
					if ( !bArray ) {
						AVP_uint nFValue;
						AVP_uint nSValue;
						PROP_Get_Val( hFProp, &nFValue, sizeof(nFValue) );
						PROP_Get_Val( hSProp, &nSValue, sizeof(nSValue) );
						return nFValue == nSValue;
					}
					else {
						AVP_dword nFCount = PROP_Arr_Count( hFProp );
						AVP_dword nSCount = PROP_Arr_Count( hSProp );
						if ( nFCount == nSCount ) {
							for ( nPos=0; nPos<nFCount; nPos++ ) {
								AVP_uint nFValue;
								AVP_uint nSValue;
								PROP_Arr_Get_Items( hFProp, nPos, &nFValue, sizeof(nFValue) );
								PROP_Arr_Get_Items( hSProp, nPos, &nSValue, sizeof(nSValue) );
								if ( nFValue != nSValue )
									return 0;
							}
							return 1;
						}
						return 0;
					}
					break;
				case avpt_bool   : 
					if ( !bArray ) {
						AVP_bool nFValue;
						AVP_bool nSValue;
						PROP_Get_Val( hFProp, &nFValue, sizeof(nFValue) );
						PROP_Get_Val( hSProp, &nSValue, sizeof(nSValue) );
						return nFValue == nSValue;
					}
					else {
						AVP_dword nFCount = PROP_Arr_Count( hFProp );
						AVP_dword nSCount = PROP_Arr_Count( hSProp );
						if ( nFCount == nSCount ) {
							for ( nPos=0; nPos<nFCount; nPos++ ) {
								AVP_bool nFValue;
								AVP_bool nSValue;
								PROP_Arr_Get_Items( hFProp, nPos, &nFValue, sizeof(nFValue) );
								PROP_Arr_Get_Items( hSProp, nPos, &nSValue, sizeof(nSValue) );
								if ( nFValue != nSValue )
									return 0;
							}
							return 1;
						}
						return 0;
					}
					break;
				case avpt_group  : 
					if ( !bArray ) {
						AVP_group nFValue;
						AVP_group nSValue;
						PROP_Get_Val( hFProp, &nFValue, sizeof(nFValue) );
						PROP_Get_Val( hSProp, &nSValue, sizeof(nSValue) );
						return nFValue == nSValue;
					}
					else {
						AVP_dword nFCount = PROP_Arr_Count( hFProp );
						AVP_dword nSCount = PROP_Arr_Count( hSProp );
						if ( nFCount == nSCount ) {
							for ( nPos=0; nPos<nFCount; nPos++ ) {
								AVP_group nFValue;
								AVP_group nSValue;
								PROP_Arr_Get_Items( hFProp, nPos, &nFValue, sizeof(nFValue) );
								PROP_Arr_Get_Items( hSProp, nPos, &nSValue, sizeof(nSValue) );
								if ( nFValue != nSValue )
									return 0;
							}
							return 1;
						}
						return 0;
					}
					break;
				case avpt_date   : 
					if ( !bArray ) {
						AVP_date nFValue;
						AVP_date nSValue;
						PROP_Get_Val( hFProp, &nFValue, sizeof(nFValue) );
						PROP_Get_Val( hSProp, &nSValue, sizeof(nSValue) );
						return !memcmp( nFValue, nSValue, sizeof(nFValue) );
					}
					else {
						AVP_dword nFCount = PROP_Arr_Count( hFProp );
						AVP_dword nSCount = PROP_Arr_Count( hSProp );
						if ( nFCount == nSCount ) {
							for ( nPos=0; nPos<nFCount; nPos++ ) {
								AVP_date nFValue;
								AVP_date nSValue;
								PROP_Arr_Get_Items( hFProp, nPos, &nFValue, sizeof(nFValue) );
								PROP_Arr_Get_Items( hSProp, nPos, &nSValue, sizeof(nSValue) );
								if ( memcmp(nFValue, nSValue, sizeof(nFValue)) )
									return 0;
							}
							return 1;
						}
						return 0;
					}
					break;
				case avpt_time   : 
					if ( !bArray ) {
						AVP_time nFValue;
						AVP_time nSValue;
						PROP_Get_Val( hFProp, &nFValue, sizeof(nFValue) );
						PROP_Get_Val( hSProp, &nSValue, sizeof(nSValue) );
						return !memcmp( nFValue, nSValue, sizeof(nFValue) );
					}
					else {
						AVP_dword nFCount = PROP_Arr_Count( hFProp );
						AVP_dword nSCount = PROP_Arr_Count( hSProp );
						if ( nFCount == nSCount ) {
							for ( nPos=0; nPos<nFCount; nPos++ ) {
								AVP_time nFValue;
								AVP_time nSValue;
								PROP_Arr_Get_Items( hFProp, nPos, &nFValue, sizeof(nFValue) );
								PROP_Arr_Get_Items( hSProp, nPos, &nSValue, sizeof(nSValue) );
								if ( memcmp(nFValue, nSValue, sizeof(nFValue)) )
									return 0;
							}
							return 1;
						}
						return 0;
					}
					break;
				case avpt_datetime   : 
					if ( !bArray ) {
						AVP_datetime nFValue;
						AVP_datetime nSValue;
						PROP_Get_Val( hFProp, &nFValue, sizeof(nFValue) );
						PROP_Get_Val( hSProp, &nSValue, sizeof(nSValue) );
						return !memcmp( nFValue, nSValue, sizeof(nFValue) );
					}
					else {
						AVP_dword nFCount = PROP_Arr_Count( hFProp );
						AVP_dword nSCount = PROP_Arr_Count( hSProp );
						if ( nFCount == nSCount ) {
							for ( nPos=0; nPos<nFCount; nPos++ ) {
								AVP_datetime nFValue;
								AVP_datetime nSValue;
								PROP_Arr_Get_Items( hFProp, nPos, &nFValue, sizeof(nFValue) );
								PROP_Arr_Get_Items( hSProp, nPos, &nSValue, sizeof(nSValue) );
								if ( memcmp(nFValue, nSValue, sizeof(nFValue)) )
									return 0;
							}
							return 1;
						}
						return 0;
					}
					break;
				case avpt_str    : 
					if ( !bArray ) {
						int nFSize = PROP_Get_Val( hFProp, NULL, 0 );
						int nSSize = PROP_Get_Val( hSProp, NULL, 0 );
						if ( nFSize == nSSize ) {
							AVP_bool bResult;
							AVP_char *pFResult = DTUAllocator(sizeof(AVP_char) * nFSize);
							AVP_char *pSResult = DTUAllocator(sizeof(AVP_char) * nSSize);
							PROP_Get_Val( hFProp, pFResult, nFSize );
							PROP_Get_Val( hSProp, pSResult, nSSize );
							bResult = !memcmp( pFResult, pSResult, nFSize );
							DTULiberator( pFResult );
							DTULiberator( pSResult );
							return bResult;
						}
						return 0;
					}
					else {
						AVP_dword nFCount = PROP_Arr_Count( hFProp );
						AVP_dword nSCount = PROP_Arr_Count( hSProp );
						if ( nFCount == nSCount ) {
							for ( nPos=0; nPos<nFCount; nPos++ ) {
								int nFSize = PROP_Arr_Get_Items( hFProp, nPos, 0, 0 );
								int nSSize = PROP_Arr_Get_Items( hSProp, nPos, 0, 0 );
								if ( nFSize == nSSize ) {
									AVP_bool bResult;
									AVP_char *pFResult = DTUAllocator(sizeof(AVP_char) * nFSize);
									AVP_char *pSResult = DTUAllocator(sizeof(AVP_char) * nSSize);
									PROP_Arr_Get_Items( hFProp, nPos, pFResult, nFSize );
									PROP_Arr_Get_Items( hSProp, nPos, pSResult, nSSize );
									bResult = !memcmp( pFResult, pSResult, nFSize );
									DTULiberator( pFResult );
									DTULiberator( pSResult );
									if ( !bResult )
										return 0;
								}
								else
									return 0;
							}
							return 1;
						}
						return 0;
					}
					break;
				case avpt_wstr   : 
					if ( !bArray ) {
						int nFSize = PROP_Get_Val( hFProp, NULL, 0 );
						int nSSize = PROP_Get_Val( hSProp, NULL, 0 );
						if ( nFSize == nSSize ) {
							AVP_bool bResult;
							AVP_wchar *pFResult = DTUAllocator(sizeof(AVP_wchar) * nFSize);
							AVP_wchar *pSResult = DTUAllocator(sizeof(AVP_wchar) * nSSize);
							PROP_Get_Val( hFProp, pFResult, nFSize );
							PROP_Get_Val( hSProp, pSResult, nSSize );
							bResult = !memcmp( pFResult, pSResult, nFSize );
							DTULiberator( pFResult );
							DTULiberator( pSResult );
							return bResult;
						}
						return 0;
					}
					else {
						AVP_dword nFCount = PROP_Arr_Count( hFProp );
						AVP_dword nSCount = PROP_Arr_Count( hSProp );
						if ( nFCount == nSCount ) {
							for ( nPos=0; nPos<nFCount; nPos++ ) {
								int nFSize = PROP_Arr_Get_Items( hFProp, nPos, 0, 0 );
								int nSSize = PROP_Arr_Get_Items( hSProp, nPos, 0, 0 );
								if ( nFSize == nSSize ) {
									AVP_bool bResult;
									AVP_wchar *pFResult = DTUAllocator(sizeof(AVP_wchar) * nFSize);
									AVP_wchar *pSResult = DTUAllocator(sizeof(AVP_wchar) * nSSize);
									PROP_Arr_Get_Items( hFProp, nPos, pFResult, nFSize );
									PROP_Arr_Get_Items( hSProp, nPos, pSResult, nSSize );
									bResult = !memcmp( pFResult, pSResult, nFSize );
									DTULiberator( pFResult );
									DTULiberator( pSResult );
									if ( !bResult )
										return 0;
								}
								else
									return 0;
							}
							return 1;
						}
						return 0;
					}
					break;
				case avpt_bin : 
					if ( !bArray ) {
						int nFSize = PROP_Get_Val( hFProp, NULL, 0 );
						int nSSize = PROP_Get_Val( hSProp, NULL, 0 );
						if ( nFSize == nSSize ) {
							AVP_bool bResult;
							AVP_byte *pFResult = DTUAllocator(sizeof(AVP_byte) * nFSize);
							AVP_byte *pSResult = DTUAllocator(sizeof(AVP_byte) * nSSize);
							PROP_Get_Val( hFProp, pFResult, nFSize );
							PROP_Get_Val( hSProp, pSResult, nSSize );
							bResult = !memcmp( pFResult, pSResult, nFSize );
							DTULiberator( pFResult );
							DTULiberator( pSResult );
							return bResult;
						}
						return 0;
					}
					else {
						AVP_dword nFCount = PROP_Arr_Count( hFProp );
						AVP_dword nSCount = PROP_Arr_Count( hSProp );
						if ( nFCount == nSCount ) {
							for ( nPos=0; nPos<nFCount; nPos++ ) {
								int nFSize = PROP_Arr_Get_Items( hFProp, nPos, 0, 0 );
								int nSSize = PROP_Arr_Get_Items( hSProp, nPos, 0, 0 );
								if ( nFSize == nSSize ) {
									AVP_bool bResult;
									AVP_byte *pFResult = DTUAllocator(sizeof(AVP_byte) * nFSize);
									AVP_byte *pSResult = DTUAllocator(sizeof(AVP_byte) * nSSize);
									PROP_Arr_Get_Items( hFProp, nPos, pFResult, nFSize );
									PROP_Arr_Get_Items( hSProp, nPos, pSResult, nSSize );
									bResult = !memcmp( pFResult, pSResult, nFSize );
									DTULiberator( pFResult );
									DTULiberator( pSResult );
									if ( !bResult )
										return 0;
								}
								else
									return 0;
							}
							return 1;
						}
						return 0;
					}
					break;
			}
		}
		return 1;
	}
	return 0;
}

// ---
static AVP_bool CompareDataTreeNodes( HDATA hData, void* pUserData ) {
	CDTInfoStruct *pInfo = (CDTInfoStruct *)pUserData;
	AVP_dword *pSeq = 0;
	AVP_bool bResult = 0;
	HDATA hFindData;

  DATA_Make_Sequence( hData, 0, pInfo->m_hFirstData, &pSeq );
	hFindData = pSeq ? DATA_Find( pInfo->m_hSecondData, pSeq ) : pInfo->m_hSecondData;
	if ( hFindData ) {
		HPROP hFHeadProp = DATA_Find_Prop(hData, 0, 0);
		HPROP hSHeadProp = DATA_Find_Prop(hFindData, 0, 0);

		AVP_dword dwFPID = PROP_Get_Id( hFHeadProp );
		AVP_dword dwSPID = PROP_Get_Id( hSHeadProp );

		if ( GET_AVP_PID_APP(dwFPID)	== GET_AVP_PID_APP(dwSPID) &&
			   GET_AVP_PID_TYPE(dwFPID)	== GET_AVP_PID_TYPE(dwSPID) &&
				 GET_AVP_PID_ARR(dwFPID)  == GET_AVP_PID_ARR(dwSPID) &&
				 (!pInfo->m_bStrict || GET_AVP_PID_ID(dwFPID)	== GET_AVP_PID_ID(dwSPID)) ) {

			bResult = DTUT_CompareDataTreeNodeProps( hFHeadProp, hSHeadProp, 0 );

		  if ( bResult ) {
				HPROP hSProp;
				HPROP hCurrProp = DATA_Get_First_Prop( hData, NULL );
				while ( hCurrProp && bResult ) {
					if ( hFHeadProp != hCurrProp ) {
						dwFPID = PROP_Get_Id( hCurrProp );
						hSProp = DATA_Find_Prop( pInfo->m_hSecondData, 0, dwFPID );
						if ( !hSProp ) {
							bResult = 0;
						}
						else
							bResult = DTUT_CompareDataTreeNodeProps( hCurrProp, hSProp, 1 );
					}
					hCurrProp = PROP_Get_Next( hCurrProp );
				}
			}
		}
	}

	DATA_Remove_Sequence( pSeq );
	
	return !bResult;
}

// Compare two data trees
// hFirstData		- the first HDATA to compare
// hSecondData	- the second HDATA to compare
// dwFlags			- trees recursion flags (see Property.h)
//                Can be OR'ed with CDT_STRICT_CMP which means the strict comparison should be used.
//                In this case data tree nodes should have fully equal PIDs - i.e. IDs, AppIDs, 
//                types and array flags. Otherwise node IDs will have been excluded from comparison.
// Return	value - 1 on trees are equal, 0 on any other cases (including errors)
DTUT_PROC AVP_bool DTUT_PARAM DTUT_CompareDataTrees( HDATA hFirstData, HDATA hSecondData, AVP_dword dwFlags ) {
	if ( !DTUAllocator || !DTULiberator ) {
    _RPT0( _CRT_ASSERT, "Library is not initialised" );
		return 0;
	}
	if ( hFirstData && hSecondData ) {
		HDATA hResult;
		AVP_dword dwCleanFlags = dwFlags & ~CDT_STRICT_CMP;

		CDTInfoStruct rcInfo;
		rcInfo.m_bStrict = !!(dwFlags & CDT_STRICT_CMP);
		rcInfo.m_hFirstData = hFirstData;
		rcInfo.m_hSecondData = hSecondData;

		hResult = DATA_First_That( hFirstData, NULL, dwCleanFlags, CompareDataTreeNodes, &rcInfo );
		if ( !hResult ) {
			rcInfo.m_hFirstData = hSecondData;
			rcInfo.m_hSecondData = hFirstData;
			hResult = DATA_First_That( hSecondData, NULL, dwCleanFlags, CompareDataTreeNodes, &rcInfo );
		}

		return !hResult;
	}

	return 0;
}