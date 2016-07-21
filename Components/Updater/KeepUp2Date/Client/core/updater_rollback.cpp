#include "updater.h"

#include "../helper/stdinc.h"
#include "../helper/update_transaction.h"
#include "../helper/indexFileXMLParser.h"

KLUPD::CoreError KLUPD::Updater::checkFilesInRollbackFolder(const FileVector &fileList,
                                                            FileVector &filesToAffect,
                                                            const bool retranslationMode)
{
    // enumeration files in rollback folder
    std::vector<Path> fileListReal;
    if(!enumerateFilesInFolder(m_callback.rollbackFolder(retranslationMode), fileListReal, pLog, true))
    {
        TRACE_MESSAGE("Failed to enumerate files in rollback folder");
        m_journal.publishMessage(CORE_GenericFileOperationFailure, m_callback.rollbackFolder(retranslationMode).m_value);
        return CORE_GenericFileOperationFailure;
    }

    filesToAffect.clear();

    if(fileList.empty())
    {
        TRACE_MESSAGE("There is no files in rollback folder");
        m_journal.publishMessage(CORE_NO_RESERVED_UPDATE_DESCRIPTION);
        return CORE_NO_RESERVED_UPDATE_DESCRIPTION;
    }

    for(FileVector::const_iterator fileIter = fileList.begin(); fileIter != fileList.end(); ++fileIter)
    {
        const Path fullFileName = m_callback.rollbackFolder(retranslationMode) + fileIter->m_relativeURLPath + fileIter->m_filename;
        // added files, were no removed
        if(fileIter->m_transactionInformation.m_changeStatus == FileInfo::added)
            continue;

        // file not found on disk
        if(std::find(fileListReal.begin(), fileListReal.end(), fullFileName) == fileListReal.end())
        {
            TRACE_MESSAGE2("File in rollback folder is absent '%S'", fullFileName.toWideChar());
            m_journal.publishMessage(CORE_NO_RESERVED_UPDATE_DESCRIPTION, fullFileName.m_value);
            return CORE_NO_RESERVED_UPDATE_DESCRIPTION;
        }

        if(fileIter->m_md5.empty())
        {
            TRACE_MESSAGE2("Internal error: for rollback expects files consistent by MD5, but md5 is absent for file '%S'",
                fullFileName.toWideChar());
            return CORE_INTERNAL_ERROR;
        }

        std::vector<unsigned char> md5;
        if(!calcMD5Hash(fullFileName, md5)
            || (md5 != fileIter->m_md5))
        {
            std::vector<char> traceEtalonMD5 = CBase64::encodeBuffer(&(fileIter->m_md5)[0], fileIter->m_md5.size());
            traceEtalonMD5.push_back(0);

            std::vector<char> traceActualMD5 = CBase64::encodeBuffer(&md5[0], md5.size());
            traceActualMD5.push_back(0);

            TRACE_MESSAGE4("File is damaged in rollback folder '%S', expected md5 is '%s', actual md5 is '%s'",
                fullFileName.toWideChar(), &traceEtalonMD5[0], &traceActualMD5[0]);

            m_journal.publishMessage(CORE_INVALID_SIGNATURE, fullFileName.m_value);
            return CORE_INVALID_SIGNATURE;
        }


        FileInfo atom = *fileIter;
        atom.m_transactionInformation.m_changeStatus = FileInfo::modified;
        atom.m_transactionInformation.m_currentLocation = fileIter->m_localPath;
        atom.m_transactionInformation.m_newLocation = m_callback.rollbackFolder(retranslationMode) + fileIter->m_relativeURLPath;
        bool fileDuplicate = false;
        filesToAffect.insertNewOrUpdateTheSame(*fileIter, atom, fileDuplicate, retranslationMode);
    }

    return CORE_NO_ERROR;
}

