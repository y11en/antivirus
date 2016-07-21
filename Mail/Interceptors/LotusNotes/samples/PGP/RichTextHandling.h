/*______________________________________________________________________________
	Copyright (C) 2002-2003 PGP Corporation
	All rights reserved.


	$Id: RichTextHandling.h 15767 2003-07-26 18:25:11Z pryan $
______________________________________________________________________________*/


#ifndef Included_RichTextHandling_h	/* [ */
#define Included_RichTextHandling_h


//system includes
#include <stdlib.h>		//for malloc(), free(), etc.
#include <string.h>
#include <crtdbg.h>		//for _ASSERTE(), etc.

#include "NotesApiGlobals.h"
//Notes API includes
#include <nsfnote.h>
#include <nsferr.h>		//for ERR_ITEM_NOT_FOUND
#include <oserr.h>		//for ERR_MEMORY
#include <miscerr.h>	//for ERR_NOT_FOUND
#include <easycd.h>		//for COMPOUNDSTYLE, CDxxx constants, etc.
#include <osmisc.h>		//for OSLoadString()
#include <colorid.h>	//for NOTE_COLOR_DKRED
#include <editdflt.h>	//for CDPABDEFINITION defaults like DEFAULT_TABS
#include <nsfsearc.h>	//for NSFFormulaCompile()

enum	{
	mi_VIRTUAL, 
	mi_ACTUAL
};

typedef struct _ItemInfo	{ 
	BLOCKID  bid_contents;		//Structure describing the position of the 
				//	memory dedicated to the item. Structure defined and used 
				//	extensively by the Notes API.
	int  i_type;				//Describes the type of item, differentiating 
				//	a virtual item (mi_VIRTUAL), one not yet committed to 
				//	a note, from an actual item (mi_ACTUAL).
	DWORD  ul_length;			//The current length of the content thus far 
				//	written to the item. A DWORD to accommodate potential 
				//	future increase in the maximum size of a rich-text item, 
				//	currently MAXONESEGSIZE.
//I don't recall what the intent behind this member was, but it's not being 
//	used anywhere, so I remmed it out
//	DWORD  ul_allocated;		//The length of the memory allocation made 
//				//	for this item. Only applicable to virtual items.
	BYTE * puc_start;			//Pointer to the start of the coíntent in the 
				//	item. Obtained via a call to OSLockBlock() against the 
				//	bid_contents member of this structure.
	struct _ItemInfo * pt_next;	//linked-list pointer to a follow-on item, 
				//	continuing the stream of rich-text content
} ItemInfo;

typedef struct	{ 
	ItemInfo  t_item;
	BLOCKID  bid_item;
} ItemInfo_Actual;

typedef struct	{
	BYTE * puc_location;		//pointer to the beginning of the CD record 
				//	being pointed to
	WORD  us_recLength;			//the length of the pointed-to CD record
	ItemInfo * pt_item;			//Pointer to a structure of information about 
				//	the rich-text item in which the cursor currently finds 
				//	itself. Often points to a node in one of the item lists 
				//	in the overriding RtfTrackingInfo structure, meaning that 
				//	a change to the structure using this pointer makes the 
				//	same change in the overriding RtfTrackingInfo structure. 
				//	Member often set to NULL to indicate an invalid cursor.
} CdRecordCursor;

typedef struct	{
	ItemInfo * pt_Actuality;	//a linked-list of information about the rich-
				//	text items already committed to the note as true items
	DWORD  ul_ActualFrontier;	//a moving absolute offset into the actual 
				//	set of rich-text items that notes the most forward point 
				//	behind which the rich-text information has been 
				//	virtualized or discarded and ahead of which is all 
				//	actuality
	ItemInfo * pt_Virtuality;	//a linked-list of information about the 
				//	rich-text items to serve as replacement to at least the 
				//	trailing actual rich-text items
//	BLOCKID  bid_firstVirtualized;	//notes the first actual item which has 
//				//	been virtualized
	ItemInfo * pt_endVirtual;	//included for ease of access to the last 
				//	virtual item, i.e. the one most likely to be next 
				//	manipulated
	ItemInfo * pt_SemiVirtuality;	//A linked-list of information about the 
				//	rich-text items that are still being treated as part of 
				//	the "actual" rich-text but that have undergone some change
				//	and so ultimately must be virtualized. Kind of an "on the 
				//	bubble" mechanism for allowing recursive processing 
				//	passes over the same stream of rich-text CD records.
} RtfTrackingInfo;

typedef struct	{
	CdRecordCursor  t_crsrBgin;	//cursor pointing to the CD record 
				//	containing the start of the span
	WORD  us_offstBgin;			//1-based offset into the beginning CD 
				//	record's *content* (e.g. a CDTEXT's text run) *after* 
				//	which the span begins
	CdRecordCursor  t_crsrEnd;	//cursor pointing to the CD record 
				//	containing the end of the span
	WORD  us_offstEnd;				//1-based offset (left to right) into the 
				//	ending CD record's *content* at which the span ends
}  CdRecordSpan;

typedef enum	{
	mi_REGULAR_VIRTUAL = 1, 
	mi_SEMI_VIRTUAL, 
}  VirtualType;

//Structure intended to be used to implement a mechanism for 
//	saving and restoring the memory-lock of rich-text content. 
//	See suggested-enhancement note in main source file.
typedef struct _MemLockStateInfo	{
	ItemInfo *	pt_item;
	BOOL  f_WasLocked;
	struct _MemLockStateInfo * pt_next;
}  MemLockStateInfo;


extern const char  ec_SPACE;

