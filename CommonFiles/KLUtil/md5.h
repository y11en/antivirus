#ifndef _KLUTIL_MD5_H__INCLUDED_
#define _KLUTIL_MD5_H__INCLUDED_

#include "defs.h"
#include "block.h"
#include <memory>

namespace KLUTIL
{
	KLUTIL_API std::auto_ptr<KLUTIL::CBlock> GetMD5(const void* pData, size_t nDataSize);
	KLUTIL_API std::auto_ptr<KLUTIL::CBlock> GetFile_MD5Checksum(const wchar_t* pszwFile);
}

#endif // _KLUTIL_MD5_H__INCLUDED_
