#include "FtpServer.h"
#include <algorithm>

using namespace std;

FtpServer::FtpServer (int control_port, const std::string& address, IP_VERSION ipver):
	m_serverPort(control_port), m_address(address), m_ipver(ipver)
{
	WSADATA wsdata;
	int code;
	code = WSAStartup (MAKEWORD(2,0),&wsdata);
	if (code!=0) throw ( std::runtime_error ("Cannon initialize WSA") );
}

FtpServer::~FtpServer ()
{
	Stop();
	WSACleanup();
}

 void FtpServer::Start ()
{
	if (isIpV6())
	{//ipv6
		int code;
		SOCKADDR_IN6 saddr;

		m_controlSocket = socket(AF_INET6, SOCK_STREAM, IPPROTO_TCP);
		if (m_controlSocket == INVALID_SOCKET) throw (std::runtime_error("Cannot create socket"));

		int iMode = 1;
		code = ioctlsocket (m_controlSocket, FIONBIO, (ULONG*) &iMode);
		if (code!=0) throw (std::runtime_error("Cannot set socket option"));

		ZeroMemory(&saddr, sizeof (SOCKADDR_IN6));
		saddr.sin6_family = AF_INET6;
		int addr_size = sizeof(sockaddr_in6);
		code = WSAStringToAddressA ((char*)m_address.c_str(), AF_INET6, NULL, (LPSOCKADDR)&saddr, &addr_size);
		if (code!=0) throw (std::runtime_error("Cannot convert address"));
		saddr.sin6_port = htons (m_serverPort);

		code = bind (m_controlSocket,(SOCKADDR*) &saddr, sizeof(saddr));
		if (code!=0) throw (std::runtime_error("Cannot bind socket"));

		code = listen (m_controlSocket, SOMAXCONN);
		if (code!=0) throw (std::runtime_error("Cannot listen network"));
	}
	else
	{//ipv4
		int code;
		SOCKADDR_IN saddr;

		m_controlSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
		if (m_controlSocket == INVALID_SOCKET) throw (std::runtime_error("Cannot create socket"));

		int iMode = 1;
		code = ioctlsocket (m_controlSocket, FIONBIO, (ULONG*) &iMode);
		if (code!=0) throw (std::runtime_error("Cannot set socket option"));

		ZeroMemory(&saddr, sizeof (SOCKADDR_IN));
		saddr.sin_family = AF_INET;
		saddr.sin_port = htons (m_serverPort);
		saddr.sin_addr.s_addr = inet_addr (m_address.c_str());

		code = bind (m_controlSocket,(SOCKADDR*) &saddr, sizeof(saddr));
		if (code!=0) throw (std::runtime_error("Cannot bind socket"));

		code = listen (m_controlSocket, SOMAXCONN);
		if (code!=0) throw (std::runtime_error("Cannot listen network"));
	}


}

