#include "Test_NetCore.h"
#include "helper.h"



void Test_NetCore::TestSettingUserInfo ()
{
	string reason;

	NetCore _nc (ptr_callback.get(), &journal, ptr_log.get());
	//former _nc.Init (ptr_log.get());
	
	string user ("username");
	string pass ("userpass");

	//UserInfo ui;

	//case 1
	//ui.username = user.c_str();
	//ui.password = pass.c_str();
	_nc.setAuthorizationInformation (user, pass);

	TD_BOOST_CHECK_MESSAGE (AuthorizationInformation_Unit("username", "userpass", "dXNlcm5hbWU6dXNlcnBhc3M=\0") ==
							_nc.m_authorizationInformation, "Checking NetAuthInfo");

	//case 2
	//ui.password = 0;

	_nc.setAuthorizationInformation (user, "");

	TD_BOOST_CHECK_MESSAGE (AuthorizationInformation_Unit("username", "", "dXNlcm5hbWU6") ==
							_nc.m_authorizationInformation, "Checking NetAuthInfo");

	//case 3
	//ui.username = 0;
	//ui.password = pass.c_str();

	_nc.setAuthorizationInformation ("", pass);


	//ui.username = 0;
	//ui.password = 0;

	Step ("TestSettingUserInfo", reason);
};

