#include "TrafficTrackerServer.h"
#include "Configuration.h"

#include <iostream>
#include <sstream>

#include <process.h>

////////////////////////////////////////////////////////////
//// TrafficTrackerServer::Handler
TrafficTrackerServer::Handler::Handler(SOCKET client, const unsigned short port)
 : m_connectionToClient(client), m_port(port)
{
    m_logFile.open(logFileName().c_str());

    unsigned thrdaddr;
    _beginthreadex(0, 0, threadRunner, this, 0, &thrdaddr);
}

unsigned int TrafficTrackerServer::Handler::threadRunner(void *param)
{
    Handler *handler = reinterpret_cast<Handler *>(param);
    handler->threadFunction();
    delete handler;
    return 0;
}

TrafficTrackerServer::Handler::~Handler()
{
}
void TrafficTrackerServer::Handler::threadFunction()
{
    // log file failed to be opened
    if(!m_logFile)
    {
        std::cerr << "failed to open log file " << logFileName() << std::endl;
        return;
    }

    Socket connectionToServer = connectToServer();
    if(connectionToServer == INVALID_SOCKET)
        return;


    while(true)
    {
        fd_set readfds;
        FD_ZERO(&readfds);
        FD_SET(m_connectionToClient, &readfds);
        FD_SET(connectionToServer, &readfds);
    
        fd_set writefds;
        FD_ZERO(&writefds);
    
        fd_set exceptfds;
        FD_ZERO(&exceptfds);
        FD_ZERO(&connectionToServer);
        FD_SET(m_connectionToClient, &readfds);
    
        const int result = select(connectionToServer + 1, &readfds, 0, &exceptfds, 0);

        // error while reading from socket
        if(!result || result == SOCKET_ERROR
            || FD_ISSET(m_connectionToClient, &exceptfds) || FD_ISSET(connectionToServer, &exceptfds))
        {
            m_logFile << "select operation result " << result << ", error " << WSAGetLastError() << std::endl;
            return;
        }

        // forward data from client to server
        if(FD_ISSET(m_connectionToClient, &readfds)
            && !forwardData(m_connectionToClient, connectionToServer))
        {
            m_logFile << "connection closed from client side" << std::endl;
            return;
        }


        // forward data from server to client
        if(FD_ISSET(connectionToServer, &readfds)
            && !forwardData(connectionToServer, m_connectionToClient))
        {
            m_logFile << "connection closed from server side" << std::endl;
            return;
        }
    }
}

std::string TrafficTrackerServer::Handler::logFileName()const
{
    std::stringstream stream;
    stream << ntohs(m_port) << "_" << ConfigurationManager::instance().m_configuration.m_logFileName;
    return stream.str();
}

SOCKET TrafficTrackerServer::Handler::connectToServer()
{
    Socket connectionToServer;
    if(connectionToServer == INVALID_SOCKET)
        return INVALID_SOCKET;


    if(connect(connectionToServer, reinterpret_cast<const sockaddr *>(&ConfigurationManager::instance().m_configuration.m_serverAddress),
        sizeof(ConfigurationManager::instance().m_configuration.m_serverAddress)) == SOCKET_ERROR)
    {
        m_logFile << "can not connect to server, error " << WSAGetLastError() << std::endl;
        return INVALID_SOCKET;
    }

    const char *serverAddressName = inet_ntoa(ConfigurationManager::instance().m_configuration.m_serverAddress.sin_addr);
    m_logFile << "connected to server " << (serverAddressName ? serverAddressName : "")
        << ":" << ntohs(ConfigurationManager::instance().m_configuration.m_serverAddress.sin_port) << std::endl;

    return connectionToServer.releaseSocket();
}

bool TrafficTrackerServer::Handler::forwardData(Socket &input, Socket &output)
{
    const size_t bufferSize = 65536;
    char buffer[bufferSize];
    memset(buffer, 0, bufferSize);

    const int result = recv(input, buffer, bufferSize - 1, 0);
    
    // connection closed
    if(!result)
        return false;
        
    // error
    if(result == SOCKET_ERROR)
    {
        m_logFile << "recv() error " << WSAGetLastError() << std::endl;
        return false;
    }

    if(send(output, buffer, result, 0) != result)
    {
        m_logFile << "send() error " << WSAGetLastError() << std::endl;
        return false;
    }


    m_logFile << buffer << std::endl;
    return true;
}

////////////////////////////////////////////////////////////
//// TrafficTrackerServer::Handler
TrafficTrackerServer::TrafficTrackerServer()
{
}

TrafficTrackerServer::~TrafficTrackerServer()
{
}

void TrafficTrackerServer::run()
{
    // staring server on specified port
    Socket server(ConfigurationManager::instance().m_configuration.m_port);
    if(server == INVALID_SOCKET)
        return;

    while(true)
    {
        sockaddr clientAddress;
        int size = sizeof sockaddr;

        SOCKET client = accept(server, &clientAddress, &size);
        if(client == INVALID_SOCKET)
            continue;

        new Handler(client, reinterpret_cast<sockaddr_in *>(&clientAddress)->sin_port);
    }
}
