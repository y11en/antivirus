/*!
 * (C) 2002 Kaspersky Lab 
 * 
 * \file	rpc_connect.c
 * \author	Pavel Mezhuev
 * \date	09.12.2002 17:34:41
 * \brief	Функции работы с RPC.
 * 
 */
//************************************************************************

#include <Prague/prague.h>
#include <Prague/iface\i_root.h>

#include <rpc.h>
#include "rpc_connect.h"
#include <string.h>
#include <stdio.h>
#include <aclapi.h>
#include <ntsecapi.h>

extern long g_UniqueId;

//************************************************************************

void __RPC_STUB rpc_Svr_BindServer(PRPC_MESSAGE _pRpcMessage);
void __RPC_STUB rpc_Svr_BindClose(PRPC_MESSAGE _pRpcMessage);
void __RPC_STUB rpc_Svr_ProxyInvoke(PRPC_MESSAGE _pRpcMessage);

static RPC_DISPATCH_FUNCTION rpc_manager_table[] =
{
    rpc_Svr_BindServer,
	rpc_Svr_BindClose,
	rpc_Svr_ProxyInvoke,
	0
};

static RPC_DISPATCH_TABLE rpc_manager_DispatchTable = {3, rpc_manager_table};
static RPC_SERVER_INTERFACE rpc__RpcServerInterface;
static RPC_CLIENT_INTERFACE rpc__RpcClientInterface;

//************************************************************************

// NETLIMITER call RpcServerListen with __0__ as first parameter
void   rpc_hook_server_listen()
{
	__try
	{
		HMODULE hRpc = GetModuleHandle("rpcrt4");
		if( !hRpc )
			return;

		PBYTE pProc = (PBYTE)GetProcAddress(hRpc, "RpcMgmtWaitServerListen");
		if( !pProc )
			return;

		PBYTE pAddr = NULL;
		for(int i = 0; i < 40; i++)
			if( pProc[i] == 0x8b && pProc[i+1] == 0x0D )
			{
				pAddr = pProc + i + 2;
				break;
			}

		if( !pAddr )
			return;

		PBYTE pRpcServer = *(PBYTE*)*(PVOID*)pAddr;
		if ( !pRpcServer )
			return;

		if( *(PDWORD)(pRpcServer + 0x24) == 0 && *(PDWORD)(pRpcServer + 0x54) == 0x4d2 ) // WinXP
			*(PDWORD)(pRpcServer + 0x24) = 1;

		else if( *(PDWORD)(pRpcServer + 0x10) == 0 && *(PDWORD)(pRpcServer + 0x40) == 0x4d2 ) // Win2K
			*(PDWORD)(pRpcServer + 0x24) = 1;

		else if( *(PDWORD)(pRpcServer + 0x78) == 0 && *(PDWORD)(pRpcServer + 0x7c) == 0x4d2 ) // Win98
			*(PDWORD)(pRpcServer + 0x78) = 1;
	}
	__except(1)
	{
	}
}

void   rpc_hook_server_port(const char* end_point, HANDLE hFrom)
{
	HMODULE hNtDll = GetModuleHandle("ntdll.dll");

	NTSTATUS (WINAPI * fnNtQueryObject)(HANDLE ObjectHandle, LONG ObjectInformationClass,
		PVOID ObjectInformation, ULONG Length, PULONG ResultLength);

	*(void**)&fnNtQueryObject = GetProcAddress(hNtDll, "NtQueryObject");
	if( !fnNtQueryObject )
		return;

	wchar_t strRpcPort[100];
	swprintf(strRpcPort, L"\\RPC Control\\%S", end_point);

	PR_TRACE((g_root, prtALWAYS_REPORTED_MSG, "rmt\tFind port"));

	char pBuff[100];
	for(int h = (int)hFrom + 4; h < 1000; h+=4)
	{
		PR_TRACE((g_root, prtALWAYS_REPORTED_MSG, "rmt\tTry NtQueryObject(%x)", h));

		ULONG outSize = 0;
		if( fnNtQueryObject((HANDLE)h, 1/*ObjectNameInformation*/, pBuff, sizeof(pBuff), &outSize) )
			continue;

		PWSTR sObjName = ((UNICODE_STRING*)pBuff)->Buffer;

		if( sObjName )
			PR_TRACE((g_root, prtALWAYS_REPORTED_MSG, "rmt\tHandle %x, %S", h, sObjName));

		if( !sObjName || wcscmp(sObjName, strRpcPort) )
			continue;

		SECURITY_DESCRIPTOR pDescr;
		PSID pSID;

		SID_IDENTIFIER_AUTHORITY authWorld = SECURITY_WORLD_SID_AUTHORITY;
		if( AllocateAndInitializeSid(&authWorld, 1, SECURITY_WORLD_RID, 0, 0, 0, 0, 0, 0, 0, &pSID) )
		{
			DWORD dwLengthACL =	GetLengthSid(pSID) + sizeof(ACL) + sizeof(ACCESS_ALLOWED_ACE) - sizeof(DWORD);
			PACL pACL = (PACL)malloc(dwLengthACL);

			if( InitializeAcl(pACL, dwLengthACL, ACL_REVISION) &&
				AddAccessAllowedAce(pACL, ACL_REVISION, GENERIC_ALL, pSID) &&
				InitializeSecurityDescriptor(&pDescr, SECURITY_DESCRIPTOR_REVISION) &&
				SetSecurityDescriptorDacl(&pDescr, TRUE, pACL, FALSE) )
			{
				SetKernelObjectSecurity((HANDLE)h, DACL_SECURITY_INFORMATION, &pDescr);
			}
			FreeSid(pSID);
			free(pACL);
		}
		break;
	}
}

