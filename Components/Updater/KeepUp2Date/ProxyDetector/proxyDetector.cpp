#include "proxyDetector.h"

// automatic proxy server address detection implemented only on Windows platform
#ifdef WIN32
    #include "JSProxyWrapper.h"

    //#include <iphlpapi.h>
#endif

ProxyDetectorNamespace::ProxyDetector::ProxyDetector(const size_t networkTimeoutSeconds,
    KLUPD::DownloadProgress &downloadProgress, KLUPD::JournalInterface &journal, KLUPD::Log *log)
 : m_wpadState(wpadNotInitialized),
   m_internetExplorerState(internetExplorerNotInitialized),
// automatic proxy server address detection implemented only on Windows platform
#ifdef WIN32
   m_winInetReader(networkTimeoutSeconds, downloadProgress, journal, log),
#endif
   m_journal(journal),
   m_downloadProgress(downloadProgress),
   pLog(log)
{
}

ProxyDetectorNamespace::ProxyDetector::~ProxyDetector()
{
}

bool ProxyDetectorNamespace::ProxyDetector::IsLocalAddress(const addrinfo* pAddrInfo, KLUPD::NoCaseString& strMsgResult)
{
#ifdef WIN32
    strMsgResult.erase();

    if( pAddrInfo->ai_family != PF_INET || pAddrInfo->ai_addrlen < sizeof(sockaddr_in) )
        return false;

    sockaddr_in* pSockAddr = reinterpret_cast<sockaddr_in*>(pAddrInfo->ai_addr);
    in_addr addr = pSockAddr->sin_addr;

    if(addr.S_un.S_addr == INADDR_ANY || addr.S_un.S_addr == INADDR_NONE)
    {
        strMsgResult = L"INADDR_ANY or INADDR_NONE address)";
        return false;
    }

    if( !m_ipHlpAPI.IsInitialized() ) 
    {
        strMsgResult = L"IpHlpAPI library has not been initialized";
        return false;
    }

    //////////////////////////////////////////
    /// get buffer size for IP address table
    unsigned long ipAddressTableBufferSize = 0;
    const DWORD getIpAddressTableSizeResult = 
		m_ipHlpAPI.m_pGetIpAddrTable(0, &ipAddressTableBufferSize, FALSE);
    if(getIpAddressTableSizeResult == NO_ERROR)
    {
        strMsgResult = L"addresses table is empty";
        return false;
    }

    if(getIpAddressTableSizeResult != ERROR_INSUFFICIENT_BUFFER)
    {
        strMsgResult = KLUPD::NoCaseString(L"error obtaining IP addresses table( ")
            + KLUPD::errnoToString(getIpAddressTableSizeResult, KLUPD::windowsStyle) + L")";
        return false;
    }


    //////////////////////////////////////////
    /// get IP address table
    std::vector<unsigned char> ipAddressTableBuffer(ipAddressTableBufferSize, 0);
    PMIB_IPADDRTABLE infoIpAddr = reinterpret_cast<PMIB_IPADDRTABLE>(&ipAddressTableBuffer[0]);
	const DWORD getIpAddressTableResult = m_ipHlpAPI.m_pGetIpAddrTable(infoIpAddr, &ipAddressTableBufferSize, FALSE);
    if(getIpAddressTableResult != NO_ERROR)
    {
        strMsgResult = KLUPD::NoCaseString(L"error obtaining IP addresses table(")
            + KLUPD::errnoToString(getIpAddressTableSizeResult, KLUPD::windowsStyle) + L")";
        return false;
    }


    //////////////////////////////////////////
    /// check if address is local against local table
    for(size_t ipAddressTableIndex = 0; ipAddressTableIndex < infoIpAddr->dwNumEntries; ++ipAddressTableIndex)
    {
        const unsigned long ipAddress = infoIpAddr->table[ipAddressTableIndex].dwAddr;
        const unsigned long subnetMask = infoIpAddr->table[ipAddressTableIndex].dwMask ;

        // skip invalid address
        if((ipAddress == INADDR_NONE) || (ipAddress == INADDR_ANY)
            || (subnetMask == INADDR_NONE) || (subnetMask == INADDR_ANY))
        {
            continue;
        }

        // check if address belongs to local network
        if( (addr.S_un.S_addr & subnetMask) == (ipAddress & subnetMask) )
        {
            strMsgResult = KLUPD::NoCaseString(L"belongs to local network connected to adapter ")
                    + KLUPD::asciiToWideChar(KLUPD::inet_ntoaSafe(*(reinterpret_cast<const in_addr *>(&ipAddress))))
                + KLUPD::NoCaseString(L" mask ")
                    + KLUPD::asciiToWideChar(KLUPD::inet_ntoaSafe(*(reinterpret_cast<const in_addr *>(&subnetMask))));

            return true;
        }
    }
    strMsgResult = L"belongs to no network connected to local adapters";
#endif // WIN32
    return false;
}

