#include "util.h"

#include "log.h"
#include "callback.h"
#include "journal.h"

#include "core/updater.h"
#include "helper/local_file.h"

//#include "..\..\..\Client\net\netcore.h"
#include "..\..\..\Client\helper\fileInfo.h"
#include "..\..\..\Client\helper\sites_info.h"


#include "TestSuite.h"
#include "TestHttpProxy.h"
#include "main.h"

#include "HttpServer.h"
#include "FtpServer.h"
#include "TestSuiteProxyDetector.h"

#include "constants.h"

void TestHttpProxy::TestBasicAuth ()
{
	using namespace KLUPD;
	std::wcout<<L"TestBasicAuth\n";
	Logger::Case _case (m_logger, m_testp, m_step, L"TestBasicAuth");

	{//variant 1 - proxy-connection close
		HttpServer hs (3128, "127.0.0.1");
		hs.Start();

		
		TestLog testLog;
		Callbacks cbacks;
		TestCallbackInterface testCallback (&testLog, &cbacks);
		TestJournalInterface testJournal;
		Signature6Checker s6c (&testLog);

		UpdaterConfiguration data(1, 1, L"1");
		data.proxy_url = L"127.0.0.1";
		data.proxy_port = 3128;
		data.m_proxyAuthorizationCredentials.userName ( L"test");
		data.m_proxyAuthorizationCredentials.password ( L"test");
		data.m_httpAuthorizationMethods.push_back (basicAuthorization);
		data.m_sourceList.addSource (UST_UserURL, L"http://httpserver", true, L"diffs");
		testCallback.SetUpdaterConfiguration (data);
		KLUPD::Source _csd (L"http://httpserver", UST_UserURL, true, L"", UPS_DIRS);
		Updater upd (testCallback.m_dp, testCallback, testJournal, &testLog);
		upd.constructSourceList(testCallback.updaterConfiguration().m_sourceList, upd.m_sourceList);
		
				
		HANDLE hDoDownload;


		{

			FileInfo _fi (L"base005.avc", L"/");
			_fi.m_localPath = L"h:\\test\\";

			_helper::DeleteDirectory(TEST_FOLDER);
			_helper::MakeDirectory(TEST_FOLDER);
			
			doEntryDownloader do_dld(&upd, hDoDownload, _fi, _fi.m_localPath, &s6c);
			
			hs.Accept();
		
			QC_BOOST_CHECK_MESSAGE (hs.ReceivePureGet(), L"Error getting request\n");

			hs.SendBasicGreeting ();
			
			QC_BOOST_CHECK_MESSAGE (hs.ReceiveGetWithBasicToken(), L"Error getting request\n");
			hs.SendFileFromBuffer (HttpServer::KeepAlive, HttpServer::Close, "0123456789");
			hs.CloseConnection ();
			
			WaitForSingleObject (hDoDownload, INFINITE);
			QC_BOOST_CHECK_MESSAGE ( CORE_NO_ERROR == do_dld.GetError(), L"Wrong return code\n" );
			QC_BOOST_CHECK_MESSAGE ( _helper::checkFileContent (wstring(TEST_FOLDER).append (L"\\base005.avc"),
											"0123456789"), L"Wrong file content\n" );
		}

		{

			FileInfo _fi (L"base004.avc", L"/");
			_fi.m_localPath = L"h:\\test\\";

			_helper::DeleteDirectory(TEST_FOLDER);
			_helper::MakeDirectory(TEST_FOLDER);
			
			doEntryDownloader do_dld(&upd, hDoDownload, _fi, _fi.m_localPath, &s6c);

			hs.Accept();
			QC_BOOST_CHECK_MESSAGE (hs.ReceiveGetWithBasicToken(), L"Error getting request\n");
			hs.SendFileFromBuffer (HttpServer::KeepAlive, HttpServer::Close, "0123456789");
			hs.CloseConnection ();
			
			WaitForSingleObject (hDoDownload, INFINITE);
			QC_BOOST_CHECK_MESSAGE ( CORE_NO_ERROR == do_dld.GetError(), L"Wrong return code\n" );
			QC_BOOST_CHECK_MESSAGE ( _helper::checkFileContent (wstring(TEST_FOLDER).append (L"\\base004.avc"),
											"0123456789"), L"Wrong file content\n" );
		}

		{

			FileInfo _fi (L"base003.avc", L"/");
			_fi.m_localPath = L"h:\\test\\";

			_helper::DeleteDirectory(TEST_FOLDER);
			_helper::MakeDirectory(TEST_FOLDER);
			
			doEntryDownloader do_dld(&upd, hDoDownload, _fi, _fi.m_localPath, &s6c);

			hs.Accept();
			QC_BOOST_CHECK_MESSAGE (hs.ReceiveGetWithBasicToken(), L"Error getting request\n");
			hs.SendFileFromBuffer (HttpServer::KeepAlive, HttpServer::Close, "0123456789");
			hs.CloseConnection ();
			
			WaitForSingleObject (hDoDownload, INFINITE);
			QC_BOOST_CHECK_MESSAGE ( CORE_NO_ERROR == do_dld.GetError(), L"Wrong return code\n" );
			QC_BOOST_CHECK_MESSAGE ( _helper::checkFileContent (wstring(TEST_FOLDER).append (L"\\base003.avc"),
											"0123456789"), L"Wrong file content\n" );
		}
		
	}
	

	{//variant 2 proxy-connection keep-alive
		HttpServer hs (3128, "127.0.0.1");
		hs.Start();

		
		TestLog testLog;
		Callbacks cbacks;
		TestCallbackInterface testCallback (&testLog, &cbacks);
		TestJournalInterface testJournal;
		Signature6Checker s6c (&testLog);

		UpdaterConfiguration data(1, 1, L"1");
		data.proxy_url = L"127.0.0.1";
		data.proxy_port = 3128;
		data.m_proxyAuthorizationCredentials.userName ( L"test");
		data.m_proxyAuthorizationCredentials.password ( L"test");
		data.m_httpAuthorizationMethods.push_back (basicAuthorization);
		data.m_sourceList.addSource (UST_UserURL, L"http://httpserver", true, L"diffs");
		testCallback.SetUpdaterConfiguration (data);
		KLUPD::Source _csd (L"http://httpserver", UST_UserURL, true, L"", UPS_DIRS);
		Updater upd (testCallback.m_dp, testCallback, testJournal, &testLog);
		upd.constructSourceList(testCallback.updaterConfiguration().m_sourceList, upd.m_sourceList);
		
				
		HANDLE hDoDownload;

		{

			FileInfo _fi (L"base005.avc", L"/");
			_fi.m_localPath = L"h:\\test\\";

			_helper::DeleteDirectory(TEST_FOLDER);
			_helper::MakeDirectory(TEST_FOLDER);
			
			doEntryDownloader do_dld(&upd, hDoDownload, _fi, _fi.m_localPath, &s6c);
			
			hs.Accept();
			QC_BOOST_CHECK_MESSAGE (hs.ReceivePureGet(), L"Error getting request\n");
			hs.SendBasicGreeting ();
			QC_BOOST_CHECK_MESSAGE (hs.ReceiveGetWithBasicToken(), L"Error getting request\n");
			hs.SendFileFromBuffer (HttpServer::KeepAlive, HttpServer::KeepAlive, "0123456789");

			
			WaitForSingleObject (hDoDownload, INFINITE);
			QC_BOOST_CHECK_MESSAGE ( CORE_NO_ERROR == do_dld.GetError(), L"Wrong return code\n" );
			QC_BOOST_CHECK_MESSAGE ( _helper::checkFileContent (wstring(TEST_FOLDER).append (L"\\base005.avc"),
											"0123456789"), L"Wrong file content\n" );
		}

		{

			FileInfo _fi (L"base004.avc", L"/");
			_fi.m_localPath = L"h:\\test\\";

			_helper::DeleteDirectory(TEST_FOLDER);
			_helper::MakeDirectory(TEST_FOLDER);
			
			doEntryDownloader do_dld(&upd, hDoDownload, _fi, _fi.m_localPath, &s6c);

			QC_BOOST_CHECK_MESSAGE (hs.ReceiveGetWithBasicToken(), L"Error getting request\n");
			hs.SendFileFromBuffer (HttpServer::KeepAlive, HttpServer::KeepAlive, "0123456789");
						
			WaitForSingleObject (hDoDownload, INFINITE);
			QC_BOOST_CHECK_MESSAGE ( CORE_NO_ERROR == do_dld.GetError(), L"Wrong return code\n" );
			QC_BOOST_CHECK_MESSAGE ( _helper::checkFileContent (wstring(TEST_FOLDER).append (L"\\base004.avc"),
											"0123456789"), L"Wrong file content\n" );
		}

		{

			FileInfo _fi (L"base003.avc", L"/");
			_fi.m_localPath = L"h:\\test\\";

			_helper::DeleteDirectory(TEST_FOLDER);
			_helper::MakeDirectory(TEST_FOLDER);
			
			doEntryDownloader do_dld(&upd, hDoDownload, _fi, _fi.m_localPath, &s6c);
		
			QC_BOOST_CHECK_MESSAGE (hs.ReceiveGetWithBasicToken(), L"Error getting request\n");
			hs.SendFileFromBuffer (HttpServer::KeepAlive, HttpServer::KeepAlive, "0123456789");
			
			hs.CloseConnection ();
			
			WaitForSingleObject (hDoDownload, INFINITE);
			QC_BOOST_CHECK_MESSAGE ( CORE_NO_ERROR == do_dld.GetError(), L"Wrong return code\n" );
			QC_BOOST_CHECK_MESSAGE ( _helper::checkFileContent (wstring(TEST_FOLDER).append (L"\\base003.avc"),
											"0123456789"), L"Wrong file content\n" );
		}	
	}
	
	{//variant 3 - proxy-connection mixed
		HttpServer hs (3128, "127.0.0.1");
		hs.Start();

		
		TestLog testLog;
		Callbacks cbacks;
		TestCallbackInterface testCallback (&testLog, &cbacks);
		TestJournalInterface testJournal;
		Signature6Checker s6c (&testLog);

		UpdaterConfiguration data(1, 1, L"1");
		data.proxy_url = L"127.0.0.1";
		data.proxy_port = 3128;
		data.m_proxyAuthorizationCredentials.userName ( L"test");
		data.m_proxyAuthorizationCredentials.password ( L"test");
		data.m_httpAuthorizationMethods.push_back (basicAuthorization);
		data.m_sourceList.addSource (UST_UserURL, L"http://httpserver", true, L"diffs");
		testCallback.SetUpdaterConfiguration (data);
		KLUPD::Source _csd (L"http://httpserver", UST_UserURL, true, L"", UPS_DIRS);
		Updater upd (testCallback.m_dp, testCallback, testJournal, &testLog);
		upd.constructSourceList(testCallback.updaterConfiguration().m_sourceList, upd.m_sourceList);
		
				
		HANDLE hDoDownload;
		{

			FileInfo _fi (L"base005.avc", L"/");
			_fi.m_localPath = L"h:\\test\\";

			_helper::DeleteDirectory(TEST_FOLDER);
			_helper::MakeDirectory(TEST_FOLDER);
			
			doEntryDownloader do_dld(&upd, hDoDownload, _fi, _fi.m_localPath, &s6c);
			
			hs.Accept();
			QC_BOOST_CHECK_MESSAGE (hs.ReceivePureGet(), L"Error getting request\n");
			hs.SendBasicGreeting ();
			QC_BOOST_CHECK_MESSAGE (hs.ReceiveGetWithBasicToken(), L"Error getting request\n");
			hs.SendFileFromBuffer (HttpServer::KeepAlive, HttpServer::Close, "0123456789");
			hs.CloseConnection();
	
			WaitForSingleObject (hDoDownload, INFINITE);
			QC_BOOST_CHECK_MESSAGE ( CORE_NO_ERROR == do_dld.GetError(), L"Wrong return code\n" );
			QC_BOOST_CHECK_MESSAGE ( _helper::checkFileContent (wstring(TEST_FOLDER).append (L"\\base005.avc"),
											"0123456789"), L"Wrong file content\n" );
		}

		{

			FileInfo _fi (L"base004.avc", L"/");
			_fi.m_localPath = L"h:\\test\\";

			_helper::DeleteDirectory(TEST_FOLDER);
			_helper::MakeDirectory(TEST_FOLDER);
			
			doEntryDownloader do_dld(&upd, hDoDownload, _fi, _fi.m_localPath, &s6c);

			hs.Accept();
			QC_BOOST_CHECK_MESSAGE (hs.ReceiveGetWithBasicToken(), L"Error getting request\n");
			hs.SendFileFromBuffer (HttpServer::KeepAlive, HttpServer::KeepAlive, "0123456789");
						
			WaitForSingleObject (hDoDownload, INFINITE);
			QC_BOOST_CHECK_MESSAGE ( CORE_NO_ERROR == do_dld.GetError(), L"Wrong return code\n" );
			QC_BOOST_CHECK_MESSAGE ( _helper::checkFileContent (wstring(TEST_FOLDER).append (L"\\base004.avc"),
											"0123456789"), L"Wrong file content\n" );
		}

		{

			FileInfo _fi (L"base003.avc", L"/");
			_fi.m_localPath = L"h:\\test\\";

			_helper::DeleteDirectory(TEST_FOLDER);
			_helper::MakeDirectory(TEST_FOLDER);
			
			doEntryDownloader do_dld(&upd, hDoDownload, _fi, _fi.m_localPath, &s6c);
		
			QC_BOOST_CHECK_MESSAGE (hs.ReceiveGetWithBasicToken(), L"Error getting request\n");
			hs.SendFileFromBuffer (HttpServer::KeepAlive, HttpServer::Close, "0123456789");
			
			hs.CloseConnection ();
			
			WaitForSingleObject (hDoDownload, INFINITE);
			QC_BOOST_CHECK_MESSAGE ( CORE_NO_ERROR == do_dld.GetError(), L"Wrong return code\n" );
			QC_BOOST_CHECK_MESSAGE ( _helper::checkFileContent (wstring(TEST_FOLDER).append (L"\\base003.avc"),
											"0123456789"), L"Wrong file content\n" );
		}	
	}
	
	
	{//variant 4 - непонятно, что это
		HttpServer hs (3128, "127.0.0.1");
		hs.Start();

		
		TestLog testLog;
		Callbacks cbacks;
		TestCallbackInterface testCallback (&testLog, &cbacks);
		TestJournalInterface testJournal;
		Signature6Checker s6c (&testLog);

		UpdaterConfiguration data(1, 1, L"1");
		data.proxy_url = L"127.0.0.1";
		data.proxy_port = 3128;
		data.m_proxyAuthorizationCredentials.userName ( L"test");
		data.m_proxyAuthorizationCredentials.password ( L"test");
		data.m_httpAuthorizationMethods.push_back (basicAuthorization);
		data.m_sourceList.addSource (UST_UserURL, L"http://httpserver", true, L"diffs");
		testCallback.SetUpdaterConfiguration (data);
		KLUPD::Source _csd (L"http://httpserver", UST_UserURL, true, L"", UPS_DIRS);
		Updater upd (testCallback.m_dp, testCallback, testJournal, &testLog);
		upd.constructSourceList(testCallback.updaterConfiguration().m_sourceList, upd.m_sourceList);
		
				
		HANDLE hDoDownload;

		string command;
		string param;
		HttpServer::CMD_TYPE type;

		{

			FileInfo _fi (L"base005.avc", L"/");
			_fi.m_localPath = L"h:\\test\\";

			_helper::DeleteDirectory(TEST_FOLDER);
			_helper::MakeDirectory(TEST_FOLDER);
			
			doEntryDownloader do_dld(&upd, hDoDownload, _fi, _fi.m_localPath, &s6c);
			
			hs.Accept();
		
			hs.Receive(command, type, param);
			QC_BOOST_CHECK_MESSAGE (type == HttpServer::CMD_GET, L"Error\n");
			hs.Send("HTTP/1.1 407 Proxy Authentication Required\x0d\x0a");
			hs.Send("Proxy-Authenticate: Basic realm=\"Hello\"\x0d\x0a");
			hs.Send("Connection: Keep-Alive\x0d\x0a");
			hs.Send("Proxy-Connection: Close\x0d\x0a");
			hs.Send("Content-Length: 0\x0d\x0a");
			hs.Send("\x0d\x0a");
			hs.Send("0123456789");
			
			hs.CloseConnection();

			QC_BOOST_CHECK_MESSAGE ( ! _helper::AutoFile (std::wstring(TEST_FOLDER).append(L"\\base005.avc")).Exist(),
										L"Incorrect file\n" );

			hs.Accept();
		
			hs.Receive(command, type, param);
			QC_BOOST_CHECK_MESSAGE (type == HttpServer::CMD_GET, L"Error\n");
			QC_BOOST_CHECK_MESSAGE (command.find("proxy-authorization: basic") != wstring::npos, L"Error\n");
			hs.Send("HTTP/1.1 200 OK\x0d\x0a");
			hs.Send("Content-Length: 10\x0d\x0a");
			hs.Send("Connection: Keep-Alive\x0d\x0a");
			hs.Send("Proxy-Connection: Keep-Alive\x0d\x0a");
			hs.Send("\x0d\x0a");
			hs.Send("0123456789");
			
			WaitForSingleObject (hDoDownload, INFINITE);
			QC_BOOST_CHECK_MESSAGE ( CORE_NO_ERROR == do_dld.GetError(), L"Wrong return code\n" );
			QC_BOOST_CHECK_MESSAGE ( _helper::checkFileContent (wstring(TEST_FOLDER).append (L"\\base005.avc"),
											"0123456789"), L"Wrong file content\n" );
		}

		{

			FileInfo _fi (L"base004.avc", L"/");
			_fi.m_localPath = L"h:\\test\\";

			_helper::DeleteDirectory(TEST_FOLDER);
			_helper::MakeDirectory(TEST_FOLDER);
			
			doEntryDownloader do_dld(&upd, hDoDownload, _fi, _fi.m_localPath, &s6c);

		
			hs.Receive(command, type, param);
			QC_BOOST_CHECK_MESSAGE (type == HttpServer::CMD_GET, L"Error\n");
			QC_BOOST_CHECK_MESSAGE (command.find("proxy-authorization: basic") != wstring::npos, L"Error\n");
			hs.Send("HTTP/1.1 200 OK\x0d\x0a");
			hs.Send("Content-Length: 10\x0d\x0a");
			hs.Send("Connection: Keep-Alive\x0d\x0a");
			hs.Send("Proxy-Connection: Keep-Alive\x0d\x0a");
			hs.Send("\x0d\x0a");
			hs.Send("0123456789");
			
			
			WaitForSingleObject (hDoDownload, INFINITE);
			QC_BOOST_CHECK_MESSAGE ( CORE_NO_ERROR == do_dld.GetError(), L"Wrong return code\n" );
			QC_BOOST_CHECK_MESSAGE ( _helper::checkFileContent (wstring(TEST_FOLDER).append (L"\\base004.avc"),
											"0123456789"), L"Wrong file content\n" );
		}

		{

			FileInfo _fi (L"base003.avc", L"/");
			_fi.m_localPath = L"h:\\test\\";

			_helper::DeleteDirectory(TEST_FOLDER);
			_helper::MakeDirectory(TEST_FOLDER);
			
			doEntryDownloader do_dld(&upd, hDoDownload, _fi, _fi.m_localPath, &s6c);
		
			hs.Receive(command, type, param);
			QC_BOOST_CHECK_MESSAGE (type == HttpServer::CMD_GET, L"Error\n");
			QC_BOOST_CHECK_MESSAGE (command.find("proxy-authorization: basic") != wstring::npos, L"Error\n");
			hs.Send("HTTP/1.1 200 OK\x0d\x0a");
			hs.Send("Content-Length: 10\x0d\x0a");
			hs.Send("Connection: Keep-Alive\x0d\x0a");
			hs.Send("Proxy-Connection: Close\x0d\x0a");
			hs.Send("\x0d\x0a");
			hs.Send("0123456789");
			
			hs.CloseConnection ();
			
			WaitForSingleObject (hDoDownload, INFINITE);
			QC_BOOST_CHECK_MESSAGE ( CORE_NO_ERROR == do_dld.GetError(), L"Wrong return code\n" );
			QC_BOOST_CHECK_MESSAGE ( _helper::checkFileContent (wstring(TEST_FOLDER).append (L"\\base003.avc"),
											"0123456789"), L"Wrong file content\n" );
		}	
	}
	
}

