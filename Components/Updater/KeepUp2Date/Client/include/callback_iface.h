#ifndef CALLBACK_IFACE_H_INCLUDED_95B38DE1_9EB2_445c_A018_2CC23FDD0CB8
#define CALLBACK_IFACE_H_INCLUDED_95B38DE1_9EB2_445c_A018_2CC23FDD0CB8

#if _MSC_VER > 1000
    #pragma once
#endif // _MSC_VER > 1000

#include "../helper/stdinc.h"
#include "../helper/fileInfo.h"
#include "stringParser.h"

namespace KLUPD {

class JournalInterface;
struct UpdateSchedule;

class KAVUPDCORE_API CallbackInterface
{
public:
    CallbackInterface();
    virtual ~CallbackInterface();

    // request for updater configuration
    virtual UpdaterConfiguration &updaterConfiguration() = 0;
    
    // the processRollbackFileList() callback is called on rollback success completion
    virtual void processRollbackFileList(const FileVector &, const bool retranslationMode, const NoCaseString& strDate);

    /// removed rollback information in case product 
    virtual bool removeRollbackSection(const bool retranslationMode) = 0;
    virtual bool readRollbackFiles(FileVector &, const bool retranslationMode) = 0;

    // pass variable read from index file to product
	virtual bool updateVariableFromIndex(const UpdatedSettings &, const bool retranslationMode);
    // pass automatic schedule for updater information
    //  Automatic Schedule for Updater may be implemented in product
    virtual void updateAutomaticScheduleForUpdater(const UpdateSchedule &);


    // callback for primary index file name and path
    // parameter retranslationMode [in] - different update root files may be used for update and retranslation modes
    virtual FileInfo getPrimaryIndex(const bool retranslationMode) = 0;
    // callback for updater configuration XML file location
    virtual FileInfo getUpdaterConfigurationXml() = 0;
    // path to file with additional source list (sites.xml)
    virtual FileInfo getSitesFileXml() = 0;


    // file is absent or damaged. Ask if product consider file
     // to be optional, and absence of file is not critical
    virtual bool checkIfFileOptional(const FileInfo &absentFile, const bool retranslationMode);

    // gives an ability to application to check downloaded black list file
    // override the function if you need to check black list file.
    // default implementation does not check black list
    // return true in case check passed, otherwise false. Update will be terminated with error
    virtual bool checkBlackListFile(const Path &fileName);

    // process file list RIGHT AFTER it is prepared (AFTER index is downloaded, but BEFORE bases are downloaded)
    // in case return is false then current operation is terminated
    virtual bool checkFilesToDownload(FileVector &, const NoCaseString &updateDate, const bool retranslationMode);
    // process file list AFTER all files are DOWNLOADED, but BEFORE COPY to product folder operation
    // in case return is false then current operation is terminated
    virtual bool checkDownloadedFiles(const FileVector &, const bool retranslationMode);
    // process file list AFTER INSTALL. File list may be used to prepare rollback list, or in other ways.
    // result is ignored (TODO: make void)
    virtual bool processReceivedFiles(const FileVector &, const bool createRollbackNeeded, const Path &rollbackFolder, const bool retranslationMode, const NoCaseString&) = 0;
    // is called on database state changing event
    virtual void OnDatabaseStateChanged(const FileVector &vecFiles, bool bConsistent, const NoCaseString& strDate) {}

    // ask product to check pre-installed files
     // in case check failed and return false, then component is rolled back
    virtual bool checkPreInstalledComponent(const Path &suffix, const NoCaseString &componentIdentifier, const FileVector &);
    virtual void lockComponentForUpdate(const NoCaseString &componentIdentifier);
    virtual void unlockComponentForUpdate(const NoCaseString &componentIdentifier);

    // gives ability to expand string to product
	virtual bool expandEnvironmentString(const NoCaseString &input, NoCaseString &output, const StringParser::ExpandOrder &) = 0;


    // request to remove locked file. Product may not want to keep trash files
     // created by updater, then product may remove locked file (e.g. on reboot).
     // To update locked files 'replace-locked-file' mechanism is used:
     //  - first file tries to be renamed to new unique file on the same volume
     //  - applicaiton is informed via removeLockedFileRequest() about the new trash unique file
     //  - then file new file is placed on locked location
    virtual void removeLockedFileRequest(const Path &fileName);

    // perform file transaction in system context, because user has
     // no permissions to modify files in program folder on Vista
    virtual void switchToSystemContext(const std::string &traceText);
    virtual void switchToUserContext(const std::string &traceText);


    //////////////////////////////////////////////////////////////////////////
    ///  Operations to get updater configuration paths ///////////////////////
    // return product folder
    Path productFolder(const bool retranslationMode);
    Path productFolder(const FileInfo &, const bool retranslationMode);
    // return temporary folder
    Path temporaryFolder();
    Path temporaryFolder(const FileInfo &);
    // return rollback folder
    virtual Path rollbackFolder(const bool retranslationMode);
    Path rollbackTemporaryFolder(const bool retranslationMode);
    // path to store updcfg.xml and sites.xml updater configuration files
    Path updaterConfigurationFilesFolder();

private:
    // root folder, whithin which temporary and rollback folders are located
    Path temporaryFolderRoot();

    // data and bases folder cache
    Path m_dataFolderCache;
    Path m_basesFolderCache;
    Path m_retranslationFolderCache;
    Path m_temporaryFolderRootCache;
};

}   // namespace KLUPD

#endif  // CALLBACK_IFACE_H_INCLUDED_95B38DE1_9EB2_445c_A018_2CC23FDD0CB8
