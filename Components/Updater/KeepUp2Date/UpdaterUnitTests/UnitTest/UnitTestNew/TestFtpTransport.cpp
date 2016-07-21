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
#include "TestFtpTransport.h"
#include "main.h"

#include "FtpServer.h"

#include "constants.h"

void TestFtpTransport::TestWrongAuth ()
{
	using namespace KLUPD;
	std::wcout<<L"TestWrongAuth\n";
	Logger::Case _case (m_logger, m_testp, m_step, L"TestWrongAuth");

	{//wrong authorization
		FtpServer fs (2121, "127.0.0.1");
		fs.Start();

		_helper::DeleteDirectory(TEST_FOLDER);
		_helper::MakeDirectory(TEST_FOLDER);
		
		TestLog testLog;
		Callbacks cbacks;
		TestCallbackInterface testCallback (&testLog, &cbacks);
		TestJournalInterface testJournal;
		Signature6Checker s6c (&testLog);

		FileInfo _fi (L"base003.avc", L"/");

		UpdaterConfiguration data(1, 1, L"1");
		data.connect_tmo = 600;
		testCallback.SetUpdaterConfiguration (data);
		
		CDownloadProgress dp (&testCallback);
		Updater upd (dp, testCallback, testJournal, &testLog);
		HANDLE hDoDownload;
		doSelfNetDownloader do_dld(&upd, hDoDownload, _fi, _fi.m_relativeURLPath, L"h:\\test\\", L"ftp://127.0.0.1:2121");
		
		string command;
		string param;
		FtpServer::CMD_TYPE type;

		fs.Accept();
		fs.Send ("220 Hello\x0d\x0a");
		
		fs.Receive(command, type, param);
		QC_BOOST_CHECK_MESSAGE (type == FtpServer::CMD_USER, L"Error\n");
		fs.Send ("331 Anonymous access allowed, send identity (e-mail name) as password.\x0d\x0a");
		fs.Receive(command, type, param);
		QC_BOOST_CHECK_MESSAGE (type == FtpServer::CMD_PASS, L"Error\n");
		fs.Send ("530 Not logged in.\x0d\x0a");
			
		WaitForSingleObject (hDoDownload, INFINITE);
		do_dld.GetError();
		QC_BOOST_CHECK_MESSAGE ( CORE_FTP_AUTH_ERROR == do_dld.GetError(), L"Wrong return code\n" );
	}
}


void TestFtpTransport::TestPassiveMode ()
{
	using namespace KLUPD;
	std::wcout<<L"TestPassiveMode\n";
	Logger::Case _case (m_logger, m_testp, m_step, L"TestPassiveMode");

	{//using passive mode
		FtpServer fs (2121, "127.0.0.1");
		fs.Start();

		_helper::DeleteDirectory(TEST_FOLDER);
		_helper::MakeDirectory(TEST_FOLDER);
		
		TestLog testLog;
		Callbacks cbacks;
		TestCallbackInterface testCallback (&testLog, &cbacks);
		TestJournalInterface testJournal;
		Signature6Checker s6c (&testLog);

		FileInfo _fi (L"base003.avc", L"/");

		UpdaterConfiguration data(1, 1, L"1");
		data.connect_tmo = 600;
		data.passive_ftp = true;
		testCallback.SetUpdaterConfiguration (data);
		

		Updater upd (testCallback.m_dp, testCallback, testJournal, &testLog);
		HANDLE hDoDownload;
		doSelfNetDownloader do_dld(&upd, hDoDownload, _fi, _fi.m_relativeURLPath, L"h:\\test\\", L"ftp://127.0.0.1:2121");
		

		string command;
		string param;
		FtpServer::CMD_TYPE type;

		fs.Accept();
		fs.Send ("220 Hello\x0d\x0a");
		
		fs.Receive(command, type, param);
		QC_BOOST_CHECK_MESSAGE (type == FtpServer::CMD_USER, L"Error\n");
		fs.Send ("331 Anonymous access allowed, send identity (e-mail name) as password.\x0d\x0a");
		fs.Receive(command, type, param);
		QC_BOOST_CHECK_MESSAGE (type == FtpServer::CMD_PASS, L"Error\n");
		fs.Send ("230 Logged in.\x0d\x0a");
		fs.Receive(command, type, param);
		QC_BOOST_CHECK_MESSAGE (type == FtpServer::CMD_TTYPE, L"Error\n");
		fs.Send ("200 Command okay.\x0d\x0a");
		fs.Receive(command, type, param);
		QC_BOOST_CHECK_MESSAGE (type == FtpServer::CMD_EPSV, L"Error\n");
		fs.Send ("500 Do not understand.\x0d\x0a");
		fs.Receive(command, type, param);
		QC_BOOST_CHECK_MESSAGE (type == FtpServer::CMD_PASV, L"Error\n");
		fs.Send ("502 Command not implemented.\x0d\x0a");
		//fs.Send ("227 Entering Passive Mode (172,16,10,175,6,11).\x0d\x0a");
		
		WaitForSingleObject (hDoDownload, INFINITE);
		QC_BOOST_CHECK_MESSAGE ( CORE_DOWNLOAD_ERROR == do_dld.GetError(), L"Wrong return code\n" );
		
	}
}

void TestFtpTransport::TestActiveMode ()
{
	using namespace KLUPD;
	std::wcout<<L"TestActiveMode\n";
	Logger::Case _case (m_logger, m_testp, m_step, L"TestActiveMode");
	{//using active mode
		FtpServer fs (2121, "127.0.0.1");
		fs.Start();

		_helper::DeleteDirectory(TEST_FOLDER);
		_helper::MakeDirectory(TEST_FOLDER);
		
		TestLog testLog;
		Callbacks cbacks;
		TestCallbackInterface testCallback (&testLog, &cbacks);
		TestJournalInterface testJournal;
		Signature6Checker s6c (&testLog);

		FileInfo _fi (L"base003.avc", L"/");

		UpdaterConfiguration data(1, 1, L"1");
		data.connect_tmo = 600;
		data.passive_ftp = false;
		testCallback.SetUpdaterConfiguration (data);
		

		Updater upd (testCallback.m_dp, testCallback, testJournal, &testLog);
		HANDLE hDoDownload;
		doSelfNetDownloader do_dld(&upd, hDoDownload, _fi, _fi.m_relativeURLPath, L"h:\\test\\", L"ftp://127.0.0.1:2121");
		

		string command;
		string param;
		FtpServer::CMD_TYPE type;

		fs.Accept();
		fs.Send ("220 Hello\x0d\x0a");
		
		fs.Receive(command, type, param);
		QC_BOOST_CHECK_MESSAGE (type == FtpServer::CMD_USER, L"Error\n");
		fs.Send ("331 Anonymous access allowed, send identity (e-mail name) as password.\x0d\x0a");
		fs.Receive(command, type, param);
		QC_BOOST_CHECK_MESSAGE (type == FtpServer::CMD_PASS, L"Error\n");
		fs.Send ("230 Logged in.\x0d\x0a");
		fs.Receive(command, type, param);
		QC_BOOST_CHECK_MESSAGE (type == FtpServer::CMD_TTYPE, L"Error\n");
		fs.Send ("200 Command okay.\x0d\x0a");
		fs.Receive(command, type, param);
		QC_BOOST_CHECK_MESSAGE (type == FtpServer::CMD_EPRT, L"Error\n");
		fs.Send ("502 Command not implemented.\x0d\x0a");
		fs.Receive(command, type, param);
		QC_BOOST_CHECK_MESSAGE (type == FtpServer::CMD_PORT, L"Error\n");
		fs.Send ("502 Command not implemented.\x0d\x0a");
		
		WaitForSingleObject (hDoDownload, INFINITE);
		QC_BOOST_CHECK_MESSAGE ( CORE_DOWNLOAD_ERROR == do_dld.GetError(), L"Wrong return code\n" );
		
	}
}

void TestFtpTransport::TestErrorRetrFile ()
{
	using namespace KLUPD;
	std::wcout<<L"TestErrorRetrFile\n";
	Logger::Case _case (m_logger, m_testp, m_step, L"TestErrorRetrFile");

	{//error on retr (passive)
		FtpServer fs (2121, "127.0.0.1");
		fs.Start();

		_helper::DeleteDirectory(TEST_FOLDER);
		_helper::MakeDirectory(TEST_FOLDER);
		
		TestLog testLog;
		Callbacks cbacks;
		TestCallbackInterface testCallback (&testLog, &cbacks);
		TestJournalInterface testJournal;
		Signature6Checker s6c (&testLog);

		FileInfo _fi (L"base003.avc", L"/");

		UpdaterConfiguration data(1, 1, L"1");
		data.connect_tmo = 600;
		data.passive_ftp = true;
		testCallback.SetUpdaterConfiguration (data);
		

		Updater upd (testCallback.m_dp, testCallback, testJournal, &testLog);
		HANDLE hDoDownload;
		doSelfNetDownloader do_dld(&upd, hDoDownload, _fi, _fi.m_relativeURLPath, L"h:\\test\\", L"ftp://127.0.0.1:2121");
		

		string command;
		string param;
		FtpServer::CMD_TYPE type;

		fs.Accept();
		fs.Send ("220 Hello\x0d\x0a");
		
		fs.Receive(command, type, param);
		QC_BOOST_CHECK_MESSAGE (type == FtpServer::CMD_USER, L"Error\n");
		fs.Send ("331 Anonymous access allowed, send identity (e-mail name) as password.\x0d\x0a");
		fs.Receive(command, type, param);
		QC_BOOST_CHECK_MESSAGE (type == FtpServer::CMD_PASS, L"Error\n");
		fs.Send ("230 Logged in.\x0d\x0a");
		fs.Receive(command, type, param);
		QC_BOOST_CHECK_MESSAGE (type == FtpServer::CMD_TTYPE, L"Error\n");
		fs.Send ("200 Command okay.\x0d\x0a");
		fs.Receive(command, type, param);
		QC_BOOST_CHECK_MESSAGE (type == FtpServer::CMD_EPSV, L"Error\n");
		fs.Send ("500 Do not understand.\x0d\x0a");
		fs.Receive(command, type, param);
		QC_BOOST_CHECK_MESSAGE (type == FtpServer::CMD_PASV, L"Error\n");

		string addr;
		fs.StartListeningPassiveDataPort (addr);
		fs.Send (string("227 Entering Passive Mode (").append(addr).append(").\x0d\x0a"));
		fs.AcceptDataConnection ();

		fs.Receive(command, type, param);
		QC_BOOST_CHECK_MESSAGE (type == FtpServer::CMD_RETR, L"Error\n");
		fs.Send ("450 Requested file action not taken.\x0d\x0a");
		
		WaitForSingleObject (hDoDownload, INFINITE);
		QC_BOOST_CHECK_MESSAGE ( CORE_DOWNLOAD_ERROR == do_dld.GetError(), L"Wrong return code\n" );
		
	}
}

void TestFtpTransport::TestGettingFilePassive ()
{
	using namespace KLUPD;
	std::wcout<<L"TestGettingFilePassive\n";
	Logger::Case _case (m_logger, m_testp, m_step, L"TestGettingFilePassive");	

	{//getting file (passive mode)
		FtpServer fs (2121, "127.0.0.1");
		fs.Start();

		_helper::DeleteDirectory(TEST_FOLDER);
		_helper::MakeDirectory(TEST_FOLDER);
		
		TestLog testLog;
		Callbacks cbacks;
		TestCallbackInterface testCallback (&testLog, &cbacks);
		TestJournalInterface testJournal;
		Signature6Checker s6c (&testLog);

		FileInfo _fi (L"base003.avc", L"/");

		UpdaterConfiguration data(1, 1, L"1");
		data.connect_tmo = 600;
		data.passive_ftp = true;
		testCallback.SetUpdaterConfiguration (data);
		

		Updater upd (testCallback.m_dp, testCallback, testJournal, &testLog);
		HANDLE hDoDownload;
		doSelfNetDownloader do_dld(&upd, hDoDownload, _fi, _fi.m_relativeURLPath, L"h:\\test\\", L"ftp://127.0.0.1:2121");
		

		string command;
		string param;
		FtpServer::CMD_TYPE type;

		fs.Accept();
		fs.Send ("220 Hello\x0d\x0a");
		
		fs.Receive(command, type, param);
		QC_BOOST_CHECK_MESSAGE (type == FtpServer::CMD_USER, L"Error\n");
		fs.Send ("331 Anonymous access allowed, send identity (e-mail name) as password.\x0d\x0a");
		fs.Receive(command, type, param);
		QC_BOOST_CHECK_MESSAGE (type == FtpServer::CMD_PASS, L"Error\n");
		fs.Send ("230 Logged in.\x0d\x0a");
		fs.Receive(command, type, param);
		QC_BOOST_CHECK_MESSAGE (type == FtpServer::CMD_TTYPE, L"Error\n");
		fs.Send ("200 Command okay.\x0d\x0a");
		fs.Receive(command, type, param);
		QC_BOOST_CHECK_MESSAGE (type == FtpServer::CMD_EPSV, L"Error\n");
		fs.Send ("500 Do not understand.\x0d\x0a");
		fs.Receive(command, type, param);
		QC_BOOST_CHECK_MESSAGE (type == FtpServer::CMD_PASV, L"Error\n");

		string addr;
		fs.CreateBindListeningPassiveDataPort (addr);
		fs.StartListeningPassiveDataPortOnly ();
		fs.Send (string("227 Entering Passive Mode (").append(addr).append(").\x0d\x0a"));
		fs.AcceptDataConnection ();

		fs.Receive(command, type, param);
		QC_BOOST_CHECK_MESSAGE (type == FtpServer::CMD_RETR, L"Error\n");
		fs.Send ("125 Data connection already open; Transfer starting.\x0d\x0a");
		fs.SendData ("0123456789");
		fs.CloseDataConnection ();
		fs.Send ("226 Transfer complete.\x0d\x0a");
		
		

		WaitForSingleObject (hDoDownload, INFINITE);
		QC_BOOST_CHECK_MESSAGE ( CORE_NO_ERROR == do_dld.GetError(), L"Wrong return code\n" );
		QC_BOOST_CHECK_MESSAGE ( _helper::checkFileContent (wstring(TEST_FOLDER).append (L"\\base003.avc"),
										"0123456789"), L"Wrong return code\n" );
		
	}
/* not for testing	
	{//getting file (passive mode)
		FtpServer fs (2121, "127.0.0.1");
		fs.Start();

		_helper::DeleteDirectory(TEST_FOLDER);
		_helper::MakeDirectory(TEST_FOLDER);
		
		TestLog testLog;
		Callbacks cbacks;
		TestCallbackInterface testCallback (&testLog, &cbacks);
		TestJournalInterface testJournal;
		Signature6Checker s6c (&testLog);

		FileInfo _fi (L"base003.avc", L"/");

		UpdaterConfiguration data(1, 1, L"1");
		data.connect_tmo = 600;
		data.passive_ftp = true;
		testCallback.SetUpdaterConfiguration (data);
		

		//Updater upd (testCallback.m_dp, testCallback, testJournal, &testLog);
		//HANDLE hDoDownload;
		//doSelfNetDownloader do_dld(&upd, hDoDownload, _fi, _fi.m_relativeURLPath, L"h:\\test\\", L"ftp://127.0.0.1:2121");
		

		string command;
		string param;
		FtpServer::CMD_TYPE type;

		fs.Accept();
		fs.Send ("220 Hello\x0d\x0a");
		
		fs.Receive(command, type, param);
		QC_BOOST_CHECK_MESSAGE (type == FtpServer::CMD_USER, L"Error\n");
		fs.Send ("331 Anonymous access allowed, send identity (e-mail name) as password.\x0d\x0a");
		fs.Receive(command, type, param);
		QC_BOOST_CHECK_MESSAGE (type == FtpServer::CMD_PASS, L"Error\n");
		fs.Send ("230 Logged in.\x0d\x0a");
		fs.Receive(command, type, param);
		QC_BOOST_CHECK_MESSAGE (type == FtpServer::CMD_TTYPE, L"Error\n");
		fs.Send ("200 Command okay.\x0d\x0a");
		
fs.Receive(command, type, param);
		fs.Send ("502 not implemented.\x0d\x0a");
fs.Receive(command, type, param);
		fs.Send ("502 not implemented.\x0d\x0a");
fs.Receive(command, type, param);
		fs.Send ("257 \"/\" is current directory.\x0d\x0a");
fs.Receive(command, type, param);
		fs.Send ("200 Command okay.\x0d\x0a");
		

		fs.Receive(command, type, param);
		QC_BOOST_CHECK_MESSAGE (type == FtpServer::CMD_PASV, L"Error\n");

		string addr;
		fs.CreateBindListeningPassiveDataPort (addr);
		fs.StartListeningPassiveDataPortOnly ();
		fs.Send (string("227 Entering Passive Mode (").append(addr).append(").\x0d\x0a"));
		

		fs.Receive(command, type, param);
		QC_BOOST_CHECK_MESSAGE (type == FtpServer::CMD_RETR, L"Error\n");
		
		
		
		fs.Send ("150 Data connection already open; Transfer starting.\x0d\x0a");
		
		
		fs.AcceptDataConnection ();


		fs.SendData ("0123456789");
		fs.CloseDataConnection ();
		fs.Send ("226 Transfer complete.\x0d\x0a");
		
		

		//WaitForSingleObject (hDoDownload, INFINITE);
		//QC_BOOST_CHECK_MESSAGE ( CORE_NO_ERROR == do_dld.GetError(), L"Wrong return code\n" );
		//QC_BOOST_CHECK_MESSAGE ( _helper::checkFileContent (wstring(TEST_FOLDER).append (L"\\base003.avc"),
		//								"0123456789"), L"Wrong return code\n" );
		
	}*/
}

