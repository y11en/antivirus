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

#include <malloc.h>
#include <LoadKeys\LoadKey.h>
#include <LoadKeys\LoadKeyImp.h>


// ---
BOOL LoadKeyTree( const char *pszFileName, HDATA *hRootData, BOOL &bVerified ) {
  LKAllocator = malloc;
  LKLiberator = free;

	return LoadKeyTreeImp( pszFileName, hRootData, bVerified );
}


