#include "util.h"

#include "log.h"
#include "callback.h"
#include "journal.h"
#include "core/updater.h"
#include "helper/local_file.h"
#include "helper/IndexFileXMLParser.h"

#include "TestSuiteProxyDetector.h"
#include "boost.h"
#include "main.h"

using namespace KLUPD;
using namespace std;
using namespace ProxyDetectorNamespace;

void TestSuiteProxyDetector::TestWPAD ()
{
	using namespace KLUPD;
	std::wcout<<L"TestWPAD\n";
	Logger::Case _case (m_logger, m_testp, m_step, L"TestWPAD");

	//symbolic
	{		
		ProxyJournal journal ( std::wstring(COLL_ROOT).append(L"\\wpads\\test_wpad.dat") );
		CDownloadProgress dp;
		ProxyDetector pd (60, dp, journal, &testLog);
		journal.SetProxyDetector(&pd);
		Address proxyAddress;
		//in_addr ia;
		ProxyServerMode mode = pd.getProxyAddress (proxyAddress, L"http://tl-srv-wxp:33/base101.avc"/*, ia,  false*/);
			
		QC_BOOST_CHECK_MESSAGE ( mode == withProxy, L"Incorrect proxy mode\n" );
		QC_BOOST_CHECK_MESSAGE (proxyAddress.m_hostname == L"1.1.1.1" && proxyAddress.m_service == L"2222",
			L"Incorrect proxy\n");
	}
	
	{		
		ProxyJournal journal ( std::wstring(COLL_ROOT).append(L"\\wpads\\test_wpad.dat") );
		CDownloadProgress dp;
		ProxyDetector pd (60, dp, journal, &testLog);
		journal.SetProxyDetector(&pd);
		Address proxyAddress;
		//in_addr ia;
		ProxyServerMode mode = pd.getProxyAddress (proxyAddress, L"http://tl-srv-wxp/base102.avc"/*, ia,  true*/);
			
		QC_BOOST_CHECK_MESSAGE ( mode == withProxy, L"Incorrect proxy mode\n" );
		QC_BOOST_CHECK_MESSAGE (proxyAddress.m_hostname == L"1.1.1.2" && proxyAddress.m_service == L"2223",
			L"Incorrect proxy\n");
	}
	{		
		ProxyJournal journal ( std::wstring(COLL_ROOT).append(L"\\wpads\\test_wpad.dat") );			
		CDownloadProgress dp;
		ProxyDetector pd (60, dp, journal, &testLog);
		journal.SetProxyDetector(&pd);
		Address proxyAddress;
		//in_addr ia;
		QC_BOOST_CHECK_MESSAGE (/*withProxy*/directConnection == pd.getProxyAddress (proxyAddress, L"downloads1.kaspersky-labs.com"/*, ia,  false*/),
			L"Incorrect proxy mode\n");
		/*QC_BOOST_CHECK_MESSAGE (proxyAddress.m_hostname == L"1.1.1.3" && proxyAddress.m_portNumber == 2224,
			L"Incorrect proxy\n");*/
	}
	{		
		ProxyJournal journal ( std::wstring(COLL_ROOT).append(L"\\wpads\\test_wpad.dat") );			
		CDownloadProgress dp;
		ProxyDetector pd (60, dp, journal, &testLog);
		journal.SetProxyDetector(&pd);
		Address proxyAddress;
		//in_addr ia;
		QC_BOOST_CHECK_MESSAGE (withProxy == pd.getProxyAddress (proxyAddress, L"ftp://downloads1.kaspersky-labs.com:3031/fake.avc"/*, ia,  true*/),
			L"Incorrect proxy mode\n");
		QC_BOOST_CHECK_MESSAGE (proxyAddress.m_hostname == L"1.1.1.4" && proxyAddress.m_service == L"2225",
			L"Incorrect proxy\n");
	}

	//ip-s
	{		
		ProxyJournal journal ( std::wstring(COLL_ROOT).append(L"\\wpads\\test_wpad.dat") );			
		CDownloadProgress dp;
		ProxyDetector pd (60, dp, journal, &testLog);
		journal.SetProxyDetector(&pd);
		Address proxyAddress;
		//in_addr ia;
		QC_BOOST_CHECK_MESSAGE (withProxy == pd.getProxyAddress (proxyAddress, L"http://172.16.10.177/base101.avc"/*, ia,  false*/),
			L"Incorrect proxy mode\n");
		QC_BOOST_CHECK_MESSAGE (proxyAddress.m_hostname == L"2.2.2.2" && proxyAddress.m_service == L"1111",
			L"Incorrect proxy\n");
	}
	{		
		ProxyJournal journal ( std::wstring(COLL_ROOT).append(L"\\wpads\\test_wpad.dat") );			
		CDownloadProgress dp;
		ProxyDetector pd (60, dp, journal, &testLog);
		journal.SetProxyDetector(&pd);
		Address proxyAddress;
		//in_addr ia;
		QC_BOOST_CHECK_MESSAGE (withProxy == pd.getProxyAddress (proxyAddress, L"http://172.16.10.177/base101.avc"/*, ia,  true*/),
			L"Incorrect proxy mode\n");
		QC_BOOST_CHECK_MESSAGE (proxyAddress.m_hostname == L"2.2.2.2" && proxyAddress.m_service == L"1111",
			L"Incorrect proxy\n");
	}
		
	{		
		ProxyJournal journal ( std::wstring(COLL_ROOT).append(L"\\wpads\\test_wpad.dat") );
		CDownloadProgress dp;
		ProxyDetector pd (60, dp, journal, &testLog);
		journal.SetProxyDetector(&pd);
		Address proxyAddress;
		//in_addr ia;
		QC_BOOST_CHECK_MESSAGE (withProxy == pd.getProxyAddress (proxyAddress, L"ftp://85.12.58.3:3031/fake.avc"/*, ia,  true*/),
			L"Incorrect proxy mode\n");
		QC_BOOST_CHECK_MESSAGE (proxyAddress.m_hostname == L"2.2.2.2" && proxyAddress.m_service == L"1112",
			L"Incorrect proxy\n");
	}
	{		
		ProxyJournal journal ( std::wstring(COLL_ROOT).append(L"\\wpads\\test_wpad.dat") );
		CDownloadProgress dp;
		ProxyDetector pd (60, dp, journal, &testLog);
		journal.SetProxyDetector(&pd);
		Address proxyAddress;
		//in_addr ia;
		QC_BOOST_CHECK_MESSAGE (directConnection == pd.getProxyAddress (proxyAddress, L"3.3.3.3"/*, ia,  true*/),
			L"Incorrect proxy mode\n");
		//QC_BOOST_CHECK_MESSAGE (proxyAddress.m_hostname == L"2.2.2.2" && proxyAddress.m_portNumber == 1112,
		//	L"Incorrect proxy\n");
	}
	{	//bypass, although wpad tells proxy	
		ProxyJournal journal ( std::wstring(COLL_ROOT).append(L"\\wpads\\test_wpad.dat") );
		CDownloadProgress dp;
		ProxyDetector pd (60, dp, journal, &testLog);
		journal.SetProxyDetector(&pd);
		Address proxyAddress;
		//in_addr ia;
		QC_BOOST_CHECK_MESSAGE (directConnection == pd.getProxyAddress (proxyAddress, L"tlserver"/*, ia,  true*/),
			L"Incorrect proxy mode\n");
		//QC_BOOST_CHECK_MESSAGE (proxyAddress.m_hostname == L"2.2.2.2" && proxyAddress.m_portNumber == 1112,
		//	L"Incorrect proxy\n");
	}
}


