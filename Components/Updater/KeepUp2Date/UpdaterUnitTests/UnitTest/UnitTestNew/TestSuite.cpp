#include "util.h"

#include "log.h"
#include "callback.h"
#include "journal.h"
#include "core/updater.h"
#include "helper/local_file.h"
#include "HttpServer.h"

#include "TestSuite.h"
#include "TestSuiteProxyDetector.h"
#include "main.h"


void TestSuite::TestCheckFilesToDownload ()
{
	using namespace KLUPD;
	std::wcout<<L"TestCheckFilesToDownload\n";
	Logger::Case _case (m_logger, m_testp, m_step, L"TestCheckFilesToDownload");


	{
		std::wcout<<L"Check transaction status\n";

		TestLog testLog;

		_helper::DeleteDirectory(ROOT);
		KLUPD::createFolder (BASES_ROOT, &testLog);
		KLUPD::createFolder (UPD_ROOT, &testLog);

		_helper::CopyFolder(std::wstring(COLL_ROOT).append(L"\\Bases_NULL\\plain").c_str(),BASES_ROOT);
		_helper::CloneFile (std::wstring(COLL_ROOT).append(L"\\Bases_NULL\\index\\6\\u0607g.xml").c_str(),
			std::wstring (UPD_ROOT).append (L"\\u0607g.xml"), false);

		UpdaterConfiguration data(1, 1, L"1");

		data.temporary_dir = TEMP_ROOT;
		//data.reserve_dir = RESERVE_ROOT;
		//data.update_root_folder = UPD_ROOT;

		data.m_pathSubstitutionMap.push_back(std::make_pair<NoCaseString,NoCaseString>(L"%BaseFolder%", BASES_ROOT));
		data.m_pathSubstitutionMap.push_back(std::make_pair<NoCaseString,NoCaseString>(L"%Folder%", ROLLBACK_ROOT));
		data.m_pathSubstitutionMap.push_back(std::make_pair<NoCaseString,NoCaseString>(L"%UpdateRoot%", UPD_ROOT));
		data.m_pathSubstitutionMap.push_back(std::make_pair<NoCaseString,NoCaseString>(L"%DSKM%", DSKM_ROOT));
		data.m_pathSubstitutionMap.push_back(std::make_pair<NoCaseString,NoCaseString>(L"%DataFolder%", UPD_ROOT));

		data.self_region = L"<default>";
		data.product_lang.push_back( NoCaseString(L"<default>"));
		
		data.os.os = L"Windows";
		data.os.version = L"5.1";

		data.m_componentsToUpdate.push_back ( L"AVS" );
		data.m_componentsToUpdate.push_back (NoCaseString(L"CORE") );
		data.connect_tmo = 30;

		data.m_sourceList.addSource (UST_UserURL, std::wstring(COLL_ROOT).append(L"\\Bases_1").c_str(), true, L"diffs");
		//data.m_sourceList.addSource (UST_UserURL, L"ftp://d5x.kaspersky-labs.com", true, L"diffs");
		//data.proxy_url = L"tl-prx-lnx";
		//data.proxy_port = 3128;
		//data.use_ie_proxy = true;
		//data.m_httpAuthorizationMethods.AddAuthMethod (AUT_Basic, true);
		data.UpdaterListFlags.update_bases = true;

		Callbacks cbacks;
		TestCallbackInterface testCallback (&testLog, &cbacks);
		TestJournalInterface testJournal;
		testCallback.SetUpdaterConfiguration (data);
		
		Updater upd (testCallback.m_dp, testCallback, testJournal, &testLog);
		HANDLE hDoUpdate;

		cbacks.bFlags[Callbacks::checkFilesToDownload] = true;
		CREATE_EVENTS(checkFilesToDownload)

		doUpdater do_upd(&upd, hDoUpdate);

		WAIT_FOR_READY_EVENT(checkFilesToDownload)
		TestCallbackInterface::_checkFilesToDownload& cfd = testCallback.GetCheckFilesToDownload();
		std::wcout<<cfd.m_matchFileList->size()<<L"\n";
		for (std::vector<FileInfo>::iterator it = cfd.m_matchFileList->begin();
				it!=cfd.m_matchFileList->end(); ++it)
				{
					std::wcout<<it->m_filename.m_value.toWideChar()<<L"\n";
				}

		QC_BOOST_REQUIRE_MESSAGE (cfd.m_matchFileList->size() == 11, L"Incorrect file list size\n");
		
		QC_BOOST_CHECK_MESSAGE (cfd.m_matchFileList->at(6).m_filename == L"base101.avc", L"Incorrect file name\n");
		QC_BOOST_CHECK_MESSAGE (cfd.m_matchFileList->at(7).m_filename == L"base102.avc", L"Incorrect file name\n");
		QC_BOOST_CHECK_MESSAGE (cfd.m_matchFileList->at(8).m_filename == L"base103.avc", L"Incorrect file name\n");
		//QC_BOOST_CHECK_MESSAGE (cfd.m_matchFileList->at(11).m_filename == L"base100.avc", L"Incorrect file name\n");

		QC_BOOST_CHECK_MESSAGE (cfd.m_matchFileList->at(6).m_transactionInformation.m_changeStatus == FileInfo::unchanged, L"Incorrect transaction status\n");
		QC_BOOST_CHECK_MESSAGE (cfd.m_matchFileList->at(7).m_transactionInformation.m_changeStatus == FileInfo::modified, L"Incorrect transaction status\n");
		QC_BOOST_CHECK_MESSAGE (cfd.m_matchFileList->at(8).m_transactionInformation.m_changeStatus == FileInfo::added, L"Incorrect transaction status\n");
		//QC_BOOST_CHECK_MESSAGE (cfd.m_matchFileList->at(11).m_transactionInformation.m_changeStatus == FileInfo::deleted, L"Incorrect transaction status\n");
		
		SET_GOON_EVENT(checkFilesToDownload);

		WaitForSingleObject (hDoUpdate, INFINITE);

		QC_BOOST_CHECK_MESSAGE (_helper::DeleteFile(std::wstring(BASES_ROOT).append (L"\\base100.avc")),
								L"old base file not found");
		QC_BOOST_CHECK_MESSAGE (_helper::IsFoldersEqual(DIFFTOOL_ROOT,
			std::wstring(COLL_ROOT).append(L"\\Bases_1\\plain").c_str(), BASES_ROOT), L"Incorrect bases\n");

		QC_BOOST_CHECK_MESSAGE (_helper::IsFoldersEqual(DIFFTOOL_ROOT,
			std::wstring(COLL_ROOT).append(L"\\Bases_1\\data").c_str(), UPD_ROOT), L"Incorrect index\n");


		QC_BOOST_CHECK_MESSAGE ( NO_ERROR == do_upd.GetError(), L"Wrong return code\n" );
	}

	{
		std::wcout<<L"Change transaction status\n";

		TestLog testLog;

		_helper::DeleteDirectory(ROOT);
		KLUPD::createFolder (BASES_ROOT, &testLog);
		_helper::CopyFolder(std::wstring(COLL_ROOT).append(L"\\Bases_NULL\\plain").c_str(),BASES_ROOT);

		UpdaterConfiguration data(1, 1, L"1");

		data.temporary_dir = TEMP_ROOT;
		//data.reserve_dir = RESERVE_ROOT;

		data.m_pathSubstitutionMap.push_back(std::make_pair<NoCaseString,NoCaseString>(L"%BaseFolder%", BASES_ROOT));
		data.m_pathSubstitutionMap.push_back(std::make_pair<NoCaseString,NoCaseString>(L"%Folder%", ROLLBACK_ROOT));
		data.m_pathSubstitutionMap.push_back(std::make_pair<NoCaseString,NoCaseString>(L"%UpdateRoot%", UPD_ROOT));
		data.m_pathSubstitutionMap.push_back(std::make_pair<NoCaseString,NoCaseString>(L"%DSKM%", DSKM_ROOT));
		data.m_pathSubstitutionMap.push_back(std::make_pair<NoCaseString,NoCaseString>(L"%DataFolder%", UPD_ROOT));

		data.self_region = L"<default>";
		data.product_lang.push_back( NoCaseString(L"<default>"));
		
		data.os.os = L"Windows";
		data.os.version = L"5.1";

		data.m_componentsToUpdate.push_back (L"AVS");
		data.m_componentsToUpdate.push_back (L"CORE");
		data.connect_tmo = 30;

		data.m_sourceList.addSource (UST_UserURL, std::wstring(COLL_ROOT).append(L"\\Bases_1").c_str(), true, L"diffs");
		//data.m_sourceList.addSource (UST_UserURL, L"ftp://d5x.kaspersky-labs.com", true, L"diffs");
		//data.proxy_url = L"tl-prx-lnx";
		//data.proxy_port = 3128;
		//data.use_ie_proxy = true;
		//data.m_httpAuthorizationMethods.AddAuthMethod (AUT_Basic, true);
		data.UpdaterListFlags.update_bases = true;

		Callbacks cbacks;
		TestCallbackInterface testCallback (&testLog, &cbacks);
		TestJournalInterface testJournal;
		testCallback.SetUpdaterConfiguration (data);
		
		Updater upd (testCallback.m_dp, testCallback, testJournal, &testLog);
		HANDLE hDoUpdate;

		cbacks.bFlags[Callbacks::checkFilesToDownload] = true;
		CREATE_EVENTS(checkFilesToDownload)

		doUpdater do_upd(&upd, hDoUpdate);

		WAIT_FOR_READY_EVENT(checkFilesToDownload)
		TestCallbackInterface::_checkFilesToDownload& cfd = testCallback.GetCheckFilesToDownload();
		//std::wcout<<cfd.m_matchFileList->size()<<L"\n";
		//for (std::vector<FileInfo>::iterator it = cfd.m_matchFileList->begin();
		//		it!=cfd.m_matchFileList->end(); ++it)
		//		{
		//			std::wcout<<it->m_filename.m_value.toWideChar()<<L"\n";
		//		}

		QC_BOOST_REQUIRE_MESSAGE (cfd.m_matchFileList->size() == 11, L"Incorrect file list size\n");

		for (std::vector<FileInfo>::iterator it = cfd.m_matchFileList->begin();
				it!=cfd.m_matchFileList->end(); ++it)

		it->m_transactionInformation.m_changeStatus = FileInfo::unchanged;
		
		SET_GOON_EVENT(checkFilesToDownload)

		WaitForSingleObject (hDoUpdate, INFINITE);

		QC_BOOST_CHECK_MESSAGE (_helper::IsFoldersEqual(DIFFTOOL_ROOT,
			std::wstring(COLL_ROOT).append(L"\\Bases_NULL\\plain").c_str(), BASES_ROOT), L"Incorrect bases\n");

		QC_BOOST_CHECK_MESSAGE ( NO_ERROR == do_upd.GetError(), L"Wrong return code\n" );
	}

{
		std::wcout<<L"Check answer result\n";

		TestLog testLog;

		_helper::DeleteDirectory(ROOT);

		UpdaterConfiguration data(1, 1, L"1");

		data.temporary_dir = TEMP_ROOT;
		//data.reserve_dir = RESERVE_ROOT;

		data.m_pathSubstitutionMap.push_back(std::make_pair<NoCaseString,NoCaseString>(L"%BaseFolder%", BASES_ROOT));
		data.m_pathSubstitutionMap.push_back(std::make_pair<NoCaseString,NoCaseString>(L"%Folder%", ROLLBACK_ROOT));
		data.m_pathSubstitutionMap.push_back(std::make_pair<NoCaseString,NoCaseString>(L"%UpdateRoot%", UPD_ROOT));
		data.m_pathSubstitutionMap.push_back(std::make_pair<NoCaseString,NoCaseString>(L"%DSKM%", DSKM_ROOT));
		data.m_pathSubstitutionMap.push_back(std::make_pair<NoCaseString,NoCaseString>(L"%DataFolder%", UPD_ROOT));

		data.self_region = L"<default>";
		data.product_lang.push_back( NoCaseString(L"<default>"));
		
		data.os.os = L"Windows";
		data.os.version = L"5.1";

		data.m_componentsToUpdate.push_back (L"AVS");
		data.m_componentsToUpdate.push_back (L"CORE");
		data.connect_tmo = 30;

		data.m_sourceList.addSource (UST_UserURL, std::wstring(COLL_ROOT).append(L"\\Bases_1").c_str(), true, L"diffs");
		//data.m_sourceList.addSource (UST_UserURL, L"ftp://d5x.kaspersky-labs.com", true, L"diffs");
		//data.proxy_url = L"tl-prx-lnx";
		//data.proxy_port = 3128;
		//data.use_ie_proxy = true;
		//data.m_httpAuthorizationMethods.AddAuthMethod (AUT_Basic, true);
		data.UpdaterListFlags.update_bases = true;

		Callbacks cbacks;
		TestCallbackInterface testCallback (&testLog, &cbacks);
		TestJournalInterface testJournal;
		testCallback.SetUpdaterConfiguration (data);
		
		Updater upd (testCallback.m_dp, testCallback, testJournal, &testLog);
		HANDLE hDoUpdate;

		cbacks.bFlags[Callbacks::checkFilesToDownload] = true;
		CREATE_EVENTS(checkFilesToDownload)

		doUpdater do_upd(&upd, hDoUpdate);

		WAIT_FOR_READY_EVENT(checkFilesToDownload)
		TestCallbackInterface::_checkFilesToDownload& cfd = testCallback.GetCheckFilesToDownload();
		//cfd.result = KLUPD::CORE_CANCELLED;
		cfd.result = false;

		SET_GOON_EVENT(checkFilesToDownload);

		WaitForSingleObject (hDoUpdate, INFINITE);

		QC_BOOST_CHECK_MESSAGE ( CORE_BASE_CHECK_FAILED == do_upd.GetError(), L"Wrong return code\n" );
	}
}

void TestSuite::TestCheckIfFileOptional ()
{
	using namespace KLUPD;
	std::wcout<<L"TestCheckIfFileOptional\n";
	Logger::Case _case (m_logger, m_testp, m_step, L"TestCheckIfFileOptional");
	{
		std::wcout<<L"Optional component\n";

		TestLog testLog;

		_helper::DeleteDirectory(ROOT);

		UpdaterConfiguration data(1, 1, L"1");

		data.temporary_dir = TEMP_ROOT;
		//data.reserve_dir = RESERVE_ROOT;

		data.m_pathSubstitutionMap.push_back(std::make_pair<NoCaseString,NoCaseString>(L"%BaseFolder%", BASES_ROOT));
		data.m_pathSubstitutionMap.push_back(std::make_pair<NoCaseString,NoCaseString>(L"%Folder%", ROLLBACK_ROOT));
		data.m_pathSubstitutionMap.push_back(std::make_pair<NoCaseString,NoCaseString>(L"%UpdateRoot%", UPD_ROOT));
		data.m_pathSubstitutionMap.push_back(std::make_pair<NoCaseString,NoCaseString>(L"%DSKM%", DSKM_ROOT));
		data.m_pathSubstitutionMap.push_back(std::make_pair<NoCaseString,NoCaseString>(L"%DataFolder%", UPD_ROOT));

		data.self_region = L"<default>";
		data.product_lang.push_back( NoCaseString(L"<default>"));
		
		data.os.os = L"Windows";
		data.os.version = L"5.1";

		data.m_componentsToUpdate.push_back (L"AVS");
		data.m_componentsToUpdate.push_back (L"AH");
		data.m_componentsToUpdate.push_back (L"CORE");

		data.connect_tmo = 30;

		data.m_sourceList.addSource (UST_UserURL, std::wstring(COLL_ROOT).append(L"\\Bases_missed").c_str(), true, L"diffs");
		//data.m_sourceList.addSource (UST_UserURL, L"ftp://d5x.kaspersky-labs.com", true, L"diffs");
		//data.proxy_url = L"tl-prx-lnx";
		//data.proxy_port = 3128;
		//data.use_ie_proxy = true;
		//data.m_httpAuthorizationMethods.AddAuthMethod (AUT_Basic, true);
		data.UpdaterListFlags.update_bases = true;

		Callbacks cbacks;
		TestCallbackInterface testCallback (&testLog, &cbacks);
		TestJournalInterface testJournal;
		testCallback.SetUpdaterConfiguration (data);
		
		Updater upd (testCallback.m_dp, testCallback, testJournal, &testLog);
		HANDLE hDoUpdate;

		cbacks.bFlags[Callbacks::checkIfFileOptional] = true;
		CREATE_EVENTS(checkIfFileOptional)
		//cbacks.bFlags[Callbacks::componentRemovedFromUpdate] = true;
		//CREATE_EVENTS(componentRemovedFromUpdate)
		
		testCallback.SetOptionalFileAnswer(true);

		doUpdater do_upd(&upd, hDoUpdate);

		WAIT_FOR_READY_EVENT(checkIfFileOptional)
		QC_BOOST_CHECK_MESSAGE (testCallback.GetAbsentFile().m_filename == L"ah-0607g.xml", L"Incorrect absent filename\n");
		SET_GOON_EVENT(checkIfFileOptional);
		WAIT_FOR_READY_EVENT(checkIfFileOptional)
		QC_BOOST_CHECK_MESSAGE (testCallback.GetAbsentFile().m_filename == L"ids1.kdz", L"Incorrect absent filename\n");
		SET_GOON_EVENT(checkIfFileOptional);
		WAIT_FOR_READY_EVENT(checkIfFileOptional)
		QC_BOOST_CHECK_MESSAGE (testCallback.GetAbsentFile().m_filename == L"ids2.kdz", L"Incorrect absent filename\n");
		SET_GOON_EVENT(checkIfFileOptional);
		WAIT_FOR_READY_EVENT(checkIfFileOptional)
		QC_BOOST_CHECK_MESSAGE (testCallback.GetAbsentFile().m_filename == L"ids3.kdz", L"Incorrect absent filename\n");
		SET_GOON_EVENT(checkIfFileOptional);

		WaitForSingleObject (hDoUpdate, INFINITE);

		QC_BOOST_CHECK_MESSAGE (_helper::IsFoldersEqual(DIFFTOOL_ROOT,
			std::wstring(COLL_ROOT).append(L"\\Bases_missed\\plain").c_str(), BASES_ROOT), L"Incorrect bases\n");

		QC_BOOST_CHECK_MESSAGE ( NO_ERROR == do_upd.GetError(), L"Wrong return code\n" );
	}

	{
		std::wcout<<L"Mandatory component\n";

		TestLog testLog;

		_helper::DeleteDirectory(ROOT);

		UpdaterConfiguration data(1, 1, L"1");

		data.temporary_dir = TEMP_ROOT;
		//data.reserve_dir = RESERVE_ROOT;

		data.m_pathSubstitutionMap.push_back(std::make_pair<NoCaseString,NoCaseString>(L"%BaseFolder%", BASES_ROOT));
		data.m_pathSubstitutionMap.push_back(std::make_pair<NoCaseString,NoCaseString>(L"%Folder%", ROLLBACK_ROOT));
		data.m_pathSubstitutionMap.push_back(std::make_pair<NoCaseString,NoCaseString>(L"%UpdateRoot%", UPD_ROOT));
		data.m_pathSubstitutionMap.push_back(std::make_pair<NoCaseString,NoCaseString>(L"%DSKM%", DSKM_ROOT));
		data.m_pathSubstitutionMap.push_back(std::make_pair<NoCaseString,NoCaseString>(L"%DataFolder%", UPD_ROOT));

		data.self_region = L"<default>";
		data.product_lang.push_back( NoCaseString(L"<default>"));
		
		data.os.os = L"Windows";
		data.os.version = L"5.1";

		data.m_componentsToUpdate.push_back (L"AVS");
		data.m_componentsToUpdate.push_back (L"AH");
		data.m_componentsToUpdate.push_back (L"CORE");

		data.connect_tmo = 30;

		data.m_sourceList.addSource (UST_UserURL, std::wstring(COLL_ROOT).append(L"\\Bases_missed").c_str(), true, L"diffs");
		//data.m_sourceList.addSource (UST_UserURL, L"ftp://d5x.kaspersky-labs.com", true, L"diffs");
		//data.proxy_url = L"tl-prx-lnx";
		//data.proxy_port = 3128;
		//data.use_ie_proxy = true;
		//data.m_httpAuthorizationMethods.AddAuthMethod (AUT_Basic, true);
		data.UpdaterListFlags.update_bases = true;

		Callbacks cbacks;
		TestCallbackInterface testCallback (&testLog, &cbacks);
		TestJournalInterface testJournal;
		testCallback.SetUpdaterConfiguration (data);
		
		Updater upd (testCallback.m_dp, testCallback, testJournal, &testLog);
		HANDLE hDoUpdate;

		cbacks.bFlags[Callbacks::checkIfFileOptional] = true;
		CREATE_EVENTS(checkIfFileOptional)
		testCallback.SetOptionalFileAnswer(false);

		doUpdater do_upd(&upd, hDoUpdate);

		WAIT_FOR_READY_EVENT(checkIfFileOptional)
		const FileInfo& absentFile = testCallback.GetAbsentFile();
		QC_BOOST_CHECK_MESSAGE (absentFile.m_filename == L"ah-0607g.xml", L"Incorrect absent filename\n");
		SET_GOON_EVENT(checkIfFileOptional);

		WaitForSingleObject (hDoUpdate, INFINITE);

//	QC_BOOST_CHECK_MESSAGE (_helper::IsFoldersEqual(DIFFTOOL_ROOT,
//			std::wstring(COLL_ROOT).append(L"\\Bases_1\\plain").c_str(), BASES_ROOT), L"Incorrect bases\n");

		QC_BOOST_CHECK_MESSAGE ( KLUPD::CORE_NoSuchFileOrDirectory == do_upd.GetError(), L"Wrong return code\n" );
	}	
}

void TestSuite::TestDoubleDownloading ()
{	
	using namespace KLUPD;
	std::wcout<<L"TestDoubleDownloading\n";
	Logger::Case _case (m_logger, m_testp, m_step, L"TestDoubleDownloading");
/*
	{
		std::wcout<<L"Master.xml version is the same\n";

		TestLog testLog;

		_helper::DeleteDirectory(ROOT);
		_helper::DeleteDirectory(BASES_MOD_ROOT);

		KLUPD::createFolder (BASES_MOD_ROOT, &testLog);
		_helper::CopyFolder(std::wstring(COLL_ROOT).append(L"\\double\\first").c_str(), BASES_MOD_ROOT);
		_helper::CloneFile(std::wstring(COLL_ROOT).append(L"\\double\\second\\bases\\av\\avc\\i386\\base103.avc").c_str(),
			std::wstring(BASES_MOD_ROOT).append(L"\\bases\\av\\avc\\i386\\base103.avc").c_str(), false);


		KLUPD::createFolder (BASES_ROOT, &testLog);
		KLUPD::createFolder (UPD_ROOT, &testLog);

		_helper::CopyFolder(std::wstring(COLL_ROOT).append(L"\\Bases_NULL\\plain").c_str(),BASES_ROOT);
		_helper::CloneFile (std::wstring(COLL_ROOT).append(L"\\Bases_NULL\\index\\6\\u0607g.xml").c_str(),
			std::wstring (UPD_ROOT).append (L"\\u0607g.xml"), false);

		UpdaterConfiguration data(1, 1, L"1");

		data.temporary_dir = TEMP_ROOT;
		//data.reserve_dir = RESERVE_ROOT;

		data.m_pathSubstitutionMap.push_back(std::make_pair<NoCaseString,NoCaseString>(L"%BaseFolder%", BASES_ROOT));
		data.m_pathSubstitutionMap.push_back(std::make_pair<NoCaseString,NoCaseString>(L"%Folder%", ROLLBACK_ROOT));
		data.m_pathSubstitutionMap.push_back(std::make_pair<NoCaseString,NoCaseString>(L"%UpdateRoot%", UPD_ROOT));
		data.m_pathSubstitutionMap.push_back(std::make_pair<NoCaseString,NoCaseString>(L"%DSKM%", DSKM_ROOT));
		data.m_pathSubstitutionMap.push_back(std::make_pair<NoCaseString,NoCaseString>(L"%DataFolder%", UPD_ROOT));

		data.self_region = L"<default>";
		data.product_lang.push_back( NoCaseString(L"<default>"));
		
		data.os.os = L"Windows";
		data.os.version = L"5.1";

		data.m_componentsToUpdate.push_back (L"AVS");
		data.m_componentsToUpdate.push_back (L"CORE");

		data.connect_tmo = 30;

		data.m_sourceList.addSource (UST_UserURL, BASES_MOD_ROOT, true, L"diffs");
		data.UpdaterListFlags.update_bases = true;

		Callbacks cbacks;
		TestCallbackInterface testCallback (&testLog, &cbacks);
		TestJournalInterface testJournal;
		testCallback.SetUpdaterConfiguration (data);
		
		Updater upd (testCallback.m_dp, testCallback, testJournal, &testLog);
		HANDLE hDoUpdate;

		cbacks.bFlags[Callbacks::checkIfFileOptional] = true;
		CREATE_EVENTS(checkIfFileOptional)
		testCallback.SetOptionalFileAnswer(false);

		doUpdater do_upd(&upd, hDoUpdate);

		WAIT_FOR_READY_EVENT(checkIfFileOptional)
		QC_BOOST_CHECK_MESSAGE (testCallback.GetAbsentFile().m_filename == L"av-i386-0607g.xml", L"Incorrect absent filename\n");
		
		//renew the component
		_helper::CopyFolder( std::wstring(COLL_ROOT).append(L"\\double\\first\\bases\\av\\avc\\i386").c_str(),
			std::wstring(BASES_MOD_ROOT).append(L"\\bases\\av\\avc\\i386").c_str() );
	
		//but the primary index is the same
		
		SET_GOON_EVENT(checkIfFileOptional);

		WaitForSingleObject (hDoUpdate, INFINITE);

		QC_BOOST_CHECK_MESSAGE (_helper::IsFoldersEqual(DIFFTOOL_ROOT,
			std::wstring(COLL_ROOT).append(L"\\Bases_NULL\\plain").c_str(), BASES_ROOT), L"Incorrect bases\n");
		QC_BOOST_CHECK_MESSAGE (_helper::IsFoldersEqual(DIFFTOOL_ROOT,
			std::wstring(COLL_ROOT).append(L"\\Bases_NULL\\data").c_str(), UPD_ROOT), L"Incorrect bases\n");

		QC_BOOST_CHECK_MESSAGE ( KLUPD::CORE_INVALID_SIGNATURE == do_upd.GetError(), L"Wrong return code\n" );
	}
*/	
	{
		std::wcout<<L"Master.xml version is newer\n";

		TestLog testLog;

		_helper::DeleteDirectory(ROOT);
		_helper::DeleteDirectory(BASES_MOD_ROOT);

		KLUPD::createFolder (BASES_MOD_ROOT, &testLog);
		_helper::CopyFolder(std::wstring(COLL_ROOT).append(L"\\double\\first").c_str(), BASES_MOD_ROOT);
		_helper::CloneFile(std::wstring(COLL_ROOT).append(L"\\double\\second\\bases\\av\\avc\\i386\\base103.avc").c_str(),
			std::wstring(BASES_MOD_ROOT).append(L"\\bases\\av\\avc\\i386\\base103.avc").c_str(), false);


		KLUPD::createFolder (BASES_ROOT, &testLog);
		KLUPD::createFolder (UPD_ROOT, &testLog);

		_helper::CopyFolder(std::wstring(COLL_ROOT).append(L"\\Bases_NULL\\plain").c_str(),BASES_ROOT);
		_helper::CloneFile (std::wstring(COLL_ROOT).append(L"\\Bases_NULL\\index\\6\\u0607g.xml").c_str(),
			std::wstring (UPD_ROOT).append (L"\\u0607g.xml"), false);

		UpdaterConfiguration data(1, 1, L"1");

		data.temporary_dir = TEMP_ROOT;
		//data.reserve_dir = RESERVE_ROOT;

		data.m_pathSubstitutionMap.push_back(std::make_pair<NoCaseString,NoCaseString>(L"%BaseFolder%", BASES_ROOT));
		data.m_pathSubstitutionMap.push_back(std::make_pair<NoCaseString,NoCaseString>(L"%Folder%", ROLLBACK_ROOT));
		data.m_pathSubstitutionMap.push_back(std::make_pair<NoCaseString,NoCaseString>(L"%UpdateRoot%", UPD_ROOT));
		data.m_pathSubstitutionMap.push_back(std::make_pair<NoCaseString,NoCaseString>(L"%DSKM%", DSKM_ROOT));
		data.m_pathSubstitutionMap.push_back(std::make_pair<NoCaseString,NoCaseString>(L"%DataFolder%", UPD_ROOT));

		data.self_region = L"<default>";
		data.product_lang.push_back( NoCaseString(L"<default>"));
		
		data.os.os = L"Windows";
		data.os.version = L"5.1";

		data.m_componentsToUpdate.push_back (L"AVS");
		data.m_componentsToUpdate.push_back (L"CORE");

		data.connect_tmo = 30;

		data.m_sourceList.addSource (UST_UserURL, BASES_MOD_ROOT, true, L"diffs");
		data.UpdaterListFlags.update_bases = true;

		Callbacks cbacks;
		TestCallbackInterface testCallback (&testLog, &cbacks);
		TestJournalInterface testJournal;
		testCallback.SetUpdaterConfiguration (data);
		
		Updater upd (testCallback.m_dp, testCallback, testJournal, &testLog);
		HANDLE hDoUpdate;

		cbacks.bFlags[Callbacks::checkIfFileOptional] = true;
		CREATE_EVENTS(checkIfFileOptional)
		testCallback.SetOptionalFileAnswer(false);

		doUpdater do_upd(&upd, hDoUpdate);

		WAIT_FOR_READY_EVENT(checkIfFileOptional)
		QC_BOOST_CHECK_MESSAGE (testCallback.GetAbsentFile().m_filename == L"av-i386-0607g.xml", L"Incorrect absent filename\n");
		
		//renew the component
		_helper::CopyFolder(std::wstring(COLL_ROOT).append(L"\\double\\second").c_str(), BASES_MOD_ROOT);
		//and the primary index is newer
		
		SET_GOON_EVENT(checkIfFileOptional);

		WaitForSingleObject (hDoUpdate, INFINITE);

		QC_BOOST_CHECK_MESSAGE (_helper::IsFoldersEqual(DIFFTOOL_ROOT,
			std::wstring(COLL_ROOT).append(L"\\double\\plain").c_str(), BASES_ROOT), L"Incorrect bases\n");
		QC_BOOST_CHECK_MESSAGE (_helper::IsFoldersEqual(DIFFTOOL_ROOT,
			std::wstring(COLL_ROOT).append(L"\\double\\data").c_str(), UPD_ROOT), L"Incorrect bases\n");

		QC_BOOST_CHECK_MESSAGE ( KLUPD::CORE_NO_ERROR == do_upd.GetError(), L"Wrong return code\n" );
	}
	
}


void TestSuite::TestCheckBlackList ()
{
	using namespace KLUPD;
	std::wcout<<L"TestCheckBlackList\n";
	Logger::Case _case (m_logger, m_testp, m_step, L"TestCheckBlackList");

	{
		std::wcout<<L"Wrong key\n";

		_helper::DeleteDirectory(ROOT);

		TestLog testLog;

		UpdaterConfiguration data(1, 1, L"1");

		data.temporary_dir = TEMP_ROOT;
		//data.reserve_dir = RESERVE_ROOT;

		data.m_pathSubstitutionMap.push_back(std::make_pair<NoCaseString,NoCaseString>(L"%BaseFolder%", BASES_ROOT));
		data.m_pathSubstitutionMap.push_back(std::make_pair<NoCaseString,NoCaseString>(L"%Folder%", ROLLBACK_ROOT));
		data.m_pathSubstitutionMap.push_back(std::make_pair<NoCaseString,NoCaseString>(L"%UpdateRoot%", UPD_ROOT));
		data.m_pathSubstitutionMap.push_back(std::make_pair<NoCaseString,NoCaseString>(L"%DSKM%", DSKM_ROOT));

		data.self_region = L"<default>";
		data.product_lang.push_back( NoCaseString(L"<default>"));
		
		data.os.os = L"Windows";
		data.os.version = L"5.1";

		data.m_componentsToUpdate.push_back (L"AVS");
		data.m_componentsToUpdate.push_back (L"CORE");
		data.connect_tmo = 30;

		//data.m_sourceList.addSource (UST_UserURL, L"h:\\fsee\\testing\\collection\\Bases\\Bases0", true, L"diffs");
		data.m_sourceList.addSource (UST_UserURL, std::wstring(COLL_ROOT).append(L"\\Bases_1").c_str(), true, L"diffs");
		//data.proxy_url = L"tl-prx-bsc";
		//data.proxy_port = 3128;
		//data.use_ie_proxy = true;
		//data.m_httpAuthorizationMethods.AddAuthMethod (AUT_Basic, true);
		data.UpdaterListFlags.update_bases = true;

		Callbacks cbacks;
		TestCallbackInterface testCallback (&testLog, &cbacks);
		TestJournalInterface testJournal;
		testCallback.SetUpdaterConfiguration (data);
		
		Updater upd (testCallback.m_dp, testCallback, testJournal, &testLog);
		HANDLE hDoUpdate;

		cbacks.bFlags[Callbacks::checkBlackListFile] = true;
		testCallback.SetBlacklistAnswer (false);

		doUpdater do_upd(&upd, hDoUpdate);

		WaitForSingleObject (hDoUpdate, INFINITE);

		QC_BOOST_CHECK_MESSAGE ( CORE_WRONG_KEY == do_upd.GetError(), L"Wrong return code\n" );
	}

	{
		std::wcout<<L"Proper key\n";
		
		_helper::DeleteDirectory(ROOT);

		TestLog testLog;

		UpdaterConfiguration data(1, 1, L"1");

		data.temporary_dir = TEMP_ROOT;
		//data.reserve_dir = RESERVE_ROOT;

		data.m_pathSubstitutionMap.push_back(std::make_pair<NoCaseString,NoCaseString>(L"%BaseFolder%", BASES_ROOT));
		data.m_pathSubstitutionMap.push_back(std::make_pair<NoCaseString,NoCaseString>(L"%Folder%", ROLLBACK_ROOT));
		data.m_pathSubstitutionMap.push_back(std::make_pair<NoCaseString,NoCaseString>(L"%UpdateRoot%", UPD_ROOT));
		data.m_pathSubstitutionMap.push_back(std::make_pair<NoCaseString,NoCaseString>(L"%DSKM%", DSKM_ROOT));

		data.self_region = L"<default>";
		data.product_lang.push_back( NoCaseString(L"<default>"));
		
		data.os.os = L"Windows";
		data.os.version = L"5.1";

		data.m_componentsToUpdate.push_back (L"AVS");
		data.m_componentsToUpdate.push_back (L"CORE");
		data.connect_tmo = 30;

		//data.m_sourceList.addSource (UST_UserURL, L"h:\\fsee\\testing\\collection\\Bases\\Bases0", true, L"diffs");
		data.m_sourceList.addSource (UST_UserURL, std::wstring(COLL_ROOT).append(L"\\Bases_1").c_str(), true, L"diffs");
		//data.proxy_url = L"tl-prx-bsc";
		//data.proxy_port = 3128;
		//data.use_ie_proxy = true;
		//data.m_httpAuthorizationMethods.AddAuthMethod (AUT_Basic, true);
		data.UpdaterListFlags.update_bases = true;

		Callbacks cbacks;
		TestCallbackInterface testCallback (&testLog, &cbacks);
		TestJournalInterface testJournal;
		testCallback.SetUpdaterConfiguration (data);
		
		Updater upd (testCallback.m_dp, testCallback, testJournal, &testLog);
		HANDLE hDoUpdate;

		cbacks.bFlags[Callbacks::checkBlackListFile] = true;
		testCallback.SetBlacklistAnswer (true);

		doUpdater do_upd(&upd, hDoUpdate);

		WaitForSingleObject (hDoUpdate, INFINITE);

		QC_BOOST_CHECK_MESSAGE ( CORE_NO_ERROR == do_upd.GetError(), L"Wrong return code\n" );
	}

}

