#include "util.h"

#include "log.h"
#include "callback.h"
#include "journal.h"
#include "core/updater.h"
#include "helper/local_file.h"

//#include "..\..\..\Client\net\netcore.h"
#include "..\..\..\Client\helper\fileInfo.h"
#include "..\..\..\Client\helper\sites_info.h"

#include <crtdbg.h>
#include <assert.h>
#include "..\..\..\..\..\..\fsee\system\impersonate.hpp"

#include "TestSuiteNet.h"
#include "TestSuite.h"
#include "main.h"
#include "constants.h"
#include "HttpServer.h"
#include "FtpServer.h"
#include "TestSuiteProxyDetector.h"

void TestSuiteNet::TestBasicAuthorizationSuccess ()
{
	std::wcout<<L"TestBasicAuthorizationSuccess\n";
	Logger::Case _case (m_logger, m_testp, m_step, L"TestBasicAuthorizationSuccess");

	_helper::DeleteDirectory(TEST_FOLDER);
	_helper::MakeDirectory(TEST_FOLDER);

	TestLog testLog;
	Callbacks cbacks;
	TestCallbackInterface testCallback (&testLog, &cbacks);
	TestJournalInterface testJournal;
	Signature6Checker s6c (&testLog);

	FileInfo _fi (L"base005.avc", L"/");
	_fi.m_localPath = L"h:\\test\\";

	UpdaterConfiguration data(1, 1, L"1");
	data.proxy_url = L"tl-prx-lnx";
	data.proxy_port = 3128;
	data.m_proxyAuthorizationCredentials.userName ( L"test");
	data.m_proxyAuthorizationCredentials.password ( L"test");
	data.m_httpAuthorizationMethods.push_back (basicAuthorization);
	data.m_sourceList.addSource (UST_UserURL, L"http://tl-srv-w2k", true, L"");
	testCallback.SetUpdaterConfiguration (data);

	Updater upd (testCallback.m_dp, testCallback, testJournal, &testLog);
	upd.constructSourceList(testCallback.updaterConfiguration().m_sourceList, upd.m_sourceList);

	HANDLE hDoDownload;
	doEntryDownloader do_dld(&upd, hDoDownload, _fi, _fi.m_localPath, &s6c);

	WaitForSingleObject (hDoDownload, INFINITE);

	
	QC_BOOST_CHECK_MESSAGE ( CORE_NO_ERROR == do_dld.GetError(), L"Wrong return code\n" );
	QC_BOOST_CHECK_MESSAGE ( _helper::AreFilesEqual (ETALON_NET_FILE, std::wstring(TEST_FOLDER).append(L"\\base005.avc")),
		L"Incorrect file\n" );
}

void TestSuiteNet::TestBasicAuthorizationNotSupported ()
{
	std::wcout<<L"TestBasicAuthorizationNotSupported\n";
	Logger::Case _case (m_logger, m_testp, m_step, L"TestBasicAuthorizationNotSupported");

	_helper::DeleteDirectory(TEST_FOLDER);
	_helper::MakeDirectory(TEST_FOLDER);

	TestLog testLog;
	Callbacks cbacks;
	TestCallbackInterface testCallback (&testLog, &cbacks);
	TestJournalInterface testJournal;
	Signature6Checker s6c (&testLog);

	FileInfo _fi (L"base005.avc", L"/");
	_fi.m_localPath = L"h:\\test\\";

	UpdaterConfiguration data(1, 1, L"1");
	data.proxy_url = L"tl-prx-nta";
	data.proxy_port = 3128;
	data.m_proxyAuthorizationCredentials.userName ( L"kl\\tester");
	data.m_proxyAuthorizationCredentials.password ( L"kf;0dsq Ntcnth");
	data.m_httpAuthorizationMethods.push_back (basicAuthorization);
	data.m_sourceList.addSource (UST_UserURL, L"http://tl-srv-w2k", true, L"");
	testCallback.SetUpdaterConfiguration (data);

	Updater upd (testCallback.m_dp, testCallback, testJournal, &testLog);
	upd.constructSourceList(testCallback.updaterConfiguration().m_sourceList, upd.m_sourceList);

	HANDLE hDoDownload;
	doEntryDownloader do_dld(&upd, hDoDownload, _fi, _fi.m_localPath, &s6c);

	WaitForSingleObject (hDoDownload, INFINITE);

	QC_BOOST_CHECK_MESSAGE ( CORE_PROXY_AUTH_ERROR == do_dld.GetError(), L"Wrong return code\n" );
	QC_BOOST_CHECK_MESSAGE ( ! _helper::AutoFile (std::wstring(TEST_FOLDER).append(L"\\base005.avc")).Exist(),
		L"Incorrect file\n" );
}

void TestSuiteNet::TestBasicAuthorizationWrongCredentials ()
{
	std::wcout<<L"TestBasicAuthorizationWrongCredentials\n";
	Logger::Case _case (m_logger, m_testp, m_step, L"TestBasicAuthorizationWrongCredentials");

	_helper::DeleteDirectory(TEST_FOLDER);
	_helper::MakeDirectory(TEST_FOLDER);

	TestLog testLog;
	Callbacks cbacks;
	TestCallbackInterface testCallback (&testLog, &cbacks);
	TestJournalInterface testJournal;
	Signature6Checker s6c (&testLog);

	FileInfo _fi (L"base005.avc", L"/");
	_fi.m_localPath = L"h:\\test\\";

	UpdaterConfiguration data(1, 1, L"1");
	data.proxy_url = L"tl-prx-lnx";
	data.proxy_port = 3128;

	data.m_proxyAuthorizationCredentials.userName ( L"test1");
	data.m_proxyAuthorizationCredentials.password ( L"test2");
	data.m_httpAuthorizationMethods.push_back (basicAuthorization);
	data.m_sourceList.addSource (UST_UserURL, L"http://tl-srv-w2k", true, L"");
	testCallback.SetUpdaterConfiguration (data);

	Updater upd (testCallback.m_dp, testCallback, testJournal, &testLog);
	upd.constructSourceList(testCallback.updaterConfiguration().m_sourceList, upd.m_sourceList);

	HANDLE hDoDownload;
	doEntryDownloader do_dld(&upd, hDoDownload, _fi, _fi.m_localPath, &s6c);

	WaitForSingleObject (hDoDownload, INFINITE);

	QC_BOOST_CHECK_MESSAGE ( CORE_PROXY_AUTH_ERROR == do_dld.GetError(), L"Wrong return code\n" );
	QC_BOOST_CHECK_MESSAGE ( ! _helper::AutoFile (std::wstring(TEST_FOLDER).append(L"\\base005.avc")).Exist(),
		L"Incorrect file\n" );
}

void TestSuiteNet::TestBasicAuthorizationAccessDenied ()
{
	std::wcout<<L"TestBasicAuthorizationAccessDenied\n";
	Logger::Case _case (m_logger, m_testp, m_step, L"TestBasicAuthorizationAccessDenied");

	_helper::DeleteDirectory(TEST_FOLDER);
	_helper::MakeDirectory(TEST_FOLDER);

	TestLog testLog;
	Callbacks cbacks;
	TestCallbackInterface testCallback (&testLog, &cbacks);
	TestJournalInterface testJournal;
	Signature6Checker s6c (&testLog);

	FileInfo _fi (L"base005.avc", L"/");
	_fi.m_localPath = L"h:\\test\\";

	UpdaterConfiguration data(1, 1, L"1");
	data.proxy_url = L"tl-prx-lnx";
	data.proxy_port = 3128;
	data.m_proxyAuthorizationCredentials.userName ( L"test");
	data.m_proxyAuthorizationCredentials.password ( L"test");
	data.m_httpAuthorizationMethods.push_back (basicAuthorization);
	data.m_sourceList.addSource (UST_UserURL, L"http://tl-srv-wxp", true, L"");
	testCallback.SetUpdaterConfiguration (data);

	Updater upd (testCallback.m_dp, testCallback, testJournal, &testLog);
	upd.constructSourceList(testCallback.updaterConfiguration().m_sourceList, upd.m_sourceList);

	HANDLE hDoDownload;
	doEntryDownloader do_dld(&upd, hDoDownload, _fi, _fi.m_localPath, &s6c);

	WaitForSingleObject (hDoDownload, INFINITE);

	//из-за новой обработки 403-го кода
	QC_BOOST_CHECK_MESSAGE ( /*CORE_PROXY_AUTH_ERROR*/CORE_DOWNLOAD_ERROR == do_dld.GetError(), L"Wrong return code\n" );
	QC_BOOST_CHECK_MESSAGE ( ! _helper::AutoFile (std::wstring(TEST_FOLDER).append(L"\\base005.avc")).Exist(),
		L"Incorrect file\n" );
}






////////////////////////////////////////////







void TestSuiteNet::TestNtlmAuthorizationSuccess ()
{
	std::wcout<<L"TestNtlmAuthorizationSuccess\n";
	Logger::Case _case (m_logger, m_testp, m_step, L"TestNtlmAuthorizationSuccess");

	_helper::DeleteDirectory(TEST_FOLDER);
	_helper::MakeDirectory(TEST_FOLDER);

	TestLog testLog;
	Callbacks cbacks;
	TestCallbackInterface testCallback (&testLog, &cbacks);
	TestJournalInterface testJournal;
	Signature6Checker s6c (&testLog);

	FileInfo _fi (L"base005.avc", L"/");
	_fi.m_localPath = L"h:\\test\\";

	UpdaterConfiguration data(1, 1, L"1");
	data.proxy_url = L"tl-prx-nta";
	data.proxy_port = 3128;
	data.m_proxyAuthorizationCredentials.userName ( L"t");
	data.m_proxyAuthorizationCredentials.password ( L"t");
	data.m_httpAuthorizationMethods.push_back (ntlmAuthorization);
	data.m_sourceList.addSource (UST_UserURL, L"http://tl-srv-wxp", true, L"");
	testCallback.SetUpdaterConfiguration (data);

	Updater upd (testCallback.m_dp, testCallback, testJournal, &testLog);
	upd.constructSourceList(testCallback.updaterConfiguration().m_sourceList, upd.m_sourceList);

	HANDLE hDoDownload;
	doEntryDownloader do_dld(&upd, hDoDownload, _fi, _fi.m_localPath, &s6c);

	WaitForSingleObject (hDoDownload, INFINITE);

	QC_BOOST_CHECK_MESSAGE ( CORE_NO_ERROR == do_dld.GetError(), L"Wrong return code\n" );
	QC_BOOST_CHECK_MESSAGE ( _helper::AreFilesEqual (ETALON_NET_FILE, std::wstring(TEST_FOLDER).append(L"\\base005.avc")),
		L"Incorrect file\n" );
}

void TestSuiteNet::TestNtlmAuthorizationNotSupported ()
{
	std::wcout<<L"TestNtlmAuthorizationNotSupported\n";
	Logger::Case _case (m_logger, m_testp, m_step, L"TestNtlmAuthorizationNotSupported");

	_helper::DeleteDirectory(TEST_FOLDER);
	_helper::MakeDirectory(TEST_FOLDER);

	TestLog testLog;
	Callbacks cbacks;
	TestCallbackInterface testCallback (&testLog, &cbacks);
	TestJournalInterface testJournal;
	Signature6Checker s6c (&testLog);

	FileInfo _fi (L"base005.avc", L"/");
	_fi.m_localPath = L"h:\\test\\";

	UpdaterConfiguration data(1, 1, L"1");
	data.proxy_url = L"tl-prx-lnx";
	data.proxy_port = 3128;
	data.m_proxyAuthorizationCredentials.userName ( L"1");
	data.m_proxyAuthorizationCredentials.password ( L"2");
	data.m_httpAuthorizationMethods.push_back (ntlmAuthorization);
	data.m_sourceList.addSource (UST_UserURL, L"http://tl-srv-w2k", true, L"");
	testCallback.SetUpdaterConfiguration (data);

	Updater upd (testCallback.m_dp, testCallback, testJournal, &testLog);
	upd.constructSourceList(testCallback.updaterConfiguration().m_sourceList, upd.m_sourceList);

	HANDLE hDoDownload;
	doEntryDownloader do_dld(&upd, hDoDownload, _fi, _fi.m_localPath, &s6c);

	std::cout<<"wait\n";
	WaitForSingleObject (hDoDownload, INFINITE);

	QC_BOOST_CHECK_MESSAGE ( CORE_PROXY_AUTH_ERROR == do_dld.GetError(), L"Wrong return code\n" );
	QC_BOOST_CHECK_MESSAGE ( ! _helper::AutoFile (std::wstring(TEST_FOLDER).append(L"\\base005.avc")).Exist(),
		L"Incorrect file\n" );
}

void TestSuiteNet::TestNtlmAuthorizationAccessDenied ()
{
	std::wcout<<L"TestNtlmAuthorizationAccessDenied\n";
	Logger::Case _case (m_logger, m_testp, m_step, L"TestNtlmAuthorizationAccessDenied");

	_helper::DeleteDirectory(TEST_FOLDER);
	_helper::MakeDirectory(TEST_FOLDER);

	TestLog testLog;
	Callbacks cbacks;
	TestCallbackInterface testCallback (&testLog, &cbacks);
	TestJournalInterface testJournal;
	Signature6Checker s6c (&testLog);

	FileInfo _fi (L"base005.avc", L"/");
	_fi.m_localPath = L"h:\\test\\";

	UpdaterConfiguration data(1, 1, L"1");
	data.proxy_url = L"tl-prx-nta";
	data.proxy_port = 3128;
	data.m_proxyAuthorizationCredentials.userName ( L"1");
	data.m_proxyAuthorizationCredentials.password ( L"2");
	data.m_httpAuthorizationMethods.push_back (ntlmAuthorization);
	data.m_sourceList.addSource (UST_UserURL, L"http://tl-srv-wxp", true, L"");
	testCallback.SetUpdaterConfiguration (data);

	Updater upd (testCallback.m_dp, testCallback, testJournal, &testLog);
	upd.constructSourceList(testCallback.updaterConfiguration().m_sourceList, upd.m_sourceList);

	HANDLE hTestToken;
	LogonUserW (L"Tester", L"kl", L"kf;0dsq Ntcnth", LOGON32_LOGON_NETWORK_CLEARTEXT,
		LOGON32_PROVIDER_DEFAULT, &hTestToken);
	winapi::security::access_token test_token (hTestToken);
	winapi::security::access_token old_token;

	winapi::security::impersonate_thread  (test_token, old_token);

	upd.reportSourceSelected ();

	QC_BOOST_CHECK_MESSAGE ( CORE_DOWNLOAD_ERROR == upd.downloadEntry(_fi, _fi.m_localPath),
		L"Wrong return code\n" );
	QC_BOOST_CHECK_MESSAGE ( ! _helper::AutoFile (std::wstring(TEST_FOLDER).append(L"\\base005.avc")).Exist(),
		L"Incorrect file\n" );

	winapi::security::impersonate_thread  (old_token);
	CloseHandle (test_token.detach());
}




//////////////////////////////







void TestSuiteNet::TestNtlmAuthorizationWithCredsSuccess ()
{
	std::wcout<<L"TestNtlmAuthorizationWithCredsSuccess\n";
	Logger::Case _case (m_logger, m_testp, m_step, L"TestNtlmAuthorizationWithCredsSuccess");

	_helper::DeleteDirectory(TEST_FOLDER);
	_helper::MakeDirectory(TEST_FOLDER);

	TestLog testLog;
	Callbacks cbacks;
	TestCallbackInterface testCallback (&testLog, &cbacks);
	TestJournalInterface testJournal;
	Signature6Checker s6c (&testLog);

	FileInfo _fi (L"base005.avc", L"/");
	_fi.m_localPath = L"h:\\test\\";

	UpdaterConfiguration data(1, 1, L"1");
	data.proxy_url = L"tl-prx-nta";
	data.proxy_port = 3128;
	data.m_proxyAuthorizationCredentials.userName ( L"kl\\tester");
	data.m_proxyAuthorizationCredentials.password ( L"kf;0dsq Ntcnth");
	data.m_httpAuthorizationMethods.push_back (ntlmAuthorizationWithCredentials);
	data.m_sourceList.addSource (UST_UserURL, L"http://tl-srv-w2k", true, L"");
	testCallback.SetUpdaterConfiguration (data);

	Updater upd (testCallback.m_dp, testCallback, testJournal, &testLog);
	upd.constructSourceList(testCallback.updaterConfiguration().m_sourceList, upd.m_sourceList);

	HANDLE hDoDownload;
	doEntryDownloader do_dld(&upd, hDoDownload, _fi, _fi.m_localPath, &s6c);

	WaitForSingleObject (hDoDownload, INFINITE);

	QC_BOOST_CHECK_MESSAGE ( CORE_NO_ERROR == do_dld.GetError(), L"Wrong return code\n" );
	QC_BOOST_CHECK_MESSAGE ( _helper::AreFilesEqual (ETALON_NET_FILE, std::wstring(TEST_FOLDER).append(L"\\base005.avc")),
	L"Incorrect file\n" );
}

void TestSuiteNet::TestNtlmAuthorizationWithCredsNotSupported ()
{
	std::wcout<<L"TestNtlmAuthorizationWithCredsNotSupported\n";
	Logger::Case _case (m_logger, m_testp, m_step, L"TestNtlmAuthorizationWithCredsNotSupported");

	_helper::DeleteDirectory(TEST_FOLDER);
	_helper::MakeDirectory(TEST_FOLDER);

	TestLog testLog;
	Callbacks cbacks;
	TestCallbackInterface testCallback (&testLog, &cbacks);
	TestJournalInterface testJournal;
	Signature6Checker s6c (&testLog);

	FileInfo _fi (L"base005.avc", L"/");
	_fi.m_localPath = L"h:\\test\\";

	UpdaterConfiguration data(1, 1, L"1");
	data.proxy_url = L"tl-prx-lnx";
	data.proxy_port = 3128;
	data.m_proxyAuthorizationCredentials.userName ( L"");
	data.m_proxyAuthorizationCredentials.password ( L"");
	data.m_proxyAuthorizationCredentials.userName ( L"kl\\tester");
	data.m_proxyAuthorizationCredentials.password ( L"kf;0dsq Ntcnth");
	data.m_httpAuthorizationMethods.push_back (ntlmAuthorizationWithCredentials);
	data.m_sourceList.addSource (UST_UserURL, L"http://tl-srv-w2k", true, L"");
	testCallback.SetUpdaterConfiguration (data);

	Updater upd (testCallback.m_dp, testCallback, testJournal, &testLog);
	upd.constructSourceList(testCallback.updaterConfiguration().m_sourceList, upd.m_sourceList);

	HANDLE hDoDownload;
	doEntryDownloader do_dld(&upd, hDoDownload, _fi, _fi.m_localPath, &s6c);

	WaitForSingleObject (hDoDownload, INFINITE);

	QC_BOOST_CHECK_MESSAGE ( CORE_PROXY_AUTH_ERROR == do_dld.GetError(), L"Wrong return code\n" );
	QC_BOOST_CHECK_MESSAGE ( ! _helper::AutoFile (std::wstring(TEST_FOLDER).append(L"\\base005.avc")).Exist(),
		L"Incorrect file\n" );
}

void TestSuiteNet::TestNtlmAuthorizationWithCredsWrongCredentials ()
{
	std::wcout<<L"TestNtlmAuthorizationWithCredsWrongCredentials\n";
	Logger::Case _case (m_logger, m_testp, m_step, L"TestNtlmAuthorizationWithCredsWrongCredentials");

	_helper::DeleteDirectory(TEST_FOLDER);
	_helper::MakeDirectory(TEST_FOLDER);

	TestLog testLog;
	Callbacks cbacks;
	TestCallbackInterface testCallback (&testLog, &cbacks);
	TestJournalInterface testJournal;
	Signature6Checker s6c (&testLog);

	FileInfo _fi (L"base005.avc", L"/");
	_fi.m_localPath = L"h:\\test\\";

	UpdaterConfiguration data(1, 1, L"1");
	data.proxy_url = L"tl-prx-nta";
	data.proxy_port = 3128;
	data.m_proxyAuthorizationCredentials.userName ( L"kl\\tester");
	data.m_proxyAuthorizationCredentials.password ( L"!kf;0dsq Ntcnth");
	data.m_httpAuthorizationMethods.push_back (ntlmAuthorizationWithCredentials);
	data.m_sourceList.addSource (UST_UserURL, L"http://tl-srv-wxp", true, L"");
	testCallback.SetUpdaterConfiguration (data);

	Updater upd (testCallback.m_dp, testCallback, testJournal, &testLog);
	upd.constructSourceList(testCallback.updaterConfiguration().m_sourceList, upd.m_sourceList);

	HANDLE hDoDownload;
	doEntryDownloader do_dld(&upd, hDoDownload, _fi, _fi.m_localPath, &s6c);

	WaitForSingleObject (hDoDownload, INFINITE);

	QC_BOOST_CHECK_MESSAGE ( CORE_PROXY_AUTH_ERROR == do_dld.GetError(), L"Wrong return code\n" );
	QC_BOOST_CHECK_MESSAGE ( ! _helper::AutoFile (std::wstring(TEST_FOLDER).append(L"\\base005.avc")).Exist(),
		L"Incorrect file\n" );
}


