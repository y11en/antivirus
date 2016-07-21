#pragma once

#include "../include/callback_iface.h"
#include "../../../SharedCode/PragueHelpers/PragueEnvironmentWrapper.h"

/**
 * Update configuration implementation.
 */
class PragueCallbacksFake : public KLUPD::CallbackInterface
{
public:
    PragueCallbacksFake();
private:
	/**
	 * Returns updater configuration object.
	 */
	virtual KLUPD::UpdaterConfiguration& updaterConfiguration();

    /// removed rollback information in case product 
    virtual bool removeRollbackSection(const bool retranslationMode) {return false; }
    virtual bool readRollbackFiles(KLUPD::FileVector &, const bool retranslationMode) {return false; }

    /**
	 * Expands environment string.
	 */
	virtual bool expandEnvironmentString(const KLUPD::NoCaseString&, KLUPD::NoCaseString&, const KLUPD::StringParser::ExpandOrder&);
	/**
	 * Default implementation.
	 */
	virtual bool checkFilesToDownload(KLUPD::FileVector&, const KLUPD::NoCaseString&, const bool);
	/**
	 * Returns primary index information.
	 */
	virtual KLUPD::FileInfo getPrimaryIndex(const bool retranslationMode);
	/**
	 * Default implementation.
	 */
	virtual KLUPD::FileInfo getUpdaterConfigurationXml();
	/**
	 * Default implementation.
	 */
	virtual KLUPD::FileInfo getSitesFileXml();

    // process file list AFTER INSTALL. File list may be used to prepare rollback list, or in other ways.
    virtual bool processReceivedFiles(const KLUPD::FileVector&, const bool, const KLUPD::Path&, const bool, const KLUPD::NoCaseString&) {return false; }

public:
	// updater configuration
	KLUPD::UpdaterConfiguration m_config;
private:
	// environment object
    PRAGUE_HELPERS::PragueEnvironmentWrapper m_pragueEnvironmentWrapper;
};
