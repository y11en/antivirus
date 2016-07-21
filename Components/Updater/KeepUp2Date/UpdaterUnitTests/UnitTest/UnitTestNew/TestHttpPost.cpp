#include "util.h"

#include "log.h"
#include "callback.h"
#include "journal.h"

#include "core/updater.h"
#include "helper/local_file.h"

#include "..\..\..\Client\helper\fileInfo.h"
#include "..\..\..\Client\helper\sites_info.h"


#include "TestSuite.h"
#include "TestHttpPost.h"
#include "main.h"

#include "HttpServer.h"
#include "FtpServer.h"
#include "TestSuiteProxyDetector.h"

#include "constants.h"

void TestHttpPost::TestServers ()
{
	using namespace KLUPD;
	std::wcout<<L"TestServers\n";
	Logger::Case _case (m_logger, m_testp, m_step, L"TestServers");

	unsigned char buf[] = {"postdata"};
	std::vector<unsigned char> data; data.assign (buf, buf + 8);
	Address addr;
	addr.m_hostname = L"tl-srv-w03";
	addr./*m_portNumber*/m_service = L"80";
	addr.m_protocol = httpTransport;

	AuthorizationTypeList auth;

	TestLog testLog;
	Callbacks cbacks;
	TestCallbackInterface testCallback (&testLog, &cbacks);
	TestJournalInterface testJournal;
	CDownloadProgress dp (&testCallback);

	//KLUPD::DNSCache dsnCache;
	KLUPD::HttpProtocolMemoryImplementation httpProtocol
		(KLUPD::AuthorizationTypeList(),
		KLUPD::Credentials(), 30, /*dsnCache,*/ dp, testJournal, &testLog
		);

	dataPoster::dataPoster_par
		dpr_par (&httpProtocol, data, L"script.php", L"/", "Updater", addr, false);

	HANDLE hDoPost;
	dataPoster do_dpr (hDoPost, &dpr_par);

	WaitForSingleObject (hDoPost, INFINITE);
	QC_BOOST_CHECK_MESSAGE ( NO_ERROR == do_dpr.GetError(), L"Wrong return code\n" );
}



void TestHttpPost::TestPost ()
{
	using namespace KLUPD;
	std::wcout<<L"TestPost\n";
	Logger::Case _case (m_logger, m_testp, m_step, L"TestPost");

	unsigned char buf[] = {"postdata"};
	std::vector<unsigned char> data; data.assign (buf, buf + 8);
	Address addr;
	addr.m_hostname = L"127.0.0.1";
	addr./*m_portNumber*/m_service = L"8081";
	addr.m_protocol = httpTransport;

	AuthorizationTypeList auth;

	TestLog testLog;
	Callbacks cbacks;
	TestCallbackInterface testCallback (&testLog, &cbacks);
	TestJournalInterface testJournal;
	CDownloadProgress dp (&testCallback);

	//KLUPD::DNSCache dsnCache;
	KLUPD::HttpProtocolMemoryImplementation httpProtocol
					(KLUPD::AuthorizationTypeList(),
					 KLUPD::Credentials(), 30, /*dsnCache,*/ dp, testJournal, &testLog
					 );

	dataPoster::dataPoster_par
			dpr_par (&httpProtocol, data, L"filename.dat", L"relpath/", "Updater", addr, false);

	HttpServer hs (8081, "127.0.0.1");
	hs.Start();

	HANDLE hDoPost;
	dataPoster do_dpr (hDoPost, &dpr_par);

	hs.Accept();
	hs.ReceivePurePost (dpr_par);
	hs.Send200OK (HttpServer::Close, HttpServer::None);
	hs.CloseConnection();

	WaitForSingleObject (hDoPost, INFINITE);
	QC_BOOST_CHECK_MESSAGE ( NO_ERROR == do_dpr.GetError(), L"Wrong return code\n" );
}


