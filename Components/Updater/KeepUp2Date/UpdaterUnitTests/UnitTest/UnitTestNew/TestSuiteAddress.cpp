#include "util.h"

#include "log.h"
#include "callback.h"
#include "journal.h"

#include "TestSuite.h"
#include "TestSuiteAddress.h"
#include "main.h"

void TestSuiteAddress::TestAddress ()
{
	using namespace KLUPD;
	std::wcout<<L"TestAddress\n";
	Logger::Case _case (m_logger, m_testp, m_step, L"TestAddress");

	Address source0(L"");
	QC_BOOST_CHECK_MESSAGE (source0.m_protocol == fileTransport &&
							source0.m_service ==L"" &&
							source0.m_path == L"" &&
							//source0.m_originalUrlToParse == L"" &&
							source0.m_hostname == L"" &&
							source0.m_fileName == L"" &&
							source0.m_credentials.userName() == L"" &&
							source0.m_credentials.password() == L"",
																	L"Incorrect parsing 0\n");

    Address source1(L"ftp://");
	QC_BOOST_CHECK_MESSAGE (source1.m_protocol == ftpTransport &&
							source1.m_service == L"21" &&
							source1.m_path == L"/" &&
							//source1.m_originalUrlToParse == L"ftp://" &&
							source1.m_hostname == L"" &&
							source1.m_fileName == L"" &&
							source1.m_credentials.userName() == L"" &&
							source1.m_credentials.password() == L"",
																	L"Incorrect parsing 1\n");

	Address source2(L"ftp://host");
	QC_BOOST_CHECK_MESSAGE (source2.m_protocol == ftpTransport &&
							source2.m_service == L"21" &&
							source2.m_path == L"/" &&
							//source2.m_originalUrlToParse == L"ftp://host" &&
							source2.m_hostname == L"host" &&
							source2.m_fileName == L"" &&
							source2.m_credentials.userName() == L"" &&
							source2.m_credentials.password() == L"",
																	L"Incorrect parsing 2\n");

    Address source3(L"ftp://host/");
	QC_BOOST_CHECK_MESSAGE (source3.m_protocol == ftpTransport &&
							source3.m_service == L"21" &&
							source3.m_path == L"/" &&
							//source3.m_originalUrlToParse == L"ftp://host/" &&
							source3.m_hostname == L"host" &&
							source3.m_fileName == L"" &&
							source3.m_credentials.userName() == L"" &&
							source3.m_credentials.password() == L"",
																	L"Incorrect parsing 3\n");

    Address source4(L"ftp://host/file.txt");
	QC_BOOST_CHECK_MESSAGE (source4.m_protocol == ftpTransport &&
							source4.m_service == L"21" &&
							source4.m_path == L"/" &&
							//source4.m_originalUrlToParse == L"ftp://host/file.txt" &&
							source4.m_hostname == L"host" &&
							source4.m_fileName == L"file.txt" &&
							source4.m_credentials.userName() == L"" &&
							source4.m_credentials.password() == L"",
																	L"Incorrect parsing 4\n");

    Address source5(L"ftp://host/folder/");
	QC_BOOST_CHECK_MESSAGE (source5.m_protocol == ftpTransport &&
							source5.m_service == L"21" &&
							source5.m_path == L"/folder/" &&
							//source5.m_originalUrlToParse == L"ftp://host/folder/" &&
							source5.m_hostname == L"host" &&
							source5.m_fileName == L"" &&
							source5.m_credentials.userName() == L"" &&
							source5.m_credentials.password() == L"",
																	L"Incorrect parsing 5\n");

    Address source6(L"ftp://host/folder/subFolder/");
	QC_BOOST_CHECK_MESSAGE (source6.m_protocol == ftpTransport &&
							source6.m_service == L"21" &&
							source6.m_path == L"/folder/subFolder/" &&
							//source6.m_originalUrlToParse == L"ftp://host/folder/subFolder/" &&
							source6.m_hostname == L"host" &&
							source6.m_fileName == L"" &&
							source6.m_credentials.userName() == L"" &&
							source6.m_credentials.password() == L"",
																	L"Incorrect parsing 6\n");

	Address source7(L"ftp://host:23/folder/subFolder/file.txt");
	QC_BOOST_CHECK_MESSAGE (source7.m_protocol == ftpTransport &&
							source7.m_service == L"23" &&
							source7.m_path == L"/folder/subFolder/" &&
							//source7.m_originalUrlToParse == L"ftp://host:23/folder/subFolder/file.txt" &&
							source7.m_hostname == L"host" &&
							source7.m_fileName == L"file.txt" &&
							source7.m_credentials.userName() == L"" &&
							source7.m_credentials.password() == L"",
																	L"Incorrect parsing 7\n");

	Address source8(L"ftp://usr:pwd@host:23/folder/subFolder/file.txt");
	QC_BOOST_CHECK_MESSAGE (source8.m_protocol == ftpTransport &&
							source8.m_service == L"23" &&
							source8.m_path == L"/folder/subFolder/" &&
							//source8.m_originalUrlToParse == L"ftp://usr:pwd@host:23/folder/subFolder/file.txt" &&
							source8.m_hostname == L"host" &&
							source8.m_fileName == L"file.txt" &&
							source8.m_credentials.userName() == L"usr" &&
							source8.m_credentials.password() == L"pwd",
																	L"Incorrect parsing 8\n");
	
	Address source9(L"http://host:81/folder/subFolder/file.txt");
	QC_BOOST_CHECK_MESSAGE (source9.m_protocol == httpTransport &&
							source9.m_service == L"81" &&
							source9.m_path == L"/folder/subFolder/" &&
							//source9.m_originalUrlToParse == L"http://host:81/folder/subFolder/file.txt" &&
							source9.m_hostname == L"host" &&
							source9.m_fileName == L"file.txt" &&
							source9.m_credentials.userName() == L"" &&
							source9.m_credentials.password() == L"",
																	L"Incorrect parsing 9\n");

	Address source10(L"\\\\host\\folder\\subFolder\\file.txt");
	QC_BOOST_CHECK_MESSAGE (source10.m_protocol == fileTransport &&
							source10.m_service == L"" &&
							source10.m_path == L"\\\\host\\folder\\subFolder\\file.txt" &&
							//source10.m_originalUrlToParse == L"\\\\host\\folder\\subFolder\\file.txt" &&
							source10.m_hostname == L"" &&
							source10.m_fileName == L"" &&
							source10.m_credentials.userName() == L"" &&
							source10.m_credentials.password() == L"",
																	L"Incorrect parsing 10\n");
	Address source11(L"ftp://usr:@host:23/folder/subFolder/file.txt");
	QC_BOOST_CHECK_MESSAGE (source11.m_protocol == ftpTransport &&
							source11.m_service == L"23" &&
							source11.m_path == L"/folder/subFolder/" &&
							//source11.m_originalUrlToParse == L"ftp://usr:@host:23/folder/subFolder/file.txt" &&
							source11.m_hostname == L"host" &&
							source11.m_fileName == L"file.txt" &&
							source11.m_credentials.userName() == L"usr" &&
							source11.m_credentials.password() == L"",
																	L"Incorrect parsing 11\n");

	Address source12(L"ftp://:pwd@host:23/folder/subFolder/file.txt");
	QC_BOOST_CHECK_MESSAGE (source12.m_protocol == ftpTransport &&
							source12.m_service == L"23" &&
							source12.m_path == L"/folder/subFolder/" &&
							//source12.m_originalUrlToParse == L"ftp://:pwd@host:23/folder/subFolder/file.txt" &&
							source12.m_hostname == L"host" &&
							source12.m_fileName == L"file.txt" &&
							source12.m_credentials.userName() == L"" &&
							source12.m_credentials.password() == L"pwd",
																	L"Incorrect parsing 12\n");
	
	Address source13(L"ftp://:@host:23/folder/subFolder/file.txt");
	QC_BOOST_CHECK_MESSAGE (source13.m_protocol == ftpTransport &&
							source13.m_service == L"23" &&
							source13.m_path == L"/folder/subFolder/" &&
							//source13.m_originalUrlToParse == L"ftp://:@host:23/folder/subFolder/file.txt" &&
							source13.m_hostname == L"host" &&
							source13.m_fileName == L"file.txt" &&
							source13.m_credentials.userName() == L"" &&
							source13.m_credentials.password() == L"",
																	L"Incorrect parsing 13\n");

	Address source14(L"ftp://:@host/folder/subFolder/file.txt");
	QC_BOOST_CHECK_MESSAGE (source14.m_protocol == ftpTransport &&
							source14.m_service == L"21" &&
							source14.m_path == L"/folder/subFolder/" &&
							//source14.m_originalUrlToParse == L"ftp://:@host/folder/subFolder/file.txt" &&
							source14.m_hostname == L"host" &&
							source14.m_fileName == L"file.txt" &&
							source14.m_credentials.userName() == L"" &&
							source14.m_credentials.password() == L"",
																	L"Incorrect parsing 14\n");

	Address source15(L"ftp://1:2@host/folder/subFolder/");
	QC_BOOST_CHECK_MESSAGE (source15.m_protocol == ftpTransport &&
							source15.m_service == L"21" &&
							source15.m_path == L"/folder/subFolder/" &&
							//source15.m_originalUrlToParse == L"ftp://1:2@host/folder/subFolder/" &&
							source15.m_hostname == L"host" &&
							source15.m_fileName == L"" &&
							source15.m_credentials.userName() == L"1" &&
							source15.m_credentials.password() == L"2",
																	L"Incorrect parsing 15\n");

	Address source16(L"ftp://1:2@host/file");
	QC_BOOST_CHECK_MESSAGE (source16.m_protocol == ftpTransport &&
							source16.m_service == L"21" &&
							source16.m_path == L"/" &&
							//source16.m_originalUrlToParse == L"ftp://1:2@host/file" &&
							source16.m_hostname == L"host" &&
							source16.m_fileName == L"file" &&
							source16.m_credentials.userName() == L"1" &&
							source16.m_credentials.password() == L"2",
																	L"Incorrect parsing 16\n");

	Address source17(L"ftp://usr:pwd@1.2.3.4:23/folder/subFolder/file.txt");
	QC_BOOST_CHECK_MESSAGE (source17.m_protocol == ftpTransport &&
		source17.m_service == L"23" &&
		source17.m_path == L"/folder/subFolder/" &&
		//source13.m_originalUrlToParse == L"ftp://:@host:23/folder/subFolder/file.txt" &&
		source17.m_hostname == L"1.2.3.4" &&
		source17.m_fileName == L"file.txt" &&
		source17.m_credentials.userName() == L"usr" &&
		source17.m_credentials.password() == L"pwd",
		L"Incorrect parsing 17\n");
}