void TestHttpProxy::TestNoAuth ()
{
	using namespace KLUPD;
	std::wcout<<L"TestNoAuth\n";
	Logger::Case _case (m_logger, m_testp, m_step, L"TestNoAuth");

	{//variant 1 - proxy-connection close
		std::wcout<<L"variant 1\n";
		HttpServer hs (3128, "127.0.0.1");
		hs.Start();
		
		TestLog testLog;
		Callbacks cbacks;
		TestCallbackInterface testCallback (&testLog, &cbacks);
		TestJournalInterface testJournal;
		Signature6Checker s6c (&testLog);

		UpdaterConfiguration data(1, 1, L"1");
		data.proxy_url = L"127.0.0.1";
		data.proxy_port = 3128;
		data.m_proxyAuthorizationCredentials.userName ( L"test");
		data.m_proxyAuthorizationCredentials.password ( L"test");
		data.m_httpAuthorizationMethods.push_back (basicAuthorization);
		data.m_sourceList.addSource (UST_UserURL, L"http://httpserver", true, L"diffs");
		testCallback.SetUpdaterConfiguration (data);
		KLUPD::Source _csd (L"http://httpserver", UST_UserURL, true, L"", UPS_DIRS);
		Updater upd (testCallback.m_dp, testCallback, testJournal, &testLog);
		upd.constructSourceList(testCallback.updaterConfiguration().m_sourceList, upd.m_sourceList);
		
				
		HANDLE hDoDownload;

		{

			FileInfo _fi (L"base005.avc", L"/");
			_fi.m_localPath = L"h:\\test\\";

			_helper::DeleteDirectory(TEST_FOLDER);
			_helper::MakeDirectory(TEST_FOLDER);
			
			doEntryDownloader do_dld(&upd, hDoDownload, _fi, _fi.m_localPath, &s6c);

			hs.Accept();
		
			QC_BOOST_CHECK_MESSAGE (hs.ReceivePureGet(), L"Error getting request\n");
			hs.SendFileFromBuffer (HttpServer::KeepAlive, HttpServer::Close, "0123456789");
			
			hs.CloseConnection ();
			
			WaitForSingleObject (hDoDownload, INFINITE);
			QC_BOOST_CHECK_MESSAGE ( CORE_NO_ERROR == do_dld.GetError(), L"Wrong return code\n" );
			QC_BOOST_CHECK_MESSAGE ( _helper::checkFileContent (wstring(TEST_FOLDER).append (L"\\base005.avc"),
											"0123456789"), L"Wrong file content\n" );
		}

		{

			FileInfo _fi (L"base004.avc", L"/");
			_fi.m_localPath = L"h:\\test\\";

			_helper::DeleteDirectory(TEST_FOLDER);
			_helper::MakeDirectory(TEST_FOLDER);
			
			doEntryDownloader do_dld(&upd, hDoDownload, _fi, _fi.m_localPath, &s6c);

			hs.Accept();

			QC_BOOST_CHECK_MESSAGE (hs.ReceivePureGet(), L"Error getting request\n");
			hs.SendFileFromBuffer (HttpServer::KeepAlive, HttpServer::Close, "0123456789");

			hs.CloseConnection ();
			
			WaitForSingleObject (hDoDownload, INFINITE);
			QC_BOOST_CHECK_MESSAGE ( CORE_NO_ERROR == do_dld.GetError(), L"Wrong return code\n" );
			QC_BOOST_CHECK_MESSAGE ( _helper::checkFileContent (wstring(TEST_FOLDER).append (L"\\base004.avc"),
											"0123456789"), L"Wrong file content\n" );
		}

		{

			FileInfo _fi (L"base003.avc", L"/");
			_fi.m_localPath = L"h:\\test\\";

			_helper::DeleteDirectory(TEST_FOLDER);
			_helper::MakeDirectory(TEST_FOLDER);
			
			doEntryDownloader do_dld(&upd, hDoDownload, _fi, _fi.m_localPath, &s6c);

			hs.Accept();

			QC_BOOST_CHECK_MESSAGE (hs.ReceivePureGet(), L"Error getting request\n");
			hs.SendFileFromBuffer (HttpServer::KeepAlive, HttpServer::Close, "0123456789");

			hs.CloseConnection ();

			
			WaitForSingleObject (hDoDownload, INFINITE);
			QC_BOOST_CHECK_MESSAGE ( CORE_NO_ERROR == do_dld.GetError(), L"Wrong return code\n" );
			QC_BOOST_CHECK_MESSAGE ( _helper::checkFileContent (wstring(TEST_FOLDER).append (L"\\base003.avc"),
											"0123456789"), L"Wrong file content\n" );
		}	
	}
	

	{// variant 2 - - proxy-connection keep-alive
		std::wcout<<L"variant 2\n";
		HttpServer hs (3128, "127.0.0.1");
		hs.Start();
		
		TestLog testLog;
		Callbacks cbacks;
		TestCallbackInterface testCallback (&testLog, &cbacks);
		TestJournalInterface testJournal;
		Signature6Checker s6c (&testLog);

		UpdaterConfiguration data(1, 1, L"1");
		data.proxy_url = L"127.0.0.1";
		data.proxy_port = 3128;
		data.m_proxyAuthorizationCredentials.userName ( L"test");
		data.m_proxyAuthorizationCredentials.password ( L"test");
		data.m_httpAuthorizationMethods.push_back (basicAuthorization);
		data.m_sourceList.addSource (UST_UserURL, L"http://httpserver", true, L"diffs");
		testCallback.SetUpdaterConfiguration (data);
		KLUPD::Source _csd (L"http://httpserver", UST_UserURL, true, L"", UPS_DIRS);
		Updater upd (testCallback.m_dp, testCallback, testJournal, &testLog);
		upd.constructSourceList(testCallback.updaterConfiguration().m_sourceList, upd.m_sourceList);
		
				
		HANDLE hDoDownload;

		string command;
		string param;

		{

			FileInfo _fi (L"base005.avc", L"/");
			_fi.m_localPath = L"h:\\test\\";

			_helper::DeleteDirectory(TEST_FOLDER);
			_helper::MakeDirectory(TEST_FOLDER);
			
			doEntryDownloader do_dld(&upd, hDoDownload, _fi, _fi.m_localPath, &s6c);

			hs.Accept();
		
			QC_BOOST_CHECK_MESSAGE (hs.ReceivePureGet(), L"Error getting request\n");
			hs.SendFileFromBuffer (HttpServer::KeepAlive, HttpServer::KeepAlive, "0123456789");
			
			WaitForSingleObject (hDoDownload, INFINITE);
			QC_BOOST_CHECK_MESSAGE ( CORE_NO_ERROR == do_dld.GetError(), L"Wrong return code\n" );
			QC_BOOST_CHECK_MESSAGE ( _helper::checkFileContent (wstring(TEST_FOLDER).append (L"\\base005.avc"),
											"0123456789"), L"Wrong file content\n" );
		}

		{

			FileInfo _fi (L"base004.avc", L"/");
			_fi.m_localPath = L"h:\\test\\";

			_helper::DeleteDirectory(TEST_FOLDER);
			_helper::MakeDirectory(TEST_FOLDER);
			
			doEntryDownloader do_dld(&upd, hDoDownload, _fi, _fi.m_localPath, &s6c);
		
			QC_BOOST_CHECK_MESSAGE (hs.ReceivePureGet(), L"Error getting request\n");
			hs.SendFileFromBuffer (HttpServer::KeepAlive, HttpServer::KeepAlive, "0123456789");
			
			WaitForSingleObject (hDoDownload, INFINITE);
			QC_BOOST_CHECK_MESSAGE ( CORE_NO_ERROR == do_dld.GetError(), L"Wrong return code\n" );
			QC_BOOST_CHECK_MESSAGE ( _helper::checkFileContent (wstring(TEST_FOLDER).append (L"\\base004.avc"),
											"0123456789"), L"Wrong file content\n" );
		}

		{

			FileInfo _fi (L"base003.avc", L"/");
			_fi.m_localPath = L"h:\\test\\";

			_helper::DeleteDirectory(TEST_FOLDER);
			_helper::MakeDirectory(TEST_FOLDER);
			
			doEntryDownloader do_dld(&upd, hDoDownload, _fi, _fi.m_localPath, &s6c);
		
			QC_BOOST_CHECK_MESSAGE (hs.ReceivePureGet(), L"Error getting request\n");
			hs.SendFileFromBuffer (HttpServer::KeepAlive, HttpServer::KeepAlive, "0123456789");
			
			hs.CloseConnection ();
			
			WaitForSingleObject (hDoDownload, INFINITE);
			QC_BOOST_CHECK_MESSAGE ( CORE_NO_ERROR == do_dld.GetError(), L"Wrong return code\n" );
			QC_BOOST_CHECK_MESSAGE ( _helper::checkFileContent (wstring(TEST_FOLDER).append (L"\\base003.avc"),
											"0123456789"), L"Wrong file content\n" );
		}	
	}
	

	{//variant 3 - proxy - connection mixed
		std::wcout<<L"variant 3\n";
		HttpServer hs (3128, "127.0.0.1");
		hs.Start();
		
		TestLog testLog;
		Callbacks cbacks;
		TestCallbackInterface testCallback (&testLog, &cbacks);
		TestJournalInterface testJournal;
		Signature6Checker s6c (&testLog);

		UpdaterConfiguration data(1, 1, L"1");
		data.proxy_url = L"127.0.0.1";
		data.proxy_port = 3128;
		data.m_proxyAuthorizationCredentials.userName ( L"test");
		data.m_proxyAuthorizationCredentials.password ( L"test");
		data.m_httpAuthorizationMethods.push_back (basicAuthorization);
		data.m_sourceList.addSource (UST_UserURL, L"http://httpserver", true, L"diffs");
		testCallback.SetUpdaterConfiguration (data);
		KLUPD::Source _csd (L"http://httpserver", UST_UserURL, true, L"", UPS_DIRS);
		Updater upd (testCallback.m_dp, testCallback, testJournal, &testLog);
		upd.constructSourceList(testCallback.updaterConfiguration().m_sourceList, upd.m_sourceList);	
				
		HANDLE hDoDownload;
		{

			FileInfo _fi (L"base005.avc", L"/");
			_fi.m_localPath = L"h:\\test\\";

			_helper::DeleteDirectory(TEST_FOLDER);
			_helper::MakeDirectory(TEST_FOLDER);
			
			doEntryDownloader do_dld(&upd, hDoDownload, _fi, _fi.m_localPath, &s6c);

			hs.Accept();
			QC_BOOST_CHECK_MESSAGE (hs.ReceivePureGet(), L"Error getting request\n");
			hs.SendFileFromBuffer (HttpServer::KeepAlive, HttpServer::Close, "0123456789");

			hs.CloseConnection();
			
			WaitForSingleObject (hDoDownload, INFINITE);
			QC_BOOST_CHECK_MESSAGE ( CORE_NO_ERROR == do_dld.GetError(), L"Wrong return code\n" );
			QC_BOOST_CHECK_MESSAGE ( _helper::checkFileContent (wstring(TEST_FOLDER).append (L"\\base005.avc"),
											"0123456789"), L"Wrong file content\n" );
		}

		{

			FileInfo _fi (L"base004.avc", L"/");
			_fi.m_localPath = L"h:\\test\\";

			_helper::DeleteDirectory(TEST_FOLDER);
			_helper::MakeDirectory(TEST_FOLDER);
			
			doEntryDownloader do_dld(&upd, hDoDownload, _fi, _fi.m_localPath, &s6c);
		
			hs.Accept();
		
			QC_BOOST_CHECK_MESSAGE (hs.ReceivePureGet(), L"Error getting request\n");
			hs.SendFileFromBuffer (HttpServer::KeepAlive, HttpServer::KeepAlive, "0123456789");
			
			WaitForSingleObject (hDoDownload, INFINITE);
			QC_BOOST_CHECK_MESSAGE ( CORE_NO_ERROR == do_dld.GetError(), L"Wrong return code\n" );
			QC_BOOST_CHECK_MESSAGE ( _helper::checkFileContent (wstring(TEST_FOLDER).append (L"\\base004.avc"),
											"0123456789"), L"Wrong file content\n" );
		}

		{

			FileInfo _fi (L"base003.avc", L"/");
			_fi.m_localPath = L"h:\\test\\";

			_helper::DeleteDirectory(TEST_FOLDER);
			_helper::MakeDirectory(TEST_FOLDER);
			
			doEntryDownloader do_dld(&upd, hDoDownload, _fi, _fi.m_localPath, &s6c);
		
			QC_BOOST_CHECK_MESSAGE (hs.ReceivePureGet(), L"Error getting request\n");
			hs.SendFileFromBuffer (HttpServer::KeepAlive, HttpServer::KeepAlive, "0123456789");
			hs.CloseConnection();
			
			WaitForSingleObject (hDoDownload, INFINITE);
			QC_BOOST_CHECK_MESSAGE ( CORE_NO_ERROR == do_dld.GetError(), L"Wrong return code\n" );
			QC_BOOST_CHECK_MESSAGE ( _helper::checkFileContent (wstring(TEST_FOLDER).append (L"\\base003.avc"),
											"0123456789"), L"Wrong file content\n" );
		}	
	}
	
}

void TestHttpProxy::TestNtlmAuth ()
{
	using namespace KLUPD;
	std::wcout<<L"TestNtlmAuth\n";
	Logger::Case _case (m_logger, m_testp, m_step, L"TestNtlmAuth");

	{//variant 1 - proxy-connection keep-alive
		HttpServer hs (3128, "127.0.0.1");
		hs.Start();
	
		TestLog testLog;
		Callbacks cbacks;
		TestCallbackInterface testCallback (&testLog, &cbacks);
		TestJournalInterface testJournal;
		Signature6Checker s6c (&testLog);

		UpdaterConfiguration data(1, 1, L"1");
		data.proxy_url = L"127.0.0.1";
		data.proxy_port = 3128;
		data.m_proxyAuthorizationCredentials.userName ( L"не нужен");
		data.m_proxyAuthorizationCredentials.password ( L"не нужен");
		data.m_httpAuthorizationMethods.push_back (ntlmAuthorization);
		data.m_sourceList.addSource (UST_UserURL, L"http://httpserver", true, L"diffs");
		testCallback.SetUpdaterConfiguration (data);
		KLUPD::Source _csd (L"http://httpserver", UST_UserURL, true, L"", UPS_DIRS);
		Updater upd (testCallback.m_dp, testCallback, testJournal, &testLog);
		upd.constructSourceList(testCallback.updaterConfiguration().m_sourceList, upd.m_sourceList);	
				
		HANDLE hDoDownload;
		{

			FileInfo _fi (L"base005.avc", L"/");
			_fi.m_localPath = L"h:\\test\\";

			_helper::DeleteDirectory(TEST_FOLDER);
			_helper::MakeDirectory(TEST_FOLDER);
			
			doEntryDownloader do_dld(&upd, hDoDownload, _fi, _fi.m_localPath, &s6c);
			
			hs.Accept();
			QC_BOOST_CHECK_MESSAGE (hs.ReceivePureGet(), L"Error getting request\n");
			hs.SendNtlmGreeting ();

			//msg1
			QC_BOOST_CHECK_MESSAGE (hs.ReceiveGetWithNtlmToken(), L"Error getting request\n");
			//msg2
			hs.SendNtlmMsg2();
			//msg3
			QC_BOOST_CHECK_MESSAGE (hs.ReceiveGetWithNtlmToken(), L"Error getting request\n");

			hs.SendFileFromBuffer (HttpServer::KeepAlive, HttpServer::KeepAlive, "0123456789");

			
			WaitForSingleObject (hDoDownload, INFINITE);
			QC_BOOST_CHECK_MESSAGE ( CORE_NO_ERROR == do_dld.GetError(), L"Wrong return code\n" );
			QC_BOOST_CHECK_MESSAGE ( _helper::checkFileContent (wstring(TEST_FOLDER).append (L"\\base005.avc"),
											"0123456789"), L"Wrong file content\n" );
		}

		{

			FileInfo _fi (L"base004.avc", L"/");
			_fi.m_localPath = L"h:\\test\\";

			_helper::DeleteDirectory(TEST_FOLDER);
			_helper::MakeDirectory(TEST_FOLDER);
			
			doEntryDownloader do_dld(&upd, hDoDownload, _fi, _fi.m_localPath, &s6c);
			
			QC_BOOST_CHECK_MESSAGE (hs.ReceivePureGet(), L"Error getting request\n");
			hs.SendFileFromBuffer (HttpServer::KeepAlive, HttpServer::KeepAlive, "0123456789");
			
			WaitForSingleObject (hDoDownload, INFINITE);
			QC_BOOST_CHECK_MESSAGE ( CORE_NO_ERROR == do_dld.GetError(), L"Wrong return code\n" );
			QC_BOOST_CHECK_MESSAGE ( _helper::checkFileContent (wstring(TEST_FOLDER).append (L"\\base004.avc"),
											"0123456789"), L"Wrong file content\n" );
		}

		{

			FileInfo _fi (L"base003.avc", L"/");
			_fi.m_localPath = L"h:\\test\\";

			_helper::DeleteDirectory(TEST_FOLDER);
			_helper::MakeDirectory(TEST_FOLDER);
			
			doEntryDownloader do_dld(&upd, hDoDownload, _fi, _fi.m_localPath, &s6c);
			
			//msg3
			QC_BOOST_CHECK_MESSAGE (hs.ReceivePureGet(), L"Error getting request\n");
			hs.SendFileFromBuffer (HttpServer::KeepAlive, HttpServer::KeepAlive, "0123456789");
			
			hs.CloseConnection ();
			
			WaitForSingleObject (hDoDownload, INFINITE);
			QC_BOOST_CHECK_MESSAGE ( CORE_NO_ERROR == do_dld.GetError(), L"Wrong return code\n" );
			QC_BOOST_CHECK_MESSAGE ( _helper::checkFileContent (wstring(TEST_FOLDER).append (L"\\base003.avc"),
											"0123456789"), L"Wrong file content\n" );
		}
	}
	

	{//variant 2 proxy-connection - close
		HttpServer hs (3128, "127.0.0.1");
		hs.Start();

		
		TestLog testLog;
		Callbacks cbacks;
		TestCallbackInterface testCallback (&testLog, &cbacks);
		TestJournalInterface testJournal;
		Signature6Checker s6c (&testLog);

		UpdaterConfiguration data(1, 1, L"1");
		data.proxy_url = L"127.0.0.1";
		data.proxy_port = 3128;
		data.m_httpAuthorizationMethods.push_back (ntlmAuthorization);
		data.m_sourceList.addSource (UST_UserURL, L"http://httpserver", true, L"diffs");
		testCallback.SetUpdaterConfiguration (data);
		KLUPD::Source _csd (L"http://httpserver", UST_UserURL, true, L"", UPS_DIRS);
		Updater upd (testCallback.m_dp, testCallback, testJournal, &testLog);
		upd.constructSourceList(testCallback.updaterConfiguration().m_sourceList, upd.m_sourceList);
		
				
		HANDLE hDoDownload;
		{

			FileInfo _fi (L"base005.avc", L"/");
			_fi.m_localPath = L"h:\\test\\";

			_helper::DeleteDirectory(TEST_FOLDER);
			_helper::MakeDirectory(TEST_FOLDER);
			
			doEntryDownloader do_dld(&upd, hDoDownload, _fi, _fi.m_localPath, &s6c);
			
			hs.Accept();
			QC_BOOST_CHECK_MESSAGE (hs.ReceivePureGet(), L"Error getting request\n");
			hs.SendNtlmGreeting ();
					
			//msg1
			QC_BOOST_CHECK_MESSAGE (hs.ReceiveGetWithNtlmToken(), L"Error getting request\n");
			//msg2
			hs.SendNtlmMsg2();
			//msg3
			QC_BOOST_CHECK_MESSAGE (hs.ReceiveGetWithNtlmToken(), L"Error getting request\n");

			hs.SendFileFromBuffer (HttpServer::KeepAlive, HttpServer::Close, "0123456789");
			hs.CloseConnection ();
			
			WaitForSingleObject (hDoDownload, INFINITE);
			QC_BOOST_CHECK_MESSAGE ( CORE_NO_ERROR == do_dld.GetError(), L"Wrong return code\n" );
			QC_BOOST_CHECK_MESSAGE ( _helper::checkFileContent (wstring(TEST_FOLDER).append (L"\\base005.avc"),
											"0123456789"), L"Wrong file content\n" );
		}

		{

			FileInfo _fi (L"base004.avc", L"/");
			_fi.m_localPath = L"h:\\test\\";

			_helper::DeleteDirectory(TEST_FOLDER);
			_helper::MakeDirectory(TEST_FOLDER);
			
			doEntryDownloader do_dld(&upd, hDoDownload, _fi, _fi.m_localPath, &s6c);
			
			hs.Accept();

			//msg1
			QC_BOOST_CHECK_MESSAGE (hs.ReceiveGetWithNtlmToken(), L"Error getting request\n");
			//msg2
			hs.SendNtlmMsg2();
			//msg3
			QC_BOOST_CHECK_MESSAGE (hs.ReceiveGetWithNtlmToken(), L"Error getting request\n");

			hs.SendFileFromBuffer (HttpServer::KeepAlive, HttpServer::Close, "0123456789");
			hs.CloseConnection ();
			
			WaitForSingleObject (hDoDownload, INFINITE);
			QC_BOOST_CHECK_MESSAGE ( CORE_NO_ERROR == do_dld.GetError(), L"Wrong return code\n" );
			QC_BOOST_CHECK_MESSAGE ( _helper::checkFileContent (wstring(TEST_FOLDER).append (L"\\base004.avc"),
											"0123456789"), L"Wrong file content\n" );
		}

		{

			FileInfo _fi (L"base003.avc", L"/");
			_fi.m_localPath = L"h:\\test\\";

			_helper::DeleteDirectory(TEST_FOLDER);
			_helper::MakeDirectory(TEST_FOLDER);
			
			doEntryDownloader do_dld(&upd, hDoDownload, _fi, _fi.m_localPath, &s6c);
			
			hs.Accept();

			//msg1
			QC_BOOST_CHECK_MESSAGE (hs.ReceiveGetWithNtlmToken(), L"Error getting request\n");
			//msg2
			hs.SendNtlmMsg2();
			//msg3
			QC_BOOST_CHECK_MESSAGE (hs.ReceiveGetWithNtlmToken(), L"Error getting request\n");

			hs.SendFileFromBuffer (HttpServer::KeepAlive, HttpServer::Close, "0123456789");
			hs.CloseConnection ();
			
			WaitForSingleObject (hDoDownload, INFINITE);
			QC_BOOST_CHECK_MESSAGE ( CORE_NO_ERROR == do_dld.GetError(), L"Wrong return code\n" );
			QC_BOOST_CHECK_MESSAGE ( _helper::checkFileContent (wstring(TEST_FOLDER).append (L"\\base003.avc"),
											"0123456789"), L"Wrong file content\n" );
		}

	}

	{//variant 3 - proxy-connection - mixed
		HttpServer hs (3128, "127.0.0.1");
		hs.Start();

		
		TestLog testLog;
		Callbacks cbacks;
		TestCallbackInterface testCallback (&testLog, &cbacks);
		TestJournalInterface testJournal;
		Signature6Checker s6c (&testLog);

		UpdaterConfiguration data(1, 1, L"1");
		data.proxy_url = L"127.0.0.1";
		data.proxy_port = 3128;
		data.m_httpAuthorizationMethods.push_back (ntlmAuthorization);
		data.m_sourceList.addSource (UST_UserURL, L"http://httpserver", true, L"diffs");
		testCallback.SetUpdaterConfiguration (data);
		KLUPD::Source _csd (L"http://httpserver", UST_UserURL, true, L"", UPS_DIRS);
		Updater upd (testCallback.m_dp, testCallback, testJournal, &testLog);
		upd.constructSourceList(testCallback.updaterConfiguration().m_sourceList, upd.m_sourceList);
		
				
		HANDLE hDoDownload;
		{

			FileInfo _fi (L"base005.avc", L"/");
			_fi.m_localPath = L"h:\\test\\";

			_helper::DeleteDirectory(TEST_FOLDER);
			_helper::MakeDirectory(TEST_FOLDER);
			
			doEntryDownloader do_dld(&upd, hDoDownload, _fi, _fi.m_localPath, &s6c);
			
			hs.Accept();
			QC_BOOST_CHECK_MESSAGE (hs.ReceivePureGet(), L"Error getting request\n");
			hs.SendNtlmGreeting ();
			
			//msg1
			QC_BOOST_CHECK_MESSAGE (hs.ReceiveGetWithNtlmToken(), L"Error getting request\n");
			//msg2
			hs.SendNtlmMsg2();
			//msg3
			QC_BOOST_CHECK_MESSAGE (hs.ReceiveGetWithNtlmToken(), L"Error getting request\n");

			hs.SendFileFromBuffer (HttpServer::KeepAlive, HttpServer::KeepAlive, "0123456789");
			
			WaitForSingleObject (hDoDownload, INFINITE);
			QC_BOOST_CHECK_MESSAGE ( CORE_NO_ERROR == do_dld.GetError(), L"Wrong return code\n" );
			QC_BOOST_CHECK_MESSAGE ( _helper::checkFileContent (wstring(TEST_FOLDER).append (L"\\base005.avc"),
											"0123456789"), L"Wrong file content\n" );
		}

		{

			FileInfo _fi (L"base004.avc", L"/");
			_fi.m_localPath = L"h:\\test\\";

			_helper::DeleteDirectory(TEST_FOLDER);
			_helper::MakeDirectory(TEST_FOLDER);
			
			doEntryDownloader do_dld(&upd, hDoDownload, _fi, _fi.m_localPath, &s6c);
			
			QC_BOOST_CHECK_MESSAGE (hs.ReceivePureGet(), L"Error getting request\n");

			hs.SendFileFromBuffer (HttpServer::KeepAlive, HttpServer::Close, "0123456789");
			hs.CloseConnection ();
			
			WaitForSingleObject (hDoDownload, INFINITE);
			QC_BOOST_CHECK_MESSAGE ( CORE_NO_ERROR == do_dld.GetError(), L"Wrong return code\n" );
			QC_BOOST_CHECK_MESSAGE ( _helper::checkFileContent (wstring(TEST_FOLDER).append (L"\\base004.avc"),
											"0123456789"), L"Wrong file content\n" );
		}

		{

			FileInfo _fi (L"base003.avc", L"/");
			_fi.m_localPath = L"h:\\test\\";

			_helper::DeleteDirectory(TEST_FOLDER);
			_helper::MakeDirectory(TEST_FOLDER);
			
			doEntryDownloader do_dld(&upd, hDoDownload, _fi, _fi.m_localPath, &s6c);
			
			hs.Accept();

			//msg1
			QC_BOOST_CHECK_MESSAGE (hs.ReceiveGetWithNtlmToken(), L"Error getting request\n");
			//msg2
			hs.SendNtlmMsg2();
			//msg3
			QC_BOOST_CHECK_MESSAGE (hs.ReceiveGetWithNtlmToken(), L"Error getting request\n");

			hs.SendFileFromBuffer (HttpServer::KeepAlive, HttpServer::KeepAlive, "0123456789");

			WaitForSingleObject (hDoDownload, INFINITE);
			QC_BOOST_CHECK_MESSAGE ( CORE_NO_ERROR == do_dld.GetError(), L"Wrong return code\n" );
			QC_BOOST_CHECK_MESSAGE ( _helper::checkFileContent (wstring(TEST_FOLDER).append (L"\\base003.avc"),
											"0123456789"), L"Wrong file content\n" );
		}

		{

			FileInfo _fi (L"base002.avc", L"/");
			_fi.m_localPath = L"h:\\test\\";

			_helper::DeleteDirectory(TEST_FOLDER);
			_helper::MakeDirectory(TEST_FOLDER);

			doEntryDownloader do_dld(&upd, hDoDownload, _fi, _fi.m_localPath, &s6c);

			QC_BOOST_CHECK_MESSAGE (hs.ReceivePureGet(), L"Error getting request\n");

			hs.SendFileFromBuffer (HttpServer::KeepAlive, HttpServer::Close, "a0123456789");
			hs.CloseConnection ();

			WaitForSingleObject (hDoDownload, INFINITE);
			QC_BOOST_CHECK_MESSAGE ( CORE_NO_ERROR == do_dld.GetError(), L"Wrong return code\n" );
			QC_BOOST_CHECK_MESSAGE ( _helper::checkFileContent (wstring(TEST_FOLDER).append (L"\\base002.avc"),
				"a0123456789"), L"Wrong file content\n" );
		}

	}

}

