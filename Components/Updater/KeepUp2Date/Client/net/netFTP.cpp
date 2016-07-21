#include "netTransfer.h"
#include "stdSocket.h"

#include "../helper/updaterStaff.h"

const char *KLUPD::FtpProtocol::s_szPortFtpDefault = "21";

KLUPD::FtpProtocol::FtpProtocol(const size_t timeoutSeconds, const FtpMode &ftpMode, const NoCaseString &identity,
                                DownloadProgress &downloadProgress, Log *log)
 : m_connected(false),
   m_bExtendedModeSupported(true),
   m_controlSocket(timeoutSeconds, downloadProgress, log),
   m_dataSocket(timeoutSeconds, downloadProgress, log),
   m_timeoutSeconds(timeoutSeconds),
   m_ftpMode(ftpMode),
   m_identity(identity),
   m_downloadProgress(downloadProgress),
   pLog(log)
{
}

KLUPD::FtpProtocol::~FtpProtocol()
{
    closeSession();
}

bool KLUPD::FtpProtocol::isResponseCodeToIgnore(const int code)const
{
    return
        // command is not recognized
        (code == 0)

        // 200 Command okay
        || (code == 200)

        // 220 Service ready for new user
        || (code == 220)

        // 230 User logged in, proceed
        || (code == 230)

        // 225 Data connection open; no transfer in progress
        || (code == 225)

        // 226 Closing data connection; requested file action successful (for example, file transfer or file abort.)
        || (code == 226)

        // 331 User name okay, need password
        || (code == 331)

        // 332 Need account for login
        || (code == 332);
}

KLUPD::CoreError KLUPD::FtpProtocol::getSingleResponse(std::string &responseString)
{
    const std::string endLineMarker = "\r\n";
     // FTP code is 3-symbols string together with trailing space ' ' or minus sybmol '-'
      // 4 is NNN for FTP code plus trailing sybmol
    const size_t ftpCodeWithTrailingSymbolSize = 4;

    responseString = m_commandQueue;

    // until the full response to FTP command is received
    std::string::size_type markerPosition = 0;
    while(true)
    {
        markerPosition = m_commandQueue.find(endLineMarker);
        if(markerPosition != std::string::npos)
            break;

        char response[MAX_NET_MESG_SIZE + 1];
        CoreError receiveResult = CORE_NO_ERROR;
        const int bytesRead = m_controlSocket.recv(response, MAX_NET_MESG_SIZE, receiveResult);
        if(receiveResult != CORE_NO_ERROR)
        {
            TRACE_MESSAGE2("Check FTP response failed, result '%S'",
                toString(receiveResult).toWideChar());
            return receiveResult;
        }
        response[bytesRead] = '\0';
        m_commandQueue += response;
    }

    responseString.assign(m_commandQueue.c_str(), markerPosition);
    m_commandQueue.erase(0, markerPosition + endLineMarker.size());

    if(responseString.size() < ftpCodeWithTrailingSymbolSize)
    {
        TRACE_MESSAGE3("Skip bad-sized (%d) FTP response received from server: '%s'",
            responseString.size(), responseString.c_str());
        return getSingleResponse(responseString);
    }

    TRACE_MESSAGE2("Response from FTP server: '%s'", responseString.c_str());

    // FTP reply code must be 3 numeric symbols and start right from the line begin
    if(!std::isdigit(responseString[0], std::locale::classic())
        || !std::isdigit(responseString[1], std::locale::classic())
        || !std::isdigit(responseString[2], std::locale::classic()))
    {
        TRACE_MESSAGE2("Skip multiline or malformed FTP response line: '%s'", responseString.c_str());
        return getSingleResponse(responseString);
    }

    const int responseCode = atoi(responseString.c_str());
    // not recognized code
    if(!responseCode)
    {
        TRACE_MESSAGE2("Skip unrecognized FTP response line: '%s'", responseString.c_str());
        return getSingleResponse(responseString);
    }

    // read multi-line FTP reply
     // Example from RFC 959:
      // 123-First line
      // Second line
      //  234 A line beginning with numbers must start with neutral symbol
      // 123 The last line
    const char multilineIndicatorSymbol = '-';
    if(responseString[ftpCodeWithTrailingSymbolSize - 1] == multilineIndicatorSymbol)
    {
        TRACE_MESSAGE2("Multiline FTP response detected: '%s'", responseString.c_str());
        std::string multilineResponseString;
        const CoreError readMultilineResponse = getSingleResponse(multilineResponseString);
        if(!isSuccess(readMultilineResponse))
            return readMultilineResponse;

        // trailing multiline code must coinside with code received in first line
        const int multilineTrailingCode = atoi(multilineResponseString.c_str());
        if(multilineTrailingCode != responseCode)
        {
            TRACE_MESSAGE("Trailing FTP multiline code '%d' does not coinside with code received in first line '%d'");
            return CORE_DOWNLOAD_ERROR;
        }
    }


    // convert know FTP code into Updater codes
    if(responseCode >= 400)
    {
        if(responseCode == 550)
            return CORE_NO_SOURCE_FILE;
        if(responseCode == 530)
            return CORE_FTP_AUTH_ERROR;

        return CORE_DOWNLOAD_ERROR;
    }

    return CORE_NO_ERROR;
}