#define mi_MAXLEN_ATTCH_OBJNM  128
static const COMPOUNDSTYLE  mt_HIDDEN_PARAGRAPH_DEF = 
								{DEFAULT_JUSTIFICATION, DEFAULT_LINE_SPACING, 
								DEFAULT_ABOVE_PAR_SPACING, 
								DEFAULT_BELOW_PAR_SPACING, 
								DEFAULT_LEFT_MARGIN, DEFAULT_RIGHT_MARGIN, 
								DEFAULT_FIRST_LEFT_MARGIN, DEFAULT_TABS, 
								{DEFAULT_TAB_INTERVAL}, 
								PABFLAG_HIDEBITS & ~PABFLAG_HIDE_IF | 
								DEFAULT_PAGINATION};


BOOL ef_GetPgraphStyleCount( CdRecordCursor, const RtfTrackingInfo *const, 
																WORD *const);
BOOL ef_FindPgraphStyleUnusedSpan( const WORD, CdRecordCursor, 
												const RtfTrackingInfo *const, 
												WORD *const, BOOL *const);
BOOL ef_RenumberPgraphStylesToSpan( const WORD, const WORD, CdRecordCursor, 
												const RtfTrackingInfo *const);

static __inline void UnlockItem( ItemInfo *const);
static BOOL f_cursorToRecord( const USHORT, CdRecordCursor *const, 
												 const RtfTrackingInfo *const, 
												 const BOOL, BOOL *const);
static BOOL f_cursorToText( CdRecordCursor *const, 
									const RtfTrackingInfo *const, const BOOL, 
									CdRecordSpan *const, BOOL *const, 
									BOOL *const, BOOL *const);
static BOOL f_ConstructStartCursor( const RtfTrackingInfo *const, 
											ItemInfo *const, 
											CdRecordCursor *const, BOOL *const);
static __inline WORD us_getCdRecLength( const BYTE *const);
static __inline void FreeItemList( ItemInfo * *const);
static STATUS us_CompileActuality( const HANDLE, const char[], 
															ItemInfo * *const);
static void AdvanceCdCursor( CdRecordCursor *const, 
												const RtfTrackingInfo *const, 
												BOOL *const, BOOL *const);
static BOOL f_PushToNextItem( CdRecordCursor *const, 
											   const RtfTrackingInfo *const, 
											   BOOL *const, BOOL *const);
static STATUS us_VirtualizePointToPoint( CdRecordCursor *const, 
									const BYTE *const, RtfTrackingInfo *const);
static BOOL f_CursorToHotspotEnd( BYTE, CdRecordCursor *const, 
										const RtfTrackingInfo *const, 
										const BOOL, BOOL *const, BOOL *const);
static __inline BOOL f_ItemIsInList( const ItemInfo *const, 
														const ItemInfo *const);
static BOOL f_LocateAbsoluteOffset( DWORD, ItemInfo *const, 
										ItemInfo * *const, const BYTE * *const, 
										DWORD *const, BOOL *const);
static BOOL f_TestStringStart( const char[], const BOOL, 
											const CdRecordCursor, const WORD, 
											const RtfTrackingInfo *const, 
											BOOL *const, WORD *const);
static __inline void UnlockItems( ItemInfo *const, const ItemInfo *const);
static unsigned long ul_getNearLinebreakOffset( const char *const, 
												unsigned long, const BOOL);
static STATUS us_AppendTextToVirtuality( const char[], DWORD, const BOOL, 
										CdRecordCursor *const, const WORD, 
										const FONTID, RtfTrackingInfo *const, 
										CdRecordSpan *const);
static void ClearItemList( ItemInfo * *const);
static DWORD ul_TextSpanLength( const CdRecordSpan *const, const DWORD, 
									const BOOL, const RtfTrackingInfo *const);
static int i_ProcessRtfText( const char *const, const WORD, const DWORD, 
							   const BOOL, DWORD *const, BYTE[], BOOL *const);
static STATUS us_FillVirtualItemAndStartNext( const DWORD, ItemInfo * *const, 
										const  BYTE * *const, DWORD *const, 
										CdRecordCursor *const);
static STATUS us_CreateVirtualItem( const VirtualType, const DWORD, 
															ItemInfo * *const);
static __inline STATUS us_StartVirtualItemList( const DWORD, 
														RtfTrackingInfo *const);
static STATUS us_VirtualAppend( const BYTE *, DWORD, RtfTrackingInfo *const, 
											const DWORD, CdRecordCursor *const);
static STATUS us_VirtualizeThruActualLocation( const BYTE *const, 
									RtfTrackingInfo *const, 
									RtfTrackingInfo *, CdRecordCursor *const);
static __inline STATUS us_InsertNextVirtualItem( ItemInfo *const, 
												const VirtualType, const DWORD);
static BOOL f_ProcessKnownLengthRtfText( const CdRecordCursor, const WORD, 
							const DWORD, const BOOL, 
							const RtfTrackingInfo *const, 
							CdRecordCursor *const, WORD *const, char *const);
static DWORD ul_GetAbsoluteOffset( const BYTE *const, const ItemInfo *const, 
														const ItemInfo *const);
static void TranslateCrlfText( const char *const, DWORD, DWORD *const, 
											BYTE *const, const char * *const);
static STATUS us_GetVirtualCdRecAppendPointer( RtfTrackingInfo *const, 
													const DWORD, BYTE * *const);
static STATUS us_VirtualizeRestOfRtf( RtfTrackingInfo *const, 
															RtfTrackingInfo *);
static DWORD ul_LogicalRtfLength( const RtfTrackingInfo *const);
static void CreateCursorAtRcrdPrior( ItemInfo *const, const BYTE *const, 
											const BYTE *const, 
											CdRecordCursor *const, BOOL *const);
static STATUS us_RetreatCursor( CdRecordCursor *const, 
									const RtfTrackingInfo *const, BOOL *const);


#endif /* ] Included_RichTextHandling_h */
