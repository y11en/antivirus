/*______________________________________________________________________________
	Copyright (C) 2002-2003 PGP Corporation
	All rights reserved.


	$Id: ItemHandling.c 12963 2003-03-15 00:18:21Z sdas $
______________________________________________________________________________*/

/*::: MODULE OVERVIEW :::::::::::::
Provides common Notes item-handling & -manipulation facilities.

--- suggested enhancements ----
3/20/00 PR: see suggestions in text-list item-matching procedures' documentation
9/12/99 PR: For text & text-list item content copying, allow for the 
	possibility that embedded null characters may be in the content. This means 
	we need a length output on a number of functions here, à la in 
	eus_AttemptGetTextItemContent() (though still not fully effective there 
	until this enhancement is fulfilled)

--- revision history ----------
3/14/03 Version 1.1.8 Paul Ryan
+ added eus_OpenTextItem() & us_OpenTextItem() and adjusted code throughout to 
  leverage it
+ extended eus_TestTextListItemHasEntry() to also handle text (not text-list) 
  items
+ minor signature change to eus_CreateTextListEntryCopy()
+ documentation adjustment, listing format adjustment, token renaming, some 
  exception-handling improvement

9/6/02 Version 1.1.7 Paul Ryan
+ minor documentation adjustment, listing format adjustment

9/16/00 Version 1.1.6 Paul Ryan
+ minor error-handling improvement
+ listing format adjustment

3/20/00 Version 1.1.5 Paul Ryan
+ added position-found and count outputs to ef_TextListContainsEntry(), thus 
  changing its signature
+ added eus_IsRtf(), ef_TextListEntryMatches()
+ added eus_ItemScan() utility stub
+ signature change to ef_GetNumberListEntry() to foster consistency with 
  text-list procedures
+ documentation improvement, sundry minor logic enhancements

9/12/99 Version 1.1 Paul Ryan
+ logic shortening
+ documentation addition
+ signature change to eus_CreateTextListEntryCopy()
+ removed eus_GetItemContents() as not useful

12/13/98 Version 1.0 Paul Ryan
::::::::::::::::::::::::::::::::::::*/

#include "ItemHandling.h"


/** eus_CreateTextListEntryCopy( ***
Purpose is to return a copy of the textual content of a specified text-list 
item entry.

--- parameters & return ----
us_ENTRY: zero-based index to element in the text-list whose content should be 
	copied
puc_LIST: pointer to the text-list from which to extract the specified entry
f_ITEM_TYPE_PRESENT: flag specifying whether the two-byte Notes item-type 
	specifier precedes the text-list structure
ppc: Output. Pointer to the pointer variable this procedure should set to point 
	to the string buffer this procedure will allocate and populate with a copy 
	of the textual content requested by the caller. CALLER IS RESPONSIBLE for 
	freeing the allocated buffer to the operating system. If the textual 
	content is not text, or if the textual content is the null string, or if 
	the procedure is unsuccessful, no buffer will be allocated and the pointer 
	variable is guaranteed to be NULL upon return.
RETURN:
	eus_SUCCESS if no errors occurred
	eus_ERR_INVLD_ARG if an invalid parameter was passed
	ERR_MEMORY if required memory allocation failed
	the Notes API error code otherwise

--- revision history -------
3/14/03 PR
+ minor signature change to provide appropriate constness
+ minor documentation adjustment

9/6/02 PR: listing format adjustment, minor exception-handling adjustment, 
	minor documentation adjustment
9/16/00 PR: improved error-handling information
3/20/00 PR: minor documentation adjustment

9/12/99 PR
+ if entry is the null string, return a null pointer instead of allocating 
  resources for a null-string return
+ shortened logic

12/2/98 PR: created			*/
STATUS eus_CreateTextListEntryCopy( const WORD  us_ENTRY, 
									const BYTE *const  puc_LIST, 
									const BOOL  f_ITEM_TYPE_PRESENT, 
									char * *const  ppc)	{
	BYTE * puc;
	WORD  us;
	STATUS  us_err;

	if (!( puc_LIST && ppc && (f_ITEM_TYPE_PRESENT ? *(WORD *) puc_LIST == 
														TYPE_TEXT_LIST : TRUE)))
		return eus_ERR_INVLD_ARG;

	*ppc = NULL;

	if (us_err = ListGetText( (BYTE *) puc_LIST, f_ITEM_TYPE_PRESENT, us_ENTRY, 
																	&puc, &us))
		return us_err;

	if (us)	{
		if (!( *ppc = malloc( us + 1)))
			return ERR_MEMORY;
		memcpy( *ppc, puc, us);
		(*ppc)[ us] = NULL;
	}

	return eus_SUCCESS;
} //eus_CreateTextListEntryCopy(


