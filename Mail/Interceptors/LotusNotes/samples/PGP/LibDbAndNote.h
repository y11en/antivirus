/*____________________________________________________________________________
	Copyright (C) 2002-2003 PGP Corporation
	All rights reserved.
	

	$Id: LibDbAndNote.h 12258 2003-02-13 23:57:46Z sdas $
____________________________________________________________________________*/

/*::: FUNCTIONAL OVERVIEW ::::::::::
Reüsable, common Notes database- & template-handling library.

--- revision history ---------
2/22/00 Version 1.0.2
+ addition of eus_GetDesignNote() to accommodate location of design notes by 
  alias name if can't be located by main title
+ documentation adjustment

9/9/99 Version 1.0: Paul Ryan
::::::::::::::::::::::::::::::::::::*/

#ifndef Included_LibDbAndNote_h	/* [ */
#define Included_LibDbAndNote_h


//arbitrary codes within Lotus' PKG_UNDERUTIL1 error-code package, used as 
//	library-specific error codes here
#define mi_ERR_NOT_MAIL_DB  0x06D6
#define mi_ERR_NOT_TEMPLATE  0x06D7


STATUS eus_TestDbIsTemplate( DBHANDLE, char *const);
STATUS eus_OpenTemplate( char[], char[], DBHANDLE *const, char *const);
STATUS eus_OpenMailTemplate( char[], char[], DBHANDLE *const, char *const);
STATUS eus_OpenDb( char[], char[], DBHANDLE *const);
STATUS eus_OpenMailDb( char[], char[], DBHANDLE *const);
STATUS eus_getTemplateClass( DBHANDLE, char *const);
STATUS eus_getDbDesignClass( DBHANDLE, char *const);
STATUS eus_ReplaceDesignNote( DBHANDLE, const WORD, char[], DBHANDLE, 
										const NOTEID, char[], NOTEID *const);
STATUS eus_RemoveDesignNote( DBHANDLE, const WORD, char[]);
STATUS eus_GetDesignNote( DBHANDLE, const WORD, char *const, NOTEID *const);


#endif /* ] Included_LibDbAndNote_h */
