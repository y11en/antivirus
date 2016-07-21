/*______________________________________________________________________________
	Copyright (C) 2002-2003 PGP Corporation
	All rights reserved.


	$Id: LibObject.h 15326 2003-06-27 08:54:16Z pryan $
______________________________________________________________________________*/

/*::: FUNCTIONAL OVERVIEW ::::::::::
Facilitates object handling within a Notes database.

--- revision history ----
3/14/03 Version 1.3 Paul Ryan
+ minor signature adjustment to eus_getObjectContentsInBuffer() to provide 
  appropriate constness
+ added eus_objCopy()

9/6/02 Version 1.2 Paul Ryan
+ added eus_getAttachInfoNext()
+ extended eus_getAttachmentInfo() to allow output of object host type

8/9/00 Version 1.1.2 Paul Ryan
+ added eus_AttachFileAsObject()

9/12/99 Version 1.1: Paul Ryan
+ genericized eus_getObjectContentsInBuffer()
+ added compression-flag return to eus_getAttachmentInfo()

12/10/98 Version 1.0: Paul Ryan
::::::::::::::::::::::::::::::::::::*/

#ifndef Included_LibObject_h	/* [ */
#define Included_LibObject_h


typedef struct	{
	BYTE  opaque[ 4 * sizeof( int)];
}  ObjInfo;


extern char  epc_ITMNM_STD_ATTCH[];


STATUS eus_PrepObjectWrite( const DBHANDLE, const DWORD, ObjInfo *const);
STATUS eus_AppendToObject( const HANDLE, const DWORD, ObjInfo *const);
STATUS eus_CommitObject( const NOTEHANDLE, const char[], const char[], 
												const WORD, const HANDLE, 
												const DWORD, ObjInfo *const);
STATUS eus_releaseObject( ObjInfo *const);
STATUS eus_AttachBufferAsObject( const BYTE *const, const DWORD, DBHANDLE, 
								NOTEHANDLE, char[], const char[], const WORD);
STATUS eus_getAttachmentInfo( const NOTEHANDLE, const char[], const char[], 
								DWORD *const, DWORD *const, BOOL *const, 
								WORD *const, BLOCKID *const, BLOCKID *const);
STATUS eus_getAttachInfoNext( const NOTEHANDLE, const char[], BLOCKID *const, 
									char[], DWORD *const, DWORD *const, 
									BOOL *const, WORD *const, BLOCKID *const);
STATUS eus_getObjectContentsInBuffer( const DWORD, const BLOCKID, const DWORD, 
												const NOTEHANDLE, 
												const DBHANDLE, HANDLE *const);
STATUS eus_AttachFileAsObject( const char[], const DBHANDLE, const NOTEHANDLE, 
											char[], const char[], const WORD);
STATUS eus_DeleteAttachment( const NOTEHANDLE, const char[], char[]);
STATUS eus_objCopy( const DBHANDLE, const DBHANDLE, const DWORD, const WORD, 
																DWORD *const);


#endif /* ] Included_LibObject_h */
