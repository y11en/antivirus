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
#include "TestHttpTransport.h"
#include "main.h"
#include "HttpServer.h"
#include "constants.h"

void TestHttpTransport::TestGetFile ()
{
	using namespace KLUPD;
	std::wcout<<L"TestGetFile\n";
	Logger::Case _case (m_logger, m_testp, m_step, L"TestGetFile");

	{//simple file getting
		HttpServer hs (8081, "127.0.0.1");
		hs.Start();

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
		data.m_sourceList.addSource (UST_UserURL, L"http://127.0.0.1:8081", false, L"");
		testCallback.SetUpdaterConfiguration (data);

		Updater upd (testCallback.m_dp, testCallback, testJournal, &testLog);
		upd.constructSourceList(testCallback.updaterConfiguration().m_sourceList, upd.m_sourceList);

		HANDLE hDoDownload;
		doEntryDownloader do_dld(&upd, hDoDownload, _fi, _fi.m_localPath, &s6c);

		string command;
		string param;
		HttpServer::CMD_TYPE type;

		hs.Accept();
	
		hs.Receive(command, type, param);
		QC_BOOST_CHECK_MESSAGE (type == HttpServer::CMD_GET, L"Error\n");
		hs.Send("HTTP/1.1 200 OK\x0d\x0a");
		hs.Send("Content-Length: 10\x0d\x0a");
		hs.Send("Connection: Keep-Alive\x0d\x0a");
		hs.Send("\x0d\x0a");
		hs.Send("0123456789");
		
		hs.CloseConnection ();
		
		WaitForSingleObject (hDoDownload, INFINITE);
		QC_BOOST_CHECK_MESSAGE ( CORE_NO_ERROR == do_dld.GetError(), L"Wrong return code\n" );
		QC_BOOST_CHECK_MESSAGE ( _helper::checkFileContent (wstring(TEST_FOLDER).append (L"\\base005.avc"),
										"0123456789"), L"Wrong file content\n" );
		
	}

}

void TestHttpTransport::TestGetFileWithRest ()
{
	using namespace KLUPD;
	std::wcout<<L"TestGetFileWithRest\n";
	Logger::Case _case (m_logger, m_testp, m_step, L"TestGetFileWithRest");

	{//simple file getting with restarting
		HttpServer hs (8081, "127.0.0.1");
		hs.Start();

		_helper::DeleteDirectory(TEST_FOLDER);
		_helper::MakeDirectory(TEST_FOLDER);


		FileInfo _fi (L"base005.avc", L"/");
		_fi.m_localPath = L"h:\\test\\";
		FILE* file = _wfopen (wstring(TEST_FOLDER).append(L"\\base005.avc").c_str(), L"w");
		if (file == NULL)  throw std::runtime_error("");
		fwrite ("012345", 1, 6, file);
		fclose (file);
		
		TestLog testLog;
		Callbacks cbacks;
		TestCallbackInterface testCallback (&testLog, &cbacks);
		TestJournalInterface testJournal;
		Signature6Checker s6c (&testLog);
	
		UpdaterConfiguration data(1, 1, L"1");
		data.m_sourceList.addSource (UST_UserURL, L"http://127.0.0.1:8081", false, L"");
		testCallback.SetUpdaterConfiguration (data);

		Updater upd (testCallback.m_dp, testCallback, testJournal, &testLog);
		upd.constructSourceList(testCallback.updaterConfiguration().m_sourceList, upd.m_sourceList);

		HANDLE hDoDownload;
		doEntryDownloader do_dld(&upd, hDoDownload, _fi, _fi.m_localPath, &s6c);

		string command;
		string param;
		HttpServer::CMD_TYPE type;

		hs.Accept();
	
		hs.Receive(command, type, param);
		QC_BOOST_CHECK_MESSAGE (type == HttpServer::CMD_GET, L"Error\n");
		QC_BOOST_CHECK_MESSAGE (command.find ("range: bytes=6-") != wstring::npos, L"Error\n");
		hs.Send("HTTP/1.1 206 Partial content\x0d\x0a");
		hs.Send("Content-Length: 4\x0d\x0a");
		hs.Send("Content-Range: bytes 6-9/10\x0d\x0a");
		hs.Send("Connection: Keep-Alive\x0d\x0a");
		hs.Send("\x0d\x0a");
		hs.Send("6789");
		
		hs.CloseConnection ();
		
		WaitForSingleObject (hDoDownload, INFINITE);
		QC_BOOST_CHECK_MESSAGE ( CORE_NO_ERROR == do_dld.GetError(), L"Wrong return code\n" );
		QC_BOOST_CHECK_MESSAGE ( _helper::checkFileContent (wstring(TEST_FOLDER).append (L"\\base005.avc"),
										"0123456789"), L"Wrong file content\n" );
		
	}

}
/* DoGetRemoteFile_NewNet
void TestHttpTransport::TestGetFileWithRestCode200 ()
{
	using namespace KLUPD;
	std::wcout<<L"TestGetFileWithRestCode200\n";
	Logger::Case _case (m_logger, m_testp, m_step, L"TestGetFileWithRestCode200");

	{//simple file getting with restarting
		HttpServer hs (8081, "127.0.0.1");
		hs.Start();

		_helper::DeleteDirectory(TEST_FOLDER);
		_helper::MakeDirectory(TEST_FOLDER);


		FileInfo _fi (L"base005.avc", L"/");
		_fi.m_localPath = L"h:\\test\\";
		FILE* file = _wfopen (wstring(TEST_FOLDER).append(L"\\base005.avc").c_str(), L"w");
		if (file == NULL)  throw std::runtime_error("");
		fwrite ("012345", 1, 6, file);
		fclose (file);
		
		TestLog testLog;
		Callbacks cbacks;
		TestCallbackInterface testCallback (&testLog, &cbacks);
		TestJournalInterface testJournal;
		Signature6Checker s6c (&testLog);
	
		UpdaterConfiguration data(1, 1, L"1");
		testCallback.SetUpdaterConfiguration (data);
		KLUPD::Source _csd (L"http://127.0.0.1:8081", UST_UserURL, false, L"", UPS_DIRS);
		//KLUPD::Source _csd (L"http://tl-srv-wnt", UST_UserURL, false, L"", UPS_DIRS);
		
		Updater upd (testCallback.m_dp, testCallback, testJournal, &testLog);
		HANDLE hDoDownload;
		doNewNetDownloader do_dld(&upd, hDoDownload, _fi, _fi.m_relativeURLPath, L"h:\\test\\", _csd, &s6c);
		
		string command;
		string param;
		HttpServer::CMD_TYPE type;

		hs.Accept();
	
		hs.Receive(command, type, param);
		QC_BOOST_CHECK_MESSAGE (type == HttpServer::CMD_GET, L"Error\n");
		QC_BOOST_CHECK_MESSAGE (command.find ("range: bytes=6-") != wstring::npos, L"Error\n");
		hs.Send("HTTP/1.1 200 OK\x0d\x0a");
		hs.Send("Content-Length: 10\x0d\x0a");
		hs.Send("Connection: Keep-Alive\x0d\x0a");
		hs.Send("\x0d\x0a");
		hs.Send("0123456789");
		
		hs.CloseConnection ();
		
		WaitForSingleObject (hDoDownload, INFINITE);
		QC_BOOST_CHECK_MESSAGE ( CORE_NO_ERROR == do_dld.GetError(), L"Wrong return code\n" );
		QC_BOOST_CHECK_MESSAGE ( _helper::checkFileContent (wstring(TEST_FOLDER).append (L"\\base005.avc"),
										"0123456789"), L"Wrong file content\n" );
		
	}

}
*/

void TestHttpTransport::TestGetFileWithRestCode200 ()
{
	using namespace KLUPD;
	std::wcout<<L"TestGetFileWithRestCode200\n";
	Logger::Case _case (m_logger, m_testp, m_step, L"TestGetFileWithRestCode200");
	
	//bug 22574
	{//simple file getting with restarting
		HttpServer hs (8081, "127.0.0.1");
		hs.Start();

		_helper::DeleteDirectory(TEST_FOLDER);
		_helper::MakeDirectory(TEST_FOLDER);


		FileInfo _fi (L"base005.avc", L"/");
		_fi.m_localPath = L"h:\\test\\";
		FILE* file = _wfopen (wstring(TEST_FOLDER).append(L"\\base005.avc").c_str(), L"w");
		if (file == NULL)  throw std::runtime_error("");
		fwrite ("012345", 1, 6, file);
		fclose (file);
		
		TestLog testLog;
		Callbacks cbacks;
		TestCallbackInterface testCallback (&testLog, &cbacks);
		TestJournalInterface testJournal;
		Signature6Checker s6c (&testLog);
	
		UpdaterConfiguration data(1, 1, L"1");
		data.m_sourceList.addSource (UST_UserURL, L"http://127.0.0.1:8081", false, L"diffs");
		testCallback.SetUpdaterConfiguration (data);

		Updater upd (testCallback.m_dp, testCallback, testJournal, &testLog);
		upd.constructSourceList(testCallback.updaterConfiguration().m_sourceList, upd.m_sourceList);

		HANDLE hDoDownload;
		doEntryDownloader do_dld(&upd, hDoDownload, _fi, _fi.m_localPath, &s6c);
		
		string command;
		string param;
		HttpServer::CMD_TYPE type;

		hs.Accept();
		hs.Receive(command, type, param);
		QC_BOOST_CHECK_MESSAGE (type == HttpServer::CMD_GET, L"Error\n");
		QC_BOOST_CHECK_MESSAGE (command.find ("range: bytes=6-") != wstring::npos, L"Error\n");
		hs.Send("HTTP/1.1 200 OK\x0d\x0a");
		hs.Send("Content-Length: 10\x0d\x0a");
		hs.Send("Connection: Keep-Alive\x0d\x0a");
		hs.Send("\x0d\x0a");
		hs.Send("0123456789");
		hs.CloseConnection ();

		hs.Accept();
		hs.Receive(command, type, param);
		QC_BOOST_CHECK_MESSAGE (type == HttpServer::CMD_GET, L"Error\n");
		QC_BOOST_CHECK_MESSAGE (command.find ("range: ") == wstring::npos, L"Error\n");
		hs.Send("HTTP/1.1 200 OK\x0d\x0a");
		hs.Send("Content-Length: 10\x0d\x0a");
		hs.Send("Connection: Keep-Alive\x0d\x0a");
		hs.Send("\x0d\x0a");
		hs.Send("0123456789");
		hs.CloseConnection ();

		WaitForSingleObject (hDoDownload, INFINITE);
		QC_BOOST_CHECK_MESSAGE ( CORE_NO_ERROR == do_dld.GetError(), L"Wrong return code\n" );
		QC_BOOST_CHECK_MESSAGE ( _helper::checkFileContent (wstring(TEST_FOLDER).append (L"\\base005.avc"),
								"0123456789"), L"Wrong file content\n" );
		
	}

}

