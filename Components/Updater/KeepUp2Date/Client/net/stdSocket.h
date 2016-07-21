#if !defined(STDSOCKET_H_INCLUDED_A5A9176C_EBE3_4803_BFDA_7B2CB0EBBFBF)
#define STDSOCKET_H_INCLUDED_A5A9176C_EBE3_4803_BFDA_7B2CB0EBBFBF

#include "../helper/comdefs.h"
#include "../helper/stdinc.h"

#ifndef SD_BOTH
    #define SD_BOTH SHUT_RDWR
#endif

#ifndef _WIN32
typedef int SOCKET;
const int INVALID_SOCKET = -1;
#endif

namespace KLUPD {

// a wrapper for Berkley Sockets
class Socket
{
public:
    // controls socket library initialization/deinitialization
    class Initializer
    {
    public:
        Initializer();
        ~Initializer();

        bool IsInitialized() {return bInitialized; }
    private:
        bool bInitialized;
    };

    Socket(int nTimeoutSecs, DownloadProgress &, Log *);
    ~Socket();

    // close socket descriptor
    void close();

    bool bind(const char* szHost, const char* szService, int nPF = PF_UNSPEC);

    // listen on socket
    bool listen();
    // connect this socket to specified address and port
    // szHost [in] - host address string
    // szService [in] - service name or port number
    // return true if connected, negative error code otherwise
    KLUPD::CoreError connect(const char* szHost, const char* szService, const bool throughProxy, int nPF = PF_UNSPEC);
    // accept incoming connection on already binded and listened socket
    // return new socket with accepted connection
    bool accept(Socket &);

    // return address of the socket
    bool getSockName(std::string &strHost, std::string& strService, int nFlags = NI_NUMERICHOST | NI_NUMERICSERV);

    // send bytes through socket
    // buffer [in] pointer to data buffer
    // len [in] length of data
    // return result of send operation
    KLUPD::CoreError send(const char *buffer, const int len);
    // receive bytes through socket
    // buffer [in] pointer to initialized data buffer
    // len [in] length of buffer in bytes
    // return number of successfully read bytes
    int recv(char *buffer, const int len, KLUPD::CoreError &result);

    // platform independently check if current errno indicates that operation on socket would block
    bool wouldBlockError(const int lastError)const;
    // platform independently check if connection is broken
    bool isConnectionBroken(const int lastError)const;

    // returns protocol family used
    int GetProtoFamily() const { return m_nPF; }

    // sets socket operations timeoout
    void SetTimeout(int nMilliseconds) { m_nTimeout = nMilliseconds;   }

private:
    // create socket descriptor
    // returns true if socket is created, false otherwise
    bool create(addrinfo* pAddrInfoList, int nPF = PF_UNSPEC);

    // helper class to track system resources
    class ResourceTracker;

    // disable copy operations
    Socket &operator=(const Socket &);
    Socket(const Socket &);


    void closeSocketCrossPlatform(const int socketHandle);


    // Wait for 1 secs on select for this descriptor
    // write [in] flag whether this descriptor for send or recv
    // return 1, if there is data in the socket, 0 if timed out and -1 on error
    int wait1Second(const bool write);
    // wait specified amount of time on socket descriptor
    // write [in] flag whether this descriptor for send or recv
    // return true, if data recieved/sent, false otherwise (timed out or error occured)
    KLUPD::CoreError select(const bool write);

    /// checks if protocol family is supported by class
    bool _IsProtoFamilySupported(int pf)
    {
        return pf == PF_INET || pf == PF_INET6;
    }

    // make socket non-blocking
    // return true, if all desired options are set, false otherwise
    bool makeNonBlocking();

    SOCKET m_socket;
    int m_nPF;      // protocol family used on socket creation

    int m_nTimeout; // operations timeou in milliseconds

    DownloadProgress &m_downloadProgress;
    Log *pLog;
};

}   // namespace KLUPD

#endif  // STDSOCKET_H_INCLUDED_A5A9176C_EBE3_4803_BFDA_7B2CB0EBBFBF