KLUPD::CoreError KLUPD::FtpProtocol::checkResponse(std::string &responseString)
{
    // skipping all non-interesting duplicate codes in cycle, until interesting code is received
    while(true)
    {
        const CoreError getSingleResponseResult = getSingleResponse(responseString);
        if(!isSuccess(getSingleResponseResult))
            return getSingleResponseResult;

        const int singleResponseCode = atoi(responseString.c_str());
        if(isResponseCodeToIgnore(singleResponseCode))
            continue;

        return CORE_NO_ERROR;
    }
}

KLUPD::CoreError KLUPD::FtpProtocol::waitFileTransferred()
{
    // skipping all non-interesting duplicate codes in cycle, until interesting code is received
    while(true)
    {
        std::string responseString;
        const CoreError getSingleResponseResult = getSingleResponse(responseString);
        if(!isSuccess(getSingleResponseResult))
            return getSingleResponseResult;

        const int singleResponseCode = atoi(responseString.c_str());

         // 226 Closing data connection.
          // Requested file action successful (for example, file transfer or file abort).
        if(singleResponseCode == 226)
            return CORE_NO_ERROR;

        // 250 Requested file action okay, completed.
        if(singleResponseCode == 250)
            return CORE_NO_ERROR;

        if(isResponseCodeToIgnore(singleResponseCode))
            continue;

        return CORE_NO_ERROR;
    }
}

KLUPD::CoreError KLUPD::FtpProtocol::sendFtpCommand(const std::string &command, const std::string &additionalParameter)
{
    std::string commandBuffer = command;
    if(!additionalParameter.empty())
    {
        commandBuffer += ' ';
        commandBuffer += additionalParameter;
    }

    TRACE_MESSAGE2("Sending FTP command '%s'", commandBuffer.c_str());

    commandBuffer += '\r';
    commandBuffer += '\n';

    const CoreError sendResult = m_controlSocket.send(commandBuffer.c_str(), commandBuffer.size());
    if(sendResult != CORE_NO_ERROR)
    {
        TRACE_MESSAGE3("Failed to send FTP command '%s', send result %S",
            (command + " " + additionalParameter).c_str(), toString(sendResult).toWideChar());
    }

    return sendResult;
}

