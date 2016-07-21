#include "Test_IndexFileXMLParser.h"
#include "helper.h"


void Test_IndexFileXMLParser::TestFileDescription()
{
	string reason;

	std::cout<<"Test parsing FileDescription element\n";

	{
		string el_fdescr =
"<FileDescription \r\n\t\
ComponentID=\"KAVPDA55\" \r\n\t\
UpdateType=\"base\" \r\n\t\
RelativeSrvPath=\"bases/av/klava/wince\" \r\n\t\
Filename=\"stream.id\" \r\n\t\
FileSign=\"0XLSznpdI71fB300e7Uwj17mxVCUmn2y7ftLMiHayGYyOkQe7yDz/RJhHM\" \r\n\t\
FileSize=\"80\" \r\n\t\
LoadingOrder=\"154\" \r\n\t\
Set=\"normal\" \r\n\t\
LocalPath=\"%BaseFolder%\" \r\n\t\
ToAppVersion=\"5.0.205\" \r\n\t\
ToCompVersion=\"5.0.255\" \r\n\t\
Comment=\"for KasperskyAV for Windows Workstations\" \r\n\t\
UpdateObligation=\"optional\" \r\n\t\
FileDate=\"03042006 1751\" />";

		std::cout<<"Normal use\n";
		int fsize = strlen(el_fdescr.c_str());
		map<string, unsigned> skippedSections;
		m_files.clear();
		

		IndexFileXMLParser xmler(m_files, paths_substs, "", &log);
		XmlReader xmlReader(el_fdescr.c_str(), fsize);
		TD_BOOST_CHECK_MESSAGE (true == xmler.parseXMLRecursively(xmlReader, 0), "Error while parsing");
		vector<FileInfo_Unit> etalon;
		map<string, unsigned> map;
		FileInfo_Unit fi_test;
		fi_test.m_componentIdSet.push_back("KAVPDA55");
		fi_test.m_type = FileInfo::base;
		fi_test.m_relativeURLPath = "bases/av/klava/wince/";
		fi_test.m_filename = "stream.id";
		fi_test.m_signature = "0XLSznpdI71fB300e7Uwj17mxVCUmn2y7ftLMiHayGYyOkQe7yDz/RJhHM";
		fi_test.m_size = 80;
		fi_test.m_dateFromIndex = "03042006 1751";
		fi_test.m_originalLocalPath = "%BaseFolder%";
		fi_test.m_localPath = "/";
		fi_test.m_comment = "for KasperskyAV for Windows Workstations";
		fi_test.m_obligatoryEntry = false;
		fi_test.m_toAppVersion = "5.0.205";
		fi_test.m_toCompVersion = "5.0.255";
		etalon.push_back (fi_test);

		TD_BOOST_CHECK_MESSAGE (etalon == m_files, "Incorrect m_files");
		//TD_BOOST_CHECK_MESSAGE (skippedSections == map, "Incorrect skippedSections");
	}

	{
		string el_fdescr =
"<FileDescription \r\n\t\
ComponentID=\"KAVPDA55\" \r\n\t\
UpdateType=\"base\" \r\n\t\
RelativeSrvPath=\"bases/av/klava/wince\" \r\n\t\
Filename=\"stream.id\" \r\n\t\
FileSign=\"0XLSznpdI71fB300e7Uwj17mxVCUmn2y7ftLMiHayGYyOkQe7yDz/RJhHM\" \r\n\t\
FileSize=\"80\" \r\n\t\
LoadingOrder=\"154\" \r\n\t\
Set=\"normal\" \r\n\t\
LocalPath=\"%BaseFolder%\" \r\n\t\
ToAppVersion=\"5.0.205\" \r\n\t\
ToCompVersion=\"5.0.255\" \r\n\t\
Comment=\"for KasperskyAV for Windows Workstations\" \r\n\t\
UpdateObligation=\"optional\" \r\n\t\
SomeUnknownTag=\"value\" \r\n\t\
FileDate=\"03042006 1751\" />";

		std::cout<<"Some extra tags\n";
		int fsize = strlen(el_fdescr.c_str());
		map<string, unsigned> skippedSections;
		m_files.clear();
		

		IndexFileXMLParser xmler(m_files, paths_substs, "", &log);
		XmlReader xmlReader(el_fdescr.c_str(), fsize);
		TD_BOOST_CHECK_MESSAGE (true == xmler.parseXMLRecursively(xmlReader, 0), "Error while parsing");
		vector<FileInfo_Unit> etalon;
		map<string, unsigned> map;
		FileInfo_Unit fi_test;
		fi_test.m_componentIdSet.push_back("KAVPDA55");
		fi_test.m_type = FileInfo::base;
		fi_test.m_relativeURLPath = "bases/av/klava/wince/";
		fi_test.m_filename = "stream.id";
		fi_test.m_signature = "0XLSznpdI71fB300e7Uwj17mxVCUmn2y7ftLMiHayGYyOkQe7yDz/RJhHM";
		fi_test.m_size = 80;
		fi_test.m_dateFromIndex = "03042006 1751";
		fi_test.m_originalLocalPath = "%BaseFolder%";
		fi_test.m_localPath = "/";
		fi_test.m_comment = "for KasperskyAV for Windows Workstations";
		fi_test.m_obligatoryEntry = false;
		fi_test.m_toAppVersion = "5.0.205";
		fi_test.m_toCompVersion = "5.0.255";
		etalon.push_back (fi_test);

		TD_BOOST_CHECK_MESSAGE (etalon == m_files, "Incorrect m_files");
		//TD_BOOST_CHECK_MESSAGE (skippedSections == map, "Incorrect skippedSections");
	}

	{
		string el_fdescr =
"<FileDescription \r\n\t\
ComponentID=\"KAVPDA55\" \r\n\t\
UpdateType=\"base\" \r\n\t\
RelativeSrvPath=\"bases/av/klava/wince\" \r\n\t\
FileSign=\"0XLSznpdI71fB300e7Uwj17mxVCUmn2y7ftLMiHayGYyOkQe7yDz/RJhHM\" \r\n\t\
FileSize=\"80\" \r\n\t\
LoadingOrder=\"154\" \r\n\t\
Set=\"normal\" \r\n\t\
LocalPath=\"%BaseFolder%\" \r\n\t\
ToAppVersion=\"5.0.205\" \r\n\t\
ToCompVersion=\"5.0.255\" \r\n\t\
Comment=\"for KasperskyAV for Windows Workstations\" \r\n\t\
UpdateObligation=\"optional\" />";

		std::cout<<"Missing obligatory tags\n";
		int fsize = strlen(el_fdescr.c_str());
		map<string, unsigned> skippedSections;
		m_files.clear();
		

		IndexFileXMLParser xmler(m_files, paths_substs, "", &log);
		XmlReader xmlReader(el_fdescr.c_str(), fsize);
		TD_BOOST_CHECK_MESSAGE (false == xmler.parseXMLRecursively(xmlReader, 0), "Error while parsing");
		vector<FileInfo_Unit> etalon;
		map<string, unsigned> map;

		TD_BOOST_CHECK_MESSAGE (etalon == m_files, "Incorrect m_files");
		//TD_BOOST_CHECK_MESSAGE (skippedSections == map, "Incorrect skippedSections");
	}
	Step ("TestFileDescription", reason);
}

