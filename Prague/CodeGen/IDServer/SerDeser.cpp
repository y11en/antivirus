////////////////////////////////////////////////////////////////////
//
//  SerDeser.cpp
//  Serialize/Deserialize utility functions
//  AVP Prague ID Server
//  Victor Matiouchenkov [victor@avp.ru], Kaspersky Labs. 1999
//  Copyright (c) Kaspersky Labs
//
////////////////////////////////////////////////////////////////////
#include "stdafx.h"
#include <string.h>
#include <windows.h>

#include "SerDeser.h"

// ---
AVP_char *SerializeDTree( HDATA hData, AVP_dword &nSSize ) {
	nSSize = 0;
	if ( hData ) {
		AVP_Serialize_Data rcSData;
		memset( &rcSData, 0, sizeof(rcSData) );

		::DATA_Serialize( hData, NULL, &rcSData, NULL, 0, &nSSize );

		AVP_char *pBuffer = new AVP_char[nSSize];
		::DATA_Serialize( hData, NULL, &rcSData, pBuffer, nSSize, &nSSize );

		return pBuffer;
	}
	return NULL;
}

// ---
HDATA DeserializeDTree( void *pBuffer, AVP_dword nSize ) {
	if ( pBuffer && nSize ) {
		HDATA hData = NULL;
		if ( *(WORD *)pBuffer == 0xadad ) {
			AVP_Serialize_Data rcSData;
			memset( &rcSData, 0, sizeof(rcSData) );
			hData = ::DATA_Deserialize( &rcSData, pBuffer, nSize, &nSize );
		}
		return hData;
	}
	return NULL;
}

