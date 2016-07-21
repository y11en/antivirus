#include "update_transaction.h"

KLUPD::UpdaterTransaction::UpdaterTransaction(const Path &rollbackFolder,
                                              const bool retranslationMode,
                                              DownloadProgress &downloadProgress,
                                              CallbackInterface &callbacks,
                                              JournalInterface &journal,
                                              Log *log)
 : m_anyComponentInstalled(false),
   m_lastError(CORE_NO_ERROR),
   m_rollbackFolder(rollbackFolder),
   m_retranslationMode(retranslationMode),
   m_downloadProgress(downloadProgress),
   m_callbacks(callbacks),   
   m_journal(journal),
   pLog(log)
{
    // special protocol to update files using suffix
    if(!m_retranslationMode)
        makeFileReplaceSuffix();
}

KLUPD::CoreError KLUPD::UpdaterTransaction::run(FileVector &files)
{
    const CoreError result = runImplementation(files);

    // set status to unchanged for all files failed to install
    for(FileVector::iterator fileIter = files.begin(); fileIter != files.end(); ++fileIter)
    {
        if(std::find_first_of(fileIter->m_componentIdSet.begin(), fileIter->m_componentIdSet.end(),
            m_failComponents.begin(), m_failComponents.end()) != fileIter->m_componentIdSet.end())
        {
            fileIter->m_transactionInformation.m_changeStatus = FileInfo::unchanged;
        }
    }

    return result;
}

KLUPD::CoreError KLUPD::UpdaterTransaction::runImplementation(const FileVector &files)
{
    // cleaning rollback folder, because new files are going to be places there and previous files are no more needed
    if(LocalFile::clearFolder(m_rollbackFolder, 0))
    {
        TRACE_MESSAGE2("Updater rollback folder removed with all its content '%S'",
            m_rollbackFolder.toWideChar());
    }
    else
    {
        TRACE_MESSAGE2("Failed to remove updater rollback folder with all its content '%S'",
            m_rollbackFolder.toWideChar());
    }


    FileVector::FilesByComponent filesByComponents;
    files.splitByComponents(filesByComponents, true);

    for(FileVector::FilesByComponent::const_iterator filesByComponentIter = filesByComponents.begin(); filesByComponentIter != filesByComponents.end(); ++filesByComponentIter)
    {
        TRACE_MESSAGE2("**** Installing component '%S' ****", filesByComponentIter->first.toWideChar());

        lockComponent(filesByComponentIter->first);
        size_t failIndex = 0;
        const bool componentInstalled = processInstallComponent(filesByComponentIter->first, filesByComponentIter->second, failIndex);
        unlockComponent(filesByComponentIter->first);

        if(componentInstalled)
        {
            TRACE_MESSAGE2("**** Component '%S' installed successfully ****", filesByComponentIter->first.toWideChar());
            m_anyComponentInstalled = true;
        }
        else
        {
            bool componentOptional = true;
            for(FileVector::const_iterator fileIter = filesByComponentIter->second.begin(); fileIter != filesByComponentIter->second.end(); ++fileIter)
            {
                if(!m_callbacks.checkIfFileOptional(*fileIter, m_retranslationMode))
                {
                    componentOptional = false;
                    break;
                }
            }

            TRACE_MESSAGE3("**** Unable to install %S component '%S' ****",
                componentOptional ? L"optional" : L"mandatory", filesByComponentIter->first.toWideChar());

            if(componentOptional)
            {
                lockComponent(filesByComponentIter->first);
                rollbackComponent(filesByComponentIter->first, filesByComponentIter->second, failIndex);
                unlockComponent(filesByComponentIter->first);
            }
            else
            {
                m_anyComponentInstalled = false;

                // rollback all installed components
                while(true)
                {
                    if(!filesByComponentIter->second.empty())
                    {
                        lockComponent(filesByComponentIter->first);
                        rollbackComponent(filesByComponentIter->first, filesByComponentIter->second, failIndex);
                        unlockComponent(filesByComponentIter->first);
                    }
                    if(filesByComponentIter == filesByComponents.begin())
                        break;
                    --filesByComponentIter;
                    failIndex = filesByComponentIter->second.size() - 1;
                }
                break;
            }
        }
    }

    cleanTransactionHelperFiles(files);

    if(m_anyComponentInstalled && !isSuccess(m_lastError))
        return CORE_NotAllComponentsAreUpdated;

    return m_lastError;
}