void Test_IndexFileXMLParser::TestUpdateFiles()
{
	string reason;

	std::cout<<"Test parsing UpdateFiles element\n";

	{
		string el_updf =
"<UpdateFiles \r\n\t\
	UpdateDate=\"27012006 1825\"> \r\n\r\n\r\n\r\n\
</UpdateFiles>";

		std::cout<<"Normal use\n";
		int fsize = strlen(el_updf.c_str());
		map<string, unsigned> skippedSections;
		m_files.clear();
		

		IndexFileXMLParser xmler(m_files, paths_substs, "", &log);
		XmlReader xmlReader(el_updf.c_str(), fsize);
		TD_BOOST_CHECK_MESSAGE (true == xmler.parseXMLRecursively(xmlReader, 0), "Error while parsing");
		vector<FileInfo_Unit> etalon;
		map<string, unsigned> map;


		TD_BOOST_CHECK_MESSAGE (etalon == m_files, "Incorrect m_files");
		//TD_BOOST_CHECK_MESSAGE (skippedSections == map, "Incorrect skippedSections");
//check		TD_BOOST_CHECK_MESSAGE (string("27012006 1825") == xmler.dateFromFile(), "Incorrect m_date");
	}

    {
		string el_updf =
"<UpdateFiles \r\n\t\
ExtraTagg=\"value\" \r\n\t\
UpdateDate=\"27012006 1825\"> \r\n\r\n\r\n\r\n\
</UpdateFiles>";

		std::cout<<"Some extra tags\n";
		int fsize = strlen(el_updf.c_str());
		map<string, unsigned> skippedSections;
		m_files.clear();
		
		m_date = "";

		IndexFileXMLParser xmler(m_files, paths_substs, "", &log);
		XmlReader xmlReader(el_updf.c_str(), fsize);
		TD_BOOST_CHECK_MESSAGE (true == xmler.parseXMLRecursively(xmlReader, 0), "Error while parsing");
		vector<FileInfo_Unit> etalon;
		map<string, unsigned> map;


		TD_BOOST_CHECK_MESSAGE (etalon == m_files, "Incorrect m_files");
		//TD_BOOST_CHECK_MESSAGE (skippedSections == map, "Incorrect skippedSections");
//check		TD_BOOST_CHECK_MESSAGE (string("27012006 1825") == xmler.dateFromFile(), "Incorrect m_date");
	}
	{
		string el_updf =
"<UpdateFiles \r\n\t\
ExtraTagg=\"value\" \r\n\r\n\r\n\r\n\
</UpdateFiles>";

		std::cout<<"Missing obligatory tags\n";
		int fsize = strlen(el_updf.c_str());
		map<string, unsigned> skippedSections;
		m_files.clear();
		
		m_date = "";

		IndexFileXMLParser xmler(m_files, paths_substs, "", &log);
		XmlReader xmlReader(el_updf.c_str(), fsize);
		TD_BOOST_CHECK_MESSAGE (false == xmler.parseXMLRecursively(xmlReader, 0), "Error while parsing");
		vector<FileInfo_Unit> etalon;
		map<string, unsigned> map;


		TD_BOOST_CHECK_MESSAGE (etalon == m_files, "Incorrect m_files");
		//TD_BOOST_CHECK_MESSAGE (skippedSections == map, "Incorrect skippedSections");
//check		TD_BOOST_CHECK_MESSAGE (string("") == xmler.dateFromFile(), "Incorrect m_date");
	}
	Step ("TestUpdateFiles", reason);
}

