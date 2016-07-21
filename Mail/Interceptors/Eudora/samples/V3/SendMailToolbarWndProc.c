/*____________________________________________________________________________
	Copyright (C) 2002 PGP Corporation
	All rights reserved.

	$Id: SendMailToolbarWndProc.c 9679 2002-08-06 20:11:24Z dallen $
____________________________________________________________________________*/

// System Headers
#include <windows.h>
#include <richedit.h>
#include <commctrl.h>

// Shared Headers
#include "pgpClientLib.h"
#include "EncryptSign.h"

// Project Headers
#include "PGPDefinedMessages.h"
#include "RichEdit_IO.h"
#include "resource.h"
#include "EudoraMailHeaders.h"
#include "TranslatorUtils.h"



// Global Variables
extern HINSTANCE		g_hinst;
extern HWND				g_hwndEudoraStatusbar;
extern HWND				g_hwndEudoraMainWindow;
extern PGPContextRef	g_pgpContext;
extern PGPtlsContextRef g_tlsContext;

BOOL g_bEncrypt = FALSE;
BOOL g_bSign = FALSE;
BOOL g_bInitializeButtons = FALSE;
BOOL g_bSendingMail = FALSE;
HWND g_hwndSendToolbar = NULL;

BOOL EncryptSignRichEditText(char** ppBuffer, 
							 long* pLength, 
							 BOOL bEncrypt, 
							 BOOL bSign, 
							 char** pAddresses, 
							 long NumAddresses,
							 char* pAttachments,
							 char** pOutAttachments);

PGPError EncryptAttachments(char* pInAttachments, 
							char** ppOutAttachments,
							PGPclRecipientDialogStruct *prds,
							PGPOptionListRef userOptions,
							BOOL bEncrypt,
							BOOL bSign);

BOOL VerifyOutputFileName(char* filename);