bool KLUPD::UpdaterTransaction::processInstallComponent(const NoCaseString &componentName, const FileVector &files, size_t &failIndex)
{
    //////////////////////////////////////////////////////
    ///// creating rollback
    const std::string createRollbackTraceText = std::string("Creating rollback for component '") + componentName.toAscii() + "'";
    TRACE_MESSAGE2("** %s ** ", createRollbackTraceText.c_str());

    switchToSystemContext(createRollbackTraceText);
    const CoreError createRollbackResult = createRollbackFileCopiesForComponent(files);
    switchToUserContext(createRollbackTraceText);

    if(!isSuccess(createRollbackResult))
    {
        TRACE_MESSAGE3("** Failed to create rollback of component '%S', result %S **",
            componentName.toWideChar(), toString(createRollbackResult).toWideChar());
        saveTransactionLastError(createRollbackResult);
        return false;
    }
    TRACE_MESSAGE2("** Rollback is created for component '%S' **", componentName.toWideChar());


    //////////////////////////////////////////////////////
    ///// pre-install component
    const std::string preInstallComponentTraceText = std::string("Pre-install component '") + componentName.toAscii() + "'";
    TRACE_MESSAGE2("** %s **", preInstallComponentTraceText.c_str());

    switchToSystemContext(preInstallComponentTraceText);
    const CoreError preInstallResult = preInstallComponent(files, failIndex);
    switchToUserContext(preInstallComponentTraceText);

    if(!isSuccess(preInstallResult))
    {
        TRACE_MESSAGE3("** Pre-install component '%S' failed, result %S **",
            componentName.toWideChar(), toString(preInstallResult).toWideChar());
        saveTransactionLastError(preInstallResult);
        return false;
    }
    TRACE_MESSAGE2("** Component '%S' pre-installed **", componentName.toWideChar());

    // no check and no post-install step in retranslation mode
    if(m_retranslationMode)
        return true;

    //////////////////////////////////////////////////////
    ///// check pre-installed component
    if(!m_callbacks.checkPreInstalledComponent(m_fileReplaceSuffix, componentName, files))
    {
        TRACE_MESSAGE2("** Product rejected pre-installed component '%S' **", componentName.toWideChar());
        saveTransactionLastError(CORE_ComponentRejectedByProduct);
        m_journal.publishMessage(CORE_ComponentRejectedByProduct, componentName);
        return false;
    }

    //////////////////////////////////////////////////////
    ///// post-install component
    const std::string postInstallComponentTraceText = std::string("Post-install component '") + componentName.toAscii() + "'";
    TRACE_MESSAGE2("** %s' **", postInstallComponentTraceText.c_str());

    switchToSystemContext(postInstallComponentTraceText);
    const CoreError postInstallResult = postInstallComponent(files);
    switchToUserContext(postInstallComponentTraceText);

    if(!isSuccess(postInstallResult))
    {
        TRACE_MESSAGE3("** Post-install component '%S' failed, result %S **",
            componentName.toWideChar(), toString(postInstallResult).toWideChar());
        saveTransactionLastError(postInstallResult);
        return false;
    }

    TRACE_MESSAGE2("** Component '%S' post-installed successfully **", componentName.toWideChar());
    return true;
}

