#include "netutil.h"

#include <Prague/pr_cpp.h>

#ifdef _WIN32
#include <winsock.h>
#define get_sock_err() WSAGetLastError()
#else
#error Place platform specific analogue for WSAGetLastError() here
#endif

LONG g_nWinSockInited = 0;


bool InitWinsock()
{
#ifdef _WIN32
	if (InterlockedCompareExchange(&g_nWinSockInited, 0, 0))
		return true;
	WSADATA wsadata;
	int nerr = WSAStartup(MAKEWORD(2, 2), &wsadata);
	if (nerr != 0)
	{
		PR_TRACE((g_root, prtERROR, TR "WSAStartup failed %d", nerr));
		return false;
	}
	InterlockedIncrement(&g_nWinSockInited);
	return true;
#else
#error Place platform specific analogue for WSAGetLastError() here
#endif
}

bool NetUtilInit()
{
	return true;
}

void NetUtilDone()
{
#ifdef _WIN32
	if (0 == InterlockedCompareExchange(&g_nWinSockInited, 0, 0))
		return;
	while (InterlockedDecrement(&g_nWinSockInited) > 0)
		WSACleanup();
	WSACleanup();
#else
#error Place platform specific analogue for WSAGetLastError() here
#endif
	return;
}

tERROR NetUtilParseAddress(
							  const char* sAddress,        // (a.b.c.d|hostname)[:port]
							  unsigned short nDefaultPort, 
							  unsigned long* pnAddress,     
							  unsigned short* pnPort        
							  )
{
	if (!sAddress)
		return errPARAMETER_INVALID;
	if (!pnAddress || !pnPort)
		return errPARAMETER_INVALID;
	if (!InitWinsock())
		return errOBJECT_CANNOT_BE_INITIALIZED;
	cStrBuff sAddr = sAddress;
	char* psPort = (char*)strchr(sAddr, ':');
	if (psPort)
	{
		unsigned long nTempPort = strtoul(psPort+1, NULL, 10);
		if (nTempPort == 0 || nTempPort > cMAX_WORD)
		{
			PR_TRACE((g_root, prtERROR, TR "ParseAddress(%s) invalid port %d", sAddress, nTempPort));
			return errOBJECT_INVALID;
		}
		*pnPort = htons((unsigned short)nTempPort);
		*psPort = 0; // cut port
	}
	else
	{
		*pnPort = htons(nDefaultPort);
	}

	// определение IP-адреса узла
	*pnAddress = inet_addr(sAddr);
	if (INADDR_NONE == *pnAddress)
	{
		HOSTENT *hst = gethostbyname(sAddr);
		if (!hst)
		{
			PR_TRACE((g_root, prtERROR, TR "ParseAddress(%s) cannot resolve", sAddress));
			return errOBJECT_INVALID;
		}
		*pnAddress = ((unsigned long **)hst->h_addr_list)[0][0];
	}
	return errOK;
}

tERROR _iNetUtilUDPSendReceive(
						    SOCKET udp_sock,
							unsigned long address, 
							unsigned short port, // host byte order
							const void* send_data,
							size_t send_len,
							void* recv_buffer,
							size_t recv_buffer_size,
							size_t* received_size,
							unsigned long timeout_ms)
{
	tERROR error = errOK;
	if (udp_sock == INVALID_SOCKET)
		return errPARAMETER_INVALID;
	if (!address || !port)
		return errPARAMETER_INVALID;
	if (send_len && !send_data)
		return errPARAMETER_INVALID;
	if (recv_buffer_size && !recv_buffer)
		return errPARAMETER_INVALID;
	if (recv_buffer && !received_size)
		return errPARAMETER_INVALID;

	if (send_len)
	{
		// ѕередача сообщени€ на сервер
		sockaddr_in dest_addr = {0};
		dest_addr.sin_family = AF_INET;
		dest_addr.sin_addr.s_addr = address;
		dest_addr.sin_port = htons(port);

		PR_TRACE((g_root, prtNOTIFY, TR "NetUtilUDPSendReceive() sending to %d.%d.%d.%d:%d, size %d", 
			dest_addr.sin_addr.S_un.S_un_b.s_b1,
			dest_addr.sin_addr.S_un.S_un_b.s_b2,
			dest_addr.sin_addr.S_un.S_un_b.s_b3,
			dest_addr.sin_addr.S_un.S_un_b.s_b4,
			port,
			send_len));

		int nerr = sendto(udp_sock, (const char*)send_data, send_len, 0, (sockaddr*)&dest_addr, sizeof(dest_addr));
		if (nerr==SOCKET_ERROR)
		{
			PR_TRACE((g_root, prtERROR, TR "NetUtilUDPSendReceive() send failed %d", error = get_sock_err()));
			return PR_WIN32_TO_PRAGUE_ERR(error);
		}
	}

	if (recv_buffer_size)
	{
		// ѕрием сообщени€ с сервера
		fd_set read_fds; FD_ZERO(&read_fds); FD_SET(udp_sock, &read_fds);
		fd_set err_fds; FD_ZERO(&err_fds); FD_SET(udp_sock, &err_fds);
		timeval timeout = {timeout_ms / 1000, (timeout_ms % 1000) * 1000 };
		int n = select(0, &read_fds, NULL, &err_fds, &timeout);
		if (n==SOCKET_ERROR)
		{
			PR_TRACE((g_root, prtERROR, TR "NetUtilUDPSendReceive() select failed", error = get_sock_err()));
			return PR_WIN32_TO_PRAGUE_ERR(error);
		}
		if (!n)
		{
			PR_TRACE((g_root, prtERROR, TR "NetUtilUDPSendReceive() timeout"));
			return errTIMEOUT;
		}
		if (FD_ISSET(udp_sock, &err_fds))
		{
			PR_TRACE((g_root, prtERROR, TR "NetUtilUDPSendReceive() error on select", error = get_sock_err()));
			return PR_WIN32_TO_PRAGUE_ERR(error);
		}

		sockaddr_in server_addr;
		int server_addr_size=sizeof(server_addr);
		n=recvfrom(udp_sock, (char*)recv_buffer, recv_buffer_size, 0, (sockaddr*)&server_addr, &server_addr_size);
		if (n==SOCKET_ERROR)
		{
			error = get_sock_err();
			if (get_sock_err() == WSAECONNRESET)
			{
				PR_TRACE((g_root, prtERROR, TR "NetUtilUDPSendReceive() ICMP Port unreachable"));
				return PR_WIN32_TO_PRAGUE_ERR(error);
			}
			PR_TRACE((g_root, prtERROR, TR "NetUtilUDPSendReceive() receive failed %d", get_sock_err()));
			return PR_WIN32_TO_PRAGUE_ERR(error);
		}
		PR_TRACE((g_root, prtNOTIFY, TR "NetUtilUDPSendReceive() received from %d.%d.%d.%d:%d, size=%d", 
			server_addr.sin_addr.S_un.S_un_b.s_b1,
			server_addr.sin_addr.S_un.S_un_b.s_b2,
			server_addr.sin_addr.S_un.S_un_b.s_b3,
			server_addr.sin_addr.S_un.S_un_b.s_b4,
			ntohs(server_addr.sin_port),
			n));
		if (server_addr.sin_addr.s_addr != address)
		{
			PR_TRACE((g_root, prtERROR, TR "NetUtilUDPSendReceive() send/recv address mismatch"));
			return errUNEXPECTED;
		}
		if (received_size)
			*received_size = n;
	}
	return errOK;
}


