// ipc_def.h /////////////////////////////////////////////////////////////////

// Interprocess Communication (IPC) Defines
// Copyright (C) 1999-2002 Kaspersky Lab. All rights reserved.

// Dmitry A. Gnusaryov [dima@kaspersky.com]

//////////////////////////////////////////////////////////////////////////////

#ifndef IPC_DEF_H
#define IPC_DEF_H

#ifdef __cplusplus
extern "C" {
#endif

//////////////////////////////////////////////////////////////////////////////

#include "ipc_err.h"

//////////////////////////////////////////////////////////////////////////////

#define	IPC_RC_INVALID_HANDLE	0x00000000
#define	IPC_RC_ERROR			IPC_ERR_UNKNOWN
#define	IPC_RC_TIMEOUT			IPC_ERR_TIMEOUT

#define	IPC_TIMEOUT_INFINITE	INFINITE

typedef	void * HIPCSERVER;		// [ 1, 2, ... , IPC_RC_TIMEOUT, IPC_RC_INVALID_HANDLE ]
typedef	void * HIPCCONNECTION;	// [ 1, 2, ... , IPC_RC_TIMEOUT, IPC_RC_INVALID_HANDLE ]

__inline BOOL CHECK_IPC_HCONNECTION(HIPCCONNECTION hConnection)
{
	return (hConnection && ((int)hConnection != -2));
}

__inline BOOL CHECK_IPC_RESULT(int rc)
{
	return ((rc != 0) && (rc != -1) && (rc != -2));
}

#ifndef		IPC_API
#	define	IPC_API				__declspec(dllimport)
#endif

//////////////////////////////////////////////////////////////////////////////

#ifdef __cplusplus
}
#endif

#endif // IPC_DEF_H

// eof ///////////////////////////////////////////////////////////////////////