KLUPD::CoreError KLUPD::UpdaterTransaction::createRollbackFileCopiesForComponent(const FileVector &files)
{
    for(size_t fileIndex = 0; fileIndex < files.size(); ++fileIndex)
    {
        const FileInfo &file = files[fileIndex];
        // rollback copy is only needed for modified files
        if(file.m_transactionInformation.m_changeStatus != FileInfo::modified)
            continue;

        const Path rollbackFolder = m_rollbackFolder + file.m_relativeURLPath;
        if(!createFolder(rollbackFolder, pLog))
        {
            m_journal.publishMessage(CORE_FailedToCreateFolder, rollbackFolder.m_value);
            TRACE_MESSAGE2("Failed to create rollback folder '%S'", rollbackFolder.toWideChar());
            return CORE_FailedToCreateFolder;
        }

        LocalFile originalFile(file.m_transactionInformation.m_currentLocation + file.m_filename, pLog);
        LocalFile rollbackFile(m_rollbackFolder + file.m_relativeURLPath + file.m_filename, pLog);

        if( !rollbackFile.exists() )
        {
            const CoreError createRollbackResult = originalFile.copy(rollbackFile.m_name);
            if(!isSuccess(createRollbackResult))
            {
                TRACE_MESSAGE3("Failed to create rollback copy for '%S' to '%S'",
                    originalFile.m_name.toWideChar(), rollbackFile.m_name.toWideChar());
                m_journal.publishMessage(createRollbackResult, originalFile.m_name.m_value);
                return createRollbackResult;
            }
        }
    }
    return CORE_NO_ERROR;
}


KLUPD::CoreError KLUPD::UpdaterTransaction::preInstallComponent(const FileVector &files, size_t &failIndex)
{
    for(failIndex = 0; failIndex < files.size(); ++failIndex)
    {
        const FileInfo &file = files[failIndex];

        // draw progress
        if(file.m_transactionInformation.m_changeStatus == FileInfo::modified)
        {
            // to reduce CPU load
            usleep(2000);
            m_downloadProgress.tickPercent();
        }

        if(file.m_transactionInformation.m_changeStatus == FileInfo::modified)
        {
            const CoreError preReplaceResult = preReplaceAlgorithmSelector(file);
            if(!isSuccess(preReplaceResult))
                return preReplaceResult;
        }
        else if(file.m_transactionInformation.m_changeStatus == FileInfo::added)
        {
            // new files may need to be added to non-existent folder
            if(!createFolder(file.m_transactionInformation.m_currentLocation, pLog))
            {
                TRACE_MESSAGE2("Failed to create folder for add transaction file operation '%S'",
                    file.m_transactionInformation.m_currentLocation.toWideChar());
                m_journal.publishMessage(CORE_FailedToCreateFolder, file.m_transactionInformation.m_currentLocation.m_value);
                return CORE_FailedToCreateFolder;
            }

            const CoreError preAddFileResult = preAddFile(file);
            if(!isSuccess(preAddFileResult))
                return preAddFileResult;
        }
    }
    return CORE_NO_ERROR;
}

KLUPD::CoreError KLUPD::UpdaterTransaction::preReplaceAlgorithmSelector(const FileInfo &file)
{
    if(file.m_transactionInformation.m_replaceMode == FileInfo::TransactionInformation::suffix)
        return preReplaceFileWithSuffix(file);
    if(preReplaceFileOrdinary(file))
        return CORE_NO_ERROR;
    return preReplaceOrdinaryLockedFile(file);
}

KLUPD::CoreError KLUPD::UpdaterTransaction::preReplaceFileWithSuffix(const FileInfo &file)
{
    LocalFile originalFile(file.m_transactionInformation.m_currentLocation + file.m_filename, pLog);
    const Path locationWithSuffix = file.m_transactionInformation.m_currentLocation + getFilePreInstallLocation(file);

    // copy file to new location with suffix
    LocalFile newFile(file.m_transactionInformation.m_newLocation + file.m_filename, pLog);
    const CoreError copyResult = newFile.copy(locationWithSuffix);
    if(isSuccess(copyResult))
        return CORE_NO_ERROR;

    TRACE_MESSAGE3("Failed to pre-replace file using suffix '%S' to '%S'",
        newFile.m_name.toWideChar(), locationWithSuffix.toWideChar());
    m_journal.publishMessage(copyResult, locationWithSuffix.m_value);
    return copyResult;
}

