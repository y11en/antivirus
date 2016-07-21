#include "util.h"

#include "log.h"
#include "callback.h"
#include "journal.h"
#include "core/updater.h"
#include "helper/local_file.h"

#include "TestSuite.h"
#include "main.h"

void TestSuite::TestRetrCheckFilesToDownload ()
{
	using namespace KLUPD;
	std::wcout<<L"TestRetrCheckFilesToDownload\n";
	Logger::Case _case (m_logger, m_testp, m_step, L"TestRetrCheckFilesToDownload");

	{
		std::wcout<<L"Check transaction status\n";

		TestLog testLog;

		_helper::DeleteDirectory(ROOT);

		KLUPD::createFolder (std::wstring (RETR_ROOT).append (L"\\index\\6").c_str(), &testLog);
		KLUPD::createFolder (std::wstring (RETR_ROOT).append (L"\\bases").c_str(), &testLog);

		_helper::CopyFolder(std::wstring(COLL_ROOT).append(L"\\Bases_NULL\\bases").c_str(),
			std::wstring (RETR_ROOT).append (L"\\bases").c_str());
		_helper::CloneFile (std::wstring(COLL_ROOT).append(L"\\Bases_NULL\\index\\6\\u0607g.xml").c_str(),
			std::wstring (RETR_ROOT).append (L"\\index\\6\\u0607g.xml"), false);

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

		data.updates_retranslation = true;
		data.retranslate_only_listed_apps = true;
		data.connect_tmo = 30;

		data.m_sourceList.addSource (UST_UserURL, std::wstring(COLL_ROOT).append(L"\\Bases_1").c_str(), true, L"diffs");
		
		data.RetranslationListFlags.update_bases = true;
		data.RetranslationListFlags.automatically_apply_available_updates = false;
		data.RetranslationListFlags.apply_urgent_updates = false;

		data.UpdaterListFlags.update_bases = false;
		data.UpdaterListFlags.automatically_apply_available_updates = false;
		data.UpdaterListFlags.apply_urgent_updates = false;

		Callbacks cbacks;
		TestCallbackInterface testCallback (&testLog, &cbacks);
		TestJournalInterface testJournal;
		testCallback.SetUpdaterConfiguration (data);
		
		std::wcout<<L"updateRequired "<<data.updateRequired()<<L"\n";
		std::wcout<<L"retranslationRequired "<<data.retranslationRequired()<<L"\n";

		FileInfo u0607g ( L"u0607g.xml", L"index/6", FileInfo::index, true );
		u0607g.m_localPath = RETR_ROOT;
		testCallback.SetPrimaryIndex(u0607g);
		
		cbacks.bFlags[Callbacks::getPrimaryIndexWithEvent] = true;
		cbacks.bFlags[Callbacks::checkFilesToDownload] = true;
		CREATE_EVENTS(getPrimaryIndexWithEvent)
		CREATE_EVENTS(checkFilesToDownload)

		Updater upd (testCallback.m_dp, testCallback, testJournal, &testLog);
		HANDLE hDoUpdate;

		doUpdater do_upd(&upd, hDoUpdate);

//		//bug workaround
//		WAIT_FOR_READY_EVENT(getPrimaryIndexWithEvent)
//		testCallback.SetPrimaryIndex( FileInfo() );
//		SET_GOON_EVENT(getPrimaryIndexWithEvent);

		WAIT_FOR_READY_EVENT(getPrimaryIndexWithEvent)
		testCallback.SetPrimaryIndex( u0607g );
		QC_BOOST_CHECK_MESSAGE (testCallback.GetGpiRetrMode() == true, L"Incorrect getPrimaryIndex retr mode\n");
		SET_GOON_EVENT(getPrimaryIndexWithEvent);


		WAIT_FOR_READY_EVENT(checkFilesToDownload)
		TestCallbackInterface::_checkFilesToDownload& cfd = testCallback.GetCheckFilesToDownload();
		std::wcout<<cfd.m_matchFileList->size()<<L"\n";
		for (std::vector<FileInfo>::iterator it = cfd.m_matchFileList->begin();
				it!=cfd.m_matchFileList->end(); ++it)
				{
					std::wcout<<it->m_filename.m_value.toWideChar()<<L"\n";
				}

		QC_BOOST_CHECK_MESSAGE (cfd.retranslationMode, L"Retr mode is not set\n");

		QC_BOOST_REQUIRE_MESSAGE (cfd.m_matchFileList->size() == 7, L"Incorrect file list size\n");
				
		QC_BOOST_CHECK_MESSAGE (cfd.m_matchFileList->at(4).m_filename == L"base101.avc", L"Incorrect file name\n");
		QC_BOOST_CHECK_MESSAGE (cfd.m_matchFileList->at(5).m_filename == L"base102.avc", L"Incorrect file name\n");
		QC_BOOST_CHECK_MESSAGE (cfd.m_matchFileList->at(6).m_filename == L"base103.avc", L"Incorrect file name\n");
		//QC_BOOST_CHECK_MESSAGE (cfd.m_matchFileList->at(9).m_filename == L"base100.avc", L"Incorrect file name\n");

		QC_BOOST_CHECK_MESSAGE (cfd.m_matchFileList->at(4).m_transactionInformation.m_changeStatus == FileInfo::unchanged, L"Incorrect transaction status\n");
		QC_BOOST_CHECK_MESSAGE (cfd.m_matchFileList->at(5).m_transactionInformation.m_changeStatus == FileInfo::modified, L"Incorrect transaction status\n");
		QC_BOOST_CHECK_MESSAGE (cfd.m_matchFileList->at(6).m_transactionInformation.m_changeStatus == FileInfo::added, L"Incorrect transaction status\n");
		//QC_BOOST_CHECK_MESSAGE (cfd.m_matchFileList->at(9).m_transactionInformation.m_changeStatus == FileInfo::deleted, L"Incorrect transaction status\n");
		
		SET_GOON_EVENT(checkFilesToDownload);

		WaitForSingleObject (hDoUpdate, INFINITE);

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


		QC_BOOST_CHECK_MESSAGE ( CORE_RETRANSLATION_SUCCESSFUL == do_upd.GetError(), L"Wrong return code\n" );

	}

	{
		std::wcout<<L"Change transaction status\n";

		TestLog testLog;

		_helper::DeleteDirectory(ROOT);

		KLUPD::createFolder (std::wstring (RETR_ROOT).append (L"\\index\\6").c_str(), &testLog);
		KLUPD::createFolder (std::wstring (RETR_ROOT).append (L"\\bases").c_str(), &testLog);

		_helper::CopyFolder(std::wstring(COLL_ROOT).append(L"\\Bases_NULL\\bases").c_str(),
			std::wstring (RETR_ROOT).append (L"\\bases").c_str());
		_helper::CloneFile (std::wstring(COLL_ROOT).append(L"\\Bases_NULL\\index\\6\\u0607g.xml").c_str(),
			std::wstring (RETR_ROOT).append (L"\\index\\6\\u0607g.xml"), false);

		UpdaterConfiguration data(1, 1, L"1");

		data.temporary_dir = TEMP_ROOT;
		//data.reserve_dir = RESERVE_ROOT;
		//data.update_root_folder = UPD_ROOT;
		data.retranslation_dir = RETR_ROOT;

		data.m_pathSubstitutionMap.push_back(std::make_pair<NoCaseString,NoCaseString>(L"%BaseFolder%", BASES_ROOT));
		data.m_pathSubstitutionMap.push_back(std::make_pair<NoCaseString,NoCaseString>(L"%Folder%", ROLLBACK_ROOT));
		data.m_pathSubstitutionMap.push_back(std::make_pair<NoCaseString,NoCaseString>(L"%UpdateRoot%", UPD_ROOT));
		data.m_pathSubstitutionMap.push_back(std::make_pair<NoCaseString,NoCaseString>(L"%DSKM%", DSKM_ROOT));
		data.m_pathSubstitutionMap.push_back(std::make_pair<NoCaseString,NoCaseString>(L"%DataFolder%", UPD_ROOT));

		data.self_region = L"<default>";
		data.product_lang.push_back( NoCaseString(L"<default>"));
		
		data.os.os = L"Windows";
		data.os.version = L"5.1";

		data.RetranslatedObjects.m_componentsToRetranslate.push_back (L"AVS");
		data.RetranslatedObjects.m_componentsToRetranslate.push_back (L"CORE");

		data.updates_retranslation = true;
		data.retranslate_only_listed_apps = true;
		data.connect_tmo = 30;

		data.m_sourceList.addSource (UST_UserURL, std::wstring(COLL_ROOT).append(L"\\Bases_1").c_str(), true, L"diffs");
		
		data.RetranslationListFlags.update_bases = true;
		data.RetranslationListFlags.automatically_apply_available_updates = false;
		data.RetranslationListFlags.apply_urgent_updates = false;

		data.UpdaterListFlags.update_bases = false;
		data.UpdaterListFlags.automatically_apply_available_updates = false;
		data.UpdaterListFlags.apply_urgent_updates = false;

		Callbacks cbacks;
		TestCallbackInterface testCallback (&testLog, &cbacks);
		TestJournalInterface testJournal;
		testCallback.SetUpdaterConfiguration (data);
		
		std::wcout<<L"updateRequired "<<data.updateRequired()<<L"\n";
		std::wcout<<L"retranslationRequired "<<data.retranslationRequired()<<L"\n";

		FileInfo u0607g ( L"u0607g.xml", L"index/6", FileInfo::index, true );
		u0607g.m_localPath = RETR_ROOT;
		testCallback.SetPrimaryIndex(u0607g);
		
		cbacks.bFlags[Callbacks::getPrimaryIndexWithEvent] = true;
		cbacks.bFlags[Callbacks::checkFilesToDownload] = true;
		CREATE_EVENTS(getPrimaryIndexWithEvent)
		CREATE_EVENTS(checkFilesToDownload)

		Updater upd (testCallback.m_dp, testCallback, testJournal, &testLog);
		HANDLE hDoUpdate;

		doUpdater do_upd(&upd, hDoUpdate);

//		//bug workaround
//		WAIT_FOR_READY_EVENT(getPrimaryIndexWithEvent)
//		testCallback.SetPrimaryIndex( FileInfo() );
//		SET_GOON_EVENT(getPrimaryIndexWithEvent);

		WAIT_FOR_READY_EVENT(getPrimaryIndexWithEvent)
		testCallback.SetPrimaryIndex( u0607g );
		QC_BOOST_CHECK_MESSAGE (testCallback.GetGpiRetrMode() == true, L"Incorrect getPrimaryIndex retr mode\n");
		SET_GOON_EVENT(getPrimaryIndexWithEvent);

		WAIT_FOR_READY_EVENT(checkFilesToDownload)
		TestCallbackInterface::_checkFilesToDownload& cfd = testCallback.GetCheckFilesToDownload();
		std::wcout<<cfd.m_matchFileList->size()<<L"\n";
		for (std::vector<FileInfo>::iterator it = cfd.m_matchFileList->begin();
				it!=cfd.m_matchFileList->end(); ++it)
				{
					std::wcout<<it->m_filename.m_value.toWideChar()<<L"\n";
				}

		QC_BOOST_CHECK_MESSAGE (cfd.retranslationMode, L"Retr mode is not set\n");

		QC_BOOST_REQUIRE_MESSAGE (cfd.m_matchFileList->size() == 7, L"Incorrect file list size\n");

		for (std::vector<FileInfo>::iterator it = cfd.m_matchFileList->begin();
				it!=cfd.m_matchFileList->end(); ++it)

		it->m_transactionInformation.m_changeStatus = FileInfo::unchanged;
		
		SET_GOON_EVENT(checkFilesToDownload)

		WaitForSingleObject (hDoUpdate, INFINITE);

		QC_BOOST_CHECK_MESSAGE (_helper::IsFoldersEqual(DIFFTOOL_ROOT,
			std::wstring(COLL_ROOT).append(L"\\Bases_NULL\\bases").c_str(),
			std::wstring (RETR_ROOT).append (L"\\bases")), L"Incorrect bases\n");
		QC_BOOST_CHECK_MESSAGE (_helper::IsFoldersEqual(DIFFTOOL_ROOT,
			std::wstring(COLL_ROOT).append(L"\\Bases_NULL\\data").c_str(),
			std::wstring (RETR_ROOT).append (L"\\index\\6")), L"Incorrect bases\n");

		QC_BOOST_CHECK_MESSAGE ( CORE_RETRANSLATION_SUCCESSFUL == do_upd.GetError(), L"Wrong return code\n" );
	}

	{
		std::wcout<<L"Check answer result\n";

		TestLog testLog;

		_helper::DeleteDirectory(ROOT);

		KLUPD::createFolder (std::wstring (RETR_ROOT).append (L"\\index\\6").c_str(), &testLog);
		KLUPD::createFolder (std::wstring (RETR_ROOT).append (L"\\bases").c_str(), &testLog);

		_helper::CopyFolder(std::wstring(COLL_ROOT).append(L"\\Bases_NULL\\bases").c_str(),
			std::wstring (RETR_ROOT).append (L"\\bases").c_str());
		_helper::CloneFile (std::wstring(COLL_ROOT).append(L"\\Bases_NULL\\index\\6\\u0607g.xml").c_str(),
			std::wstring (RETR_ROOT).append (L"\\index\\6\\u0607g.xml"), false);

		UpdaterConfiguration data(1, 1, L"1");

		data.temporary_dir = TEMP_ROOT;
		//data.reserve_dir = RESERVE_ROOT;
		//data.update_root_folder = UPD_ROOT;
		data.retranslation_dir = RETR_ROOT;

		data.m_pathSubstitutionMap.push_back(std::make_pair<NoCaseString,NoCaseString>(L"%BaseFolder%", BASES_ROOT));
		data.m_pathSubstitutionMap.push_back(std::make_pair<NoCaseString,NoCaseString>(L"%Folder%", ROLLBACK_ROOT));
		data.m_pathSubstitutionMap.push_back(std::make_pair<NoCaseString,NoCaseString>(L"%UpdateRoot%", UPD_ROOT));
		data.m_pathSubstitutionMap.push_back(std::make_pair<NoCaseString,NoCaseString>(L"%DSKM%", DSKM_ROOT));
		data.m_pathSubstitutionMap.push_back(std::make_pair<NoCaseString,NoCaseString>(L"%DataFolder%", UPD_ROOT));

		data.self_region = L"<default>";
		data.product_lang.push_back( NoCaseString(L"<default>"));
		
		data.os.os = L"Windows";
		data.os.version = L"5.1";

		data.RetranslatedObjects.m_componentsToRetranslate.push_back (L"AVS");
		data.RetranslatedObjects.m_componentsToRetranslate.push_back (L"CORE");

		data.updates_retranslation = true;
		data.retranslate_only_listed_apps = true;
		data.connect_tmo = 30;

		data.m_sourceList.addSource (UST_UserURL, std::wstring(COLL_ROOT).append(L"\\Bases_1").c_str(), true, L"diffs");
		
		data.RetranslationListFlags.update_bases = true;
		data.RetranslationListFlags.automatically_apply_available_updates = false;
		data.RetranslationListFlags.apply_urgent_updates = false;

		data.UpdaterListFlags.update_bases = false;
		data.UpdaterListFlags.automatically_apply_available_updates = false;
		data.UpdaterListFlags.apply_urgent_updates = false;

		Callbacks cbacks;
		TestCallbackInterface testCallback (&testLog, &cbacks);
		TestJournalInterface testJournal;
		testCallback.SetUpdaterConfiguration (data);
		
		std::wcout<<L"updateRequired "<<data.updateRequired()<<L"\n";
		std::wcout<<L"retranslationRequired "<<data.retranslationRequired()<<L"\n";

		FileInfo u0607g ( L"u0607g.xml", L"index/6", FileInfo::index, true );
		u0607g.m_localPath = RETR_ROOT;
		testCallback.SetPrimaryIndex(u0607g);
		
		cbacks.bFlags[Callbacks::getPrimaryIndexWithEvent] = true;
		cbacks.bFlags[Callbacks::checkFilesToDownload] = true;
		CREATE_EVENTS(getPrimaryIndexWithEvent)
		CREATE_EVENTS(checkFilesToDownload)

		Updater upd (testCallback.m_dp, testCallback, testJournal, &testLog);
		HANDLE hDoUpdate;

		doUpdater do_upd(&upd, hDoUpdate);

//		//bug workaround
//		WAIT_FOR_READY_EVENT(getPrimaryIndexWithEvent)
//		testCallback.SetPrimaryIndex( FileInfo() );
//		SET_GOON_EVENT(getPrimaryIndexWithEvent);

		WAIT_FOR_READY_EVENT(getPrimaryIndexWithEvent)
		testCallback.SetPrimaryIndex( u0607g );
		QC_BOOST_CHECK_MESSAGE (testCallback.GetGpiRetrMode() == true, L"Incorrect getPrimaryIndex retr mode\n");
		SET_GOON_EVENT(getPrimaryIndexWithEvent);

		WAIT_FOR_READY_EVENT(checkFilesToDownload)
		TestCallbackInterface::_checkFilesToDownload& cfd = testCallback.GetCheckFilesToDownload();
		std::wcout<<cfd.m_matchFileList->size()<<L"\n";
		for (std::vector<FileInfo>::iterator it = cfd.m_matchFileList->begin();
				it!=cfd.m_matchFileList->end(); ++it)
				{
					std::wcout<<it->m_filename.m_value.toWideChar()<<L"\n";
				}

		QC_BOOST_CHECK_MESSAGE (cfd.retranslationMode, L"Retr mode is not set\n");

		QC_BOOST_REQUIRE_MESSAGE (cfd.m_matchFileList->size() == 7, L"Incorrect file list size\n");

		//cfd.result = KLUPD::CORE_CANCELLED;
		cfd.result = false;
		
		SET_GOON_EVENT(checkFilesToDownload)

		WaitForSingleObject (hDoUpdate, INFINITE);

		QC_BOOST_CHECK_MESSAGE (_helper::IsFoldersEqual(DIFFTOOL_ROOT,
			std::wstring(COLL_ROOT).append(L"\\Bases_NULL\\bases").c_str(),
			std::wstring (RETR_ROOT).append (L"\\bases")), L"Incorrect bases\n");
		QC_BOOST_CHECK_MESSAGE (_helper::IsFoldersEqual(DIFFTOOL_ROOT,
			std::wstring(COLL_ROOT).append(L"\\Bases_NULL\\data").c_str(),
			std::wstring (RETR_ROOT).append (L"\\index\\6")), L"Incorrect bases\n");

		QC_BOOST_CHECK_MESSAGE ( CORE_BASE_CHECK_FAILED == do_upd.GetError(), L"Wrong return code\n" );
	}
	
}



