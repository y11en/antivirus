#include "NetworkInitializer.h"

#include <iostream>

#include <winsock2.h>

NetworkInitializer::NetworkInitializer()
{
    WSADATA wsaData;
    m_initialized = WSAStartup(MAKEWORD( 2, 2 ), &wsaData) == 0;

    if(!m_initialized)
        std::cerr << "Error: WSAStartup() failed" << std::cout;
}

NetworkInitializer::~NetworkInitializer()
{
    WSACleanup();
}

bool NetworkInitializer::initialized()const
{
    return m_initialized;
}
