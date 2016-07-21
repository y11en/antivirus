#include "updater.h"

#include "../helper/updaterStaff.h"
#include "../helper/update_transaction.h"

#include "../helper/indexFileXMLParser.h"

bool KLUPD::Updater::downloadedIndexNewerThenLocal(const FileInfo &file, const Path &downloadedPath, const Path &localPath, Signature6Checker &signature6Checker, const bool retranslationMode)const
{
    // date check is not performed for files which are consistent by md5 or signature 5
    if(!file.m_signature5.empty() || !file.m_md5.empty())
        return true;

    const NoCaseString downloadedFileDate = IndexFileXMLParser::getTagFromLocalFile(IndexFileXMLParser::updateDate,
        downloadedPath + file.m_filename, m_callback, pLog);
    const unsigned long downloadedFileSequenceNumber = updateDateStringToSequenceNumber(downloadedFileDate);
    const NoCaseString localFileDate = IndexFileXMLParser::getTagFromLocalFile(IndexFileXMLParser::updateDate,
        localPath + file.m_filename, m_callback, pLog);
    const unsigned long localFileSequenceNumber = updateDateStringToSequenceNumber(localFileDate);

    // failed to get date, consider downloaded file is newer
    if(downloadedFileDate.empty() || localFileDate.empty())
        return true;

    // check if date is the same, to avoid extra copy operations
    if(downloadedFileSequenceNumber == localFileSequenceNumber)
    {
        TRACE_MESSAGE4("\tDownloaded file date '%S' is the same then local one '%S'. Local file will be used '%S'",
            downloadedFileDate.toWideChar(), localFileDate.toWideChar(), (file.m_relativeURLPath + file.m_filename).toWideChar());
        return false;
    }

    // if local bases are not consistent, then date check is not performed,
    //  because file may belongs to other update set
    const bool &localBasesConsistent = retranslationMode ? m_retranslationLocalBasesAreConsistent : m_updateLocalBasesAreConsistent;
    if(!localBasesConsistent || m_sourceList.currentSourceIgnoreDates())
        return true;


    if(downloadedFileSequenceNumber < localFileSequenceNumber)
    {
        TRACE_MESSAGE4("\tDownloaded file date '%S' is NOT newer then local one '%S'. Local file will be used '%S'",
            downloadedFileDate.toWideChar(), localFileDate.toWideChar(), (file.m_relativeURLPath + file.m_filename).toWideChar());
        return false;
    }
    return true;
}

KLUPD::CoreError KLUPD::Updater::downloadSecondaryIndexes(UpdateInfo &update)
{
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

        // push progress bar slightly
        m_downloadProgress.tickPercent();

        CoreError parseResult = CORE_NO_ERROR;
        // get local file because it did not changed
        if(file.m_transactionInformation.m_changeStatus == FileInfo::unchanged)
        {
            parseResult = update.parse(m_callback.productFolder(file, update.m_retranslationMode), file,
                // check state is performed, though index has not been changed, because filtering
                 // parameters could change in comparison to previous download attempt
                m_sourceList.currentSourceIgnoreDates() ? UpdateInfo::checkStateButIgnoreDate : UpdateInfo::checkStateAgainstLocalFile,
                localConsistentFiles(update.m_retranslationMode),
                false);
        }
        // download file is needed
        else
        {
            // local file does not suit, downloading file from source
            const CoreError downloadResult = downloadFileUsingCache(file, update);

            if(isSuccess(downloadResult))
            {
                // index file date downloaded from source may differs from local file date,
                 // that is why index file date must be re-checked
                if(downloadedIndexNewerThenLocal(file, m_callback.temporaryFolder(file),
                    m_callback.productFolder(file, update.m_retranslationMode), update.m_signature6Checker, update.m_retranslationMode))
                {
                    // downloaded file is newer, so using it
                    parseResult = update.parse(m_callback.temporaryFolder(file), file,
                        m_sourceList.currentSourceIgnoreDates() ? UpdateInfo::checkStateButIgnoreDate : UpdateInfo::checkStateAgainstLocalFile,
                        localConsistentFiles(update.m_retranslationMode),
                        false);
                }
                // use more recent local file instead downloaded file
                else
                {
                    // update information with index unchanged status
                    FileInfo newValue = file;
                    newValue.m_transactionInformation.m_changeStatus = FileInfo::unchanged;
                    bool fileDuplicate = true;
                    update.m_matchFileList.insertNewOrUpdateTheSame(file, newValue, fileDuplicate, update.m_retranslationMode);
                    parseResult = update.parse(m_callback.productFolder(file, update.m_retranslationMode), file,
                        // check state is performed, though index has not been changed, because filtering
                         // parameters could change in comparison to previous download attempt
                        m_sourceList.currentSourceIgnoreDates() ? UpdateInfo::checkStateButIgnoreDate : UpdateInfo::checkStateAgainstLocalFile,
                        localConsistentFiles(update.m_retranslationMode),
                        false);
                }
            }
            else
            {
                // other then damaged file error, are considered to be critical for this source
                if(!absentFile(downloadResult) && !badFile(downloadResult))
                    return downloadResult;


                // query product if this file is not critical
                if(!update.checkIfFileOptional(file))
                    return downloadResult;   // obligatory file is absent
                update.removeComponentFromUpdate(file.m_componentIdSet, m_journal);

                // optional file skipped
                TRACE_MESSAGE2("\tSkipped optional entry while downloading local files '%S'",
                    (m_callback.temporaryFolder(file) + file.m_filename).toWideChar());
            }

        }

        if(!isSuccess(parseResult))
        {
            TRACE_MESSAGE3("Failed to attach secondary update index '%S', error '%S'",
                (m_callback.temporaryFolder(file) + file.m_filename).toWideChar(), toString(parseResult).toWideChar());
            return parseResult;
        }

        // processing new files added to transaction
        if(savedUpdateSize != update.m_matchFileList.size())
            fileIndex = static_cast<size_t>(-1);
    }

    // check if anything is to be updated
    for(FileVector::const_iterator fileIter = update.m_matchFileList.begin(); fileIter != update.m_matchFileList.end(); ++fileIter)
    {
        if(fileIter->m_transactionInformation.m_changeStatus != FileInfo::unchanged)
            return CORE_NO_ERROR;
    }
    TRACE_MESSAGE2("Secondary index downloaded for %s operation, but there is no files to update",
        update.m_retranslationMode ? "retranslation" : "update");
    return CORE_NOTHING_TO_UPDATE;
}

