#include "NetFacHTTP.h"

#include "../helper/updaterStaff.h"

KLUPD::HttpProtocol::HttpHeader::HttpHeader()
 : m_contentLength(-1),    // -1 - not  defined
   m_httpCode(0),
   m_serverConnection(close),
   m_proxyConnection(close)
{
}

void KLUPD::HttpProtocol::HttpHeader::clear()
{
    m_httpCode = 0;
    m_serverConnection = close;
    m_proxyConnection = close;
    m_contentLength = -1;
    m_location.erase();
}

bool KLUPD::HttpProtocol::HttpHeader::load(const char *headerBuffer, HttpAuthorizationDriver &httpAuthorizationDriver)
{
    if(!headerBuffer)
        return false;

    clear();

    // getting HTTP code
    std::string httpCodeBuffer;
    const size_t bytesReadHeader = getLine(headerBuffer, httpCodeBuffer);
    if(!bytesReadHeader)
        return false;
    headerBuffer += bytesReadHeader;

    if(!parseFirstLine(httpCodeBuffer))
        return false;

    while(*headerBuffer)
    {
        std::string fieldBuffer;
        const size_t bytesReadField = getLine(headerBuffer, fieldBuffer);
        if(!bytesReadField)
            return false;
        headerBuffer += bytesReadField;

        parseHeaderField(asciiToWideChar(fieldBuffer), httpAuthorizationDriver);
    }

    return true;
}

bool KLUPD::HttpProtocol::HttpHeader::parseFirstLine(const std::string &httpCodeBuffer)
{
    // Sample "HTTP/1.1 200 OK"
    if(httpCodeBuffer.size() < strlen("HTTP/1.1 200"))
        return false;

    // additional check
    if(_strnicmp(httpCodeBuffer.c_str(), "http/", strlen("http/")) != 0)
        return false;

    size_t codeOffset = httpCodeBuffer.find_first_not_of(' ', strlen("HTTP/1.1 "));

    // skip blanks
    while(httpCodeBuffer[codeOffset] == ' ')
        ++codeOffset;

    // http code expected
    if(httpCodeBuffer.size() < codeOffset + 3)
        return false;

    std::vector<char> codeBuffer(4, 0);
    strncpy(&codeBuffer[0], httpCodeBuffer.c_str() + codeOffset, 3);

    m_httpCode = atoi(&codeBuffer[0]);
    return true;
}


size_t KLUPD::HttpProtocol::HttpHeader::getLine(const char *input, std::string &headerLine)
{
    if(!input)
        return 0;

    size_t bytesRead = 0;

    // skip blanks
    while((input[bytesRead] == '\r')
        || (input[bytesRead] == '\n')
        || (input[bytesRead] == ' '))
    {
        ++bytesRead;
    }

    // loop until end marker reached
    while((input[bytesRead] != 0)
        && (input[bytesRead] != '\r')
        && (input[bytesRead] != '\n'))
    {
        headerLine += input[bytesRead];
        ++bytesRead;
    }
    return bytesRead;
}

// HTTP header field
const KLUPD::NoCaseString KLUPD::HttpProtocol::HttpHeader::s_httpFieldConnection = L"connection:";
const KLUPD::NoCaseString KLUPD::HttpProtocol::HttpHeader::s_httpFieldProxyConnection = L"proxy-connection:";
const KLUPD::NoCaseString KLUPD::HttpProtocol::HttpHeader::s_httpFieldContentLength = L"content-length:";
const KLUPD::NoCaseString KLUPD::HttpProtocol::HttpHeader::s_httpFieldProxy_Authenticate = L"proxy-authenticate:";
const KLUPD::NoCaseString KLUPD::HttpProtocol::HttpHeader::s_httpFieldLocation = L"location:";
const KLUPD::NoCaseString KLUPD::HttpProtocol::HttpHeader::s_httpFieldContentLocation = L"content-location:";

