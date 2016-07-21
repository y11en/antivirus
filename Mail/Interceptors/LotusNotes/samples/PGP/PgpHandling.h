/*______________________________________________________________________________
	Copyright (C) 2002-2003 PGP Corporation
	All rights reserved.
	

	$Id: PgpHandling.h 15552 2003-07-14 09:59:47Z pryan $
______________________________________________________________________________*/

#ifndef Included_PgpHandling_h	/* [ */
#define Included_PgpHandling_h

//system includes
#include <stdio.h>				//for sprintf()
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#undef WIN32_LEAN_AND_MEAN
#include <crtdbg.h>

//PGPsdk includes
#include <PGPsc.h>
#include <pgpErrors.h>
#include <pgpMem.h>
#include <pgpClientLib.h>
#include <pgpVersionHeader.h>
#include <pgpPFLPriv.h>			//for PGPValidateParam()
#include <pgpBuild.h>			//for PGPVERSIONMAJOR, PGPVERSIONMINOR
#include <pgpWin32Errors.h>		//for kPGPError_Win32_Expired
#include <pgpClientPrefs.h>		//for prefs token definitions like 
								//	kPGPPrefDecryptCacheEnable; also pgpBase.h 
								//	definitions like INT_MAX

//PGP shared client-code includes
#include <DecryptVerify.h>
#include <EncryptSign.h>
#include <Working.h>
#include <WorkingResource.h>
#include <Prefs.h>				//for utility procedures like ByDefaultSign()
#include <SharedStrings.h>
#include <SigEvent.h>
#include <VerificationBlock.h>
#include <BlockUtils.h>			//for FindEncryptedBlock()
#include <pgpLnLib.h>			//for kPGPlnOperationGraceExpiration

//local includes
#include "VbCallable.h"
#include "LibFileSystem.h"		//for ei_FileAccess()
#include "LibLinkList.h"		//for e_FreeList(), ef_AddListNodeUnique()


typedef struct	{
	HINSTANCE  h_Instance;
	HWND  h_wndMain;
	PGPContextRef  pgpContext;
	PGPtlsContextRef  pgpTlsContext;
	char *  pc_AppNm;
	const char *  pc_ModuleNm;
}  PgpBasicInput;

typedef enum	{
	mi_TEST_FOR_PGP_ASCII = 1, 
	mi_GET_PGP_BLOCK_COORDS, 
	mi_NO_TASK = 0
}  SpecialPlugInTask;

typedef struct	{
	SpecialPlugInTask  i_Task;
	BOOL * pf_Successful;
	void * pv_ctx;
}  SpecialPlugInTaskInfo;

typedef struct	{
	DWORD  ul_offsetBegin;
	DWORD  ul_offsetEnd;
	BOOL  f_tmpst;
}  PgpBlockCoordInfo;

typedef struct _KeyIdNode	{
	PGPKeyID * pt_kyId;
	struct _KeyIdNode * pt_nxt;
}  KeyIdNode;

typedef struct	{
	BOOL  f_Sign;
	BOOL  f_Encrypt;
	BOOL  f_asciiOnlyBdy;
	KeyIdNode * pt_missngAdks;
	PGPclRecipientDialogStruct  t_recipients;
	PGPOptionListRef  pgpUserOptions;
}  PgpEncodeContext;

//structure copied from shared-code PGP module DecryptVerify.c
typedef struct _VerificationBlock	{
	struct _VerificationBlock * next;
	struct _VerificationBlock * previous;
	char * szBlockBegin;
	char * szBlockEnd;
	void * pOutput;
	PGPSize  outSize;
	PGPBoolean  bEncrypted;
	PGPBoolean  FYEO;
}  VerificationBlock;

//structure copied from shared-code PGP module DecryptVerify.c
typedef struct	{
	HINSTANCE  h_Instance;
	HWND  h_wndMain;
	HWND  h_wndWorking;
	PGPtlsContextRef  pgpTlsContext;
	char * pc_AppNm;
	PGPKeyDBRef  keyDB;
	PGPKeyDBRef  keydbNew;
	PGPKeySetRef  recipients;
	PGPUInt32  ul_keyCount;
	PGPKeyID * pt_keyIds;
	VerificationBlock *	pt_block;
	PGPByte  contents;
	PGPFileSpecRef  fsrFileRef;
}  DecodeEventData;

typedef struct	{
	DecodeEventData  t_normalData;
	SpecialPlugInTaskInfo  t_task;
}  MyDecodeEventData;

typedef struct _NameFoundNode	{
	struct	{							//avoiding inclusion of Notes modules
		char * pc_nm;
		BOOL  f_found;
		BYTE  opaque[ sizeof( void *)];	//allowance for struct NameFoundInfoEx
	} * pt_nm;
	struct _NameFoundNode * pt_nxt;
}  NameFoundNode;


extern const char  epc_CRLF[];
extern const int  ei_NOT_FOUND, ei_SAME;
extern const size_t  eui_LEN_CRLF;
extern const WORD  eus_ERR_INVLD_ARG;

extern HINSTANCE  eh_Instance;

//prototype copied from DecryptVerify.c
PGPError DecodeEventHandler( PGPContextRef, PGPEvent *, PGPUserValue);
//prototype copied from EncryptSign.c
PGPError EncodeEventHandler( PGPContextRef, PGPEvent *, PGPUserValue);

static PGPError i_DecodeEventHandlerOverride( PGPContextRef, PGPEvent *, 
																PGPUserValue);
static void DisplayPgpError( const char *const, const int, const char *const, 
																		int);
static PGPError i_DecryptVerify( const PgpBasicInput *const, PGPOptionListRef, 
									const BOOL, const SpecialPlugInTaskInfo, 
									VerificationBlock *const, 
									const BOOL, BOOL *const);
static PGPError i_DecryptVerifyBuffer( const PgpBasicInput *const, 
										const char *const, const DWORD, 
										const BOOL, const BOOL, const BOOL, 
										SpecialPlugInTaskInfo, void * *const, 
										PGPSize *const, BOOL *const);
static PGPError i_PgpAsciiCoordsOrTempest( char * *const, const DWORD, 
													DWORD *const, DWORD *const, 
													char * *const, int *const);
static PGPError i_EncryptSignBuffer( const PgpBasicInput *const, void *, 
									DWORD, PGPclRecipientDialogStruct *const, 
									PGPOptionListRef, PGPOptionListRef *, 
									void * *, PGPSize *, BOOL, BOOL, BOOL);
static PGPError i_EncryptSign( const PgpBasicInput *const, 
										PGPclRecipientDialogStruct *, 
										PGPOptionListRef, PGPOptionListRef, 
										PGPOptionListRef *, BOOL, BOOL, BOOL);
static PGPError i_NoteMissingAdks( const PGPKeySetRef, KeyIdNode * *const);


#endif /* ] Included_PgpHandling_h */
