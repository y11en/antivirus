#include "Test_CNetCore.h"
#include "helper.h"

void Test_CNetCore::TestDownloadHttp ()
{
	string reason;
	vector<string> dirs;
	dirs.push_back (config_nc.destination);
	dirs.push_back (config_nc.destination_etalon);

	CNetFileInfo _fi;
	NetCore _nc (&m_callback, &m_journal, &m_log);
	CNetCore _cnc (_nc, &m_log);
	KLUPD::ProxyAuthorizationMethods _cpaml;

	string url_base = "http://kiryukhin-xp";
	string url_rel = "/";
	string url_file = "base005.avc";
	
	string url_base2 = "http://kiryukhin-xp";
	string url_rel2 = "/directory/";
	string url_file2 = "base006.avc";

	string url_base3 = "http://kiryukhin-xp";
	string url_rel3 = "/directory/";
	string url_file3 = "base007.avc";

	//case 1
	std::cout<<"\nRequesting a simple file from http server\n";
	helper::RecreateDirectories (dirs);
	helper::CloneFile (string(config_nc.storage).append("\\base005.avc").c_str(), string(config_nc.destination_etalon).append("\\base005.avc").c_str(), false);

    _fi.m_remotePath = url_rel.c_str();
    _fi.m_remoteFilename = url_file.c_str();
    _fi.m_localPath = config_nc.destination.c_str();
    _fi.m_localFilename = url_file.c_str();
    _fi.m_regetting = false;

	{
		KLUPD::Source _csd (string(url_base).c_str());

		TD_BOOST_CHECK_MESSAGE (NFER_NoError == _cnc.getRemoteFile (_csd, _fi, m_upddata, _cpaml, NULL), "Getting files");
		TD_BOOST_CHECK_MESSAGE (helper::IsFoldersEqual (config_nc.difftool, config_nc.destination_etalon, config_nc.destination), "Checking destination folder");
	}

	//case 2
	std::cout<<"\nRequesting a file (path with directories) from http server\n";

	helper::RecreateDirectories (dirs);
	helper::CloneFile (string(config_nc.storage).append("\\base006.avc").c_str(), string(config_nc.destination_etalon).append("\\base006.avc").c_str(), false);

    _fi.m_remotePath = url_rel2.c_str();
    _fi.m_remoteFilename = url_file2.c_str();
    _fi.m_localPath = config_nc.destination.c_str();
    _fi.m_localFilename = url_file2.c_str();
    _fi.m_regetting = false;

	{
		KLUPD::Source _csd (string(url_base2).c_str());

		TD_BOOST_CHECK_MESSAGE (NFER_NoError == _cnc.getRemoteFile (_csd, _fi, m_upddata, _cpaml, NULL), "Getting files");
		TD_BOOST_CHECK_MESSAGE (helper::IsFoldersEqual (config_nc.difftool, config_nc.destination_etalon, config_nc.destination), "Checking destination folder");
	}

	//case 3
	std::cout<<"\nRequesting a non-existent file from http server when the source file doesnt exist\n";

	helper::RecreateDirectories (dirs);
	
    _fi.m_remotePath = url_rel3.c_str();
    _fi.m_remoteFilename = url_file3.c_str();
    _fi.m_localPath = config_nc.destination.c_str();
    _fi.m_localFilename = url_file3.c_str();
    _fi.m_regetting = false;

	{
		KLUPD::Source _csd (string(url_base3).c_str());

		TD_BOOST_CHECK_MESSAGE (NFER_NoFile == _cnc.getRemoteFile (_csd, _fi, m_upddata, _cpaml, NULL), "Getting files");
		TD_BOOST_CHECK_MESSAGE (helper::IsFoldersEqual (config_nc.difftool, config_nc.destination_etalon, config_nc.destination), "Checking destination folder");
	}

	//case 4
	std::cout<<"\nRequesting a file from http server when a part of the target file already exists\n";

	helper::RecreateDirectories (dirs);
	helper::CloneFile (string(config_nc.storage).append("\\base006.avc").c_str(), string(config_nc.destination_etalon).append("\\base006.avc").c_str(), false);
	helper::CloneFile (string(config_nc.storage).append("\\base006a.avc").c_str(), string(config_nc.destination).append("\\base006.avc").c_str(), false);
	
	
    _fi.m_remotePath = url_rel2.c_str();
    _fi.m_remoteFilename = url_file2.c_str();
    _fi.m_localPath = config_nc.destination.c_str();
    _fi.m_localFilename = url_file2.c_str();
    _fi.m_regetting = true;

	{
		KLUPD::Source _csd (string(url_base2).c_str());

		TD_BOOST_CHECK_MESSAGE (NFER_NoError == _cnc.getRemoteFile (_csd, _fi, m_upddata, _cpaml, NULL), "Getting files");
		TD_BOOST_CHECK_MESSAGE (helper::IsFoldersEqual (config_nc.difftool, config_nc.destination_etalon, config_nc.destination), "Checking destination folder");
	}

	//case 5
	std::cout<<"\nRequesting a file from http server when the size of the existent part of the target file exceeds the size of the original file\n";

	helper::RecreateDirectories (dirs);
	helper::CloneFile (string(config_nc.storage).append("\\base006b.avc").c_str(), string(config_nc.destination_etalon).append("\\base006.avc").c_str(), false);
	helper::CloneFile (string(config_nc.storage).append("\\base006b.avc").c_str(), string(config_nc.destination).append("\\base006.avc").c_str(), false);
	
	
    _fi.m_remotePath = url_rel2.c_str();
    _fi.m_remoteFilename = url_file2.c_str();
    _fi.m_localPath = config_nc.destination.c_str();
    _fi.m_localFilename = url_file2.c_str();
    _fi.m_regetting = true;

	{
		KLUPD::Source _csd (string(url_base2).c_str());

		TD_BOOST_CHECK_MESSAGE (NFER_HTTP_GeneralError == _cnc.getRemoteFile (_csd, _fi, m_upddata, _cpaml, NULL), "Getting files");
		TD_BOOST_CHECK_MESSAGE (helper::IsFoldersEqual (config_nc.difftool, config_nc.destination_etalon, config_nc.destination), "Checking destination folder");
	}

	//case 6
	std::cout<<"\nRequesting a file from http server when it cannot be written\n";

	helper::RecreateDirectories (dirs);
	helper::CloneFile (string(config_nc.storage).append("\\base006a.avc").c_str(), string(config_nc.destination_etalon).append("\\base006.avc").c_str(), false);
	helper::CloneFile (string(config_nc.storage).append("\\base006a.avc").c_str(), string(config_nc.destination).append("\\base006.avc").c_str(), false);

	_fi.m_remotePath = url_rel2.c_str();
    _fi.m_remoteFilename = url_file2.c_str();
    _fi.m_localPath = config_nc.destination.c_str();
    _fi.m_localFilename = url_file2.c_str();
    _fi.m_regetting = false;

	{
		KLUPD::Source _csd (string(url_base2).c_str());
		AutoFile hFile (string(config_nc.destination).append("\\base006.avc").c_str(), GENERIC_WRITE, FILE_SHARE_READ, OPEN_EXISTING);

		TD_BOOST_CHECK_MESSAGE (NFER_FileOperationFailed == _cnc.getRemoteFile (_csd, _fi, m_upddata, _cpaml, NULL), "Getting files");
		TD_BOOST_CHECK_MESSAGE (helper::IsFoldersEqual (config_nc.difftool, config_nc.destination_etalon, config_nc.destination), "Checking destination folder");
	}

	//case 7
	std::cout<<"\nRequesting a simple file from http server after the failure\n";
	helper::RecreateDirectories (dirs);
	helper::CloneFile (string(config_nc.storage).append("\\base005.avc").c_str(), string(config_nc.destination_etalon).append("\\base005.avc").c_str(), false);

    _fi.m_remotePath = url_rel.c_str();
    _fi.m_remoteFilename = url_file.c_str();
    _fi.m_localPath = config_nc.destination.c_str();
    _fi.m_localFilename = url_file.c_str();
    _fi.m_regetting = false;

	{
		KLUPD::Source _csd (string(url_base).c_str());

		TD_BOOST_CHECK_MESSAGE (NFER_NoError == _cnc.getRemoteFile (_csd, _fi, m_upddata, _cpaml, NULL), "Getting files");
		TD_BOOST_CHECK_MESSAGE (helper::IsFoldersEqual (config_nc.difftool, config_nc.destination_etalon, config_nc.destination), "Checking destination folder");
	}
	Step ("TestDownloadHttp", reason);

}

