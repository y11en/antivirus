// CmdLine.h /////////////////////////////////////

// Command Line Parser (CL)
// [Version 1.13.XXX]
// Copyright (C) 1999-2000 Kaspersky Lab. All rights reserved.

//////////////////////////////////////////////////

#ifndef __COMMAND_LINE_H
#define __COMMAND_LINE_H

#ifdef __cplusplus
extern "C" {
#endif

//////////////////////////////////////////////////

#define CL_ERROR	-1
#define CL_MAX_ARGS	100

//////////////////////////////////////////////////

typedef void *		HCL;

//////////////////////////////////////////////////

			HCL								// NULL
CL_Parse(	TCHAR *	pszCmdLine);

				void
CL_DestroyParse(HCL	hCmdLine);

					TCHAR *					// NULL
CL_GetParseError(	HCL		hCmdLine);		// !!! NOW NOT RELEASED !!!

				DWORD						// CL_ERROR, 0, 1 ... CL_MAX_ARGS
CL_GetArgsCount(HCL	hCmdLine);

					TCHAR *					// NULL
CL_GetArgByNumber(	HCL		hCmdLine,
					DWORD	dwNumber);		// 0 , 1, ... (CL_GetArgsCount - 1)

						TCHAR *				// NULL
CL_GetArgValueByNumber(	HCL		hCmdLine,
						DWORD	dwNumber);	// 0 , 1, ... (CL_GetArgsCount - 1)

			BOOL
CL_IsArg(	HCL				hCmdLine,
			const TCHAR *	pszArg);

				TCHAR *						// NULL
CL_GetArgValue(	HCL				hCmdLine,
				const TCHAR *	pszArg);

//////////////////////////////////////////////////

#ifdef __cplusplus
}
#endif

#endif // __COMMAND_LINE_H

// EOF ///////////////////////////////////////////