void Test_IndexFileXMLParser::TestOs()
{
	string reason;
	std::cout<<"Test parsing OS element\n";

	{
		string el_os =
"<FileDescription \
Filename=\"stream.id\" \r\n\t\
FileSize=\"80\" \r\n\t\
FileSign=\"0XLSznpdI71fB300e7Uwj17mxVCUmn2y7ftLMiHayGYyOkQe7yDz/RJhHM\">\
<OS Name=\"Windows\"/>\
</FileDescription>";


		std::cout<<"Normal use\n";
		int fsize = strlen(el_os.c_str());
		map<string, unsigned> skippedSections;
		m_files.clear();
		

		IndexFileXMLParser xmler(m_files, paths_substs, "", &log);
		XmlReader xmlReader(el_os.c_str(), fsize);
		vector<FileInfo_Unit> etalon;
		FileInfo_Unit fiu;
		fiu.m_filename = "stream.id";
		fiu.m_size = 80;
		fiu.m_signature = "0XLSznpdI71fB300e7Uwj17mxVCUmn2y7ftLMiHayGYyOkQe7yDz/RJhHM";
		fiu.m_type = FileInfo::base;
		fiu.m_localPath = "/";
		fiu.m_originalLocalPath = "%UpdateRoot%";
		FileInfo::OS osv;
		osv.m_name = "Windows";
		fiu.m_OSes.push_back (osv);
		etalon.push_back (fiu);
		map<string, unsigned> map;

		TD_BOOST_CHECK_MESSAGE (true == xmler.parseXMLRecursively(xmlReader, 0), "Error while parsing");
		TD_BOOST_CHECK_MESSAGE (etalon == m_files, "Incorrect m_files");
		//TD_BOOST_CHECK_MESSAGE (skippedSections == map, "Incorrect skippedSections");
	}
	{
		
				string el_os =
"<FileDescription \
Filename=\"stream.id\" \r\n\t\
FileSize=\"80\" \r\n\t\
FileSign=\"0XLSznpdI71fB300e7Uwj17mxVCUmn2y7ftLMiHayGYyOkQe7yDz/RJhHM\">\
<OS Name=\"Windows\" \r\n\t Unknown=\"value\"/>\
</FileDescription>";

		std::cout<<"Some extra tags\n";
		int fsize = strlen(el_os.c_str());
		map<string, unsigned> skippedSections;
		m_files.clear();
		

		IndexFileXMLParser xmler(m_files, paths_substs, "", &log);
		XmlReader xmlReader(el_os.c_str(), fsize);
		vector<FileInfo_Unit> etalon;
		FileInfo_Unit fiu;
		fiu.m_filename = "stream.id";
		fiu.m_size = 80;
		fiu.m_signature = "0XLSznpdI71fB300e7Uwj17mxVCUmn2y7ftLMiHayGYyOkQe7yDz/RJhHM";
		fiu.m_type = FileInfo::base;
		fiu.m_localPath = "/";
		fiu.m_originalLocalPath = "%UpdateRoot%";
		FileInfo::OS osv;
		osv.m_name = "Windows";
		fiu.m_OSes.push_back (osv);
		etalon.push_back (fiu);
		map<string, unsigned> map;

		TD_BOOST_CHECK_MESSAGE (true == xmler.parseXMLRecursively(xmlReader, 0), "Error while parsing");
		TD_BOOST_CHECK_MESSAGE (etalon == m_files, "Incorrect m_files");
		//TD_BOOST_CHECK_MESSAGE (skippedSections == map, "Incorrect skippedSections");
	}
	{

		string el_os = "<OS Name=\"Windows\"/>";
		std::cout<<"Specific case\n";
		int fsize = strlen(el_os.c_str());
		map<string, unsigned> skippedSections;
		m_files.clear();

		IndexFileXMLParser xmler(m_files, paths_substs, "", &log);
		XmlReader xmlReader(el_os.c_str(), fsize);
		vector<FileInfo_Unit> etalon;
		map<string, unsigned> map;

		TD_BOOST_CHECK_MESSAGE (true == xmler.parseXMLRecursively(xmlReader, 0), "Error while parsing");
		TD_BOOST_CHECK_MESSAGE (etalon == m_files, "Incorrect m_files");
		//TD_BOOST_CHECK_MESSAGE (skippedSections == map, "Incorrect skippedSections");
	}
	Step ("TestOs", reason);
	
};

