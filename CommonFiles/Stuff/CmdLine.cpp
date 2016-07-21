// CmdLine.cpp ///////////////////////////////////

// Command Line Parser (CL)
// [Version 1.13.001]
// Copyright (C) 1999-2000 Kaspersky Lab. All rights reserved.

//////////////////////////////////////////////////

#include <windows.h>
#include <tchar.h>
#include "CmdLine.h"

//////////////////////////////////////////////////

typedef struct _HCMDL {// Handle Of Command Line

	DWORD		dwArgsCount;
	TCHAR * *	ppszArgsTable;
	TCHAR * *	ppszArgsValueTable;
	TCHAR *		pszError;			// !!! NOW NOT RELEASED !!!

} HCMDL, * PHCMDL;

//////////////////////////////////////////////////

			HCL
CL_Parse(	TCHAR *	pszCmdLine) {

	if(pszCmdLine == NULL)
		return NULL;

	//////////////////////////////////////////////

	DWORD	dwArgsCount		= 0;
	DWORD	dwArgsBufSize	= 1;
	TCHAR *	p				= pszCmdLine;

	while(*p) {
		if(*p == '/' || *p == '-') {

			p++;
			dwArgsBufSize++;

			while(*p == ' ') {
				p++;
				dwArgsBufSize++;
			}

			if(*p && *p != '/' && *p != '-' && *p != '=') {
				p++;
				dwArgsBufSize++;

				while(*p && *p != '/' && *p != '-' && *p != '=' && *p != ' ') {
					p++;
					dwArgsBufSize++;
				}

				if(*p == '=') {

					p++;
					dwArgsBufSize++;

					while(*p == ' ') {
						p++;
						dwArgsBufSize++;
					}

					if(*p) {
						if(*p == '"') {
							p++;
							dwArgsBufSize++;

							while(*p && *p != '"') {
								p++;
								dwArgsBufSize++;
							}
							/*
							while(*p && *p == ' ') {
								p++;
								dwArgsBufSize++;
							}
							*/
						}
						else {
							while(*p && *p != '/' && *p != '-' && *p != ' ') {
								p++;
								dwArgsBufSize++;
							}
						}
					}
				}

				dwArgsCount++;
			}
		}
		else {
			p++;
			dwArgsBufSize++;
		}
	}

	if(dwArgsCount > CL_MAX_ARGS)
		return NULL;

	//////////////////////////////////////////////

	PHCMDL	h;

	if(dwArgsCount == 0) {
		h = (PHCMDL) new BYTE[sizeof(HCMDL)];
		if(h == NULL)
			return NULL;

		h->dwArgsCount			= 0;
		h->ppszArgsTable		= NULL;
		h->ppszArgsValueTable	= NULL;
		h->pszError				= NULL;

		return h;
	}

	//////////////////////////////////////////////

	dwArgsBufSize *= sizeof(TCHAR);
	DWORD dwPtrTableSize = dwArgsCount * sizeof(TCHAR *);
	h = (PHCMDL) new BYTE[sizeof(HCMDL) + 2 * dwPtrTableSize + dwArgsBufSize];
	if(h == NULL)
		return NULL;

	h->dwArgsCount			= dwArgsCount;
	h->ppszArgsTable		= (TCHAR * *)(h + 1);
	h->ppszArgsValueTable	= (TCHAR * *)((BYTE *)h->ppszArgsTable + dwPtrTableSize);
	h->pszError				= NULL;

	TCHAR *	pszCmdLineBuf = (TCHAR *)((BYTE *)h->ppszArgsValueTable + dwPtrTableSize);
	memcpy(pszCmdLineBuf, pszCmdLine, dwArgsBufSize);

	while(*pszCmdLineBuf && *pszCmdLineBuf != '/' && *pszCmdLineBuf != '-')
		pszCmdLineBuf++;

	//////////////////////////////////////////////

	p = pszCmdLineBuf;
	dwArgsCount = 0;
	while(*p) {
		if(*p == '/' || *p == '-') {
			*p = 0;
			p++;
			while(*p == ' ') {
				*p = 0;
				p++;
			}
			if(*p && *p != '/' && *p != '-' && *p != '=') {
				h->ppszArgsTable[dwArgsCount] = p;
				h->ppszArgsValueTable[dwArgsCount] = NULL;
				p++;
				while(*p && *p != '/' && *p != '-' && *p != '=' && *p != ' ')
					p++;

				TCHAR * OldPos;
				OldPos = p;
				p--;
				while(*p == ' ') {
					*p = 0;
					p--;
				}
				p = OldPos;

				if(*p == '=') {
					*p = 0;
					p++;
					while(*p == ' ') {
						*p = 0;
						p++;
					}
					if(*p) {
						if(*p == '"') {
							p++;
							h->ppszArgsValueTable[dwArgsCount] = p;
							while(*p && *p != '"')
								p++;
							if(*p == '"') {
								*p = 0;
								p++;
							}
							else
								*p = 0;
							while(*p && *p == ' ') {
								*p = 0;
								p++;
							}
						}
						else {
							h->ppszArgsValueTable[dwArgsCount] = p;
							while(*p && *p != '/' && *p != '-' && *p != ' ')
								p++;
							OldPos = p;
							p--;
							while(*p == ' ') {
								*p = 0;
								p--;
							}
							p = OldPos;
						}
					}
				}

				dwArgsCount++;
			}
		}
		else {
			*p = 0;
			p++;
		}
	}

	return h;
}

