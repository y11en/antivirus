#include "_aux.h"
#include <iostream>
#include <iterator>
#include <fstream>
#include <algorithm>
#include <functional>
#include <string>
#include <sstream>
#include <vector>
#include <xutility>


#include <Prague/prague.h>
#include <Prague/pr_cpp.h>

///////////////////////////////////////////////////////////////////////////////
namespace
{
	///////////////////////////////////////////////////////////////////////////////
	size_t accum_file_size(size_t sz, std::string const& fname)
	{
		struct _stat s;
		_stat(fname.c_str(), &s);
		return sz + s.st_size;
	}
	size_t file_size(FILE* f)
	{
		size_t pos(ftell(f));
		fseek(f, 0, SEEK_END);
		size_t size(ftell(f));
		fseek(f, pos, SEEK_SET);
		return size;
	}
}
///////////////////////////////////////////////////////////////////////////////

namespace aux
{

std::string GetCurrentThreadId()
{
	char buf[32];
	return itoa (::GetCurrentThreadId(), buf, 10);
}


std::string read_str( const std::string& filename )
{
	std::string result;

	std::ifstream w(filename.c_str(), std::ios_base::binary);
	w.unsetf(std::ios_base::skipws);
	std::copy(std::istream_iterator< char >(w), std::istream_iterator< char >(), std::back_inserter(result));
	return result;
}

///////////////////////////////////////////////////////////////////////////////
std::auto_ptr<std::vector<std::string> > http_packets(char const* src_dir)
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
///////////////////////////////////////////////////////////////////////////////
std::pair<char*, size_t> create_http_stream(std::string const& fname)
{
	size_t need_size = accum_file_size(0, fname);
	memmap_handle mem_map(CreateFileMapping(INVALID_HANDLE_VALUE, 0, PAGE_READWRITE, 0, need_size, 0));
	if(!mem_map)
		throw std::runtime_error("can not create file mapping");
	void* mem = MapViewOfFile(mem_map.get(), FILE_MAP_WRITE, 0, 0, need_size);
	if(!mem)
		throw std::runtime_error("can not map view to a memory");
	char* stm = static_cast<char*>(mem);
	file_t f(fopen(fname.c_str(), "rb"));
	if(!f)
	{
		std::string msg("can not open file \'");
		msg.append(fname.c_str()).append(1, '\'');
		throw std::runtime_error(msg);
	}
	fread(stm, sizeof(char), need_size, f.get());
	return std::pair<char*, size_t>(static_cast<char*>(mem), need_size);
}
///////////////////////////////////////////////////////////////////////////////
std::pair<char*, size_t> create_http_stream(std::list<std::string> const& l)
{
	typedef std::list<std::string>::const_iterator const_iterator;

	size_t need_size(std::accumulate(l.begin(), l.end(), static_cast<size_t>(0), &accum_file_size));
	memmap_handle mem_map(CreateFileMapping(INVALID_HANDLE_VALUE, 0, PAGE_READWRITE, 0, need_size, 0));
	if(!mem_map)
		throw std::runtime_error("can not create file mapping");
	void* mem = MapViewOfFile(mem_map.get(), FILE_MAP_WRITE, 0, 0, need_size);
	if(!mem)
		throw std::runtime_error("can not map view to a memory");
	const_iterator it(l.begin());
	char* stm = static_cast<char*>(mem);
	for(; it != l.end(); ++it)
	{
		file_t f(fopen(it->c_str(), "rb"));
		if(!f)
		{
			std::string msg("can not open file \'");
			msg.append(it->c_str()).append(1, '\'');
			throw std::runtime_error(msg);
		}
		size_t size(file_size(f.get()));
		fread(stm, sizeof(char), size, f.get());
		stm += size;
	}
	return std::pair<char*, size_t>(static_cast<char*>(mem), need_size);
}
///////////////////////////////////////////////////////////////////////////////
std::auto_ptr<std::vector<hIO> > get_control_files (hOBJECT g_root, char const* src_dir)
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

std::auto_ptr<std::vector<aux::output_packet> > get_output_packets (hOBJECT g_root, char const* src_dir)
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


void ReadPacketsDescription (std::vector<aux::output_packet>* packets, const std::string& src_dir, const std::string& file)
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

bool isIoEquals (hIO left_io, hIO right_io)
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

size_t rnd_chunk_size (int limit)
{return 0x1;
	size_t size(rand() % (limit+1));
	return size ? size : 1;
	//return 1;

	//если передавать  мелкими кусками, то будет бага с разбором
	//передаем целыми пакетами

	//return 0xFFFF;
}

size_t rnd_chunk_size2 (int limit)
{
	size_t size(rand() % (limit+1));
	return size ? size : 1;

}

unsigned char rnd_symbol ()
{
	unsigned char smb (rand() % 0x100);
	return smb;
	//return 'A';
}

void pump_io_to_array (hIO io, std::vector<byte>& stream)
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

hIO create_io_from_array (void* stream, size_t size)
{
	tDWORD result;
	prague::IO<unsigned char> packet(prague::create_temp_io());
	hIO hData = packet.release();
	hData->SeekWrite (&result, 0, (tPTR)stream, size);

	return hData;
}

msg_direction SourceToDirection (const TrafficProtocoller::data_source_t& source)
{
	switch (source)
	{
	case TrafficProtocoller::dsClient: return http::to_server;
	case TrafficProtocoller::dsServer: return http::to_client;
	default: return this_is_big_ass;
	}
};

bool DeleteDirectory (const char* dir)
{
	_ASSERTE (strlen(dir) != 0);
	int ret=0;
	char name1[256];
	WIN32_FIND_DATA info;
	HANDLE hp;
	char *cp;

	sprintf(name1, "%s\\*.*",dir);
	hp = FindFirstFile(name1,&info);
	if(!hp || hp==INVALID_HANDLE_VALUE)
		return (true);
	do
	{
		cp = info.cFileName;
		if(cp[1]==0 && *cp=='.')
			continue;
		else if(cp[2]==0 && *cp=='.' && cp[1]=='.')
			continue;
		sprintf(name1,"%s\\%s",dir,info.cFileName);
		if(info.dwFileAttributes&FILE_ATTRIBUTE_READONLY)
		{
			SetFileAttributes(name1,info.dwFileAttributes&~FILE_ATTRIBUTE_READONLY);
		}
		if(info.dwFileAttributes&FILE_ATTRIBUTE_DIRECTORY)
			DeleteDirectory(name1);
		else DeleteFile(name1);

	}
	while(FindNextFile(hp,&info));

	FindClose(hp);
	if(info.dwFileAttributes&FILE_ATTRIBUTE_READONLY)
	{
		SetFileAttributes(dir,info.dwFileAttributes&~FILE_ATTRIBUTE_READONLY);
	}

	return (0 != RemoveDirectory(dir));
}

bool MakeDirectory (const char* dir)
{
	return (0 != CreateDirectory (dir, NULL) || GetLastError() == ERROR_ALREADY_EXISTS);
};

void MakePacketDescriptionFile (const string& folder, const string& file,
									 const http::msg_direction& direction, const TrafficProtocoller::process_status_t& status)
{
	std::ofstream ffile (string(folder).append("\\").append(file).append(".pak").c_str(), std::ios::binary);
	ffile<<"[1]\n";
	ffile<<"packet="<<file.c_str()<<"\n";
	ffile<<"status=";
	if (status == TrafficProtocoller::psKeepAlive) ffile<<"psKeepAlive"; else ffile<<"psClose";
	ffile<<"\n";
	ffile<<"target=";
	if (direction == http::to_server) ffile<<"dsclient"; else ffile<<"dsserver";
	ffile<<"\n";
}


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


processor_proxy::processor_proxy (std::auto_ptr<processing_strategy> strategy, std::auto_ptr<session> proxy,
									std::auto_ptr<analyzer> analyzer, long timeout):
			trace_File (string("h:\\HTTP\\to_process.txt").append(aux::GetCurrentThreadId())),
			detect_Result (string("h:\\HTTP\\to_detect.txt").append(aux::GetCurrentThreadId())),
			bHttpDetected (false), p(strategy, proxy, analyzer, timeout)		
{

};

bool processor_proxy::process(char const* begin, char const* end, msg_direction direction, bool is_ssl)
{		
	if (!bHttpDetected)
	{
		traffic_cache.insert (traffic_cache.end(), begin, end);
		detection_code det_code = p.detect(&traffic_cache[0], &traffic_cache[0] + traffic_cache.size(), direction);
		detect_Result.Write (&traffic_cache[0], &traffic_cache[0] + traffic_cache.size());
		char buf[20];
		sprintf (buf, " - %i\n", det_code);
		detect_Result.Write (buf);
		detect_Result.Flush();

		if (det_code == detected) 
		{
			bHttpDetected = true;
			trace_File.Write (&traffic_cache[0], &traffic_cache[0] + traffic_cache.size());
			trace_File.Flush ();
			bool bResult =  p.process(&traffic_cache[0], &traffic_cache[0] + traffic_cache.size(), direction, is_ssl);
			traffic_cache.clear();
			return bResult;
		}
	}
	else
	{
		trace_File.Write (begin, end);
		trace_File.Flush ();
		return p.process(begin, end, direction, is_ssl);
	}

	return true;
}

void processor_proxy::flush()
{
	if (!traffic_cache.empty())
	{

		static char fl[] = "<<FLUSH>>\n";
		static char fl2[] = "Flush performed\n"; 

		detect_Result.Write (fl2);
		detect_Result.Flush();

		trace_File.Write (fl);
		trace_File.Flush ();

		bHttpDetected = false;
		traffic_cache.clear();
	}

	p.flush();
}


}
