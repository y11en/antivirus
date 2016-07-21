#include "../include/ver_mod.h"

#include "../helper/stdinc.h"
#include "../helper/updaterStaff.h"
#include "../helper/update_transaction.h"

#include "updater.h"

void KLUPD::Updater::outputUpdaterVersionToTrace()const
{
    std::ostringstream stream;
    stream.imbue(std::locale::classic());
    stream << "Updater build: " << UPD_VER_MAIN;

#if (defined _WIN32) || (defined WIN32) || (defined(__APPLE__))
    stream << ", compiled for: " << VER_PRODUCTNAME_STR
        << ":" << VER_PRODUCTVERSION_HIGH << "." << VER_PRODUCTVERSION_LOW << "." << VER_FILEVERSION_BUILD << "." << VER_FILEVERSION_COMPILATION;
#else
    stream << ", project version " << AM_PROJECT_VERSION << ", component build " << AM_COMPONENT_BUILD_NUMBER << ", project build " << AM_PROJECT_BUILD_NUMBER;
#endif


    // output executable file name to trace
    #ifdef _WIN32
        TCHAR executableFileName[_MAX_PATH + 1];
        memset(executableFileName, 0, _MAX_PATH + 1);
        if(GetModuleFileName(0, executableFileName, _MAX_PATH))
            stream << std::endl << "\tExecutable file name: " << tStringToAscii(executableFileName);
    #endif

    TRACE_MESSAGE(stream.str().c_str());
}

void KLUPD::Updater::reportCanNotCreateFolder(const Path &folder, const std::string &additionalInfo)const
{
	TRACE_MESSAGE3("Failed to create folder (%s) '%S'", additionalInfo.c_str(), folder.toWideChar());
    m_journal.publishMessage(CORE_FailedToCreateFolder, folder.m_value);
}

KLUPD::CoreError KLUPD::Updater::initialize()
{
    m_retranslationDone = !m_callback.updaterConfiguration().retranslationRequired();
    m_updateDone = !m_callback.updaterConfiguration().updateRequired();

    m_retranslationResult = m_updateResult = CORE_NO_ERROR;

    ///////////////////////////////////////////////////////
    // Prepare to Updater and check input configuration
    outputUpdaterVersionToTrace();

    // fix region settings
    if(m_callback.updaterConfiguration().self_region.empty())
	{
        TRACE_MESSAGE("Empty self_region, set to '*'");
        m_callback.updaterConfiguration().self_region = L"*";
    }
    if(m_callback.updaterConfiguration().self_region == SS_KEY_UseDefaultRegion)
    {
        m_callback.updaterConfiguration().self_region = getPreferredRegion(pLog);
        TRACE_MESSAGE2("Region got from OS settings: %S", m_callback.updaterConfiguration().self_region.toWideChar());
    }

    TRACE_MESSAGE2("Updater settings:\n%S", m_callback.updaterConfiguration().toString().toWideChar());

    // stop if no operation requested
	if(m_updateDone && m_retranslationDone)
    {
        TRACE_MESSAGE2("***** Update operation result: %S *****",
            toString(CORE_NO_OPERATION_REQUESTED).toWideChar());
        m_retranslationResult = m_updateResult = CORE_NO_OPERATION_REQUESTED;
		return CORE_NO_OPERATION_REQUESTED;
    }

    // create temporary folder
    if(!createFolder(m_callback.temporaryFolder(), pLog))
    {
        reportCanNotCreateFolder(m_callback.temporaryFolder(), "temporary folder");
        m_retranslationResult = m_updateResult = CORE_FailedToCreateFolder;
        // fatal erron, failed to create temporary folder
        return CORE_FailedToCreateFolder;
    }

    // create retranslation product folder
    if(!m_retranslationDone && !createFolder(m_callback.productFolder(true), pLog))
    {
        // retranslation error is not fatal, because update operation may still be successful
        reportCanNotCreateFolder(m_callback.productFolder(true), "retranslation product folder");
        m_retranslationDone = true;
        m_retranslationResult = CORE_FailedToCreateFolder;
    }

    // create update temporary and product folders
    if(!m_updateDone && !createFolder(m_callback.productFolder(false), pLog))
    {
        reportCanNotCreateFolder(m_callback.productFolder(false), "update product folder");
        m_updateResult = CORE_FailedToCreateFolder;
        if(!m_retranslationDone)
            m_retranslationResult = CORE_FailedToCreateFolder;
        return CORE_FailedToCreateFolder;
    }

    return CORE_NO_ERROR;
}