void TestHttpProxy::TestConnectionClosingTimeout ()
{
	using namespace KLUPD;
	std::wcout<<L"TestConnectionClosingTimeout\n";
	Logger::Case _case (m_logger, m_testp, m_step, L"TestConnectionClosingTimeout");
//add other authorization types

	{//While using basic auth
	//Closing idle connection
		HttpServer hs (3128, "127.0.0.1");
		hs.Start();

		
		TestLog testLog;
		Callbacks cbacks;
		TestCallbackInterface testCallback (&testLog, &cbacks);
		TestJournalInterface testJournal;
		Signature6Checker s6c (&testLog);

		UpdaterConfiguration data(1, 1, L"1");
		data.proxy_url = L"127.0.0.1";
		data.proxy_port = 3128;
		data.m_proxyAuthorizationCredentials.userName ( L"test");
		data.m_proxyAuthorizationCredentials.password ( L"test");
		data.m_httpAuthorizationMethods.push_back (basicAuthorization);
		data.m_sourceList.addSource (UST_UserURL, L"http://httpserver", true, L"diffs");
		testCallback.SetUpdaterConfiguration (data);
		KLUPD::Source _csd (L"http://httpserver", UST_UserURL, true, L"", UPS_DIRS);
		Updater upd (testCallback.m_dp, testCallback, testJournal, &testLog);
		upd.constructSourceList(testCallback.updaterConfiguration().m_sourceList, upd.m_sourceList);
		
				
		HANDLE hDoDownload;

		{
			FileInfo _fi (L"base005.avc", L"/");
			_fi.m_localPath = L"h:\\test\\";

			_helper::DeleteDirectory(TEST_FOLDER);
			_helper::MakeDirectory(TEST_FOLDER);
			
			doEntryDownloader do_dld(&upd, hDoDownload, _fi, _fi.m_localPath, &s6c);
			
			hs.Accept();
			QC_BOOST_CHECK_MESSAGE (hs.ReceivePureGet(), L"Error getting request\n");
			hs.SendBasicGreeting (HttpServer::Close);	
			hs.CloseConnection ();

			hs.Accept();
			QC_BOOST_CHECK_MESSAGE (hs.ReceiveGetWithBasicToken(), L"Error getting request\n");
			hs.SendFileFromBuffer (HttpServer::KeepAlive, HttpServer::KeepAlive, "0123456789");
			
			WaitForSingleObject (hDoDownload, INFINITE);
			QC_BOOST_CHECK_MESSAGE ( CORE_NO_ERROR == do_dld.GetError(), L"Wrong return code\n" );
			QC_BOOST_CHECK_MESSAGE ( _helper::checkFileContent (wstring(TEST_FOLDER).append (L"\\base005.avc"),
											"0123456789"), L"Wrong file content\n" );
		}

		{

			FileInfo _fi (L"base004.avc", L"/");
			_fi.m_localPath = L"h:\\test\\";

			_helper::DeleteDirectory(TEST_FOLDER);
			_helper::MakeDirectory(TEST_FOLDER);
			
			doEntryDownloader do_dld(&upd, hDoDownload, _fi, _fi.m_localPath, &s6c);

			QC_BOOST_CHECK_MESSAGE (hs.ReceiveGetWithBasicToken(), L"Error getting request\n");
			hs.SendFileFromBuffer (HttpServer::KeepAlive, HttpServer::KeepAlive, "0123456789");
						
			WaitForSingleObject (hDoDownload, INFINITE);
			QC_BOOST_CHECK_MESSAGE ( CORE_NO_ERROR == do_dld.GetError(), L"Wrong return code\n" );
			QC_BOOST_CHECK_MESSAGE ( _helper::checkFileContent (wstring(TEST_FOLDER).append (L"\\base004.avc"),
											"0123456789"), L"Wrong file content\n" );

			//timeout occured
			std::wcout<<L"timeout occured\n";
			hs.CloseConnection();
			
		}

		{

			FileInfo _fi (L"base003.avc", L"/");
			_fi.m_localPath = L"h:\\test\\";

			_helper::DeleteDirectory(TEST_FOLDER);
			_helper::MakeDirectory(TEST_FOLDER);
			
			doEntryDownloader do_dld(&upd, hDoDownload, _fi, _fi.m_localPath, &s6c);
		
			//need to reestablish connection
			std::wcout<<L"need to reestablish connection\n";

			hs.Accept();
			QC_BOOST_CHECK_MESSAGE (hs.ReceiveGetWithBasicToken(), L"Error getting request\n");
			hs.SendFileFromBuffer (HttpServer::KeepAlive, HttpServer::Close, "0123456789");
			hs.CloseConnection ();
			
			WaitForSingleObject (hDoDownload, INFINITE);
			QC_BOOST_CHECK_MESSAGE ( CORE_NO_ERROR == do_dld.GetError(), L"Wrong return code\n" );
			QC_BOOST_CHECK_MESSAGE ( _helper::checkFileContent (wstring(TEST_FOLDER).append (L"\\base003.avc"),
											"0123456789"), L"Wrong file content\n" );
		}	
	}

	//Closing non-idle connection
	{//While using basic auth
		HttpServer hs (3128, "127.0.0.1");
		hs.Start();


		TestLog testLog;
		Callbacks cbacks;
		TestCallbackInterface testCallback (&testLog, &cbacks);
		TestJournalInterface testJournal;
		Signature6Checker s6c (&testLog);

		UpdaterConfiguration data(1, 1, L"1");
		data.proxy_url = L"127.0.0.1";
		data.proxy_port = 3128;
		data.m_proxyAuthorizationCredentials.userName ( L"test");
		data.m_proxyAuthorizationCredentials.password ( L"test");
		data.m_httpAuthorizationMethods.push_back (basicAuthorization);
		data.m_sourceList.addSource (UST_UserURL, L"http://httpserver", true, L"diffs");
		testCallback.SetUpdaterConfiguration (data);
		KLUPD::Source _csd (L"http://httpserver", UST_UserURL, true, L"", UPS_DIRS);
		Updater upd (testCallback.m_dp, testCallback, testJournal, &testLog);
		upd.constructSourceList(testCallback.updaterConfiguration().m_sourceList, upd.m_sourceList);


		HANDLE hDoDownload;

		string command;
		string param;

		{

			FileInfo _fi (L"base005.avc", L"/");
			_fi.m_localPath = L"h:\\test\\";

			_helper::DeleteDirectory(TEST_FOLDER);
			_helper::MakeDirectory(TEST_FOLDER);

			doEntryDownloader do_dld(&upd, hDoDownload, _fi, _fi.m_localPath, &s6c);

			hs.Accept();
			QC_BOOST_CHECK_MESSAGE (hs.ReceivePureGet(), L"Error getting request\n");
			hs.SendBasicGreeting (HttpServer::Close);	
			hs.CloseConnection ();

			hs.Accept();
			QC_BOOST_CHECK_MESSAGE (hs.ReceiveGetWithBasicToken(), L"Error getting request\n");
			hs.SendFileFromBuffer (HttpServer::KeepAlive, HttpServer::KeepAlive, "0123456789");

			WaitForSingleObject (hDoDownload, INFINITE);
			QC_BOOST_CHECK_MESSAGE ( CORE_NO_ERROR == do_dld.GetError(), L"Wrong return code\n" );
			QC_BOOST_CHECK_MESSAGE ( _helper::checkFileContent (wstring(TEST_FOLDER).append (L"\\base005.avc"),
				"0123456789"), L"Wrong file content\n" );
		}

		{

			FileInfo _fi (L"base004.avc", L"/");
			_fi.m_localPath = L"h:\\test\\";

			_helper::DeleteDirectory(TEST_FOLDER);
			_helper::MakeDirectory(TEST_FOLDER);

			doEntryDownloader do_dld(&upd, hDoDownload, _fi, _fi.m_localPath, &s6c);

			QC_BOOST_CHECK_MESSAGE (hs.ReceiveGetWithBasicToken(), L"Error getting request\n");
			hs.SendFileFromBuffer (HttpServer::KeepAlive, HttpServer::KeepAlive, "0123456789");

			WaitForSingleObject (hDoDownload, INFINITE);
			QC_BOOST_CHECK_MESSAGE ( CORE_NO_ERROR == do_dld.GetError(), L"Wrong return code\n" );
			QC_BOOST_CHECK_MESSAGE ( _helper::checkFileContent (wstring(TEST_FOLDER).append (L"\\base004.avc"),
				"0123456789"), L"Wrong file content\n" );

			//timeout occured
			std::wcout<<L"timeout occured\n";
			hs.CloseConnection();

		}

		{

			FileInfo _fi (L"base003.avc", L"/");
			_fi.m_localPath = L"h:\\test\\";

			_helper::DeleteDirectory(TEST_FOLDER);
			_helper::MakeDirectory(TEST_FOLDER);

			doEntryDownloader do_dld(&upd, hDoDownload, _fi, _fi.m_localPath, &s6c);

			//need to reestablish connection
			std::wcout<<L"need to reestablish connection\n";

			hs.Accept();
			QC_BOOST_CHECK_MESSAGE (hs.ReceiveGetWithBasicToken(), L"Error getting request\n");
			
			//timeout occured again (in fact, it wasn't disconnect by timeout)
			std::wcout<<L"timeout occured\n";
			hs.CloseConnection();
			
			WaitForSingleObject (hDoDownload, INFINITE);
			QC_BOOST_CHECK_MESSAGE ( CORE_REMOTE_HOST_CLOSED_CONNECTION == do_dld.GetError(), L"Wrong return code\n" );
			QC_BOOST_CHECK_MESSAGE ( !_helper::AutoFile (wstring(TEST_FOLDER).append (L"\\base003.avc")).Exist(),
				L"Wrong file content\n" );
		}	
	}
	
	{//While using ntlm auth
	 //Closing idle connection
		HttpServer hs (3128, "127.0.0.1");
		hs.Start();


		TestLog testLog;
		Callbacks cbacks;
		TestCallbackInterface testCallback (&testLog, &cbacks);
		TestJournalInterface testJournal;
		Signature6Checker s6c (&testLog);

		UpdaterConfiguration data(1, 1, L"1");
		data.proxy_url = L"127.0.0.1";
		data.proxy_port = 3128;
		data.m_httpAuthorizationMethods.push_back (ntlmAuthorization);
		data.m_sourceList.addSource (UST_UserURL, L"http://httpserver", true, L"diffs");
		testCallback.SetUpdaterConfiguration (data);
		KLUPD::Source _csd (L"http://httpserver", UST_UserURL, true, L"", UPS_DIRS);
		Updater upd (testCallback.m_dp, testCallback, testJournal, &testLog);
		upd.constructSourceList(testCallback.updaterConfiguration().m_sourceList, upd.m_sourceList);


		HANDLE hDoDownload;

		{
			FileInfo _fi (L"base005.avc", L"/");
			_fi.m_localPath = L"h:\\test\\";

			_helper::DeleteDirectory(TEST_FOLDER);
			_helper::MakeDirectory(TEST_FOLDER);

			doEntryDownloader do_dld(&upd, hDoDownload, _fi, _fi.m_localPath, &s6c);

			hs.Accept();
			QC_BOOST_CHECK_MESSAGE (hs.ReceivePureGet(), L"Error getting request\n");
			hs.SendNtlmGreeting (HttpServer::Close);	
			hs.CloseConnection ();

			hs.Accept();
			QC_BOOST_CHECK_MESSAGE (hs.ReceiveGetWithNtlmToken(), L"Error getting request\n");
			hs.SendNtlmMsg2();
			QC_BOOST_CHECK_MESSAGE (hs.ReceiveGetWithNtlmToken(), L"Error getting request\n");
			hs.SendFileFromBuffer (HttpServer::KeepAlive, HttpServer::KeepAlive, "0123456789");

			WaitForSingleObject (hDoDownload, INFINITE);
			QC_BOOST_CHECK_MESSAGE ( CORE_NO_ERROR == do_dld.GetError(), L"Wrong return code\n" );
			QC_BOOST_CHECK_MESSAGE ( _helper::checkFileContent (wstring(TEST_FOLDER).append (L"\\base005.avc"),
				"0123456789"), L"Wrong file content\n" );
		}

		{

			FileInfo _fi (L"base004.avc", L"/");
			_fi.m_localPath = L"h:\\test\\";

			_helper::DeleteDirectory(TEST_FOLDER);
			_helper::MakeDirectory(TEST_FOLDER);

			doEntryDownloader do_dld(&upd, hDoDownload, _fi, _fi.m_localPath, &s6c);

			QC_BOOST_CHECK_MESSAGE (hs.ReceivePureGet(), L"Error getting request\n");
			hs.SendFileFromBuffer (HttpServer::KeepAlive, HttpServer::KeepAlive, "0123456789");

			WaitForSingleObject (hDoDownload, INFINITE);
			QC_BOOST_CHECK_MESSAGE ( CORE_NO_ERROR == do_dld.GetError(), L"Wrong return code\n" );
			QC_BOOST_CHECK_MESSAGE ( _helper::checkFileContent (wstring(TEST_FOLDER).append (L"\\base004.avc"),
				"0123456789"), L"Wrong file content\n" );

			//timeout occured
			std::wcout<<L"timeout occured\n";
			hs.CloseConnection();

		}

		{

			FileInfo _fi (L"base003.avc", L"/");
			_fi.m_localPath = L"h:\\test\\";

			_helper::DeleteDirectory(TEST_FOLDER);
			_helper::MakeDirectory(TEST_FOLDER);

			doEntryDownloader do_dld(&upd, hDoDownload, _fi, _fi.m_localPath, &s6c);

			//need to reestablish connection
			std::wcout<<L"need to reestablish connection\n";

			hs.Accept();
			QC_BOOST_CHECK_MESSAGE (hs.ReceiveGetWithNtlmToken(), L"Error getting request\n");
			hs.SendNtlmMsg2();
			QC_BOOST_CHECK_MESSAGE (hs.ReceiveGetWithNtlmToken(), L"Error getting request\n");
			hs.SendFileFromBuffer (HttpServer::KeepAlive, HttpServer::Close, "0123456789");
			hs.CloseConnection ();

			WaitForSingleObject (hDoDownload, INFINITE);
			QC_BOOST_CHECK_MESSAGE ( CORE_NO_ERROR == do_dld.GetError(), L"Wrong return code\n" );
			QC_BOOST_CHECK_MESSAGE ( _helper::checkFileContent (wstring(TEST_FOLDER).append (L"\\base003.avc"),
				"0123456789"), L"Wrong file content\n" );
		}	
	}

	//Closing non-idle connection
	{//While using ntlm auth
		HttpServer hs (3128, "127.0.0.1");
		hs.Start();


		TestLog testLog;
		Callbacks cbacks;
		TestCallbackInterface testCallback (&testLog, &cbacks);
		TestJournalInterface testJournal;
		Signature6Checker s6c (&testLog);

		UpdaterConfiguration data(1, 1, L"1");
		data.proxy_url = L"127.0.0.1";
		data.proxy_port = 3128;
		data.m_httpAuthorizationMethods.push_back (ntlmAuthorization);
		data.m_sourceList.addSource (UST_UserURL, L"http://httpserver", true, L"diffs");
		testCallback.SetUpdaterConfiguration (data);
		KLUPD::Source _csd (L"http://httpserver", UST_UserURL, true, L"", UPS_DIRS);
		Updater upd (testCallback.m_dp, testCallback, testJournal, &testLog);
		upd.constructSourceList(testCallback.updaterConfiguration().m_sourceList, upd.m_sourceList);

		HANDLE hDoDownload;
		{

			FileInfo _fi (L"base005.avc", L"/");
			_fi.m_localPath = L"h:\\test\\";

			_helper::DeleteDirectory(TEST_FOLDER);
			_helper::MakeDirectory(TEST_FOLDER);

			doEntryDownloader do_dld(&upd, hDoDownload, _fi, _fi.m_localPath, &s6c);

			hs.Accept();
			QC_BOOST_CHECK_MESSAGE (hs.ReceivePureGet(), L"Error getting request\n");
			hs.SendNtlmGreeting (HttpServer::Close);	
			hs.CloseConnection ();

			hs.Accept();
			QC_BOOST_CHECK_MESSAGE (hs.ReceiveGetWithNtlmToken(), L"Error getting request\n");
			hs.SendNtlmMsg2();
			QC_BOOST_CHECK_MESSAGE (hs.ReceiveGetWithNtlmToken(), L"Error getting request\n");
			hs.SendFileFromBuffer (HttpServer::KeepAlive, HttpServer::KeepAlive, "0123456789");

			WaitForSingleObject (hDoDownload, INFINITE);
			QC_BOOST_CHECK_MESSAGE ( CORE_NO_ERROR == do_dld.GetError(), L"Wrong return code\n" );
			QC_BOOST_CHECK_MESSAGE ( _helper::checkFileContent (wstring(TEST_FOLDER).append (L"\\base005.avc"),
				"0123456789"), L"Wrong file content\n" );
		}

		{

			FileInfo _fi (L"base004.avc", L"/");
			_fi.m_localPath = L"h:\\test\\";

			_helper::DeleteDirectory(TEST_FOLDER);
			_helper::MakeDirectory(TEST_FOLDER);

			doEntryDownloader do_dld(&upd, hDoDownload, _fi, _fi.m_localPath, &s6c);

			QC_BOOST_CHECK_MESSAGE (hs.ReceivePureGet(), L"Error getting request\n");
			hs.SendFileFromBuffer (HttpServer::KeepAlive, HttpServer::KeepAlive, "0123456789");

			WaitForSingleObject (hDoDownload, INFINITE);
			QC_BOOST_CHECK_MESSAGE ( CORE_NO_ERROR == do_dld.GetError(), L"Wrong return code\n" );
			QC_BOOST_CHECK_MESSAGE ( _helper::checkFileContent (wstring(TEST_FOLDER).append (L"\\base004.avc"),
				"0123456789"), L"Wrong file content\n" );

			//timeout occured
			std::wcout<<L"timeout occured\n";
			hs.CloseConnection();

		}

		{

			FileInfo _fi (L"base003.avc", L"/");
			_fi.m_localPath = L"h:\\test\\";

			_helper::DeleteDirectory(TEST_FOLDER);
			_helper::MakeDirectory(TEST_FOLDER);

			doEntryDownloader do_dld(&upd, hDoDownload, _fi, _fi.m_localPath, &s6c);

			//need to reestablish connection
			std::wcout<<L"need to reestablish connection\n";

			hs.Accept();
			QC_BOOST_CHECK_MESSAGE (hs.ReceiveGetWithNtlmToken(), L"Error getting request\n");
			hs.SendNtlmMsg2();
			QC_BOOST_CHECK_MESSAGE (hs.ReceiveGetWithNtlmToken(), L"Error getting request\n");

			//timeout occured again (in fact, it wasn't disconnect by timeout)
			std::wcout<<L"timeout occured\n";
			hs.CloseConnection();

			WaitForSingleObject (hDoDownload, INFINITE);
			QC_BOOST_CHECK_MESSAGE ( CORE_REMOTE_HOST_CLOSED_CONNECTION == do_dld.GetError(), L"Wrong return code\n" );
			QC_BOOST_CHECK_MESSAGE ( !_helper::AutoFile (wstring(TEST_FOLDER).append (L"\\base003.avc")).Exist(),
				L"Wrong file content\n" );
		}	
	}
	

	{//While using basic & ntlm auth
	 //Closing idle connection
		HttpServer hs (3128, "127.0.0.1");
		hs.Start();


		TestLog testLog;
		Callbacks cbacks;
		TestCallbackInterface testCallback (&testLog, &cbacks);
		TestJournalInterface testJournal;
		Signature6Checker s6c (&testLog);

		UpdaterConfiguration data(1, 1, L"1");
		data.proxy_url = L"127.0.0.1";
		data.proxy_port = 3128;
		data.m_proxyAuthorizationCredentials.userName ( L"test");
		data.m_proxyAuthorizationCredentials.password ( L"test");
		data.m_httpAuthorizationMethods.push_back (ntlmAuthorization);
		data.m_httpAuthorizationMethods.push_back (basicAuthorization);
		data.m_sourceList.addSource (UST_UserURL, L"http://httpserver", true, L"diffs");
		testCallback.SetUpdaterConfiguration (data);
		KLUPD::Source _csd (L"http://httpserver", UST_UserURL, true, L"", UPS_DIRS);
		Updater upd (testCallback.m_dp, testCallback, testJournal, &testLog);
		upd.constructSourceList(testCallback.updaterConfiguration().m_sourceList, upd.m_sourceList);


		HANDLE hDoDownload;

		{
			FileInfo _fi (L"base005.avc", L"/");
			_fi.m_localPath = L"h:\\test\\";

			_helper::DeleteDirectory(TEST_FOLDER);
			_helper::MakeDirectory(TEST_FOLDER);

			doEntryDownloader do_dld(&upd, hDoDownload, _fi, _fi.m_localPath, &s6c);

			hs.Accept();
			QC_BOOST_CHECK_MESSAGE (hs.ReceivePureGet(), L"Error getting request\n");
			hs.SendNtlmBasicGreeting (HttpServer::Close);	
			hs.CloseConnection ();

			hs.Accept();
			QC_BOOST_CHECK_MESSAGE (hs.ReceiveGetWithNtlmToken(), L"Error getting request\n");
			hs.SendNtlmMsg2();
			QC_BOOST_CHECK_MESSAGE (hs.ReceiveGetWithNtlmToken(), L"Error getting request\n");


			hs.SendFileFromBuffer (HttpServer::KeepAlive, HttpServer::KeepAlive, "0123456789");

			WaitForSingleObject (hDoDownload, INFINITE);
			QC_BOOST_CHECK_MESSAGE ( CORE_NO_ERROR == do_dld.GetError(), L"Wrong return code\n" );
			QC_BOOST_CHECK_MESSAGE ( _helper::checkFileContent (wstring(TEST_FOLDER).append (L"\\base005.avc"),
				"0123456789"), L"Wrong file content\n" );
		}

		{

			FileInfo _fi (L"base004.avc", L"/");
			_fi.m_localPath = L"h:\\test\\";

			_helper::DeleteDirectory(TEST_FOLDER);
			_helper::MakeDirectory(TEST_FOLDER);

			doEntryDownloader do_dld(&upd, hDoDownload, _fi, _fi.m_localPath, &s6c);

			QC_BOOST_CHECK_MESSAGE (hs.ReceivePureGet(), L"Error getting request\n");

			hs.SendFileFromBuffer (HttpServer::KeepAlive, HttpServer::KeepAlive, "0123456789");

			WaitForSingleObject (hDoDownload, INFINITE);
			QC_BOOST_CHECK_MESSAGE ( CORE_NO_ERROR == do_dld.GetError(), L"Wrong return code\n" );
			QC_BOOST_CHECK_MESSAGE ( _helper::checkFileContent (wstring(TEST_FOLDER).append (L"\\base004.avc"),
				"0123456789"), L"Wrong file content\n" );

			//timeout occured
			std::wcout<<L"timeout occured\n";
			hs.CloseConnection();

		}

		{

			FileInfo _fi (L"base003.avc", L"/");
			_fi.m_localPath = L"h:\\test\\";

			_helper::DeleteDirectory(TEST_FOLDER);
			_helper::MakeDirectory(TEST_FOLDER);

			doEntryDownloader do_dld(&upd, hDoDownload, _fi, _fi.m_localPath, &s6c);

			//need to reestablish connection
			std::wcout<<L"need to reestablish connection\n";

			hs.Accept();
			QC_BOOST_CHECK_MESSAGE (hs.ReceiveGetWithNtlmToken(), L"Error getting request\n");
			hs.SendNtlmMsg2();
			QC_BOOST_CHECK_MESSAGE (hs.ReceiveGetWithNtlmToken(), L"Error getting request\n");

			hs.SendFileFromBuffer (HttpServer::KeepAlive, HttpServer::KeepAlive, "0123456789");

			WaitForSingleObject (hDoDownload, INFINITE);
			QC_BOOST_CHECK_MESSAGE ( CORE_NO_ERROR == do_dld.GetError(), L"Wrong return code\n" );
			QC_BOOST_CHECK_MESSAGE ( _helper::checkFileContent (wstring(TEST_FOLDER).append (L"\\base003.avc"),
				"0123456789"), L"Wrong file content\n" );

			//timeout occured
			std::wcout<<L"timeout occured\n";
			hs.CloseConnection();
		}
		// auth type is the same
		{

			FileInfo _fi (L"base003.avc", L"/");
			_fi.m_localPath = L"h:\\test\\";

			_helper::DeleteDirectory(TEST_FOLDER);
			_helper::MakeDirectory(TEST_FOLDER);

			doEntryDownloader do_dld(&upd, hDoDownload, _fi, _fi.m_localPath, &s6c);

			//need to reestablish connection
			std::wcout<<L"need to reestablish connection\n";

			hs.Accept();
			QC_BOOST_CHECK_MESSAGE (hs.ReceiveGetWithNtlmToken(), L"Error getting request\n");
			hs.SendNtlmMsg2();
			QC_BOOST_CHECK_MESSAGE (hs.ReceiveGetWithNtlmToken(), L"Error getting request\n");


			//timeout occured again (in fact, it wasn't disconnect by timeout)
			std::wcout<<L"timeout occured\n";
			hs.CloseConnection();

			WaitForSingleObject (hDoDownload, INFINITE);
			QC_BOOST_CHECK_MESSAGE ( CORE_REMOTE_HOST_CLOSED_CONNECTION == do_dld.GetError(), L"Wrong return code\n" );
			QC_BOOST_CHECK_MESSAGE ( !_helper::AutoFile (wstring(TEST_FOLDER).append (L"\\base003.avc")).Exist(),
				L"Wrong file content\n" );

		}
	}
}

