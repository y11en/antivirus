/*______________________________________________________________________________
	Copyright (C) 2002-2003 PGP Corporation
	All rights reserved.


	$Id: RichTextHandling.c 17927 2003-10-13 20:16:06Z pryan $
______________________________________________________________________________*/

/*::: MODULE OVERVIEW :::::::::::::
Encapsulates low-level functionality for handling Notes rich-text. To use this 
functionality, the developer should include the LibRichText.h header file in 
her own module.

--- suggested enhancements ---
11/10/98 PR
+ Decouple the rich-text item information members in the ItemInfo structure 
  from the linked-list implementation of stringing such items together. My 
  suggested approach is to create a new structure called something like 
  ItemNode that has two members: an ItemInfo structure and a pointer to the 
  next ItemNode in the list.
+ A currently un-addressed problem in this implementation is that the content 
  of rich-text items become locked in memory for access purposes. However the 
  items are never then unlocked until the rich-text field is discarded. This is 
  wasteful of memory resources. My suggested solution is to implement a 
  mechanism for saving and restoring state information about the memory locks 
  on rich-text content associated with a given rich-text context. An idea of 
  the direction I would take can be gleaned from the sidelined commentary in 
  the f_TestStringStart() function and in the corresponding, currently unused 
  MemLockStateInfo structure defined in the RichTextHandling.h header file. 
  Such a mechanism would provide internal, and possibly external, logic a 
  simple means with which to conserve memory resources.

11/26/98 PR
Perhaps some reference-count locking mechanism should be incorporated into the 
ItemInfo structure so that items can be locked and "unlocked" freely with no 
risk of invalidating unknown pointers which rely on the formerly locked item 
information. The current set-up poses some risks. See the in-line documentation 
in us_VirtualAppend() for a real example of a risk being taken which could be 
avoided if we had some reference-count mechanism.

I wonder... Does OSLockBlock() and OSUnlockBlock() already provide us with a 
refrence-count mechanism that we could utilize?

11/30/98 PR
The current implementation of "absolute offsets," including ActualFrontier, 
include the length of the TYPE_COMPOSITE beginning to rich-text items. Better 
to make absolute offsets completely independent of the item structure against 
which the offset is to be measured.

11/30/98 PR
Based on the logic specified in eus_ReplaceRtfSpanText(), we may be able to 
simplify matters by getting rid of semi-virtuality altogether. Since virtuality 
can always be adjusted with little performance hit if the adjustments are made 
only at the end of an item, there seems to be little need for a semi-virtuality 
that is little more than a mechanism for adjusting virtualized content at the 
end of virtuality.
	12/8/98 PR: comment
	But semi-virtuality may still be useful for streaming CD records together 
	without having to worry about item limits -- ad-hoc buffers, for example? 
	I don't really like allocating a full segment if all I need is some space 
	to string a few CD records together. But then again, I want to isolate this 
	module and allow it to be accessible from a higher level, meaning that I 
	have to give up some efficiency & fine-tuning possibilities.

	3/20/00 PR: comment
	I am becoming convinced that doing away with semi-virtuality in favor of 
	substantially extending support for ad-hoc buffers. It would be useful, for 
	instance, to be able to write _ad-hoc_ to a pre-allocated buffer for a 
	CD-record stream, and then be able to copy/append/insert such a buffer into 
	a specified virtuality context. This functionality would fit nicely into 
	the eus_InsertSubformTop() procedure, for example, with respect to its need 
	to write a particular text-structure more than once.

3/20/00 PR: The NSFItemConvertToText and NSFItemConvertValueToText APIs might 
	be a much simpler way to extract text from rich-text items than what's 
	currently implemented here. Much testing is required to find out: 
	end-of-line handling and hotspot-text handling, for prime examples.

--- revision history ---------
9/3/03 Version 1.3.3 Paul Ryan
+ fixed regression error in building template CD records, introduced in 1.3.2

7/21/03 Version 1.3.2 Paul Ryan
+ logic enhancment to eus_CursorToFormulaActionAttach() & 
  eus_CollapseToEndAttachWrap() to accommodate possible blank space within, and 
  SIG_CD_BEGIN/-END wrapping of the attachment hotspot within, 
  action-attachment hotspots like those written for the PGP plug-in
+ logic adjustment due to signature change to f_CursorToHotspotEnd()
+ logic enhancement to emulate better the Notes client in assigning 
  formula-hotspot record flags
+ token renaming, minor logic adjustment to maintain module consistency

3/14/03 Version 1.3 Paul Ryan
+ added functionality in support of Notes 6 attachment-handling adjustments
+ added functionality to assist in creation of link hotspots
+ adjustments in support of globals-initialization reörganizaiton
+ much documentation improvement and listing format adjustment, etc.

9/6/02 Version 1.2 Paul Ryan
+ added eus_CursorAdvance(), eus_InsertAttachHotspot()
+ enhanced eus_ReplaceRtfSpanText() so it outputs rich-text span marking 
  replacement
+ improved reliability of ef_CursorToAttachmentHotspot()
+ improved eus_WriteEndHotspotRecord() causing rename to eus_InsertHotspotEnd()
+ fixed bug in f_TestStringStart()
+ added documentation, generally improved consistency of code and listing format

11/14/00 Version 1.1.6: Paul Ryan
+ documentation adjustment

9/16/00 Version 1.1.5 Paul Ryan
+ finished enhancements surrounding special handling of PGP-encoded 
  attachments in a Notes message
+ minor output adjustment made to ef_CursorToAttachmentHotspot()
+ documentation improvement

8/9/00 Version 1.1.4
+ began enhancements surrounding special handling of PGP-encoded attachments 
  in a Notes message
+ minor logic adjustments

5/30/00 Version 1.1.3 Paul Ryan
+ documentation improvement

3/20/00 Version 1.1.2 Paul Ryan
+ adjustment of eus_InsertSubformTop() to allow subforms consisting of multiple 
  rich-text items to be inserted correctly
+ adjustments made in support of making the PGP Plug-In compatible with the 
  Notes R5 mail template
+ enhancements to handling of the rich-text-field information structure
+ improved initial-context checking in us_CompileActuality() such that 
  procedure fails if specified item is not in fact a rich-text field
+ Extended us_VirtualizeRestOfRtf() such that the target virtuality does not 
  have to belong to the rich-text context structure holding the source 
  actuality. The extenstion caused the signature of eus_InsertSubformTop() to 
  change as well.
+ improvement to span-alignment output of ef_CursorToAttachmentHotspot()
+ factored out functionality for locating the hotspot-end CD record 
  corresponding to a provided hotspot-begin record: f_CursorToHotspotEnd()
+ bug fix to eus_AppendAttachmentHotspot()
+ enhanced fault-tolerance in stepping through rich-text streams, adjusting 
  AdvanceCdCursor() and generating f_PushToNextItem()
+ added and leveraged throughout the utility procedure UnlockItem()
+ much documentation improvement

9/12/99 Version 1.1 Paul Ryan
+ fixed bugs in eus_ReplaceRtfWithStandardText() so that messages exceeding 64K 
  are handled properly
+ Enhanced eus_ReplaceRtfSpanText() to handle translation of PGP ASCII-Armor 
  blocks greaer than 64K in size. Process led to creation of 
  us_AppendTextToVirtuality() and concomitant adjustment of 
  eus_ReplaceRtfWithStandardText().

1/16/99 Version 1.0 Paul Ryan
::::::::::::::::::::::::::::::::::::*/

#include "RichTextHandling.h"

// #include "rsrcods.h" For 602 API

const BOOL  ei_FOREVER = TRUE;
const DWORD  eul_ERR_FAILURE = 0xFFFFFFFF;	//function failure code for 
				//	eul_GetTextSpanText(), etc.

//ensure that we can understand certain critical R5 CD structures no matter 
//	what version of the Notes C API we're compiling with
#ifndef SIG_CD_BEGIN
	#define SIG_CD_BEGIN			(221 | BYTERECORDLENGTH)
	#define SIG_CD_END				(222 | BYTERECORDLENGTH)
	typedef struct	{
		BSIG Header;		/* Signature and length of this record */
		WORD Version;		
		WORD Signature;		/* Signature of record begin is for */			
	} CDBEGINRECORD;
	typedef struct	{
		BSIG Header;		/* Signature and length of this record */
		WORD Version;		
		WORD Signature;		/* Signature of record end is for */	
	} CDENDRECORD;

	#define	SIG_CD_V5HOTSPOTBEGIN	(126 | WORDRECORDLENGTH)
	#define SIG_CD_HREF				(246 | WORDRECORDLENGTH)
	typedef struct {
		WSIG	Header;
		DWORD 	Flags;				/* one of CDRESOURCE_FLAGS_ */
		WORD 	Type;				/* one of CDRESOURCE_TYPE_ */
		WORD	ResourceClass;		/* one of CDRESOURCE_CLASS_ */
		WORD	Length1;			/* meaning depends on Type */
		WORD	ServerHintLength;		/* length of the server hint */
		WORD	FileHintLength;		/* length of the file hint */
		BYTE 	Reserved[8];
		/*	Variable length follows:
		* 	String of size ServerHintLength, a hint as to the resource's server
		* 	String of size FileHintLength, a hint as to the resource's file
		*	- if CDRESOURCE_TYPE_URL : 
		*		string of size Length1 follows, the URL.
		*	- if CDRESOURCE_TYPE_NOTELINK : 
		*		if CDRESOURCE_FLAGS_NOTELINKINLINE is NOT set in Flags then this data is
		*			WORD LinkID, index into $Links
		*			string of size Length1 follows, the anchor name (optional)
		*		if CDRESOURCE_FLAGS_NOTELINKINLINE is set in Flags then this data is
		*			NOTELINK NoteLink
		*			string of size Length1 follows, the anchor name (optional)
		*	- if CDRESOURCE_TYPE_NAMEDELEMENT :
		*		TIMEDATE ReplicaID;		if element is in a different database - zero if current db
		*		string of size Length1 follows, the name of element
		*/
	} CDRESOURCE;
	#define HOTSPOTREC_TYPE_RCLINK			32	/* Not a new type, but renamed ...*/
	#define CDRESOURCE_TYPE_URL				1

	#define SIG_CD_COLOR			(210 | BYTERECORDLENGTH)
	typedef struct {
		WORD Flags;
		BYTE Component1;
		BYTE Component2;
		BYTE Component3;
		BYTE Component4;

		/* RGB color space 
		*	Component1 = red;
		*	Component2 = green;
		*	Component3 = blue;
		*	Component4 = unused;
		*/

		/*	Variable length data follows:
		*
		*
		* If COLOR_VALUE_FLAGS_HASGRADIENT is set then:
		*
		* 	COLOR_VALUE gradient_i follows
		*/

	} COLOR_VALUE;
	typedef struct
		{
		BSIG Header;		/* Signature and length of this record */
		COLOR_VALUE Color;
		} CDCOLOR;
	#define COLOR_VALUE_FLAGS_ISRGB			0x0001	/* Color space is RGB */
	#define COLOR_VALUE_FLAGS_SYSTEMCOLOR	0x0008	/* Use system default color, ignore color here */
#endif //ndef SIG_CD_BEGIN

#define muc_SIGTYPE_LSIG  '\0'
#define muc_SIGTYPE_WSIG  (BYTE) '\xFF'	//for some reason '\xFF' by itself 
				//	produces a maxed-out 'int', all F's
#define muc_SIG_CDHOTSPOTEND  LOBYTE( SIG_CD_HOTSPOTEND)
#define muc_SIG_CDPARAGRAPH  LOBYTE( SIG_CD_PARAGRAPH)
#define muc_SIG_CDBEGIN  LOBYTE( SIG_CD_BEGIN)
#define muc_SIG_CDHOTSPOTBEGIN  LOBYTE( SIG_CD_HOTSPOTBEGIN)
#define muc_SIG_CDEND  LOBYTE( SIG_CD_END)
#define muc_SIG_CDV4HOTSPOTEND  LOBYTE( SIG_CD_V4HOTSPOTEND)
#define muc_SIG_CDV4HOTSPOTBEGIN  LOBYTE( SIG_CD_V4HOTSPOTBEGIN)
static const BYTE  muc_SIG_CDTEXT = LOBYTE( SIG_CD_TEXT);
static const char mc_LINEFEED_RICHTEXT = '\0';
static const DWORD  mul_MAXLEN_RICH_TEXT_ITEM = MAXONESEGSIZE, 
					mul_LMT_COMFY = 0x0400 * 40, 
					mul_LMT_STRETCH = 0x0400 * 55, 
					mul_FRONTIER_NO_MORE = 0xFFFFFFFF;
static const WORD  mus_MAX_STYLE_ID = 0xFFFF, 
					mus_LENLIMIT_CDTEXT_REC = 20 * 0x400, 
					mus_HOTSPT_FILE_WEIRD = 10;
static const UINT  mui_HOTSPT_ACTN_FRMULA_FLGS = HOTSPOTREC_RUNFLAG_BOX | 
												HOTSPOTREC_RUNFLAG_NOBORDER | 
												HOTSPOTREC_RUNFLAG_FORMULA | 
												HOTSPOTREC_RUNFLAG_SIGNED;
static const CDHOTSPOTEND  mt_HOTSPOT_END = {{SIG_CD_HOTSPOTEND, 
														sizeof( CDHOTSPOTEND)}};
static const CDPARAGRAPH  mt_PARAGRAPH = {{SIG_CD_PARAGRAPH, 
														sizeof( CDPARAGRAPH)}};
static const CDBEGINRECORD  mt_R5_BEGIN_HOTSPOT = {{SIG_CD_BEGIN, 
													sizeof( CDBEGINRECORD)}, 0, 
													SIG_CD_V4HOTSPOTBEGIN};
static const CDENDRECORD  mt_R5_END_HOTSPOT = {{SIG_CD_END, sizeof( 
															CDENDRECORD)}, 0, 
															SIG_CD_V4HOTSPOTEND};
static const CDHOTSPOTEND  mt_V4HOTSPOT_END = {{SIG_CD_V4HOTSPOTEND, 
														sizeof( CDHOTSPOTEND)}};


/** eus_InitializeRtfContext( ***
Initialize a rich-text-field context structure the caller needs to use to 
employ the functionality of this module for handling Lotus Notes rich text. The 
caller chooses whether to engage a complete initialization based on an existing 
field or to simply initialize a context with which free-form rich-text content 
may be constructed. The caller is responsible for disposal of the resources 
tracked by the rich-text context structure, via a call to ef_FreeRtfContext().

--- parameters & return ----
h_NOTE: handle to the open note containing the rich-text field which should be 
	described by this procedure
pc_ITMNM: address of the string telling the name of the rich-text field to be 
	described by this procedure
pt_crsr: Optional Output. Address of the variable in which to store a 
	rich-text cursor pointing to the first CD record in the rich-text field 
	to be manipulated. Ignored if passed in null.
pt_ctx: Output. Address of the context structure to be initialized to describe 
	the specified field. This context structure is required input to most of 
	the public interfaces to this module and so must be maintained by the 
	calling procedure.
RETURN: eus_SUCCESS if no error occured. !eus_SUCCESS if any parameters are 
	invalid. The Notes API error code otherwise. The procedure will always be 
	successful if the context structure cannot be initialized to describe an 
	existing rich-text field.

--- revision history -------
3/14/03 PR
+ minor signature adjustment to provide appropriate constness
+ listing format adjustment, minor documentation adjustment, token renaming

3/20/00 PR: documentation improvement
2/11/99 PR: created			*/
STATUS eus_InitializeRtfContext( const NOTEHANDLE  h_NOTE, 
									const char  pc_ITMNM[], 
									CdRecordCursor *const  pt_crsr, 
									RtfTrackingInfo *const  pt_ctx)	{
	ItemInfo *  pt_item, * pt_itm;
	UINT  ui_blankItms = NULL;
	STATUS  us_err;

	if (!( h_NOTE && pc_ITMNM && pt_ctx))
		return !eus_SUCCESS;

	memset( pt_ctx, NULL, sizeof( RtfTrackingInfo));
	if (pt_crsr)
		memset( pt_crsr, NULL, sizeof( CdRecordCursor));

	//Fill out the context's Actuality list. If the specified rich-text field 
	//	doesn't exist, short-circuit with success.
	if (us_err = us_CompileActuality( h_NOTE, pc_ITMNM, &pt_item))
		return us_err;
	if (!pt_item)
		return eus_SUCCESS;

	//initialize the cursor to point to the beginning of the first rich-text 
	//	item's content
	if (pt_crsr)
		if (!f_ConstructStartCursor( NULL, pt_item, pt_crsr, NULL))
			goto errJump;

	pt_itm = pt_item;
	while (pt_itm->ul_length == sizeof( WORD))	{
		ui_blankItms++;
		if (!( pt_itm = pt_itm->pt_next))
			goto errJump;
	}

	pt_ctx->pt_Actuality = pt_item;
	pt_ctx->ul_ActualFrontier = sizeof( WORD) * (ui_blankItms + 1);

	return eus_SUCCESS;

errJump:
	ClearItemList( &pt_item);

	return !eus_SUCCESS;
} //eus_InitializeRtfContext(


/** eus_ActionHotspotJustBefore( ***
Tells whether a begin-action-hotspot CD record immediately precedes the CD 
record pointed to by caller.

--- parameters & return ----
pt_CRSR: address of structure telling the location of the CD record whose 
	predecessor if of interest
pt_CTX: address of structure describing the the rich-text environment we're 
	operating with
pf: Output. Address of flag variable in which to store whether a 
	begin-action-hotspot precedes (TRUE) or not (FALSE).
RETURN: 
	eus_ERR_INVLD_ARG if any passed-in parameter is obviously invalid
	eus_SUCCESS if no error occured
	Notes API error code otherwise

--- revision history -------
9/6/02 PR: created			*/
STATUS eus_ActionHotspotJustBefore( const CdRecordCursor *const  pt_CRSR, 
									const RtfTrackingInfo *const  pt_CTX, 
									BOOL *const  pf)	{
	CdRecordCursor  t_crsr;
	BOOL  f_itmLockd;
	CDHOTSPOTBEGIN * pt;
	STATUS  us_err;

	if (!( pt_CRSR && pt_CRSR->puc_location && pt_CRSR->pt_item && pt_CTX && 
																			pf))
		return eus_ERR_INVLD_ARG;

	*pf = FALSE;

	//if the immediately previous CD record is the start of an action hotspot, 
	//	tell caller so
	t_crsr = *pt_CRSR;
	if (us_err = us_RetreatCursor( &t_crsr, pt_CTX, &f_itmLockd))
		return us_err;
	if (*t_crsr.puc_location == muc_SIG_CDV4HOTSPOTBEGIN && 
						(pt = (CDHOTSPOTBEGIN *) t_crsr.puc_location)->Type == 
						HOTSPOTREC_TYPE_HOTREGION && pt->Flags & 
						HOTSPOTREC_RUNFLAG_BEGIN && pt->Flags & 
						HOTSPOTREC_RUNFLAG_ACTION | HOTSPOTREC_RUNFLAG_SCRIPT | 
						HOTSPOTREC_RUNFLAG_FORMULA)
		*pf = TRUE;

	//if necessary, put memory resources back the way they were
	if (f_itmLockd)
		UnlockItem( t_crsr.pt_item);

	return eus_SUCCESS;
} //eus_ActionHotspotJustBefore(


/** us_RetreatCursor( ***
Moves the given rich-text cursor one CD record back, taking into account the 
rich-text field environment. Note that making a cursor go backwards is much 
more processing expensive than making it go forward.

--- parameters & return ----
pt_crsr: Input & Output. Address of the cursor to be backed up. Output is 
	either a backed-up cursor or a cursor whose item pointer is nullified to 
	indicate that no previous CD records are available. If cursor could not be 
	retreated, its pt_item member is guaranteed to be null.
pt_CTX: Optional Input. Address of a structure describing the rich-text 
	environment being navigated. If null, the cursor cannot be backed up to a 
	previous item, nor will virtuality be taken into account.
pf_itmLockd: Optional. Address of the flag in which to store whether a 
	rich-text item was newly locked in the course of this procedure. If null, 
	the procedure will ignore this functionality.
RETURN: !eus_SUCCESS if an error occurred in determining the boundary between 
	actuality and virtuality; else eus_SUCCESS

--- side effect ------------
The rich-text environment information pointed to by pt_crsr or pt_CTX may be 
adjusted due to cursor advancement.

--- revision history -------
3/14/03 PR
+ fixed logic error during reanalysis of decision tree where retreat on virtual 
  items might not succeed
+ documentation adjustment

9/6/02 PR: created			*/
static STATUS us_RetreatCursor( CdRecordCursor *const  pt_crsr, 
								const RtfTrackingInfo *const  pt_CTX, 
								BOOL *const  pf_itmLockd)	{
	ItemInfo * pt_itm;
	BOOL  f_itmLockd = FALSE;
	CdRecordCursor  t_crsr = {NULL, NULL, NULL};
	STATUS  us_err = eus_SUCCESS;

	_ASSERTE( pt_crsr && pt_crsr->puc_location && pt_crsr->pt_item && 
												pt_crsr->pt_item->puc_start);

	if (pf_itmLockd)
		*pf_itmLockd = FALSE;

	//if the item holding the pointed-to location is virutal or if there's no 
	//	virtuality involved in the given context...
	if (!( pt_crsr->pt_item->i_type == mi_ACTUAL && pt_CTX->pt_endVirtual))
		//if beginning of the item precedes the pointed-to location...
		if (pt_crsr->pt_item->puc_start < pt_crsr->puc_location)
			//move from there to the preceding CD record
			CreateCursorAtRcrdPrior( pt_crsr->pt_item, NULL, 
										pt_crsr->puc_location, &t_crsr, NULL);
		//else the preceding CD record is to be found on a preceding item...
		else	{
			//if no item precedes the item containing the pointed-to location, 
			//	short-circuit indicating no preceding CD record could be found
			pt_itm = pt_crsr->pt_item->i_type == mi_ACTUAL ? 
														pt_CTX->pt_Actuality : 
														pt_CTX->pt_Virtuality;
			while (pt_itm)	{
				if (pt_itm->pt_next == pt_crsr->pt_item)
					break;
				pt_itm = pt_itm->pt_next;
			}
			if (!pt_itm)
				goto errJump;

			//locate the last CD record in the preceding item
			CreateCursorAtRcrdPrior( pt_itm, NULL, NULL, &t_crsr, &f_itmLockd);
		} //if (pt_crsr->pt_item->puc_start < pt_crsr->puc_location)
	//else we're working in a context where the pointed-to location resides in 
	//	actuality and the rich-text context involves virtuality...
	else	{
		const BYTE * puc;

		//if the actual location which virtuality is currently flowing into 
		//	(the "actual frontier") is on the same rich-text item as the 
		//	pointed-to location...
		if (!f_LocateAbsoluteOffset( pt_CTX->ul_ActualFrontier, 
												pt_CTX->pt_Actuality, &pt_itm, 
												&puc, NULL, &f_itmLockd))	{
			us_err = !eus_SUCCESS;
			goto errJump;
		}
		if (pt_itm == pt_crsr->pt_item)	{
			//if the pointed-to location is the same as the actual frontier...
			_ASSERTE( !f_itmLockd);
			if (puc == pt_crsr->puc_location)	{
				//move to the last virtualized CD record
				CreateCursorAtRcrdPrior( pt_CTX->pt_endVirtual, NULL, NULL, 
														&t_crsr, &f_itmLockd);
			//else if the pointed-to location follows the actual frontier...
			}else if (puc < pt_crsr->puc_location)
				//starting from the frontier, move to the preceding CD record
				CreateCursorAtRcrdPrior( pt_crsr->pt_item, puc, 
										pt_crsr->puc_location, &t_crsr, NULL);			
			//Else the pointed-to location is, oddly, within the actuality 
			//	slated to be overwritten by the current virtuality. If the 
			//	beginning of the item holding the pointed-to location precedes 
			//	the pointed-to location...
			else if (pt_itm->puc_start < pt_crsr->puc_location)
				//move from there to the preceding CD record
				CreateCursorAtRcrdPrior( pt_itm, NULL, pt_crsr->puc_location, 
																&t_crsr, NULL);
			//else the preceding CD record is to be found on a preceding 
			//	actual item...
			else	{
				//if no item precedes the item holding the pointed-to location, 
				//	short-circuit indicating no preceding CD record could be 
				//	found
				pt_itm = pt_CTX->pt_Actuality;
				while (pt_itm)	{
					if (pt_itm->pt_next == pt_crsr->pt_item)
						break;
					pt_itm = pt_itm->pt_next;
				}
				if (!pt_itm)
					goto errJump;

				//locate the last CD record in the preceding item
				CreateCursorAtRcrdPrior( pt_itm, NULL, NULL, &t_crsr, 
																&f_itmLockd);
			} //if (puc == pt_crsr->puc_location)
		//else if the beginning of the item holding the pointed-to location 
		//	precedes the pointed-to location...
		}else if (pt_crsr->pt_item->puc_start < pt_crsr->puc_location)
			//move from there to the preceding CD record
			CreateCursorAtRcrdPrior( pt_crsr->pt_item, NULL, 
										pt_crsr->puc_location, &t_crsr, NULL);
		//else the preceding actual item will hold the preceding CD record...
		else	{
			//if no item precedes the item containing the pointed-to location, 
			//	short-circuit indicating no preceding CD record could be found
			ItemInfo * pt_item = pt_CTX->pt_Actuality;
			while (pt_item)	{
				if (pt_item->pt_next == pt_crsr->pt_item)
					break;
				pt_item = pt_item->pt_next;
			}
			if (!pt_item)
				goto errJump;

			//if the preceding item is the same item the actual frontier is 
			//	located on...
			if (pt_item == pt_itm)
				//starting from the actual frontier, locate the last CD record 
				//	in the item
				CreateCursorAtRcrdPrior( pt_crsr->pt_item, puc, 
										pt_crsr->puc_location, &t_crsr, NULL);
			//else locate the last CD record in the preceding item
			else
				CreateCursorAtRcrdPrior( pt_item, NULL, NULL, &t_crsr, 
																&f_itmLockd);
		} //if (pt_itm == pt_crsr->pt_item)
	} //if (!( pt_crsr->pt_item->i_type == mi_ACTUAL &&

	//give caller the results
	*pt_crsr = t_crsr;
	if (pf_itmLockd && f_itmLockd)
		*pf_itmLockd = TRUE;

	return eus_SUCCESS;

errJump:
	//signify to caller that no immediately previous CD record was located, and 
	//	whether an error occurred
	pt_crsr->pt_item = NULL;
	return us_err;
} //us_RetreatCursor(


/** CreateCursorAtRcrdPrior( ***
Provided that all inputs relate to a single rich-text item, manufactures a 
rich-text cursor at the CD record immediately preceding a given CD record.

--- parameters ----------
pt_itm: Input & Output. Address of structure describing the rich-text item to 
	be operated on. If item content must be locked in order to be accessed, the 
	structure will be updated to reflect the event.
puc_STRT: Optional. Address of CD record at which the rich-text traverse will 
	begin. If null, traverse will begin at the start of the rich-text item.
puc_END: Optional. Address of CD record that immediately follows the CD record 
	for which a rich-text cursor is to be manufactured. If null, the rich-text 
	cursor will be manufactured to point to the last CD record in the rich-text 
	item.
pt_crsr: Output. Address of the rich-text cursor structure to be set to point 
	to the "immediately preceding" CD record.
pf_itmLockd: Optional Output. Address of the flag in which to store whether the 
	rich-text item provided via the pt_itm input had to be locked in order for 
	its contents to be accessed. If null, this output will be ignored.

--- revision history ----
3/14/03 PR: minor logic adjustment to get rid of pragmas
9/6/02 PR: created			*/
static void CreateCursorAtRcrdPrior( ItemInfo *const  pt_itm, 
										const BYTE *const  puc_STRT, 
										const BYTE *const  puc_END, 
										CdRecordCursor *const  pt_crsr, 
										BOOL *const  pf_itmLockd)	{
	const BYTE * puc_end = puc_END, * puc, * puc_;
	BOOL  f_itmLockd = FALSE;
	USHORT  us;

	_ASSERTE( pt_itm && pt_crsr && (puc_STRT || puc_END ? pt_itm->puc_start && 
									(puc_STRT ? pt_itm->puc_start <= 
									puc_STRT && puc_STRT < pt_itm->puc_start + 
									pt_itm->ul_length  : TRUE) && (puc_END ? 
									pt_itm->puc_start <= puc_END && puc_END < 
									pt_itm->puc_start + pt_itm->ul_length : 
									TRUE) && (puc_STRT && puc_END ? puc_STRT < 
									puc_END : TRUE) : TRUE));

	memset( pt_crsr, NULL, sizeof( CdRecordCursor));
	if (pf_itmLockd)
		*pf_itmLockd = FALSE;

	//if the end-boundary isn't known yet, determine it
	if (!puc_end)	{
		if (!pt_itm->puc_start)	{
			pt_itm->puc_start = OSLockBlock( BYTE, pt_itm->bid_contents);
			f_itmLockd = TRUE;
		}
		puc_end = pt_itm->puc_start + pt_itm->ul_length;
	} //if (!puc_end)

	//locate the preceding CD record
	us = us_getCdRecLength( puc = puc_ = puc_STRT ? puc_STRT : 
											pt_itm->puc_start + sizeof( WORD));
	_ASSERTE( us);
	while ((puc_ += us + us % 2) < puc_end)	{
		puc = puc_;
		us = us_getCdRecLength( puc);
	}

	//output result
//#pragma warning( disable : 4090)
	pt_crsr->puc_location = (BYTE *) puc; //suppressing "different 'const' qualifiers" compiler warning
//#pragma warning( default : 4090)
	pt_crsr->pt_item = pt_itm;
	pt_crsr->us_recLength = us;
	if (f_itmLockd && pf_itmLockd)
		*pf_itmLockd = TRUE;
} //CreateCursorAtRcrdPrior(


/** ef_CursorToAttachmentHotspot( ***
Find and provide information about the next file-attachment in the rich-text 
field being navigated by the passed-in cursor.

--- parameters & return ----
pt_crsr: Input & Output. Address of the rich-text cursor positioned to the 
	point at which the search for the next file-attachment hotspot should 
	commence. This cursor is then used when advancing through CD records. If 
	the procedure is successful, the cursor will finish at the hotspot-end CD 
	record of a found file-attachment hotspot, else it will be nullified 
	because it will have traveled through the entire available rich-text field.
pt_CTX: Optional. Address of environment information about the rich-text field 
	being navigated. If passed in null, the rich-text cursor being used will be 
	unable to move from virtuality to actuality.
f_BUMP_FWD_1ST: Flag telling whether the passed-in starting cursor should be 
	moved to the next CD record before initiating the search for 
	file-attachment hotspots. Usually when starting at the beginning of the 
	rich-text field, the flag will be set to FALSE so that the first CD record 
	may be checked as to whether it's a file-attachment hotspot. Ensuing calls 
	might then set the flag to TRUE to move to the next CD record since the 
	current CD record (an end-hotspot record) is already known.
pt_spn: Optional Output. Address of the structure to hold information about 
	where a found file-attachment hotspot begins and ends within the rich-text 
	field. The "begins" member will point to the hotspot-begin CD record; the 
	"ends" member will point to the hotspot-end CD record. Feature is ignored 
	if the pointer passed in is null.
ppc_fileNm: Optional Output. Address of the string pointer to be set to the 
	name of the file the user attached to the note. If null, output is 
	suppressed. The pointed-to memory is owned by the note, so the caller 
	should not free the addressed memory.
ppc_objNm: Optional Output. Address of the string pointer to be set to the name 
	Notes uses internally for the file attachment associated with the 
	attachment hotspot. If null, output is suppressed. The pointed-to memory is 
	owned by the note, so the caller should not free the addressed memory.
RETURN: TRUE if no error situation was encountered; FALSE otherwise.

--- side effect -------------
The rich-text environment information pointed to through pt_crsr or pt_CTX may 
be adjusted due to cursor advancement. If the enhancement suggested below is 
implemented, this side effect would be eliminated.

--- suggested enhancement ----
9/6/02 PR: change return to a STATUS so eus_ERR_INVLD_ARG can be returned
11/10/98 PR: The starting memory-lock state should be restored upon function 
	exit, but no good mechanism is yet available with which to accomplish this. 
	See this module's suggested-enhancement note for a description of the 
	problem and proposed solution.

--- revision history ---------
7/21/03 PR
+ minor logic adjustment due to signature change to f_CursorToHotspotEnd()
+ global token renaming

3/14/03 PR
+ logic adjustment fixing bug in handling of filename vs. object name
+ documentation improvement, token renaming

9/6/02 PR
+ fixed logic error where pt_crsr output was not set properly if a pt_spn 
  output was requested
+ listing format adjustment, token renaming

9/16/00 PR
+ allowed a return of success if given cursor seems to already be at the end of 
  all rich-text content
+ token renaming

3/20/00 PR
+ Adjusted "span" output so that the "end" member points to the CD record 
  immediately following the attachment's hotspot-end CD record. Done to 
  facilitate a potential ensuing call to remove the attachment hotspot 
  altogether.
+ factored out functionality for locating the hotspot-end CD record 
  corresponding to a provided hotspot-begin record: f_CursorToHotspotEnd()
+ documentation adjustment

12/11/98 PR: created		*/
BOOL ef_CursorToAttachmentHotspot( CdRecordCursor *const  pt_crsr, 
									const RtfTrackingInfo *const  pt_CTX, 
									const BOOL  f_BUMP_FWD_1ST, 
									CdRecordSpan *const  pt_spn, 
									const char * *const  ppc_fileNm, 
									const char * *const  ppc_objNm)	{
	WORD  us;

	if (!( pt_crsr->puc_location && pt_crsr->us_recLength && (pt_CTX ? (BOOL) 
												pt_CTX->pt_Actuality : TRUE)))
		return FALSE;

	//as applicable, default the passed-in span structure to all null and the 
	//	file-name pointers to null
	if (pt_spn)
		memset( pt_spn, NULL, sizeof( CdRecordSpan));
	if (ppc_fileNm)
		*ppc_fileNm = NULL;
	if (ppc_objNm)
		*ppc_objNm = NULL;

	//if cursor seems to already be at the end of all content, return that the 
	//	procedure has run error free, but indicating that no attachment hotspot 
	//	was found
	if (!pt_crsr->pt_item)	{
		pt_crsr->puc_location = NULL;
		return TRUE;
	}

	//If requested, do an intial advance of the cursor to the next CD record. 
	//	If the rich-text field has been exhausted, return that the procedure 
	//	has run error free, but flag that no file-attachment hotspot was found.
	if (f_BUMP_FWD_1ST)	{
		AdvanceCdCursor( pt_crsr, pt_CTX, NULL, NULL);
		if (!pt_crsr->pt_item)	{
			pt_crsr->puc_location = NULL;
			return TRUE;
		}
	}

	//Loop record-by-record until the beginning of a file-attachment hotspot is 
	//	found. The "weird" file-hotspot types occurs when an attachment is 
	//	copied to a new document via the "inherit entire selected document into 
	//	rich text field" form property [and maybe via @Command( [MailForward]) 
	//	too?].
	while (!( muc_SIG_CDHOTSPOTBEGIN == *pt_crsr->puc_location && 
											((us = ((CDHOTSPOTBEGIN *) 
											pt_crsr->puc_location)->Type) == 
											HOTSPOTREC_TYPE_FILE || us == 
											mus_HOTSPT_FILE_WEIRD)))	{
		//Advance the cursor to the next CD record. If the rich-text field has 
		//	been exhausted, return that the procedure has run error free, but 
		//	flag that no file-attachment hotspot was found.
		AdvanceCdCursor( pt_crsr, pt_CTX, NULL, NULL);
		if (!pt_crsr->pt_item)	{
			pt_crsr->puc_location = NULL;
			return TRUE;
		}
	} //while (!( muc_SIG_CDHOTSPOTBEGIN ==

	//as applicable, set the span's beginning-coordinate information and set 
	//	the filename pointers
	if (pt_spn)
		pt_spn->t_crsrBgin = *pt_crsr;
	if (ppc_objNm)
		*ppc_objNm = pt_crsr->puc_location + sizeof( CDHOTSPOTBEGIN);
	if (ppc_fileNm)
		*ppc_fileNm = pt_crsr->puc_location + sizeof( CDHOTSPOTBEGIN) + 
												strlen( pt_crsr->puc_location + 
												sizeof( CDHOTSPOTBEGIN)) + 1;

	//locate the end of the hotspot
	if (!f_CursorToHotspotEnd( muc_SIG_CDHOTSPOTEND, pt_crsr, pt_CTX, TRUE, 
															NULL, NULL))	{
		pt_crsr->puc_location = NULL;
		return FALSE;
	}

	//set the span's ending-coordinate information to point to the end-hotspot 
	//	CD record
	if (pt_spn)
		pt_spn->t_crsrEnd = *pt_crsr;

	return TRUE;
} //ef_CursorToAttachmentHotspot(


/** eus_CursorToFormulaActionAttach( ***
Find and provide information about the next "action-attach" hotspot span 
located in the rich-text field being navigated by the passed-in cursor. 
"Action-attach" in this context means a formula-action hotspot with an 
attachment hotspot embedded immediately (for all intents and purposes) _within_ 
the formula-action hotspot.

--- parameters & return ----
pt_crsr: Input & Output. Address of the rich-text cursor positioned to the 
	point at which the search for the next action-attachment hotspot succession 
	should commence. This cursor is then used when advancing through further 
	rich-text content. If the procedure is successful, the cursor will finish 
	at the hotspot-end CD record of the (outer) action hotspot, else it will be 
	nullified because it will have traveled through the entire available 
	rich-text field.
pt_CTX: Optional. Address of environment information about the rich-text field 
	being navigated. If passed in null, the rich-text cursor being used will be 
	unable to move from virtuality to actuality.
f_BMP_FWD_1ST: Flag telling whether the passed-in starting cursor should be 
	moved to the next CD record before initiating the search for an 
	action-attachment hotspot succession. Usually when starting at the 
	beginning of the rich-text field, the flag will be set to FALSE so that the 
	first CD record may be checked as to whether it's a file-attachment 
	hotspot. Ensuing calls might then set the flag to TRUE to move to the next 
	CD record since the current CD record (an end-hotspot record) is already 
	known.
pt_spnFrmla: Optional Output. Address of structure to receive information on 
	where a found action-attachment hotspot succession begins and ends within 
	the rich-text field (essentially, the action-hotspot dimensions). The 
	"begins" member will point to the hotspot-begin CD record; the "ends" 
	member will point to the hotspot-end CD record. If address is null, output 
	is suppressed.
pt_spnAtch: Optional Output. Address of structure to receive information on 
	where the embedded attachment hotspot resides within the action hotspot. 
	The "begin" member will point to the first CD record of the hotspot 
	sequence; the "end" member will point to the first CD record of the 
	hotspot-close sequence (always a SIG_CD_HOTSPOTEND and possibly itself the 
	end of the sequence). If the begin member points to a SIG_CD_BEGIN record 
	(introduced in R5), the counterpart -END record will follow immediately the 
	SIG_CD_HOTSPOTEND pointed to by the end member. That is, the span may mark 
	a SIG_CD_BEGIN/-END wrapping of the actual, immediately encapsulated 
	attachment hotspot. If address is null, output is suppressed.
ppc_fileNm: Optional Output. Address of the string pointer to be set to the 
	name of the file the user attached to the note. If null, output is 
	suppressed. The pointed-to memory is owned by the note, so the caller 
	should not free the addressed memory.
ppc_objNm: Optional Output. Address of the string pointer to be set to the name 
	Notes uses internally for the file attachment associated with the 
	attachment hotspot. If null, output is suppressed. The pointed-to memory is 
	owned by the note, so the caller should not free the addressed memory.
RETURN: eus_ERR_INVLD_ARG if any input parameter is obviously invalid; 
	eus_SUCCESS otherwise

--- revision history -----
7/21/03 PR
+ logic extension and concomitant signature change to accommodate Notes-client 
  rewrites of an R4-style formulation of action-attach hotspots
+ logic enhancement to emulate better the Notes client in assigning 
  formula-hotspot record flags
+ logic adjustment due to signature change to f_CursorToHotspotEnd()
+ token renaming, documentation improvement

3/14/03 PR: created		*/
STATUS eus_CursorToFormulaActionAttach( CdRecordCursor *const  pt_crsr, 
										const RtfTrackingInfo *const  pt_CTX, 
										const BOOL  f_BMP_FWD_1ST, 
										CdRecordSpan *const  pt_spnFrmla, 
										CdRecordSpan *const  pt_spnAtch, 
										const char * *const  ppc_fileNm, 
										const char * *const  ppc_objNm)	{
	WORD  us;
	CDHOTSPOTBEGIN * pt;
	CdRecordCursor  t_crsrBgnAtch, t_crsrAtchInfo, t_crsrEndAtch;
	BYTE  uc;
	const char * pc;

	if (!( pt_crsr->puc_location && pt_crsr->us_recLength && (pt_CTX ? (BOOL) 
												pt_CTX->pt_Actuality : TRUE)))
		return eus_ERR_INVLD_ARG;

	//as applicable, default the passed-in span structure to all null and the 
	//	file-name pointers to null
	if (pt_spnFrmla)
		memset( pt_spnFrmla, NULL, sizeof( CdRecordSpan));
	if (pt_spnAtch)
		memset( pt_spnAtch, NULL, sizeof( CdRecordSpan));
	if (ppc_fileNm)
		*ppc_fileNm = NULL;
	if (ppc_objNm)
		*ppc_objNm = NULL;

	//if cursor seems to already be at the end of all content, return that the 
	//	procedure has run error free, but indicating that no attachment hotspot 
	//	was found
	if (!pt_crsr->pt_item)	{
		pt_crsr->puc_location = NULL;
		return eus_SUCCESS;
	}

	//If requested, do an intial advance of the cursor to the next CD record. 
	//	If the rich-text field has been exhausted, return that the procedure 
	//	has run error free, but flag that no file-attachment hotspot was found.
	if (f_BMP_FWD_1ST)	{
		AdvanceCdCursor( pt_crsr, pt_CTX, NULL, NULL);
		if (!pt_crsr->pt_item)	{
			pt_crsr->puc_location = NULL;
			return eus_SUCCESS;
		}
	} //if (f_BMP_FWD_1ST)

	//find an appropriate action-attachment hotspot succession...
	t_crsrBgnAtch.pt_item = NULL;
	do	{
		//Find a formula-action hotspot's begin CD record. If we exhaust the 
		//	rich-text field, short-circuit with success and an indication that 
		//	our target wasn't found. (We don't demand equality in the flags 
		//	member because the HOTSPOTREC_RUNFLAG_BEGIN flag may be present 
		//	(unnecessarily) due to encodings by earlier versions of this DLL; 
		//	Notes R5 seems not to include the flag, so we stopped doing so to 
		//	match.)
		while (!( muc_SIG_CDV4HOTSPOTBEGIN == *pt_crsr->puc_location && 
									(pt = (CDHOTSPOTBEGIN *) 
									pt_crsr->puc_location)->Type == 
									HOTSPOTREC_TYPE_HOTREGION && (pt->Flags & 
									mui_HOTSPT_ACTN_FRMULA_FLGS) == 
									mui_HOTSPT_ACTN_FRMULA_FLGS))	{
			AdvanceCdCursor( pt_crsr, pt_CTX, NULL, NULL);
			if (!pt_crsr->pt_item)	{
				pt_crsr->puc_location = NULL;
				return eus_SUCCESS;
			}
		} //while (!( muc_SIG_CDV4HOTSPOTBEGIN ==

		//if caller's interested, take tentative note of the position of the 
		//	action-hotspot's begin CD record
		if (pt_spnFrmla)
			pt_spnFrmla->t_crsrBgin = *pt_crsr;

		//skip over any blank CDTEXT records (as may obtain if the Notes client 
		//	has rewritten simpler, "old-style" hotspot rich-text of the sort we 
		//	tend to  write, e.g. as client may during a message save or forward)
		do	{
			AdvanceCdCursor( pt_crsr, pt_CTX, NULL, NULL);
			if (!pt_crsr->pt_item)	{
				pt_crsr->puc_location = NULL;
				return eus_SUCCESS;
			}
		} while ((uc = *pt_crsr->puc_location) == muc_SIG_CDTEXT && 
									pt_crsr->us_recLength == sizeof( CDTEXT));

		//if the ensuing CD record could be the beginning of a wrap of an 
		//	attachment hotspot, move to the next record
		if (muc_SIG_CDBEGIN == uc && ((CDBEGINRECORD *) 
										pt_crsr->puc_location)->Signature == 
										SIG_CD_V4HOTSPOTBEGIN)	{
			t_crsrBgnAtch = *pt_crsr;
			AdvanceCdCursor( pt_crsr, pt_CTX, NULL, NULL);
			if (!pt_crsr->pt_item)	{
				pt_crsr->puc_location = NULL;
				return eus_SUCCESS;
			}
			uc = *pt_crsr->puc_location;
		} //if (muc_SIG_CDBEGIN == uc && 

		//If the CD record of interest is an attachment-hotspot's begin 
		//	record... (The "weird" file-hotspot types occurs when an 
		//	attachment is copied to a new document via the "inherit entire 
		//	selected document into rich text field" form property [and maybe 
		//	via the MailForward @Command too?].)
		if (muc_SIG_CDHOTSPOTBEGIN == uc && ((us = ((CDHOTSPOTBEGIN *) 
											pt_crsr->puc_location)->Type) == 
											HOTSPOTREC_TYPE_FILE || us == 
											mus_HOTSPT_FILE_WEIRD))	{
			//if above we haven't marked the start of a wrapper of the 
			//	attachment hotspot, note that the start of the attachment 
			//	hotspot is here
			t_crsrAtchInfo = *pt_crsr;
			if (!t_crsrBgnAtch.pt_item)
				t_crsrBgnAtch = t_crsrAtchInfo;
		//else if the start of an attachment-hotspot wrapper was tentatively 
		//	marked above, clear it now
		}else if (t_crsrBgnAtch.pt_item)
			t_crsrBgnAtch.pt_item = NULL;

		//if no hotspot-end record is found corresponding (presumably) to 
		//	either the attachment hotspot (if we did just locate a wrapped 
		//	attachment just above) or the action hotspot (if the opposite), 
		//	short-circuit with success but indicating that we came up dry
		if (!f_CursorToHotspotEnd( (BYTE) (t_crsrBgnAtch.pt_item ? 
											muc_SIG_CDHOTSPOTEND : 
											muc_SIG_CDV4HOTSPOTEND), pt_crsr, 
											pt_CTX, FALSE, NULL, NULL))	{
			pt_crsr->puc_location = NULL;
			return eus_SUCCESS;
		}

		//if it seems the attachment hotspot is being wrapped, make sure of it 
		//	before we allow the search to stop
		t_crsrEndAtch = *pt_crsr;
		if (muc_SIG_CDBEGIN == *t_crsrBgnAtch.puc_location)	{
			AdvanceCdCursor( pt_crsr, pt_CTX, NULL, NULL);
			if (!pt_crsr->pt_item)	{
				pt_crsr->puc_location = NULL;
				return eus_SUCCESS;
			}
			if (!( muc_SIG_CDEND == *pt_crsr->puc_location && ((CDENDRECORD *) 
										pt_crsr->puc_location)->Signature == 
										SIG_CD_V4HOTSPOTEND))
				t_crsrBgnAtch.pt_item = NULL;
		} //if (muc_SIG_CDBEGIN ==
	} while (!t_crsrBgnAtch.pt_item);

	//if caller's interested, output the span marking out the embedded 
	//	attachment
	if (pt_spnAtch)	{
		pt_spnAtch->t_crsrEnd = t_crsrEndAtch;
		pt_spnAtch->t_crsrBgin = t_crsrBgnAtch;
	}

	//determine the position of the action-hotspot's hotspot-end record and, if 
	//	caller's interested, incorporate that information in the span output
	if (!f_CursorToHotspotEnd( muc_SIG_CDV4HOTSPOTEND, pt_crsr, pt_CTX, 
														TRUE, NULL, NULL))	{
		pt_crsr->puc_location = NULL;
		return eus_SUCCESS;
	}
	if (pt_spnFrmla)
		pt_spnFrmla->t_crsrEnd = *pt_crsr;

	//as caller's interested, provide file-/object-name info
	pc = t_crsrAtchInfo.puc_location + sizeof( CDHOTSPOTBEGIN);
	if (ppc_fileNm)
		*ppc_fileNm = pc + strlen( pc) + 1;
	if (ppc_objNm)
		*ppc_objNm = pc;

	return eus_SUCCESS;
} //eus_CursorToFormulaActionAttach(


/** eus_ResetAttachHotspotNames( ***
Makes changes to the internal and for-external-use filenames associated with 
the given rich-text attachment hotspot.

--- parameters & return ------
pc_OBJNM: Optional. Address of string telling what the internal attachment name 
	should become. If null, the internal name currently contained within the 
	specified attachment hotspot will be used.
pc_FILENM: Optional. Address of the for-external-use attachment name. If null, 
	the for-external-use attachment name currently contained within the 
	specified attachment hotspot will be used.
t_CRSR: Information structure giving the position hotspot-begin CD record 
	associated with the attachment whose name we're resetting. If the position 
	is at a point in actuality that precedes the "actual frontier" where 
	virtuality flows into actually, procedure will not manipulate the frontier, 
	no matter the instruction given in the f_IGNOR_FRONTR argument.
f_IGNOR_FRONTR: flag telling whether procedure should desist from updating the 
	"actual frontier" where virtuality flows into actuality and so also from 
	virtualizing all content preceding the hotspot-begin CD record
pt_ctx: Input & Output. Address of state information about the rich-text field 
	whose content is to be adjusted. The virtuality components of this 
	information may be updated as a result of this call.
RETURN: 
	eus_ERR_INVLD_ARG if any passed-in parameter is obviously invalid
	eus_SUCCESS if no error occured
	Notes API error code otherwise

--- suggested enhancement ---
5/30/00 PR: improve so PGP-development shortcuts aren't needed

--- revision history --------
7/21/03 PR: participated in global token renaming

3/14/03 PR
+ logic enhancement and associated signature change to allow the appendage of 
  the revised hotspot-begin record to occur without consideration of the 
  "actual frontier" (and the concomitant virtualization of content preceding 
  the hotspot-begin record)
+ minor logic adjustment in support of signature change to 
	us_VirtualizeThruActualLocation()

9/6/02 PR: listing format adjustment, minor exception-handling adjustment, 
	minor documentation adjustment
5/30/00 PR: added standard documentation; token renaming; minor error-handling 
	improvement
12/15/98 PR: created		*/
STATUS eus_ResetAttachHotspotNames( const char  pc_OBJNM[], 
									const char  pc_FILENM[], 
									const CdRecordCursor  t_CRSR, 
									const BOOL  f_IGNOR_FRONTR, 
									RtfTrackingInfo *const  pt_ctx)	{
	BOOL  f_ignrFrontr = f_IGNOR_FRONTR;
	WORD  us_lenOrigRec, us;
	char * pc_objNmOrig, * pc_FileNmOrig;
	UINT  ui_lenAttachNm;
	BYTE * puc;
	CDHOTSPOTBEGIN * pt;
	STATUS  us_err;

	if (!( (pc_OBJNM || pc_FILENM) && t_CRSR.puc_location && t_CRSR.pt_item && 
								t_CRSR.us_recLength && *t_CRSR.puc_location == 
								muc_SIG_CDHOTSPOTBEGIN && pt_ctx))
		return eus_ERR_INVLD_ARG;

//PGP development shortcut: skip all the complexity of doing this with a 
//	virtual start point
if (t_CRSR.pt_item->i_type != mi_ACTUAL || pt_ctx->ul_ActualFrontier == 
														mul_FRONTIER_NO_MORE)
	return eus_ERR_INVLD_ARG;

	//if caller requests that the actual frontier updated as a result of this 
	//	call...
	if (!f_ignrFrontr)	{
		//if the position of the hotspot-begin record precedes the actual 
		//	frontier...
		const DWORD ul_STRT = ul_GetAbsoluteOffset( t_CRSR.puc_location, 
										t_CRSR.pt_item, pt_ctx->pt_Actuality), 
					ul_FRONTR = pt_ctx->ul_ActualFrontier;
		if (ul_STRT == eul_ERR_FAILURE)
			return eus_ERR_INVLD_ARG;
		if (ul_STRT < ul_FRONTR)
			//ignore from here on the caller's request that the actual frontier 
			//	be updated (because it makes no sense)
			f_ignrFrontr = TRUE;
		//else if the frontier precedes the postion of the hotspot-begin, 
		//	virtualize all content preceding the hotspot-begin record
		else if (ul_STRT > ul_FRONTR)
			if (us_err = us_VirtualizeThruActualLocation( t_CRSR.puc_location, 
															pt_ctx, NULL, NULL))
				return us_err;
	} //if (!f_ignrFrontr)

	//if we're adjusting the actual frontier as we go, note the current length 
	//	of the hotspot-begin record
	if (!f_ignrFrontr)
		us_lenOrigRec = ((CDHOTSPOTBEGIN *) t_CRSR.puc_location)->Header.Length;

	//get a pointer to where we can append an adjusted hotspot-begin record
	pc_objNmOrig = t_CRSR.puc_location + sizeof( CDHOTSPOTBEGIN);
	us = sizeof( CDHOTSPOTBEGIN) + (ui_lenAttachNm = (pc_OBJNM ? strlen( 
										pc_OBJNM) : strlen( pc_objNmOrig)) + 
										1) + (pc_FILENM ? strlen( pc_FILENM) : 
										strlen( pc_FileNmOrig = pc_objNmOrig + 
										strlen( pc_objNmOrig) + 1)) + 1;
	if (us_err = us_GetVirtualCdRecAppendPointer( pt_ctx, us, &puc))
		return us_err;

	//copy in the existing hotspot up to where the attachment names begin
	memcpy( puc, t_CRSR.puc_location, sizeof( CDHOTSPOTBEGIN));

	//reset as needed member fields in the hotspot-begin CD record
	(pt = (CDHOTSPOTBEGIN *) puc)->Header.Length = us;
	pt->DataLength = us - sizeof( CDHOTSPOTBEGIN);
	pt->Flags &= 0x0000FFFF;		//relativize!!

	//copy in the adjusted names
	strcpy( puc += sizeof( CDHOTSPOTBEGIN), pc_OBJNM ? pc_OBJNM : pc_objNmOrig);
	strcpy( puc + ui_lenAttachNm, pc_FILENM ? pc_FILENM : pc_FileNmOrig);

	//note the new length of the ending virtual item
	pt_ctx->pt_endVirtual->ul_length += pt_ctx->pt_endVirtual->ul_length % 2 + 
																			us;

	//if we're adjusting the actual frontier as we go, finish it up
	if (!f_ignrFrontr)
		pt_ctx->ul_ActualFrontier += us_lenOrigRec + us_lenOrigRec % 2;

	return eus_SUCCESS;
} //eus_ResetAttachHotspotNames(


/** eus_AppendHotspotInnardsToVirtuality( ***
Copies the CD records inside a rich-text hotspot span to the end of virutality. 
If the 

--- parameters & return ----
pt_crsr: Input & Output. Address of information describing the starting 
	rich-text position (a) from which the operation will commence and (b) 
	before which any non-virtualized rich-text content will be virtualized 
	before the CD records strictly inside the hotspot are appended. Therefore 
	on input the cursor must be positioned on a begin-hotspot record or already 
	among the innards encompassed by a hotspot. Currently, this starting 
	position must fall within actuality beyond the actual frontier. On output, 
	the cursor will be positioned at the end-hotspot record.
pt_ctx: Input & Output. Address of information describing the environment 
	surrounding the given rich-text content. This information may undergo 
	change as a result of the call.
RETURN:
	eus_ERR_INVLD_ARG if procedure's input context is obviously invalid or if 
		the given rich-text position proves not to fall within a rich-text 
		hotspot span
	eus_SUCCESS if successful
	the Notes API error code otherwise

--- revision history -------
7/21/03 PR
+ minor logic adjustment due to signature change to f_CursorToHotspotEnd()
+ participated in global token renaming

3/14/03 PR: created			*/
STATUS eus_AppendHotspotInnardsToVirtuality( 
											CdRecordCursor *const  pt_crsr, 
											RtfTrackingInfo *const  pt_ctx)	{
	BYTE  uc;
	const BYTE * puc_bgn;
	CdRecordCursor  t_crsrEnd;
	STATUS  us_err;

	if (!( pt_crsr && pt_crsr->pt_item && pt_crsr->pt_item->i_type == 
										mi_ACTUAL && pt_crsr->puc_location && 
										pt_crsr->us_recLength && pt_ctx && 
										pt_ctx->ul_ActualFrontier != 
										mul_FRONTIER_NO_MORE))
		return eus_ERR_INVLD_ARG;

	//advance cursor to the first CD record that's not a hotspot-begin record
	puc_bgn = pt_crsr->puc_location;
	while ((uc = *(BYTE *) pt_crsr->puc_location) == muc_SIG_CDBEGIN || uc == 
												muc_SIG_CDV4HOTSPOTBEGIN || 
												uc == muc_SIG_CDHOTSPOTBEGIN)
		AdvanceCdCursor( pt_crsr, pt_ctx, NULL, NULL);

	//locate the end of the hotspot
	t_crsrEnd = *pt_crsr;
	if (!f_CursorToHotspotEnd( NULL, &t_crsrEnd, pt_ctx, FALSE, NULL, NULL))
		return eus_ERR_INVLD_ARG;

	//virtualize up to the provided starting point
	if (us_err = us_VirtualizeThruActualLocation( puc_bgn, pt_ctx, NULL, NULL))
		return us_err;

	//append thereto the (remaining) CD records inside the hotspot
	if (us_err = us_VirtualizePointToPoint( pt_crsr, t_crsrEnd.puc_location, 
																		pt_ctx))
		return us_err;

	//reset the "actual frontier" to point to the end-hotspot record
	pt_ctx->ul_ActualFrontier = ul_GetAbsoluteOffset( t_crsrEnd.puc_location, 
									t_crsrEnd.pt_item, pt_ctx->pt_Actuality);

	//tell caller the position of the end-hotspot record
	*pt_crsr = t_crsrEnd;

	return eus_SUCCESS;
} //eus_AppendHotspotInnardsToVirtuality(


/** eus_InsertTextAtHotspotEnd( ***
Insert text string provided just before the specified hotspot-end CD record, 
thus making the string part of the hotspot.

--- parameters & return -----
PC: address of string to insert just before the next hotspot-end CD record
t_crsr: rich-text cursor that either already points to the hotspot-end CD 
	record or points to the corresponding, preceding hotspot-begin CD record
pt_ctx: Input & Output. Address of environment information about the rich-text 
	field being operated on. Its virtuality information may be updated by the 
	procedure.
RETURN:
	eus_SUCCESS if no errors occurred
	eus_ERR_INVLD_ARG if any passed-in parameter is obviously invalid
	the Notes API error code otherwise

--- suggested enhancement ----
8/9/00 PR: improve so PGP-development shortcuts aren't needed

--- revision history ---------
7/21/03 PR
+ minor logic adjustment due to signature change to f_CursorToHotspotEnd()
+ participated in global token renaming

3/14/03 PR
+ minor logic adjustment in support of signature change to 
  us_VirtualizeThruActualLocation()
+ minor documentation adjustment, minor signature adjustment

9/6/02 PR: minor documentation adjustment, token renaming, listing format 
	adjustment
8/9/00 PR: minor exception-handling adjustment

3/20/00 PR
+ extended to allow input of the hotspot-begin CD record instead of the 
  hotspot-end record
+ completed standard documentation

12/15/98 PR: created		*/
STATUS eus_InsertTextAtHotspotEnd( const char  PC[], 
									CdRecordCursor  t_crsr, 
									RtfTrackingInfo *const  pt_ctx)	{
	DWORD  ul;
	BYTE * puc;
	CDTEXT  t = {{SIG_CD_TEXT}};
	STATUS  us_err;

	if (!( PC && pt_ctx && t_crsr.puc_location && t_crsr.pt_item && 
								t_crsr.us_recLength && (*t_crsr.puc_location == 
								muc_SIG_CDHOTSPOTEND || *t_crsr.puc_location == 
								muc_SIG_CDHOTSPOTBEGIN)))
		return eus_ERR_INVLD_ARG;

//PGP development shortcut: skip all the complexity of doing this with a 
//	virtual start point
if (t_crsr.pt_item->i_type != mi_ACTUAL || pt_ctx->ul_ActualFrontier == 
														mul_FRONTIER_NO_MORE)
	return eus_ERR_INVLD_ARG;

	//move to the end of the hotspot
	if (*(BYTE *) t_crsr.puc_location == muc_SIG_CDHOTSPOTBEGIN && 
								!f_CursorToHotspotEnd( muc_SIG_CDHOTSPOTEND, 
								&t_crsr, pt_ctx, FALSE, NULL, NULL))
		return !eus_SUCCESS;

	if (us_err = us_VirtualizeThruActualLocation( t_crsr.puc_location, pt_ctx, 
																	NULL, NULL))
		return us_err;

	//get a pointer at which we can virtualize the insertion CDTEXT
	if (us_err = us_GetVirtualCdRecAppendPointer( pt_ctx, (ul = sizeof( 
												CDTEXT) + strlen( PC)), &puc))
		return us_err;

t.FontID = FontSetColor( DEFAULT_SMALL_FONT_ID, NOTES_COLOR_DKRED);	//should 
					//	make adjustable by caller
	t.Header.Length = (WORD) ul;

	//virtualize the CDTEXT
	memcpy( puc, &t, sizeof( CDTEXT));
memcpy( puc + sizeof( CDTEXT), PC, strlen( PC));	//not accounting for 
													//	linefeed translation

	//done with the virtualization of the CDTEXT, update the variable that 
	//	tracks the length of the virtual item being written to
	pt_ctx->pt_endVirtual->ul_length += pt_ctx->pt_endVirtual->ul_length % 2 + 
																			ul;

	return eus_SUCCESS;
} //eus_InsertTextAtHotspotEnd(


/** eus_StartFormulaActionHotspot( ***
Writes the opening of an @Formula action hotspot at the specified location in 
the given rich-text context.

--- parameters & return -----
PC: address of the @Formula string to compile and use in the action hotspot to 
	be constructed
pt_CRSR: Optional. Address of rich-text cursor positioned at the point before 
	which the action hotspot is to be constructed. If cursor is null of null 
	location, procedure appends the hotspot start to the content in play.
pt_ctx: Input & Output. Address of environment information about the rich-text 
	field being operated on. Its virtuality information will be updated by the 
	procedure.
RETURN:
	eus_SUCCESS if no errors occurred
	eus_ERR_INVLD_ARG if any passed-in parameter is obviously invalid
	the Notes API error code otherwise

--- suggested enhancement ---
3/14/03 PR: Isn't there some better way than heavy-handed LoadLibrary() without 
	corresponding FreeLibrary() after work is done? [If GetModuleHandle()(?) 
	always returns the same value for h_dll, then there should be a better way.]
8/9/00 PR: improve so PGP-development shortcut isn't needed

--- revision history --------
9/3/03 PR: fixed regression error in building template CD records

3/14/03 PR
+ minor logic adjustment in support of signature change to 
  us_VirtualizeThruActualLocation() and shared component with new 
  eus_CursorToFormulaActionAttach()
+ minor signature adjustment to provide appropriate constness
+ documentation adjustment, listing format adjustment, token renaming

9/6/02 PR
+ logic enhancement to allow appending to a rich-text context as opposed to 
  just insertion
+ listing format adjustment, token renaming, minor documentation adjustment, 
  minor exception-handling adjustment

11/14/00 PR: documentation adjustment
9/16/00 PR: completed code
8/9/00 PR: created			*/
STATUS eus_StartFormulaActionHotspot( const char  PC[], 
										const CdRecordCursor *const  pt_CRSR, 
										RtfTrackingInfo *const  pt_ctx)	{
	static HMODULE  h_dll;
	static FARPROC  pr_NSFBeginSignature, pr_NSFMemorySign, pr_NSFEndSignature;

	FORMULAHANDLE  h_frmla;
	HANDLE  h_preSig = NULL, h_sig;
	WORD  us_lenFrmla, us_lenSig, us;
	BYTE * puc_frmla = NULL, * puc;
	CDHOTSPOTBEGIN  t_HotsptBgn = {{SIG_CD_V4HOTSPOTBEGIN}, 
												HOTSPOTREC_TYPE_HOTREGION, 
												mui_HOTSPT_ACTN_FRMULA_FLGS};
	STATUS  us_err;

	if (!( PC && pt_ctx))
		return eus_ERR_INVLD_ARG;

//PGP development shortcut: skip all the complexity of doing this with a 
//	virtual start point
if ((pt_CRSR && pt_CRSR->pt_item ? pt_CRSR->pt_item->i_type != mi_ACTUAL : 
										FALSE) || pt_ctx->ul_ActualFrontier == 
										mul_FRONTIER_NO_MORE)
	return eus_ERR_INVLD_ARG;

	//If we haven't already, get handles on the undocumented Notes API calls 
	//	needed to sign the compiled formula with the current user ID. It is not 
	//	necessary to unload the DLL loaded herein with the LoadLibrary() call 
	//	because the normal process shutdown will force the unload when the time 
	//	comes. So we might as well maintain the load-state for the duration of 
	//	the session.
	if (!h_dll)	{
		if (!( h_dll = LoadLibrary( "nnotes.dll")))
			return !eus_SUCCESS;
		pr_NSFBeginSignature = GetProcAddress( h_dll, "NSFBeginSignature");
		pr_NSFMemorySign = GetProcAddress( h_dll, "NSFMemorySign");
		pr_NSFEndSignature = GetProcAddress( h_dll, "NSFEndSignature");
		if (!( pr_NSFBeginSignature && pr_NSFMemorySign && 
													pr_NSFEndSignature))	{
			FreeLibrary( h_dll);
			return !eus_SUCCESS;
		}
	} //if (!h_dll)

	//compile the provided formula
	if (us_err = NSFFormulaCompile( NULL, NULL, (char *) PC, (WORD) strlen( 
												PC), &h_frmla, &us_lenFrmla, 
												&us, &us, &us, &us, &us))
		return us_err;

	//construct signature against the compiled formula
	if (us_err = pr_NSFBeginSignature( &h_preSig))
		goto errJump;
	if (us_err = pr_NSFMemorySign( h_preSig, puc_frmla = OSLockObject( 
														h_frmla), us_lenFrmla))
		goto errJump;
	if (us_err = pr_NSFEndSignature( h_preSig, &h_sig, &us_lenSig))
		goto errJump;
	h_preSig = NULL;

	//virtualize up to where the hotspot-begin CD record is to be written
	if (pt_CRSR && pt_CRSR->puc_location && pt_CRSR->pt_item)	{
		if (us_err = us_VirtualizeThruActualLocation( pt_CRSR->puc_location, 
															pt_ctx, NULL, NULL))
			goto errJump;
	}else
		if (us_err = us_VirtualizeRestOfRtf( pt_ctx, NULL))
			goto errJump;

	//complete the hotspot-begin CD record
	t_HotsptBgn.DataLength = us_lenFrmla;
	t_HotsptBgn.Header.Length = (us = sizeof( CDHOTSPOTBEGIN) + us_lenFrmla + 
													sizeof( WORD) + us_lenSig);

	//obtain the location where the record may be written, and write the record
	if (us_err = us_GetVirtualCdRecAppendPointer( pt_ctx, us, &puc))
		goto errJump;
	memcpy( puc, &t_HotsptBgn, sizeof( CDHOTSPOTBEGIN));
	memcpy( puc += sizeof( CDHOTSPOTBEGIN), puc_frmla, us_lenFrmla);
	*(WORD *) (puc += us_lenFrmla) = us_lenSig;
	memcpy( puc + sizeof( WORD), OSLockObject( h_sig), us_lenSig);
	OSUnlockObject( h_sig);

	//done with the virtualization, update the variable that tracks the length 
	//	of the virtual item being written to
	pt_ctx->pt_endVirtual->ul_length += pt_ctx->pt_endVirtual->ul_length % 2 + 
																			us;

errJump:
	if (h_sig)
		OSMemFree( h_sig);
	if (puc_frmla)
		OSUnlockObject( h_frmla);
	if (h_preSig)
		OSMemFree( h_preSig);
	OSMemFree( h_frmla);

	return us_err;
} //eus_StartFormulaActionHotspot(


/** eus_StartLinkHotspot( ***
Writes the opening of a link action hotspot at the specified location in the 
given rich-text context.

--- parameters & return -----
PC: address of the link string to use in the action hotspot to be constructed
pt_CRSR: Optional. Address of rich-text cursor positioned at the point before 
	which the link hotspot is to be constructed. If cursor is null of null 
	location, procedure appends the hotspot start to the content in play.
pt_ctx: Input & Output. Address of environment information about the rich-text 
	field being operated on. Its virtuality information will be updated by the 
	procedure.
RETURN:
	eus_SUCCESS if no errors occurred
	eus_ERR_INVLD_ARG if any passed-in parameter is obviously invalid
	the Notes API error code otherwise

--- suggested enhancement ---
8/9/00 PR: improve so PGP-development shortcut isn't needed

--- revision history --------
9/3/03 PR: fixed regression error in building template CD records
3/14/03 PR: created			*/
STATUS eus_StartLinkHotspot( const char  PC[], 
								const CdRecordCursor *const  pt_CRSR, 
								RtfTrackingInfo *const  pt_ctx)	{
	const size_t  UI = PC ? strlen( PC) : NULL;
	const CDHOTSPOTBEGIN  t_HOTSPT_BGN_V5 = {{SIG_CD_V5HOTSPOTBEGIN, 
											sizeof( CDHOTSPOTBEGIN)}, 
											HOTSPOTREC_TYPE_RCLINK, 
											HOTSPOTREC_RUNFLAG_NOBORDER, 0}, 
							t_HOTSPT_BGN_V4 = {{SIG_CD_V4HOTSPOTBEGIN, 
											sizeof( CDHOTSPOTBEGIN) + UI + 1}, 
											HOTSPOTREC_TYPE_ANY + 
											HOTSPOTREC_TYPE_BUTTON, 
											HOTSPOTREC_RUNFLAG_INOTES, UI + 1};
	const CDBEGINRECORD  t_BGN_HOTLNK_TXT = {{SIG_CD_BEGIN, sizeof( 
											CDBEGINRECORD)}, 0, SIG_CD_TEXT};

	CDRESOURCE  t_rsrc = {{SIG_CD_HREF, sizeof( CDRESOURCE) + UI}, NULL, 
							CDRESOURCE_TYPE_URL, NULL, (WORD) UI, NULL, NULL};
	WORD  us;
	BYTE * puc;
	STATUS  us_err;

	if (!( PC && *PC && pt_ctx))
		return eus_ERR_INVLD_ARG;

	memset( t_rsrc.Reserved, NULL, sizeof( t_rsrc.Reserved));

	//if an insertion location was given...
	if (pt_CRSR && pt_CRSR->puc_location)	{
		//virtualize up to the insertion point
		if (pt_CRSR->pt_item->i_type == mi_ACTUAL)	{
			if (us_err = us_VirtualizeThruActualLocation( 
									pt_CRSR->puc_location, pt_ctx, NULL, NULL))
				return us_err;
		}else
//PGP development shortcut: disallowing virtual insertion points!
return eus_ERR_INVLD_ARG;
	//else virtualize the rest of any in-play actual rich-text context so we're 
	//	ready to append to it
	}else
		if (us_err = us_VirtualizeRestOfRtf( pt_ctx, NULL))
			return us_err;

	//compute the length of buffer needed to accomodate what we need to write 
	//	out
	us = sizeof( CDBEGINRECORD) * 2 + sizeof( CDHOTSPOTBEGIN) + 
														t_rsrc.Header.Length;
	us += us % 2 + t_HOTSPT_BGN_V4.Header.Length;

	//obtain the location where the record may be written, and write the record
	if (us_err = us_GetVirtualCdRecAppendPointer( pt_ctx, us, &puc))
		return us_err;
	memcpy( puc, &mt_R5_BEGIN_HOTSPOT, sizeof( CDBEGINRECORD));
	memcpy( puc += sizeof( CDBEGINRECORD), &t_HOTSPT_BGN_V5, 
													sizeof( CDHOTSPOTBEGIN));
	memcpy( puc += sizeof( CDHOTSPOTBEGIN), &t_rsrc, sizeof( CDRESOURCE));
	memcpy( puc += sizeof( CDRESOURCE), PC, UI);
	memcpy( puc += UI + UI % 2, &t_HOTSPT_BGN_V4, sizeof( CDHOTSPOTBEGIN));
	memcpy( puc += sizeof( CDHOTSPOTBEGIN), PC, UI + 1);
	memcpy( puc + UI + 1 + (UI + 1) % 2, &t_BGN_HOTLNK_TXT, 
														sizeof( CDBEGINRECORD));

	//done with the virtualization, update the variable that tracks the length 
	//	of the virtual item being written to
	pt_ctx->pt_endVirtual->ul_length += pt_ctx->pt_endVirtual->ul_length % 2 + 
																			us;

	return eus_SUCCESS;
} //eus_StartLinkHotspot(


/** eus_CloseLinkHotspot( ***
Writes the closing of a link action hotspot at the specified location in the 
given rich-text context.

--- parameters & return -----
pt_CRSR: Optional. Address of rich-text cursor positioned at the CD record 
	before which the closure is to be inserted. If null or of null location, 
	the closure will be appended to the end of the content in play.
pt_ctx: Input & Output. Address of environment information about the rich-text 
	field being operated on. Its virtuality information will be updated by the 
	procedure.
RETURN:
	eus_SUCCESS if no errors occurred
	eus_ERR_INVLD_ARG if any passed-in parameter is obviously invalid
	the Notes API error code otherwise

--- revision history --------
9/3/03 PR: fixed regression error in building template CD records
7/21/03 PR: participated in global token renaming
3/14/03 PR: created			*/
STATUS eus_CloseLinkHotspot( const CdRecordCursor *const  pt_CRSR, 
								RtfTrackingInfo *const  pt_ctx)	{
	const CDCOLOR  t_COLOR = {{SIG_CD_COLOR, sizeof( CDCOLOR)}, 
								{COLOR_VALUE_FLAGS_ISRGB + 
								COLOR_VALUE_FLAGS_SYSTEMCOLOR, NULL, NULL, 255, 
								NULL}};
	const CDENDRECORD  t_END_HOTLNK_TXT = {{SIG_CD_END, sizeof( CDENDRECORD)}, 
																0, SIG_CD_TEXT};

	WORD  us;
	BYTE * puc;
	STATUS  us_err;

	if (!pt_ctx)
		return eus_ERR_INVLD_ARG;

	//if an insertion location was given...
	if (pt_CRSR && pt_CRSR->puc_location)	{
		//virtualize up to the insertion point
		if (pt_CRSR->pt_item->i_type == mi_ACTUAL)	{
			if (us_err = us_VirtualizeThruActualLocation( 
									pt_CRSR->puc_location, pt_ctx, NULL, NULL))
				return us_err;
		}else
//PGP development shortcut: disallowing virtual insertion points!
return eus_ERR_INVLD_ARG;
	//else virtualize the rest of any in-play actual rich-text context so we're 
	//	ready to append to it
	}else
		if (us_err = us_VirtualizeRestOfRtf( pt_ctx, NULL))
			return us_err;

	//compute the length of buffer needed to accomodate what we need to write 
	//	out
	us = sizeof( CDCOLOR) + sizeof( CDENDRECORD) * 2 + sizeof( CDHOTSPOTEND);

	//obtain the location where the record may be written, and write the record
	if (us_err = us_GetVirtualCdRecAppendPointer( pt_ctx, us, &puc))
		return us_err;
	memcpy( puc, &t_COLOR, sizeof( CDCOLOR));
	memcpy( puc += sizeof( CDCOLOR), &t_END_HOTLNK_TXT, sizeof( CDENDRECORD));
	memcpy( puc += sizeof( CDENDRECORD), &mt_V4HOTSPOT_END, 
														sizeof( CDHOTSPOTEND));
	memcpy( puc + sizeof( CDHOTSPOTEND), &mt_R5_END_HOTSPOT, 
														sizeof( CDENDRECORD));

	//done with the virtualization, update the variable that tracks the length 
	//	of the virtual item being written to
	pt_ctx->pt_endVirtual->ul_length += pt_ctx->pt_endVirtual->ul_length % 2 + 
																			us;

	return eus_SUCCESS;
} //eus_CloseLinkHotspot(


/** us_VirtualizePointToPoint( ***
Virtualizes rich-text content from a starting point to just before a given end 
point, without adjusting the "actual frontier" which marks where virtuality 
flows back into actuality.

--- parameters & return ----
pt_crsr: Input & Output. Address of rich-text cursor describing the position 
	from which virtualization will start. On output, cursor will be positioned 
	at the last CD record virtualized (i.e. the virtualized copy of the record 
	immediately preceding the given stop point).
puc_END: address of the point immediately before which virtualization must stop
pt_ctx: Input & Output. Address of information describing the rich-text 
	environment within which we're operating for the call. Information may be 
	updated as a result of this call.
RETURN:
	eus_ERR_INVLD_ARG if procedure's input context is obviously invalid
	eus_SUCCESS if successful
	the Notes API error code otherwise

--- revision history -------
3/14/03 PR: created			*/
static STATUS us_VirtualizePointToPoint( CdRecordCursor *const  pt_crsr, 
											const BYTE *const  puc_END, 
											RtfTrackingInfo *const  pt_ctx)	{
	ItemInfo * pt_itmFrontr, * pt_endVirtl, * pt_itm, * pt_item;
	const BYTE * puc_frontr, * puc_end, * puc;
	BOOL  f_itmGotLckd, f_frontrItmGotLckd;
	DWORD  ul_lenEndVirtl, ul, ul_len;
	STATUS  us_err;

	_ASSERTE( pt_crsr && pt_crsr->puc_location && pt_crsr->pt_item && 
									pt_crsr->us_recLength && puc_END && pt_ctx);

	//Ensure that the start point precedes the end point within our rich-text 
	//	environment. If the end point resides on the same item as the start 
	//	point...
	if (pt_crsr->pt_item->puc_start <= puc_END && puc_END < 
												pt_crsr->pt_item->puc_start + 
												pt_crsr->pt_item->ul_length)
		//if the end point falls before the start point...
		if (puc_END < pt_crsr->puc_location)
			//short-circuit with invalid-argument failure
			return eus_ERR_INVLD_ARG;
		//else we're dealing with a simple span of content (i.e. all's 
		//	encompassed within the same rich-text item)...
		else	{
			//virtualize the simple span of content marked out between the 
			//	start and end points
			puc = pt_crsr->puc_location;
			if (us_err = us_VirtualAppend( puc, puc_END - puc, pt_ctx, NULL, 
													memset( pt_crsr, NULL, 
													sizeof( RtfTrackingInfo))))
				return us_err;
			_ASSERTE( pt_crsr->pt_item);

			//determine the position of the last CD record virtualized
			if (pt_crsr->pt_item != pt_ctx->pt_endVirtual)
				do
					if (!( pt_crsr->pt_item = pt_crsr->pt_item->pt_next))
						return !eus_SUCCESS;
					else if (pt_crsr->pt_item == pt_ctx->pt_endVirtual)	{
						if (!f_ConstructStartCursor( NULL, pt_crsr->pt_item, 
																pt_crsr, NULL))
							return !eus_SUCCESS;
						break;
					}
				while (TRUE);
			puc_end = pt_ctx->pt_endVirtual->puc_start + 
											pt_ctx->pt_endVirtual->ul_length;
			while (pt_crsr->puc_location + pt_crsr->us_recLength < puc_end)	{
				AdvanceCdCursor( pt_crsr, NULL, NULL, NULL);
				_ASSERTE( pt_crsr->pt_item);
			}

			return eus_SUCCESS;
		} //if (puc_END < pt_crsr->puc_location)
	//else if the start point resides in virtuality...
	else if (pt_crsr->pt_item->i_type != mi_ACTUAL)	{
		//if the end point doesn't fall within the rest of virtuality...
		pt_itm = pt_crsr->pt_item;
		while (pt_itm = pt_itm->pt_next)
			if (pt_itm->puc_start && pt_itm->puc_start <= puc_END && puc_END < 
										pt_itm->puc_start + pt_itm->ul_length)
				break;
		if (!pt_itm)	{
			//determine coördinates of the current "actual frontier" point
			if (!f_LocateAbsoluteOffset( pt_ctx->ul_ActualFrontier, 
										pt_ctx->pt_Actuality, &pt_itmFrontr, 
										&puc_frontr, NULL, &f_frontrItmGotLckd))
				return eus_ERR_INVLD_ARG;

			//if the end point doesn't fall within the rest of actuality, 
			//	short-circuit with invalid-argument failure
			do
				if (pt_itm->puc_start && pt_itm->puc_start <= puc_END && 
												puc_END < pt_itm->puc_start + 
												pt_itm->ul_length)
					break;
			while (pt_itm = pt_itm->pt_next);
			if (!pt_itm)
				return eus_ERR_INVLD_ARG;
		} //if (!pt_itm)
	//else if the end point doesn't fall later in the flow of actuality, 
	//	short-circuit with invalid-argument failure
	}else	{
		pt_itm = pt_crsr->pt_item;
		while (pt_itm = pt_itm->pt_next)
			if (pt_itm->puc_start && pt_itm->puc_start <= puc_END && puc_END < 
										pt_itm->puc_start + pt_itm->ul_length)
				break;
			if (!pt_itm)
				return eus_ERR_INVLD_ARG;
	} //if (pt_crsr->pt_item->i_type != mi_ACTUAL)

	//virtualize any content from the start point up to the start of the item 
	//	whose content holds the end point...
	pt_endVirtl = pt_ctx->pt_endVirtual;
	ul_lenEndVirtl = pt_ctx->pt_endVirtual->ul_length;
	puc = pt_crsr->puc_location;
	pt_item = pt_crsr->pt_item;
	ul_len = pt_item->ul_length;
	do	{
		//determine the length of content to virtualize within the current item
		ul = pt_item->puc_start + ul_len - puc;

		//virtualize that content
		us_err = us_VirtualAppend( puc, ul, pt_ctx, NULL, NULL);

		//if the item containing the content just virtualized was locked in 
		//	memory for the sake of this operation, unlock it now
/*		if (f_itmGotLckd)	{
			UnlockItem( pt_item);
			f_itmGotLckd = FALSE;
		}
*/
		//if any error occurred during the virtualization, return its code to 
		//	the caller
		if (us_err)
			return us_err;

		//Move to the next item on our way toward the item containing the end, 
		//	specified byte location. If we're not on what was the last virtual 
		//	item when we started...
		if (pt_item != pt_endVirtl)	{
			//if a follow-on item within the item stream we're working with 
			//	is available...
			if (pt_item = pt_item->pt_next)	{
				//if necessary, make its contents available
				if (!(puc = pt_item->puc_start))	{
					puc = pt_item->puc_start = OSLockBlock( BYTE, 
														pt_item->bid_contents);
					f_itmGotLckd = TRUE;
				}

				//note the length of the content to be virtualized
				if (pt_item != pt_endVirtl)
					ul_len = pt_item->ul_length;
				else
					ul_len = ul_lenEndVirtl;
			} //if (pt_item = pt_ietm->pt_next)
		//else switch from virtuality to actuality
		}else	{
			pt_item = pt_itmFrontr;
			puc = puc_frontr;
			f_itmGotLckd = f_frontrItmGotLckd;
			ul_len = pt_item->ul_length;
		} //if (pt_item = pt_ietm->pt_next)
	} while (pt_item != pt_itm);

	//virtualize the last piece of content
	if (us_err = us_VirtualAppend( pt_itm->puc_start, puc_END - 
									pt_itm->puc_start, pt_ctx, NULL, memset( 
									pt_crsr, NULL, sizeof( RtfTrackingInfo))))
		return us_err;
	_ASSERTE( pt_crsr->pt_item);

	//determine the position of the last CD record virtualized
	if (pt_crsr->pt_item != pt_ctx->pt_endVirtual)
		do
			if (!( pt_crsr->pt_item = pt_crsr->pt_item->pt_next))
				return !eus_SUCCESS;
			else if (pt_crsr->pt_item == pt_ctx->pt_endVirtual)	{
				if (!f_ConstructStartCursor( NULL, pt_crsr->pt_item, pt_crsr, 
																		NULL))
					return !eus_SUCCESS;
				break;
			}
		while (TRUE);
	puc_end = pt_ctx->pt_endVirtual->puc_start + 
											pt_ctx->pt_endVirtual->ul_length;
	while (pt_crsr->puc_location + pt_crsr->us_recLength < puc_end)	{
		AdvanceCdCursor( pt_crsr, NULL, NULL, NULL);
		_ASSERTE( pt_crsr->pt_item);
	}

	return eus_SUCCESS;
} //us_VirtualizePointToPoint(


/** f_CursorToHotspotEnd( ***
Move cursor positioned at the beginning of a hotspot to the CD record marking 
the end of the hotspot.

--- parameters & return ----
uc_sig: Optional. Rich-text signature associated with they type of hotspot 
	being navigated. The signature may be either the begin or the end signature
	of the hotspot type (e.g. SIG_CD_V4HOTSPOTBEGIN or SIG_CD_V4HOTSPOTEND), 
	with the procedure understanding that a begin signature implies its end 
	counterpart. If no signature provided, procedure will attempt to determine 
	hotspot type or simply an end in another manner.
pt_crsr: Input & Output. Address of the cursor to be advanced. If uc_SIG not 
	provided, procedure will check whether cursor lies on a begin-hotspot CD 
	record (even if after being bumped forward first by f_BMP_FWD_1ST), and if 
	it does, procedure will seek its counterpart end record; otherwise 
	procedure will seek any hotspot-end record. Output is either an advanced 
	cursor or a cursor whose item pointer is nullified to indicate that the end 
	of the CD record stream has been reached.
pt_CTX: Optional Input. Address of the context of the rich-text information 
	being navigated. If passed in null, the cursor will not travel 
	automatically from a virtual or semi-virtual rich-text item to an ensuing 
	actual rich-text item.
f_BMP_FWD_1ST: Flag telling whether the passed-in starting cursor should be 
	moved to the next CD record before initiating the search for text.
pf_LftSemiVirtuality: Optional Output. Address of the flag in which to store 
	whether the cursor has advanced out of semi-virtual rich-text content. If 
	passed in null, the procedure will ignore this functionality.
pf_itmGotLckd: Optional. Address of the flag in which to store whether a 
	rich-text item was newly locked in the course of this procedure. If passed 
	in null, the procedure will ignore this functionality.
RETURN: TRUE if the corresponding hotspot-end record was located; FALSE 
	otherwise

--- side effect --------------
The rich-text environment information pointed to by pt_crsr or pt_CTX may be 
adjusted due to cursor advancement.

--- suggested enhancement ----
7/21/03 PR: when searching for a particular type of end-hotspot record, allow 
	nested hotspots of the same type to be skipped over so that the end-hotspot 
	record ultimately found does indeed correspond to the presumed beginning 
	hotspot record that gave rise to the procedure call at the given point in 
	the rich-text stream

--- revision history -------
7/21/03 PR
+ logic extension and concomitant signature adjustment to allow caller to 
  specify the type of hotspot to be navigated
+ token renaming

3/14/03 PR
+ logic extension to allow the starting cursor to start within, instead of just 
  at the start of, a hotspot and the end of the hotspot to be considered to be 
  any of the end-hotspot record signatures allowed through the various versions 
  of Notes
+ logic enhancement (and concomitant signature change) to allow an initial 
  "bump forward" of cursor if caller desires
+ logic improvement surrounding general "item got locked" functionality
+ minor documentation adjustment, token renaming

9/6/02 PR: listing format adjustment, token renaming, documentation adjustment
3/20/00 PR: created			*/
static BOOL f_CursorToHotspotEnd( BYTE  uc_sig, 
									CdRecordCursor *const  pt_crsr, 
									const RtfTrackingInfo *const  pt_CTX, 
									const BOOL  f_BMP_FWD_1ST, 
									BOOL *const  pf_LftSemiVirtuality, 
									BOOL *const  pf_itmGotLckd)	{
	BYTE  uc = NULL;
	BOOL  f_lftSemiVirtuality = FALSE, * pf_lftSemiVirtuality = NULL, 
			f_lckd = FALSE;
	ItemInfo * pt_lckd = NULL;

	_ASSERTE( (uc_sig ? uc_sig == muc_SIG_CDBEGIN || uc_sig == 
						muc_SIG_CDV4HOTSPOTBEGIN || uc_sig == 
						muc_SIG_CDHOTSPOTBEGIN || uc_sig == muc_SIG_CDEND || 
						uc_sig == muc_SIG_CDV4HOTSPOTEND || uc_sig == 
						muc_SIG_CDHOTSPOTEND : TRUE) && pt_crsr && 
						pt_crsr->puc_location && pt_crsr->pt_item && 
						pt_crsr->us_recLength);

	//as applicable, indicate as a default that semi-virtuality has not been 
	//	left and the memory associated with an item different from the current 
	//	item has not been locked
	if (pf_LftSemiVirtuality)	{
		*pf_LftSemiVirtuality = FALSE;
		pf_lftSemiVirtuality = &f_lftSemiVirtuality;
	}
	if (pf_itmGotLckd)
		*pf_itmGotLckd = FALSE;

	//if caller didn't tell us the type of hotspot we're to operate on, see if 
	//	the cursor can tell us
	uc = *(BYTE *) pt_crsr->puc_location;
	if (!uc_sig)
		uc_sig = uc;

	//if necessary and possible, determine the end-hotspot signature we're after
	if (uc_sig == muc_SIG_CDBEGIN)
		uc_sig = muc_SIG_CDEND;
	else if (uc_sig == muc_SIG_CDV4HOTSPOTBEGIN)
		uc_sig = muc_SIG_CDV4HOTSPOTEND;
	else if (uc_sig == muc_SIG_CDHOTSPOTBEGIN)
		uc_sig = muc_SIG_CDHOTSPOTEND;
	else if (uc_sig == uc && !(uc == muc_SIG_CDEND || uc == 
													muc_SIG_CDV4HOTSPOTEND || 
													uc == muc_SIG_CDHOTSPOTEND))
		uc_sig = NULL;

	//if caller requested an intial advance of the cursor to the next CD record 
	//	before starting the search...
	if (f_BMP_FWD_1ST)	{
		//Advance the cursor. If the available rich-text context has been 
		//	exhausted, return that the sought-for record was not found.
		AdvanceCdCursor( pt_crsr, pt_CTX, NULL, &f_lckd);
		if (!pt_crsr->pt_item)
			return FALSE;
		if (f_lckd)
			pt_lckd = pt_crsr->pt_item;

		//if caller didn't tell us the type of hotspot we're to operate on and 
		//	we couldn't glean it from the cursor's initial position, see if the 
		//	cursor's new position can tell us
		uc = *(BYTE *) pt_crsr->puc_location;
		if (!uc_sig)
			if (uc == muc_SIG_CDBEGIN)
				uc_sig = muc_SIG_CDEND;
			else if (uc == muc_SIG_CDV4HOTSPOTBEGIN)
				uc_sig = muc_SIG_CDV4HOTSPOTEND;
			else if (uc == muc_SIG_CDHOTSPOTBEGIN)
				uc_sig = muc_SIG_CDHOTSPOTEND;
	} //if (f_BMP_FWD_1ST)

	//cursor past any begin-hotspot record we may be starting on...
	while (uc == muc_SIG_CDBEGIN || uc == muc_SIG_CDV4HOTSPOTBEGIN || uc == 
													muc_SIG_CDHOTSPOTBEGIN)	{
		//Advance the cursor to the next CD record. If there isn't one, 
		//	short-circuit that the end-hotspot record wasn't found.
		AdvanceCdCursor( pt_crsr, pt_CTX, pf_lftSemiVirtuality, &f_lckd);
		if (!pt_crsr->pt_item)
			goto errJump;

		//if called for, set other outputs caller may have requested
		if (pf_lftSemiVirtuality && *pf_lftSemiVirtuality)	{
			*pf_LftSemiVirtuality = TRUE;
			pf_lftSemiVirtuality = NULL;
		}
		if (f_lckd)	{
			if (pt_lckd)
				UnlockItem( pt_lckd);
			pt_lckd = pt_crsr->pt_item;
		}

		uc = *(BYTE *) pt_crsr->puc_location;
	} //while (uc == muc_SIG_CDBEGIN || uc ==

	//look for a hotspot-end record...
	while (!( uc_sig ? uc == uc_sig : uc == muc_SIG_CDEND || uc == 
												muc_SIG_CDV4HOTSPOTEND || uc == 
												muc_SIG_CDHOTSPOTEND))	{
		//advance the cursor to the next CD record
		AdvanceCdCursor( pt_crsr, pt_CTX, pf_lftSemiVirtuality, pf_itmGotLckd);
		if (f_lckd)	{
			if (pt_lckd)
				UnlockItem( pt_lckd);
			pt_lckd = pt_crsr->pt_item;
		}

		//if we know we just left semi-virtuality, let caller know as well and 
		//	make it so we won't bother to look anymore
		if (pf_lftSemiVirtuality && *pf_lftSemiVirtuality)	{
			*pf_LftSemiVirtuality = TRUE;
			pf_lftSemiVirtuality = NULL;
		}

		//If no more CD records are available or if a hotspot-begin record is 
		//	encountered while we're seeking any end-hotspot, we have a weird 
		//	situation where another hotspot-begin CD record is found before a 
		//	corresponding hotspot-end CD record. This can be a valid 
		//	configuration, but we aren't sophisticated enough yet to handle it, 
		//	so call it an error.
		uc = *(BYTE *) pt_crsr->puc_location;
		if (!pt_crsr->pt_item || (!uc_sig ? uc == muc_SIG_CDBEGIN || uc == 
											muc_SIG_CDV4HOTSPOTBEGIN || uc == 
											muc_SIG_CDHOTSPOTBEGIN : FALSE))
			goto errJump;
	} //while (!( uc_sig ? uc == uc_sig : uc == muc_SIG_CDEND ||

	if (pf_itmGotLckd && pt_lckd)
		*pf_itmGotLckd = TRUE;

	return TRUE;

errJump:
	if (pt_lckd)
		UnlockItem( pt_lckd);
	if (pf_itmGotLckd)
		*pf_itmGotLckd = FALSE;
	return FALSE;
} //f_CursorToHotspotEnd(


/** eus_RemoveRtSpan( ***
Adjust the given rich-text environment such that the specified span of 
rich-text CD records are set to be removed from the field when field is 
committed.

--- parameters & return ----
pt_SPN: address of structure describing the span of rich-text CD records to be 
	removed
pt_ctx: Input & Output. Address of structure describing the rich-text 
	environment we're operating in. Structure will be updated as a result of 
	this call.
RETURN: eus_SUCCESS if no error occured; the Notes API error code otherwise

--- revision history -------
3/14/03 PR
+ made adjustment so that span's ending CD record is treated as needing 
  removal, instead of as only those before it needing removal
+ added standard documentation, token renaming
+ minor logic adjustment in support of signature change to 
  us_VirtualizeThruActualLocation()

12/13/98 PR: created		*/
STATUS eus_RemoveRtSpan( const CdRecordSpan *const  pt_SPN, 
						  RtfTrackingInfo *const  pt_ctx)	{
	DWORD  ul_begin, ul_end = eul_ERR_FAILURE;
	STATUS  us_err;

	if (!( pt_SPN && pt_SPN->t_crsrBgin.puc_location && 
										pt_SPN->t_crsrBgin.us_recLength && 
										pt_SPN->t_crsrBgin.pt_item && pt_ctx))
		return eus_ERR_INVLD_ARG;

//PGP development shortcuts: no offsets, only actuality involved
_ASSERTE( !pt_SPN->us_offstBgin && !pt_SPN->us_offstEnd && 
							pt_SPN->t_crsrBgin.pt_item->i_type == mi_ACTUAL && 
							pt_SPN->t_crsrEnd.pt_item->i_type == mi_ACTUAL);

	ul_begin = ul_GetAbsoluteOffset( pt_SPN->t_crsrBgin.puc_location, 
													pt_SPN->t_crsrBgin.pt_item, 
													pt_ctx->pt_Actuality);
_ASSERTE( ul_begin != eul_ERR_FAILURE);

	if (pt_SPN->t_crsrEnd.pt_item)	{
		CdRecordCursor t = pt_SPN->t_crsrEnd;
		BOOL  f_itmGotLockd;

		AdvanceCdCursor( &t, pt_ctx, NULL, &f_itmGotLockd);
		if (t.pt_item)	{
			ul_end = ul_GetAbsoluteOffset( t.puc_location,t.pt_item, 
														pt_ctx->pt_Actuality);
			if (f_itmGotLockd)
				UnlockItem( t.pt_item);
		}
	} //if (pt_SPN->t_crsrEnd.pt_item)

	if (us_err = us_VirtualizeThruActualLocation( 
											pt_SPN->t_crsrBgin.puc_location, 
											pt_ctx, NULL, NULL))
		return us_err;

	pt_ctx->ul_ActualFrontier = ul_end != eul_ERR_FAILURE ? ul_end : 
														mul_FRONTIER_NO_MORE;

	return eus_SUCCESS;
} //eus_RemoveRtSpan(


/** eus_ReplaceRtfWithStandardText( ***
Purpose is to adjust the passed-in rich-text context so that the current 
actual rich-text items will be replaced with the virtual rich-text items 
to be written by this fuction using the text content passed in.

--- parameters & return ----
PC: pointer to the null-terminated text content to be streamed into the 
	virtual rich-text items to be created by this function
f_TRANSLATE_LINEFEEDS: flag specifying whether CRLFs should be translated to 
	rich-text linefeeds when copying content (TRUE) or not (FALSE)
pt_context: Input & Output. Pointer to environment information about the 
	rich-text field being operated on. Its virtuality information will be 
	updated by this function.
RETURN: eus_SUCCESS if no error occured; the Notes API error code otherwise

--- revision history -------
9/12/99 PR
+ fixed bug that manifested itself when multiple CDTEXTs had to be written
+ fixed bug that manifested itself when PGP ASCII Armor exceeds 64K in length
+ added documentation

12/16/98 PR: created		*/
STATUS eus_ReplaceRtfWithStandardText( const char  PC[], 
										const BOOL  f_TRANSLATE_LINEFEEDS, 
										RtfTrackingInfo *const  pt_context)	{
	ItemInfo * pt_item = pt_context->pt_Actuality;
	DWORD  ul = 0;

	if (!( PC && pt_context && pt_item))
		return !eus_SUCCESS;

	//set the actual frontier to the end of actuality
	do
		ul += pt_item->ul_length + pt_item->ul_length % 2;
	while (pt_item = pt_item->pt_next);
	pt_context->ul_ActualFrontier = ul;

	//if any virtuality obtains, clear any items beyond a first item and 
	//	reset the length of the first item to the beginning of the item
	if (pt_item = pt_context->pt_Virtuality)	{
		if (pt_item->pt_next)	{
			ClearItemList( &pt_item->pt_next);
			pt_context->pt_endVirtual = pt_item;
		}
		pt_item->ul_length = sizeof( WORD);
	} //if (pt_item = pt_context->pt_Virtuality)

	return us_AppendTextToVirtuality( PC, NULL, f_TRANSLATE_LINEFEEDS, NULL, 
												NULL, NULL, pt_context, NULL);
} //eus_ReplaceRtfWithStandardText(


/** ul_getNearLinebreakOffset( ***
Purpose is to return the offset to the first character following the nearest, 
best linebreak candidate in an ASCII text run, accounting if necessary for 
translation of CRLFs to rich-text linefeed characters. If CRLF translation is 
specified, the nearest CRLF will be sought as the linebreak point. If a CRLF 
is not found or if CRLF translation is not specified, the nearest word break 
will be sought as the linebreak point. If all else fails, a linebreak point 
corresponding to the specified limit point will be returned.

--- parameters & return ----
PC: pointer to the null-terminated ASCII text run to be considered
ul_LIMIT: the maximum length at which a linebreak point may be returned
f_TRANSLATE_LINEFEEDS: flag specifying whether CRLFs should be translated to 
	rich-text linefeeds when copying content (TRUE) or not (FALSE)
RETURN: The offset to the first character following the nearest, best 
	linebreak candidate. If the linebreak is at a CRLF, the offset will be to 
	the beginning of the CRLF.

--- revision history -------
9/12/99 PR: created			*/
static unsigned long ul_getNearLinebreakOffset( 
										const char *const  PC, 
										unsigned long  ul_LIMIT, 
										const BOOL  f_TRANSLATE_LINEFEEDS)	{
	const char * pc, * pc_limit = NULL;
	DWORD  ul;

	_ASSERTE( PC && ul_LIMIT);

	//if we're translating linefeeds, the nearest linebreak is the nearest 
	//	linefeed, presuming one is available
	if (f_TRANSLATE_LINEFEEDS)	{
		//determine the pointer where the ultimate limit occurs in the 
		//	untranslated text
		TranslateCrlfText( PC, ul_LIMIT, &ul, NULL, &pc_limit);

		//Step back until a CRLF is encountered. If one is encountered, 
		//	determine and return the offset in terms of the *translated* 
		//	text. The "1" represents the length of a rich-text linefeed.
		pc = pc_limit;
		do
			if (memcmp( pc, epc_CRLF, eui_LEN_CRLF) == ei_SAME)
				return ul - (pc_limit - pc - eui_LEN_CRLF + 1);
		while (--pc > PC);
	} //if (f_TRANSLATE_LINEFEEDS)

	//the nearest linebreak is at the beginning of the nearest word, 
	//	presuming one is available
	if (!pc_limit)	{
		_ASSERTE( ul_LIMIT <= strlen( PC));

		pc_limit = PC + (ul = ul_LIMIT);
	}
	pc = pc_limit;
	while (--pc > PC)
		if (*pc == ec_SPACE)
			return ul - (pc_limit - pc + 1);

	//since there isn't a good linebreak, we'll just say the break is right 
	//	at the limit, probably intra-word
	return ul;
} //ul_getNearLinebreakOffset(


/** eus_ReplaceRtfSpanText( ***
Purpose is to replace an identified span of rich text with the contents of the 
provided, null-terminated text buffer.

--- parameters & return ------
pt_spn: Input & Output. Pointer to a rich-text span structure which holds the 
	beginning coördinates of the span to be replaced, and optionally, the 
	ending coördinates. If the ul_LEN_TEXT_TO_REPLACE parameter is provided, 
	passed-in ending coördinates are ignored and will be reset according to the 
	dimension of that parameter. Output tells replacement span.
ul_LEN_TXT_TO_REPL: Optional. The length of the text to in the rich-text field 
	to be replaced. Parameter required if null ending coördinates are provided 
	in the pt_spn parameter.
f_TRANSLT_LF: flag specifying whether rich-text linefeed and paragraph markers 
	should be translated to CRLFs when assessing content (TRUE) or not (FALSE)
PC: the replacement text content, null-terminated
pt_ctx: Input & Output. Pointer to tracking information about the operative 
	rich-text field.
RETURN:
	eus_SUCCESS if no errors occurred
	eus_ERR_INVLD_ARG if any passed-in parameter is obviously invalid
	the Notes API error code otherwise

--- suggested enhancement ----
9/6/02 PR: um, code out PDL'd logic

--- revision history ---------
3/14/03 PR: minor logic adjustment in support of signature change to 
	us_VirtualizeThruActualLocation()

9/6/02 PR
+ implemented replacement-span output
+ listing format adjustment, minor safety improvement, token renaming, 
  documentation adjustment

3/20/00 PR
+ fixed bug wherein content to be preserved in an ending CDTEXT was not being 
  handled properly
+ logic shortening, some by leveraging the new UnlockItem() procedure

9/12/99 PR
+ added support for replacement content in excess of MAXONESEGSIZE
+ documentation adjustment

12/8/98 PR: created			*/
STATUS eus_ReplaceRtfSpanText( CdRecordSpan *const  pt_spn, 
								const DWORD  ul_LEN_TXT_TO_REPL, 
								const BOOL  f_TRANSLT_LF, 
								const char  PC[], 
								RtfTrackingInfo *const  pt_ctx)	{
	CdRecordCursor  t_crsrBgin, t_crsrEnd, t_crsr;
	WORD  us_offstEnd = NULL, us_lenEndPrsrv = NULL;
	BYTE * puc, * puc_bgnSpnCdRec;
	DWORD  ul_lenRepl, ul;
	STATUS  us_err;
	BOOL  f_OneRcrdSpn = FALSE, f_ItmJustLockd;

	//If any of the parameters are obviously invalid, short-circuit with 
	//	general failure. A valid span requires either a content length or a 
	//	valid end cursor.
	if (!( pt_spn && PC && pt_ctx && pt_spn->t_crsrBgin.pt_item && 
										pt_spn->t_crsrBgin.puc_location && 
										(pt_spn->t_crsrEnd.puc_location && 
										pt_spn->t_crsrEnd.pt_item || 
										ul_LEN_TXT_TO_REPL) && 
										(pt_spn->t_crsrBgin.pt_item == 
										pt_spn->t_crsrEnd.pt_item ? 
										pt_spn->t_crsrBgin.puc_location < 
										pt_spn->t_crsrEnd.puc_location : TRUE)))
		return eus_ERR_INVLD_ARG;

	//now that the input seems okay, it's safe to initialize certain local 
	//	variables
	t_crsrBgin = pt_spn->t_crsrBgin;
	puc_bgnSpnCdRec = t_crsrBgin.puc_location;
	t_crsrEnd = pt_spn->t_crsrEnd;
	if (t_crsrEnd.puc_location && *t_crsrEnd.puc_location == muc_SIG_CDTEXT)
		us_offstEnd = pt_spn->us_offstEnd;

	//determine the length of the replacement content, accounting if needed for 
	//	CRLF to rich-text linefeed conversion
	if (f_TRANSLT_LF)
		TranslateCrlfText( PC, NULL, &ul_lenRepl, NULL, NULL);
	else
		ul_lenRepl = strlen( PC);

	//if the passed-in span is marked by offset instead of end cursor, 
	//	determine the appropriate end-cursor configuration
	if (ul_LEN_TXT_TO_REPL && !f_ProcessKnownLengthRtfText( t_crsrBgin, 
										pt_spn->us_offstBgin, 
										ul_LEN_TXT_TO_REPL, f_TRANSLT_LF, 
										pt_ctx, &t_crsrEnd, &us_offstEnd, NULL))
		return !eus_SUCCESS;
	_ASSERTE( t_crsrEnd.puc_location);

	//if a question, determine the length of text to preserve in the ending 
	//	CDTEXT record, and whether we're dealing with a span that involves only 
	//	one CDTEXT record and can be replaced with only one CDTEXT record
	ul = sizeof( CDTEXT) + ul_lenRepl;
	if (*t_crsrEnd.puc_location == muc_SIG_CDTEXT)	{
		us_lenEndPrsrv = t_crsrEnd.us_recLength - (sizeof( CDTEXT) + 
																us_offstEnd);
		f_OneRcrdSpn = *puc_bgnSpnCdRec == muc_SIG_CDTEXT && puc_bgnSpnCdRec == 
									t_crsrEnd.puc_location && ul + 
									pt_spn->us_offstBgin + us_lenEndPrsrv <= 
									mus_LENLIMIT_CDTEXT_REC;
	}

	//if the starting CD record of the span is not virtualized...
	if (t_crsrBgin.pt_item->i_type != mi_VIRTUAL)	{
		//Virtualize up through the CD record preceding the starting CD record 
		//	involved in the span. The actual frontier absolute offset will then 
		//	point to the starting CD record.
		if (us_err = us_VirtualizeThruActualLocation( puc_bgnSpnCdRec, pt_ctx, 
																	NULL, NULL))
			return us_err;
	//Else we have to deal with already virtualized content. And that's a lot 
	//	of complicated work.
	}else	{
return !eus_SUCCESS;	//PGP development shortcut
		//if the CD record following the span's ending CD record resides on an 
		//	item different from the item containing the starting CD record...
			//if the item containing the starting CD record is not the last 
			//	virtual item, set the virtuality member of a proxy 
			//	item-tracking structure to a copy of the item structure 
			//	associated with the starting CD record's item but with a null 
			//	next-item pointer

			//if the starting CD record is not the first record in the item...
				//reset the length of the item to point to the end of the CD 
				//	record preceding the starting CD record
			//else reset the length of the item to point to the beginning of 
			//	the item (i.e. no item content at this point)
		//else we need to recast the item bounding the rich-text span
			//in a proxy item-tracking structure, create an initial virtual 
			//	item and initialize it with the content on the item containing 
			//	the starting CD record that precedes the starting record

		//set an internal flag telling this procedure to nullify the beginning 
		//	cursor in the passed-in span structure to indicate its invalidity 
		//	to the caller
	} //if (t_crsrBgin.pt_item->i_type != mi_VIRTUAL)

	//if the starting coördinates of the span to be replaced fall within, not 
	//	at the beginning of, a CDTEXT...
	if (*puc_bgnSpnCdRec == muc_SIG_CDTEXT && pt_spn->us_offstBgin)	{
		WORD  us;
		CdRecordCursor  t_newStart;

		//determine the tentative length of the first replacement CDTEXT as the 
		//	sum of the length of a CDTEXT structure, the length of the content 
		//	which precedes the the beginning of the span in the initial CDTEXT, 
		//	the length of our replacement text, and if the ending coördinates 
		//	of the span fall within the same CDTEXT as the starting coördinates 
		//	do, the length of the content following the end of the span -- 
		//	tentative because the length cannot exceed the length limit we're 
		//	observing on CDTEXT records.
		ul += pt_spn->us_offstBgin;
		if (f_OneRcrdSpn)
			ul += us_lenEndPrsrv;

		//get a pointer at which we can virtualize a CDTEXT CD record of the 
		//	length just determined or of our limit length
		us = ul < mus_LENLIMIT_CDTEXT_REC ? (WORD) ul : mus_LENLIMIT_CDTEXT_REC;
		_ASSERTE( f_OneRcrdSpn ? ul == us : TRUE);
		if (us_err = us_GetVirtualCdRecAppendPointer( pt_ctx, us, &puc))
			return us_err;

		//Virtualize the initial CDTEXT structure that accounts for its 
		//	ultimate length, content included. Utilize the CDTEXT associated 
		//	with the beginning of the rich-text span to preserve the font 
		//	signature.
		memcpy( puc, puc_bgnSpnCdRec, sizeof( CDTEXT));
		((CDTEXT *) puc)->Header.Length = us;

		//virtualize the content that precedes the beginning of the span in the 
		//	initial CDTEXT
		memcpy( puc + sizeof( CDTEXT), puc_bgnSpnCdRec + sizeof( CDTEXT), 
														pt_spn->us_offstBgin);

		//Virtualize the replacement content, initializing the replacement-span 
		//	output along the way.
		t_newStart.puc_location = puc;
		t_newStart.pt_item = pt_ctx->pt_endVirtual;
		t_newStart.us_recLength = us;
		if (us_err = us_AppendTextToVirtuality( PC, ul_lenRepl, f_TRANSLT_LF, 
											&t_newStart, pt_spn->us_offstBgin, 
											NULL, pt_ctx, pt_spn))
			return us_err;

		//if the ending coördinates of the span fall within the same CDTEXT 
		//	that the starting coördinates do...
		if (f_OneRcrdSpn)	{
			//tack on the content following the end of the span
			memcpy( puc + ul - us_lenEndPrsrv, puc_bgnSpnCdRec + sizeof( 
									CDTEXT) + us_offstEnd + 1, us_lenEndPrsrv);

			//set the ending-offset coördinate of the replacement-span output
			pt_spn->us_offstEnd = (WORD) (ul - us_lenEndPrsrv - 
															sizeof( CDTEXT));

			//done with the virtualization of the CDTEXT, update the variable 
			//	that tracks the length of the virtual item being written to
			pt_ctx->pt_endVirtual->ul_length += 
									pt_ctx->pt_endVirtual->ul_length % 2 + ul;
		} //if (f_OneRcrdSpn)
	//else we construct our own CDTEXTs to hold the replacement content
	}else	{
//PGP development shortcut: assuming that the first record in the span is a 
//	CDTEXT. If it's not, return general failure.
if (*puc_bgnSpnCdRec != muc_SIG_CDTEXT)
	return !eus_SUCCESS;

		//Determine a FONTID that we can use in the CDTEXT we need to write. 
		//	Basically, we want the closest CDTEXT before (and including) the 
		//	beginning of the span. If one isn't available, we want the closest 
		//	one after the beginning CD record of the span (even if it's in the 
		//	span). If no CDTEXTs are available at all, we will use the Notes 
		//	default FONTID.

//PGP development shortcut: using the beginning, source CDTEXT's FONTID
		//virtualize the replacement content and note the span of the added 
		//	rich-text
		if (us_err = us_AppendTextToVirtuality( PC, ul_lenRepl, f_TRANSLT_LF, 
									NULL, NULL, ((CDTEXT *) 
									puc_bgnSpnCdRec)->FontID, pt_ctx, pt_spn))
			return us_err;
	} //if (*t_crsrBgin.puc_location == LOBYTE(

	//if some of the content in the CD record pointed to by the ending cursor 
	//	needs to be preserved and we're not working with a one-record span...
	if (!f_OneRcrdSpn && us_lenEndPrsrv)	{
		//get a pointer at which we can virtualize a CDTEXT containing only the 
		//	content to be preserved
		if (us_err = us_GetVirtualCdRecAppendPointer( pt_ctx, sizeof( CDTEXT) + 
														us_lenEndPrsrv, &puc))
			return us_err;

		//virtualize a CDTEXT based on the ending CDTEXT's format but 
		//	containing just the content not included in the specified span
		memcpy( puc, t_crsrEnd.puc_location, sizeof( CDTEXT));
		((CDTEXT *) puc)->Header.Length = sizeof( CDTEXT) + us_lenEndPrsrv;
		memcpy( puc + sizeof( CDTEXT), t_crsrEnd.puc_location + sizeof( 
									CDTEXT) + us_offstEnd + 1, us_lenEndPrsrv);

		//done with the virtualization of the CDTEXT, update the variable that 
		//	tracks the length of the virtual item being written to
		pt_ctx->pt_endVirtual->ul_length += pt_ctx->pt_endVirtual->ul_length % 
										2 + sizeof( CDTEXT) + us_lenEndPrsrv;
	} //if (!f_OneRcrdSpn && us_lenEndPrsrv)

	//if the CD record that follows the original ending CD record is 
	//	virtualized...
	t_crsr = t_crsrEnd;
	AdvanceCdCursor( &t_crsr, pt_ctx, NULL, &f_ItmJustLockd);
	if (t_crsr.pt_item && t_crsr.pt_item->i_type == mi_VIRTUAL)	{
return !eus_SUCCESS;		//PGP development shortcut
		//if that CD record is the first record in its item...
			//restore virtuality by setting the next pointer of the last item 
			//	in the proxy virtuality equal to the item containing this CD 
			//	record that follows the ending CD record
		//else
			//virtualize that CD record following the original ending CD record 
			//	and all ensuing records up to the end of its item

			//set the next pointer of the last proxy item equal to the item 
			//	pointed to by the next pointer of the item we just virtualized
	//else if a following CD record exists (in Actuality)...
	}else if (t_crsr.pt_item)	{
		//reset the actual-frontier absolute offset to the beginning of the CD 
		//	record following the original ending CD record
		pt_ctx->ul_ActualFrontier = ul_GetAbsoluteOffset( 
									t_crsr.puc_location, t_crsr.pt_item, 
									pt_ctx->pt_Actuality);

		//if the item containing the following CD record was just locked in 
		//	memory for the sake of this operation, unlock it now
		if (f_ItmJustLockd)
			UnlockItem( t_crsr.pt_item);
	//else reset the actual-frontier absolute offset to the ultimate length of 
	//	the rich-text field
	}else
		pt_ctx->ul_ActualFrontier = ul_GetAbsoluteOffset( 
									t_crsrEnd.puc_location, 
									t_crsrEnd.pt_item, pt_ctx->pt_Actuality) + 
									t_crsrEnd.us_recLength + 
									t_crsrEnd.us_recLength % 2;

	//if either the original starting CD record or the CD record following the 
	//	original ending CD record was virtualized...
		//if the first item of the proxy virtuality is the same as the original 
		//	item containing the starting CD record...
			//discard the items that fall between the original item containing 
			//	the starting CD record and the item pointed to by the next 
			//	pointer in the last proxy-virtual item
		//else
			//if the original item containing the starting CD record is the 
			//	first virtual item...
				//set the virtuality member of the main item-tracking structure 
				//	to point to the first proxy virtual item
			//else locate in the original virtuality the item whose next 
			//	pointer specifies this original item and reset its next pointer 
			//	to the first proxy virtual item

			//discard the original item and any that follow it up through the 
			//	one pointed to by the next pointer in the last proxy-virtual 
			//	item

	return eus_SUCCESS;
} //eus_ReplaceRtfSpanText(


/** us_AppendTextToVirtuality( ***
Appends a null-terminated buffer of text content to the current virtual 
rich-text stream. The procedue does not concern itself with paragraph styles 
even though it does break up larger content streams into separate paragraphs.

--- parameters & return ----
PC: address of the text to be appended, null-terminated
ul_lenRplcmt: Optional. The length of text to be written. If null, all of the 
	text content will be written out.
f_XLATE_LNFDS: flag telling whether or not rich-text linefeed and paragraph 
	markers should be translated to CRLFs when assessing content
pt_crsrStrtCdTxt: Optional. Input & Output. Address of a rich-text cursor 
	positioned at a CDTEXT which has already been started and to which text 
	should continue to be added up to the length specified in the us_recLength 
	member. Such a starting CDTEXT must be positioned at the end of the last 
	obtaining virtual item, and the ul_length member of that item must not yet 
	include this new CDTEXT being written. If NULL, a new CDTEXT will be 
	fashioned to which the provided text will begin to be written.

	If the length of the starting CDTEXT is shortened from that allocated by 
	the caller (because, for instance, an appropriate linebreak falls before 
	the end of the allocated space) the us_recLength member of the cursor will 
	be updated to reflect the revised length. Note that this adjustment will 
	only occur if more than one CDTEXT (and thus paragraph) needs to be used.
us_lenStrtTxt: Optional. The length of text content already written into 
	the CDTEXT pointed to by t_startCdText.
ul_FONT: Optional. The FONTID that should be used with new CDTEXTs written by 
	this function. If omitted and pt_crsrStrtCdTxt is provided, procedure will 
	default to the FONTID contained in the starting CDTEXT. If omitted and no 
	pt_crsrStrtCdTxt is provided, procedure will default to the monospaced 
	FOREIGN_FONT_ID provided by Notes.
pt_ctx: Input & Output. Address of tracking information about the operative 
	rich-text field. The ul_length member of the last virtual item (pointed to 
	by pt_endVirtual) will be updated to reflect CD records fully written. Note 
	that a first CDTEXT described by pt_crsrStrtCdTxt will not be fully written 
	if all the replacement content specified fails to fill up the space 
	allocated for the CDTEXT content by the caller.
pt_spn: Optional Output. The span of rich-text virtualized by this call.
RETURN: eus_SUCCESS if no error occured; the Notes API error code otherwise

--- revision history -------
3/14/03 PR
+ minor logic adjustment in support of signature change to us_VirtualAppend()
+ documentation adjustment, token renaming, listing format adjustment

9/6/02 PR
+ listing format adjustment, minor documentation adjustment, token renaming
+ removed remmed-out code

3/20/00 PR
+ fixed a couple logic bugs, I think
+ token renaming, logic shortening

9/12/99 PR: created		*/
static STATUS us_AppendTextToVirtuality( 
									const char  PC[], 
									DWORD  ul_lenRplcmt, 
									const BOOL  f_XLATE_LNFDS, 
									CdRecordCursor *const  pt_crsrStrtCdTxt, 
									const WORD  us_lenStrtTxt, 
									const FONTID  ul_FONT, 
									RtfTrackingInfo *const  pt_ctx, 
									CdRecordSpan *const  pt_spn)	{
	CDTEXT  t = {{SIG_CD_TEXT}};
	BYTE * puc;
	const char * pc = PC;
	STATUS  us_err;

	_ASSERTE( PC && pt_ctx && (pt_crsrStrtCdTxt ? 
						pt_crsrStrtCdTxt->pt_item && 
						pt_crsrStrtCdTxt->puc_location && 
						*pt_crsrStrtCdTxt->puc_location == muc_SIG_CDTEXT && 
						pt_crsrStrtCdTxt->us_recLength == 
						((WSIG *) pt_crsrStrtCdTxt->puc_location)->Length && 
						pt_crsrStrtCdTxt->pt_item == pt_ctx->pt_endVirtual && 
						pt_crsrStrtCdTxt->pt_item->puc_start + 
						pt_crsrStrtCdTxt->pt_item->ul_length + 
						pt_crsrStrtCdTxt->pt_item->ul_length % 2 == 
						pt_crsrStrtCdTxt->puc_location : TRUE) && 
						(ul_lenRplcmt && !f_XLATE_LNFDS ? 
						ul_lenRplcmt <= strlen( PC) : TRUE));

	if (pt_spn)
		memset( pt_spn, NULL, sizeof( CdRecordSpan));

	//if the length of the text to be written was not specified by the caller, 
	//	default it to the length implied by the length of the specified 
	//	null-terminated text buffer
	if (!ul_lenRplcmt)
		if (f_XLATE_LNFDS)
			TranslateCrlfText( PC, NULL, &ul_lenRplcmt, NULL, NULL);
		else
			ul_lenRplcmt = strlen( PC);

	//if a starting CDTEXT has been provided...
	if (pt_crsrStrtCdTxt)	{
		const WORD  us_LEN_REMAIN = pt_crsrStrtCdTxt->us_recLength - 
												sizeof( CDTEXT) - us_lenStrtTxt;

		DWORD  ul_len = ul_lenRplcmt;

		//Unless the length of the content to write has been specified and the 
		//	length will fit into the remaining content space available in the 
		//	CDTEXT, determine the offset to the best place to break the text 
		//	stream such that we conform to the length limit we have on the 
		//	CDTEXT. If the offset differs from the length limit, adjust the 
		//	cursor's and CDTEXT's length specifiers.
		if (!ul_lenRplcmt || ul_lenRplcmt > us_LEN_REMAIN)	{
			if ((ul_len = ul_getNearLinebreakOffset( PC, us_LEN_REMAIN, 
											f_XLATE_LNFDS)) < 	us_LEN_REMAIN)
				((WSIG *) pt_crsrStrtCdTxt->puc_location)->Length = 
											pt_crsrStrtCdTxt->us_recLength -= 
											us_LEN_REMAIN - (WORD) ul_len;
			_ASSERTE( ul_len <= us_LEN_REMAIN);
		} //if (!ul_lenRplcmt ||

		//add the length of content to the current CDTEXT
		puc = pt_crsrStrtCdTxt->puc_location + sizeof( CDTEXT) + us_lenStrtTxt;
		if (f_XLATE_LNFDS)	{
			DWORD  ul;

			TranslateCrlfText( PC, ul_len, &ul, puc, &pc);
			_ASSERTE( ul == ul_len);
		}else	{
			memcpy( puc, PC, ul_len);
			pc += ul_len;
		} //if (f_XLATE_LNFDS)

		//if requested by caller, set the starting coördinates of the rich-text 
		//	span being written
		if (pt_spn)	{
			pt_spn->t_crsrBgin = *pt_crsrStrtCdTxt;
			pt_spn->us_offstBgin = us_lenStrtTxt;
		}

		//if there's no excess content space in this CDTEXT, update the length 
		//	member of the end virtual item to reflect the full CD record added
		if (ul_lenRplcmt >= us_LEN_REMAIN)
			pt_ctx->pt_endVirtual->ul_length += 
									pt_ctx->pt_endVirtual->ul_length % 2 + 
									sizeof( CDTEXT) + us_lenStrtTxt + ul_len;

		//if all text content has been written, set, if requested by caller, 
		//	the ending coördinates of the rich-text span that was written, then  
		//	short-circuit with success
		if (ul_len == ul_lenRplcmt)	{
			if (pt_spn)
				pt_spn->t_crsrEnd = *pt_crsrStrtCdTxt;
			return eus_SUCCESS;
		}

		//adjust the length of the content remaining to be written by the 
		//	amount just written, accounting for a CRLF linebreak if necessary 
		//	(ul_getNearLinebreakOffset returns offsets to the *beginning* of 
		//	CRLF linebreaks)
		ul_lenRplcmt -= ul_len;
		if (f_XLATE_LNFDS && memcmp( pc, epc_CRLF, eui_LEN_CRLF) == ei_SAME)	{
			pc += eui_LEN_CRLF;
			ul_lenRplcmt -= 1;	//i.e. the length of a rich-text linefeed
		}

		//Since we know now that more than just the starting CDTEXT is needed 
		//	to accommodate the replacement content, virtualize a CDPARAGRAPH 
		//	record, continuing to use the paragraph style currently in effect, 
		//	and update the length member of the end virtual item to reflect the 
		//	addition. We do this to ensure that we will comply with the 64K 
		//	limit on paragraphs.
		if (us_err = us_VirtualAppend( (BYTE *) &mt_PARAGRAPH, sizeof( 
											CDPARAGRAPH), pt_ctx, NULL, NULL))
			return us_err;
	} //if (pt_crsrStrtCdTxt->pt_item)

	//set the FONTID of ensuing CDTEXTS to be written
	if (ul_FONT)
		t.FontID = ul_FONT;
	else if (pt_crsrStrtCdTxt)
		t.FontID = ((CDTEXT *) pt_crsrStrtCdTxt->puc_location)->FontID;
	else
		t.FontID = FOREIGN_FONT_ID;

	//As necessary, break up the replacement content into multiple paragraphs. 
	//	We can't simply break it up into multiple CDTEXTs because of the 64K 
	//	limit on paragraphs.
	while (ul_lenRplcmt + sizeof( CDTEXT) > mus_LENLIMIT_CDTEXT_REC)	{
		//determine offset to the best place to break the text stream such that 
		//	we conform to the length limit we have on the CDTEXT
		const WORD  US = (WORD) ul_getNearLinebreakOffset( pc, 
											mus_LENLIMIT_CDTEXT_REC - sizeof( 
											CDTEXT), f_XLATE_LNFDS), 
					us_LEN_REC = sizeof( CDTEXT) + US;

		//get a pointer at which we can virtualize the CDTEXT
		if (us_err = us_GetVirtualCdRecAppendPointer( pt_ctx, us_LEN_REC + 
												us_LEN_REC % 2 + 
												sizeof( CDPARAGRAPH), &puc))
			return us_err;

		//finish off the CDTEXT structure to be used with the replacement text
		t.Header.Length = us_LEN_REC;
		memcpy( puc, &t, sizeof( CDTEXT));
		if (f_XLATE_LNFDS)
			TranslateCrlfText( pc, US, NULL, puc += sizeof( CDTEXT), &pc);
		else
			memcpy( puc += sizeof( CDTEXT), pc, US);

		//if requested by caller and not already done, set the starting 
		//	coördinates of the rich-text span being written
		if (pt_spn && !pt_spn->t_crsrBgin.pt_item)	{
			pt_spn->t_crsrBgin.puc_location = puc;
			pt_spn->t_crsrBgin.pt_item = pt_ctx->pt_endVirtual;
			pt_spn->t_crsrBgin.us_recLength = t.Header.Length;
		}

		//virtualize a CDPARAGRAPH record, continuing to use the paragraph 
		//	style currently in effect
		memcpy( puc + US + us_LEN_REC % 2, &mt_PARAGRAPH, sizeof( CDPARAGRAPH));

		//done with the virtualization of the CDTEXT, update the variable that 
		//	tracks the length of the virtual item being written to
		pt_ctx->pt_endVirtual->ul_length += pt_ctx->pt_endVirtual->ul_length % 
											2 + us_LEN_REC + us_LEN_REC % 2 + 
											sizeof( CDPARAGRAPH);

		//adjust the length of the content remaining to be written by the 
		//	amount just written, accounting for a CRLF linebreak if necessary 
		//	(ul_getNearLinebreakOffset returns offsets to the *beginning* of 
		//	CRLF linebreaks)
		ul_lenRplcmt -= US;
		if (f_XLATE_LNFDS && memcmp( pc, epc_CRLF, eui_LEN_CRLF) == ei_SAME)	{
			pc += eui_LEN_CRLF;
			ul_lenRplcmt -= 1;	//i.e. the length of a rich-text linefeed
		}
	} //while (ul_lenRplcmt + sizeof( CDTEXT)

	//get a pointer at which we can virtualize the last CDTEXT to be appended
	if (us_err = us_GetVirtualCdRecAppendPointer( pt_ctx, sizeof( CDTEXT) + 
															ul_lenRplcmt, &puc))
		return us_err;

	//finish off the CDTEXT structure to be used
	t.Header.Length = (WORD) (ul_lenRplcmt + sizeof( CDTEXT));

	//virtualize a separated CDTEXT record involving the controlling CDTEXT 
	//	format and the rest of the output text
	memcpy( puc, &t, sizeof( CDTEXT));
	if (f_XLATE_LNFDS)
		TranslateCrlfText( pc, ul_lenRplcmt, NULL, puc + sizeof( CDTEXT), NULL);
	else
		memcpy( puc + sizeof( CDTEXT), pc, ul_lenRplcmt);

	//if requested by caller, set the ending coördinates of the rich-text span 
	//	that's been written and, if not already done, the beginning coördinates
	if (pt_spn)	{
		pt_spn->t_crsrEnd.puc_location = puc;
		pt_spn->t_crsrEnd.pt_item = pt_ctx->pt_endVirtual;
		pt_spn->t_crsrEnd.us_recLength = t.Header.Length;
		if (!pt_spn->t_crsrBgin.pt_item)
			pt_spn->t_crsrBgin = pt_spn->t_crsrEnd;
	} //if (pt_spn)

	//done with the virtualization of the CDTEXT, update the variable that 
	//	tracks the length of the virtual item being written to
	pt_ctx->pt_endVirtual->ul_length += pt_ctx->pt_endVirtual->ul_length % 2 + 
												sizeof( CDTEXT) + ul_lenRplcmt;

	return eus_SUCCESS;
} //us_AppendTextToVirtuality(


/** ul_GetAbsoluteOffset( ***
Determine the number of bytes from the start of a rich-text item list to a 
specified location falling within the confines of that list.

--- parameters & return ----
PUC: the location up to which is to be measured
pt_ITM: address of the structure describing the rich-text item containing the 
	location to be measured up to
pt_ITM_LIST: address of the structure describing the head rich-text item in a 
	list of such items of which the item containing the specified location is a 
	member
RETURN: the byte-offset to the specified location

--- revision history -------
3/14/03 PR
+ completed standard documentation
+ token renaming

11/30/98 PR: created		*/
static DWORD ul_GetAbsoluteOffset( const BYTE *const  PUC, 
									const ItemInfo *const  pt_ITM, 
									const ItemInfo *const  pt_ITM_LIST)	{
	const ItemInfo * pt_itm = pt_ITM_LIST;
	DWORD  ul_len = NULL;

	_ASSERTE( PUC && pt_ITM && pt_ITM_LIST);

	while (!( pt_itm == pt_ITM && pt_itm))	{
		//remember, <item>.ul_length includes a TYPE_COMPOSITE header (2 bytes)
		ul_len += pt_itm->ul_length + pt_itm->ul_length % 2;

		pt_itm = pt_itm->pt_next;
	}

	return pt_itm ? ul_len + (PUC ? PUC - pt_ITM->puc_start : 
										pt_ITM->ul_length) : eul_ERR_FAILURE;
} //ul_GetAbsoluteOffset(


/** TranslateCrlfText( ***
Purpose is to traverse ASCII text, translating embedded CRLF linefeeds to 
equivalent rich-text ready text as progress is made. All this means is that 
CRLFs are converted to rich-text linefeeds (the null character) inside of the 
specified length constraint.

--- parameters ---------
PC: pointer to the ASCII text to be translated
ul_LIMIT: Optional. Limit on how much _translated_ text to accumulate. If 
	omitted, it is assumed that the ASCII text should be translated in full.
pul_length: Optional. Address of variable in which to return the length of 
	the text _translated_. Will not exceed a provided ul_LIMIT.
puc_write: Optional. Address of a buffer in which to write translated text.
ppc_ending: Optional. Address of the pointer variable in which to return the 
	position in the original ASCII of the character immediately following the 
	last character translated.

--- revision history ---
3/20/00 PR: documentation adjustment
9/12/99 PR: added documentation
12/16/98 PR: created	*/
static void TranslateCrlfText( const char *const  PC, 
								DWORD  ul_LIMIT, 
								DWORD *const  pul_length, 
								BYTE *const  puc_write, 
								const char * *const  ppc_ending)	{
	char * pc;
	DWORD  ul_traversed = NULL, ul_lenTranslated = NULL, ul = NULL;
	BOOL  f_LimitHit = FALSE;

	_ASSERTE( PC && (pul_length || puc_write));

	if (pul_length)
		*pul_length = NULL;
	if (puc_write)
		*puc_write = NULL;
	if (ppc_ending)
		*ppc_ending = NULL;
	
	//loop through the passed-in content, from CRLF to CRLF, processing 
	//	as requested
	while (!f_LimitHit && (pc = strstr( PC + ul_traversed, epc_CRLF)))	{
		//determine how much input content we just traversed in moving to 
		//	the current CRLF
		ul = pc - (PC + ul_traversed);

		if (f_LimitHit = ul_LIMIT && ul_lenTranslated + ul >= ul_LIMIT)
			ul = ul_LIMIT - ul_lenTranslated;

		//if we're suppossed to copy translated content using the passed-in 
		//	pointer, copy the section of text we just traversed plus the CRLF 
		//	translated to a rich-text linefeed
		if (puc_write)	{
			memcpy( puc_write + ul_lenTranslated, PC + ul_traversed, ul);
			if (!f_LimitHit)
				puc_write[ ul_lenTranslated + ul] = mc_LINEFEED_RICHTEXT;
		}

		//increment the translated length based on the offset from where we 
		//	started searching plus the translated length of the CRLF we found
		ul_lenTranslated += ul + (!f_LimitHit ? 1 : 0);

		//increment the offset into the input content so our next pass will 
		//	be ready to go
		ul_traversed += ul + (!f_LimitHit ? eui_LEN_CRLF : 0);
	} //while (!f_LimitHit && pc = strstr( PC + ul_traversed,

	//if requested and input content remains, copy any trailing input content 
	//	containing no CRLFs using the passed-in pointer
	if (!f_LimitHit && (ul = strlen( PC) - ul_traversed) && puc_write)
		memcpy( puc_write + ul_lenTranslated, PC + ul_traversed, ul);

	//if requested, set the caller's length variable with the size of the 
	//	input content with CRLFs translated to rich-text linefeeds
	if (pul_length)
		*pul_length = ul_lenTranslated + (!f_LimitHit ? ul : 0);
	if (ppc_ending)
		*ppc_ending = PC + ul_traversed;
} //TranslateCrlfText(


/** us_GetVirtualCdRecAppendPointer( ***
Delivers a location pointer that the caller can use to append a custom-built CD 
record of a specified maximum length to the end of current virtual content. (CD 
records cannot be split between items.)

--- parameters & return ----
pt_ctx: Input & Output. Address of tracking information about the operative 
	rich-text field. The pt_endVirtual member may be updated by the procedure.
ul_LEN_NEEDED: the length of _item_ content that must be available beginning at 
	the returned location pointer
ppuc: Output. Memory address of the caller's pointer variable that this 
	procedure should set.
RETURN: eus_SUCCESS if no error occured; the Notes API error code otherwise

--- note -------------------
If a follow-on virtual item must be created, the memory associated with the 
former ending virtual item will not be unlocked.

--- revision history -------
3/14/03 PR: listing format adjustment, token renaming
3/20/00 PR: accommodated the signature change to us_StartVirtualItemList()
12/8/98 PR: created			*/
static STATUS us_GetVirtualCdRecAppendPointer( RtfTrackingInfo *const  pt_ctx, 
												const DWORD  ul_LEN_NEEDED, 
												BYTE * *const  ppuc)	{
	ItemInfo * pt_itm;
	int  i_limit, i;
	STATUS  us_err;

	_ASSERTE( pt_ctx && ppuc);

	//if the length to accommodate exceeds the maximum size of a rich-text 
	//	item, return general failure
	if (ul_LEN_NEEDED > mul_MAXLEN_RICH_TEXT_ITEM)
		return !eus_SUCCESS;

	//if virtuality hasn't been started yet, start it now
	if (!pt_ctx->pt_Virtuality)
		if (us_err = us_StartVirtualItemList( NULL, pt_ctx))
			return us_err;

	//determine the operative maximum length in effect for the last virtual 
	//	item currently underway
	i_limit = (pt_itm = pt_ctx->pt_endVirtual)->ul_length <= 
											mul_LMT_COMFY ? mul_LMT_COMFY : 
											mul_LMT_STRETCH;

	//if the length to accommodate fits within the space still available within 
	//	the last virtual item...
	if (i_limit - (i = pt_itm->ul_length + pt_itm->ul_length % 2) > 
															(int) ul_LEN_NEEDED)
		//set the location pointer to where the caller's custom writing 
		//	(virtualization) should begin
		*ppuc = pt_itm->puc_start + i;
	//else we need to string on an additional virtual item so that the 
	//	requested length can be accommodated
	else	{
		//create a new end virtual item and update the tracking information 
		//	accordingly
		if (us_err = us_InsertNextVirtualItem( pt_itm, mi_REGULAR_VIRTUAL, 
																(WORD) NULL))
			return us_err;
		pt_itm = pt_ctx->pt_endVirtual = pt_itm->pt_next;

		//set the location pointer to where the caller's custom writing 
		//	(virtualization) should begin
		*ppuc = pt_itm->puc_start + sizeof( WORD);
	} //if (ul_limit - (ul = pt_itm->ul_length +

	return eus_SUCCESS;
} //us_GetVirtualCdRecAppendPointer(


/** eus_CommitChangedRtf( ***
Commit a rich-text field that has undergone changes to its underlying note, 
thus converting all virtuality to actuality.

--- parameters & return ----
h_NOTE: handle to the note on which the specified rich-text field exists
pc_ITMNM: address of the name of the rich-text field to be replaced
pt_ctx: Input & Output. Address of the rich-text context structure that 
	describes the rich-text field being manipulated. Structure will be reset to 
	reflect the committal (i.e. virtual content becomes actual content).
RETURN: eus_SUCCESS if no error occured; the Notes API error code otherwise

--- revision history -------
3/14/03 PR
+ minor signature adjustment to provide appropriate constness
+ listing format adjustment, minor documentation adjustment, token renaming

3/20/00 PR
+ safety & functionality improvement of having the rich-text field's context 
  structure updated to reflect work done by the procedure
+ logic adjustment to accommodate creation, not replacement, of a field
+ documentation improvement

1/16/99 PR: created			*/
STATUS eus_CommitChangedRtf( const NOTEHANDLE  h_NOTE, 
								const char  pc_ITMNM[], 
								RtfTrackingInfo *const  pt_ctx)	{
	ItemInfo * pt_item = pt_ctx->pt_Actuality, * pt_itm, * * ppt_itm, * pt_nxt;
	STATUS  us_err;

	if (!pt_ctx->pt_Virtuality)
		return eus_SUCCESS;

	if (us_err = us_VirtualizeRestOfRtf( pt_ctx, NULL))
		return us_err;

	//Remove the current field items on the note. Could use NSFItemDelete, but 
	//	doing it by the BLOCKIDs we already have probably saves cycles.
	if (pt_ctx->pt_Actuality)	{
		UnlockItems( pt_ctx->pt_Actuality, NULL);
		do
			if (us_err = NSFItemDeleteByBLOCKID( (NOTEHANDLE) h_NOTE, 
									((ItemInfo_Actual *) pt_item)->bid_item))
				return us_err;
		while (pt_item = pt_item->pt_next);
	}

	//For each virtual item, actualize it and update the context structure 
	//	accordingly...
	UnlockItems( pt_ctx->pt_Virtuality, NULL);
	pt_item = pt_ctx->pt_Virtuality;
	ppt_itm = &pt_ctx->pt_Actuality;
	do	{
		//if a follow-on information structure needs to be allocated for the 
		//	soon-to-be actual rich-text item, do so now
		if (!*ppt_itm)	{
			if (!( *ppt_itm = malloc( sizeof( ItemInfo_Actual))))	{
				if (pt_ctx->pt_Actuality)
					FreeItemList( &pt_ctx->pt_Actuality);
				return !eus_SUCCESS;
			}
			(*ppt_itm)->pt_next = NULL;
		}

		//copy the virtual item's information structure into the next available 
		//	actual-item information structure and reset properties appropriately
		pt_nxt = (pt_itm = *ppt_itm)->pt_next;
		*pt_itm = *pt_item;
		pt_itm->pt_next = pt_nxt;
		pt_itm->i_type = mi_ACTUAL;

		//Actualize the constructed virtual item by appending it to the note. 
		//	As a safety measure, note that the state of the "virtual" item has 
		//	now changed to actual.
		if (us_err = NSFItemAppendByBLOCKID( (NOTEHANDLE) h_NOTE, ITEM_SIGN, 
								(char *) pc_ITMNM, (WORD) strlen( pc_ITMNM), 
								pt_itm->bid_contents, pt_itm->ul_length, 
								&((ItemInfo_Actual *) pt_itm)->bid_item))
			return us_err;
		pt_item->i_type = mi_ACTUAL;

		//in preparation for the next pass, get the address of the pointer to 
		//	the next "actual" item information structure which may be already 
		//	allocated
		ppt_itm = &pt_itm->pt_next;
	} while (pt_item = pt_item->pt_next);

	//do final cleanup, with the result that everything previously virtual is 
	//	reflected now as actual
	if (pt_itm->pt_next)
		FreeItemList( &pt_itm->pt_next);
	FreeItemList( &pt_ctx->pt_Virtuality);
	pt_ctx->ul_ActualFrontier = sizeof( WORD);
	pt_ctx->pt_endVirtual = NULL;

	return eus_SUCCESS;
} //eus_CommitChangedRtf(


/** us_VirtualizeRestOfRtf( ***


--- parameters & return ----

RETURN: eus_SUCCESS if no error occured; the Notes API error code otherwise

--- side effect ------------

--- revision history -------
3/14/03 PR: minor logic adjustment in support of signature change to 
	us_VirtualizeThruActualLocation()

3/20/00 PR
+ changed signature to allow wrting to a virtual context that does not belong 
  to the source context
+ logic shortening, some by leveraging the new UnlockItem() procedure
+ documentation adjustment, token renaming

early 1999 PR: created			*/
//DOC!!
static STATUS us_VirtualizeRestOfRtf( RtfTrackingInfo *const  pt_src, 
										RtfTrackingInfo * pt_trgt)	{
	ItemInfo * pt_item;
	BOOL  f_ItemJustLocked = FALSE;
	DWORD  ul = NULL;
	STATUS  us_err = eus_SUCCESS;

	_ASSERTE( pt_src);

	//if there's no more actual content to virtualize, short-circuit with 
	//	success
	if (!pt_src->pt_Actuality || pt_src->ul_ActualFrontier == 
														mul_FRONTIER_NO_MORE)
		return eus_SUCCESS;

	if (!pt_trgt)
		pt_trgt = pt_src;

	//determine the total physical length of the current (actual) rich-text 
	//	field
	pt_item = pt_src->pt_Actuality;
	while (pt_item->pt_next)	{
		ul += ul % 2 + pt_item->ul_length;
		pt_item = pt_item->pt_next;
	}
	ul += ul % 2 + pt_item->ul_length;

	//if there's more actual content to virtualize...
	if (pt_src->ul_ActualFrontier < ul)	{
		//if necessary, lock in the last actual item
		if (!pt_item->puc_start)	{
			pt_item->puc_start = OSLockBlock( BYTE, pt_item->bid_contents);
			f_ItemJustLocked = TRUE;
		}

		//virtualize up through the end of the last actual item
		us_err = us_VirtualizeThruActualLocation( pt_item->puc_start + 
									pt_item->ul_length, pt_src, pt_trgt, NULL);

		//if we locked the last actual item, release it now
		if (f_ItemJustLocked)
			UnlockItem( pt_item);
	} //if (pt_src->ul_ActualFrontier < ul)

	//note that there's no more actual content to virtualize
	if (!us_err)
		pt_src->ul_ActualFrontier = mul_FRONTIER_NO_MORE;

	return us_err;	//probably success
} //us_VirtualizeRestOfRtf(


/** us_VirtualizeThruActualLocation( ***
Virtualize _actual_ rich-text content up to a specified point and thus advance 
the "actual frontier" which demarcates virtuality (changed rich-text) from 
actuality (the current rich-text stored in the field). 

--- parameters & return ----
puc_END: address of the byte location which should become the new "actual 
	frontier" and thus before which any actual content between itself and the 
	current actual frontier should be virtualized
pt_src: Input & Output. Address of the information structure describing the 
	rich-text field holding the source content. The actual-frontier member will 
	be updated to reflect the work of this procedure. If a target rich-text 
	context is not specified, procedure will write the content to this source 
	context's virtuality.
pt_trgt: Optional Output. Address of the rich-text context whose virtuality 
	component will receive the rich-text content being copied. If null, 
	procedure will write the content to the source context's virtuality.
pt_crsr: Optional Output. Address of the rich-text cursor to set to the first 
	CD record virtualized by the call. If null, output will be suppressed.
RETURN: eus_SUCCESS if successful; the Notes API error code otherwise

--- revision history -------
3/14/03 PR
+ signature change and logic adjustment to add optional output of cursor at 
  start of virtuality written by the call
+ listing format adjustment, token renaming, documentation adjustment

3/20/00 PR
+ signature change to accommodate writing to virtuality not belonging to the 
  source rich-text context structure
+ logic shortening, some by leveraging the new UnlockItem() procedure
+ documentation adjustment

12/8/98 PR: created			*/
static STATUS us_VirtualizeThruActualLocation( 
											const BYTE *const  puc_END, 
											RtfTrackingInfo *const  pt_src, 
											RtfTrackingInfo * pt_trgt, 
											CdRecordCursor *const  pt_crsr)	{
	ItemInfo * pt_itm, * pt_item;
	const BYTE * puc_appndg;
	DWORD  ul_lenToCopy, ul = NULL, ul_lenTotalActual = NULL;
	BOOL  f_itmJustLckd = FALSE;
	STATUS  us_err;

	_ASSERTE( puc_END && pt_src && pt_src->pt_Actuality && 
							pt_src->ul_ActualFrontier != mul_FRONTIER_NO_MORE);

	if (pt_crsr)
		memset( pt_crsr, NULL, sizeof( CdRecordCursor));

	//if no target rich-text context specified, make the target the source 
	//	context itself	
	if (!pt_trgt)
		pt_trgt = pt_src;

	//determine working coördinates of the current "actual frontier" point, as 
	//	well as the absolute offset associated with the end of the item 
	//	containing the actual-frontier point
	if (!f_LocateAbsoluteOffset( pt_src->ul_ActualFrontier, 
											pt_src->pt_Actuality, &pt_itm, 
											&puc_appndg, &ul, &f_itmJustLckd))
		return !eus_SUCCESS;

	//if the actual frontier appears in the same item as the specified byte 
	//	location to become the new frontier...
	if (pt_itm->puc_start < puc_END && puc_END <= pt_itm->puc_start + 
														pt_itm->ul_length)	{
		//if the specified byte location does not follow the actual-frontier 
		//	location, return general failure
		if (puc_END < puc_appndg)
			return !eus_SUCCESS;
	//else make sure that the specified byte location follows the 
	//	actual-frontier location by testing the rich-text items following the 
	//	actual frontier's item until an item is found that contains the 
	//	specified location to become the new frontier
	}else	{
		pt_item = pt_itm->pt_next;

		do
			if (pt_item->puc_start < puc_END && puc_END <= pt_item->puc_start + 
															pt_item->ul_length)
				break;
		while( pt_item = pt_item->pt_next);

		//if no containing item was found, return general failure
		if (!pt_item)
			return !eus_SUCCESS;
	} //if (pt_itm->puc_start <= puc_END &&

	//to help make virtualization more efficient downstream, compile the total 
	//	length of the actual rich-text field
	pt_item = pt_src->pt_Actuality;
	while (pt_item)	{
		ul_lenTotalActual += ul_lenTotalActual % 2 + pt_item->ul_length;
		pt_item = pt_item->pt_next;
	}

	//if the current actual frontier and the byte location that is to become 
	//	the new "actual frontier" fall on different items, virtualize any 
	//	content falling between the actual frontier and the beginning of the 
	//	item containing the byte location of the new frontier...
	while (puc_END < pt_itm->puc_start || puc_END > pt_itm->puc_start + 
														pt_itm->ul_length)	{
		//determine the length of content to virtualize from the current item
		ul_lenToCopy = ul - pt_src->ul_ActualFrontier;

		//virtualize the length of content after fist advancing the actual 
		//	frontier to the beginning of content in the next rich-text item
		pt_src->ul_ActualFrontier += ul_lenToCopy + ul_lenToCopy % 2 + 
																sizeof( WORD);
		us_err = us_VirtualAppend( puc_appndg, ul_lenToCopy, pt_trgt, 
											ul_lenTotalActual, pt_crsr && 
											!pt_crsr->pt_item ? pt_crsr : NULL);

		//if the item containing the content just virtualized was locked in 
		//	memory for the sake of this operation, unlock it now
		if (f_itmJustLckd)	{
			UnlockItem( pt_itm);
			f_itmJustLckd = FALSE;
		}

		//if any error occurred during the virtualization, return its code to 
		//	the caller
		if (us_err)
			return us_err;

		//move to the next item on our way toward the item containing the end, 
		//	specified byte location
		pt_itm = pt_itm->pt_next;
		if (!pt_itm->puc_start)	{
			pt_itm->puc_start = OSLockBlock( BYTE, pt_itm->bid_contents);
			f_itmJustLckd = TRUE;
		}

		//update the variable telling the absolute length up through the 
		//	current item, and set the pointer telling where virtualization 
		//	should start on our new item
		ul += ul % 2 + pt_itm->ul_length;
		puc_appndg = pt_itm->puc_start + sizeof( WORD);
	} //puc_END < pt_itm->puc_start ||

	//Get the length of whatever content is left over in the current actual 
	//	item up to the point that will mark the new actual frontier. If 
	//	there is no remaining content, return success.
	if ((ul_lenToCopy = puc_END - puc_appndg) <= 0)
		return eus_SUCCESS;

	//virtualize the last piece of content before the new actual frontier
	pt_src->ul_ActualFrontier += ul_lenToCopy + ul_lenToCopy % 2;
	return us_VirtualAppend( puc_appndg, ul_lenToCopy, pt_trgt, 
											ul_lenTotalActual, pt_crsr && 
											!pt_crsr->pt_item ? pt_crsr : NULL);
} //us_VirtualizeThruActualLocation(


/** us_VirtualAppend( ***
Appends the specified length of CD-record content to the end of any current 
virtual content, accounting as necessary for the creation of rich-text items to 
accommodate the content being appended.

NOTE: Only the last virtual rich-text item written to will remain with its 
content locked in memory. However, if the caller seeks to maintain a note of 
the location where the current virutalization cycle (possibly consisting of a 
succession of calls to this procedure) began virtualizing, the content of the 
rich-text item encompasing that location will not be unlocked.

The function uses two threshold values on the size of virtual items it creates: 
"comfortable" and "stretch." The comfortable limit specifies a standard size 
that leaves ample room for ad-hoc growth if needed and that accommodates the 
fact that Notes handles rich-text items better when they're not maxed out at 
64K. The stretch limit is used principally on what will likely be the last 
virtual item, allowing the length of that item to be somewhat larger so that 
another, likely small, item doesn't have to be created (an efficiency measure, 
in other words). When this procedure is used against "proxy" virtuality, the 
stretch limit can regularly come into play because the proxy virtuality usually 
consists of enlarging particular virtual items that are about to be replaced.

--- parameters & return ----
puc_appndg: address of the beginning of the content to be appended to any 
	current virtual content
ul_lenAppnd: the byte-length of the content to be appended
pt_ctx: Input & Output. Pointer to environment information about the 
	rich-text field being operated on. Its pointer to virtuality may be 
	initialized by this function.
ul_LEN_TOTAL_ACTUAL: Optional. The aggregate absolute length of all the CD 
	records comprising the rich-text field as it actually exists now on disk, 
	no changes having been committed. Used only in conjunction with stretching 
	the length of what seems to be the last virtual item in order that another, 
	possibly small, virtual item does not have to be created. If passed in 
	null, this "stretching" functionality will be ignored.
pt_crsr: Optional Output. Address of the already nullified rich-text cursor to 
	set to the first CD record virtualized by the call. If not null, no 
	adjustment to the cursor will occur, and if the procedure is otherwise 
	working with the rich-text item that encompasses the cursor's location, the 
	procedure will not unlock that item's content in memory.
RETURN: eus_SUCCESS if no error occured; the Notes API error code otherwise

--- side effects ------------
One somewhat risky behavior of this function is that if the virtual item we are 
initially appending to is filled, its memory will be unlocked by this procedure 
after it creates a follow-on virtual item. If any active pointers outside this 
procedure point to this virtual item, they will be invalidated with no way to 
know it. If the suggested enhancement immediately below is implemented, this 
issue should pose no more risk.

--- suggested enhancement ---
11/26/98 PR
Perhaps some reference-count locking mechanism should be incorporated into the 
ItemInfo structure so that items can be locked and "unlocked" freely with no 
risk of invalidating unknown pointers relying on formerly locked item 
information.

--- revision history --------
3/14/03 PR
+ signature change to add optional output of cursor at start of virtuality 
  written by the call
+ listing format adjustment, token renaming, documentation adjustment

3/20/00 PR
+ accommodated signature change to us_StartVirtualItemList()
+ logic shortening by leveraging the new UnlockItem() procedure

9/12/99 PR: documentation adjustment
12/8/98 PR: created			*/
static STATUS us_VirtualAppend( const BYTE * puc_appndg, 
								DWORD  ul_lenAppnd, 
								RtfTrackingInfo *const  pt_ctx, 
								const DWORD  ul_LEN_TOTAL_ACTUAL, 
								CdRecordCursor *const  pt_crsr)	{
	const DWORD  ul_FRONTIER_REMAINING = ul_LEN_TOTAL_ACTUAL && 
								pt_ctx->ul_ActualFrontier != 
								mul_FRONTIER_NO_MORE ? ul_LEN_TOTAL_ACTUAL - 
								pt_ctx->ul_ActualFrontier : 0;

	DWORD  ul_lmtCurr;
	ItemInfo * pt_itm;
	BYTE * puc;
	STATUS  us_err;

	_ASSERTE( puc_appndg && pt_ctx && (pt_crsr ? !pt_crsr->pt_item : TRUE));

	//if the caller passed a null length, short-circuit with success
	if (!ul_lenAppnd)
		return eus_SUCCESS;

	//if virtuality hasn't been started yet, start it now
	if (!pt_ctx->pt_Virtuality)
		if (us_err = us_StartVirtualItemList( NULL, pt_ctx))
			return us_err;

	//Set the initial length limit we will use against the size of the current 
	//	virtual item. If we're already over the "comfortable" limit, default to 
	//	the "stretch" limit size.
	pt_itm = pt_ctx->pt_endVirtual;
	if (!pt_itm->puc_start)
		pt_itm->puc_start = OSLockBlock( BYTE, pt_itm->bid_contents);
	ul_lmtCurr = pt_itm && pt_itm->ul_length > mul_LMT_COMFY ? 
											mul_LMT_STRETCH : mul_LMT_COMFY;

	//if adding the specified length of content exceeds the limit we have on 
	//	the length of the current virtual item, run a loop that will fill up as 
	//	many virtual items as necessary until the remaining length will fit 
	//	within an item with space left over for later virtualizing
	while (pt_itm->ul_length + pt_itm->ul_length % 2 + ul_lenAppnd > 
																ul_lmtCurr)	{
		//if we're using the "comfortable" limit and it seems that the current 
		//	virtual item will be the last and the length to be virtualized fits 
		//	within the "stretch" limit, switch to using the stretch limit as 
		//	the size threshold and break out of this "fill full items" loop
		if (ul_lmtCurr == mul_LMT_COMFY && ul_LEN_TOTAL_ACTUAL && 
								pt_itm->ul_length + pt_itm->ul_length % 2 + 
								ul_lenAppnd + ul_lenAppnd % 2 + 
								ul_FRONTIER_REMAINING <= mul_LMT_STRETCH)	{
			ul_lmtCurr = mul_LMT_STRETCH;
			break;
		//if we are dealing with special insertions into already written 
		//	virtuality via a proxy virtuality, use the stretch limit to squash 
		//	any memcpying domino effect by trying to fit a few bytes into an 
		//	already full space
		}else if (!pt_ctx->ul_ActualFrontier)	{
			ul_lmtCurr = mul_LMT_STRETCH;

			//if the switch to a stretch limit now accommodates the length to 
			//	be virtualized within the curent item, break out of this "fill 
			//	full items" loop
			if (pt_itm->ul_length + pt_itm->ul_length % 2 + ul_lenAppnd <= 
																	ul_lmtCurr)
				break;
		} //if (ul_lmtCurr == mul_LMT_COMFY &&

		//Fill up the current virtual item with as much of the content to be 
		//	virtualized as our limit will allow, get ready with the next 
		//	virtual item to fill, and unlock the memory associated with the 
		//	virtual item just filled. In the case of the last step, we are 
		//	assuming that there are no other active pointers into the item 
		//	filled. This is only risky for the virtual item initially passed 
		//	into this procedure, since we create any follow-on items ourselves 
		//	in this loop and so know all there is to know about those items.
		if (us_err = us_FillVirtualItemAndStartNext( ul_lmtCurr, 
										&pt_ctx->pt_endVirtual, &puc_appndg, 
										&ul_lenAppnd, pt_crsr && 
										!pt_crsr->pt_item ? pt_crsr : NULL))
			return us_err;
		if (!( pt_crsr && pt_crsr->pt_item == pt_itm))
			UnlockItem( pt_itm);

		pt_itm = pt_ctx->pt_endVirtual;
	} //while (pt_ctx->pt_endVirtual->ul_length +

	//copy whatever content wasn't processed above into the latest current 
	//	virtual item, and if requested and not yet set, set the cursor pointing to the first 
	//	virtualized CD record
	memcpy( puc = pt_itm->puc_start + pt_itm->ul_length + pt_itm->ul_length % 
													2, puc_appndg, ul_lenAppnd);
	pt_itm->ul_length += pt_itm->ul_length % 2 + ul_lenAppnd;
	if (pt_crsr && !pt_crsr->pt_item)	{
		pt_crsr->puc_location = puc;
		pt_crsr->us_recLength = us_getCdRecLength( puc);
		pt_crsr->pt_item = pt_itm;
	}

	return eus_SUCCESS;
} //us_VirtualAppend(


/** us_StartVirtualItemList( ***
Purpose is to lay an infrastructure to support the changing of the current 
field's rich-text content in a normalized, flexible way. This infrastructure 
is a set of "virtual" rich-text items similar to the actual items currently 
in use by the note, the only difference being that the actual items have 
been committed (saved) to the note, and therefore owned by the note, whereas 
the virtual items are owned by our process until we decide to commit the 
virtual items and so replace the current actual items.

--- parameter & return ----
ul_LEN_PARTICULAR: Optional. The content length the caller has requested to 
	be allocated to the first virtual item. The procedure will allocate a 
	shorter length if the requested length exceeds the maximum allowed 
	(mul_MAXLEN_RICH_TEXT_ITEM). If null, the item will be set to a default 
	length.
pt_context: Output. Pointer to a structure of item-tracking information about 
	the rich-text field being operated on. Members of this structure related 
	to virtuality will be adjusted by this function.
RETURN: eus_SUCCESS if no error occured; the Notes API error code otherwise

--- revision history ------
3/20/00 PR: extended to allow creation of specific-length items
11/25/98 PR: created		*/
static __inline STATUS us_StartVirtualItemList( 
										const DWORD  ul_LEN_PARTICULAR, 
										RtfTrackingInfo *const  pt_context)	{
	STATUS  us_err;

	_ASSERT( pt_context && !pt_context->pt_Virtuality);

	//create the first virtual item associated with this rich-text field and 
	//	set the current-virtual member of the item-tracking structure being 
	//	used by the rich-text field
	if (us_err = us_CreateVirtualItem( mi_REGULAR_VIRTUAL, 
												ul_LEN_PARTICULAR, 
												&pt_context->pt_Virtuality))
		return us_err;
	pt_context->pt_endVirtual = pt_context->pt_Virtuality;

	return eus_SUCCESS;
} //us_StartVirtualItemList(


/** us_CreateVirtualItem( ***
Purpose is to create a structure for tracking and describing a virtual 
rich-text item.

--- parameters & return ----
i_TYPE: enumerated value specifying the type of virtual item to create: 
	mi_REGULAR_VIRTUAL or mi_SEMI_VIRTUAL
ul_LEN_PARTICULAR: Optional. The content length the caller has requested to 
	be allocated to the item. The procedure will allocate the requested 
	length if the length complys with the constraints associated with the 
	item type specified. If null, the item will be set to a default length.
ppt_item: Output. Memory address of the pointer variable to set to the 
	item structure created.
RETURN: eus_SUCCESS if no error occured; the Notes API error code otherwise

--- revision history -------
3/20/00 PR: extended to allow creation of a specific-length item 
	irrespective of item type
11/28/98 PR: created		*/
static STATUS us_CreateVirtualItem( const VirtualType  i_TYPE, 
									const DWORD  ul_LEN_PARTICULAR, 
									ItemInfo * *const  ppt_item)	{
	const DWORD  ul_SEMI_VIRTUAL_LIMIT = 0x0400 * 5;

	ItemInfo * pt_item;
	STATUS  us_err;

	_ASSERTE( ppt_item && i_TYPE);

	//allocate memory to hold the structure that will track and describe the 
	//	item we're creating, and initialize it's linked-list "next" member to 
	//	null
	if (!( pt_item = malloc( sizeof( ItemInfo))))
		return !eus_SUCCESS;
	pt_item->pt_next = NULL;

	//Allocate memory to hold the content we will be putting into the item. 
	//	If a particular allocation length is requested, comply as long as 
	//	the request is less than the maximum length allowed if a regular 
	//	vitual item (i.e. one appendable to a note) or greater than the 
	//	minimum allowed if the item is simply to hold a stream of CD records. 
	//	In the latter case no limits are set because such items are not 
	//	committed as such to a note; their content must be fully virtualized 
	//	first. We want however to ensure that such items have a minimum 
	//	length so that we won't have the overhead of creating lots of little 
	//	ones linked together.
	if (us_err = OSMemAlloc( NULL, i_TYPE == mi_REGULAR_VIRTUAL ? 
							ul_LEN_PARTICULAR && ul_LEN_PARTICULAR < 
							mul_MAXLEN_RICH_TEXT_ITEM ? ul_LEN_PARTICULAR : 
							mul_MAXLEN_RICH_TEXT_ITEM : ul_LEN_PARTICULAR && 
							ul_LEN_PARTICULAR > ul_SEMI_VIRTUAL_LIMIT ? 
							ul_LEN_PARTICULAR : ul_SEMI_VIRTUAL_LIMIT, 
							&pt_item->bid_contents.pool))	{
		free( pt_item);
		*ppt_item = NULL;
		return us_err;
	}

	//nullify the item block so Notes won't think it owns the block (although 
	//	this is a bit strange since sometimes Notes returns a null block 
	//	member for blocks it owns -- so far I've seen this only on the 
	//	rich-text Body field during the mail-send event), set the item member 
	//	that indicates that the item is virtual and prepare the item so that 
	//	it can be written to
	pt_item->bid_contents.block = NULLBLOCK;
	pt_item->i_type = mi_VIRTUAL;
	pt_item->puc_start = OSLockBlock( BYTE, pt_item->bid_contents);

	//initialize item members according to item type
	if (i_TYPE == mi_REGULAR_VIRTUAL)	{
		*(WORD *) pt_item->puc_start = TYPE_COMPOSITE;
		pt_item->ul_length = sizeof( WORD);
	}else
		pt_item->ul_length = NULL;

	*ppt_item = pt_item;

	return eus_SUCCESS;
} //us_CreateVirtualItem(


/** us_FillVirtualItemAndStartNext( ***
Purpose is to fill the virtual rich-text item currently being populated with 
the specified stream of CD-record content.

--- parameters & return ----
ul_LMT_CURR: the maximum allowed size that the current virtual item may become
ppt_itm: Input & Output. Address of the pointer to the item to which the 
	specified content should be appended, as much as possible. If a follow-on 
	virtual item is created because the appendage fills up the item initially 
	pointed to by this parameter, the pointer will be moved to the beginning of 
	the follow-on item just before the procedure ends. Thus the pointer is 
	always "at-the-ready" for another round of virtualization.
ppuc_appndg: Input & Output. Address of the pointer positioned to the content 
	to be virtualized in this procedure. The pointer will be advanced by this 
	procedure by the number of bytes actually virtualized. So if the content to 
	be appended cannot all fit within the specified virtual item, another call 
	to this procedure will pick up where the last call left off.
pul_lenAppnd: Input & Output. Address of the variable in which the length of 
	the content to be virtualized is stored. The variable will be decremented 
	by the amount of content actually virtualized by this procedure. So if the 
	content to be appended cannot all fit within the specified virtual item, 
	the caller already has the length it should use in the next call to this 
	procedure so it may pick up where it left off.
pt_crsr: Optional Output. Address of the already nullified rich-text cursor to 
	set to the first CD record virtualized by the call. If null, output will be 
	suppressed.
RETURN: eus_SUCCESS if no error occured; the Notes API error code otherwise

--- revision history -------
3/14/03 PR
+ signature change to add optional output of cursor at start of virtuality 
  written by the call
+ listing format adjustment, token renaming, documentation adjustment

3/20/00 PR: logic shortened slightly
11/28/98 PR: created		*/
static STATUS us_FillVirtualItemAndStartNext( 
											const DWORD  ul_LMT_CURR, 
											ItemInfo * *const  ppt_itm, 
											const  BYTE * *const  ppuc_appndg, 
											DWORD *const  pul_lenAppnd, 
											CdRecordCursor *const  pt_crsr)	{
	ItemInfo * pt_itm = ppt_itm ? *ppt_itm : NULL;
	DWORD  ul_lenNxtRec, ul_lenToCopy = NULL;
	BYTE * puc;
	STATUS  us_err;

	//determine the space available in the virtual item to be appended to
	const DWORD  ul_SPACE_AVAILABLE = pt_itm ? ul_LMT_CURR - 
												pt_itm->ul_length + 
												pt_itm->ul_length % 2 : NULL;

	_ASSERTE( ul_LMT_CURR && pt_itm && ppuc_appndg && pul_lenAppnd && 
										(pt_crsr ? !pt_crsr->pt_item : TRUE));

	//measure out the length of the content that can fit within the space 
	//	available by looping over the succession of CD records that make up the 
	//	content
	ul_lenNxtRec = us_getCdRecLength( *ppuc_appndg);
	while (ul_lenToCopy + ul_lenToCopy % 2 + ul_lenNxtRec < 
														ul_SPACE_AVAILABLE)	{
		ul_lenToCopy += ul_lenToCopy % 2 + ul_lenNxtRec;
		ul_lenNxtRec = us_getCdRecLength( *ppuc_appndg + ul_lenToCopy + 
															ul_lenToCopy % 2);
	}

	//if the virtual item doesn't have any content in it yet and the first CD 
	//	record is so large that it can't fit within the specified limit...
	if (ul_SPACE_AVAILABLE == ul_LMT_CURR && !ul_lenToCopy)	{
		//if the record is larger than the maximum size that a rich-text item 
		//	can hold, return general failure
		if (ul_lenNxtRec >= mul_MAXLEN_RICH_TEXT_ITEM - sizeof( WORD))
			return !eus_SUCCESS;

		//go ahead and allow the item length to be stretched to accommodate the 
		//	very large record
		ul_lenToCopy = ul_lenNxtRec;
	}

	//copy into the virtual item the content that can fit and adjust the item's 
	//	length accordingly, and if requested, set the cursor pointing to the 
	//	first virtualized CD record
	memcpy( puc = pt_itm->puc_start + pt_itm->ul_length + pt_itm->ul_length % 
												2, *ppuc_appndg, ul_lenToCopy);
	pt_itm->ul_length += pt_itm->ul_length % 2 + ul_lenToCopy;
	if (pt_crsr)	{
		pt_crsr->puc_location = puc;
		pt_crsr->us_recLength = us_getCdRecLength( puc);
		pt_crsr->pt_item = pt_itm;
	}

	//create and move to a follow-on virtual item so we're all ready to append 
	//	the next time this function is called
	if (us_err = us_InsertNextVirtualItem( pt_itm, mi_REGULAR_VIRTUAL, NULL))
		return us_err;
	*ppt_itm = pt_itm->pt_next;

	//Advance the content pointer by the amout of content we virtualized, and 
	//	decrease the byte length of the content to be appended by a 
	//	corresponding amount. These outputs allow the caller to continuously 
	//	call this procedure until an entire length of content is virtualized.
	*ppuc_appndg += ul_lenToCopy + ul_lenToCopy % 2;
	*pul_lenAppnd -= ul_lenToCopy + ul_lenToCopy % 2;

	return eus_SUCCESS;
} //us_FillVirtualItemAndStartNext(


/** us_InsertNextVirtualItem( ***
Append a new rich-text item to the list of virtual items currently in play.

--- parameters & return ----
pt_itm: Input & Output. Address of information about the virtual rich-text item 
	that precedes the virtual item to be created. The "pt_next" member of the 
	specified item will be reset to reflect the insertion.
i_TYP: enumerated value specifying the type of virtual item to create: 
	mi_REGULAR_VIRTUAL or mi_SEMI_VIRTUAL
ul_LEN: Optional. The content length the caller has requested to be allocated 
	to the item. The procedure will allocate the requested length if the length 
	complies with the constraints associated with the item type specified. If 
	null, the item will be set to a default length.
RETURN: eus_SUCCESS if no error occured; the Notes API error code otherwise

--- revision history -------
3/14/03 PR: listing format adjustment, token renaming, minor documentation 
	adjustment
3/20/00 PR: documentation & token-name adjustment associated with enhancement 
	to allow creation of a specific-length item irrespective of item type
11/28/98 PR: created		*/
static __inline STATUS us_InsertNextVirtualItem( ItemInfo *const  pt_itm, 
													const VirtualType  i_TYP, 
													const DWORD  ul_LEN)	{
	ItemInfo * pt_itmNew;
	STATUS  us_err;

	_ASSERTE( pt_itm && i_TYP);

	//create a virtual item
	if (us_err = us_CreateVirtualItem( i_TYP, ul_LEN, &pt_itmNew))
		return us_err;

	//put the new item in place after the passed-in item
	pt_itmNew->pt_next = pt_itm->pt_next;
	pt_itm->pt_next = pt_itmNew;

	return eus_SUCCESS;
} //us_InsertNextVirtualItem(


/** eul_GetRtfSpanText( ***
Purpose is to copy a given span of rich-text textual content into a buffer, 
translating rich-text linefeed and paragraph markers to CRLFs if required. 
A text span is delimited by a beginning cursor and either an end cursor or a 
maximum buffer length. If both an end cursor and maximum buffer length are 
provided, the first of the two to be encountered/fulfilled relative to the 
beginning cursor (in terms of the succession of rich-text) will be recognized 
as the span's endpoint.

This function allocates the buffer into which the content is copied, the 
caller is responsible for freeing the memory pointer to the buffer!!

--- parameters & return ----
pt_SPAN: Pointer to a structure describing the span of rich-text whose 
	textual content should be copied The end cursor member's location pointer 
	may be set to null if a maximum content length is specified.
ul_MAXLEN_BUFFER: Optional. 
			provided that an end cursor has been specified in 
			the span-structure parameter. 
	The maximum-desired length of the buffer to 
	be created to contain the copied content. A null value indicates that no 
	maximum limit should be enforced.
f_TRANSLATE_LINEFEEDS: flag specifying whether rich-text linefeed and 
	paragraph markers should be translated to CRLFs when copying content 
	(TRUE) or not (FALSE)
pt_CONTEXT: Optional. Pointer to state information about the rich-text field. 
	If passed in null, no transfer between virtuality and actuality can occur.
ppc: Output. Memory address of the pointer to set to the buffer that this 
	function will create in which to store the rich-text content to be 
	copied. The caller is responsible for freeing the memory associated with 
	this buffer.
RETURN: The length of the buffer allocated by this function to contain the 
	copied rich-text content. eul_ERR_FAILURE if an invalid parameter 
	configuration was passed.

--- side effect -------------
The rich-text environment information pointed to through pt_SPAN or 
pt_CONTEXT may be adjusted due to cursor advancement. If the enhancement 
suggested below is implemented, this side effect would be eliminated.

--- suggested enhancement ---
11/10/98 PR
The starting memory-lock state should be restored upon function exit, but no 
good mechanism is yet available with which to accomplish this. See this 
module's suggested-enhancement note for a description of the problem and 
proposed solution.

--- revision history --------
3/20/00 PR: minor logic shortening
11/23/98 PR: created		*/
DWORD eul_GetRtfSpanText( const CdRecordSpan *const  pt_SPAN, 
							const DWORD  ul_MAXLEN_BUFFER, 
							const BOOL  f_TRANSLATE_LINEFEEDS, 
							const RtfTrackingInfo *const  pt_CONTEXT, 
							char * *const  ppc)	{
	//Determine the maximum-desired content (not buffer) length, considering 
	//	that if rich-text linefeed and paragraph markers are to be translated 
	//	with CRLFs, a non-content terminating null character will be 
	//	necessary. Also determine the content length needed to accommodate 
	//	the specified text span. If a length cannot be determined, an invalid  
	//	rich-text span must have been specified.
	const DWORD  ul_MAXLEN_CONTENT = ul_MAXLEN_BUFFER - 
													(f_TRANSLATE_LINEFEEDS && 
													ul_MAXLEN_BUFFER ? 1 : 0);
	const DWORD  ul_LEN_CONTENT = ul_TextSpanLength( pt_SPAN, 
										ul_MAXLEN_CONTENT, 
										f_TRANSLATE_LINEFEEDS, pt_CONTEXT);

	WORD  us;
	DWORD  ul_lenAggregate = NULL;
	CdRecordCursor  t_cursor;

	//if invalid parameters were passed, short-circuit with general failure
	if (!ppc || ul_LEN_CONTENT == eul_ERR_FAILURE)
		return eul_ERR_FAILURE;

	//Allocate a buffer to the size of the ultimate buffer length, taking 
	//	into account that if rich-text linefeed and paragraph markers are to 
	//	be translated to CRLFs, an extra byte is needed to null-terminate the 
	//	string. Next, if CRLF-translation is needed, set the last byte of the 
	//	allocated buffer to null.
	if (!( *ppc = malloc( ul_LEN_CONTENT + (f_TRANSLATE_LINEFEEDS ? 1 : 0))))
		return eul_ERR_FAILURE;
	if (f_TRANSLATE_LINEFEEDS)
		(*ppc)[ ul_LEN_CONTENT] = NULL;

	//if there's no length to the text span, short-circuit with a return of 
	//	the ultimate content length plus one for null-termination if rich-
	//	text linefeed and paragraph markers were to be translated to CRLFs
	if (!ul_LEN_CONTENT)
		return f_TRANSLATE_LINEFEEDS ? 1 : 0;

	//copy the rich-text span's textual content into the buffer just allocated
	if (!f_ProcessKnownLengthRtfText( pt_SPAN->t_crsrBgin, 
									pt_SPAN->us_offstBgin, ul_LEN_CONTENT, 
									f_TRANSLATE_LINEFEEDS, pt_CONTEXT, 
									&t_cursor, &us, *ppc))
		goto errJump;

	//If applicable, verify that the ending coordinates of the text copying 
	//	match the end coordinates specified. If they don't, return failure.
	if (pt_SPAN->t_crsrEnd.puc_location && 
									!(pt_SPAN->t_crsrEnd.puc_location == 
									t_cursor.puc_location && 
									pt_SPAN->us_offstEnd == us))
		goto errJump;

	//return the ultimate text length, incremented by one if rich-text 
	//	linefeed and paragraph markers were translated to CRLFs
	return ul_LEN_CONTENT + (f_TRANSLATE_LINEFEEDS ? 1 : 0);

errJump:
	free( *ppc);
	*ppc = NULL;

	return eul_ERR_FAILURE;
} //eul_GetRtfSpanText(


/** f_ProcessKnownLengthRtfText( ***
Purpose is to serve as a means by which to conduct basic operations vis-à-vis a 
known length of rich-text textual content. Two basic operations are supported: 
provide the coordinates of the end of the specified length in terms of the 
rich-text constructs of a cursor and offset or copy the textual content into a 
memory buffer. Both operations allow rich-text linefeed and paragraph markers 
to be treated as CRLFs.

Often the "known length" is derived from a prior call to ul_TextSpanLength().

--- parameters & return ----
t_CURSOR: rich-text cursor pointing to the first CD record at which processing 
	should commence on the rich-text field
us_OFFSET: Number of bytes from content-start in the first CD record after 
	which processing should commence. Only meaningful if the first CD record is 
	a CDTEXT.
ul_LEN_TEXT: the ultimate length of the textual content to be processed, often 
	obtained via a prior call to ul_TextSpanLength()
f_TRANSLATE_LINEFEEDS: flag specifying whether rich-text linefeed and paragraph 
	markers should be translated as CRLFs when processing rich-text textual 
	content (TRUE) or not translated (FALSE)
pt_CONTEXT: Optional. Address of state information about the rich-text field. 
	If passed in null, no transfer between virtuality and actuality can occur.
pt_crsrEnd: Optional Output. Pointer to the variable in which to store the 
	cursor pointing to the CD record containing the content that marks the 
	end of the specified length of rich text. If passed in null, this 
	functionality will be ignored.
pus_offstEnd: Optional Output. Pointer to the variable in which to store the 
	number of content bytes processed in the final CD record. Guaranteed to be 
	null if the final CD record is a CDPARAGRAPH. If passed in null, this 
	functionality will be ignored.
pc: Optional Output. Pointer to the buffer in which to store an unformatted 
	copy of the rich-text content being traversed. If passed in null, the 
	copying functionality will be ignored.
RETURN: TRUE if no errors occurred; FALSE otherwise

--- side effect -------------
The rich-text environment information pointed to through t_CURSOR or pt_CONTEXT 
may be adjusted due to cursor advancement. If the enhancement suggested below 
is implemented, this side effect would be eliminated.

--- suggested enhancement ---
11/10/98 PR: The starting memory-lock state should be restored upon function 
	exit, but no good mechanism is yet available with which to accomplish this. 
	See this module's suggested-enhancement note for a description of the 
	problem and proposed solution.

--- revision history -------
7/21/03 PR: participated in global token rename
9/6/02 PR: listing format adjustment and documentation adjustment, improvement
3/20/00 PR: token renaming
9/12/99 PR: minor logic shortening, documentation adjustment
11/23/98 PR: created		*/
static BOOL f_ProcessKnownLengthRtfText( 
									const CdRecordCursor  t_CURSOR, 
									const WORD  us_OFFSET, 
									const DWORD  ul_LEN_TEXT, 
									const BOOL  f_TRANSLATE_LINEFEEDS, 
									const RtfTrackingInfo *const  pt_CONTEXT, 
									CdRecordCursor *const  pt_crsrEnd, 
									WORD *const  pus_offstEnd, 
									char *const  pc)	{
	BYTE  uc;
	const BYTE * puc;
	WORD  us;
	DWORD  ul_length = NULL;
	int  i;
	BOOL  f_MaximumReached = NULL;
	CdRecordCursor  t_cursor;

	_ASSERTE( t_CURSOR.pt_item && t_CURSOR.puc_location && ul_LEN_TEXT);

	//default the outputs to null values
	if (pt_crsrEnd)
		memset( pt_crsrEnd, NULL, sizeof( CdRecordCursor));
	if (pus_offstEnd)
		*pus_offstEnd = NULL;
	if (pc)
		*pc = NULL;

	//if the starting cursor is on a CDTEXT or CDPARAGRAPH...
	puc = t_CURSOR.puc_location;
	uc = *puc;
	if (muc_SIG_CDTEXT == uc || muc_SIG_CDPARAGRAPH == uc)	{
		//if the starting cursor is on a CDTEXT
		if (muc_SIG_CDTEXT == uc)	{
			//determine the length of the record's available actual content, 
			//	taking into account any beginning offset
			us = t_CURSOR.us_recLength - sizeof( CDTEXT) - us_OFFSET;

			//set a pointer to the beginning of the record's available actual 
			//	content, taking into account any beginning offset
			puc += sizeof( CDTEXT) + us_OFFSET;
		//else the record is a CDPARAGRAPH, so indicate this by nullifying 
		//	the content pointer
		}else
			puc = NULL;

		//start off with this record's contribution to the rich-text span's 
		//	textual content
		i = i_ProcessRtfText( puc, us, ul_LEN_TEXT, f_TRANSLATE_LINEFEEDS, 
										&ul_length, pc, &f_MaximumReached);

		//if all of the content in the text span has been processed, return 
		//	success after setting the required outputs
		if (ul_length == ul_LEN_TEXT)	{
			if (pt_crsrEnd)
				*pt_crsrEnd = t_CURSOR;
			if (pus_offstEnd)
				*pus_offstEnd = us_OFFSET + i;
			return TRUE;
		}

		//if no more data can fit within the specified length, and since 
		//	we haven't matched the specified length exactly, the specified 
		//	length must be invalid, so return failure
		if (f_MaximumReached)
			return FALSE;
	} //if (muc_SIG_CDTEXT == us ||

	//loop forward record-by-record until the specified length of textual 
	//	content has been processed
	t_cursor = t_CURSOR;
	do	{
		//advance to the next CD record, if no further records exist, some 
		//	weird error occurred, so return failure
		AdvanceCdCursor( &t_cursor, pt_CONTEXT, NULL, NULL);
		if (!t_cursor.pt_item)
			return FALSE;

		//the current CD record is of interest only if it's a CDTEXT or 
		//	CDPARAGRAH
		uc = *t_cursor.puc_location;
		if (muc_SIG_CDTEXT == uc || muc_SIG_CDPARAGRAPH == uc)	{
			//if the record is a CDTEXT, determine the pointer to the 
			//	beginning of its available content and the content's length
			if (muc_SIG_CDTEXT == uc)	{
				puc = t_cursor.puc_location + sizeof( CDTEXT);
				us = t_cursor.us_recLength - sizeof( CDTEXT);
			//else the record is a CDPARAGRAPH, so indicate this by 
			//	nullifying the content pointer
			}else
				puc = NULL;

			//process this record's contribution to the specified length of 
			//	rich-text textual content
			i = i_ProcessRtfText( puc, us, ul_LEN_TEXT, 
										f_TRANSLATE_LINEFEEDS, 
										&ul_length, pc, &f_MaximumReached);
		} //if (muc_SIG_CDTEXT == uc ||
	} while (!f_MaximumReached);

	//if we haven't matched the specified length exactly, the specified 
	//	length must be invalid, so return failure
	if (ul_length != ul_LEN_TEXT)
		return FALSE;

	//set the required outputs
	if (pt_crsrEnd)
		*pt_crsrEnd = t_cursor;
	if (pus_offstEnd)
		*pus_offstEnd = i;

	return TRUE;
} //f_ProcessKnownLengthRtfText(


/** eul_GetRtfText( ***
Purpose is to return the textual content of the rich-text field starting from 
its beginning. If any of the rich-text field has been virtualized, "the 
beginning" is the beginning of virtuality.

--- parameters & return -----
pt_CONTEXT: pointer to state information about the rich-text field
ul_MAXLEN_BUFFER: Optional. The maximum-desired length of the buffer to be 
	created to contain the copied content. A null value indicates that no 
	maximum limit should be enforced.
f_TRANSLATE_LINEFEEDS: flag specifying whether rich-text linefeed and 
	paragraph markers should be translated to CRLFs when copying content 
	(TRUE) or not (FALSE)
ppc_output: Output. Pointer to the pointer to the buffer specifying the 
	buffer that this function will create in which to store the rich-text 
	content to be copied. The caller is responsible for freeing the memory 
	associated with this buffer.
RETURN: The length of the buffer allocated by this function to contain the 
	copied rich-text content. eul_ERR_FAILURE if an invalid parameter 
	configuration was passed.

::: TO DO ::::::::::::
Check out NSFItemConvertToText() to see how that handles CRLFs, tables, etc. 
Consider whether it may be useful here to enhance or replace some of its 
functionality.

--- side effect -------------
The rich-text environment information pointed to through pt_CONTEXT may be 
adjusted due to cursor advancement. If the enhancement suggested below is 
implemented, this side effect would be eliminated.

--- suggested enhancement ---
11/13/98 PR
The starting memory-lock state should be restored upon function exit, but no 
good mechanism is yet available with which to accomplish this. See this 
module's suggested-enhancement note for a description of the problem and 
proposed solution.

--- revision history --------
3/20/00 PR
+ logic shortening & rationalization
+ adjusted arbitrary maximum-length constant for a rich-text field to a 
  reasonable length

11/13/98 PR: created		*/
DWORD eul_GetRtfText( const RtfTrackingInfo *const  pt_CONTEXT, 
						const DWORD  ul_MAXLEN_BUFFER, 
						const BOOL  f_TRANSLATE_LINEFEEDS, 
						char * *const  ppc_output)	{
	CdRecordSpan  t_dmySpan;

	if (!( pt_CONTEXT && pt_CONTEXT->pt_Actuality && ppc_output))
		return eul_ERR_FAILURE;

	//nullify the dummy rich-text span structure
	memset( &t_dmySpan, NULL, sizeof( CdRecordSpan));

	//initialize the beginning cursor to the beginning of the current 
	//	rich-text field
	if (!f_ConstructStartCursor( pt_CONTEXT, NULL, &t_dmySpan.t_crsrBgin, 
																		NULL))
		return eul_ERR_FAILURE;

	//get the rich-text field's text, returning its length
	return eul_GetRtfSpanText( &t_dmySpan, ul_MAXLEN_BUFFER, 
										f_TRANSLATE_LINEFEEDS, pt_CONTEXT, 
										ppc_output);
} //eul_GetRtfText(


/** eus_CopyRtfIntoBuffer( ***
Copies the specified rich-text content into a memory buffer.

--- parameters & return ----
pt_CTXT: pointer to state information about the rich-text field whose content 
	is to be copied
ph: Output. Address of the variable in which to store the Notes handle to the 
	memory in which the copy will be stored. The un/lock state of the handle 
	is controlled by the optional ppuc paramenter, described below.
pul_len: Output. Address of the variable in which to store the length of the 
	content copied.
ppuc: Optional Output. Address to the pointer in which to store the beginning 
	of the memory buffer in which the rich-text field content will be stored. 
	If null pointer provided, this functionality is ignored and the memory 
	handle will be returned unlocked, else it will be returned locked.
RETURN: eus_SUCCESS if no error occured; the Notes API error code otherwise

--- revision history -------
3/20/00 PR
+ converted to Notes memory handling instead of malloc/free in order to 
  provide good support for holding onto the allocation for an extended 
  period of time (locking/unlocking, etc.)
+ logic shortening by leveraging the new UnlockItem() procedure
+ completed standard documentation

12/10/98 PR: created		*/
STATUS eus_CopyRtfIntoBuffer( const RtfTrackingInfo *const  pt_CTXT, 
								HANDLE *const  ph, 
								DWORD *const  pul_len, 
								BYTE * *const  ppuc)	{
	HANDLE  h;
	DWORD  ul_len, ul;
	BYTE * puc_start, * puc, * puc_frontier;
	ItemInfo * pt_item;
	BOOL  f_ItemGotLocked = FALSE;
	STATUS  us_err;

	if (!( pt_CTXT && (pt_CTXT->pt_Virtuality || pt_CTXT->pt_Actuality) && 
															ph && pul_len))
		return !eus_SUCCESS;

	*pul_len = (DWORD) *ph = NULL;
	if (ppuc)
		*ppuc = NULL;

	if (!( ul_len = ul_LogicalRtfLength( pt_CTXT)))
		return !eus_SUCCESS;
	_ASSERTE( ul_len > 1);

	if (us_err = OSMemAlloc( NULL, ul_len += sizeof( WORD), &h))
		return us_err;
	puc = puc_start = OSLockObject( h);

	*(WORD *) puc = TYPE_COMPOSITE;
	puc += sizeof( WORD);

	//if applicable, copy any virtaul content into the buffer
	if (pt_item = pt_CTXT->pt_Virtuality)	{
		do	{
			ul = pt_item->ul_length - sizeof( WORD);

			if (!pt_item->puc_start)	{
				pt_item->puc_start = OSLockBlock( BYTE, 
													pt_item->bid_contents);
				f_ItemGotLocked = TRUE;
			}
			memcpy( puc += (DWORD) puc % 2, pt_item->puc_start + 
														sizeof( WORD), ul);
			if (f_ItemGotLocked)	{
				OSUnlockBlock( pt_item->bid_contents);
				f_ItemGotLocked = FALSE;
			}

			puc += ul;
		} while (pt_item = pt_item->pt_next);
	} //if (pt_item = pt_CTXT->pt_Virtuality

	//if applicable, add on any content between the actual frontier and the 
	//	end of actuality
	if (pt_CTXT->pt_Actuality && pt_CTXT->ul_ActualFrontier != 
									mul_FRONTIER_NO_MORE && 
									f_LocateAbsoluteOffset( 
									pt_CTXT->ul_ActualFrontier, 
									pt_CTXT->pt_Actuality, &pt_item, 
									&puc_frontier, NULL, &f_ItemGotLocked))	{
		ul = pt_item->puc_start + pt_item->ul_length - puc_frontier;
		memcpy( puc += (DWORD) puc % 2, puc_frontier, ul);
		puc += ul;

		if (f_ItemGotLocked)
			UnlockItem( pt_item);

		while (pt_item = pt_item->pt_next)	{
			ul = pt_item->ul_length - sizeof( WORD);

			if (!pt_item->puc_start)	{
				pt_item->puc_start = OSLockBlock( BYTE, 
													pt_item->bid_contents);
				f_ItemGotLocked = TRUE;
			}
			memcpy( puc += (DWORD) puc % 2, pt_item->puc_start + 
														sizeof( WORD), ul);
			if (f_ItemGotLocked)	{
				OSUnlockBlock( pt_item->bid_contents);
				f_ItemGotLocked = FALSE;
			}

			puc += ul;
		} //while (pt_item = pt_item->pt_next)
	} //if (pt_CTXT->pt_Actuality &&
	
	_ASSERTE( puc == puc_start + ul_len);

	*ph = h;
	*pul_len = ul_len;
	if (ppuc)
		*ppuc = puc_start;
	else
		OSUnlockObject( h);

	return eus_SUCCESS;
} //eus_CopyRtfIntoBuffer(


/** ul_LogicalRtfLength( ***


--- parameter & return ----

RETURN: 

--- revision history ------
3/20/00 PR
+ logic shortening by leveraging the new UnlockItem() procedure
+ minor documentation adjustment

1/16/99 PR: created		*/
//DOC!!
static DWORD ul_LogicalRtfLength( const RtfTrackingInfo *const  pt_CONTEXT)	{
	ItemInfo * pt_item;
	DWORD  ul = NULL;
	BOOL  f_ItemGotLocked;
	BYTE * puc;

	_ASSERTE( pt_CONTEXT && (pt_CONTEXT->pt_Virtuality || 
												pt_CONTEXT->pt_Actuality));

	//if applicable, determine the total length of the content virtualized 
	//	so far
	if (pt_item = pt_CONTEXT->pt_Virtuality)
		do
			ul += pt_item->ul_length - sizeof( WORD);
		while (pt_item = pt_item->pt_next);

	//if applicable, add on the length of any content between the actual 
	//	frontier and the end of actuality
	if (pt_CONTEXT->pt_Actuality && pt_CONTEXT->ul_ActualFrontier != 
										mul_FRONTIER_NO_MORE && 
										f_LocateAbsoluteOffset( 
										pt_CONTEXT->ul_ActualFrontier, 
										pt_CONTEXT->pt_Actuality, &pt_item, 
										&puc, NULL, &f_ItemGotLocked))	{
		ul += ul % 2 + pt_item->puc_start + pt_item->ul_length - puc;
		if (f_ItemGotLocked)
			UnlockItem( pt_item);

		while (pt_item = pt_item->pt_next)
			ul += ul % 2 + pt_item->ul_length - sizeof( WORD);
	} //if (pt_CONTEXT->pt_Actuality &&

	return ul;
} //ul_LogicalRtfLength(


/** ul_TextSpanLength( ***
Purpose is to determine the size of the specified span of rich-text textual 
content, no matter if rich-text linefeed and paragraph markers should be 
translated to CRLFs. A text span is delimited by a beginning cursor and 
either an end cursor or a maximum content length. If both an end cursor and 
maximum content length are provided, the first of the two to be 
encountered/fulfilled relative to the beginning cursor (in terms of the 
succession of rich-text) will be recognized as the span's endpoint.

--- parameters & return ----
pt_SPAN: Pointer to the structure specifying the span of rich-text whose 
	textual content is to be sized. The end cursor member's location pointer 
	may be set to null if a maximum content length is specified.
ul_MAXLEN_CONTENT: Optional.
			provided that an end cursor has been specified in 
			the span-structure parameter. 
	The maximum-desired length of any content 
	to be sized. Acts as a short-circuit on the sizing process. A null value 
	indicates that no maximum constriant is to be applied.
f_TRANSLATE_LINEFEEDS: flag specifying whether rich-text linefeed and 
	paragraph markers should be sized as CRLFs when determining ultimate size
pt_CONTEXT: Optional. Pointer to state information about the rich-text field. 
	If passed in null, no transfer between virtuality and actuality can occur.
RETURN: If successful, the length of the the span's textual content. 
	eul_ERR_FAILURE if the parameters specify an invalid rich-text span.

--- revision history -------
7/21/03 PR: participated in global token rename

3/20/00 PR
+ extended to allow no end cursor or maximum length to be specified, in which 
  case the maximum length becomes the length of whatever's left in the 
  provided CD-record stream
+ safety improvements, token renaming

11/24/98 PR: created		*/
//DOC!!
static DWORD ul_TextSpanLength( const CdRecordSpan *const  pt_SPAN, 
								const DWORD  ul_MAXLEN_CONTENT, 
								const BOOL  f_TRANSLATE_LINEFEEDS, 
								const RtfTrackingInfo *const  pt_CONTEXT)	{
	const BYTE *const  puc_END_RECORD = pt_SPAN ? 
									pt_SPAN->t_crsrEnd.puc_location : NULL;

	const BYTE * puc = pt_SPAN ? pt_SPAN->t_crsrBgin.puc_location : NULL;
	BYTE uc = *puc;
	WORD  us;
	DWORD  ul_length = NULL;
	BOOL  f_MaximumReached;
	CdRecordCursor  t_cursor;

	//if any of the parameters are obviously invalid, short-circuit with 
	//	general failure
//A valid span requires either a maximum-desired 
//	buffer length (not content length) or a valid end cursor. NAH, we should 
// also allow length to be just what's remaining in the item stream, I think.
	_ASSERTE( puc && pt_SPAN->t_crsrBgin.pt_item && (puc_END_RECORD ? 
										(BOOL) pt_SPAN->t_crsrEnd.pt_item : 
										TRUE) /*|| ul_MAXLEN_CONTENT)*/ && 
										(pt_SPAN->t_crsrBgin.pt_item == 
										pt_SPAN->t_crsrEnd.pt_item ? 
										puc < puc_END_RECORD : TRUE));

	//if the starting cursor is on a CDTEXT or CDPARAGRAPH...
	if (muc_SIG_CDTEXT == uc || muc_SIG_CDPARAGRAPH == uc)	{
		//if the starting cursor is on a CDTEXT...
		if (muc_SIG_CDTEXT == uc)	{
			//if the beginning and ending cursors point to the same CD record 
			//	and the ending offset is less than the beginning offset, 
			//	short-circuit with a null content length
			if (puc == puc_END_RECORD && pt_SPAN->us_offstEnd < 
													pt_SPAN->us_offstBgin)
				return NULL;

			//determine the length of the record's available actual content, 
			//	taking into account any beginning offset and, if the starting 
			//	and ending cursor point to the same record, any ending offset
			if (puc != puc_END_RECORD)
				us = pt_SPAN->t_crsrBgin.us_recLength - sizeof( CDTEXT) - 
													pt_SPAN->us_offstBgin;
			else
				us = pt_SPAN->us_offstEnd - pt_SPAN->us_offstBgin;

			//set a pointer to the beginning of the record's available actual 
			//	content, taking into account any beginning offset
			puc += sizeof( CDTEXT) + pt_SPAN->us_offstBgin;
		//else the record is a CDPARAGRAPH, so indicate this by nullifying 
		//	the content pointer
		}else
			puc = NULL;

		//start off the current-length variable by the amount of the record's 
		//	content that could be included under any specified maximum limit
		i_ProcessRtfText( puc, us, ul_MAXLEN_CONTENT, 
										f_TRANSLATE_LINEFEEDS, &ul_length, 
										NULL, &f_MaximumReached);

		//if applicable and the maximum-desired content length has been met, 
		//	return that the span length is the value of the current-length 
		//	variable
		if (ul_MAXLEN_CONTENT && f_MaximumReached)
			return ul_length;
	//else if the beginning and ending cursors point to the same CD record, 
	//	short-circuit with a null content length
	}else if (puc == puc_END_RECORD)
		return NULL;

	//advance to the next CD record
	t_cursor = pt_SPAN->t_crsrBgin;
	AdvanceCdCursor( &t_cursor, pt_CONTEXT, NULL, NULL);

	//if no further records exist...
	if (!t_cursor.pt_item)
		//if an end cursor was specified, we failed to make it there, so 
		//	return general failure, else return that the content length 
		//	is the value of the current-length variable
		return puc_END_RECORD ? eul_ERR_FAILURE : ul_length;

	//loop until the CD record the end cursor points to is reached
	while (t_cursor.puc_location != puc_END_RECORD)	{
		//the current CD record is of interest only if it's a CDTEXT or 
		//	CDPARAGRAH
		uc = *t_cursor.puc_location;
		if (muc_SIG_CDTEXT == uc || muc_SIG_CDPARAGRAPH == uc)	{
			//if the record is a CDTEXT, determine the pointer to the 
			//	beginning of its available content and the content's length
			if (muc_SIG_CDTEXT == uc)	{
				puc = t_cursor.puc_location + sizeof( CDTEXT);
				us = t_cursor.us_recLength - sizeof( CDTEXT);
			//else the record is a CDPARAGRAPH, so indicate this by 
			//	nullifying the content pointer
			}else
				puc = NULL;

			//increment the current-length variable by the amount of the 
			//	record's content which could be included under any specified 
			//	maximum limit
			i_ProcessRtfText( puc, us, ul_MAXLEN_CONTENT, 
										f_TRANSLATE_LINEFEEDS, &ul_length, 
										NULL, &f_MaximumReached);

			//if applicable and the maximum-desired content length has been 
			//	met, return that the content length is the value of the 
			//	current-length variable
			if (ul_MAXLEN_CONTENT && f_MaximumReached)
				return ul_length;
		} //if (muc_SIG_CDTEXT == uc ||

		//advance the current cursor to the next CD record
		AdvanceCdCursor( &t_cursor, pt_CONTEXT, NULL, NULL);

		//if no further records exist...
		if (!t_cursor.pt_item)
			//if an end cursor was specified, we failed to make it there, so 
			//	return general failure, else return that the content length 
			//	is the value of the current-length variable
			return puc_END_RECORD ? eul_ERR_FAILURE : ul_length;
	} //while (t_cursor.puc_location != puc_END_RECORD)

	//if this end CD record is a CDTEXT or CDPARAGRAH...
	uc = *pt_SPAN->t_crsrBgin.puc_location;
	if (muc_SIG_CDTEXT == uc || muc_SIG_CDPARAGRAPH == uc)	{
		//if the ending cursor is on a CDTEXT, set a pointer to the beginning 
		//	of the record's available actual content, and set the length of 
		//	the record's available actual content equal to the ending offset
		if (muc_SIG_CDTEXT == uc)	{
			puc = puc_END_RECORD + sizeof( CDTEXT);
			us = pt_SPAN->us_offstEnd;
		//else the record is a CDPARAGRAPH, so indicate this by nullifying 
		//	the content pointer
		}else
			puc = NULL;

		//one last time, increment the current-length variable by the amount 
		//	of the record's content that could be included under any 
		//	specified maximum limit
		i_ProcessRtfText( puc, us, ul_MAXLEN_CONTENT, f_TRANSLATE_LINEFEEDS, 
													&ul_length, NULL, NULL);
	} //if (muc_SIG_CDTEXT == us ||

	//return that the content length is the value of the current-length 
	//	variable 
	return ul_length;
} //ul_TextSpanLength(


/** i_ProcessRtfText( ***
For the given CD record, this function appends sizing information about the 
record's textual content and, if requested, appends the record's content to 
a provided buffer.

--- parameters & return ---
pc_content: Pointer to the rich-text textual content to be sized or copied. 
	A null value indicates that a paragraph equivalent should be apppended.
us_LEN_AVAILABLE: the length of the current textual content available to 
	be sized or copied
ul_MAXLEN_CONTENT: Optional. The maximum-desired length of the rich-text 
	content being sized or copied. If null, this constraint functionality is 
	bypassed.
f_TRANSLATE_LINEFEEDS: flag specifying whether rich-text linefeeds and 
	paragraph markers should be translated to a CRLF for the sake of sizing 
	and copying (TRUE) or not (FALSE)
pul_lenAggregate: Input & Output. Pointer to the variable holding the 
	aggregate length of the rich-text thus far sized or copied. The variable 
	is incremented according to the number of bytes of additional content 
	copied or able-to-be-copied given any constraints.
pc_contentBuf: Pointer to a buffer into which the rich-text content should 
	be copied. A null pointer indicates that the content should only be 
	sized, not copied.
pf_MaximumReached: Optional. Pointer to a flag variable in which to return 
	whether the maximum-desired content length has been met (TRUE) or not 
	(FALSE). Only applicable when ul_MAXLEN_CONTENT has been specified. A 
	null pointer indicates that this functionality should be ignored.
RETURN: If dealing with a CDTEXT, the offset corresponding to the number of 
	content bytes processed. If the first content byte cannot be processed, 
	a return of -1 will be received, meaning that the ultimate last content 
	byte is the preceding CD record's last content byte.

--- revision history -------
3/20/00 PR: documenation adjustment
11/23/98 PR: created		*/
static int i_ProcessRtfText( const char *const  pc_CONTENT, 
								const WORD  us_LEN_AVAILABLE, 
								const DWORD  ul_MAXLEN_CONTENT, 
								const BOOL  f_TRANSLATE_LINEFEEDS, 
								DWORD *const  pul_lenAggregate, 
								BYTE  pc_contentBuf[], 
								BOOL *const  pf_MaximumReached)	{
	//determine whether there's any chance that an applicable 
	//	maximum-desired content length could be exhausted by processing  
	//	the specified rich-text content
	const BOOL f_CHANCE_OF_MAX = ul_MAXLEN_CONTENT && *pul_lenAggregate + 
										us_LEN_AVAILABLE * 
										(f_TRANSLATE_LINEFEEDS ? 
										eui_LEN_CRLF : 1) > ul_MAXLEN_CONTENT;

	WORD  i;

	_ASSERTE( pul_lenAggregate);

	//if applicable, default to FALSE the flag telling whether the maximum 
	//	content length has been reached
	if (pf_MaximumReached)
		*pf_MaximumReached = FALSE;

	//if the curent cursor is on a CDTEXT...
	if (pc_CONTENT)	{
		//if no translation of rich-text linefeed or paragraph markers 
		//	is requested
		if (!f_TRANSLATE_LINEFEEDS)	{
			//determine the length of the content of interest, accounting 
			//	if necessary for the maximum limit on content length
			i = us_LEN_AVAILABLE - (f_CHANCE_OF_MAX ? 
												(WORD) (*pul_lenAggregate + 
												us_LEN_AVAILABLE - 
												ul_MAXLEN_CONTENT) : 0);

			//if applicable, append the content of interest to the 
			//	copied-content buffer
			if (pc_contentBuf)
				memcpy( pc_contentBuf + *pul_lenAggregate, pc_CONTENT, i);

			//increment the current-length variable by the length of the 
			//	content of interest
			*pul_lenAggregate += i;
		//else it's most efficient to work character-by-character, accounting 
		//	for linefeed-to-CRLF translation as linefeeds occur
		}else
			//loop forward from the first to the last character of the 
			//	available content
			for (i = 0; i < us_LEN_AVAILABLE; i++)	{
				//if the pointed-to character is not a rich-text linefeed, 
				//	increment the current-length variable and, if applicable, 
				//	append the character to the copied-content buffer
				if (pc_CONTENT[ i] != mc_LINEFEED_RICHTEXT)	{
					if (pc_contentBuf)
						pc_contentBuf[ *pul_lenAggregate] = pc_CONTENT[ i];
					(*pul_lenAggregate)++;
				//else if no chance exists that the specified maximum limit
				//	will be exceeded, increment the current-length variable 
				//	by the size of a CRLF and, if applicable, append the CRLF 
				//	to the copied-content buffer
				}else if (!f_CHANCE_OF_MAX)	{
					if (pc_contentBuf)
						memcpy( pc_contentBuf + *pul_lenAggregate, epc_CRLF, 
																eui_LEN_CRLF);
					*pul_lenAggregate += eui_LEN_CRLF;
				//Else if applicable and the current-length plus the size 
				//	of the replacement CRLF exceeds the specified maximum 
				//	length, return that the maximum-desired content length 
				//	has been reached at the previous content character. This 
				//	test removes the possibility of a split CRLF occurring at 
				//	the end of the content buffer being filled/sized.
				}else if (ul_MAXLEN_CONTENT && *pul_lenAggregate + 
										eui_LEN_CRLF > ul_MAXLEN_CONTENT)	{
					if (pf_MaximumReached)
						*pf_MaximumReached = TRUE;

					return i - 1;
				//else increment the current-length variable by the size of 
				//	a CRLF and, if applicable, append the CRLF to the 
				//	copied-content buffer
				}else	{
					if (pc_contentBuf)
						memcpy( pc_contentBuf + *pul_lenAggregate, epc_CRLF, 
																eui_LEN_CRLF);
					*pul_lenAggregate += eui_LEN_CRLF;
				} //if (pc_content[ i] != c_LINEFEED_RICHTEXT)
			} //for (i = 0; i < us_LEN_AVAILABLE; i++)
		//END if (!f_TRANSLATE_LINEFEEDS)
	//else the current cursor must be on a CDPARAGRAPH...
	}else	{
		//if rich-text linefeeds and paragraph markers are to be translated 
		//	to CRLFs
		if (f_TRANSLATE_LINEFEEDS)	{
			//If applicable and the current-length plus the size of the 
			//	replacement CRLF exceeds the specified maximum length, return 
			//	that the maximum-desired content length has been reached. 
			//	This test removes the possibility of a split CRLF occurring 
			//	at the end of the content buffer being filled/sized.
			if (ul_MAXLEN_CONTENT && *pul_lenAggregate + eui_LEN_CRLF > 
														ul_MAXLEN_CONTENT)	{
				if (pf_MaximumReached)
					*pf_MaximumReached = TRUE;
				return NULL;
			//else increment the current-length variable by the size of a 
			//	CRLF and, if applicable, append a CRLF to the copied-content 
			//	buffer
			}else	{
				if (pc_contentBuf)
					memcpy( pc_contentBuf + *pul_lenAggregate, epc_CRLF, 
																eui_LEN_CRLF);
				*pul_lenAggregate += eui_LEN_CRLF;
			} //if (ul_MAXLEN_CONTENT && *pul_lenAggregate +
		//else increment the current-length variable and, if applicable, 
		//	append a rich-text linefeed to the copied-content buffer
		}else	{
			if (pc_contentBuf)
				pc_contentBuf[ *pul_lenAggregate] = mc_LINEFEED_RICHTEXT;
			*pul_lenAggregate;
		} //if (f_TRANSLATE_LINEFEEDS)
	} //if (pc_content)

	//if applicable and the maximum-desired content length has been met, 
	//	indicate that the maximum-desired content length has been reached
	if (ul_MAXLEN_CONTENT && pf_MaximumReached && *pul_lenAggregate == 
															ul_MAXLEN_CONTENT)
		*pf_MaximumReached = TRUE;

	//if we're dealing with a CDTEXT, return the offset into the content at 
	//	which processing was stopped (usually it will mark the entire content)
	return pc_CONTENT ? i : NULL;
} //i_ProcessRtfText(


/** eus_CursorToStringStart( ***
Purpose is to case-sensitive search for and point to the next instance of a 
passed-in string within the rich-text field whose context is passed in as 
well. A successful find means that the string has been discovered in, at its 
most complex, a contiguous succession of CDTEXT records, "contiguous" meaning 
uninterrupted by any other type of CD record.

--- parameters & return ----
PC: pointer to the string to be searched for
f_CASE_SENSITIVE: flag telling whether the search should be case-sensitive 
t_cursor: the starting point for the search within the rich-text field
us_OFFSET_AT_START: if the starting point is a CDTEXT record, the offset into 
	the record's text run at which the search should commence
pt_context: Optional. Pointer to contextual information about the rich-text 
	field being navigated. If null, only the stream of CD records following 
	t_cursor may be navigated, meaning that no switching from virtuality to 
	actuality can occur.
pt_foundCursor: Output. Pointer to the rich-text cursor in which to store the 
	coördinates of the CDTEXT containing at least the start of a found 
	instance of the string that was searched for. If no instance was found, 
	this cursor's location pointer is guaranteed to be NULL.
pus_foundOffset: Output. Pointer to the variable in which to store the 
	1-based offset into the CDTEXT's content after which an instance of the 
	sought-for string was found.
RETURN: TRUE if no error occured. FALSE otherwise

--- side effect --------
The rich-text environment information pointed to through t_cursor or 
pt_CONTEXT may be adjusted due to cursor advancement. If the enhancement 
suggested below is implemented, this side effect would be eliminated.

--- suggested enhancement ---
11/10/98 PR
The starting memory-lock state should be restored upon function exit (absent 
the item containing the "found cursor," of course), but no good mechanism is 
yet available with which to accomplish this. See this module's 
suggested-enhancement note for a description of the problem and proposed 
solution.

--- revision history -------
3/20/00 PR: token renaming
9/12/99 PR: documentation adjustment
11/22/98 PR: created		*/
BOOL ef_CursorToStringStart( const char  PC[], 
								const BOOL  f_CASE_SENSITIVE, 
								CdRecordCursor  t_cursor, 
								const WORD  us_OFFSET_AT_START, 
								const RtfTrackingInfo *const  pt_CONTEXT, 
								CdRecordCursor *const  pt_foundCursor, 
								WORD *const  pus_foundOffset)	{
	char * pc = NULL;
	BOOL  f_found = FALSE, f_result = TRUE;

	//if obvious invalidity among the parameters is present, short-circuit 
	//	with general failure
	if (!( PC && t_cursor.pt_item && t_cursor.puc_location && 
									pt_foundCursor && pus_foundOffset && 
									(pt_CONTEXT ? 
									(BOOL) pt_CONTEXT->pt_Actuality : TRUE)))
		return FALSE;

	//default the found cursor's location to NULL
	pt_foundCursor->puc_location = NULL;

	//if this is a non-case-sensitive search, convert the passed-in string 
	//	to upper-case, as required by the f_TestStringStart() function
	if (!f_CASE_SENSITIVE)	{
		if (!( pc = calloc( strlen( PC) + 1, sizeof( char))))
			return !eus_SUCCESS;
		strupr( strcpy( pc, PC));
	}

	//if the passed-in cursor is at a CDTEXT and the offset-at-start value is 
	//	reasonable, test whether a string match is found beginning in the 
	//	CDTEXT
	if (muc_SIG_CDTEXT == *t_cursor.puc_location && us_OFFSET_AT_START + 
									sizeof( CDTEXT) < t_cursor.us_recLength)
		if (!( f_result = f_TestStringStart( pc ? pc : PC, f_CASE_SENSITIVE, 
									t_cursor, us_OFFSET_AT_START, pt_CONTEXT, 
									&f_found, pus_foundOffset)))
			goto cleanUp;

	//If an instance of the sought-for string wasn't found in the first CDTEXT
	//	from the passed-in offset, search forward through the rest of the 
	//	rich-text field for an instance. Offsets are of no concern anymore.
	while (!f_found)	{
		//Advance the current cursor to the next CD record.  If no 
		//	rich-text is left, break out of the loop.
		AdvanceCdCursor( &t_cursor, pt_CONTEXT, NULL, NULL);
		if (!t_cursor.pt_item)
			break;

		//if the cursor is not at a CDTEXT, iterate the loop.
		if (muc_SIG_CDTEXT != *t_cursor.puc_location)
			continue;

		//if a string match is found beginning in the CDTEXT...
		if (!( f_result = f_TestStringStart( pc ? pc : PC, f_CASE_SENSITIVE, 
												t_cursor, 0, pt_CONTEXT, 
												&f_found, pus_foundOffset)))
			goto cleanUp;
	} //while (!f_found)

	//If an instance of the sought-for string was found, set the "found" 
	//	cursor equal to the cursor where the instance was located. The  
	//	"found" offset relative to the start of the sought-for string 
	//	was set during the search.
	if (f_found)
		*pt_foundCursor = t_cursor;
	//else nullify the puc_location member of the "found" cursor to signify 
	//	that no instance was found
	else
		pt_foundCursor->puc_location = NULL;

cleanUp:
	//if memory was allocated to accommodate a non-case-sensitive search, 
	//	free it
	if (pc)
		free( pc);

	return f_result;	//probably TRUE
} //ef_CursorToStringStart(


/** eus_CursorAdvance( ***
Advances the given rich-text cursor to the next CD record in the rich-text 
field.

--- parameters & return ----
pt_crsr: Input & Output. Address of rich-text cursor to be advanced. If no 
	ensuing CD record is available (because the end of the rich-text field has 
	been reached), cursor is nullified to indicate an end-reached status.
pt_CTX: address of structure describing the rich-text field being operated on
RETURN:
	eus_SUCCESS if no error occured
	eus_ERR_INVLD_ARG if any passed-in parameter is obviously invalid

--- side effect ------------
The rich-text environment information pointed to by pt_crsr or pt_CTX may be 
adjusted due to cursor advancement (item-locking, in particular).

--- revision history -------
9/6/02 PR: created			*/
STATUS eus_CursorAdvance( CdRecordCursor *const  pt_crsr, 
							const RtfTrackingInfo *const  pt_CTX)	{
	if (!( pt_crsr && pt_CTX))
		return eus_ERR_INVLD_ARG;

	AdvanceCdCursor( pt_crsr, pt_CTX, NULL, NULL);

	return eus_SUCCESS;
} //eus_CursorAdvance(


/** AdvanceCdCursor( ***
Purpose is to advance the given rich-text cursor to its next CD record, 
flexibly taking into account the rich-text field environment.

--- parameters -------------
pt_cursor: Input & Output. Pointer to the cursor to be advanced. Output is 
	either an advanced cursor or a cursor whose item pointer is nullified to 
	indicate that no ensuing CD records are available.
pt_CONTEXT: Optional Input. Pointer to the context of the rich-text 
	information being navigated. If passed in null, the cursor will not 
	travel automatically from a virtual or semi-virtual rich-text item to 
	an ensuing actual rich-text item.
pf_LeftSemiVirtuality: Optional Output. Pointer to the flag in which to store 
	whether the cursor has advanced out of semi-virtual rich-text content. If 
	passed in null, the procedure will ignore this functionality.
pf_ItemGotLocked: Optional. Pointer to the flag in which to store whether 
	a rich-text item was newly locked in the course of this procedure. If 
	passed in null, the procedure will ignore this functionality.

--- side effect ------------
The rich-text environment information pointed to by pt_cursor or pt_CONTEXT 
may be adjusted due to cursor advancement.

--- suggested enhancment ---
3/20/00 PR: In the case where an item's reported length exceeds the actual 
	length of valid CD content, the length member is not being corrected. It 
	probably could be corrected (carefully!), but an interested caller should 
	be notified if any update occurred.

--- revision history -------
3/20/00 PR
+ Improved fault tolerance to handle (1) case where an item's reported length 
  exceeds the actual length of valid CD content, and (2) case where the 
  cursor's length member is null and therefore invalid. In achieving these 
  aims, factored appropriate functionality out into the f_PushToNextItem() 
  procedure.
+ documentation adjustment

11/23/98 PR: created		*/
static void AdvanceCdCursor( CdRecordCursor *const  pt_cursor, 
								const RtfTrackingInfo *const  pt_CONTEXT, 
								BOOL *const  pf_LeftSemiVirtuality, 
								BOOL *const  pf_ItemGotLocked)	{
	BYTE * puc;

	_ASSERTE( pt_cursor && pt_cursor->puc_location && pt_cursor->pt_item && 
											pt_cursor->us_recLength && 
											(pt_CONTEXT ? (BOOL) 
											pt_CONTEXT->pt_Actuality : TRUE));

	//as applicable, indicate as a default that semi-virtuality has not been 
	//	left and the memory associated with an item different from the 
	//	current item has not been locked
	if (pf_LeftSemiVirtuality)
		*pf_LeftSemiVirtuality = FALSE;
	if (pf_ItemGotLocked)
		*pf_ItemGotLocked = FALSE;

	//If cursor is not on the last CD record of the current item... ("Last 
	//	record" is determined according to the following algorithm which 
	//	Notes itself seems to follow: <1> if the reported item length has 
	//	been exceeded [the '+ 1' is a measure to ensure that the last CD 
	//	record does not end one byte short of the length of the actual item 
	//	stored by Notes {a situation that occurs rarely, perhaps as a result 
	//	of some previous truncation of the item}] or <2> if the ostensible 
	//	next CD record has a null, and thus invalid, signature.)
	puc = pt_cursor->puc_location + pt_cursor->us_recLength + 
												pt_cursor->us_recLength % 2;
	if ((DWORD) (pt_cursor->puc_location - pt_cursor->pt_item->puc_start + 
											pt_cursor->us_recLength + 1) < 
											pt_cursor->pt_item->ul_length && 
											LOBYTE( *puc))
		//simply advance forward to the next record
		pt_cursor->puc_location = puc;
	//else move to the next appropriate rich-text item
	else
		if (!f_PushToNextItem( pt_cursor, pt_CONTEXT, 
									pf_LeftSemiVirtuality, pf_ItemGotLocked))
			return;

	//Store in the cursor structure the length of the just-moved-to CD 
	//	record. In the improper case that the length comes back as zero, push 
	//	on to the next item in stream (if any) and try again.
	while (!( pt_cursor->us_recLength = us_getCdRecLength( 
													pt_cursor->puc_location)))
		if (!f_PushToNextItem( pt_cursor, pt_CONTEXT, 
									pf_LeftSemiVirtuality, pf_ItemGotLocked))
			return;
} //AdvanceCdCursor(


/** f_PushToNextItem( ***
Moves cursor to the next rich-text item in the provided rich-text stream, 
flexibly taking into account the rich-text field environment.

--- parameters & return ----
pt_cursor: Input & Output. Pointer to the cursor to be advanced. Output is 
	either an advanced cursor or a cursor whose item pointer is nullified to 
	indicate that no ensuing CD records are available.
pt_CONTEXT: Optional Input. Pointer to the context of the rich-text 
	information being navigated. If passed in null, the cursor will not 
	travel automatically from a virtual or semi-virtual rich-text item to 
	an ensuing actual rich-text item.
pf_LeftSemiVirtuality: Optional Output. Pointer to the flag in which to store 
	whether the cursor has advanced out of semi-virtual rich-text content. If 
	passed in null, the procedure will ignore this functionality.
pf_ItemGotLocked: Optional. Pointer to the flag in which to store whether 
	a rich-text item was newly locked in the course of this procedure. If 
	passed in null, the procedure will ignore this functionality.
RETURN: TRUE if cursor was successfully moved to an ensuing rich-text item; 
	FALSE otherwise

--- side effect ------------
The rich-text environment information pointed to by pt_cursor or pt_context 
may be adjusted due to cursor advancement.

--- revision history -------
3/20/00 PR: created			*/
static BOOL f_PushToNextItem( CdRecordCursor *const  pt_cursor, 
								const RtfTrackingInfo *const  pt_CONTEXT, 
								BOOL *const  pf_LeftSemiVirtuality, 
								BOOL *const  pf_ItemGotLocked)	{
	_ASSERTE( pt_cursor && pt_cursor->puc_location && pt_cursor->pt_item && 
											(pt_CONTEXT ? (BOOL) 
											pt_CONTEXT->pt_Actuality : TRUE));

	//as applicable, indicate as a default that semi-virtuality has not been 
	//	left and the memory associated with an item different from the 
	//	current item has not been locked
	if (pf_LeftSemiVirtuality)
		*pf_LeftSemiVirtuality = FALSE;
	if (pf_ItemGotLocked)
		*pf_ItemGotLocked = FALSE;

	//if cursor is in something virtual and the caller has chosen the option 
	//	of switching over to actuality at this point...
	if (pt_CONTEXT && !(pt_cursor->pt_item->pt_next || 
								pt_cursor->pt_item->i_type == mi_ACTUAL))	{
		//if the caller wishes to know, note whether we're leaving 
		//	semi-virtuality
		if (pf_LeftSemiVirtuality && pt_CONTEXT->pt_SemiVirtuality && 
										f_ItemIsInList( pt_cursor->pt_item, 
										pt_CONTEXT->pt_SemiVirtuality))
			*pf_LeftSemiVirtuality = TRUE;

		//Set the cursor to point to the "next" record. If there aren't any 
		//	more records, quit this town; the pt_item member of the cursor 
		//	will have been nullified during the call to indicate the 
		//	condition.
		return f_LocateAbsoluteOffset( pt_CONTEXT->ul_ActualFrontier, 
											pt_CONTEXT->pt_Actuality, 
											&pt_cursor->pt_item, 
											&pt_cursor->puc_location, NULL, 
											pf_ItemGotLocked);
	} //if (pt_CONTEXT && !(pt_cursor->pt_item->pt_next ||

	//If no further items are available, we've reached the end of all rich 
	//	text known to this function, so return. The cursor's null item 
	//	pointer will indicate the condition to the caller.
	do
		if (!( pt_cursor->pt_item = pt_cursor->pt_item->pt_next))
			return FALSE;
	while (pt_cursor->pt_item->ul_length < sizeof( WORD) + sizeof( BSIG));

	//if the content of the succeeding item hasn't been locked in memory, do 
	//	it now, and if applicable, set to TRUE the flag telling whether a 
	//	rich-text item different from the item containing the starting 
	//	cursor was locked
	if (!pt_cursor->pt_item->puc_start)	{
		pt_cursor->pt_item->puc_start = OSLockBlock( BYTE, 
											pt_cursor->pt_item->bid_contents);
		if (pf_ItemGotLocked)
			*pf_ItemGotLocked = TRUE;
	}

	//Set cursor's location pointer to the first CD record in the item. For 
	//	non semi-virtual items, the WORD increment steps over the lead 
	//	TYPE_COMPOSITE item designation.
	pt_cursor->puc_location = pt_cursor->pt_item->puc_start + (pt_CONTEXT && 
										f_ItemIsInList( pt_cursor->pt_item, 
										pt_CONTEXT->pt_SemiVirtuality) ? 
										NULL : sizeof( WORD));

	return TRUE;
} //f_PushToNextItem(


/** f_ItemIsInList( ***
Purpose is to determine whether a given rich-text item appears in a given 
linked-list of rich-text items.

--- parameters & return ----
pt_ITEM: pointer to the rich-text item to test for appearance
pt_HEAD_NODE: pointer to the head node of the list to be searched
RETURN: TRUE if the given item appears in the list; FALSE otherwise

--- revision history -------
10/27/98 PR: created		*/
static __inline BOOL f_ItemIsInList( 
									const ItemInfo *const  pt_ITEM, 
									const ItemInfo *const  pt_HEAD_NODE)	{
	const ItemInfo * pt_item;

	_ASSERTE( pt_ITEM);

	//if the list passed in is empty, short-circuit that the item is not 
	//	in the list
	if (!( pt_item = pt_HEAD_NODE))
		return FALSE;

	//loop through the items in the passed-in list
	do	{
		//if the passed-in item equates to the list item, return that 
		//	the passed-in item does appear in the list
		if (pt_ITEM->bid_contents.pool == pt_item->bid_contents.pool)
			return TRUE;
	} while( pt_item = pt_item->pt_next);

	//since we've made it through the list without a match, return 
	//	that the passed-in item does not appear in the list
	return FALSE;
} //f_ItemIsInList(


/** f_LocateAbsoluteOffset( ***
Translate a logical offset into a succession of CD records into an actual 
cursor "pointer" to the physical location associated with the offset. The 
logical offset is specified in terms of bytes and ignores the fact that 
rich-text _items_ are concatenated to comprise a rich-text _field_. Rather the 
offset views the succession of rich-text CD records as uniterrupted. This 
procedure translates the logical offset into the actual corresponding 
coördinates within the rich-text item containing the logical offset.

--- parameters & return ----
ul_OFFST: the logical offset to be translated into physical location pointers
pt_hdNode: address of the item from which the logical offset is to be measured
ppt_itm: Output. Address of the address in which to store the pointer to the 
	rich-text item containing the logical offset. The address is guaranteed to 
	be null if the offset is invalid for some reason.
ppuc: Optional Output. Pointer to the address in which to store the pointer to 
	the CD record referred to by the logical offset. If pointer is null, only 
	the item output will be provided, and the item's contents will not be 
	locked in memory purely because this procedure was called.
pul_lenItems: Optional Output. Address of the variable in which to store the 
	aggregate length of the physical rich-text items up to and including the 
	item containing the logical offset. If address is null, this functionality 
	will be ignored.
pf_itmGotLckd: Optional Output. Address of the variable in which to store 
	whether the memory containing the content of the rich-text item in which 
	the logical offset falls was locked in order that pointers into the content 
	could be set. If address is null or if byte-location output is suppressed, 
	this functionality will be ignored.
RETURN: TRUE if the absolute offset was successfully located; FALSE otherwise

--- revision history -------
3/14/03 PR
+ made the byte-location output optional
+ token renaming, listing format adjustment, documentation adjustment

11/18/98 PR: created		*/
static BOOL f_LocateAbsoluteOffset( DWORD  ul_OFFST, 
									ItemInfo *const  pt_hdNode, 
									ItemInfo * *const  ppt_itm, 
									const BYTE * *const  ppuc, 
									DWORD *const  pul_lenItems, 
									BOOL *const  pf_itmGotLckd)	{
	ItemInfo * pt_itm = pt_hdNode;
	DWORD  ul = NULL;
	BOOL  f_itmGotLckd = FALSE;

	_ASSERTE( pt_hdNode && ppt_itm);

	*ppt_itm = NULL;
	if (ppuc)
		*ppuc = NULL;
	if (pul_lenItems)
		*pul_lenItems = NULL;
	if (pf_itmGotLckd)
		*pf_itmGotLckd = FALSE;

	//get the length of all actual items up to and including the one containing 
	//	the logical offset
	while ( (ul += pt_itm->ul_length) < ul_OFFST)	{
		//add on the "even out" byte as needed
		ul += pt_itm->ul_length % 2;

		if (!( pt_itm = pt_itm->pt_next))
			return FALSE;
	}

	//if the aggregate length equals the logical offset, the offset must point 
	//	to invalid territory, so return that the offset could not be located
	if (ul == ul_OFFST)
		return FALSE;

	//If necessary, lock the memory containing the content of the rich-text 
	//	item containing the logical offset. Also, if the caller requests, 
	//	communicate whether a lock was made.
	if (!pt_itm->puc_start)	{
		pt_itm->puc_start = OSLockBlock( BYTE, pt_itm->bid_contents);
		f_itmGotLckd = TRUE;
	}

	//if the caller requests, communicate the aggregate length of the rich-text 
	//	items involved in determining the physical location of the logical 
	//	offset
	if (pul_lenItems)
		*pul_lenItems = ul;

	//set the pointers the caller will use to access the physical rich-text 
	//	item and CD record "pointed to" by the logical offset
	if (ppuc)	{
		*ppuc = pt_itm->puc_start + pt_itm->ul_length - (ul - ul_OFFST);
		if (pf_itmGotLckd)
			*pf_itmGotLckd = f_itmGotLckd;
	}
	*ppt_itm = pt_itm;

	//return that the offset was successfully translated
	return TRUE;
} //f_LocateAbsoluteOffset(


/** f_TestStringStart( ***
Purpose is to test whether an instance of a particular string is found starting 
in the current CDTEXT record, from the passed-in offset on. Note that the 
entire string doesn't need to be in the current CDTEXT; it just needs to start 
there. As the function looks on into follow-on CDTEXTs for remaining characters 
in a string instance that may be a match, if a non-CDTEXT record is 
encountered, the search is aborted.

--- parameters & return -----
PC: The string for which an instance is being sought in the current rich-text 
	field. If the search is to be non-case-sensitive, this string must be in 
	*upper case*.
f_CASE_SNSTV: flag telling whether the search should be case-sensitive
t_CRSR: cursor pointing to the initial CDTEXT to be searched for the start of 
	an instance of the sought-for string
us_OFFST_AT_STRT: offset into the record's text run after which the search 
	should commence
pt_CTX: Pointer to environment information about the rich-text field being 
	manipulated.
pf_fnd: Output. Pointer to the flag in which to store whether the start of an 
	instance of the string was found (TRUE) or not (FALSE).
pus_fndOffst: Output. Pointer to the variable in which to store the 1-based 
	offset into the CDTEXT after which the start of a found instance begins.
RETURN: TRUE if no error occurred. FALSE if heap memory could not be allocated.

--- side effect --------
The rich-text environment information pointed to through t_CRSR or pt_CTX may 
be adjusted due to cursor advancement. If the enhancement suggested below is 
implemented, this side effect would be eliminated.

--- suggested enhancement ---
11/10/98 PR
The starting memory-lock state should be restored upon function exit, but no 
good mechanism is yet available with which to accomplish this. See this 
module's suggested-enhancement note for a description of the problem and 
proposed solution. The sidelined commentatry in this function indicates an 
direction by which such a mechanism might be implemented.

On second thought, it would probably be better to do the state saving and 
restoring in a higher-level function, because this function may be called so 
often that it's better just to leave memory locked.

--- revision history --------
9/6/02 PR
+ fixed bugs in tracking amount of memory left to be searched
+ listing format adjustment, token renaming, minor documentation adjustment

3/20/00 PR: token renaming
9/12/99 PR: documentation adjustment
11/10/98 PR: created		*/
static BOOL f_TestStringStart( const char  PC[], 
								const BOOL  f_CASE_SNSTV, 
								const CdRecordCursor  t_CRSR, 
								const WORD  us_OFFST_AT_STRT, 
								const RtfTrackingInfo *const  pt_CTX, 
								BOOL *const  pf_fnd, 
								WORD *const  pus_fndOffst)	{
	const WORD  us_LEN_STR = strlen( PC), 
				us_LEN_1ST_OFFST = sizeof( CDTEXT) + us_OFFST_AT_STRT, 
				us_BYTES_TO_SRCH_INITIAL = t_CRSR.us_recLength - 
															us_LEN_1ST_OFFST;

	char * pc_uprVer = NULL, * pc_latest = NULL, * pc, * pc_;
	WORD  us_bytesToSrch = us_BYTES_TO_SRCH_INITIAL, 
			us_lenAlloc = 0, us_chrsMtchd, us_bytesToSrchNxt;
	CdRecordCursor  t_crsr;
//MemoryLockStateInfo  t_lockState;
	BOOL  f_ItmGotLockd, f_rslt = TRUE;

	_ASSERTE( PC && t_CRSR.puc_location && muc_SIG_CDTEXT == 
										*t_CRSR.puc_location && pf_fnd && 
										pus_fndOffst && t_CRSR.us_recLength >= 
										us_LEN_1ST_OFFST);

	*pf_fnd = FALSE;
	*pus_fndOffst = NULL;

	//if there isn't any content to search, short-circuit that no instance of 
	//	the sought-for string starts in this CDTEXT
	if (!us_bytesToSrch)
		return TRUE;

	//Set the step-wise pointer to be used in the search to one less than where 
	//	the search will start. Also, if this search is to be case-insensitive, 
	//	create an upper-case copy of the content of interest in the initial 
	//	CDTEXT.
	if (!f_CASE_SNSTV)	{
		if (!( pc = pc_uprVer = calloc( us_bytesToSrch + 1, sizeof( char))))
			return FALSE;
		strupr( memcpy( pc--, t_CRSR.puc_location + us_LEN_1ST_OFFST, 
															us_bytesToSrch));
	}else
		pc = t_CRSR.puc_location + us_LEN_1ST_OFFST - 1;

//store a copy of the current memory-lock state of the rich-text field's 
//	item content
//EnsuingLockStatePush( t_CRSR.pt_item, pt_CTX, &t_lockState);

	//loop until an instance of the sought-for string is found or it is proven 
	//	that the initial CDTEXT starts no instance of the string
	while (pc = memchr( pc_ = pc + 1, PC[0], us_bytesToSrch))	{
		//initialize this pass by updating variables telling how many 
		//	characters in the sought-for string have been matched and how many 
		//	characters remain to be searched in the initial CDTEXT
		us_chrsMtchd = 1;
		us_bytesToSrch -= 1 + pc - pc_;
		if ((us_bytesToSrchNxt = us_bytesToSrch) > us_LEN_STR - 1)
			us_bytesToSrchNxt = us_LEN_STR - 1;

		//if the next characters to be tested do not match the sought-for 
		//	string, search anew starting with the next character over
		if (memcmp( pc + us_chrsMtchd, PC + us_chrsMtchd, us_bytesToSrchNxt) != 
																		ei_SAME)
			continue;

		//Update the number of characters matched. If the number equals the 
		//	length of the sought-for string, set the found flag.
		if (( us_chrsMtchd += us_bytesToSrchNxt) == us_LEN_STR)
			*pf_fnd = TRUE;
		//else prepare to search follow-on CDTEXT records by intializing a 
		//	navigation cursor to the current CDTEXT
		else
			t_crsr = t_CRSR;

		//loop through follow-on CDTEXTs until it has been determined whether 
		//	the current potential match is indeed a match
		while (!*pf_fnd)	{
			//Advance to the next CD record in the rich-text field. If the 
			//	record is not a CDTEXT, break out of the loop.
			AdvanceCdCursor( &t_crsr, pt_CTX, NULL, &f_ItmGotLockd);
			if (muc_SIG_CDTEXT != *t_crsr.puc_location)
				break;

			//Determine the number of bytes to be searched based on the content 
			//	of the latest CDTEXT record. If the CDTEXT has no content, 
			//	iterate the loop to get the next CDTEXT.
			if (!( us_bytesToSrchNxt = t_crsr.us_recLength - (WORD) sizeof( 
										CDTEXT) < us_LEN_STR - us_chrsMtchd ? 
										t_crsr.us_recLength - sizeof( CDTEXT) : 
										us_LEN_STR - us_chrsMtchd))
				continue;

			//If this is a non-case-sensitive search, copy the bytes to be 
			//	searched into a string and convert to upper case. If needed, 
			//	increase (or create) the memory set aside for the string if not 
			//	enough is available already.
			if (!f_CASE_SNSTV)	{
				if (us_bytesToSrchNxt + 1 > us_lenAlloc)
					if (!( pc_latest = realloc( pc_latest, us_lenAlloc = 
												us_bytesToSrchNxt + 1)))	{
						f_rslt = FALSE;
						goto cleanUp;
					}
				memcpy( pc_latest, t_CRSR.puc_location + sizeof( CDTEXT), 
															us_bytesToSrchNxt);
				pc_latest[ us_bytesToSrchNxt] = NULL;
				strupr( pc_latest);
			} //if (!f_CASE_SNSTV)

			//if the next characters to be tested do not match the sought-for 
			//	string, break out of the immediate loop
			if (memcmp( pc_latest ? pc_latest : t_crsr.puc_location + sizeof( 
												CDTEXT), PC + us_chrsMtchd, 
												us_bytesToSrchNxt) != ei_SAME)
				break;

			//Update the number of characters matched. If the number equals the 
			//	length of the sought-for string, set the found flag.
			if (( us_chrsMtchd += us_bytesToSrchNxt) == us_LEN_STR)
				*pf_fnd = TRUE;
		} //while (!*pf_fnd)

		//if an instance of the sought-for string was found, set the "found 
		//	offset" parameter to the offset after which the first character was 
		//	found, then break out of the loop
		if (*pf_fnd)	{
			*pus_fndOffst = pc - (pc_uprVer ? pc_uprVer - us_OFFST_AT_STRT : 
										t_CRSR.puc_location + sizeof( CDTEXT));
			break;
		}
	} //while (pc = memchr( pc + 1, PC[0],

cleanUp:
//if necessary, restore the original memory-lock state of the rich-text field's 
//	item content
//if (f_ItmGotLockd)
//	EnsuingLockStateRestore( t_lockState);

	if (!f_CASE_SNSTV)	{
		if (pc_uprVer)
			free( pc_uprVer);
		if (pc_latest)
			free( pc_latest);
	}

	return f_rslt;
} //f_TestStringStart(


/** eus_SwapHotspots( ***
Purpose is to replace a given rich-text hotspot with a different rich-text 
hotspot.

--- parameters & return ----
pt_cursor: Input & Output. Pointer to the rich-text cursor at which the 
	hotspot to be replaced is located. If the replacement is successful, the 
	cursor will be reset to point to the beginning of the new, replacement 
	hotspot in the context of the overall rich-text field.
t_replacementCursor: the cursor pointing to the beginning of the CD records 
	comprising the replacement hotspot
pt_context: Input. Pointer to the environment information about the 
	rich-text field being manipulated.
RETURN: eus_SUCCESS if no error occured. !eus_SUCCESS if any parameters or 
	the replacement hotspot is invalid. The Notes API error code otherwise.

--- side effect --------
The rich-text environment information associated with pt_cursor or pt_context 
will likely be adjusted due to writing CD records and to cursor advancement.

--- revision history -------
11/10/98 PR: created		*/
STATUS eus_SwapHotspots( CdRecordCursor *const  pt_cursor, 
							CdRecordCursor  t_replacementCursor, 
							RtfTrackingInfo *const  pt_context)	{
	//if any of the parameters are obviously invalid , short-circuit with 
	//	general failure
	if (!( pt_cursor && pt_cursor->puc_location && pt_cursor->pt_item && 
								t_replacementCursor.puc_location && 
								t_replacementCursor.pt_item && pt_context && 
								pt_context->pt_Actuality))
		return !eus_SUCCESS;

	//if the feedback option was enabled, default that the replacement 
	//	hotspot is not invalid

	//if the last CD record in the replacement hotspot CD records in not 
	//	an end-hotspot record, short-circuit with failure

	//if no virtualization has occurred yet
		//set the actual frontier to the beginning of the current 
		//	actual rich-text item
		//note the first actual item to be virtualized in a local variable
	//virtualize from the current actual frontier up to the start 
	//	of the attachment hotspot
	//write in the replacement rich-text text hotspot
	//reset the actual frontier to the record following the next, actual 
	//	hotspot end record
	//reset the passed-in cursor to point to the beginning of the 
	//	replacement hotspot

	return eus_SUCCESS;
} //eus_SwapHotspots(


/** us_CompileActuality( ***
Purpose is to compile a linked-list complete description of the specified 
rich-text field stored in the specified note. A rich-text field is comprised of 
a succession of same-named TYPE_COMPOSITE items.

--- parameters & return ----
h_NOTE: the note to be inspected for the specifiend rich-text field
pc_ITMNM: the name of the rich-text field whose item information is to be 
	gathered
ppt_Actuality: Output. Pointer to the head node of the linked-list complete 
	description of the specified rich-text field. If the specified rich-text 
	field does not exist, the head node is guaranteed to be NULL.
RETURN:
	eus_SUCCESS if no errors occurred
	ERR_ITEM_DATATYPE if specified item is not rich-text in type
	ERR_MEMORY if a memory allocation for the item-description list failed
	the Notes API error code otherwise

--- revision history -------
3/14/03 PR
+ minor signature adjustment to provide appropriate constness
+ listing format adjustment, minor documentation adjustment, token renaming

3/20/00 PR
+ enhanced context-checking to react properly if specified item is not rich-text
+ improved error-return information & associated documentation
+ minor performance enhancement

10/26/98 PR: created		*/
static STATUS us_CompileActuality( const NOTEHANDLE  h_NOTE, 
									const char  pc_ITMNM[], 
									ItemInfo * *const  ppt_Actuality)	{
	WORD  us_len, us;
	ItemInfo_Actual  t_actualItm;
	ItemInfo * pt_entry;
	STATUS  us_err;

	_ASSERTE( h_NOTE && pc_ITMNM && ppt_Actuality);

	//if we can't get information about the named item...
	if (us_err = NSFItemInfo( (NOTEHANDLE) h_NOTE, (char *) pc_ITMNM, us_len = 
											(WORD) strlen( pc_ITMNM), 
											&t_actualItm.bid_item, &us, 
											&t_actualItm.t_item.bid_contents, 
											&t_actualItm.t_item.ul_length))
		//if the specified item doesn't exist...
		if (ERR( us_err) == ERR_ITEM_NOT_FOUND)	{
			//nullify the list to indicate that no rich-text needs to be 
			//	processed
			*ppt_Actuality = NULL;
			return eus_SUCCESS;
		//else throw back whatever error this is
		}else
			return us_err;

	//if the specified item isn't rich-text, short-circuit with general failure
	if (us != TYPE_COMPOSITE)
{BYTE * puc = OSLockBlock( BYTE, t_actualItm.t_item.bid_contents);
NSFItemQuery( h_NOTE, t_actualItm.bid_item, NULL, NULL, NULL, &us, NULL, 
																NULL, NULL);
OSUnlockBlock( t_actualItm.t_item.bid_contents);
		return ERR_ITEM_DATATYPE;
}
	//allocate memory to hold the first item's information in the linked-list 
	//	complete description of the rich-text field
	if (!( pt_entry = *ppt_Actuality = malloc( sizeof( ItemInfo_Actual))))
		return ERR_MEMORY;

	//initiate loop to finish construction of a complete description of the 
	//	rich-text field's item information
	for (; ei_FOREVER;)	{
		BLOCKID  bid_prvItm;

		//Initialize the final members of the item-information structure: the 
		//	pointer to the start of the item's content (NULL indicates that the 
		//	content hasn't been memory locked) and the type member to signify 
		//	that this is an actual, not virtual item.
		t_actualItm.t_item.puc_start = NULL;
		t_actualItm.t_item.i_type = mi_ACTUAL;

		//The structure filled, copy its contents into the item's entry in the
		//	complete linked-list description of the rich-text field. The entry
		//	must be cast to an ItemInfo_Actual to avoid a compiler warning.
		*(ItemInfo_Actual *) pt_entry = t_actualItm;

		//Get the item information about the next, same-named rich-text item in 
		//	the note. If no next item exists, close off the linked-list 
		//	description of the rich-text field.
		bid_prvItm = t_actualItm.bid_item;
		if (us_err = NSFItemInfoNext( (NOTEHANDLE) h_NOTE, bid_prvItm, (char *) 
									pc_ITMNM, us_len, &t_actualItm.bid_item, 
									NULL, &t_actualItm.t_item.bid_contents, 
									&t_actualItm.t_item.ul_length))
			if (ERR( us_err) == ERR_ITEM_NOT_FOUND)	{
				pt_entry->pt_next = NULL;

				return eus_SUCCESS;
			//Else an unexpected error has occurred. Free whatever memory was 
			//	allocated within the function before returning the error.
			}else	{
				FreeItemList( ppt_Actuality);
				return us_err;
			} //if (ERR( us_err) == ERR_ITEM_NOT_FOUND)

		//allocate memory to hold this next item's information in the 
		//	linked-list complete description of the rich-text field
		if (!( pt_entry = pt_entry->pt_next = malloc( 
												sizeof( ItemInfo_Actual))))	{
			FreeItemList( ppt_Actuality);
			return ERR_MEMORY;
		}
	} //for (; ei_FOREVER;)

	return 0;	//should never get here, line used to avoid a compiler 
				//	warning in VC++ 6.0
} //us_CompileActuality(


/** eus_AddRtfActualContext( ***
Add "actuality" context assumed from the specified rich-text field to the 
provided rich-text context structure. This exported interface provides a means 
by which an already constructed stream of virtual CD records may be prepared to 
replace an existing rich-text field already attached to a note.

--- NOTE! ------------------
3/20/00 Procedure not yet tested!!

--- parameters & return ----
h_NOTE: the note to be inspected for the specifiend rich-text field
pc_ITMNM: the name of the rich-text field whose item information is to be 
	gathered
pt_context: Output. Structure containing contextual information about the 
	rich-text field being manipulated. This structure is required input to 
	most of the public interfaces to this module and so must be maintained 
	by the calling procedure.
RETURN: eus_SUCCESS if no error occured; the Notes API error code otherwise

--- revision history -------
3/14/03 PR: documentation adjustment
3/20/00 PR: created		*/
STATUS eus_AddRtfActualContext( NOTEHANDLE  h_NOTE, 
								char  pc_ITMNM[], 
								RtfTrackingInfo *const  pt_context)	{
	if (!( h_NOTE && pc_ITMNM && pt_context))
		return !eus_SUCCESS;

	//fill out the context's Actuality list
	return us_CompileActuality( h_NOTE, pc_ITMNM, &pt_context->pt_Actuality);
} //eus_AddRtfActualContext(


/** ef_CopyRtfContent( ***
Produces a copy of the rich-text content associated with a given rich-text 
context.

--- NOTE! ------------------
3/20/00 Procedure not yet tested!!

--- parameters & return ----
pt_SRC: structure containing contextual information about the rich-text 
	content to be copied
f_CONSERVE_MEMORY: flag telling whether as much as possible only the memory 
	needed for a copy of the source content should be allocated (TRUE) or 
	(FALSE) any "slack" memory currently allocated in the source container 
	(for content growth or insertion) should be preserved
pt_dest: Output. Structure in which to record context information about the 
	copy produced of the source's virtual rich-text content. If procedure is 
	unsuccessful, the structure is guaranteed to reflect that it has no 
	virtual content.
RETURN: eus_SUCCESS if no error occured; the Notes API error code otherwise

--- revision history -------
3/20/00 PR: created		*/
STATUS eus_CopyRtfContent( const RtfTrackingInfo *const  pt_SRC, 
							const BOOL  f_CONSERVE_MEMORY, 
							RtfTrackingInfo *const  pt_dest)	{
	ItemInfo * pt_s;
	DWORD  ul;
	BOOL  f_hasActualityToCopy;
	STATUS  us_err;

	if (!( pt_SRC && pt_dest))
		return !eus_SUCCESS;

	pt_dest->pt_Virtuality = pt_dest->pt_endVirtual = NULL;

	//if there's nothing to copy, short-circuit with success
	pt_s = pt_SRC->pt_Virtuality;
	if (!( (f_hasActualityToCopy = pt_SRC->pt_Actuality && 
											pt_SRC->ul_ActualFrontier != 
											mul_FRONTIER_NO_MORE) || pt_s))
		return eus_SUCCESS;

	//if source has virtualized content...
	if (pt_s)	{
		BOOL  f_keepNewItemOpenForMore, f_ItemJustLocked = FALSE;
		ItemInfo * pt_d;

		//start off destination's "virtual" item-information list
		ul = pt_s->ul_length;
		f_keepNewItemOpenForMore = f_hasActualityToCopy && !pt_s->pt_next;
		if (us_err = us_StartVirtualItemList( f_CONSERVE_MEMORY && 
												!f_keepNewItemOpenForMore ? 
												ul : NULL, pt_dest))
			return us_err;

		//for each of source's virtual items...
		pt_d = pt_dest->pt_Virtuality;
		for (; ei_FOREVER; ) {
			//if necessary, lock in the source item
			if (!pt_s->puc_start)	{
				pt_s->puc_start = OSLockBlock( BYTE, pt_s->bid_contents);
				f_ItemJustLocked = TRUE;
			}

			//copy the content from source to destination
			memcpy( pt_d->puc_start, pt_s->puc_start, ul);
			pt_d->ul_length = ul;

			//if we won't be writing anymore to the new destination item, 
			//	release it
			if (!f_keepNewItemOpenForMore)
				UnlockItem( pt_d);

			//if we locked in the source item, release it now
			if (f_ItemJustLocked)	{
				UnlockItem( pt_s);
				f_ItemJustLocked = FALSE;
			}

			//if there's a follow-on source item...
			if (pt_s = pt_s->pt_next)	{
				//Tack on another destination virtual item. The item will be 
				//	"locked in" by the us_InsertNextVirtualItem() call.
				ul = pt_s->ul_length;
				f_keepNewItemOpenForMore = f_hasActualityToCopy && 
															!pt_s->pt_next;
				if (us_err = us_InsertNextVirtualItem( pt_d, 
												mi_REGULAR_VIRTUAL, 
												f_CONSERVE_MEMORY && 
												!f_keepNewItemOpenForMore ? 
												ul : NULL))	{
					ClearItemList( &pt_dest->pt_Virtuality);
					return us_err;
				}
				pt_d = pt_d->pt_next;
			}else
				break;
		} //for (; ei_FOREVER;)

		pt_dest->pt_endVirtual = pt_d;
	} //if (pt_s)

	//if unvirtualized actual content remains to be copied...
	if (f_hasActualityToCopy)	{
		//construct a temporary context to manage the remaining 
		//	virtualization needed
		RtfTrackingInfo  t_context = *pt_SRC;
		t_context.pt_Virtuality = pt_dest->pt_Virtuality;
		t_context.pt_endVirtual = pt_dest->pt_endVirtual;

		//virtualize the remaining actual content
		if (us_err = us_VirtualizeRestOfRtf( &t_context, NULL))	{
			ClearItemList( &t_context.pt_Virtuality);
			pt_dest->pt_Virtuality = pt_dest->pt_endVirtual = NULL;
			return us_err;
		}

		//update the destination context structure with the final results
		pt_dest->pt_Virtuality = t_context.pt_Virtuality;
		pt_dest->pt_endVirtual = t_context.pt_endVirtual;
	} //if (f_hasActualityToCopy)

	return eus_SUCCESS;
} //ef_CopyRtfContent(


/** eus_ReplaceRtfWithCdStream( ***


--- parameters & return ----

RETURN: eus_SUCCESS if no error occured; the Notes API error code otherwise

--- revision history -------
3/14/03 PR: minor logic adjustment in support of signature change to 
	us_VirtualAppend()
12/12/98 PR: created		*/
//DOC!!
STATUS eus_ReplaceRtfWithCdStream( const BYTE *const  PUC, 
									const DWORD  ul_LEN, 
									RtfTrackingInfo *const  pt_context)	{
	ItemInfo * pt_item;
	int  i_offset;

	if (!( PUC && ul_LEN > (DWORD) (i_offset = (*(WORD *) PUC == 
											TYPE_COMPOSITE ? sizeof( WORD) : 
											NULL)) && pt_context))
		return !eus_SUCCESS;

	//set the actual-frontier member of the tracking structure to say that 
	//	there is no more frontier
	pt_context->ul_ActualFrontier = mul_FRONTIER_NO_MORE;

	//if virtuality has been started, clear all the virtual items except the 
	//	first and set the first item's length member to the shortest 
	//	possible length
	if (pt_item = pt_context->pt_Virtuality)	{
		if (pt_item->pt_next)
			ClearItemList( &pt_item->pt_next);
		pt_item->ul_length = sizeof( WORD);
	}

	//virtualize the input stream of CD records to the context associated 
	//	with the specified rich-text field
	return us_VirtualAppend( PUC + i_offset, ul_LEN - i_offset, pt_context, 
																ul_LEN, NULL);
} //eus_ReplaceRtfWithCdStream(


/** FreeItemList( ***
Purpose is to free allocated memory associated with the given list of rich-text 
item structures.

--- parameter -----------
ppt_ListHead: Input & Output. Pointer to the head node of the list to be freed. 
	The head node will return nullified.

--- revision history ----
3/14/03 PR: listing format adjustment
10/29/98 PR: created		*/
static __inline void FreeItemList( ItemInfo * *const  ppt_ListHead)	{
	ItemInfo * pt_item, * pt_next;

	_ASSERTE( ppt_ListHead && *ppt_ListHead && 
											(*ppt_ListHead)->bid_contents.pool);

	//loop through the nodes in the passed-in list, freeing associated memory 
	pt_item = *ppt_ListHead;
	do	{
		//if the item's a virtual item that was never written to a note, free 
		//	the block of memory associated with the item
		if (pt_item->i_type == mi_VIRTUAL && pt_item->bid_contents.pool)
			OSMemFree( pt_item->bid_contents.pool);

		pt_next = pt_item->pt_next;
		free( pt_item);
	} while (pt_item = pt_next);

	//nullify the head node to signify that the given list has been freed
	*ppt_ListHead = NULL;
} //FreeItemList(


/** us_getCdRecLength( ***
Purpose is to return the length of the passed-in rich-text CD record.

--- parameter & return ----
puc_CD_RECORD: pointer to the CD record whose length needs to be determined
RETURN: the length in bytes of the passed-in CD record

--- revision history ------
3/20/00 PR: safety adjustment, token renaming
10/29/98 PR: created		*/
static __inline WORD us_getCdRecLength( const BYTE *const  puc_CD_RECORD)	{
	DWORD  ul;

	_ASSERTE( puc_CD_RECORD);

	//Get the length from the length member of the signature structure. The 
	//	first byte of a CD Record defines the "signature" type of the record. 
	//	This byte is or'ed with a second-byte mask specifying whether the 
	//	length of the record can be provided in the space of a BYTE, WORD or 
	//	DWORD.
	switch (HIBYTE( *(WORD *) puc_CD_RECORD))	{
		case muc_SIGTYPE_LSIG:
			ul = ((LSIG *) puc_CD_RECORD)->Length;
			break;
		case muc_SIGTYPE_WSIG:
			ul = ((WSIG *) puc_CD_RECORD)->Length;
			break;
		default: //muc_SIGTYPE_BSIG:
			ul = ((BSIG *) puc_CD_RECORD)->Length;
	}
	_ASSERTE( ul && ul == (WORD) ul);

	return (WORD) ul;
} //us_getCdRecLength(


/** ef_GetPgraphStyleCount( ***


--- parameters & return ----

RETURN: eus_SUCCESS if no error occured; the Notes API error code otherwise

--- revision history -------
3/20/00 PR: logic shortening by leveraging the new UnlockItem() procedure
1/17/99 PR: created			*/
//DOC!!
BOOL ef_GetPgraphStyleCount( CdRecordCursor  t_cursor, 
								const RtfTrackingInfo *const  pt_CONTEXT, 
								WORD *const  pus_styles)	{
	WORD  us = 0;
	BOOL  f_ItemGotLocked;
	ItemInfo * pt_itemLocked = NULL;

	if (!( t_cursor.puc_location && t_cursor.pt_item && 
													t_cursor.us_recLength && 
													pt_CONTEXT && pus_styles))
		return FALSE;

	*pus_styles = NULL;

	//loop record-by-record through the rich-text content
	while (t_cursor.pt_item)	{
		if (LOBYTE( SIG_CD_PABDEFINITION) == *t_cursor.puc_location)
			us++;

		//advance the cursor to the next CD record
		AdvanceCdCursor( &t_cursor, pt_CONTEXT, NULL, &f_ItemGotLocked);

		//if we just moved to a follow-on rich-text item and the previous 
		//	item was locked by this procedure, unlock the previous item
		if (pt_itemLocked && pt_itemLocked != t_cursor.pt_item)	{
			UnlockItem( pt_itemLocked);
			pt_itemLocked = NULL;
		}

		//if a follow-on rich-text item was locked by this procedure, flag 
		//	the item for later unlocking
		if (f_ItemGotLocked)
			pt_itemLocked = t_cursor.pt_item;
	} //while (t_cursor.pt_item)

	*pus_styles = us;

	return TRUE;
} //ef_GetPgraphStyleCount(


/** ef_FindPgraphStyleUnusedSpan( ***


--- parameters & return ----

RETURN: eus_SUCCESS if no error occured; the Notes API error code otherwise

--- revision history -------
3/20/00 PR: logic shortening by leveraging the new UnlockItem() procedure
1/17/99 PR: created			*/
//DOC!!
BOOL ef_FindPgraphStyleUnusedSpan( const WORD  us_LEN_SPAN, 
									CdRecordCursor  t_cursor, 
									const RtfTrackingInfo *const  pt_CONTEXT, 
									WORD *const  pus_style, 
									BOOL *const  pf_gap)	{
	WORD  us = 0, us_highestStyle = 0;
	BOOL  f_ItemGotLocked;
	ItemInfo * pt_itemLocked = NULL;

	if (!( t_cursor.puc_location && t_cursor.pt_item && 
													t_cursor.us_recLength && 
													pt_CONTEXT && pus_style))
		return FALSE;

	*pus_style = NULL;
	if (pf_gap)
		*pf_gap = FALSE;

	if (!us_LEN_SPAN)
		return TRUE;

	//loop through the rich-text field, record by record
	while (t_cursor.pt_item)	{
		//If this CD record is a paragraph-definition or -reference and the 
		//	associated ID is greater than the highest ID we've encountered so 
		//	far, reset the highest-ID variable. We're checking 
		//	paragraph-reference records as well as -definition records 
		//	because sometimes Notes leaves reference records in that point to 
		//	non-existent definition records.
		if (LOBYTE( SIG_CD_PABDEFINITION) == *t_cursor.puc_location && 
											(us = ((CDPABDEFINITION *) 
											t_cursor.puc_location)->PABID) > 
											us_highestStyle)
			us_highestStyle = us;
		else if (LOBYTE( SIG_CD_PABREFERENCE) == *t_cursor.puc_location && 
											(us = ((CDPABREFERENCE *) 
											t_cursor.puc_location)->PABID) > 
											us_highestStyle)
			us_highestStyle = us;

		//advance the cursor to the next CD record
		AdvanceCdCursor( &t_cursor, pt_CONTEXT, NULL, &f_ItemGotLocked);

		//if we just moved to a follow-on rich-text item and the previous 
		//	item was locked by this procedure, unlock the previous item
		if (pt_itemLocked && pt_itemLocked != t_cursor.pt_item)	{
			UnlockItem( pt_itemLocked);
			pt_itemLocked = NULL;
		}

		//if a follow-on rich-text item was locked by this procedure, flag 
		//	the item for later unlocking
		if (f_ItemGotLocked)
			pt_itemLocked = t_cursor.pt_item;
	} //while (t_cursor.pt_item)

	//if the desired span length cannot be accommodated between the 
	//	highest-possible ID and the highest ID found, we need to look for a 
	//	gap within the ID set where the span can be accommodated (NOT YET
	//	DONE)
	if (us_highestStyle > mus_MAX_STYLE_ID - us_LEN_SPAN)	{
return FALSE;

		*pf_gap = TRUE;
	}else
		*pus_style = us_highestStyle + 1;

	return TRUE;
} //ef_FindPgraphStyleUnusedSpan(


/** ef_RenumberPgraphStylesToSpan( ***


--- parameters & return ----

RETURN: eus_SUCCESS if no error occured; the Notes API error code otherwise

--- suggested enhancement ---
1/16/99 PR
The starting memory-lock state should be restored upon function exit, but no 
good mechanism is yet available with which to accomplish this. See this 
module's suggested-enhancement note for a description of the problem and 
proposed solution.

--- revision history -------
1/19/99 PR: created			*/
//DOC!!
BOOL ef_RenumberPgraphStylesToSpan( 
								const WORD  us_MIN_ID, 
								const WORD  us_MAX_ID, 
								CdRecordCursor  t_cursor, 
								const RtfTrackingInfo *const  pt_CONTEXT)	{
	CdRecordCursor  t_StartOff, t_cursr;
	WORD  us, us_IdToChange;
	CDPABDEFINITION * pt_pab;
	CDPABREFERENCE * pt_pgref;
	CDPABFORMULAREF * pt_phref;
	CDPABHIDE * pt_phide;

	if (!( us_MIN_ID && us_MIN_ID <= us_MAX_ID && t_cursor.puc_location && 
										t_cursor.pt_item && 
										t_cursor.us_recLength && pt_CONTEXT))
		return FALSE;

	//loop through the rich-text content, record by record
	us = us_MIN_ID - 1;
	do	{
		//if the record is a paragraph-style definition and the associated ID 
		//	falls outside the remaining span...
		if (LOBYTE( SIG_CD_PABDEFINITION) == *t_cursor.puc_location)	{
			us_IdToChange = (pt_pab = (CDPABDEFINITION *) 
												t_cursor.puc_location)->PABID;
			if (us_IdToChange < us_MIN_ID || us_IdToChange > us_MAX_ID)	{
				//reset the start-off cursor to the ensuing CD record
				t_StartOff = t_cursor;
				AdvanceCdCursor( &t_StartOff, pt_CONTEXT, NULL, NULL);

				//reset the current ID to the next-available ID in the span
				if (++us > us_MAX_ID)
					return FALSE;
				pt_pab->PABID = us;

				//starting from the start-off cursor, loop through the 
				//	rich-text content, record by record
				t_cursr = t_StartOff;
				while (t_cursr.pt_item)	{
					//If the record is a paragraph-style reference and its ID 
					//	matches the ID we need to change, reset the reference 
					//	equal to the ID we're changing to. (There's no need 
					//	for virtualization here since we're not changing the 
					//	length of the rich-text items at all.)
					if (LOBYTE( SIG_CD_PABREFERENCE) == 
											*t_cursr.puc_location && 
											(pt_pgref = (CDPABREFERENCE *) 
											t_cursr.puc_location)->PABID == 
											us_IdToChange)
						pt_pgref->PABID = us;
					else if (LOBYTE( SIG_CD_PABFORMREF) == 
													*t_cursr.puc_location)	{
						if ((pt_phref = (CDPABFORMULAREF *) 
										t_cursr.puc_location)->DestPABID == 
										us_IdToChange)
							pt_phref->DestPABID = us;
						if (pt_phref->SourcePABID == us_IdToChange)
							pt_phref->SourcePABID = us;
					}else if (LOBYTE( SIG_CD_PABHIDE) == 
											*t_cursr.puc_location && 
											(pt_phide = (CDPABHIDE *) 
											t_cursr.puc_location)->PABID == 
											us_IdToChange)
						pt_phide->PABID = us;

					//advance the operative cursor to next record in the 
					//	rich-text content
					AdvanceCdCursor( &t_cursr, pt_CONTEXT, NULL, NULL);
				} //while (t_cursr.pt_item)
			} //if (us_IdToChange < us_MIN_ID ||
		} //if (LOBYTE( SIG_CD_PABDEFINITION) ==

		//advance to the next record in the rich-text content
		AdvanceCdCursor( &t_cursor, pt_CONTEXT, NULL, NULL);
	} while (t_cursor.pt_item);

	return TRUE;
} //ef_RenumberPgraphStylesToSpan(


/** ef_FreeRtfContext( ***
Purpose is to polish off the structures and allocations made to support 
an instance of rich-text handling here in this module.

--- parameter & return ---
pt_context: Input & Output. The structure managing environment information 
	about an instance of rich-text handling. The ItemInfo lists contained 
	by the structure are freed and cleared by this function.
RETURN: TRUE if procedure was successful; FALSE if the pointer to the 
	rich-text item-tracking context is null and therefore clearly invalid

--- revision history -----
3/20/00 PR: minor documentation adjustment
12/7/98 PR: created		*/
BOOL ef_FreeRtfContext( RtfTrackingInfo *const  pt_context)	{
	if (!pt_context)
		return FALSE;

	//free & clear all information concerning the rich-text items managed 
	//	by the given instance of rich-text handling
	ClearItemList( &pt_context->pt_Actuality);
	ClearItemList( &pt_context->pt_Virtuality);
	ClearItemList( &pt_context->pt_SemiVirtuality);

	//Nullify the container structure itself. Less likely to be reused that 
	//	way.
	memset( pt_context, sizeof( RtfTrackingInfo), NULL);

	return TRUE;
} //ef_FreeRtfContext(


/** e_InitUtilityRtfContext( ***

--- parameter & return ----

--- revision history ------
3/14/03 PR: token renaming
12/8/98 PR: created			*/
//DOC!!
RtfTrackingInfo * ept_InitUtilityRtfContext( 
											RtfTrackingInfo *const  pt_ctx)	{
	if (!pt_ctx)
		return NULL;

	memset( pt_ctx, NULL, sizeof( RtfTrackingInfo));

	return pt_ctx;
} //e_InitUtilityRtfContext(


/** eus_AppendRtParagraph( ***

--- parameters & return ----

RETURN: eus_SUCCESS if no error occured; the Notes API error code otherwise

--- revision history -------
9/3/03 PR: fixed regression error in building template CD records
7/21/03 PR: minor logic adjustment in support of maintaining module consistency
3/14/03 PR: minor logic adjustment in support of signature change to 
	us_VirtualAppend()
3/20/00 PR: minor logic shortening, token renaming
12/8/98 PR: created			*/
//DOC!!
STATUS eus_AppendRtParagraph( const WORD  us_ID_PARAGRAPH_DEF, 
								const COMPOUNDSTYLE *const  pt_PARAGRAPH_DEF, 
								RtfTrackingInfo *const  pt_context, 
								WORD *const  pus_usedParagraphDefId)	{
	const WORD  us_START_CUSTOM_IDS = 0xA000;
	const WSIG  t_PARAGRAPH_STYLE_HEADER = {SIG_CD_PABDEFINITION, 
													sizeof( CDPABDEFINITION)};
	
	const COMPOUNDSTYLE * pt_ParagraphDef = NULL;
	BYTE * puc;
	WORD  us_ParagraphDefId;
	CDPABREFERENCE  t_ParagraphDefinitionReference = {{SIG_CD_PABREFERENCE, 
													sizeof( CDPABREFERENCE)}};
	STATUS  us_err;

	if (!( pt_context && (us_ID_PARAGRAPH_DEF || pt_PARAGRAPH_DEF)))
		return !eus_SUCCESS;

//PGP development short-cut
_ASSERTE( pt_PARAGRAPH_DEF && us_ID_PARAGRAPH_DEF);

	//if applicable, default to null the variable in which the caller wants 
	//	to store the ID of the paragraph-definition record we use
	if (pus_usedParagraphDefId)
		*pus_usedParagraphDefId = NULL;

	//if a paragraph style ID was provided by the caller...
	if (us_ParagraphDefId = us_ID_PARAGRAPH_DEF)	{
		//see if the specified paragraph style record is already 
		//	defined within the rich-text field

		//If the record is not yet defined and if a paragraph-attribute 
		//	structure was provided, set the internal style-structure variable 
		//	to point to the structure provided by the caller. If the caller 
		//	didn't provide a structure, we just won't reference a paragraph 
		//	definition at all.
		if (pt_PARAGRAPH_DEF)
			pt_ParagraphDef = pt_PARAGRAPH_DEF;
	//else just a paragraph-attribute structure was provided, so we need 
	//	to create our own style ID...
	}else	{
		//loop until an acceptable ID is generated...
			//generate a random ID number within the non-custom range
return !eus_SUCCESS;
			//if the style ID is not already taken within virtuality, 
			//	break out of the loop

			//if we've tried this loop ten times already, something's 
			//	gotta be wrong, so return general failure

		//set the internal style-structure variable to point to the structure 
		//	provided by the caller
	} //if (us_ParagraphDefId = us_ID_PARAGRAPH_DEF)

	//if there's any non-virtualized actuality involved here, virtualize it
	if (us_err = us_VirtualizeRestOfRtf( pt_context, NULL))
		return us_err;

	//append next the paragraph record we need
	if (us_err = us_VirtualAppend( (BYTE *) &mt_PARAGRAPH, sizeof( 
										CDPARAGRAPH), pt_context, NULL, NULL))
		return us_err;
/*	if (us_err = us_GetVirtualCdRecAppendPointer( pt_context, 
												sizeof( CDPARAGRAPH), &puc))
		return us_err;

	//write the paragraph record
	memcpy( puc, &mt_PARAGRAPH, sizeof( CDPARAGRAPH));

	//update the length of the item written to
	pt_context->pt_endVirtual->ul_length += 
									pt_context->pt_endVirtual->ul_length % 
									2 + sizeof( CDPARAGRAPH);
*/
	//if we need to create and append a paragraph style structure...
	if (pt_ParagraphDef)	{
		//get a pointer at which the record can be written
		if (us_err = us_GetVirtualCdRecAppendPointer( pt_context, sizeof( 
													CDPABDEFINITION), &puc))
			return us_err;

		//write the header bytes to the full style record, then copy in the 
		//	provided style structure, then write the trailing record bytes
		memcpy( puc, &t_PARAGRAPH_STYLE_HEADER, sizeof( WSIG));
		memcpy( puc += sizeof( WSIG), &us_ParagraphDefId, sizeof( WORD));
		memcpy( puc += sizeof( WORD), pt_ParagraphDef, 
													sizeof( COMPOUNDSTYLE));
		memset( puc + sizeof( COMPOUNDSTYLE), NULL, sizeof( DWORD) + 
															sizeof( WORD));

		//update the length of the item written to
		pt_context->pt_endVirtual->ul_length += 
									pt_context->pt_endVirtual->ul_length % 
									2 + sizeof( CDPABDEFINITION);
	} //if (pt_ParagraphDef)

	//if needed, update the generic paragraph-reference record with our 
	//	favorite style ID, and write the paragraph-definition reference record
	if (us_ParagraphDefId)	{
		//get a pointer at which the record can be written
		if (us_err = us_GetVirtualCdRecAppendPointer( pt_context, sizeof( 
													CDPABREFERENCE), &puc))
			return us_err;

		t_ParagraphDefinitionReference.PABID = us_ParagraphDefId;
		memcpy( puc, &t_ParagraphDefinitionReference, 
													sizeof( CDPABREFERENCE));

		//update the length of the item written to
		pt_context->pt_endVirtual->ul_length += 
									pt_context->pt_endVirtual->ul_length % 
									2 + sizeof( CDPABREFERENCE);
	} //if (us_ParagraphDefId)

	//if applicable, update the variable in which the caller wants to store 
	//	the ID of the paragraph-definition record we used
	if (pus_usedParagraphDefId && us_ParagraphDefId)
		*pus_usedParagraphDefId = us_ParagraphDefId;
	
	return eus_SUCCESS;
} //eus_AppendRtParagraph(


/** eus_AppendAttachmentHotspot( ***


--- parameters & return ----
pc_UniqueNm: Input & Output.

RETURN: eus_SUCCESS if no error occured; the Notes API error code otherwise

--- revision history -------
3/20/00 PR: fixed bug where computed length of records written did not 
	account for the alignment byte needed for CDHOTSPOTBEGIN records of odd 
	length due to its "data" (the file names) having an odd length
1/29/99 PR: created			*/
//DOC!!
STATUS eus_AppendAttachmentHotspot( NOTEHANDLE  h_NOTE, 
									const char  pc_FILENM[], 
									char  pc_UniqueNm[], 
									const BYTE *const  puc_GRAPHIC_CD_RECS, 
									const WORD  us_LEN_GRAPHIC_CD_RECS, 
									RtfTrackingInfo *const pt_context)	{
	CDHOTSPOTBEGIN  t_HotspotBegin = {{SIG_CD_HOTSPOTBEGIN}, 
										HOTSPOTREC_TYPE_FILE, 
										HOTSPOTREC_RUNFLAG_BEGIN | 
										HOTSPOTREC_RUNFLAG_NOBORDER};
	UINT  ui_lenUniqueNm, ui;
	BYTE * puc;
	STATUS  us_err;

	if (!( h_NOTE && pc_UniqueNm && (puc_GRAPHIC_CD_RECS ? 
								us_LEN_GRAPHIC_CD_RECS : TRUE) && pt_context))
		return !eus_SUCCESS;

	//if no unique name has been provided...
		//if a filename has been provided...
			//if the filename is unique within the given note and rich-text 
			//	context...
				//copy the filename into the unique name

	//if a unique name still has not been set, determine a filename unique 
	//	within the given note and rich-text context

//PGP development shortcut
ui_lenUniqueNm = strlen( pc_UniqueNm);
if (pc_FILENM && !ui_lenUniqueNm)	{
  strcpy( pc_UniqueNm, pc_FILENM);
  ui_lenUniqueNm = strlen( pc_UniqueNm);
}

	ui = ui_lenUniqueNm + (pc_FILENM ? strlen( pc_FILENM) : (UINT) NULL) + 2;
	if (ui > (WORD) ui)
		return !eus_SUCCESS;
	t_HotspotBegin.DataLength = (WORD) ui;
	t_HotspotBegin.Header.Length = (WORD) (ui += sizeof( CDHOTSPOTBEGIN));

	//if there's any non-virtualized actuality involved here, virtualize it
	if (us_err = us_VirtualizeRestOfRtf( pt_context, NULL))
		return us_err;

	//get a pointer to where we can append the attachment-hotspot records 
	//	we need -- PGP SHORTCUT: left out graphics stuff!!
	if (us_err = us_GetVirtualCdRecAppendPointer( pt_context, ui += ui % 2 + 
												sizeof( CDHOTSPOTEND), &puc))
		return us_err;

	//write in the attachment hotspot
	memcpy( puc, &t_HotspotBegin, sizeof( CDHOTSPOTBEGIN));
	strcpy( puc += sizeof( CDHOTSPOTBEGIN), pc_UniqueNm);
	puc += ui_lenUniqueNm + 1;
	if (pc_FILENM)	{
		strcpy( puc, pc_FILENM);
		puc += strlen( pc_FILENM);
	}else
		puc[0] = NULL;

	memcpy( ++puc + (DWORD) puc % 2, &mt_HOTSPOT_END, sizeof( CDHOTSPOTEND));

	//lastly, update the length of the item written to
	pt_context->pt_endVirtual->ul_length += 
								pt_context->pt_endVirtual->ul_length % 2 + ui;

	return eus_SUCCESS;
} //eus_AppendAttachmentHotspot(


/** eus_InsertHotspotEnd( ***
Inserts an end-hotspot CD record at the specified location in the given 
rich-text stream.

--- parameter & return ----
t_CRSR: Rich-text cursor telling where the hotspot-end record should be written
pt_ctx: Input & Output. Address of an information structure describing the 
	rich-text context being navigated and recompiled.
RETURN:
	eus_SUCCESS if no errors occurred
	eus_ERR_INVLD_ARG if any passed-in parameter is obviously invalid
	the Notes API error code otherwise

--- revision history ------
3/14/03 PR: minor logic adjustment in support of signature change to 
	us_VirtualizeThruActualLocation()
9/6/02 PR: enhanced procedure to allow insertion anywhere within the given 
	rich-text stream, prompting procedure to be renamed from 
	eus_WriteEndHotspotRecord()
8/22/00 PR: created			*/
STATUS eus_InsertHotspotEnd( const CdRecordCursor  t_CRSR, 
								RtfTrackingInfo *const  pt_ctx)	{
	BYTE * puc;
	STATUS  us_err;

	if (!( pt_ctx && pt_ctx->pt_Virtuality))
		return eus_ERR_INVLD_ARG;

	//virtualize up to where the hotspot-end CD record is to be written
	if (t_CRSR.pt_item)	{
		if (us_err = us_VirtualizeThruActualLocation( t_CRSR.puc_location, 
															pt_ctx, NULL, NULL))
			return us_err;
	}else
		if (us_err = us_VirtualizeRestOfRtf( pt_ctx, NULL))
			return us_err;

	//get a pointer to where we can append an end-hotspot record, then write it 
	//	in
	if (us_err = us_GetVirtualCdRecAppendPointer( pt_ctx, sizeof( 
														CDHOTSPOTEND), &puc))
		return us_err;
	memcpy( puc, &mt_V4HOTSPOT_END, sizeof( CDHOTSPOTEND));

	//update the length of the virtual item written to
	pt_ctx->pt_endVirtual->ul_length += pt_ctx->pt_endVirtual->ul_length % 2 + 
														sizeof( CDHOTSPOTEND);

	return eus_SUCCESS;
} //eus_InsertHotspotEnd(


/** eus_InsertAttachHotspot( ***
Inserts a generic file-attachment hotspot at the specified point in the given 
rich-text field.

--- parameters & return -------
pt_crsr: Optional Input & Output. Address of rich-text cursor giving the 
	insertion point for the hotspot. If null or a null location is given, 
	procedure will append to the end of all existing rich-text context. Unless 
	null, upon successful return cursor will point to the CD record following 
	the hotspot's end record, if any. If null, output will be suppressed.
pc_FILE_NM: Optional if pc_OBJNM provided. Address of the user-friendly 
	("original") filename to be associated with the attachment. If null, 
	procedure will use the provided object name as the filename.
pc_objNm: Either Input or Output. If Input, address of the note's object name 
	for the attachment. If null-string then Output, address of buffer of at 
	least length mi_MAXLEN_ATTCH_OBJNM bytes into which procedure will output 
	the object name chosen for the attachment. In doing this choosing, 
	procedure assumes that the target attachment has not yet been attached to 
	the note and will use the filename as the object name unless that name is 
	being used already within the note, in which case the procedure will come 
	up with a unique object name.
h_NOTE: Required only if pc_OBJNM is null-string. Handle to note to own the 
	attachment to be referenced by the hotspot
puc_GRPHC_RECS: Optional. Address of a buffer holding the graphic CD records to 
	include in the hotspot. If null, no graphic element will be included.
us_LEN_GRPHC_RECS: Required only if puc_GRPHC_RECS input is provided. Length of 
	the buffer holding the graphic CD records to be included.
pc_B4TXT: Optional. Address of the extra string to include before the hotspot. 
	If null, no extra string will be included.
pc_ENDTXT: Optional. Address of the extra string to include at the end of the 
	hotspot. If null, no extra string will be included.
ul_FNT: Optional. The font to be used in formatting any optional text to be 
	included. If null, the default font of the paragraph will be used, 
	implicitly. Ignored if neither pc_B4TXT nor pc_ENDTXT provided.
pt_ctx: Input & Output. Pointer to environment information about the rich-text 
	field being operated on. Its virtuality information may be updated by the 
	procedure.
RETURN:
	eus_SUCCESS if no errors occurred
	eus_ERR_INVLD_ARG if any passed-in parameter is obviously invalid
	the Notes API error code otherwise

--- suggested enhancement ----
9/6/02 PR
+ allow caller to specify separate filename and object name for hotspot
+ get rid of PGP-dev shortcuts

--- revision history ---------
3/14/03 PR
+ signature adjustment in support of Notes-version-specific writing of the 
  special attachment-action hotspots used with PGP-encoded attachments
+ documentation adjustment

9/6/02 PR: created			*/
STATUS eus_InsertAttachHotspot( CdRecordCursor *const  pt_crsr, 
								const char  pc_FILE_NM[], 
								char  pc_objNm[], 
								const NOTEHANDLE  h_NOTE, 
								const BYTE *const  puc_GRPHC_RECS, 
								const WORD  us_LEN_GRPHC_RECS, 
								const char  pc_B4TXT[], 
								const char  pc_ENDTXT[], 
								const FONTID  ul_FNT,   
								CdRecordSpan *const  pt_spn, 
								RtfTrackingInfo *const  pt_ctx)	{
	const char *const  pc_FILENM = pc_FILE_NM && *pc_FILE_NM ? pc_FILE_NM : 
																	pc_objNm;
	const WORD  us_LEN_FILENM = pc_FILENM ? strlen( pc_FILENM) : NULL, 
				us_LEN_B4TXT = pc_B4TXT ? strlen( pc_B4TXT) : NULL, 
				us_LEN_ENDTXT = pc_ENDTXT ? strlen( pc_ENDTXT) : NULL;

	CDTEXT  t_txt;
	CDHOTSPOTBEGIN  t_HotspotBgin = {{SIG_CD_HOTSPOTBEGIN}, 
										HOTSPOTREC_TYPE_FILE, 
										HOTSPOTREC_RUNFLAG_BEGIN | 
										HOTSPOTREC_RUNFLAG_NOBORDER};
	WORD  us_lenObjNm;
	DWORD  ul;
	BYTE * puc;
	STATUS  us_err;

	if (!( pc_objNm && us_LEN_FILENM && (!*pc_objNm ? (const BOOL) 
									h_NOTE : TRUE) && (puc_GRPHC_RECS ? 
									us_LEN_GRPHC_RECS : TRUE) && (pc_B4TXT ? 
									us_LEN_B4TXT : TRUE) && (pc_ENDTXT ? 
									us_LEN_ENDTXT : TRUE) && pt_ctx))
		return eus_ERR_INVLD_ARG;

	if (pt_spn)
		memset( pt_spn, NULL, sizeof( CdRecordSpan));

	//if no object name has been provided...
	if (!*pc_objNm)	{
		//if the provided filename is unique within the given note in terms of 
		//	object names associated with the note and within any virtual 
		//	rich-text in the given rich-text context...
//PGP development shortcut: assuming uniqueness!
		if (TRUE)	{
			//copy the filename as the object name to use here
			strcpy( pc_objNm, pc_FILENM);
			us_lenObjNm = us_LEN_FILENM;
		//else determine an object name unique within the given note and and 
		//	within any virtual rich-text in the given rich-text context
		}else
			;
	}else
		us_lenObjNm = strlen( pc_objNm);

	//compute the length of buffer needed to accomodate what we need to write 
	//	out and set length members of associated CD structures along the way
	ul = us_lenObjNm + us_LEN_FILENM + 2;
	if (ul > (WORD) ul)
		return eus_ERR_INVLD_ARG;
	t_HotspotBgin.DataLength = (WORD) ul;
	ul += sizeof( CDHOTSPOTBEGIN);
	t_HotspotBgin.Header.Length = (WORD) ul;
	ul += ul % 2 + (puc_GRPHC_RECS ? us_LEN_GRPHC_RECS : NULL);
	if (us_LEN_B4TXT)
		ul += ul % 2 + sizeof( CDTEXT) + us_LEN_B4TXT;
	if (us_LEN_ENDTXT)
		ul += ul % 2 + sizeof( CDTEXT) + us_LEN_ENDTXT;
	ul += ul % 2 + sizeof( CDHOTSPOTEND);

	//if an insertion location was given...
	if (pt_crsr && pt_crsr->puc_location)	{
		//virtualize up to the insertion point
		if (pt_crsr->pt_item->i_type == mi_ACTUAL)	{
			if (us_err = us_VirtualizeThruActualLocation( 
									pt_crsr->puc_location, pt_ctx, NULL, NULL))
				return us_err;
		}else
//PGP development shortcut: disallowing virtual insertion points!
return eus_ERR_INVLD_ARG;
	//else virtualize the rest of any in-play actual rich-text context so we're 
	//	ready to append to it
	}else
		if (us_err = us_VirtualizeRestOfRtf( pt_ctx, NULL))
			return us_err;

	//obtain the buffer needed and if caller wants to know, note where we're 
	//	beginning the hotspot
	if (us_err = us_GetVirtualCdRecAppendPointer( pt_ctx, ul, &puc))
		return us_err;
	if (pt_spn)	{
		pt_spn->t_crsrBgin.puc_location = puc;
		pt_spn->t_crsrBgin.us_recLength = t_HotspotBgin.Header.Length;
		pt_spn->t_crsrBgin.pt_item = pt_spn->t_crsrEnd.pt_item = 
														pt_ctx->pt_endVirtual;
	}

	//if we'll be including any before- or end-text, populate the styles 
	//	portion of the text record
	if (us_LEN_B4TXT || us_LEN_ENDTXT)	{
		t_txt.Header.Signature = SIG_CD_TEXT;
		t_txt.FontID = ul_FNT ? ul_FNT : NULLFONTID;
//delete: FontSetColor( DEFAULT_FONT_ID, NOTES_COLOR_BLACK); //
						//	should make adjustable by caller
	}

	//if requested by caller, write out the before-text record
	if (us_LEN_B4TXT)	{
		t_txt.Header.Length = sizeof( CDTEXT) + us_LEN_B4TXT;
		memcpy( puc, &t_txt, sizeof( CDTEXT));
		memcpy( puc += sizeof( CDTEXT), pc_B4TXT, us_LEN_B4TXT); //not 
						//	accounting for linefeed translation
		puc += us_LEN_B4TXT;
		puc += (DWORD) puc % 2;
	}

	//next, populate and write out the begin-hotspot record
	memcpy( puc, &t_HotspotBgin, sizeof( CDHOTSPOTBEGIN));
	strcpy( puc += sizeof( CDHOTSPOTBEGIN), pc_objNm);
	strcpy( puc += us_lenObjNm + 1, pc_FILENM);
	puc += us_LEN_FILENM + 1;

	//append the generic bitmap records
	memcpy( puc += (DWORD) puc % 2, puc_GRPHC_RECS, us_LEN_GRPHC_RECS);
	puc += us_LEN_GRPHC_RECS;

	//if requested by caller, construct and append the end-text record
	if (us_LEN_ENDTXT)	{
		t_txt.Header.Length = sizeof( CDTEXT) + us_LEN_ENDTXT;
		memcpy( puc += (DWORD) puc % 2, &t_txt, sizeof( CDTEXT));
		memcpy( puc += sizeof( CDTEXT), pc_ENDTXT, us_LEN_ENDTXT); //not 
						//	accounting for linefeed translation
		puc += us_LEN_ENDTXT;
	}

	//append the end-hotspot record and if caller wants to know, note where 
	//	we're ending the hotspot
	memcpy( puc += (DWORD) puc % 2, &mt_HOTSPOT_END, sizeof( CDHOTSPOTEND));
	if (pt_spn)	{
		pt_spn->t_crsrEnd.puc_location = puc;
		pt_spn->t_crsrEnd.us_recLength = sizeof( CDHOTSPOTEND);
	}

	//update the length of the item written to
	pt_ctx->pt_endVirtual->ul_length += pt_ctx->pt_endVirtual->ul_length % 2 + 
																			ul;

	//if caller's interested, output a cursor pointing to the record 
	//	immediately following the hotspot just written
	if (pt_crsr)	{
		if (pt_spn)
			*pt_crsr = pt_spn->t_crsrEnd;
		else	{
			pt_crsr->puc_location = puc;
			pt_crsr->us_recLength = sizeof( CDHOTSPOTEND);
			pt_crsr->pt_item = pt_ctx->pt_endVirtual;
		} //if (pt_spn)
		AdvanceCdCursor( pt_crsr, pt_ctx->pt_Actuality ? pt_ctx : NULL, NULL, 
																		NULL);
	} //if (pt_crsr)

	return eus_SUCCESS;
} //eus_InsertAttachHotspot(


/** f_ConstructStartCursor( ***
Populate a rich-text cursor positioned at the start of a rich-text context or 
particular item.

--- parameters & return ----
pt_CTX: Optional, required if pt_item input not provided. Address of 
	information structure describing the rich-text environment involved and 
	through which the cursor position will be determined. Information referred 
	to by this structure may be adjusted as a result of the call. If null, 
	input is ignored.
pt_item: Optional. Address of information describing the particular rich-text 
	item against which the cursor should be constructed. Ignored if pt_CTX 
	input provided.
pt_crsr: Output. Address of cursor to be set by procedure. If pt_CTX input 
	provided, will be set to the regular start of the rich-text content, 
	virtuality preceding actuality.
pf_itmGotLckd: Optional Output. Address of flag variable to receive whether the 
	contents of the rich-text item encompassing the position marked by pt_crsr 
	output. If null, output is suppressed.
RETURN: TRUE if a valid cursor could be set; FALSE otherwise

--- revision history -------
3/14/03 PR: completed documentation, token renaming, listing format adjustment
3/20/00 PR: logic shortening by leveraging the new UnlockItem() procedure
1/16/99 PR: created			*/
static BOOL f_ConstructStartCursor( const RtfTrackingInfo *const  pt_CTX, 
									ItemInfo *const  pt_item, 
									CdRecordCursor *const  pt_crsr, 
									BOOL *const  pf_itmGotLckd)	{
	const BOOL  f_SEMI_VRTL = pt_CTX ? (!pt_CTX->pt_Virtuality && 
													pt_CTX->pt_SemiVirtuality ? 
													TRUE : FALSE) : FALSE;

	ItemInfo * pt_itm;
	DWORD  ul;
	BOOL  f_itmGotLckd = FALSE;
	CdRecordCursor  t;

	_ASSERTE( (pt_CTX || pt_item) && pt_crsr);

	memset( pt_crsr, (BYTE) NULL, sizeof( CdRecordCursor));
	if (pf_itmGotLckd)
		*pf_itmGotLckd = FALSE;
	
	//determine the rich-text item against which the cursor will be constructed
	pt_itm = pt_CTX ? (pt_CTX->pt_Virtuality ? pt_CTX->pt_Virtuality : 
									(f_SEMI_VRTL ? pt_CTX->pt_SemiVirtuality : 
									pt_CTX->pt_Actuality)) : pt_item;

	//if the item is too short, short-circuit with failure
	if (pt_itm->ul_length < (ul = !f_SEMI_VRTL ? sizeof( WORD) : NULL))
		return FALSE;

	//if necessary, move to an item that actually has content
	while (pt_itm->ul_length == ul && (pt_itm = pt_itm->pt_next));
	if (!pt_itm)
		return FALSE;

	//if necessary, lock the item's content in memory to make it accessible
	if (!pt_itm->puc_start)	{
		pt_itm->puc_start = OSLockBlock( BYTE, pt_itm->bid_contents);
		f_itmGotLckd = TRUE;
	}

	//populate the cursor
	t.puc_location = pt_itm->puc_start + (!f_SEMI_VRTL ? sizeof( WORD) : NULL);
	if (!(t.us_recLength = us_getCdRecLength( t.puc_location)))
		goto errJump;
	t.pt_item = pt_itm;

	//if caller's interested, tell whether the contents of the rich-text item 
	//	encompassing the cursor got locked in memory as a result of this call
	*pt_crsr = t;
	if (pf_itmGotLckd)
		*pf_itmGotLckd = f_itmGotLckd;

	return TRUE;

errJump:
	if (f_itmGotLckd)
		UnlockItem( pt_itm);

	return FALSE;
} //f_ConstructStartCursor(


/** eus_ConstructRtCursor( ***
Populate a rich-text cursor positioned at the start of a rich-text context or 
particular item.

--- parameters & return ----
pt_CTX: Address of information structure describing the rich-text environment 
	involved and through which the cursor position will be determined. 
	Information referred to by this structure may be adjusted as a result of 
	the call. If null, input is ignored.
pt_crsr: Address of cursor to be set by procedure. Will be set to the regular 
	start of the rich-text content, virtuality preceding actuality.
RETURN:
	eus_ERR_INVLD_ARG if procedure's input context is obviously invalid
	ERR_ITEM_NOT_FOUND if context provided yielded no content against which a 
		rich-text cursor could be constructed
	eus_SUCCESS if cursor successfully constructed

--- revision history -------
3/14/03 PR: created			*/
STATUS eus_ConstructRtCursor( const RtfTrackingInfo *const  pt_CTX, 
								CdRecordCursor *const  pt_crsr)	{
	if (!( pt_CTX && pt_crsr))
		return eus_ERR_INVLD_ARG;

	return f_ConstructStartCursor( pt_CTX, NULL, pt_crsr, NULL) ? eus_SUCCESS : 
															ERR_ITEM_NOT_FOUND;
} //eus_ConstructRtCursor(


/** eus_AppendItemsToRtf( ***
Appends the virtual items tracked by one rich-text context to another rich-text 
context.

--- parameters & return ---
pt_APPND: address of the rich-text context whose virtuality is to be appended 
	to the target context
pt_mainCtx: Input & Output. Address of the rich-text context to be appended to.
	eus_ERR_INVLD_ARG if procedure's input context is obviously invalid
	eus_SUCCESS if successful
	the Notes API error code otherwise

--- revision history ------
3/14/03 PR
+ minor exception-handling improvement
+ minor documentation adjustment, listing format adjustment, token renaming

3/20/00 PR
+ minor signature adjustment for constness
+ completed standard documentation

12/8/98 PR: created			*/
STATUS eus_AppendItemsToRtf( const RtfTrackingInfo *const  pt_APPND, 
								RtfTrackingInfo *const  pt_mainCtx)	{
	ItemInfo * pt_itm;
	STATUS  us_err;

	if (!( pt_APPND && pt_APPND->pt_Virtuality && pt_mainCtx))
		return eus_ERR_INVLD_ARG;

	if (us_err = us_VirtualizeRestOfRtf( pt_mainCtx, NULL))
		return us_err;

	if (pt_mainCtx->pt_endVirtual)	{
		pt_itm = pt_mainCtx->pt_endVirtual;
		pt_itm->pt_next = pt_APPND->pt_Virtuality;
	}else
		pt_itm = pt_mainCtx->pt_Virtuality = pt_APPND->pt_Virtuality;

	while (pt_itm->pt_next)
		pt_itm = pt_itm->pt_next;

	pt_mainCtx->pt_endVirtual = pt_itm;

	return eus_SUCCESS;
} //eus_AppendItemsToRtf(


/** eus_ReplaceRtf( ***
Sets up a target rich-text context to be replaced by another context of 
rich-text content.

--- parameters & return -----
pt_dest: Input & Output. Address of environment information about the rich-text 
	field to be replaced. Its virtuality information will be updated by the 
	procedure.
pt_src: Input & Output. Address of environment information about the 
	replacement rich-text field. Ownership of ts virtuality will be transferred 
	to the pt_dest rich-text context. As a result, pt_src's virtuality 
	information will be updated by the procedure.
RETURN:
	eus_SUCCESS if no errors occurred
	eus_ERR_INVLD_ARG if any passed-in parameter is obviously invalid
	the Notes API error code otherwise

--- revision history --------
3/14/03 PR: created			*/
STATUS eus_ReplaceRtf( RtfTrackingInfo *const  pt_dest, 
						RtfTrackingInfo *const  pt_src)	{
	STATUS  us_err;

	if (!( pt_dest && pt_src))
		return eus_ERR_INVLD_ARG;

	//ensure the replacement content is fully virtualized
	if (us_err = us_VirtualizeRestOfRtf( pt_src, NULL))
		return us_err;

	//get rid of any virtuality obtaining in the detination context
	if (pt_dest->pt_Virtuality)
		FreeItemList( &pt_dest->pt_Virtuality);

	//transfer ownership of the replacement virtuality to the destination 
	//	context
	pt_dest->pt_Virtuality = pt_src->pt_Virtuality;
	pt_dest->pt_endVirtual = pt_src->pt_endVirtual;
	pt_dest->ul_ActualFrontier = mul_FRONTIER_NO_MORE;
	pt_src->pt_Virtuality = pt_src->pt_endVirtual = NULL;
	pt_src->ul_ActualFrontier = sizeof( WORD);

	return eus_SUCCESS;
} //eus_ReplaceRtf(


/** eus_AppendCdsToRtf( ***
Appends the given CD-record stream to the specified rich-text context.

--- parameters & return ----
PUC: address of the stream of CD records to append
UL: length of the stream
pt_ctx: Input & Output. Address of structure describing the the rich-text 
	environment we're operating with. Structure may undergo adjustment as a 
	result of the call.
RETURN:
	eus_ERR_INVLD_ARG if procedure's input context is obviously invalid
	eus_SUCCESS if successful
	the Notes API error code otherwise

--- revision history -------
3/14/03 PR: created			*/
STATUS eus_AppendCdsToRtf( const BYTE *const  PUC, 
							const ULONG  UL, 
							RtfTrackingInfo *const  pt_ctx)	{
	STATUS  us_err;

	if (!( PUC && UL && pt_ctx))
		return eus_ERR_INVLD_ARG;

	if (us_err = us_VirtualizeRestOfRtf( pt_ctx, NULL))
		return us_err;

	return us_VirtualAppend( PUC, UL, pt_ctx, NULL, NULL);
} //eus_AppendCdsToRtf(


/** ef_UnappendRtfItems( ***
Undo a prior item-appendage (likely via eus_AppendItemsToRtf()) made to a 
particular rich-text context.

--- parameters & return ----
pt_APPENDED: address of the rich-text context describing the virtual-item 
	stream already incorporated into the other specified rich-text context
pt_mainContext: Input & Output. Address of the rich-text context that 
	received the specified appendage. The context will be adjusted such that 
	the appendage is disincorporated.
RETURN: TRUE if successful; FALSE otherwise

--- revision history -------
3/20/00 PR: created			*/
BOOL ef_UnappendRtfItems( const RtfTrackingInfo *const  pt_APPENDED, 
							RtfTrackingInfo *const  pt_mainContext)	{
	BLOCKID  bid;
	ItemInfo * pt, * pt_itm;

	if (!( pt_APPENDED && pt_APPENDED->pt_Virtuality && pt_mainContext && 
							(pt = pt_itm = pt_mainContext->pt_Virtuality)))
		return FALSE;

	//if the point at which the appendage occurred cannot be located, 
	//	short-circuit with failure
	bid = pt_APPENDED->pt_Virtuality->bid_contents;
	do
		if (memcmp( &pt_itm->bid_contents, &bid, sizeof( BLOCKID)) == ei_SAME)
			break;
	while (pt_itm = pt_itm->pt_next);
	if (!pt_itm)
		return FALSE;

	//excise the appendage from the virtual-item stream by updating 
	//	associated pointers
	do
		if (pt->pt_next = pt_itm)
			break;
	while (pt = pt->pt_next);
	if (pt)
		pt->pt_next = pt_mainContext->pt_endVirtual = NULL;
	else
		pt_mainContext->pt_endVirtual = pt_mainContext->pt_Virtuality = NULL;

	return TRUE;
} //ef_UnappendRtfItems(


/** eus_RemoveAttachWrap( ***
Remove the specified attachment hotspot.

--- parameters & return ------
pt_spn: Input & Output. Address of the span structure marking the attachment 
	hotspot to be removed. Structure will be updated to mark the CD record 
	following the beginning and end of the hotspot being removed.
pt_ctx: Input & Output. Address of structure describing the the rich-text 
	environment we're operating with. Structure may undergo adjustment as a 
	result of the call. 
RETURN:
	eus_ERR_INVLD_ARG if procedure's input context is obviously invalid
	eus_SUCCESS if successful
	the Notes API error code otherwise

--- suggested enhancement ----
7/21/03 PR: probably needs updating to accommodate a possible 
	SIG_CD_BEGIN/-END wrapping of an actual, immediately encapsulated 
	attachment hotspot

--- revision history ---------
3/14/03 PR: created			*/
STATUS eus_RemoveAttachWrap( CdRecordSpan *const  pt_spn, 
								RtfTrackingInfo *const  pt_ctx)	{
	CdRecordCursor * pt_crsr;
	STATUS  us_err;

	if (!( pt_spn && (pt_crsr = &pt_spn->t_crsrBgin)->puc_location && 
						*pt_crsr->puc_location == muc_SIG_CDHOTSPOTBEGIN && 
						!pt_spn->us_offstBgin && 
						pt_spn->t_crsrEnd.puc_location && 
						*pt_spn->t_crsrEnd.puc_location == 
						muc_SIG_CDHOTSPOTEND && !pt_spn->us_offstEnd && pt_ctx))
		return eus_ERR_INVLD_ARG;

	//if the beginning of the hotspot is in actuality...
	if (pt_crsr->pt_item->i_type == mi_ACTUAL)	{
		//virtualize up to just before the hotspot-begin record
		if (us_err = us_VirtualizeThruActualLocation( pt_crsr->puc_location, 
															pt_ctx, NULL, NULL))
			return us_err;

		//virtualize from record just past the hotspot-begin record up to just 
		//	before the hotspot-end record
		AdvanceCdCursor( pt_crsr, pt_ctx, NULL, NULL);
		if (!pt_crsr->pt_item)
			return !eus_SUCCESS;
		pt_ctx->ul_ActualFrontier = ul_GetAbsoluteOffset( 
									pt_crsr->puc_location, pt_crsr->pt_item, 
									pt_ctx->pt_Actuality);
		if (us_err = us_VirtualizeThruActualLocation( (pt_crsr = 
											&pt_spn->t_crsrEnd)->puc_location, 
											pt_ctx, NULL, NULL))
			return us_err;

		//set the actual frontier at just past the end-hotspot CD record 
		//	(remember, virtuality flows into actuality, meaning virtuality 
		//	precedes actuality)
		AdvanceCdCursor( pt_crsr, pt_ctx, NULL, NULL);
		pt_ctx->ul_ActualFrontier = pt_crsr->pt_item ? ul_GetAbsoluteOffset( 
								pt_crsr->puc_location, pt_crsr->pt_item, 
								pt_ctx->pt_Actuality) : mul_FRONTIER_NO_MORE;
	//else virtuality makes things complicated...
	}else
//PGP dev shortcut: not handling the case of the hotspot involving virtuality
return eus_ERR_INVLD_ARG;

	return eus_SUCCESS;
} //eus_RemoveAttachWrap(


/** eus_CollapseToEndAttachWrap( ***
Collapse the specified attachment hotspot by moving its hotspot-begin CD record 
to just before its associated hotspot-end CD record. This makes the hotspot 
impossible for the user to click and makes the attachment "hidden" on the 
document. (Removing the hotspot would cause it to appear "below-the-line" on 
the document.)

--- parameters & return ------
pt_spn: Input & Output. Address of information structure describing the span 
	marking the attachment hotspot to be collapsed. The information structure 
	will be updated so the span begins at the CD record following the original 
	hotspot beginning.
pt_ctx: Input & Output. Address of information describing the the rich-text 
	environment we're operating with. Structure may undergo adjustment as a 
	result of the call. 
RETURN:
	eus_ERR_INVLD_ARG if procedure's input context is obviously invalid
	eus_SUCCESS if successful
	the Notes API error code otherwise

--- suggested enhancement ----
7/21/03 PR: genericize this procedure to operate on any sort of hotspot
3/14/03 PR: get rid of PGP-dev shortcut

--- revision history ---------
7/21/03 PR
+ logic extension to accommodate a possible SIG_CD_BEGIN/-END wrapping of the 
  actual, immediately encapsulated attachment hotspot
+ participated in global token renaming

3/14/03 PR: created			*/
STATUS eus_CollapseToEndAttachWrap( CdRecordSpan *const  pt_spn, 
									   RtfTrackingInfo *const  pt_ctx)	{
	CdRecordCursor * pt_crsr, t;
	BYTE  uc;
	STATUS  us_err;

	if (!( pt_spn && (pt_crsr = &pt_spn->t_crsrBgin)->puc_location && 
					((uc = *pt_crsr->puc_location) == muc_SIG_CDHOTSPOTBEGIN || 
					uc == muc_SIG_CDBEGIN) && !pt_spn->us_offstBgin && 
					pt_spn->t_crsrEnd.puc_location && 
					*pt_spn->t_crsrEnd.puc_location == muc_SIG_CDHOTSPOTEND && 
					!pt_spn->us_offstEnd && pt_ctx))
		return eus_ERR_INVLD_ARG;

	//if we're dealing with a SIG_CD_BEGIN/-END wrapping of the actual 
	//	attachment hotspot, ensure that the attachment-hotspot begin record 
	//	immediately follows the begin record of the wrapper
	t.pt_item = NULL;
	if (uc == muc_SIG_CDBEGIN)	{
		t = *pt_crsr;
		AdvanceCdCursor( &t, pt_ctx, NULL, NULL);
		if (!t.pt_item)
			return !eus_SUCCESS;
		if (muc_SIG_CDHOTSPOTBEGIN != *t.puc_location)
			return eus_ERR_INVLD_ARG;
	} //if (uc == muc_SIG_CDBEGIN)

	//if the beginning of the hotspot is in actuality...
	if (pt_crsr->pt_item->i_type == mi_ACTUAL)	{
		DWORD  ul, ul_;
		BYTE * puc;

		//virtualize up to just before the hotspot-begin record
		if (us_err = us_VirtualizeThruActualLocation( pt_crsr->puc_location, 
															pt_ctx, NULL, NULL))
			return us_err;

		//virtualize from record just past the hotspot-begin record up to just 
		//	before the hotspot-end record
		if (!t.pt_item)
			t = *pt_crsr;
		AdvanceCdCursor( &t, pt_ctx, NULL, NULL);
		if (!t.pt_item)
			return !eus_SUCCESS;
		ul = pt_ctx->ul_ActualFrontier;
		puc = t.puc_location;
		pt_ctx->ul_ActualFrontier = ul_GetAbsoluteOffset( puc, t.pt_item, 
														pt_ctx->pt_Actuality);
		if (us_err = us_VirtualizeThruActualLocation( 
											pt_spn->t_crsrEnd.puc_location, 
											pt_ctx, NULL, &t))
			return us_err;

		//Lastly, virtualize in the hotspot-begin record(s), and reset so that 
		//	the hotspot-end record(s) will be the next to go.
		ul_ = pt_ctx->ul_ActualFrontier;
		pt_ctx->ul_ActualFrontier = ul;
		if (us_err = us_VirtualizeThruActualLocation( puc, pt_ctx, NULL, NULL))
			return us_err;
		pt_ctx->ul_ActualFrontier = ul_;

		//for the purpose of output, reset the begin cursor to point to the 
		//	now-virtualized CD record which followed the original hotspot-begin 
		//	record
		*pt_crsr = t;
	//else virtuality makes things complicated...
	}else
//PGP dev shortcut: not handling the case of the hotspot involving virtuality
return eus_ERR_INVLD_ARG;

	return eus_SUCCESS;
} //eus_CollapseToEndAttachWrap(


/** eus_InsertSubformTop( ***
Inserts the specified subform at the beginning of the provided rich-text 
context, _by reference_.

--- parameters & return ----
pt_ctx:					context should have no virtuality. All will be 
	unlocked at end!
pc_SUBFORMNM: address of the name of the subform to be inserted
RETURN:
	eus_SUCCESS if no errors occurred
	eus_ERR_INVLD_ARG if any passed-in parameter is obviously invalid
	the Notes API error code otherwise

--- revision history -------
9/3/03 PR: fixed regression error in building template CD records
3/14/03 PR: minor logic adjustments in support of signature change to 
	us_VirtualAppend(), us_VirtualizeThruActualLocation()
9/6/02 PR: listing format adjustment, minor exception-handling adjustment, 
	token renaming
3/20/00 PR: Redeveloped so the subform is inserted by reference, without need 
	of duplicating the subform rich-text format within the target form. Caused 
	signature change.
1/16/99 PR: created			*/
//DOC!!
STATUS eus_InsertSubformTop( RtfTrackingInfo *const  pt_ctx, 
								const char  pc_SUBFORMNM[])	{
	const CDTEXT  t_BLANK = {{SIG_CD_TEXT, sizeof( CDTEXT)}, DEFAULT_FONT_ID};
	//Notes includes the null terminator when writing the subform name with 
	//	the begin-hotspot record, so we do too. BTW, without the null 
	//	terminator, Notes doesn't open the form properly.
	const WORD  us_LEN_SUBFORMNM = (WORD) (pc_SUBFORMNM ? strlen( 
													pc_SUBFORMNM) + 1 : NULL), 
				us_LEN_HOTSPOT_BEGIN = sizeof( CDHOTSPOTBEGIN) + 
															us_LEN_SUBFORMNM;
	//I don't know what the 0x10000000 flag is, but Notes includes it with all 
	//	subforms, so we do too.
	const CDHOTSPOTBEGIN  t_HOTSPOT_BEGIN = {{SIG_CD_V4HOTSPOTBEGIN, 
												us_LEN_HOTSPOT_BEGIN}, 
												HOTSPOTREC_TYPE_SUBFORM, 
												HOTSPOTREC_RUNFLAG_BEGIN | 
												HOTSPOTREC_RUNFLAG_NOBORDER | 
												0x10000000, us_LEN_SUBFORMNM};

	CdRecordCursor  t_crsr, t;
	BOOL  f_startedUtilityRtfContext = FALSE, f_fail;
	BYTE * puc_pab, * puc;
	CdRecordSpan  t_span;
	RtfTrackingInfo  t_setOffTextStructure;
	DWORD  ul;
	ItemInfo * pt;
	STATUS  us_err = eus_SUCCESS;

	if (!( pt_ctx && !pt_ctx->pt_Virtuality && us_LEN_SUBFORMNM))
		return eus_ERR_INVLD_ARG;

	//construct a cursor pointing to the start of the form
	if (!f_ConstructStartCursor( pt_ctx, NULL, &t_crsr, NULL))
		return !eus_SUCCESS;

	//locate the first paragraph-style-reference CD record in the form
	if (f_fail = !f_cursorToRecord( SIG_CD_PABREFERENCE, &t_crsr, NULL, FALSE, 
																		NULL))
		goto errJump;
	puc_pab = t_crsr.puc_location;

	//locate the first following text span in the form
	if (f_fail = !f_cursorToText( &t_crsr, NULL, TRUE, &t_span, NULL, NULL, 
																		NULL))
		goto errJump;

	//virtualize up to the first text structure in the form
	if (us_err = us_VirtualizeThruActualLocation( (t = 
											t_span.t_crsrBgin).puc_location, 
											pt_ctx, NULL, NULL))
		goto errJump;

	//Virtualize into a utility virtuality up to the first CDTEXT record in the 
	//	form. We will use this virtuality for making copies of a blank text 
	//	structure in the format currently being used within the form.
	if (*t.puc_location != muc_SIG_CDTEXT)	{
		CdRecordSpan  t_s;

		if (( f_fail = !f_cursorToText( &t, NULL, TRUE, &t_s, NULL, NULL, 
														NULL)) || !t.pt_item)
			goto errJump;
		_ASSERTE( *t_s.t_crsrBgin.puc_location == muc_SIG_CDTEXT);
		t = t_s.t_crsrBgin;
	}
	f_startedUtilityRtfContext = TRUE;
	if (us_err = us_VirtualizeThruActualLocation( t.puc_location, pt_ctx, 
												ept_InitUtilityRtfContext( 
												&t_setOffTextStructure), NULL))
		goto errJump;

	//append to the utility virtuality a copy of the rest of the text span in 
	//	the form, but with _no_ content, "blank"
	if (us_err = us_GetVirtualCdRecAppendPointer( &t_setOffTextStructure, 
														sizeof( CDTEXT), &puc))
		goto errJump;
	memcpy( puc, t.puc_location, sizeof( CDTEXT));
	((CDTEXT *) puc)->Header.Length = sizeof( CDTEXT);
	t_setOffTextStructure.pt_endVirtual->ul_length += 
							t_setOffTextStructure.pt_endVirtual->ul_length % 
							2 + sizeof( CDTEXT);
	AdvanceCdCursor( &t, NULL, NULL, NULL);
	ul = ul_GetAbsoluteOffset( t.puc_location, t.pt_item, pt_ctx->pt_Actuality);
	_ASSERTE( t_span.t_crsrEnd.pt_item ? ul != eul_ERR_FAILURE : TRUE);
	pt_ctx->ul_ActualFrontier = ul == eul_ERR_FAILURE ? mul_FRONTIER_NO_MORE : 
																			ul;
	if (t_span.t_crsrEnd.pt_item)	{
		if (t_crsr.pt_item)	{
			if (us_err = us_VirtualizeThruActualLocation( 
												t_crsr.puc_location, pt_ctx, 
												&t_setOffTextStructure, NULL))
				goto errJump;
		}else
			if (us_err = us_VirtualizeRestOfRtf( pt_ctx, 
														&t_setOffTextStructure))
				goto errJump;
	} //if (t_span.t_crsrEnd.pt_item)

	//the copy of the blank set-off text structure complete, reset the actual 
	//	frontier of the form's rich-text context back to the beginning of the 
	//	text structure upon which the copy was based
	pt_ctx->ul_ActualFrontier = ul_GetAbsoluteOffset( 
											t_span.t_crsrBgin.puc_location, 
											t_span.t_crsrBgin.pt_item, 
											pt_ctx->pt_Actuality);

	//include next a copy of the now-constructed, blank set-off text structure
	pt = t_setOffTextStructure.pt_Virtuality;
	do	{
		if (!pt->puc_start)
			pt->puc_start = OSLockBlock( BYTE, pt->bid_contents);
		if (us_err = us_VirtualAppend( pt->puc_start + sizeof( WORD), 
													pt->ul_length - sizeof( 
													WORD), pt_ctx, NULL, NULL))
			goto errJump;
	} while (pt = pt->pt_next);

	//follow that with the full R5 subform-hotspot-reference CD-record stream 
	//	(the R5 extensions will be ignored by pre-R5 clients)
	_ASSERTE( !( sizeof( CDBEGINRECORD) % 2 || sizeof( CDHOTSPOTEND) % 2 || 
													sizeof( CDENDRECORD) % 2));
	if (us_err = us_GetVirtualCdRecAppendPointer( pt_ctx, 
												ul = sizeof( CDBEGINRECORD) + 
												us_LEN_HOTSPOT_BEGIN + 
												us_LEN_HOTSPOT_BEGIN % 2 + 
												sizeof( CDHOTSPOTEND) + 
												sizeof( CDENDRECORD), &puc))
		goto errJump;
	memcpy( puc, &mt_R5_BEGIN_HOTSPOT, sizeof( CDBEGINRECORD));
	memcpy( puc += sizeof( CDBEGINRECORD), &t_HOTSPOT_BEGIN, 
													sizeof( CDHOTSPOTBEGIN));
	memcpy( puc += sizeof( CDHOTSPOTBEGIN), pc_SUBFORMNM, us_LEN_SUBFORMNM);
	memcpy( puc += us_LEN_SUBFORMNM + us_LEN_SUBFORMNM % 2, &mt_V4HOTSPOT_END, 
														sizeof( CDHOTSPOTEND));
	memcpy( puc + sizeof( CDHOTSPOTEND), &mt_R5_END_HOTSPOT, 
														sizeof( CDENDRECORD));
	pt_ctx->pt_endVirtual->ul_length += pt_ctx->pt_endVirtual->ul_length % 
																	2 + ul;

	//follow next with a copy of the blank set-off text structure
	pt = t_setOffTextStructure.pt_Virtuality;
	do	{
		if (!pt->puc_start)
			pt->puc_start = OSLockBlock( BYTE, pt->bid_contents);
		if (us_err = us_VirtualAppend( pt->puc_start + sizeof( WORD), 
													pt->ul_length - sizeof( 
													WORD), pt_ctx, NULL, NULL))
			goto errJump;
	} while (pt = pt->pt_next);

	//And follow that with a "set-off" paragraph CD record followed by a copy 
	//	of the paragraph-style-reference CD record first found in the form. 
	//	This is actually an improvement on the normal insertion behavior of 
	//	Designer (or at least 5.0.2b). Designer will place after the set-off 
	//	paragraph record a copy of the last paragraph-style CD-record stream 
	//	found _in the subform_; meaning that if, for instance, the last 
	//	paragraph in the subform is hidden while the content at the insertion 
	//	point in the form is _not_ hidden, that content will be made hidden 
	//	(illogically) by the insertion of the subform. So our way is better 
	//	(keep the original formatting), not to mention easier.
	_ASSERTE( !( sizeof( CDPARAGRAPH) % 2 || sizeof( CDPABREFERENCE) % 2));
	if (us_err = us_GetVirtualCdRecAppendPointer( pt_ctx, ul = sizeof( 
														CDPARAGRAPH) + sizeof( 
														CDPABREFERENCE), &puc))
		goto errJump;
	memcpy( puc, &mt_PARAGRAPH, sizeof( CDPARAGRAPH));
	memcpy( puc + sizeof( CDPARAGRAPH), puc_pab, sizeof( CDPABREFERENCE));
	pt_ctx->pt_endVirtual->ul_length += pt_ctx->pt_endVirtual->ul_length % 2 + 
																			ul;

errJump:
	//free resources allocated by this procedure
	if (f_startedUtilityRtfContext)
		ef_FreeRtfContext( &t_setOffTextStructure);
	UnlockItems( pt_ctx->pt_Actuality, NULL);
	UnlockItems( pt_ctx->pt_Virtuality, NULL);

	return us_err + f_fail;
} //eus_InsertSubformTop(


/** f_cursorToText( ***
Moves provided rich-text cursor to the next instance of a regular text CD 
structure within the provided rich-text context. Accommodates the R5 text 
multi-CD-record structure as well as the regular single-CD-record structure 
used prior to R5.

--- parameters & return ----
pt_crsr: Input & Output. Address of the rich-text cursor positioned to the 
	point at which the search for the specified CD record should commence. This 
	cursor is then used when advancing through CD records. If the procedure is 
	successful, the cursor will finish at the CD record _following_ the last 
	record involved in the next instance of a text CD structure. If the cursor 
	exhausts the available rich-text context in doing this, its "item" member 
	will be nullified to indicate this.
pt_CTX: Optional. Address of an information structure describing the rich-text 
	context being navigated. If passed in null, the rich-text cursor being used 
	will be unable to jump from an actual rich-text context to a virtual 
	rich-text context.
f_BUMP_FWD_1ST: Flag telling whether the passed-in starting cursor should be 
	moved to the next CD record before initiating the search for text. Usually 
	when starting at the beginning of the rich-text field, the flag will be set 
	to FALSE so that the first CD record may be checked as to whether it's a 
	file-attachment hotspot. Ensuing calls might then set the flag to TRUE to 
	move to the next CD record since the current CD record is already known.
pt_spn: Optional Output. Address of the structure to fill with information about 
	the next instance of regular text discovered. If it's an R5 multi-CD-record 
	structure that's discovered, the begin-cursor member will point to the R5 
	"Begin" record and the end-cursor member will point to the corresponding 
	"End" record. If a pre-R5 single-CD-record structure is discovered, the 
	begin cursor will point directly to the main CDTEXT structure, and the end 
	cursor's item member will be null. If the R5 multi-record structure is 
	returned but the caller wishes to consider only the core CDTEXT, the caller 
	may simply call the procedure a second time using the begin cursor as the 
	starting point and specifying that the search should begin with the CD 
	record following the "Begin" record the cursor lies upon. If paramater is 
	null, the procedure will ignore this descriptive functionality, although 
	the main cursor will be positioned as described above.
pf_lckd: Optional Output. Address of the flag in which to store whether the 
	rich-text item in which the main cursor finds itself was newly locked in 
	the course of this procedure. If passed in null, the procedure will ignore 
	this functionality.
pf_lckdBegin: Optional Output. Address of the flag in which to store whether 
	the rich-text item in which the begin-cursor member of a returned span 
	structure finds itself was newly locked in the course of this procedure. If 
	passed in null or if the span-structure parameter is null, the procedure 
	will ignore this functionality.
pf_lckdEnd: Optional Output. Address of the flag in which to store whether 
	the rich-text item in which the end-cursor member of a returned span 
	structure finds itself was newly locked in the course of this procedure. If 
	a single-CD-record structure is what is discovered, this output will of 
	course be FALSE. If passed in null or if the span-structure parameter is 
	null, the procedure will ignore this functionality.
RETURN: TRUE if an ensuing text structure was found; FALSE if not

--- revision history -------
7/21/03 PR: token renaming

3/14/03 PR
+ minor logic shortening to remove an unnecessary safety measure
+ minor documentation adjustment, listing format adjustment, token renaming

3/20/00 PR: created			*/
static BOOL f_cursorToText( CdRecordCursor *const  pt_crsr, 
							const RtfTrackingInfo *const  pt_CTX, 
							const BOOL  f_BUMP_FWD_1ST, 
							CdRecordSpan *const  pt_spn, 
							BOOL *const  pf_lckd, 
							BOOL *const  pf_lckdBegin, 
							BOOL *const  pf_lckdEnd)	{
	BOOL  f_lckd;
	ItemInfo * pt_lckd = NULL;

	_ASSERTE( pt_crsr && pt_crsr->puc_location && pt_crsr->pt_item && 
												pt_crsr->pt_item->puc_start && 
												pt_crsr->us_recLength);
	
	if (pt_spn)
		memset( pt_spn, NULL, sizeof( CdRecordSpan));
	if (pf_lckd)
		*pf_lckd = FALSE;
	if (pf_lckdBegin)
		*pf_lckdBegin = FALSE;
	if (pf_lckdEnd)
		*pf_lckdEnd = FALSE;

	//if caller requested an intial advance of the cursor to the next CD record 
	//	before starting the search...
	if (f_BUMP_FWD_1ST)	{
		//Advance the cursor. If the available rich-text context has been 
		//	exhausted, return that the sought-for record was not found.
		AdvanceCdCursor( pt_crsr, pt_CTX, NULL, &f_lckd);
		if (!pt_crsr->pt_item)
			return FALSE;

		if (f_lckd)
			pt_lckd = pt_crsr->pt_item;
	} //if (f_BUMP_FWD_1ST)

	//Loop record-by-record until the specified CD record is found or the 
	//	provided rich-text context has been exhausted. Manage item locking so 
	//	that we make efficient use of memory resources.
	while (!( muc_SIG_CDTEXT == *pt_crsr->puc_location || muc_SIG_CDBEGIN == 
												*pt_crsr->puc_location))	{
		AdvanceCdCursor( pt_crsr, pt_CTX, NULL, &f_lckd);
		if (!pt_crsr->pt_item)
			goto failJump;
		if (f_lckd)	{
			if (pt_lckd)
				UnlockItem( pt_lckd);
			pt_lckd = pt_crsr->pt_item;
		}
	} //while (!( muc_SIG_CDTEXT == 
	_ASSERTE( muc_SIG_CDTEXT == *pt_crsr->puc_location ? muc_SIGTYPE_WSIG == 
											*(pt_crsr->puc_location + 1) : 
											!( *(pt_crsr->puc_location + 1) == 
											muc_SIGTYPE_LSIG || 
											*(pt_crsr->puc_location + 1) == 
											muc_SIGTYPE_WSIG));

	//set requested begin-record outputs
	if (pt_spn)	{
		pt_spn->t_crsrBgin = *pt_crsr;
		if (pf_lckdBegin && f_lckd)	{
			*pf_lckdBegin = TRUE;
			f_lckd = FALSE;
		}
	}

	//if we've lit upon the beginning of an R5 text multi-CD-record structure...
	if (muc_SIG_CDBEGIN == *pt_crsr->puc_location)	{
		//Loop to the end of the structure or until the rich-text context has 
		//	been exhausted. Manage item locking so that we make efficient use 
		//	of memory resources.
		do	{
			AdvanceCdCursor( pt_crsr, pt_CTX, NULL, &f_lckd);
			_ASSERTE( muc_SIG_CDBEGIN != *pt_crsr->puc_location);
			if (!pt_crsr->pt_item)
				goto failJump;
			if (f_lckd)	{
				if (pt_lckd)
					UnlockItem( pt_lckd);
				pt_lckd = pt_crsr->pt_item;
			}
		} while (muc_SIG_CDEND != *pt_crsr->puc_location);

		//set requested end-record outputs
		if (pt_spn)	{
			pt_spn->t_crsrEnd = *pt_crsr;
			if (pf_lckdEnd && f_lckd)	{
				*pf_lckdEnd = TRUE;
				f_lckd = FALSE;
			}
		}
	} //if (muc_SIG_CDBEGIN == *pt_crsr->puc_location)

	//advance the main cursor to the record immediately following the text 
	//	structure discovered
	AdvanceCdCursor( pt_crsr, pt_CTX, NULL, &f_lckd);
	if (f_lckd || !pt_crsr->pt_item)	{
		if (pt_lckd)
			UnlockItem( pt_lckd);
		pt_lckd = pt_crsr->pt_item;
	}

	if (pf_lckd && pt_lckd)
		*pf_lckd = TRUE;

	return TRUE;

failJump:
	if (pt_spn && pt_spn->t_crsrBgin.pt_item)	{
		memset( &pt_spn->t_crsrBgin, NULL, sizeof( CdRecordCursor));
		if (pf_lckdBegin && *pf_lckdBegin)
			UnlockItem( pt_spn->t_crsrBgin.pt_item);
	}
	if (pt_lckd)
		UnlockItem( pt_lckd);

	return FALSE;
} //f_cursorToText(


/** f_cursorToRecord( ***
Moves provided rich-text cursor to the next instance of the specified CD 
record within the provided rich-text context.

--- parameters & return ----
us_SIG: the signature of the CD record being sought (SIG_CD_xxx)
pt_cursor: Input & Output. Address of the rich-text cursor positioned to the 
	point at which the search for the specified CD record should commence. 
	This cursor is then used when advancing through CD records. If the 
	procedure is successful, the cursor will finish at the next instance of 
	the specified record, else its "item" member will have been nullified to 
	indicate that it has travelled through the entire rich-text context 
	available.
pt_CTXT: Optional. Address of an information structure describing the 
	rich-text context being navigated. If passed in null, the rich-text 
	cursor being used will be unable to jump from an actual rich-text context 
	to a virtual rich-text context.
f_BUMP_FORWARD_FIRST: Flag telling whether the passed-in starting cursor 
	should be moved to the next CD record before initiating the search for 
	the specified CD record. Usually when starting at the beginning of the 
	rich-text field, the flag will be set to FALSE so that the first CD 
	record may be checked as to whether it's a file-attachment hotspot. 
	Ensuing calls might then set the flag to TRUE to move to the next CD 
	record since the current CD record is already known.
pf_locked: Optional Output. Address of the flag in which to store whether the
	rich-text item in which the output cursor finds itself was newly locked 
	in the course of this procedure. If passed in null, the procedure will 
	ignore this functionality.
RETURN: TRUE if specified record was found; FALSE if not

--- revision history -------
3/20/00 PR: created			*/
static BOOL f_cursorToRecord( const USHORT  us_SIG, 
								CdRecordCursor *const  pt_crsr, 
								const RtfTrackingInfo *const  pt_CTXT, 
								const BOOL  f_BUMP_FORWARD_FIRST, 
								BOOL *const  pf_locked)	{
	BYTE  uc_SIG = LOBYTE( us_SIG);

	BOOL  f_locked;
	ItemInfo * pt_locked = NULL;

	_ASSERTE( uc_SIG && pt_crsr && pt_crsr->puc_location && 
											pt_crsr->pt_item && 
											pt_crsr->pt_item->puc_start && 
											pt_crsr->us_recLength);

	if (pf_locked)
		*pf_locked = FALSE;

	//if caller requested an intial advance of the cursor to the next CD 
	//	record before starting the search...
	if (f_BUMP_FORWARD_FIRST)	{
		//Advance the cursor. If the available rich-text context has been 
		//	exhausted, return that the sought-for record was not found.
		AdvanceCdCursor( pt_crsr, pt_CTXT, NULL, &f_locked);
		if (!pt_crsr->pt_item)	{
			pt_crsr->puc_location = NULL;
			return FALSE;
		}

		if (f_locked)
			pt_locked = pt_crsr->pt_item;
	} //if (f_BUMP_FORWARD_FIRST)

	//Loop record-by-record until the specified CD record is found or the 
	//	provided rich-text context has been exhausted. Manage item locking so 
	//	that we make efficient use of memory resources.
	while (*pt_crsr->puc_location != uc_SIG)	{
		AdvanceCdCursor( pt_crsr, pt_CTXT, NULL, &f_locked);
		if (!pt_crsr->pt_item)	{
			if (pt_locked)
				UnlockItem( pt_locked);
			pt_crsr->puc_location = NULL;
			return FALSE;
		}
		if (f_locked)	{
			if (pt_locked)
				UnlockItem( pt_locked);
			pt_locked = pt_crsr->pt_item;
		}
	} //while (*pt_crsr->puc_location != uc_SIG)
	_ASSERTE( *(pt_crsr->puc_location + 1) == muc_SIGTYPE_LSIG ? 
										HIBYTE( us_SIG) == muc_SIGTYPE_LSIG : 
										*(pt_crsr->puc_location + 1) == 
										muc_SIGTYPE_WSIG ? HIBYTE( us_SIG) == 
										muc_SIGTYPE_LSIG : TRUE);

	if (pf_locked && pt_locked)
		*pf_locked = TRUE;

	return TRUE;
} //f_cursorToRecord(


/** ClearItemList( ***
Purpose is to clear out completely a list of rich-text item structures, 
including the freeing of associated memory.

--- parameter ----------
ppt_ListHead: Input & Output. Pointer to the node at which to start the 
	clearing out of the list. The node will be nullified upon return.

--- revision history ---
10/29/98 PR: created		*/
static void ClearItemList( ItemInfo * *const  ppt_ListHead)	{
	_ASSERTE( ppt_ListHead);

	//if there's something to deal with, clear out the list
	if (*ppt_ListHead)	{
		UnlockItems( *ppt_ListHead, NULL);
		FreeItemList( ppt_ListHead);
	}
} //ClearItemList(


/** UnlockItems( ***
Purpose is to unlock the memory associated with a given succession of 
rich-text items.

--- parameters ---------
pt_ListHead: Input & Output. Address of the information structure describing 
	the first item of the list of rich-text items to unlock (the "head 
	node"). This head node need not be the actual head node of the source 
	linked-list of items. For each item unlocked, its information structure 
	will be updated to reflect the change.
pt_STOP_NODE: Optional. Address of the information structure describing the 
	rich-text item at which unlocking should stop. Obviously, to be effective 
	the item must appear later in the list than the specified head node. If 
	null, procedure will automatically unlock all items following the head 
	node.

--- revision history ---
3/20/00 PR
+ logic shortening due to creation of UnlockItem() procedure
+ documentation adjustment, safety improvement

10/29/98 PR: created		*/
static __inline void UnlockItems( ItemInfo *const  pt_ListHead, 
									const ItemInfo *const  pt_STOP_NODE)	{
	ItemInfo * pt_item = pt_ListHead;

	_ASSERTE( pt_ListHead);

	//Loop through the given list, unlocking the memory associated with each 
	//	rich-text-item node, as necessary. If no stop node was provided, the 
	//	loop will run through to the end of the list.
	while (pt_item && pt_item != pt_STOP_NODE)	{
		UnlockItem( pt_item);
		pt_item = pt_item->pt_next;
	}
} //UnlockItems(


/** UnlockItem( ***
If necessary, unlocks the memory associated with a given rich-text item.

--- parameter ----------
pt: Input & Output. Address of the information structure describing the 
	rich-text item to be unlocked. The structure will be updated to reflect 
	that the item is no longer locked in memory.

--- revision history ---
3/20/00 PR: created		*/
static __inline void UnlockItem( ItemInfo *const  pt)	{
	_ASSERTE( pt);

	if (!pt->puc_start)
		return;

	OSUnlockBlock( pt->bid_contents);
	pt->puc_start = NULL;
} //UnlockItem(


