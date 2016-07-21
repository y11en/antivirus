// ipc_err.h /////////////////////////////////////////////////////////////////

// Interprocess Communication Library Error Defines
// Copyright (C) 1999-2002 Kaspersky Lab. All rights reserved.

// Dmitry A. Gnusaryov [dima@kaspersky.com]

//////////////////////////////////////////////////////////////////////////////

#ifndef IPC_ERR_H
#define IPC_ERR_H

#ifdef __cplusplus
extern "C" {
#endif

//////////////////////////////////////////////////////////////////////////////

#define	IPC_ERR_NO_ERROR			0x00000000	// the operation completed successfully
#define	IPC_ERR_INVALID_ARG			0x00000001	// one or more arguments are invalid
#define	IPC_ERR_OUT_OF_MEMORY		0x00000002	// not enough memory
#define	IPC_ERR_USER_EVENT_SET		0x00000003	// user event signaled
#define	IPC_ERR_CLOSED				0x00000004	// connection closed from other thread
#define	IPC_ERR_BROKEN				0x00000005	// connection broken
#define	IPC_ERR_TIMEOUT				0xfffffffe	// this operation returned because the timeout period expired
#define	IPC_ERR_UNKNOWN				0xffffffff	// unknown error

//////////////////////////////////////////////////////////////////////////////

#ifdef __cplusplus
}
#endif

#endif // IPC_ERR_H

// eof ///////////////////////////////////////////////////////////////////////