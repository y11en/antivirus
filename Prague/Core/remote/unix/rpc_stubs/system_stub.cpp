#include <hashutil/hashutil.h>
#include <rpc/requestmanager.h>
#include <rpc/debugtrace.h>
#include <pr_system/rpc_system.h>

using namespace PragueRPC;

bool  SYS_ObjectCreate_Stub ( Request& aRequest )
{
  ENTER;
  rpcOBJECT _this = 0;
  rpcOBJECT* result = 0;
  tIID iid;
  tPID pid;
  tDWORD subtype;
  tERROR anError;
  if ( !aRequest.getParameter ( _this ) ||
       !aRequest.getParameter ( result, 1 ) ||
       !aRequest.getParameter ( iid ) ||
       !aRequest.getParameter ( pid ) ||
       !aRequest.getParameter ( subtype ) )
    return false;
  if ( !aRequest.check () )
    return false;
  anError = PRSYS_ObjectCreate ( aRequest.getConnectionID(), _this, result, iid, pid, subtype );
  aRequest.addParameter ( result, 1 );
  aRequest.addParameter ( anError );
  return true;
}

bool  SYS_ObjectCreateDone_Stub ( Request& aRequest )
{
  ENTER;
  rpcOBJECT _this = 0;
  tERROR anError;
  if ( !aRequest.getParameter ( _this ) )
    return false;
  if ( !aRequest.check () )
    return false;
  anError = PRSYS_ObjectCreateDone ( aRequest.getConnectionID(), _this );
  aRequest.addParameter ( anError );
  return true;
}

bool  SYS_ObjectCreateQuick_Stub ( Request& aRequest )
{
  ENTER;
  rpcOBJECT _this = 0;
  rpcOBJECT* result = 0;
  tIID iid;
  tPID pid;
  tDWORD subtype;
  tERROR anError;
  if ( !aRequest.getParameter ( _this ) ||
       !aRequest.getParameter ( result, 1 ) ||
       !aRequest.getParameter ( iid ) ||
       !aRequest.getParameter ( pid ) ||
       !aRequest.getParameter ( subtype ) )
    return false;
  if ( !aRequest.check () )
    return false;
  anError = PRSYS_ObjectCreateQuick ( aRequest.getConnectionID(), _this, result, iid, pid, subtype );
  aRequest.addParameter ( result, 1 );
  aRequest.addParameter ( anError );
  return true;
}

bool  SYS_ObjectClose_Stub ( Request& aRequest )
{
  ENTER;
  rpcOBJECT _this = 0;
  tERROR anError;
  if ( !aRequest.getParameter ( _this ) )
    return false;
  if ( !aRequest.check () )
    return false;
  anError = PRSYS_ObjectClose ( aRequest.getConnectionID(), _this );
  aRequest.addParameter ( anError );
  return true;
}

bool  SYS_PropertyGet_Stub ( Request& aRequest )
{
  ENTER;
  rpcOBJECT _this = 0;
  tDWORD* out_size = 0;
  tPROPID prop_id;
  tCHAR* buffer = 0;
  tDWORD size;
  tERROR anError;
  if ( !aRequest.getParameter ( _this ) ||
       !aRequest.getParameter ( out_size, 1 ) ||
       !aRequest.getParameter ( prop_id ) ||
       !aRequest.getParameter ( size ) ||
       !aRequest.getParameter ( buffer, size ) )
    return false;
  if ( !aRequest.check () )
    return false;
  anError = PRSYS_PropertyGet ( aRequest.getConnectionID(), _this, out_size, prop_id, buffer, size );
  aRequest.addParameter ( out_size, 1 );
  aRequest.addParameter ( buffer, size );
  aRequest.addParameter ( anError );
  return true;
}

