#include "test.h"
#include "config.h"

#include <process.h>

#include <iostream>
#include <fstream>

using namespace http;
using namespace std;

#define SCAN_INFO(variable,verdict) cScanProcessInfo variable; variable.m_nProcessStatusMask = verdict;
#define DETECT_INFO(variable,virname) cDetectObjectInfo variable; variable.m_strDetectName = virname;
#define DATA_ROOT "H:\\PPP\\Mail\\Interceptors\\TrafficMonitor2\\ProtocollersEx\\tests\\HTTPUnitTests\\DataManager\\"
#define ENTRY\
	LOG_CALL5("\n",__FUNCTION__," ",fld,"\n");\
	string data_folder (DATA_ROOT);\
	data_folder.append (fld);

#define DECLARE_HTTP_CLASSES\
	tp::session_t s_t;\
	CTrafficMonitor mc;\
	tEngine eng;\
	std::auto_ptr<processing_strategy> strategy(EngineTypeSelector<T>::CreateStrategy (0, (hOBJECT)&eng));\
	std::auto_ptr<session> proxy(new http::session((hOBJECT)&mc, &s_t));\
	std::auto_ptr<analyzer> analyzer(new http::analyzer());

#define CONFIGURE(SIZE,TIMEOUT,RECONNECT)\
	Configure::GetConfiguration().MaximumChunkSize(SIZE);\
	Configure::GetConfiguration().TimeoutBetweenChunks(TIMEOUT);\
	Configure::GetConfiguration().ReconnectAfterEachPacket(RECONNECT);

#define PROCESS_PACKET(PROCESSOR,D_BEGIN,D_END,DIRECTION,IS_SSL)\
	SetEvent(mc.hAboutToCompletePacketEvent);\
	_ASSERTE(p.process(D_BEGIN, D_END, DIRECTION, IS_SSL));\
	WaitForSingleObject(mc.hPacketCompletedEvent, INFINITE);

#define PROCESS_DATA(PROCESSOR,D_BEGIN,D_END,DIRECTION,IS_SSL)\
	mc.strSpecificData = string(D_BEGIN, D_END);\
	SetEvent(mc.hAboutToCompleteSpecificDataEvent);\
	_ASSERTE(p.process(D_BEGIN, D_END, DIRECTION, IS_SSL));\
	WaitForSingleObject(mc.hSpecificDataCompletedEvent, INFINITE);

#define PROCESS_PACKET_ASYNC(PROCESSOR,D_BEGIN,D_END,DIRECTION,IS_SSL)\
	SetEvent(mc.hAboutToCompletePacketEvent);\
	TrafficSender::Enqueue(PROCESSOR, D_BEGIN, D_END, DIRECTION, IS_SSL);\
	WaitForSingleObject(mc.hPacketCompletedEvent, INFINITE);

#define ADD_NEW_PACKET(STORAGE,DATA,SOURCE,STATUS)\
	STORAGE->push_back(aux::output_packet (DATA, TrafficProtocoller::SOURCE, TrafficProtocoller::STATUS));

template<EngineType T> void TestHTTP<T>::test_syntax (const std::string& fld)
{
	ENTRY
	DECLARE_HTTP_CLASSES
	aux::processor_proxy p(strategy, proxy, analyzer);

	auto_ptr<vector<string> > packets = aux::http_packets (data_folder.c_str());
	auto_ptr<vector<aux::output_packet> > out_packets = aux::get_output_packets ((hOBJECT)g_root, data_folder.c_str());
	auto_ptr<vector<hIO> > control_files = aux::get_control_files ((hOBJECT)g_root, data_folder.c_str());

	CONFIGURE(10,100,false)

	SendBytesToAnalyzer (p, packets.get(), out_packets.get());

	eng.VerifyCheckedObjects (control_files.get());
	mc.VerifyConnStream (out_packets.get());

}

template<EngineType T> void TestHTTP<T>::test_one_byte (const std::string& fld)
{
	ENTRY
	DECLARE_HTTP_CLASSES
	aux::processor_proxy p(strategy, proxy, analyzer, 10);

	auto_ptr<vector<string> > packets = aux::http_packets (data_folder.c_str());
	auto_ptr<vector<aux::output_packet> > out_packets = aux::get_output_packets ((hOBJECT)g_root, data_folder.c_str());
	auto_ptr<vector<hIO> > control_files = aux::get_control_files ((hOBJECT)g_root, data_folder.c_str());

	CONFIGURE(1,100,false)

	SendBytesToAnalyzer (p, packets.get(), out_packets.get());

	eng.VerifyCheckedObjects (control_files.get());
	mc.VerifyConnStream (out_packets.get());

}

template<EngineType T> void TestHTTP<T>::test_pipelining (const std::string& fld)
{
	ENTRY
	DECLARE_HTTP_CLASSES
	aux::processor_proxy p(strategy, proxy, analyzer);

	auto_ptr<vector<string> > packets = aux::http_packets (data_folder.c_str());
	auto_ptr<vector<aux::output_packet> > out_packets = aux::get_output_packets ((hOBJECT)g_root, data_folder.c_str());
	auto_ptr<vector<hIO> > control_files = aux::get_control_files ((hOBJECT)g_root, data_folder.c_str());

	CONFIGURE(10,500,false)
	mc.setEachPacketSignalling (true);

	string buffer_to_send;

	buffer_to_send.append (packets->at(0));
	buffer_to_send.append (packets->at(1));
	buffer_to_send.append (packets->at(2));
	buffer_to_send.append (packets->at(3));
	buffer_to_send.append (packets->at(4));
	char* start = const_cast<char*>(buffer_to_send.c_str());
	int chunk_size = buffer_to_send.size();

	PROCESS_PACKET(p,start,start + chunk_size,to_server,false)

	buffer_to_send = packets->at(5);
	start = const_cast<char*>(buffer_to_send.c_str());
	chunk_size = buffer_to_send.size();

	PROCESS_PACKET(p,start,start + chunk_size,to_client,false)

	SetEvent(mc.hAboutToCompletePacketEvent);
	WaitForSingleObject(mc.hPacketCompletedEvent, INFINITE);


	eng.VerifyCheckedObjects (control_files.get());
	mc.VerifyConnStream (out_packets.get());

}

template<EngineType T> void TestHTTP<T>::test_caching_response_chunks (const std::string& fld)
{//добавить тест с пайплайнингом
	ENTRY
	DECLARE_HTTP_CLASSES
	aux::processor_proxy p(strategy, proxy, analyzer, 100);

	//auto_ptr<vector<aux::output_packet> > out_packets = aux::get_output_packets ((hOBJECT)g_root, data_folder.c_str());
	auto_ptr<vector<aux::output_packet> > out_packets (new vector<aux::output_packet>());
	auto_ptr<vector<hIO> > control_files = aux::get_control_files ((hOBJECT)g_root, data_folder.c_str());

	mc.setEachPacketSignalling (true);

	string get_request ("GET http://fake/fake.txt HTTP/1.0\015\012\015\012");
	string answer_header ("HTTP/1.1 200 OK\015\012Transfer-Encoding: chunked\015\012\015\012");
	string null_chunk ("0\015\012\015\012");
	auto_ptr<vector<string> > packets = aux::http_packets (data_folder.c_str());

	vector<string>::iterator it;

	for (it = packets->begin(); it != packets->end(); ++it)
	{

		string this_answer (answer_header);
		//sending get
		PROCESS_PACKET(p,get_request.c_str(),get_request.c_str() + get_request.size(), to_server, false)
		ADD_NEW_PACKET(out_packets,get_request,dsClient,psKeepAlive)

		//sending response
		_ASSERTE(p.process(answer_header.c_str(), answer_header.c_str() + answer_header.size(), to_client, false));

		const char* data = it->c_str();
		int bytes_remain = it->size();

		//отправляем чанки данных
		while (bytes_remain)
		{
			int bytes_to_send = std::min ((long)bytes_remain, aux::rnd_chunk_size2(1)/*bytes_remain*/);
			string chunk = makeChunk (data, bytes_to_send);
			data += bytes_to_send;
			bytes_remain -= bytes_to_send;
			_ASSERTE(p.process(chunk.c_str(), chunk.c_str() + chunk.size(), to_client, false));
			this_answer.append (chunk);
			Sleep ( aux::rnd_chunk_size2(500) );
		}

		//последний - нулевой
		PROCESS_PACKET(p,null_chunk.c_str(), null_chunk.c_str() + null_chunk.size(), to_client, false)

		this_answer.append (null_chunk);
		ADD_NEW_PACKET(out_packets,CTrafficMonitor::RebuildPacket(this_answer),dsClient,psKeepAlive)

	}

	eng.VerifyCheckedObjects (control_files.get());

	//когда будет готова сборка данных пакета из chunk-ов
	mc.VerifyConnStream (out_packets.get());

}

template<EngineType T> void TestHTTP<T>::test_caching_response_normal (const std::string& fld)
{//добавить тест с pipelining_ом после исправления ошибки
	ENTRY
	DECLARE_HTTP_CLASSES
	aux::processor_proxy p(strategy, proxy, analyzer, 100);

	auto_ptr<vector<aux::output_packet> > out_packets /*= aux::get_output_packets ((hOBJECT)g_root, data_folder.c_str());*/
		( new vector<aux::output_packet>() );
	auto_ptr<vector<hIO> > control_files = aux::get_control_files ((hOBJECT)g_root, data_folder.c_str());

	mc.setEachPacketSignalling (true);

	string get_request ("GET http://fake/fake.txt HTTP/1.0\015\012\015\012");
	string answer_header ("HTTP/1.1 200 OK\015\012Content-Length: ");
	auto_ptr<vector<string> > packets = aux::http_packets (data_folder.c_str());

	vector<string>::iterator it;

	for (it = packets->begin(); it != packets->end(); ++it)
	{
		char buf[10];
		string this_answer_heaader (answer_header);
		this_answer_heaader.append (itoa(it->size(), buf, 10)). append("\015\012\015\012");
		//sending get
		PROCESS_PACKET(p,get_request.c_str(), get_request.c_str() + get_request.size(), to_server, false);
		ADD_NEW_PACKET(out_packets,get_request,dsClient,psKeepAlive)

		//sending response
		_ASSERTE(p.process(this_answer_heaader.c_str(), this_answer_heaader.c_str() + this_answer_heaader.size(), to_client, false));

		const char* data = it->c_str();
		int bytes_remain = it->size();

		//отправляем данные
		while (bytes_remain)
		{
			int bytes_to_send = std::min ((long)bytes_remain, aux::rnd_chunk_size2(/*it->size()*/1));
			if (bytes_to_send == bytes_remain)
			{
				PROCESS_PACKET(p,data, data + bytes_to_send, to_client, false);
			} else _ASSERTE(p.process(data, data + bytes_to_send, to_client, false));

			data += bytes_to_send;
			bytes_remain -= bytes_to_send;	
			Sleep ( aux::rnd_chunk_size2(500) );
		}

		//добавим в массив сравнения сообщение
		ADD_NEW_PACKET(out_packets,string(this_answer_heaader).append(it->c_str()),dsClient,psKeepAlive)


	}

	eng.VerifyCheckedObjects (control_files.get());
	mc.VerifyConnStream (out_packets.get());

}

