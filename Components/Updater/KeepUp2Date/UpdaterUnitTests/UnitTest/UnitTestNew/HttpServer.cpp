#include "HttpServer.h"
#include <algorithm>
#include <map>

#include "util.h"

using namespace std;

HttpServer::HttpServer (int control_port, const std::string& address, IP_VERSION ipversion):
	m_serverPort(control_port), m_address(address), m_ipversion(ipversion)
{
	WSADATA wsdata;
	int code;
	code = WSAStartup (MAKEWORD(2,0),&wsdata);
	if (code!=0) throw ( std::runtime_error ("Cannon initialize WSA") );
}

HttpServer::~HttpServer ()
{
	Stop();
	WSACleanup();
}

 void HttpServer::Start ()
{
	if (isIpV6())
	{

		int code;
		SOCKADDR_IN6 saddr;

		m_mainSocket = socket(AF_INET6, SOCK_STREAM, IPPROTO_TCP);
		if (m_mainSocket == INVALID_SOCKET) throw (std::runtime_error("Cannot create socket"));

		int iMode = 1;
		code = ioctlsocket (m_mainSocket, FIONBIO, (ULONG*) &iMode);
		if (code!=0) throw (std::runtime_error("Cannot set socket option"));

		ZeroMemory(&saddr, sizeof (SOCKADDR_IN6));
		saddr.sin6_family = AF_INET6;
		int addr_size = sizeof(sockaddr_in6);
		code = WSAStringToAddressA ((char*)m_address.c_str(), AF_INET6, NULL, (LPSOCKADDR)&saddr, &addr_size);
		if (code!=0) throw (std::runtime_error("Cannot convert address"));
		saddr.sin6_port = htons (m_serverPort);

		code = bind (m_mainSocket,(SOCKADDR*) &saddr, sizeof(saddr));
		if (code!=0) throw (std::runtime_error("Cannot bind socket"));

		code = listen (m_mainSocket, SOMAXCONN);
		if (code!=0) throw (std::runtime_error("Cannot listen network"));
	}
	else
	{

		int code;
		SOCKADDR_IN saddr;

		m_mainSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
		if (m_mainSocket == INVALID_SOCKET) throw (std::runtime_error("Cannot create socket"));

		int iMode = 1;
		code = ioctlsocket (m_mainSocket, FIONBIO, (ULONG*) &iMode);
		if (code!=0) throw (std::runtime_error("Cannot set socket option"));

		ZeroMemory(&saddr, sizeof (SOCKADDR_IN));
		saddr.sin_family = AF_INET;
		saddr.sin_port = htons (m_serverPort);
		saddr.sin_addr.s_addr = inet_addr (m_address.c_str());

		code = bind (m_mainSocket,(SOCKADDR*) &saddr, sizeof(saddr));
		if (code!=0) throw (std::runtime_error("Cannot bind socket"));

		code = listen (m_mainSocket, SOMAXCONN);
		if (code!=0) throw (std::runtime_error("Cannot listen network"));
	}

}

int HttpServer::Accept (int timeout)
{
	WSAEVENT hEvent;
	int code;
	WSANETWORKEVENTS wsnEvents;

	hEvent = WSACreateEvent();

	code = WSAEventSelect (m_mainSocket, hEvent, FD_ACCEPT);
	if (code!=0) throw (std::runtime_error("Cannot perform WSAEventSelect"));

	code = WSAWaitForMultipleEvents (1, &hEvent, false, timeout, false);
	if (code == WSA_WAIT_FAILED) throw (std::runtime_error("Cannot perform WSAWaitForMultipleEvents"));
	if (code == WSA_WAIT_TIMEOUT) return code;

	code = WSAEnumNetworkEvents(m_mainSocket, hEvent, &wsnEvents);
	if (code!=0) throw (std::runtime_error("Cannot perform WSAEnumNetworkEvents"));
	if (wsnEvents.iErrorCode[FD_ACCEPT_BIT]) throw (std::runtime_error("FD_ACCEPT_BIT error code"));
	
	if (isIpV6())
	{
		SOCKADDR_IN6 sa;
		int size = sizeof(SOCKADDR_IN6);
		m_socketWork = accept (m_mainSocket, (SOCKADDR*)&sa, &size);
		m_connected = true;
	}
	else
	{
		SOCKADDR sa;
		int size = sizeof(SOCKADDR);
		m_socketWork = accept (m_mainSocket, &sa, &size);
		m_connected = true;
	}
	
	
	int iMode = 1;
	code = ioctlsocket (m_socketWork, FIONBIO, (ULONG*) &iMode);
	if (code!=0) throw (std::runtime_error("Cannot set socket option"));
	
	int bBuf = 0;
	code = setsockopt (m_socketWork, SOL_SOCKET, SO_SNDBUF, (char*) &bBuf, sizeof(int));
	if (code!=0) throw (std::runtime_error("Cannot perform setsockopt"));
	code = setsockopt (m_socketWork, SOL_SOCKET, SO_RCVBUF, (char*) &bBuf, sizeof(int));
	if (code!=0) throw (std::runtime_error("Cannot perform setsockopt"));

	WSACloseEvent (hEvent);

	return code;
}