LRESULT WINAPI 
SendMailToolbarWndProc(	HWND hwnd, 
						UINT msg, 
						WPARAM wParam, 
						LPARAM lParam)
{
	WNDPROC lpOldProc;

	lpOldProc = (WNDPROC)GetProp( hwnd, "oldproc" );
   
	switch(msg)
	{
		case WM_DESTROY:   
		{
			//  Put back old window proc and
			SetWindowLong( hwnd, GWL_WNDPROC, (DWORD)lpOldProc );

			// remove window property
			RemoveProp( hwnd, "oldproc" ); 
			break;
		}

		case WM_NOTIFY:
		{
			static char szTooltip[256] = {0x00};
			LPTOOLTIPTEXT lpToolTipText = (LPTOOLTIPTEXT) lParam;;

			switch (((LPNMHDR)lParam)->code)
			{
				case TTN_NEEDTEXTA:
				{
					if(lpToolTipText->hdr.idFrom >= IDC_DECRYPT && 
						lpToolTipText->hdr.idFrom <= IDC_MIME )
					{
						int StringId =	lpToolTipText->hdr.idFrom -  
										WM_USER - 1000;

						LoadString (g_hinst, 
									StringId,
									szTooltip,
									sizeof(szTooltip));

						lpToolTipText->lpszText = szTooltip;

						if(g_hwndEudoraStatusbar)
						{
							SendMessage(g_hwndEudoraStatusbar,
										SB_SETTEXT,
										(WPARAM)0,
										(LPARAM)szTooltip);
						}
					
						return 0;
					}
				}

				case TTN_NEEDTEXTW:
				{
					static wchar_t wideBuf[256];

					if(lpToolTipText->hdr.idFrom >= IDC_DECRYPT && 
						lpToolTipText->hdr.idFrom <= IDC_MIME )
					{
						int StringId = lpToolTipText->hdr.idFrom -  
										WM_USER - 1000;
					
    					LoadString (g_hinst, 
									StringId,
									szTooltip,
									sizeof(szTooltip));

						MultiByteToWideChar(CP_ACP, 
											MB_PRECOMPOSED, 
											szTooltip, 
											-1, 
											wideBuf, 
											256);

						lpToolTipText->lpszText = (char*)wideBuf;

						if(g_hwndEudoraStatusbar)
						{
							SendMessage(g_hwndEudoraStatusbar,
										SB_SETTEXT,
										(WPARAM)0,
										(LPARAM)szTooltip);
						}

						return 0;
					}
				}
			}
			break;
		}
	
		case WM_COMMAND:
		{
			switch(wParam)
			{
				case IDC_SENDMAIL:
				{
					HWND hwndSendToolbar = NULL;
					BOOL bEncrypt = FALSE;
					BOOL bSign = FALSE;
					BOOL bPGPMIME = FALSE;
					BYTE KeyboardStatus[256];
					//char temp[100];

					// Did the user have the shift key pressed 
					// when they sent mail?
					GetKeyboardState( KeyboardStatus );

					// Get our Toolbar
					hwndSendToolbar =  FindWindowEx(hwnd, 
													NULL, 
													"ToolbarWindow32", 
													NULL);

					if(hwndSendToolbar)
					{	
						g_bSendingMail = TRUE;
						g_hwndSendToolbar = hwndSendToolbar;

						// set up the Eudora Plugin Menu correctly
						SendMessage(hwnd, 
									WM_COMMAND, 
									(WPARAM)IDC_PLUGINMENU, 
									0);
						
						// See if the user wants us to use PGPMIME
						if(SendMessage(	hwndSendToolbar,
										TB_ISBUTTONCHECKED,
										(WPARAM)IDC_MIME,
										0))
						{
							bPGPMIME = TRUE;
						}

						// See if the user wants us encrypt the message
						if(SendMessage(	hwndSendToolbar,
										TB_ISBUTTONCHECKED,
										(WPARAM)IDC_ENCRYPT,
										0))
						{
							bEncrypt = TRUE;
						}

						// See if the user wants us sign the message
						if(SendMessage(	hwndSendToolbar,
										TB_ISBUTTONCHECKED,
										(WPARAM)IDC_SIGN,
										0))
						{
							bSign = TRUE;
						}
					}
					else
					{
						int err;
						char szMessage[256];
						char szTitle[256];

						LoadString(g_hinst, IDS_E_SENDMAIL, szMessage, 
							sizeof(szMessage));
						LoadString(g_hinst, IDS_E_SENDMAILTITLE, szTitle, 
							sizeof(szTitle));

						err = MessageBox(NULL, 
								szMessage, 
								szTitle,	
								MB_YESNO|MB_ICONSTOP);

						if(err == IDNO)
						{
							return 0;
						}
					}	

					if(!bPGPMIME)
					{
						if(!SendMessage(hwnd, WM_COMMAND, IDC_JUSTDOIT, 0))
						{
							return 0;
						}
						else
						{
							LRESULT lresult;
							BYTE oldKeyboardStatus[256];
							
							GetKeyboardState( oldKeyboardStatus );
							SetKeyboardState( KeyboardStatus );

							lresult =  CallWindowProc(	lpOldProc, 
														hwnd, 
														msg, 
														wParam, 
														lParam );
							
							// reset these keys so they don't "stick"
							oldKeyboardStatus[VK_SHIFT]		= 0;
							oldKeyboardStatus[VK_CONTROL]	= 0;
							oldKeyboardStatus[VK_MENU]		= 0;

							SetKeyboardState( oldKeyboardStatus );

							return lresult;
						}
					}
					
					break;
				}

				case IDC_JUSTDOIT:
				{
					HWND hwndSendToolbar = NULL;
					BOOL bEncrypt = FALSE;
					BOOL bSign = FALSE;
					BOOL bEncryptSuccessful = FALSE;
					HCURSOR hCursor, hOldCursor;
					//char temp[100];

					// Get our Toolbar
					hwndSendToolbar =  FindWindowEx(hwnd, 
													NULL, 
													"ToolbarWindow32", 
													NULL);

					if(hwndSendToolbar)
					{		
						// See if the user wants us encrypt the message
						if(SendMessage(	hwndSendToolbar,
										TB_ISBUTTONCHECKED,
										(WPARAM)IDC_ENCRYPT,
										0))
						{
							bEncrypt = TRUE;
						}

						// See if the user wants us sign the message
						if(SendMessage(	hwndSendToolbar,
										TB_ISBUTTONCHECKED,
										(WPARAM)IDC_SIGN,
										0))
						{
							bSign = TRUE;
						}
					}

					// does the user want us to do anything to the message
					if(bEncrypt || bSign )
					{
						BOOL bSelectedText = FALSE;
						CHARRANGE chRange = {0,0};
						HWND hwndParent = NULL;
						HWND hwndRichEdit = NULL;
						HWND hwndAfxWnd = NULL;
						HWND hwndHeaders = NULL;
						char* pRichEditText = NULL;
						char** pPGPRecipients = NULL;
						long NumAddresses = 0;
						long nChar = 0;
						HEADERDATA hd;

						// zero out HEADERDATA struct
						memset(&hd, 0x00, sizeof(hd));

						// Find the windows we are interested in...
						hwndParent		= GetParent(hwnd);
						hwndAfxWnd		= FindWindowEx(	hwndParent, 
														NULL, 
														"AfxMDIFrame40", 
														NULL);

						if(!hwndAfxWnd) // 3.04 and 3.05 use new mfc versions
						{
							hwndAfxWnd	= FindWindowEx(	hwndParent, 
														NULL, 
														"AfxMDIFrame42", 
									 					NULL);
							if(!hwndAfxWnd) 
							{
								char szMessage[256];

								LoadString(g_hinst, IDS_E_OLDVERSION, 
									szMessage, sizeof(szMessage));

								MessageBox(NULL,
									szMessage,
									0, 
									MB_OK);

								return 0;
							}

						}						

						hwndRichEdit	= FindWindowEx(	hwndAfxWnd, 
														NULL, 
														"RICHEDIT", 
														NULL);

						hwndHeaders		= FindWindowEx(	hwndAfxWnd, 
														NULL, 
														"#32770", //Dialog 
														NULL);

						// See if the user has selected text in the window
						SendMessage(hwndRichEdit,  
									EM_EXGETSEL, 
									(WPARAM)0,
									(LPARAM) 
									&chRange);

						bSelectedText = chRange.cpMax - chRange.cpMin;

						// if not context menu invoked then we don't 
						// want to do selected text. 
						// this will protect users 
						if(lParam != SENT_FROM_CONTEXT_MENU)
						{
							bSelectedText = FALSE;
						}

						if(!bSelectedText)
						{
							RECT rect;

							// there is a bug in the richedit control... 
							// so if there is no selected text click in 
							// the lower right corner real quick to 
							// clear up the bug...

							GetClientRect(hwndRichEdit, &rect);
							SendMessage(hwndRichEdit, 
										WM_LBUTTONDOWN, 
										MK_LBUTTON, 
										MAKELPARAM(rect.right - 5, 
										rect.bottom -5 )); 

							SendMessage(hwndRichEdit, 
										WM_LBUTTONUP, 
										MK_LBUTTON,  
										MAKELPARAM(rect.right - 5 , 
										rect.bottom -5 )); 
						}

						// This could take awhile... give some feedback.
						hCursor = LoadCursor(NULL, IDC_WAIT);
						hOldCursor = SetCursor(hCursor);

						// get text of message
						pRichEditText = GetRichEditContents (	hwndRichEdit, 
																&nChar, 
																FALSE, 
																bSelectedText);

						// get the email headers
						GetEudoraHeaders( hwndHeaders, &hd);

						// now create an appropriate list for pgplib
						NumAddresses = CreateRecipientListFromEudoraHeaders(
														&hd, 
														&pPGPRecipients);

						if( pRichEditText )
						{				
							char* pInAttachments = "";
							char* pOutAttachments = NULL;

							// if context menu invoked then we don't 
							// want to do attachments. 
							if(lParam != SENT_FROM_CONTEXT_MENU)
							{
								pInAttachments = hd.pAttachments;
							}

							// length might have changed
							nChar = strlen(pRichEditText);

							// Encrypt Text
							bEncryptSuccessful = EncryptSignRichEditText(	
															&pRichEditText, 
															&nChar, 
															bEncrypt, 
															bSign, 
															pPGPRecipients, 
															NumAddresses,
															pInAttachments,
															&pOutAttachments);

							if( bEncryptSuccessful )
							{
								SetRichEditContents (	hwndRichEdit, 
														pRichEditText,
														FALSE, 
														bSelectedText);

								if(pOutAttachments)
								{
									// set the attachments
									SetEudoraAttachments(	hwndHeaders, 
															pOutAttachments);

									free(pOutAttachments);
								}
								else
								{
									// Wait for about .5 seconds 
									// so user can see encrypted text...
									Sleep(500);
								}
							}
						}
						
						// want to reset buttons for context menu
						// regardless of success or failure
						if(	bEncryptSuccessful || 
							lParam == SENT_FROM_CONTEXT_MENU)
						{
							// Reset the buttons
							SendMessage(hwndSendToolbar,
										TB_CHECKBUTTON,
										(WPARAM)IDC_ENCRYPT, 
										MAKELPARAM(FALSE,0));
							
							HeapFree(GetProcessHeap(), 0, pRichEditText);
						} 

						// want to reset buttons for context menu
						// regardless of success or failure
						if(	bEncryptSuccessful || 
							lParam == SENT_FROM_CONTEXT_MENU)
						{
							// Reset the buttons
							SendMessage(hwndSendToolbar,
										TB_CHECKBUTTON,
										(WPARAM)IDC_ENCRYPT, 
										MAKELPARAM(FALSE,0));

							SendMessage(hwndSendToolbar,
										TB_CHECKBUTTON,
										(WPARAM)IDC_SIGN,
										MAKELPARAM(FALSE, 0));

							SendMessage(hwndSendToolbar,
										TB_ENABLEBUTTON,
										(WPARAM)IDC_JUSTDOIT,
										MAKELPARAM(FALSE, 0));
						}

						// clean up after ourselves
						FreeRecipientList(pPGPRecipients, NumAddresses);

						FreeHeaderData(&hd);

						SetCursor(hOldCursor);
					}
					else
					{
						// we 'successfully' did nothing to the message
						bEncryptSuccessful = TRUE;
					}

					return bEncryptSuccessful;
					break;
				}

				case IDC_KEYMGR:
				{
					char szPath[MAX_PATH];
					char szPGPkeys[256];
					PGPError error = kPGPError_NoErr;

					LoadString(g_hinst, IDS_PGPKEYSEXE, szPGPkeys,
						sizeof(szPGPkeys));

					error = PGPclGetPath (kPGPclPGPkeysExeFile,
						szPath, sizeof(szPath));

					if( IsntPGPError(error) )
					{
						PGPclExecute (szPath, SW_SHOW);
					}
					else
					{
						char szError[256];

						LoadString(g_hinst, IDS_E_LAUNCHPGPKEYS, szError,
							sizeof(szError));

						MessageBox(NULL, 
							szError, 
							0, 
							MB_OK);
					}
					
					break;
				}
			}
		}
		
		case WM_ENTERIDLE:
		{
			if( g_bSendingMail && g_hwndSendToolbar && wParam == MSGF_MENU )
			{
				g_bInitializeButtons = FALSE;
				g_bSendingMail = FALSE;
				g_hwndSendToolbar = NULL;
				PostMessage(hwnd, WM_KEYDOWN, (WPARAM)VK_ESCAPE, 0x001c0001); 
				PostMessage(hwnd, WM_KEYUP, (WPARAM)VK_ESCAPE, 0x001c0001); 
			}

			break;
		}
	
		case WM_DRAWITEM:
		{
			// control identifier 
			UINT idCtl = (UINT) wParam;	
			// item-drawing information
			LPDRAWITEMSTRUCT lpdis = (LPDRAWITEMSTRUCT) lParam;  
			//char szString[256] = {0x00};

			if( g_bSendingMail )
			{
				if(lpdis && lpdis->CtlType == ODT_MENU)
				{
					if(*((void**)(lpdis->itemData + 4)))
					{
						WPARAM wCommand = 0;
						BOOL bItemChecked = FALSE;

						// see if it is already checked
						if((lpdis->itemState & ODS_CHECKED)) 
						{
							bItemChecked = TRUE;
						}

						// see which plugin this menuitem represents
						if( !strcmp(*((char**)(lpdis->itemData + 4)), 
										"PGP plugin 1 (use toolbar)") )
						{
							wCommand = IDC_ENCRYPT;

							// we might be getting a window from a cancel...
							if( g_bInitializeButtons && bItemChecked) 
							{
								g_bEncrypt = TRUE;
							}
						}
						else if( !strcmp(*((char**)(lpdis->itemData + 4)), 
											"PGP plugin 2 (use toolbar)") )
						{
							wCommand = IDC_SIGN;

							// we might be getting a window from a cancel...
							if( g_bInitializeButtons && bItemChecked) 
							{
								g_bSign	= TRUE;
							}
						}

						if( !g_bInitializeButtons )
						{
							// make sure the window handle is valid
							if( g_hwndSendToolbar) 
							{
								if( wCommand ) // is this one of our menus
								{
									BOOL bCommandPressed = FALSE;
									BOOL bPGPMIMEPressed = FALSE;

									bCommandPressed = SendMessage(	
														g_hwndSendToolbar, 
														TB_ISBUTTONCHECKED, 
														wCommand, 
														0);

									bPGPMIMEPressed = SendMessage(	
														g_hwndSendToolbar, 
														TB_ISBUTTONCHECKED, 
														IDC_MIME, 
														0);

									if( (bCommandPressed && 
										!bItemChecked && bPGPMIMEPressed) || 
										(bCommandPressed && bItemChecked && 
										!bPGPMIMEPressed) ||
										(!bCommandPressed && bItemChecked))
									{
										SendMessage(hwnd, 
													WM_COMMAND, 
													(WPARAM)lpdis->itemID, 
													0);
									}
								}
							}
						}
					}
				}
			}
			break;
		}
	} 
	
	//  Pass all non-custom messages to old window proc
	return CallWindowProc(lpOldProc, hwnd, msg, wParam, lParam ) ;
}