void TestHttpProxy::TestFtpOverHttp ()
{
	using namespace KLUPD;
	std::wcout<<L"TestFtpOverHttp\n";
	Logger::Case _case (m_logger, m_testp, m_step, L"TestFtpOverHttp");

	{//proxy - no auth
		HttpServer hs (3128, "127.0.0.1");
		hs.Start();
		
		TestLog testLog;
		Callbacks cbacks;
		TestCallbackInterface testCallback (&testLog, &cbacks);
		TestJournalInterface testJournal;
		Signature6Checker s6c (&testLog);

		UpdaterConfiguration data(1, 1, L"1");
		data.proxy_url = L"127.0.0.1";
		data.proxy_port = 3128;
		data.m_proxyAuthorizationCredentials.userName ( L"test");
		data.m_proxyAuthorizationCredentials.password ( L"test");
		data.m_httpAuthorizationMethods.push_back (basicAuthorization);
		data.m_sourceList.addSource (UST_UserURL, L"ftp://user:pass@ftphost:3333/root/", true, L"diffs");
		testCallback.SetUpdaterConfiguration (data);
		Updater upd (testCallback.m_dp, testCallback, testJournal, &testLog);
		upd.constructSourceList(testCallback.updaterConfiguration().m_sourceList, upd.m_sourceList);
				
		HANDLE hDoDownload;

		{

			FileInfo _fi (L"base005.avc", L"/");
			_fi.m_localPath = L"h:\\test\\";

			_helper::DeleteDirectory(TEST_FOLDER);
			_helper::MakeDirectory(TEST_FOLDER);
			
			doEntryDownloader do_dld(&upd, hDoDownload, _fi, _fi.m_localPath, &s6c);

			hs.Accept();
			QC_BOOST_CHECK_MESSAGE (hs.ReceivePureGet("get ftp://user:pass@ftphost:3333/root/base005.avc"), L"Error getting request\n");
			hs.SendFileFromBuffer (HttpServer::KeepAlive, HttpServer::Close, "0123456789");
			hs.CloseConnection ();
			
			WaitForSingleObject (hDoDownload, INFINITE);
			QC_BOOST_CHECK_MESSAGE ( CORE_NO_ERROR == do_dld.GetError(), L"Wrong return code\n" );
			QC_BOOST_CHECK_MESSAGE ( _helper::checkFileContent (wstring(TEST_FOLDER).append (L"\\base005.avc"),
											"0123456789"), L"Wrong file content\n" );
		}	
	}

}

void TestHttpProxy::TestReusingServersThroughProxy ()
{
	using namespace KLUPD;
	std::wcout<<L"TestReusingServersThroughProxy\n";
	Logger::Case _case (m_logger, m_testp, m_step, L"TestReusingServersThroughProxy");

	{//keep-alive
		HttpServer hs (3128, "127.0.0.1");
		hs.Start();
	
		TestLog testLog;
		Callbacks cbacks;
		TestCallbackInterface testCallback (&testLog, &cbacks);
		TestJournalInterface testJournal;
		Signature6Checker s6c (&testLog);

		UpdaterConfiguration data(1, 1, L"1");
		data.proxy_url = L"127.0.0.1";
		data.proxy_port = 3128;
		data.m_httpAuthorizationMethods.push_back (ntlmAuthorization);
		data.m_sourceList.addSource (UST_UserURL, L"http://httpserver", true, L"diffs");
		testCallback.SetUpdaterConfiguration (data);

		Updater upd (testCallback.m_dp, testCallback, testJournal, &testLog);
		upd.constructSourceList(testCallback.updaterConfiguration().m_sourceList, upd.m_sourceList);

		HANDLE hDoDownload;

		FileInfo _fi (L"base005.avc", L"/");
		_fi.m_localPath = L"h:\\test\\";

		FileInfo _fi2 (L"base004.avc", L"/");
		_fi2.m_localPath = L"h:\\test\\";

		for (int i = 0; i < 1000; ++i)
		{		

			_helper::DeleteDirectory(TEST_FOLDER);
			_helper::MakeDirectory(TEST_FOLDER);
			
			doEntryDownloader do_dld(&upd, hDoDownload, _fi, _fi.m_localPath, &s6c);		

			if (i==0) 
			{
				//initing connection
				hs.Accept();
				QC_BOOST_CHECK_MESSAGE (hs.ReceivePureGet(), L"Error getting request\n");
				hs.SendNtlmGreeting ();
				
				QC_BOOST_CHECK_MESSAGE (hs.ReceiveGetWithNtlmToken(), L"Error getting request\n");
				hs.SendNtlmMsg2();
				QC_BOOST_CHECK_MESSAGE (hs.ReceiveGetWithNtlmToken(), L"Error getting request\n");

			}
			else
			{
				QC_BOOST_CHECK_MESSAGE (hs.ReceivePureGet(), L"Error getting request\n");
			}

			hs.SendFileFromBuffer (HttpServer::KeepAlive, HttpServer::KeepAlive, "0123456789");
	
			WaitForSingleObject (hDoDownload, INFINITE);
			QC_BOOST_CHECK_MESSAGE ( CORE_NO_ERROR == do_dld.GetError(), L"Wrong return code\n" );
			QC_BOOST_CHECK_MESSAGE ( _helper::checkFileContent (wstring(TEST_FOLDER).append (L"\\base005.avc"),
											"0123456789"), L"Wrong file content\n" );

			doEntryDownloader do_dld2(&upd, hDoDownload, _fi2, _fi2.m_localPath, &s6c);		

			QC_BOOST_CHECK_MESSAGE (hs.ReceivePureGet(), L"Error getting request\n");
			hs.SendFileFromBuffer (HttpServer::KeepAlive, HttpServer::KeepAlive, "9876543210");

			WaitForSingleObject (hDoDownload, INFINITE);
			QC_BOOST_CHECK_MESSAGE ( CORE_NO_ERROR == do_dld.GetError(), L"Wrong return code\n" );
			QC_BOOST_CHECK_MESSAGE ( _helper::checkFileContent (wstring(TEST_FOLDER).append (L"\\base004.avc"),
											"9876543210"), L"Wrong file content\n" );
		}
		
		hs.CloseConnection();
		
	}

	{//connection: close
		HttpServer hs (3128, "127.0.0.1");
		hs.Start();
		
		TestLog testLog;
		Callbacks cbacks;
		TestCallbackInterface testCallback (&testLog, &cbacks);
		TestJournalInterface testJournal;
		Signature6Checker s6c (&testLog);

		UpdaterConfiguration data(1, 1, L"1");
		data.proxy_url = L"127.0.0.1";
		data.proxy_port = 3128;
		data.m_httpAuthorizationMethods.push_back (ntlmAuthorization);
		data.m_sourceList.addSource (UST_UserURL, L"http://httpserver", true, L"diffs");
		testCallback.SetUpdaterConfiguration (data);

		Updater upd (testCallback.m_dp, testCallback, testJournal, &testLog);
		upd.constructSourceList(testCallback.updaterConfiguration().m_sourceList, upd.m_sourceList);

		HANDLE hDoDownload;

		FileInfo _fi (L"base005.avc", L"/");
		_fi.m_localPath = L"h:\\test\\";

		FileInfo _fi2 (L"base004.avc", L"/");
		_fi2.m_localPath = L"h:\\test\\";

		for (int i = 0; i < 1000; ++i)
		{		

			_helper::DeleteDirectory(TEST_FOLDER);
			_helper::MakeDirectory(TEST_FOLDER);
			
			doEntryDownloader do_dld(&upd, hDoDownload, _fi, _fi.m_localPath, &s6c);		

			if (i==0) 
			{
				//initing connection
				hs.Accept();
				QC_BOOST_CHECK_MESSAGE (hs.ReceivePureGet(), L"Error getting request\n");
				hs.SendNtlmGreeting (HttpServer::Close);
				hs.CheckConnectionClosed();
			}
				
			hs.Accept();
			QC_BOOST_CHECK_MESSAGE (hs.ReceiveGetWithNtlmToken(), L"Error getting request\n");
			hs.SendNtlmMsg2();
			QC_BOOST_CHECK_MESSAGE (hs.ReceiveGetWithNtlmToken(), L"Error getting request\n");
			hs.SendFileFromBuffer (HttpServer::KeepAlive, HttpServer::Close, "0123456789");
			hs.CloseConnection ();
			
			WaitForSingleObject (hDoDownload, INFINITE);
			QC_BOOST_CHECK_MESSAGE ( CORE_NO_ERROR == do_dld.GetError(), L"Wrong return code\n" );
			QC_BOOST_CHECK_MESSAGE ( _helper::checkFileContent (wstring(TEST_FOLDER).append (L"\\base005.avc"),
											"0123456789"), L"Wrong file content\n" );

			doEntryDownloader do_dld2(&upd, hDoDownload, _fi2, _fi2.m_localPath, &s6c);		

			hs.Accept();
			QC_BOOST_CHECK_MESSAGE (hs.ReceiveGetWithNtlmToken(), L"Error getting request\n");
			hs.SendNtlmMsg2();
			QC_BOOST_CHECK_MESSAGE (hs.ReceiveGetWithNtlmToken(), L"Error getting request\n");
			hs.SendFileFromBuffer (HttpServer::KeepAlive, HttpServer::Close, "9876543210");
			hs.CloseConnection ();


			WaitForSingleObject (hDoDownload, INFINITE);
			QC_BOOST_CHECK_MESSAGE ( CORE_NO_ERROR == do_dld.GetError(), L"Wrong return code\n" );
			QC_BOOST_CHECK_MESSAGE ( _helper::checkFileContent (wstring(TEST_FOLDER).append (L"\\base004.avc"),
											"9876543210"), L"Wrong file content\n" );

		}
		
	}
	
}

