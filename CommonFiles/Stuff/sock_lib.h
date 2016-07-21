#ifndef __sock_lib_h
#define __sock_lib_h

/*
#define INCL_WINSOCK_API_TYPEDEFS 1
#include <winsock2.h>
*/

#include <winsock.h>

#if defined( __cplusplus )
extern "C" {
#endif

#define WSAAPI FAR PASCAL

typedef int (WSAAPI * LPFN_WSASTARTUP)(WORD wVersionRequested,LPWSADATA lpWSAData);
typedef int (WSAAPI * LPFN_WSACLEANUP)(void);
typedef int (WSAAPI * LPFN_WSAGETLASTERROR)(void);
typedef struct hostent FAR * (WSAAPI * LPFN_GETHOSTBYNAME)(const char FAR *name);
typedef struct hostent FAR * (WSAAPI * LPFN_GETHOSTBYADDR)(const char FAR *addr, int len, int type);
typedef u_short (WSAAPI * LPFN_HTONS)(u_short hostshort);
typedef u_long (WSAAPI * LPFN_HTONL)(u_long hostlong);
typedef u_short (WSAAPI * LPFN_NTOHS)(u_short netshort);
typedef u_long (WSAAPI * LPFN_NTOHL)(u_long netlong);
typedef int (WSAAPI * LPFN_SHUTDOWN)(SOCKET s,int how);
typedef int (WSAAPI * LPFN_CLOSESOCKET)(SOCKET s);
typedef SOCKET (WSAAPI * LPFN_ACCEPT)(SOCKET s,struct sockaddr FAR * addr,int FAR * addrlen);
typedef int (WSAAPI * LPFN_LISTEN)(SOCKET s,int backlog);
typedef int (WSAAPI * LPFN_BIND)(SOCKET s,const struct sockaddr FAR * name,int namelen);
typedef int (WSAAPI * LPFN_RECV)(SOCKET s,char FAR * buf,int len,int flags);
typedef unsigned long (WSAAPI * LPFN_INET_ADDR)(const char FAR * cp);
typedef int (WSAAPI * LPFN_SELECT)(int nfds,fd_set FAR * readfds,fd_set FAR * writefds,fd_set FAR *exceptfds,const struct timeval FAR * timeout);
typedef int (WSAAPI * LPFN_SEND)(SOCKET s,const char FAR * buf,int len,int flags);
typedef int (WSAAPI * LPFN_CONNECT)(SOCKET s,const struct sockaddr FAR * name,int namelen);
typedef int (WSAAPI * LPFN_SETSOCKOPT)(SOCKET s,int level,int optname,const char FAR * optval,int optlen);
typedef SOCKET (WSAAPI * LPFN_SOCKET)(int af,int type,int protocol);
typedef int (WSAAPI * LPFN_GETHOSTNAME)(char FAR * name,int namelen);
typedef int (WSAAPI * LPFN_GETSOCKNAME)(SOCKET s,struct sockaddr FAR * name,int FAR * namelen);
typedef int (WSAAPI * LPFN_GETPEERNAME)(SOCKET s,struct sockaddr FAR * name,int FAR * namelen);
typedef int (WSAAPI * LPFN_SETSOCKOPT)(SOCKET s,int level,int optname,const char FAR * optval,int optlen);
typedef int (WSAAPI * LPFN_GETSOCKOPT)(SOCKET s,int level,int optname,char FAR * optval,int FAR * optlen);


extern LPFN_WSASTARTUP      pfWSAStartup;
extern LPFN_WSACLEANUP      pfWSACleanup;
extern LPFN_WSAGETLASTERROR pfWSAGetLastError;
extern LPFN_GETHOSTBYNAME   pfgethostbyname;
extern LPFN_GETHOSTBYADDR   pfgethostbyaddr;
extern LPFN_HTONS           pfhtons;
extern LPFN_HTONL           pfhtonl;
extern LPFN_NTOHS           pfntohs;
extern LPFN_NTOHL           pfntohl;
extern LPFN_SHUTDOWN        pfshutdown;
extern LPFN_CLOSESOCKET     pfclosesocket;
extern LPFN_ACCEPT          pfaccept;
extern LPFN_LISTEN          pflisten;
extern LPFN_BIND            pfbind;
extern LPFN_RECV            pfrecv;
extern LPFN_INET_ADDR       pfinet_addr;
extern LPFN_SELECT          pfselect;
extern LPFN_SEND            pfsend;
extern LPFN_CONNECT         pfconnect;
extern LPFN_SOCKET          pfsocket;
extern LPFN_GETHOSTNAME     pfgethostname;
extern LPFN_GETSOCKNAME     pfgetsockname;
extern LPFN_GETPEERNAME     pfgetpeername;
extern LPFN_SETSOCKOPT      pfsetsockopt;
extern LPFN_GETSOCKOPT      pfgetsockopt;

int   sock_InitLib( void );
void  sock_TermLib( void );
int   sock_CheckError( const char** str, int* error );

int   sock_SendBytes( SOCKET s, char* pBuf, int cbBuf, int timeout );
int   sock_SendMsg( SOCKET s, char* pBuf, int cbBuf, int timeout );
int   sock_SendCmd( SOCKET s, int command, char* pBuf, int cbBuf, int timeout );

int   sock_MsgLen( int cbBuf );
int   sock_CmdLen( int cbBuf );

int   sock_ReceiveBytes( SOCKET s, char* pBuf, int cbBuf, int timeout );
int   sock_ReceiveMsg( SOCKET s, char* pBuf, int cbBuf, int* rest, int timeout );
int   sock_ReceiveCmd( SOCKET s, int* command, char* pBuf, int cbBuf, int* rest, int timeout );

int   sock_ReceiveWaiting( SOCKET s, int timeout );
int   sock_SendWaiting( SOCKET s, int timeout );

int   sock_MsgContLen( int cbBuf );
int   sock_CmdContLen( int cbBuf );

int   sock_CreateListen( unsigned short port, SOCKET* socket );
int   sock_FillAddr( struct sockaddr_in* addr, const char* host_name, short port );
int   sock_FillIPAddr( struct sockaddr_in* addr, int b1, int b2, int b3, int b4, short port );

int   sock_ConnectTo( const char* host_name, short port, SOCKET* socket );
int   sock_ConnectToAddr( int b1, int b2, int b3, int b4, short port, SOCKET* socket );
int   sock_GetPeerName( SOCKET s, char* pBuf, int cbBuf );

#if defined( __cplusplus )
}
#endif

