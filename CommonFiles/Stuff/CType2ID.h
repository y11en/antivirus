////////////////////////////////////////////////////////////////////
//
//  CType2ID.cpp
//  Defines the CTYPE to ID convertion function 
//  AVP general purposes stuff
//  Victor Matiouchenkov [victor@avp.ru], Kaspersky Labs. 2000
//  Copyright (c) Kaspersky Labs
//
////////////////////////////////////////////////////////////////////
#ifndef __CTYPE2ID_H__
#define __CTYPE2ID_H__


// Converts AVP Component CTYPE (see AVPRegID.h) to AVP Application ID (see AVPComID.h)
// dwCType			- value of standard AVP Component CType
// Return value - standard AVP Application ID
DWORD AvpCTypeToID( DWORD dwCType );

// Converts AVP Application ID (see AVPComID.h) tp AVP Component CTYPE (see AVPRegID.h) 
// dwAppID			- value of AVP Application ID
// Return value - standard standard AVP Component CType
DWORD AvpIDToCType( DWORD dwAppID );


#endif //__CTYPE2ID_H__