bool KLUPD::UpdaterTransaction::preReplaceFileOrdinary(const FileInfo &file)
{
    LocalFile originalFile(file.m_transactionInformation.m_currentLocation + file.m_filename, pLog);
    if(!isSuccess(originalFile.unlink()) || originalFile.exists())
        return false;

    // move file (rename()) is not used, because single file in temporary
     // folder may stands for several files in product folders
    const CoreError replaceFileResult = LocalFile(file.m_transactionInformation.m_newLocation + file.m_filename, pLog).copy(file.m_transactionInformation.m_currentLocation);
    if(isSuccess(replaceFileResult))
    {
        m_journal.fileUpdated(file, JournalInterface::odinary, m_retranslationMode);
        m_journal.publishMessage(EVENT_FILE_UPDATED, originalFile.m_name.m_value);
        return true;
    }
    TRACE_MESSAGE2("Failed to copy a new version of file '%S'", originalFile.m_name.toWideChar());
    return false;
}

KLUPD::CoreError KLUPD::UpdaterTransaction::preReplaceOrdinaryLockedFile(const FileInfo &file)
{
    const Path productFile = file.m_transactionInformation.m_currentLocation + file.m_filename;
    const Path sourceFile = file.m_transactionInformation.m_newLocation + file.m_filename;


    TRACE_MESSAGE2("Trying 'replace-locked-file' mechanism for '%S'", productFile.toWideChar());

    // rename locking file, because executable file lock allows it (lock does not allow to delete or replace file)
    const Path movedLockedFile = productFile + L"." + m_fileReplaceSuffix + L"_2553_4126.removeOnNextReboot";
    // rename may fail on 9.x platform
    LocalFile productFileToMove(productFile, pLog);
    const CoreError lockedFileMoveResult = productFileToMove.rename(movedLockedFile);

    // ask application to remove trash file
    if(LocalFile(movedLockedFile, pLog).exists())
        m_callbacks.removeLockedFileRequest(movedLockedFile);

    // copy new file on location from which locked file was removed
    if(!isSuccess(lockedFileMoveResult))
    {
        TRACE_MESSAGE2("'Replace-locked-file' mechanism failed, unable to move file '%S'", productFile.toWideChar());
        m_journal.publishMessage(lockedFileMoveResult, productFileToMove.m_name.m_value);
        return lockedFileMoveResult;
    }

    const CoreError copyOnMovedLocation = LocalFile(sourceFile).copy(productFile);
    if(!isSuccess(copyOnMovedLocation))
    {
        TRACE_MESSAGE3("Failed to copy new file to the location where locked file was '%S', result '%S'",
            productFile.toWideChar(), toString(copyOnMovedLocation).toWideChar());
        m_journal.publishMessage(copyOnMovedLocation, productFileToMove.m_name.m_value);
        return copyOnMovedLocation;
    }
    TRACE_MESSAGE2("File replaced with 'replace-locked-file' mechanism '%S'", productFile.toWideChar());
    m_journal.fileUpdated(file, JournalInterface::lockedFileReplaceMechanismUsed, m_retranslationMode);
    m_journal.publishMessage(EVENT_FILE_UPDATED, productFile.m_value);
    return CORE_NO_ERROR;
}