void TestFtpTransport::TestGettingFileActive ()
{//2 variants
	using namespace KLUPD;
	std::wcout<<L"TestGettingFileActive\n";
	Logger::Case _case (m_logger, m_testp, m_step, L"TestGettingFileActive");	
	
	{//getting file (active mode)
		FtpServer fs (2121, "127.0.0.1");
		fs.Start();

		_helper::DeleteDirectory(TEST_FOLDER);
		_helper::MakeDirectory(TEST_FOLDER);
		
		TestLog testLog;
		Callbacks cbacks;
		TestCallbackInterface testCallback (&testLog, &cbacks);
		TestJournalInterface testJournal;
		Signature6Checker s6c (&testLog);

		FileInfo _fi (L"base003.avc", L"/");

		UpdaterConfiguration data(1, 1, L"1");
		data.connect_tmo = 600;
		data.passive_ftp = false;
		testCallback.SetUpdaterConfiguration (data);
		

		Updater upd (testCallback.m_dp, testCallback, testJournal, &testLog);
		HANDLE hDoDownload;
		doSelfNetDownloader do_dld(&upd, hDoDownload, _fi, _fi.m_relativeURLPath, L"h:\\test\\", L"ftp://127.0.0.1:2121");
		

		string command;
		string param;
		FtpServer::CMD_TYPE type;

		fs.Accept();
		fs.Send ("220 Hello\x0d\x0a");
		
		fs.Receive(command, type, param);
		QC_BOOST_CHECK_MESSAGE (type == FtpServer::CMD_USER, L"Error\n");
		fs.Send ("331 Anonymous access allowed, send identity (e-mail name) as password.\x0d\x0a");
		fs.Receive(command, type, param);
		QC_BOOST_CHECK_MESSAGE (type == FtpServer::CMD_PASS, L"Error\n");
		fs.Send ("230 Logged in.\x0d\x0a");
		fs.Receive(command, type, param);
		QC_BOOST_CHECK_MESSAGE (type == FtpServer::CMD_TTYPE, L"Error\n");
		fs.Send ("200 Command okay.\x0d\x0a");
		fs.Receive(command, type, param);
		QC_BOOST_CHECK_MESSAGE (type == FtpServer::CMD_EPRT, L"Error\n");
		fs.Send ("500 error.\x0d\x0a");
		fs.Receive(command, type, param);
		QC_BOOST_CHECK_MESSAGE (type == FtpServer::CMD_PORT, L"Error\n");
		fs.Send ("200 PORT command okay.\x0d\x0a");

		fs.EstablishActiveConnection(param);

		fs.Receive(command, type, param);
		QC_BOOST_CHECK_MESSAGE (type == FtpServer::CMD_RETR, L"Error\n");
		fs.Send ("125 Data connection already open; Transfer starting.\x0d\x0a");
		fs.SendData ("0123456789");
		fs.CloseDataConnection ();
		fs.Send ("226 Transfer complete.\x0d\x0a");
		
		

		WaitForSingleObject (hDoDownload, INFINITE);
		QC_BOOST_CHECK_MESSAGE ( CORE_NO_ERROR == do_dld.GetError(), L"Wrong return code\n" );
		QC_BOOST_CHECK_MESSAGE ( _helper::checkFileContent (wstring(TEST_FOLDER).append (L"\\base003.avc"),
										"0123456789"), L"Wrong return code\n" );
		
	}
	
	{//getting file (active mode)
		FtpServer fs (2121, "127.0.0.1");
		fs.Start();

		_helper::DeleteDirectory(TEST_FOLDER);
		_helper::MakeDirectory(TEST_FOLDER);
		
		TestLog testLog;
		Callbacks cbacks;
		TestCallbackInterface testCallback (&testLog, &cbacks);
		TestJournalInterface testJournal;
		Signature6Checker s6c (&testLog);

		FileInfo _fi (L"base003.avc", L"/");

		UpdaterConfiguration data(1, 1, L"1");
		data.connect_tmo = 600;
		data.passive_ftp = false;
		testCallback.SetUpdaterConfiguration (data);
		

		Updater upd (testCallback.m_dp, testCallback, testJournal, &testLog);
		HANDLE hDoDownload;
		doSelfNetDownloader do_dld(&upd, hDoDownload, _fi, _fi.m_relativeURLPath, L"h:\\test\\", L"ftp://127.0.0.1:2121");
		

		string command;
		string param;
		FtpServer::CMD_TYPE type;

		fs.Accept();
		fs.Send ("220 Hello\x0d\x0a");
		
		fs.Receive(command, type, param);
		QC_BOOST_CHECK_MESSAGE (type == FtpServer::CMD_USER, L"Error\n");
		fs.Send ("331 Anonymous access allowed, send identity (e-mail name) as password.\x0d\x0a");
		fs.Receive(command, type, param);
		QC_BOOST_CHECK_MESSAGE (type == FtpServer::CMD_PASS, L"Error\n");
		fs.Send ("230 Logged in.\x0d\x0a");
		fs.Receive(command, type, param);
		QC_BOOST_CHECK_MESSAGE (type == FtpServer::CMD_TTYPE, L"Error\n");
		fs.Send ("200 Command okay.\x0d\x0a");
		fs.Receive(command, type, param);
		QC_BOOST_CHECK_MESSAGE (type == FtpServer::CMD_EPRT, L"Error\n");
		fs.Send ("500 error\x0d\x0a");
		fs.Receive(command, type, param);
		QC_BOOST_CHECK_MESSAGE (type == FtpServer::CMD_PORT, L"Error\n");
		fs.Send ("200 PORT command okay.\x0d\x0a");
		string addr = param;

		fs.Receive(command, type, param);
		QC_BOOST_CHECK_MESSAGE (type == FtpServer::CMD_RETR, L"Error\n");
		fs.Send ("150 File status okay; about to open data connection.\x0d\x0a");
		fs.EstablishActiveConnection(addr);
		fs.SendData ("0123456789");
		fs.CloseDataConnection ();
		fs.Send ("226 Transfer complete.\x0d\x0a");
		
		

		WaitForSingleObject (hDoDownload, INFINITE);
		QC_BOOST_CHECK_MESSAGE ( CORE_NO_ERROR == do_dld.GetError(), L"Wrong return code\n" );
		QC_BOOST_CHECK_MESSAGE ( _helper::checkFileContent (wstring(TEST_FOLDER).append (L"\\base003.avc"),
										"0123456789"), L"Wrong return code\n" );
		
	}
}

void TestFtpTransport::TestGettingFileWithPath ()
{
	using namespace KLUPD;
	std::wcout<<L"TestGettingFileWithPath\n";
	Logger::Case _case (m_logger, m_testp, m_step, L"TestGettingFileWithPath");
	
	{//getting file with path (passive mode)
		FtpServer fs (2121, "127.0.0.1");
		fs.Start();

		_helper::DeleteDirectory(TEST_FOLDER);
		_helper::MakeDirectory(TEST_FOLDER);
		
		TestLog testLog;
		Callbacks cbacks;
		TestCallbackInterface testCallback (&testLog, &cbacks);
		TestJournalInterface testJournal;
		Signature6Checker s6c (&testLog);

		FileInfo _fi (L"base003.avc", L"/folder/subfolder/");

		UpdaterConfiguration data(1, 1, L"1");
		data.connect_tmo = 600;
		data.passive_ftp = true;
		testCallback.SetUpdaterConfiguration (data);
		

		Updater upd (testCallback.m_dp, testCallback, testJournal, &testLog);
		HANDLE hDoDownload;
		doSelfNetDownloader do_dld(&upd, hDoDownload, _fi, _fi.m_relativeURLPath, L"h:\\test\\", L"ftp://127.0.0.1:2121");
		

		string command;
		string param;
		FtpServer::CMD_TYPE type;

		fs.Accept();
		fs.Send ("220 Hello\x0d\x0a");
		
		fs.Receive(command, type, param);
		QC_BOOST_CHECK_MESSAGE (type == FtpServer::CMD_USER, L"Error\n");
		fs.Send ("331 Anonymous access allowed, send identity (e-mail name) as password.\x0d\x0a");
		fs.Receive(command, type, param);
		QC_BOOST_CHECK_MESSAGE (type == FtpServer::CMD_PASS, L"Error\n");
		fs.Send ("230 Logged in.\x0d\x0a");
		fs.Receive(command, type, param);
		QC_BOOST_CHECK_MESSAGE (type == FtpServer::CMD_TTYPE, L"Error\n");
		fs.Send ("200 Command okay.\x0d\x0a");
		fs.Receive(command, type, param);
		QC_BOOST_CHECK_MESSAGE (type == FtpServer::CMD_EPSV, L"Error\n");
		fs.Send ("500 Error.\x0d\x0a");
		fs.Receive(command, type, param);
		QC_BOOST_CHECK_MESSAGE (type == FtpServer::CMD_PASV, L"Error\n");

		string addr;
		fs.StartListeningPassiveDataPort (addr);
		fs.Send (string("227 Entering Passive Mode (").append(addr).append(").\x0d\x0a"));
		fs.AcceptDataConnection ();

		fs.Receive(command, type, param);
		QC_BOOST_CHECK_MESSAGE (type == FtpServer::CMD_RETR, L"Error\n");
		QC_BOOST_CHECK_MESSAGE (param == "folder/subfolder/base003.avc", L"Error\n");
		fs.Send ("125 Data connection already open; Transfer starting.\x0d\x0a");
		fs.SendData ("0123456789");
		fs.CloseDataConnection ();
		fs.Send ("226 Transfer complete.\x0d\x0a");
		
		
		WaitForSingleObject (hDoDownload, INFINITE);
		QC_BOOST_CHECK_MESSAGE ( CORE_NO_ERROR == do_dld.GetError(), L"Wrong return code\n" );
		QC_BOOST_CHECK_MESSAGE ( _helper::checkFileContent (wstring(TEST_FOLDER).append (L"\\base003.avc"),
										"0123456789"), L"Wrong return code\n" );	

	}
}

void TestFtpTransport::TestGetting2Files ()
{
	using namespace KLUPD;
	std::wcout<<L"TestGetting2Files\n";
	Logger::Case _case (m_logger, m_testp, m_step, L"TestGetting2Files");
	
	{//getting 2 files (passive mode)
	// also different code are used after closing data connection
		FtpServer fs (2121, "127.0.0.1");
		fs.Start();

		_helper::DeleteDirectory(TEST_FOLDER);
		_helper::MakeDirectory(TEST_FOLDER);
		
		TestLog testLog;
		Callbacks cbacks;
		TestCallbackInterface testCallback (&testLog, &cbacks);
		TestJournalInterface testJournal;
		Signature6Checker s6c (&testLog);

		FileInfo _fi (L"base003.avc", L"/");

		UpdaterConfiguration data(1, 1, L"1");
		data.connect_tmo = 600;
		data.passive_ftp = true;
		testCallback.SetUpdaterConfiguration (data);
		

		Updater upd (testCallback.m_dp, testCallback, testJournal, &testLog);
		HANDLE hDoDownload;
		doSelfNetDownloader do_dld(&upd, hDoDownload, _fi, _fi.m_relativeURLPath, L"h:\\test\\", L"ftp://127.0.0.1:2121");
		

		string command;
		string param;
		FtpServer::CMD_TYPE type;

		fs.Accept();
		fs.Send ("220 Hello\x0d\x0a");
		
		fs.Receive(command, type, param);
		QC_BOOST_CHECK_MESSAGE (type == FtpServer::CMD_USER, L"Error\n");
		fs.Send ("331 Anonymous access allowed, send identity (e-mail name) as password.\x0d\x0a");
		fs.Receive(command, type, param);
		QC_BOOST_CHECK_MESSAGE (type == FtpServer::CMD_PASS, L"Error\n");
		fs.Send ("230 Logged in.\x0d\x0a");
		fs.Receive(command, type, param);
		QC_BOOST_CHECK_MESSAGE (type == FtpServer::CMD_TTYPE, L"Error\n");
		fs.Send ("200 Command okay.\x0d\x0a");
		fs.Receive(command, type, param);
		QC_BOOST_CHECK_MESSAGE (type == FtpServer::CMD_EPSV, L"Error\n");
		fs.Send ("500 Error.\x0d\x0a");
		fs.Receive(command, type, param);
		QC_BOOST_CHECK_MESSAGE (type == FtpServer::CMD_PASV, L"Error\n");

		string addr;
		fs.StartListeningPassiveDataPort (addr);
		fs.Send (string("227 Entering Passive Mode (").append(addr).append(").\x0d\x0a"));
		fs.AcceptDataConnection ();

		fs.Receive(command, type, param);
		QC_BOOST_CHECK_MESSAGE (type == FtpServer::CMD_RETR, L"Error\n");
		QC_BOOST_CHECK_MESSAGE (param == "base003.avc", L"Error\n");
		fs.Send ("125 Data connection already open; Transfer starting.\x0d\x0a");
		fs.SendData ("0123456789");
		fs.CloseDataConnection ();
		fs.Send ("226 Transfer complete.\x0d\x0a");
		
		
		WaitForSingleObject (hDoDownload, INFINITE);
		QC_BOOST_CHECK_MESSAGE ( CORE_NO_ERROR == do_dld.GetError(), L"Wrong return code\n" );
		QC_BOOST_CHECK_MESSAGE ( _helper::checkFileContent (wstring(TEST_FOLDER).append (L"\\base003.avc"),
										"0123456789"), L"Wrong return code\n" );	

		//2
		FileInfo _fi2 (L"base004.avc", L"/");
		doSelfNetDownloader do_dld2(&upd, hDoDownload, _fi2, _fi2.m_relativeURLPath, L"h:\\test\\", L"ftp://127.0.0.1:2121");
//--->>вот тут вопрос		
		fs.Receive(command, type, param);
		QC_BOOST_CHECK_MESSAGE (type == FtpServer::CMD_PASV, L"Error\n");

		fs.StartListeningPassiveDataPort (addr);
		fs.Send (string("227 Entering Passive Mode (").append(addr).append(").\x0d\x0a"));
		fs.AcceptDataConnection ();

		fs.Receive(command, type, param);
		QC_BOOST_CHECK_MESSAGE (type == FtpServer::CMD_RETR, L"Error\n");
		QC_BOOST_CHECK_MESSAGE (param == "base004.avc", L"Error\n");
		fs.Send ("125 Data connection already open; Transfer starting.\x0d\x0a");
		fs.SendData ("abcdefgh");
		fs.CloseDataConnection ();
		fs.Send ("250 Transfer complete.\x0d\x0a");
		
		
		WaitForSingleObject (hDoDownload, INFINITE);
		QC_BOOST_CHECK_MESSAGE ( CORE_NO_ERROR == do_dld2.GetError(), L"Wrong return code\n" );
		QC_BOOST_CHECK_MESSAGE ( _helper::checkFileContent (wstring(TEST_FOLDER).append (L"\\base004.avc"),
										"abcdefgh"), L"Wrong return code\n" );	
	}
}

