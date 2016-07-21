/*____________________________________________________________________________
	Copyright (C) 2002 PGP Corporation
	All rights reserved.

	$Id: Translators.c 27776 2004-09-23 09:16:08Z wjb $
____________________________________________________________________________*/
 
// System Headers
#include <windows.h>
#include <assert.h>

// Eudora Headers
#include "emssdk/Mimetype.h"
#include "emssdk/Encoding.h"

// Shared Headers
#include "pgpClientXMLPrefs.h"
#include "pgpClientLib.h"
#include "PGPsc.h"
#include "EncryptSign.h"
#include "DecryptVerify.h"
#include "Prefs.h"
#include "pgpHex.h"
#include "pgpRc.h"

// Project Headers
#include "TranslatorIDs.h"
#include "MapFile.h"
#include "TranslatorUtils.h"
#include "MyMIMEUtils.h"
#include "resource.h"
#include "pgpUnicodeWin32.h"

#define WARN_BIT		0x0001
#define CONTINUE_BIT	0x0002

// Global Variables
extern HINSTANCE		g_hinst;
extern PGPContextRef	g_pgpContext;
extern PGPtlsContextRef	g_tlsContext;
extern HWND				g_hwndHidden;
extern PGPChar *			g_szWipeFile;


long CanTranslateMIMEType(	long transContext,
							emsMIMEtypeP mimeType,
							const char* type, 
							const char* subType);

BOOL UIWarnYesNo(HWND hwnd, UINT nID, PGPChar8 *nPrefIndex);


long CanPerformTranslation(
	const long trans_id, 
	const emsMIMEtypeP in_mime, 
	const long context
)
{
	long err = EMSR_CANT_TRANS;

	if ( in_mime )
	{			
		switch(trans_id)
		{
			// manual (aka On-Demand) translators
#if SENDOK
			case kManualEncryptTranslatorID:
			case kManualSignTranslatorID:
			case kManualEncryptSignTranslatorID:
#endif // SENDOK
			case kManualDecryptVerifyTranslatorID:
			{
				if( in_mime && match_mime_type(	in_mime, 
												"text", 
												"plain") )
				{
					if( ( context & EMSF_ON_REQUEST ) != 0 )
					{
						err = EMSR_NOW;
					}
				}
				break;
			}
#if SENDOK
			// Mime Translators
			case kEncryptTranslatorID:
			case kSignTranslatorID:
			case kEncryptAndSignTranslatorID:
			{
				err = EMSR_NOW;
				break;
			}
#endif // SENDOK
			case kDecryptTranslatorID:
			{
				if( in_mime && match_mime_type(	in_mime, 
												"multipart", 
												"encrypted") )
				{
					err = CanTranslateMIMEType(	context, 
												in_mime, 
												"multipart", 
												"encrypted");
				}

				break;
			}

			case kVerifyTranslatorID:
			{
				if( in_mime && match_mime_type(	in_mime, 
												"multipart", 
												"signed") )
				{
					err = CanTranslateMIMEType(	context, 
												in_mime, 
												"multipart", 
												"signed");
				}

				break;
			}
		}		
	}

	return err;
}

