#include "HttpProtocolImplementation.h"

#include "../helper/updaterStaff.h"

KLUPD::HttpProtocolImplementation::HttpProtocolImplementation(
    const AuthorizationTypeList &authorizationTypeSupportedByClient,
    const Credentials &proxyAuthorizationCredentials,
    const size_t downloadTimeout,
    DownloadProgress &downloadProgress, JournalInterface &journal, Log *log)
 : HttpProtocol(authorizationTypeSupportedByClient, proxyAuthorizationCredentials,
                downloadTimeout, downloadProgress, log),
   m_journal(journal)
{
}

KLUPD::CoreError KLUPD::HttpProtocolImplementation::resolveAddress(const bool useProxy, Address &destination)
{
    return CORE_NO_ERROR;
}

void KLUPD::HttpProtocolImplementation::informAuthorizationFailed(const DownloadProgress::AuthorizationTarget &authorizationTarget,
    const Path &fileName, const NoCaseString &serverWithCredentials)
{
    m_journal.publishMessage(
        authorizationTarget == DownloadProgress::server ? CORE_SERVER_AUTH_ERROR : CORE_PROXY_AUTH_ERROR,
        fileName.toWideChar(), serverWithCredentials);
}

void KLUPD::HttpProtocolImplementation::informNameResolved(const Path &name, const std::string &IP)
{
    m_journal.publishMessage(EVENT_DNS_NAME_RESOLVED, name.toWideChar(), asciiToWideChar(IP).c_str());
}


//////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////

KLUPD::HttpProtocolFileImplementation::HttpProtocolFileImplementation(
    const AuthorizationTypeList &authorizationTypeSupportedByClient,
    const Credentials &proxyAuthorizationCredentials,
    const size_t downloadTimeout,
    DownloadProgress &downloadProgress, JournalInterface &journal, Log *log)
 : HttpProtocolImplementation(authorizationTypeSupportedByClient,
        proxyAuthorizationCredentials, downloadTimeout,
        downloadProgress, journal, log),
   m_destinationFile(log),
   m_fileWithVariableExtensionNotObligatoryForUpdate(false)
{
}

KLUPD::CoreError KLUPD::HttpProtocolFileImplementation::getFile(
        const Path &fileName,
        const Path &localPath,
        const Path &relativeUrlPath,
        const Address &serverAddress, const std::string &userAgent,
        const bool useProxy, const Address &proxyAddress, const AuthorizationTypeList &proxyAuthorizationMethods,
        const size_t regettingPosition,
        const bool fileWithVariableExtensionNotObligatoryForUpdate)
{
    m_fileName = fileName;
    m_localPath = localPath;
    m_fileWithVariableExtensionNotObligatoryForUpdate = fileWithVariableExtensionNotObligatoryForUpdate;

    const CoreError getFileResult = HttpProtocol::getFile(fileName, relativeUrlPath,
        serverAddress, userAgent,
        useProxy, proxyAddress, proxyAuthorizationMethods, regettingPosition);

    m_destinationFile.close();

    return getFileResult;
}


KLUPD::CoreError KLUPD::HttpProtocolFileImplementation::dataReceived(const unsigned char *data, const size_t &size)
{
    return saveDataToFile(m_localPath + m_fileName, data, size, m_regettingPosition != 0, m_destinationFile, pLog);
}

bool KLUPD::HttpProtocolFileImplementation::treat_403_502_httpCodesAs407()
{
    return !m_fileWithVariableExtensionNotObligatoryForUpdate;
}

//////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////

KLUPD::HttpProtocolMemoryImplementation::HttpProtocolMemoryImplementation(
    const AuthorizationTypeList &authorizationTypeSupportedByClient,
    const Credentials &proxyAuthorizationCredentials,
    const size_t downloadTimeout,
    DownloadProgress &downloadProgress, JournalInterface &journal, Log *log)
 : HttpProtocolImplementation(authorizationTypeSupportedByClient,
        proxyAuthorizationCredentials,
        downloadTimeout,
        downloadProgress, journal, log)
{
}

KLUPD::CoreError KLUPD::HttpProtocolMemoryImplementation::getFile(
        const Path &fileName,
        const Path &relativeUrlPath,
        const Address &serverAddress, const std::string &userAgent,
        const bool useProxy, const Address &proxyAddress, const AuthorizationTypeList &proxyAuthorizationMethods)
{
    return HttpProtocol::getFile(fileName, relativeUrlPath,
        serverAddress, userAgent,
        useProxy, proxyAddress, proxyAuthorizationMethods, 0);
}

KLUPD::CoreError KLUPD::HttpProtocolMemoryImplementation::postFile(
        const std::vector<unsigned char> &postData,
        const Path &fileName,
        const Path &relativeUrlPath,
        const Address &serverAddress, const std::string &userAgent,
        const bool useProxy, const Address &proxyAddress, const AuthorizationTypeList &proxyAuthorizationMethods)
{
    return HttpProtocol::postFile(
        postData,
        fileName, relativeUrlPath,
        serverAddress, userAgent,
        useProxy, proxyAddress, proxyAuthorizationMethods);
}


KLUPD::CoreError KLUPD::HttpProtocolMemoryImplementation::dataReceived(const unsigned char *data, const size_t &size)
{
    m_fileInMemory.insert(m_fileInMemory.end(), data, data + size);
    return CORE_NO_ERROR;
}

