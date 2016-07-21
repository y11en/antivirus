// test.cpp : Defines the entry point for the console application.
//

#pragma warning( disable : 4786 )

#include <time.h>
#include <iostream>
#include <fstream>
#include <string>
#include "../imap/ImapProtocolAnalyzer.h"
#include "../imap/ImapProtocolAnalyzerImpl.h"
#include "../imap/ImapMessageDataParser.h"
#include "../nntp/NntpProtocolAnalyzer.h"
#include "../nntp/NntpProtocolAnalyzerImpl.h"
#include "../extra/ProtocolProcessor.h"
#include "../extra/DefaultContentHandler.h"
#include <utils/prague/prague.hpp>
#include "auxfunc.h"
#include "http_test_cases.h"
using namespace TrafficProtocoller;

//////////////////////////////////////////////////////////////////////////
class TestSessionProxy : public ISessionProxy
{
public:
	TestSessionProxy(std::ostream& os)
		: m_os(os)
	{
	}
	virtual void Send(data_source_t ds, const char* pData, size_t nDataSize)
	{
		using namespace TrafficProtocoller;
		m_os << (ds == dsServer ? "(S)" : "(C)") << "-----------------" << std::endl;
		m_os.write(pData, nDataSize);
		m_os << std::endl;
	}
private:
	std::ostream& m_os;
};
//////////////////////////////////////////////////////////////////////////
void print_help()
{
	using namespace std;
	cout 
		<< "test.exe <test name> <input file> <output file>" << endl 
		<< "\t<test name> ::= protocol_buffer | imap | nntp | http" << endl 
		<< endl;
}
//////////////////////////////////////////////////////////////////////////
template <typename AnalyzerImpl>
void test_analyzer_impl(std::istream& in_stream, std::ostream& out_stream, AnalyzerImpl* = 0)
{
	using namespace TrafficProtocoller;
	TestSessionProxy session_proxy(out_stream);
	ProtocolProcessor<AnalyzerImpl, DefaultContentHandler> processor(session_proxy);
	detect_code_t dc = dcNeedMoreInfo;

	while (!in_stream.eof() && dc != dcProtocolUnknown)
	{
		std::string sTemp;
		std::getline(in_stream, sTemp);
		if (!in_stream.fail() && !sTemp.empty())
		{
			data_source_t ds;
			std::string sPrefix = sTemp.substr(0, 3);
			std::string sCommand = sTemp.substr(3);
			if (sPrefix == "S> ")
				ds = dsServer;
			else if (sPrefix == "C> ")
				ds = dsClient;
			else
				break;
			sCommand += '\r';
			sCommand += '\n';
			if (dc == dcNeedMoreInfo || dc == dcProtocolDetected)
			{
				dc = processor.Detect(ds, sCommand.c_str(), sCommand.size());
				session_proxy.Send(ds, sCommand.c_str(), sCommand.size());
			}
			else if (dc == dcDataDetected)
			{
				processor.Process(ds, sCommand.c_str(), sCommand.size());
			}
		}
	}
}

//////////////////////////////////////////////////////////////////////////
void test_ImapMessageDataParser(std::istream& in_stream, std::ostream& out_stream)
{
	using namespace TrafficProtocoller;
	ImapUtil::ImapMessageDataParser parser;
	ContentItemsList content_items;

	while (!in_stream.eof())
	{
		std::string sTemp;
		std::getline(in_stream, sTemp);
		if (!in_stream.fail() && !sTemp.empty())
		{
			sTemp += "\r\n";
			parser.Parse(&sTemp[0], &sTemp[sTemp.size()], content_items);
		}
	}

	for (ContentItemsList::const_iterator it = content_items.begin();
			it != content_items.end();
			++it)
	{
		out_stream << std::endl << "===========================" << std::endl;
		ContentItemPtr p = *it;
		for (int i = 0; i < p->DataSize(); ++i)
			out_stream << p->DataElement(i);
		out_stream << std::endl << "===========================" << std::endl;
	}
}

//////////////////////////////////////////////////////////////////////////
void test_imap(std::istream& in_stream, std::ostream& out_stream)
{
	ImapProtocolAnalyzer analyzer;
}