void TestHttpTransport::TestGetFileWithRestCode404 ()
{
	using namespace KLUPD;
	std::wcout<<L"TestGetFileWithRestCode404\n";
	Logger::Case _case (m_logger, m_testp, m_step, L"TestGetFileWithRestCode404");

	{//simple file getting with restarting
		HttpServer hs (8081, "127.0.0.1");
		hs.Start();

		_helper::DeleteDirectory(TEST_FOLDER);
		_helper::MakeDirectory(TEST_FOLDER);


		char buf[50000];
		FileInfo _fi (L"base005.avc", L"/");
		_fi.m_localPath = L"h:\\test\\";
		FILE* file = _wfopen (wstring(TEST_FOLDER).append(L"\\base005.avc").c_str(), L"w");
		if (file == NULL)  throw std::runtime_error("");
		fwrite (buf, 1, 50000, file);
		fclose (file);
		
		TestLog testLog;
		Callbacks cbacks;
		TestCallbackInterface testCallback (&testLog, &cbacks);
		TestJournalInterface testJournal;
		Signature6Checker s6c (&testLog);
	
		UpdaterConfiguration data(1, 1, L"1");
		data.m_sourceList.addSource (UST_UserURL, L"http://127.0.0.1:8081", false, L"diffs");
		testCallback.SetUpdaterConfiguration (data);
		Updater upd (testCallback.m_dp, testCallback, testJournal, &testLog);
		KLUPD::Source _csd (L"http://127.0.0.1:8081", UST_UserURL, false, L"", UPS_DIRS);
		upd.constructSourceList(testCallback.updaterConfiguration().m_sourceList, upd.m_sourceList);
		
		
		HANDLE hDoDownload;
		doEntryDownloader do_dld(&upd, hDoDownload, _fi, _fi.m_localPath, &s6c);
		
		string command;
		string param;
		HttpServer::CMD_TYPE type;

		hs.Accept();
	
		hs.Receive(command, type, param);
		QC_BOOST_CHECK_MESSAGE (type == HttpServer::CMD_GET, L"Error\n");
		QC_BOOST_CHECK_MESSAGE (command.find ("range: bytes=50000-") != wstring::npos, L"Error\n");
		
		hs.Send("HTTP/1.1 404 Not found\x0d\x0a");
		hs.Send("\x0d\x0a");
 
		hs.CloseConnection ();
		hs.Accept();

		hs.Receive(command, type, param);
		QC_BOOST_CHECK_MESSAGE (type == HttpServer::CMD_GET, L"Error\n");
		QC_BOOST_CHECK_MESSAGE (command.find ("range: bytes=") == wstring::npos, L"Error\n");

		hs.Send("HTTP/1.1 404 Not found\x0d\x0a");
		hs.Send("\x0d\x0a");
		
		hs.CloseConnection ();
		
		WaitForSingleObject (hDoDownload, INFINITE);
		QC_BOOST_CHECK_MESSAGE ( CORE_NO_SOURCE_FILE == do_dld.GetError(), L"Wrong return code\n" );
		QC_BOOST_CHECK_MESSAGE ( !_helper::AutoFile (wstring(TEST_FOLDER).append (L"\\base005.avc")).Exist(),
										L"Wrong file content\n" );
		
	}

}

void TestHttpTransport::TestGetFileWithRestCode416 ()
{
	using namespace KLUPD;
	std::wcout<<L"TestGetFileWithRestCode416\n";
	Logger::Case _case (m_logger, m_testp, m_step, L"TestGetFileWithRestCode416");

	{//simple file getting with restarting
		HttpServer hs (8081, "127.0.0.1");
		hs.Start();

		_helper::DeleteDirectory(TEST_FOLDER);
		_helper::MakeDirectory(TEST_FOLDER);


		char buf[50000];
		FileInfo _fi (L"base005.avc", L"/");
		_fi.m_localPath = L"h:\\test\\";
		FILE* file = _wfopen (wstring(TEST_FOLDER).append(L"\\base005.avc").c_str(), L"w");
		if (file == NULL)  throw std::runtime_error("");
		fwrite (buf, 1, 50000, file);
		fclose (file);
		
		TestLog testLog;
		Callbacks cbacks;
		TestCallbackInterface testCallback (&testLog, &cbacks);
		TestJournalInterface testJournal;
		Signature6Checker s6c (&testLog);
	
		UpdaterConfiguration data(1, 1, L"1");
		data.m_sourceList.addSource (UST_UserURL, L"http://127.0.0.1:8081", false, L"diffs");
		testCallback.SetUpdaterConfiguration (data);
		Updater upd (testCallback.m_dp, testCallback, testJournal, &testLog);
		KLUPD::Source _csd (L"http://127.0.0.1:8081", UST_UserURL, false, L"", UPS_DIRS);
		upd.constructSourceList(testCallback.updaterConfiguration().m_sourceList, upd.m_sourceList);
		
		
		HANDLE hDoDownload;
		doEntryDownloader do_dld(&upd, hDoDownload, _fi, _fi.m_localPath, &s6c);
		
		string command;
		string param;
		HttpServer::CMD_TYPE type;

		hs.Accept();
	
		hs.Receive(command, type, param);
		QC_BOOST_CHECK_MESSAGE (type == HttpServer::CMD_GET, L"Error\n");
		QC_BOOST_CHECK_MESSAGE (command.find ("range: bytes=50000-") != wstring::npos, L"Error\n");
		
		hs.Send("HTTP/1.1 416 Requested Range Not Satisfiable\x0d\x0a");
		hs.Send("Content-Length: 0\x0d\x0a");
		hs.Send("Connection: Keep-Alive\x0d\x0a");
		hs.Send("Content-Range: bytes */10\x0d\x0a");
		hs.Send("\x0d\x0a");

		hs.CloseConnection ();
		hs.Accept();

		hs.Receive(command, type, param);
		QC_BOOST_CHECK_MESSAGE (type == HttpServer::CMD_GET, L"Error\n");
		QC_BOOST_CHECK_MESSAGE (command.find ("range: bytes=") == wstring::npos, L"Error\n");

		hs.Send("HTTP/1.1 200 OK\x0d\x0a");
		hs.Send("Content-Length: 10\x0d\x0a");
		hs.Send("Connection: Keep-Alive\x0d\x0a");
		hs.Send("\x0d\x0a");
		hs.Send("0123456789");
		
		hs.CloseConnection ();
		
		WaitForSingleObject (hDoDownload, INFINITE);
		QC_BOOST_CHECK_MESSAGE ( CORE_NO_ERROR == do_dld.GetError(), L"Wrong return code\n" );
		QC_BOOST_CHECK_MESSAGE ( _helper::checkFileContent (wstring(TEST_FOLDER).append (L"\\base005.avc"),
										"0123456789"), L"Wrong file content\n" );
		
	}

}

