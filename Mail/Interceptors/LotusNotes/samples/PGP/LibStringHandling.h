/*____________________________________________________________________________
	Copyright (C) 1998 Pretty Good Privacy, Inc.
	All rights reserved.


	$Id: LibStringHandling.h 3931 2000-08-11 20:20:51Z pbj $
____________________________________________________________________________*/

#ifndef Included_LibString_h	/* [ */
#define Included_LibString_h


typedef struct _StringNode	{
	char * pc;
	struct _StringNode * pt_next;
}  StringNode;


void e_FreeStringArray( char * * *const, const unsigned long);
void e_FreeStringArrayElements( char * *const, const unsigned long);
void e_FreeStringList( StringNode *);
BOOL ef_AddStringNodeFifo( const char, StringNode * *const);


#endif /* ] Included_LibString_h */