bool  SYS_PropertyGetStr_Stub ( Request& aRequest )
{
  ENTER;
  rpcOBJECT _this = 0;
  tDWORD* out_size = 0;
  tPROPID prop_id;
  tCHAR* buffer = 0;
  tDWORD size;
  tCODEPAGE receive_cp;
  tERROR anError;
  if ( !aRequest.getParameter ( _this ) ||
       !aRequest.getParameter ( out_size, 1 ) ||
       !aRequest.getParameter ( prop_id ) ||
       !aRequest.getParameter ( size ) ||
       !aRequest.getParameter ( buffer, size )  ||
       !aRequest.getParameter ( receive_cp ) )
    return false;
  if ( !aRequest.check () )
    return false;
  anError = PRSYS_PropertyGetStr ( aRequest.getConnectionID(), _this, out_size, prop_id, buffer, size, receive_cp );
  aRequest.addParameter ( out_size, 1 );
  aRequest.addParameter ( buffer, size );
  aRequest.addParameter ( anError );
  return true;
}

bool  SYS_PropertyGetStrCP_Stub ( Request& aRequest )
{
  ENTER;
  rpcOBJECT _this = 0;
  tCODEPAGE* cp = 0;
  tPROPID prop_id;
  tERROR anError;
  if ( !aRequest.getParameter ( _this ) ||
       !aRequest.getParameter ( cp, 1 ) ||
       !aRequest.getParameter ( prop_id ) )
    return false;
  if ( !aRequest.check () )
    return false;
  anError = PRSYS_PropertyGetStrCP ( aRequest.getConnectionID(), _this, cp, prop_id );
  aRequest.addParameter ( cp, 1 );
  aRequest.addParameter ( anError );
  return true;
}

bool  SYS_PropertySet_Stub ( Request& aRequest )
{
  ENTER;
  rpcOBJECT _this = 0;
  tDWORD* out_size = 0;
  tPROPID prop_id;
  tCHAR* buffer = 0;
  tDWORD size;
  tERROR anError;
  if ( !aRequest.getParameter ( _this ) ||
       !aRequest.getParameter ( out_size, 1 ) ||
       !aRequest.getParameter ( prop_id ) ||
       !aRequest.getParameter ( size ) ||
       !aRequest.getParameter ( buffer, size ) )
    return false;
  if ( !aRequest.check () )
    return false;
  anError = PRSYS_PropertySet ( aRequest.getConnectionID(), _this, out_size, prop_id, buffer, size );
  aRequest.addParameter ( out_size, 1 );
  aRequest.addParameter ( anError );
  return true;
}

bool  SYS_PropertySetStr_Stub ( Request& aRequest )
{
  ENTER;
  rpcOBJECT _this = 0;
  tDWORD* out_size = 0;
  tPROPID prop_id;
  tCHAR* buffer = 0;
  tDWORD size;
  tCODEPAGE src_cp;
  tERROR anError;
  if ( !aRequest.getParameter ( _this ) ||
       !aRequest.getParameter ( out_size, 1 ) ||
       !aRequest.getParameter ( prop_id ) ||
       !aRequest.getParameter ( size ) ||
       !aRequest.getParameter ( buffer, size )  ||
       !aRequest.getParameter ( src_cp ) )
    return false;
  if ( !aRequest.check () )
    return false;
  anError = PRSYS_PropertySetStr ( aRequest.getConnectionID(), _this, out_size, prop_id, buffer, size, src_cp );
  aRequest.addParameter ( out_size, 1 );
  aRequest.addParameter ( anError );
  return true;
}

bool  SYS_PropertySetHeap_Stub ( Request& aRequest )
{
  ENTER;
  rpcOBJECT _this = 0;
  tDWORD heap_pid;
  tERROR anError;
  if ( !aRequest.getParameter ( _this ) ||
       !aRequest.getParameter ( heap_pid ) )
    return false;
  if ( !aRequest.check () )
    return false;
  anError = PRSYS_PropertySetHeap ( aRequest.getConnectionID(), _this, heap_pid );
  aRequest.addParameter ( anError );
  return true;
}

bool  SYS_PropertyIsPresent_Stub ( Request& aRequest )
{
  ENTER;
  rpcOBJECT _this = 0;
  tPROPID prop_id;
  tERROR anError;
  if ( !aRequest.getParameter ( _this ) ||
       !aRequest.getParameter ( prop_id ) )
    return false;
  if ( !aRequest.check () )
    return false;
  anError = PRSYS_PropertyIsPresent ( aRequest.getConnectionID(), _this, prop_id );
  aRequest.addParameter ( anError );
  return true;
}

