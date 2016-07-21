/*____________________________________________________________________________
	Copyright (C) 2002 PGP Corporation
	All rights reserved.

	$Id: EudoraMailHeaders.c 9679 2002-08-06 20:11:24Z dallen $
____________________________________________________________________________*/

// System Headers
#include <windows.h>
#include <assert.h>
#include <stdio.h>

// Project Headers
#include "strstri.h"
#include "EudoraMailHeaders.h"

int GetEudoraAddressFromNickname(char *pNickname, 
								 char *pAddress, 
								 int nAddressSize);

void GetEudoraHeaders( HWND hwndHeaders, PHEADERDATA phd)
{
	HWND hwndTo = NULL;
	HWND hwndFrom = NULL;
	HWND hwndSubject = NULL;
	HWND hwndCC = NULL;
	HWND hwndBCC = NULL;
	HWND hwndAttach = NULL;
	long FieldLength = 0;

	if( hwndHeaders  && phd )
	{
		// Find the windows we are interested in...
		hwndTo		= FindWindowEx(hwndHeaders, NULL,		"Edit", NULL);
		hwndFrom	= FindWindowEx(hwndHeaders, hwndTo,		"Edit", NULL);
		hwndSubject = FindWindowEx(hwndHeaders, hwndFrom,	"Edit", NULL);
		hwndCC		= FindWindowEx(hwndHeaders, hwndSubject,"Edit", NULL);
		hwndBCC		= FindWindowEx(hwndHeaders, hwndCC,		"Edit", NULL);
		hwndAttach	= FindWindowEx(hwndHeaders, hwndBCC,	"Edit", NULL);

		// Get all the recipients...
		if( hwndTo )
		{
			FieldLength = GetWindowTextLength( hwndTo );
			FieldLength += 2;

			phd->pTo = (char*)malloc( FieldLength );

			if( phd->pTo )
			{
				GetWindowText( hwndTo, phd->pTo, FieldLength + 2);
				//MessageBox(NULL, pTo, "pTo", MB_OK);
			}
		}

		if( hwndCC )
		{
			FieldLength = GetWindowTextLength( hwndCC );
			FieldLength += 2;

			phd->pCC = (char*)malloc( FieldLength );

			if( phd->pCC )
			{
				GetWindowText( hwndCC, phd->pCC, FieldLength + 2);
				//MessageBox(NULL, pCC, "pCC", MB_OK);
			}
		}

		if( hwndBCC )
		{
			FieldLength = GetWindowTextLength( hwndBCC );
			FieldLength += 2;

			phd->pBCC = (char*)malloc( FieldLength );

			if( phd->pBCC )
			{
				GetWindowText( hwndBCC, phd->pBCC, FieldLength + 2);
				//MessageBox(NULL, pBCC, "pBCC", MB_OK);
			}
		}

		// get attachments
		if(hwndAttach)
		{
			FieldLength = GetWindowTextLength( hwndAttach );
			FieldLength += 2;

			phd->pAttachments = (char*)malloc( FieldLength );

			if( phd->pAttachments )
			{
				GetWindowText( hwndAttach, 
					phd->pAttachments, 
					FieldLength + 2);
			}
		}

		// get subject
		if(hwndSubject)
		{
			FieldLength = GetWindowTextLength( hwndSubject );
			FieldLength += 2;

			phd->pSubject = (char*)malloc( FieldLength );

			if( phd->pSubject )
			{
				GetWindowText( hwndSubject, phd->pSubject, FieldLength + 2);
			}
		}


		// get From
		if(hwndFrom)
		{
			FieldLength = GetWindowTextLength( hwndFrom );
			FieldLength += 2;

			phd->pFrom = (char*)malloc( FieldLength );

			if( phd->pFrom )
			{
				GetWindowText( hwndFrom, phd->pFrom, FieldLength + 2);
			}
		}
	}
}


