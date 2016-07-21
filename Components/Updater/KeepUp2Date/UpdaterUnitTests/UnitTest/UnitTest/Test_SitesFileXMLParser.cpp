#include "Test_SitesFileXMLParser.h"
#include "helper.h"


void Test_SitesFileXMLParser::TestSites()
{
	string reason;
	std::cout<<"Test parsing sites\n";
	//1
	{
		std::cout<<"Test old sites format\n";

		string el_sites=
		"<UpdaterSettingsSection><sites_list>\
<site url=\"http://ru1h.kaspersky-labs.com\" weight=\"3\" region=\"Russia\"></site>\
<site url=\"http://ru2h.kaspersky-labs.com\" weight=\"5\" region=\"Gonduras\"></site>\
</sites_list></UpdaterSettingsSection>";
	

		int fsize = strlen(el_sites.c_str());
		map<string, unsigned> skippedSections;
		vector<SitesInfo> m_sites;
		

		SitesFileXMLParser xmler(&m_sites, true, &log);
		XmlReader xmlReader(el_sites.c_str(), fsize);
		vector<SitesInfo_Unit> etalon;
		map<string, unsigned> map;

		{
			SitesInfo_Unit si_test;
			si_test.srvURL = "http://ru1h.kaspersky-labs.com";
			si_test.srvWeight = 3;
			si_test.srvRegion = "Russia";
			si_test.m_RegionList.push_back ("Russia");
			etalon.push_back (si_test);
		}
		{
			SitesInfo_Unit si_test;
			si_test.srvURL = "http://ru2h.kaspersky-labs.com";
			si_test.srvWeight = 5;
			si_test.srvRegion = "Gonduras";
			si_test.m_RegionList.push_back ("Gonduras");
			etalon.push_back (si_test);
		}

		TD_BOOST_CHECK_MESSAGE (true == xmler.parseSiteXMLRecursively(xmlReader, 0), "Error while parsing");
		TD_BOOST_CHECK_MESSAGE (etalon == m_sites, "Incorrect sites list");
		//TD_BOOST_CHECK_MESSAGE (skippedSections == map, "Incorrect skippedSections");
	}
	//2
	{
		std::cout<<"Test new sites format\n";

		string el_sites=
		"<UpdaterSettingsSection><sites_list>\
<site2 url=\"http://ru1h.kaspersky-labs.com\" weight=\"3\" region=\"ab,cd,ef\"/>\
<site2 url=\"http://ru2h.kaspersky-labs.com\" weight=\"5\" region=\"*\"/>\
</sites_list></UpdaterSettingsSection>";
	

		int fsize = strlen(el_sites.c_str());
		map<string, unsigned> skippedSections;
		vector<SitesInfo> m_sites;
		

		SitesFileXMLParser xmler(&m_sites, false, &log);
		XmlReader xmlReader(el_sites.c_str(), fsize);
		vector<SitesInfo_Unit> etalon;
		map<string, unsigned> map;

		{
			SitesInfo_Unit si_test;
			si_test.srvURL = "http://ru1h.kaspersky-labs.com";
			si_test.srvWeight = 3;
			si_test.srvRegion = "ab,cd,ef";
			si_test.m_RegionList.push_back ("ab");
			si_test.m_RegionList.push_back ("cd");
			si_test.m_RegionList.push_back ("ef");
			etalon.push_back (si_test);
		}
		{
			SitesInfo_Unit si_test;
			si_test.srvURL = "http://ru2h.kaspersky-labs.com";
			si_test.srvWeight = 5;
			si_test.srvRegion = "*";
			si_test.m_RegionList.push_back ("*");
			etalon.push_back (si_test);
		}

		TD_BOOST_CHECK_MESSAGE (true == xmler.parseSiteXMLRecursively(xmlReader, 0), "Error while parsing");
		TD_BOOST_CHECK_MESSAGE (etalon == m_sites, "Incorrect sites list");
		//TD_BOOST_CHECK_MESSAGE (skippedSections == map, "Incorrect skippedSections");
	}
	//3
	{
		std::cout<<"Test selecting old sites tags from mixed data\n";

		string el_sites=
		"<UpdaterSettingsSection><sites_list>\
<site url=\"http://ru1h.kaspersky-labs.com\" weight=\"3\" region=\"Russia\"></site>\
<site2 url=\"http://ru1h.kaspersky-labs.com\" weight=\"3\" region=\"ab,cd,ef\"/>\
<site url=\"http://ru2h.kaspersky-labs.com\" weight=\"5\" region=\"Gonduras\"></site>\
</sites_list></UpdaterSettingsSection>";
	

		int fsize = strlen(el_sites.c_str());
		map<string, unsigned> skippedSections;
		vector<SitesInfo> m_sites;
		

		SitesFileXMLParser xmler(&m_sites, true, &log);
		XmlReader xmlReader(el_sites.c_str(), fsize);
		vector<SitesInfo_Unit> etalon;
		map<string, unsigned> map;

		{
			SitesInfo_Unit si_test;
			si_test.srvURL = "http://ru1h.kaspersky-labs.com";
			si_test.srvWeight = 3;
			si_test.srvRegion = "Russia";
			si_test.m_RegionList.push_back ("Russia");
			etalon.push_back (si_test);
		}
		{
			SitesInfo_Unit si_test;
			si_test.srvURL = "http://ru2h.kaspersky-labs.com";
			si_test.srvWeight = 5;
			si_test.srvRegion = "Gonduras";
			si_test.m_RegionList.push_back ("Gonduras");
			etalon.push_back (si_test);
		}
		map["site2"] = 1;

		TD_BOOST_CHECK_MESSAGE (true == xmler.parseSiteXMLRecursively(xmlReader, 0), "Error while parsing");
		TD_BOOST_CHECK_MESSAGE (etalon == m_sites, "Incorrect sites list");
		//TD_BOOST_CHECK_MESSAGE (skippedSections == map, "Incorrect skippedSections");
	}
	//4
	{
		std::cout<<"Test selecting new sites tags from mixed data\n";

		string el_sites=
		"<UpdaterSettingsSection><sites_list>\
<site2 url=\"http://ru1h.kaspersky-labs.com\" weight=\"3\" region=\"ab,cd,ef\"/>\
<site url=\"http://ru2h.kaspersky-labs.com\" weight=\"5\" region=\"Gonduras\"></site>\
<site2 url=\"http://ru2h.kaspersky-labs.com\" weight=\"5\" region=\"*\"/>\
</sites_list></UpdaterSettingsSection>";
	

		int fsize = strlen(el_sites.c_str());
		map<string, unsigned> skippedSections;
		vector<SitesInfo> m_sites;
		

		SitesFileXMLParser xmler(&m_sites, false, &log);
		XmlReader xmlReader(el_sites.c_str(), fsize);
		vector<SitesInfo_Unit> etalon;
		map<string, unsigned> map;

		{
			SitesInfo_Unit si_test;
			si_test.srvURL = "http://ru1h.kaspersky-labs.com";
			si_test.srvWeight = 3;
			si_test.srvRegion = "ab,cd,ef";
			si_test.m_RegionList.push_back ("ab");
			si_test.m_RegionList.push_back ("cd");
			si_test.m_RegionList.push_back ("ef");
			etalon.push_back (si_test);
		}
		{
			SitesInfo_Unit si_test;
			si_test.srvURL = "http://ru2h.kaspersky-labs.com";
			si_test.srvWeight = 5;
			si_test.srvRegion = "*";
			si_test.m_RegionList.push_back ("*");
			etalon.push_back (si_test);
		}
		map["site"] = 1;

		TD_BOOST_CHECK_MESSAGE (true == xmler.parseSiteXMLRecursively(xmlReader, 0), "Error while parsing");
		TD_BOOST_CHECK_MESSAGE (etalon == m_sites, "Incorrect sites list");
		//TD_BOOST_CHECK_MESSAGE (skippedSections == map, "Incorrect skippedSections");
	}
	//5
	{
		std::cout<<"Test parsing old format records mixed with unknown formats\n";

		string el_sites=
		"<UpdaterSettingsSection><sites_list>\
<site3 url=\"http://ru1h.kaspersky-labs.com\" weight=\"3\" region=\"ab,cd,ef\"/>\
<site url=\"http://ru2h.kaspersky-labs.com\" weight=\"5\" region=\"Gonduras\"></site>\
<site4 url=\"http://ru2h.kaspersky-labs.com\" weight=\"5\" region=\"*\"/>\
</sites_list></UpdaterSettingsSection>";
	

		int fsize = strlen(el_sites.c_str());
		map<string, unsigned> skippedSections;
		vector<SitesInfo> m_sites;
		

		SitesFileXMLParser xmler(&m_sites, true, &log);
		XmlReader xmlReader(el_sites.c_str(), fsize);
		vector<SitesInfo_Unit> etalon;
		map<string, unsigned> map;

		{
			SitesInfo_Unit si_test;
			si_test.srvURL = "http://ru2h.kaspersky-labs.com";
			si_test.srvWeight = 5;
			si_test.srvRegion = "Gonduras";
			si_test.m_RegionList.push_back ("Gonduras");
			etalon.push_back (si_test);
		}

		map["site3"] = 1;
		map["site4"] = 1;

		TD_BOOST_CHECK_MESSAGE (true == xmler.parseSiteXMLRecursively(xmlReader, 0), "Error while parsing");
		TD_BOOST_CHECK_MESSAGE (etalon == m_sites, "Incorrect sites list");
		//TD_BOOST_CHECK_MESSAGE (skippedSections == map, "Incorrect skippedSections");
	}
	//6
	{
		std::cout<<"Test parsing new format records mixed with unknown formats\n";

		string el_sites=
		"<UpdaterSettingsSection><sites_list>\
<site3 url=\"http://ru1h.kaspersky-labs.com\" weight=\"3\" region=\"ab,cd,ef\"/>\
<site2 url=\"http://ru1h.kaspersky-labs.com\" weight=\"3\" region=\"ab,cd,ef\"/>\
<site4 url=\"http://ru2h.kaspersky-labs.com\" weight=\"5\" region=\"*\"/>\
</sites_list></UpdaterSettingsSection>";
	

		int fsize = strlen(el_sites.c_str());
		map<string, unsigned> skippedSections;
		vector<SitesInfo> m_sites;
		

		SitesFileXMLParser xmler(&m_sites, false, &log);
		XmlReader xmlReader(el_sites.c_str(), fsize);
		vector<SitesInfo_Unit> etalon;
		map<string, unsigned> map;

		{
			SitesInfo_Unit si_test;
			si_test.srvURL = "http://ru1h.kaspersky-labs.com";
			si_test.srvWeight = 3;
			si_test.srvRegion = "ab,cd,ef";
			si_test.m_RegionList.push_back ("ab");
			si_test.m_RegionList.push_back ("cd");
			si_test.m_RegionList.push_back ("ef");
			etalon.push_back (si_test);
		}

		map["site3"] = 1;
		map["site4"] = 1;

		TD_BOOST_CHECK_MESSAGE (true == xmler.parseSiteXMLRecursively(xmlReader, 0), "Error while parsing");
		TD_BOOST_CHECK_MESSAGE (etalon == m_sites, "Incorrect sites list");
		//TD_BOOST_CHECK_MESSAGE (skippedSections == map, "Incorrect skippedSections");
	}
	//7
	{
		std::cout<<"Test parsing corrupted data #1\n";

		string el_sites=
		"<sites_list>\
<site url=\"http://ru1h.kaspersky-labs.com\" weight=\"3\" region=\"ab,cd,ef\"/>\
<site url=\"http://ru1h.kaspersky-labs.com\" weight=\"3\" region=\"ab,cd,ef\"/>\
<site url=\"http://ru2h.kaspersky-labs.com\" weight=\"5\" region=\"*\"/>\
</sites_list>";
	

		int fsize = strlen(el_sites.c_str());
		map<string, unsigned> skippedSections;
		vector<SitesInfo> m_sites;
		

		SitesFileXMLParser xmler(&m_sites, true, &log);
		XmlReader xmlReader(el_sites.c_str(), fsize);
		vector<SitesInfo_Unit> etalon;
		map<string, unsigned> map;

		map["sites_list"] = 1;
		map["site"] = 3;

		TD_BOOST_CHECK_MESSAGE (true == xmler.parseSiteXMLRecursively(xmlReader, 0), "Error while parsing");
		TD_BOOST_CHECK_MESSAGE (etalon == m_sites, "Incorrect sites list");
		//TD_BOOST_CHECK_MESSAGE (skippedSections == map, "Incorrect skippedSections");
	}
	//8
	{
		std::cout<<"Test parsing corrupted data #2\n";

		string el_sites=
		"\
<site url=\"http://ru1h.kaspersky-labs.com\" weight=\"3\" region=\"ab,cd,ef\"/>\
<site url=\"http://ru1h.kaspersky-labs.com\" weight=\"3\" region=\"ab,cd,ef\"/>\
<site url=\"http://ru2h.kaspersky-labs.com\" weight=\"5\" region=\"*\"/>\
";
	

		int fsize = strlen(el_sites.c_str());
		map<string, unsigned> skippedSections;
		vector<SitesInfo> m_sites;
		

		SitesFileXMLParser xmler(&m_sites, true, &log);
		XmlReader xmlReader(el_sites.c_str(), fsize);
		vector<SitesInfo_Unit> etalon;
		map<string, unsigned> map;

		map["site"] = 3;

		TD_BOOST_CHECK_MESSAGE (true == xmler.parseSiteXMLRecursively(xmlReader, 0), "Error while parsing");
		TD_BOOST_CHECK_MESSAGE (etalon == m_sites, "Incorrect sites list");
		//TD_BOOST_CHECK_MESSAGE (skippedSections == map, "Incorrect skippedSections");
	}
	//9
	{
		std::cout<<"Test parsing corrupted data #3\n";

		string el_sites=
		"<UpdaterSettingsSection>\
<site url=\"http://ru1h.kaspersky-labs.com\" weight=\"3\" region=\"ab,cd,ef\"/>\
<site url=\"http://ru1h.kaspersky-labs.com\" weight=\"3\" region=\"ab,cd,ef\"/>\
<site url=\"http://ru2h.kaspersky-labs.com\" weight=\"5\" region=\"*\"/>\
</UpdaterSettingsSection>";
	

		int fsize = strlen(el_sites.c_str());
		map<string, unsigned> skippedSections;
		vector<SitesInfo> m_sites;
		

		SitesFileXMLParser xmler(&m_sites, true, &log);
		XmlReader xmlReader(el_sites.c_str(), fsize);
		vector<SitesInfo_Unit> etalon;
		map<string, unsigned> map;

		map["site"] = 3;

		TD_BOOST_CHECK_MESSAGE (true == xmler.parseSiteXMLRecursively(xmlReader, 0), "Error while parsing");
		TD_BOOST_CHECK_MESSAGE (etalon == m_sites, "Incorrect sites list");
		//TD_BOOST_CHECK_MESSAGE (skippedSections == map, "Incorrect skippedSections");
	}
	//10
	{
		std::cout<<"Test parsing corrupted data #4\n";

		string el_sites=
		"<UpdaterSettingsSection><sites_list>\
<site2 url=\"http://ru1h.kaspersky-labs.com\" weight=\"yy\" region=\"ab,cd,ef\"/>\
</sites_list></UpdaterSettingsSection>";
	

		int fsize = strlen(el_sites.c_str());
		map<string, unsigned> skippedSections;
		vector<SitesInfo> m_sites;
		

		SitesFileXMLParser xmler(&m_sites, false, &log);
		XmlReader xmlReader(el_sites.c_str(), fsize);
		vector<SitesInfo_Unit> etalon;
		map<string, unsigned> map;

		{
			SitesInfo_Unit si_test;
			si_test.srvURL = "http://ru1h.kaspersky-labs.com";
			si_test.srvWeight = 1;
			si_test.srvRegion = "ab,cd,ef";
			si_test.m_RegionList.push_back ("ab");
			si_test.m_RegionList.push_back ("cd");
			si_test.m_RegionList.push_back ("ef");
			etalon.push_back (si_test);
		}

		TD_BOOST_CHECK_MESSAGE (true == xmler.parseSiteXMLRecursively(xmlReader, 0), "Error while parsing");
		TD_BOOST_CHECK_MESSAGE (etalon == m_sites, "Incorrect sites list");
		//TD_BOOST_CHECK_MESSAGE (skippedSections == map, "Incorrect skippedSections");
	}

	Step ("TestSites", reason);
};


Suite_SitesFileXMLParser::Suite_SitesFileXMLParser (const string& ini_file, const string& log_name)
{
	bool bLog;
	if (log_name == "") bLog =false; else bLog = true;

	boost::shared_ptr<Test_SitesFileXMLParser> instance (new Test_SitesFileXMLParser(ini_file, bLog, log_name));
	
	add (boost::unit_test::make_test_case<Test_SitesFileXMLParser>(&Test_SitesFileXMLParser::Start, "Test_SitesFileXMLParser::Start", instance));
	add (BOOST_CLASS_TEST_CASE (&Test_SitesFileXMLParser::TestSites, instance));
	add (boost::unit_test::make_test_case<Test_SitesFileXMLParser>(&Test_SitesFileXMLParser::End, "Test_SitesFileXMLParser::End", instance));
	

};