void TestSuite::TestRetrCheckIfFileOptional ()
{
	using namespace KLUPD;
	std::wcout<<L"TestRetrCheckIfFileOptional\n";
	Logger::Case _case (m_logger, m_testp, m_step, L"TestRetrCheckIfFileOptional");

	{
		std::wcout<<L"Optional component\n";

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
		data.m_pathSubstitutionMap.push_back(std::make_pair<NoCaseString,NoCaseString>(L"%DataFolder%", UPD_ROOT));

		data.self_region = L"<default>";
		data.product_lang.push_back( NoCaseString(L"<default>"));
		
		data.os.os = L"Windows";
		data.os.version = L"5.1";

		data.RetranslatedObjects.m_componentsToRetranslate.push_back (L"AVS");
		data.RetranslatedObjects.m_componentsToRetranslate.push_back (L"AH");
		data.RetranslatedObjects.m_componentsToRetranslate.push_back (L"CORE");

		data.updates_retranslation = true;
		data.retranslate_only_listed_apps = true;
		data.connect_tmo = 30;

		data.m_sourceList.addSource (UST_UserURL, std::wstring(COLL_ROOT).append(L"\\Bases_missed").c_str(), true, L"diffs");
		
		data.RetranslationListFlags.update_bases = true;
		data.RetranslationListFlags.automatically_apply_available_updates = false;
		data.RetranslationListFlags.apply_urgent_updates = false;

		data.UpdaterListFlags.update_bases = false;
		data.UpdaterListFlags.automatically_apply_available_updates = false;
		data.UpdaterListFlags.apply_urgent_updates = false;

		Callbacks cbacks;
		TestCallbackInterface testCallback (&testLog, &cbacks);
		TestJournalInterface testJournal;
		testCallback.SetUpdaterConfiguration (data);
		
		std::wcout<<L"updateRequired "<<data.updateRequired()<<L"\n";
		std::wcout<<L"retranslationRequired "<<data.retranslationRequired()<<L"\n";

		FileInfo u0607g ( L"u0607g.xml", L"index/6", FileInfo::index, true );
		u0607g.m_localPath = RETR_ROOT;
		testCallback.SetPrimaryIndex(u0607g);
		
		cbacks.bFlags[Callbacks::getPrimaryIndexWithEvent] = true;
		cbacks.bFlags[Callbacks::checkIfFileOptional] = true;
		cbacks.bFlags[Callbacks::componentRemovedFromUpdate] = true;
		CREATE_EVENTS(checkIfFileOptional)
		CREATE_EVENTS(getPrimaryIndexWithEvent)

		testCallback.SetOptionalFileAnswer(true);

		Updater upd (testCallback.m_dp, testCallback, testJournal, &testLog);
		HANDLE hDoUpdate;

		doUpdater do_upd(&upd, hDoUpdate);

		WAIT_FOR_READY_EVENT(getPrimaryIndexWithEvent)
		testCallback.SetPrimaryIndex( u0607g );
		QC_BOOST_CHECK_MESSAGE (testCallback.GetGpiRetrMode() == true, L"Incorrect getPrimaryIndex retr mode\n");
		SET_GOON_EVENT(getPrimaryIndexWithEvent);

		WAIT_FOR_READY_EVENT(checkIfFileOptional)
		QC_BOOST_CHECK_MESSAGE (testCallback.GetAbsentFile().m_filename == L"ah-0607g.xml", L"Incorrect absent filename\n");
		SET_GOON_EVENT(checkIfFileOptional)
		WAIT_FOR_READY_EVENT(checkIfFileOptional)
		QC_BOOST_CHECK_MESSAGE (testCallback.GetAbsentFile().m_filename == L"ids1.kdz", L"Incorrect absent filename\n");
		SET_GOON_EVENT(checkIfFileOptional)
		WAIT_FOR_READY_EVENT(checkIfFileOptional)
		QC_BOOST_CHECK_MESSAGE (testCallback.GetAbsentFile().m_filename == L"ids2.kdz", L"Incorrect absent filename\n");
		SET_GOON_EVENT(checkIfFileOptional)
		WAIT_FOR_READY_EVENT(checkIfFileOptional)
		QC_BOOST_CHECK_MESSAGE (testCallback.GetAbsentFile().m_filename == L"ids3.kdz", L"Incorrect absent filename\n");
		SET_GOON_EVENT(checkIfFileOptional)

		WaitForSingleObject (hDoUpdate, INFINITE);

		QC_BOOST_CHECK_MESSAGE ( CORE_RETRANSLATION_SUCCESSFUL == do_upd.GetError(), L"Wrong return code\n" );

		QC_BOOST_CHECK_MESSAGE (_helper::IsFoldersEqual(DIFFTOOL_ROOT,
			std::wstring(COLL_ROOT).append(L"\\Bases_missed\\bases\\av").c_str(),
			std::wstring(RETR_ROOT).append(L"\\bases\\av")), L"Incorrect av comp\n");
		QC_BOOST_CHECK_MESSAGE (_helper::IsFoldersEqual(DIFFTOOL_ROOT,
			std::wstring(COLL_ROOT).append(L"\\Bases_missed\\data").c_str(),
			std::wstring (RETR_ROOT).append (L"\\index\\6")), L"Incorrect index\n");
	}

	{
		std::wcout<<L"Mandatory component\n";

		TestLog testLog;

		_helper::DeleteDirectory(ROOT);

		KLUPD::createFolder (std::wstring (RETR_ROOT).append (L"\\index\\6").c_str(), &testLog);
		KLUPD::createFolder (std::wstring (RETR_ROOT).append (L"\\bases").c_str(), &testLog);

		_helper::CopyFolder(std::wstring(COLL_ROOT).append(L"\\Bases_NULL\\bases").c_str(),
			std::wstring (RETR_ROOT).append (L"\\bases").c_str());
		_helper::CloneFile (std::wstring(COLL_ROOT).append(L"\\Bases_NULL\\index\\6\\u0607g.xml").c_str(),
			std::wstring (RETR_ROOT).append (L"\\index\\6\\u0607g.xml"), false);

		UpdaterConfiguration data(1, 1, L"1");

		data.temporary_dir = TEMP_ROOT;
		//data.reserve_dir = RESERVE_ROOT;
		//data.update_root_folder = UPD_ROOT;
		data.retranslation_dir = RETR_ROOT;

		data.m_pathSubstitutionMap.push_back(std::make_pair<NoCaseString,NoCaseString>(L"%BaseFolder%", BASES_ROOT));
		data.m_pathSubstitutionMap.push_back(std::make_pair<NoCaseString,NoCaseString>(L"%Folder%", ROLLBACK_ROOT));
		data.m_pathSubstitutionMap.push_back(std::make_pair<NoCaseString,NoCaseString>(L"%UpdateRoot%", UPD_ROOT));
		data.m_pathSubstitutionMap.push_back(std::make_pair<NoCaseString,NoCaseString>(L"%DSKM%", DSKM_ROOT));
		data.m_pathSubstitutionMap.push_back(std::make_pair<NoCaseString,NoCaseString>(L"%DataFolder%", UPD_ROOT));

		data.self_region = L"<default>";
		data.product_lang.push_back( NoCaseString(L"<default>"));
		
		data.os.os = L"Windows";
		data.os.version = L"5.1";

		data.RetranslatedObjects.m_componentsToRetranslate.push_back (L"AVS");
		data.RetranslatedObjects.m_componentsToRetranslate.push_back (L"AH");
		data.RetranslatedObjects.m_componentsToRetranslate.push_back (L"CORE");

		data.updates_retranslation = true;
		data.retranslate_only_listed_apps = true;
		data.connect_tmo = 30;

		data.m_sourceList.addSource (UST_UserURL, std::wstring(COLL_ROOT).append(L"\\Bases_missed").c_str(), true, L"diffs");
		
		data.RetranslationListFlags.update_bases = true;
		data.RetranslationListFlags.automatically_apply_available_updates = false;
		data.RetranslationListFlags.apply_urgent_updates = false;

		data.UpdaterListFlags.update_bases = false;
		data.UpdaterListFlags.automatically_apply_available_updates = false;
		data.UpdaterListFlags.apply_urgent_updates = false;

		Callbacks cbacks;
		TestCallbackInterface testCallback (&testLog, &cbacks);
		TestJournalInterface testJournal;
		testCallback.SetUpdaterConfiguration (data);
		
		std::wcout<<L"updateRequired "<<data.updateRequired()<<L"\n";
		std::wcout<<L"retranslationRequired "<<data.retranslationRequired()<<L"\n";

		FileInfo u0607g ( L"u0607g.xml", L"index/6", FileInfo::index, true );
		u0607g.m_localPath = RETR_ROOT;
				
		cbacks.bFlags[Callbacks::getPrimaryIndexWithEvent] = true;
		cbacks.bFlags[Callbacks::checkIfFileOptional] = true;
		cbacks.bFlags[Callbacks::componentRemovedFromUpdate] = true;
		CREATE_EVENTS(checkIfFileOptional)
		CREATE_EVENTS(getPrimaryIndexWithEvent)

		testCallback.SetOptionalFileAnswer(false);

		Updater upd (testCallback.m_dp, testCallback, testJournal, &testLog);
		HANDLE hDoUpdate;

		doUpdater do_upd(&upd, hDoUpdate);

		WAIT_FOR_READY_EVENT(getPrimaryIndexWithEvent)
		testCallback.SetPrimaryIndex( u0607g );
		QC_BOOST_CHECK_MESSAGE (testCallback.GetGpiRetrMode() == true, L"Incorrect getPrimaryIndex retr mode\n");
		SET_GOON_EVENT(getPrimaryIndexWithEvent);

		WAIT_FOR_READY_EVENT(checkIfFileOptional)
		QC_BOOST_CHECK_MESSAGE (testCallback.GetAbsentFile().m_filename == L"ah-0607g.xml", L"Incorrect absent filename\n");
		SET_GOON_EVENT(checkIfFileOptional);

		WaitForSingleObject (hDoUpdate, INFINITE);

		QC_BOOST_CHECK_MESSAGE ( CORE_NoSuchFileOrDirectory == do_upd.GetError(), L"Wrong return code\n" );

		QC_BOOST_CHECK_MESSAGE (_helper::IsFoldersEqual(DIFFTOOL_ROOT,
			std::wstring(COLL_ROOT).append(L"\\Bases_NULL\\bases").c_str(),
			std::wstring (RETR_ROOT).append (L"\\bases")), L"Incorrect bases\n");
		QC_BOOST_CHECK_MESSAGE (_helper::IsFoldersEqual(DIFFTOOL_ROOT,
			std::wstring(COLL_ROOT).append(L"\\Bases_NULL\\data").c_str(),
			std::wstring (RETR_ROOT).append (L"\\index\\6")), L"Incorrect bases\n");
	}
}

