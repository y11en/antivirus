//check checkExpectedOnSourceDateAgainstLocalDate parameter

#include "Test_UpdateInfo.h"
#include "helper.h"

void Test_UpdateInfo::TestUpdateLocLang ()
{
	Ini_Updater _updater (NULL, &log, true);
	string reason;
	{
		//test 1
		std::cout<<"Test LocLang condition in xml file and product settings\n";

		UpdaterConfigurationData ucd;

		//Ini_ProductConfiguration& _prod_config = static_cast<Ini_ProductConfiguration&>(_updater.getProdConfig());
		Ini_ProductConfiguration& _prod_config = static_cast<Ini_ProductConfiguration&>(_updater.getProdConfig());
		Ini_UpdaterSettings& _upd_config = static_cast<Ini_UpdaterSettings&>(_updater.getUpdConfig());

		_prod_config.data.product_lang = "gondurasian";
		_prod_config.data.update_root_folder = "h:\\";

		UpdateInfo _ui (_prod_config, ucd, &log, &journal, &callback, UpdateInfo::withValidationWithFiltering, false);

		FileInfo_Unit fi_test;
		vector<FileInfo_Unit> fi_vector;
		vector<FileInfo_Unit> ind_vector;
		FileInfo fi1 ("upd_loclang.xml");

		ind_vector.push_back (FileInfo_Unit ("upd_loclang.xml"));

		// 1
		fi_test.m_type = FileInfo::base;
		fi_test.m_relativeURLPath = "bases/av/klava/wince/";
		fi_test.m_filename = "stream.id";
		fi_test.m_signature = "0XLSznpdI71fB300e7Uwj17mxVCUmn2y7ftLMiHayGYyOkQe7yDz/RJhHM";
		fi_test.m_size = 80;
		fi_test.m_dateFromIndex = "03042006 1751";
		fi_test.m_originalLocalPath = "%UpdateRoot%";
		fi_test.m_localPath = "/";
		fi_test.m_language.clear();
		fi_test.m_language.push_back ("russian");
		fi_test.m_language.push_back ("english");
		fi_test.m_language.push_back ("gondurasian");
		fi_vector.push_back (fi_test);
		//2
		fi_test.m_type = FileInfo::base;
		fi_test.m_relativeURLPath = "index/";
		fi_test.m_filename = "stream.kfb";
		fi_test.m_signature = "0XLSznpdI71fB300e7Uwj1NpFafWhvt5vcBIpNl/vu+nnNvXG4hJYL25hR";
		fi_test.m_size = 10185;
		fi_test.m_dateFromIndex = "24032006 1629";
		fi_test.m_originalLocalPath = "%UpdateRoot%";
		fi_test.m_localPath = "/";
		fi_test.m_language.clear();
		fi_test.m_language.push_back ("gondurasian");
		fi_vector.push_back (fi_test);
		//3
		fi_test.m_type = FileInfo::base;
		fi_test.m_relativeURLPath = "index/";
		fi_test.m_filename = "test.xml";
		fi_test.m_signature = "0XLSznpdI71fB300e7Uwj1NpFafWhvt5vcBIpNl/vu+nnNvXG4hJYL25hR";
		fi_test.m_size = 10185;
		fi_test.m_dateFromIndex = "24032006 1629";
		fi_test.m_originalLocalPath = "%UpdateRoot%";
		fi_test.m_localPath = "/";
		fi_test.m_language.clear();
		fi_vector.push_back (fi_test);

		//TD_BOOST_CHECK_MESSAGE (_ui.parse (updater, config_ui.xmlfolder, fi1, true, false, false), "parse failed");
		TD_BOOST_CHECK_MESSAGE (KLUPD::CORE_NO_ERROR == _ui.parse (config_ui.xmlfolder, fi1, UpdateInfo::noDateCheckForLocalFiles, true), "parse failed");
		TD_BOOST_CHECK_MESSAGE (fi_vector == _ui.m_matchFileList, "m_matchFileList comparison failed");
		//TD_BOOST_CHECK_MESSAGE (ind_vector == _ui.m_attachedIndex, "m_attachedIndex comparison failed");
	}
	{
		//test 2
		std::cout<<"Test missing LocLang condition in product settings\n";

		UpdaterConfigurationData ucd;

		Ini_ProductConfiguration& _prod_config = static_cast<Ini_ProductConfiguration&>(_updater.getProdConfig());
		Ini_UpdaterSettings& _upd_config = static_cast<Ini_UpdaterSettings&>(_updater.getUpdConfig());

		UpdateInfo _ui (_prod_config, ucd, &log, &journal, &callback, UpdateInfo::withValidationWithFiltering, false);

		FileInfo_Unit fi_test;
		vector<FileInfo_Unit> fi_vector;
		vector<FileInfo_Unit> ind_vector;
		FileInfo fi1 ("upd_loclang.xml");

		ind_vector.push_back (FileInfo_Unit ("upd_loclang.xml"));

		//1
		fi_test.m_type = FileInfo::base;
		fi_test.m_relativeURLPath = "index/";
		fi_test.m_filename = "test.xml";
		fi_test.m_signature = "0XLSznpdI71fB300e7Uwj1NpFafWhvt5vcBIpNl/vu+nnNvXG4hJYL25hR";
		fi_test.m_size = 10185;
		fi_test.m_dateFromIndex = "24032006 1629";
		fi_test.m_originalLocalPath = "%UpdateRoot%";
		fi_test.m_localPath = "/";
		fi_vector.push_back (fi_test);

//		TD_BOOST_CHECK_MESSAGE (_ui.parse (updater, config_ui.xmlfolder, fi1, true, false, false), "parse failed");
		TD_BOOST_CHECK_MESSAGE (KLUPD::CORE_NO_ERROR == _ui.parse (config_ui.xmlfolder, fi1, UpdateInfo::noDateCheckForLocalFiles, true), "parse failed");
		TD_BOOST_CHECK_MESSAGE (fi_vector == _ui.m_matchFileList, "m_matchFileList comparison failed");
		//TD_BOOST_CHECK_MESSAGE (ind_vector == _ui.m_attachedIndex, "m_attachedIndex comparison failed");
	}
	Step ("TestUpdateLocLang", reason);
};

void Test_UpdateInfo::TestUpdateAppID ()
{
	string reason;
	Ini_Updater _updater (NULL, &log, true);
	{
		//test 1
		std::cout<<"Test ApplicationID condition in xml file and product settings\n";

		UpdaterConfigurationData ucd;

		Ini_UpdaterSettings& _upd_config = static_cast<Ini_UpdaterSettings&>(_updater.getUpdConfig());
		Ini_ProductConfiguration& _prod_config = static_cast<Ini_ProductConfiguration&>(_updater.getProdConfig());
		_prod_config.m_updater.pID = "UnitTest";

		UpdateInfo _ui (_prod_config, ucd, &log, &journal, &callback, UpdateInfo::withValidationWithFiltering, false);

		FileInfo_Unit fi_test;
		vector<FileInfo_Unit> fi_vector;
		vector<FileInfo_Unit> ind_vector;
		FileInfo fi1 ("upd_appid.xml");

		ind_vector.push_back (FileInfo_Unit ("upd_appid.xml"));

		// 1
		fi_test.m_type = FileInfo::base;
		fi_test.m_relativeURLPath = "bases/av/klava/wince/";
		fi_test.m_filename = "stream.id";
		fi_test.m_signature = "0XLSznpdI71fB300e7Uwj17mxVCUmn2y7ftLMiHayGYyOkQe7yDz/RJhHM";
		fi_test.m_size = 80;
		fi_test.m_dateFromIndex = "03042006 1751";
		fi_test.m_originalLocalPath = "%UpdateRoot%";
		fi_test.m_localPath = "/";
		fi_test.m_applicationID = "UnitTest";
		fi_vector.push_back (fi_test);
		//2
		fi_test.m_type = FileInfo::base;
		fi_test.m_relativeURLPath = "index/";
		fi_test.m_filename = "updcfg.xml";
		fi_test.m_signature = "0XLSznpdI71fB300e7Uwj1NpFafWhvt5vcBIpNl/vu+nnNvXG4hJYL25hR";
		fi_test.m_size = 10185;
		fi_test.m_dateFromIndex = "24032006 1629";
		fi_test.m_originalLocalPath = "%UpdateRoot%";
		fi_test.m_localPath = "/";
		fi_test.m_applicationID = "";
		fi_vector.push_back (fi_test);

		TD_BOOST_CHECK_MESSAGE (KLUPD::CORE_NO_ERROR == _ui.parse (config_ui.xmlfolder, fi1, UpdateInfo::noDateCheckForLocalFiles, true), "parse failed");
		TD_BOOST_CHECK_MESSAGE (fi_vector == _ui.m_matchFileList, "m_matchFileList comparison failed");
		//TD_BOOST_CHECK_MESSAGE (ind_vector == _ui.m_attachedIndex, "m_attachedIndex comparison failed");
	}
	{
		//test 2
		std::cout<<"Test missing ApplicationID condition in product settings\n";

		UpdaterConfigurationData ucd;

		Ini_UpdaterSettings& _upd_config = static_cast<Ini_UpdaterSettings&>(_updater.getUpdConfig());
		Ini_ProductConfiguration& _prod_config = static_cast<Ini_ProductConfiguration&>(_updater.getProdConfig());
		_prod_config.m_updater.pID = "";

		UpdateInfo _ui (_prod_config, ucd, &log, &journal, &callback, UpdateInfo::withValidationWithFiltering, false);

		FileInfo_Unit fi_test;
		vector<FileInfo_Unit> fi_vector;
		vector<FileInfo_Unit> ind_vector;
		FileInfo fi1 ("upd_appid.xml");

		ind_vector.push_back (FileInfo_Unit ("upd_appid.xml"));

		//1
		fi_test.m_type = FileInfo::base;
		fi_test.m_relativeURLPath = "index/";
		fi_test.m_filename = "updcfg.xml";
		fi_test.m_signature = "0XLSznpdI71fB300e7Uwj1NpFafWhvt5vcBIpNl/vu+nnNvXG4hJYL25hR";
		fi_test.m_size = 10185;
		fi_test.m_dateFromIndex = "24032006 1629";
		fi_test.m_originalLocalPath = "%UpdateRoot%";
		fi_test.m_localPath = "/";
		fi_vector.push_back (fi_test);

		TD_BOOST_CHECK_MESSAGE (KLUPD::CORE_NO_ERROR == _ui.parse (config_ui.xmlfolder, fi1, UpdateInfo::noDateCheckForLocalFiles, true), "parse failed");
		TD_BOOST_CHECK_MESSAGE (fi_vector == _ui.m_matchFileList, "m_matchFileList comparison failed");
		//TD_BOOST_CHECK_MESSAGE (ind_vector == _ui.m_attachedIndex, "m_attachedIndex comparison failed");
	}
	Step ("TestUpdateAppID", reason);
};


