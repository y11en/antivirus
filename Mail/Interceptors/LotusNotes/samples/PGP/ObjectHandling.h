/*______________________________________________________________________________
	Copyright (C) 2002-2003 PGP Corporation
	All rights reserved.


	$Id: ObjectHandling.h 15326 2003-06-27 08:54:16Z pryan $
______________________________________________________________________________*/

#ifndef Included_ObjectHandling_h	/* [ */
#define Included_ObjectHandling_h


//system includes
#include <sys/types.h>	//for struct _stat
#include <sys/stat.h>	//for _stat()
#include <stdlib.h>		//for free(), etc.
#include <stdio.h>		//for remove()
#include <io.h>			//for filelength()
#include <crtdbg.h>

#include "NotesApiGlobals.h"
//Notes API includes
#include <nsfdb.h>		//for NSFDbReadObject(), etc.
#include <nsfnote.h>
#include <nsfobjec.h>
#include <ostime.h>		//for OSCurrentTIMEDATE()
#include <nsferr.h>
#include <stdnames.h>	//for ITEM_NAME_ATTACHMENT

#include "LibFileSystem.h"


typedef struct	{
	DWORD  ul_id;
	DWORD  ul_len;
	DWORD  ul_wrtn;
	DBHANDLE  h_db;
}  ObjInfo;


extern const STATUS  eus_SUCCESS, eus_ERR_INVLD_ARG;


static STATUS us_AttachObjectItem( const NOTEHANDLE, const DWORD, const char[], 
										const char[], const DWORD, const WORD);
STATUS eus_AppendToObject( const HANDLE, const DWORD, ObjInfo *const);


#endif /* ] Included_ObjectHandling_h */
