#include "../helper/updaterStaff.h"

#include "stdSocket.h"

KLUPD::Socket::Initializer::Initializer() : bInitialized(false)
{
#ifdef WIN32
    WORD wVersionRequested;
    WSADATA wsaData;

    wVersionRequested = MAKEWORD(2, 0);
    bInitialized = WSAStartup(wVersionRequested, &wsaData) == 0;
#else
    bInitialized = true;
#endif
}
KLUPD::Socket::Initializer::~Initializer()
{
#ifdef WIN32
    if(bInitialized)
        WSACleanup();
#endif
}

KLUPD::Socket::Socket(int nTimeoutSecs, DownloadProgress &downloadProgress, Log *log)
 : m_socket(INVALID_SOCKET),
   m_nPF(PF_UNSPEC),
   m_nTimeout(nTimeoutSecs*1000),
   m_downloadProgress(downloadProgress),
   pLog(log)
{
}

KLUPD::Socket::~Socket()
{
    close();
}

bool KLUPD::Socket::create(addrinfo* pAddrInfoList, int nPF)
{
    if(m_socket != INVALID_SOCKET)
    {
        TRACE_MESSAGE("Failed to create socket: already in use");
        return false;
    }

    if( nPF != PF_UNSPEC && !_IsProtoFamilySupported(nPF) )
    {
        TRACE_MESSAGE2("Failed to create socket: protocol family %d is not supported", nPF);
        return false;
    }
    
    for(addrinfo *AI = pAddrInfoList; AI != NULL; AI = AI->ai_next)
    {
        if( nPF == PF_UNSPEC && _IsProtoFamilySupported(AI->ai_family) ||
            AI->ai_family == nPF )
        {
            m_socket = socket(AI->ai_family, SOCK_STREAM, 0);
            if( m_socket != INVALID_SOCKET )
            {
                m_nPF = AI->ai_family;
                break;
            }
        }
    }


    if(m_socket == INVALID_SOCKET)
    {
        const int lastError = WSAGetLastError();
        TRACE_MESSAGE2("Failed to create socket, last error %S", errnoToString(lastError, windowsStyle).toWideChar());
        return false;
    }
    
    if( !makeNonBlocking() )
    {
        TRACE_MESSAGE("Can't set socket options.");
        return false;
    }

    return true;
}

void KLUPD::Socket::close()
{
	if(m_socket == INVALID_SOCKET)
        return;

	closeSocketCrossPlatform(m_socket);

    m_socket = INVALID_SOCKET;
    m_nPF = PF_UNSPEC;
}

bool KLUPD::Socket::accept(Socket &resultSocket)
{
    resultSocket.close();
	if(m_socket == INVALID_SOCKET)
        return false;

    m_downloadProgress.updateSpeedStartTimer();
    resultSocket.m_socket = ::accept(m_socket, 0, 0);
    m_downloadProgress.updateSpeedStopTimer();

	if(resultSocket.m_socket == INVALID_SOCKET)
    {
        const int lastError = WSAGetLastError();
        TRACE_MESSAGE2("Failed to accept connection: accept failed, last error %S",
            errnoToString(lastError, windowsStyle).toWideChar());
        return false;
    }

    if(!makeNonBlocking())
    {
        TRACE_MESSAGE("Failed to accept connection: unbale to make socket non-blocking");
        resultSocket.close();
        return false;
    }
    return true;
}

bool KLUPD::Socket::bind(const char* szHost, const char* szService, int nPF)
{
    if(m_socket != INVALID_SOCKET)
    {
        TRACE_MESSAGE3("Failed to bind socket to \'%s:%s\': already in use", szHost, szService);
        return false;
    }

    addrinfo *aiList = NULL;
    if(::getaddrinfo(szHost, szService, NULL, &aiList) != 0)
    {
        TRACE_MESSAGE3("Failed to resolve address for \'%s:%s\'", szHost, szService);
        return false;
    }

    bool bSuccess = false;

    if( bSuccess = create(aiList, nPF) )
    {
        // loop until binded
        for(addrinfo *AI = aiList; AI != NULL; AI = AI->ai_next)
            if( AI->ai_family == m_nPF )
                if(bSuccess = ::bind(m_socket, AI->ai_addr, AI->ai_addrlen) == 0)
                    break;
    }

    ::freeaddrinfo(aiList);

    return bSuccess;
}

bool KLUPD::Socket::getSockName(std::string &strHost, std::string& strService, int nFlags)
{
    if(m_socket == INVALID_SOCKET)
    {
        TRACE_MESSAGE("Failed to get socket name: socket is not opened");
        return false;
    }

#ifdef _WIN32
    SOCKADDR_STORAGE sockinfo = {0};
#else
	struct sockaddr_storage sockinfo = {0};
#endif
    socklen_t sockinfosize = sizeof(sockinfo);
    if(::getsockname(m_socket, reinterpret_cast<sockaddr*>(&sockinfo), &sockinfosize) != 0)
    {
        const int lastError = WSAGetLastError();
        TRACE_MESSAGE2("Failed to get socket name, last error %S",
            errnoToString(lastError, windowsStyle).toWideChar());
        return false;
    }

    char szHost[NI_MAXHOST];
    char szService[NI_MAXSERV];
    if( getnameinfo(reinterpret_cast<sockaddr*>(&sockinfo), sockinfosize, szHost, NI_MAXHOST, szService, NI_MAXSERV, nFlags) != 0 )
    {
        TRACE_MESSAGE("Failed to get socket name");
        return false;
    }

    strHost = szHost;
    strService = szService;

    return true;
}