bool KLUPD::Updater::downloadPrimaryIndex(UpdateInfo &retranslationInfo, UpdateInfo &updateInfo)
{
    CoreError downloadPrimaryIndexResult = CORE_NO_ERROR;
    if(!m_updateDone)
    {
        const FileInfo updatePrimaryIndex = getPrimaryIndexFileInfo(false, false);
        TRACE_MESSAGE2("========= Downloading primary index to temporary folder %S for update operation =========",
            m_callback.temporaryFolder(updatePrimaryIndex).toWideChar());
        downloadPrimaryIndexResult = downloadFileUsingCache(updatePrimaryIndex, updateInfo);
    }

    // update operation is more critical, if dowload primary index
     // for update fails, then both update and retranslation fails
    if(!m_retranslationDone
        && (downloadPrimaryIndexResult == CORE_NO_ERROR))
    {
        const FileInfo retranslationPrimaryIndex = getPrimaryIndexFileInfo(false, true);
        TRACE_MESSAGE2("========= Downloading primary index to temporary folder %S for retranslation operation =========",
            m_callback.temporaryFolder(retranslationPrimaryIndex).toWideChar());
        downloadPrimaryIndexResult = downloadFileUsingCache(retranslationPrimaryIndex, retranslationInfo);
    }
    TRACE_MESSAGE2("========= Downloading primary index result %S =========",
        toString(downloadPrimaryIndexResult).toWideChar());


    /// failed to download primary index
    if(downloadPrimaryIndexResult != CORE_NO_ERROR)
    {
        if(!m_retranslationDone)
            m_retranslationResult = downloadPrimaryIndexResult;
        if(!m_updateDone)
            m_updateResult = downloadPrimaryIndexResult;

        TRACE_MESSAGE2("Primary index not downloaded, result %S",
            toString(m_updateResult).toWideChar());

        // download primary index failed try to ask index from application
        return false;
    }

    /// primary index downloaded

    // check if consistent and enumerate local files
    if(!m_localFilesEnumerated)
    {
        m_downloadProgress.percentAdjustState(DownloadProgress::checkingLocalFiles);
        m_journal.publishMessage(EVENT_GeneratingFileListToDownload);

        // Warning: enumerate local files is time consuming operation
        if(!m_retranslationDone)
            m_retranslationLocalBasesAreConsistent = EnumerateLocalFiles(m_retranslationLocalFiles, true);
        if(!m_updateDone)
            m_updateLocalBasesAreConsistent = EnumerateLocalFiles(m_updateLocalFiles, false);

        m_localFilesEnumerated = true;
    }

    m_downloadProgress.percentAdjustState(DownloadProgress::downloadingIndex);

    // copy primary index to retranslation temporary folder
    if(!m_retranslationDone)
    {
        // parse retranslation primary index
        const CoreError parseResult = parsePrimaryIndex(retranslationInfo);

        // files are up-to-date in retranslation folder
        if(isNoOperationRequestedCode(parseResult))
        {
            m_retranslationResult = parseResult;
            m_retranslationDone = true;
        }
        // error happened while parsing index
        else if(parseResult != CORE_NO_ERROR)
        {
            TRACE_MESSAGE2("Failed to parse primary index in retranslation temporary folder %S",
                toString(parseResult).toWideChar());
            m_retranslationResult = parseResult;
            if(!m_updateDone)
                m_updateResult = parseResult;
            return false;
        }
    }

    // parse update primary index
    if(!m_updateDone)
    {
        // parse update primary index
        const CoreError parseResult = parsePrimaryIndex(updateInfo);

        // files are up-to-date in update folder
        if(isNoOperationRequestedCode(parseResult))
        {
            m_updateResult = parseResult;
            m_updateDone = true;
        }
        // error happened while parsing index
        else if(parseResult != CORE_NO_ERROR)
        {
            TRACE_MESSAGE2("Failed to parse primary index in update temporary folder %S",
                toString(parseResult).toWideChar());
            m_updateResult = parseResult;
            return false;
        }
    }
    return true;
}