void Test_IndexFileXMLParser::TestOsVer()
{
	string reason;
	std::cout<<"Test parsing OS versions element\n";

	{

		string el_osver =
"<FileDescription \
Filename=\"stream.id\" \r\n\t\
FileSize=\"80\" \r\n\t\
FileSign=\"0XLSznpdI71fB300e7Uwj17mxVCUmn2y7ftLMiHayGYyOkQe7yDz/RJhHM\">\
<OS Name=\"Windows\">\
<Version \r\n\t\
value=\"5.0\" \r\n\t\
value=\"6.0\" \r\n\t\
value=\"7.0\"/>\
</OS>\
</FileDescription>";

		std::cout<<"Normal use\n";
		int fsize = strlen(el_osver.c_str());
		map<string, unsigned> skippedSections;
		m_files.clear();
		

		IndexFileXMLParser xmler(m_files, paths_substs, "", &log);
		XmlReader xmlReader(el_osver.c_str(), fsize);
		vector<FileInfo_Unit> etalon;
		FileInfo_Unit fiu;
		fiu.m_filename = "stream.id";
		fiu.m_size = 80;
		fiu.m_signature = "0XLSznpdI71fB300e7Uwj17mxVCUmn2y7ftLMiHayGYyOkQe7yDz/RJhHM";
		fiu.m_type = FileInfo::base;
		fiu.m_localPath = "/";
		fiu.m_originalLocalPath = "%UpdateRoot%";
		FileInfo::OS osv;
		osv.m_name = "Windows";
		osv.m_versionSet.push_back("5.0");
		osv.m_versionSet.push_back("6.0");
		osv.m_versionSet.push_back("7.0");
		fiu.m_OSes.push_back (osv);
		etalon.push_back (fiu);
		map<string, unsigned> map;

		TD_BOOST_CHECK_MESSAGE (true == xmler.parseXMLRecursively(xmlReader, 0), "Error while parsing");
		TD_BOOST_CHECK_MESSAGE (etalon == m_files, "Incorrect m_files");
	}
	{
				string el_osver =
"<FileDescription \
Filename=\"stream.id\" \r\n\t\
FileSize=\"80\" \r\n\t\
FileSign=\"0XLSznpdI71fB300e7Uwj17mxVCUmn2y7ftLMiHayGYyOkQe7yDz/RJhHM\">\
<OS Name=\"Windows\">\
<Version \r\n\t\
value=\"5.0\" \r\n\t\
unknown=\"8.0\" \r\n\t\
value=\"6.0\" \r\n\t\
value=\"7.0\"/>\
</OS>\
</FileDescription>";

		std::cout<<"Some extra tags\n";
		int fsize = strlen(el_osver.c_str());
		map<string, unsigned> skippedSections;
		m_files.clear();
		

		IndexFileXMLParser xmler(m_files, paths_substs, "", &log);
		XmlReader xmlReader(el_osver.c_str(), fsize);
		vector<FileInfo_Unit> etalon;
			
		FileInfo_Unit fiu;
		fiu.m_filename = "stream.id";
		fiu.m_size = 80;
		fiu.m_signature = "0XLSznpdI71fB300e7Uwj17mxVCUmn2y7ftLMiHayGYyOkQe7yDz/RJhHM";
		fiu.m_type = FileInfo::base;
		fiu.m_localPath = "/";
		fiu.m_originalLocalPath = "%UpdateRoot%";

		FileInfo::OS osv;
		osv.m_name = "Windows";
		osv.m_versionSet.push_back("5.0");
		osv.m_versionSet.push_back("6.0");
		osv.m_versionSet.push_back("7.0");
		fiu.m_OSes.push_back (osv);
		etalon.push_back (fiu);
		map<string, unsigned> map;

		TD_BOOST_CHECK_MESSAGE (true == xmler.parseXMLRecursively(xmlReader, 0), "Error while parsing");
		TD_BOOST_CHECK_MESSAGE (etalon == m_files, "Incorrect m_files");
	}
	{
		string el_osver = "<Version \r\n\t\
value=\"5.0\" \r\n\t\
unknown=\"8.0\" \r\n\t\
value=\"6.0\" \r\n\t\
value=\"7.0\"/>";

		std::cout<<"Specific case #1\n";
		int fsize = strlen(el_osver.c_str());
		map<string, unsigned> skippedSections;
		m_files.clear();
		

		IndexFileXMLParser xmler(m_files, paths_substs, "", &log);
		XmlReader xmlReader(el_osver.c_str(), fsize);
		vector<FileInfo_Unit> etalon;
		FileInfo_Unit fiu;
		etalon.push_back (fiu);
		map<string, unsigned> map;
		FileInfo fi;
		//fi.OSes.push_back (OS());
		m_files.push_back (fi);

		TD_BOOST_CHECK_MESSAGE (true == xmler.parseXMLRecursively(xmlReader, 0), "Error while parsing");
		TD_BOOST_CHECK_MESSAGE (etalon == m_files, "Incorrect m_files");
		TD_BOOST_CHECK_MESSAGE (skippedSections == map, "Incorrect skippedSections");
	}
	{
		string el_osver = "<Version \r\n\t\
value=\"5.0\" \r\n\t\
unknown=\"8.0\" \r\n\t\
value=\"6.0\" \r\n\t\
value=\"7.0\"/>";

		std::cout<<"Specific case #2\n";
		int fsize = strlen(el_osver.c_str());
		map<string, unsigned> skippedSections;
		m_files.clear();
		

		IndexFileXMLParser xmler(m_files, paths_substs, "", &log);
		XmlReader xmlReader(el_osver.c_str(), fsize);
		vector<FileInfo_Unit> etalon;
		map<string, unsigned> map;
		FileInfo fi;
		//fi.OSes.push_back (OS());
		//m_files.push_back (fi);

		TD_BOOST_CHECK_MESSAGE (true == xmler.parseXMLRecursively(xmlReader, 0), "Error while parsing");
		TD_BOOST_CHECK_MESSAGE (etalon == m_files, "Incorrect m_files");
		TD_BOOST_CHECK_MESSAGE (skippedSections == map, "Incorrect skippedSections");
	}
	Step ("TestOsVer", reason);
};