void Test_UpdateInfo::TestUpdateFromAppVersion ()
{
	string reason;
	Ini_Updater _updater (NULL, &log, true);
	{
		//test 1
		std::cout<<"Test FromAppVersion condition in xml file and product settings\n";

		UpdaterConfigurationData ucd;

		Ini_UpdaterSettings& _upd_config = static_cast<Ini_UpdaterSettings&>(_updater.getUpdConfig());
		Ini_ProductConfiguration& _prod_config = static_cast<Ini_ProductConfiguration&>(_updater.getProdConfig());
		_prod_config.m_updater.pVer = "5.0.205";

		UpdateInfo _ui (_prod_config, ucd, &log, &journal, &callback, UpdateInfo::withValidationWithFiltering, false);

		FileInfo_Unit fi_test;
		vector<FileInfo_Unit> fi_vector;
		vector<FileInfo_Unit> ind_vector;
		FileInfo fi1 ("upd_fromapp.xml");

		ind_vector.push_back (FileInfo_Unit ("upd_fromapp.xml"));

		// 1
		fi_test.m_type = FileInfo::base;
		fi_test.m_relativeURLPath = "bases/av/klava/wince/";
		fi_test.m_filename = "stream.kfb";
		fi_test.m_signature = "0XLSznpdI71fB300e7Uwj1iKaHrkbhn/8gF4lPrfgw7XTZk2qiF/zr0RZ1";
		fi_test.m_size = 90016;
		fi_test.m_dateFromIndex = "03042006 1751";
		fi_test.m_originalLocalPath = "%UpdateRoot%";
		fi_test.m_localPath = "/";
		fi_vector.push_back (fi_test);
		// 2
		fi_test.m_type = FileInfo::base;
		fi_test.m_relativeURLPath = "index/";
		fi_test.m_filename = "updcfg.xml";
		fi_test.m_signature = "0XLSznpdI71fB300e7Uwj1NpFafWhvt5vcBIpNl/vu+nnNvXG4hJYL25hR";
		fi_test.m_size = 10185;
		fi_test.m_dateFromIndex = "24032006 1629";
		fi_test.m_originalLocalPath = "%UpdateRoot%";
		fi_test.m_localPath = "/";
		fi_test.m_fromAppVersion.clear();
		fi_test.m_fromAppVersion.push_back ("5.0.125");
		fi_test.m_fromAppVersion.push_back ("5.0.225");
		fi_vector.push_back (fi_test);
		// 3
		fi_test.m_type = FileInfo::base;
		fi_test.m_relativeURLPath = "index/";
		fi_test.m_filename = "updcfg2.xml";
		fi_test.m_signature = "0XLSznpdI71fB300e7Uwj1NpFafWhvt5vcBIpNl/vu+nnNvXG4hJYL25hR";
		fi_test.m_size = 10185;
		fi_test.m_dateFromIndex = "24032006 1629";
		fi_test.m_originalLocalPath = "%UpdateRoot%";
		fi_test.m_localPath = "/";
		fi_test.m_fromAppVersion.clear();
		fi_test.m_fromAppVersion.push_back ("5.0.205");
		fi_vector.push_back (fi_test);

		TD_BOOST_CHECK_MESSAGE (KLUPD::CORE_NO_ERROR == _ui.parse (config_ui.xmlfolder, fi1, UpdateInfo::noDateCheckForLocalFiles, true), "parse failed");
		TD_BOOST_CHECK_MESSAGE (fi_vector == _ui.m_matchFileList, "m_matchFileList comparison failed");
		//TD_BOOST_CHECK_MESSAGE (ind_vector == _ui.m_attachedIndex, "m_attachedIndex comparison failed");
	}
	{
		//test 2
		std::cout<<"Test missing FromAppVersion condition in product settings\n";

		UpdaterConfigurationData ucd;

		Ini_ProductConfiguration& _prod_config = static_cast<Ini_ProductConfiguration&>(_updater.getProdConfig());
		_prod_config.m_updater.pVer = "";

		UpdateInfo _ui (_prod_config, ucd, &log, &journal, &callback, UpdateInfo::withValidationWithFiltering, false);

		FileInfo_Unit fi_test;
		vector<FileInfo_Unit> fi_vector;
		vector<FileInfo_Unit> ind_vector;
		FileInfo fi1 ("upd_fromapp.xml");

		ind_vector.push_back (FileInfo_Unit ("upd_fromapp.xml"));

		// 1
		fi_test.m_type = FileInfo::base;
		fi_test.m_relativeURLPath = "bases/av/klava/wince/";
		fi_test.m_filename = "stream.id";
		fi_test.m_signature = "0XLSznpdI71fB300e7Uwj17mxVCUmn2y7ftLMiHayGYyOkQe7yDz/RJhHM";
		fi_test.m_size = 80;
		fi_test.m_dateFromIndex = "03042006 1751";
		fi_test.m_originalLocalPath = "%UpdateRoot%";
		fi_test.m_localPath = "/";
		fi_test.m_fromAppVersion.clear();
		fi_test.m_fromAppVersion.push_back ("5.0.202");
		fi_test.m_fromAppVersion.push_back ("5.0.203");
		fi_test.m_fromAppVersion.push_back ("5.0.204");
		fi_vector.push_back (fi_test);
		// 2
		fi_test.m_type = FileInfo::base;
		fi_test.m_relativeURLPath = "bases/av/klava/wince/";
		fi_test.m_filename = "stream.kfb";
		fi_test.m_signature = "0XLSznpdI71fB300e7Uwj1iKaHrkbhn/8gF4lPrfgw7XTZk2qiF/zr0RZ1";
		fi_test.m_size = 90016;
		fi_test.m_dateFromIndex = "03042006 1751";
		fi_test.m_originalLocalPath = "%UpdateRoot%";
		fi_test.m_localPath = "/";
		fi_test.m_fromAppVersion.clear();
		fi_vector.push_back (fi_test);
		// 3
		fi_test.m_type = FileInfo::base;
		fi_test.m_relativeURLPath = "index/";
		fi_test.m_filename = "updcfg.xml";
		fi_test.m_signature = "0XLSznpdI71fB300e7Uwj1NpFafWhvt5vcBIpNl/vu+nnNvXG4hJYL25hR";
		fi_test.m_size = 10185;
		fi_test.m_dateFromIndex = "24032006 1629";
		fi_test.m_originalLocalPath = "%UpdateRoot%";
		fi_test.m_localPath = "/";
		fi_test.m_fromAppVersion.clear();
		fi_test.m_fromAppVersion.push_back ("5.0.125");
		fi_test.m_fromAppVersion.push_back ("5.0.225");
		fi_vector.push_back (fi_test);
		// 4
		fi_test.m_type = FileInfo::base;
		fi_test.m_relativeURLPath = "index/";
		fi_test.m_filename = "updcfg2.xml";
		fi_test.m_signature = "0XLSznpdI71fB300e7Uwj1NpFafWhvt5vcBIpNl/vu+nnNvXG4hJYL25hR";
		fi_test.m_size = 10185;
		fi_test.m_dateFromIndex = "24032006 1629";
		fi_test.m_originalLocalPath = "%UpdateRoot%";
		fi_test.m_localPath = "/";
		fi_test.m_fromAppVersion.clear();
		fi_test.m_fromAppVersion.push_back ("5.0.205");
		fi_vector.push_back (fi_test);

		TD_BOOST_CHECK_MESSAGE (KLUPD::CORE_NO_ERROR == _ui.parse (config_ui.xmlfolder, fi1, UpdateInfo::noDateCheckForLocalFiles, true), "parse failed");
		TD_BOOST_CHECK_MESSAGE (fi_vector == _ui.m_matchFileList, "m_matchFileList comparison failed");
		//TD_BOOST_CHECK_MESSAGE (ind_vector == _ui.m_attachedIndex, "m_attachedIndex comparison failed");
	}
	Step ("TestUpdateFromAppVersion", reason);
};

void Test_UpdateInfo::TestUpdateComponents ()
{
	string reason;
	Ini_Updater _updater (NULL, &log, true);
	{
		//test 1
		std::cout<<"Test ComponentID and FromCompVersion condition in xml file and product settings\n";

		UpdaterConfigurationData ucd;

		Ini_UpdaterSettings& _upd_config = static_cast<Ini_UpdaterSettings&>(_updater.getUpdConfig());
		Ini_ProductConfiguration& _prod_config = static_cast<Ini_ProductConfiguration&>(_updater.getProdConfig());
		ProductConfigurationData::ProductComponent _pc;
		_pc.Name = "UnitTest";
		_pc.Version = "5.0.100";
		_prod_config.data.m_components.push_back (_pc);
		_pc.Name = "UnitTest1";
		_pc.Version = "6.0.100";
		_prod_config.data.m_components.push_back (_pc);

		UpdateInfo _ui (_prod_config, ucd, &log, &journal, &callback, UpdateInfo::withValidationWithFiltering, false);

		FileInfo_Unit fi_test;
		vector<FileInfo_Unit> fi_vector;
		vector<FileInfo_Unit> ind_vector;
		FileInfo fi1 ("upd_comps.xml");

		ind_vector.push_back (FileInfo_Unit ("upd_comps.xml"));
		//comp1
		{
			FileInfo_Unit fi_test;
			fi_test.m_componentIdSet.push_back ("UnitTest");
			fi_test.m_type = FileInfo::base;
			fi_test.m_relativeURLPath = "index/";
			fi_test.m_filename = "updcfg2.xml";
			fi_test.m_signature = "0XLSznpdI71fB300e7Uwj1NpFafWhvt5vcBIpNl/vu+nnNvXG4hJYL25hR";
			fi_test.m_size = 10185;
			fi_test.m_dateFromIndex = "24032006 1629";
			fi_test.m_originalLocalPath = "%UpdateRoot%";
			fi_test.m_localPath = "/";
			fi_test.m_fromCompVersion.push_back ("5.0.101");
			fi_vector.push_back(fi_test);
		};
		{
			FileInfo_Unit fi_test;
			fi_test.m_componentIdSet.push_back ("UnitTest");
			fi_test.m_type = FileInfo::base;
			fi_test.m_relativeURLPath = "index/";
			fi_test.m_filename = "updcfg3.xml";
			fi_test.m_signature = "0XLSznpdI71fB300e7Uwj1NpFafWhvt5vcBIpNl/vu+nnNvXG4hJYL25hR";
			fi_test.m_size = 10185;
			fi_test.m_dateFromIndex = "24032006 1629";
			fi_test.m_originalLocalPath = "%UpdateRoot%";
			fi_test.m_localPath = "/";
			fi_test.m_fromCompVersion.push_back ("5.0.100");
			fi_vector.push_back(fi_test);
		};
		//comp2
		{
			FileInfo_Unit fi_test;
			fi_test.m_componentIdSet.push_back ("UnitTest1");
			fi_test.m_type = FileInfo::base;
			fi_test.m_relativeURLPath = "index/";
			fi_test.m_filename = "updcfg5.xml";
			fi_test.m_signature = "0XLSznpdI71fB300e7Uwj1NpFafWhvt5vcBIpNl/vu+nnNvXG4hJYL25hR";
			fi_test.m_size = 10185;
			fi_test.m_dateFromIndex = "24032006 1629";
			fi_test.m_originalLocalPath = "%UpdateRoot%";
			fi_test.m_localPath = "/";
			fi_test.m_fromCompVersion.push_back ("6.0.97");
			fi_test.m_fromCompVersion.push_back ("6.0.101");
			fi_vector.push_back(fi_test);
		};
		{
			FileInfo_Unit fi_test;
			fi_test.m_componentIdSet.push_back ("UnitTest1");
			fi_test.m_type = FileInfo::base;
			fi_test.m_relativeURLPath = "index/";
			fi_test.m_filename = "updcfg6.xml";
			fi_test.m_signature = "0XLSznpdI71fB300e7Uwj1NpFafWhvt5vcBIpNl/vu+nnNvXG4hJYL25hR";
			fi_test.m_size = 10185;
			fi_test.m_dateFromIndex = "24032006 1629";
			fi_test.m_originalLocalPath = "%UpdateRoot%";
			fi_test.m_localPath = "/";
			fi_test.m_fromCompVersion.push_back ("6.0.100");
			fi_vector.push_back(fi_test);
		};
		//comp not defined
		{
			FileInfo_Unit fi_test;
			fi_test.m_type = FileInfo::base;
			fi_test.m_relativeURLPath = "index/";
			fi_test.m_filename = "updcfg10.xml";
			fi_test.m_signature = "0XLSznpdI71fB300e7Uwj1NpFafWhvt5vcBIpNl/vu+nnNvXG4hJYL25hR";
			fi_test.m_size = 10185;
			fi_test.m_dateFromIndex = "24032006 1629";
			fi_test.m_originalLocalPath = "%UpdateRoot%";
			fi_test.m_localPath = "/";
			fi_test.m_fromCompVersion.push_back ("5.0.98");
			fi_test.m_fromCompVersion.push_back ("5.0.99");
			fi_vector.push_back(fi_test);
		};
		{
			FileInfo_Unit fi_test;
			fi_test.m_type = FileInfo::base;
			fi_test.m_relativeURLPath = "index/";
			fi_test.m_filename = "updcfg11.xml";
			fi_test.m_signature = "0XLSznpdI71fB300e7Uwj1NpFafWhvt5vcBIpNl/vu+nnNvXG4hJYL25hR";
			fi_test.m_size = 10185;
			fi_test.m_dateFromIndex = "24032006 1629";
			fi_test.m_originalLocalPath = "%UpdateRoot%";
			fi_test.m_localPath = "/";
			fi_test.m_fromCompVersion.push_back ("5.0.101");
			fi_vector.push_back(fi_test);
		};
		{
			FileInfo_Unit fi_test;
			fi_test.m_type = FileInfo::base;
			fi_test.m_relativeURLPath = "index/";
			fi_test.m_filename = "updcfg12.xml";
			fi_test.m_signature = "0XLSznpdI71fB300e7Uwj1NpFafWhvt5vcBIpNl/vu+nnNvXG4hJYL25hR";
			fi_test.m_size = 10185;
			fi_test.m_dateFromIndex = "24032006 1629";
			fi_test.m_originalLocalPath = "%UpdateRoot%";
			fi_test.m_localPath = "/";
			fi_test.m_fromCompVersion.push_back ("5.0.100");
			fi_vector.push_back(fi_test);
		};

		TD_BOOST_CHECK_MESSAGE (KLUPD::CORE_NO_ERROR == _ui.parse (config_ui.xmlfolder, fi1, UpdateInfo::noDateCheckForLocalFiles, true), "parse failed");
		TD_BOOST_CHECK_MESSAGE (fi_vector == _ui.m_matchFileList, "m_matchFileList comparison failed");
		//TD_BOOST_CHECK_MESSAGE (ind_vector == _ui.m_attachedIndex, "m_attachedIndex comparison failed");
	}
	Step ("TestUpdateComponents", reason);
};

