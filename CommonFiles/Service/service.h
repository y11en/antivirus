// service.h /////////////////////////////////////

// Service
// [Version 1.04.XXX]
// Copyright (C) 1999 Kaspersky Lab. All rights reserved.

//////////////////////////////////////////////////

#ifndef __SERVICE_H
#define __SERVICE_H

//////////////////////////////////////////////////

#define	SERVICE_STARTING_DEFAULT_TIME	3000			// msec
#define SERVICE_STOPPING_DEFAULT_TIME	3000			// msec

typedef	void (* SERVICE_PSTART_HANDLER)(DWORD dwArgc,  LPTSTR * pszArgv);
typedef	void (* SERVICE_PSTOP_HANDLER)();

//////////////////////////////////////////////////

					BOOL
SERVICE_Init	(	LPTSTR					pszServiceInternalName,
					SERVICE_PSTART_HANDLER	pServiceStart,
					DWORD					dwServiceStartingTime,	// 0 - default (msec)
					SERVICE_PSTOP_HANDLER	pServiceStop,
					DWORD					dwServiceStoppingTime);	// 0 - default (msec)

	void
SERVICE_Running	();

//////////////////////////////////////////////////

#endif // __SERVICE_H

// EOF ///////////////////////////////////////////