void TestSuiteNet::TestNtlmAuthorizationWithCredsAccessDenied ()
{
	std::wcout<<L"TestNtlmAuthorizationWithCredsAccessDenied\n";
	Logger::Case _case (m_logger, m_testp, m_step, L"TestNtlmAuthorizationWithCredsAccessDenied");

	_helper::DeleteDirectory(TEST_FOLDER);
	_helper::MakeDirectory(TEST_FOLDER);

	TestLog testLog;
	Callbacks cbacks;
	TestCallbackInterface testCallback (&testLog, &cbacks);
	TestJournalInterface testJournal;
	Signature6Checker s6c (&testLog);

	FileInfo _fi (L"base005.avc", L"/");
	_fi.m_localPath = L"h:\\test\\";

	UpdaterConfiguration data(1, 1, L"1");
	data.proxy_url = L"tl-prx-nta";
	data.proxy_port = 3128;
	data.m_proxyAuthorizationCredentials.userName ( L"kl\\tester");
	data.m_proxyAuthorizationCredentials.password ( L"kf;0dsq Ntcnth");
	data.m_httpAuthorizationMethods.push_back (ntlmAuthorizationWithCredentials);
	data.m_sourceList.addSource (UST_UserURL, L"http://tl-srv-wxp", true, L"");
	testCallback.SetUpdaterConfiguration (data);

	Updater upd (testCallback.m_dp, testCallback, testJournal, &testLog);
	upd.constructSourceList(testCallback.updaterConfiguration().m_sourceList, upd.m_sourceList);

	HANDLE hDoDownload;
	doEntryDownloader do_dld(&upd, hDoDownload, _fi, _fi.m_localPath, &s6c);

	WaitForSingleObject (hDoDownload, INFINITE);

	QC_BOOST_CHECK_MESSAGE ( CORE_DOWNLOAD_ERROR == do_dld.GetError(), L"Wrong return code\n" );
	QC_BOOST_CHECK_MESSAGE ( ! _helper::AutoFile (std::wstring(TEST_FOLDER).append(L"\\base005.avc")).Exist(),
	L"Incorrect file\n" );
}



//////////////////////////////


void TestSuiteNet::TestNoAuthAuthorizationSuccess ()
{
	std::wcout<<L"TestNoAuthAuthorizationSuccess\n";
	Logger::Case _case (m_logger, m_testp, m_step, L"TestNoAuthAuthorizationSuccess");

	_helper::DeleteDirectory(TEST_FOLDER);
	_helper::MakeDirectory(TEST_FOLDER);

	TestLog testLog;
	Callbacks cbacks;
	TestCallbackInterface testCallback (&testLog, &cbacks);
	TestJournalInterface testJournal;
	Signature6Checker s6c (&testLog);

	FileInfo _fi (L"base005.avc", L"/");
	_fi.m_localPath = L"h:\\test\\";

	UpdaterConfiguration data(1, 1, L"1");
	data.proxy_url = L"tl-prx-ipa";
	data.proxy_port = 8080;
	data.m_httpAuthorizationMethods.push_back (noAuthorization);
	data.m_sourceList.addSource (UST_UserURL, L"http://tl-srv-w03", true, L"");
	testCallback.SetUpdaterConfiguration (data);

	Updater upd (testCallback.m_dp, testCallback, testJournal, &testLog);
	upd.constructSourceList(testCallback.updaterConfiguration().m_sourceList, upd.m_sourceList);

	HANDLE hDoDownload;
	doEntryDownloader do_dld(&upd, hDoDownload, _fi, _fi.m_localPath, &s6c);

	WaitForSingleObject (hDoDownload, INFINITE);

	QC_BOOST_CHECK_MESSAGE ( CORE_NO_ERROR == do_dld.GetError(), L"Wrong return code\n" );
	QC_BOOST_CHECK_MESSAGE ( _helper::AreFilesEqual (ETALON_NET_FILE, std::wstring(TEST_FOLDER).append(L"\\base005.avc")),
	L"Incorrect file\n" );
}

void TestSuiteNet::TestNoAuthAuthorizationNotSupported ()
{
	std::wcout<<L"TestNoAuthAuthorizationNotSupported\n";
	Logger::Case _case (m_logger, m_testp, m_step, L"TestNoAuthAuthorizationNotSupported");

	_helper::DeleteDirectory(TEST_FOLDER);
	_helper::MakeDirectory(TEST_FOLDER);

	TestLog testLog;
	Callbacks cbacks;
	TestCallbackInterface testCallback (&testLog, &cbacks);
	TestJournalInterface testJournal;
	Signature6Checker s6c (&testLog);

	FileInfo _fi (L"base005.avc", L"/");
	_fi.m_localPath = L"h:\\test\\";

	UpdaterConfiguration data(1, 1, L"1");
	data.proxy_url = L"tl-prx-i2k";
	data.proxy_port = 3128;
	data.m_httpAuthorizationMethods.push_back (noAuthorization);
	data.m_sourceList.addSource (UST_UserURL, L"http://tl-srv-wxp", true, L"");
	testCallback.SetUpdaterConfiguration (data);

	Updater upd (testCallback.m_dp, testCallback, testJournal, &testLog);
	upd.constructSourceList(testCallback.updaterConfiguration().m_sourceList, upd.m_sourceList);

	HANDLE hDoDownload;
	doEntryDownloader do_dld(&upd, hDoDownload, _fi, _fi.m_localPath, &s6c);

	WaitForSingleObject (hDoDownload, INFINITE);

	QC_BOOST_CHECK_MESSAGE ( CORE_PROXY_AUTH_ERROR == do_dld.GetError(), L"Wrong return code\n" );
	QC_BOOST_CHECK_MESSAGE ( ! _helper::AutoFile (std::wstring(TEST_FOLDER).append(L"\\base005.avc")).Exist(),
		L"Incorrect file\n" );
}

void TestSuiteNet::TestNoAuthAuthorizationAccessDenied ()
{
	std::wcout<<L"TestNoAuthAuthorizationAccessDenied\n";
	Logger::Case _case (m_logger, m_testp, m_step, L"TestNoAuthAuthorizationAccessDenied");

	_helper::DeleteDirectory(TEST_FOLDER);
	_helper::MakeDirectory(TEST_FOLDER);

	TestLog testLog;
	Callbacks cbacks;
	TestCallbackInterface testCallback (&testLog, &cbacks);
	TestJournalInterface testJournal;
	Signature6Checker s6c (&testLog);

	FileInfo _fi (L"base005.avc", L"/");
	_fi.m_localPath = L"h:\\test\\";

	UpdaterConfiguration data(1, 1, L"1");
	data.proxy_url = L"tl-prx-ipa";
	data.proxy_port = 8080;
	data.m_httpAuthorizationMethods.push_back (noAuthorization);
	data.m_sourceList.addSource (UST_UserURL, L"http://tl-srv-wxp", true, L"");
	testCallback.SetUpdaterConfiguration (data);

	Updater upd (testCallback.m_dp, testCallback, testJournal, &testLog);
	upd.constructSourceList(testCallback.updaterConfiguration().m_sourceList, upd.m_sourceList);

	HANDLE hDoDownload;
	doEntryDownloader do_dld(&upd, hDoDownload, _fi, _fi.m_localPath, &s6c);

	WaitForSingleObject (hDoDownload, INFINITE);

	QC_BOOST_CHECK_MESSAGE ( CORE_DOWNLOAD_ERROR == do_dld.GetError(), L"Wrong return code\n" );
	QC_BOOST_CHECK_MESSAGE ( ! _helper::AutoFile (std::wstring(TEST_FOLDER).append(L"\\base005.avc")).Exist(),
	L"Incorrect file\n" );
}

////////////////////////////

void TestSuiteNet::TestOnlyBasicAuthorization ()
{
	std::wcout<<L"TestOnlyBasicAuthorization\n";
	Logger::Case _case (m_logger, m_testp, m_step, L"TestOnlyBasicAuthorization");
	{
		//QC_BOOST_REQUIRE_MESSAGE (false, L"Bug 13887\n");
		_helper::DeleteDirectory(TEST_FOLDER);
		_helper::MakeDirectory(TEST_FOLDER);

		TestLog testLog;
		Callbacks cbacks;
		TestCallbackInterface testCallback (&testLog, &cbacks);
		TestJournalInterface testJournal;
		Signature6Checker s6c (&testLog);

		FileInfo _fi (L"base005.avc", L"/");
		_fi.m_localPath = L"h:\\test\\";

		UpdaterConfiguration data(1, 1, L"1");
		data.proxy_url = L"tl-prx-lnx";
		data.proxy_port = 3128;
		data.m_proxyAuthorizationCredentials.userName ( L"test");
		data.m_proxyAuthorizationCredentials.password ( L"test");
		data.m_httpAuthorizationMethods.push_back (ntlmAuthorization);
		data.m_httpAuthorizationMethods.push_back (basicAuthorization);
		data.m_sourceList.addSource (UST_UserURL, L"http://tl-srv-w2k", true, L"");
		testCallback.SetUpdaterConfiguration (data);

		Updater upd (testCallback.m_dp, testCallback, testJournal, &testLog);
		upd.constructSourceList(testCallback.updaterConfiguration().m_sourceList, upd.m_sourceList);

		HANDLE hDoDownload;
		doEntryDownloader do_dld(&upd, hDoDownload, _fi, _fi.m_localPath, &s6c);

		WaitForSingleObject (hDoDownload, INFINITE);

		QC_BOOST_CHECK_MESSAGE ( CORE_NO_ERROR == do_dld.GetError(), L"Wrong return code\n" );
		QC_BOOST_CHECK_MESSAGE ( _helper::AreFilesEqual (ETALON_NET_FILE, std::wstring(TEST_FOLDER).append(L"\\base005.avc")),
			L"Incorrect file\n" );
	}

	{
		//QC_BOOST_REQUIRE_MESSAGE (false, L"Bug 13887\n");
		_helper::DeleteDirectory(TEST_FOLDER);
		_helper::MakeDirectory(TEST_FOLDER);

		TestLog testLog;
		Callbacks cbacks;
		TestCallbackInterface testCallback (&testLog, &cbacks);
		TestJournalInterface testJournal;
		Signature6Checker s6c (&testLog);

		FileInfo _fi (L"base005.avc", L"/");
		_fi.m_localPath = L"h:\\test\\";

		UpdaterConfiguration data(1, 1, L"1");
		data.proxy_url = L"tl-prx-lnx";
		data.proxy_port = 3128;
		data.m_proxyAuthorizationCredentials.userName ( L"test");
		data.m_proxyAuthorizationCredentials.password ( L"test");
		data.m_httpAuthorizationMethods.push_back (noAuthorization);
		data.m_httpAuthorizationMethods.push_back (ntlmAuthorization);
		data.m_httpAuthorizationMethods.push_back (basicAuthorization);
		data.m_sourceList.addSource (UST_UserURL, L"http://tl-srv-w2k", true, L"");
		testCallback.SetUpdaterConfiguration (data);

		Updater upd (testCallback.m_dp, testCallback, testJournal, &testLog);
		upd.constructSourceList(testCallback.updaterConfiguration().m_sourceList, upd.m_sourceList);

		HANDLE hDoDownload;
		doEntryDownloader do_dld(&upd, hDoDownload, _fi, _fi.m_localPath, &s6c);

		WaitForSingleObject (hDoDownload, INFINITE);

		QC_BOOST_CHECK_MESSAGE ( CORE_NO_ERROR == do_dld.GetError(), L"Wrong return code\n" );
		QC_BOOST_CHECK_MESSAGE ( _helper::AreFilesEqual (ETALON_NET_FILE, std::wstring(TEST_FOLDER).append(L"\\base005.avc")),
			L"Incorrect file\n" );
	}
}


void TestSuiteNet::TestOnlyNtlmAuthorization ()
{
	std::wcout<<L"TestOnlyNtlmAuthorization\n";
	Logger::Case _case (m_logger, m_testp, m_step, L"TestOnlyNtlmAuthorization");

	_helper::DeleteDirectory(TEST_FOLDER);
	_helper::MakeDirectory(TEST_FOLDER);

	TestLog testLog;
	Callbacks cbacks;
	TestCallbackInterface testCallback (&testLog, &cbacks);
	TestJournalInterface testJournal;
	Signature6Checker s6c (&testLog);

	FileInfo _fi (L"base005.avc", L"/");
	_fi.m_localPath = L"h:\\test\\";

	UpdaterConfiguration data(1, 1, L"1");
	data.proxy_url = L"tl-prx-nta";
	data.proxy_port = 3128;
	data.m_proxyAuthorizationCredentials.userName ( L"t");
	data.m_proxyAuthorizationCredentials.password ( L"t");
	data.m_httpAuthorizationMethods.push_back (basicAuthorization);
	data.m_httpAuthorizationMethods.push_back (ntlmAuthorization);
	data.m_sourceList.addSource (UST_UserURL, L"http://tl-srv-wxp", true, L"");
	testCallback.SetUpdaterConfiguration (data);

	Updater upd (testCallback.m_dp, testCallback, testJournal, &testLog);
	upd.constructSourceList(testCallback.updaterConfiguration().m_sourceList, upd.m_sourceList);

	HANDLE hDoDownload;
	doEntryDownloader do_dld(&upd, hDoDownload, _fi, _fi.m_localPath, &s6c);

	WaitForSingleObject (hDoDownload, INFINITE);

	QC_BOOST_CHECK_MESSAGE ( CORE_NO_ERROR == do_dld.GetError(), L"Wrong return code\n" );
	QC_BOOST_CHECK_MESSAGE ( _helper::AreFilesEqual (ETALON_NET_FILE, std::wstring(TEST_FOLDER).append(L"\\base005.avc")),
	L"Incorrect file\n" );
}

void TestSuiteNet::TestOnlyNtlmAuthorizationWithCreds ()
{
	std::wcout<<L"TestOnlyNtlmAuthorizationWithCreds\n";
	Logger::Case _case (m_logger, m_testp, m_step, L"TestOnlyNtlmAuthorizationWithCreds");

	_helper::DeleteDirectory(TEST_FOLDER);
	_helper::MakeDirectory(TEST_FOLDER);

	TestLog testLog;
	Callbacks cbacks;
	TestCallbackInterface testCallback (&testLog, &cbacks);
	TestJournalInterface testJournal;
	Signature6Checker s6c (&testLog);

	FileInfo _fi (L"base005.avc", L"/");
	_fi.m_localPath = L"h:\\test\\";

	UpdaterConfiguration data(1, 1, L"1");
	data.proxy_url = L"tl-prx-nta";
	data.proxy_port = 3128;
	data.m_proxyAuthorizationCredentials.userName ( L"kl\\tester");
	data.m_proxyAuthorizationCredentials.password ( L"kf;0dsq Ntcnth");
	data.m_httpAuthorizationMethods.push_back (basicAuthorization);
	data.m_httpAuthorizationMethods.push_back (ntlmAuthorizationWithCredentials);
	data.m_sourceList.addSource (UST_UserURL, L"http://tl-srv-w2k", true, L"");
	testCallback.SetUpdaterConfiguration (data);

	Updater upd (testCallback.m_dp, testCallback, testJournal, &testLog);
	upd.constructSourceList(testCallback.updaterConfiguration().m_sourceList, upd.m_sourceList);

	HANDLE hDoDownload;
	doEntryDownloader do_dld(&upd, hDoDownload, _fi, _fi.m_localPath, &s6c);

	WaitForSingleObject (hDoDownload, INFINITE);

	QC_BOOST_CHECK_MESSAGE ( CORE_NO_ERROR == do_dld.GetError(), L"Wrong return code\n" );
	QC_BOOST_CHECK_MESSAGE ( _helper::AreFilesEqual (ETALON_NET_FILE, std::wstring(TEST_FOLDER).append(L"\\base005.avc")),
		L"Incorrect file\n" );
}

void TestSuiteNet::TestOnlyNoAuthAuthorization ()
{
	std::wcout<<L"TestOnlyNoAuthAuthorization\n";
	Logger::Case _case (m_logger, m_testp, m_step, L"TestOnlyNoAuthAuthorization");

	_helper::DeleteDirectory(TEST_FOLDER);
	_helper::MakeDirectory(TEST_FOLDER);

	TestLog testLog;
	Callbacks cbacks;
	TestCallbackInterface testCallback (&testLog, &cbacks);
	TestJournalInterface testJournal;
	Signature6Checker s6c (&testLog);

	FileInfo _fi (L"base005.avc", L"/");
	_fi.m_localPath = L"h:\\test\\";

	UpdaterConfiguration data(1, 1, L"1");
	data.proxy_url = L"tl-prx-ipa";
	data.proxy_port = 8080;
	data.m_httpAuthorizationMethods.push_back (basicAuthorization);
	data.m_httpAuthorizationMethods.push_back (ntlmAuthorization);
	data.m_httpAuthorizationMethods.push_back (noAuthorization);
	data.m_sourceList.addSource (UST_UserURL, L"http://tl-srv-w03", true, L"");
	testCallback.SetUpdaterConfiguration (data);

	Updater upd (testCallback.m_dp, testCallback, testJournal, &testLog);
	upd.constructSourceList(testCallback.updaterConfiguration().m_sourceList, upd.m_sourceList);

	HANDLE hDoDownload;
	doEntryDownloader do_dld(&upd, hDoDownload, _fi, _fi.m_localPath, &s6c);

	WaitForSingleObject (hDoDownload, INFINITE);

	QC_BOOST_CHECK_MESSAGE ( CORE_NO_ERROR == do_dld.GetError(), L"Wrong return code\n" );
	QC_BOOST_CHECK_MESSAGE ( _helper::AreFilesEqual (ETALON_NET_FILE, std::wstring(TEST_FOLDER).append(L"\\base005.avc")),
		L"Incorrect file\n" );
}

/////////////////////////////////////////////

void TestSuiteNet::TestHttpPortSpecification()
{
	std::wcout<<L"TestHttpPortSpecification\n";
	Logger::Case _case (m_logger, m_testp, m_step, L"TestHttpPortSpecification");

	_helper::DeleteDirectory(TEST_FOLDER);
	_helper::MakeDirectory(TEST_FOLDER);

	TestLog testLog;
	Callbacks cbacks;
	TestCallbackInterface testCallback (&testLog, &cbacks);
	TestJournalInterface testJournal;
	Signature6Checker s6c (&testLog);

	FileInfo _fi (L"base004.avc", L"/");
	_fi.m_localPath = L"h:\\test\\";

	UpdaterConfiguration data(1, 1, L"1");
	data.m_sourceList.addSource (UST_UserURL, L"http://tl-srv-w03:8081", false, L"");
	testCallback.SetUpdaterConfiguration (data);

	Updater upd (testCallback.m_dp, testCallback, testJournal, &testLog);
	upd.constructSourceList(testCallback.updaterConfiguration().m_sourceList, upd.m_sourceList);

	HANDLE hDoDownload;
	doEntryDownloader do_dld(&upd, hDoDownload, _fi, _fi.m_localPath, &s6c);

	WaitForSingleObject (hDoDownload, INFINITE);

	QC_BOOST_CHECK_MESSAGE ( CORE_NO_ERROR == do_dld.GetError(), L"Wrong return code\n" );
	QC_BOOST_CHECK_MESSAGE ( _helper::AreFilesEqual (ETALON_NET_FILE, std::wstring(TEST_FOLDER).append(L"\\base004.avc")),
		L"Incorrect file\n" );
}


