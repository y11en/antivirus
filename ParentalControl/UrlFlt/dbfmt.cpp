#define KLAV_PRAGUE 0

#include <kl_byteswap.h>

#include "dbfmt.h"

// ########################################################################
// ########################################################################

void _WriteInt16(uint8_t * buf, uint16_t v)
{
	*(buf + 0) = (uint8_t)(v);
	*(buf + 1) = (uint8_t)(v >> 8);
}

uint16_t _ReadInt16(uint8_t * buf)
{
	uint16_t v;

	v = ((uint16_t)*(buf + 0) << 0) |
		((uint16_t)*(buf + 1) << 8);

	return v;
}

void _WriteInt32(uint8_t * buf, uint32_t v)
{
	*(buf + 0) = (uint8_t)(v);
	*(buf + 1) = (uint8_t)(v >> 8);
	*(buf + 2) = (uint8_t)(v >> 16);
	*(buf + 3) = (uint8_t)(v >> 24);
}

uint32_t _ReadInt32(uint8_t * buf)
{
	uint32_t v;

	v = ((uint32_t)*(buf + 0) << 0) |
		((uint32_t)*(buf + 1) << 8) |
		((uint32_t)*(buf + 2) << 16) |
		((uint32_t)*(buf + 3) << 24);

	return v;
}

void _WriteInt32Ex(uint8_t * buf, uint32_t cb, uint32_t v)
{
	buf += cb - 1;
	while( cb )
	{
		*buf = (uint8_t)(v);
		v >>= 8;
		cb--; buf--;
	}
}

uint32_t _ReadInt32Ex(uint8_t * buf, uint32_t cb)
{
	uint32_t v = 0;

	while( cb )
	{
		v <<= 8;
		v |= (uint32_t)*buf;
		cb--; buf++;
	}

	return v;
}

// ########################################################################

void _ParctlXorBuf(uint8_t * buf, uint32_t size, bool forward, bool smart)
{
	uint32_t key = 0xa7c41d39, next;
	uint32_t cb, v;

	if( forward )
	{
		while( size )
		{
			cb = size >= 4 ? 4 : size;

			v = _ReadInt32Ex(buf, cb);

			if( smart )
				next = key + v;
			_WriteInt32Ex(buf, cb, v ^ key);
			if( smart )
				key = next;

			buf += cb;
			size -= cb;
		}
	}
	else
	{
		while( size )
		{
			cb = size >= 4 ? 4 : size;

			v = _ReadInt32Ex(buf, cb) ^ key;
			_WriteInt32Ex(buf, cb, v);
			if( smart )
				key += v;

			buf += cb;
			size -= cb;
		}
	}
}

// ########################################################################
// ########################################################################
