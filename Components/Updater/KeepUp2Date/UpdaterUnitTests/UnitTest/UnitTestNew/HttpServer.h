#ifndef TESTHTTPSERVER_H
#define TESTHTTPSERVER_H

#include "stdafx.h"

#include "Winsock2.h"
#include <string>
#include <vector>
#include <utility>

#include "util.h"
#include "net.h"

class HttpServer
{
public:

	enum CMD_TYPE
	{
		CMD_GET,
		CMD_HEAD,
		CMD_POST,
		CMD_NONE
	};

	enum ConnType
	{
		KeepAlive,
		Close,
		None
	};


	HttpServer (int control_port, const std::string& address, IP_VERSION ipversion = IP_V4);
	~HttpServer ();

	int Accept (int timeout = WSA_INFINITE);
	void Start ();
	void Stop ();
	void Send (const std::string& buffer);
	void Send (char* buffer, int length);
	void SendFile (const std::wstring& file, bool proxy = false, bool connection = true, bool proxy_connection = true);
	void SendFilePartly (const std::wstring& file, bool proxy = false, bool connection = true, bool proxy_connection = true);
	void Receive (std::string& buffer, CMD_TYPE& type, std::string& param);
	void CloseConnection ();
	void BreakConnection ();

	bool CheckConnectionClosed (int timeout = 20000);

	//service

	void SendNtlmGreeting (const ConnType& = KeepAlive);
	void SendNtlmBasicGreeting (const ConnType& = KeepAlive);
	void SendBasicGreeting (const ConnType& = KeepAlive);
	void SendNtlmMsg2 ();
	void SendFileFromBuffer (const ConnType& conn_ka, const ConnType& proxy_conn_ka, std::string message);
	void SendFileFromFile (const ConnType& conn_ka, const ConnType& proxy_conn_ka, std::wstring message);
	void Send403Forbidden (const ConnType& conn_ka, const ConnType& proxy_conn_ka);
	void Send404NotFound (const ConnType& conn_ka, const ConnType& proxy_conn_ka);
	void Send200OK (const ConnType& conn_ka, const ConnType& proxy_conn_ka, const string& text = string());
	void Send204OK (const ConnType& conn_ka, const ConnType& proxy_conn_ka);
	void Send502Denied (const ConnType& conn_ka, const ConnType& proxy_conn_ka);
	void Send302Redirected (const ConnType& conn_ka, const ConnType& proxy_conn_ka, const std::string& url);
	bool ReceiveGetWithNtlmToken (std::string custom = "");
	bool ReceiveGetWithBasicToken (std::string custom = "");
	bool ReceivePureGet (std::string custom = "");

	bool ParsePostPacket (const std::string& param, std::string& file, std::string& content,
			std::vector<std::pair<std::string, std::string> >& fields);
	bool ReceivePostBase (const dataPoster::dataPoster_par& data, const std::string& = std::string());
	bool ReceivePurePost (const dataPoster::dataPoster_par& data);
	bool ReceivePostWithBasicToken (const dataPoster::dataPoster_par& data);
	bool ReceivePostWithNtlmToken (const dataPoster::dataPoster_par& data);

	bool isIpV6 ()
	{
		return m_ipversion == IP_V6;
	};

private:

	IP_VERSION m_ipversion;

    SOCKET  m_mainSocket;
    SOCKET  m_socketWork;

	bool m_connected;

	std::string rcv_buf;

	int m_serverPort;
	std::string m_address;
	
	void DefineCommand (const std::string& command, CMD_TYPE& type);

};



#endif