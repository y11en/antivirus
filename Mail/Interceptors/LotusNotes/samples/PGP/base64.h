/*______________________________________________________________________________
	Copyright (C) 2002-2003 PGP Corporation
	All rights reserved.


	$Id: base64.h 15326 2003-06-27 08:54:16Z pryan $
______________________________________________________________________________*/


#ifndef Included_base64_h	/* [ */
#define Included_base64_h


BOOL f_base64encode( unsigned char * *const, size_t *const, 
												const unsigned char *, size_t);
BOOL f_base64decode( unsigned char * *const, size_t *const, 
														const unsigned char *);


#endif /* ] Included_base64_h */