uint rpc_init_guids()
{
static RPC_SYNTAX_IDENTIFIER InterfaceId =    {{0x3C418E9C,0x9C52,0x437f,{0x87,0x9A,0x50,0xD4,0x59,0x87,0x9C,0xAF}},{1,0}};
static RPC_SYNTAX_IDENTIFIER TransferSyntax = {{0x8A885D04,0x1CEB,0x11C9,{0x9F,0xE8,0x08,0x00,0x2B,0x10,0x48,0x60}},{2,0}};

	if( rpc__RpcServerInterface.Length )
		return 0;

	*(long*)InterfaceId.SyntaxGUID.Data4 = g_UniqueId;

	rpc__RpcServerInterface.Length = sizeof(RPC_SERVER_INTERFACE);
	rpc__RpcServerInterface.InterfaceId = InterfaceId;
	rpc__RpcServerInterface.TransferSyntax = TransferSyntax;
	rpc__RpcServerInterface.DispatchTable = &rpc_manager_DispatchTable;

	rpc__RpcClientInterface.Length = sizeof(RPC_CLIENT_INTERFACE);
	rpc__RpcClientInterface.InterfaceId = InterfaceId;
	rpc__RpcClientInterface.TransferSyntax = TransferSyntax;
	return 0;
}

uint rpc_registry_server( const char* protseq, const char* end_point )
{
	rpc_init_guids();
	rpc_hook_server_listen();

	HANDLE hDummy = NULL;
	DWORD dwVersion = GetVersion();
	if( !(dwVersion & 0x80000000) && (DWORD)(LOBYTE(LOWORD(dwVersion))) == 4 )
		// needs to correct security for RPC port for NT40
		hDummy = CreateEvent(NULL, TRUE, TRUE, NULL);

	RPC_STATUS status = 1;
	RpcTryExcept {
		if ( 0 != (status=RpcServerUseProtseqEp( (uchar*)protseq, RPC_C_LISTEN_MAX_CALLS_DEFAULT, (uchar*)end_point, 0 )) )
			PR_TRACE((g_root, prtALWAYS_REPORTED_MSG, "rmt\tRpcServerUseProtseqEp error(%d)", status));
	}
	RpcExcept( EXCEPTION_EXECUTE_HANDLER ) {
	}
	RpcEndExcept;

	if( hDummy )
		CloseHandle(hDummy);

	if( !hDummy || status )
		return !status;

	rpc_hook_server_port(end_point, hDummy);
	return 1;
}


uint rpc_start_server()
{
	RpcTryExcept {
		RpcServerRegisterIfEx( &rpc__RpcServerInterface, 0, 0, RPC_IF_AUTOLISTEN|RPC_IF_ALLOW_CALLBACKS_WITH_NO_AUTH,
			RPC_C_LISTEN_MAX_CALLS_DEFAULT, NULL );
	}
	RpcExcept( EXCEPTION_EXECUTE_HANDLER ) {
		return 0;
	}
	RpcEndExcept;
	return 1;
}

