/*____________________________________________________________________________
	Copyright (C) 2002 PGP Corporation
	All rights reserved.

	$Id: MyMIMEUtils.c 9679 2002-08-06 20:11:24Z dallen $
____________________________________________________________________________*/

// System Headers
#include <windows.h>

// Eudora Headers
#include "emssdk/mimetype.h"

// Project Headers
#include "MapFile.h"
#include "MyMIMEUtils.h"



	long
CreateMIMEParam(
	const char*				nameString,
	const char*				valueString,
	emsMIMEParamP *			ppMimeParam )
{
	emsMIMEParamP		pParam = NULL;
	long				err	= 0;
	
	pParam = (emsMIMEParamP) malloc ( sizeof(emsMIMEparam) );

	if ( pParam )
	{
		pParam->name	= strdup( nameString );
		pParam->value	= strdup( valueString );
		pParam->next	= NULL;
		
	}
	else
	{
		err	= 1;
	}
	
	*ppMimeParam	= pParam;
	return( err );
}


			
	long
AddMIMEParam(
	emsMIMEtypeP		pMimeType,
	const char*			nameString,
	const char*			valueString
)
{
	long				err	= 0;
	emsMIMEParamP		pParam = NULL;
	
	err	= CreateMIMEParam( nameString, valueString, &pParam );

	if ( ! err ) 
	{
		if ( !pMimeType->params ) 
		{
			pMimeType->params	= pParam;
		}
		else
		{
			emsMIMEParamP	pCurParam;
	
			// now find the last param, and add the new param to it
			pCurParam	= pMimeType->params;

			while ( pCurParam )
			{
				if ( !pCurParam->next ) 
				{
					pCurParam->next	= pParam;
					break;
				}
				
				pCurParam	= pCurParam->next;
			}
		}
	}
	 
	return( err );
}


	long
CreateMIMEType(
	const char*				typeString,
	const char*				subtypeString,
	emsMIMEtypeP *			ppMimeType
)
{
	emsMIMEtypeP	pTempMimeType;
	long			err	= 0;

	pTempMimeType = (emsMIMEtypeP) malloc ( sizeof(emsMIMEtype) );

	if(pTempMimeType)
	{
		pTempMimeType->mime_version = strdup("1.0");
		pTempMimeType->mime_type = strdup(typeString);
		pTempMimeType->sub_type = strdup(subtypeString);
		pTempMimeType->params	= NULL;
	}
	
	*ppMimeType = pTempMimeType;
	return( err );
}

	
	void
DisposeMIMEType( emsMIMEtypeP	pMimeType)
{
	emsMIMEparam*	pParams;

	if(pMimeType->mime_version)
		free(pMimeType->mime_version);

	if(pMimeType->mime_type)
		free(pMimeType->mime_type);

	if(pMimeType->sub_type)
		free(pMimeType->sub_type);
	
	pParams	= pMimeType->params;
	
	while ( pParams )
	{
		emsMIMEparam*	next;
		
		next	= pParams->next;
		free(pParams);
		pParams	= next;
	}	
}


/*___________________________________________________________________________
	Build a Eudora data structure representing
	multipart/encrypted; boundary=foo; protocol="application/pgp-encrypted"
___________________________________________________________________________*/
	long
BuildEncryptedPGPMIMEType(
	emsMIMEtypeP *	ppMimeType,
	const char *	boundaryString )
{
	long				err	= 0;
	emsMIMEtypeP		pMimeType = NULL;

	err	= CreateMIMEType( "multipart", "encrypted", &pMimeType );
	if ( ! err ) 
	{		
		err	= AddMIMEParam( pMimeType, "boundary", boundaryString );
		
		if (! err ) 
		{
			err	= AddMIMEParam( pMimeType, 
								"protocol", 
								"application/pgp-encrypted" );
		}
		
		if (  err  )
		{
			DisposeMIMEType( pMimeType );
			pMimeType = NULL;
		}
	}
	
	*ppMimeType = pMimeType;
	
	return( err );
}


/*__________________________________________________________________________
	Build a Eudora data structure representing
	multipart/signed; boundary=foo; protocol="application/pgp-signature"
___________________________________________________________________________*/
	long
BuildSignedPGPMIMEType(
	emsMIMEtypeP *	ppMimeType,
	const char *	boundaryString )
{
	long				err	= 0;
	emsMIMEtypeP		pMimeType = NULL;
	
	err	= CreateMIMEType( "multipart", "signed", &pMimeType );
	if (! err ) 
	{				
		err	= AddMIMEParam( pMimeType, "boundary", boundaryString );
	
		if (! err ) 
		{
			err	= AddMIMEParam( pMimeType, 
								"protocol", 
								"application/pgp-signature" );
		}
		
		if (! err ) 
		{
			err	= AddMIMEParam( pMimeType, "micalg", "pgp-md5" );
		}

		
		if (  err )
		{
			DisposeMIMEType( pMimeType );
			pMimeType	= NULL;
		}
	}
	
	*ppMimeType = pMimeType;
	
	return( err );
}


/*__________________________________________________________________________
	*ppMimeType is set to NULL if no valid Content-type is found.
___________________________________________________________________________*/
	
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
		(*ppMimeType)->mime_version = strdup( "1.0");
	}
}

long
ParseFileForMIMEType(
	const char *		szInputFile,
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



	BOOL
GetMIMEParameter(
	emsMIMEtype*			pMimeType,
	const char*				paramName,
	char*					outValue )
{
	emsMIMEparam*	pParams;
	
	*outValue = 0x00;
	
	pParams	= (*pMimeType).params;

	while ( pParams )
	{
		if ( !strcmp( (*pParams).name, paramName) )
		{
			strcpy( outValue, (*pParams).value);
			break;
		}
		pParams	= (*pParams).next;
	}

    return( strlen( outValue ) != 0 );
}	



/*__Editor_settings____

	Local Variables:
	tab-width: 4
	End:
	vi: ts=4 sw=4
	vim: si
_____________________*/
