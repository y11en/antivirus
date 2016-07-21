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

#define CANCEL_CREDENTIALS \
{\
WAIT_FOR_READY_EVENT(requestCredentialsForAuthorizationOnProxy);\
TestCallbackInterface::_credentials creds;\
creds.result = false;\
testCallback.SetRequestCreds(creds);\
SET_GOON_EVENT(requestCredentialsForAuthorizationOnProxy);\
}

void TestHttpProxy::TestSpecialProxyCases ()
{
	//старые баги по тестам - 24707, 28364

	using namespace KLUPD;
	std::wcout<<L"TestSpecialProxyCases\n";
	Logger::Case _case (m_logger, m_testp, m_step, L"TestSpecialProxyCases");
	
	//перезапрос срабатывает только на первый файл после получения 407-й ошибки
	//проверка переключений между типами авторизации и запросов новой авторизационной информации
	//когда-то написал этот камент, но что он означает уже не вспомню...
/*
	{//proxy (NTLM) returns 403 error code
	//one proxy (predefined) - 2 sources - invalid signature
	//credentials won't be requested
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
		cbacks.bFlags[Callbacks::requestCredentialsForAuthorizationOnProxy] = true;
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

			hs.Send403Forbidden (HttpServer::KeepAlive, HttpServer::Close);
			hs.CloseConnection();

			//xml
			hs.Accept();
			QC_BOOST_CHECK_MESSAGE (hs.ReceiveGetWithNtlmToken(), L"Error getting request\n");
			hs.SendNtlmMsg2();
			QC_BOOST_CHECK_MESSAGE (hs.ReceiveGetWithNtlmToken(), L"Error getting request\n");
			hs.Send403Forbidden (HttpServer::KeepAlive, HttpServer::Close);
			hs.CloseConnection();

		}

		//second source
		{	//klz
			hs.Accept();
			QC_BOOST_CHECK_MESSAGE (hs.ReceiveGetWithNtlmToken("bases2"), L"Error getting request\n");
			hs.SendNtlmMsg2();
			QC_BOOST_CHECK_MESSAGE (hs.ReceiveGetWithNtlmToken("bases2"), L"Error getting request\n");
			hs.Send403Forbidden (HttpServer::KeepAlive, HttpServer::Close);
			hs.CloseConnection();

			//xml
			hs.Accept();
			QC_BOOST_CHECK_MESSAGE (hs.ReceiveGetWithNtlmToken("bases2"), L"Error getting request\n");
			hs.SendNtlmMsg2();
			QC_BOOST_CHECK_MESSAGE (hs.ReceiveGetWithNtlmToken("bases2"), L"Error getting request\n");
			hs.Send403Forbidden (HttpServer::KeepAlive, HttpServer::Close);
			hs.CloseConnection();

		}

		WaitForSingleObject (hDoUpdate, INFINITE);
		QC_BOOST_CHECK_MESSAGE ( CORE_DOWNLOAD_ERROR == do_upd.GetError(), L"Wrong return code\n" );
	}
*/
 /*
	{//proxy (Basic) returns 403 error code

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
		cbacks.bFlags[Callbacks::requestCredentialsForAuthorizationOnProxy] = true;
		CREATE_EVENTS(requestCredentialsForAuthorizationOnProxy);
		TestCallbackInterface testCallback (&testLog, &cbacks);
		TestJournalInterface testJournal;
		testCallback.SetUpdaterConfiguration (data);

		Updater upd (testCallback.m_dp, testCallback, testJournal, &testLog);
		HANDLE hDoUpdate;

		doUpdater do_upd(&upd, hDoUpdate);

		//first source
		{	//klz
			hs.Accept();
			QC_BOOST_CHECK_MESSAGE (hs.ReceivePureGet("host: bases\r\n"), L"Error getting request\n");
			hs.SendBasicGreeting ();
			QC_BOOST_CHECK_MESSAGE (hs.ReceiveGetWithBasicToken("host: bases\r\n"), L"Error getting request\n");
			hs.Send403Forbidden (HttpServer::KeepAlive, HttpServer::Close);
			hs.CloseConnection();
			
			for (int i = 0; i <= 1; ++i)
			{//klz und xml
				
				for (int i = 1; i < 3; ++i)
				{
					//request credentials
					WAIT_FOR_READY_EVENT(requestCredentialsForAuthorizationOnProxy)
					TestCallbackInterface::_credentials creds;
					creds.name = L"test";
					creds.password = L"test";
					testCallback.SetRequestCreds(creds);
					SET_GOON_EVENT(requestCredentialsForAuthorizationOnProxy);

					hs.Accept();
					QC_BOOST_CHECK_MESSAGE (hs.ReceiveGetWithBasicToken("host: bases\r\n"), L"Error getting request\n");
					hs.Send403Forbidden (HttpServer::KeepAlive, HttpServer::Close);
					hs.CloseConnection();
				}

				CANCEL_CREDENTIALS
			}

		}

		//second source
		{	
			for (int i = 0; i <= 1; ++i)
			{//klz und xml

				for (int i = 1; i < 3; ++i)
				{
					//request credentials
					WAIT_FOR_READY_EVENT(requestCredentialsForAuthorizationOnProxy)
					TestCallbackInterface::_credentials creds;
					creds.name = L"test";
					creds.password = L"test";
					testCallback.SetRequestCreds(creds);
					SET_GOON_EVENT(requestCredentialsForAuthorizationOnProxy);

					hs.Accept();
					QC_BOOST_CHECK_MESSAGE (hs.ReceiveGetWithBasicToken("host: bases2\r\n"), L"Error getting request\n");
					hs.Send403Forbidden (HttpServer::KeepAlive, HttpServer::Close);
					hs.CloseConnection();
				}

				CANCEL_CREDENTIALS
			
			}

		}

		WaitForSingleObject (hDoUpdate, INFINITE);
		QC_BOOST_CHECK_MESSAGE ( CORE_DOWNLOAD_ERROR == do_upd.GetError(), L"Wrong return code\n" );
	}
*/
/*
	{	//proxy (NTLMwithCredentials) returns 502 error code

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
		data.m_httpAuthorizationMethods.push_back (ntlmAuthorizationWithCredentials);

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
		data.m_sourceList.addSource (UST_UserURL, L"http://bases2", true, L"diffs");

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

		//first source
		{	//klz
			hs.Accept();
			QC_BOOST_CHECK_MESSAGE (hs.ReceivePureGet(), L"Error getting request\n");
			hs.SendNtlmGreeting ();
			QC_BOOST_CHECK_MESSAGE (hs.ReceiveGetWithNtlmToken("host: bases\r\n"), L"Error getting request\n");
			hs.SendNtlmMsg2();
			QC_BOOST_CHECK_MESSAGE (hs.ReceiveGetWithNtlmToken("host: bases\r\n"), L"Error getting request\n");
			hs.Send502Denied (HttpServer::KeepAlive, HttpServer::Close);
			hs.CloseConnection();

			for (int i = 0; i <= 1; ++i)
			{//klz und xml

				for (int i = 1; i < 3; ++i)
				{
					//request credentials
					WAIT_FOR_READY_EVENT(requestCredentialsForAuthorizationOnProxy)
					TestCallbackInterface::_credentials creds;
					creds.name = L"test";
					creds.password = L"test";
					testCallback.SetRequestCreds(creds);
					SET_GOON_EVENT(requestCredentialsForAuthorizationOnProxy);

					hs.Accept();
					QC_BOOST_CHECK_MESSAGE (hs.ReceiveGetWithNtlmToken("host: bases\r\n"), L"Error getting request\n");
					hs.SendNtlmMsg2();
					QC_BOOST_CHECK_MESSAGE (hs.ReceiveGetWithNtlmToken("host: bases\r\n"), L"Error getting request\n");
					hs.Send502Denied (HttpServer::KeepAlive, HttpServer::Close);
					hs.CloseConnection();
				}

				CANCEL_CREDENTIALS
			}

		}

		//second source
		{	
			for (int i = 0; i <= 1; ++i)
			{//klz und xml

				for (int i = 1; i < 3; ++i)
				{
					//request credentials
					WAIT_FOR_READY_EVENT(requestCredentialsForAuthorizationOnProxy)
					TestCallbackInterface::_credentials creds;
					creds.name = L"test";
					creds.password = L"test";
					testCallback.SetRequestCreds(creds);
					SET_GOON_EVENT(requestCredentialsForAuthorizationOnProxy);

					hs.Accept();
					QC_BOOST_CHECK_MESSAGE (hs.ReceiveGetWithNtlmToken("host: bases2\r\n"), L"Error getting request\n");
					hs.SendNtlmMsg2();
					QC_BOOST_CHECK_MESSAGE (hs.ReceiveGetWithNtlmToken("host: bases2\r\n"), L"Error getting request\n");
					hs.Send502Denied (HttpServer::KeepAlive, HttpServer::Close);
					hs.CloseConnection();
				}

				CANCEL_CREDENTIALS
			}

		}

		WaitForSingleObject (hDoUpdate, INFINITE);
		QC_BOOST_CHECK_MESSAGE ( CORE_DOWNLOAD_ERROR == do_upd.GetError(), L"Wrong return code\n" );
	}
*/	
/*
	{	//proxy (NTLM, NTLMwithCredentials, Basic) returns 502 error code
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
		data.m_httpAuthorizationMethods.push_back (ntlmAuthorizationWithCredentials);
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
		data.m_componentsToUpdate.push_back (NoCaseString(L"CORE") );
		data.connect_tmo = 30;

		data.m_sourceList.addSource (UST_UserURL, L"http://bases", true, L"diffs");
		data.m_sourceList.addSource (UST_UserURL, L"http://bases2", true, L"diffs");

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

		//first source
		{	//klz

			hs.Accept();
			QC_BOOST_CHECK_MESSAGE (hs.ReceivePureGet("host: bases\r\n"), L"Error getting request\n");
			hs.SendNtlmBasicGreeting ();

			//ntlm
			QC_BOOST_CHECK_MESSAGE (hs.ReceiveGetWithNtlmToken("host: bases\r\n"), L"Error getting request\n");
			hs.SendNtlmMsg2();
			QC_BOOST_CHECK_MESSAGE (hs.ReceiveGetWithNtlmToken("host: bases\r\n"), L"Error getting request\n");
			hs.Send502Denied (HttpServer::KeepAlive, HttpServer::Close);
			hs.CloseConnection();
			//ntlm with creds
			hs.Accept();
			QC_BOOST_CHECK_MESSAGE (hs.ReceiveGetWithNtlmToken("host: bases\r\n"), L"Error getting request\n");
			hs.SendNtlmMsg2();
			QC_BOOST_CHECK_MESSAGE (hs.ReceiveGetWithNtlmToken("host: bases\r\n"), L"Error getting request\n");
			hs.Send502Denied (HttpServer::KeepAlive, HttpServer::Close);
			hs.CloseConnection();


			for (int i = 0; i <= 1; ++i)
			{//klz und xml
				if (i == 1)
				{
					//ntlm
					hs.Accept();
					QC_BOOST_CHECK_MESSAGE (hs.ReceiveGetWithNtlmToken("host: bases\r\n"), L"Error getting request\n");
					hs.SendNtlmMsg2();
					QC_BOOST_CHECK_MESSAGE (hs.ReceiveGetWithNtlmToken("host: bases\r\n"), L"Error getting request\n");
					hs.Send502Denied (HttpServer::KeepAlive, HttpServer::Close);
					hs.CloseConnection();
				}

				//for (int i = 1; i < 3; ++i)
				//{
					//request credentials
					WAIT_FOR_READY_EVENT(requestCredentialsForAuthorizationOnProxy)
					TestCallbackInterface::_credentials creds;
					creds.name = L"test";
					creds.password = L"test";
					testCallback.SetRequestCreds(creds);
					SET_GOON_EVENT(requestCredentialsForAuthorizationOnProxy);

					hs.Accept();
					QC_BOOST_CHECK_MESSAGE (hs.ReceiveGetWithNtlmToken("host: bases\r\n"), L"Error getting request\n");
					hs.SendNtlmMsg2();
					QC_BOOST_CHECK_MESSAGE (hs.ReceiveGetWithNtlmToken("host: bases\r\n"), L"Error getting request\n");
					hs.Send502Denied (HttpServer::KeepAlive, HttpServer::Close);
					hs.CloseConnection();
				//}

				CANCEL_CREDENTIALS
			}
		}

		//second source
		{
			for (int i = 0; i <= 1; ++i)
			{//klz und xml

				//ntlm
				hs.Accept();
				QC_BOOST_CHECK_MESSAGE (hs.ReceiveGetWithNtlmToken("host: bases2\r\n"), L"Error getting request\n");
				hs.SendNtlmMsg2();
				QC_BOOST_CHECK_MESSAGE (hs.ReceiveGetWithNtlmToken("host: bases2\r\n"), L"Error getting request\n");
				hs.Send502Denied (HttpServer::KeepAlive, HttpServer::Close);
				hs.CloseConnection();

				for (int i = 1; i < 3; ++i)
				{
					//request credentials
					WAIT_FOR_READY_EVENT(requestCredentialsForAuthorizationOnProxy)
					TestCallbackInterface::_credentials creds;
					creds.name = L"test";
					creds.password = L"test";
					testCallback.SetRequestCreds(creds);
					SET_GOON_EVENT(requestCredentialsForAuthorizationOnProxy);

					hs.Accept();
					QC_BOOST_CHECK_MESSAGE (hs.ReceiveGetWithNtlmToken("host: bases2\r\n"), L"Error getting request\n");
					hs.SendNtlmMsg2();
					QC_BOOST_CHECK_MESSAGE (hs.ReceiveGetWithNtlmToken("host: bases2\r\n"), L"Error getting request\n");
					hs.Send502Denied (HttpServer::KeepAlive, HttpServer::Close);
					hs.CloseConnection();
				}

				CANCEL_CREDENTIALS
			}

		}

		WaitForSingleObject (hDoUpdate, INFINITE);
		QC_BOOST_CHECK_MESSAGE ( CORE_DOWNLOAD_ERROR == do_upd.GetError(), L"Wrong return code\n" );
		//QC_BOOST_CHECK_MESSAGE ( CORE_PROXY_AUTH_ERROR == do_upd.GetError(), L"Wrong return code\n" );
	}
	*/
	
}


