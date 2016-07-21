#if !(defined(__LOADKEYIMP_H__))
#define __LOADKEYIMP_H__

////////////////////////////////////////////////////////////////////
//
//  LoadKeyImp.h
//  Defines for Load AVP key procedures
//  AVP general purposes software
//  Victor Matiouchenkov [victor@avp.ru], Kaspersky Labs. 1999
//  Copyright (c) Kaspersky Labs
//
////////////////////////////////////////////////////////////////////

#include <property/property.h>

extern void* (* LKAllocator)(AVP_uint);
extern void  (* LKLiberator)(void*);

BOOL LoadKeyTreeImp( const TCHAR *pszFileName, HDATA *hRootData, BOOL &bVerified );

#endif