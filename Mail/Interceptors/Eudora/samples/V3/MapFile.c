/*____________________________________________________________________________
	Copyright (C) 2002 PGP Corporation
	All rights reserved.

	$Id: MapFile.c 9679 2002-08-06 20:11:24Z dallen $
____________________________________________________________________________*/

// System Headers
#include <windows.h>

char* MapFile(const char* Path, DWORD* FileSize)
{
	char* cp = NULL;
	char* pBeginning = NULL;
	HANDLE fh = NULL;
	DWORD Bytes;

	fh = CreateFile(Path, 
					GENERIC_READ, 
					FILE_SHARE_READ|FILE_SHARE_WRITE, 
					NULL, 
					OPEN_EXISTING, 
					FILE_ATTRIBUTE_NORMAL,
					NULL);

	if(fh != INVALID_HANDLE_VALUE)
	{
		*FileSize =  GetFileSize(fh, NULL);

		if(*FileSize > 0)
		{
			pBeginning = (char*) malloc (*FileSize + 1);

			if(!pBeginning)
			{
				return NULL;
			}

			cp = pBeginning;

			ReadFile(fh, cp, *FileSize, &Bytes, NULL);

			*(cp+(*FileSize)) = 0x00;	
		}
		else
		{
			pBeginning = (char*) malloc (1);

			if(pBeginning)
			{
				*pBeginning = 0x00;
			}
		}

		CloseHandle( fh );
	}

	return pBeginning;
}


/*__Editor_settings____

	Local Variables:
	tab-width: 4
	End:
	vi: ts=4 sw=4
	vim: si
_____________________*/