void TestSuite::TestProcessReceivedFiles ()
{
	using namespace KLUPD;
	std::wcout<<L"TestProcessReceivedFiles\n";
	Logger::Case _case (m_logger, m_testp, m_step, L"TestProcessReceivedFiles");

	{
		TestLog testLog;

		_helper::DeleteDirectory(ROOT);
		KLUPD::createFolder (BASES_ROOT, &testLog);
		_helper::CopyFolder(std::wstring(COLL_ROOT).append(L"\\Bases_NULL\\plain").c_str(),BASES_ROOT);

		UpdaterConfiguration data(1, 1, L"1");

		data.temporary_dir = TEMP_ROOT;
		//data.reserve_dir = RESERVE_ROOT;

		data.m_pathSubstitutionMap.push_back(std::make_pair<NoCaseString,NoCaseString>(L"%BaseFolder%", BASES_ROOT));
		data.m_pathSubstitutionMap.push_back(std::make_pair<NoCaseString,NoCaseString>(L"%Folder%", ROLLBACK_ROOT));
		data.m_pathSubstitutionMap.push_back(std::make_pair<NoCaseString,NoCaseString>(L"%UpdateRoot%", UPD_ROOT));
		data.m_pathSubstitutionMap.push_back(std::make_pair<NoCaseString,NoCaseString>(L"%DSKM%", DSKM_ROOT));
		data.m_pathSubstitutionMap.push_back(std::make_pair<NoCaseString,NoCaseString>(L"%DataFolder%", UPD_ROOT));

		data.self_region = L"<default>";
		data.product_lang.push_back( NoCaseString(L"<default>"));
		
		data.os.os = L"Windows";
		data.os.version = L"5.1";

		data.m_componentsToUpdate.push_back (L"AVS");
		data.m_componentsToUpdate.push_back (L"CORE");
		data.connect_tmo = 30;

		data.m_sourceList.addSource (UST_UserURL, std::wstring(COLL_ROOT).append(L"\\Bases_1").c_str(), true, L"diffs");
		data.UpdaterListFlags.update_bases = true;

		Callbacks cbacks;
		TestCallbackInterface testCallback (&testLog, &cbacks);
		TestJournalInterface testJournal;
		testCallback.SetUpdaterConfiguration (data);
		
		Updater upd (testCallback.m_dp, testCallback, testJournal, &testLog);
		HANDLE hDoUpdate;

		cbacks.bFlags[Callbacks::processReceivedFiles] = true;
		CREATE_EVENTS(processReceivedFiles)

		doUpdater do_upd(&upd, hDoUpdate);

		WAIT_FOR_READY_EVENT(processReceivedFiles)
		TestCallbackInterface::_processReceivedFiles& prf = testCallback.GetProcessReceivedFiles();

		std::wcout<<prf.fv.size()<<L"\n";
		for (std::vector<FileInfo>::iterator it = prf.fv.begin();
				it!=prf.fv.end(); ++it)
				{
					std::wcout<<it->m_filename.m_value.toWideChar()<<L"\n";
				}

		QC_BOOST_REQUIRE_MESSAGE (prf.fv.size() == 11, L"Incorrect file list size\n");
		
		QC_BOOST_CHECK_MESSAGE (prf.fv.at(5).m_filename == L"u0607g.xml", L"Incorrect file name\n");
		QC_BOOST_CHECK_MESSAGE (prf.fv.at(1).m_filename == L"av-i386-0607g.xml", L"Incorrect file name\n");
		QC_BOOST_CHECK_MESSAGE (prf.fv.at(6).m_filename == L"base101.avc", L"Incorrect file name\n");
		QC_BOOST_CHECK_MESSAGE (prf.fv.at(7).m_filename == L"base102.avc", L"Incorrect file name\n");
		QC_BOOST_CHECK_MESSAGE (prf.fv.at(8).m_filename == L"base103.avc", L"Incorrect file name\n");

		QC_BOOST_CHECK_MESSAGE (prf.fv.at(5).m_transactionInformation.m_changeStatus == FileInfo::added, L"Incorrect transaction status\n");
		QC_BOOST_CHECK_MESSAGE (prf.fv.at(1).m_transactionInformation.m_changeStatus == FileInfo::modified, L"Incorrect transaction status\n");
		QC_BOOST_CHECK_MESSAGE (prf.fv.at(6).m_transactionInformation.m_changeStatus == FileInfo::unchanged, L"Incorrect transaction status\n");
		QC_BOOST_CHECK_MESSAGE (prf.fv.at(7).m_transactionInformation.m_changeStatus == FileInfo::modified, L"Incorrect transaction status\n");
		QC_BOOST_CHECK_MESSAGE (prf.fv.at(8).m_transactionInformation.m_changeStatus == FileInfo::added, L"Incorrect transaction status\n");
		
		SET_GOON_EVENT(processReceivedFiles);

		WaitForSingleObject (hDoUpdate, INFINITE);

		QC_BOOST_CHECK_MESSAGE ( NO_ERROR == do_upd.GetError(), L"Wrong return code\n" );
	}
}

void TestSuite::TestReplaceFileOnReboot ()
{
	using namespace KLUPD;
	std::wcout<<L"TestReplaceFileOnReboot\n";
	Logger::Case _case (m_logger, m_testp, m_step, L"TestReplaceFileOnReboot");
	
	{
		std::wcout<<L"Updater can remove file\n";
		TestLog testLog;

		_helper::DeleteDirectory(ROOT);
		KLUPD::createFolder (BASES_ROOT, &testLog);
		KLUPD::createFolder (UPD_ROOT, &testLog);
		_helper::CopyFolder(std::wstring(COLL_ROOT).append(L"\\Bases_NULL\\plain").c_str(),BASES_ROOT);

		_helper::CloneFile (std::wstring(COLL_ROOT).append(L"\\Bases_NULL\\index\\6\\u0607g.xml").c_str(),
			std::wstring (UPD_ROOT).append (L"\\u0607g.xml"), false);

		UpdaterConfiguration data(1, 1, L"1");

		data.temporary_dir = TEMP_ROOT_ANOTHER_DRIVE;
		//data.reserve_dir = RESERVE_ROOT_ANOTHER_DRIVE;
		//data.update_root_folder = UPD_ROOT;

		data.m_pathSubstitutionMap.push_back(std::make_pair<NoCaseString,NoCaseString>(L"%BaseFolder%", BASES_ROOT));
		data.m_pathSubstitutionMap.push_back(std::make_pair<NoCaseString,NoCaseString>(L"%Folder%", ROLLBACK_ROOT));
		data.m_pathSubstitutionMap.push_back(std::make_pair<NoCaseString,NoCaseString>(L"%UpdateRoot%", UPD_ROOT));
		data.m_pathSubstitutionMap.push_back(std::make_pair<NoCaseString,NoCaseString>(L"%DSKM%", DSKM_ROOT));
		data.m_pathSubstitutionMap.push_back(std::make_pair<NoCaseString,NoCaseString>(L"%DataFolder%", UPD_ROOT));

		data.self_region = L"<default>";
		data.product_lang.push_back( NoCaseString(L"<default>"));
		
		data.os.os = L"Windows";
		data.os.version = L"5.1";

		data.m_componentsToUpdate.push_back (L"AVS");
		data.m_componentsToUpdate.push_back (L"CORE");
		data.connect_tmo = 30;

		data.m_sourceList.addSource (UST_UserURL, std::wstring(COLL_ROOT).append(L"\\Bases_1").c_str(), true, L"diffs");
		data.UpdaterListFlags.update_bases = true;

		Callbacks cbacks;
		TestCallbackInterface testCallback (&testLog, &cbacks);
		TestJournalInterface testJournal;
		testCallback.SetUpdaterConfiguration (data);
		
		Updater upd (testCallback.m_dp, testCallback, testJournal, &testLog);
		HANDLE hDoUpdate;

		cbacks.bFlags[Callbacks::removeLockedFileRequest] = true;
		CREATE_EVENTS(removeLockedFileRequest)
		std::wstring file;

		{

			_helper::CloneFile( L"c:\\windows\\system32\\msvcrt.dll", std::wstring(BASES_ROOT).append(L"\\base102.avc"),
								false);
			HMODULE hm = LoadLibrary ( std::wstring(BASES_ROOT).append(L"\\base102.avc").c_str() );
			//_helper::AutoFile locked ( std::wstring(BASES_ROOT).append(L"\\base102.avc") );
			doUpdater do_upd(&upd, hDoUpdate);

			WAIT_FOR_READY_EVENT(removeLockedFileRequest)

			file = testCallback.GetRequestFileToRemove().toWideChar();

			QC_BOOST_CHECK_MESSAGE (file.find(L"base102.avc") != std::wstring::npos &&
					file.find(L"removeOnNextReboot") != std::wstring::npos, L"Incorrect file to replace\n");
			
			SET_GOON_EVENT(removeLockedFileRequest);

			WaitForSingleObject (hDoUpdate, INFINITE);

			QC_BOOST_CHECK_MESSAGE ( NO_ERROR == do_upd.GetError(), L"Wrong return code\n" );
			
			FreeLibrary (hm);
		};

		_helper::DeleteFile (file);
		_helper::DeleteFile ( std::wstring(BASES_ROOT).append(L"\\base100.avc") );
		QC_BOOST_CHECK_MESSAGE (_helper::IsFoldersEqual(DIFFTOOL_ROOT,
			std::wstring(COLL_ROOT).append(L"\\Bases_1\\plain").c_str(), BASES_ROOT), L"Incorrect bases\n");

	}
	
	//QC_BOOST_REQUIRE_MESSAGE (false, L"Bug 21202\n");

	{
		std::wcout<<L"Updater cannot remove file\n";
		TestLog testLog;

		_helper::DeleteDirectory(ROOT);
		KLUPD::createFolder (BASES_ROOT, &testLog);
		KLUPD::createFolder (UPD_ROOT, &testLog);
		_helper::CopyFolder(std::wstring(COLL_ROOT).append(L"\\Bases_NULL\\plain").c_str(),BASES_ROOT);

		_helper::CloneFile (std::wstring(COLL_ROOT).append(L"\\Bases_NULL\\index\\6\\u0607g.xml").c_str(),
			std::wstring (UPD_ROOT).append (L"\\u0607g.xml"), false);

		UpdaterConfiguration data(1, 1, L"1");

		data.temporary_dir = TEMP_ROOT;
		//data.reserve_dir = RESERVE_ROOT;
		//data.update_root_folder = UPD_ROOT;

		data.m_pathSubstitutionMap.push_back(std::make_pair<NoCaseString,NoCaseString>(L"%BaseFolder%", BASES_ROOT));
		data.m_pathSubstitutionMap.push_back(std::make_pair<NoCaseString,NoCaseString>(L"%Folder%", ROLLBACK_ROOT));
		data.m_pathSubstitutionMap.push_back(std::make_pair<NoCaseString,NoCaseString>(L"%UpdateRoot%", UPD_ROOT));
		data.m_pathSubstitutionMap.push_back(std::make_pair<NoCaseString,NoCaseString>(L"%DSKM%", DSKM_ROOT));
		data.m_pathSubstitutionMap.push_back(std::make_pair<NoCaseString,NoCaseString>(L"%DataFolder%", UPD_ROOT));

		data.self_region = L"<default>";
		data.product_lang.push_back( NoCaseString(L"<default>"));
		
		data.os.os = L"Windows";
		data.os.version = L"5.1";

		data.m_componentsToUpdate.push_back (L"AVS");
		data.m_componentsToUpdate.push_back (L"CORE");
		data.connect_tmo = 30;

		data.m_sourceList.addSource (UST_UserURL, std::wstring(COLL_ROOT).append(L"\\Bases_1").c_str(), true, L"diffs");
		data.UpdaterListFlags.update_bases = true;

		Callbacks cbacks;
		TestCallbackInterface testCallback (&testLog, &cbacks);
		TestJournalInterface testJournal;
		testCallback.SetUpdaterConfiguration (data);
		
		Updater upd (testCallback.m_dp, testCallback, testJournal, &testLog);
		HANDLE hDoUpdate;

		{

			_helper::AutoFile locked ( std::wstring(BASES_ROOT).append(L"\\base102.avc") );
			doUpdater do_upd(&upd, hDoUpdate);

			WaitForSingleObject (hDoUpdate, INFINITE);

			QC_BOOST_CHECK_MESSAGE ( //CORE_FILE_OPERATION_FAILURE
					CORE_NotEnoughPermissions == do_upd.GetError(), L"Wrong return code\n" );
		};

		QC_BOOST_CHECK_MESSAGE (_helper::IsFoldersEqual(DIFFTOOL_ROOT,
			std::wstring(COLL_ROOT).append(L"\\Bases_NULL\\plain").c_str(), BASES_ROOT), L"Incorrect bases\n");

	}
	
}

void TestSuite::TestLocalIndexes ()
{
	using namespace KLUPD;
	std::wcout<<L"TestLocalIndexes\n";
	Logger::Case _case (m_logger, m_testp, m_step, L"TestLocalIndexes");

	{
		std::wcout<<L"Providing main index\n";

		TestLog testLog;

		_helper::DeleteDirectory(ROOT);
		KLUPD::createFolder (BASES_ROOT, &testLog);
		KLUPD::createFolder (UPD_ROOT, &testLog);

		_helper::CopyFolder(std::wstring(COLL_ROOT).append(L"\\Bases_NULL\\plain").c_str(),BASES_ROOT);
		_helper::CloneFile (std::wstring(COLL_ROOT).append(L"\\Bases_NULL\\index\\6\\u0607g.xml").c_str(),
			std::wstring (UPD_ROOT).append (L"\\u0607g.xml"), false);

		UpdaterConfiguration data(1, 1, L"1");

		data.temporary_dir = TEMP_ROOT;
		//data.reserve_dir = RESERVE_ROOT;
		//data.update_root_folder = UPD_ROOT;

		data.m_pathSubstitutionMap.push_back(std::make_pair<NoCaseString,NoCaseString>(L"%BaseFolder%", BASES_ROOT));
		data.m_pathSubstitutionMap.push_back(std::make_pair<NoCaseString,NoCaseString>(L"%Folder%", ROLLBACK_ROOT));
		data.m_pathSubstitutionMap.push_back(std::make_pair<NoCaseString,NoCaseString>(L"%UpdateRoot%", UPD_ROOT));
		data.m_pathSubstitutionMap.push_back(std::make_pair<NoCaseString,NoCaseString>(L"%DSKM%", DSKM_ROOT));
		data.m_pathSubstitutionMap.push_back(std::make_pair<NoCaseString,NoCaseString>(L"%DataFolder%", UPD_ROOT));

		data.self_region = L"<default>";
		data.product_lang.push_back( NoCaseString(L"<default>"));
		
		data.os.os = L"Windows";
		data.os.version = L"5.1";

		data.m_componentsToUpdate.push_back (L"AVS");
		data.m_componentsToUpdate.push_back (L"CORE");
		data.connect_tmo = 30;

		data.m_sourceList.addSource (UST_UserURL, std::wstring(COLL_ROOT).append(L"\\Bases_1").c_str(), true, L"diffs");
		data.UpdaterListFlags.update_bases = true;

		Callbacks cbacks;
		TestCallbackInterface testCallback (&testLog, &cbacks);
		TestJournalInterface testJournal;
		testCallback.SetUpdaterConfiguration (data);
		
		Updater upd (testCallback.m_dp, testCallback, testJournal, &testLog);
		HANDLE hDoUpdate;

	//	cbacks.bFlags[Callbacks::checkFilesToDownload] = true;
	//	CREATE_EVENTS(checkFilesToDownload)

		doUpdater do_upd(&upd, hDoUpdate);

	//	WAIT_FOR_READY_EVENT(checkFilesToDownload)
	//	TestCallbackInterface::_checkFilesToDownload& cfd = testCallback.GetCheckFilesToDownload();
	//	std::wcout<<cfd.m_matchFileList->size()<<L"\n";
	//	for (std::vector<FileInfo>::iterator it = cfd.m_matchFileList->begin();
	//			it!=cfd.m_matchFileList->end(); ++it)
	//			{
	//				std::wcout<<it->m_filename.m_value.toWideChar()<<L"\n";
	//			}
	
	//	SET_GOON_EVENT(checkFilesToDownload);

		WaitForSingleObject (hDoUpdate, INFINITE);

		QC_BOOST_CHECK_MESSAGE (_helper::DeleteFile(std::wstring(BASES_ROOT).append (L"\\base100.avc")),
								L"old base file not found");
		QC_BOOST_CHECK_MESSAGE (_helper::IsFoldersEqual(DIFFTOOL_ROOT,
			std::wstring(COLL_ROOT).append(L"\\backup_NULL_to_1").c_str(), RESERVE_ROOT), L"Incorrect backup folder\n");
		QC_BOOST_CHECK_MESSAGE (_helper::IsFoldersEqual(DIFFTOOL_ROOT,
			std::wstring(COLL_ROOT).append(L"\\Bases_1\\plain").c_str(), BASES_ROOT), L"Incorrect bases\n");
		QC_BOOST_CHECK_MESSAGE (_helper::IsFoldersEqual(DIFFTOOL_ROOT,
			std::wstring(COLL_ROOT).append(L"\\Bases_1\\data").c_str(), UPD_ROOT), L"Incorrect index\n");


		QC_BOOST_CHECK_MESSAGE ( NO_ERROR == do_upd.GetError(), L"Wrong return code\n" );
	}

	/*{
		std::wcout<<L"Providing substituted indexes\n";

		TestLog testLog;

		_helper::DeleteDirectory(ROOT);
		KLUPD::createFolder (BASES_ROOT, &testLog);
		KLUPD::createFolder (UPD_ROOT, &testLog);

		_helper::CopyFolder(std::wstring(COLL_ROOT).append(L"\\Bases_NULL_ki\\plain").c_str(),BASES_ROOT);
		//_helper::CloneFile (std::wstring(COLL_ROOT).append(L"\\Bases_NULL_ki\\index\\6\\u0607g.xml").c_str(),
		//	std::wstring (UPD_ROOT).append (L"\\u0607g.xml"), false);

		UpdaterConfiguration data(1, 1, L"1");

		data.temporary_dir = TEMP_ROOT;
		//data.reserve_dir = RESERVE_ROOT;
		//data.update_root_folder = UPD_ROOT;

		data.m_pathSubstitutionMap.push_back(std::make_pair<NoCaseString,NoCaseString>(L"%BaseFolder%", BASES_ROOT));
		data.m_pathSubstitutionMap.push_back(std::make_pair<NoCaseString,NoCaseString>(L"%Folder%", ROLLBACK_ROOT));
		data.m_pathSubstitutionMap.push_back(std::make_pair<NoCaseString,NoCaseString>(L"%UpdateRoot%", UPD_ROOT));
		data.m_pathSubstitutionMap.push_back(std::make_pair<NoCaseString,NoCaseString>(L"%DSKM%", DSKM_ROOT));
		data.m_pathSubstitutionMap.push_back(std::make_pair<NoCaseString,NoCaseString>(L"%DataFolder%", UPD_ROOT));

		data.self_region = L"<default>";
		data.product_lang.push_back( NoCaseString(L"<default>"));
		
		data.os.os = L"Windows";
		data.os.version = L"5.1";

		data.m_componentsToUpdate.push_back (L"AVS");
		data.m_componentsToUpdate.push_back (L"CORE");
		data.connect_tmo = 30;

		data.m_sourceList.addSource (UST_UserURL, std::wstring(COLL_ROOT).append(L"\\Bases_1").c_str(), true, L"diffs");
		data.UpdaterListFlags.update_bases = true;

		Callbacks cbacks;
		TestCallbackInterface testCallback (&testLog, &cbacks);
		TestJournalInterface testJournal;
		testCallback.SetUpdaterConfiguration (data);
		
		Updater upd (testCallback.m_dp, testCallback, testJournal, &testLog);
		HANDLE hDoUpdate;

		cbacks.bFlags[Callbacks::getSubstitutedIndex] = true;
		CREATE_EVENTS(getSubstitutedIndex)

		doUpdater do_upd(&upd, hDoUpdate);

		WAIT_FOR_READY_EVENT(getSubstitutedIndex)

		FileVector fv;
		fv.push_back( FileInfo (L"av-i386-0607g.xml", L"bases/av/avc/i386/", FileInfo::index, true) );
		fv.back().m_originalLocalPath = BASES_ROOT;
		fv.push_back( FileInfo (L"blst-0607g.xml", L"bases/blst/", FileInfo::index, true) );
		fv.back().m_originalLocalPath = BASES_ROOT;

		testCallback.SetSubstitutedIndexes (fv);
	
		SET_GOON_EVENT(getSubstitutedIndex);

		//WAIT_FOR_READY_EVENT(getSubstitutedIndex)
		//SET_GOON_EVENT(getSubstitutedIndex);

		WaitForSingleObject (hDoUpdate, INFINITE);

		_helper::CloneFile ( std::wstring(COLL_ROOT).append(L"\\backup_NULL_ki_to_1\\u0607g.xml").c_str(),
							std::wstring(RESERVE_ROOT).append(L"\\u0607g.xml"), false );
		QC_BOOST_CHECK_MESSAGE (_helper::DeleteFile(std::wstring(BASES_ROOT).append (L"\\base100.avc")),
								L"old base file not found");
		QC_BOOST_CHECK_MESSAGE (_helper::IsFoldersEqual(DIFFTOOL_ROOT,
			std::wstring(COLL_ROOT).append(L"\\backup_NULL_ki_to_1").c_str(), RESERVE_ROOT), L"Incorrect backup folder\n");
		QC_BOOST_CHECK_MESSAGE (_helper::IsFoldersEqual(DIFFTOOL_ROOT,
			std::wstring(COLL_ROOT).append(L"\\Bases_1\\plain").c_str(), BASES_ROOT), L"Incorrect bases\n");
		QC_BOOST_CHECK_MESSAGE (_helper::IsFoldersEqual(DIFFTOOL_ROOT,
			std::wstring(COLL_ROOT).append(L"\\Bases_1\\data").c_str(), UPD_ROOT), L"Incorrect index\n");

		QC_BOOST_CHECK_MESSAGE ( NO_ERROR == do_upd.GetError(), L"Wrong return code\n" );
	}*/ //такого функционала больше нет

	{
		std::wcout<<L"Providing neither primary nor substituted indexes\n";

		TestLog testLog;

		_helper::DeleteDirectory(ROOT);
		KLUPD::createFolder (BASES_ROOT, &testLog);
		KLUPD::createFolder (UPD_ROOT, &testLog);

		_helper::CopyFolder(std::wstring(COLL_ROOT).append(L"\\Bases_NULL_ki\\plain").c_str(),BASES_ROOT);
		//_helper::CloneFile (std::wstring(COLL_ROOT).append(L"\\Bases_NULL_ki\\index\\6\\u0607g.xml").c_str(),
		//	std::wstring (UPD_ROOT).append (L"\\u0607g.xml"), false);

		UpdaterConfiguration data(1, 1, L"1");

		data.temporary_dir = TEMP_ROOT;
		//data.reserve_dir = RESERVE_ROOT;
		//data.update_root_folder = UPD_ROOT;

		data.m_pathSubstitutionMap.push_back(std::make_pair<NoCaseString,NoCaseString>(L"%BaseFolder%", BASES_ROOT));
		data.m_pathSubstitutionMap.push_back(std::make_pair<NoCaseString,NoCaseString>(L"%Folder%", ROLLBACK_ROOT));
		data.m_pathSubstitutionMap.push_back(std::make_pair<NoCaseString,NoCaseString>(L"%UpdateRoot%", UPD_ROOT));
		data.m_pathSubstitutionMap.push_back(std::make_pair<NoCaseString,NoCaseString>(L"%DSKM%", DSKM_ROOT));
		data.m_pathSubstitutionMap.push_back(std::make_pair<NoCaseString,NoCaseString>(L"%DataFolder%", UPD_ROOT));

		data.self_region = L"<default>";
		data.product_lang.push_back( NoCaseString(L"<default>"));
		
		data.os.os = L"Windows";
		data.os.version = L"5.1";

		data.m_componentsToUpdate.push_back (L"AVS");
		data.m_componentsToUpdate.push_back (L"CORE");
		data.connect_tmo = 30;

		data.m_sourceList.addSource (UST_UserURL, std::wstring(COLL_ROOT).append(L"\\Bases_1").c_str(), true, L"diffs");
		data.UpdaterListFlags.update_bases = true;

		Callbacks cbacks;
		TestCallbackInterface testCallback (&testLog, &cbacks);
		TestJournalInterface testJournal;
		testCallback.SetUpdaterConfiguration (data);
		
		Updater upd (testCallback.m_dp, testCallback, testJournal, &testLog);
		HANDLE hDoUpdate;

		doUpdater do_upd(&upd, hDoUpdate);

		WaitForSingleObject (hDoUpdate, INFINITE);

		_helper::DeleteFile( std::wstring (BASES_ROOT).append(L"\\base100.avc") );

		//no sense
		//QC_BOOST_CHECK_MESSAGE (_helper::IsFoldersEqual(DIFFTOOL_ROOT,
		//	EMPTY_ROOT, RESERVE_ROOT), L"Incorrect backup folder\n");
		QC_BOOST_CHECK_MESSAGE (_helper::IsFoldersEqual(DIFFTOOL_ROOT,
			std::wstring(COLL_ROOT).append(L"\\Bases_1\\plain").c_str(), BASES_ROOT), L"Incorrect bases\n");
		QC_BOOST_CHECK_MESSAGE (_helper::IsFoldersEqual(DIFFTOOL_ROOT,
			std::wstring(COLL_ROOT).append(L"\\Bases_1\\data").c_str(), UPD_ROOT), L"Incorrect index\n");

		QC_BOOST_CHECK_MESSAGE ( NO_ERROR == do_upd.GetError(), L"Wrong return code\n" );
	}
	
}


void TestSuite::TestSourceIndexesPlanar ()
{
	using namespace KLUPD;
	std::wcout<<L"TestSourceIndexesPlanar\n";	
	Logger::Case _case (m_logger, m_testp, m_step, L"TestSourceIndexesPlanar");

	{
		std::wcout<<L"Providing primary index planar structure\n";

		TestLog testLog;

		_helper::DeleteDirectory(ROOT);
		KLUPD::createFolder (BASES_ROOT, &testLog);
		KLUPD::createFolder (UPD_ROOT, &testLog);

		_helper::CopyFolder(std::wstring(COLL_ROOT).append(L"\\Bases_NULL\\plain").c_str(),BASES_ROOT);
		_helper::CloneFile (std::wstring(COLL_ROOT).append(L"\\Bases_NULL\\index\\6\\u0607g.xml").c_str(),
			std::wstring (UPD_ROOT).append (L"\\u0607g.xml"), false);

		UpdaterConfiguration data(1, 1, L"1");

		data.temporary_dir = TEMP_ROOT;
		//data.reserve_dir = RESERVE_ROOT;
		//data.update_root_folder = UPD_ROOT;

		data.m_pathSubstitutionMap.push_back(std::make_pair<NoCaseString,NoCaseString>(L"%BaseFolder%", BASES_ROOT));
		data.m_pathSubstitutionMap.push_back(std::make_pair<NoCaseString,NoCaseString>(L"%Folder%", ROLLBACK_ROOT));
		data.m_pathSubstitutionMap.push_back(std::make_pair<NoCaseString,NoCaseString>(L"%UpdateRoot%", UPD_ROOT));
		data.m_pathSubstitutionMap.push_back(std::make_pair<NoCaseString,NoCaseString>(L"%DSKM%", DSKM_ROOT));
		data.m_pathSubstitutionMap.push_back(std::make_pair<NoCaseString,NoCaseString>(L"%DataFolder%", UPD_ROOT));

		data.self_region = L"<default>";
		data.product_lang.push_back( NoCaseString(L"<default>"));
		
		data.os.os = L"Windows";
		data.os.version = L"5.1";

		data.m_componentsToUpdate.push_back (L"AVS");
		data.m_componentsToUpdate.push_back (L"CORE");
		data.connect_tmo = 30;

		data.m_sourceList.addSource (UST_UserURL, std::wstring(COLL_ROOT).append(L"\\Bases_1_planar").c_str(), true, L"diffs");
		data.UpdaterListFlags.update_bases = true;

		Callbacks cbacks;
		TestCallbackInterface testCallback (&testLog, &cbacks);
		TestJournalInterface testJournal;
		testCallback.SetUpdaterConfiguration (data);
		
		Updater upd (testCallback.m_dp, testCallback, testJournal, &testLog);
		HANDLE hDoUpdate;

		cbacks.bFlags[Callbacks::getSubstitutedIndex] = true;
		CREATE_EVENTS(getSubstitutedIndex)

		doUpdater do_upd(&upd, hDoUpdate);

		WaitForSingleObject (hDoUpdate, INFINITE);

		QC_BOOST_CHECK_MESSAGE (_helper::DeleteFile(std::wstring(BASES_ROOT).append (L"\\base100.avc")),
								L"old base file not found");
		QC_BOOST_CHECK_MESSAGE (_helper::IsFoldersEqual(DIFFTOOL_ROOT,
			std::wstring(COLL_ROOT).append(L"\\backup_NULL_to_1").c_str(), RESERVE_ROOT), L"Incorrect backup folder\n");
		QC_BOOST_CHECK_MESSAGE (_helper::IsFoldersEqual(DIFFTOOL_ROOT,
			std::wstring(COLL_ROOT).append(L"\\Bases_1\\plain").c_str(), BASES_ROOT), L"Incorrect bases\n");
		QC_BOOST_CHECK_MESSAGE (_helper::IsFoldersEqual(DIFFTOOL_ROOT,
			std::wstring(COLL_ROOT).append(L"\\Bases_1\\data").c_str(), UPD_ROOT), L"Incorrect index\n");

		QC_BOOST_CHECK_MESSAGE ( NO_ERROR == do_upd.GetError(), L"Wrong return code\n" );
	}

	/*{ такого функционала больше нет
		std::wcout<<L"Providing substituted indexes planar structure\n";
		std::wcout<<L"Bug 17592\n";

		TestLog testLog;

		_helper::DeleteDirectory(ROOT);
		KLUPD::createFolder (BASES_ROOT, &testLog);
		KLUPD::createFolder (UPD_ROOT, &testLog);

		_helper::CopyFolder(std::wstring(COLL_ROOT).append(L"\\Bases_NULL\\plain").c_str(),BASES_ROOT);
		_helper::CloneFile (std::wstring(COLL_ROOT).append(L"\\Bases_NULL\\index\\6\\u0607g.xml").c_str(),
			std::wstring (UPD_ROOT).append (L"\\u0607g.xml"), false);

		UpdaterConfiguration data(1, 1, L"1");

		data.temporary_dir = TEMP_ROOT;
		//data.reserve_dir = RESERVE_ROOT;
		//data.update_root_folder = UPD_ROOT;

		data.m_pathSubstitutionMap.push_back(std::make_pair<NoCaseString,NoCaseString>(L"%BaseFolder%", BASES_ROOT));
		data.m_pathSubstitutionMap.push_back(std::make_pair<NoCaseString,NoCaseString>(L"%Folder%", ROLLBACK_ROOT));
		data.m_pathSubstitutionMap.push_back(std::make_pair<NoCaseString,NoCaseString>(L"%UpdateRoot%", UPD_ROOT));
		data.m_pathSubstitutionMap.push_back(std::make_pair<NoCaseString,NoCaseString>(L"%DSKM%", DSKM_ROOT));
		data.m_pathSubstitutionMap.push_back(std::make_pair<NoCaseString,NoCaseString>(L"%DataFolder%", UPD_ROOT));

		data.self_region = L"<default>";
		data.product_lang.push_back( NoCaseString(L"<default>"));
		
		data.os.os = L"Windows";
		data.os.version = L"5.1";

		data.m_componentsToUpdate.push_back (L"AVS");
		data.m_componentsToUpdate.push_back (L"CORE");
		data.connect_tmo = 30;

		data.m_sourceList.addSource (UST_UserURL, std::wstring(COLL_ROOT).append(L"\\Bases_1_ki_planar").c_str(), true, L"diffs");
		data.UpdaterListFlags.update_bases = true;

		Callbacks cbacks;
		TestCallbackInterface testCallback (&testLog, &cbacks);
		TestJournalInterface testJournal;
		testCallback.SetUpdaterConfiguration (data);
		
		Updater upd (testCallback.m_dp, testCallback, testJournal, &testLog);
		HANDLE hDoUpdate;

		cbacks.bFlags[Callbacks::getSubstitutedIndex] = true;
		CREATE_EVENTS(getSubstitutedIndex)

		doUpdater do_upd(&upd, hDoUpdate);

		WAIT_FOR_READY_EVENT(getSubstitutedIndex)

		FileVector fv;
		fv.push_back( FileInfo (L"av-i386-0607g.xml", L"bases/av/avc/i386/", FileInfo::index, true) );
		fv.back().m_originalLocalPath = BASES_ROOT;
		fv.push_back( FileInfo (L"blst-0607g.xml", L"bases/blst/", FileInfo::index, true) );
		fv.back().m_originalLocalPath = BASES_ROOT;

		testCallback.SetSubstitutedIndexes (fv);
	
		SET_GOON_EVENT(getSubstitutedIndex);

	//	WAIT_FOR_READY_EVENT(getSubstitutedIndex)
	//	SET_GOON_EVENT(getSubstitutedIndex);

		WaitForSingleObject (hDoUpdate, INFINITE);

		QC_BOOST_CHECK_MESSAGE (_helper::DeleteFile(std::wstring(BASES_ROOT).append (L"\\base100.avc")),
								L"old base file not found");
		QC_BOOST_CHECK_MESSAGE (_helper::IsFoldersEqual(DIFFTOOL_ROOT,
			std::wstring(COLL_ROOT).append(L"\\backup_NULL_to_1").c_str(), RESERVE_ROOT), L"Incorrect backup folder\n");
		QC_BOOST_CHECK_MESSAGE (_helper::IsFoldersEqual(DIFFTOOL_ROOT,
			std::wstring(COLL_ROOT).append(L"\\Bases_1_ki_planar").c_str(), BASES_ROOT), L"Incorrect bases\n");
		QC_BOOST_CHECK_MESSAGE (_helper::IsFoldersEqual(DIFFTOOL_ROOT,
			std::wstring(COLL_ROOT).append(L"\\Bases_1\\data").c_str(), UPD_ROOT), L"Incorrect index\n");

		QC_BOOST_CHECK_MESSAGE ( NO_ERROR == do_upd.GetError(), L"Wrong return code\n" );
	}*/
}

void TestSuite::TestSourceIndexesDirectoried()
{
	return; //такого функционала больше нет

	using namespace KLUPD;
	std::wcout<<L"TestSourceIndexesDirectoried\n";
	Logger::Case _case (m_logger, m_testp, m_step, L"TestSourceIndexesDirectoried");
	QC_BOOST_REQUIRE_MESSAGE (false, L"Bug 17546\n");
	{
		std::wcout<<L"Providing substituted indexes directoried structure\n";

		TestLog testLog;

		_helper::DeleteDirectory(ROOT);
		KLUPD::createFolder (BASES_ROOT, &testLog);
		KLUPD::createFolder (UPD_ROOT, &testLog);

		_helper::CopyFolder(std::wstring(COLL_ROOT).append(L"\\Bases_NULL\\plain").c_str(),BASES_ROOT);
		_helper::CloneFile (std::wstring(COLL_ROOT).append(L"\\Bases_NULL\\index\\6\\u0607g.xml").c_str(),
			std::wstring (UPD_ROOT).append (L"\\u0607g.xml"), false);

		UpdaterConfiguration data(1, 1, L"1");

		data.temporary_dir = TEMP_ROOT;
		//data.reserve_dir = RESERVE_ROOT;
		//data.update_root_folder = UPD_ROOT;

		data.m_pathSubstitutionMap.push_back(std::make_pair<NoCaseString,NoCaseString>(L"%BaseFolder%", BASES_ROOT));
		data.m_pathSubstitutionMap.push_back(std::make_pair<NoCaseString,NoCaseString>(L"%Folder%", ROLLBACK_ROOT));
		data.m_pathSubstitutionMap.push_back(std::make_pair<NoCaseString,NoCaseString>(L"%UpdateRoot%", UPD_ROOT));
		data.m_pathSubstitutionMap.push_back(std::make_pair<NoCaseString,NoCaseString>(L"%DSKM%", DSKM_ROOT));
		data.m_pathSubstitutionMap.push_back(std::make_pair<NoCaseString,NoCaseString>(L"%DataFolder%", UPD_ROOT));

		data.self_region = L"<default>";
		data.product_lang.push_back( NoCaseString(L"<default>"));
		
		data.os.os = L"Windows";
		data.os.version = L"5.1";

		data.m_componentsToUpdate.push_back (L"AVS");
		data.m_componentsToUpdate.push_back (L"CORE");
		data.connect_tmo = 30;

		data.m_sourceList.addSource (UST_UserURL, std::wstring(COLL_ROOT).append(L"\\Bases_1_ki").c_str(), true, L"diffs");
		data.UpdaterListFlags.update_bases = true;

		Callbacks cbacks;
		TestCallbackInterface testCallback (&testLog, &cbacks);
		TestJournalInterface testJournal;
		testCallback.SetUpdaterConfiguration (data);
		
		Updater upd (testCallback.m_dp, testCallback, testJournal, &testLog);
		HANDLE hDoUpdate;

		cbacks.bFlags[Callbacks::getSubstitutedIndex] = true;
		CREATE_EVENTS(getSubstitutedIndex)

		doUpdater do_upd(&upd, hDoUpdate);

		WAIT_FOR_READY_EVENT(getSubstitutedIndex)

		FileVector fv;
		fv.push_back( FileInfo (L"av-i386-0607g.xml", L"bases/av/avc/i386/", FileInfo::index, true) );
		//fv.back().m_originalLocalPath = BASES_ROOT;
		fv.push_back( FileInfo (L"blst-0607g.xml", L"bases/blst/", FileInfo::index, true) );
		//fv.back().m_originalLocalPath = BASES_ROOT;

		testCallback.SetSubstitutedIndexes (fv);
	
		SET_GOON_EVENT(getSubstitutedIndex);

	//	WAIT_FOR_READY_EVENT(getSubstitutedIndex)
	//	SET_GOON_EVENT(getSubstitutedIndex);

		WaitForSingleObject (hDoUpdate, INFINITE);

		_helper::CloneFile ( std::wstring(COLL_ROOT).append(L"\\backup_NULL_ki_to_1\\u0607g.xml").c_str(),
							std::wstring(RESERVE_ROOT).append(L"\\u0607g.xml"), false );
		QC_BOOST_CHECK_MESSAGE (_helper::IsFoldersEqual(DIFFTOOL_ROOT,
			std::wstring(COLL_ROOT).append(L"\\backup_NULL_ki_to_1").c_str(), RESERVE_ROOT), L"Incorrect backup folder\n");
		QC_BOOST_CHECK_MESSAGE (_helper::IsFoldersEqual(DIFFTOOL_ROOT,
			std::wstring(COLL_ROOT).append(L"\\Bases_1\\plain").c_str(), BASES_ROOT), L"Incorrect bases\n");
		QC_BOOST_CHECK_MESSAGE (_helper::IsFoldersEqual(DIFFTOOL_ROOT,
			std::wstring(COLL_ROOT).append(L"\\Bases_1\\data").c_str(), UPD_ROOT), L"Incorrect index\n");

		QC_BOOST_CHECK_MESSAGE ( NO_ERROR == do_upd.GetError(), L"Wrong return code\n" );
	}
	
}

