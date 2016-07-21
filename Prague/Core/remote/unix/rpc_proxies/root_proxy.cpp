#include <hashutil/hashutil.h>
#include <rpc/requestmanager.h>
#include <rpc/debugtrace.h>
#include <rpc_root.h>

using namespace PragueRPC;

UUID RootUUID = { 0x945DEB37, 0x7086, 0x42A5, 0xA0, 0xC2, { 0x62, 0x16, 0x8F, 0x8E, 0x7C, 0x3A } };


tERROR Root_FindPlugins              ( /* [in] */ ConnectionID IDL_handle,
                                       rpcROOT _this,
                                       tPID mfinder_pid,
                                       const tPTR param_pool,
                                       tDWORD param_pool_size,
                                       tDWORD loadflags )
{ 
  ENTER;
  if ( !theRequestManager->isConnectionAlive ( IDL_handle ) )
    return errUNEXPECTED;
  static unsigned long aMethodHash = iCountCRC32str ( __FUNCTION__ );
  Request aRequest;
  aRequest.setConnectionID ( IDL_handle );
  aRequest.setUUID ( RootUUID );
  aRequest.setMethodHash ( aMethodHash );
  aRequest.addParameter ( _this );
  aRequest.addParameter ( mfinder_pid );
  aRequest.addParameter ( param_pool_size );
  aRequest.addParameter ( reinterpret_cast<const char*>(param_pool), param_pool_size );
  aRequest.addParameter ( loadflags );
  if ( !theRequestManager->processRequest ( aRequest ) || ( aRequest.getStatus () != RequestHeader::NoErrors ) )
    return errUNEXPECTED;
  tERROR anError;
  if ( !aRequest.getParameter ( anError ) )
    return errUNEXPECTED;    
  if ( !aRequest.check () )
    return errUNEXPECTED;
  return anError;
}  

tERROR Root_FreeUnusedPlugins        ( /* [in] */ ConnectionID IDL_handle,
                                       rpcROOT _this,
                                       /* [in,size_is(pid_array_count)] */ tPID* pid_array,
                                       tDWORD pid_array_count,
                                       tFUP_ACTION action )
{ 
  ENTER;
  if ( !theRequestManager->isConnectionAlive ( IDL_handle ) )
    return errUNEXPECTED;
  static unsigned long aMethodHash = iCountCRC32str ( __FUNCTION__ );
  Request aRequest;
  aRequest.setConnectionID ( IDL_handle );
  aRequest.setUUID ( RootUUID );
  aRequest.setMethodHash ( aMethodHash );
  aRequest.addParameter ( _this );
  aRequest.addParameter ( pid_array_count );
  aRequest.addParameter ( pid_array, pid_array_count );
  aRequest.addParameter ( action );
  if ( !theRequestManager->processRequest ( aRequest ) || ( aRequest.getStatus () != RequestHeader::NoErrors ) )
    return errUNEXPECTED;
  tERROR anError;
  if ( !aRequest.getParameter ( anError ) )
    return errUNEXPECTED;    
  if ( !aRequest.check () )
    return errUNEXPECTED;
  return anError;
}  

tERROR Root_GetPluginInstance        ( /* [in] */ ConnectionID IDL_handle,
                                       rpcROOT _this,
                                       /* [mout] */ rpcPLUGIN* result,
                                       /* [in,mout] */ tDWORD* cookie,
                                       tPID pid )
{ 
  ENTER;
  if ( !theRequestManager->isConnectionAlive ( IDL_handle ) )
    return errUNEXPECTED;
  static unsigned long aMethodHash = iCountCRC32str ( __FUNCTION__ );
  Request aRequest;
  aRequest.setConnectionID ( IDL_handle );
  aRequest.setUUID ( RootUUID );
  aRequest.setMethodHash ( aMethodHash );
  aRequest.addParameter ( _this );
  aRequest.addParameter ( result, 1 );
  aRequest.addParameter ( cookie, sizeof ( *cookie ) );
  aRequest.addParameter ( pid );
  if ( !theRequestManager->processRequest ( aRequest ) || ( aRequest.getStatus () != RequestHeader::NoErrors ) )
    return errUNEXPECTED;
  tERROR anError;
  if ( !aRequest.getParameter ( result, 1 ) ||
       !aRequest.getParameter ( cookie, sizeof ( *cookie ) ) ||
       !aRequest.getParameter ( anError ) )
    return errUNEXPECTED;    
  if ( !aRequest.check () )
    return errUNEXPECTED;
  return anError;
}  
  
