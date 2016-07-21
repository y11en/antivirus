#if !defined (_RPC_BUFFER_H_)
#define _RPC_BUFFER_H_
#include <prague.h>
#include <iface/i_buffer.h>
#include <rpc/praguerpc.h>

using PragueRPC::ConnectionID;

extern PragueRPC::Interface PRrpc_buffer_ServerIfHandle;

typedef rpcOBJECT	rpcBUFFER;

extern "C" {
  tERROR Buffer_SeekRead  ( ConnectionID aConnectionID, rpcBUFFER _this, /*[mout]*/ tDWORD* result, tQWORD offset, /*[in,size_is(size)]*/ tCHAR* buffer, tDWORD size );
  tERROR Buffer_SeekWrite ( ConnectionID aConnectionID, rpcBUFFER _this, /*[mout]*/ tDWORD* result, tQWORD offset, /*[in,size_is(size)]*/ tCHAR* buffer, tDWORD size );
  tERROR Buffer_GetSize   ( ConnectionID aConnectionID, rpcBUFFER _this, /*[mout]*/ tQWORD* result, IO_SIZE_TYPE type );
  tERROR Buffer_SetSize   ( ConnectionID aConnectionID, rpcBUFFER _this, tQWORD new_size );
  tERROR Buffer_Flush     ( ConnectionID aConnectionID, rpcBUFFER _this );

  tERROR PRBuffer_SeekRead  ( ConnectionID aConnectionID, rpcBUFFER _this, /*[mout]*/ tDWORD* result, tQWORD offset, /*[in,size_is(size)]*/ tCHAR* buffer, tDWORD size );
  tERROR PRBuffer_SeekWrite ( ConnectionID aConnectionID, rpcBUFFER _this, /*[mout]*/ tDWORD* result, tQWORD offset, /*[in,size_is(size)]*/ tCHAR* buffer, tDWORD size );
  tERROR PRBuffer_GetSize   ( ConnectionID aConnectionID, rpcBUFFER _this, /*[mout]*/ tQWORD* result, IO_SIZE_TYPE type );
  tERROR PRBuffer_SetSize   ( ConnectionID aConnectionID, rpcBUFFER _this, tQWORD new_size );
  tERROR PRBuffer_Flush     ( ConnectionID aConnectionID, rpcBUFFER _this );
}
#endif //_RPC_BUFFER_H_
