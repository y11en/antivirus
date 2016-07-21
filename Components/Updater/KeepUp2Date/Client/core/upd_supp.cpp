#include "updater.h"

#include "../helper/stdinc.h"

#include "../helper/sites_info.h"
#include "../helper/updaterStaff.h"
#include "../helper/update_transaction.h"
#include "../helper/indexFileXMLParser.h"

#include "../include/ver_mod.h"


KLUPD::Updater::Updater(DownloadProgress &downloadProgress, CallbackInterface &callbacks, JournalInterface &journal, Log *log)
 : m_journal(journal),
   m_downloadProgress(downloadProgress),
   m_callback(callbacks),
   pLog(log),
   m_localFilesEnumerated(false),
   m_retranslationLocalBasesAreConsistent(false),
   m_updateLocalBasesAreConsistent(false),
   m_retranslationLocalSignature6Checker(log),
   m_updateLocalSignature6Checker(log),
   m_httpProtocol(callbacks.updaterConfiguration().m_httpAuthorizationMethods,
        callbacks.updaterConfiguration().m_proxyAuthorizationCredentials,
        callbacks.updaterConfiguration().connect_tmo,
        downloadProgress, journal, log),
   m_ftpProtocol(callbacks.updaterConfiguration().connect_tmo,
        callbacks.updaterConfiguration().passive_ftp
            ? (callbacks.updaterConfiguration().m_tryActiveFtpIfPassiveFails ? tryActiveFtpIfPassiveFails : passiveFtp)
            : activeFtp,
        callbacks.updaterConfiguration().composeIdentityInfoString(), downloadProgress, log),
   m_administrationKitProtocol(downloadProgress, log),
   m_fileProtocol(downloadProgress, log),
   m_retranslationResult(CORE_NO_ERROR),
   m_updateResult(CORE_NO_ERROR),
   m_retranslationDone(false),
   m_updateDone(false),
   m_retranslationBlackListLimit(0),
   m_updateBlackListLimit(0),
   m_proxyDetector(callbacks.updaterConfiguration().connect_tmo, downloadProgress, journal, log)
{
}

KLUPD::CoreError KLUPD::Updater::enumerateLocalSecondaryIndexes(UpdateInfo &update)
{
    CoreError result = CORE_NO_ERROR;

    for(size_t fileIndex = 0; fileIndex < update.m_matchFileList.size(); ++fileIndex)
    {
        const size_t savedUpdateSize = update.m_matchFileList.size();
        FileInfo& file = update.m_matchFileList[fileIndex];
        
        // select only index, and filter other file type
        if(!file.isIndex())
            continue;

        // file already processed
        if(update.fileAlreadyParsed(file))
            continue;

        if(m_downloadProgress.checkCancel())
            return CORE_CANCELLED;


        const CoreError parseResult = update.parse(m_callback.productFolder(file, update.m_retranslationMode),
            file, UpdateInfo::noStateChangeCheck, FileVector(), true);
        if(!isSuccess(parseResult))
        {
            result = parseResult;
            TRACE_MESSAGE3("\tMissed secondary index file '%S', error '%S'",
                (m_callback.temporaryFolder(file) + file.m_filename).toWideChar(), toString(parseResult).toWideChar());
        }

        // processing new files added to transaction
        if(savedUpdateSize != update.m_matchFileList.size())
            fileIndex = static_cast<size_t>(-1);
    }

    return result;
}

KLUPD::NoCaseString KLUPD::Updater::GetUpdateDate(const FileVector& vecFiles)
{
    time_t _time = {0};
    NoCaseString strUpdateDate;

    int nYear = 0, nMonth = 0, nDay = 0, nHour = 0, nMinute = 0;
    for(FileVector::const_iterator iter = vecFiles.begin(); iter != vecFiles.end(); ++iter)
    {
        if( !iter->isIndex() )
            continue;

        tm _tm;
        if( !iter->m_strUpdateDate.ToTime(_tm) )
            continue;

        time_t _time2 = mktime(&_tm);
        if( _time2 > _time )
        {
            _time = _time2;
            strUpdateDate = iter->m_strUpdateDate;
        }
    }

    return strUpdateDate;
}