//////////////////////////////////////////////////////////////////////////
void test_nntp(std::istream& in_stream, std::ostream& out_stream)
{
	NntpProtocolAnalyzer analyzer;
}
hROOT g_root;
tPROPID  g_propVirtualName = 0;
tPROPID  g_propMailerPID = 0;
tPROPID  g_propMessageIsIncoming = 0;
tPROPID  g_propMsgDirection = 0;
tPROPID  g_propStartScan = 0;
//////////////////////////////////////////////////////////////////////////
int main(int argc, char* argv[])
{
	using namespace std;

	if (argc < 4)
	{
		print_help();
		return 0;
	}

	static char const* plugins[] = {"nfio.ppl", "prutil.ppl", "tempfile.ppl", 0};
	prague::init_prague init(plugins);
	prague::pr_remote_api().GetRoot(&g_root);

	string sTest(argv[1]);
	string sInputFile(argv[2]);
	string sOutputFile(argv[3]);

	enum TestModeEnum
	{
		testUnknown,
		testProtocolBuffer,
		testImap,
		testImapParser,
		testNntp,
		testHttp
	};

	TestModeEnum test_mode = testUnknown;
	int input_file_mode = ios::in;

	if (sTest == "imap")
	{
		test_mode = testImap;
	}
	else if (sTest == "imap_parser")
	{
		test_mode = testImapParser;
	}
	else if (sTest == "nntp")
	{
		test_mode = testNntp;
	}
	else if (sTest == "http")
	{
		test_mode = testHttp;
	}

	if (test_mode == testUnknown)
	{
		cout << "Specified test is not implemented." << endl;
		return 0;
	}

	ifstream in_stream;
	ofstream out_stream;

	if(test_mode != testHttp)
	{
		in_stream.open(sInputFile.c_str(), input_file_mode);;
		if (in_stream.fail())
		{
			cout << "Can't open input file " << sInputFile << " for reading." << endl;
			return 0;
		}
		out_stream.open(sOutputFile.c_str(), ios::out | ios::trunc);
		if (out_stream.fail())
		{
			cout << "Can't open output file " << sOutputFile << " for writing." << endl;
			return 0;
		}
	}

	switch(test_mode)
	{
		case testImap:
			test_analyzer_impl<ImapProtocolAnalyzerImpl>(in_stream, out_stream);
			break;
		case testImapParser:
			test_ImapMessageDataParser(in_stream, out_stream);
		case testNntp:
			test_analyzer_impl<NntpProtocolAnalyzerImpl>(in_stream, out_stream);
			break;
		case testHttp:
			{
				typedef util::scoped_handle<char*, BOOL (WINAPI*)(void const*), &UnmapViewOfFile, 0, 0> map_mem_ptr;
				//1. считать все файлы создать список файлов
				std::auto_ptr<std::list<std::string> > list(aux::http_packets(sInputFile.c_str()));
				//2. сделать MMF куда поместить все данные
				std::pair<char*, size_t> stm(aux::create_http_stream(*list));
				map_mem_ptr mem(stm.first);
				//3. начать процесс обработки
				std::cout << "[1]\t - detect http traffic\n"
						  << "[2]\t - full packet traffic\n"
						  << "[3]\t - async full packet traffic\n"
						  << "[4]\t - parse chunk transfer endcoding\n"
						  << "[5]\t - crypt\\encrypt input file\n"
						  << "input http test number:";
				int num(0);
				std::cin >> num;
				bool test_result(false);
				size_t stm_size(stm.second);
				switch(num)
				{
				case 1:
					test_result = http::test_detect_traffic(mem.get(), mem.get() + stm_size);
					break;
				case 2:
					test_result = http::test_full_packet_traffic(mem.get(), mem.get() + stm_size, sOutputFile);
					break;
				case 3:
					test_result = http::test_async_full_packet_traffic(mem.get(), mem.get() + stm_size, sOutputFile);
					break;
				case 4:
					test_result = http::test_chunk_parser(mem.get(), mem.get() + stm_size);
				case 5:
					{
						std::string fname;
						std::cout << "input file name:";
						std::cin >> fname;
						http::xoring_file(fname, std::string("out.txt"));
						test_result = true;
					}
					break;
				case 6:
					{
						test_result = http::test_processor(mem.get(), mem.get() + stm_size);
					}
					break;
				case 7:
					{
						//1. считать все файлы создать список файлов
						std::auto_ptr<std::list<std::string> > list(aux::http_packets(sInputFile.c_str()));
						test_result = http::test_processor(*list);
					}
					break;
				default:
					std::cerr << "unknown test case\n";
				}
				std::cout << (test_result ? "TEST PASSED" : "TEST FAILED") << std::endl;
			}
			break;
	}
	return 0;
}

extern "C" BOOST_THREAD_DECL int on_thread_exit(void (__cdecl * func)(void))
{
	return 0;
}