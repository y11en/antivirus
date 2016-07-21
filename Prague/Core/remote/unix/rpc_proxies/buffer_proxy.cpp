#include <hashutil/hashutil.h>
#include <rpc/requestmanager.h>
#include <rpc/debugtrace.h>
#include <rpc_buffer.h>

using namespace PragueRPC;

UUID BufferUUID = { 0x6DEC2539, 0x5026, 0x495B, 0xA1, 0x1C, { 0xFB,0x0C,0x9F,0x95,0xBB,0x9C } };

tERROR Buffer_SeekRead  ( ConnectionID aConnectionID, 
                          rpcBUFFER _this, 
                          tDWORD* result, 
                          tQWORD offset, 
                          tCHAR* buffer, 
                          tDWORD size )
{
  ENTER;
  if ( !theRequestManager->isConnectionAlive ( aConnectionID ) )
    return errUNEXPECTED;
  static unsigned long aMethodHash = iCountCRC32str ( __FUNCTION__ );
  Request aRequest;
  aRequest.setConnectionID ( aConnectionID );
  aRequest.setUUID ( BufferUUID );
  aRequest.setMethodHash ( aMethodHash );
  aRequest.addParameter ( _this );
  aRequest.addParameter ( result, 1 );
  aRequest.addParameter ( offset );
  aRequest.addParameter ( size );  
  aRequest.addParameter ( buffer, size );
  if ( !theRequestManager->processRequest ( aRequest ) || ( aRequest.getStatus () != RequestHeader::NoErrors ) )
    return errUNEXPECTED;
  tERROR anError;
  if ( !aRequest.getParameter ( result, 1 ) ||
       !aRequest.getParameter ( buffer, size ) || 
       !aRequest.getParameter ( anError ) )
    return errUNEXPECTED;    
  if ( !aRequest.check () )
    return errUNEXPECTED;
  return anError;
}

tERROR Buffer_SeekWrite ( ConnectionID aConnectionID, 
                          rpcBUFFER _this, 
                          /*[mout]*/ tDWORD* result, 
                          tQWORD offset, 
                          /*[in,size_is(size)]*/ tCHAR* buffer, 
                          tDWORD size )
{  
  ENTER;
  if ( !theRequestManager->isConnectionAlive ( aConnectionID ) )
    return errUNEXPECTED;
  static unsigned long aMethodHash = iCountCRC32str ( __FUNCTION__ );
  Request aRequest;
  aRequest.setConnectionID ( aConnectionID );
  aRequest.setUUID ( BufferUUID );
  aRequest.setMethodHash ( aMethodHash );
  aRequest.addParameter ( _this );
  aRequest.addParameter ( result, 1 );
  aRequest.addParameter ( offset );
  aRequest.addParameter ( size );  
  aRequest.addParameter ( buffer, size );
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

tERROR Buffer_GetSize   ( ConnectionID aConnectionID, 
                          rpcBUFFER _this, 
                          /*[mout]*/ tQWORD* result, 
                          IO_SIZE_TYPE type )
{ 
  ENTER;
  if ( !theRequestManager->isConnectionAlive ( aConnectionID ) )
    return errUNEXPECTED;
  static unsigned long aMethodHash = iCountCRC32str ( __FUNCTION__ );
  Request aRequest;
  aRequest.setConnectionID ( aConnectionID );
  aRequest.setUUID ( BufferUUID );
  aRequest.setMethodHash ( aMethodHash );
  aRequest.addParameter ( _this );
  aRequest.addParameter ( result, 1 );
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

tERROR Buffer_SetSize   ( ConnectionID aConnectionID, 
                          rpcBUFFER _this, 
                          tQWORD new_size )
{ 
  ENTER;
  if ( !theRequestManager->isConnectionAlive ( aConnectionID ) )
    return errUNEXPECTED;
  static unsigned long aMethodHash = iCountCRC32str ( __FUNCTION__ );
  Request aRequest;
  aRequest.setConnectionID ( aConnectionID );
  aRequest.setUUID ( BufferUUID );
  aRequest.setMethodHash ( aMethodHash );
  aRequest.addParameter ( _this );
  aRequest.addParameter ( new_size );
  if ( !theRequestManager->processRequest ( aRequest ) || ( aRequest.getStatus () != RequestHeader::NoErrors ) )
    return errUNEXPECTED;
  tERROR anError;
  if ( !aRequest.getParameter ( anError ) )
    return errUNEXPECTED;    
  if ( !aRequest.check () )
    return errUNEXPECTED;
  return anError;
}

tERROR Buffer_Flush     ( ConnectionID aConnectionID, 
                          rpcBUFFER _this )
{ 
  ENTER;
  if ( !theRequestManager->isConnectionAlive ( aConnectionID ) )
    return errUNEXPECTED;
  static unsigned long aMethodHash = iCountCRC32str ( __FUNCTION__ );
  Request aRequest;
  aRequest.setConnectionID ( aConnectionID );
  aRequest.setUUID ( BufferUUID );
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