bool KLUPD::Updater::_EnumerateLocalFiles(UpdateInfo &info, bool bRetr)
{
    bool bOk = true;

    // check if local primary index exists
    FileInfo primaryIndex = getPrimaryIndexFileInfo(false, bRetr);
    Path primaryIndexFileName = m_callback.productFolder(primaryIndex, bRetr) + primaryIndex.m_filename;

    CoreError result = CORE_NO_ERROR;

    bool bDuplicate = false;
    info.m_matchFileList.insertNewOrUpdateTheSame(primaryIndex, primaryIndex, bDuplicate, bRetr);

    result = info.parse( m_callback.productFolder(primaryIndex, bRetr),
                         primaryIndex, UpdateInfo::noStateChangeCheck, FileVector(), true );
    if( !(bOk = isSuccess(result)) )
        TRACE_MESSAGE2( "*** Failed to enumerate local files: can't parse local primary index (error '%S') ***",
                        KLUPD::toString(result).toWideChar() );

    if( bOk )
    {
        result = enumerateLocalSecondaryIndexes(info);
        if( !(bOk = isSuccess(result)) )
            TRACE_MESSAGE2( "*** Failed to enumerate local files: can't parse local secondary index (error '%S') ***",
                            KLUPD::toString(result).toWideChar() );
    }

    return bOk;
}

bool KLUPD::Updater::_VerifyFiles(UpdateInfo &info, bool bRetr)
{
    // check black list presence
    unsigned long blackListLimit = 0;
    if(!checkBlackListPresence(info.m_matchFileList, blackListLimit, pLog))
    {
        TRACE_MESSAGE("*** Database verification failed: can't find black list file ***");
        return false;
    }

    if(!checkBlackDateLimit(info.m_matchFileList, blackListLimit, bRetr))
    {
        TRACE_MESSAGE("*** Database verification failed: black list limit date check failed ***");
        return false;
    }

    // process matched files
    FileVector& files = info.m_matchFileList;
    for(size_t index = 0; index < files.size();)
    {
        // inform user that update is not freezed
        m_downloadProgress.tickPercent();
        if(m_downloadProgress.checkCancel())
        {
            TRACE_MESSAGE("*** Enumerate local files cancelled ***");
            return false;
        }

        const Path fullFileName = m_callback.productFolder(files[index], bRetr) + files[index].m_filename;

        // check file signature
        NoCaseString signature6ErrorMessage;
        if(!checkFileMd5OrPrimaryIndexSignature5And6(fullFileName, files[index],
            info.m_signature6Checker, signature6ErrorMessage, pLog))
        {
            TRACE_MESSAGE3("*** Database verification failed: file '%S' is damaged (%S) ***",
                fullFileName.toWideChar(), signature6ErrorMessage.toWideChar());
            return false;
        }

        ++index;
    }
    TRACE_MESSAGE2("*** Database verification succeed (%S mode) ***", bRetr ? L"retranslation" : L"update");
    return true;
}

bool KLUPD::Updater::EnumerateLocalFiles(FileVector &vecFiles, bool bRetr, bool bVerify)
{
    TRACE_MESSAGE2("*** Enumerating local files in %s mode ***", bRetr ? "retranslation" : "update");

    Signature6Checker signer(pLog);
    UpdateInfo info( m_callback.updaterConfiguration().filterFlags(bRetr),
                     m_callback.updaterConfiguration().filterComponents(bRetr), signer, bRetr, m_callback, pLog );

    bool bOk = true;
    if( (bOk = _EnumerateLocalFiles(info, bRetr)) && bVerify )
        bOk = _VerifyFiles(info, bRetr);

    vecFiles.swap(info.m_matchFileList);

    TRACE_MESSAGE3("*** Enumerating local files in %s mode %s ***", bRetr ? "retranslation" : "update", bOk ? "succeed" : "failed");
    return bOk;
}