void TestFtpTransport::TestUsingMultilineReplies ()
{
	using namespace KLUPD;
	std::wcout<<L"TestUsingMultilineReplies\n";
	Logger::Case _case (m_logger, m_testp, m_step, L"TestUsingMultilineReplies");

	//QC_BOOST_REQUIRE_MESSAGE (false, L"Bug\n")

	{//getting 2 files (passive mode) - using multiple answers
		FtpServer fs (2121, "127.0.0.1");
		fs.Start();

		_helper::DeleteDirectory(TEST_FOLDER);
		_helper::MakeDirectory(TEST_FOLDER);
		
		TestLog testLog;
		Callbacks cbacks;
		TestCallbackInterface testCallback (&testLog, &cbacks);
		TestJournalInterface testJournal;
		Signature6Checker s6c (&testLog);

		FileInfo _fi (L"base003.avc", L"/");

		UpdaterConfiguration data(1, 1, L"1");
		data.connect_tmo = 600;
		data.passive_ftp = true;
		testCallback.SetUpdaterConfiguration (data);
		

		Updater upd (testCallback.m_dp, testCallback, testJournal, &testLog);
		HANDLE hDoDownload;
		doSelfNetDownloader do_dld(&upd, hDoDownload, _fi, _fi.m_relativeURLPath, L"h:\\test\\", L"ftp://127.0.0.1:2121");
		

		string command;
		string param;
		FtpServer::CMD_TYPE type;

		fs.Accept();
		fs.Send ("220-Hello\015\012Congrats\015\012220 Hello\015\012");
		
		fs.Receive(command, type, param);
		QC_BOOST_CHECK_MESSAGE (type == FtpServer::CMD_USER, L"Error\n");
		fs.Send ("331-Anonymous access allowed,\015\012send identity (e-mail name)\015\012331 as password.\x0d\x0a");
		fs.Receive(command, type, param);
		QC_BOOST_CHECK_MESSAGE (type == FtpServer::CMD_PASS, L"Error\n");
		fs.Send ("230-Logged in.\015\012Congrats\015\012230 Logged in\015\012");
		fs.Receive(command, type, param);
		QC_BOOST_CHECK_MESSAGE (type == FtpServer::CMD_TTYPE, L"Error\n");
		fs.Send ("200-Command\015\012200 okay.\015\012");
		fs.Receive(command, type, param);
		QC_BOOST_CHECK_MESSAGE (type == FtpServer::CMD_EPSV, L"Error\n");
		fs.Send ("500-Error\015\012500 Error.\015\012");
		fs.Receive(command, type, param);
		QC_BOOST_CHECK_MESSAGE (type == FtpServer::CMD_PASV, L"Error\n");

		string addr;
		fs.StartListeningPassiveDataPort (addr);
		//fs.Send (string("227-Entering\015\012227 Passive Mode (").append(addr).append(").\x0d\x0a"));
		fs.Send (string("227 Entering Passive Mode (").append(addr).append(").\x0d\x0a"));
		fs.AcceptDataConnection ();

		fs.Receive(command, type, param);
		QC_BOOST_CHECK_MESSAGE (type == FtpServer::CMD_RETR, L"Error\n");
		QC_BOOST_CHECK_MESSAGE (param == "base003.avc", L"Error\n");
		fs.Send ("125-Data connection already open;\015\012125Transfer starting.\x0d\x0a");
		fs.SendData ("0123456789");
		fs.CloseDataConnection ();
		fs.Send ("226-Transfer\015\012226 complete.\x0d\x0a");
		
		
		WaitForSingleObject (hDoDownload, INFINITE);
		QC_BOOST_CHECK_MESSAGE ( CORE_NO_ERROR == do_dld.GetError(), L"Wrong return code\n" );
		QC_BOOST_CHECK_MESSAGE ( _helper::checkFileContent (wstring(TEST_FOLDER).append (L"\\base003.avc"),
										"0123456789"), L"Wrong return code\n" );	

		//2
		FileInfo _fi2 (L"base004.avc", L"/");
		doSelfNetDownloader do_dld2(&upd, hDoDownload, _fi2, _fi2.m_relativeURLPath, L"h:\\test\\", L"ftp://127.0.0.1:2121");
//------->уточнить		
		fs.Receive(command, type, param);
		QC_BOOST_CHECK_MESSAGE (type == FtpServer::CMD_PASV, L"Error\n");

		fs.StartListeningPassiveDataPort (addr);
		fs.Send (string("227 Entering Passive Mode (").append(addr).append(").\x0d\x0a"));
		fs.AcceptDataConnection ();

		fs.Receive(command, type, param);
		QC_BOOST_CHECK_MESSAGE (type == FtpServer::CMD_RETR, L"Error\n");
		QC_BOOST_CHECK_MESSAGE (param == "base004.avc", L"Error\n");
		fs.Send ("125-Data connection already open;\015\012125 Transfer starting.\x0d\x0a");
		fs.SendData ("abcdefgh");
		fs.CloseDataConnection ();
		fs.Send ("226-Transfer\015\012226complete.\x0d\x0a");
		
		
		WaitForSingleObject (hDoDownload, INFINITE);
		QC_BOOST_CHECK_MESSAGE ( CORE_NO_ERROR == do_dld2.GetError(), L"Wrong return code\n" );
		QC_BOOST_CHECK_MESSAGE ( _helper::checkFileContent (wstring(TEST_FOLDER).append (L"\\base004.avc"),
										"abcdefgh"), L"Wrong return code\n" );	
	}
}

void TestFtpTransport::TestUsingMultilineRepliesAdv ()
{
	using namespace KLUPD;
	std::wcout<<L"TestUsingMultilineRepliesAdv\n";
	Logger::Case _case (m_logger, m_testp, m_step, L"TestUsingMultilineRepliesAdv");

	//QC_BOOST_REQUIRE_MESSAGE (false, L"Bug\n")

	{//getting 2 files (passive mode) - trick with multiline replies
		FtpServer fs (2121, "127.0.0.1");
		fs.Start();

		_helper::DeleteDirectory(TEST_FOLDER);
		_helper::MakeDirectory(TEST_FOLDER);
		
		TestLog testLog;
		Callbacks cbacks;
		TestCallbackInterface testCallback (&testLog, &cbacks);
		TestJournalInterface testJournal;
		Signature6Checker s6c (&testLog);

		FileInfo _fi (L"base003.avc", L"/");

		UpdaterConfiguration data(1, 1, L"1");
		data.connect_tmo = 600;
		data.passive_ftp = true;
		testCallback.SetUpdaterConfiguration (data);
		

		Updater upd (testCallback.m_dp, testCallback, testJournal, &testLog);
		HANDLE hDoDownload;
		doSelfNetDownloader do_dld(&upd, hDoDownload, _fi, _fi.m_relativeURLPath, L"h:\\test\\", L"ftp://127.0.0.1:2121");
		

		string command;
		string param;
		FtpServer::CMD_TYPE type;

		fs.Accept();
		fs.Send ("220-Hello\015\012   421 Service not available\015\012220 Hello\x0d\x0a");
		
		fs.Receive(command, type, param);
		QC_BOOST_CHECK_MESSAGE (type == FtpServer::CMD_USER, L"Error\n");
		fs.Send ("331 Anonymous access allowed, send identity (e-mail name) as password.\x0d\x0a");
		fs.Receive(command, type, param);
		QC_BOOST_CHECK_MESSAGE (type == FtpServer::CMD_PASS, L"Error\n");
		fs.Send ("230 Logged in.\x0d\x0a");
		fs.Receive(command, type, param);
		QC_BOOST_CHECK_MESSAGE (type == FtpServer::CMD_TTYPE, L"Error\n");
		fs.Send ("200 Command okay.\x0d\x0a");
		fs.Receive(command, type, param);
		QC_BOOST_CHECK_MESSAGE (type == FtpServer::CMD_EPSV, L"Error\n");
		fs.Send ("500 Error.\x0d\x0a");
		fs.Receive(command, type, param);
		QC_BOOST_CHECK_MESSAGE (type == FtpServer::CMD_PASV, L"Error\n");

		string addr;
		fs.StartListeningPassiveDataPort (addr);
		fs.Send (string("227 Entering Passive Mode (").append(addr).append(").\x0d\x0a"));
		fs.AcceptDataConnection ();

		fs.Receive(command, type, param);
		QC_BOOST_CHECK_MESSAGE (type == FtpServer::CMD_RETR, L"Error\n");
		QC_BOOST_CHECK_MESSAGE (param == "base003.avc", L"Error\n");
		fs.Send ("125 Data connection already open; Transfer starting.\x0d\x0a");
		fs.SendData ("0123456789");
		fs.CloseDataConnection ();
		fs.Send ("226 Transfer complete.\x0d\x0a");
		
		
		WaitForSingleObject (hDoDownload, INFINITE);
		QC_BOOST_CHECK_MESSAGE ( CORE_NO_ERROR == do_dld.GetError(), L"Wrong return code\n" );
		QC_BOOST_CHECK_MESSAGE ( _helper::checkFileContent (wstring(TEST_FOLDER).append (L"\\base003.avc"),
										"0123456789"), L"Wrong return code\n" );	

		//2
		FileInfo _fi2 (L"base004.avc", L"/");
		doSelfNetDownloader do_dld2(&upd, hDoDownload, _fi2, _fi2.m_relativeURLPath, L"h:\\test\\", L"ftp://127.0.0.1:2121");
//-->>уточнить		
		fs.Receive(command, type, param);
		QC_BOOST_CHECK_MESSAGE (type == FtpServer::CMD_PASV, L"Error\n");

		fs.StartListeningPassiveDataPort (addr);
		fs.Send (string("227 Entering Passive Mode (").append(addr).append(").\x0d\x0a"));
		fs.AcceptDataConnection ();

		fs.Receive(command, type, param);
		QC_BOOST_CHECK_MESSAGE (type == FtpServer::CMD_RETR, L"Error\n");
		QC_BOOST_CHECK_MESSAGE (param == "base004.avc", L"Error\n");
		fs.Send ("125 Data connection already open; Transfer starting.\x0d\x0a");
		fs.SendData ("abcdefgh");
		fs.CloseDataConnection ();
		fs.Send ("226 Transfer complete.\x0d\x0a");
		
		
		WaitForSingleObject (hDoDownload, INFINITE);
		QC_BOOST_CHECK_MESSAGE ( CORE_NO_ERROR == do_dld2.GetError(), L"Wrong return code\n" );
		QC_BOOST_CHECK_MESSAGE ( _helper::checkFileContent (wstring(TEST_FOLDER).append (L"\\base004.avc"),
										"abcdefgh"), L"Wrong return code\n" );	
	}
}	

 void TestFtpTransport::TestGettingFileWithRest ()
{
	using namespace KLUPD;
	std::wcout<<L"TestGettingFileWithRest\n";
	Logger::Case _case (m_logger, m_testp, m_step, L"TestGettingFileWithRest");
	{//getting file with rest (passive mode)
		FtpServer fs (2121, "127.0.0.1");
		fs.Start();

		_helper::DeleteDirectory(TEST_FOLDER);
		_helper::MakeDirectory(TEST_FOLDER);
		
		TestLog testLog;
		Callbacks cbacks;
		TestCallbackInterface testCallback (&testLog, &cbacks);
		TestJournalInterface testJournal;
		Signature6Checker s6c (&testLog);

		FileInfo _fi (L"base003.avc", L"/");
		FILE* file = _wfopen (wstring(TEST_FOLDER).append(L"\\base003.avc").c_str(), L"w");
		if (file == NULL)  throw std::runtime_error("");
		fwrite ("012345", 1, 6, file);
		fclose (file);

		UpdaterConfiguration data(1, 1, L"1");
		data.connect_tmo = 600;
		data.passive_ftp = true;
		testCallback.SetUpdaterConfiguration (data);
		

		Updater upd (testCallback.m_dp, testCallback, testJournal, &testLog);
		HANDLE hDoDownload;
		doSelfNetDownloader do_dld(&upd, hDoDownload, _fi, _fi.m_relativeURLPath, L"h:\\test\\", L"ftp://127.0.0.1:2121");
		

		string command;
		string param;
		FtpServer::CMD_TYPE type;

		fs.Accept();
		fs.Send ("220 Hello\x0d\x0a");
		
		fs.Receive(command, type, param);
		QC_BOOST_CHECK_MESSAGE (type == FtpServer::CMD_USER, L"Error\n");
		fs.Send ("331 Anonymous access allowed, send identity (e-mail name) as password.\x0d\x0a");
		fs.Receive(command, type, param);
		QC_BOOST_CHECK_MESSAGE (type == FtpServer::CMD_PASS, L"Error\n");
		fs.Send ("230 Logged in.\x0d\x0a");
		fs.Receive(command, type, param);
		QC_BOOST_CHECK_MESSAGE (type == FtpServer::CMD_TTYPE, L"Error\n");
		fs.Send ("200 Command okay.\x0d\x0a");
		fs.Receive(command, type, param);
		QC_BOOST_CHECK_MESSAGE (type == FtpServer::CMD_EPSV, L"Error\n");
		fs.Send ("500 Error.\x0d\x0a");
		fs.Receive(command, type, param);
		QC_BOOST_CHECK_MESSAGE (type == FtpServer::CMD_PASV, L"Error\n");

		string addr;
		fs.StartListeningPassiveDataPort (addr);
		fs.Send (string("227 Entering Passive Mode (").append(addr).append(").\x0d\x0a"));
		fs.AcceptDataConnection ();

		fs.Receive(command, type, param);
		QC_BOOST_CHECK_MESSAGE (type == FtpServer::CMD_REST, L"Error\n");
		fs.Send ("350 Restarting.\x0d\x0a");

		fs.Receive(command, type, param);
		QC_BOOST_CHECK_MESSAGE (type == FtpServer::CMD_RETR, L"Error\n");
		fs.Send ("125 Data connection already open; Transfer starting.\x0d\x0a");
		fs.SendData ("6789");
		fs.CloseDataConnection ();
		fs.Send ("226 Transfer complete.\x0d\x0a");
		
		

		WaitForSingleObject (hDoDownload, INFINITE);
		QC_BOOST_CHECK_MESSAGE ( CORE_NO_ERROR == do_dld.GetError(), L"Wrong return code\n" );
		QC_BOOST_CHECK_MESSAGE ( _helper::checkFileContent (wstring(TEST_FOLDER).append (L"\\base003.avc"),
										"0123456789"), L"Wrong return code\n" );
		
	}
}