void KLUPD::Updater::calculateDuplicatesForRetranslation(const FileVector &retranslationFiles)
{
    for(FileVector::const_iterator current = retranslationFiles.begin(); current != retranslationFiles.end(); ++current)
    {
        if(current + 1 == retranslationFiles.end())
            return;

        for(FileVector::const_iterator fileIter = current + 1; fileIter != retranslationFiles.end(); ++fileIter)
        {
            if((current->m_filename == fileIter->m_filename)
                && (current->m_localPath == fileIter->m_localPath)
                && (current->m_relativeURLPath != fileIter->m_relativeURLPath))
            {
                m_duplicatesForRetranslation.push_back(*current);
            }
        }
    }
}
void KLUPD::Updater::cleanDuplicatesForRetranslation()
{
    m_duplicatesForRetranslation.clear();
}
bool KLUPD::Updater::checkIfDuplicate(const FileInfo &file)const
{
    for(FileVector::const_iterator fileIter = m_duplicatesForRetranslation.begin(); fileIter != m_duplicatesForRetranslation.end(); ++fileIter)
    {
        if((file.m_filename == fileIter->m_filename)
            && (file.m_localPath == fileIter->m_localPath))
        {
            return true;
        }
    }
    return false;
}

void KLUPD::Updater::downloadFromSource(bool &primaryIndexDownloadedAndParsed)
{
    if(m_downloadProgress.checkCancel())
    {
        if(!m_retranslationDone)
            m_retranslationResult = CORE_CANCELLED;
        if(!m_updateDone)
            m_updateResult = CORE_CANCELLED;
        m_retranslationDone = m_updateDone = true;
        return;
    }

    primaryIndexDownloadedAndParsed = false;

    Signature6Checker downloadedFilesSignature6Checker(pLog);
    UpdateInfo retranslationInfo(m_callback.updaterConfiguration().filterFlags(true),
        m_callback.updaterConfiguration().filterComponents(true),
        downloadedFilesSignature6Checker, true,
        m_callback, pLog);
    UpdateInfo updateInfo(m_callback.updaterConfiguration().filterFlags(false),
        m_callback.updaterConfiguration().filterComponents(false),
        downloadedFilesSignature6Checker, false,
        m_callback, pLog);

    if(!downloadPrimaryIndex(retranslationInfo, updateInfo))
        return;

    // this is second attempt to download file from this source,
     //  and bases date has not changed switch to next source
    primaryIndexDownloadedAndParsed = true;
    if(m_basesDateAtStartUpdate == m_basesDateCurrentIteration)
    {
        TRACE_MESSAGE2("Index downloaded second time, but the version on the source is the same %S",
            m_basesDateCurrentIteration.toWideChar());
        return;
    }

    // the bases changed on server, reset code from previous failure update (only in case previous update failed
    //  reset of code is delayed up to this moment, *because* otherwise result code from previous iteration is lost
    if(!m_retranslationDone && !isNoOperationRequestedCode(m_retranslationResult)
        && !absentFile(m_retranslationResult))
    {
        m_retranslationResult = CORE_NO_ERROR;
    }
    if(!m_updateDone && !isNoOperationRequestedCode(m_updateResult)
        && !absentFile(m_updateResult))
    {
        m_updateResult = CORE_NO_ERROR;
    }



    //////////////////////////////////////////////////////////////////////////
    //// retranslation secondary index download
    if(!m_retranslationDone && m_retranslationResult == CORE_NO_ERROR)
    {
        TRACE_MESSAGE("========= download index for retranslation operation started =========");

        m_retranslationResult = downloadSecondaryIndexes(retranslationInfo);
        m_retranslationDone = isNoOperationRequestedCode(m_retranslationResult);

        TRACE_MESSAGE2("========= download index for retranslation operation, result: %S =========",
            toString(m_retranslationResult).toWideChar());

        if(m_retranslationResult == CORE_CANCELLED)
            return;

        // switch to next update source
        if(isNetworkError(m_retranslationResult))
            return;
    }
    //////////////////////////////////////////////////////////////////////////
    //// update secondary index download
    if(!m_updateDone && m_updateResult == CORE_NO_ERROR)
    {
        TRACE_MESSAGE("========= download index for update operation started =========");

        m_updateResult = downloadSecondaryIndexes(updateInfo);
        m_updateDone = isNoOperationRequestedCode(m_updateResult);

        TRACE_MESSAGE2("========= download index for update operation, result: %S =========",
            toString(m_updateResult).toWideChar());

        if(m_updateResult == CORE_CANCELLED)
            return;

        // switch to next update source
        if(isNetworkError(m_updateResult))
            return;
    }


    m_downloadProgress.percentAdjustState(DownloadProgress::indexDownloaded);

    //////////////////////////////////////////////////////////////////////////
    //// retranslation black list date check
    if(!m_retranslationDone && m_retranslationResult == CORE_NO_ERROR)
    {
        if(!checkBlackListPresence(retranslationInfo.m_matchFileList, m_retranslationBlackListLimit, pLog))
        {
            TRACE_MESSAGE("Black list date check for retranslation operation failed");
            m_retranslationDone = m_updateDone = true;
            m_retranslationResult = m_updateResult = CORE_WRONG_KEY;
        }

        if(!checkBlackDateLimit(retranslationInfo.m_matchFileList, m_retranslationBlackListLimit, true))
        {
            TRACE_MESSAGE("Retranslation black list date limit check failed");
            m_retranslationDone = m_updateDone = true;
            m_retranslationResult = m_updateResult = CORE_WRONG_KEY;
        }
    }
    //////////////////////////////////////////////////////////////////////////
    //// update black list date check
    if(!m_updateDone && m_updateResult == CORE_NO_ERROR)
    {
        if(!checkBlackListPresence(updateInfo.m_matchFileList, m_updateBlackListLimit, pLog))
        {
            TRACE_MESSAGE("Black list date check for update operation failed");
            m_retranslationDone = m_updateDone = true;
            m_retranslationResult = m_updateResult = CORE_WRONG_KEY;
        }

        if(!checkBlackDateLimit(updateInfo.m_matchFileList, m_updateBlackListLimit, false))
        {
            TRACE_MESSAGE("Update black list date limit check failed");
            m_retranslationDone = m_updateDone = true;
            m_retranslationResult = m_updateResult = CORE_WRONG_KEY;
        }
    }

    //////////////////////////////////////////////////////////////////////////
    /// give opportunity for product to check and cancel update
    if(!m_retranslationDone && m_retranslationResult == CORE_NO_ERROR)
    {
        KLUPD::NoCaseString strUpdateDate = GetUpdateDate(retranslationInfo.m_matchFileList);
        if(!m_callback.checkFilesToDownload(retranslationInfo.m_matchFileList, strUpdateDate, retranslationInfo.m_retranslationMode))
        {
            m_retranslationResult = CORE_BASE_CHECK_FAILED;
            m_retranslationDone = true;

            m_journal.publishMessage(m_retranslationResult, strUpdateDate);
            TRACE_MESSAGE2("Product check of files to retranslate failed, result '%S'",
                toString(m_retranslationResult).toWideChar());
        }
    }

    if(!m_updateDone && m_updateResult == CORE_NO_ERROR)
    {
        KLUPD::NoCaseString strUpdateDate = GetUpdateDate(updateInfo.m_matchFileList);
        if(!m_callback.checkFilesToDownload(updateInfo.m_matchFileList, strUpdateDate, updateInfo.m_retranslationMode))
        {
            m_updateResult = CORE_BASE_CHECK_FAILED;
            m_updateDone = true;

            m_journal.publishMessage(m_updateResult, strUpdateDate);
            TRACE_MESSAGE2("Product check of files to update failed, result '%S'",
                toString(m_updateResult).toWideChar());
        }
    }


    //////////////////////////////////////////////////////////////////////////
    /// calculating amount of data to download
    calculatingDownloadSize(updateInfo, retranslationInfo);


    //////////////////////////////////////////////////////////////////////////
    //// update bases download
    if(!m_updateDone && isSuccess(m_updateResult))
    {
        TRACE_MESSAGE("========= download update bases operation started =========");
        m_updateResult = downloadBases(updateInfo);
        TRACE_MESSAGE2("========= download update bases result: %S =========",
            toString(m_updateResult).toWideChar());

        if(m_updateResult == CORE_CANCELLED)
            return;
        // switch to next update source
        if(isNetworkError(m_updateResult))
            return;
    }


    //////////////////////////////////////////////////////////////////////////
    //// retranslation bases download

    /// Warning: bases for retranslation are downloaded ONLY after update,
     //   because retranslation in most products option and can be turned on/off.
     //  By downloading files for retranslation operation difference files will save traffic.
     //  Suppose situation when retranslation is temprary switched off, but update is
     //   not switched (because in most products it is not possible to turn off).
     //  Then all downloaded and applied files will be reused, and download
     //   retranslation file is not performed from the stop of retranslation point
    if(!m_retranslationDone && isSuccess(m_retranslationResult))
    {
        TRACE_MESSAGE("========= download retranslation bases operation started =========");
        // download retranslation files
        calculateDuplicatesForRetranslation(retranslationInfo.m_matchFileList);
        m_retranslationResult = downloadBases(retranslationInfo);
        cleanDuplicatesForRetranslation();
        TRACE_MESSAGE2("========= download retranslation bases result: %S =========",
            toString(m_retranslationResult).toWideChar());

        if(m_retranslationResult == CORE_CANCELLED)
            return;

        // switch to next update source
        if(isNetworkError(m_retranslationResult))
            return;
    }


    //////////////////////////////////////////////////////////////////////////
    //// report if files are need to be replaced
    const bool retranslationNeedReplaceFiles = (!m_retranslationDone && isSuccess(m_retranslationResult));
    const bool updateNeedReplaceFiles = (!m_updateDone && isSuccess(m_updateResult));
    if(retranslationNeedReplaceFiles || updateNeedReplaceFiles)
        m_downloadProgress.percentAdjustState(DownloadProgress::replacingFiles);

    // close connections to reduce load on update sources,
     // thus save few seconds while files are copied
    if((m_retranslationDone || retranslationNeedReplaceFiles)
        && (m_updateDone || updateNeedReplaceFiles))
    {
        m_httpProtocol.closeSession();
        m_ftpProtocol.closeSession();
        m_administrationKitProtocol.closeSession();
    }

    //////////////////////////////////////////////////////////////////////////
    //// replacing retranslation bases
    if(retranslationNeedReplaceFiles)
    {
        TRACE_MESSAGE("========= replacing retranslation bases operation started =========");
        m_journal.publishMessage(EVENT_StartInstallFilesForRetranslation);
        m_retranslationResult = replaceFiles(retranslationInfo);
        m_retranslationDone = true;
        TRACE_MESSAGE2("========= replacing retranslation bases result: %S =========",
            toString(m_retranslationResult).toWideChar());
    }


    //////////////////////////////////////////////////////////////////////////
    //// replacing update bases download
    if(updateNeedReplaceFiles)
    {
        TRACE_MESSAGE("========= replacing update bases operation started =========");
        m_journal.publishMessage(EVENT_StartInstallFilesForUpdate);
        m_updateResult = replaceFiles(updateInfo);
        m_updateDone = true;
        TRACE_MESSAGE2("========= replacing update bases result: %S =========",
            toString(m_updateResult).toWideChar());
    }
}