void Test_IndexFileXMLParser::TestLocLang()
{
	string reason;
	std::cout<<"Test parsing LocLang element\n";

	{
		string el_loclang =
			"<FileDescription \
Filename=\"stream.id\" \r\n\t\
FileSize=\"80\" \r\n\t\
FileSign=\"0XLSznpdI71fB300e7Uwj17mxVCUmn2y7ftLMiHayGYyOkQe7yDz/RJhHM\">\
<LocLang\r\n\tLang=\"russian\"/>\
</FileDescription>";

		std::cout<<"Normal use\n";
		int fsize = strlen(el_loclang.c_str());
		map<string, unsigned> skippedSections;
		m_files.clear();
		

		IndexFileXMLParser xmler(m_files, paths_substs, "", &log);
		XmlReader xmlReader(el_loclang.c_str(), fsize);
		vector<FileInfo_Unit> etalon;
		FileInfo_Unit fiu;
		fiu.m_filename = "stream.id";
		fiu.m_size = 80;
		fiu.m_signature = "0XLSznpdI71fB300e7Uwj17mxVCUmn2y7ftLMiHayGYyOkQe7yDz/RJhHM";
		fiu.m_type = FileInfo::base;
		fiu.m_localPath = "/";
		fiu.m_originalLocalPath = "%UpdateRoot%";
		
		fiu.m_language.push_back ("russian");
		etalon.push_back (fiu);
		map<string, unsigned> map;

		TD_BOOST_CHECK_MESSAGE (true == xmler.parseXMLRecursively(xmlReader, 0), "Error while parsing");
		TD_BOOST_CHECK_MESSAGE (etalon == m_files, "Incorrect m_files");
	}
	{
		string el_loclang =
"<FileDescription \
Filename=\"stream.id\" \r\n\t\
FileSize=\"80\" \r\n\t\
FileSign=\"0XLSznpdI71fB300e7Uwj17mxVCUmn2y7ftLMiHayGYyOkQe7yDz/RJhHM\">\
<LocLang\r\n\tLang=\"russian\"\r\n\tLang=\"english\"\r\n\tUnknown=\"...\"/>\
</FileDescription>";
			
		std::cout<<"Some extra tags\n";
		int fsize = strlen(el_loclang.c_str());
		map<string, unsigned> skippedSections;
		m_files.clear();
		

		IndexFileXMLParser xmler(m_files, paths_substs, "", &log);
		XmlReader xmlReader(el_loclang.c_str(), fsize);
		vector<FileInfo_Unit> etalon;
		FileInfo_Unit fiu;
		fiu.m_filename = "stream.id";
		fiu.m_size = 80;
		fiu.m_signature = "0XLSznpdI71fB300e7Uwj17mxVCUmn2y7ftLMiHayGYyOkQe7yDz/RJhHM";
		fiu.m_type = FileInfo::base;
		fiu.m_localPath = "/";
		fiu.m_originalLocalPath = "%UpdateRoot%";
		fiu.m_language.push_back ("russian");
		etalon.push_back (fiu);
		map<string, unsigned> map;

		TD_BOOST_CHECK_MESSAGE (true == xmler.parseXMLRecursively(xmlReader, 0), "Error while parsing");
		TD_BOOST_CHECK_MESSAGE (etalon == m_files, "Incorrect m_files");
	}
	{
		string el_loclang = "<LocLang\r\n\tLang=\"russian\"\r\n\tLang=\"english\"\r\n\tUnknown=\"...\"/>";

		std::cout<<"Specific case\n";
		int fsize = strlen(el_loclang.c_str());
		map<string, unsigned> skippedSections;
		m_files.clear();
		

		IndexFileXMLParser xmler(m_files, paths_substs, "", &log);
		XmlReader xmlReader(el_loclang.c_str(), fsize);
		vector<FileInfo_Unit> etalon;
		map<string, unsigned> map;

		TD_BOOST_CHECK_MESSAGE (true == xmler.parseXMLRecursively(xmlReader, 0), "Error while parsing");
		TD_BOOST_CHECK_MESSAGE (etalon == m_files, "Incorrect m_files");
		TD_BOOST_CHECK_MESSAGE (skippedSections == map, "Incorrect skippedSections");
	}
	Step ("TestLocLang", reason);
};


