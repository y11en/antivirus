/*____________________________________________________________________________
	Copyright (C) 2002 PGP Corporation
	All rights reserved.

	$Id: TranslatorUtils.h 20209 2004-01-25 22:24:43Z wjb $
____________________________________________________________________________*/
#ifndef Included_TranslatorUtils_h	/* [ */
#define Included_TranslatorUtils_h

#define DELIMITERLENGTH 256

#include "emssdk/ems-win.h"

long ExtractAddressesFromMailHeader(
	emsHeaderDataP header, 
	char*** addresses
	);

void 
FreeRecipientList(
	char** Recipients, 
	unsigned long  numRecipients
	);

void 
StripMIMEHeader(
	const PGPChar* path
	);


#endif /* ] Included_TranslatorUtils_h */


/*__Editor_settings____

	Local Variables:
	tab-width: 4
	End:
	vi: ts=4 sw=4
	vim: si
_____________________*/