void FtpServer::Accept ()
{
	WSAEVENT hEvent;
	int code;
	WSANETWORKEVENTS wsnEvents;

	hEvent = WSACreateEvent();

	code = WSAEventSelect (m_controlSocket, hEvent, FD_ACCEPT);
	if (code!=0) throw (std::runtime_error("Cannot perform WSAEventSelect"));

	code = WSAWaitForMultipleEvents (1, &hEvent, false, WSA_INFINITE, false);
	if (code==WSA_WAIT_FAILED) throw (std::runtime_error("Cannot perform WSAWaitForMultipleEvents"));

	code = WSAEnumNetworkEvents(m_controlSocket, hEvent, &wsnEvents);
	if (code!=0) throw (std::runtime_error("Cannot perform WSAEnumNetworkEvents"));
	if (wsnEvents.iErrorCode[FD_ACCEPT_BIT]) throw (std::runtime_error("FD_ACCEPT_BIT error code"));
	
	if (isIpV6())
	{
		SOCKADDR_IN6 sa;
		int size = sizeof(SOCKADDR_IN6);
		m_controlSocketWork = accept (m_controlSocket, (sockaddr *)&sa, &size);
		m_connected = true;
	}
	else
	{
		SOCKADDR_IN sa;
		int size = sizeof(SOCKADDR_IN);
		m_controlSocketWork = accept (m_controlSocket, (sockaddr *)&sa, &size);
		m_connected = true;
	}

	
	int iMode = 1;
	code = ioctlsocket (m_controlSocketWork, FIONBIO, (ULONG*) &iMode);
	if (code!=0) throw (std::runtime_error("Cannot set socket option"));
	
	int bBuf = 0;
	code = setsockopt (m_controlSocketWork, SOL_SOCKET, SO_SNDBUF, (char*) &bBuf, sizeof(int));
	if (code!=0) throw (std::runtime_error("Cannot perform setsockopt"));
	code = setsockopt (m_controlSocketWork, SOL_SOCKET, SO_RCVBUF, (char*) &bBuf, sizeof(int));
	if (code!=0) throw (std::runtime_error("Cannot perform setsockopt"));

	WSACloseEvent (hEvent);

}

void FtpServer::AcceptDataConnection ()
{
	WSAEVENT hEvent;
	int code;
	WSANETWORKEVENTS wsnEvents;

	hEvent = WSACreateEvent();

	code = WSAEventSelect (m_dataSocket, hEvent, FD_ACCEPT);
	if (code!=0) throw (std::runtime_error("Cannot perform WSAEventSelect"));

	code = WSAWaitForMultipleEvents (1, &hEvent, false, WSA_INFINITE, false);
	if (code==WSA_WAIT_FAILED) throw (std::runtime_error("Cannot perform WSAWaitForMultipleEvents"));

	code = WSAEnumNetworkEvents(m_dataSocket, hEvent, &wsnEvents);
	if (code!=0) throw (std::runtime_error("Cannot perform WSAEnumNetworkEvents"));
	if (wsnEvents.iErrorCode[FD_ACCEPT_BIT]) throw (std::runtime_error("FD_ACCEPT_BIT error code"));
	
	if (isIpV6())
	{
		SOCKADDR_IN6 sa;
		int size = sizeof(SOCKADDR_IN6);
		m_dataSocketWork = accept (m_dataSocket, (SOCKADDR*)&sa, &size);
	}
	else
	{
		SOCKADDR sa;
		int size = sizeof(SOCKADDR);
		m_dataSocketWork = accept (m_dataSocket, &sa, &size);
	}
	

	shutdown (m_dataSocket, SD_BOTH);
	closesocket (m_dataSocket);
	m_data_connected = true;
	
	int iMode = 1;
	code = ioctlsocket (m_dataSocketWork, FIONBIO, (ULONG*) &iMode);
	if (code!=0) throw (std::runtime_error("Cannot set socket option"));
	
	int bBuf = 0;
	code = setsockopt (m_dataSocketWork, SOL_SOCKET, SO_SNDBUF, (char*) &bBuf, sizeof(int));
	if (code!=0) throw (std::runtime_error("Cannot perform setsockopt"));
	code = setsockopt (m_dataSocketWork, SOL_SOCKET, SO_RCVBUF, (char*) &bBuf, sizeof(int));
	if (code!=0) throw (std::runtime_error("Cannot perform setsockopt"));

	WSACloseEvent (hEvent);

}

void FtpServer::Stop ()
{
	CloseControlConnection();

	shutdown (m_controlSocket, SD_BOTH);
	closesocket (m_controlSocket);

	//shutdown (m_dataSocket, SD_BOTH);
	//closesocket (m_dataSocket);
			
}

void FtpServer::CloseDataConnection ()
{
	if (m_data_connected)
	{
		shutdown (m_dataSocketWork, SD_BOTH);
		closesocket (m_dataSocketWork);
		m_data_connected = !m_data_connected;
	}		
}

