/*______________________________________________________________________________
	Copyright (C) 2002-2003 PGP Corporation
	All rights reserved.


	$Id: PstGlobals.h 15326 2003-06-27 08:54:16Z pryan $
______________________________________________________________________________*/

/*::: FUNCTIONAL OVERVIEW ::::::::::
Provides global declarations defined by Process Stream Technologies for 
universal use.

--- revision history --------
3/14/03 Version 1.0.1.2 Paul Ryan
+ moved all definitions into newly created PstGlobals.c
+ removed ec_SPACE (and into LibString.h)
+ added eus_ERR_MEM, epc_PATH_UNC_PRFX, eui_LEN_PATH_UNC_PRFX

9/6/02 Version 1.0.1.1 Paul Ryan
+ listing format adjustment

9/16/00 Version 1.0.1 Paul Ryan
+ adjusted to include certain universal error codes
+ added this standard documentation

1/5/99 Version 1.0 Paul Ryan
::::::::::::::::::::::::::::::::::::*/

#ifndef Included_PstGlobals_h	/* [ */
#define Included_PstGlobals_h


extern const char  epc_NULL[], epc_CRLF[], ec_PATH_SPECIFIER, 
					epc_PATH_SPECIFIER[], epc_PATH_UNC_PRFX[];
extern const int  ei_NOT_FOUND, ei_FAIL, ei_SAME;
extern const size_t  eui_LEN_CRLF, eui_LEN_PATH_UNC_PRFX;

extern const unsigned short  eus_SUCCESS, eus_ERR_INVLD_ARG, eus_ERR_MEM;


#endif /* ] Included_PstGlobals_h */
