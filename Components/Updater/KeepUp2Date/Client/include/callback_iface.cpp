#include "callback_iface.h"

#include "../helper/updaterStaff.h"


KLUPD::CallbackInterface::CallbackInterface()
{
}
KLUPD::CallbackInterface::~CallbackInterface()
{
}

void KLUPD::CallbackInterface::processRollbackFileList(const FileVector &, const bool retranslationMode, const NoCaseString& strDate)
{
    // no action by default
}

bool KLUPD::CallbackInterface::removeRollbackSection(const bool retranslationMode)
{
    // rollback is not implemented
    return false;
}

bool KLUPD::CallbackInterface::readRollbackFiles(FileVector &, const bool retranslationMode)
{
    // rollback is not implemented
    return false;
}

void KLUPD::CallbackInterface::updateAutomaticScheduleForUpdater(const UpdateSchedule &)
{
}

bool KLUPD::CallbackInterface::updateVariableFromIndex(const UpdatedSettings &, const bool retranslationMode)
{
    // by default write variable is not implemented
    return false;
}

bool KLUPD::CallbackInterface::checkIfFileOptional(const FileInfo &, const bool retranslationMode)
{
    return false;
}

bool KLUPD::CallbackInterface::checkBlackListFile(const Path &)
{
    // do not check black lists file by default
    return true;
}

bool KLUPD::CallbackInterface::checkFilesToDownload(FileVector &, const NoCaseString &, const bool)
{
    return true;
}
bool KLUPD::CallbackInterface::checkDownloadedFiles(const FileVector &, const bool retranslationMode)
{
    // by default no check of downloaded files
    return true;
}
bool KLUPD::CallbackInterface::processReceivedFiles(const FileVector &, const bool createRollbackNeeded, const Path &rollbackFolder, const bool retranslationMode, const NoCaseString& strUpdateDate)
{
    // by default no processing of received files
    return true;
}

bool KLUPD::CallbackInterface::checkPreInstalledComponent(const Path &suffix, const NoCaseString &component, const FileVector &)
{
    return true;
}
void KLUPD::CallbackInterface::lockComponentForUpdate(const NoCaseString &componentIdentifier)
{
}
void KLUPD::CallbackInterface::unlockComponentForUpdate(const NoCaseString &componentIdentifier)
{
}

bool KLUPD::CallbackInterface::expandEnvironmentString(const NoCaseString &, NoCaseString &, const StringParser::ExpandOrder &)
{
    return false;
}

void KLUPD::CallbackInterface::removeLockedFileRequest(const Path &)
{
}

void KLUPD::CallbackInterface::switchToSystemContext(const std::string &traceText)
{
}
void KLUPD::CallbackInterface::switchToUserContext(const std::string &traceText)
{
}


KLUPD::Path KLUPD::CallbackInterface::productFolder(const bool retranslationMode)
{
    if(retranslationMode)
    {
        // cache retrnaslation folder
        if(m_retranslationFolderCache.empty())
        {
            m_retranslationFolderCache = updaterConfiguration().retranslation_dir;
            // user may need to use environment variable to specify retranslation folder
            StringParser::canonizePath(StringParser::bySystemFirst, m_retranslationFolderCache, *this);
        }
        return m_retranslationFolderCache;
    }

    // update mode
     // cache udpate folder
    if(m_dataFolderCache.empty())
    {
		if(!expandEnvironmentString(SS_KEY_UpdaterDataFolder, m_dataFolderCache.m_value, StringParser::byProductFirst))
            m_dataFolderCache = L"%" SS_KEY_UpdaterDataFolder L"%";

        StringParser::canonizePath(StringParser::byProductFirst, m_dataFolderCache, *this);
    }
    return m_dataFolderCache;
}
KLUPD::Path KLUPD::CallbackInterface::productFolder(const FileInfo &fileEntry, const bool retranslationMode)
{
    Path folder = productFolder(retranslationMode);
    if(retranslationMode)
        folder += fileEntry.m_relativeURLPath.emptyIfSingleSlash();
    // local path has been calculated and path substitutions are taked into consideration
    else
        folder = fileEntry.m_localPath;

    StringParser::canonizePath(StringParser::byProductFirst, folder, *this);
    return folder;
}

KLUPD::Path KLUPD::CallbackInterface::temporaryFolderRoot()
{
    if(m_temporaryFolderRootCache.empty())
    {
        m_temporaryFolderRootCache = updaterConfiguration().temporary_dir;
        StringParser::canonizePath(StringParser::byProductFirst, m_temporaryFolderRootCache, *this);
    }
    return m_temporaryFolderRootCache;
}

KLUPD::Path KLUPD::CallbackInterface::temporaryFolder()
{
    return temporaryFolderRoot() + L"temporaryFolder/";
}
KLUPD::Path KLUPD::CallbackInterface::temporaryFolder(const FileInfo &fileEntry)
{
    Path folder = temporaryFolder() + fileEntry.m_relativeURLPath.emptyIfSingleSlash();
    StringParser::canonizePath(StringParser::byProductFirst, folder, *this);
    return folder;
}

KLUPD::Path KLUPD::CallbackInterface::rollbackFolder(const bool retranslationMode)
{
    return temporaryFolderRoot()
        + (retranslationMode ? L"retranslation/rollback/" : L"update/rollback/");
}
KLUPD::Path KLUPD::CallbackInterface::rollbackTemporaryFolder(const bool retranslationMode)
{
    return temporaryFolder()
        + (retranslationMode ? L"retranslation/rollbackTemporary/" : L"update/rollbackTemporary/");
}
KLUPD::Path KLUPD::CallbackInterface::updaterConfigurationFilesFolder()
{
    if(m_basesFolderCache.empty())
    {
        if(!expandEnvironmentString(SS_KEY_BaseFolder, m_basesFolderCache.m_value, StringParser::byProductFirst))
        {
            m_basesFolderCache = L"%" SS_KEY_BaseFolder L"%";
            StringParser::canonizePath(StringParser::byProductFirst, m_basesFolderCache, *this);
        }
        else
            StringParser::canonizePath(StringParser::byProductFirst, m_basesFolderCache, *this);
    }
    return m_basesFolderCache;
}

