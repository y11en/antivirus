#include "StdAfx.h"
#include <rpc_buffer.h>

#define PR_BUFFER_STUB		((hBUFFER)PR_THIS_STUB)

// ---
extern "C" tERROR PRBuffer_SeekRead( handle_t IDL_handle, rpcBUFFER _this, tDWORD *result, tQWORD offset, tCHAR *buffer, tDWORD size ) {
	return CALL_Buffer_SeekRead( PR_BUFFER_STUB, result, offset, buffer, size );
}
// ---
extern "C" tERROR PRBuffer_SeekWrite( handle_t IDL_handle, rpcBUFFER _this, tDWORD *result, tQWORD offset, tCHAR *buffer, tDWORD size ) {
	return CALL_Buffer_SeekWrite( PR_BUFFER_STUB, result, offset, buffer, size );
}
// ---
extern "C" tERROR PRBuffer_GetSize( handle_t IDL_handle, rpcBUFFER _this, tQWORD *result, IO_SIZE_TYPE type ) {
	return CALL_Buffer_GetSize( PR_BUFFER_STUB, result, type );
}
// ---
extern "C" tERROR PRBuffer_SetSize( handle_t IDL_handle, rpcBUFFER _this, tQWORD new_size ) {
	return CALL_Buffer_SetSize( PR_BUFFER_STUB, new_size );
}
// ---
extern "C" tERROR PRBuffer_Flush( handle_t IDL_handle, rpcBUFFER _this ) {
	return CALL_Buffer_Flush( PR_BUFFER_STUB );
}

PR_IMPLEMENT_STUB(IID_BUFFER, PRrpc_buffer_ServerIfHandle)