/** ef_GetNumberListEntry( ***
Obtains a copy of the specified entry in a given number-list.

--- parameters & return ----
puc: address of the number-list to check
f_ITEM_TYPE_PRESENT: tells whether a TYPE_NUMBER_RANGE item-type header 
	precedes the structure of the number-list
US: zero-based index of the entry to copy
RETURN: TRUE if number-list entry was successfully copied; FALSE if not or if 
	the search could not be conducted due to an invalid argument

--- revision history -------
3/20/00 PR
+ signature change to foster consistency with text-list procedures
+ logic improvement
+ completed standard documentation

9/12/99 PR: logic optimization
12/2/98 PR: created			*/
BOOL ef_GetNumberListEntry( const BYTE * puc, 
							const BOOL  f_ITEM_TYPE_PRESENT, 
							const WORD  US, 
							double *const  pdbl)	{
	if (!( puc && pdbl))
		return FALSE;

	//if the purported number-list is obviously incompatible with the task 
	//	at hand, short-circuit that the requested entry could not be delivered
	if (f_ITEM_TYPE_PRESENT)	{
		if (*(WORD *) puc != TYPE_NUMBER_RANGE)
			return FALSE;
		puc += sizeof( WORD);
	}
	if (!( ((RANGE *) puc)->ListEntries > US && 
											!((RANGE *) puc)->RangeEntries))
		return FALSE;

	//copy the specified entry into the output variable
	*pdbl = *( (double *) (puc += sizeof( RANGE)) + US);

	return TRUE;
} //ef_GetNumberListEntry(


/** eus_RemoveItem( ***
Remove (delete) a specified item or items from an open note. If specified item 
is not present on the note, procedure returns success. If item-name is 
specified, all items going by that name are removed.

--- parameters & return ----
h_NOTE: handle to the open note containing the named rich-text item
pc_ITMNM: Optional if bit_ITM input provided. Address of the name of the 
	item(s) to remove. If null, procedure will use the item BLOCKID instead.
bit_ITM: Ignored if pc_ITMNM input provided. Copy of the item BLOCKID. 
	Parameter ignored if the name of the rich-text item is provided. A blank 
	BLOCKID structure is provided by the constant ebid_NULLBLOCKID.
RETURN:
	eus_SUCCESS if no error occured
	eus_ERR_INVLD_ARG if an invalid parameter was passed
	the Notes API error code otherwise

--- revision history -------
3/14/03 PR
+ minor signature changes to provide appropriate constness, logic shortening
+ minor exception-handling improvement, listing format adjustment, token 
  renaming

3/20/00 PR
+ improved safety by means of better context-checking
+ completed standard documentation

9/12/99 PR: logic optimization
12/13/98 PR: created		*/
STATUS eus_RemoveItem( const NOTEHANDLE  h_NOTE, 
						const char  pc_ITMNM[], 
						const BLOCKID  bit_ITM)	{
	WORD  us;

	if (!( h_NOTE && (pc_ITMNM && *pc_ITMNM || !ISNULLBLOCKID( bit_ITM))))
		return eus_ERR_INVLD_ARG;

	//delete the item from the note by the method implied by the passed-in 
	//	arguments
	if (pc_ITMNM && *pc_ITMNM)
		if (!NSFItemIsPresent( (NOTEHANDLE) h_NOTE, (char *) pc_ITMNM, us = 
															strlen( pc_ITMNM)))
			return eus_SUCCESS;
		else
			return NSFItemDelete( (NOTEHANDLE) h_NOTE, (char *) pc_ITMNM, us);

	return NSFItemDeleteByBLOCKID( (NOTEHANDLE) h_NOTE, bit_ITM);
} //eus_RemoveItem(