void KLUPD::Updater::calculatingDownloadSize(const UpdateInfo &update, const UpdateInfo &retranslation)
{
    // reset download size. Take into consideration only already downloaded data
    m_downloadProgress.m_expectedDownloadSize = m_downloadProgress.m_bytesTransferred;


    // calculate update size for update operation
    for(FileVector::const_iterator updateFileIter = update.m_matchFileList.begin(); updateFileIter != update.m_matchFileList.end(); ++updateFileIter)
    {
        // index and inlined files are already downloaded
        if(updateFileIter->isInlined() || updateFileIter->isIndex())
            continue;

        if(updateFileIter->downloadNeeded() && (updateFileIter->m_size != static_cast<size_t>(-1)))
            m_downloadProgress.m_expectedDownloadSize += updateFileIter->m_size;
    }

    // calculate update size for update operation
    for(FileVector::const_iterator retranslationFileIter = retranslation.m_matchFileList.begin(); retranslationFileIter != retranslation.m_matchFileList.end(); ++retranslationFileIter)
    {
        // index and inlined files are already downloaded
        if(retranslationFileIter->isInlined() || retranslationFileIter->isIndex())
            continue;

        if(retranslationFileIter->downloadNeeded() && (retranslationFileIter->m_size != static_cast<size_t>(-1)))
        {
            FileInfo updateFileInfo;
            // check if file already presents in update set, to avoid calculating its size twice
            if(update.m_matchFileList.findTheSameFile(*retranslationFileIter, updateFileInfo, update.m_retranslationMode))
            {
                // file presents in update set, but its size was not taken into account,
                 // because it is needed to be downloaded only in retranslation mode
                if((updateFileInfo.m_transactionInformation.m_changeStatus == FileInfo::unchanged)
                    && (m_downloadProgress.m_expectedDownloadSize != static_cast<size_t>(-1)))
                {
                    m_downloadProgress.m_expectedDownloadSize += retranslationFileIter->m_size;
                }
            }
            else
            {
                // file does not present in update set
                m_downloadProgress.m_expectedDownloadSize += retranslationFileIter->m_size;
            }

        }
    }

    TRACE_MESSAGE2("Expected update size is %d bytes", m_downloadProgress.m_expectedDownloadSize);
}


