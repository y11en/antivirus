#ifndef CONFIGURATION_H
#define CONFIGURATION_H

#pragma warning(disable : 4786)

/*
typedef pair<string, string> pair_parval;
typedef vector<parval> vec_parval;
typedef pair<string, vec_parval> pair_sec;
*/
#include <string>
#include <map>

using namespace std;

//#include <windows.h>

struct Config
{
	string difftool;
};

struct Config_FileTransaction: public Config
{
	string source;
	string source_etalon;
	string destination;
	string destination_etalon;
	string backup;
	string backup_etalon;
	string difftool;
	string tdtool;
	string storage;
	string storage2;
	string storage3;
};

struct Config_LocalFile: public Config
{
	string destination;
	string destination_etalon;
	string source;
	string source_etalon;
	string storage;
};

struct Config_NetCore: public Config
{
	string destination;
	string destination_etalon;
	string storage;

};

struct Config_UpdateInfo
{
	string xmlfolder;
	string destination;
};

#define SECTION_CLASSES						"TESTCLASSES"
#define CLASSES_LOCALFILE					"LocalFile"
#define CLASSES_FILETRANSACTION				"FileTransaction"
#define CLASSES_UPDATERTRANSACTION			"UpdaterTransaction"
#define CLASSES_NETCORE						"NetCore"
#define CLASSES_CNETCORE					"CNetCore"
#define CLASSES_UPDATEINFO					"UpdateInfo"
#define CLASSES_CNTLMMAN					"CNtlmMan"
#define CLASSES_CHTTP						"CHTTP"
#define CLASSES_INDEXFILEXMLPARSER			"IndexFileXMLParser"
#define CLASSES_SITESFILEXMLPARSER			"SitesFileXMLParser"
#define CLASSES_PRIMARYINDEXXMLPARSER		"PrimaryIndexXMLParser"


#define KIRYUKHIN_XP						"172.16.128.220"
#define DOWNLOADS2_IP						"193.138.220.187"
#define DOWNLOADS1_IP						"213.206.94.83"
#define AVP_PROXY							"proxy.avp.ru"
#define AVP_PROXY_PORT						8080
#define ZHUZHNEV_PROXY						"172.16.10.108"
#define ZHUZHNEV_WO_AUTH_PORT				8080
#define ZHUZHNEV_BASIC_AUTH_PORT			3128

#define SECTION_COMMON						"Common"
#define COMMON_BASEFOLDER					"BaseFolder"
#define COMMON_DIFFTOOL						"DiffTool"
#define COMMON_TDTOOL						"TDTool"

#define SECTION_FILETRANSACTION				"FileTransaction"
#define FILETRANSACTION_SOURCE				"Source"
#define FILETRANSACTION_DESTINATION			"Destination"
#define FILETRANSACTION_BACKUP				"Backup"
#define FILETRANSACTION_SOURCE_ETALON		"SourceEtalon"
#define FILETRANSACTION_DESTINATION_ETALON	"DestinationEtalon"
#define FILETRANSACTION_BACKUP_ETALON		"BackupEtalon"
#define FILETRANSACTION_DIFFTOOL			"DiffTool"
#define FILETRANSACTION_FILESSTORAGE		"FilesStorage"
#define FILETRANSACTION_FILESSTORAGE2		"FilesStorage2"
#define FILETRANSACTION_FILESSTORAGE3		"FilesStorage3"

#define SECTION_LOCALFILE					"LocalFile"
#define LOCALFILE_DESTINATION				"Destination"
#define LOCALFILE_DESTINATION_ETALON		"DestinationEtalon"
#define LOCALFILE_FILESSTORAGE				"FilesStorage"
#define LOCALFILE_SOURCE					"Source"
#define LOCALFILE_SOURCE_ETALON				"SourceEtalon"

#define SECTION_NETCORE						"NetCore"
#define NETCORE_DESTINATION					"Destination"
#define NETCORE_DESTINATION_ETALON			"DestinationEtalon"
#define NETCORE_STORAGE						"FilesStorage"

#define SECTION_UPDATEINFO					"UpdateInfo"
#define UPDATEINFO_XMLFOLDER				"XmlStorage"
#define UPDATEINFO_DESTINATION				"Destination"


class Configuration
{
	public:

		static Configuration& Init (const char* file);
		
		const Config_FileTransaction& GetFileTransactionConfig ();
		const Config_LocalFile& GetLocalFileConfig ();
		const Config_NetCore& GetNetCoreConfig ();
		const Config_UpdateInfo& GetUpdateInfoConfig ();
		
		map<string, int>& GetClasses ();
		

	private:
		
		bool bRead;
		Configuration (): bRead(false) {};
		void Read (const char* file);
		
		string BaseFolder;
		Config_FileTransaction config_ft;
		Config_LocalFile	   config_lf;
		Config_NetCore		   config_nc;
		Config_UpdateInfo	   config_updinfo;

		map<string, int> classes;

};








#endif