void TestSuite::TestSitesXml ()
{
	using namespace KLUPD;
	std::wcout<<L"TestSitesXml\n";
	Logger::Case _case (m_logger, m_testp, m_step, L"TestSitesXml");

	{
		TestLog testLog;

		_helper::DeleteDirectory(ROOT);
		KLUPD::createFolder (BASES_ROOT, &testLog);
		KLUPD::createFolder (UPD_ROOT, &testLog);
		KLUPD::createFolder (PRODUCT_ROOT, &testLog);

		_helper::CopyFolder(std::wstring(COLL_ROOT).append(L"\\Bases_NULL\\plain").c_str(),BASES_ROOT);
		_helper::CloneFile (std::wstring(COLL_ROOT).append(L"\\Bases_NULL\\index\\6\\u0607g.xml").c_str(),
			std::wstring (UPD_ROOT).append (L"\\u0607g.xml"), false);

		_helper::CloneFile(std::wstring(COLL_ROOT).append(L"\\Updcfgs\\sites.xml").c_str(),
			std::wstring (PRODUCT_ROOT).append(L"\\sites.xml"), false);
		_helper::CloneFile(std::wstring(COLL_ROOT).append(L"\\Updcfgs\\updcfg.xml").c_str(),
			std::wstring (UPD_ROOT).append(L"\\updcfg.xml"), false);

		UpdaterConfiguration data(1, 1, L"1");

		data.temporary_dir = TEMP_ROOT;
		//data.reserve_dir = RESERVE_ROOT;
		//data.update_root_folder = UPD_ROOT;
		//data.product_path = PRODUCT_ROOT;

		data.m_pathSubstitutionMap.push_back(std::make_pair<NoCaseString,NoCaseString>(L"%BaseFolder%", BASES_ROOT));
		data.m_pathSubstitutionMap.push_back(std::make_pair<NoCaseString,NoCaseString>(L"%Folder%", ROLLBACK_ROOT));
		data.m_pathSubstitutionMap.push_back(std::make_pair<NoCaseString,NoCaseString>(L"%UpdateRoot%", UPD_ROOT));
		data.m_pathSubstitutionMap.push_back(std::make_pair<NoCaseString,NoCaseString>(L"%DSKM%", DSKM_ROOT));
		data.m_pathSubstitutionMap.push_back(std::make_pair<NoCaseString,NoCaseString>(L"%DataFolder%", UPD_ROOT));

		data.self_region = L"<default>";
		data.product_lang.push_back( NoCaseString(L"<default>"));
		
		data.os.os = L"Windows";
		data.os.version = L"5.1";

		data.m_componentsToUpdate.push_back (L"AVS");
		data.m_componentsToUpdate.push_back (L"CORE");
		data.connect_tmo = 30;

		//data.m_sourceList.addSource (UST_UserURL, std::wstring(COLL_ROOT).append(L"\\Bases_1").c_str(), true, L"diffs");
		//data.m_sourceList.addSource (UST_UserURL, L"h:\\d5x", true, L"diffs");
		data.m_sourceList.addSource (UST_KLServer, L"", false, L"diffs");
		//data.proxy_url = L"tl-prx-lnx";
		//data.proxy_port = 3128;
		//data.use_ie_proxy = true;
		//data.m_httpAuthorizationMethods.AddAuthMethod (AUT_Basic, true);
		data.UpdaterListFlags.update_bases = true;

		Callbacks cbacks;
		TestCallbackInterface testCallback (&testLog, &cbacks);
		TestJournalInterface testJournal;
		testCallback.SetUpdaterConfiguration (data);
		testCallback.SetPrimaryIndex( FileInfo (L"u0607g.xml", L"updater13/bases_1/index/6", FileInfo::index, true) );
		
		FileInfo u0607g ( L"u0607g.xml", L"updater13/bases_1/index/6", FileInfo::index, true );
		u0607g.m_localPath = UPD_ROOT;
		testCallback.SetPrimaryIndex(u0607g);
		
		FileInfo updcfg ( L"updcfg.xml", L"updater13/bases_1/index/6", FileInfo::base, false );
		updcfg.m_localPath = UPD_ROOT;
		testCallback.SetUpdcfgXml(updcfg);

		FileInfo sites ( L"sites.xml", L"updater13/bases_1/index/6", FileInfo::base, false );
		sites.m_localPath = PRODUCT_ROOT;
		testCallback.SetSitesXml(sites);

		Updater upd (testCallback.m_dp, testCallback, testJournal, &testLog);
		HANDLE hDoUpdate;

		cbacks.bFlags[Callbacks::getPrimaryIndex] = true;
		cbacks.bFlags[Callbacks::getUpdaterConfigurationXml] = true;
		cbacks.bFlags[Callbacks::getSitesFileXml] = true;
		
		doUpdater do_upd(&upd, hDoUpdate);

		WaitForSingleObject (hDoUpdate, INFINITE);

		QC_BOOST_CHECK_MESSAGE ( NO_ERROR == do_upd.GetError(), L"Wrong return code\n" );

		//_helper::DeleteFile( std::wstring (UPD_ROOT).append(L"\\sites.xml") );
		_helper::DeleteFile( std::wstring (UPD_ROOT).append(L"\\updcfg.xml") );

		QC_BOOST_CHECK_MESSAGE (_helper::DeleteFile(std::wstring(BASES_ROOT).append (L"\\base100.avc")),
								L"old base file not found");
		QC_BOOST_CHECK_MESSAGE (_helper::IsFoldersEqual(DIFFTOOL_ROOT,
			std::wstring(COLL_ROOT).append(L"\\tl-srv-wxp\\Bases_1\\plain").c_str(), BASES_ROOT), L"Incorrect bases\n");
		QC_BOOST_CHECK_MESSAGE (_helper::IsFoldersEqual(DIFFTOOL_ROOT,
			std::wstring(COLL_ROOT).append(L"\\tl-srv-wxp\\Bases_1\\data").c_str(), UPD_ROOT), L"Incorrect index\n");
	}
}

void TestSuite::TestUpdcfgXml ()
{
	using namespace KLUPD;
	std::wcout<<L"TestUpdcfgXml\n";
	Logger::Case _case (m_logger, m_testp, m_step, L"TestUpdcfgXml");

	{
		TestLog testLog;

		_helper::DeleteDirectory(ROOT);
		KLUPD::createFolder (BASES_ROOT, &testLog);
		KLUPD::createFolder (UPD_ROOT, &testLog);

		_helper::CopyFolder(std::wstring(COLL_ROOT).append(L"\\Bases_NULL\\plain").c_str(),BASES_ROOT);
		_helper::CloneFile (std::wstring(COLL_ROOT).append(L"\\Bases_NULL\\index\\6\\u0607g.xml").c_str(),
			std::wstring (UPD_ROOT).append (L"\\u0607g.xml"), false);

		_helper::CloneFile(std::wstring(COLL_ROOT).append(L"\\Updcfgs\\updcfg2.xml").c_str(),
			std::wstring (UPD_ROOT).append(L"\\updcfg.xml"), false);

		UpdaterConfiguration data(1, 1, L"1");

		data.temporary_dir = TEMP_ROOT;
		//data.reserve_dir = RESERVE_ROOT;
		//data.update_root_folder = UPD_ROOT;
		//data.product_path = PRODUCT_ROOT;
		
		data.m_pathSubstitutionMap.push_back(std::make_pair<NoCaseString,NoCaseString>(L"%BaseFolder%", BASES_ROOT));
		data.m_pathSubstitutionMap.push_back(std::make_pair<NoCaseString,NoCaseString>(L"%Folder%", ROLLBACK_ROOT));
		data.m_pathSubstitutionMap.push_back(std::make_pair<NoCaseString,NoCaseString>(L"%UpdateRoot%", UPD_ROOT));
		data.m_pathSubstitutionMap.push_back(std::make_pair<NoCaseString,NoCaseString>(L"%DSKM%", DSKM_ROOT));
		data.m_pathSubstitutionMap.push_back(std::make_pair<NoCaseString,NoCaseString>(L"%DataFolder%", UPD_ROOT));

		data.self_region = L"<default>";
		data.product_lang.push_back( NoCaseString(L"<default>"));
		
		data.os.os = L"Windows";
		data.os.version = L"5.1";

		data.m_componentsToUpdate.push_back (L"AVS");
		data.m_componentsToUpdate.push_back (L"CORE");
		data.connect_tmo = 30;

		//data.m_sourceList.addSource (UST_UserURL, std::wstring(COLL_ROOT).append(L"\\Bases_1").c_str(), true, L"diffs");
		//data.m_sourceList.addSource (UST_UserURL, L"h:\\d5x", true, L"diffs");
		data.m_sourceList.addSource (UST_KLServer, L"", false, L"diffs");
		//data.proxy_url = L"tl-prx-lnx";
		//data.proxy_port = 3128;
		//data.use_ie_proxy = true;
		//data.m_httpAuthorizationMethods.AddAuthMethod (AUT_Basic, true);
		data.UpdaterListFlags.update_bases = true;

		Callbacks cbacks;
		TestCallbackInterface testCallback (&testLog, &cbacks);
		TestJournalInterface testJournal;
		testCallback.SetUpdaterConfiguration (data);
		testCallback.SetPrimaryIndex( FileInfo (L"u0607g.xml", L"updater13/bases_1/index/6", FileInfo::index, true) );
		
		FileInfo u0607g ( L"u0607g.xml", L"updater13/bases_1/index/6", FileInfo::index, true );
		//u0607g.m_localPath = UPD_ROOT;
		testCallback.SetPrimaryIndex(u0607g);
		
		FileInfo updcfg ( L"updcfg.xml", L"updater13/bases_1/index/6", FileInfo::base, false );
		updcfg.m_localPath = UPD_ROOT;
		testCallback.SetUpdcfgXml(updcfg);

		FileInfo sites ( L"sites.xml", L"updater13/bases_1/index/6", FileInfo::base, false );
		sites.m_localPath = PRODUCT_ROOT;
		testCallback.SetSitesXml(sites);

		Updater upd (testCallback.m_dp, testCallback, testJournal, &testLog);
		HANDLE hDoUpdate;

		cbacks.bFlags[Callbacks::getPrimaryIndex] = true;
		cbacks.bFlags[Callbacks::getUpdaterConfigurationXml] = true;
		cbacks.bFlags[Callbacks::getSitesFileXml] = true;
		
		doUpdater do_upd(&upd, hDoUpdate);

		WaitForSingleObject (hDoUpdate, INFINITE);

		QC_BOOST_CHECK_MESSAGE ( NO_ERROR == do_upd.GetError(), L"Wrong return code\n" );

		_helper::DeleteFile( std::wstring (UPD_ROOT).append(L"\\updcfg.xml") );

		QC_BOOST_CHECK_MESSAGE (_helper::DeleteFile(std::wstring(BASES_ROOT).append (L"\\base100.avc")),
								L"old base file not found");
		QC_BOOST_CHECK_MESSAGE (_helper::IsFoldersEqual(DIFFTOOL_ROOT,
			std::wstring(COLL_ROOT).append(L"\\tl-srv-wxp\\Bases_1\\plain").c_str(), BASES_ROOT), L"Incorrect bases\n");
		QC_BOOST_CHECK_MESSAGE (_helper::IsFoldersEqual(DIFFTOOL_ROOT,
			std::wstring(COLL_ROOT).append(L"\\tl-srv-wxp\\Bases_1\\data").c_str(), UPD_ROOT), L"Incorrect index\n");
	}
}

void TestSuite::TestRequestCredentials ()
{
	using namespace KLUPD;
	std::wcout<<L"TestRequestCredentials\n";
	Logger::Case _case (m_logger, m_testp, m_step, L"TestRequestCredentials");

	{
		TestLog testLog;

		_helper::DeleteDirectory(ROOT);
		KLUPD::createFolder (BASES_ROOT, &testLog);
		KLUPD::createFolder (UPD_ROOT, &testLog);

		_helper::CopyFolder(std::wstring(COLL_ROOT).append(L"\\Bases_NULL\\plain").c_str(),BASES_ROOT);
		_helper::CloneFile (std::wstring(COLL_ROOT).append(L"\\Bases_NULL\\index\\6\\u0607g.xml").c_str(),
			std::wstring (UPD_ROOT).append (L"\\u0607g.xml"), false);

		_helper::CloneFile(std::wstring(COLL_ROOT).append(L"\\Updcfgs\\updcfg_w2k.xml").c_str(),
			std::wstring (UPD_ROOT).append(L"\\updcfg.xml"), false);

		UpdaterConfiguration data(1, 1, L"1");

		data.temporary_dir = TEMP_ROOT;
		//data.reserve_dir = RESERVE_ROOT;
		//data.update_root_folder = UPD_ROOT;
		//data.product_path = PRODUCT_ROOT;
		
		data.m_pathSubstitutionMap.push_back(std::make_pair<NoCaseString,NoCaseString>(L"%BaseFolder%", BASES_ROOT));
		data.m_pathSubstitutionMap.push_back(std::make_pair<NoCaseString,NoCaseString>(L"%Folder%", ROLLBACK_ROOT));
		data.m_pathSubstitutionMap.push_back(std::make_pair<NoCaseString,NoCaseString>(L"%UpdateRoot%", UPD_ROOT));
		data.m_pathSubstitutionMap.push_back(std::make_pair<NoCaseString,NoCaseString>(L"%DSKM%", DSKM_ROOT));
		data.m_pathSubstitutionMap.push_back(std::make_pair<NoCaseString,NoCaseString>(L"%DataFolder%", UPD_ROOT));

		data.self_region = L"<default>";
		data.product_lang.push_back( NoCaseString(L"<default>"));
		
		data.os.os = L"Windows";
		data.os.version = L"5.1";

		data.m_componentsToUpdate.push_back (L"AVS");
		data.m_componentsToUpdate.push_back (L"CORE");
		data.connect_tmo = 30;

		data.m_sourceList.addSource (UST_KLServer, L"", true, L"diffs");
		data.proxy_url = L"tl-prx-lnx";
		data.proxy_port = 3128;
		//data.m_httpAuthorizationMethods.AddAuthMethod (AUT_Basic, true);
		data.m_httpAuthorizationMethods.push_back (basicAuthorization);
		data.UpdaterListFlags.update_bases = true;

		Callbacks cbacks;
		TestCallbackInterface testCallback (&testLog, &cbacks);
		TestJournalInterface testJournal;
		testCallback.SetUpdaterConfiguration (data);
		testCallback.SetPrimaryIndex( FileInfo (L"u0607g.xml", L"updater13/bases_1/index/6", FileInfo::index, true) );
		
		FileInfo u0607g ( L"u0607g.xml", L"updater13/bases_1/index/6", FileInfo::index, true );
		testCallback.SetPrimaryIndex(u0607g);
		FileInfo updcfg ( L"updcfg.xml", L"updater13/bases_1/index/6", FileInfo::base, false );
		updcfg.m_localPath = UPD_ROOT;
		testCallback.SetUpdcfgXml(updcfg);
		FileInfo sites ( L"sites.xml", L"updater13/bases_1/index/6", FileInfo::base, false );
		sites.m_localPath = PRODUCT_ROOT;
		testCallback.SetSitesXml(sites);

		Updater upd (testCallback.m_dp, testCallback, testJournal, &testLog);
		HANDLE hDoUpdate;

		cbacks.bFlags[Callbacks::getPrimaryIndex] = true;
		cbacks.bFlags[Callbacks::getUpdaterConfigurationXml] = true;
		cbacks.bFlags[Callbacks::getSitesFileXml] = true;
		cbacks.bFlags[Callbacks::requestCredentialsForAuthorizationOnProxy] = true;
		//cbacks.bFlags[Callbacks::processProxyAuthorizationSuccess] = true;
		
		CREATE_EVENTS(requestCredentialsForAuthorizationOnProxy)
		//CREATE_EVENTS(processProxyAuthorizationSuccess)

		doUpdater do_upd(&upd, hDoUpdate);

		WAIT_FOR_READY_EVENT(requestCredentialsForAuthorizationOnProxy)
		TestCallbackInterface::_credentials creds;
		creds.name = L"test";
		creds.password = L"test";
		testCallback.SetRequestCreds(creds);
		SET_GOON_EVENT(requestCredentialsForAuthorizationOnProxy);

		//WAIT_FOR_READY_EVENT(processProxyAuthorizationSuccess)
		//QC_BOOST_CHECK_MESSAGE ( testCallback.GetConfirmCreds().name == creds.name &&
		//	testCallback.GetConfirmCreds().password == creds.password, L"Incorrect credentials\n" );
		//SET_GOON_EVENT(processProxyAuthorizationSuccess);

		WaitForSingleObject (hDoUpdate, INFINITE);

		QC_BOOST_CHECK_MESSAGE ( NO_ERROR == do_upd.GetError(), L"Wrong return code\n" );

		_helper::DeleteFile( std::wstring (UPD_ROOT).append(L"\\updcfg.xml") );

		QC_BOOST_CHECK_MESSAGE (_helper::DeleteFile(std::wstring(BASES_ROOT).append (L"\\base100.avc")),
								L"old base file not found");
		QC_BOOST_CHECK_MESSAGE (_helper::IsFoldersEqual(DIFFTOOL_ROOT,
			std::wstring(COLL_ROOT).append(L"\\tl-srv-w2k\\Bases_1\\plain").c_str(), BASES_ROOT), L"Incorrect bases\n");
		QC_BOOST_CHECK_MESSAGE (_helper::IsFoldersEqual(DIFFTOOL_ROOT,
			std::wstring(COLL_ROOT).append(L"\\tl-srv-w2k\\Bases_1\\data").c_str(), UPD_ROOT), L"Incorrect index\n");

	 }
}

void TestSuite::TestUpdateSchedule()
{
	using namespace KLUPD;
	std::wcout<<L"TestUpdateSchedule\n";
	Logger::Case _case (m_logger, m_testp, m_step, L"TestUpdateSchedule");

	{
		TestLog testLog;

		_helper::DeleteDirectory(ROOT);
		KLUPD::createFolder (BASES_ROOT, &testLog);
		KLUPD::createFolder (UPD_ROOT, &testLog);

		_helper::CopyFolder(std::wstring(COLL_ROOT).append(L"\\Bases_NULL\\plain").c_str(),BASES_ROOT);
		_helper::CloneFile (std::wstring(COLL_ROOT).append(L"\\Bases_NULL\\index\\6\\u0607g.xml").c_str(),
			std::wstring (UPD_ROOT).append (L"\\u0607g.xml"), false);

		UpdaterConfiguration data(1, 1, L"1");

		data.temporary_dir = TEMP_ROOT;
		//data.reserve_dir = RESERVE_ROOT;
		//data.update_root_folder = UPD_ROOT;
		//data.product_path = PRODUCT_ROOT;
		
		data.m_pathSubstitutionMap.push_back(std::make_pair<NoCaseString,NoCaseString>(L"%BaseFolder%", BASES_ROOT));
		data.m_pathSubstitutionMap.push_back(std::make_pair<NoCaseString,NoCaseString>(L"%Folder%", ROLLBACK_ROOT));
		data.m_pathSubstitutionMap.push_back(std::make_pair<NoCaseString,NoCaseString>(L"%UpdateRoot%", UPD_ROOT));
		data.m_pathSubstitutionMap.push_back(std::make_pair<NoCaseString,NoCaseString>(L"%DSKM%", DSKM_ROOT));
		data.m_pathSubstitutionMap.push_back(std::make_pair<NoCaseString,NoCaseString>(L"%DataFolder%", DATA_ROOT));

		data.self_region = L"<default>";
		data.product_lang.push_back( NoCaseString(L"<default>"));
		
		data.os.os = L"Windows";
		data.os.version = L"5.1";

		//data.m_componentsToUpdate.push_back (L"AVS");
		data.m_componentsToUpdate.push_back (L"CORE");
		data.m_componentsToUpdate.push_back (L"Updater");
		data.connect_tmo = 30;

		data.m_sourceList.addSource (UST_UserURL, std::wstring(COLL_ROOT).append(L"\\Bases_1").c_str(), true, L"diffs");
		data.UpdaterListFlags.update_bases = true;

		Callbacks cbacks;
		TestCallbackInterface testCallback (&testLog, &cbacks);
		TestJournalInterface testJournal;
		testCallback.SetUpdaterConfiguration (data);
		//testCallback.SetPrimaryIndex( FileInfo (L"u0607g.xml", L"updater13/bases_1/index/6", FileInfo::index, true) );
		
		FileInfo u0607g ( L"u0607g.xml", L"index/6", FileInfo::index, true );
		u0607g.m_localPath = UPD_ROOT;
		testCallback.SetPrimaryIndex(u0607g);
		FileInfo updcfg ( L"updcfg.xml", L"", FileInfo::base, false );
		testCallback.SetUpdcfgXml(updcfg);

		Updater upd (testCallback.m_dp, testCallback, testJournal, &testLog);
		HANDLE hDoUpdate;

		cbacks.bFlags[Callbacks::getPrimaryIndex] = true;
		cbacks.bFlags[Callbacks::getUpdaterConfigurationXml] = true;
		cbacks.bFlags[Callbacks::updateAutomaticScheduleForUpdater] = true;
		CREATE_EVENTS(updateAutomaticScheduleForUpdater)

		doUpdater do_upd(&upd, hDoUpdate);

		WAIT_FOR_READY_EVENT(updateAutomaticScheduleForUpdater)
		QC_BOOST_CHECK_MESSAGE ( testCallback.GetUpdaterSchedule().m_timeoutOnFailure == 1 &&
			testCallback.GetUpdaterSchedule().m_timeoutOnSuccess == 120, L"Incorrect UpdateSchedule\n" );
		SET_GOON_EVENT(updateAutomaticScheduleForUpdater)


		WaitForSingleObject (hDoUpdate, INFINITE);

		QC_BOOST_CHECK_MESSAGE ( NO_ERROR == do_upd.GetError(), L"Wrong return code\n" );

	 }
}

void TestSuite::TestUpdateVariables ()
{
	using namespace KLUPD;
	std::wcout<<L"TestUpdateVariables\n";
	Logger::Case _case (m_logger, m_testp, m_step, L"TestUpdateVariables");

	{
		TestLog testLog;

		_helper::DeleteDirectory(ROOT);

		UpdaterConfiguration data(1, 1, L"1");

		data.temporary_dir = TEMP_ROOT;
		//data.reserve_dir = RESERVE_ROOT;
		//data.update_root_folder = UPD_ROOT;

		data.m_pathSubstitutionMap.push_back(std::make_pair<NoCaseString,NoCaseString>(L"%BaseFolder%", BASES_ROOT));
		data.m_pathSubstitutionMap.push_back(std::make_pair<NoCaseString,NoCaseString>(L"%Folder%", ROLLBACK_ROOT));
		data.m_pathSubstitutionMap.push_back(std::make_pair<NoCaseString,NoCaseString>(L"%UpdateRoot%", UPD_ROOT));
		data.m_pathSubstitutionMap.push_back(std::make_pair<NoCaseString,NoCaseString>(L"%DSKM%", DSKM_ROOT));
		data.m_pathSubstitutionMap.push_back(std::make_pair<NoCaseString,NoCaseString>(L"%DataFolder%", DATA_ROOT));

		data.self_region = L"<default>";
		data.product_lang.push_back( NoCaseString(L"<default>"));
		
		data.os.os = L"Windows";
		data.os.version = L"5.1";

		data.m_componentsToUpdate.push_back (L"AVS");
		data.m_componentsToUpdate.push_back (L"CORE");
		data.connect_tmo = 30;

		data.m_sourceList.addSource (UST_UserURL, std::wstring(COLL_ROOT).append(L"\\Bases_vars").c_str(), true, L"diffs");
		data.UpdaterListFlags.update_bases = true;

		Callbacks cbacks;
		TestCallbackInterface testCallback (&testLog, &cbacks);
		TestJournalInterface testJournal;
		testCallback.SetUpdaterConfiguration (data);
		
		Updater upd (testCallback.m_dp, testCallback, testJournal, &testLog);
		HANDLE hDoUpdate;

		cbacks.bFlags[Callbacks::updateVariableFromIndex] = true;
		CREATE_EVENTS(updateVariableFromIndex)

		doUpdater do_upd(&upd, hDoUpdate);
		const std::vector<UpdatedSettings>& setts = testCallback.GetUpdatedSettings ();

		WAIT_FOR_READY_EVENT(updateVariableFromIndex)
		const UpdatedSettings& us = setts.back();
		QC_BOOST_CHECK_MESSAGE (us.m_name == L"Updater/update_date" && us.m_type == L"date" && us.m_value == L"31122006 1500" ,L"Incorrect UpdatedSettings");
		SET_GOON_EVENT(updateVariableFromIndex)

		WAIT_FOR_READY_EVENT(updateVariableFromIndex)
		const UpdatedSettings& us2 = setts.back();
		QC_BOOST_CHECK_MESSAGE (us2.m_name == L"Updater/av_records" && us2.m_type == L"long" && us2.m_value == L"99999" ,L"Incorrect UpdatedSettings");
		SET_GOON_EVENT(updateVariableFromIndex)

		WaitForSingleObject (hDoUpdate, INFINITE);

		QC_BOOST_CHECK_MESSAGE ( NO_ERROR == do_upd.GetError(), L"Wrong return code\n" );
	}
}

void TestSuite::TestRefreshIds ()
{
	using namespace KLUPD;
	std::wcout<<L"TestRefreshIds\n";
	Logger::Case _case (m_logger, m_testp, m_step, L"TestRefreshIds");

	return; // неактуально
	//QC_BOOST_REQUIRE_MESSAGE ( false, L"Bug 17698\n" );
	{
		TestLog testLog;

		_helper::DeleteDirectory(ROOT);
		KLUPD::createFolder (BASES_ROOT, &testLog);
		KLUPD::createFolder (UPD_ROOT, &testLog);
		KLUPD::createFolder (PRODUCT_ROOT, &testLog);

		_helper::CopyFolder(std::wstring(COLL_ROOT).append(L"\\Bases_NULL\\plain").c_str(),BASES_ROOT);
		_helper::CloneFile (std::wstring(COLL_ROOT).append(L"\\Bases_NULL\\index\\6\\u0607g.xml").c_str(),
			std::wstring (UPD_ROOT).append (L"\\u0607g.xml"), false);

		_helper::CloneFile(std::wstring(COLL_ROOT).append(L"\\Updcfgs\\sites.xml").c_str(),
			std::wstring (PRODUCT_ROOT).append(L"\\sites.xml"), false);
		_helper::CloneFile(std::wstring(COLL_ROOT).append(L"\\Updcfgs\\updcfg.xml").c_str(),
			std::wstring (UPD_ROOT).append(L"\\updcfg.xml"), false);

		UpdaterConfiguration data(1, 1, L"1");

		data.temporary_dir = TEMP_ROOT;
		//data.reserve_dir = RESERVE_ROOT;
		//data.update_root_folder = UPD_ROOT;

		data.m_pathSubstitutionMap.push_back(std::make_pair<NoCaseString,NoCaseString>(L"%BaseFolder%", BASES_ROOT));
		data.m_pathSubstitutionMap.push_back(std::make_pair<NoCaseString,NoCaseString>(L"%Folder%", ROLLBACK_ROOT));
		data.m_pathSubstitutionMap.push_back(std::make_pair<NoCaseString,NoCaseString>(L"%UpdateRoot%", UPD_ROOT));
		data.m_pathSubstitutionMap.push_back(std::make_pair<NoCaseString,NoCaseString>(L"%DSKM%", DSKM_ROOT));
		data.m_pathSubstitutionMap.push_back(std::make_pair<NoCaseString,NoCaseString>(L"%DataFolder%", DATA_ROOT));

		data.self_region = L"<default>";
		data.product_lang.push_back( NoCaseString(L"<default>"));
		
		data.os.os = L"Windows";
		data.os.version = L"5.1";

		data.m_componentsToUpdate.push_back (L"AVS");
		data.m_componentsToUpdate.push_back (L"CORE");
		data.connect_tmo = 30;
		data.m_sourceList.addSource (UST_KLServer, L"", true, L"diffs");
		data.UpdaterListFlags.update_bases = true;

		Callbacks cbacks;
		TestCallbackInterface testCallback (&testLog, &cbacks);
		TestJournalInterface testJournal;
		testCallback.SetUpdaterConfiguration (data);
		testCallback.SetPrimaryIndex( FileInfo (L"u0607g.xml", L"updater13/bases_1/index/6", FileInfo::index, true) );
		
		FileInfo u0607g ( L"u0607g.xml", L"updater13/bases_1/index/6", FileInfo::index, true );
		testCallback.SetPrimaryIndex(u0607g);
		
		FileInfo updcfg ( L"updcfg.xml", L"updater13/bases_1/index/6", FileInfo::base, false );
		testCallback.SetUpdcfgXml(updcfg);

		FileInfo sites ( L"sites.xml", L"updater13/bases_1/index/6", FileInfo::base, false );
		testCallback.SetSitesXml(sites);
		
		Updater upd (testCallback.m_dp, testCallback, testJournal, &testLog);
		HANDLE hDoUpdate;

		cbacks.bFlags[Callbacks::refreshIdentifier] = true;
		cbacks.bFlags[Callbacks::getPrimaryIndex] = true;
		cbacks.bFlags[Callbacks::getUpdaterConfigurationXml] = true;
		cbacks.bFlags[Callbacks::getSitesFileXml] = true;
		//CREATE_EVENTS(updateVariableFromIndex)

		doUpdater do_upd(&upd, hDoUpdate);

		WaitForSingleObject (hDoUpdate, INFINITE);

		QC_BOOST_CHECK_MESSAGE ( NO_ERROR == do_upd.GetError(), L"Wrong return code\n" );
	}
}


void TestSuite::TestRollback()
{
	using namespace KLUPD;
	std::wcout<<L"TestRollback\n";
	Logger::Case _case (m_logger, m_testp, m_step, L"TestRollback");

	//Update
	{
		std::wcout<<L"Update\n";
		TestLog testLog;

		_helper::DeleteDirectory(ROOT);
		KLUPD::createFolder (BASES_ROOT, &testLog);
		KLUPD::createFolder (UPD_ROOT, &testLog);

		_helper::CopyFolder(std::wstring(COLL_ROOT).append(L"\\Bases_NULL_NEW\\plain").c_str(),BASES_ROOT);
		_helper::CloneFile (std::wstring(COLL_ROOT).append(L"\\Bases_NULL_NEW\\index\\6\\u0607g.xml").c_str(),
			std::wstring (UPD_ROOT).append (L"\\u0607g.xml"), false);

		UpdaterConfiguration data(1, 1, L"1");

		data.temporary_dir = TEMP_ROOT;
		//data.reserve_dir = RESERVE_ROOT;
		//data.update_root_folder = UPD_ROOT;
		//data.product_path = PRODUCT_ROOT;
		
		data.m_pathSubstitutionMap.push_back(std::make_pair<NoCaseString,NoCaseString>(L"%BaseFolder%", BASES_ROOT));
		data.m_pathSubstitutionMap.push_back(std::make_pair<NoCaseString,NoCaseString>(L"%Folder%", ROLLBACK_ROOT));
		data.m_pathSubstitutionMap.push_back(std::make_pair<NoCaseString,NoCaseString>(L"%UpdateRoot%", UPD_ROOT));
		data.m_pathSubstitutionMap.push_back(std::make_pair<NoCaseString,NoCaseString>(L"%DSKM%", DSKM_ROOT));
		data.m_pathSubstitutionMap.push_back(std::make_pair<NoCaseString,NoCaseString>(L"%DataFolder%", UPD_ROOT));

		data.self_region = L"<default>";
		data.product_lang.push_back( NoCaseString(L"<default>"));
		
		data.os.os = L"Windows";
		data.os.version = L"5.1";

		data.m_componentsToUpdate.push_back (L"AVS");
		data.m_componentsToUpdate.push_back (L"CORE");
		//data.m_componentsToUpdate.push_back (L"Updater");
		data.connect_tmo = 30;

		data.m_sourceList.addSource (UST_UserURL, std::wstring(COLL_ROOT).append(L"\\Bases_1").c_str(), true, L"diffs");
		data.UpdaterListFlags.update_bases = true;

		Callbacks cbacks;
		TestCallbackInterface testCallback (&testLog, &cbacks);
		TestJournalInterface testJournal;
		testCallback.SetUpdaterConfiguration (data);
		
		FileInfo u0607g ( L"u0607g.xml", L"index/6", FileInfo::index, true );
		u0607g.m_localPath = UPD_ROOT;
		testCallback.SetPrimaryIndex(u0607g);

		cbacks.bFlags[Callbacks::getPrimaryIndex] = true;
		cbacks.bFlags[Callbacks::processReceivedFiles] = true;
		CREATE_EVENTS(processReceivedFiles)
		
		Updater upd (testCallback.m_dp, testCallback, testJournal, &testLog);

		//Update

		HANDLE hDoUpdate;
		IniFile Ini_file (ROLLBACK_FILE, &testLog);
		IniBasedRollbackFile Ibrf (Ini_file, &testLog);

		testCallback.SetRollbackFile(&Ibrf);
		testCallback.SetRollbackFolder(RESERVE_ROOT);

		doUpdater do_upd(&upd, hDoUpdate);

		WAIT_FOR_READY_EVENT(processReceivedFiles)
		SET_GOON_EVENT(processReceivedFiles)

		WaitForSingleObject (hDoUpdate, INFINITE);

		QC_BOOST_CHECK_MESSAGE ( NO_ERROR == do_upd.GetError(), L"Wrong return code\n" );
	}

	//Rollback
	{
		std::wcout<<L"Rollback\n";
		TestLog testLog;

		UpdaterConfiguration data(1, 1, L"1");

		data.temporary_dir = TEMP_ROOT;
		//data.reserve_dir = RESERVE_ROOT;
		//data.update_root_folder = UPD_ROOT;
		//data.product_path = PRODUCT_ROOT;
		
		data.m_pathSubstitutionMap.push_back(std::make_pair<NoCaseString,NoCaseString>(L"%BaseFolder%", BASES_ROOT));
		data.m_pathSubstitutionMap.push_back(std::make_pair<NoCaseString,NoCaseString>(L"%Folder%", ROLLBACK_ROOT));
		data.m_pathSubstitutionMap.push_back(std::make_pair<NoCaseString,NoCaseString>(L"%UpdateRoot%", UPD_ROOT));
		data.m_pathSubstitutionMap.push_back(std::make_pair<NoCaseString,NoCaseString>(L"%DSKM%", DSKM_ROOT));
		data.m_pathSubstitutionMap.push_back(std::make_pair<NoCaseString,NoCaseString>(L"%DataFolder%", UPD_ROOT));

		data.self_region = L"<default>";
		data.product_lang.push_back( NoCaseString(L"<default>"));
		
		data.os.os = L"Windows";
		data.os.version = L"5.1";

		data.m_componentsToUpdate.push_back (L"AVS");
		data.m_componentsToUpdate.push_back (L"CORE");
		//data.m_componentsToUpdate.push_back (L"Updater");
		data.connect_tmo = 30;

		data.m_sourceList.addSource (UST_UserURL, std::wstring(COLL_ROOT).append(L"\\Bases_1").c_str(), true, L"diffs");
		data.UpdaterListFlags.update_bases = true;

		Callbacks cbacks;
		TestCallbackInterface testCallback (&testLog, &cbacks);
		TestJournalInterface testJournal;
		testCallback.SetUpdaterConfiguration (data);
		
		//FileInfo u0607g ( L"u0607g.xml", L"index/6", FileInfo::index, true );
		//u0607g.m_localPath = UPD_ROOT;
		//testCallback.SetPrimaryIndex(u0607g);

		//cbacks.bFlags[Callbacks::getPrimaryIndex] = true;
		cbacks.bFlags[Callbacks::readRollbackFiles] = true;
		CREATE_EVENTS(readRollbackFiles)
		
		Updater upd (testCallback.m_dp, testCallback, testJournal, &testLog);

		//Rollback

		HANDLE hDoRollback;
		IniFile Ini_file (ROLLBACK_FILE, &testLog);
		IniBasedRollbackFile Ibrf (Ini_file, &testLog);

		doRollbacker do_rbc(&upd, hDoRollback);

		WAIT_FOR_READY_EVENT(readRollbackFiles)
		testCallback.SetRollbackFile(&Ibrf);
		SET_GOON_EVENT(readRollbackFiles)

		WaitForSingleObject (hDoRollback, INFINITE);

		QC_BOOST_CHECK_MESSAGE ( NO_ERROR == do_rbc.GetError(), L"Wrong return code\n" );

		QC_BOOST_CHECK_MESSAGE (_helper::IsFoldersEqual(DIFFTOOL_ROOT,
			std::wstring(COLL_ROOT).append(L"\\Bases_NULL_NEW\\plain_rbacked").c_str(), BASES_ROOT), L"Incorrect bases\n");
		QC_BOOST_CHECK_MESSAGE (_helper::IsFoldersEqual(DIFFTOOL_ROOT,
			std::wstring(COLL_ROOT).append(L"\\Bases_NULL_NEW\\data").c_str(), UPD_ROOT), L"Incorrect bases\n");

	}
}

void TestSuite::TestRollbackListCases()
{
	using namespace KLUPD;
	std::wcout<<L"TestRollbackListCases\n";
	Logger::Case _case (m_logger, m_testp, m_step, L"TestRollbackListCases");

	{
		std::wcout<<L"Corrupted bases set\n";
		TestLog testLog;

		_helper::DeleteDirectory(ROOT);
		KLUPD::createFolder (BASES_ROOT, &testLog);
		KLUPD::createFolder (UPD_ROOT, &testLog);

		_helper::DeleteFile(ROLLBACK_FILE);

		_helper::CopyFolder(std::wstring(COLL_ROOT).append(L"\\Bases_NULL_NEW\\plain").c_str(),BASES_ROOT);

		UpdaterConfiguration data(1, 1, L"1");

		data.temporary_dir = TEMP_ROOT;
		//data.reserve_dir = RESERVE_ROOT;
		//data.update_root_folder = UPD_ROOT;
		//data.product_path = PRODUCT_ROOT;
		
		data.m_pathSubstitutionMap.push_back(std::make_pair<NoCaseString,NoCaseString>(L"%BaseFolder%", BASES_ROOT));
		data.m_pathSubstitutionMap.push_back(std::make_pair<NoCaseString,NoCaseString>(L"%Folder%", ROLLBACK_ROOT));
		data.m_pathSubstitutionMap.push_back(std::make_pair<NoCaseString,NoCaseString>(L"%UpdateRoot%", UPD_ROOT));
		data.m_pathSubstitutionMap.push_back(std::make_pair<NoCaseString,NoCaseString>(L"%DSKM%", DSKM_ROOT));
		data.m_pathSubstitutionMap.push_back(std::make_pair<NoCaseString,NoCaseString>(L"%DataFolder%", UPD_ROOT));

		data.self_region = L"<default>";
		data.product_lang.push_back( NoCaseString(L"<default>"));
		
		data.os.os = L"Windows";
		data.os.version = L"5.1";

		data.m_componentsToUpdate.push_back (L"AVS");
		data.m_componentsToUpdate.push_back (L"CORE");
		//data.m_componentsToUpdate.push_back (L"Updater");
		data.connect_tmo = 30;

		data.m_sourceList.addSource (UST_UserURL, std::wstring(COLL_ROOT).append(L"\\Bases_1").c_str(), true, L"diffs");
		data.UpdaterListFlags.update_bases = true;

		Callbacks cbacks;
		TestCallbackInterface testCallback (&testLog, &cbacks);
		TestJournalInterface testJournal;
		testCallback.SetUpdaterConfiguration (data);
		
		FileInfo u0607g ( L"u0607g.xml", L"index/6", FileInfo::index, true );
		u0607g.m_localPath = UPD_ROOT;
		testCallback.SetPrimaryIndex(u0607g);

		cbacks.bFlags[Callbacks::getPrimaryIndex] = true;
		cbacks.bFlags[Callbacks::processReceivedFiles] = true;
		CREATE_EVENTS(processReceivedFiles)
		
		Updater upd (testCallback.m_dp, testCallback, testJournal, &testLog);

		//Update

		HANDLE hDoUpdate;
		IniFile Ini_file (L"c:\\rollback.ini", &testLog);
		IniBasedRollbackFile Ibrf (Ini_file, &testLog);

		testCallback.SetRollbackFile(&Ibrf);
		testCallback.SetRollbackFolder(RESERVE_ROOT);

		doUpdater do_upd(&upd, hDoUpdate);

		WAIT_FOR_READY_EVENT(processReceivedFiles)
		SET_GOON_EVENT(processReceivedFiles)

		WaitForSingleObject (hDoUpdate, INFINITE);

		FileVector to_rollback;
		Ibrf.readRollback(to_rollback);

		QC_BOOST_CHECK_MESSAGE (to_rollback.size() == 0, L"Incorrect rollback list");
		QC_BOOST_CHECK_MESSAGE ( NO_ERROR == do_upd.GetError(), L"Wrong return code\n" );
	}
}