template<EngineType T> void TestHTTP<T>::test_100_continue_on_request (const std::string& fld)
{
	LOG_CALL5("\n",__FUNCTION__," ",fld,"\n");
	_ASSERTE (!"Implement after having the bug fixed");
}

template<EngineType T> void TestHTTP<T>::test_cannot_detect_length (const std::string& fld)
{
	ENTRY
	DECLARE_HTTP_CLASSES
	aux::processor_proxy p(strategy, proxy, analyzer);

	auto_ptr<vector<string> > packets = aux::http_packets (data_folder.c_str());
	auto_ptr<vector<aux::output_packet> > out_packets = aux::get_output_packets ((hOBJECT)g_root, data_folder.c_str());
	auto_ptr<vector<hIO> > control_files = aux::get_control_files ((hOBJECT)g_root, data_folder.c_str());

	CONFIGURE(10,500,false)
	mc.setEachPacketSignalling (true);

	string buffer_to_send;

	buffer_to_send = packets->at(0);
	char* start = const_cast<char*>(buffer_to_send.c_str());
	int chunk_size = buffer_to_send.size();

	PROCESS_PACKET(p,start, start + chunk_size, to_server, false);

	buffer_to_send = packets->at(1);
	start = const_cast<char*>(buffer_to_send.c_str());
	chunk_size = buffer_to_send.size();

	PROCESS_PACKET(p,start, start + chunk_size, to_client, false);
}

template<EngineType T> void TestHTTP<T>::test_chunks_cases (const std::string& fld)
{
	ENTRY

	{//long chunk on response
		DECLARE_HTTP_CLASSES
		aux::processor_proxy p(strategy, proxy, analyzer);

		auto_ptr<vector<aux::output_packet> > out_packets;
			//= aux::get_output_packets ((hOBJECT)g_root, data_folder.c_str());

		mc.setEachPacketSignalling (true);

		string get_request ("GET http://fake/fake.txt HTTP/1.0\015\012\015\012");
		string answer_header ("HTTP/1.1 200 OK\015\012Transfer-Encoding: chunked\015\012\015\012");
		string null_chunk ("\015\0120\015\012\015\012");
		//auto_ptr<vector<string> > packets = aux::http_packets (data_folder.c_str());

		vector<string>::iterator it;
		const int chunk_size = 1024*1024;
		const int max_buffer_size = 20*1024;

		{
			//sending get
			PROCESS_PACKET(p,get_request.c_str(), get_request.c_str() + get_request.size(), to_server, false);

			//sending response
			_ASSERTE(p.process(answer_header.c_str(), answer_header.c_str() + answer_header.size(), to_client, false));

			int bytes_remain = chunk_size;
			char buf[20];
			char buffer[max_buffer_size];

			string s1 ( itoa (bytes_remain, buf, 16));
			s1.append("\x0d\x0a");
			_ASSERTE (p.process (s1.c_str(), s1.c_str() + s1.size(), to_client, false));

			std::ofstream w( string(data_folder).append("\\object.01.chk").c_str(), std::ios_base::binary);	
			//отправляем чанки данных
			while (bytes_remain)
			{
				int bytes_to_send = std::min ((long)bytes_remain, aux::rnd_chunk_size2(max_buffer_size));
				for (int i = 0; i < bytes_to_send; ++i) buffer[i] = aux::rnd_chunk_size2(0xFF);
				bytes_remain -= bytes_to_send;
				_ASSERTE(p.process(&buffer[0], &buffer[bytes_to_send], to_client, false));
				std::copy (&buffer[0], &buffer[bytes_to_send], std::ostream_iterator<char>(w));
			}
			//последний - нулевой
			PROCESS_PACKET(p,null_chunk.c_str(), null_chunk.c_str() + null_chunk.size(), to_client, false);

		}
		auto_ptr<vector<hIO> > control_files = aux::get_control_files ((hOBJECT)g_root, data_folder.c_str());
		eng.VerifyCheckedObjects (control_files.get());
		//когда будет готова сборка данных пакета из chunk-ов
		//mc.VerifyConnStream (out_packets.get());
	}

}

template<EngineType T> void TestHTTP<T>::test_packet_after_packet (const std::string& fld)
{
	ENTRY
	DECLARE_HTTP_CLASSES
	aux::processor_proxy p(strategy, proxy, analyzer);

	auto_ptr<vector<string> > packets = aux::http_packets (data_folder.c_str());
	auto_ptr<vector<aux::output_packet> > out_packets = aux::get_output_packets ((hOBJECT)g_root, data_folder.c_str());
	auto_ptr<vector<hIO> > control_files = aux::get_control_files ((hOBJECT)g_root, data_folder.c_str());

	CONFIGURE(10,500,false)
	mc.setEachPacketSignalling (true);

	vector<string>::iterator it;
	vector<aux::output_packet>::iterator it2;

	for (it = packets->begin(), it2 = out_packets->begin(); it != packets->end(), it2 != out_packets->end(); ++it2, ++it)
	{
		PROCESS_PACKET(p,it->c_str(), it->c_str() + it->size(), aux::SourceToDirection(it2->dsDataTarget), false);
	}

	eng.VerifyCheckedObjects (control_files.get());
	mc.VerifyConnStream (out_packets.get());

}

template<EngineType T> void TestHTTP<T>::test_data_after_data (const std::string& fld)
{
	ENTRY
	DECLARE_HTTP_CLASSES
	aux::processor_proxy p(strategy, proxy, analyzer);

	auto_ptr<vector<string> > packets = aux::http_packets (data_folder.c_str());
	auto_ptr<vector<aux::output_packet> > out_packets = aux::get_output_packets ((hOBJECT)g_root, data_folder.c_str());
	auto_ptr<vector<hIO> > control_files = aux::get_control_files ((hOBJECT)g_root, data_folder.c_str());

	CONFIGURE(10,500,false)
	mc.setSpecificDataSignalling (true);

	vector<string>::iterator it;
	vector<aux::output_packet>::iterator it2;
	it = packets->begin();
	it2 = out_packets->begin();

	for (; it != packets->end(), it2 != out_packets->end(); ++it2, ++it)
	{
		PROCESS_DATA(p,it->c_str(), it->c_str() + it->size(), aux::SourceToDirection(it2->dsDataTarget), false);
	}

	eng.VerifyCheckedObjects (control_files.get());
	mc.VerifyConnStream (out_packets.get());

}

template<EngineType T> void TestHTTP<T>::test_method_supported (const std::string& method)
{
	LOG_CALL5("\n",__FUNCTION__," ",method,"\n");

	//request and answer w/o body
	{
		DECLARE_HTTP_CLASSES
		aux::processor_proxy p(strategy, proxy, analyzer);

		mc.setEachPacketSignalling (true);

		string request (method);
		request.append (" http://fake/fake.txt HTTP/1.1\015\012\015\012");
		string response ("HTTP/1.1 204 No Content\015\012\015\012");

		vector<aux::output_packet> out_packets;
		out_packets.push_back ( aux::output_packet
			(request, TrafficProtocoller::dsClient, TrafficProtocoller::psKeepAlive) );
		out_packets.push_back ( aux::output_packet
			(response, TrafficProtocoller::dsServer, TrafficProtocoller::psKeepAlive) );

		//sending request
		PROCESS_PACKET(p,request.c_str(), request.c_str() + request.size(), to_server, false);

		//sending response
		PROCESS_PACKET(p,response.c_str(), response.c_str() + response.size(), to_client, false);

		//eng.VerifyCheckedObjects (&vector<hIO>());
		//bug workaround
		vector<hIO> objects;
		//objects.push_back ( prague::create_temp_io() );

		eng.VerifyCheckedObjects (&objects);
		mc.VerifyConnStream (&out_packets);
	}

	//request and answer w/ body
	{
		DECLARE_HTTP_CLASSES
		aux::processor_proxy p(strategy, proxy, analyzer);

		mc.setEachPacketSignalling (true);

		string request (method);
		request.append (" http://fake/fake.txt HTTP/1.1\015\012Content-Length: 21\015\012\015\012");
		request.append ("from client to server");
		string response ("HTTP/1.1 207 Multi-Status\015\012Content-Length: 21\015\012\015\012");
		response.append ("from server to client");

		vector<aux::output_packet> out_packets;
		out_packets.push_back ( aux::output_packet
			(request, TrafficProtocoller::dsClient, TrafficProtocoller::psKeepAlive) );
		out_packets.push_back ( aux::output_packet
			(response, TrafficProtocoller::dsServer, TrafficProtocoller::psKeepAlive) );

		//sending request
		PROCESS_PACKET(p,request.c_str(), request.c_str() + request.size(), to_server, false);

		//sending response
		PROCESS_PACKET(p,response.c_str(), response.c_str() + response.size(), to_client, false);

		vector<hIO> objects;
		objects.push_back ( prague::create_temp_io() );
		tDWORD result;
		objects.back()->SeekWrite(&result, 0, "from server to client", 21);

		eng.VerifyCheckedObjects (&objects);
		mc.VerifyConnStream (&out_packets);
	}

}