void Test_CNetCore::TestUsingHttpProxy ()
{
	string reason;
	std::cout<<"\nChecking using authorization\n";

	vector<string> dirs;
	dirs.push_back (config_nc.destination);
	dirs.push_back (config_nc.destination_etalon);

	CNetFileInfo _fi;
	NetCore _nc (&m_callback, &m_journal, &m_log);
	CNetCore _cnc (_nc, &m_log);

	string url_base = "http://kiryukhin-xp";
	string url_rel = "/";
	string url_file = "base005.avc";
	
	string url_base2 = "http://kiryukhin-xp";
	string url_rel2 = "/directory/";
	string url_file2 = "base006.avc";

	string url_base3 = "http://kiryukhin-xp";
	string url_rel3 = "/directory/";
	string url_file3 = "base007.avc";

	_fi.m_remotePath = url_rel.c_str();
    _fi.m_remoteFilename = url_file.c_str();
    _fi.m_localPath = config_nc.destination.c_str();
    _fi.m_localFilename = url_file.c_str();
    _fi.m_regetting = false;

	//case 1
	std::cout<<"\nRequesting a simple file from http server without authorization, when it is not supported \n";
	helper::RecreateDirectories (dirs);

	{
		KLUPD::Source _csd (string(url_base).c_str());
		_csd.m_useProxy = true;
		KLUPD::ProxyAuthorizationMethods _cpaml;

		m_upddata.proxy_url = ZHUZHNEV_PROXY;
		m_upddata.proxy_port = ZHUZHNEV_BASIC_AUTH_PORT;
		m_upddata.proxy_login = "";
		m_upddata.proxy_pwd = "";

		TD_BOOST_CHECK_MESSAGE (NFER_ProxyAuthFailed == _cnc.getRemoteFile (_csd, _fi, m_upddata, _cpaml, NULL), "Getting files");
		TD_BOOST_CHECK_MESSAGE (helper::IsFoldersEqual (config_nc.difftool, config_nc.destination_etalon, config_nc.destination), "Checking destination folder");
	}

	//case 1a
	std::cout<<"\nRequesting a simple file from http server without authorization, when it is not supported (variant 2)\n";
	helper::RecreateDirectories (dirs);

	{
		KLUPD::Source _csd (string(url_base).c_str());
		_csd.m_useProxy = true;
		KLUPD::ProxyAuthorizationMethods _cpaml;
		_cpaml.AddAuthMethod (KLUPD::AUT_Basic, false);

		m_upddata.proxy_url = ZHUZHNEV_PROXY;
		m_upddata.proxy_port = ZHUZHNEV_BASIC_AUTH_PORT;
		m_upddata.proxy_login = "1";
		m_upddata.proxy_pwd = "1";

		TD_BOOST_CHECK_MESSAGE (NFER_ProxyAuthFailed == _cnc.getRemoteFile (_csd, _fi, m_upddata, _cpaml, NULL), "Getting files");
		TD_BOOST_CHECK_MESSAGE (helper::IsFoldersEqual (config_nc.difftool, config_nc.destination_etalon, config_nc.destination), "Checking destination folder");
	}

	//case 2
	std::cout<<"\nRequesting a simple file from http server without authorization, when it is supported \n";
	helper::RecreateDirectories (dirs);
	helper::CloneFile (string(config_nc.storage).append("\\base005.avc").c_str(), string(config_nc.destination_etalon).append("\\base005.avc").c_str(), false);
	
    _fi.m_remotePath = url_rel.c_str();
    _fi.m_remoteFilename = url_file.c_str();
    _fi.m_localPath = config_nc.destination.c_str();
    _fi.m_localFilename = url_file.c_str();
    _fi.m_regetting = false;

	{
		KLUPD::Source _csd (string(url_base).c_str());
		_csd.m_useProxy = true;
		KLUPD::ProxyAuthorizationMethods _cpaml;

		m_upddata.proxy_url = ZHUZHNEV_PROXY;
		m_upddata.proxy_port = ZHUZHNEV_WO_AUTH_PORT;
		m_upddata.proxy_login = "";
		m_upddata.proxy_pwd = "";

		TD_BOOST_CHECK_MESSAGE (NFER_NoError == _cnc.getRemoteFile (_csd, _fi, m_upddata, _cpaml, NULL), "Getting files");
		TD_BOOST_CHECK_MESSAGE (helper::IsFoldersEqual (config_nc.difftool, config_nc.destination_etalon, config_nc.destination), "Checking destination folder");
	}

	//case 3
	std::cout<<"\nRequesting a simple file from http server with AUT_NTLM type\n";
	helper::RecreateDirectories (dirs);
	helper::CloneFile (string(config_nc.storage).append("\\base005.avc").c_str(), string(config_nc.destination_etalon).append("\\base005.avc").c_str(), false);
	
    _fi.m_remotePath = url_rel.c_str();
    _fi.m_remoteFilename = url_file.c_str();
    _fi.m_localPath = config_nc.destination.c_str();
    _fi.m_localFilename = url_file.c_str();
    _fi.m_regetting = false;

	{
		KLUPD::Source _csd (string(url_base).c_str());
		_csd.m_useProxy = true;
		KLUPD::ProxyAuthorizationMethods _cpaml;
		_cpaml.AddAuthMethod (KLUPD::AUT_Ntlm, false);

		m_upddata.proxy_url = AVP_PROXY;
		m_upddata.proxy_port = AVP_PROXY_PORT;
		m_upddata.proxy_login = "kl\\tester";
		m_upddata.proxy_pwd = "kf;0dsq Ntcnth";
		
		TD_BOOST_CHECK_MESSAGE (NFER_NoError == _cnc.getRemoteFile (_csd, _fi, m_upddata, _cpaml, NULL), "Getting files");
		TD_BOOST_CHECK_MESSAGE (helper::IsFoldersEqual (config_nc.difftool, config_nc.destination_etalon, config_nc.destination), "Checking destination folder");
	}

	//case 3a
	std::cout<<"\nRequesting a simple file from http server with AUT_NTLM_MANUAL type\n";
	helper::RecreateDirectories (dirs);
	helper::CloneFile (string(config_nc.storage).append("\\base005.avc").c_str(), string(config_nc.destination_etalon).append("\\base005.avc").c_str(), false);
	
    _fi.m_remotePath = url_rel.c_str();
    _fi.m_remoteFilename = url_file.c_str();
    _fi.m_localPath = config_nc.destination.c_str();
    _fi.m_localFilename = url_file.c_str();
    _fi.m_regetting = false;

	{
		KLUPD::Source _csd (string(url_base).c_str());
		_csd.m_useProxy = true;
		KLUPD::ProxyAuthorizationMethods _cpaml;
		_cpaml.AddAuthMethod (KLUPD::AUT_NtlmWithCredentials, false);

		m_upddata.proxy_url = AVP_PROXY;
		m_upddata.proxy_port = AVP_PROXY_PORT;
		m_upddata.proxy_login = "kl\\tester";
		m_upddata.proxy_pwd = "kf;0dsq Ntcnth";
		
		TD_BOOST_CHECK_MESSAGE (NFER_NoError == _cnc.getRemoteFile (_csd, _fi, m_upddata, _cpaml, NULL), "Getting files");
		TD_BOOST_CHECK_MESSAGE (helper::IsFoldersEqual (config_nc.difftool, config_nc.destination_etalon, config_nc.destination), "Checking destination folder");
	}

	//case 4
	std::cout<<"\nRequesting a simple file from http server with AUT_Basic type\n";
	helper::RecreateDirectories (dirs);
	helper::CloneFile (string(config_nc.storage).append("\\base005.avc").c_str(), string(config_nc.destination_etalon).append("\\base005.avc").c_str(), false);
	
    _fi.m_remotePath = url_rel.c_str();
    _fi.m_remoteFilename = url_file.c_str();
    _fi.m_localPath = config_nc.destination.c_str();
    _fi.m_localFilename = url_file.c_str();
    _fi.m_regetting = false;

	{
		KLUPD::Source _csd (string(url_base).c_str());
		_csd.m_useProxy = true;
		KLUPD::ProxyAuthorizationMethods _cpaml;
		_cpaml.AddAuthMethod (KLUPD::AUT_Basic, true);

		m_upddata.proxy_url = ZHUZHNEV_PROXY;
		m_upddata.proxy_port = ZHUZHNEV_BASIC_AUTH_PORT;
		m_upddata.proxy_login = "test";
		m_upddata.proxy_pwd = "test";

		TD_BOOST_CHECK_MESSAGE (NFER_NoError == _cnc.getRemoteFile (_csd, _fi, m_upddata, _cpaml, NULL), "Getting files");
		TD_BOOST_CHECK_MESSAGE (helper::IsFoldersEqual (config_nc.difftool, config_nc.destination_etalon, config_nc.destination), "Checking destination folder");
	}

	//case 5
	std::cout<<"\nRequesting a simple file from http server with AUT_NTLM type, when it is not supported\n";
	helper::RecreateDirectories (dirs);
	//helper::CloneFile (string(config_nc.storage).append("\\base005.avc").c_str(), string(config_nc.destination_etalon).append("\\base005.avc").c_str(), false);
	
    _fi.m_remotePath = url_rel.c_str();
    _fi.m_remoteFilename = url_file.c_str();
    _fi.m_localPath = config_nc.destination.c_str();
    _fi.m_localFilename = url_file.c_str();
    _fi.m_regetting = false;

	{
		KLUPD::Source _csd (string(url_base).c_str());
		_csd.m_useProxy = true;
		KLUPD::ProxyAuthorizationMethods _cpaml;
		_cpaml.AddAuthMethod (KLUPD::AUT_Ntlm, true);

		m_upddata.proxy_url = ZHUZHNEV_PROXY;
		m_upddata.proxy_port = ZHUZHNEV_BASIC_AUTH_PORT;
		m_upddata.proxy_login = "test";
		m_upddata.proxy_pwd = "test";

		TD_BOOST_CHECK_MESSAGE (NFER_ProxyAuthFailed == _cnc.getRemoteFile (_csd, _fi, m_upddata, _cpaml, NULL), "Getting files");
		TD_BOOST_CHECK_MESSAGE (helper::IsFoldersEqual (config_nc.difftool, config_nc.destination_etalon, config_nc.destination), "Checking destination folder");
	}

	//case 6
	std::cout<<"\nRequesting a simple file from http server with AUT_Basic type with wrong credentials\n";
	helper::RecreateDirectories (dirs);
	
    _fi.m_remotePath = url_rel.c_str();
    _fi.m_remoteFilename = url_file.c_str();
    _fi.m_localPath = config_nc.destination.c_str();
    _fi.m_localFilename = url_file.c_str();
    _fi.m_regetting = false;

	{
		KLUPD::Source _csd (string(url_base).c_str());
		_csd.m_useProxy = true;
		KLUPD::ProxyAuthorizationMethods _cpaml;
		_cpaml.AddAuthMethod (KLUPD::AUT_Basic, true);

		m_upddata.proxy_url = ZHUZHNEV_PROXY;
		m_upddata.proxy_port = ZHUZHNEV_BASIC_AUTH_PORT;
		m_upddata.proxy_login = "test1";
		m_upddata.proxy_pwd = "test1";

		TD_BOOST_CHECK_MESSAGE (NFER_ProxyAuthFailed == _cnc.getRemoteFile (_csd, _fi, m_upddata, _cpaml, NULL), "Getting files");
		TD_BOOST_CHECK_MESSAGE (helper::IsFoldersEqual (config_nc.difftool, config_nc.destination_etalon, config_nc.destination), "Checking destination folder");
	}	


	//case 7
	std::cout<<"\nRequesting a simple file from http server with AUT_NTLM, AUT_None and AUT_Basic, when the third is only supported\n";
	helper::RecreateDirectories (dirs);
	helper::CloneFile (string(config_nc.storage).append("\\base005.avc").c_str(), string(config_nc.destination_etalon).append("\\base005.avc").c_str(), false);
	
    _fi.m_remotePath = url_rel.c_str();
    _fi.m_remoteFilename = url_file.c_str();
    _fi.m_localPath = config_nc.destination.c_str();
    _fi.m_localFilename = url_file.c_str();
    _fi.m_regetting = false;

	{
		KLUPD::Source _csd (string(url_base).c_str());
		_csd.m_useProxy = true;
		KLUPD::ProxyAuthorizationMethods _cpaml;
		_cpaml.AddAuthMethod (KLUPD::AUT_None, false);
		//_cpaml.AddAuthMethod (KLUPD::AUT_Ntlm, true);
		_cpaml.AddAuthMethod (KLUPD::AUT_Basic, true);
		//_cpaml.AddAuthMethod (KLUPD::AUT_None, true);
		//ошибка проявляется в заависимости от порядка задания методов авторизации

		m_upddata.proxy_url = ZHUZHNEV_PROXY;
		m_upddata.proxy_port = ZHUZHNEV_BASIC_AUTH_PORT;
		m_upddata.proxy_login = "test";
		m_upddata.proxy_pwd = "test";

		TD_BOOST_CHECK_MESSAGE (NFER_ProxyAuthFailed == _cnc.getRemoteFile (_csd, _fi, m_upddata, _cpaml, NULL), "Getting files");
		TD_BOOST_CHECK_MESSAGE (helper::IsFoldersEqual (config_nc.difftool, config_nc.destination_etalon, config_nc.destination), "Checking destination folder");
	}
	
	Step ("TestUsingHttpProxy", reason);
};

