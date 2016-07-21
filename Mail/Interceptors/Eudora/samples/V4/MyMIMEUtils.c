/*____________________________________________________________________________
	Copyright (C) 2002 PGP Corporation
	All rights reserved.

	$Id: MyMIMEUtils.c 20209 2004-01-25 22:24:43Z wjb $
____________________________________________________________________________*/

// System Headers
#include <windows.h>

// Eudora Headers
#include "emssdk/mimetype.h"

// Project Headers
#include "pgpBase.h"
#include "MapFile.h"
#include "MyMIMEUtils.h"


/*_______________________________________________________________
	*ppMimeType is set to NULL if no valid Content-type is found.
________________________________________________________________*/

	void
ParseMIMEType(
	const char*			szText,
	emsMIMEtype**	ppMimeType
)
{
	*ppMimeType	= NULL;
	
	*ppMimeType = parse_make_mime_type(szText);

	if ( *ppMimeType )
	{
		// parse_mime_ct doesn't fill in mime_version
		(*ppMimeType)->version = strdup( "1.0");
	}
}

long
ParseFileForMIMEType(
	const PGPChar *		szInputFile,
	emsMIMEtype**		ppMimeType
)
{
	long err = 0;
	char* buffer = NULL;
	DWORD length = 0;

	buffer	= MapFile( szInputFile, &length );
	
	if ( buffer )
	{
		ParseMIMEType( buffer, ppMimeType);

		free(buffer);
	}
	else
	{
		err = 1;
	}
	
	return( err );
}


/*__Editor_settings____

	Local Variables:
	tab-width: 4
	End:
	vi: ts=4 sw=4
	vim: si
_____________________*/

