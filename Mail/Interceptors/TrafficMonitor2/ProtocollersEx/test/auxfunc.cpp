#define NOMINMAX
#include <windows.h>
#include <io.h>
#include <sys/stat.h>
#include <algorithm>
#include <numeric>
#include "auxfunc.h"
#include <utils/prague/scoped_handle.hpp>
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
///////////////////////////////////////////////////////////////////////////////
}//unnamed namespace
///////////////////////////////////////////////////////////////////////////////
typedef util::scoped_handle<long, int (*)(intptr_t), &_findclose, 0, -1> find_handle;
typedef util::scoped_handle<HANDLE, BOOL (WINAPI*)(HANDLE), &CloseHandle, 0, 0> memmap_handle;
typedef util::scoped_handle<FILE*, int (*)(FILE*), &fclose, 0, 0> file_t;
///////////////////////////////////////////////////////////////////////////////
//! return list of files that contain http packet
std::auto_ptr<std::list<std::string> > aux::http_packets(char const* src_dir)
{
	std::auto_ptr<std::list<std::string> > l(new std::list<std::string>);
	_finddata_t data;
	std::string mask(src_dir);
	mask.append("\\*.dat?");
	find_handle h(_findfirst(mask.c_str(), &data));
	
	if(h)
	{
		do
		{
			if(!(data.attrib & _A_SUBDIR))
			{
				std::string name(src_dir);
				name.append(1, '\\').append(data.name);
				l->push_back(name);
			}
		}
		while(_findnext(h.get(), &data) != -1);
	}
	return l;
}
///////////////////////////////////////////////////////////////////////////////
std::pair<char*, size_t> aux::create_http_stream(std::string const& fname)
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
//! return pair of mapped memory pointer & size of memory
std::pair<char*, size_t> aux::create_http_stream(std::list<std::string> const& l)
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