void TestSuite::TestRetrDoubleDownloading ()
{	
	using namespace KLUPD;
	std::wcout<<L"TestRetrDoubleDownloading\n";
	Logger::Case _case (m_logger, m_testp, m_step, L"TestRetrDoubleDownloading");

	{
		std::wcout<<L"Master.xml version is the same\n";

		TestLog testLog;

		_helper::DeleteDirectory(ROOT);
		_helper::DeleteDirectory(BASES_MOD_ROOT);

		KLUPD::createFolder (BASES_MOD_ROOT, &testLog);
		_helper::CopyFolder(std::wstring(COLL_ROOT).append(L"\\double\\first").c_str(), BASES_MOD_ROOT);
		_helper::CloneFile(std::wstring(COLL_ROOT).append(L"\\double\\second\\bases\\av\\avc\\i386\\base103.avc").c_str(),
			std::wstring(BASES_MOD_ROOT).append(L"\\bases\\av\\avc\\i386\\base103.avc").c_str(), false);


		KLUPD::createFolder (std::wstring (RETR_ROOT).append (L"\\index\\6").c_str(), &testLog);
		KLUPD::createFolder (std::wstring (RETR_ROOT).append (L"\\bases").c_str(), &testLog);

		_helper::CopyFolder(std::wstring(COLL_ROOT).append(L"\\Bases_NULL\\bases").c_str(),
			std::wstring (RETR_ROOT).append (L"\\bases").c_str());
		_helper::CloneFile (std::wstring(COLL_ROOT).append(L"\\Bases_NULL\\index\\6\\u0607g.xml").c_str(),
			std::wstring (RETR_ROOT).append (L"\\index\\6\\u0607g.xml"), false);

		UpdaterConfiguration data(1, 1, L"1");

		data.temporary_dir = TEMP_ROOT;
		//data.reserve_dir = RESERVE_ROOT;
		data.retranslation_dir = RETR_ROOT;

		data.m_pathSubstitutionMap.push_back(std::make_pair<NoCaseString,NoCaseString>(L"%BaseFolder%", BASES_ROOT));
		data.m_pathSubstitutionMap.push_back(std::make_pair<NoCaseString,NoCaseString>(L"%Folder%", ROLLBACK_ROOT));
		data.m_pathSubstitutionMap.push_back(std::make_pair<NoCaseString,NoCaseString>(L"%UpdateRoot%", UPD_ROOT));
		data.m_pathSubstitutionMap.push_back(std::make_pair<NoCaseString,NoCaseString>(L"%DSKM%", DSKM_ROOT));
		data.m_pathSubstitutionMap.push_back(std::make_pair<NoCaseString,NoCaseString>(L"%DataFolder%", UPD_ROOT));

		data.self_region = L"<default>";
		data.product_lang.push_back( NoCaseString(L"<default>"));
		
		data.os.os = L"Windows";
		data.os.version = L"5.1";

		data.RetranslatedObjects.m_componentsToRetranslate.push_back (L"AVS");
		data.RetranslatedObjects.m_componentsToRetranslate.push_back (L"CORE");

		data.updates_retranslation = true;
		data.retranslate_only_listed_apps = true;
		data.connect_tmo = 30;

		data.m_sourceList.addSource (UST_UserURL, BASES_MOD_ROOT, true, L"diffs");
		
		data.RetranslationListFlags.update_bases = true;
		data.RetranslationListFlags.automatically_apply_available_updates = false;
		data.RetranslationListFlags.apply_urgent_updates = false;

		data.UpdaterListFlags.update_bases = false;
		data.UpdaterListFlags.automatically_apply_available_updates = false;
		data.UpdaterListFlags.apply_urgent_updates = false;

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
			std::wstring(COLL_ROOT).append(L"\\Bases_NULL\\bases").c_str(),
			std::wstring (RETR_ROOT).append (L"\\bases")), L"Incorrect bases\n");
		QC_BOOST_CHECK_MESSAGE (_helper::IsFoldersEqual(DIFFTOOL_ROOT,
			std::wstring(COLL_ROOT).append(L"\\Bases_NULL\\data").c_str(),
			std::wstring (RETR_ROOT).append (L"\\index\\6")), L"Incorrect bases\n");

		QC_BOOST_CHECK_MESSAGE ( KLUPD::CORE_INVALID_SIGNATURE == do_upd.GetError(), L"Wrong return code\n" );
	}
	
	{
		std::wcout<<L"Master.xml version is newer\n";

		TestLog testLog;

		_helper::DeleteDirectory(ROOT);
		_helper::DeleteDirectory(BASES_MOD_ROOT);

		KLUPD::createFolder (BASES_MOD_ROOT, &testLog);
		_helper::CopyFolder(std::wstring(COLL_ROOT).append(L"\\double\\first").c_str(), BASES_MOD_ROOT);
		_helper::CloneFile(std::wstring(COLL_ROOT).append(L"\\double\\second\\bases\\av\\avc\\i386\\base103.avc").c_str(),
			std::wstring(BASES_MOD_ROOT).append(L"\\bases\\av\\avc\\i386\\base103.avc").c_str(), false);


		KLUPD::createFolder (std::wstring (RETR_ROOT).append (L"\\index\\6").c_str(), &testLog);
		KLUPD::createFolder (std::wstring (RETR_ROOT).append (L"\\bases").c_str(), &testLog);

		_helper::CopyFolder(std::wstring(COLL_ROOT).append(L"\\Bases_NULL\\bases").c_str(),
			std::wstring (RETR_ROOT).append (L"\\bases").c_str());
		_helper::CloneFile (std::wstring(COLL_ROOT).append(L"\\Bases_NULL\\index\\6\\u0607g.xml").c_str(),
			std::wstring (RETR_ROOT).append (L"\\index\\6\\u0607g.xml"), false);


		UpdaterConfiguration data(1, 1, L"1");

		data.temporary_dir = TEMP_ROOT;
		//data.reserve_dir = RESERVE_ROOT;
		data.retranslation_dir = RETR_ROOT;

		data.m_pathSubstitutionMap.push_back(std::make_pair<NoCaseString,NoCaseString>(L"%BaseFolder%", BASES_ROOT));
		data.m_pathSubstitutionMap.push_back(std::make_pair<NoCaseString,NoCaseString>(L"%Folder%", ROLLBACK_ROOT));
		data.m_pathSubstitutionMap.push_back(std::make_pair<NoCaseString,NoCaseString>(L"%UpdateRoot%", UPD_ROOT));
		data.m_pathSubstitutionMap.push_back(std::make_pair<NoCaseString,NoCaseString>(L"%DSKM%", DSKM_ROOT));
		data.m_pathSubstitutionMap.push_back(std::make_pair<NoCaseString,NoCaseString>(L"%DataFolder%", UPD_ROOT));

		data.self_region = L"<default>";
		data.product_lang.push_back( NoCaseString(L"<default>"));
		
		data.os.os = L"Windows";
		data.os.version = L"5.1";

		data.RetranslatedObjects.m_componentsToRetranslate.push_back (L"AVS");
		data.RetranslatedObjects.m_componentsToRetranslate.push_back (L"CORE");

		data.updates_retranslation = true;
		data.retranslate_only_listed_apps = true;
		data.connect_tmo = 30;

		data.m_sourceList.addSource (UST_UserURL, BASES_MOD_ROOT, true, L"diffs");
		
		data.RetranslationListFlags.update_bases = true;
		data.RetranslationListFlags.automatically_apply_available_updates = false;
		data.RetranslationListFlags.apply_urgent_updates = false;

		data.UpdaterListFlags.update_bases = false;
		data.UpdaterListFlags.automatically_apply_available_updates = false;
		data.UpdaterListFlags.apply_urgent_updates = false;
		
		Callbacks cbacks;
		TestCallbackInterface testCallback (&testLog, &cbacks);
		TestJournalInterface testJournal;
		testCallback.SetUpdaterConfiguration (data);

		std::wcout<<L"updateRequired "<<data.updateRequired()<<L"\n";
		std::wcout<<L"retranslationRequired "<<data.retranslationRequired()<<L"\n";

		FileInfo u0607g ( L"u0607g.xml", L"index/6", FileInfo::index, true );
		u0607g.m_localPath = RETR_ROOT;
		testCallback.SetPrimaryIndex(u0607g);
		
		Updater upd (testCallback.m_dp, testCallback, testJournal, &testLog);
		HANDLE hDoUpdate;

		cbacks.bFlags[Callbacks::checkIfFileOptional] = true;
		cbacks.bFlags[Callbacks::getPrimaryIndex] = true;
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

		_helper::DeleteFile ( std::wstring(RETR_ROOT).append(L"\\bases\\av\\avc\\i386\\base100.avc").c_str() );
		QC_BOOST_CHECK_MESSAGE (_helper::IsFoldersEqual(DIFFTOOL_ROOT,
			std::wstring(COLL_ROOT).append(L"\\double\\second\\bases\\av").c_str(),
			std::wstring(RETR_ROOT).append(L"\\bases\\av")), L"Incorrect av comp\n");
		QC_BOOST_CHECK_MESSAGE (_helper::IsFoldersEqual(DIFFTOOL_ROOT,
			std::wstring(COLL_ROOT).append(L"\\double\\second\\bases\\blst").c_str(),
			std::wstring(RETR_ROOT).append(L"\\bases\\blst")), L"Incorrect blst comp\n");	
		QC_BOOST_CHECK_MESSAGE (_helper::IsFoldersEqual(DIFFTOOL_ROOT,
			std::wstring(COLL_ROOT).append(L"\\double\\data").c_str(),
			std::wstring (RETR_ROOT).append (L"\\index\\6")), L"Incorrect index\n");

		QC_BOOST_CHECK_MESSAGE ( KLUPD::CORE_RETRANSLATION_SUCCESSFUL == do_upd.GetError(), L"Wrong return code\n" );
	}
}


