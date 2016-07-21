// PipeliningTest.cpp : Defines the entry point for the console application.
//
#define WIN32_LEAN_AND_MEAN		// Exclude rarely-used stuff from Windows headers
#include <stdio.h>
#include <tchar.h>

#include <string>

#include "Winsock2.h"
#include <string>
#include <iostream>

struct servStruct
{
	SOCKET serverSocket;
	SOCKET clientSocket;

	SOCKET main_servSock;
	SOCKADDR_IN serv_addr_in;
};

bool EstablishConnection (servStruct* ss)
{

	WSAEVENT hServerEvent = (WSAEVENT) CreateEvent (NULL, true, false, NULL);
	WSAEVENT hClientEvent = (WSAEVENT) CreateEvent (NULL, true, false, NULL);
	WSANETWORKEVENTS wsnServerEvents;
	WSANETWORKEVENTS wsnClientEvents;

	int code = 0;
	int iMode = 1;

	BOOL bOptVal = TRUE;
	int bOptLen = sizeof(BOOL);

	//
	SOCKET tempSock;
	tempSock = INVALID_SOCKET;

	if (ss->serverSocket != INVALID_SOCKET) closesocket(ss->serverSocket);
	if (ss->clientSocket != INVALID_SOCKET) closesocket(ss->clientSocket);

	tempSock = socket (AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (tempSock == INVALID_SOCKET) throw (code);

	code = setsockopt (tempSock, SOL_SOCKET, SO_REUSEADDR, (char*) &bOptVal, bOptLen);
	if (code!=0) throw (WSAGetLastError());

	code = ioctlsocket (tempSock, FIONBIO, (ULONG*) &iMode);
	if (code!=0) throw (code);

	code = connect (tempSock, (sockaddr*)&ss->serv_addr_in, sizeof(SOCKADDR_IN));

	code = WSAEventSelect (ss->main_servSock, hServerEvent, FD_ACCEPT);
	if (code!=0) throw (code);
	code = WSAEventSelect (tempSock, hClientEvent, FD_CONNECT);
	if (code!=0) throw (code);

	bool bConnected = false;
	bool bAccepted = false;

	while (!bConnected && !bAccepted)
	{
		code = WSAWaitForMultipleEvents (1,&hServerEvent,true,60000,false);
		if (code == WSA_WAIT_TIMEOUT) throw (code);
		if (code == WSA_WAIT_EVENT_0 && !bAccepted)
		{
			code = WSAEnumNetworkEvents(ss->main_servSock,hServerEvent,&wsnServerEvents);
			if (code!=0) throw (code);
			if (wsnServerEvents.iErrorCode[FD_ACCEPT_BIT]) throw (wsnServerEvents.iErrorCode);
			ss->serverSocket = accept (ss->main_servSock, NULL, NULL);
			bAccepted = true;
		}

		code = WSAWaitForMultipleEvents (1,&hClientEvent,true,60000,false);
		if (code == WSA_WAIT_TIMEOUT) throw (code);
		if (code == WSA_WAIT_EVENT_0 && !bConnected)
		{
			code = WSAEnumNetworkEvents(tempSock,hClientEvent,&wsnClientEvents);
			if (code!=0) throw (code);
			if (wsnClientEvents.iErrorCode[FD_CONNECT_BIT]) throw (wsnClientEvents.iErrorCode);
			else 
			{
				ss->clientSocket = tempSock;
				bConnected = true;
			}
		}
	}

	WSACloseEvent(hServerEvent);
	WSACloseEvent(hClientEvent);
	return 0;

}

int _tmain(int argc, _TCHAR* argv[])
{
	std::cout<<"start\n";
	WSADATA wsaData;
	int iResult = WSAStartup(MAKEWORD(2,2), &wsaData);

	servStruct ss;

	int code = 0;
	int iMode = 1;

	ZeroMemory(&ss.serv_addr_in, sizeof (SOCKADDR_IN));
	ss.serv_addr_in.sin_family = AF_INET;
	ss.serv_addr_in.sin_port = htons (80);
	ss.serv_addr_in.sin_addr.s_addr = inet_addr("172.16.1.185");

	ss.main_servSock = INVALID_SOCKET;

	ss.main_servSock = socket (AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (ss.main_servSock == INVALID_SOCKET) throw (code);

	BOOL bOptVal = TRUE;
	int bOptLen = sizeof(BOOL);

	code = setsockopt (ss.main_servSock ,SOL_SOCKET, SO_REUSEADDR, (char*) &bOptVal, bOptLen);
	if (code!=0) throw (WSAGetLastError());

	code = bind (ss.main_servSock, (SOCKADDR*) &ss.serv_addr_in, sizeof (SOCKADDR_IN));
	if (code!=0) throw (code);

	code = ioctlsocket (ss.main_servSock, FIONBIO, (ULONG*) &iMode);
	if (code!=0) throw (code);

	code = listen (ss.main_servSock, SOMAXCONN);
	if (code!=0) throw (code);


	ss.clientSocket = INVALID_SOCKET;
	ss.serverSocket = INVALID_SOCKET;

	EstablishConnection(&ss);

	WSAEVENT hEvent = (WSAEVENT) CreateEvent (NULL, true, false, NULL);

	code = WSAEventSelect (ss.clientSocket, hEvent, 0);
	if (code!=0) throw (code);
	code = WSAEventSelect (ss.serverSocket, hEvent, 0);
	if (code!=0) throw (code);

	WSACloseEvent(hEvent);

	iMode = 0;
	code = ioctlsocket (ss.clientSocket, FIONBIO, (ULONG*) &iMode);
	if (code!=0) throw (code);
	code = ioctlsocket (ss.serverSocket, FIONBIO, (ULONG*) &iMode);
	if (code!=0) throw (code);

//
	char rec_buf[65536];

	{

		std::string req ("\
POST /doMod/showPage.pl HTTP/1.1\r\n\
Accept: image/gif, image/x-xbitmap, image/jpeg, image/pjpeg, application/x-shockwave-flash, application/vnd.ms-excel, application/vnd.ms-powerpoint, application/msword\r\n\
Accept-Language: en-us\r\n\
Content-Type: application/x-www-form-urlencoded\r\n\
Accept-Encoding: gzip, deflate\r\n\
User-Agent: Mozilla/4.0 (compatible; MSIE 6.0; Windows NT 5.1; SV1; .NET CLR 1.1.4322; .NET CLR 2.0.50727)\r\n\
Host: kiryukhin-xp\r\n\
Content-Length: 7\r\n\
Connection: Keep-Alive\r\n\
Cache-Control: no-cache\r\n\r\n");
		
		code = send (ss.clientSocket, req.c_str(), req.size(), 0);
		std::cout<<req.c_str();
		do{	Sleep(100);
		code = recv (ss.serverSocket, rec_buf, 65536,0);
		if (code > 0)
		{
			rec_buf[code]=0;
			std::cout<<rec_buf;
		}
		} while (std::string(rec_buf) != req);
	}

	{

		std::string req ("HTTP/1.1 100 Continue\r\n\r\n");

		code = send (ss.serverSocket, req.c_str(), req.size(), 0);
		std::cout<<req.c_str();
		do{	Sleep(100);
		code = recv (ss.clientSocket, rec_buf, 65536,0);
		if (code > 0)
		{
			rec_buf[code]=0;
			std::cout<<rec_buf;
		}
		} while (std::string(rec_buf) != req);
	}



	{

		std::string req ("1234567");

		code = send (ss.clientSocket, req.c_str(), req.size(), 0);
		std::cout<<req.c_str();
		do{	Sleep(100);
		code = recv (ss.serverSocket, rec_buf, 65536,0);
		if (code > 0)
		{
			rec_buf[code]=0;
			std::cout<<rec_buf;
		}
		} while (std::string(rec_buf) != req);
	}

	
	{

		std::string req ("\
HTTP/1.1 200 OK\r\n\
Content-Length: 10\r\n\
\r\n\
0123456789");

		code = send (ss.serverSocket, req.c_str(), req.size(), 0);
		std::cout<<req.c_str();
		do{	Sleep(100);
		code = recv (ss.clientSocket, rec_buf, 65536,0);
		if (code > 0)
		{
			rec_buf[code]=0;
			std::cout<<rec_buf;
		}
		} while (std::string(rec_buf) != req);
	}

	

	Sleep(INFINITE);

	return 0;
}