/** ef_TextListContainsEntry( ***
Tells whether a given Notes text-list contains the specified string. A 
containment test that cannot be conducted because the inputs are clearly wrong 
is treated as though the string was not found.

--- parameters & return ------
PV: address of the content to check, either a text-list LIST structure or a 
	text-item content stream
f_ITM_TYP_PRESNT: tlag telling whether an item-type header precedes the content 
	(TYPE_TEXT_LIST)
PC: address of the string to check for in the text-list
pus: Optional Output. Address of variable to receive the zero-based position in 
	the list at which a match was found. If null pointer provided, procedure 
	ignores this functionality.
pus_count: Optional Output. Address of variable to receive the one-based count 
	of the number of items in the list. If null pointer provided, procedure 
	ignores this functionality.
RETURN: TRUE if string was found in the text-list; FALSE if not or if the 
	search could not be conducted due to an invalid argument

--- suggested enhancement ----
3/20/00 PR: allow for case-insensitive testing (will require signature 
	adjustment)

--- revision history ---------
3/14/03 PR
+ minor signature adjustment to provide appropriate constness
+ documentation improvement, listing format adjustment, token renaming

3/20/00 PR
+ added optional position-found and count outputs, thus changing procedure's 
  signature
+ standard documentation
+ improved error handling

12/2/98 PR: created			*/
BOOL ef_TextListContainsEntry( const void *const  PV, 
								const BOOL  f_ITM_TYP_PRESNT, 
								const char  PC[], 
								WORD *const  pus, 
								WORD *const  pus_count)	{
	const UINT  UI = PC ? strlen( PC) : NULL;
	const WORD  us_LEN_STR = (WORD) UI;

	WORD  us_count, us;

	//if any parameters are obviously bad or if the string passed in is too 
	//	long, short-circuit with general failure
	if (!( PV && PC && UI == us_LEN_STR && (f_ITM_TYP_PRESNT ? *(WORD *) PV == 
														TYPE_TEXT_LIST : TRUE)))
		return FALSE;

	if (pus)
		*pus = NULL;
	if (pus_count)
		*pus_count = NULL;

	//for each entry in the list...
	us_count = ListGetNumEntries( (void *) PV, f_ITM_TYP_PRESNT);
	for (us = 0; us < us_count; us++)	{
		//if the entry matches the specified string, set outputs as needed and 
		//	return that the list does contain the string
		if (f_TextListEntryMatches( PV, f_ITM_TYP_PRESNT, us, PC, 
															us_LEN_STR))	{
			if (pus)
				*pus = us;
			if (pus_count)
				*pus_count = us_count;
			return TRUE;
		}
	} //for (us = 0; us < us_count

	//no entry matched the specified string, so return that the list does not 
	//	contain the string
	return FALSE;
} //ef_TextListContainsEntry(