void TestSuiteNet::TestHttpConfigurationTest()
{
	std::wcout<<L"TestHttpConfigurationTest\n";
	Logger::Case _case (m_logger, m_testp, m_step, L"TestHttpConfigurationTest");

	{
		std::wcout<<L"IIS 3.0\n";

		_helper::DeleteDirectory(TEST_FOLDER);
		_helper::MakeDirectory(TEST_FOLDER);

		TestLog testLog;
		Callbacks cbacks;
		TestCallbackInterface testCallback (&testLog, &cbacks);
		TestJournalInterface testJournal;
		Signature6Checker s6c (&testLog);

		FileInfo _fi (L"base005.avc", L"/");
		_fi.m_localPath = L"h:\\test\\";

		UpdaterConfiguration data(1, 1, L"1");
		data.m_sourceList.addSource (UST_UserURL, L"http://tl-srv-wnt", false, L"");
		testCallback.SetUpdaterConfiguration (data);

		Updater upd (testCallback.m_dp, testCallback, testJournal, &testLog);
		upd.constructSourceList(testCallback.updaterConfiguration().m_sourceList, upd.m_sourceList);

		HANDLE hDoDownload;
		doEntryDownloader do_dld(&upd, hDoDownload, _fi, _fi.m_localPath, &s6c);

		WaitForSingleObject (hDoDownload, INFINITE);

		QC_BOOST_CHECK_MESSAGE ( CORE_NO_ERROR == do_dld.GetError(), L"Wrong return code\n" );
		QC_BOOST_CHECK_MESSAGE ( _helper::AreFilesEqual (ETALON_NET_FILE, std::wstring(TEST_FOLDER).append(L"\\base005.avc")),
			L"Incorrect file\n" );
	}
	{
		std::wcout<<L"Apache 1.3 win\n";

		_helper::DeleteDirectory(TEST_FOLDER);
		_helper::MakeDirectory(TEST_FOLDER);

		TestLog testLog;
		Callbacks cbacks;
		TestCallbackInterface testCallback (&testLog, &cbacks);
		TestJournalInterface testJournal;
		Signature6Checker s6c (&testLog);

		FileInfo _fi (L"base005.avc", L"/");
		_fi.m_localPath = L"h:\\test\\";

		UpdaterConfiguration data(1, 1, L"1");
		data.m_sourceList.addSource (UST_UserURL, L"http://tl-srv-wnt:8081", false, L"");
		testCallback.SetUpdaterConfiguration (data);

		Updater upd (testCallback.m_dp, testCallback, testJournal, &testLog);
		upd.constructSourceList(testCallback.updaterConfiguration().m_sourceList, upd.m_sourceList);

		HANDLE hDoDownload;
		doEntryDownloader do_dld(&upd, hDoDownload, _fi, _fi.m_localPath, &s6c);

		WaitForSingleObject (hDoDownload, INFINITE);

		QC_BOOST_CHECK_MESSAGE ( CORE_NO_ERROR == do_dld.GetError(), L"Wrong return code\n" );
		QC_BOOST_CHECK_MESSAGE ( _helper::AreFilesEqual (ETALON_NET_FILE, std::wstring(TEST_FOLDER).append(L"\\base005.avc")),
			L"Incorrect file\n" );
	}

	{
		std::wcout<<L"IIS 5.0\n";

		_helper::DeleteDirectory(TEST_FOLDER);
		_helper::MakeDirectory(TEST_FOLDER);

		TestLog testLog;
		Callbacks cbacks;
		TestCallbackInterface testCallback (&testLog, &cbacks);
		TestJournalInterface testJournal;
		Signature6Checker s6c (&testLog);

		FileInfo _fi (L"base005.avc", L"/");
		_fi.m_localPath = L"h:\\test\\";

		UpdaterConfiguration data(1, 1, L"1");
		data.m_sourceList.addSource (UST_UserURL, L"http://tl-srv-w2k", false, L"");
		testCallback.SetUpdaterConfiguration (data);

		Updater upd (testCallback.m_dp, testCallback, testJournal, &testLog);
		upd.constructSourceList(testCallback.updaterConfiguration().m_sourceList, upd.m_sourceList);

		HANDLE hDoDownload;
		doEntryDownloader do_dld(&upd, hDoDownload, _fi, _fi.m_localPath, &s6c);

		WaitForSingleObject (hDoDownload, INFINITE);

		QC_BOOST_CHECK_MESSAGE ( CORE_NO_ERROR == do_dld.GetError(), L"Wrong return code\n" );
		QC_BOOST_CHECK_MESSAGE ( _helper::AreFilesEqual (ETALON_NET_FILE, std::wstring(TEST_FOLDER).append(L"\\base005.avc")),
			L"Incorrect file\n" );
	}
	{
		std::wcout<<L"Apache 2.0 win\n";

		_helper::DeleteDirectory(TEST_FOLDER);
		_helper::MakeDirectory(TEST_FOLDER);

		TestLog testLog;
		Callbacks cbacks;
		TestCallbackInterface testCallback (&testLog, &cbacks);
		TestJournalInterface testJournal;
		Signature6Checker s6c (&testLog);

		FileInfo _fi (L"base005.avc", L"/");
		_fi.m_localPath = L"h:\\test\\";

		UpdaterConfiguration data(1, 1, L"1");
		data.m_sourceList.addSource (UST_UserURL, L"http://tl-srv-w2k:8081", false, L"");
		testCallback.SetUpdaterConfiguration (data);

		Updater upd (testCallback.m_dp, testCallback, testJournal, &testLog);
		upd.constructSourceList(testCallback.updaterConfiguration().m_sourceList, upd.m_sourceList);

		HANDLE hDoDownload;
		doEntryDownloader do_dld(&upd, hDoDownload, _fi, _fi.m_localPath, &s6c);

		WaitForSingleObject (hDoDownload, INFINITE);

		QC_BOOST_CHECK_MESSAGE ( CORE_NO_ERROR == do_dld.GetError(), L"Wrong return code\n" );
		QC_BOOST_CHECK_MESSAGE ( _helper::AreFilesEqual (ETALON_NET_FILE, std::wstring(TEST_FOLDER).append(L"\\base005.avc")),
			L"Incorrect file\n" );
	}

	{
		std::wcout<<L"IIS 6.0\n";

		_helper::DeleteDirectory(TEST_FOLDER);
		_helper::MakeDirectory(TEST_FOLDER);

		TestLog testLog;
		Callbacks cbacks;
		TestCallbackInterface testCallback (&testLog, &cbacks);
		TestJournalInterface testJournal;
		Signature6Checker s6c (&testLog);

		FileInfo _fi (L"base005.avc", L"/");
		_fi.m_localPath = L"h:\\test\\";

		UpdaterConfiguration data(1, 1, L"1");
		data.m_sourceList.addSource (UST_UserURL, L"http://tl-srv-w03", false, L"");
		testCallback.SetUpdaterConfiguration (data);

		Updater upd (testCallback.m_dp, testCallback, testJournal, &testLog);
		upd.constructSourceList(testCallback.updaterConfiguration().m_sourceList, upd.m_sourceList);

		HANDLE hDoDownload;
		doEntryDownloader do_dld(&upd, hDoDownload, _fi, _fi.m_localPath, &s6c);

		WaitForSingleObject (hDoDownload, INFINITE);

		QC_BOOST_CHECK_MESSAGE ( CORE_NO_ERROR == do_dld.GetError(), L"Wrong return code\n" );
		QC_BOOST_CHECK_MESSAGE ( _helper::AreFilesEqual (ETALON_NET_FILE, std::wstring(TEST_FOLDER).append(L"\\base005.avc")),
			L"Incorrect file\n" );
	}
	{
		std::wcout<<L"Apache 2.2 win\n";

		_helper::DeleteDirectory(TEST_FOLDER);
		_helper::MakeDirectory(TEST_FOLDER);

		TestLog testLog;
		Callbacks cbacks;
		TestCallbackInterface testCallback (&testLog, &cbacks);
		TestJournalInterface testJournal;
		Signature6Checker s6c (&testLog);

		FileInfo _fi (L"base005.avc", L"/");
		_fi.m_localPath = L"h:\\test\\";

		UpdaterConfiguration data(1, 1, L"1");
		data.m_sourceList.addSource (UST_UserURL, L"http://tl-srv-w03:8081", false, L"");
		testCallback.SetUpdaterConfiguration (data);

		Updater upd (testCallback.m_dp, testCallback, testJournal, &testLog);
		upd.constructSourceList(testCallback.updaterConfiguration().m_sourceList, upd.m_sourceList);

		HANDLE hDoDownload;
		doEntryDownloader do_dld(&upd, hDoDownload, _fi, _fi.m_localPath, &s6c);

		WaitForSingleObject (hDoDownload, INFINITE);

		QC_BOOST_CHECK_MESSAGE ( CORE_NO_ERROR == do_dld.GetError(), L"Wrong return code\n" );
		QC_BOOST_CHECK_MESSAGE ( _helper::AreFilesEqual (ETALON_NET_FILE, std::wstring(TEST_FOLDER).append(L"\\base005.avc")),
			L"Incorrect file\n" );
	}

	{
		std::wcout<<L"IIS 5.1\n";

		_helper::DeleteDirectory(TEST_FOLDER);
		_helper::MakeDirectory(TEST_FOLDER);

		TestLog testLog;
		Callbacks cbacks;
		TestCallbackInterface testCallback (&testLog, &cbacks);
		TestJournalInterface testJournal;
		Signature6Checker s6c (&testLog);

		FileInfo _fi (L"base005.avc", L"/");
		_fi.m_localPath = L"h:\\test\\";

		UpdaterConfiguration data(1, 1, L"1");
		data.m_sourceList.addSource (UST_UserURL, L"http://tl-srv-wxp", false, L"");
		testCallback.SetUpdaterConfiguration (data);

		Updater upd (testCallback.m_dp, testCallback, testJournal, &testLog);
		upd.constructSourceList(testCallback.updaterConfiguration().m_sourceList, upd.m_sourceList);

		HANDLE hDoDownload;
		doEntryDownloader do_dld(&upd, hDoDownload, _fi, _fi.m_localPath, &s6c);

		WaitForSingleObject (hDoDownload, INFINITE);

		QC_BOOST_CHECK_MESSAGE ( CORE_NO_ERROR == do_dld.GetError(), L"Wrong return code\n" );
		QC_BOOST_CHECK_MESSAGE ( _helper::AreFilesEqual (ETALON_NET_FILE, std::wstring(TEST_FOLDER).append(L"\\base005.avc")),
			L"Incorrect file\n" );
	}

	/*{//пока не работает
		std::wcout<<L"Apache 2.0 lnx\n";

		_helper::DeleteDirectory(TEST_FOLDER);
		_helper::MakeDirectory(TEST_FOLDER);

		TestLog testLog;
		Callbacks cbacks;
		TestCallbackInterface testCallback (&testLog, &cbacks);
		TestJournalInterface testJournal;
		Signature6Checker s6c (&testLog);

		FileInfo _fi (L"base005.avc", L"/");
		_fi.m_localPath = L"h:\\test\\";

		UpdaterConfiguration data(1, 1, L"1");
		data.m_sourceList.addSource (UST_UserURL, L"http://tl-srv-lnx", false, L"");
		testCallback.SetUpdaterConfiguration (data);

		Updater upd (testCallback.m_dp, testCallback, testJournal, &testLog);
		upd.constructSourceList(testCallback.updaterConfiguration().m_sourceList, upd.m_sourceList);

		HANDLE hDoDownload;
		doEntryDownloader do_dld(&upd, hDoDownload, _fi, _fi.m_localPath, &s6c);

		WaitForSingleObject (hDoDownload, INFINITE);

		QC_BOOST_CHECK_MESSAGE ( CORE_NO_ERROR == do_dld.GetError(), L"Wrong return code\n" );
		QC_BOOST_CHECK_MESSAGE ( _helper::AreFilesEqual (ETALON_NET_FILE, std::wstring(TEST_FOLDER).append(L"\\base005.avc")),
			L"Incorrect file\n" );
	}*/
}

//////////////////////////////

void TestSuiteNet::TestFtpPortSpecification()
{
	std::wcout<<L"TestFtpPortSpecification\n";
	Logger::Case _case (m_logger, m_testp, m_step, L"TestFtpPortSpecification");

	_helper::DeleteDirectory(TEST_FOLDER);
	_helper::MakeDirectory(TEST_FOLDER);

	TestLog testLog;
	Callbacks cbacks;
	TestCallbackInterface testCallback (&testLog, &cbacks);
	TestJournalInterface testJournal;
	Signature6Checker s6c (&testLog);

	FileInfo _fi (L"base003.avc", L"/");

	UpdaterConfiguration data(1, 1, L"1");
	testCallback.SetUpdaterConfiguration (data);

	Updater upd (testCallback.m_dp, testCallback, testJournal, &testLog);
	QC_BOOST_CHECK_MESSAGE ( CORE_NO_ERROR == upd.DoGetRemoteFile_SelfNet (_fi.m_filename, L"h:\\test\\", _fi.m_relativeURLPath, L"ftp://tl-srv-w03:2121"),
		L"Wrong return code\n" );
	QC_BOOST_CHECK_MESSAGE ( _helper::AreFilesEqual (ETALON_NET_FILE, std::wstring(TEST_FOLDER).append(L"\\base003.avc")),
		L"Incorrect file\n" );
}


void TestSuiteNet::TestFtpAuthorization()
{
	std::wcout<<L"TestFtpAuthorization\n";
	Logger::Case _case (m_logger, m_testp, m_step, L"TestFtpAuthorization");

	{
		_helper::DeleteDirectory(TEST_FOLDER);
		_helper::MakeDirectory(TEST_FOLDER);

		TestLog testLog;
		Callbacks cbacks;
		TestCallbackInterface testCallback (&testLog, &cbacks);
		TestJournalInterface testJournal;
		Signature6Checker s6c (&testLog);

		FileInfo _fi (L"base005.avc", L"/nor");
		_fi.m_localPath = L"h:\\test\\";

		UpdaterConfiguration data(1, 1, L"1");
		testCallback.SetUpdaterConfiguration (data);

		Updater upd (testCallback.m_dp, testCallback, testJournal, &testLog);
		QC_BOOST_CHECK_MESSAGE ( CORE_NO_SOURCE_FILE == upd.DoGetRemoteFile_SelfNet (_fi.m_filename, _fi.m_localPath, _fi.m_relativeURLPath, L"ftp://test:test@tl-srv-w03:21"),
			L"Wrong return code\n" );
		QC_BOOST_CHECK_MESSAGE ( ! _helper::AutoFile (std::wstring(TEST_FOLDER).append(L"\\base005.avc")).Exist(),
			L"Incorrect file\n" );
	}

	{
		_helper::DeleteDirectory(TEST_FOLDER);
		_helper::MakeDirectory(TEST_FOLDER);

		TestLog testLog;
		Callbacks cbacks;
		TestCallbackInterface testCallback (&testLog, &cbacks);
		TestJournalInterface testJournal;
		Signature6Checker s6c (&testLog);

		FileInfo _fi (L"base005.avc", L"/now");
		_fi.m_localPath = L"h:\\test\\";

		UpdaterConfiguration data(1, 1, L"1");
		testCallback.SetUpdaterConfiguration (data);

		Updater upd (testCallback.m_dp, testCallback, testJournal, &testLog);
		QC_BOOST_CHECK_MESSAGE ( CORE_NO_ERROR == upd.DoGetRemoteFile_SelfNet (_fi.m_filename, _fi.m_localPath, _fi.m_relativeURLPath, L"ftp://test:test@tl-srv-w03:21"),
			L"Wrong return code\n" );
		QC_BOOST_CHECK_MESSAGE ( _helper::AreFilesEqual (ETALON_NET_FILE, std::wstring(TEST_FOLDER).append(L"\\base005.avc")),
			L"Incorrect file\n" );
	}
}


void TestSuiteNet::TestFtpConfigurationTest()
{
	std::wcout<<L"TestFtpConfigurationTest\n";
	Logger::Case _case (m_logger, m_testp, m_step, L"TestFtpConfigurationTest");

	{
		std::wcout<<L"IIS 3.0\n";

		_helper::DeleteDirectory(TEST_FOLDER);
		_helper::MakeDirectory(TEST_FOLDER);

		TestLog testLog;
		Callbacks cbacks;
		TestCallbackInterface testCallback (&testLog, &cbacks);
		TestJournalInterface testJournal;
		Signature6Checker s6c (&testLog);

		FileInfo _fi (L"base005.avc", L"/");
		_fi.m_localPath = L"h:\\test\\";

		UpdaterConfiguration data(1, 1, L"1");
		testCallback.SetUpdaterConfiguration (data);

		Updater upd (testCallback.m_dp, testCallback, testJournal, &testLog);
		QC_BOOST_CHECK_MESSAGE ( CORE_NO_ERROR == upd.DoGetRemoteFile_SelfNet (_fi.m_filename, _fi.m_localPath, _fi.m_relativeURLPath, L"ftp://tl-srv-wnt"),
			L"Wrong return code\n" );
		QC_BOOST_CHECK_MESSAGE ( _helper::AreFilesEqual (ETALON_NET_FILE, std::wstring(TEST_FOLDER).append(L"\\base005.avc")),
			L"Incorrect file\n" );
	}

	{
		std::wcout<<L"IIS 5.0\n";

		_helper::DeleteDirectory(TEST_FOLDER);
		_helper::MakeDirectory(TEST_FOLDER);

		TestLog testLog;
		Callbacks cbacks;
		TestCallbackInterface testCallback (&testLog, &cbacks);
		TestJournalInterface testJournal;
		Signature6Checker s6c (&testLog);

		FileInfo _fi (L"base005.avc", L"/");
		_fi.m_localPath = L"h:\\test\\";

		UpdaterConfiguration data(1, 1, L"1");
		testCallback.SetUpdaterConfiguration (data);

		Updater upd (testCallback.m_dp, testCallback, testJournal, &testLog);
		QC_BOOST_CHECK_MESSAGE ( CORE_NO_ERROR == upd.DoGetRemoteFile_SelfNet (_fi.m_filename, _fi.m_localPath, _fi.m_relativeURLPath, L"ftp://tl-srv-w2k"),
			L"Wrong return code\n" );
		QC_BOOST_CHECK_MESSAGE ( _helper::AreFilesEqual (ETALON_NET_FILE, std::wstring(TEST_FOLDER).append(L"\\base005.avc")),
			L"Incorrect file\n" );
	}

	{
		std::wcout<<L"IIS 6.0\n";

		_helper::DeleteDirectory(TEST_FOLDER);
		_helper::MakeDirectory(TEST_FOLDER);

		TestLog testLog;
		Callbacks cbacks;
		TestCallbackInterface testCallback (&testLog, &cbacks);
		TestJournalInterface testJournal;
		Signature6Checker s6c (&testLog);

		FileInfo _fi (L"base005.avc", L"/");
		_fi.m_localPath = L"h:\\test\\";

		UpdaterConfiguration data(1, 1, L"1");
		testCallback.SetUpdaterConfiguration (data);

		Updater upd (testCallback.m_dp, testCallback, testJournal, &testLog);
		QC_BOOST_CHECK_MESSAGE ( CORE_NO_ERROR == upd.DoGetRemoteFile_SelfNet (_fi.m_filename, _fi.m_localPath, _fi.m_relativeURLPath, L"ftp://tl-srv-w03"),
			L"Wrong return code\n" );
		QC_BOOST_CHECK_MESSAGE ( _helper::AreFilesEqual (ETALON_NET_FILE, std::wstring(TEST_FOLDER).append(L"\\base005.avc")),
			L"Incorrect file\n" );
	}

	{
		std::wcout<<L"IIS 5.1\n";

		_helper::DeleteDirectory(TEST_FOLDER);
		_helper::MakeDirectory(TEST_FOLDER);

		TestLog testLog;
		Callbacks cbacks;
		TestCallbackInterface testCallback (&testLog, &cbacks);
		TestJournalInterface testJournal;
		Signature6Checker s6c (&testLog);

		FileInfo _fi (L"base005.avc", L"/");
		_fi.m_localPath = L"h:\\test\\";

		UpdaterConfiguration data(1, 1, L"1");
		testCallback.SetUpdaterConfiguration (data);

		Updater upd (testCallback.m_dp, testCallback, testJournal, &testLog);
		QC_BOOST_CHECK_MESSAGE ( CORE_NO_ERROR == upd.DoGetRemoteFile_SelfNet (_fi.m_filename, _fi.m_localPath, _fi.m_relativeURLPath, L"ftp://tl-srv-wxp"),
			L"Wrong return code\n" );
		QC_BOOST_CHECK_MESSAGE ( _helper::AreFilesEqual (ETALON_NET_FILE, std::wstring(TEST_FOLDER).append(L"\\base005.avc")),
			L"Incorrect file\n" );
	}

	{
		std::wcout<<L"Cerberus\n";

		_helper::DeleteDirectory(TEST_FOLDER);
		_helper::MakeDirectory(TEST_FOLDER);

		TestLog testLog;
		Callbacks cbacks;
		TestCallbackInterface testCallback (&testLog, &cbacks);
		TestJournalInterface testJournal;
		Signature6Checker s6c (&testLog);

		FileInfo _fi (L"base005.avc", L"/");
		_fi.m_localPath = L"h:\\test\\";

		UpdaterConfiguration data(1, 1, L"1");
		testCallback.SetUpdaterConfiguration (data);

		Updater upd (testCallback.m_dp, testCallback, testJournal, &testLog);
		QC_BOOST_CHECK_MESSAGE ( CORE_NO_ERROR == upd.DoGetRemoteFile_SelfNet (_fi.m_filename, _fi.m_localPath, _fi.m_relativeURLPath, L"ftp://tl-srv-w03:2121"),
			L"Wrong return code\n" );
		QC_BOOST_CHECK_MESSAGE ( _helper::AreFilesEqual (ETALON_NET_FILE, std::wstring(TEST_FOLDER).append(L"\\base005.avc")),
			L"Incorrect file\n" );
	}

	/*{//пока не работает этот сервер
		std::wcout<<L"VSFtpd 2.3 linux\n";

		_helper::DeleteDirectory(TEST_FOLDER);
		_helper::MakeDirectory(TEST_FOLDER);

		TestLog testLog;
		Callbacks cbacks;
		TestCallbackInterface testCallback (&testLog, &cbacks);
		TestJournalInterface testJournal;
		Signature6Checker s6c (&testLog);

		FileInfo _fi (L"base005.avc", L"/");
		_fi.m_localPath = L"h:\\test\\";

		UpdaterConfiguration data(1, 1, L"1");
		testCallback.SetUpdaterConfiguration (data);

		Updater upd (testCallback.m_dp, testCallback, testJournal, &testLog);
		QC_BOOST_CHECK_MESSAGE ( CORE_NO_ERROR == upd.DoGetRemoteFile_SelfNet (_fi.m_filename, _fi.m_localPath, _fi.m_relativeURLPath, L"ftp://tl-srv-lnx"),
			L"Wrong return code\n" );
		QC_BOOST_CHECK_MESSAGE ( _helper::AreFilesEqual (ETALON_NET_FILE, std::wstring(TEST_FOLDER).append(L"\\base005.avc")),
			L"Incorrect file\n" );
	}*/

}