void FtpServer::CloseControlConnection ()
{
	if (m_connected)
	{
		shutdown (m_controlSocketWork, SD_BOTH);
		closesocket (m_controlSocketWork);
		m_connected = !m_connected;
	}
			
}

void FtpServer::CloseDataConnectionImpolite ()
{
	if (m_data_connected)
	{
		closesocket (m_dataSocketWork);
		m_data_connected = !m_data_connected;
	}		
}

void FtpServer::CloseControlConnectionImpolite ()
{
	if (m_connected)
	{
		closesocket (m_controlSocketWork);
		m_connected = !m_connected;
	}
			
}

void FtpServer::Receive (std::string& buffer, CMD_TYPE& type, std::string& param)
{
	WSAEVENT hEvent;
	int code;
	WSANETWORKEVENTS wsnEvents;

	hEvent = WSACreateEvent();

	code = WSAEventSelect (m_controlSocketWork, hEvent, FD_READ);
	if (code!=0) throw (std::runtime_error("Cannot perform WSAEventSelect"));

	while (true)
	{
		code = WSAWaitForMultipleEvents (1, &hEvent, false, WSA_INFINITE, false);
		if (code==WSA_WAIT_FAILED) throw (std::runtime_error("Cannot perform WSAWaitForMultipleEvents"));

		code = WSAEnumNetworkEvents(m_controlSocketWork, hEvent, &wsnEvents);
		if (code!=0) throw (std::runtime_error("Cannot perform WSAEnumNetworkEvents"));
		
		if (wsnEvents.lNetworkEvents&FD_READ)
		{
			//if (wsnEvents.iErrorCode[FD_READ]) throw (std::runtime_error("FD_READ error code"));
			char buf[1024];
			int bytesRead = recv (m_controlSocketWork, buf, 1024, 0);
			rcv_buf.append (&buf[0], &buf[0] + bytesRead);
		}
		size_t pos = rcv_buf.find ("\x0d\x0a");
		if (pos != string::npos)
		{
			buffer = rcv_buf.substr (0, pos);
			transform (buffer.c_str(), buffer.c_str() + buffer.length(), (char*)buffer.c_str(), tolower);
			rcv_buf.erase (0, pos+2); //+eol line
			DefineCommand (buffer, type);
			size_t pos2 = buffer.find (" ");
			if (pos2 != string::npos)
			{
				param = buffer.substr (pos2 + 1);
			} else param = "";
			WSACloseEvent (hEvent);
			return;
		}
	}
}

void FtpServer::Send (const string& buffer)
{
	send (m_controlSocketWork, buffer.c_str(), buffer.length(), 0);
}

void FtpServer::SendData (const string& buffer)
{
	send (m_dataSocketWork, buffer.c_str(), buffer.length(), 0);
}

void FtpServer::DefineCommand (const std::string& command, CMD_TYPE& type)
{
	if (command.find ("user") == 0)
	{
		type = CMD_USER;
		return;
	}
	if (command.find ("pass") == 0)
	{
		type = CMD_PASS;
		return;
	}
	if (command.find ("pasv") == 0)
	{
		type = CMD_PASV;
		return;
	}
	if (command.find ("type") == 0)
	{
		type = CMD_TTYPE;
		return;
	}
	if (command.find ("port") == 0)
	{
		type = CMD_PORT;
		return;
	}
	if (command.find ("retr") == 0)
	{
		type = CMD_RETR;
		return;
	}
	if (command.find ("rest") == 0)
	{
		type = CMD_REST;
		return;
	}
	if (command.find ("quit") == 0)
	{
		type = CMD_QUIT;
		return;
	}
	if (command.find ("epsv") == 0)
	{
		type = CMD_EPSV;
		return;
	}
	if (command.find ("eprt") == 0)
	{
		type = CMD_EPRT;
		return;
	}
	type = CMD_NONE;
}