KLUPD::FileInfo &KLUPD::Updater::getPrimaryIndexFileInfo(const bool checkIfChangedAgainstFileInTemporaryFolder, const bool retranslationMode)
{
    ///////////////////////////////
    ///// Retranslation mode
    if(retranslationMode)
    {
        // set default values in case primary index information is not set
        if(m_retranslationPrimaryIndexInformation.m_filename.empty())
        {
            m_retranslationPrimaryIndexInformation = m_callback.getPrimaryIndex(retranslationMode);
        
            // pathes
            StringParser::canonizePath(StringParser::byProductFirst, m_retranslationPrimaryIndexInformation.m_localPath, m_callback);
            StringParser::canonizePath(StringParser::byProductFirst, m_retranslationPrimaryIndexInformation.m_relativeURLPath, m_callback);
            m_retranslationPrimaryIndexInformation.m_localPath
                = m_callback.productFolder(retranslationMode) + m_retranslationPrimaryIndexInformation.m_relativeURLPath;

            // transaction information
            m_retranslationPrimaryIndexInformation.m_transactionInformation.m_currentLocation
                = m_callback.productFolder(m_retranslationPrimaryIndexInformation, retranslationMode);
            m_retranslationPrimaryIndexInformation.m_transactionInformation.m_newLocation
                = m_callback.temporaryFolder(m_retranslationPrimaryIndexInformation);

            // change status
            LocalFile oldFile(m_callback.productFolder(m_retranslationPrimaryIndexInformation, retranslationMode)
                + m_retranslationPrimaryIndexInformation.m_filename, pLog);
            m_retranslationPrimaryIndexInformation.m_transactionInformation.m_changeStatus
                = oldFile.exists() ? FileInfo::modified : FileInfo::added;

            TRACE_MESSAGE2("Retranslation primary index information: %S",
                m_retranslationPrimaryIndexInformation.toString(true).toWideChar());
        }

        if(checkIfChangedAgainstFileInTemporaryFolder)
        {
            std::vector<unsigned char> oldFileBuffer;
            LocalFile oldFile(m_callback.productFolder(m_retranslationPrimaryIndexInformation, retranslationMode) + m_retranslationPrimaryIndexInformation.m_filename, pLog);
            std::vector<unsigned char> newFileBuffer;
            LocalFile newFile(m_callback.temporaryFolder(m_retranslationPrimaryIndexInformation) + m_retranslationPrimaryIndexInformation.m_filename, pLog);
            if(isSuccess(oldFile.read(oldFileBuffer))
                && isSuccess(newFile.read(newFileBuffer))
                && (oldFileBuffer == newFileBuffer))
            {
                m_retranslationPrimaryIndexInformation.m_transactionInformation.m_changeStatus = FileInfo::unchanged;
            }
        }
        return m_retranslationPrimaryIndexInformation;
    }


    ///////////////////////////////
    ///// Update mode
    // set default values in case primary index information is not set
    if(m_updatePrimaryIndexInformation.m_filename.empty())
    {
        m_updatePrimaryIndexInformation = m_callback.getPrimaryIndex(retranslationMode);

        // pathes
        StringParser::canonizePath(StringParser::byProductFirst, m_updatePrimaryIndexInformation.m_localPath, m_callback);
        StringParser::canonizePath(StringParser::byProductFirst, m_updatePrimaryIndexInformation.m_relativeURLPath, m_callback);
        if(m_updatePrimaryIndexInformation.m_localPath.emptyIfSingleSlash().empty())
            m_updatePrimaryIndexInformation.m_localPath = m_callback.productFolder(retranslationMode);

        // transaction information
        m_updatePrimaryIndexInformation.m_transactionInformation.m_currentLocation
            = m_callback.productFolder(m_updatePrimaryIndexInformation, retranslationMode);
        m_updatePrimaryIndexInformation.m_transactionInformation.m_newLocation
            = m_callback.temporaryFolder(m_updatePrimaryIndexInformation);

        // change status
        LocalFile oldFile(m_callback.productFolder(m_updatePrimaryIndexInformation,
            retranslationMode) + m_updatePrimaryIndexInformation.m_filename, pLog);
        m_updatePrimaryIndexInformation.m_transactionInformation.m_changeStatus
            = oldFile.exists() ? FileInfo::modified : FileInfo::added;

        TRACE_MESSAGE2("Update primary index information: %S",
            m_updatePrimaryIndexInformation.toString(true).toWideChar());
    }

    if(checkIfChangedAgainstFileInTemporaryFolder)
    {
        std::vector<unsigned char> oldFileBuffer;
        LocalFile oldFile(m_callback.productFolder(m_updatePrimaryIndexInformation, retranslationMode) + m_updatePrimaryIndexInformation.m_filename, pLog);
        std::vector<unsigned char> newFileBuffer;
        LocalFile newFile(m_callback.temporaryFolder(m_updatePrimaryIndexInformation) + m_updatePrimaryIndexInformation.m_filename, pLog);
        if(isSuccess(oldFile.read(oldFileBuffer))
            && isSuccess(newFile.read(newFileBuffer))
            && (oldFileBuffer == newFileBuffer))
        {
            m_updatePrimaryIndexInformation.m_transactionInformation.m_changeStatus = FileInfo::unchanged;
        }
    }
    return m_updatePrimaryIndexInformation;
}

KLUPD::CoreError KLUPD::Updater::parsePrimaryIndex(UpdateInfo &update)
{
    FileInfo primaryIndex = getPrimaryIndexFileInfo(true, update.m_retranslationMode);
    const CoreError parseResult = update.parse(m_callback.temporaryFolder(primaryIndex), primaryIndex,
        m_sourceList.currentSourceIgnoreDates() ? UpdateInfo::checkStateButIgnoreDate : UpdateInfo::checkStateAgainstLocalFile,
        localConsistentFiles(update.m_retranslationMode), false);
    if(!isSuccess(parseResult))
    {
        TRACE_MESSAGE2("Primary index parse error '%S'",
            KLUPD::toString(parseResult).toWideChar());
        return parseResult;
    }

    // add the file to transaction
    bool fileDuplicate = false;
    update.m_matchFileList.insertNewOrUpdateTheSame(primaryIndex, primaryIndex, fileDuplicate, update.m_retranslationMode);
    TRACE_MESSAGE2("Primary index file added to transaction %S",
        primaryIndex.toString(true).toWideChar());

    // save bases date for licence check and check if files on source changed while update
    m_basesDateCurrentIteration = primaryIndex.m_strUpdateDate;
    return CORE_NO_ERROR;
}