void TestSuiteAddress::TestAddress_IpV6 ()
{
	using namespace KLUPD;
	std::wcout<<L"TestAddress_IpV6\n";
	Logger::Case _case (m_logger, m_testp, m_step, L"TestAddress_IpV6");
	

	Address source0(L"http://[FEDC:BA98:7654:3210:FEDC:BA98:7654:3210]:82/index.html");
	QC_BOOST_CHECK_MESSAGE (source0.m_protocol == httpTransport &&
		source0.m_service == L"82" &&
		source0.m_path == L"/" &&
		source0.m_hostname == L"[FEDC:BA98:7654:3210:FEDC:BA98:7654:3210]" &&
		source0.m_fileName == L"index.html" &&
		source0.m_credentials.userName() == L"" &&
		source0.m_credentials.password() == L"",
		L"Incorrect parsing 0\n");

	Address source1(L"http://[::FFFF:129.144.52.38]/sub/index.html");
	QC_BOOST_CHECK_MESSAGE (source1.m_protocol == httpTransport &&
		source1.m_service == L"80" &&
		source1.m_path == L"/sub/" &&
		source1.m_hostname == L"[::FFFF:129.144.52.38]" &&
		source1.m_fileName == L"index.html" &&
		source1.m_credentials.userName() == L"" &&
		source1.m_credentials.password() == L"",
		L"Incorrect parsing 1\n");

	Address source2(L"ftp://usr:pwd@[::FFFF:129.144.52.38]:25/sub/index.html");
	QC_BOOST_CHECK_MESSAGE (source2.m_protocol == ftpTransport &&
		source2.m_service == L"25" &&
		source2.m_path == L"/sub/" &&
		source2.m_hostname == L"[::FFFF:129.144.52.38]" &&
		source2.m_fileName == L"index.html" &&
		source2.m_credentials.userName() == L"usr" &&
		source2.m_credentials.password() == L"pwd",
		L"Incorrect parsing 2\n");

	Address source3(L"ftp://usr:@[::FFFF:129.144.52.38]:25/sub/index.html");
	QC_BOOST_CHECK_MESSAGE (source3.m_protocol == ftpTransport &&
		source3.m_service == L"25" &&
		source3.m_path == L"/sub/" &&
		source3.m_hostname == L"[::FFFF:129.144.52.38]" &&
		source3.m_fileName == L"index.html" &&
		source3.m_credentials.userName() == L"usr" &&
		source3.m_credentials.password() == L"",
		L"Incorrect parsing 3\n");

	Address source4(L"ftp://:pwd@[::FFFF:129.144.52.38]:25/sub/index.html");
	QC_BOOST_CHECK_MESSAGE (source4.m_protocol == ftpTransport &&
		source4.m_service == L"25" &&
		source4.m_path == L"/sub/" &&
		source4.m_hostname == L"[::FFFF:129.144.52.38]" &&
		source4.m_fileName == L"index.html" &&
		source4.m_credentials.userName() == L"" &&
		source4.m_credentials.password() == L"pwd",
		L"Incorrect parsing 4\n");
 
	Address source5(L"ftp://:@[::FFFF:129.144.52.38]:25/sub/index.html");
	QC_BOOST_CHECK_MESSAGE (source5.m_protocol == ftpTransport &&
		source5.m_service == L"25" &&
		source5.m_path == L"/sub/" &&
		source5.m_hostname == L"[::FFFF:129.144.52.38]" &&
		source5.m_fileName == L"index.html" &&
		source5.m_credentials.userName() == L"" &&
		source5.m_credentials.password() == L"",
		L"Incorrect parsing 5\n");

	Address source6(L"\\\\3ffe:2a00:100:7031::1\\folder\\subFolder\\file.txt");
	QC_BOOST_CHECK_MESSAGE (source6.m_protocol == fileTransport &&
		source6.m_service == L"" &&
		source6.m_path == L"\\\\3ffe:2a00:100:7031::1\\folder\\subFolder\\file.txt" &&
		source6.m_hostname == L"" &&
		source6.m_fileName == L"" &&
		source6.m_credentials.userName() == L"" &&
		source6.m_credentials.password() == L"",
		L"Incorrect parsing 6\n");

	Address source7(L"ftp://[1080:0:0:0:8:800:200C:4171]");
	QC_BOOST_CHECK_MESSAGE (source7.m_protocol == ftpTransport &&
		source7.m_service == L"21" &&
		source7.m_path == L"/" &&
		source7.m_hostname == L"[1080:0:0:0:8:800:200C:4171]" &&
		source7.m_fileName == L"" &&
		source7.m_credentials.userName() == L"" &&
		source7.m_credentials.password() == L"",
		L"Incorrect parsing 7\n");
}
