#include <hashutil/hashutil.h>
#include <rpc/requestmanager.h>
#include <rpc/debugtrace.h>
#include <rpc_root.h>

using namespace PragueRPC;

bool Root_FindPlugins_Stub ( Request& aRequest )
{
  ENTER;
  rpcROOT _this = 0;
  tPID mfinder_pid;
  tBYTE* param_pool = 0;
  tDWORD param_pool_size;
  tDWORD loadflags;
  tERROR anError;
  if ( !aRequest.getParameter ( _this ) ||
       !aRequest.getParameter ( mfinder_pid ) ||
       !aRequest.getParameter ( param_pool_size ) ||
       !aRequest.getParameter ( param_pool, param_pool_size ) ||
       !aRequest.getParameter ( loadflags ) )
    return false;
  if ( !aRequest.check () )
    return false;
  anError = PRRoot_FindPlugins ( aRequest.getConnectionID(), _this, mfinder_pid, param_pool, param_pool_size, loadflags );
  aRequest.addParameter ( anError );
  return true;
}  

bool Root_FreeUnusedPlugins_Stub ( Request& aRequest )
{
  ENTER;
  rpcROOT _this = 0;
  tPID* pid_array = 0;
  tDWORD pid_array_count;
  tFUP_ACTION action;
  tERROR anError;
  if ( !aRequest.getParameter ( _this ) ||
       !aRequest.getParameter ( pid_array ) ||
       !aRequest.getParameter ( pid_array, pid_array_count ) ||
       !aRequest.getParameter ( action ) )
    return false;
  if ( !aRequest.check () )
    return false;
  anError = PRRoot_FreeUnusedPlugins( aRequest.getConnectionID(), _this, pid_array, pid_array_count, action );
  aRequest.addParameter ( anError );
  return true;
}  

bool Root_GetPluginInstance_Stub ( Request& aRequest )
{
  ENTER;
  rpcROOT _this = 0;
  rpcPLUGIN* result = 0;
  tDWORD* cookie = 0;
  tPID pid;
  tERROR anError;
  if ( !aRequest.getParameter ( _this ) ||
       !aRequest.getParameter ( result, 1 ) ||
       !aRequest.getParameter ( cookie, 1 ) ||
       !aRequest.getParameter ( pid ) )
    return false;
  if ( !aRequest.check () )
    return false;
  anError = PRRoot_GetPluginInstance( aRequest.getConnectionID(), _this, result, cookie, pid );
  aRequest.addParameter ( result, 1 );
  aRequest.addParameter ( cookie, 1 );
  aRequest.addParameter ( anError );
  return true;
}  
  
bool Root_RegisterCustomPropId_Stub ( Request& aRequest )
{
  ENTER;
  rpcROOT _this = 0;
  tDWORD* result = 0;
  tSTRING name = 0;
  tDWORD type;
  tERROR anError;
  if ( !aRequest.getParameter ( _this ) ||
       !aRequest.getParameter ( result, 1 ) ||
       !aRequest.getParameter ( name ) ||
       !aRequest.getParameter ( type ) )
    return false;
  if ( !aRequest.check () )
    return false;
  anError = PRRoot_RegisterCustomPropId( aRequest.getConnectionID(), _this, result, name, type );
  aRequest.addParameter ( result, 1 );
  aRequest.addParameter ( anError );
  return true;
}  

bool Root_UnregisterCustomPropId_Stub ( Request& aRequest )
{
  ENTER;
  rpcROOT _this = 0;
  tSTRING name = 0;
  tERROR anError;
  if ( !aRequest.getParameter ( _this ) ||
       !aRequest.getParameter ( name ) )
    return false;
  if ( !aRequest.check () )
    return false;
  anError = PRRoot_UnregisterCustomPropId ( aRequest.getConnectionID(), _this, name );
  aRequest.addParameter ( anError );
  return true;
}  

