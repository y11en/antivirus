#ifndef __NET_UTIL_H
#define __NET_UTIL_H

#include <Prague/Prague.h>
#include <Prague/pr_cpp.h>

bool NetUtilInit();
void NetUtilDone();

tERROR NetUtilParseAddress(
							  const char* sAddress,        // (a.b.c.d|hostname)[:port]
							  unsigned short nDefaultPort, 
							  unsigned long* pnAddress,     
							  unsigned short* pnPort);

tERROR NetUtilUDPSendReceive(
							unsigned long address, 
							unsigned short port, // host byte order
							const void* send_data,
							size_t send_len,
							void* recv_buffer,
							size_t recv_buffer_size,
							size_t* received_size,
							unsigned long timeout_ms);


unsigned short NetUtil_htons(unsigned short value);
unsigned short NetUtil_ntohs(unsigned short value);
unsigned long  NetUtil_htonl(unsigned long  value);
unsigned long  NetUtil_ntohl(unsigned long  value);

tERROR NetUtilCopyStrToUtf8(cStringObj& str, void* utf8_buff, size_t utf8_buff_size);
tERROR NetUtilAssignStrFromUtf8(cStringObj& str, void* utf8, size_t utf8_len);



#endif // __NET_UTIL_H