KLUPD::CoreError KLUPD::FtpProtocol::connectToFtp()
{
    std::string strService = !m_addressConnectedTo.m_service.empty() ? m_addressConnectedTo.m_service.toAscii() : s_szPortFtpDefault;
    TRACE_MESSAGE3("Connecting to FTP server '%s:%s'", m_addressConnectedTo.m_hostname.toAscii().c_str(), strService.c_str());
    
    const CoreError connectResult =
        m_controlSocket.connect( m_addressConnectedTo.m_hostname.toAscii().c_str(),
                                 m_addressConnectedTo.m_service.toAscii().c_str(), false);
    if(connectResult != CORE_NO_ERROR)
    {
        TRACE_MESSAGE2("Failed to connect to FTP server, result '%S'",
            toString(connectResult).toWideChar());
        return connectResult;
    }

    std::string checkIfReadyString;
    const CoreError checkIfReadyResult = getSingleResponse(checkIfReadyString);
    if(checkIfReadyResult != CORE_NO_ERROR)
        return checkIfReadyResult;

    const int checkIfReadyCode = atoi(checkIfReadyString.c_str());
    if(checkIfReadyCode != 220)
    {
        TRACE_MESSAGE2("Error: expected code '220 Service ready for new user' is not received, result '%s'", checkIfReadyString.c_str());
        return CORE_DOWNLOAD_ERROR;
    }

    m_connected = true;
    m_bExtendedModeSupported = true;    // reset extended ftp mode any time we connect to a server
    return CORE_NO_ERROR;
}

KLUPD::CoreError KLUPD::FtpProtocol::checkLoginResult()
{
    while(true)
    {
        std::string singleResponseString;
        const CoreError receiveResponseResult = getSingleResponse(singleResponseString);
        if(!isSuccess(receiveResponseResult))
            return receiveResponseResult;

        const int responseCode = atoi(singleResponseString.c_str());

        // 230 User logged in, proceed
        if(responseCode == 230)
            return CORE_NO_ERROR;

        // skip duplicate codes, and successful codes
        if(isResponseCodeToIgnore(responseCode))
            continue;

        TRACE_MESSAGE2("Error: un-expected code for login to FTP '%d'", responseCode);
        return CORE_DOWNLOAD_ERROR;
    }
}

KLUPD::CoreError KLUPD::FtpProtocol::login()
{
    const NoCaseString userName = m_addressConnectedTo.m_credentials.userName().empty()
        ? L"anonymous" : m_addressConnectedTo.m_credentials.userName();

    const CoreError sendUserResult = sendFtpCommand("USER", userName.toAscii());
    if(sendUserResult != CORE_NO_ERROR) 
        return sendUserResult;
    
    // FTP-server answered "331 - password required for user ..."
    NoCaseString password;
        // empty password may be set for user
    if(!m_addressConnectedTo.m_credentials.userName().empty()
        || !m_addressConnectedTo.m_credentials.password().empty())
    {
        password = m_addressConnectedTo.m_credentials.password();
    }
    else
    {
// TODO: what for the '@' symbol is appended ?
        password = m_identity + L"@";
    }

    const CoreError sendPasswordResult = sendFtpCommand("PASS", password.toAscii());
    if(sendPasswordResult != CORE_NO_ERROR)
    {
        TRACE_MESSAGE2("Error on command FTP command 'PASS', result '%S'",
            toString(sendPasswordResult).toWideChar());
        return sendPasswordResult;
    }

    return checkLoginResult();
}

KLUPD::CoreError KLUPD::FtpProtocol::openSession()
{
    CoreError result = connectToFtp();
    if( isSuccess(result) )
    {
        result = login();
        if( isSuccess(result) )
        {
            result = sendFtpCommand("TYPE", "I");
            if(result == CORE_NO_ERROR)
            {
                std::string strResponse;
                result = getSingleResponse(strResponse);
                if(result == CORE_NO_ERROR)
                {
                    if( atoi(strResponse.c_str()) != 200 )
                        result = CORE_DOWNLOAD_ERROR;
                }
            }
        }
    }

    if( !isSuccess(result) )
        closeSession();

    return result;
}

