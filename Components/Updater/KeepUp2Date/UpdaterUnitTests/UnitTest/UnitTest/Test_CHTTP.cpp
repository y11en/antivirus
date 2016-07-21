#include "Test_CHTTP.h"
#include "helper.h"

using namespace KLUPD;

void Test_CHTTP::TestGeneratingRequest()
{
	string reason;
	std::cout<<"Testing generating HTTP request\n";

	vector<char> cred;
	CHTTPData _chd (&m_log);

	CQueryData _cqd;
	_cqd.m_userAgent = "Stasasan";
	_cqd.m_server = "kiryukhin-xp";
	_cqd.m_serverType = NFT_HTTP;
	_cqd.m_rootFolder = "/relative/";

	CProxyData _cpd;
	_cpd.m_type = PXY_None;
	_cpd.m_userName = "kl\\tester";
	_cpd.m_password = "kf;0dsq Ntcnth";
	
	_cpd.m_server = ZHUZHNEV_PROXY;
	_cpd.m_serverPort = ZHUZHNEV_BASIC_AUTH_PORT;


	CNetFileInfo _cnf;
    _cnf.m_remoteFilename = "base005.avc";
    _cnf.m_localPath = "c:\\test";
    _cnf.m_localFilename = "base006.avc";
    _cnf.m_regetting = true;
    _cnf.m_regetPosition = 333;

	//case 1
	string req1 ("GET /relative/base005.avc HTTP/1.0\r\nHost: kiryukhin-xp\r\nPragma: no-cache\r\nCache-Control: no-cache\r\nConnection: keep-alive\r\nUser-Agent: Stasasan\r\nRange: bytes=333-\r\n\r\n");
	_chd.Init (_cqd, _cpd, _cnf, AUT_Undefined, AUF_Unknown);
	TD_BOOST_CHECK_MESSAGE (req1 == _chd.generateRequest (), "Error while generating request #1");
	//TD_BOOST_CHECK_MESSAGE (req1 == request, "Incorrect request #1");

	//case 2
	string req2 ("GET http://kiryukhin-xp/relative/base005.avc HTTP/1.0\r\nHost: kiryukhin-xp\r\nPragma: no-cache\r\nCache-Control: no-cache\r\nConnection: keep-alive\r\nUser-Agent: Stasasan\r\nProxy-Authorization: NTLM TlRMTVNTUAABAAAAB7IIogIAAgA0AAAADAAMACgAAAAFASgKAAAAD0tJUllVS0hJTi1YUEtM\r\nRange: bytes=333-\r\n\r\n");
	
	_cpd.m_type = PXY_HTTP;
	
	_chd.Init (_cqd, _cpd, _cnf, AUT_Ntlm, AUF_NTLMInitiation);
	TD_BOOST_CHECK_MESSAGE (req2 == _chd.generateRequest (), "Error while generating request #2");
	//TD_BOOST_CHECK_MESSAGE (req2 == request, "Incorrect request #2");

	//case 2a
	string req2a ("GET http://kiryukhin-xp/relative/base005.avc HTTP/1.0\r\nHost: kiryukhin-xp\r\nPragma: no-cache\r\nCache-Control: no-cache\r\nConnection: keep-alive\r\nUser-Agent: Stasasan\r\nProxy-Authorization: NTLM TlRMTVNTUAABAAAAB4IIogAAAAAAAAAAAAAAAAAAAAAFASgKAAAADw==\r\nRange: bytes=333-\r\n\r\n");
	
	_cpd.m_type = PXY_HTTP;
	
	_chd.Init (_cqd, _cpd, _cnf, AUT_NtlmWithCredentials, AUF_NTLMInitiation);
	TD_BOOST_CHECK_MESSAGE (req2a == _chd.generateRequest (), "Error while generating request #2a");
	//TD_BOOST_CHECK_MESSAGE (req2 == request, "Incorrect request #2");


	//case 3
	string req3 ("GET http://downloads1.kaspersky-labs.com/base005.avc HTTP/1.0\r\nHost: downloads1.kaspersky-labs.com\r\nPragma: no-cache\r\nCache-Control: no-cache\r\nConnection: keep-alive\r\nUser-Agent: KLAgent\r\nProxy-Authorization: Basic a2xcdGVzdGVyOmtmOzBkc3EgTnRjbnRo\r\nRange: bytes=444-\r\n\r\n");
	
	_cpd.m_type = PXY_HTTP;
	_cqd.m_rootFolder = "";
	_cqd.m_userAgent = "KLAgent";
	_cqd.m_server = "downloads1.kaspersky-labs.com";
	_cnf.m_regetPosition = 444;
	
	_chd.Init (_cqd, _cpd, _cnf, AUT_Undefined, AUF_Basic);
	TD_BOOST_CHECK_MESSAGE (req3 == _chd.generateRequest (), "Error while generating request #3");
	//TD_BOOST_CHECK_MESSAGE (req3 == request, "Incorrect request #3");

	//case 4
	string req4 ("GET /base005.avc HTTP/1.0\r\nHost: downloads2.com\r\nPragma: no-cache\r\nCache-Control: no-cache\r\nConnection: keep-alive\r\n\r\n");
	
	_cpd.m_type = PXY_None;
	_cqd.m_rootFolder = "";
	_cqd.m_userAgent = "";
	_cqd.m_server = "downloads2.com";
	_cnf.m_regetting = false;
	
	_chd.Init (_cqd, _cpd, _cnf, AUT_Undefined, AUF_Unknown);
	TD_BOOST_CHECK_MESSAGE (req4 == _chd.generateRequest (), "Error while generating request #4");
	//TD_BOOST_CHECK_MESSAGE (req4 == request, "Incorrect request #4");
	Step ("TestGeneratingRequest", reason);			
};

