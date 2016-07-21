////////////////////////////////////////////////////////////////////
//
//  ExchProp.c
//  Exchange prop value module
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
static void FlushArray( HPROP hArrayProp ) {
	AVP_dword nCount = PROP_Arr_Count( hArrayProp );
	if ( nCount )
		PROP_Arr_Remove( hArrayProp, 0, nCount );
}


// ---
DTUT_PROC AVP_bool DTUT_PARAM DTUT_ExchangePropValue( HPROP hFromProp, HPROP hToProp ) {
	if ( !DTUAllocator || !DTULiberator ) {
    _RPT0( _CRT_ASSERT, "Library is not initialised" );
		return 0;
	}
	else {
		AVP_dword nPos;
		AVP_dword nFromID = PROP_Get_Id( hFromProp );
		AVP_dword nToID   = PROP_Get_Id( hToProp );
		if ( nFromID == nToID ) { 
			AVP_dword nType  = GET_AVP_PID_TYPE(nFromID);
			AVP_bool  bArray = GET_AVP_PID_ARR(nFromID);
			switch ( nType ) {
				case avpt_char   : 
					if ( !bArray ) {
						AVP_char nValue;
						PROP_Get_Val( hFromProp, &nValue, sizeof(nValue) );
						PROP_Set_Val( hToProp, nValue, 0 );
					}
					else {
						AVP_dword nCount = PROP_Arr_Count( hFromProp );
						FlushArray( hToProp );
						for ( nPos=0; nPos<nCount; nPos++ ) {
							AVP_char nValue;
							PROP_Arr_Get_Items( hFromProp, nPos, &nValue, sizeof(nValue) );
							PROP_Arr_Insert( hToProp, PROP_ARR_LAST, &nValue, sizeof(nValue) );
						}
					}
					break;
				case avpt_wchar  : 
					if ( !bArray ) {
						AVP_wchar pWChar[2] = { 0, 0 };
						PROP_Get_Val( hFromProp, pWChar, 1 );
						PROP_Set_Val( hToProp, (AVP_size_t)pWChar, 0 );
					}
					else {
						AVP_dword nCount = PROP_Arr_Count( hFromProp );
						FlushArray( hToProp );
						for ( nPos=0; nPos<nCount; nPos++ ) {
							AVP_wchar pWChar[2] = { 0, 0 };
							PROP_Arr_Get_Items( hFromProp, nPos, pWChar, 2 );
							PROP_Arr_Insert( hToProp, PROP_ARR_LAST, &pWChar[0], sizeof(AVP_wchar) );
						}
					}
					break;
				case avpt_short  : 
					if ( !bArray ) {
						AVP_short nValue;
						PROP_Get_Val( hFromProp, &nValue, sizeof(nValue) );
						PROP_Set_Val( hToProp, nValue, 0 );
					}
					else {
						AVP_dword nCount = PROP_Arr_Count( hFromProp );
						FlushArray( hToProp );
						for ( nPos=0; nPos<nCount; nPos++ ) {
							AVP_short nValue;
							PROP_Arr_Get_Items( hFromProp, nPos, &nValue, sizeof(nValue) );
							PROP_Arr_Insert( hToProp, PROP_ARR_LAST, &nValue, sizeof(nValue) );
						}
					}
					break;
				case avpt_long   : 
					if ( !bArray ) {
						AVP_long nValue;
						PROP_Get_Val( hFromProp, &nValue, sizeof(nValue) );
						PROP_Set_Val( hToProp, nValue, 0 );
					}
					else {
						AVP_dword nCount = PROP_Arr_Count( hFromProp );
						FlushArray( hToProp );
						for ( nPos=0; nPos<nCount; nPos++ ) {
							AVP_long nValue;
							PROP_Arr_Get_Items( hFromProp, nPos, &nValue, sizeof(nValue) );
							PROP_Arr_Insert( hToProp, PROP_ARR_LAST, &nValue, sizeof(nValue) );
						}
					}
					break;
				case avpt_longlong   : 
					if ( !bArray ) {
						AVP_longlong nValue;
						PROP_Get_Val( hFromProp, &nValue, sizeof(nValue) );
						PROP_Set_Val( hToProp, (AVP_size_t)&nValue, sizeof(nValue) );
					}
					else {
						AVP_dword nCount = PROP_Arr_Count( hFromProp );
						FlushArray( hToProp );
						for ( nPos=0; nPos<nCount; nPos++ ) {
							AVP_longlong nValue;
							PROP_Arr_Get_Items( hFromProp, nPos, &nValue, sizeof(nValue) );
							PROP_Arr_Insert( hToProp, PROP_ARR_LAST, &nValue, sizeof(nValue) );
						}
					}
					break;
				case avpt_byte   : 
					if ( !bArray ) {
						AVP_byte nValue;
						PROP_Get_Val( hFromProp, &nValue, sizeof(nValue) );
						PROP_Set_Val( hToProp, nValue, 0 );
					}
					else {
						AVP_dword nCount = PROP_Arr_Count( hFromProp );
						FlushArray( hToProp );
						for ( nPos=0; nPos<nCount; nPos++ ) {
							AVP_byte nValue;
							PROP_Arr_Get_Items( hFromProp, nPos, &nValue, sizeof(nValue) );
							PROP_Arr_Insert( hToProp, PROP_ARR_LAST, &nValue, sizeof(nValue) );
						}
					}
					break;
				case avpt_word   : 
					if ( !bArray ) {
						AVP_word nValue;
						PROP_Get_Val( hFromProp, &nValue, sizeof(nValue) );
						PROP_Set_Val( hToProp, nValue, 0 );
					}
					else {
						AVP_dword nCount = PROP_Arr_Count( hFromProp );
						FlushArray( hToProp );
						for ( nPos=0; nPos<nCount; nPos++ ) {
							AVP_word nValue;
							PROP_Arr_Get_Items( hFromProp, nPos, &nValue, sizeof(nValue) );
							PROP_Arr_Insert( hToProp, PROP_ARR_LAST, &nValue, sizeof(nValue) );
						}
					}
					break;
				case avpt_dword  : 
					if ( !bArray ) {
						AVP_dword nValue;
						PROP_Get_Val( hFromProp, &nValue, sizeof(nValue) );
						PROP_Set_Val( hToProp, nValue, 0 );
					}
					else {
						AVP_dword nCount = PROP_Arr_Count( hFromProp );
						FlushArray( hToProp );
						for ( nPos=0; nPos<nCount; nPos++ ) {
							AVP_dword nValue;
							PROP_Arr_Get_Items( hFromProp, nPos, &nValue, sizeof(nValue) );
							PROP_Arr_Insert( hToProp, PROP_ARR_LAST, &nValue, sizeof(nValue) );
						}
					}
					break;
				case avpt_qword  : 
					if ( !bArray ) {
						AVP_qword nValue;
						PROP_Get_Val( hFromProp, &nValue, sizeof(nValue) );
						PROP_Set_Val( hToProp, (AVP_size_t)&nValue, sizeof(nValue) );
					}
					else {
						AVP_qword nCount = PROP_Arr_Count( hFromProp );
						FlushArray( hToProp );
						for ( nPos=0; nPos<nCount; nPos++ ) {
							AVP_qword nValue;
							PROP_Arr_Get_Items( hFromProp, nPos, &nValue, sizeof(nValue) );
							PROP_Arr_Insert( hToProp, PROP_ARR_LAST, &nValue, sizeof(nValue) );
						}
					}
					break;
				case avpt_int   : 
					if ( !bArray ) {
						AVP_int nValue;
						PROP_Get_Val( hFromProp, &nValue, sizeof(nValue) );
						PROP_Set_Val( hToProp, nValue, 0 );
					}
					else {
						AVP_dword nCount = PROP_Arr_Count( hFromProp );
						FlushArray( hToProp );
						for ( nPos=0; nPos<nCount; nPos++ ) {
							AVP_int nValue;
							PROP_Arr_Get_Items( hFromProp, nPos, &nValue, sizeof(nValue) );
							PROP_Arr_Insert( hToProp, PROP_ARR_LAST, &nValue, sizeof(nValue) );
						}
					}
					break;
				case avpt_uint   : 
					if ( !bArray ) {
						AVP_uint nValue;
						PROP_Get_Val( hFromProp, &nValue, sizeof(nValue) );
						PROP_Set_Val( hToProp, nValue, 0 );
					}
					else {
						AVP_dword nCount = PROP_Arr_Count( hFromProp );
						FlushArray( hToProp );
						for ( nPos=0; nPos<nCount; nPos++ ) {
							AVP_uint nValue;
							PROP_Arr_Get_Items( hFromProp, nPos, &nValue, sizeof(nValue) );
							PROP_Arr_Insert( hToProp, PROP_ARR_LAST, &nValue, sizeof(nValue) );
						}
					}
					break;
				case avpt_bool   : 
					if ( !bArray ) {
						AVP_bool nValue;
						PROP_Get_Val( hFromProp, &nValue, sizeof(nValue) );
						PROP_Set_Val( hToProp, nValue, 0 );
					}
					else {
						AVP_dword nCount = PROP_Arr_Count( hFromProp );
						FlushArray( hToProp );
						for ( nPos=0; nPos<nCount; nPos++ ) {
							AVP_bool nValue;
							PROP_Arr_Get_Items( hFromProp, nPos, &nValue, sizeof(nValue) );
							PROP_Arr_Insert( hToProp, PROP_ARR_LAST, &nValue, sizeof(nValue) );
						}
					}
					break;
				case avpt_group  : 
					if ( !bArray ) {
						AVP_group nValue;
						PROP_Get_Val( hFromProp, &nValue, sizeof(nValue) );
						PROP_Set_Val( hToProp, nValue, 0 );
					}
					else {
						AVP_dword nCount = PROP_Arr_Count( hFromProp );
						FlushArray( hToProp );
						for ( nPos=0; nPos<nCount; nPos++ ) {
							AVP_group nValue;
							PROP_Arr_Get_Items( hFromProp, nPos, &nValue, sizeof(nValue) );
							PROP_Arr_Insert( hToProp, PROP_ARR_LAST, &nValue, sizeof(nValue) );
						}
					}
					break;
				case avpt_date   : 
					if ( !bArray ) {
						AVP_date nValue;
						PROP_Get_Val( hFromProp, &nValue, sizeof(nValue) );
						PROP_Set_Val( hToProp, (AVP_size_t)nValue, 0 );
					}
					else {
						AVP_dword nCount = PROP_Arr_Count( hFromProp );
						FlushArray( hToProp );
						for ( nPos=0; nPos<nCount; nPos++ ) {
							AVP_date nValue;
							PROP_Arr_Get_Items( hFromProp, nPos, &nValue, sizeof(nValue) );
							PROP_Arr_Insert( hToProp, PROP_ARR_LAST, &nValue, sizeof(nValue) );
						}
					}
					break;
				case avpt_time   : 
					if ( !bArray ) {
						AVP_time nValue;
						PROP_Get_Val( hFromProp, &nValue, sizeof(nValue) );
						PROP_Set_Val( hToProp, (AVP_size_t)nValue, 0 );
					}
					else {
						AVP_dword nCount = PROP_Arr_Count( hFromProp );
						FlushArray( hToProp );
						for ( nPos=0; nPos<nCount; nPos++ ) {
							AVP_time nValue;
							PROP_Arr_Get_Items( hFromProp, nPos, &nValue, sizeof(nValue) );
							PROP_Arr_Insert( hToProp, PROP_ARR_LAST, &nValue, sizeof(nValue) );
						}
					}
					break;
				case avpt_datetime   : 
					if ( !bArray ) {
						AVP_datetime nValue;
						PROP_Get_Val( hFromProp, &nValue, sizeof(nValue) );
						PROP_Set_Val( hToProp, (AVP_size_t)nValue, 0 );
					}
					else {
						AVP_dword nCount = PROP_Arr_Count( hFromProp );
						FlushArray( hToProp );
						for ( nPos=0; nPos<nCount; nPos++ ) {
							AVP_datetime nValue;
							PROP_Arr_Get_Items( hFromProp, nPos, &nValue, sizeof(nValue) );
							PROP_Arr_Insert( hToProp, PROP_ARR_LAST, &nValue, sizeof(nValue) );
						}
					}
					break;
				case avpt_str    : 
					if ( !bArray ) {
						int nSize = PROP_Get_Val( hFromProp, NULL, 0 );
						if ( nSize ) {
							AVP_char *pResult = DTUAllocator(sizeof(AVP_char) * nSize);
							PROP_Get_Val( hFromProp, pResult, nSize );
							PROP_Set_Val( hToProp, (AVP_size_t)pResult, 0 );
							DTULiberator( pResult );
						}
						else
							PROP_Set_Val( hToProp, (AVP_size_t)"", 0 );
					}
					else {
						AVP_dword nCount = PROP_Arr_Count( hFromProp );
						FlushArray( hToProp );
						for ( nPos=0; nPos<nCount; nPos++ ) {
							int nSize = PROP_Arr_Get_Items( hFromProp, nPos, 0, 0 ) + 1;
							if ( nSize ) {
								AVP_char *pResult = DTUAllocator(sizeof(AVP_char) * nSize);
								PROP_Arr_Get_Items( hFromProp, nPos, pResult, nSize );
								PROP_Arr_Insert( hToProp, PROP_ARR_LAST, pResult, 0 );
								DTULiberator( pResult );
							}
							else
								PROP_Arr_Insert( hToProp, PROP_ARR_LAST, "", 0 );
						}
					}
					break;
				case avpt_wstr   : 
					if ( !bArray ) {
						int nSize = PROP_Get_Val( hFromProp, NULL, 0 );
						if ( nSize ) {
							AVP_wchar *pWChar = DTUAllocator(sizeof(AVP_wchar) * nSize);
							PROP_Get_Val( hFromProp, pWChar, nSize );
							PROP_Set_Val( hToProp, (AVP_size_t)pWChar, 0 );
							DTULiberator( pWChar );
						}
						else {
							AVP_wchar pWStr[2] = { 0, 0 };
							PROP_Set_Val( hToProp, (AVP_size_t)pWStr, 0 );
						}
					}
					else {
						AVP_dword nCount = PROP_Arr_Count( hFromProp );
						FlushArray( hToProp );
						for ( nPos=0; nPos<nCount; nPos++ ) {
							int nSize = PROP_Arr_Get_Items( hFromProp, nPos, 0, 0 ) + 1;
							if ( nSize ) {
								AVP_wchar *pWChar = DTUAllocator(sizeof(AVP_wchar) * nSize);
								PROP_Arr_Get_Items( hFromProp, nPos, pWChar, nSize );
								PROP_Arr_Insert( hToProp, PROP_ARR_LAST, pWChar, 0 );
								DTULiberator( pWChar );
							}
							else {
								AVP_wchar pWStr[2] = { 0, 0 };
								PROP_Arr_Insert( hToProp, PROP_ARR_LAST, pWStr, 0 );
							}
						}
					}
					break;
				case avpt_bin : 
					if ( !bArray ) {
						AVP_dword nSize = PROP_Get_Val( hFromProp, NULL, 0 );
						if ( nSize ) {
							AVP_byte *pValue = DTUAllocator(sizeof(AVP_byte) * nSize);
							PROP_Get_Val( hFromProp, pValue, nSize );
							PROP_Set_Val( hToProp, (AVP_size_t)pValue, nSize );
							DTULiberator( pValue );
						}
					}
					else {
						AVP_dword nCount = PROP_Arr_Count( hFromProp );
						FlushArray( hToProp );
						for ( nPos=0; nPos<nCount; nPos++ ) {
							int nSize = PROP_Arr_Get_Items( hFromProp, nPos, 0, 0 ) + 1;
							if ( nSize ) {
								AVP_byte *pValue = DTUAllocator(sizeof(AVP_byte) * nSize);
								PROP_Arr_Get_Items( hFromProp, nPos, pValue, nSize );
								PROP_Arr_Insert( hToProp, PROP_ARR_LAST, pValue, 0 );
								DTULiberator( pValue );
							}
							else
								PROP_Arr_Insert( hToProp, PROP_ARR_LAST, 0, 0 );
						}
					}
					break;
			}
		}
		return 1;
	}
	return 0;
}