void TestFtpTransport::TestGettingFileWithoutRest ()
{
	using namespace KLUPD;
	std::wcout<<L"TestGettingFileWithoutRest\n";
	Logger::Case _case (m_logger, m_testp, m_step, L"TestGettingFileWithoutRest");
	{//getting file,  rest not supported (passive mode)
		FtpServer fs (2121, "127.0.0.1");
		fs.Start();

		_helper::DeleteDirectory(TEST_FOLDER);
		_helper::MakeDirectory(TEST_FOLDER);
		
		TestLog testLog;
		Callbacks cbacks;
		TestCallbackInterface testCallback (&testLog, &cbacks);
		TestJournalInterface testJournal;
		Signature6Checker s6c (&testLog);

		FileInfo _fi (L"base003.avc", L"/");
		FILE* file = _wfopen (wstring(TEST_FOLDER).append(L"\\base003.avc").c_str(), L"w");
		if (file == NULL)  throw std::runtime_error("");
		fwrite ("012345", 1, 6, file);
		fclose (file);

		UpdaterConfiguration data(1, 1, L"1");
		data.connect_tmo = 600;
		data.passive_ftp = true;
		testCallback.SetUpdaterConfiguration (data);
		

		Updater upd (testCallback.m_dp, testCallback, testJournal, &testLog);
		HANDLE hDoDownload;
		doSelfNetDownloader do_dld(&upd, hDoDownload, _fi, _fi.m_relativeURLPath, L"h:\\test\\", L"ftp://127.0.0.1:2121");
		

		string command;
		string param;
		FtpServer::CMD_TYPE type;

		fs.Accept();
		fs.Send ("220 Hello\x0d\x0a");
		
		fs.Receive(command, type, param);
		QC_BOOST_CHECK_MESSAGE (type == FtpServer::CMD_USER, L"Error\n");
		fs.Send ("331 Anonymous access allowed, send identity (e-mail name) as password.\x0d\x0a");
		fs.Receive(command, type, param);
		QC_BOOST_CHECK_MESSAGE (type == FtpServer::CMD_PASS, L"Error\n");
		fs.Send ("230 Logged in.\x0d\x0a");
		fs.Receive(command, type, param);
		QC_BOOST_CHECK_MESSAGE (type == FtpServer::CMD_TTYPE, L"Error\n");
		fs.Send ("200 Command okay.\x0d\x0a");
		fs.Receive(command, type, param);
		QC_BOOST_CHECK_MESSAGE (type == FtpServer::CMD_EPSV, L"Error\n");
		fs.Send ("500 Error.\x0d\x0a");
		fs.Receive(command, type, param);
		QC_BOOST_CHECK_MESSAGE (type == FtpServer::CMD_PASV, L"Error\n");

		string addr;
		fs.StartListeningPassiveDataPort (addr);
		fs.Send (string("227 Entering Passive Mode (").append(addr).append(").\x0d\x0a"));
		fs.AcceptDataConnection ();

		fs.Receive(command, type, param);
		QC_BOOST_CHECK_MESSAGE (type == FtpServer::CMD_REST, L"Error\n");
		fs.Send ("502 Command not implemented.\x0d\x0a");

		fs.Receive(command, type, param);
		QC_BOOST_CHECK_MESSAGE (type == FtpServer::CMD_RETR, L"Error\n");
		fs.Send ("125 Data connection already open; Transfer starting.\x0d\x0a");
		fs.SendData ("0123456789");
		fs.CloseDataConnection ();
		fs.Send ("226 Transfer complete.\x0d\x0a");
		
		

		WaitForSingleObject (hDoDownload, INFINITE);
		QC_BOOST_CHECK_MESSAGE ( CORE_NO_ERROR == do_dld.GetError(), L"Wrong return code\n" );
		QC_BOOST_CHECK_MESSAGE ( _helper::checkFileContent (wstring(TEST_FOLDER).append (L"\\base003.avc"),
										"0123456789"), L"Wrong file\n" );	
	}
}

void TestFtpTransport::TestAbortTransferringClient ()
{
	using namespace KLUPD;
	std::wcout<<L"TestAbortTransferringClient\n";
	Logger::Case _case (m_logger, m_testp, m_step, L"TestAbortTransferringClient");
	
	{//abort while getting data from ftp (passive mode)
		FtpServer fs (2121, "127.0.0.1");
		fs.Start();

		_helper::DeleteDirectory(TEST_FOLDER);
		_helper::MakeDirectory(TEST_FOLDER);
		
		TestLog testLog;
		Callbacks cbacks;
		TestCallbackInterface testCallback (&testLog, &cbacks);
		TestJournalInterface testJournal;
		Signature6Checker s6c (&testLog);
		cbacks.bFlags[Callbacks::checkCancel] = true;

		FileInfo _fi (L"base003.avc", L"/folder/subfolder/");

		UpdaterConfiguration data(1, 1, L"1");
		data.connect_tmo = 600;
		data.passive_ftp = true;
		testCallback.SetUpdaterConfiguration (data);
		

		Updater upd (testCallback.m_dp, testCallback, testJournal, &testLog);
		HANDLE hDoDownload;
		doSelfNetDownloader do_dld(&upd, hDoDownload, _fi, _fi.m_relativeURLPath, L"h:\\test\\", L"ftp://127.0.0.1:2121");
		

		string command;
		string param;
		FtpServer::CMD_TYPE type;

		fs.Accept();
		fs.Send ("220 Hello\x0d\x0a");
		
		fs.Receive(command, type, param);
		QC_BOOST_CHECK_MESSAGE (type == FtpServer::CMD_USER, L"Error\n");
		fs.Send ("331 Anonymous access allowed, send identity (e-mail name) as password.\x0d\x0a");
		fs.Receive(command, type, param);
		QC_BOOST_CHECK_MESSAGE (type == FtpServer::CMD_PASS, L"Error\n");
		fs.Send ("230 Logged in.\x0d\x0a");
		fs.Receive(command, type, param);
		QC_BOOST_CHECK_MESSAGE (type == FtpServer::CMD_TTYPE, L"Error\n");
		fs.Send ("200 Command okay.\x0d\x0a");
		fs.Receive(command, type, param);
		QC_BOOST_CHECK_MESSAGE (type == FtpServer::CMD_EPSV, L"Error\n");
		fs.Send ("500 Error.\x0d\x0a");
		fs.Receive(command, type, param);
		QC_BOOST_CHECK_MESSAGE (type == FtpServer::CMD_PASV, L"Error\n");

		string addr;
		fs.StartListeningPassiveDataPort (addr);
		fs.Send (string("227 Entering Passive Mode (").append(addr).append(").\x0d\x0a"));
		fs.AcceptDataConnection ();

		fs.Receive(command, type, param);
		QC_BOOST_CHECK_MESSAGE (type == FtpServer::CMD_RETR, L"Error\n");
		QC_BOOST_CHECK_MESSAGE (param == "folder/subfolder/base003.avc", L"Error\n");
		fs.Send ("125 Data connection already open; Transfer starting.\x0d\x0a");
		fs.SendData ("0123456789");
		
		Sleep (5000);
		testCallback.SetCancelUpdate (true);

		QC_BOOST_CHECK_MESSAGE (fs.CheckDataConnectionClosed(), L"Error\n");
		QC_BOOST_CHECK_MESSAGE (fs.CheckControlConnectionClosed(), L"Error\n");
				
		WaitForSingleObject (hDoDownload, INFINITE);
		QC_BOOST_CHECK_MESSAGE ( CORE_CANCELLED == do_dld.GetError(), L"Wrong return code\n" );
		QC_BOOST_CHECK_MESSAGE ( _helper::checkFileContent (wstring(TEST_FOLDER).append (L"\\base003.avc"),
										"0123456789"), L"Wrong return code\n" );	

	}
}	

void TestFtpTransport::TestAbortTransferringServer ()
{
	using namespace KLUPD;
	std::wcout<<L"TestAbortTransferringServer\n";
	Logger::Case _case (m_logger, m_testp, m_step, L"TestAbortTransferringServer");
	{//abort from the server-side while getting data from ftp (passive mode)
		FtpServer fs (2121, "127.0.0.1");
		fs.Start();

		_helper::DeleteDirectory(TEST_FOLDER);
		_helper::MakeDirectory(TEST_FOLDER);
		
		TestLog testLog;
		Callbacks cbacks;
		TestCallbackInterface testCallback (&testLog, &cbacks);
		TestJournalInterface testJournal;
		Signature6Checker s6c (&testLog);

		FileInfo _fi (L"base003.avc", L"/folder/subfolder/");

		UpdaterConfiguration data(1, 1, L"1");
		data.connect_tmo = 600;
		data.passive_ftp = true;
		testCallback.SetUpdaterConfiguration (data);
		

		Updater upd (testCallback.m_dp, testCallback, testJournal, &testLog);
		HANDLE hDoDownload;
		doSelfNetDownloader do_dld(&upd, hDoDownload, _fi, _fi.m_relativeURLPath, L"h:\\test\\", L"ftp://127.0.0.1:2121");
		

		string command;
		string param;
		FtpServer::CMD_TYPE type;

		fs.Accept();
		fs.Send ("220 Hello\x0d\x0a");
		
		fs.Receive(command, type, param);
		QC_BOOST_CHECK_MESSAGE (type == FtpServer::CMD_USER, L"Error\n");
		fs.Send ("331 Anonymous access allowed, send identity (e-mail name) as password.\x0d\x0a");
		fs.Receive(command, type, param);
		QC_BOOST_CHECK_MESSAGE (type == FtpServer::CMD_PASS, L"Error\n");
		fs.Send ("230 Logged in.\x0d\x0a");
		fs.Receive(command, type, param);
		QC_BOOST_CHECK_MESSAGE (type == FtpServer::CMD_TTYPE, L"Error\n");
		fs.Send ("200 Command okay.\x0d\x0a");
		fs.Receive(command, type, param);
		QC_BOOST_CHECK_MESSAGE (type == FtpServer::CMD_EPSV, L"Error\n");
		fs.Send ("500 Error.\x0d\x0a");
		fs.Receive(command, type, param);
		QC_BOOST_CHECK_MESSAGE (type == FtpServer::CMD_PASV, L"Error\n");

		string addr;
		fs.StartListeningPassiveDataPort (addr);
		fs.Send (string("227 Entering Passive Mode (").append(addr).append(").\x0d\x0a"));
		fs.AcceptDataConnection ();

		fs.Receive(command, type, param);
		QC_BOOST_CHECK_MESSAGE (type == FtpServer::CMD_RETR, L"Error\n");
		QC_BOOST_CHECK_MESSAGE (param == "folder/subfolder/base003.avc", L"Error\n");
		fs.Send ("125 Data connection already open; Transfer starting.\x0d\x0a");
		fs.SendData ("0123456789");
		fs.CloseDataConnection ();
		fs.Send ("426 Connection closed; transfer aborted.\x0d\x0a");
		
				
		WaitForSingleObject (hDoDownload, INFINITE);
		QC_BOOST_CHECK_MESSAGE ( CORE_DOWNLOAD_ERROR == do_dld.GetError(), L"Wrong return code\n" );

	}
}

void TestFtpTransport::TestTimeouts ()
{
	using namespace KLUPD;
	std::wcout<<L"TestTimeouts\n";
	Logger::Case _case (m_logger, m_testp, m_step, L"TestTimeouts");
	{//testing timeouts (passive mode)
		FtpServer fs (2121, "127.0.0.1");
		fs.Start();

		_helper::DeleteDirectory(TEST_FOLDER);
		_helper::MakeDirectory(TEST_FOLDER);
		
		TestLog testLog;
		Callbacks cbacks;
		TestCallbackInterface testCallback (&testLog, &cbacks);
		TestJournalInterface testJournal;
		Signature6Checker s6c (&testLog);

		FileInfo _fi (L"base003.avc", L"/folder/subfolder/");

		UpdaterConfiguration data(1, 1, L"1");
		data.connect_tmo = 8; //seconds
		data.passive_ftp = true;
		testCallback.SetUpdaterConfiguration (data);
		
		Updater upd (testCallback.m_dp, testCallback, testJournal, &testLog);
		HANDLE hDoDownload;
		doSelfNetDownloader do_dld(&upd, hDoDownload, _fi, _fi.m_relativeURLPath, L"h:\\test\\", L"ftp://127.0.0.1:2121");
		

		string command;
		string param;
		FtpServer::CMD_TYPE type;

		fs.Accept();
		fs.Send ("220 Hello\x0d\x0a");
		
		fs.Receive(command, type, param);
		QC_BOOST_CHECK_MESSAGE (type == FtpServer::CMD_USER, L"Error\n");
		fs.Send ("331 Anonymous access allowed, send identity (e-mail name) as password.\x0d\x0a");
		fs.Receive(command, type, param);
		QC_BOOST_CHECK_MESSAGE (type == FtpServer::CMD_PASS, L"Error\n");
		fs.Send ("230 Logged in.\x0d\x0a");
		fs.Receive(command, type, param);
		QC_BOOST_CHECK_MESSAGE (type == FtpServer::CMD_TTYPE, L"Error\n");
		fs.Send ("200 Command okay.\x0d\x0a");
		fs.Receive(command, type, param);
		QC_BOOST_CHECK_MESSAGE (type == FtpServer::CMD_EPSV, L"Error\n");
		fs.Send ("500 Error.\x0d\x0a");
		fs.Receive(command, type, param);
		QC_BOOST_CHECK_MESSAGE (type == FtpServer::CMD_PASV, L"Error\n");

		string addr;
		fs.StartListeningPassiveDataPort (addr);
		fs.Send (string("227 Entering Passive Mode (").append(addr).append(").\x0d\x0a"));
		fs.AcceptDataConnection ();

		fs.Receive(command, type, param);
		QC_BOOST_CHECK_MESSAGE (type == FtpServer::CMD_RETR, L"Error\n");
		QC_BOOST_CHECK_MESSAGE (param == "folder/subfolder/base003.avc", L"Error\n");
		fs.Send ("125 Data connection already open; Transfer starting.\x0d\x0a");
		int i;
		for (i = 0; i < 100; i++)
		{
			Sleep (i*1000);
			fs.SendData ("0");
			if ( fs.CheckDataConnectionClosed(500) ) break;
		}

		QC_BOOST_CHECK_MESSAGE ( fs.CheckControlConnectionClosed (), L"Error\n" );
		QC_BOOST_CHECK_MESSAGE (i >= 8, L"Incorrect timeout\n");
		
		WaitForSingleObject (hDoDownload, INFINITE);
		QC_BOOST_CHECK_MESSAGE ( CORE_DOWNLOAD_TIMEOUT == do_dld.GetError(), L"Wrong return code\n" );

	}
}