void TestHttpProxy::TestSpecialProxyCases_ConstCancel ()
{
//тут тесты, ориентированные на возможное использование в приложениях,
//т.е. когда Cancel жмется один раз, он будет применяться всегда
	using namespace KLUPD;
	std::wcout<<L"TestSpecialProxyCases_ConstCancel\n";
	Logger::Case _case (m_logger, m_testp, m_step, L"TestSpecialProxyCases_ConstCancel");
	

	{	//BASIC
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
		cbacks.bFlags[Callbacks::requestCredentialsForAuthorizationOnProxy] = true;
		CREATE_EVENTS(requestCredentialsForAuthorizationOnProxy);
		TestCallbackInterface testCallback (&testLog, &cbacks);
		TestJournalInterface testJournal;
		testCallback.SetUpdaterConfiguration (data);

		Updater upd (testCallback.m_dp, testCallback, testJournal, &testLog);
		HANDLE hDoUpdate;

		doUpdater do_upd(&upd, hDoUpdate);

		//first source
		{	//klz
			hs.Accept();
			QC_BOOST_CHECK_MESSAGE (hs.ReceivePureGet("host: bases\r\n"), L"Error getting request\n");
			hs.SendBasicGreeting ();
			QC_BOOST_CHECK_MESSAGE (hs.ReceiveGetWithBasicToken("host: bases\r\n"), L"Error getting request\n");
			hs.Send403Forbidden (HttpServer::KeepAlive, HttpServer::Close);
			hs.CloseConnection();
			CANCEL_CREDENTIALS

			//xml
			CANCEL_CREDENTIALS
		}

		//second source
		{	//klz
			CANCEL_CREDENTIALS
			
			//xml
			CANCEL_CREDENTIALS

		}

		WaitForSingleObject (hDoUpdate, INFINITE);
		QC_BOOST_CHECK_MESSAGE ( CORE_PROXY_AUTH_ERROR == do_upd.GetError(), L"Wrong return code\n" );
	}


	{	//NTLMwithCREDS
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
		data.m_httpAuthorizationMethods.push_back (ntlmAuthorizationWithCredentials);

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
		data.m_sourceList.addSource (UST_UserURL, L"http://bases2", true, L"diffs");

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

		//first source
		{	//klz
			hs.Accept();
			QC_BOOST_CHECK_MESSAGE (hs.ReceivePureGet(), L"Error getting request\n");
			hs.SendNtlmGreeting ();
			QC_BOOST_CHECK_MESSAGE (hs.ReceiveGetWithNtlmToken("host: bases\r\n"), L"Error getting request\n");
			hs.SendNtlmMsg2();
			QC_BOOST_CHECK_MESSAGE (hs.ReceiveGetWithNtlmToken("host: bases\r\n"), L"Error getting request\n");
			hs.Send502Denied (HttpServer::KeepAlive, HttpServer::Close);
			hs.CloseConnection();
			
			CANCEL_CREDENTIALS

			//xml
			CANCEL_CREDENTIALS

		}

		//second source
		{	
			//klz	
			CANCEL_CREDENTIALS

			//xml
			CANCEL_CREDENTIALS

		}

		WaitForSingleObject (hDoUpdate, INFINITE);
		QC_BOOST_CHECK_MESSAGE ( CORE_PROXY_AUTH_ERROR == do_upd.GetError(), L"Wrong return code\n" );
	}


	{	//NTLM + NTLMwithCREDS + BASIC

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
		data.m_httpAuthorizationMethods.push_back (ntlmAuthorizationWithCredentials);
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
		data.m_componentsToUpdate.push_back (NoCaseString(L"CORE") );
		data.connect_tmo = 30;

		data.m_sourceList.addSource (UST_UserURL, L"http://bases", true, L"diffs");
		data.m_sourceList.addSource (UST_UserURL, L"http://bases2", true, L"diffs");

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

		//first source
		{	//klz

			hs.Accept();
			QC_BOOST_CHECK_MESSAGE (hs.ReceivePureGet("host: bases\r\n"), L"Error getting request\n");
			hs.SendNtlmBasicGreeting ();

			//ntlm
			QC_BOOST_CHECK_MESSAGE (hs.ReceiveGetWithNtlmToken("host: bases\r\n"), L"Error getting request\n");
			hs.SendNtlmMsg2();
			QC_BOOST_CHECK_MESSAGE (hs.ReceiveGetWithNtlmToken("host: bases\r\n"), L"Error getting request\n");
			hs.Send502Denied (HttpServer::KeepAlive, HttpServer::Close);
			hs.CloseConnection();
			//ntlm with creds
			hs.Accept();
			QC_BOOST_CHECK_MESSAGE (hs.ReceiveGetWithNtlmToken("host: bases\r\n"), L"Error getting request\n");
			hs.SendNtlmMsg2();
			QC_BOOST_CHECK_MESSAGE (hs.ReceiveGetWithNtlmToken("host: bases\r\n"), L"Error getting request\n");
			hs.Send502Denied (HttpServer::KeepAlive, HttpServer::Close);
			hs.CloseConnection();
			//basic - ?
//			hs.Accept();
//			QC_BOOST_CHECK_MESSAGE (hs.ReceiveGetWithBasicToken("host: bases\r\n"), L"Error getting request\n");
//			hs.Send502Denied (HttpServer::KeepAlive, HttpServer::Close);
//			hs.CloseConnection();

			CANCEL_CREDENTIALS

			//xml
			//ntlm
			hs.Accept();
			QC_BOOST_CHECK_MESSAGE (hs.ReceiveGetWithNtlmToken("host: bases\r\n"), L"Error getting request\n");
			hs.SendNtlmMsg2();
			QC_BOOST_CHECK_MESSAGE (hs.ReceiveGetWithNtlmToken("host: bases\r\n"), L"Error getting request\n");
			hs.Send502Denied (HttpServer::KeepAlive, HttpServer::Close);
			hs.CloseConnection();

			CANCEL_CREDENTIALS
		}

		//second source
		{	//klz
			//ntlm
			hs.Accept();
			QC_BOOST_CHECK_MESSAGE (hs.ReceiveGetWithNtlmToken("host: bases2\r\n"), L"Error getting request\n");
			hs.SendNtlmMsg2();
			QC_BOOST_CHECK_MESSAGE (hs.ReceiveGetWithNtlmToken("host: bases2\r\n"), L"Error getting request\n");
			hs.Send502Denied (HttpServer::KeepAlive, HttpServer::Close);
			hs.CloseConnection();
			
			CANCEL_CREDENTIALS

			//xml
			//ntlm
			hs.Accept();
			QC_BOOST_CHECK_MESSAGE (hs.ReceiveGetWithNtlmToken("host: bases2\r\n"), L"Error getting request\n");
			hs.SendNtlmMsg2();
			QC_BOOST_CHECK_MESSAGE (hs.ReceiveGetWithNtlmToken("host: bases2\r\n"), L"Error getting request\n");
			hs.Send502Denied (HttpServer::KeepAlive, HttpServer::Close);
			hs.CloseConnection();
			
			CANCEL_CREDENTIALS
		}

		WaitForSingleObject (hDoUpdate, INFINITE);
		QC_BOOST_CHECK_MESSAGE ( CORE_DOWNLOAD_ERROR == do_upd.GetError(), L"Wrong return code\n" );
	}

	{	//NTLM + BASIC

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
		data.m_componentsToUpdate.push_back (NoCaseString(L"CORE") );
		data.connect_tmo = 30;

		data.m_sourceList.addSource (UST_UserURL, L"http://bases", true, L"diffs");
		data.m_sourceList.addSource (UST_UserURL, L"http://bases2", true, L"diffs");

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

		//first source
		{	//klz

			hs.Accept();
			QC_BOOST_CHECK_MESSAGE (hs.ReceivePureGet("host: bases\r\n"), L"Error getting request\n");
			hs.SendNtlmBasicGreeting ();

			//ntlm
			QC_BOOST_CHECK_MESSAGE (hs.ReceiveGetWithNtlmToken("host: bases\r\n"), L"Error getting request\n");
			hs.SendNtlmMsg2();
			QC_BOOST_CHECK_MESSAGE (hs.ReceiveGetWithNtlmToken("host: bases\r\n"), L"Error getting request\n");
			hs.Send502Denied (HttpServer::KeepAlive, HttpServer::Close);
			hs.CloseConnection();
			//basic
			hs.Accept();
			QC_BOOST_CHECK_MESSAGE (hs.ReceiveGetWithBasicToken("host: bases\r\n"), L"Error getting request\n");
			hs.Send502Denied (HttpServer::KeepAlive, HttpServer::Close);
			hs.CloseConnection();

			CANCEL_CREDENTIALS

			//xml
			//ntlm
			hs.Accept();
			QC_BOOST_CHECK_MESSAGE (hs.ReceiveGetWithNtlmToken("host: bases\r\n"), L"Error getting request\n");
			hs.SendNtlmMsg2();
			QC_BOOST_CHECK_MESSAGE (hs.ReceiveGetWithNtlmToken("host: bases\r\n"), L"Error getting request\n");
			hs.Send502Denied (HttpServer::KeepAlive, HttpServer::Close);
			hs.CloseConnection();

			CANCEL_CREDENTIALS
		}

		//second source
		{	//klz
			//ntlm
			hs.Accept();
			QC_BOOST_CHECK_MESSAGE (hs.ReceiveGetWithNtlmToken("host: bases2\r\n"), L"Error getting request\n");
			hs.SendNtlmMsg2();
			QC_BOOST_CHECK_MESSAGE (hs.ReceiveGetWithNtlmToken("host: bases2\r\n"), L"Error getting request\n");
			hs.Send502Denied (HttpServer::KeepAlive, HttpServer::Close);
			hs.CloseConnection();

			CANCEL_CREDENTIALS

			//xml
			//ntlm
			hs.Accept();
			QC_BOOST_CHECK_MESSAGE (hs.ReceiveGetWithNtlmToken("host: bases2\r\n"), L"Error getting request\n");
			hs.SendNtlmMsg2();
			QC_BOOST_CHECK_MESSAGE (hs.ReceiveGetWithNtlmToken("host: bases2\r\n"), L"Error getting request\n");
			hs.Send502Denied (HttpServer::KeepAlive, HttpServer::Close);
			hs.CloseConnection();

			CANCEL_CREDENTIALS
		}

		WaitForSingleObject (hDoUpdate, INFINITE);
		QC_BOOST_CHECK_MESSAGE ( CORE_DOWNLOAD_ERROR == do_upd.GetError(), L"Wrong return code\n" );
	}

	{	//NTLMwithCREDS + BASIC

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
		data.m_httpAuthorizationMethods.push_back (ntlmAuthorizationWithCredentials);

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
		data.m_sourceList.addSource (UST_UserURL, L"http://bases2", true, L"diffs");

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

		//first source
		{	//klz

			hs.Accept();
			QC_BOOST_CHECK_MESSAGE (hs.ReceivePureGet("host: bases\r\n"), L"Error getting request\n");
			hs.SendNtlmBasicGreeting ();

			//ntlm with creds
			QC_BOOST_CHECK_MESSAGE (hs.ReceiveGetWithNtlmToken("host: bases\r\n"), L"Error getting request\n");
			hs.SendNtlmMsg2();
			QC_BOOST_CHECK_MESSAGE (hs.ReceiveGetWithNtlmToken("host: bases\r\n"), L"Error getting request\n");
			hs.Send502Denied (HttpServer::KeepAlive, HttpServer::Close);
			hs.CloseConnection();
			//basic - ?
			//hs.Accept();
			//QC_BOOST_CHECK_MESSAGE (hs.ReceiveGetWithBasicToken("host: bases\r\n"), L"Error getting request\n");
			//hs.Send502Denied (HttpServer::KeepAlive, HttpServer::Close);
			//hs.CloseConnection();
			CANCEL_CREDENTIALS

			//xml
			CANCEL_CREDENTIALS
		}

		//second source
		{	
			//klz
			CANCEL_CREDENTIALS
			//xml
			CANCEL_CREDENTIALS
		}

		WaitForSingleObject (hDoUpdate, INFINITE);
		QC_BOOST_CHECK_MESSAGE ( CORE_PROXY_AUTH_ERROR == do_upd.GetError(), L"Wrong return code\n" );
	}

	{	//No authorization

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
		data.m_httpAuthorizationMethods.push_back (ntlmAuthorizationWithCredentials);
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
		data.m_componentsToUpdate.push_back (NoCaseString(L"CORE") );
		data.connect_tmo = 30;

		data.m_sourceList.addSource (UST_UserURL, L"http://bases", true, L"diffs");
		data.m_sourceList.addSource (UST_UserURL, L"http://bases2", true, L"diffs");

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

		//first source
		{	
			//klz
			hs.Accept();
			QC_BOOST_CHECK_MESSAGE (hs.ReceivePureGet("host: bases\r\n"), L"Error getting request\n");
			hs.Send502Denied (HttpServer::KeepAlive, HttpServer::Close);
			hs.CloseConnection();
			
			//xml
			hs.Accept();
			QC_BOOST_CHECK_MESSAGE (hs.ReceivePureGet("host: bases\r\n"), L"Error getting request\n");
			hs.Send502Denied (HttpServer::KeepAlive, HttpServer::Close);
			hs.CloseConnection();
		}

		//second source
		{	
			//klz
			hs.Accept();
			QC_BOOST_CHECK_MESSAGE (hs.ReceivePureGet("host: bases2\r\n"), L"Error getting request\n");
			hs.Send502Denied (HttpServer::KeepAlive, HttpServer::Close);
			hs.CloseConnection();

			//xml
			hs.Accept();
			QC_BOOST_CHECK_MESSAGE (hs.ReceivePureGet("host: bases2\r\n"), L"Error getting request\n");
			hs.Send502Denied (HttpServer::KeepAlive, HttpServer::Close);
			hs.CloseConnection();
		}

		WaitForSingleObject (hDoUpdate, INFINITE);
		QC_BOOST_CHECK_MESSAGE ( CORE_DOWNLOAD_ERROR == do_upd.GetError(), L"Wrong return code\n" );
	}

}


