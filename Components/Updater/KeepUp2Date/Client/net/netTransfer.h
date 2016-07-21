#ifndef NETTRANSFER_H_INCLUDED_0A12C2BD_08EB_4492_9D4B_ECE417A49368
#define NETTRANSFER_H_INCLUDED_0A12C2BD_08EB_4492_9D4B_ECE417A49368

#include "../helper/stdinc.h"
#include "stdSocket.h"

#define MAX_NET_MESG_SIZE 1024
#define MAX_IO_BUFFER_SIZE 102400

namespace KLUPD {

// class to deal with network transfer through FTP protocol
class KAVUPDCORE_API FtpProtocol : public Socket::Initializer
{
public:
    explicit FtpProtocol(const size_t timeoutSeconds, const FtpMode &, const NoCaseString &identity,
        DownloadProgress &, Log *);
    ~FtpProtocol();

	// provide ability to force close connection thus reduce load on server
    void closeSession();

	// get file from server
    CoreError getFile(const Address &, const Path &sourceFullPath, const Path &targetFullPath);

private:
    // disable copy operations
    FtpProtocol &operator=(const FtpProtocol &);
    FtpProtocol(const FtpProtocol &);

    // check if code can be skipped. For duplicate and FTP successful codes
    bool isResponseCodeToIgnore(const int code)const;

    // get single code from FTP server
    CoreError getSingleResponse(std::string &responseString);

    // check ftp server's response and save it, if requested. Skip informative FTP codes
	// responseCode [out] - FTP response obtained from server
	// return indicates whether response code indicates FTP error or some other error while getting FTP response occured
    CoreError checkResponse(std::string &responseString);

    // check whether control ftp session is set and open it, if required
	// return result code
    CoreError initSession(const Address &);

    // establish connection to FTP server
    CoreError connectToFtp();

    // skip duplicate codes until '230 User logged in, proceed' is obtained
    CoreError checkLoginResult();

    // login to FTP server
    CoreError login();

    // wait till file transfer finished
    CoreError waitFileTransferred();


    // Start control ftp session: log in to server using either
	//  anonymous or specified user and password and set binary mode
	CoreError openSession();

	// Prepare active ftp data transfer mode (starts listening socket)
    CoreError _EnterActiveMode(Socket *server);
    CoreError _EnterActiveModeEx(Socket *server);
	
	// Prepare passive ftp data transfer mode (connects to server's listening socket)
    // passiveConnectFailed [out] - indicates if passive connection failed (active FTP connection still may work)
    CoreError _EnterPassiveMode(bool &passiveConnectFailed);
    CoreError _EnterPassiveModeEx(bool &passiveConnectFailed);

    // send command which does not need reply
    CoreError sendFtpCommand(const std::string &command, const std::string &additionalParameter = std::string());

    // start data session, i.e. get data socket ready for data reading
    CoreError startDataTransfer(const Path &sourceFullPath, const Path &fullTargetPath, size_t &reggetingOffset);

    CoreError getFileImplementation(const Path &sourceFullPath, const Path &targetFullPath);

	
    // stack to store commands in case more then 1 command came in single receive operation
    std::string m_commandQueue;

    // current connection state
    bool m_connected;
    Address m_addressConnectedTo;
    bool m_bExtendedModeSupported;  // shows whether extended ftp mode is supported by server (true by default)

    // FTP control connection socket
    Socket m_controlSocket;
    // FTP data connection socket
    Socket m_dataSocket;

    const size_t m_timeoutSeconds;
    FtpMode m_ftpMode;
    const KLUPD::NoCaseString m_identity;
    DownloadProgress &m_downloadProgress;
    Log *pLog;

    static const char *s_szPortFtpDefault;
};

class FileTransferProtocol
{
public:
    explicit FileTransferProtocol(DownloadProgress &, Log *);

    CoreError getFile(const Path &sourceFullPath, const Path &targetFullPath);
    
private:
    // disable copy operations
    FileTransferProtocol &operator=(const FileTransferProtocol &);
    FileTransferProtocol(const FileTransferProtocol &);

    CoreError getFileWithTimer(const Path &sourceFullPath, const Path &targetFullPath);

    DownloadProgress &m_downloadProgress;
    Log *pLog;
};

}   // namespace KLUPD

#endif // NETTRANSFER_H_INCLUDED_0A12C2BD_08EB_4492_9D4B_ECE417A49368