/** ef_TextListEntryMatches( ***
Tells whether the specified entry matches a given string. A match test that 
cannot be conducted because the inputs are clearly wrong is treated as though 
the string and "entry" do not match.

--- parameters & return ------
PV: address of the text-list to check
f_ITEM_TYPE_PRESENT: tells whether a TYPE_TEXT_LIST item-type header precedes 
	the LIST structure of the text-list
US: zero-based index of list entry to test for match
PC: address of the string to test aginst the text-list entry
RETURN: TRUE if string was found in the text-list; FALSE if not or if the 
	search could not be conducted due to an invalid argument

--- suggested enhancement ----
3/20/00 PR: allow for case-insensitive testing (will require signature 
	adjustment)

--- revision history ---------
3/20/00 PR: created			*/
BOOL ef_TextListEntryMatches( void *const  PV, 
								const BOOL  f_ITEM_TYPE_PRESENT, 
								const WORD  US, 
								const char  PC[])	{
	const UINT  UI = PC ? strlen( PC) : NULL;
	const WORD  us_LEN_STR = (WORD) UI;

	if (!( PV && PC && UI == us_LEN_STR && (f_ITEM_TYPE_PRESENT ? 
									*(WORD *) PV == TYPE_TEXT_LIST : TRUE)))
		return FALSE;

	return f_TextListEntryMatches( PV, f_ITEM_TYPE_PRESENT, US, PC, 
																us_LEN_STR);
} //ef_TextListEntryMatches(


/** f_TextListEntryMatches( ***
Tells whether the specified entry matches a given string. Inlined for speed.

--- parameters & return ------
PV: address of the text-list to check
f_ITEM_TYPE_PRESENT: tells whether a TYPE_TEXT_LIST item-type header precedes 
	the LIST structure of the text-list
US: zero-based index of list entry to test for match
PC: address of the string to test aginst the text-list entry
us_LEN: Optional Input. Length of the given string to use in the test. If 
	zero, procedure will evaluate length itself.
RETURN: TRUE if string was found in the text-list; FALSE if not or if the 
	search could not be conducted due to an invalid argument

--- suggested enhancement ----
3/20/00 PR: allow for case-insensitive testing (will require signature 
	adjustment)

--- revision history ---------
3/20/00 PR: created			*/
static _inline BOOL f_TextListEntryMatches( const void *const  PV, 
											const BOOL  f_ITEM_TYPE_PRESENT, 
											const WORD  US, 
											const char  PC[], 
											const WORD  us_LEN)	{
	const UINT  UI = us_LEN ? us_LEN : PC ? strlen( PC) : NULL;
	const WORD  us_LEN_STR = (WORD) UI;

	const BYTE * puc;
	WORD  us_len;

	_ASSERTE( PV && PC && UI == us_LEN_STR && (f_ITEM_TYPE_PRESENT ? 
									*(WORD *) PV == TYPE_TEXT_LIST : TRUE));

	if (eus_SUCCESS != ListGetText( (void *) PV, f_ITEM_TYPE_PRESENT, US, 
													(BYTE **) &puc, &us_len))
		return FALSE;

	return us_len == us_LEN_STR && memcmp( puc, PC, us_LEN_STR) == ei_SAME;
} //f_TextListEntryMatches(