void FtpServer::CreateBindListeningPassiveDataPort (string& address)
{
	SOCKADDR_IN saddr;
	CreateBindListeningPassiveDataPort_impl (saddr);

	char buf[10];
	address = "";
	char* s = inet_ntoa (saddr.sin_addr);
	address.append(s);
	address.append(",");

	u_short port = ntohs (saddr.sin_port);
	_itoa ((int)port/256,buf,10);
	address.append(buf);
	address.append(",");	
	_itoa ((int)port%256,buf,10);
	address.append(buf);

	for (unsigned index = 0; index = address.find(".", index), index != std::string::npos;)
	{
		address.replace(index, 1, ",");
		index += 1;
	}

}

void FtpServer::CreateBindListeningPassiveDataPortEx (string& address)
{

	u_short sin_port;
	CreateBindListeningPassiveDataPort_impl (sin_port);

	char buf[10];
	address = "|||";

	u_short port = ntohs (sin_port);
	_itoa ((int)port,buf,10);
	
	address.append(buf).append("|");

}

void FtpServer::CreateBindListeningPassiveDataPort_impl (u_short& result)
{

	if (isIpV6())
	{//ipv6

		int code;
		SOCKADDR_IN6 saddr;
		int startPort = 1025;

		m_dataSocket = socket(AF_INET6, SOCK_STREAM, IPPROTO_TCP);
		if (m_dataSocket == INVALID_SOCKET) throw (std::runtime_error("Cannot create socket"));

		int iMode = 1;
		code = ioctlsocket (m_dataSocket, FIONBIO, (ULONG*) &iMode);
		if (code!=0) throw (std::runtime_error("Cannot set socket option"));

		ZeroMemory(&saddr, sizeof (SOCKADDR_IN6));
		saddr.sin6_family = AF_INET6;
		int addr_size = sizeof(sockaddr_in6);
		code = WSAStringToAddressA ((char*)m_address.c_str(), AF_INET6, NULL, (LPSOCKADDR)&saddr, &addr_size);
		if (code!=0) throw (std::runtime_error("Cannot convert address"));

		do
		{
			saddr.sin6_port = htons (startPort);
			code = bind (m_dataSocket,(SOCKADDR*) &saddr, sizeof(saddr));
			startPort++;
		}
		while (code!=0);

		result = saddr.sin6_port;
	}

	else

	{//ipv4
		int code;
		SOCKADDR_IN saddr;
		int startPort = 1025;

		m_dataSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
		if (m_dataSocket == INVALID_SOCKET) throw (std::runtime_error("Cannot create socket"));

		int iMode = 1;
		code = ioctlsocket (m_dataSocket, FIONBIO, (ULONG*) &iMode);
		if (code!=0) throw (std::runtime_error("Cannot set socket option"));

		ZeroMemory(&saddr, sizeof (SOCKADDR_IN));
		saddr.sin_family = AF_INET;
		saddr.sin_addr.s_addr = inet_addr (m_address.c_str());
		saddr.sin_port = htons (m_serverPort);

		do
		{
			saddr.sin_port = htons (startPort);
			code = bind (m_dataSocket,(SOCKADDR*) &saddr, sizeof(saddr));
			startPort++;
		}
		while (code!=0);

		result = saddr.sin_port;
	}
}


void FtpServer::CreateBindListeningPassiveDataPort_impl (SOCKADDR_IN& result)
{

	_ASSERTE (!isIpV6());

	{//ipv4
		int code;
		SOCKADDR_IN saddr;
		int startPort = 1025;

		m_dataSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
		if (m_dataSocket == INVALID_SOCKET) throw (std::runtime_error("Cannot create socket"));

		int iMode = 1;
		code = ioctlsocket (m_dataSocket, FIONBIO, (ULONG*) &iMode);
		if (code!=0) throw (std::runtime_error("Cannot set socket option"));

		ZeroMemory(&saddr, sizeof (SOCKADDR_IN));
		saddr.sin_family = AF_INET;
		saddr.sin_addr.s_addr = inet_addr (m_address.c_str());
		saddr.sin_port = htons (m_serverPort);

		do
		{
			saddr.sin_port = htons (startPort);
			code = bind (m_dataSocket,(SOCKADDR*) &saddr, sizeof(saddr));
			startPort++;
		}
		while (code!=0);

		result = saddr;
	}

}