template<EngineType T> void TestHTTP<T>::test_method_unsupported_middle (const std::string& method)
{
	LOG_CALL5("\n",__FUNCTION__," ",method,"\n");

	{//without content-length
		DECLARE_HTTP_CLASSES
		aux::processor_proxy p(strategy, proxy, analyzer);

		mc.setEachPacketSignalling (true);

		string known_request ("GET http://fake/fake.txt HTTP/1.1\015\012\015\012");
		string known_response ("HTTP/1.1 204 No Content\015\012\015\012");

		string request (method); request.append (" http://fake/fake.txt HTTP/1.1\015\012\015\012");
		string response ("HTTP/1.1 204 No Content\015\012\015\012");

		vector<aux::output_packet> out_packets;
		out_packets.push_back ( aux::output_packet
			(request, TrafficProtocoller::dsClient, TrafficProtocoller::psKeepAlive) );
		out_packets.push_back ( aux::output_packet
			(response, TrafficProtocoller::dsServer, TrafficProtocoller::psKeepAlive) );

		//sending known request
		PROCESS_PACKET(p,known_request.c_str(), known_request.c_str() + known_request.size(), to_server, false);

		//sending known response
		PROCESS_PACKET(p,known_response.c_str(), known_response.c_str() + known_response.size(), to_client, false);

		//sending unknown request
		PROCESS_PACKET(p,request.c_str(), request.c_str() + request.size(), to_server, false);

		//sending unknown response
		PROCESS_PACKET(p,response.c_str(), response.c_str() + response.size(), to_client, false);

		//sending known request
		PROCESS_PACKET(p,known_request.c_str(), known_request.c_str() + known_request.size(), to_server, false);

		//sending known response
		PROCESS_PACKET(p,known_response.c_str(), known_response.c_str() + known_response.size(), to_client, false);

		/*
		//eng.VerifyCheckedObjects (&vector<hIO>());
		//bug workaround
		vector<hIO> objects;
		objects.push_back ( prague::create_temp_io() );

		eng.VerifyCheckedObjects (&objects);
		mc.VerifyConnStream (&out_packets);*/
	}

	{//with content-length
		DECLARE_HTTP_CLASSES
		aux::processor_proxy p(strategy, proxy, analyzer);

		mc.setEachPacketSignalling (true);

		string known_request ("GET http://fake/fake.txt HTTP/1.1\015\012\015\012");
		string known_response ("HTTP/1.1 204 No Content\015\012\015\012");

		string request (method);
		request.append (" http://fake/fake.txt HTTP/1.1\015\012Content-Length: 21\015\012\015\012");
		request.append ("from client to server");

		string response ("HTTP/1.1 207 Multi-Status\015\012Content-Length: 21\015\012\015\012");
		response.append ("from server to client");

		vector<aux::output_packet> out_packets;
		out_packets.push_back ( aux::output_packet
			(request, TrafficProtocoller::dsClient, TrafficProtocoller::psKeepAlive) );
		out_packets.push_back ( aux::output_packet
			(response, TrafficProtocoller::dsServer, TrafficProtocoller::psKeepAlive) );

		//sending known request
		PROCESS_PACKET(p,known_request.c_str(), known_request.c_str() + known_request.size(), to_server, false);

		//sending known response
		PROCESS_PACKET(p,known_response.c_str(), known_response.c_str() + known_response.size(), to_client, false);

		//sending unknown request
		PROCESS_PACKET(p,request.c_str(), request.c_str() + request.size(), to_server, false);

		//sending unknown response
		PROCESS_PACKET(p,response.c_str(), response.c_str() + response.size(), to_client, false);

		//sending known request
		PROCESS_PACKET(p,known_request.c_str(), known_request.c_str() + known_request.size(), to_server, false);

		//sending known response
		PROCESS_PACKET(p,known_response.c_str(), known_response.c_str() + known_response.size(), to_client, false);

		/*
		//eng.VerifyCheckedObjects (&vector<hIO>());
		//bug workaround
		vector<hIO> objects;
		objects.push_back ( prague::create_temp_io() );

		eng.VerifyCheckedObjects (&objects);
		mc.VerifyConnStream (&out_packets);*/
	}
}

template<EngineType T> void TestHTTP<T>::test_litter (const std::string& fld)
{
	ENTRY
	aux::DeleteDirectory (data_folder.c_str());
	aux::MakeDirectory (data_folder.c_str());

	{//test unparsable data after request-response
		DECLARE_HTTP_CLASSES
		aux::processor_proxy p(strategy, proxy, analyzer);

		mc.setEachPacketSignalling (true);

		string request ("GET http://fake/fake.txt HTTP/1.0\015\012\015\012");
		string response ("HTTP/1.1 200 OK\015\012Content-Length: 10\015\012\015\012qwertyuiop");

		//sending get
		PROCESS_PACKET(p,request.c_str(), request.c_str() + request.size(), to_server, false);
		//sending response
		PROCESS_PACKET(p,response.c_str(), response.c_str() + response.size(), to_client, false);

		int bytes_remain = 0x10000;
		const int max_chunk = 5120;
		char buffer[max_chunk];

		{
			std::ofstream w( string(data_folder).append("\\object.0000.dat").c_str(), std::ios_base::binary);	
			w<<request;
			aux::MakePacketDescriptionFile (data_folder, "object.0000.dat", to_server, TrafficProtocoller::psKeepAlive);
		}
		{
			std::ofstream w( string(data_folder).append("\\object.0001.dat").c_str(), std::ios_base::binary);	
			w<<response;
			aux::MakePacketDescriptionFile (data_folder, "object.0001.dat", to_client, TrafficProtocoller::psKeepAlive);
		}
		{
			std::ofstream w( string(data_folder).append("\\object.0001.chk").c_str(), std::ios_base::binary);	
			w<<"qwertyuiop";
		}

		//отправляем чанки данных
		int portion_no = 2;
		while (bytes_remain)
		{
			msg_direction direction = (portion_no%2) ? to_client : to_server;
			int bytes_to_send = std::min ((long)bytes_remain, aux::rnd_chunk_size2(max_chunk));
			for (int i = 0; i < bytes_to_send; ++i) buffer[i] = aux::rnd_chunk_size2(0xFF);
			bytes_remain -= bytes_to_send;
			_ASSERTE(p.process(&buffer[0], &buffer[bytes_to_send], direction, false));
			char p_no[10];
			sprintf (p_no, "%04d", portion_no);
			portion_no++;
			std::ofstream w( string(data_folder).append("\\object.").append(p_no).append(".dat").c_str(), std::ios_base::binary);	
			std::copy (&buffer[0], &buffer[bytes_to_send], std::ostream_iterator<char>(w));
			aux::MakePacketDescriptionFile (data_folder, string("object.").append(p_no).append(".dat"), direction, TrafficProtocoller::psKeepAlive);
		}

		auto_ptr<vector<aux::output_packet> > out_packets = aux::get_output_packets ((hOBJECT)g_root, data_folder.c_str());
		mc.VerifyConnStream (out_packets.get());

		auto_ptr<vector<hIO> > control_files = aux::get_control_files ((hOBJECT)g_root, data_folder.c_str());
		eng.VerifyCheckedObjects (control_files.get());
	}

}

template<EngineType T> void TestHTTP<T>::test_request_filters (const std::string& fld)
{
	ENTRY
	DECLARE_HTTP_CLASSES
	boost::shared_ptr<aux::processor_proxy> p (new aux::processor_proxy (strategy, proxy, analyzer));

	auto_ptr<vector<string> > packets = aux::http_packets (data_folder.c_str());
	auto_ptr<vector<aux::output_packet> > out_packets = aux::get_output_packets ((hOBJECT)g_root, data_folder.c_str());
	auto_ptr<vector<hIO> > control_files = aux::get_control_files ((hOBJECT)g_root, data_folder.c_str());

	CONFIGURE(10,500,false)

	eng.GetFilterQuery().Activate();
	mc.setEachPacketSignalling (true);

	vector<string>::iterator it = packets->begin();
	vector<aux::output_packet>::iterator it2 = out_packets->begin();

	tEngine::FilterQuery& filter = eng.GetFilterQuery();
	string url;
	tEngine::URLFilterEnum type; 
	filter.Activate();

	SetEvent(mc.hAboutToCompletePacketEvent);
	TrafficSender::Enqueue(p, it->c_str(), it->c_str() + it->size(), aux::SourceToDirection(it2->dsDataTarget), false);

	string etalon_url;
	std::ifstream url01(string (data_folder).append("\\01.url.txt").c_str(), std::ios::binary);
	url01.unsetf(std::ios_base::skipws);
	std::copy (istream_iterator<char>(url01), istream_iterator<char>(), back_inserter (etalon_url));


	for (int i = 0; i < tEngine::filter_count; ++i)
	{
		filter.GetRequest (type, url);
		_ASSERTE (type == (tEngine::URLFilterEnum)i && url == etalon_url);
		filter.GoOn (errOK);
	}

	WaitForSingleObject(mc.hPacketCompletedEvent, INFINITE);

	++it; ++it2;

	PROCESS_PACKET_ASYNC(p, it->c_str(), it->c_str() + it->size(), aux::SourceToDirection(it2->dsDataTarget), false);

	eng.VerifyCheckedObjects (control_files.get());
	mc.VerifyConnStream (out_packets.get());

}

template<EngineType T> void TestHTTP<T>::test_request_filters_deny (const std::string& fld, const cEngineBase::URLFilterEnum& stop_type)
{
	LOG_CALL3("\n",__FUNCTION__,fld);
	LOG_CALL3(" - ",stop_type,"\n");
	string data_folder (DATA_ROOT);
	data_folder.append (fld);
	DECLARE_HTTP_CLASSES
	boost::shared_ptr<aux::processor_proxy> p (new aux::processor_proxy (strategy, proxy, analyzer));

	auto_ptr<vector<string> > packets = aux::http_packets (data_folder.c_str());
	auto_ptr<vector<aux::output_packet> > out_packets = aux::get_output_packets ((hOBJECT)g_root, data_folder.c_str());
	auto_ptr<vector<hIO> > control_files = aux::get_control_files ((hOBJECT)g_root, data_folder.c_str());

	CONFIGURE(10,500,false)

	eng.GetFilterQuery().Activate();
	mc.setEachPacketSignalling (true);

	vector<string>::iterator it = packets->begin();
	vector<aux::output_packet>::iterator it2 = out_packets->begin();

	tEngine::FilterQuery& filter = eng.GetFilterQuery();
	string url;
	tEngine::URLFilterEnum type; 
	filter.Activate();

	SetEvent(mc.hAboutToCompletePacketEvent);
	TrafficSender::Enqueue(p, it->c_str(), it->c_str() + it->size(), aux::SourceToDirection(it2->dsDataTarget), false);

	string etalon_url;
	std::ifstream url01(string (data_folder).append("\\01.url.txt").c_str(), std::ios::binary);
	url01.unsetf(std::ios_base::skipws);
	std::copy (istream_iterator<char>(url01), istream_iterator<char>(), back_inserter (etalon_url));


	for (int i = 0; i < tEngine::filter_count; ++i)
	{
		filter.GetRequest (type, url);
		_ASSERTE (type == i && url == etalon_url);
		if (i == stop_type) filter.GoOn (errACCESS_DENIED); else filter.GoOn(errOK);
	}

	WaitForSingleObject(mc.hPacketCompletedEvent, INFINITE);

	//++it; ++it2;
	//SetEvent(mc.hAboutToCompletePacketEvent);
	//TrafficSender::Enqueue(p, it->c_str(), it->c_str() + it->size(), aux::SourceToDirection(it2->dsDataTarget), false);
	//WaitForSingleObject(mc.hPacketCompletedEvent, INFINITE);


	//Запрос не уйдет на сервер, так что выкидываем первый пакет из сравнения
	out_packets->erase(out_packets->begin());
	eng.VerifyCheckedObjects (control_files.get());
	mc.VerifyConnStream (out_packets.get());
	mc.VerifySentPackets (out_packets.get());

}

