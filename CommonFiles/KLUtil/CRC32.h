#ifndef _KLUTIL_CRC32_H__INCLUDED_
#define _KLUTIL_CRC32_H__INCLUDED_

#include "defs.h"

namespace KLUTIL
{
	KLUTIL_API ULONG crc32(ULONG crc, const BYTE* buf, ULONG len);
}

#endif //_KLUTIL_CRC32_H__INCLUDED_