#ifndef TEST_SITESFILEXMLPARSER_H
#define TEST_SITESFILEXMLPARSER_H

#include "..\..\..\Client\helper\update_transaction.h"
#include "prod_config.h"
#include "..\..\..\Client\helper\sitesfilexmlparser.h"
#include "..\..\..\Client\helper\sites_info.h"

#include "..\..\..\Client\Include\kavupd_strings.h"

#include "helper.h"
#include "Configuration.h"
#include "TestInterface.h"

#include "boost_test.h"

class Test_SitesFileXMLParser: public BaseTest
{
public:
	Test_SitesFileXMLParser (const string& ini_file, bool log_enable, const string& log_name):
									log (log_enable),
									updater (NULL, &log, true),
									journal (&log),
									prod_config (updater, &log, true),
									BaseTest ("SitesFileXMLParser")
									{};
		
	void TestSites ();


private:

	Ini_Log log;
	Ini_Journal journal;
	Ini_Updater updater;
	Ini_ProductConfiguration prod_config;

};


class Suite_SitesFileXMLParser: public test_suite
{
	public:

		Suite_SitesFileXMLParser (const string& ini_file, const string& log_name);
};


#endif