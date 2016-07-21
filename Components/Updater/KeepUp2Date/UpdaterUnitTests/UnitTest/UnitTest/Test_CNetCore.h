#ifndef TEST_CNETCORE_H
#define TEST_CNETCORE_H

#include <winsock2.h>
//#include "..\..\..\Client\net\netcore.h"
#include "..\..\..\Client\core\netfac.h"


#include "helper.h"
#include "Configuration.h"
#include "TestInterface.h"

#include "boost_test.h"

class Test_CNetCore: public BaseTest
{
public:
	Test_CNetCore (const string& ini_file, bool log_enable, const string& log_name):
					 m_log (log_enable),
					 config_nc (Configuration::Init (ini_file.c_str()) .GetNetCoreConfig()),
					 BaseTest ("CNetCore") {};

	void TestDownloadHttp ();
	void TestUsingHttpProxy ();

	void TestDownloadAK ();

private:
	
//	CReporter m_reporter;
	Ini_Log m_log;
	IniCallbacks m_callback;
	Ini_Journal m_journal;
	UpdaterConfigurationData m_upddata;
	const Config_NetCore& config_nc;

};

class Suite_CNetCore: public test_suite
{
	public:

		Suite_CNetCore (const string& ini_file, const string& log_name);
};



#endif