bool ProxyDetectorNamespace::ProxyDetector::_IsLocalAddressEx(const addrinfo* pAddrHost, KLUPD::NoCaseString& strMsgResult, bool& bConsidered)
{
    bConsidered = true;

    switch(pAddrHost->ai_family)
    {
    case PF_INET:
        if(pAddrHost->ai_addrlen >= sizeof(sockaddr_in))
        {
            const sockaddr_in* pAddrInHost = reinterpret_cast<const sockaddr_in*>(pAddrHost->ai_addr);
            if(pAddrInHost->sin_addr.s_addr == 0x0100007f) // 127.0.0.1
            {
                strMsgResult = L"loopback IPv4 address";
                return true;
            }
        }
        break;
    case PF_INET6:
        if(pAddrHost->ai_addrlen >= sizeof(sockaddr_in6))
        {
            const sockaddr_in6* pAddrIn6Host = reinterpret_cast<const sockaddr_in6*>(pAddrHost->ai_addr);

            // check for unspecified or loopback addresses
            bool bZeros = false;
            for(int i = 0; i < 15; ++i)
                if( !(bZeros = pAddrIn6Host->sin6_addr.s6_addr[i] == 0x0000) )
                    break;
            if(bZeros)
            {
                if(pAddrIn6Host->sin6_addr.s6_addr[15] == 0x0000) // ::0000
                {
                    strMsgResult = L"unspecified IPv6 address";
                    return false;
                }
                if(pAddrIn6Host->sin6_addr.s6_addr[15] == 0x0001) // ::0001
                {
                    strMsgResult = L"loopback IPv6 address";
                    return true;
                }
            }
        }
        break;
    }

    bConsidered = false;
    return false;
}

