// PipeliningTest.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "Winsock2.h"
#include <string>
#include <iostream>

int _tmain(int argc, _TCHAR* argv[])
{
	WSADATA wsaData;
	int iResult = WSAStartup(MAKEWORD(2,2), &wsaData);

	SOCKET cs = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

	sockaddr_in clientService; 
	clientService.sin_family = AF_INET;
	clientService.sin_addr.s_addr = inet_addr( "172.16.1.185" );
	clientService.sin_port = htons( 80 );

	int code;
	char rec_buf[65536];

	code = connect (cs, (SOCKADDR*) &clientService, sizeof(clientService));

	//string request (
	//"GET http://fake/fake.txt HTTP/1.1\015\012\015\012GET http://fake/fake.txt HTTP/1.1\015\012\015\012GET http://fake/fake.txt HTTP/1.1\015\012\015\012"
					//);

	std::string req ("GET /copy.txt HTTP/1.1\015\012Connection: Keep-Alive\015\012host: 172.16.1.185\015\012\015\012");
	std::string req2 ("GET /copy2.txt HTTP/1.1\015\012Connection: Keep-Alive\015\012host: 172.16.1.185\015\012\015\012");
	std::string req3 ("GET /copy3.txt HTTP/1.1\015\012Connection: Keep-Alive\015\012host: 172.16.1.185\015\012\015\012");
	std::string req4 ("GET /copy4.txt HTTP/1.1\015\012Connection: Keep-Alive\015\012host: 172.16.1.185\015\012\015\012");
	std::string req5 ("GET /copy5.txt HTTP/1.1\015\012Connection: Keep-Alive\015\012host: 172.16.1.185\015\012\015\012");

/*
	code = send (cs, req.c_str(), req.size(), 0);
	code = recv (cs, rec_buf, 65536,0);

	code = send (cs, req2.c_str(), req2.size(), 0);
	code = recv (cs, rec_buf, 65536,0);

	code = send (cs, req3.c_str(), req3.size(), 0);
	code = recv (cs, rec_buf, 65536,0);

	code = send (cs, req4.c_str(), req4.size(), 0);
	code = recv (cs, rec_buf, 65536,0);

	code = send (cs, req5.c_str(), req5.size(), 0);
	code = recv (cs, rec_buf, 65536,0);
*/
	std::string req_pl (std::string(req).append(req2).append(req3).append(req4).append(req5));
	code = send (cs, req_pl.c_str(), req_pl.size(), 0);

	std::cout<<"Bytes sent: "<<code<<"\n";
	std::cout<<req_pl;

while (true)
	{
		code = recv (cs, rec_buf, 65536,0);
		rec_buf[code] = 0;
		std::cout<<"Bytes received: "<<code<<"\n";
		std::cout<<rec_buf;
	}

	return 0;
}


