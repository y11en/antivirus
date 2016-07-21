/*______________________________________________________________________________
	Copyright (C) 2002-2003 PGP Corporation
	All rights reserved.


	$Id: ItemHandling.h 15552 2003-07-14 09:59:47Z pryan $
______________________________________________________________________________*/

#ifndef Included_ItemHandling_h	/* [ */
#define Included_ItemHandling_h

//system includes
#include <stdlib.h>		//for free(), etc.
#include <crtdbg.h>		//for _ASSERTE(), etc.

#include "NotesApiGlobals.h"
//Notes API includes
#include <nsfnote.h>
#include <textlist.h>	//for ListGetText(), etc.
#include <osmisc.h>		//for OSLoadString()
#include <osmem.h>		//for OSLock(), OSUnlockObject()
#include <oserr.h>		//for ERR_MEMORY
#include <nsferr.h>		//for ERR_ITEM_NOT_FOUND
#include <odserr.h>		//for ERR_ODS_NO_SUCH_ENTRY


BOOL ef_TextListContainsEntry( const void *const, const BOOL, const char[], 
												WORD *const, WORD *const);
STATUS eus_CreateTextListEntryCopy( const WORD, const BYTE *const, const BOOL, 
															char * *const);
BOOL ef_TextListEntryMatches( void *const, const BOOL, const WORD, 
																const char[]);

static _inline BOOL f_TextListEntryMatches( const void *const, const BOOL, 
										const WORD, const char[], const WORD);
static STATUS us_OpenTextItem( const NOTEHANDLE, const char[], 
										  const BYTE * *const, BLOCKID *const, 
										  DWORD *const, WORD *const);


#endif /* ] Included_ItemHandling_h */