KLUPD::CoreError KLUPD::FtpProtocol::_EnterPassiveMode(bool &passiveConnectFailed)
{
    passiveConnectFailed = false;

    const CoreError sendPasvCommandResult = sendFtpCommand("PASV");
    if(sendPasvCommandResult != CORE_NO_ERROR)
        return sendPasvCommandResult;
    
    std::string responseString;
    const CoreError checkResponseResult = checkResponse(responseString);
    if(checkResponseResult != CORE_NO_ERROR)
    {
        TRACE_MESSAGE2("FTP command 'PASV' failed, result '%S'",
            toString(checkResponseResult).toWideChar());
        return checkResponseResult;
    }

    std::string::size_type position = responseString.find("(");
    if(position == std::string::npos)
    {
        TRACE_MESSAGE("Expected symbol '(' in FTP response for 'PASV' command");
        return CORE_DOWNLOAD_ERROR; 
    }

    std::string addressString = responseString.c_str() + position + 1;
    position = addressString.rfind(",");
    if(position == std::string::npos)
    {
        TRACE_MESSAGE("Expected symbol ',' in FTP response for 'PASV' command");
        return CORE_DOWNLOAD_ERROR; 
    }

    position = addressString.rfind(",", position - 1);
    if(position == std::string::npos)
    {
        TRACE_MESSAGE("Expected 2 ',' symbols in FTP response for 'PASV' command");
        return CORE_DOWNLOAD_ERROR; 
    }

    const std::string portString = addressString.c_str() + position + 1;
    addressString.resize(position);
    // replacing ',' symbols with '.' in string
    for(std::string::iterator addressStringIter = addressString.begin(); addressStringIter != addressString.end(); ++addressStringIter)
    {
        if(*addressStringIter == ',')
            *addressStringIter = '.';
    }
    

    // converting port string to number
    int i = 0;
    int j = 0;
    if(sscanf(portString.c_str(), "%d,%d", &i, &j) != 2)
    {
        TRACE_MESSAGE("Failed to extract port string from 'PASV' FTP command response");
        return CORE_DOWNLOAD_ERROR;
    }
    unsigned short port = static_cast<unsigned short>((i << 8) + j);
        
    if(addressString.empty() || (addressString.size() > 15))
    {
        TRACE_MESSAGE2("Address retrieved has invalid length %d", addressString.size());
        return CORE_DOWNLOAD_ERROR;
    }

    TRACE_MESSAGE3("Using IP address for data connection: '%s:%d'", addressString.c_str(), port);

    std::ostringstream strService;
    strService << port;

    const CoreError connectResult = m_dataSocket.connect(addressString.c_str(), strService.str().c_str(), false);
    if(connectResult != CORE_NO_ERROR)
    {
        TRACE_MESSAGE2("Unable to establish data connection to %S", m_addressConnectedTo.m_hostname.toWideChar());
        passiveConnectFailed = true;
        return connectResult;
    }
    
    return CORE_NO_ERROR;
}

bool IsValidDelimiter(char del) {return 33 <= del && del <= 126; }

