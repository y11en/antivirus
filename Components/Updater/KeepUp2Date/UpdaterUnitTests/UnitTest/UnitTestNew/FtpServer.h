#ifndef TESTFTPSERVER_H
#define TESTFTPSERVER_H

#include "stdafx.h"

#include "Winsock2.h"
#include "ws2tcpip.h"
#include <string>

#include "net.h"

class FtpServer
{
public:
	enum CMD_TYPE
	{
		CMD_USER,
		CMD_PASS,
		CMD_TTYPE,
		CMD_PASV, CMD_EPSV,
		CMD_PORT, CMD_EPRT,
		CMD_RETR,
		CMD_REST,
		CMD_QUIT,
		CMD_NONE
	};


	FtpServer (int control_port, const std::string& address, IP_VERSION ipver = IP_V4);
	~FtpServer ();

	void Accept ();
	void Start ();
	void Stop ();
	void Send (const std::string& buffer);
	void Receive (std::string& buffer, CMD_TYPE& type, std::string& param);

	void CreateBindListeningPassiveDataPort_impl (u_short& result);
	void CreateBindListeningPassiveDataPort_impl (SOCKADDR_IN& result);
	void CreateBindListeningPassiveDataPort (std::string& address);
	void CreateBindListeningPassiveDataPortEx (std::string& address);

	void StartListeningPassiveDataPortOnly ();
	void StartListeningPassiveDataPort (std::string& address);
	void AcceptDataConnection ();
	void SendData (const std::string& buffer);
	void CloseDataConnection ();
	void CloseControlConnection ();

	void EstablishActiveConnection (const std::string& param);
	void EstablishActiveConnectionEx (const std::string& param);
	void EstablishActiveConnection_impl (const std::string& host, const int& port);

	bool CheckDataConnectionClosed (int timeout = 20000);
	bool CheckControlConnectionClosed (int timeout = 20000);

	void CloseDataConnectionImpolite ();
	void CloseControlConnectionImpolite ();

	void Hello ();
	bool ProcessTypeCommand (std::wstring& result);
	bool Authorize (const std::string& user, const std::string& pass, std::wstring& result);
	//τΰιλ
	bool SendFileFromFile (const std::string& filename, const std::wstring& filebuf, const bool active_mode, std::wstring& result);
	//αστεπ
	bool SendFileFromBuffer (const std::string& filename, const std::string& filebuf,	const bool active_mode, std::wstring& result);
	//
	bool SendFileNotFound (const std::string& filename, const bool active_mode, std::wstring& result);

	bool isIpV6 ()
	{
		return m_ipver == IP_V6;
	};

private:
	//ip version
	IP_VERSION m_ipver;
	// FTP control connection socket
    SOCKET  m_controlSocket;
    // FTP data connection socket
    SOCKET  m_dataSocket;
	// work control socket
	SOCKET  m_controlSocketWork;
	//work data connection socket
	SOCKET  m_dataSocketWork;

	bool m_connected;
	bool m_data_connected;

	std::string rcv_buf;

	int m_serverPort;
	std::string m_address;
	
	void DefineCommand (const std::string& command, CMD_TYPE& type);

};



#endif