// check each index file in update set agaings Black Limit Date
bool KLUPD::Updater::checkBlackDateLimit(const FileVector &files, const unsigned long blackListLimit, const bool retranslationMode)
{
    for(FileVector::const_iterator fileIter = files.begin(); fileIter != files.end(); ++fileIter)
    {
        if(fileIter->m_type != FileInfo::index)
            continue;

        // check downloaded file against black list limit date
        const Path path = fileIter->m_transactionInformation.m_changeStatus == FileInfo::unchanged
            ? m_callback.productFolder(*fileIter, retranslationMode)
            : m_callback.temporaryFolder(*fileIter);

        const NoCaseString downloadedFileDate = IndexFileXMLParser::getTagFromLocalFile(IndexFileXMLParser::blackDate,
            path + fileIter->m_filename, m_callback, pLog);

        // check is optional and may be turned off by removing tag from index
        if(downloadedFileDate.empty())
            continue;

        const unsigned long downloadedFileSequenceNumber = updateDateStringToSequenceNumber(downloadedFileDate);
        if(blackListLimit < downloadedFileSequenceNumber)
        {
            TRACE_MESSAGE3("Black list date check is invalid for '%S' dated by '%S'",
                (path + fileIter->m_filename).toWideChar(), downloadedFileDate.toWideChar());
            return false;
        }
    }

    // Black Date may be specified for no file in update set, it is optional
    return true;
}

