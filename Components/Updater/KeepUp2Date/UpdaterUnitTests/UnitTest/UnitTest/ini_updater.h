#ifndef INI_UPDATER_H_INCLUDED_B9D7FA4B_D58B_45f5_9D6B_09C520BA6A14
#define INI_UPDATER_H_INCLUDED_B9D7FA4B_D58B_45f5_9D6B_09C520BA6A14

#include "ini_helper.h"
#include "core/updater.h"
#include "prod_config.h"
#include "upd_config.h"
#include "ini_updater_lists.h"


class INI_LIBS_API Ini_Updater : public Updater
{
public:
    /// constructor with external logger
    Ini_Updater(CallbackInterface *, Log *, const bool useCurrentFolder);
	
	ProductConfiguration& getProdConfig()
	{
		getProductConfiguration();
		return *pConfig;
	}
	
	UpdaterSettings& getUpdConfig()
	{
		getUpdaterConfiguration();
		return *uSettings;
	}

protected:  
    /// set callback facility override
    virtual CallbackInterface *setCallbacks();
    /// set journal facility override
    virtual JournalInterface *createJournal();

    /// Get configuration of product being processed. Modifies pConfig member
    /// @return Error code
    virtual KLUPD::CoreError getProductConfiguration();
    
    /// get self configuration. Modifies uSettings member
    /// @return Error code
    virtual KLUPD::CoreError getUpdaterConfiguration();
    
    /// Get updater lists. Modifies uLists member
    /// @return Error code
    virtual KLUPD::CoreError getUpdaterLists();
    
private:
    // disable copy operations
    Ini_Updater &operator=(const Ini_Updater &);
    Ini_Updater(const Ini_Updater &);

    // specifies whether to get storage and lists files from start or current folder
    bool m_useCurrentFolder;
};

#endif  // #ifndef INI_UPDATER_H_INCLUDED_B9D7FA4B_D58B_45f5_9D6B_09C520BA6A14
