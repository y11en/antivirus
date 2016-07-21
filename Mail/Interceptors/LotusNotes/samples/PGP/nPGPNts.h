/*______________________________________________________________________________
	Copyright (C) 2002-2003 PGP Corporation
	All rights reserved.


	$Id: nPGPNts.h 15552 2003-07-14 09:59:47Z pryan $
______________________________________________________________________________*/

#ifndef Included_nPGPNts_h	/* [ */
#define Included_nPGPNts_h

//Windows SDK includes
#define WIN32_LEAN_AND_MEAN
#include <windows.h>	//for MessageBox(), string.h, etc.
#include <commdlg.h>	//for GetSaveFileName(), etc.
#include <shellapi.h>	//for ShellExecute()
#undef WIN32_LEAN_AND_MEAN

//C run-time includes
#include <stdlib.h>		//for malloc(), free(), etc.
#include <stdio.h>		//for sprintf(), _tempnam()
#include <crtdbg.h>		//for debug macros like _ASSERTE()

#include "NotesApiGlobals.h"
//Notes API includes
#include <nsfdb.h>		//for NSFDbGetUnreadTable(), etc.
#include <nsfnote.h>
#include <textlist.h>	//for ListGetNumEntries()
#include <lookup.h>		//for NAMELookup(), etc.
#include <osmisc.h>		//for OSLoadString()
#include <osfile.h>		//for OSGetDataDirectory()
#include <names.h>		//for MAXPATH
#include <nsfsearc.h>	//for NSFComputeEvaluate(), etc.
#include <kfm.h>		//for SECKFMGetUserName()
#include <idtable.h>	//for IDTableCopy(), etc.
#include <fontid.h>		//for FontSetColor(), etc.
#include <colorid.h>	//for NOTES_COLOR_DKRED
#include <stdnames.h>	//for MAIL_MAILER_ITEM 
#include <nsferr.h>		//for ERR_NOTE_BADATTSIGN, etc.
#include <oserr.h>		//for ERR_MEMORY

//internal includes
#include "LibRichText.h"
#include "LibItem.h"
#include "LibObject.h"
#include "LibNab.h"
#include "LibFileSystem.h"
#include "LibNotesMime.h"
#include "VbCallable.h"
#include "resource.h"

#define i_LEN_OPAQUE_PGP_ENCODE_CONTEXT  52 + 4
typedef struct	{
	BOOL  f_Sign;
	BOOL  f_Encrypt;
	BOOL  f_asciiOnlyBdy;
	void * pv_missngAdks;
	BYTE  opaque[ i_LEN_OPAQUE_PGP_ENCODE_CONTEXT];
}  PgpEncodeContext;

typedef struct	{
	NameFoundInfo  t;
	char * pc_parent;
}  NameFoundInfoEx;

typedef struct	{
	HANDLE  h_content;				//handle to memory buffer holding the 
						//	rich-text field content
	DWORD  ul_len;					//length of content in buffer
	NOTEHANDLE  h_note;				//handle to the Notes document upon which 
						//	any reset will occur
}  ResettableRtfInfo;

typedef struct	{
	HWND  h;						//handle to window
	WNDPROC  prc;						
}  WndProcInfo;

typedef struct _WndProcNode	{
	WndProcInfo * pt;
	struct _WndProcNode * pt_nxt;
}  WndProcNode;

typedef struct _MimePrtNode	{
	MimePrtItmCtnt * pt;
	struct _MimePrtNode * pt_nxt;
}  MimePrtNode;

typedef struct	{
	char * pc_fileNm;
	char * pc_extFileNm;
	int * pi_choice;
}  DlgIoExtract;

typedef struct	{
	HFONT  h_fntFileCaption;
	int  i_choice;
	BOOL  f_dcryptdChecked;
	HWND  h_rdLaunch;
	HWND  h_rdExtract;
	HWND  h_ebxExtFileNm;
}  DlgStateExtract;


extern const int  ei_USER_ABORT;
extern char  epc_APPNM[], epc_ITMNM_STD_ATTCH[], ec_HYPH, ec_QUOTES;

extern HWND  eh_mainWnd;
extern BOOL  ef_LoadedSessionLong;


void e_ReleasePgpContext( void);
BOOL ef_alwaysUseTempest( void);
UINT eui_ShowInTempestWnd( const char[], const BOOL);
int ei_TestForPgpAscii( const char *const, BOOL *const);
int ei_PgpDecodeBuffer( const BYTE *const, const DWORD, const BOOL, 
											const char * *const, DWORD *const);
void e_FreePgpMem( BYTE *const);
int ei_PgpEncodeBuffer( char *const, const DWORD, PgpEncodeContext *const, 
									const BOOL, long *const, char * *const);