bool KLUPD::HttpProtocol::HttpHeader::parseHeaderField(const NoCaseString &field, HttpAuthorizationDriver &httpAuthorizationDriver)
{
    if(field.empty())
        return false;

    // the field must be in the first position (because field value may contain the same text, but starting not from 0-offset,
     // e.g.:   "Connection" and "Proxy-Connection" the string literal "Connection" should be in the very first position
    const size_t startStringOffset = 0;

    // Connection: close
    // Connection: keep-alive
    if(field.find(s_httpFieldConnection.toWideChar(), 0, s_httpFieldConnection.size()) == startStringOffset)
    {
        size_t offset = s_httpFieldConnection.size();
        // skip a blank
        while(field[offset] == L' ')
            ++offset;

        const NoCaseString value = field.toWideChar() + offset;

        if(value == L"close")
        {
            m_serverConnection = close;
            return true;
        }
        if(value == L"keep-alive")
        {
            m_serverConnection = keepAlive;
            return true;
        }

        // unknown value for "connection"
        return false;
    }

    // Proxy-Connection: close
    // Proxy-Connection: keep-alive
    if(field.find(s_httpFieldProxyConnection.toWideChar(), 0, s_httpFieldProxyConnection.size()) == startStringOffset)
    {
        size_t offset = s_httpFieldProxyConnection.size();
        // skip a blank
        while(field[offset] == ' ')
            ++offset;

        const NoCaseString value = field.toWideChar() + offset;

        if(value == L"close")
        {
            m_proxyConnection = close;
            return true;
        }
        if(value == L"keep-alive")
        {
            m_proxyConnection = keepAlive;
            return true;
        }

        // unknown value for "proxy-connection"
        return false;
    }

    // Content-Length: 100
    if(field.find(s_httpFieldContentLength.toWideChar(), 0, s_httpFieldContentLength.size()) == startStringOffset)
    {
        size_t offset = s_httpFieldContentLength.size();
        // skip a blank
        while(field[offset] == L' ')
            ++offset;

        const NoCaseString value = field.toWideChar() + offset;
        m_contentLength = atol(value.toAscii().c_str());
        return true;
    }

    // Proxy-Authenticate
    if(field.find(s_httpFieldProxy_Authenticate.toWideChar(), 0, s_httpFieldProxy_Authenticate.size()) == startStringOffset)
    {
        std::string::size_type offset = s_httpFieldProxy_Authenticate.size();
        // skip a blank
        while(field[offset] == ' ')
            ++offset;

        const NoCaseString value = field.toWideChar() + offset;

        const AuthorizationType authorizationType = fromStringAuthorization(value);
        httpAuthorizationDriver.addAuthorizationTypeSupportedByServer(authorizationType);

        if((authorizationType == ntlmAuthorization) || (authorizationType == ntlmAuthorizationWithCredentials))
            httpAuthorizationDriver.setNtlmAuthorizationToken(value.toAscii().c_str());

        return true;
    }

    // Location
    const bool location = field.find(s_httpFieldLocation.toWideChar(), 0, s_httpFieldLocation.size()) == startStringOffset;
    const bool conentLocation = field.find(s_httpFieldContentLocation.toWideChar(), 0, s_httpFieldContentLocation.size()) == startStringOffset;
    if(location || conentLocation)
    {
        std::string::size_type offset = location
            ? s_httpFieldLocation.size() : s_httpFieldContentLocation.size();

        // skip a blank
        while(field[offset] == L' ')
            ++offset;

        m_location = field.toWideChar() + offset;
        return true;
    }

    // some non-interested field
    return true;
}

int KLUPD::HttpProtocol::HttpHeader::httpCode()const
{
    return m_httpCode;
}

bool KLUPD::HttpProtocol::HttpHeader::isFile()const
{
// TODO: in case regetting is used and code 200 obtained, then file must be reset to zero
    return m_httpCode == 200      // OK
        || m_httpCode == 201         // Created (POST response)
        || m_httpCode == 204         // No Content (POST response)
        || m_httpCode == 206;        // Partial Content
}

bool KLUPD::HttpProtocol::HttpHeader::fileNotFound()const
{
    return redirectRequired()
        || m_httpCode == 404     // Not Found
        || m_httpCode == 410;    // Gone
}

bool KLUPD::HttpProtocol::HttpHeader::authorizationNeeded()const
{
    return m_httpCode == 401
        || m_httpCode == 407;
}

KLUPD::DownloadProgress::AuthorizationTarget KLUPD::HttpProtocol::HttpHeader::authorizationTarget()const
{
    return m_httpCode == 407
        ? DownloadProgress::proxyServer : DownloadProgress::server;
}

bool KLUPD::HttpProtocol::HttpHeader::redirectRequired()const
{
    if(m_location.empty())
        return false;

    return m_httpCode == 300
        || m_httpCode == 301
        || m_httpCode == 302
        || m_httpCode == 303
        || m_httpCode == 307;
}

bool KLUPD::HttpProtocol::HttpHeader::resourceUnavailable()const
{
    return m_httpCode == 503;
}

bool KLUPD::HttpProtocol::HttpHeader::retryAuthorization(const bool authorizationWasNeeded)const
{
    if(!authorizationWasNeeded)
        return false;

    return m_httpCode == 403
        || m_httpCode == 502;
}

KLUPD::CoreError KLUPD::HttpProtocol::HttpHeader::convertHttpCodeToUpdaterCode(const int code)
{
    switch(code)
    {
        case 404:
        case 410:
            return CORE_NO_SOURCE_FILE;
        case 401:
            return CORE_SERVER_AUTH_ERROR;
        case 407:
            return CORE_PROXY_AUTH_ERROR;
        case 200:
        case 206:
            return CORE_NO_ERROR;
        case 503:
            return CORE_CANT_CONNECT_INET_SERVER;
        default:
            return CORE_DOWNLOAD_ERROR;
    }
}

