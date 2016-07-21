#if !defined (_RPC_SYSTEM_H_)
#define _RPC_SYSTEM_H_
#include <prague.h>
#include <pr_sys.h>
#include <rpc/praguerpc.h>

extern PragueRPC::Interface PRrpc_system_ServerIfHandle;

using PragueRPC::ConnectionID;


//[ uuid (02F29FD6-810E-40f7-8DD0-071CB2B4AB91),
//  version(1.0),
//  pointer_default(unique)
//]

extern "C" {
  //
  //  Client prototypes
  //

  // -----------------------------------------
  // obj management
  tERROR  SYS_ObjectCreate        ( /* [in] */ ConnectionID IDL_handle, rpcOBJECT obj, /*[mout]*/ rpcOBJECT* new_obj, tIID iid, tPID pid, tDWORD subtype );
  tERROR  SYS_ObjectCreateDone    ( /* [in] */ ConnectionID IDL_handle, rpcOBJECT obj );
  tERROR  SYS_ObjectCreateQuick   ( /* [in] */ ConnectionID IDL_handle, rpcOBJECT obj, 
                                    /*[mout]*/ rpcOBJECT* new_obj, tIID iid, tPID pid, tDWORD subtype );
  tERROR  SYS_ObjectClose         ( /* [in] */ ConnectionID IDL_handle,  rpcOBJECT obj );

  // -----------------------------------------
  // property management
  tERROR  SYS_PropertyGet         ( /* [in] */ ConnectionID IDL_handle,  rpcOBJECT obj, /*[mout]*/ tDWORD* out_size, tPROPID prop_id, /* [mout,size_is(size)] */tCHAR* buffer, tDWORD size );
  tERROR  SYS_PropertyGetStr      ( /* [in] */ ConnectionID IDL_handle,  rpcOBJECT obj, /*[mout]*/ tDWORD* out_size, tPROPID prop_id, /* [mout, size_is(size)] */ tCHAR* buffer, tDWORD size, tCODEPAGE receive_cp );
  tERROR  SYS_PropertyGetStrCP    ( /* [in] */ ConnectionID IDL_handle,  rpcOBJECT obj, /*[mout]*/ tCODEPAGE* cp, tPROPID prop_id );

  tERROR  SYS_PropertySet         ( /* [in] */ ConnectionID IDL_handle,  rpcOBJECT obj, /*[mout]*/ tDWORD* out_size, tPROPID prop_id, /* [in, size_is(size)] */ const tCHAR* buffer, tDWORD size );
  tERROR  SYS_PropertySetStr      ( /* [in] */ ConnectionID IDL_handle,  rpcOBJECT obj, /*[mout]*/ tDWORD* out_size, tPROPID prop_id, /* [in, size_is(size)] */ const tCHAR* pStrz, tDWORD size, tCODEPAGE src_cp );
  tERROR  SYS_PropertySetHeap     ( /* [in] */ ConnectionID IDL_handle,  rpcOBJECT obj, tDWORD heap_pid );

  tERROR  SYS_PropertyIsPresent   ( /* [in] */ ConnectionID IDL_handle,  rpcOBJECT obj, tPROPID prop_id );
  tERROR  SYS_PropertyDelete      ( /* [in] */ ConnectionID IDL_handle,  rpcOBJECT obj, tPROPID prop_id );
  tDWORD  SYS_PropertySize        ( /* [in] */ ConnectionID IDL_handle,  rpcOBJECT obj, tPROPID prop_id );
  tERROR  SYS_PropertyEnumId      ( /* [in] */ ConnectionID IDL_handle,  rpcOBJECT obj, /*[mout]*/ tPROPID* prop_id_result, /*[mout]*/ tDWORD* cookie );

  // -----------------------------------------
  // message management

  tERROR  SYS_RegisterMsgHandlerList  (  /* [in] */  ConnectionID IDL_handle,  rpcOBJECT obj, tDWORD handler_count, /* [in,size_is(handler_count)] */ rpcOBJECT *handlers, /* [in,size_is(handler_count)] */ const tMsgHandlerDescr* handler_list );
  tERROR  SYS_UnregisterMsgHandlerList(  /* [in] */  ConnectionID IDL_handle,  rpcOBJECT obj, tDWORD handler_count, /* [in,size_is(handler_count)] */ rpcOBJECT *handlers, /* [in,size_is(handler_count)] */ const tMsgHandlerDescr* handler_list );

  // -----------------------------------------
  // user data management
  tERROR  SYS_UserDataGet         (  /* [in] */  ConnectionID IDL_handle,  rpcOBJECT obj, /* [mout] */ tDWORD* data );
  tERROR  SYS_UserDataSet         (  /* [in] */  ConnectionID IDL_handle,  rpcOBJECT obj, tDWORD  data );

  // -----------------------------------------
  // recognize methods
  tERROR  SYS_Recognize           (  /* [in] */  ConnectionID IDL_handle,  rpcOBJECT obj, tIID by_iid, tPID by_pid, tDWORD type ); 


  //
  //  Server prototypes
  //
  // -----------------------------------------
  // obj management
  tERROR  PRSYS_ObjectCreate        (  /* [in] */  ConnectionID IDL_handle, rpcOBJECT obj, /* [mout] */ rpcOBJECT* new_obj, tIID iid, tPID pid, tDWORD subtype );
  tERROR  PRSYS_ObjectCreateDone    (  /* [in] */  ConnectionID IDL_handle,  rpcOBJECT obj );
  tERROR  PRSYS_ObjectCreateQuick   (  /* [in] */  ConnectionID IDL_handle,  rpcOBJECT obj, /* [mout] */ rpcOBJECT* new_obj, tIID iid, tPID pid, tDWORD subtype );
  tERROR  PRSYS_ObjectClose         (  /* [in] */  ConnectionID IDL_handle,  rpcOBJECT obj );

  // -----------------------------------------
  // property management
  tERROR  PRSYS_PropertyGet         (  /* [in] */  ConnectionID IDL_handle,  rpcOBJECT obj, /* [mout] */ tDWORD* out_size, tPROPID prop_id, /* [mout,size_is(size)] */tCHAR* buffer, tDWORD size );
  tERROR  PRSYS_PropertyGetStr      (  /* [in] */  ConnectionID IDL_handle,  rpcOBJECT obj, /* [mout] */ tDWORD* out_size, tPROPID prop_id, /* [mout, size_is(size)] */ tCHAR* buffer, tDWORD size, tCODEPAGE receive_cp );
  tERROR  PRSYS_PropertyGetStrCP    (  /* [in] */  ConnectionID IDL_handle,  rpcOBJECT obj, /* [mout] */ tCODEPAGE* cp, tPROPID prop_id );

  tERROR  PRSYS_PropertySet         (  /* [in] */  ConnectionID IDL_handle,  rpcOBJECT obj, /* [mout] */ tDWORD* out_size, tPROPID prop_id, /* [in, size_is(size)] */ const tCHAR* buffer, tDWORD size );
  tERROR  PRSYS_PropertySetStr      (  /* [in] */  ConnectionID IDL_handle,  rpcOBJECT obj, /* [mout] */ tDWORD* out_size, tPROPID prop_id, /* [in, size_is(size)] */ const tCHAR* pStrz, tDWORD size, tCODEPAGE src_cp );
  tERROR  PRSYS_PropertySetHeap     (  /* [in] */  ConnectionID IDL_handle,  rpcOBJECT obj, tDWORD heap_pid );

  tERROR  PRSYS_PropertyIsPresent   (  /* [in] */  ConnectionID IDL_handle,  rpcOBJECT obj, tPROPID prop_id );
  tERROR  PRSYS_PropertyDelete      (  /* [in] */  ConnectionID IDL_handle,  rpcOBJECT obj, tPROPID prop_id );
  tDWORD  PRSYS_PropertySize        (  /* [in] */  ConnectionID IDL_handle,  rpcOBJECT obj, tPROPID prop_id );
  tERROR  PRSYS_PropertyEnumId      (  /* [in] */  ConnectionID IDL_handle,  rpcOBJECT obj, /* [mout] */ tPROPID* prop_id_result, /* [mout] */ tDWORD* cookie );

  // -----------------------------------------
  // message management

  tERROR  PRSYS_RegisterMsgHandlerList  (  /* [in] */  ConnectionID IDL_handle,  rpcOBJECT obj, tDWORD handler_count, /* [in,size_is(handler_count)] */ rpcOBJECT *handlers, /* [in,size_is(handler_count)] */ const tMsgHandlerDescr* handler_list );
  tERROR  PRSYS_UnregisterMsgHandlerList(  /* [in] */  ConnectionID IDL_handle,  rpcOBJECT obj, tDWORD handler_count, /* [in,size_is(handler_count)] */ rpcOBJECT *handlers, /* [in,size_is(handler_count)] */ const tMsgHandlerDescr* handler_list );

  // -----------------------------------------
  // user data management
  tERROR  PRSYS_UserDataGet         (  /* [in] */  ConnectionID IDL_handle,  rpcOBJECT obj, /* [mout] */ tDWORD* data );
  tERROR  PRSYS_UserDataSet         (  /* [in] */  ConnectionID IDL_handle,  rpcOBJECT obj, tDWORD  data );

  // -----------------------------------------
  // recognize methods
  tERROR  PRSYS_Recognize           (  /* [in] */  ConnectionID IDL_handle,  rpcOBJECT obj, tIID by_iid, tPID by_pid, tDWORD type );   

}
#endif // __RPC_PRSYSTEM_H_
