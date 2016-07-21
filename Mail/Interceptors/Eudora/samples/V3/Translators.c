/*____________________________________________________________________________
	Copyright (C) 2002 PGP Corporation
	All rights reserved.

	$Id: Translators.c 9679 2002-08-06 20:11:24Z dallen $
____________________________________________________________________________*/

// System Headers
#include <windows.h>
#include <assert.h>

// Eudora Headers
#include "emssdk/Mimetype.h"

// Shared Headers
#include "pgpClientLib.h"
#include "PGPsc.h"
#include "EncryptSign.h"
#include "DecryptVerify.h"

// Project Headers
#include "PGPPlug.h"
#include "TranslatorUtils.h"
#include "MyMIMEUtils.h"

// Global Variables
extern HINSTANCE		g_hinst;
extern PGPContextRef	g_pgpContext;
extern PGPtlsContextRef g_tlsContext;
extern HWND				g_hwndEudoraMainWindow;


PluginError CanTranslateMIMEType(long transContext,
								 emsMIMEtypeP mimeType,
								 const char* type, 
								 const char* subType);



PluginError CanPerformTranslation(
	short trans_id, 
	emsMIMEtypeP in_mime, 
	long context
)
{
	PluginError err = EMSR_CANT_TRANS;

	if ( in_mime )
	{			
		switch(trans_id)
		{
			// manual (aka On-Demand) translators
			case kManualEncryptSignTranslatorID:
			case kManualDecryptVerifyTranslatorID:
			{
				if( in_mime && match_mime_type(in_mime, "text", "plain") )
				{
					if ( ( context & EMSF_ON_REQUEST ) != 0 )
					{
						err = EMSR_NOW;
					}
				}
				break;
			}

			// Mime Translators
			case kEncryptTranslatorID:
			case kSignTranslatorID:
			case kEncryptAndSignTranslatorID:
			{
				err = EMSR_NOW;
				break;
			}

			case kDecryptTranslatorID:
			{
				err = CanTranslateMIMEType(	context, 
											in_mime, 
											"multipart", 
											"encrypted");

				break;
			}

			case kVerifyTranslatorID:
			{
				err = CanTranslateMIMEType(	context, 
											in_mime, 
											"multipart", 
											"signed");

				break;
			}
		}		
	}

	return err;
}

