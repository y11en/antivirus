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

#include <rpc.h>
#include <string.h>
#include "rpc_connect.h"
#include <Hook\FSDFSDrvLib.h>

HANDLE	g_rpc_thread = NULL;

//************************************************************************

uint   rpc_registry_server( const char* protseq, const char* end_point )
{
	RpcTryExcept {
		RPC_STATUS status;
		if ( 0 != (status=RpcServerUseProtseqEp( (uchar*)protseq, RPC_C_LISTEN_MAX_CALLS_DEFAULT, (uchar*)end_point, 0 )) )
			return 0;
	}
	RpcExcept( EXCEPTION_EXECUTE_HANDLER ) {
		return 0;
	}
	RpcEndExcept;
	return 1;
}

// ---
uint   rpc_registry_server_iface( RPC_IF_HANDLE iface_handle )
{
	RpcTryExcept {
		RpcServerRegisterIf( iface_handle, 0, 0 );
	}
	RpcExcept( EXCEPTION_EXECUTE_HANDLER ) {
		return 0;
	}
	RpcEndExcept;
	return 1;
}

// ---
static RPC_STATUS rpc_server_routine( BOOL fDontWait )
{
	RPC_STATUS status;
	
	FSDrv_RegisterInvisibleThread();

	RpcTryExcept {
		if ( status = RpcMgmtWaitServerListen() )
			return status;
	}
	RpcExcept( EXCEPTION_EXECUTE_HANDLER ) {
		return RPC_E_UNEXPECTED;
	}
	RpcEndExcept;


	RpcTryExcept {
		RpcServerUnregisterIf( 0, 0, FALSE );
	}
	RpcExcept( EXCEPTION_EXECUTE_HANDLER ) {
	}
	RpcEndExcept;
	return ERROR_SUCCESS;
}

uint rpc_start_server()
{
	DWORD thread_id;
	if( RpcServerListen(0, RPC_C_LISTEN_MAX_CALLS_DEFAULT, TRUE) != ERROR_SUCCESS )
		return 0;

	g_rpc_thread = CreateThread( 0, 0, (LPTHREAD_START_ROUTINE)rpc_server_routine, NULL, 0, &thread_id );
	return g_rpc_thread != NULL;
}

void rpc_stop_server()
{
	RpcMgmtStopServerListening(NULL);
	if( g_rpc_thread )
		WaitForSingleObject(g_rpc_thread, INFINITE);
}


// ---
RPC_STATUS rpc_connect_to_server( const char* protseq, const char* server, const char* end_point, handle_t* iface_handle ) {
	
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



// ---
RPC_STATUS rpc_disconnect_from_server( handle_t iface_handle ) {
	RPC_STATUS err;

	if( !iface_handle )
		return ERROR_SUCCESS;

	RpcTryExcept {
		err = ERROR_SUCCESS;
		RpcBindingFree( iface_handle );
	}
	RpcExcept( EXCEPTION_EXECUTE_HANDLER ) {
		err = RPC_E_UNEXPECTED;
	}
	RpcEndExcept;
	return err;
}


// ---
void __RPC_FAR * __RPC_USER MIDL_user_allocate( size_t len )       { return malloc(len); }
void             __RPC_USER MIDL_user_free( void __RPC_FAR * ptr ) { free(ptr);} 