void HttpServer::Stop ()
{

	shutdown (m_mainSocket, SD_BOTH);
	closesocket (m_mainSocket);
			
}

void HttpServer::CloseConnection ()
{
	if (m_connected)
	{
		//shutdown (m_socketWork, SD_BOTH);
		//closesocket (m_socketWork);
		//m_connected = !m_connected;
		int code;
		HANDLE hEvent = WSACreateEvent();
		WSANETWORKEVENTS wsnEvents;
		code = WSAEventSelect (m_socketWork, hEvent, FD_CLOSE);
		code = WSAEnumNetworkEvents(m_socketWork, hEvent, &wsnEvents);

		shutdown (m_socketWork, SD_SEND);
		//closesocket (m_socketWork);
		//m_connected = !m_connected;
		WSACloseEvent(hEvent);
	}
			
}

void HttpServer::BreakConnection ()
{
	if (m_connected)
	{
		closesocket (m_socketWork);
		m_connected = !m_connected;
	}
			
}


void HttpServer::Receive (std::string& buffer, CMD_TYPE& type, std::string& param)
{
	WSAEVENT hEvent;
	int code;
	WSANETWORKEVENTS wsnEvents;

	hEvent = WSACreateEvent();

	code = WSAEventSelect (m_socketWork, hEvent, FD_READ);
	if (code!=0) throw (std::runtime_error("Cannot perform WSAEventSelect"));

	while (true)
	{
		code = WSAWaitForMultipleEvents (1, &hEvent, false, WSA_INFINITE, false);
		if (code==WSA_WAIT_FAILED) throw (std::runtime_error("Cannot perform WSAWaitForMultipleEvents"));

		code = WSAEnumNetworkEvents(m_socketWork, hEvent, &wsnEvents);
		if (code!=0) throw (std::runtime_error("Cannot perform WSAEnumNetworkEvents"));
		
		if (wsnEvents.lNetworkEvents&FD_READ)
		{
			//if (wsnEvents.iErrorCode[FD_READ]) throw (std::runtime_error("FD_READ error code"));
			char buf[1024];
			int bytesRead = recv (m_socketWork, buf, 1024, 0);
			rcv_buf.append (&buf[0], &buf[0] + bytesRead);
		}
		size_t pos = rcv_buf.find ("\x0d\x0a\x0d\x0a");
		if (pos != string::npos)
		{
			buffer = rcv_buf.substr (0, pos);
			transform (buffer.c_str(), buffer.c_str() + buffer.length(), (char*)buffer.c_str(), tolower);
			DefineCommand (buffer, type);
			if (type == CMD_POST)
			{
				size_t cl_pos = buffer.find ("content-length: ");
				_ASSERTE (cl_pos != string::npos);
				cl_pos += 16;
				size_t cl_pos_end = buffer.find ("\x0d\x0a", cl_pos);
				if (cl_pos_end == string::npos) cl_pos_end = buffer.size();
				char _buf[20];
				strncpy (_buf, buffer.c_str()+cl_pos, cl_pos_end - cl_pos);
				_buf[cl_pos_end-cl_pos] = 0;
				int content_length = atoi (_buf);

				while ((rcv_buf.size() < pos + 4 + content_length))
				{
					//недополучены данные с POST
					code = WSAWaitForMultipleEvents (1, &hEvent, false, WSA_INFINITE, false);
					if (code==WSA_WAIT_FAILED) throw (std::runtime_error("Cannot perform WSAWaitForMultipleEvents"));

					code = WSAEnumNetworkEvents(m_socketWork, hEvent, &wsnEvents);
					if (code!=0) throw (std::runtime_error("Cannot perform WSAEnumNetworkEvents"));

					if (wsnEvents.lNetworkEvents&FD_READ)
					{
						//if (wsnEvents.iErrorCode[FD_READ]) throw (std::runtime_error("FD_READ error code"));
						char buf[1024];
						int bytesRead = recv (m_socketWork, buf, 1024, 0);
						rcv_buf.append (&buf[0], &buf[0] + bytesRead);
					}
				}

				buffer = rcv_buf.substr (0, pos + 4 + content_length);

				rcv_buf.erase (0, pos + 4 + content_length); //+double eol line
				size_t pos2 = buffer.find (" ");
				if (pos2 != string::npos)
				{
					param = buffer.substr (pos2 + 1);
				} else param = "";

			}
			else
			{//other commands
				rcv_buf.erase (0, pos+4); //+double eol line
				size_t pos2 = buffer.find (" ");
				if (pos2 != string::npos)
				{
					param = buffer.substr (pos2 + 1);
				} else param = "";
			}
			
			WSACloseEvent (hEvent);
			return;
		}
	}
}