void Test_IndexFileXMLParser::TestExtraSections()
{
	string reason;
	std::cout<<"Test parsing Extra Sections elements\n";

	{
		string el_extra = 
"<FileDescription \
Filename=\"stream.id\" \r\n\t\
FileSize=\"80\" \r\n\t\
FileSign=\"0XLSznpdI71fB300e7Uwj17mxVCUmn2y7ftLMiHayGYyOkQe7yDz/RJhHM\">\
<ExtraSection\r\n\tpar1=\"val1\"\r\n\tpar2=\"val2\"/>\r\n\t<ExtraSection2\r\n\tpar1=\"val1\"\r\n\tpar2=\"val2\"/>\
</FileDescription>";
			
		std::cout<<"Normal use\n";
		int fsize = strlen(el_extra.c_str());
		map<string, unsigned> skippedSections;
		m_files.clear();
		
		IndexFileXMLParser xmler(m_files, paths_substs, "", &log);
		XmlReader xmlReader(el_extra.c_str(), fsize);
		vector<FileInfo_Unit> etalon;
		map<string, unsigned> map;
		map["ExtraSection"] = 1;
		map["ExtraSection2"] = 1;

		TD_BOOST_CHECK_MESSAGE (true == xmler.parseXMLRecursively(xmlReader, 0), "Error while parsing");
	}
	{
		string el_extra = "<ExtraSection>\r\n\t<OS Name=\"Windows\"/> </ExtraSection>";
		
		std::cout<<"Normal section inside unknown\n";
		int fsize = strlen(el_extra.c_str());
		map<string, unsigned> skippedSections;
		m_files.clear();
		

		IndexFileXMLParser xmler(m_files, paths_substs, "", &log);
		XmlReader xmlReader(el_extra.c_str(), fsize);
		vector<FileInfo_Unit> etalon;
		map<string, unsigned> map;
		map["ExtraSection"] = 1;
		map["OS"] = 1;

		TD_BOOST_CHECK_MESSAGE (true == xmler.parseXMLRecursively(xmlReader, 0), "Error while parsing");
	}
	Step ("TestExtraSections", reason);
};