KLUPD::CoreError KLUPD::HttpProtocol::HttpHeader::convertHttpCodeToUpdaterCode()const
{
    return convertHttpCodeToUpdaterCode(m_httpCode);
}

bool KLUPD::HttpProtocol::HttpHeader::needCloseConnection(const bool useProxy)const
{
    return useProxy
        ? m_proxyConnection == HttpHeader::close
        : m_serverConnection == HttpHeader::close;
}

///////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////

KLUPD::HttpProtocol::HTTPRequestBuilder::HTTPRequestBuilder(const Method &method)
 : m_method(method)
{
}

void KLUPD::HttpProtocol::HTTPRequestBuilder::addLine(const std::string &line)
{
    if(line.empty())
        return;

    // erase last "\r\n"
    if(!m_requestBuffer.empty())
        m_requestBuffer.resize(m_requestBuffer.size() > 1 ? m_requestBuffer.size() - 2 : 0);

    m_requestBuffer.insert(m_requestBuffer.end(), line.begin(), line.end());
    const std::string headerEndMarker = "\r\n\r\n";
    m_requestBuffer.insert(m_requestBuffer.end(), headerEndMarker.begin(), headerEndMarker.end());
}

std::string KLUPD::HttpProtocol::HTTPRequestBuilder::toString()const
{
    if(m_requestBuffer.empty())
        return std::string();

    return toString(reinterpret_cast<const char *>(&m_requestBuffer[0]), m_requestBuffer.size());
}

std::string KLUPD::HttpProtocol::HTTPRequestBuilder::toString(const char *input, size_t size)
{
	std::string result = size != -1 ? std::string(input, size) : input;

    // remove data for better output
    const std::string::size_type dataOffset = result.find("\r\n\r\n");
    if(dataOffset != std::string::npos)
        result.erase(dataOffset);

    // removing '\r' symbol for more convenient output
    result.erase(std::remove_if(result.begin(), result.end(),
        std::bind2nd(std::equal_to<std::string::value_type>(), '\r')), result.end());

    return result;
}

const std::vector<unsigned char> KLUPD::HttpProtocol::HTTPRequestBuilder::generateRequest(const Path &fileName, const Path &relativeUrlPath,
    const bool useProxy, const Address &serverAddress,
    const std::string &userAgent,
    const std::string &proxyAuthorizationHeader,
    const size_t regettingPosition,
    const std::vector<unsigned char> &postData)
{
    m_requestBuffer.clear();

    // 1. "GET ftp://user:password@server.com/path/file.xml HTTP/1.1"
    {
        std::ostringstream stream;
        stream.imbue(std::locale::classic());

        // request type
        stream << (m_method == get ? "GET " : "POST ");

        // request_protocol_prefix, request_host
        if(useProxy)
        {
            stream << toProtocolPrefix(serverAddress.m_protocol).toAscii();
            if((serverAddress.m_protocol == ftpTransport)
                && !serverAddress.m_credentials.empty())
            {
                // user:password@
                stream << serverAddress.m_credentials.userName().toAscii()
                    << ":" << serverAddress.m_credentials.password().toAscii() << "@";
            }
            stream << serverAddress.m_hostname.toAscii();
            if(serverAddress.m_protocol == ftpTransport)
            {
                if(serverAddress.m_service != L"21")
                    stream << ":" << serverAddress.m_service.toAscii();
            }
            else
            {
                if(serverAddress.m_service != L"80")
                    stream << ":" << serverAddress.m_service.toAscii();
            }
        }

        Path remotePath = serverAddress.m_path + relativeUrlPath;
        remotePath.correctPathDelimiters();
        stream << remotePath.toAscii() << fileName.toAscii();

        // HTTP version
        stream << " HTTP/1.0";

        addLine(stream.str());
    }


    // 2. "Host: downloads1.kaspersky-labs.com"
    {
        std::ostringstream stream;
        stream.imbue(std::locale::classic());
        stream << "Host: " << serverAddress.m_hostname.toAscii();
        addLine(stream.str());
    }

    if(m_method == get)
    {
        // 3. "Pragma: no-cache" & "Cache-Control: no-cache"
        addLine("Pragma: no-cache");
        addLine("Cache-Control: no-cache");
    }

    // 4. Keep connection
    if(useProxy)
    {
        addLine("Proxy-Connection: keep-alive");
        addLine("Connection: keep-alive");
    }
    else
        addLine("Connection: keep-alive");

    // 5. User-agent
    if(!userAgent.empty())
    {
        std::ostringstream stream;
        stream.imbue(std::locale::classic());
        stream << "User-Agent: " << userAgent;
        addLine(stream.str());
    }

    // 6. Proxy authorization
    if(useProxy && !proxyAuthorizationHeader.empty())
    {
        std::ostringstream stream;
        stream.imbue(std::locale::classic());
        stream << "Proxy-Authorization: " << proxyAuthorizationHeader;
        addLine(stream.str());
    }

    // 7. Regetting range
    if(regettingPosition)
    {
        std::ostringstream stream;
        stream.imbue(std::locale::classic());
        stream << "Range: bytes=" << regettingPosition << "-";
        addLine(stream.str());
    }

    if(m_method == post)
    {
        // 8. Content Length
        std::ostringstream stream;
        stream.imbue(std::locale::classic());
        stream << "Content-Length: " << postData.size();
        addLine(stream.str());

        // 9. post data
        m_requestBuffer.insert(m_requestBuffer.end(), postData.begin(), postData.end());
    }
    return m_requestBuffer;
}


