#include "../helper/stdinc.h"

#include "updater.h"
#include "diffs.h"
#include "../helper/sites_info.h"
#include "../helper/updaterStaff.h"
#include "../helper/update_transaction.h"


bool KLUPD::Updater::constructSourceList(KLUPD::SourceList &input, KLUPD::SourceList &result)
{
    result.m_sources.clear();

    Source source;
    for(input.moveFirst(); input.getCurrentElement(source); input.moveNext())
    {
        if(source.m_type == UST_AKServer)
            result.addSource(UST_AKServer, Path(), false, source.m_differenceFolder);
        else if(source.m_type == UST_AKMServer)
            result.addSource(UST_AKMServer, Path(), false, source.m_differenceFolder);
        // getting Kaspersky Laboratory addresses from configuration files
        else if(source.m_type == UST_KLServer)
        {
            std::vector<Path> ordered_sites_list;

            if(!loadSitesList(ordered_sites_list))
                return false;

            for(size_t k = 0; k < ordered_sites_list.size(); ++k)
                result.addSource(UST_KLServer, ordered_sites_list[k], source.m_useProxy, source.m_differenceFolder);
        }
        else
        {
            // canonize path to allow environment variable substitutions in it,
             // user may need system environment variable as update source, that is why it is used first
            StringParser::canonizePath(StringParser::bySystemFirst, source.m_url, m_callback);

            result.addSource(UST_UserURL, source.m_url, source.m_useProxy, source.m_differenceFolder);
        }
    }

    result.moveFirst();
    return true;
}

// TODO: move function to "helper/sites_info.h" class
bool KLUPD::Updater::loadSitesList(std::vector<Path> &ordered_sites_list)
{
    // 1. getting user defined sites information from sites.xml
    FileInfo sitesXmlFile = m_callback.getSitesFileXml();
    StringParser::canonizePath(StringParser::byProductFirst, sitesXmlFile.m_relativeURLPath, m_callback);
    StringParser::canonizePath(StringParser::byProductFirst, sitesXmlFile.m_localPath, m_callback);

    if(sitesXmlFile.m_filename.empty())
    {
        TRACE_MESSAGE("Additional sources information file has not been provided by product");
    }
    else
    {
        const Path fullPathToSitesConfigurationFiles = sitesXmlFile.m_localPath + sitesXmlFile.m_filename;
        TRACE_MESSAGE2("Getting user defined sites information from file '%S' (using <site2> tag)",
            fullPathToSitesConfigurationFiles.toWideChar());

        std::vector<SiteInfo> userUpdateSites;
        SiteInfo::Description description;
        if(SiteInfo().getSitesInfo(userUpdateSites, description, fullPathToSitesConfigurationFiles, false, pLog, false))
        {
            SiteInfo().sortSites(userUpdateSites, SiteInfo::Region(m_callback.updaterConfiguration().self_region),
                ordered_sites_list, pLog);
            TRACE_MESSAGE3("%d user defined sites list are read successfully from file '%S'",
                userUpdateSites.size(), fullPathToSitesConfigurationFiles.toWideChar());
        }
        else
        {
            TRACE_MESSAGE2("Failed to get user defined sites information from file %S",
                fullPathToSitesConfigurationFiles.toWideChar());
        }
    }


    // 2. getting user defined sites information from updcfg.xml
    FileInfo updaterConfigurationFile = m_callback.getUpdaterConfigurationXml();
    StringParser::canonizePath(StringParser::byProductFirst, updaterConfigurationFile.m_localPath, m_callback);

    if(updaterConfigurationFile.m_filename.empty())
    {
        TRACE_MESSAGE("Warning: updater configuration file has not been provided by product");
    }
    else
    {
        const Path fullPathToUpdaterConfiguration = updaterConfigurationFile.m_localPath + updaterConfigurationFile.m_filename;

        std::vector<SiteInfo> updateSites;
        SiteInfo::Description description;
        TRACE_MESSAGE2("Getting user defined sites information from file '%S' (using <site2> tag)",
            fullPathToUpdaterConfiguration.toWideChar());
        if(!SiteInfo().getSitesInfo(updateSites, description, fullPathToUpdaterConfiguration, false, pLog, true))
        {
            TRACE_MESSAGE2("Failed to get sites list (using tag <site2>) from '%S'. Parse error",
                fullPathToUpdaterConfiguration.toWideChar());
            return false;
        }


        if(updateSites.empty())
        {
            TRACE_MESSAGE2("Getting user defined sites information from file '%S' (using <site> tag)",
                fullPathToUpdaterConfiguration.toWideChar());
            if(SiteInfo().getSitesInfo(updateSites, description, fullPathToUpdaterConfiguration, true, pLog, true))
            {
                TRACE_MESSAGE3("Successfully got %d sites in old format (using tag <site>) from '%S'",
                    updateSites.size(), fullPathToUpdaterConfiguration.toWideChar());
            }
            else
            {
                TRACE_MESSAGE2("Failed to got sites in old format (using tag <site>) from '%S'",
                    fullPathToUpdaterConfiguration.toWideChar());
            }
        }


        std::vector<Path> sortedUpdateSites;
        SiteInfo().sortSites(updateSites, m_callback.updaterConfiguration().self_region,
            sortedUpdateSites, pLog);

        if(sortedUpdateSites.empty())
        {
            TRACE_MESSAGE("No site URL's found, adding site from hardcoded list");
            SiteInfo().appendRandomlyChosenHarcodedSite(sortedUpdateSites);
        }


        // append sorted sites from updcfg.xml to result
        for(std::vector<Path>::const_iterator iter = sortedUpdateSites.begin(); iter != sortedUpdateSites.end(); ++iter)
            ordered_sites_list.push_back(*iter);
    }

    // 3. adding randomly chosen site from predefined site list
    SiteInfo().appendRandomlyChosenHarcodedSite(ordered_sites_list);

    return true;
}

