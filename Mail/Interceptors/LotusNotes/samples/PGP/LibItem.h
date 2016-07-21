/*______________________________________________________________________________
	Copyright (C) 2002-2003 PGP Corporation
	All rights reserved.


	$Id: LibItem.h 16469 2003-08-21 17:56:49Z pryan $
______________________________________________________________________________*/

/*::: FUNCTIONAL OVERVIEW ::::::::::
Provides common Notes item-handling & -manipulation facilities.

--- revision history --------
8/21/03 Version 1.1.9 Paul Ryan
+ fixed signature error in eus_AttemptCopyTextItemContent() declaration

3/14/03 Version 1.1.8 Paul Ryan
+ added eus_OpenTextItem()
+ extended eus_TestTextListItemHasEntry() to also handle text (not text-list) 
  items
+ minor signature change to eus_CreateTextListEntryCopy()

9/6/02 Version 1.1.6 Paul Ryan
+ listing format adjustment

3/20/00 Version 1.1.5 Paul Ryan
+ added position-found and count output to ef_TextListContainsEntry(), thus 
  changing its signature
+ added eus_IsRtf(), ef_TextListEntryMatches()
+ added eus_ItemScan() utility stub
+ signature change to ef_GetNumberListEntry() to foster consistency with 
  text-list procedures

9/9/99 Version 1.1 Paul Ryan
+ signature change to eus_CreateTextListEntryCopy()
+ removed eus_GetItemContents() as not useful; use NSFItemInfo() instead

12/2/98 Version 1.0 Paul Ryan
::::::::::::::::::::::::::::::::::::*/

#ifndef Included_LibItem_h	/* [ */
#define Included_LibItem_h


extern const BLOCKID  ebid_NULLBLOCKID;


BOOL ef_TextListEntryMatches( void *const, const BOOL, const WORD, 
																const char[]);
STATUS eus_CreateTextListEntryCopy( const WORD, BYTE *const, const BOOL, 
																char * *const);
BOOL ef_GetNumberListEntry( const BYTE *, const BOOL, const WORD, 
																double *const);
BOOL ef_TextListContainsEntry( const void *const, const BOOL, const char[], 
													WORD *const, WORD *const);
STATUS eus_RemoveItem( const NOTEHANDLE, const char[], const BLOCKID);
STATUS eus_TestTextListItemHasEntry( NOTEHANDLE, char[], const char[], 
																BOOL *const);
STATUS eus_AttemptCopyTextListItemEntry( NOTEHANDLE, char[], const WORD, 
																char * *const);
STATUS eus_AttemptCopyTextItemContent( NOTEHANDLE, char[], char[], 
												BYTE * *const, DWORD *const);
STATUS LNCALLBACK eus_ItemScan( WORD, WORD, char *, WORD, void *, DWORD, 
																		void *);
STATUS eus_OpenTextItem( const NOTEHANDLE, const char[], const BYTE * *const, 
									BLOCKID *const, DWORD *const, WORD *const);


#endif /* ] Included_LibItem_h */