///////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////

KLUPD::HttpProtocol::HttpProtocol(const AuthorizationTypeList &authorizationTypeSupportedByClient,
                                  const Credentials &proxyAuthorizationCredentials,
                                  const size_t downloadTimeout, DownloadProgress &downloadProgress, Log *log)
 : pLog(log),
   m_regettingPosition(0),
   m_socket(downloadTimeout, downloadProgress, log),
   m_authorizationDriver(authorizationTypeSupportedByClient, proxyAuthorizationCredentials, log),
   m_connected(false),
   m_method(get),
   m_downloadProgress(downloadProgress)
{
}

KLUPD::HttpProtocol::~HttpProtocol()
{
    closeSession();
}

KLUPD::CoreError KLUPD::HttpProtocol::getFile(
    const Path &fileNameIn,
    const Path &relativeUrlPathIn,
    const Address &serverAddress, const std::string &userAgent,
    const bool useProxy, const Address &proxyAddress,
    const AuthorizationTypeList &proxyAuthorizationMethods,
    const size_t regettingPosition)
{
    m_method = get;
    return httpRequest(fileNameIn, relativeUrlPathIn,
        serverAddress, userAgent,
        useProxy, proxyAddress,
        proxyAuthorizationMethods,
        regettingPosition);
}

KLUPD::CoreError KLUPD::HttpProtocol::postFile(
    const std::vector<unsigned char> postData,
    const Path &fileNameIn,
    const Path &relativeUrlPathIn,
    const Address &serverAddress, const std::string &userAgent,
    const bool useProxy, const Address &proxyAddress,
    const AuthorizationTypeList &proxyAuthorizationMethods)
{
    m_postData = postData;
    m_method = post;

    return httpRequest(fileNameIn, relativeUrlPathIn,
        serverAddress, userAgent,
        useProxy, proxyAddress,
        proxyAuthorizationMethods,
        0);
}

