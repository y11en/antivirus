/*______________________________________________________________________________
	Copyright (C) 2002-2003 PGP Corporation
	All rights reserved.


	$Id: StringHandling.h 15326 2003-06-27 08:54:16Z pryan $
______________________________________________________________________________*/

#ifndef Included_StringHandling_h	/* [ */
#define Included_StringHandling_h

//system includes
#include <stdlib.h>		//for free(), etc.
#include <tchar.h>		//for strspnp(), incorporates string.h

//local includes
#include "PstGlobals.h"
#include "LibLinkList.h"

#ifndef BYTE
typedef unsigned char  BYTE;
#endif

typedef struct _StringNode	{
	char * pc;
	struct _StringNode * pt_next;
}  StringNode;


void e_FreeStringArrayElements( char * *const, const unsigned long);
BOOL ef_ListContainsString( const char[], const StringNode *, const BOOL);


#endif /* ] Included_StringHandling_h */
