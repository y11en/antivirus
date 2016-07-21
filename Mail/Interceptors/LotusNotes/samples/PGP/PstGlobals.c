/*______________________________________________________________________________
	Copyright (C) 2002-2003 PGP Corporation
	All rights reserved.


	$Id: PstGlobals.c 15326 2003-06-27 08:54:16Z pryan $
______________________________________________________________________________*/

#include <stddef.h>				//for size_t

char  epc_NULL[] = {'\0'};
const char  epc_CRLF[] = "\r\n", 
			ec_PATH_SPECIFIER = '\\', 
			epc_PATH_SPECIFIER[] = "\\", 
			epc_PATH_UNC_PRFX[] = "\\\\";
const int  ei_NOT_FOUND = -1, ei_FAIL = -1, 
			ei_SAME; 				//used with "cmp" functions to signify 
								//	sameness
const size_t  eui_LEN_CRLF = sizeof( epc_CRLF) - 1, 
				eui_LEN_PATH_UNC_PRFX = sizeof( epc_PATH_UNC_PRFX) - 1;

const unsigned short  eus_SUCCESS = (unsigned short) NULL,	//same as NOERROR
						eus_ERR_INVLD_ARG = 0x0241, //same as ERR_BAD_PARAM
						eus_ERR_MEM = 0x0107;		//same as ERR_MEMORY