void Test_IndexFileXMLParser::TestFromApp()
{
	string reason;
	std::cout<<"Test parsing FromAppVersion Sections elements\n";

	{
		string el_app = 
"<FileDescription \
Filename=\"stream.id\" \r\n\t\
FileSize=\"80\" \r\n\t\
FileSign=\"0XLSznpdI71fB300e7Uwj17mxVCUmn2y7ftLMiHayGYyOkQe7yDz/RJhHM\">\
<FromAppVersion\r\n\t Version=\"5.0\"/>\
</FileDescription>";
		
		std::cout<<"Normal use\n";
		int fsize = strlen(el_app.c_str());
		map<string, unsigned> skippedSections;
		map<string, unsigned> map;
		m_files.clear();		

		IndexFileXMLParser xmler(m_files, paths_substs, "", &log);
		XmlReader xmlReader(el_app.c_str(), fsize);
		vector<FileInfo_Unit> etalon;
		FileInfo_Unit fiu;

		fiu.m_filename = "stream.id";
		fiu.m_size = 80;
		fiu.m_signature = "0XLSznpdI71fB300e7Uwj17mxVCUmn2y7ftLMiHayGYyOkQe7yDz/RJhHM";
		fiu.m_type = FileInfo::base;
		fiu.m_localPath = "/";
		fiu.m_originalLocalPath = "%UpdateRoot%";

		fiu.m_fromAppVersion.push_back ("5.0");
		etalon.push_back (fiu);

		TD_BOOST_CHECK_MESSAGE (true == xmler.parseXMLRecursively(xmlReader, 0), "Error while parsing");
		TD_BOOST_CHECK_MESSAGE (etalon == m_files, "Incorrect m_files");
	}
	{
		string el_app = 
"<FileDescription \
Filename=\"stream.id\" \r\n\t\
FileSize=\"80\" \r\n\t\
FileSign=\"0XLSznpdI71fB300e7Uwj17mxVCUmn2y7ftLMiHayGYyOkQe7yDz/RJhHM\">\
<FromAppVersion\r\n\t Version=\"5.0\"/><FromAppVersion\r\n\t Version=\"6.0\"/>\
</FileDescription>";

		std::cout<<"Several sections\n";
		int fsize = strlen(el_app.c_str());
		map<string, unsigned> skippedSections;
		map<string, unsigned> map;
		m_files.clear();
		

		IndexFileXMLParser xmler(m_files, paths_substs, "", &log);
		XmlReader xmlReader(el_app.c_str(), fsize);
		vector<FileInfo_Unit> etalon;
		FileInfo_Unit fiu;

		fiu.m_filename = "stream.id";
		fiu.m_size = 80;
		fiu.m_signature = "0XLSznpdI71fB300e7Uwj17mxVCUmn2y7ftLMiHayGYyOkQe7yDz/RJhHM";
		fiu.m_type = FileInfo::base;
		fiu.m_localPath = "/";
		fiu.m_originalLocalPath = "%UpdateRoot%";

		fiu.m_fromAppVersion.push_back ("5.0");
		fiu.m_fromAppVersion.push_back ("6.0");
		etalon.push_back (fiu);

		TD_BOOST_CHECK_MESSAGE (true == xmler.parseXMLRecursively(xmlReader, 0), "Error while parsing");
		TD_BOOST_CHECK_MESSAGE (etalon == m_files, "Incorrect m_files");
	}
	{
		string el_app = 
"<FileDescription \
Filename=\"stream.id\" \r\n\t\
FileSize=\"80\" \r\n\t\
FileSign=\"0XLSznpdI71fB300e7Uwj17mxVCUmn2y7ftLMiHayGYyOkQe7yDz/RJhHM\">\
<FromAppVersion\r\n\t Version=\"5.0\"/><FromAppVersion\r\n\t Version=\"5.0\"/>\
</FileDescription>";

		std::cout<<"Identical sections\n";
		int fsize = strlen(el_app.c_str());
		map<string, unsigned> skippedSections;
		map<string, unsigned> map;
		m_files.clear();
		
		

		IndexFileXMLParser xmler(m_files, paths_substs, "", &log);
		XmlReader xmlReader(el_app.c_str(), fsize);
		vector<FileInfo_Unit> etalon;
		FileInfo_Unit fiu;

		fiu.m_filename = "stream.id";
		fiu.m_size = 80;
		fiu.m_signature = "0XLSznpdI71fB300e7Uwj17mxVCUmn2y7ftLMiHayGYyOkQe7yDz/RJhHM";
		fiu.m_type = FileInfo::base;
		fiu.m_localPath = "/";
		fiu.m_originalLocalPath = "%UpdateRoot%";

		fiu.m_fromAppVersion.push_back ("5.0");
		fiu.m_fromAppVersion.push_back ("5.0");
		etalon.push_back (fiu);

		TD_BOOST_CHECK_MESSAGE (true == xmler.parseXMLRecursively(xmlReader, 0), "Error while parsing");
		TD_BOOST_CHECK_MESSAGE (etalon == m_files, "Incorrect m_files");
	}
	{
		string el_app = "<FromAppVersion\r\n\t Version=\"5.0\"/>";
		
		std::cout<<"Specific case\n";
		int fsize = strlen(el_app.c_str());
		map<string, unsigned> skippedSections;
		map<string, unsigned> map;
		m_files.clear();

		IndexFileXMLParser xmler(m_files, paths_substs, "", &log);
		XmlReader xmlReader(el_app.c_str(), fsize);
		vector<FileInfo_Unit> etalon;

		TD_BOOST_CHECK_MESSAGE (true == xmler.parseXMLRecursively(xmlReader, 0), "Error while parsing");
		TD_BOOST_CHECK_MESSAGE (etalon == m_files, "Incorrect m_files");
	}
	Step ("TestFromApp", reason);
};

