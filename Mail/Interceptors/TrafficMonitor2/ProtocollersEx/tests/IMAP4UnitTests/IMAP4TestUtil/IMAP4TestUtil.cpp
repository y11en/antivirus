// IMAP4TestUtil.cpp : Defines the entry point for the console application.
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
	clientService.sin_addr.s_addr = inet_addr( "172.16.10.105" );
	clientService.sin_port = htons( 143 );

	int code;
	char rec_buf[65536];

	code = connect (cs, (SOCKADDR*) &clientService, sizeof(clientService));

	code = recv (cs, rec_buf, 65536,0);

	std::string req ("l login kiryukhin01@testlab2k.avp.ru 123\r\n");

	code = send (cs, req.c_str(), req.size(), 0);
	code = recv (cs, rec_buf, 65536,0);

	std::string req2 ("s select inbox\r\n");

	code = send (cs, req2.c_str(), req2.size(), 0);
	code = recv (cs, rec_buf, 65536,0);

	std::string req3 ("f1 fetch 2 rfc822\r\nf2 fetch 2 rfc822\r\n");
	code = send (cs, req3.c_str(), req3.size(), 0);

	while(true)
	{
		code = recv (cs, rec_buf, 65536,0);
		std::cout<<std::string(rec_buf, rec_buf + code).c_str();
	}

	return 0;
}