void HttpServer::Send (const string& buffer)
{
	send (m_socketWork, buffer.c_str(), buffer.length(), 0);
}

void HttpServer::Send (char* buffer, int length)
{
	send (m_socketWork, buffer, length, 0);
}

void HttpServer::DefineCommand (const std::string& command, CMD_TYPE& type)
{
	if (command.find ("get") == 0)
	{
		type = CMD_GET;
		return;
	}
	if (command.find ("head") == 0)
	{
		type = CMD_HEAD;
		return;
	}
	if (command.find ("post") == 0)
	{
		type = CMD_POST;
		return;
	}
	type = CMD_NONE;
}


bool HttpServer::CheckConnectionClosed (int timeout)
{
	WSAEVENT hEvent;
	int code;
	WSANETWORKEVENTS wsnEvents;

	hEvent = WSACreateEvent();

	code = WSAEventSelect (m_socketWork, hEvent, FD_CLOSE);
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

		code = WSAEnumNetworkEvents(m_socketWork, hEvent, &wsnEvents);
		if (code!=0) throw (std::runtime_error("Cannot perform WSAEnumNetworkEvents"));
		
		if (wsnEvents.lNetworkEvents&FD_CLOSE)
		{
			CloseConnection ();
			WSACloseEvent (hEvent);
			return true;
		}
	}
}

void HttpServer::SendFile (const std::wstring& file, bool proxy, bool connection, bool proxy_connection)
{
	_helper::AutoFile af (file);
	char buf[20];
	int fileSize = af.GetFileSize();

	vector<char> filebuf;
	filebuf.reserve (fileSize);
	filebuf.resize(fileSize);
	int bytesRead = af.GetContent (&filebuf[0], fileSize);
	if (bytesRead != fileSize) return;

	map<bool, string> conn_map;
	conn_map.insert(map<bool, string>::value_type(false, "Close"));
	conn_map.insert(map<bool, string>::value_type(true, "Keep-Alive"));

	Send("HTTP/1.1 200 OK\x0d\x0a");
	Send("Content-Length: "); Send (itoa (bytesRead, buf, 10)); Send ("\x0d\x0a");
	Send("Connection: "); Send (conn_map[connection]); Send ("\x0d\x0a");
	if (proxy)
	{
		Send("Proxy-Connection: "); Send (conn_map[proxy_connection]); Send ("\x0d\x0a");
	}
	
	Send("\x0d\x0a");

	Send(&filebuf[0], bytesRead);

}