void TestHttpProxy::TestSwitchingURL_UseProxyCases ()
{
	using namespace KLUPD;
	std::wcout<<L"TestSwitchingURL_UseProxyCases\n";
	Logger::Case _case (m_logger, m_testp, m_step, L"TestSwitchingURL_UseProxyCases");

	{//one proxy (predefined) - 2 sources - invalid signature
		HttpServer hs (3128, "127.0.0.1");
		hs.Start();

		TestLog testLog;

		_helper::DeleteDirectory(ROOT);
		KLUPD::createFolder (BASES_ROOT, &testLog);
		KLUPD::createFolder (UPD_ROOT, &testLog);

		UpdaterConfiguration data(1, 1, L"1");
		data.proxy_url = L"127.0.0.1";
		data.proxy_port = 3128;
		data.m_proxyAuthorizationCredentials.userName ( L"fakeuser");
		data.m_proxyAuthorizationCredentials.password ( L"fakepwd");
		data.m_httpAuthorizationMethods.push_back (ntlmAuthorization);
		data.m_httpAuthorizationMethods.push_back (basicAuthorization);

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

		data.m_sourceList.addSource (UST_UserURL, L"http://bases", true, L"diffs");
		data.m_sourceList.addSource (UST_UserURL, L"http://bases2", true, L"diffs");

		data.UpdaterListFlags.update_bases = true;

		Callbacks cbacks;
		TestCallbackInterface testCallback (&testLog, &cbacks);
		TestJournalInterface testJournal;
		testCallback.SetUpdaterConfiguration (data);

		Updater upd (testCallback.m_dp, testCallback, testJournal, &testLog);
		HANDLE hDoUpdate;

		doUpdater do_upd(&upd, hDoUpdate);

		//first source
		{	//klz
			hs.Accept();
			QC_BOOST_CHECK_MESSAGE (hs.ReceivePureGet(), L"Error getting request\n");
			hs.SendNtlmGreeting ();

			QC_BOOST_CHECK_MESSAGE (hs.ReceiveGetWithNtlmToken(), L"Error getting request\n");
			hs.SendNtlmMsg2();
			QC_BOOST_CHECK_MESSAGE (hs.ReceiveGetWithNtlmToken(), L"Error getting request\n");
			
			//hs.Send403Forbidden (HttpServer::KeepAlive, HttpServer::Close);
			hs.SendFileFromBuffer (HttpServer::KeepAlive, HttpServer::Close, "444");
			hs.CloseConnection();

			//xml
			hs.Accept();
			QC_BOOST_CHECK_MESSAGE (hs.ReceiveGetWithNtlmToken(), L"Error getting request\n");
			hs.SendNtlmMsg2();
			QC_BOOST_CHECK_MESSAGE (hs.ReceiveGetWithNtlmToken(), L"Error getting request\n");

			//hs.Send403Forbidden (HttpServer::KeepAlive, HttpServer::Close);
			hs.SendFileFromBuffer (HttpServer::KeepAlive, HttpServer::Close, "444");

			hs.CloseConnection();

		}

		Sleep (3000);
		for (int i = 1; i <= 10; ++i)
		std::wcout<<L"----------------------------------------------------------"<<L"\n";
		//second source
		{	//klz
			hs.Accept();
			QC_BOOST_CHECK_MESSAGE (hs.ReceiveGetWithNtlmToken("bases2"), L"Error getting request\n");
			hs.SendNtlmMsg2();
			QC_BOOST_CHECK_MESSAGE (hs.ReceiveGetWithNtlmToken("bases2"), L"Error getting request\n");
			hs.SendFileFromBuffer (HttpServer::KeepAlive, HttpServer::Close, "444");
			hs.CloseConnection();

			//xml
			hs.Accept();
			QC_BOOST_CHECK_MESSAGE (hs.ReceiveGetWithNtlmToken("bases2"), L"Error getting request\n");
			hs.SendNtlmMsg2();
			QC_BOOST_CHECK_MESSAGE (hs.ReceiveGetWithNtlmToken("bases2"), L"Error getting request\n");
			hs.SendFileFromBuffer (HttpServer::KeepAlive, HttpServer::Close, "444");
			hs.CloseConnection();

		}

		WaitForSingleObject (hDoUpdate, INFINITE);
		QC_BOOST_CHECK_MESSAGE ( CORE_INVALID_SIGNATURE == do_upd.GetError(), L"Wrong return code\n" );
	}
	
	{//two different proxies for two sources - Basic and NTLM
		TestLog testLog;

		HttpServer hs (3127, "127.0.0.1");
		hs.Start();
		HttpServer hs2 (3129, "127.0.0.1");
		hs2.Start();

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
		data.connect_tmo = 10;

		data.m_sourceList.addSource (UST_UserURL, L"http://bases/", true, L"");
		data.m_sourceList.addSource (UST_UserURL, L"http://bases2/", true, L"");
		//data.m_sourceList.addSource (UST_UserURL, L"http://yandex.ru", true, L"");
		//data.m_sourceList.addSource (UST_UserURL, L"http://sports.ru", true, L"");

		data.m_proxyAuthorizationCredentials.userName ( L"test");
		data.m_proxyAuthorizationCredentials.password ( L"test");
		data.use_ie_proxy = true;
		data.m_httpAuthorizationMethods.push_back (basicAuthorization);
		data.m_httpAuthorizationMethods.push_back (ntlmAuthorization);
		data.UpdaterListFlags.update_bases = true;

		Callbacks cbacks;
		TestCallbackInterface testCallback (&testLog, &cbacks);
		TestSuiteProxyDetector::ProxyJournal journal ( std::wstring(COLL_ROOT).append(L"\\wpads\\http_wpad.dat") );
		testCallback.SetUpdaterConfiguration (data);

		Updater upd (testCallback.m_dp, testCallback, journal, &testLog);
		journal.SetUpdater(&upd);

		cbacks.bFlags[Callbacks::requestCredentialsForAuthorizationOnProxy] = true;

		HANDLE hDoUpdate;

		doUpdater do_upd(&upd, hDoUpdate);

		//first source
		{	//klz
			hs.Accept();
			QC_BOOST_CHECK_MESSAGE (hs.ReceivePureGet(), L"Error getting request\n");
			hs.SendBasicGreeting ();
			QC_BOOST_CHECK_MESSAGE (hs.ReceiveGetWithBasicToken("host: bases"), L"Error getting request\n");
			hs.SendFileFromBuffer (HttpServer::KeepAlive, HttpServer::Close, "fake");
			hs.CloseConnection();

			//xml
			hs.Accept();
			QC_BOOST_CHECK_MESSAGE (hs.ReceiveGetWithBasicToken("host: bases"), L"Error getting request\n");
			hs.SendFileFromBuffer (HttpServer::KeepAlive, HttpServer::Close, "fake");
			hs.CloseConnection();
		}


		Sleep (3000);
		for (int i = 1; i <= 10; ++i)
			std::wcout<<L"----------------------------------------------------------"<<L"\n";

		//second source
		{	//klz

			hs2.Accept();
			QC_BOOST_CHECK_MESSAGE (hs2.ReceivePureGet(), L"Error getting request\n");
			//hs2.SendBasicGreeting ();
			//QC_BOOST_CHECK_MESSAGE (hs2.ReceiveGetWithBasicToken("host: bases2"), L"Error getting request\n");
			hs2.SendNtlmGreeting ();

			QC_BOOST_CHECK_MESSAGE (hs2.ReceiveGetWithNtlmToken(), L"Error getting request\n");
			hs2.SendNtlmMsg2();
			QC_BOOST_CHECK_MESSAGE (hs2.ReceiveGetWithNtlmToken(), L"Error getting request\n");
			hs2.SendFileFromBuffer (HttpServer::KeepAlive, HttpServer::Close, "fake");
			hs2.CloseConnection();

			//xml
			hs2.Accept();
			QC_BOOST_CHECK_MESSAGE (hs2.ReceiveGetWithNtlmToken(), L"Error getting request\n");
			hs2.SendNtlmMsg2();
			QC_BOOST_CHECK_MESSAGE (hs2.ReceiveGetWithNtlmToken(), L"Error getting request\n");
			hs2.SendFileFromBuffer (HttpServer::KeepAlive, HttpServer::Close, "fake");
			hs2.CloseConnection();
		}

		WaitForSingleObject (hDoUpdate, INFINITE);
		QC_BOOST_CHECK_MESSAGE ( CORE_INVALID_SIGNATURE == do_upd.GetError(), L"Wrong return code\n" );
	}

	{//two different proxies for two sources - Basic+NTLM and Basic
		TestLog testLog;

		HttpServer hs (3127, "127.0.0.1");
		hs.Start();
		HttpServer hs2 (3129, "127.0.0.1");
		hs2.Start();

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
		data.connect_tmo = 10;

		data.m_sourceList.addSource (UST_UserURL, L"http://bases/", true, L"");
		data.m_sourceList.addSource (UST_UserURL, L"http://bases2/", true, L"");
		data.m_proxyAuthorizationCredentials.userName ( L"test");
		data.m_proxyAuthorizationCredentials.password ( L"test");
		data.use_ie_proxy = true;
		data.m_httpAuthorizationMethods.push_back (basicAuthorization);
		data.m_httpAuthorizationMethods.push_back (ntlmAuthorization);
		data.UpdaterListFlags.update_bases = true;

		Callbacks cbacks;
		TestCallbackInterface testCallback (&testLog, &cbacks);
		TestSuiteProxyDetector::ProxyJournal journal ( std::wstring(COLL_ROOT).append(L"\\wpads\\http_wpad.dat") );
		testCallback.SetUpdaterConfiguration (data);

		Updater upd (testCallback.m_dp, testCallback, journal, &testLog);
		journal.SetUpdater(&upd);

		cbacks.bFlags[Callbacks::requestCredentialsForAuthorizationOnProxy] = true;

		HANDLE hDoUpdate;

		doUpdater do_upd(&upd, hDoUpdate);

		//first source
		{	//klz
			hs.Accept();
			QC_BOOST_CHECK_MESSAGE (hs.ReceivePureGet(), L"Error getting request\n");
			hs.SendNtlmBasicGreeting ();
			QC_BOOST_CHECK_MESSAGE (hs.ReceiveGetWithNtlmToken(), L"Error getting request\n");
			hs.SendNtlmMsg2();
			QC_BOOST_CHECK_MESSAGE (hs.ReceiveGetWithNtlmToken(), L"Error getting request\n");
			hs.SendNtlmBasicGreeting ();

			QC_BOOST_CHECK_MESSAGE (hs.ReceiveGetWithBasicToken("host: bases"), L"Error getting request\n");
			hs.SendFileFromBuffer (HttpServer::KeepAlive, HttpServer::Close, "fake");
			hs.CloseConnection();

			//xml
			hs.Accept();
			QC_BOOST_CHECK_MESSAGE (hs.ReceiveGetWithBasicToken("host: bases"), L"Error getting request\n");
			hs.SendFileFromBuffer (HttpServer::KeepAlive, HttpServer::Close, "fake");
			hs.CloseConnection();
		}

		//second source
		{	//klz

			hs2.Accept();
			QC_BOOST_CHECK_MESSAGE (hs2.ReceivePureGet("host: bases2"), L"Error getting request\n");
			hs2.SendBasicGreeting ();

			QC_BOOST_CHECK_MESSAGE (hs2.ReceiveGetWithBasicToken(), L"Error getting request\n");
			hs2.SendFileFromBuffer (HttpServer::KeepAlive, HttpServer::Close, "fake");
			hs2.CloseConnection();

			//xml
			hs2.Accept();
			QC_BOOST_CHECK_MESSAGE (hs2.ReceiveGetWithBasicToken(), L"Error getting request\n");
			hs2.SendFileFromBuffer (HttpServer::KeepAlive, HttpServer::Close, "fake");
			hs2.CloseConnection();
		}

		WaitForSingleObject (hDoUpdate, INFINITE);
		QC_BOOST_CHECK_MESSAGE ( CORE_INVALID_SIGNATURE == do_upd.GetError(), L"Wrong return code\n" );
	}
	
	{//two different proxies for two sources - Basic+NTLM and Basic
	//почти то же самое, что и предыдущий кейс, но прокси-сервер реально тот же, поэтому
	//надо помнить типы авторизации
		TestLog testLog;

		HttpServer hs (3130, "127.0.0.1");
		hs.Start();

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
		data.connect_tmo = 10;

		data.m_sourceList.addSource (UST_UserURL, L"http://bases/", true, L"");
		data.m_sourceList.addSource (UST_UserURL, L"http://bases2/", true, L"");
		data.m_proxyAuthorizationCredentials.userName ( L"test");
		data.m_proxyAuthorizationCredentials.password ( L"test");
		data.use_ie_proxy = true;
		data.m_httpAuthorizationMethods.push_back (basicAuthorization);
		data.m_httpAuthorizationMethods.push_back (ntlmAuthorization);
		data.UpdaterListFlags.update_bases = true;

		Callbacks cbacks;
		TestCallbackInterface testCallback (&testLog, &cbacks);
		TestSuiteProxyDetector::ProxyJournal journal ( std::wstring(COLL_ROOT).append(L"\\wpads\\http_wpad4.dat") );
		testCallback.SetUpdaterConfiguration (data);

		Updater upd (testCallback.m_dp, testCallback, journal, &testLog);
		journal.SetUpdater(&upd);

		cbacks.bFlags[Callbacks::requestCredentialsForAuthorizationOnProxy] = true;

		HANDLE hDoUpdate;

		doUpdater do_upd(&upd, hDoUpdate);

		//first source
		{	//klz
			hs.Accept();
			QC_BOOST_CHECK_MESSAGE (hs.ReceivePureGet(), L"Error getting request\n");
			hs.SendNtlmBasicGreeting ();
			QC_BOOST_CHECK_MESSAGE (hs.ReceiveGetWithNtlmToken("host: bases"), L"Error getting request\n");

			hs.SendNtlmBasicGreeting ();
			QC_BOOST_CHECK_MESSAGE (hs.ReceiveGetWithBasicToken(), L"Error getting request\n");
			//hs.SendNtlmMsg2();
			//QC_BOOST_CHECK_MESSAGE (hs.ReceiveGetWithNtlmToken(), L"Error getting request\n");
			hs.SendFileFromBuffer (HttpServer::KeepAlive, HttpServer::Close, "fake");
			hs.CloseConnection();

			//xml
			hs.Accept();
			QC_BOOST_CHECK_MESSAGE (hs.ReceiveGetWithBasicToken(), L"Error getting request\n");
			//QC_BOOST_CHECK_MESSAGE (hs.ReceiveGetWithNtlmToken(), L"Error getting request\n");
			//hs.SendNtlmMsg2();
			//QC_BOOST_CHECK_MESSAGE (hs.ReceiveGetWithNtlmToken(), L"Error getting request\n");
			hs.SendFileFromBuffer (HttpServer::KeepAlive, HttpServer::Close, "fake");
			hs.CloseConnection();
		}

		//second source
		{	//klz

			hs.Accept();
			QC_BOOST_CHECK_MESSAGE (hs.ReceiveGetWithBasicToken(), L"Error getting request\n");
			//hs.SendNtlmBasicGreeting ();

			//QC_BOOST_CHECK_MESSAGE (hs.ReceiveGetWithNtlmToken("host: bases2"), L"Error getting request\n");
			//hs.SendNtlmMsg2();
			//QC_BOOST_CHECK_MESSAGE (hs.ReceiveGetWithNtlmToken("host: bases2"), L"Error getting request\n");
			hs.SendFileFromBuffer (HttpServer::KeepAlive, HttpServer::Close, "fake");
			hs.CloseConnection();

			//xml
			hs.Accept();
			QC_BOOST_CHECK_MESSAGE (hs.ReceiveGetWithBasicToken(), L"Error getting request\n");
			//QC_BOOST_CHECK_MESSAGE (hs.ReceiveGetWithNtlmToken("host: bases2"), L"Error getting request\n");
			//hs.SendNtlmMsg2();
			//QC_BOOST_CHECK_MESSAGE (hs.ReceiveGetWithNtlmToken("host: bases2"), L"Error getting request\n");
			hs.SendFileFromBuffer (HttpServer::KeepAlive, HttpServer::Close, "fake");
			hs.CloseConnection();
		}

		WaitForSingleObject (hDoUpdate, INFINITE);
		QC_BOOST_CHECK_MESSAGE ( CORE_INVALID_SIGNATURE == do_upd.GetError(), L"Wrong return code\n" );
	}

	{//two different proxies for two sources - Basic+NTLM and Basic
		//почти то же самое, что и предыдущий кейс, но прокси-сервер реально тот же, поэтому
		//надо помнить, типы авторизации + прокси держат соединения keep-alive
		TestLog testLog;

		HttpServer hs (3130, "127.0.0.1");
		hs.Start();

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
		data.connect_tmo = 10;

		data.m_sourceList.addSource (UST_UserURL, L"http://bases/", true, L"");
		data.m_sourceList.addSource (UST_UserURL, L"http://bases2/", true, L"");
		data.m_proxyAuthorizationCredentials.userName ( L"test");
		data.m_proxyAuthorizationCredentials.password ( L"test");
		data.use_ie_proxy = true;
		data.m_httpAuthorizationMethods.push_back (basicAuthorization);
		data.m_httpAuthorizationMethods.push_back (ntlmAuthorization);
		data.UpdaterListFlags.update_bases = true;

		Callbacks cbacks;
		TestCallbackInterface testCallback (&testLog, &cbacks);
		TestSuiteProxyDetector::ProxyJournal journal ( std::wstring(COLL_ROOT).append(L"\\wpads\\http_wpad4.dat") );
		testCallback.SetUpdaterConfiguration (data);

		Updater upd (testCallback.m_dp, testCallback, journal, &testLog);
		journal.SetUpdater(&upd);

		cbacks.bFlags[Callbacks::requestCredentialsForAuthorizationOnProxy] = true;

		HANDLE hDoUpdate;

		doUpdater do_upd(&upd, hDoUpdate);

		//first source
		{	//klz
			hs.Accept();
			QC_BOOST_CHECK_MESSAGE (hs.ReceivePureGet(), L"Error getting request\n");
			//hs.SendNtlmBasicGreeting ();
			//QC_BOOST_CHECK_MESSAGE (hs.ReceiveGetWithBasicToken("host: bases"), L"Error getting request\n");

			hs.SendNtlmBasicGreeting ();
			QC_BOOST_CHECK_MESSAGE (hs.ReceiveGetWithNtlmToken(), L"Error getting request\n");
			hs.SendNtlmMsg2();
			QC_BOOST_CHECK_MESSAGE (hs.ReceiveGetWithNtlmToken(), L"Error getting request\n");
			hs.SendFileFromBuffer (HttpServer::KeepAlive, HttpServer::KeepAlive, "fake");

			//xml
			QC_BOOST_CHECK_MESSAGE (hs.ReceivePureGet(), L"Error getting request\n");
			hs.SendFileFromBuffer (HttpServer::KeepAlive, HttpServer::KeepAlive, "fake");
		}

		//second source
		{	//klz
			QC_BOOST_CHECK_MESSAGE (hs.ReceivePureGet("host: bases2"), L"Error getting request\n");
			hs.SendFileFromBuffer (HttpServer::KeepAlive, HttpServer::KeepAlive, "fake");

			//xml
			QC_BOOST_CHECK_MESSAGE (hs.ReceivePureGet("host: bases2"), L"Error getting request\n");
			hs.SendFileFromBuffer (HttpServer::KeepAlive, HttpServer::KeepAlive, "fake");
			hs.CheckConnectionClosed();
		}

		WaitForSingleObject (hDoUpdate, INFINITE);
		QC_BOOST_CHECK_MESSAGE ( CORE_INVALID_SIGNATURE == do_upd.GetError(), L"Wrong return code\n" );
	}
	
	
}


//добавить кейс, когда поддерживаем 2 вида авторизации, первый прокси поддерживает NTLM, а второй BASIC
//это в SwitchingProxies
void TestHttpProxy::TestSwitchingURL_UseWPAD ()
{
	using namespace KLUPD;
	std::wcout<<L"TestSwitchingURL_UseWPAD\n";
	Logger::Case _case (m_logger, m_testp, m_step, L"TestSwitchingURL_UseWPAD");

	{//two different proxies for two sources
		TestLog testLog;

		HttpServer hs (3127, "127.0.0.1");
		hs.Start();
		HttpServer hs2 (3129, "127.0.0.1");
		hs2.Start();

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
		data.connect_tmo = 10;

		data.m_sourceList.addSource (UST_UserURL, L"http://bases/", true, L"");
		data.m_sourceList.addSource (UST_UserURL, L"http://bases2/", true, L"");
		data.m_proxyAuthorizationCredentials.userName ( L"null");
		data.m_proxyAuthorizationCredentials.password ( L"null");
		data.use_ie_proxy = true;
		data.m_httpAuthorizationMethods.push_back (ntlmAuthorization);
		data.UpdaterListFlags.update_bases = true;

		Callbacks cbacks;
		TestCallbackInterface testCallback (&testLog, &cbacks);
		TestSuiteProxyDetector::ProxyJournal journal ( std::wstring(COLL_ROOT).append(L"\\wpads\\http_wpad.dat") );
		testCallback.SetUpdaterConfiguration (data);

		Updater upd (testCallback.m_dp, testCallback, journal, &testLog);
		journal.SetUpdater(&upd);

		HANDLE hDoUpdate;

		doUpdater do_upd(&upd, hDoUpdate);

		//first source
		{	//klz
			hs.Accept();
			QC_BOOST_CHECK_MESSAGE (hs.ReceivePureGet(), L"Error getting request\n");
			hs.SendNtlmGreeting ();

			QC_BOOST_CHECK_MESSAGE (hs.ReceiveGetWithNtlmToken("host: bases"), L"Error getting request\n");
			hs.SendNtlmMsg2();
			QC_BOOST_CHECK_MESSAGE (hs.ReceiveGetWithNtlmToken(), L"Error getting request\n");
			//hs.Send403Forbidden (HttpServer::KeepAlive, HttpServer::Close);
			hs.SendFileFromBuffer (HttpServer::KeepAlive, HttpServer::Close, "444");
			hs.CloseConnection();

			//xml
			hs.Accept();
			QC_BOOST_CHECK_MESSAGE (hs.ReceiveGetWithNtlmToken("host: bases"), L"Error getting request\n");
			hs.SendNtlmMsg2();
			QC_BOOST_CHECK_MESSAGE (hs.ReceiveGetWithNtlmToken(), L"Error getting request\n");
			//hs.Send403Forbidden (HttpServer::KeepAlive, HttpServer::Close);
			hs.SendFileFromBuffer (HttpServer::KeepAlive, HttpServer::Close, "444");
			hs.CloseConnection();
		}

		
		Sleep (3000);
		for (int i = 1; i <= 10; ++i)
			std::wcout<<L"----------------------------------------------------------"<<L"\n";

		//second source
		{	//klz
			hs2.Accept();
			QC_BOOST_CHECK_MESSAGE (hs2.ReceivePureGet(), L"Error getting request\n");
			hs2.SendNtlmGreeting ();

			QC_BOOST_CHECK_MESSAGE (hs2.ReceiveGetWithNtlmToken("host: bases2"), L"Error getting request\n");
			hs2.SendNtlmMsg2();
			QC_BOOST_CHECK_MESSAGE (hs2.ReceiveGetWithNtlmToken(), L"Error getting request\n");
			//hs2.Send403Forbidden (HttpServer::KeepAlive, HttpServer::Close);
			hs2.SendFileFromBuffer (HttpServer::KeepAlive, HttpServer::Close, "555");
			hs2.CloseConnection();

			//xml
			hs2.Accept();
			QC_BOOST_CHECK_MESSAGE (hs2.ReceiveGetWithNtlmToken("host: bases2"), L"Error getting request\n");
			hs2.SendNtlmMsg2();
			QC_BOOST_CHECK_MESSAGE (hs2.ReceiveGetWithNtlmToken(), L"Error getting request\n");
			//hs2.Send403Forbidden (HttpServer::KeepAlive, HttpServer::Close);
			hs2.SendFileFromBuffer (HttpServer::KeepAlive, HttpServer::Close, "555");
			hs2.CloseConnection();
		}

		WaitForSingleObject (hDoUpdate, INFINITE);
		QC_BOOST_CHECK_MESSAGE ( CORE_INVALID_SIGNATURE == do_upd.GetError(), L"Wrong return code\n" );
	}

	{//one source uses proxy, the other does not (wpad instruction)
		TestLog testLog;

		HttpServer hs_proxy (3127, "127.0.0.1");
		hs_proxy.Start();
		HttpServer hs_server(8001, "127.0.0.1");
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
		data.connect_tmo = 10;

		data.m_sourceList.addSource (UST_UserURL, L"http://bases/", true, L"");
		data.m_sourceList.addSource (UST_UserURL, L"http://127.0.0.1:8001/", true, L"");
		data.m_proxyAuthorizationCredentials.userName ( L"null");
		data.m_proxyAuthorizationCredentials.password ( L"null");
		data.use_ie_proxy = true;
		data.m_httpAuthorizationMethods.push_back (ntlmAuthorization);
		data.UpdaterListFlags.update_bases = true;

		Callbacks cbacks;
		TestCallbackInterface testCallback (&testLog, &cbacks);
		TestSuiteProxyDetector::ProxyJournal journal ( std::wstring(COLL_ROOT).append(L"\\wpads\\http_wpad2.dat") );
		testCallback.SetUpdaterConfiguration (data);

		Updater upd (testCallback.m_dp, testCallback, journal, &testLog);
		journal.SetUpdater(&upd);

		HANDLE hDoUpdate;

		doUpdater do_upd(&upd, hDoUpdate);

		//first source
		{	//klz
			hs_proxy.Accept();
			QC_BOOST_CHECK_MESSAGE (hs_proxy.ReceivePureGet(), L"Error getting request\n");
			hs_proxy.SendNtlmGreeting ();

			QC_BOOST_CHECK_MESSAGE (hs_proxy.ReceiveGetWithNtlmToken("host: bases"), L"Error getting request\n");
			hs_proxy.SendNtlmMsg2();
			QC_BOOST_CHECK_MESSAGE (hs_proxy.ReceiveGetWithNtlmToken(), L"Error getting request\n");
			//hs_proxy.Send403Forbidden (HttpServer::KeepAlive, HttpServer::Close);
			hs_proxy.SendFileFromBuffer (HttpServer::KeepAlive, HttpServer::Close, "555");
			hs_proxy.CloseConnection();

			//xml
			hs_proxy.Accept();
			QC_BOOST_CHECK_MESSAGE (hs_proxy.ReceiveGetWithNtlmToken("host: bases"), L"Error getting request\n");
			hs_proxy.SendNtlmMsg2();
			QC_BOOST_CHECK_MESSAGE (hs_proxy.ReceiveGetWithNtlmToken(), L"Error getting request\n");
			//hs_proxy.Send403Forbidden (HttpServer::KeepAlive, HttpServer::Close);
			hs_proxy.SendFileFromBuffer (HttpServer::KeepAlive, HttpServer::Close, "555");
			hs_proxy.CloseConnection();

		}

		//second source
		{	//klz
			hs_server.Accept();

			QC_BOOST_CHECK_MESSAGE (hs_server.ReceivePureGet("host: 127.0.0.1"), L"Error getting request\n");
			hs_server.SendFileFromBuffer (HttpServer::Close, HttpServer::None, "0123456789");
			hs_server.CloseConnection();

			//xml
			hs_server.Accept();
			QC_BOOST_CHECK_MESSAGE (hs_server.ReceivePureGet("host: 127.0.0.1"), L"Error getting request\n");
			hs_server.SendFileFromBuffer (HttpServer::Close, HttpServer::None, "0123456789");
			hs_server.CloseConnection();
		}

		WaitForSingleObject (hDoUpdate, INFINITE);

		QC_BOOST_CHECK_MESSAGE ( CORE_INVALID_SIGNATURE == do_upd.GetError(), L"Wrong return code\n" );
	}
	
	{//one source uses proxy, the other does not (user instruction)
		TestLog testLog;

		HttpServer hs_proxy (3127, "127.0.0.1");
		hs_proxy.Start();
		HttpServer hs_server(8001, "127.0.0.1");
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
		data.connect_tmo = 10;

		data.m_sourceList.addSource (UST_UserURL, L"http://bases/", true, L"");
		data.m_sourceList.addSource (UST_UserURL, L"http://127.0.0.1:8001/", false, L"");
		data.m_proxyAuthorizationCredentials.userName ( L"null");
		data.m_proxyAuthorizationCredentials.password ( L"null");
		data.use_ie_proxy = true;
		data.m_httpAuthorizationMethods.push_back (ntlmAuthorization);
		data.UpdaterListFlags.update_bases = true;

		Callbacks cbacks;
		TestCallbackInterface testCallback (&testLog, &cbacks);
		TestSuiteProxyDetector::ProxyJournal journal ( std::wstring(COLL_ROOT).append(L"\\wpads\\http_wpad2.dat") );
		testCallback.SetUpdaterConfiguration (data);

		Updater upd (testCallback.m_dp, testCallback, journal, &testLog);
		journal.SetUpdater(&upd);

		HANDLE hDoUpdate;

		doUpdater do_upd(&upd, hDoUpdate);

		//first source
		{	//klz
			hs_proxy.Accept();
			QC_BOOST_CHECK_MESSAGE (hs_proxy.ReceivePureGet(), L"Error getting request\n");
			hs_proxy.SendNtlmGreeting ();

			QC_BOOST_CHECK_MESSAGE (hs_proxy.ReceiveGetWithNtlmToken("host: bases"), L"Error getting request\n");
			hs_proxy.SendNtlmMsg2();
			QC_BOOST_CHECK_MESSAGE (hs_proxy.ReceiveGetWithNtlmToken(), L"Error getting request\n");
			//hs_proxy.Send403Forbidden (HttpServer::KeepAlive, HttpServer::Close);
			hs_proxy.SendFileFromBuffer (HttpServer::KeepAlive, HttpServer::Close, "555");
			hs_proxy.CloseConnection();

			//xml
			hs_proxy.Accept();
			QC_BOOST_CHECK_MESSAGE (hs_proxy.ReceiveGetWithNtlmToken("host: bases"), L"Error getting request\n");
			hs_proxy.SendNtlmMsg2();
			QC_BOOST_CHECK_MESSAGE (hs_proxy.ReceiveGetWithNtlmToken(), L"Error getting request\n");
			//hs_proxy.Send403Forbidden (HttpServer::KeepAlive, HttpServer::Close);
			hs_proxy.SendFileFromBuffer (HttpServer::KeepAlive, HttpServer::Close, "555");
			hs_proxy.CloseConnection();

		}

		//second source
		{	//klz
			hs_server.Accept();

			QC_BOOST_CHECK_MESSAGE (hs_server.ReceivePureGet("host: 127.0.0.1"), L"Error getting request\n");
			hs_server.SendFileFromBuffer (HttpServer::Close, HttpServer::None, "0123456789");
			hs_server.CloseConnection();

			//xml
			hs_server.Accept();
			QC_BOOST_CHECK_MESSAGE (hs_server.ReceivePureGet("host: 127.0.0.1"), L"Error getting request\n");
			hs_server.SendFileFromBuffer (HttpServer::Close, HttpServer::None, "0123456789");
			hs_server.CloseConnection();
		}

		WaitForSingleObject (hDoUpdate, INFINITE);

		QC_BOOST_CHECK_MESSAGE ( CORE_INVALID_SIGNATURE == do_upd.GetError(), L"Wrong return code\n" );
	}

}