void Test_NetCore::TestSettingUrl ()
{
	string reason;

	std::cout<<"TestSettingUrl\n";

	NetCore _nc (ptr_callback.get(), &journal, ptr_log.get());
	//former _nc.Init (ptr_log.get());

	string url ("ftp://downloads1.kaspersky-labs.com/index.htm");
	string url2 ("ftp://downloads2.kaspersky-labs.com/index.html");
	string url3 ("ftp://downloads1.kaspersky-labs.com:23/index.html");
	string url4 ("ftp://downloads1.kaspersky-labs.com/");
	string url5 ("ftp://downloads1.kaspersky-labs.com:21/rel1/rel2/");
	string url6 ("ftp://downloads1.kaspersky-labs.com:22/rel1/rel2/rel3/index.htm");
	string url7 ("ftp://downloads1.kaspersky-labs.com");

	string url8 ("\\\\avp_server\\common\\file.txt");
	string url9 ("http://10.64.0.5/common:8088/file.txt");

	//case 1
	TD_BOOST_CHECK_MESSAGE (_nc.SetUrl (url.c_str(), false) == NET_NO_ERROR, "Setting URL");
	TD_BOOST_CHECK_MESSAGE (
		AddressInformation_Unit (AddressInformation::Protocol::ftpPassive, url.c_str(), "downloads1.kaspersky-labs.com",
						DOWNLOADS1_IP, "/index.htm/", 0
						) == _nc.m_addressInformation, "Checking NetUrlInfo"
						);

  
	//case 2
	TD_BOOST_CHECK_MESSAGE (_nc.SetUrl (url2.c_str(), false) == NET_NO_ERROR, "Setting URL");
	TD_BOOST_CHECK_MESSAGE (
		AddressInformation_Unit (AddressInformation::Protocol::ftpPassive, url2.c_str(), "downloads2.kaspersky-labs.com",
						DOWNLOADS2_IP, "/index.html/", 0
						) == _nc.m_addressInformation, "Checking NetUrlInfo"
						);

	//case 2a
	TD_BOOST_CHECK_MESSAGE (_nc.SetUrl (url2.c_str(), true) == NET_NO_ERROR, "Setting URL");
	TD_BOOST_CHECK_MESSAGE (
		AddressInformation_Unit (AddressInformation::Protocol::ftpActive, url2.c_str(), "downloads2.kaspersky-labs.com",
						DOWNLOADS2_IP, "/index.html/", 0
						) == _nc.m_addressInformation, "Checking NetUrlInfo"
						);

	//case 3
	TD_BOOST_CHECK_MESSAGE (_nc.SetUrl (url3.c_str(), true) == NET_NO_ERROR, "Setting URL");
	TD_BOOST_CHECK_MESSAGE (
		AddressInformation_Unit (AddressInformation::Protocol::ftpActive, url3.c_str(), "downloads1.kaspersky-labs.com",
						DOWNLOADS1_IP, "/index.html/", 23
						) == _nc.m_addressInformation, "Checking NetUrlInfo"
						);

	//case 4
	TD_BOOST_CHECK_MESSAGE (_nc.SetUrl (url4.c_str(), true) == NET_NO_ERROR, "Setting URL");
	TD_BOOST_CHECK_MESSAGE (
		AddressInformation_Unit (AddressInformation::Protocol::ftpActive, url4.c_str(), "downloads1.kaspersky-labs.com",
						DOWNLOADS1_IP, "/", 0
						) == _nc.m_addressInformation, "Checking NetUrlInfo"
						);

	//case 5
	TD_BOOST_CHECK_MESSAGE (_nc.SetUrl (url5.c_str(), true) == NET_NO_ERROR, "Setting URL");
	TD_BOOST_CHECK_MESSAGE (
		AddressInformation_Unit (AddressInformation::Protocol::ftpActive, url5.c_str(), "downloads1.kaspersky-labs.com",
						DOWNLOADS1_IP, "/rel1/rel2/", 21
						) == _nc.m_addressInformation, "Checking NetUrlInfo"
						);

	//case 6
	TD_BOOST_CHECK_MESSAGE (_nc.SetUrl (url6.c_str(), true) == NET_NO_ERROR, "Setting URL");
	TD_BOOST_CHECK_MESSAGE (
		AddressInformation_Unit (AddressInformation::Protocol::ftpActive, url6.c_str(), "downloads1.kaspersky-labs.com",
						DOWNLOADS1_IP, "/rel1/rel2/rel3/index.htm/", 22
						) == _nc.m_addressInformation, "Checking NetUrlInfo"
						);

	//case 7
	TD_BOOST_CHECK_MESSAGE (_nc.SetUrl (url7.c_str(), true) == NET_NO_ERROR, "Setting URL");
	TD_BOOST_CHECK_MESSAGE (
		AddressInformation_Unit (AddressInformation::Protocol::ftpActive, url7.c_str(), "downloads1.kaspersky-labs.com",
						DOWNLOADS1_IP, "/", 0
						) == _nc.m_addressInformation, "Checking NetUrlInfo"
						);

	//case 8
	TD_BOOST_CHECK_MESSAGE (_nc.SetUrl (url8.c_str(), true) == NET_NO_ERROR, "Setting URL");
	TD_BOOST_CHECK_MESSAGE (
		AddressInformation_Unit (AddressInformation::Protocol::fileProtocol, "", "",
						"", url8.c_str(), 0
						) == _nc.m_addressInformation, "Checking NetUrlInfo"
						);

	//case 9
	TD_BOOST_CHECK_MESSAGE (_nc.SetUrl (url9.c_str(), true) == NET_WRONG_USAGE, "Setting URL");
	/*TD_BOOST_CHECK_MESSAGE (
		AddressInformation_Unit (AddressInformation::Protocol::notDefined, url9.c_str(), "10.64.0.5",
						"10.64.0.5", "common", 8088
						) == _nc.m_addressInformation, "Checking NetUrlInfo"
						);*/

	Step ("TestSettingUrl", reason);
};