tERROR NetUtilUDPSendReceive(
							unsigned long address, 
							unsigned short port, // host byte order
							const void* send_data,
							size_t send_len,
							void* recv_buffer,
							size_t recv_buffer_size,
							size_t* received_size,
							unsigned long timeout_ms)
{
	if (!InitWinsock())
		return errOBJECT_CANNOT_BE_INITIALIZED;
	tERROR error = errOK;
	SOCKET udp_sock=socket(AF_INET, SOCK_DGRAM, 0);
	if (udp_sock==INVALID_SOCKET)
	{
		error = get_sock_err();
		PR_TRACE((g_root, prtERROR, TR "NetUtilUDPSendReceive cannot create socket %d", error));
		return PR_WIN32_TO_PRAGUE_ERR(error);
	}

	error = _iNetUtilUDPSendReceive(udp_sock, address, port, send_data, send_len, recv_buffer, recv_buffer_size, received_size, timeout_ms);
	
	closesocket(udp_sock);
	return error;
}

unsigned short NetUtil_htons(unsigned short value)
{
	return htons(value);
}

unsigned short NetUtil_ntohs(unsigned short value)
{
	return ntohs(value);
}

unsigned long NetUtil_htonl(unsigned long value)
{
	return htonl(value);
}

unsigned long NetUtil_ntohl(unsigned long value)
{
	return ntohl(value);
}

tERROR NetUtilCopyStrToUtf8(cStringObj& str, void* utf8_buff, size_t utf8_buff_size)
{
	if (!utf8_buff_size || !utf8_buff)
		return errPARAMETER_INVALID;
	tERROR error = str.copy(utf8_buff, utf8_buff_size, cCP_UTF8);
	if (PR_SUCC(error))
		return error;
#ifdef _WIN32
	DWORD nWChars = WideCharToMultiByte(CP_UTF8, 0, str.data(), str.size()+1, (LPSTR)utf8_buff, utf8_buff_size, 0, 0);
	if (!nWChars)
		return errUNEXPECTED;
	return errOK;
#else
#error Insert UTF8 conversion code here
#endif
}

tERROR NetUtilAssignStrFromUtf8(cStringObj& str, void* utf8, size_t utf8_len)
{
	tERROR error = str.assign(utf8, cCP_UTF8, utf8_len);
	if (PR_SUCC(error))
		return error;
#ifdef _WIN32
	DWORD nWChars = MultiByteToWideChar(CP_UTF8, 0, (LPCSTR)utf8, utf8_len, 0, 0);
	if (PR_FAIL(error = str.resize(nWChars+1)))
		return error;
	DWORD nConverted = MultiByteToWideChar(CP_UTF8, 0, (LPCSTR)utf8, utf8_len, (LPWSTR)str.data(), nWChars);
	if (nConverted != nWChars)
		return errUNEXPECTED;
	((LPWSTR)str.data())[nWChars] = 0;
	return errOK;
#else
#error Insert UTF8 conversion code here
#endif
}
