#include "serverprotocol.h"
///////////////////////////////////////////////////////////////
//	
///////////////////////////////////////////////////////////////
SOCKET OpenServer(int iPort)
{
    WORD sockVersion;
    WSADATA wsaData;
    int rVal;

    sockVersion = MAKEWORD(2,0);
    WSAStartup(sockVersion, &wsaData);
	
	SOCKET s = WSASocket (AF_INET, SOCK_STREAM, 0, 0, 0, 0);

    if(s == INVALID_SOCKET){
        WSACleanup();
        return SERVER_SOCKET_ERROR;
    }

    SOCKADDR_IN sin;
    sin.sin_family = PF_INET;
    sin.sin_port = htons(iPort);
    sin.sin_addr.s_addr = INADDR_ANY;

    rVal = bind(s, (LPSOCKADDR)&sin, sizeof(sin));
    if(rVal == SOCKET_ERROR){
        WSACleanup();
        return SERVER_SOCKET_ERROR;
    }

    rVal = listen(s, 1);
    if(rVal == SOCKET_ERROR){
        WSACleanup();
        return SERVER_SOCKET_ERROR;
    }

	return s;
}


///////////////////////////////////////////////////////////////
//	
///////////////////////////////////////////////////////////////
SOCKET WaitForClientConnection(SOCKET s){
	SOCKET client;
    client = accept(s, NULL, NULL);

    if(client == INVALID_SOCKET){
        WSACleanup();
        return SERVER_SOCKET_ERROR;
    }
    return client;
}
///////////////////////////////////////////////////////////////
//	
///////////////////////////////////////////////////////////////
int RecvFrom(SOCKET s, char *pBuffer)
{
	char *tmpBuffer = new char[BUFFER_LENGTH];
	int res;

	*pBuffer = '\0';

	do{
		res=recv(s,tmpBuffer,sizeof(tmpBuffer),0);
		
		if (tmpBuffer[0] == '\0') return SERVER_SOCKET_ERROR;

		if (res>=0)	*(tmpBuffer+res)='\0';
		lstrcat(pBuffer,tmpBuffer);
	}while(*(tmpBuffer+res-1) != '\n');

	return SERVER_SOCKET_OK;
}
///////////////////////////////////////////////////////////////
//	
///////////////////////////////////////////////////////////////
int SendTo(SOCKET s, char *pText)
{
	return send(s, pText,lstrlen(pText),0);
}
///////////////////////////////////////////////////////////////
//	
///////////////////////////////////////////////////////////////
SOCKET InitClientSocket(char *pHost, int iPort)
{
	struct sockaddr_in saClient;
	struct hostent *pHostinfo;
	SOCKET sock;

	WORD version;
	WSADATA WSAData;
	version=MAKEWORD(1,1);
	WSAStartup(version, &WSAData);

	if ((sock=socket(AF_INET,SOCK_STREAM,0))==SOCKET_ERROR){
		return 0;
	}

	pHostinfo=gethostbyname(pHost);
	if (pHostinfo==NULL){
		return 0;
	}

	saClient.sin_family=AF_INET;
	saClient.sin_addr=*((struct in_addr *)pHostinfo->h_addr);
	saClient.sin_port=htons(iPort);

	if (connect(sock,(struct sockaddr *)&saClient, sizeof(saClient))){
		return 0;
	}
	return sock;
}