long PerformTranslation(
	const long trans_id, 
	const PGPChar8* in_fileASCII, 
	const PGPChar8* out_fileASCII, 
	emsHeaderDataP header, 
	emsMIMEtypeP in_mime,
	emsMIMEtypeP* out_mime
)
{
	long		pluginReturn	= EMSR_UNKNOWN_FAIL;
	BOOL		bSign			= FALSE;
	PGPError	error			= kPGPError_NoErr;
	PGPSize		mimeBodyOffset	= 0;
	BOOL		bMIME			= FALSE;
	BOOL		bUsePGPMIME		= FALSE;
	BOOL		bManual			= TRUE;
	BOOL		bDisableFYEO	= FALSE;
	HWND		hwndMain;
	PGPChar		szExe[256];
	PGPChar		szDll[256];
	PGPChar		temp_file[256];
	PGPChar		in_file[MAX_PATH];
	PGPChar		out_file[MAX_PATH];

	assert(in_fileASCII);
	assert(out_fileASCII);

#if UNICODE
	pgpUTF8StringToUTF16(in_fileASCII,kPGPUnicodeNullTerminated,
		in_file,MAX_PATH, NULL);
	pgpUTF8StringToUTF16(out_fileASCII,kPGPUnicodeNullTerminated,
		out_file,MAX_PATH, NULL);
#else
	PGPStrCpy(in_file,in_fileASCII);
	PGPStrCpy(out_file,out_fileASCII);
#endif

	temp_file[0] = 0;

	PGPLoadString(g_hinst, IDS_EXE, szExe, sizeof(szExe)/sizeof(PGPChar));
	PGPLoadString(g_hinst, IDS_DLL, szDll, sizeof(szDll)/sizeof(PGPChar));
	hwndMain = GetForegroundWindow();

	switch( trans_id )
	{
#if SENDOK
		case kEncryptTranslatorID:
		case kSignTranslatorID:
		case kEncryptAndSignTranslatorID:
		{
			bManual = FALSE;
			bUsePGPMIME = UsePGPMimeForEncryption(PGPPeekContextMemoryMgr(g_pgpContext));

			if(bUsePGPMIME || 
				( in_mime && !match_mime_type(in_mime, "text", "plain") ) )
			{
				bMIME = TRUE;

				/* Disable FYEO if there's an attachment or HTML */

				if (in_mime && !match_mime_type(in_mime, "text", "plain"))
				{
					bDisableFYEO = TRUE;

					if (!bUsePGPMIME)
					/* Warn user that the plug-in is about to use PGP/MIME */

					if (!UIWarnYesNo(hwndMain, IDS_W_PGPMIME, 
						kPGPPrefWin32EudoraOutputPGPMIME))
						return EMSR_UNKNOWN_FAIL;
				}
			}
		}

		case kManualEncryptTranslatorID:
		case kManualSignTranslatorID:
		case kManualEncryptSignTranslatorID:
		{
			char** addresses = NULL;
			unsigned long numRecipients = 0;
			PGPOptionListRef pgpOptions = NULL;
			PGPOptionListRef signOptions = NULL;
			char mimeSeparator[kPGPMimeSeparatorSize];
			BOOL bEncrypt = FALSE;
			BOOL bSign = FALSE;
			PGPclRecipientDialogStruct *prds = NULL;	

			if(bManual)
			{
				bEncrypt = ((trans_id == kManualSignTranslatorID) 
								? FALSE : TRUE );

				bSign = ((trans_id == kManualEncryptTranslatorID) 
								? FALSE : TRUE );
			}
			else
			{
				DWORD dwMapLength = 0;
				char *szMapBuffer = NULL;
				char *szContent = NULL;

				bEncrypt = ((trans_id == kSignTranslatorID) ? FALSE : TRUE );
				bSign = ((trans_id == kEncryptTranslatorID) ? FALSE : TRUE );

				szMapBuffer	= MapFile( in_file, &dwMapLength );
				
				if ( szMapBuffer )
				{
					szContent = rfc822_extract_cte(szMapBuffer);
					free(szMapBuffer);
				}

				if (szContent)
				{
					free(szContent);
					bMIME = TRUE;
				}

				if(!bMIME)
				{
					// want it to look clean upon decryption
					// we need only the text of the message
					StripMIMEHeader(in_file);
				}
			}			

			if (!bMIME)
			{
				// Convert to UTF8 and wrap if necessary -wjb
				long lWrapWidth = 0;
				HANDLE hFile = NULL;
				DWORD dwBytes;
				DWORD dwLength;
				char *szFileData = NULL;
				char *szFileUTF8 = NULL;
				char *szWrapped = NULL;
				DWORD memamt,dummy;
				BOOL bWrapOK = FALSE;

				hFile = CreateFile(in_file, GENERIC_READ, 0, NULL, 
							OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);

				dwLength = GetFileSize(hFile, NULL);

				szFileData = (char *) calloc(sizeof(char), dwLength+1);

				ReadFile(hFile, szFileData, dwLength, &dwBytes, NULL);
				CloseHandle(hFile);
				szFileData[dwLength] = 0;

				pgpLocalStringToUTF8 (0, 
					GetACP(), szFileData, dwLength, 
					NULL,0,&memamt);

				szFileUTF8 = (char *) calloc(sizeof(char), memamt+1);

				if(szFileUTF8)
				{
					pgpLocalStringToUTF8 (0, 
						GetACP(), szFileData, dwLength, 
						szFileUTF8,memamt+1,&dummy);
				}

				PGPStrCpy(temp_file, in_file);
				dwBytes = PGPStrLen(temp_file);
				temp_file[dwBytes-5]++;
				temp_file[dwBytes-6]++;
				temp_file[dwBytes-7]++;

				hFile = CreateFile(temp_file, GENERIC_WRITE, 0, NULL, 
							CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, 
							NULL);

				if (ByDefaultWordWrap(PGPPeekContextMemoryMgr(g_pgpContext), 
					&lWrapWidth))
				{
					if (WrapBuffer(&szWrapped, szFileUTF8, (short)lWrapWidth))
					{
						WriteFile(hFile, szWrapped, strlen(szWrapped),
							&dwBytes, NULL);

						bWrapOK=TRUE;
					}
				}
				
				if(!bWrapOK)
				{
					WriteFile(hFile, szFileUTF8, strlen(szFileUTF8),
						&dwBytes, NULL);
				}

				CloseHandle(hFile);

//						in_file = temp_file; use below instead -wjb
				PGPStrCpy(in_file,temp_file);

				if(szFileData!=NULL)
				{
					memset(szFileData,0x00,dwLength);
					free(szFileData);
				}

				if(szWrapped!=NULL)
				{
					memset(szWrapped,0x00,strlen(szWrapped));
					free(szWrapped);
				}

				if(szFileUTF8!=NULL)
				{
					memset(szFileUTF8,0x00,memamt);
					free(szFileUTF8);
				}
			}

			// allocate a recipient dialog structure
			prds = (PGPclRecipientDialogStruct *) 
					calloc(sizeof(PGPclRecipientDialogStruct), 1);

			if(prds)
			{
				PGPChar szTitle[256] = {0x00};		// title for recipient dialog
				UINT recipientReturn = FALSE;	// recipient dialog result
				PGPKeySetRef keyset	= NULL;

				error =	PGPclOpenDefaultKeyrings(g_pgpContext, 
							kPGPOpenKeyDBFileOptions_Mutable, 
							&(prds->keydbOriginal));

				if (IsPGPError(error))
					error =	PGPclOpenDefaultKeyrings(g_pgpContext, 
								kPGPOpenKeyDBFileOptions_None, 
								&(prds->keydbOriginal));

				if (IsPGPError(error))
				{
					PGPChar szTitle[255];
					PGPChar szBuffer[1024];

					PGPLoadString(g_hinst, IDS_TITLE_PGPERROR, szTitle, sizeof(szTitle)/sizeof(PGPChar));
					PGPLoadString(g_hinst, IDS_Q_NOKEYRINGS, szBuffer, sizeof(szBuffer)/sizeof(PGPChar));
	
					if (MessageBox(hwndMain, szBuffer, szTitle, MB_YESNO)
						== IDYES)
					{
						PGPChar szPath[MAX_PATH];

						PGPclGetPath(kPGPclPGPkeysExeFile, szPath, MAX_PATH-1);
						PGPclExecute(szPath, SW_SHOW);
					}

					return EMSR_UNKNOWN_FAIL;
				}

				if (bEncrypt)
				{
					// do we have the header structure?
					// should we autofind recipients?
					if(header) 
					{
						numRecipients = ExtractAddressesFromMailHeader(	
													header,
													&addresses);
					}

					PGPLoadString(GetModuleHandle(PGPTXT_MACHINE("PGPeudora.dll")), 
						IDS_TITLE_RECIPIENTDIALOG, szTitle, sizeof(szTitle)/sizeof(PGPChar));

					prds->context			= g_pgpContext;
					prds->tlsContext		= g_tlsContext;
					prds->Version			= kPGPCurrentRecipVersion;
					prds->hwndParent		= hwndMain;			
					prds->szTitle			= szTitle;
					prds->dwOptions			= kPGPclASCIIArmor;	
					prds->dwDisableFlags	= 
										kPGPclDisableWipeOriginal |
										kPGPclDisableASCIIArmor |
										kPGPclDisableSelfDecryptingArchive;

					if (!bMIME)
						prds->dwDisableFlags |= kPGPclDisableInputIsText;

					prds->dwNumRecipients	= numRecipients;	
					prds->szRecipientArray	= addresses;

					if (bDisableFYEO)
						prds->dwDisableFlags |= kPGPclDisableSecureViewer;

					// If shift is pressed, force the dialog to pop.
					if (GetAsyncKeyState( VK_CONTROL ) & 0x8000)
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
						if (addresses)
							FreeRecipientList(addresses, numRecipients);

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
								(bMIME  ? 
									PGPOPGPMIMEEncoding(g_pgpContext, 
										TRUE, 
										&mimeBodyOffset, 
										mimeSeparator) : 
									PGPONullOption(g_pgpContext) ),     
								PGPOLastOption(g_pgpContext) );
				}

				if(IsntPGPError( error))
				{
					error = EncryptSignFile(g_hinst, hwndMain, 
								g_pgpContext, g_tlsContext, szExe, 
								szDll, (PGPChar *)in_file, prds, 
								pgpOptions, &signOptions, (PGPChar *)out_file, 
								bEncrypt, bSign, FALSE); 

					PGPFreeOptionList(pgpOptions);
					PGPFreeOptionList(signOptions);
				}
				else
				{
					PGPclEncDecErrorBox (hwndMain, error);
				}

				if (addresses)
					FreeRecipientList(addresses, numRecipients);
				if (prds->keydbSelected != NULL)
					PGPFreeKeyDB(prds->keydbSelected);

				PGPFreeKeyDB(prds->keydbOriginal);
				free(prds);

				/* Add a blank line at the beginning to work around
					Eudora bug */
				
				if (IsntPGPError(error) && bSign && !bManual && !bMIME)
				{
					HANDLE hFile = NULL;
					DWORD dwBytes;
					DWORD dwLength;
					char *szFileData = NULL;
					char szLineEnd[] = "\r\n";

					hFile = CreateFile(out_file, GENERIC_READ | GENERIC_WRITE,
								0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL,
								NULL);

					dwLength = GetFileSize(hFile, NULL);

					szFileData = (char *) calloc(sizeof(char), dwLength+1);

					ReadFile(hFile, szFileData, dwLength, &dwBytes, NULL);
					SetFilePointer(hFile, 0, NULL, FILE_BEGIN);

					WriteFile(hFile, szLineEnd, strlen(szLineEnd), &dwBytes, 
						NULL);

					WriteFile(hFile, szFileData, dwLength, &dwBytes, NULL);

					CloseHandle(hFile);
					free(szFileData);
				}

//				if (in_file == temp_file)
				if(!PGPStrCmp(in_file,temp_file))
					DeleteFile(temp_file);

				if( IsntPGPError(error) )
				{
					if( out_mime )
					{
						if ( !*out_mime )
							*out_mime = (emsMIMEtypeP) calloc(1, 
														sizeof(emsMIMEtype));

						if(bMIME)
						{
							*out_mime = make_mime_type(	"multipart",
														"encrypted", 
														"1.0");

							if( *out_mime )
							{
								pluginReturn = EMSR_OK;

								add_mime_parameter(	*out_mime, 
													"boundary", 
													mimeSeparator);

								add_mime_parameter(	*out_mime, 
													"PGPFormat", 
													"PGPMIME-encrypted");
							}
						}
						else
						{
							if( *out_mime )
							{
								pluginReturn = EMSR_OK;

								*out_mime = make_mime_type(	"text",
															"plain", 
															"1.0");
							}
						}
					}
				}
			}

			break;
		}
#endif // SENDOK
		case kDecryptTranslatorID:
		case kVerifyTranslatorID:
		{
			bManual = FALSE;
		}
		case kManualDecryptVerifyTranslatorID:
		{
			PGPChar *szTempFile = NULL;
			BOOL bFYEO = FALSE;
			void *pOutput = NULL;
			PGPSize outSize = 0;
			HANDLE hCheckFile = INVALID_HANDLE_VALUE;
			char *szBuffer = NULL;
			
			hCheckFile = CreateFile(in_file, GENERIC_READ, 0, 
							NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 
							NULL);

			if (hCheckFile != INVALID_HANDLE_VALUE)
			{
				DWORD dwFileSize;
				char *szHeaderEnd;
				char cOldEnd = 0;
				char *szLine;
				char *szLineEnd;
				UINT nPGPBlock = 0;
				char szBeginMessage[] = "-----BEGIN PGP MESSAGE";
				char szBeginSigned[] = "-----BEGIN PGP SIGNED";
				char szEndPGP[] = "-----END PGP";
				char szAttachment[] = "Attachment Converted: ";
				UINT nBeginMessage = strlen(szBeginMessage);
				UINT nBeginSigned = strlen(szBeginSigned);
				UINT nEndPGP = strlen(szEndPGP);
				UINT nAttachment = strlen(szAttachment);

				dwFileSize = GetFileSize(hCheckFile, NULL);
				szBuffer = (char *) calloc(1, dwFileSize + 1);
				ReadFile(hCheckFile, szBuffer, dwFileSize, &dwFileSize, NULL);
				CloseHandle(hCheckFile);
				
				szLine = szBuffer;
				szLineEnd = strstr(szLine, "\r\n");
				while (szLine)
				{
					if (!strncmp(szLine, szBeginMessage, nBeginMessage) ||
						!strncmp(szLine, szBeginSigned, nBeginSigned))
					{
						nPGPBlock++;
					}
					else if (!strncmp(szLine, szEndPGP, nEndPGP))
					{
						nPGPBlock--;
					}

					if (!nPGPBlock)
					{
						if (!strncmp(szLine, szAttachment, nAttachment))
						{
							PGPChar szTitle[255];
							PGPChar szMsg[1024];
							
							PGPLoadString(g_hinst, IDS_EXE, szTitle, sizeof(szTitle)/sizeof(PGPChar));
							PGPLoadString(g_hinst, IDS_Q_HASATTACHMENT, szMsg, sizeof(szMsg)/sizeof(PGPChar));
							
							if (MessageBox(hwndMain, szMsg, szTitle, MB_YESNO)
								== IDNO)
							{
								free(szBuffer);
								return EMSR_UNKNOWN_FAIL;
							}

							break;
						}
					}

					if (szLineEnd != NULL)
					{
						szLine = szLineEnd + 2;
						szLineEnd = strstr(szLine, "\r\n");
					}
					else
						szLine = NULL;
				}

				szHeaderEnd = strstr(szBuffer, "\r\n\r\n");
				if (szHeaderEnd != NULL)
				{
					if (strlen(szHeaderEnd) > 4)
					{
						cOldEnd = szHeaderEnd[4];
						szHeaderEnd[4] = 0;
					}
				}

				if (strstr(szBuffer, "Mime-Version: ") &&
					strstr(szBuffer, "Content-Type: multipart") &&
					strstr(szBuffer, "protocol=\"application/pgp"))
				{
					if (bManual)
					{
						PGPChar szTitle[255];
						PGPChar szMsg[1024];

						PGPLoadString(g_hinst, IDS_TITLE_PGPERROR, szTitle, sizeof(szTitle)/sizeof(PGPChar));
						PGPLoadString(g_hinst, IDS_E_PGPMIME, szMsg, sizeof(szMsg)/sizeof(PGPChar));

						MessageBox(hwndMain, szMsg, szTitle, MB_ICONEXCLAMATION);
						free(szBuffer);
						return EMSR_UNKNOWN_FAIL;
					}
					else
					{
						char *szBoundary;

						szBoundary = strstr(szBuffer, "boundary=");
						szBoundary += strlen("boundary=");
						if (*szBoundary != '\"')
						{
							char *szEndBoundary;
							DWORD dwBytes;

							szEndBoundary = szBoundary + 
												strcspn(szBoundary, ";\r\n");

							if (cOldEnd)
								szHeaderEnd[4] = cOldEnd;

							hCheckFile = CreateFile((PGPChar *) in_file, 
											GENERIC_WRITE, 0, NULL, 
											OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL, 
											NULL);

							WriteFile(hCheckFile, szBuffer,
								szBoundary - szBuffer, &dwBytes, NULL);
							WriteFile(hCheckFile, "\"", 1, &dwBytes, NULL);
							WriteFile(hCheckFile, szBoundary,
								szEndBoundary - szBoundary, &dwBytes, NULL);
							WriteFile(hCheckFile, "\"", 1, &dwBytes, NULL);
							WriteFile(hCheckFile, szEndBoundary,
								strlen(szEndBoundary), &dwBytes, NULL);
							CloseHandle(hCheckFile);
						}
					}
				}
				
				free(szBuffer);
			}

			error = DecryptVerifyFile(g_hinst, hwndMain, g_pgpContext, 
						g_tlsContext, szExe, szDll, (PGPChar *) in_file, 
						!bManual, FALSE, &szTempFile, &pOutput, &outSize,
						&bFYEO);

			if( IsntPGPError(error) ) 
			{
				PGPChar szHex[] = PGPTXT_MACHINE("00000000");
				PGPUTF8 szHexUTF8[] = PGPTXT_MACHINE8("00000000");
				PGPUInt32 nFileNum;
				int nFileStart;

				if ((bFYEO)||(PGPscGetSecureViewerPref((void *)g_pgpContext)))
					PGPscTempestViewer((void *)g_pgpContext,hwndMain,
						pOutput,outSize,bFYEO);

// Change from UTF8 to local or keep as local if high ASCII
				{
					HANDLE hFile;
					DWORD dwLength,dwBytes,memamt,dummy;
					char *szFileData=NULL;
					char *szFileLocal=NULL;

					hFile = CreateFile(szTempFile, GENERIC_READ, 0, NULL, 
								OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);

					dwLength = GetFileSize(hFile, NULL);

					szFileData = (char *) calloc(sizeof(char), dwLength+1);

					ReadFile(hFile, szFileData, dwLength, &dwBytes, NULL);
					CloseHandle(hFile);
					szFileData[dwLength] = 0;

					if(pgpIsntUTF8String (szFileData,dwLength))
					{
						// Just assume its high ascii
						CopyFile(szTempFile, out_file, FALSE);
					}
					else
					{
						pgpUTF8StringToLocal (0, 
							GetACP(), szFileData, dwLength, 
							NULL,0,&memamt);

						szFileLocal = (char *) calloc(sizeof(char), memamt+1);

						if(szFileLocal)
						{
							pgpUTF8StringToLocal (0, 
								GetACP(), szFileData, dwLength, 
								szFileLocal,memamt+1,&dummy);
						}

						hFile = CreateFile(out_file, GENERIC_WRITE, 0, NULL, 
									CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, 
									NULL);

						WriteFile(hFile, szFileLocal, strlen(szFileLocal),
							&dwBytes, NULL);

						CloseHandle(hFile);

						if(szFileLocal!=NULL)
						{
							memset(szFileLocal,0x00,memamt);
							free(szFileLocal);
						}
					}

					if(szFileData!=NULL)
					{
						memset(szFileData,0x00,dwLength);
						free(szFileData);
					}
				}
// End processing
//				CopyFile(szTempFile, out_file, FALSE);
				DeleteFile(szTempFile);

				if( out_mime )
				{
					ParseFileForMIMEType( out_file, out_mime );
				}

				if ((bFYEO)||(PGPscGetSecureViewerPref((void *)g_pgpContext)))
					pluginReturn = EMSR_UNKNOWN_FAIL;
				else
					pluginReturn = EMSR_OK;

				g_szWipeFile = (PGPChar *) calloc(1, (PGPStrLen(out_file) + 1)*sizeof(PGPChar));
				
				PGPStrCpy(g_szWipeFile, out_file);
				nFileStart = PGPStrLen(g_szWipeFile) - 10;
				memcpy(&(g_szWipeFile[nFileStart]), PGPTXT_MACHINE("tmp"), 3*sizeof(PGPChar));
				memcpy(&(szHex[5]), &(g_szWipeFile[nFileStart+3]), 3*sizeof(PGPChar));

#if UNICODE
				pgpUTF16StringToUTF8(szHex,kPGPUnicodeNullTerminated,
					szHexUTF8,sizeof(szHexUTF8)/sizeof(PGPUTF8), NULL);
#else
				PGPStrCpy(szHexUTF8,szHex);
#endif
				nFileNum = pgpHexToPGPUInt32(szHexUTF8);
				pgpPGPUInt32ToHex(nFileNum + 1, FALSE, szHexUTF8);
#if UNICODE
				pgpUTF8StringToUTF16(szHexUTF8,kPGPUnicodeNullTerminated,
					szHex,sizeof(szHex)/sizeof(PGPChar), NULL);
#else
				PGPStrCpy(szHex,szHexUTF8);
#endif
				memcpy(&(g_szWipeFile[nFileStart+3]), &(szHex[5]), 3*sizeof(PGPChar));

				SetTimer(g_hwndHidden, 1, 3000, NULL);
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


long CanTranslateMIMEType(
	long					transContext,
	emsMIMEtypeP			mimeType,
	const char*				type,
	const char*				subType)
{
	long	err	= EMSR_CANT_TRANS;
	
	if ( mimeType )
	{
		BOOL		haveValidContext;
		const long	kValidContextMask = EMSF_ON_DISPLAY | EMSF_ON_ARRIVAL;
		
		haveValidContext	= ( transContext & kValidContextMask ) != 0;
		
		if ( haveValidContext )
		{
			BOOL haveMatch;
			
			haveMatch = match_mime_type( mimeType, type, subType );

			if( haveMatch )
			{
				const char* PGPMimeParam = NULL;

				PGPMimeParam = get_mime_parameter(	mimeType, 
													"protocol");

				if(	PGPMimeParam && 
					(!strcmp(PGPMimeParam, "application/pgp-encrypted") || 
					!strcmp(PGPMimeParam, "application/pgp-signature") ) 
				)
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


BOOL CALLBACK WarnYesNoDlgProc(HWND hwndDlg, 
							   UINT uMsg, 
							   WPARAM wParam, 
							   LPARAM lParam)
{
	BOOL bReturnCode = FALSE;
	DWORD dwResult;

	switch(uMsg)
	{
	case WM_INITDIALOG:
		{
			SetWindowText(GetDlgItem(hwndDlg, IDC_WARNING), (PGPChar *) lParam);
			break;
		}

	case WM_COMMAND:
		switch (LOWORD(wParam))
		{
		case IDC_YESCONTINUE:
			if (IsDlgButtonChecked(hwndDlg, IDC_NOWARN) == BST_CHECKED)
				dwResult = CONTINUE_BIT;
			else
				dwResult = CONTINUE_BIT | WARN_BIT;

			EndDialog(hwndDlg, dwResult);
			bReturnCode = TRUE;
			break;

		case IDC_NOCONTINUE:
			if (IsDlgButtonChecked(hwndDlg, IDC_NOWARN) == BST_CHECKED)
				dwResult = 0;
			else
				dwResult = WARN_BIT;

			EndDialog(hwndDlg, dwResult);
			bReturnCode = TRUE;
			break;
		}
		break;
	}

	return(bReturnCode);
}


BOOL UIWarnYesNo(HWND hwnd, UINT nID, PGPChar8 *nPrefIndex)
{
	PGPChar szMessage[1024];
	PGPBoolean bWarn;
	BOOL bContinue;
	DWORD dwResult;
	PGPError err = kPGPError_NoErr;

	PGPLoadString(g_hinst, nID, szMessage, sizeof(szMessage)/sizeof(PGPChar));

	{
		PGPXMLPrefRef prefref = kInvalidPGPXMLPrefRef;

		err=PGPclLockClientLibXMLPrefRef(&prefref);

		if(IsntPGPError(err))
		{
			err = PGPGetXMLPrefBoolean(prefref, nPrefIndex, &bWarn);
			if (IsPGPError(err))
				bWarn = TRUE;

			PGPclReleaseClientLibXMLPrefRef(prefref,FALSE);
		}
	}	

	if (!bWarn)
		return TRUE;

	dwResult = DialogBoxParam(g_hinst, MAKEINTRESOURCE(IDD_WARN), 
					hwnd, (DLGPROC) WarnYesNoDlgProc, (LPARAM) szMessage);

	bWarn = (dwResult & WARN_BIT) == WARN_BIT;
	bContinue = (dwResult & CONTINUE_BIT) == CONTINUE_BIT;

	{
		PGPXMLPrefRef prefref = kInvalidPGPXMLPrefRef;

		err=PGPclLockClientLibXMLPrefRef(&prefref);

		if(IsntPGPError(err))
		{
			PGPSetXMLPrefBoolean(prefref, nPrefIndex, bWarn);

			PGPclReleaseClientLibXMLPrefRef(prefref,TRUE);
		}
	}	

	return bContinue;
}


/*__Editor_settings____

	Local Variables:
	tab-width: 4
	End:
	vi: ts=4 sw=4
	vim: si
_____________________*/

