#ifndef _KLUTIL_CRC32_H__INCLUDED_
#define _KLUTIL_CRC32_H__INCLUDED_

typedef unsigned long ULONG;
typedef unsigned char BYTE;

namespace KLUTIL
{
	ULONG crc32(ULONG crc, const BYTE* buf, ULONG len);
}

#endif //_KLUTIL_CRC32_H__INCLUDED_