KLUPD::CoreError KLUPD::FtpProtocol::_EnterPassiveModeEx(bool &passiveConnectFailed)
{
    CoreError result = CORE_DOWNLOAD_ERROR;

    if(m_bExtendedModeSupported)
    {
        int nProtoFamily = m_controlSocket.GetProtoFamily();

        result = sendFtpCommand("EPSV");
        if(result == CORE_NO_ERROR)
        {
            std::string response;
            result = getSingleResponse(response);
            if( !response.empty() )
            {
                int code = atoi(response.c_str());
                if( code == 229 )   // entered extended passive mode
                {
                    std::string strPort;
                    const char* p = response.c_str();
                    while(*p != '\0' && *p != '(') ++p;
                    char del = *++p;    // delimiter
                    if(IsValidDelimiter(del) && *++p == del && *++p == del)
                    {
                        const char* p2 = ++p;
                        while(*p != '\0' && *p != del) ++p;
                        if(*p == del)
                            strPort.assign(p2, p);
                    }
                    
                    if( !strPort.empty() )
                        result = m_dataSocket.connect(  m_addressConnectedTo.m_hostname.toAscii().c_str(),
                                                        strPort.c_str(), false, nProtoFamily);
                }
                else if( code == 500 )  // server has not understood the command
                {
                    m_bExtendedModeSupported = false;   // disable extended mode
                    TRACE_MESSAGE("FTP server does not support extended mode, consider using basic mode");
                }
            }
        }
    }

    if(result != CORE_NO_ERROR)
        result = _EnterPassiveMode(passiveConnectFailed);

    return result;
}

KLUPD::CoreError KLUPD::FtpProtocol::_EnterActiveMode(Socket *server_sock)
{
    std::string strHost;
    std::string strService;
    if(!m_controlSocket.getSockName(strHost, strService))
    {
        TRACE_MESSAGE("Failed to prepare active FTP data connection, unable to retrieve the data device address and port number");
        return CORE_DOWNLOAD_ERROR;
    }

    if(!server_sock->bind(strHost.c_str(), NULL, m_controlSocket.GetProtoFamily()))
    {
        TRACE_MESSAGE("Failed to prepare active FTP data connection, unable to bind socket");
        return CORE_DOWNLOAD_ERROR;
    }
    
    std::string strHost2;
    std::string strService2;
    if(!server_sock->getSockName(strHost2, strService2))
    {
        TRACE_MESSAGE("Failed to prepare active FTP data connection, unable to retrieve the server device address and port number");
        return CORE_DOWNLOAD_ERROR;
    }
    
    if(!server_sock->listen())
    {
        TRACE_MESSAGE2("Failed to prepare active FTP data connection, unable to listen on server socket on port %s", strService2.c_str());
        return CORE_DOWNLOAD_ERROR;
    }

    std::replace_if(strHost.begin(), strHost.end(), std::bind2nd(std::equal_to<char>(), '.'), ',');

    unsigned int port = atoi( strService2.c_str() );

    std::ostringstream requestStream;
    requestStream.imbue(std::locale::classic());
    const unsigned short i = static_cast<unsigned short>(port / 256);
    const unsigned short j = static_cast<unsigned short>(port % 256);
    requestStream << "PORT " << strHost.c_str() << "," << i << "," << j;
    
    const CoreError sendPortCommandResult = sendFtpCommand(requestStream.str());
    if(sendPortCommandResult != CORE_NO_ERROR)
        return sendPortCommandResult;
    
    std::string responseString;
    const CoreError checkResponseResult = getSingleResponse(responseString);
    if(checkResponseResult != CORE_NO_ERROR)
    {
        TRACE_MESSAGE2("FTP command 'PORT' failed, result '%S'",
            toString(checkResponseResult).toWideChar());
        return checkResponseResult;
    }

    // ACTIVE data connection prepared
    return CORE_NO_ERROR;
}