void HttpServer::SendFilePartly (const std::wstring& file, bool proxy, bool connection, bool proxy_connection)
{
	const int bytesRest = 100;

	_helper::AutoFile af (file);
	char buf[20];
	int fileSize = af.GetFileSize();
	int fileSizeOrigin = fileSize;
	fileSize -=  bytesRest;
	if (fileSize < 1) fileSize = 1;

	vector<char> filebuf;
	filebuf.reserve (fileSize);
	filebuf.resize (fileSize);
	int bytesRead = af.GetContent (&filebuf[0], fileSize);
	if (bytesRead != fileSize) return;

	map<bool, string> conn_map;
	conn_map.insert(map<bool, string>::value_type(false, "Close"));
	conn_map.insert(map<bool, string>::value_type(true, "Keep-Alive"));

	Send("HTTP/1.1 200 OK\x0d\x0a");
	Send("Content-Length: "); Send (itoa (fileSizeOrigin, buf, 10)); Send ("\x0d\x0a");
	Send("Connection: Keep-Alive\x0d\x0a");

	Send("\x0d\x0a");

	Send(&filebuf[0], bytesRead);

}

void HttpServer::SendNtlmGreeting (const ConnType& keep_alive)
{
	string conn;
	keep_alive == HttpServer::KeepAlive ? conn = "Keep-Alive" : conn = "Close";
	Send("HTTP/1.1 407 Proxy Authentication Required\x0d\x0a");
	Send("Proxy-Authenticate: NTLM\x0d\x0a");
	Send("Content-Length: 0\x0d\x0a");
	Send("Connection: Keep-Alive\x0d\x0a");
	Send("Proxy-Connection: ");
	Send(conn);
	Send("\x0d\x0a");
	Send("\x0d\x0a");
}

void HttpServer::SendNtlmBasicGreeting (const ConnType& keep_alive)
{
	string conn;
	keep_alive == HttpServer::KeepAlive ? conn = "Keep-Alive" : conn = "Close";
	Send("HTTP/1.1 407 Proxy Authentication Required\x0d\x0a");
	Send("Proxy-Authenticate: NTLM\x0d\x0a");
	Send("Proxy-Authenticate: Basic realm=\"Hello\"\x0d\x0a");
	Send("Content-Length: 0\x0d\x0a");
	Send("Connection: Keep-Alive\x0d\x0a");
	Send("Proxy-Connection: ");
	Send(conn);
	Send("\x0d\x0a");
	Send("\x0d\x0a");
}

void HttpServer::SendBasicGreeting (const ConnType& keep_alive)
{
	string conn;
	keep_alive == HttpServer::KeepAlive ? conn = "Keep-Alive" : conn = "Close";
	Send("HTTP/1.1 407 Proxy Authentication Required\x0d\x0a");
	Send("Proxy-Authenticate: Basic realm=\"Hello\"\x0d\x0a");
	Send("Content-Length: 0\x0d\x0a");
	Send("Connection: Keep-Alive\x0d\x0a");
	Send("Proxy-Connection: ");
	Send(conn);
	Send("\x0d\x0a");
	Send("\x0d\x0a");
}


void HttpServer::SendNtlmMsg2 ()
{
	Send("HTTP/1.1 407 Proxy Authentication Required\x0d\x0a");
	Send("Content-Length: 0\x0d\x0a");
	Send("Connection: Keep-Alive\x0d\x0a");
	Send("Proxy-Connection: Keep-Alive\x0d\x0a");
	Send("Proxy-Authenticate: NTLM TlRMTVNTUAACAAAABAAEADAAAAAFgomg1xL3GW9+PnoAAAAAAAAAAFoAWgA0AAAASwBMAAIABABLAEwAAQAUAFQATAAtAFAAUgBYAC0ATgBUAEEABAAMAGEAdgBwAC4AcgB1AAMAIgB0AGwALQBwAHIAeAAtAG4AdABhAC4AYQB2AHAALgByAHUAAAAAAA==\x0d\x0a");
	Send("\x0d\x0a");
}