void TestFtpTransport::TestReusingSessionPassive ()
{
	using namespace KLUPD;
	std::wcout<<L"TestReusingSessionPassive\n";
	Logger::Case _case (m_logger, m_testp, m_step, L"TestReusingSessionPassive");
	
		{//getting 2 files (passive mode)
			FtpServer fs (2121, "127.0.0.1");
			fs.Start();

			_helper::DeleteDirectory(TEST_FOLDER);
			_helper::MakeDirectory(TEST_FOLDER);
			
			TestLog testLog;
			Callbacks cbacks;
			TestCallbackInterface testCallback (&testLog, &cbacks);
			TestJournalInterface testJournal;
			Signature6Checker s6c (&testLog);

			FileInfo _fi (L"base003.avc", L"/");

			UpdaterConfiguration data(1, 1, L"1");
			data.connect_tmo = 600;
			data.passive_ftp = true;
			testCallback.SetUpdaterConfiguration (data);
			

			Updater upd (testCallback.m_dp, testCallback, testJournal, &testLog);
			HANDLE hDoDownload;
			doSelfNetDownloader do_dld(&upd, hDoDownload, _fi, _fi.m_relativeURLPath, L"h:\\test\\", L"ftp://127.0.0.1:2121");
			

			string command;
			string param;
			FtpServer::CMD_TYPE type;

			fs.Accept();
			fs.Send ("220 Hello\x0d\x0a");
			
			fs.Receive(command, type, param);
			QC_BOOST_CHECK_MESSAGE (type == FtpServer::CMD_USER, L"Error\n");
			fs.Send ("331 Anonymous access allowed, send identity (e-mail name) as password.\x0d\x0a");
			fs.Receive(command, type, param);
			QC_BOOST_CHECK_MESSAGE (type == FtpServer::CMD_PASS, L"Error\n");
			fs.Send ("230 Logged in.\x0d\x0a");
			fs.Receive(command, type, param);
			QC_BOOST_CHECK_MESSAGE (type == FtpServer::CMD_TTYPE, L"Error\n");
			fs.Send ("200 Command okay.\x0d\x0a");
			fs.Receive(command, type, param);
			QC_BOOST_CHECK_MESSAGE (type == FtpServer::CMD_EPSV, L"Error\n");
			fs.Send ("522 Error.\x0d\x0a");
			fs.Receive(command, type, param);
			QC_BOOST_CHECK_MESSAGE (type == FtpServer::CMD_PASV, L"Error\n");

			string addr;
			fs.StartListeningPassiveDataPort (addr);
			fs.Send (string("227 Entering Passive Mode (").append(addr).append(").\x0d\x0a"));
			fs.AcceptDataConnection ();

			fs.Receive(command, type, param);
			QC_BOOST_CHECK_MESSAGE (type == FtpServer::CMD_RETR, L"Error\n");
			QC_BOOST_CHECK_MESSAGE (param == "base003.avc", L"Error\n");
			fs.Send ("125 Data connection already open; Transfer starting.\x0d\x0a");
			fs.SendData ("0123456789");
			fs.CloseDataConnection ();
			fs.Send ("226 Transfer complete.\x0d\x0a");
			
			
			WaitForSingleObject (hDoDownload, INFINITE);
			QC_BOOST_CHECK_MESSAGE ( CORE_NO_ERROR == do_dld.GetError(), L"Wrong return code\n" );
			QC_BOOST_CHECK_MESSAGE ( _helper::checkFileContent (wstring(TEST_FOLDER).append (L"\\base003.avc"),
											"0123456789"), L"Wrong return code\n" );	

			for (int i = 1; i< 1000; ++i)
			{
				//2
				_helper::DeleteDirectory(TEST_FOLDER);
				_helper::MakeDirectory(TEST_FOLDER);

				FileInfo _fi2 (L"base004.avc", L"/");
				doSelfNetDownloader do_dld2(&upd, hDoDownload, _fi2, _fi2.m_relativeURLPath, L"h:\\test\\", L"ftp://127.0.0.1:2121");
				
				//fs.Receive(command, type, param);
				//QC_BOOST_CHECK_MESSAGE (type == FtpServer::CMD_EPSV, L"Error\n");
				//fs.Send ("500 Error.\x0d\x0a");
				fs.Receive(command, type, param);
				QC_BOOST_CHECK_MESSAGE (type == FtpServer::CMD_PASV, L"Error\n");

				fs.StartListeningPassiveDataPort (addr);
				fs.Send (string("227 Entering Passive Mode (").append(addr).append(").\x0d\x0a"));
				fs.AcceptDataConnection ();

				fs.Receive(command, type, param);
				QC_BOOST_CHECK_MESSAGE (type == FtpServer::CMD_RETR, L"Error\n");
				QC_BOOST_CHECK_MESSAGE (param == "base004.avc", L"Error\n");
				fs.Send ("125 Data connection already open; Transfer starting.\x0d\x0a");
				fs.SendData ("abcdefgh");
				fs.CloseDataConnection ();
				fs.Send ("226 Transfer complete.\x0d\x0a");
				
				
				WaitForSingleObject (hDoDownload, INFINITE);
				QC_BOOST_CHECK_MESSAGE ( CORE_NO_ERROR == do_dld2.GetError(), L"Wrong return code\n" );
				QC_BOOST_CHECK_MESSAGE ( _helper::checkFileContent (wstring(TEST_FOLDER).append (L"\\base004.avc"),
												"abcdefgh"), L"Wrong return code\n" );
			}
		}

}

void TestFtpTransport::TestReusingSessionDifferentServers ()
{
	using namespace KLUPD;
	std::wcout<<L"TestReusingSessionDifferentServers\n";
	Logger::Case _case (m_logger, m_testp, m_step, L"TestReusingSessionDifferentServers");
	
	FtpServer fs (2121, "127.0.0.1");
	fs.Start();
	FtpServer fs2 (3131, "127.0.0.1");
	fs2.Start();

	TestLog testLog;
	Callbacks cbacks;
	TestCallbackInterface testCallback (&testLog, &cbacks);
	TestJournalInterface testJournal;
	Signature6Checker s6c (&testLog);
	UpdaterConfiguration data(1, 1, L"1");
	data.connect_tmo = 600;
	data.passive_ftp = true;
	testCallback.SetUpdaterConfiguration (data);
	Updater upd (testCallback.m_dp, testCallback, testJournal, &testLog);
	HANDLE hDoDownload;

	string command;
	string param;
	FtpServer::CMD_TYPE type;

	for (int i = 0; i < 500; ++i)

		{
			_helper::DeleteDirectory(TEST_FOLDER);
			_helper::MakeDirectory(TEST_FOLDER);
			
			
			FileInfo _fi (L"base003.avc", L"/");	
			doSelfNetDownloader do_dld(&upd, hDoDownload, _fi, _fi.m_relativeURLPath, L"h:\\test\\", L"ftp://127.0.0.1:2121");

			if (i != 0)
			{
				fs2.Receive(command, type, param);
				QC_BOOST_CHECK_MESSAGE (type == FtpServer::CMD_QUIT, L"Error\n");
				fs2.CloseControlConnection ();
			}

			fs.Accept();
			fs.Send ("220 Hello\x0d\x0a");
			
			fs.Receive(command, type, param);
			QC_BOOST_CHECK_MESSAGE (type == FtpServer::CMD_USER, L"Error\n");
			fs.Send ("331 Anonymous access allowed, send identity (e-mail name) as password.\x0d\x0a");
			fs.Receive(command, type, param);
			QC_BOOST_CHECK_MESSAGE (type == FtpServer::CMD_PASS, L"Error\n");
			fs.Send ("230 Logged in.\x0d\x0a");
			fs.Receive(command, type, param);
			QC_BOOST_CHECK_MESSAGE (type == FtpServer::CMD_TTYPE, L"Error\n");
			fs.Send ("200 Command okay.\x0d\x0a");
			fs.Receive(command, type, param);
			QC_BOOST_CHECK_MESSAGE (type == FtpServer::CMD_EPSV, L"Error\n");
			fs.Send ("500 Error.\x0d\x0a");
			fs.Receive(command, type, param);
			QC_BOOST_CHECK_MESSAGE (type == FtpServer::CMD_PASV, L"Error\n");

			string addr;
			fs.StartListeningPassiveDataPort (addr);
			fs.Send (string("227 Entering Passive Mode (").append(addr).append(").\x0d\x0a"));
			fs.AcceptDataConnection ();

			fs.Receive(command, type, param);
			QC_BOOST_CHECK_MESSAGE (type == FtpServer::CMD_RETR, L"Error\n");
			QC_BOOST_CHECK_MESSAGE (param == "base003.avc", L"Error\n");
			fs.Send ("125 Data connection already open; Transfer starting.\x0d\x0a");
			fs.SendData ("0123456789");
			fs.CloseDataConnection ();
			fs.Send ("226 Transfer complete.\x0d\x0a");
						
			
			WaitForSingleObject (hDoDownload, INFINITE);
			QC_BOOST_CHECK_MESSAGE ( CORE_NO_ERROR == do_dld.GetError(), L"Wrong return code\n" );
			QC_BOOST_CHECK_MESSAGE ( _helper::checkFileContent (wstring(TEST_FOLDER).append (L"\\base003.avc"),
											"0123456789"), L"Wrong return code\n" );	

//2
			

			_helper::DeleteDirectory(TEST_FOLDER);
			_helper::MakeDirectory(TEST_FOLDER);
			
			FileInfo _fi2 (L"base004.avc", L"/");

			doSelfNetDownloader do_dld2(&upd, hDoDownload, _fi2, _fi2.m_relativeURLPath, L"h:\\test\\", L"ftp://127.0.0.1:3131");

			fs.Receive(command, type, param);
			QC_BOOST_CHECK_MESSAGE (type == FtpServer::CMD_QUIT, L"Error\n");
			fs.CloseControlConnection ();

			fs2.Accept();
			fs2.Send ("220 Hello\x0d\x0a");
			
			fs2.Receive(command, type, param);
			QC_BOOST_CHECK_MESSAGE (type == FtpServer::CMD_USER, L"Error\n");
			fs2.Send ("331 Anonymous access allowed, send identity (e-mail name) as password.\x0d\x0a");
			fs2.Receive(command, type, param);
			QC_BOOST_CHECK_MESSAGE (type == FtpServer::CMD_PASS, L"Error\n");
			fs2.Send ("230 Logged in.\x0d\x0a");
			fs2.Receive(command, type, param);
			QC_BOOST_CHECK_MESSAGE (type == FtpServer::CMD_TTYPE, L"Error\n");
			fs2.Send ("200 Command okay.\x0d\x0a");
			fs2.Receive(command, type, param);
			QC_BOOST_CHECK_MESSAGE (type == FtpServer::CMD_EPSV, L"Error\n");
			fs2.Send ("500 Error.\x0d\x0a");
			fs2.Receive(command, type, param);
			QC_BOOST_CHECK_MESSAGE (type == FtpServer::CMD_PASV, L"Error\n");

			fs2.StartListeningPassiveDataPort (addr);
			fs2.Send (string("227 Entering Passive Mode (").append(addr).append(").\x0d\x0a"));
			fs2.AcceptDataConnection ();

			fs2.Receive(command, type, param);
			QC_BOOST_CHECK_MESSAGE (type == FtpServer::CMD_RETR, L"Error\n");
			QC_BOOST_CHECK_MESSAGE (param == "base004.avc", L"Error\n");
			fs2.Send ("125 Data connection already open; Transfer starting.\x0d\x0a");
			fs2.SendData ("abcdefgh");
			fs2.CloseDataConnection ();
			fs2.Send ("226 Transfer complete.\x0d\x0a");
			
			WaitForSingleObject (hDoDownload, INFINITE);
			QC_BOOST_CHECK_MESSAGE ( CORE_NO_ERROR == do_dld2.GetError(), L"Wrong return code\n" );
			QC_BOOST_CHECK_MESSAGE ( _helper::checkFileContent (wstring(TEST_FOLDER).append (L"\\base004.avc"),
											"abcdefgh"), L"Wrong return code\n" );	
		}

}

void TestFtpTransport::TestReusingSessionActive ()
{
	using namespace KLUPD;
	std::wcout<<L"TestReusingSessionActive\n";
	Logger::Case _case (m_logger, m_testp, m_step, L"TestReusingSessionActive");
	
		{//getting 2 files (active mode)
			FtpServer fs (2121, "127.0.0.1");
			fs.Start();

			_helper::DeleteDirectory(TEST_FOLDER);
			_helper::MakeDirectory(TEST_FOLDER);
			
			TestLog testLog;
			Callbacks cbacks;
			TestCallbackInterface testCallback (&testLog, &cbacks);
			TestJournalInterface testJournal;
			Signature6Checker s6c (&testLog);

			FileInfo _fi (L"base003.avc", L"/");

			UpdaterConfiguration data(1, 1, L"1");
			data.connect_tmo = 600;
			data.passive_ftp = false;
			testCallback.SetUpdaterConfiguration (data);
			

			Updater upd (testCallback.m_dp, testCallback, testJournal, &testLog);
			HANDLE hDoDownload;
			doSelfNetDownloader do_dld(&upd, hDoDownload, _fi, _fi.m_relativeURLPath, L"h:\\test\\", L"ftp://127.0.0.1:2121");
			

			string command;
			string param;
			FtpServer::CMD_TYPE type;

			fs.Accept();
			fs.Send ("220 Hello\x0d\x0a");
			
			fs.Receive(command, type, param);
			QC_BOOST_CHECK_MESSAGE (type == FtpServer::CMD_USER, L"Error\n");
			fs.Send ("331 Anonymous access allowed, send identity (e-mail name) as password.\x0d\x0a");
			fs.Receive(command, type, param);
			QC_BOOST_CHECK_MESSAGE (type == FtpServer::CMD_PASS, L"Error\n");
			fs.Send ("230 Logged in.\x0d\x0a");
			fs.Receive(command, type, param);
			QC_BOOST_CHECK_MESSAGE (type == FtpServer::CMD_TTYPE, L"Error\n");
			fs.Send ("200 Command okay.\x0d\x0a");
			fs.Receive(command, type, param);
			QC_BOOST_CHECK_MESSAGE (type == FtpServer::CMD_EPRT, L"Error\n");
			fs.Send ("500 Error.\x0d\x0a");
			fs.Receive(command, type, param);
			QC_BOOST_CHECK_MESSAGE (type == FtpServer::CMD_PORT, L"Error\n");
			fs.Send ("200 PORT command okay.\x0d\x0a");

			fs.EstablishActiveConnection(param);

			fs.Receive(command, type, param);
			QC_BOOST_CHECK_MESSAGE (type == FtpServer::CMD_RETR, L"Error\n");
			fs.Send ("125 Data connection already open; Transfer starting.\x0d\x0a");
				fs.SendData ("0123456789");
			fs.CloseDataConnection ();
			fs.Send ("226 Transfer complete.\x0d\x0a");
			
			
			WaitForSingleObject (hDoDownload, INFINITE);
			QC_BOOST_CHECK_MESSAGE ( CORE_NO_ERROR == do_dld.GetError(), L"Wrong return code\n" );
			QC_BOOST_CHECK_MESSAGE ( _helper::checkFileContent (wstring(TEST_FOLDER).append (L"\\base003.avc"),
											"0123456789"), L"Wrong return code\n" );	

			for (int i = 1; i< 1000; ++i)
			{
				//2
				_helper::DeleteDirectory(TEST_FOLDER);
				_helper::MakeDirectory(TEST_FOLDER);

				FileInfo _fi2 (L"base004.avc", L"/");
				doSelfNetDownloader do_dld2(&upd, hDoDownload, _fi2, _fi2.m_relativeURLPath, L"h:\\test\\", L"ftp://127.0.0.1:2121");
				
				//fs.Receive(command, type, param);
				//QC_BOOST_CHECK_MESSAGE (type == FtpServer::CMD_EPRT, L"Error\n");
				//fs.Send ("500 Error.\x0d\x0a");
				fs.Receive(command, type, param);
				QC_BOOST_CHECK_MESSAGE (type == FtpServer::CMD_PORT, L"Error\n");
				fs.Send ("200 PORT command okay.\x0d\x0a");
				fs.EstablishActiveConnection(param);
				fs.Receive(command, type, param);
				QC_BOOST_CHECK_MESSAGE (type == FtpServer::CMD_RETR, L"Error\n");
				fs.Send ("125 Data connection already open; Transfer starting.\x0d\x0a");fs.SendData ("abcdefgh");
				fs.CloseDataConnection ();
				fs.Send ("226 Transfer complete.\x0d\x0a");
				
				
				WaitForSingleObject (hDoDownload, INFINITE);
				QC_BOOST_CHECK_MESSAGE ( CORE_NO_ERROR == do_dld2.GetError(), L"Wrong return code\n" );
				QC_BOOST_CHECK_MESSAGE ( _helper::checkFileContent (wstring(TEST_FOLDER).append (L"\\base004.avc"),
												"abcdefgh"), L"Wrong return code\n" );
			}
		}

}

