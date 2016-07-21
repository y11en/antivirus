#ifndef INICALLBACKS_H_C245FF06_C40D_4EC5_9365_067129C1BE0A__INCLUDED_
#define INICALLBACKS_H_C245FF06_C40D_4EC5_9365_067129C1BE0A__INCLUDED_

#if _MSC_VER > 1000
    #pragma once
#endif // _MSC_VER > 1000

#include "../helper/stdinc.h"
#include "../../IniFile/IniBasedRollback.h"

#include "ini_log.h"
#include "IniConfiguration.h"

class IniCallbacks : public KLUPD::CallbackInterface
{
public:
    IniCallbacks(const IniConfiguration &, IniLog &);
    bool readUpdaterConfiguration();

    virtual KLUPD::UpdaterConfiguration &updaterConfiguration();

    virtual bool removeRollbackSection(const bool retranslationMode);
    virtual bool readRollbackFiles(KLUPD::FileVector &, const bool retranslationMode);

    virtual bool checkIfFileOptional(const KLUPD::FileInfo &absentFile, const bool retranslationMode);

    virtual KLUPD::FileInfo getPrimaryIndex(const bool retranslationMode);
    virtual KLUPD::FileInfo getUpdaterConfigurationXml();
    virtual KLUPD::FileInfo getSitesFileXml();

    virtual bool processReceivedFiles(const KLUPD::FileVector &, const bool createRollbackNeeded,
        const KLUPD::Path &rollbackFolder, const bool retranslationMode, const KLUPD::NoCaseString& strUpdateDate);

    virtual bool expandEnvironmentString(const KLUPD::NoCaseString &input, KLUPD::NoCaseString &output, const KLUPD::StringParser::ExpandOrder &);

private:
    // disable copy operations
    IniCallbacks &operator=(const IniCallbacks &);
    IniCallbacks(const IniCallbacks &);

    void readPrimaryIndexChangedByConfiguration();

    // read user sources from configuration Ini file and append it to updater source list
    void addUserSources(const bool useProxy);
    // read sources from configuration file
    void readSources();

    // read encoded proxy password
    void decodeProxyPassword();
    void readRetranslationApplicationsFromIniFile(const KLUPD::NoCaseString &sectionName, bool &mode, KLUPD::MapStringVStringWrapper &);
    void readRetranslationComponentsFromIniFile(const KLUPD::NoCaseString &sectionName, bool &mode, KLUPD::ComponentIdentefiers &);
    
    void readRetranslationObjectsList(KLUPD::UpdaterConfiguration::SRetranslatedObjects &);


    /// read list of optional components
    void optionalComponents(KLUPD::ComponentIdentefiers &components)const;



    KLUPD::Path getConfigurationFileName();
    KLUPD::Path rollbackFileName(const bool retranslationMode);

    // Warning: initialization order is important before m_configurationFile member
    KLUPD::Log *pLog;

    const IniConfiguration &m_iniConfiguration;

	KLUPD::UpdaterConfiguration m_updaterConfiguration;

    // primary index cache to begin update with
    KLUPD::FileInfo m_primaryIndex;

    KLUPD::IniFile m_configurationFile;

    KLUPD::IniFile m_rollbackFileForRetranslation;
    KLUPD::IniFile m_rollbackFileForUpdate;
    KLUPD::IniBasedRollbackFile m_rollbackForRetranslation;
    KLUPD::IniBasedRollbackFile m_rollbackForUpdate;

    // a set of optional components
    KLUPD::ComponentIdentefiers m_optionalComponents;


    static const wchar_t *s_rollbackFileNameForRetranslation;
    static const wchar_t *s_rollbackFileNameForUpdate;
    // updater configuration for products of 5 line hardcoded file name
    static const wchar_t *s_updaterConfigurationFileName5ProductsLine;
    // hardcoded locations for primary index, configuration, and site files
    static const wchar_t *s_primaryIndexRelativeUrlPath;
    // sites file for products of 5 line hardcoded file name
    static const wchar_t *s_additionalSitesFileName5ProductsLine;
};

#endif  // AFX_INICALLBACKS_H__C245FF06_C40D_4EC5_9365_067129C1BE0A__INCLUDED_
