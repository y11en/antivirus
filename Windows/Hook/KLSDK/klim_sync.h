#ifndef _KLIM_SYNC_H__
#define _KLIM_SYNC_H__

extern HANDLE g_KlimSyncEvent;

HANDLE
KlimSyn_CreateEvent();

void
KlimSyn_CloseEvent( HANDLE Event );

void
KlimSyn_WaitEvent( HANDLE Event );

void
KlimSyn_SetEvent( HANDLE Event );

typedef
void
(*KlimSynCallback) ( PVOID Context );

typedef struct _KLIN_SYN_THREAD_CTX {
    PVOID           pObject;
    
    KlimSynCallback Callback;
    PVOID           CallbackCtx;
} KLIN_SYN_THREAD_CTX, *PKLIN_SYN_THREAD_CTX;

void
KlinSyn_StartWaitThread( KlimSynCallback Callback, PVOID CallbackCtx );


#ifndef EVENT_ALL_ACCESS
#define EVENT_ALL_ACCESS (STANDARD_RIGHTS_REQUIRED|SYNCHRONIZE|0x3) // winnt
#endif

#ifndef OBJ_KERNEL_HANDLE
#define OBJ_KERNEL_HANDLE       OBJ_CASE_INSENSITIVE
#endif

NTSYSAPI
NTSTATUS
NTAPI
ZwCreateEvent (
               OUT PHANDLE EventHandle,
               IN ACCESS_MASK DesiredAccess,
               IN POBJECT_ATTRIBUTES ObjectAttributes OPTIONAL,
               IN EVENT_TYPE EventType,
               IN BOOLEAN InitialState);
NTSYSAPI
NTSTATUS
NTAPI
ZwOpenEvent(
            OUT PHANDLE  EventHandle,
            IN ACCESS_MASK  DesiredAccess,
            IN POBJECT_ATTRIBUTES  ObjectAttributes
            );

NTSYSAPI
NTSTATUS
NTAPI
ZwWaitForSingleObject(
                      IN HANDLE  Handle,
                      IN BOOLEAN  Alertable,
                      IN PLARGE_INTEGER  Timeout OPTIONAL
                      );

NTSYSAPI
NTSTATUS
NTAPI
ZwSetEvent(
           IN PHANDLE  EventHandle,
           OUT PLONG  PreviousState OPTIONAL
           );


#endif