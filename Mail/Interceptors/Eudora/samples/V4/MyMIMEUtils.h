/*____________________________________________________________________________
	Copyright (C) 2002 PGP Corporation
	All rights reserved.

	$Id: MyMIMEUtils.h 20209 2004-01-25 22:24:43Z wjb $
____________________________________________________________________________*/
#ifndef Included_MyMIMEUtils_h	/* [ */
#define Included_MyMIMEUtils_h


 
#include "emssdk/ems-win.h"

long ParseFileForMIMEType(
	const PGPChar *		szInputFile,
	emsMIMEtype**		ppMimeType
);

void ParseMIMEType( 
	const char* szText, 
	emsMIMEtype** ppMimeType
);


#endif /* ] Included_MyMIMEUtils_h */


/*__Editor_settings____

	Local Variables:
	tab-width: 4
	End:
	vi: ts=4 sw=4
	vim: si
_____________________*/