KLUPD::CoreError KLUPD::Updater::DoGetRemoteFile_SelfNet(
    const Path &fileName,
    const Path &localPath,
    const Path &relativeUrlPath,
    const Path &url)
{
    // file resides on some mounted resource 
    TRACE_MESSAGE5("Copying remote file '%S%S%S' -> '%S'", url.toWideChar(),
        relativeUrlPath.emptyIfSingleSlash().toWideChar(), fileName.toWideChar(), localPath.toWideChar());

    // set the URL, we want to get file from
    Address sourceAddress(url);

    // downloading file
    const Path sourceFullPath = sourceAddress.m_path.emptyIfSingleSlash() + relativeUrlPath.emptyIfSingleSlash() + fileName;
    const Path fullTargetPath = localPath.emptyIfSingleSlash() + fileName;

    const CoreError getFileResult = (sourceAddress.m_protocol == fileTransport)
        ? m_fileProtocol.getFile(sourceFullPath, fullTargetPath)
        : m_ftpProtocol.getFile(sourceAddress, sourceFullPath, fullTargetPath);

    if(getFileResult != CORE_NO_ERROR
        // to avoid duplicates in trace
        && !absentFile(getFileResult))
    {
        TRACE_MESSAGE3("Failed to get file '%S', result '%S'",
            fileName.toWideChar(), toString(getFileResult).toWideChar());
    }
    return getFileResult;
}

bool KLUPD::Updater::canBeDownloadedWithRegetting(bool &targetFileIsValid, const Path &fileToCheck, const FileInfo &etalon)const
{
    targetFileIsValid = false;

    // Only files consistent by signature 5 can be downloaded with regetting

    // Note: check if file in temporary folder suits has already been performed
     // in the tryFindLocalFileAndCopyToTemporaryFolder() function
 
    // Neither MD5, no check consistency by date or by signature 6 can be downloaded
    //   with regetting, because it is not possible to determine if we have complete
    //  local file from previous update set or not

    // signature 5 is unknown,
    if(etalon.m_signature5.empty())
        return false;

    // size must be known
    if(etalon.m_size == static_cast<size_t>(-1))
        return false;

    Log *suppressLog = 0;
    const size_t localFileSize = LocalFile(fileToCheck, suppressLog).size();

    // file does not exist or empty, thus can be downloaded with regetting
    if((localFileSize == static_cast<size_t>(-1)) || !localFileSize)
        return true;

    // local file size already exceeds etalon file size
    if(etalon.m_size < localFileSize)
        return false;


    // file can be downloaded with regetting, because it size if less then etalon
    if(localFileSize < etalon.m_size)
    {
        // in case local file has valid signature, but its size is less
        //  then expected, then it can *not* be downloaded with regetting
        return !checkInsideSignature5(fileToCheck);
    }


    // if etalon file size is equal to local file size
    //  and local file has valid signature, and regetting will succeed, otherwise fails
    targetFileIsValid = checkInsideSignature5AgainstEtalon(fileToCheck, etalon.m_signature5, pLog);
    if(targetFileIsValid)
    {
        TRACE_MESSAGE2("Regetting check detected that file '%S' is not needs to be downloaded, because valid and consistent",
            fileToCheck.toWideChar())
    }
    return targetFileIsValid;
}