void KLUPD::Updater::downloadFromAllSources()
{
    m_journal.publishMessage(EVENT_TaskStarted);

    // build and dump source list
    if(!constructSourceList(m_callback.updaterConfiguration().m_sourceList, m_sourceList))
        TRACE_MESSAGE("Failed to construct source list");

    TRACE_MESSAGE2("Source list:\n%S", m_sourceList.toString().toWideChar());
    if(m_sourceList.m_sources.empty())
    {
        if(!m_retranslationDone)
        {
            m_retranslationDone = true;
            m_retranslationResult = CORE_URLS_EXHAUSTED;
        }
        if(!m_updateDone)
        {
            m_updateDone = true;
            m_updateResult = CORE_URLS_EXHAUSTED;
        }
        return;
    }

    // try update from all sources
    m_sourceList.moveFirst();
    reportSourceSelected();


    Source source;
    while(m_sourceList.getCurrentElement(source))
    {
        // flag indicates whether primary index has been downloaded and parsed.
        //  If not then switch to next source should be performed
        bool primaryIndexDownloadedAndParsed = false;

        downloadFromSource(primaryIndexDownloadedAndParsed);

        if(m_retranslationDone && m_updateDone)
            return;

        ////////////////////////////////////////
        /// switch to next source if needed

        // not network failure, then no need to try to download from other source
        const bool networkError = (!m_retranslationDone && isNetworkError(m_retranslationResult))
            || (!m_updateDone && isNetworkError(m_updateResult));
        // in case file is not absent or damaged on source, try to download from other source
        const bool badOrAbsentFile
            = (!m_retranslationDone && (badFile(m_retranslationResult) || absentFile(m_retranslationResult)))
            || (!m_updateDone && (badFile(m_updateResult) || absentFile(m_updateResult)));

        if(!networkError && !badOrAbsentFile)
            return;

        // primary index has been downloaded, check if its date changed
        if(primaryIndexDownloadedAndParsed
            // bases date changed while update, try to update from the same source
            && badOrAbsentFile && (m_basesDateAtStartUpdate != m_basesDateCurrentIteration))
        {
            // save version, thus we can check if update failed due to change of files on source while update procedure
            m_basesDateAtStartUpdate = m_basesDateCurrentIteration;
            TRACE_MESSAGE2("Try the same source second time to check if bases version changed, current version %S",
                m_basesDateCurrentIteration.toWideChar());

            // erase information about all files downloaded from current
             // source, because we expect files to be changed on source
            m_downloadedFilesInCurrentSession.clear();
            continue;
        }


        // try to update from other source
        m_sourceList.moveNext();
        // all sources were tried, but update did not succeed
        if(m_sourceList.getCurrentElement(source))
            reportSourceSelected();
        /// no more sourses
        else
            break;

        // clear update date and reset error codes due to source switch
        m_basesDateAtStartUpdate.erase();

        // reset error to success, because it can contain value from previous source update operation
        if(!m_retranslationDone)
            m_retranslationResult = CORE_NO_ERROR;
        if(!m_updateDone)
            m_updateResult = CORE_NO_ERROR;
    }

    if(!m_retranslationDone && m_retranslationResult == CORE_NO_ERROR)
        m_retranslationResult = CORE_URLS_EXHAUSTED;
    if(!m_updateDone && m_updateResult == CORE_NO_ERROR)
        m_updateResult = CORE_URLS_EXHAUSTED;
}