// ---
DTUT_PROC AVP_bool DTUT_PARAM DTUT_ExchangePropValueEx( HPROP hFromProp, HPROP hToProp ) {
	if ( !DTUAllocator || !DTULiberator ) {
    _RPT0( _CRT_ASSERT, "Library is not initialised" );
		return 0;
	}
	else {
		AVP_dword nPos;
		AVP_dword nFromID = PROP_Get_Id( hFromProp );
		AVP_dword nToID   = PROP_Get_Id( hToProp );
		AVP_dword nType  = GET_AVP_PID_TYPE(nFromID);
		AVP_bool  bArray = GET_AVP_PID_ARR(nFromID);
		switch ( nType ) {
			case avpt_char   : 
				if ( !bArray ) {
					if ( nFromID == nToID ) {
						AVP_char nValue;
						PROP_Get_Val( hFromProp, &nValue, sizeof(nValue) );
						PROP_Set_Val( hToProp, nValue, 0 );
					}
					else {
						TCHAR *pValue = DTUT_GetPropValueAsString( hFromProp, 0 );
						DTUT_SetPropValue( hToProp, pValue, 0 );
						DTULiberator( pValue );
					}
				}
				else {
					if ( nFromID == nToID ) {
						AVP_dword nCount = PROP_Arr_Count( hFromProp );
						FlushArray( hToProp );
						for ( nPos=0; nPos<nCount; nPos++ ) {
							AVP_char nValue;
							PROP_Arr_Get_Items( hFromProp, nPos, &nValue, sizeof(nValue) );
							PROP_Arr_Insert( hToProp, PROP_ARR_LAST, &nValue, sizeof(nValue) );
						}
					}
					else {
						AVP_dword nCount = PROP_Arr_Count( hFromProp );
						FlushArray( hToProp );
						for ( nPos=0; nPos<nCount; nPos++ ) {
							TCHAR *pValue = DTUT_GetPropArrayValueAsString( hFromProp, nPos, 0 );
							DTUT_SetPropArrayValue( hToProp, PROP_ARR_LAST, pValue, 0 );
							DTULiberator( pValue );
						}
					}
				}
				break;
			case avpt_wchar  : 
				if ( !bArray ) {
					if ( nFromID == nToID ) {
						AVP_wchar pWChar[2] = { 0, 0 };
						PROP_Get_Val( hFromProp, pWChar, 1 );
						PROP_Set_Val( hToProp, (AVP_size_t)pWChar, 0 );
					}
					else {
						TCHAR *pValue = DTUT_GetPropValueAsString( hFromProp, 0 );
						DTUT_SetPropValue( hToProp, pValue, 0 );
						DTULiberator( pValue );
					}
				}
				else {
					if ( nFromID == nToID ) {
						AVP_dword nCount = PROP_Arr_Count( hFromProp );
						FlushArray( hToProp );
						for ( nPos=0; nPos<nCount; nPos++ ) {
							AVP_wchar pWChar[2] = { 0, 0 };
							PROP_Arr_Get_Items( hFromProp, nPos, pWChar, 2 );
							PROP_Arr_Insert( hToProp, PROP_ARR_LAST, &pWChar[0], sizeof(AVP_wchar) );
						}
					}
					else {
						AVP_dword nCount = PROP_Arr_Count( hFromProp );
						FlushArray( hToProp );
						for ( nPos=0; nPos<nCount; nPos++ ) {
							TCHAR *pValue = DTUT_GetPropArrayValueAsString( hFromProp, nPos, 0 );
							DTUT_SetPropArrayValue( hToProp, PROP_ARR_LAST, pValue, 0 );
							DTULiberator( pValue );
						}
					}
				}
				break;
			case avpt_short  : 
				if ( !bArray ) {
					AVP_short nValue;
					PROP_Get_Val( hFromProp, &nValue, sizeof(nValue) );
					PROP_Set_Val( hToProp, nValue, 0 );
				}
				else {
					AVP_dword nCount = PROP_Arr_Count( hFromProp );
					FlushArray( hToProp );
					for ( nPos=0; nPos<nCount; nPos++ ) {
						AVP_short nValue;
						PROP_Arr_Get_Items( hFromProp, nPos, &nValue, sizeof(nValue) );
						PROP_Arr_Insert( hToProp, PROP_ARR_LAST, &nValue, sizeof(nValue) );
					}
				}
				break;
			case avpt_long   : 
				if ( !bArray ) {
					AVP_long nValue;
					PROP_Get_Val( hFromProp, &nValue, sizeof(nValue) );
					PROP_Set_Val( hToProp, nValue, 0 );
				}
				else {
					AVP_dword nCount = PROP_Arr_Count( hFromProp );
					FlushArray( hToProp );
					for ( nPos=0; nPos<nCount; nPos++ ) {
						AVP_long nValue;
						PROP_Arr_Get_Items( hFromProp, nPos, &nValue, sizeof(nValue) );
						PROP_Arr_Insert( hToProp, PROP_ARR_LAST, &nValue, sizeof(nValue) );
					}
				}
				break;
			case avpt_longlong   : 
				if ( !bArray ) {
					AVP_longlong nValue;
					PROP_Get_Val( hFromProp, &nValue, sizeof(nValue) );
					PROP_Set_Val( hToProp, (AVP_size_t)&nValue, sizeof(nValue) );
				}
				else {
					AVP_dword nCount = PROP_Arr_Count( hFromProp );
					FlushArray( hToProp );
					for ( nPos=0; nPos<nCount; nPos++ ) {
						AVP_longlong nValue;
						PROP_Arr_Get_Items( hFromProp, nPos, &nValue, sizeof(nValue) );
						PROP_Arr_Insert( hToProp, PROP_ARR_LAST, &nValue, sizeof(nValue) );
					}
				}
				break;
			case avpt_byte   : 
				if ( !bArray ) {
					AVP_byte nValue;
					PROP_Get_Val( hFromProp, &nValue, sizeof(nValue) );
					PROP_Set_Val( hToProp, nValue, 0 );
				}
				else {
					AVP_dword nCount = PROP_Arr_Count( hFromProp );
					FlushArray( hToProp );
					for ( nPos=0; nPos<nCount; nPos++ ) {
						AVP_byte nValue;
						PROP_Arr_Get_Items( hFromProp, nPos, &nValue, sizeof(nValue) );
						PROP_Arr_Insert( hToProp, PROP_ARR_LAST, &nValue, sizeof(nValue) );
					}
				}
				break;
			case avpt_word   : 
				if ( !bArray ) {
					AVP_word nValue;
					PROP_Get_Val( hFromProp, &nValue, sizeof(nValue) );
					PROP_Set_Val( hToProp, nValue, 0 );
				}
				else {
					AVP_dword nCount = PROP_Arr_Count( hFromProp );
					FlushArray( hToProp );
					for ( nPos=0; nPos<nCount; nPos++ ) {
						AVP_word nValue;
						PROP_Arr_Get_Items( hFromProp, nPos, &nValue, sizeof(nValue) );
						PROP_Arr_Insert( hToProp, PROP_ARR_LAST, &nValue, sizeof(nValue) );
					}
				}
				break;
			case avpt_dword  : 
				if ( !bArray ) {
					AVP_dword nValue;
					PROP_Get_Val( hFromProp, &nValue, sizeof(nValue) );
					PROP_Set_Val( hToProp, nValue, 0 );
				}
				else {
					AVP_dword nCount = PROP_Arr_Count( hFromProp );
					FlushArray( hToProp );
					for ( nPos=0; nPos<nCount; nPos++ ) {
						AVP_dword nValue;
						PROP_Arr_Get_Items( hFromProp, nPos, &nValue, sizeof(nValue) );
						PROP_Arr_Insert( hToProp, PROP_ARR_LAST, &nValue, sizeof(nValue) );
					}
				}
				break;
			case avpt_qword  : 
				if ( !bArray ) {
					AVP_qword nValue;
					PROP_Get_Val( hFromProp, &nValue, sizeof(nValue) );
					PROP_Set_Val( hToProp, (AVP_size_t)&nValue, sizeof(nValue) );
				}
				else {
					AVP_qword nCount = PROP_Arr_Count( hFromProp );
					FlushArray( hToProp );
					for ( nPos=0; nPos<nCount; nPos++ ) {
						AVP_qword nValue;
						PROP_Arr_Get_Items( hFromProp, nPos, &nValue, sizeof(nValue) );
						PROP_Arr_Insert( hToProp, PROP_ARR_LAST, &nValue, sizeof(nValue) );
					}
				}
				break;
			case avpt_int   : 
				if ( !bArray ) {
					AVP_int nValue;
					PROP_Get_Val( hFromProp, &nValue, sizeof(nValue) );
					PROP_Set_Val( hToProp, nValue, 0 );
				}
				else {
					AVP_dword nCount = PROP_Arr_Count( hFromProp );
					FlushArray( hToProp );
					for ( nPos=0; nPos<nCount; nPos++ ) {
						AVP_int nValue;
						PROP_Arr_Get_Items( hFromProp, nPos, &nValue, sizeof(nValue) );
						PROP_Arr_Insert( hToProp, PROP_ARR_LAST, &nValue, sizeof(nValue) );
					}
				}
				break;
			case avpt_uint   : 
				if ( !bArray ) {
					AVP_uint nValue;
					PROP_Get_Val( hFromProp, &nValue, sizeof(nValue) );
					PROP_Set_Val( hToProp, nValue, 0 );
				}
				else {
					AVP_dword nCount = PROP_Arr_Count( hFromProp );
					FlushArray( hToProp );
					for ( nPos=0; nPos<nCount; nPos++ ) {
						AVP_uint nValue;
						PROP_Arr_Get_Items( hFromProp, nPos, &nValue, sizeof(nValue) );
						PROP_Arr_Insert( hToProp, PROP_ARR_LAST, &nValue, sizeof(nValue) );
					}
				}
				break;
			case avpt_bool   : 
				if ( !bArray ) {
					AVP_bool nValue;
					PROP_Get_Val( hFromProp, &nValue, sizeof(nValue) );
					PROP_Set_Val( hToProp, nValue, 0 );
				}
				else {
					AVP_dword nCount = PROP_Arr_Count( hFromProp );
					FlushArray( hToProp );
					for ( nPos=0; nPos<nCount; nPos++ ) {
						AVP_bool nValue;
						PROP_Arr_Get_Items( hFromProp, nPos, &nValue, sizeof(nValue) );
						PROP_Arr_Insert( hToProp, PROP_ARR_LAST, &nValue, sizeof(nValue) );
					}
				}
				break;
			case avpt_group  : 
				if ( !bArray ) {
					AVP_group nValue;
					PROP_Get_Val( hFromProp, &nValue, sizeof(nValue) );
					PROP_Set_Val( hToProp, nValue, 0 );
				}
				else {
					AVP_dword nCount = PROP_Arr_Count( hFromProp );
					FlushArray( hToProp );
					for ( nPos=0; nPos<nCount; nPos++ ) {
						AVP_group nValue;
						PROP_Arr_Get_Items( hFromProp, nPos, &nValue, sizeof(nValue) );
						PROP_Arr_Insert( hToProp, PROP_ARR_LAST, &nValue, sizeof(nValue) );
					}
				}
				break;
			case avpt_date   : 
				if ( !bArray ) {
					AVP_date nValue;
					PROP_Get_Val( hFromProp, &nValue, sizeof(nValue) );
					PROP_Set_Val( hToProp, (AVP_size_t)nValue, 0 );
				}
				else {
					AVP_dword nCount = PROP_Arr_Count( hFromProp );
					FlushArray( hToProp );
					for ( nPos=0; nPos<nCount; nPos++ ) {
						AVP_date nValue;
						PROP_Arr_Get_Items( hFromProp, nPos, &nValue, sizeof(nValue) );
						PROP_Arr_Insert( hToProp, PROP_ARR_LAST, &nValue, sizeof(nValue) );
					}
				}
				break;
			case avpt_time   : 
				if ( !bArray ) {
					AVP_time nValue;
					PROP_Get_Val( hFromProp, &nValue, sizeof(nValue) );
					PROP_Set_Val( hToProp, (AVP_size_t)nValue, 0 );
				}
				else {
					AVP_dword nCount = PROP_Arr_Count( hFromProp );
					FlushArray( hToProp );
					for ( nPos=0; nPos<nCount; nPos++ ) {
						AVP_time nValue;
						PROP_Arr_Get_Items( hFromProp, nPos, &nValue, sizeof(nValue) );
						PROP_Arr_Insert( hToProp, PROP_ARR_LAST, &nValue, sizeof(nValue) );
					}
				}
				break;
			case avpt_datetime   : 
				if ( !bArray ) {
					AVP_datetime nValue;
					PROP_Get_Val( hFromProp, &nValue, sizeof(nValue) );
					PROP_Set_Val( hToProp, (AVP_size_t)nValue, 0 );
				}
				else {
					AVP_dword nCount = PROP_Arr_Count( hFromProp );
					FlushArray( hToProp );
					for ( nPos=0; nPos<nCount; nPos++ ) {
						AVP_datetime nValue;
						PROP_Arr_Get_Items( hFromProp, nPos, &nValue, sizeof(nValue) );
						PROP_Arr_Insert( hToProp, PROP_ARR_LAST, &nValue, sizeof(nValue) );
					}
				}
				break;
			case avpt_str    : 
				if ( !bArray ) {
					if ( nFromID == nToID ) {
						int nSize = PROP_Get_Val( hFromProp, NULL, 0 );
						if ( nSize ) {
							AVP_char *pResult = DTUAllocator(sizeof(AVP_char) * nSize);
							PROP_Get_Val( hFromProp, pResult, nSize );
							PROP_Set_Val( hToProp, (AVP_size_t)pResult, 0 );
							DTULiberator( pResult );
						}
						else
							PROP_Set_Val( hToProp, (AVP_size_t)"", 0 );
					}
					else {
						TCHAR *pValue = DTUT_GetPropValueAsString( hFromProp, 0 );
						DTUT_SetPropValue( hToProp, pValue, 0 );
						DTULiberator( pValue );
					}
				}
				else {
					if ( nFromID == nToID ) {
						AVP_dword nCount = PROP_Arr_Count( hFromProp );
						FlushArray( hToProp );
						for ( nPos=0; nPos<nCount; nPos++ ) {
							int nSize = PROP_Arr_Get_Items( hFromProp, nPos, 0, 0 ) + 1;
							if ( nSize ) {
								AVP_char *pResult = DTUAllocator(sizeof(AVP_char) * nSize);
								PROP_Arr_Get_Items( hFromProp, nPos, pResult, nSize );
								PROP_Arr_Insert( hToProp, PROP_ARR_LAST, pResult, 0 );
								DTULiberator( pResult );
							}
							else
								PROP_Arr_Insert( hToProp, PROP_ARR_LAST, "", 0 );
						}
					}
					else {
						AVP_dword nCount = PROP_Arr_Count( hFromProp );
						FlushArray( hToProp );
						for ( nPos=0; nPos<nCount; nPos++ ) {
							TCHAR *pValue = DTUT_GetPropArrayValueAsString( hFromProp, nPos, 0 );
							DTUT_SetPropArrayValue( hToProp, PROP_ARR_LAST, pValue, 0 );
							DTULiberator( pValue );
						}
					}
				}
				break;
			case avpt_wstr   : 
				if ( !bArray ) {
					if ( nFromID == nToID ) {
						int nSize = PROP_Get_Val( hFromProp, NULL, 0 );
						if ( nSize ) {
							AVP_wchar *pWChar = DTUAllocator(sizeof(AVP_wchar) * nSize);
							PROP_Get_Val( hFromProp, pWChar, nSize );
							PROP_Set_Val( hToProp, (AVP_size_t)pWChar, 0 );
							DTULiberator( pWChar );
						}
						else {
							AVP_wchar pWStr[2] = { 0, 0 };
							PROP_Set_Val( hToProp, (AVP_size_t)pWStr, 0 );
						}
					}
					else {
						TCHAR *pValue = DTUT_GetPropValueAsString( hFromProp, 0 );
						DTUT_SetPropValue( hToProp, pValue, 0 );
						DTULiberator( pValue );
					}
				}
				else {
					if ( nFromID == nToID ) {
						AVP_dword nCount = PROP_Arr_Count( hFromProp );
						FlushArray( hToProp );
						for ( nPos=0; nPos<nCount; nPos++ ) {
							int nSize = PROP_Arr_Get_Items( hFromProp, nPos, 0, 0 ) + 1;
							if ( nSize ) {
								AVP_wchar *pWChar = DTUAllocator(sizeof(AVP_wchar) * nSize);
								PROP_Arr_Get_Items( hFromProp, nPos, pWChar, nSize );
								PROP_Arr_Insert( hToProp, PROP_ARR_LAST, pWChar, 0 );
								DTULiberator( pWChar );
							}
							else {
								AVP_wchar pWStr[2] = { 0, 0 };
								PROP_Arr_Insert( hToProp, PROP_ARR_LAST, pWStr, 0 );
							}
						}
					}
					else {
						AVP_dword nCount = PROP_Arr_Count( hFromProp );
						FlushArray( hToProp );
						for ( nPos=0; nPos<nCount; nPos++ ) {
							TCHAR *pValue = DTUT_GetPropArrayValueAsString( hFromProp, nPos, 0 );
							DTUT_SetPropArrayValue( hToProp, PROP_ARR_LAST, pValue, 0 );
							DTULiberator( pValue );
						}
					}
				}
				break;
			case avpt_bin : 
				if ( !bArray ) {
					AVP_dword nSize = PROP_Get_Val( hFromProp, NULL, 0 );
					if ( nSize ) {
						AVP_byte *pValue = DTUAllocator(sizeof(AVP_byte) * nSize);
						PROP_Get_Val( hFromProp, pValue, nSize );
						PROP_Set_Val( hToProp, (AVP_size_t)pValue, nSize );
						DTULiberator( pValue );
					}
				}
				else {
					AVP_dword nCount = PROP_Arr_Count( hFromProp );
					FlushArray( hToProp );
					for ( nPos=0; nPos<nCount; nPos++ ) {
						int nSize = PROP_Arr_Get_Items( hFromProp, nPos, 0, 0 ) + 1;
						if ( nSize ) {
							AVP_byte *pValue = DTUAllocator(sizeof(AVP_byte) * nSize);
							PROP_Arr_Get_Items( hFromProp, nPos, pValue, nSize );
							PROP_Arr_Insert( hToProp, PROP_ARR_LAST, pValue, 0 );
							DTULiberator( pValue );
						}
						else
							PROP_Arr_Insert( hToProp, PROP_ARR_LAST, 0, 0 );
					}
				}
				break;
		}
		return 1;
	}
	return 0;
}



// ---
DTUT_PROC AVP_bool DTUT_PARAM DTUT_ExchangeDataPropValues( HDATA hFromData, HDATA hToData ) {
	if ( hFromData && hToData ) {
		HPROP hCurrProp;
		HPROP hFromHeadProp = DATA_Find_Prop(hFromData, 0, 0);
		HPROP hToHeadProp		= DATA_Find_Prop(hToData, 0, 0);

		DTUT_ExchangePropValue( hFromHeadProp, hToHeadProp );

		hCurrProp = DATA_Get_First_Prop( hFromData, NULL );
		while ( hCurrProp ) {
			if ( hFromHeadProp != hCurrProp ) {
				AVP_dword nPID = PROP_Get_Id( hCurrProp );
				HPROP hToProp = DATA_Find_Prop( hToData, 0, nPID );
				if ( !hToProp )	
					hToProp = DATA_Add_Prop( hToData, 0, nPID, 0, 0 );
				if ( hToProp )	
					DTUT_ExchangePropValue( hCurrProp, hToProp );
			}
			hCurrProp = PROP_Get_Next( hCurrProp );
		}
		return 1;
	}
	return 0;
}