#ifdef WIN32
bool ProxyDetectorNamespace::ProxyDetector::_IsLocalAddressExWin32(const addrinfo* pAddrHost, IP_ADAPTER_ADDRESSES* pAdapterAddresses, KLUPD::NoCaseString& strMsgResult)
{
    bool bConsidered;
    bool bLocal = _IsLocalAddressEx(pAddrHost, strMsgResult, bConsidered);
    if( bConsidered )
        return bLocal;

    bLocal = false;

    for(IP_ADAPTER_ADDRESSES* pAA = pAdapterAddresses; pAA != NULL && !bLocal; pAA = pAA->Next)
    {
        if( pAdapterAddresses->Length < sizeof(IP_ADAPTER_ADDRESSES) )
        {
            strMsgResult = L"unsupported adapters table format";
            return false;
        }
        for(IP_ADAPTER_PREFIX* pPrefix = pAA->FirstPrefix; pPrefix != NULL && !bLocal; pPrefix = pPrefix->Next)
        {
            if( pPrefix->Length < sizeof(IP_ADAPTER_PREFIX) )
            {
                strMsgResult = L"unsupported adapters prefixes format";
                return false;
            }
            LPSOCKADDR pAddrPrefix = pPrefix->Address.lpSockaddr;
            if( pAddrPrefix->sa_family == pAddrHost->ai_family )
            {
                const BYTE* pBytesHost = NULL;
                const BYTE* pBytesPrefix = NULL;

                switch(pAddrHost->ai_family)
                {
                case PF_INET:
                    if(pAddrHost->ai_addrlen >= sizeof(sockaddr_in) && pPrefix->PrefixLength <= sizeof(in_addr)*8)
                    {
                        pBytesHost = &reinterpret_cast<const sockaddr_in*>(pAddrHost->ai_addr)->sin_addr.S_un.S_un_b.s_b1;
                        pBytesPrefix = &reinterpret_cast<const sockaddr_in*>(pAddrPrefix)->sin_addr.S_un.S_un_b.s_b1;
                    }
                    break;
                case PF_INET6:
                    if(pAddrHost->ai_addrlen >= sizeof(sockaddr_in6) && pPrefix->PrefixLength <= sizeof(in6_addr)*8)
                    {
                        pBytesHost = reinterpret_cast<const sockaddr_in6*>(pAddrHost->ai_addr)->sin6_addr.u.Byte;
                        pBytesPrefix = reinterpret_cast<const sockaddr_in6*>(pAddrPrefix)->sin6_addr.u.Byte;
                    }
                    break;
                }

                if( pBytesHost != NULL && pBytesPrefix != NULL )
                {
                    DWORD nBytes = pPrefix->PrefixLength / 8;       // number of byte pairs to compare
                    BYTE nBits = BYTE(pPrefix->PrefixLength % 8);   // number of bits to compare last pair by
                    BYTE nMask = ((1 << nBits) - 1) << (8-nBits);   // mask to compare last pair by

                    bool bEqual = true;
                    for(DWORD i = 0; i < nBytes && bEqual; ++i)
                        bEqual = pBytesHost[i] == pBytesPrefix[i];
                    if(bEqual)
                        bLocal = nMask == 0 || (pBytesHost[nBytes] & nMask) == (pBytesPrefix[nBytes] & nMask);
                }
            }
        }
        if(bLocal)
            strMsgResult = KLUPD::NoCaseString(L"address belongs to \'") + pAA->Description + L"\' adapter network area";
    }

    return bLocal;
}
#endif  // WIN32

bool ProxyDetectorNamespace::ProxyDetector::IsLocalAddressEx(const KLUPD::NoCaseString& strAddress, KLUPD::NoCaseString& strMsgResult)
{
    bool bLocal = false;
    bool bOk = false;

    addrinfo* aiList = NULL;
    if( !(bOk = ::getaddrinfo(strAddress.toAscii().c_str(), NULL, NULL, &aiList) == 0) )
    {
        strMsgResult = L"can't resolve address";
        return bLocal;
    }
#ifdef WIN32
    BYTE* pBuf = NULL;
    IP_ADAPTER_ADDRESSES* pAdapterAddresses = NULL;

    if( bOk = m_ipHlpAPI.IsInitialized() )
    {
        DWORD dwFlags = GAA_FLAG_INCLUDE_PREFIX | GAA_FLAG_SKIP_UNICAST | GAA_FLAG_SKIP_ANYCAST | GAA_FLAG_SKIP_FRIENDLY_NAME | GAA_FLAG_SKIP_MULTICAST | GAA_FLAG_SKIP_DNS_SERVER;
        DWORD dwSize;
        DWORD dwRes = m_ipHlpAPI.SafeGetAdaptersAddresses(PF_UNSPEC, dwFlags, NULL, NULL, &dwSize);
        if (bOk = dwRes == ERROR_BUFFER_OVERFLOW)
        {
            if(bOk = dwSize >= sizeof(IP_ADAPTER_ADDRESSES))
            {
                pBuf = new BYTE[dwSize];
                if( bOk = pBuf != NULL )
                {
                    pAdapterAddresses = reinterpret_cast<IP_ADAPTER_ADDRESSES*>(pBuf);

                    dwRes = m_ipHlpAPI.SafeGetAdaptersAddresses(PF_UNSPEC, dwFlags, NULL, pAdapterAddresses, &dwSize);
                    bOk = dwRes == ERROR_SUCCESS;
                }
            }
        }
        
        if(!bOk)
            strMsgResult = L"can't enumerate local adapters";
    }
    else
        strMsgResult = L"IpHlpAPI library has not been initialized";

    bool bUseOldScheme = !bOk;
    for(addrinfo* pAddrInfo = aiList; pAddrInfo != NULL; pAddrInfo = pAddrInfo->ai_next)
    {
        if( !(bOk = pAddrInfo->ai_addr != NULL) )
        {
            strMsgResult += L"can't resolve address";
            break;
        }
        if( bLocal = bUseOldScheme ? IsLocalAddress(pAddrInfo, strMsgResult) : 
                                     _IsLocalAddressExWin32(pAddrInfo, pAdapterAddresses, strMsgResult) )
            break;
    }

    delete[] pBuf;
#else
    bool bConsidered = false;
    for(addrinfo* pAddrInfo = aiList; pAddrInfo != NULL; pAddrInfo = pAddrInfo->ai_next)
    {
        if( !(bOk = pAddrInfo->ai_addr != NULL) )
        {
            strMsgResult += L"can't resolve address";
            break;
        }
        if( bLocal = _IsLocalAddressEx(pAddrInfo, strMsgResult, bConsidered) )
            break;
    }
    if( !bConsidered )
        strMsgResult = L"not implemeted for current platform";
#endif // WIN32

    ::freeaddrinfo(aiList);

    return bLocal;
}


