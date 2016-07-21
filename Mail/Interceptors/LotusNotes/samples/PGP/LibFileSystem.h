/*______________________________________________________________________________
	Copyright (C) 2002-2003 PGP Corporation
	All rights reserved.


	$Id: LibFileSystem.h 15326 2003-06-27 08:54:16Z pryan $
______________________________________________________________________________*/

/*::: FUNCTIONAL OVERVIEW ::::::::::
Declares utility functions useful in file-system handling. Assumes caller does 
not necessarily have the stdio.h definitions like FILE.

--- revision history ----
3/14/03 Version 1.3 Paul Ryan
+ added buffered read-file functionality [see ef_bufferFileForRead() for 
  starters]
+ added ept_createTmpFile()

9/6/02 Version 1.1 Paul Ryan
+ added ef_WriteMemToFile()

9/16/00 Version 1.0.1 Paul Ryan
+ added ei_FileAccess() wrapper for ANSI access() function

8/6/99 Version 1.0: Paul Ryan
::::::::::::::::::::::::::::::::::::*/

#ifndef Included_LibFileSystem_h	/* [ */
#define Included_LibFileSystem_h


#include <stdio.h>			//for FILE, etc.


extern const int  ei_ERR_ACCESS, ei_ERR_EXIST, ei_FILE_ACCS_EXIST, 
					ei_FILE_ACCS_WRITE, ei_FILE_ACCS_READ, 
					ei_FILE_ACCS_READWRITE;

typedef struct	{
	BYTE * puc;				//address of where the entire buffer starts (prefix 
					//	buffer plus file buffer)
	size_t  ui_lenPrfx;		//length of the prefix portion of the buffer
	size_t  ui_lenBuf;		//length of the file buffer
	BYTE  opaque[ 6 * sizeof( long)];
}  BufFile;

char * epc_getTempDirNm( char *const, const unsigned int);
BOOL ef_LoadTextFileEntire( const char[], char * *const);
BOOL ef_LoadBinaryFile( const char[], void *const, const int);
int ei_FileAccess( const char[], const int, const BOOL);
BOOL ef_WriteMemToFile( const unsigned char *const, const unsigned long, 
																const char[]);
BOOL ef_closeFile( void * *const);
long el_addToOpenFile( void *const, const BYTE *const, const size_t, 
																	const BOOL);
void * ept_openFile( const char[], const char[]);
BOOL ef_closeAndDeleteFile( void *const  pf, const char[]);
void * ept_createTmpFile( const BYTE *const, const size_t);
long el_bufFileSeekStr( const char[], const long, const long, BufFile *const);
BOOL ef_bufferFileForRead( void *const, const char[], const size_t, 
												const size_t, BufFile *const);
BOOL ef_base64encodeFile( const char[], const char[], void * *const);
int ei_appendFileContent( void *, void *, const char[], const char[], 
																const size_t);
BOOL ef_writeToNewFile( const unsigned char *const, const unsigned long, 
												const char[], void * *const);
BOOL ef_bufFileClose( BufFile *const);
long el_bufFileCpyCtnt( const long, size_t  ui_len, BYTE *const, 
																BufFile *const);
BOOL ef_bufFileSpliceToPrfx( const long, size_t  ui_len, BufFile *const);
long ei_fileLength( void *const);


#endif /* ] Included_LibFileSystem_h */