void Test_UpdateInfo::TestUpdateOSes ()
{
	Ini_Updater _updater (NULL, &log, true);
	string reason;
	{
		//test 1
		std::cout<<"Test OS Name and OS Version condition in xml file and product settings\n";

		UpdaterConfigurationData ucd;

		Ini_UpdaterSettings& _upd_config = static_cast<Ini_UpdaterSettings&>(_updater.getUpdConfig());
		Ini_ProductConfiguration& _prod_config = static_cast<Ini_ProductConfiguration&>(_updater.getProdConfig());
		_prod_config.data.os.os = "Windows";
		_prod_config.data.os.version = "5.1";

		UpdateInfo _ui (_prod_config, ucd, &log, &journal, &callback, UpdateInfo::withValidationWithFiltering, false);

		FileInfo_Unit fi_test;
		vector<FileInfo_Unit> fi_vector;
		vector<FileInfo_Unit> ind_vector;
		FileInfo fi1 ("upd_oses.xml");

		ind_vector.push_back (FileInfo_Unit ("upd_oses.xml"));

		{
			FileInfo_Unit fi_test;
			fi_test.m_type = FileInfo::base;
			fi_test.m_relativeURLPath = "index/";
			fi_test.m_filename = "updcfg.xml";
			fi_test.m_signature = "0XLSznpdI71fB300e7Uwj1NpFafWhvt5vcBIpNl/vu+nnNvXG4hJYL25hR";
			fi_test.m_size = 10185;
			fi_test.m_dateFromIndex = "24032006 1629";
			fi_test.m_originalLocalPath = "%UpdateRoot%";
			fi_test.m_localPath = "/";
			FileInfo::OS os;
			os.m_name = "Windows";
			os.m_versionSet.push_back ("5.0");
			os.m_versionSet.push_back ("5.1");
			fi_test.m_OSes.push_back (os);
			fi_vector.push_back(fi_test);
		};
		{
			FileInfo_Unit fi_test;
			fi_test.m_type = FileInfo::base;
			fi_test.m_relativeURLPath = "index/";
			fi_test.m_filename = "updcfg3.xml";
			fi_test.m_signature = "0XLSznpdI71fB300e7Uwj1NpFafWhvt5vcBIpNl/vu+nnNvXG4hJYL25hR";
			fi_test.m_size = 10185;
			fi_test.m_dateFromIndex = "24032006 1629";
			fi_test.m_originalLocalPath = "%UpdateRoot%";
			fi_test.m_localPath = "/";
			FileInfo::OS os;
			os.m_name = "Windows";
			os.m_versionSet.push_back ("5.1");
			fi_test.m_OSes.push_back (os);
			fi_vector.push_back(fi_test);
		};
		{
			FileInfo_Unit fi_test;
			fi_test.m_type = FileInfo::base;
			fi_test.m_relativeURLPath = "index/";
			fi_test.m_filename = "updcfg4.xml";
			fi_test.m_signature = "0XLSznpdI71fB300e7Uwj1NpFafWhvt5vcBIpNl/vu+nnNvXG4hJYL25hR";
			fi_test.m_size = 10185;
			fi_test.m_dateFromIndex = "24032006 1629";
			fi_test.m_originalLocalPath = "%UpdateRoot%";
			fi_test.m_localPath = "/";
			FileInfo::OS os;
			os.m_name = "Windows";
			fi_test.m_OSes.push_back (os);
			fi_vector.push_back(fi_test);
		};
		{
			FileInfo_Unit fi_test;
			fi_test.m_type = FileInfo::base;
			fi_test.m_relativeURLPath = "index/";
			fi_test.m_filename = "updcfg9.xml";
			fi_test.m_signature = "0XLSznpdI71fB300e7Uwj1NpFafWhvt5vcBIpNl/vu+nnNvXG4hJYL25hR";
			fi_test.m_size = 10185;
			fi_test.m_dateFromIndex = "24032006 1629";
			fi_test.m_originalLocalPath = "%UpdateRoot%";
			fi_test.m_localPath = "/";
			fi_vector.push_back(fi_test);
		};


		TD_BOOST_CHECK_MESSAGE (KLUPD::CORE_NO_ERROR == _ui.parse (config_ui.xmlfolder, fi1, UpdateInfo::noDateCheckForLocalFiles, true), "parse failed");
		TD_BOOST_CHECK_MESSAGE (fi_vector == _ui.m_matchFileList, "m_matchFileList comparison failed\n");
//		TD_BOOST_CHECK_MESSAGE (ind_vector == _ui.m_attachedIndex, "m_attachedIndex comparison failed\n");

	}
	{
		//test 2
		std::cout<<"Test missed OS Name and OS Version condition in xml file\n";

		UpdaterConfigurationData ucd;

		Ini_UpdaterSettings& _upd_config = static_cast<Ini_UpdaterSettings&>(_updater.getUpdConfig());
		Ini_ProductConfiguration& _prod_config = static_cast<Ini_ProductConfiguration&>(_updater.getProdConfig());

		UpdateInfo _ui (_prod_config, ucd, &log, &journal, &callback, UpdateInfo::withValidationWithFiltering, false);

		FileInfo_Unit fi_test;
		vector<FileInfo_Unit> fi_vector;
		vector<FileInfo_Unit> ind_vector;
		FileInfo fi1 ("upd_oses.xml");

		ind_vector.push_back (FileInfo_Unit ("upd_oses.xml"));

		{
			FileInfo_Unit fi_test;
			fi_test.m_type = FileInfo::base;
			fi_test.m_relativeURLPath = "index/";
			fi_test.m_filename = "updcfg9.xml";
			fi_test.m_signature = "0XLSznpdI71fB300e7Uwj1NpFafWhvt5vcBIpNl/vu+nnNvXG4hJYL25hR";
			fi_test.m_size = 10185;
			fi_test.m_dateFromIndex = "24032006 1629";
			fi_test.m_originalLocalPath = "%UpdateRoot%";
			fi_test.m_localPath = "/";
			fi_vector.push_back(fi_test);
		};


		TD_BOOST_CHECK_MESSAGE (KLUPD::CORE_NO_ERROR == _ui.parse (config_ui.xmlfolder, fi1, UpdateInfo::noDateCheckForLocalFiles, true), "parse failed");
		TD_BOOST_CHECK_MESSAGE (fi_vector == _ui.m_matchFileList, "m_matchFileList comparison failed");
		//TD_BOOST_CHECK_MESSAGE (ind_vector == _ui.m_attachedIndex, "m_attachedIndex comparison failed");

	}
	Step ("TestUpdateOSes", reason);
};

void Test_UpdateInfo::TestLocFilter ()
{

	std::cout<<"Test filtering file list by existence (update mode)\n";
	string reason;
	//test 1
	{
		std::cout<<"Full existence\n";

		Ini_Updater _updater (NULL, &log, true);
		UpdaterConfigurationData ucd;

		Ini_UpdaterSettings& _upd_config = static_cast<Ini_UpdaterSettings&>(_updater.getUpdConfig());
		Ini_ProductConfiguration& _prod_config = static_cast<Ini_ProductConfiguration&>(_updater.getProdConfig());

		_prod_config.data.os.os = "Windows";
		_prod_config.data.os.version = "5.1";
		_prod_config.data.update_root_folder = config_ui.destination;

		vector<FileInfo_Unit> fi_vector;
		vector<FileInfo_Unit> ind_vector;
		FileInfo fi1 ("check_existance.xml");

		ind_vector.push_back (FileInfo_Unit ("check_existance.xml"));
		_prod_config.data.os.os = "UnitOS";
		_prod_config.data.product_lang = "albanian";
		_prod_config.m_updater.pID = "444";
		_prod_config.m_updater.pVer = "333";
		_prod_config.data.substs.AddPair ("%TestFolder%", config_ui.destination);

		UpdateInfo _ui (_prod_config, ucd, &log, &journal, &callback, UpdateInfo::noValidationNoFiltering, false);

		vector<string> dirs;
		dirs.push_back (config_ui.destination);
		helper::RecreateDirectories (dirs);

		helper::CloneFile (string(config_ui.xmlfolder).append("\\updcfg1.xml").c_str(), string(config_ui.destination).append("\\updcfg1.xml").c_str(), false);
		helper::CloneFile (string(config_ui.xmlfolder).append("\\updcfg2.xml").c_str(), string(config_ui.destination).append("\\updcfg2.xml").c_str(), false);
		helper::CloneFile (string(config_ui.xmlfolder).append("\\updcfg3.xml").c_str(), string(config_ui.destination).append("\\updcfg3.xml").c_str(), false);
		helper::CloneFile (string(config_ui.xmlfolder).append("\\updcfg4.xml").c_str(), string(config_ui.destination).append("\\updcfg4.xml").c_str(), false);
		helper::CloneFile (string(config_ui.xmlfolder).append("\\updcfg5.xml").c_str(), string(config_ui.destination).append("\\updcfg5.xml").c_str(), false);

		{
			FileInfo_Unit fi_test;
			fi_test.m_type = FileInfo::base;
			fi_test.m_relativeURLPath = "index/";
			fi_test.m_filename = "updcfg1.xml";
			fi_test.m_signature = "0XLSznpdI71fB300e7Uwj1NpFafWhvt5vcBIpNl/vu+nnNvXG4hJYL25hR";
			fi_test.m_size = 10185;
			fi_test.m_dateFromIndex = "24032006 1629";
			fi_test.m_originalLocalPath = "%TestFolder%";
			fi_test.m_localPath = string(config_ui.destination).append("/");
			fi_test.m_language.push_back ("gondurasian");
			fi_vector.push_back(fi_test);
		};
		{
			FileInfo_Unit fi_test;
			fi_test.m_type = FileInfo::base;
			fi_test.m_relativeURLPath = "index/";
			fi_test.m_filename = "updcfg2.xml";
			fi_test.m_signature = "0XLSznpdI71fB300e7Uwj1NpFafWhvt5vcBIpNl/vu+nnNvXG4hJYL25hR";
			fi_test.m_size = 10185;
			fi_test.m_dateFromIndex = "24032006 1629";
			fi_test.m_originalLocalPath = "%TestFolder%";
			fi_test.m_localPath = string(config_ui.destination).append("/");
			fi_test.m_fromAppVersion.push_back ("5.0.200");
			fi_vector.push_back(fi_test);
		};
		{
			FileInfo_Unit fi_test;
			fi_test.m_type = FileInfo::base;
			fi_test.m_relativeURLPath = "index/";
			fi_test.m_filename = "updcfg3.xml";
			fi_test.m_signature = "0XLSznpdI71fB300e7Uwj1NpFafWhvt5vcBIpNl/vu+nnNvXG4hJYL25hR";
			fi_test.m_size = 10185;
			fi_test.m_dateFromIndex = "24032006 1629";
			fi_test.m_originalLocalPath = "%TestFolder%";
			fi_test.m_localPath = string(config_ui.destination).append("/");
			fi_test.m_componentIdSet.push_back ("Unknown");
			fi_vector.push_back(fi_test);
		};
		{
			FileInfo_Unit fi_test;
			fi_test.m_type = FileInfo::base;
			fi_test.m_relativeURLPath = "index/";
			fi_test.m_filename = "updcfg4.xml";
			fi_test.m_signature = "0XLSznpdI71fB300e7Uwj1NpFafWhvt5vcBIpNl/vu+nnNvXG4hJYL25hR";
			fi_test.m_size = 10185;
			fi_test.m_dateFromIndex = "24032006 1629";
			fi_test.m_originalLocalPath = "%TestFolder%";
			fi_test.m_localPath = string(config_ui.destination).append("/");
			fi_test.m_applicationID = "Unknown";
			fi_vector.push_back(fi_test);
		};
		{
			FileInfo_Unit fi_test;
			fi_test.m_type = FileInfo::base;
			fi_test.m_relativeURLPath = "index/";
			fi_test.m_filename = "updcfg5.xml";
			fi_test.m_signature = "0XLSznpdI71fB300e7Uwj1NpFafWhvt5vcBIpNl/vu+nnNvXG4hJYL25hR";
			fi_test.m_size = 10185;
			fi_test.m_dateFromIndex = "24032006 1629";
			fi_test.m_originalLocalPath = "%TestFolder%";
			fi_test.m_localPath = string(config_ui.destination).append("/");
			FileInfo::OS os;
			os.m_name = "Unknown";
			fi_test.m_OSes.push_back (os);
			fi_vector.push_back(fi_test);
		};

		TD_BOOST_CHECK_MESSAGE (KLUPD::CORE_NO_ERROR == _ui.parse (config_ui.xmlfolder, fi1, UpdateInfo::noDateCheckForLocalFiles, true), "parse failed");
		TD_BOOST_CHECK_MESSAGE (fi_vector == _ui.m_matchFileList, "m_matchFileList comparison failed");
		//TD_BOOST_CHECK_MESSAGE (ind_vector == _ui.m_attachedIndex, "m_attachedIndex comparison failed");

		}

		//test2
		{
		std::cout<<"Full inexistence\n";

		Ini_Updater _updater (NULL, &log, true);
		UpdaterConfigurationData ucd;

		Ini_UpdaterSettings& _upd_config = static_cast<Ini_UpdaterSettings&>(_updater.getUpdConfig());
		Ini_ProductConfiguration& _prod_config = static_cast<Ini_ProductConfiguration&>(_updater.getProdConfig());

		_prod_config.data.os.os = "Windows";
		_prod_config.data.os.version = "5.1";
		_prod_config.data.update_root_folder = config_ui.destination;

		vector<FileInfo_Unit> fi_vector;
		vector<FileInfo_Unit> ind_vector;
		FileInfo fi1 ("check_existance.xml");

		ind_vector.push_back (FileInfo_Unit ("check_existance.xml"));
		_prod_config.data.os.os = "UnitOS";
		_prod_config.data.product_lang = "albanian";
		_prod_config.m_updater.pID = "444";
		_prod_config.m_updater.pVer = "333";
		_prod_config.data.substs.AddPair ("%TestFolder%", config_ui.destination);

		UpdateInfo _ui (_prod_config, ucd, &log, &journal, &callback, UpdateInfo::noValidationNoFiltering, false);

		vector<string> dirs;
		dirs.push_back (config_ui.destination);
		helper::RecreateDirectories (dirs);


		TD_BOOST_CHECK_MESSAGE (KLUPD::CORE_NO_ERROR == _ui.parse (config_ui.xmlfolder, fi1, UpdateInfo::noDateCheckForLocalFiles, true), "parse failed");
		TD_BOOST_CHECK_MESSAGE (fi_vector == _ui.m_matchFileList, "m_matchFileList comparison failed");
		//TD_BOOST_CHECK_MESSAGE (ind_vector == _ui.m_attachedIndex, "m_attachedIndex comparison failed");

		}
		//test 3
		{
		std::cout<<"Partial existence\n";

		Ini_Updater _updater (NULL, &log, true);
		UpdaterConfigurationData ucd;

		Ini_UpdaterSettings& _upd_config = static_cast<Ini_UpdaterSettings&>(_updater.getUpdConfig());
		Ini_ProductConfiguration& _prod_config = static_cast<Ini_ProductConfiguration&>(_updater.getProdConfig());

		_prod_config.data.os.os = "Windows";
		_prod_config.data.os.version = "5.1";
		_prod_config.data.update_root_folder = config_ui.destination;

		UpdateInfo _ui (_prod_config, ucd, &log, &journal, &callback, UpdateInfo::noValidationNoFiltering, false);

		vector<FileInfo_Unit> fi_vector;
		vector<FileInfo_Unit> ind_vector;
		FileInfo fi1 ("check_existance.xml");

		ind_vector.push_back (FileInfo_Unit ("check_existance.xml"));
		_prod_config.data.os.os = "UnitOS";
		_prod_config.data.product_lang = "albanian";
		_prod_config.m_updater.pID = "444";
		_prod_config.m_updater.pVer = "333";
		_prod_config.data.substs.AddPair ("%TestFolder%", config_ui.destination);

		vector<string> dirs;
		dirs.push_back (config_ui.destination);
		helper::RecreateDirectories (dirs);

		helper::CloneFile (string(config_ui.xmlfolder).append("\\updcfg1.xml").c_str(), string(config_ui.destination).append("\\updcfg1.xml").c_str(), false);
		helper::CloneFile (string(config_ui.xmlfolder).append("\\updcfg3.xml").c_str(), string(config_ui.destination).append("\\updcfg3.xml").c_str(), false);
		helper::CloneFile (string(config_ui.xmlfolder).append("\\updcfg5.xml").c_str(), string(config_ui.destination).append("\\updcfg5.xml").c_str(), false);

		{
			FileInfo_Unit fi_test;
			fi_test.m_type = FileInfo::base;
			fi_test.m_relativeURLPath = "index/";
			fi_test.m_filename = "updcfg1.xml";
			fi_test.m_signature = "0XLSznpdI71fB300e7Uwj1NpFafWhvt5vcBIpNl/vu+nnNvXG4hJYL25hR";
			fi_test.m_size = 10185;
			fi_test.m_dateFromIndex = "24032006 1629";
			fi_test.m_originalLocalPath = "%TestFolder%";
			fi_test.m_localPath = string(config_ui.destination).append("/");
			fi_test.m_language.push_back ("gondurasian");
			fi_vector.push_back(fi_test);
		};
		{
			FileInfo_Unit fi_test;
			fi_test.m_type = FileInfo::base;
			fi_test.m_relativeURLPath = "index/";
			fi_test.m_filename = "updcfg3.xml";
			fi_test.m_signature = "0XLSznpdI71fB300e7Uwj1NpFafWhvt5vcBIpNl/vu+nnNvXG4hJYL25hR";
			fi_test.m_size = 10185;
			fi_test.m_dateFromIndex = "24032006 1629";
			fi_test.m_originalLocalPath = "%TestFolder%";
			fi_test.m_localPath = string(config_ui.destination).append("/");
			fi_test.m_componentIdSet.push_back("Unknown");
			fi_vector.push_back(fi_test);
		};
		{
			FileInfo_Unit fi_test;
			fi_test.m_type = FileInfo::base;
			fi_test.m_relativeURLPath = "index/";
			fi_test.m_filename = "updcfg5.xml";
			fi_test.m_signature = "0XLSznpdI71fB300e7Uwj1NpFafWhvt5vcBIpNl/vu+nnNvXG4hJYL25hR";
			fi_test.m_size = 10185;
			fi_test.m_dateFromIndex = "24032006 1629";
			fi_test.m_originalLocalPath = "%TestFolder%";
			fi_test.m_localPath = string(config_ui.destination).append("/");
			FileInfo::OS os;
			os.m_name = "Unknown";
			fi_test.m_OSes.push_back (os);
			fi_vector.push_back(fi_test);
		};

		TD_BOOST_CHECK_MESSAGE (KLUPD::CORE_NO_ERROR == _ui.parse (config_ui.xmlfolder, fi1, UpdateInfo::noDateCheckForLocalFiles, true), "parse failed");
		TD_BOOST_CHECK_MESSAGE (fi_vector == _ui.m_matchFileList, "m_matchFileList comparison failed");
		//TD_BOOST_CHECK_MESSAGE (ind_vector == _ui.m_attachedIndex, "m_attachedIndex comparison failed");
	}
	Step ("TestLocFilter", reason);
	
};