bool  SYS_PropertyDelete_Stub ( Request& aRequest )
{
  ENTER;
  rpcOBJECT _this = 0;
  tPROPID prop_id;
  tERROR anError;
  if ( !aRequest.getParameter ( _this ) ||
       !aRequest.getParameter ( prop_id ) )
    return false;
  if ( !aRequest.check () )
    return false;
  anError = PRSYS_PropertyDelete ( aRequest.getConnectionID(), _this, prop_id );
  aRequest.addParameter ( anError );
  return true;
}

tDWORD  SYS_PropertySize_Stub ( Request& aRequest )
{
  ENTER;
  rpcOBJECT _this = 0;
  tPROPID prop_id;
  tDWORD aResult;
  if ( !aRequest.getParameter ( _this ) ||
       !aRequest.getParameter ( prop_id ) )
    return false;
  if ( !aRequest.check () )
    return false;
  aResult = PRSYS_PropertySize ( aRequest.getConnectionID(), _this, prop_id );
  aRequest.addParameter ( aResult );
  return true;
}

bool  SYS_PropertyEnumId_Stub ( Request& aRequest )
{
  ENTER;
  rpcOBJECT _this = 0;
  tPROPID* prop_id_result = 0;
  tDWORD* cookie = 0;
  tERROR anError;
  if ( !aRequest.getParameter ( _this ) ||
       !aRequest.getParameter ( prop_id_result, 1 ) ||
       !aRequest.getParameter ( cookie, 1 ) )
    return false;
  if ( !aRequest.check () )
    return false;
  anError = PRSYS_PropertyEnumId ( aRequest.getConnectionID(), _this, prop_id_result, cookie );
  aRequest.addParameter ( prop_id_result, 1 );
  aRequest.addParameter ( cookie, 1 );
  aRequest.addParameter ( anError );
  return true;
}

bool  SYS_RegisterMsgHandlerList_Stub ( Request& aRequest )
{
  ENTER;
  rpcOBJECT _this = 0;
  tDWORD handler_count;
  rpcOBJECT *handlers = 0;
  tMsgHandlerDescr* handler_list = 0;
  tERROR anError;
  if ( !aRequest.getParameter ( _this ) ||
       !aRequest.getParameter ( handler_count ) ||
       !aRequest.getParameter ( handlers, handler_count  ) ||
       !aRequest.getParameter ( handler_list, handler_count  ) )
    return false;
  if ( !aRequest.check () )
    return false;
  anError = PRSYS_RegisterMsgHandlerList ( aRequest.getConnectionID (), _this, handler_count, handlers, handler_list );
  aRequest.addParameter ( anError );
  return true;
}

bool  SYS_UnregisterMsgHandlerList_Stub ( Request& aRequest )
{
  ENTER;
  rpcOBJECT _this = 0;
  tDWORD handler_count;
  rpcOBJECT *handlers = 0;
  tMsgHandlerDescr* handler_list = 0;
  tERROR anError;
  if ( !aRequest.getParameter ( _this ) ||
       !aRequest.getParameter ( handler_count ) ||
       !aRequest.getParameter ( handlers, handler_count  ) ||
       !aRequest.getParameter ( handler_list, handler_count  ) )
    return false;
  if ( !aRequest.check () )
    return false;
  anError =  PRSYS_UnregisterMsgHandlerList ( aRequest.getConnectionID (), _this,  handler_count, handlers, handler_list );
  aRequest.addParameter ( anError );
  return true;
}

bool  SYS_UserDataGet_Stub ( Request& aRequest )
{
  ENTER;
  rpcOBJECT _this = 0;
  tDWORD* data = 0;
  tERROR anError;
  if ( !aRequest.getParameter ( _this ) ||
       !aRequest.getParameter ( data, 1 ) )
    return false;
  if ( !aRequest.check () )
    return false;
  anError = PRSYS_UserDataGet ( aRequest.getConnectionID(), _this, data );
  aRequest.addParameter ( data, 1 );
  aRequest.addParameter ( anError );
  return true;
}