void TestHttpTransport::TestGetFileNoContentLength ()
{
	using namespace KLUPD;
	std::wcout<<L"TestGetFileNoContentLength\n";
	Logger::Case _case (m_logger, m_testp, m_step, L"TestGetFileNoContentLength");

	{//signaling by closing connection
		HttpServer hs (8081, "127.0.0.1");
		hs.Start();

		TestLog testLog;
		Callbacks cbacks;
		TestCallbackInterface testCallback (&testLog, &cbacks);
		TestJournalInterface testJournal;
		Signature6Checker s6c (&testLog);

		UpdaterConfiguration data(1, 1, L"1");
		data.m_sourceList.addSource (UST_UserURL, L"http://127.0.0.1:8081", false, L"");
		testCallback.SetUpdaterConfiguration (data);

		Updater upd (testCallback.m_dp, testCallback, testJournal, &testLog);
		upd.constructSourceList(testCallback.updaterConfiguration().m_sourceList, upd.m_sourceList);

		HANDLE hDoDownload;

		string command;
		string param;
		HttpServer::CMD_TYPE type;

		{

			_helper::DeleteDirectory(TEST_FOLDER);
			_helper::MakeDirectory(TEST_FOLDER);
			
			FileInfo _fi (L"base005.avc", L"/");
			_fi.m_localPath = L"h:\\test\\";

			//doNewNetDownloader do_dld(&upd, hDoDownload, _fi, _fi.m_relativeURLPath, L"h:\\test\\", _csd, &s6c);
			doEntryDownloader do_dld(&upd, hDoDownload, _fi, _fi.m_localPath, &s6c);
			
			hs.Accept();
		
			hs.Receive(command, type, param);
			QC_BOOST_CHECK_MESSAGE (type == HttpServer::CMD_GET, L"Error\n");
			hs.Send("HTTP/1.1 200 OK\x0d\x0a");
			hs.Send("Connection: close\x0d\x0a");
			hs.Send("\x0d\x0a");
			hs.Send("0123456789ABCDEF");
			
			hs.CloseConnection ();
			
			WaitForSingleObject (hDoDownload, INFINITE);
			QC_BOOST_CHECK_MESSAGE ( CORE_NO_ERROR == do_dld.GetError(), L"Wrong return code\n" );
			QC_BOOST_CHECK_MESSAGE ( _helper::checkFileContent (wstring(TEST_FOLDER).append (L"\\base005.avc"),
											"0123456789ABCDEF"), L"Wrong file content\n" );
		}
		{

			_helper::DeleteDirectory(TEST_FOLDER);
			_helper::MakeDirectory(TEST_FOLDER);
			
			FileInfo _fi (L"base004.avc", L"/");
			_fi.m_localPath = L"h:\\test\\";

			//doNewNetDownloader do_dld(&upd, hDoDownload, _fi, _fi.m_relativeURLPath, L"h:\\test\\", _csd, &s6c);
			doEntryDownloader do_dld(&upd, hDoDownload, _fi, _fi.m_localPath, &s6c);
			
			hs.Accept();
		
			hs.Receive(command, type, param);
			QC_BOOST_CHECK_MESSAGE (type == HttpServer::CMD_GET, L"Error\n");
			hs.Send("HTTP/1.1 200 OK\x0d\x0a");
			hs.Send("Connection: close\x0d\x0a");
			hs.Send("\x0d\x0a");
			hs.Send("0123456789abcdef");
			
			hs.CloseConnection ();
			
			WaitForSingleObject (hDoDownload, INFINITE);
			QC_BOOST_CHECK_MESSAGE ( CORE_NO_ERROR == do_dld.GetError(), L"Wrong return code\n" );
			QC_BOOST_CHECK_MESSAGE ( _helper::checkFileContent (wstring(TEST_FOLDER).append (L"\\base004.avc"),
											"0123456789abcdef"), L"Wrong file content\n" );
		}
		
	}

	QC_BOOST_REQUIRE_MESSAGE (false, L"Bug\n");

	{//signaling by by byte-range
		HttpServer hs (8081, "127.0.0.1");
		hs.Start();

		_helper::DeleteDirectory(TEST_FOLDER);
		_helper::MakeDirectory(TEST_FOLDER);
		
		TestLog testLog;
		Callbacks cbacks;
		TestCallbackInterface testCallback (&testLog, &cbacks);
		TestJournalInterface testJournal;
		Signature6Checker s6c (&testLog);
	
		UpdaterConfiguration data(1, 1, L"1");
		data.m_sourceList.addSource (UST_UserURL, L"http://127.0.0.1:8081", false, L"");
		testCallback.SetUpdaterConfiguration (data);

		Updater upd (testCallback.m_dp, testCallback, testJournal, &testLog);
		upd.constructSourceList(testCallback.updaterConfiguration().m_sourceList, upd.m_sourceList);

		HANDLE hDoDownload;
		
		string command;
		string param;
		HttpServer::CMD_TYPE type;
		
		{
			FileInfo _fi (L"base005.avc", L"/");
			_fi.m_localPath = L"h:\\test\\";
			FILE* file = _wfopen (wstring(TEST_FOLDER).append(L"\\base005.avc").c_str(), L"w");
			if (file == NULL)  throw std::runtime_error("");
			fwrite ("012345", 1, 6, file);
			fclose (file);

			doEntryDownloader do_dld(&upd, hDoDownload, _fi, _fi.m_localPath, &s6c);
			hs.Accept();

			hs.Receive(command, type, param);
			QC_BOOST_CHECK_MESSAGE (type == HttpServer::CMD_GET, L"Error\n");
			QC_BOOST_CHECK_MESSAGE (command.find ("range: bytes=6-") != wstring::npos, L"Error\n");
			hs.Send("HTTP/1.1 206 Partial content\x0d\x0a");
			hs.Send("Content-Range: bytes 6-9/10\x0d\x0a");
			hs.Send("Connection: Keep-Alive\x0d\x0a");
			hs.Send("\x0d\x0a");
			hs.Send("6789");	
			WaitForSingleObject (hDoDownload, INFINITE);
			QC_BOOST_CHECK_MESSAGE ( CORE_NO_ERROR == do_dld.GetError(), L"Wrong return code\n" );
			QC_BOOST_CHECK_MESSAGE ( _helper::checkFileContent (wstring(TEST_FOLDER).append (L"\\base005.avc"),
											"0123456789"), L"Wrong file content\n" );
		}
		{
			FileInfo _fi (L"base004.avc", L"/");
			_fi.m_localPath = L"h:\\test\\";
			FILE* file = _wfopen (wstring(TEST_FOLDER).append(L"\\base004.avc").c_str(), L"w");
			if (file == NULL)  throw std::runtime_error("");
			fwrite ("abcdef", 1, 6, file);
			fclose (file);

			doEntryDownloader do_dld(&upd, hDoDownload, _fi, _fi.m_localPath, &s6c);		
			hs.Receive(command, type, param);
			QC_BOOST_CHECK_MESSAGE (type == HttpServer::CMD_GET, L"Error\n");
			QC_BOOST_CHECK_MESSAGE (command.find ("range: bytes=6-") != wstring::npos, L"Error\n");
			hs.Send("HTTP/1.1 206 Partial content\x0d\x0a");
			hs.Send("Content-Range: bytes 6-9/10\x0d\x0a");
			hs.Send("Connection: Keep-Alive\x0d\x0a");
			hs.Send("\x0d\x0a");
			hs.Send("ghik");	
			WaitForSingleObject (hDoDownload, INFINITE);
			QC_BOOST_CHECK_MESSAGE ( CORE_NO_ERROR == do_dld.GetError(), L"Wrong return code\n" );
			QC_BOOST_CHECK_MESSAGE ( _helper::checkFileContent (wstring(TEST_FOLDER).append (L"\\base005.avc"),
											"abcdefghik"), L"Wrong file content\n" );
		}
		
		hs.CloseConnection ();

	}


	{//signaling methods priority
		HttpServer hs (8081, "127.0.0.1");
		hs.Start();

		_helper::DeleteDirectory(TEST_FOLDER);
		_helper::MakeDirectory(TEST_FOLDER);
		
		TestLog testLog;
		Callbacks cbacks;
		TestCallbackInterface testCallback (&testLog, &cbacks);
		TestJournalInterface testJournal;
		Signature6Checker s6c (&testLog);
	
		UpdaterConfiguration data(1, 1, L"1");
		data.m_sourceList.addSource (UST_UserURL, L"http://127.0.0.1:8081", false, L"");
		testCallback.SetUpdaterConfiguration (data);

		Updater upd (testCallback.m_dp, testCallback, testJournal, &testLog);
		upd.constructSourceList(testCallback.updaterConfiguration().m_sourceList, upd.m_sourceList);

		HANDLE hDoDownload;

		string command;
		string param;
		HttpServer::CMD_TYPE type;

		hs.Accept();
		
		{//using DoGetRemoteFile_NewNet
			FileInfo _fi (L"base005.avc", L"/");
			_fi.m_localPath = L"h:\\test\\";
			FILE* file = _wfopen (wstring(TEST_FOLDER).append(L"\\base005.avc").c_str(), L"w");
			if (file == NULL)  throw std::runtime_error("");
			fwrite ("012345", 1, 6, file);
			fclose (file);

			doEntryDownloader do_dld(&upd, hDoDownload, _fi, _fi.m_localPath, &s6c);	
			
			hs.Accept();

			hs.Receive(command, type, param);
			QC_BOOST_CHECK_MESSAGE (type == HttpServer::CMD_GET, L"Error\n");
			QC_BOOST_CHECK_MESSAGE (command.find ("range: bytes=6-") != wstring::npos, L"Error\n");
			hs.Send("HTTP/1.1 206 Partial content\x0d\x0a");
			hs.Send("Content-Range: bytes 6-9/10\x0d\x0a");
			hs.Send("Connection: Close\x0d\x0a");
			hs.Send("\x0d\x0a");
			hs.Send("678");
			
			hs.CloseConnection();

			WaitForSingleObject (hDoDownload, INFINITE);
			
			//соединение закрыто, но не все данные отправлены -> ошибка
			QC_BOOST_CHECK_MESSAGE ( CORE_NO_ERROR != do_dld.GetError(), L"Wrong return code\n" );//which error_code ???
			//QC_BOOST_CHECK_MESSAGE ( _helper::checkFileContent (wstring(TEST_FOLDER).append (L"\\base005.avc"),
			//								"0123456789"), L"Wrong file content\n" );
		}
		{//using DoGetRemoteFile_NewNet
			FileInfo _fi (L"base004.avc", L"/");
			_fi.m_localPath = L"h:\\test\\";
			FILE* file = _wfopen (wstring(TEST_FOLDER).append(L"\\base004.avc").c_str(), L"w");
			if (file == NULL)  throw std::runtime_error("");
			fwrite ("012345", 1, 6, file);
			fclose (file);

			doEntryDownloader do_dld(&upd, hDoDownload, _fi, _fi.m_localPath, &s6c);	
			
			hs.Accept();

			hs.Receive(command, type, param);
			QC_BOOST_CHECK_MESSAGE (type == HttpServer::CMD_GET, L"Error\n");
			QC_BOOST_CHECK_MESSAGE (command.find ("range: bytes=6-") != wstring::npos, L"Error\n");
			hs.Send("HTTP/1.1 206 Partial content\x0d\x0a");
			hs.Send("Content-Range: bytes 6-9/10\x0d\x0a");
			hs.Send("Connection: Close\x0d\x0a");
			hs.Send("\x0d\x0a");
			hs.Send("6789a");
			
			hs.CloseConnection();

			WaitForSingleObject (hDoDownload, INFINITE);
			
			//соединение закрыто, но отправлены лишние данные, которые должны быть отброшены
			QC_BOOST_CHECK_MESSAGE ( CORE_NO_ERROR == do_dld.GetError(), L"Wrong return code\n" );
			QC_BOOST_CHECK_MESSAGE ( _helper::checkFileContent (wstring(TEST_FOLDER).append (L"\\base004.avc"),
											"0123456789"), L"Wrong file content\n" );
		}

	}

}

