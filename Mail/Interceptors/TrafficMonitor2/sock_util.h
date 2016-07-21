#ifndef _SOCKET_UTILITIES_H__
#define _SOCKET_UTILITIES_H__

#include <ProductCore/structs/s_ip.h>

int
SockInitialize();

void
SockDone();

void
SetBlocking( SOCKET sock, bool block );

bool 
CheckPort(unsigned short nPort);

bool 
ScanPorts(USHORT nStartPort, USHORT nRange, USHORT &nResultPort);

cIP 
GetServerIP(const char* szServerName);

SOCKET
ServerSocket( unsigned short Port, int Family );

int
SendAndTrace( SOCKET sock, const char* buf, int size, bool to_client );

bool
GetLocalName( SOCKET sock, char* AddrName, int AddrNameSize );

char*
CR_GetParamStr( char* DstStr, char* SrcStr, ULONG Index);

#define DWAIT_FAILED        -1
#define DWAIT_OBJECT_0      10
#define DWAIT_TIMEOUT       1

#define CLOSESOCKET(_socket)	        \
{	                                    \
    if ( _socket != INVALID_SOCKET )	\
    {	                                \
        shutdown (_socket, 2 );	        \
        LINGER linger = { 1, 0 };	    \
        setsockopt(_socket, SOL_SOCKET, SO_LINGER, (char*) & linger, sizeof (linger));	\
        /*setsockopt(_socket, SOL_SOCKET, SO_UPDATE_CONNECT_CONTEXT, NULL, 0 );*/	    \
        closesocket(_socket);	        \
    }	                                \
    _socket = INVALID_SOCKET;	        \
}

/* from MSWSockDef.h */
#define SIO_SET_COMPATIBILITY_MODE  _WSAIOW(IOC_VENDOR,300)

typedef enum _WSA_COMPATIBILITY_BEHAVIOR_ID {
    WsaBehaviorAll = 0,
    WsaBehaviorReceiveBuffering,
    WsaBehaviorAutoTuning
} WSA_COMPATIBILITY_BEHAVIOR_ID, *PWSA_COMPATIBILITY_BEHAVIOR_ID;

/* from sdkddkver.h */
#define NTDDI_LONGHORN                      0x06000000

typedef struct _WSA_COMPATIBILITY_MODE {
    WSA_COMPATIBILITY_BEHAVIOR_ID BehaviorId;
    ULONG TargetOsVersion;
} WSA_COMPATIBILITY_MODE, *PWSA_COMPATIBILITY_MODE;

#endif