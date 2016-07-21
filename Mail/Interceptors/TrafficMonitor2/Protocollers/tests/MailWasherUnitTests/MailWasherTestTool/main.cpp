// PipeliningTest.cpp : Defines the entry point for the console application.
//
#define WIN32_LEAN_AND_MEAN		// Exclude rarely-used stuff from Windows headers
#include <stdio.h>
#include <tchar.h>

#include <string>

#include "Winsock2.h"
#include <string>
#include <iostream>
#

int _tmain(int argc, _TCHAR* argv[])
{
	std::cout<<"start\n";
	WSADATA wsaData;
	int iResult = WSAStartup(MAKEWORD(2,2), &wsaData);

	SOCKET cs = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

	sockaddr_in clientService; 
	clientService.sin_family = AF_INET;
	clientService.sin_addr.s_addr = inet_addr( "172.16.1.185" );
	clientService.sin_port = htons( 110 );

	int code;
	char rec_buf[65536];

	code = connect (cs, (SOCKADDR*) &clientService, sizeof(clientService));
	code = recv (cs, rec_buf, 65536,0);

	{

		std::string req ("user stas\r\n");
		code = send (cs, req.c_str(), req.size(), 0);
		std::cout<<req.c_str();
		do{	Sleep(100);
			code = recv (cs, rec_buf, 65536,0);
			if (code > 0)
			{
				rec_buf[code]=0;
				std::cout<<rec_buf;
			}
		} while (std::string(rec_buf).find("\r\n") == std::string::npos);
	}

	{

		std::string req ("pass ezf0\r\n");
		code = send (cs, req.c_str(), req.size(), 0);
		std::cout<<req.c_str();
		do{	Sleep(100);	
			code = recv (cs, rec_buf, 65536,0);
			if (code > 0)
			{
				rec_buf[code]=0;
				std::cout<<rec_buf;
			}
		} while (std::string(rec_buf).find("\r\n") == std::string::npos);
	}
/*
	{

		std::string req ("list\r\n");
		code = send (cs, req.c_str(), req.size(), 0);
		std::cout<<req.c_str();
		do{	Sleep(100);
			code = recv (cs, rec_buf, 65536,0);
			if (code > 0)
			{
				rec_buf[code]=0;
				std::cout<<rec_buf;
			}
		} while (std::string(rec_buf).find("\r\n.\r\n") == std::string::npos);
	}
*/
	{

		std::string req ("retr 1\r\n");
		code = send (cs, req.c_str(), req.size(), 0);
		std::cout<<req.c_str();
		do{	Sleep(100);
			code = recv (cs, rec_buf, 65536,0);
			if (code > 0)
			{
				rec_buf[code]=0;
				std::cout<<rec_buf;
			}
		} while (std::string(rec_buf).find("\r\n.\r\n") == std::string::npos);
	}
	

	Sleep(INFINITE);

	return 0;
}


