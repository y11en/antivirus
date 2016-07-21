#if !defined (_RPC_REMOTE_H_)
#define _RPC_REMOTE_H_
#include <prague.h>
#include <rpc/praguerpc.h>

extern PragueRPC::Interface PRrpc_manager_ServerIfHandle;
extern PragueRPC::Interface PRrpc_library_ServerIfHandle;
using PragueRPC::ConnectionID;

typedef void* PCONNECT_CONTEXT;

typedef struct  tag_tProxyCreateInfo {
  rpcOBJECT object;
  rpcOBJECT stub;
  tDWORD offset;
  tDWORD size;
  tIID iid;
  tIID pid;
} tProxyCreateInfo;


extern "C" {
  //
  //  Client prototypes
  //

  tERROR  RPC_BindServer( /* [in] */ ConnectionID IDL_handle, /* [out] */ PCONNECT_CONTEXT *connect_context, rpcOBJECT client_root );
  tERROR  RPC_BindClose( /* [in] */ ConnectionID IDL_handle, /* [in,out] */ PCONNECT_CONTEXT *connect_context);

  tERROR  RPC_ProxyInvoke(/* [in] */ ConnectionID IDL_handle, /*[mout,size_is(size)]*/ tCHAR* buff, tDWORD size);

  //
  // Server prototypes
  //

  tERROR  PRRPC_BindServer( /* [in] */ ConnectionID IDL_handle, /* [out] */ PCONNECT_CONTEXT *connect_context, rpcOBJECT client_root );
  tERROR  PRRPC_BindClose( /* [in] */ ConnectionID IDL_handle, /* [in,out] */ PCONNECT_CONTEXT *connect_context);

  tERROR  PRRPC_ProxyInvoke( /* [in] */ ConnectionID IDL_handle, /*[mout,size_is(size)]*/ tCHAR* buff, tDWORD size);
}
#endif // _RPC_REMOTE_H_