void TestSuite::TestExpandStrings ()
{
	using namespace KLUPD;
	std::wcout<<L"TestExpandStrings\n";
	Logger::Case _case (m_logger, m_testp, m_step, L"TestExpandStrings");

	{
		std::wcout<<L"Using substitution map\n";
		TestLog testLog;

		_helper::DeleteDirectory(ROOT);
		KLUPD::createFolder (BASES_ROOT, &testLog);
		KLUPD::createFolder (UPD_ROOT, &testLog);

		_helper::CopyFolder(std::wstring(COLL_ROOT).append(L"\\Bases_NULL\\plain").c_str(),BASES_ROOT);
		_helper::CloneFile (std::wstring(COLL_ROOT).append(L"\\Bases_NULL\\index\\6\\u0607g.xml").c_str(),
			std::wstring (UPD_ROOT).append (L"\\u0607g.xml"), false);

		UpdaterConfiguration data(1, 1, L"1");

		data.temporary_dir = TEMP_ROOT;
		//data.reserve_dir = RESERVE_ROOT;
		//data.update_root_folder = UPD_ROOT;

		data.m_pathSubstitutionMap.push_back(std::make_pair<NoCaseString,NoCaseString>(L"%BaseFolder%", BASES_ROOT));
		data.m_pathSubstitutionMap.push_back(std::make_pair<NoCaseString,NoCaseString>(L"%Folder%", ROLLBACK_ROOT));
		data.m_pathSubstitutionMap.push_back(std::make_pair<NoCaseString,NoCaseString>(L"%UpdateRoot%", UPD_ROOT));
		data.m_pathSubstitutionMap.push_back(std::make_pair<NoCaseString,NoCaseString>(L"%DSKM%", DSKM_ROOT));
		data.m_pathSubstitutionMap.push_back(std::make_pair<NoCaseString,NoCaseString>(L"%DataFolder%", UPD_ROOT));

		data.self_region = L"<default>";
		data.product_lang.push_back( NoCaseString(L"<default>"));
		
		data.os.os = L"Windows";
		data.os.version = L"5.1";

		data.m_componentsToUpdate.push_back (L"AVS");
		data.m_componentsToUpdate.push_back (L"CORE");
		data.connect_tmo = 30;

		data.m_sourceList.addSource (UST_UserURL, std::wstring(COLL_ROOT).append(L"\\Bases_1").c_str(), true, L"diffs");
		data.UpdaterListFlags.update_bases = true;

		Callbacks cbacks;
		TestCallbackInterface testCallback (&testLog, &cbacks);
		TestJournalInterface testJournal;
		testCallback.SetUpdaterConfiguration (data);
		
		Updater upd (testCallback.m_dp, testCallback, testJournal, &testLog);
		HANDLE hDoUpdate;

		cbacks.bFlags[Callbacks::expandEnvironmentString] = true;

		doUpdater do_upd(&upd, hDoUpdate);

		WaitForSingleObject (hDoUpdate, INFINITE);

		
		QC_BOOST_CHECK_MESSAGE (testCallback.GetExpandStringsCalls () == 0, L"Incorrect ExpandStrings callback calls count\n");

		QC_BOOST_CHECK_MESSAGE ( NO_ERROR == do_upd.GetError(), L"Wrong return code\n" );

		QC_BOOST_CHECK_MESSAGE (_helper::DeleteFile(std::wstring(BASES_ROOT).append (L"\\base100.avc")),
								L"old base file not found");
		QC_BOOST_CHECK_MESSAGE (_helper::IsFoldersEqual(DIFFTOOL_ROOT,
			std::wstring(COLL_ROOT).append(L"\\Bases_1\\plain").c_str(), BASES_ROOT), L"Incorrect bases\n");
		QC_BOOST_CHECK_MESSAGE (_helper::IsFoldersEqual(DIFFTOOL_ROOT,
			std::wstring(COLL_ROOT).append(L"\\Bases_1\\data").c_str(), UPD_ROOT), L"Incorrect index\n");
	}
	{
		std::wcout<<L"Using ExpandEnvironmentString callback\n";
		TestLog testLog;

		_helper::DeleteDirectory(ROOT);
		KLUPD::createFolder (BASES_ROOT, &testLog);
		KLUPD::createFolder (UPD_ROOT, &testLog);

		_helper::CopyFolder(std::wstring(COLL_ROOT).append(L"\\Bases_NULL\\plain").c_str(),BASES_ROOT);
		_helper::CloneFile (std::wstring(COLL_ROOT).append(L"\\Bases_NULL\\index\\6\\u0607g.xml").c_str(),
			std::wstring (UPD_ROOT).append (L"\\u0607g.xml"), false);

		UpdaterConfiguration data(1, 1, L"1");

		data.temporary_dir = TEMP_ROOT;
		//data.reserve_dir = RESERVE_ROOT;
		//data.update_root_folder = UPD_ROOT;

		data.self_region = L"<default>";
		data.product_lang.push_back( NoCaseString(L"<default>"));
		
		data.os.os = L"Windows";
		data.os.version = L"5.1";

		data.m_componentsToUpdate.push_back (L"AVS");
		data.m_componentsToUpdate.push_back (L"CORE");
		data.connect_tmo = 30;

		data.m_sourceList.addSource (UST_UserURL, std::wstring(COLL_ROOT).append(L"\\Bases_1").c_str(), true, L"diffs");
		data.UpdaterListFlags.update_bases = true;

		PathSubstitutionMap ps_map;
		ps_map.push_back(std::make_pair<NoCaseString,NoCaseString>(L"BaseFolder", BASES_ROOT));
		ps_map.push_back(std::make_pair<NoCaseString,NoCaseString>(L"Folder", ROLLBACK_ROOT));
		ps_map.push_back(std::make_pair<NoCaseString,NoCaseString>(L"UpdateRoot", UPD_ROOT));
		ps_map.push_back(std::make_pair<NoCaseString,NoCaseString>(L"DSKM", DSKM_ROOT));
		ps_map.push_back(std::make_pair<NoCaseString,NoCaseString>(L"DataFolder", UPD_ROOT));

		Callbacks cbacks;
		TestCallbackInterface testCallback (&testLog, &cbacks);
		TestJournalInterface testJournal;
		testCallback.SetUpdaterConfiguration (data);
		testCallback.SetSubstitutionMap (ps_map);
		
		Updater upd (testCallback.m_dp, testCallback, testJournal, &testLog);
		HANDLE hDoUpdate;

		cbacks.bFlags[Callbacks::expandEnvironmentString] = true;

		doUpdater do_upd(&upd, hDoUpdate);

		WaitForSingleObject (hDoUpdate, INFINITE);

		QC_BOOST_CHECK_MESSAGE (testCallback.GetExpandStringsCalls () != 0, L"Incorrect ExpandStrings callback calls count\n");

		QC_BOOST_CHECK_MESSAGE ( NO_ERROR == do_upd.GetError(), L"Wrong return code\n" );

		QC_BOOST_CHECK_MESSAGE (_helper::DeleteFile(std::wstring(BASES_ROOT).append (L"\\base100.avc")),
								L"old base file not found");
		QC_BOOST_CHECK_MESSAGE (_helper::IsFoldersEqual(DIFFTOOL_ROOT,
			std::wstring(COLL_ROOT).append(L"\\Bases_1\\plain").c_str(), BASES_ROOT), L"Incorrect bases\n");
		QC_BOOST_CHECK_MESSAGE (_helper::IsFoldersEqual(DIFFTOOL_ROOT,
			std::wstring(COLL_ROOT).append(L"\\Bases_1\\data").c_str(), UPD_ROOT), L"Incorrect index\n");
	}

}

void TestSuite::TestSubEnvrnmnt ()
{
	using namespace KLUPD;
	std::wcout<<L"TestSubEnvrnmnt\n";
	Logger::Case _case (m_logger, m_testp, m_step, L"TestSubEnvrnmnt");

	{
		std::wcout<<L"Double subst bug (17852)\n";
		TestLog testLog;

		_helper::DeleteDirectory(ROOT);

		UpdaterConfiguration data(1, 1, L"1");

		data.temporary_dir = TEMP_ROOT;
		//data.reserve_dir = RESERVE_ROOT;
		//data.update_root_folder = UPD_ROOT;
		data.retranslation_dir = RETR_ROOT;

		data.m_pathSubstitutionMap.push_back(std::make_pair<NoCaseString,NoCaseString>(L"%BaseFolder%", BASES_ROOT));
		data.m_pathSubstitutionMap.push_back(std::make_pair<NoCaseString,NoCaseString>(L"%Folder%", ROLLBACK_ROOT));
		data.m_pathSubstitutionMap.push_back(std::make_pair<NoCaseString,NoCaseString>(L"%UpdateRoot%", UPD_ROOT));
		data.m_pathSubstitutionMap.push_back(std::make_pair<NoCaseString,NoCaseString>(L"%DSKM%", DSKM_ROOT));
		data.m_pathSubstitutionMap.push_back(std::make_pair<NoCaseString,NoCaseString>(L"%Data%", DATA_ROOT));
		data.m_pathSubstitutionMap.push_back(std::make_pair<NoCaseString,NoCaseString>(L"%DataFolder%", UPD_ROOT));
		data.m_pathSubstitutionMap.push_back(std::make_pair<NoCaseString,NoCaseString>(L"%Localization%", L"fr"));

		data.self_region = L"<default>";
		data.product_lang.push_back( NoCaseString(L"<default>"));
		
		data.os.os = L"Windows";
		data.os.version = L"5.1";

		data.m_componentsToUpdate.push_back (L"RT");
		data.m_componentsToUpdate.push_back (L"CORE");
		data.connect_tmo = 30;

		data.m_sourceList.addSource (UST_UserURL, std::wstring(COLL_ROOT).append(L"\\Bases_1").c_str(), true, L"diffs");
		data.UpdaterListFlags.update_bases = true;

		PathSubstitutionMap ps_map;
		ps_map.push_back(std::make_pair<NoCaseString,NoCaseString>(L"Data", DATA_ROOT));

		Callbacks cbacks;
		TestCallbackInterface testCallback (&testLog, &cbacks);
		TestJournalInterface testJournal;
		testCallback.SetUpdaterConfiguration (data);
		testCallback.SetSubstitutionMap (ps_map);
		
		Updater upd (testCallback.m_dp, testCallback, testJournal, &testLog);
		HANDLE hDoUpdate;

		cbacks.bFlags[Callbacks::expandEnvironmentString] = true;

		doUpdater do_upd(&upd, hDoUpdate);

		WaitForSingleObject (hDoUpdate, INFINITE);

		//QC_BOOST_CHECK_MESSAGE (testCallback.GetExpandStringsCalls () == 0, L"Incorrect ExpandStrings callback calls count\n");

		QC_BOOST_CHECK_MESSAGE ( NO_ERROR == do_upd.GetError(), L"Wrong return code\n" );

		QC_BOOST_CHECK_MESSAGE (_helper::IsFoldersEqual(DIFFTOOL_ROOT,
			std::wstring(COLL_ROOT).append(L"\\Bases_1\\rt_bases").c_str(), BASES_ROOT), L"Incorrect bases\n");
		QC_BOOST_CHECK_MESSAGE (_helper::IsFoldersEqual(DIFFTOOL_ROOT,
			std::wstring(COLL_ROOT).append(L"\\Bases_1\\data").c_str(), UPD_ROOT), L"Incorrect index\n");
		QC_BOOST_CHECK_MESSAGE (_helper::IsFoldersEqual(DIFFTOOL_ROOT,
			std::wstring(COLL_ROOT).append(L"\\Bases_1\\rt_fr").c_str(), DATA_ROOT), L"Incorrect bases\n");
	}
	
	{
		std::wcout<<L"Default env variable value test\n";
		std::wcout<<L"There is a workaround for bug 17852\n";
		TestLog testLog;

		_helper::DeleteDirectory(ROOT);

		UpdaterConfiguration data(1, 1, L"1");

		data.temporary_dir = TEMP_ROOT;
		//data.reserve_dir = RESERVE_ROOT;
		//data.update_root_folder = UPD_ROOT;
		data.retranslation_dir = RETR_ROOT;

		data.m_pathSubstitutionMap.push_back(std::make_pair<NoCaseString,NoCaseString>(L"%BaseFolder%", BASES_ROOT));
		data.m_pathSubstitutionMap.push_back(std::make_pair<NoCaseString,NoCaseString>(L"%Folder%", ROLLBACK_ROOT));
		data.m_pathSubstitutionMap.push_back(std::make_pair<NoCaseString,NoCaseString>(L"%UpdateRoot%", UPD_ROOT));
		data.m_pathSubstitutionMap.push_back(std::make_pair<NoCaseString,NoCaseString>(L"%DSKM%", DSKM_ROOT));
		data.m_pathSubstitutionMap.push_back(std::make_pair<NoCaseString,NoCaseString>(L"%Data%", DATA_ROOT));
		data.m_pathSubstitutionMap.push_back(std::make_pair<NoCaseString,NoCaseString>(L"%DataFolder%", UPD_ROOT));

		data.self_region = L"<default>";
		data.product_lang.push_back( NoCaseString(L"<default>"));
		
		data.os.os = L"Windows";
		data.os.version = L"5.1";

		data.m_componentsToUpdate.push_back (L"RT");
		data.m_componentsToUpdate.push_back (L"CORE");
		data.connect_tmo = 30;

		data.m_sourceList.addSource (UST_UserURL, std::wstring(COLL_ROOT).append(L"\\Bases_1").c_str(), true, L"diffs");
		data.UpdaterListFlags.update_bases = true;

		PathSubstitutionMap ps_map;
		ps_map.push_back(std::make_pair<NoCaseString,NoCaseString>(L"Data", DATA_ROOT));

		Callbacks cbacks;
		TestCallbackInterface testCallback (&testLog, &cbacks);
		TestJournalInterface testJournal;
		testCallback.SetUpdaterConfiguration (data);
		testCallback.SetSubstitutionMap (ps_map);
		
		Updater upd (testCallback.m_dp, testCallback, testJournal, &testLog);
		HANDLE hDoUpdate;

		cbacks.bFlags[Callbacks::expandEnvironmentString] = true;

		doUpdater do_upd(&upd, hDoUpdate);

		WaitForSingleObject (hDoUpdate, INFINITE);

		//QC_BOOST_CHECK_MESSAGE (testCallback.GetExpandStringsCalls () == 1, L"Incorrect ExpandStrings callback calls count\n");

		QC_BOOST_CHECK_MESSAGE ( NO_ERROR == do_upd.GetError(), L"Wrong return code\n" );

		QC_BOOST_CHECK_MESSAGE (_helper::IsFoldersEqual(DIFFTOOL_ROOT,
			std::wstring(COLL_ROOT).append(L"\\Bases_1\\rt_bases").c_str(), BASES_ROOT), L"Incorrect bases\n");
		QC_BOOST_CHECK_MESSAGE (_helper::IsFoldersEqual(DIFFTOOL_ROOT,
			std::wstring(COLL_ROOT).append(L"\\Bases_1\\data").c_str(), UPD_ROOT), L"Incorrect index\n");
		QC_BOOST_CHECK_MESSAGE (_helper::IsFoldersEqual(DIFFTOOL_ROOT,
			std::wstring(COLL_ROOT).append(L"\\Bases_1\\rt_en").c_str(), DATA_ROOT), L"Incorrect bases\n");
	}

}



//////////////


void TestSuite::TestFtpSourceUrlWithCreds ()
{
	using namespace KLUPD;
	std::wcout<<L"TestFtpSourceWithCreds\n";
	Logger::Case _case (m_logger, m_testp, m_step, L"TestFtpSourceUrlWithCreds");

	{
		TestLog testLog;

		_helper::DeleteDirectory(ROOT);
		KLUPD::createFolder (BASES_ROOT, &testLog);
		KLUPD::createFolder (UPD_ROOT, &testLog);
		KLUPD::createFolder (PRODUCT_ROOT, &testLog);

		UpdaterConfiguration data(1, 1, L"1");

		data.temporary_dir = TEMP_ROOT;
		//data.reserve_dir = RESERVE_ROOT;
		//data.update_root_folder = UPD_ROOT;
		//data.product_path = PRODUCT_ROOT;

		data.m_pathSubstitutionMap.push_back(std::make_pair<NoCaseString,NoCaseString>(L"%BaseFolder%", BASES_ROOT));
		data.m_pathSubstitutionMap.push_back(std::make_pair<NoCaseString,NoCaseString>(L"%Folder%", ROLLBACK_ROOT));
		data.m_pathSubstitutionMap.push_back(std::make_pair<NoCaseString,NoCaseString>(L"%UpdateRoot%", UPD_ROOT));
		data.m_pathSubstitutionMap.push_back(std::make_pair<NoCaseString,NoCaseString>(L"%DSKM%", DSKM_ROOT));
		data.m_pathSubstitutionMap.push_back(std::make_pair<NoCaseString,NoCaseString>(L"%DataFolder%", UPD_ROOT));

		data.self_region = L"<default>";
		data.product_lang.push_back( NoCaseString(L"<default>"));
		
		data.os.os = L"Windows";
		data.os.version = L"5.1";

		data.m_componentsToUpdate.push_back (L"AVS");
		data.m_componentsToUpdate.push_back (L"CORE");
		data.connect_tmo = 30;

		data.m_sourceList.addSource (UST_UserURL, L"ftp://test:test@tl-srv-wxp/updater13restricted/bases_1", false, L"diffs");
		data.UpdaterListFlags.update_bases = true;

		Callbacks cbacks;
		TestCallbackInterface testCallback (&testLog, &cbacks);
		TestJournalInterface testJournal;
		testCallback.SetUpdaterConfiguration (data);
		testCallback.SetPrimaryIndex( FileInfo (L"u0607g.xml", L"index/6", FileInfo::index, true) );
		
		FileInfo u0607g ( L"u0607g.xml", L"index/6", FileInfo::index, true );
		//u0607g.m_localPath = UPD_ROOT;
		testCallback.SetPrimaryIndex(u0607g);
		
		Updater upd (testCallback.m_dp, testCallback, testJournal, &testLog);
		HANDLE hDoUpdate;

		cbacks.bFlags[Callbacks::getPrimaryIndex] = true;
		
		doUpdater do_upd(&upd, hDoUpdate);

		WaitForSingleObject (hDoUpdate, INFINITE);

		QC_BOOST_CHECK_MESSAGE ( NO_ERROR == do_upd.GetError(), L"Wrong return code\n" );

		QC_BOOST_CHECK_MESSAGE (_helper::IsFoldersEqual(DIFFTOOL_ROOT,
			std::wstring(COLL_ROOT).append(L"\\tl-srv-wxp\\Bases_1\\plain").c_str(), BASES_ROOT), L"Incorrect bases\n");
		QC_BOOST_CHECK_MESSAGE (_helper::IsFoldersEqual(DIFFTOOL_ROOT,
			std::wstring(COLL_ROOT).append(L"\\tl-srv-wxp\\Bases_1\\data").c_str(), UPD_ROOT), L"Incorrect index\n");
	}
}

void TestSuite::TestCaseTT17598 ()
{
	using namespace KLUPD;
	std::wcout<<L"TestCaseTT17598\n";
	Logger::Case _case (m_logger, m_testp, m_step, L"TestCaseTT17598");
	std::wcout<<L"Unpack collection from archive\n";
	{
		TestLog testLog;

		_helper::DeleteDirectory(ROOT);
		KLUPD::createFolder (BASES_ROOT, &testLog);
		KLUPD::createFolder (UPD_ROOT, &testLog);
		KLUPD::createFolder (PRODUCT_ROOT, &testLog);

		_helper::CopyFolder(std::wstring(COLL_ROOT).append(L"\\Bases_NULL\\plain").c_str(),BASES_ROOT);
		_helper::CloneFile (std::wstring(COLL_ROOT).append(L"\\Bases_NULL\\index\\6\\u0607g.xml").c_str(),
			std::wstring (UPD_ROOT).append (L"\\u0607g.xml"), false);

		UpdaterConfiguration data(1, 1, L"1");

		data.temporary_dir = TEMP_ROOT;
		//data.reserve_dir = RESERVE_ROOT;
		//data.update_root_folder = UPD_ROOT;
		//data.product_path = PRODUCT_ROOT;

		data.m_pathSubstitutionMap.push_back(std::make_pair<NoCaseString,NoCaseString>(L"%BaseFolder%", BASES_ROOT));
		data.m_pathSubstitutionMap.push_back(std::make_pair<NoCaseString,NoCaseString>(L"%Folder%", ROLLBACK_ROOT));
		data.m_pathSubstitutionMap.push_back(std::make_pair<NoCaseString,NoCaseString>(L"%UpdateRoot%", UPD_ROOT));
		data.m_pathSubstitutionMap.push_back(std::make_pair<NoCaseString,NoCaseString>(L"%DSKM%", DSKM_ROOT));

		data.self_region = L"<default>";
		data.product_lang.push_back( NoCaseString(L"<default>"));
		
		data.os.os = L"Windows";
		data.os.version = L"5.1";

		data.m_componentsToUpdate.push_back (L"AVS");
		data.m_componentsToUpdate.push_back (L"CORE");
		data.connect_tmo = 30;

		data.m_sourceList.addSource (UST_UserURL, std::wstring(COLL_ROOT).append(L"\\17598").c_str(), true, L"diffs");
		//data.m_sourceList.addSource (UST_UserURL, L"ftp://d5x.kaspersky-labs.com", true, L"diffs");
		data.UpdaterListFlags.update_bases = true;

		Callbacks cbacks;
		TestCallbackInterface testCallback (&testLog, &cbacks);
		TestJournalInterface testJournal;
		testCallback.SetUpdaterConfiguration (data);
		testCallback.SetPrimaryIndex( FileInfo (L"u0607g.xml", L"index/6", FileInfo::index, true) );
		
		FileInfo u0607g ( L"u0607g.xml", L"index/6", FileInfo::index, true );
		//u0607g.m_localPath = UPD_ROOT;
		testCallback.SetPrimaryIndex(u0607g);
		
		Updater upd (testCallback.m_dp, testCallback, testJournal, &testLog);
		HANDLE hDoUpdate;

		cbacks.bFlags[Callbacks::getPrimaryIndex] = true;
		
		doUpdater do_upd(&upd, hDoUpdate);

		WaitForSingleObject (hDoUpdate, INFINITE);

		QC_BOOST_CHECK_MESSAGE ( NO_ERROR == do_upd.GetError(), L"Wrong return code\n" );
/*
		QC_BOOST_CHECK_MESSAGE (_helper::IsFoldersEqual(DIFFTOOL_ROOT,
			std::wstring(COLL_ROOT).append(L"\\tl-srv-wxp\\Bases_1\\plain").c_str(), BASES_ROOT), L"Incorrect bases\n");
		QC_BOOST_CHECK_MESSAGE (_helper::IsFoldersEqual(DIFFTOOL_ROOT,
			std::wstring(COLL_ROOT).append(L"\\tl-srv-wxp\\Bases_1\\data").c_str(), UPD_ROOT), L"Incorrect index\n");
*/
	}
};


/*void TestSuite::Test ()
{
	using namespace KLUPD;
	std::wcout<<L"Test\n";

	{
		TestLog testLog;

		_helper::DeleteDirectory(ROOT);
		_helper::DeleteDirectory(BASES_MOD_ROOT);

		KLUPD::createFolder (BASES_ROOT, &testLog);
		KLUPD::createFolder (UPD_ROOT, &testLog);

		_helper::CopyFolder(std::wstring(COLL_ROOT).append(L"\\Bases_NULL\\plain").c_str(),BASES_ROOT);
		_helper::CloneFile (std::wstring(COLL_ROOT).append(L"\\Bases_NULL\\index\\6\\u0607g.xml").c_str(),
			std::wstring (UPD_ROOT).append (L"\\u0607g.xml"), false);

		UpdaterConfiguration data(1, 1, L"1");

		data.temporary_dir = TEMP_ROOT;
		//data.reserve_dir = RESERVE_ROOT;

		data.m_pathSubstitutionMap.push_back(std::make_pair<NoCaseString,NoCaseString>(L"%BaseFolder%", BASES_ROOT));
		data.m_pathSubstitutionMap.push_back(std::make_pair<NoCaseString,NoCaseString>(L"%Folder%", ROLLBACK_ROOT));
		data.m_pathSubstitutionMap.push_back(std::make_pair<NoCaseString,NoCaseString>(L"%UpdateRoot%", UPD_ROOT));
		data.m_pathSubstitutionMap.push_back(std::make_pair<NoCaseString,NoCaseString>(L"%DSKM%", DSKM_ROOT));
		data.m_pathSubstitutionMap.push_back(std::make_pair<NoCaseString,NoCaseString>(L"%DataFolder%", UPD_ROOT));

		data.self_region = L"<default>";
		data.product_lang.push_back( NoCaseString(L"<default>"));
		
		data.os.os = L"Windows";
		data.os.version = L"5.1";

		data.m_componentsToUpdate.push_back (L"AVS");
		data.m_componentsToUpdate.push_back (L"CORE");

		data.connect_tmo = 30;

		data.m_sourceList.addSource (UST_UserURL, std::wstring(COLL_ROOT).append(L"\\Bases_1").c_str(), true, L"diffs");
		data.UpdaterListFlags.update_bases = true;

		Callbacks cbacks;
		TestCallbackInterface testCallback (&testLog, &cbacks);
		TestJournalInterface testJournal;
		testCallback.SetUpdaterConfiguration (data);
		
		Updater upd (testCallback.m_dp, testCallback, testJournal, &testLog);
		HANDLE hDoUpdate;

		doUpdater do_upd(&upd, hDoUpdate);

		WaitForSingleObject (hDoUpdate, INFINITE);

		_helper::DeleteFile( std::wstring(BASES_ROOT).append (L"\\base100.avc") );
		QC_BOOST_CHECK_MESSAGE (_helper::IsFoldersEqual(DIFFTOOL_ROOT,
			std::wstring(COLL_ROOT).append(L"\\Bases_1\\plain").c_str(), BASES_ROOT), L"Incorrect bases\n");
		QC_BOOST_CHECK_MESSAGE (_helper::IsFoldersEqual(DIFFTOOL_ROOT,
			std::wstring(COLL_ROOT).append(L"\\Bases_1\\data").c_str(), UPD_ROOT), L"Incorrect bases\n");

		QC_BOOST_CHECK_MESSAGE ( KLUPD::CORE_NO_ERROR == do_upd.GetError(), L"Wrong return code\n" );
	}
}*/

void TestSuite::TestCheckCancel ()
{
	using namespace KLUPD;
	std::wcout<<L"TestCheckCancel\n";
	Logger::Case _case (m_logger, m_testp, m_step, L"TestCheckCancel");

	{
		std::wcout<<L"Before checkFilesToDownload call\n";

		TestLog testLog;

		_helper::DeleteDirectory(ROOT);
		KLUPD::createFolder (BASES_ROOT, &testLog);
		KLUPD::createFolder (UPD_ROOT, &testLog);

		_helper::CopyFolder(std::wstring(COLL_ROOT).append(L"\\Bases_NULL\\plain").c_str(),BASES_ROOT);
		_helper::CloneFile (std::wstring(COLL_ROOT).append(L"\\Bases_NULL\\index\\6\\u0607g.xml").c_str(),
			std::wstring (UPD_ROOT).append (L"\\u0607g.xml"), false);

		UpdaterConfiguration data(1, 1, L"1");

		data.temporary_dir = TEMP_ROOT;
		//data.reserve_dir = RESERVE_ROOT;
		//data.update_root_folder = UPD_ROOT;

		data.m_pathSubstitutionMap.push_back(std::make_pair<NoCaseString,NoCaseString>(L"%BaseFolder%", BASES_ROOT));
		data.m_pathSubstitutionMap.push_back(std::make_pair<NoCaseString,NoCaseString>(L"%Folder%", ROLLBACK_ROOT));
		data.m_pathSubstitutionMap.push_back(std::make_pair<NoCaseString,NoCaseString>(L"%UpdateRoot%", UPD_ROOT));
		data.m_pathSubstitutionMap.push_back(std::make_pair<NoCaseString,NoCaseString>(L"%DSKM%", DSKM_ROOT));
		data.m_pathSubstitutionMap.push_back(std::make_pair<NoCaseString,NoCaseString>(L"%DataFolder%", UPD_ROOT));

		data.self_region = L"<default>";
		data.product_lang.push_back( NoCaseString(L"<default>"));
		
		data.os.os = L"Windows";
		data.os.version = L"5.1";

		data.m_componentsToUpdate.push_back ( L"AVS" );
		data.m_componentsToUpdate.push_back (NoCaseString(L"CORE") );
		data.connect_tmo = 30;

		data.m_sourceList.addSource (UST_UserURL, std::wstring(COLL_ROOT).append(L"\\Bases_1").c_str(), true, L"diffs");
		data.UpdaterListFlags.update_bases = true;

		Callbacks cbacks;
		TestCallbackInterface testCallback (&testLog, &cbacks);
		TestJournalInterface testJournal;
		testCallback.SetUpdaterConfiguration (data);
		
		Updater upd (testCallback.m_dp, testCallback, testJournal, &testLog);
		HANDLE hDoUpdate;

		cbacks.bFlags[Callbacks::checkFilesToDownload] = true;
		cbacks.bFlags[Callbacks::checkCancel] = true;
		testCallback.SetCancelUpdate (true);

		doUpdater do_upd(&upd, hDoUpdate);
		WaitForSingleObject (hDoUpdate, INFINITE);

		QC_BOOST_CHECK_MESSAGE ( CORE_CANCELLED == do_upd.GetError(), L"Wrong return code\n" );
		QC_BOOST_CHECK_MESSAGE (_helper::IsFoldersEqual(DIFFTOOL_ROOT,
			std::wstring(COLL_ROOT).append(L"\\Bases_NULL\\plain").c_str(), BASES_ROOT), L"Incorrect bases\n");

	}
	
	{
		std::wcout<<L"Before processReceivedFiles call\n";

		TestLog testLog;

		_helper::DeleteDirectory(ROOT);
		KLUPD::createFolder (BASES_ROOT, &testLog);
		KLUPD::createFolder (UPD_ROOT, &testLog);

		_helper::CopyFolder(std::wstring(COLL_ROOT).append(L"\\Bases_NULL\\plain").c_str(),BASES_ROOT);
		_helper::CloneFile (std::wstring(COLL_ROOT).append(L"\\Bases_NULL\\index\\6\\u0607g.xml").c_str(),
			std::wstring (UPD_ROOT).append (L"\\u0607g.xml"), false);

		UpdaterConfiguration data(1, 1, L"1");

		data.temporary_dir = TEMP_ROOT;
		//data.reserve_dir = RESERVE_ROOT;
		//data.update_root_folder = UPD_ROOT;

		data.m_pathSubstitutionMap.push_back(std::make_pair<NoCaseString,NoCaseString>(L"%BaseFolder%", BASES_ROOT));
		data.m_pathSubstitutionMap.push_back(std::make_pair<NoCaseString,NoCaseString>(L"%Folder%", ROLLBACK_ROOT));
		data.m_pathSubstitutionMap.push_back(std::make_pair<NoCaseString,NoCaseString>(L"%UpdateRoot%", UPD_ROOT));
		data.m_pathSubstitutionMap.push_back(std::make_pair<NoCaseString,NoCaseString>(L"%DSKM%", DSKM_ROOT));
		data.m_pathSubstitutionMap.push_back(std::make_pair<NoCaseString,NoCaseString>(L"%DataFolder%", UPD_ROOT));

		data.self_region = L"<default>";
		data.product_lang.push_back( NoCaseString(L"<default>"));
		
		data.os.os = L"Windows";
		data.os.version = L"5.1";

		data.m_componentsToUpdate.push_back ( L"AVS" );
		data.m_componentsToUpdate.push_back (NoCaseString(L"CORE") );
		data.connect_tmo = 30;

		data.m_sourceList.addSource (UST_UserURL, std::wstring(COLL_ROOT).append(L"\\Bases_1").c_str(), true, L"diffs");
		data.UpdaterListFlags.update_bases = true;

		Callbacks cbacks;
		TestCallbackInterface testCallback (&testLog, &cbacks);
		TestJournalInterface testJournal;
		testCallback.SetUpdaterConfiguration (data);
		
		Updater upd (testCallback.m_dp, testCallback, testJournal, &testLog);
		HANDLE hDoUpdate;

		cbacks.bFlags[Callbacks::checkFilesToDownload] = true;
		cbacks.bFlags[Callbacks::processReceivedFiles] = true;
		cbacks.bFlags[Callbacks::checkCancel] = true;
		CREATE_EVENTS(checkFilesToDownload)

		doUpdater do_upd(&upd, hDoUpdate);

		WAIT_FOR_READY_EVENT(checkFilesToDownload)
		SET_GOON_EVENT(checkFilesToDownload);
		testCallback.SetCancelUpdate (true);
			
		WaitForSingleObject (hDoUpdate, INFINITE);

		QC_BOOST_CHECK_MESSAGE ( CORE_CANCELLED == do_upd.GetError(), L"Wrong return code\n" );
		QC_BOOST_CHECK_MESSAGE (_helper::IsFoldersEqual(DIFFTOOL_ROOT,
			std::wstring(COLL_ROOT).append(L"\\Bases_NULL\\plain").c_str(), BASES_ROOT), L"Incorrect bases\n");

	}

	{
		std::wcout<<L"During authorization on proxy\n";
		TestLog testLog;

		_helper::DeleteDirectory(ROOT);
		KLUPD::createFolder (BASES_ROOT, &testLog);
		KLUPD::createFolder (UPD_ROOT, &testLog);

		_helper::CopyFolder(std::wstring(COLL_ROOT).append(L"\\Bases_NULL\\plain").c_str(),BASES_ROOT);
		_helper::CloneFile (std::wstring(COLL_ROOT).append(L"\\Bases_NULL\\index\\6\\u0607g.xml").c_str(),
			std::wstring (UPD_ROOT).append (L"\\u0607g.xml"), false);

		_helper::CloneFile(std::wstring(COLL_ROOT).append(L"\\Updcfgs\\updcfg_w2k.xml").c_str(),
			std::wstring (UPD_ROOT).append(L"\\updcfg.xml"), false);

		UpdaterConfiguration data(1, 1, L"1");

		data.temporary_dir = TEMP_ROOT;
		//data.reserve_dir = RESERVE_ROOT;
		
		data.m_pathSubstitutionMap.push_back(std::make_pair<NoCaseString,NoCaseString>(L"%BaseFolder%", BASES_ROOT));
		data.m_pathSubstitutionMap.push_back(std::make_pair<NoCaseString,NoCaseString>(L"%Folder%", ROLLBACK_ROOT));
		data.m_pathSubstitutionMap.push_back(std::make_pair<NoCaseString,NoCaseString>(L"%UpdateRoot%", UPD_ROOT));
		data.m_pathSubstitutionMap.push_back(std::make_pair<NoCaseString,NoCaseString>(L"%DSKM%", DSKM_ROOT));
		data.m_pathSubstitutionMap.push_back(std::make_pair<NoCaseString,NoCaseString>(L"%DataFolder%", UPD_ROOT));

		data.self_region = L"<default>";
		data.product_lang.push_back( NoCaseString(L"<default>"));
		
		data.os.os = L"Windows";
		data.os.version = L"5.1";

		data.m_componentsToUpdate.push_back (L"AVS");
		data.m_componentsToUpdate.push_back (L"CORE");
		data.connect_tmo = 30;

		data.m_sourceList.addSource (UST_KLServer, L"", true, L"diffs");
		data.proxy_url = L"tl-prx-lnx";
		data.proxy_port = 3128;
		//data.m_httpAuthorizationMethods.AddAuthMethod (AUT_Basic, true);
		data.m_httpAuthorizationMethods.push_back (basicAuthorization);
		data.UpdaterListFlags.update_bases = true;

		Callbacks cbacks;
		TestCallbackInterface testCallback (&testLog, &cbacks);
		TestJournalInterface testJournal;
		testCallback.SetUpdaterConfiguration (data);
		testCallback.SetPrimaryIndex( FileInfo (L"u0607g.xml", L"updater13/bases_1/index/6", FileInfo::index, true) );
		
		FileInfo u0607g ( L"u0607g.xml", L"updater13/bases_1/index/6", FileInfo::index, true );
		testCallback.SetPrimaryIndex(u0607g);
		FileInfo updcfg ( L"updcfg.xml", L"updater13/bases_1/index/6", FileInfo::base, false );
		updcfg.m_localPath = UPD_ROOT;
		testCallback.SetUpdcfgXml(updcfg);
		FileInfo sites ( L"sites.xml", L"updater13/bases_1/index/6", FileInfo::base, false );
		sites.m_localPath = PRODUCT_ROOT;
		testCallback.SetSitesXml(sites);

		Updater upd (testCallback.m_dp, testCallback, testJournal, &testLog);
		HANDLE hDoUpdate;

		cbacks.bFlags[Callbacks::getPrimaryIndex] = true;
		cbacks.bFlags[Callbacks::getUpdaterConfigurationXml] = true;
		cbacks.bFlags[Callbacks::getSitesFileXml] = true;
		cbacks.bFlags[Callbacks::requestCredentialsForAuthorizationOnProxy] = true;
		//cbacks.bFlags[Callbacks::processProxyAuthorizationSuccess] = true;
		cbacks.bFlags[Callbacks::checkCancel] = true;
		
		CREATE_EVENTS(requestCredentialsForAuthorizationOnProxy)
		//CREATE_EVENTS(processProxyAuthorizationSuccess)

		doUpdater do_upd(&upd, hDoUpdate);

		WAIT_FOR_READY_EVENT(requestCredentialsForAuthorizationOnProxy)
		TestCallbackInterface::_credentials creds;
		creds.name = L"test";
		creds.password = L"test";
		testCallback.SetRequestCreds(creds);
		SET_GOON_EVENT(requestCredentialsForAuthorizationOnProxy);

		//WAIT_FOR_READY_EVENT(processProxyAuthorizationSuccess)
		//QC_BOOST_CHECK_MESSAGE ( testCallback.GetConfirmCreds().name == creds.name &&
		//	testCallback.GetConfirmCreds().password == creds.password, L"Incorrect credentials\n" );
		//SET_GOON_EVENT(processProxyAuthorizationSuccess);
		testCallback.SetCancelUpdate (true);

		WaitForSingleObject (hDoUpdate, INFINITE);;

		_helper::DeleteFile( std::wstring (UPD_ROOT).append(L"\\updcfg.xml") );

		QC_BOOST_CHECK_MESSAGE ( CORE_CANCELLED == do_upd.GetError(), L"Wrong return code\n" );
		QC_BOOST_CHECK_MESSAGE (_helper::IsFoldersEqual(DIFFTOOL_ROOT,
			std::wstring(COLL_ROOT).append(L"\\Bases_NULL\\plain").c_str(), BASES_ROOT), L"Incorrect bases\n");

	 }
	 
	 {
		std::wcout<<L"Cancelling in case hanging on connect\n";

		TestLog testLog;

		_helper::DeleteDirectory(ROOT);
		KLUPD::createFolder (BASES_ROOT, &testLog);
		KLUPD::createFolder (UPD_ROOT, &testLog);

		_helper::CopyFolder(std::wstring(COLL_ROOT).append(L"\\Bases_NULL\\plain").c_str(),BASES_ROOT);
		_helper::CloneFile (std::wstring(COLL_ROOT).append(L"\\Bases_NULL\\index\\6\\u0607g.xml").c_str(),
			std::wstring (UPD_ROOT).append (L"\\u0607g.xml"), false);

		UpdaterConfiguration data(1, 1, L"1");

		data.temporary_dir = TEMP_ROOT;
		//data.reserve_dir = RESERVE_ROOT;
		//data.update_root_folder = UPD_ROOT;

		data.m_pathSubstitutionMap.push_back(std::make_pair<NoCaseString,NoCaseString>(L"%BaseFolder%", BASES_ROOT));
		data.m_pathSubstitutionMap.push_back(std::make_pair<NoCaseString,NoCaseString>(L"%Folder%", ROLLBACK_ROOT));
		data.m_pathSubstitutionMap.push_back(std::make_pair<NoCaseString,NoCaseString>(L"%UpdateRoot%", UPD_ROOT));
		data.m_pathSubstitutionMap.push_back(std::make_pair<NoCaseString,NoCaseString>(L"%DSKM%", DSKM_ROOT));
		data.m_pathSubstitutionMap.push_back(std::make_pair<NoCaseString,NoCaseString>(L"%DataFolder%", UPD_ROOT));

		data.self_region = L"<default>";
		data.product_lang.push_back( NoCaseString(L"<default>"));
		
		data.os.os = L"Windows";
		data.os.version = L"5.1";

		data.m_componentsToUpdate.push_back ( L"AVS" );
		data.m_componentsToUpdate.push_back (NoCaseString(L"CORE") );
		data.connect_tmo = 600;

		data.m_sourceList.addSource (UST_UserURL, L"http://tl-srv-w2k/updater13/bases_1", true, L"diffs");
		data.proxy_url = L"tl-prx-nta";
		data.proxy_port = 3127; //wrong port
		data.m_proxyAuthorizationCredentials.userName (L"t");
		data.m_proxyAuthorizationCredentials.password (L"t");
		//data.m_httpAuthorizationMethods.AddAuthMethod (AUT_Ntlm, true);
		data.m_httpAuthorizationMethods.push_back (ntlmAuthorization);
		data.UpdaterListFlags.update_bases = true;

		Callbacks cbacks;
		TestCallbackInterface testCallback (&testLog, &cbacks);
		TestJournalInterface testJournal;
		testCallback.SetUpdaterConfiguration (data);
		
		Updater upd (testCallback.m_dp, testCallback, testJournal, &testLog);
		HANDLE hDoUpdate;

		cbacks.bFlags[Callbacks::checkCancel] = true;

		doUpdater do_upd(&upd, hDoUpdate);
		
		srand ((unsigned)time( NULL ));
		Sleep ( (rand()/2) + 3000 );
		testCallback.SetCancelUpdate (true);
			
		//update process must be terminated instantly
		DWORD result = WaitForSingleObject (hDoUpdate, 5000);
		if (result == WAIT_OBJECT_0)
		{
			QC_BOOST_CHECK_MESSAGE ( CORE_CANCELLED == do_upd.GetError(), L"Wrong return code\n" );
			QC_BOOST_CHECK_MESSAGE (_helper::IsFoldersEqual(DIFFTOOL_ROOT,
				std::wstring(COLL_ROOT).append(L"\\Bases_NULL\\plain").c_str(), BASES_ROOT), L"Incorrect bases\n");
		}
		else QC_BOOST_CHECK_MESSAGE (false, L"Update process hasn't been terminated\n");
	}
	
	{
		std::wcout<<L"Cancelling in case hanging on recv\n";

		TestLog testLog;

		_helper::DeleteDirectory(ROOT);
		KLUPD::createFolder (BASES_ROOT, &testLog);
		KLUPD::createFolder (UPD_ROOT, &testLog);

		_helper::CopyFolder(std::wstring(COLL_ROOT).append(L"\\Bases_NULL\\plain").c_str(),BASES_ROOT);
		_helper::CloneFile (std::wstring(COLL_ROOT).append(L"\\Bases_NULL\\index\\6\\u0607g.xml").c_str(),
			std::wstring (UPD_ROOT).append (L"\\u0607g.xml"), false);

		UpdaterConfiguration data(1, 1, L"1");

		data.temporary_dir = TEMP_ROOT;
		//data.reserve_dir = RESERVE_ROOT;
		//data.update_root_folder = UPD_ROOT;

		data.m_pathSubstitutionMap.push_back(std::make_pair<NoCaseString,NoCaseString>(L"%BaseFolder%", BASES_ROOT));
		data.m_pathSubstitutionMap.push_back(std::make_pair<NoCaseString,NoCaseString>(L"%Folder%", ROLLBACK_ROOT));
		data.m_pathSubstitutionMap.push_back(std::make_pair<NoCaseString,NoCaseString>(L"%UpdateRoot%", UPD_ROOT));
		data.m_pathSubstitutionMap.push_back(std::make_pair<NoCaseString,NoCaseString>(L"%DSKM%", DSKM_ROOT));
		data.m_pathSubstitutionMap.push_back(std::make_pair<NoCaseString,NoCaseString>(L"%DataFolder%", UPD_ROOT));

		data.self_region = L"<default>";
		data.product_lang.push_back( NoCaseString(L"<default>"));
		
		data.os.os = L"Windows";
		data.os.version = L"5.1";

		data.m_componentsToUpdate.push_back ( L"AVS" );
		data.m_componentsToUpdate.push_back (NoCaseString(L"CORE") );
		data.connect_tmo = 600;

		data.m_sourceList.addSource (UST_UserURL, L"http://tl-srv-w2k/updater13/bases_1", true, L"diffs");
		data.proxy_url = L"tl-srv-w2k"; //not a proxy
		data.proxy_port = 21;
		data.m_proxyAuthorizationCredentials.userName (L"t");
		data.m_proxyAuthorizationCredentials.password (L"t");
		//data.m_httpAuthorizationMethods.AddAuthMethod (AUT_Ntlm, true);
		data.m_httpAuthorizationMethods.push_back (ntlmAuthorization);
		data.UpdaterListFlags.update_bases = true;

		Callbacks cbacks;
		TestCallbackInterface testCallback (&testLog, &cbacks);
		TestJournalInterface testJournal;
		testCallback.SetUpdaterConfiguration (data);
		
		Updater upd (testCallback.m_dp, testCallback, testJournal, &testLog);
		HANDLE hDoUpdate;

		cbacks.bFlags[Callbacks::checkCancel] = true;

		doUpdater do_upd(&upd, hDoUpdate);
		
		srand ((unsigned)time( NULL ));
		Sleep ( rand() + 3000 );
		testCallback.SetCancelUpdate (true);
			
		//update process must be terminated instantly
		DWORD result = WaitForSingleObject (hDoUpdate, 5000);
		if (result == WAIT_OBJECT_0)
		{
			QC_BOOST_CHECK_MESSAGE ( CORE_CANCELLED == do_upd.GetError(), L"Wrong return code\n" );
			QC_BOOST_CHECK_MESSAGE (_helper::IsFoldersEqual(DIFFTOOL_ROOT,
				std::wstring(COLL_ROOT).append(L"\\Bases_NULL\\plain").c_str(), BASES_ROOT), L"Incorrect bases\n");
		}
		else QC_BOOST_CHECK_MESSAGE (false, L"Update process hasn't been terminated\n");
	}
}

