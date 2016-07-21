#include "Test_PrimaryIndexXMLParser.h"
#include "helper.h"

void Test_PrimaryIndexXMLParser::TestPrimary()
{
	string reason;

	std::cout<<"Test parsing UpdateFileDescription element\n";

	{
		string el_ufd =
"<UpdaterSettingsSection>\
<UpdateFileDescription Filename=\"master.xml\" RelativeSrvPath=\"index\" LocalPath=\"TestPath\"></UpdateFileDescription>\
</UpdaterSettingsSection>";

		std::cout<<"Normal use\n";
		int fsize = strlen(el_ufd.c_str());
		map<string, unsigned> skippedSections;
		FileInfo primaryIndex;
		
		UpdaterSettings::PrimaryIndexXMLParser xmler(&primaryIndex, &log);
		XmlReader xmlReader(el_ufd.c_str(), fsize);
		map<string, unsigned> map;
		
		FileInfo_Unit fi_test;
		fi_test.RelativeURLPath = "index";
		fi_test.Filename = "master.xml";
		fi_test.LocalPath = "TestPath";

		TD_BOOST_CHECK_MESSAGE (true == xmler.parseConfigXMLRecursively(xmlReader, 0, skippedSections), "Error while parsing");
		TD_BOOST_CHECK_MESSAGE (fi_test == primaryIndex, "Incorrect m_primaryIndex");
		TD_BOOST_CHECK_MESSAGE (skippedSections == map, "Incorrect skippedSections");
	}
	
	{
		string el_ufd =
"UpdateFileDescription Filename=\"master.xml\" RelativeSrvPath=\"index\"></UpdateFileDescription>";

		std::cout<<"Special case\n";
		int fsize = strlen(el_ufd.c_str());
		map<string, unsigned> skippedSections;
		FileInfo primaryIndex;
		
		PrimaryIndexXMLParser xmler(&primaryIndex, &log);
		XmlReader xmlReader(el_ufd.c_str(), fsize);
		map<string, unsigned> map;
		
		FileInfo_Unit fi_test;

		TD_BOOST_CHECK_MESSAGE (true == xmler.parseConfigXMLRecursively(xmlReader, 0, skippedSections), "Error while parsing");
		TD_BOOST_CHECK_MESSAGE (fi_test == primaryIndex, "Incorrect m_primaryIndex");
		TD_BOOST_CHECK_MESSAGE (skippedSections == map, "Incorrect skippedSections");
	}
	
	Step ("TestPrimary", reason);
};



Suite_PrimaryIndexXMLParser::Suite_PrimaryIndexXMLParser (const string& ini_file, const string& log_name)
{
	bool bLog;
	if (log_name == "") bLog =false; else bLog = true;

	boost::shared_ptr<Test_PrimaryIndexXMLParser> instance (new Test_PrimaryIndexXMLParser(ini_file, bLog, log_name));
	
	add (boost::unit_test::make_test_case<Test_PrimaryIndexXMLParser>(&Test_PrimaryIndexXMLParser::Start, "Test_PrimaryIndexXMLParser::Start", instance));
	add (BOOST_CLASS_TEST_CASE (&Test_PrimaryIndexXMLParser::TestPrimary, instance));
	add (boost::unit_test::make_test_case<Test_PrimaryIndexXMLParser>(&Test_PrimaryIndexXMLParser::End, "Test_PrimaryIndexXMLParser::End", instance));
	
	

};