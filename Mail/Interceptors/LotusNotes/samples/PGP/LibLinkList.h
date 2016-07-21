/*______________________________________________________________________________
	Copyright (C) 2002-2003 PGP Corporation
	All rights reserved.


	$Id: LibLinkList.h 15326 2003-06-27 08:54:16Z pryan $
______________________________________________________________________________*/

/*::: FUNCTIONAL OVERVIEW ::::::::::
Provides various linked-list handling services.

--- revision history --------
6/26/03 Version 1.2.7 Paul Ryan
+ added ef_AddListNodeUnique(), ef_listToArray()
+ added typedef definitions needed to include library

1/21/03 Version 1.2.5 Paul Ryan
+ changed name of ef_AddListNodeFifo() to ef_AddListNode() to reflect addition 
  of LIFO/FIFO input to the procedure
+ added ef_nodeInList()
+ added standard documentation

6/20/01 Version 1.2: Paul Ryan
+ added ei_verCmp() for comparing version strings

1/9/99 Version 1.0: Paul Ryan
::::::::::::::::::::::::::::::::::::*/

#ifndef Included_LibLinkList_h	/* [ */
#define Included_LibLinkList_h

#ifndef FALSE
	typedef enum {
		FALSE, 
		TRUE
	}  BOOL;
	typedef unsigned long  ULONG;
#endif //FALSE

BOOL ef_AddListNode( void *const, const BOOL, void * *const);
BOOL ef_AddListNodeUnique( void *const, const BOOL, const ULONG, BOOL *const, 
																void * *const);
void e_FreeList( void * *, const BOOL);
BOOL ef_nodeInList( const void *const, const void *);
BOOL ef_listToArray( const void *, const ULONG, void * *const, ULONG *const);


#endif /* ] Included_LibLinkList_h */
