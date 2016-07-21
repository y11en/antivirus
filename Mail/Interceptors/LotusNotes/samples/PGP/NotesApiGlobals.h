/*______________________________________________________________________________
	Copyright (C) 2002-2003 PGP Corporation
	All rights reserved.


	$Id: NotesApiGlobals.h 15326 2003-06-27 08:54:16Z pryan $
______________________________________________________________________________*/

/*::: FUNCTIONAL OVERVIEW ::::::::::
Provides global declarations related to Process Stream Technologies' use of 
the Notes API.

--- revision history --------
3/14/03 Version 1.0.2 Paul Ryan
+ added osmem.h include

9/16/00 Version 1.0.1 Paul Ryan
+ adjusted to support universal error codes included in PstGlobals.h
+ added this standard documentation

1/5/99 Version 1.0 Paul Ryan
::::::::::::::::::::::::::::::::::::*/

#ifndef Included_NotesApiGlobals_h	/* [ */
#define Included_NotesApiGlobals_h


//Notes API includes
#include <global.h>
#include <pool.h>				//for BLOCKID
#include <osmem.h>				//for OSLock()

#include "PstGlobals.h"


extern const BLOCKID  ebid_NULLBLOCKID;


#endif /* ] Included_NotesApiGlobals_h */