BOOL 
EncryptSignRichEditText(char** ppBuffer, 
						long* pLength, 
						BOOL bEncrypt, 
						BOOL bSign, 
						char** pAddresses, 
						long NumAddresses,
						char* pAttachments,
						char** ppOutAttachments)
{
	BOOL ReturnValue = FALSE;
	void* pOutput = NULL;
	long outSize = 0;
	PGPError error = kPGPError_NoErr;
	PGPOptionListRef userOptions = NULL;
	PGPclRecipientDialogStruct *prds = NULL;	
	char szExe[256];
	char szDll[256];

	LoadString(g_hinst, IDS_EXE, szExe, sizeof(szExe));
	LoadString(g_hinst, IDS_DLL, szDll, sizeof(szDll));

	// allocate a recipient dialog structure
	prds = (PGPclRecipientDialogStruct *) 
			calloc(sizeof(PGPclRecipientDialogStruct), 1);

	if (!prds)
	{
		PGPclErrorBox(g_hwndEudoraMainWindow, kPGPError_OutOfMemory);
		return FALSE;
	}

	error =	PGPclOpenDefaultKeyrings(g_pgpContext, 
				kPGPOpenKeyDBFileOptions_Mutable, &(prds->keydbOriginal));

	if (IsPGPError(error))
		error =	PGPclOpenDefaultKeyrings(g_pgpContext, 
					kPGPOpenKeyDBFileOptions_None, &(prds->keydbOriginal));

	if (IsPGPError(error))
	{
		char szTitle[255];
		char szBuffer[1024];
		
		LoadString(g_hinst, IDS_DLL, szTitle, 254);
		LoadString(g_hinst, IDS_Q_NOKEYRINGS, szBuffer, 1023);
		
		if (MessageBox(g_hwndEudoraMainWindow, szBuffer, szTitle, MB_YESNO))
		{
			char szPath[MAX_PATH];
			
			PGPclGetPath(kPGPclPGPkeysExeFile, szPath, MAX_PATH-1);
			PGPclExecute(szPath, SW_SHOW);
		}
		
		return S_FALSE;
	}

	if(prds && bEncrypt)
	{
		char szTitle[256]		= {0x00};	// title for recipient dialog
		UINT recipientReturn	= FALSE;	// recipient dialog result

		LoadString(GetModuleHandle("PGPplugin.dll"), 
			IDS_TITLE_RECIPIENTDIALOG, szTitle, sizeof(szTitle));

		if( IsntPGPError(error) )
		{
			prds->context			= g_pgpContext;
			prds->tlsContext		= g_tlsContext;
			prds->Version			= kPGPCurrentRecipVersion;
			prds->hwndParent		= g_hwndEudoraMainWindow;
			prds->szTitle			= szTitle;
			prds->dwOptions			= kPGPclASCIIArmor;	

			prds->dwDisableFlags	= kPGPclDisableWipeOriginal |
									  kPGPclDisableASCIIArmor |
									  kPGPclDisableSelfDecryptingArchive |
									  kPGPclDisableInputIsText;

			prds->dwNumRecipients	= NumAddresses;	
			prds->szRecipientArray	= pAddresses;

			// If shift is pressed, force the dialog to pop.
			if (GetAsyncKeyState( VK_CONTROL) & 0x8000)
				prds->dwDisableFlags |= kPGPclDisableAutoMode;

			// See who we wish to encrypt this to
			recipientReturn = PGPclRecipientDialog( prds );
		}

		if (prds->keydbAdded != NULL)
		{
			PGPUInt32 numKeys;
			PGPKeySetRef keySet;

			PGPNewKeySet(prds->keydbAdded, &keySet);
			PGPCountKeys(keySet, &numKeys);
			if (numKeys > 0)
				PGPclImportKeys(g_pgpContext, g_tlsContext, prds->hwndParent,
					keySet, prds->keydbOriginal, 
					kPGPclNoDialogForValidSingletons);

			PGPFreeKeySet(keySet);
			PGPFreeKeyDB(prds->keydbAdded);
			prds->keydbAdded = NULL;
		}

		if (!recipientReturn)
		{
			if (prds->keydbSelected != NULL)
				PGPFreeKeyDB(prds->keydbSelected);

			PGPFreeKeyDB(prds->keydbOriginal);
			free(prds);
			return FALSE;
		}
	}

	if( IsntPGPError(error) )
	{
		error = EncryptSignBuffer(g_hinst, g_hwndEudoraMainWindow,
					g_pgpContext, g_tlsContext, szExe, szDll,
					*ppBuffer, *pLength, prds, NULL, &userOptions, &pOutput,
					&outSize, bEncrypt, bSign, FALSE, FALSE);
	}
	else
	{
		PGPclEncDecErrorBox(g_hwndEudoraMainWindow, error);
	}

	*pLength = outSize;

	if( IsntPGPError(error) )
	{
		if( pOutput )
		{
			*ppBuffer = (char*)HeapReAlloc(	GetProcessHeap(), 
											HEAP_ZERO_MEMORY, 
											*ppBuffer, 
											*pLength + 1);

			if(*ppBuffer)
			{
				ReturnValue = TRUE;
				memcpy(*ppBuffer, (char*)pOutput, *pLength);
				*(*ppBuffer + *pLength) = 0x00; // NULL terminate the string
				memset(pOutput, 0x00, *pLength);
			}
			else 
			{
				error = kPGPError_OutOfMemory;
			}

			PGPFreeData(pOutput);
		}
	}
	
	// are there attachments?
	if(IsntPGPError(error) && pAttachments && *pAttachments) 
	{
		error = EncryptAttachments(	pAttachments, 
									ppOutAttachments,
									prds,
									userOptions,
									bEncrypt,
									bSign);

	}

	if(userOptions != NULL)
	{
		PGPFreeOptionList(userOptions);
	}

	if (prds)
	{
		if (prds->keydbSelected != NULL)
			PGPFreeKeyDB(prds->keydbSelected);

		PGPFreeKeyDB(prds->keydbOriginal);
		free(prds);
	}

	return ReturnValue;
}

