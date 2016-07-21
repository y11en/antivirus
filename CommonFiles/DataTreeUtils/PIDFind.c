////////////////////////////////////////////////////////////////////
//
//  PIDFind.c
//  Find unique ID among tree node children
//  AVP generic purpose stuff
//  Victor Matiouchenkov [victor@avp.ru], Kaspersky Labs. 1999
//  Copyright (c) Kaspersky Labs
//
////////////////////////////////////////////////////////////////////

#include <stdlib.h>
#include "datatreeutils/dtutils.h"


// ---
DTUT_PROC AVP_dword DTUT_PARAM DTUT_FindUniquePID( HDATA hParentData, AVP_dword nSourcePID ) {
	static AVP_bool bRandInitialized = 0;

	AVP_dword nSAPID = GET_AVP_PID_APP(nSourcePID);
	AVP_dword nSType = GET_AVP_PID_TYPE(nSourcePID);
	AVP_dword nSArr  = GET_AVP_PID_ARR(nSourcePID);

	if ( !bRandInitialized ) {
		srand( (int)hParentData );
		bRandInitialized = 1;
	}

	while( 1 ) {
		AVP_dword nID = rand() % GET_AVP_PID_ID_MAX_VALUE;
		AVP_dword nPID = MAKE_AVP_PID( nID, nSAPID, nSType, nSArr );
		MAKE_ADDR1(nAddr, nPID); 
		if ( !DATA_Find(hParentData, nAddr) )
			return nPID;
	}

	return 0;
}
