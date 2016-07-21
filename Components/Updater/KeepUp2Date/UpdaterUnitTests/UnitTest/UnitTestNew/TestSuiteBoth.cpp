#include "util.h"

#include "log.h"
#include "callback.h"
#include "journal.h"
#include "core/updater.h"
#include "helper/local_file.h"

#include "TestSuite.h"
#include "main.h"

void TestSuite::TestBothCheckIfFileOptional ()
{
	using namespace KLUPD;
	std::wcout<<L"TestBothCheckIfFileOptional\n";
	Logger::Case _case (m_logger, m_testp, m_step, L"TestBothCheckIfFileOptional");
	std::wcout<<L"Empty folder must exist\n";

	{
		std::wcout<<L"Both components are optional\n";

		TestLog testLog;

		_helper::DeleteDirectory(ROOT);

		UpdaterConfiguration data(1, 1, L"1");

		data.temporary_dir = TEMP_ROOT;
		////data.reserve_dir = RESERVE_ROOT;
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

		data.m_componentsToUpdate.push_back (L"AVS");
		data.m_componentsToUpdate.push_back (L"AH");
		data.m_componentsToUpdate.push_back (L"CORE");

		data.updates_retranslation = true;
		data.retranslate_only_listed_apps = true;
		data.connect_tmo = 30;

		data.m_sourceList.addSource (UST_UserURL, std::wstring(COLL_ROOT).append(L"\\Bases_missed").c_str(), true, L"diffs");
		
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
		
		std::wcout<<L"updateRequired "<<data.updateRequired()<<L"\n";
		std::wcout<<L"retranslationRequired "<<data.retranslationRequired()<<L"\n";

		FileInfo u0607g_retr ( L"u0607g.xml", L"index/6", FileInfo::index, true );
		u0607g_retr.m_localPath = RETR_ROOT;
		FileInfo u0607g_upd ( L"u0607g.xml", L"index/6", FileInfo::index, true );
		u0607g_upd.m_localPath = UPD_ROOT;
		
		cbacks.bFlags[Callbacks::getPrimaryIndexWithEvent] = true;
		cbacks.bFlags[Callbacks::checkIfFileOptional] = true;
		CREATE_EVENTS(checkIfFileOptional)
		CREATE_EVENTS(getPrimaryIndexWithEvent)

		testCallback.SetOptionalFileAnswer(true);

		Updater upd (testCallback.m_dp, testCallback, testJournal, &testLog);
		HANDLE hDoUpdate;

		doUpdater do_upd(&upd, hDoUpdate);

		for (int i = 1; i <= 2; ++i)
		{
			WAIT_FOR_READY_EVENT(getPrimaryIndexWithEvent)
			if ( testCallback.GetGpiRetrMode() ) testCallback.SetPrimaryIndex(u0607g_retr);
				else testCallback.SetPrimaryIndex(u0607g_upd);
			SET_GOON_EVENT(getPrimaryIndexWithEvent);
		}
		for (int i = 1; i <= 2; ++i)
		{
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
		}

		WaitForSingleObject (hDoUpdate, INFINITE);

		QC_BOOST_CHECK_MESSAGE ( CORE_NO_ERROR == do_upd.GetError(), L"Wrong return code\n" );

		QC_BOOST_CHECK_MESSAGE (_helper::IsFoldersEqual(DIFFTOOL_ROOT,
			std::wstring(COLL_ROOT).append(L"\\Bases_missed\\bases\\av").c_str(),
			std::wstring(RETR_ROOT).append(L"\\bases\\av")), L"Incorrect av comp\n");
		QC_BOOST_CHECK_MESSAGE (_helper::IsFoldersEqual(DIFFTOOL_ROOT,
			std::wstring(COLL_ROOT).append(L"\\Bases_missed\\data").c_str(),
			std::wstring (RETR_ROOT).append (L"\\index\\6")), L"Incorrect index\n");
		QC_BOOST_CHECK_MESSAGE (_helper::IsFoldersEqual(DIFFTOOL_ROOT,
			std::wstring(COLL_ROOT).append(L"\\Bases_missed\\plain").c_str(), BASES_ROOT), L"Incorrect bases\n");
		QC_BOOST_CHECK_MESSAGE (_helper::IsFoldersEqual(DIFFTOOL_ROOT,
			std::wstring(COLL_ROOT).append(L"\\Bases_missed\\data").c_str(), UPD_ROOT), L"Incorrect index\n");


		QC_BOOST_CHECK_MESSAGE (testJournal.GetUpdateResult() == CORE_NO_ERROR, L"Incorrect Update result");
		QC_BOOST_CHECK_MESSAGE (testJournal.GetRetranslationResult() == CORE_NO_ERROR, L"Incorrect Retranslation result");
	}

	{
		std::wcout<<L"Both components are mandatory\n";

		TestLog testLog;

		_helper::DeleteDirectory(ROOT);
		KLUPD::createFolder ( UPD_ROOT, &testLog );

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

		data.m_componentsToUpdate.push_back (L"AVS");
		data.m_componentsToUpdate.push_back (L"AH");
		data.m_componentsToUpdate.push_back (L"CORE");

		data.updates_retranslation = true;
		data.retranslate_only_listed_apps = true;
		data.connect_tmo = 30;

		data.m_sourceList.addSource (UST_UserURL, std::wstring(COLL_ROOT).append(L"\\Bases_missed").c_str(), true, L"diffs");
		
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
		
		std::wcout<<L"updateRequired "<<data.updateRequired()<<L"\n";
		std::wcout<<L"retranslationRequired "<<data.retranslationRequired()<<L"\n";

		FileInfo u0607g_retr ( L"u0607g.xml", L"index/6", FileInfo::index, true );
		u0607g_retr.m_localPath = RETR_ROOT;
		FileInfo u0607g_upd ( L"u0607g.xml", L"index/6", FileInfo::index, true );
		u0607g_upd.m_localPath = UPD_ROOT;
		
		cbacks.bFlags[Callbacks::getPrimaryIndexWithEvent] = true;
		cbacks.bFlags[Callbacks::checkIfFileOptional] = true;
		cbacks.bFlags[Callbacks::processReceivedFiles] = true;
		CREATE_EVENTS(checkIfFileOptional)
		CREATE_EVENTS(getPrimaryIndexWithEvent)
		CREATE_EVENTS(processReceivedFiles)

		testCallback.SetOptionalFileAnswer(false);

		Updater upd (testCallback.m_dp, testCallback, testJournal, &testLog);
		HANDLE hDoUpdate;

		doUpdater do_upd(&upd, hDoUpdate);

		for (int i = 1; i <= 2; ++i)
		{
			WAIT_FOR_READY_EVENT(getPrimaryIndexWithEvent)
			if ( testCallback.GetGpiRetrMode() ) testCallback.SetPrimaryIndex(u0607g_retr);
				else testCallback.SetPrimaryIndex(u0607g_upd);
			SET_GOON_EVENT(getPrimaryIndexWithEvent);
		}
		for (int i = 1; i <= 2; ++i)
		{
			WAIT_FOR_READY_EVENT(checkIfFileOptional)
			const FileInfo& absentFile = testCallback.GetAbsentFile();
			QC_BOOST_CHECK_MESSAGE (absentFile.m_filename == L"ah-0607g.xml", L"Incorrect absent filename\n");
			SET_GOON_EVENT(checkIfFileOptional);
		}

		WaitForSingleObject (hDoUpdate, INFINITE);

		QC_BOOST_CHECK_MESSAGE ( CORE_NoSuchFileOrDirectory == do_upd.GetError(), L"Wrong return code\n" );

		QC_BOOST_CHECK_MESSAGE (testJournal.GetUpdateResult() == CORE_NoSuchFileOrDirectory, L"Incorrect Update result");
		QC_BOOST_CHECK_MESSAGE (testJournal.GetRetranslationResult() == CORE_NoSuchFileOrDirectory, L"Incorrect Retranslation result");
	
	}

	{

		std::wcout<<L"Retranslated component is optional, updated is mandatory\n";

		TestLog testLog;

		_helper::DeleteDirectory(ROOT);
		KLUPD::createFolder ( BASES_ROOT, &testLog );

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

		data.m_componentsToUpdate.push_back (L"AVS");
		data.m_componentsToUpdate.push_back (L"AH");
		data.m_componentsToUpdate.push_back (L"CORE");

		data.updates_retranslation = true;
		data.retranslate_only_listed_apps = true;
		data.connect_tmo = 30;

		data.m_sourceList.addSource (UST_UserURL, std::wstring(COLL_ROOT).append(L"\\Bases_missed").c_str(), true, L"diffs");
		
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
		
		std::wcout<<L"updateRequired "<<data.updateRequired()<<L"\n";
		std::wcout<<L"retranslationRequired "<<data.retranslationRequired()<<L"\n";

		FileInfo u0607g_retr ( L"u0607g.xml", L"index/6", FileInfo::index, true );
		u0607g_retr.m_localPath = RETR_ROOT;
		FileInfo u0607g_upd ( L"u0607g.xml", L"index/6", FileInfo::index, true );
		u0607g_upd.m_localPath = UPD_ROOT;
		
		cbacks.bFlags[Callbacks::getPrimaryIndexWithEvent] = true;
		cbacks.bFlags[Callbacks::checkIfFileOptional] = true;
		CREATE_EVENTS(checkIfFileOptional)
		CREATE_EVENTS(getPrimaryIndexWithEvent)

		testCallback.SetOptionalFileAnswer(true);

		Updater upd (testCallback.m_dp, testCallback, testJournal, &testLog);
		HANDLE hDoUpdate;

		doUpdater do_upd(&upd, hDoUpdate);

		for (int i = 1; i <= 2; ++i)
		{
			WAIT_FOR_READY_EVENT(getPrimaryIndexWithEvent)
			if ( testCallback.GetGpiRetrMode() ) testCallback.SetPrimaryIndex(u0607g_retr);
				else testCallback.SetPrimaryIndex(u0607g_upd);
			SET_GOON_EVENT(getPrimaryIndexWithEvent);
		}

		for (int i = 1; i <= 2; ++i)
		{
			WAIT_FOR_READY_EVENT(checkIfFileOptional)

			if ( testCallback.GetCfoRetrMode() )
			{
				testCallback.SetOptionalFileAnswer(true);

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
			}

			if ( !testCallback.GetCfoRetrMode() )
			{
				testCallback.SetOptionalFileAnswer(false);

				QC_BOOST_CHECK_MESSAGE (testCallback.GetAbsentFile().m_filename == L"ah-0607g.xml", L"Incorrect absent filename\n");
				SET_GOON_EVENT(checkIfFileOptional);
			}
		}

		WaitForSingleObject (hDoUpdate, INFINITE);

		QC_BOOST_CHECK_MESSAGE ( CORE_NoSuchFileOrDirectory == do_upd.GetError(), L"Wrong return code\n" );

		QC_BOOST_CHECK_MESSAGE (_helper::IsFoldersEqual(DIFFTOOL_ROOT,
			std::wstring(COLL_ROOT).append(L"\\Bases_missed\\bases\\av").c_str(),
			std::wstring(RETR_ROOT).append(L"\\bases\\av")), L"Incorrect av comp\n");
		QC_BOOST_CHECK_MESSAGE (_helper::IsFoldersEqual(DIFFTOOL_ROOT,
			std::wstring(COLL_ROOT).append(L"\\Bases_missed\\data").c_str(),
			std::wstring (RETR_ROOT).append (L"\\index\\6")), L"Incorrect index\n");
		QC_BOOST_CHECK_MESSAGE (_helper::IsFoldersEqual(DIFFTOOL_ROOT,
			std::wstring(COLL_ROOT).append(L"\\Empty").c_str(), BASES_ROOT), L"Incorrect bases\n");
		QC_BOOST_CHECK_MESSAGE (_helper::IsFoldersEqual(DIFFTOOL_ROOT,
			std::wstring(COLL_ROOT).append(L"\\Empty").c_str(), UPD_ROOT), L"Incorrect index\n");

		QC_BOOST_CHECK_MESSAGE (testJournal.GetUpdateResult() == CORE_NoSuchFileOrDirectory, L"Incorrect Update result");
		QC_BOOST_CHECK_MESSAGE (testJournal.GetRetranslationResult() == CORE_NO_ERROR, L"Incorrect Retranslation result");
	
	}
	
	{
		std::wcout<<L"Retranslated component is mandatory, updated is optional\n";

		TestLog testLog;

		_helper::DeleteDirectory(ROOT);
		KLUPD::createFolder ( BASES_ROOT, &testLog );

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

		data.m_componentsToUpdate.push_back (L"AVS");
		data.m_componentsToUpdate.push_back (L"AH");
		data.m_componentsToUpdate.push_back (L"CORE");

		data.updates_retranslation = true;
		data.retranslate_only_listed_apps = true;
		data.connect_tmo = 30;

		data.m_sourceList.addSource (UST_UserURL, std::wstring(COLL_ROOT).append(L"\\Bases_missed").c_str(), true, L"diffs");
		
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
		
		std::wcout<<L"updateRequired "<<data.updateRequired()<<L"\n";
		std::wcout<<L"retranslationRequired "<<data.retranslationRequired()<<L"\n";

		FileInfo u0607g_retr ( L"u0607g.xml", L"index/6", FileInfo::index, true );
		u0607g_retr.m_localPath = RETR_ROOT;
		FileInfo u0607g_upd ( L"u0607g.xml", L"index/6", FileInfo::index, true );
		u0607g_upd.m_localPath = UPD_ROOT;
		
		cbacks.bFlags[Callbacks::getPrimaryIndexWithEvent] = true;
		cbacks.bFlags[Callbacks::checkIfFileOptional] = true;
		CREATE_EVENTS(checkIfFileOptional)
		CREATE_EVENTS(getPrimaryIndexWithEvent)

		testCallback.SetOptionalFileAnswer(true);

		Updater upd (testCallback.m_dp, testCallback, testJournal, &testLog);
		HANDLE hDoUpdate;

		doUpdater do_upd(&upd, hDoUpdate);

		for (int i = 1; i <= 2; ++i)
		{
			WAIT_FOR_READY_EVENT(getPrimaryIndexWithEvent)
			if ( testCallback.GetGpiRetrMode() ) testCallback.SetPrimaryIndex(u0607g_retr);
				else testCallback.SetPrimaryIndex(u0607g_upd);
			SET_GOON_EVENT(getPrimaryIndexWithEvent);
		}

		for (int i = 1; i <= 2; ++i)
		{
			WAIT_FOR_READY_EVENT(checkIfFileOptional)

			if ( !testCallback.GetCfoRetrMode() )
			{
				testCallback.SetOptionalFileAnswer(true);
				
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
			}

			if ( testCallback.GetCfoRetrMode() )
			{
				testCallback.SetOptionalFileAnswer(false);
				
				QC_BOOST_CHECK_MESSAGE (testCallback.GetAbsentFile().m_filename == L"ah-0607g.xml", L"Incorrect absent filename\n");
				SET_GOON_EVENT(checkIfFileOptional);
			}
		}

		WaitForSingleObject (hDoUpdate, INFINITE);

		QC_BOOST_CHECK_MESSAGE ( CORE_UpdateSuccessfulRetranslationFailed == do_upd.GetError(), L"Wrong return code\n" );

		QC_BOOST_CHECK_MESSAGE (_helper::IsFoldersEqual(DIFFTOOL_ROOT,
			std::wstring(COLL_ROOT).append(L"\\empty").c_str(), RETR_ROOT), L"Incorrect av comp\n");
		QC_BOOST_CHECK_MESSAGE (_helper::IsFoldersEqual(DIFFTOOL_ROOT,
			std::wstring(COLL_ROOT).append(L"\\Bases_missed\\plain").c_str(), BASES_ROOT), L"Incorrect bases\n");

		QC_BOOST_CHECK_MESSAGE (testJournal.GetUpdateResult() == CORE_NO_ERROR, L"Incorrect Update result");
		QC_BOOST_CHECK_MESSAGE (testJournal.GetRetranslationResult() == CORE_NoSuchFileOrDirectory, L"Incorrect Retranslation result");
	
	}
	
}