void TestHttpProxy::TestRequestingAuthInformation ()
{
	using namespace KLUPD;
	std::wcout<<L"TestRequestingAuthInformation\n";
	Logger::Case _case (m_logger, m_testp, m_step, L"TestRequestingAuthInformation");

	{//Basic only, creds are requested
		HttpServer hs (3128, "127.0.0.1");
		hs.Start();

		TestLog testLog;
		Callbacks cbacks;
		TestCallbackInterface testCallback (&testLog, &cbacks);
		TestJournalInterface testJournal;
		Signature6Checker s6c (&testLog);

		UpdaterConfiguration data(1, 1, L"1");
		data.proxy_url = L"127.0.0.1";
		data.proxy_port = 3128;
		data.m_httpAuthorizationMethods.push_back (basicAuthorization);
		data.m_sourceList.addSource (UST_UserURL, L"http://httpserver", true, L"diffs");
		testCallback.SetUpdaterConfiguration (data);
		KLUPD::Source _csd (L"http://httpserver", UST_UserURL, true, L"", UPS_DIRS);
		
		cbacks.bFlags[Callbacks::requestCredentialsForAuthorizationOnProxy] = true;
		CREATE_EVENTS(requestCredentialsForAuthorizationOnProxy)

		Updater upd (testCallback.m_dp, testCallback, testJournal, &testLog);
		upd.constructSourceList(testCallback.updaterConfiguration().m_sourceList, upd.m_sourceList);

		HANDLE hDoDownload;

		string command;
		string param;
		HttpServer::CMD_TYPE type;

		{

			FileInfo _fi (L"base005.avc", L"/");
			_fi.m_localPath = L"h:\\test\\";

			_helper::DeleteDirectory(TEST_FOLDER);
			_helper::MakeDirectory(TEST_FOLDER);

			doEntryDownloader do_dld(&upd, hDoDownload, _fi, _fi.m_localPath, &s6c);

			hs.Accept();

			hs.Receive(command, type, param);
			QC_BOOST_CHECK_MESSAGE (type == HttpServer::CMD_GET, L"Error\n");
			hs.Send("HTTP/1.1 407 Proxy Authentication Required\x0d\x0a");
			hs.Send("Proxy-Authenticate: Basic realm=\"Hello\"\x0d\x0a");
			hs.Send("Connection: Keep-Alive\x0d\x0a");
			hs.Send("Proxy-Connection: Close\x0d\x0a");
			hs.Send("\x0d\x0a");
			hs.CloseConnection ();

			WAIT_FOR_READY_EVENT(requestCredentialsForAuthorizationOnProxy)
			TestCallbackInterface::_credentials creds;
			creds.name = L"test";
			creds.password = L"test";
			testCallback.SetRequestCreds(creds);
			SET_GOON_EVENT(requestCredentialsForAuthorizationOnProxy);

			hs.Accept();
			hs.Receive(command, type, param);
			QC_BOOST_CHECK_MESSAGE (type == HttpServer::CMD_GET, L"Error\n");
			QC_BOOST_CHECK_MESSAGE (command.find("proxy-authorization: basic") != wstring::npos, L"Error\n");
			string basic_auth1 = command.substr (command.find("proxy-authorization: basic ") + 27);
			hs.Send("HTTP/1.1 200 OK\x0d\x0a");
			hs.Send("Content-Length: 10\x0d\x0a");
			hs.Send("Connection: Keep-Alive\x0d\x0a");
			hs.Send("Proxy-Connection: Close\x0d\x0a");
			hs.Send("\x0d\x0a");
			hs.Send("0123456789");

			hs.CloseConnection ();

			WaitForSingleObject (hDoDownload, INFINITE);
			QC_BOOST_CHECK_MESSAGE ( CORE_NO_ERROR == do_dld.GetError(), L"Wrong return code\n" );
			QC_BOOST_CHECK_MESSAGE ( _helper::checkFileContent (wstring(TEST_FOLDER).append (L"\\base005.avc"),
				"0123456789"), L"Wrong file content\n" );
		}
	}

	{//no authorization, creds are not requested
		HttpServer hs (3128, "127.0.0.1");
		hs.Start();

		TestLog testLog;
		Callbacks cbacks;
		TestCallbackInterface testCallback (&testLog, &cbacks);
		TestJournalInterface testJournal;
		Signature6Checker s6c (&testLog);

		UpdaterConfiguration data(1, 1, L"1");
		data.proxy_url = L"127.0.0.1";
		data.proxy_port = 3128;
		data.m_httpAuthorizationMethods.push_back (basicAuthorization);
		data.m_sourceList.addSource (UST_UserURL, L"http://httpserver", true, L"diffs");
		testCallback.SetUpdaterConfiguration (data);
		KLUPD::Source _csd (L"http://httpserver", UST_UserURL, true, L"", UPS_DIRS);

		cbacks.bFlags[Callbacks::requestCredentialsForAuthorizationOnProxy] = true;
		CREATE_EVENTS(requestCredentialsForAuthorizationOnProxy)
		Updater upd (testCallback.m_dp, testCallback, testJournal, &testLog);
		upd.constructSourceList(testCallback.updaterConfiguration().m_sourceList, upd.m_sourceList);


		HANDLE hDoDownload;

		string command;
		string param;
		HttpServer::CMD_TYPE type;

		{

			FileInfo _fi (L"base005.avc", L"/");
			_fi.m_localPath = L"h:\\test\\";

			_helper::DeleteDirectory(TEST_FOLDER);
			_helper::MakeDirectory(TEST_FOLDER);

			doEntryDownloader do_dld(&upd, hDoDownload, _fi, _fi.m_localPath, &s6c);

			hs.Accept();

			hs.Receive(command, type, param);
			QC_BOOST_CHECK_MESSAGE (type == HttpServer::CMD_GET, L"Error\n");
			QC_BOOST_CHECK_MESSAGE (command.find("proxy-authorization:") == wstring::npos, L"Error\n");
			hs.Send("HTTP/1.1 200 OK\x0d\x0a");
			hs.Send("Content-Length: 10\x0d\x0a");
			hs.Send("Connection: Keep-Alive\x0d\x0a");
			hs.Send("Proxy-Connection: Close\x0d\x0a");
			hs.Send("\x0d\x0a");
			hs.Send("0123456789");

			hs.CloseConnection ();

			WaitForSingleObject (hDoDownload, INFINITE);
			QC_BOOST_CHECK_MESSAGE ( CORE_NO_ERROR == do_dld.GetError(), L"Wrong return code\n" );
			QC_BOOST_CHECK_MESSAGE ( _helper::checkFileContent (wstring(TEST_FOLDER).append (L"\\base005.avc"),
				"0123456789"), L"Wrong file content\n" );
		}
	}

	{//NTLM - creds are not requested
		HttpServer hs (3128, "127.0.0.1");
		hs.Start();

		TestLog testLog;
		Callbacks cbacks;
		TestCallbackInterface testCallback (&testLog, &cbacks);
		TestJournalInterface testJournal;
		Signature6Checker s6c (&testLog);

		UpdaterConfiguration data(1, 1, L"1");
		data.proxy_url = L"127.0.0.1";
		data.proxy_port = 3128;
		data.m_httpAuthorizationMethods.push_back (ntlmAuthorization);
		data.m_sourceList.addSource (UST_UserURL, L"http://httpserver", true, L"diffs");
		testCallback.SetUpdaterConfiguration (data);
		KLUPD::Source _csd (L"http://httpserver", UST_UserURL, true, L"", UPS_DIRS);

		cbacks.bFlags[Callbacks::requestCredentialsForAuthorizationOnProxy] = true;
		CREATE_EVENTS(requestCredentialsForAuthorizationOnProxy)
		Updater upd (testCallback.m_dp, testCallback, testJournal, &testLog);
		upd.constructSourceList(testCallback.updaterConfiguration().m_sourceList, upd.m_sourceList);


		HANDLE hDoDownload;

		string command;
		string param;
		HttpServer::CMD_TYPE type;

		{

			FileInfo _fi (L"base005.avc", L"/");
			_fi.m_localPath = L"h:\\test\\";

			_helper::DeleteDirectory(TEST_FOLDER);
			_helper::MakeDirectory(TEST_FOLDER);

			doEntryDownloader do_dld(&upd, hDoDownload, _fi, _fi.m_localPath, &s6c);

			hs.Accept();

			hs.Receive(command, type, param);
			QC_BOOST_CHECK_MESSAGE (type == HttpServer::CMD_GET, L"Error\n");
			hs.Send("HTTP/1.1 407 Proxy Authentication Required\x0d\x0a");
			hs.Send("Proxy-Authenticate: NTLM\x0d\x0a");
			hs.Send("Content-Length: 0\x0d\x0a");
			hs.Send("Connection: Keep-Alive\x0d\x0a");
			hs.Send("Proxy-Connection: Keep-Alive\x0d\x0a");
			hs.Send("\x0d\x0a");

			//msg1
			hs.Receive(command, type, param);
			QC_BOOST_CHECK_MESSAGE (type == HttpServer::CMD_GET, L"Error\n");
			QC_BOOST_CHECK_MESSAGE (command.find("proxy-authorization: ntlm") != wstring::npos, L"Error\n");
			//msg2
			hs.Send("HTTP/1.1 407 Proxy Authentication Required\x0d\x0a");
			hs.Send("Content-Length: 0\x0d\x0a");
			hs.Send("Connection: Keep-Alive\x0d\x0a");
			hs.Send("Proxy-Connection: Keep-Alive\x0d\x0a");
			hs.Send("Proxy-Authenticate: NTLM TlRMTVNTUAACAAAABAAEADAAAAAFgomg1xL3GW9+PnoAAAAAAAAAAFoAWgA0AAAASwBMAAIABABLAEwAAQAUAFQATAAtAFAAUgBYAC0ATgBUAEEABAAMAGEAdgBwAC4AcgB1AAMAIgB0AGwALQBwAHIAeAAtAG4AdABhAC4AYQB2AHAALgByAHUAAAAAAA==\x0d\x0a");
			hs.Send("\x0d\x0a");
			//msg3
			hs.Receive(command, type, param);
			QC_BOOST_CHECK_MESSAGE (type == HttpServer::CMD_GET, L"Error\n");
			QC_BOOST_CHECK_MESSAGE (command.find("proxy-authorization: ntlm") != wstring::npos, L"Error\n");
			hs.Send("HTTP/1.1 200 OK\x0d\x0a");
			hs.Send("Content-Length: 10\x0d\x0a");
			hs.Send("Connection: Keep-Alive\x0d\x0a");
			hs.Send("Proxy-Connection: Keep-Alive\x0d\x0a");
			hs.Send("\x0d\x0a");
			hs.Send("0123456789");

			WaitForSingleObject (hDoDownload, INFINITE);
			QC_BOOST_CHECK_MESSAGE ( CORE_NO_ERROR == do_dld.GetError(), L"Wrong return code\n" );
			QC_BOOST_CHECK_MESSAGE ( _helper::checkFileContent (wstring(TEST_FOLDER).append (L"\\base005.avc"),
				"0123456789"), L"Wrong file content\n" );
		}
	}

	{//NTLM with creds - creds are requested
		HttpServer hs (3128, "127.0.0.1");
		hs.Start();

		TestLog testLog;
		Callbacks cbacks;
		TestCallbackInterface testCallback (&testLog, &cbacks);
		TestJournalInterface testJournal;
		Signature6Checker s6c (&testLog);

		UpdaterConfiguration data(1, 1, L"1");
		data.proxy_url = L"127.0.0.1";
		data.proxy_port = 3128;
		data.m_httpAuthorizationMethods.push_back (ntlmAuthorizationWithCredentials);
		data.m_sourceList.addSource (UST_UserURL, L"http://httpserver", true, L"diffs");
		testCallback.SetUpdaterConfiguration (data);
		KLUPD::Source _csd (L"http://httpserver", UST_UserURL, true, L"", UPS_DIRS);

		cbacks.bFlags[Callbacks::requestCredentialsForAuthorizationOnProxy] = true;
		CREATE_EVENTS(requestCredentialsForAuthorizationOnProxy)
		
		Updater upd (testCallback.m_dp, testCallback, testJournal, &testLog);
		upd.constructSourceList(testCallback.updaterConfiguration().m_sourceList, upd.m_sourceList);

		HANDLE hDoDownload;

		string command;
		string param;
		HttpServer::CMD_TYPE type;

		{

			FileInfo _fi (L"base005.avc", L"/");
			_fi.m_localPath = L"h:\\test\\";

			_helper::DeleteDirectory(TEST_FOLDER);
			_helper::MakeDirectory(TEST_FOLDER);

			doEntryDownloader do_dld(&upd, hDoDownload, _fi, _fi.m_localPath, &s6c);

			hs.Accept();

			hs.Receive(command, type, param);
			QC_BOOST_CHECK_MESSAGE (type == HttpServer::CMD_GET, L"Error\n");
			hs.Send("HTTP/1.1 407 Proxy Authentication Required\x0d\x0a");
			hs.Send("Proxy-Authenticate: NTLM\x0d\x0a");
			hs.Send("Content-Length: 0\x0d\x0a");
			hs.Send("Connection: Keep-Alive\x0d\x0a");
			hs.Send("Proxy-Connection: Keep-Alive\x0d\x0a");
			hs.Send("\x0d\x0a");

			WAIT_FOR_READY_EVENT(requestCredentialsForAuthorizationOnProxy)
			TestCallbackInterface::_credentials creds;
			creds.name = L"test";
			creds.password = L"test";
			testCallback.SetRequestCreds(creds);
			SET_GOON_EVENT(requestCredentialsForAuthorizationOnProxy);

			//msg1
			hs.Receive(command, type, param);
			QC_BOOST_CHECK_MESSAGE (type == HttpServer::CMD_GET, L"Error\n");
			QC_BOOST_CHECK_MESSAGE (command.find("proxy-authorization: ntlm") != wstring::npos, L"Error\n");
			//msg2
			hs.Send("HTTP/1.1 407 Proxy Authentication Required\x0d\x0a");
			hs.Send("Content-Length: 0\x0d\x0a");
			hs.Send("Connection: Keep-Alive\x0d\x0a");
			hs.Send("Proxy-Connection: Keep-Alive\x0d\x0a");
			hs.Send("Proxy-Authenticate: NTLM TlRMTVNTUAACAAAABAAEADAAAAAFgomg1xL3GW9+PnoAAAAAAAAAAFoAWgA0AAAASwBMAAIABABLAEwAAQAUAFQATAAtAFAAUgBYAC0ATgBUAEEABAAMAGEAdgBwAC4AcgB1AAMAIgB0AGwALQBwAHIAeAAtAG4AdABhAC4AYQB2AHAALgByAHUAAAAAAA==\x0d\x0a");
			hs.Send("\x0d\x0a");
			//msg3
			hs.Receive(command, type, param);
			QC_BOOST_CHECK_MESSAGE (type == HttpServer::CMD_GET, L"Error\n");
			QC_BOOST_CHECK_MESSAGE (command.find("proxy-authorization: ntlm") != wstring::npos, L"Error\n");
			hs.Send("HTTP/1.1 200 OK\x0d\x0a");
			hs.Send("Content-Length: 10\x0d\x0a");
			hs.Send("Connection: Keep-Alive\x0d\x0a");
			hs.Send("Proxy-Connection: Keep-Alive\x0d\x0a");
			hs.Send("\x0d\x0a");
			hs.Send("0123456789");

			WaitForSingleObject (hDoDownload, INFINITE);
			QC_BOOST_CHECK_MESSAGE ( CORE_NO_ERROR == do_dld.GetError(), L"Wrong return code\n" );
			QC_BOOST_CHECK_MESSAGE ( _helper::checkFileContent (wstring(TEST_FOLDER).append (L"\\base005.avc"),
				"0123456789"), L"Wrong file content\n" );
		}
	}

	{//NTLM and Basic - creds are requested
		HttpServer hs (3128, "127.0.0.1");
		hs.Start();

		TestLog testLog;
		Callbacks cbacks;
		TestCallbackInterface testCallback (&testLog, &cbacks);
		TestJournalInterface testJournal;
		Signature6Checker s6c (&testLog);

		UpdaterConfiguration data(1, 1, L"1");
		data.proxy_url = L"127.0.0.1";
		data.proxy_port = 3128;
		data.m_httpAuthorizationMethods.push_back (ntlmAuthorization);
		data.m_httpAuthorizationMethods.push_back (basicAuthorization);
		data.m_sourceList.addSource (UST_UserURL, L"http://httpserver", true, L"diffs");
		testCallback.SetUpdaterConfiguration (data);
		KLUPD::Source _csd (L"http://httpserver", UST_UserURL, true, L"", UPS_DIRS);

		cbacks.bFlags[Callbacks::requestCredentialsForAuthorizationOnProxy] = true;
		CREATE_EVENTS(requestCredentialsForAuthorizationOnProxy)
		Updater upd (testCallback.m_dp, testCallback, testJournal, &testLog);
		upd.constructSourceList(testCallback.updaterConfiguration().m_sourceList, upd.m_sourceList);


		HANDLE hDoDownload;

		string command;
		string param;
		HttpServer::CMD_TYPE type;

		{

			FileInfo _fi (L"base005.avc", L"/");
			_fi.m_localPath = L"h:\\test\\";

			_helper::DeleteDirectory(TEST_FOLDER);
			_helper::MakeDirectory(TEST_FOLDER);

			doEntryDownloader do_dld(&upd, hDoDownload, _fi, _fi.m_localPath, &s6c);

			hs.Accept();

			hs.Receive(command, type, param);
			QC_BOOST_CHECK_MESSAGE (type == HttpServer::CMD_GET, L"Error\n");
			hs.Send("HTTP/1.1 407 Proxy Authentication Required\x0d\x0a");
			hs.Send("Proxy-Authenticate: NTLM\x0d\x0a");
			hs.Send("Proxy-Authenticate: Basic realm=\"Hello\"\x0d\x0a");
			hs.Send("Content-Length: 0\x0d\x0a");
			hs.Send("Connection: Keep-Alive\x0d\x0a");
			hs.Send("Proxy-Connection: Keep-Alive\x0d\x0a");
			hs.Send("\x0d\x0a");

			//msg1
			hs.Receive(command, type, param);
			QC_BOOST_CHECK_MESSAGE (type == HttpServer::CMD_GET, L"Error\n");
			QC_BOOST_CHECK_MESSAGE (command.find("proxy-authorization: ntlm") != wstring::npos, L"Error\n");
			//msg2
			hs.Send("HTTP/1.1 407 Proxy Authentication Required\x0d\x0a");
			hs.Send("Content-Length: 0\x0d\x0a");
			hs.Send("Connection: Keep-Alive\x0d\x0a");
			hs.Send("Proxy-Connection: Keep-Alive\x0d\x0a");
			hs.Send("Proxy-Authenticate: NTLM TlRMTVNTUAACAAAABAAEADAAAAAFgomg1xL3GW9+PnoAAAAAAAAAAFoAWgA0AAAASwBMAAIABABLAEwAAQAUAFQATAAtAFAAUgBYAC0ATgBUAEEABAAMAGEAdgBwAC4AcgB1AAMAIgB0AGwALQBwAHIAeAAtAG4AdABhAC4AYQB2AHAALgByAHUAAAAAAA==\x0d\x0a");
			hs.Send("\x0d\x0a");
			//msg3
			hs.Receive(command, type, param);
			QC_BOOST_CHECK_MESSAGE (type == HttpServer::CMD_GET, L"Error\n");
			QC_BOOST_CHECK_MESSAGE (command.find("proxy-authorization: ntlm") != wstring::npos, L"Error\n");
			hs.Send("HTTP/1.1 407 Proxy Authentication Required\x0d\x0a");
			hs.Send("Proxy-Authenticate: NTLM\x0d\x0a");
			hs.Send("Proxy-Authenticate: Basic realm=\"Hello\"\x0d\x0a");
			hs.Send("Content-Length: 0\x0d\x0a");
			hs.Send("Connection: Keep-Alive\x0d\x0a");
			hs.Send("Proxy-Connection: Keep-Alive\x0d\x0a");
			hs.Send("\x0d\x0a");

			//basic

			WAIT_FOR_READY_EVENT(requestCredentialsForAuthorizationOnProxy)
			TestCallbackInterface::_credentials creds;
			creds.name = L"test";
			creds.password = L"test";
			testCallback.SetRequestCreds(creds);
			SET_GOON_EVENT(requestCredentialsForAuthorizationOnProxy);

			hs.Receive(command, type, param);
			QC_BOOST_CHECK_MESSAGE (type == HttpServer::CMD_GET, L"Error\n");
			QC_BOOST_CHECK_MESSAGE (command.find("proxy-authorization: basic") != wstring::npos, L"Error\n");
			string basic_auth1 = command.substr (command.find("proxy-authorization: basic ") + 27);
			hs.Send("HTTP/1.1 200 OK\x0d\x0a");
			hs.Send("Content-Length: 10\x0d\x0a");
			hs.Send("Connection: Keep-Alive\x0d\x0a");
			hs.Send("Proxy-Connection: Close\x0d\x0a");
			hs.Send("\x0d\x0a");
			hs.Send("0123456789");

			hs.CloseConnection ();

			WaitForSingleObject (hDoDownload, INFINITE);
			QC_BOOST_CHECK_MESSAGE ( CORE_NO_ERROR == do_dld.GetError(), L"Wrong return code\n" );
			QC_BOOST_CHECK_MESSAGE ( _helper::checkFileContent (wstring(TEST_FOLDER).append (L"\\base005.avc"),
				"0123456789"), L"Wrong file content\n" );
		}
	}
	
}