void HttpServer::SendFileFromBuffer (const ConnType& conn_ka, const ConnType& proxy_conn_ka, string _message)
{
	//filling buffer
	string filebuf = _message;

	string conn;
	conn_ka == KeepAlive ? conn = "Keep-Alive" : conn = "Close";
	string proxy_conn;
	proxy_conn_ka == KeepAlive ? proxy_conn = "Keep-Alive" : proxy_conn = "Close";
	char length[10];
	itoa (filebuf.length(), length, 10);

	Send("HTTP/1.1 200 OK\x0d\x0a");
	Send("Content-Length: ");
	Send(length);
	Send("\x0d\x0a");
	if (conn_ka != None)
	{
		Send("Connection: ");
		Send(conn);
		Send("\x0d\x0a");
	}
	if (proxy_conn_ka != None)
	{
		Send("Proxy-Connection: ");
		Send(proxy_conn);
		Send("\x0d\x0a");
	}
	Send("\x0d\x0a");
	Send(filebuf);
}

void HttpServer::SendFileFromFile (const ConnType& conn_ka, const ConnType& proxy_conn_ka, wstring _message)
{
	//filling buffer
	string filebuf;
	FILE* file = _wfopen (_message.c_str(), L"rb");

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

	string conn;
	conn_ka == KeepAlive ? conn = "Keep-Alive" : conn = "Close";
	string proxy_conn;
	proxy_conn_ka == KeepAlive ? proxy_conn = "Keep-Alive" : proxy_conn = "Close";
	char length[10];
	itoa (filebuf.length(), length, 10);

	Send("HTTP/1.1 200 OK\x0d\x0a");
	Send("Content-Length: ");
	Send(length);
	Send("\x0d\x0a");
	if (conn_ka != None)
	{
		Send("Connection: ");
		Send(conn);
		Send("\x0d\x0a");
	}
	if (proxy_conn_ka != None)
	{
		Send("Proxy-Connection: ");
		Send(proxy_conn);
		Send("\x0d\x0a");
	}
	Send("\x0d\x0a");
	Send(filebuf);
}

bool HttpServer::ReceiveGetWithNtlmToken (std::string custom)
{
	string command;
	string param;
	HttpServer::CMD_TYPE type;
	bool result = true;

	Receive(command, type, param);
	if (type != HttpServer::CMD_GET)
	{
		std::wcout<<"Received command is not GET\n";
		 result = false;
	}
	if (command.find("proxy-authorization: ntlm") == string::npos)
	{
		std::wcout<<"Missing NTLM token in request\n";
		result = false;
	}
	if (!custom.empty() && command.find(custom) == string::npos)
	{
		std::wcout<<"Missing custom substring in request\n";
		result = false;
	}
	return result;
}

bool HttpServer::ReceiveGetWithBasicToken (std::string custom)
{
	string command;
	string param;
	HttpServer::CMD_TYPE type;
	bool result = true;

	Receive(command, type, param);
	if (type != HttpServer::CMD_GET)
	{
		std::wcout<<"Received command is not GET\n";
		result = false;
	}
	if (command.find("proxy-authorization: basic") == string::npos)
	{
		std::wcout<<"Missing basic token in request\n";
		result = false;
	}
	if (!custom.empty() && command.find(custom) == string::npos)
	{
		std::wcout<<"Missing custom substring in request\n";
		result = false;
	}
	return result;
}

bool HttpServer::ReceivePureGet (std::string custom)
{
	string command;
	string param;
	HttpServer::CMD_TYPE type;
	bool result = true;

	Receive(command, type, param);
	if (type != HttpServer::CMD_GET)
	{
		std::wcout<<"Received command is not GET\n";
		result = false;
	}
	if (command.find("proxy-authorization: ntlm") != string::npos)
	{
		std::wcout<<"Missing NTLM token in request\n";
		result = false;
	}
	if (command.find("proxy-authorization: basic") != string::npos)
	{
		std::wcout<<"Missing Basic token in request\n";
		result = false;
	}
	if (!custom.empty() && command.find(custom) == string::npos)
	{
		std::wcout<<"Missing custom substring in request\n";
		result = false;
	}
	return result;
}

void HttpServer::Send403Forbidden (const ConnType& conn_ka, const ConnType& proxy_conn_ka)
{
	string conn;
	conn_ka == KeepAlive ? conn = "Keep-Alive" : conn = "Close";
	string proxy_conn;
	proxy_conn_ka == KeepAlive ? proxy_conn = "Keep-Alive" : proxy_conn = "Close";

	Send("HTTP/1.1 403 Forbidden\x0d\x0a");
	Send("Content-Length: 0\x0d\x0a");
	if (conn_ka != None)
	{
		Send("Connection: ");
		Send(conn);
		Send("\x0d\x0a");
	}
	if (proxy_conn_ka != None)
	{
		Send("Proxy-Connection: ");
		Send(proxy_conn);
		Send("\x0d\x0a");
	}
	Send("\x0d\x0a");
}