void TestHttpPost::TestPostViaProxy ()
{
	using namespace KLUPD;
	std::wcout<<L"TestPostViaProxy\n";
	Logger::Case _case (m_logger, m_testp, m_step, L"TestPostViaProxy");

	{//basic
		HttpServer hs (3128, "127.0.0.1");
		hs.Start();

		unsigned char buf[] = {"postdata"};
		std::vector<unsigned char> data; data.assign (buf, buf + 8);
		Address addr;
		addr.m_hostname = L"55.55.55.55";
		addr.m_service = L"7777";
		addr.m_protocol = httpTransport;
		Address proxy;
		proxy.m_hostname = L"127.0.0.1";
		proxy.m_service = L"3128";


		AuthorizationTypeList auth;
		auth.push_back(basicAuthorization);

		TestLog testLog;
		Callbacks cbacks;
		TestCallbackInterface testCallback (&testLog, &cbacks);
		TestJournalInterface testJournal;
		CDownloadProgress dp (&testCallback);

		//KLUPD::DNSCache dsnCache;
		KLUPD::HttpProtocolMemoryImplementation httpProtocol
			(auth,
			KLUPD::Credentials(L"username",L"password"), 30,  dp, testJournal, &testLog
			);

		dataPoster::dataPoster_par
			dpr_par (&httpProtocol, data, L"filename.dat", L"/relpath/", "Updater", addr, true, proxy, auth);

		HANDLE hDoPost;
		dataPoster do_dpr (hDoPost, &dpr_par);

		hs.Accept();
		QC_BOOST_CHECK_MESSAGE (hs.ReceivePurePost (dpr_par), L"Error getting request\n");
		hs.SendBasicGreeting ();
		QC_BOOST_CHECK_MESSAGE (hs.ReceivePostWithBasicToken(dpr_par), L"Error getting request\n");
		hs.Send200OK (HttpServer::Close, HttpServer::Close);
		hs.CloseConnection();

		WaitForSingleObject (hDoPost, INFINITE);
		QC_BOOST_CHECK_MESSAGE ( NO_ERROR == do_dpr.GetError(), L"Wrong return code\n" );
	}


	{//ntlm
		HttpServer hs (3128, "127.0.0.1");
		hs.Start();

		unsigned char buf[] = {"postdata"};
		std::vector<unsigned char> data; data.assign (buf, buf + 8);
		Address addr;
		addr.m_hostname = L"55.55.55.55";
		addr./*m_portNumber*/m_service = L"7777";
		addr.m_protocol = httpTransport;
		Address proxy;
		proxy.m_hostname = L"127.0.0.1";
		proxy./*m_portNumber*/m_service = L"3128";


		AuthorizationTypeList auth;
		auth.push_back(ntlmAuthorization);

		TestLog testLog;
		Callbacks cbacks;
		TestCallbackInterface testCallback (&testLog, &cbacks);
		TestJournalInterface testJournal;
		CDownloadProgress dp (&testCallback);

		//KLUPD::DNSCache dsnCache;
		KLUPD::HttpProtocolMemoryImplementation httpProtocol
			(auth,
			KLUPD::Credentials(L"username",L"password"), 30, /*dsnCache,*/ dp, testJournal, &testLog
			);

		dataPoster::dataPoster_par
			dpr_par (&httpProtocol, data, L"filename.dat", L"/relpath/", "Updater", addr, true, proxy, auth);

		HANDLE hDoPost;
		dataPoster do_dpr (hDoPost, &dpr_par);

		hs.Accept();
		QC_BOOST_CHECK_MESSAGE (hs.ReceivePurePost (dpr_par), L"Error getting request\n");
		hs.SendNtlmGreeting ();
		QC_BOOST_CHECK_MESSAGE (hs.ReceivePostWithNtlmToken(dpr_par), L"Error getting request\n");
		hs.SendNtlmMsg2();
		QC_BOOST_CHECK_MESSAGE (hs.ReceivePostWithNtlmToken(dpr_par), L"Error getting request\n");

		hs.Send200OK (HttpServer::Close, HttpServer::Close);
		hs.CloseConnection();

		WaitForSingleObject (hDoPost, INFINITE);
		QC_BOOST_CHECK_MESSAGE ( NO_ERROR == do_dpr.GetError(), L"Wrong return code\n" );
	}

	{//no auth
		HttpServer hs (3128, "127.0.0.1");
		hs.Start();

		unsigned char buf[] = {"postdata"};
		std::vector<unsigned char> data; data.assign (buf, buf + 8);
		Address addr;
		addr.m_hostname = L"55.55.55.55";
		addr./*m_portNumber*/m_service = L"7777";
		addr.m_protocol = httpTransport;
		Address proxy;
		proxy.m_hostname = L"127.0.0.1";
		proxy./*m_portNumber*/m_service = L"3128";


		AuthorizationTypeList auth;
		auth.push_back(basicAuthorization);
		auth.push_back(ntlmAuthorization);
		auth.push_back(noAuthorization);

		TestLog testLog;
		Callbacks cbacks;
		TestCallbackInterface testCallback (&testLog, &cbacks);
		TestJournalInterface testJournal;
		CDownloadProgress dp (&testCallback);

		//KLUPD::DNSCache dsnCache;
		KLUPD::HttpProtocolMemoryImplementation httpProtocol
			(auth,
			KLUPD::Credentials(L"username",L"password"), 30, /*dsnCache,*/ dp, testJournal, &testLog
			);

		dataPoster::dataPoster_par
			dpr_par (&httpProtocol, data, L"filename.dat", L"/relpath/", "Updater", addr, true, proxy, auth);

		HANDLE hDoPost;
		dataPoster do_dpr (hDoPost, &dpr_par);

		hs.Accept();
		QC_BOOST_CHECK_MESSAGE (hs.ReceivePurePost (dpr_par), L"Error getting request\n");
		hs.Send200OK (HttpServer::Close, HttpServer::Close);
		hs.CloseConnection();

		WaitForSingleObject (hDoPost, INFINITE);
		QC_BOOST_CHECK_MESSAGE ( NO_ERROR == do_dpr.GetError(), L"Wrong return code\n" );
	}
}