void Test_CHTTP::TestMakingAuthPhrase()
{
	string reason;
	string ntlm_request = "NTLM TlRMTVNTUAABAAAAB7IIogIAAgA0AAAADAAMACgAAAAFASgKAAAAD0tJUllVS0hJTi1YUEtM";
	string ntlm_serverToken = "NTLM TlRMTVNTUAACAAAAAAAAADAAAAAyAgAAxDzAtKJdLpIAAAAAAAAAAAAAAAAwAAAA";
	//string ntlm_response = "NTLM TlRMTVNTUAADAAAAGAAYAF8AAAAYABgAdwAAAAIAAgBIAAAACQAJAEoAAAAMAAwAUwAAAAAAAACPAAAABgIAAgUBKAoAAAAPS0xLSVJZVUtISU5LSVJZVUtISU4tWFCiDCrzVKp0Y/OYzZEHd1eZHNfwxyy5HoX5m9Zs4WZAQMsQio6ETI3o7cammYvosqU=";
	string ntlm_response = "NTLM TlRMTVNTUAADAAAAGAAYAF8AAAAYABgAdwAAAAIAAgBIAAAACQAJAEoAAAAMAAwAUwAAAAAAAACPAAAABgIAAgUBKAoAAAAPS0xLSVJZVUtISU5LSVJZVUtISU4tWFASSB+Su/iaq2J9D5F0wWsXHNfwxyy5HoWr1Y5knR9Qyk0UMq5JT3ZpO8H693cATyM=";
	string basic = "Basic a2xcdGVzdGVyOmtmOzBkc3EgTnRjbnRo";
	string ntlm_request_man = "NTLM TlRMTVNTUAABAAAAB4IIogAAAAAAAAAAAAAAAAAAAAAFASgKAAAADw==";
	string ntlm_serverToken_man = "NTLM TlRMTVNTUAACAAAAAAAAADAAAAAyAgAA85DDvQuCsj8AAAAAAAAAAAAAAAAwAAAA";
	string ntlm_response_man = "NTLM TlRMTVNTUAADAAAAGAAYAFwAAAAYABgAdAAAAAIAAgBIAAAABgAGAEoAAAAMAAwAUAAAAAAAAACMAAAABgIAAgUBKAoAAAAPS0xURVNURVJLSVJZVUtISU4tWFBnCwR4N/9lv4jW6r4w2c7+blUeOZUPta/kNddQf8MzCrkvvi4+ykWOj58cD/leK5I=";
	
	
	std::cout<<"Testing generating Authorization phrases\n";

	vector<char> cred;
	CHTTPData _chd (&m_log);
	CQueryData _cqd;
	_cqd.m_serverType = NFT_HTTP;
	CProxyData _cpd;
	_cpd.m_type = PXY_HTTP;
	_cpd.m_userName = "kl\\tester";
	_cpd.m_password = "kf;0dsq Ntcnth";

	CNetFileInfo _cnf;

	//case 1
	_chd.Init (_cqd, _cpd, _cnf, AUT_Undefined, AUF_Unknown);

	TD_BOOST_CHECK_MESSAGE (NFER_NoError == _chd.constructProxyAuthorizationPhrase (AUT_Undefined, AUF_Basic, ""), "Error while generating request #1");
	TD_BOOST_CHECK_MESSAGE (basic == _chd.m_proxyAuthorizationField, "Incorrect phrase #1");

	std::cout<<"NTLM automatic\n";
	//case 2
	_chd.Init (_cqd, _cpd, _cnf, AUT_Ntlm, AUF_NTLMInitiation);

	TD_BOOST_CHECK_MESSAGE (NFER_NoError == _chd.constructProxyAuthorizationPhrase (AUT_Ntlm, AUF_NTLMInitiation, ""), "Error while generating request #2");
	TD_BOOST_CHECK_MESSAGE (ntlm_request == _chd.m_proxyAuthorizationField, "Incorrect phrase #2");

	//case 3
	_chd.Init (_cqd, _cpd, _cnf, AUT_Ntlm, AUF_NTLMChallenge);

	TD_BOOST_CHECK_MESSAGE (NFER_NoError == _chd.constructProxyAuthorizationPhrase (AUT_Ntlm, AUF_NTLMChallenge, ntlm_serverToken.c_str()), "Error while generating request #3");
	TD_BOOST_CHECK_MESSAGE (ntlm_response == _chd.m_proxyAuthorizationField, "Incorrect phrase #3");

	//case 4
	_chd.Init (_cqd, _cpd, _cnf, AUT_Ntlm, AUF_NTLMAuthorized);

	TD_BOOST_CHECK_MESSAGE (NFER_NoError == _chd.constructProxyAuthorizationPhrase (AUT_Ntlm, AUF_NTLMAuthorized, ntlm_serverToken.c_str()), "Error while generating request #4");
	TD_BOOST_CHECK_MESSAGE (_chd.m_proxyAuthorizationField == "", "Incorrect phrase #4");

	std::cout<<"NTLM manual\n";
	//case 5
	_chd.Init (_cqd, _cpd, _cnf, AUT_NtlmWithCredentials, AUF_NTLMInitiation);

	TD_BOOST_CHECK_MESSAGE (NFER_NoError == _chd.constructProxyAuthorizationPhrase (AUT_NtlmWithCredentials, AUF_NTLMInitiation, ""), "Error while generating request #5");
	TD_BOOST_CHECK_MESSAGE (ntlm_request_man == _chd.m_proxyAuthorizationField, "Incorrect phrase #5");

	//case 6
	_chd.Init (_cqd, _cpd, _cnf, AUT_NtlmWithCredentials, AUF_NTLMChallenge);

	TD_BOOST_CHECK_MESSAGE (NFER_NoError == _chd.constructProxyAuthorizationPhrase (AUT_NtlmWithCredentials, AUF_NTLMChallenge, ntlm_serverToken_man.c_str()), "Error while generating request #6");
	TD_BOOST_CHECK_MESSAGE (ntlm_response_man == _chd.m_proxyAuthorizationField, "Incorrect phrase #6");

	//case 7
	_chd.Init (_cqd, _cpd, _cnf, AUT_NtlmWithCredentials, AUF_NTLMAuthorized);

	TD_BOOST_CHECK_MESSAGE (NFER_NoError == _chd.constructProxyAuthorizationPhrase (AUT_NtlmWithCredentials, AUF_NTLMAuthorized, ntlm_serverToken.c_str()), "Error while generating request #7");
	TD_BOOST_CHECK_MESSAGE (_chd.m_proxyAuthorizationField == "", "Incorrect phrase #7");

	Step ("TestMakingAuthPhrase", reason);
};