void FtpServer::StartListeningPassiveDataPortOnly ()
{
	int code;
	code = listen (m_dataSocket, SOMAXCONN);
	if (code!=0) throw (std::runtime_error("Cannot listen network"));
}

void FtpServer::StartListeningPassiveDataPort (std::string& address)
{
	CreateBindListeningPassiveDataPort (address);
	StartListeningPassiveDataPortOnly ();
}

void FtpServer::EstablishActiveConnectionEx (const string& param)
{
	char dlmt = param[0];

	size_t pos = 0;
	for (int i = 0; i < 2 || pos == string::npos; ++i)
		pos = param.find (dlmt, pos+1);
	if (pos == string::npos) throw (std::runtime_error(""));

	string host = param.substr (3, pos-3);

	size_t pos2 = param.find (dlmt, pos+1);

	if (pos2 == string::npos) throw (std::runtime_error(""));
	int port = atoi (param.substr (pos+1,pos2-pos-1).c_str());

	EstablishActiveConnection_impl (host, port);
}

void FtpServer::EstablishActiveConnection (const string& param)
{
	size_t pos = 0;
	for (int i = 0; i < 4 || pos == string::npos; ++i)
		pos = param.find (",", pos+1);
	if (pos == string::npos) throw (std::runtime_error(""));

	string host = param.substr (0, pos);
	for (unsigned index = 0; index = host.find(",", index), index != std::string::npos;)
	{
		host.replace(index, 1, ".");
		index += 1;
	}

	size_t pos2 = param.find (",", pos+1);
	if (pos2 == string::npos) throw (std::runtime_error(""));
	int port = atoi (param.substr (pos+1,pos2-pos-1).c_str())*256 + atoi (param.substr (pos2+1).c_str());

	EstablishActiveConnection_impl (host, port);
}