void TestHttpPost::TestPostResponses ()
{
	using namespace KLUPD;
	std::wcout<<L"TestPostResponses\n";
	Logger::Case _case (m_logger, m_testp, m_step, L"TestPostResponses");

	{//200 with body

		unsigned char buf[] = {"postdata"};
		std::vector<unsigned char> data; data.assign (buf, buf + 8);
		Address addr;
		addr.m_hostname = L"127.0.0.1";
		addr./*m_portNumber*/m_service = L"8081";
		addr.m_protocol = httpTransport;

		AuthorizationTypeList auth;

		TestLog testLog;
		Callbacks cbacks;
		TestCallbackInterface testCallback (&testLog, &cbacks);
		TestJournalInterface testJournal;
		CDownloadProgress dp (&testCallback);

		KLUPD::HttpProtocolMemoryImplementation httpProtocol
			(KLUPD::AuthorizationTypeList(),
			KLUPD::Credentials(), 30, dp, testJournal, &testLog
			);

		dataPoster::dataPoster_par
			dpr_par (&httpProtocol, data, L"filename.dat", L"/", "Updater", addr, false);

		HttpServer hs (8081, "127.0.0.1");
		hs.Start();

		HANDLE hDoPost;
		dataPoster do_dpr (hDoPost, &dpr_par);

		hs.Accept();
		hs.ReceivePurePost (dpr_par);
		hs.Send200OK (HttpServer::Close, HttpServer::None, "Preved");
		hs.CloseConnection();

		WaitForSingleObject (hDoPost, INFINITE);
		QC_BOOST_CHECK_MESSAGE ( NO_ERROR == do_dpr.GetError(), L"Wrong return code\n" );
	}

	{//204

		unsigned char buf[] = {"postdata"};
		std::vector<unsigned char> data; data.assign (buf, buf + 8);
		Address addr;
		addr.m_hostname = L"127.0.0.1";
		addr./*m_portNumber*/m_service = L"8081";
		addr.m_protocol = httpTransport;

		AuthorizationTypeList auth;

		TestLog testLog;
		Callbacks cbacks;
		TestCallbackInterface testCallback (&testLog, &cbacks);
		TestJournalInterface testJournal;
		CDownloadProgress dp (&testCallback);

		KLUPD::HttpProtocolMemoryImplementation httpProtocol
			(KLUPD::AuthorizationTypeList(),
			KLUPD::Credentials(), 30, dp, testJournal, &testLog
			);

		dataPoster::dataPoster_par
			dpr_par (&httpProtocol, data, L"filename.dat", L"/", "Updater", addr, false);

		HttpServer hs (8081, "127.0.0.1");
		hs.Start();

		HANDLE hDoPost;
		dataPoster do_dpr (hDoPost, &dpr_par);

		hs.Accept();
		hs.ReceivePurePost (dpr_par);
		hs.Send204OK (HttpServer::Close, HttpServer::None);
		hs.CloseConnection();

		WaitForSingleObject (hDoPost, INFINITE);
		QC_BOOST_CHECK_MESSAGE ( NO_ERROR == do_dpr.GetError(), L"Wrong return code\n" );
	}

	{//403

		unsigned char buf[] = {"postdata"};
		std::vector<unsigned char> data; data.assign (buf, buf + 8);
		Address addr;
		addr.m_hostname = L"127.0.0.1";
		addr./*m_portNumber*/m_service = L"8081";
		addr.m_protocol = httpTransport;

		AuthorizationTypeList auth;

		TestLog testLog;
		Callbacks cbacks;
		TestCallbackInterface testCallback (&testLog, &cbacks);
		TestJournalInterface testJournal;
		CDownloadProgress dp (&testCallback);

//		KLUPD::DNSCache dsnCache;
		KLUPD::HttpProtocolMemoryImplementation httpProtocol
			(KLUPD::AuthorizationTypeList(),
			KLUPD::Credentials(), 30, /*dsnCache,*/ dp, testJournal, &testLog
			);

		dataPoster::dataPoster_par
			dpr_par (&httpProtocol, data, L"filename.dat", L"/", "Updater", addr, false);

		HttpServer hs (8081, "127.0.0.1");
		hs.Start();

		HANDLE hDoPost;
		dataPoster do_dpr (hDoPost, &dpr_par);

		hs.Accept();
		hs.ReceivePurePost (dpr_par);
		hs.Send403Forbidden (HttpServer::Close, HttpServer::None);
		hs.CloseConnection();

		WaitForSingleObject (hDoPost, INFINITE);
		QC_BOOST_CHECK_MESSAGE ( CORE_DOWNLOAD_ERROR == do_dpr.GetError(), L"Wrong return code\n" );
	}
}