void TestHttpProxy::TestSpecialProxyCases_CalculatedDiffs ()
{
	using namespace KLUPD;
	std::wcout<<L"TestSpecialProxyCases_CalculatedDiffs\n";
	Logger::Case _case (m_logger, m_testp, m_step, L"TestSpecialProxyCases_CalculatedDiffs");

	//не должно происходить переключения авторизации на файлах с изменяемыми расширениями

	{//proxy (NTLM) returns 403 error code	
		//proxy (Basic) returns 403 error code
		//one proxy (predefined) - 2 sources - invalid signature
		//bug 24686, check that credentials are not asked for diffs with calculated extensions
		HttpServer hs (3128, "127.0.0.1");
		hs.Start();

		TestLog testLog;

		_helper::DeleteDirectory(ROOT);
		KLUPD::createFolder (BASES_ROOT, &testLog);
		KLUPD::createFolder (UPD_ROOT, &testLog);

		_helper::CopyFolder(std::wstring(COLL_ROOT).append(L"\\Bases_NULL_FOR_DNLD\\plain").c_str(),BASES_ROOT);
		_helper::CloneFile (std::wstring(COLL_ROOT).append(L"\\Bases_NULL_FOR_DNLD\\index\\6\\u0607g.xml").c_str(),
			std::wstring (UPD_ROOT).append (L"\\u0607g.xml"), false);

		UpdaterConfiguration data(1, 1, L"1");
		data.proxy_url = L"127.0.0.1";
		data.proxy_port = 3128;
		data.m_proxyAuthorizationCredentials.userName ( L"fakeuser");
		data.m_proxyAuthorizationCredentials.password ( L"fakepwd");
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
		//diff
		hs.Accept();
		QC_BOOST_CHECK_MESSAGE (hs.ReceivePureGet(), L"Error getting request\n");
		hs.SendBasicGreeting ();
		QC_BOOST_CHECK_MESSAGE (hs.ReceiveGetWithBasicToken("u0607g.xml.dif"), L"Error getting request\n");
		hs.Send403Forbidden (HttpServer::KeepAlive, HttpServer::Close);
		hs.CloseConnection();
		//asking credentials
		WAIT_FOR_READY_EVENT(requestCredentialsForAuthorizationOnProxy)
		SET_GOON_EVENT(requestCredentialsForAuthorizationOnProxy);
		hs.Accept();
		QC_BOOST_CHECK_MESSAGE (hs.ReceiveGetWithBasicToken("u0607g.xml.dif"), L"Error getting request\n");
		hs.SendFileFromFile (HttpServer::KeepAlive, HttpServer::Close, std::wstring(COLL_ROOT).append(L"\\Bases_1\\data\\u0607g.xml"));
		hs.CloseConnection();

		//klz
		hs.Accept();
		QC_BOOST_CHECK_MESSAGE (hs.ReceiveGetWithBasicToken("u0607g.xml.klz"), L"Error getting request\n");
		hs.Send403Forbidden (HttpServer::KeepAlive, HttpServer::Close);
		hs.CloseConnection();
		//asking credentials
		WAIT_FOR_READY_EVENT(requestCredentialsForAuthorizationOnProxy)
		SET_GOON_EVENT(requestCredentialsForAuthorizationOnProxy);
		hs.Accept();
		QC_BOOST_CHECK_MESSAGE (hs.ReceiveGetWithBasicToken("u0607g.xml.klz"), L"Error getting request\n");
		hs.SendFileFromFile (HttpServer::KeepAlive, HttpServer::Close, std::wstring(COLL_ROOT).append(L"\\Bases_1\\data\\u0607g.xml"));
		hs.CloseConnection();

		//xml
		hs.Accept();
		QC_BOOST_CHECK_MESSAGE (hs.ReceiveGetWithBasicToken("u0607g.xml"), L"Error getting request\n");
		hs.Send403Forbidden (HttpServer::KeepAlive, HttpServer::Close);
		hs.CloseConnection();
		//asking credentials
		WAIT_FOR_READY_EVENT(requestCredentialsForAuthorizationOnProxy)
		SET_GOON_EVENT(requestCredentialsForAuthorizationOnProxy);
		hs.Accept();
		QC_BOOST_CHECK_MESSAGE (hs.ReceiveGetWithBasicToken("u0607g.xml"), L"Error getting request\n");
		hs.SendFileFromFile (HttpServer::KeepAlive, HttpServer::Close, std::wstring(COLL_ROOT).append(L"\\Bases_1\\data\\u0607g.xml"));
		hs.CloseConnection();

		//av-i386
		//diff
		hs.Accept();
		QC_BOOST_CHECK_MESSAGE (hs.ReceiveGetWithBasicToken("av-i386-0607g.xml.dif"), L"Error getting request\n");
		hs.Send403Forbidden (HttpServer::KeepAlive, HttpServer::Close);
		hs.CloseConnection();
		//asking credentials
		WAIT_FOR_READY_EVENT(requestCredentialsForAuthorizationOnProxy)
		SET_GOON_EVENT(requestCredentialsForAuthorizationOnProxy);
		hs.Accept();
		QC_BOOST_CHECK_MESSAGE (hs.ReceiveGetWithBasicToken("av-i386-0607g.xml.dif"), L"Error getting request\n");
		hs.SendFileFromFile (HttpServer::KeepAlive, HttpServer::Close, std::wstring(COLL_ROOT).append(L"\\Bases_1\\plain\\av-i386-0607g.xml"));
		hs.CloseConnection();

		//klz
		hs.Accept();
		QC_BOOST_CHECK_MESSAGE (hs.ReceiveGetWithBasicToken("av-i386-0607g.xml.klz"), L"Error getting request\n");
		hs.Send403Forbidden (HttpServer::KeepAlive, HttpServer::Close);
		hs.CloseConnection();
		//asking credentials
		WAIT_FOR_READY_EVENT(requestCredentialsForAuthorizationOnProxy)
		SET_GOON_EVENT(requestCredentialsForAuthorizationOnProxy);
		hs.Accept();
		QC_BOOST_CHECK_MESSAGE (hs.ReceiveGetWithBasicToken("av-i386-0607g.xml.klz"), L"Error getting request\n");
		hs.SendFileFromFile (HttpServer::KeepAlive, HttpServer::Close, std::wstring(COLL_ROOT).append(L"\\Bases_1\\plain\\av-i386-0607g.xml"));
		hs.CloseConnection();

		//xml
		hs.Accept();
		QC_BOOST_CHECK_MESSAGE (hs.ReceiveGetWithBasicToken("av-i386-0607g.xml"), L"Error getting request\n");
		hs.Send403Forbidden (HttpServer::KeepAlive, HttpServer::Close);
		hs.CloseConnection();
		//asking credentials
		WAIT_FOR_READY_EVENT(requestCredentialsForAuthorizationOnProxy)
		SET_GOON_EVENT(requestCredentialsForAuthorizationOnProxy);
		hs.Accept();
		QC_BOOST_CHECK_MESSAGE (hs.ReceiveGetWithBasicToken("av-i386-0607g.xml"), L"Error getting request\n");
		hs.SendFileFromFile (HttpServer::KeepAlive, HttpServer::Close, std::wstring(COLL_ROOT).append(L"\\Bases_1\\plain\\av-i386-0607g.xml"));
		hs.CloseConnection();

		//blst
		//diff
		hs.Accept();
		QC_BOOST_CHECK_MESSAGE (hs.ReceiveGetWithBasicToken("blst-0607g.xml.dif"), L"Error getting request\n");
		hs.Send403Forbidden (HttpServer::KeepAlive, HttpServer::Close);
		hs.CloseConnection();
		//asking credentials
		WAIT_FOR_READY_EVENT(requestCredentialsForAuthorizationOnProxy)
		SET_GOON_EVENT(requestCredentialsForAuthorizationOnProxy);
		hs.Accept();
		QC_BOOST_CHECK_MESSAGE (hs.ReceiveGetWithBasicToken("blst-0607g.xml.dif"), L"Error getting request\n");
		hs.SendFileFromFile (HttpServer::KeepAlive, HttpServer::Close, std::wstring(COLL_ROOT).append(L"\\Bases_1\\plain\\blst-0607g.xml"));
		hs.CloseConnection();

		//klz
		hs.Accept();
		QC_BOOST_CHECK_MESSAGE (hs.ReceiveGetWithBasicToken("blst-0607g.xml.klz"), L"Error getting request\n");
		hs.Send403Forbidden (HttpServer::KeepAlive, HttpServer::Close);
		hs.CloseConnection();
		//asking credentials
		WAIT_FOR_READY_EVENT(requestCredentialsForAuthorizationOnProxy)
		SET_GOON_EVENT(requestCredentialsForAuthorizationOnProxy);
		hs.Accept();
		QC_BOOST_CHECK_MESSAGE (hs.ReceiveGetWithBasicToken("blst-0607g.xml.klz"), L"Error getting request\n");
		hs.SendFileFromFile (HttpServer::KeepAlive, HttpServer::Close, std::wstring(COLL_ROOT).append(L"\\Bases_1\\plain\\blst-0607g.xml"));
		hs.CloseConnection();

		//xml
		hs.Accept();
		QC_BOOST_CHECK_MESSAGE (hs.ReceiveGetWithBasicToken("blst-0607g.xml"), L"Error getting request\n");
		hs.Send403Forbidden (HttpServer::KeepAlive, HttpServer::Close);
		hs.CloseConnection();
		//asking credentials
		WAIT_FOR_READY_EVENT(requestCredentialsForAuthorizationOnProxy)
		SET_GOON_EVENT(requestCredentialsForAuthorizationOnProxy);
		hs.Accept();
		QC_BOOST_CHECK_MESSAGE (hs.ReceiveGetWithBasicToken("blst-0607g.xml"), L"Error getting request\n");
		hs.SendFileFromFile (HttpServer::KeepAlive, HttpServer::Close, std::wstring(COLL_ROOT).append(L"\\Bases_1\\plain\\blst-0607g.xml"));
		hs.CloseConnection();

		//blst-file
		//diff
		hs.Accept();
		QC_BOOST_CHECK_MESSAGE (hs.ReceiveGetWithBasicToken("black.lst."), L"Error getting request\n");
		hs.Send403Forbidden (HttpServer::KeepAlive, HttpServer::Close);
		hs.CloseConnection();
		//no asking credentials

		//blst
		hs.Accept();
		QC_BOOST_CHECK_MESSAGE (hs.ReceivePureGet(), L"Error getting request\n");
		hs.SendBasicGreeting ();
		QC_BOOST_CHECK_MESSAGE (hs.ReceiveGetWithBasicToken("black.lst"), L"Error getting request\n");
		hs.Send403Forbidden (HttpServer::KeepAlive, HttpServer::Close);
		hs.CloseConnection();
		//asking credentials
		WAIT_FOR_READY_EVENT(requestCredentialsForAuthorizationOnProxy)
		SET_GOON_EVENT(requestCredentialsForAuthorizationOnProxy);
		//blst again
		hs.Accept();
		QC_BOOST_CHECK_MESSAGE (hs.ReceiveGetWithBasicToken("black.lst"), L"Error getting request\n");
		hs.SendFileFromFile (HttpServer::KeepAlive, HttpServer::Close, std::wstring(COLL_ROOT).append(L"\\Bases_1\\plain\\black.lst"));
		hs.CloseConnection();

		//base-file
		//diff
		hs.Accept();
		QC_BOOST_CHECK_MESSAGE (hs.ReceiveGetWithBasicToken("base102.avc."), L"Error getting request\n");
		hs.Send403Forbidden (HttpServer::KeepAlive, HttpServer::Close);
		hs.CloseConnection();
		//no asking credentials

		//base file
		hs.Accept();
		QC_BOOST_CHECK_MESSAGE (hs.ReceivePureGet(), L"Error getting request\n");
		hs.SendBasicGreeting ();
		QC_BOOST_CHECK_MESSAGE (hs.ReceiveGetWithBasicToken("base102.avc"), L"Error getting request\n");
		hs.Send403Forbidden (HttpServer::KeepAlive, HttpServer::Close);
		hs.CloseConnection();
		//asking credentials
		WAIT_FOR_READY_EVENT(requestCredentialsForAuthorizationOnProxy)
		SET_GOON_EVENT(requestCredentialsForAuthorizationOnProxy);
		//base file again
		hs.Accept();
		QC_BOOST_CHECK_MESSAGE (hs.ReceiveGetWithBasicToken("base102.avc"), L"Error getting request\n");
		hs.SendFileFromFile (HttpServer::KeepAlive, HttpServer::Close, std::wstring(COLL_ROOT).append(L"\\Bases_1\\plain\\base102.avc"));
		hs.CloseConnection();
		//base file 2
		hs.Accept();
		QC_BOOST_CHECK_MESSAGE (hs.ReceiveGetWithBasicToken("base103.avc"), L"Error getting request\n");
		hs.SendFileFromFile (HttpServer::KeepAlive, HttpServer::Close, std::wstring(COLL_ROOT).append(L"\\Bases_1\\plain\\base103.avc"));
		hs.CloseConnection();

		WaitForSingleObject (hDoUpdate, INFINITE);
		QC_BOOST_CHECK_MESSAGE ( CORE_NO_ERROR == do_upd.GetError(), L"Wrong return code\n" );
	}

/*	{//proxy (NTLM) returns 403 error code	
		//proxy (Basic) returns 403 error code
		//one proxy (predefined) - 2 sources - invalid signature
		//bug 24686, check that credentials are not asked for diffs with calculated extensions
		HttpServer hs (3128, "127.0.0.1");
		hs.Start();

		TestLog testLog;

		_helper::DeleteDirectory(ROOT);
		KLUPD::createFolder (BASES_ROOT, &testLog);
		KLUPD::createFolder (UPD_ROOT, &testLog);

		_helper::CopyFolder(std::wstring(COLL_ROOT).append(L"\\Bases_NULL_FOR_DNLD\\plain").c_str(),BASES_ROOT);
		_helper::CloneFile (std::wstring(COLL_ROOT).append(L"\\Bases_NULL_FOR_DNLD\\index\\6\\u0607g.xml").c_str(),
			std::wstring (UPD_ROOT).append (L"\\u0607g.xml"), false);

		UpdaterConfiguration data(1, 1, L"1");
		data.proxy_url = L"127.0.0.1";
		data.proxy_port = 3128;
		data.m_proxyAuthorizationCredentials.userName ( L"fakeuser");
		data.m_proxyAuthorizationCredentials.password ( L"fakepwd");
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
		//diff
		hs.Accept();
		QC_BOOST_CHECK_MESSAGE (hs.ReceivePureGet(), L"Error getting request\n");
		hs.SendNtlmBasicGreeting ();

		//ntlm
		QC_BOOST_CHECK_MESSAGE (hs.ReceiveGetWithNtlmToken("u0607g.xml.dif"), L"Error getting request\n");
		hs.SendNtlmMsg2();
		QC_BOOST_CHECK_MESSAGE (hs.ReceiveGetWithNtlmToken("u0607g.xml.dif"), L"Error getting request\n");
		hs.SendFileFromFile (HttpServer::KeepAlive, HttpServer::Close, std::wstring(COLL_ROOT).append(L"\\Bases_1\\data\\u0607g.xml"));
		hs.CloseConnection();

		//klz
		hs.Accept();
		QC_BOOST_CHECK_MESSAGE (hs.ReceiveGetWithNtlmToken("u0607g.xml.klz"), L"Error getting request\n");
		hs.SendNtlmMsg2();
		QC_BOOST_CHECK_MESSAGE (hs.ReceiveGetWithNtlmToken("u0607g.xml.klz"), L"Error getting request\n");
		hs.SendFileFromFile (HttpServer::KeepAlive, HttpServer::Close, std::wstring(COLL_ROOT).append(L"\\Bases_1\\data\\u0607g.xml"));
		hs.CloseConnection();

		//xml
		hs.Accept();
		QC_BOOST_CHECK_MESSAGE (hs.ReceiveGetWithNtlmToken("u0607g.xml"), L"Error getting request\n");
		hs.SendNtlmMsg2();
		QC_BOOST_CHECK_MESSAGE (hs.ReceiveGetWithNtlmToken("u0607g.xml"), L"Error getting request\n");
		hs.SendFileFromFile (HttpServer::KeepAlive, HttpServer::Close, std::wstring(COLL_ROOT).append(L"\\Bases_1\\data\\u0607g.xml"));
		hs.CloseConnection();

		//av-i386
		//diff
		hs.Accept();
		QC_BOOST_CHECK_MESSAGE (hs.ReceiveGetWithNtlmToken("av-i386-0607g.xml.dif"), L"Error getting request\n");
		hs.SendNtlmMsg2();
		QC_BOOST_CHECK_MESSAGE (hs.ReceiveGetWithNtlmToken("av-i386-0607g.xml.dif"), L"Error getting request\n");
		hs.SendFileFromFile (HttpServer::KeepAlive, HttpServer::Close, std::wstring(COLL_ROOT).append(L"\\Bases_1\\plain\\av-i386-0607g.xml"));
		hs.CloseConnection();

		//klz
		hs.Accept();
		QC_BOOST_CHECK_MESSAGE (hs.ReceiveGetWithNtlmToken("av-i386-0607g.xml.klz"), L"Error getting request\n");
		hs.SendNtlmMsg2();
		QC_BOOST_CHECK_MESSAGE (hs.ReceiveGetWithNtlmToken("av-i386-0607g.xml.klz"), L"Error getting request\n");
		hs.SendFileFromFile (HttpServer::KeepAlive, HttpServer::Close, std::wstring(COLL_ROOT).append(L"\\Bases_1\\plain\\av-i386-0607g.xml"));
		hs.CloseConnection();

		//xml
		hs.Accept();
		QC_BOOST_CHECK_MESSAGE (hs.ReceiveGetWithNtlmToken("av-i386-0607g.xml"), L"Error getting request\n");
		hs.SendNtlmMsg2();
		QC_BOOST_CHECK_MESSAGE (hs.ReceiveGetWithNtlmToken("av-i386-0607g.xml"), L"Error getting request\n");
		hs.SendFileFromFile (HttpServer::KeepAlive, HttpServer::Close, std::wstring(COLL_ROOT).append(L"\\Bases_1\\plain\\av-i386-0607g.xml"));
		hs.CloseConnection();

		//blst
		//diff
		hs.Accept();
		QC_BOOST_CHECK_MESSAGE (hs.ReceiveGetWithNtlmToken("blst-0607g.xml.dif"), L"Error getting request\n");
		hs.SendNtlmMsg2();
		QC_BOOST_CHECK_MESSAGE (hs.ReceiveGetWithNtlmToken("blst-0607g.xml.dif"), L"Error getting request\n");
		hs.SendFileFromFile (HttpServer::KeepAlive, HttpServer::Close, std::wstring(COLL_ROOT).append(L"\\Bases_1\\plain\\blst-0607g.xml"));
		hs.CloseConnection();

		//klz
		hs.Accept();
		QC_BOOST_CHECK_MESSAGE (hs.ReceiveGetWithNtlmToken("blst-0607g.xml.klz"), L"Error getting request\n");
		hs.SendNtlmMsg2();
		QC_BOOST_CHECK_MESSAGE (hs.ReceiveGetWithNtlmToken("blst-0607g.xml.klz"), L"Error getting request\n");
		hs.SendFileFromFile (HttpServer::KeepAlive, HttpServer::Close, std::wstring(COLL_ROOT).append(L"\\Bases_1\\plain\\blst-0607g.xml"));
		hs.CloseConnection();

		//xml
		hs.Accept();
		QC_BOOST_CHECK_MESSAGE (hs.ReceiveGetWithNtlmToken("blst-0607g.xml"), L"Error getting request\n");
		hs.SendNtlmMsg2();
		QC_BOOST_CHECK_MESSAGE (hs.ReceiveGetWithNtlmToken("blst-0607g.xml"), L"Error getting request\n");
		hs.SendFileFromFile (HttpServer::KeepAlive, HttpServer::Close, std::wstring(COLL_ROOT).append(L"\\Bases_1\\plain\\blst-0607g.xml"));
		hs.CloseConnection();

		//blst-file
		//diff
		hs.Accept();
		QC_BOOST_CHECK_MESSAGE (hs.ReceiveGetWithNtlmToken("black.lst."), L"Error getting request\n");
		hs.SendNtlmMsg2();
		QC_BOOST_CHECK_MESSAGE (hs.ReceiveGetWithNtlmToken("black.lst."), L"Error getting request\n");
		hs.Send403Forbidden (HttpServer::KeepAlive, HttpServer::Close);
		hs.CloseConnection();
		//no asking credentials

		//blst
		hs.Accept();
		QC_BOOST_CHECK_MESSAGE (hs.ReceivePureGet(), L"Error getting request\n");
		hs.SendNtlmBasicGreeting ();
		QC_BOOST_CHECK_MESSAGE (hs.ReceiveGetWithNtlmToken("black.lst"), L"Error getting request\n");
		hs.SendNtlmMsg2();
		QC_BOOST_CHECK_MESSAGE (hs.ReceiveGetWithNtlmToken("black.lst"), L"Error getting request\n");
		hs.Send403Forbidden (HttpServer::KeepAlive, HttpServer::Close);

		hs.Accept();
		QC_BOOST_CHECK_MESSAGE (hs.ReceiveGetWithBasicToken("black.lst"), L"Error getting request\n");
		hs.Send403Forbidden (HttpServer::KeepAlive, HttpServer::Close);
		hs.CloseConnection();
		//asking credentials
		WAIT_FOR_READY_EVENT(requestCredentialsForAuthorizationOnProxy)
		SET_GOON_EVENT(requestCredentialsForAuthorizationOnProxy);
		//blst again
		hs.Accept();
		QC_BOOST_CHECK_MESSAGE (hs.ReceiveGetWithBasicToken("black.lst"), L"Error getting request\n");
		hs.SendFileFromFile (HttpServer::KeepAlive, HttpServer::Close, std::wstring(COLL_ROOT).append(L"\\Bases_1\\plain\\black.lst"));
		hs.CloseConnection();

		//base-file
		//diff
		hs.Accept();
		QC_BOOST_CHECK_MESSAGE (hs.ReceiveGetWithBasicToken("base102.avc."), L"Error getting request\n");
		hs.Send403Forbidden (HttpServer::KeepAlive, HttpServer::Close);
		hs.CloseConnection();
		//no asking credentials

		//base file
		hs.Accept();
		QC_BOOST_CHECK_MESSAGE (hs.ReceivePureGet(), L"Error getting request\n");
		hs.SendNtlmBasicGreeting ();

		QC_BOOST_CHECK_MESSAGE (hs.ReceiveGetWithNtlmToken("base102.avc"), L"Error getting request\n");
		hs.SendNtlmMsg2();
		QC_BOOST_CHECK_MESSAGE (hs.ReceiveGetWithNtlmToken("base102.avc"), L"Error getting request\n");
		hs.Send403Forbidden (HttpServer::KeepAlive, HttpServer::Close);
		hs.CloseConnection();

		hs.Accept();
		QC_BOOST_CHECK_MESSAGE (hs.ReceiveGetWithBasicToken("base102.avc"), L"Error getting request\n");
		hs.Send403Forbidden (HttpServer::KeepAlive, HttpServer::Close);
		hs.CloseConnection();

		//asking credentials
		WAIT_FOR_READY_EVENT(requestCredentialsForAuthorizationOnProxy)
		SET_GOON_EVENT(requestCredentialsForAuthorizationOnProxy);
		//base file again
		hs.Accept();
		QC_BOOST_CHECK_MESSAGE (hs.ReceiveGetWithBasicToken("base102.avc"), L"Error getting request\n");
		hs.SendFileFromFile (HttpServer::KeepAlive, HttpServer::Close, std::wstring(COLL_ROOT).append(L"\\Bases_1\\plain\\base102.avc"));
		hs.CloseConnection();
		//base file 2
		hs.Accept();
		QC_BOOST_CHECK_MESSAGE (hs.ReceiveGetWithBasicToken("base103.avc"), L"Error getting request\n");
		hs.SendFileFromFile (HttpServer::KeepAlive, HttpServer::Close, std::wstring(COLL_ROOT).append(L"\\Bases_1\\plain\\base103.avc"));
		hs.CloseConnection();

		WaitForSingleObject (hDoUpdate, INFINITE);
		QC_BOOST_CHECK_MESSAGE ( CORE_NO_ERROR == do_upd.GetError(), L"Wrong return code\n" );
	}*/

}