bool KLUPD::Socket::listen()
{
    if(m_socket == INVALID_SOCKET)
        return false;
	
    // for updater purpose only single connection is needed
    const int backlogSingleConnection = 1;
    return ::listen(m_socket, backlogSingleConnection) == 0;
}

bool KLUPD::Socket::wouldBlockError(const int lastError)const
{
#ifdef WIN32
    return lastError == WSAEWOULDBLOCK;
#else
    return lastError == EAGAIN
        || lastError == EINTR
        || lastError == EINPROGRESS;
#endif
}

bool KLUPD::Socket::isConnectionBroken(const int lastError)const
{
#ifdef WIN32
    return lastError == WSAENOTCONN
        || lastError == WSAEINTR
        || lastError == WSAENETRESET
        || lastError == WSAESHUTDOWN
        || lastError == WSAECONNABORTED
        || lastError == WSAETIMEDOUT
        || lastError == WSAECONNRESET;
#else
    return lastError ==  ENOTCONN
        || lastError == ECONNRESET
        || lastError == EINTR;
#endif
}


int KLUPD::Socket::wait1Second(const bool write)
{
    fd_set Set, ErrSet;
    FD_ZERO(&Set);
    FD_ZERO(&ErrSet);
    FD_SET(static_cast<unsigned int>(m_socket), &Set);
    FD_SET(static_cast<unsigned int>(m_socket), &ErrSet);

    struct timeval tv;
    tv.tv_sec = 1;
    tv.tv_usec = 0;


    const int selectResult = (write == true)
        ? ::select(m_socket + 1, 0, &Set, &ErrSet, &tv)
        : ::select(m_socket + 1, &Set, 0, &ErrSet, &tv);

    // timeout expired
    if(!selectResult)
        return selectResult;

    // error
    const int lastError = WSAGetLastError();
    if( selectResult == -1 )
    {
        TRACE_MESSAGE2("Select failed (select result), last error %S",
            errnoToString(lastError, windowsStyle).toWideChar());
        return -1;
    }
    else if( FD_ISSET(m_socket, &ErrSet) )
    {
        int nSockErr;
        socklen_t nSizeSockErr = sizeof(nSockErr);
        ::getsockopt(m_socket, SOL_SOCKET, SO_ERROR, reinterpret_cast<char*>(&nSockErr), &nSizeSockErr);
        TRACE_MESSAGE2("Select failed (select result), last error %S",
            errnoToString(nSockErr, windowsStyle).toWideChar());
        return -1;
    }

    // success
    if(FD_ISSET(m_socket, &Set))
        return 1;

    // unknown error
    TRACE_MESSAGE2("Select failed, last error %S",
        errnoToString(lastError, windowsStyle).toWideChar());
    return -1;
}

KLUPD::CoreError KLUPD::Socket::select(const bool write)
{
    const int nTimeoutSeconds = m_nTimeout/1000;
    for(size_t secondsPassed = 0; secondsPassed <= nTimeoutSeconds; ++secondsPassed)
    {
        if(m_downloadProgress.checkCancel())
            return CORE_CANCELLED;

        m_downloadProgress.updateSpeedStartTimer();
        const signed int waitResult = wait1Second(write);
        m_downloadProgress.updateSpeedStopTimer();

        if(waitResult > 0)
            return CORE_NO_ERROR;
        if(waitResult < 0)
            return CORE_DOWNLOAD_ERROR;
    }
    
    TRACE_MESSAGE2("Timeout, waited %d seconds", nTimeoutSeconds);
    return CORE_DOWNLOAD_TIMEOUT;
}

int KLUPD::Socket::recv(char *buff, const int bufflen, CoreError &result)
{
    result = CORE_DOWNLOAD_ERROR;
    if(!buff || bufflen < 1)
    {
        TRACE_MESSAGE("Failed to receive data from socket: incorrect arguments buffer size for message is 0");
        return 0;
    }
    if(m_socket == INVALID_SOCKET)
    {
        TRACE_MESSAGE("Failed to receive data from socket: socket is already closed" );
        return 0;
    }

    result = select(false);
    if(result != CORE_NO_ERROR)
        return result;
    
    const signed int bytesRecveived = ::recv(m_socket, buff, bufflen, 0);
    const int lastError = WSAGetLastError();
    if(!bytesRecveived)
    {
        result = CORE_REMOTE_HOST_CLOSED_CONNECTION;
        return 0;
    }

    // succeed
    if(0 < bytesRecveived)
    {
        m_downloadProgress.bytesTransferred(bytesRecveived);
        return bytesRecveived;
    }

    TRACE_MESSAGE2("Failed to receive data from socket, last error %S",
        errnoToString(lastError, windowsStyle).toWideChar());

    result = isConnectionBroken(lastError) ? CORE_REMOTE_HOST_CLOSED_CONNECTION : CORE_DOWNLOAD_ERROR;
    return 0;
}