void TestSuiteNet::TestProxyConfigurationTest ()
{
	std::wcout<<L"TestProxyConfigurationTest\n";
	Logger::Case _case (m_logger, m_testp, m_step, L"TestProxyConfigurationTest");

	{
		std::wcout<<L"tl-prx-i2k ISA2002 Basic\n";

		_helper::DeleteDirectory(TEST_FOLDER);
		_helper::MakeDirectory(TEST_FOLDER);

		TestLog testLog;
		Callbacks cbacks;
		TestCallbackInterface testCallback (&testLog, &cbacks);
		TestJournalInterface testJournal;
		Signature6Checker s6c (&testLog);

		FileInfo _fi (L"base005.avc", L"/");
		_fi.m_localPath = L"h:\\test\\";

		UpdaterConfiguration data(1, 1, L"1");
		data.proxy_url = L"tl-prx-i2k";
		data.proxy_port = 3128;
		data.m_proxyAuthorizationCredentials.userName ( L"tl-prx-i2k\\test");
		data.m_proxyAuthorizationCredentials.password ( L"test");
		data.m_httpAuthorizationMethods.push_back (basicAuthorization);
		data.m_sourceList.addSource (UST_UserURL, L"http://tl-srv-w03", true, L"");
		testCallback.SetUpdaterConfiguration (data);

		Updater upd (testCallback.m_dp, testCallback, testJournal, &testLog);
		upd.constructSourceList(testCallback.updaterConfiguration().m_sourceList, upd.m_sourceList);

		HANDLE hDoDownload;
		doEntryDownloader do_dld(&upd, hDoDownload, _fi, _fi.m_localPath, &s6c);

		WaitForSingleObject (hDoDownload, INFINITE);

		QC_BOOST_CHECK_MESSAGE ( CORE_NO_ERROR == do_dld.GetError(), L"Wrong return code\n" );
		QC_BOOST_CHECK_MESSAGE ( _helper::AreFilesEqual (ETALON_NET_FILE, std::wstring(TEST_FOLDER).append(L"\\base005.avc")),
			L"Incorrect file\n" );
	}

	{
		std::wcout<<L"tl-prx-i2k ISA2002 Ntlm\n";

		_helper::DeleteDirectory(TEST_FOLDER);
		_helper::MakeDirectory(TEST_FOLDER);

		TestLog testLog;
		Callbacks cbacks;
		TestCallbackInterface testCallback (&testLog, &cbacks);
		TestJournalInterface testJournal;
		Signature6Checker s6c (&testLog);

		FileInfo _fi (L"base005.avc", L"/");
		_fi.m_localPath = L"h:\\test\\";

		UpdaterConfiguration data(1, 1, L"1");
		data.proxy_url = L"tl-prx-i2k";
		data.proxy_port = 3128;
		data.m_proxyAuthorizationCredentials.userName ( L"1");
		data.m_proxyAuthorizationCredentials.password ( L"2");
		data.m_httpAuthorizationMethods.push_back (ntlmAuthorization);
		data.m_sourceList.addSource (UST_UserURL, L"http://tl-srv-w03", true, L"");
		testCallback.SetUpdaterConfiguration (data);

		Updater upd (testCallback.m_dp, testCallback, testJournal, &testLog);
		upd.constructSourceList(testCallback.updaterConfiguration().m_sourceList, upd.m_sourceList);

		HANDLE hDoDownload;
		doEntryDownloader do_dld(&upd, hDoDownload, _fi, _fi.m_localPath, &s6c);

		WaitForSingleObject (hDoDownload, INFINITE);

		QC_BOOST_CHECK_MESSAGE ( CORE_NO_ERROR == do_dld.GetError(), L"Wrong return code\n" );
		QC_BOOST_CHECK_MESSAGE ( _helper::AreFilesEqual (ETALON_NET_FILE, std::wstring(TEST_FOLDER).append(L"\\base005.avc")),
			L"Incorrect file\n" );
	}

	{
		std::wcout<<L"tl-prx-i04 ISA2004 Basic\n";

		_helper::DeleteDirectory(TEST_FOLDER);
		_helper::MakeDirectory(TEST_FOLDER);

		TestLog testLog;
		Callbacks cbacks;
		TestCallbackInterface testCallback (&testLog, &cbacks);
		TestJournalInterface testJournal;
		Signature6Checker s6c (&testLog);

		FileInfo _fi (L"base005.avc", L"/");
		_fi.m_localPath = L"h:\\test\\";

		UpdaterConfiguration data(1, 1, L"1");
		data.proxy_url = L"tl-prx-i04";
		data.proxy_port = 3128;
		data.m_proxyAuthorizationCredentials.userName ( L"tl-prx-i04\\test");
		data.m_proxyAuthorizationCredentials.password ( L"test");
		data.m_httpAuthorizationMethods.push_back (basicAuthorization);
		data.m_sourceList.addSource (UST_UserURL, L"http://tl-srv-w03", true, L"");
		testCallback.SetUpdaterConfiguration (data);

		Updater upd (testCallback.m_dp, testCallback, testJournal, &testLog);
		upd.constructSourceList(testCallback.updaterConfiguration().m_sourceList, upd.m_sourceList);

		HANDLE hDoDownload;
		doEntryDownloader do_dld(&upd, hDoDownload, _fi, _fi.m_localPath, &s6c);

		WaitForSingleObject (hDoDownload, INFINITE);

		QC_BOOST_CHECK_MESSAGE ( CORE_NO_ERROR == do_dld.GetError(), L"Wrong return code\n" );
		QC_BOOST_CHECK_MESSAGE ( _helper::AreFilesEqual (ETALON_NET_FILE, std::wstring(TEST_FOLDER).append(L"\\base005.avc")),
			L"Incorrect file\n" );
	}

	{
		std::wcout<<L"tl-prx-i04 ISA2004 Ntlm\n";

		_helper::DeleteDirectory(TEST_FOLDER);
		_helper::MakeDirectory(TEST_FOLDER);

		TestLog testLog;
		Callbacks cbacks;
		TestCallbackInterface testCallback (&testLog, &cbacks);
		TestJournalInterface testJournal;
		Signature6Checker s6c (&testLog);

		FileInfo _fi (L"base005.avc", L"/");
		_fi.m_localPath = L"h:\\test\\";

		UpdaterConfiguration data(1, 1, L"1");
		data.proxy_url = L"tl-prx-i04";
		data.proxy_port = 3128;
		data.m_proxyAuthorizationCredentials.userName ( L"1");
		data.m_proxyAuthorizationCredentials.password ( L"2");
		data.m_httpAuthorizationMethods.push_back (ntlmAuthorization);
		data.m_sourceList.addSource (UST_UserURL, L"http://tl-srv-w03", true, L"");
		testCallback.SetUpdaterConfiguration (data);

		Updater upd (testCallback.m_dp, testCallback, testJournal, &testLog);
		upd.constructSourceList(testCallback.updaterConfiguration().m_sourceList, upd.m_sourceList);

		HANDLE hDoDownload;
		doEntryDownloader do_dld(&upd, hDoDownload, _fi, _fi.m_localPath, &s6c);

		WaitForSingleObject (hDoDownload, INFINITE);

		QC_BOOST_CHECK_MESSAGE ( CORE_NO_ERROR == do_dld.GetError(), L"Wrong return code\n" );
		QC_BOOST_CHECK_MESSAGE ( _helper::AreFilesEqual (ETALON_NET_FILE, std::wstring(TEST_FOLDER).append(L"\\base005.avc")),
			L"Incorrect file\n" );
	}

	{
		std::wcout<<L"tl-prx-lnx Squid 2.6.15 Basic\n";

		_helper::DeleteDirectory(TEST_FOLDER);
		_helper::MakeDirectory(TEST_FOLDER);

		TestLog testLog;
		Callbacks cbacks;
		TestCallbackInterface testCallback (&testLog, &cbacks);
		TestJournalInterface testJournal;
		Signature6Checker s6c (&testLog);

		FileInfo _fi (L"base005.avc", L"/");
		_fi.m_localPath = L"h:\\test\\";

		UpdaterConfiguration data(1, 1, L"1");
		data.proxy_url = L"tl-prx-lnx";
		data.proxy_port = 3128;
		data.m_proxyAuthorizationCredentials.userName ( L"test");
		data.m_proxyAuthorizationCredentials.password ( L"test");
		data.m_httpAuthorizationMethods.push_back (basicAuthorization);
		data.m_sourceList.addSource (UST_UserURL, L"http://tl-srv-w2k", true, L"");
		testCallback.SetUpdaterConfiguration (data);

		Updater upd (testCallback.m_dp, testCallback, testJournal, &testLog);
		upd.constructSourceList(testCallback.updaterConfiguration().m_sourceList, upd.m_sourceList);

		HANDLE hDoDownload;
		doEntryDownloader do_dld(&upd, hDoDownload, _fi, _fi.m_localPath, &s6c);

		WaitForSingleObject (hDoDownload, INFINITE);

		QC_BOOST_CHECK_MESSAGE ( CORE_NO_ERROR == do_dld.GetError(), L"Wrong return code\n" );
		QC_BOOST_CHECK_MESSAGE ( _helper::AreFilesEqual (ETALON_NET_FILE, std::wstring(TEST_FOLDER).append(L"\\base005.avc")),
			L"Incorrect file\n" );
	}
}

void TestSuiteNet::TestHttpAuthorization()
{
	std::wcout<<L"TestHttpAuthorization\n";
	std::wcout<<L"Not implemented\n";
	Logger::Case _case (m_logger, m_testp, m_step, L"TestHttpAuthorization");
	return;

	QC_BOOST_REQUIRE_MESSAGE (false,L"Not implemented\n");

	_helper::DeleteDirectory(TEST_FOLDER);
	_helper::MakeDirectory(TEST_FOLDER);

	TestLog testLog;
	Callbacks cbacks;
	TestCallbackInterface testCallback (&testLog, &cbacks);
	TestJournalInterface testJournal;
	Signature6Checker s6c (&testLog);

	FileInfo _fi (L"grants.avc", L"/");
	_fi.m_localPath = L"h:\\test\\";

	UpdaterConfiguration data(1, 1, L"1");
	testCallback.SetUpdaterConfiguration (data);

	KLUPD::Source _csd (L"http://tl-srv-w03", UST_UserURL, false, L"", UPS_DIRS);

	Updater upd (testCallback.m_dp, testCallback, testJournal, &testLog);
	//QC_BOOST_CHECK_MESSAGE ( CORE_NO_ERROR == upd.DoGetRemoteFile_NewNet (_fi, _fi.m_relativeURLPath, _fi.m_localPath, _csd),
	//				L"Wrong return code\n" );
	QC_BOOST_CHECK_MESSAGE ( _helper::AreFilesEqual (ETALON_NET_FILE, std::wstring(TEST_FOLDER).append(L"\\base005.avc")),
		L"Incorrect file\n" );
}

void TestSuiteNet::TestFtpOverHttp ()
{
	std::wcout<<L"TestFtpOverHttp\n";
	Logger::Case _case (m_logger, m_testp, m_step, L"TestFtpOverHttp");

	//на машине w03 ftp и http сервера имеют разные корневые каталоги
	{
		_helper::DeleteDirectory(TEST_FOLDER);
		_helper::MakeDirectory(TEST_FOLDER);

		TestLog testLog;
		Callbacks cbacks;
		TestCallbackInterface testCallback (&testLog, &cbacks);
		TestJournalInterface testJournal;
		Signature6Checker s6c (&testLog);

		FileInfo _fi (L"base004.avc", L"/");
		_fi.m_localPath = L"h:\\test\\";

		UpdaterConfiguration data(1, 1, L"1");
		data.proxy_url = L"tl-prx-i04";
		data.proxy_port = 3128;
		data.m_proxyAuthorizationCredentials.userName ( L"tl-prx-i04\\test");
		data.m_proxyAuthorizationCredentials.password ( L"test");
		data.m_httpAuthorizationMethods.push_back (basicAuthorization);
		data.m_sourceList.addSource (UST_UserURL, L"ftp://tl-srv-w03:21", true, L"");
		testCallback.SetUpdaterConfiguration (data);

		Updater upd (testCallback.m_dp, testCallback, testJournal, &testLog);
		upd.constructSourceList(testCallback.updaterConfiguration().m_sourceList, upd.m_sourceList);

		HANDLE hDoDownload;
		doEntryDownloader do_dld(&upd, hDoDownload, _fi, _fi.m_localPath, &s6c);

		WaitForSingleObject (hDoDownload, INFINITE);

		QC_BOOST_CHECK_MESSAGE ( CORE_NO_ERROR == do_dld.GetError(), L"Wrong return code\n" );
		QC_BOOST_CHECK_MESSAGE ( _helper::AreFilesEqual (ETALON_NET_FILE, std::wstring(TEST_FOLDER).append(L"\\base004.avc")),
			L"Incorrect file\n" );
	}
}


void TestSuiteNet::TestHttpRelativePath ()
{
	std::wcout<<L"TestHttpRelativePath\n";
	Logger::Case _case (m_logger, m_testp, m_step, L"TestHttpRelativePath");

	_helper::DeleteDirectory(TEST_FOLDER);
	_helper::MakeDirectory(TEST_FOLDER);

	TestLog testLog;
	Callbacks cbacks;
	TestCallbackInterface testCallback (&testLog, &cbacks);
	TestJournalInterface testJournal;
	Signature6Checker s6c (&testLog);

	FileInfo _fi (L"base005rel.avc", L"/relative");
	_fi.m_localPath = L"h:\\test\\";

	UpdaterConfiguration data(1, 1, L"1");
	data.m_sourceList.addSource (UST_UserURL, L"http://tl-srv-w2k", false, L"");
	testCallback.SetUpdaterConfiguration (data);

	Updater upd (testCallback.m_dp, testCallback, testJournal, &testLog);
	upd.constructSourceList(testCallback.updaterConfiguration().m_sourceList, upd.m_sourceList);

	HANDLE hDoDownload;
	doEntryDownloader do_dld(&upd, hDoDownload, _fi, _fi.m_localPath, &s6c);

	WaitForSingleObject (hDoDownload, INFINITE);

	QC_BOOST_CHECK_MESSAGE ( CORE_NO_ERROR == do_dld.GetError(), L"Wrong return code\n" );
	QC_BOOST_CHECK_MESSAGE ( _helper::AreFilesEqual (ETALON_NET_FILE, std::wstring(TEST_FOLDER).append(L"\\base005rel.avc")),
		L"Incorrect file\n" );
}

void TestSuiteNet::TestFtpRelativePath()
{
	std::wcout<<L"TestFtpRelativePath\n";
	Logger::Case _case (m_logger, m_testp, m_step, L"TestFtpRelativePath");
	{
		_helper::DeleteDirectory(TEST_FOLDER);
		_helper::MakeDirectory(TEST_FOLDER);

		TestLog testLog;
		Callbacks cbacks;
		TestCallbackInterface testCallback (&testLog, &cbacks);
		TestJournalInterface testJournal;
		Signature6Checker s6c (&testLog);

		FileInfo _fi (L"base005rel.avc", L"/relative");
		_fi.m_localPath = L"h:\\test\\";

		UpdaterConfiguration data(1, 1, L"1");
		testCallback.SetUpdaterConfiguration (data);

		Updater upd (testCallback.m_dp, testCallback, testJournal, &testLog);
		QC_BOOST_CHECK_MESSAGE ( CORE_NO_ERROR == upd.DoGetRemoteFile_SelfNet (_fi.m_filename, _fi.m_localPath, _fi.m_relativeURLPath, L"ftp://test:test@tl-srv-w2k"),
			L"Wrong return code\n" );
		QC_BOOST_CHECK_MESSAGE ( _helper::AreFilesEqual (ETALON_NET_FILE, std::wstring(TEST_FOLDER).append(L"\\base005rel.avc")),
			L"Incorrect file\n" );
	}

	{
		_helper::DeleteDirectory(TEST_FOLDER);
		_helper::MakeDirectory(TEST_FOLDER);

		TestLog testLog;
		Callbacks cbacks;
		TestCallbackInterface testCallback (&testLog, &cbacks);
		TestJournalInterface testJournal;
		Signature6Checker s6c (&testLog);

		FileInfo _fi (L"base005rel.avc", L"/relative");
		_fi.m_localPath = L"h:\\test\\";

		UpdaterConfiguration data(1, 1, L"1");
		testCallback.SetUpdaterConfiguration (data);

		Updater upd (testCallback.m_dp, testCallback, testJournal, &testLog);
		QC_BOOST_CHECK_MESSAGE ( CORE_NO_ERROR == upd.DoGetRemoteFile_SelfNet (_fi.m_filename, _fi.m_localPath, _fi.m_relativeURLPath, L"ftp://tl-srv-w2k"),
			L"Wrong return code\n" );
		QC_BOOST_CHECK_MESSAGE ( _helper::AreFilesEqual (ETALON_NET_FILE, std::wstring(TEST_FOLDER).append(L"\\base005rel.avc")),
			L"Incorrect file\n" );
	}
}

