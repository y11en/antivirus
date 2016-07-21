#include <winsock2.h>

#define SERVER_SOCKET_ERROR 1
#define SERVER_SOCKET_OK 0
#define BUFFER_LENGTH 256
#define BLOCK_LENGTH 4096
#define SEPARATOR '-'

//#include <fstream.h>


SOCKET OpenServer(int iPort);
SOCKET WaitForClientConnection(SOCKET s);
SOCKET InitClientSocket(char *pHost, int iPort);

int RecvFrom(SOCKET s, char *pBuffer);
int SendTo(SOCKET s, char *pText);