void Test_CHTTP::TestParsingHeader()
{
	string reason;
	std::cout<<"Testing parsing HTTP headers\n";
	CHTTPHeader _chh;

	string header1 = "\
HTTP/1.0 407 Proxy Authentication Required\r\n\
Server: Squid/2.4.STABLE7\r\n\
Mime-Version: 1.0\r\n\
Date: Sat, 08 Apr 2006 13:19:11 GMT\r\n\
Content-Type: text/html\r\n\
Content-Length: 1007\r\n\
Expires: Sat, 08 Apr 2006 13:19:11 GMT\r\n\
X-Squid-Error: ERR_CACHE_ACCESS_DENIED 0\r\n\
Proxy-Authenticate: Basic realm=\"Squid proxy-caching web server\"\r\n\
X-Cache: MISS from tl-unix-mailserver.avp.ru\r\n\
X-Cache-Lookup: NONE from tl-unix-mailserver.avp.ru:3128\r\n\
Proxy-Connection: keep-alive\
";
	string auth1 = "Basic realm=\"Squid proxy-caching web server\"";

	TD_BOOST_CHECK_MESSAGE (_chh.load (header1.c_str()), "Error while loading header #1");
	TD_BOOST_CHECK_MESSAGE (_chh.m_httpCode == 407 &&
						 _chh.m_contentLength == 1007 &&
						 _chh.m_connection == 0 &&
						 _chh.m_proxyConnection == 2 &&
						 _chh.m_proxyAuthorizationMethods == vector<string>(1,auth1)
						 , "Incorrect parsing header #1");
	
	string header2 = "\
HTTP/1.0 200 OK\r\n\
Server: Microsoft-IIS/5.1\r\n\
Date: Sat, 08 Apr 2006 13:49:53 GMT\r\n\
Content-Type: application/octet-stream\r\n\
Accept-Ranges: bytes\r\n\
Last-Modified: Wed, 22 Mar 2006 03:40:00 GMT\r\n\
ETag: \"068c54b624dc61:a45\"\r\n\
Content-Length: 48549\r\n\
X-Cache: MISS from squid.avp.ru\r\n\
X-Cache-Lookup: MISS from squid.avp.ru:3128\r\n\
X-Cache: MISS from tl-unix-mailserver.avp.ru\r\n\
X-Cache-Lookup: HIT from tl-unix-mailserver.avp.ru:8080\r\n\
Proxy-Connection: keep-alive\
";

	TD_BOOST_CHECK_MESSAGE (_chh.load (header2.c_str()), "Error while loading header #2");
	TD_BOOST_CHECK_MESSAGE (_chh.m_httpCode == 200 &&
						 _chh.m_contentLength == 48549 &&
						 _chh.m_connection == 0 &&
						 _chh.m_proxyConnection == 2 &&
						 _chh.m_proxyAuthorizationMethods == vector<string>()
						 , "Incorrect parsing header #2");

	string header3 = "\
HTTP/1.0 407 Proxy Authentication Required\r\n\
Server: squid/2.5.STABLE13\r\n\
Mime-Version: 1.0\r\n\
Date: Sat, 08 Apr 2006 13:49:59 GMT\r\n\
Content-Type: text/html\r\n\
Content-Length: 1372\r\n\
Expires: Sat, 08 Apr 2006 13:49:59 GMT\r\n\
X-Squid-Error: ERR_CACHE_ACCESS_DENIED 0\r\n\
Proxy-Authenticate: NTLM\r\n\
Proxy-Authenticate: Basic realm=\"KL Squid proxy-caching web server\"\r\n\
X-Cache: MISS from squid.avp.ru\r\n\
X-Cache-Lookup: NONE from squid.avp.ru:3128\r\n\
Proxy-Connection: close\
";
	vector<string> auth2;
	auth2.push_back ("NTLM");
	auth2.push_back ("Basic realm=\"KL Squid proxy-caching web server\"");

	TD_BOOST_CHECK_MESSAGE (_chh.load (header3.c_str()), "Error while loading header #3");
	TD_BOOST_CHECK_MESSAGE (_chh.m_httpCode == 407 &&
						 _chh.m_contentLength == 1372 &&
						 _chh.m_connection == 0 &&
						 _chh.m_proxyConnection == 1 &&
						 _chh.m_proxyAuthorizationMethods == auth2
						 , "Incorrect parsing header #3");

	string header4 = "\
HTTP/1.1 200 OK\r\n\
Server: Microsoft-IIS/5.1\r\n\
Connection: keep-alive\r\n\
Date: Mon, 10 Apr 2006 08:47:41 GMT\r\n\
Content-Type: application/octet-stream\r\n\
Accept-Ranges: bytes\r\n\
Last-Modified: Wed, 22 Mar 2006 03:40:00 GMT\r\n\
ETag: \"068c54b624dc61:a47\"\r\n\
Content-Length: 48305\
";
	TD_BOOST_CHECK_MESSAGE (_chh.load (header4.c_str()), "Error while loading header #4");
	TD_BOOST_CHECK_MESSAGE (_chh.m_httpCode == 200 &&
						 _chh.m_contentLength == 48305 &&
						 _chh.m_connection == 2 &&
						 _chh.m_proxyConnection == 0 &&
						 _chh.m_proxyAuthorizationMethods == vector<string>()
						 , "Incorrect parsing header #4");


	string header5 = "\
HTTP/1.1 404 Object Not Found\r\n\
Server: Microsoft-IIS/5.1\r\n\
Date: Mon, 10 Apr 2006 08:47:42 GMT\r\n\
Content-Length: 4040\r\n\
Content-Type: text/html\
";
	TD_BOOST_CHECK_MESSAGE (_chh.load (header5.c_str()), "Error while loading header #5");
	TD_BOOST_CHECK_MESSAGE (_chh.m_httpCode == 404 &&
						 _chh.m_contentLength == 4040 &&
						 _chh.m_connection == 0 &&
						 _chh.m_proxyConnection == 0 &&
						 _chh.m_proxyAuthorizationMethods == vector<string>()
						 , "Incorrect parsing header #5");

		string header6 = "\
HTertyregfh4 Objdhg Not Found\r\n\
Servedfghrcrosoft-IIS/5.1\r\n\
Date: Monrty0 Apr 2rtyrt8:47:42 GMT\r\n\
Contenfghength: 4040\r\n\
Content-Type: fhghdhtml\
";
	TD_BOOST_CHECK_MESSAGE (true == _chh.load (header6.c_str()), "Error while loading header #6");
	Step ("TestParsingHeader", reason);
};

