/*____________________________________________________________________________
	Copyright (C) 2002 PGP Corporation
	All rights reserved.

	$Id: strstri.c 9679 2002-08-06 20:11:24Z dallen $
____________________________________________________________________________*/

// System Headers
#include <windows.h>

char* strstri(char * inBuffer, char * inSearchStr)
{
	char*  currBuffPointer = inBuffer;

    while (*currBuffPointer != 0x00)
	{
        char* compareOne = currBuffPointer;
        char* compareTwo = inSearchStr;

        while (tolower(*compareOne) == tolower(*compareTwo))
		{
            compareOne++;
            compareTwo++;
            if (*compareTwo == 0x00)
			{
				return (char*) currBuffPointer;
            }
        }
        currBuffPointer++;
    }
    return NULL;
}


/*__Editor_settings____

	Local Variables:
	tab-width: 4
	End:
	vi: ts=4 sw=4
	vim: si
_____________________*/