void TestSuite::TestCheckCancelWhileDownloadingGroup ()
{
//	TestCheckCancelWhileDownloading (L"TestCheckCancelWhileDownloadingLocal", std::wstring(COLL_ROOT).append(L"\\BaseDownloading\\Bases_1").c_str() );
	TestCheckCancelWhileDownloading (L"TestCheckCancelWhileDownloadingFtp", L"ftp://tl-srv-wxp/BaseDownloading/Bases_1" );
//	TestCheckCancelWhileDownloading (L"TestCheckCancelWhileDownloadingHttp", L"http://tl-srv-wxp/BaseDownloading/Bases_1" );
//	TestCheckCancelWhileDownloading (L"TestCheckCancelWhileDownloadingSmb", L"\\\\tl-srv-wxp\\BaseDownloading\\Bases_1" );
};

void TestSuite::TestCheckCancelWhileDownloading (const wchar_t* test_name, const wchar_t* source)
{
	using namespace KLUPD;
	std::wcout<<test_name<<L"\n";
	Logger::Case _case (m_logger, m_testp, m_step, test_name);
	
	bool bStopTesting = false;
	int i = 0;

	std::wcout<<L"Testing...\n";

	while (!bStopTesting)
	{
		std::wcout<<L"Test case "<<i+1<<L"\n";

		TestLog testLog;

		_helper::DeleteDirectory(ROOT);
		KLUPD::createFolder (BASES_ROOT, &testLog);
		KLUPD::createFolder (UPD_ROOT, &testLog);

		_helper::CopyFolder(std::wstring(COLL_ROOT).append(L"\\BaseDownloading\\Bases_NULL\\plain").c_str(),BASES_ROOT);
		_helper::CloneFile (std::wstring(COLL_ROOT).append(L"\\BaseDownloading\\Bases_NULL\\index\\6\\u0607g.xml").c_str(),
			std::wstring (UPD_ROOT).append (L"\\u0607g.xml"), false);

		UpdaterConfiguration data(1, 1, L"1");

		data.temporary_dir = TEMP_ROOT;
		//data.reserve_dir = RESERVE_ROOT;
		//data.update_root_folder = UPD_ROOT;

		data.m_pathSubstitutionMap.push_back(std::make_pair<NoCaseString,NoCaseString>(L"%BaseFolder%", BASES_ROOT));
		data.m_pathSubstitutionMap.push_back(std::make_pair<NoCaseString,NoCaseString>(L"%Folder%", ROLLBACK_ROOT));
		data.m_pathSubstitutionMap.push_back(std::make_pair<NoCaseString,NoCaseString>(L"%UpdateRoot%", UPD_ROOT));
		data.m_pathSubstitutionMap.push_back(std::make_pair<NoCaseString,NoCaseString>(L"%DSKM%", DSKM_ROOT));
		data.m_pathSubstitutionMap.push_back(std::make_pair<NoCaseString,NoCaseString>(L"%DataFolder%", UPD_ROOT));

		data.self_region = L"<default>";
		data.product_lang.push_back( NoCaseString(L"<default>"));
		
		data.os.os = L"Windows";
		data.os.version = L"5.1";

		data.m_componentsToUpdate.push_back ( L"AVS" );
		data.m_componentsToUpdate.push_back (NoCaseString(L"CORE") );
		data.m_componentsToUpdate.push_back ( L"Updater" );
		data.connect_tmo = 30;

		data.m_sourceList.addSource (UST_UserURL, source, false, L"diffs");
		data.UpdaterListFlags.update_bases = true;

		Callbacks cbacks;
		TestCallbackInterface testCallback (&testLog, &cbacks);
		TestJournalInterface testJournal;
		testCallback.SetUpdaterConfiguration (data);
		
		Updater upd (testCallback.m_dp, testCallback, testJournal, &testLog);
		HANDLE hDoUpdate;

		//cbacks.bFlags[Callbacks::checkFilesToDownload] = true;
		cbacks.bFlags[Callbacks::processReceivedFiles] = true;
		cbacks.bFlags[Callbacks::checkCancel] = true;
		cbacks.bFlags[Callbacks::getPrimaryIndex] = true;
		cbacks.bFlags[Callbacks::percentCallback] = true;
		
		testCallback.SetPrimaryIndex( FileInfo (L"u0607g.xml", L"index/6", FileInfo::index, true) );
		
		//CREATE_EVENTS(checkFilesToDownload)
		CREATE_EVENTS(processReceivedFiles)	
		CREATE_EVENTS(percentCallback)

		doUpdater do_upd(&upd, hDoUpdate);

		//WAIT_FOR_READY_EVENT(checkFilesToDownload)
		//SET_GOON_EVENT(checkFilesToDownload);

		//cbacks.bFlags[Callbacks::percentCallback] = true;
		//CREATE_EVENTS(percentCallback)

		HANDLE hnls[3];
		hnls[0] = cbacks.hReadyEvents[Callbacks::percentCallback];
		hnls[1] = cbacks.hReadyEvents[Callbacks::processReceivedFiles];
		hnls[2] = hDoUpdate;
		DWORD waitResult;
		int j = 0;
		//calculating percent callbacks while downloading files
		while ( ( waitResult = WaitForMultipleObjects (3, hnls, false, INFINITE) )== WAIT_OBJECT_0 )
		{
			if (i*2 == j) testCallback.SetCancelUpdate (true);
			j++;
			SET_GOON_EVENT(percentCallback);
		}
		if (waitResult == (WAIT_OBJECT_0 + 1) ) 
		{
			
			cbacks.bFlags[Callbacks::percentCallback] = false;
			cbacks.bFlags[Callbacks::processReceivedFiles] = false;
			SET_GOON_EVENT(processReceivedFiles);

			WaitForSingleObject (hDoUpdate, INFINITE);
			bStopTesting = true;
			continue;
		};

		if (CORE_CANCELLED == do_upd.GetError())
		{
			std::wcout<<testCallback.m_dp.bytesTransferred()<<L"\n";
			QC_BOOST_CHECK_MESSAGE ( testCallback.GetBytesDownloadedBeforeCancelling()
							== testCallback.m_dp.bytesTransferred(), L"Wrong amount of bytes downloaded\n" );
			QC_BOOST_CHECK_MESSAGE (_helper::IsFoldersEqual(DIFFTOOL_ROOT,
				std::wstring(COLL_ROOT).append(L"\\BaseDownloading\\Bases_NULL\\plain").c_str(), BASES_ROOT), L"Incorrect bases\n");
			QC_BOOST_CHECK_MESSAGE (_helper::IsFoldersEqual(DIFFTOOL_ROOT,
				std::wstring(COLL_ROOT).append(L"\\BaseDownloading\\Bases_NULL\\data").c_str(), UPD_ROOT), L"Incorrect index\n");
		}
		else
		{
			QC_BOOST_CHECK_MESSAGE (false, L"Fault return code\n");
		}
		i++;
	}

}

void TestSuite::TestUsingWPADProxy ()
{
	using namespace KLUPD;
	std::wcout<<L"TestUsingWPADProxy\n";
	Logger::Case _case (m_logger, m_testp, m_step, L"TestUsingWPADProxy");

	std::wcout<<L"После исправления ошибки 19187 переделать с использованием внутреннего сервера"<<L"\n";
	QC_BOOST_REQUIRE_MESSAGE (false, L"19187, using corrected data (test_wpad.dat)\n");
	{
		TestLog testLog;

		_helper::DeleteDirectory(ROOT);
		KLUPD::createFolder (BASES_ROOT, &testLog);
		KLUPD::createFolder (UPD_ROOT, &testLog);

		_helper::CopyFolder(std::wstring(COLL_ROOT).append(L"\\Bases_NULL\\plain").c_str(),BASES_ROOT);
		_helper::CloneFile (std::wstring(COLL_ROOT).append(L"\\Bases_NULL\\index\\6\\u0607g.xml").c_str(),
			std::wstring (UPD_ROOT).append (L"\\u0607g.xml"), false);

		UpdaterConfiguration data(1, 1, L"1");

		data.temporary_dir = TEMP_ROOT;
		//data.reserve_dir = RESERVE_ROOT;
		//data.update_root_folder = UPD_ROOT;

		data.m_pathSubstitutionMap.push_back(std::make_pair<NoCaseString,NoCaseString>(L"%BaseFolder%", BASES_ROOT));
		data.m_pathSubstitutionMap.push_back(std::make_pair<NoCaseString,NoCaseString>(L"%Folder%", ROLLBACK_ROOT));
		data.m_pathSubstitutionMap.push_back(std::make_pair<NoCaseString,NoCaseString>(L"%UpdateRoot%", UPD_ROOT));
		data.m_pathSubstitutionMap.push_back(std::make_pair<NoCaseString,NoCaseString>(L"%DSKM%", DSKM_ROOT));
		data.m_pathSubstitutionMap.push_back(std::make_pair<NoCaseString,NoCaseString>(L"%DataFolder%", UPD_ROOT));

		data.self_region = L"<default>";
		data.product_lang.push_back( NoCaseString(L"<default>"));
		
		data.os.os = L"Windows";
		data.os.version = L"5.1";

		data.m_componentsToUpdate.push_back ( L"AVS" );
		data.m_componentsToUpdate.push_back ( L"CORE" );
		data.connect_tmo = 10;

		data.m_sourceList.addSource (UST_UserURL, L"http://tl-srv-wxpa/updater13/bases_1/", true, L"diffs");
		data.use_ie_proxy = true;
		//data.m_httpAuthorizationMethods.AddAuthMethod (AUT_None, true);
		data.m_httpAuthorizationMethods.push_back (noAuthorization);
		data.UpdaterListFlags.update_bases = true;

		Callbacks cbacks;
		TestCallbackInterface testCallback (&testLog, &cbacks);
		TestSuiteProxyDetector::ProxyJournal journal ( std::wstring(COLL_ROOT).append(L"\\wpads\\test_wpad.dat") );
		testCallback.SetUpdaterConfiguration (data);
		
		Updater upd (testCallback.m_dp, testCallback, journal, &testLog);

		HANDLE hDoUpdate;

		doUpdater do_upd(&upd, hDoUpdate);

		WaitForSingleObject (hDoUpdate, INFINITE);

		QC_BOOST_CHECK_MESSAGE (_helper::DeleteFile(std::wstring(BASES_ROOT).append (L"\\base100.avc")),
								L"old base file not found");
		QC_BOOST_CHECK_MESSAGE (_helper::IsFoldersEqual(DIFFTOOL_ROOT,
			std::wstring(COLL_ROOT).append(L"\\tl-srv-wxp\\Bases_1\\plain").c_str(), BASES_ROOT), L"Incorrect bases\n");
		QC_BOOST_CHECK_MESSAGE (_helper::IsFoldersEqual(DIFFTOOL_ROOT,
			std::wstring(COLL_ROOT).append(L"\\tl-srv-wxp\\Bases_1\\data").c_str(), UPD_ROOT), L"Incorrect index\n");

		QC_BOOST_CHECK_MESSAGE ( NO_ERROR == do_upd.GetError(), L"Wrong return code\n" );
		
	}
}

void TestSuite::TestCheckDownloadedFiles ()
{
	using namespace KLUPD;
	std::wcout<<L"TestCheckDownloadedFiles\n";
	Logger::Case _case (m_logger, m_testp, m_step, L"TestCheckDownloadedFiles");

	{//normal check

		TestLog testLog;

		_helper::DeleteDirectory(ROOT);
		KLUPD::createFolder (BASES_ROOT, &testLog);
		KLUPD::createFolder (UPD_ROOT, &testLog);

		_helper::CopyFolder(std::wstring(COLL_ROOT).append(L"\\Bases_NULL\\plain").c_str(),BASES_ROOT);
		_helper::CloneFile (std::wstring(COLL_ROOT).append(L"\\Bases_NULL\\index\\6\\u0607g.xml").c_str(),
			std::wstring (UPD_ROOT).append (L"\\u0607g.xml"), false);

		UpdaterConfiguration data(1, 1, L"1");

		data.temporary_dir = TEMP_ROOT;
		//data.reserve_dir = RESERVE_ROOT;
		//data.update_root_folder = UPD_ROOT;

		data.m_pathSubstitutionMap.push_back(std::make_pair<NoCaseString,NoCaseString>(L"%BaseFolder%", BASES_ROOT));
		data.m_pathSubstitutionMap.push_back(std::make_pair<NoCaseString,NoCaseString>(L"%Folder%", ROLLBACK_ROOT));
		data.m_pathSubstitutionMap.push_back(std::make_pair<NoCaseString,NoCaseString>(L"%UpdateRoot%", UPD_ROOT));
		data.m_pathSubstitutionMap.push_back(std::make_pair<NoCaseString,NoCaseString>(L"%DSKM%", DSKM_ROOT));
		data.m_pathSubstitutionMap.push_back(std::make_pair<NoCaseString,NoCaseString>(L"%DataFolder%", UPD_ROOT));

		data.self_region = L"<default>";
		data.product_lang.push_back( NoCaseString(L"<default>"));

		data.os.os = L"Windows";
		data.os.version = L"5.1";

		data.m_componentsToUpdate.push_back ( L"AVS" );
		data.m_componentsToUpdate.push_back (NoCaseString(L"CORE") );
		data.connect_tmo = 30;

		data.m_sourceList.addSource (UST_UserURL, std::wstring(COLL_ROOT).append(L"\\Bases_1").c_str(), true, L"diffs");
		//data.m_sourceList.addSource (UST_UserURL, L"ftp://d5x.kaspersky-labs.com", true, L"diffs");
		//data.proxy_url = L"tl-prx-lnx";
		//data.proxy_port = 3128;
		//data.use_ie_proxy = true;
		//data.m_httpAuthorizationMethods.AddAuthMethod (AUT_Basic, true);
		data.UpdaterListFlags.update_bases = true;

		Callbacks cbacks;
		TestCallbackInterface testCallback (&testLog, &cbacks);
		TestJournalInterface testJournal;
		testCallback.SetUpdaterConfiguration (data);

		Updater upd (testCallback.m_dp, testCallback, testJournal, &testLog);
		HANDLE hDoUpdate;

		cbacks.bFlags[Callbacks::checkDownloadedFiles] = true;
		CREATE_EVENTS(checkDownloadedFiles)

		doUpdater do_upd(&upd, hDoUpdate);

		WAIT_FOR_READY_EVENT(checkDownloadedFiles)
		TestCallbackInterface::_checkDownloadedFiles& cdf = testCallback.GetCheckDownloadedFiles();
		std::wcout<<cdf.fv.size()<<L"\n";
		for (std::vector<FileInfo>::iterator it = cdf.fv.begin();
			it!=cdf.fv.end(); ++it)
		{
			std::wcout<<it->m_filename.m_value.toWideChar()<<L"\n";
		}

		QC_BOOST_REQUIRE_MESSAGE (cdf.retranslationMode == false, L"Incorrect retranslation mode\n");
		QC_BOOST_REQUIRE_MESSAGE (cdf.fv.size() == 11, L"Incorrect file list size\n");

		QC_BOOST_CHECK_MESSAGE (cdf.fv.at(6).m_filename == L"base101.avc", L"Incorrect file name\n");
		QC_BOOST_CHECK_MESSAGE (cdf.fv.at(7).m_filename == L"base102.avc", L"Incorrect file name\n");
		QC_BOOST_CHECK_MESSAGE (cdf.fv.at(8).m_filename == L"base103.avc", L"Incorrect file name\n");
		//QC_BOOST_CHECK_MESSAGE (cdf.fv.at(11).m_filename == L"base100.avc", L"Incorrect file name\n");

		QC_BOOST_CHECK_MESSAGE (cdf.fv.at(6).m_transactionInformation.m_changeStatus == FileInfo::unchanged, L"Incorrect transaction status\n");
		QC_BOOST_CHECK_MESSAGE (cdf.fv.at(7).m_transactionInformation.m_changeStatus == FileInfo::modified, L"Incorrect transaction status\n");
		QC_BOOST_CHECK_MESSAGE (cdf.fv.at(8).m_transactionInformation.m_changeStatus == FileInfo::added, L"Incorrect transaction status\n");
		//QC_BOOST_CHECK_MESSAGE (cdf.fv.at(11).m_transactionInformation.m_changeStatus == FileInfo::deleted, L"Incorrect transaction status\n");

		SET_GOON_EVENT(checkDownloadedFiles);

		WaitForSingleObject (hDoUpdate, INFINITE);

		QC_BOOST_CHECK_MESSAGE (_helper::DeleteFile(std::wstring(BASES_ROOT).append (L"\\base100.avc")),
			L"old base file not found");
		QC_BOOST_CHECK_MESSAGE (_helper::IsFoldersEqual(DIFFTOOL_ROOT,
			std::wstring(COLL_ROOT).append(L"\\Bases_1\\plain").c_str(), BASES_ROOT), L"Incorrect bases\n");

		QC_BOOST_CHECK_MESSAGE (_helper::IsFoldersEqual(DIFFTOOL_ROOT,
			std::wstring(COLL_ROOT).append(L"\\Bases_1\\data").c_str(), UPD_ROOT), L"Incorrect index\n");


		QC_BOOST_CHECK_MESSAGE ( NO_ERROR == do_upd.GetError(), L"Wrong return code\n" );
	}
	

	{//return false

		TestLog testLog;

		_helper::DeleteDirectory(ROOT);
		KLUPD::createFolder (BASES_ROOT, &testLog);
		KLUPD::createFolder (UPD_ROOT, &testLog);

		_helper::CopyFolder(std::wstring(COLL_ROOT).append(L"\\Bases_NULL\\plain").c_str(),BASES_ROOT);
		_helper::CloneFile (std::wstring(COLL_ROOT).append(L"\\Bases_NULL\\index\\6\\u0607g.xml").c_str(),
			std::wstring (UPD_ROOT).append (L"\\u0607g.xml"), false);

		UpdaterConfiguration data(1, 1, L"1");

		data.temporary_dir = TEMP_ROOT;
		//data.reserve_dir = RESERVE_ROOT;
		//data.update_root_folder = UPD_ROOT;

		data.m_pathSubstitutionMap.push_back(std::make_pair<NoCaseString,NoCaseString>(L"%BaseFolder%", BASES_ROOT));
		data.m_pathSubstitutionMap.push_back(std::make_pair<NoCaseString,NoCaseString>(L"%Folder%", ROLLBACK_ROOT));
		data.m_pathSubstitutionMap.push_back(std::make_pair<NoCaseString,NoCaseString>(L"%UpdateRoot%", UPD_ROOT));
		data.m_pathSubstitutionMap.push_back(std::make_pair<NoCaseString,NoCaseString>(L"%DSKM%", DSKM_ROOT));
		data.m_pathSubstitutionMap.push_back(std::make_pair<NoCaseString,NoCaseString>(L"%DataFolder%", UPD_ROOT));

		data.self_region = L"<default>";
		data.product_lang.push_back( NoCaseString(L"<default>"));

		data.os.os = L"Windows";
		data.os.version = L"5.1";

		data.m_componentsToUpdate.push_back ( L"AVS" );
		data.m_componentsToUpdate.push_back (NoCaseString(L"CORE") );
		data.connect_tmo = 30;

		data.m_sourceList.addSource (UST_UserURL, std::wstring(COLL_ROOT).append(L"\\Bases_1").c_str(), true, L"diffs");
		data.UpdaterListFlags.update_bases = true;

		Callbacks cbacks;
		TestCallbackInterface testCallback (&testLog, &cbacks);
		TestJournalInterface testJournal;
		testCallback.SetUpdaterConfiguration (data);

		Updater upd (testCallback.m_dp, testCallback, testJournal, &testLog);
		HANDLE hDoUpdate;

		cbacks.bFlags[Callbacks::checkDownloadedFiles] = true;
		CREATE_EVENTS(checkDownloadedFiles)

		doUpdater do_upd(&upd, hDoUpdate);

		WAIT_FOR_READY_EVENT(checkDownloadedFiles)
		TestCallbackInterface::_checkDownloadedFiles& cdf = testCallback.GetCheckDownloadedFiles();
		cdf.result = false;

		SET_GOON_EVENT(checkDownloadedFiles);

		WaitForSingleObject (hDoUpdate, INFINITE);

		QC_BOOST_CHECK_MESSAGE (_helper::IsFoldersEqual(DIFFTOOL_ROOT,
			std::wstring(COLL_ROOT).append(L"\\Bases_NULL\\plain").c_str(), BASES_ROOT), L"Incorrect bases\n");

		QC_BOOST_CHECK_MESSAGE (_helper::IsFoldersEqual(DIFFTOOL_ROOT,
			std::wstring(COLL_ROOT).append(L"\\Bases_NULL\\data").c_str(), UPD_ROOT), L"Incorrect index\n");

		QC_BOOST_CHECK_MESSAGE ( CORE_BASE_CHECK_FAILED == do_upd.GetError(), L"Wrong return code\n" );
	}
}