KLUPD::CoreError KLUPD::FtpProtocol::_EnterActiveModeEx(Socket *server_sock)
{
    CoreError result = CORE_DOWNLOAD_ERROR;

    if(m_bExtendedModeSupported)
    {
        result = CORE_NO_ERROR;

        std::string strHost, strService;
        if( result == CORE_NO_ERROR &&
            !m_controlSocket.getSockName(strHost, strService) )
        {
            TRACE_MESSAGE("Failed to prepare extended active FTP data connection, unable to retrieve the data device address and port number");
            result = CORE_DOWNLOAD_ERROR;
        }

        int nPF = m_controlSocket.GetProtoFamily();

        if( result == CORE_NO_ERROR &&
            !server_sock->bind(strHost.c_str(), NULL, nPF) )
        {
            TRACE_MESSAGE("Failed to prepare extended active FTP data connection, unable to bind socket");
            result = CORE_DOWNLOAD_ERROR;
        }

        std::string strHost2, strService2;
        if( result == CORE_NO_ERROR &&
            !server_sock->getSockName(strHost2, strService2) )
        {
            TRACE_MESSAGE("Failed to prepare extended active FTP data connection, unable to retrieve the server device address and port number");
            result = CORE_DOWNLOAD_ERROR;
        }

        if( result == CORE_NO_ERROR &&
            !server_sock->listen())
        {
            TRACE_MESSAGE2("Failed to prepare extended active FTP data connection, unable to listen on server socket on port %s", strService2.c_str());
            return CORE_DOWNLOAD_ERROR;
        }

        if(result == CORE_NO_ERROR)
        {
            int nNetPrt = nPF == PF_INET ? 1 : nPF == PF_INET6 ? 2 : 0;
            if(nNetPrt != 0)
            {
                std::ostringstream requestStream;
                requestStream.imbue(std::locale::classic());
                requestStream << "EPRT " << '|' << nNetPrt << '|' << strHost2 << '|' << strService2 << '|';

                result = sendFtpCommand(requestStream.str());
                if(result == CORE_NO_ERROR)
                {
                    std::string strResponse;
                    result = getSingleResponse(strResponse);
                    if( !strResponse.empty() )
                    {
                        int code = atoi(strResponse.c_str());
                        if(code != 200)
                            result = CORE_DOWNLOAD_ERROR;
                        if( code == 500 )  // server has not understood the command
                        {
                            m_bExtendedModeSupported = false;   // disable extended mode
                            TRACE_MESSAGE("FTP server does not support extended mode, consider using basic mode");
                        }
                    }
                }
                if(result != CORE_NO_ERROR)
                    TRACE_MESSAGE("Failed to prepare extended active FTP data connection: FTP request failed");
            }
            else
            {
                TRACE_MESSAGE2("Failed to prepare extended active FTP data connection: unknown protocol family used: %d", nPF);
                result = CORE_DOWNLOAD_ERROR;
            }
        }
    }

    if(result != CORE_NO_ERROR)
    {
        server_sock->close();
        result = _EnterActiveMode(server_sock);
    }

    return result;
}

