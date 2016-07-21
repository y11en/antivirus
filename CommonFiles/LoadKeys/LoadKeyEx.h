#if !(defined(__LOADKEY_H__))
#define __LOADKEY_H__

////////////////////////////////////////////////////////////////////
//
//  LoadKey.cpp
//  Defines for Load AVP key procedures
//  AVP general purposes software
//  Victor Matiouchenkov [victor@avp.ru], Kaspersky Labs. 1999
//  Copyright (c) Kaspersky Labs
//
////////////////////////////////////////////////////////////////////

#include <property/property.h>
#include <AvpComID.h>
#include <loadkeys/avpkeyid.h>

#define  ID_SERIALSTR  7
#define  PID_SERIALSTR MAKE_AVP_PID(ID_SERIALSTR,AVP_APID_FIRST,avpt_str,0)

BOOL LoadKeyTree( const char* pKeyFileName, HDATA *hRootData, BOOL &bVerified );
BOOL LoadKeyTreeEx( const char* pKeyFileName, HDATA *hRootData, BOOL &bVerified, void* (*pfMemAlloc)(AVP_uint), void (*pfMemFree)(void*) );

// Return TRUE if key file processed and key tree loaded successfully

#endif