void TestSuite::TestRetrProcessReceivedFiles ()
{
	using namespace KLUPD;
	std::wcout<<L"TestRetrProcessReceivedFiles\n";
	Logger::Case _case (m_logger, m_testp, m_step, L"TestRetrProcessReceivedFiles");

	{
		TestLog testLog;

		_helper::DeleteDirectory(ROOT);

		KLUPD::createFolder (std::wstring (RETR_ROOT).append (L"\\index\\6").c_str(), &testLog);
		KLUPD::createFolder (std::wstring (RETR_ROOT).append (L"\\bases").c_str(), &testLog);

		_helper::CopyFolder(std::wstring(COLL_ROOT).append(L"\\Bases_NULL\\bases").c_str(),
			std::wstring (RETR_ROOT).append (L"\\bases").c_str());
		_helper::CloneFile (std::wstring(COLL_ROOT).append(L"\\Bases_NULL\\index\\6\\u0607g.xml").c_str(),
			std::wstring (RETR_ROOT).append (L"\\index\\6\\u0607g.xml"), false);

		UpdaterConfiguration data(1, 1, L"1");

		data.temporary_dir = TEMP_ROOT;
		//data.reserve_dir = RESERVE_ROOT;
		//data.update_root_folder = UPD_ROOT;
		data.retranslation_dir = RETR_ROOT;

		data.m_pathSubstitutionMap.push_back(std::make_pair<NoCaseString,NoCaseString>(L"%BaseFolder%", BASES_ROOT));
		data.m_pathSubstitutionMap.push_back(std::make_pair<NoCaseString,NoCaseString>(L"%Folder%", ROLLBACK_ROOT));
		data.m_pathSubstitutionMap.push_back(std::make_pair<NoCaseString,NoCaseString>(L"%UpdateRoot%", UPD_ROOT));
		data.m_pathSubstitutionMap.push_back(std::make_pair<NoCaseString,NoCaseString>(L"%DSKM%", DSKM_ROOT));
		data.m_pathSubstitutionMap.push_back(std::make_pair<NoCaseString,NoCaseString>(L"%DataFolder%", UPD_ROOT));

		data.self_region = L"<default>";
		data.product_lang.push_back( NoCaseString(L"<default>"));
		
		data.os.os = L"Windows";
		data.os.version = L"5.1";

		data.RetranslatedObjects.m_componentsToRetranslate.push_back (L"AVS");
		data.RetranslatedObjects.m_componentsToRetranslate.push_back (L"CORE");

		data.updates_retranslation = true;
		data.retranslate_only_listed_apps = true;
		data.connect_tmo = 30;

		data.m_sourceList.addSource (UST_UserURL, std::wstring(COLL_ROOT).append(L"\\Bases_1").c_str(), true, L"diffs");
		
		data.RetranslationListFlags.update_bases = true;
		data.RetranslationListFlags.automatically_apply_available_updates = false;
		data.RetranslationListFlags.apply_urgent_updates = false;

		data.UpdaterListFlags.update_bases = false;
		data.UpdaterListFlags.automatically_apply_available_updates = false;
		data.UpdaterListFlags.apply_urgent_updates = false;

		Callbacks cbacks;
		TestCallbackInterface testCallback (&testLog, &cbacks);
		TestJournalInterface testJournal;
		testCallback.SetUpdaterConfiguration (data);
		
		std::wcout<<L"updateRequired "<<data.updateRequired()<<L"\n";
		std::wcout<<L"retranslationRequired "<<data.retranslationRequired()<<L"\n";

		FileInfo u0607g ( L"u0607g.xml", L"index/6", FileInfo::index, true );
		u0607g.m_localPath = RETR_ROOT;
		
		Updater upd (testCallback.m_dp, testCallback, testJournal, &testLog);
		HANDLE hDoUpdate;

		cbacks.bFlags[Callbacks::processReceivedFiles] = true;
		cbacks.bFlags[Callbacks::getPrimaryIndexWithEvent] = true;
		CREATE_EVENTS(processReceivedFiles)
		CREATE_EVENTS(getPrimaryIndexWithEvent)

		doUpdater do_upd(&upd, hDoUpdate);

		WAIT_FOR_READY_EVENT(getPrimaryIndexWithEvent)
			testCallback.SetPrimaryIndex( u0607g );
		QC_BOOST_CHECK_MESSAGE (testCallback.GetGpiRetrMode() == true, L"Incorrect getPrimaryIndex retr mode\n");
		SET_GOON_EVENT(getPrimaryIndexWithEvent);

		WAIT_FOR_READY_EVENT(processReceivedFiles)
		TestCallbackInterface::_processReceivedFiles& prf = testCallback.GetProcessReceivedFiles();
		std::wcout<<prf.fv.size()<<L"\n";
		for (std::vector<FileInfo>::iterator it = prf.fv.begin();
				it!=prf.fv.end(); ++it)
				{
					std::wcout<<it->m_filename.m_value.toWideChar()<<L"\n";
				}

		QC_BOOST_CHECK_MESSAGE (prf.retranslationMode, L"Retr mode is not set\n");

		QC_BOOST_REQUIRE_MESSAGE (prf.fv.size() == 7, L"Incorrect file list size\n");
		
		QC_BOOST_CHECK_MESSAGE (prf.fv.at(3).m_filename == L"u0607g.xml", L"Incorrect file name\n");
		QC_BOOST_CHECK_MESSAGE (prf.fv.at(1).m_filename == L"av-i386-0607g.xml", L"Incorrect file name\n");
		QC_BOOST_CHECK_MESSAGE (prf.fv.at(4).m_filename == L"base101.avc", L"Incorrect file name\n");
		QC_BOOST_CHECK_MESSAGE (prf.fv.at(5).m_filename == L"base102.avc", L"Incorrect file name\n");
		QC_BOOST_CHECK_MESSAGE (prf.fv.at(6).m_filename == L"base103.avc", L"Incorrect file name\n");

		QC_BOOST_CHECK_MESSAGE (prf.fv.at(3).m_transactionInformation.m_changeStatus == FileInfo::modified, L"Incorrect transaction status\n");
		QC_BOOST_CHECK_MESSAGE (prf.fv.at(1).m_transactionInformation.m_changeStatus == FileInfo::modified, L"Incorrect transaction status\n");
		QC_BOOST_CHECK_MESSAGE (prf.fv.at(4).m_transactionInformation.m_changeStatus == FileInfo::unchanged, L"Incorrect transaction status\n");
		QC_BOOST_CHECK_MESSAGE (prf.fv.at(5).m_transactionInformation.m_changeStatus == FileInfo::modified, L"Incorrect transaction status\n");
		QC_BOOST_CHECK_MESSAGE (prf.fv.at(6).m_transactionInformation.m_changeStatus == FileInfo::added, L"Incorrect transaction status\n");
		
		SET_GOON_EVENT(processReceivedFiles);

		WaitForSingleObject (hDoUpdate, INFINITE);

		QC_BOOST_CHECK_MESSAGE ( CORE_RETRANSLATION_SUCCESSFUL == do_upd.GetError(), L"Wrong return code\n" );
	}
}