KLUPD::CoreError KLUPD::Updater::downloadFileWithRegetting(const FileInfo &fileEntry, const Path &targetFolder, Signature6Checker &signature6Checker)
{
    const LocalFile tempFile(targetFolder + fileEntry.m_filename, pLog);

    // two download attempt in case file exists. File may exist but regetting feature may
    //  make it incorrect, because start of file does not suit for regetting trailer
    while(true)
    {
        bool targetFileIsValid = false;
        // deleting file in temporary folder, because it can not be downloaded with regetting
        if(!canBeDownloadedWithRegetting(targetFileIsValid, tempFile.m_name, fileEntry))
        {
            const CoreError unlinkResult = tempFile.unlink();
            if(!isSuccess(unlinkResult))
            {
                TRACE_MESSAGE3("Download with regetting failed, can not unlink file '%S', result '%S'",
                    tempFile.m_name.toWideChar(), toString(unlinkResult).toWideChar());
                return unlinkResult;
            }

        }
        // download is not needed, because file already present in target folder
        else if(targetFileIsValid)
            return CORE_NO_ERROR;


        // saving if file exists
        const bool tempFileWasEmpty = !tempFile.exists() || !tempFile.size();
        
        const CoreError result = downloadEntry(fileEntry, targetFolder);
        if(!isSuccess(result))
            return result;

        NoCaseString signature6ErrorMessage;
        if(checkSignature5And6AndMd5AgainstEtalon(targetFolder + fileEntry.m_filename, fileEntry,
            signature6Checker, signature6ErrorMessage, enableInsideSignatureWithLoadKey, m_callback, pLog))
        {
            TRACE_MESSAGE2("File '%S' downloaded with Regetting facility and checked successfully",
                tempFile.m_name.toWideChar());
            return CORE_NO_ERROR;
        }

        if(tempFileWasEmpty)
        {
            TRACE_MESSAGE3("File '%S' downloaded with Regetting, but is damaged (%S)",
                tempFile.m_name.toWideChar(), signature6ErrorMessage.toWideChar());
            return CORE_INVALID_SIGNATURE;
        }

        const CoreError unlinkResult = tempFile.unlink();
        if(!isSuccess(unlinkResult))
        {
            TRACE_MESSAGE3("Failed to remove invalid file '%S' downloaded with regetting, result '%S'",
                tempFile.m_name.toWideChar(), toString(unlinkResult).toWideChar());
            return unlinkResult;
        }

        TRACE_MESSAGE3("File '%S' downloaded with Regetting, but is damaged (%S), and was removed. Trying download from scratch",
            tempFile.m_name.toWideChar(), signature6ErrorMessage.toWideChar());
    }
}

KLUPD::CoreError KLUPD::Updater::downloadBases(UpdateInfo &update)
{
    for(size_t fileIndex = 0; fileIndex < update.m_matchFileList.size(); ++fileIndex)
    {
        FileInfo &file = update.m_matchFileList[fileIndex];

        // skip index and inlined files
        if(file.isIndex())
            continue;

        // download is not needed
        if(!file.downloadNeeded())
            continue;
        
        const CoreError result = downloadFileUsingCache(file, update);

        // query product if this file is not critical
        if(absentFile(result) || badFile(result))
        {
            // obligatory file is absent, stop execution
            if(!update.checkIfFileOptional(file))
                return result;
            update.removeComponentFromUpdate(file.m_componentIdSet, m_journal);

            // start parse from the very begin, because some elements
            //  could have been removed with downloadFileUsingCache() call
            fileIndex = static_cast<size_t>(-1);

            // optional file skipped
            continue;
        }

        // error occured while downloading
        if(result != CORE_NO_ERROR)
            return result;

        // give opportunity to product to check black list
        const Path downloadedFullFileName = m_callback.temporaryFolder(file) + file.m_filename;
        if(file.isBlackList() && !m_callback.checkBlackListFile(downloadedFullFileName))
        {
            TRACE_MESSAGE2("Black list check failed '%S'", downloadedFullFileName.toWideChar());
            return CORE_WRONG_KEY;
        }
        
        // update automatic schedule for Updater
        if(!update.m_retranslationMode
            && (file.m_filename == m_callback.getUpdaterConfigurationXml().m_filename))
        {
            UpdateSchedule automaticScheduleForUpdate;

            std::vector<SiteInfo> unused;
            SiteInfo::Description unused2;
            if(SiteInfo().parseSiteXML(unused, unused2, automaticScheduleForUpdate, downloadedFullFileName, false, pLog))
                m_callback.updateAutomaticScheduleForUpdater(automaticScheduleForUpdate);
        }
    }

    return CORE_NO_ERROR;      
}