void Test_UpdateInfo::TestLocFilterRetr ()
{

	std::cout<<"Test filtering file list by existence (retranslation mode)\n";
	string reason;
	//test 1
	{
		std::cout<<"Full existence\n";

		Ini_Updater _updater (NULL, &log, true);
		UpdaterConfigurationData ucd;

		Ini_ProductConfiguration& _prod_config = static_cast<Ini_ProductConfiguration&>(_updater.getProdConfig());
		Ini_UpdaterSettings& _upd_config = static_cast<Ini_UpdaterSettings&>(_updater.getUpdConfig());

		_prod_config.data.os.os = "Windows";
		_prod_config.data.os.version = "5.1";
		_upd_config.data.retranslation_dir = config_ui.destination;

		vector<FileInfo_Unit> fi_vector;
		vector<FileInfo_Unit> ind_vector;
		FileInfo fi1 ("check_existance.xml");

		ind_vector.push_back (FileInfo_Unit ("check_existance.xml"));
		_prod_config.data.os.os = "UnitOS";
		_prod_config.data.product_lang = "albanian";
		_prod_config.m_updater.pID = "444";
		_prod_config.m_updater.pVer = "333";
		_prod_config.data.substs.AddPair ("%TestFolder%", config_ui.destination);

		UpdateInfo _ui (_prod_config, ucd, &log, &journal, &callback, UpdateInfo::noValidationNoFiltering, true);

		vector<string> dirs;
		dirs.push_back (config_ui.destination);
		dirs.push_back (string(config_ui.destination).append("index/"));
		helper::RecreateDirectories (dirs);

		helper::CloneFile (string(config_ui.xmlfolder).append("\\updcfg1.xml").c_str(), string(config_ui.destination).append("index\\updcfg1.xml").c_str(), false);
		helper::CloneFile (string(config_ui.xmlfolder).append("\\updcfg2.xml").c_str(), string(config_ui.destination).append("index\\updcfg2.xml").c_str(), false);
		helper::CloneFile (string(config_ui.xmlfolder).append("\\updcfg3.xml").c_str(), string(config_ui.destination).append("index\\updcfg3.xml").c_str(), false);
		helper::CloneFile (string(config_ui.xmlfolder).append("\\updcfg4.xml").c_str(), string(config_ui.destination).append("index\\updcfg4.xml").c_str(), false);
		helper::CloneFile (string(config_ui.xmlfolder).append("\\updcfg5.xml").c_str(), string(config_ui.destination).append("index\\updcfg5.xml").c_str(), false);

		{
			FileInfo_Unit fi_test;
			fi_test.m_type = FileInfo::base;
			fi_test.m_relativeURLPath = "index/";
			fi_test.m_filename = "updcfg1.xml";
			fi_test.m_signature = "0XLSznpdI71fB300e7Uwj1NpFafWhvt5vcBIpNl/vu+nnNvXG4hJYL25hR";
			fi_test.m_size = 10185;
			fi_test.m_dateFromIndex = "24032006 1629";
			fi_test.m_originalLocalPath = "%TestFolder%";
			fi_test.m_localPath = string(config_ui.destination).append("/");
			fi_test.m_language.push_back ("gondurasian");
			fi_vector.push_back(fi_test);
		};
		{
			FileInfo_Unit fi_test;
			fi_test.m_type = FileInfo::base;
			fi_test.m_relativeURLPath = "index/";
			fi_test.m_filename = "updcfg2.xml";
			fi_test.m_signature = "0XLSznpdI71fB300e7Uwj1NpFafWhvt5vcBIpNl/vu+nnNvXG4hJYL25hR";
			fi_test.m_size = 10185;
			fi_test.m_dateFromIndex = "24032006 1629";
			fi_test.m_originalLocalPath = "%TestFolder%";
			fi_test.m_localPath = string(config_ui.destination).append("/");
			fi_test.m_fromAppVersion.push_back ("5.0.200");
			fi_vector.push_back(fi_test);
		};
		{
			FileInfo_Unit fi_test;
			fi_test.m_type = FileInfo::base;
			fi_test.m_relativeURLPath = "index/";
			fi_test.m_filename = "updcfg3.xml";
			fi_test.m_signature = "0XLSznpdI71fB300e7Uwj1NpFafWhvt5vcBIpNl/vu+nnNvXG4hJYL25hR";
			fi_test.m_size = 10185;
			fi_test.m_dateFromIndex = "24032006 1629";
			fi_test.m_originalLocalPath = "%TestFolder%";
			fi_test.m_localPath = string(config_ui.destination).append("/");
			fi_test.m_componentIdSet.push_back ("Unknown");
			fi_vector.push_back(fi_test);
		};
		{
			FileInfo_Unit fi_test;
			fi_test.m_type = FileInfo::base;
			fi_test.m_relativeURLPath = "index/";
			fi_test.m_filename = "updcfg4.xml";
			fi_test.m_signature = "0XLSznpdI71fB300e7Uwj1NpFafWhvt5vcBIpNl/vu+nnNvXG4hJYL25hR";
			fi_test.m_size = 10185;
			fi_test.m_dateFromIndex = "24032006 1629";
			fi_test.m_originalLocalPath = "%TestFolder%";
			fi_test.m_localPath = string(config_ui.destination).append("/");
			fi_test.m_applicationID = "Unknown";
			fi_vector.push_back(fi_test);
		};
		{
			FileInfo_Unit fi_test;
			fi_test.m_type = FileInfo::base;
			fi_test.m_relativeURLPath = "index/";
			fi_test.m_filename = "updcfg5.xml";
			fi_test.m_signature = "0XLSznpdI71fB300e7Uwj1NpFafWhvt5vcBIpNl/vu+nnNvXG4hJYL25hR";
			fi_test.m_size = 10185;
			fi_test.m_dateFromIndex = "24032006 1629";
			fi_test.m_originalLocalPath = "%TestFolder%";
			fi_test.m_localPath = string(config_ui.destination).append("/");
			FileInfo::OS os;
			os.m_name = "Unknown";
			fi_test.m_OSes.push_back (os);
			fi_vector.push_back(fi_test);
		};

		TD_BOOST_CHECK_MESSAGE (KLUPD::CORE_NO_ERROR == _ui.parse (config_ui.xmlfolder, fi1, UpdateInfo::noDateCheckForLocalFiles, true), "parse failed");
		TD_BOOST_CHECK_MESSAGE (fi_vector == _ui.m_matchFileList, "m_matchFileList comparison failed");
		//TD_BOOST_CHECK_MESSAGE (ind_vector == _ui.m_attachedIndex, "m_attachedIndex comparison failed");

		}

	//test 2
	{
		std::cout<<"Full inexistence\n";

		Ini_Updater _updater (NULL, &log, true);
		UpdaterConfigurationData ucd;

		Ini_ProductConfiguration& _prod_config = static_cast<Ini_ProductConfiguration&>(_updater.getProdConfig());
		Ini_UpdaterSettings& _upd_config = static_cast<Ini_UpdaterSettings&>(_updater.getUpdConfig());

		_prod_config.data.os.os = "Windows";
		_prod_config.data.os.version = "5.1";
		_upd_config.data.retranslation_dir = config_ui.destination;

		vector<FileInfo_Unit> fi_vector;
		vector<FileInfo_Unit> ind_vector;
		FileInfo fi1 ("check_existance.xml");

		ind_vector.push_back (FileInfo_Unit ("check_existance.xml"));
		_prod_config.data.os.os = "UnitOS";
		_prod_config.data.product_lang = "albanian";
		_prod_config.m_updater.pID = "444";
		_prod_config.m_updater.pVer = "333";
		_prod_config.data.substs.AddPair ("%TestFolder%", config_ui.destination);

		UpdateInfo _ui (_prod_config, ucd, &log, &journal, &callback, UpdateInfo::noValidationNoFiltering, true);

		vector<string> dirs;
		dirs.push_back (config_ui.destination);
		dirs.push_back (string(config_ui.destination).append("index/"));
		helper::RecreateDirectories (dirs);

		TD_BOOST_CHECK_MESSAGE (KLUPD::CORE_NO_ERROR == _ui.parse (config_ui.xmlfolder, fi1, UpdateInfo::noDateCheckForLocalFiles, true), "parse failed");
		TD_BOOST_CHECK_MESSAGE (fi_vector == _ui.m_matchFileList, "m_matchFileList comparison failed");
		//TD_BOOST_CHECK_MESSAGE (ind_vector == _ui.m_attachedIndex, "m_attachedIndex comparison failed");

	}
	//test 3
	{
		std::cout<<"Partial inexistence\n";

		Ini_Updater _updater (NULL, &log, true);
		UpdaterConfigurationData ucd;

		Ini_ProductConfiguration& _prod_config = static_cast<Ini_ProductConfiguration&>(_updater.getProdConfig());
		Ini_UpdaterSettings& _upd_config = static_cast<Ini_UpdaterSettings&>(_updater.getUpdConfig());

		_prod_config.data.os.os = "Windows";
		_prod_config.data.os.version = "5.1";
		_upd_config.data.retranslation_dir = config_ui.destination;

		vector<FileInfo_Unit> fi_vector;
		vector<FileInfo_Unit> ind_vector;
		FileInfo fi1 ("check_existance.xml");

		ind_vector.push_back (FileInfo_Unit ("check_existance.xml"));
		_prod_config.data.os.os = "UnitOS";
		_prod_config.data.product_lang = "albanian";
		_prod_config.m_updater.pID = "444";
		_prod_config.m_updater.pVer = "333";
		_prod_config.data.substs.AddPair ("%TestFolder%", config_ui.destination);

		UpdateInfo _ui (_prod_config, ucd, &log, &journal, &callback, UpdateInfo::noValidationNoFiltering, true);

		vector<string> dirs;
		dirs.push_back (config_ui.destination);
		dirs.push_back (string(config_ui.destination).append("index/"));
		helper::RecreateDirectories (dirs);

		helper::CloneFile (string(config_ui.xmlfolder).append("\\updcfg1.xml").c_str(), string(config_ui.destination).append("index\\updcfg1.xml").c_str(), false);
		helper::CloneFile (string(config_ui.xmlfolder).append("\\updcfg3.xml").c_str(), string(config_ui.destination).append("index\\updcfg3.xml").c_str(), false);
		helper::CloneFile (string(config_ui.xmlfolder).append("\\updcfg5.xml").c_str(), string(config_ui.destination).append("index\\updcfg5.xml").c_str(), false);

		{
			FileInfo_Unit fi_test;
			fi_test.m_type = FileInfo::base;
			fi_test.m_relativeURLPath = "index/";
			fi_test.m_filename = "updcfg1.xml";
			fi_test.m_signature = "0XLSznpdI71fB300e7Uwj1NpFafWhvt5vcBIpNl/vu+nnNvXG4hJYL25hR";
			fi_test.m_size = 10185;
			fi_test.m_dateFromIndex = "24032006 1629";
			fi_test.m_originalLocalPath = "%TestFolder%";
			fi_test.m_localPath = string(config_ui.destination).append("/");
			fi_test.m_language.push_back ("gondurasian");
			fi_vector.push_back(fi_test);
		};
		{
			FileInfo_Unit fi_test;
			fi_test.m_type = FileInfo::base;
			fi_test.m_relativeURLPath = "index/";
			fi_test.m_filename = "updcfg3.xml";
			fi_test.m_signature = "0XLSznpdI71fB300e7Uwj1NpFafWhvt5vcBIpNl/vu+nnNvXG4hJYL25hR";
			fi_test.m_size = 10185;
			fi_test.m_dateFromIndex = "24032006 1629";
			fi_test.m_originalLocalPath = "%TestFolder%";
			fi_test.m_localPath = string(config_ui.destination).append("/");
			fi_test.m_componentIdSet.push_back ("Unknown");
			fi_vector.push_back(fi_test);
		};
		{
			FileInfo_Unit fi_test;
			fi_test.m_type = FileInfo::base;
			fi_test.m_relativeURLPath = "index/";
			fi_test.m_filename = "updcfg5.xml";
			fi_test.m_signature = "0XLSznpdI71fB300e7Uwj1NpFafWhvt5vcBIpNl/vu+nnNvXG4hJYL25hR";
			fi_test.m_size = 10185;
			fi_test.m_dateFromIndex = "24032006 1629";
			fi_test.m_originalLocalPath = "%TestFolder%";
			fi_test.m_localPath = string(config_ui.destination).append("/");
			FileInfo::OS os;
			os.m_name = "Unknown";
			fi_test.m_OSes.push_back (os);
			fi_vector.push_back(fi_test);
		};

		TD_BOOST_CHECK_MESSAGE (KLUPD::CORE_NO_ERROR == _ui.parse (config_ui.xmlfolder, fi1, UpdateInfo::noDateCheckForLocalFiles, true), "parse failed");
		TD_BOOST_CHECK_MESSAGE (fi_vector == _ui.m_matchFileList, "m_matchFileList comparison failed");
		//TD_BOOST_CHECK_MESSAGE (ind_vector == _ui.m_attachedIndex, "m_attachedIndex comparison failed");

	}
	Step ("TestLocFilterRetr", reason);

}