PluginError PerformTranslation(
	short trans_id, 
	char* in_file, 
	char* out_file, 
	char** addresses, 
	emsMIMEtypeP  in_mime,
	emsMIMEtypeP* out_mime
)
{
	PluginError pluginReturn			= EMSR_UNKNOWN_FAIL;
	BOOL		bSign					= FALSE;
	PGPError	error					= 0;
	PGPSize		mimeBodyOffset			= 0;
	char		szExe[256];
	char		szDll[256];

	assert(in_file);
	assert(out_file);

	LoadString(g_hinst, IDS_EXE, szExe, sizeof(szExe));
	LoadString(g_hinst, IDS_DLL, szDll, sizeof(szDll));

	switch( trans_id )
	{
		case kEncryptTranslatorID:
		case kSignTranslatorID:
		case kEncryptAndSignTranslatorID:
		{
			char** RecipientList = NULL;
			unsigned long numRecipients = 0;
			PGPOptionListRef pgpOptions = NULL;
			PGPOptionListRef signOptions = NULL;
			char mimeSeparator[kPGPMimeSeparatorSize];
			PGPclRecipientDialogStruct *prds = NULL;	
		
			// allocate a recipient dialog structure
			prds = (PGPclRecipientDialogStruct *) 
					calloc(sizeof(PGPclRecipientDialogStruct), 1);

			if(prds)
			{
				char szTitle[256] = {0x00};		// title for recipient dialog
				UINT recipientReturn = FALSE;	// recipient dialog result

				error =	PGPclOpenDefaultKeyrings(g_pgpContext, 
							kPGPOpenKeyDBFileOptions_Mutable, 
							&(prds->keydbOriginal));

				if (IsPGPError(error))
					error =	PGPclOpenDefaultKeyrings(g_pgpContext, 
								kPGPOpenKeyDBFileOptions_None, 
								&(prds->keydbOriginal));

				if (IsPGPError(error))
				{
					char szTitle[255];
					char szBuffer[1024];

					LoadString(g_hinst, IDS_TITLE_PGPERROR, szTitle, 254);
					LoadString(g_hinst, IDS_Q_NOKEYRINGS, szBuffer, 1023);
	
					if (MessageBox(NULL, szBuffer, szTitle, MB_YESNO))
					{
						char szPath[MAX_PATH];

						PGPclGetPath(kPGPclPGPkeysExeFile, szPath, MAX_PATH-1);
						PGPclExecute(szPath, SW_SHOW);
					}

					return EMSR_UNKNOWN_FAIL;
				}

				if ((trans_id == kEncryptTranslatorID) ||
					(trans_id == kEncryptAndSignTranslatorID))
				{
					if(addresses) // do we have addresses to pass along
					{
						numRecipients = CreateRecipientList(addresses, 
															&RecipientList);
					}

					LoadString(GetModuleHandle("PGPplugin.dll"), 
						IDS_TITLE_RECIPIENTDIALOG, szTitle, sizeof(szTitle));

					prds->context			= g_pgpContext;
					prds->tlsContext		= g_tlsContext;
					prds->Version			= kPGPCurrentRecipVersion;
					prds->hwndParent		= g_hwndEudoraMainWindow;			
					prds->szTitle			= szTitle;
					prds->dwOptions			= kPGPclASCIIArmor;	

					prds->dwDisableFlags	= 
										kPGPclDisableWipeOriginal |
										kPGPclDisableASCIIArmor |
										kPGPclDisableSelfDecryptingArchive;

					prds->dwNumRecipients	= numRecipients;	
					prds->szRecipientArray	= RecipientList;

					/* Disable FYEO if there's an attachment or HTML */

					if (in_mime && !match_mime_type(in_mime, "text", "plain"))
						prds->dwDisableFlags |= kPGPclDisableSecureViewer;
					else
						prds->dwDisableFlags |= kPGPclDisableInputIsText;

					// If shift is pressed, force the dialog to pop.
					if (GetAsyncKeyState( VK_CONTROL) & 0x8000)
						prds->dwDisableFlags |= kPGPclDisableAutoMode;

					// See who we wish to encrypt this to
					recipientReturn = PGPclRecipientDialog( prds );
					
					if (prds->keydbAdded != NULL)
					{
						PGPUInt32 numKeys;
						PGPKeySetRef keySet;
						
						PGPNewKeySet(prds->keydbAdded, &keySet);
						PGPCountKeys(keySet, &numKeys);
						if (numKeys > 0)
							PGPclImportKeys(g_pgpContext, g_tlsContext, 
								prds->hwndParent, keySet, prds->keydbOriginal, 
								kPGPclNoDialogForValidSingletons);
						
						PGPFreeKeySet(keySet);
						PGPFreeKeyDB(prds->keydbAdded);
						prds->keydbAdded = NULL;
					}

					if (!recipientReturn)
					{
						if (RecipientList)
							FreeRecipientList(RecipientList, numRecipients);

						if (prds->keydbSelected != NULL)
							PGPFreeKeyDB(prds->keydbSelected);

						PGPFreeKeyDB(prds->keydbOriginal);
						free(prds);
						return EMSR_UNKNOWN_FAIL;
					}
				}

				if( IsntPGPError(error) )
				{
					error = PGPBuildOptionList(g_pgpContext, &pgpOptions,
								PGPOOutputLineEndType(g_pgpContext, 
									kPGPLineEnd_CRLF),
								PGPOPGPMIMEEncoding(g_pgpContext, 
									TRUE, 
									&mimeBodyOffset, 
									mimeSeparator),     
								PGPOLastOption(g_pgpContext) );
				}

				if(IsntPGPError( error))
				{
					error = EncryptSignFile(g_hinst, g_hwndEudoraMainWindow, 
								g_pgpContext, g_tlsContext, szExe, 
								szDll, in_file, prds, pgpOptions, 
								&signOptions, out_file, 
								((trans_id == 
									kSignTranslatorID) ? FALSE : TRUE ),
								((trans_id == 
									kEncryptTranslatorID) ? FALSE : TRUE ),
								FALSE);

					PGPFreeOptionList(pgpOptions);
					PGPFreeOptionList(signOptions);
				}
				else
				{
					PGPclEncDecErrorBox (NULL, error);
				}

				if (RecipientList)
					FreeRecipientList(RecipientList, numRecipients);
	
				if (prds->keydbSelected != NULL)
					PGPFreeKeyDB(prds->keydbSelected);

				PGPFreeKeyDB(prds->keydbOriginal);
				free(prds);

				if( IsntPGPError(error) )
				{
					if( out_mime )
					{
						pluginReturn = BuildEncryptedPGPMIMEType( 
													out_mime, 
													mimeSeparator );

						if(EMSR_OK == pluginReturn)
						{
							pluginReturn = AddMIMEParam(*out_mime, 
														"PGPFormat", 
														"PGPMIME-encrypted" );
						}
					}
				}
			}

			break;
		}

		case kDecryptTranslatorID:
		case kVerifyTranslatorID:
		{
			char *szTempFile = NULL;
			BOOL bFYEO = FALSE;
			void *pOutput = NULL;
			PGPSize outSize = 0;

			error = DecryptVerifyFile(g_hinst, g_hwndEudoraMainWindow, 
						g_pgpContext, g_tlsContext, szExe, szDll, in_file, 
						TRUE, FALSE, &szTempFile, &pOutput, &outSize, &bFYEO);

			if( IsntPGPError(error) ) 
			{
				if ((bFYEO)||(PGPscGetSecureViewerPref((void *)g_pgpContext)))
					PGPscTempestViewer((void *)g_pgpContext,NULL,pOutput,
						outSize,bFYEO);

				CopyFile(szTempFile, out_file, FALSE);
				DeleteFile(szTempFile);

				if( out_mime )
				{
					ParseFileForMIMEType( out_file, out_mime );
				}

				if ((bFYEO)||(PGPscGetSecureViewerPref((void *)g_pgpContext)))
					pluginReturn = EMSR_UNKNOWN_FAIL;
				else
					pluginReturn = EMSR_OK;
			}

			if (szTempFile != NULL)
				PGPFreeData(szTempFile);
			if (pOutput != NULL)
				PGPFreeData(pOutput);
			break;
		}

	}

	return pluginReturn;
}


