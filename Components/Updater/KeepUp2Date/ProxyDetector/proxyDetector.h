#ifndef PROXYDETECTOR_H_7CA14DDC_3C23_4167_9F51_D6070351DE32
#define PROXYDETECTOR_H_7CA14DDC_3C23_4167_9F51_D6070351DE32

#include "../Client/include/cfg_updater2.h"

// automatic proxy server address detection implemented only on Windows platform
#ifdef WIN32
    #include "winInetReader.h"
    #include "registryReader.h"
    #include "jsproxywrapper.h"

    // 4251 - needs to have dll-interface to be used by clients
    // 4786 - identifier was truncated to '255' characters in the browser information
    #pragma warning(disable: 4251 4786)
#endif

#include "iphlpapi.h"


#include <string>

namespace KLUPD {
    class JournalInterface;
    class DownloadProgress;
    class Log;
}

namespace ProxyDetectorNamespace {

// Purpose: proxy server address detection
class KAVUPDCORE_API ProxyDetector
{
public:
    ProxyDetector(const size_t networkTimeoutSeconds,
        KLUPD::DownloadProgress &, KLUPD::JournalInterface &, KLUPD::Log *);

	~ProxyDetector();

    // deprecated method to check if address belongs to one of local networks areas
    // lstAddrInfo [in] must point to addinfo structures chain, must not be NULL
    // strMsgResult [out] takes result message text
    // returns true if address is local, false otherwise
    bool IsLocalAddress(const addrinfo* pAddrInfo, KLUPD::NoCaseString& strMsgResult);

    // checks if address belongs to one of local networks areas
    // strAddress [in] must contain address string to check
    // strMsgResult [out] takes result message text
    // returns true if address is local, false otherwise
    bool IsLocalAddressEx(const KLUPD::NoCaseString& strAddress, KLUPD::NoCaseString& strMsgResult);

    // return proxy server address for given destination
    KLUPD::ProxyServerMode getProxyAddress(KLUPD::Address &proxyAddress, const KLUPD::Path &destinationUrl);
private:
    // checks if address is local
    bool _IsLocalAddressEx(const addrinfo* pAddrHost, KLUPD::NoCaseString& strMsgResult, bool& bConsidered);

#ifdef WIN32
    // checks if address is local
    bool _IsLocalAddressExWin32(const addrinfo* pAddrHost, IP_ADAPTER_ADDRESSES* pAdapterAddresses, KLUPD::NoCaseString& strMsgResult);
#endif

private:
    enum WpadState
    {
        wpadNotInitialized,
        wpadDownloaded,
        wpadDownloadFailed,
    };

    enum InternetExplorerState
    {
        internetExplorerNotInitialized,
        proxyCachedForAllDestinations,
        proxyCachedForAllDestinationsFailed,
    };

    WpadState m_wpadState;
    KLUPD::Path m_pathToDownloadedWpadScript;

    InternetExplorerState m_internetExplorerState;
    KLUPD::Address m_cachedProxyAddressForAllDestinations;


// automatic proxy server address detection implemented only on Windows platform
#ifdef WIN32
    WinInetReader m_winInetReader;
    JSProxyWrapper m_jsProxyWrapper;
	CIPHlpAPI m_ipHlpAPI;
#endif

    KLUPD::JournalInterface &m_journal;
    KLUPD::DownloadProgress &m_downloadProgress;
    KLUPD::Log *pLog;
};


} // namespace ProxyDetectorNamespace


#endif // PROXYDETECTOR_H_7CA14DDC_3C23_4167_9F51_D6070351DE32