void TestHttpTransport::TestConnection ()
{
	using namespace KLUPD;
	std::wcout<<L"TestConnection\n";
	Logger::Case _case (m_logger, m_testp, m_step, L"TestConnection");

	{//connection: keep-alive
		HttpServer hs (8081, "127.0.0.1");
		hs.Start();

		_helper::DeleteDirectory(TEST_FOLDER);
		_helper::MakeDirectory(TEST_FOLDER);
		
		TestLog testLog;
		Callbacks cbacks;
		TestCallbackInterface testCallback (&testLog, &cbacks);
		TestJournalInterface testJournal;
		Signature6Checker s6c (&testLog);

		UpdaterConfiguration data(1, 1, L"1");
		data.m_sourceList.addSource (UST_UserURL, L"http://127.0.0.1:8081", false, L"");
		testCallback.SetUpdaterConfiguration (data);

		Updater upd (testCallback.m_dp, testCallback, testJournal, &testLog);
		upd.constructSourceList(testCallback.updaterConfiguration().m_sourceList, upd.m_sourceList);

		HANDLE hDoDownload;
		
		{
			_helper::DeleteDirectory(TEST_FOLDER);
			_helper::MakeDirectory(TEST_FOLDER);
		
			FileInfo _fi (L"base005.avc", L"/");
			_fi.m_localPath = L"h:\\test\\";
			doEntryDownloader do_dld(&upd, hDoDownload, _fi, _fi.m_localPath, &s6c);

			string command;
			string param;
			HttpServer::CMD_TYPE type;

			hs.Accept();
		
			hs.Receive(command, type, param);
			QC_BOOST_CHECK_MESSAGE (type == HttpServer::CMD_GET, L"Error\n");
			hs.Send("HTTP/1.1 200 OK\x0d\x0a");
			hs.Send("Content-Length: 10\x0d\x0a");
			hs.Send("Connection: Keep-Alive\x0d\x0a");
			hs.Send("\x0d\x0a");
			hs.Send("0123456789");
		
		
			WaitForSingleObject (hDoDownload, INFINITE);
			QC_BOOST_CHECK_MESSAGE ( CORE_NO_ERROR == do_dld.GetError(), L"Wrong return code\n" );
			QC_BOOST_CHECK_MESSAGE ( _helper::checkFileContent (wstring(TEST_FOLDER).append (L"\\base005.avc"),
											"0123456789"), L"Wrong file content\n" );
		}

		{

			_helper::DeleteDirectory(TEST_FOLDER);
			_helper::MakeDirectory(TEST_FOLDER);

			FileInfo _fi (L"base005.avc", L"/");
			_fi.m_localPath = L"h:\\test\\";
			doEntryDownloader do_dld(&upd, hDoDownload, _fi, _fi.m_localPath, &s6c);

			string command;
			string param;
			HttpServer::CMD_TYPE type;
		
			hs.Receive(command, type, param);
			QC_BOOST_CHECK_MESSAGE (type == HttpServer::CMD_GET, L"Error\n");
			hs.Send("HTTP/1.1 200 OK\x0d\x0a");
			hs.Send("Content-Length: 10\x0d\x0a");
			hs.Send("Connection: Keep-Alive\x0d\x0a");
			hs.Send("\x0d\x0a");
			hs.Send("abcdefghijk");
		
		
			WaitForSingleObject (hDoDownload, INFINITE);
			QC_BOOST_CHECK_MESSAGE ( CORE_NO_ERROR == do_dld.GetError(), L"Wrong return code\n" );
			QC_BOOST_CHECK_MESSAGE ( _helper::checkFileContent (wstring(TEST_FOLDER).append (L"\\base005.avc"),
											"abcdefghijk"), L"Wrong file content\n" );
		}


		hs.CloseConnection ();
		
	}

	{//connection - close
		HttpServer hs (8081, "127.0.0.1");
		hs.Start();

		_helper::DeleteDirectory(TEST_FOLDER);
		_helper::MakeDirectory(TEST_FOLDER);
		
		TestLog testLog;
		Callbacks cbacks;
		TestCallbackInterface testCallback (&testLog, &cbacks);
		TestJournalInterface testJournal;
		Signature6Checker s6c (&testLog);	

		UpdaterConfiguration data(1, 1, L"1");
		data.m_sourceList.addSource (UST_UserURL, L"http://127.0.0.1:8081", false, L"");
		testCallback.SetUpdaterConfiguration (data);

		Updater upd (testCallback.m_dp, testCallback, testJournal, &testLog);
		upd.constructSourceList(testCallback.updaterConfiguration().m_sourceList, upd.m_sourceList);

		HANDLE hDoDownload;

		
		{
			_helper::DeleteDirectory(TEST_FOLDER);
			_helper::MakeDirectory(TEST_FOLDER);
		
			FileInfo _fi (L"base005.avc", L"/");
			_fi.m_localPath = L"h:\\test\\";
			doEntryDownloader do_dld(&upd, hDoDownload, _fi, _fi.m_localPath, &s6c);

			string command;
			string param;
			HttpServer::CMD_TYPE type;

			hs.Accept();
		
			hs.Receive(command, type, param);
			QC_BOOST_CHECK_MESSAGE (type == HttpServer::CMD_GET, L"Error\n");
			hs.Send("HTTP/1.1 200 OK\x0d\x0a");
			hs.Send("Content-Length: 10\x0d\x0a");
			hs.Send("Connection: close\x0d\x0a");
			//hs.Send("Connection: Keep-Alive\x0d\x0a");
			hs.Send("\x0d\x0a");
			hs.Send("0123456789");
		
		
			WaitForSingleObject (hDoDownload, INFINITE);
			QC_BOOST_CHECK_MESSAGE ( CORE_NO_ERROR == do_dld.GetError(), L"Wrong return code\n" );
			QC_BOOST_CHECK_MESSAGE ( _helper::checkFileContent (wstring(TEST_FOLDER).append (L"\\base005.avc"),
											"0123456789"), L"Wrong file content\n" );

			hs.CloseConnection ();
		}

		{

			_helper::DeleteDirectory(TEST_FOLDER);
			_helper::MakeDirectory(TEST_FOLDER);

			FileInfo _fi (L"base005.avc", L"/");
			_fi.m_localPath = L"h:\\test\\";
			doEntryDownloader do_dld(&upd, hDoDownload, _fi, _fi.m_localPath, &s6c);

			string command;
			string param;
			HttpServer::CMD_TYPE type;

			hs.Accept();
		
			hs.Receive(command, type, param);
			QC_BOOST_CHECK_MESSAGE (type == HttpServer::CMD_GET, L"Error\n");
			hs.Send("HTTP/1.1 200 OK\x0d\x0a");
			hs.Send("Content-Length: 10\x0d\x0a");
			hs.Send("Connection: close\x0d\x0a");
			//hs.Send("Connection: Keep-Alive\x0d\x0a");
			hs.Send("\x0d\x0a");
			hs.Send("abcdefghijk");
		
		
			WaitForSingleObject (hDoDownload, INFINITE);
			QC_BOOST_CHECK_MESSAGE ( CORE_NO_ERROR == do_dld.GetError(), L"Wrong return code\n" );
			QC_BOOST_CHECK_MESSAGE ( _helper::checkFileContent (wstring(TEST_FOLDER).append (L"\\base005.avc"),
											"abcdefghijk"), L"Wrong file content\n" );
		
			hs.CloseConnection ();
		}


	}

}