void TestSuite::TestRetrReplaceFileOnReboot ()
{
	using namespace KLUPD;
	std::wcout<<L"TestRetrReplaceFileOnReboot\n";
	Logger::Case _case (m_logger, m_testp, m_step, L"TestRetrReplaceFileOnReboot");
	
	{
		std::wcout<<L"Updater can remove file\n";
		TestLog testLog;

		_helper::DeleteDirectory(ROOT);

		KLUPD::createFolder (std::wstring (RETR_ROOT).append (L"\\index\\6").c_str(), &testLog);
		KLUPD::createFolder (std::wstring (RETR_ROOT).append (L"\\bases").c_str(), &testLog);

		_helper::CopyFolder(std::wstring(COLL_ROOT).append(L"\\Bases_NULL\\bases").c_str(),
			std::wstring (RETR_ROOT).append (L"\\bases").c_str());
		_helper::CloneFile (std::wstring(COLL_ROOT).append(L"\\Bases_NULL\\index\\6\\u0607g.xml").c_str(),
			std::wstring (RETR_ROOT).append (L"\\index\\6\\u0607g.xml"), false);

		UpdaterConfiguration data(1, 1, L"1");

		data.temporary_dir = TEMP_ROOT_ANOTHER_DRIVE;
		//data.reserve_dir = RESERVE_ROOT_ANOTHER_DRIVE;
		//data.update_root_folder = UPD_ROOT;
		data.retranslation_dir = RETR_ROOT;

		data.m_pathSubstitutionMap.push_back(std::make_pair<NoCaseString,NoCaseString>(L"%BaseFolder%", BASES_ROOT));
		data.m_pathSubstitutionMap.push_back(std::make_pair<NoCaseString,NoCaseString>(L"%Folder%", ROLLBACK_ROOT));
		data.m_pathSubstitutionMap.push_back(std::make_pair<NoCaseString,NoCaseString>(L"%UpdateRoot%", UPD_ROOT));
		data.m_pathSubstitutionMap.push_back(std::make_pair<NoCaseString,NoCaseString>(L"%DSKM%", DSKM_ROOT));
		data.m_pathSubstitutionMap.push_back(std::make_pair<NoCaseString,NoCaseString>(L"%DataFolder%", UPD_ROOT));

		data.self_region = L"<default>";
		data.product_lang.push_back( NoCaseString(L"<default>"));
		
		data.os.os = L"Windows";
		data.os.version = L"5.1";

		data.RetranslatedObjects.m_componentsToRetranslate.push_back (L"AVS");
		data.RetranslatedObjects.m_componentsToRetranslate.push_back (L"CORE");

		data.updates_retranslation = true;
		data.retranslate_only_listed_apps = true;
		data.connect_tmo = 30;

		data.m_sourceList.addSource (UST_UserURL, std::wstring(COLL_ROOT).append(L"\\Bases_1").c_str(), true, L"diffs");
		
		data.RetranslationListFlags.update_bases = true;
		data.RetranslationListFlags.automatically_apply_available_updates = false;
		data.RetranslationListFlags.apply_urgent_updates = false;

		data.UpdaterListFlags.update_bases = false;
		data.UpdaterListFlags.automatically_apply_available_updates = false;
		data.UpdaterListFlags.apply_urgent_updates = false;

		Callbacks cbacks;
		TestCallbackInterface testCallback (&testLog, &cbacks);
		TestJournalInterface testJournal;
		testCallback.SetUpdaterConfiguration (data);
		
		std::wcout<<L"updateRequired "<<data.updateRequired()<<L"\n";
		std::wcout<<L"retranslationRequired "<<data.retranslationRequired()<<L"\n";

		FileInfo u0607g ( L"u0607g.xml", L"index/6", FileInfo::index, true );
		u0607g.m_localPath = RETR_ROOT;
		testCallback.SetPrimaryIndex(u0607g);
		
		Updater upd (testCallback.m_dp, testCallback, testJournal, &testLog);
		HANDLE hDoUpdate;

		cbacks.bFlags[Callbacks::removeLockedFileRequest] = true;
		CREATE_EVENTS(removeLockedFileRequest)
		std::wstring file;

		{

			_helper::CloneFile( L"c:\\windows\\system32\\msvcrt.dll",
				std::wstring(RETR_ROOT).append(L"\\bases\\av\\avc\\i386\\base102.avc"),
								false);
			HMODULE hm = LoadLibrary ( std::wstring(RETR_ROOT).append(L"\\bases\\av\\avc\\i386\\base102.avc").c_str() );
			//_helper::AutoFile locked ( std::wstring(BASES_ROOT).append(L"\\base102.avc") );
			doUpdater do_upd(&upd, hDoUpdate);

			WAIT_FOR_READY_EVENT(removeLockedFileRequest)

			file = testCallback.GetRequestFileToRemove().toWideChar();

			QC_BOOST_CHECK_MESSAGE (file.find(L"base102.avc") != std::wstring::npos &&
					file.find(L"removeOnNextReboot") != std::wstring::npos, L"Incorrect file to replace\n");
			
			SET_GOON_EVENT(removeLockedFileRequest);

			WaitForSingleObject (hDoUpdate, INFINITE);

			QC_BOOST_CHECK_MESSAGE ( CORE_RETRANSLATION_SUCCESSFUL == do_upd.GetError(), L"Wrong return code\n" );
			
			FreeLibrary (hm);
		};

		_helper::DeleteFile (file);
		_helper::DeleteFile ( std::wstring(RETR_ROOT).append(L"\\bases\\av\\avc\\i386\\base100.avc") );
		QC_BOOST_CHECK_MESSAGE (_helper::IsFoldersEqual(DIFFTOOL_ROOT,
			std::wstring(COLL_ROOT).append(L"\\Bases_1\\bases\\av").c_str(),
			std::wstring(RETR_ROOT).append(L"\\bases\\av")), L"Incorrect av comp\n");	
		QC_BOOST_CHECK_MESSAGE (_helper::IsFoldersEqual(DIFFTOOL_ROOT,
			std::wstring(COLL_ROOT).append(L"\\Bases_1\\data").c_str(),
			std::wstring(RETR_ROOT).append(L"\\index\\6")), L"Incorrect index\n");

	}
	
	{
		std::wcout<<L"Updater cannot remove file\n";
		TestLog testLog;

		_helper::DeleteDirectory(ROOT);

		KLUPD::createFolder (std::wstring (RETR_ROOT).append (L"\\index\\6").c_str(), &testLog);
		KLUPD::createFolder (std::wstring (RETR_ROOT).append (L"\\bases").c_str(), &testLog);

		_helper::CopyFolder(std::wstring(COLL_ROOT).append(L"\\Bases_NULL\\bases").c_str(),
			std::wstring (RETR_ROOT).append (L"\\bases").c_str());
		_helper::CloneFile (std::wstring(COLL_ROOT).append(L"\\Bases_NULL\\index\\6\\u0607g.xml").c_str(),
			std::wstring (RETR_ROOT).append (L"\\index\\6\\u0607g.xml"), false);

		UpdaterConfiguration data(1, 1, L"1");

		data.temporary_dir = TEMP_ROOT_ANOTHER_DRIVE;
		//data.reserve_dir = RESERVE_ROOT_ANOTHER_DRIVE;
		//data.update_root_folder = UPD_ROOT;
		data.retranslation_dir = RETR_ROOT;

		data.m_pathSubstitutionMap.push_back(std::make_pair<NoCaseString,NoCaseString>(L"%BaseFolder%", BASES_ROOT));
		data.m_pathSubstitutionMap.push_back(std::make_pair<NoCaseString,NoCaseString>(L"%Folder%", ROLLBACK_ROOT));
		data.m_pathSubstitutionMap.push_back(std::make_pair<NoCaseString,NoCaseString>(L"%UpdateRoot%", UPD_ROOT));
		data.m_pathSubstitutionMap.push_back(std::make_pair<NoCaseString,NoCaseString>(L"%DSKM%", DSKM_ROOT));

		data.self_region = L"<default>";
		data.product_lang.push_back( NoCaseString(L"<default>"));
		
		data.os.os = L"Windows";
		data.os.version = L"5.1";

		data.RetranslatedObjects.m_componentsToRetranslate.push_back (L"AVS");
		data.RetranslatedObjects.m_componentsToRetranslate.push_back (L"CORE");

		data.updates_retranslation = true;
		data.retranslate_only_listed_apps = true;
		data.connect_tmo = 30;

		data.m_sourceList.addSource (UST_UserURL, std::wstring(COLL_ROOT).append(L"\\Bases_1").c_str(), true, L"diffs");
		
		data.RetranslationListFlags.update_bases = true;
		data.RetranslationListFlags.automatically_apply_available_updates = false;
		data.RetranslationListFlags.apply_urgent_updates = false;

		data.UpdaterListFlags.update_bases = false;
		data.UpdaterListFlags.automatically_apply_available_updates = false;
		data.UpdaterListFlags.apply_urgent_updates = false;

		Callbacks cbacks;
		TestCallbackInterface testCallback (&testLog, &cbacks);
		TestJournalInterface testJournal;
		testCallback.SetUpdaterConfiguration (data);
		
		std::wcout<<L"updateRequired "<<data.updateRequired()<<L"\n";
		std::wcout<<L"retranslationRequired "<<data.retranslationRequired()<<L"\n";

		FileInfo u0607g ( L"u0607g.xml", L"index/6", FileInfo::index, true );
		u0607g.m_localPath = RETR_ROOT;
		testCallback.SetPrimaryIndex(u0607g);
		
		Updater upd (testCallback.m_dp, testCallback, testJournal, &testLog);
		HANDLE hDoUpdate;

		{

			_helper::AutoFile locked ( std::wstring(RETR_ROOT).append(L"\\bases\\av\\avc\\i386\\base102.avc") );
			doUpdater do_upd(&upd, hDoUpdate);

			WaitForSingleObject (hDoUpdate, INFINITE);

			QC_BOOST_CHECK_MESSAGE ( /*CORE_FILE_OPERATION_FAILURE*/
							CORE_NotEnoughPermissions == do_upd.GetError(), L"Wrong return code\n" );
		};

		QC_BOOST_CHECK_MESSAGE (_helper::IsFoldersEqual(DIFFTOOL_ROOT,
			std::wstring(COLL_ROOT).append(L"\\Bases_NULL\\bases").c_str(),
			std::wstring(RETR_ROOT).append(L"\\bases")), L"Incorrect bases\n");	
		QC_BOOST_CHECK_MESSAGE (_helper::IsFoldersEqual(DIFFTOOL_ROOT,
			std::wstring(COLL_ROOT).append(L"\\Bases_NULL\\data").c_str(),
			std::wstring(RETR_ROOT).append(L"\\index\\6")), L"Incorrect index\n");

	}
	
}