KLUPD::CoreError KLUPD::UpdaterTransaction::preAddFile(const FileInfo &file)
{
    // add file with suffix
    if(!m_retranslationMode
        && file.m_transactionInformation.m_replaceMode == FileInfo::TransactionInformation::suffix)
    {
        const Path locationWithSuffix = file.m_transactionInformation.m_currentLocation + getFilePreInstallLocation(file);
        const CoreError addFileWithSuffixResult = LocalFile(file.m_transactionInformation.m_newLocation + file.m_filename, pLog).copy(locationWithSuffix);
        if(!isSuccess(addFileWithSuffixResult))
        {
            TRACE_MESSAGE3("Failed to copy a new version of transaction file with suffix '%S', result '%S'",
                locationWithSuffix.toWideChar(), toString(addFileWithSuffixResult).toWideChar());
            m_journal.publishMessage(addFileWithSuffixResult, locationWithSuffix.m_value);
            return addFileWithSuffixResult;
        }
        return CORE_NO_ERROR;
    }

    // add file without suffix
    const Path productLocation = file.m_transactionInformation.m_currentLocation + file.m_filename;
    const CoreError addFileResult = LocalFile(file.m_transactionInformation.m_newLocation + file.m_filename, pLog).copy(productLocation);
    if(!isSuccess(addFileResult))
    {
        TRACE_MESSAGE3("Failed to copy a new version of transaction file '%S', result '%S'",
            productLocation.toWideChar(), toString(addFileResult).toWideChar());
        m_journal.publishMessage(addFileResult, productLocation.m_value);
        return addFileResult;
    }
    m_journal.fileAdded(file, m_retranslationMode);
    m_journal.publishMessage(EVENT_NEW_FILE_INSTALLED, productLocation.m_value);
    return CORE_NO_ERROR;
}

KLUPD::CoreError KLUPD::UpdaterTransaction::postInstallComponent(const FileVector &files)
{
    // in retranslation modes files are already installed on pre-install step
    if(m_retranslationMode)
        return KLUPD::CORE_NO_ERROR;

    for(size_t fileIndex = 0; fileIndex < files.size(); ++fileIndex)
    {
        const FileInfo &file = files[fileIndex];
        // skip files without suffix
        if(file.m_transactionInformation.m_replaceMode != FileInfo::TransactionInformation::suffix)
            continue;
        // skip unchanged files
        if(file.m_transactionInformation.m_changeStatus == FileInfo::unchanged)
            continue;


        const Path productLocation = file.m_transactionInformation.m_currentLocation + file.m_filename;
        const Path locationWithSuffix = file.m_transactionInformation.m_currentLocation + getFilePreInstallLocation(file);

        // move files with suffix, because they may already be locked
        const CoreError renameSuffixFileResult = LocalFile(locationWithSuffix, pLog).rename(productLocation);
        if(!isSuccess(renameSuffixFileResult))
        {
            TRACE_MESSAGE3("Failed to unlink file before copy file with suffix in product folder '%S', result '%S'",
                productLocation.toWideChar(), toString(renameSuffixFileResult).toWideChar());
            m_journal.publishMessage(renameSuffixFileResult, productLocation.m_value);
            return renameSuffixFileResult;
        }

        m_journal.fileUpdated(file, JournalInterface::odinary, m_retranslationMode);
        m_journal.publishMessage(EVENT_FILE_UPDATED, productLocation.m_value);
    }
    return CORE_NO_ERROR;
}

void KLUPD::UpdaterTransaction::rollbackComponent(const NoCaseString &componentIdentifier, const FileVector &files, const size_t failIndex)
{
    TRACE_MESSAGE2("**** Reverting component '%S' to initial state ****", componentIdentifier.toWideChar());

    // check if component is already reverted
    if(std::find(m_failComponents.begin(), m_failComponents.end(), componentIdentifier) != m_failComponents.end())
    {
        TRACE_MESSAGE2("**** Component '%S' has already been reverted ****", componentIdentifier.toWideChar());
        return;
    }

	m_journal.publishMessage
		(
			m_retranslationMode ? EVENT_ComponentIsNotRetranslated : EVENT_ComponentIsNotUpdated, 
			componentIdentifier
		);
    m_failComponents.push_back(componentIdentifier);

                                    // '<=' comparison to include fail file
    for(size_t fileIndex = 0; (fileIndex <= failIndex) && (fileIndex < files.size()); ++fileIndex)
    {
        const FileInfo &file = files[fileIndex];

        // all files but modified may stay at current location
        if(file.m_transactionInformation.m_changeStatus != FileInfo::modified)
            continue;


        LocalFile originalFile(file.m_transactionInformation.m_currentLocation + file.m_filename, pLog);
        LocalFile rollbackFile(m_rollbackFolder + file.m_relativeURLPath + file.m_filename, pLog);

        CoreError revertReplacedFileResult = originalFile.unlink();
        if(isSuccess(revertReplacedFileResult))
        {
            revertReplacedFileResult = rollbackFile.copy(originalFile.m_name);
            if(isSuccess(revertReplacedFileResult))
            {
                m_journal.fileUpdated(file, JournalInterface::odinary, m_retranslationMode);
                m_journal.publishMessage(EVENT_FILE_ROLLED_BACK, originalFile.m_name.m_value);
            }
        }
        else
        {
            // error is not fatal. Anyway files are into inconsistent state
            TRACE_MESSAGE2("Failed to revert replaced file to initial state '%S'", originalFile.m_name.toWideChar());
            m_journal.publishMessage(revertReplacedFileResult, originalFile.m_name.m_value);
        }
    }
    m_callbacks.checkPreInstalledComponent(m_fileReplaceSuffix, componentIdentifier, files);

    TRACE_MESSAGE2("**** Component '%S' reverted ****", componentIdentifier.toWideChar());
}