bool  SYS_UserDataSet_Stub ( Request& aRequest )
{
  ENTER;
  rpcOBJECT _this = 0;
  tDWORD data;
  tERROR anError;
  if ( !aRequest.getParameter ( _this ) ||
       !aRequest.getParameter ( data ) )
    return false;
  if ( !aRequest.check () )
    return false;
    anError = PRSYS_UserDataSet ( aRequest.getConnectionID(), _this, data );
  aRequest.addParameter ( anError );
  return true;
}

bool  SYS_Recognize_Stub ( Request& aRequest )
{
  ENTER;
  rpcOBJECT _this = 0;
  tIID by_iid;
  tPID by_pid;
  tDWORD type;
  tERROR anError;
  if ( !aRequest.getParameter ( _this ) ||
       !aRequest.getParameter ( by_iid ) ||
       !aRequest.getParameter ( by_pid ) ||
       !aRequest.getParameter ( type ) )
    return false;
  if ( !aRequest.check () )
    return false;
  anError = PRSYS_Recognize ( aRequest.getConnectionID(), _this, by_iid, by_pid, type );
  aRequest.addParameter ( anError );
  return true;
}

Method SystemMethods [] = 
  { { iCountCRC32str ( "SYS_ObjectCreate" ), (void*)&SYS_ObjectCreate_Stub },
    { iCountCRC32str ( "SYS_ObjectCreateDone" ), (void*)&SYS_ObjectCreateDone_Stub },
    { iCountCRC32str ( "SYS_ObjectCreateQuick" ), (void*)&SYS_ObjectCreateQuick_Stub },
    { iCountCRC32str ( "SYS_ObjectClose" ), (void*)&SYS_ObjectClose_Stub },
    { iCountCRC32str ( "SYS_PropertyGet" ), (void*)&SYS_PropertyGet_Stub },
    { iCountCRC32str ( "SYS_PropertyGetStr" ), (void*)&SYS_PropertyGetStr_Stub },
    { iCountCRC32str ( "SYS_PropertyGetStrCP" ), (void*)&SYS_PropertyGetStrCP_Stub },
    { iCountCRC32str ( "SYS_PropertySet" ), (void*)&SYS_PropertySet_Stub },
    { iCountCRC32str ( "SYS_PropertySetStr" ), (void*)&SYS_PropertySetStr_Stub },
    { iCountCRC32str ( "SYS_PropertySetHeap" ), (void*)&SYS_PropertySetHeap_Stub },
    { iCountCRC32str ( "SYS_PropertyIsPresent" ), (void*)&SYS_PropertyIsPresent_Stub },
    { iCountCRC32str ( "SYS_PropertyDelete" ), (void*)&SYS_PropertyDelete_Stub },
    { iCountCRC32str ( "SYS_PropertySize" ), (void*)&SYS_PropertySize_Stub },
    { iCountCRC32str ( "SYS_PropertyEnumId" ), (void*)&SYS_PropertyEnumId_Stub },
    { iCountCRC32str ( "SYS_RegisterMsgHandlerList" ), (void*)&SYS_RegisterMsgHandlerList_Stub },
    { iCountCRC32str ( "SYS_UnregisterMsgHandlerList" ), (void*)&SYS_UnregisterMsgHandlerList_Stub },
    { iCountCRC32str ( "SYS_UserDataGet" ), (void*)&SYS_UserDataGet_Stub },
    { iCountCRC32str ( "SYS_UserDataSet" ), (void*)&SYS_UserDataSet_Stub },
    { iCountCRC32str ( "SYS_Recognize" ), (void*)&SYS_Recognize_Stub },
    { 0, 0 }, };

Interface PRrpc_system_ServerIfHandle = { { 0x02F29FD6, 0x810E, 0x40f7, 0x8D, 0xD0, { 0x07, 0x1C, 0xB2, 0xB4, 0xAB, 0x91 } }, SystemMethods };