template<EngineType T> void TestHTTP<T>::test_request_avs_exchange (const std::string& fld)
{
	ENTRY
	DECLARE_HTTP_CLASSES
	boost::shared_ptr<aux::processor_proxy> p (new aux::processor_proxy (strategy, proxy, analyzer));

	auto_ptr<vector<string> > packets = aux::http_packets (data_folder.c_str());
	auto_ptr<vector<aux::output_packet> > out_packets = aux::get_output_packets ((hOBJECT)g_root, data_folder.c_str());
	auto_ptr<vector<hIO> > control_files = aux::get_control_files ((hOBJECT)g_root, data_folder.c_str());

	mc.setEachPacketSignalling (true);

	tEngine::AVSQuery& avs = eng.GetAVSQuery();
	avs.Activate();

	SCAN_INFO(si1,object_detected|object_was_deleted)
	DETECT_INFO(di1,"Zloj virus")

	vector<string>::const_iterator it = packets->begin();
	vector<aux::output_packet>::iterator it2 = out_packets->begin();

	PROCESS_PACKET_ASYNC(p, it->c_str(), it->c_str() + it->size(), aux::SourceToDirection(it2->dsDataTarget), false);

	++it; ++it2;
	SetEvent(mc.hAboutToCompletePacketEvent);
	TrafficSender::Enqueue(p, it->c_str(), it->c_str() + it->size(), aux::SourceToDirection(it2->dsDataTarget), false);
	avs.SetScanHTTP_options (di1, si1);
	//	mc.ExpectClosingConnection();
	WaitForSingleObject(mc.hPacketCompletedEvent, INFINITE);

	eng.VerifyCheckedObjects (control_files.get());
	mc.VerifyConnStream (out_packets.get());

}

template<EngineType T> void TestHTTP<T>::test_method_unsupported_start (const std::string& method)
{
	_ASSERTE (!"Not implemented");
}

template<EngineType T> void TestHTTP<T>::test_request_avs_interrupt (const std::string& fld)
{
	ENTRY
	DECLARE_HTTP_CLASSES
	boost::shared_ptr<aux::processor_proxy> p (new aux::processor_proxy (strategy, proxy, analyzer, 100));

	auto_ptr<vector<aux::output_packet> > out_packets( new vector<aux::output_packet>() );
	auto_ptr<vector<hIO> > control_files = aux::get_control_files ((hOBJECT)g_root, data_folder.c_str());

	mc.setEachPacketSignalling (true);
	tEngine::AVSQuery& avs = eng.GetAVSQuery();
	SCAN_INFO(si1,object_detected|object_was_deleted)
	DETECT_INFO(di1,"Zloj virus")

	string get_request ("GET http://fake/fake.txt HTTP/1.0\015\012\015\012");
	string answer_header ("HTTP/1.1 200 OK\015\012Content-Length: ");
	auto_ptr<vector<string> > packets = aux::http_packets (data_folder.c_str());

	vector<string>::iterator it = packets->begin();

	char buf[10];
	string this_answer_heaader (answer_header);
	this_answer_heaader.append (itoa(it->size(), buf, 10)). append("\015\012\015\012");
	//sending get
	SetEvent(mc.hAboutToCompletePacketEvent);
	TrafficSender::Enqueue(p, get_request.c_str(), get_request.c_str() + get_request.size(), http::to_server, false);
	WaitForSingleObject(mc.hPacketCompletedEvent, INFINITE);

	ADD_NEW_PACKET(out_packets,get_request,dsClient,psKeepAlive)

	//sending response
	TrafficSender::Enqueue(p, this_answer_heaader.c_str(), this_answer_heaader.c_str() + this_answer_heaader.size(), http::to_client, false);

	const char* data = it->c_str();
	int bytes_remain = it->size();

	//отправляем данные
	while (bytes_remain)
	{
		int bytes_to_send = std::min ((long)bytes_remain, aux::rnd_chunk_size2(1));
		if (bytes_to_send == bytes_remain)
		{
			avs.Activate();
			avs.SetScanHTTP_options (di1, si1);
			mc.ExpectClosingConnection();

			SetEvent(mc.hAboutToCompletePacketEvent);
			TrafficSender::Enqueue(p, data, data + bytes_to_send, http::to_client, false);
			WaitForSingleObject(mc.hPacketCompletedEvent, INFINITE);
		} else
			TrafficSender::Enqueue(p, data, data + bytes_to_send, http::to_client, false);

		data += bytes_to_send;
		bytes_remain -= bytes_to_send;	
		Sleep ( aux::rnd_chunk_size2(500) );
	}

	//добавим в массив сравнения сообщение
	ADD_NEW_PACKET(out_packets,string(this_answer_heaader).append(it->c_str()),dsClient,psKeepAlive)

	//сообщение, возвращенное из процессора, будет передано не до конца,
	//подрежем эталон до его длины

	int size = mc.GetHttpData().back().size();
	tQWORD old_size = 0;
	out_packets->rbegin()->hData->GetSize (&old_size, IO_SIZE_TYPE_EXPLICIT);
	_ASSERTE (old_size && (size != old_size));
	out_packets->rbegin()->hData->SetSize (size);

	eng.VerifyCheckedObjects (control_files.get());
	mc.VerifyConnStream (out_packets.get());


}


template<EngineType T> void TestHTTP<T>::test_request_avs_interrupt_chunked (const std::string& fld)
{
	ENTRY
	DECLARE_HTTP_CLASSES
	boost::shared_ptr<aux::processor_proxy> p (new aux::processor_proxy p(strategy, proxy, analyzer, 100));

	auto_ptr<vector<aux::output_packet> > out_packets( new vector<aux::output_packet>() );
	auto_ptr<vector<hIO> > control_files = aux::get_control_files ((hOBJECT)g_root, data_folder.c_str());

	mc.setEachPacketSignalling (true);
	tEngine::AVSQuery& avs = eng.GetAVSQuery();
	SCAN_INFO(si1,object_detected|object_was_deleted)
	DETECT_INFO(di1,"Zloj virus")

	string get_request ("GET http://fake/fake.txt HTTP/1.0\015\012\015\012");
	string answer_header ("HTTP/1.1 200 OK\015\012Transfer-Encoding: chunked\015\012\015\012");
	string null_chunk ("0\015\012\015\012");
	string this_answer/* (answer_header)*/;
	auto_ptr<vector<string> > packets = aux::http_packets (data_folder.c_str());

	vector<string>::iterator it = packets->begin();

	//sending get
	SetEvent(mc.hAboutToCompletePacketEvent);
	TrafficSender::Enqueue(p, get_request.c_str(), get_request.c_str() + get_request.size(), http::to_server, false);
	WaitForSingleObject(mc.hPacketCompletedEvent, INFINITE);

	ADD_NEW_PACKET(out_packets,get_request,dsClient,psKeepAlive)

	//sending response
	TrafficSender::Enqueue(p, answer_header.c_str(), answer_header.c_str() + answer_header.size(), http::to_client, false);

	const char* data = it->c_str();
	int bytes_remain = it->size();

	avs.Activate();
	avs.SetScanHTTP_options (di1, si1);
	mc.ExpectClosingConnection();

	//отправляем чанки данных
	while (bytes_remain)
	{
		int bytes_to_send = std::min ((long)bytes_remain, aux::rnd_chunk_size2(/*it->size()*/1));
		string chunk = makeChunk (data, bytes_to_send);
		data += bytes_to_send;
		bytes_remain -= bytes_to_send;
		if (!bytes_remain)
		{
			avs.Activate();
			avs.SetScanHTTP_options (di1, si1);
			mc.ExpectClosingConnection();
		}
		TrafficSender::Enqueue(p, chunk.c_str(), chunk.c_str() + chunk.size(), to_client, false);
		//this_answer.append (chunk);
		Sleep ( aux::rnd_chunk_size2(500) );
	}


	//последний - нулевой
	PROCESS_PACKET_ASYNC(p, null_chunk.c_str(), null_chunk.c_str() + null_chunk.size(), to_client, false);

	//добавим в массив сравнения сообщение
	ADD_NEW_PACKET(out_packets,it->c_str(),dsClient,psKeepAlive)//только данные

	//сообщение, возвращенное из процессора, будет передано не до конца,
	//подрежем эталон до его длины

	int size = mc.GetHttpData().back().size();
	tQWORD old_size = 0;
	out_packets->rbegin()->hData->GetSize (&old_size, IO_SIZE_TYPE_EXPLICIT);
	_ASSERTE (old_size && (size != old_size));
	out_packets->rbegin()->hData->SetSize (size);

	eng.VerifyCheckedObjects (control_files.get());
	mc.VerifyConnStream (out_packets.get());


}


template<EngineType T> void TestHTTP<T>::SendBytesToAnalyzer (aux::processor_proxy& p, vector<string>* packets, vector<aux::output_packet>* out_packets,
									bool finishing_flush)
{
	//чтобы порции трафика не ограничивались рамками одного пакета, но и можно было сделать закрытие соединения	
	string prev_packet_chunk;
	vector<string>::const_iterator it;
	vector<aux::output_packet>::iterator it2;
	for (it = packets->begin(), it2 = out_packets->begin(); it != packets->end(); ++it, ++it2)
	{
		Sleep (Configure::GetConfiguration().TimeoutBetweenChunks());

		string pckt = *it;
		if (prev_packet_chunk.size()) 
		{
			pckt = prev_packet_chunk;
			string pckt2 (*it);
			pckt.append (pckt2.c_str());
			prev_packet_chunk.clear();
		}

		const char* begin = pckt.c_str();
		const char* end = begin + pckt.size();
		char* start = const_cast<char*>(begin);
		while (start != end)
		{
			size_t chunk_size (aux::rnd_chunk_size(Configure::GetConfiguration().MaximumChunkSize()));
			if (chunk_size >= static_cast<size_t>(end - start))
			{
				if (Configure::GetConfiguration().ReconnectAfterEachPacket() || it2->bFlush ||
					it2->dsDataTarget != ((it+1 != packets->end())?(it2+1)->dsDataTarget : it2->dsDataTarget))
				{
					chunk_size = static_cast<size_t>(end - start);
					_ASSERTE (p.process(start, start + chunk_size, aux::SourceToDirection(it2->dsDataTarget), false));
					start += chunk_size;
				}
				else
				{	
					prev_packet_chunk.assign (start, end);
					break;
				}
			}
			else
			{
				_ASSERTE(p.process(start, start + chunk_size, aux::SourceToDirection(it2->dsDataTarget), false));
				start += chunk_size;
			}
			Sleep (Configure::GetConfiguration().TimeoutBetweenChunks());
		}
		LOG_CALL(".");
	}
	if (prev_packet_chunk.size())
	{
		_ASSERTE(p.process (prev_packet_chunk.c_str(), prev_packet_chunk.c_str() + prev_packet_chunk.size(),
			aux::SourceToDirection(out_packets->rbegin()->dsDataTarget), false));
	}
	if (finishing_flush) p.flush();

	LOG_CALL("\n");
}