void rpc_stop_server()
{
	RpcTryExcept {
		RpcServerUnregisterIf(&rpc__RpcServerInterface, NULL, TRUE);
	}
	RpcExcept( EXCEPTION_EXECUTE_HANDLER ) {
	}
	RpcEndExcept;
}

//************************************************************************

long rpc_connect_to_server( const char* protseq, const char* server, const char* end_point, handle_t* iface_handle ) {
	
	RPC_STATUS err;
	uchar*     pszStringBinding;

	if( !iface_handle )
		return ERROR_SUCCESS;

	pszStringBinding = 0;
		
	RpcTryExcept {
		if ( 0 != (err=RpcStringBindingCompose(0,(uchar*)protseq,(uchar*)server,(uchar*)end_point,0,(uchar**)&pszStringBinding)) )
			return err;
		
		RpcBindingFromStringBinding( pszStringBinding, iface_handle );
		
		RpcStringFree( &pszStringBinding );  // remote calls done; unbind
		err = ERROR_SUCCESS;
	}
	RpcExcept( EXCEPTION_EXECUTE_HANDLER ) {
		err = RPC_E_UNEXPECTED;
	}
	RpcEndExcept;
	
	return err;
}

long rpc_init_context_handle(handle_t iface_handle, void** context_handle, void* context)
{
	tERROR error = errPROXY_STATE_INVALID;

	RPC_MESSAGE _RpcMessage;
	memset(&_RpcMessage, 0, sizeof(RPC_MESSAGE));
	_RpcMessage.Handle = iface_handle;
	_RpcMessage.RpcInterfaceInformation = &rpc__RpcClientInterface;
	_RpcMessage.ProcNum = 0;
	_RpcMessage.BufferLength = sizeof(void*);

	RPC_STATUS _RpcStatus = I_RpcGetBuffer(&_RpcMessage);
	if( _RpcStatus != RPC_S_OK )
		return error;

	*(void**)_RpcMessage.Buffer = context;
//	PR_TRACE((g_root, prtERROR, "rmt\trpc_init_context_handle:I_RpcSendReceive"));
	_RpcStatus = I_RpcSendReceive(&_RpcMessage);
//	PR_TRACE((g_root, prtERROR, "rmt\trpc_init_context_handle:I_RpcSendReceive end"));

	if( _RpcStatus == RPC_S_OK )
	{
		::NDRCContextUnmarshall(context_handle, iface_handle,
			_RpcMessage.Buffer, _RpcMessage.DataRepresentation);

		I_RpcFreeBuffer(&_RpcMessage);
		error = errOK;
	}
	return error;
}

long rpc_send_receive(handle_t iface_handle, void* buff, ulong size)
{
	tERROR error = errPROXY_STATE_INVALID;

	RPC_MESSAGE _RpcMessage;
	memset(&_RpcMessage, 0, sizeof(RPC_MESSAGE));
	_RpcMessage.Handle = iface_handle;
	_RpcMessage.RpcInterfaceInformation = &rpc__RpcClientInterface;
	_RpcMessage.ProcNum = 2;
	_RpcMessage.BufferLength = sizeof(ulong) + size;

	RPC_STATUS _RpcStatus = I_RpcGetBuffer(&_RpcMessage);
	if( _RpcStatus != RPC_S_OK )
		return error;

	*(ulong*)_RpcMessage.Buffer = size;
	memcpy((ulong*)_RpcMessage.Buffer+1, buff, size);

//	PR_TRACE((g_root, prtERROR, "rmt\trpc_send_receive:I_RpcSendReceive"));
	_RpcStatus = I_RpcSendReceive(&_RpcMessage);
//	PR_TRACE((g_root, prtERROR, "rmt\trpc_send_receive:I_RpcSendReceive end"));

	if( _RpcStatus == RPC_S_OK )
	{
		error = *(tERROR*)_RpcMessage.Buffer;
		memcpy(buff, (ulong*)_RpcMessage.Buffer+1, size);
		I_RpcFreeBuffer(&_RpcMessage);
	}
	else
	{
		PR_TRACE((g_root, prtERROR, "rmt\tI_RpcSendReceive error(0x%lx = %ld)", _RpcStatus, _RpcStatus));

	}
	return error;
}

long  rpc_impersonate(handle_t handle, long begin)
{
	if( begin )
	{
		RPC_STATUS err = RpcImpersonateClient(handle);
		return err == RPC_S_OK || err == RPC_S_CANNOT_SUPPORT;
	}

	RpcRevertToSelf();
	return errOK;
}

