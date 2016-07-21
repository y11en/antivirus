////////////////////////////////////////////////////////////////////
//
//  SerDeser.h
//  Serialize/Deserialize utility functions
//  AVP Prague ID Server
//  Victor Matiouchenkov [victor@avp.ru], Kaspersky Labs. 1999
//  Copyright (c) Kaspersky Labs
//
////////////////////////////////////////////////////////////////////

#ifndef __SERDESER_H__
#define __SERDESER_H__

#include <property/property.h>

AVP_char *SerializeDTree( HDATA hData, AVP_dword &nSSize );
HDATA DeserializeDTree( void *pBuffer, AVP_dword nSize );

#endif