void TestHttpProxy::TestEmptyAuthListMode ()
{
	using namespace KLUPD;
	std::wcout<<L"TestEmptyAuthListMode\n";
	Logger::Case _case (m_logger, m_testp, m_step, L"TestEmptyAuthListMode");

	{//No auth
		HttpServer hs (3128, "127.0.0.1");
		hs.Start();

		TestLog testLog;
		Callbacks cbacks;
		TestCallbackInterface testCallback (&testLog, &cbacks);
		TestJournalInterface testJournal;
		Signature6Checker s6c (&testLog);

		UpdaterConfiguration data(1, 1, L"1");
		data.proxy_url = L"127.0.0.1";
		data.proxy_port = 3128;
		data.m_sourceList.addSource (UST_UserURL, L"http://httpserver", true, L"diffs");
		testCallback.SetUpdaterConfiguration (data);
		KLUPD::Source _csd (L"http://httpserver", UST_UserURL, true, L"", UPS_DIRS);
		Updater upd (testCallback.m_dp, testCallback, testJournal, &testLog);
		upd.constructSourceList(testCallback.updaterConfiguration().m_sourceList, upd.m_sourceList);

		HANDLE hDoDownload;

		string command;
		string param;
		HttpServer::CMD_TYPE type;

		{
			FileInfo _fi (L"base005.avc", L"/");
			_fi.m_localPath = L"h:\\test\\";

			_helper::DeleteDirectory(TEST_FOLDER);
			_helper::MakeDirectory(TEST_FOLDER);

			doEntryDownloader do_dld(&upd, hDoDownload, _fi, _fi.m_localPath, &s6c);

			hs.Accept();

			hs.Receive(command, type, param);
			QC_BOOST_CHECK_MESSAGE (type == HttpServer::CMD_GET, L"Error\n");
			QC_BOOST_CHECK_MESSAGE (command.find("proxy-authorization:") == wstring::npos, L"Error\n");
			hs.Send("HTTP/1.1 200 OK\x0d\x0a");
			hs.Send("Content-Length: 10\x0d\x0a");
			hs.Send("Connection: Keep-Alive\x0d\x0a");
			hs.Send("Proxy-Connection: Close\x0d\x0a");
			hs.Send("\x0d\x0a");
			hs.Send("0123456789");

			hs.CloseConnection ();

			WaitForSingleObject (hDoDownload, INFINITE);
			QC_BOOST_CHECK_MESSAGE ( CORE_NO_ERROR == do_dld.GetError(), L"Wrong return code\n" );
			QC_BOOST_CHECK_MESSAGE ( _helper::checkFileContent (wstring(TEST_FOLDER).append (L"\\base005.avc"),
				"0123456789"), L"Wrong file content\n" );
		}	
	}

	{//Basic
		HttpServer hs (3128, "127.0.0.1");
		hs.Start();

		TestLog testLog;
		Callbacks cbacks;
		TestCallbackInterface testCallback (&testLog, &cbacks);
		TestJournalInterface testJournal;
		Signature6Checker s6c (&testLog);

		UpdaterConfiguration data(1, 1, L"1");
		data.proxy_url = L"127.0.0.1";
		data.proxy_port = 3128;
		data.m_proxyAuthorizationCredentials.userName ( L"test");
		data.m_proxyAuthorizationCredentials.password ( L"test");
		data.m_sourceList.addSource (UST_UserURL, L"http://httpserver", true, L"diffs");
		testCallback.SetUpdaterConfiguration (data);
		KLUPD::Source _csd (L"http://httpserver", UST_UserURL, true, L"", UPS_DIRS);
		Updater upd (testCallback.m_dp, testCallback, testJournal, &testLog);
		upd.constructSourceList(testCallback.updaterConfiguration().m_sourceList, upd.m_sourceList);

		HANDLE hDoDownload;

		string command;
		string param;
		HttpServer::CMD_TYPE type;

		{
			FileInfo _fi (L"base005.avc", L"/");
			_fi.m_localPath = L"h:\\test\\";

			_helper::DeleteDirectory(TEST_FOLDER);
			_helper::MakeDirectory(TEST_FOLDER);

			doEntryDownloader do_dld(&upd, hDoDownload, _fi, _fi.m_localPath, &s6c);

			hs.Accept();

			hs.Receive(command, type, param);
			QC_BOOST_CHECK_MESSAGE (type == HttpServer::CMD_GET, L"Error\n");
			hs.Send("HTTP/1.1 407 Proxy Authentication Required\x0d\x0a");
			hs.Send("Proxy-Authenticate: Basic realm=\"Hello\"\x0d\x0a");
			hs.Send("Connection: Keep-Alive\x0d\x0a");
			hs.Send("Proxy-Connection: Keep-Alive\x0d\x0a");
			hs.Send("Content-Length: 0\x0d\x0a");
			hs.Send("\x0d\x0a");

			WaitForSingleObject (hDoDownload, INFINITE);
			QC_BOOST_CHECK_MESSAGE ( CORE_PROXY_AUTH_ERROR == do_dld.GetError(), L"Wrong return code\n" );
		}
	}
	
	{//NTLM
		HttpServer hs (3128, "127.0.0.1");
		hs.Start();

		TestLog testLog;
		Callbacks cbacks;
		TestCallbackInterface testCallback (&testLog, &cbacks);
		TestJournalInterface testJournal;
		Signature6Checker s6c (&testLog);

		UpdaterConfiguration data(1, 1, L"1");
		data.proxy_url = L"127.0.0.1";
		data.proxy_port = 3128;
		data.m_proxyAuthorizationCredentials.userName ( L"test");
		data.m_proxyAuthorizationCredentials.password ( L"test");
		data.m_sourceList.addSource (UST_UserURL, L"http://httpserver", true, L"diffs");
		testCallback.SetUpdaterConfiguration (data);
		KLUPD::Source _csd (L"http://httpserver", UST_UserURL, true, L"", UPS_DIRS);
		Updater upd (testCallback.m_dp, testCallback, testJournal, &testLog);
		upd.constructSourceList(testCallback.updaterConfiguration().m_sourceList, upd.m_sourceList);

		HANDLE hDoDownload;

		string command;
		string param;
		HttpServer::CMD_TYPE type;

		{
			FileInfo _fi (L"base005.avc", L"/");
			_fi.m_localPath = L"h:\\test\\";

			_helper::DeleteDirectory(TEST_FOLDER);
			_helper::MakeDirectory(TEST_FOLDER);

			doEntryDownloader do_dld(&upd, hDoDownload, _fi, _fi.m_localPath, &s6c);

			hs.Accept();

			hs.Receive(command, type, param);
			QC_BOOST_CHECK_MESSAGE (type == HttpServer::CMD_GET, L"Error\n");
			hs.Send("HTTP/1.1 407 Proxy Authentication Required\x0d\x0a");
			hs.Send("Proxy-Authenticate: NTLM\x0d\x0a");
			hs.Send("Content-Length: 0\x0d\x0a");
			hs.Send("Connection: Keep-Alive\x0d\x0a");
			hs.Send("Proxy-Connection: Keep-Alive\x0d\x0a");
			hs.Send("\x0d\x0a");

			WaitForSingleObject (hDoDownload, INFINITE);
			QC_BOOST_CHECK_MESSAGE ( CORE_PROXY_AUTH_ERROR == do_dld.GetError(), L"Wrong return code\n" );
		}
	}

}


void TestHttpProxy::TestSpecialProxyCases_404 ()
{
	using namespace KLUPD;
	std::wcout<<L"TestSpecialProxyCases_404\n";
	Logger::Case _case (m_logger, m_testp, m_step, L"TestSpecialProxyCases_404");

	{	//Basic auth - code 404 - keep-alive
		HttpServer hs (3128, "127.0.0.1");
		hs.Start();

		TestLog testLog;

		_helper::DeleteDirectory(ROOT);
		KLUPD::createFolder (BASES_ROOT, &testLog);
		KLUPD::createFolder (UPD_ROOT, &testLog);

		UpdaterConfiguration data(1, 1, L"1");
		data.proxy_url = L"127.0.0.1";
		data.proxy_port = 3128;
		data.m_proxyAuthorizationCredentials.userName ( L"test");
		data.m_proxyAuthorizationCredentials.password ( L"test");
		data.m_httpAuthorizationMethods.push_back (basicAuthorization);

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
		data.m_componentsToUpdate.push_back ( L"CORE");
		data.connect_tmo = 30;

		data.m_sourceList.addSource (UST_UserURL, L"http://bases", true, L"diffs");

		data.UpdaterListFlags.update_bases = true;

		Callbacks cbacks;
		cbacks.bFlags[Callbacks::requestCredentialsForAuthorizationOnProxy] = true;
		CREATE_EVENTS(requestCredentialsForAuthorizationOnProxy);
		TestCallbackInterface testCallback (&testLog, &cbacks);
		TestJournalInterface testJournal;
		testCallback.SetUpdaterConfiguration (data);

		Updater upd (testCallback.m_dp, testCallback, testJournal, &testLog);
		HANDLE hDoUpdate;

		doUpdater do_upd(&upd, hDoUpdate);

		//klz - tree
		hs.Accept();
		QC_BOOST_CHECK_MESSAGE (hs.ReceivePureGet(), L"Error getting request\n");
		hs.SendBasicGreeting ();
		QC_BOOST_CHECK_MESSAGE (hs.ReceiveGetWithBasicToken(), L"Error getting request\n");
		hs.Send404NotFound (HttpServer::KeepAlive, HttpServer::KeepAlive);
		//klz - plain
		QC_BOOST_CHECK_MESSAGE (hs.ReceiveGetWithBasicToken(), L"Error getting request\n");
		hs.Send404NotFound (HttpServer::KeepAlive, HttpServer::KeepAlive);
		//xml - tree
		QC_BOOST_CHECK_MESSAGE (hs.ReceiveGetWithBasicToken(), L"Error getting request\n");
		hs.Send404NotFound (HttpServer::KeepAlive, HttpServer::KeepAlive);
		//xml - plain
		QC_BOOST_CHECK_MESSAGE (hs.ReceiveGetWithBasicToken(), L"Error getting request\n");
		hs.Send404NotFound (HttpServer::KeepAlive, HttpServer::KeepAlive);

		WaitForSingleObject (hDoUpdate, INFINITE);
		QC_BOOST_CHECK_MESSAGE ( CORE_NO_SOURCE_FILE == do_upd.GetError(), L"Wrong return code\n" );
		
	}

	{	//NTLM auth - code 404 - keep-alive
		HttpServer hs (3128, "127.0.0.1");
		hs.Start();

		TestLog testLog;

		_helper::DeleteDirectory(ROOT);
		KLUPD::createFolder (BASES_ROOT, &testLog);
		KLUPD::createFolder (UPD_ROOT, &testLog);

		UpdaterConfiguration data(1, 1, L"1");
		data.proxy_url = L"127.0.0.1";
		data.proxy_port = 3128;
		data.m_proxyAuthorizationCredentials.userName ( L"test");
		data.m_proxyAuthorizationCredentials.password ( L"test");
		data.m_httpAuthorizationMethods.push_back (ntlmAuthorization);

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
		data.m_componentsToUpdate.push_back ( L"CORE");
		data.connect_tmo = 30;

		data.m_sourceList.addSource (UST_UserURL, L"http://bases", true, L"diffs");

		data.UpdaterListFlags.update_bases = true;

		Callbacks cbacks;
		cbacks.bFlags[Callbacks::requestCredentialsForAuthorizationOnProxy] = true;
		CREATE_EVENTS(requestCredentialsForAuthorizationOnProxy);
		TestCallbackInterface testCallback (&testLog, &cbacks);
		TestJournalInterface testJournal;
		testCallback.SetUpdaterConfiguration (data);

		Updater upd (testCallback.m_dp, testCallback, testJournal, &testLog);
		HANDLE hDoUpdate;

		doUpdater do_upd(&upd, hDoUpdate);

		//klz - tree
		hs.Accept();
		QC_BOOST_CHECK_MESSAGE (hs.ReceivePureGet(), L"Error getting request\n");
		hs.SendNtlmGreeting ();
		QC_BOOST_CHECK_MESSAGE (hs.ReceiveGetWithNtlmToken(), L"Error getting request\n");
		hs.SendNtlmMsg2();
		QC_BOOST_CHECK_MESSAGE (hs.ReceiveGetWithNtlmToken(), L"Error getting request\n");
		hs.Send404NotFound (HttpServer::KeepAlive, HttpServer::KeepAlive);
		//klz - plain
		QC_BOOST_CHECK_MESSAGE (hs.ReceivePureGet(), L"Error getting request\n");
		hs.Send404NotFound (HttpServer::KeepAlive, HttpServer::KeepAlive);
		//xml - tree
		QC_BOOST_CHECK_MESSAGE (hs.ReceivePureGet(), L"Error getting request\n");
		hs.Send404NotFound (HttpServer::KeepAlive, HttpServer::KeepAlive);
		//xml - plain
		QC_BOOST_CHECK_MESSAGE (hs.ReceivePureGet(), L"Error getting request\n");
		hs.Send404NotFound (HttpServer::KeepAlive, HttpServer::KeepAlive);

		WaitForSingleObject (hDoUpdate, INFINITE);
		QC_BOOST_CHECK_MESSAGE ( CORE_NO_SOURCE_FILE == do_upd.GetError(), L"Wrong return code\n" );

	}

	{	//No auth - code 404 - keep-alive
		HttpServer hs (3128, "127.0.0.1");
		hs.Start();

		TestLog testLog;

		_helper::DeleteDirectory(ROOT);
		KLUPD::createFolder (BASES_ROOT, &testLog);
		KLUPD::createFolder (UPD_ROOT, &testLog);

		UpdaterConfiguration data(1, 1, L"1");
		data.proxy_url = L"127.0.0.1";
		data.proxy_port = 3128;
		data.m_proxyAuthorizationCredentials.userName ( L"test");
		data.m_proxyAuthorizationCredentials.password ( L"test");
		data.m_httpAuthorizationMethods.push_back (ntlmAuthorization);
		data.m_httpAuthorizationMethods.push_back (basicAuthorization);

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
		data.m_componentsToUpdate.push_back ( L"CORE");
		data.connect_tmo = 30;

		data.m_sourceList.addSource (UST_UserURL, L"http://bases", true, L"diffs");

		data.UpdaterListFlags.update_bases = true;

		Callbacks cbacks;
		cbacks.bFlags[Callbacks::requestCredentialsForAuthorizationOnProxy] = true;
		CREATE_EVENTS(requestCredentialsForAuthorizationOnProxy);
		TestCallbackInterface testCallback (&testLog, &cbacks);
		TestJournalInterface testJournal;
		testCallback.SetUpdaterConfiguration (data);

		Updater upd (testCallback.m_dp, testCallback, testJournal, &testLog);
		HANDLE hDoUpdate;

		doUpdater do_upd(&upd, hDoUpdate);

		//klz - tree
		hs.Accept();
		QC_BOOST_CHECK_MESSAGE (hs.ReceivePureGet(), L"Error getting request\n");
		hs.Send404NotFound (HttpServer::KeepAlive, HttpServer::KeepAlive);
		//klz - plain
		QC_BOOST_CHECK_MESSAGE (hs.ReceivePureGet(), L"Error getting request\n");
		hs.Send404NotFound (HttpServer::KeepAlive, HttpServer::KeepAlive);
		//xml - tree
		QC_BOOST_CHECK_MESSAGE (hs.ReceivePureGet(), L"Error getting request\n");
		hs.Send404NotFound (HttpServer::KeepAlive, HttpServer::KeepAlive);
		//xml - plain
		QC_BOOST_CHECK_MESSAGE (hs.ReceivePureGet(), L"Error getting request\n");
		hs.Send404NotFound (HttpServer::KeepAlive, HttpServer::KeepAlive);

		WaitForSingleObject (hDoUpdate, INFINITE);
		QC_BOOST_CHECK_MESSAGE ( CORE_NO_SOURCE_FILE == do_upd.GetError(), L"Wrong return code\n" );
	}

	{	//Basic auth - code 404 - keep-alive
		HttpServer hs (3128, "127.0.0.1");
		hs.Start();

		TestLog testLog;

		_helper::DeleteDirectory(ROOT);
		KLUPD::createFolder (BASES_ROOT, &testLog);
		KLUPD::createFolder (UPD_ROOT, &testLog);

		UpdaterConfiguration data(1, 1, L"1");
		data.proxy_url = L"127.0.0.1";
		data.proxy_port = 3128;
		data.m_proxyAuthorizationCredentials.userName ( L"test");
		data.m_proxyAuthorizationCredentials.password ( L"test");
		data.m_httpAuthorizationMethods.push_back (basicAuthorization);

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
		data.m_componentsToUpdate.push_back ( L"CORE");
		data.connect_tmo = 30;

		data.m_sourceList.addSource (UST_UserURL, L"http://bases", true, L"diffs");

		data.UpdaterListFlags.update_bases = true;

		Callbacks cbacks;
		cbacks.bFlags[Callbacks::requestCredentialsForAuthorizationOnProxy] = true;
		CREATE_EVENTS(requestCredentialsForAuthorizationOnProxy);
		TestCallbackInterface testCallback (&testLog, &cbacks);
		TestJournalInterface testJournal;
		testCallback.SetUpdaterConfiguration (data);

		Updater upd (testCallback.m_dp, testCallback, testJournal, &testLog);
		HANDLE hDoUpdate;

		doUpdater do_upd(&upd, hDoUpdate);

		//klz - tree
		hs.Accept();
		QC_BOOST_CHECK_MESSAGE (hs.ReceivePureGet(), L"Error getting request\n");
		hs.SendBasicGreeting ();
		QC_BOOST_CHECK_MESSAGE (hs.ReceiveGetWithBasicToken(), L"Error getting request\n");
		hs.Send404NotFound (HttpServer::KeepAlive, HttpServer::Close);
		hs.CloseConnection();
		//klz - plain
		hs.Accept();
		QC_BOOST_CHECK_MESSAGE (hs.ReceiveGetWithBasicToken(), L"Error getting request\n");
		hs.Send404NotFound (HttpServer::KeepAlive, HttpServer::Close);
		hs.CloseConnection();
		//xml - tree
		hs.Accept();
		QC_BOOST_CHECK_MESSAGE (hs.ReceiveGetWithBasicToken(), L"Error getting request\n");
		hs.Send404NotFound (HttpServer::KeepAlive, HttpServer::Close);
		hs.CloseConnection();
		//xml - plain
		hs.Accept();
		QC_BOOST_CHECK_MESSAGE (hs.ReceiveGetWithBasicToken(), L"Error getting request\n");
		hs.Send404NotFound (HttpServer::KeepAlive, HttpServer::Close);
		hs.CloseConnection();

		WaitForSingleObject (hDoUpdate, INFINITE);
		QC_BOOST_CHECK_MESSAGE ( CORE_NO_SOURCE_FILE == do_upd.GetError(), L"Wrong return code\n" );

	}

	{	//Basic auth - code 404 - close
		HttpServer hs (3128, "127.0.0.1");
		hs.Start();

		TestLog testLog;

		_helper::DeleteDirectory(ROOT);
		KLUPD::createFolder (BASES_ROOT, &testLog);
		KLUPD::createFolder (UPD_ROOT, &testLog);

		UpdaterConfiguration data(1, 1, L"1");
		data.proxy_url = L"127.0.0.1";
		data.proxy_port = 3128;
		data.m_proxyAuthorizationCredentials.userName ( L"test");
		data.m_proxyAuthorizationCredentials.password ( L"test");
		data.m_httpAuthorizationMethods.push_back (basicAuthorization);

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
		data.m_componentsToUpdate.push_back ( L"CORE");
		data.connect_tmo = 30;

		data.m_sourceList.addSource (UST_UserURL, L"http://bases", true, L"diffs");

		data.UpdaterListFlags.update_bases = true;

		Callbacks cbacks;
		cbacks.bFlags[Callbacks::requestCredentialsForAuthorizationOnProxy] = true;
		CREATE_EVENTS(requestCredentialsForAuthorizationOnProxy);
		TestCallbackInterface testCallback (&testLog, &cbacks);
		TestJournalInterface testJournal;
		testCallback.SetUpdaterConfiguration (data);

		Updater upd (testCallback.m_dp, testCallback, testJournal, &testLog);
		HANDLE hDoUpdate;

		doUpdater do_upd(&upd, hDoUpdate);

		//klz - tree
		hs.Accept();
		QC_BOOST_CHECK_MESSAGE (hs.ReceivePureGet(), L"Error getting request\n");
		hs.SendBasicGreeting ();
		QC_BOOST_CHECK_MESSAGE (hs.ReceiveGetWithBasicToken(), L"Error getting request\n");
		hs.Send404NotFound (HttpServer::KeepAlive, HttpServer::Close);
		hs.CloseConnection();
		//klz - plain
		hs.Accept();
		QC_BOOST_CHECK_MESSAGE (hs.ReceiveGetWithBasicToken(), L"Error getting request\n");
		hs.Send404NotFound (HttpServer::KeepAlive, HttpServer::Close);
		hs.CloseConnection();
		//xml - tree
		hs.Accept();
		QC_BOOST_CHECK_MESSAGE (hs.ReceiveGetWithBasicToken(), L"Error getting request\n");
		hs.Send404NotFound (HttpServer::KeepAlive, HttpServer::Close);
		hs.CloseConnection();
		//xml - plain
		hs.Accept();
		QC_BOOST_CHECK_MESSAGE (hs.ReceiveGetWithBasicToken(), L"Error getting request\n");
		hs.Send404NotFound (HttpServer::KeepAlive, HttpServer::Close);
		hs.CloseConnection();

		WaitForSingleObject (hDoUpdate, INFINITE);
		QC_BOOST_CHECK_MESSAGE ( CORE_NO_SOURCE_FILE == do_upd.GetError(), L"Wrong return code\n" );

	}

	{	//NTLM auth - code 404 - close
		HttpServer hs (3128, "127.0.0.1");
		hs.Start();

		TestLog testLog;

		_helper::DeleteDirectory(ROOT);
		KLUPD::createFolder (BASES_ROOT, &testLog);
		KLUPD::createFolder (UPD_ROOT, &testLog);

		UpdaterConfiguration data(1, 1, L"1");
		data.proxy_url = L"127.0.0.1";
		data.proxy_port = 3128;
		data.m_proxyAuthorizationCredentials.userName ( L"test");
		data.m_proxyAuthorizationCredentials.password ( L"test");
		data.m_httpAuthorizationMethods.push_back (ntlmAuthorization);

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
		data.m_componentsToUpdate.push_back ( L"CORE");
		data.connect_tmo = 30;

		data.m_sourceList.addSource (UST_UserURL, L"http://bases", true, L"diffs");

		data.UpdaterListFlags.update_bases = true;

		Callbacks cbacks;
		cbacks.bFlags[Callbacks::requestCredentialsForAuthorizationOnProxy] = true;
		CREATE_EVENTS(requestCredentialsForAuthorizationOnProxy);
		TestCallbackInterface testCallback (&testLog, &cbacks);
		TestJournalInterface testJournal;
		testCallback.SetUpdaterConfiguration (data);

		Updater upd (testCallback.m_dp, testCallback, testJournal, &testLog);
		HANDLE hDoUpdate;

		doUpdater do_upd(&upd, hDoUpdate);

		//klz - tree
		hs.Accept();
		QC_BOOST_CHECK_MESSAGE (hs.ReceivePureGet(), L"Error getting request\n");
		hs.SendNtlmGreeting ();
		QC_BOOST_CHECK_MESSAGE (hs.ReceiveGetWithNtlmToken(), L"Error getting request\n");
		hs.SendNtlmMsg2();
		QC_BOOST_CHECK_MESSAGE (hs.ReceiveGetWithNtlmToken(), L"Error getting request\n");
		hs.Send404NotFound (HttpServer::KeepAlive, HttpServer::Close);
		hs.CloseConnection();
		//klz - plain
		hs.Accept();
		QC_BOOST_CHECK_MESSAGE (hs.ReceiveGetWithNtlmToken(), L"Error getting request\n");
		hs.SendNtlmMsg2();
		QC_BOOST_CHECK_MESSAGE (hs.ReceiveGetWithNtlmToken(), L"Error getting request\n");
		hs.Send404NotFound (HttpServer::KeepAlive, HttpServer::Close);
		hs.CloseConnection();
		//xml - tree
		hs.Accept();
		QC_BOOST_CHECK_MESSAGE (hs.ReceiveGetWithNtlmToken(), L"Error getting request\n");
		hs.SendNtlmMsg2();
		QC_BOOST_CHECK_MESSAGE (hs.ReceiveGetWithNtlmToken(), L"Error getting request\n");
		hs.Send404NotFound (HttpServer::KeepAlive, HttpServer::Close);
		hs.CloseConnection();
		//xml - plain
		hs.Accept();
		QC_BOOST_CHECK_MESSAGE (hs.ReceiveGetWithNtlmToken(), L"Error getting request\n");
		hs.SendNtlmMsg2();
		QC_BOOST_CHECK_MESSAGE (hs.ReceiveGetWithNtlmToken(), L"Error getting request\n");
		hs.Send404NotFound (HttpServer::KeepAlive, HttpServer::Close);
		hs.CloseConnection();

		WaitForSingleObject (hDoUpdate, INFINITE);
		QC_BOOST_CHECK_MESSAGE ( CORE_NO_SOURCE_FILE == do_upd.GetError(), L"Wrong return code\n" );

	}


}

