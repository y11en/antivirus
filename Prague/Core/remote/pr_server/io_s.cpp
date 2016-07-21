#include "stdafx.h"
#include <rpc_io.h>

#define PR_IO_STUB		((hIO)PR_THIS_STUB)

// ---
extern "C" tERROR PRIO_SeekRead( handle_t IDL_handle, rpcIO _this, tDWORD *result, tQWORD offset, tCHAR *buffer, tDWORD size ) {
	return CALL_IO_SeekRead( PR_IO_STUB, result, offset, buffer, size );
}
// ---
extern "C" tERROR PRIO_SeekWrite( handle_t IDL_handle, rpcIO _this, tDWORD *result, tQWORD offset, tCHAR *buffer, tDWORD size ) {
	return CALL_IO_SeekWrite( PR_IO_STUB, result, offset, buffer, size );
}
// ---
extern "C" tERROR PRIO_GetSize( handle_t IDL_handle, rpcIO _this, tQWORD *result, IO_SIZE_TYPE type ) {
	return CALL_IO_GetSize( PR_IO_STUB, result, type );
}
// ---
extern "C" tERROR PRIO_SetSize( handle_t IDL_handle, rpcIO _this, tQWORD new_size ) {
	return CALL_IO_SetSize( PR_IO_STUB, new_size );
}
// ---
extern "C" tERROR PRIO_Flush( handle_t IDL_handle, rpcIO _this ) {
	return CALL_IO_Flush( PR_IO_STUB );
}

PR_IMPLEMENT_STUB(IID_IO, PRrpc_io_ServerIfHandle)