DWORD CreateRecipientListFromEudoraHeaders(	PHEADERDATA phd, 
											char*** Recipients)
{
	int RecipientBufferLength = 0;
	char* pAddresses = NULL;
	DWORD numRecipients = 0;

	// initialize pNumAddresses and ppPGPRecipientList
	*Recipients = NULL;

	// calculate initial length we might need for buffer
	if(phd->pTo)
	{
		RecipientBufferLength += strlen(phd->pTo);
	}

	if(phd->pCC)
	{
		RecipientBufferLength += strlen(phd->pCC);
	}


	if(phd->pBCC)
	{
		RecipientBufferLength += strlen(phd->pBCC);
	}

	// make room for trailing NULL
	RecipientBufferLength += 1;

	// now strip out real email addresses and 
	// place in one list
	pAddresses = (char*) malloc( RecipientBufferLength + 3 );

	if(pAddresses)
	{
		char* pToken = NULL;
		char* pQuoted = NULL;
		BOOL bInQuotes = FALSE;

		wsprintf(pAddresses, "%s,%s,%s", phd->pTo, phd->pCC, phd->pBCC);
	
		// iterate through the comma delimited list
		pToken = strtok(pAddresses,",");
 
		while(pToken)
		{
			char* pAtSymbol = NULL;

			if(!(*Recipients))
			{
				*Recipients = (char**) malloc(sizeof(char*));
			}
			else
			{
				*Recipients = realloc(*Recipients, 
						( sizeof(char*) * numRecipients) + sizeof(char*) );
			}

			if (!bInQuotes)
			{
				// strip off leading spaces or tabs
				while( *pToken == ' ' || *pToken == '\t')
				{
					pToken++;
				}
				
				// strip off trailing spaces or tabs
				while( *(pToken + strlen(pToken) - 1) == ' ' || 
						*(pToken + strlen(pToken) - 1) == '\t')
				{
					memmove(pToken + 1, pToken, strlen(pToken) - 1);
					pToken++;
				}
			}

			//MessageBox(NULL, pToken, "pToken", MB_OK);

			if ((!bInQuotes && (*pToken == '\"')) || bInQuotes)
			{
				char *pQuote;
				
				pQuote = pToken;
				
				do
				{
					pQuote++;
				}
				while (*pQuote && (*pQuote != '\"'));
				
				if (*pQuote == 0)
				{
					if (!bInQuotes)
						pQuoted = (char *) calloc(sizeof(char), 
											strlen(pToken) + 2);
					else
						pQuoted = realloc(pQuoted, strlen(pQuoted) + 
									strlen(pToken) + 2);
					
					strcat(pQuoted, pToken);
					strcat(pQuoted, ",");
					pToken = strtok(NULL,",");
					bInQuotes = TRUE;
					continue;
				}
				else
				{
					if (!bInQuotes)
						pQuoted = (char *) calloc(sizeof(char), 
											strlen(pToken) + 1);
					else
						pQuoted = realloc(pQuoted, strlen(pQuoted) + 
									strlen(pToken) + 1);

					strcat(pQuoted, pToken);
					pToken = pQuoted;
					bInQuotes = TRUE;
				}
			}

			// see if we can find an email address
			pAtSymbol = strchr(pToken, '@');

			if( pAtSymbol )
			{
				char* pBeginning = NULL;
				char* pEnd = NULL;
					
				// find the beginning and ending of the email address
				pBeginning = pAtSymbol;
				pEnd = pAtSymbol;

				while(	!isspace(*pBeginning) && 
						*(pBeginning - 1)!= '<' && 
						pBeginning > pToken)
				{
					pBeginning--;
				}

				while(!isspace(*pEnd) && *pEnd != '>' && *pEnd != 0x00)
				{
					//MessageBox(NULL, pEnd, "pEnd", MB_OK);
					pEnd++;
				}

				*pEnd = 0x00;
				
				if(*Recipients)
				{

					(*Recipients)[numRecipients] = 
									(char*) malloc(strlen(pBeginning) + 1); 

					strcpy((*Recipients)[numRecipients++], pBeginning);
				}

				//MessageBox(NULL, pRawAddresses, "Address", MB_OK);
			}
			else // see if this is an alias
			{
				char szAddress[512];

				if(GetEudoraAddressFromNickname(pToken, 
												szAddress, 
												sizeof(szAddress)) == 0)
				{
					if( *Recipients )
					{
						(*Recipients)[numRecipients] = 
									(char*) malloc(strlen(szAddress) + 1); 

						strcpy((*Recipients)[numRecipients++], szAddress);
					}
				}
				else // append the default extension to the name
				{
					char szDefaultDomain[256];
					char ApplicationPath[3 * MAX_PATH];
					HKEY hkey;
					char szKey[] = "Software\\Qualcomm\\Eudora\\"
									"CommandLine\\current";
					char* pEudoraPath = NULL;

					// Get the Install Path from the Registry
					if(ERROR_SUCCESS == RegOpenKeyEx(HKEY_CLASSES_ROOT,
								szKey,
								0, 
								KEY_ALL_ACCESS,
								&hkey))
					{
						DWORD Size,Type;

						Size = sizeof(ApplicationPath);
						RegQueryValueEx(hkey, 
										NULL, 
										0, 
										&Type, 
										(BYTE*)ApplicationPath, 
										&Size);

						pEudoraPath = strrchr(ApplicationPath, 0x20);

						if(pEudoraPath)
						{
							pEudoraPath++; // move past the space
						}

						RegCloseKey(hkey);
					}

					GetPrivateProfileString(
						"Settings", 
						"DomainQualifier", 
						"",
						szDefaultDomain, 
						sizeof(szDefaultDomain),
						pEudoraPath);

					strcpy(szAddress, pToken);

					// check to see if the string is empty or not
					if(*szDefaultDomain)
					{
						strcat(szAddress, "@");
						strcat(szAddress, szDefaultDomain);
					}

					if( *Recipients )
					{
						(*Recipients)[numRecipients] = 
									(char*) malloc(strlen(szAddress) + 1); 

						strcpy((*Recipients)[numRecipients++], szAddress);
					}
				}
			}

			if (bInQuotes)
			{
				bInQuotes = FALSE;
				free(pQuoted);
			}

			pToken = strtok(NULL,",");
		}
	}

	if( pAddresses )
	{ 
		free( pAddresses);
	}

	return numRecipients;
}


