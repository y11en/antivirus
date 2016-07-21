#ifndef TEST_PRIMARYINDEXXMLPARSER_H
#define TEST_PRIMARYINDEXXMLPARSER_H

#include "..\..\..\Client\helper\update_transaction.h"
#include "prod_config.h"
#include "..\..\..\Client\helper\sitesfilexmlparser.h"
#include "..\..\..\Client\helper\sites_info.h"

#include "..\..\..\Client\Include\kavupd_strings.h"

#include "helper.h"
#include "Configuration.h"
#include "TestInterface.h"

#include "boost_test.h"

class Test_PrimaryIndexXMLParser: public BaseTest
{
public:
	Test_PrimaryIndexXMLParser (const string& ini_file, bool log_enable, const string& log_name):
									log (log_enable),
									updater (NULL, &log, true),
									journal (&log),
									prod_config (updater, &log, true),
									BaseTest ("PrimaryIndexXMLParser")
									{};
		
	void TestPrimary ();


private:

	Ini_Log log;
	Ini_Journal journal;
	Ini_Updater updater;
	Ini_ProductConfiguration prod_config;

};


class Suite_PrimaryIndexXMLParser: public test_suite
{
	public:

		Suite_PrimaryIndexXMLParser (const string& ini_file, const string& log_name);
};


#endif