void TestFtpTransport::TestTimeoutAfterTransferringData ()
{
	using namespace KLUPD;
	std::wcout<<L"TestTimeoutAfterTransferringData\n";
	Logger::Case _case (m_logger, m_testp, m_step, L"TestTimeoutAfterTransferringData");
	{//
		FtpServer fs (2121, "127.0.0.1");
		fs.Start();

		_helper::DeleteDirectory(TEST_FOLDER);
		_helper::MakeDirectory(TEST_FOLDER);
		
		TestLog testLog;
		Callbacks cbacks;
		TestCallbackInterface testCallback (&testLog, &cbacks);
		TestJournalInterface testJournal;
		Signature6Checker s6c (&testLog);

		FileInfo _fi (L"base003.avc", L"/folder/subfolder/");

		UpdaterConfiguration data(1, 1, L"1");
		data.connect_tmo = 10; //seconds
		data.passive_ftp = true;
		testCallback.SetUpdaterConfiguration (data);
		
		Updater upd (testCallback.m_dp, testCallback, testJournal, &testLog);
		HANDLE hDoDownload;
		doSelfNetDownloader do_dld(&upd, hDoDownload, _fi, _fi.m_relativeURLPath, L"h:\\test\\", L"ftp://127.0.0.1:2121");
		

		string command;
		string param;
		FtpServer::CMD_TYPE type;

		fs.Accept();
		fs.Send ("220 Hello\x0d\x0a");
		
		fs.Receive(command, type, param);
		QC_BOOST_CHECK_MESSAGE (type == FtpServer::CMD_USER, L"Error\n");
		fs.Send ("331 Anonymous access allowed, send identity (e-mail name) as password.\x0d\x0a");
		fs.Receive(command, type, param);
		QC_BOOST_CHECK_MESSAGE (type == FtpServer::CMD_PASS, L"Error\n");
		fs.Send ("230 Logged in.\x0d\x0a");
		fs.Receive(command, type, param);
		QC_BOOST_CHECK_MESSAGE (type == FtpServer::CMD_TTYPE, L"Error\n");
		fs.Send ("200 Command okay.\x0d\x0a");
		fs.Receive(command, type, param);
		QC_BOOST_CHECK_MESSAGE (type == FtpServer::CMD_EPSV, L"Error\n");
		fs.Send ("500 Error.\x0d\x0a");
		fs.Receive(command, type, param);
		QC_BOOST_CHECK_MESSAGE (type == FtpServer::CMD_PASV, L"Error\n");

		string addr;
		fs.StartListeningPassiveDataPort (addr);
		fs.Send (string("227 Entering Passive Mode (").append(addr).append(").\x0d\x0a"));
		fs.AcceptDataConnection ();

		fs.Receive(command, type, param);
		QC_BOOST_CHECK_MESSAGE (type == FtpServer::CMD_RETR, L"Error\n");
		QC_BOOST_CHECK_MESSAGE (param == "folder/subfolder/base003.avc", L"Error\n");
		fs.Send ("125 Data connection already open; Transfer starting.\x0d\x0a");
		fs.SendData ("0123456789");
		fs.CloseDataConnection ();
		int i = 0;
		for (i = 0; i < 100; i++)
		{
			Sleep (1000);
			if ( fs.CheckControlConnectionClosed(0) ) break;
		}

		//fs.Receive(command, type, param);
		//QC_BOOST_CHECK_MESSAGE (type == FtpServer::CMD_QUIT, L"Error\n");

		QC_BOOST_CHECK_MESSAGE (i >= 10, L"Incorrect timeout\n");
		
		WaitForSingleObject (hDoDownload, INFINITE);
		QC_BOOST_CHECK_MESSAGE ( CORE_DOWNLOAD_TIMEOUT == do_dld.GetError(), L"Wrong return code\n" );

	}
}

void TestFtpTransport::TestUserPassword ()
{
	using namespace KLUPD;
	std::wcout<<L"TestUserPassword\n";
	Logger::Case _case (m_logger, m_testp, m_step, L"TestUserPassword");	

	{//getting file (passive mode)
		FtpServer fs (2121, "127.0.0.1");
		fs.Start();

		_helper::DeleteDirectory(TEST_FOLDER);
		_helper::MakeDirectory(TEST_FOLDER);
		
		TestLog testLog;
		Callbacks cbacks;
		TestCallbackInterface testCallback (&testLog, &cbacks);
		TestJournalInterface testJournal;
		Signature6Checker s6c (&testLog);

		FileInfo _fi (L"base003.avc", L"/");
		_fi.m_localPath = L"h:\\test\\";

		UpdaterConfiguration data(1, 1, L"1");
		data.connect_tmo = 600;
		data.passive_ftp = true;
		testCallback.SetUpdaterConfiguration (data);
		
		HANDLE hDoDownload;

		data.m_sourceList.addSource (UST_UserURL, L"ftp://user:pass@127.0.0.1:2121", false, L"diffs");
		testCallback.SetUpdaterConfiguration (data);
		Updater upd (testCallback.m_dp, testCallback, testJournal, &testLog);
		upd.constructSourceList(testCallback.updaterConfiguration().m_sourceList, upd.m_sourceList);

		doEntryDownloader do_dld(&upd, hDoDownload, _fi, _fi.m_localPath, &s6c);	

		string command;
		string param;
		FtpServer::CMD_TYPE type;

		fs.Accept();
		fs.Send ("220 Hello\x0d\x0a");
		
		fs.Receive(command, type, param);
		QC_BOOST_CHECK_MESSAGE (type == FtpServer::CMD_USER, L"Error\n");
		QC_BOOST_CHECK_MESSAGE (param == "user", L"Error\n");
		fs.Send ("331 Anonymous access allowed, send identity (e-mail name) as password.\x0d\x0a");
		fs.Receive(command, type, param);
		QC_BOOST_CHECK_MESSAGE (type == FtpServer::CMD_PASS, L"Error\n");
		QC_BOOST_CHECK_MESSAGE (param == "pass", L"Error\n");
		fs.Send ("230 Logged in.\x0d\x0a");
		fs.Receive(command, type, param);
		QC_BOOST_CHECK_MESSAGE (type == FtpServer::CMD_TTYPE, L"Error\n");
		fs.Send ("200 Command okay.\x0d\x0a");
		fs.Receive(command, type, param);
		QC_BOOST_CHECK_MESSAGE (type == FtpServer::CMD_EPSV, L"Error\n");
		fs.Send ("500 Do not understand.\x0d\x0a");
		fs.Receive(command, type, param);
		QC_BOOST_CHECK_MESSAGE (type == FtpServer::CMD_PASV, L"Error\n");

		string addr;
		fs.CreateBindListeningPassiveDataPort (addr);
		fs.StartListeningPassiveDataPortOnly ();
		fs.Send (string("227 Entering Passive Mode (").append(addr).append(").\x0d\x0a"));
		fs.AcceptDataConnection ();

		fs.Receive(command, type, param);
		QC_BOOST_CHECK_MESSAGE (type == FtpServer::CMD_RETR, L"Error\n");
		fs.Send ("125 Data connection already open; Transfer starting.\x0d\x0a");
		fs.SendData ("0123456789");
		fs.CloseDataConnection ();
		fs.Send ("226 Transfer complete.\x0d\x0a");
		
		

		WaitForSingleObject (hDoDownload, INFINITE);
		QC_BOOST_CHECK_MESSAGE ( CORE_NO_ERROR == do_dld.GetError(), L"Wrong return code\n" );
		QC_BOOST_CHECK_MESSAGE ( _helper::checkFileContent (wstring(TEST_FOLDER).append (L"\\base003.avc"),
										"0123456789"), L"Wrong return code\n" );
		
	}
}

void TestFtpTransport::TestTryActiveIfPassiveFails ()
{
	using namespace KLUPD;
	std::wcout<<L"TestTryActiveIfPassiveFails\n";
	Logger::Case _case (m_logger, m_testp, m_step, L"TestTryActiveIfPassiveFails");

	// signalling by not accepting connection (firewall)
	{//using passive mode
	FtpServer fs (2121, "127.0.0.1");
	fs.Start();

	_helper::DeleteDirectory(TEST_FOLDER);
	_helper::MakeDirectory(TEST_FOLDER);

	TestLog testLog;
	Callbacks cbacks;
	TestCallbackInterface testCallback (&testLog, &cbacks);
	TestJournalInterface testJournal;
	Signature6Checker s6c (&testLog);

	FileInfo _fi (L"base003.avc", L"/");

	UpdaterConfiguration data(1, 1, L"1");
	data.connect_tmo = 600;
	data.passive_ftp = true;
	data.m_tryActiveFtpIfPassiveFails = true;
	testCallback.SetUpdaterConfiguration (data);

	Updater upd (testCallback.m_dp, testCallback, testJournal, &testLog);
	HANDLE hDoDownload;
	doSelfNetDownloader do_dld(&upd, hDoDownload, _fi, _fi.m_relativeURLPath, L"h:\\test\\", L"ftp://127.0.0.1:2121");

	string command;
	string param;
	FtpServer::CMD_TYPE type;

	fs.Accept();
	fs.Send ("220 Hello\x0d\x0a");

	fs.Receive(command, type, param);
	QC_BOOST_CHECK_MESSAGE (type == FtpServer::CMD_USER, L"Error\n");
	fs.Send ("331 Anonymous access allowed, send identity (e-mail name) as password.\x0d\x0a");
	fs.Receive(command, type, param);
	QC_BOOST_CHECK_MESSAGE (type == FtpServer::CMD_PASS, L"Error\n");
	fs.Send ("230 Logged in.\x0d\x0a");
	fs.Receive(command, type, param);
	QC_BOOST_CHECK_MESSAGE (type == FtpServer::CMD_TTYPE, L"Error\n");
	fs.Send ("200 Command okay.\x0d\x0a");
	fs.Receive(command, type, param);
	QC_BOOST_CHECK_MESSAGE (type == FtpServer::CMD_EPSV, L"Error\n");
	fs.Send ("500 Error.\x0d\x0a");
	fs.Receive(command, type, param);
	QC_BOOST_CHECK_MESSAGE (type == FtpServer::CMD_PASV, L"Error\n");

	string addr;
	fs.CreateBindListeningPassiveDataPort (addr);
	fs.CloseDataConnectionImpolite ();
	fs.Send (string("227 Entering Passive Mode (").append(addr).append(").\x0d\x0a"));

	//fs.Receive(command, type, param);
	//QC_BOOST_CHECK_MESSAGE (type == FtpServer::CMD_EPRT, L"Error\n");
	//fs.Send ("500 Error.\x0d\x0a");
	fs.Receive(command, type, param);
	QC_BOOST_CHECK_MESSAGE (type == FtpServer::CMD_PORT, L"Error\n");
	fs.Send ("200 PORT command okay.\x0d\x0a");


	fs.EstablishActiveConnection(param);
	fs.Receive(command, type, param);
	QC_BOOST_CHECK_MESSAGE (type == FtpServer::CMD_RETR, L"Error\n");
	fs.Send ("125 Data connection already open; Transfer starting.\x0d\x0a");
	fs.SendData ("0123456789");
	fs.CloseDataConnection ();
	fs.Send ("226 Transfer complete.\x0d\x0a");

	WaitForSingleObject (hDoDownload, INFINITE);
	QC_BOOST_CHECK_MESSAGE ( CORE_NO_ERROR == do_dld.GetError(), L"Wrong return code\n" );
	QC_BOOST_CHECK_MESSAGE ( _helper::checkFileContent (wstring(TEST_FOLDER).append (L"\\base003.avc"),
	"0123456789"), L"Wrong return code\n" );

	}

	// signalling by error code
	{//using passive mode
		FtpServer fs (2121, "127.0.0.1");
		fs.Start();

		_helper::DeleteDirectory(TEST_FOLDER);
		_helper::MakeDirectory(TEST_FOLDER);

		TestLog testLog;
		Callbacks cbacks;
		TestCallbackInterface testCallback (&testLog, &cbacks);
		TestJournalInterface testJournal;
		Signature6Checker s6c (&testLog);

		FileInfo _fi (L"base003.avc", L"/");

		UpdaterConfiguration data(1, 1, L"1");
		data.connect_tmo = 600;
		data.passive_ftp = true;
		data.m_tryActiveFtpIfPassiveFails = true;
		testCallback.SetUpdaterConfiguration (data);


		Updater upd (testCallback.m_dp, testCallback, testJournal, &testLog);
		HANDLE hDoDownload;
		doSelfNetDownloader do_dld(&upd, hDoDownload, _fi, _fi.m_relativeURLPath, L"h:\\test\\", L"ftp://127.0.0.1:2121");

		string command;
		string param;
		FtpServer::CMD_TYPE type;

		fs.Accept();
		fs.Send ("220 Hello\x0d\x0a");

		fs.Receive(command, type, param);
		QC_BOOST_CHECK_MESSAGE (type == FtpServer::CMD_USER, L"Error\n");
		fs.Send ("331 Anonymous access allowed, send identity (e-mail name) as password.\x0d\x0a");
		fs.Receive(command, type, param);
		QC_BOOST_CHECK_MESSAGE (type == FtpServer::CMD_PASS, L"Error\n");
		fs.Send ("230 Logged in.\x0d\x0a");
		fs.Receive(command, type, param);
		QC_BOOST_CHECK_MESSAGE (type == FtpServer::CMD_TTYPE, L"Error\n");
		fs.Send ("200 Command okay.\x0d\x0a");
		fs.Receive(command, type, param);
		QC_BOOST_CHECK_MESSAGE (type == FtpServer::CMD_EPSV, L"Error\n");
		fs.Send ("500 Error.\x0d\x0a");
		fs.Receive(command, type, param);
		QC_BOOST_CHECK_MESSAGE (type == FtpServer::CMD_PASV, L"Error\n");

		string addr;
		fs.CreateBindListeningPassiveDataPort (addr);
		fs.Send (string("227 Entering Passive Mode (").append(addr).append(").\x0d\x0a"));

		//fs.Receive(command, type, param);
		//QC_BOOST_CHECK_MESSAGE (type == FtpServer::CMD_EPRT, L"Error\n");
		//fs.Send ("500 Error.\x0d\x0a");
		fs.Receive(command, type, param);
		QC_BOOST_CHECK_MESSAGE (type == FtpServer::CMD_PORT, L"Error\n");
		fs.Send ("200 PORT command okay.\x0d\x0a");

		fs.CloseDataConnectionImpolite ();

		fs.EstablishActiveConnection(param);
		fs.Receive(command, type, param);
		QC_BOOST_CHECK_MESSAGE (type == FtpServer::CMD_RETR, L"Error\n");
		fs.Send ("125 Data connection already open; Transfer starting.\x0d\x0a");
		fs.SendData ("0123456789");
		fs.CloseDataConnection ();
		fs.Send ("226 Transfer complete.\x0d\x0a");

		WaitForSingleObject (hDoDownload, INFINITE);
		QC_BOOST_CHECK_MESSAGE ( CORE_NO_ERROR == do_dld.GetError(), L"Wrong return code\n" );
		QC_BOOST_CHECK_MESSAGE ( _helper::checkFileContent (wstring(TEST_FOLDER).append (L"\\base003.avc"),
							"0123456789"), L"Wrong return code\n" );

	}

}

