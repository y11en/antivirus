#include "util.h"

#include "log.h"
#include "callback.h"
#include "journal.h"
#include "core/updater.h"
#include "helper/local_file.h"
#include "helper/IndexFileXMLParser.h"

#include "TestSuiteSmallClasses.h"
#include "main.h"

using namespace KLUPD;
using namespace std;

#define PREPARE_DIRS\
	vector<wstring> dirs;\
	dirs.push_back (ROOT);\
	dirs.push_back (TRANS_ROOT);\
	dirs.push_back (TRANS_DEST);\
	dirs.push_back (TRANS_DEST_ETALON);\
	dirs.push_back (TRANS_SOURCE);\
	dirs.push_back (TRANS_SOURCE_ETALON);\
	dirs.push_back (TRANS_BACKUP);\
	dirs.push_back (TRANS_BACKUP_ETALON);\
	_helper::RecreateDirectories (dirs);

void TestSuiteSmallClasses::TestLocalFile ()
{
	std::wcout<<L"TestLocalFile\n";
	Logger::Case _case (m_logger, m_testp, m_step, L"TestLocalFile");

	{
		std::wcout<<L"Test copying a file\n";

		PREPARE_DIRS

		CopyFile (wstring(TRANS_STORAGE).append(L"\\unp000.avc").c_str(), wstring(TRANS_DEST_ETALON).append(L"\\unp000.avc").c_str(), false);
		CopyFile (wstring(TRANS_STORAGE).append(L"\\unp000.avc").c_str(), wstring(TRANS_SOURCE).append(L"\\unp000.avc").c_str(), false);
		CopyFile (wstring(TRANS_STORAGE).append(L"\\unp000.avc").c_str(), wstring(TRANS_SOURCE_ETALON).append(L"\\unp000.avc").c_str(), false);

		{
			LocalFile locFile_src (wstring(TRANS_SOURCE).append(L"\\unp000.avc").c_str(), &testLog);		
			QC_BOOST_CHECK_MESSAGE (CORE_NO_ERROR == locFile_src.copy ( wstring(TRANS_DEST).append(L"\\unp000.avc").c_str() ), L"Copying file");
			}

		QC_BOOST_CHECK_MESSAGE (_helper::IsFoldersEqual (DIFFTOOL_ROOT, TRANS_DEST_ETALON, TRANS_DEST), L"Destination folder");
		QC_BOOST_CHECK_MESSAGE (_helper::IsFoldersEqual (DIFFTOOL_ROOT, TRANS_SOURCE_ETALON, TRANS_SOURCE), L"Source folder");	
	};

	{
		std::wcout<<L"Test copying a file when the target one already exists\n";

		PREPARE_DIRS

		CopyFile (wstring(TRANS_STORAGE).append(L"\\unp000.avc").c_str(), wstring(TRANS_DEST_ETALON).append(L"\\unp000.avc").c_str(), false);
		CopyFile (wstring(TRANS_STORAGE).append(L"\\unp001.avc").c_str(), wstring(TRANS_DEST).append(L"\\unp000.avc").c_str(), false);
		CopyFile (wstring(TRANS_STORAGE).append(L"\\unp000.avc").c_str(), wstring(TRANS_SOURCE).append(L"\\unp000.avc").c_str(), false);
		CopyFile (wstring(TRANS_STORAGE).append(L"\\unp000.avc").c_str(), wstring(TRANS_SOURCE_ETALON).append(L"\\unp000.avc").c_str(), false);

		{
			LocalFile locFile_src (wstring(TRANS_SOURCE).append(L"\\unp000.avc").c_str(), &testLog);
			QC_BOOST_CHECK_MESSAGE (CORE_NO_ERROR == locFile_src.copy ( wstring(TRANS_DEST).append(L"\\unp000.avc").c_str() ), L"Copying file");
		}

		QC_BOOST_CHECK_MESSAGE (_helper::IsFoldersEqual (DIFFTOOL_ROOT, TRANS_DEST_ETALON, TRANS_DEST), L"Destination folder");
		QC_BOOST_CHECK_MESSAGE (_helper::IsFoldersEqual (DIFFTOOL_ROOT, TRANS_SOURCE_ETALON, TRANS_SOURCE), L"Source folder");	
	};


	{
		std::wcout<<L"Test deleting a file successfully\n";

		PREPARE_DIRS

		CopyFile (wstring(TRANS_STORAGE).append(L"\\unp000.avc").c_str(), wstring(TRANS_DEST).append(L"\\unp000.avc").c_str(), false);
		
		{
			LocalFile locFile_src (wstring(TRANS_DEST).append(L"\\unp000.avc").c_str(), &testLog);
			QC_BOOST_CHECK_MESSAGE (CORE_NO_ERROR == locFile_src.unlink(), L"Deleting file");
		}

		QC_BOOST_CHECK_MESSAGE (_helper::IsFoldersEqual (DIFFTOOL_ROOT, TRANS_DEST_ETALON, TRANS_DEST), L"Destination folder");
		QC_BOOST_CHECK_MESSAGE (_helper::IsFoldersEqual (DIFFTOOL_ROOT, TRANS_SOURCE_ETALON, TRANS_SOURCE), L"Source folder");	

	};

	{
		std::wcout<<L"Test deleteting a file when it is locked\n";

		PREPARE_DIRS

		CopyFile (wstring(TRANS_STORAGE).append(L"\\unp000.avc").c_str(), wstring(TRANS_DEST_ETALON).append(L"\\unp000.avc").c_str(), false);
		CopyFile (wstring(TRANS_STORAGE).append(L"\\unp000.avc").c_str(), wstring(TRANS_DEST).append(L"\\unp000.avc").c_str(), false);
		
		{
			LocalFile locFile_src (wstring(TRANS_DEST).append(L"\\unp000.avc").c_str(), &testLog);
			
			_helper::AutoFile hFile (wstring(TRANS_DEST).append(L"\\unp000.avc").c_str(), GENERIC_READ, FILE_SHARE_READ, OPEN_EXISTING);
			
			QC_BOOST_CHECK_MESSAGE (CORE_NotEnoughPermissions == locFile_src.unlink(), L"Deleting file");
		}

		QC_BOOST_CHECK_MESSAGE (_helper::IsFoldersEqual (DIFFTOOL_ROOT, TRANS_DEST_ETALON, TRANS_DEST), L"Destination folder");
		QC_BOOST_CHECK_MESSAGE (_helper::IsFoldersEqual (DIFFTOOL_ROOT, TRANS_SOURCE_ETALON, TRANS_SOURCE), L"Source folder");	

	};

	{
		std::wcout<<L"Test renaming a file sucessfully\n";

		PREPARE_DIRS

		CopyFile (wstring(TRANS_STORAGE).append(L"\\unp000.avc").c_str(), wstring(TRANS_DEST_ETALON).append(L"\\unp001.avc").c_str(), false);
		CopyFile (wstring(TRANS_STORAGE).append(L"\\unp000.avc").c_str(), wstring(TRANS_DEST).append(L"\\unp000.avc").c_str(), false);
		
		{
			LocalFile locFile_src (wstring(TRANS_DEST).append(L"\\unp000.avc").c_str(), &testLog);
			
			QC_BOOST_CHECK_MESSAGE (CORE_NO_ERROR == locFile_src.rename (wstring(TRANS_DEST).append(L"\\unp001.avc").c_str()), L"Renaming file");
		}

		QC_BOOST_CHECK_MESSAGE (_helper::IsFoldersEqual (DIFFTOOL_ROOT, TRANS_DEST_ETALON, TRANS_DEST), L"Destination folder");
		
	};

	{
		std::wcout<<L"Test renaming a file when it is locked\n";

		PREPARE_DIRS

		CopyFile (wstring(TRANS_STORAGE).append(L"\\unp000.avc").c_str(), wstring(TRANS_DEST_ETALON).append(L"\\unp000.avc").c_str(), false);
		CopyFile (wstring(TRANS_STORAGE).append(L"\\unp000.avc").c_str(), wstring(TRANS_DEST).append(L"\\unp000.avc").c_str(), false);
		
		{
			LocalFile locFile_src (wstring(TRANS_DEST).append(L"\\unp000.avc").c_str(), &testLog);
			_helper::AutoFile hFile  (wstring(TRANS_DEST).append(L"\\unp000.avc").c_str(), GENERIC_READ, FILE_SHARE_READ, OPEN_EXISTING);
			
			QC_BOOST_CHECK_MESSAGE (CORE_NotEnoughPermissions == locFile_src.rename (wstring(TRANS_DEST).append(L"\\unp001.avc").c_str()), L"Renaming file");
		}

		QC_BOOST_CHECK_MESSAGE (_helper::IsFoldersEqual (DIFFTOOL_ROOT, TRANS_DEST_ETALON, TRANS_DEST), L"Destination folder");

	};

	{
		std::wcout<<L"Test existence of a file when it exists\n";

		PREPARE_DIRS

		CopyFile (wstring(TRANS_STORAGE).append(L"\\unp000.avc").c_str(), wstring(TRANS_DEST).append(L"\\unp000.avc").c_str(), false);
		
		LocalFile locFile_src (wstring(TRANS_DEST).append(L"\\unp000.avc").c_str(), &testLog);
			
		QC_BOOST_CHECK_MESSAGE (locFile_src.exists(), L"Checking existence of a file");

	};

	{
		std::wcout<<L"Test existence of a file when it is missed\n";

		PREPARE_DIRS
		
		LocalFile locFile_src (wstring(TRANS_DEST).append(L"\\unp000.avc").c_str(), &testLog);
			
		QC_BOOST_CHECK_MESSAGE (!locFile_src.exists(), L"Checking existence of a file");
	};
}
/*
void TestSuiteSmallClasses::TestIndexFileXMLParser()
{

	{
		std::wcout<<L"Test parsing FileDescription element\n";

		{
			wstring el_fdescr =
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

			std::wcout<<L"Normal use\n";
			int fsize = wcslen(el_fdescr.c_str());
			map<wstring, unsigned> skippedSections;
			m_files.clear();
			

			IndexFileXMLParser xmler(m_files, s6c, L"", false, tCallback, &testLog);
			XmlReader xmlReader(el_fdescr.c_str(), fsize);
			QC_BOOST_CHECK_MESSAGE (true == xmler.parseXMLRecursively(xmlReader, 0), L"Error while parsing");
			vector<FileInfo_Unit> etalon;
			map<wstring, unsigned> map;
			FileInfo_Unit fi_test;
			fi_test.m_componentIdSet.push_back(L"KAVPDA55");
			fi_test.m_type = FileInfo::base;
			fi_test.m_relativeURLPath = L"bases/av/klava/wince/";
			fi_test.m_filename = L"stream.id";
			fi_test.m_signature5 = L"0XLSznpdI71fB300e7Uwj17mxVCUmn2y7ftLMiHayGYyOkQe7yDz/RJhHM";
			fi_test.m_size = 80;
			fi_test.m_dateFromIndex = L"03042006 1751";
			fi_test.m_originalLocalPath = L"%BaseFolder%";
			fi_test.m_localPath = L"/";
			fi_test.m_comment = L"for KasperskyAV for Windows Workstations";
			fi_test.m_obligatoryEntry = false;
			fi_test.m_toAppVersion = L"5.0.205";
			//fi_test.m_toCompVersion = L"5.0.255";
			etalon.push_back (fi_test);

			QC_BOOST_CHECK_MESSAGE (etalon == m_files, L"Incorrect m_files");
		}

		{
			wstring el_fdescr =
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

			std::wcout<<L"Some extra tags\n";
			int fsize = wcslen(el_fdescr.c_str());
			map<wstring, unsigned> skippedSections;
			m_files.clear();
			

			IndexFileXMLParser xmler(m_files, paths_substs, L"", &log);
			XmlReader xmlReader(el_fdescr.c_str(), fsize);
			QC_BOOST_CHECK_MESSAGE (true == xmler.parseXMLRecursively(xmlReader, 0), L"Error while parsing");
			vector<FileInfo_Unit> etalon;
			map<wstring, unsigned> map;
			FileInfo_Unit fi_test;
			fi_test.m_componentIdSet.push_back(L"KAVPDA55");
			fi_test.m_type = FileInfo::base;
			fi_test.m_relativeURLPath = L"bases/av/klava/wince/";
			fi_test.m_filename = L"stream.id";
			fi_test.m_signature = L"0XLSznpdI71fB300e7Uwj17mxVCUmn2y7ftLMiHayGYyOkQe7yDz/RJhHM";
			fi_test.m_size = 80;
			fi_test.m_dateFromIndex = L"03042006 1751";
			fi_test.m_originalLocalPath = L"%BaseFolder%";
			fi_test.m_localPath = L"/";
			fi_test.m_comment = L"for KasperskyAV for Windows Workstations";
			fi_test.m_obligatoryEntry = false;
			fi_test.m_toAppVersion = L"5.0.205";
			fi_test.m_toCompVersion = L"5.0.255";
			etalon.push_back (fi_test);

			QC_BOOST_CHECK_MESSAGE (etalon == m_files, L"Incorrect m_files");
		}

		{
			wstring el_fdescr =
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

			std::wcout<<L"Missing obligatory tags\n";
			int fsize = wcslen(el_fdescr.c_str());
			map<wstring, unsigned> skippedSections;
			m_files.clear();
			

			IndexFileXMLParser xmler(m_files, paths_substs, L"", &log);
			XmlReader xmlReader(el_fdescr.c_str(), fsize);
			QC_BOOST_CHECK_MESSAGE (false == xmler.parseXMLRecursively(xmlReader, 0), L"Error while parsing");
			vector<FileInfo_Unit> etalon;
			map<wstring, unsigned> map;

			QC_BOOST_CHECK_MESSAGE (etalon == m_files, L"Incorrect m_files");
			//QC_BOOST_CHECK_MESSAGE (skippedSections == map, L"Incorrect skippedSections");
		}
	}

	{
		std::wcout<<L"Test parsing UpdateFiles element\n";

		{
			wstring el_updf =
	"<UpdateFiles \r\n\t\
		UpdateDate=\"27012006 1825\"> \r\n\r\n\r\n\r\n\
	</UpdateFiles>";

			std::wcout<<L"Normal use\n";
			int fsize = wcslen(el_updf.c_str());
			map<wstring, unsigned> skippedSections;
			m_files.clear();
			

			IndexFileXMLParser xmler(m_files, paths_substs, L"", &log);
			XmlReader xmlReader(el_updf.c_str(), fsize);
			QC_BOOST_CHECK_MESSAGE (true == xmler.parseXMLRecursively(xmlReader, 0), L"Error while parsing");
			vector<FileInfo_Unit> etalon;
			map<wstring, unsigned> map;


			QC_BOOST_CHECK_MESSAGE (etalon == m_files, L"Incorrect m_files");
	//check		QC_BOOST_CHECK_MESSAGE (wstring(L"27012006 1825") == xmler.dateFromFile(), L"Incorrect m_date");
		}

		{
			wstring el_updf =
	"<UpdateFiles \r\n\t\
	ExtraTagg=\"value\" \r\n\t\
	UpdateDate=\"27012006 1825\"> \r\n\r\n\r\n\r\n\
	</UpdateFiles>";

			std::wcout<<L"Some extra tags\n";
			int fsize = wcslen(el_updf.c_str());
			map<wstring, unsigned> skippedSections;
			m_files.clear();
			
			m_date = L"";

			IndexFileXMLParser xmler(m_files, paths_substs, L"", &log);
			XmlReader xmlReader(el_updf.c_str(), fsize);
			QC_BOOST_CHECK_MESSAGE (true == xmler.parseXMLRecursively(xmlReader, 0), L"Error while parsing");
			vector<FileInfo_Unit> etalon;
			map<wstring, unsigned> map;


			QC_BOOST_CHECK_MESSAGE (etalon == m_files, L"Incorrect m_files");
	//check		QC_BOOST_CHECK_MESSAGE (wstring(L"27012006 1825") == xmler.dateFromFile(), L"Incorrect m_date");
		}
		{
			wstring el_updf =
	"<UpdateFiles \r\n\t\
	ExtraTagg=\"value\" \r\n\r\n\r\n\r\n\
	</UpdateFiles>";

			std::wcout<<L"Missing obligatory tags\n";
			int fsize = wcslen(el_updf.c_str());
			map<wstring, unsigned> skippedSections;
			m_files.clear();
			
			m_date = L"";

			IndexFileXMLParser xmler(m_files, paths_substs, L"", &log);
			XmlReader xmlReader(el_updf.c_str(), fsize);
			QC_BOOST_CHECK_MESSAGE (false == xmler.parseXMLRecursively(xmlReader, 0), L"Error while parsing");
			vector<FileInfo_Unit> etalon;
			map<wstring, unsigned> map;


			QC_BOOST_CHECK_MESSAGE (etalon == m_files, L"Incorrect m_files");
	//check		QC_BOOST_CHECK_MESSAGE (wstring(L"") == xmler.dateFromFile(), L"Incorrect m_date");
		}
	}

	{
		std::wcout<<L"Test parsing OS element\n";

		{
			wstring el_os =
	"<FileDescription \
	Filename=\"stream.id\" \r\n\t\
	FileSize=\"80\" \r\n\t\
	FileSign=\"0XLSznpdI71fB300e7Uwj17mxVCUmn2y7ftLMiHayGYyOkQe7yDz/RJhHM\">\
	<OS Name=\"Windows\"/>\
	</FileDescription>";


			std::wcout<<L"Normal use\n";
			int fsize = wcslen(el_os.c_str());
			map<wstring, unsigned> skippedSections;
			m_files.clear();
			

			IndexFileXMLParser xmler(m_files, paths_substs, L"", &log);
			XmlReader xmlReader(el_os.c_str(), fsize);
			vector<FileInfo_Unit> etalon;
			FileInfo_Unit fiu;
			fiu.m_filename = L"stream.id";
			fiu.m_size = 80;
			fiu.m_signature = L"0XLSznpdI71fB300e7Uwj17mxVCUmn2y7ftLMiHayGYyOkQe7yDz/RJhHM";
			fiu.m_type = FileInfo::base;
			fiu.m_localPath = L"/";
			fiu.m_originalLocalPath = L"%UpdateRoot%";
			FileInfo::OS osv;
			osv.m_name = L"Windows";
			fiu.m_OSes.push_back (osv);
			etalon.push_back (fiu);
			map<wstring, unsigned> map;

			QC_BOOST_CHECK_MESSAGE (true == xmler.parseXMLRecursively(xmlReader, 0), L"Error while parsing");
			QC_BOOST_CHECK_MESSAGE (etalon == m_files, L"Incorrect m_files");
		}
		{
			
					wstring el_os =
	"<FileDescription \
	Filename=\"stream.id\" \r\n\t\
	FileSize=\"80\" \r\n\t\
	FileSign=\"0XLSznpdI71fB300e7Uwj17mxVCUmn2y7ftLMiHayGYyOkQe7yDz/RJhHM\">\
	<OS Name=\"Windows\" \r\n\t Unknown=\"value\"/>\
	</FileDescription>";

			std::wcout<<L"Some extra tags\n";
			int fsize = wcslen(el_os.c_str());
			map<wstring, unsigned> skippedSections;
			m_files.clear();
			

			IndexFileXMLParser xmler(m_files, paths_substs, L"", &log);
			XmlReader xmlReader(el_os.c_str(), fsize);
			vector<FileInfo_Unit> etalon;
			FileInfo_Unit fiu;
			fiu.m_filename = L"stream.id";
			fiu.m_size = 80;
			fiu.m_signature = L"0XLSznpdI71fB300e7Uwj17mxVCUmn2y7ftLMiHayGYyOkQe7yDz/RJhHM";
			fiu.m_type = FileInfo::base;
			fiu.m_localPath = L"/";
			fiu.m_originalLocalPath = L"%UpdateRoot%";
			FileInfo::OS osv;
			osv.m_name = L"Windows";
			fiu.m_OSes.push_back (osv);
			etalon.push_back (fiu);
			map<wstring, unsigned> map;

			QC_BOOST_CHECK_MESSAGE (true == xmler.parseXMLRecursively(xmlReader, 0), L"Error while parsing");
			QC_BOOST_CHECK_MESSAGE (etalon == m_files, L"Incorrect m_files");
		}
		{
			wstring el_os = L"<OS Name=\"Windows\"/>";
			std::wcout<<L"Specific case\n";
			int fsize = wcslen(el_os.c_str());
			map<wstring, unsigned> skippedSections;
			m_files.clear();

			IndexFileXMLParser xmler(m_files, paths_substs, L"", &log);
			XmlReader xmlReader(el_os.c_str(), fsize);
			vector<FileInfo_Unit> etalon;
			map<wstring, unsigned> map;

			QC_BOOST_CHECK_MESSAGE (true == xmler.parseXMLRecursively(xmlReader, 0), L"Error while parsing");
			QC_BOOST_CHECK_MESSAGE (etalon == m_files, L"Incorrect m_files");
		}
		
	};

	{
		std::wcout<<L"Test parsing OS versions element\n";

		{

			wstring el_osver =
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

			std::wcout<<L"Normal use\n";
			int fsize = wcslen(el_osver.c_str());
			map<wstring, unsigned> skippedSections;
			m_files.clear();
			

			IndexFileXMLParser xmler(m_files, paths_substs, L"", &log);
			XmlReader xmlReader(el_osver.c_str(), fsize);
			vector<FileInfo_Unit> etalon;
			FileInfo_Unit fiu;
			fiu.m_filename = L"stream.id";
			fiu.m_size = 80;
			fiu.m_signature = L"0XLSznpdI71fB300e7Uwj17mxVCUmn2y7ftLMiHayGYyOkQe7yDz/RJhHM";
			fiu.m_type = FileInfo::base;
			fiu.m_localPath = L"/";
			fiu.m_originalLocalPath = L"%UpdateRoot%";
			FileInfo::OS osv;
			osv.m_name = L"Windows";
			osv.m_versionSet.push_back(L"5.0");
			osv.m_versionSet.push_back(L"6.0");
			osv.m_versionSet.push_back(L"7.0");
			fiu.m_OSes.push_back (osv);
			etalon.push_back (fiu);
			map<wstring, unsigned> map;

			QC_BOOST_CHECK_MESSAGE (true == xmler.parseXMLRecursively(xmlReader, 0), L"Error while parsing");
			QC_BOOST_CHECK_MESSAGE (etalon == m_files, L"Incorrect m_files");
		}
		{
					wstring el_osver =
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

			std::wcout<<L"Some extra tags\n";
			int fsize = wcslen(el_osver.c_str());
			map<wstring, unsigned> skippedSections;
			m_files.clear();
			

			IndexFileXMLParser xmler(m_files, paths_substs, L"", &log);
			XmlReader xmlReader(el_osver.c_str(), fsize);
			vector<FileInfo_Unit> etalon;
				
			FileInfo_Unit fiu;
			fiu.m_filename = L"stream.id";
			fiu.m_size = 80;
			fiu.m_signature = L"0XLSznpdI71fB300e7Uwj17mxVCUmn2y7ftLMiHayGYyOkQe7yDz/RJhHM";
			fiu.m_type = FileInfo::base;
			fiu.m_localPath = L"/";
			fiu.m_originalLocalPath = L"%UpdateRoot%";

			FileInfo::OS osv;
			osv.m_name = L"Windows";
			osv.m_versionSet.push_back(L"5.0");
			osv.m_versionSet.push_back(L"6.0");
			osv.m_versionSet.push_back(L"7.0");
			fiu.m_OSes.push_back (osv);
			etalon.push_back (fiu);
			map<wstring, unsigned> map;

			QC_BOOST_CHECK_MESSAGE (true == xmler.parseXMLRecursively(xmlReader, 0), L"Error while parsing");
			QC_BOOST_CHECK_MESSAGE (etalon == m_files, L"Incorrect m_files");
		}
		{
			wstring el_osver = L"<Version \r\n\t\
	value=\"5.0\" \r\n\t\
	unknown=\"8.0\" \r\n\t\
	value=\"6.0\" \r\n\t\
	value=\"7.0\"/>";

			std::wcout<<L"Specific case #1\n";
			int fsize = wcslen(el_osver.c_str());
			map<wstring, unsigned> skippedSections;
			m_files.clear();
			

			IndexFileXMLParser xmler(m_files, paths_substs, L"", &log);
			XmlReader xmlReader(el_osver.c_str(), fsize);
			vector<FileInfo_Unit> etalon;
			FileInfo_Unit fiu;
			etalon.push_back (fiu);
			map<wstring, unsigned> map;
			FileInfo fi;
			//fi.OSes.push_back (OS());
			m_files.push_back (fi);

			QC_BOOST_CHECK_MESSAGE (true == xmler.parseXMLRecursively(xmlReader, 0), L"Error while parsing");
			QC_BOOST_CHECK_MESSAGE (etalon == m_files, L"Incorrect m_files");
			QC_BOOST_CHECK_MESSAGE (skippedSections == map, L"Incorrect skippedSections");
		}
		{
			wstring el_osver = L"<Version \r\n\t\
	value=\"5.0\" \r\n\t\
	unknown=\"8.0\" \r\n\t\
	value=\"6.0\" \r\n\t\
	value=\"7.0\"/>";

			std::wcout<<L"Specific case #2\n";
			int fsize = wcslen(el_osver.c_str());
			map<wstring, unsigned> skippedSections;
			m_files.clear();
			

			IndexFileXMLParser xmler(m_files, paths_substs, L"", &log);
			XmlReader xmlReader(el_osver.c_str(), fsize);
			vector<FileInfo_Unit> etalon;
			map<wstring, unsigned> map;
			FileInfo fi;
			//fi.OSes.push_back (OS());
			//m_files.push_back (fi);

			QC_BOOST_CHECK_MESSAGE (true == xmler.parseXMLRecursively(xmlReader, 0), L"Error while parsing");
			QC_BOOST_CHECK_MESSAGE (etalon == m_files, L"Incorrect m_files");
			QC_BOOST_CHECK_MESSAGE (skippedSections == map, L"Incorrect skippedSections");
		}
	};

	{
		std::wcout<<L"Test parsing LocLang element\n";

		{
			wstring el_loclang =
				"<FileDescription \
	Filename=\"stream.id\" \r\n\t\
	FileSize=\"80\" \r\n\t\
	FileSign=\"0XLSznpdI71fB300e7Uwj17mxVCUmn2y7ftLMiHayGYyOkQe7yDz/RJhHM\">\
	<LocLang\r\n\tLang=\"russian\"/>\
	</FileDescription>";

			std::wcout<<L"Normal use\n";
			int fsize = wcslen(el_loclang.c_str());
			map<wstring, unsigned> skippedSections;
			m_files.clear();
			

			IndexFileXMLParser xmler(m_files, paths_substs, L"", &log);
			XmlReader xmlReader(el_loclang.c_str(), fsize);
			vector<FileInfo_Unit> etalon;
			FileInfo_Unit fiu;
			fiu.m_filename = L"stream.id";
			fiu.m_size = 80;
			fiu.m_signature = L"0XLSznpdI71fB300e7Uwj17mxVCUmn2y7ftLMiHayGYyOkQe7yDz/RJhHM";
			fiu.m_type = FileInfo::base;
			fiu.m_localPath = L"/";
			fiu.m_originalLocalPath = L"%UpdateRoot%";
			
			fiu.m_language.push_back (L"russian");
			etalon.push_back (fiu);
			map<wstring, unsigned> map;

			QC_BOOST_CHECK_MESSAGE (true == xmler.parseXMLRecursively(xmlReader, 0), L"Error while parsing");
			QC_BOOST_CHECK_MESSAGE (etalon == m_files, L"Incorrect m_files");
		}
		{
			wstring el_loclang =
	"<FileDescription \
	Filename=\"stream.id\" \r\n\t\
	FileSize=\"80\" \r\n\t\
	FileSign=\"0XLSznpdI71fB300e7Uwj17mxVCUmn2y7ftLMiHayGYyOkQe7yDz/RJhHM\">\
	<LocLang\r\n\tLang=\"russian\"\r\n\tLang=\"english\"\r\n\tUnknown=\"...\"/>\
	</FileDescription>";
				
			std::wcout<<L"Some extra tags\n";
			int fsize = wcslen(el_loclang.c_str());
			map<wstring, unsigned> skippedSections;
			m_files.clear();
			

			IndexFileXMLParser xmler(m_files, paths_substs, L"", &log);
			XmlReader xmlReader(el_loclang.c_str(), fsize);
			vector<FileInfo_Unit> etalon;
			FileInfo_Unit fiu;
			fiu.m_filename = L"stream.id";
			fiu.m_size = 80;
			fiu.m_signature = L"0XLSznpdI71fB300e7Uwj17mxVCUmn2y7ftLMiHayGYyOkQe7yDz/RJhHM";
			fiu.m_type = FileInfo::base;
			fiu.m_localPath = L"/";
			fiu.m_originalLocalPath = L"%UpdateRoot%";
			fiu.m_language.push_back (L"russian");
			etalon.push_back (fiu);
			map<wstring, unsigned> map;

			QC_BOOST_CHECK_MESSAGE (true == xmler.parseXMLRecursively(xmlReader, 0), L"Error while parsing");
			QC_BOOST_CHECK_MESSAGE (etalon == m_files, L"Incorrect m_files");
		}
		{
			wstring el_loclang = L"<LocLang\r\n\tLang=\"russian\"\r\n\tLang=\"english\"\r\n\tUnknown=\"...\"/>";

			std::wcout<<L"Specific case\n";
			int fsize = wcslen(el_loclang.c_str());
			map<wstring, unsigned> skippedSections;
			m_files.clear();
			

			IndexFileXMLParser xmler(m_files, paths_substs, L"", &log);
			XmlReader xmlReader(el_loclang.c_str(), fsize);
			vector<FileInfo_Unit> etalon;
			map<wstring, unsigned> map;

			QC_BOOST_CHECK_MESSAGE (true == xmler.parseXMLRecursively(xmlReader, 0), L"Error while parsing");
			QC_BOOST_CHECK_MESSAGE (etalon == m_files, L"Incorrect m_files");
			QC_BOOST_CHECK_MESSAGE (skippedSections == map, L"Incorrect skippedSections");
		}
	};

	{
		std::wcout<<L"Test parsing Extra Sections elements\n";

		{
			wstring el_extra = 
	"<FileDescription \
	Filename=\"stream.id\" \r\n\t\
	FileSize=\"80\" \r\n\t\
	FileSign=\"0XLSznpdI71fB300e7Uwj17mxVCUmn2y7ftLMiHayGYyOkQe7yDz/RJhHM\">\
	<ExtraSection\r\n\tpar1=\"val1\"\r\n\tpar2=\"val2\"/>\r\n\t<ExtraSection2\r\n\tpar1=\"val1\"\r\n\tpar2=\"val2\"/>\
	</FileDescription>";
				
			std::wcout<<L"Normal use\n";
			int fsize = wcslen(el_extra.c_str());
			map<wstring, unsigned> skippedSections;
			m_files.clear();
			
			IndexFileXMLParser xmler(m_files, paths_substs, L"", &log);
			XmlReader xmlReader(el_extra.c_str(), fsize);
			vector<FileInfo_Unit> etalon;
			map<wstring, unsigned> map;
			map["ExtraSection"] = 1;
			map["ExtraSection2"] = 1;

			QC_BOOST_CHECK_MESSAGE (true == xmler.parseXMLRecursively(xmlReader, 0), L"Error while parsing");
		}
		{
			wstring el_extra = L"<ExtraSection>\r\n\t<OS Name=\"Windows\"/> </ExtraSection>";
			
			std::wcout<<L"Normal section inside unknown\n";
			int fsize = wcslen(el_extra.c_str());
			map<wstring, unsigned> skippedSections;
			m_files.clear();
			

			IndexFileXMLParser xmler(m_files, paths_substs, L"", &log);
			XmlReader xmlReader(el_extra.c_str(), fsize);
			vector<FileInfo_Unit> etalon;
			map<wstring, unsigned> map;
			map["ExtraSection"] = 1;
			map["OS"] = 1;

			QC_BOOST_CHECK_MESSAGE (true == xmler.parseXMLRecursively(xmlReader, 0), L"Error while parsing");
		}
	};

	{
		wstring reason;
		std::wcout<<L"Test parsing FromAppVersion Sections elements\n";

		{
			wstring el_app = 
	"<FileDescription \
	Filename=\"stream.id\" \r\n\t\
	FileSize=\"80\" \r\n\t\
	FileSign=\"0XLSznpdI71fB300e7Uwj17mxVCUmn2y7ftLMiHayGYyOkQe7yDz/RJhHM\">\
	<FromAppVersion\r\n\t Version=\"5.0\"/>\
	</FileDescription>";
			
			std::wcout<<L"Normal use\n";
			int fsize = wcslen(el_app.c_str());
			map<wstring, unsigned> skippedSections;
			map<wstring, unsigned> map;
			m_files.clear();		

			IndexFileXMLParser xmler(m_files, paths_substs, L"", &log);
			XmlReader xmlReader(el_app.c_str(), fsize);
			vector<FileInfo_Unit> etalon;
			FileInfo_Unit fiu;

			fiu.m_filename = L"stream.id";
			fiu.m_size = 80;
			fiu.m_signature = L"0XLSznpdI71fB300e7Uwj17mxVCUmn2y7ftLMiHayGYyOkQe7yDz/RJhHM";
			fiu.m_type = FileInfo::base;
			fiu.m_localPath = L"/";
			fiu.m_originalLocalPath = L"%UpdateRoot%";

			fiu.m_fromAppVersion.push_back (L"5.0");
			etalon.push_back (fiu);

			QC_BOOST_CHECK_MESSAGE (true == xmler.parseXMLRecursively(xmlReader, 0), L"Error while parsing");
			QC_BOOST_CHECK_MESSAGE (etalon == m_files, L"Incorrect m_files");
		}
		{
			wstring el_app = 
	"<FileDescription \
	Filename=\"stream.id\" \r\n\t\
	FileSize=\"80\" \r\n\t\
	FileSign=\"0XLSznpdI71fB300e7Uwj17mxVCUmn2y7ftLMiHayGYyOkQe7yDz/RJhHM\">\
	<FromAppVersion\r\n\t Version=\"5.0\"/><FromAppVersion\r\n\t Version=\"6.0\"/>\
	</FileDescription>";

			std::wcout<<L"Several sections\n";
			int fsize = wcslen(el_app.c_str());
			map<wstring, unsigned> skippedSections;
			map<wstring, unsigned> map;
			m_files.clear();
			

			IndexFileXMLParser xmler(m_files, paths_substs, L"", &log);
			XmlReader xmlReader(el_app.c_str(), fsize);
			vector<FileInfo_Unit> etalon;
			FileInfo_Unit fiu;

			fiu.m_filename = L"stream.id";
			fiu.m_size = 80;
			fiu.m_signature = L"0XLSznpdI71fB300e7Uwj17mxVCUmn2y7ftLMiHayGYyOkQe7yDz/RJhHM";
			fiu.m_type = FileInfo::base;
			fiu.m_localPath = L"/";
			fiu.m_originalLocalPath = L"%UpdateRoot%";

			fiu.m_fromAppVersion.push_back (L"5.0");
			fiu.m_fromAppVersion.push_back (L"6.0");
			etalon.push_back (fiu);

			QC_BOOST_CHECK_MESSAGE (true == xmler.parseXMLRecursively(xmlReader, 0), L"Error while parsing");
			QC_BOOST_CHECK_MESSAGE (etalon == m_files, L"Incorrect m_files");
		}
		{
			wstring el_app = 
	"<FileDescription \
	Filename=\"stream.id\" \r\n\t\
	FileSize=\"80\" \r\n\t\
	FileSign=\"0XLSznpdI71fB300e7Uwj17mxVCUmn2y7ftLMiHayGYyOkQe7yDz/RJhHM\">\
	<FromAppVersion\r\n\t Version=\"5.0\"/><FromAppVersion\r\n\t Version=\"5.0\"/>\
	</FileDescription>";

			std::wcout<<L"Identical sections\n";
			int fsize = wcslen(el_app.c_str());
			map<wstring, unsigned> skippedSections;
			map<wstring, unsigned> map;
			m_files.clear();
			
			

			IndexFileXMLParser xmler(m_files, paths_substs, L"", &log);
			XmlReader xmlReader(el_app.c_str(), fsize);
			vector<FileInfo_Unit> etalon;
			FileInfo_Unit fiu;

			fiu.m_filename = L"stream.id";
			fiu.m_size = 80;
			fiu.m_signature = L"0XLSznpdI71fB300e7Uwj17mxVCUmn2y7ftLMiHayGYyOkQe7yDz/RJhHM";
			fiu.m_type = FileInfo::base;
			fiu.m_localPath = L"/";
			fiu.m_originalLocalPath = L"%UpdateRoot%";

			fiu.m_fromAppVersion.push_back (L"5.0");
			fiu.m_fromAppVersion.push_back (L"5.0");
			etalon.push_back (fiu);

			QC_BOOST_CHECK_MESSAGE (true == xmler.parseXMLRecursively(xmlReader, 0), L"Error while parsing");
			QC_BOOST_CHECK_MESSAGE (etalon == m_files, L"Incorrect m_files");
		}
		{
			wstring el_app = L"<FromAppVersion\r\n\t Version=\"5.0\"/>";
			
			std::wcout<<L"Specific case\n";
			int fsize = wcslen(el_app.c_str());
			map<wstring, unsigned> skippedSections;
			map<wstring, unsigned> map;
			m_files.clear();

			IndexFileXMLParser xmler(m_files, paths_substs, L"", &log);
			XmlReader xmlReader(el_app.c_str(), fsize);
			vector<FileInfo_Unit> etalon;

			QC_BOOST_CHECK_MESSAGE (true == xmler.parseXMLRecursively(xmlReader, 0), L"Error while parsing");
			QC_BOOST_CHECK_MESSAGE (etalon == m_files, L"Incorrect m_files");
		}
	};

	{
		wstring reason;
		std::wcout<<L"Test parsing FromCompVersion Sections elements\n";

		{
			wstring el_app =
	"<FileDescription \
	Filename=\"stream.id\" \r\n\t\
	FileSize=\"80\" \r\n\t\
	FileSign=\"0XLSznpdI71fB300e7Uwj17mxVCUmn2y7ftLMiHayGYyOkQe7yDz/RJhHM\">\
	<FromCompVersion\r\n\t Version=\"5.0\"/>\
	</FileDescription>";

			std::wcout<<L"Normal use\n";
			int fsize = wcslen(el_app.c_str());
			map<wstring, unsigned> skippedSections;
			map<wstring, unsigned> map;
			m_files.clear();

			IndexFileXMLParser xmler(m_files, paths_substs, L"", &log);
			XmlReader xmlReader(el_app.c_str(), fsize);
			vector<FileInfo_Unit> etalon;
			FileInfo_Unit fiu;

			fiu.m_filename = L"stream.id";
			fiu.m_size = 80;
			fiu.m_signature = L"0XLSznpdI71fB300e7Uwj17mxVCUmn2y7ftLMiHayGYyOkQe7yDz/RJhHM";
			fiu.m_type = FileInfo::base;
			fiu.m_localPath = L"/";
			fiu.m_originalLocalPath = L"%UpdateRoot%";

			fiu.m_fromCompVersion.push_back (L"5.0");
			etalon.push_back (fiu);

			QC_BOOST_CHECK_MESSAGE (true == xmler.parseXMLRecursively(xmlReader, 0), L"Error while parsing");
			QC_BOOST_CHECK_MESSAGE (etalon == m_files, L"Incorrect m_files");
		}
		{
			wstring el_app =
	"<FileDescription \
	Filename=\"stream.id\" \r\n\t\
	FileSize=\"80\" \r\n\t\
	FileSign=\"0XLSznpdI71fB300e7Uwj17mxVCUmn2y7ftLMiHayGYyOkQe7yDz/RJhHM\">\
	<FromCompVersion\r\n\t Version=\"5.0\"/><FromCompVersion\r\n\t Version=\"6.0\"/>\
	</FileDescription>";
			
			std::wcout<<L"Several sections\n";
			int fsize = wcslen(el_app.c_str());
			map<wstring, unsigned> skippedSections;
			map<wstring, unsigned> map;
			m_files.clear();
			
			

			IndexFileXMLParser xmler(m_files, paths_substs, L"", &log);
			XmlReader xmlReader(el_app.c_str(), fsize);
			vector<FileInfo_Unit> etalon;
			FileInfo_Unit fiu;

			fiu.m_filename = L"stream.id";
			fiu.m_size = 80;
			fiu.m_signature = L"0XLSznpdI71fB300e7Uwj17mxVCUmn2y7ftLMiHayGYyOkQe7yDz/RJhHM";
			fiu.m_type = FileInfo::base;
			fiu.m_localPath = L"/";
			fiu.m_originalLocalPath = L"%UpdateRoot%";

			fiu.m_fromCompVersion.push_back (L"5.0");
			fiu.m_fromCompVersion.push_back (L"6.0");
			etalon.push_back (fiu);

			QC_BOOST_CHECK_MESSAGE (true == xmler.parseXMLRecursively(xmlReader, 0), L"Error while parsing");
			QC_BOOST_CHECK_MESSAGE (etalon == m_files, L"Incorrect m_files");
		}
		
		{
			wstring el_app = L"<FromCompVersion\r\n\t Version=\"5.0\"/>";
			
			std::wcout<<L"Specific case\n";
			int fsize = wcslen(el_app.c_str());
			map<wstring, unsigned> skippedSections;
			map<wstring, unsigned> map;
			m_files.clear();

			IndexFileXMLParser xmler(m_files, paths_substs, L"", &log);
			XmlReader xmlReader(el_app.c_str(), fsize);
			vector<FileInfo_Unit> etalon;

			QC_BOOST_CHECK_MESSAGE (true == xmler.parseXMLRecursively(xmlReader, 0), L"Error while parsing");
			QC_BOOST_CHECK_MESSAGE (etalon == m_files, L"Incorrect m_files");
		}
	};
}
*/