template<EngineType T> string TestHTTP<T>::makeChunk (const char* data, int bytes_to_send, bool w_comments)
{
	static int makeChunk_calls = 0;
	if (w_comments) makeChunk_calls++;
	char buf[10];
	itoa (bytes_to_send, buf, 16);
	string chunk(buf);
	if (w_comments & (makeChunk_calls % 2))
	{
		itoa (makeChunk_calls, buf, 10);
		chunk.append (";par=comment").append(buf);
	}
	chunk.append ("\015\012").append (data, data + bytes_to_send).append ("\015\012\000");

	return chunk;

}

template<EngineType T> void TestHTTP<T>::test_skip_that_no_avs (const std::string& fld)
{
	ENTRY

	{//case 1
		DECLARE_HTTP_CLASSES
		boost::shared_ptr<aux::processor_proxy> p (new aux::processor_proxy (strategy, proxy, analyzer));

		auto_ptr<vector<string> > packets = aux::http_packets (data_folder.c_str());
		auto_ptr<vector<aux::output_packet> > out_packets = aux::get_output_packets ((hOBJECT)g_root, data_folder.c_str());
		auto_ptr<vector<hIO> > control_files = aux::get_control_files ((hOBJECT)g_root, data_folder.c_str());

		CONFIGURE(10,500,false)

		mc.setEachPacketSignalling (true);

		vector<string>::iterator it = packets->begin();
		vector<aux::output_packet>::iterator it2 = out_packets->begin();

		CTrafficMonitor::SkipThatQuery& skip_filter = mc.GetSkipThatQuery();
		string url;
		string content_type;
		skip_filter.Activate();


		SetEvent(mc.hAboutToCompletePacketEvent);
		TrafficSender::Enqueue(p, it->c_str(), it->c_str() + it->size(), aux::SourceToDirection(it2->dsDataTarget), false);
		//request
		skip_filter.GetRequest (url, content_type);
		_ASSERTE (content_type == "" && url == "http://fake/fake.txt");
		skip_filter.GoOn (errOK_NO_DECIDERS);
		WaitForSingleObject(mc.hPacketCompletedEvent, INFINITE);

		++it; ++it2;
		SetEvent(mc.hAboutToCompletePacketEvent);
		TrafficSender::Enqueue(p, it->c_str(), it->c_str() + it->size(), aux::SourceToDirection(it2->dsDataTarget), false);
		//response
		skip_filter.GetRequest (url, content_type);
		_ASSERTE (content_type == "" && url == "");
		skip_filter.GoOn (errOK_NO_DECIDERS);
		WaitForSingleObject(mc.hPacketCompletedEvent, INFINITE);

		++it; ++it2;
		SetEvent(mc.hAboutToCompletePacketEvent);
		TrafficSender::Enqueue(p, it->c_str(), it->c_str() + it->size(), aux::SourceToDirection(it2->dsDataTarget), false);
		//request
		skip_filter.GetRequest (url, content_type);
		_ASSERTE (content_type == "" && url == "http://fake/fake2.txt");
		skip_filter.GoOn (errOK_NO_DECIDERS);
		WaitForSingleObject(mc.hPacketCompletedEvent, INFINITE);

		++it; ++it2;
		SetEvent(mc.hAboutToCompletePacketEvent);
		TrafficSender::Enqueue(p, it->c_str(), it->c_str() + it->size(), aux::SourceToDirection(it2->dsDataTarget), false);
		//response
		skip_filter.GetRequest (url, content_type);
		_ASSERTE (content_type == "" && url == "");
		skip_filter.GoOn (errOK_NO_DECIDERS);
		WaitForSingleObject(mc.hPacketCompletedEvent, INFINITE);

		eng.VerifyCheckedObjects (control_files.get());
		mc.VerifyConnStream (out_packets.get());
	}
	{//case2
		DECLARE_HTTP_CLASSES
		boost::shared_ptr<aux::processor_proxy> p (new aux::processor_proxy (strategy, proxy, analyzer));

		auto_ptr<vector<string> > packets = aux::http_packets (data_folder.c_str());
		auto_ptr<vector<aux::output_packet> > out_packets = aux::get_output_packets ((hOBJECT)g_root, data_folder.c_str());
		auto_ptr<vector<hIO> > control_files = aux::get_control_files ((hOBJECT)g_root, data_folder.c_str());

		CONFIGURE(10,500,false)

		mc.setEachPacketSignalling (true);

		vector<string>::iterator it = packets->begin();
		vector<aux::output_packet>::iterator it2 = out_packets->begin();

		CTrafficMonitor::SkipThatQuery& skip_filter = mc.GetSkipThatQuery();
		string url;
		string content_type;
		skip_filter.Activate();


		SetEvent(mc.hAboutToCompletePacketEvent);
		TrafficSender::Enqueue(p, it->c_str(), it->c_str() + it->size(), aux::SourceToDirection(it2->dsDataTarget), false);
		//request
		skip_filter.GetRequest (url, content_type);
		_ASSERTE (content_type == "" && url == "http://fake/fake.txt");
		skip_filter.GoOn (errOK_DECIDED);
		WaitForSingleObject(mc.hPacketCompletedEvent, INFINITE);

		++it; ++it2;
		SetEvent(mc.hAboutToCompletePacketEvent);
		TrafficSender::Enqueue(p, it->c_str(), it->c_str() + it->size(), aux::SourceToDirection(it2->dsDataTarget), false);
		//response
		skip_filter.GetRequest (url, content_type);
		_ASSERTE (content_type == "" && url == "");
		skip_filter.GoOn (errOK_NO_DECIDERS);
		WaitForSingleObject(mc.hPacketCompletedEvent, INFINITE);

		++it; ++it2;
		SetEvent(mc.hAboutToCompletePacketEvent);
		TrafficSender::Enqueue(p, it->c_str(), it->c_str() + it->size(), aux::SourceToDirection(it2->dsDataTarget), false);
		//request
		skip_filter.GetRequest (url, content_type);
		_ASSERTE (content_type == "" && url == "http://fake/fake2.txt");
		skip_filter.GoOn (errOK_NO_DECIDERS);
		WaitForSingleObject(mc.hPacketCompletedEvent, INFINITE);

		++it; ++it2;
		SetEvent(mc.hAboutToCompletePacketEvent);
		TrafficSender::Enqueue(p, it->c_str(), it->c_str() + it->size(), aux::SourceToDirection(it2->dsDataTarget), false);
		//response
		skip_filter.GetRequest (url, content_type);
		_ASSERTE (content_type == "" && url == "");
		skip_filter.GoOn (errOK_DECIDED);
		WaitForSingleObject(mc.hPacketCompletedEvent, INFINITE);

		control_files->erase (control_files->rbegin().base());
		eng.VerifyCheckedObjects (control_files.get());
		mc.VerifyConnStream (out_packets.get());
	}

}

template<EngineType T> void TestHTTP<T>::SendDataWithInstantOutputControl (const char* data, size_t length, CTrafficMonitor& mc,
																		   boost::shared_ptr<aux::processor_proxy> p,
																		   vector<aux::output_packet>* out_packets_cont,
																		   HANDLE hSentDataEvent,
																		   size_t pause,
																		   std::string& this_answer,
																		   bool use_chunked_encoding)
{
	size_t bytes_remain = length;

	if (use_chunked_encoding)
	{
		string null_chunk ("0\015\012\015\012");
		//отправляем чанки данных
		while (bytes_remain)
		{
			int bytes_to_send = std::min ((long)bytes_remain, aux::rnd_chunk_size2(1)/*bytes_remain*/);
			string chunk = makeChunk (data, bytes_to_send, false /*no comments*/);
			data += bytes_to_send;
			bytes_remain -= bytes_to_send;
			TrafficSender::Enqueue(p, chunk.c_str(), chunk.c_str() + chunk.size(), to_client, false, hSentDataEvent);
			WaitForSingleObject (hSentDataEvent, INFINITE);
			this_answer.append (chunk);
			out_packets_cont->back().AddData(chunk.c_str(), chunk.size());
			mc.VerifyConnStream (out_packets_cont);
			Sleep ( aux::rnd_chunk_size2(pause) );
		}

		//последний - нулевой
		SetEvent(mc.hAboutToCompletePacketEvent);
		TrafficSender::Enqueue(p, null_chunk.c_str(), null_chunk.c_str() + null_chunk.size(), to_client, false);
		WaitForSingleObject(mc.hPacketCompletedEvent, INFINITE);
		this_answer.append (null_chunk);
	}
	else
	{
		//отправляем данные
		while (bytes_remain)
		{
			int bytes_to_send = std::min ((long)bytes_remain, /*aux::rnd_chunk_size2(it->size())*/ 1);
			if (bytes_to_send == bytes_remain)
			{
				SetEvent(mc.hAboutToCompletePacketEvent);
				TrafficSender::Enqueue(p, data, data + bytes_to_send, to_client, false, hSentDataEvent);
				WaitForSingleObject(mc.hPacketCompletedEvent, INFINITE);
				out_packets_cont->back().AddData(data, bytes_to_send);
				WaitForSingleObject(hSentDataEvent, INFINITE);
			} else
			{
				TrafficSender::Enqueue(p, data, data + bytes_to_send, to_client, false, hSentDataEvent);
				out_packets_cont->back().AddData(data, bytes_to_send);
				WaitForSingleObject(hSentDataEvent, INFINITE);
			}

			_ASSERTE ( mc.VerifyConnStream (out_packets_cont) );

			data += bytes_to_send;
			bytes_remain -= bytes_to_send;	
			Sleep ( aux::rnd_chunk_size2(pause) );
		}
	}
}

