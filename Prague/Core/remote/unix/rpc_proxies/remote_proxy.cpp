#include <hashutil/hashutil.h>
#include <rpc/requestmanager.h>
#include <rpc/debugtrace.h>
#include <rpc_remote.h>

using namespace PragueRPC;

UUID RemoteUUID =  { 0x3C418E9C, 0x9C52, 0x437F, 0x87, 0x9A, { 0x50, 0xD4, 0x59, 0x87, 0x9C, 0xAF } };

tERROR  RPC_BindServer( /* [in] */ ConnectionID IDL_handle,
                        /* [out] */ PCONNECT_CONTEXT *connect_context,
                        rpcOBJECT client_root )
{ 
  ENTER;
  if ( !theRequestManager->isConnectionAlive ( IDL_handle ) )
    return errUNEXPECTED;
  static unsigned long aMethodHash = iCountCRC32str ( __FUNCTION__ );
  Request aRequest;
  aRequest.setConnectionID ( IDL_handle );
  aRequest.setUUID ( RemoteUUID );
  aRequest.setMethodHash ( aMethodHash );
  aRequest.addParameter ( connect_context, 1 );
  aRequest.addParameter ( client_root );
  if ( !theRequestManager->processRequest ( aRequest ) || ( aRequest.getStatus () != RequestHeader::NoErrors ) )
    return errUNEXPECTED;
  tERROR anError;
  if ( !aRequest.getParameter ( connect_context, 1 ) ||
       !aRequest.getParameter ( anError ) )
    return errUNEXPECTED;    
  if ( !aRequest.check () )
    return errUNEXPECTED;
  return anError;
}

tERROR  RPC_BindClose( /* [in] */ ConnectionID IDL_handle, 
                       /* [in,out] */ PCONNECT_CONTEXT *connect_context)
{ 
  ENTER;
  if ( !theRequestManager->isConnectionAlive ( IDL_handle ) )
    return errUNEXPECTED;
  static unsigned long aMethodHash = iCountCRC32str ( __FUNCTION__ );
  Request aRequest;
  aRequest.setConnectionID ( IDL_handle );
  aRequest.setUUID ( RemoteUUID );
  aRequest.setMethodHash ( aMethodHash );
  aRequest.addParameter ( connect_context, 1 );
  if ( !theRequestManager->processRequest ( aRequest ) || ( aRequest.getStatus () != RequestHeader::NoErrors ) )
    return errUNEXPECTED;
  tERROR anError;
  if ( !aRequest.getParameter ( connect_context, 1 ) ||
       !aRequest.getParameter ( anError ) )
    return errUNEXPECTED;    
  if ( !aRequest.check () )
    return errUNEXPECTED;
  return anError;
}


tERROR  RPC_ProxyInvoke ( /* [in] */ ConnectionID IDL_handle,
                          /*[    mout,size_is(size)]*/ tCHAR* buff, 
                          tDWORD size)
{ 
  ENTER;
  if ( !theRequestManager->isConnectionAlive ( IDL_handle ) )
    return errUNEXPECTED;
  static unsigned long aMethodHash = iCountCRC32str ( __FUNCTION__ );
  Request aRequest;
  aRequest.setConnectionID ( IDL_handle );
  aRequest.setUUID ( RemoteUUID );
  aRequest.addParameter ( size );
  aRequest.addParameter ( buff, size );
  if ( !theRequestManager->processRequest ( aRequest ) || ( aRequest.getStatus () != RequestHeader::NoErrors ) )
    return errUNEXPECTED;
  tERROR anError;
  if ( !aRequest.getParameter ( buff, size ) ||
       !aRequest.getParameter ( anError ) )
    return errUNEXPECTED;    
  if ( !aRequest.check () )
    return errUNEXPECTED;
  return anError;
}