PluginError CanTranslateMIMEType(
	long					transContext,
	emsMIMEtypeP			mimeType,
	const char*				type,
	const char*				subType)
{
	PluginError	err	= EMSR_CANT_TRANS;
	
	if ( mimeType )
	{
		BOOL		haveValidContext;
		const long	kValidContextMask	= EMSF_ON_DISPLAY | EMSF_ON_ARRIVAL;
		
		haveValidContext	= ( transContext & kValidContextMask ) != 0;
		
		if ( haveValidContext )
		{
			BOOL	haveMatch;
			
			haveMatch	= match_mime_type( mimeType, type, subType );

			if( haveMatch )
			{
				const char* PGPMimeParam = NULL;

				PGPMimeParam = get_mime_parameter(mimeType, "protocol");

				if(PGPMimeParam && 
					(!strcmp(PGPMimeParam, "application/pgp-encrypted") || 
					!strcmp(PGPMimeParam, "application/pgp-signature") ) )
				{
					haveMatch = TRUE;
				}
				else
				{
					haveMatch = FALSE;
				}
			}
			
			if ( haveMatch )
			{
	 		    if( transContext == EMSF_ON_ARRIVAL )
	 		    {
		   	        err = EMSR_NOT_NOW;
		   	    }
		   	    else
		   	    {
		   		    err = EMSR_NOW;
		   		}
			}
		}
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