tERROR Root_RegisterCustomPropId     ( /* [in] */ ConnectionID IDL_handle,
                                       rpcROOT _this,
                                       /* [mout] */ tDWORD* result,
                                       tSTRING name,
                                       tDWORD type )
{ 
  ENTER;
  if ( !theRequestManager->isConnectionAlive ( IDL_handle ) )
    return errUNEXPECTED;
  static unsigned long aMethodHash = iCountCRC32str ( __FUNCTION__ );
  Request aRequest;
  aRequest.setConnectionID ( IDL_handle );
  aRequest.setUUID ( RootUUID );
  aRequest.setMethodHash ( aMethodHash );
  aRequest.addParameter ( _this );
  aRequest.addParameter ( result, 1 );
  aRequest.addParameter ( name );
  aRequest.addParameter ( type );
  if ( !theRequestManager->processRequest ( aRequest ) || ( aRequest.getStatus () != RequestHeader::NoErrors ) )
    return errUNEXPECTED;
  tERROR anError;
  if ( !aRequest.getParameter ( result, 1 ) ||
       !aRequest.getParameter ( anError ) )
    return errUNEXPECTED;    
  if ( !aRequest.check () )
    return errUNEXPECTED;
  return anError;
}  

tERROR Root_UnregisterCustomPropId   ( /* [in] */ ConnectionID IDL_handle,
                                       rpcROOT _this,
                                       tSTRING name )
{ 
  ENTER;
  if ( !theRequestManager->isConnectionAlive ( IDL_handle ) )
    return errUNEXPECTED;
  static unsigned long aMethodHash = iCountCRC32str ( __FUNCTION__ );
  Request aRequest;
  aRequest.setConnectionID ( IDL_handle );
  aRequest.setUUID ( RootUUID );
  aRequest.setMethodHash ( aMethodHash );
  aRequest.addParameter ( _this );
  aRequest.addParameter ( name );
  if ( !theRequestManager->processRequest ( aRequest ) || ( aRequest.getStatus () != RequestHeader::NoErrors ) )
    return errUNEXPECTED;
  tERROR anError;
  if ( !aRequest.getParameter ( anError ) )
    return errUNEXPECTED;    
  if ( !aRequest.check () )
    return errUNEXPECTED;
  return anError;
}  

tERROR Root_GetCustomPropId          ( /* [in] */ ConnectionID IDL_handle,
                                       rpcROOT _this,
                                       /* [mout] */ tDWORD* result,
                                       tSTRING name,
                                       tDWORD type )
{ 
  ENTER;
  if ( !theRequestManager->isConnectionAlive ( IDL_handle ) )
    return errUNEXPECTED;
  static unsigned long aMethodHash = iCountCRC32str ( __FUNCTION__ );
  Request aRequest;
  aRequest.setConnectionID ( IDL_handle );
  aRequest.setUUID ( RootUUID );
  aRequest.setMethodHash ( aMethodHash );
  aRequest.addParameter ( _this );
  aRequest.addParameter ( result, 1 );
  aRequest.addParameter ( name );
  aRequest.addParameter ( type );
  if ( !theRequestManager->processRequest ( aRequest ) || ( aRequest.getStatus () != RequestHeader::NoErrors ) )
    return errUNEXPECTED;
  tERROR anError;
  if ( !aRequest.getParameter ( result, 1 ) ||
       !aRequest.getParameter ( anError ) )
    return errUNEXPECTED;    
  if ( !aRequest.check () )
    return errUNEXPECTED;
  return anError;
}  