void TestHttpProxy::TestSpecialProxyCases_Redirect ()
{
	using namespace KLUPD;
	std::wcout<<L"TestSpecialProxyCases_Redirect\n";
	Logger::Case _case (m_logger, m_testp, m_step, L"TestSpecialProxyCases_Redirect");

	{	//bug 24347
		//authorization request was even for Domain User, NTLM authorization
		//state error in case HTTP redirect AND close connection happens
		HttpServer hs (3128, "127.0.0.1");
		hs.Start();

		TestLog testLog;

		_helper::DeleteDirectory(ROOT);
		KLUPD::createFolder (BASES_ROOT, &testLog);
		KLUPD::createFolder (UPD_ROOT, &testLog);

		UpdaterConfiguration data(1, 1, L"1");
		data.proxy_url = L"127.0.0.1";
		data.proxy_port = 3128;
		data.m_proxyAuthorizationCredentials.userName ( L"");
		data.m_proxyAuthorizationCredentials.password ( L"");
		data.m_httpAuthorizationMethods.push_back (ntlmAuthorization);
		data.m_httpAuthorizationMethods.push_back (ntlmAuthorizationWithCredentials);
		data.m_httpAuthorizationMethods.push_back (basicAuthorization);

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
		data.m_componentsToUpdate.push_back ( L"CORE");
		data.connect_tmo = 30;

		data.m_sourceList.addSource (UST_UserURL, L"http://bases", true, L"diffs");

		data.UpdaterListFlags.update_bases = true;

		Callbacks cbacks;
		cbacks.bFlags[Callbacks::requestCredentialsForAuthorizationOnProxy] = true;
		CREATE_EVENTS(requestCredentialsForAuthorizationOnProxy);
		TestCallbackInterface testCallback (&testLog, &cbacks);
		TestJournalInterface testJournal;
		testCallback.SetUpdaterConfiguration (data);

		Updater upd (testCallback.m_dp, testCallback, testJournal, &testLog);
		HANDLE hDoUpdate;

		doUpdater do_upd(&upd, hDoUpdate);

		//klz
		hs.Accept();
		QC_BOOST_CHECK_MESSAGE (hs.ReceivePureGet("host: bases"), L"Error getting request\n");
		hs.SendNtlmBasicGreeting ();
		QC_BOOST_CHECK_MESSAGE (hs.ReceiveGetWithNtlmToken("host: bases"), L"Error getting request\n");
		hs.SendNtlmMsg2();
		QC_BOOST_CHECK_MESSAGE (hs.ReceiveGetWithNtlmToken("host: bases"), L"Error getting request\n");
		hs.Send302Redirected (HttpServer::KeepAlive, HttpServer::Close, "http://kiryukhin-xp/v_u0607g.xml.klz");
		hs.CloseConnection();
//bug was here
		hs.Accept();
		QC_BOOST_CHECK_MESSAGE (hs.ReceiveGetWithNtlmToken("host: kiryukhin-xp"), L"Error getting request\n");
		hs.SendNtlmMsg2();
		QC_BOOST_CHECK_MESSAGE (hs.ReceiveGetWithNtlmToken("host: kiryukhin-xp"), L"Error getting request\n");
		hs.SendFileFromBuffer (HttpServer::KeepAlive, HttpServer::KeepAlive, "1111");

		//xml
		QC_BOOST_CHECK_MESSAGE (hs.ReceivePureGet("host: bases"), L"Error getting request\n");
		hs.Send302Redirected (HttpServer::KeepAlive, HttpServer::KeepAlive, "http://kiryukhin-xp/v_u0607g.xml");
		QC_BOOST_CHECK_MESSAGE (hs.ReceivePureGet("host: kiryukhin-xp"), L"Error getting request\n");
		hs.SendFileFromBuffer (HttpServer::KeepAlive, HttpServer::KeepAlive, "1111");
		hs.CloseConnection ();


		WaitForSingleObject (hDoUpdate, INFINITE);
		QC_BOOST_CHECK_MESSAGE ( CORE_INVALID_SIGNATURE == do_upd.GetError(), L"Wrong return code\n" );

	}

}

void TestHttpProxy::TestExhaustingAuthTypes ()
{
	using namespace KLUPD;
	std::wcout<<L"TestExhaustingAuthTypes\n";
	Logger::Case _case (m_logger, m_testp, m_step, L"TestExhaustingAuthTypes");

	HttpServer hs (3128, "127.0.0.1");
	hs.Start();

	TestLog testLog;

	_helper::DeleteDirectory(ROOT);
	KLUPD::createFolder (BASES_ROOT, &testLog);
	KLUPD::createFolder (UPD_ROOT, &testLog);

	UpdaterConfiguration data(1, 1, L"1");
	data.proxy_url = L"127.0.0.1";
	data.proxy_port = 3128;
	data.m_proxyAuthorizationCredentials.userName ( L"fakeuser");
	data.m_proxyAuthorizationCredentials.password ( L"fakepwd");
	data.m_httpAuthorizationMethods.push_back (ntlmAuthorization);
	data.m_httpAuthorizationMethods.push_back (ntlmAuthorizationWithCredentials);
	data.m_httpAuthorizationMethods.push_back (basicAuthorization);

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
	data.m_componentsToUpdate.push_back (NoCaseString(L"CORE") );
	data.connect_tmo = 30;

	data.m_sourceList.addSource (UST_UserURL, L"http://bases", true, L"diffs");

	data.UpdaterListFlags.update_bases = true;

	Callbacks cbacks;
	cbacks.bFlags[Callbacks::requestCredentialsForAuthorizationOnProxy] = true;
	CREATE_EVENTS(requestCredentialsForAuthorizationOnProxy);
	TestCallbackInterface testCallback (&testLog, &cbacks);
	TestJournalInterface testJournal;
	testCallback.SetUpdaterConfiguration (data);

	TestCallbackInterface::_credentials creds;
	creds.name = L"test";
	creds.password = L"test";
	testCallback.SetRequestCreds(creds);

	Updater upd (testCallback.m_dp, testCallback, testJournal, &testLog);
	HANDLE hDoUpdate;

	doUpdater do_upd(&upd, hDoUpdate);
	//u0607

	//klz
	//ntlm failed
	hs.Accept();
	QC_BOOST_CHECK_MESSAGE (hs.ReceivePureGet("u0607g.xml.klz"), L"Error getting request\n");
	hs.SendNtlmBasicGreeting ();
	QC_BOOST_CHECK_MESSAGE (hs.ReceiveGetWithNtlmToken("u0607g.xml.klz"), L"Error getting request\n");
	hs.SendNtlmMsg2();
	QC_BOOST_CHECK_MESSAGE (hs.ReceiveGetWithNtlmToken("u0607g.xml.klz"), L"Error getting request\n");
	hs.Send502Denied(HttpServer::KeepAlive, HttpServer::Close);
	//ntlm with creds succeeded
	hs.Accept();
	QC_BOOST_CHECK_MESSAGE (hs.ReceiveGetWithNtlmToken("u0607g.xml.klz"), L"Error getting request\n");
	hs.SendNtlmMsg2();
	QC_BOOST_CHECK_MESSAGE (hs.ReceiveGetWithNtlmToken("u0607g.xml.klz"), L"Error getting request\n");
	hs.SendFileFromFile (HttpServer::KeepAlive, HttpServer::Close, std::wstring(COLL_ROOT).append(L"\\Bases_1\\data\\u0607g.xml"));
	hs.CloseConnection();

	//xml
	//ntlm with creds failed
	hs.Accept();
	QC_BOOST_CHECK_MESSAGE (hs.ReceiveGetWithNtlmToken("u0607g.xml"), L"Error getting request\n");
	hs.SendNtlmMsg2();
	QC_BOOST_CHECK_MESSAGE (hs.ReceiveGetWithNtlmToken("u0607g.xml"), L"Error getting request\n");
	hs.Send502Denied(HttpServer::KeepAlive, HttpServer::Close);
	//basic succeeded
	hs.Accept();
	QC_BOOST_CHECK_MESSAGE (hs.ReceiveGetWithBasicToken("u0607g.xml"), L"Error getting request\n");
	hs.SendFileFromFile (HttpServer::KeepAlive, HttpServer::Close, std::wstring(COLL_ROOT).append(L"\\Bases_1\\data\\u0607g.xml"));
	hs.CloseConnection();

	//av-i386
	//klz
	//basic failed
	hs.Accept();
	QC_BOOST_CHECK_MESSAGE (hs.ReceiveGetWithBasicToken("av-i386-0607g.xml.klz"), L"Error getting request\n");
	hs.SendFileFromFile (HttpServer::KeepAlive, HttpServer::Close, std::wstring(COLL_ROOT).append(L"\\Bases_1\\plain\\av-i386-0607g.xml"));
	hs.CloseConnection();


	//av-i386
	//xml
	//basic failed
	hs.Accept();
	QC_BOOST_CHECK_MESSAGE (hs.ReceiveGetWithBasicToken("av-i386-0607g.xml"), L"Error getting request\n");
	hs.Send502Denied(HttpServer::KeepAlive, HttpServer::Close);
	hs.CloseConnection();
	creds.result = false;
	testCallback.SetRequestCreds(creds);
	WAIT_FOR_READY_EVENT(requestCredentialsForAuthorizationOnProxy)
	SET_GOON_EVENT(requestCredentialsForAuthorizationOnProxy);
	
	WaitForSingleObject (hDoUpdate, INFINITE);
	QC_BOOST_CHECK_MESSAGE ( CORE_PROXY_AUTH_ERROR == do_upd.GetError(), L"Wrong return code\n" );
}


void TestHttpProxy::TestGetFile_IpV6 ()
{
	using namespace KLUPD;
	std::wcout<<L"TestGetFile_IpV6\n";
	Logger::Case _case (m_logger, m_testp, m_step, L"TestGetFile_IpV6");

	{//variant 1 - proxy-connection close
		HttpServer hs (3128, IP_V6_ADDR_1, IP_V6);
		hs.Start();


		TestLog testLog;
		Callbacks cbacks;
		TestCallbackInterface testCallback (&testLog, &cbacks);
		TestJournalInterface testJournal;
		Signature6Checker s6c (&testLog);

		UpdaterConfiguration data(1, 1, L"1");
		data.proxy_url = IP_V6_ADDR_1_WSTR;
		data.proxy_port = 3128;
		data.m_proxyAuthorizationCredentials.userName ( L"test");
		data.m_proxyAuthorizationCredentials.password ( L"test");
		data.m_httpAuthorizationMethods.push_back (basicAuthorization);
		data.m_sourceList.addSource (UST_UserURL, L"http://[dead::c597]", true, L"diffs");
		testCallback.SetUpdaterConfiguration (data);
		KLUPD::Source _csd (L"http://httpserver", UST_UserURL, true, L"", UPS_DIRS);
		Updater upd (testCallback.m_dp, testCallback, testJournal, &testLog);
		upd.constructSourceList(testCallback.updaterConfiguration().m_sourceList, upd.m_sourceList);


		HANDLE hDoDownload;


		{

			FileInfo _fi (L"base005.avc", L"/");
			_fi.m_localPath = L"h:\\test\\";

			_helper::DeleteDirectory(TEST_FOLDER);
			_helper::MakeDirectory(TEST_FOLDER);

			doEntryDownloader do_dld(&upd, hDoDownload, _fi, _fi.m_localPath, &s6c);

			hs.Accept();

			QC_BOOST_CHECK_MESSAGE (hs.ReceivePureGet(), L"Error getting request\n");

			hs.SendBasicGreeting ();

			QC_BOOST_CHECK_MESSAGE (hs.ReceiveGetWithBasicToken(), L"Error getting request\n");
			hs.SendFileFromBuffer (HttpServer::KeepAlive, HttpServer::Close, "0123456789");
			hs.CloseConnection ();

			WaitForSingleObject (hDoDownload, INFINITE);
			QC_BOOST_CHECK_MESSAGE ( CORE_NO_ERROR == do_dld.GetError(), L"Wrong return code\n" );
			QC_BOOST_CHECK_MESSAGE ( _helper::checkFileContent (wstring(TEST_FOLDER).append (L"\\base005.avc"),
				"0123456789"), L"Wrong file content\n" );
		}

		{

			FileInfo _fi (L"base004.avc", L"/");
			_fi.m_localPath = L"h:\\test\\";

			_helper::DeleteDirectory(TEST_FOLDER);
			_helper::MakeDirectory(TEST_FOLDER);

			doEntryDownloader do_dld(&upd, hDoDownload, _fi, _fi.m_localPath, &s6c);

			hs.Accept();
			QC_BOOST_CHECK_MESSAGE (hs.ReceiveGetWithBasicToken(), L"Error getting request\n");
			hs.SendFileFromBuffer (HttpServer::KeepAlive, HttpServer::Close, "0123456789");
			hs.CloseConnection ();

			WaitForSingleObject (hDoDownload, INFINITE);
			QC_BOOST_CHECK_MESSAGE ( CORE_NO_ERROR == do_dld.GetError(), L"Wrong return code\n" );
			QC_BOOST_CHECK_MESSAGE ( _helper::checkFileContent (wstring(TEST_FOLDER).append (L"\\base004.avc"),
				"0123456789"), L"Wrong file content\n" );
		}

		{

			FileInfo _fi (L"base003.avc", L"/");
			_fi.m_localPath = L"h:\\test\\";

			_helper::DeleteDirectory(TEST_FOLDER);
			_helper::MakeDirectory(TEST_FOLDER);

			doEntryDownloader do_dld(&upd, hDoDownload, _fi, _fi.m_localPath, &s6c);

			hs.Accept();
			QC_BOOST_CHECK_MESSAGE (hs.ReceiveGetWithBasicToken(), L"Error getting request\n");
			hs.SendFileFromBuffer (HttpServer::KeepAlive, HttpServer::Close, "0123456789");
			hs.CloseConnection ();

			WaitForSingleObject (hDoDownload, INFINITE);
			QC_BOOST_CHECK_MESSAGE ( CORE_NO_ERROR == do_dld.GetError(), L"Wrong return code\n" );
			QC_BOOST_CHECK_MESSAGE ( _helper::checkFileContent (wstring(TEST_FOLDER).append (L"\\base003.avc"),
				"0123456789"), L"Wrong file content\n" );
		}

	}
}