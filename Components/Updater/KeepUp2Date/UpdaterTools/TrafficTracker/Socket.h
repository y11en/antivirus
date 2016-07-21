#if !defined(AFX_SOCKET_H__2BCF3127_930E_4B30_92BD_EEC9BB633C02__INCLUDED_)
#define AFX_SOCKET_H__2BCF3127_930E_4B30_92BD_EEC9BB633C02__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


#include <winsock2.h>


/// this class simplifies socket usage
class Socket
{
public:
    enum socketState
    {
        // socket is ready for read operation
        readyRead,
        // socket is write operation
        readyWrite,
        // exception happened on socket
        exception
    };

    /// creates and initializes socket
	Socket();
    /// creates socket from SOCKET
	Socket(SOCKET);
    /// 1. create socket
    /// 2. binds socket on specified port number
    /// 3. put socket in listen state
	Socket(const unsigned short port);

    /// automatically closes socket
	virtual ~Socket();

    /// releases socket from control of Socket, thus socket will not be automatically closed
    SOCKET releaseSocket();

    /// for convenience type conversion
    operator SOCKET();

    /// block execution thread until socket becomes in signaling state
    socketState select(bool read);

    SOCKET m_socket;



private:
    // creates socket
    bool create();
    // binds socket on specified port
    bool bind(const unsigned short port);
    // places a socket in a state in which it is listening for an incoming connection
    bool listen();
    // closes socket
    void close();

    // disabling copy operations, because closesocket() will be called
    Socket(const Socket &);
    Socket &operator=(const Socket &);

};

#endif // !defined(AFX_SOCKET_H__2BCF3127_930E_4B30_92BD_EEC9BB633C02__INCLUDED_)