void Test_UpdateInfo::TestEntryRequirements ()
{
	string reason;
	std::cout<<"Test entry requirements\n";
	std::cout<<"Test files filtering (update mode)\n";

	//test 1
	{
		std::cout<<"Test update_bases flag\n";

		Ini_Updater _updater (NULL, &log, true);
		UpdaterConfigurationData ucd;

		Ini_ProductConfiguration& _prod_config = static_cast<Ini_ProductConfiguration&>(_updater.getProdConfig());
		Ini_UpdaterSettings& _upd_config = static_cast<Ini_UpdaterSettings&>(_updater.getUpdConfig());

		ucd.UpdaterListFlags.apply_urgent_updates = false;
		ucd.UpdaterListFlags.automatically_apply_available_updates = false;
		ucd.UpdaterListFlags.update_bases = true;

		UpdateInfo _ui (_prod_config, ucd, &log, &journal, &callback, UpdateInfo::withValidationWithFiltering, false);

		vector<FileInfo_Unit> fi_vector;
		vector<FileInfo_Unit> ind_vector;
		FileInfo fi1 ("check_required.xml");

		ind_vector.push_back (FileInfo_Unit ("check_required.xml"));

		{
			FileInfo_Unit fi_test;
			fi_test.m_type = FileInfo::blackList;
			fi_test.m_obligatoryEntry = true;
			fi_test.m_relativeURLPath = "index/";
			fi_test.m_filename = "updcfg11.xml";
			fi_test.m_signature = "0XLSznpdI71fB300e7Uwj1NpFafWhvt5vcBIpNl/vu+nnNvXG4hJYL25hR";
			fi_test.m_size = 10185;
			fi_test.m_dateFromIndex = "24032006 1629";
			fi_test.m_localPath = "/";
			fi_test.m_originalLocalPath = "%UpdateRoot%";
			fi_vector.push_back(fi_test);
		};
		{
			FileInfo_Unit fi_test;
			fi_test.m_type = FileInfo::base;
			fi_test.m_obligatoryEntry = true;
			fi_test.m_relativeURLPath = "index/";
			fi_test.m_filename = "updcfg8.xml";
			fi_test.m_signature = "0XLSznpdI71fB300e7Uwj1NpFafWhvt5vcBIpNl/vu+nnNvXG4hJYL25hR";
			fi_test.m_size = 10185;
			fi_test.m_dateFromIndex = "24032006 1629";
			fi_test.m_localPath = "/";
			fi_test.m_originalLocalPath = "%UpdateRoot%";
			fi_vector.push_back(fi_test);
		};
		{
			FileInfo_Unit fi_test;
			fi_test.m_type = FileInfo::index;
			fi_test.m_obligatoryEntry = true;
			fi_test.m_relativeURLPath = "index/";
			fi_test.m_filename = "updcfg10.xml";
			fi_test.m_signature = "0XLSznpdI71fB300e7Uwj1NpFafWhvt5vcBIpNl/vu+nnNvXG4hJYL25hR";
			fi_test.m_size = 10185;
			fi_test.m_dateFromIndex = "24032006 1629";
			fi_test.m_localPath = "/";
			fi_test.m_originalLocalPath = "%UpdateRoot%";
			fi_vector.push_back(fi_test);
		};

		TD_BOOST_CHECK_MESSAGE (KLUPD::CORE_NO_ERROR == _ui.parse (config_ui.xmlfolder, fi1, UpdateInfo::noDateCheckForLocalFiles, true), "parse failed");
		TD_BOOST_CHECK_MESSAGE (fi_vector == _ui.m_matchFileList, "m_matchFileList comparison failed");
		//TD_BOOST_CHECK_MESSAGE (ind_vector == _ui.m_attachedIndex, "m_attachedIndex comparison failed");

	}
	//test 2
	{
		std::cout<<"Test apply_urgent_updates flag\n";

		Ini_Updater _updater (NULL, &log, true);
		UpdaterConfigurationData ucd;

		Ini_ProductConfiguration& _prod_config = static_cast<Ini_ProductConfiguration&>(_updater.getProdConfig());
		Ini_UpdaterSettings& _upd_config = static_cast<Ini_UpdaterSettings&>(_updater.getUpdConfig());

		ucd.UpdaterListFlags.apply_urgent_updates = true;
		ucd.UpdaterListFlags.automatically_apply_available_updates = false;
		ucd.UpdaterListFlags.update_bases = false;

		UpdateInfo _ui (_prod_config, ucd, &log, &journal, &callback, UpdateInfo::withValidationWithFiltering, false);

		vector<FileInfo_Unit> fi_vector;
		vector<FileInfo_Unit> ind_vector;
		FileInfo fi1 ("check_required.xml");

		ind_vector.push_back (FileInfo_Unit ("check_required.xml"));

		{
			FileInfo_Unit fi_test;
			fi_test.m_type = FileInfo::code;
			fi_test.m_obligatoryEntry = true;
			fi_test.m_relativeURLPath = "index/";
			fi_test.m_filename = "updcfg7.xml";
			fi_test.m_signature = "0XLSznpdI71fB300e7Uwj1NpFafWhvt5vcBIpNl/vu+nnNvXG4hJYL25hR";
			fi_test.m_size = 10185;
			fi_test.m_dateFromIndex = "24032006 1629";
			fi_test.m_localPath = "/";
			fi_test.m_originalLocalPath = "%UpdateRoot%";
			fi_vector.push_back(fi_test);
		};
		{
			FileInfo_Unit fi_test;
			fi_test.m_type = FileInfo::patch;
			fi_test.m_obligatoryEntry = true;
			fi_test.m_relativeURLPath = "index/";
			fi_test.m_filename = "updcfg9.xml";
			fi_test.m_signature = "0XLSznpdI71fB300e7Uwj1NpFafWhvt5vcBIpNl/vu+nnNvXG4hJYL25hR";
			fi_test.m_size = 10185;
			fi_test.m_dateFromIndex = "24032006 1629";
			fi_test.m_localPath = "/";
			fi_test.m_originalLocalPath = "%UpdateRoot%";
			fi_vector.push_back(fi_test);
		};
		{
			FileInfo_Unit fi_test;
			fi_test.m_type = FileInfo::index;
			fi_test.m_obligatoryEntry = true;
			fi_test.m_relativeURLPath = "index/";
			fi_test.m_filename = "updcfg10.xml";
			fi_test.m_signature = "0XLSznpdI71fB300e7Uwj1NpFafWhvt5vcBIpNl/vu+nnNvXG4hJYL25hR";
			fi_test.m_size = 10185;
			fi_test.m_dateFromIndex = "24032006 1629";
			fi_test.m_localPath = "/";
			fi_test.m_originalLocalPath = "%UpdateRoot%";
			fi_vector.push_back(fi_test);
		};

		TD_BOOST_CHECK_MESSAGE (KLUPD::CORE_NO_ERROR == _ui.parse (config_ui.xmlfolder, fi1, UpdateInfo::noDateCheckForLocalFiles, true), "parse failed");
		TD_BOOST_CHECK_MESSAGE (fi_vector == _ui.m_matchFileList, "m_matchFileList comparison failed");
		//TD_BOOST_CHECK_MESSAGE (ind_vector == _ui.m_attachedIndex, "m_attachedIndex comparison failed");

	}
	//test 3
	{
		std::cout<<"Test automatically_apply_available_updates flag\n";

		Ini_Updater _updater (NULL, &log, true);
		UpdaterConfigurationData ucd;

		Ini_ProductConfiguration& _prod_config = static_cast<Ini_ProductConfiguration&>(_updater.getProdConfig());
		Ini_UpdaterSettings& _upd_config = static_cast<Ini_UpdaterSettings&>(_updater.getUpdConfig());

		ucd.UpdaterListFlags.apply_urgent_updates = false;
		ucd.UpdaterListFlags.automatically_apply_available_updates = true;
		ucd.UpdaterListFlags.update_bases = false;

		UpdateInfo _ui (_prod_config, ucd, &log, &journal, &callback, UpdateInfo::withValidationWithFiltering, false);

		vector<FileInfo_Unit> fi_vector;
		vector<FileInfo_Unit> ind_vector;
		FileInfo fi1 ("check_required.xml");

		ind_vector.push_back (FileInfo_Unit ("check_required.xml"));

		{
			FileInfo_Unit fi_test;
			fi_test.m_type = FileInfo::code;
			fi_test.m_obligatoryEntry = false;
			fi_test.m_relativeURLPath = "index/";
			fi_test.m_filename = "updcfg1.xml";
			fi_test.m_signature = "0XLSznpdI71fB300e7Uwj1NpFafWhvt5vcBIpNl/vu+nnNvXG4hJYL25hR";
			fi_test.m_size = 10185;
			fi_test.m_dateFromIndex = "24032006 1629";
			fi_test.m_localPath = "/";
			fi_test.m_originalLocalPath = "%UpdateRoot%";
			fi_vector.push_back(fi_test);
		};
		{
			FileInfo_Unit fi_test;
			fi_test.m_type = FileInfo::patch;
			fi_test.m_obligatoryEntry = false;
			fi_test.m_relativeURLPath = "index/";
			fi_test.m_filename = "updcfg3.xml";
			fi_test.m_signature = "0XLSznpdI71fB300e7Uwj1NpFafWhvt5vcBIpNl/vu+nnNvXG4hJYL25hR";
			fi_test.m_size = 10185;
			fi_test.m_dateFromIndex = "24032006 1629";
			fi_test.m_localPath = "/";
			fi_test.m_originalLocalPath = "%UpdateRoot%";
			fi_vector.push_back(fi_test);
		};
		{
			FileInfo_Unit fi_test;
			fi_test.m_type = FileInfo::index;
			fi_test.m_obligatoryEntry = false;
			fi_test.m_relativeURLPath = "index/";
			fi_test.m_filename = "updcfg4.xml";
			fi_test.m_signature = "0XLSznpdI71fB300e7Uwj1NpFafWhvt5vcBIpNl/vu+nnNvXG4hJYL25hR";
			fi_test.m_size = 10185;
			fi_test.m_dateFromIndex = "24032006 1629";
			fi_test.m_localPath = "/";
			fi_test.m_originalLocalPath = "%UpdateRoot%";
			fi_vector.push_back(fi_test);
		};
		{
			FileInfo_Unit fi_test;
			fi_test.m_type = FileInfo::code;
			fi_test.m_obligatoryEntry = true;
			fi_test.m_relativeURLPath = "index/";
			fi_test.m_filename = "updcfg7.xml";
			fi_test.m_signature = "0XLSznpdI71fB300e7Uwj1NpFafWhvt5vcBIpNl/vu+nnNvXG4hJYL25hR";
			fi_test.m_size = 10185;
			fi_test.m_dateFromIndex = "24032006 1629";
			fi_test.m_localPath = "/";
			fi_test.m_originalLocalPath = "%UpdateRoot%";
			fi_vector.push_back(fi_test);
		};
		{
			FileInfo_Unit fi_test;
			fi_test.m_type = FileInfo::patch;
			fi_test.m_obligatoryEntry = true;
			fi_test.m_relativeURLPath = "index/";
			fi_test.m_filename = "updcfg9.xml";
			fi_test.m_signature = "0XLSznpdI71fB300e7Uwj1NpFafWhvt5vcBIpNl/vu+nnNvXG4hJYL25hR";
			fi_test.m_size = 10185;
			fi_test.m_dateFromIndex = "24032006 1629";
			fi_test.m_localPath = "/";
			fi_test.m_originalLocalPath = "%UpdateRoot%";
			fi_vector.push_back(fi_test);
		};
		{
			FileInfo_Unit fi_test;
			fi_test.m_type = FileInfo::index;
			fi_test.m_obligatoryEntry = true;
			fi_test.m_relativeURLPath = "index/";
			fi_test.m_filename = "updcfg10.xml";
			fi_test.m_signature = "0XLSznpdI71fB300e7Uwj1NpFafWhvt5vcBIpNl/vu+nnNvXG4hJYL25hR";
			fi_test.m_size = 10185;
			fi_test.m_dateFromIndex = "24032006 1629";
			fi_test.m_localPath = "/";
			fi_test.m_originalLocalPath = "%UpdateRoot%";
			fi_vector.push_back(fi_test);
		};

		TD_BOOST_CHECK_MESSAGE (KLUPD::CORE_NO_ERROR == _ui.parse (config_ui.xmlfolder, fi1, UpdateInfo::noDateCheckForLocalFiles, true), "parse failed");
		TD_BOOST_CHECK_MESSAGE (fi_vector == _ui.m_matchFileList, "m_matchFileList comparison failed");
		//TD_BOOST_CHECK_MESSAGE (ind_vector == _ui.m_attachedIndex, "m_attachedIndex comparison failed");

	}

	std::cout<<"Test files filtering (retranslation mode)\n";

	//test 4
	{
		std::cout<<"Test update_bases flag\n";

		Ini_Updater _updater (NULL, &log, true);
		UpdaterConfigurationData ucd;

		Ini_ProductConfiguration& _prod_config = static_cast<Ini_ProductConfiguration&>(_updater.getProdConfig());
		Ini_UpdaterSettings& _upd_config = static_cast<Ini_UpdaterSettings&>(_updater.getUpdConfig());

		ucd.RetranslationListFlags.apply_urgent_updates = false;
		ucd.RetranslationListFlags.automatically_apply_available_updates = false;
		ucd.RetranslationListFlags.update_bases = true;

		UpdateInfo _ui (_prod_config, ucd, &log, &journal, &callback, UpdateInfo::withValidationWithFiltering, true);

		vector<FileInfo_Unit> fi_vector;
		vector<FileInfo_Unit> ind_vector;
		FileInfo fi1 ("check_required.xml");

		ind_vector.push_back (FileInfo_Unit ("check_required.xml"));

		{
			FileInfo_Unit fi_test;
			fi_test.m_type = FileInfo::blackList;
			fi_test.m_obligatoryEntry = true;
			fi_test.m_relativeURLPath = "index/";
			fi_test.m_filename = "updcfg11.xml";
			fi_test.m_signature = "0XLSznpdI71fB300e7Uwj1NpFafWhvt5vcBIpNl/vu+nnNvXG4hJYL25hR";
			fi_test.m_size = 10185;
			fi_test.m_dateFromIndex = "24032006 1629";
			fi_test.m_localPath = "/";
			fi_test.m_originalLocalPath = "%UpdateRoot%";
			fi_vector.push_back(fi_test);
		};
		{
			FileInfo_Unit fi_test;
			fi_test.m_type = FileInfo::base;
			fi_test.m_obligatoryEntry = true;
			fi_test.m_relativeURLPath = "index/";
			fi_test.m_filename = "updcfg8.xml";
			fi_test.m_signature = "0XLSznpdI71fB300e7Uwj1NpFafWhvt5vcBIpNl/vu+nnNvXG4hJYL25hR";
			fi_test.m_size = 10185;
			fi_test.m_dateFromIndex = "24032006 1629";
			fi_test.m_localPath = "/";
			fi_test.m_originalLocalPath = "%UpdateRoot%";
			fi_vector.push_back(fi_test);
		};
		{
			FileInfo_Unit fi_test;
			fi_test.m_type = FileInfo::index;
			fi_test.m_obligatoryEntry = true;
			fi_test.m_relativeURLPath = "index/";
			fi_test.m_filename = "updcfg10.xml";
			fi_test.m_signature = "0XLSznpdI71fB300e7Uwj1NpFafWhvt5vcBIpNl/vu+nnNvXG4hJYL25hR";
			fi_test.m_size = 10185;
			fi_test.m_dateFromIndex = "24032006 1629";
			fi_test.m_localPath = "/";
			fi_test.m_originalLocalPath = "%UpdateRoot%";
			fi_vector.push_back(fi_test);
		};

		TD_BOOST_CHECK_MESSAGE (KLUPD::CORE_NO_ERROR == _ui.parse (config_ui.xmlfolder, fi1, UpdateInfo::noDateCheckForLocalFiles, true), "parse failed");
		TD_BOOST_CHECK_MESSAGE (fi_vector == _ui.m_matchFileList, "m_matchFileList comparison failed");
		//TD_BOOST_CHECK_MESSAGE (ind_vector == _ui.m_attachedIndex, "m_attachedIndex comparison failed");

	}
	//test 5
	{
		std::cout<<"Test apply_urgent_updates flag\n";

		Ini_Updater _updater (NULL, &log, true);
		UpdaterConfigurationData ucd;

		Ini_ProductConfiguration& _prod_config = static_cast<Ini_ProductConfiguration&>(_updater.getProdConfig());
		Ini_UpdaterSettings& _upd_config = static_cast<Ini_UpdaterSettings&>(_updater.getUpdConfig());

		ucd.RetranslationListFlags.apply_urgent_updates = true;
		ucd.RetranslationListFlags.automatically_apply_available_updates = false;
		ucd.RetranslationListFlags.update_bases = false;

		UpdateInfo _ui (_prod_config, ucd, &log, &journal, &callback, UpdateInfo::withValidationWithFiltering, true);

		vector<FileInfo_Unit> fi_vector;
		vector<FileInfo_Unit> ind_vector;
		FileInfo fi1 ("check_required.xml");

		ind_vector.push_back (FileInfo_Unit ("check_required.xml"));

		{
			FileInfo_Unit fi_test;
			fi_test.m_type = FileInfo::code;
			fi_test.m_obligatoryEntry = true;
			fi_test.m_relativeURLPath = "index/";
			fi_test.m_filename = "updcfg7.xml";
			fi_test.m_signature = "0XLSznpdI71fB300e7Uwj1NpFafWhvt5vcBIpNl/vu+nnNvXG4hJYL25hR";
			fi_test.m_size = 10185;
			fi_test.m_dateFromIndex = "24032006 1629";
			fi_test.m_localPath = "/";
			fi_test.m_originalLocalPath = "%UpdateRoot%";
			fi_vector.push_back(fi_test);
		};
		{
			FileInfo_Unit fi_test;
			fi_test.m_type = FileInfo::patch;
			fi_test.m_obligatoryEntry = true;
			fi_test.m_relativeURLPath = "index/";
			fi_test.m_filename = "updcfg9.xml";
			fi_test.m_signature = "0XLSznpdI71fB300e7Uwj1NpFafWhvt5vcBIpNl/vu+nnNvXG4hJYL25hR";
			fi_test.m_size = 10185;
			fi_test.m_dateFromIndex = "24032006 1629";
			fi_test.m_localPath = "/";
			fi_test.m_originalLocalPath = "%UpdateRoot%";
			fi_vector.push_back(fi_test);
		};
		{
			FileInfo_Unit fi_test;
			fi_test.m_type = FileInfo::index;
			fi_test.m_obligatoryEntry = true;
			fi_test.m_relativeURLPath = "index/";
			fi_test.m_filename = "updcfg10.xml";
			fi_test.m_signature = "0XLSznpdI71fB300e7Uwj1NpFafWhvt5vcBIpNl/vu+nnNvXG4hJYL25hR";
			fi_test.m_size = 10185;
			fi_test.m_dateFromIndex = "24032006 1629";
			fi_test.m_localPath = "/";
			fi_test.m_originalLocalPath = "%UpdateRoot%";
			fi_vector.push_back(fi_test);
		};

		TD_BOOST_CHECK_MESSAGE (KLUPD::CORE_NO_ERROR == _ui.parse (config_ui.xmlfolder, fi1, UpdateInfo::noDateCheckForLocalFiles, true), "parse failed");
		TD_BOOST_CHECK_MESSAGE (fi_vector == _ui.m_matchFileList, "m_matchFileList comparison failed");
		//TD_BOOST_CHECK_MESSAGE (ind_vector == _ui.m_attachedIndex, "m_attachedIndex comparison failed");

	}
	//test 6
	{
		std::cout<<"Test automatically_apply_available_updates flag\n";

		Ini_Updater _updater (NULL, &log, true);
		UpdaterConfigurationData ucd;

		Ini_ProductConfiguration& _prod_config = static_cast<Ini_ProductConfiguration&>(_updater.getProdConfig());
		Ini_UpdaterSettings& _upd_config = static_cast<Ini_UpdaterSettings&>(_updater.getUpdConfig());

		ucd.RetranslationListFlags.apply_urgent_updates = false;
		ucd.RetranslationListFlags.automatically_apply_available_updates = true;
		ucd.RetranslationListFlags.update_bases = false;

		UpdateInfo _ui (_prod_config, ucd, &log, &journal, &callback, UpdateInfo::withValidationWithFiltering, true);

		vector<FileInfo_Unit> fi_vector;
		vector<FileInfo_Unit> ind_vector;
		FileInfo fi1 ("check_required.xml");

		ind_vector.push_back (FileInfo_Unit ("check_required.xml"));

		{
			FileInfo_Unit fi_test;
			fi_test.m_type = FileInfo::code;
			fi_test.m_obligatoryEntry = false;
			fi_test.m_relativeURLPath = "index/";
			fi_test.m_filename = "updcfg1.xml";
			fi_test.m_signature = "0XLSznpdI71fB300e7Uwj1NpFafWhvt5vcBIpNl/vu+nnNvXG4hJYL25hR";
			fi_test.m_size = 10185;
			fi_test.m_dateFromIndex = "24032006 1629";
			fi_test.m_localPath = "/";
			fi_test.m_originalLocalPath = "%UpdateRoot%";
			fi_vector.push_back(fi_test);
		};
		{
			FileInfo_Unit fi_test;
			fi_test.m_type = FileInfo::patch;
			fi_test.m_obligatoryEntry = false;
			fi_test.m_relativeURLPath = "index/";
			fi_test.m_filename = "updcfg3.xml";
			fi_test.m_signature = "0XLSznpdI71fB300e7Uwj1NpFafWhvt5vcBIpNl/vu+nnNvXG4hJYL25hR";
			fi_test.m_size = 10185;
			fi_test.m_dateFromIndex = "24032006 1629";
			fi_test.m_localPath = "/";
			fi_test.m_originalLocalPath = "%UpdateRoot%";
			fi_vector.push_back(fi_test);
		};
		{
			FileInfo_Unit fi_test;
			fi_test.m_type = FileInfo::index;
			fi_test.m_obligatoryEntry = false;
			fi_test.m_relativeURLPath = "index/";
			fi_test.m_filename = "updcfg4.xml";
			fi_test.m_signature = "0XLSznpdI71fB300e7Uwj1NpFafWhvt5vcBIpNl/vu+nnNvXG4hJYL25hR";
			fi_test.m_size = 10185;
			fi_test.m_dateFromIndex = "24032006 1629";
			fi_test.m_localPath = "/";
			fi_test.m_originalLocalPath = "%UpdateRoot%";
			fi_vector.push_back(fi_test);
		};
		{
			FileInfo_Unit fi_test;
			fi_test.m_type = FileInfo::code;
			fi_test.m_obligatoryEntry = true;
			fi_test.m_relativeURLPath = "index/";
			fi_test.m_filename = "updcfg7.xml";
			fi_test.m_signature = "0XLSznpdI71fB300e7Uwj1NpFafWhvt5vcBIpNl/vu+nnNvXG4hJYL25hR";
			fi_test.m_size = 10185;
			fi_test.m_dateFromIndex = "24032006 1629";
			fi_test.m_localPath = "/";
			fi_test.m_originalLocalPath = "%UpdateRoot%";
			fi_vector.push_back(fi_test);
		};
		{
			FileInfo_Unit fi_test;
			fi_test.m_type = FileInfo::patch;
			fi_test.m_obligatoryEntry = true;
			fi_test.m_relativeURLPath = "index/";
			fi_test.m_filename = "updcfg9.xml";
			fi_test.m_signature = "0XLSznpdI71fB300e7Uwj1NpFafWhvt5vcBIpNl/vu+nnNvXG4hJYL25hR";
			fi_test.m_size = 10185;
			fi_test.m_dateFromIndex = "24032006 1629";
			fi_test.m_localPath = "/";
			fi_test.m_originalLocalPath = "%UpdateRoot%";
			fi_vector.push_back(fi_test);
		};
		{
			FileInfo_Unit fi_test;
			fi_test.m_type = FileInfo::index;
			fi_test.m_obligatoryEntry = true;
			fi_test.m_relativeURLPath = "index/";
			fi_test.m_filename = "updcfg10.xml";
			fi_test.m_signature = "0XLSznpdI71fB300e7Uwj1NpFafWhvt5vcBIpNl/vu+nnNvXG4hJYL25hR";
			fi_test.m_size = 10185;
			fi_test.m_dateFromIndex = "24032006 1629";
			fi_test.m_localPath = "/";
			fi_test.m_originalLocalPath = "%UpdateRoot%";
			fi_vector.push_back(fi_test);
		};

		TD_BOOST_CHECK_MESSAGE (KLUPD::CORE_NO_ERROR == _ui.parse (config_ui.xmlfolder, fi1, UpdateInfo::noDateCheckForLocalFiles, true), "parse failed");
		TD_BOOST_CHECK_MESSAGE (fi_vector == _ui.m_matchFileList, "m_matchFileList comparison failed");
		//TD_BOOST_CHECK_MESSAGE (ind_vector == _ui.m_attachedIndex, "m_attachedIndex comparison failed");
	}
	Step ("TestEntryRequirements", reason);
};


