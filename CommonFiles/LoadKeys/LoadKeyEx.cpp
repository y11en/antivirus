////////////////////////////////////////////////////////////////////
//
//  LoadKey.cpp
//  Definition for Load AVP key procedures
//  AVP general purposes software
//  Victor Matiouchenkov [victor@avp.ru], Kaspersky Labs. 1999
//  Copyright (c) Kaspersky Labs
//
////////////////////////////////////////////////////////////////////

#include <stdafx.h>

#include <LoadKeys\LoadKey.h>
#include <LoadKeys\LoadKeyImp.h>

// ---
BOOL LoadKeyTreeEx( const TCHAR *pszFileName, HDATA *hRootData, BOOL &bVerified, void* (*pfMemAlloc)(AVP_uint), void (*pfMemFree)(void*) ) {
  LKAllocator = pfMemAlloc;
  LKLiberator = pfMemFree;

	return LoadKeyTreeImp( pszFileName, hRootData, bVerified );
}