void TestFtpTransport::TestAuthorizationCases ()
{
	using namespace KLUPD;
	std::wcout<<L"TestAuthorizationCases\n";
	Logger::Case _case (m_logger, m_testp, m_step, L"TestAuthorizationCases");

	{// check url with empty password, новый баг
	 // bug 24684
		FtpServer fs (2121, "127.0.0.1");
		fs.Start();

		_helper::DeleteDirectory(TEST_FOLDER);
		_helper::MakeDirectory(TEST_FOLDER);

		TestLog testLog;
		Callbacks cbacks;
		TestCallbackInterface testCallback (&testLog, &cbacks);
		TestJournalInterface testJournal;
		Signature6Checker s6c (&testLog);

		FileInfo _fi (L"base003.avc", L"/");

		UpdaterConfiguration data(1, 1, L"1");
		data.connect_tmo = 600;
		data.passive_ftp = true;
		testCallback.SetUpdaterConfiguration (data);

		Updater upd (testCallback.m_dp, testCallback, testJournal, &testLog);
		HANDLE hDoDownload;
		doSelfNetDownloader do_dld(&upd, hDoDownload, _fi, _fi.m_relativeURLPath, L"h:\\test\\", L"ftp://user:@127.0.0.1:2121");

		//doSelfNetDownloader do_dld(&upd, hDoDownload, _fi, _fi.m_relativeURLPath, L"h:\\test\\", L"ftp://megauser:@tl-srv-wnt:21");
		
		string command;
		string param;
		FtpServer::CMD_TYPE type;

		fs.Accept();
		fs.Send ("220 Hello\x0d\x0a");

		fs.Receive(command, type, param);
		QC_BOOST_CHECK_MESSAGE (type == FtpServer::CMD_USER, L"Error\n");
		fs.Send ("331 Anonymous access allowed, send identity (e-mail name) as password.\x0d\x0a");
		fs.Receive(command, type, param);
		QC_BOOST_CHECK_MESSAGE (type == FtpServer::CMD_PASS, L"Error\n");
		QC_BOOST_CHECK_MESSAGE (command == "pass", L"Error\n");
		fs.Send ("230 Logged in.\x0d\x0a");
		fs.Receive(command, type, param);
		QC_BOOST_CHECK_MESSAGE (type == FtpServer::CMD_TTYPE, L"Error\n");
		fs.Send ("200 Command okay.\x0d\x0a");
		fs.Receive(command, type, param);
		QC_BOOST_CHECK_MESSAGE (type == FtpServer::CMD_EPSV, L"Error\n");
		fs.Send ("500 Error.\x0d\x0a");
		fs.Receive(command, type, param);
		QC_BOOST_CHECK_MESSAGE (type == FtpServer::CMD_PASV, L"Error\n");

		string addr;
		fs.CreateBindListeningPassiveDataPort (addr);
		fs.StartListeningPassiveDataPortOnly ();
		fs.Send (string("227 Entering Passive Mode (").append(addr).append(").\x0d\x0a"));
		fs.AcceptDataConnection ();

		fs.Receive(command, type, param);
		QC_BOOST_CHECK_MESSAGE (type == FtpServer::CMD_RETR, L"Error\n");
		fs.Send ("125 Data connection already open; Transfer starting.\x0d\x0a");
		fs.SendData ("0123456789");
		fs.CloseDataConnection ();
		fs.Send ("226 Transfer complete.\x0d\x0a");

		WaitForSingleObject (hDoDownload, INFINITE);
		QC_BOOST_CHECK_MESSAGE ( CORE_NO_ERROR == do_dld.GetError(), L"Wrong return code\n" );
		QC_BOOST_CHECK_MESSAGE ( _helper::checkFileContent (wstring(TEST_FOLDER).append (L"\\base003.avc"),
								"0123456789"), L"Wrong return code\n" );

	}

	{//check using credentials while downloading a sequence of files
	 //bug 24565
		TestLog testLog;

		FtpServer fs (2121, "127.0.0.1");
		fs.Start();
		FtpServer fs2 (2122, "127.0.0.1");
		fs2.Start();

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

		data.m_sourceList.addSource (UST_UserURL, L"ftp://megauser:megapassword@127.0.0.1:2121/", false, L"");
		data.m_sourceList.addSource (UST_UserURL, L"ftp://megauser2:megapassword2@127.0.0.1:2122/", false, L"");
		data.m_sourceList.addSource (UST_UserURL, L"ftp://megauser3:megapassword3@127.0.0.1:2122/", false, L"");
		data.UpdaterListFlags.update_bases = true;

		Callbacks cbacks;
		TestCallbackInterface testCallback (&testLog, &cbacks);
		TestJournalInterface testJournal;
		testCallback.SetUpdaterConfiguration (data);

		Updater upd (testCallback.m_dp, testCallback, testJournal, &testLog);

		cbacks.bFlags[Callbacks::requestCredentialsForAuthorizationOnProxy] = true;

		HANDLE hDoUpdate;
		doUpdater do_upd(&upd, hDoUpdate);
		wstring rs;

		//source 1 - tree structure
		fs.Hello();
		QC_BOOST_CHECK_MESSAGE (fs.Authorize ("megauser", "megapassword", rs), rs.c_str());
		QC_BOOST_CHECK_MESSAGE (fs.ProcessTypeCommand (rs), rs.c_str());
		QC_BOOST_CHECK_MESSAGE (fs.SendFileFromBuffer ("index/6/u0607g.xml.klz", "0123456789", false, rs), rs.c_str());
		QC_BOOST_CHECK_MESSAGE (fs.SendFileFromBuffer ("index/6/u0607g.xml", "0123456789", false, rs), rs.c_str());
		fs.CheckControlConnectionClosed();
		
		//source 2 - plain structure
		fs2.Hello();
		QC_BOOST_CHECK_MESSAGE (fs2.Authorize ("megauser2", "megapassword2", rs), rs.c_str());
		QC_BOOST_CHECK_MESSAGE (fs2.ProcessTypeCommand (rs), rs.c_str());
		QC_BOOST_CHECK_MESSAGE (fs2.SendFileNotFound ("index/6/u0607g.xml.klz", false, rs), rs.c_str());
		QC_BOOST_CHECK_MESSAGE (fs2.SendFileFromBuffer ("u0607g.xml.klz", "0123456789", false, rs), rs.c_str());
		QC_BOOST_CHECK_MESSAGE (fs2.SendFileNotFound ("index/6/u0607g.xml", false, rs), rs.c_str());
		QC_BOOST_CHECK_MESSAGE (fs2.SendFileFromBuffer ("u0607g.xml", "0123456789", false, rs), rs.c_str());
		fs2.CheckControlConnectionClosed();

		//source 3 - plain structure

		fs2.Hello();
		QC_BOOST_CHECK_MESSAGE (fs2.Authorize ("megauser3", "megapassword3", rs), rs.c_str());
		QC_BOOST_CHECK_MESSAGE (fs2.ProcessTypeCommand (rs), rs.c_str());
		QC_BOOST_CHECK_MESSAGE (fs2.SendFileNotFound ("index/6/u0607g.xml.klz", false, rs), rs.c_str());
		QC_BOOST_CHECK_MESSAGE (fs2.SendFileFromBuffer ("u0607g.xml.klz", "0123456789", false, rs), rs.c_str());
		QC_BOOST_CHECK_MESSAGE (fs2.SendFileNotFound ("index/6/u0607g.xml", false, rs), rs.c_str());
		QC_BOOST_CHECK_MESSAGE (fs2.SendFileFromFile ("u0607g.xml",
			std::wstring(COLL_ROOT).append(L"\\Bases_1\\index\\6\\u0607g.xml"), false, rs), rs.c_str());
		QC_BOOST_CHECK_MESSAGE (fs2.SendFileNotFound ("av-i386-0607g.xml.klz", false, rs), rs.c_str());
		QC_BOOST_CHECK_MESSAGE (fs2.SendFileFromFile ("av-i386-0607g.xml",
			std::wstring(COLL_ROOT).append(L"\\Bases_1\\bases\\av\\avc\\i386\\av-i386-0607g.xml"), false, rs), rs.c_str());
		QC_BOOST_CHECK_MESSAGE (fs2.SendFileNotFound ("blst-0607g.xml.klz", false, rs), rs.c_str());
		QC_BOOST_CHECK_MESSAGE (fs2.SendFileNotFound ("blst-0607g.xml", false, rs), rs.c_str());
		//try the same source
		QC_BOOST_CHECK_MESSAGE (fs2.SendFileNotFound ("index/6/u0607g.xml.klz", false, rs), rs.c_str());
		QC_BOOST_CHECK_MESSAGE (fs2.SendFileNotFound ("u0607g.xml.klz", false, rs), rs.c_str());
		QC_BOOST_CHECK_MESSAGE (fs2.SendFileNotFound ("index/6/u0607g.xml", false, rs), rs.c_str());
		QC_BOOST_CHECK_MESSAGE (fs2.SendFileNotFound ("u0607g.xml", false, rs), rs.c_str());
		
		WaitForSingleObject (hDoUpdate, INFINITE);
		QC_BOOST_CHECK_MESSAGE ( CORE_NO_SOURCE_FILE == do_upd.GetError(), L"Wrong return code\n" );
	}

}

void TestFtpTransport::TestExtendedModeIp4_Active ()
{//2 variants
	using namespace KLUPD;
	std::wcout<<L"TestExtendedModeIp4_Active\n";
	Logger::Case _case (m_logger, m_testp, m_step, L"TestExtendedModeIp4_Active");	

	{//getting file (active mode)
		FtpServer fs (2121, "127.0.0.1");
		fs.Start();

		_helper::DeleteDirectory(TEST_FOLDER);
		_helper::MakeDirectory(TEST_FOLDER);

		TestLog testLog;
		Callbacks cbacks;
		TestCallbackInterface testCallback (&testLog, &cbacks);
		TestJournalInterface testJournal;
		Signature6Checker s6c (&testLog);

		FileInfo _fi (L"base003.avc", L"/");

		UpdaterConfiguration data(1, 1, L"1");
		data.connect_tmo = 600;
		data.passive_ftp = false;
		testCallback.SetUpdaterConfiguration (data);


		Updater upd (testCallback.m_dp, testCallback, testJournal, &testLog);
		HANDLE hDoDownload;
		doSelfNetDownloader do_dld(&upd, hDoDownload, _fi, _fi.m_relativeURLPath, L"h:\\test\\", L"ftp://127.0.0.1:2121");


		string command;
		string param;
		FtpServer::CMD_TYPE type;

		fs.Accept();
		fs.Send ("220 Hello\x0d\x0a");

		fs.Receive(command, type, param);
		QC_BOOST_CHECK_MESSAGE (type == FtpServer::CMD_USER, L"Error\n");
		fs.Send ("331 Anonymous access allowed, send identity (e-mail name) as password.\x0d\x0a");
		fs.Receive(command, type, param);
		QC_BOOST_CHECK_MESSAGE (type == FtpServer::CMD_PASS, L"Error\n");
		fs.Send ("230 Logged in.\x0d\x0a");
		fs.Receive(command, type, param);
		QC_BOOST_CHECK_MESSAGE (type == FtpServer::CMD_TTYPE, L"Error\n");
		fs.Send ("200 Command okay.\x0d\x0a");
		fs.Receive(command, type, param);
		QC_BOOST_CHECK_MESSAGE (type == FtpServer::CMD_EPRT, L"Error\n");
		fs.Send ("200 EPRT command okay.\x0d\x0a");

		fs.EstablishActiveConnectionEx(param);

		fs.Receive(command, type, param);
		QC_BOOST_CHECK_MESSAGE (type == FtpServer::CMD_RETR, L"Error\n");
		fs.Send ("125 Data connection already open; Transfer starting.\x0d\x0a");
		fs.SendData ("0123456789");
		fs.CloseDataConnection ();
		fs.Send ("226 Transfer complete.\x0d\x0a");

		WaitForSingleObject (hDoDownload, INFINITE);
		QC_BOOST_CHECK_MESSAGE ( CORE_NO_ERROR == do_dld.GetError(), L"Wrong return code\n" );
		QC_BOOST_CHECK_MESSAGE ( _helper::checkFileContent (wstring(TEST_FOLDER).append (L"\\base003.avc"),
			"0123456789"), L"Wrong return code\n" );

	}

	{//getting file (active mode)
		FtpServer fs (2121, "127.0.0.1");
		fs.Start();

		_helper::DeleteDirectory(TEST_FOLDER);
		_helper::MakeDirectory(TEST_FOLDER);

		TestLog testLog;
		Callbacks cbacks;
		TestCallbackInterface testCallback (&testLog, &cbacks);
		TestJournalInterface testJournal;
		Signature6Checker s6c (&testLog);

		FileInfo _fi (L"base003.avc", L"/");

		UpdaterConfiguration data(1, 1, L"1");
		data.connect_tmo = 600;
		data.passive_ftp = false;
		testCallback.SetUpdaterConfiguration (data);


		Updater upd (testCallback.m_dp, testCallback, testJournal, &testLog);
		HANDLE hDoDownload;
		doSelfNetDownloader do_dld(&upd, hDoDownload, _fi, _fi.m_relativeURLPath, L"h:\\test\\", L"ftp://127.0.0.1:2121");


		string command;
		string param;
		FtpServer::CMD_TYPE type;

		fs.Accept();
		fs.Send ("220 Hello\x0d\x0a");

		fs.Receive(command, type, param);
		QC_BOOST_CHECK_MESSAGE (type == FtpServer::CMD_USER, L"Error\n");
		fs.Send ("331 Anonymous access allowed, send identity (e-mail name) as password.\x0d\x0a");
		fs.Receive(command, type, param);
		QC_BOOST_CHECK_MESSAGE (type == FtpServer::CMD_PASS, L"Error\n");
		fs.Send ("230 Logged in.\x0d\x0a");
		fs.Receive(command, type, param);
		QC_BOOST_CHECK_MESSAGE (type == FtpServer::CMD_TTYPE, L"Error\n");
		fs.Send ("200 Command okay.\x0d\x0a");
		fs.Receive(command, type, param);
		QC_BOOST_CHECK_MESSAGE (type == FtpServer::CMD_EPRT, L"Error\n");
		fs.Send ("200 EPRT command okay.\x0d\x0a");
		string addr = param;

		fs.Receive(command, type, param);
		QC_BOOST_CHECK_MESSAGE (type == FtpServer::CMD_RETR, L"Error\n");
		fs.Send ("150 File status okay; about to open data connection.\x0d\x0a");
		fs.EstablishActiveConnectionEx(addr);
		fs.SendData ("0123456789");
		fs.CloseDataConnection ();
		fs.Send ("226 Transfer complete.\x0d\x0a");

		WaitForSingleObject (hDoDownload, INFINITE);
		QC_BOOST_CHECK_MESSAGE ( CORE_NO_ERROR == do_dld.GetError(), L"Wrong return code\n" );
		QC_BOOST_CHECK_MESSAGE ( _helper::checkFileContent (wstring(TEST_FOLDER).append (L"\\base003.avc"),
			"0123456789"), L"Wrong return code\n" );

	}
}

void TestFtpTransport::TestExtendedModeIp4_Passive ()
{
	using namespace KLUPD;
	std::wcout<<L"TestExtendedModeIp4_Passive\n";
	Logger::Case _case (m_logger, m_testp, m_step, L"TestExtendedModeIp4_Passive");	

	{//getting file (passive mode)
		FtpServer fs (2121, "127.0.0.1");
		fs.Start();

		_helper::DeleteDirectory(TEST_FOLDER);
		_helper::MakeDirectory(TEST_FOLDER);

		TestLog testLog;
		Callbacks cbacks;
		TestCallbackInterface testCallback (&testLog, &cbacks);
		TestJournalInterface testJournal;
		Signature6Checker s6c (&testLog);

		FileInfo _fi (L"base003.avc", L"/");

		UpdaterConfiguration data(1, 1, L"1");
		data.connect_tmo = 600;
		data.passive_ftp = true;
		testCallback.SetUpdaterConfiguration (data);


		Updater upd (testCallback.m_dp, testCallback, testJournal, &testLog);
		HANDLE hDoDownload;
		doSelfNetDownloader do_dld(&upd, hDoDownload, _fi, _fi.m_relativeURLPath, L"h:\\test\\", L"ftp://127.0.0.1:2121");

		string command;
		string param;
		FtpServer::CMD_TYPE type;

		fs.Accept();
		fs.Send ("220 Hello\x0d\x0a");

		fs.Receive(command, type, param);
		QC_BOOST_CHECK_MESSAGE (type == FtpServer::CMD_USER, L"Error\n");
		fs.Send ("331 Anonymous access allowed, send identity (e-mail name) as password.\x0d\x0a");
		fs.Receive(command, type, param);
		QC_BOOST_CHECK_MESSAGE (type == FtpServer::CMD_PASS, L"Error\n");
		fs.Send ("230 Logged in.\x0d\x0a");
		fs.Receive(command, type, param);
		QC_BOOST_CHECK_MESSAGE (type == FtpServer::CMD_TTYPE, L"Error\n");
		fs.Send ("200 Command okay.\x0d\x0a");
		fs.Receive(command, type, param);
		QC_BOOST_CHECK_MESSAGE (type == FtpServer::CMD_EPSV, L"Error\n");

		string addr;
		fs.CreateBindListeningPassiveDataPortEx (addr);
		fs.StartListeningPassiveDataPortOnly ();
		fs.Send (string("229 Entering Passive Mode (").append(addr).append(").\x0d\x0a"));
		fs.AcceptDataConnection ();

		fs.Receive(command, type, param);
		QC_BOOST_CHECK_MESSAGE (type == FtpServer::CMD_RETR, L"Error\n");
		fs.Send ("125 Data connection already open; Transfer starting.\x0d\x0a");
		fs.SendData ("0123456789");
		fs.CloseDataConnection ();
		fs.Send ("226 Transfer complete.\x0d\x0a");

		WaitForSingleObject (hDoDownload, INFINITE);
		QC_BOOST_CHECK_MESSAGE ( CORE_NO_ERROR == do_dld.GetError(), L"Wrong return code\n" );
		QC_BOOST_CHECK_MESSAGE ( _helper::checkFileContent (wstring(TEST_FOLDER).append (L"\\base003.avc"),
			"0123456789"), L"Wrong return code\n" );

	}
}