/*
void Test_CHTTP::TestCHttpRequester ()
{
	std::cout<<"Testing CHTTPRequester";

	string line1 = "line1";
	string line2 = "line2";
	string line3 = "";
	string line4 = "line3\r\n";
	string crlf = "\r\n";

	CHTTPRequester _chr;
	
	//TD_BOOST_CHECK_MESSAGE (0 == _chr.GetLength(), "Incorrect request length");
	//TD_BOOST_CHECK_MESSAGE (string ("") == _chr.GetRequest(), "Incorrect request");

	TD_BOOST_CHECK_MESSAGE (true == _chr.AddLine (line1.c_str()), "Error while adding line #1");
	TD_BOOST_CHECK_MESSAGE (9 == _chr.GetLength(), "Incorrect request length #1");
	TD_BOOST_CHECK_MESSAGE (string (line1+crlf+crlf) == _chr.GetRequest(), "Incorrect request #1");

	TD_BOOST_CHECK_MESSAGE (true == _chr.AddLine (line2.c_str()), "Error while adding line #2");
	TD_BOOST_CHECK_MESSAGE (16 == _chr.GetLength(), "Incorrect request length #2");
	TD_BOOST_CHECK_MESSAGE (string (line1+crlf+line2+crlf+crlf) == _chr.GetRequest(), "Incorrect request #2");

	TD_BOOST_CHECK_MESSAGE (false == _chr.AddLine (line3.c_str()), "Error while adding line #3");
	TD_BOOST_CHECK_MESSAGE (16 == _chr.GetLength(), "Incorrect request length #3");
	TD_BOOST_CHECK_MESSAGE (string (line1+crlf+line2+crlf+crlf) == _chr.GetRequest(), "Incorrect request #3");

	TD_BOOST_CHECK_MESSAGE (true == _chr.AddLine (line4.c_str()), "Error while adding line #4");
	TD_BOOST_CHECK_MESSAGE (25 == _chr.GetLength(), "Incorrect request length #4");
	TD_BOOST_CHECK_MESSAGE (string (line1+crlf+line2+crlf+line4+crlf+crlf) == _chr.GetRequest(), "Incorrect request #4");

};
*/

Suite_CHTTP::Suite_CHTTP (const string& ini_file, const string& log_name)
{
	bool bLog;
	if (log_name == "") bLog =false; else bLog = true;

	boost::shared_ptr<Test_CHTTP> instance (new Test_CHTTP(ini_file, bLog, log_name));
	
	add (boost::unit_test::make_test_case<Test_CHTTP>(&Test_CHTTP::Start, "Test_CHTTP::Start", instance));
	add (BOOST_CLASS_TEST_CASE (&Test_CHTTP::TestGeneratingRequest, instance), 0);
	add (BOOST_CLASS_TEST_CASE (&Test_CHTTP::TestMakingAuthPhrase, instance), 0);
	add (BOOST_CLASS_TEST_CASE (&Test_CHTTP::TestParsingHeader, instance), 0);
	//add (BOOST_CLASS_TEST_CASE (&Test_CHTTP::TestCHttpRequester, instance));
	add (boost::unit_test::make_test_case<Test_CHTTP>(&Test_CHTTP::End, "Test_CHTTP::End", instance));
	
	
	

}
	