void Test_NetCore::TestGettingFileInfoAndFile ()
{
	string reason;

	std::cout<<"TestGettingFileInfoAndFile\n";

	NetCore _nc (ptr_callback.get(), &journal, ptr_log.get());
	//former _nc.Init (ptr_log.get());
	_nc.setTempPath (config_nc.destination.c_str());

	vector<string> dirs;

	dirs.push_back (config_nc.destination);
	dirs.push_back (config_nc.destination_etalon);

	string url_base = "ftp://kiryukhin-xp";
	string url_rel = "/";
	string url_file = "base005.avc";
	
	string url_base2 = "ftp://kiryukhin-xp";
	string url_rel2 = "/directory/";
	string url_file2 = "base006.avc";

	string url_base3 = "ftp://kiryukhin-xp";
	string url_rel3 = "/directory/";
	string url_file3 = "base007.avc";

	string url_base4 = "\\\\kiryukhin-xp\\common\\";
	string url_file4 = "base061.avc";

	string url_base5 = "\\\\kiryukhin-xp\\common\\";
	string url_file5 = "base006.avc";

	string url_base6 = "\\\\kiryukhin-xp\\restricted\\";
	string url_file6 = "base006.avc";
		
	NetFileInfo fi ("");

	//case 1

	std::cout<<"\nRequesting a simple file info and a file from ftp server\n";

	helper::RecreateDirectories (dirs);
	helper::CloneFile (string(config_nc.storage).append("\\base005.avc").c_str(), string(config_nc.destination_etalon).append("\\base005.avc").c_str(), false);

	fi.m_fileName = string(url_file).c_str();
	_nc.SetUrl (string(url_base+url_rel).c_str(), true);
	//former strcpy (&_nc.m_ui.m_path[0], url_rel.c_str());

	TD_BOOST_CHECK_MESSAGE (_nc.SetFileInfo (fi) == NET_NO_ERROR, "Getting file info");
	//TD_BOOST_CHECK_MESSAGE (
	//	NetFileInfoPointer_Unit (url_file.c_str(), url_rel.c_str(), config_nc.destination.c_str(), 48549) 
	//								== _nc.m_fileInformation, "Checking file info");
	TD_BOOST_CHECK_MESSAGE (_nc.GetFile (fi) == NET_NO_ERROR, "Getting file info");
	TD_BOOST_CHECK_MESSAGE (helper::IsFoldersEqual (config_nc.difftool, config_nc.destination_etalon, config_nc.destination), "Destination folder");

	//case 2
	std::cout<<"\nRequesting a file (path with directories) info and a file from ftp server\n";

	helper::RecreateDirectories (dirs);
	helper::CloneFile (string(config_nc.storage).append("\\base006.avc").c_str(), string(config_nc.destination_etalon).append("\\base006.avc").c_str(), false);

	fi.m_fileName = string(url_rel2 + url_file2).c_str();
	_nc.SetUrl (string(url_base2).c_str(), true);
	//former strcpy (&_nc.m_ui.m_path[0], url_rel2.c_str());

	TD_BOOST_CHECK_MESSAGE (_nc.SetFileInfo (fi) == NET_NO_ERROR, "Getting file info");
	//TD_BOOST_CHECK_MESSAGE (
	//	NetFileInfoPointer_Unit (url_file2.c_str(), url_rel2.c_str(), config_nc.destination.c_str(), 48305) 
	//								== _nc.m_fileInformation, "Checking file info");
	TD_BOOST_CHECK_MESSAGE (_nc.GetFile (fi) == NET_NO_ERROR, "Getting file info");
	TD_BOOST_CHECK_MESSAGE (helper::IsFoldersEqual (config_nc.difftool, config_nc.destination_etalon, config_nc.destination), "Destination folder");
  
	//case 3
	std::cout<<"\nRequesting a non-existent file info and a file from ftp server when the source file doesnt exist\n";

	helper::RecreateDirectories (dirs);
	//helper::CloneFile (string(config_nc.storage).append("\\base007.avc").c_str(), string(config_nc.destination_etalon).append("\\base007.avc").c_str(), false);
	
	fi.m_fileName = string(url_rel3+url_file3).c_str();
	_nc.SetUrl (string(url_base3).c_str(), true);
	//former strcpy (&_nc.m_ui.m_path[0], url_rel3.c_str());

	TD_BOOST_CHECK_MESSAGE (_nc.SetFileInfo (fi) == NET_NO_ERROR, "Getting file info");
	TD_BOOST_CHECK_MESSAGE (_nc.GetFile (fi) == NET_FILE_NOT_FOUND, "Getting file info");
	TD_BOOST_CHECK_MESSAGE (helper::IsFoldersEqual (config_nc.difftool, config_nc.destination_etalon, config_nc.destination), "Destination folder");

	//case 4
	std::cout<<"\nRequesting a file info and a file from ftp server when a part of the target file already exists\n";

	helper::RecreateDirectories (dirs);
	helper::CloneFile (string(config_nc.storage).append("\\base006.avc").c_str(), string(config_nc.destination_etalon).append("\\base006.avc").c_str(), false);
	helper::CloneFile (string(config_nc.storage).append("\\base006a.avc").c_str(), string(config_nc.destination).append("\\base006.avc").c_str(), false);
	
	fi.m_fileName = string(url_rel2+url_file2).c_str();
	_nc.SetUrl (string(url_base2).c_str(), true);
	//former strcpy (&_nc.m_ui.m_path[0], url_rel2.c_str());

	TD_BOOST_CHECK_MESSAGE (_nc.SetFileInfo (fi) == NET_NO_ERROR, "Getting file info");
	_nc.m_fileInformation.m_localSize = 100;
	TD_BOOST_CHECK_MESSAGE (_nc.GetFile (fi) == NET_NO_ERROR, "Getting file info");
	TD_BOOST_CHECK_MESSAGE (helper::IsFoldersEqual (config_nc.difftool, config_nc.destination_etalon, config_nc.destination), "Destination folder");

	//case 5
	std::cout<<"\nRequesting a file info and a file from ftp server when the size of the existent part of the target file exceeds the size of the original file\n";

	helper::RecreateDirectories (dirs);
	helper::CloneFile (string(config_nc.storage).append("\\base006a.avc").c_str(), string(config_nc.destination_etalon).append("\\base006.avc").c_str(), false);
	helper::CloneFile (string(config_nc.storage).append("\\base006a.avc").c_str(), string(config_nc.destination).append("\\base006.avc").c_str(), false);
	
	fi.m_fileName = string(url_rel2+url_file2).c_str();
	_nc.SetUrl (string(url_base2).c_str(), true);
	//formertstrcpy (&_nc.m_ui.m_path[0], url_rel2.c_str());

	TD_BOOST_CHECK_MESSAGE (_nc.SetFileInfo (fi) == NET_NO_ERROR, "Getting file info");
	_nc.m_fileInformation.m_localSize = 100000;
	TD_BOOST_CHECK_MESSAGE (_nc.GetFile (fi) == NET_FILE_NOT_FOUND, "Getting file info");
	TD_BOOST_CHECK_MESSAGE (helper::IsFoldersEqual (config_nc.difftool, config_nc.destination_etalon, config_nc.destination), "Destination folder");


	//case 6
	std::cout<<"\nRequesting a file info and a file from ftp server when it cannot be written\n";

	helper::RecreateDirectories (dirs);
	helper::CloneFile (string(config_nc.storage).append("\\base006a.avc").c_str(), string(config_nc.destination_etalon).append("\\base006.avc").c_str(), false);
	helper::CloneFile (string(config_nc.storage).append("\\base006a.avc").c_str(), string(config_nc.destination).append("\\base006.avc").c_str(), false);
	
	fi.m_fileName = string(url_rel2+url_file2).c_str();
	_nc.SetUrl (string(url_base2).c_str(), true);
	//former strcpy (&_nc.m_ui.m_path[0], url_rel2.c_str());

	TD_BOOST_CHECK_MESSAGE (_nc.SetFileInfo (fi) == NET_NO_ERROR, "Getting file info");
	_nc.m_fileInformation.m_localSize = 100;

	{
		AutoFile hFile (string(config_nc.destination).append("\\base006.avc").c_str(), GENERIC_WRITE, FILE_SHARE_READ, OPEN_EXISTING);
	
		TD_BOOST_CHECK_MESSAGE (_nc.GetFile (fi) == NET_FILE_IO_ERROR, "Getting file");
		TD_BOOST_CHECK_MESSAGE (helper::IsFoldersEqual (config_nc.difftool, config_nc.destination_etalon, config_nc.destination), "Destination folder");
		TD_BOOST_CHECK_MESSAGE (_nc.SetFileInfo (fi) == NET_NO_ERROR, "Getting file info");
	};

	//case 7
	std::cout<<"\nRequesting a file info and a file from ftp server in passive mode\n";

	helper::RecreateDirectories (dirs);
	helper::CloneFile (string(config_nc.storage).append("\\base006.avc").c_str(), string(config_nc.destination_etalon).append("\\base006.avc").c_str(), false);

	fi.m_fileName = string(url_rel2+url_file2).c_str();
	_nc.SetUrl (string(url_base2).c_str(), false);
	//former strcpy (&_nc.m_ui.m_path[0], url_rel2.c_str());

	TD_BOOST_CHECK_MESSAGE (_nc.SetFileInfo (fi) == NET_NO_ERROR, "Getting file info");
//	TD_BOOST_CHECK_MESSAGE (
//		NetFileInfoPointer_Unit (url_file2.c_str(), url_rel2.c_str(), config_nc.destination.c_str(), 48305) 
//									== _nc.m_fileInformation, "Checking file info");
	TD_BOOST_CHECK_MESSAGE (_nc.GetFile (fi) == NET_NO_ERROR, "Getting file info");
	TD_BOOST_CHECK_MESSAGE (helper::IsFoldersEqual (config_nc.difftool, config_nc.destination_etalon, config_nc.destination), "Destination folder");
	

	//case 8
	std::cout<<"\nRequesting a file info and a file via SMB\n";
	helper::RecreateDirectories (dirs);
	helper::CloneFile (string(config_nc.storage).append("\\base061.avc").c_str(), string(config_nc.destination_etalon).append("\\base061.avc").c_str(), false);
	
	fi.m_fileName = string(url_file4).c_str();
	_nc.SetUrl (string(url_base4).c_str(), false);

	TD_BOOST_CHECK_MESSAGE (_nc.SetFileInfo (fi) == NET_NO_ERROR, "Getting file info");
	TD_BOOST_CHECK_MESSAGE (_nc.GetFile (fi) == NET_NO_ERROR, "Getting file");
	TD_BOOST_CHECK_MESSAGE (helper::IsFoldersEqual (config_nc.difftool, config_nc.destination_etalon, config_nc.destination), "Destination folder");
	

	//case 9
	std::cout<<"\nRequesting a file info and a file via SMB when the part of the target file has been already created\n";
	helper::RecreateDirectories (dirs);
	helper::CloneFile (string(config_nc.storage).append("\\base006.avc").c_str(), string(config_nc.destination_etalon).append("\\base006.avc").c_str(), false);
	helper::CloneFile (string(config_nc.storage).append("\\base006a.avc").c_str(), string(config_nc.destination).append("\\base006.avc").c_str(), false);
	
	fi.m_fileName = string(url_file5).c_str();
	_nc.SetUrl (string(url_base5).c_str(), false);

	TD_BOOST_CHECK_MESSAGE (_nc.SetFileInfo (fi) == NET_NO_ERROR, "Getting file info");
	_nc.m_fileInformation.m_localSize = 100;
	TD_BOOST_CHECK_MESSAGE (_nc.GetFile (fi) == NET_NO_ERROR, "Getting file");
	TD_BOOST_CHECK_MESSAGE (helper::IsFoldersEqual (config_nc.difftool, config_nc.destination_etalon, config_nc.destination), "Destination folder");

	
	//case 10
	std::cout<<"\nRequesting a file info and a file via SMB when the part of the target file has been already created and locked\n";
	helper::RecreateDirectories (dirs);
	helper::CloneFile (string(config_nc.storage).append("\\base006a.avc").c_str(), string(config_nc.destination_etalon).append("\\base006.avc").c_str(), false);
	helper::CloneFile (string(config_nc.storage).append("\\base006a.avc").c_str(), string(config_nc.destination).append("\\base006.avc").c_str(), false);
	
	fi.m_fileName = string(url_file5).c_str();
	_nc.SetUrl (string(url_base5).c_str(), false);

	TD_BOOST_CHECK_MESSAGE (_nc.SetFileInfo (fi) == NET_NO_ERROR, "Getting file info");
	_nc.m_fileInformation.m_localSize = 100;
	{
		AutoFile hFile (string(config_nc.destination).append("\\base006.avc").c_str(), GENERIC_WRITE, FILE_SHARE_READ, OPEN_EXISTING);
	
		TD_BOOST_CHECK_MESSAGE (_nc.GetFile (fi) == NET_FILE_IO_ERROR, "Getting file");
		TD_BOOST_CHECK_MESSAGE (helper::IsFoldersEqual (config_nc.difftool, config_nc.destination_etalon, config_nc.destination), "Destination folder");
	};
	
	//case 11
	std::cout<<"\nRequesting a file info and a file via SMB when the access is prohibited\n";
	helper::RecreateDirectories (dirs);

	fi.m_fileName = string(url_file6).c_str();
	_nc.SetUrl (string(url_base6).c_str(), false);

	TD_BOOST_CHECK_MESSAGE (_nc.SetFileInfo (fi) == NET_NO_ERROR, "Getting file info");

	TD_BOOST_CHECK_MESSAGE (_nc.GetFile (fi) == NET_FILE_NOT_FOUND, "Getting file");
	TD_BOOST_CHECK_MESSAGE (helper::IsFoldersEqual (config_nc.difftool, config_nc.destination_etalon, config_nc.destination), "Destination folder");
	

	//case 12
	std::cout<<"\nRequesting a file info and a file via SMB when the part of the target file has been already created and its size is greater, than the size of the source file\n";
	helper::RecreateDirectories (dirs);
	helper::CloneFile (string(config_nc.storage).append("\\base006b.avc").c_str(), string(config_nc.destination_etalon).append("\\base006.avc").c_str(), false);
	helper::CloneFile (string(config_nc.storage).append("\\base006b.avc").c_str(), string(config_nc.destination).append("\\base006.avc").c_str(), false);
	
	fi.m_fileName = string(url_file5).c_str();
	_nc.SetUrl (string(url_base5).c_str(), false);

	TD_BOOST_CHECK_MESSAGE (_nc.SetFileInfo (fi) == NET_NO_ERROR, "Getting file info");
	TD_BOOST_CHECK_MESSAGE (_nc.GetFile (fi) == NET_NO_ERROR, "Getting file");
	TD_BOOST_CHECK_MESSAGE (helper::IsFoldersEqual (config_nc.difftool, config_nc.destination_etalon, config_nc.destination), "Destination folder");

	Step ("TestGettingFileInfoAndFile", reason);
};


Suite_NetCore::Suite_NetCore (const string& ini_file, const string& log_name)
{
	bool bLog;
	if (log_name == "") bLog =false; else bLog = true;

	boost::shared_ptr<Test_NetCore> instance (new Test_NetCore (ini_file, bLog, log_name));
	
	add (boost::unit_test::make_test_case<Test_NetCore>(&Test_NetCore::Start, "Test_NetCore::Start", instance));
	add (BOOST_CLASS_TEST_CASE (&Test_NetCore::TestSettingUserInfo, instance));
	add (BOOST_CLASS_TEST_CASE (&Test_NetCore::TestSettingUrl, instance));
	add (BOOST_CLASS_TEST_CASE (&Test_NetCore::TestGettingFileInfoAndFile, instance));
	add (boost::unit_test::make_test_case<Test_NetCore>(&Test_NetCore::End, "Test_NetCore::End", instance));
	

	
	

}	