/** eus_TestTextListItemHasEntry( ***
Tests whether a purported text-list item contains a given entry. If the given 
item is not a text-list, procedure returns success, but entry not found.

--- parameters & return -------
h_NOTE: handle to the open note containing the named text-list item
pc_ITMNM: address of string telling the name of the text-list item
pc_ENTRY: address of the string to check for in the text list
pf_found: Output. Address of the flag variable in which to store whether the 
	given entry was found in the purported text-list item.
RETURN:
	eus_SUCCESS if no error occured
	eus_ERR_INVLD_ARG if an invalid parameter was passed
	the Notes API error code otherwise

--- suggested enhancements ----
3/20/00 PR
+ Change the pf_found output so that it optionally outputs the zero-based 
  position at which the given entry was found, or ei_NOT_FOUND if not found at 
  all. Also so that it optionally outputs the one-based count of the number of 
  items in the list. This will of course require checking that dependent code 
  is updated to support this behavioral change.
+ allow for case-insensitive testing (will require signature adjustment)

--- revision history ----------
3/14/03 PR
+ logic enhancement to allow fallback if item is actually a text item, not 
  text-list
+ minor documentation adjustment, listing format adjustment, token renaming, 
  minor exception-handling improvement

3/20/00 PR
+ standard documentation
+ minor logic adjustment, including support of signature change to 
  ef_TextListContainsEntry()

9/12/99 PR: minor logic adjustment
1/16/99 PR: created			*/
STATUS eus_TestTextListItemHasEntry( const NOTEHANDLE  h_NOTE, 
										const char  pc_ITMNM[], 
										const char  pc_ENTRY[], 
										BOOL *const  pf_found)	{
	WORD  us_typ;
	const BYTE * puc;
	DWORD  ul_len;
	BLOCKID  bid;
	STATUS  us_err;

	if (!( h_NOTE && pc_ITMNM && *pc_ITMNM && pf_found))
		return eus_ERR_INVLD_ARG;

	*pf_found = FALSE;

	//Open the specified item. If it wasn't found, short-circuit that the entry 
	//	was not found.
	if (us_err = us_OpenTextItem( h_NOTE, pc_ITMNM, &puc, &bid, &ul_len, 
																	&us_typ))
		return ERR( us_err) == ERR_ITEM_NOT_FOUND ? eus_SUCCESS : us_err;

	//if we're actually working with a text item (not text-list), fallback to 
	//	checking if its contents represents a match, and return result to caller
	if (us_typ = TYPE_TEXT)	{
		if (strlen( pc_ENTRY) == ul_len)
			*pf_found = memcmp( pc_ENTRY, puc, ul_len) == ei_SAME;
		goto cleanUpJmp;
	}

	//if we're not working with a text-list item, return that the entry was not 
	//	found
	if (us_typ != TYPE_TEXT_LIST)
		goto cleanUpJmp;

	//determine if the item contains the entry of interest
	*pf_found = ef_TextListContainsEntry( (BYTE *) puc, TRUE, pc_ENTRY, NULL, 
																		NULL);

cleanUpJmp:
	OSUnlockBlock( bid);
	return eus_SUCCESS;
} //eus_TestTextListItemHasEntry(


/** eus_OpenTextItem( ***
Opens and exposes the raw contents of a text/text-list item.

--- parameters & return ----
h_NOTE: handle to the open note containing the named text/text-list item
pc_ITMNM: address of string giving the name of the text/text-list item
ppuc: Output. Address of pointer in which to store the address of the item's 
	contents, locked in memory.
pbid: Output. Address of information structure used by Notes to reference the 
	item's _contents_. Caller is responsible for unlocking the block, owned by 
	the containing note.
pul: Optional Output. Address of variable in which to store the length of the 
	item's content. If null, the output will be suppressed.
pus: Optional Output. Address of variable in which to store the type of the 
	specified item (TYPE_TEXT or TYPE_TEXT_LIST). If null, the output will be 
	suppressed.
RETURN:
	eus_SUCCESS if no error occured
	eus_ERR_INVLD_ARG if an invalid parameter was passed
	the Notes API error code otherwise

--- revision history -------
3/14/03 PR: created			*/
STATUS eus_OpenTextItem( const NOTEHANDLE  h_NOTE, 
							const char  pc_ITMNM[], 
							const BYTE * *const  ppuc, 
							BLOCKID *const  pbid, 
							DWORD *const  pul, 
							WORD *const  pus)	{
	if (!( h_NOTE && pc_ITMNM && *pc_ITMNM && ppuc && pbid && pul))
		return eus_ERR_INVLD_ARG;

	*ppuc = NULL;
	*pbid = ebid_NULLBLOCKID;
	if (pul)
		*pul = NULL;
	if (*pus)
		*pus = NULL;

	return us_OpenTextItem( h_NOTE, pc_ITMNM, ppuc, pbid, pul, pus);
} //eus_OpenTextListItem(


