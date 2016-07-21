#include "registryReader.h"

#include "WindowsRegistryWrapper.h"
#include "proxyDetector.h"

bool ProxyDetectorNamespace::RegistryReader::get(KLUPD::Address &proxyAddress, const KLUPD::Path &destinationUrl, KLUPD::Log *log)
{
	std::string proxyServerFromRegistry;
	if(!readStringFromRegistry(proxyServerFromRegistry, log))
		return false;

	parseString(proxyServerFromRegistry, proxyAddress, destinationUrl, log);
    return true;
}

// reads proxy server string from registry
bool ProxyDetectorNamespace::RegistryReader::readStringFromRegistry(std::string &proxyServer, KLUPD::Log *pLog)
{
    const std::string internetSettings = "Software\\Microsoft\\Windows\\CurrentVersion\\Internet Settings";
    HKEY key = 0;
    if(!WindowsRegistryWrapper::openCurrentThreadUserRegistryKey(internetSettings.c_str(), key, pLog))
    {
        TRACE_MESSAGE2("Failed to open proxy settings registry key '%s'", internetSettings.c_str());
        return false;
    }

    const char *proxyEnableLiteral = "ProxyEnable";
    DWORD proxyEnabled = 0;
    if(!WindowsRegistryWrapper::readRegistryDword(key, proxyEnableLiteral, proxyEnabled, pLog))
    {
        TRACE_MESSAGE2("Failed to read proxy settings registry key '%s'", proxyEnableLiteral);
        RegCloseKey(key);
        return false;
    }

    if(!proxyEnabled)
    {
        TRACE_MESSAGE("Proxy server is disabled based on 'ProxyEnable' value from registry");
        RegCloseKey(key);
        return false;
    }

    // reading proxy server string from registry
    const bool result = WindowsRegistryWrapper::readRegistryString(key, "ProxyServer", proxyServer, pLog);
    RegCloseKey(key);
    return result;
}

// parses proxy server string which was read from Windows registry
void ProxyDetectorNamespace::RegistryReader::parseString(const std::string &proxyServer, KLUPD::Address &proxyAddress, const KLUPD::Path &destinationUrl, KLUPD::Log *pLog)
{
    const KLUPD::Address destinationAddress(destinationUrl);
    const std::string protocolPattern = (destinationAddress.m_protocol == KLUPD::ftpTransport) ? "ftp=" : "http=";

    std::string::size_type position = proxyServer.find(protocolPattern.c_str());
    if(position == std::string::npos)
        position = 0;
    else
        position += protocolPattern.size();     // skip protocol prefix

    std::string::size_type positionEnd = proxyServer.find(";", position);
    if(positionEnd == std::string::npos)
        positionEnd = proxyServer.size();

    std::string strAddr;
    strAddr.assign(proxyServer, position, positionEnd - position);
    if(strAddr.find("://") == std::string::npos)    // look for protocol prefix
        strAddr.insert(0, "http://");

    KLUPD::Path pathProxy;
    pathProxy.fromAscii(strAddr);
    pathProxy.correctPathDelimiters();

    proxyAddress.parse(pathProxy);
}
