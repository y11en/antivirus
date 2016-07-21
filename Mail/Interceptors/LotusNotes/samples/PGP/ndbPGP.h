/*______________________________________________________________________________
	Copyright (C) 2002-2003 PGP Corporation
	All rights reserved.


	$Id: ndbPGP.h 15326 2003-06-27 08:54:16Z pryan $
______________________________________________________________________________*/

#ifndef Included_ndbPGP_h	/* [ */
#define Included_ndbPGP_h


//C run-time includes
#include <stdlib.h>
#include <crtdbg.h>

//Notes API includes
#include <global.h>
#include <pool.h>		//for LibItem.h
#include <names.h>		//for MAXPATH
#include <osmisc.h>		//for OSLoadString()
#include <osmem.h>		//for OSMemAlloc()
#include <dbdrv.h>
#include <dbdrverr.h>	//for ERR_DBD_FUNCTION

#include "LibItem.h"


extern const STATUS  eus_SUCCESS, eus_ERR_INVLD_ARG;
extern const int  ei_USER_ABORT;


void e_ReleasePgpContext( void);
void e_FreeGdiResources( void);
int ei_InitializePgpContext( void);
STATUS eus_PreMailSend( void *const [], HANDLE *const, DWORD *const);
short es_ReplaceMimeWithRtf( void *const []);
short es_SpecialDecryptAttachment( void *const []);
STATUS eus_TestNoteForPgpCtnt( void *const [], HANDLE *const, DWORD *const);

static STATUS LNPUBLIC us_Terminating( DBVEC *);
static STATUS LNPUBLIC us_dmyOpen( DBVEC *, HDBDSESSION *);
static STATUS LNPUBLIC us_dmyClose( DBVEC *, HDBDSESSION);
static STATUS LNPUBLIC us_PlugInCommand( DBVEC *, HDBDSESSION, WORD, WORD, 
										DWORD *, void * *, HANDLE *, DWORD *);


#endif /* ] Included_ItemHandling_h */
