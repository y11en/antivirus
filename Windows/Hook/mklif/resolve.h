#ifndef __resolve
#define __resolve

#include "pch.h"

#ifndef PECP_LIST
	#define PECP_LIST_notdefined
	#define PECP_LIST					PVOID
#endif //PECP_LIST

#ifndef PPREFETCH_OPEN_ECP_CONTEXT
	#define PPREFETCH_OPEN_ECP_CONTEXT_notdefined
	#define PPREFETCH_OPEN_ECP_CONTEXT	PVOID
#endif // PPREFETCH_OPEN_ECP_CONTEXT

#ifndef GUID_ECP_PREFETCH_OPEN
	#define GUID_ECP_PREFETCH_OPEN_notdefined
	DEFINE_GUID( GUID_ECP_PREFETCH_OPEN, 0xe1777b21, 0x847e, 0x4837, 0xaa, 
	0x45, 0x64, 0x16, 0x1d, 0x28, 0x6, 0x55 );
#endif // GUID_ECP_PREFETCH_OPEN

// dynamic functions------------------------------
typedef 
NTSTATUS
(FLTAPI* _tpfFltGetEcpListFromCallbackData) (
    __in PFLT_FILTER Filter,
    __in PFLT_CALLBACK_DATA CallbackData,
    __out PECP_LIST *EcpList
    );

typedef
NTSTATUS
(FLTAPI* _tpfFltFindExtraCreateParameter) (
    __in PFLT_FILTER Filter,
    __in PECP_LIST EcpList,
    __in LPCGUID EcpType,
    __deref_opt_out PVOID *EcpContext,
    __out_opt ULONG *EcpContextSize
    );

typedef
BOOLEAN
(FLTAPI* _tpfFltIsEcpFromUserMode) (
    __in PFLT_FILTER Filter,
    __in PVOID EcpContext
    );

typedef
NTSTATUS
(FLTAPI* _tpfFltQueryEaFile) (
    IN PFLT_INSTANCE  Instance,
    IN PFILE_OBJECT  FileObject,
    OUT PVOID  ReturnedEaData,
    IN ULONG  Length,
    IN BOOLEAN  ReturnSingleEntry,
    IN PVOID  EaList OPTIONAL,
    IN ULONG  EaListLength,
    IN PULONG  EaIndex OPTIONAL,
    IN BOOLEAN  RestartScan,
    OUT PULONG  LengthReturned OPTIONAL
    ); 

typedef
NTSTATUS
(NTAPI*  _tpCmRegisterCallbackEx) (
    IN PEX_CALLBACK_FUNCTION  Function,
    IN PCUNICODE_STRING  Altitude,
    IN PVOID  Driver,
    IN PVOID  Context,
    OUT PLARGE_INTEGER  Cookie,
    PVOID  Reserved
    );

typedef
VOID
(NTAPI* _tpfCmGetCallbackVersion) (
	__out_opt   PULONG  Major,
	__out_opt   PULONG  Minor
	);

typedef
NTSTATUS
(NTAPI* _tpfCmSetCallbackObjectContext) (
	__inout     PVOID	Object,
	__in		PLARGE_INTEGER  Cookie,
	__in        PVOID	NewContext,
	__out_opt   PVOID	*OldContext
	);

typedef
PDEVICE_OBJECT ( NTAPI *t_fIoGetLowerDeviceObject )(
	__in PDEVICE_OBJECT  DeviceObject
	);

// vista transactions support
typedef
NTSTATUS
(NTAPI* t_fFltGetTransactionContext)(
	IN PFLT_INSTANCE  Instance,
	IN PKTRANSACTION  Transaction,
	OUT PFLT_CONTEXT  *Context
	); 

typedef
NTSTATUS
(NTAPI* t_fFltSetTransactionContext)(
	IN PFLT_INSTANCE  Instance,
	IN PKTRANSACTION  Transaction,
	IN FLT_SET_CONTEXT_OPERATION  Operation,
	IN PFLT_CONTEXT  NewContext,
	OUT PFLT_CONTEXT  *OldContext OPTIONAL
	);

typedef
NTSTATUS
(NTAPI* t_fFltEnlistInTransaction)(
	IN PFLT_INSTANCE  Instance,
	IN PKTRANSACTION  Transaction,
	IN PFLT_CONTEXT  TransactionContext,
	IN NOTIFICATION_MASK  NotificationMask
	); 

typedef
PVOID
(NTAPI* t_fCmGetBoundTransaction)(
	IN PLARGE_INTEGER  Cookie,
	IN PVOID  Object
	);

typedef
NTSTATUS
(NTAPI* t_fFltRollbackEnlistment)(
	IN PFLT_INSTANCE  Instance,
	IN PKTRANSACTION  Transaction,
	IN PFLT_CONTEXT  TransactionContext OPTIONAL
	); 
//-------------------------------------------------

//
_tpfFltGetEcpListFromCallbackData pfFltGetEcpListFromCallbackData;
_tpfFltFindExtraCreateParameter pfFltFindExtraCreateParameter;
_tpfFltIsEcpFromUserMode pfFltIsEcpFromUserMode;
_tpfFltQueryEaFile pfFltQueryEaFile;
_tpCmRegisterCallbackEx pfCmRegisterCallbackEx;
_tpfCmGetCallbackVersion pfCmGetCallbackVersion;
_tpfCmSetCallbackObjectContext pfCmSetCallbackObjectContext;
extern t_fIoGetLowerDeviceObject fIoGetLowerDeviceObject;

//+ transaction operations for Vista -------------------------------------
extern BOOLEAN g_bIsW2K;
extern BOOLEAN g_bIsWXP;
extern BOOLEAN g_bIsW2003;
extern BOOLEAN g_bIsVista;

extern BOOLEAN g_bTransactionOperationsSupported;

t_fFltGetTransactionContext		g_pfFltGetTransactionContext;
t_fFltSetTransactionContext		g_pfFltSetTransactionContext;
t_fFltEnlistInTransaction		g_pfFltEnlistInTransaction;
t_fCmGetBoundTransaction		g_pfCmGetBoundTransaction;
t_fFltRollbackEnlistment		g_pfFltRollbackEnlistment;
//- transaction operations for Vista -------------------------------------


VOID
Resolve_Init (
	);

// settings
VOID
CheckSysPatch (
	);

BOOLEAN
IsUnloadAllowed (
	);

BOOLEAN
IsCalcHashAllowed (
	);

BOOLEAN
IsINetSwiftDisabled (
	);

BOOLEAN
IsISwiftDisabled (
	);

ULONG
GetFileIdMode (
	);

NTSTATUS
GetAltitude (
	__out PUNICODE_STRING pusAltitude
);

BOOLEAN
IsTimingEnabled (
	);

#endif // __resolve