KLUPD::CoreError KLUPD::Socket::connect(const char* szHost, const char* szService, const bool throughProxy, int nPF)
{
    addrinfo *aiList = NULL;
    if(::getaddrinfo(szHost, szService, NULL, &aiList) != 0)
    {
        TRACE_MESSAGE3("Failed to resolve address for \'%s:%s\'", szHost, szService);
        return CORE_DOWNLOAD_ERROR;
    }

    int nWSAError = 0;

    bool bKeepOpen = m_socket != INVALID_SOCKET;   // indicates that socket must be kept open in any case
    bool bSuccess = false;
    if( bKeepOpen || create(aiList, nPF) )
    {
        for(addrinfo *AI = aiList; AI != NULL; AI = AI->ai_next)    // loop until connected
            if( AI->ai_family == m_nPF )
                if( bSuccess = ::connect(m_socket, AI->ai_addr, AI->ai_addrlen) == 0 ||
                    wouldBlockError(nWSAError = WSAGetLastError()) )
                        break;
    }

    ::freeaddrinfo(aiList);

    CoreError err = CORE_NO_ERROR;
    if(!bSuccess)
    {
        TRACE_MESSAGE2("Could not connect to host, last error %S",
            errnoToString(nWSAError, windowsStyle).toWideChar());

        err = throughProxy ? CORE_CANT_CONNECT_TO_PROXY : CORE_CANT_CONNECT_INET_SERVER;
    }

    if(err == CORE_NO_ERROR)
    {
        err = select(true);
        if(err != CORE_NO_ERROR)
        {
            const int lastError = WSAGetLastError();
            TRACE_MESSAGE5("Failed to connect to '%s:%s', select result '%S', last error %S",
                szHost, szService, toString(err).toWideChar(), errnoToString(lastError, windowsStyle).toWideChar());

            if(isNetworkError(err))
                err = throughProxy ? CORE_CANT_CONNECT_TO_PROXY : CORE_CANT_CONNECT_INET_SERVER;
        }
        else
            TRACE_MESSAGE3("Connected successfully to '%s:%s'", szHost, szService);
    }

    if(err != CORE_NO_ERROR && !bKeepOpen)  // close socket on connection fail
        close();

    return err;
}

KLUPD::CoreError KLUPD::Socket::send(const char *buff, const int bufflen)
{
    if(m_socket <= 0)
    {
        TRACE_MESSAGE("Unable to send, socket already closed");
        return CORE_DOWNLOAD_ERROR;
    }

    const CoreError selectResult = select(true);
    const int lastSelectError = WSAGetLastError();
    if(selectResult != CORE_NO_ERROR)
    {
        TRACE_MESSAGE3("Unable to send data in socket: '%S', last error %S",
            toString(selectResult).toWideChar(), errnoToString(lastSelectError, windowsStyle).toWideChar());
        return selectResult;
    }

    const int sent = ::send(m_socket, buff, bufflen, 0);
    const int lastSendError = WSAGetLastError();
    if(sent != bufflen)
    {
// TODO: make here a cycle, because send() can send only a part of data
        TRACE_MESSAGE4("Unable to send data in socket, last error %S, bytes sent %d, bytes to be sent %d",
            errnoToString(lastSendError, windowsStyle).toWideChar(), sent, bufflen);

        return isConnectionBroken(lastSendError) ? CORE_REMOTE_HOST_CLOSED_CONNECTION : CORE_DOWNLOAD_ERROR;
    }

    m_downloadProgress.bytesTransferred(bufflen);

    return CORE_NO_ERROR;
}

bool KLUPD::Socket::makeNonBlocking()
{
    #ifdef WIN32
        unsigned long on = 1;
        if(ioctlsocket(m_socket, FIONBIO, &on) != 0)
        {
            const int lastError = WSAGetLastError();
            TRACE_MESSAGE2("Could not set non-blocking flag, error: %S",
                errnoToString(lastError, windowsStyle).toWideChar());
            return false;
        }
    #else
        const int flags = fcntl(m_socket, F_GETFL) & (~O_NONBLOCK);
        if(fcntl(m_socket, F_SETFL, flags | O_NONBLOCK) != 0)
        {
            const int lastError = errno;
            TRACE_MESSAGE2("Could not set non-blocking flag, last error '%S'",
                errnoToString(lastError, posixStyle).toWideChar());
            return false;
        }
    #endif

    return true;
}

void KLUPD::Socket::closeSocketCrossPlatform(const int socketHandle)
{
    #ifdef WIN32
        closesocket(socketHandle);
    #else
        ::close(socketHandle);
    #endif
}