void TestSuite::TestCheckPreInstall ()
{
	using namespace KLUPD;
	std::wcout<<L"TestCheckPreInstall\n";
	Logger::Case _case (m_logger, m_testp, m_step, L"TestCheckPreInstall");

	{
		std::wcout<<L"Control sequence of calls\n";

		TestLog testLog;

		_helper::DeleteDirectory(ROOT);
		KLUPD::createFolder (BASES_ROOT, &testLog);
		KLUPD::createFolder (UPD_ROOT, &testLog);

		_helper::CopyFolder(std::wstring(COLL_ROOT).append(L"\\Bases_NULL\\plain").c_str(),BASES_ROOT);
		_helper::CloneFile (std::wstring(COLL_ROOT).append(L"\\Bases_NULL\\index\\6\\u0607g.xml").c_str(),
			std::wstring (UPD_ROOT).append (L"\\u0607g.xml"), false);

		UpdaterConfiguration data(1, 1, L"1");

		data.temporary_dir = TEMP_ROOT;
		//data.reserve_dir = RESERVE_ROOT;
		//data.update_root_folder = UPD_ROOT;

		data.m_pathSubstitutionMap.push_back(std::make_pair<NoCaseString,NoCaseString>(L"%BaseFolder%", BASES_ROOT));
		data.m_pathSubstitutionMap.push_back(std::make_pair<NoCaseString,NoCaseString>(L"%Folder%", ROLLBACK_ROOT));
		data.m_pathSubstitutionMap.push_back(std::make_pair<NoCaseString,NoCaseString>(L"%UpdateRoot%", UPD_ROOT));
		data.m_pathSubstitutionMap.push_back(std::make_pair<NoCaseString,NoCaseString>(L"%DSKM%", DSKM_ROOT));
		data.m_pathSubstitutionMap.push_back(std::make_pair<NoCaseString,NoCaseString>(L"%DataFolder%", UPD_ROOT));

		data.self_region = L"<default>";
		data.product_lang.push_back( NoCaseString(L"<default>"));

		data.os.os = L"Windows";
		data.os.version = L"5.1";

		data.m_componentsToUpdate.push_back ( L"AVS" );
		data.m_componentsToUpdate.push_back (NoCaseString(L"CORE") );
		data.connect_tmo = 30;

		data.m_sourceList.addSource (UST_UserURL, std::wstring(COLL_ROOT).append(L"\\Bases_1").c_str(), true, L"diffs");
		data.UpdaterListFlags.update_bases = true;

		Callbacks cbacks;
		TestCallbackInterface testCallback (&testLog, &cbacks);
		TestJournalInterface testJournal;
		testCallback.SetUpdaterConfiguration (data);

		Updater upd (testCallback.m_dp, testCallback, testJournal, &testLog);
		HANDLE hDoUpdate;

		cbacks.bFlags[Callbacks::checkPreInstalledComponent] = true;
		CREATE_EVENTS(checkPreInstalledComponent)

		doUpdater do_upd(&upd, hDoUpdate);

		TestCallbackInterface::_checkPreInstalledComponent& cpic = testCallback.GetCheckPreinstalledComponent();
		for (int i = 0; i < 3; ++i)
		{
			WAIT_FOR_READY_EVENT(checkPreInstalledComponent)
			
			std::wcout<<L"Component "<<cpic.compId.toWideChar()<<L"\n";
			std::wcout<<cpic.fv.size()<<L"\n";
			for (std::vector<FileInfo>::iterator it = cpic.fv.begin(); it!=cpic.fv.end(); ++it)
									std::wcout<<it->m_filename.m_value.toWideChar()<<L"\n";

			if (cpic.compId == L"")
			{
				QC_BOOST_REQUIRE_MESSAGE (cpic.fv.size() == 1, L"Incorrect file list size\n");
				QC_BOOST_CHECK_MESSAGE (cpic.fv.at(0).m_filename == L"u0607g.xml", L"Incorrect file name\n");
			}
			if (cpic.compId == L"AVS")
			{
				QC_BOOST_REQUIRE_MESSAGE (cpic.fv.size() == 4, L"Incorrect file list size\n");
				QC_BOOST_CHECK_MESSAGE (cpic.fv.at(0).m_filename == L"av-i386-0607g.xml", L"Incorrect file name\n");
				QC_BOOST_CHECK_MESSAGE (cpic.fv.at(1).m_filename == L"base101.avc", L"Incorrect file name\n");
				QC_BOOST_CHECK_MESSAGE (cpic.fv.at(2).m_filename == L"base102.avc", L"Incorrect file name\n");
				QC_BOOST_CHECK_MESSAGE (cpic.fv.at(3).m_filename == L"base103.avc", L"Incorrect file name\n");
			}
			if (cpic.compId == L"CORE")
			{
				QC_BOOST_REQUIRE_MESSAGE (cpic.fv.size() == 6, L"Incorrect file list size\n");
				QC_BOOST_CHECK_MESSAGE (cpic.fv.at(0).m_filename == L"black.lst", L"Incorrect file name\n");
				QC_BOOST_CHECK_MESSAGE (cpic.fv.at(1).m_filename == L"blst-0607g.xml", L"Incorrect file name\n");
				QC_BOOST_CHECK_MESSAGE (cpic.fv.at(2).m_filename == L"kl-0607g.pbv", L"Incorrect file name\n");
				QC_BOOST_CHECK_MESSAGE (cpic.fv.at(3).m_filename == L"updater-0607g.pbv", L"Incorrect file name\n");
				QC_BOOST_CHECK_MESSAGE (cpic.fv.at(4).m_filename == L"av-i386-0607g.krg", L"Incorrect file name\n");
				QC_BOOST_CHECK_MESSAGE (cpic.fv.at(5).m_filename == L"blst-rt-0607g.krg", L"Incorrect file name\n");
			}
			SET_GOON_EVENT(checkPreInstalledComponent);
		}
		
		WaitForSingleObject (hDoUpdate, INFINITE);

		QC_BOOST_CHECK_MESSAGE (_helper::DeleteFile(std::wstring(BASES_ROOT).append (L"\\base100.avc")),
			L"old base file not found");
		QC_BOOST_CHECK_MESSAGE (_helper::IsFoldersEqual(DIFFTOOL_ROOT,
			std::wstring(COLL_ROOT).append(L"\\Bases_1\\plain").c_str(), BASES_ROOT), L"Incorrect bases\n");

		QC_BOOST_CHECK_MESSAGE (_helper::IsFoldersEqual(DIFFTOOL_ROOT,
			std::wstring(COLL_ROOT).append(L"\\Bases_1\\data").c_str(), UPD_ROOT), L"Incorrect index\n");

		QC_BOOST_CHECK_MESSAGE ( NO_ERROR == do_upd.GetError(), L"Wrong return code\n" );
	}

	{
		std::wcout<<L"Error in AVS component (optional)\n";

		TestLog testLog;

		_helper::DeleteDirectory(ROOT);
		KLUPD::createFolder (BASES_ROOT, &testLog);
		KLUPD::createFolder (UPD_ROOT, &testLog);

		_helper::CopyFolder(std::wstring(COLL_ROOT).append(L"\\Bases_NULL\\plain").c_str(),BASES_ROOT);
		_helper::CloneFile (std::wstring(COLL_ROOT).append(L"\\Bases_NULL\\index\\6\\u0607g.xml").c_str(),
			std::wstring (UPD_ROOT).append (L"\\u0607g.xml"), false);

		UpdaterConfiguration data(1, 1, L"1");

		data.temporary_dir = TEMP_ROOT;
		//data.reserve_dir = RESERVE_ROOT;
		//data.update_root_folder = UPD_ROOT;

		data.m_pathSubstitutionMap.push_back(std::make_pair<NoCaseString,NoCaseString>(L"%BaseFolder%", BASES_ROOT));
		data.m_pathSubstitutionMap.push_back(std::make_pair<NoCaseString,NoCaseString>(L"%Folder%", ROLLBACK_ROOT));
		data.m_pathSubstitutionMap.push_back(std::make_pair<NoCaseString,NoCaseString>(L"%UpdateRoot%", UPD_ROOT));
		data.m_pathSubstitutionMap.push_back(std::make_pair<NoCaseString,NoCaseString>(L"%DSKM%", DSKM_ROOT));
		data.m_pathSubstitutionMap.push_back(std::make_pair<NoCaseString,NoCaseString>(L"%DataFolder%", UPD_ROOT));

		data.self_region = L"<default>";
		data.product_lang.push_back( NoCaseString(L"<default>"));

		data.os.os = L"Windows";
		data.os.version = L"5.1";

		data.m_componentsToUpdate.push_back ( L"AVS" );
		data.m_componentsToUpdate.push_back (NoCaseString(L"CORE") );
		data.connect_tmo = 30;

		data.m_sourceList.addSource (UST_UserURL, std::wstring(COLL_ROOT).append(L"\\Bases_1").c_str(), true, L"diffs");
		data.UpdaterListFlags.update_bases = true;

		Callbacks cbacks;
		TestCallbackInterface testCallback (&testLog, &cbacks);
		TestJournalInterface testJournal;
		testCallback.SetUpdaterConfiguration (data);

		Updater upd (testCallback.m_dp, testCallback, testJournal, &testLog);
		HANDLE hDoUpdate;

		cbacks.bFlags[Callbacks::checkPreInstalledComponent] = true;
		CREATE_EVENTS(checkPreInstalledComponent)
		cbacks.bFlags[Callbacks::checkIfFileOptional] = true;
		CREATE_EVENTS(checkIfFileOptional)

		doUpdater do_upd(&upd, hDoUpdate);

		TestCallbackInterface::_checkPreInstalledComponent& cpic = testCallback.GetCheckPreinstalledComponent();
		//inform, that AVS component is corrupted
		for (int i = 0; i < 2; ++i)
		{
			WAIT_FOR_READY_EVENT(checkPreInstalledComponent)

			if (cpic.compId == L"AVS")
			{
				cpic.result = false;
			}
			else
			{
				cpic.result = true;
			}

			SET_GOON_EVENT(checkPreInstalledComponent);
		}
		//inform, that all files of the component are optional

		testCallback.SetOptionalFileAnswer (true);
		for (int i = 0; i < 4; ++i)
		{
			wstring avs_files[] = {L"av-i386-0607g.xml", L"base101.avc", L"base102.avc", L"base103.avc"};
			WAIT_FOR_READY_EVENT(checkIfFileOptional)
			const FileInfo& absentFile = testCallback.GetAbsentFile();
			QC_BOOST_CHECK_MESSAGE (absentFile.m_filename == avs_files[i].c_str(), L"Incorrect filename\n");
			SET_GOON_EVENT(checkIfFileOptional);
		}

		//inform, that now the components are alright
		for (int i = 0; i < 2; ++i)
		{
			WAIT_FOR_READY_EVENT(checkPreInstalledComponent)

				if (cpic.compId == L"AVS")
				{
					cpic.result = true;
				}
				else
				{
					cpic.result = true;
				}

				SET_GOON_EVENT(checkPreInstalledComponent);
		}

		WaitForSingleObject (hDoUpdate, INFINITE);

		QC_BOOST_CHECK_MESSAGE (_helper::DeleteFile(std::wstring(BASES_ROOT).append (L"\\base103.avc")),
			L"old base file not found");
		QC_BOOST_CHECK_MESSAGE (_helper::IsFoldersEqual(DIFFTOOL_ROOT,
			std::wstring(COLL_ROOT).append(L"\\Bases_1\\plain_wo_avs").c_str(), BASES_ROOT), L"Incorrect bases\n");

		QC_BOOST_CHECK_MESSAGE (_helper::IsFoldersEqual(DIFFTOOL_ROOT,
			std::wstring(COLL_ROOT).append(L"\\Bases_1\\data").c_str(), UPD_ROOT), L"Incorrect index\n");

		QC_BOOST_CHECK_MESSAGE ( NO_ERROR == do_upd.GetError(), L"Wrong return code\n" );
	}
	
	{
		std::wcout<<L"Error in AVS component (mandatory)\n";

		TestLog testLog;

		_helper::DeleteDirectory(ROOT);
		KLUPD::createFolder (BASES_ROOT, &testLog);
		KLUPD::createFolder (UPD_ROOT, &testLog);

		_helper::CopyFolder(std::wstring(COLL_ROOT).append(L"\\Bases_NULL\\plain").c_str(),BASES_ROOT);
		_helper::CloneFile (std::wstring(COLL_ROOT).append(L"\\Bases_NULL\\index\\6\\u0607g.xml").c_str(),
			std::wstring (UPD_ROOT).append (L"\\u0607g.xml"), false);

		UpdaterConfiguration data(1, 1, L"1");

		data.temporary_dir = TEMP_ROOT;
		//data.reserve_dir = RESERVE_ROOT;
		//data.update_root_folder = UPD_ROOT;

		data.m_pathSubstitutionMap.push_back(std::make_pair<NoCaseString,NoCaseString>(L"%BaseFolder%", BASES_ROOT));
		data.m_pathSubstitutionMap.push_back(std::make_pair<NoCaseString,NoCaseString>(L"%Folder%", ROLLBACK_ROOT));
		data.m_pathSubstitutionMap.push_back(std::make_pair<NoCaseString,NoCaseString>(L"%UpdateRoot%", UPD_ROOT));
		data.m_pathSubstitutionMap.push_back(std::make_pair<NoCaseString,NoCaseString>(L"%DSKM%", DSKM_ROOT));
		data.m_pathSubstitutionMap.push_back(std::make_pair<NoCaseString,NoCaseString>(L"%DataFolder%", UPD_ROOT));

		data.self_region = L"<default>";
		data.product_lang.push_back( NoCaseString(L"<default>"));

		data.os.os = L"Windows";
		data.os.version = L"5.1";

		data.m_componentsToUpdate.push_back ( L"AVS" );
		data.m_componentsToUpdate.push_back (NoCaseString(L"CORE") );
		data.connect_tmo = 30;

		data.m_sourceList.addSource (UST_UserURL, std::wstring(COLL_ROOT).append(L"\\Bases_1").c_str(), true, L"diffs");
		data.UpdaterListFlags.update_bases = true;

		Callbacks cbacks;
		TestCallbackInterface testCallback (&testLog, &cbacks);
		TestJournalInterface testJournal;
		testCallback.SetUpdaterConfiguration (data);

		Updater upd (testCallback.m_dp, testCallback, testJournal, &testLog);
		HANDLE hDoUpdate;

		cbacks.bFlags[Callbacks::checkPreInstalledComponent] = true;
		CREATE_EVENTS(checkPreInstalledComponent)
			cbacks.bFlags[Callbacks::checkIfFileOptional] = true;
		CREATE_EVENTS(checkIfFileOptional)

			doUpdater do_upd(&upd, hDoUpdate);

		TestCallbackInterface::_checkPreInstalledComponent& cpic = testCallback.GetCheckPreinstalledComponent();
		//inform, that AVS component is corrupted
		for (int i = 0; i < 2; ++i)
		{
			WAIT_FOR_READY_EVENT(checkPreInstalledComponent)

				if (cpic.compId == L"AVS")
				{
					cpic.result = false;
				}
				else
				{
					cpic.result = true;
				}

				SET_GOON_EVENT(checkPreInstalledComponent);
		}
		//inform, that all files of the component are optional

		testCallback.SetOptionalFileAnswer (false);
		wstring avs_file = L"av-i386-0607g.xml";
		WAIT_FOR_READY_EVENT(checkIfFileOptional)
		const FileInfo& absentFile = testCallback.GetAbsentFile();
		QC_BOOST_CHECK_MESSAGE (absentFile.m_filename == avs_file.c_str(), L"Incorrect filename\n");
		SET_GOON_EVENT(checkIfFileOptional);

		//rollback avs
		WAIT_FOR_READY_EVENT(checkPreInstalledComponent)
		cpic.result = true;
		SET_GOON_EVENT(checkPreInstalledComponent)
		//install core
		WAIT_FOR_READY_EVENT(checkPreInstalledComponent)
		cpic.result = true;
		SET_GOON_EVENT(checkPreInstalledComponent)

		WaitForSingleObject (hDoUpdate, INFINITE);

		QC_BOOST_CHECK_MESSAGE (_helper::DeleteFile(std::wstring(BASES_ROOT).append (L"\\base103.avc")),
			L"old base file not found");
		QC_BOOST_CHECK_MESSAGE (_helper::IsFoldersEqual(DIFFTOOL_ROOT,
			std::wstring(COLL_ROOT).append(L"\\Bases_NULL\\plain").c_str(), BASES_ROOT), L"Incorrect bases\n");
		QC_BOOST_CHECK_MESSAGE (_helper::IsFoldersEqual(DIFFTOOL_ROOT,
			std::wstring(COLL_ROOT).append(L"\\Bases_NULL\\data").c_str(), UPD_ROOT), L"Incorrect index\n");
		QC_BOOST_CHECK_MESSAGE ( NO_ERROR == do_upd.GetError(), L"Wrong return code\n" );
	}
	
	{
		std::wcout<<L"Error in AVS component (mandatory) + Retranslation\n";

		TestLog testLog;

		_helper::DeleteDirectory(ROOT);

		//retr
		KLUPD::createFolder (std::wstring (RETR_ROOT).append (L"\\index\\6").c_str(), &testLog);
		KLUPD::createFolder (std::wstring (RETR_ROOT).append (L"\\bases").c_str(), &testLog);
		_helper::CopyFolder(std::wstring(COLL_ROOT).append(L"\\Bases_NULL\\bases").c_str(),
			std::wstring (RETR_ROOT).append (L"\\bases").c_str());
		_helper::CloneFile (std::wstring(COLL_ROOT).append(L"\\Bases_NULL\\index\\6\\u0607g.xml").c_str(),
			std::wstring (RETR_ROOT).append (L"\\index\\6\\u0607g.xml"), false);
		//upd
		KLUPD::createFolder (BASES_ROOT, &testLog);
		KLUPD::createFolder (UPD_ROOT, &testLog);

		_helper::CopyFolder(std::wstring(COLL_ROOT).append(L"\\Bases_NULL\\plain").c_str(),BASES_ROOT);
		_helper::CloneFile (std::wstring(COLL_ROOT).append(L"\\Bases_NULL\\index\\6\\u0607g.xml").c_str(),
			std::wstring (UPD_ROOT).append (L"\\u0607g.xml"), false);

		UpdaterConfiguration data(1, 1, L"1");

		data.temporary_dir = TEMP_ROOT;
		//data.reserve_dir = RESERVE_ROOT;
		//data.update_root_folder = UPD_ROOT;
		data.retranslation_dir = RETR_ROOT;

		data.m_pathSubstitutionMap.push_back(std::make_pair<NoCaseString,NoCaseString>(L"%BaseFolder%", BASES_ROOT));
		data.m_pathSubstitutionMap.push_back(std::make_pair<NoCaseString,NoCaseString>(L"%Folder%", ROLLBACK_ROOT));
		data.m_pathSubstitutionMap.push_back(std::make_pair<NoCaseString,NoCaseString>(L"%UpdateRoot%", UPD_ROOT));
		data.m_pathSubstitutionMap.push_back(std::make_pair<NoCaseString,NoCaseString>(L"%DSKM%", DSKM_ROOT));
		data.m_pathSubstitutionMap.push_back(std::make_pair<NoCaseString,NoCaseString>(L"%Data%", DATA_ROOT));
		data.m_pathSubstitutionMap.push_back(std::make_pair<NoCaseString,NoCaseString>(L"%DataFolder%", UPD_ROOT));

		data.self_region = L"<default>";
		data.product_lang.push_back( NoCaseString(L"<default>"));

		data.os.os = L"Windows";
		data.os.version = L"5.1";

		data.RetranslatedObjects.m_componentsToRetranslate.push_back (L"AVS");
		data.RetranslatedObjects.m_componentsToRetranslate.push_back (L"CORE");

		data.m_componentsToUpdate.push_back (L"AVS");
		data.m_componentsToUpdate.push_back (L"CORE");

		data.updates_retranslation = true;
		data.retranslate_only_listed_apps = true;
		data.connect_tmo = 30;

		data.m_sourceList.addSource (UST_UserURL, std::wstring(COLL_ROOT).append(L"\\Bases_1").c_str(), true, L"diffs");

		data.RetranslationListFlags.update_bases = true;
		data.RetranslationListFlags.automatically_apply_available_updates = false;
		data.RetranslationListFlags.apply_urgent_updates = false;

		data.UpdaterListFlags.update_bases = true;
		data.UpdaterListFlags.automatically_apply_available_updates = false;
		data.UpdaterListFlags.apply_urgent_updates = false;

		Callbacks cbacks;
		TestCallbackInterface testCallback (&testLog, &cbacks);
		TestJournalInterface testJournal;
		testCallback.SetUpdaterConfiguration (data);

		Updater upd (testCallback.m_dp, testCallback, testJournal, &testLog);
		HANDLE hDoUpdate;

		cbacks.bFlags[Callbacks::checkPreInstalledComponent] = true;
		CREATE_EVENTS(checkPreInstalledComponent)
		cbacks.bFlags[Callbacks::checkIfFileOptional] = true;
		CREATE_EVENTS(checkIfFileOptional)

		doUpdater do_upd(&upd, hDoUpdate);

		TestCallbackInterface::_checkPreInstalledComponent& cpic = testCallback.GetCheckPreinstalledComponent();
		//inform, that AVS component is corrupted
		for (int i = 0; i < 2; ++i)
		{
			WAIT_FOR_READY_EVENT(checkPreInstalledComponent)

				if (cpic.compId == L"AVS")
				{
					cpic.result = false;
				}
				else
				{
					cpic.result = true;
				}

				SET_GOON_EVENT(checkPreInstalledComponent);
		}
		//inform, that all files of the component are optional

		testCallback.SetOptionalFileAnswer (false);
		wstring avs_file = L"av-i386-0607g.xml";
		WAIT_FOR_READY_EVENT(checkIfFileOptional)
			const FileInfo& absentFile = testCallback.GetAbsentFile();
		QC_BOOST_CHECK_MESSAGE (absentFile.m_filename == avs_file.c_str(), L"Incorrect filename\n");
		SET_GOON_EVENT(checkIfFileOptional);

		//rollback avs
		WAIT_FOR_READY_EVENT(checkPreInstalledComponent)
		cpic.result = true;
		SET_GOON_EVENT(checkPreInstalledComponent)
		//install core
		WAIT_FOR_READY_EVENT(checkPreInstalledComponent)
		cpic.result = true;
		SET_GOON_EVENT(checkPreInstalledComponent)

		WaitForSingleObject (hDoUpdate, INFINITE);

		//retr
		QC_BOOST_CHECK_MESSAGE (_helper::DeleteFile(std::wstring(RETR_ROOT).append (L"\\bases\\av\\avc\\i386\\base100.avc")),
			L"old base file not found");
		QC_BOOST_CHECK_MESSAGE (_helper::IsFoldersEqual(DIFFTOOL_ROOT,
			std::wstring(COLL_ROOT).append(L"\\Bases_1\\bases\\av").c_str(),
			std::wstring(RETR_ROOT).append(L"\\bases\\av")), L"Incorrect av comp\n");
		QC_BOOST_CHECK_MESSAGE (_helper::IsFoldersEqual(DIFFTOOL_ROOT,
			std::wstring(COLL_ROOT).append(L"\\Bases_1\\bases\\blst").c_str(),
			std::wstring(RETR_ROOT).append(L"\\bases\\blst")), L"Incorrect blst comp\n");	
		QC_BOOST_CHECK_MESSAGE (_helper::IsFoldersEqual(DIFFTOOL_ROOT,
			std::wstring(COLL_ROOT).append(L"\\Bases_1\\data").c_str(),
			std::wstring(RETR_ROOT).append(L"\\index\\6")), L"Incorrect index\n");
		//upd
		QC_BOOST_CHECK_MESSAGE (_helper::DeleteFile(std::wstring(BASES_ROOT).append (L"\\base103.avc")),
			L"old base file not found");
		QC_BOOST_CHECK_MESSAGE (_helper::IsFoldersEqual(DIFFTOOL_ROOT,
			std::wstring(COLL_ROOT).append(L"\\Bases_NULL\\plain").c_str(), BASES_ROOT), L"Incorrect bases\n");
		QC_BOOST_CHECK_MESSAGE (_helper::IsFoldersEqual(DIFFTOOL_ROOT,
			std::wstring(COLL_ROOT).append(L"\\Bases_NULL\\data").c_str(), UPD_ROOT), L"Incorrect index\n");

		QC_BOOST_CHECK_MESSAGE ( NO_ERROR == do_upd.GetError(), L"Wrong return code\n" );
	}
	

	{
		std::wcout<<L"Control sequence of calls - nameless component\n";

		TestLog testLog;

		_helper::DeleteDirectory(ROOT);
		KLUPD::createFolder (BASES_ROOT, &testLog);
		KLUPD::createFolder (UPD_ROOT, &testLog);

		_helper::CopyFolder(std::wstring(COLL_ROOT).append(L"\\Bases_NULL\\plain").c_str(),BASES_ROOT);
		_helper::CloneFile (std::wstring(COLL_ROOT).append(L"\\Bases_NULL\\index\\6\\u0607g.xml").c_str(),
			std::wstring (UPD_ROOT).append (L"\\u0607g.xml"), false);

		UpdaterConfiguration data(1, 1, L"1");

		data.temporary_dir = TEMP_ROOT;
		//data.reserve_dir = RESERVE_ROOT;
		//data.update_root_folder = UPD_ROOT;

		data.m_pathSubstitutionMap.push_back(std::make_pair<NoCaseString,NoCaseString>(L"%BaseFolder%", BASES_ROOT));
		data.m_pathSubstitutionMap.push_back(std::make_pair<NoCaseString,NoCaseString>(L"%Folder%", ROLLBACK_ROOT));
		data.m_pathSubstitutionMap.push_back(std::make_pair<NoCaseString,NoCaseString>(L"%UpdateRoot%", UPD_ROOT));
		data.m_pathSubstitutionMap.push_back(std::make_pair<NoCaseString,NoCaseString>(L"%DSKM%", DSKM_ROOT));
		data.m_pathSubstitutionMap.push_back(std::make_pair<NoCaseString,NoCaseString>(L"%DataFolder%", UPD_ROOT));

		data.self_region = L"<default>";
		data.product_lang.push_back( NoCaseString(L"<default>"));

		data.os.os = L"Windows";
		data.os.version = L"5.1";

		data.m_componentsToUpdate.push_back ( L"AVS" );
		data.m_componentsToUpdate.push_back (NoCaseString(L"CORE") );
		data.connect_tmo = 30;

		data.m_sourceList.addSource (UST_UserURL, std::wstring(COLL_ROOT).append(L"\\Bases_1_nameless_comp").c_str(), true, L"diffs");
		data.UpdaterListFlags.update_bases = true;

		Callbacks cbacks;
		TestCallbackInterface testCallback (&testLog, &cbacks);
		TestJournalInterface testJournal;
		testCallback.SetUpdaterConfiguration (data);

		Updater upd (testCallback.m_dp, testCallback, testJournal, &testLog);
		HANDLE hDoUpdate;

		cbacks.bFlags[Callbacks::checkPreInstalledComponent] = true;
		CREATE_EVENTS(checkPreInstalledComponent)

			doUpdater do_upd(&upd, hDoUpdate);

		TestCallbackInterface::_checkPreInstalledComponent& cpic = testCallback.GetCheckPreinstalledComponent();
		for (int i = 0; i < 3; ++i)
		{
			WAIT_FOR_READY_EVENT(checkPreInstalledComponent)

			std::wcout<<L"Component "<<cpic.compId.toWideChar()<<L"\n";
			std::wcout<<cpic.fv.size()<<L"\n";
			for (std::vector<FileInfo>::iterator it = cpic.fv.begin(); it!=cpic.fv.end(); ++it)
				std::wcout<<it->m_filename.m_value.toWideChar()<<L"\n";

			if (cpic.compId == L"")
			{
				QC_BOOST_REQUIRE_MESSAGE (cpic.fv.size() == 4, L"Incorrect file list size\n");
				QC_BOOST_CHECK_MESSAGE (cpic.fv.at(0).m_filename == L"ah-i386-0607g.xml", L"Incorrect file name\n");
				QC_BOOST_CHECK_MESSAGE (cpic.fv.at(1).m_filename == L"u0607g.xml", L"Incorrect file name\n");
				QC_BOOST_CHECK_MESSAGE (cpic.fv.at(2).m_filename == L"klick.kdz", L"Incorrect file name\n");
				QC_BOOST_CHECK_MESSAGE (cpic.fv.at(3).m_filename == L"klin.kdz", L"Incorrect file name\n");
			}
			if (cpic.compId == L"AVS")
			{
				QC_BOOST_REQUIRE_MESSAGE (cpic.fv.size() == 4, L"Incorrect file list size\n");
				QC_BOOST_CHECK_MESSAGE (cpic.fv.at(0).m_filename == L"av-i386-0607g.xml", L"Incorrect file name\n");
				QC_BOOST_CHECK_MESSAGE (cpic.fv.at(1).m_filename == L"base101.avc", L"Incorrect file name\n");
				QC_BOOST_CHECK_MESSAGE (cpic.fv.at(2).m_filename == L"base102.avc", L"Incorrect file name\n");
				QC_BOOST_CHECK_MESSAGE (cpic.fv.at(3).m_filename == L"base103.avc", L"Incorrect file name\n");
			}
			if (cpic.compId == L"CORE")
			{
				QC_BOOST_REQUIRE_MESSAGE (cpic.fv.size() == 7, L"Incorrect file list size\n");
				QC_BOOST_CHECK_MESSAGE (cpic.fv.at(0).m_filename == L"black.lst", L"Incorrect file name\n");
				QC_BOOST_CHECK_MESSAGE (cpic.fv.at(1).m_filename == L"blst-0607g.xml", L"Incorrect file name\n");
				QC_BOOST_CHECK_MESSAGE (cpic.fv.at(2).m_filename == L"kl-0607g.pbv", L"Incorrect file name\n");
				QC_BOOST_CHECK_MESSAGE (cpic.fv.at(3).m_filename == L"xml-0607g.pbv", L"Incorrect file name\n");
				QC_BOOST_CHECK_MESSAGE (cpic.fv.at(4).m_filename == L"av-i386-0607g.krg", L"Incorrect file name\n");
				QC_BOOST_CHECK_MESSAGE (cpic.fv.at(5).m_filename == L"ah-i386-0607g.krg", L"Incorrect file name\n");
				QC_BOOST_CHECK_MESSAGE (cpic.fv.at(6).m_filename == L"blst-0607g.krg", L"Incorrect file name\n");
			}
			SET_GOON_EVENT(checkPreInstalledComponent);
		}

		WaitForSingleObject (hDoUpdate, INFINITE);

		QC_BOOST_CHECK_MESSAGE (_helper::DeleteFile(std::wstring(BASES_ROOT).append (L"\\base100.avc")),
			L"old base file not found");
		QC_BOOST_CHECK_MESSAGE (_helper::IsFoldersEqual(DIFFTOOL_ROOT,
			std::wstring(COLL_ROOT).append(L"\\Bases_1_nameless_comp\\plain").c_str(), BASES_ROOT), L"Incorrect bases\n");

		QC_BOOST_CHECK_MESSAGE (_helper::IsFoldersEqual(DIFFTOOL_ROOT,
			std::wstring(COLL_ROOT).append(L"\\Bases_1_nameless_comp\\data").c_str(), UPD_ROOT), L"Incorrect index\n");

		QC_BOOST_CHECK_MESSAGE ( NO_ERROR == do_upd.GetError(), L"Wrong return code\n" );
	}

}

void TestSuite::TestLockComponents ()
{
	using namespace KLUPD;
	std::wcout<<L"TestLockComponents\n";
	Logger::Case _case (m_logger, m_testp, m_step, L"TestLockComponents");
	
	{//installation only
		std::wcout<<L"Installation only\n";
		TestLog testLog;

		_helper::DeleteDirectory(ROOT);
		KLUPD::createFolder (BASES_ROOT, &testLog);
		KLUPD::createFolder (UPD_ROOT, &testLog);

		_helper::CopyFolder(std::wstring(COLL_ROOT).append(L"\\Bases_NULL\\plain").c_str(),BASES_ROOT);
		_helper::CloneFile (std::wstring(COLL_ROOT).append(L"\\Bases_NULL\\index\\6\\u0607g.xml").c_str(),
		std::wstring (UPD_ROOT).append (L"\\u0607g.xml"), false);

		UpdaterConfiguration data(1, 1, L"1");

		data.temporary_dir = TEMP_ROOT;
		//data.reserve_dir = RESERVE_ROOT;
		//data.update_root_folder = UPD_ROOT;

		data.m_pathSubstitutionMap.push_back(std::make_pair<NoCaseString,NoCaseString>(L"%BaseFolder%", BASES_ROOT));
		data.m_pathSubstitutionMap.push_back(std::make_pair<NoCaseString,NoCaseString>(L"%Folder%", ROLLBACK_ROOT));
		data.m_pathSubstitutionMap.push_back(std::make_pair<NoCaseString,NoCaseString>(L"%UpdateRoot%", UPD_ROOT));
		data.m_pathSubstitutionMap.push_back(std::make_pair<NoCaseString,NoCaseString>(L"%DSKM%", DSKM_ROOT));
		data.m_pathSubstitutionMap.push_back(std::make_pair<NoCaseString,NoCaseString>(L"%DataFolder%", UPD_ROOT));

		data.self_region = L"<default>";
		data.product_lang.push_back( NoCaseString(L"<default>"));

		data.os.os = L"Windows";
		data.os.version = L"5.1";

		data.m_componentsToUpdate.push_back ( L"AVS" );
		data.m_componentsToUpdate.push_back (NoCaseString(L"CORE") );
		data.connect_tmo = 30;

		data.m_sourceList.addSource (UST_UserURL, std::wstring(COLL_ROOT).append(L"\\Bases_1").c_str(), true, L"diffs");
		data.UpdaterListFlags.update_bases = true;

		Callbacks cbacks;
		TestCallbackInterface testCallback (&testLog, &cbacks);
		TestJournalInterface testJournal;
		testCallback.SetUpdaterConfiguration (data);

		Updater upd (testCallback.m_dp, testCallback, testJournal, &testLog);
		HANDLE hDoUpdate;

		cbacks.bFlags[Callbacks::lockComponentForUpdate] = true;
		cbacks.bFlags[Callbacks::unlockComponentForUpdate] = true;
		CREATE_EVENTS(lockComponentForUpdate)
		CREATE_EVENTS(unlockComponentForUpdate)

		doUpdater do_upd(&upd, hDoUpdate);

		WAIT_FOR_READY_EVENT(lockComponentForUpdate)
		QC_BOOST_CHECK_MESSAGE (testCallback.GetComp2Lock() == L"", L"Incorrect component\n");
		SET_GOON_EVENT(lockComponentForUpdate)
		WAIT_FOR_READY_EVENT(unlockComponentForUpdate)
		QC_BOOST_CHECK_MESSAGE (testCallback.GetComp2UnLock() == L"", L"Incorrect component\n");
		SET_GOON_EVENT(unlockComponentForUpdate)

		WAIT_FOR_READY_EVENT(lockComponentForUpdate)
		QC_BOOST_CHECK_MESSAGE (testCallback.GetComp2Lock() == L"AVS", L"Incorrect component\n");
		SET_GOON_EVENT(lockComponentForUpdate)
		WAIT_FOR_READY_EVENT(unlockComponentForUpdate)
		QC_BOOST_CHECK_MESSAGE (testCallback.GetComp2UnLock() == L"AVS", L"Incorrect component\n");
		SET_GOON_EVENT(unlockComponentForUpdate)

		WAIT_FOR_READY_EVENT(lockComponentForUpdate)
		QC_BOOST_CHECK_MESSAGE (testCallback.GetComp2Lock() == L"CORE", L"Incorrect component\n");
		SET_GOON_EVENT(lockComponentForUpdate)
		WAIT_FOR_READY_EVENT(unlockComponentForUpdate)
		QC_BOOST_CHECK_MESSAGE (testCallback.GetComp2UnLock() == L"CORE", L"Incorrect component\n");
		SET_GOON_EVENT(unlockComponentForUpdate)

		WaitForSingleObject (hDoUpdate, INFINITE);

		QC_BOOST_CHECK_MESSAGE (_helper::DeleteFile(std::wstring(BASES_ROOT).append (L"\\base100.avc")),
					L"old base file not found");
		QC_BOOST_CHECK_MESSAGE (_helper::IsFoldersEqual(DIFFTOOL_ROOT,
					std::wstring(COLL_ROOT).append(L"\\Bases_1\\plain").c_str(), BASES_ROOT), L"Incorrect bases\n");

		QC_BOOST_CHECK_MESSAGE (_helper::IsFoldersEqual(DIFFTOOL_ROOT,
					std::wstring(COLL_ROOT).append(L"\\Bases_1\\data").c_str(), UPD_ROOT), L"Incorrect index\n");

		QC_BOOST_CHECK_MESSAGE ( NO_ERROR == do_upd.GetError(), L"Wrong return code\n" );
	}
	
	{//installation and rollback
		std::wcout<<L"Installation and rollback\n";
		TestLog testLog;

		_helper::DeleteDirectory(ROOT);
		KLUPD::createFolder (BASES_ROOT, &testLog);
		KLUPD::createFolder (UPD_ROOT, &testLog);

		_helper::CopyFolder(std::wstring(COLL_ROOT).append(L"\\Bases_NULL\\plain").c_str(),BASES_ROOT);
		_helper::CloneFile (std::wstring(COLL_ROOT).append(L"\\Bases_NULL\\index\\6\\u0607g.xml").c_str(),
			std::wstring (UPD_ROOT).append (L"\\u0607g.xml"), false);

		UpdaterConfiguration data(1, 1, L"1");

		data.temporary_dir = TEMP_ROOT;
		//data.reserve_dir = RESERVE_ROOT;
		//data.update_root_folder = UPD_ROOT;

		data.m_pathSubstitutionMap.push_back(std::make_pair<NoCaseString,NoCaseString>(L"%BaseFolder%", BASES_ROOT));
		data.m_pathSubstitutionMap.push_back(std::make_pair<NoCaseString,NoCaseString>(L"%Folder%", ROLLBACK_ROOT));
		data.m_pathSubstitutionMap.push_back(std::make_pair<NoCaseString,NoCaseString>(L"%UpdateRoot%", UPD_ROOT));
		data.m_pathSubstitutionMap.push_back(std::make_pair<NoCaseString,NoCaseString>(L"%DSKM%", DSKM_ROOT));
		data.m_pathSubstitutionMap.push_back(std::make_pair<NoCaseString,NoCaseString>(L"%DataFolder%", UPD_ROOT));

		data.self_region = L"<default>";
		data.product_lang.push_back( NoCaseString(L"<default>"));

		data.os.os = L"Windows";
		data.os.version = L"5.1";

		data.m_componentsToUpdate.push_back ( L"AVS" );
		data.m_componentsToUpdate.push_back (NoCaseString(L"CORE") );
		data.connect_tmo = 30;

		data.m_sourceList.addSource (UST_UserURL, std::wstring(COLL_ROOT).append(L"\\Bases_1").c_str(), true, L"diffs");
		data.UpdaterListFlags.update_bases = true;

		Callbacks cbacks;
		TestCallbackInterface testCallback (&testLog, &cbacks);
		TestJournalInterface testJournal;
		testCallback.SetUpdaterConfiguration (data);

		Updater upd (testCallback.m_dp, testCallback, testJournal, &testLog);
		HANDLE hDoUpdate;

		cbacks.bFlags[Callbacks::lockComponentForUpdate] = true;
		cbacks.bFlags[Callbacks::unlockComponentForUpdate] = true;
		cbacks.bFlags[Callbacks::checkPreInstalledComponent] = true;
		CREATE_EVENTS(lockComponentForUpdate)
		CREATE_EVENTS(unlockComponentForUpdate)
		CREATE_EVENTS(checkPreInstalledComponent)

		doUpdater do_upd(&upd, hDoUpdate);
		TestCallbackInterface::_checkPreInstalledComponent& cpic = testCallback.GetCheckPreinstalledComponent();
		testCallback.SetOptionalFileAnswer (false);
		//install
		WAIT_FOR_READY_EVENT(lockComponentForUpdate)
		QC_BOOST_CHECK_MESSAGE (testCallback.GetComp2Lock() == L"", L"Incorrect component\n");
		SET_GOON_EVENT(lockComponentForUpdate)
			WAIT_FOR_READY_EVENT(checkPreInstalledComponent)
			cpic.result = true;
			SET_GOON_EVENT(checkPreInstalledComponent)
		WAIT_FOR_READY_EVENT(unlockComponentForUpdate)
		QC_BOOST_CHECK_MESSAGE (testCallback.GetComp2UnLock() == L"", L"Incorrect component\n");
		SET_GOON_EVENT(unlockComponentForUpdate)

		

		WAIT_FOR_READY_EVENT(lockComponentForUpdate)
		QC_BOOST_CHECK_MESSAGE (testCallback.GetComp2Lock() == L"AVS", L"Incorrect component\n");
		SET_GOON_EVENT(lockComponentForUpdate)
			WAIT_FOR_READY_EVENT(checkPreInstalledComponent)
			cpic.result = false;
			SET_GOON_EVENT(checkPreInstalledComponent)
		WAIT_FOR_READY_EVENT(unlockComponentForUpdate)
		QC_BOOST_CHECK_MESSAGE (testCallback.GetComp2UnLock() == L"AVS", L"Incorrect component\n");
		SET_GOON_EVENT(unlockComponentForUpdate)

		

		//rollback
		WAIT_FOR_READY_EVENT(lockComponentForUpdate)
		QC_BOOST_CHECK_MESSAGE (testCallback.GetComp2Lock() == L"AVS", L"Incorrect component\n");
		SET_GOON_EVENT(lockComponentForUpdate)
			//idle
			WAIT_FOR_READY_EVENT(checkPreInstalledComponent)
			cpic.result = true;
			SET_GOON_EVENT(checkPreInstalledComponent)
		WAIT_FOR_READY_EVENT(unlockComponentForUpdate)
		QC_BOOST_CHECK_MESSAGE (testCallback.GetComp2UnLock() == L"AVS", L"Incorrect component\n");
		SET_GOON_EVENT(unlockComponentForUpdate)
		

		//rollback
		WAIT_FOR_READY_EVENT(lockComponentForUpdate)
		QC_BOOST_CHECK_MESSAGE (testCallback.GetComp2Lock() == L"", L"Incorrect component\n");
		SET_GOON_EVENT(lockComponentForUpdate)
			//idle
			WAIT_FOR_READY_EVENT(checkPreInstalledComponent)
			cpic.result = true;
			SET_GOON_EVENT(checkPreInstalledComponent)
		WAIT_FOR_READY_EVENT(unlockComponentForUpdate)
		QC_BOOST_CHECK_MESSAGE (testCallback.GetComp2UnLock() == L"", L"Incorrect component\n");
		SET_GOON_EVENT(unlockComponentForUpdate)
		

		WaitForSingleObject (hDoUpdate, INFINITE);

		QC_BOOST_CHECK_MESSAGE (_helper::DeleteFile(std::wstring(BASES_ROOT).append (L"\\base103.avc")),
			L"old base file not found");
		QC_BOOST_CHECK_MESSAGE (_helper::IsFoldersEqual(DIFFTOOL_ROOT,
			std::wstring(COLL_ROOT).append(L"\\Bases_NULL\\plain").c_str(), BASES_ROOT), L"Incorrect bases\n");
		QC_BOOST_CHECK_MESSAGE (_helper::IsFoldersEqual(DIFFTOOL_ROOT,
			std::wstring(COLL_ROOT).append(L"\\Bases_NULL\\data").c_str(), UPD_ROOT), L"Incorrect index\n");

		QC_BOOST_CHECK_MESSAGE ( NO_ERROR == do_upd.GetError(), L"Wrong return code\n" );
	}
}