//////////////////////////////////////////////////

				void
CL_DestroyParse(HCL	hCmdLine) {

	if(hCmdLine != NULL)
		delete[] (BYTE *)hCmdLine;
}

//////////////////////////////////////////////////

					TCHAR *
CL_GetParseError(	HCL	hCmdLine)
{
	PHCMDL h = (PHCMDL)hCmdLine;

	if(h == NULL)
		return NULL;

	return h->pszError;
}

//////////////////////////////////////////////////

				DWORD
CL_GetArgsCount(HCL	hCmdLine) {

	PHCMDL h = (PHCMDL)hCmdLine;

	if(h == NULL)
		return CL_ERROR;

	return h->dwArgsCount;
}

//////////////////////////////////////////////////

					TCHAR *
CL_GetArgByNumber(	HCL		hCmdLine,
					DWORD	dwNumber) {

	PHCMDL h = (PHCMDL)hCmdLine;

	if(h == NULL || dwNumber >= h->dwArgsCount)
		return NULL;

	return h->ppszArgsTable[dwNumber];
}

//////////////////////////////////////////////////

						TCHAR *
CL_GetArgValueByNumber(	HCL		hCmdLine,
						DWORD	dwNumber) {

	PHCMDL h = (PHCMDL)hCmdLine;

	if(h == NULL || dwNumber >= h->dwArgsCount)
		return NULL;

	return h->ppszArgsValueTable[dwNumber];
}

//////////////////////////////////////////////////

			BOOL
CL_IsArg(	HCL				hCmdLine,
			const TCHAR *	pszArg) {

	PHCMDL h = (PHCMDL)hCmdLine;

	if(h == NULL || pszArg == NULL || *pszArg == 0 || h->dwArgsCount == 0)
		return FALSE;

	for(DWORD i = 0; i < h->dwArgsCount; i++)
		if(_tcsicmp(h->ppszArgsTable[i], pszArg) == 0)
			return TRUE;

	return FALSE;
}

//////////////////////////////////////////////////

				TCHAR *
CL_GetArgValue(	HCL				hCmdLine,
				const TCHAR *	pszArg) {

	PHCMDL h = (PHCMDL)hCmdLine;

	if(h == NULL || pszArg == NULL || *pszArg == 0 || h->dwArgsCount == 0)
		return NULL;

	for(DWORD i = 0; i < h->dwArgsCount; i++)
		if(_tcsicmp(h->ppszArgsTable[i], pszArg) == 0)
			return h->ppszArgsValueTable[i];

	return NULL;
}

// EOF ///////////////////////////////////////////