void Test_IndexFileXMLParser::TestFromComp()
{
	string reason;
	std::cout<<"Test parsing FromCompVersion Sections elements\n";

	{
		string el_app =
"<FileDescription \
Filename=\"stream.id\" \r\n\t\
FileSize=\"80\" \r\n\t\
FileSign=\"0XLSznpdI71fB300e7Uwj17mxVCUmn2y7ftLMiHayGYyOkQe7yDz/RJhHM\">\
<FromCompVersion\r\n\t Version=\"5.0\"/>\
</FileDescription>";

		std::cout<<"Normal use\n";
		int fsize = strlen(el_app.c_str());
		map<string, unsigned> skippedSections;
		map<string, unsigned> map;
		m_files.clear();

		IndexFileXMLParser xmler(m_files, paths_substs, "", &log);
		XmlReader xmlReader(el_app.c_str(), fsize);
		vector<FileInfo_Unit> etalon;
		FileInfo_Unit fiu;

		fiu.m_filename = "stream.id";
		fiu.m_size = 80;
		fiu.m_signature = "0XLSznpdI71fB300e7Uwj17mxVCUmn2y7ftLMiHayGYyOkQe7yDz/RJhHM";
		fiu.m_type = FileInfo::base;
		fiu.m_localPath = "/";
		fiu.m_originalLocalPath = "%UpdateRoot%";

		fiu.m_fromCompVersion.push_back ("5.0");
		etalon.push_back (fiu);

		TD_BOOST_CHECK_MESSAGE (true == xmler.parseXMLRecursively(xmlReader, 0), "Error while parsing");
		TD_BOOST_CHECK_MESSAGE (etalon == m_files, "Incorrect m_files");
	}
	{
		string el_app =
"<FileDescription \
Filename=\"stream.id\" \r\n\t\
FileSize=\"80\" \r\n\t\
FileSign=\"0XLSznpdI71fB300e7Uwj17mxVCUmn2y7ftLMiHayGYyOkQe7yDz/RJhHM\">\
<FromCompVersion\r\n\t Version=\"5.0\"/><FromCompVersion\r\n\t Version=\"6.0\"/>\
</FileDescription>";
		
		std::cout<<"Several sections\n";
		int fsize = strlen(el_app.c_str());
		map<string, unsigned> skippedSections;
		map<string, unsigned> map;
		m_files.clear();
		
		

		IndexFileXMLParser xmler(m_files, paths_substs, "", &log);
		XmlReader xmlReader(el_app.c_str(), fsize);
		vector<FileInfo_Unit> etalon;
		FileInfo_Unit fiu;

		fiu.m_filename = "stream.id";
		fiu.m_size = 80;
		fiu.m_signature = "0XLSznpdI71fB300e7Uwj17mxVCUmn2y7ftLMiHayGYyOkQe7yDz/RJhHM";
		fiu.m_type = FileInfo::base;
		fiu.m_localPath = "/";
		fiu.m_originalLocalPath = "%UpdateRoot%";

		fiu.m_fromCompVersion.push_back ("5.0");
		fiu.m_fromCompVersion.push_back ("6.0");
		etalon.push_back (fiu);

		TD_BOOST_CHECK_MESSAGE (true == xmler.parseXMLRecursively(xmlReader, 0), "Error while parsing");
		TD_BOOST_CHECK_MESSAGE (etalon == m_files, "Incorrect m_files");
	}
	
	{
		string el_app = "<FromCompVersion\r\n\t Version=\"5.0\"/>";
		
		std::cout<<"Specific case\n";
		int fsize = strlen(el_app.c_str());
		map<string, unsigned> skippedSections;
		map<string, unsigned> map;
		m_files.clear();

		IndexFileXMLParser xmler(m_files, paths_substs, "", &log);
		XmlReader xmlReader(el_app.c_str(), fsize);
		vector<FileInfo_Unit> etalon;

		TD_BOOST_CHECK_MESSAGE (true == xmler.parseXMLRecursively(xmlReader, 0), "Error while parsing");
		TD_BOOST_CHECK_MESSAGE (etalon == m_files, "Incorrect m_files");
	}
	Step ("TestFromComp", reason);
};

Suite_IndexFileXMLParser::Suite_IndexFileXMLParser (const string& ini_file, const string& log_name)
{
	bool bLog;
	if (log_name == "") bLog =false; else bLog = true;

	boost::shared_ptr<Test_IndexFileXMLParser> instance (new Test_IndexFileXMLParser(ini_file, bLog, log_name));
	
	add (boost::unit_test::make_test_case<Test_IndexFileXMLParser>(&Test_IndexFileXMLParser::Start, "Test_IndexFileXMLParser::Start", instance));
	add (BOOST_CLASS_TEST_CASE (&Test_IndexFileXMLParser::TestFileDescription, instance));
	add (BOOST_CLASS_TEST_CASE (&Test_IndexFileXMLParser::TestUpdateFiles, instance));
	add (BOOST_CLASS_TEST_CASE (&Test_IndexFileXMLParser::TestOs, instance));
	add (BOOST_CLASS_TEST_CASE (&Test_IndexFileXMLParser::TestOsVer, instance));
	add (BOOST_CLASS_TEST_CASE (&Test_IndexFileXMLParser::TestLocLang, instance));
	add (BOOST_CLASS_TEST_CASE (&Test_IndexFileXMLParser::TestExtraSections, instance));
	add (BOOST_CLASS_TEST_CASE (&Test_IndexFileXMLParser::TestFromApp, instance));
	add (BOOST_CLASS_TEST_CASE (&Test_IndexFileXMLParser::TestFromComp, instance));
	add (boost::unit_test::make_test_case<Test_IndexFileXMLParser>(&Test_IndexFileXMLParser::End, "Test_IndexFileXMLParser::End", instance));
	

};