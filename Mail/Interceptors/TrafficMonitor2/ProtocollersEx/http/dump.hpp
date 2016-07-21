/*!
*		
*		
*		(C) 2003 Kaspersky Lab 
*		
*		\file	dump.hpp
*		\brief	
*		
*		\author Владислав Овчарик
*		\date	16.06.2005 13:11:55
*		
*		
*/		
#if !defined(_DUMP_INCLUDED_HPP_)
#define _DUMP_INCLUDED_HPP_

#include <version/ver_product.h>

///////////////////////////////////////////////////////////////////////////////
namespace http
{
///////////////////////////////////////////////////////////////////////////////
template<int unique>
struct dump
{
	explicit dump(char const* prefix, void* p = 0)
		: suffix_(p)
	{
		memset(prefix_, 0, sizeof(prefix_));
		strncpy(prefix_, prefix, sizeof(prefix_));
	}
	//! write data to the dump file
	template<typename It> void write(It beg, It end) const
	{
		InterlockedIncrement(&idx);
		char suffix[0x40] = {0};
		wsprintfA(suffix, "%09d_%p", idx, suffix_);
		std::auto_ptr<std::vector<char> > path(this->dump_path());
		path->insert(path->end() - 1, prefix_, prefix_ + strlen(prefix_));
		path->insert(path->end() - 1, suffix, suffix + strlen(suffix));
		path->push_back(0);
		std::ofstream ofs(&(*path)[0], std::ios::binary);
		if(ofs.is_open())
			std::copy(beg, end, std::ostream_iterator<char>(ofs));
	}
	//! return true if user need dump packets
	static bool need_dump()
	{
		HKEY hkey(0);
		unsigned long dump_packet(0);
		if(RegOpenKeyExA(HKEY_LOCAL_MACHINE, dump_key, 0, KEY_READ, &hkey) == ERROR_SUCCESS)
		{
			unsigned long type(0), size(sizeof(dump_packet));
			RegQueryValueExA(hkey, "DumpHttpTraffic"
							, 0, &type
							, reinterpret_cast<unsigned char*>(&dump_packet)
							, &size);
			RegCloseKey(hkey);
		}
		return dump_packet != 0;
	}
private:
	//! return dump path, where dump files were located
	std::auto_ptr<std::vector<char> > dump_path() const
	{
		std::auto_ptr<std::vector<char> > path(new std::vector<char>);
		HKEY hkey(0);
		if(RegOpenKeyExA(HKEY_LOCAL_MACHINE, dump_key, 0, KEY_READ, &hkey) == ERROR_SUCCESS)
		{
			unsigned long type(0), size(0);
			if(RegQueryValueExA(hkey, "DumpHttpTrafficPath", 0, &type, 0, &size) == ERROR_SUCCESS)
			{
				path->reserve(size);
				path->insert(path->end(), size, 0);
				RegQueryValueExA(hkey, "DumpHttpTrafficPath", 0, &type, reinterpret_cast<unsigned char*>(&(*path)[0]), &size);
				if((*path)[size - 1] != '\\'
						|| (*path)[size - 1] != '//')
						path->insert(path->begin() + (size - 1), 1, '\\');
			}
			RegCloseKey(hkey);
		}
		if(path->empty())
			path->push_back(0);
		return path;
	}
private:
	static long			idx;			//! index of dump
	static char const	dump_key[];		//! registry key where dump flags presents
	char				prefix_[0x40];	//! dump's file prefix
	void*				suffix_;
};
///////////////////////////////////////////////////////////////////////////////
template<int unique> long dump<unique>::idx = 0;
///////////////////////////////////////////////////////////////////////////////
template<int unique>
char const dump<unique>::dump_key[] = VER_PRODUCT_REGISTRY_PATH "\\Trace\\Default";
///////////////////////////////////////////////////////////////////////////////
}//namespace http
///////////////////////////////////////////////////////////////////////////////
#endif//!defined(_DUMP_INCLUDED_HPP_)