void TestSuite::TestAdjustLoginName ()
{
	std::wcout<<L"TestAdjustLoginName\n";
	Logger::Case _case (m_logger, m_testp, m_step, L"TestAdjustLoginName");

	{//OK
		TestLog testLog;

		_helper::DeleteDirectory(ROOT);
		KLUPD::createFolder (BASES_ROOT, &testLog);
		KLUPD::createFolder (UPD_ROOT, &testLog);
		KLUPD::createFolder (PRODUCT_ROOT, &testLog);

		_helper::CopyFolder(std::wstring(COLL_ROOT).append(L"\\Bases_NULL\\plain").c_str(),BASES_ROOT);
		_helper::CloneFile (std::wstring(COLL_ROOT).append(L"\\Bases_NULL\\index\\6\\u0607g.xml").c_str(),
			std::wstring (UPD_ROOT).append (L"\\u0607g.xml"), false);

		UpdaterConfiguration data(1, 1, L"1");

		data.temporary_dir = TEMP_ROOT;

		data.m_pathSubstitutionMap.push_back(std::make_pair<NoCaseString,NoCaseString>(L"%BaseFolder%", BASES_ROOT));
		data.m_pathSubstitutionMap.push_back(std::make_pair<NoCaseString,NoCaseString>(L"%Folder%", ROLLBACK_ROOT));
		data.m_pathSubstitutionMap.push_back(std::make_pair<NoCaseString,NoCaseString>(L"%UpdateRoot%", UPD_ROOT));
		data.m_pathSubstitutionMap.push_back(std::make_pair<NoCaseString,NoCaseString>(L"%DSKM%", DSKM_ROOT));
		data.m_pathSubstitutionMap.push_back(std::make_pair<NoCaseString,NoCaseString>(L"%DataFolder%", UPD_ROOT));

		data.self_region = L"<default>";
		data.product_lang.push_back( NoCaseString(L"<default>"));

		data.os.os = L"Windows";
		data.os.version = L"5.1";

		data.m_componentsToUpdate.push_back (L"AVS");
		data.m_componentsToUpdate.push_back (L"CORE");
		data.connect_tmo = 30;

		data.m_sourceList.addSource (UST_UserURL, L"http://tl-srv-w2k", true, L"diffs");
		data.proxy_url = L"tl-prx-nta";
		data.proxy_port = 3128;

		data.m_proxyAuthorizationCredentials.userName (L"tester@avp.ru");
		data.m_proxyAuthorizationCredentials.password (L"kf;0dsq Ntcnth");

		data.m_httpAuthorizationMethods.push_back (ntlmAuthorizationWithCredentials);
		data.UpdaterListFlags.update_bases = true;

		Callbacks cbacks;
		TestCallbackInterface testCallback (&testLog, &cbacks);
		TestJournalInterface testJournal;
		testCallback.SetUpdaterConfiguration (data);
		testCallback.SetPrimaryIndex( FileInfo (L"u0607g.xml", L"updater13/bases_1/index/6", FileInfo::index, true) );

		FileInfo u0607g ( L"u0607g.xml", L"updater13/bases_1/index/6", FileInfo::index, true );
		u0607g.m_localPath = UPD_ROOT;
		testCallback.SetPrimaryIndex(u0607g);

		Updater upd (testCallback.m_dp, testCallback, testJournal, &testLog);
		HANDLE hDoUpdate;

		cbacks.bFlags[Callbacks::getPrimaryIndex] = true;

		doUpdater do_upd(&upd, hDoUpdate);

		WaitForSingleObject (hDoUpdate, INFINITE);

		QC_BOOST_CHECK_MESSAGE ( NO_ERROR == do_upd.GetError(), L"Wrong return code\n" );


		QC_BOOST_CHECK_MESSAGE (_helper::DeleteFile(std::wstring(BASES_ROOT).append (L"\\base100.avc")),
			L"old base file not found");
		QC_BOOST_CHECK_MESSAGE (_helper::IsFoldersEqual(DIFFTOOL_ROOT,
			std::wstring(COLL_ROOT).append(L"\\tl-srv-w2k\\Bases_1\\plain").c_str(), BASES_ROOT), L"Incorrect bases\n");
		QC_BOOST_CHECK_MESSAGE (_helper::IsFoldersEqual(DIFFTOOL_ROOT,
			std::wstring(COLL_ROOT).append(L"\\tl-srv-w2k\\Bases_1\\data").c_str(), UPD_ROOT), L"Incorrect index\n");
	}
	
	{//Failed
		TestLog testLog;

		_helper::DeleteDirectory(ROOT);
		KLUPD::createFolder (BASES_ROOT, &testLog);
		KLUPD::createFolder (UPD_ROOT, &testLog);
		KLUPD::createFolder (PRODUCT_ROOT, &testLog);

		_helper::CopyFolder(std::wstring(COLL_ROOT).append(L"\\Bases_NULL\\plain").c_str(),BASES_ROOT);
		_helper::CloneFile (std::wstring(COLL_ROOT).append(L"\\Bases_NULL\\index\\6\\u0607g.xml").c_str(),
			std::wstring (UPD_ROOT).append (L"\\u0607g.xml"), false);

		UpdaterConfiguration data(1, 1, L"1");

		data.temporary_dir = TEMP_ROOT;

		data.m_pathSubstitutionMap.push_back(std::make_pair<NoCaseString,NoCaseString>(L"%BaseFolder%", BASES_ROOT));
		data.m_pathSubstitutionMap.push_back(std::make_pair<NoCaseString,NoCaseString>(L"%Folder%", ROLLBACK_ROOT));
		data.m_pathSubstitutionMap.push_back(std::make_pair<NoCaseString,NoCaseString>(L"%UpdateRoot%", UPD_ROOT));
		data.m_pathSubstitutionMap.push_back(std::make_pair<NoCaseString,NoCaseString>(L"%DSKM%", DSKM_ROOT));
		data.m_pathSubstitutionMap.push_back(std::make_pair<NoCaseString,NoCaseString>(L"%DataFolder%", UPD_ROOT));

		data.self_region = L"<default>";
		data.product_lang.push_back( NoCaseString(L"<default>"));

		data.os.os = L"Windows";
		data.os.version = L"5.1";

		data.m_componentsToUpdate.push_back (L"AVS");
		data.m_componentsToUpdate.push_back (L"CORE");
		data.connect_tmo = 30;

		data.m_sourceList.addSource (UST_UserURL, L"http://tl-srv-w2k", true, L"diffs");
		data.proxy_url = L"tl-prx-nta";
		data.proxy_port = 3128;

		data.m_proxyAuthorizationCredentials.userName (L"tester@kaspersky.ru");
		data.m_proxyAuthorizationCredentials.password (L"kf;0dsq Ntcnth");

		data.m_httpAuthorizationMethods.push_back (ntlmAuthorizationWithCredentials);
		data.UpdaterListFlags.update_bases = true;

		Callbacks cbacks;
		TestCallbackInterface testCallback (&testLog, &cbacks);
		TestJournalInterface testJournal;
		testCallback.SetUpdaterConfiguration (data);
		testCallback.SetPrimaryIndex( FileInfo (L"u0607g.xml", L"updater13/bases_1/index/6", FileInfo::index, true) );

		FileInfo u0607g ( L"u0607g.xml", L"updater13/bases_1/index/6", FileInfo::index, true );
		u0607g.m_localPath = UPD_ROOT;
		testCallback.SetPrimaryIndex(u0607g);

		Updater upd (testCallback.m_dp, testCallback, testJournal, &testLog);
		HANDLE hDoUpdate;

		cbacks.bFlags[Callbacks::getPrimaryIndex] = true;

		doUpdater do_upd(&upd, hDoUpdate);

		WaitForSingleObject (hDoUpdate, INFINITE);

		QC_BOOST_CHECK_MESSAGE ( CORE_PROXY_AUTH_ERROR == do_upd.GetError(), L"Wrong return code\n" );
	}
}


void TestSuite::TestFileExistsInTempFolder ()
{
	using namespace KLUPD;
	std::wcout<<L"TestFileExistsInTempFolder\n";
	Logger::Case _case (m_logger, m_testp, m_step, L"TestFileExistsInTempFolder");
	
	//base
	{
		std::wcout<<L"Base file\n";

		TestLog testLog;

		_helper::DeleteDirectory(ROOT);
		KLUPD::createFolder (BASES_ROOT, &testLog);
		KLUPD::createFolder (UPD_ROOT, &testLog);
		KLUPD::createFolder (TEMP_ROOT, &testLog);
		KLUPD::createFolder (wstring(TEMP_ROOT).append (L"\\temporaryFolder").c_str(), &testLog);

		_helper::CopyFolder(std::wstring(COLL_ROOT).append(L"\\Bases_1_basefile_missed\\missed").c_str(), wstring(TEMP_ROOT).append (L"\\temporaryFolder").c_str());

		UpdaterConfiguration data(1, 1, L"1");

		data.temporary_dir = TEMP_ROOT;
		//data.reserve_dir = RESERVE_ROOT;
		//data.update_root_folder = UPD_ROOT;

		data.m_pathSubstitutionMap.push_back(std::make_pair<NoCaseString,NoCaseString>(L"%BaseFolder%", BASES_ROOT));
		data.m_pathSubstitutionMap.push_back(std::make_pair<NoCaseString,NoCaseString>(L"%Folder%", ROLLBACK_ROOT));
		data.m_pathSubstitutionMap.push_back(std::make_pair<NoCaseString,NoCaseString>(L"%UpdateRoot%", UPD_ROOT));
		data.m_pathSubstitutionMap.push_back(std::make_pair<NoCaseString,NoCaseString>(L"%DSKM%", DSKM_ROOT));
		data.m_pathSubstitutionMap.push_back(std::make_pair<NoCaseString,NoCaseString>(L"%DataFolder%", UPD_ROOT));

		data.self_region = L"<default>";
		data.product_lang.push_back( NoCaseString(L"<default>"));

		data.os.os = L"Windows";
		data.os.version = L"5.1";

		data.m_componentsToUpdate.push_back ( L"AVS" );
		data.m_componentsToUpdate.push_back (NoCaseString(L"CORE") );
		data.connect_tmo = 30;

		data.m_sourceList.addSource (UST_UserURL, std::wstring(COLL_ROOT).append(L"\\Bases_1_basefile_missed").c_str(), true, L"diffs");
		data.UpdaterListFlags.update_bases = true;

		Callbacks cbacks;
		TestCallbackInterface testCallback (&testLog, &cbacks);
		TestJournalInterface testJournal;
		testCallback.SetUpdaterConfiguration (data);

		Updater upd (testCallback.m_dp, testCallback, testJournal, &testLog);
		HANDLE hDoUpdate;

		doUpdater do_upd(&upd, hDoUpdate);
	
		WaitForSingleObject (hDoUpdate, INFINITE);

		QC_BOOST_CHECK_MESSAGE (_helper::IsFoldersEqual(DIFFTOOL_ROOT,
			std::wstring(COLL_ROOT).append(L"\\Bases_1\\plain").c_str(), BASES_ROOT), L"Incorrect bases\n");

		QC_BOOST_CHECK_MESSAGE (_helper::IsFoldersEqual(DIFFTOOL_ROOT,
			std::wstring(COLL_ROOT).append(L"\\Bases_1\\data").c_str(), UPD_ROOT), L"Incorrect index\n");


		 ( NO_ERROR == do_upd.GetError(), L"Wrong return code\n" );
	}
	
	//secondary index (user's source)
	{
		std::wcout<<L"secondary index (user's source)\n";

		TestLog testLog;

		_helper::DeleteDirectory(ROOT);
		KLUPD::createFolder (BASES_ROOT, &testLog);
		KLUPD::createFolder (UPD_ROOT, &testLog);
		KLUPD::createFolder (TEMP_ROOT, &testLog);
		KLUPD::createFolder (wstring(TEMP_ROOT).append (L"\\temporaryFolder").c_str(), &testLog);

		_helper::CopyFolder(std::wstring(COLL_ROOT).append(L"\\Bases_1_second_index_missed\\missed").c_str(), wstring(TEMP_ROOT).append (L"\\temporaryFolder").c_str());

		UpdaterConfiguration data(1, 1, L"1");

		data.temporary_dir = TEMP_ROOT;
		//data.reserve_dir = RESERVE_ROOT;
		//data.update_root_folder = UPD_ROOT;

		data.m_pathSubstitutionMap.push_back(std::make_pair<NoCaseString,NoCaseString>(L"%BaseFolder%", BASES_ROOT));
		data.m_pathSubstitutionMap.push_back(std::make_pair<NoCaseString,NoCaseString>(L"%Folder%", ROLLBACK_ROOT));
		data.m_pathSubstitutionMap.push_back(std::make_pair<NoCaseString,NoCaseString>(L"%UpdateRoot%", UPD_ROOT));
		data.m_pathSubstitutionMap.push_back(std::make_pair<NoCaseString,NoCaseString>(L"%DSKM%", DSKM_ROOT));
		data.m_pathSubstitutionMap.push_back(std::make_pair<NoCaseString,NoCaseString>(L"%DataFolder%", UPD_ROOT));

		data.self_region = L"<default>";
		data.product_lang.push_back( NoCaseString(L"<default>"));

		data.os.os = L"Windows";
		data.os.version = L"5.1";

		data.m_componentsToUpdate.push_back ( L"AVS" );
		data.m_componentsToUpdate.push_back (NoCaseString(L"CORE") );
		data.connect_tmo = 30;

		data.m_sourceList.addSource (UST_UserURL, std::wstring(COLL_ROOT).append(L"\\Bases_1_second_index_missed").c_str(), true, L"diffs");
		data.UpdaterListFlags.update_bases = true;

		Callbacks cbacks;
		TestCallbackInterface testCallback (&testLog, &cbacks);
		TestJournalInterface testJournal;
		testCallback.SetUpdaterConfiguration (data);

		Updater upd (testCallback.m_dp, testCallback, testJournal, &testLog);
		HANDLE hDoUpdate;

		doUpdater do_upd(&upd, hDoUpdate);

		WaitForSingleObject (hDoUpdate, INFINITE);

		QC_BOOST_CHECK_MESSAGE (_helper::IsFoldersEqual(DIFFTOOL_ROOT,
			std::wstring(COLL_ROOT).append(L"\\Bases_1\\plain").c_str(), BASES_ROOT), L"Incorrect bases\n");

		QC_BOOST_CHECK_MESSAGE (_helper::IsFoldersEqual(DIFFTOOL_ROOT,
			std::wstring(COLL_ROOT).append(L"\\Bases_1\\data").c_str(), UPD_ROOT), L"Incorrect index\n");


		QC_BOOST_CHECK_MESSAGE ( NO_ERROR == do_upd.GetError(), L"Wrong return code\n" );
	}
	/*	QC_BOOST_CHECK_MESSAGE (false, L"Bug 23430\n");
	//secondary index (KL source)
	{
		std::wcout<<L"secondary index (KL source)\n";

		TestLog testLog;

		_helper::DeleteDirectory(ROOT);
		KLUPD::createFolder (BASES_ROOT, &testLog);
		KLUPD::createFolder (UPD_ROOT, &testLog);
		KLUPD::createFolder (TEMP_ROOT, &testLog);
		KLUPD::createFolder (wstring(TEMP_ROOT).append (L"\\temporaryFolder\\updater13\\bases_1_wo_si").c_str(), &testLog);

		_helper::CopyFolder(std::wstring(COLL_ROOT).append(L"\\Bases_1_second_index_missed_kl_source\\missed").c_str(),
			wstring(TEMP_ROOT).append (L"\\temporaryFolder\\updater13\\bases_1_wo_si").c_str());
		_helper::CloneFile(std::wstring(COLL_ROOT).append(L"\\Updcfgs\\updcfg_w2k_only.xml").c_str(),
							std::wstring (UPD_ROOT).append(L"\\updcfg.xml"), false);

		UpdaterConfiguration data(1, 1, L"1");

		data.temporary_dir = TEMP_ROOT;
		//data.reserve_dir = RESERVE_ROOT;
		//data.update_root_folder = UPD_ROOT;

		data.m_pathSubstitutionMap.push_back(std::make_pair<NoCaseString,NoCaseString>(L"%BaseFolder%", BASES_ROOT));
		data.m_pathSubstitutionMap.push_back(std::make_pair<NoCaseString,NoCaseString>(L"%Folder%", ROLLBACK_ROOT));
		data.m_pathSubstitutionMap.push_back(std::make_pair<NoCaseString,NoCaseString>(L"%UpdateRoot%", UPD_ROOT));
		data.m_pathSubstitutionMap.push_back(std::make_pair<NoCaseString,NoCaseString>(L"%DSKM%", DSKM_ROOT));
		data.m_pathSubstitutionMap.push_back(std::make_pair<NoCaseString,NoCaseString>(L"%DataFolder%", UPD_ROOT));

		data.self_region = L"<default>";
		data.product_lang.push_back( NoCaseString(L"<default>"));

		data.os.os = L"Windows";
		data.os.version = L"5.1";

		data.m_componentsToUpdate.push_back ( L"AVS" );
		data.m_componentsToUpdate.push_back (NoCaseString(L"CORE") );
		data.connect_tmo = 30;

		data.m_sourceList.addSource (UST_KLServer, L"updater13/bases_1_wo_si/index/6", false, L"diffs");
		data.UpdaterListFlags.update_bases = true;

		Callbacks cbacks;
		TestCallbackInterface testCallback (&testLog, &cbacks);
		TestJournalInterface testJournal;
		testCallback.SetUpdaterConfiguration (data);

		Updater upd (testCallback.m_dp, testCallback, testJournal, &testLog);
		HANDLE hDoUpdate;

		cbacks.bFlags[Callbacks::getUpdaterConfigurationXml] = true;
		FileInfo updcfg ( L"updcfg.xml", L"updater13/bases_1_wo_si/index/6", FileInfo::base, false );
		updcfg.m_localPath = UPD_ROOT;
		testCallback.SetUpdcfgXml(updcfg);
		
		cbacks.bFlags[Callbacks::getPrimaryIndex] = true;
		FileInfo u0607g ( L"u0607g.xml", L"updater13/bases_1_wo_si/index/6", FileInfo::index, true );
		u0607g.m_localPath = UPD_ROOT;
		testCallback.SetPrimaryIndex(u0607g);

		doUpdater do_upd(&upd, hDoUpdate);

		WaitForSingleObject (hDoUpdate, INFINITE);

		QC_BOOST_CHECK_MESSAGE (_helper::IsFoldersEqual(DIFFTOOL_ROOT,
			std::wstring(COLL_ROOT).append(L"\\Bases_1\\plain").c_str(), BASES_ROOT), L"Incorrect bases\n");

		QC_BOOST_CHECK_MESSAGE (_helper::IsFoldersEqual(DIFFTOOL_ROOT,
			std::wstring(COLL_ROOT).append(L"\\Bases_1\\data").c_str(), UPD_ROOT), L"Incorrect index\n");

		QC_BOOST_CHECK_MESSAGE ( NO_ERROR == do_upd.GetError(), L"Wrong return code\n" );
	}
	*/
}

void TestSuite::TestFileExistsInTargetFolder ()
{
	using namespace KLUPD;
	std::wcout<<L"TestFileExistsInTargetFolder\n";
	Logger::Case _case (m_logger, m_testp, m_step, L"TestFileExistsInTargetFolder");
	
	//base
	{
		std::wcout<<L"Base file\n";

		TestLog testLog;

		_helper::DeleteDirectory(ROOT);
		KLUPD::createFolder (BASES_ROOT, &testLog);
		KLUPD::createFolder (UPD_ROOT, &testLog);
		KLUPD::createFolder (TEMP_ROOT, &testLog);

		_helper::CopyFolder(std::wstring(COLL_ROOT).append(L"\\Bases_1_basefile_missed\\missed\\bases\\av\\avc\\i386").c_str(),
							wstring(BASES_ROOT).c_str());

		UpdaterConfiguration data(1, 1, L"1");

		data.temporary_dir = TEMP_ROOT;
		//data.reserve_dir = RESERVE_ROOT;
		//data.update_root_folder = UPD_ROOT;

		data.m_pathSubstitutionMap.push_back(std::make_pair<NoCaseString,NoCaseString>(L"%BaseFolder%", BASES_ROOT));
		data.m_pathSubstitutionMap.push_back(std::make_pair<NoCaseString,NoCaseString>(L"%Folder%", ROLLBACK_ROOT));
		data.m_pathSubstitutionMap.push_back(std::make_pair<NoCaseString,NoCaseString>(L"%UpdateRoot%", UPD_ROOT));
		data.m_pathSubstitutionMap.push_back(std::make_pair<NoCaseString,NoCaseString>(L"%DSKM%", DSKM_ROOT));
		data.m_pathSubstitutionMap.push_back(std::make_pair<NoCaseString,NoCaseString>(L"%DataFolder%", UPD_ROOT));

		data.self_region = L"<default>";
		data.product_lang.push_back( NoCaseString(L"<default>"));

		data.os.os = L"Windows";
		data.os.version = L"5.1";

		data.m_componentsToUpdate.push_back ( L"AVS" );
		data.m_componentsToUpdate.push_back (NoCaseString(L"CORE") );
		data.connect_tmo = 30;

		data.m_sourceList.addSource (UST_UserURL, std::wstring(COLL_ROOT).append(L"\\Bases_1_basefile_missed").c_str(), true, L"diffs");
		data.UpdaterListFlags.update_bases = true;

		Callbacks cbacks;
		TestCallbackInterface testCallback (&testLog, &cbacks);
		TestJournalInterface testJournal;
		testCallback.SetUpdaterConfiguration (data);

		Updater upd (testCallback.m_dp, testCallback, testJournal, &testLog);
		HANDLE hDoUpdate;

		doUpdater do_upd(&upd, hDoUpdate);

		WaitForSingleObject (hDoUpdate, INFINITE);

		QC_BOOST_CHECK_MESSAGE (_helper::IsFoldersEqual(DIFFTOOL_ROOT,
		std::wstring(COLL_ROOT).append(L"\\Bases_1\\plain").c_str(), BASES_ROOT), L"Incorrect bases\n");

		QC_BOOST_CHECK_MESSAGE (_helper::IsFoldersEqual(DIFFTOOL_ROOT,
		std::wstring(COLL_ROOT).append(L"\\Bases_1\\data").c_str(), UPD_ROOT), L"Incorrect index\n");


		QC_BOOST_CHECK_MESSAGE ( NO_ERROR == do_upd.GetError(), L"Wrong return code\n" );
	}


	//secondary index (user's source)
	{
		std::wcout<<L"secondary index (user's source)\n";

		TestLog testLog;

		_helper::DeleteDirectory(ROOT);
		KLUPD::createFolder (BASES_ROOT, &testLog);
		KLUPD::createFolder (UPD_ROOT, &testLog);
		KLUPD::createFolder (TEMP_ROOT, &testLog);

		_helper::CopyFolder(std::wstring(COLL_ROOT).append(L"\\Bases_1_second_index_missed\\missed\\bases\\av\\avc\\i386").c_str(),
			wstring(BASES_ROOT).c_str());

		UpdaterConfiguration data(1, 1, L"1");

		data.temporary_dir = TEMP_ROOT;
		//data.reserve_dir = RESERVE_ROOT;
		//data.update_root_folder = UPD_ROOT;

		data.m_pathSubstitutionMap.push_back(std::make_pair<NoCaseString,NoCaseString>(L"%BaseFolder%", BASES_ROOT));
		data.m_pathSubstitutionMap.push_back(std::make_pair<NoCaseString,NoCaseString>(L"%Folder%", ROLLBACK_ROOT));
		data.m_pathSubstitutionMap.push_back(std::make_pair<NoCaseString,NoCaseString>(L"%UpdateRoot%", UPD_ROOT));
		data.m_pathSubstitutionMap.push_back(std::make_pair<NoCaseString,NoCaseString>(L"%DSKM%", DSKM_ROOT));
		data.m_pathSubstitutionMap.push_back(std::make_pair<NoCaseString,NoCaseString>(L"%DataFolder%", UPD_ROOT));

		data.self_region = L"<default>";
		data.product_lang.push_back( NoCaseString(L"<default>"));

		data.os.os = L"Windows";
		data.os.version = L"5.1";

		data.m_componentsToUpdate.push_back ( L"AVS" );
		data.m_componentsToUpdate.push_back (NoCaseString(L"CORE") );
		data.connect_tmo = 30;

		data.m_sourceList.addSource (UST_UserURL, std::wstring(COLL_ROOT).append(L"\\Bases_1_second_index_missed").c_str(), true, L"diffs");
		data.UpdaterListFlags.update_bases = true;

		Callbacks cbacks;
		TestCallbackInterface testCallback (&testLog, &cbacks);
		TestJournalInterface testJournal;
		testCallback.SetUpdaterConfiguration (data);

		Updater upd (testCallback.m_dp, testCallback, testJournal, &testLog);
		HANDLE hDoUpdate;

		doUpdater do_upd(&upd, hDoUpdate);

		WaitForSingleObject (hDoUpdate, INFINITE);

		QC_BOOST_CHECK_MESSAGE (_helper::IsFoldersEqual(DIFFTOOL_ROOT,
		std::wstring(COLL_ROOT).append(L"\\Bases_1\\plain").c_str(), BASES_ROOT), L"Incorrect bases\n");

		QC_BOOST_CHECK_MESSAGE (_helper::IsFoldersEqual(DIFFTOOL_ROOT,
		std::wstring(COLL_ROOT).append(L"\\Bases_1\\data").c_str(), UPD_ROOT), L"Incorrect index\n");


		QC_BOOST_CHECK_MESSAGE ( NO_ERROR == do_upd.GetError(), L"Wrong return code\n" );
	}

	/*QC_BOOST_CHECK_MESSAGE (false, L"Bug 23430\n");
	//secondary index (KL source)
	{
		std::wcout<<L"secondary index (KL source)\n";

		TestLog testLog;

		_helper::DeleteDirectory(ROOT);
		KLUPD::createFolder (BASES_ROOT, &testLog);
		KLUPD::createFolder (UPD_ROOT, &testLog);
		KLUPD::createFolder (TEMP_ROOT, &testLog);
		//KLUPD::createFolder (wstring(TEMP_ROOT).append (L"\\temporaryFolder\\updater13\\bases_1_wo_si").c_str(), &testLog);

		_helper::CopyFolder(std::wstring(COLL_ROOT).append(L"\\Bases_1_second_index_missed\\missed\\bases\\av\\avc\\i386").c_str(),
						    wstring(BASES_ROOT).c_str());
		_helper::CloneFile(std::wstring(COLL_ROOT).append(L"\\Updcfgs\\updcfg_w2k_only.xml").c_str(),
			std::wstring (UPD_ROOT).append(L"\\updcfg.xml"), false);

		UpdaterConfiguration data(1, 1, L"1");

		data.temporary_dir = TEMP_ROOT;
		//data.reserve_dir = RESERVE_ROOT;
		//data.update_root_folder = UPD_ROOT;

		data.m_pathSubstitutionMap.push_back(std::make_pair<NoCaseString,NoCaseString>(L"%BaseFolder%", BASES_ROOT));
		data.m_pathSubstitutionMap.push_back(std::make_pair<NoCaseString,NoCaseString>(L"%Folder%", ROLLBACK_ROOT));
		data.m_pathSubstitutionMap.push_back(std::make_pair<NoCaseString,NoCaseString>(L"%UpdateRoot%", UPD_ROOT));
		data.m_pathSubstitutionMap.push_back(std::make_pair<NoCaseString,NoCaseString>(L"%DSKM%", DSKM_ROOT));
		data.m_pathSubstitutionMap.push_back(std::make_pair<NoCaseString,NoCaseString>(L"%DataFolder%", UPD_ROOT));

		data.self_region = L"<default>";
		data.product_lang.push_back( NoCaseString(L"<default>"));

		data.os.os = L"Windows";
		data.os.version = L"5.1";

		data.m_componentsToUpdate.push_back ( L"AVS" );
		data.m_componentsToUpdate.push_back (NoCaseString(L"CORE") );
		data.connect_tmo = 30;

		data.m_sourceList.addSource (UST_KLServer, L"updater13/bases_1_wo_si/index/6", false, L"diffs");
		data.UpdaterListFlags.update_bases = true;

		Callbacks cbacks;
		TestCallbackInterface testCallback (&testLog, &cbacks);
		TestJournalInterface testJournal;
		testCallback.SetUpdaterConfiguration (data);

		Updater upd (testCallback.m_dp, testCallback, testJournal, &testLog);
		HANDLE hDoUpdate;

		cbacks.bFlags[Callbacks::getUpdaterConfigurationXml] = true;
		FileInfo updcfg ( L"updcfg.xml", L"updater13/bases_1_wo_si/index/6", FileInfo::base, false );
		updcfg.m_localPath = UPD_ROOT;
		testCallback.SetUpdcfgXml(updcfg);

		cbacks.bFlags[Callbacks::getPrimaryIndex] = true;
		FileInfo u0607g ( L"u0607g.xml", L"updater13/bases_1_wo_si/index/6", FileInfo::index, true );
		u0607g.m_localPath = UPD_ROOT;
		testCallback.SetPrimaryIndex(u0607g);

		doUpdater do_upd(&upd, hDoUpdate);

		WaitForSingleObject (hDoUpdate, INFINITE);

		QC_BOOST_CHECK_MESSAGE (_helper::IsFoldersEqual(DIFFTOOL_ROOT,
			std::wstring(COLL_ROOT).append(L"\\Bases_1\\plain").c_str(), BASES_ROOT), L"Incorrect bases\n");

		QC_BOOST_CHECK_MESSAGE (_helper::IsFoldersEqual(DIFFTOOL_ROOT,
			std::wstring(COLL_ROOT).append(L"\\Bases_1\\data").c_str(), UPD_ROOT), L"Incorrect index\n");

		QC_BOOST_CHECK_MESSAGE ( NO_ERROR == do_upd.GetError(), L"Wrong return code\n" );
	}
	*/
}

void TestSuite::TestFileExistsInTempTargetFolder ()
{
	using namespace KLUPD;
	std::wcout<<L"TestFileExistsInTempTargetFolder\n";
	Logger::Case _case (m_logger, m_testp, m_step, L"TestFileExistsInTempTargetFolder");
	
	//base
	{
		std::wcout<<L"Base file 1\n";

		TestLog testLog;

		_helper::DeleteDirectory(ROOT);
		KLUPD::createFolder (BASES_ROOT, &testLog);
		KLUPD::createFolder (UPD_ROOT, &testLog);
		KLUPD::createFolder (TEMP_ROOT, &testLog);
		KLUPD::createFolder (wstring(TEMP_ROOT).append (L"\\temporaryFolder\\bases\\av\\avc\\i386").c_str(), &testLog);

		_helper::CloneFile(std::wstring(COLL_ROOT).append(L"\\Bases_1_basefile_missed\\diff\\base102.avc_old").c_str(), wstring(TEMP_ROOT).append (L"\\temporaryFolder\\bases\\av\\avc\\i386\\base102.avc").c_str(), false);
		_helper::CloneFile(std::wstring(COLL_ROOT).append(L"\\Bases_1_basefile_missed\\diff\\base102.avc_new").c_str(), wstring(BASES_ROOT).append (L"\\base102.avc").c_str(), false);

		UpdaterConfiguration data(1, 1, L"1");

		data.temporary_dir = TEMP_ROOT;
		//data.reserve_dir = RESERVE_ROOT;
		//data.update_root_folder = UPD_ROOT;

		data.m_pathSubstitutionMap.push_back(std::make_pair<NoCaseString,NoCaseString>(L"%BaseFolder%", BASES_ROOT));
		data.m_pathSubstitutionMap.push_back(std::make_pair<NoCaseString,NoCaseString>(L"%Folder%", ROLLBACK_ROOT));
		data.m_pathSubstitutionMap.push_back(std::make_pair<NoCaseString,NoCaseString>(L"%UpdateRoot%", UPD_ROOT));
		data.m_pathSubstitutionMap.push_back(std::make_pair<NoCaseString,NoCaseString>(L"%DSKM%", DSKM_ROOT));
		data.m_pathSubstitutionMap.push_back(std::make_pair<NoCaseString,NoCaseString>(L"%DataFolder%", UPD_ROOT));

		data.self_region = L"<default>";
		data.product_lang.push_back( NoCaseString(L"<default>"));

		data.os.os = L"Windows";
		data.os.version = L"5.1";

		data.m_componentsToUpdate.push_back ( L"AVS" );
		data.m_componentsToUpdate.push_back (NoCaseString(L"CORE") );
		data.connect_tmo = 30;

		data.m_sourceList.addSource (UST_UserURL, std::wstring(COLL_ROOT).append(L"\\Bases_1_basefile_missed").c_str(), true, L"diffs");
		data.UpdaterListFlags.update_bases = true;

		Callbacks cbacks;
		TestCallbackInterface testCallback (&testLog, &cbacks);
		TestJournalInterface testJournal;
		testCallback.SetUpdaterConfiguration (data);

		Updater upd (testCallback.m_dp, testCallback, testJournal, &testLog);
		HANDLE hDoUpdate;

		doUpdater do_upd(&upd, hDoUpdate);

		WaitForSingleObject (hDoUpdate, INFINITE);

		QC_BOOST_CHECK_MESSAGE (_helper::IsFoldersEqual(DIFFTOOL_ROOT,
		std::wstring(COLL_ROOT).append(L"\\Bases_1\\plain").c_str(), BASES_ROOT), L"Incorrect bases\n");

		QC_BOOST_CHECK_MESSAGE (_helper::IsFoldersEqual(DIFFTOOL_ROOT,
		std::wstring(COLL_ROOT).append(L"\\Bases_1\\data").c_str(), UPD_ROOT), L"Incorrect index\n");

		QC_BOOST_CHECK_MESSAGE ( NO_ERROR == do_upd.GetError(), L"Wrong return code\n" );
	}
	{
		std::wcout<<L"Base file 2\n";

		TestLog testLog;

		_helper::DeleteDirectory(ROOT);
		KLUPD::createFolder (BASES_ROOT, &testLog);
		KLUPD::createFolder (UPD_ROOT, &testLog);
		KLUPD::createFolder (TEMP_ROOT, &testLog);
		KLUPD::createFolder (wstring(TEMP_ROOT).append (L"\\temporaryFolder\\bases\\av\\avc\\i386").c_str(), &testLog);

		_helper::CloneFile(std::wstring(COLL_ROOT).append(L"\\Bases_1_basefile_missed\\diff\\base102.avc_new").c_str(), wstring(TEMP_ROOT).append (L"\\temporaryFolder\\bases\\av\\avc\\i386\\base102.avc").c_str(), false);
		_helper::CloneFile(std::wstring(COLL_ROOT).append(L"\\Bases_1_basefile_missed\\diff\\base102.avc_old").c_str(), wstring(BASES_ROOT).append (L"\\base102.avc").c_str(), false);

		UpdaterConfiguration data(1, 1, L"1");

		data.temporary_dir = TEMP_ROOT;
		//data.reserve_dir = RESERVE_ROOT;
		//data.update_root_folder = UPD_ROOT;

		data.m_pathSubstitutionMap.push_back(std::make_pair<NoCaseString,NoCaseString>(L"%BaseFolder%", BASES_ROOT));
		data.m_pathSubstitutionMap.push_back(std::make_pair<NoCaseString,NoCaseString>(L"%Folder%", ROLLBACK_ROOT));
		data.m_pathSubstitutionMap.push_back(std::make_pair<NoCaseString,NoCaseString>(L"%UpdateRoot%", UPD_ROOT));
		data.m_pathSubstitutionMap.push_back(std::make_pair<NoCaseString,NoCaseString>(L"%DSKM%", DSKM_ROOT));
		data.m_pathSubstitutionMap.push_back(std::make_pair<NoCaseString,NoCaseString>(L"%DataFolder%", UPD_ROOT));

		data.self_region = L"<default>";
		data.product_lang.push_back( NoCaseString(L"<default>"));

		data.os.os = L"Windows";
		data.os.version = L"5.1";

		data.m_componentsToUpdate.push_back ( L"AVS" );
		data.m_componentsToUpdate.push_back (NoCaseString(L"CORE") );
		data.connect_tmo = 30;

		data.m_sourceList.addSource (UST_UserURL, std::wstring(COLL_ROOT).append(L"\\Bases_1_basefile_missed").c_str(), true, L"diffs");
		data.UpdaterListFlags.update_bases = true;

		Callbacks cbacks;
		TestCallbackInterface testCallback (&testLog, &cbacks);
		TestJournalInterface testJournal;
		testCallback.SetUpdaterConfiguration (data);

		Updater upd (testCallback.m_dp, testCallback, testJournal, &testLog);
		HANDLE hDoUpdate;

		doUpdater do_upd(&upd, hDoUpdate);

		WaitForSingleObject (hDoUpdate, INFINITE);

		QC_BOOST_CHECK_MESSAGE (_helper::IsFoldersEqual(DIFFTOOL_ROOT,
			std::wstring(COLL_ROOT).append(L"\\Bases_1\\plain").c_str(), BASES_ROOT), L"Incorrect bases\n");

		QC_BOOST_CHECK_MESSAGE (_helper::IsFoldersEqual(DIFFTOOL_ROOT,
			std::wstring(COLL_ROOT).append(L"\\Bases_1\\data").c_str(), UPD_ROOT), L"Incorrect index\n");

		QC_BOOST_CHECK_MESSAGE ( NO_ERROR == do_upd.GetError(), L"Wrong return code\n" );
	}
}

void TestSuite::TestSwitchingURLs ()
{
	using namespace KLUPD;
	std::wcout<<L"TestSwitchingURLs\n";
	Logger::Case _case (m_logger, m_testp, m_step, L"TestSwitchingURLs");

	{
		std::wcout<<L"\n";

		TestLog testLog;

		_helper::DeleteDirectory(ROOT);
		KLUPD::createFolder (BASES_ROOT, &testLog);
		KLUPD::createFolder (UPD_ROOT, &testLog);

		_helper::CopyFolder(std::wstring(COLL_ROOT).append(L"\\Bases_NULL\\plain").c_str(),BASES_ROOT);
		_helper::CloneFile (std::wstring(COLL_ROOT).append(L"\\Bases_NULL\\index\\6\\u0607g.xml").c_str(),
			std::wstring (UPD_ROOT).append (L"\\u0607g.xml"), false);

		UpdaterConfiguration data(1, 1, L"1");

		data.temporary_dir = TEMP_ROOT;
		//data.reserve_dir = RESERVE_ROOT;
		//data.update_root_folder = UPD_ROOT;

		data.m_pathSubstitutionMap.push_back(std::make_pair<NoCaseString,NoCaseString>(L"%BaseFolder%", BASES_ROOT));
		data.m_pathSubstitutionMap.push_back(std::make_pair<NoCaseString,NoCaseString>(L"%Folder%", ROLLBACK_ROOT));
		data.m_pathSubstitutionMap.push_back(std::make_pair<NoCaseString,NoCaseString>(L"%UpdateRoot%", UPD_ROOT));
		data.m_pathSubstitutionMap.push_back(std::make_pair<NoCaseString,NoCaseString>(L"%DSKM%", DSKM_ROOT));
		data.m_pathSubstitutionMap.push_back(std::make_pair<NoCaseString,NoCaseString>(L"%DataFolder%", UPD_ROOT));

		data.self_region = L"<default>";
		data.product_lang.push_back( NoCaseString(L"<default>"));

		data.os.os = L"Windows";
		data.os.version = L"5.1";

		data.m_componentsToUpdate.push_back ( L"AVS" );
		data.m_componentsToUpdate.push_back (NoCaseString(L"CORE") );
		data.connect_tmo = 30;

		data.m_sourceList.addSource (UST_UserURL, std::wstring(COLL_ROOT).append(L"\\Bases_fake").c_str(), true, L"diffs");
		data.m_sourceList.addSource (UST_UserURL, std::wstring(COLL_ROOT).append(L"\\Bases_ne").c_str(), true, L"diffs");
		data.m_sourceList.addSource (UST_UserURL, std::wstring(COLL_ROOT).append(L"\\Bases_1").c_str(), true, L"diffs");

		data.UpdaterListFlags.update_bases = true;

		Callbacks cbacks;
		TestCallbackInterface testCallback (&testLog, &cbacks);
		TestJournalInterface testJournal;
		testCallback.SetUpdaterConfiguration (data);

		Updater upd (testCallback.m_dp, testCallback, testJournal, &testLog);
		HANDLE hDoUpdate;

		doUpdater do_upd(&upd, hDoUpdate);

		WaitForSingleObject (hDoUpdate, INFINITE);

		QC_BOOST_CHECK_MESSAGE (_helper::DeleteFile(std::wstring(BASES_ROOT).append (L"\\base100.avc")),
			L"old base file not found");
		QC_BOOST_CHECK_MESSAGE (_helper::IsFoldersEqual(DIFFTOOL_ROOT,
			std::wstring(COLL_ROOT).append(L"\\Bases_1\\plain").c_str(), BASES_ROOT), L"Incorrect bases\n");

		QC_BOOST_CHECK_MESSAGE (_helper::IsFoldersEqual(DIFFTOOL_ROOT,
			std::wstring(COLL_ROOT).append(L"\\Bases_1\\data").c_str(), UPD_ROOT), L"Incorrect index\n");


		QC_BOOST_CHECK_MESSAGE ( NO_ERROR == do_upd.GetError(), L"Wrong return code\n" );
	}
}