void FtpServer::EstablishActiveConnection_impl (const string& host, const int& port)
{
	if (isIpV6())
	{//ipv6

		int code, error = 1, iMode = 1;
		SOCKADDR_IN6 saddr;
		SOCKADDR_IN6 saddr_out;
		int addr_size = sizeof(sockaddr_in6);

		ZeroMemory(&saddr, sizeof (SOCKADDR_IN6));
		saddr.sin6_family = AF_INET6;
		code = WSAStringToAddressA ((char*)m_address.c_str(), AF_INET6, NULL, (LPSOCKADDR)&saddr, &addr_size);
		if (code!=0) throw (std::runtime_error("Cannot convert address"));
		saddr.sin6_port = htons (22);

		ZeroMemory(&saddr_out, sizeof (SOCKADDR_IN6));
		saddr_out.sin6_family = AF_INET6;
		code = WSAStringToAddressA ((char*)host.c_str(), AF_INET6, NULL, (LPSOCKADDR)&saddr_out, &addr_size);
		if (code!=0) throw (std::runtime_error("Cannot convert address"));
		saddr_out.sin6_port = htons (port);

		m_dataSocketWork = socket (AF_INET6, SOCK_STREAM, IPPROTO_TCP);
		
		BOOL bOptVal = TRUE;
		int bOptLen = sizeof(BOOL);

		code = setsockopt (m_dataSocketWork, SOL_SOCKET, SO_REUSEADDR, (char*) &bOptVal, bOptLen);
		if (code!=0) throw (std::runtime_error(""));

		code = bind (m_dataSocketWork, (SOCKADDR*) &saddr, sizeof (saddr));
		if (code!=0) throw (std::runtime_error(""));

		code = connect (m_dataSocketWork, (SOCKADDR*) &saddr_out, sizeof (saddr_out));
		if (code!=0) throw (std::runtime_error(""));
		m_data_connected = true;

		code = ioctlsocket (m_dataSocketWork, FIONBIO, (ULONG*) &iMode);
		if (code!=0) throw (std::runtime_error(""));

	}

	else

	{//ipv4

		int code, error = 1, iMode = 1;
		SOCKADDR_IN saddr;
		SOCKADDR_IN saddr_out;

		ZeroMemory(&saddr, sizeof (SOCKADDR_IN));
		saddr.sin_family = AF_INET;
		saddr.sin_port = htons (22);
		saddr.sin_addr.s_addr = inet_addr(m_address.c_str());

		ZeroMemory(&saddr_out, sizeof (SOCKADDR_IN));
		saddr_out.sin_family = AF_INET;
		saddr_out.sin_port = htons (port);
		saddr_out.sin_addr.s_addr = inet_addr(host.c_str());

		m_dataSocketWork = socket (AF_INET, SOCK_STREAM, IPPROTO_TCP);

		BOOL bOptVal = TRUE;
		int bOptLen = sizeof(BOOL);

		code = setsockopt (m_dataSocketWork, SOL_SOCKET, SO_REUSEADDR, (char*) &bOptVal, bOptLen);
		if (code!=0) throw (std::runtime_error(""));

		code = bind (m_dataSocketWork, (SOCKADDR*) &saddr, sizeof (saddr));
		if (code!=0) throw (std::runtime_error(""));

		code = connect (m_dataSocketWork, (SOCKADDR*) &saddr_out, sizeof (saddr_out));
		if (code!=0) throw (std::runtime_error(""));
		m_data_connected = true;

		code = ioctlsocket (m_dataSocketWork, FIONBIO, (ULONG*) &iMode);
		if (code!=0) throw (std::runtime_error(""));

	}

}

bool FtpServer::CheckDataConnectionClosed (int timeout)
{
	WSAEVENT hEvent;
	int code;
	WSANETWORKEVENTS wsnEvents;

	hEvent = WSACreateEvent();

	code = WSAEventSelect (m_dataSocketWork, hEvent, FD_CLOSE);
	if (code!=0) throw (std::runtime_error("Cannot perform WSAEventSelect"));

	while (true)
	{
		code = WSAWaitForMultipleEvents (1, &hEvent, false, timeout, false);
		if (code==WSA_WAIT_FAILED) throw (std::runtime_error("Cannot perform WSAWaitForMultipleEvents"));
		if (code==WSA_WAIT_TIMEOUT)
		{
			WSACloseEvent (hEvent);
			return false;
		}

		code = WSAEnumNetworkEvents(m_dataSocketWork, hEvent, &wsnEvents);
		if (code!=0) throw (std::runtime_error("Cannot perform WSAEnumNetworkEvents"));
		
		if (wsnEvents.lNetworkEvents&FD_CLOSE)
		{
			CloseDataConnection ();
			WSACloseEvent (hEvent);
			return true;
		}
	}
}

bool FtpServer::CheckControlConnectionClosed (int timeout)
{
	WSAEVENT hEvent;
	int code;
	WSANETWORKEVENTS wsnEvents;

	hEvent = WSACreateEvent();

	code = WSAEventSelect (m_controlSocketWork, hEvent, FD_CLOSE);
	if (code!=0) throw (std::runtime_error("Cannot perform WSAEventSelect"));

	while (true)
	{
		code = WSAWaitForMultipleEvents (1, &hEvent, false, timeout, false);
		if (code==WSA_WAIT_FAILED) throw (std::runtime_error("Cannot perform WSAWaitForMultipleEvents"));
		if (code==WSA_WAIT_TIMEOUT)
		{
			WSACloseEvent (hEvent);
			return false;
		}

		code = WSAEnumNetworkEvents(m_controlSocketWork, hEvent, &wsnEvents);
		if (code!=0) throw (std::runtime_error("Cannot perform WSAEnumNetworkEvents"));
		
		if (wsnEvents.lNetworkEvents&FD_CLOSE)
		{
			CloseControlConnection ();
			WSACloseEvent (hEvent);
			return true;
		}
	}
}

