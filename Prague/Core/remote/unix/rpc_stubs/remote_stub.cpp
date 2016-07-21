#include <hashutil/hashutil.h>
#include <rpc/requestmanager.h>
#include <rpc/debugtrace.h>
#include <rpc_remote.h>

using namespace PragueRPC;

bool RPC_BindServer_Stub ( Request& aRequest )
{
  ENTER;
  PCONNECT_CONTEXT* connect_context = 0;
  rpcOBJECT client_root;
  tERROR anError;
  if ( !aRequest.getParameter ( connect_context, 1 ) ||
       !aRequest.getParameter ( client_root ) )
    return false;
  if ( !aRequest.check () )
    return false;
  anError = PRRPC_BindServer(  aRequest.getConnectionID (), connect_context, client_root );
  aRequest.addParameter ( connect_context, 1 );
  aRequest.addParameter ( anError );
  return true;

}

bool RPC_BindClose_Stub ( Request& aRequest )
{
  ENTER;
  PCONNECT_CONTEXT* connect_context = 0;
  tERROR anError;
  if ( !aRequest.getParameter ( connect_context, 1 ) )
    return false;
  if ( !aRequest.check () )
    return false;
  anError = PRRPC_BindClose(  aRequest.getConnectionID (), connect_context );
  aRequest.addParameter ( connect_context, 1 );
  aRequest.addParameter ( anError );
  return true;  
}


bool RPC_ProxyInvoke_Stub ( Request& aRequest )
{
  ENTER;
  tCHAR* buff = 0;
  tDWORD size;
  tERROR anError;
  if ( !aRequest.getParameter ( size ) || 
       !aRequest.getParameter ( buff, size ) )
    return false;
  if ( !aRequest.check () )
    return false;
  anError = PRRPC_ProxyInvoke ( aRequest.getConnectionID (), buff, size );
  aRequest.addParameter ( anError );
  return true;
}

Method RemoteMethods [] = 
  { { iCountCRC32str ( "RPC_BindServer" ), (void*)&RPC_BindServer_Stub },
    { iCountCRC32str ( "RPC_BindClose" ), (void*)&RPC_BindClose_Stub },
    { iCountCRC32str ( "RPC_ProxyInvoke" ), (void*)&RPC_ProxyInvoke_Stub },
    { 0, 0 }, };

Interface PRrpc_manager_ServerIfHandle = { { 0x3C418E9C, 0x9C52, 0x437F, 0x87, 0x9A, { 0x50, 0xD4, 0x59, 0x87, 0x9C, 0xAF } }, RemoteMethods };

