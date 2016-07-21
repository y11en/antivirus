/*______________________________________________________________________________
	Copyright (C) 2002-2003 PGP Corporation
	All rights reserved.


	$Id: LibString.h 12963 2003-03-15 00:18:21Z sdas $
______________________________________________________________________________*/

/*::: FUNCTIONAL OVERVIEW ::::::::::
Provides various string-handling services.

--- revision history --------
3/14/03 Version 1.2.1 Paul Ryan
+ added epc_strInBuf() for finding strings within memory buffers
+ changed name of ef_AddStringNodeFifo() to ef_AddStringNode() to reflect 
  addition of LIFO/FIFO input to the procedure

6/20/01 Version 1.2 Paul Ryan
+ added ei_verCmp() for comparing version strings

1/9/99 Version 1.0 Paul Ryan
::::::::::::::::::::::::::::::::::::*/

#ifndef Included_LibString_h	/* [ */
#define Included_LibString_h


#include "LibLinkList.h"


extern char epc_ALPHALWR[], epc_ALPHAUPR[], epc_NUM10[], epc_ALPHANUMLWR[], 
			epc_ALPHANUMUPR[], epc_ALPHANUM[], ec_DOT, ec_SPACE, ec_QUOTES, 
			epc_QUOTES[], ec_HYPH, ec_SEMICLN, ec_TILDE, epc_TILDE[];

typedef struct _StringNode	{
	char * pc;
	struct _StringNode * pt_next;
}  StringNode;


void e_FreeStringArray( char * * *const, const unsigned long);
void e_FreeStringArrayElements( char * *const, const unsigned long);
BOOL ef_AddStringNode( char *const, const BOOL, const BOOL, 
														StringNode * *const);
BOOL ef_AddStringNodeFifoUnique( char *const, const BOOL, const BOOL, 
														StringNode * *const);
BOOL ef_ListContainsString( const char[], const StringNode *, const BOOL);
char * epc_strtokStrict( char *const  pc, const char[]);
int ei_verCmp( const char[], const char[], int *const);
char * epc_strInBuf( const char *const, const BYTE *, size_t, const size_t);


#endif /* ] Included_LibString_h */