/** us_OpenTextItem( ***
Opens and exposes the raw contents of a text/text-list item.

--- parameters & return ----
h_NOTE: handle to the open note containing the named text/text-list item
pc_ITMNM: address of string giving the name of the text/text-list item
ppuc: Output. Address of pointer in which to store the address of the item's 
	contents, locked in memory.
pbid: Output. Address of information structure used by Notes to reference the 
	item's _contents_. Caller is responsible for unlocking the block, owned by 
	the containing note.
pul: Optional Output. Address of variable in which to store the length of the 
	item's content. If null, the output will be suppressed.
pus: Optional Output. Address of variable in which to store the type of the 
	specified item (TYPE_TEXT or TYPE_TEXT_LIST). If null, the output will be 
	suppressed.
RETURN:
	eus_SUCCESS if no error occured
	the Notes API error code otherwise

--- revision history -------
3/14/03 PR: created			*/
static STATUS us_OpenTextItem( const NOTEHANDLE  h_NOTE, 
								const char  pc_ITMNM[], 
								const BYTE * *const  ppuc, 
								BLOCKID *const  pbid, 
								DWORD *const  pul, 
								WORD *const  pus)	{
	WORD  us_typ;
	DWORD  ul_len;
	STATUS  us_err;

	_ASSERTE( h_NOTE && pc_ITMNM && *pc_ITMNM && ppuc && pbid && pul);

	//open the specified item
	if (us_err = NSFItemInfo( (NOTEHANDLE) h_NOTE, (char *) pc_ITMNM, (WORD) 
													strlen( pc_ITMNM), NULL, 
													&us_typ, pbid, &ul_len))
		return us_err;

	//if this isn't a text/text-list item, short-circuit with failure
	if (!( us_typ == TYPE_TEXT || us_typ == TYPE_TEXT_LIST))
		return ERR_ITEM_DATATYPE;

	*ppuc = OSLockBlock( BYTE, *pbid);
	if (pul)
		*pul = ul_len;
	if (pus)
		*pus = us_typ;

	return eus_SUCCESS;
} //us_OpenTextItem(


/** eus_AttemptCopyTextListItemEntry( ***
Attempts to copy the string at the specified element of the named text-list 
item. If the copy cannot be made for obvious reasons (e.g. not a text-list 
item, no such element, null string), procedure is successful, but no copying 
occurs.

--- parameters & return ----
h_NOTE: handle to the open note containing the named text-list item
pc_ITMNM: address of the name of the text-list item
us_ENTRY: the zero-based index of the element in the text-list item to be 
	retrieved
ppc: Output. Address of the variable in which to store a pointer to a copy of 
	the string at the specified text-list element. CALLER IS RESPONSIBLE for 
	freeing the resources associated with the copied string. Variable is 
	guaranteed to be NULL if no copying (and thus resource allocation) occurred.
RETURN:
	eus_SUCCESS if no error occured
	eus_ERR_INVLD_ARG if an invalid parameter was passed
	ERR_MEMORY if required memory allocation failed
	the Notes API error code otherwise

--- revision history -------
3/14/03 PR
+ logic enhancment to take advantage of new us_OpenTextItem()
+ minor logic adjustment, token renaming, minor exception-handling improvement

9/6/02 PR: listing format adjustment, minor exception-handling adjustment
9/16/00 PR: made error-handling more informative
3/20/00 PR: minor logic adjustment

9/12/99 PR
+ minor logic adjustment; handled not-found error
+ completed standard documentation

1/22/99 PR: created			*/
STATUS eus_AttemptCopyTextListItemEntry( NOTEHANDLE  h_NOTE, 
											char  pc_ITMNM[], 
											const WORD  us_ENTRY, 
											char * *const  ppc)	{
	const BYTE * puc;
	WORD  us_typ;
	BLOCKID  bid;
	STATUS  us_err;

	if (!( h_NOTE && pc_ITMNM && *pc_ITMNM && ppc))
		return eus_ERR_INVLD_ARG;

	*ppc = NULL;

	//Open the specified item. If it wasn't found, short-circuit that the entry 
	//	was not found.
	if (us_err = us_OpenTextItem( h_NOTE, pc_ITMNM, &puc, &bid, NULL, &us_typ))
		return ERR( us_err) == ERR_ITEM_NOT_FOUND ? eus_SUCCESS : us_err;
	if (us_typ != TYPE_TEXT_LIST)
		goto cleanUpJmp;

	//make a copy of the requested entry, if possible
	if (us_err = eus_CreateTextListEntryCopy( us_ENTRY, puc, TRUE, ppc))
		if (us_err == ERR_ODS_NO_SUCH_ENTRY)
			us_err = eus_SUCCESS;

cleanUpJmp:
	OSUnlockBlock( bid);
	return us_err;
} //eus_AttemptCopyTextListItemEntry(