KLUPD::CoreError KLUPD::FtpProtocol::startDataTransfer(const Path &sourceFullPathIn,
                                           const Path &targetFullPath,
                                           size_t &regettingPosition)
{
    regettingPosition = LocalFile(targetFullPath).size();
    
    // server socket object for accept data connection from ftp server in active data transfer mode
    Socket server_sock(m_timeoutSeconds, m_downloadProgress, pLog);

    // passive FTP mode
    if((m_ftpMode == passiveFtp)
        || (m_ftpMode == tryActiveFtpIfPassiveFails))
    {
        bool passiveConnectFailed = false;
        CoreError EnterPassiveModeResult = _EnterPassiveModeEx(passiveConnectFailed);
        if(EnterPassiveModeResult != CORE_NO_ERROR)
        {
            if(!passiveConnectFailed)
            {
                TRACE_MESSAGE2("Failed to prepare passive FTP data connection, result '%S'",
                    toString(EnterPassiveModeResult).toWideChar());
                return EnterPassiveModeResult;
            }

            TRACE_MESSAGE2("Switch to active FTP mode, because connection to FTP server in passive mode failed, result '%S'",
                toString(EnterPassiveModeResult).toWideChar());
            m_ftpMode = activeFtp;
        }
    }

    // active FTP mode
    if(m_ftpMode == activeFtp)
    {
        const CoreError EnterActiveModeResult = _EnterActiveModeEx(&server_sock);
        if(EnterActiveModeResult != CORE_NO_ERROR)
        {
            TRACE_MESSAGE2("Failed to prepare active FTP data connection, result '%S'",
                toString(EnterActiveModeResult).toWideChar());
            return EnterActiveModeResult;
        }
    }

    // use regetting feature to download from specified position
    if(regettingPosition)
    {
        std::ostringstream restCommandStream;
        restCommandStream.imbue(std::locale::classic());
        restCommandStream << "REST " << regettingPosition;
        const CoreError sendRestCommandResult = sendFtpCommand(restCommandStream.str());
        if(sendRestCommandResult != CORE_NO_ERROR)
            return sendRestCommandResult;
        
        std::string restResponseString;
        const CoreError checkResponseResult = checkResponse(restResponseString);
        if(checkResponseResult != CORE_NO_ERROR)
            regettingPosition = 0;

        const int restResponseCode = atoi(restResponseString.c_str());
        // 350 Requested file action pending further information
        if(restResponseCode != 350)
            regettingPosition = 0;
    }

    // removing leading '/'
    Path sourceFullPath = sourceFullPathIn;
    sourceFullPath.removeSingleLeadingDelimeter();

    const CoreError sendRetrCommandResult = sendFtpCommand("RETR", sourceFullPath.toAscii());
    if(sendRetrCommandResult != CORE_NO_ERROR)
        return sendRetrCommandResult;

    std::string retrResponseString;
    const CoreError checkResponseResult = checkResponse(retrResponseString);
    if(checkResponseResult != CORE_NO_ERROR)
    {
        TRACE_MESSAGE2("Failed to check response on FTP command 'RETR', result '%S'",
            toString(checkResponseResult).toWideChar());
        return checkResponseResult;
    }
    
    if(m_ftpMode == activeFtp)
    {
        time_t timeElapsed = 0;
        time_t start_time = time(0);
        bool incomingConnectionReceived = false;
        while((unsigned int)timeElapsed < m_timeoutSeconds)
        {
            incomingConnectionReceived = server_sock.accept(m_dataSocket);
            const int lastError = WSAGetLastError();
            if(incomingConnectionReceived || !server_sock.wouldBlockError(lastError))
                break;

            // sleep 1 ms and repeat attempt to accept incoming network connection
            usleep(1000);
            
            if(m_downloadProgress.checkCancel())
                return CORE_CANCELLED;

            timeElapsed = time(0) - start_time;
        }
        if(!incomingConnectionReceived)
        {
            TRACE_MESSAGE3("Error: unable to accept server connection to local port. Waited %d of %d seconds",
                timeElapsed, m_timeoutSeconds);
            return CORE_DOWNLOAD_ERROR;
        }
    }
    return CORE_NO_ERROR;
}

void KLUPD::FtpProtocol::closeSession()
{
    m_dataSocket.close();

    if(m_connected)
    {
        sendFtpCommand("QUIT");
        m_connected = false;
    }
    m_controlSocket.close();
}

KLUPD::CoreError KLUPD::FtpProtocol::initSession(const Address &address)
{
    // check if address changed and closing session to previous source if changed
    if(address != m_addressConnectedTo)
        closeSession();
    m_addressConnectedTo = address;

    if(m_connected)
        return CORE_NO_ERROR;

    // initialization ftp control connection
    const CoreError openSessionResult = openSession();
    if(openSessionResult != CORE_NO_ERROR)
    {
        TRACE_MESSAGE3("Unable to open ftp session to '%S', result '%S'",
            m_addressConnectedTo.m_hostname.toWideChar(), toString(openSessionResult).toWideChar());
        return openSessionResult;
    }
    return CORE_NO_ERROR;
}

