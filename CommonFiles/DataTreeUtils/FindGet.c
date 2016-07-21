////////////////////////////////////////////////////////////////////
//
//  FindGet.c
//  Get value by unique ID module
//  AVP generic purpose stuff
//  Victor Matiouchenkov [victor@avp.ru], Kaspersky Labs. 1999
//  Copyright (c) Kaspersky Labs
//
////////////////////////////////////////////////////////////////////

#include <string.h>
#include "datatreeutils/dtutils.h"
#include "avpprpid.h"
#include "AVPPort.h"

extern void* (* DTUAllocator)(AVP_uint);
extern void  (* DTULiberator)(void*);

// ---
static AVP_bool FindNodeByUniqueID( HDATA hData, void* pUserValue ) {
	HPROP hIDProp = DATA_Find_Prop( hData, NULL, PP_PID_UNIQUEID );
	if ( hIDProp ) {
		AVP_dword nID;
		PROP_Get_Val( hIDProp, &nID, sizeof(nID) );
		if ( nID == *(AVP_dword *)pUserValue )
			return 1;
	}

	return 0;
}


// ---
// Find tree node by unique id and get its value
// hStartData	 - HDATA of root node to be scanned
// nUniqueID   - value of unique id node looked for
// nDesiredPID - value of desired PID node looked for (0 - for trusted looking for)
// pValue      - pointer to buffer for value to be get
// nSize       - size of buffer
DTUT_PROC HDATA DTUT_PARAM DTUT_GetDataByUniqueID( HDATA hStartData, 
																									 AVP_dword nUniqueID, 
																									 AVP_dword nDesiredPID, 
																									 void *pValue, 
																									 AVP_dword nSize ) {

	if ( hStartData ) {
		HDATA hFoundData = DATA_First_That( hStartData, NULL, 0, FindNodeByUniqueID, &nUniqueID );
		if ( hFoundData ) {
			AVP_dword nPID = DATA_Get_Id( hFoundData, 0 );
			if ( nDesiredPID && nPID != nDesiredPID ) 
				return 0;

			if ( pValue ) {
				if ( !DATA_Get_Val(hFoundData, NULL, 0, pValue, nSize) )
					return 0;
			}

			return hFoundData;
		}
	}
	return 0;
}

// Find tree node by unique id and set its value
// hStartData	 - HDATA of root node to be scanned
// nUniqueID   - value of unique id node looked for
// nDesiredPID - value of desired PID node looked for (0 - for trusted looking for)
// nValue      - buffer with value to be set
// Return value - HDATA of node was found, or 0 - in case of node was not found
DTUT_PROC HDATA DTUT_PARAM DTUT_SetDataByUniqueID( HDATA hStartData, 
																									 AVP_dword nUniqueID, 
																									 AVP_dword nDesiredPID, 
																									 AVP_dword nValue ) {

	if ( hStartData ) {
		HDATA hFoundData = DATA_First_That( hStartData, NULL, 0, FindNodeByUniqueID, &nUniqueID );
		if ( hFoundData ) {
			AVP_dword nPID = DATA_Get_Id( hFoundData, 0 );
			if ( nDesiredPID && nPID != nDesiredPID ) 
				return 0;

			if ( !DATA_Set_Val(hFoundData, 0, 0, nValue, 0) )
				return 0;

			return hFoundData;
		}
	}
	return 0;
}