KLUPD::ProxyServerMode ProxyDetectorNamespace::ProxyDetector::getProxyAddress(KLUPD::Address &proxyAddress,
    const KLUPD::Path &destinationUrl)
{
// automatic proxy server address detection implemented only on Windows platform
#ifdef WIN32
    if(m_wpadState == wpadNotInitialized)
    {
	    // downloading WPAD script
 	    if(m_winInetReader.downloadWPAD_Script())
        {
            m_pathToDownloadedWpadScript = m_winInetReader.m_wpad_LocalFullFileName;
            m_wpadState = wpadDownloaded;
        }
        else
            m_wpadState = wpadDownloadFailed;
    }

    if(m_wpadState == wpadDownloaded)
    {
        KLUPD::NoCaseString resultReadableDescription;
        const KLUPD::ProxyServerMode getProxyInfoResult = m_jsProxyWrapper.internetGetProxyInfo(m_pathToDownloadedWpadScript,
            proxyAddress, destinationUrl, resultReadableDescription);
        if(!resultReadableDescription.empty())
            TRACE_MESSAGE2("%S", resultReadableDescription.toWideChar());
        return getProxyInfoResult;
    }


    // try to read from Windows Registry for Internet Explorer
    if(m_internetExplorerState == internetExplorerNotInitialized)
    {
        if(ProxyDetectorNamespace::RegistryReader::get(m_cachedProxyAddressForAllDestinations, destinationUrl, pLog))
            m_internetExplorerState = proxyCachedForAllDestinations;
        else
            m_internetExplorerState = proxyCachedForAllDestinationsFailed;
    }

    if(m_internetExplorerState == proxyCachedForAllDestinations)
    {
        proxyAddress = m_cachedProxyAddressForAllDestinations;
        if(proxyAddress.empty())
        {
            TRACE_MESSAGE2("Internet Explorer settings not to use proxy server for '%S'", destinationUrl.toWideChar());
            return KLUPD::directConnection;
        }
        TRACE_MESSAGE2("Internet Explorer settings to use proxy server for '%S'", destinationUrl.toWideChar());
        return KLUPD::withProxy;
    }

#else
    TRACE_MESSAGE("Warning: automatic proxy detection is not implemented");
#endif

    TRACE_MESSAGE2("Failed to detect proxy server address automatically, direct connection is used for '%S'",
        destinationUrl.toWideChar());
    return KLUPD::directConnection;
}
