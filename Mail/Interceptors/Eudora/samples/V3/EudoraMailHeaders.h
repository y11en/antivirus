/*____________________________________________________________________________
	Copyright (C) 2002 PGP Corporation
	All rights reserved.

	$Id: EudoraMailHeaders.h 9679 2002-08-06 20:11:24Z dallen $
____________________________________________________________________________*/
#ifndef Included_EudoraMailHeaders_h	/* [ */
#define Included_EudoraMailHeaders_h


typedef struct _HEADERDATA
{
	char*	pTo; 
	char*	pFrom; 
	char*	pCC;
	char*	pBCC;
	char*	pSubject;
	char*	pAttachments;
	char*	pEmailSig;
	int		priority;
	int		receipt;

}HEADERDATA, *PHEADERDATA;

void 
GetEudoraHeaders(	HWND hwndHeaders, 
					PHEADERDATA phd);

DWORD CreateRecipientListFromEudoraHeaders(	PHEADERDATA phd, 
											char*** Recipients);

void SetEudoraAttachments(HWND hwndHeaders,
						  char* pAttach);


void FreeHeaderData( PHEADERDATA phd );

#endif /* ] Included_EudoraMailHeaders_h */


/*__Editor_settings____

	Local Variables:
	tab-width: 4
	End:
	vi: ts=4 sw=4
	vim: si
_____________________*/