bool KLUPD::Updater::tryFindLocalFileAndCopyToTemporaryFolder(const FileInfo &fileEntry, Signature6Checker &signature6Checker, const bool retranslationMode)const
{
    // primary index must be downloaded (saving file read operation number)
    if(fileEntry.m_primaryIndex)
        return false;

    // destination temporary folder to copy local file if found any
    const Path destinationFolder = m_callback.temporaryFolder(fileEntry);

    // to suppress un-needed trace messages
    Log *suppressLog = 0;
    Log *saveSignature6CheckerLog = signature6Checker.pLog;
    signature6Checker.pLog = 0;

    // search in temporary folder
    Path searchFolder = destinationFolder;
    NoCaseString unusedErrorMessage;
    if(checkSignature5And6AndMd5AgainstEtalon(searchFolder + fileEntry.m_filename, fileEntry,
        signature6Checker, unusedErrorMessage, disableInsideSignature, m_callback, suppressLog))
    {
        signature6Checker.pLog = saveSignature6CheckerLog;

        // additional protection against incorrect file in temporary folder
         // this condition can be used against file names collision in product folder
        LocalFile fileInTemporaryFolder(searchFolder + fileEntry.m_filename);
        if(fileEntry.m_size != fileInTemporaryFolder.size())
            return false;

        // return because no copies needed, because file is already in temporary folder
        TRACE_MESSAGE3("\t  Download is not requested, because local file found '%S' in temporary folder '%S'",
            (searchFolder + fileEntry.m_filename).toWideChar(), searchFolder.toWideChar());

        return true;
    }

    // try find local file in destination product folder, but for other
     //  current operation: update for retranslation operation and vice-versa
    {
        // avoid problem that several files from different relative URL location may go
         //  to single local path folder, and incorrect file is searched in product folder
        const bool skipDuplicateFile = retranslationMode && checkIfDuplicate(fileEntry);
        if(skipDuplicateFile)
        {
            TRACE_MESSAGE2("Local file is not going to be search into update product folder, because file is duplicate by update criteria %S",
                fileEntry.toString().toWideChar());
            signature6Checker.pLog = saveSignature6CheckerLog;
            return false;
        }

        searchFolder = m_callback.productFolder(fileEntry, !retranslationMode);
        NoCaseString unusedErrorMessage2;
        if(!checkSignature5And6AndMd5AgainstEtalon(searchFolder + fileEntry.m_filename,
            fileEntry, signature6Checker, unusedErrorMessage2, disableInsideSignature, m_callback, suppressLog))
        {
            signature6Checker.pLog = saveSignature6CheckerLog;
            return false;
        }
    }

    // Note: search in product folder is not needed, because it is done before, while forming update list

    signature6Checker.pLog = saveSignature6CheckerLog;


    // copy to temporary folder to reduce download number
    if(!isSuccess(LocalFile(searchFolder + fileEntry.m_filename, pLog).copy(destinationFolder + fileEntry.m_filename)))
        return false;

    TRACE_MESSAGE3("\t  Download is not requested, because local file found '%S' which is copied to '%S'",
        (searchFolder + fileEntry.m_filename).toWideChar(), destinationFolder.toWideChar());
    return true;
}

KLUPD::CoreError KLUPD::Updater::downloadFileUsingCache(const FileInfo &fileEntry, UpdateInfo &update)
{
    const Path fileNameOnSource = fileEntry.m_relativeURLPath + fileEntry.m_filename;

    // search if file has already been tried to download in current session
    std::map<Path, CoreError>::const_iterator downloadedPreviousFile = m_downloadedFilesInCurrentSession.find(fileNameOnSource);
    if(downloadedPreviousFile != m_downloadedFilesInCurrentSession.end())
    {
        TRACE_MESSAGE3("File has already been tried to be downloaded in this session '%S', result '%S'",
            fileNameOnSource.toWideChar(), toString(downloadedPreviousFile->second).toWideChar())
        return downloadedPreviousFile->second;
    }

    // file does not present in cache, and need to be downloaded in current update session
    const CoreError downloadResult = downloadFileWithAllCompressionMechanisms(fileEntry, update);

    // add file to cache
    m_downloadedFilesInCurrentSession[fileNameOnSource] = downloadResult;

    return downloadResult;
}