template<EngineType T> void TestHTTP<T>::test_skip_that_no_traffic_delay (const std::string& fld)
{
	ENTRY
	DECLARE_HTTP_CLASSES
	boost::shared_ptr<aux::processor_proxy> p (new aux::processor_proxy (strategy, proxy, analyzer));

	HANDLE hSentDataEvent = CreateEvent (NULL, false, false, NULL);
	auto_ptr<vector<aux::output_packet> > out_packets ( new vector<aux::output_packet>() );
	auto_ptr<vector<aux::output_packet> > out_packets_cont ( new vector<aux::output_packet>() );
	//auto_ptr<vector<hIO> > control_files = aux::get_control_files ((hOBJECT)g_root, data_folder.c_str());
	auto_ptr<vector<hIO> > control_files ( new vector<hIO>() );

	mc.setEachPacketSignalling (true);

	CTrafficMonitor::SkipThatQuery& skip_filter = mc.GetSkipThatQuery();
	string url;
	string content_type;
	skip_filter.Activate();

	string get_request ("GET http://fake/fake.txt HTTP/1.0\015\012\015\012");
	string answer_header ("HTTP/1.1 200 OK\015\012Content-Length: ");
	auto_ptr<vector<string> > packets = aux::http_packets (data_folder.c_str());

	vector<string>::iterator it;

	for (it = packets->begin(); it != packets->end(); ++it)
	{
		char buf[10];
		string this_answer_heaader (answer_header);
		this_answer_heaader.append (itoa(it->size(), buf, 10)). append("\015\012\015\012");
		//sending get
		SetEvent(mc.hAboutToCompletePacketEvent);
		TrafficSender::Enqueue(p, get_request.c_str(), get_request.c_str() + get_request.size(), to_server, false);

		skip_filter.GetRequest (url, content_type);
		skip_filter.GoOn (errOK_NO_DECIDERS);
		WaitForSingleObject(mc.hPacketCompletedEvent, INFINITE);

		ADD_NEW_PACKET(out_packets,get_request,dsClient,psKeepAlive)
		ADD_NEW_PACKET(out_packets,get_request,dsClient,psKeepAlive)

		//sending response
		TrafficSender::Enqueue(p, this_answer_heaader.c_str(), this_answer_heaader.c_str() + this_answer_heaader.size(), to_client, false);
		out_packets_cont->push_back (aux::output_packet
			(this_answer_heaader, TrafficProtocoller::dsClient, TrafficProtocoller::psKeepAlive) );


		//говорим, что пропускать response без проверок и задержек
		skip_filter.GetRequest (url, content_type);
		_ASSERTE (content_type == "" && url == "");
		skip_filter.GoOn (errOK_DECIDED);

		TestHTTP<T>::SendDataWithInstantOutputControl(it->c_str(), it->size(), mc, p, out_packets_cont.get(), hSentDataEvent,50);

		//добавим в массив сравнения сообщение
		ADD_NEW_PACKET(out_packets,string(this_answer_heaader).append(it->c_str()),dsClient,psKeepAlive)

	}

	_ASSERTE ( eng.VerifyCheckedObjects (control_files.get()) );
	_ASSERTE ( mc.VerifyConnStream (out_packets.get()) );
	_ASSERTE ( mc.VerifyConnStream (out_packets_cont.get()) );

}


template<EngineType T> void TestHTTP<T>::test_skip_that_no_traffic_delay_chunked (const std::string& fld)
{
	ENTRY
	DECLARE_HTTP_CLASSES
	boost::shared_ptr<aux::processor_proxy> p (new aux::processor_proxy (strategy, proxy, analyzer, 100));

	HANDLE hSentDataEvent = CreateEvent (NULL, false, false, NULL);
	auto_ptr<vector<aux::output_packet> > out_packets ( new vector<aux::output_packet>() );
	auto_ptr<vector<aux::output_packet> > out_packets_cont ( new vector<aux::output_packet>() );
	//auto_ptr<vector<hIO> > control_files = aux::get_control_files ((hOBJECT)g_root, data_folder.c_str());
	auto_ptr<vector<hIO> > control_files ( new vector<hIO>() );

	mc.setEachPacketSignalling (true);

	CTrafficMonitor::SkipThatQuery& skip_filter = mc.GetSkipThatQuery();
	string url;
	string content_type;
	skip_filter.Activate();

	string get_request ("GET http://fake/fake.txt HTTP/1.0\015\012\015\012");
	string answer_header ("HTTP/1.1 200 OK\015\012Transfer-Encoding: chunked\015\012\015\012");
	string null_chunk ("0\015\012\015\012");

	auto_ptr<vector<string> > packets = aux::http_packets (data_folder.c_str());

	vector<string>::iterator it;

	for (it = packets->begin(); it != packets->end(); ++it)
	{
		string this_answer (answer_header);
		//sending get
		SetEvent(mc.hAboutToCompletePacketEvent);
		TrafficSender::Enqueue(p, get_request.c_str(), get_request.c_str() + get_request.size(), to_server, false);
		//говорим ...
		skip_filter.GetRequest (url, content_type);
		skip_filter.GoOn (errOK_NO_DECIDERS);
		WaitForSingleObject(mc.hPacketCompletedEvent, INFINITE);

		ADD_NEW_PACKET(out_packets,get_request,dsClient,psKeepAlive)
		ADD_NEW_PACKET(out_packets_cont,get_request,dsClient,psKeepAlive)

		//sending response
		TrafficSender::Enqueue(p, this_answer.c_str(), this_answer.c_str() + this_answer.size(), to_client, false);
		out_packets_cont->push_back (aux::output_packet
			(this_answer, TrafficProtocoller::dsClient, TrafficProtocoller::psKeepAlive) );

		//говорим, что пропускать response без проверок и задержек
		skip_filter.GetRequest (url, content_type);
		_ASSERTE (content_type == "" && url == "");
		skip_filter.GoOn (errOK_DECIDED);

		TestHTTP<T>::SendDataWithInstantOutputControl(it->c_str(), it->size(), mc, p, out_packets_cont.get(), hSentDataEvent, 500, this_answer, true);

		//добавим в массив сравнения сообщение
		this_answer = CTrafficMonitor::RebuildPacket (this_answer);
		out_packets_cont->back().SetData(this_answer.c_str(), this_answer.size());
		ADD_NEW_PACKET(out_packets,this_answer,dsClient,psKeepAlive)

	}

	_ASSERTE ( eng.VerifyCheckedObjects (control_files.get()) );
	_ASSERTE ( mc.VerifyConnStream (out_packets.get()) );
	_ASSERTE ( mc.VerifyConnStream (out_packets_cont.get()) );

}


template<EngineType T> void TestHTTP<T>::test_skip_message_on_request (const std::string& fld)
{
	ENTRY
	{//case 1
		DECLARE_HTTP_CLASSES
		boost::shared_ptr<aux::processor_proxy> p (new aux::processor_proxy (strategy, proxy, analyzer));

		auto_ptr<vector<string> > packets = aux::http_packets (data_folder.c_str());
		auto_ptr<vector<aux::output_packet> > out_packets = aux::get_output_packets ((hOBJECT)g_root, data_folder.c_str());
		auto_ptr<vector<hIO> > control_files = aux::get_control_files ((hOBJECT)g_root, data_folder.c_str());

		CONFIGURE(10,500,false)

		mc.setEachPacketSignalling (true);
		mc.ExpectNewDetectRequest();//переход в режим детекта

		vector<string>::iterator it = packets->begin();
		vector<aux::output_packet>::iterator it2 = out_packets->begin();

		PROCESS_PACKET_ASYNC(p, it->c_str(), it->c_str() + it->size(), aux::SourceToDirection(it2->dsDataTarget), false);

		eng.VerifyCheckedObjects (control_files.get());
		mc.VerifyConnStream (out_packets.get());
	}
}

template<EngineType T> void TestHTTP<T>::test_skip_message_on_response (const std::string& fld)
{
	ENTRY
	DECLARE_HTTP_CLASSES
	boost::shared_ptr<aux::processor_proxy> p (new aux::processor_proxy (strategy, proxy, analyzer, 100));

	HANDLE hSentDataEvent = CreateEvent (NULL, false, false, NULL);
	auto_ptr<vector<aux::output_packet> > out_packets ( new vector<aux::output_packet>() );
	auto_ptr<vector<aux::output_packet> > out_packets_cont ( new vector<aux::output_packet>() );
	auto_ptr<vector<hIO> > control_files ( new vector<hIO>() );

	mc.setEachPacketSignalling (true);

	string get_request ("GET http://fake/fake.txt HTTP/1.0\015\012\015\012");
	string answer_header ("HTTP/1.1 200 OK\015\012Content-Type: video/x-ms-asf\015\012Content-Length: ");
	auto_ptr<vector<string> > packets = aux::http_packets (data_folder.c_str());

	vector<string>::iterator it = packets->begin();

		char buf[10];
		string this_answer_heaader (answer_header);
		this_answer_heaader.append (itoa(it->size(), buf, 10)). append("\015\012\015\012");
		//sending get
		PROCESS_PACKET_ASYNC(p, get_request.c_str(), get_request.c_str() + get_request.size(), to_server, false);

		ADD_NEW_PACKET(out_packets,get_request,dsClient,psKeepAlive)
		ADD_NEW_PACKET(out_packets,get_request,dsClient,psKeepAlive)

		//sending response header
		mc.ExpectNewDetectRequest();//переход в режим детекта
		PROCESS_PACKET_ASYNC(p, this_answer_heaader.c_str(), this_answer_heaader.c_str() + this_answer_heaader.size(), to_client, false);
		out_packets_cont->push_back (aux::output_packet
			(this_answer_heaader, TrafficProtocoller::dsClient, TrafficProtocoller::psKeepAlive) );
		
		//протоколлер отказался от трафика,
		//отгружаем остатки трафика в анализатор, тот будет отказываться от него
		_ASSERTE (!mc.bNewDetectRequestExpected);
		p->HttpModeRst();
		int bytes_remain = it->size();
		const char* data = it->c_str();
		while (bytes_remain)
		{
			int bytes_to_send = std::min ((long)bytes_remain, 10);
			if (bytes_to_send == bytes_remain)
			{
				SetEvent(mc.hAboutToCompletePacketEvent);
				TrafficSender::Enqueue(p, data, data + bytes_to_send, to_client, false, hSentDataEvent);
				WaitForSingleObject(hSentDataEvent, INFINITE);
			} else
			{
				TrafficSender::Enqueue(p, data, data + bytes_to_send, to_client, false, hSentDataEvent);
				WaitForSingleObject(hSentDataEvent, INFINITE);
			}

			_ASSERTE ( mc.VerifyConnStream (out_packets_cont.get()) );

			data += bytes_to_send;
			bytes_remain -= bytes_to_send;	
			Sleep ( aux::rnd_chunk_size2(50) );
		}
		p->flush();
		//
		
		//добавим в массив сравнения сообщение
		ADD_NEW_PACKET(out_packets,this_answer_heaader,dsClient,psKeepAlive)

	_ASSERTE ( eng.VerifyCheckedObjects (control_files.get()) );
	_ASSERTE ( mc.VerifyConnStream (out_packets.get()) );
	_ASSERTE ( mc.VerifyConnStream (out_packets_cont.get()) );

	//теперь  пихаем в анализатор "нормальный" request/response, должно все отработать корректно

	string get_request2 ("GET http://fake/fake2.txt HTTP/1.0\015\012\015\012");
	string answer2 ("HTTP/1.1 200 OK\015\012Content-Type: text/html\015\012Content-Length: 10\015\012\015\012qwertyuiop");

	//sending request 2
	PROCESS_PACKET_ASYNC(p, get_request2.c_str(), get_request2.c_str() + get_request2.size(), to_client, false);
	ADD_NEW_PACKET(out_packets,get_request2,dsClient,psKeepAlive)

	//getting response 2
	PROCESS_PACKET_ASYNC(p, answer2.c_str(), answer2.c_str() + answer2.size(), to_client, false);
	ADD_NEW_PACKET(out_packets,answer2,dsServer,psKeepAlive)

	auto_ptr<vector<hIO> > control_files2 = aux::get_control_files ((hOBJECT)g_root, data_folder.c_str());
	_ASSERTE ( eng.VerifyCheckedObjects (control_files2.get()) );
	_ASSERTE ( mc.VerifyConnStream (out_packets.get()) );
}