void TestHttpTransport::TestPathes ()
{
	using namespace KLUPD;
	std::wcout<<L"TestPathes\n";
	Logger::Case _case (m_logger, m_testp, m_step, L"TestPathes");

	{
		HttpServer hs (8081, "127.0.0.1");
		hs.Start();

		_helper::DeleteDirectory(TEST_FOLDER);
		_helper::MakeDirectory(TEST_FOLDER);
		
		TestLog testLog;
		Callbacks cbacks;
		TestCallbackInterface testCallback (&testLog, &cbacks);
		TestJournalInterface testJournal;
		Signature6Checker s6c (&testLog);

		FileInfo _fi (L"base005.avc", L"folder");
		_fi.m_localPath = L"h:\\test\\";

		UpdaterConfiguration data(1, 1, L"1");
		data.m_sourceList.addSource (UST_UserURL, L"http://127.0.0.1:8081", false, L"");
		testCallback.SetUpdaterConfiguration (data);

		Updater upd (testCallback.m_dp, testCallback, testJournal, &testLog);
		upd.constructSourceList(testCallback.updaterConfiguration().m_sourceList, upd.m_sourceList);

		HANDLE hDoDownload;
		doEntryDownloader do_dld(&upd, hDoDownload, _fi, _fi.m_localPath, &s6c);

		string command;
		string param;
		HttpServer::CMD_TYPE type;

		hs.Accept();
	
		hs.Receive(command, type, param);
		QC_BOOST_CHECK_MESSAGE (type == HttpServer::CMD_GET, L"Error\n");
		QC_BOOST_CHECK_MESSAGE (command.find ("get /folder/base005.avc") != wstring::npos, L"Error\n");
		hs.Send("HTTP/1.1 200 OK\x0d\x0a");
		hs.Send("Content-Length: 10\x0d\x0a");
		hs.Send("Connection: Keep-Alive\x0d\x0a");
		hs.Send("\x0d\x0a");
		hs.Send("0123456789");
		
		hs.CloseConnection ();
		
		WaitForSingleObject (hDoDownload, INFINITE);
		QC_BOOST_CHECK_MESSAGE ( CORE_NO_ERROR == do_dld.GetError(), L"Wrong return code\n" );
		QC_BOOST_CHECK_MESSAGE ( _helper::checkFileContent (wstring(TEST_FOLDER).append (L"\\base005.avc"),
										"0123456789"), L"Wrong file content\n" );
		
	}


}



void TestHttpTransport::TestConnectionClosingTimeout ()
{
	using namespace KLUPD;
	std::wcout<<L"TestConnectionClosingTimeout\n";
	Logger::Case _case (m_logger, m_testp, m_step, L"TestConnectionClosingTimeout");

	QC_BOOST_REQUIRE_MESSAGE (false, L"Bug\n");

	{//connection: keep-alive
		HttpServer hs (8081, "127.0.0.1");
		hs.Start();

		_helper::DeleteDirectory(TEST_FOLDER);
		_helper::MakeDirectory(TEST_FOLDER);
		
		TestLog testLog;
		Callbacks cbacks;
		TestCallbackInterface testCallback (&testLog, &cbacks);
		TestJournalInterface testJournal;
		Signature6Checker s6c (&testLog);

		UpdaterConfiguration data(1, 1, L"1");
		data.m_sourceList.addSource (UST_UserURL, L"http://127.0.0.1:8081", false, L"");
		testCallback.SetUpdaterConfiguration (data);

		Updater upd (testCallback.m_dp, testCallback, testJournal, &testLog);
		upd.constructSourceList(testCallback.updaterConfiguration().m_sourceList, upd.m_sourceList);

		HANDLE hDoDownload;
				
		{
			_helper::DeleteDirectory(TEST_FOLDER);
			_helper::MakeDirectory(TEST_FOLDER);
		
			FileInfo _fi (L"base005.avc", L"/");
			_fi.m_localPath = L"h:\\test\\";
			doEntryDownloader do_dld(&upd, hDoDownload, _fi, _fi.m_localPath, &s6c);

			string command;
			string param;
			HttpServer::CMD_TYPE type;

			hs.Accept();
		
			hs.Receive(command, type, param);
			QC_BOOST_CHECK_MESSAGE (type == HttpServer::CMD_GET, L"Error\n");
			hs.Send("HTTP/1.1 200 OK\x0d\x0a");
			hs.Send("Content-Length: 10\x0d\x0a");
			hs.Send("Connection: Keep-Alive\x0d\x0a");
			hs.Send("\x0d\x0a");
			hs.Send("0123456789");
		
		
			WaitForSingleObject (hDoDownload, INFINITE);
			QC_BOOST_CHECK_MESSAGE ( CORE_NO_ERROR == do_dld.GetError(), L"Wrong return code\n" );
			QC_BOOST_CHECK_MESSAGE ( _helper::checkFileContent (wstring(TEST_FOLDER).append (L"\\base005.avc"),
											"0123456789"), L"Wrong file content\n" );
		}
			hs.CloseConnection();
			

		{

			_helper::DeleteDirectory(TEST_FOLDER);
			_helper::MakeDirectory(TEST_FOLDER);

			FileInfo _fi (L"base005.avc", L"/");
			_fi.m_localPath = L"h:\\test\\";
			doEntryDownloader do_dld(&upd, hDoDownload, _fi, _fi.m_localPath, &s6c);
			
			string command;
			string param;
			HttpServer::CMD_TYPE type;
		
			hs.Accept();

			hs.Receive(command, type, param);
			QC_BOOST_CHECK_MESSAGE (type == HttpServer::CMD_GET, L"Error\n");
			hs.Send("HTTP/1.1 200 OK\x0d\x0a");
			hs.Send("Content-Length: 10\x0d\x0a");
			hs.Send("Connection: Keep-Alive\x0d\x0a");
			hs.Send("\x0d\x0a");
			hs.Send("abcdefghijk");
		
		
			WaitForSingleObject (hDoDownload, INFINITE);
			QC_BOOST_CHECK_MESSAGE ( CORE_NO_ERROR == do_dld.GetError(), L"Wrong return code\n" );
			QC_BOOST_CHECK_MESSAGE ( _helper::checkFileContent (wstring(TEST_FOLDER).append (L"\\base005.avc"),
											"abcdefghijk"), L"Wrong file content\n" );
		}


		hs.CloseConnection ();
		
	}

}

void TestHttpTransport::TestGetFileContentLength ()
{//add emulating error/hangup while transmitting a header
	using namespace KLUPD;
	std::wcout<<L"TestGetFileContentLength\n";
	Logger::Case _case (m_logger, m_testp, m_step, L"TestGetFileContentLength");

	//Content-Length is larger, than number of bytes sent
	{//timeout exceeds
		HttpServer hs (8081, "127.0.0.1");
		hs.Start();

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
		data.connect_tmo = 10;
		data.m_sourceList.addSource (UST_UserURL, L"http://127.0.0.1:8081", false, L"");
		testCallback.SetUpdaterConfiguration (data);

		Updater upd (testCallback.m_dp, testCallback, testJournal, &testLog);
		upd.constructSourceList(testCallback.updaterConfiguration().m_sourceList, upd.m_sourceList);

		HANDLE hDoDownload;
		doEntryDownloader do_dld(&upd, hDoDownload, _fi, _fi.m_localPath, &s6c);		

		string command;
		string param;
		HttpServer::CMD_TYPE type;

		hs.Accept();
	
		hs.Receive(command, type, param);
		QC_BOOST_CHECK_MESSAGE (type == HttpServer::CMD_GET, L"Error\n");
		hs.Send("HTTP/1.1 200 OK\x0d\x0a");
		hs.Send("Content-Length: 10\x0d\x0a");
		hs.Send("Connection: Keep-Alive\x0d\x0a");
		hs.Send("\x0d\x0a");
		hs.Send("012345");
			
		WaitForSingleObject (hDoDownload, INFINITE);
		QC_BOOST_CHECK_MESSAGE ( CORE_DOWNLOAD_TIMEOUT/*CORE_NO_ERROR*/ == do_dld.GetError(), L"Wrong return code\n" );
		
		hs.CloseConnection ();
	}

	{//disconnect
		HttpServer hs (8081, "127.0.0.1");
		hs.Start();

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
		data.connect_tmo = 30;
		data.m_sourceList.addSource (UST_UserURL, L"http://127.0.0.1:8081", false, L"");
		testCallback.SetUpdaterConfiguration (data);

		Updater upd (testCallback.m_dp, testCallback, testJournal, &testLog);
		upd.constructSourceList(testCallback.updaterConfiguration().m_sourceList, upd.m_sourceList);

		HANDLE hDoDownload;
		doEntryDownloader do_dld(&upd, hDoDownload, _fi, _fi.m_localPath, &s6c);

		string command;
		string param;
		HttpServer::CMD_TYPE type;

		hs.Accept();
	
		hs.Receive(command, type, param);
		QC_BOOST_CHECK_MESSAGE (type == HttpServer::CMD_GET, L"Error\n");
		hs.Send("HTTP/1.1 200 OK\x0d\x0a");
		hs.Send("Content-Length: 10\x0d\x0a");
		hs.Send("Connection: Close\x0d\x0a");
		hs.Send("\x0d\x0a");
		hs.Send("012345");

		Sleep (10000);
		hs.CloseConnection();
			
		WaitForSingleObject (hDoDownload, INFINITE);
		QC_BOOST_CHECK_MESSAGE (CORE_REMOTE_HOST_CLOSED_CONNECTION == do_dld.GetError(), L"Wrong return code\n" );
		
		QC_BOOST_CHECK_MESSAGE ( _helper::checkFileContent (wstring(TEST_FOLDER).append (L"\\base004.avc"),
											"012345"), L"Wrong file content\n" );


		_helper::DeleteDirectory(TEST_FOLDER);
		_helper::MakeDirectory(TEST_FOLDER);

		doEntryDownloader do_dld2(&upd, hDoDownload, _fi, _fi.m_localPath, &s6c);
		
		hs.Accept();
	
		hs.Receive(command, type, param);
		QC_BOOST_CHECK_MESSAGE (type == HttpServer::CMD_GET, L"Error\n");
		hs.Send("HTTP/1.1 200 OK\x0d\x0a");
		hs.Send("Content-Length: 10\x0d\x0a");
		hs.Send("Connection: Keep-Alive\x0d\x0a");
		hs.Send("\x0d\x0a");
		hs.Send("012345");
		
		Sleep (10000);
		hs.CloseConnection();

		WaitForSingleObject (hDoDownload, INFINITE);
		QC_BOOST_CHECK_MESSAGE (CORE_REMOTE_HOST_CLOSED_CONNECTION == do_dld.GetError(), L"Wrong return code\n" );

		QC_BOOST_CHECK_MESSAGE ( _helper::checkFileContent (wstring(TEST_FOLDER).append (L"\\base004.avc"),
								"012345"), L"Wrong file content\n" );
			
	}
}


