#ifndef __FILE_ABSTRACT_HPP__
#define __FILE_ABSTRACT_HPP__

#include "file_al.h"

class cFileAL : public tFile 
{
public:
	operator tFile*()
	{
		return (tFile*)this;
	}
	bool Read(void* pData, size_t nSize, size_t* pnBytesRead)
	{
		return tFile::Read(this, pData, nSize, pnBytesRead);
	}
	bool Write(void* pData, size_t nSize, size_t* pnBytesWritten)
	{
		return tFile::Write(this, pData, nSize, pnBytesWritten);
	}
	bool Seek(tFilePos nPos, int nSeekMethod, tFilePos* pnNewPos)
	{
		return tFile::Seek(this, nPos, nSeekMethod, pnNewPos);
	}
	bool GetSize(tFileSize* pnSize)
	{
		return tFile::GetSize(this, pnSize);
	}
	bool SetSize(tFileSize nSize)
	{
		return tFile::SetSize(this, nSize);
	}
	bool Flush()
	{
		return tFile::Flush(this);
	}
	bool Close()
	{
		return tFile::Close(this);
	}
};

#endif __FILE_ABSTRACT_HPP__