#define SOCKLIB_E_BASE                       -2000

#define SOCKLIB_E_OK                         0
#define SOCKLIB_E_CANNOT_LOAD_WSOCK          (SOCKLIB_E_BASE - 1)
#define SOCKLIB_E_BAD_WSOCK                  (SOCKLIB_E_BASE - 2)
#define SOCKLIB_E_CANNOT_SEND                (SOCKLIB_E_BASE - 3)
#define SOCKLIB_E_CANNOT_RECEIVE             (SOCKLIB_E_BASE - 4)
#define SOCKLIB_E_TIMEOUT                    (SOCKLIB_E_BASE - 5)
#define SOCKLIB_E_HOST_CANT_PROCESS_DATA     (SOCKLIB_E_BASE - 6)
#define SOCKLIB_E_HOST_BUSY                  (SOCKLIB_E_BASE - 7)
#define SOCKLIB_E_HOST_CANT_GET_ENOUGH_DATA  (SOCKLIB_E_BASE - 8)
#define SOCKLIB_E_HOST_ERROR                 (SOCKLIB_E_BASE - 9)
#define SOCKLIB_E_BAD_PARAMETER              (SOCKLIB_E_BASE - 10)
#define SOCKLIB_E_CREATE_SOCKET              (SOCKLIB_E_BASE - 11)
#define SOCKLIB_E_BIND_FAILED                (SOCKLIB_E_BASE - 12)
#define SOCKLIB_E_LISTEN_FAILED              (SOCKLIB_E_BASE - 13)
#define SOCKLIB_E_HOST_NOT_FOUND             (SOCKLIB_E_BASE - 14)
#define SOCKLIB_E_INVALID_PARAMETER          (SOCKLIB_E_BASE - 15)
#define SOCKLIB_E_SET_SOCK_OPT               (SOCKLIB_E_BASE - 16)
#define SOCKLIB_E_CONNECT                    (SOCKLIB_E_BASE - 17)

#endif