void KLUPD::Updater::reportSourceSelected()
{
    // erase information about all files downloaded from previous source
    m_downloadedFilesInCurrentSession.clear();

    // though operation is finished, different files may be present on update source,
     // that is why files are to be downloaded againt
    m_retranslationDone = !m_callback.updaterConfiguration().retranslationRequired();
    m_updateDone = !m_callback.updaterConfiguration().updateRequired();

    Source source;
    if(!m_sourceList.getCurrentElement(source))
        return;

    // check if automatic proxy server address detection is possible for this address
     // otherwise FTP whith proxy spoils DoGetRemoteFile_SelfNet() API
    if(source.m_useProxy)
    {
        // check if local address for bypass proxy for local addresses mode
        if(m_callback.updaterConfiguration().m_bypassProxyServerForLocalAddresses)
        {
            NoCaseString strHostName = KLUPD::Address(source.m_url).m_hostname.m_value;

            KLUPD::NoCaseString localAddressAnalysisResult;
            if(m_proxyDetector.IsLocalAddressEx(strHostName, localAddressAnalysisResult))
            {
                TRACE_MESSAGE2("Direct connection (no proxy), because bypass proxy server check (%S)", localAddressAnalysisResult.toWideChar());
                m_sourceList.disableProxyForCurrentSource();
                source.m_useProxy = false;
            }
            else
                TRACE_MESSAGE2("Address is not local (%S)", localAddressAnalysisResult.toWideChar());
        }
    }

    if(source.m_useProxy)
    {
        // automatic proxy server address detection
        if(m_callback.updaterConfiguration().use_ie_proxy)
        {
            if(directConnection == m_proxyDetector.getProxyAddress(m_currentProxyAddress, source.m_url))
            {
                TRACE_MESSAGE2("Automatic proxy server address detection result: direct connection for '%S'",
                    source.m_url.toWideChar());
                m_sourceList.disableProxyForCurrentSource();

                // for correct output to trace
                source.m_useProxy = false;
            }
        }
        // use user-specified proxy server address
        else
        {
            m_currentProxyAddress.m_hostname = m_callback.updaterConfiguration().proxy_url;

            // convert port to wide-char string
            std::ostringstream strPort;
            strPort << m_callback.updaterConfiguration().proxy_port;
            m_currentProxyAddress.m_service.m_value.fromAscii(strPort.str());
        }
    }

    TRACE_MESSAGE2("Source selected %S", source.toString().toWideChar());
    if(source.m_type == UST_AKServer || source.m_type == UST_AKMServer)
        m_journal.publishMessage(EVENT_ADMIN_KIT_SOURCE_SELECTED);
    else
        m_journal.publishMessage(EVENT_SOURCE_SELECTED, source.m_url.toWideChar());

    // report proxy server is used
    if(source.m_useProxy)
        m_journal.publishMessage(KLUPD::EVENT_PROXY_SERVER_SELECTED, m_currentProxyAddress.BuildURI().toWideChar());
}

void KLUPD::Updater::reportNetworkEvent(const CoreError &code, const NoCaseString &parameter)const
{
    m_journal.publishMessage(code,
        // adjust journal message to output proxy server address
        (code == CORE_CANT_CONNECT_TO_PROXY) || (code == CORE_CANT_RESOLVE_PROXY)
        ? m_currentProxyAddress.BuildURI().toWideChar() : parameter);
}

const KLUPD::FileVector KLUPD::Updater::localConsistentFiles(const bool retranslationMode)const
{
    if((retranslationMode ? m_retranslationLocalBasesAreConsistent : m_updateLocalBasesAreConsistent))
        return retranslationMode ? m_retranslationLocalFiles : m_updateLocalFiles;

    // local files are not consistent
    return FileVector();
}