KLUPD::CoreError KLUPD::Updater::downloadFileWithAllCompressionMechanisms(const FileInfo &fileEntry, UpdateInfo &update)
{
    const Path temporaryFolder = m_callback.temporaryFolder(fileEntry);

    Path serverAddressForTrace;
    Source source;
    if(m_sourceList.getCurrentElement(source))
        serverAddressForTrace = source.m_url;

    // creating temporary folder
    if(!createFolder(temporaryFolder, pLog))
    {
        reportCanNotCreateFolder(temporaryFolder, "temporary folder for file");
        return CORE_FailedToCreateFolder;
    }

    // file content was encoded inside XML 'Body' attribute. No need to download it, just store to file
    if(fileEntry.isInlined())
    {
        LocalFile inlinedFile(temporaryFolder + fileEntry.m_filename, pLog);
        const CoreError inlinedFileWriteResult = inlinedFile.write(fileEntry.m_inlinedContent);
        if(isSuccess(inlinedFileWriteResult))
        {
            TRACE_MESSAGE2("\t Inlined file has been stored to disk '%S'", inlinedFile.m_name.toWideChar());
            return CORE_NO_ERROR;
        }
        TRACE_MESSAGE3("\t Failed to store inlined file content to disk '%S', result '%S'",
            inlinedFile.m_name.toWideChar(), toString(inlinedFileWriteResult).toWideChar());
        m_journal.publishMessage(inlinedFileWriteResult, inlinedFile.m_name.toWideChar());
        return inlinedFileWriteResult;
    }

    //////////////////////////////////////////////////////////////////////////
    /// 1. check if file already presents locally, then file does not needs to be downloaded
    if(tryFindLocalFileAndCopyToTemporaryFolder(fileEntry, update.m_signature6Checker, update.m_retranslationMode))
        return CORE_NO_ERROR;

    //////////////////////////////////////////////////////////////////////////
    /// 2. download with difference file for index compression
    if(fileEntry.isIndex())
    {
        const CoreError updateWithDifferenceFileForIndexResult = updateWithDifferenceFileForIndex(m_callback.productFolder(fileEntry, update.m_retranslationMode) + fileEntry.m_filename,
            fileEntry, temporaryFolder, update.m_signature6Checker);
        if(isSuccess(updateWithDifferenceFileForIndexResult))
            return CORE_NO_ERROR;
        if(criticalErrorForDifferenceFile(updateWithDifferenceFileForIndexResult))
        {
            reportNetworkEvent(updateWithDifferenceFileForIndexResult, serverAddressForTrace.toWideChar());
            return updateWithDifferenceFileForIndexResult;
        }
    }


    //////////////////////////////////////////////////////////////////////////
    /// 3. downloading with difference file compression mechanism

    // difference file can not be used for self-signed file,
     // because intermediate versions may not suit, though having correct inside-signature
    if(!fileEntry.isIndex()
        // signature 5 and md5 can be downloaded with difference file mechanism
        || !fileEntry.m_signature5.empty() || !fileEntry.m_md5.empty())
    {
        const CoreError updateWithDifferenceFileResult = updateWithDifferenceFile(m_callback.productFolder(fileEntry, update.m_retranslationMode)
            + fileEntry.m_filename, fileEntry, temporaryFolder, update.m_signature6Checker);
        if(isSuccess(updateWithDifferenceFileResult))
            return CORE_NO_ERROR;
        if(criticalErrorForDifferenceFile(updateWithDifferenceFileResult))
        {
            reportNetworkEvent(updateWithDifferenceFileResult, serverAddressForTrace.toWideChar());
            return updateWithDifferenceFileResult;
        }
    }

    //////////////////////////////////////////////////////////////////////////
    /// 4. downloading with difference file compression mechanism
    if(fileEntry.canBeDownloadedWithKlz())
    {
        const CoreError updateWithKlzResult = updateFileWithKlzCompression(fileEntry, temporaryFolder, update.m_signature6Checker);
        if(isSuccess(updateWithKlzResult))
        {
            TRACE_MESSAGE2("File '%S' has been got and checked with klz-file", (temporaryFolder + fileEntry.m_filename).toWideChar());
            return CORE_NO_ERROR;
        }
        if(criticalErrorForDifferenceFile(updateWithKlzResult))
        {
            reportNetworkEvent(updateWithKlzResult, serverAddressForTrace.toWideChar());
            return updateWithKlzResult;
        }
    }


    //////////////////////////////////////////////////////////////////////////
    /// 5. downloaded file with regetting mechanism
    const CoreError updateWithRegettingResult = downloadFileWithRegetting(fileEntry, temporaryFolder, update.m_signature6Checker);

    // report to journal file download result
    if(!isSuccess(updateWithRegettingResult))
    {
        Path relativeURLPath = fileEntry.m_relativeURLPath;
        if(// adjust path
            isPlainUpdateStructure(m_sourceList.currentUpdateStructure())
            // give full path for primary index
            && !fileEntry.m_primaryIndex)
        {
            relativeURLPath.erase();
        }
        m_journal.publishMessage(updateWithRegettingResult, (serverAddressForTrace + relativeURLPath + fileEntry.m_filename).toWideChar());
    }

    // downloading with regetting
    return updateWithRegettingResult;
}