void FtpServer::Hello ()
{
	Accept ();
	Send ("220 Hello\x0d\x0a");
}


bool FtpServer::Authorize (const std::string& user, const std::string& pass, std::wstring& result)
{
	string command;
	string param;
	FtpServer::CMD_TYPE type;

	Receive(command, type, param);
	if (type != FtpServer::CMD_USER || param != user)
	{
		result = L"Command USER not received or incorrect\n";
		return false;
	}
	Send ("331 Anonymous access allowed, send identity (e-mail name) as password.\x0d\x0a");
	Receive(command, type, param);
	if (type != FtpServer::CMD_PASS || param != pass)
	{
		result = L"Command PASS not received or incorrect\n";
		return false;
	}
	Send ("230 Logged in.\x0d\x0a");
	return true;
}

bool FtpServer::ProcessTypeCommand (std::wstring& result)
{
	string command;
	string param;
	FtpServer::CMD_TYPE type;

	Receive(command, type, param);
	if (type != FtpServer::CMD_TTYPE)
	{
		result = L"Command TYPE not received or incorrect\n";
		return false;
	}
	Send ("200 Command okay.\x0d\x0a");
	return true;
}

bool FtpServer::SendFileFromFile (const std::string& filename, const std::wstring& server_file, const bool active_mode, std::wstring& result)
{
	//filling buffer
	string filebuf;
	FILE* file = _wfopen (server_file.c_str(), L"rb");
	{//file exists
		char buf[1024];
		int count = 1;
		while (count)
		{
			count = fread (buf, 1, 1024, file);
			if (count != 0) filebuf.append (buf, buf + count);
		}
		fclose (file);
	}

	string command;
	string param;
	FtpServer::CMD_TYPE type;

	Receive(command, type, param);
	
	if (!active_mode)
	{//passive mode
		_ASSERTE (type == FtpServer::CMD_EPSV);

		Send ("500 Error.\x0d\x0a");
		Receive(command, type, param);

		if (type != FtpServer::CMD_PASV)
		{
			result = L"Command PASV not received or incorrect\n";
			return false;
		}

		string addr;
		CreateBindListeningPassiveDataPort (addr);
		StartListeningPassiveDataPortOnly ();
		Send (string("227 Entering Passive Mode (").append(addr).append(").\x0d\x0a"));
		AcceptDataConnection ();

		Receive(command, type, param);
		if (type != FtpServer::CMD_RETR || param != filename)
		{
			result = L"Command RETR not received or incorrect\n";
			return false;
		}

		Send ("125 Data connection already open; Transfer starting.\x0d\x0a");
		SendData (filebuf.c_str());
		CloseDataConnection ();
		Send ("226 Transfer complete.\x0d\x0a");
	}
	else
	{//active mode
		_ASSERTE (type == FtpServer::CMD_EPRT);

		Send ("500 Error.\x0d\x0a");
		Receive(command, type, param);

		if (type != FtpServer::CMD_PORT)
		{
			result = L"Command PORT not received or incorrect\n";
			return false;
		}
		Send ("200 PORT command okay.\x0d\x0a");

		EstablishActiveConnection(param);

		Receive(command, type, param);
		if (type != FtpServer::CMD_RETR || param != filename)
		{
			result = L"Command RETR not received or incorrect\n";
			return false;
		}
		Send ("125 Data connection already open; Transfer starting.\x0d\x0a");
		SendData ("0123456789");
		CloseDataConnection ();
		Send ("226 Transfer complete.\x0d\x0a");
	}
	return true;
}

