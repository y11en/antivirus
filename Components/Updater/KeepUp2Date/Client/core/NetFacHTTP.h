#ifndef NetFacHTTP_H_INCLUDED_072A9FA4_E9BD_49ae_8BF3_29EC7EFA5FEA
#define NetFacHTTP_H_INCLUDED_072A9FA4_E9BD_49ae_8BF3_29EC7EFA5FEA

#if _MSC_VER > 1000
    #pragma once
#endif // _MSC_VER > 1000

#include "../helper/stdinc.h"
#include "HttpAuthorizationDriver.h"
#include "../net/stdSocket.h"

namespace KLUPD {

class KAVUPDCORE_API HttpProtocol : public Socket::Initializer
{
public:
    enum Method
    {
        get,
        post
    };

    HttpProtocol(const AuthorizationTypeList &authorizationTypeSupportedByClient,
                 const Credentials &proxyAuthorizationCredentials,
                 const size_t downloadTimeout, DownloadProgress &, Log *);
    virtual ~HttpProtocol();

	// provide ability to force close connection thus reduce load on server
    void closeSession();

    CoreError getFile(
        const Path &fileName,
        const Path &relativeUrlPath,
        const Address &serverAddress, const std::string &userAgent,
        const bool useProxy, const Address &proxyAddress, const AuthorizationTypeList &,
        const size_t regettingPosition);

    CoreError postFile(
        const std::vector<unsigned char> postData,
        const Path &fileName,
        const Path &relativeUrlPath,
        const Address &serverAddress, const std::string &userAgent,
        const bool useProxy, const Address &proxyAddress, const AuthorizationTypeList &);

protected:
    // callback on full HTTP header received. You may get header fields
     // to parse content according to encoding, compression and other parameters
    virtual void fullHttpHeaderReceived(const std::string &httpHeader);
    // callback on data received
     // Note: default implementation calculates location to to save file from FileInfo
    virtual CoreError dataReceived(const unsigned char *data, const size_t &) = 0;

    // it is recommended that update theat 403 and 502 codes as 407 code
     // to force use authorization against some specific servers.
    // WARNING: do not thread 403 and 502 codes as 407 if you use general HTTP implementation,
     // it is going to break normal HTTP download process and only usable for update procedure
    //
    // in case 403 or 502 code is received, then one more authorization
    //  request is performed and user credentials are requested
    virtual bool treat_403_502_httpCodesAs407();

    virtual void informAuthorizationFailed(const DownloadProgress::AuthorizationTarget &,
        const Path &fileName, const NoCaseString &serverWithCredentials);
    virtual void informNameResolved(const Path &name, const std::string &IP);


    // protected for convenience child classes uses single pLog variable
    Log *pLog;
    // current regetting position
    size_t m_regettingPosition;

private:
    // The HttpHeader class represent structure of HTTP response received
    class HttpHeader
    {
    public:
        enum KeepConnection
        {
            keepAlive,
            close,
        };

        HttpHeader();
        void clear();
        bool load(const char *buffer, HttpAuthorizationDriver &proxyAuthorizationMethods);

        int httpCode()const;
        // return true in case 200 or 206 code
        bool isFile()const;
        // 404 and some 3xx codes
        bool fileNotFound()const;
        bool authorizationNeeded()const;
        DownloadProgress::AuthorizationTarget authorizationTarget()const;
        bool redirectRequired()const;
        bool resourceUnavailable()const;
        // some proxies may return 403 or 502 codes after successful authorization,
         // and user may provide another credentials to authorize
        bool retryAuthorization(const bool authorizationWasNeeded)const;


        static CoreError convertHttpCodeToUpdaterCode(const int code);
        CoreError convertHttpCodeToUpdaterCode()const;
        bool needCloseConnection(const bool useProxy)const;

        signed long m_contentLength;     // -1 - not  defined
        // redirect location
        Path m_location;

    private:
        // Extracts HTTP line from buffer
         // return number of bytes taken from buffer
        size_t getLine(const char *input, std::string &headerLine);
        bool parseFirstLine(const std::string &httpCodeBuffer);
        bool parseHeaderField(const NoCaseString &field, HttpAuthorizationDriver &proxyAuthorizationMethods);


        int m_httpCode;
        KeepConnection m_serverConnection;
        KeepConnection m_proxyConnection;

        // HTTP header field
        static const NoCaseString s_httpFieldConnection;
        static const NoCaseString s_httpFieldProxyConnection;
        static const NoCaseString s_httpFieldContentLength;
        static const NoCaseString s_httpFieldProxy_Authenticate;
        static const NoCaseString s_httpFieldLocation;
        static const NoCaseString s_httpFieldContentLocation;
    };


    /// Request building helper
    class HTTPRequestBuilder
    {
    public:
        HTTPRequestBuilder(const Method &);
        void addLine(const std::string &);
        
        // output HTTP message in human-readable way. Removing HTTP un-readable delimiters '\r\n'
        std::string toString()const;
        static std::string toString(const char *, size_t = -1);

        // generates HTTP request string from provided input values
        const std::vector<unsigned char> generateRequest(const Path &fileName, const Path &relativeUrlPath,
            const bool useProxy,
            const Address &serverAddress,
            const std::string &userAgent,
            const std::string &proxyAuthorizationHeader,
            const size_t regettingPosition,
            const std::vector<unsigned char> &postData);

    private:
        std::vector<unsigned char> m_requestBuffer;
        Method m_method;
    };


    // disable copy operations
    HttpProtocol &operator=(const HttpProtocol &);
    HttpProtocol(const HttpProtocol &);

    CoreError httpRequest(
        const Path &fileName,
        const Path &relativeUrlPath,
        const Address &serverAddress, const std::string &userAgent,
        const bool useProxy, const Address &proxyAddress, const AuthorizationTypeList &,
        const size_t regettingPosition);

    // two attempts should be done to download file, because HTTP connection may be broken due to timeout
    CoreError httpRequestAttempt(
        const Path &fileName,
        const Path &relativeUrlPath,
        const Address &serverAddress, const std::string &userAgent,
        const bool useProxy, const Address &proxyAddress, const AuthorizationTypeList &,
        const size_t regettingPosition);

    // establishes connection if connection is not already established
    CoreError setupLowLevelConnectionIfNeed(const bool useProxy, Address &destination, const AuthorizationTypeList &);


    CoreError receiveResponseAndData(HttpHeader &);

    bool switchAuthorization(const Path &fileName, const Path &relativeUrlPath,
        const DownloadProgress::AuthorizationTarget &, const Address &proxyAddress,
        bool &authorizationTypeSwitched,
        bool &ntlmAuthorizationTriedAlready);

    Socket m_socket;
    HttpAuthorizationDriver m_authorizationDriver;

    bool m_connected;
    Address m_connectedTo;

    Method m_method;
    std::vector<unsigned char> m_postData;

    DownloadProgress &m_downloadProgress;
};

}   // namespace KLUPD

#endif // NetFacHTTP_H_INCLUDED_072A9FA4_E9BD_49ae_8BF3_29EC7EFA5FEA