void TestSuiteNet::TestHttpRegetting()
{
	std::wcout<<L"TestHttpRegetting\n";
	Logger::Case _case (m_logger, m_testp, m_step, L"TestHttpRegetting");

	{
		std::wcout<<L"IIS 3.0\n";

		_helper::DeleteDirectory(TEST_FOLDER);
		_helper::MakeDirectory(TEST_FOLDER);
		_helper::CloneFile (ETALON_HALF_NET_FILE, std::wstring(TEST_FOLDER).append(L"\\base005mod.avc"), false);

		TestLog testLog;
		Callbacks cbacks;
		TestCallbackInterface testCallback (&testLog, &cbacks);
		TestJournalInterface testJournal;
		Signature6Checker s6c (&testLog);

		FileInfo _fi (L"base005mod.avc", L"/");
		_fi.m_localPath = L"h:\\test\\";

		UpdaterConfiguration data(1, 1, L"1");
		data.m_sourceList.addSource (UST_UserURL, L"http://tl-srv-wnt", false, L"");
		testCallback.SetUpdaterConfiguration (data);

		Updater upd (testCallback.m_dp, testCallback, testJournal, &testLog);
		upd.constructSourceList(testCallback.updaterConfiguration().m_sourceList, upd.m_sourceList);

		HANDLE hDoDownload;
		doEntryDownloader do_dld(&upd, hDoDownload, _fi, _fi.m_localPath, &s6c);

		WaitForSingleObject (hDoDownload, INFINITE);

		QC_BOOST_CHECK_MESSAGE ( CORE_NO_ERROR == do_dld.GetError(), L"Wrong return code\n" );
		QC_BOOST_CHECK_MESSAGE ( _helper::AreFilesEqual (ETALON_NET_FILE, std::wstring(TEST_FOLDER).append(L"\\base005mod.avc")),
			L"Incorrect file\n" );
	}
	{
		std::wcout<<L"Apache 1.3 win\n";

		_helper::DeleteDirectory(TEST_FOLDER);
		_helper::MakeDirectory(TEST_FOLDER);
		_helper::CloneFile (ETALON_HALF_NET_FILE, std::wstring(TEST_FOLDER).append(L"\\base005mod.avc"), false);

		TestLog testLog;
		Callbacks cbacks;
		TestCallbackInterface testCallback (&testLog, &cbacks);
		TestJournalInterface testJournal;
		Signature6Checker s6c (&testLog);

		FileInfo _fi (L"base005mod.avc", L"/");
		_fi.m_localPath = L"h:\\test\\";

		UpdaterConfiguration data(1, 1, L"1");
		data.m_sourceList.addSource (UST_UserURL, L"http://tl-srv-wnt:8081", false, L"");
		testCallback.SetUpdaterConfiguration (data);

		Updater upd (testCallback.m_dp, testCallback, testJournal, &testLog);
		upd.constructSourceList(testCallback.updaterConfiguration().m_sourceList, upd.m_sourceList);

		HANDLE hDoDownload;
		doEntryDownloader do_dld(&upd, hDoDownload, _fi, _fi.m_localPath, &s6c);

		WaitForSingleObject (hDoDownload, INFINITE);

		QC_BOOST_CHECK_MESSAGE ( CORE_NO_ERROR == do_dld.GetError(), L"Wrong return code\n" );
		QC_BOOST_CHECK_MESSAGE ( _helper::AreFilesEqual (ETALON_NET_FILE, std::wstring(TEST_FOLDER).append(L"\\base005mod.avc")),
			L"Incorrect file\n" );
	}

	{
		std::wcout<<L"IIS 5.0\n";

		_helper::DeleteDirectory(TEST_FOLDER);
		_helper::MakeDirectory(TEST_FOLDER);
		_helper::CloneFile (ETALON_HALF_NET_FILE, std::wstring(TEST_FOLDER).append(L"\\base005mod.avc"), false);

		TestLog testLog;
		Callbacks cbacks;
		TestCallbackInterface testCallback (&testLog, &cbacks);
		TestJournalInterface testJournal;
		Signature6Checker s6c (&testLog);

		FileInfo _fi (L"base005mod.avc", L"/");
		_fi.m_localPath = L"h:\\test\\";

		UpdaterConfiguration data(1, 1, L"1");
		data.m_sourceList.addSource (UST_UserURL, L"http://tl-srv-w2k", false, L"");
		testCallback.SetUpdaterConfiguration (data);

		Updater upd (testCallback.m_dp, testCallback, testJournal, &testLog);
		upd.constructSourceList(testCallback.updaterConfiguration().m_sourceList, upd.m_sourceList);

		HANDLE hDoDownload;
		doEntryDownloader do_dld(&upd, hDoDownload, _fi, _fi.m_localPath, &s6c);

		WaitForSingleObject (hDoDownload, INFINITE);

		QC_BOOST_CHECK_MESSAGE ( CORE_NO_ERROR == do_dld.GetError(), L"Wrong return code\n" );
		QC_BOOST_CHECK_MESSAGE ( _helper::AreFilesEqual (ETALON_NET_FILE, std::wstring(TEST_FOLDER).append(L"\\base005mod.avc")),
			L"Incorrect file\n" );
	}
	{
		std::wcout<<L"Apache 2.0 win\n";

		_helper::DeleteDirectory(TEST_FOLDER);
		_helper::MakeDirectory(TEST_FOLDER);
		_helper::CloneFile (ETALON_HALF_NET_FILE, std::wstring(TEST_FOLDER).append(L"\\base005mod.avc"), false);

		TestLog testLog;
		Callbacks cbacks;
		TestCallbackInterface testCallback (&testLog, &cbacks);
		TestJournalInterface testJournal;
		Signature6Checker s6c (&testLog);

		FileInfo _fi (L"base005mod.avc", L"/");
		_fi.m_localPath = L"h:\\test\\";

		UpdaterConfiguration data(1, 1, L"1");
		data.m_sourceList.addSource (UST_UserURL, L"http://tl-srv-w2k:8081", false, L"");
		testCallback.SetUpdaterConfiguration (data);

		Updater upd (testCallback.m_dp, testCallback, testJournal, &testLog);
		upd.constructSourceList(testCallback.updaterConfiguration().m_sourceList, upd.m_sourceList);

		HANDLE hDoDownload;
		doEntryDownloader do_dld(&upd, hDoDownload, _fi, _fi.m_localPath, &s6c);

		WaitForSingleObject (hDoDownload, INFINITE);

		QC_BOOST_CHECK_MESSAGE ( CORE_NO_ERROR == do_dld.GetError(), L"Wrong return code\n" );
		QC_BOOST_CHECK_MESSAGE ( _helper::AreFilesEqual (ETALON_NET_FILE, std::wstring(TEST_FOLDER).append(L"\\base005mod.avc")),
			L"Incorrect file\n" );
	}

	{
		std::wcout<<L"IIS 6.0\n";

		_helper::DeleteDirectory(TEST_FOLDER);
		_helper::MakeDirectory(TEST_FOLDER);
		_helper::CloneFile (ETALON_HALF_NET_FILE, std::wstring(TEST_FOLDER).append(L"\\base005mod.avc"), false);

		TestLog testLog;
		Callbacks cbacks;
		TestCallbackInterface testCallback (&testLog, &cbacks);
		TestJournalInterface testJournal;
		Signature6Checker s6c (&testLog);

		FileInfo _fi (L"base005mod.avc", L"/");
		_fi.m_localPath = L"h:\\test\\";

		UpdaterConfiguration data(1, 1, L"1");
		data.m_sourceList.addSource (UST_UserURL, L"http://tl-srv-w03", false, L"");
		testCallback.SetUpdaterConfiguration (data);

		Updater upd (testCallback.m_dp, testCallback, testJournal, &testLog);
		upd.constructSourceList(testCallback.updaterConfiguration().m_sourceList, upd.m_sourceList);

		HANDLE hDoDownload;
		doEntryDownloader do_dld(&upd, hDoDownload, _fi, _fi.m_localPath, &s6c);

		WaitForSingleObject (hDoDownload, INFINITE);

		QC_BOOST_CHECK_MESSAGE ( CORE_NO_ERROR == do_dld.GetError(), L"Wrong return code\n" );
		QC_BOOST_CHECK_MESSAGE ( _helper::AreFilesEqual (ETALON_NET_FILE, std::wstring(TEST_FOLDER).append(L"\\base005mod.avc")),
			L"Incorrect file\n" );
	}
	{
		std::wcout<<L"Apache 2.2 win\n";

		_helper::DeleteDirectory(TEST_FOLDER);
		_helper::MakeDirectory(TEST_FOLDER);
		_helper::CloneFile (ETALON_HALF_NET_FILE, std::wstring(TEST_FOLDER).append(L"\\base005mod.avc"), false);

		TestLog testLog;
		Callbacks cbacks;
		TestCallbackInterface testCallback (&testLog, &cbacks);
		TestJournalInterface testJournal;
		Signature6Checker s6c (&testLog);

		FileInfo _fi (L"base005mod.avc", L"/");
		_fi.m_localPath = L"h:\\test\\";

		UpdaterConfiguration data(1, 1, L"1");
		data.m_sourceList.addSource (UST_UserURL, L"http://tl-srv-w03:8081", false, L"");
		testCallback.SetUpdaterConfiguration (data);

		Updater upd (testCallback.m_dp, testCallback, testJournal, &testLog);
		upd.constructSourceList(testCallback.updaterConfiguration().m_sourceList, upd.m_sourceList);

		HANDLE hDoDownload;
		doEntryDownloader do_dld(&upd, hDoDownload, _fi, _fi.m_localPath, &s6c);

		WaitForSingleObject (hDoDownload, INFINITE);

		QC_BOOST_CHECK_MESSAGE ( CORE_NO_ERROR == do_dld.GetError(), L"Wrong return code\n" );
		QC_BOOST_CHECK_MESSAGE ( _helper::AreFilesEqual (ETALON_NET_FILE, std::wstring(TEST_FOLDER).append(L"\\base005mod.avc")),
			L"Incorrect file\n" );
	}

	{
		std::wcout<<L"IIS 5.1\n";

		_helper::DeleteDirectory(TEST_FOLDER);
		_helper::MakeDirectory(TEST_FOLDER);
		_helper::CloneFile (ETALON_HALF_NET_FILE, std::wstring(TEST_FOLDER).append(L"\\base005mod.avc"), false);

		TestLog testLog;
		Callbacks cbacks;
		TestCallbackInterface testCallback (&testLog, &cbacks);
		TestJournalInterface testJournal;
		Signature6Checker s6c (&testLog);

		FileInfo _fi (L"base005mod.avc", L"/");
		_fi.m_localPath = L"h:\\test\\";

		UpdaterConfiguration data(1, 1, L"1");
		data.m_sourceList.addSource (UST_UserURL, L"http://tl-srv-wxp", false, L"");
		testCallback.SetUpdaterConfiguration (data);

		Updater upd (testCallback.m_dp, testCallback, testJournal, &testLog);
		upd.constructSourceList(testCallback.updaterConfiguration().m_sourceList, upd.m_sourceList);

		HANDLE hDoDownload;
		doEntryDownloader do_dld(&upd, hDoDownload, _fi, _fi.m_localPath, &s6c);

		WaitForSingleObject (hDoDownload, INFINITE);

		QC_BOOST_CHECK_MESSAGE ( CORE_NO_ERROR == do_dld.GetError(), L"Wrong return code\n" );
		QC_BOOST_CHECK_MESSAGE ( _helper::AreFilesEqual (ETALON_NET_FILE, std::wstring(TEST_FOLDER).append(L"\\base005mod.avc")),
			L"Incorrect file\n" );
	}

/*	{//нет пока такого сервера
		std::wcout<<L"Apache 2.0 lnx\n";

		_helper::DeleteDirectory(TEST_FOLDER);
		_helper::MakeDirectory(TEST_FOLDER);
		_helper::CloneFile (ETALON_HALF_NET_FILE, std::wstring(TEST_FOLDER).append(L"\\base005mod.avc"), false);

		TestLog testLog;
		Callbacks cbacks;
		TestCallbackInterface testCallback (&testLog, &cbacks);
		TestJournalInterface testJournal;
		Signature6Checker s6c (&testLog);

		FileInfo _fi (L"base005mod.avc", L"/");
		_fi.m_localPath = L"h:\\test\\";

		UpdaterConfiguration data(1, 1, L"1");
		data.m_sourceList.addSource (UST_UserURL, L"ftp://tl-srv-lnx", false, L"");
		testCallback.SetUpdaterConfiguration (data);

		Updater upd (testCallback.m_dp, testCallback, testJournal, &testLog);
		upd.constructSourceList(testCallback.updaterConfiguration().m_sourceList, upd.m_sourceList);

		HANDLE hDoDownload;
		doEntryDownloader do_dld(&upd, hDoDownload, _fi, _fi.m_localPath, &s6c);

		WaitForSingleObject (hDoDownload, INFINITE);

		QC_BOOST_CHECK_MESSAGE ( CORE_NO_ERROR == do_dld.GetError(), L"Wrong return code\n" );
		QC_BOOST_CHECK_MESSAGE ( _helper::AreFilesEqual (ETALON_NET_FILE, std::wstring(TEST_FOLDER).append(L"\\base005mod.avc")),
			L"Incorrect file\n" );
	}*/

}

void TestSuiteNet::TestFtpRegetting()
{
	std::wcout<<L"TestFtpRegetting\n";
	Logger::Case _case (m_logger, m_testp, m_step, L"TestFtpRegetting");
	/*
	{
	std::wcout<<L"IIS 3.0\n";//regetting is not supported

	_helper::DeleteDirectory(TEST_FOLDER);
	_helper::MakeDirectory(TEST_FOLDER);
	_helper::CloneFile (ETALON_HALF_NET_FILE, std::wstring(TEST_FOLDER).append(L"\\base005mod.avc"), false);

	TestLog testLog;
	Callbacks cbacks;
	TestCallbackInterface testCallback (&testLog, &cbacks);
	TestJournalInterface testJournal;
	Signature6Checker s6c (&testLog);

	FileInfo _fi (L"base005mod.avc", L"/");
	NetCore _nc (testCallback, testJournal, &testLog);
	_nc.SetUrl(L"ftp://tl-srv-wnt");
	_nc.setFileInfo(_fi);
	_nc.setTempPath(L"h:\\test\\");

	UpdaterConfiguration data(1, 1, L"1");
	testCallback.SetUpdaterConfiguration (data);

	KLUPD::CoreError error = _nc.GetFile();

	QC_BOOST_CHECK_MESSAGE (error == CORE_NO_ERROR, L"Incorrect getFile result\n");
	QC_BOOST_CHECK_MESSAGE ( _helper::AreFilesEqual (ETALON_MOD_NET_FILE, std::wstring(TEST_FOLDER).append(L"\\base005mod.avc")),
	L"Incorrect file\n" );
	}
	*/
	{
		std::wcout<<L"IIS 5.0\n";

		_helper::DeleteDirectory(TEST_FOLDER);
		_helper::MakeDirectory(TEST_FOLDER);
		_helper::CloneFile (ETALON_HALF_NET_FILE, std::wstring(TEST_FOLDER).append(L"\\base005mod.avc"), false);

		TestLog testLog;
		Callbacks cbacks;
		TestCallbackInterface testCallback (&testLog, &cbacks);
		TestJournalInterface testJournal;
		Signature6Checker s6c (&testLog);

		FileInfo _fi (L"base005mod.avc", L"/");
		_fi.m_localPath = L"h:\\test\\";

		UpdaterConfiguration data(1, 1, L"1");
		testCallback.SetUpdaterConfiguration (data);

		Updater upd (testCallback.m_dp, testCallback, testJournal, &testLog);
		QC_BOOST_CHECK_MESSAGE ( CORE_NO_ERROR == upd.DoGetRemoteFile_SelfNet (_fi.m_filename, _fi.m_localPath, _fi.m_relativeURLPath, L"ftp://tl-srv-w2k"),
		L"Wrong return code\n" );
		QC_BOOST_CHECK_MESSAGE ( _helper::AreFilesEqual (ETALON_NET_FILE, std::wstring(TEST_FOLDER).append(L"\\base005mod.avc")),
		L"Incorrect file\n" );
	}

	{
		std::wcout<<L"IIS 6.0\n";

		_helper::DeleteDirectory(TEST_FOLDER);
		_helper::MakeDirectory(TEST_FOLDER);
		_helper::CloneFile (ETALON_HALF_NET_FILE, std::wstring(TEST_FOLDER).append(L"\\base005mod.avc"), false);

		TestLog testLog;
		Callbacks cbacks;
		TestCallbackInterface testCallback (&testLog, &cbacks);
		TestJournalInterface testJournal;
		Signature6Checker s6c (&testLog);

		FileInfo _fi (L"base005mod.avc", L"/");
		_fi.m_localPath = L"h:\\test\\";

		UpdaterConfiguration data(1, 1, L"1");
		testCallback.SetUpdaterConfiguration (data);

		Updater upd (testCallback.m_dp, testCallback, testJournal, &testLog);
		QC_BOOST_CHECK_MESSAGE ( CORE_NO_ERROR == upd.DoGetRemoteFile_SelfNet (_fi.m_filename, _fi.m_localPath, _fi.m_relativeURLPath, L"ftp://tl-srv-w03"),
		L"Wrong return code\n" );
		QC_BOOST_CHECK_MESSAGE ( _helper::AreFilesEqual (ETALON_NET_FILE, std::wstring(TEST_FOLDER).append(L"\\base005mod.avc")),
		L"Incorrect file\n" );
	}

	{
		std::wcout<<L"IIS 5.1\n";

		_helper::DeleteDirectory(TEST_FOLDER);
		_helper::MakeDirectory(TEST_FOLDER);
		_helper::CloneFile (ETALON_HALF_NET_FILE, std::wstring(TEST_FOLDER).append(L"\\base005mod.avc"), false);

		TestLog testLog;
		Callbacks cbacks;
		TestCallbackInterface testCallback (&testLog, &cbacks);
		TestJournalInterface testJournal;
		Signature6Checker s6c (&testLog);

		FileInfo _fi (L"base005mod.avc", L"/");
		_fi.m_localPath = L"h:\\test\\";

		UpdaterConfiguration data(1, 1, L"1");
		testCallback.SetUpdaterConfiguration (data);

		Updater upd (testCallback.m_dp, testCallback, testJournal, &testLog);
		QC_BOOST_CHECK_MESSAGE ( CORE_NO_ERROR == upd.DoGetRemoteFile_SelfNet (_fi.m_filename, _fi.m_localPath, _fi.m_relativeURLPath, L"ftp://tl-srv-wxp"),
		L"Wrong return code\n" );
		QC_BOOST_CHECK_MESSAGE ( _helper::AreFilesEqual (ETALON_NET_FILE, std::wstring(TEST_FOLDER).append(L"\\base005mod.avc")),
		L"Incorrect file\n" );
	}
	
	{
		//все это для того, чтобы проверить, что при сбое докачки по вине сервере файл будет перезакачен целиком
		std::wcout<<L"TT#19002, bug in ftp-server\n";
		std::wcout<<L"Cerberus\n";

		_helper::DeleteDirectory(TEST_FOLDER);
		_helper::MakeDirectory(TEST_FOLDER);
		_helper::CloneFile (ETALON_HALF_NET_FILE, std::wstring(TEST_FOLDER).append(L"\\base005mod.avc"), false);

		TestLog testLog;
		Callbacks cbacks;
		TestCallbackInterface testCallback (&testLog, &cbacks);
		TestJournalInterface testJournal;
		Signature6Checker s6c (&testLog);

		FileInfo _fi (L"base005mod.avc", L"/");
		_fi.m_localPath = L"h:\\test\\";

		UpdaterConfiguration data(1, 1, L"1");
		data.m_sourceList.addSource (UST_UserURL, L"ftp://tl-srv-w03:2121/", false, L"diffs");
		testCallback.SetUpdaterConfiguration (data);
		Updater upd (testCallback.m_dp, testCallback, testJournal, &testLog);
		upd.constructSourceList(testCallback.updaterConfiguration().m_sourceList, upd.m_sourceList);

		HANDLE hDoDownload;
		doEntryDownloader do_dld(&upd, hDoDownload, _fi, _fi.m_localPath, &s6c);

		WaitForSingleObject (hDoDownload, INFINITE);

		QC_BOOST_CHECK_MESSAGE ( CORE_NO_ERROR == do_dld.GetError(), L"Wrong return code\n" );
		QC_BOOST_CHECK_MESSAGE ( _helper::AreFilesEqual (ETALON_MOD_NET_FILE, std::wstring(TEST_FOLDER).append(L"\\base005mod.avc")),
			L"Incorrect file\n" );
	}
	
	/*{//пока не работает
		std::wcout<<L"VSFtpd 2.3 linux\n";

		_helper::DeleteDirectory(TEST_FOLDER);
		_helper::MakeDirectory(TEST_FOLDER);
		_helper::CloneFile (ETALON_HALF_NET_FILE, std::wstring(TEST_FOLDER).append(L"\\base005mod.avc"), false);

		TestLog testLog;
		Callbacks cbacks;
		TestCallbackInterface testCallback (&testLog, &cbacks);
		TestJournalInterface testJournal;
		Signature6Checker s6c (&testLog);

		FileInfo _fi (L"base005mod.avc", L"/");
		_fi.m_localPath = L"h:\\test\\";

		UpdaterConfiguration data(1, 1, L"1");
		testCallback.SetUpdaterConfiguration (data);

		Updater upd (testCallback.m_dp, testCallback, testJournal, &testLog);
		QC_BOOST_CHECK_MESSAGE ( CORE_NO_ERROR == upd.DoGetRemoteFile_SelfNet (_fi.m_filename, _fi.m_localPath, _fi.m_relativeURLPath, L"ftp://tl-srv-lnx"),
		L"Wrong return code\n" );
		QC_BOOST_CHECK_MESSAGE ( _helper::AreFilesEqual (ETALON_NET_FILE, std::wstring(TEST_FOLDER).append(L"\\base005mod.avc")),
		L"Incorrect file\n" );
	}*/
	
}


void TestSuiteNet::TestProxy_172_16_10_108()
{
	std::wcout<<L"TestProxy_172_16_10_108\n";
	Logger::Case _case (m_logger, m_testp, m_step, L"TestProxy_172_16_10_108");

	{
		std::wcout<<L"Specific proxy server\n";

		_helper::DeleteDirectory(TEST_FOLDER);
		_helper::MakeDirectory(TEST_FOLDER);

		TestLog testLog;
		Callbacks cbacks;
		TestCallbackInterface testCallback (&testLog, &cbacks);
		TestJournalInterface testJournal;
		Signature6Checker s6c (&testLog);

		FileInfo _fi (L"base005.avc", L"/");
		_fi.m_localPath = L"h:\\test\\";

		UpdaterConfiguration data(1, 1, L"1");
		data.proxy_url = L"172.16.10.108";
		data.proxy_port = 3128;
		data.m_proxyAuthorizationCredentials.userName ( L"test");
		data.m_proxyAuthorizationCredentials.password ( L"test");
		data.m_httpAuthorizationMethods.push_back (basicAuthorization);
		data.m_sourceList.addSource (UST_UserURL, L"http://172.16.10.175", true, L"diffs");
		testCallback.SetUpdaterConfiguration (data);
		Updater upd (testCallback.m_dp, testCallback, testJournal, &testLog);
		upd.constructSourceList(testCallback.updaterConfiguration().m_sourceList, upd.m_sourceList);

		HANDLE hDoDownload;
		doEntryDownloader do_dld(&upd, hDoDownload, _fi, _fi.m_localPath, &s6c);

		WaitForSingleObject (hDoDownload, INFINITE);

		QC_BOOST_CHECK_MESSAGE ( CORE_NO_ERROR == do_dld.GetError(), L"Wrong return code\n" );
		QC_BOOST_CHECK_MESSAGE ( _helper::AreFilesEqual (ETALON_NET_FILE, std::wstring(TEST_FOLDER).append(L"\\base005.avc")),
			L"Incorrect file\n" );
	}
}

void TestSuiteNet::TestFtpActivePassive()
{
	std::wcout<<L"TestFtpActivePassive\n";
	Logger::Case _case (m_logger, m_testp, m_step, L"TestFtpActivePassive");

	{
		std::wcout<<L"Passive\n";

		_helper::DeleteDirectory(TEST_FOLDER);
		_helper::MakeDirectory(TEST_FOLDER);

		TestLog testLog;
		Callbacks cbacks;
		TestCallbackInterface testCallback (&testLog, &cbacks);
		TestJournalInterface testJournal;
		Signature6Checker s6c (&testLog);

		FileInfo _fi (L"base005.avc", L"/");
		_fi.m_localPath = L"h:\\test\\";

		UpdaterConfiguration data(1, 1, L"1");
		data.passive_ftp = true;
		testCallback.SetUpdaterConfiguration (data);

		Updater upd (testCallback.m_dp, testCallback, testJournal, &testLog);
		QC_BOOST_CHECK_MESSAGE ( CORE_NO_ERROR == upd.DoGetRemoteFile_SelfNet (_fi.m_filename, _fi.m_localPath, _fi.m_relativeURLPath, L"ftp://tl-srv-w03"),
			L"Wrong return code\n" );
		QC_BOOST_CHECK_MESSAGE ( _helper::AreFilesEqual (ETALON_NET_FILE, std::wstring(TEST_FOLDER).append(L"\\base005.avc")),
			L"Incorrect file\n" );
	}
	{
		std::wcout<<L"Active\n";

		_helper::DeleteDirectory(TEST_FOLDER);
		_helper::MakeDirectory(TEST_FOLDER);

		TestLog testLog;
		Callbacks cbacks;
		TestCallbackInterface testCallback (&testLog, &cbacks);
		TestJournalInterface testJournal;
		Signature6Checker s6c (&testLog);

		FileInfo _fi (L"base005.avc", L"/");
		_fi.m_localPath = L"h:\\test\\";

		UpdaterConfiguration data(1, 1, L"1");
		data.passive_ftp = false;
		testCallback.SetUpdaterConfiguration (data);

		Updater upd (testCallback.m_dp, testCallback, testJournal, &testLog);
		QC_BOOST_CHECK_MESSAGE ( CORE_NO_ERROR == upd.DoGetRemoteFile_SelfNet (_fi.m_filename, _fi.m_localPath, _fi.m_relativeURLPath, L"ftp://tl-srv-w03"),
			L"Wrong return code\n" );
		QC_BOOST_CHECK_MESSAGE ( _helper::AreFilesEqual (ETALON_NET_FILE, std::wstring(TEST_FOLDER).append(L"\\base005.avc")),
			L"Incorrect file\n" );
	}
}
/*
void TestSuiteNet::TestRegettingFailed()
{
	std::wcout<<L"TestRegettingFailed\n";
	Logger::Case _case (m_logger, m_testp, m_step, L"TestRegettingFailed");
	//тест низкого приоритета, в нерабочем состоянии
	{
		HttpServer hs (8001, "127.0.0.1");

		_helper::DeleteDirectory(TEST_FOLDER);
		_helper::MakeDirectory(TEST_FOLDER);
		_helper::CloneFile (ETALON_HALF_NET_FILE, std::wstring(TEST_FOLDER).append(L"\\base005mod_fake.avc"), false);
		_helper::AutoFile af ( std::wstring(TEST_FOLDER).append(L"\\base005mod_fake.avc"), GENERIC_READ, FILE_SHARE_WRITE|FILE_SHARE_READ );

		TestLog testLog;
		Callbacks cbacks;
		TestCallbackInterface testCallback (&testLog, &cbacks);
		TestJournalInterface testJournal;
		Signature6Checker s6c (&testLog);

		FileInfo _fi (L"base005mod_fake.avc", L"/");
		_fi.m_localPath = L"h:\\test\\";

		UpdaterConfiguration data(1, 1, L"1");
		testCallback.SetUpdaterConfiguration (data);
		KLUPD::Source _csd (L"http://tl-srv-w2k", UST_UserURL, false, L"", UPS_DIRS);
		data.m_sourceList.addSource (UST_UserURL, L"http://127.0.0.1:8001", false, L"diffs");
		testCallback.SetUpdaterConfiguration (data);
		Updater upd (testCallback.m_dp, testCallback, testJournal, &testLog);
		upd.constructSourceList(testCallback.updaterConfiguration().m_sourceList, upd.m_sourceList);

		//лочим файл, который будет дозаписываться
		hs.Accept();
		hs.Receive(command, type, param);
		QC_BOOST_CHECK_MESSAGE (type == HttpServer::CMD_GET, L"Error\n");
		_ASSERTE (command.find (L"av-i386-0607g.xml")!= wstring::npos);
		hs.SendFile (std::wstring(COLL_ROOT).append(L"\\Bases_1\\plain\\av-i386-0607g.xml"), CLOSE_CONNECTION);
		hs.CloseConnection ();


		QC_BOOST_CHECK_MESSAGE ( CORE_NO_SOURCE_FILE == upd.downloadEntry(_fi, _fi.m_localPath),
			L"Wrong return code\n" );
	}
}
*/

