#include "_aux.h"
#include "tracer.h"
#include <algorithm>

std::string aux::read_str( const std::string& filename )
{
	std::string result;

	std::ifstream w(filename.c_str(), std::ios_base::binary);
	w.unsetf(std::ios_base::skipws);
	std::copy(std::istream_iterator< char >(w), std::istream_iterator< char >(), std::back_inserter(result));
	return result;
}

std::auto_ptr<std::string> aux::getFileImage( const std::string& filename )
{
	std::auto_ptr<std::string> result(new std::string());

	std::ifstream w(filename.c_str(), std::ios_base::binary);
	w.unsetf(std::ios_base::skipws);
	std::copy(std::istream_iterator< char >(w), std::istream_iterator< char >(), std::back_inserter(*result));
	return result;
}

std::auto_ptr<std::vector<std::string> > aux::get_raw_packets(char const* src_dir)
{
	std::auto_ptr<std::vector<std::string> > l(new std::vector<std::string>);
	_finddata_t data;
	std::string mask(src_dir);
	mask.append("\\*.dat");
	find_handle h(_findfirst(mask.c_str(), &data));

	if(h)
	{
		do
		{
			if(!(data.attrib & _A_SUBDIR))
			{
				std::string name(src_dir);
				name.append(1, '\\').append(data.name);
				l->push_back(read_str(name));
			}
		}
		while(_findnext(h.get(), &data) != -1);
	}
	return l;
}

std::auto_ptr<std::vector<aux::output_packet> > aux::get_output_packets (hOBJECT g_root, char const* src_dir)
{
	std::auto_ptr<std::vector<output_packet> > v (new std::vector<output_packet>);
	_finddata_t data;
	std::string mask(src_dir);
	mask.append("\\*.pak");
	find_handle h(_findfirst(mask.c_str(), &data));

	if(h)
	{
		do
		{
			if(!(data.attrib & _A_SUBDIR))
			{
				std::string name(src_dir);
				name.append(1, '\\').append(data.name);

				aux::ReadPacketsDescription (v.get(), src_dir, name);
			}
		}
		while(_findnext(h.get(), &data) != -1);
	}

	return v;

}

std::auto_ptr<std::vector<hIO> > aux::get_control_files (hOBJECT g_root, char const* src_dir)
{
	std::auto_ptr<std::vector<hIO> > v(new std::vector<hIO>);
	_finddata_t data;
	std::string mask(src_dir);
	mask.append("\\*.chk");
	find_handle h(_findfirst(mask.c_str(), &data));

	if(h)
	{
		do
		{
			if(!(data.attrib & _A_SUBDIR))
			{
				std::string name(src_dir);
				name.append(1, '\\').append(data.name);

				cStringObj hObjectName ( name.c_str() );
				cIOObj hIo(g_root, cAutoString(hObjectName), fACCESS_READ | fACCESS_FORCE_READ, 
					fOMODE_OPEN_IF_EXIST , PID_NATIVE_FIO );
				v->push_back(hIo.relinquish());
			}
		}
		while(_findnext(h.get(), &data) != -1);
	}
	return v;

}

void aux::ReadPacketsDescription (std::vector<aux::output_packet>* packets, const std::string& src_dir, const std::string& file)
{
	char buf[16384];
	int bytes_written = GetPrivateProfileSectionNames (buf, 16384, file.c_str());
	std::vector<std::string> sections;
	int cur = 0;
	do 
	{
		std::string sec_name (&buf[cur]);
		char buffer_file[512];
		char buffer_state[32];
		char buffer_target[32];
		if (
			GetPrivateProfileString (sec_name.c_str(), "packet", "", buffer_file, 512, file.c_str()) &&
			GetPrivateProfileString (sec_name.c_str(), "status", "", buffer_state, 32, file.c_str()) &&
			GetPrivateProfileString (sec_name.c_str(), "target", "", buffer_target, 32, file.c_str())
			)
		{
			TrafficProtocoller::data_target_t buffer_target_key = TrafficProtocoller::dsUnknown;
			TrafficProtocoller::process_status_t buffer_state_key = TrafficProtocoller::psUnknown;
			std::string buffer_state_str (buffer_state);
			std::string buffer_target_str (buffer_target);
			std::transform	(buffer_state_str.begin(), buffer_state_str.end(),
				buffer_state_str.begin(), tolower);
			std::transform	(buffer_target_str.begin(), buffer_target_str.end(),
				buffer_target_str.begin(), tolower);

			if (buffer_state_str == "psprocessfinished") buffer_state_key = TrafficProtocoller::psProcessFinished;
			if (buffer_state_str == "pskeepalive") buffer_state_key = TrafficProtocoller::psKeepAlive;
			if (buffer_state_str == "pscloseconnection") buffer_state_key = TrafficProtocoller::psCloseConnection;

			if (buffer_target_str == "dsclient") buffer_target_key = TrafficProtocoller::dsClient;
			if (buffer_target_str == "dsserver") buffer_target_key = TrafficProtocoller::dsServer;

			packets->push_back (aux::output_packet (std::string(src_dir).append("\\").append(buffer_file),
				buffer_target_key, buffer_state_key,
				GetPrivateProfileInt (sec_name.c_str(), "flush", 0, file.c_str())));

		}
		cur += sec_name.length();
		cur ++;

	} while(cur<bytes_written && buf[cur]!=0);

}


size_t aux::rnd_chunk_size (int limit)
{
	size_t size(rand() % (limit+1));
	return size ? size : 1;
}

hIO aux::create_io_from_array (const void* stream, size_t size)
{
	tDWORD result;
	prague::IO<unsigned char> packet(prague::create_temp_io());
	hIO hData = packet.release();
	hData->SeekWrite (&result, 0, (tPTR)stream, size);

	return hData;
}