KLUPD::CoreError KLUPD::FtpProtocol::getFileImplementation(const Path &sourceFullPath, const Path &targetFullPath)
{
    size_t regettingPosition = 0;
    const CoreError startDataTransferResult = startDataTransfer(sourceFullPath, targetFullPath, regettingPosition);
    if(startDataTransferResult != CORE_NO_ERROR)
    {
        m_dataSocket.close();
        TRACE_MESSAGE4("FTP get file error: start data transfer failed on file '%S', size %d, error %S",
            sourceFullPath.toWideChar(), regettingPosition, toString(startDataTransferResult).toWideChar());
        return startDataTransferResult;
    }

    // open file handler for writing
    AutoStream writeStream(pLog);
    const CoreError openWriteFileResult = writeStream.open(targetFullPath, regettingPosition ? L"r+bc" : L"wbc");
    if(!isSuccess(openWriteFileResult))
    {
        TRACE_MESSAGE3("FTP get file error: failed to open target file '%S', result '%S'",
            targetFullPath.toWideChar(), toString(openWriteFileResult).toWideChar());
        return openWriteFileResult;
    }

    // seek to offset
    if(regettingPosition
        && (fseek(writeStream.stream(), regettingPosition, SEEK_CUR) != 0))
    {
        const int lastError = errno;
        TRACE_MESSAGE4("FTP get file error: unable to seek to %d position on file '%S', last error '%S'",
            regettingPosition, targetFullPath.toWideChar(), errnoToString(lastError, posixStyle).toWideChar());
        return lastErrorToUpdaterFileErrorCode(lastError);
    }

    unsigned long size = 0;

    // file downloading loop
    CoreError fileTransferResult = CORE_NO_ERROR;
    while(true)
    {
        char pbuffer[MAX_IO_BUFFER_SIZE + 1];
        memset(pbuffer, 0, MAX_IO_BUFFER_SIZE + 1);
        
        const int bytesReceived = m_dataSocket.recv(pbuffer, MAX_IO_BUFFER_SIZE, fileTransferResult);

        // FTP data connection is closed by server that indicate that file received
        if(fileTransferResult == CORE_REMOTE_HOST_CLOSED_CONNECTION)
        {
            fileTransferResult = waitFileTransferred();
            if(fileTransferResult != CORE_NO_ERROR)
            {
                TRACE_MESSAGE3("Error: FTP data connection closed by remote peer for file '%S', but via control connection success is not obtained, result '%S'",
                    sourceFullPath.toWideChar(), toString(fileTransferResult).toWideChar());
            }

            // file obtaining, break loop
            break;
        }
        
        if(fileTransferResult != CORE_NO_ERROR)
        {
            TRACE_MESSAGE3("Failed to get file from FTP source '%S', result '%S'",
                sourceFullPath.toWideChar(), toString(fileTransferResult).toWideChar());
            // file obtaining, break loop
            break;
        }

        // inform application about number of downloaded bytes
        if(1 != fwrite(pbuffer, bytesReceived, 1, writeStream.stream()))
        {
            const int lastError = errno;
            TRACE_MESSAGE3("FTP get file error: write error occured to file '%S', last error '%S'",
                sourceFullPath.toWideChar(), errnoToString(lastError, posixStyle).toWideChar());

            fileTransferResult = lastErrorToUpdaterFileErrorCode(lastError);

            // file obtaining, break loop
            break;
        }
        size += bytesReceived;
    }

    m_dataSocket.close();

    // close file handler
    return fileTransferResult;
}

KLUPD::CoreError KLUPD::FtpProtocol::getFile(const Address &address, const Path &sourceFullPath, const Path &targetFullPath)
{
    const CoreError initSessionResult = initSession(address);
    if(initSessionResult != CORE_NO_ERROR)
    {
        TRACE_MESSAGE2("FTP get file error: initing ftp session, error %d", initSessionResult);
        return initSessionResult;
    }

    const CoreError getFileResult = getFileImplementation(sourceFullPath, targetFullPath);
    if((getFileResult != CORE_NO_ERROR)
        && (getFileResult != CORE_NO_SOURCE_FILE))
    {
        closeSession();
    }

    return getFileResult;
}