/*
void TestFtpTransport::Draft ()
{//2 variants
	using namespace KLUPD;
	std::wcout<<L"TestDraft\n";
	Logger::Case _case (m_logger, m_testp, m_step, L"TestDraft");	

	{//getting file (active mode)
		//FtpServer fs (2121, "172.16.1.185", IP_V6);
		FtpServer fs (2121, IP_V6_ADDR_1, IP_V6);

		fs.Start();

		_helper::DeleteDirectory(TEST_FOLDER);
		_helper::MakeDirectory(TEST_FOLDER);

		TestLog testLog;
		Callbacks cbacks;
		TestCallbackInterface testCallback (&testLog, &cbacks);
		TestJournalInterface testJournal;
		Signature6Checker s6c (&testLog);

		FileInfo _fi (L"base003.avc", L"/");

		UpdaterConfiguration data(1, 1, L"1");
		data.connect_tmo = 600;
		data.passive_ftp = false;
		testCallback.SetUpdaterConfiguration (data);


		Updater upd (testCallback.m_dp, testCallback, testJournal, &testLog);
		HANDLE hDoDownload;

		doSelfNetDownloader do_dld(&upd, hDoDownload, _fi, _fi.m_relativeURLPath, L"h:\\test\\",
				wstringL"ftp://[dead::c596]:2121");


		string command;
		string param;
		FtpServer::CMD_TYPE type;

		fs.Accept();
		fs.Send ("220 Hello\x0d\x0a");

		fs.Receive(command, type, param);
		QC_BOOST_CHECK_MESSAGE (type == FtpServer::CMD_USER, L"Error\n");
		fs.Send ("331 Anonymous access allowed, send identity (e-mail name) as password.\x0d\x0a");
		fs.Receive(command, type, param);
		QC_BOOST_CHECK_MESSAGE (type == FtpServer::CMD_PASS, L"Error\n");
		fs.Send ("230 Logged in.\x0d\x0a");
		fs.Receive(command, type, param);
		QC_BOOST_CHECK_MESSAGE (type == FtpServer::CMD_TTYPE, L"Error\n");
		fs.Send ("200 Command okay.\x0d\x0a");
		fs.Receive(command, type, param);
		QC_BOOST_CHECK_MESSAGE (type == FtpServer::CMD_EPRT, L"Error\n");
		fs.Send ("200 EPRT command okay.\x0d\x0a");
		string addr = param;

		fs.EstablishActiveConnectionEx(param);

		fs.Receive(command, type, param);
		QC_BOOST_CHECK_MESSAGE (type == FtpServer::CMD_RETR, L"Error\n");
		fs.Send ("125 Data connection already open; Transfer starting.\x0d\x0a");
		fs.SendData ("0123456789");
		fs.CloseDataConnection ();
		fs.Send ("226 Transfer complete.\x0d\x0a");


		WaitForSingleObject (hDoDownload, INFINITE);
		QC_BOOST_CHECK_MESSAGE ( CORE_NO_ERROR == do_dld.GetError(), L"Wrong return code\n" );
		QC_BOOST_CHECK_MESSAGE ( _helper::checkFileContent (wstring(TEST_FOLDER).append (L"\\base003.avc"),
			"0123456789"), L"Wrong return code\n" );

	}
}
*/

void TestFtpTransport::TestExtendedModeIp6_Active ()
{//2 variants
	using namespace KLUPD;
	std::wcout<<L"TestExtendedModeIp6_Active\n";
	Logger::Case _case (m_logger, m_testp, m_step, L"TestExtendedModeIp6_Active");	

	{//getting file (active mode)
		FtpServer fs (2121, IP_V6_ADDR_1, IP_V6);
		fs.Start();

		_helper::DeleteDirectory(TEST_FOLDER);
		_helper::MakeDirectory(TEST_FOLDER);

		TestLog testLog;
		Callbacks cbacks;
		TestCallbackInterface testCallback (&testLog, &cbacks);
		TestJournalInterface testJournal;
		Signature6Checker s6c (&testLog);

		FileInfo _fi (L"base003.avc", L"/");

		UpdaterConfiguration data(1, 1, L"1");
		data.connect_tmo = 600;
		data.passive_ftp = false;
		testCallback.SetUpdaterConfiguration (data);


		Updater upd (testCallback.m_dp, testCallback, testJournal, &testLog);
		HANDLE hDoDownload;
		doSelfNetDownloader do_dld(&upd, hDoDownload, _fi, _fi.m_relativeURLPath, L"h:\\test\\",
			wstring(L"ftp://").append(IP_V6_ADDR_1_WSTR).append(L":2121").c_str());


		string command;
		string param;
		FtpServer::CMD_TYPE type;

		fs.Accept();
		fs.Send ("220 Hello\x0d\x0a");

		fs.Receive(command, type, param);
		QC_BOOST_CHECK_MESSAGE (type == FtpServer::CMD_USER, L"Error\n");
		fs.Send ("331 Anonymous access allowed, send identity (e-mail name) as password.\x0d\x0a");
		fs.Receive(command, type, param);
		QC_BOOST_CHECK_MESSAGE (type == FtpServer::CMD_PASS, L"Error\n");
		fs.Send ("230 Logged in.\x0d\x0a");
		fs.Receive(command, type, param);
		QC_BOOST_CHECK_MESSAGE (type == FtpServer::CMD_TTYPE, L"Error\n");
		fs.Send ("200 Command okay.\x0d\x0a");
		fs.Receive(command, type, param);
		QC_BOOST_CHECK_MESSAGE (type == FtpServer::CMD_EPRT, L"Error\n");
		fs.Send ("200 EPRT command okay.\x0d\x0a");

		fs.EstablishActiveConnectionEx(param);

		fs.Receive(command, type, param);
		QC_BOOST_CHECK_MESSAGE (type == FtpServer::CMD_RETR, L"Error\n");
		fs.Send ("125 Data connection already open; Transfer starting.\x0d\x0a");
		fs.SendData ("0123456789");
		fs.CloseDataConnection ();
		fs.Send ("226 Transfer complete.\x0d\x0a");

		WaitForSingleObject (hDoDownload, INFINITE);
		QC_BOOST_CHECK_MESSAGE ( CORE_NO_ERROR == do_dld.GetError(), L"Wrong return code\n" );
		QC_BOOST_CHECK_MESSAGE ( _helper::checkFileContent (wstring(TEST_FOLDER).append (L"\\base003.avc"),
			"0123456789"), L"Wrong return code\n" );

	}

	{//getting file (active mode)
		FtpServer fs (2121, IP_V6_ADDR_1, IP_V6);
		fs.Start();

		_helper::DeleteDirectory(TEST_FOLDER);
		_helper::MakeDirectory(TEST_FOLDER);

		TestLog testLog;
		Callbacks cbacks;
		TestCallbackInterface testCallback (&testLog, &cbacks);
		TestJournalInterface testJournal;
		Signature6Checker s6c (&testLog);

		FileInfo _fi (L"base003.avc", L"/");

		UpdaterConfiguration data(1, 1, L"1");
		data.connect_tmo = 600;
		data.passive_ftp = false;
		testCallback.SetUpdaterConfiguration (data);


		Updater upd (testCallback.m_dp, testCallback, testJournal, &testLog);
		HANDLE hDoDownload;
		doSelfNetDownloader do_dld(&upd, hDoDownload, _fi, _fi.m_relativeURLPath, L"h:\\test\\",
			wstring(L"ftp://").append(IP_V6_ADDR_1_WSTR).append(L":2121").c_str());


		string command;
		string param;
		FtpServer::CMD_TYPE type;

		fs.Accept();
		fs.Send ("220 Hello\x0d\x0a");

		fs.Receive(command, type, param);
		QC_BOOST_CHECK_MESSAGE (type == FtpServer::CMD_USER, L"Error\n");
		fs.Send ("331 Anonymous access allowed, send identity (e-mail name) as password.\x0d\x0a");
		fs.Receive(command, type, param);
		QC_BOOST_CHECK_MESSAGE (type == FtpServer::CMD_PASS, L"Error\n");
		fs.Send ("230 Logged in.\x0d\x0a");
		fs.Receive(command, type, param);
		QC_BOOST_CHECK_MESSAGE (type == FtpServer::CMD_TTYPE, L"Error\n");
		fs.Send ("200 Command okay.\x0d\x0a");
		fs.Receive(command, type, param);
		QC_BOOST_CHECK_MESSAGE (type == FtpServer::CMD_EPRT, L"Error\n");
		fs.Send ("200 EPRT command okay.\x0d\x0a");
		string addr = param;

		fs.Receive(command, type, param);
		QC_BOOST_CHECK_MESSAGE (type == FtpServer::CMD_RETR, L"Error\n");
		fs.Send ("150 File status okay; about to open data connection.\x0d\x0a");
		fs.EstablishActiveConnectionEx(addr);
		fs.SendData ("0123456789");
		fs.CloseDataConnection ();
		fs.Send ("226 Transfer complete.\x0d\x0a");

		WaitForSingleObject (hDoDownload, INFINITE);
		QC_BOOST_CHECK_MESSAGE ( CORE_NO_ERROR == do_dld.GetError(), L"Wrong return code\n" );
		QC_BOOST_CHECK_MESSAGE ( _helper::checkFileContent (wstring(TEST_FOLDER).append (L"\\base003.avc"),
			"0123456789"), L"Wrong return code\n" );
	}
}

void TestFtpTransport::TestExtendedModeIp6_Passive ()
{
	using namespace KLUPD;
	std::wcout<<L"TestExtendedModeIp6_Passive\n";
	Logger::Case _case (m_logger, m_testp, m_step, L"TestExtendedModeIp6_Passive");	

	{//getting file (passive mode)
		FtpServer fs (2121, IP_V6_ADDR_1, IP_V6);
		fs.Start();

		_helper::DeleteDirectory(TEST_FOLDER);
		_helper::MakeDirectory(TEST_FOLDER);

		TestLog testLog;
		Callbacks cbacks;
		TestCallbackInterface testCallback (&testLog, &cbacks);
		TestJournalInterface testJournal;
		Signature6Checker s6c (&testLog);

		FileInfo _fi (L"base003.avc", L"/");

		UpdaterConfiguration data(1, 1, L"1");
		data.connect_tmo = 600;
		data.passive_ftp = true;
		testCallback.SetUpdaterConfiguration (data);


		Updater upd (testCallback.m_dp, testCallback, testJournal, &testLog);
		HANDLE hDoDownload;
		doSelfNetDownloader do_dld(&upd, hDoDownload, _fi, _fi.m_relativeURLPath, L"h:\\test\\",
									wstring(L"ftp://").append(IP_V6_ADDR_1_WSTR).append(L":2121").c_str());

		string command;
		string param;
		FtpServer::CMD_TYPE type;

		fs.Accept();
		fs.Send ("220 Hello\x0d\x0a");

		fs.Receive(command, type, param);
		QC_BOOST_CHECK_MESSAGE (type == FtpServer::CMD_USER, L"Error\n");
		fs.Send ("331 Anonymous access allowed, send identity (e-mail name) as password.\x0d\x0a");
		fs.Receive(command, type, param);
		QC_BOOST_CHECK_MESSAGE (type == FtpServer::CMD_PASS, L"Error\n");
		fs.Send ("230 Logged in.\x0d\x0a");
		fs.Receive(command, type, param);
		QC_BOOST_CHECK_MESSAGE (type == FtpServer::CMD_TTYPE, L"Error\n");
		fs.Send ("200 Command okay.\x0d\x0a");
		fs.Receive(command, type, param);
		QC_BOOST_CHECK_MESSAGE (type == FtpServer::CMD_EPSV, L"Error\n");

		string addr;
		fs.CreateBindListeningPassiveDataPortEx (addr);
		fs.StartListeningPassiveDataPortOnly ();
		fs.Send (string("229 Entering Passive Mode (").append(addr).append(").\x0d\x0a"));
		fs.AcceptDataConnection ();

		fs.Receive(command, type, param);
		QC_BOOST_CHECK_MESSAGE (type == FtpServer::CMD_RETR, L"Error\n");
		fs.Send ("125 Data connection already open; Transfer starting.\x0d\x0a");
		fs.SendData ("0123456789");
		fs.CloseDataConnection ();
		fs.Send ("226 Transfer complete.\x0d\x0a");

		WaitForSingleObject (hDoDownload, INFINITE);
		QC_BOOST_CHECK_MESSAGE ( CORE_NO_ERROR == do_dld.GetError(), L"Wrong return code\n" );
		QC_BOOST_CHECK_MESSAGE ( _helper::checkFileContent (wstring(TEST_FOLDER).append (L"\\base003.avc"),
			"0123456789"), L"Wrong return code\n" );

	}
}


void TestFtpTransport::TestIpV6_Resolving ()
{//перед началом теста нужно настроить ДНС таким образом, чтобы он разрезолвил наш адрес на нужный ip6
	using namespace KLUPD;
	std::wcout<<L"TestIpV6_Resolving\n";
	Logger::Case _case (m_logger, m_testp, m_step, L"TestIpV6_Resolving");	

	{//getting file (passive mode)
		FtpServer fs (2121, IP_V6_ADDR_2, IP_V6);
		fs.Start();

		_helper::DeleteDirectory(TEST_FOLDER);
		_helper::MakeDirectory(TEST_FOLDER);

		TestLog testLog;
		Callbacks cbacks;
		TestCallbackInterface testCallback (&testLog, &cbacks);
		TestJournalInterface testJournal;
		Signature6Checker s6c (&testLog);

		FileInfo _fi (L"base003.avc", L"/");

		UpdaterConfiguration data(1, 1, L"1");
		data.connect_tmo = 600;
		data.passive_ftp = true;
		testCallback.SetUpdaterConfiguration (data);


		Updater upd (testCallback.m_dp, testCallback, testJournal, &testLog);
		HANDLE hDoDownload;
		doSelfNetDownloader do_dld(&upd, hDoDownload, _fi, _fi.m_relativeURLPath, L"h:\\test\\",
			wstring(L"ftp://").append(IP_V6_ADDR_2_WSTR).append(L":2121").c_str());

		string command;
		string param;
		FtpServer::CMD_TYPE type;

		fs.Accept();
		fs.Send ("220 Hello\x0d\x0a");

		fs.Receive(command, type, param);
		QC_BOOST_CHECK_MESSAGE (type == FtpServer::CMD_USER, L"Error\n");
		fs.Send ("331 Anonymous access allowed, send identity (e-mail name) as password.\x0d\x0a");
		fs.Receive(command, type, param);
		QC_BOOST_CHECK_MESSAGE (type == FtpServer::CMD_PASS, L"Error\n");
		fs.Send ("230 Logged in.\x0d\x0a");
		fs.Receive(command, type, param);
		QC_BOOST_CHECK_MESSAGE (type == FtpServer::CMD_TTYPE, L"Error\n");
		fs.Send ("200 Command okay.\x0d\x0a");
		fs.Receive(command, type, param);
		QC_BOOST_CHECK_MESSAGE (type == FtpServer::CMD_EPSV, L"Error\n");

		string addr;
		fs.CreateBindListeningPassiveDataPortEx (addr);
		fs.StartListeningPassiveDataPortOnly ();
		fs.Send (string("229 Entering Passive Mode (").append(addr).append(").\x0d\x0a"));
		fs.AcceptDataConnection ();

		fs.Receive(command, type, param);
		QC_BOOST_CHECK_MESSAGE (type == FtpServer::CMD_RETR, L"Error\n");
		fs.Send ("125 Data connection already open; Transfer starting.\x0d\x0a");
		fs.SendData ("0123456789");
		fs.CloseDataConnection ();
		fs.Send ("226 Transfer complete.\x0d\x0a");

		WaitForSingleObject (hDoDownload, INFINITE);
		QC_BOOST_CHECK_MESSAGE ( CORE_NO_ERROR == do_dld.GetError(), L"Wrong return code\n" );
		QC_BOOST_CHECK_MESSAGE ( _helper::checkFileContent (wstring(TEST_FOLDER).append (L"\\base003.avc"),
			"0123456789"), L"Wrong return code\n" );

	}
}