KLUPD::CoreError KLUPD::Updater::doUpdate()
{
    const CoreError initializeResult = initialize();

    if(initializeResult == CORE_NO_ERROR)
    {
        downloadFromAllSources();
        TRACE_MESSAGE2("Total %d bytes downloaded", m_downloadProgress.bytesTransferred());
    }
    else
    {
        m_retranslationDone = m_updateDone = true;
        return initializeResult;
    }


    // report cancellation
    if(m_retranslationResult == CORE_CANCELLED || m_updateResult == CORE_CANCELLED)
    {
        if(!m_retranslationDone)
            m_retranslationResult = CORE_CANCELLED;
        if(!m_updateDone)
            m_updateResult = CORE_CANCELLED;
    }

    m_downloadProgress.percentCallback(100);

    CoreError result = CORE_NO_ERROR;
    if(m_callback.updaterConfiguration().retranslationRequired())
    {
        // correct return code
        if((m_retranslationResult == CORE_NO_ERROR) && !m_callback.updaterConfiguration().updateRequired())
            result = CORE_RETRANSLATION_SUCCESSFUL;
        else
            result = m_retranslationResult;

        m_journal.publishRetranslationResult(m_retranslationResult);
        TRACE_MESSAGE2("******** Retranslation operation result %S ********",
            toString(m_retranslationResult).toWideChar());
    }

    if(m_callback.updaterConfiguration().updateRequired())
    {
        // correct return code
        result = m_updateResult;

        // adjust code for case update successful, but retranslation failed
        if(isSuccess(m_updateResult)
            && m_callback.updaterConfiguration().retranslationRequired()
            && !isSuccess(m_retranslationResult))
        {
            result = CORE_UpdateSuccessfulRetranslationFailed;
            m_journal.publishMessage(CORE_UpdateSuccessfulRetranslationFailed);
        }

        m_journal.publishUpdateResult(m_updateResult);
        TRACE_MESSAGE2("******** Update operation result %S ********",
            toString(m_updateResult).toWideChar());
    }

    // neither update no retranslation is requested
    if(!m_callback.updaterConfiguration().retranslationRequired() && !m_callback.updaterConfiguration().updateRequired())
        result = CORE_NO_OPERATION_REQUESTED;

    return result;
}

KLUPD::CoreError KLUPD::Updater::getRetranslationOperationResult()const
{
    return m_retranslationResult;
}