void KLUPD::UpdaterTransaction::makeFileReplaceSuffix()
{
    // mirror is created without suffix
    if(m_retranslationMode)
        return;

    // make suffix from current time
    std::ostringstream stream;
    stream.imbue(std::locale::classic());
    stream << time(0);
    m_fileReplaceSuffix.fromAscii(stream.str());

    // inform product about suffix name
    TRACE_MESSAGE2("Update suffix to replace files '%S'", m_fileReplaceSuffix.toWideChar());
}

KLUPD::Path KLUPD::UpdaterTransaction::getFilePreInstallLocation(const FileInfo &file)const
{
    if(file.m_transactionInformation.m_replaceMode != FileInfo::TransactionInformation::suffix)
        return file.m_filename;

    Path fileNameWithoutExtension;
    Path extension;
    file.m_filename.parseFileName(fileNameWithoutExtension, extension);
    return fileNameWithoutExtension + m_fileReplaceSuffix + L"." + extension;
}


void KLUPD::UpdaterTransaction::cleanTransactionHelperFiles(const FileVector &files)
{
    // retranslation more there is no helper suffix files to remove, because files
    //  are directly go into retranslation folder and product does not lock files
    if(m_retranslationMode)
        return;

    TRACE_MESSAGE("** Removing helper transaction files **");
    for(size_t atomIndex = 0; atomIndex < files.size(); ++atomIndex)
    {
        // remove pre-installed file with suffix
        if(files[atomIndex].m_transactionInformation.m_replaceMode == FileInfo::TransactionInformation::suffix)
        {
            const Path location = files[atomIndex].m_transactionInformation.m_currentLocation
                + getFilePreInstallLocation(files[atomIndex]);

            // just unlink file in case it rest
             // ussually it should not, because it should be renamed on post-build step
            LocalFile(location, pLog).unlink();
        }
    }
}

void KLUPD::UpdaterTransaction::saveTransactionLastError(const CoreError &lastError)
{
    if(!isSuccess(lastError))
        m_lastError = lastError;
}

void KLUPD::UpdaterTransaction::lockComponent(const NoCaseString &componentIdentifier)
{
    // lock only in update mode
    if(m_retranslationMode)
        return;
    m_callbacks.lockComponentForUpdate(componentIdentifier);
}
void KLUPD::UpdaterTransaction::unlockComponent(const NoCaseString &componentIdentifier)
{
    // lock only in update mode
    if(m_retranslationMode)
        return;
    m_callbacks.unlockComponentForUpdate(componentIdentifier);
}

void KLUPD::UpdaterTransaction::switchToSystemContext(const std::string &traceText)
{
    if(!m_retranslationMode)
        m_callbacks.switchToSystemContext(traceText);
}

void KLUPD::UpdaterTransaction::switchToUserContext(const std::string &traceText)
{
    if(!m_retranslationMode)
        m_callbacks.switchToUserContext(traceText);
}