void Test_UpdateInfo::TestBlackWhiteListsApp ()
{
	string reason;
	{
		//test 1
		std::cout<<"Test application whitelist \n";

		Ini_Updater _updater (NULL, &log, true);
		UpdaterConfigurationData ucd;

		Ini_ProductConfiguration& _prod_config = static_cast<Ini_ProductConfiguration&>(_updater.getProdConfig());
		Ini_UpdaterSettings& _upd_config = static_cast<Ini_UpdaterSettings&>(_updater.getUpdConfig());

		ucd.retranslate_only_listed_apps = true;
		ucd.RetranslatedObjects.Applications_WhiteListMode = true;
		{
			vector<STRING> vct;
			vct.push_back ("5.0");
			vct.push_back ("5.1");
			ucd.RetranslatedObjects.Applications.AddPair ("Application", vct);
		}
		ucd.RetranslatedObjects.Applications.AddPair ("Application1", vector<STRING>());
		{
			vector<STRING> vct;
			vct.push_back ("22.0");
			ucd.RetranslatedObjects.Applications.AddPair ("Application3", vct);
		}

		UpdateInfo _ui (_prod_config, ucd, &log, &journal, &callback, UpdateInfo::withValidationWithFiltering, true);

		FileInfo_Unit fi_test;
		vector<FileInfo_Unit> fi_vector;
		vector<FileInfo_Unit> ind_vector;
		FileInfo fi1 ("check_appwlists.xml");

		ind_vector.push_back (FileInfo_Unit ("check_appwlists.xml"));

		{
			FileInfo_Unit fi_test;
			fi_test.m_applicationID = "Application";
			fi_test.m_type = FileInfo::base;
			fi_test.m_filename = "updcfg1.xml";
			fi_test.m_signature = "0XLSznpdI71fB300e7Uwj1NpFafWhvt5vcBIpNl/vu+nnNvXG4hJYL25hR";
			fi_test.m_size = 10185;
			fi_test.m_dateFromIndex = "24032006 1629";
			fi_test.m_relativeURLPath = "/";
			fi_test.m_localPath = "/";
			fi_test.m_originalLocalPath = "%UpdateRoot%";
			fi_vector.push_back(fi_test);
		};
		{
			FileInfo_Unit fi_test;
			fi_test.m_applicationID = "Application";
			fi_test.m_type = FileInfo::base;
			fi_test.m_filename = "updcfg2.xml";
			fi_test.m_signature = "0XLSznpdI71fB300e7Uwj1NpFafWhvt5vcBIpNl/vu+nnNvXG4hJYL25hR";
			fi_test.m_size = 10185;
			fi_test.m_dateFromIndex = "24032006 1629";
			fi_test.m_relativeURLPath = "/";
			fi_test.m_localPath = "/";
			fi_test.m_originalLocalPath = "%UpdateRoot%";
			fi_test.m_fromAppVersion.push_back ("11.0");
			fi_test.m_fromAppVersion.push_back ("12.0");
			fi_vector.push_back(fi_test);
		};
		{
			FileInfo_Unit fi_test;
			fi_test.m_applicationID = "Application";
			fi_test.m_type = FileInfo::base;
			fi_test.m_filename = "updcfg4.xml";
			fi_test.m_signature = "0XLSznpdI71fB300e7Uwj1NpFafWhvt5vcBIpNl/vu+nnNvXG4hJYL25hR";
			fi_test.m_size = 10185;
			fi_test.m_dateFromIndex = "24032006 1629";
			fi_test.m_relativeURLPath = "/";
			fi_test.m_localPath = "/";
			fi_test.m_originalLocalPath = "%UpdateRoot%";
			fi_test.m_fromAppVersion.push_back ("5.5");
			fi_vector.push_back(fi_test);
		};
		{
			FileInfo_Unit fi_test;
			fi_test.m_applicationID = "Application1";
			fi_test.m_type = FileInfo::base;
			fi_test.m_filename = "updcfg5.xml";
			fi_test.m_signature = "0XLSznpdI71fB300e7Uwj1NpFafWhvt5vcBIpNl/vu+nnNvXG4hJYL25hR";
			fi_test.m_size = 10185;
			fi_test.m_dateFromIndex = "24032006 1629";
			fi_test.m_relativeURLPath = "/";
			fi_test.m_localPath = "/";
			fi_test.m_originalLocalPath = "%UpdateRoot%";
			fi_test.m_fromAppVersion.push_back ("10.0");
			fi_vector.push_back(fi_test);
		};
		{
			FileInfo_Unit fi_test;
			fi_test.m_applicationID = "Application3";
			fi_test.m_type = FileInfo::base;
			fi_test.m_filename = "updcfg7.xml";
			fi_test.m_signature = "0XLSznpdI71fB300e7Uwj1NpFafWhvt5vcBIpNl/vu+nnNvXG4hJYL25hR";
			fi_test.m_size = 10185;
			fi_test.m_dateFromIndex = "24032006 1629";
			fi_test.m_relativeURLPath = "/";
			fi_test.m_localPath = "/";
			fi_test.m_originalLocalPath = "%UpdateRoot%";
			fi_vector.push_back(fi_test);
		};
		{
			FileInfo_Unit fi_test;
			fi_test.m_type = FileInfo::base;
			fi_test.m_filename = "updcfg8.xml";
			fi_test.m_signature = "0XLSznpdI71fB300e7Uwj1NpFafWhvt5vcBIpNl/vu+nnNvXG4hJYL25hR";
			fi_test.m_size = 10185;
			fi_test.m_dateFromIndex = "24032006 1629";
			fi_test.m_relativeURLPath = "/";
			fi_test.m_localPath = "/";
			fi_test.m_originalLocalPath = "%UpdateRoot%";
			fi_vector.push_back(fi_test);
		};


		TD_BOOST_CHECK_MESSAGE (KLUPD::CORE_NO_ERROR == _ui.parse (config_ui.xmlfolder, fi1, UpdateInfo::noDateCheckForLocalFiles, true), "parse failed");
		TD_BOOST_CHECK_MESSAGE (fi_vector == _ui.m_matchFileList, "m_matchFileList comparison failed");
		//TD_BOOST_CHECK_MESSAGE (ind_vector == _ui.m_attachedIndex, "m_attachedIndex comparison failed");
	}

	{
		//test 2
		std::cout<<"Test application blacklist \n";

		Ini_Updater _updater (NULL, &log, true);
		UpdaterConfigurationData ucd;

		Ini_ProductConfiguration& _prod_config = static_cast<Ini_ProductConfiguration&>(_updater.getProdConfig());
		Ini_UpdaterSettings& _upd_config = static_cast<Ini_UpdaterSettings&>(_updater.getUpdConfig());

		ucd.retranslate_only_listed_apps = true;
		ucd.RetranslatedObjects.Applications_WhiteListMode = false;
		{
			vector<STRING> vct;
			vct.push_back ("11.0");
			ucd.RetranslatedObjects.Applications.AddPair ("Application", vct);
		}
		ucd.RetranslatedObjects.Applications.AddPair ("Application1", vector<STRING>());

		UpdateInfo _ui (_prod_config, ucd, &log, &journal, &callback, UpdateInfo::withValidationWithFiltering, true);

		FileInfo_Unit fi_test;
		vector<FileInfo_Unit> fi_vector;
		vector<FileInfo_Unit> ind_vector;
		FileInfo fi1 ("check_appblist.xml");

		ind_vector.push_back (FileInfo_Unit ("check_appblist.xml"));

		{
			FileInfo_Unit fi_test;
			fi_test.m_applicationID = "Application2";
			fi_test.m_type = FileInfo::base;
			fi_test.m_filename = "updcfg4.xml";
			fi_test.m_signature = "0XLSznpdI71fB300e7Uwj1NpFafWhvt5vcBIpNl/vu+nnNvXG4hJYL25hR";
			fi_test.m_size = 10185;
			fi_test.m_dateFromIndex = "24032006 1629";
			fi_test.m_relativeURLPath = "/";
			fi_test.m_localPath = "/";
			fi_test.m_originalLocalPath = "%UpdateRoot%";
			fi_vector.push_back(fi_test);
		};

		TD_BOOST_CHECK_MESSAGE (KLUPD::CORE_NO_ERROR == _ui.parse (config_ui.xmlfolder, fi1, UpdateInfo::noDateCheckForLocalFiles, true), "parse failed");
		TD_BOOST_CHECK_MESSAGE (fi_vector == _ui.m_matchFileList, "m_matchFileList comparison failed");
		//TD_BOOST_CHECK_MESSAGE (ind_vector == _ui.m_attachedIndex, "m_attachedIndex comparison failed");
	}
	
{
		//test 3
		std::cout<<"Test retranslate_only_listed_apps setting \n";

		Ini_Updater _updater (NULL, &log, true);
		UpdaterConfigurationData ucd;

		Ini_ProductConfiguration& _prod_config = static_cast<Ini_ProductConfiguration&>(_updater.getProdConfig());
		Ini_UpdaterSettings& _upd_config = static_cast<Ini_UpdaterSettings&>(_updater.getUpdConfig());

		ucd.retranslate_only_listed_apps = false;
		ucd.RetranslatedObjects.Applications_WhiteListMode = true;
		{
			vector<STRING> vct;
			vct.push_back ("11.0");
			ucd.RetranslatedObjects.Applications.AddPair ("Application", vct);
		}

		UpdateInfo _ui (_prod_config, ucd, &log, &journal, &callback, UpdateInfo::withValidationWithFiltering, true);

		FileInfo_Unit fi_test;
		vector<FileInfo_Unit> fi_vector;
		vector<FileInfo_Unit> ind_vector;
		FileInfo fi1 ("check_appblist.xml");

		ind_vector.push_back (FileInfo_Unit ("check_appblist.xml"));

		{
			FileInfo_Unit fi_test;
			fi_test.m_applicationID = "Application";
			fi_test.m_type = FileInfo::base;
			fi_test.m_filename = "updcfg1.xml";
			fi_test.m_signature = "0XLSznpdI71fB300e7Uwj1NpFafWhvt5vcBIpNl/vu+nnNvXG4hJYL25hR";
			fi_test.m_size = 10185;
			fi_test.m_dateFromIndex = "24032006 1629";
			fi_test.m_relativeURLPath = "/";
			fi_test.m_localPath = "/";
			fi_test.m_originalLocalPath = "%UpdateRoot%";
			fi_test.m_fromAppVersion.push_back ("12.0");
			fi_vector.push_back(fi_test);
		};
		{
			FileInfo_Unit fi_test;
			fi_test.m_applicationID = "Application";
			fi_test.m_type = FileInfo::base;
			fi_test.m_filename = "updcfg2.xml";
			fi_test.m_signature = "0XLSznpdI71fB300e7Uwj1NpFafWhvt5vcBIpNl/vu+nnNvXG4hJYL25hR";
			fi_test.m_size = 10185;
			fi_test.m_dateFromIndex = "24032006 1629";
			fi_test.m_relativeURLPath = "/";
			fi_test.m_localPath = "/";
			fi_test.m_originalLocalPath = "%UpdateRoot%";
			fi_test.m_fromAppVersion.push_back ("10.0");
			fi_vector.push_back(fi_test);
		};
		{
			FileInfo_Unit fi_test;
			fi_test.m_applicationID = "Application1";
			fi_test.m_type = FileInfo::base;
			fi_test.m_filename = "updcfg3.xml";
			fi_test.m_signature = "0XLSznpdI71fB300e7Uwj1NpFafWhvt5vcBIpNl/vu+nnNvXG4hJYL25hR";
			fi_test.m_size = 10185;
			fi_test.m_dateFromIndex = "24032006 1629";
			fi_test.m_relativeURLPath = "/";
			fi_test.m_localPath = "/";
			fi_test.m_originalLocalPath = "%UpdateRoot%";
			fi_vector.push_back(fi_test);
		};
		{
			FileInfo_Unit fi_test;
			fi_test.m_applicationID = "Application2";
			fi_test.m_type = FileInfo::base;
			fi_test.m_filename = "updcfg4.xml";
			fi_test.m_signature = "0XLSznpdI71fB300e7Uwj1NpFafWhvt5vcBIpNl/vu+nnNvXG4hJYL25hR";
			fi_test.m_size = 10185;
			fi_test.m_dateFromIndex = "24032006 1629";
			fi_test.m_relativeURLPath = "/";
			fi_test.m_localPath = "/";
			fi_test.m_originalLocalPath = "%UpdateRoot%";
			fi_vector.push_back(fi_test);
		};

		TD_BOOST_CHECK_MESSAGE (KLUPD::CORE_NO_ERROR == _ui.parse (config_ui.xmlfolder, fi1, UpdateInfo::noDateCheckForLocalFiles, true), "parse failed");
		TD_BOOST_CHECK_MESSAGE (fi_vector == _ui.m_matchFileList, "m_matchFileList comparison failed");
		//TD_BOOST_CHECK_MESSAGE (ind_vector == _ui.m_attachedIndex, "m_attachedIndex comparison failed");
	}
	Step ("TestBlackWhiteListsApp", reason);
};


