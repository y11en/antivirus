/*______________________________________________________________________________
	Copyright (C) 2002-2003 PGP Corporation
	All rights reserved.


	$Id: LibRichText.h 15767 2003-07-26 18:25:11Z pryan $
______________________________________________________________________________*/

/*::: FUNCTIONAL OVERVIEW ::::::::::

--- revision history ----
7/21/03 Version 1.3.2 Paul Ryan
+ logic enhancment to eus_CursorToFormulaActionAttach() & 
  eus_CollapseToEndAttachWrap() to accommodate possible blank space within, and 
  SIG_CD_BEGIN/-END wrapping of the attachment hotspot within, 
  action-attachment hotspots like those written for the PGP plug-in

3/14/03 Version 1.3 Paul Ryan
+ added functionality to support Notes 6 PGP plug-in
+ minor signature adjustment to eus_InsertTextAtHotspotEnd()

9/6/02 Version 1.2.3 Paul Ryan
+ added eus_CursorAdvance()
+ enhanced eus_ReplaceRtfSpanText() so it outputs rich-text span marking 
  replacement
+ improved reliability of ef_CursorToAttachmentHotspot()
+ improved eus_WriteEndHotspotRecord() causing rename to eus_InsertHotspotEnd()

5/30/00 Version 1.2.1 Paul Ryan
+ extended to allow construction of formula action-hotspots

3/20/00 Version 1.2 Paul Ryan
+ improvement, R5-enablement of eus_SubformTop(), associated signature change
+ adjustments made in support of making the PGP Plug-In compatible with the 
  Notes R5 mail template
+ imporvement to span-alignment output of ef_CursorToAttachmentHotspot()
+ bug fix to eus_AppendAttachmentHotspot()

1/5/99 Version 1.0 Paul Ryan
::::::::::::::::::::::::::::::::::::*/

#ifndef Included_LibRichText_h	/* [ */
#define Included_LibRichText_h


//Notes API includes
#include <easycd.h>		//for COMPOUNDSTYLE
#include <editdflt.h>	//for CDPABDEFINITION defaults like DEFAULT_TABS


#define i_LEN_OPAQUE_RTCURSOR  4
typedef struct	{
	BYTE * puc_location;		//should always point to the beginning of 
						//	a CD record
	WORD  us_recLength;
	BYTE  opaque[ i_LEN_OPAQUE_RTCURSOR];
} RtfCursor;

#define i_LEN_OPAQUE_RTCONTEXT  26
typedef struct	{
	BYTE  opaque[ i_LEN_OPAQUE_RTCONTEXT];
} RtfContext;

typedef struct	{
	RtfCursor  t_crsrBgin;
	WORD  us_offstBgin;
	RtfCursor  t_crsrEnd;
	WORD  us_offstEnd;
}  RtfSpan;


extern const DWORD  eul_ERR_FAILURE;

#define mi_MAXLEN_ATTCH_OBJNM  128
static const COMPOUNDSTYLE  mt_RT_PGRPH_HIDDEN = 
								{DEFAULT_JUSTIFICATION, DEFAULT_LINE_SPACING, 
								DEFAULT_ABOVE_PAR_SPACING, 
								DEFAULT_BELOW_PAR_SPACING, 
								DEFAULT_LEFT_MARGIN, DEFAULT_RIGHT_MARGIN, 
								DEFAULT_FIRST_LEFT_MARGIN, DEFAULT_TABS, 
								{DEFAULT_TAB_INTERVAL}, 
								PABFLAG_HIDEBITS & ~PABFLAG_HIDE_IF | 
								DEFAULT_PAGINATION}, 
							mt_RT_PGRPH_NORMAL = 
								{DEFAULT_JUSTIFICATION, DEFAULT_LINE_SPACING, 
								DEFAULT_ABOVE_PAR_SPACING, 
								DEFAULT_BELOW_PAR_SPACING, 
								DEFAULT_LEFT_MARGIN, DEFAULT_RIGHT_MARGIN, 
								DEFAULT_FIRST_LEFT_MARGIN, DEFAULT_TABS, 
								{DEFAULT_TAB_INTERVAL}, 
								DEFAULT_PAGINATION};


STATUS eus_InitializeRtfContext( const NOTEHANDLE, const char[], 
										RtfCursor *const, RtfContext *const);
STATUS eus_CursorAdvance( RtfCursor *const, const RtfContext *const);
BOOL ef_CursorToStringStart( const char[], const BOOL, RtfCursor, const WORD, 
												const RtfContext *const, 
												RtfCursor *const, WORD *const);
