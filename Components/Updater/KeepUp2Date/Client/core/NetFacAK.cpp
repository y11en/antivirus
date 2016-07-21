#include "../helper/stdinc.h"
#include "../helper/updaterStaff.h"

#include "NetFacAK.h"

#ifndef DISABLE_AK_FILETRANSFER

    #include <kca/ft/filetransferbridge.h>

    namespace KLUPD {
        std::string toString(const KLFT::FileOpeartionResult &);
    }

    std::string KLUPD::toString(const KLFT::FileOpeartionResult &result)
    {
        switch(result)
        {
        case KLFT::FR_Ok:
            return "success";
        case KLFT::FR_WrongReceiverId:
            return "wrong identifier, either receiver is already deleted or not registered";
        case KLFT::FR_OkDownloaded:
            return "last file chunk has been obtained";
        case KLFT::FR_WrongArguments:
            return "invalid arguments (wrong file offset is requested)";
        case KLFT::FR_FileNotFound:
            return "file is not found";
        case KLFT::FR_Timeout:
            return "timeout expired";
        case KLFT::FR_ErrorInOperation:
            return "download file chunk operation error";
        }

        std::ostringstream stream;
        stream.imbue(std::locale::classic());
        stream << "unknown error " << result;
        return stream.str();
    }

#endif  // DISABLE_AK_FILETRANSFER

//////////////////////////////////////////////////////////
///// class AdministrationKitProtocol
KLUPD::AdministrationKitProtocol::AdministrationKitProtocol(DownloadProgress &downloadProgress, Log *log)
 : m_adminKitTransprot(0),
   m_connected(false),
   m_downloadProgress(downloadProgress),
   pLog(log)
{
#ifndef DISABLE_AK_FILETRANSFER
    KLFT_CreateFileTransferBridge(&m_adminKitTransprot);
#endif
}

KLUPD::AdministrationKitProtocol::~AdministrationKitProtocol()
{
    closeSession();
#ifndef DISABLE_AK_FILETRANSFER
    KLFT_ReleaseFileTransferBridge(&m_adminKitTransprot);
#endif
}

void KLUPD::AdministrationKitProtocol::closeSession()
{
#ifndef DISABLE_AK_FILETRANSFER
    if(m_connected)
        m_adminKitTransprot->Disconnect();

    m_connected = false;
#endif
}