void Test_UpdateInfo::TestBlackWhiteListsComp ()
{
	string reason;
	{
		//test 1
		std::cout<<"Test Component whitelist \n";

		Ini_Updater _updater (NULL, &log, true);
		UpdaterConfigurationData ucd;

		Ini_ProductConfiguration& _prod_config = static_cast<Ini_ProductConfiguration&>(_updater.getProdConfig());
		Ini_UpdaterSettings& _upd_config = static_cast<Ini_UpdaterSettings&>(_updater.getUpdConfig());

		ucd.retranslate_only_listed_apps = true;
		ucd.RetranslatedObjects.Components_WhiteListMode = true;
		{
			vector<STRING> vct;
			vct.push_back ("5.0");
			vct.push_back ("5.1");
			ucd.RetranslatedObjects.Components.AddPair ("Component", vct);
		}
		ucd.RetranslatedObjects.Components.AddPair ("Component1", vector<STRING>());
		{
			vector<STRING> vct;
			vct.push_back ("22.0");
			ucd.RetranslatedObjects.Components.AddPair ("Component3", vct);
		}

		UpdateInfo _ui (_prod_config, ucd, &log, &journal, &callback, UpdateInfo::withValidationWithFiltering, true);

		FileInfo_Unit fi_test;
		vector<FileInfo_Unit> fi_vector;
		vector<FileInfo_Unit> ind_vector;
		FileInfo fi1 ("check_compwlist.xml");

		ind_vector.push_back (FileInfo_Unit ("check_compwlist.xml"));

		{
			FileInfo_Unit fi_test;
			fi_test.m_componentIdSet.push_back ("Component");
			fi_test.m_type = FileInfo::base;
			fi_test.m_filename = "updcfg1.xml";
			fi_test.m_signature = "0XLSznpdI71fB300e7Uwj1NpFafWhvt5vcBIpNl/vu+nnNvXG4hJYL25hR";
			fi_test.m_size = 10185;
			fi_test.m_dateFromIndex = "24032006 1629";
			fi_test.m_relativeURLPath = "/";
			fi_test.m_localPath = "/";
			fi_test.m_originalLocalPath = "%UpdateRoot%";
			fi_vector.push_back(fi_test);
		};
		{
			FileInfo_Unit fi_test;
			fi_test.m_componentIdSet.push_back ("Component");
			fi_test.m_type = FileInfo::base;
			fi_test.m_filename = "updcfg2.xml";
			fi_test.m_signature = "0XLSznpdI71fB300e7Uwj1NpFafWhvt5vcBIpNl/vu+nnNvXG4hJYL25hR";
			fi_test.m_size = 10185;
			fi_test.m_dateFromIndex = "24032006 1629";
			fi_test.m_relativeURLPath = "/";
			fi_test.m_localPath = "/";
			fi_test.m_originalLocalPath = "%UpdateRoot%";
			fi_test.m_fromCompVersion.push_back ("11.0");
			fi_test.m_fromCompVersion.push_back ("12.0");
			fi_vector.push_back(fi_test);
		};
		{
			FileInfo_Unit fi_test;
			fi_test.m_componentIdSet.push_back ("Component");
			fi_test.m_type = FileInfo::base;
			fi_test.m_filename = "updcfg4.xml";
			fi_test.m_signature = "0XLSznpdI71fB300e7Uwj1NpFafWhvt5vcBIpNl/vu+nnNvXG4hJYL25hR";
			fi_test.m_size = 10185;
			fi_test.m_dateFromIndex = "24032006 1629";
			fi_test.m_relativeURLPath = "/";
			fi_test.m_localPath = "/";
			fi_test.m_originalLocalPath = "%UpdateRoot%";
			fi_test.m_fromCompVersion.push_back ("5.5");
			fi_vector.push_back(fi_test);
		};
		{
			FileInfo_Unit fi_test;
			fi_test.m_componentIdSet.push_back ("Component1");
			fi_test.m_type = FileInfo::base;
			fi_test.m_filename = "updcfg5.xml";
			fi_test.m_signature = "0XLSznpdI71fB300e7Uwj1NpFafWhvt5vcBIpNl/vu+nnNvXG4hJYL25hR";
			fi_test.m_size = 10185;
			fi_test.m_dateFromIndex = "24032006 1629";
			fi_test.m_relativeURLPath = "/";
			fi_test.m_localPath = "/";
			fi_test.m_originalLocalPath = "%UpdateRoot%";
			fi_test.m_fromCompVersion.push_back ("10.0");
			fi_vector.push_back(fi_test);
		};
		{
			FileInfo_Unit fi_test;
			fi_test.m_componentIdSet.push_back ("Component3");
			fi_test.m_type = FileInfo::base;
			fi_test.m_filename = "updcfg7.xml";
			fi_test.m_signature = "0XLSznpdI71fB300e7Uwj1NpFafWhvt5vcBIpNl/vu+nnNvXG4hJYL25hR";
			fi_test.m_size = 10185;
			fi_test.m_dateFromIndex = "24032006 1629";
			fi_test.m_relativeURLPath = "/";
			fi_test.m_localPath = "/";
			fi_test.m_originalLocalPath = "%UpdateRoot%";
			fi_vector.push_back(fi_test);
		};
		{
			FileInfo_Unit fi_test;
			fi_test.m_type = FileInfo::base;
			fi_test.m_filename = "updcfg8.xml";
			fi_test.m_signature = "0XLSznpdI71fB300e7Uwj1NpFafWhvt5vcBIpNl/vu+nnNvXG4hJYL25hR";
			fi_test.m_size = 10185;
			fi_test.m_dateFromIndex = "24032006 1629";
			fi_test.m_relativeURLPath = "/";
			fi_test.m_localPath = "/";
			fi_test.m_originalLocalPath = "%UpdateRoot%";
			fi_vector.push_back(fi_test);
		};


		TD_BOOST_CHECK_MESSAGE (KLUPD::CORE_NO_ERROR == _ui.parse (config_ui.xmlfolder, fi1, UpdateInfo::noDateCheckForLocalFiles, true), "parse failed");
		TD_BOOST_CHECK_MESSAGE (fi_vector == _ui.m_matchFileList, "m_matchFileList comparison failed");
		//TD_BOOST_CHECK_MESSAGE (ind_vector == _ui.m_attachedIndex, "m_attachedIndex comparison failed");
	}

	{
		//test 2
		std::cout<<"Test Component blacklist \n";

		Ini_Updater _updater (NULL, &log, true);
		UpdaterConfigurationData ucd;

		Ini_ProductConfiguration& _prod_config = static_cast<Ini_ProductConfiguration&>(_updater.getProdConfig());
		Ini_UpdaterSettings& _upd_config = static_cast<Ini_UpdaterSettings&>(_updater.getUpdConfig());

		ucd.retranslate_only_listed_apps = true;
		ucd.RetranslatedObjects.Components_WhiteListMode = false;
		{
			vector<STRING> vct;
			vct.push_back ("11.0");
			ucd.RetranslatedObjects.Components.AddPair ("Component", vct);
		}
		ucd.RetranslatedObjects.Components.AddPair ("Component1", vector<STRING>());

		UpdateInfo _ui (_prod_config, ucd, &log, &journal, &callback, UpdateInfo::withValidationWithFiltering, true);

		FileInfo_Unit fi_test;
		vector<FileInfo_Unit> fi_vector;
		vector<FileInfo_Unit> ind_vector;
		FileInfo fi1 ("check_compblist.xml");

		ind_vector.push_back (FileInfo_Unit ("check_compblist.xml"));

		{
			FileInfo_Unit fi_test;
			fi_test.m_componentIdSet.push_back ("Component2");
			fi_test.m_type = FileInfo::base;
			fi_test.m_filename = "updcfg4.xml";
			fi_test.m_signature = "0XLSznpdI71fB300e7Uwj1NpFafWhvt5vcBIpNl/vu+nnNvXG4hJYL25hR";
			fi_test.m_size = 10185;
			fi_test.m_dateFromIndex = "24032006 1629";
			fi_test.m_relativeURLPath = "/";
			fi_test.m_localPath = "/";
			fi_test.m_originalLocalPath = "%UpdateRoot%";
			fi_vector.push_back(fi_test);
		};

		TD_BOOST_CHECK_MESSAGE (KLUPD::CORE_NO_ERROR == _ui.parse (config_ui.xmlfolder, fi1, UpdateInfo::noDateCheckForLocalFiles, true), "parse failed");
		TD_BOOST_CHECK_MESSAGE (fi_vector == _ui.m_matchFileList, "m_matchFileList comparison failed");
		//TD_BOOST_CHECK_MESSAGE (ind_vector == _ui.m_attachedIndex, "m_attachedIndex comparison failed");
	}
	
{
		//test 3
		std::cout<<"Test retranslate_only_listed_Comps setting \n";

		Ini_Updater _updater (NULL, &log, true);
		UpdaterConfigurationData ucd;

		Ini_ProductConfiguration& _prod_config = static_cast<Ini_ProductConfiguration&>(_updater.getProdConfig());
		Ini_UpdaterSettings& _upd_config = static_cast<Ini_UpdaterSettings&>(_updater.getUpdConfig());

		ucd.retranslate_only_listed_apps = false;
		ucd.RetranslatedObjects.Components_WhiteListMode = true;
		{
			vector<STRING> vct;
			vct.push_back ("11.0");
			ucd.RetranslatedObjects.Components.AddPair ("Component", vct);
		}

		UpdateInfo _ui (_prod_config, ucd, &log, &journal, &callback, UpdateInfo::withValidationWithFiltering, true);


		FileInfo_Unit fi_test;
		vector<FileInfo_Unit> fi_vector;
		vector<FileInfo_Unit> ind_vector;
		FileInfo fi1 ("check_compblist.xml");

		ind_vector.push_back (FileInfo_Unit ("check_compblist.xml"));

		{
			FileInfo_Unit fi_test;
			fi_test.m_componentIdSet.push_back ("Component");
			fi_test.m_type = FileInfo::base;
			fi_test.m_filename = "updcfg1.xml";
			fi_test.m_signature = "0XLSznpdI71fB300e7Uwj1NpFafWhvt5vcBIpNl/vu+nnNvXG4hJYL25hR";
			fi_test.m_size = 10185;
			fi_test.m_dateFromIndex = "24032006 1629";
			fi_test.m_relativeURLPath = "/";
			fi_test.m_localPath = "/";
			fi_test.m_originalLocalPath = "%UpdateRoot%";
			fi_test.m_fromCompVersion.push_back ("12.0");
			fi_vector.push_back(fi_test);
		};
		{
			FileInfo_Unit fi_test;
			fi_test.m_componentIdSet.push_back ("Component");
			fi_test.m_type = FileInfo::base;
			fi_test.m_filename = "updcfg2.xml";
			fi_test.m_signature = "0XLSznpdI71fB300e7Uwj1NpFafWhvt5vcBIpNl/vu+nnNvXG4hJYL25hR";
			fi_test.m_size = 10185;
			fi_test.m_dateFromIndex = "24032006 1629";
			fi_test.m_relativeURLPath = "/";
			fi_test.m_localPath = "/";
			fi_test.m_originalLocalPath = "%UpdateRoot%";
			fi_test.m_fromCompVersion.push_back ("10.0");
			fi_vector.push_back(fi_test);
		};
		{
			FileInfo_Unit fi_test;
			fi_test.m_componentIdSet.push_back ("Component1");
			fi_test.m_type = FileInfo::base;
			fi_test.m_filename = "updcfg3.xml";
			fi_test.m_signature = "0XLSznpdI71fB300e7Uwj1NpFafWhvt5vcBIpNl/vu+nnNvXG4hJYL25hR";
			fi_test.m_size = 10185;
			fi_test.m_dateFromIndex = "24032006 1629";
			fi_test.m_relativeURLPath = "/";
			fi_test.m_localPath = "/";
			fi_test.m_originalLocalPath = "%UpdateRoot%";
			fi_vector.push_back(fi_test);
		};
		{
			FileInfo_Unit fi_test;
			fi_test.m_componentIdSet.push_back ("Component2");
			fi_test.m_type = FileInfo::base;
			fi_test.m_filename = "updcfg4.xml";
			fi_test.m_signature = "0XLSznpdI71fB300e7Uwj1NpFafWhvt5vcBIpNl/vu+nnNvXG4hJYL25hR";
			fi_test.m_size = 10185;
			fi_test.m_dateFromIndex = "24032006 1629";
			fi_test.m_relativeURLPath = "/";
			fi_test.m_localPath = "/";
			fi_test.m_originalLocalPath = "%UpdateRoot%";
			fi_vector.push_back(fi_test);
		};

		TD_BOOST_CHECK_MESSAGE (KLUPD::CORE_NO_ERROR == _ui.parse (config_ui.xmlfolder, fi1, UpdateInfo::noDateCheckForLocalFiles, true), "parse failed");
		TD_BOOST_CHECK_MESSAGE (fi_vector == _ui.m_matchFileList, "m_matchFileList comparison failed");
		//TD_BOOST_CHECK_MESSAGE (ind_vector == _ui.m_attachedIndex, "m_attachedIndex comparison failed");
	}
	Step ("TestBlackWhiteListsComp", reason);
};