void TestHttpTransport::TestReusingTheSameServer()
{
	using namespace KLUPD;
	std::wcout<<L"TestReusingTheSameServer\n";
	Logger::Case _case (m_logger, m_testp, m_step, L"TestReusingTheSameServer");

	{//connection: close
		HttpServer hs (8081, "127.0.0.1");
		hs.Start();

		TestLog testLog;
		Callbacks cbacks;
		TestCallbackInterface testCallback (&testLog, &cbacks);
		TestJournalInterface testJournal;
		Signature6Checker s6c (&testLog);

		FileInfo _fi (L"base005.avc", L"/");
		_fi.m_localPath = L"h:\\test\\";

		UpdaterConfiguration data(1, 1, L"1");
		data.m_sourceList.addSource (UST_UserURL, L"http://127.0.0.1:8081", false, L"diffs");
		data.connect_tmo = 30;
		testCallback.SetUpdaterConfiguration (data);

		Updater upd (testCallback.m_dp, testCallback, testJournal, &testLog);
		upd.constructSourceList(testCallback.updaterConfiguration().m_sourceList, upd.m_sourceList);

		HANDLE hDoDownload;

		string command;
		string param;
		HttpServer::CMD_TYPE type;

		for (int i = 0 ; i < 1000; ++i)
		{
			_helper::DeleteDirectory(TEST_FOLDER);
			_helper::MakeDirectory(TEST_FOLDER);

			doEntryDownloader do_dld(&upd, hDoDownload, _fi, _fi.m_localPath, &s6c);
			hs.Accept();
		
			hs.Receive(command, type, param);
			QC_BOOST_CHECK_MESSAGE (type == HttpServer::CMD_GET, L"Error\n");
			hs.Send("HTTP/1.1 200 OK\x0d\x0a");
			hs.Send("Content-Length: 10\x0d\x0a");
			hs.Send("Connection: Close\x0d\x0a");
			hs.Send("\x0d\x0a");
			hs.Send("0123456789");
			hs.CloseConnection ();
				
			WaitForSingleObject (hDoDownload, INFINITE);
			QC_BOOST_CHECK_MESSAGE ( CORE_NO_ERROR == do_dld.GetError(), L"Wrong return code\n" );
			QC_BOOST_CHECK_MESSAGE ( _helper::checkFileContent (wstring(TEST_FOLDER).append (L"\\base005.avc"),
												"0123456789"), L"Wrong file content\n" );
			
			
		}

	}

	{//
		HttpServer hs (8081, "127.0.0.1");
		hs.Start();

		TestLog testLog;
		Callbacks cbacks;
		TestCallbackInterface testCallback (&testLog, &cbacks);
		TestJournalInterface testJournal;
		Signature6Checker s6c (&testLog);

		FileInfo _fi (L"base005.avc", L"/");
		_fi.m_localPath = L"h:\\test\\";

		UpdaterConfiguration data(1, 1, L"1");
		data.m_sourceList.addSource (UST_UserURL, L"http://127.0.0.1:8081", false, L"diffs");
		data.connect_tmo = 30;
		testCallback.SetUpdaterConfiguration (data);

		Updater upd (testCallback.m_dp, testCallback, testJournal, &testLog);
		upd.constructSourceList(testCallback.updaterConfiguration().m_sourceList, upd.m_sourceList);

		HANDLE hDoDownload;

		string command;
		string param;
		HttpServer::CMD_TYPE type;

		for (int i = 0 ; i < 1000; ++i)
		{
			_helper::DeleteDirectory(TEST_FOLDER);
			_helper::MakeDirectory(TEST_FOLDER);

			doEntryDownloader do_dld(&upd, hDoDownload, _fi, _fi.m_localPath, &s6c);

			if (i==0) hs.Accept();
		
			hs.Receive(command, type, param);
			QC_BOOST_CHECK_MESSAGE (type == HttpServer::CMD_GET, L"Error\n");
			hs.Send("HTTP/1.1 200 OK\x0d\x0a");
			hs.Send("Content-Length: 10\x0d\x0a");
			hs.Send("Connection: Keep-Alive\x0d\x0a");
			hs.Send("\x0d\x0a");
			hs.Send("0123456789");
			
				
			WaitForSingleObject (hDoDownload, INFINITE);
			QC_BOOST_CHECK_MESSAGE ( CORE_NO_ERROR == do_dld.GetError(), L"Wrong return code\n" );
			QC_BOOST_CHECK_MESSAGE ( _helper::checkFileContent (wstring(TEST_FOLDER).append (L"\\base005.avc"),
												"0123456789"), L"Wrong file content\n" );
		}
		hs.CloseConnection ();

	}

}

