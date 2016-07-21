/*______________________________________________________________________________
	Copyright (C) 2002-2003 PGP Corporation
	All rights reserved.


	$Id: FileSystemHandling.h 15326 2003-06-27 08:54:16Z pryan $
______________________________________________________________________________*/

#ifndef Included_FileSystemHandling_h	/* [ */
#define Included_FileSystemHandling_h

//system includes
#include <string.h>
#include <stdlib.h>		//for getenv()
#include <stdio.h>		//for fopen(), etc.
#include <io.h>			//for filelength(), access(), etc.
#include <fcntl.h>		//for _O_RDONLY
#include <share.h>		//for _SH_DENYRW
#include <errno.h>		//for EACCES, etc.

typedef unsigned char  BYTE;

typedef enum {
	FALSE, 
	TRUE
}  BOOL;

//project includes
#include "PstGlobals.h"	//for ei_SAME
#include "base64.h"		//for f_base64decode()


#define MAXBYTE  0xFF

typedef struct	{
	BYTE * puc;				//address of where the entire buffer starts (prefix 
					//	buffer plus file buffer)
	size_t  ui_lenPrfx;		//length of the prefix portion of the buffer
	size_t  ui_lenBuf;		//length of the file buffer
	FILE * pfl;				//address of structure describing file object being 
					//	bufferred
	BYTE * puc_strt;		//address of where the file-buffer portion of the 
					//	entire buffer starts
	BYTE * puc_bgn;			//address of the beginning of valid content in the 
					//	file buffer
	long  l_bgn;			//offset to location in file where the first byte 
					//	currently bufferred resides
	BYTE * puc_end;			//address of the end of valid content in the file 
					//	buffer
	long  l_end;			//offset to location in file where the last byte 
					//	currently bufferred resides
}  BufFile;


#endif /* ] Included_FileSystemHandling_h */
