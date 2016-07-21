/*____________________________________________________________________________
	Copyright (C) 2002-2003 PGP Corporation
	All rights reserved.


	$Id: DbAndNoteHandling.h 12258 2003-02-13 23:57:46Z sdas $
____________________________________________________________________________*/

#ifndef Included_DbAndNoteHandling_h	/* [ */
#define Included_DbAndNoteHandling_h


//system includes
#include <stdlib.h>		//for free()
#include <crtdbg.h>		//for _ASSERTE()

//Notes API includes
#include <global.h>
#include <nsfdb.h>
#include <nsfnote.h>
#include <nif.h>		//for NIFFindDesignNote(), NIFOpenCollection(), etc.
#include <osmem.h>		//for OSLockObject(), etc.
#include <osfile.h>		//for OSPathNetConstruct()
#include <nsferr.h>		//for ERR_DIRECTORY
#include <miscerr.h>	//for ERR_NOT_FOUND
#include <stdnames.h>	//for DESIGN_CLASS

#include "LibItem.h"


extern const STATUS  eus_SUCCESS;
extern const BLOCKID  ebid_NULLBLOCKID;


STATUS eus_getTemplateClass( DBHANDLE, char *const);
STATUS eus_TestDbIsTemplate( DBHANDLE, char *const);
STATUS eus_OpenDb( char[], char[], DBHANDLE *const);
STATUS eus_OpenMailDb( char[], char[], DBHANDLE *const);
STATUS eus_GetDesignNote( DBHANDLE, const WORD, char *const, NOTEID *const);


#endif /* ] Included_DbAndNoteHandling_h */