tERROR Root_SyncCustomPropValue      ( /* [in] */ ConnectionID IDL_handle,
                                       rpcROOT _this,
                                       tPROPID prop_id )
{ 
  ENTER;
  if ( !theRequestManager->isConnectionAlive ( IDL_handle ) )
    return errUNEXPECTED;
  static unsigned long aMethodHash = iCountCRC32str ( __FUNCTION__ );
  Request aRequest;
  aRequest.setConnectionID ( IDL_handle );
  aRequest.setUUID ( RootUUID );
  aRequest.setMethodHash ( aMethodHash );
  aRequest.addParameter ( _this );
  aRequest.addParameter ( prop_id );
  if ( !theRequestManager->processRequest ( aRequest ) || ( aRequest.getStatus () != RequestHeader::NoErrors ) )
    return errUNEXPECTED;
  tERROR anError;
  if ( !aRequest.getParameter ( anError ) )
    return errUNEXPECTED;    
  if ( !aRequest.check () )
    return errUNEXPECTED;
  return anError;
}  
  
tERROR Root_TraceLevelSet            ( /* [in] */ ConnectionID IDL_handle,
                                       rpcROOT _this,
                                       tDWORD op,
                                       tIID iid,
                                       tPID pid,
                                       tDWORD subtype,
                                       tDWORD vid,
                                       tDWORD level,
                                       tDWORD min_level )
{ 
  ENTER;
  if ( !theRequestManager->isConnectionAlive ( IDL_handle ) )
    return errUNEXPECTED;
  static unsigned long aMethodHash = iCountCRC32str ( __FUNCTION__ );
  Request aRequest;
  aRequest.setConnectionID ( IDL_handle );
  aRequest.setUUID ( RootUUID );
  aRequest.setMethodHash ( aMethodHash );
  aRequest.addParameter ( _this );
  aRequest.addParameter ( op );
  aRequest.addParameter ( iid );
  aRequest.addParameter ( pid );
  aRequest.addParameter ( subtype );
  aRequest.addParameter ( vid );
  aRequest.addParameter ( level );
  aRequest.addParameter ( min_level );
  if ( !theRequestManager->processRequest ( aRequest ) || ( aRequest.getStatus () != RequestHeader::NoErrors ) )
    return errUNEXPECTED;
  tERROR anError;
  if ( !aRequest.getParameter ( anError ) )
    return errUNEXPECTED;    
  if ( !aRequest.check () )
    return errUNEXPECTED;
  return anError;
}  

tERROR Root_GetTracer                ( /* [in] */ ConnectionID IDL_handle,
                                       rpcROOT _this,
                                       /* [mout] */ rpcTRACER* result )
{ 
  ENTER;
  if ( !theRequestManager->isConnectionAlive ( IDL_handle ) )
    return errUNEXPECTED;
  static unsigned long aMethodHash = iCountCRC32str ( __FUNCTION__ );
  Request aRequest;
  aRequest.setConnectionID ( IDL_handle );
  aRequest.setUUID ( RootUUID );
  aRequest.setMethodHash ( aMethodHash );
  aRequest.addParameter ( _this );
  aRequest.addParameter ( result, 1 );
  if ( !theRequestManager->processRequest ( aRequest ) || ( aRequest.getStatus () != RequestHeader::NoErrors ) )
    return errUNEXPECTED;
  tERROR anError;
  if ( !aRequest.getParameter ( result, 1 ) || 
       !aRequest.getParameter ( anError ) )
    return errUNEXPECTED;    
  if ( !aRequest.check () )
    return errUNEXPECTED;
  return anError;
}            
  