KLUPD::CoreError KLUPD::HttpProtocol::httpRequest(
    const Path &fileNameIn,
    const Path &relativeUrlPathIn,
    const Address &serverAddress, const std::string &userAgent,
    const bool useProxy, const Address &proxyAddress,
    const AuthorizationTypeList &proxyAuthorizationMethods,
    const size_t regettingPosition)
{
    // second get file attempt may needed in case current connection has been timed out
    bool needSecondAttempt
        = m_connected && (m_connectedTo == (useProxy ? proxyAddress : serverAddress));

    while(true)
    {
        const CoreError getFileResult = httpRequestAttempt(fileNameIn, relativeUrlPathIn, serverAddress,
            userAgent, useProxy, proxyAddress, proxyAuthorizationMethods, regettingPosition);

        // check if not connection broken problem
        if(getFileResult != CORE_REMOTE_HOST_CLOSED_CONNECTION)
            return getFileResult;

        closeSession();

        // was not connected, thus no timeout could happen
        if(!needSecondAttempt)
            return getFileResult;

        TRACE_MESSAGE("Second HTTP download attempt will be performed, because connection may have been timed out");

        // last attempt (only 2 attempts)
        needSecondAttempt = false;

        // will re-use previous success authorization
        m_authorizationDriver.authorized(false);
    }
}
KLUPD::CoreError KLUPD::HttpProtocol::httpRequestAttempt(
    const Path &fileNameIn,
    const Path &relativeUrlPathIn,
    const Address &serverAddressIn, const std::string &userAgent,
    const bool useProxy, const Address &proxyAddressIn,
    const AuthorizationTypeList &proxyAuthorizationMethods,
    const size_t regettingPosition)
{
    size_t infiniteRedirectLoop = 0;

    Path fileName = fileNameIn;
    Path relativeUrlPath = relativeUrlPathIn;
    Address serverAddress = serverAddressIn;
    Address proxyAddress = proxyAddressIn;

    m_regettingPosition = regettingPosition;

    // cache for next iteration generate request
     // at first attempt the target is not known, but proxy server target is used,
     //  because of possible protection from client for authorization on server
    DownloadProgress::AuthorizationTarget authorizationTarget = DownloadProgress::proxyServer;


    ////////////////////
    // the State flags agains proxies that close connection after authorization switch.
    //  There are proxies (e.g. Squid/2.4.STABLE7) which deals in next way:
    // *** Client -> Server:  GET file
    // *** Server -> Client: HTTP 407 / Proxy-Connection: keep-alive
    // *** Server closes connection
    // *** Client -> Server GET 407
    //
    // Here are 3 flags to track such situations, that previous response is received,
    //  BUT then server closes connection
    bool previousResponseReceived = false;
    bool authorizationTypeSwitched = false;
    bool requestHasAlreadyBeenRepeated = false;
    ////////////////////


    ////////////////////
    // in case proxy server requires authorization, but sends incorrect (from HTTP)
    //   code for authorization failure in next way
    // *** Client -> Server:  GET file
    // *** Server -> Client: HTTP 407 / Proxy-Authorization: NTLM
    // *** Client -> Server GET  / Proxy-Authorization: NTLM
    // *** Server -> Client: HTTP 502 (or 403 or other code)
    // *** here is authorization state is forgotten, because 502 request does NOT contain "Proxy-Authorization: NTLM"
    // *** Client ask from product new credentials
    // *** Client -> Server GET  / Proxy-Authorization: NTLM (with new credentials)
    bool authorizationWasNeededOnProxy = false;

    // it makes no sence to try Ntlm without credentials authorization type,
     // because this authorization type does not depend on provided credentials
    bool ntlmAuthorizationTriedAlready = false;

    int lastHttpCode = 0;

    for(size_t protectionAgainstCyclingCounter = 100; protectionAgainstCyclingCounter; --protectionAgainstCyclingCounter)
    {
        const CoreError connectionResult = setupLowLevelConnectionIfNeed(useProxy, useProxy ? proxyAddress : serverAddress, proxyAuthorizationMethods);
        if(connectionResult != CORE_NO_ERROR)
        {
            TRACE_MESSAGE2("Failed to setup connection to HTTP Server, result '%S'",
                toString(connectionResult).toWideChar());
            return connectionResult;
        }

        // make proxy authorization header
        std::string proxyAuthorizationHeader;
        if(!m_authorizationDriver.makeProxyAuthorizationHeader(proxyAuthorizationHeader)
            // we know authorization methods supported by proxy server
            || ((m_authorizationDriver.currentAuthorizationType() == noAuthorization) && !m_authorizationDriver.supportedAuthorizationTypesByServer().empty()))
        {
            if(!switchAuthorization(fileName, relativeUrlPath, authorizationTarget, proxyAddress,
                authorizationTypeSwitched, ntlmAuthorizationTriedAlready))
            {
                if(lastHttpCode)
                    return HttpHeader::convertHttpCodeToUpdaterCode(lastHttpCode);

                return useProxy ? CORE_PROXY_AUTH_ERROR : CORE_SERVER_AUTH_ERROR;
            }

            // try other authorization type
            continue;
        }

        HTTPRequestBuilder requestBuilder(m_method);
        const std::vector<unsigned char> requestBuffer = requestBuilder.generateRequest(
            fileName,
            relativeUrlPath,
            useProxy,
            serverAddress,
            userAgent,
            proxyAuthorizationHeader,
            m_regettingPosition,
            m_postData);

        TRACE_MESSAGE2("Sending HTTP request\n%s", requestBuilder.toString().c_str());

        if(requestBuffer.empty())
        {
            TRACE_MESSAGE("Failed to send empty HTTP request");
            return CORE_DOWNLOAD_ERROR;
        }

        const CoreError sendRequestResult = m_socket.send(reinterpret_cast<const char *>(&requestBuffer[0]), requestBuffer.size());
        if(sendRequestResult != CORE_NO_ERROR)
        {
            TRACE_MESSAGE2("Failed to send HTTP request, error %S", toString(sendRequestResult).toWideChar());
            if((sendRequestResult == CORE_REMOTE_HOST_CLOSED_CONNECTION)
                && previousResponseReceived && authorizationTypeSwitched && !requestHasAlreadyBeenRepeated)
            {
                TRACE_MESSAGE("Repeating the same request (without authorization switch), because server was reachable, but unexpectedly closed connection");
                requestHasAlreadyBeenRepeated = true;
                continue;
            }

            return sendRequestResult;
        }

        HttpHeader httpHeader;
        const CoreError receiveResponseAndDataResult = receiveResponseAndData(httpHeader);

        const bool needCloseConnection = httpHeader.needCloseConnection(useProxy)
            || (receiveResponseAndDataResult != CORE_NO_ERROR);

        if(needCloseConnection)
        {
            TRACE_MESSAGE2("Closing connection to HTTP server, get file result %S",
                toString(receiveResponseAndDataResult).toWideChar());
            closeSession();
        }

        if(receiveResponseAndDataResult != CORE_NO_ERROR)
        {
            TRACE_MESSAGE2("Failed to receive HTTP response, error %S",
                toString(receiveResponseAndDataResult).toWideChar());

            if((receiveResponseAndDataResult == CORE_REMOTE_HOST_CLOSED_CONNECTION)
                && previousResponseReceived && authorizationTypeSwitched && !requestHasAlreadyBeenRepeated)
            {
                TRACE_MESSAGE("Repeating the same request (without authorization switch), because server was reachable, but unexpectedly closed connection");
                requestHasAlreadyBeenRepeated = true;
                continue;
            }
            return receiveResponseAndDataResult;
        }
        previousResponseReceived = true;
        requestHasAlreadyBeenRepeated = false;

        m_authorizationDriver.authorized(
            // authorization information is reset in case connection is to be closed
            !needCloseConnection
            // authorization success in case file received or successful redirect
            && (httpHeader.isFile()
                 || httpHeader.redirectRequired()
                 || httpHeader.fileNotFound()));

        if(httpHeader.isFile())
        {
            // in case proxy server by some ocassion desided to close
            //  connection after successful file receive event
            if(needCloseConnection)
                m_authorizationDriver.resetNtlmState();

            return CORE_NO_ERROR;
        }

        authorizationTarget = httpHeader.authorizationTarget();
		authorizationWasNeededOnProxy = !proxyAuthorizationHeader.empty();
        lastHttpCode = httpHeader.httpCode();

        // authorization error
        if(httpHeader.authorizationNeeded())
        {
            authorizationWasNeededOnProxy = (authorizationTarget == DownloadProgress::proxyServer);
            if(!switchAuthorization(fileName, relativeUrlPath, httpHeader.authorizationTarget(), proxyAddress,
                authorizationTypeSwitched, ntlmAuthorizationTriedAlready))
            {
                return httpHeader.convertHttpCodeToUpdaterCode();
            }
        }
        // redirect needed
        else if(httpHeader.redirectRequired())
        {
            // protection against infinite loop (according to RFC 2616)
            if(++infiniteRedirectLoop > 2)
            {
                TRACE_MESSAGE2("Infinite redirection loop detected for location '%S'",
                    httpHeader.m_location.toWideChar());
                return CORE_NO_SOURCE_FILE;
            }

            if(httpHeader.m_location.isAbsoluteUri())
                serverAddress.parse(httpHeader.m_location);
            else
            {
                serverAddress.parse(toProtocolPrefix(serverAddress.m_protocol) + serverAddress.m_hostname
                    + serverAddress.m_path + httpHeader.m_location);
                serverAddress.m_path.correctPathDelimiters();
            }

            fileName = serverAddress.m_fileName;
            relativeUrlPath.erase();

            TRACE_MESSAGE3("HTTP Redirect to file '%S' on server %S",
                fileName.toWideChar(), serverAddress.toString().toWideChar());
        }
        // known HTTP results
        else if(httpHeader.resourceUnavailable()
            || httpHeader.fileNotFound())
        {
            return httpHeader.convertHttpCodeToUpdaterCode();
        }
        // retry authorization with other credentials in case Forbidden
         // code received after successful authorization has happened
        else if(httpHeader.retryAuthorization(authorizationWasNeededOnProxy)
            && treat_403_502_httpCodesAs407())
        {
            if(!switchAuthorization(fileName, relativeUrlPath, DownloadProgress::proxyServer,
                proxyAddress, authorizationTypeSwitched, ntlmAuthorizationTriedAlready))
            {
                TRACE_MESSAGE2("Authorization was needed, but error HTTP code '%d' received and switch to next authorization type failed",
                    httpHeader.httpCode());
                return httpHeader.convertHttpCodeToUpdaterCode();
            }
            TRACE_MESSAGE3("Authorization was needed, but error HTTP code '%d' received, try next authorization type '%S'",
                httpHeader.httpCode(),
                toString(m_authorizationDriver.currentAuthorizationType()).toWideChar());
        }
        else
        {
            // processing HTTP code is not implemented
            m_authorizationDriver.resetAuthorizatoinState(proxyAuthorizationMethods);
            return httpHeader.convertHttpCodeToUpdaterCode();
        }
    }

    // Authorization state machine is complex and may contain bug,
     //  that is why protection against infinite loop is implemented
    TRACE_MESSAGE3("Error in HTTP authorization state implementation: credentials '%S', current authorization type '%S'",
        m_authorizationDriver.credentials().toString().toWideChar(),
        toString(m_authorizationDriver.currentAuthorizationType()).toWideChar());
    return CORE_DOWNLOAD_ERROR;
}

