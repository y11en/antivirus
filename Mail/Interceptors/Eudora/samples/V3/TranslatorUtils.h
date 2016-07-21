/*____________________________________________________________________________
	Copyright (C) 2002 PGP Corporation
	All rights reserved.

	$Id: TranslatorUtils.h 9679 2002-08-06 20:11:24Z dallen $
____________________________________________________________________________*/
#ifndef Included_TranslatorUtils_h	/* [ */
#define Included_TranslatorUtils_h


#define DELIMITERLENGTH 256

#include "emssdk/ems-win.h"

unsigned long CreateRecipientList(
	char** EudoraAddresses, 
	char*** Recipients
	);

void 
FreeRecipientList(
	char** Recipients, 
	unsigned long  numRecipients
	);

long ConvertFileToEncryptedPGPMIME(
	const char*			file,
	char *				delimiter 
	);

long ConvertFileToSignedPGPMIME(
	const char*			file,
	const char*			sigfile, 
	long				hash,
	char*				delimiter 
	);

long WrapWithEncryptedPGPMIME(
	const char*			inputfile,
	const char*			outputfile,
	const char *		delimiter	// does not include the "--" part
	);

long WrapWithSignedPGPMIME(
	const char *		inputfile,
	const char *		sigfile,
	const char *		outputfile,
	const char *		delimiter, // does not include the "--" part
	unsigned long		hash
	);

void GetUniqueMIMEDelimiter( 
	char * delimiter 
	);

long CopyInputFileToOutputFile(
	const char* szInputFile, 
	HANDLE hOutputFile
	);


void AddCRLFToEOF(
	const char* szInputFile
	);

void RecreateOriginalMessageFile(
	const char* szInputFile, 
	const char* szOutputFile
	);

char* RecreateOriginalMessageBuffer(
	const char* pInputBuffer, 
	const char* pOutputBuffer
	);

#endif /* ] Included_TranslatorUtils_h */


/*__Editor_settings____

	Local Variables:
	tab-width: 4
	End:
	vi: ts=4 sw=4
	vim: si
_____________________*/

