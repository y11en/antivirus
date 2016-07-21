#ifndef TEST_INDEXFILEXMLPARSER_H
#define TEST_INDEXFILEXMLPARSER_H

#include "..\..\..\Client\helper\update_transaction.h"
#include "prod_config.h"
#include "..\..\..\Client\helper\indexfilexmlparser.h"

#include "..\..\..\Client\Include\kavupd_strings.h"

#include "helper.h"
#include "Configuration.h"
#include "TestInterface.h"

#include "boost_test.h"

#include "..\..\..\Client\Include\kavupd_strings.h"


class Test_IndexFileXMLParser: public BaseTest
{
public:
	Test_IndexFileXMLParser (const string& ini_file, bool log_enable, const string& log_name):
									log (log_enable),
									updater (NULL, &log, true),
									journal (&log),
									prod_config (updater, &log, true),
									BaseTest ("IndexFileXMLParser")
									{};
		
	void TestFileDescription();
	void TestUpdateFiles();
	void TestOs();
	void TestOsVer();
	void TestLocLang();
	void TestExtraSections();
	void TestFromApp();
	void TestFromComp();


private:

	Ini_Log log;
	Ini_Journal journal;
	Ini_Updater updater;
	Ini_ProductConfiguration prod_config;

	FileVector m_files;
	STRING m_date;
	unsigned long m_sequenceNumber;
	MapStringStringWrapper paths_substs;

};


class Suite_IndexFileXMLParser: public test_suite
{
	public:

		Suite_IndexFileXMLParser (const string& ini_file, const string& log_name);
};


#endif