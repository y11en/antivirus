/*____________________________________________________________________________
	Copyright (C) 2002 PGP Corporation
	All rights reserved.

	$Id: TranslatorUtils.c 9679 2002-08-06 20:11:24Z dallen $
____________________________________________________________________________*/

// System Headers
#include <windows.h>
#include <assert.h>
#include <time.h>

// PGPsdk Headers
#include "pgpHash.h"

// Shared Headers
#include "BlockUtils.h"

// Project Headers
#include "MapFile.h"
#include "TranslatorUtils.h"

/*	Address list:
		[0] From
		[1] To
		[2] Cc
		[3] Bcc
*/

#define	FROM_ADDRESS	0

/*____________________________________________________________________________
	'EudoraAddresses' as supplied by Eudora consists 
	of an array of StringHandles that come in groups 
	of three.  Each group consists of

		{ "user@host.domain", "full user name", "nickname" }
		
	The last two entries may be null and the list terminates 
	when a first item is null.
____________________________________________________________________________*/
unsigned long 
CreateRecipientList(char** EudoraAddresses, 
					char*** Recipients)
{
	const USHORT kClumpSize = 3;
	ULONG numRecipients = 0;
	ULONG curGroupIndex	= 0;	// start at first recipient 

	assert(EudoraAddresses);
	assert(Recipients);

	while ( EudoraAddresses )
	{
		char* userName = NULL;

		userName = EudoraAddresses [ curGroupIndex ];

		if ( !userName )
			break;	// all done
		
		if ( userName[ 0 ] == '\0' )
			break;	// all done

		// Don't encrypt to the From: address */

		if (curGroupIndex == (ULONG)(FROM_ADDRESS * kClumpSize))
		{
			curGroupIndex += kClumpSize;
			continue;
		}

		if(!(*Recipients))
		{
			*Recipients = (char**) malloc(sizeof(char*));

			if(*Recipients)
			{
				(*Recipients)[numRecipients] = 
								(char*) malloc(strlen(userName) + 1); 

				strcpy((*Recipients)[numRecipients++], userName);
			}
			else
			{
				break;
			}
		}
		else
		{
			*Recipients = (char**) realloc(*Recipients, 
				sizeof(char*) * (numRecipients + 1) );

			if(*Recipients)
			{
				(*Recipients)[numRecipients] = 
								(char*) malloc(strlen(userName) + 1); 

				strcpy((*Recipients)[numRecipients++], userName);
			}
			else
			{
				break;
			}
		}
		
		curGroupIndex += kClumpSize;
	}

	return numRecipients;
}

void 
FreeRecipientList(char** Recipients, 
				  unsigned long  numRecipients)
{
	unsigned long index = 0;

	//assert(Recipients);

	if(Recipients)
	{
		for(index = 0; index < numRecipients; index++)
		{
			if( Recipients[index] )
			{
				// free each string
				free(Recipients[index]);
			}
		}

		free(Recipients);
	}
}


	long
ConvertFileToEncryptedPGPMIME(
	const char*			file,
	char *				delimiter )
{
	long		err	= 0;
	char		tempFile[MAX_PATH];

	assert(file);
	assert(delimiter);
	
	GetUniqueMIMEDelimiter( delimiter );

	strcpy(tempFile, file);
	strcat(tempFile, ".tmp");

	err	= WrapWithEncryptedPGPMIME( file, tempFile, delimiter);

	if ( !err ) 
	{
		if( DeleteFile(file))
		{
			MoveFile( tempFile, file );
		}
	}
	
	return( err );
}

	long
ConvertFileToSignedPGPMIME(
	const char*			file,
	const char*			sigfile, 
	long				hash,
	char *				delimiter )
{
	long		err	= 0;
	char		tempFile[MAX_PATH];

	assert(file);
	assert(sigfile);
	assert(delimiter);
	
	GetUniqueMIMEDelimiter( delimiter );

	strcpy(tempFile, file);
	strcat(tempFile, ".tmp");

	err	= WrapWithSignedPGPMIME( file, sigfile, tempFile, delimiter, hash);

	if ( !err ) 
	{
		if( DeleteFile(file))
		{
			MoveFile( tempFile, file );
		}
	}
	
	return( err );
}

/*____________________________________________________________________________
	Wrap a PGP encrypted message block with PGP mime headers.
	
	The output is suitable for a mailer.
____________________________________________________________________________*/
	long