void TestSuite::TestRetrLocalIndexes ()
{
	using namespace KLUPD;
	std::wcout<<L"TestRetrLocalIndexes\n";
	Logger::Case _case (m_logger, m_testp, m_step, L"TestRetrLocalIndexes");

	{
		std::wcout<<L"Providing main index\n";

		TestLog testLog;

		_helper::DeleteDirectory(ROOT);

		KLUPD::createFolder (std::wstring (RETR_ROOT).append (L"\\index\\6").c_str(), &testLog);
		KLUPD::createFolder (std::wstring (RETR_ROOT).append (L"\\bases").c_str(), &testLog);

		_helper::CopyFolder(std::wstring(COLL_ROOT).append(L"\\Bases_NULL\\bases").c_str(),
			std::wstring (RETR_ROOT).append (L"\\bases").c_str());
		_helper::CloneFile (std::wstring(COLL_ROOT).append(L"\\Bases_NULL\\index\\6\\u0607g.xml").c_str(),
			std::wstring (RETR_ROOT).append (L"\\index\\6\\u0607g.xml"), false);

		UpdaterConfiguration data(1, 1, L"1");

		data.temporary_dir = TEMP_ROOT;
		//data.reserve_dir = RESERVE_ROOT;
		//data.update_root_folder = UPD_ROOT;
		data.retranslation_dir = RETR_ROOT;

		data.m_pathSubstitutionMap.push_back(std::make_pair<NoCaseString,NoCaseString>(L"%BaseFolder%", BASES_ROOT));
		data.m_pathSubstitutionMap.push_back(std::make_pair<NoCaseString,NoCaseString>(L"%Folder%", ROLLBACK_ROOT));
		data.m_pathSubstitutionMap.push_back(std::make_pair<NoCaseString,NoCaseString>(L"%UpdateRoot%", UPD_ROOT));
		data.m_pathSubstitutionMap.push_back(std::make_pair<NoCaseString,NoCaseString>(L"%DSKM%", DSKM_ROOT));
		data.m_pathSubstitutionMap.push_back(std::make_pair<NoCaseString,NoCaseString>(L"%DataFolder%", UPD_ROOT));

		data.self_region = L"<default>";
		data.product_lang.push_back( NoCaseString(L"<default>"));
		
		data.os.os = L"Windows";
		data.os.version = L"5.1";

		data.RetranslatedObjects.m_componentsToRetranslate.push_back (L"AVS");
		data.RetranslatedObjects.m_componentsToRetranslate.push_back (L"CORE");

		data.updates_retranslation = true;
		data.retranslate_only_listed_apps = true;
		data.connect_tmo = 30;

		data.m_sourceList.addSource (UST_UserURL, std::wstring(COLL_ROOT).append(L"\\Bases_1").c_str(), true, L"diffs");
		
		data.RetranslationListFlags.update_bases = true;
		data.RetranslationListFlags.automatically_apply_available_updates = false;
		data.RetranslationListFlags.apply_urgent_updates = false;

		data.UpdaterListFlags.update_bases = false;
		data.UpdaterListFlags.automatically_apply_available_updates = false;
		data.UpdaterListFlags.apply_urgent_updates = false;

		Callbacks cbacks;
		TestCallbackInterface testCallback (&testLog, &cbacks);
		TestJournalInterface testJournal;
		testCallback.SetUpdaterConfiguration (data);

		std::wcout<<L"updateRequired "<<data.updateRequired()<<L"\n";
		std::wcout<<L"retranslationRequired "<<data.retranslationRequired()<<L"\n";

		FileInfo u0607g ( L"u0607g.xml", L"index/6", FileInfo::index, true );
		u0607g.m_localPath = RETR_ROOT;
		testCallback.SetPrimaryIndex(u0607g);
		
		Updater upd (testCallback.m_dp, testCallback, testJournal, &testLog);
		HANDLE hDoUpdate;

		cbacks.bFlags[Callbacks::getPrimaryIndex] = true;

		doUpdater do_upd(&upd, hDoUpdate);

		WaitForSingleObject (hDoUpdate, INFINITE);

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
		QC_BOOST_CHECK_MESSAGE (_helper::IsFoldersEqual(DIFFTOOL_ROOT,
			std::wstring(COLL_ROOT).append(L"\\backup_NULL_to_1").c_str(),
			std::wstring (TEMP_ROOT).append(L"\\retranslation\\rollback")), L"Incorrect backup folder\n");


		QC_BOOST_CHECK_MESSAGE ( CORE_RETRANSLATION_SUCCESSFUL == do_upd.GetError(), L"Wrong return code\n" );
	}

	/*{no longer used
		std::wcout<<L"Providing substituted indexes\n";

		TestLog testLog;

		_helper::DeleteDirectory(ROOT);

		KLUPD::createFolder (std::wstring (RETR_ROOT).append (L"\\index\\6").c_str(), &testLog);
		KLUPD::createFolder (std::wstring (RETR_ROOT).append (L"\\bases").c_str(), &testLog);

		_helper::CopyFolder(std::wstring(COLL_ROOT).append(L"\\Bases_NULL_ki\\bases").c_str(),
			std::wstring (RETR_ROOT).append (L"\\bases").c_str());
	//	_helper::CloneFile (std::wstring(COLL_ROOT).append(L"\\Bases_NULL_ki\\index\\6\\u0607g.xml").c_str(),
	//		std::wstring (RETR_ROOT).append (L"\\index\\6\\u0607g.xml"), false);

		UpdaterConfiguration data(1, 1, L"1");

		data.temporary_dir = TEMP_ROOT;
		//data.reserve_dir = RESERVE_ROOT;
		//data.update_root_folder = UPD_ROOT;
		data.retranslation_dir = RETR_ROOT;

		data.m_pathSubstitutionMap.push_back(std::make_pair<NoCaseString,NoCaseString>(L"%BaseFolder%", BASES_ROOT));
		data.m_pathSubstitutionMap.push_back(std::make_pair<NoCaseString,NoCaseString>(L"%Folder%", ROLLBACK_ROOT));
		data.m_pathSubstitutionMap.push_back(std::make_pair<NoCaseString,NoCaseString>(L"%UpdateRoot%", UPD_ROOT));
		data.m_pathSubstitutionMap.push_back(std::make_pair<NoCaseString,NoCaseString>(L"%DSKM%", DSKM_ROOT));
		data.m_pathSubstitutionMap.push_back(std::make_pair<NoCaseString,NoCaseString>(L"%DataFolder%", UPD_ROOT));

		data.self_region = L"<default>";
		data.product_lang.push_back( NoCaseString(L"<default>"));
		
		data.os.os = L"Windows";
		data.os.version = L"5.1";

		data.RetranslatedObjects.m_componentsToRetranslate.push_back (L"AVS");
		data.RetranslatedObjects.m_componentsToRetranslate.push_back (L"CORE");

		data.updates_retranslation = true;
		data.retranslate_only_listed_apps = true;
		data.connect_tmo = 30;

		data.m_sourceList.addSource (UST_UserURL, std::wstring(COLL_ROOT).append(L"\\Bases_1").c_str(), true, L"diffs");
		
		data.RetranslationListFlags.update_bases = true;
		data.RetranslationListFlags.automatically_apply_available_updates = false;
		data.RetranslationListFlags.apply_urgent_updates = false;

		data.UpdaterListFlags.update_bases = false;
		data.UpdaterListFlags.automatically_apply_available_updates = false;
		data.UpdaterListFlags.apply_urgent_updates = false;

		Callbacks cbacks;
		TestCallbackInterface testCallback (&testLog, &cbacks);
		TestJournalInterface testJournal;
		testCallback.SetUpdaterConfiguration (data);

		std::wcout<<L"updateRequired "<<data.updateRequired()<<L"\n";
		std::wcout<<L"retranslationRequired "<<data.retranslationRequired()<<L"\n";

		FileInfo u0607g ( L"u0607g.xml", L"index/6", FileInfo::index, true );
		u0607g.m_localPath = RETR_ROOT;
		testCallback.SetPrimaryIndex(u0607g);
		
		Updater upd (testCallback.m_dp, testCallback, testJournal, &testLog);
		HANDLE hDoUpdate;

		cbacks.bFlags[Callbacks::getPrimaryIndex] = true;
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

		//WAIT_FOR_READY_EVENT(getSubstitutedIndex)
		//SET_GOON_EVENT(getSubstitutedIndex);

		WaitForSingleObject (hDoUpdate, INFINITE);

		_helper::CloneFile ( std::wstring(COLL_ROOT).append(L"\\backup_NULL_ki_to_1\\u0607g.xml").c_str(),
							std::wstring (TEMP_ROOT).append(L"\\rollback\\u0607g.xml"), false );
		
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
		QC_BOOST_CHECK_MESSAGE (_helper::IsFoldersEqual(DIFFTOOL_ROOT,
			std::wstring(COLL_ROOT).append(L"\\backup_NULL_ki_to_1_retr").c_str(),
			std::wstring (TEMP_ROOT).append(L"\\retranslation\\rollback")), L"Incorrect backup folder\n");

		QC_BOOST_CHECK_MESSAGE ( CORE_RETRANSLATION_SUCCESSFUL == do_upd.GetError(), L"Wrong return code\n" );
	}*/

	{
		std::wcout<<L"Providing neither primary nor substituted indexes\n";

		TestLog testLog;

		_helper::DeleteDirectory(ROOT);

		KLUPD::createFolder (std::wstring (RETR_ROOT).append (L"\\index\\6").c_str(), &testLog);
		KLUPD::createFolder (std::wstring (RETR_ROOT).append (L"\\bases").c_str(), &testLog);

		_helper::CopyFolder(std::wstring(COLL_ROOT).append(L"\\Bases_NULL_ki\\bases").c_str(),
			std::wstring (RETR_ROOT).append (L"\\bases").c_str());
	//	_helper::CloneFile (std::wstring(COLL_ROOT).append(L"\\Bases_NULL_ki\\index\\6\\u0607g.xml").c_str(),
	//		std::wstring (RETR_ROOT).append (L"\\index\\6\\u0607g.xml"), false);

		UpdaterConfiguration data(1, 1, L"1");

		data.temporary_dir = TEMP_ROOT;
		//data.reserve_dir = RESERVE_ROOT;
		//data.update_root_folder = UPD_ROOT;
		data.retranslation_dir = RETR_ROOT;

		data.m_pathSubstitutionMap.push_back(std::make_pair<NoCaseString,NoCaseString>(L"%BaseFolder%", BASES_ROOT));
		data.m_pathSubstitutionMap.push_back(std::make_pair<NoCaseString,NoCaseString>(L"%Folder%", ROLLBACK_ROOT));
		data.m_pathSubstitutionMap.push_back(std::make_pair<NoCaseString,NoCaseString>(L"%UpdateRoot%", UPD_ROOT));
		data.m_pathSubstitutionMap.push_back(std::make_pair<NoCaseString,NoCaseString>(L"%DSKM%", DSKM_ROOT));
		data.m_pathSubstitutionMap.push_back(std::make_pair<NoCaseString,NoCaseString>(L"%DataFolder%", UPD_ROOT));

		data.self_region = L"<default>";
		data.product_lang.push_back( NoCaseString(L"<default>"));
		
		data.os.os = L"Windows";
		data.os.version = L"5.1";

		data.RetranslatedObjects.m_componentsToRetranslate.push_back (L"AVS");
		data.RetranslatedObjects.m_componentsToRetranslate.push_back (L"CORE");

		data.updates_retranslation = true;
		data.retranslate_only_listed_apps = true;
		data.connect_tmo = 30;

		data.m_sourceList.addSource (UST_UserURL, std::wstring(COLL_ROOT).append(L"\\Bases_1").c_str(), true, L"diffs");
		
		data.RetranslationListFlags.update_bases = true;
		data.RetranslationListFlags.automatically_apply_available_updates = false;
		data.RetranslationListFlags.apply_urgent_updates = false;

		data.UpdaterListFlags.update_bases = false;
		data.UpdaterListFlags.automatically_apply_available_updates = false;
		data.UpdaterListFlags.apply_urgent_updates = false;

		Callbacks cbacks;
		TestCallbackInterface testCallback (&testLog, &cbacks);
		TestJournalInterface testJournal;
		testCallback.SetUpdaterConfiguration (data);

		std::wcout<<L"updateRequired "<<data.updateRequired()<<L"\n";
		std::wcout<<L"retranslationRequired "<<data.retranslationRequired()<<L"\n";

		FileInfo u0607g ( L"u0607g.xml", L"index/6", FileInfo::index, true );
		u0607g.m_localPath = RETR_ROOT;
		testCallback.SetPrimaryIndex(u0607g);
		
		Updater upd (testCallback.m_dp, testCallback, testJournal, &testLog);
		HANDLE hDoUpdate;

		cbacks.bFlags[Callbacks::getPrimaryIndex] = true;
		cbacks.bFlags[Callbacks::getSubstitutedIndex] = true;
		CREATE_EVENTS(getSubstitutedIndex)

		doUpdater do_upd(&upd, hDoUpdate);
/*
		WAIT_FOR_READY_EVENT(getSubstitutedIndex)

		FileVector fv;
		fv.push_back( FileInfo (L"av-i386-0607g.xml", L"bases/av/avc/i386/", FileInfo::index, true) );
		//fv.back().m_originalLocalPath = BASES_ROOT;
		fv.push_back( FileInfo (L"blst-0607g.xml", L"bases/blst/", FileInfo::index, true) );
		//fv.back().m_originalLocalPath = BASES_ROOT;

		testCallback.SetSubstitutedIndexes (fv);
	
		SET_GOON_EVENT(getSubstitutedIndex);
*/
		//WAIT_FOR_READY_EVENT(getSubstitutedIndex)
		//SET_GOON_EVENT(getSubstitutedIndex);

		WaitForSingleObject (hDoUpdate, INFINITE);

		_helper::DeleteFile( std::wstring (BASES_ROOT).append(L"\\base100.avc") );

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
		
		//no sense
		//QC_BOOST_CHECK_MESSAGE (_helper::IsFoldersEqual(DIFFTOOL_ROOT,
		//	std::wstring(COLL_ROOT).append(L"\\backup_NULL_ki_to_1_retr").c_str(),
		//	std::wstring (TEMP_ROOT).append(L"\\retranslation\\rollback")), L"Incorrect backup folder\n");

		QC_BOOST_CHECK_MESSAGE ( CORE_RETRANSLATION_SUCCESSFUL == do_upd.GetError(), L"Wrong return code\n" );
	}
	
}

