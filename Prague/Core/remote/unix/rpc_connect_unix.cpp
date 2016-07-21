/*!
 *(C)2002 Kaspersky Lab 
 * 
 * \file	rpc_connect.cpp
 * \author	Alexander Sychev
 * \date	31.05.2005
 * \brief	RPC control function
 * 
 */
//************************************************************************

#undef _USE_VTBL
#include <prague.h>
#include <iface/i_root.h>
#include <iface/i_threadpool.h>
#include <rpc.h>
#include <rpc_connect.h>
#include <string.h>
#include <hashutil/hashutil.h>
#include <rpc/requestmanager.h>
#include <rpc/debugtrace.h>
#include <rpc_remote.h>
#include <string>
#include <sys/stat.h>

using namespace PragueRPC;

UUID RemoteUUID = {0x3C418E9C, 0x9C52, 0x437F, 0x87, 0x9A, {0x50, 0xD4, 0x59, 0x87, 0x9C, 0xAF}};


bool RPC_BindServer_Stub(Request& aRequest)
{
	ENTER;
	PCONNECT_CONTEXT* connect_context = 0;
	rpcOBJECT client_root;
	tERROR anError;
	if(!aRequest.getParameter(connect_context, 1) ||
	   !aRequest.getParameter(client_root))
		return false;
	if(!aRequest.check())
		return false;
	anError = PRRPC_BindServer(aRequest.getConnectionID(), connect_context, client_root);
	aRequest.addParameter(connect_context, 1);
	aRequest.addParameter(anError);
	return true;

}

bool RPC_BindClose_Stub(Request& aRequest)
{
	ENTER;
	PCONNECT_CONTEXT* connect_context = 0;
	tERROR anError;
	if(!aRequest.getParameter(connect_context, 1))
		return false;
	if(!aRequest.check())
		return false;
	anError = PRRPC_BindClose( aRequest.getConnectionID(), connect_context);
	aRequest.addParameter(connect_context, 1);
	aRequest.addParameter(anError);
	return true;  
}


bool RPC_ProxyInvoke_Stub(Request& aRequest)
{
	ENTER;
	tCHAR* buff = 0;
	tDWORD size;
	tERROR anError;
	if(!aRequest.getParameter(size) || 
	   !aRequest.getParameter(buff, size))
		return false;
	if(!aRequest.check())
		return false;
	anError = PRRPC_ProxyInvoke(aRequest.getConnectionID(), buff, size);
	aRequest.addParameter(buff, size);
	aRequest.addParameter(anError);
	return true;
}

Method RemoteMethods [] = 
	{{iCountCRC32str("RPC_BindServer"), (void*)&RPC_BindServer_Stub},
	 {iCountCRC32str("RPC_BindClose"), (void*)&RPC_BindClose_Stub},
	 {iCountCRC32str("RPC_ProxyInvoke"), (void*)&RPC_ProxyInvoke_Stub},
	 {0, 0},};

Interface PRrpc_manager_ServerIfHandle = {RemoteUUID, RemoteMethods};



//************************************************************************

uint rpc_registry_server(const char* protseq, const char* end_point)
{
//	std::string anEndPoint = end_point;
//	anEndPoint += "-";
//	if(startRPCService(end_point, MAX_CLIENTS))
//		return 1;
//	return 0;
    mode_t oldMode = umask( 0 );
	mkdir( "/tmp/kav_sockets", S_IRWXU | S_IRWXG | S_IRWXO );
	umask( oldMode );

	std::string anEndPoint = "/tmp/kav_sockets/";
	anEndPoint += end_point;
	if(startRPCService(anEndPoint.c_str(), MAX_CLIENTS))
		return 1;
	return 0;
}

uint rpc_start_server()
{
	if(registerInterface(PRrpc_manager_ServerIfHandle))
		return 1;
	return 0;
}

void rpc_stop_server()
{
	stopRPCService();
}