KLUPD::CoreError KLUPD::Updater::updateWithDifferenceFileForIndex(const Path &productFullFileName,
        const FileInfo &fileEntry, const Path &targetTemporaryFolder, Signature6Checker &signature6Checker)
{
    // creating folder to download difference for index file into
    const Path differenceFolder = targetTemporaryFolder + L"ForDiff/";
    if(!createFolder(differenceFolder, pLog))
    {
        reportCanNotCreateFolder(differenceFolder, "index difference folder");
        return CORE_FailedToCreateFolder;
    }

    // can not apply difference file, because original file does not exist
    if(!LocalFile(productFullFileName, pLog).exists())
        return CORE_INTERNAL_ERROR;

    // downloading difference full file name, by appending hardcoded ".dif" string to file name
    const Path hardcodedDifferenceForIndexExtension = L".dif";
    const Path differenceFileName = fileEntry.m_filename + hardcodedDifferenceForIndexExtension;

    const CoreError unlinkDifferenceFileResult = LocalFile(differenceFolder + differenceFileName, pLog).unlink();
    if(!isSuccess(unlinkDifferenceFileResult))
    {
        TRACE_MESSAGE4("File '%S' can not be downloaded with difference for index compression mechanism, failed to unlink '%S', result '%S'",
            productFullFileName.toWideChar(), (differenceFolder + differenceFileName).toWideChar(), toString(unlinkDifferenceFileResult).toWideChar());
        return unlinkDifferenceFileResult;
    }

    const CoreError downloadResult = downloadEntry(FileInfo(differenceFileName, fileEntry.m_relativeURLPath,
        fileEntry.m_type, fileEntry.m_primaryIndex), differenceFolder);
    if(!isSuccess(downloadResult))
    {
        TRACE_MESSAGE3("File '%S' can not be downloaded with difference for index compression mechanism, result '%S'",
            productFullFileName.toWideChar(), toString(downloadResult).toWideChar());
        return downloadResult;
    }

    const Path applyDifferenceOnIndexResult = differenceFolder + fileEntry.m_filename;
    const CoreError unlinkApplyResultFileResult = LocalFile(applyDifferenceOnIndexResult, pLog).unlink();
    if(!isSuccess(unlinkApplyResultFileResult))
    {
        TRACE_MESSAGE3("Failed to unlink difference apply result file on index '%S', result '%S'",
            applyDifferenceOnIndexResult.toWideChar(), toString(unlinkApplyResultFileResult).toWideChar());
        return unlinkApplyResultFileResult;
    }
    if(!applyDifferenceOnIndexFile(productFullFileName, differenceFolder + differenceFileName, applyDifferenceOnIndexResult, pLog))
    {
        // difference was not applied
        TRACE_MESSAGE4("Failed to apply difference on index '%S', difference file '%S', destination '%S'",
            productFullFileName.toWideChar(), (differenceFolder + differenceFileName).toWideChar(), applyDifferenceOnIndexResult.toWideChar());
        return CORE_INTERNAL_ERROR;
    }

    TRACE_MESSAGE4("Difference applies on index '%S', difference '%S', destination '%S'", productFullFileName.toWideChar(),
        (differenceFolder + differenceFileName).toWideChar(), applyDifferenceOnIndexResult.toWideChar());

    // checking file downloaded with difference file compression mechanism
    NoCaseString signature6ErrorMessage;
    if(checkSignature5And6AndMd5AgainstEtalon(applyDifferenceOnIndexResult, fileEntry,
        signature6Checker, signature6ErrorMessage, enableInsideSignatureWithLoadKey, m_callback, pLog))
    {
        // move file to temporary folder
        const CoreError moveFileResult = LocalFile(applyDifferenceOnIndexResult).rename(targetTemporaryFolder + fileEntry.m_filename);
        if(!isSuccess(moveFileResult))
        {
            TRACE_MESSAGE4("Failed to apply difference file: unable to move file '%S' to '%S', result '%S'",
                applyDifferenceOnIndexResult.toWideChar(), targetTemporaryFolder.toWideChar(), toString(moveFileResult).toWideChar());
            return moveFileResult;
        }

        TRACE_MESSAGE2("File '%S' has been downloaded with difference for index file. Validity check passed",
            (targetTemporaryFolder + fileEntry.m_filename).toWideChar());
        return CORE_NO_ERROR;
    }

    // check was not successful, try to apply recursive difference file is required
    TRACE_MESSAGE2("Difference file has been applied on file '%S', bug signature check failed",
        (targetTemporaryFolder + fileEntry.m_filename).toWideChar());
    return CORE_INTERNAL_ERROR;
}