void TestSuite::TestUnableToInit ()
{
	using namespace KLUPD;
	std::wcout<<L"TestUnableToInit\n";
	Logger::Case _case (m_logger, m_testp, m_step, L"TestUnableToInit");

	{
		TestLog testLog;

		UpdaterConfiguration data(1, 1, L"1");

		data.temporary_dir = L"b:\\upd\\root";

		data.self_region = L"<default>";
		data.product_lang.push_back( NoCaseString(L"<default>"));

		data.os.os = L"Windows";
		data.os.version = L"5.1";

		data.m_componentsToUpdate.push_back ( L"AVS" );
		data.m_componentsToUpdate.push_back (NoCaseString(L"CORE") );
		data.connect_tmo = 30;

		data.m_sourceList.addSource (UST_UserURL, std::wstring(COLL_ROOT).append(L"\\Bases_1").c_str(), true, L"diffs");

		data.UpdaterListFlags.update_bases = true;

		Callbacks cbacks;
		TestCallbackInterface testCallback (&testLog, &cbacks);
		TestJournalInterface testJournal;
		testCallback.SetUpdaterConfiguration (data);

		Updater upd (testCallback.m_dp, testCallback, testJournal, &testLog);
		HANDLE hDoUpdate;

		doUpdater do_upd(&upd, hDoUpdate);

		WaitForSingleObject (hDoUpdate, INFINITE);

		QC_BOOST_CHECK_MESSAGE ( CORE_FailedToCreateFolder == do_upd.GetError(), L"Wrong return code\n" );
	}
}

void TestSuite::TestSwitchingURLsWhileDownloading ()
{
	std::wcout<<L"TestSwitchingURLsWhileDownloading\n";
	Logger::Case _case (m_logger, m_testp, m_step, L"TestSwitchingURLsWhileDownloading");
	
	//TestSwitchingURLsWhileDownloading_Implementation (TRANSFER_ERROR);
	TestSwitchingURLsWhileDownloading_Implementation (INTERNAL_ERROR_500);
}

void TestSuite::EmulateNetError (HttpServer& hs, const NetError& error, const wstring& filename, const string& reqname, ...)
{
	switch (error)
	{
		case TRANSFER_ERROR:
			{
				string command;
				string param;
				HttpServer::CMD_TYPE type;

				hs.Accept();
				hs.Receive(command, type, param);
				QC_BOOST_CHECK_MESSAGE (type == HttpServer::CMD_GET, L"Error\n");
				_ASSERTE (command.find (reqname)!= wstring::npos);
				hs.SendFilePartly (filename);
				hs.CloseConnection ();
			}
		break;
		case INTERNAL_ERROR_500:
			{
				string command;
				string param;
				HttpServer::CMD_TYPE type;

				hs.Accept();
				hs.Receive(command, type, param);
				QC_BOOST_CHECK_MESSAGE (type == HttpServer::CMD_GET, L"Error\n");
				_ASSERTE (command.find (reqname)!= wstring::npos);
				hs.Send("HTTP/1.1 500 Internal error\x0d\x0a");
				hs.Send("Content-Length: 0\x0d\x0a");
				hs.Send("Connection: close\x0d\x0a");
				hs.Send("\x0d\x0a");
				hs.CloseConnection ();
			}
		break;
		case TIMEOUT_EXCEEDS:
		{
			string command;
			string param;
			HttpServer::CMD_TYPE type;

			hs.Accept();
			hs.Receive(command, type, param);
			QC_BOOST_CHECK_MESSAGE (type == HttpServer::CMD_GET, L"Error\n");
			_ASSERTE (command.find (reqname)!= wstring::npos);
			hs.CheckConnectionClosed (INFINITE);
		}
		break;
	}
}

void TestSuite::TestSwitchingURLsWhileDownloading_Implementation (const NetError& reason)
{
	#define CLOSE_CONNECTION false, false, false

	#define DO_ERROR(SERVER,FILENAME,REQNAME)\
	switch (reason)\
	{\
	case TRANSFER_ERROR: case INTERNAL_ERROR_500:\
		EmulateNetError(SERVER,reason,FILENAME,REQNAME);\
		break;\
	}

	std::wcout<<L"TestSwitchingURLsWhileDownloading_Implementation code="<<reason<<L"\n";

	{
		TestLog testLog;
		HttpServer hs (8001, "127.0.0.1");
		hs.Start();
		HttpServer hs2 (8002, "127.0.0.1");
		hs2.Start();
		HttpServer hs3 (8003, "127.0.0.1");
		hs3.Start();
		HttpServer hs4 (8004, "127.0.0.1");
		hs4.Start();
		HttpServer hs5 (8005, "127.0.0.1");
		hs5.Start();

		_helper::DeleteDirectory(ROOT);
		KLUPD::createFolder (BASES_ROOT, &testLog);
		KLUPD::createFolder (UPD_ROOT, &testLog);

		_helper::CopyFolder(std::wstring(COLL_ROOT).append(L"\\Bases_NULL_FOR_DNLD\\plain").c_str(),BASES_ROOT);
		_helper::CloneFile (std::wstring(COLL_ROOT).append(L"\\Bases_NULL_FOR_DNLD\\index\\6\\u0607g.xml").c_str(),
			std::wstring (UPD_ROOT).append (L"\\u0607g.xml"), false);

		UpdaterConfiguration data(1, 1, L"1");

		data.temporary_dir = TEMP_ROOT;

		data.m_pathSubstitutionMap.push_back(std::make_pair<NoCaseString,NoCaseString>(L"%BaseFolder%", BASES_ROOT));
		data.m_pathSubstitutionMap.push_back(std::make_pair<NoCaseString,NoCaseString>(L"%Folder%", ROLLBACK_ROOT));
		data.m_pathSubstitutionMap.push_back(std::make_pair<NoCaseString,NoCaseString>(L"%UpdateRoot%", UPD_ROOT));
		data.m_pathSubstitutionMap.push_back(std::make_pair<NoCaseString,NoCaseString>(L"%DSKM%", DSKM_ROOT));
		data.m_pathSubstitutionMap.push_back(std::make_pair<NoCaseString,NoCaseString>(L"%DataFolder%", UPD_ROOT));

		data.self_region = L"<default>";
		data.product_lang.push_back( NoCaseString(L"<default>"));

		data.os.os = L"Windows";
		data.os.version = L"5.1";

		data.m_componentsToUpdate.push_back ( L"AVS" );
		data.m_componentsToUpdate.push_back ( L"CORE" ); //внесены коррективы в тест для блеклиста
		data.connect_tmo = 10;

		data.m_sourceList.addSource (UST_UserURL, L"http://127.0.0.1:8001", false, L"diffs");
		data.m_sourceList.addSource (UST_UserURL, L"http://127.0.0.1:8002", false, L"diffs");
		data.m_sourceList.addSource (UST_UserURL, L"http://127.0.0.1:8003", false, L"diffs");
		data.m_sourceList.addSource (UST_UserURL, L"http://127.0.0.1:8004", false, L"diffs");
		data.m_sourceList.addSource (UST_UserURL, L"http://127.0.0.1:8005", false, L"diffs");

		data.UpdaterListFlags.update_bases = true;

		Callbacks cbacks;
		TestCallbackInterface testCallback (&testLog, &cbacks);
		TestJournalInterface testJournal;
		testCallback.SetUpdaterConfiguration (data);

		Updater upd (testCallback.m_dp, testCallback, testJournal, &testLog);
		HANDLE hDoUpdate;

		doUpdater do_upd(&upd, hDoUpdate);
		
		//при ошибке на diff и klz продолжает работу с текущим источником (1)
		//diff
		DO_ERROR(hs,std::wstring(COLL_ROOT).append(L"\\Bases_1\\data\\u0607g.xml"),"u0607g.xml.dif")
		//klz
		DO_ERROR(hs,std::wstring(COLL_ROOT).append(L"\\Bases_1\\data\\u0607g.xml"),"u0607g.xml.klz")
		//xml
		DO_ERROR(hs,std::wstring(COLL_ROOT).append(L"\\Bases_1\\data\\u0607g.xml"),"u0607g.xml")

		//после ошибки на главном индексе переключается на следующий источник (2)
		
		//diff
		DO_ERROR(hs2,std::wstring(COLL_ROOT).append(L"\\Bases_1\\data\\u0607g.xml"),"u0607g.xml.dif")
		//klz
		DO_ERROR(hs2,std::wstring(COLL_ROOT).append(L"\\Bases_1\\data\\u0607g.xml"),"u0607g.xml.klz")

		//xml
		TransferFile(hs2, std::wstring(COLL_ROOT).append(L"\\Bases_1\\data\\u0607g.xml"), "u0607g.xml");

		//дали скачать главный индекс, повторяем ситуацию на вторичном
		//при ошибке на diff-е и klz используется тот же источник
		//diff
		DO_ERROR(hs2,std::wstring(COLL_ROOT).append(L"\\Bases_1\\data\\av-i386-0607g.xml"),"av-i386-0607g.xml.dif")
		//klz
		DO_ERROR(hs2,std::wstring(COLL_ROOT).append(L"\\Bases_1\\data\\av-i386-0607g.xml"),"av-i386-0607g.xml.klz")
		//xml
		DO_ERROR(hs2,std::wstring(COLL_ROOT).append(L"\\Bases_1\\data\\av-i386-0607g.xml"),"av-i386-0607g.xml")

		//переключаемся на третий источник, все начнется заново со скачивания главного индекса

		//diff
		DO_ERROR(hs3,std::wstring(COLL_ROOT).append(L"\\Bases_1\\data\\u0607g.xml"),"u0607g.xml.dif")
		//klz
		DO_ERROR(hs3,std::wstring(COLL_ROOT).append(L"\\Bases_1\\data\\u0607g.xml"),"u0607g.xml.klz")

		//xml
		TransferFile(hs3, std::wstring(COLL_ROOT).append(L"\\Bases_1\\data\\u0607g.xml"), "u0607g.xml");

		//дадим скачать вторичный индекс

		//diff
		DO_ERROR(hs3,std::wstring(COLL_ROOT).append(L"\\Bases_1\\data\\av-i386-0607g.xml"),"av-i386-0607g.xml.dif")
		//klz
		DO_ERROR(hs3,std::wstring(COLL_ROOT).append(L"\\Bases_1\\data\\av-i386-0607g.xml"),"av-i386-0607g.xml.klz")

		//xml
		TransferFile(hs3, std::wstring(COLL_ROOT).append(L"\\Bases_1\\plain\\av-i386-0607g.xml"), "av-i386-0607g.xml");

		//начинаем скачивать базы

	TransferFile(hs3, std::wstring(COLL_ROOT).append(L"\\Bases_1\\plain\\blst-0607g.xml"), "blst-0607g.xml.dif");
	TransferFile(hs3, std::wstring(COLL_ROOT).append(L"\\Bases_1\\plain\\blst-0607g.xml"), "blst-0607g.xml.klz");
	TransferFile(hs3, std::wstring(COLL_ROOT).append(L"\\Bases_1\\plain\\blst-0607g.xml"), "blst-0607g.xml");
	TransferFile(hs3, std::wstring(COLL_ROOT).append(L"\\Bases_1\\plain\\black.lst"), "black.lst.");
	TransferFile(hs3, std::wstring(COLL_ROOT).append(L"\\Bases_1\\plain\\black.lst"), "black.lst");

		//avc.diff
		DO_ERROR(hs3,std::wstring(COLL_ROOT).append(L"\\Bases_1\\data\\base102.avc"),"base102.avc.")
		//avc
		DO_ERROR(hs3,std::wstring(COLL_ROOT).append(L"\\Bases_1\\data\\base102.avc"),"base102.avc")

		//ошибка при скачивании целого файла баз, переключаемся на следующий источник
		//начинаем с главного индекса

		//diff
		DO_ERROR(hs4,std::wstring(COLL_ROOT).append(L"\\Bases_1\\data\\u0607g.xml"),"u0607g.xml.dif")
		//klz
		DO_ERROR(hs4,std::wstring(COLL_ROOT).append(L"\\Bases_1\\data\\u0607g.xml"),"u0607g.xml.klz")

		//xml
		TransferFile(hs4, std::wstring(COLL_ROOT).append(L"\\Bases_1\\data\\u0607g.xml"), "u0607g.xml");

		//скачивается вторичный индекс (ошибка 23430)
		//QC_BOOST_CHECK_MESSAGE (false, L"BUG 23430\n");

		//diff
		DO_ERROR(hs4,std::wstring(COLL_ROOT).append(L"\\Bases_1\\data\\av-i386-0607g.xml"),"av-i386-0607g.xml.dif")
		//klz
		DO_ERROR(hs4,std::wstring(COLL_ROOT).append(L"\\Bases_1\\data\\av-i386-0607g.xml"),"av-i386-0607g.xml.klz")

		//xml
		TransferFile(hs4, std::wstring(COLL_ROOT).append(L"\\Bases_1\\plain\\av-i386-0607g.xml"), "av-i386-0607g.xml");

		//начинаем скачивать базы
		//даем скачать первый файл баз

	TransferFile(hs4, std::wstring(COLL_ROOT).append(L"\\Bases_1\\plain\\blst-0607g.xml"), "blst-0607g.xml.dif");
	TransferFile(hs4, std::wstring(COLL_ROOT).append(L"\\Bases_1\\plain\\blst-0607g.xml"), "blst-0607g.xml.klz");
	TransferFile(hs4, std::wstring(COLL_ROOT).append(L"\\Bases_1\\plain\\blst-0607g.xml"), "blst-0607g.xml");

		//avc.diff
		TransferFile(hs4, std::wstring(COLL_ROOT).append(L"\\Bases_1\\plain\\base102.avc"), "base102.avc.");

		//avc
		TransferFile(hs4, std::wstring(COLL_ROOT).append(L"\\Bases_1\\plain\\base102.avc"), "base102.avc");

		//на выкачивании второго файла баз происходит ошибка

		//avc
		DO_ERROR(hs4,std::wstring(COLL_ROOT).append(L"\\Bases_1\\data\\base103.avc"),"base103.avc")

		//переключаемся на пятый источник

		//опять начинаем с главного индекса

		//diff
		DO_ERROR(hs5,std::wstring(COLL_ROOT).append(L"\\Bases_1\\data\\u0607g.xml"),"u0607g.xml.dif")
		//klz
		DO_ERROR(hs5,std::wstring(COLL_ROOT).append(L"\\Bases_1\\data\\u0607g.xml"),"u0607g.xml.klz")

		//xml
		TransferFile(hs5, std::wstring(COLL_ROOT).append(L"\\Bases_1\\data\\u0607g.xml"), "u0607g.xml");

		//скачивается вторичный индекс (ошибка 23430)
		//QC_BOOST_CHECK_MESSAGE (false, L"BUG 23430\n");

		//diff
		DO_ERROR(hs5,std::wstring(COLL_ROOT).append(L"\\Bases_1\\data\\av-i386-0607g.xml"),"av-i386-0607g.xml.dif")
		//klz
		DO_ERROR(hs5,std::wstring(COLL_ROOT).append(L"\\Bases_1\\data\\av-i386-0607g.xml"),"av-i386-0607g.xml.klz")

		//xml
		TransferFile(hs5, std::wstring(COLL_ROOT).append(L"\\Bases_1\\plain\\av-i386-0607g.xml"), "av-i386-0607g.xml");

		//начинаем скачивать базы
		//даем скачать первый файл баз

		//первый файл баз уже закэширован
		//выкачивается второй файл баз

	TransferFile(hs5, std::wstring(COLL_ROOT).append(L"\\Bases_1\\plain\\blst-0607g.xml"), "blst-0607g.xml.dif");
	TransferFile(hs5, std::wstring(COLL_ROOT).append(L"\\Bases_1\\plain\\blst-0607g.xml"), "blst-0607g.xml.klz");
	TransferFile(hs5, std::wstring(COLL_ROOT).append(L"\\Bases_1\\plain\\blst-0607g.xml"), "blst-0607g.xml");

		//avc
		TransferFile(hs5, std::wstring(COLL_ROOT).append(L"\\Bases_1\\plain\\base103.avc"), "base103.avc");

		WaitForSingleObject (hDoUpdate, INFINITE);

		/*
		QC_BOOST_CHECK_MESSAGE (_helper::DeleteFile(std::wstring(BASES_ROOT).append (L"\\base100.avc")),
			L"old base file not found");
		QC_BOOST_CHECK_MESSAGE (_helper::IsFoldersEqual(DIFFTOOL_ROOT,
			std::wstring(COLL_ROOT).append(L"\\Bases_1\\plain").c_str(), BASES_ROOT), L"Incorrect bases\n");

		QC_BOOST_CHECK_MESSAGE (_helper::IsFoldersEqual(DIFFTOOL_ROOT,
			std::wstring(COLL_ROOT).append(L"\\Bases_1\\data").c_str(), UPD_ROOT), L"Incorrect index\n");
	*/

		QC_BOOST_CHECK_MESSAGE ( NO_ERROR == do_upd.GetError(), L"Wrong return code\n" );
	}

}

void TestSuite::TestSwitchingURLsInCaseOfTimeouts ()
{
	#define CLOSE_CONNECTION false, false, false
	#define DO_ERROR(SERVER,FILENAME,REQNAME) EmulateNetError(SERVER,TIMEOUT_EXCEEDS,FILENAME,REQNAME);

	std::wcout<<L"TestSwitchingURLsInCaseOfTimeouts\n";
	{
		TestLog testLog;
		HttpServer hs (8001, "127.0.0.1"); hs.Start();
		HttpServer hs2 (8002, "127.0.0.1"); hs2.Start();
		HttpServer hs3 (8003, "127.0.0.1"); hs3.Start();
		HttpServer hs4 (8004, "127.0.0.1"); hs4.Start();
		HttpServer hs5 (8005, "127.0.0.1"); hs5.Start();
		HttpServer hs6 (8006, "127.0.0.1"); hs6.Start();
		HttpServer hs7 (8007, "127.0.0.1"); hs7.Start();


		_helper::DeleteDirectory(ROOT);
		KLUPD::createFolder (BASES_ROOT, &testLog);
		KLUPD::createFolder (UPD_ROOT, &testLog);

		_helper::CopyFolder(std::wstring(COLL_ROOT).append(L"\\Bases_NULL_FOR_DNLD\\plain").c_str(),BASES_ROOT);
		_helper::CloneFile (std::wstring(COLL_ROOT).append(L"\\Bases_NULL_FOR_DNLD\\index\\6\\u0607g.xml").c_str(),
			std::wstring (UPD_ROOT).append (L"\\u0607g.xml"), false);

		UpdaterConfiguration data(1, 1, L"1");

		data.temporary_dir = TEMP_ROOT;

		data.m_pathSubstitutionMap.push_back(std::make_pair<NoCaseString,NoCaseString>(L"%BaseFolder%", BASES_ROOT));
		data.m_pathSubstitutionMap.push_back(std::make_pair<NoCaseString,NoCaseString>(L"%Folder%", ROLLBACK_ROOT));
		data.m_pathSubstitutionMap.push_back(std::make_pair<NoCaseString,NoCaseString>(L"%UpdateRoot%", UPD_ROOT));
		data.m_pathSubstitutionMap.push_back(std::make_pair<NoCaseString,NoCaseString>(L"%DSKM%", DSKM_ROOT));
		data.m_pathSubstitutionMap.push_back(std::make_pair<NoCaseString,NoCaseString>(L"%DataFolder%", UPD_ROOT));

		data.self_region = L"<default>";
		data.product_lang.push_back( NoCaseString(L"<default>"));

		data.os.os = L"Windows";
		data.os.version = L"5.1";

		data.m_componentsToUpdate.push_back ( L"AVS" );
		data.m_componentsToUpdate.push_back ( L"CORE" );
		data.connect_tmo = 10;

		data.m_sourceList.addSource (UST_UserURL, L"http://127.0.0.1:8001", false, L"diffs");
		data.m_sourceList.addSource (UST_UserURL, L"http://127.0.0.1:8002", false, L"diffs");
		data.m_sourceList.addSource (UST_UserURL, L"http://127.0.0.1:8003", false, L"diffs");
		data.m_sourceList.addSource (UST_UserURL, L"http://127.0.0.1:8004", false, L"diffs");
		data.m_sourceList.addSource (UST_UserURL, L"http://127.0.0.1:8005", false, L"diffs");
		data.m_sourceList.addSource (UST_UserURL, L"http://127.0.0.1:8006", false, L"diffs");
		data.m_sourceList.addSource (UST_UserURL, L"http://127.0.0.1:8007", false, L"diffs");

		data.UpdaterListFlags.update_bases = true;

		Callbacks cbacks;
		TestCallbackInterface testCallback (&testLog, &cbacks);
		TestJournalInterface testJournal;
		testCallback.SetUpdaterConfiguration (data);

		Updater upd (testCallback.m_dp, testCallback, testJournal, &testLog);
		HANDLE hDoUpdate;

		doUpdater do_upd(&upd, hDoUpdate);

		//таймаут на приеме диффа
		//diff
		DO_ERROR(hs,std::wstring(COLL_ROOT).append(L"\\Bases_1\\data\\u0607g.xml"),"u0607g.xml.dif")

		//переключается на следующий источник (2)
		//таймаут на приеме klz
		//diff
		TransferFile(hs2, std::wstring(COLL_ROOT).append(L"\\Bases_1\\data\\u0607g.xml"), "u0607g.xml.dif");
		//klz
		DO_ERROR(hs2,std::wstring(COLL_ROOT).append(L"\\Bases_1\\data\\u0607g.xml"),"u0607g.xml.klz")

		//переключается на следующий источник (3)
		//таймаут на приеме главного xml
		//diff
		TransferFile(hs3, std::wstring(COLL_ROOT).append(L"\\Bases_1\\data\\u0607g.xml"), "u0607g.xml.dif");
		//klz
		TransferFile(hs3, std::wstring(COLL_ROOT).append(L"\\Bases_1\\data\\u0607g.xml"), "u0607g.xml.klz");
		//xml
		DO_ERROR(hs3,std::wstring(COLL_ROOT).append(L"\\Bases_1\\data\\u0607g.xml"),"u0607g.xml")

		//переключается на следующий источник (4)
		//таймаут на приеме вторичного xml
		//diff
		TransferFile(hs4, std::wstring(COLL_ROOT).append(L"\\Bases_1\\data\\u0607g.xml"), "u0607g.xml.dif");
		//klz
		TransferFile(hs4, std::wstring(COLL_ROOT).append(L"\\Bases_1\\data\\u0607g.xml"), "u0607g.xml.klz");
		//xml
		TransferFile(hs4, std::wstring(COLL_ROOT).append(L"\\Bases_1\\data\\u0607g.xml"), "u0607g.xml");
		//diff
		TransferFile(hs4, std::wstring(COLL_ROOT).append(L"\\Bases_1\\plain\\av-i386-0607g.xml"), "av-i386-0607g.xml.dif");
		//klz
		TransferFile(hs4, std::wstring(COLL_ROOT).append(L"\\Bases_1\\plain\\av-i386-0607g.xml"), "av-i386-0607g.xml.klz");
		//xml
		DO_ERROR(hs4,std::wstring(COLL_ROOT).append(L"\\Bases_1\\plain\\av-i386-0607g.xml"),"av-i386-0607g.xml")

		//переключается на следующий источник (5)
		//таймаут на приеме диффа для баз
		//diff
		TransferFile(hs5, std::wstring(COLL_ROOT).append(L"\\Bases_1\\data\\u0607g.xml"), "u0607g.xml.dif");
		//klz
		TransferFile(hs5, std::wstring(COLL_ROOT).append(L"\\Bases_1\\data\\u0607g.xml"), "u0607g.xml.klz");
		//xml
		TransferFile(hs5, std::wstring(COLL_ROOT).append(L"\\Bases_1\\data\\u0607g.xml"), "u0607g.xml");
		//diff
		TransferFile(hs5, std::wstring(COLL_ROOT).append(L"\\Bases_1\\plain\\av-i386-0607g.xml"), "av-i386-0607g.xml.dif");
		//klz
		TransferFile(hs5, std::wstring(COLL_ROOT).append(L"\\Bases_1\\plain\\av-i386-0607g.xml"), "av-i386-0607g.xml.klz");
		//xml
		TransferFile(hs5, std::wstring(COLL_ROOT).append(L"\\Bases_1\\plain\\av-i386-0607g.xml"), "av-i386-0607g.xml");
		
	TransferFile(hs5, std::wstring(COLL_ROOT).append(L"\\Bases_1\\plain\\blst-0607g.xml"), "blst-0607g.xml.dif");
	TransferFile(hs5, std::wstring(COLL_ROOT).append(L"\\Bases_1\\plain\\blst-0607g.xml"), "blst-0607g.xml.klz");
	TransferFile(hs5, std::wstring(COLL_ROOT).append(L"\\Bases_1\\plain\\blst-0607g.xml"), "blst-0607g.xml");
	TransferFile(hs5, std::wstring(COLL_ROOT).append(L"\\Bases_1\\plain\\black.lst"), "black.lst.");
	TransferFile(hs5, std::wstring(COLL_ROOT).append(L"\\Bases_1\\plain\\black.lst"), "black.lst");

		//diff.avc
		DO_ERROR(hs5,std::wstring(COLL_ROOT).append(L"\\Bases_1\\plain\\base102.avc"),"base102.avc.");

		//переключается на следующий источник (6)
		//таймаут на приеме файла баз
		//diff
		TransferFile(hs6, std::wstring(COLL_ROOT).append(L"\\Bases_1\\data\\u0607g.xml"), "u0607g.xml.dif");
		//klz
		TransferFile(hs6, std::wstring(COLL_ROOT).append(L"\\Bases_1\\data\\u0607g.xml"), "u0607g.xml.klz");
		//xml
		TransferFile(hs6, std::wstring(COLL_ROOT).append(L"\\Bases_1\\data\\u0607g.xml"), "u0607g.xml");
		//diff
		TransferFile(hs6, std::wstring(COLL_ROOT).append(L"\\Bases_1\\plain\\av-i386-0607g.xml"), "av-i386-0607g.xml.dif");
		//klz
		TransferFile(hs6, std::wstring(COLL_ROOT).append(L"\\Bases_1\\plain\\av-i386-0607g.xml"), "av-i386-0607g.xml.klz");
		//xml
		TransferFile(hs6, std::wstring(COLL_ROOT).append(L"\\Bases_1\\plain\\av-i386-0607g.xml"), "av-i386-0607g.xml");
		
	TransferFile(hs6, std::wstring(COLL_ROOT).append(L"\\Bases_1\\plain\\blst-0607g.xml"), "blst-0607g.xml.dif");
	TransferFile(hs6, std::wstring(COLL_ROOT).append(L"\\Bases_1\\plain\\blst-0607g.xml"), "blst-0607g.xml.klz");
	TransferFile(hs6, std::wstring(COLL_ROOT).append(L"\\Bases_1\\plain\\blst-0607g.xml"), "blst-0607g.xml");

		//diff.avc
		TransferFile(hs6, std::wstring(COLL_ROOT).append(L"\\Bases_1\\plain\\base102.avc"), "base102.avc.");
		//avc
		DO_ERROR(hs6,std::wstring(COLL_ROOT).append(L"\\Bases_1\\plain\\base102.avc"),"base102.avc");

		//переключается на следующий источник (7)
		//нормальный прием баз
		//diff
		TransferFile(hs7, std::wstring(COLL_ROOT).append(L"\\Bases_1\\data\\u0607g.xml"), "u0607g.xml.dif");
		//klz
		TransferFile(hs7, std::wstring(COLL_ROOT).append(L"\\Bases_1\\data\\u0607g.xml"), "u0607g.xml.klz");
		//xml
		TransferFile(hs7, std::wstring(COLL_ROOT).append(L"\\Bases_1\\data\\u0607g.xml"), "u0607g.xml");
		//diff
		TransferFile(hs7, std::wstring(COLL_ROOT).append(L"\\Bases_1\\plain\\av-i386-0607g.xml"), "av-i386-0607g.xml.dif");
		//klz
		TransferFile(hs7, std::wstring(COLL_ROOT).append(L"\\Bases_1\\plain\\av-i386-0607g.xml"), "av-i386-0607g.xml.klz");
		//xml
		TransferFile(hs7, std::wstring(COLL_ROOT).append(L"\\Bases_1\\plain\\av-i386-0607g.xml"), "av-i386-0607g.xml");
		
	TransferFile(hs7, std::wstring(COLL_ROOT).append(L"\\Bases_1\\plain\\blst-0607g.xml"), "blst-0607g.xml.dif");
	TransferFile(hs7, std::wstring(COLL_ROOT).append(L"\\Bases_1\\plain\\blst-0607g.xml"), "blst-0607g.xml.klz");
	TransferFile(hs7, std::wstring(COLL_ROOT).append(L"\\Bases_1\\plain\\blst-0607g.xml"), "blst-0607g.xml");
		
		//avc
		TransferFile(hs7, std::wstring(COLL_ROOT).append(L"\\Bases_1\\plain\\base102.avc"), "base102.avc");
		//avc
		TransferFile(hs7, std::wstring(COLL_ROOT).append(L"\\Bases_1\\plain\\base103.avc"), "base103.avc");

		WaitForSingleObject (hDoUpdate, INFINITE);

		/*
		QC_BOOST_CHECK_MESSAGE (_helper::DeleteFile(std::wstring(BASES_ROOT).append (L"\\base100.avc")),
		L"old base file not found");
		QC_BOOST_CHECK_MESSAGE (_helper::IsFoldersEqual(DIFFTOOL_ROOT,
		std::wstring(COLL_ROOT).append(L"\\Bases_1\\plain").c_str(), BASES_ROOT), L"Incorrect bases\n");

		QC_BOOST_CHECK_MESSAGE (_helper::IsFoldersEqual(DIFFTOOL_ROOT,
		std::wstring(COLL_ROOT).append(L"\\Bases_1\\data").c_str(), UPD_ROOT), L"Incorrect index\n");
		*/

		QC_BOOST_CHECK_MESSAGE ( NO_ERROR == do_upd.GetError(), L"Wrong return code\n" );
	}

}

void TestSuite::TransferFile (HttpServer& hs, const wstring& filename, const string& reqname)
{
	string command;
	string param;
	HttpServer::CMD_TYPE type;

	hs.Accept();
	hs.Receive(command, type, param);
	QC_BOOST_CHECK_MESSAGE (type == HttpServer::CMD_GET, L"Error\n");
	_ASSERTE (command.find (reqname)!= wstring::npos);
	hs.SendFile (filename, false, false, false);
	hs.CloseConnection ();

}

void TestSuite::TestWhitespaces ()
{
	using namespace KLUPD;
	std::wcout<<L"TestWhitespaces\n";
	Logger::Case _case (m_logger, m_testp, m_step, L"TestWhitespaces");

	{//leading, trailing
		TestLog testLog;

		_helper::DeleteDirectory(ROOT);
		KLUPD::createFolder (BASES_ROOT, &testLog);
		KLUPD::createFolder (UPD_ROOT, &testLog);
		KLUPD::createFolder (PRODUCT_ROOT, &testLog);

		_helper::CopyFolder(std::wstring(COLL_ROOT).append(L"\\Bases_NULL\\plain").c_str(),BASES_ROOT);
		_helper::CloneFile (std::wstring(COLL_ROOT).append(L"\\Bases_NULL\\index\\6\\u0607g.xml").c_str(),
			std::wstring (UPD_ROOT).append (L"\\u0607g.xml"), false);

		UpdaterConfiguration data(1, 1, L"1");

		data.temporary_dir = TEMP_ROOT;

		data.m_pathSubstitutionMap.push_back(std::make_pair<NoCaseString,NoCaseString>(L"%BaseFolder%", BASES_ROOT));
		data.m_pathSubstitutionMap.push_back(std::make_pair<NoCaseString,NoCaseString>(L"%Folder%", ROLLBACK_ROOT));
		data.m_pathSubstitutionMap.push_back(std::make_pair<NoCaseString,NoCaseString>(L"%UpdateRoot%", UPD_ROOT));
		data.m_pathSubstitutionMap.push_back(std::make_pair<NoCaseString,NoCaseString>(L"%DSKM%", DSKM_ROOT));
		data.m_pathSubstitutionMap.push_back(std::make_pair<NoCaseString,NoCaseString>(L"%DataFolder%", UPD_ROOT));

		data.self_region = L"<default>";
		data.product_lang.push_back( NoCaseString(L"<default>"));

		data.os.os = L"Windows";
		data.os.version = L"5.1";

		data.m_componentsToUpdate.push_back (L"AVS");
		data.m_componentsToUpdate.push_back (L"CORE");
		data.connect_tmo = 30;

		data.m_sourceList.addSource (UST_UserURL, L"  \t  http://tl-srv-wxp \t", false, L"diffs");
		data.UpdaterListFlags.update_bases = true;

		Callbacks cbacks;
		TestCallbackInterface testCallback (&testLog, &cbacks);
		TestJournalInterface testJournal;
		testCallback.SetUpdaterConfiguration (data);
		testCallback.SetPrimaryIndex( FileInfo (L"u0607g.xml", L"updater13/bases_1/index/6", FileInfo::index, true) );

		FileInfo u0607g ( L"u0607g.xml", L"updater13/bases_1/index/6", FileInfo::index, true );
		u0607g.m_localPath = UPD_ROOT;
		testCallback.SetPrimaryIndex(u0607g);

		Updater upd (testCallback.m_dp, testCallback, testJournal, &testLog);
		HANDLE hDoUpdate;

		cbacks.bFlags[Callbacks::getPrimaryIndex] = true;

		doUpdater do_upd(&upd, hDoUpdate);

		WaitForSingleObject (hDoUpdate, INFINITE);

		QC_BOOST_CHECK_MESSAGE ( NO_ERROR == do_upd.GetError(), L"Wrong return code\n" );
		QC_BOOST_CHECK_MESSAGE (_helper::DeleteFile(std::wstring(BASES_ROOT).append (L"\\base100.avc")),
			L"old base file not found");
		QC_BOOST_CHECK_MESSAGE (_helper::IsFoldersEqual(DIFFTOOL_ROOT,
			std::wstring(COLL_ROOT).append(L"\\tl-srv-wxp\\Bases_1\\plain").c_str(), BASES_ROOT), L"Incorrect bases\n");
		QC_BOOST_CHECK_MESSAGE (_helper::IsFoldersEqual(DIFFTOOL_ROOT,
			std::wstring(COLL_ROOT).append(L"\\tl-srv-wxp\\Bases_1\\data").c_str(), UPD_ROOT), L"Incorrect index\n");
	}
/*	{//2
		TestLog testLog;

		_helper::DeleteDirectory(ROOT);
		KLUPD::createFolder (BASES_ROOT, &testLog);
		KLUPD::createFolder (UPD_ROOT, &testLog);
		KLUPD::createFolder (PRODUCT_ROOT, &testLog);

		_helper::CopyFolder(std::wstring(COLL_ROOT).append(L"\\Bases_NULL\\plain").c_str(),BASES_ROOT);
		_helper::CloneFile (std::wstring(COLL_ROOT).append(L"\\Bases_NULL\\index\\6\\u0607g.xml").c_str(),
			std::wstring (UPD_ROOT).append (L"\\u0607g.xml"), false);

		UpdaterConfiguration data(1, 1, L"1");

		data.temporary_dir = TEMP_ROOT;

		data.m_pathSubstitutionMap.push_back(std::make_pair<NoCaseString,NoCaseString>(L"%BaseFolder%", BASES_ROOT));
		data.m_pathSubstitutionMap.push_back(std::make_pair<NoCaseString,NoCaseString>(L"%Folder%", ROLLBACK_ROOT));
		data.m_pathSubstitutionMap.push_back(std::make_pair<NoCaseString,NoCaseString>(L"%UpdateRoot%", UPD_ROOT));
		data.m_pathSubstitutionMap.push_back(std::make_pair<NoCaseString,NoCaseString>(L"%DSKM%", DSKM_ROOT));
		data.m_pathSubstitutionMap.push_back(std::make_pair<NoCaseString,NoCaseString>(L"%DataFolder%", UPD_ROOT));

		data.self_region = L"<default>";
		data.product_lang.push_back( NoCaseString(L"<default>"));

		data.os.os = L"Windows";
		data.os.version = L"5.1";

		data.m_componentsToUpdate.push_back (L"AVS");
		data.m_componentsToUpdate.push_back (L"CORE");
		data.connect_tmo = 30;

		data.m_sourceList.addSource (UST_UserURL, L"\thttp://tl-srv-wxp/sp ace\t\t\t ", false, L"diffs");
		data.UpdaterListFlags.update_bases = true;

		Callbacks cbacks;
		TestCallbackInterface testCallback (&testLog, &cbacks);
		TestJournalInterface testJournal;
		testCallback.SetUpdaterConfiguration (data);
		testCallback.SetPrimaryIndex( FileInfo (L"u0607g.xml", L"updater13/bases_1/index/6", FileInfo::index, true) );

		FileInfo u0607g ( L"u0607g.xml", L"updater13/bases_1/index/6", FileInfo::index, true );
		u0607g.m_localPath = UPD_ROOT;
		testCallback.SetPrimaryIndex(u0607g);

		Updater upd (testCallback.m_dp, testCallback, testJournal, &testLog);
		HANDLE hDoUpdate;

		cbacks.bFlags[Callbacks::getPrimaryIndex] = true;

		doUpdater do_upd(&upd, hDoUpdate);

		WaitForSingleObject (hDoUpdate, INFINITE);

		QC_BOOST_CHECK_MESSAGE ( NO_ERROR == do_upd.GetError(), L"Wrong return code\n" );
		QC_BOOST_CHECK_MESSAGE (_helper::DeleteFile(std::wstring(BASES_ROOT).append (L"\\base100.avc")),
			L"old base file not found");
		QC_BOOST_CHECK_MESSAGE (_helper::IsFoldersEqual(DIFFTOOL_ROOT,
			std::wstring(COLL_ROOT).append(L"\\tl-srv-wxp\\Bases_1\\plain").c_str(), BASES_ROOT), L"Incorrect bases\n");
		QC_BOOST_CHECK_MESSAGE (_helper::IsFoldersEqual(DIFFTOOL_ROOT,
			std::wstring(COLL_ROOT).append(L"\\tl-srv-wxp\\Bases_1\\data").c_str(), UPD_ROOT), L"Incorrect index\n");
	}
*/	
}