void KLUPD::HttpProtocol::fullHttpHeaderReceived(const std::string &httpHeader)
{
}

bool KLUPD::HttpProtocol::treat_403_502_httpCodesAs407()
{
    return false;
}

void KLUPD::HttpProtocol::informAuthorizationFailed(const DownloadProgress::AuthorizationTarget &,
    const Path &fileName, const NoCaseString &serverWithCredentials)
{
    // no default implementation
}
void KLUPD::HttpProtocol::informNameResolved(const Path &name, const std::string &IP)
{
    // no default implementation
}

KLUPD::CoreError KLUPD::HttpProtocol::setupLowLevelConnectionIfNeed(const bool useProxy,
    Address &destination, const AuthorizationTypeList &proxyAuthorizationMethods)
{
    if(m_connected)
    {
        // already connected
        if(m_connectedTo == destination)
            return CORE_NO_ERROR;

        // disconnect first
        closeSession();
        m_authorizationDriver.resetAuthorizatoinState(proxyAuthorizationMethods);
    }

    // reset authrozation state in case destination address changes
    if(m_connectedTo != destination)
        m_authorizationDriver.resetAuthorizatoinState(proxyAuthorizationMethods);

    const CoreError connectResult =
        m_socket.connect(destination.m_hostname.toAscii().c_str(), destination.m_service.toAscii().c_str(), useProxy);
    if(connectResult != CORE_NO_ERROR)
        return connectResult;

    m_connectedTo = destination;
    m_connected = true;
    return CORE_NO_ERROR;
}