KLUPD::CoreError KLUPD::Updater::doRollbackImplementation(const bool retranslationMode)
{
    m_downloadProgress.percentCallback(20);
    if(m_downloadProgress.checkCancel())
        return CORE_CANCELLED;

    const CoreError initializeResult = initialize();
    if(!isSuccess(initializeResult))
    {
        TRACE_MESSAGE2("Failed to obtain updater configuration, result %S",
            toString(initializeResult).toWideChar());
        return initializeResult;
    }

    m_downloadProgress.percentCallback(30);
    if(m_downloadProgress.checkCancel())
        return CORE_CANCELLED;

    FileVector fileList;
    m_callback.readRollbackFiles(fileList, retranslationMode);
    if(fileList.empty())
    {
        TRACE_MESSAGE("No reserved files found, nothing to rollback");
        m_journal.publishMessage(CORE_NO_RESERVED_UPDATE_DESCRIPTION);
        return CORE_NO_RESERVED_UPDATE_DESCRIPTION;
    }

    m_downloadProgress.percentCallback(40);
    if(m_downloadProgress.checkCancel())
        return CORE_CANCELLED;

    // check existence and sign
    Signature6Checker signature6Checker(pLog);
    UpdateInfo rollback(m_callback.updaterConfiguration().filterFlags(retranslationMode),
        m_callback.updaterConfiguration().filterComponents(retranslationMode),
        signature6Checker, retranslationMode,
        m_callback, pLog);

    const CoreError checkRollbackFolderResult = checkFilesInRollbackFolder(fileList, rollback.m_matchFileList, retranslationMode);
    if(!isSuccess(checkRollbackFolderResult))
        return checkRollbackFolderResult;

    m_downloadProgress.percentCallback(50);
    if(m_downloadProgress.checkCancel())
        return CORE_CANCELLED;

    m_journal.publishMessage(EVENT_StartCopyFilesForRollback);
    UpdaterTransaction rollbackTransaction(m_callback.rollbackTemporaryFolder(retranslationMode),
        retranslationMode, m_downloadProgress, m_callback, m_journal, pLog);
    const CoreError transactionResult = rollbackTransaction.run(rollback.m_matchFileList);

    m_downloadProgress.percentCallback(60);
    // cancel is not possible to keep consistency with product

    if( isSuccess(transactionResult) )
    {
        // check consistency of local database
        FileVector vecFilesLocal;
        bool bConsistent = EnumerateLocalFiles(vecFilesLocal, retranslationMode);

        NoCaseString strDate;
        strDate = GetUpdateDate(vecFilesLocal);

        if(bConsistent)
            // send rolled back file list to product
            m_callback.processRollbackFileList(rollback.m_matchFileList, retranslationMode, strDate);
        else
            m_callback.OnDatabaseStateChanged(vecFilesLocal, bConsistent, strDate);
    }

    m_downloadProgress.percentCallback(70);
    // cancel is not possible to keep consistency with product

    // rollback is not available anymore
    m_callback.removeRollbackSection(retranslationMode); 

    m_downloadProgress.percentCallback(80);
    if(m_downloadProgress.checkCancel())
    {
        // actually rollback is already done, that is why return cancel makes no more sense
        return CORE_NO_ERROR;
    }

    if(LocalFile::clearFolder(m_callback.rollbackFolder(retranslationMode), 0))
    {
        TRACE_MESSAGE2("Updater rollback folder removed with all its content '%S'",
            m_callback.rollbackFolder(retranslationMode).toWideChar());
    }
    else
    {
        TRACE_MESSAGE2("Failed to remove updater rollback folder with all its content '%S'",
            m_callback.rollbackFolder(retranslationMode).toWideChar());
    }

    // rollback transaction failed
    if(!isSuccess(transactionResult))
        return transactionResult;

    m_downloadProgress.percentCallback(90);
    if(m_downloadProgress.checkCancel())
    {
        // actually rollback is already done, that is why return cancel makes no more sense
        return CORE_NO_ERROR;
    }

    if(LocalFile::clearFolder(m_callback.rollbackTemporaryFolder(retranslationMode), 0))
    {
        TRACE_MESSAGE2("Updater temporary rollback folder removed with all its content '%S'",
            m_callback.rollbackTemporaryFolder(retranslationMode).toWideChar());
    }
    else
    {
        TRACE_MESSAGE2("Failed to remove updater temporary rollback folder with all its content '%S'",
            m_callback.rollbackTemporaryFolder(retranslationMode).toWideChar());
    }


    TRACE_MESSAGE("Rollback performed succesfully");
    return CORE_NO_ERROR;
}

KLUPD::CoreError KLUPD::Updater::doRollback(const bool retranslationMode)
{
    TRACE_MESSAGE("Rollback started");
    m_journal.publishMessage(EVENT_TaskStarted);

    const CoreError rollbackResult = doRollbackImplementation(retranslationMode);

    m_downloadProgress.percentCallback(100);
    m_journal.publishRollbackResult(rollbackResult);
    TRACE_MESSAGE2("Rollback finished, result %S",
        toString(rollbackResult).toWideChar());

    return rollbackResult;
}