KLUPD::CoreError KLUPD::Updater::downloadEntry(const FileInfo &fileEntry, const Path &temporaryFolder,
                                               const bool fileWithVariableExtensionNotObligatoryForUpdate)
{
    // creating destination folder
    if(!createFolder(temporaryFolder, pLog))
    {
        reportCanNotCreateFolder(temporaryFolder, "temporary folder to download file");
        return CORE_FailedToCreateFolder;
    }

    if(fileEntry.m_primaryIndex && (m_sourceList.currentUpdateStructure() != UPS_FORCE_PLAIN_FOR_SUBSTITUTE_INDEX))
    {
        TRACE_MESSAGE("Try directoried update structure to download primary index file");
        m_sourceList.currentUpdateStructure(UPS_DIRS);
    }

    Path relativeURLPath;
    if(isPlainUpdateStructure(m_sourceList.currentUpdateStructure()))
        relativeURLPath = L"/";
    else
        relativeURLPath = fileEntry.m_relativeURLPath;

    m_journal.publishMessage(EVENT_DOWNLOAD_FILE_STARTED, (fileEntry.m_relativeURLPath + fileEntry.m_filename).toWideChar());

    // loop: if update from plain structure then try update from local structure
    while(true)
    {
        Source source;
        if(!m_sourceList.getCurrentElement(source))
            return CORE_URLS_EXHAUSTED;


        CoreError result = CORE_NO_ERROR;

        // FTP via proxy server transport, or HTTP protocol
        const bool ftpWithProxyOrHttp = (source.m_url.find(L"http:") != NoCaseStringImplementation::npos)
            || (source.m_useProxy && (source.m_url.find(L"ftp:") != NoCaseStringImplementation::npos));


        // in case download with regetting fails, then will retry without regetting
        while(true)
        {
            LocalFile regettingLocalFile(temporaryFolder + fileEntry.m_filename, pLog);
            const size_t regettingFileSize = regettingLocalFile.size();
            const bool fileWasEmpty = !regettingFileSize;

            // Administration Server transport protocol
            if((source.m_type == UST_AKServer) || (source.m_type == UST_AKMServer))
            {
                result = m_administrationKitProtocol.getFile(fileEntry.m_filename,
                    temporaryFolder, relativeURLPath, source.m_type == UST_AKMServer);
            }

            // HTTP or FTP via proxy transport
            else if(ftpWithProxyOrHttp)
            {
                Address serverAddress(source.m_url);

                Log *suppressLog = 0;
                const size_t regettingPosition = LocalFile(temporaryFolder + fileEntry.m_filename, suppressLog).size();

                result = m_httpProtocol.getFile(
                    fileEntry.m_filename,
                    temporaryFolder,
                    relativeURLPath,
                    serverAddress,
                    m_callback.updaterConfiguration().composeIdentityInfoString().toAscii(),
                    source.m_useProxy,
                    m_currentProxyAddress,
                    m_callback.updaterConfiguration().m_httpAuthorizationMethods,
                    regettingPosition,
                    fileWithVariableExtensionNotObligatoryForUpdate);
            }

            // FTP without proxy server, and update with file transfer transport
            else
            {
                result = DoGetRemoteFile_SelfNet(fileEntry.m_filename,
                    temporaryFolder, relativeURLPath, source.m_url);
            }

            if(result == KLUPD::CORE_NO_ERROR)
            {
                if(!fileWasEmpty)
                {
                    TRACE_MESSAGE3("File downloaded with regetting from position '%d', '%S'",
                        regettingFileSize, regettingLocalFile.m_name.toWideChar());
                }

                break;
            }

            if(fileWasEmpty)
                break;
            if(!isSuccess(regettingLocalFile.unlink()))
                break;

            TRACE_MESSAGE2("Failed to download with regetting. File was deleted and download with no regetting will be tried for '%S'",
                regettingLocalFile.m_name.toWideChar());
        }

        // log succesful file download event
        if(isSuccess(result))
            m_journal.publishMessage(EVENT_FILE_DOWNLOADED, (relativeURLPath + fileEntry.m_filename).toWideChar());

        if(absentFile(result))
        {
            // do not try plain structure if update source does not support it
            if(source.onlyDirectoriedStructureAllowed())
                return result;

            // change update structure only for primary index
            if((m_sourceList.currentUpdateStructure() == UPS_DIRS) && fileEntry.m_primaryIndex)
            {
                // try to update from plain source
                relativeURLPath = L"/";
                TRACE_MESSAGE2("Plain update structure is used, because failed to get file '%S' from directoried structure",
                    fileEntry.m_filename.toWideChar());
                m_sourceList.currentUpdateStructure(UPS_PLAIN);
                continue;
            }

            /// source structure may differ from local structure, because there are 2 tags
            ///  in index <RelativeSrvPath> and <LocalPath>. Local structure is determined by tag <LocalPath>
            /// the structure of files is the similar to local update result structure
            if(isPlainUpdateStructure(m_sourceList.currentUpdateStructure()))
            {
                Path localStructureRelativeFolder = fileEntry.m_originalLocalPath;
                // expand all substitutions to empty string
                StringParser::clearSubstitutions(localStructureRelativeFolder, L'%');
                StringParser::canonizePath(StringParser::byProductFirst, localStructureRelativeFolder, m_callback);

                // local structured folder does not different, no need to download
                if(relativeURLPath.emptyIfSingleSlash() == localStructureRelativeFolder.emptyIfSingleSlash())
                    return result;

                relativeURLPath = localStructureRelativeFolder;
                continue;
            }
        }

        return result;
    }
}