void TestSuite::TestBothCheckFilesToDownload ()
{
	using namespace KLUPD;
	std::wcout<<L"TestBothCheckFilesToDownload\n";
	Logger::Case _case (m_logger, m_testp, m_step, L"TestBothCheckFilesToDownload");

	{
		std::wcout<<L"Check transaction status\n";

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

		std::wcout<<L"updateRequired "<<data.updateRequired()<<L"\n";
		std::wcout<<L"retranslationRequired "<<data.retranslationRequired()<<L"\n";

		FileInfo u0607g_retr ( L"u0607g.xml", L"index/6", FileInfo::index, true );
		u0607g_retr.m_localPath = RETR_ROOT;
		FileInfo u0607g_upd ( L"u0607g.xml", L"index/6", FileInfo::index, true );
		u0607g_upd.m_localPath = UPD_ROOT;
		//testCallback.SetPrimaryIndex(u0607g);

		cbacks.bFlags[Callbacks::getPrimaryIndexWithEvent] = true;
		cbacks.bFlags[Callbacks::checkFilesToDownload] = true;
		CREATE_EVENTS(getPrimaryIndexWithEvent)
		CREATE_EVENTS(checkFilesToDownload)

		CDownloadProgress dp (&testCallback); Updater upd (dp, testCallback, testJournal, &testLog);
		HANDLE hDoUpdate;

		doUpdater do_upd(&upd, hDoUpdate);

		WAIT_FOR_READY_EVENT(getPrimaryIndexWithEvent)
			if ( testCallback.GetGpiRetrMode() ) testCallback.SetPrimaryIndex(u0607g_retr);
			else testCallback.SetPrimaryIndex(u0607g_upd);
			SET_GOON_EVENT(getPrimaryIndexWithEvent);
			WAIT_FOR_READY_EVENT(getPrimaryIndexWithEvent)
				if ( testCallback.GetGpiRetrMode() ) testCallback.SetPrimaryIndex(u0607g_retr);
				else testCallback.SetPrimaryIndex(u0607g_upd);
				SET_GOON_EVENT(getPrimaryIndexWithEvent);

				for (int i = 1; i<=2; ++i)
				{
					WAIT_FOR_READY_EVENT(checkFilesToDownload)
						TestCallbackInterface::_checkFilesToDownload& cfd = testCallback.GetCheckFilesToDownload();
					std::wcout<<cfd.m_matchFileList->size()<<L"\n";
					for (std::vector<FileInfo>::iterator it = cfd.m_matchFileList->begin();
						it!=cfd.m_matchFileList->end(); ++it)
					{
						std::wcout<<it->m_filename.m_value.toWideChar()<<L"\n";
					}

					if (cfd.retranslationMode)
					{

						QC_BOOST_REQUIRE_MESSAGE (cfd.m_matchFileList->size() == 7, L"Incorrect file list size\n");

						QC_BOOST_CHECK_MESSAGE (cfd.m_matchFileList->at(4).m_filename == L"base101.avc", L"Incorrect file name\n");
						QC_BOOST_CHECK_MESSAGE (cfd.m_matchFileList->at(5).m_filename == L"base102.avc", L"Incorrect file name\n");
						QC_BOOST_CHECK_MESSAGE (cfd.m_matchFileList->at(6).m_filename == L"base103.avc", L"Incorrect file name\n");
						//QC_BOOST_CHECK_MESSAGE (cfd.m_matchFileList->at(9).m_filename == L"base100.avc", L"Incorrect file name\n");

						QC_BOOST_CHECK_MESSAGE (cfd.m_matchFileList->at(4).m_transactionInformation.m_changeStatus == FileInfo::unchanged, L"Incorrect transaction status\n");
						QC_BOOST_CHECK_MESSAGE (cfd.m_matchFileList->at(5).m_transactionInformation.m_changeStatus == FileInfo::modified, L"Incorrect transaction status\n");
						QC_BOOST_CHECK_MESSAGE (cfd.m_matchFileList->at(6).m_transactionInformation.m_changeStatus == FileInfo::added, L"Incorrect transaction status\n");
						//QC_BOOST_CHECK_MESSAGE (cfd.m_matchFileList->at(9).m_transactionInformation.m_changeStatus == FileInfo::deleted, L"Incorrect transaction status\n");
					}
					else
					{
						QC_BOOST_REQUIRE_MESSAGE (cfd.m_matchFileList->size() == 11, L"Incorrect file list size\n");

						QC_BOOST_CHECK_MESSAGE (cfd.m_matchFileList->at(6).m_filename == L"base101.avc", L"Incorrect file name\n");
						QC_BOOST_CHECK_MESSAGE (cfd.m_matchFileList->at(7).m_filename == L"base102.avc", L"Incorrect file name\n");
						QC_BOOST_CHECK_MESSAGE (cfd.m_matchFileList->at(8).m_filename == L"base103.avc", L"Incorrect file name\n");
						//QC_BOOST_CHECK_MESSAGE (cfd.m_matchFileList->at(11).m_filename == L"base100.avc", L"Incorrect file name\n");

						QC_BOOST_CHECK_MESSAGE (cfd.m_matchFileList->at(6).m_transactionInformation.m_changeStatus == FileInfo::unchanged, L"Incorrect transaction status\n");
						QC_BOOST_CHECK_MESSAGE (cfd.m_matchFileList->at(7).m_transactionInformation.m_changeStatus == FileInfo::modified, L"Incorrect transaction status\n");
						QC_BOOST_CHECK_MESSAGE (cfd.m_matchFileList->at(8).m_transactionInformation.m_changeStatus == FileInfo::added, L"Incorrect transaction status\n");
						//QC_BOOST_CHECK_MESSAGE (cfd.m_matchFileList->at(11).m_transactionInformation.m_changeStatus == FileInfo::deleted, L"Incorrect transaction status\n");

					}

					SET_GOON_EVENT(checkFilesToDownload);
				}

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
				QC_BOOST_CHECK_MESSAGE (_helper::DeleteFile(std::wstring(BASES_ROOT).append (L"\\base100.avc")),
					L"old base file not found");
				QC_BOOST_CHECK_MESSAGE (_helper::IsFoldersEqual(DIFFTOOL_ROOT,
					std::wstring(COLL_ROOT).append(L"\\Bases_1\\plain").c_str(), BASES_ROOT), L"Incorrect bases\n");

				QC_BOOST_CHECK_MESSAGE (_helper::IsFoldersEqual(DIFFTOOL_ROOT,
					std::wstring(COLL_ROOT).append(L"\\Bases_1\\data").c_str(), UPD_ROOT), L"Incorrect index\n");


				QC_BOOST_CHECK_MESSAGE ( NO_ERROR == do_upd.GetError(), L"Wrong return code\n" );

	}

	{
		std::wcout<<L"Change transaction status in retranslation part\n";

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

		std::wcout<<L"updateRequired "<<data.updateRequired()<<L"\n";
		std::wcout<<L"retranslationRequired "<<data.retranslationRequired()<<L"\n";

		FileInfo u0607g_retr ( L"u0607g.xml", L"index/6", FileInfo::index, true );
		u0607g_retr.m_localPath = RETR_ROOT;
		FileInfo u0607g_upd ( L"u0607g.xml", L"index/6", FileInfo::index, true );
		u0607g_upd.m_localPath = UPD_ROOT;
		//testCallback.SetPrimaryIndex(u0607g);

		cbacks.bFlags[Callbacks::getPrimaryIndexWithEvent] = true;
		cbacks.bFlags[Callbacks::checkFilesToDownload] = true;
		CREATE_EVENTS(getPrimaryIndexWithEvent)
			CREATE_EVENTS(checkFilesToDownload)

			CDownloadProgress dp (&testCallback); Updater upd (dp, testCallback, testJournal, &testLog);
		HANDLE hDoUpdate;

		doUpdater do_upd(&upd, hDoUpdate);

		WAIT_FOR_READY_EVENT(getPrimaryIndexWithEvent)
			if ( testCallback.GetGpiRetrMode() ) testCallback.SetPrimaryIndex(u0607g_retr);
			else testCallback.SetPrimaryIndex(u0607g_upd);
			SET_GOON_EVENT(getPrimaryIndexWithEvent);
			WAIT_FOR_READY_EVENT(getPrimaryIndexWithEvent)
				if ( testCallback.GetGpiRetrMode() ) testCallback.SetPrimaryIndex(u0607g_retr);
				else testCallback.SetPrimaryIndex(u0607g_upd);
				SET_GOON_EVENT(getPrimaryIndexWithEvent);

				for (int i = 1; i<=2; ++i)
				{
					WAIT_FOR_READY_EVENT(checkFilesToDownload)
						TestCallbackInterface::_checkFilesToDownload& cfd = testCallback.GetCheckFilesToDownload();
					std::wcout<<cfd.m_matchFileList->size()<<L"\n";
					for (std::vector<FileInfo>::iterator it = cfd.m_matchFileList->begin();
						it!=cfd.m_matchFileList->end(); ++it)
					{
						std::wcout<<it->m_filename.m_value.toWideChar()<<L"\n";
					}

					if (cfd.retranslationMode)
					{

						QC_BOOST_REQUIRE_MESSAGE (cfd.m_matchFileList->size() == 7, L"Incorrect file list size\n");

						for (std::vector<FileInfo>::iterator it = cfd.m_matchFileList->begin();
							it!=cfd.m_matchFileList->end(); ++it)
							it->m_transactionInformation.m_changeStatus = FileInfo::unchanged;
					}
					else
					{
						QC_BOOST_REQUIRE_MESSAGE (cfd.m_matchFileList->size() == 11, L"Incorrect file list size\n");
					}

					SET_GOON_EVENT(checkFilesToDownload);
				}

				WaitForSingleObject (hDoUpdate, INFINITE);

				//retr
				QC_BOOST_CHECK_MESSAGE (_helper::IsFoldersEqual(DIFFTOOL_ROOT,
					std::wstring(COLL_ROOT).append(L"\\Bases_NULL\\bases").c_str(),
					std::wstring (RETR_ROOT).append (L"\\bases")), L"Incorrect bases\n");
				QC_BOOST_CHECK_MESSAGE (_helper::IsFoldersEqual(DIFFTOOL_ROOT,
					std::wstring(COLL_ROOT).append(L"\\Bases_NULL\\data").c_str(),
					std::wstring (RETR_ROOT).append (L"\\index\\6")), L"Incorrect bases\n");
				//upd
				QC_BOOST_CHECK_MESSAGE (_helper::DeleteFile(std::wstring(BASES_ROOT).append (L"\\base100.avc")),
					L"old base file not found");
				QC_BOOST_CHECK_MESSAGE (_helper::IsFoldersEqual(DIFFTOOL_ROOT,
					std::wstring(COLL_ROOT).append(L"\\Bases_1\\plain").c_str(), BASES_ROOT), L"Incorrect bases\n");

				QC_BOOST_CHECK_MESSAGE (_helper::IsFoldersEqual(DIFFTOOL_ROOT,
					std::wstring(COLL_ROOT).append(L"\\Bases_1\\data").c_str(), UPD_ROOT), L"Incorrect index\n");


				QC_BOOST_CHECK_MESSAGE ( NO_ERROR == do_upd.GetError(), L"Wrong return code\n" );

	}

	{
		std::wcout<<L"Change transaction status in update part\n";

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

		std::wcout<<L"updateRequired "<<data.updateRequired()<<L"\n";
		std::wcout<<L"retranslationRequired "<<data.retranslationRequired()<<L"\n";

		FileInfo u0607g_retr ( L"u0607g.xml", L"index/6", FileInfo::index, true );
		u0607g_retr.m_localPath = RETR_ROOT;
		FileInfo u0607g_upd ( L"u0607g.xml", L"index/6", FileInfo::index, true );
		u0607g_upd.m_localPath = UPD_ROOT;
		//testCallback.SetPrimaryIndex(u0607g);

		cbacks.bFlags[Callbacks::getPrimaryIndexWithEvent] = true;
		cbacks.bFlags[Callbacks::checkFilesToDownload] = true;
		CREATE_EVENTS(getPrimaryIndexWithEvent)
			CREATE_EVENTS(checkFilesToDownload)

			CDownloadProgress dp (&testCallback); Updater upd (dp, testCallback, testJournal, &testLog);
		HANDLE hDoUpdate;

		doUpdater do_upd(&upd, hDoUpdate);

		WAIT_FOR_READY_EVENT(getPrimaryIndexWithEvent)
			if ( testCallback.GetGpiRetrMode() ) testCallback.SetPrimaryIndex(u0607g_retr);
			else testCallback.SetPrimaryIndex(u0607g_upd);
			SET_GOON_EVENT(getPrimaryIndexWithEvent);
			WAIT_FOR_READY_EVENT(getPrimaryIndexWithEvent)
				if ( testCallback.GetGpiRetrMode() ) testCallback.SetPrimaryIndex(u0607g_retr);
				else testCallback.SetPrimaryIndex(u0607g_upd);
				SET_GOON_EVENT(getPrimaryIndexWithEvent);

				for (int i = 1; i<=2; ++i)
				{
					WAIT_FOR_READY_EVENT(checkFilesToDownload)
						TestCallbackInterface::_checkFilesToDownload& cfd = testCallback.GetCheckFilesToDownload();
					std::wcout<<cfd.m_matchFileList->size()<<L"\n";
					for (std::vector<FileInfo>::iterator it = cfd.m_matchFileList->begin();
						it!=cfd.m_matchFileList->end(); ++it)
					{
						std::wcout<<it->m_filename.m_value.toWideChar()<<L"\n";
					}

					if (cfd.retranslationMode)
					{

						QC_BOOST_REQUIRE_MESSAGE (cfd.m_matchFileList->size() == 7, L"Incorrect file list size\n");
					}
					else
					{
						QC_BOOST_REQUIRE_MESSAGE (cfd.m_matchFileList->size() == 11, L"Incorrect file list size\n");

						for (std::vector<FileInfo>::iterator it = cfd.m_matchFileList->begin();
							it!=cfd.m_matchFileList->end(); ++it)
							it->m_transactionInformation.m_changeStatus = FileInfo::unchanged;
					}

					SET_GOON_EVENT(checkFilesToDownload);
				}

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
				QC_BOOST_CHECK_MESSAGE (_helper::IsFoldersEqual(DIFFTOOL_ROOT,
					std::wstring(COLL_ROOT).append(L"\\Bases_NULL\\plain").c_str(), BASES_ROOT), L"Incorrect bases\n");

				QC_BOOST_CHECK_MESSAGE (_helper::IsFoldersEqual(DIFFTOOL_ROOT,
					std::wstring(COLL_ROOT).append(L"\\Bases_NULL\\data").c_str(), UPD_ROOT), L"Incorrect index\n");


				QC_BOOST_CHECK_MESSAGE ( NO_ERROR == do_upd.GetError(), L"Wrong return code\n" );

	}

	{
		//std::wcout<<L"Bug 17883 (fixed\n";
		std::wcout<<L"Implement the contrary test\n";
		std::wcout<<L"Check results if retranslation part is cancelled\n";

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

		UpdaterConfiguration data(1, 1, L"1");

		data.temporary_dir = TEMP_ROOT;
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

		std::wcout<<L"updateRequired "<<data.updateRequired()<<L"\n";
		std::wcout<<L"retranslationRequired "<<data.retranslationRequired()<<L"\n";

		FileInfo u0607g_retr ( L"u0607g.xml", L"index/6", FileInfo::index, true );
		u0607g_retr.m_localPath = RETR_ROOT;
		FileInfo u0607g_upd ( L"u0607g.xml", L"index/6", FileInfo::index, true );
		u0607g_upd.m_localPath = UPD_ROOT;

		cbacks.bFlags[Callbacks::getPrimaryIndexWithEvent] = true;
		cbacks.bFlags[Callbacks::checkFilesToDownload] = true;
		CREATE_EVENTS(getPrimaryIndexWithEvent)
		CREATE_EVENTS(checkFilesToDownload)

		CDownloadProgress dp (&testCallback); Updater upd (dp, testCallback, testJournal, &testLog);
		HANDLE hDoUpdate;

		doUpdater do_upd(&upd, hDoUpdate);

		WAIT_FOR_READY_EVENT(getPrimaryIndexWithEvent)
			if ( testCallback.GetGpiRetrMode() ) testCallback.SetPrimaryIndex(u0607g_retr);
			else testCallback.SetPrimaryIndex(u0607g_upd);
		SET_GOON_EVENT(getPrimaryIndexWithEvent);
		WAIT_FOR_READY_EVENT(getPrimaryIndexWithEvent)
			if ( testCallback.GetGpiRetrMode() ) testCallback.SetPrimaryIndex(u0607g_retr);
			else testCallback.SetPrimaryIndex(u0607g_upd);
		SET_GOON_EVENT(getPrimaryIndexWithEvent);

				for (int i = 1; i<=2; ++i)
				{
					WAIT_FOR_READY_EVENT(checkFilesToDownload)
					TestCallbackInterface::_checkFilesToDownload& cfd = testCallback.GetCheckFilesToDownload();
					std::wcout<<cfd.m_matchFileList->size()<<L"\n";
					for (std::vector<FileInfo>::iterator it = cfd.m_matchFileList->begin();
						it!=cfd.m_matchFileList->end(); ++it)
					{
						std::wcout<<it->m_filename.m_value.toWideChar()<<L"\n";
					}

					if (cfd.retranslationMode)
					{
						cfd.result = false;
					}
					else
					{
						cfd.result = true;
					}

					SET_GOON_EVENT(checkFilesToDownload);
				}

				WaitForSingleObject (hDoUpdate, INFINITE);

				//retr
				QC_BOOST_CHECK_MESSAGE (_helper::IsFoldersEqual(DIFFTOOL_ROOT,
					std::wstring(COLL_ROOT).append(L"\\Bases_NULL\\bases").c_str(),
					std::wstring (RETR_ROOT).append (L"\\bases")), L"Incorrect retr bases\n");
				QC_BOOST_CHECK_MESSAGE (_helper::IsFoldersEqual(DIFFTOOL_ROOT,
					std::wstring(COLL_ROOT).append(L"\\Bases_NULL\\data").c_str(),
					std::wstring (RETR_ROOT).append (L"\\index\\6")), L"Incorrect retr bases\n");
				//upd
				QC_BOOST_CHECK_MESSAGE (_helper::IsFoldersEqual(DIFFTOOL_ROOT,
					std::wstring(COLL_ROOT).append(L"\\Bases_1\\plain").c_str(), BASES_ROOT), L"Incorrect upd bases\n");

				QC_BOOST_CHECK_MESSAGE (_helper::IsFoldersEqual(DIFFTOOL_ROOT,
					std::wstring(COLL_ROOT).append(L"\\Bases_1\\data").c_str(), UPD_ROOT), L"Incorrect upd index\n");


				QC_BOOST_CHECK_MESSAGE ( CORE_UpdateSuccessfulRetranslationFailed == do_upd.GetError(), L"Wrong return code\n" );

	}

	{
		//std::wcout<<L"Bug 17883 (fixed\n";
		std::wcout<<L"Implement the contrary test\n";
		std::wcout<<L"Check results if update part is cancelled\n";

		TestLog testLog;

		_helper::DeleteDirectory(ROOT);

		//upd
		KLUPD::createFolder (BASES_ROOT, &testLog);
		KLUPD::createFolder (UPD_ROOT, &testLog);
		
		_helper::CopyFolder(std::wstring(COLL_ROOT).append(L"\\Bases_NULL\\plain").c_str(),BASES_ROOT);
		_helper::CloneFile (std::wstring(COLL_ROOT).append(L"\\Bases_NULL\\index\\6\\u0607g.xml").c_str(),
		std::wstring (UPD_ROOT).append (L"\\u0607g.xml"), false);
		
		UpdaterConfiguration data(1, 1, L"1");

		data.temporary_dir = TEMP_ROOT;
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

		std::wcout<<L"updateRequired "<<data.updateRequired()<<L"\n";
		std::wcout<<L"retranslationRequired "<<data.retranslationRequired()<<L"\n";

		FileInfo u0607g_retr ( L"u0607g.xml", L"index/6", FileInfo::index, true );
		u0607g_retr.m_localPath = RETR_ROOT;
		FileInfo u0607g_upd ( L"u0607g.xml", L"index/6", FileInfo::index, true );
		u0607g_upd.m_localPath = UPD_ROOT;

		cbacks.bFlags[Callbacks::getPrimaryIndexWithEvent] = true;
		cbacks.bFlags[Callbacks::checkFilesToDownload] = true;
		CREATE_EVENTS(getPrimaryIndexWithEvent)
			CREATE_EVENTS(checkFilesToDownload)

			CDownloadProgress dp (&testCallback); Updater upd (dp, testCallback, testJournal, &testLog);
		HANDLE hDoUpdate;

		doUpdater do_upd(&upd, hDoUpdate);

		WAIT_FOR_READY_EVENT(getPrimaryIndexWithEvent)
			if ( testCallback.GetGpiRetrMode() ) testCallback.SetPrimaryIndex(u0607g_retr);
			else testCallback.SetPrimaryIndex(u0607g_upd);
			SET_GOON_EVENT(getPrimaryIndexWithEvent);
			WAIT_FOR_READY_EVENT(getPrimaryIndexWithEvent)
				if ( testCallback.GetGpiRetrMode() ) testCallback.SetPrimaryIndex(u0607g_retr);
				else testCallback.SetPrimaryIndex(u0607g_upd);
				SET_GOON_EVENT(getPrimaryIndexWithEvent);

				for (int i = 1; i<=2; ++i)
				{
					WAIT_FOR_READY_EVENT(checkFilesToDownload)
						TestCallbackInterface::_checkFilesToDownload& cfd = testCallback.GetCheckFilesToDownload();
					std::wcout<<cfd.m_matchFileList->size()<<L"\n";
					for (std::vector<FileInfo>::iterator it = cfd.m_matchFileList->begin();
						it!=cfd.m_matchFileList->end(); ++it)
					{
						std::wcout<<it->m_filename.m_value.toWideChar()<<L"\n";
					}

					if (cfd.retranslationMode)
					{
						cfd.result = true;
					}
					else
					{
						cfd.result = false;
					}

					SET_GOON_EVENT(checkFilesToDownload);
				}

				WaitForSingleObject (hDoUpdate, INFINITE);

				//retr
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
				QC_BOOST_CHECK_MESSAGE (_helper::IsFoldersEqual(DIFFTOOL_ROOT,
					std::wstring(COLL_ROOT).append(L"\\Bases_NULL\\plain").c_str(), BASES_ROOT), L"Incorrect upd bases\n");

				QC_BOOST_CHECK_MESSAGE (_helper::IsFoldersEqual(DIFFTOOL_ROOT,
					std::wstring(COLL_ROOT).append(L"\\Bases_NULL\\data").c_str(), UPD_ROOT), L"Incorrect upd index\n");


				QC_BOOST_CHECK_MESSAGE ( CORE_BASE_CHECK_FAILED == do_upd.GetError(), L"Wrong return code\n" );

	}

}