void KLUPD::HttpProtocol::closeSession()
{
    if(!m_connected)
        return;

    TRACE_MESSAGE("Connection to HTTP server is closed by updater");
    m_socket.close();   // SO_LINGER with 0 timeout assumed

    m_authorizationDriver.resetNtlmState();

    m_connected = false;
}

KLUPD::CoreError KLUPD::HttpProtocol::receiveResponseAndData(HttpHeader &httpHeader)
{
    bool httpHeaderFullyReceived = false;

    const char *entity = 0;
    size_t total_entity_bytes = 0;

    const size_t httpBufferSize = 65536;
    char httpBuffer[httpBufferSize + 1];
    memset(httpBuffer, 0, httpBufferSize + 1);
    size_t currentOffsetInHttpBuffer = 0;

    while(true)
    {
        CoreError receiveResult = CORE_NO_ERROR;
        const int bytesReceived = m_socket.recv(httpBuffer + currentOffsetInHttpBuffer,
            httpBufferSize - currentOffsetInHttpBuffer, receiveResult);
        currentOffsetInHttpBuffer += bytesReceived;

        // according to RFC 2616, 4.4 point 5: connection close may indicate
         // file transfer completion if Range or Content-Length is not specified
        if(httpHeaderFullyReceived
            && (receiveResult == CORE_REMOTE_HOST_CLOSED_CONNECTION)
            && (httpHeader.m_contentLength == -1))
        {
            TRACE_MESSAGE2("HTTP connection closed by server, no Content-Length field, consider file is obtained, size %d bytes", currentOffsetInHttpBuffer);
            return CORE_NO_ERROR;
        }
        if(receiveResult != CORE_NO_ERROR)
            return receiveResult;

        // size of entity part that is currently in buffer
        size_t entityBytes = 0;
        if(httpHeaderFullyReceived)
        {
            // http header was obtained on previous iteration
            entityBytes = currentOffsetInHttpBuffer;
            entity = httpBuffer;
        }
        else
        {
            // HTTP header has not been obtained yet, check if it is in httpBuffer
            const char *const headerEndMarker = "\r\n\r\n";
            char *const headerLastPosition = strstr(httpBuffer, headerEndMarker);

            if(!headerLastPosition)
            {
                // no end-of-header marker found
                if(httpBufferSize < currentOffsetInHttpBuffer)
                {
                    TRACE_MESSAGE3("Error: response HTTP header can not fit into %d-bytes buffer (already received %d bytes)",
                        httpBufferSize, currentOffsetInHttpBuffer);
                    return CORE_DOWNLOAD_ERROR;
                }

                // continue reading from socket
                continue;
            }

            httpHeaderFullyReceived = true;

            // pass to applicaiton data included into HTTP header
            fullHttpHeaderReceived(std::string(httpBuffer, headerLastPosition + strlen(headerEndMarker)));

            *headerLastPosition = 0;
            TRACE_MESSAGE2("HTTP response received:\n%s", HTTPRequestBuilder::toString(httpBuffer).c_str());

            // Parse HTTP header, data loaded into httpHeader structure
            if(!httpHeader.load(httpBuffer, m_authorizationDriver))
            {
                TRACE_MESSAGE("Failed to parse HTTP response header");
                return CORE_DOWNLOAD_ERROR;
            }

            // it is expected 206 code in case regetting is used
            if(m_regettingPosition && (httpHeader.httpCode() == 200))
            {
                TRACE_MESSAGE("It is expected 206 code in case regetting is used, but 200 code is received");
                return CORE_DOWNLOAD_ERROR;
            }

            // data from socket after the HTTP header
            entityBytes = (httpBuffer + currentOffsetInHttpBuffer)  // pointer to the end of received bytes
                - (headerLastPosition + strlen(headerEndMarker));   // pointer to the data (begin of the file)
            entity = entityBytes ? headerLastPosition + strlen(headerEndMarker) : 0;
        }

        total_entity_bytes += entityBytes;

        if(httpHeader.isFile()
            // only header has been received, but data is not received yet
            && entityBytes)
        {
            const CoreError saveDataToFileResult = dataReceived(reinterpret_cast<const unsigned char *>(entity), entityBytes);
            if(!isSuccess(saveDataToFileResult))
            {
                TRACE_MESSAGE2("Failed to write data obtained from HTTP Server, result %S",
                    toString(saveDataToFileResult).toWideChar());
                return saveDataToFileResult;
            }
            m_regettingPosition += entityBytes;
        }

        //////////////////////////////////////////////////////////////////////////
        /// check if complete file is downloaded

        // Content-Length header presents
        if(httpHeader.m_contentLength == -1)
        {
            TRACE_MESSAGE("HTTP chunk received (header does not contain the content-length field), continue receiving message until connection is closed by remote peer");
        }
        else if(static_cast<size_t>(httpHeader.m_contentLength) <= total_entity_bytes)
        {
            TRACE_MESSAGE2("HTTP message successfully received, content length = %d", httpHeader.m_contentLength);
            return CORE_NO_ERROR;
        }

        currentOffsetInHttpBuffer = 0;
        entityBytes = 0;
    }
}