PGPError EncryptAttachments(char* pInAttachments, 
							char** ppOutAttachments,
							PGPclRecipientDialogStruct *prds,
							PGPOptionListRef userOptions,
							BOOL bEncrypt,
							BOOL bSign)
{
	PGPError error = kPGPError_NoErr;
	char in_file[MAX_PATH] = {0x00};
	char out_file[MAX_PATH] = {0x00};
	char* pInAttachBackup = NULL;
	char* token = NULL;
	long size = 0;
	char szExe[256];
	char szDll[256];

	LoadString(g_hinst, IDS_EXE, szExe, sizeof(szExe));
	LoadString(g_hinst, IDS_DLL, szDll, sizeof(szDll));

	size = strlen(pInAttachments);

	pInAttachBackup = malloc(size + 1);

	if( !pInAttachBackup )
	{
		return kPGPError_OutOfMemory;
	}
	else
	{
		strcpy(pInAttachBackup, pInAttachments);
	}

	*ppOutAttachments = malloc(size*2);

	if( !*ppOutAttachments )
	{
		free(pInAttachBackup);
		return kPGPError_OutOfMemory;
	}

	// NULL terminate
	**ppOutAttachments = 0x00;

	token = strtok(pInAttachments,";");

	while(token)
	{
		// strip off leading spaces or tabs
		while( *token == ' ' || *token == '\t')
		{
			token++;
		}

		strcpy(in_file, token);
		strcpy(out_file, token);
		strcat(out_file, ".pgp");

		if(VerifyOutputFileName(out_file))
		{
			strcat(*ppOutAttachments, out_file);
			strcat(*ppOutAttachments, ";");

			error = EncryptSignFile(g_hinst, g_hwndEudoraMainWindow, 
						g_pgpContext, g_tlsContext, szExe, szDll, in_file,
						prds, NULL, &userOptions, out_file, bEncrypt, bSign,
						TRUE);
		}
		else
		{
			strcpy(*ppOutAttachments, pInAttachBackup);
			error = kPGPError_UserAbort;
			break;
		}

		token = strtok(NULL,";");	
	}

	if(pInAttachBackup)
	{
		free(pInAttachBackup);
	}

	return error;
}