void TestHttpTransport::TestReusingServers()
{
	using namespace KLUPD;
	std::wcout<<L"TestReusingServer\n";
	Logger::Case _case (m_logger, m_testp, m_step, L"TestReusingServer");
	//после очередного изменения интерфейсов переделать тест очень затруднительно
/*

	{//connection: close
		HttpServer hs (8081, "127.0.0.1");
		HttpServer hs2 (8082, "127.0.0.1");
		hs.Start();
		hs2.Start();

		TestLog testLog;
		Callbacks cbacks;
		TestCallbackInterface testCallback (&testLog, &cbacks);
		TestJournalInterface testJournal;
		Signature6Checker s6c (&testLog);

		FileInfo _fi (L"base005.avc", L"/");
		_fi.m_localPath = L"h:\\test\\";

		FileInfo _fi2 (L"base004.avc", L"/");
		_fi2.m_localPath = L"h:\\test\\";

		UpdaterConfiguration data(1, 1, L"1");
		data.m_sourceList.addSource (UST_UserURL, L"http://127.0.0.1:8081", false, L"diffs");
		data.connect_tmo = 30;
		testCallback.SetUpdaterConfiguration (data);
		KLUPD::Source _csd (L"http://127.0.0.1:8081", UST_UserURL, false, L"", UPS_DIRS);
		KLUPD::Source _csd2 (L"http://127.0.0.1:8082", UST_UserURL, false, L"", UPS_DIRS);
	
		Updater upd (testCallback.m_dp, testCallback, testJournal, &testLog);
		upd.constructSourceList(testCallback.updaterConfiguration().m_sourceList, upd.m_sourceList);
		
		HANDLE hDoDownload;

		string command;
		string param;
		HttpServer::CMD_TYPE type;

		for (int i = 0 ; i < 1000; ++i)
		{
			//1
			_helper::DeleteDirectory(TEST_FOLDER);
			_helper::MakeDirectory(TEST_FOLDER);

			doNewNetDownloader do_dld(&upd, hDoDownload, _fi, _fi.m_relativeURLPath, L"h:\\test\\", _csd, &s6c);
			hs.Accept();
		
			hs.Receive(command, type, param);
			QC_BOOST_CHECK_MESSAGE (type == HttpServer::CMD_GET, L"Error\n");
			hs.Send("HTTP/1.1 200 OK\x0d\x0a");
			hs.Send("Content-Length: 10\x0d\x0a");
			hs.Send("Connection: Close\x0d\x0a");
			hs.Send("\x0d\x0a");
			hs.Send("0123456789");
			hs.CloseConnection ();
				
			WaitForSingleObject (hDoDownload, INFINITE);
			QC_BOOST_CHECK_MESSAGE ( CORE_NO_ERROR == do_dld.GetError(), L"Wrong return code\n" );
			QC_BOOST_CHECK_MESSAGE ( _helper::checkFileContent (wstring(TEST_FOLDER).append (L"\\base005.avc"),
												"0123456789"), L"Wrong file content\n" );

			//2
			_helper::DeleteDirectory(TEST_FOLDER);
			_helper::MakeDirectory(TEST_FOLDER);

			doNewNetDownloader do_dld2(&upd, hDoDownload, _fi2, _fi2.m_relativeURLPath, L"h:\\test\\", _csd2, &s6c);
			hs2.Accept();
		
			hs2.Receive(command, type, param);
			QC_BOOST_CHECK_MESSAGE (type == HttpServer::CMD_GET, L"Error\n");
			hs2.Send("HTTP/1.1 200 OK\x0d\x0a");
			hs2.Send("Content-Length: 10\x0d\x0a");
			hs2.Send("Connection: Close\x0d\x0a");
			hs2.Send("\x0d\x0a");
			hs2.Send("9876543210");
			hs2.CloseConnection ();
				
			WaitForSingleObject (hDoDownload, INFINITE);
			QC_BOOST_CHECK_MESSAGE ( CORE_NO_ERROR == do_dld.GetError(), L"Wrong return code\n" );
			QC_BOOST_CHECK_MESSAGE ( _helper::checkFileContent (wstring(TEST_FOLDER).append (L"\\base004.avc"),
												"9876543210"), L"Wrong file content\n" );
			
			
		}

	}

	{//connection: keep-alive
		HttpServer hs (8081, "127.0.0.1");
		HttpServer hs2 (8082, "127.0.0.1");
		hs.Start();
		hs2.Start();

		TestLog testLog;
		Callbacks cbacks;
		TestCallbackInterface testCallback (&testLog, &cbacks);
		TestJournalInterface testJournal;
		Signature6Checker s6c (&testLog);

		FileInfo _fi (L"base005.avc", L"/");
		_fi.m_localPath = L"h:\\test\\";

		FileInfo _fi2 (L"base004.avc", L"/");
		_fi2.m_localPath = L"h:\\test\\";

		UpdaterConfiguration data(1, 1, L"1");
		data.m_sourceList.addSource (UST_UserURL, L"http://127.0.0.1:8081", false, L"diffs");
		data.connect_tmo = 30;
		testCallback.SetUpdaterConfiguration (data);
		KLUPD::Source _csd (L"http://127.0.0.1:8081", UST_UserURL, false, L"", UPS_DIRS);
		KLUPD::Source _csd2 (L"http://127.0.0.1:8082", UST_UserURL, false, L"", UPS_DIRS);
	
		Updater upd (testCallback.m_dp, testCallback, testJournal, &testLog);
		upd.constructSourceList(testCallback.updaterConfiguration().m_sourceList, upd.m_sourceList);
		
		HANDLE hDoDownload;

		string command;
		string param;
		HttpServer::CMD_TYPE type;

		for (int i = 0 ; i < 1000; ++i)
		{
			//1
			_helper::DeleteDirectory(TEST_FOLDER);
			_helper::MakeDirectory(TEST_FOLDER);

			doNewNetDownloader do_dld(&upd, hDoDownload, _fi, _fi.m_relativeURLPath, L"h:\\test\\", _csd, &s6c);
			
			if (i != 0)
			{
				hs2.CheckConnectionClosed();
			}
			
			hs.Accept();
		
			hs.Receive(command, type, param);
			QC_BOOST_CHECK_MESSAGE (type == HttpServer::CMD_GET, L"Error\n");
			hs.Send("HTTP/1.1 200 OK\x0d\x0a");
			hs.Send("Content-Length: 10\x0d\x0a");
			hs.Send("Connection: Keep-Alive\x0d\x0a");
			hs.Send("\x0d\x0a");
			hs.Send("0123456789");
				
			WaitForSingleObject (hDoDownload, INFINITE);
			QC_BOOST_CHECK_MESSAGE ( CORE_NO_ERROR == do_dld.GetError(), L"Wrong return code\n" );
			QC_BOOST_CHECK_MESSAGE ( _helper::checkFileContent (wstring(TEST_FOLDER).append (L"\\base005.avc"),
												"0123456789"), L"Wrong file content\n" );

			//2
			_helper::DeleteDirectory(TEST_FOLDER);
			_helper::MakeDirectory(TEST_FOLDER);

			doNewNetDownloader do_dld2(&upd, hDoDownload, _fi2, _fi2.m_relativeURLPath, L"h:\\test\\", _csd2, &s6c);
			
			hs.CheckConnectionClosed();
			hs2.Accept();
		
			hs2.Receive(command, type, param);
			QC_BOOST_CHECK_MESSAGE (type == HttpServer::CMD_GET, L"Error\n");
			hs2.Send("HTTP/1.1 200 OK\x0d\x0a");
			hs2.Send("Content-Length: 10\x0d\x0a");
			hs2.Send("Connection: Keep-Alive\x0d\x0a");
			hs2.Send("\x0d\x0a");
			hs2.Send("9876543210");
				
			WaitForSingleObject (hDoDownload, INFINITE);
			QC_BOOST_CHECK_MESSAGE ( CORE_NO_ERROR == do_dld.GetError(), L"Wrong return code\n" );
			QC_BOOST_CHECK_MESSAGE ( _helper::checkFileContent (wstring(TEST_FOLDER).append (L"\\base004.avc"),
												"9876543210"), L"Wrong file content\n" );
			
			
		}

	}
*/
}

void TestHttpTransport::TestRedirectionCodes()
{
	using namespace KLUPD;
	std::wcout<<L"TestRedirectionCodes\n";
	Logger::Case _case (m_logger, m_testp, m_step, L"TestRedirectionCodes");

	TestRedirectionCodes_impl (300);
	TestRedirectionCodes_impl (301);
	TestRedirectionCodes_impl (302);
	TestRedirectionCodes_impl (303);
	//TestRedirectionCodes_impl (305); - not implemented
	TestRedirectionCodes_impl (307);

}

void TestHttpTransport::TestRedirectionCodes_impl (const int code)
{
	using namespace KLUPD;
	std::wcout<<L"TestRedirectionCodes_impl "<<code<<L"\n";

	{//simple file getting
		HttpServer hs (8081, "127.0.0.1");
		HttpServer hs_redir (18081, "127.0.0.1");
		hs.Start();
		hs_redir.Start();

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
		data.m_sourceList.addSource (UST_UserURL, L"http://127.0.0.1:8081", false, L"");
		testCallback.SetUpdaterConfiguration (data);

		Updater upd (testCallback.m_dp, testCallback, testJournal, &testLog);
		upd.constructSourceList(testCallback.updaterConfiguration().m_sourceList, upd.m_sourceList);

		HANDLE hDoDownload;
		doEntryDownloader do_dld(&upd, hDoDownload, _fi, _fi.m_localPath, &s6c);

		string command;
		string param;
		HttpServer::CMD_TYPE type;

		hs.Accept();
		hs.Receive(command, type, param);
		QC_BOOST_CHECK_MESSAGE (type == HttpServer::CMD_GET, L"Error\n");
		char redir_code[32];
		itoa (code, redir_code, 10);
		hs.Send("HTTP/1.1 ");
		hs.Send(redir_code);
		hs.Send(" Redirected\x0d\x0a");
		hs.Send("Content-Length: 0\x0d\x0a");
		hs.Send("Location: http://127.0.0.1:18081/bases005.avc\x0d\x0a");
		hs.Send("Connection: Close\x0d\x0a");
		hs.Send("\x0d\x0a");
		hs.CloseConnection ();

		hs_redir.Accept();
		hs_redir.Receive(command, type, param);
		QC_BOOST_CHECK_MESSAGE (type == HttpServer::CMD_GET, L"Error\n");
		hs_redir.Send("HTTP/1.1 200 OK\x0d\x0a");
		hs_redir.Send("Content-Length: 10\x0d\x0a");
		hs_redir.Send("Connection: Close\x0d\x0a");
		hs_redir.Send("\x0d\x0a");
		hs_redir.Send("0123456789");
		hs_redir.CloseConnection ();

		WaitForSingleObject (hDoDownload, INFINITE);
		QC_BOOST_CHECK_MESSAGE ( CORE_NO_ERROR == do_dld.GetError(), L"Wrong return code\n" );
		QC_BOOST_CHECK_MESSAGE ( _helper::checkFileContent (wstring(TEST_FOLDER).append (L"\\base005.avc"),
			"0123456789"), L"Wrong file content\n" );

	}

}

void TestHttpTransport::TestRedirectionCycling ()
{
	using namespace KLUPD;
	std::wcout<<L"TestRedirectionCycling\n";
	Logger::Case _case (m_logger, m_testp, m_step, L"TestRedirectionCycling");

	{//
		HttpServer hs (8081, "127.0.0.1");
		HttpServer hs_redir (10001, "127.0.0.1");
		HttpServer hs_redir2 (10002, "127.0.0.1");
		HttpServer hs_redir3 (10003, "127.0.0.1");

		hs.Start();
		hs_redir.Start();
		hs_redir2.Start();
		hs_redir3.Start();

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
		data.m_sourceList.addSource (UST_UserURL, L"http://127.0.0.1:8081", false, L"");
		testCallback.SetUpdaterConfiguration (data);

		Updater upd (testCallback.m_dp, testCallback, testJournal, &testLog);
		upd.constructSourceList(testCallback.updaterConfiguration().m_sourceList, upd.m_sourceList);

		HANDLE hDoDownload;
		doEntryDownloader do_dld(&upd, hDoDownload, _fi, _fi.m_localPath, &s6c);

		string command;
		string param;
		HttpServer::CMD_TYPE type;

		hs.Accept();
		hs.Receive(command, type, param);
		QC_BOOST_CHECK_MESSAGE (type == HttpServer::CMD_GET, L"Error\n");
		hs.Send("HTTP/1.1 301 Redirected\x0d\x0a");
		hs.Send("Content-Length: 0\x0d\x0a");
		hs.Send("Location: http://127.0.0.1:10001/bases005.avc\x0d\x0a");
		hs.Send("Connection: Close\x0d\x0a");
		hs.Send("\x0d\x0a");
		hs.CloseConnection ();

		hs_redir.Accept();
		hs_redir.Receive(command, type, param);
		QC_BOOST_CHECK_MESSAGE (type == HttpServer::CMD_GET, L"Error\n");
		hs_redir.Send("HTTP/1.1 301 Redirected\x0d\x0a");
		hs_redir.Send("Content-Length: 0\x0d\x0a");
		hs_redir.Send("Location: http://127.0.0.1:10002/bases005.avc\x0d\x0a");
		hs_redir.Send("Connection: Close\x0d\x0a");
		hs_redir.Send("\x0d\x0a");
		hs_redir.CloseConnection ();

		hs_redir2.Accept();
		hs_redir2.Receive(command, type, param);
		QC_BOOST_CHECK_MESSAGE (type == HttpServer::CMD_GET, L"Error\n");
		hs_redir2.Send("HTTP/1.1 301 Redirected\x0d\x0a");
		hs_redir2.Send("Content-Length: 0\x0d\x0a");
		hs_redir2.Send("Location: http://127.0.0.1:10003/bases005.avc\x0d\x0a");
		hs_redir2.Send("Connection: Close\x0d\x0a");
		hs_redir2.Send("\x0d\x0a");
		hs_redir2.CloseConnection ();

		WaitForSingleObject (hDoDownload, INFINITE);
		QC_BOOST_CHECK_MESSAGE ( CORE_NO_SOURCE_FILE == do_dld.GetError(), L"Wrong return code\n" );

	}

}

