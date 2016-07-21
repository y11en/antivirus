#include "Socket.h"

#include <iostream>


Socket::Socket()
{
    create();
}

Socket::Socket(SOCKET socket)
 : m_socket(socket)
{
}

Socket::Socket(const unsigned short port)
{
    // failed to create socket
    if(!create() || !bind(port) || !listen())
    {
        close();
    }
}

Socket::~Socket()
{
    close();
}

SOCKET Socket::releaseSocket()
{
    SOCKET result = m_socket;
    m_socket = INVALID_SOCKET;
    return result;
}


Socket::operator SOCKET()
{
    return m_socket;
}

Socket::socketState Socket::select(bool read)
{
    fd_set readfds;
    FD_ZERO(&readfds);
    FD_SET(m_socket, &readfds);
    
    fd_set writefds;
    FD_ZERO(&writefds);
    FD_SET(m_socket, &readfds);
    
    fd_set exceptfds;
    FD_ZERO(&exceptfds);
    FD_SET(m_socket, &readfds);
    
    const int result = read
        ? ::select(m_socket + 1, &readfds, 0, &exceptfds, 0)
        : ::select(m_socket + 1, 0, &writefds, &exceptfds, 0);


    if(!result || result == SOCKET_ERROR || FD_ISSET(m_socket, &exceptfds))
        return exception;

    if(FD_ISSET(m_socket, &readfds))
        return readyRead;

    if(FD_ISSET(m_socket, &writefds))
        return readyWrite;

    return exception;   // to suppress compiler warning
}




bool Socket::create()
{
    m_socket = socket(AF_INET, SOCK_STREAM, 0);
    if(m_socket == INVALID_SOCKET)
        std::cerr << "Error: unable to create socket, error " << WSAGetLastError();

    return m_socket != INVALID_SOCKET;
}

bool Socket::bind(const unsigned short port)
{
    sockaddr_in service;
    service.sin_family = AF_INET;
    service.sin_addr.s_addr = inet_addr("127.0.0.1");
    service.sin_port = htons(port);
    if(::bind(m_socket, reinterpret_cast<const struct sockaddr*>(&service), sizeof service)  == SOCKET_ERROR)
    {
        std::cerr << "Error: unable to bind socket on port " << port << ", error " << WSAGetLastError();
        return false;
    }
    return true;
}

bool Socket::listen()
{
    if(::listen(m_socket, SOMAXCONN) == SOCKET_ERROR)
    {
        std::cerr << "Error: unable to listen on socket, error " << WSAGetLastError();
        return false;
    }
    return true;
}

void Socket::close()
{
    if(m_socket != INVALID_SOCKET)
    {
        closesocket(m_socket);
        m_socket = INVALID_SOCKET;
    }
}
