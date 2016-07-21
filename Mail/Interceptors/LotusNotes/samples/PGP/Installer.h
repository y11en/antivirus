/*______________________________________________________________________________
	Copyright (C) 2002-2003 PGP Corporation
	All rights reserved.


	$Id: Installer.h 15637 2003-07-22 16:43:18Z pryan $
______________________________________________________________________________*/

#ifndef Included_Installer_h	/* [ */
#define Included_Installer_h


//system includes
#include <windows.h>	//for FindWindow(), etc.
#include <stdlib.h>		//for free(), etc.
#include <stdio.h>		//for sprintf()
#include <io.h>			//for filelength()
#include <crtdbg.h>		//for _ASSERTE(), etc.

#include "NotesApiGlobals.h"
//Notes API includes
#include <nsfdb.h>
#include <nsfnote.h>
#include <nif.h>			//for NIFFindDesignNote()
#include <osfile.h>			//for OSPathNetConstruct()
#include <kfm.h>			//for SECKFMGetUserName()
#include <acl.h>			//for ACLEnumEntries()
#include <osmem.h>			//for OSMemFree()
#include <misc.h>			//for TIMEDATE, etc.
#include <stdnames.h>		//for DESIGN_FLAGS, etc.
#include <osmisc.h>			//for OSLoadString()
#include <nsferr.h>			//for ERR_CANT_CHANGE_UNID, ERR_DIRECTORY
#include <oserr.h>			//for ERR_NOEXIST
#include <miscerr.h>		//for ERR_NOT_FOUND

#include "LibDbAndNote.h"	//for eus_ReplaceDesignNote(), etc.
#include "LibRichText.h"
#include "LibItem.h"		//for eus_RemoveItem()


typedef struct	{
	DBHANDLE  h_template;
	char  pc_extFileNmTemplate[ MAXPATH];
	char  pc_UserNm[ MAXUSERNAME];
	char  pc_DbClass[ NSF_INFO_SIZE];
	NOTEID  nid_Memo;
	NOTEID  nid_Reply;
	NOTEID	nid_ReplyWH;
	NOTEID  nid_Default;
	NOTEID  nid_PgpSbfrmMain;
	NOTEID  nid_PgpSbfrmDll;
}  EnvInfo;

typedef struct	{
	char * pc_name;
	BOOL  f_found;
	WORD  us_level;
	WORD  us_flags;
}  AclEntryInfo;


static STATUS us_CompileUsersAclNamesList( const char[], char[], 
														NAMES_LIST * *const);
static STATUS us_PlacePgpTemplate( const char[], const char[], const char[], 
												const DBHANDLE, const char[], 
												DBHANDLE *const, char *const);
static STATUS us_TestPgpTemplatePlaced( const char[], const char[], 
												const char[], DBHANDLE *const, 
												char *const, char *const);
static STATUS us_LookupAclEntry( char[], HANDLE, BOOL *const, WORD *const, 
																WORD *const);
static void LNCALLBACK VetAclEntry( void *, char *, WORD, ACL_PRIVILEGES *, 
																		WORD);
static STATUS us_RefreshPgpTemplate( const DBHANDLE, const char[], 
									const char[], NOTEID *const, NOTEID *const);
static STATUS us_PrepPgpTemplateProductionDesign( const DBHANDLE, const BOOL, 
								const char[], NOTEID *const, NOTEID *const, 
								NOTEID *const, NOTEID *const, NOTEID *const);
static STATUS us_PgpEnableEmailForm( char[], const BOOL, DBHANDLE, DBHANDLE, 
								const BOOL, /*NOTEHANDLE, */NOTEID *const);
static STATUS us_InsertSubformTop( NOTEHANDLE, /*NOTEHANDLE, */char[]);
static STATUS us_GetSourceEnvInfo( const char[], const char[], EnvInfo *const);
static STATUS us_InstallPlugInTemplate( const char[], const char[], 
												const char[], EnvInfo *const);
static STATUS us_RefreshMailDbs( const char[], const char[], char[], 
										const EnvInfo *const, char * *const);
static STATUS us_RefreshMailDb( const DBHANDLE, const EnvInfo *const);
static __inline BOOL f_StartMereErrLog( char * *const);
static BOOL f_LoadTextFileEntire( const char[], char * *const);
static STATUS us_OpenAndValidateMailDb( const char[], const char[], 
												char * *const, DBHANDLE *const);
static STATUS us_RollBackMailDbs( DBHANDLE, char[], char[], char[], 
										const char[], char *, char * *const);
static STATUS us_RollBackMailDb( const DBHANDLE, const EnvInfo *const, 
												const char[], char * *const);
static STATUS us_OpenMailTemplate( const char[], const char[], DBHANDLE *const, 
																char *const);


#endif /* ] Included_Installer_h */