void HttpServer::Send404NotFound (const ConnType& conn_ka, const ConnType& proxy_conn_ka)
{
	string conn;
	conn_ka == KeepAlive ? conn = "Keep-Alive" : conn = "Close";
	string proxy_conn;
	proxy_conn_ka == KeepAlive ? proxy_conn = "Keep-Alive" : proxy_conn = "Close";

	Send("HTTP/1.1 404 Not Found\x0d\x0a");
	Send("Content-Length: 0\x0d\x0a");
	if (conn_ka != None)
	{
		Send("Connection: ");
		Send(conn);
		Send("\x0d\x0a");
	}
	if (proxy_conn_ka != None)
	{
		Send("Proxy-Connection: ");
		Send(proxy_conn);
		Send("\x0d\x0a");
	}
	Send("\x0d\x0a");
}

void HttpServer::Send200OK (const ConnType& conn_ka, const ConnType& proxy_conn_ka, const string& text)
{
	string conn;
	char temp[20];
	conn_ka == KeepAlive ? conn = "Keep-Alive" : conn = "Close";
	string proxy_conn;
	proxy_conn_ka == KeepAlive ? proxy_conn = "Keep-Alive" : proxy_conn = "Close";

	Send("HTTP/1.1 200 OK\x0d\x0a");
	if (!text.empty())
	{
		Send("Content-Length: ");
		Send(itoa (text.size(), temp, 10));
		Send("\x0d\x0a");
	};
	if (conn_ka != None)
	{
		Send("Connection: ");
		Send(conn);
		Send("\x0d\x0a");
	}
	if (proxy_conn_ka != None)
	{
		Send("Proxy-Connection: ");
		Send(proxy_conn);
		Send("\x0d\x0a");
	}
	Send("\x0d\x0a");
	if (!text.empty()) Send (text);
}

void HttpServer::Send204OK (const ConnType& conn_ka, const ConnType& proxy_conn_ka)
{
	string conn;
	conn_ka == KeepAlive ? conn = "Keep-Alive" : conn = "Close";
	string proxy_conn;
	proxy_conn_ka == KeepAlive ? proxy_conn = "Keep-Alive" : proxy_conn = "Close";

	Send("HTTP/1.1 204 OK\x0d\x0a");
	if (conn_ka != None)
	{
		Send("Connection: ");
		Send(conn);
		Send("\x0d\x0a");
	}
	if (proxy_conn_ka != None)
	{
		Send("Proxy-Connection: ");
		Send(proxy_conn);
		Send("\x0d\x0a");
	}
	Send("\x0d\x0a");
}

void HttpServer::Send502Denied (const ConnType& conn_ka, const ConnType& proxy_conn_ka)
{
	string conn;
	conn_ka == KeepAlive ? conn = "Keep-Alive" : conn = "Close";
	string proxy_conn;
	proxy_conn_ka == KeepAlive ? proxy_conn = "Keep-Alive" : proxy_conn = "Close";

	Send("HTTP/1.1 502 Bad Gateway\x0d\x0a");
	Send("Content-Length: 0\x0d\x0a");
	if (conn_ka != None)
	{
		Send("Connection: ");
		Send(conn);
		Send("\x0d\x0a");
	}
	if (proxy_conn_ka != None)
	{
		Send("Proxy-Connection: ");
		Send(proxy_conn);
		Send("\x0d\x0a");
	}
	Send("\x0d\x0a");
}

void HttpServer::Send302Redirected (const ConnType& conn_ka, const ConnType& proxy_conn_ka, const string& url)
{
	string conn;
	conn_ka == KeepAlive ? conn = "Keep-Alive" : conn = "Close";
	string proxy_conn;
	proxy_conn_ka == KeepAlive ? proxy_conn = "Keep-Alive" : proxy_conn = "Close";

	Send("HTTP/1.1 302 Moved\x0d\x0a");
	Send("Content-Length: 0\x0d\x0a");
	Send("Location: ");
	Send(url);
	Send("\x0d\x0a");
	if (conn_ka != None)
	{
		Send("Connection: ");
		Send(conn);
		Send("\x0d\x0a");
	}
	if (proxy_conn_ka != None)
	{
		Send("Proxy-Connection: ");
		Send(proxy_conn);
		Send("\x0d\x0a");
	}
	Send("\x0d\x0a");
}

