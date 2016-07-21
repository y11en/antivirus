#include "Configuration.h"
#include <windows.h>

Configuration& Configuration::Init (const char* file = NULL)
{
	static Configuration config;
	if ((!config.bRead)&&file)
	{
		config.Read (file);
		config.bRead = true;
	};
	return config;
};

void Configuration::Read (const char* file)
{
	const PAR_SIZE = 256;
	char buf[PAR_SIZE];

	GetPrivateProfileString (SECTION_COMMON, COMMON_BASEFOLDER, "", buf, PAR_SIZE, file);
	string base_folder = buf;
	GetPrivateProfileString (SECTION_COMMON, COMMON_DIFFTOOL, "", buf, PAR_SIZE, file); 
	config_ft.difftool = base_folder + buf;
	config_lf.difftool = base_folder + buf;
	config_nc.difftool = base_folder + buf;
	GetPrivateProfileString (SECTION_COMMON, COMMON_TDTOOL, "", buf, PAR_SIZE, file); 
	config_ft.tdtool = base_folder + buf;

	GetPrivateProfileString (SECTION_FILETRANSACTION, FILETRANSACTION_SOURCE, "", buf, PAR_SIZE, file);
	config_ft.source = base_folder + buf;
	GetPrivateProfileString (SECTION_FILETRANSACTION, FILETRANSACTION_DESTINATION, "", buf, PAR_SIZE, file);
	config_ft.destination = base_folder + buf;
	GetPrivateProfileString (SECTION_FILETRANSACTION, FILETRANSACTION_BACKUP, "", buf, PAR_SIZE, file); 
	config_ft.backup = base_folder + buf;
	
	GetPrivateProfileString (SECTION_FILETRANSACTION, FILETRANSACTION_SOURCE_ETALON, "", buf, PAR_SIZE, file);
	config_ft.source_etalon = base_folder + buf;
	GetPrivateProfileString (SECTION_FILETRANSACTION, FILETRANSACTION_DESTINATION_ETALON, "", buf, PAR_SIZE, file);
	config_ft.destination_etalon = base_folder + buf;
	GetPrivateProfileString (SECTION_FILETRANSACTION, FILETRANSACTION_BACKUP_ETALON, "", buf, PAR_SIZE, file); 
	config_ft.backup_etalon = base_folder + buf;
	

	GetPrivateProfileString (SECTION_FILETRANSACTION, FILETRANSACTION_FILESSTORAGE, "", buf, PAR_SIZE, file); 
	config_ft.storage = base_folder + buf;
	GetPrivateProfileString (SECTION_FILETRANSACTION, FILETRANSACTION_FILESSTORAGE2, "", buf, PAR_SIZE, file); 
	config_ft.storage2 = base_folder + buf;
	GetPrivateProfileString (SECTION_FILETRANSACTION, FILETRANSACTION_FILESSTORAGE3, "", buf, PAR_SIZE, file); 
	config_ft.storage3 = base_folder + buf;

	GetPrivateProfileString (SECTION_LOCALFILE, LOCALFILE_DESTINATION, "", buf, PAR_SIZE, file);
	config_lf.destination = base_folder + buf;
	GetPrivateProfileString (SECTION_LOCALFILE, LOCALFILE_DESTINATION_ETALON, "", buf, PAR_SIZE, file);
	config_lf.destination_etalon = base_folder + buf;
	GetPrivateProfileString (SECTION_LOCALFILE, LOCALFILE_FILESSTORAGE, "", buf, PAR_SIZE, file); 
	config_lf.storage = base_folder + buf;
	GetPrivateProfileString (SECTION_LOCALFILE, LOCALFILE_SOURCE, "", buf, PAR_SIZE, file);
	config_lf.source = base_folder + buf;
	GetPrivateProfileString (SECTION_LOCALFILE, LOCALFILE_SOURCE_ETALON, "", buf, PAR_SIZE, file);
	config_lf.source_etalon = base_folder + buf;

	GetPrivateProfileString (SECTION_NETCORE, NETCORE_DESTINATION, "", buf, PAR_SIZE, file);
	config_nc.destination = base_folder + buf;
	GetPrivateProfileString (SECTION_NETCORE, NETCORE_DESTINATION_ETALON, "", buf, PAR_SIZE, file);
	config_nc.destination_etalon = base_folder + buf;
	GetPrivateProfileString (SECTION_NETCORE, NETCORE_STORAGE, "", buf, PAR_SIZE, file);
	config_nc.storage = base_folder + buf;

	//UpdateInfo
	GetPrivateProfileString (SECTION_UPDATEINFO, UPDATEINFO_XMLFOLDER, "", buf, PAR_SIZE, file);
	config_updinfo.xmlfolder = base_folder + buf;
	GetPrivateProfileString (SECTION_UPDATEINFO, UPDATEINFO_DESTINATION, "", buf, PAR_SIZE, file);
	config_updinfo.destination = base_folder + buf;
	
	//classes to test
	classes[CLASSES_FILETRANSACTION] = GetPrivateProfileInt (SECTION_CLASSES, CLASSES_FILETRANSACTION, 0, file);
	classes[CLASSES_LOCALFILE] = GetPrivateProfileInt (SECTION_CLASSES, CLASSES_LOCALFILE, 0, file);
	classes[CLASSES_NETCORE] = GetPrivateProfileInt (SECTION_CLASSES, CLASSES_NETCORE, 0, file);
	classes[CLASSES_CNETCORE] = GetPrivateProfileInt (SECTION_CLASSES, CLASSES_CNETCORE, 0, file);
	classes[CLASSES_UPDATERTRANSACTION] = GetPrivateProfileInt (SECTION_CLASSES, CLASSES_UPDATERTRANSACTION, 0, file);
	classes[CLASSES_UPDATEINFO] = GetPrivateProfileInt (SECTION_CLASSES, CLASSES_UPDATEINFO, 0, file);
	classes[CLASSES_CNTLMMAN] = GetPrivateProfileInt (SECTION_CLASSES, CLASSES_CNTLMMAN, 0, file);
	classes[CLASSES_CHTTP] = GetPrivateProfileInt (SECTION_CLASSES, CLASSES_CHTTP, 0, file);
	classes[CLASSES_INDEXFILEXMLPARSER] = GetPrivateProfileInt (SECTION_CLASSES, CLASSES_INDEXFILEXMLPARSER, 0, file);
	classes[CLASSES_SITESFILEXMLPARSER] = GetPrivateProfileInt (SECTION_CLASSES, CLASSES_SITESFILEXMLPARSER, 0, file);
	classes[CLASSES_PRIMARYINDEXXMLPARSER] = GetPrivateProfileInt (SECTION_CLASSES, CLASSES_PRIMARYINDEXXMLPARSER, 0, file);


};

const Config_FileTransaction& Configuration::GetFileTransactionConfig ()
{
	return config_ft;
};

const Config_LocalFile& Configuration::GetLocalFileConfig ()
{
	return config_lf;
};

const Config_NetCore& Configuration::GetNetCoreConfig ()
{
	return config_nc;
};

const Config_UpdateInfo& Configuration::GetUpdateInfoConfig ()
{
	return config_updinfo;
};


map<string, int>& Configuration::GetClasses ()
{
	return classes;
};