bool FtpServer::SendFileFromBuffer (const std::string& filename, const std::string& buffer, const bool active_mode, std::wstring& result)
{
	//filling buffer
	string filebuf = buffer;

	string command;
	string param;
	FtpServer::CMD_TYPE type;

	Receive(command, type, param);

	if (!active_mode)
	{//passive mode


		_ASSERTE (type == FtpServer::CMD_EPSV);

		Send ("500 Error.\x0d\x0a");
		Receive(command, type, param);

		if (type != FtpServer::CMD_PASV)
		{
			result = L"Command PASV not received or incorrect\n";
			return false;
		}

		string addr;
		CreateBindListeningPassiveDataPort (addr);
		StartListeningPassiveDataPortOnly ();
		Send (string("227 Entering Passive Mode (").append(addr).append(").\x0d\x0a"));
		AcceptDataConnection ();

		Receive(command, type, param);
		if (type != FtpServer::CMD_RETR || param != filename)
		{
			result = L"Command RETR not received or incorrect\n";
			return false;
		}

		Send ("125 Data connection already open; Transfer starting.\x0d\x0a");
		SendData (filebuf.c_str());
		CloseDataConnection ();
		Send ("226 Transfer complete.\x0d\x0a");
	}
	else
	{//active mode
		_ASSERTE (type == FtpServer::CMD_EPRT);

		Send ("500 Error.\x0d\x0a");
		Receive(command, type, param);

		if (type != FtpServer::CMD_PORT)
		{
			result = L"Command PORT not received or incorrect\n";
			return false;
		}
		Send ("200 PORT command okay.\x0d\x0a");

		EstablishActiveConnection(param);

		Receive(command, type, param);
		if (type != FtpServer::CMD_RETR || param != filename)
		{
			result = L"Command RETR not received or incorrect\n";
			return false;
		}
		Send ("125 Data connection already open; Transfer starting.\x0d\x0a");
		SendData ("0123456789");
		CloseDataConnection ();
		Send ("226 Transfer complete.\x0d\x0a");
	}
	return true;
}

bool FtpServer::SendFileNotFound(const std::string& filename, const bool active_mode, std::wstring& result)
{
	string command;
	string param;
	FtpServer::CMD_TYPE type;

	Receive(command, type, param);

	if (!active_mode)
	{//passive mode

		_ASSERTE (type == FtpServer::CMD_EPSV);

		Send ("500 Error.\x0d\x0a");
		Receive(command, type, param);

		if (type != FtpServer::CMD_PASV)
		{
			result = L"Command PASV not received or incorrect\n";
			return false;
		}

		string addr;
		CreateBindListeningPassiveDataPort (addr);
		StartListeningPassiveDataPortOnly ();
		Send (string("227 Entering Passive Mode (").append(addr).append(").\x0d\x0a"));
		AcceptDataConnection ();

		Receive(command, type, param);
		if (type != FtpServer::CMD_RETR || param != filename)
		{
			result = L"Command RETR not received or incorrect\n";
			return false;
		}

		CloseDataConnection ();
		Send ("550 File not found.\x0d\x0a");
	}
	else
	{//active mode
		_ASSERTE (type == FtpServer::CMD_EPRT);

		Send ("500 Error.\x0d\x0a");
		Receive(command, type, param);

		if (type != FtpServer::CMD_PORT)
		{
			result = L"Command PORT not received or incorrect\n";
			return false;
		}
		Send ("200 PORT command okay.\x0d\x0a");

		EstablishActiveConnection(param);

		Receive(command, type, param);
		if (type != FtpServer::CMD_RETR || param != filename)
		{
			result = L"Command RETR not received or incorrect\n";
			return false;
		}

		CloseDataConnection ();
		Send ("550 File not found.\x0d\x0a");
	}
	return true;
}