BOOL VerifyOutputFileName(char* filename)
{
	HANDLE hFileSearch = NULL;
	WIN32_FIND_DATA FindDataStruct;
	BOOL returnValue = FALSE;

	hFileSearch = FindFirstFile(filename, &FindDataStruct);

	if(hFileSearch != INVALID_HANDLE_VALUE)
	{
		BOOL userCancel = FALSE;
		OPENFILENAME saveFileName;
		char initialDir[MAX_PATH] = {0x00};
		char* slash = NULL;
		char finalFile[MAX_PATH] = {0x00};
		char defaultExtension[MAX_PATH] = {0x00};
		int fileStart = 0, fileExtensionStart = 0;

		strcpy(initialDir, filename );
		strcpy(finalFile, filename );

		slash = strrchr(initialDir, '\\');

		if(slash)
		{
			*slash = 0x00;
		}

		saveFileName.lStructSize=sizeof(saveFileName); 
		saveFileName.hwndOwner=NULL; 
	    saveFileName.hInstance=NULL; 
	    saveFileName.lpstrFilter=	"PGP Files (*.PGP, *.ASC)\0"
									"*.PGP;*.ASC\0"
									"All Files (*.*)\0"
									"*.*\0\0";
		saveFileName.lpstrCustomFilter=NULL; 
	    saveFileName.nMaxCustFilter=0; 
		saveFileName.nFilterIndex=1; 
  	    saveFileName.lpstrFile=finalFile; 
	    saveFileName.nMaxFile=MAX_PATH; 
	    saveFileName.lpstrFileTitle=NULL; 
		saveFileName.nMaxFileTitle=0; 
		saveFileName.lpstrInitialDir=initialDir; 
		saveFileName.lpstrTitle=NULL; 
		saveFileName.Flags= OFN_OVERWRITEPROMPT | 
							OFN_HIDEREADONLY | 
							OFN_NOREADONLYRETURN| 
							OFN_EXPLORER;
		saveFileName.nFileOffset=fileStart; 
		saveFileName.nFileExtension=fileExtensionStart; 
		saveFileName.lpstrDefExt=defaultExtension; 
		saveFileName.lCustData=(long)NULL; 
		saveFileName.lpfnHook=NULL;
		saveFileName.lpTemplateName=NULL; 

		returnValue = GetSaveFileName(&saveFileName);

		if(returnValue)
		{
			strcpy(filename, finalFile);
		}
		
		FindClose(hFileSearch);
	}
	else
	{
		returnValue =  TRUE;
	}


	return returnValue;

}


/*__Editor_settings____

	Local Variables:
	tab-width: 4
	End:
	vi: ts=4 sw=4
	vim: si
_____________________*/