void TestSuiteNet::TestProxyCredentialsCache ()
{
	std::wcout<<L"TestProxyCredentialsCache\n";
	Logger::Case _case (m_logger, m_testp, m_step, L"TestProxyCredentialsCache");
	{
		_helper::DeleteDirectory(TEST_FOLDER);
		_helper::MakeDirectory(TEST_FOLDER);

		TestLog testLog;
		Callbacks cbacks;
		TestCallbackInterface testCallback (&testLog, &cbacks);
		TestJournalInterface testJournal;
		Signature6Checker s6c (&testLog);

		FileInfo _fi (L"base005.avc", L"/");
		_fi.m_localPath = L"h:\\test\\";

		UpdaterConfiguration data(1, 1, L"1");
		data.m_sourceList.addSource (UST_UserURL, L"http://tl-srv-w2k", true, L"diffs");
		data.proxy_url = L"tl-prx-lnx";
		data.proxy_port = 3128;
		data.m_proxyAuthorizationCredentials.userName ( L"test1");
		data.m_proxyAuthorizationCredentials.password ( L"test1");
		data.m_httpAuthorizationMethods.push_back (basicAuthorization);
		testCallback.SetUpdaterConfiguration (data);
		Updater upd (testCallback.m_dp, testCallback, testJournal, &testLog);
		upd.constructSourceList(testCallback.updaterConfiguration().m_sourceList, upd.m_sourceList);

		cbacks.bFlags[Callbacks::requestCredentialsForAuthorizationOnProxy] = true;
		CREATE_EVENTS(requestCredentialsForAuthorizationOnProxy);

		HANDLE hDoDownload;
		doEntryDownloader do_dld(&upd, hDoDownload, _fi, _fi.m_localPath, &s6c);

		WAIT_FOR_READY_EVENT(requestCredentialsForAuthorizationOnProxy)
			TestCallbackInterface::_credentials creds;
		creds.name = L"test";
		creds.password = L"test";
		testCallback.SetRequestCreds(creds);
		SET_GOON_EVENT(requestCredentialsForAuthorizationOnProxy);

		WaitForSingleObject (hDoDownload, INFINITE);

		QC_BOOST_CHECK_MESSAGE ( CORE_NO_ERROR == do_dld.GetError(), L"Wrong return code\n" );
		QC_BOOST_CHECK_MESSAGE ( _helper::AreFilesEqual (ETALON_NET_FILE, std::wstring(TEST_FOLDER).append(L"\\base005.avc")),
			L"Incorrect file\n" );
	}
}


void TestSuiteNet::TestBypassProxyForLocalAddresses ()
{
	using namespace KLUPD;
	std::wcout<<L"TestBypassProxyForLocalAddresses\n";
	Logger::Case _case (m_logger, m_testp, m_step, L"TestBypassProxyForLocalAddresses");

	{//without wpad
		TestLog testLog;

		HttpServer hs_proxy (3127, "127.0.0.1");
		hs_proxy.Start();
		HttpServer hs_server (8001, _helper::GetSelfIp());
		hs_server.Start();

		_helper::DeleteDirectory(ROOT);
		KLUPD::createFolder (BASES_ROOT, &testLog);
		KLUPD::createFolder (UPD_ROOT, &testLog);

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
		data.connect_tmo = 1000;

		data.m_sourceList.addSource (UST_UserURL, L"http://kiryukhin-xp_ipv4:8001/", true, L"");
		//data.m_sourceList.addSource (UST_UserURL, L"http://steady/", true, L"");
		data.m_sourceList.addSource (UST_UserURL, L"http://kaspersky.ru/", true, L"");

		data.m_proxyAuthorizationCredentials.userName ( L"null");
		data.m_proxyAuthorizationCredentials.password ( L"null");
		data.proxy_url = L"127.0.0.1";
		data.proxy_port = 3127;
		data.use_ie_proxy = false;
		data.m_bypassProxyServerForLocalAddresses = true;
		data.m_httpAuthorizationMethods.push_back (basicAuthorization);
		data.UpdaterListFlags.update_bases = true;

		Callbacks cbacks;
		TestCallbackInterface testCallback (&testLog, &cbacks);
		TestJournalInterface journal;
		testCallback.SetUpdaterConfiguration (data);

		Updater upd (testCallback.m_dp, testCallback, journal, &testLog);

		HANDLE hDoUpdate;

		doUpdater do_upd(&upd, hDoUpdate);

		string command;
		string param;
		HttpServer::CMD_TYPE type;

		//first source
		{	//klz
			hs_server.Accept();
			hs_server.Receive(command, type, param);
			QC_BOOST_CHECK_MESSAGE (type == HttpServer::CMD_GET, L"Error\n");
			QC_BOOST_CHECK_MESSAGE (command.find("host: kiryukhin-xp_ipv4") != wstring::npos, L"Error\n");
			hs_server.Send("HTTP/1.1 200 OK\x0d\x0a");
			hs_server.Send("Content-Length: 10\x0d\x0a");
			hs_server.Send("Connection: Close\x0d\x0a");
			hs_server.Send("\x0d\x0a");
			hs_server.Send("0123456789");
			hs_server.CloseConnection();

			//xml
			hs_server.Accept();
			hs_server.Receive(command, type, param);
			QC_BOOST_CHECK_MESSAGE (type == HttpServer::CMD_GET, L"Error\n");
			QC_BOOST_CHECK_MESSAGE (command.find("host: kiryukhin-xp_ipv4") != wstring::npos, L"Error\n");
			hs_server.Send("HTTP/1.1 200 OK\x0d\x0a");
			hs_server.Send("Content-Length: 10\x0d\x0a");
			hs_server.Send("Connection: Close\x0d\x0a");
			hs_server.Send("\x0d\x0a");
			hs_server.Send("0123456789");
			hs_server.CloseConnection();

		}

		//second source
		{	//klz
			hs_proxy.Accept();
			hs_proxy.Receive(command, type, param);
			QC_BOOST_CHECK_MESSAGE (type == HttpServer::CMD_GET, L"Error\n");
			QC_BOOST_CHECK_MESSAGE (command.find("host: kaspersky.ru") != wstring::npos, L"Error\n");
			hs_proxy.Send("HTTP/1.1 200 OK\x0d\x0a");
			hs_proxy.Send("Content-Length: 10\x0d\x0a");
			hs_proxy.Send("Connection: Keep-Alive\x0d\x0a");
			hs_proxy.Send("Proxy-Connection: Close\x0d\x0a");
			hs_proxy.Send("\x0d\x0a");
			hs_proxy.Send("0123456789");
			hs_proxy.CloseConnection();

			//xml
			hs_proxy.Accept();
			hs_proxy.Receive(command, type, param);
			QC_BOOST_CHECK_MESSAGE (type == HttpServer::CMD_GET, L"Error\n");
			QC_BOOST_CHECK_MESSAGE (command.find("host: kaspersky.ru") != wstring::npos, L"Error\n");
			hs_proxy.Send("HTTP/1.1 200 OK\x0d\x0a");
			hs_proxy.Send("Content-Length: 10\x0d\x0a");
			hs_proxy.Send("Connection: Keep-Alive\x0d\x0a");
			hs_proxy.Send("Proxy-Connection: Close\x0d\x0a");
			hs_proxy.Send("\x0d\x0a");
			hs_proxy.Send("0123456789");
			hs_proxy.CloseConnection();

		}

		WaitForSingleObject (hDoUpdate, INFINITE);
		QC_BOOST_CHECK_MESSAGE ( CORE_INVALID_SIGNATURE == do_upd.GetError(), L"Wrong return code\n" );
	}
	

	{//using wpad
		TestLog testLog;

		HttpServer hs_proxy (3130, "127.0.0.1");
		hs_proxy.Start();
		HttpServer hs_server (8001, _helper::GetSelfIp());
		hs_server.Start();
		HttpServer hs_server2 (8002, _helper::GetSelfIp());
		hs_server2.Start();

		_helper::DeleteDirectory(ROOT);
		KLUPD::createFolder (BASES_ROOT, &testLog);
		KLUPD::createFolder (UPD_ROOT, &testLog);

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
		data.connect_tmo = 1000;

		data.m_sourceList.addSource (UST_UserURL, L"http://kiryukhin-xp_ipv4:8001/", true, L"");
		data.m_sourceList.addSource (UST_UserURL, L"http://kiryukhin-xp_ipv4:8002/", true, L"");
		data.m_sourceList.addSource (UST_UserURL, L"http://kaspersky.ru/", true, L"");

		data.m_proxyAuthorizationCredentials.userName ( L"null");
		data.m_proxyAuthorizationCredentials.password ( L"null");
		data.proxy_url = L"127.0.0.1";
		data.proxy_port = 3127;
		data.use_ie_proxy = true;
		data.m_bypassProxyServerForLocalAddresses = true;
		data.m_httpAuthorizationMethods.push_back (basicAuthorization);
		data.UpdaterListFlags.update_bases = true;

		Callbacks cbacks;
		TestCallbackInterface testCallback (&testLog, &cbacks);
		TestSuiteProxyDetector::ProxyJournal journal ( std::wstring(COLL_ROOT).append(L"\\wpads\\bypass_wpad.dat") );
		testCallback.SetUpdaterConfiguration (data);

		Updater upd (testCallback.m_dp, testCallback, journal, &testLog);
		journal.SetUpdater(&upd);

		HANDLE hDoUpdate;

		doUpdater do_upd(&upd, hDoUpdate);

		string command;
		string param;
		HttpServer::CMD_TYPE type;

		//first source
		{	//klz
			hs_server.Accept();
			hs_server.Receive(command, type, param);
			QC_BOOST_CHECK_MESSAGE (type == HttpServer::CMD_GET, L"Error\n");
			QC_BOOST_CHECK_MESSAGE (command.find("host: kiryukhin-xp_ipv4") != wstring::npos, L"Error\n");
			hs_server.Send("HTTP/1.1 200 OK\x0d\x0a");
			hs_server.Send("Content-Length: 10\x0d\x0a");
			hs_server.Send("Connection: Close\x0d\x0a");
			hs_server.Send("\x0d\x0a");
			hs_server.Send("0123456789");
			hs_server.CloseConnection();

			//xml
			hs_server.Accept();
			hs_server.Receive(command, type, param);
			QC_BOOST_CHECK_MESSAGE (type == HttpServer::CMD_GET, L"Error\n");
			QC_BOOST_CHECK_MESSAGE (command.find("host: kiryukhin-xp_ipv4") != wstring::npos, L"Error\n");
			hs_server.Send("HTTP/1.1 200 OK\x0d\x0a");
			hs_server.Send("Content-Length: 10\x0d\x0a");
			hs_server.Send("Connection: Close\x0d\x0a");
			hs_server.Send("\x0d\x0a");
			hs_server.Send("0123456789");
			hs_server.CloseConnection();

		}

		//second source
		{	//klz
			hs_server2.Accept();
			hs_server2.Receive(command, type, param);
			QC_BOOST_CHECK_MESSAGE (type == HttpServer::CMD_GET, L"Error\n");
			QC_BOOST_CHECK_MESSAGE (command.find("host: kiryukhin-xp_ipv4") != wstring::npos, L"Error\n");
			hs_server2.Send("HTTP/1.1 200 OK\x0d\x0a");
			hs_server2.Send("Content-Length: 10\x0d\x0a");
			hs_server2.Send("Connection: Close\x0d\x0a");
			hs_server2.Send("\x0d\x0a");
			hs_server2.Send("0123456789");
			hs_server2.CloseConnection();

			//xml
			hs_server2.Accept();
			hs_server2.Receive(command, type, param);
			QC_BOOST_CHECK_MESSAGE (type == HttpServer::CMD_GET, L"Error\n");
			QC_BOOST_CHECK_MESSAGE (command.find("host: kiryukhin-xp_ipv4") != wstring::npos, L"Error\n");
			hs_server2.Send("HTTP/1.1 200 OK\x0d\x0a");
			hs_server2.Send("Content-Length: 10\x0d\x0a");
			hs_server2.Send("Connection: Close\x0d\x0a");
			hs_server2.Send("\x0d\x0a");
			hs_server2.Send("0123456789");
			hs_server2.CloseConnection();

		}

		//third source
		{	//klz
			hs_proxy.Accept();
			hs_proxy.Receive(command, type, param);
			QC_BOOST_CHECK_MESSAGE (type == HttpServer::CMD_GET, L"Error\n");
			QC_BOOST_CHECK_MESSAGE (command.find("host: kaspersky.ru") != wstring::npos, L"Error\n");
			hs_proxy.Send("HTTP/1.1 200 OK\x0d\x0a");
			hs_proxy.Send("Content-Length: 10\x0d\x0a");
			hs_proxy.Send("Connection: Keep-Alive\x0d\x0a");
			hs_proxy.Send("Proxy-Connection: Close\x0d\x0a");
			hs_proxy.Send("\x0d\x0a");
			hs_proxy.Send("0123456789");
			hs_proxy.CloseConnection();

			//xml
			hs_proxy.Accept();
			hs_proxy.Receive(command, type, param);
			QC_BOOST_CHECK_MESSAGE (type == HttpServer::CMD_GET, L"Error\n");
			QC_BOOST_CHECK_MESSAGE (command.find("host: kaspersky.ru") != wstring::npos, L"Error\n");
			hs_proxy.Send("HTTP/1.1 200 OK\x0d\x0a");
			hs_proxy.Send("Content-Length: 10\x0d\x0a");
			hs_proxy.Send("Connection: Keep-Alive\x0d\x0a");
			hs_proxy.Send("Proxy-Connection: Close\x0d\x0a");
			hs_proxy.Send("\x0d\x0a");
			hs_proxy.Send("0123456789");
			hs_proxy.CloseConnection();

		}

		WaitForSingleObject (hDoUpdate, INFINITE);
		QC_BOOST_CHECK_MESSAGE ( CORE_INVALID_SIGNATURE == do_upd.GetError(), L"Wrong return code\n" );
	}
	

	{//using ip in URL (our network)
		TestLog testLog;

		HttpServer hs_server (8001, _helper::GetSelfIp());
		hs_server.Start();

		_helper::DeleteDirectory(ROOT);
		KLUPD::createFolder (BASES_ROOT, &testLog);
		KLUPD::createFolder (UPD_ROOT, &testLog);

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
		data.connect_tmo = 1000;

		data.m_sourceList.addSource (UST_UserURL,
			wstring(L"http://").append(_helper::GetSelfIpW()).append(L":8001/").c_str(), true, L"");

		data.m_proxyAuthorizationCredentials.userName ( L"null");
		data.m_proxyAuthorizationCredentials.password ( L"null");
		data.proxy_url = L"127.0.0.1";
		data.proxy_port = 3127;
		data.use_ie_proxy = false;
		data.m_bypassProxyServerForLocalAddresses = true;
		data.m_httpAuthorizationMethods.push_back (basicAuthorization);
		data.UpdaterListFlags.update_bases = true;

		Callbacks cbacks;
		TestCallbackInterface testCallback (&testLog, &cbacks);
		TestJournalInterface journal;
		testCallback.SetUpdaterConfiguration (data);

		Updater upd (testCallback.m_dp, testCallback, journal, &testLog);

		HANDLE hDoUpdate;

		doUpdater do_upd(&upd, hDoUpdate);

		string command;
		string param;
		HttpServer::CMD_TYPE type;

		//first source
		{	//klz
			hs_server.Accept();
			hs_server.Receive(command, type, param);
			QC_BOOST_CHECK_MESSAGE (type == HttpServer::CMD_GET, L"Error\n");
			QC_BOOST_CHECK_MESSAGE (command.find(string("host: ").append(_helper::GetSelfIp())) != string::npos, L"Error\n");
			hs_server.Send("HTTP/1.1 200 OK\x0d\x0a");
			hs_server.Send("Content-Length: 10\x0d\x0a");
			hs_server.Send("Connection: Close\x0d\x0a");
			hs_server.Send("\x0d\x0a");
			hs_server.Send("0123456789");
			hs_server.CloseConnection();

			//xml
			hs_server.Accept();
			hs_server.Receive(command, type, param);
			QC_BOOST_CHECK_MESSAGE (type == HttpServer::CMD_GET, L"Error\n");
			QC_BOOST_CHECK_MESSAGE (command.find(string("host: ").append(_helper::GetSelfIp())) != string::npos, L"Error\n");
			hs_server.Send("HTTP/1.1 200 OK\x0d\x0a");
			hs_server.Send("Content-Length: 10\x0d\x0a");
			hs_server.Send("Connection: Close\x0d\x0a");
			hs_server.Send("\x0d\x0a");
			hs_server.Send("0123456789");
			hs_server.CloseConnection();

		}

		WaitForSingleObject (hDoUpdate, INFINITE);
		QC_BOOST_CHECK_MESSAGE ( CORE_INVALID_SIGNATURE == do_upd.GetError(), L"Wrong return code\n" );
	}
	
	{//using ip in URL (external network)
		TestLog testLog;

		HttpServer hs_proxy (3131, "127.0.0.1");
		hs_proxy.Start();

		_helper::DeleteDirectory(ROOT);
		KLUPD::createFolder (BASES_ROOT, &testLog);
		KLUPD::createFolder (UPD_ROOT, &testLog);

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
		data.connect_tmo = 1000;

		data.m_sourceList.addSource (UST_UserURL,
			wstring(L"http://").append(_helper::GetKasperskyRuIpW()).c_str(), true, L"");

		data.m_proxyAuthorizationCredentials.userName ( L"null");
		data.m_proxyAuthorizationCredentials.password ( L"null");
		data.proxy_url = L"127.0.0.1";
		data.proxy_port = 3131;
		data.use_ie_proxy = false;
		data.m_bypassProxyServerForLocalAddresses = true;
		data.m_httpAuthorizationMethods.push_back (basicAuthorization);
		data.UpdaterListFlags.update_bases = true;

		Callbacks cbacks;
		TestCallbackInterface testCallback (&testLog, &cbacks);
		TestJournalInterface journal;
		testCallback.SetUpdaterConfiguration (data);

		Updater upd (testCallback.m_dp, testCallback, journal, &testLog);

		HANDLE hDoUpdate;

		doUpdater do_upd(&upd, hDoUpdate);

		string command;
		string param;
		HttpServer::CMD_TYPE type;

		//first source
		{	//klz
			hs_proxy.Accept();
			hs_proxy.Receive(command, type, param);
			QC_BOOST_CHECK_MESSAGE (type == HttpServer::CMD_GET, L"Error\n");
			QC_BOOST_CHECK_MESSAGE (command.find(string("host: ").append(_helper::GetKasperskyRuIp())) != string::npos, L"Error\n");
			hs_proxy.Send("HTTP/1.1 200 OK\x0d\x0a");
			hs_proxy.Send("Content-Length: 10\x0d\x0a");
			hs_proxy.Send("Connection: Close\x0d\x0a");
			hs_proxy.Send("\x0d\x0a");
			hs_proxy.Send("0123456789");
			hs_proxy.CloseConnection();

			//xml
			hs_proxy.Accept();
			hs_proxy.Receive(command, type, param);
			QC_BOOST_CHECK_MESSAGE (type == HttpServer::CMD_GET, L"Error\n");
			QC_BOOST_CHECK_MESSAGE (command.find(string("host: ").append(_helper::GetKasperskyRuIp())) != string::npos, L"Error\n");
			hs_proxy.Send("HTTP/1.1 200 OK\x0d\x0a");
			hs_proxy.Send("Content-Length: 10\x0d\x0a");
			hs_proxy.Send("Connection: Close\x0d\x0a");
			hs_proxy.Send("\x0d\x0a");
			hs_proxy.Send("0123456789");
			hs_proxy.CloseConnection();

		}

		WaitForSingleObject (hDoUpdate, INFINITE);
		QC_BOOST_CHECK_MESSAGE ( CORE_INVALID_SIGNATURE == do_upd.GetError(), L"Wrong return code\n" );
	}

	{// check settings collision - bypass is on, the source is external (w/o proxy) 
		TestLog testLog;

		HttpServer hs_proxy (3127, "127.0.0.1");
		hs_proxy.Start();

		_helper::DeleteDirectory(ROOT);
		KLUPD::createFolder (BASES_ROOT, &testLog);
		KLUPD::createFolder (UPD_ROOT, &testLog);

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
		data.connect_tmo = 1000;

		data.m_sourceList.addSource (UST_UserURL, L"http://kaspersky.ru/", false, L"");

		data.m_proxyAuthorizationCredentials.userName ( L"");
		data.m_proxyAuthorizationCredentials.password ( L"");
		data.proxy_url = L"127.0.0.1";
		data.proxy_port = 3127;
		data.use_ie_proxy = false;
		data.m_bypassProxyServerForLocalAddresses = true;
		//data.m_httpAuthorizationMethods.push_back (basicAuthorization);
		data.UpdaterListFlags.update_bases = true;

		Callbacks cbacks;
		TestCallbackInterface testCallback (&testLog, &cbacks);
		TestJournalInterface journal;
		testCallback.SetUpdaterConfiguration (data);

		Updater upd (testCallback.m_dp, testCallback, journal, &testLog);

		HANDLE hDoUpdate;

		doUpdater do_upd(&upd, hDoUpdate);

		// проверить, что запрос идет на внешний адрес, мы не можем,
		// так что ждем, что к нашему прокси нет запросов
		QC_BOOST_CHECK_MESSAGE ( WSA_WAIT_TIMEOUT == hs_proxy.Accept(15000), L"Incorrect connection\n" );

		WaitForSingleObject (hDoUpdate, INFINITE);
		//не нужно нам проверять, что там с kaspersky.ru скачалось - не скачалось
		//QC_BOOST_CHECK_MESSAGE ( CORE_DOWNLOAD_ERROR == do_upd.GetError(), L"Wrong return code\n" );
	}
	
}


