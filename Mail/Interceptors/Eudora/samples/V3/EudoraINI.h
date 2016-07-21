/*____________________________________________________________________________
	Copyright (C) 2002 PGP Corporation
	All rights reserved.

	$Id: EudoraINI.h 9679 2002-08-06 20:11:24Z dallen $
____________________________________________________________________________*/
#ifndef Included_EudoraINI_h
#define Included_EudoraINI_h


typedef struct _EUDORAMAPPINGS{
	char extension[10];
	char type[10];
	char creator[10];
	char mimetype[256];
	struct _EUDORAMAPPINGS* next;

}EUDORAMAPPINGS, *PEUDORAMAPPINGS;

void SetDefaultMailAction(void);
BOOL GetEudoraINIPath(char* Path);
BOOL GetEudoraUserId(char* buffer);
PEUDORAMAPPINGS LoadEudoraMappings(void);
	

#endif /* Included_EudoraINI_h */

/*__Editor_settings____

	Local Variables:
	tab-width: 4
	End:
	vi: ts=4 sw=4
	vim: si
_____________________*/