bool KLUPD::HttpProtocol::switchAuthorization(const Path &fileName, const Path &relativeUrlPath,
                const DownloadProgress::AuthorizationTarget &authorizationTarget, const Address &proxyAddress,
                bool &authorizationTypeSwitched,
                bool &ntlmAuthorizationTriedAlready)
{
    authorizationTypeSwitched = false;
    const AuthorizationType currentAuthorizationType = m_authorizationDriver.currentAuthorizationType();

    if(!m_downloadProgress.authorizationTargetEnabled(authorizationTarget))
    {
        TRACE_MESSAGE2("Failed to switch authorization by product '%s'", DownloadProgress::toString(authorizationTarget).c_str());
        return false;
    }

    // authorization needed
    bool needUpdateCredentials = false;
    const bool switchedToNextAuthorizationSuccess = m_authorizationDriver.switchToNextAuthorization(needUpdateCredentials,
        ntlmAuthorizationTriedAlready);

    // report authorization failed
    if(!switchedToNextAuthorizationSuccess || needUpdateCredentials)
    {
        if(m_authorizationDriver.credentials().userName().empty())
            informAuthorizationFailed(authorizationTarget, relativeUrlPath + fileName, proxyAddress.BuildURI().toWideChar());
        else
        {
            informAuthorizationFailed(authorizationTarget, relativeUrlPath + fileName,
                m_authorizationDriver.credentials().userName() + L"@" + proxyAddress.BuildURI().toWideChar());
        }
    }

    if(!switchedToNextAuthorizationSuccess)
        return false;

    if(needUpdateCredentials)
    {
        Credentials newCredentials = m_authorizationDriver.credentials();
        // proxy authorization failed, asking credentials to authenticate on proxy
        if(!m_downloadProgress.requestAuthorizationCredentials(newCredentials))
        {
            TRACE_MESSAGE("User cancelled HTTP authorization");
			// Treat cancel as "don't use credentials at all"
            //m_authorizationDriver.resetNtlmState();
			newCredentials.cancel( true );
			m_authorizationDriver.credentials( newCredentials );
            m_authorizationDriver.currentAuthorizationType(noAuthorization);

            // reset authorization to initial state to keep object consistency
            bool needUpdateCredentials = false;
            bool ntlmAuthorizationTriedAlready = false;
            m_authorizationDriver.switchToNextAuthorization(needUpdateCredentials, ntlmAuthorizationTriedAlready);
            return false;
        }
        // no credentials specified
        if(newCredentials.userName().empty())
        {
            TRACE_MESSAGE("Credentials to authenticate on proxy were asked, but not obtained from Product: user name is empty");
            return false;
        }

        TRACE_MESSAGE2("Credentials to authenticate on proxy were asked, user name provided is '%S'",
            newCredentials.userName().toWideChar());

        m_authorizationDriver.credentials(newCredentials);
    }

    authorizationTypeSwitched = currentAuthorizationType != m_authorizationDriver.currentAuthorizationType();
    return true;
}
