/*!
 * (C) 2002 Kaspersky Lab 
 * 
 * \file	rpc_connect.h
 * \author	Pavel Mezhuev
 * \date	09.12.2002 17:34:41
 * \brief	Декларация функций для работы с RPC.
 * 
 */
//************************************************************************

#ifndef __rpc_server_h
#define __rpc_server_h

#ifdef __cplusplus
	extern "C" {
#endif

typedef unsigned char uchar;
typedef unsigned int  uint;
typedef unsigned long ulong;

uint  rpc_registry_server(const char* protseq, const char* end_point);
uint  rpc_start_server();
void  rpc_stop_server();

long  rpc_connect_to_server(const char* protseq, const char* server, const char* end_point, handle_t* iface_handle);
long  rpc_init_context_handle(handle_t iface_handle, void** context_handle, void* context);
long  rpc_send_receive(handle_t iface_handle, void* buff, ulong size);
long  rpc_impersonate(handle_t handle, long begin);
long  rpc_disconnect_from_server(handle_t iface_handle, char is_connected, void** context_handle);

extern long rpc_rundown_client(void* context_handle, long shutdown);
extern long rpc_send_receive_server(handle_t handle, void* buff, ulong size);

#ifdef __cplusplus
	}
#endif
	
#endif