WrapWithEncryptedPGPMIME(
	const char*			inputfile,
	const char*			outputfile,
	const char *		delimiter	// does not include the "--" part
	)
{
	HANDLE		hOutput = NULL;
	long		err	= 0;
	
	// create and open the output file
	hOutput = CreateFile(	outputfile,
							GENERIC_WRITE|GENERIC_READ,
							0, 
							NULL, 
							CREATE_ALWAYS, 
							FILE_ATTRIBUTE_NORMAL, 
							NULL);


	// did we successfully open the file
	if ( INVALID_HANDLE_VALUE != hOutput)
	{
		char	buffer[ 1024 ];
		DWORD	count;
		
		const char	kEncryptedMIMEHeaderTemplate[] =
		"Mime-Version: 1.0\r\n"
		"Content-Type: multipart/encrypted; boundary="
		"\"%s\"; protocol=\"application/pgp-encrypted\"; "
		"PGPFormat=\"PGPMIME-encrypted\"\r\n\r\n"
		"--%s\r\n"
		"Content-Type: application/pgp-encrypted\r\n\r\n"
		"Version: 1\r\n\r\n"
		"--%s\r\n"
		"Content-Type: application/octet-stream\r\n\r\n";
		
		wsprintf( buffer,kEncryptedMIMEHeaderTemplate, 
					delimiter, delimiter, delimiter);
		
		count	= strlen( buffer );
		
		if ( WriteFile(hOutput,buffer,count, &count, NULL) )
		{
			err = CopyInputFileToOutputFile( inputfile, hOutput );

			if ( ! err )
			{
				wsprintf( buffer,  "\r\n--%s--\r\n", delimiter );
				count = strlen( buffer );

				if(!WriteFile(hOutput, buffer, count, &count, NULL))
				{
					err = 1;
				}
			}
		}
		else
		{
			err = 1;
		}
		
		CloseHandle( hOutput );
	}
	
	return( err );
}


	long
WrapWithSignedPGPMIME(
	const char *		inputfile,
	const char *		sigfile,
	const char *		outputfile,
	const char *		delimiter,
	unsigned long		hash)
{
	HANDLE		hOutput = NULL;
	long		err	= 0;

	// create and open the output file
	hOutput = CreateFile(	outputfile,
							GENERIC_WRITE|GENERIC_READ,
							0, 
							NULL, 
							CREATE_ALWAYS, 
							FILE_ATTRIBUTE_NORMAL, 
							NULL);



	// did we successfully open the files
	if(	( INVALID_HANDLE_VALUE != hOutput ) )
	{
		char	buffer[ 1024 ];
		DWORD	count;
		const char	kMessageIntegrityCheckMD5[] = "pgp-md5";
		const char	kMessageIntegrityCheckSHA1[] = "pgp-sha1";
		const char	kSignedMIMEHeaderTemplate[] =
			"Mime-Version: 1.0\r\n"
			"Content-Type: multipart/signed; boundary="
			"\"%s\""
			"; micalg=%s; protocol=\"application/pgp-signature\"; "
			"PGPFormat=\"PGPMIME-signed\""
			"\r\n\r\n"
			"--%s\r\n";
	
		wsprintf(	buffer, 
					kSignedMIMEHeaderTemplate, 
					delimiter, 
					(hash == kPGPHashAlgorithm_MD5 ? 
					kMessageIntegrityCheckMD5 : kMessageIntegrityCheckSHA1), 
					delimiter );
			
		count = strlen( buffer );

		if ( WriteFile(hOutput,buffer,count, &count, NULL) )
		{
			err = CopyInputFileToOutputFile( inputfile, hOutput );

			if ( ! err )
			{
				const char kSignedDividerTemplate[] =
				"\r\n--%s\r\nContent-Type: application/pgp-signature\r\n\r\n";

				wsprintf( buffer, kSignedDividerTemplate, delimiter );
				count = strlen( buffer );

				if(!WriteFile(hOutput, buffer, count, &count, NULL))
				{
					err = 1;
				}
				else
				{
					err = CopyInputFileToOutputFile( sigfile, hOutput );

					if( ! err )
					{
						wsprintf( buffer,  "\r\n--%s--\r\n", delimiter );
						count	= strlen( buffer );

						if(!WriteFile(hOutput, buffer, count, &count, NULL))
						{
							err = 1;
						}
					}
				}
			}
		}
		else
		{
			err = 1;
		}
		
		CloseHandle( hOutput );
	}
	
	return( err );
}


	void
GetUniqueMIMEDelimiter( char * delimiter )
{
	static BOOL bSeeded = FALSE;
	const char szPGPmimePlugin[] = "=====_Eudora-PGP-Plugin";

	assert(delimiter);

	if( !bSeeded)
	{
		srand( (unsigned)time( NULL ) );
		bSeeded = TRUE;
	}

	wsprintf(delimiter, "%s%d==_",
						szPGPmimePlugin,
						rand());
}