long  rpc_connect_to_server(const char* protseq, const char* server, const char* end_point, handle_t* iface_handle)
{
//	if(connectToRPCService(end_point, *iface_handle))
//		return ERROR_SUCCESS;
//	return RPC_E_UNEXPECTED;
	std::string anEndPoint = "/tmp/kav_sockets/";
	anEndPoint += end_point;
	if(connectToRPCService(anEndPoint.c_str(), *iface_handle))
		return ERROR_SUCCESS;
	return RPC_E_UNEXPECTED;

}

long  rpc_init_context_handle(handle_t iface_handle, void** context_handle, void* context)
{
	ENTER;
	if(!theRequestManager || !theRequestManager->isConnectionAlive(iface_handle))
		return errUNEXPECTED;
	static unsigned long aMethodHash = iCountCRC32str("RPC_BindServer");
	Request aRequest;
	aRequest.setConnectionID(iface_handle);
	aRequest.setUUID(RemoteUUID);
	aRequest.setMethodHash(aMethodHash);
	aRequest.addParameter(context_handle, 1);
	aRequest.addParameter(context);
	if(!theRequestManager->processRequest(aRequest) || (aRequest.getStatus() != RequestHeader::NoErrors))
		return errUNEXPECTED;
	tERROR anError;
	if(!aRequest.getParameter(context_handle, 1) ||
	   !aRequest.getParameter(anError))
		return errUNEXPECTED;    
	if(!aRequest.check())
		return errUNEXPECTED;
	return anError;
	return RPC_E_UNEXPECTED;
}

long  rpc_send_receive(handle_t iface_handle, void* buff, ulong size)
{
	ENTER;
	if(!theRequestManager || !theRequestManager->isConnectionAlive(iface_handle))
		return errUNEXPECTED;
	static unsigned long aMethodHash = iCountCRC32str("RPC_ProxyInvoke");
	Request aRequest;
	aRequest.setConnectionID(iface_handle);
	aRequest.setMethodHash(aMethodHash);
	aRequest.setUUID(RemoteUUID);
	aRequest.addParameter(size);
	aRequest.addParameter(reinterpret_cast<char*>(buff), size);
	if(!theRequestManager->processRequest(aRequest) || (aRequest.getStatus() != RequestHeader::NoErrors))
		return errUNEXPECTED;
	tERROR anError;
	if(!aRequest.getParameter(reinterpret_cast<char*&>(buff), size)||
	   !aRequest.getParameter(anError))
		return errUNEXPECTED;    
	if(!aRequest.check())
		return errUNEXPECTED;
	return anError;
}

long  rpc_impersonate(handle_t handle, long begin)
{
	return RPC_E_UNEXPECTED;
}

long  rpc_disconnect_from_server(handle_t iface_handle, char is_connected, void** context_handle)
{
	ENTER;
	if(!theRequestManager || !theRequestManager->isConnectionAlive(iface_handle))
		return errUNEXPECTED;
	static unsigned long aMethodHash = iCountCRC32str("RPC_BindClose");
	Request aRequest;
	aRequest.setConnectionID(iface_handle);
	aRequest.setUUID(RemoteUUID);
	aRequest.setMethodHash(aMethodHash);
	aRequest.addParameter(context_handle, 1);
	if(!theRequestManager->processRequest(aRequest) || (aRequest.getStatus() != RequestHeader::NoErrors))
		return errUNEXPECTED;
	tERROR anError;
	if(!aRequest.getParameter(context_handle, 1) ||
	   !aRequest.getParameter(anError))
		return errUNEXPECTED;    
	if(!aRequest.check())
		return errUNEXPECTED;
	disconnectFromRPCService(iface_handle);
	return anError;
}

tERROR PRRPC_BindServer(handle_t iface_handle, void** connect_context, rpcOBJECT client_root)
{
	*connect_context =(void*)client_root;
	return errOK;
}

tERROR PRRPC_BindClose(handle_t iface_handle, void** connect_context)
{
	rpc_rundown_client(*connect_context, 0);
	*connect_context = NULL;
	return errOK;
}

tERROR PRRPC_ProxyInvoke(handle_t iface_handle, tCHAR* buff, tDWORD size)
{
	return rpc_send_receive_server(iface_handle, buff, size);
}
