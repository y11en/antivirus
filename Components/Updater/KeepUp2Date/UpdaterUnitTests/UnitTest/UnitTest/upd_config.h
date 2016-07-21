#ifndef INIUPDATERSETTINGS_H
#define INIUPDATERSETTINGS_H

#include "include/cfg_updater.h"

class Ini_UpdaterSettings : public UpdaterSettings
{
private:
    bool m_UseCurrentDir;

public:
	Ini_UpdaterSettings(Log *log, const bool useCurrentFolder)
        : UpdaterSettings(log), m_UseCurrentDir(useCurrentFolder), data_obtained(false)
    {
    }
  
	virtual UpdaterConfigurationData &GetData();
	UpdaterConfigurationData data;

private:
	bool Read();

    // read encoded proxy password
    bool decodeProxyPassword(const STRING &configurationFileName);
 
	bool data_obtained;
};

#endif