KLUPD::CoreError KLUPD::Updater::updateWithDifferenceFile(const Path &productFullFileName, const FileInfo &fileEntry,
                                                          const Path &targetTemporaryFolder, Signature6Checker &signature6Checker)
{
    // can not apply difference file, because original file does not exist
    if(!LocalFile(productFullFileName, pLog).exists())
        return CORE_INTERNAL_ERROR;

    // creating difference folder
    const Path differenceFolder = targetTemporaryFolder + L"ForDiff/";
    if(!createFolder(differenceFolder, pLog))
    {
        reportCanNotCreateFolder(differenceFolder, "folder to download difference files");
        return CORE_FailedToCreateFolder;
    }

    Path differnceFileInput = productFullFileName;
    Path previousDifferenceFileNameAgainsCycling;

    // cycle of downloading and applying difference files
    while(true)
    {
        // calculate difference path for current source,
         // and calculate file extension by hashing file content
        Source currentSource;
        if(!m_sourceList.getCurrentElement(currentSource))
        {
            TRACE_MESSAGE("Unable to generate difference file extension, because can not get current source");
            return CORE_URLS_EXHAUSTED;
        }
        Path differenceFileExtension;
        const Path differenceFileRelativeUrlPath = currentSource.m_differenceFolder + fileEntry.m_relativeURLPath.emptyIfSingleSlash();
        if(!makeDifferenceFileExtension(differnceFileInput, differenceFileExtension))
        {
            TRACE_MESSAGE2("Failed to create difference file name for '%S'", differnceFileInput.toWideChar());
            return CORE_INTERNAL_ERROR;
        }

        const Path differenceFileName = fileEntry.m_filename + L"." + differenceFileExtension;
        if(previousDifferenceFileNameAgainsCycling == differenceFileName)
        {
            TRACE_MESSAGE3("Error: cycling in difference file apply algorithm '%S', the name of difference file does not differs from previous one '%S'",
                differnceFileInput.toWideChar(), differenceFileName.toWideChar());
            return CORE_INTERNAL_ERROR;
        }
        previousDifferenceFileNameAgainsCycling = differenceFileName;

        // do not download difference file if it exists already
        if(LocalFile(differenceFolder + differenceFileName, pLog).exists())
        {
            TRACE_MESSAGE2("Local difference file found, using difference file from local cache '%S'",
                (differenceFolder + differenceFileName).toWideChar());
        }
        else
        {
            // difference file does not exist, so download it
            const CoreError downloadResult = downloadEntry(FileInfo(differenceFileName, differenceFileRelativeUrlPath,
                fileEntry.m_type, fileEntry.m_primaryIndex), differenceFolder, true);
            if(!isSuccess(downloadResult))
            {
                TRACE_MESSAGE3("File '%S' can not be downloaded with difference file compression mechanism, result '%S'",
                    productFullFileName.toWideChar(), toString(downloadResult).toWideChar());
                return downloadResult;
            }
        }

        const Path applyDifferenceResult = targetTemporaryFolder + fileEntry.m_filename;
        if(!diffsApplySingleDiff(differnceFileInput, differenceFolder + differenceFileName, applyDifferenceResult, pLog))
        {
            // difference was not applied
            TRACE_MESSAGE4("Failed to apply difference on file '%S', difference file '%S', destination '%S'",
                differnceFileInput.toWideChar(), (differenceFolder + differenceFileName).toWideChar(), applyDifferenceResult.toWideChar());
            return CORE_INTERNAL_ERROR;
        }

        TRACE_MESSAGE4("Difference applies on file '%S', difference '%S', destination '%S'",
            differnceFileInput.toWideChar(), (differenceFolder + differenceFileName).toWideChar(), applyDifferenceResult.toWideChar());


        // checking file downloaded with difference file compression mechanism
        NoCaseString signature6ErrorMessage;
        if(checkSignature5And6AndMd5AgainstEtalon(applyDifferenceResult, fileEntry, signature6Checker, signature6ErrorMessage, enableInsideSignature, m_callback, pLog))
        {
            TRACE_MESSAGE2("File '%S' has been downloaded with difference file. Validity check passed", applyDifferenceResult.toWideChar());
            return CORE_NO_ERROR;
        }

        // check was not successful, try to apply recursive difference file is required
        TRACE_MESSAGE2("Difference file has been applied on file '%S', trying to download next recursive difference file",
            productFullFileName.toWideChar());

        // rename file to apply next difference on it
        differnceFileInput = applyDifferenceResult;
    }
}