data_target_t aux::SourceToTarget (const data_source_t& source)
{
	switch (source)
	{
	case dsClient: return dsServer;
	case dsServer: return dsClient;
	default: _ASSERTE (!"Unexpected");
	}
};

void aux::pump_io_to_array (hIO io, std::vector<byte>& stream)
{
	const int chunk_size = 0x100;
	char buf[chunk_size];
	tQWORD size;
	io->GetSize (&size, IO_SIZE_TYPE_EXPLICIT);

	if (!size) return;

	int bytes_remain = size;
	int offset = 0;
	int stream_offset = stream.size();
	stream./*reserve*/resize (stream_offset + size);

	do 
	{
		tDWORD result;

		io->SeekRead (&result, offset, &stream[stream_offset], bytes_remain<chunk_size?bytes_remain:chunk_size);
		offset += bytes_remain<chunk_size?bytes_remain:chunk_size;
		stream_offset += bytes_remain<chunk_size?bytes_remain:chunk_size;
		bytes_remain -= chunk_size;

	} while(bytes_remain > 0);

}

void aux::pump_io_to_io (hIO io_source, hIO io_dest)
{
	const int chunk_size = 0x100;
	char buf[chunk_size];
	tQWORD size;
	io_source->GetSize (&size, IO_SIZE_TYPE_EXPLICIT);
	io_dest->SetSize (size);

	if (!size) return;

	int bytes_remain = size;
	int offset = 0;

	cIOObj::copy (io_dest, io_source, 1024);

}

namespace aux
{//aux


	TraceFile::TraceFile (const std::string& filename)
	{
		file = fopen (filename.c_str(), "wb");
	}

	TraceFile::~TraceFile ()
	{
		fclose (file);
	}

	void TraceFile::Write (hIO io)
	{
		sync::local_mutex::scoped_lock lock(m_guard);
		std::vector<byte> data;
		pump_io_to_array (io, data);
		fwrite (&data[0], 1, data.size(), file);
	}

	void TraceFile::Write (const char* begin, const char* end)
	{
		sync::local_mutex::scoped_lock lock(m_guard);
		fwrite (begin, 1, end-begin, file);
	}

	void TraceFile::Write (const char* begin)
	{
		sync::local_mutex::scoped_lock lock(m_guard);
		fwrite (begin, 1, strlen(begin), file);
	}

	void TraceFile::Flush ()
	{
		fflush (file);
	}
	//~aux
}

hIO aux::create_io_from_disk (const std::string& filename)
{
	hIO hData;
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

	return hData;
}

bool aux::isIoEquals (hIO left_io, hIO right_io)
{
	const int chunk_size = 0x100;
	char buf[chunk_size], buf2[chunk_size];
	tQWORD left_size, right_size;
	left_io->GetSize ( &left_size, IO_SIZE_TYPE_EXPLICIT);
	right_io->GetSize ( &right_size, IO_SIZE_TYPE_EXPLICIT);

	if (left_size != right_size)
	{
		tDWORD result;
		int offset = 0;

		char buf[32], buf2[32];
		LOG_CALL5("\tIncorrect io size: ",itoa(left_size, buf, 10)," - ",itoa(right_size, buf, 10),"\n");

		//left
		char* data = new char[left_size+1];
		data[left_size]=0;
		left_io->SeekRead (&result, offset, data, left_size);
		LOG_CALL3("<<FIRST_IO>>:",data,"\n");
		delete[] data;

		//right
		data = new char[right_size+1];
		data[right_size]=0;
		right_io->SeekRead (&result, offset, data, right_size);
		LOG_CALL3("<<SECOND_IO>>:",data,"\n");
		delete[] data;

		return false;
	}
	int bytes_remain = left_size;
	int offset = 0;
	do 
	{
		tDWORD result;

		left_io->SeekRead (&result, offset, buf, bytes_remain<chunk_size?bytes_remain:chunk_size);
		right_io->SeekRead (&result, offset, buf2, bytes_remain<chunk_size?bytes_remain:chunk_size);

		if (memcmp (buf, buf2, bytes_remain<chunk_size?bytes_remain:chunk_size))
		{
			char buf[32], buf2[32];
			LOG_CALL5("Incorrect content, bytes ",itoa(offset, buf, 10)," through ",\
				itoa(offset + (bytes_remain<chunk_size?bytes_remain:chunk_size), buf2, 10),"\n");
			return false;
		}

		offset += bytes_remain<chunk_size?bytes_remain:chunk_size;
		bytes_remain -= chunk_size;

	} while(bytes_remain > 0);

	return true;
}

std::string aux::detectCodeToString (detect_code_t t)
{
	switch (t)
	{
	case dcProtocolUnknown: return " cdcProtocolUnknown ";
	case dcProtocolDetected: return " dcProtocolDetected ";
	case dcNeedMoreInfo: return " dcNeedMoreInfo ";
	case dcDataDetected: return " dcDataDetected ";
	case dcProtocolLikeDetected: return " dcProtocolLikeDetected ";
	default: return "";
	}
}

std::string aux::prResultToString (tERROR error)
{
	switch (error)
	{
	case errOK_DECIDED: return " errOK_DECIDED ";
	case errOK: return " errOK ";
	case errOK_NO_DECIDERS: return " errOK_NO_DECIDERS ";
	default: return " Unknown code error ";
	}
}

std::string aux::processStatusToString (process_status_t p)
{
	switch (p)
	{
	case psProcessFinished: return " psProcessFinished ";
	case psKeepAlive: return " psKeepAlive ";
	default: return " Unexpected process_status_t ";
	}
}

std::string aux::noRN (const std::string& strok)
{
	size_t sz = strok.find("\r");
	return (sz == std::string::npos) ?  strok : strok.substr (0, sz-1);
}