// ---
long rpc_disconnect_from_server( handle_t iface_handle, char is_connected, void** context_handle) {
	RPC_STATUS err;

	if( is_connected && context_handle && *context_handle )
	{
		RPC_MESSAGE _RpcMessage;
		memset(&_RpcMessage, 0, sizeof(RPC_MESSAGE));
		_RpcMessage.Handle = iface_handle;
		_RpcMessage.RpcInterfaceInformation = &rpc__RpcClientInterface;
		_RpcMessage.ProcNum = 1;
		_RpcMessage.BufferLength = cbNDRContext;

		RPC_STATUS _RpcStatus = I_RpcGetBuffer(&_RpcMessage);
		if( _RpcStatus == RPC_S_OK )
		{
			::NDRCContextMarshall(*context_handle, _RpcMessage.Buffer);

			_RpcStatus = I_RpcSendReceive(&_RpcMessage);

			if( _RpcStatus == RPC_S_OK )
			{
				::NDRCContextUnmarshall(context_handle, iface_handle,
					_RpcMessage.Buffer, _RpcMessage.DataRepresentation);

				I_RpcFreeBuffer(&_RpcMessage);
			}
		}
	}

	RpcTryExcept {
		err = ERROR_SUCCESS;

		if( context_handle && *context_handle )
			RpcSsDestroyClientContext(context_handle);

		if( iface_handle )
			RpcBindingFree( &iface_handle );
	}
	RpcExcept( EXCEPTION_EXECUTE_HANDLER ) {
		err = RPC_E_UNEXPECTED;
	}
	RpcEndExcept;
	return err;
}

//************************************************************************
// Stub routine

void __RPC_USER PCONNECT_CONTEXT_rundown( void* connect_context )
{
	rpc_rundown_client(connect_context, 1);
}

void __RPC_STUB rpc_Svr_BindServer(PRPC_MESSAGE _pRpcMessage)
{
	NDR_SCONTEXT scontext = NDRSContextUnmarshall((char *)0, _pRpcMessage->DataRepresentation);
	*NDRSContextValue(scontext) = *(void**)_pRpcMessage->Buffer;

//	PR_TRACE((g_root, prtERROR, "rmt\trpc_Svr_BindServer"));

	_pRpcMessage->BufferLength = cbNDRContext;
	RPC_STATUS _RpcStatus = I_RpcGetBuffer(_pRpcMessage);

	if( _RpcStatus )
		RpcRaiseException(_RpcStatus);

	NDRSContextMarshall(scontext, _pRpcMessage->Buffer, PCONNECT_CONTEXT_rundown);

//	PR_TRACE((g_root, prtERROR, "rmt\trpc_Svr_BindServer end"));
}

void __RPC_STUB rpc_Svr_BindClose(PRPC_MESSAGE _pRpcMessage)
{
	NDR_SCONTEXT scontext = NDRSContextUnmarshall(_pRpcMessage->Buffer, _pRpcMessage->DataRepresentation);
	void** connect_contex = NDRSContextValue(scontext);

	rpc_rundown_client(*connect_contex, 0);
	*connect_contex = NULL;

	_pRpcMessage->BufferLength = cbNDRContext;
	RPC_STATUS _RpcStatus = I_RpcGetBuffer(_pRpcMessage);

	if( _RpcStatus )
		RpcRaiseException(_RpcStatus);

	NDRSContextMarshall(scontext, _pRpcMessage->Buffer, PCONNECT_CONTEXT_rundown);
}

void __RPC_STUB rpc_Svr_ProxyInvoke(PRPC_MESSAGE _pRpcMessage)
{
	ulong size = *(ulong*)_pRpcMessage->Buffer;
	void* buff = (ulong*)_pRpcMessage->Buffer + 1;

//	PR_TRACE((g_root, prtERROR, "rmt\trpc_Svr_ProxyInvoke"));

	ulong error = rpc_send_receive_server(_pRpcMessage->Handle, buff, size);

	_pRpcMessage->BufferLength = size + sizeof(tERROR);
	RPC_STATUS _RpcStatus = I_RpcGetBuffer(_pRpcMessage);
	if( _RpcStatus )
		RpcRaiseException(_RpcStatus);

	memcpy((ulong*)_pRpcMessage->Buffer+1, buff, size);
	*(ulong*)_pRpcMessage->Buffer = error;
}

//************************************************************************