void TestHttpProxy::TestSpecialProxyCases_Code407 ()
{
	using namespace KLUPD;
	std::wcout<<L"TestSpecialProxyCases_Code407\n";
	Logger::Case _case (m_logger, m_testp, m_step, L"TestSpecialProxyCases_Code407");

/*	
	{	
		//proxy (NTLM, NTLMwithCredentials, Basic) returns 407 error code
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
		data.m_httpAuthorizationMethods.push_back (ntlmAuthorizationWithCredentials);
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
		data.m_componentsToUpdate.push_back (NoCaseString(L"CORE") );
		data.connect_tmo = 30;

		data.m_sourceList.addSource (UST_UserURL, L"http://bases", true, L"diffs");
		data.m_sourceList.addSource (UST_UserURL, L"http://bases2", true, L"diffs");

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

		//first source
		{	//klz

			hs.Accept();
			QC_BOOST_CHECK_MESSAGE (hs.ReceivePureGet("host: bases\r\n"), L"Error getting request\n");
			hs.SendNtlmBasicGreeting ();

			//ntlm
			QC_BOOST_CHECK_MESSAGE (hs.ReceiveGetWithNtlmToken("host: bases\r\n"), L"Error getting request\n");
			hs.SendNtlmMsg2();
			QC_BOOST_CHECK_MESSAGE (hs.ReceiveGetWithNtlmToken("host: bases\r\n"), L"Error getting request\n");
			hs.SendNtlmBasicGreeting ();
			//ntlm with creds
			QC_BOOST_CHECK_MESSAGE (hs.ReceiveGetWithNtlmToken("host: bases\r\n"), L"Error getting request\n");
			hs.SendNtlmMsg2();
			QC_BOOST_CHECK_MESSAGE (hs.ReceiveGetWithNtlmToken("host: bases\r\n"), L"Error getting request\n");
			hs.SendNtlmBasicGreeting ();

			for (int i = 0; i <= 1; ++i)
			{//klz und xml
				if (i == 1)
				{
					//ntlm
					QC_BOOST_CHECK_MESSAGE (hs.ReceiveGetWithNtlmToken("host: bases\r\n"), L"Error getting request\n");
					hs.SendNtlmMsg2();
					QC_BOOST_CHECK_MESSAGE (hs.ReceiveGetWithNtlmToken("host: bases\r\n"), L"Error getting request\n");
					hs.SendNtlmBasicGreeting ();
				}

				for (int i = 1; i < 3; ++i)
				{
					//request credentials
					WAIT_FOR_READY_EVENT(requestCredentialsForAuthorizationOnProxy)
					TestCallbackInterface::_credentials creds;
					creds.name = L"test";
					creds.password = L"test";
					testCallback.SetRequestCreds(creds);
					SET_GOON_EVENT(requestCredentialsForAuthorizationOnProxy);

					QC_BOOST_CHECK_MESSAGE (hs.ReceiveGetWithNtlmToken("host: bases\r\n"), L"Error getting request\n");
					hs.SendNtlmMsg2();
					QC_BOOST_CHECK_MESSAGE (hs.ReceiveGetWithNtlmToken("host: bases\r\n"), L"Error getting request\n");
					hs.SendNtlmBasicGreeting ();
				}

				CANCEL_CREDENTIALS
			}
			
		}

		//hs.CloseConnection();

		//second source
		{	//klz

			//hs.Accept();

			for (int i = 0; i <= 1; ++i)
			{//klz und xml

				//ntlm
				QC_BOOST_CHECK_MESSAGE (hs.ReceiveGetWithNtlmToken("host: bases2\r\n"), L"Error getting request\n");
				hs.SendNtlmMsg2();
				QC_BOOST_CHECK_MESSAGE (hs.ReceiveGetWithNtlmToken("host: bases2\r\n"), L"Error getting request\n");
				hs.SendNtlmBasicGreeting ();


				for (int i = 1; i < 3; ++i)
				{
					//request credentials
					WAIT_FOR_READY_EVENT(requestCredentialsForAuthorizationOnProxy)
					TestCallbackInterface::_credentials creds;
					creds.name = L"test";
					creds.password = L"test";
					testCallback.SetRequestCreds(creds);
					SET_GOON_EVENT(requestCredentialsForAuthorizationOnProxy);

					QC_BOOST_CHECK_MESSAGE (hs.ReceiveGetWithNtlmToken("host: bases2\r\n"), L"Error getting request\n");
					hs.SendNtlmMsg2();
					QC_BOOST_CHECK_MESSAGE (hs.ReceiveGetWithNtlmToken("host: bases2\r\n"), L"Error getting request\n");
					hs.SendNtlmBasicGreeting ();
				}

				CANCEL_CREDENTIALS
			}
		}

		WaitForSingleObject (hDoUpdate, INFINITE);
		//QC_BOOST_CHECK_MESSAGE ( CORE_DOWNLOAD_ERROR == do_upd.GetError(), L"Wrong return code\n" );
		QC_BOOST_CHECK_MESSAGE ( CORE_PROXY_AUTH_ERROR == do_upd.GetError(), L"Wrong return code\n" );
	}
*/	
	{
		HttpServer hs (3128, "127.0.0.1");
		hs.Start();

		TestLog testLog;

		_helper::DeleteDirectory(ROOT);
		KLUPD::createFolder (BASES_ROOT, &testLog);
		KLUPD::createFolder (UPD_ROOT, &testLog);

		_helper::CopyFolder(std::wstring(COLL_ROOT).append(L"\\Bases_NULL_FOR_DNLD\\plain").c_str(),BASES_ROOT);
		_helper::CloneFile (std::wstring(COLL_ROOT).append(L"\\Bases_NULL_FOR_DNLD\\index\\6\\u0607g.xml").c_str(),
			std::wstring (UPD_ROOT).append (L"\\u0607g.xml"), false);

		UpdaterConfiguration data(1, 1, L"1");
		data.proxy_url = L"127.0.0.1";
		data.proxy_port = 3128;
		data.m_proxyAuthorizationCredentials.userName ( L"fakeuser");
		data.m_proxyAuthorizationCredentials.password ( L"fakepwd");
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
		//testCallback.SetRequestCreds(creds);

		Updater upd (testCallback.m_dp, testCallback, testJournal, &testLog);
		HANDLE hDoUpdate;

		doUpdater do_upd(&upd, hDoUpdate);
		//u0607
		//diff
		hs.Accept();
		QC_BOOST_CHECK_MESSAGE (hs.ReceivePureGet(), L"Error getting request\n");
		hs.SendNtlmBasicGreeting ();
		QC_BOOST_CHECK_MESSAGE (hs.ReceiveGetWithNtlmToken("u0607g.xml.dif"), L"Error getting request\n");
		hs.SendNtlmMsg2();
		QC_BOOST_CHECK_MESSAGE (hs.ReceiveGetWithNtlmToken("u0607g.xml.dif"), L"Error getting request\n");
		hs.SendNtlmBasicGreeting ();
		QC_BOOST_CHECK_MESSAGE (hs.ReceiveGetWithBasicToken("u0607g.xml.dif"), L"Error getting request\n");
		hs.SendNtlmBasicGreeting ();//hs.Send403Forbidden (HttpServer::KeepAlive, HttpServer::Close);
		//hs.CloseConnection();
		//asking credentials
		WAIT_FOR_READY_EVENT(requestCredentialsForAuthorizationOnProxy)
		creds.result = false;
		testCallback.SetRequestCreds(creds);
		SET_GOON_EVENT(requestCredentialsForAuthorizationOnProxy);
		//hs.Accept();
		//QC_BOOST_CHECK_MESSAGE (hs.ReceiveGetWithBasicToken("u0607g.xml.dif"), L"Error getting request\n");
		//hs.SendFileFromFile (HttpServer::KeepAlive, HttpServer::Close, std::wstring(COLL_ROOT).append(L"\\Bases_1\\data\\u0607g.xml"));
		//hs.CloseConnection();

		//klz
		//hs.Accept();
		QC_BOOST_CHECK_MESSAGE (hs.ReceiveGetWithNtlmToken("u0607g.xml.klz"), L"Error getting request\n");
		hs.SendNtlmMsg2();
		QC_BOOST_CHECK_MESSAGE (hs.ReceiveGetWithNtlmToken("u0607g.xml.klz"), L"Error getting request\n");
		hs.SendNtlmBasicGreeting ();
		//QC_BOOST_CHECK_MESSAGE (hs.ReceiveGetWithBasicToken("u0607g.xml.klz"), L"Error getting request\n");
		//hs.SendNtlmBasicGreeting ();//hs.Send403Forbidden (HttpServer::KeepAlive, HttpServer::Close);
		//hs.CloseConnection();
		//asking credentials
		WAIT_FOR_READY_EVENT(requestCredentialsForAuthorizationOnProxy)
		creds.result = false;
		testCallback.SetRequestCreds(creds);
		SET_GOON_EVENT(requestCredentialsForAuthorizationOnProxy);
		//hs.Accept();
		//QC_BOOST_CHECK_MESSAGE (hs.ReceiveGetWithBasicToken("u0607g.xml.klz"), L"Error getting request\n");
		//hs.SendFileFromFile (HttpServer::KeepAlive, HttpServer::Close, std::wstring(COLL_ROOT).append(L"\\Bases_1\\data\\u0607g.xml"));
		//hs.CloseConnection();

		//xml
		//hs.Accept();
		QC_BOOST_CHECK_MESSAGE (hs.ReceiveGetWithNtlmToken("u0607g.xml"), L"Error getting request\n");
		hs.SendNtlmMsg2();
		QC_BOOST_CHECK_MESSAGE (hs.ReceiveGetWithNtlmToken("u0607g.xml"), L"Error getting request\n");
		hs.SendNtlmBasicGreeting ();
		//QC_BOOST_CHECK_MESSAGE (hs.ReceiveGetWithBasicToken("u0607g.xml"), L"Error getting request\n");
		//hs.SendNtlmBasicGreeting ();//hs.Send403Forbidden (HttpServer::KeepAlive, HttpServer::Close);
		//hs.CloseConnection();
		//asking credentials
		WAIT_FOR_READY_EVENT(requestCredentialsForAuthorizationOnProxy)
		creds.result = true;
		testCallback.SetRequestCreds(creds);
		SET_GOON_EVENT(requestCredentialsForAuthorizationOnProxy);
		//hs.Accept();
		QC_BOOST_CHECK_MESSAGE (hs.ReceiveGetWithBasicToken("u0607g.xml"), L"Error getting request\n");
		hs.SendFileFromFile (HttpServer::KeepAlive, HttpServer::Close, std::wstring(COLL_ROOT).append(L"\\Bases_1\\data\\u0607g.xml"));
		hs.CloseConnection();
//<здесь повторение старого теста>
		//av-i386
		//diff
		hs.Accept();
		QC_BOOST_CHECK_MESSAGE (hs.ReceiveGetWithBasicToken("av-i386-0607g.xml.dif"), L"Error getting request\n");
		hs.Send403Forbidden (HttpServer::KeepAlive, HttpServer::Close);
		hs.CloseConnection();
		//asking credentials
		WAIT_FOR_READY_EVENT(requestCredentialsForAuthorizationOnProxy)
		creds.result = true;
		testCallback.SetRequestCreds(creds);
		SET_GOON_EVENT(requestCredentialsForAuthorizationOnProxy);
		hs.Accept();
		QC_BOOST_CHECK_MESSAGE (hs.ReceiveGetWithBasicToken("av-i386-0607g.xml.dif"), L"Error getting request\n");
		hs.SendFileFromFile (HttpServer::KeepAlive, HttpServer::Close, std::wstring(COLL_ROOT).append(L"\\Bases_1\\plain\\av-i386-0607g.xml"));
		hs.CloseConnection();

		//klz
		hs.Accept();
		QC_BOOST_CHECK_MESSAGE (hs.ReceiveGetWithBasicToken("av-i386-0607g.xml.klz"), L"Error getting request\n");
		hs.Send403Forbidden (HttpServer::KeepAlive, HttpServer::Close);
		hs.CloseConnection();
		//asking credentials
		WAIT_FOR_READY_EVENT(requestCredentialsForAuthorizationOnProxy)
		creds.result = true;
		testCallback.SetRequestCreds(creds);
		SET_GOON_EVENT(requestCredentialsForAuthorizationOnProxy);
		hs.Accept();
		QC_BOOST_CHECK_MESSAGE (hs.ReceiveGetWithBasicToken("av-i386-0607g.xml.klz"), L"Error getting request\n");
		hs.SendFileFromFile (HttpServer::KeepAlive, HttpServer::Close, std::wstring(COLL_ROOT).append(L"\\Bases_1\\plain\\av-i386-0607g.xml"));
		hs.CloseConnection();

		//xml
		hs.Accept();
		QC_BOOST_CHECK_MESSAGE (hs.ReceiveGetWithBasicToken("av-i386-0607g.xml"), L"Error getting request\n");
		hs.Send403Forbidden (HttpServer::KeepAlive, HttpServer::Close);
		hs.CloseConnection();
		//asking credentials
		WAIT_FOR_READY_EVENT(requestCredentialsForAuthorizationOnProxy)
		creds.result = true;
		testCallback.SetRequestCreds(creds);
		SET_GOON_EVENT(requestCredentialsForAuthorizationOnProxy);
		hs.Accept();
		QC_BOOST_CHECK_MESSAGE (hs.ReceiveGetWithBasicToken("av-i386-0607g.xml"), L"Error getting request\n");
		hs.SendFileFromFile (HttpServer::KeepAlive, HttpServer::Close, std::wstring(COLL_ROOT).append(L"\\Bases_1\\plain\\av-i386-0607g.xml"));
		hs.CloseConnection();

		//blst
		//diff
		hs.Accept();
		QC_BOOST_CHECK_MESSAGE (hs.ReceiveGetWithBasicToken("blst-0607g.xml.dif"), L"Error getting request\n");
		hs.Send403Forbidden (HttpServer::KeepAlive, HttpServer::Close);
		hs.CloseConnection();
		//asking credentials
		WAIT_FOR_READY_EVENT(requestCredentialsForAuthorizationOnProxy)
		creds.result = true;
		testCallback.SetRequestCreds(creds);
		SET_GOON_EVENT(requestCredentialsForAuthorizationOnProxy);
		hs.Accept();
		QC_BOOST_CHECK_MESSAGE (hs.ReceiveGetWithBasicToken("blst-0607g.xml.dif"), L"Error getting request\n");
		hs.SendFileFromFile (HttpServer::KeepAlive, HttpServer::Close, std::wstring(COLL_ROOT).append(L"\\Bases_1\\plain\\blst-0607g.xml"));
		hs.CloseConnection();

		//klz
		hs.Accept();
		QC_BOOST_CHECK_MESSAGE (hs.ReceiveGetWithBasicToken("blst-0607g.xml.klz"), L"Error getting request\n");
		hs.Send403Forbidden (HttpServer::KeepAlive, HttpServer::Close);
		hs.CloseConnection();
		//asking credentials
		WAIT_FOR_READY_EVENT(requestCredentialsForAuthorizationOnProxy)
		creds.result = true;
		testCallback.SetRequestCreds(creds);
		SET_GOON_EVENT(requestCredentialsForAuthorizationOnProxy);
		hs.Accept();
		QC_BOOST_CHECK_MESSAGE (hs.ReceiveGetWithBasicToken("blst-0607g.xml.klz"), L"Error getting request\n");
		hs.SendFileFromFile (HttpServer::KeepAlive, HttpServer::Close, std::wstring(COLL_ROOT).append(L"\\Bases_1\\plain\\blst-0607g.xml"));
		hs.CloseConnection();

		//xml
		hs.Accept();
		QC_BOOST_CHECK_MESSAGE (hs.ReceiveGetWithBasicToken("blst-0607g.xml"), L"Error getting request\n");
		hs.Send403Forbidden (HttpServer::KeepAlive, HttpServer::Close);
		hs.CloseConnection();
		//asking credentials
		WAIT_FOR_READY_EVENT(requestCredentialsForAuthorizationOnProxy)
		creds.result = true;
		testCallback.SetRequestCreds(creds);
		SET_GOON_EVENT(requestCredentialsForAuthorizationOnProxy);
		hs.Accept();
		QC_BOOST_CHECK_MESSAGE (hs.ReceiveGetWithBasicToken("blst-0607g.xml"), L"Error getting request\n");
		hs.SendFileFromFile (HttpServer::KeepAlive, HttpServer::Close, std::wstring(COLL_ROOT).append(L"\\Bases_1\\plain\\blst-0607g.xml"));
		hs.CloseConnection();

		//blst-file
		//diff
		hs.Accept();
		QC_BOOST_CHECK_MESSAGE (hs.ReceiveGetWithBasicToken("black.lst."), L"Error getting request\n");
		hs.Send403Forbidden (HttpServer::KeepAlive, HttpServer::Close);
		hs.CloseConnection();
		//no asking credentials

		//blst
		hs.Accept();
		QC_BOOST_CHECK_MESSAGE (hs.ReceivePureGet(), L"Error getting request\n");
		hs.SendBasicGreeting ();
		QC_BOOST_CHECK_MESSAGE (hs.ReceiveGetWithBasicToken("black.lst"), L"Error getting request\n");
		hs.Send403Forbidden (HttpServer::KeepAlive, HttpServer::Close);
		hs.CloseConnection();
		//asking credentials
		WAIT_FOR_READY_EVENT(requestCredentialsForAuthorizationOnProxy)
		creds.result = true;
		testCallback.SetRequestCreds(creds);
		SET_GOON_EVENT(requestCredentialsForAuthorizationOnProxy);
		//blst again
		hs.Accept();
		QC_BOOST_CHECK_MESSAGE (hs.ReceiveGetWithBasicToken("black.lst"), L"Error getting request\n");
		hs.SendFileFromFile (HttpServer::KeepAlive, HttpServer::Close, std::wstring(COLL_ROOT).append(L"\\Bases_1\\plain\\black.lst"));
		hs.CloseConnection();
//<здесь повторение старого теста заканчивается>
		//base-file
		//diff
		hs.Accept();
		QC_BOOST_CHECK_MESSAGE (hs.ReceiveGetWithBasicToken("base102.avc."), L"Error getting request\n");
		hs.SendNtlmBasicGreeting ();//hs.Send403Forbidden (HttpServer::KeepAlive, HttpServer::Close);
		//hs.CloseConnection();
		//no asking credentials

		//base file
		//hs.Accept();
	/*	QC_BOOST_CHECK_MESSAGE (hs.ReceivePureGet(), L"Error getting request\n");
		hs.SendBasicGreeting ();
		QC_BOOST_CHECK_MESSAGE (hs.ReceiveGetWithBasicToken("base102.avc"), L"Error getting request\n");
		hs.Send403Forbidden (HttpServer::KeepAlive, HttpServer::Close);
		hs.CloseConnection();*/
		//asking credentials
		WAIT_FOR_READY_EVENT(requestCredentialsForAuthorizationOnProxy)
		creds.result = false;
		testCallback.SetRequestCreds(creds);
		SET_GOON_EVENT(requestCredentialsForAuthorizationOnProxy);
		//base file again
		QC_BOOST_CHECK_MESSAGE (hs.ReceiveGetWithNtlmToken("base102.avc"), L"Error getting request\n");
		hs.SendNtlmMsg2();
		QC_BOOST_CHECK_MESSAGE (hs.ReceiveGetWithNtlmToken("base102.avc"), L"Error getting request\n");
		hs.SendNtlmBasicGreeting ();
		
		WAIT_FOR_READY_EVENT(requestCredentialsForAuthorizationOnProxy)
		creds.result = true;
		testCallback.SetRequestCreds(creds);
		SET_GOON_EVENT(requestCredentialsForAuthorizationOnProxy);

		hs.SendFileFromFile (HttpServer::KeepAlive, HttpServer::Close, std::wstring(COLL_ROOT).append(L"\\Bases_1\\plain\\base102.avc"));
		hs.CloseConnection();
		
		//base file 2
		hs.Accept();
		QC_BOOST_CHECK_MESSAGE (hs.ReceiveGetWithBasicToken("base103.avc"), L"Error getting request\n");
		hs.SendFileFromFile (HttpServer::KeepAlive, HttpServer::Close, std::wstring(COLL_ROOT).append(L"\\Bases_1\\plain\\base103.avc"));
		hs.CloseConnection();

		WaitForSingleObject (hDoUpdate, INFINITE);
		QC_BOOST_CHECK_MESSAGE ( CORE_NO_ERROR == do_upd.GetError(), L"Wrong return code\n" );
	}


}