int ef_FreePgpEncodeContext( PgpEncodeContext *const);
int ei_SetUpPgpEncodeContext( const BOOL, const BOOL, PgpEncodeContext *const, 
																BOOL *const);
int ei_ResolveEncryptionKeys( char * *const, const UINT, const BOOL, 
										const BOOL, PgpEncodeContext *const);
int ei_FindAndDecodePgpBlock( char * *const, const DWORD, BOOL *const, 
												char * *const, DWORD *const, 
												char * *const, int *const);
int ei_PgpEncodeFile( PgpEncodeContext *const, const char[], const char[]);
int ei_PgpLookupEmailLocal( const BOOL, PgpEncodeContext *const, 
															NameFoundNode *);
int ei_PgpLookupEmailViaServers( PgpEncodeContext *const, NameFoundNode *);
int ei_PgpLookupMissingAdks( PgpEncodeContext *const);
int ei_PgpDecodeFile( const char[], char * *const);
int ei_PgpWipeFile( const char[]);
BOOL ef_findPgpEncryptedBlock( const char[], ULONG, const char * *const, 
																ULONG *const);
STATUS xs_PgpSecureMessage( const NOTEHANDLE, BOOL, BOOL, char[], const BOOL, 
										char[], const int, const char[], 
										const char[], ResettableRtfInfo *const);
STATUS xus_TestNoteForPgpAscii( const NOTEHANDLE, char[], short *const);
STATUS xus_TestNoteForPgpArmor( const NOTEHANDLE, char[], short *const, 
																short *const);

static STATUS us_ReplaceRichAsciiLines( const char *const, const char *const, 
										const DWORD, const char *const, 
										RtfCursor *const, RtfContext *const);
static int i_MatchPgpKeys( const BOOL, const UINT, char *const *const, 
													PgpEncodeContext *const);
static int i_PgpMatchNotesNames( const BOOL, const UINT, char *const *const, 
													PgpEncodeContext *const);
static int i_PgpMatchSmtpAddresses( const BOOL, const UINT, char *const *const, 
													PgpEncodeContext *const);
static int i_PgpDecodeUsingAsciiText( const NOTEHANDLE, const char[], 
												const BOOL, RtfCursor, 
												RtfContext *const, BOOL *const);
static int i_PgpDecodeUsingRichTextObj( const DWORD, const DWORD, 
											const BLOCKID, const NOTEHANDLE, 
											const char[], const BYTE *const, 
											RtfContext *const, int *const);
static STATUS us_ActionWrapPgpAttachments( const NOTEHANDLE, RtfCursor, 
															RtfContext *const);
static int i_PgpEncodeAttachments( PgpEncodeContext *const, const NOTEHANDLE, 
									const char[], RtfCursor, RtfContext *const);
static STATUS us_ActionWrapAttachment( const RtfSpan *const, const char[], 
									const char[], const NOTEHANDLE, const BOOL, 
									const char[], RtfContext *const);
static int i_PgpEncodeRtfText( PgpEncodeContext *const, RtfContext *const, 
																char * *const);
static STATUS us_CreateRecipientsArray( char[], const BOOL, char[], 
													char * * *const, 
													UINT *const, BOOL *const);
static __inline void FreeAllocatedStringArray( char * * *const, const DWORD);
static int i_PgpEncodeRtfAsAttachment( PgpEncodeContext *const, 
									const NOTEHANDLE, const RtfContext *const, 
									RtfContext *const, BLOCKID *const);
static BOOL f_TestForPgpRtfAttachment( RtfCursor, const RtfContext, 
																BOOL *const);
static STATUS us_LocatePgpRtfAttachment( RtfCursor, const RtfContext *const, 
												const NOTEHANDLE, DWORD *const, 
												DWORD *const, BLOCKID *const);
static STATUS us_SetupRtfAttachParagraph( NOTEHANDLE, RtfContext *const, 
																char *const);
static LRESULT CALLBACK ActionBarWndProc( const HWND, const UINT, 
												const WPARAM, const LPARAM);
static DWORD ul_ExtractDlgInit( const HWND, const DlgIoExtract *const, 
										const HWND, DlgStateExtract *const);
static BOOL CALLBACK f_DlgProcExtract( HWND, UINT, WPARAM, LPARAM);
static LRESULT CALLBACK WndProcExtractRdo( const HWND, const UINT, 
												const WPARAM, const LPARAM);
static __inline void ClearRdOverride( const HWND);
static __inline BOOL f_PgpExtension( const char[], const size_t);
static STATUS us_ExtractPgpMimeDecodable( const NOTEHANDLE, char * *const, 
									const BYTE * *const, BLOCKID *const, 
									size_t *const, const MimePrtNode * *const);


#endif /* ] Included_nPGPNts_h */
