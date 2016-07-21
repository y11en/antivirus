#if !defined(__TESTHTTP_H)
#define __TESTHTTP_H

#include "environment.h"
#include "engine.h"
#include "_aux.h"
#include <string>

#include "Tracer.h"

enum EngineType
{
	eng_stream,
	eng_third
};

template<EngineType> struct EngineTypeSelector
{

};

template<> struct EngineTypeSelector<eng_stream>
{
	typedef cStreamEngine engine_type_value;
	typedef http::stream_strategy strategy_type_value;

	static std::auto_ptr<processing_strategy> CreateStrategy (preprocessing* prp, hOBJECT service)
	{
		return std::auto_ptr<processing_strategy>(new strategy_type_value (prp, service));
	}
};

template<> struct EngineTypeSelector<eng_third>
{
	typedef cThirdEngine engine_type_value;
	typedef http::cached_strategy strategy_type_value;

	static std::auto_ptr<processing_strategy> CreateStrategy (preprocessing* prp, hOBJECT service)
	{
		return std::auto_ptr<processing_strategy>(new strategy_type_value (prp, service, 100));
	}
};

template<EngineType T>class TestHTTP
{
public:

	typedef typename EngineTypeSelector<T>::engine_type_value tEngine;
	typedef typename EngineTypeSelector<T>::strategy_type_value tStrategy;

	static void test_syntax (const std::string& fld);
	static void test_one_byte (const std::string& fld);
	static void test_pipelining (const std::string& fld);
	static void test_caching_response_chunks (const std::string& fld);
	static void test_caching_response_normal (const std::string& fld);
	static void test_100_continue_on_request (const std::string& fld);
	static void test_cannot_detect_length (const std::string& fld);
	static void test_chunks_cases (const std::string& fld);
	static void test_packet_after_packet (const std::string& fld);
	static void test_data_after_data (const std::string& fld);
	static void test_method_supported (const std::string& method);
	static void test_method_unsupported_middle (const std::string& method);
	static void test_method_unsupported_start (const std::string& method);
	static void test_litter (const std::string& fld);
	static void test_request_filters (const std::string& fld);
	static void test_request_filters_deny (const std::string& fld, const cEngineBase::URLFilterEnum& stop_type);
	static void test_request_avs_exchange (const std::string& fld);
	static void test_request_avs_interrupt (const std::string& fld);
	static void test_request_avs_interrupt_chunked (const std::string& fld);
	
	static void test_skip_that_no_avs (const std::string& fld);
	static void test_skip_that_no_traffic_delay (const std::string& fld);
	static void test_skip_that_no_traffic_delay_chunked (const std::string& fld);

	static void test_skip_message_on_request (const std::string& fld);
	static void test_skip_message_on_response (const std::string& fld);
	static void test_skip_message_on_response_chunked (const std::string& fld);

	//new functionality
	static void test_async_data_send_runner (const std::string& fld);
	static void test_async_long_message_normal_and_timeout();
	static void test_async_long_message_chunked_and_timeout();

	static void test_connections();

	static void test_header_timeout();
	

private:
	static void SendBytesToAnalyzer (aux::processor_proxy& p, vector<string>* packets, vector<aux::output_packet>* out_packets,
									bool finishing_flush = true);
	static string makeChunk (const char* data, int bytes_to_send, bool w_comments = true);
	static void SendDataWithInstantOutputControl (	const char* data, size_t length, CTrafficMonitor& mc,
													boost::shared_ptr<aux::processor_proxy> p,
													vector<aux::output_packet>* out_packets_cont,
													HANDLE hSentDataEvent,
													size_t pause,
													std::string& this_answer = string (""),
													bool use_chunked_encoding = false);

private:

	static void _impl_test_async_data_send (const std::string& fld, int data_portion, int data_delay, int prtcl_timeout);
	static unsigned int __stdcall _impl_test_connections (void* );

private:

	struct _data_test_connections
	{
		_data_test_connections (int p1, int p2, int p3, int p4):
							data_amount(p1), data_portion(p2), data_delay(p3), prtcl_timeout(p4) {};
		int data_amount;
		int data_portion;
		int data_delay;
		int prtcl_timeout;
	};

};



#endif