#define MAX_LINEBUF 500

//
// returns: -1 file error
//			-2 record too large (>MAX_LINEBUF bytes)
//			 1 not found
//			 0 success

int GetEudoraAddressFromNickname(char *pNickname, 
								 char *pAddress, 
								 int nAddressSize)
{
	FILE *fh;
	char buffer[MAX_LINEBUF+1];
	char *pEOL;
	char *pSystemCommandLine, *pEudoraPath = NULL, *pEudoraExe;
	int	 nNicknameLen;
	int	 nOffset;
	int	 rc = 1;

	assert(pNickname);
	assert(pAddress);

	//DebugBreak();

	nNicknameLen = strlen(pNickname);

	//Get the pointer to the system's static buf:
	pSystemCommandLine = GetCommandLine();

	//plus 2 because "nndbase.txt" is one character more than "eudora.exe".
	if((pEudoraPath = (char*)malloc(strlen(pSystemCommandLine) + 2)))
	{
		strcpy(pEudoraPath, pSystemCommandLine);
		if((pEudoraExe = strstri(pEudoraPath, "\\eudora.exe")))
		{
			*pEudoraExe = '\0';
			strcat(pEudoraPath, "\\nndbase.txt");
		}
		else
		{
			free(pEudoraPath);
			pEudoraPath = NULL;
		}
	}

	if(pEudoraPath)
	{
		//Win95 puts this string in quotes.  #@&*^%@#^%!
		if(*pEudoraPath == '\"')
			nOffset = 1;
		else
			nOffset = 0;

		fh = fopen((pEudoraPath + nOffset), "rt");
		free(pEudoraPath);
	}
	else
	{
		fh = fopen("\\eudora\\nnbase.txt", "rt");
	}

	if (!fh) {
		return -1;
	}

	while (!feof(fh)) {
		if (fgets(buffer, MAX_LINEBUF, fh) == NULL) {
			break;
		}
		if ((pEOL = strchr(buffer, '\n')) == NULL) {
			// no end of line...line too big
			rc = -2;
			break;
		} else {
			// stamp over it with a null
			*pEOL = '\0';
		}


		nOffset = 6;
		// look for "alias ", that will follow the nickname and 
		// then the email address
		// alias nickname foo@bar.com

		if (!strncmp(buffer, "alias ", nOffset)) {
			// see if the nickname matches
			if (!strncmp(buffer+nOffset, pNickname, nNicknameLen) && 
				buffer[nOffset+nNicknameLen] == ' ') {
				// step past the nickname + white space, and 
				//copy the email address
				nOffset += nNicknameLen+1;
				strncpy(pAddress, buffer+nOffset, nAddressSize);
				//These things seem to have trailing commas a lot...
				if(*(pAddress + strlen(pAddress) - 1) == ',')
					*(pAddress + strlen(pAddress) - 1) = '\0';
				rc = 0;
				break;
			}
		}
	}
	fclose(fh);
	return(rc);
}


void SetEudoraAttachments(HWND hwndHeaders, char* pAttach)
{
	HWND hwndTo = NULL;
	HWND hwndFrom = NULL;
	HWND hwndSubject = NULL;
	HWND hwndCC = NULL;
	HWND hwndBCC = NULL;
	HWND hwndAttach = NULL;
	long FieldLength = 0;

	if( hwndHeaders  && pAttach )
	{
		// Find the windows we are interested in...
		hwndTo		= FindWindowEx(hwndHeaders, NULL,		"Edit", NULL);
		hwndFrom	= FindWindowEx(hwndHeaders, hwndTo,		"Edit", NULL);
		hwndSubject = FindWindowEx(hwndHeaders, hwndFrom,	"Edit", NULL);
		hwndCC		= FindWindowEx(hwndHeaders, hwndSubject,"Edit", NULL);
		hwndBCC		= FindWindowEx(hwndHeaders, hwndCC,		"Edit", NULL);
		hwndAttach	= FindWindowEx(hwndHeaders, hwndBCC,	"Edit", NULL);

		SetWindowText(hwndAttach, pAttach);
	}
}

void FreeHeaderData(PHEADERDATA phd)
{
	if( phd->pTo )
	{
		free(phd->pTo);
	}

	if( phd->pFrom)
	{
		free(phd->pFrom);
	}

	if( phd->pCC )
	{
		free(phd->pCC);
	}

	if( phd->pBCC )
	{
		free(phd->pBCC);
	}

	if( phd->pAttachments )
	{
		free(phd->pAttachments);
	}

	if( phd->pSubject )
	{
		free(phd->pSubject);
	}

	if( phd->pEmailSig )
	{
		free(phd->pEmailSig);
	}

}


/*__Editor_settings____

	Local Variables:
	tab-width: 4
	End:
	vi: ts=4 sw=4
	vim: si
_____________________*/