/** eus_IsRtf( ***
Determines whether the specified item is a Notes Rich-Text item. If item 
doesn't exist on the note, procedure returns success.

--- parameters & return ----
h_NOTE: handle to the open note containing the named rich-text item
pc_ITMNM: Optional. Address of string giving the name of the rich-text item. If 
	null, procedure will use the item BLOCKID instead.
bid_CONTENTS: Optional. Copy of the item-contents BLOCKID. Parameter ignored if 
	the name of the rich-text item is provided.
pf:	Output. Address of a boolean variable in which to store whether the 
	specified item is a rich-text item (TRUE) or not (FALSE). Guaranteed to be 
	FALSE if a Notes API error is encountered.
RETURN:
	eus_SUCCESS if no error occured
	eus_ERR_INVLD_ARG if an invalid parameter was passed
	the Notes API error code otherwise

--- revision history -------
3/14/03 PR: listing format adjustment, minor exception-handling improvement, 
	minor documentation improvement
3/20/00 PR: created			*/
STATUS eus_TestIsRtf( NOTEHANDLE  h_NOTE, 
						char pc_ITMNM[], 
						const BLOCKID  bid_CONTENTS, 
						BOOL *const  pf)	{
	STATUS  us_err;

	if (!( h_NOTE && (pc_ITMNM && *pc_ITMNM || !ISNULLBLOCKID( 
														bid_CONTENTS)) && pf))
		return eus_ERR_INVLD_ARG;

	*pf = FALSE;

	//if the name of the item was provided...
	if (pc_ITMNM && *pc_ITMNM)	{
		WORD  us;

		//determine whether it's a rich-text item
		if (us_err = NSFItemInfo( h_NOTE, pc_ITMNM, (WORD) strlen( pc_ITMNM), 
													NULL, &us, NULL, NULL))	{
			if (ERR( us_err) == ERR_ITEM_NOT_FOUND)
				return us_err;
		}else
			*pf = us == TYPE_COMPOSITE;
	//else determine whether the content specified by the content BLOCKID is a 
	//	rich-text item
	}else	{
		*pf = *OSLockBlock( WORD, bid_CONTENTS) == TYPE_COMPOSITE;
		OSUnlockBlock( bid_CONTENTS);
	} //if (pc_ITMNM && *pc_ITMNM)

	return eus_SUCCESS;
} //eus_IsRtf(