KLUPD::CoreError KLUPD::Updater::updateFileWithKlzCompression(const FileInfo &fileEntry, const Path &targetFolder, Signature6Checker &signature6Checker)
{
    Path klzRelativeUrlPath;
    Path klzFileName;
    makeKlzCompressionFileName(fileEntry.m_relativeURLPath, fileEntry.m_filename, klzRelativeUrlPath, klzFileName);
    LocalFile klzLocalFile(targetFolder + klzFileName, pLog);
    bool fileExisted = klzLocalFile.exists();

    // incorrect previously downloaded klz-file (or klz-file part) can present locally
    //  1. downloaded klz file remainder with regetting
    //  2. local file signature is checked
    //  3. in case incorrect signature file is deleted and re-downloaded from scratch
    while(true)
    {
        // downloading *.klz file
        const FileInfo fileInfo(klzFileName, klzRelativeUrlPath, fileEntry.m_type, fileEntry.m_primaryIndex);

        // klz-file must not be re-used, because inside signature 6 allows to accept the obsolete files
        CoreError unlinkKlzFileResult = klzLocalFile.unlink();
        if(!isSuccess(unlinkKlzFileResult))
        {
            TRACE_MESSAGE3("Failed to unlink klz-file '%S', result '%S'",
                klzLocalFile.m_name.toWideChar(), toString(unlinkKlzFileResult).toWideChar());
            return unlinkKlzFileResult;
        }

        const CoreError downloadResult = downloadEntry(fileInfo, targetFolder);
        if(!isSuccess(downloadResult))
            return downloadResult;

        LocalFile klzTargetFile(targetFolder + fileEntry.m_filename, pLog);
        const CoreError unlinkKlzTargetResult = klzTargetFile.unlink();
        if(!isSuccess(unlinkKlzTargetResult))
        {
            TRACE_MESSAGE3("Failed to unlink klz-target file '%S', result '%S'",
                klzTargetFile.m_name.toWideChar(), toString(unlinkKlzTargetResult).toWideChar());
            return unlinkKlzTargetResult;
        }
        
        if(!unpackSingleKLZ(targetFolder + klzFileName, targetFolder + fileEntry.m_filename, pLog))
            return CORE_INTERNAL_ERROR;

        NoCaseString signature6ErrorMessage;
        if(checkSignature5And6AndMd5AgainstEtalon(targetFolder + fileEntry.m_filename, fileEntry,
            signature6Checker, signature6ErrorMessage, enableInsideSignatureWithLoadKey, m_callback, pLog))
        {
            return CORE_NO_ERROR;
        }

        // file was downloaded from scratch, but check failed
        if(!fileExisted)
            return CORE_INVALID_SIGNATURE;

        // file or file part presented, try to delete it and re-download from scratch
        unlinkKlzFileResult = klzLocalFile.unlink();
        if(!isSuccess(unlinkKlzFileResult))
        {
            TRACE_MESSAGE3("Failed to unlink damaged Klz-file which was downloaded with regetting '%S', result '%S'",
                klzLocalFile.m_name.toWideChar(), toString(unlinkKlzFileResult).toWideChar());
            return unlinkKlzFileResult;
        }

        TRACE_MESSAGE2("Damaged Klz-file has downloaded with Regetting on file '%S', trying to download klz-file from scratch",
            klzLocalFile.m_name.toWideChar());
        fileExisted = false;
    }
}

KLUPD::CoreError KLUPD::Updater::replaceFiles(UpdateInfo &update)
{
    if(!m_callback.checkDownloadedFiles(update.m_matchFileList, update.m_retranslationMode))
    {
        TRACE_MESSAGE("** Check downloaded files by application failed **");
        return CORE_BASE_CHECK_FAILED;
    }

    UpdaterTransaction updaterTransaction(m_callback.rollbackFolder(update.m_retranslationMode), update.m_retranslationMode,
        m_downloadProgress, m_callback, m_journal, pLog);
    const CoreError transactionResult = updaterTransaction.run(update.m_matchFileList);
    if(!isSuccess(transactionResult))
        return transactionResult;

    // index processed successfully
    
    // write additional variable from index XML file to settings storage
    for(std::vector<UpdatedSettings>::const_iterator updatedSettingsIter = update.m_updatedSettings.begin(); updatedSettingsIter != update.m_updatedSettings.end(); ++updatedSettingsIter)
    {
        if(m_callback.updateVariableFromIndex(*updatedSettingsIter, update.m_retranslationMode))
        {
            TRACE_MESSAGE2("Variable is written to settings storage: %S",
                updatedSettingsIter->toString().toWideChar());
        }
        else
        {
            TRACE_MESSAGE2("Failed to write variable to settings storage: %S",
                updatedSettingsIter->toString().toWideChar());
        }
    }

    // inform application about all files in set
    sendLocalFilesToProduct(update);
    return CORE_NO_ERROR;
}

void KLUPD::Updater::sendLocalFilesToProduct(const UpdateInfo &update)
{
    const bool localBasesWereConsistent = update.m_retranslationMode ? m_retranslationLocalBasesAreConsistent : m_updateLocalBasesAreConsistent;
    TRACE_MESSAGE4("Saving updater configuration (files number %d) in %s mode, local bases were %s before update",
        update.m_matchFileList.size(), update.m_retranslationMode ? "retranslation" : "update",
        localBasesWereConsistent ? "consistent" : "damaged");

	if(m_callback.processReceivedFiles( update.m_matchFileList, localBasesWereConsistent,
                                        m_callback.rollbackFolder(update.m_retranslationMode),
                                        update.m_retranslationMode,
                                        GetUpdateDate(update.m_matchFileList) ))
    {
        TRACE_MESSAGE2("Result list has been saved in %s mode", update.m_retranslationMode ? "retranslation" : "update");
    }
    else
    {
        TRACE_MESSAGE2("Failed to save update lists in %s mode", update.m_retranslationMode ? "retranslation" : "update");
    }
}
