#ifndef TEST_NETCORE_H
#define TEST_NETCORE_H

#include "..\..\..\Client\net\netcore.h"
//#include "..\..\..\Client\net\net.h"

#include "helper.h"
#include "Configuration.h"
#include "TestInterface.h"

#include "boost_test.h"

class Test_NetCore: public BaseTest
{
public:
	Test_NetCore (const string& ini_file, bool log_enable, const string& log_name):
					 ptr_callback (new IniCallbacks()),
					 ptr_log (new Ini_Log (log_enable)),
					 config_nc (Configuration::Init (ini_file.c_str()) .GetNetCoreConfig()),
					 BaseTest ("NetCore") {};

	void TestSettingUserInfo ();
	void TestSettingUrl ();
	void TestGettingFileInfoAndFile ();


private:

	shared_ptr<CallbackInterface> ptr_callback;
	shared_ptr<Log> ptr_log;
	const Config_NetCore& config_nc;
	Ini_Journal journal;

};

class Suite_NetCore: public test_suite
{
	public:

		Suite_NetCore (const string& ini_file, const string& log_name);
};









































#endif