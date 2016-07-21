#ifndef HTTPPROTOCOLIMPLEMENTATION_H_INCLUDED_4D8EC3BB_E57B_435d_BA94_2D457E40E09E
#define HTTPPROTOCOLIMPLEMENTATION_H_INCLUDED_4D8EC3BB_E57B_435d_BA94_2D457E40E09E

#if _MSC_VER > 1000
    #pragma once
#endif // _MSC_VER > 1000

#include "NetFacHTTP.h"
#include "../helper/updaterStaff.h"

namespace KLUPD {

// common code for File and Memory implemenations
class KAVUPDCORE_API HttpProtocolImplementation : public HttpProtocol
{
public:
    HttpProtocolImplementation(
        const AuthorizationTypeList &authorizationTypeSupportedByClient,
        const Credentials &proxyAuthorizationCredentials,
        const size_t downloadTimeout,
        DownloadProgress &, JournalInterface &, Log *);

    CoreError getFile(
        const std::string &fileName,
        const std::string &relativeUrlPath,
        const Address &serverAddress, const std::string &userAgent,
        const bool useProxy, const Address &proxyAddress, const AuthorizationTypeList &,
        const size_t regettingPosition);

protected:
    virtual CoreError resolveAddress(const bool useProxy, Address &);

    virtual void informAuthorizationFailed(const DownloadProgress::AuthorizationTarget &,
        const Path &fileName, const NoCaseString &serverWithCredentials);
    virtual void informNameResolved(const Path &name, const std::string &IP);


private:
    JournalInterface &m_journal;
};



// download HTTP into file
class KAVUPDCORE_API HttpProtocolFileImplementation : public HttpProtocolImplementation
{
public:
    HttpProtocolFileImplementation(
        const AuthorizationTypeList &authorizationTypeSupportedByClient,
        const Credentials &proxyAuthorizationCredentials,
        const size_t downloadTimeout,
        DownloadProgress &, JournalInterface &, Log *);

    CoreError getFile(
        const Path &fileName,
        // local path to save file
        const Path &localPath,
        const Path &relativeUrlPath,
        const Address &serverAddress, const std::string &userAgent,
        const bool useProxy, const Address &proxyAddress, const AuthorizationTypeList &,
        const size_t regettingPosition,
        // difference files are not obligatory for Update, because plain file will be downloaded,
         //  difference files may have different extensions and be not allowed on proxies.
         // It is not needed to tread 403 and 502 codes as 407 code in this case
        const bool fileWithVariableExtensionNotObligatoryForUpdate);

protected:
    virtual CoreError dataReceived(const unsigned char *data, const size_t &);
    virtual bool treat_403_502_httpCodesAs407();

private:
    AutoStream m_destinationFile;

    Path m_fileName;
    Path m_localPath;
    bool m_fileWithVariableExtensionNotObligatoryForUpdate;
};



// download HTTP into memory
class KAVUPDCORE_API HttpProtocolMemoryImplementation : public HttpProtocolImplementation
{
public:
    HttpProtocolMemoryImplementation(
        const AuthorizationTypeList &authorizationTypeSupportedByClient,
        const Credentials &proxyAuthorizationCredentials,
        const size_t downloadTimeout,
        DownloadProgress &, JournalInterface &, Log *);

    CoreError getFile(
        const Path &fileName,
        const Path &relativeUrlPath,
        const Address &serverAddress, const std::string &userAgent,
        const bool useProxy, const Address &proxyAddress, const AuthorizationTypeList &);

    CoreError postFile(
        const std::vector<unsigned char> &postData,
        const Path &fileName,
        const Path &relativeUrlPath,
        const Address &serverAddress, const std::string &userAgent,
        const bool useProxy, const Address &proxyAddress, const AuthorizationTypeList &);

    std::vector<unsigned char> m_fileInMemory;

protected:
    virtual CoreError dataReceived(const unsigned char *data, const size_t &);
};


}   // namespace KLUPD

#endif  // HTTPPROTOCOLIMPLEMENTATION_H_INCLUDED_4D8EC3BB_E57B_435d_BA94_2D457E40E09E