Suite_UpdateInfo::Suite_UpdateInfo (const string& ini_file, const string& log_name)
{
	bool bLog;
	if (log_name == "") bLog =false; else bLog = true;

	boost::shared_ptr<Test_UpdateInfo> instance (new Test_UpdateInfo(ini_file, bLog, log_name));
	
	add (boost::unit_test::make_test_case<Test_UpdateInfo>(&Test_UpdateInfo::Start, "Test_UpdateInfo::Start", instance));
	//add (BOOST_CLASS_TEST_CASE (&Test_UpdateInfo::TestUpdateLocLang, instance));
	//add (BOOST_CLASS_TEST_CASE (&Test_UpdateInfo::TestUpdateAppID, instance));
	//add (BOOST_CLASS_TEST_CASE (&Test_UpdateInfo::TestUpdateFromAppVersion, instance));
	add (BOOST_CLASS_TEST_CASE (&Test_UpdateInfo::TestUpdateComponents, instance));
	//add (BOOST_CLASS_TEST_CASE (&Test_UpdateInfo::TestUpdateOSes, instance));
	//add (BOOST_CLASS_TEST_CASE (&Test_UpdateInfo::TestLocFilter, instance));
	//add (BOOST_CLASS_TEST_CASE (&Test_UpdateInfo::TestLocFilterRetr, instance));
	//add (BOOST_CLASS_TEST_CASE (&Test_UpdateInfo::TestEntryRequirements, instance));
	//add (BOOST_CLASS_TEST_CASE (&Test_UpdateInfo::TestBlackWhiteListsApp, instance));
	//add (BOOST_CLASS_TEST_CASE (&Test_UpdateInfo::TestBlackWhiteListsComp, instance));
	add (boost::unit_test::make_test_case<Test_UpdateInfo>(&Test_UpdateInfo::End, "Test_UpdateInfo::End", instance));

	
	
	

};