bool Root_GetCustomPropId_Stub ( Request& aRequest )
{
  ENTER;
  rpcROOT _this = 0;
  tDWORD* result = 0;
  tSTRING name = 0;
  tDWORD type;
  tERROR anError;
  if ( !aRequest.getParameter ( _this ) ||
       !aRequest.getParameter ( result, 1 ) ||
       !aRequest.getParameter ( name ) ||
       !aRequest.getParameter ( type ) )
    return false;
  if ( !aRequest.check () )
    return false;
  anError = PRRoot_GetCustomPropId( aRequest.getConnectionID(), _this, result, name, type );
  aRequest.addParameter ( result, 1 );
  aRequest.addParameter ( anError );
  return true;
}  

bool Root_SyncCustomPropValue_Stub ( Request& aRequest )
{
  ENTER;
  rpcROOT _this;
  tPROPID prop_id;
  tERROR anError;
  if ( !aRequest.getParameter ( _this ) ||
       !aRequest.getParameter ( prop_id ) )
    return false;
  if ( !aRequest.check () )
    return false;
  anError = PRRoot_SyncCustomPropValue( aRequest.getConnectionID (), _this, prop_id );
  aRequest.addParameter ( anError );
  return true;
}  
  
bool Root_TraceLevelSet_Stub ( Request& aRequest )
{
  ENTER;
  rpcROOT _this = 0;
  tDWORD op;
  tIID iid;
  tPID pid;
  tDWORD subtype;
  tDWORD vid;
  tDWORD level;
  tDWORD min_level;
  tERROR anError;
  if ( !aRequest.getParameter ( _this ) ||
       !aRequest.getParameter ( op ) ||
       !aRequest.getParameter ( iid ) ||
       !aRequest.getParameter ( pid ) ||
       !aRequest.getParameter ( subtype ) ||
       !aRequest.getParameter ( vid ) ||
       !aRequest.getParameter ( level ) ||
       !aRequest.getParameter ( min_level ))
    return false;
  if ( !aRequest.check () )
    return false;
  anError = PRRoot_TraceLevelSet ( aRequest.getConnectionID(), _this, op, iid, pid, subtype, vid, level, min_level );
  aRequest.addParameter ( anError );
  return true;
}  

bool Root_GetTracer_Stub ( Request& aRequest )
{
  ENTER;
  rpcROOT _this = 0;
  rpcTRACER* result = 0;
  tERROR anError;
  if ( !aRequest.getParameter ( _this ) ||
       !aRequest.getParameter ( result, 1 ) )
    return false;
  if ( !aRequest.check () )
    return false;
  anError = PRRoot_GetTracer ( aRequest.getConnectionID(), _this, result );
  aRequest.addParameter ( result, 1 );
  aRequest.addParameter ( anError );
  return true;
}            
  

Method RootMethods [] = 
  { { iCountCRC32str ( "Root_FindPlugins" ), (void*)&Root_FindPlugins_Stub },
    { iCountCRC32str ( "Root_FreeUnusedPlugins" ), (void*)&Root_FreeUnusedPlugins_Stub },
    { iCountCRC32str ( "Root_GetPluginInstance" ), (void*)&Root_GetPluginInstance_Stub },
    { iCountCRC32str ( "Root_RegisterCustomPropId" ), (void*)&Root_RegisterCustomPropId_Stub },
    { iCountCRC32str ( "Root_UnregisterCustomPropId" ), (void*)&Root_UnregisterCustomPropId_Stub },
    { iCountCRC32str ( "Root_GetCustomPropId" ), (void*)&Root_GetCustomPropId_Stub },
    { iCountCRC32str ( "Root_SyncCustomPropValue" ), (void*)&Root_SyncCustomPropValue_Stub },
    { iCountCRC32str ( "Root_TraceLevelSet" ), (void*)&Root_TraceLevelSet_Stub },
    { iCountCRC32str ( "Root_GetTracer" ), (void*)&Root_GetTracer_Stub },
    { 0, 0 }, };

Interface PRrpc_root_ServerIfHandle = { { 0x945DEB37, 0x7086, 0x42A5, 0xA0, 0xC2, { 0x62, 0x16, 0x8F, 0x8E, 0x7C, 0x3A } }, RootMethods };