template<EngineType T> void TestHTTP<T>::test_skip_message_on_response_chunked (const std::string& fld)
{
	ENTRY
	DECLARE_HTTP_CLASSES
	boost::shared_ptr<aux::processor_proxy> p (new aux::processor_proxy (strategy, proxy, analyzer, 100));

	HANDLE hSentDataEvent = CreateEvent (NULL, false, false, NULL);
	auto_ptr<vector<aux::output_packet> > out_packets ( new vector<aux::output_packet>() );
	auto_ptr<vector<aux::output_packet> > out_packets_cont ( new vector<aux::output_packet>() );
	auto_ptr<vector<hIO> > control_files ( new vector<hIO>() );

	mc.setEachPacketSignalling (true);

	string get_request ("GET http://fake/fake.txt HTTP/1.0\015\012\015\012");
	string answer_header ("HTTP/1.1 200 OK\015\012Content-Type: video/x-ms-asf\015\012Transfer-Encoding: chunked");
	auto_ptr<vector<string> > packets = aux::http_packets (data_folder.c_str());

	vector<string>::iterator it = packets->begin();

	string this_answer_heaader (answer_header);
	this_answer_heaader.append("\015\012\015\012");
	//sending get
	SetEvent(mc.hAboutToCompletePacketEvent);
	TrafficSender::Enqueue(p, get_request.c_str(), get_request.c_str() + get_request.size(), to_server, false);
	WaitForSingleObject(mc.hPacketCompletedEvent, INFINITE);

	ADD_NEW_PACKET(out_packets,get_request,dsClient,psKeepAlive)
	ADD_NEW_PACKET(out_packets,get_request,dsClient,psKeepAlive)

	//sending response header
	mc.ExpectNewDetectRequest();//переход в режим детекта
	PROCESS_PACKET_ASYNC(p, this_answer_heaader.c_str(), this_answer_heaader.c_str() + this_answer_heaader.size(), to_client, false);
	out_packets_cont->push_back (aux::output_packet
		(this_answer_heaader, TrafficProtocoller::dsClient, TrafficProtocoller::psKeepAlive) );

	//протоколлер отказался от трафика,
	//отгружаем остатки трафика в анализатор, тот будет отказываться от него
	_ASSERTE (!mc.bNewDetectRequestExpected);
	p->HttpModeRst();
	int bytes_remain = it->size();
	const char* data = it->c_str();
	string null_chunk ("0\015\012\015\012");
	//отправляем чанки данных
	while (bytes_remain)
	{
		int bytes_to_send = std::min ((long)bytes_remain, aux::rnd_chunk_size2(10)/*bytes_remain*/);
		string chunk = makeChunk (data, bytes_to_send, false /*no comments*/);
		data += bytes_to_send;
		bytes_remain -= bytes_to_send;
		TrafficSender::Enqueue(p, chunk.c_str(), chunk.c_str() + chunk.size(), to_client, false, hSentDataEvent);
		WaitForSingleObject (hSentDataEvent, INFINITE);
		mc.VerifyConnStream (out_packets_cont.get());
		Sleep ( aux::rnd_chunk_size2(50) );
	}
	p->flush();
	//

	//добавим в массив сравнения сообщение
	ADD_NEW_PACKET(out_packets,this_answer_heaader,dsClient,psKeepAlive)

	_ASSERTE ( eng.VerifyCheckedObjects (control_files.get()) );
	_ASSERTE ( mc.VerifyConnStream (out_packets.get()) );
	_ASSERTE ( mc.VerifyConnStream (out_packets_cont.get()) );

	//теперь пихаем в анализатор "нормальный" request/response, должно все отработать корректно

	string get_request2 ("GET http://fake/fake2.txt HTTP/1.0\015\012\015\012");
	string answer2 ("HTTP/1.1 200 OK\015\012Content-Type: text/html\015\012Content-Length: 10\015\012\015\012qwertyuiop");

	//sending request 2
	PROCESS_PACKET_ASYNC(p, get_request2.c_str(), get_request2.c_str() + get_request2.size(), to_client, false);
	ADD_NEW_PACKET(out_packets,get_request2,dsClient,psKeepAlive)

	//getting response 2
	PROCESS_PACKET_ASYNC(p, answer2.c_str(), answer2.c_str() + answer2.size(), to_client, false);
	ADD_NEW_PACKET(out_packets,answer2,dsServer,psKeepAlive)

	auto_ptr<vector<hIO> > control_files2 = aux::get_control_files ((hOBJECT)g_root, data_folder.c_str());
	_ASSERTE ( eng.VerifyCheckedObjects (control_files2.get()) );
	_ASSERTE ( mc.VerifyConnStream (out_packets.get()) );
}


template<EngineType T> void TestHTTP<T>::test_async_data_send_runner (const std::string& fld)
{
	//==
	_impl_test_async_data_send (fld, 50, 1000, 50);
	//>>
	_impl_test_async_data_send (fld, 10000, 1000, 50);
	//<<
	_impl_test_async_data_send (fld, 50, 10000, 2000);
}

template<EngineType T> void TestHTTP<T>::_impl_test_async_data_send (const std::string& fld, int data_portion, int data_delay, int prtcl_timeout)
{
	ENTRY
	DECLARE_HTTP_CLASSES
	aux::processor_proxy p(strategy, proxy, analyzer, prtcl_timeout);

	auto_ptr<vector<aux::output_packet> > out_packets /*= aux::get_output_packets ((hOBJECT)g_root, data_folder.c_str());*/
		( new vector<aux::output_packet>() );
	auto_ptr<vector<hIO> > control_files = aux::get_control_files ((hOBJECT)g_root, data_folder.c_str());

	mc.setEachPacketSignalling (true);

	string get_request ("GET http://fake/fake.txt HTTP/1.0\015\012\015\012");
	string answer_header ("HTTP/1.1 200 OK\015\012Content-Length: ");
	auto_ptr<vector<string> > packets = aux::http_packets (data_folder.c_str());

	vector<string>::iterator it;

	for (it = packets->begin(); it != packets->end(); ++it)
	{
		char buf[10];
		string this_answer_heaader (answer_header);
		this_answer_heaader.append (itoa(it->size(), buf, 10)). append("\015\012\015\012");
		//sending get
		PROCESS_PACKET(p,get_request.c_str(), get_request.c_str() + get_request.size(), to_server, false);
		ADD_NEW_PACKET(out_packets,get_request,dsClient,psKeepAlive)

			//sending response
			_ASSERTE(p.process(this_answer_heaader.c_str(), this_answer_heaader.c_str() + this_answer_heaader.size(), to_client, false));

		const char* data = it->c_str();
		int bytes_remain = it->size();

		//отправляем данные
		while (bytes_remain)
		{
			int bytes_to_send = std::min ((long)bytes_remain, aux::rnd_chunk_size2(data_portion));
			if (bytes_to_send == bytes_remain)
			{
				PROCESS_PACKET(p,data, data + bytes_to_send, to_client, false);
			} else _ASSERTE(p.process(data, data + bytes_to_send, to_client, false));

			data += bytes_to_send;
			bytes_remain -= bytes_to_send;	

			Sleep ( aux::rnd_chunk_size2(data_delay) );

		}

		//добавим в массив сравнения сообщение
		ADD_NEW_PACKET(out_packets,string(this_answer_heaader).append(it->c_str()),dsClient,psKeepAlive)

	}

	eng.VerifyCheckedObjects (control_files.get());
	mc.VerifyConnStream (out_packets.get());

}


template<EngineType T> void TestHTTP<T>::test_async_long_message_normal_and_timeout ()
{//"бесконечный" коннект с очень большим Content-Length

	LOG_CALL3("\n",__FUNCTION__,"\n");
	LOG_CALL("Only ASSERTS are valid in this test\n");

	int fake_cl = 100000000;
	int data_cl = 70000;

	int webav_timeout = 10000;
	unsigned long time_start;

	vector<char> v_data;
	v_data.reserve (data_cl);
	for (int i = 0; i < data_cl; ++i) v_data.push_back (aux::rnd_symbol());

	DECLARE_HTTP_CLASSES
	aux::processor_proxy p(strategy, proxy, analyzer, webav_timeout);

	auto_ptr<vector<aux::output_packet> > out_packets ( new vector<aux::output_packet>() );
	auto_ptr<vector<hIO> > control_files ( new vector<hIO>() );
	auto_ptr<vector<string> > packets ( new vector<string>() );

	mc.setEachPacketSignalling (true);

	string get_request ("GET http://fake/fake.txt HTTP/1.0\015\012\015\012");
	string answer_header ("HTTP/1.1 200 OK\015\012Content-Length: ");

	char buf[10];
	string this_answer_heaader (answer_header);
	this_answer_heaader.append (itoa(fake_cl, buf, 10)). append("\015\012\015\012");
	//sending get
	PROCESS_PACKET(p,get_request.c_str(), get_request.c_str() + get_request.size(), to_server, false);
	ADD_NEW_PACKET(out_packets,get_request,dsClient,psKeepAlive)

	//sending response
	_ASSERTE(p.process(this_answer_heaader.c_str(), this_answer_heaader.c_str() + this_answer_heaader.size(), to_client, false));
	time_start = GetTickCount();

	const char* data = &v_data[0];
	int bytes_remain = v_data.size();

	//добавим пустой пакет
	ADD_NEW_PACKET(out_packets,string(this_answer_heaader),dsClient,psKeepAlive)

	//GET ушел клиенту
	//mc.VerifyConnStream (out_packets.get());

	//отправляем данные
	while (bytes_remain)
	{
		int bytes_to_send = std::min ((long)bytes_remain, aux::rnd_chunk_size2(100/*байт*/));

		out_packets->back().AddData (data, bytes_to_send);

		_ASSERTE(p.process(data, data + bytes_to_send, to_client, false));

		data += bytes_to_send;
		bytes_remain -= bytes_to_send;	
		Sleep ( aux::rnd_chunk_size2(50/*миллисекунд*/) );
		unsigned long time_passed = GetTickCount() - time_start;
		
		if (time_passed  > webav_timeout - 1000)
		{//скоро таймаут, притормозим и подождем асинхронного вызова
			_ASSERTE (!mc.VerifyConnStream (out_packets.get()));
			Sleep (2000);
			_ASSERTE (mc.VerifyConnStream (out_packets.get()));
			//дальше по барабану
			break;
		}
	}

	_ASSERTE(eng.VerifyCheckedObjects (control_files.get()));

	p.flush ();

	//после сброса пакета по-любому все, что получено, идет на проверку
	control_files->push_back(aux::create_io_from_array (&v_data[0], v_data.size() - bytes_remain));
	_ASSERTE(eng.VerifyCheckedObjects (control_files.get()));
}