void TestSuite::TestDownloadTheSameFiles ()
{
	using namespace KLUPD;
	std::wcout<<L"TestDownloadTheSameFiles\n";
	Logger::Case _case (m_logger, m_testp, m_step, L"TestDownloadTheSameFiles");

	{
		std::wcout<<L"Theme\n";

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
		data.m_componentsToUpdate.push_back (L"AVS");

		data.RetranslatedObjects.m_componentsToRetranslate.push_back (L"CORE");
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

		std::wcout<<L"updateRequired "<<data.updateRequired()<<L"\n";
		std::wcout<<L"retranslationRequired "<<data.retranslationRequired()<<L"\n";

		FileInfo u0607g_retr ( L"u0607g.xml", L"index/6", FileInfo::index, true );
		u0607g_retr.m_localPath = RETR_ROOT;
		FileInfo u0607g_upd ( L"u0607g.xml", L"index/6", FileInfo::index, true );
		u0607g_upd.m_localPath = UPD_ROOT;

		cbacks.bFlags[Callbacks::getPrimaryIndexWithEvent] = true;
		CREATE_EVENTS(getPrimaryIndexWithEvent)

		Updater upd (testCallback.m_dp, testCallback, testJournal, &testLog);
		HANDLE hDoUpdate;

		doUpdater do_upd(&upd, hDoUpdate);

		for (int i = 1; i <= 2; ++i)
		{
			WAIT_FOR_READY_EVENT(getPrimaryIndexWithEvent)
			if ( testCallback.GetGpiRetrMode() ) testCallback.SetPrimaryIndex(u0607g_retr);
			else testCallback.SetPrimaryIndex(u0607g_upd);
			SET_GOON_EVENT(getPrimaryIndexWithEvent);
		}

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
		QC_BOOST_CHECK_MESSAGE (_helper::DeleteFile(std::wstring(BASES_ROOT).append (L"\\base100.avc")),
			L"old base file not found");
		QC_BOOST_CHECK_MESSAGE (_helper::IsFoldersEqual(DIFFTOOL_ROOT,
			std::wstring(COLL_ROOT).append(L"\\Bases_1\\plain").c_str(), BASES_ROOT), L"Incorrect bases\n");

		QC_BOOST_CHECK_MESSAGE (_helper::IsFoldersEqual(DIFFTOOL_ROOT,
			std::wstring(COLL_ROOT).append(L"\\Bases_1\\data").c_str(), UPD_ROOT), L"Incorrect index\n");


		QC_BOOST_CHECK_MESSAGE ( NO_ERROR == do_upd.GetError(), L"Wrong return code\n" );

		std::wcout<<testCallback.m_dp.bytesTransferred()<<L"\n";
		QC_BOOST_CHECK_MESSAGE (testCallback.m_dp.bytesTransferred() == /*103473*/103511, L"Incorrect number of bytes downloaded\n");
	}
}


