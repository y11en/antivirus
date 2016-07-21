#include <hashutil/hashutil.h>
#include <rpc/requestmanager.h>
#include <rpc/debugtrace.h>
#include <pr_system/rpc_system.h>

using namespace PragueRPC;

UUID SystemUUID = { 0x02F29FD6, 0x810E, 0x40f7, 0x8D, 0xD0, { 0x07, 0x1C, 0xB2, 0xB4, 0xAB, 0x91 } };

tERROR  SYS_ObjectCreate        ( /* [in] */ ConnectionID IDL_handle,
                                  rpcOBJECT _this,
                                  /*[mout]*/ rpcOBJECT* result,
                                  tIID iid,
                                  tPID pid,
                                  tDWORD subtype )
{ 
  ENTER;
  if ( !theRequestManager->isConnectionAlive ( IDL_handle ) )
    return errUNEXPECTED;
  static unsigned long aMethodHash = iCountCRC32str ( __FUNCTION__ );
  Request aRequest;
  aRequest.setConnectionID ( IDL_handle );
  aRequest.setUUID ( SystemUUID );
  aRequest.setMethodHash ( aMethodHash );
  aRequest.addParameter ( _this );
  aRequest.addParameter ( result, 1 );
  aRequest.addParameter ( iid );
  aRequest.addParameter ( pid );
  aRequest.addParameter ( subtype );
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

tERROR  SYS_ObjectCreateDone    ( /* [in] */ ConnectionID IDL_handle,
                                  rpcOBJECT _this )
{ 
  ENTER;
  if ( !theRequestManager->isConnectionAlive ( IDL_handle ) )
    return errUNEXPECTED;
  static unsigned long aMethodHash = iCountCRC32str ( __FUNCTION__ );
  Request aRequest;
  aRequest.setConnectionID ( IDL_handle );
  aRequest.setUUID ( SystemUUID );
  aRequest.setMethodHash ( aMethodHash );
  aRequest.addParameter ( _this );
  if ( !theRequestManager->processRequest ( aRequest ) || ( aRequest.getStatus () != RequestHeader::NoErrors ) )
    return errUNEXPECTED;
  tERROR anError;
  if ( !aRequest.getParameter ( anError ) )
    return errUNEXPECTED;    
  if ( !aRequest.check () )
    return errUNEXPECTED;
  return anError;
}

tERROR  SYS_ObjectCreateQuick   ( /* [in] */ ConnectionID IDL_handle,
                                  rpcOBJECT _this,
                                  /*[mout]*/ rpcOBJECT* result,
                                  tIID iid,
                                  tPID pid,
                                  tDWORD subtype )
{ 
  ENTER;
  if ( !theRequestManager->isConnectionAlive ( IDL_handle ) )
    return errUNEXPECTED;
  static unsigned long aMethodHash = iCountCRC32str ( __FUNCTION__ );
  Request aRequest;
  aRequest.setConnectionID ( IDL_handle );
  aRequest.setUUID ( SystemUUID );
  aRequest.setMethodHash ( aMethodHash );
  aRequest.addParameter ( _this );
  aRequest.addParameter ( result, 1 );
  aRequest.addParameter ( iid );
  aRequest.addParameter ( pid );
  aRequest.addParameter ( subtype );
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

tERROR  SYS_ObjectClose         ( /* [in] */ ConnectionID IDL_handle,
                                  rpcOBJECT _this )
{ 
  ENTER;
  if ( !theRequestManager->isConnectionAlive ( IDL_handle ) )
    return errUNEXPECTED;
  static unsigned long aMethodHash = iCountCRC32str ( __FUNCTION__ );
  Request aRequest;
  aRequest.setConnectionID ( IDL_handle );
  aRequest.setUUID ( SystemUUID );
  aRequest.setMethodHash ( aMethodHash );
  aRequest.addParameter ( _this );
  if ( !theRequestManager->processRequest ( aRequest ) || ( aRequest.getStatus () != RequestHeader::NoErrors ) )
    return errUNEXPECTED;
  tERROR anError;
  if ( !aRequest.getParameter ( anError ) )
    return errUNEXPECTED;    
  if ( !aRequest.check () )
    return errUNEXPECTED;
  return anError;
}

tERROR  SYS_PropertyGet         ( /* [in] */ ConnectionID IDL_handle,
                                  rpcOBJECT _this,
                                  /*[mout]*/ tDWORD* out_size,
                                  tPROPID prop_id,
                                  /* [mout,
                                     size_is(size)] */tCHAR* buffer,
                                  tDWORD size )
{ 
  ENTER;
  if ( !theRequestManager->isConnectionAlive ( IDL_handle ) )
    return errUNEXPECTED;
  static unsigned long aMethodHash = iCountCRC32str ( __FUNCTION__ );
  Request aRequest;
  aRequest.setConnectionID ( IDL_handle );
  aRequest.setUUID ( SystemUUID );
  aRequest.setMethodHash ( aMethodHash );
  aRequest.addParameter ( _this );
  aRequest.addParameter ( out_size, 1 );
  aRequest.addParameter ( prop_id );
  aRequest.addParameter ( size );
  aRequest.addParameter ( buffer, size );
  if ( !theRequestManager->processRequest ( aRequest ) || ( aRequest.getStatus () != RequestHeader::NoErrors ) )
    return errUNEXPECTED;
  tERROR anError;
  if ( !aRequest.getParameter ( out_size, 1 )  || 
       !aRequest.getParameter ( buffer, size )  || 
       !aRequest.getParameter ( anError ) )
    return errUNEXPECTED;    
  if ( !aRequest.check () )
    return errUNEXPECTED;
  return anError;
}

tERROR  SYS_PropertyGetStr      ( /* [in] */ ConnectionID IDL_handle,
                                  rpcOBJECT _this,
                                  /*[mout]*/ tDWORD* out_size,
                                  tPROPID prop_id,
                                  /* [mout,
                                     size_is(size)] */ tCHAR* buffer,
                                  tDWORD size,
                                  tCODEPAGE receive_cp )
{ 
  ENTER;
  if ( !theRequestManager->isConnectionAlive ( IDL_handle ) )
    return errUNEXPECTED;
  static unsigned long aMethodHash = iCountCRC32str ( __FUNCTION__ );
  Request aRequest;
  aRequest.setConnectionID ( IDL_handle );
  aRequest.setUUID ( SystemUUID );
  aRequest.setMethodHash ( aMethodHash );
  aRequest.addParameter ( _this );
  aRequest.addParameter ( out_size, 1 );
  aRequest.addParameter ( prop_id );
  aRequest.addParameter ( size );
  aRequest.addParameter ( buffer, size );
  aRequest.addParameter ( receive_cp );
  if ( !theRequestManager->processRequest ( aRequest ) || ( aRequest.getStatus () != RequestHeader::NoErrors ) )
    return errUNEXPECTED;
  tERROR anError;
  if ( !aRequest.getParameter ( out_size, 1 )  || 
       !aRequest.getParameter ( buffer, size )  || 
       !aRequest.getParameter ( anError ) )
    return errUNEXPECTED;    
  if ( !aRequest.check () )
    return errUNEXPECTED;
  return anError;
}

tERROR  SYS_PropertyGetStrCP    ( /* [in] */ ConnectionID IDL_handle,
                                  rpcOBJECT _this,
                                  /*[mout]*/ tCODEPAGE* cp,
                                  tPROPID prop_id )
{ 
  ENTER;
  if ( !theRequestManager->isConnectionAlive ( IDL_handle ) )
    return errUNEXPECTED;
  static unsigned long aMethodHash = iCountCRC32str ( __FUNCTION__ );
  Request aRequest;
  aRequest.setConnectionID ( IDL_handle );
  aRequest.setUUID ( SystemUUID );
  aRequest.setMethodHash ( aMethodHash );
  aRequest.addParameter ( _this );
  aRequest.addParameter ( cp, 1 );
  aRequest.addParameter ( prop_id );
  if ( !theRequestManager->processRequest ( aRequest ) || ( aRequest.getStatus () != RequestHeader::NoErrors ) )
    return errUNEXPECTED;
  tERROR anError;
  if ( !aRequest.getParameter ( cp, 1 )  || 
       !aRequest.getParameter ( anError ) )
    return errUNEXPECTED;    
  if ( !aRequest.check () )
    return errUNEXPECTED;
  return anError;
}


tERROR  SYS_PropertySet         ( /* [in] */ ConnectionID IDL_handle,
                                  rpcOBJECT _this,
                                  /*[mout]*/ tDWORD* out_size,
                                  tPROPID prop_id,
                                  /* [in,
                                     size_is(size)] */ const tCHAR* buffer,
                                  tDWORD size )
{ 
  ENTER;
  if ( !theRequestManager->isConnectionAlive ( IDL_handle ) )
    return errUNEXPECTED;
  static unsigned long aMethodHash = iCountCRC32str ( __FUNCTION__ );
  Request aRequest;
  aRequest.setConnectionID ( IDL_handle );
  aRequest.setUUID ( SystemUUID );
  aRequest.setMethodHash ( aMethodHash );
  aRequest.addParameter ( _this );
  aRequest.addParameter ( out_size, 1 );
  aRequest.addParameter ( prop_id );
  aRequest.addParameter ( size );
  aRequest.addParameter ( buffer, size );
  if ( !theRequestManager->processRequest ( aRequest ) || ( aRequest.getStatus () != RequestHeader::NoErrors ) )
    return errUNEXPECTED;
  tERROR anError;
  if ( !aRequest.getParameter ( out_size, 1 )  || 
       !aRequest.getParameter ( anError ) )
    return errUNEXPECTED;    
  if ( !aRequest.check () )
    return errUNEXPECTED;
  return anError;
}

tERROR  SYS_PropertySetStr      ( /* [in] */ ConnectionID IDL_handle,
                                  rpcOBJECT _this,
                                  /*[mout]*/ tDWORD* out_size,
                                  tPROPID prop_id,
                                  /* [in,
                                     size_is(size)] */ const tCHAR* buffer,
                                  tDWORD size,
                                  tCODEPAGE src_cp )
{ 
  ENTER;
  if ( !theRequestManager->isConnectionAlive ( IDL_handle ) )
    return errUNEXPECTED;
  static unsigned long aMethodHash = iCountCRC32str ( __FUNCTION__ );
  Request aRequest;
  aRequest.setConnectionID ( IDL_handle );
  aRequest.setUUID ( SystemUUID );
  aRequest.setMethodHash ( aMethodHash );
  aRequest.addParameter ( _this );
  aRequest.addParameter ( out_size, 1 );
  aRequest.addParameter ( prop_id );
  aRequest.addParameter ( size );
  aRequest.addParameter ( buffer, size );
  aRequest.addParameter ( src_cp );
  if ( !theRequestManager->processRequest ( aRequest ) || ( aRequest.getStatus () != RequestHeader::NoErrors ) )
    return errUNEXPECTED;
  tERROR anError;
  if ( !aRequest.getParameter ( out_size, 1 )  || 
       !aRequest.getParameter ( anError ) )
    return errUNEXPECTED;    
  if ( !aRequest.check () )
    return errUNEXPECTED;
  return anError;
}

tERROR  SYS_PropertySetHeap     ( /* [in] */ ConnectionID IDL_handle,
                                  rpcOBJECT _this,
                                  tDWORD heap_pid )
{ 
  ENTER;
  if ( !theRequestManager->isConnectionAlive ( IDL_handle ) )
    return errUNEXPECTED;
  static unsigned long aMethodHash = iCountCRC32str ( __FUNCTION__ );
  Request aRequest;
  aRequest.setConnectionID ( IDL_handle );
  aRequest.setUUID ( SystemUUID );
  aRequest.setMethodHash ( aMethodHash );
  aRequest.addParameter ( _this );
  aRequest.addParameter ( heap_pid );
  if ( !theRequestManager->processRequest ( aRequest ) || ( aRequest.getStatus () != RequestHeader::NoErrors ) )
    return errUNEXPECTED;
  tERROR anError;
  if ( !aRequest.getParameter ( anError ) )
    return errUNEXPECTED;    
  if ( !aRequest.check () )
    return errUNEXPECTED;
  return anError;
}


tERROR  SYS_PropertyIsPresent   ( /* [in] */ ConnectionID IDL_handle,
                                  rpcOBJECT _this,
                                  tPROPID prop_id )
{ 
  ENTER;
  if ( !theRequestManager->isConnectionAlive ( IDL_handle ) )
    return errUNEXPECTED;
  static unsigned long aMethodHash = iCountCRC32str ( __FUNCTION__ );
  Request aRequest;
  aRequest.setConnectionID ( IDL_handle );
  aRequest.setUUID ( SystemUUID );
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

tERROR  SYS_PropertyDelete      ( /* [in] */ ConnectionID IDL_handle,
                                  rpcOBJECT _this,
                                  tPROPID prop_id )
{ 
  ENTER;
  if ( !theRequestManager->isConnectionAlive ( IDL_handle ) )
    return errUNEXPECTED;
  static unsigned long aMethodHash = iCountCRC32str ( __FUNCTION__ );
  Request aRequest;
  aRequest.setConnectionID ( IDL_handle );
  aRequest.setUUID ( SystemUUID );
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

tDWORD  SYS_PropertySize        ( /* [in] */ ConnectionID IDL_handle,
                                  rpcOBJECT _this,
                                  tPROPID prop_id )
{ 
  ENTER;
  if ( !theRequestManager->isConnectionAlive ( IDL_handle ) )
    return errUNEXPECTED;
  static unsigned long aMethodHash = iCountCRC32str ( __FUNCTION__ );
  Request aRequest;
  aRequest.setConnectionID ( IDL_handle );
  aRequest.setUUID ( SystemUUID );
  aRequest.setMethodHash ( aMethodHash );
  aRequest.addParameter ( _this );
  aRequest.addParameter ( prop_id );
  if ( !theRequestManager->processRequest ( aRequest ) || ( aRequest.getStatus () != RequestHeader::NoErrors ) )
    return errUNEXPECTED;
  tDWORD aResult;
  if ( !aRequest.getParameter ( aResult ) )
    return errUNEXPECTED;    
  if ( !aRequest.check () )
    return errUNEXPECTED;
  return aResult;
}

tERROR  SYS_PropertyEnumId      ( /* [in] */ ConnectionID IDL_handle,
                                  rpcOBJECT _this,
                                  /*[mout]*/ tPROPID* prop_id_result,
                                  /*[mout]*/ tDWORD* cookie )
{ 
  ENTER;
  if ( !theRequestManager->isConnectionAlive ( IDL_handle ) )
    return errUNEXPECTED;
  static unsigned long aMethodHash = iCountCRC32str ( __FUNCTION__ );
  Request aRequest;
  aRequest.setConnectionID ( IDL_handle );
  aRequest.setUUID ( SystemUUID );
  aRequest.setMethodHash ( aMethodHash );
  aRequest.addParameter ( _this );
  aRequest.addParameter ( prop_id_result, 1 );
  aRequest.addParameter ( cookie, 1 );
  if ( !theRequestManager->processRequest ( aRequest ) || ( aRequest.getStatus () != RequestHeader::NoErrors ) )
    return errUNEXPECTED;
  tERROR anError;
  if ( !aRequest.getParameter ( prop_id_result, 1 ) ||
       !aRequest.getParameter ( cookie, 1 ) ||
       !aRequest.getParameter ( anError ) )
    return errUNEXPECTED;    
  if ( !aRequest.check () )
    return errUNEXPECTED;
  return anError;
}

tERROR  SYS_RegisterMsgHandlerList  (  /* [in] */  ConnectionID IDL_handle,
                                       rpcOBJECT _this,
                                       tDWORD handler_count,
                                       /* [in,
                                          size_is(handler_count)] */ rpcOBJECT *handlers,
                                       /* [in,
                                          size_is(handler_count)] */ const tMsgHandlerDescr* handler_list )
{ 
  ENTER;
  if ( !theRequestManager->isConnectionAlive ( IDL_handle ) )
    return errUNEXPECTED;
  static unsigned long aMethodHash = iCountCRC32str ( __FUNCTION__ );
  Request aRequest;
  aRequest.setConnectionID ( IDL_handle );
  aRequest.setUUID ( SystemUUID );
  aRequest.setMethodHash ( aMethodHash );
  aRequest.addParameter ( _this );
  aRequest.addParameter ( handler_count );
  aRequest.addParameter ( handlers, handler_count );
  aRequest.addParameter ( handler_list, handler_count );
  if ( !theRequestManager->processRequest ( aRequest ) || ( aRequest.getStatus () != RequestHeader::NoErrors ) )
    return errUNEXPECTED;
  tERROR anError;
  if ( !aRequest.getParameter ( anError ) )
    return errUNEXPECTED;    
  if ( !aRequest.check () )
    return errUNEXPECTED;
  return anError;
}

tERROR  SYS_UnregisterMsgHandlerList(  /* [in] */  ConnectionID IDL_handle,
                                       rpcOBJECT _this,
                                       tDWORD handler_count,
                                       /* [in,
                                          size_is(handler_count)] */ rpcOBJECT *handlers,
                                       /* [in,
                                          size_is(handler_count)] */ const tMsgHandlerDescr* handler_list )
{ 
  ENTER;
  if ( !theRequestManager->isConnectionAlive ( IDL_handle ) )
    return errUNEXPECTED;
  static unsigned long aMethodHash = iCountCRC32str ( __FUNCTION__ );
  Request aRequest;
  aRequest.setConnectionID ( IDL_handle );
  aRequest.setUUID ( SystemUUID );
  aRequest.setMethodHash ( aMethodHash );
  aRequest.addParameter ( _this );
  aRequest.addParameter ( handler_count );
  aRequest.addParameter ( handlers, handler_count );
  aRequest.addParameter ( handler_list, handler_count );
  if ( !theRequestManager->processRequest ( aRequest ) || ( aRequest.getStatus () != RequestHeader::NoErrors ) )
    return errUNEXPECTED;
  tERROR anError;
  if ( !aRequest.getParameter ( anError ) )
    return errUNEXPECTED;    
  if ( !aRequest.check () )
    return errUNEXPECTED;
  return anError;
}

tERROR  SYS_UserDataGet         (  /* [in] */  ConnectionID IDL_handle,
                                   rpcOBJECT _this,
                                   /* [mout] */ tDWORD* data )
{ 
  ENTER;
  if ( !theRequestManager->isConnectionAlive ( IDL_handle ) )
    return errUNEXPECTED;
  static unsigned long aMethodHash = iCountCRC32str ( __FUNCTION__ );
  Request aRequest;
  aRequest.setConnectionID ( IDL_handle );
  aRequest.setUUID ( SystemUUID );
  aRequest.setMethodHash ( aMethodHash );
  aRequest.addParameter ( _this );
  aRequest.addParameter ( data, 1 );
  if ( !theRequestManager->processRequest ( aRequest ) || ( aRequest.getStatus () != RequestHeader::NoErrors ) )
    return errUNEXPECTED;
  tERROR anError;
  if ( !aRequest.getParameter ( data, 1 ) ||
       !aRequest.getParameter ( anError ) )
    return errUNEXPECTED;    
  if ( !aRequest.check () )
    return errUNEXPECTED;
  return anError;
}

tERROR  SYS_UserDataSet         (  /* [in] */  ConnectionID IDL_handle,
                                   rpcOBJECT _this,
                                   tDWORD  data )
{ 
  ENTER;
  if ( !theRequestManager->isConnectionAlive ( IDL_handle ) )
    return errUNEXPECTED;
  static unsigned long aMethodHash = iCountCRC32str ( __FUNCTION__ );
  Request aRequest;
  aRequest.setConnectionID ( IDL_handle );
  aRequest.setUUID ( SystemUUID );
  aRequest.setMethodHash ( aMethodHash );
  aRequest.addParameter ( _this );
  aRequest.addParameter ( data );
  if ( !theRequestManager->processRequest ( aRequest ) || ( aRequest.getStatus () != RequestHeader::NoErrors ) )
    return errUNEXPECTED;
  tERROR anError;
  if ( !aRequest.getParameter ( anError ) )
    return errUNEXPECTED;    
  if ( !aRequest.check () )
    return errUNEXPECTED;
  return anError;
}

tERROR  SYS_Recognize           (  /* [in] */  ConnectionID IDL_handle,
                                   rpcOBJECT _this,
                                   tIID by_iid,
                                   tPID by_pid,
                                   tDWORD type )
{ 
  ENTER;
  if ( !theRequestManager->isConnectionAlive ( IDL_handle ) )
    return errUNEXPECTED;
  static unsigned long aMethodHash = iCountCRC32str ( __FUNCTION__ );
  Request aRequest;
  aRequest.setConnectionID ( IDL_handle );
  aRequest.setUUID ( SystemUUID );
  aRequest.setMethodHash ( aMethodHash );
  aRequest.addParameter ( _this );
  aRequest.addParameter ( by_iid );
  aRequest.addParameter ( by_pid );
  aRequest.addParameter ( type );
  if ( !theRequestManager->processRequest ( aRequest ) || ( aRequest.getStatus () != RequestHeader::NoErrors ) )
    return errUNEXPECTED;
  tERROR anError;
  if ( !aRequest.getParameter ( anError ) )
    return errUNEXPECTED;    
  if ( !aRequest.check () )
    return errUNEXPECTED;
  return anError;
} 


