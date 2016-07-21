#include "StdAfx.h"
#include <rpc_buffer.h>

// ---
tERROR pr_call Local_Buffer_SeekRead( hBUFFER _this, tDWORD* result, tQWORD offset, tPTR buffer, tDWORD size ) {
	PR_PROXY_CALL( Buffer_SeekRead(PR_THIS_PROXY, result, offset, (tCHAR*)buffer, size) );
}
// ---
tERROR pr_call Local_Buffer_SeekWrite( hBUFFER _this, tDWORD* result, tQWORD offset, tPTR buffer, tDWORD size ) { // -- writes buffer to the object;
	PR_PROXY_CALL( Buffer_SeekWrite( PR_THIS_PROXY, result, offset, (tCHAR*)buffer, size ) );
}
// ---
tERROR pr_call Local_Buffer_GetSize( hBUFFER _this, tQWORD* result, IO_SIZE_TYPE type ) {         // -- returns size of the requested type of the object;
	PR_PROXY_CALL( Buffer_GetSize( PR_THIS_PROXY, result, type ) );
}
// ---
tERROR pr_call Local_Buffer_SetSize( hBUFFER _this, tQWORD new_size ) {           // -- resizes the object;
	PR_PROXY_CALL( Buffer_SetSize( PR_THIS_PROXY, new_size ) );
}
// ---
tERROR pr_call Local_Buffer_Flush( hBUFFER _this ) {                            // -- flush internal buffers;
	PR_PROXY_CALL( Buffer_Flush( PR_THIS_PROXY ) );
}


tERROR pr_call Local_Buffer_Lock( hBUFFER _this, tPTR* result ) {                        // -- //rus:фиксирует регион памяти и возвращает указатель на него//eng:lock memory and get access to it;
	return errNOT_SUPPORTED;
}

tERROR pr_call Local_Buffer_AddLock( hBUFFER _this, tPTR* result, tDWORD size_to_add ) {    // -- //rus:добавляет указанный размер к региону и возвращает указатель на добавленную память//eng:add some space to the object and returns pointer to the added space;
	return errNOT_SUPPORTED;
}

tERROR pr_call Local_Buffer_SizeLock( hBUFFER _this, tPTR* result, tDWORD size ) {           // -- //rus:перераспределяет регион с новым размером, фиксирует его и вохвращает указатель на содержимое//eng:set size of the buffer, lock it and returns pointer to the content;
	return errNOT_SUPPORTED;
}

tERROR pr_call Local_Buffer_Unlock( hBUFFER _this ) {                        // -- //rus:разблокирует регион//eng:unlock memory;
	return errNOT_SUPPORTED;
}


// ---
iBufferVtbl buffer_iface = {
	Local_Buffer_SeekRead,
	Local_Buffer_SeekWrite,
	Local_Buffer_GetSize,
	Local_Buffer_SetSize,
	Local_Buffer_Flush,

	Local_Buffer_Lock,
	Local_Buffer_AddLock,
	Local_Buffer_SizeLock,
	Local_Buffer_Unlock
}; // "Buffer" external virtual table prototype

PR_IMPLEMENT_PROXY(IID_BUFFER, buffer_iface)