STATUS eus_SwapHotspots( RtfCursor *const, RtfCursor, RtfContext *const);
BOOL ef_FreeRtfContext( RtfContext *const);
DWORD eul_GetRtfSpanText( const RtfSpan *const, const DWORD, const BOOL, 
										const RtfContext *const, char * *const);
BOOL ef_CursorToAttachmentHotspot( RtfCursor *const, const RtfContext *const, 
									const BOOL, RtfSpan *const, 
									const char * *const, const char * *const);
STATUS eus_CursorToFormulaActionAttach( RtfCursor *const, 
									  const RtfContext *const, const BOOL, 
									  RtfSpan *const, RtfSpan *const, 
									  const char * *const, const char * *const);
STATUS eus_ResetAttachHotspotNames( const char[], const char[], 
												const RtfCursor, const BOOL, 
												RtfContext *const);
DWORD eul_GetRtfText( const RtfContext *const, const DWORD, const BOOL, 
																char * *const);
STATUS eus_ReplaceRtfSpanText( RtfSpan *const, const DWORD, const BOOL, 
											const char[], RtfContext *const);
STATUS eus_CommitChangedRtf( const NOTEHANDLE, const char[], RtfContext *const);
STATUS eus_ReplaceRtfWithStandardText( const char[], const BOOL, 
															RtfContext *const);
STATUS eus_AppendRtParagraph( const WORD, const COMPOUNDSTYLE *const, 
												RtfContext *const, WORD *const);
RtfContext * ept_InitUtilityRtfContext( RtfContext *const);
STATUS eus_AppendItemsToRtf( const RtfContext *const, RtfContext *const);
BOOL ef_UnappendRtfItems( const RtfContext *const, RtfContext *const);
STATUS eus_ReplaceRtf( RtfContext *const, RtfContext *const);
STATUS eus_CopyRtfIntoBuffer( const RtfContext *const, HANDLE *const, 
												DWORD *const, BYTE * *const);
STATUS eus_AppendAttachmentHotspot( NOTEHANDLE, const char[], char[], 
												const BYTE *const, const WORD, 
												RtfContext *const);
STATUS eus_ReplaceRtfWithCdStream( const BYTE *const, const DWORD, 
															RtfContext *const);
STATUS eus_RemoveRtSpan( const RtfSpan *const, RtfContext *const);
STATUS eus_InsertTextAtHotspotEnd( const char[], RtfCursor, RtfContext *const);
BOOL ef_RenumberPgraphStylesToSpan( const WORD, const WORD, RtfCursor, 
													const RtfContext *const);
BOOL ef_FindPgraphStyleUnusedSpan( const WORD, RtfCursor, 
													const RtfContext *const, 
													WORD *const, BOOL *const);
BOOL ef_GetPgraphStyleCount( RtfCursor, const RtfContext *const, WORD *const);
STATUS eus_InsertSubformTop( RtfContext *const, const char[]);
STATUS eus_AddRtfActualContext( NOTEHANDLE, char[], RtfContext *const);
STATUS eus_CopyRtfContent( RtfContext *const, const BOOL, RtfContext *const);
STATUS eus_StartFormulaActionHotspot( const char[], const RtfCursor *const, 
															RtfContext *const);
STATUS eus_StartLinkHotspot( const char[], const RtfCursor *const, 
															RtfContext *const);
STATUS eus_CloseLinkHotspot( const RtfCursor *const, RtfContext *const);
STATUS eus_InsertHotspotEnd( const RtfCursor, RtfContext *const);
STATUS eus_InsertAttachHotspot( RtfCursor *const, const char[], char[], 
										const NOTEHANDLE, const BYTE *const, 
										const WORD, const char[], 
										const char[], const FONTID, 
										RtfSpan *const, RtfContext *const);
STATUS eus_ActionHotspotJustBefore( const RtfCursor *const, 
										const RtfContext *const, BOOL *const);
STATUS eus_AppendCdsToRtf( const BYTE *const, const ULONG, RtfContext *const);
STATUS eus_RemoveAttachWrap( RtfSpan *const, RtfContext *const);
STATUS eus_CollapseToEndAttachWrap( RtfSpan *const, RtfContext *const);
STATUS eus_AppendHotspotInnardsToVirtuality( RtfCursor *const, 
															RtfContext *const);
STATUS eus_ConstructRtCursor( const RtfContext *const, RtfCursor *const);


#endif /* ] Included_LibRichText_h */