KLUPD::CoreError KLUPD::AdministrationKitProtocol::getFile(
    const Path &fileName, const Path &localPath, const Path &relativeUrlPathIn,
    const bool useMasterAdministrationServer)
{
#ifdef DISABLE_AK_FILETRANSFER
    TRACE_MESSAGE("Administration Kit Transport is not implemented");
    return CORE_DOWNLOAD_ERROR;
#else
    // skip initial slash in relative URL path
    Path relativeUrlPath = relativeUrlPathIn;
    if(!relativeUrlPath.empty()
        && (relativeUrlPath[0] == L'\\' || relativeUrlPath[0] == L'/'))
    {
        relativeUrlPath = relativeUrlPath.toWideChar() + 1;   
    }

    const Path path = relativeUrlPath + fileName;

    ///////////////////////////////////////////////
    /// checking current state and parameters
    const CoreError connectionResult = setupLowLevelConnectionIfNeed(useMasterAdministrationServer);
    if(connectionResult != CORE_NO_ERROR)
    {
        TRACE_MESSAGE2("Failed to setup connection to Administration Server, result '%S'",
            toString(connectionResult).toWideChar());
        return connectionResult;
    }


    ///////////////////////////////////////////////
    /// receiving data by portions
    size_t regettingPosition = LocalFile(localPath + fileName).size();
    AutoStream destinationFile(pLog);
    while(true)
    {
        // check if request should be cancelled
        if(m_downloadProgress.checkCancel())
        {
            TRACE_MESSAGE2("File transfer cancelled '%S'", path.toWideChar());
            return CORE_CANCELLED;
        }

        const long localBufferSize = 65536;
        std::vector<unsigned char> localBuffer(localBufferSize, 0);

        int bytesRead = 0;
        m_downloadProgress.updateSpeedStartTimer();
        const KLFT::FileOpeartionResult getChunkResult = m_adminKitTransprot->GetFileChunk(
            path.toWideChar(),
            regettingPosition,
            &localBuffer[0], localBuffer.size(),
            bytesRead);
        m_downloadProgress.updateSpeedStopTimer();


        // a portion of file got successfully
        if(getChunkResult == KLFT::FR_Ok || getChunkResult == KLFT::FR_OkDownloaded)
        {
            const CoreError saveDataToFileResult = saveDataToFile(
                localPath + fileName,
                &localBuffer[0],
                bytesRead, regettingPosition != 0, destinationFile, pLog);
            if(!isSuccess(saveDataToFileResult))
            {
                TRACE_MESSAGE3("Failed to write data obtained from Administration Server to file '%S', result '%S'",
                    path.toWideChar(), toString(saveDataToFileResult).toWideChar());
                return saveDataToFileResult;
            }

            regettingPosition += bytesRead;
            m_downloadProgress.bytesTransferred(bytesRead);

            // file has been completely downloaded
            if(getChunkResult == KLFT::FR_OkDownloaded)
                return CORE_NO_ERROR;

            continue;
        }

        TRACE_MESSAGE5("Failed to obtain file chunk from Administration Server, file '%S', result '%s', bytes read %d, current position %d",
            path.toWideChar(), toString(getChunkResult).c_str(), bytesRead, regettingPosition);

        if(getChunkResult == KLFT::FR_Timeout)
        {
            // TODO check if time out is over and error should be returned
            continue;
        }

        // error while download happened
        switch(getChunkResult)
        {
        // invalid AdminKit Transport identifier: either receiver was deleted or connect was already called for this receiver
        case KLFT::FR_WrongReceiverId:
            return CORE_AK_WrongReceiverId;

        // invalid arguments for AdminKit Transport
           // wrong offset is requested, consider file is downloaded and signature should be checked
        case KLFT::FR_WrongArguments:
            {            // TODO: this is *work around* about the problem that AdminKit returns
                return CORE_NO_ERROR;    //  FR_WrongArguments code in case source contains
                                        //  valid file in target download folder
                                      // this code can be deleted when problem is fixed in AdminKit
            }

        // file not found on AdminKit server
        case KLFT::FR_FileNotFound:
            return CORE_NO_SOURCE_FILE;

        // AdminKit transport file receive error
        case KLFT::FR_ErrorInOperation:
            return CORE_AK_ErrorInOperation;

        // unknown AdminKit transport receive operation error
        default:
            return CORE_AK_UnknownError;
        }
    }
#endif  // DISABLE_AK_FILETRANSFER
}

KLUPD::CoreError KLUPD::AdministrationKitProtocol::setupLowLevelConnectionIfNeed(const bool useMasterAdministrationServer)
{
#ifdef DISABLE_AK_FILETRANSFER
    TRACE_MESSAGE("Administration Kit Transport is not implemented");
    return CORE_DOWNLOAD_ERROR;
#else
    if(m_connected)
        return CORE_NO_ERROR;

    m_downloadProgress.updateSpeedStartTimer();

    KLFT::ConnectResult connectResult = KLFT::CR_ConnectionServerError;
    if(useMasterAdministrationServer)
    {
        TRACE_MESSAGE("Connecting to master administration server");
        connectResult = m_adminKitTransprot->ConnectToMasterServer();
    }
    else
    {
        TRACE_MESSAGE("Connecting to administration server");
        connectResult = m_adminKitTransprot->Connect();
    }

    m_downloadProgress.updateSpeedStopTimer();

    switch(connectResult)
    {
    // successfully connected
    case KLFT::CR_Ok:
        m_connected = true;
        return CORE_NO_ERROR;
        
    // invalid AdminKit Transport identifier: either receiver was
     //  deleted or connect was already called for this receiver
    case KLFT::CR_WrongReceiverId:
        return CORE_AK_WrongReceiverId;
        
    // AdminKit server is busy and can not handle requests now
    case KLFT::CR_ServerBusy:
        return CORE_AK_ServerBusy;
        
    // physical connection to AdminKit error
    case KLFT::CR_ConnectionError:
        return CORE_AK_ConnectionError;
        
    // connection to AdminKit network agent physical error
    case KLFT::CR_ConnectionNagentError:
        return CORE_AK_ConnectionNagentError;
        
    // connection to master AdminKit server physical error
    case KLFT::CR_ConnectionServerError:
        return CORE_AK_ConnectionServerError;
        
    // unknown AdminKit connection error
    default:
        TRACE_MESSAGE2("Connecting to Administration Server failed with unknown code '%d'", connectResult);
        return CORE_AK_CannotConnectToServer;
    }
#endif  // DISABLE_AK_FILETRANSFER
}