void TestSuite::TestBothCheckDownloadedFiles ()
{
	using namespace KLUPD;
	std::wcout<<L"TestBothCheckDownloadedFiles\n";
	Logger::Case _case (m_logger, m_testp, m_step, L"TestBothCheckDownloadedFiles");

	{
		std::wcout<<L"Check transaction status\n";

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
		
		std::wcout<<L"updateRequired "<<data.updateRequired()<<L"\n";
		std::wcout<<L"retranslationRequired "<<data.retranslationRequired()<<L"\n";

		FileInfo u0607g_retr ( L"u0607g.xml", L"index/6", FileInfo::index, true );
		u0607g_retr.m_localPath = RETR_ROOT;
		FileInfo u0607g_upd ( L"u0607g.xml", L"index/6", FileInfo::index, true );
		u0607g_upd.m_localPath = UPD_ROOT;
		//testCallback.SetPrimaryIndex(u0607g);
		
		cbacks.bFlags[Callbacks::getPrimaryIndexWithEvent] = true;
		cbacks.bFlags[Callbacks::checkDownloadedFiles] = true;
		CREATE_EVENTS(getPrimaryIndexWithEvent)
		CREATE_EVENTS(checkDownloadedFiles)

		Updater upd (testCallback.m_dp, testCallback, testJournal, &testLog);
		HANDLE hDoUpdate;

		doUpdater do_upd(&upd, hDoUpdate);

		WAIT_FOR_READY_EVENT(getPrimaryIndexWithEvent)
		if ( testCallback.GetGpiRetrMode() ) testCallback.SetPrimaryIndex(u0607g_retr);
			else testCallback.SetPrimaryIndex(u0607g_upd);
		SET_GOON_EVENT(getPrimaryIndexWithEvent);
		WAIT_FOR_READY_EVENT(getPrimaryIndexWithEvent)
		if ( testCallback.GetGpiRetrMode() ) testCallback.SetPrimaryIndex(u0607g_retr);
			else testCallback.SetPrimaryIndex(u0607g_upd);
		SET_GOON_EVENT(getPrimaryIndexWithEvent);

		for (int i = 1; i<=2; ++i)
		{
			WAIT_FOR_READY_EVENT(checkDownloadedFiles)
			TestCallbackInterface::_checkDownloadedFiles& cfd = testCallback.GetCheckDownloadedFiles();
			std::wcout<<cfd.fv.size()<<L"\n";

			if (cfd.retranslationMode)
			{

				QC_BOOST_REQUIRE_MESSAGE (cfd.fv.size() == 7, L"Incorrect file list size\n");
						
				QC_BOOST_CHECK_MESSAGE (cfd.fv.at(4).m_filename == L"base101.avc", L"Incorrect file name\n");
				QC_BOOST_CHECK_MESSAGE (cfd.fv.at(5).m_filename == L"base102.avc", L"Incorrect file name\n");
				QC_BOOST_CHECK_MESSAGE (cfd.fv.at(6).m_filename == L"base103.avc", L"Incorrect file name\n");
				//QC_BOOST_CHECK_MESSAGE (cfd.fv.at(9).m_filename == L"base100.avc", L"Incorrect file name\n");

				QC_BOOST_CHECK_MESSAGE (cfd.fv.at(4).m_transactionInformation.m_changeStatus == FileInfo::unchanged, L"Incorrect transaction status\n");
				QC_BOOST_CHECK_MESSAGE (cfd.fv.at(5).m_transactionInformation.m_changeStatus == FileInfo::modified, L"Incorrect transaction status\n");
				QC_BOOST_CHECK_MESSAGE (cfd.fv.at(6).m_transactionInformation.m_changeStatus == FileInfo::added, L"Incorrect transaction status\n");
				//QC_BOOST_CHECK_MESSAGE (cfd.fv.at(9).m_transactionInformation.m_changeStatus == FileInfo::deleted, L"Incorrect transaction status\n");
			}
			else
			{
				QC_BOOST_REQUIRE_MESSAGE (cfd.fv.size() == 11, L"Incorrect file list size\n");
			
				QC_BOOST_CHECK_MESSAGE (cfd.fv.at(6).m_filename == L"base101.avc", L"Incorrect file name\n");
				QC_BOOST_CHECK_MESSAGE (cfd.fv.at(7).m_filename == L"base102.avc", L"Incorrect file name\n");
				QC_BOOST_CHECK_MESSAGE (cfd.fv.at(8).m_filename == L"base103.avc", L"Incorrect file name\n");
				//QC_BOOST_CHECK_MESSAGE (cfd.fv.at(11).m_filename == L"base100.avc", L"Incorrect file name\n");

				QC_BOOST_CHECK_MESSAGE (cfd.fv.at(6).m_transactionInformation.m_changeStatus == FileInfo::unchanged, L"Incorrect transaction status\n");
				QC_BOOST_CHECK_MESSAGE (cfd.fv.at(7).m_transactionInformation.m_changeStatus == FileInfo::modified, L"Incorrect transaction status\n");
				QC_BOOST_CHECK_MESSAGE (cfd.fv.at(8).m_transactionInformation.m_changeStatus == FileInfo::added, L"Incorrect transaction status\n");
				//QC_BOOST_CHECK_MESSAGE (cfd.fv.at(11).m_transactionInformation.m_changeStatus == FileInfo::deleted, L"Incorrect transaction status\n");
			
			}

			SET_GOON_EVENT(checkDownloadedFiles);
		}

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
		QC_BOOST_CHECK_MESSAGE (_helper::DeleteFile(std::wstring(BASES_ROOT).append (L"\\base100.avc")),
								L"old base file not found");
		QC_BOOST_CHECK_MESSAGE (_helper::IsFoldersEqual(DIFFTOOL_ROOT,
			std::wstring(COLL_ROOT).append(L"\\Bases_1\\plain").c_str(), BASES_ROOT), L"Incorrect bases\n");

		QC_BOOST_CHECK_MESSAGE (_helper::IsFoldersEqual(DIFFTOOL_ROOT,
			std::wstring(COLL_ROOT).append(L"\\Bases_1\\data").c_str(), UPD_ROOT), L"Incorrect index\n");


		QC_BOOST_CHECK_MESSAGE ( NO_ERROR == do_upd.GetError(), L"Wrong return code\n" );

	}
	

	{
		std::wcout<<L"Change result in retranslation part\n";

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
		
		std::wcout<<L"updateRequired "<<data.updateRequired()<<L"\n";
		std::wcout<<L"retranslationRequired "<<data.retranslationRequired()<<L"\n";

		FileInfo u0607g_retr ( L"u0607g.xml", L"index/6", FileInfo::index, true );
		u0607g_retr.m_localPath = RETR_ROOT;
		FileInfo u0607g_upd ( L"u0607g.xml", L"index/6", FileInfo::index, true );
		u0607g_upd.m_localPath = UPD_ROOT;
		//testCallback.SetPrimaryIndex(u0607g);
		
		cbacks.bFlags[Callbacks::getPrimaryIndexWithEvent] = true;
		cbacks.bFlags[Callbacks::checkDownloadedFiles] = true;
		CREATE_EVENTS(getPrimaryIndexWithEvent)
		CREATE_EVENTS(checkDownloadedFiles)

		Updater upd (testCallback.m_dp, testCallback, testJournal, &testLog);
		HANDLE hDoUpdate;

		doUpdater do_upd(&upd, hDoUpdate);

		WAIT_FOR_READY_EVENT(getPrimaryIndexWithEvent)
		if ( testCallback.GetGpiRetrMode() ) testCallback.SetPrimaryIndex(u0607g_retr);
			else testCallback.SetPrimaryIndex(u0607g_upd);
		SET_GOON_EVENT(getPrimaryIndexWithEvent);
		WAIT_FOR_READY_EVENT(getPrimaryIndexWithEvent)
		if ( testCallback.GetGpiRetrMode() ) testCallback.SetPrimaryIndex(u0607g_retr);
			else testCallback.SetPrimaryIndex(u0607g_upd);
		SET_GOON_EVENT(getPrimaryIndexWithEvent);

		for (int i = 1; i<=2; ++i)
		{
			WAIT_FOR_READY_EVENT(checkDownloadedFiles)
			TestCallbackInterface::_checkDownloadedFiles& cfd = testCallback.GetCheckDownloadedFiles();
			std::wcout<<cfd.fv.size()<<L"\n";

			if (cfd.retranslationMode)
			{
				QC_BOOST_REQUIRE_MESSAGE (cfd.fv.size() == 7, L"Incorrect file list size\n");
				cfd.result = false;		
			}
			else
			{
				QC_BOOST_REQUIRE_MESSAGE (cfd.fv.size() == 11, L"Incorrect file list size\n");
				cfd.result = true;	
			}

			SET_GOON_EVENT(checkDownloadedFiles);
		}

		WaitForSingleObject (hDoUpdate, INFINITE);

		//retr
		QC_BOOST_CHECK_MESSAGE (_helper::IsFoldersEqual(DIFFTOOL_ROOT,
			std::wstring(COLL_ROOT).append(L"\\Bases_NULL\\bases").c_str(),
			std::wstring (RETR_ROOT).append (L"\\bases")), L"Incorrect retr bases\n");
		QC_BOOST_CHECK_MESSAGE (_helper::IsFoldersEqual(DIFFTOOL_ROOT,
			std::wstring(COLL_ROOT).append(L"\\Bases_NULL\\data").c_str(),
			std::wstring (RETR_ROOT).append (L"\\index\\6")), L"Incorrect retr bases\n");
		//upd
		QC_BOOST_CHECK_MESSAGE (_helper::DeleteFile(std::wstring(BASES_ROOT).append (L"\\base100.avc")),
								L"old base file not found");
		QC_BOOST_CHECK_MESSAGE (_helper::IsFoldersEqual(DIFFTOOL_ROOT,
			std::wstring(COLL_ROOT).append(L"\\Bases_1\\plain").c_str(), BASES_ROOT), L"Incorrect bases\n");

		QC_BOOST_CHECK_MESSAGE (_helper::IsFoldersEqual(DIFFTOOL_ROOT,
			std::wstring(COLL_ROOT).append(L"\\Bases_1\\data").c_str(), UPD_ROOT), L"Incorrect index\n");


		QC_BOOST_CHECK_MESSAGE ( CORE_UpdateSuccessfulRetranslationFailed == do_upd.GetError(), L"Wrong return code\n" );

	}

	{
		std::wcout<<L"Change transaction result in update part\n";

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
		
		std::wcout<<L"updateRequired "<<data.updateRequired()<<L"\n";
		std::wcout<<L"retranslationRequired "<<data.retranslationRequired()<<L"\n";

		FileInfo u0607g_retr ( L"u0607g.xml", L"index/6", FileInfo::index, true );
		u0607g_retr.m_localPath = RETR_ROOT;
		FileInfo u0607g_upd ( L"u0607g.xml", L"index/6", FileInfo::index, true );
		u0607g_upd.m_localPath = UPD_ROOT;
		//testCallback.SetPrimaryIndex(u0607g);
		
		cbacks.bFlags[Callbacks::getPrimaryIndexWithEvent] = true;
		cbacks.bFlags[Callbacks::checkDownloadedFiles] = true;
		CREATE_EVENTS(getPrimaryIndexWithEvent)
		CREATE_EVENTS(checkDownloadedFiles)

		Updater upd (testCallback.m_dp, testCallback, testJournal, &testLog);
		HANDLE hDoUpdate;

		doUpdater do_upd(&upd, hDoUpdate);

		WAIT_FOR_READY_EVENT(getPrimaryIndexWithEvent)
		if ( testCallback.GetGpiRetrMode() ) testCallback.SetPrimaryIndex(u0607g_retr);
			else testCallback.SetPrimaryIndex(u0607g_upd);
		SET_GOON_EVENT(getPrimaryIndexWithEvent);
		WAIT_FOR_READY_EVENT(getPrimaryIndexWithEvent)
		if ( testCallback.GetGpiRetrMode() ) testCallback.SetPrimaryIndex(u0607g_retr);
			else testCallback.SetPrimaryIndex(u0607g_upd);
		SET_GOON_EVENT(getPrimaryIndexWithEvent);

		for (int i = 1; i<=2; ++i)
		{
			WAIT_FOR_READY_EVENT(checkDownloadedFiles)
			TestCallbackInterface::_checkDownloadedFiles& cfd = testCallback.GetCheckDownloadedFiles();
			std::wcout<<cfd.fv.size()<<L"\n";

			if (cfd.retranslationMode)
			{
				QC_BOOST_REQUIRE_MESSAGE (cfd.fv.size() == 7, L"Incorrect file list size\n");
				cfd.result = true;
			}
			else
			{
				QC_BOOST_REQUIRE_MESSAGE (cfd.fv.size() == 11, L"Incorrect file list size\n");
				cfd.result = false;
			}

			SET_GOON_EVENT(checkDownloadedFiles);
		}

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
		QC_BOOST_CHECK_MESSAGE (_helper::IsFoldersEqual(DIFFTOOL_ROOT,
			std::wstring(COLL_ROOT).append(L"\\Bases_NULL\\plain").c_str(), BASES_ROOT), L"Incorrect bases\n");

		QC_BOOST_CHECK_MESSAGE (_helper::IsFoldersEqual(DIFFTOOL_ROOT,
			std::wstring(COLL_ROOT).append(L"\\Bases_NULL\\data").c_str(), UPD_ROOT), L"Incorrect index\n");


		QC_BOOST_CHECK_MESSAGE ( CORE_BASE_CHECK_FAILED == do_upd.GetError(), L"Wrong return code\n" );

	}

	{
		std::wcout<<L"Change transaction and update results\n";

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

		std::wcout<<L"updateRequired "<<data.updateRequired()<<L"\n";
		std::wcout<<L"retranslationRequired "<<data.retranslationRequired()<<L"\n";

		FileInfo u0607g_retr ( L"u0607g.xml", L"index/6", FileInfo::index, true );
		u0607g_retr.m_localPath = RETR_ROOT;
		FileInfo u0607g_upd ( L"u0607g.xml", L"index/6", FileInfo::index, true );
		u0607g_upd.m_localPath = UPD_ROOT;
		//testCallback.SetPrimaryIndex(u0607g);

		cbacks.bFlags[Callbacks::getPrimaryIndexWithEvent] = true;
		cbacks.bFlags[Callbacks::checkDownloadedFiles] = true;
		CREATE_EVENTS(getPrimaryIndexWithEvent)
			CREATE_EVENTS(checkDownloadedFiles)

			Updater upd (testCallback.m_dp, testCallback, testJournal, &testLog);
		HANDLE hDoUpdate;

		doUpdater do_upd(&upd, hDoUpdate);

		WAIT_FOR_READY_EVENT(getPrimaryIndexWithEvent)
		if ( testCallback.GetGpiRetrMode() ) testCallback.SetPrimaryIndex(u0607g_retr);
			else testCallback.SetPrimaryIndex(u0607g_upd);
		SET_GOON_EVENT(getPrimaryIndexWithEvent);
		WAIT_FOR_READY_EVENT(getPrimaryIndexWithEvent)
		if ( testCallback.GetGpiRetrMode() ) testCallback.SetPrimaryIndex(u0607g_retr);
			else testCallback.SetPrimaryIndex(u0607g_upd);
		SET_GOON_EVENT(getPrimaryIndexWithEvent);

		for (int i = 1; i<=2; ++i)
		{
			WAIT_FOR_READY_EVENT(checkDownloadedFiles)
			TestCallbackInterface::_checkDownloadedFiles& cfd = testCallback.GetCheckDownloadedFiles();
			std::wcout<<cfd.fv.size()<<L"\n";

			if (cfd.retranslationMode)
			{
				QC_BOOST_REQUIRE_MESSAGE (cfd.fv.size() == 7, L"Incorrect file list size\n");
				cfd.result = false;
			}
			else
			{
				QC_BOOST_REQUIRE_MESSAGE (cfd.fv.size() == 11, L"Incorrect file list size\n");
				cfd.result = false;
			}

			SET_GOON_EVENT(checkDownloadedFiles);
		}

		WaitForSingleObject (hDoUpdate, INFINITE);

		//retr
		QC_BOOST_CHECK_MESSAGE (_helper::IsFoldersEqual(DIFFTOOL_ROOT,
			std::wstring(COLL_ROOT).append(L"\\Bases_NULL\\bases").c_str(),
			std::wstring (RETR_ROOT).append (L"\\bases")), L"Incorrect retr bases\n");
		QC_BOOST_CHECK_MESSAGE (_helper::IsFoldersEqual(DIFFTOOL_ROOT,
			std::wstring(COLL_ROOT).append(L"\\Bases_NULL\\data").c_str(),
			std::wstring (RETR_ROOT).append (L"\\index\\6")), L"Incorrect retr bases\n");
		//upd
		QC_BOOST_CHECK_MESSAGE (_helper::IsFoldersEqual(DIFFTOOL_ROOT,
			std::wstring(COLL_ROOT).append(L"\\Bases_NULL\\plain").c_str(), BASES_ROOT), L"Incorrect bases\n");

		QC_BOOST_CHECK_MESSAGE (_helper::IsFoldersEqual(DIFFTOOL_ROOT,
			std::wstring(COLL_ROOT).append(L"\\Bases_NULL\\data").c_str(), UPD_ROOT), L"Incorrect index\n");

		QC_BOOST_CHECK_MESSAGE ( CORE_BASE_CHECK_FAILED == do_upd.GetError(), L"Wrong return code\n" );

	}
	
}