void TestSuite::TestRetrRollback()
{
	using namespace KLUPD;
	std::wcout<<L"TestRetrRollback\n";
	Logger::Case _case (m_logger, m_testp, m_step, L"TestRetrRollback");

	//QC_BOOST_REQUIRE_MESSAGE (false, L"Bug 18266\n");

	//Update
	{
		std::wcout<<L"Retranslation\n";
		TestLog testLog;

		_helper::DeleteDirectory(ROOT);

		KLUPD::createFolder (std::wstring (RETR_ROOT).append (L"\\index\\6").c_str(), &testLog);
		KLUPD::createFolder (std::wstring (RETR_ROOT).append (L"\\bases").c_str(), &testLog);

		_helper::CopyFolder(std::wstring(COLL_ROOT).append(L"\\Bases_NULL_NEW\\bases").c_str(),
			std::wstring (RETR_ROOT).append (L"\\bases").c_str());
		_helper::CloneFile (std::wstring(COLL_ROOT).append(L"\\Bases_NULL_NEW\\index\\6\\u0607g.xml").c_str(),
			std::wstring (RETR_ROOT).append (L"\\index\\6\\u0607g.xml"), false);

		UpdaterConfiguration data(1, 1, L"1");

		data.temporary_dir = TEMP_ROOT;
		//data.reserve_dir = RESERVE_ROOT;
		//data.update_root_folder = UPD_ROOT;
		data.retranslation_dir = RETR_ROOT;
		
		data.m_pathSubstitutionMap.push_back(std::make_pair<NoCaseString,NoCaseString>(L"%BaseFolder%", BASES_ROOT));
		data.m_pathSubstitutionMap.push_back(std::make_pair<NoCaseString,NoCaseString>(L"%Folder%", ROLLBACK_ROOT));
		data.m_pathSubstitutionMap.push_back(std::make_pair<NoCaseString,NoCaseString>(L"%UpdateRoot%", UPD_ROOT));
		data.m_pathSubstitutionMap.push_back(std::make_pair<NoCaseString,NoCaseString>(L"%DSKM%", DSKM_ROOT));
		data.m_pathSubstitutionMap.push_back(std::make_pair<NoCaseString,NoCaseString>(L"%DataFolder%", UPD_ROOT));

		data.self_region = L"<default>";
		data.product_lang.push_back( NoCaseString(L"<default>"));
		
		data.os.os = L"Windows";
		data.os.version = L"5.1";

		data.RetranslatedObjects.m_componentsToRetranslate.push_back (L"AVS");
		data.RetranslatedObjects.m_componentsToRetranslate.push_back (L"CORE");

		data.updates_retranslation = true;
		data.retranslate_only_listed_apps = true;
		data.connect_tmo = 30;

		data.m_sourceList.addSource (UST_UserURL, std::wstring(COLL_ROOT).append(L"\\Bases_1").c_str(), true, L"diffs");
		
		data.RetranslationListFlags.update_bases = true;
		data.RetranslationListFlags.automatically_apply_available_updates = false;
		data.RetranslationListFlags.apply_urgent_updates = false;

		data.UpdaterListFlags.update_bases = false;
		data.UpdaterListFlags.automatically_apply_available_updates = false;
		data.UpdaterListFlags.apply_urgent_updates = false;

		Callbacks cbacks;
		TestCallbackInterface testCallback (&testLog, &cbacks);
		TestJournalInterface testJournal;
		testCallback.SetUpdaterConfiguration (data);
		
		FileInfo u0607g ( L"u0607g.xml", L"index/6", FileInfo::index, true );
		u0607g.m_localPath = RETR_ROOT;
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

		QC_BOOST_CHECK_MESSAGE ( CORE_RETRANSLATION_SUCCESSFUL == do_upd.GetError(), L"Wrong return code\n" );
	}

	//Rollback
	{
		std::wcout<<L"Rollback\n";
		TestLog testLog;

		UpdaterConfiguration data(1, 1, L"1");

		data.temporary_dir = TEMP_ROOT;
		//data.reserve_dir = std::wstring (TEMP_ROOT).append(L"\\rollback");
		//data.update_root_folder = UPD_ROOT;
		data.retranslation_dir = RETR_ROOT;
		
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
		
		//чтобы запустить откат ретрансляции
		data.updates_retranslation = true;
		data.RetranslationListFlags.update_bases = true;
		data.RetranslationListFlags.automatically_apply_available_updates = false;
		data.RetranslationListFlags.apply_urgent_updates = false;

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
			std::wstring(COLL_ROOT).append(L"\\Bases_NULL_NEW\\retr_rbacked").c_str(),
			std::wstring (RETR_ROOT)), L"Incorrect retranslation\n");
	}
	
}

void TestSuite::TestRetrRollbackListCases()
{
	using namespace KLUPD;
	std::wcout<<L"TestRetrRollbackListCases\n";
	Logger::Case _case (m_logger, m_testp, m_step, L"TestRetrRollbackListCases");

	{
		std::wcout<<L"Corrupted retranslation bases set\n";
		TestLog testLog;

		_helper::DeleteDirectory(ROOT);

		KLUPD::createFolder (std::wstring (RETR_ROOT).append (L"\\index\\6").c_str(), &testLog);
		KLUPD::createFolder (std::wstring (RETR_ROOT).append (L"\\bases").c_str(), &testLog);
		_helper::DeleteFile(ROLLBACK_FILE);
		_helper::CopyFolder(std::wstring(COLL_ROOT).append(L"\\Bases_NULL\\bases").c_str(),
			std::wstring (RETR_ROOT).append (L"\\bases").c_str());

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

		data.updates_retranslation = true;
		data.retranslate_only_listed_apps = true;
		data.connect_tmo = 30;

		data.m_sourceList.addSource (UST_UserURL, std::wstring(COLL_ROOT).append(L"\\Bases_1").c_str(), true, L"diffs");
		
		data.RetranslationListFlags.update_bases = true;
		data.RetranslationListFlags.automatically_apply_available_updates = false;
		data.RetranslationListFlags.apply_urgent_updates = false;

		data.UpdaterListFlags.update_bases = false;
		data.UpdaterListFlags.automatically_apply_available_updates = false;
		data.UpdaterListFlags.apply_urgent_updates = false;

		Callbacks cbacks;
		TestCallbackInterface testCallback (&testLog, &cbacks);
		TestJournalInterface testJournal;
		testCallback.SetUpdaterConfiguration (data);
		
		std::wcout<<L"updateRequired "<<data.updateRequired()<<L"\n";
		std::wcout<<L"retranslationRequired "<<data.retranslationRequired()<<L"\n";
		
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
		QC_BOOST_CHECK_MESSAGE ( CORE_RETRANSLATION_SUCCESSFUL == do_upd.GetError(), L"Wrong return code\n" );
	}
}


void TestSuite::TestRetrSourceIndexesDirectoried()
{
	using namespace KLUPD;
	std::wcout<<L"TestRetrSourceIndexesDirectoried\n";	
	Logger::Case _case (m_logger, m_testp, m_step, L"TestRetrSourceIndexesDirectoried");

	/*{no longer used
		std::wcout<<L"bug 17546\n";
		std::wcout<<L"Providing substituted indexes directoried structure for retranslation\n";
		QC_BOOST_REQUIRE_MESSAGE (false,L"");

		TestLog testLog;

		_helper::DeleteDirectory(ROOT);

		KLUPD::createFolder (std::wstring (RETR_ROOT).append (L"\\index\\6").c_str(), &testLog);
		KLUPD::createFolder (std::wstring (RETR_ROOT).append (L"\\bases").c_str(), &testLog);

		_helper::CopyFolder(std::wstring(COLL_ROOT).append(L"\\Bases_NULL\\bases").c_str(),
			std::wstring (RETR_ROOT).append (L"\\bases").c_str());
		_helper::CloneFile (std::wstring(COLL_ROOT).append(L"\\Bases_NULL\\index\\6\\u0607g.xml").c_str(),
			std::wstring (RETR_ROOT).append (L"\\index\\6\\u0607g.xml"), false);

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
		data.m_pathSubstitutionMap.push_back(std::make_pair<NoCaseString,NoCaseString>(L"%DataFolder%", DATA_ROOT));

		data.self_region = L"<default>";
		data.product_lang.push_back( NoCaseString(L"<default>"));
		
		data.os.os = L"Windows";
		data.os.version = L"5.1";

		data.RetranslatedObjects.m_componentsToRetranslate.push_back (L"AVS");
		data.RetranslatedObjects.m_componentsToRetranslate.push_back (L"CORE");

		data.updates_retranslation = true;
		data.retranslate_only_listed_apps = true;
		data.connect_tmo = 30;

		data.m_sourceList.addSource (UST_UserURL, std::wstring(COLL_ROOT).append(L"\\Bases_1_ki").c_str(), true, L"diffs");
		
		data.RetranslationListFlags.update_bases = true;
		data.RetranslationListFlags.automatically_apply_available_updates = false;
		data.RetranslationListFlags.apply_urgent_updates = false;

		data.UpdaterListFlags.update_bases = false;
		data.UpdaterListFlags.automatically_apply_available_updates = false;
		data.UpdaterListFlags.apply_urgent_updates = false;

		Callbacks cbacks;
		TestCallbackInterface testCallback (&testLog, &cbacks);
		TestJournalInterface testJournal;
		testCallback.SetUpdaterConfiguration (data);
		
		std::wcout<<L"updateRequired "<<data.updateRequired()<<L"\n";
		std::wcout<<L"retranslationRequired "<<data.retranslationRequired()<<L"\n";
		
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

//		_helper::CloneFile ( std::wstring(COLL_ROOT).append(L"\\backup_NULL_ki_to_1\\u0607g.xml").c_str(),
//							std::wstring(RESERVE_ROOT).append(L"\\u0607g.xml"), false );
//		QC_BOOST_CHECK_MESSAGE (_helper::IsFoldersEqual(DIFFTOOL_ROOT,
//			std::wstring(COLL_ROOT).append(L"\\backup_NULL_ki_to_1").c_str(), RESERVE_ROOT), L"Incorrect backup folder\n");
//		QC_BOOST_CHECK_MESSAGE (_helper::IsFoldersEqual(DIFFTOOL_ROOT,
//			std::wstring(COLL_ROOT).append(L"\\Bases_1\\plain").c_str(), BASES_ROOT), L"Incorrect bases\n");
//		QC_BOOST_CHECK_MESSAGE (_helper::IsFoldersEqual(DIFFTOOL_ROOT,
//			std::wstring(COLL_ROOT).append(L"\\Bases_1\\data").c_str(), UPD_ROOT), L"Incorrect index\n");

		QC_BOOST_CHECK_MESSAGE ( CORE_RETRANSLATION_SUCCESSFUL == do_upd.GetError(), L"Wrong return code\n" );
	}*/
}