/** eus_AttemptCopyTextItemContent( ***
Attempts to copy the text content of the specified text or text-list item. 
Content contained in elements following the first element of a text-list item 
is copied only if a delimiting string is provided. If the copy cannot be made 
for obvious reasons (e.g. not a text-list item, no such element, null string), 
procedure is successful, but no copying occurs.

--- implementation NOTE -----
9/12/99 PR: "implosion" of text-list content not yet implemented, for lack of 
	time and immediate need
	3/20/00 PR: consider NSFItemConvertToText() when ready to implement this

--- suggested enhancement ---
9/12/99 PR: length (pul) output will be fully effective only once 9/12/99 
	module enhancement is implemented

--- parameters & return -----
h_NOTE: handle to the open note containing the named text-list item
pc_ITMNM: address of the name of the text/text-list item
pc_DELIMIT: Optional. Address of a string of characters specifying how a 
	text-list's elements' content should be delimited in the return content. If 
	NULL is passed, only the first element's content of a text-list will be 
	copied to the return content.
ppuc: Output. Address of the pointer variable in which to store the address of 
	the return content. The return content is guaranteed to be null terminated 
	(although the first null-terminator is not guaranteed to be the last). If 
	no copying (and so no memory allocation) occurs, output pointer is 
	guaranteed to be null. NOTE: Caller is responsible for freeing the memory 
	allocated to the copy.
pul: Optional Output. Address of the variable in which to store the length of 
	the return content. If NULL is passed, this output is suppressed.
RETURN:
	eus_SUCCESS if no error occured (no text copied is not an error)
	eus_ERR_INVLD_ARG if an invalid parameter was passed
	ERR_MEMORY if required memory allocation failed
	the Notes API error code otherwise

--- revision history --------
3/14/03 PR
+ logic enhancment to take advantage of new us_OpenTextItem()
+ minor documentation adjustment, minor exception-handling improvement

9/6/02 PR: minor documentation adjustment, listing format adjustment
3/20/00 PR: minor documentation correction & logic adjustment
9/12/99 PR: created			*/
STATUS eus_AttemptCopyTextItemContent( NOTEHANDLE  h_NOTE, 
										char  pc_ITMNM[], 
										char  pc_DELIMIT[], 
										BYTE * *const  ppuc, 
										DWORD *const  pul)	{
	const BYTE * puc;
	WORD  us_typ;
	DWORD  ul_len;
	BLOCKID  bid;
	STATUS  us_err;

	if (!( h_NOTE && pc_ITMNM && ppuc))
		return eus_ERR_INVLD_ARG;

	*ppuc = NULL;
	if (pul)
		*pul = NULL;

	//Open the specified item. If it wasn't found, short-circuit that the entry 
	//	was not found.
	if (us_err = us_OpenTextItem( h_NOTE, pc_ITMNM, &puc, &bid, &ul_len, 
																	&us_typ))
		return ERR( us_err) == ERR_ITEM_NOT_FOUND ? eus_SUCCESS : us_err;

	//if a simple text item...
	if (us_typ == TYPE_TEXT)	{
		//copy content if not blank
		if (!( *ppuc = malloc( (ul_len -= sizeof( WORD)) + 1)))
			return ERR_MEMORY;
		memcpy( *ppuc, puc + sizeof( WORD), ul_len);
		(*ppuc)[ ul_len++] = NULL;

if (pul)
*pul = ul_len;
	//else if a text-list item...
	}else if (us_typ == TYPE_TEXT_LIST)	{
		WORD  us_entries;

		//if we're not delimiting the text-list element content or if there's 
		//	only one element...
		if (!pc_DELIMIT || (us_entries = ListGetNumEntries( (BYTE *) puc, 
															TRUE)) == 1)	{
			//copy the first element's content if not blank
			if (us_err = eus_CreateTextListEntryCopy( 0, puc, TRUE, ppuc))
				goto cleanUpJmp;
		//else implode the text-list element content
		}else if (us_entries)	{
			//allocate space for the return string
			//	ul_len - sizeof( LIST) - 
			//	sizeof( WORD) * (entries + 1) + 
			//	strlen( pc_DELIMIT) * entries + 1

			//copy the first element's content

			//for each of the remaining elements...
				//append delimiting string

				//append the element's content

			//null terminate the return content
		//else we've got a blank list, so just return that the _attempt_ was 
		//	successful
		}else
			goto cleanUpJmp;
	//else the item doesn't contain text, so just return that the _attempt_ 
	//	was successful
	}else
		goto cleanUpJmp;

	//if requested, set the length output
//	if (pul)
//		*pul = us_len;

cleanUpJmp:
	OSUnlockBlock( bid);
	return us_err;
} //eus_AttemptCopyTextItemContent(


STATUS LNCALLBACK eus_ItemScan( WORD  us_spare, 
								WORD  us_flags, 
								char * pc_nm, 
								WORD  us_lenNm, 
								void * pv_val, 
								DWORD  ul_lenVal, 
								void * pv_ctx)	{
	return eus_SUCCESS;
} //eus_ItemScan(