template<EngineType T> void TestHTTP<T>::test_async_long_message_chunked_and_timeout ()
{//"бесконечное" количество чанков
	LOG_CALL3("\n",__FUNCTION__,"\n");

	int data_cl = 100000;

	int webav_timeout = 10000;
	unsigned long time_start;

	vector<char> v_data;
	v_data.reserve (data_cl);
	for (int i = 0; i < data_cl; ++i) v_data.push_back (aux::rnd_symbol());

	DECLARE_HTTP_CLASSES
	aux::processor_proxy p(strategy, proxy, analyzer, webav_timeout);

	auto_ptr<vector<aux::output_packet> > out_packets ( new vector<aux::output_packet>() );
	auto_ptr<vector<hIO> > control_files ( new vector<hIO>() );
	auto_ptr<vector<string> > packets ( new vector<string>() );

	mc.setEachPacketSignalling (true);

	string get_request ("GET http://fake/fake.txt HTTP/1.0\015\012\015\012");
	string answer_header ("HTTP/1.1 200 OK\015\012Transfer-Encoding: chunked\015\012\015\012");
	string null_chunk ("0\015\012\015\012");

	char buf[10];
	//sending get
	PROCESS_PACKET(p,get_request.c_str(), get_request.c_str() + get_request.size(), to_server, false);
	ADD_NEW_PACKET(out_packets,get_request,dsClient,psKeepAlive)

	//sending response
	_ASSERTE(p.process(answer_header.c_str(), answer_header.c_str() + answer_header.size(), to_client, false));
	time_start = GetTickCount();

	const char* data = &v_data[0];
	int bytes_remain = v_data.size();
	
	string this_answer (answer_header);
	//отправляем чанки данных
	while (bytes_remain)
	{
		int bytes_to_send = std::min ((long)bytes_remain, aux::rnd_chunk_size2(100));
		string chunk = makeChunk (data, bytes_to_send);
		data += bytes_to_send;
		bytes_remain -= bytes_to_send;
		_ASSERTE(p.process(chunk.c_str(), chunk.c_str() + chunk.size(), to_client, false));
		this_answer.append (chunk);
		Sleep ( aux::rnd_chunk_size2(50) );

		unsigned long time_passed = GetTickCount() - time_start;

		if (time_passed  > webav_timeout - 1000)
		{//скоро таймаут, притормозим и подождем асинхронного вызова
			_ASSERTE (mc.VerifyConnStream (out_packets.get()));
			Sleep (2000);

			//нам скинули один здоровый чанк
			// пока без нулевого чанка
			this_answer.append (null_chunk);
			string to_client_wo_null = CTrafficMonitor::RebuildPacket(this_answer);
			to_client_wo_null.erase(to_client_wo_null.size()-5);
			ADD_NEW_PACKET(out_packets,to_client_wo_null,dsClient,psKeepAlive);
			_ASSERTE (mc.VerifyConnStream (out_packets.get()));

			//заканчиваем передачу, отдаем null-chunk и получаем его
			PROCESS_PACKET(p,null_chunk.c_str(), null_chunk.c_str() + null_chunk.size(), to_client, false)
			out_packets->back().AddData (null_chunk.c_str(), null_chunk.size());

			//че приходило на проверку в движок?
			control_files->push_back(aux::create_io_from_array (&v_data[0], v_data.size() - bytes_remain));
			_ASSERTE(eng.VerifyCheckedObjects (control_files.get()));

			break;
		}
	}
}

template<EngineType T> void TestHTTP<T>::test_connections ()
{
	const int count = 50;
	_data_test_connections par (10000,100,50,50);

	HANDLE hThreads[count];
	for (int i = 0; i < count; ++i) 
		hThreads[i] = (HANDLE)_beginthreadex(NULL, 0, TestHTTP<T>::_impl_test_connections,&par,NULL,NULL);

	WaitForMultipleObjects(count, &hThreads[0], true, INFINITE);

	for (int i = 0; i < count; ++i) 
		CloseHandle (hThreads[i]);
}

template<EngineType T> unsigned TestHTTP<T>::_impl_test_connections (void * params)
{
	LOG_CALL3("\n",__FUNCTION__,"\n");

	_data_test_connections* dtc =  reinterpret_cast<_data_test_connections*>(params);

	vector<char> v_data;
	v_data.reserve (dtc->data_amount);
	for (int i = 0; i < dtc->data_amount; ++i) v_data.push_back (aux::rnd_symbol());

	DECLARE_HTTP_CLASSES
	aux::processor_proxy p(strategy, proxy, analyzer, dtc->prtcl_timeout/*миллисекунд*/);

	auto_ptr<vector<aux::output_packet> > out_packets ( new vector<aux::output_packet>() );
	auto_ptr<vector<hIO> > control_files ( new vector<hIO>() );
	auto_ptr<vector<string> > packets ( new vector<string>() );

	mc.setEachPacketSignalling (true);

	string get_request ("GET http://fake/fake.txt HTTP/1.0\015\012\015\012");
	string answer_header ("HTTP/1.1 200 OK\015\012Content-Length: ");

	char buf[10];
	string this_answer_heaader (answer_header);
	this_answer_heaader.append (itoa(dtc->data_amount, buf, 10)). append("\015\012\015\012");
	//sending get
	PROCESS_PACKET(p,get_request.c_str(), get_request.c_str() + get_request.size(), to_server, false);
	ADD_NEW_PACKET(out_packets,get_request,dsClient,psKeepAlive)

	//sending response
	_ASSERTE(p.process(this_answer_heaader.c_str(), this_answer_heaader.c_str() + this_answer_heaader.size(), to_client, false));

	const char* data = &v_data[0];
	int bytes_remain = v_data.size();

	//добавим пустой пакет
	ADD_NEW_PACKET(out_packets,string(this_answer_heaader),dsClient,psKeepAlive)

	//отправляем данные
	while (bytes_remain)
	{
		int bytes_to_send = std::min ((long)bytes_remain, aux::rnd_chunk_size2(dtc->data_portion/*байт*/));

		out_packets->back().AddData (data, bytes_to_send);

		if (bytes_to_send == bytes_remain)
		{
			PROCESS_PACKET(p,data, data + bytes_to_send, to_client, false);
		} else _ASSERTE(p.process(data, data + bytes_to_send, to_client, false));

		data += bytes_to_send;
		bytes_remain -= bytes_to_send;	
		Sleep ( aux::rnd_chunk_size2(dtc->data_delay/*миллисекунд*/) );

		//тут надо проверить, что переданы все данные
		//mc.VerifyConnStream (out_packets.get());
	}

	_ASSERTE (mc.VerifyConnStream (out_packets.get()) );
	control_files->push_back(aux::create_io_from_array (&v_data[0], v_data.size()));
	_ASSERTE (eng.VerifyCheckedObjects (control_files.get()) );

	return 0;
}


//------------------------------------------------------------------

template<EngineType T> void TestHTTP<T>::test_header_timeout ()
{//"пауза" на передаче заголовка

	LOG_CALL3("\n",__FUNCTION__,"\n");

	int data_length = 1024;

	int webav_timeout = 10000;
	unsigned long time_start;

	vector<char> v_data;
	v_data.reserve (data_length);
	for (int i = 0; i < data_length; ++i) v_data.push_back (aux::rnd_symbol());

	DECLARE_HTTP_CLASSES
	aux::processor_proxy p(strategy, proxy, analyzer, webav_timeout);

	auto_ptr<vector<aux::output_packet> > out_packets ( new vector<aux::output_packet>() );
	auto_ptr<vector<hIO> > control_files ( new vector<hIO>() );
	auto_ptr<vector<string> > packets ( new vector<string>() );

	mc.setEachPacketSignalling (true);

	string get_request ("GET http://fake/fake.txt HTTP/1.0\015\012\015\012");
	string answer_header_part1 ("HTTP/1.1 200 OK\015\012Field1: Value1\015\012Field");
	string answer_header_part2 ("2: Value2\015\012Field3: Value3\015\012Content-Length: ");
	char buf[10];
	answer_header_part2.append (itoa(data_length, buf, 10)). append("\015\012\015\012");

	//sending get
	PROCESS_PACKET(p,get_request.c_str(), get_request.c_str() + get_request.size(), to_server, false);
	ADD_NEW_PACKET(out_packets,get_request,dsClient,psKeepAlive)

	//sending response header part 1
	_ASSERTE(p.process(answer_header_part1.c_str(), answer_header_part1.c_str() + answer_header_part1.size(), to_client, false));
	time_start = GetTickCount();
	Sleep (webav_timeout - 1000);

	_ASSERTE( mc.VerifyConnStream (out_packets.get()) );
	Sleep (5000);
	string part2(answer_header_part2);
	part2.append(&v_data[0], v_data.size());
	PROCESS_PACKET(p,part2.c_str(), part2.c_str() + part2.size(), to_client, false);

	ADD_NEW_PACKET(out_packets,string(answer_header_part1).append(part2),dsClient,psKeepAlive)
	_ASSERTE (mc.VerifyConnStream (out_packets.get()));

	control_files->push_back(aux::create_io_from_array (&v_data[0], v_data.size() ));
	_ASSERTE(eng.VerifyCheckedObjects (control_files.get()));
	
}