void TestSuite::TestRetrSourceIndexesPlanar ()
{
	using namespace KLUPD;
	std::wcout<<L"TestRetrSourceIndexesPlanar\n";	
	Logger::Case _case (m_logger, m_testp, m_step, L"TestRetrSourceIndexesPlanar");

	{
		std::wcout<<L"Providing primary index planar structure\n";

		TestLog testLog;

		_helper::DeleteDirectory(ROOT);

		KLUPD::createFolder (std::wstring (RETR_ROOT).append (L"\\index\\6").c_str(), &testLog);
		KLUPD::createFolder (std::wstring (RETR_ROOT).append (L"\\bases").c_str(), &testLog);

		_helper::CopyFolder(std::wstring(COLL_ROOT).append(L"\\Bases_NULL\\bases").c_str(),
			std::wstring (RETR_ROOT).append (L"\\bases").c_str());
		_helper::CloneFile (std::wstring(COLL_ROOT).append(L"\\Bases_NULL\\index\\6\\u0607g.xml").c_str(),
			std::wstring (RETR_ROOT).append (L"\\index\\6\\u0607g.xml"), false);

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

		data.updates_retranslation = true;
		data.retranslate_only_listed_apps = true;
		data.connect_tmo = 30;

		data.m_sourceList.addSource (UST_UserURL, std::wstring(COLL_ROOT).append(L"\\Bases_1_planar").c_str(), true, L"diffs");
		
		data.RetranslationListFlags.update_bases = true;
		data.RetranslationListFlags.automatically_apply_available_updates = false;
		data.RetranslationListFlags.apply_urgent_updates = false;

		data.UpdaterListFlags.update_bases = false;
		data.UpdaterListFlags.automatically_apply_available_updates = false;
		data.UpdaterListFlags.apply_urgent_updates = false;

		Callbacks cbacks;
		TestCallbackInterface testCallback (&testLog, &cbacks);
		TestJournalInterface testJournal;
		testCallback.SetUpdaterConfiguration (data);
		
		std::wcout<<L"updateRequired "<<data.updateRequired()<<L"\n";
		std::wcout<<L"retranslationRequired "<<data.retranslationRequired()<<L"\n";

		FileInfo u0607g ( L"u0607g.xml", L"index/6", FileInfo::index, true );
		u0607g.m_localPath = RETR_ROOT;
		testCallback.SetPrimaryIndex(u0607g);
		
		Updater upd (testCallback.m_dp, testCallback, testJournal, &testLog);
		HANDLE hDoUpdate;

		cbacks.bFlags[Callbacks::getPrimaryIndex] = true;

		doUpdater do_upd(&upd, hDoUpdate);

		WaitForSingleObject (hDoUpdate, INFINITE);
		
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

		QC_BOOST_CHECK_MESSAGE ( CORE_RETRANSLATION_SUCCESSFUL == do_upd.GetError(), L"Wrong return code\n" );
	}

/*	{ no longer used
		std::wcout<<L"Providing substituted indexes planar structure\n";

		TestLog testLog;

		_helper::DeleteDirectory(ROOT);

		KLUPD::createFolder (std::wstring (RETR_ROOT).append (L"\\index\\6").c_str(), &testLog);
		KLUPD::createFolder (std::wstring (RETR_ROOT).append (L"\\bases").c_str(), &testLog);

		_helper::CopyFolder(std::wstring(COLL_ROOT).append(L"\\Bases_NULL\\bases").c_str(),
			std::wstring (RETR_ROOT).append (L"\\bases").c_str());
		_helper::CloneFile (std::wstring(COLL_ROOT).append(L"\\Bases_NULL\\index\\6\\u0607g.xml").c_str(),
			std::wstring (RETR_ROOT).append (L"\\index\\6\\u0607g.xml"), false);

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
		data.m_pathSubstitutionMap.push_back(std::make_pair<NoCaseString,NoCaseString>(L"%DataFolder%", DATA_ROOT));

		data.self_region = L"<default>";
		data.product_lang.push_back( NoCaseString(L"<default>"));
		
		data.os.os = L"Windows";
		data.os.version = L"5.1";

		data.RetranslatedObjects.m_componentsToRetranslate.push_back (L"AVS");
		data.RetranslatedObjects.m_componentsToRetranslate.push_back (L"CORE");

		data.updates_retranslation = true;
		data.retranslate_only_listed_apps = true;
		data.connect_tmo = 30;

		data.m_sourceList.addSource (UST_UserURL, std::wstring(COLL_ROOT).append(L"\\Bases_1_ki_planar").c_str(), true, L"diffs");
		
		data.RetranslationListFlags.update_bases = true;
		data.RetranslationListFlags.automatically_apply_available_updates = false;
		data.RetranslationListFlags.apply_urgent_updates = false;

		data.UpdaterListFlags.update_bases = false;
		data.UpdaterListFlags.automatically_apply_available_updates = false;
		data.UpdaterListFlags.apply_urgent_updates = false;

		Callbacks cbacks;
		TestCallbackInterface testCallback (&testLog, &cbacks);
		TestJournalInterface testJournal;
		testCallback.SetUpdaterConfiguration (data);
		
		std::wcout<<L"updateRequired "<<data.updateRequired()<<L"\n";
		std::wcout<<L"retranslationRequired "<<data.retranslationRequired()<<L"\n";
		
		Updater upd (testCallback.m_dp, testCallback, testJournal, &testLog);
		HANDLE hDoUpdate;

//		cbacks.bFlags[Callbacks::getSubstitutedIndex] = true;
//		CREATE_EVENTS(getSubstitutedIndex)

		doUpdater do_upd(&upd, hDoUpdate);

//		WAIT_FOR_READY_EVENT(getSubstitutedIndex)

		FileVector fv;
		fv.push_back( FileInfo (L"av-i386-0607g.xml", L"bases/av/avc/i386/", FileInfo::index, true) );
		fv.back().m_originalLocalPath = RETR_ROOT;
		fv.push_back( FileInfo (L"blst-0607g.xml", L"bases/blst/", FileInfo::index, true) );
		fv.back().m_originalLocalPath = RETR_ROOT;

//		testCallback.SetSubstitutedIndexes (fv);
	
//		SET_GOON_EVENT(getSubstitutedIndex);

		WaitForSingleObject (hDoUpdate, INFINITE);

//		QC_BOOST_CHECK_MESSAGE (_helper::IsFoldersEqual(DIFFTOOL_ROOT,
//			std::wstring(COLL_ROOT).append(L"\\backup_NULL_to_1").c_str(), RESERVE_ROOT), L"Incorrect backup folder\n");
//		QC_BOOST_CHECK_MESSAGE (_helper::IsFoldersEqual(DIFFTOOL_ROOT,
//			std::wstring(COLL_ROOT).append(L"\\Bases_1_ki_planar").c_str(), BASES_ROOT), L"Incorrect bases\n");
//		QC_BOOST_CHECK_MESSAGE (_helper::IsFoldersEqual(DIFFTOOL_ROOT,
//			std::wstring(COLL_ROOT).append(L"\\Bases_1\\data").c_str(), UPD_ROOT), L"Incorrect index\n");

		QC_BOOST_CHECK_MESSAGE ( CORE_RETRANSLATION_SUCCESSFUL == do_upd.GetError(), L"Wrong return code\n" );
	}*/

}

void TestSuite::TestRetrCheckDownloadedFiles ()
{
	using namespace KLUPD;
	std::wcout<<L"TestRetrCheckDownloadedFiles\n";
	Logger::Case _case (m_logger, m_testp, m_step, L"TestRetrCheckDownloadedFiles");

	{//normal check

		TestLog testLog;

		_helper::DeleteDirectory(ROOT);

		KLUPD::createFolder (std::wstring (RETR_ROOT).append (L"\\index\\6").c_str(), &testLog);
		KLUPD::createFolder (std::wstring (RETR_ROOT).append (L"\\bases").c_str(), &testLog);

		_helper::CopyFolder(std::wstring(COLL_ROOT).append(L"\\Bases_NULL\\bases").c_str(),
			std::wstring (RETR_ROOT).append (L"\\bases").c_str());
		_helper::CloneFile (std::wstring(COLL_ROOT).append(L"\\Bases_NULL\\index\\6\\u0607g.xml").c_str(),
			std::wstring (RETR_ROOT).append (L"\\index\\6\\u0607g.xml"), false);

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

		data.updates_retranslation = true;
		data.retranslate_only_listed_apps = true;
		data.connect_tmo = 30;

		data.m_sourceList.addSource (UST_UserURL, std::wstring(COLL_ROOT).append(L"\\Bases_1").c_str(), true, L"diffs");

		data.RetranslationListFlags.update_bases = true;
		data.RetranslationListFlags.automatically_apply_available_updates = false;
		data.RetranslationListFlags.apply_urgent_updates = false;

		data.UpdaterListFlags.update_bases = false;
		data.UpdaterListFlags.automatically_apply_available_updates = false;
		data.UpdaterListFlags.apply_urgent_updates = false;

		Callbacks cbacks;
		TestCallbackInterface testCallback (&testLog, &cbacks);
		TestJournalInterface testJournal;
		testCallback.SetUpdaterConfiguration (data);

		std::wcout<<L"updateRequired "<<data.updateRequired()<<L"\n";
		std::wcout<<L"retranslationRequired "<<data.retranslationRequired()<<L"\n";

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

		QC_BOOST_REQUIRE_MESSAGE (cdf.retranslationMode == true, L"Incorrect retranslation mode\n");
		
		QC_BOOST_REQUIRE_MESSAGE (cdf.fv.size() == 7, L"Incorrect file list size\n");

		QC_BOOST_CHECK_MESSAGE (cdf.fv.at(4).m_filename == L"base101.avc", L"Incorrect file name\n");
		QC_BOOST_CHECK_MESSAGE (cdf.fv.at(5).m_filename == L"base102.avc", L"Incorrect file name\n");
		QC_BOOST_CHECK_MESSAGE (cdf.fv.at(6).m_filename == L"base103.avc", L"Incorrect file name\n");
		//QC_BOOST_CHECK_MESSAGE (cdf.fv.at(9).m_filename == L"base100.avc", L"Incorrect file name\n");

		QC_BOOST_CHECK_MESSAGE (cdf.fv.at(4).m_transactionInformation.m_changeStatus == FileInfo::unchanged, L"Incorrect transaction status\n");
		QC_BOOST_CHECK_MESSAGE (cdf.fv.at(5).m_transactionInformation.m_changeStatus == FileInfo::modified, L"Incorrect transaction status\n");
		QC_BOOST_CHECK_MESSAGE (cdf.fv.at(6).m_transactionInformation.m_changeStatus == FileInfo::added, L"Incorrect transaction status\n");
		//QC_BOOST_CHECK_MESSAGE (cdf.fv.at(9).m_transactionInformation.m_changeStatus == FileInfo::deleted, L"Incorrect transaction status\n");

		SET_GOON_EVENT(checkDownloadedFiles);

		WaitForSingleObject (hDoUpdate, INFINITE);

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

		QC_BOOST_CHECK_MESSAGE ( CORE_RETRANSLATION_SUCCESSFUL == do_upd.GetError(), L"Wrong return code\n" );
	}

	{//return fail

		TestLog testLog;

		_helper::DeleteDirectory(ROOT);

		KLUPD::createFolder (std::wstring (RETR_ROOT).append (L"\\index\\6").c_str(), &testLog);
		KLUPD::createFolder (std::wstring (RETR_ROOT).append (L"\\bases").c_str(), &testLog);

		_helper::CopyFolder(std::wstring(COLL_ROOT).append(L"\\Bases_NULL\\bases").c_str(),
			std::wstring (RETR_ROOT).append (L"\\bases").c_str());
		_helper::CloneFile (std::wstring(COLL_ROOT).append(L"\\Bases_NULL\\index\\6\\u0607g.xml").c_str(),
			std::wstring (RETR_ROOT).append (L"\\index\\6\\u0607g.xml"), false);

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

		data.updates_retranslation = true;
		data.retranslate_only_listed_apps = true;
		data.connect_tmo = 30;

		data.m_sourceList.addSource (UST_UserURL, std::wstring(COLL_ROOT).append(L"\\Bases_1").c_str(), true, L"diffs");

		data.RetranslationListFlags.update_bases = true;
		data.RetranslationListFlags.automatically_apply_available_updates = false;
		data.RetranslationListFlags.apply_urgent_updates = false;

		data.UpdaterListFlags.update_bases = false;
		data.UpdaterListFlags.automatically_apply_available_updates = false;
		data.UpdaterListFlags.apply_urgent_updates = false;

		Callbacks cbacks;
		TestCallbackInterface testCallback (&testLog, &cbacks);
		TestJournalInterface testJournal;
		testCallback.SetUpdaterConfiguration (data);

		std::wcout<<L"updateRequired "<<data.updateRequired()<<L"\n";
		std::wcout<<L"retranslationRequired "<<data.retranslationRequired()<<L"\n";

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
			std::wstring(COLL_ROOT).append(L"\\Bases_NULL\\bases\\av").c_str(),
			std::wstring(RETR_ROOT).append(L"\\bases\\av")), L"Incorrect av comp\n");
		QC_BOOST_CHECK_MESSAGE (_helper::IsFoldersEqual(DIFFTOOL_ROOT,
			std::wstring(COLL_ROOT).append(L"\\Bases_NULL\\bases\\blst").c_str(),
			std::wstring(RETR_ROOT).append(L"\\bases\\blst")), L"Incorrect blst comp\n");	
		QC_BOOST_CHECK_MESSAGE (_helper::IsFoldersEqual(DIFFTOOL_ROOT,
			std::wstring(COLL_ROOT).append(L"\\Bases_NULL\\data").c_str(),
			std::wstring(RETR_ROOT).append(L"\\index\\6")), L"Incorrect index\n");

		QC_BOOST_CHECK_MESSAGE ( CORE_BASE_CHECK_FAILED == do_upd.GetError(), L"Wrong return code\n" );
	}
	
}