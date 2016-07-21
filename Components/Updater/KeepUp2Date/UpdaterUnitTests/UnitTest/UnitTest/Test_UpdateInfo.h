#ifndef TEST_UPDATEINFO_H
#define TEST_UPDATEINFO_H

#include "ini_updater.h"
#include "..\..\..\Client\core\update_info.h"
#include "..\..\..\Client\helper\update_transaction.h"
#include "prod_config.h"

#include "helper.h"
#include "Configuration.h"
#include "TestInterface.h"

#include "boost_test.h"


class Test_UpdateInfo: public BaseTest
{
	public:
		Test_UpdateInfo (const string& ini_file, bool log_enable, const string& log_name):
									log (log_enable),
									updater (NULL, &log, true),
									journal (&log),
									//prod_config (updater, &log, true),
									config_ui (Configuration::Init (ini_file.c_str()) .GetUpdateInfoConfig()),
									BaseTest ("UpdateInfo")
									{};
	
	void TestUpdateLocLang ();
	void TestUpdateAppID ();
	void TestUpdateFromAppVersion ();
	void TestUpdateComponents ();
	void TestUpdateOSes ();
	void TestLocFilter ();
	void TestLocFilterRetr ();
	void TestEntryRequirements ();
	void TestBlackWhiteListsApp ();
	void TestBlackWhiteListsComp ();

	private:

		Ini_Log log;
		Ini_Journal journal;
		Ini_Updater updater;
		//Ini_ProductConfiguration prod_config;
		IniCallbacks callback;

		const Config_UpdateInfo& config_ui;
	
};


class Suite_UpdateInfo: public test_suite
{
	public:

		Suite_UpdateInfo (const string& ini_file, const string& log_name);
		
};





















#endif