void Test_CNetCore::TestDownloadAK ()
{
	string reason;
	vector<string> dirs;
	dirs.push_back (config_nc.destination);
	dirs.push_back (config_nc.destination_etalon);

	CNetFileInfo _fi;
	KLUPD::ProxyAuthorizationMethods _cpaml;
	NetCore _nc (&m_callback, &m_journal, &m_log);
	CNetCore _cnc (_nc, &m_log);

	string url_base = "ak://kiryukhin-xp";
	string url_rel = "/";
	string url_file = "base005.avc";
	
	string url_base2 = "ak://kiryukhin-xp";
	string url_rel2 = "/directory/";
	string url_file2 = "base006.avc";

	string url_base3 = "ak://kiryukhin-xp";
	string url_rel3 = "/directory/";
	string url_file3 = "base007.avc";

	helper::StartNetworkAgent ();


	//case 1
	std::cout<<"\nRequesting a simple file from adminkit server\n";
	helper::RecreateDirectories (dirs);
	helper::CloneFile (string(config_nc.storage).append("\\base005.avc").c_str(), string(config_nc.destination_etalon).append("\\base005.avc").c_str(), false);

    _fi.m_remotePath = url_rel.c_str();
    _fi.m_remoteFilename = url_file.c_str();
    _fi.m_localPath = config_nc.destination.c_str();
    _fi.m_localFilename = url_file.c_str();
    _fi.m_regetting = false;

	{
		KLUPD::Source _csd (string(url_base).c_str());

		TD_BOOST_CHECK_MESSAGE (NFER_NoError == _cnc.getRemoteFile (_csd, _fi, m_upddata, _cpaml, NULL), "Getting files");
		TD_BOOST_CHECK_MESSAGE (helper::IsFoldersEqual (config_nc.difftool, config_nc.destination_etalon, config_nc.destination), "Checking destination folder");
	}

	//case 2
	std::cout<<"\nRequesting a file (path with directories) from adminkit server\n";

	helper::RecreateDirectories (dirs);
	helper::CloneFile (string(config_nc.storage).append("\\base006.avc").c_str(), string(config_nc.destination_etalon).append("\\base006.avc").c_str(), false);

    _fi.m_remotePath = url_rel2.c_str();
    _fi.m_remoteFilename = url_file2.c_str();
    _fi.m_localPath = config_nc.destination.c_str();
    _fi.m_localFilename = url_file2.c_str();
    _fi.m_regetting = false;

	{
		KLUPD::Source _csd (string(url_base2).c_str());

		TD_BOOST_CHECK_MESSAGE (NFER_NoError == _cnc.getRemoteFile (_csd, _fi, m_upddata, _cpaml, NULL), "Getting files");
		TD_BOOST_CHECK_MESSAGE (helper::IsFoldersEqual (config_nc.difftool, config_nc.destination_etalon, config_nc.destination), "Checking destination folder");
	}

	//case 3
	std::cout<<"\nRequesting a non-existent file from adminkit server when the source file doesnt exist\n";

	helper::RecreateDirectories (dirs);
	
    _fi.m_remotePath = url_rel3.c_str();
    _fi.m_remoteFilename = url_file3.c_str();
    _fi.m_localPath = config_nc.destination.c_str();
    _fi.m_localFilename = url_file3.c_str();
    _fi.m_regetting = false;

	{
		KLUPD::Source _csd (string(url_base3).c_str());

		TD_BOOST_CHECK_MESSAGE (NFER_NoFile == _cnc.getRemoteFile (_csd, _fi, m_upddata, _cpaml, NULL), "Getting files");
		TD_BOOST_CHECK_MESSAGE (helper::IsFoldersEqual (config_nc.difftool, config_nc.destination_etalon, config_nc.destination), "Checking destination folder");
	}

	//case 4
	std::cout<<"\nRequesting a file from adminkit server when a part of the target file already exists\n";

	helper::RecreateDirectories (dirs);
	helper::CloneFile (string(config_nc.storage).append("\\base006.avc").c_str(), string(config_nc.destination_etalon).append("\\base006.avc").c_str(), false);
	helper::CloneFile (string(config_nc.storage).append("\\base006a.avc").c_str(), string(config_nc.destination).append("\\base006.avc").c_str(), false);
	
	
    _fi.m_remotePath = url_rel2.c_str();
    _fi.m_remoteFilename = url_file2.c_str();
    _fi.m_localPath = config_nc.destination.c_str();
    _fi.m_localFilename = url_file2.c_str();
    _fi.m_regetting = true;
	_fi.m_regetPosition = 100;

	{
		KLUPD::Source _csd (string(url_base2).c_str());

		TD_BOOST_CHECK_MESSAGE (NFER_NoError == _cnc.getRemoteFile (_csd, _fi, m_upddata, _cpaml, NULL), "Getting files");
		TD_BOOST_CHECK_MESSAGE (helper::IsFoldersEqual (config_nc.difftool, config_nc.destination_etalon, config_nc.destination), "Checking destination folder");
	}

	//case 5
	std::cout<<"\nRequesting a file from adminkit server when a part of the target file already exists, and its size exceeds the size of the target file\n";

	helper::RecreateDirectories (dirs);
	helper::CloneFile (string(config_nc.storage).append("\\base006a.avc").c_str(), string(config_nc.destination_etalon).append("\\base006.avc").c_str(), false);
	helper::CloneFile (string(config_nc.storage).append("\\base006a.avc").c_str(), string(config_nc.destination).append("\\base006.avc").c_str(), false);
	
	
    _fi.m_remotePath = url_rel2.c_str();
    _fi.m_remoteFilename = url_file2.c_str();
    _fi.m_localPath = config_nc.destination.c_str();
    _fi.m_localFilename = url_file2.c_str();
    _fi.m_regetting = true;
	_fi.m_regetPosition = 100000;

	{
		KLUPD::Source _csd (string(url_base2).c_str());

		TD_BOOST_CHECK_MESSAGE (NFER_NoError == _cnc.getRemoteFile (_csd, _fi, m_upddata, _cpaml, NULL), "Getting files");
		TD_BOOST_CHECK_MESSAGE (helper::IsFoldersEqual (config_nc.difftool, config_nc.destination_etalon, config_nc.destination), "Checking destination folder");
	}

	//case 6
	std::cout<<"\nRequesting a file from adminkit server when it cannot be written\n";

	helper::RecreateDirectories (dirs);
	helper::CloneFile (string(config_nc.storage).append("\\base006a.avc").c_str(), string(config_nc.destination_etalon).append("\\base006.avc").c_str(), false);
	helper::CloneFile (string(config_nc.storage).append("\\base006a.avc").c_str(), string(config_nc.destination).append("\\base006.avc").c_str(), false);

	_fi.m_remotePath = url_rel2.c_str();
    _fi.m_remoteFilename = url_file2.c_str();
    _fi.m_localPath = config_nc.destination.c_str();
    _fi.m_localFilename = url_file2.c_str();
    _fi.m_regetting = false;

	{
		KLUPD::Source _csd (string(url_base2).c_str());
		AutoFile hFile (string(config_nc.destination).append("\\base006.avc").c_str(), GENERIC_WRITE, FILE_SHARE_READ, OPEN_EXISTING);
		
		TD_BOOST_CHECK_MESSAGE (NFER_FileOperationFailed == _cnc.getRemoteFile (_csd, _fi, m_upddata, _cpaml, NULL), "Getting files");
		TD_BOOST_CHECK_MESSAGE (helper::IsFoldersEqual (config_nc.difftool, config_nc.destination_etalon, config_nc.destination), "Checking destination folder");
	}

	//case 7
	std::cout<<"\nRequesting a simple file from adminkit server after the failure\n";
	helper::RecreateDirectories (dirs);
	helper::CloneFile (string(config_nc.storage).append("\\base005.avc").c_str(), string(config_nc.destination_etalon).append("\\base005.avc").c_str(), false);

    _fi.m_remotePath = url_rel.c_str();
    _fi.m_remoteFilename = url_file.c_str();
    _fi.m_localPath = config_nc.destination.c_str();
    _fi.m_localFilename = url_file.c_str();
    _fi.m_regetting = false;

	{
		KLUPD::Source _csd (string(url_base).c_str());

		TD_BOOST_CHECK_MESSAGE (NFER_NoError == _cnc.getRemoteFile (_csd, _fi, m_upddata, _cpaml, NULL), "Getting files");
		TD_BOOST_CHECK_MESSAGE (helper::IsFoldersEqual (config_nc.difftool, config_nc.destination_etalon, config_nc.destination), "Checking destination folder");
	}


	//case 8
	std::cout<<"\nRequesting a file from adminkit server when network agent is disabled\n";

	helper::RecreateDirectories (dirs);

	_fi.m_remotePath = url_rel2.c_str();
    _fi.m_remoteFilename = url_file2.c_str();
    _fi.m_localPath = config_nc.destination.c_str();
    _fi.m_localFilename = url_file2.c_str();
    _fi.m_regetting = false;

	{
		KLUPD::Source _csd (string(url_base2).c_str());
		
		helper::StopNetworkAgent ();
		TD_BOOST_CHECK_MESSAGE (NFER_AK_ErrorInOperation == _cnc.getRemoteFile (_csd, _fi, m_upddata, _cpaml, NULL), "Getting files");
		TD_BOOST_CHECK_MESSAGE (helper::IsFoldersEqual (config_nc.difftool, config_nc.destination_etalon, config_nc.destination), "Checking destination folder");
		helper::StartNetworkAgent ();
	}

	//CNetCore _cnc1 (_nc, &m_log);
	//case 9
	std::cout<<"\nRequesting a simple file from adminkit server after the failure\n";
	helper::RecreateDirectories (dirs);
	helper::CloneFile (string(config_nc.storage).append("\\base005.avc").c_str(), string(config_nc.destination_etalon).append("\\base005.avc").c_str(), false);

    _fi.m_remotePath = url_rel.c_str();
    _fi.m_remoteFilename = url_file.c_str();
    _fi.m_localPath = config_nc.destination.c_str();
    _fi.m_localFilename = url_file.c_str();
    _fi.m_regetting = false;

	{
		KLUPD::Source _csd (string(url_base).c_str());

		TD_BOOST_CHECK_MESSAGE (NFER_NoError == _cnc.getRemoteFile (_csd, _fi, m_upddata, _cpaml, NULL), "Getting files");
		TD_BOOST_CHECK_MESSAGE (helper::IsFoldersEqual (config_nc.difftool, config_nc.destination_etalon, config_nc.destination), "Checking destination folder");
	}
	Step ("TestDownloadAK", reason);
};


Suite_CNetCore::Suite_CNetCore (const string& ini_file, const string& log_name)
{
	bool bLog;
	if (log_name == "") bLog =false; else bLog = true;

	boost::shared_ptr<Test_CNetCore> instance (new Test_CNetCore (ini_file, bLog, log_name));
	
	add (boost::unit_test::make_test_case<Test_CNetCore>(&Test_CNetCore::Start, "Test_CNetCore::Start", instance));
	add (BOOST_CLASS_TEST_CASE (&Test_CNetCore::TestDownloadHttp, instance));
	add (BOOST_CLASS_TEST_CASE (&Test_CNetCore::TestUsingHttpProxy, instance));
	add (BOOST_CLASS_TEST_CASE (&Test_CNetCore::TestDownloadAK, instance));
	add (boost::unit_test::make_test_case<Test_CNetCore>(&Test_CNetCore::End, "Test_CNetCore::End", instance));
	
	
	

}	