long CopyInputFileToOutputFile(const char* szInputFile, 
							   HANDLE hOutputFile)
{
	long err = 0;
	HANDLE hInputFile = NULL;


	// open the input file
	hInputFile = CreateFile(	szInputFile,
								GENERIC_READ,
								FILE_SHARE_READ, 
								NULL, 
								OPEN_EXISTING, 
								FILE_ATTRIBUTE_NORMAL, 
								NULL);

	// did we successfully open the file
	if ( INVALID_HANDLE_VALUE != hInputFile)
	{
		void* buffer = NULL;
		DWORD dwFileSize = 0;

		// let's find out how much data there is to copy
		dwFileSize = GetFileSize(hInputFile, NULL);

		if( dwFileSize != 0xffffffff) // if there was no error
		{
			buffer = malloc( dwFileSize + 1);

			if(buffer)
			{
				// read in the data from the input file
				if( ReadFile(	hInputFile, 
								buffer, 
								dwFileSize,
								&dwFileSize, 
								NULL) )
				{
					// write it to the current position of the ouput file
					if(!WriteFile(	hOutputFile, 
									buffer, 
									dwFileSize, 
									&dwFileSize, 
									NULL))
					{
						err = 1;
					}
				}
				else
				{
					err = 1;
				}

				// clean up
				free(buffer);
			}
		}

		// clean up
		CloseHandle(hInputFile);
	}

	return err;
}

void RecreateOriginalMessageFile(const char* szInputFile, 
								 const char* szOutputFile)
{
	char szTempFile[MAX_PATH];
	HANDLE hTemp;
	DWORD InputFileSize = 0;
	char* InputBuffer = NULL;
	DWORD encryptedBlockStart = 0;
	DWORD encryptedBlockSize = 0;
	BOOL Success = FALSE;

	strcpy(szTempFile, szOutputFile);
	strcat(szTempFile, ".tmp");

	hTemp = CreateFile(		szTempFile,
							GENERIC_WRITE|GENERIC_READ,
							0, 
							NULL, 
							CREATE_ALWAYS, 
							FILE_ATTRIBUTE_NORMAL, 
							NULL);



	// did we successfully open the file
	if(	( INVALID_HANDLE_VALUE != hTemp ) )
	{
		InputBuffer = MapFile(szInputFile, &InputFileSize);

		if(InputBuffer)
		{
			if( FindEncryptedBlock((const void *)InputBuffer, 
									InputFileSize, 
									&encryptedBlockStart,
									&encryptedBlockSize) )
			{
				DWORD count = encryptedBlockStart;

				if(WriteFile(hTemp, InputBuffer, count, &count, NULL))
				{
					CopyInputFileToOutputFile(szOutputFile, hTemp);

					count = InputFileSize - 
						( (InputBuffer + encryptedBlockStart + 
							encryptedBlockSize) - InputBuffer);

					if(WriteFile(hTemp, 
						(InputBuffer + encryptedBlockStart + 
							encryptedBlockSize), 
						count, 
						&count, 
						NULL))
					{
						Success = TRUE;
					}
				}
			}

			free(InputBuffer);
		}

		CloseHandle(hTemp);

		if(Success)
		{
			CopyFile(szTempFile, szOutputFile, FALSE); 
		}

		DeleteFile(szTempFile);
	}
}

char* RecreateOriginalMessageBuffer(const char* pInputBuffer, 
									const char* pOutputBuffer)
{
	DWORD encryptedBlockStart = 0;
	DWORD encryptedBlockSize = 0;
	char* pTempMem = NULL;
	DWORD dwTempMemSize = 0;

	dwTempMemSize = ( strlen(pInputBuffer) > strlen(pOutputBuffer) ) 
				? strlen(pInputBuffer) : strlen(pOutputBuffer);

	if(pInputBuffer)
	{
		if( FindEncryptedBlock((const void *)pInputBuffer, 
								strlen(pInputBuffer), 
								&encryptedBlockStart, 
								&encryptedBlockSize) )
		{
			DWORD count = encryptedBlockStart;
			
			dwTempMemSize += encryptedBlockSize;
	
			pTempMem = (char*)malloc(dwTempMemSize);

			if( pTempMem )
			{
				memset(pTempMem, 0x00, dwTempMemSize);

				memcpy(pTempMem, pInputBuffer, count);

				strcat(pTempMem, pOutputBuffer);

				count = strlen(pInputBuffer) - 
					( (pInputBuffer + encryptedBlockStart + 
						encryptedBlockSize) - pInputBuffer);

				strcat(	pTempMem, 
						(pInputBuffer + encryptedBlockStart + 
							encryptedBlockSize));
			}
		}
	}

	return pTempMem;
}

void AddCRLFToEOF(const char* szInputFile)
{
	HANDLE		hInputFile = NULL;

	// create and open the output file
	hInputFile = CreateFile(	szInputFile,
								GENERIC_WRITE|GENERIC_READ,
								0, 
								NULL, 
								OPEN_EXISTING, 
								FILE_ATTRIBUTE_NORMAL, 
								NULL);

	// did we successfully open the file
	if ( INVALID_HANDLE_VALUE != hInputFile)
	{
		const char CRLF[] = "\r\n";
		DWORD count = strlen(CRLF);

		SetFilePointer(hInputFile, 0, NULL, FILE_END);

		WriteFile(hInputFile, CRLF, count, &count, NULL);

		// clean up
		CloseHandle(hInputFile);
	}
}

/*__Editor_settings____

	Local Variables:
	tab-width: 4
	End:
	vi: ts=4 sw=4
	vim: si
_____________________*/
