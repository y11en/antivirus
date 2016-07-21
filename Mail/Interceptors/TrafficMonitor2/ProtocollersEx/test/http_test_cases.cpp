/*!
*		
*		
*		(C) 2003 Kaspersky Lab 
*		
*		\file	http_test_cases.cpp
*		\brief	реализация тестовых сценариев
*		
*		\author Владислав Овчарик
*		\date	27.04.2005 12:03:10
*		
*		
*/		
#define NOMINMAX
#pragma warning(disable : 4275)
#include <windows.h>
#include <prague.h>
#include <http/analyzerimpl.h>
#include <http/chunk.hpp>
#include <iostream>
#include <fstream>
#include <cstdlib>
#include <time.h>
#include <process.h>
#include "http_test_cases.h"
#include "../httpprotocoller/xor.hpp"
#include <utils/prague/io.hpp>
#include "../httpprotocoller/processor.h"
#include "../httpprotocoller/session.h"
#include "../httpprotocoller/cached_strategy.h"
#include "../httpprotocoller/stream_strategy.h"
#include "auxfunc.h"
///////////////////////////////////////////////////////////////////////////////
namespace
{
///////////////////////////////////////////////////////////////////////////////
__int64 const xor_key = 0xBC5C0E69EC4845E3;
///////////////////////////////////////////////////////////////////////////////
size_t rnd_chunk_size()
{
	size_t size(rand() % 0x403);
	return size < 0x80 ? 0x7f : size;
	//return 0x10000;
}
///////////////////////////////////////////////////////////////////////////////
typedef util::static_array<char const*> media_types;
///////////////////////////////////////////////////////////////////////////////
inline util::static_array<char const*> get_media_types()
{
	static char const* cnttype[] = 
	{
		"application",
		"audio",
		"image",
		"message",
		"model",
		"multipart",
		"text",
		"video"
	};
	return util::static_array<char const*>(cnttype, sizeof(cnttype) / sizeof(char const*));
}
///////////////////////////////////////////////////////////////////////////////
}//unnamed namespace
///////////////////////////////////////////////////////////////////////////////
bool http::test_detect_traffic(char const* begin, char const* end)
{
	try
	{
		std::auto_ptr<analyzer> a(new analyzer);
		int i = 1;
		for(char const* t=begin; (t+i) != end; ++i)
		{
			recognition result(a->recognize(t, t + i));
			switch(result)
			{
			case its_mine:
			case likely_mine:
				std::cout << "http has been detected\n";
				break;
			default:
				std::cout << "http has not been detected\n";
			}
		}
	}
	catch(std::exception& e)
	{
		std::cerr << e.what() << std::endl;
		return false;
	}
	return true;
}
///////////////////////////////////////////////////////////////////////////////
bool http::test_full_packet_traffic(char const* begin, char const* end
									, std::string const& out)
{
	bool test_result(false);
	srand(time(0));
	try
	{
		std::auto_ptr<analyzer> a(new analyzer);
		recognition result(a->recognize(begin, end));
		if(result == not_mine)
			throw std::runtime_error("input stream should be a http traffic");
		std::ofstream ofs(out.c_str(), std::ios::binary);
		if(!ofs.good())
			throw std::runtime_error("can not open/create output file");
		std::auto_ptr<message> msg;
		while(begin != end || a->can_extract())
		{
			size_t chunk_size(std::min(rnd_chunk_size(), static_cast<size_t>(end - begin)));
			if(msg.get())
			{
				a->enqueue(begin, begin + chunk_size);
				a->extract(*msg);
			}
			else
			{
				a->enqueue(begin, begin + chunk_size);
				msg = a->extract();
			}
			if(msg.get() && msg->complete())
			{
				prague::IO<char> packet(msg->assemble(0));
				std::copy(packet.begin(), packet.end(), std::ostream_iterator<char>(ofs));
				ofs << "\n";
				msg = std::auto_ptr<message>();
			}
			begin += chunk_size;
		}
		test_result = (begin == end && msg.get() == 0);
	}
	catch(std::exception& e)
	{
		std::cerr << e.what() << std::endl;
		return false;
	}
	return test_result;
}
///////////////////////////////////////////////////////////////////////////////
//! данные на обработку
struct processor_data
{
	processor_data(http::analyzer* a, std::string const& out)
		: stop_event(CreateEvent(0, TRUE, FALSE, 0))
		, analyzer(a)
		, fname(out.c_str())
	{
	}
	~processor_data()
	{
		CloseHandle(stop_event);
	}
	HANDLE stop_event;
	http::analyzer* analyzer;
	std::string fname;
};
///////////////////////////////////////////////////////////////////////////////
unsigned int __stdcall http_processor(void* params)
{
	processor_data* data = static_cast<processor_data*>(params);
	http::analyzer& a = *data->analyzer;
	std::ofstream ofs(data->fname.c_str());
	if(!ofs.good())
			throw std::runtime_error("can not open/create output file");
	std::auto_ptr<http::message> msg;
	while(WaitForSingleObject(data->stop_event, 10) == WAIT_TIMEOUT)
	{
		char const xxx[] = "HTTP/1.1 404 dgdgdgdgdgdgdgdgdgggggggggggggggggggggggdgdgdgdgd \r\n\r\n";
		a.recognize(xxx, xxx + sizeof(xxx));
//		msg.get() ? a.extract(msg.get()) : (msg = a.extract());
//		//! проверяем на полный пакет
//		if(msg.get() && msg->complete())
//		{
//			media_types const& mtypes = get_media_types();
//			ofs << "exctract "
//				<< (msg->type() == http::request_type ? "request" : "response")
//				<< " message";
//			if(msg->type() == http::response_type)
//				ofs << " (content-type: " << mtypes[msg->contenttype() - http::_application] << ")\n";
//			else
//				ofs << "\n" << std::endl;
//			if(msg->contenttype() == http::_text)
//			{
//				prague::WeakIO<char> content(msg->content());
//				std::copy(content.io().begin(), content.io().end(), std::ostream_iterator<char>(ofs));
//				ofs << "\n";
//			}
//			msg = std::auto_ptr<http::message>();
//		}
	}
	return 0;
}
///////////////////////////////////////////////////////////////////////////////
bool http::test_async_full_packet_traffic(char const* begin
										, char const* end
										, std::string const& out)
{
	srand(time(0));
	try
	{
		std::auto_ptr<http::analyzer> a(new http::analyzer);
		std::auto_ptr<http::analyzer> a1(new http::analyzer);
		processor_data data(a.get(), out);
		processor_data data1(a1.get(), out + "1");
		unsigned long h = _beginthreadex(0, 0, http_processor, &data, 0, 0);
		unsigned long h1 = _beginthreadex(0, 0, http_processor, &data1, 0, 0);
		while(begin != end)
		{
			size_t chunk_size(std::min(rnd_chunk_size(), static_cast<size_t>(end - begin)));
			a->enqueue(begin, begin + chunk_size);
			Sleep(1);
			a1->enqueue(begin, begin + chunk_size);
			begin += chunk_size;
			Sleep(5);
		}
		SetEvent(data.stop_event);
		SetEvent(data1.stop_event);
		WaitForSingleObject(reinterpret_cast<HANDLE>(h), INFINITE);
		WaitForSingleObject(reinterpret_cast<HANDLE>(h1), INFINITE);
		CloseHandle(reinterpret_cast<HANDLE>(h));
		CloseHandle(reinterpret_cast<HANDLE>(h1));
	}
	catch(std::exception& e)
	{
		std::cerr << e.what() << std::endl;
		return false;
	}
	return true;
}
///////////////////////////////////////////////////////////////////////////////
bool http::test_chunk_parser(char const* begin, char const* end)
{
	namespace spirit = boost::spirit;
	//begin += 0x139;
	http::chunk_data data;
	http::chunk ch(&data);
	spirit::parse_info<> info = spirit::parse(begin, end, ch);
	if(info.hit)
	{
		std::string s(info.stop, end);
		std::cout << "ok:\n"<< s << std::endl;
		http::last_chunk_data last_data;
		http::last_chunk ch1(&last_data);
		info = spirit::parse(info.stop, end, ch1);
		if(info.hit)
		{
			std::string s(info.stop, end);
			std::cout << "ok:\n"<< s << std::endl;
		}
	}
	else
	{
		std::string s(info.stop, end);
		std::cout << "error:\n"<< s << std::endl;
	}
	return true;
}
///////////////////////////////////////////////////////////////////////////////
void http::xoring_file(std::string const& fname, std::string const& out)
{
	std::ifstream ifs(fname.c_str(), std::ios::binary);
	std::ofstream ofs(out.c_str(), std::ios::binary);
	char buffer[4096] = { 0 };
	std::ifstream::pos_type pos(ifs.tellg());
	size_t read(0);
	ifs.seekg(0, std::ios::end);
	size_t size(ifs.tellg() - std::ifstream::pos_type(0));
	ifs.seekg(0, std::ios::beg);
	while(!ifs.read(buffer, sizeof(buffer)).eof())
	{
		read = ifs.tellg() - pos;
		ofs.write(crypt::xor_encode(buffer, buffer + read, xor_key), read);
		pos = ifs.tellg();
	}
	if(read = (size - pos))
	{
		ofs.write(crypt::xor_encode(buffer, buffer + read, xor_key), read);
	}
}
///////////////////////////////////////////////////////////////////////////////
bool http::test_processor(char const* begin, char const* end)
{
	std::auto_ptr<processing_strategy> strategy(new http::cached_strategy(0, 0));
	std::auto_ptr<session> proxy(new http::session(0, 0));
	std::auto_ptr<analyzer> analyzer(new http::analyzer());
	http::processor p(strategy, proxy, analyzer, 1000);
	p.process(begin, end, http::to_client, false);
	return true;
}
///////////////////////////////////////////////////////////////////////////////
bool http::test_processor(std::list<std::string> const& l)
{
	typedef util::scoped_handle<char*, BOOL (WINAPI*)(void const*), &UnmapViewOfFile, 0, 0> map_mem_ptr;

	std::auto_ptr<processing_strategy> strategy(new http::cached_strategy(0, 0));
	std::auto_ptr<session> proxy(new http::session(0, 0));
	std::auto_ptr<analyzer> analyzer(new http::analyzer());
	http::processor p(strategy, proxy, analyzer, 1000);
	std::list<std::string>::const_iterator it = l.begin();
	for(;it != l.end(); ++it)
	{
		std::pair<char*, size_t> stm(aux::create_http_stream(*it));
		map_mem_ptr mem(stm.first);
		size_t stm_size(stm.second);
		int direction = (*it)[it->length() - 1];
		p.process(mem.get(), mem.get() + stm_size, (direction == 'c' ? http::to_client : http::to_server), false);
	}
	return true;
}