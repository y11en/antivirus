#if !defined(__AUX_H)
#define __AUX_H

#define NOMINMAX

#include <list>
#include <memory>
#include <string>
#include <vector>

#include <windows.h>
#include <io.h>
#include <sys/stat.h>
#include <algorithm>
#include <numeric>
#include "_aux.h"
#include <utils/prague/scoped_handle.hpp>

#include <Prague/prague.h>
#include <Prague/iface/i_io.h>
#include <avpsize_t.h>

#include "../../../../trafficprotocoller.h"

#include <boost/noncopyable.hpp>
#include <utils/local_mutex.hpp>
#include <utils/prague/prague.hpp>
#include <utils/prague/io.hpp>
#include <utils/prague/io_helpers.h>

#include <HttpProtocoller/proc_strategy.h>
#include <HttpProtocoller/cached_strategy.h>
#include <HttpProtocoller/stream_strategy.h>
#include <HttpProtocoller/service_proxy.h>
#include <HttpProtocoller/session.h>
#include <HttpProtocoller/processor.h>
#include <http/analyzerimpl.h>

#include "Tracer.h"

using namespace http;
using namespace std;

///////////////////////////////////////////////////////////////////////////////
namespace aux
{
	///////////////////////////////////////////////////////////////////////////////
	typedef util::scoped_handle<long, int (*)(intptr_t), &_findclose, 0, -1> find_handle;
	typedef util::scoped_handle<HANDLE, BOOL (WINAPI*)(HANDLE), &CloseHandle, 0, 0> memmap_handle;
	typedef util::scoped_handle<FILE*, int (*)(FILE*), &fclose, 0, 0> file_t;
	typedef util::scoped_handle<char*, BOOL (WINAPI*)(void const*), &UnmapViewOfFile, 0, 0> map_mem_ptr;

	///////////////////////////////////////////////////////////////////////////////
	//! return list of files that contain http packet
	std::auto_ptr<std::vector<std::string> > http_packets(char const* src_dir);
	//! return pair of mapped memory pointer & size of memory
	std::pair<char*, size_t> create_http_stream(std::string const& f);
	std::pair<char*, size_t> create_http_stream(std::list<std::string> const& l);
	void pump_io_to_array (hIO io, std::vector<byte>& stream);
	hIO create_io_from_array (void* stream, size_t size);

	msg_direction SourceToDirection (const TrafficProtocoller::data_source_t& source);

	struct output_packet
	{
		output_packet (const std::string& filename, const TrafficProtocoller::data_target_t& target, const TrafficProtocoller::process_status_t& status, bool _flush = false):
						filename (filename), psStatus(status), dsDataTarget(target), bFlush(_flush)
						{
							cStringObj hObjectName ( filename.c_str() );
							cIOObj hIo	((hOBJECT)g_root, cAutoString(hObjectName), fACCESS_READ | fACCESS_FORCE_READ, 
										fOMODE_OPEN_IF_EXIST , PID_NATIVE_FIO );
							if (PR_FAIL(hIo.last_error()))
							{
								prague::IO<char> packet(prague::create_temp_io());
								tDWORD result;
								packet.get()->SeekWrite (&result, 0, (tPTR)filename.c_str(), filename.size());
								hData = packet.release();
							}
							else hData = hIo.relinquish();

							tQWORD size;
							hData->GetSize ( &size, IO_SIZE_TYPE_EXPLICIT);
							_ASSERTE (size);
						}

		hIO hData;
		TrafficProtocoller::data_target_t dsDataTarget;
		TrafficProtocoller::process_status_t psStatus;
		std::string filename;
		bool bFlush;

		void GetData (std::vector<byte>& data) const
		{
			pump_io_to_array (hData, data);
		}
		void AddData (const void* data, size_t length)
		{
			tDWORD result;
			tQWORD size_source = 0;
			hData->GetSize ( &size_source, IO_SIZE_TYPE_EXPLICIT);
			hData->SeekWrite (&result, size_source, (tPTR)data, length);
		}
		void SetData (const void* data, size_t length)
		{
			hData->SetSize(0);
			AddData (data, length);
		}


	};

	class TraceFile
	{
	public:
		TraceFile (const std::string& filename);
		~TraceFile ();

		void Write (hIO io);
		void Write (const char* begin, const char* end);
		void Write (const char* begin);
		void Flush ();

	private:
		FILE* file;

		sync::local_mutex m_guard;
	};

	class processor_proxy
	{
	public:
		processor_proxy (std::auto_ptr<processing_strategy> strategy, std::auto_ptr<session> proxy,
						 std::auto_ptr<analyzer> analyzer, long timeout = 1000);
		bool process(char const* begin, char const* end, msg_direction direction, bool is_ssl);
		void flush();
		void HttpModeRst()
		{
			bHttpDetected = false;
		}

		bool isHttpDetected ()
		{
			return bHttpDetected;
		}

	private:
		TraceFile trace_File;
		TraceFile detect_Result;
		http::processor p;
		vector<char> traffic_cache;
		bool bHttpDetected;		
	};


	void ReadPacketsDescription (std::vector<aux::output_packet>* packets, const std::string& src_dir, const std::string& file);
	std::auto_ptr<std::vector<hIO> > get_control_files (hOBJECT g_root, char const* src_dir);
	std::auto_ptr<std::vector<output_packet> > get_output_packets (hOBJECT g_root, char const* src_dir);
	bool isIoEquals (hIO left_io, hIO right_io);
	size_t rnd_chunk_size (int limit);
	size_t rnd_chunk_size2 (int limit);
	unsigned char rnd_symbol ();
	bool DeleteDirectory (const char* dir);
	bool MakeDirectory (const char* dir);
	void MakePacketDescriptionFile (const std::string& folder, const std::string& file,
		const http::msg_direction& direction, const TrafficProtocoller::process_status_t& status);

	std::string GetCurrentThreadId();
}


#endif