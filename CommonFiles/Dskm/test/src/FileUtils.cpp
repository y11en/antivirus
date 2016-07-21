/*
*/

#include "StdAfx.h"
#include "FileUtils.h"
#include <fstream>
#include <vector>
#include <direct.h>

namespace KL  {
namespace DskmLibWrapper  {
namespace Test  {

//////////////////////////////////////////////////////////////////////////
FolderGuard::FolderGuard(const char* path, bool create_if_not_exists)
{
	std::vector<char> buf(4096);
	if (_getcwd(&buf[0], buf.size()))
	{
		m_cur = &buf[0];
		if (_chdir(path) != 0)
		{
			if (create_if_not_exists)
			{
				if (_mkdir(path) != 0 || _chdir(path) != 0)
				{
					std::string s("Can't create folder: ");
					s += path;
					throw std::exception(s.c_str());
				}
			}
			else
			{
				std::string s("Can't find folder: ");
				s += path;
				throw std::exception(s.c_str());
			}
		}
	}
	else
	{
		throw std::exception("Can't get current folder");
	}
}

//////////////////////////////////////////////////////////////////////////
FolderGuard::~FolderGuard()
{
	if (!m_cur.empty())
		_chdir(m_cur.c_str());
}

//////////////////////////////////////////////////////////////////////////
bool ReadDataFromFile(
	const char* file_name, 
	int pos, 
	char* pData, 
	size_t nDataSize
	)
{
	try
	{
		std::fstream os(file_name, std::ios_base::in | std::ios_base::binary);
		if (os.is_open())
		{
			os.seekp(pos, std::ios_base::beg);
			os.read(pData, nDataSize);
			return true;
		}
	}
	catch(std::exception&)
	{
	}
	return false;
}

//////////////////////////////////////////////////////////////////////////
bool CreateFileAndWriteData(const char* file_name, const char* pData, size_t nDataSize)
{
	try
	{
		std::fstream os(file_name, std::ios_base::out | std::ios_base::binary | std::ios_base::trunc);
		if (os.is_open())
		{
			os.write(pData, nDataSize);
			return true;
		}
	}
	catch(std::exception&)
	{
	}
	return false;
}

//////////////////////////////////////////////////////////////////////////
bool WriteDataToFile(
	const char* file_name, 
	int pos, 
	const char* pData, 
	size_t nDataSize
	)
{
	try
	{
		std::fstream os(file_name, std::ios_base::in | std::ios_base::out | std::ios_base::binary);
		if (os.is_open())
		{
			os.seekp(pos, std::ios_base::beg);
			os.write(pData, nDataSize);
			return true;
		}
	}
	catch(std::exception&)
	{
	}
	return false;
}

//////////////////////////////////////////////////////////////////////////
bool DeleteFile(const char* path)
{
	return (remove(path) == 0);
}

}  // namespace Test
}  // namespace DskmLibWrapper
}  // namespace KL