void TestSuiteProxyDetector::TestWPADCorrupted ()
{
	using namespace KLUPD;
	std::wcout<<L"TestWPADCorrupted\n";
	Logger::Case _case (m_logger, m_testp, m_step, L"TestWPADCorrupted");

	//symbolic
	{		
		ProxyJournal journal ( std::wstring(COLL_ROOT).append(L"\\wpads\\test_wpad_corrupted.dat") );
		CDownloadProgress dp;
		ProxyDetector pd (60, dp, journal, &testLog);
		journal.SetProxyDetector(&pd);
		Address proxyAddress;
		//in_addr ia;
		ProxyServerMode mode = pd.getProxyAddress (proxyAddress, L"http://tl-srv-wxp:33/base101.avc"/*, ia,  false*/);
			
		QC_BOOST_CHECK_MESSAGE ( mode == directConnection, L"Incorrect proxy mode\n" );
	}
}


void TestSuiteProxyDetector::TestWPAD_IpV6 ()
{
	using namespace KLUPD;
	std::wcout<<L"TestWPAD_IpV6\n";
	Logger::Case _case (m_logger, m_testp, m_step, L"TestWPAD_IpV6");

	//symbolic
	{		
		ProxyJournal journal ( std::wstring(COLL_ROOT).append(L"\\wpads\\test_wpad_ipv6.dat") );
		CDownloadProgress dp;
		ProxyDetector pd (60, dp, journal, &testLog);
		journal.SetProxyDetector(&pd);
		Address proxyAddress;
		//in_addr ia;
		ProxyServerMode mode = pd.getProxyAddress (proxyAddress, L"http://tl-srv-wxp:33/base101.avc"/*, ia,  false*/);
  
		QC_BOOST_CHECK_MESSAGE ( mode == withProxy, L"Incorrect proxy mode\n" );
		QC_BOOST_CHECK_MESSAGE (proxyAddress.m_hostname == L"[FEDC:BA98:7654:3210:FEDC:BA98:7654:3210]" && proxyAddress.m_service == L"2222",
			L"Incorrect proxy\n");
	}

	{		
		ProxyJournal journal ( std::wstring(COLL_ROOT).append(L"\\wpads\\test_wpad_ipv6.dat") );
		CDownloadProgress dp;
		ProxyDetector pd (60, dp, journal, &testLog);
		journal.SetProxyDetector(&pd);
		Address proxyAddress;
		//in_addr ia;
		ProxyServerMode mode = pd.getProxyAddress (proxyAddress, L"http://tl-srv-wxp/base102.avc"/*, ia,  true*/);

		QC_BOOST_CHECK_MESSAGE ( mode == withProxy, L"Incorrect proxy mode\n" );
		QC_BOOST_CHECK_MESSAGE (proxyAddress.m_hostname == L"[1080:0:0:0:8:800:200C:4171]" && proxyAddress.m_service == L"2223",
			L"Incorrect proxy\n");
	}
	{		
		ProxyJournal journal ( std::wstring(COLL_ROOT).append(L"\\wpads\\test_wpad_ipv6.dat") );			
		CDownloadProgress dp;
		ProxyDetector pd (60, dp, journal, &testLog);
		journal.SetProxyDetector(&pd);
		Address proxyAddress;
		//in_addr ia;
		QC_BOOST_CHECK_MESSAGE (/*withProxy*/directConnection == pd.getProxyAddress (proxyAddress, L"downloads1.kaspersky-labs.com"/*, ia,  false*/),
			L"Incorrect proxy mode\n");
		/*QC_BOOST_CHECK_MESSAGE (proxyAddress.m_hostname == L"1.1.1.3" && proxyAddress.m_portNumber == 2224,
		L"Incorrect proxy\n");*/
	}
	{		
		ProxyJournal journal ( std::wstring(COLL_ROOT).append(L"\\wpads\\test_wpad_ipv6.dat") );			
		CDownloadProgress dp;
		ProxyDetector pd (60, dp, journal, &testLog);
		journal.SetProxyDetector(&pd);
		Address proxyAddress;
		//in_addr ia;
		QC_BOOST_CHECK_MESSAGE (withProxy == pd.getProxyAddress (proxyAddress, L"ftp://downloads1.kaspersky-labs.com:3031/fake.avc"/*, ia,  true*/),
			L"Incorrect proxy mode\n");
		QC_BOOST_CHECK_MESSAGE (proxyAddress.m_hostname == L"[3ffe:2a00:100:7031::1]" && proxyAddress.m_service == L"2225",
			L"Incorrect proxy\n");
	}

	//ip-s
	{		
		ProxyJournal journal ( std::wstring(COLL_ROOT).append(L"\\wpads\\test_wpad_ipv6.dat") );			
		CDownloadProgress dp;
		ProxyDetector pd (60, dp, journal, &testLog);
		journal.SetProxyDetector(&pd);
		Address proxyAddress;
		//in_addr ia;
		QC_BOOST_CHECK_MESSAGE (withProxy == pd.getProxyAddress (proxyAddress, L"http://172.16.10.177/base101.avc"/*, ia,  false*/),
			L"Incorrect proxy mode\n");
		QC_BOOST_CHECK_MESSAGE (proxyAddress.m_hostname == L"[::192.9.5.5]" && proxyAddress.m_service == L"1111",
			L"Incorrect proxy\n");
	}
	{		
		ProxyJournal journal ( std::wstring(COLL_ROOT).append(L"\\wpads\\test_wpad_ipv6.dat") );			
		CDownloadProgress dp;
		ProxyDetector pd (60, dp, journal, &testLog);
		journal.SetProxyDetector(&pd);
		Address proxyAddress;
		//in_addr ia;
		QC_BOOST_CHECK_MESSAGE (withProxy == pd.getProxyAddress (proxyAddress, L"http://172.16.10.177/base101.avc"/*, ia,  true*/),
			L"Incorrect proxy mode\n");
		QC_BOOST_CHECK_MESSAGE (proxyAddress.m_hostname == L"[::192.9.5.5]" && proxyAddress.m_service == L"1111",
			L"Incorrect proxy\n");
	}

	{		
		ProxyJournal journal ( std::wstring(COLL_ROOT).append(L"\\wpads\\test_wpad_ipv6.dat") );
		CDownloadProgress dp;
		ProxyDetector pd (60, dp, journal, &testLog);
		journal.SetProxyDetector(&pd);
		Address proxyAddress;
		//in_addr ia;
		QC_BOOST_CHECK_MESSAGE (withProxy == pd.getProxyAddress (proxyAddress, L"ftp://85.12.58.3:3031/fake.avc"/*, ia,  true*/),
			L"Incorrect proxy mode\n");
		QC_BOOST_CHECK_MESSAGE (proxyAddress.m_hostname == L"[::192.9.5.5]" && proxyAddress.m_service == L"1112",
			L"Incorrect proxy\n");
	}
	{		
		ProxyJournal journal ( std::wstring(COLL_ROOT).append(L"\\wpads\\test_wpad_ipv6.dat") );
		CDownloadProgress dp;
		ProxyDetector pd (60, dp, journal, &testLog);
		journal.SetProxyDetector(&pd);
		Address proxyAddress;
		//in_addr ia;
		QC_BOOST_CHECK_MESSAGE (directConnection == pd.getProxyAddress (proxyAddress, L"3.3.3.3"/*, ia,  true*/),
			L"Incorrect proxy mode\n");
		//QC_BOOST_CHECK_MESSAGE (proxyAddress.m_hostname == L"::192.9.5.5" && proxyAddress.m_portNumber == 1112,
		//	L"Incorrect proxy\n");
	}
	{	//bypass, although wpad tells proxy	
		ProxyJournal journal ( std::wstring(COLL_ROOT).append(L"\\wpads\\test_wpad_ipv6.dat") );
		CDownloadProgress dp;
		ProxyDetector pd (60, dp, journal, &testLog);
		journal.SetProxyDetector(&pd);
		Address proxyAddress;
		//in_addr ia;
		QC_BOOST_CHECK_MESSAGE (directConnection == pd.getProxyAddress (proxyAddress, L"tlserver"/*, ia,  true*/),
			L"Incorrect proxy mode\n");
		//QC_BOOST_CHECK_MESSAGE (proxyAddress.m_hostname == L"::192.9.5.5" && proxyAddress.m_portNumber == 1112,
		//	L"Incorrect proxy\n");
	}
}
