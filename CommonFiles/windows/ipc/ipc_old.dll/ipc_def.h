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

typedef	unsigned long			HIPCSERVER;		// [ 1, 2, ... , IPC_RC_TIMEOUT, IPC_RC_INVALID_HANDLE ]
typedef	unsigned long			HIPCCONNECTION;	// [ 1, 2, ... , IPC_RC_TIMEOUT, IPC_RC_INVALID_HANDLE ]

#ifndef		IPC_API
#	define	IPC_API				__declspec(dllimport)
#endif

//////////////////////////////////////////////////////////////////////////////

#ifdef __cplusplus
}
#endif

#endif // IPC_DEF_H

// eof ///////////////////////////////////////////////////////////////////////