#if !defined(__AUX_H)
#define __AUX_H

#include <windows.h>
#include <io.h>

#include <string>
#include <memory>
#include <fstream>
#include <vector>

#include <utils/prague/prague.hpp>
#include <utils/prague/io.hpp>
#include <utils/prague/io_helpers.h>

#include <Prague/prague.h>
#include <Prague/iface/i_io.h>
#include <avpsize_t.h>

#include "../../../../trafficprotocoller.h"

#include <utils/local_mutex.hpp>

using namespace TrafficProtocoller;

//#include <boost/noncopyable.hpp>
//#include <utils/local_mutex.hpp>




namespace aux
{
	struct output_packet;

	typedef util::scoped_handle<long, int (*)(intptr_t), &_findclose, 0, -1> find_handle;


	std::string read_str( const std::string& filename );
	std::auto_ptr<std::string> getFileImage( const std::string& filename );
	std::auto_ptr<std::vector<std::string> > get_raw_packets(char const* src_dir);
	std::auto_ptr<std::vector<aux::output_packet> > get_output_packets (hOBJECT g_root, char const* src_dir);
	std::auto_ptr<std::vector<hIO> > get_control_files (hOBJECT g_root, char const* src_dir);
	std::auto_ptr<std::string> aux::getFileImage( const std::string& filename );
	void ReadPacketsDescription (std::vector<aux::output_packet>* packets, const std::string& src_dir, const std::string& file);

	size_t rnd_chunk_size (int limit);

	hIO create_io_from_array (const void* stream, size_t size);
	__declspec(dllexport) void pump_io_to_array (hIO io, std::vector<byte>& stream);
	__declspec(dllexport) void pump_io_to_io (hIO io_source, hIO io_dest);
	hIO create_io_from_disk (const std::string& filename);
	data_target_t SourceToTarget (const data_source_t& source);

	std::string detectCodeToString (detect_code_t t);
	std::string prResultToString (tERROR error);
	std::string processStatusToString (process_status_t p);
	std::string noRN (const std::string& strok);

	bool isIoEquals (hIO left_io, hIO right_io);

	
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
/*	void AddData (const void* data, size_t length)
	{
		tDWORD result;
		tQWORD size_source = 0;
		hData->GetSize ( &size_source, IO_SIZE_TYPE_EXPLICIT);
		hData->SeekWrite (&result, size_source, (tPTR)data, length);
	}
*/
	};

	

	class TraceFile
	{
	public:
		__declspec(dllexport) TraceFile (const std::string& filename);
		__declspec(dllexport) ~TraceFile ();

		__declspec(dllexport) void Write (hIO io);
		__declspec(dllexport) void Write (const char* begin, const char* end);
		__declspec(dllexport) void Write (const char* begin);
		__declspec(dllexport) void Flush ();

	private:
		FILE* file;

		sync::local_mutex m_guard;
	};
}














#endif