void TestSuiteNet::TestWpadProxyDetectResult ()
{
	using namespace KLUPD;
	std::wcout<<L"TestWpadProxyDetectResult\n";
	Logger::Case _case (m_logger, m_testp, m_step, L"TestWpadProxyDetectResult");

	//проверка обработки результата, возвращенного wpad (особенно, если возвращается DIRECT
	//и особенно для ftp-сервера, т.к. в этом случае различается используемый протокол)

	{//http - direct connection
		TestLog testLog;

		HttpServer hs_server (8001, "127.0.0.1");
		hs_server.Start();

		_helper::DeleteDirectory(ROOT);
		KLUPD::createFolder (BASES_ROOT, &testLog);
		KLUPD::createFolder (UPD_ROOT, &testLog);

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
		data.connect_tmo = 1000;

		data.m_sourceList.addSource (UST_UserURL, L"http://127.0.0.1:8001/", true, L"");

		data.m_proxyAuthorizationCredentials.userName ( L"null");
		data.m_proxyAuthorizationCredentials.password ( L"null");
		data.proxy_url = L"127.0.0.1";
		data.proxy_port = 3127;
		data.use_ie_proxy = true;
		data.m_httpAuthorizationMethods.push_back (basicAuthorization);
		data.UpdaterListFlags.update_bases = true;

		Callbacks cbacks;
		TestCallbackInterface testCallback (&testLog, &cbacks);
		TestSuiteProxyDetector::ProxyJournal journal ( std::wstring(COLL_ROOT).append(L"\\wpads\\direct_wpad.dat") );
		testCallback.SetUpdaterConfiguration (data);

		Updater upd (testCallback.m_dp, testCallback, journal, &testLog);
		journal.SetUpdater(&upd);

		HANDLE hDoUpdate;

		doUpdater do_upd(&upd, hDoUpdate);

		string command;
		string param;
		HttpServer::CMD_TYPE type;

		{	//klz
			hs_server.Accept();
			hs_server.Receive(command, type, param);
			QC_BOOST_CHECK_MESSAGE (type == HttpServer::CMD_GET, L"Error\n");
			QC_BOOST_CHECK_MESSAGE (command.find("host: 127.0.0.1") != wstring::npos, L"Error\n");
			hs_server.Send("HTTP/1.1 200 OK\x0d\x0a");
			hs_server.Send("Content-Length: 10\x0d\x0a");
			hs_server.Send("Connection: Close\x0d\x0a");
			hs_server.Send("\x0d\x0a");
			hs_server.Send("0123456789");
			hs_server.CloseConnection();

			//xml
			hs_server.Accept();
			hs_server.Receive(command, type, param);
			QC_BOOST_CHECK_MESSAGE (type == HttpServer::CMD_GET, L"Error\n");
			QC_BOOST_CHECK_MESSAGE (command.find("host: 127.0.0.1") != wstring::npos, L"Error\n");
			hs_server.Send("HTTP/1.1 200 OK\x0d\x0a");
			hs_server.Send("Content-Length: 10\x0d\x0a");
			hs_server.Send("Connection: Close\x0d\x0a");
			hs_server.Send("\x0d\x0a");
			hs_server.Send("0123456789");
			hs_server.CloseConnection();

		}

		WaitForSingleObject (hDoUpdate, INFINITE);
		QC_BOOST_CHECK_MESSAGE ( CORE_INVALID_SIGNATURE == do_upd.GetError(), L"Wrong return code\n" );
	}
	

	{//ftp - proxy connection
		TestLog testLog;

		HttpServer hs_proxy (3128, "127.0.0.1");
		hs_proxy.Start();

		_helper::DeleteDirectory(ROOT);
		KLUPD::createFolder (BASES_ROOT, &testLog);
		KLUPD::createFolder (UPD_ROOT, &testLog);

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
		data.connect_tmo = 1000;

		data.m_sourceList.addSource (UST_UserURL, L"ftp://127.0.0.1:8001/", true, L"");

		data.m_proxyAuthorizationCredentials.userName ( L"null");
		data.m_proxyAuthorizationCredentials.password ( L"null");
		data.proxy_url = L"127.0.0.1";
		data.proxy_port = 3127;
		data.use_ie_proxy = true;
		data.m_httpAuthorizationMethods.push_back (basicAuthorization);
		data.UpdaterListFlags.update_bases = true;

		Callbacks cbacks;
		TestCallbackInterface testCallback (&testLog, &cbacks);
		TestSuiteProxyDetector::ProxyJournal journal ( std::wstring(COLL_ROOT).append(L"\\wpads\\proxy_wpad.dat") );
		testCallback.SetUpdaterConfiguration (data);

		Updater upd (testCallback.m_dp, testCallback, journal, &testLog);
		journal.SetUpdater(&upd);

		HANDLE hDoUpdate;

		doUpdater do_upd(&upd, hDoUpdate);

		string command;
		string param;
		HttpServer::CMD_TYPE type;

		{	//klz
			hs_proxy.Accept();
			hs_proxy.Receive(command, type, param);
			QC_BOOST_CHECK_MESSAGE (type == HttpServer::CMD_GET, L"Error\n");
			QC_BOOST_CHECK_MESSAGE (command.find("get ftp://") != wstring::npos, L"Error\n");
			hs_proxy.Send("HTTP/1.1 200 OK\x0d\x0a");
			hs_proxy.Send("Content-Length: 10\x0d\x0a");
			hs_proxy.Send("Connection: Close\x0d\x0a");
			hs_proxy.Send("\x0d\x0a");
			hs_proxy.Send("0123456789");
			hs_proxy.CloseConnection();

			//xml
			hs_proxy.Accept();
			hs_proxy.Receive(command, type, param);
			QC_BOOST_CHECK_MESSAGE (type == HttpServer::CMD_GET, L"Error\n");
			QC_BOOST_CHECK_MESSAGE (command.find("get ftp://") != wstring::npos, L"Error\n");
			hs_proxy.Send("HTTP/1.1 200 OK\x0d\x0a");
			hs_proxy.Send("Content-Length: 10\x0d\x0a");
			hs_proxy.Send("Connection: Close\x0d\x0a");
			hs_proxy.Send("\x0d\x0a");
			hs_proxy.Send("0123456789");
			hs_proxy.CloseConnection();

		}

		WaitForSingleObject (hDoUpdate, INFINITE);
		QC_BOOST_CHECK_MESSAGE ( CORE_INVALID_SIGNATURE == do_upd.GetError(), L"Wrong return code\n" );
	}

	
	{//ftp - direct connection
		TestLog testLog;

		FtpServer fs (1212, "127.0.0.1");
		fs.Start();

		_helper::DeleteDirectory(ROOT);
		KLUPD::createFolder (BASES_ROOT, &testLog);
		KLUPD::createFolder (UPD_ROOT, &testLog);

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
		data.connect_tmo = 1000;

		data.m_sourceList.addSource (UST_UserURL, L"ftp://127.0.0.1:1212/", true, L"");

		data.m_proxyAuthorizationCredentials.userName ( L"null");
		data.m_proxyAuthorizationCredentials.password ( L"null");
		data.proxy_url = L"127.0.0.1";
		data.proxy_port = 3127;
		data.use_ie_proxy = true;
		data.m_httpAuthorizationMethods.push_back (basicAuthorization);
		data.UpdaterListFlags.update_bases = true;

		Callbacks cbacks;
		TestCallbackInterface testCallback (&testLog, &cbacks);
		TestSuiteProxyDetector::ProxyJournal journal ( std::wstring(COLL_ROOT).append(L"\\wpads\\direct_wpad.dat") );
		testCallback.SetUpdaterConfiguration (data);

		Updater upd (testCallback.m_dp, testCallback, journal, &testLog);
		journal.SetUpdater(&upd);

		HANDLE hDoUpdate;

		doUpdater do_upd(&upd, hDoUpdate);

		{	//klz
			fs.Accept();
			fs.CloseControlConnection();
			
			//xml
			fs.Accept();
			fs.CloseControlConnection();
		}

		WaitForSingleObject (hDoUpdate, INFINITE);
		QC_BOOST_CHECK_MESSAGE ( CORE_REMOTE_HOST_CLOSED_CONNECTION == do_upd.GetError(), L"Wrong return code\n" );
	}
}


void TestSuiteNet::TestHttpIpV6()
{
	std::wcout<<L"TestHttpIpV6\n";
	Logger::Case _case (m_logger, m_testp, m_step, L"TestHttpIpV6");

	{//dns-name

		_helper::DeleteDirectory(TEST_FOLDER);
		_helper::MakeDirectory(TEST_FOLDER);

		TestLog testLog;
		Callbacks cbacks;
		TestCallbackInterface testCallback (&testLog, &cbacks);
		TestJournalInterface testJournal;
		Signature6Checker s6c (&testLog);

		FileInfo _fi (L"base004.gif", L"/");
		_fi.m_localPath = L"h:\\test\\";

		UpdaterConfiguration data(1, 1, L"1");
		data.m_sourceList.addSource (UST_UserURL, L"http://[dead::c001]", false, L"");
		testCallback.SetUpdaterConfiguration (data);

		Updater upd (testCallback.m_dp, testCallback, testJournal, &testLog);
		upd.constructSourceList(testCallback.updaterConfiguration().m_sourceList, upd.m_sourceList);

		HANDLE hDoDownload;
		doEntryDownloader do_dld(&upd, hDoDownload, _fi, _fi.m_localPath, &s6c);

		WaitForSingleObject (hDoDownload, INFINITE);

		QC_BOOST_CHECK_MESSAGE ( CORE_NO_ERROR == do_dld.GetError(), L"Wrong return code\n" );
		QC_BOOST_CHECK_MESSAGE ( _helper::AreFilesEqual (ETALON_NET_FILE, std::wstring(TEST_FOLDER).append(L"\\base004.gif")),
			L"Incorrect file\n" );
	}

	{//dns-name

		_helper::DeleteDirectory(TEST_FOLDER);
		_helper::MakeDirectory(TEST_FOLDER);

		TestLog testLog;
		Callbacks cbacks;
		TestCallbackInterface testCallback (&testLog, &cbacks);
		TestJournalInterface testJournal;
		Signature6Checker s6c (&testLog);

		FileInfo _fi (L"base004.gif", L"/");
		_fi.m_localPath = L"h:\\test\\";

		UpdaterConfiguration data(1, 1, L"1");
		data.m_sourceList.addSource (UST_UserURL, L"http://f508.klipv6.ru", false, L"");
		testCallback.SetUpdaterConfiguration (data);

		Updater upd (testCallback.m_dp, testCallback, testJournal, &testLog);
		upd.constructSourceList(testCallback.updaterConfiguration().m_sourceList, upd.m_sourceList);

		HANDLE hDoDownload;
		doEntryDownloader do_dld(&upd, hDoDownload, _fi, _fi.m_localPath, &s6c);

		WaitForSingleObject (hDoDownload, INFINITE);

		QC_BOOST_CHECK_MESSAGE ( CORE_NO_ERROR == do_dld.GetError(), L"Wrong return code\n" );
		QC_BOOST_CHECK_MESSAGE ( _helper::AreFilesEqual (ETALON_NET_FILE, std::wstring(TEST_FOLDER).append(L"\\base004.gif")),
			L"Incorrect file\n" );
	}
}

void TestSuiteNet::TestFtpIpV6()
{
	{//ip
		std::wcout<<L"TestFtpIpV6\n";
		Logger::Case _case (m_logger, m_testp, m_step, L"TestFtpIpV6");

		_helper::DeleteDirectory(TEST_FOLDER);
		_helper::MakeDirectory(TEST_FOLDER);

		TestLog testLog;
		Callbacks cbacks;
		TestCallbackInterface testCallback (&testLog, &cbacks);
		TestJournalInterface testJournal;
		Signature6Checker s6c (&testLog);

		FileInfo _fi (L"base004.avc", L"/files/test");

		UpdaterConfiguration data(1, 1, L"1");
		data.passive_ftp = true;
		testCallback.SetUpdaterConfiguration (data);

		Updater upd (testCallback.m_dp, testCallback, testJournal, &testLog);
		QC_BOOST_CHECK_MESSAGE ( CORE_NO_ERROR == upd.DoGetRemoteFile_SelfNet (_fi.m_filename, L"h:\\test\\",
			_fi.m_relativeURLPath, L"ftp://user01:01@[dead::c123]"),
			L"Wrong return code\n" );
		QC_BOOST_CHECK_MESSAGE ( _helper::AreFilesEqual (ETALON_NET_FILE, std::wstring(TEST_FOLDER).append(L"\\base004.avc")),
			L"Incorrect file\n" );
	}

	{//dns-name
		std::wcout<<L"TestFtpIpV6\n";
		Logger::Case _case (m_logger, m_testp, m_step, L"TestFtpIpV6");

		_helper::DeleteDirectory(TEST_FOLDER);
		_helper::MakeDirectory(TEST_FOLDER);

		TestLog testLog;
		Callbacks cbacks;
		TestCallbackInterface testCallback (&testLog, &cbacks);
		TestJournalInterface testJournal;
		Signature6Checker s6c (&testLog);

		FileInfo _fi (L"base004.avc", L"/files/test");

		UpdaterConfiguration data(1, 1, L"1");
		data.passive_ftp = false;
		testCallback.SetUpdaterConfiguration (data);

		Updater upd (testCallback.m_dp, testCallback, testJournal, &testLog);
		QC_BOOST_CHECK_MESSAGE ( CORE_NO_ERROR == upd.DoGetRemoteFile_SelfNet (_fi.m_filename, L"h:\\test\\",
			_fi.m_relativeURLPath, L"ftp://user01:01@ftp.klipv6.ru"),
			L"Wrong return code\n" );
		QC_BOOST_CHECK_MESSAGE ( _helper::AreFilesEqual (ETALON_NET_FILE, std::wstring(TEST_FOLDER).append(L"\\base004.avc")),
			L"Incorrect file\n" );
	}
}

void TestSuiteNet::TestShareIpV6()
{
	{//dns-name
		std::wcout<<L"TestShareIpV6\n";
		Logger::Case _case (m_logger, m_testp, m_step, L"TestShareIpV6");

		_helper::DeleteDirectory(TEST_FOLDER);
		_helper::MakeDirectory(TEST_FOLDER);

		TestLog testLog;
		Callbacks cbacks;
		TestCallbackInterface testCallback (&testLog, &cbacks);
		TestJournalInterface testJournal;
		Signature6Checker s6c (&testLog);

		FileInfo _fi (L"base004.gif", L"/");

		UpdaterConfiguration data(1, 1, L"1");
		data.passive_ftp = false;
		testCallback.SetUpdaterConfiguration (data);

		Updater upd (testCallback.m_dp, testCallback, testJournal, &testLog);
		QC_BOOST_CHECK_MESSAGE ( CORE_NO_ERROR == upd.DoGetRemoteFile_SelfNet (_fi.m_filename, L"h:\\test\\",
			_fi.m_relativeURLPath, L"\\\\f508.klipv6.ru\\wwwroot\\"),
			L"Wrong return code\n" );
		QC_BOOST_CHECK_MESSAGE ( _helper::AreFilesEqual (ETALON_NET_FILE, std::wstring(TEST_FOLDER).append(L"\\base004.gif")),
			L"Incorrect file\n" );
	}
}


