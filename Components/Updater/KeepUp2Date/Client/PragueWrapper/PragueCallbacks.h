#ifndef PRAGUECALLBACKS_H__24752399_0E52_40F8_90D7_F3B6121FB515__INCLUDED_
#define PRAGUECALLBACKS_H__24752399_0E52_40F8_90D7_F3B6121FB515__INCLUDED_

#if _MSC_VER > 1000
    #pragma once
#endif // _MSC_VER > 1000

#include "comdefs.h"
#include "../../IniFile/IniBasedRollback.h"
#include "UpdaterTask/task.h"

#include <Updater/updater.h>
#include "UpdaterConfigurator/i_updaterconfigurator.h"

class PragueCallbacks : public KLUPD::CallbackInterface
{
public:
    PragueCallbacks(CUpdater2005 &, PRAGUE_HELPERS::PragueLog &);
    ~PragueCallbacks();

    void informProductAboutRollbackAvailability(const bool rollbackAvailable);

    // configures update operation
    bool loadUpdaterConfiguration();

	virtual KLUPD::UpdaterConfiguration &updaterConfiguration();

    virtual void processRollbackFileList(const KLUPD::FileVector &, const bool retranslationMode, const KLUPD::NoCaseString& strDate);
    virtual bool removeRollbackSection(const bool retranslationMode);
    virtual bool readRollbackFiles(KLUPD::FileVector &, const bool retranslationMode);

    virtual void updateAutomaticScheduleForUpdater(const KLUPD::UpdateSchedule &);

    virtual KLUPD::FileInfo getPrimaryIndex(const bool retranslationMode);
    virtual KLUPD::FileInfo getUpdaterConfigurationXml();
    virtual KLUPD::FileInfo getSitesFileXml();


    virtual bool checkIfFileOptional(const KLUPD::FileInfo &absentFile, const bool retranslationMode);

    virtual bool checkFilesToDownload(KLUPD::FileVector &, const KLUPD::NoCaseString &updateDate, const bool retranslationMode);
    virtual bool processReceivedFiles(const KLUPD::FileVector &, const bool createRollbackNeeded,
        const KLUPD::Path &rollbackFolder, const bool retranslationMode, const KLUPD::NoCaseString& strUpdateDate);
    virtual void OnDatabaseStateChanged(const KLUPD::FileVector &vecFiles, bool bConsistent, const KLUPD::NoCaseString& strDate);

    virtual bool checkPreInstalledComponent(const KLUPD::Path &suffix, const KLUPD::NoCaseString &componentIdentifier, const KLUPD::FileVector &);
    virtual void lockComponentForUpdate(const KLUPD::NoCaseString &componentIdentifier);
    virtual void unlockComponentForUpdate(const KLUPD::NoCaseString &componentIdentifier);

	virtual bool expandEnvironmentString(const KLUPD::NoCaseString &input, KLUPD::NoCaseString &output, const KLUPD::StringParser::ExpandOrder &);

    virtual void removeLockedFileRequest(const KLUPD::Path &fileName);

    virtual void switchToSystemContext(const std::string &traceText);
    virtual void switchToUserContext(const std::string &traceText);

    // Warning: initialization order is important before m_updaterSettingsFile member
    KLUPD::Log *pLog;

    std::vector<KLUPD::Path> m_lockedFilesToRemove;

    // black list only mode is used to update only black list file
     // in case black list is damaged or absent in product
    bool m_blackListOnly;


private:
    friend class PragueJournal;

    PRAGUE_HELPERS::PragueEnvironmentWrapper m_pragueEnvironmentWrapper;

    // helper for dialing with Prague properties
    enum UpdaterPropertyIdentifier
    {
        propertyComponentsToUpdate,
        propertyComponentsToRetranslate,
    };
    tERROR getStringFromUpdaterProperty(KLUPD::NoCaseString &result, cUpdaterConfigurator &, const UpdaterPropertyIdentifier &);


    // configures update operation network settings
    void loadNetworkConfiguration();
    // load retranslation and update filters
    bool loadFilters();

    // convert Update status to Prague status
    static cUpdaterUpdatedFile::ChangeStatus changeStatusToPragueChangeStatus(const KLUPD::FileInfo::ChangeStatus &);
    // convert the KLUPD::FileInfo structure to serializable Prague structure
    static cUpdaterUpdatedFile serializedFileFromFileInfo(const KLUPD::FileInfo &);
    void informApplicationAboutReceivedUpdate(const KLUPD::FileVector &, const bool rollbackAvailable, const bool retranslationMode, const bool rollbackMode, const KLUPD::NoCaseString& strDate);
    bool checkIfAnyComponentIsUpdated(const KLUPD::FileVector &);
    // check if reboot needed on component update
    bool needRebootIfComponentUpdated(const KLUPD::ComponentIdentefiers &)const;

    KLUPD::Path rollbackFileName();
    KLUPD::Path updaterSettingsFile();


    // the member must be initialized before
    CUpdater2005 &m_updateTask;

    KLUPD::UpdaterConfiguration m_updaterConfiguration;

    // store component filter for localization components by Updater Core request
    KLUPD::IniFile m_updaterSettingsFile;

    // the implementation of rollback is ini-file based file
    KLUPD::IniFile m_rollbackFile;
    KLUPD::IniBasedRollbackFile m_rollbackForUpdate;
    
    // the flag is set and sent to product in case file is replaced on reboot
    tBOOL m_needReboot;

    // component set which update require reboot
    KLUPD::ComponentIdentefiers m_needRebootComponents;

    // for products like File Server no interaction with user should be performed
    std::map<KLUPD::NoCaseString, cMutex *> m_mutexByComponent;

    // folder where data files for update operation are located
    static const KLUPD::Path s_updaterTemporarySubfolder;

    static const KLUPD::NoCaseString s_updateOperationPrimaryIndex;
    static const KLUPD::NoCaseString s_retranslationOperationPrimaryIndex;
    static const KLUPD::NoCaseString s_updaterConfigurationFileName;

    static const KLUPD::NoCaseString s_rollbackComponentFilter;
    
    // the name of the section to store path to localizaiton file
    static const KLUPD::NoCaseString s_componentLocalizationSection;
    // section name to store installation identifier
    static const KLUPD::NoCaseString s_installationIdentifierSection;


    // hardcoded name used to get product build to filter updates
    static const KLUPD::NoCaseString s_build;
    static const KLUPD::NoCaseString s_OS;

    static const KLUPD::Path s_rollbackFileName;
    static const KLUPD::Path s_updaterSettingsFile;
    static const KLUPD::Path s_componentLocalizationFileName;

    static const KLUPD::NoCaseString s_needRebootIfComponentUpdatedRegistryKey;

    static const KLUPD::NoCaseString s_emptyString;
};

#endif  // PRAGUECALLBACKS_H__24752399_0E52_40F8_90D7_F3B6121FB515__INCLUDED_
