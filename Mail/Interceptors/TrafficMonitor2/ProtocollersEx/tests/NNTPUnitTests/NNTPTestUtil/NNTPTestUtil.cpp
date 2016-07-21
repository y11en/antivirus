// NNTPTestUtil.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "winsock2.h"
#include <iostream>


int _tmain(int argc, _TCHAR* argv[])
{
	std::cout<<"start\n";
	WSADATA wsaData;
	int iResult = WSAStartup(MAKEWORD(2,2), &wsaData);

	SOCKET cs = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

	sockaddr_in clientService; 
	clientService.sin_family = AF_INET;
	clientService.sin_addr.s_addr = inet_addr( "172.16.4.190" );
	clientService.sin_port = htons( 119 );

	int code;
	char rec_buf[65536];

	code = connect (cs, (SOCKADDR*) &clientService, sizeof(clientService));

	code = recv (cs, rec_buf, 65536,0);

	std::string req ("group test1\r\n");

	code = send (cs, req.c_str(), req.size(), 0);
	code = recv (cs, rec_buf, 65536,0);

//	std::string req2 ("article 1\r\narticle 2\r\narticle 3\r\n");
	std::string req2 ("article 1\r\narticle 2\r\n");

	code = send (cs, req2.c_str(), req2.size(), 0);

	while(true)
	{
		code = recv (cs, rec_buf, 65536,0);
		std::cout<<std::string(rec_buf, rec_buf + code).c_str();
	}

	return 0;
}