bool HttpServer::ParsePostPacket (const string& param,
								  string& file, string& content, vector<pair<string, string> >& fields)
{
	size_t pos = param.find (" ");
	file = param.substr (0, pos);
	size_t pos_d = param.find ("\x0d\x0a\x0d\x0a");
	content = param.substr (pos_d + 4);
	pos = param.find ("\x0d\x0a", pos);
	while (pos != pos_d)
	{
		pos += 2;
		size_t pos_n = param.find (": ", pos);
		size_t pos_n2 = param.find ("\x0d\x0a", pos_n);
		char _buf[512], _buf2[512];
		strncpy (_buf, param.c_str()+pos, pos_n-pos); _buf[pos_n-pos] = 0;
		strncpy (_buf2, param.c_str()+pos_n + 2, pos_n2-pos_n-2); _buf2[pos_n2-pos_n-2] = 0;
		fields.push_back (make_pair(_buf, _buf2));
		pos = param.find ("\x0d\x0a", pos);
	}
	
	return true;
}

bool HttpServer::ReceivePostBase (const dataPoster::dataPoster_par& data, const string& auth_token)
{
	string command;
	string param;
	HttpServer::CMD_TYPE type;
	bool result = true;

	Receive(command, type, param);
	if (type != HttpServer::CMD_POST)
	{
		std::wcout<<"Received command is not POST\n";
		result = false;
	}

	string filename, content;
	char temp[20];
	vector<pair<string, string> > fields;
	ParsePostPacket (param,	filename,  content, fields);
	
	string uri;
	if (data.useProxy)
	{
		uri.append ("http://");
		uri.append (data.serverAddress.m_hostname.toAscii());
		if (data.serverAddress./*m_portNumber*/m_service != L"80")
		{
			uri.append(":");
			uri.append(/*itoa(*/data.serverAddress./*m_portNumber*/m_service.toAscii()/*, temp,10)*/);
		}
	}
	uri.append(data.full_filename);

	if (filename != uri)
	{
		std::wcout<<"Incorrect filename in request\n";
		result = false;
	}

	if ((content.size() != data.data.size()) || strncmp (content.c_str(), (char*)&data.data[0], content.size()))
	{
		std::wcout<<"Incorrect content in request\n";
		result = false;
	}

	if (find (fields.begin(), fields.end(), make_pair<string,string> ("Content-Length", itoa(data.data.size(), temp, 10))) == fields.end())
	{
		std::wcout<<"Wrong Content-Length field in request\n";
		result = false;
	}

	if (find (fields.begin(), fields.end(), make_pair<string,string> ("Host", data.serverAddress.m_hostname.toAscii())) == fields.end())
	{
		std::wcout<<"Wrong Host field in request\n";
		result = false;
	}

	if (find (fields.begin(), fields.end(), make_pair<string,string> ("User-Agent",data.userAgent)) == fields.end())
	{
		std::wcout<<"Wrong User-Agent field in request\n";
		result = false;
	}

	if (auth_token.empty())
	{
		if (param.find("Proxy-Authorization") != string::npos)
		{
			std::wcout<<"Authorization token found in request\n";
			result = false;
		}
	}
	else
	{
		if (param.find(auth_token) == string::npos)
		{
			std::wcout<<"Authorization token not found in request\n";
			result = false;
		}
	}

	return result;
}

bool HttpServer::ReceivePurePost (const dataPoster::dataPoster_par& data)
{
	return ReceivePostBase (data);
}

bool HttpServer::ReceivePostWithBasicToken (const dataPoster::dataPoster_par& data)
{
	return ReceivePostBase (data, "Proxy-Authorization: Basic");
}

bool HttpServer::ReceivePostWithNtlmToken (const dataPoster::dataPoster_par& data)
{
	return ReceivePostBase (data, "Proxy-Authorization: NTLM");
}