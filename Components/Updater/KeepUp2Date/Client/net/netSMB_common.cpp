#include "../helper/local_file.h"

#include "../helper/updaterStaff.h"

#include "netTransfer.h"

KLUPD::FileTransferProtocol::FileTransferProtocol(DownloadProgress &downloadProgress, Log *log)
 : m_downloadProgress(downloadProgress),
   pLog(log)
{
}

KLUPD::CoreError KLUPD::FileTransferProtocol::getFile(const Path &sourceFullPathIn, const Path &targetFullPathIn)
{
    m_downloadProgress.updateSpeedStartTimer();
    const CoreError getFileResult = getFileWithTimer(sourceFullPathIn, targetFullPathIn);
    m_downloadProgress.updateSpeedStopTimer();

    return getFileResult;
}

KLUPD::CoreError KLUPD::FileTransferProtocol::getFileWithTimer(const Path &sourceFullPathIn,
                                                               const Path &targetFullPathIn)
{
    Path sourceFullPath = sourceFullPathIn;
    sourceFullPath.fixPathForWin9x();
    Path targetFullPath = targetFullPathIn;
    targetFullPath.fixPathForWin9x();

    const size_t sourceFileSize = LocalFile(sourceFullPath, 0).size();
    AutoStream readStream(pLog);
    const CoreError openReadFileResult = readStream.open(sourceFullPath, L"rb");
    if(!isSuccess(openReadFileResult))
    {
        TRACE_MESSAGE3("Get file error: failed to open source file '%S', result '%S'",
            sourceFullPath.toWideChar(), toString(openReadFileResult).toWideChar());
        return openReadFileResult;
    }

    // open read stream (over the network)
    size_t regettingPosition = LocalFile(targetFullPath, 0).size();
    if(regettingPosition && (fseek(readStream.stream(), regettingPosition, SEEK_CUR) != 0))
    {
        const int lastError = errno;
        TRACE_MESSAGE4("Get file error: seek to %d position error on source file '%S', last error '%S'",
            regettingPosition, sourceFullPath.toWideChar(), errnoToString(lastError, posixStyle).toWideChar());
        return lastErrorToUpdaterFileErrorCode(lastError);
    }
    
    // open write (local) stream
    AutoStream writeStream(pLog);
    const CoreError openWriteFileResult = writeStream.open(targetFullPath, regettingPosition ? L"r+bc" : L"wbc");
    if(!isSuccess(openWriteFileResult))
    {
        TRACE_MESSAGE3("Get file error: failed to open target file '%S', result '%S'",
            targetFullPath.toWideChar(), toString(openWriteFileResult).toWideChar());
        return openWriteFileResult;
    }

    if(regettingPosition
        && (fseek(writeStream.stream(), regettingPosition, SEEK_CUR) != 0))
    {
        const int lastError = errno;
        TRACE_MESSAGE4("Get file error: unable to seek to %d position on file '%S', last error '%S'",
            regettingPosition, targetFullPath.toWideChar(), errnoToString(lastError, posixStyle).toWideChar());
        return lastErrorToUpdaterFileErrorCode(lastError);
    }


    char buffer[MAX_IO_BUFFER_SIZE + 1];
    memset(buffer, 0, MAX_IO_BUFFER_SIZE + 1);

    CoreError fileCopyResult = CORE_NO_ERROR;
    while(regettingPosition < sourceFileSize)
    {
        if(m_downloadProgress.checkCancel())
            return CORE_CANCELLED;
        
        if(feof(readStream.stream()))
            break;
        
        const size_t bytesRead = fread((void *)buffer, 1, MAX_IO_BUFFER_SIZE, readStream.stream());
        if(ferror(readStream.stream()))
        {
            const int lastError = errno;
            TRACE_MESSAGE3("Read operation error on file '%S', last error '%S'",
                sourceFullPath.toWideChar(), errnoToString(lastError, posixStyle).toWideChar());
            fileCopyResult = lastErrorToUpdaterFileErrorCode(lastError);
            break;
        }

        // end of file reached
        if(bytesRead)
        {
            if((fwrite(reinterpret_cast<void *>(buffer), 1, bytesRead, writeStream.stream()) <= 0)
                || ferror(writeStream.stream()))
            {
                const int lastError = errno;
                TRACE_MESSAGE3("Write operation error on file '%S', last error '%S'",
                    targetFullPath.toWideChar(), errnoToString(lastError, posixStyle).toWideChar());
                fileCopyResult = lastErrorToUpdaterFileErrorCode(lastError);
                break;
            }

            regettingPosition += bytesRead;
            m_downloadProgress.bytesTransferred(bytesRead);
        }
    }

    return fileCopyResult;
}
