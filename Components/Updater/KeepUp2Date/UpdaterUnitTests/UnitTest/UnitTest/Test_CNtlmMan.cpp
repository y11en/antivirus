#include "Test_CNtlmMan.h"
#include "helper.h"

Test_CNtlmMan::Test_CNtlmMan (const string& ini_file, bool log_enable, const string& log_name):
	  m_log (Ini_Log (log_enable, log_name.c_str())), BaseTest ("CNtlmMan")
{
	DWORD size;
	size = 64;
	GetUserName (user, &size);
	_strupr (user);
	size = 64;
	GetComputerName (host, &size);
	_strupr (host);
	strcpy (domen, "KL");
};

void Test_CNtlmMan::Test ()
{
	char short_req[10];
	char req[1024];

	string reason;
	std::cout<<"Testing normal use of CNtlmMan\n";

	string request = "TlRMTVNTUAABAAAAB7IIogIAAgA0AAAADAAMACgAAAAFASgKAAAAD0tJUllVS0hJTi1YUEtM";
	string serverToken = "TlRMTVNTUAACAAAAAAAAADAAAAAyAgAAxDzAtKJdLpIAAAAAAAAAAAAAAAAwAAAA";
	string response = "TlRMTVNTUAADAAAAGAAYAF8AAAAYABgAdwAAAAIAAgBIAAAACQAJAEoAAAAMAAwAUwAAAAAAAACPAAAABgIAAgUBKAoAAAAPS0xLSVJZVUtISU5LSVJZVUtISU4tWFCiDCrzVKp0Y/OYzZEHd1eZHNfwxyy5HoX5m9Zs4WZAQMsQio6ETI3o7cammYvosqU=";
	{
		CNtlmMan _cnm;
		_cnm.Init (&m_log);;
		TD_BOOST_CHECK_MESSAGE (_cnm.Make_InitialRequest (req, 1024, &m_log), "Making InitialRequest");
		Type1Message_Unit output_u (host, domen);
		Type1Message output;
		TD_BOOST_CHECK_MESSAGE (0 != CBase64().DecodeBuffer(req, reinterpret_cast<char *>(&output)), "Incorrect InitialRequest");
		TD_BOOST_CHECK_MESSAGE (output_u == output, "Incorrect InitialRequest");
		
		Type3Message_Unit output_u3 (host, domen, user);
		Type3Message output3;
		TD_BOOST_CHECK_MESSAGE (_cnm.Make_Response (serverToken.c_str(), req, 1024, &m_log), "Making Response");
		TD_BOOST_CHECK_MESSAGE (0 != CBase64().DecodeBuffer(req, reinterpret_cast<char *>(&output3)), "Incorrect Response");
		TD_BOOST_CHECK_MESSAGE (output_u3 == output3, "Incorrect Response");
		
	};

	std::cout<<"Testing short buffers\n";
	CNtlmMan _cnm;
	TD_BOOST_CHECK_MESSAGE (false == _cnm.Make_InitialRequest (short_req, 10, &m_log), "Making InitialRequest");
	TD_BOOST_CHECK_MESSAGE (false == _cnm.Make_Response (serverToken.c_str(), short_req, 10, &m_log), "Making Response");

	std::cout<<"Testing wrong serverToken\n";
	TD_BOOST_CHECK_MESSAGE (false == _cnm.Make_Response ("trulala", req, 1024, &m_log), "Making Response");

	Step ("Test", reason);		
};


Suite_CNtlmMan::Suite_CNtlmMan (const string& ini_file, const string& log_name)
{
	bool bLog;
	if (log_name == "") bLog =false; else bLog = true;

	boost::shared_ptr<Test_CNtlmMan> instance (new Test_CNtlmMan(ini_file, bLog, log_name));
	
	add (boost::unit_test::create_test_case<Test_CNtlmMan>(&Test_CNtlmMan::Start, "Test_CNtlmMan::Start", instance));
	add (BOOST_CLASS_TEST_CASE (&Test_CNtlmMan::Test, instance), 0);
	add (boost::unit_test::create_test_case<Test_CNtlmMan>(&Test_CNtlmMan::End, "Test_CNtlmMan::End", instance));
	

	
	

}
	