void TestSuiteNet::TestBypassProxyForLocalAddresses_IpV6 ()
{
	using namespace KLUPD;
	std::wcout<<L"TestBypassProxyForLocalAddresses_IpV6\n";
	Logger::Case _case (m_logger, m_testp, m_step, L"TestBypassProxyForLocalAddresses_IpV6");

	{//without wpad
		TestLog testLog;

		HttpServer hs_proxy (3127, "127.0.0.1");
		hs_proxy.Start();
		HttpServer hs_server (8001, _helper::GetSelfIp_IpV6(), IP_V6);
		hs_server.Start();

		_helper::DeleteDirectory(ROOT);
		KLUPD::createFolder (BASES_ROOT, &testLog);
		KLUPD::createFolder (UPD_ROOT, &testLog);

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
		data.connect_tmo = 1000;

		//data.m_sourceList.addSource (UST_UserURL, L"http://kiryukhin-xp_ipv6:8001/", true, L"");
		data.m_sourceList.addSource (UST_UserURL, L"http://[dead::c596]:8001/", true, L"");
		data.m_sourceList.addSource (UST_UserURL, L"http://kaspersky.ru/", true, L"");

		data.m_proxyAuthorizationCredentials.userName ( L"null");
		data.m_proxyAuthorizationCredentials.password ( L"null");
		data.proxy_url = L"127.0.0.1";
		data.proxy_port = 3127;
		data.use_ie_proxy = false;
		data.m_bypassProxyServerForLocalAddresses = true;
		data.m_httpAuthorizationMethods.push_back (basicAuthorization);
		data.UpdaterListFlags.update_bases = true;

		Callbacks cbacks;
		TestCallbackInterface testCallback (&testLog, &cbacks);
		TestJournalInterface journal;
		testCallback.SetUpdaterConfiguration (data);

		Updater upd (testCallback.m_dp, testCallback, journal, &testLog);

		HANDLE hDoUpdate;

		doUpdater do_upd(&upd, hDoUpdate);

		string command;
		string param;
		HttpServer::CMD_TYPE type;

		//first source
		{	//klz
			hs_server.Accept();
			hs_server.Receive(command, type, param);
			QC_BOOST_CHECK_MESSAGE (type == HttpServer::CMD_GET, L"Error\n");
			QC_BOOST_CHECK_MESSAGE (command.find("host: kiryukhin-xp_ipv6") != wstring::npos, L"Error\n");
			hs_server.Send("HTTP/1.1 200 OK\x0d\x0a");
			hs_server.Send("Content-Length: 10\x0d\x0a");
			hs_server.Send("Connection: Close\x0d\x0a");
			hs_server.Send("\x0d\x0a");
			hs_server.Send("0123456789");
			hs_server.CloseConnection();

			//xml
			hs_server.Accept();
			hs_server.Receive(command, type, param);
			QC_BOOST_CHECK_MESSAGE (type == HttpServer::CMD_GET, L"Error\n");
			QC_BOOST_CHECK_MESSAGE (command.find("host: kiryukhin-xp_ipv6") != wstring::npos, L"Error\n");
			hs_server.Send("HTTP/1.1 200 OK\x0d\x0a");
			hs_server.Send("Content-Length: 10\x0d\x0a");
			hs_server.Send("Connection: Close\x0d\x0a");
			hs_server.Send("\x0d\x0a");
			hs_server.Send("0123456789");
			hs_server.CloseConnection();

		}

		//second source
		{	//klz
			hs_proxy.Accept();
			hs_proxy.Receive(command, type, param);
			QC_BOOST_CHECK_MESSAGE (type == HttpServer::CMD_GET, L"Error\n");
			QC_BOOST_CHECK_MESSAGE (command.find("host: kaspersky.ru") != wstring::npos, L"Error\n");
			hs_proxy.Send("HTTP/1.1 200 OK\x0d\x0a");
			hs_proxy.Send("Content-Length: 10\x0d\x0a");
			hs_proxy.Send("Connection: Keep-Alive\x0d\x0a");
			hs_proxy.Send("Proxy-Connection: Close\x0d\x0a");
			hs_proxy.Send("\x0d\x0a");
			hs_proxy.Send("0123456789");
			hs_proxy.CloseConnection();

			//xml
			hs_proxy.Accept();
			hs_proxy.Receive(command, type, param);
			QC_BOOST_CHECK_MESSAGE (type == HttpServer::CMD_GET, L"Error\n");
			QC_BOOST_CHECK_MESSAGE (command.find("host: kaspersky.ru") != wstring::npos, L"Error\n");
			hs_proxy.Send("HTTP/1.1 200 OK\x0d\x0a");
			hs_proxy.Send("Content-Length: 10\x0d\x0a");
			hs_proxy.Send("Connection: Keep-Alive\x0d\x0a");
			hs_proxy.Send("Proxy-Connection: Close\x0d\x0a");
			hs_proxy.Send("\x0d\x0a");
			hs_proxy.Send("0123456789");
			hs_proxy.CloseConnection();

		}

		WaitForSingleObject (hDoUpdate, INFINITE);
		QC_BOOST_CHECK_MESSAGE ( CORE_INVALID_SIGNATURE == do_upd.GetError(), L"Wrong return code\n" );
	}

/*
	{//using wpad
		TestLog testLog;

		HttpServer hs_proxy (3130, "[::1]", IP_V6);
		hs_proxy.Start();
		HttpServer hs_server (8001, _helper::GetSelfIp_IpV6(), IP_V6);
		hs_server.Start();
		HttpServer hs_server2 (8002, _helper::GetSelfIp_IpV6(), IP_V6);
		hs_server2.Start();

		_helper::DeleteDirectory(ROOT);
		KLUPD::createFolder (BASES_ROOT, &testLog);
		KLUPD::createFolder (UPD_ROOT, &testLog);

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
		data.connect_tmo = 1000;

		data.m_sourceList.addSource (UST_UserURL, L"http://kiryukhin-xp_ipv6:8001/", true, L"");
		data.m_sourceList.addSource (UST_UserURL, L"http://kiryukhin-xp_ipv6:8002/", true, L"");
		data.m_sourceList.addSource (UST_UserURL, L"http://kaspersky.ru/", true, L"");

		data.m_proxyAuthorizationCredentials.userName ( L"null");
		data.m_proxyAuthorizationCredentials.password ( L"null");
		data.proxy_url = L"[::1]";
		data.proxy_port = 3127;
		data.use_ie_proxy = true;
		data.m_bypassProxyServerForLocalAddresses = true;
		data.m_httpAuthorizationMethods.push_back (basicAuthorization);
		data.UpdaterListFlags.update_bases = true;

		Callbacks cbacks;
		TestCallbackInterface testCallback (&testLog, &cbacks);
		TestSuiteProxyDetector::ProxyJournal journal ( std::wstring(COLL_ROOT).append(L"\\wpads\\bypass_wpad.dat") );
		testCallback.SetUpdaterConfiguration (data);

		Updater upd (testCallback.m_dp, testCallback, journal, &testLog);
		journal.SetUpdater(&upd);

		HANDLE hDoUpdate;

		doUpdater do_upd(&upd, hDoUpdate);

		string command;
		string param;
		HttpServer::CMD_TYPE type;

		//first source
		{	//klz
			hs_server.Accept();
			hs_server.Receive(command, type, param);
			QC_BOOST_CHECK_MESSAGE (type == HttpServer::CMD_GET, L"Error\n");
			QC_BOOST_CHECK_MESSAGE (command.find("host: kiryukhin-xp_ipv4") != wstring::npos, L"Error\n");
			hs_server.Send("HTTP/1.1 200 OK\x0d\x0a");
			hs_server.Send("Content-Length: 10\x0d\x0a");
			hs_server.Send("Connection: Close\x0d\x0a");
			hs_server.Send("\x0d\x0a");
			hs_server.Send("0123456789");
			hs_server.CloseConnection();

			//xml
			hs_server.Accept();
			hs_server.Receive(command, type, param);
			QC_BOOST_CHECK_MESSAGE (type == HttpServer::CMD_GET, L"Error\n");
			QC_BOOST_CHECK_MESSAGE (command.find("host: kiryukhin-xp_ipv4") != wstring::npos, L"Error\n");
			hs_server.Send("HTTP/1.1 200 OK\x0d\x0a");
			hs_server.Send("Content-Length: 10\x0d\x0a");
			hs_server.Send("Connection: Close\x0d\x0a");
			hs_server.Send("\x0d\x0a");
			hs_server.Send("0123456789");
			hs_server.CloseConnection();

		}

		//second source
		{	//klz
			hs_server2.Accept();
			hs_server2.Receive(command, type, param);
			QC_BOOST_CHECK_MESSAGE (type == HttpServer::CMD_GET, L"Error\n");
			QC_BOOST_CHECK_MESSAGE (command.find("host: kiryukhin-xp_ipv4") != wstring::npos, L"Error\n");
			hs_server2.Send("HTTP/1.1 200 OK\x0d\x0a");
			hs_server2.Send("Content-Length: 10\x0d\x0a");
			hs_server2.Send("Connection: Close\x0d\x0a");
			hs_server2.Send("\x0d\x0a");
			hs_server2.Send("0123456789");
			hs_server2.CloseConnection();

			//xml
			hs_server2.Accept();
			hs_server2.Receive(command, type, param);
			QC_BOOST_CHECK_MESSAGE (type == HttpServer::CMD_GET, L"Error\n");
			QC_BOOST_CHECK_MESSAGE (command.find("host: kiryukhin-xp_ipv4") != wstring::npos, L"Error\n");
			hs_server2.Send("HTTP/1.1 200 OK\x0d\x0a");
			hs_server2.Send("Content-Length: 10\x0d\x0a");
			hs_server2.Send("Connection: Close\x0d\x0a");
			hs_server2.Send("\x0d\x0a");
			hs_server2.Send("0123456789");
			hs_server2.CloseConnection();

		}

		//third source
		{	//klz
			hs_proxy.Accept();
			hs_proxy.Receive(command, type, param);
			QC_BOOST_CHECK_MESSAGE (type == HttpServer::CMD_GET, L"Error\n");
			QC_BOOST_CHECK_MESSAGE (command.find("host: kaspersky.ru") != wstring::npos, L"Error\n");
			hs_proxy.Send("HTTP/1.1 200 OK\x0d\x0a");
			hs_proxy.Send("Content-Length: 10\x0d\x0a");
			hs_proxy.Send("Connection: Keep-Alive\x0d\x0a");
			hs_proxy.Send("Proxy-Connection: Close\x0d\x0a");
			hs_proxy.Send("\x0d\x0a");
			hs_proxy.Send("0123456789");
			hs_proxy.CloseConnection();

			//xml
			hs_proxy.Accept();
			hs_proxy.Receive(command, type, param);
			QC_BOOST_CHECK_MESSAGE (type == HttpServer::CMD_GET, L"Error\n");
			QC_BOOST_CHECK_MESSAGE (command.find("host: kaspersky.ru") != wstring::npos, L"Error\n");
			hs_proxy.Send("HTTP/1.1 200 OK\x0d\x0a");
			hs_proxy.Send("Content-Length: 10\x0d\x0a");
			hs_proxy.Send("Connection: Keep-Alive\x0d\x0a");
			hs_proxy.Send("Proxy-Connection: Close\x0d\x0a");
			hs_proxy.Send("\x0d\x0a");
			hs_proxy.Send("0123456789");
			hs_proxy.CloseConnection();

		}

		WaitForSingleObject (hDoUpdate, INFINITE);
		QC_BOOST_CHECK_MESSAGE ( CORE_INVALID_SIGNATURE == do_upd.GetError(), L"Wrong return code\n" );
	}
/*
/*
	{//using ip in URL (our network)
		TestLog testLog;

		HttpServer hs_server (8001, _helper::GetSelfIp());
		hs_server.Start();

		_helper::DeleteDirectory(ROOT);
		KLUPD::createFolder (BASES_ROOT, &testLog);
		KLUPD::createFolder (UPD_ROOT, &testLog);

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
		data.connect_tmo = 1000;

		data.m_sourceList.addSource (UST_UserURL,
			wstring(L"http://").append(_helper::GetSelfIpW()).append(L":8001/").c_str(), true, L"");

		data.m_proxyAuthorizationCredentials.userName ( L"null");
		data.m_proxyAuthorizationCredentials.password ( L"null");
		data.proxy_url = L"127.0.0.1";
		data.proxy_port = 3127;
		data.use_ie_proxy = false;
		data.m_bypassProxyServerForLocalAddresses = true;
		data.m_httpAuthorizationMethods.push_back (basicAuthorization);
		data.UpdaterListFlags.update_bases = true;

		Callbacks cbacks;
		TestCallbackInterface testCallback (&testLog, &cbacks);
		TestJournalInterface journal;
		testCallback.SetUpdaterConfiguration (data);

		Updater upd (testCallback.m_dp, testCallback, journal, &testLog);

		HANDLE hDoUpdate;

		doUpdater do_upd(&upd, hDoUpdate);

		string command;
		string param;
		HttpServer::CMD_TYPE type;

		//first source
		{	//klz
			hs_server.Accept();
			hs_server.Receive(command, type, param);
			QC_BOOST_CHECK_MESSAGE (type == HttpServer::CMD_GET, L"Error\n");
			QC_BOOST_CHECK_MESSAGE (command.find(string("host: ").append(_helper::GetSelfIp())) != string::npos, L"Error\n");
			hs_server.Send("HTTP/1.1 200 OK\x0d\x0a");
			hs_server.Send("Content-Length: 10\x0d\x0a");
			hs_server.Send("Connection: Close\x0d\x0a");
			hs_server.Send("\x0d\x0a");
			hs_server.Send("0123456789");
			hs_server.CloseConnection();

			//xml
			hs_server.Accept();
			hs_server.Receive(command, type, param);
			QC_BOOST_CHECK_MESSAGE (type == HttpServer::CMD_GET, L"Error\n");
			QC_BOOST_CHECK_MESSAGE (command.find(string("host: ").append(_helper::GetSelfIp())) != string::npos, L"Error\n");
			hs_server.Send("HTTP/1.1 200 OK\x0d\x0a");
			hs_server.Send("Content-Length: 10\x0d\x0a");
			hs_server.Send("Connection: Close\x0d\x0a");
			hs_server.Send("\x0d\x0a");
			hs_server.Send("0123456789");
			hs_server.CloseConnection();

		}

		WaitForSingleObject (hDoUpdate, INFINITE);
		QC_BOOST_CHECK_MESSAGE ( CORE_INVALID_SIGNATURE == do_upd.GetError(), L"Wrong return code\n" );
	}

	{//using ip in URL (external network)
		TestLog testLog;

		HttpServer hs_proxy (3131, "127.0.0.1");
		hs_proxy.Start();

		_helper::DeleteDirectory(ROOT);
		KLUPD::createFolder (BASES_ROOT, &testLog);
		KLUPD::createFolder (UPD_ROOT, &testLog);

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
		data.connect_tmo = 1000;

		data.m_sourceList.addSource (UST_UserURL,
			wstring(L"http://").append(_helper::GetKasperskyRuIpW()).c_str(), true, L"");

		data.m_proxyAuthorizationCredentials.userName ( L"null");
		data.m_proxyAuthorizationCredentials.password ( L"null");
		data.proxy_url = L"127.0.0.1";
		data.proxy_port = 3131;
		data.use_ie_proxy = false;
		data.m_bypassProxyServerForLocalAddresses = true;
		data.m_httpAuthorizationMethods.push_back (basicAuthorization);
		data.UpdaterListFlags.update_bases = true;

		Callbacks cbacks;
		TestCallbackInterface testCallback (&testLog, &cbacks);
		TestJournalInterface journal;
		testCallback.SetUpdaterConfiguration (data);

		Updater upd (testCallback.m_dp, testCallback, journal, &testLog);

		HANDLE hDoUpdate;

		doUpdater do_upd(&upd, hDoUpdate);

		string command;
		string param;
		HttpServer::CMD_TYPE type;

		//first source
		{	//klz
			hs_proxy.Accept();
			hs_proxy.Receive(command, type, param);
			QC_BOOST_CHECK_MESSAGE (type == HttpServer::CMD_GET, L"Error\n");
			QC_BOOST_CHECK_MESSAGE (command.find(string("host: ").append(_helper::GetKasperskyRuIp())) != string::npos, L"Error\n");
			hs_proxy.Send("HTTP/1.1 200 OK\x0d\x0a");
			hs_proxy.Send("Content-Length: 10\x0d\x0a");
			hs_proxy.Send("Connection: Close\x0d\x0a");
			hs_proxy.Send("\x0d\x0a");
			hs_proxy.Send("0123456789");
			hs_proxy.CloseConnection();

			//xml
			hs_proxy.Accept();
			hs_proxy.Receive(command, type, param);
			QC_BOOST_CHECK_MESSAGE (type == HttpServer::CMD_GET, L"Error\n");
			QC_BOOST_CHECK_MESSAGE (command.find(string("host: ").append(_helper::GetKasperskyRuIp())) != string::npos, L"Error\n");
			hs_proxy.Send("HTTP/1.1 200 OK\x0d\x0a");
			hs_proxy.Send("Content-Length: 10\x0d\x0a");
			hs_proxy.Send("Connection: Close\x0d\x0a");
			hs_proxy.Send("\x0d\x0a");
			hs_proxy.Send("0123456789");
			hs_proxy.CloseConnection();

		}

		WaitForSingleObject (hDoUpdate, INFINITE);
		QC_BOOST_CHECK_MESSAGE ( CORE_INVALID_SIGNATURE == do_upd.GetError(), L"Wrong return code\n" );
	}

	{// check settings collision - bypass is on, the source is external (w/o proxy) 
		TestLog testLog;

		HttpServer hs_proxy (3127, "127.0.0.1");
		hs_proxy.Start();

		_helper::DeleteDirectory(ROOT);
		KLUPD::createFolder (BASES_ROOT, &testLog);
		KLUPD::createFolder (UPD_ROOT, &testLog);

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
		data.connect_tmo = 1000;

		data.m_sourceList.addSource (UST_UserURL, L"http://kaspersky.ru/", false, L"");

		data.m_proxyAuthorizationCredentials.userName ( L"");
		data.m_proxyAuthorizationCredentials.password ( L"");
		data.proxy_url = L"127.0.0.1";
		data.proxy_port = 3127;
		data.use_ie_proxy = false;
		data.m_bypassProxyServerForLocalAddresses = true;
		//data.m_httpAuthorizationMethods.push_back (basicAuthorization);
		data.UpdaterListFlags.update_bases = true;

		Callbacks cbacks;
		TestCallbackInterface testCallback (&testLog, &cbacks);
		TestJournalInterface journal;
		testCallback.SetUpdaterConfiguration (data);

		Updater upd (testCallback.m_dp, testCallback, journal, &testLog);

		HANDLE hDoUpdate;

		doUpdater do_upd(&upd, hDoUpdate);

		// проверить, что запрос идет на внешний адрес, мы не можем,
		// так что ждем, что к нашему прокси нет запросов
		QC_BOOST_CHECK_MESSAGE ( WSA_WAIT_TIMEOUT == hs_proxy.Accept(15000), L"Incorrect connection\n" );

		WaitForSingleObject (hDoUpdate, INFINITE);
		//не нужно нам проверять, что там с kaspersky.ru скачалось - не скачалось
		//QC_BOOST_CHECK_MESSAGE ( CORE_DOWNLOAD_ERROR == do_upd.GetError(), L"Wrong return code\n" );
	}
*/
}

void TestSuiteNet::TestWpadProxyDetectResult_IpV6 ()
{
	using namespace KLUPD;
	std::wcout<<L"TestWpadProxyDetectResult_IpV6\n";
	Logger::Case _case (m_logger, m_testp, m_step, L"TestWpadProxyDetectResult_IpV6");

	//проверка обработки результата, возвращенного wpad (особенно, если возвращается DIRECT
	//и особенно для ftp-сервера, т.к. в этом случае различается используемый протокол)

	{//http - direct connection
		TestLog testLog;

		HttpServer hs_server (8001, "[::1]", IP_V6);
		hs_server.Start();

		_helper::DeleteDirectory(ROOT);
		KLUPD::createFolder (BASES_ROOT, &testLog);
		KLUPD::createFolder (UPD_ROOT, &testLog);

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
		data.connect_tmo = 1000;

		data.m_sourceList.addSource (UST_UserURL, L"http://[::1]:8001/", true, L"");

		data.m_proxyAuthorizationCredentials.userName ( L"null");
		data.m_proxyAuthorizationCredentials.password ( L"null");
		data.proxy_url = L"127.0.0.1";
		data.proxy_port = 3127;
		data.use_ie_proxy = true;
		data.m_httpAuthorizationMethods.push_back (basicAuthorization);
		data.UpdaterListFlags.update_bases = true;

		Callbacks cbacks;
		TestCallbackInterface testCallback (&testLog, &cbacks);
		TestSuiteProxyDetector::ProxyJournal journal ( std::wstring(COLL_ROOT).append(L"\\wpads\\direct_wpad_ipv6.dat") );
		testCallback.SetUpdaterConfiguration (data);

		Updater upd (testCallback.m_dp, testCallback, journal, &testLog);
		journal.SetUpdater(&upd);

		HANDLE hDoUpdate;

		doUpdater do_upd(&upd, hDoUpdate);

		string command;
		string param;
		HttpServer::CMD_TYPE type;

		{	//klz
			hs_server.Accept();
			hs_server.Receive(command, type, param);
			QC_BOOST_CHECK_MESSAGE (type == HttpServer::CMD_GET, L"Error\n");
			QC_BOOST_CHECK_MESSAGE (command.find("host: [::1]") != wstring::npos, L"Error\n");
			hs_server.Send("HTTP/1.1 200 OK\x0d\x0a");
			hs_server.Send("Content-Length: 10\x0d\x0a");
			hs_server.Send("Connection: Close\x0d\x0a");
			hs_server.Send("\x0d\x0a");
			hs_server.Send("0123456789");
			hs_server.CloseConnection();

			//xml
			hs_server.Accept();
			hs_server.Receive(command, type, param);
			QC_BOOST_CHECK_MESSAGE (type == HttpServer::CMD_GET, L"Error\n");
			QC_BOOST_CHECK_MESSAGE (command.find("host: [::1]") != wstring::npos, L"Error\n");
			hs_server.Send("HTTP/1.1 200 OK\x0d\x0a");
			hs_server.Send("Content-Length: 10\x0d\x0a");
			hs_server.Send("Connection: Close\x0d\x0a");
			hs_server.Send("\x0d\x0a");
			hs_server.Send("0123456789");
			hs_server.CloseConnection();

		}

		WaitForSingleObject (hDoUpdate, INFINITE);
		QC_BOOST_CHECK_MESSAGE ( CORE_INVALID_SIGNATURE == do_upd.GetError(), L"Wrong return code\n" );
	}

	{//ftp - proxy connection
		TestLog testLog;

		HttpServer hs_proxy (3128, _helper::GetSelfIp_IpV6(), IP_V6);
		hs_proxy.Start();

		_helper::DeleteDirectory(ROOT);
		KLUPD::createFolder (BASES_ROOT, &testLog);
		KLUPD::createFolder (UPD_ROOT, &testLog);

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
		data.connect_tmo = 1000;

		data.m_sourceList.addSource (UST_UserURL, L"ftp://[::1]:8001/", true, L"");

		data.m_proxyAuthorizationCredentials.userName ( L"null");
		data.m_proxyAuthorizationCredentials.password ( L"null");
		data.proxy_url = L"[::1]";
		data.proxy_port = 3127;
		data.use_ie_proxy = true;
		data.m_httpAuthorizationMethods.push_back (basicAuthorization);
		data.UpdaterListFlags.update_bases = true;

		Callbacks cbacks;
		TestCallbackInterface testCallback (&testLog, &cbacks);
		TestSuiteProxyDetector::ProxyJournal journal ( std::wstring(COLL_ROOT).append(L"\\wpads\\proxy_wpad_ipv6.dat") );
		testCallback.SetUpdaterConfiguration (data);

		Updater upd (testCallback.m_dp, testCallback, journal, &testLog);
		journal.SetUpdater(&upd);

		HANDLE hDoUpdate;

		doUpdater do_upd(&upd, hDoUpdate);

		string command;
		string param;
		HttpServer::CMD_TYPE type;

		{	//klz
			hs_proxy.Accept();
			hs_proxy.Receive(command, type, param);
			QC_BOOST_CHECK_MESSAGE (type == HttpServer::CMD_GET, L"Error\n");
			QC_BOOST_CHECK_MESSAGE (command.find("get ftp://") != wstring::npos, L"Error\n");
			hs_proxy.Send("HTTP/1.1 200 OK\x0d\x0a");
			hs_proxy.Send("Content-Length: 10\x0d\x0a");
			hs_proxy.Send("Connection: Close\x0d\x0a");
			hs_proxy.Send("\x0d\x0a");
			hs_proxy.Send("0123456789");
			hs_proxy.CloseConnection();

			//xml
			hs_proxy.Accept();
			hs_proxy.Receive(command, type, param);
			QC_BOOST_CHECK_MESSAGE (type == HttpServer::CMD_GET, L"Error\n");
			QC_BOOST_CHECK_MESSAGE (command.find("get ftp://") != wstring::npos, L"Error\n");
			hs_proxy.Send("HTTP/1.1 200 OK\x0d\x0a");
			hs_proxy.Send("Content-Length: 10\x0d\x0a");
			hs_proxy.Send("Connection: Close\x0d\x0a");
			hs_proxy.Send("\x0d\x0a");
			hs_proxy.Send("0123456789");
			hs_proxy.CloseConnection();

		}

		WaitForSingleObject (hDoUpdate, INFINITE);
		QC_BOOST_CHECK_MESSAGE ( CORE_INVALID_SIGNATURE == do_upd.GetError(), L"Wrong return code\n" );
	}

	{//ftp - direct connection
		TestLog testLog;

		FtpServer fs (1212, "[::1]", IP_V6);
		fs.Start();

		_helper::DeleteDirectory(ROOT);
		KLUPD::createFolder (BASES_ROOT, &testLog);
		KLUPD::createFolder (UPD_ROOT, &testLog);

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
		data.connect_tmo = 1000;

		data.m_sourceList.addSource (UST_UserURL, L"ftp://[::1]:1212/", true, L"");

		data.m_proxyAuthorizationCredentials.userName ( L"null");
		data.m_proxyAuthorizationCredentials.password ( L"null");
		data.proxy_url = L"[::1]";
		data.proxy_port = 3127;
		data.use_ie_proxy = true;
		data.m_httpAuthorizationMethods.push_back (basicAuthorization);
		data.UpdaterListFlags.update_bases = true;

		Callbacks cbacks;
		TestCallbackInterface testCallback (&testLog, &cbacks);
		TestSuiteProxyDetector::ProxyJournal journal ( std::wstring(COLL_ROOT).append(L"\\wpads\\direct_wpad_ipv6.dat") );
		testCallback.SetUpdaterConfiguration (data);

		Updater upd (testCallback.m_dp, testCallback, journal, &testLog);
		journal.SetUpdater(&upd);

		HANDLE hDoUpdate;

		doUpdater do_upd(&upd, hDoUpdate);

		{	//klz
			fs.Accept();
			fs.CloseControlConnection();

			//xml
			fs.Accept();
			fs.CloseControlConnection();
		}

		WaitForSingleObject (hDoUpdate, INFINITE);
		QC_BOOST_CHECK_MESSAGE ( CORE_REMOTE_HOST_CLOSED_CONNECTION == do_upd.GetError(), L"Wrong return code\n" );
	}
}