void TestHttpTransport::TestRedirectionCases ()
{
	using namespace KLUPD;
	std::wcout<<L"TestRedirectionCases\n";
	Logger::Case _case (m_logger, m_testp, m_step, L"TestRedirectionCases");

	{//Connection - Keep-Alive
		HttpServer hs (8081, "127.0.0.1");
		HttpServer hs_redir (18081, "127.0.0.1");
		hs.Start();
		hs_redir.Start();

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
		data.m_sourceList.addSource (UST_UserURL, L"http://127.0.0.1:8081", false, L"");
		testCallback.SetUpdaterConfiguration (data);

		Updater upd (testCallback.m_dp, testCallback, testJournal, &testLog);
		upd.constructSourceList(testCallback.updaterConfiguration().m_sourceList, upd.m_sourceList);

		HANDLE hDoDownload;
		doEntryDownloader do_dld(&upd, hDoDownload, _fi, _fi.m_localPath, &s6c);

		hs.Accept();
		QC_BOOST_CHECK_MESSAGE (hs.ReceivePureGet(), L"Error getting request\n");
		hs.Send302Redirected (HttpServer::KeepAlive, HttpServer::None, "http://127.0.0.1:18081/bases006.avc");
		hs.CheckConnectionClosed ();

		hs_redir.Accept ();
		QC_BOOST_CHECK_MESSAGE (hs_redir.ReceivePureGet("/bases006.avc"), L"Error getting request\n");
		hs_redir.SendFileFromBuffer (HttpServer::Close, HttpServer::None, "0123456789");
		hs_redir.CloseConnection ();

		WaitForSingleObject (hDoDownload, INFINITE);
		QC_BOOST_CHECK_MESSAGE ( CORE_NO_ERROR == do_dld.GetError(), L"Wrong return code\n" );
		QC_BOOST_CHECK_MESSAGE ( _helper::checkFileContent (wstring(TEST_FOLDER).append (L"\\base005.avc"),
			"0123456789"), L"Wrong file content\n" );

	}
	
	{//Connection - Close
		HttpServer hs (8081, "127.0.0.1");
		HttpServer hs_redir (18081, "127.0.0.1");
		hs.Start();
		hs_redir.Start();

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
		data.m_sourceList.addSource (UST_UserURL, L"http://127.0.0.1:8081", false, L"");
		testCallback.SetUpdaterConfiguration (data);

		Updater upd (testCallback.m_dp, testCallback, testJournal, &testLog);
		upd.constructSourceList(testCallback.updaterConfiguration().m_sourceList, upd.m_sourceList);

		HANDLE hDoDownload;
		doEntryDownloader do_dld(&upd, hDoDownload, _fi, _fi.m_localPath, &s6c);

		hs.Accept();
		QC_BOOST_CHECK_MESSAGE (hs.ReceivePureGet(), L"Error getting request\n");
		hs.Send302Redirected (HttpServer::Close, HttpServer::None, "http://127.0.0.1:18081/bases006.avc");
		hs.CheckConnectionClosed ();

		hs_redir.Accept ();
		QC_BOOST_CHECK_MESSAGE (hs_redir.ReceivePureGet("/bases006.avc"), L"Error getting request\n");
		hs_redir.SendFileFromBuffer (HttpServer::Close, HttpServer::None, "0123456789");
		hs_redir.CloseConnection ();

		WaitForSingleObject (hDoDownload, INFINITE);
		QC_BOOST_CHECK_MESSAGE ( CORE_NO_ERROR == do_dld.GetError(), L"Wrong return code\n" );
		QC_BOOST_CHECK_MESSAGE ( _helper::checkFileContent (wstring(TEST_FOLDER).append (L"\\base005.avc"),
			"0123456789"), L"Wrong file content\n" );

	}

	{//Connection to proxy (basic auth)
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
			hs.Send("\x0d\x0a");
			hs.CloseConnection ();

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
			hs.Send("HTTP/1.1 301 Redirected\x0d\x0a");
			hs.Send("Content-Length: 0\x0d\x0a");
			hs.Send("Location: http://kaspersky.ru:10001/bases038.avc\x0d\x0a");
			hs.Send("Connection: Keep-Alive\x0d\x0a");
			hs.Send("Proxy-Connection: Keep-Alive\x0d\x0a");
			hs.Send("\x0d\x0a");

			hs.Receive(command, type, param);
			QC_BOOST_CHECK_MESSAGE (type == HttpServer::CMD_GET, L"Error\n");
			QC_BOOST_CHECK_MESSAGE (command.find("proxy-authorization: basic") != wstring::npos, L"Error\n");
			QC_BOOST_CHECK_MESSAGE (command.find("http://kaspersky.ru:10001/bases038.avc") != wstring::npos, L"Error\n");
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
	}
	

	{//Connection to proxy (ntlm auth)
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

		{

			FileInfo _fi (L"base004.avc", L"/");
			_fi.m_localPath = L"h:\\test\\";

			_helper::DeleteDirectory(TEST_FOLDER);
			_helper::MakeDirectory(TEST_FOLDER);

			doEntryDownloader do_dld(&upd, hDoDownload, _fi, _fi.m_localPath, &s6c);

			hs.Receive(command, type, param);
			QC_BOOST_CHECK_MESSAGE (type == HttpServer::CMD_GET, L"Error\n");
			hs.Send("HTTP/1.1 301 Redirected\x0d\x0a");
			hs.Send("Content-Length: 0\x0d\x0a");
			hs.Send("Location: http://kaspersky.ru:10001/bases038.avc\x0d\x0a");
			hs.Send("Connection: Keep-Alive\x0d\x0a");
			hs.Send("Proxy-Connection: Keep-Alive\x0d\x0a");
			hs.Send("\x0d\x0a");


			//msg3
			hs.Receive(command, type, param);
			QC_BOOST_CHECK_MESSAGE (type == HttpServer::CMD_GET, L"Error\n");
			QC_BOOST_CHECK_MESSAGE (command.find("proxy-authorization: ntlm") == wstring::npos, L"Error\n");
			QC_BOOST_CHECK_MESSAGE (command.find("http://kaspersky.ru:10001/bases038.avc") != wstring::npos, L"Error\n");
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
	}
	
}

void TestHttpTransport::TestGetFile_IpV6 ()
{
	using namespace KLUPD;
	std::wcout<<L"TestGetFile_IpV6\n";
	Logger::Case _case (m_logger, m_testp, m_step, L"TestGetFile_IpV6");

	{//simple file getting
		HttpServer hs (8081, IP_V6_ADDR_1, IP_V6);
		hs.Start();

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
		data.m_sourceList.addSource (UST_UserURL, wstring(L"http://")
								.append(IP_V6_ADDR_1_WSTR).append(L":8081").c_str(), false, L"");
		testCallback.SetUpdaterConfiguration (data);

		Updater upd (testCallback.m_dp, testCallback, testJournal, &testLog);
		upd.constructSourceList(testCallback.updaterConfiguration().m_sourceList, upd.m_sourceList);

		HANDLE hDoDownload;
		doEntryDownloader do_dld(&upd, hDoDownload, _fi, _fi.m_localPath, &s6c);

		string command;
		string param;
		HttpServer::CMD_TYPE type;

		hs.Accept();

		hs.Receive(command, type, param);
		QC_BOOST_CHECK_MESSAGE (type == HttpServer::CMD_GET, L"Error\n");
		hs.Send("HTTP/1.1 200 OK\x0d\x0a");
		hs.Send("Content-Length: 10\x0d\x0a");
		hs.Send("Connection: Keep-Alive\x0d\x0a");
		hs.Send("\x0d\x0a");
		hs.Send("0123456789");

		hs.CloseConnection ();

		WaitForSingleObject (hDoDownload, INFINITE);
		QC_BOOST_CHECK_MESSAGE ( CORE_NO_ERROR == do_dld.GetError(), L"Wrong return code\n" );
		QC_BOOST_CHECK_MESSAGE ( _helper::checkFileContent (wstring(TEST_FOLDER).append (L"\\base005.avc"),
			"0123456789"), L"Wrong file content\n" );

	}

}