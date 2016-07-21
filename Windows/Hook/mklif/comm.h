#ifndef __COMM_H
#define __COMM_H

#include "pch.h"

NTSTATUS
Comm_CreatePort (
	__in PUNICODE_STRING pusDrvName
);

VOID
Comm_ClosePort (
	);

NTSTATUS
Comm_PortConnect (
    __in PFLT_PORT ClientPort,
    __in_opt PVOID ServerPortCookie,
    __in_bcount_opt(SizeOfContext) PVOID ConnectionContext,
    __in ULONG SizeOfContext,
    __deref_out_opt PVOID *ConnectionCookie
    );

VOID
Comm_PortDisconnect (
    __in PVOID ConnectionCookie
    );

VOID
Comm_PortContextLock (
	__in PCOMM_CONTEXT pPortContext
	);

VOID
Comm_PortContextUnLock (
	__in PCOMM_CONTEXT pPortContext
	);

NTSTATUS
Comm_SingleEvent (
	__in PQCLIENT pClient,
	__in PCOMM_CONTEXT pPortContext,
	__in PMKLIF_EVENT_TRANSMIT pEventTr,
	__in PEVENT_OBJ pEventObj,
	__in PFILTER_SUB pFilterSub,
	__inout tefVerdict* pVerdict
	);

ULONGLONG 
GetStatCounter(
);

#endif // __COMM_H