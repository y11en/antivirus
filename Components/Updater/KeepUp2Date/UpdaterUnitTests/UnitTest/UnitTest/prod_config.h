#if !defined(INI_PRODUCT_CONFIG_H_INCLUDED)
#define INI_PRODUCT_CONFIG_H_INCLUDED

#include "ini_helper.h"

class Ini_ProductConfiguration : public ProductConfiguration
{
public:
    Ini_ProductConfiguration(Updater &, Log *, const bool useCurrentFolder);
    
    virtual ~Ini_ProductConfiguration()
    {
    }
    /// returns obtained data
    /// @except throw const STRING &
    virtual const ProductConfigurationData &GetData();

    /// Change version information
    /// @param application [in] Application ID
    /// @param component [in] Component ID
    /// @param version [in] Version
    /// @return true when written
    virtual bool SetVersionInformation(const STRING &application, const STRING &component, const STRING &version);
    
    virtual bool RemoveFromComponentList(const STRING &compID);
    
    virtual void LoadUpdateID(unsigned long &installationID, unsigned long &updateSessionID);
    virtual void SaveUpdateID(unsigned long installationID, unsigned long updateSessionID);
    
    virtual unsigned long GetApplicationID();

	/// Product configuration data
    ProductConfigurationData data;
    
private:
    /// Reads configuration data to data members
    /// @return Configuration has been read OK
    bool Read();
    
    bool m_UseCurrentDir;
    
    /// Flag indicating, data has been read
    bool data_obtained;
};

#endif

