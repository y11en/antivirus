#include "bintrace.h"
#if !defined (__APPLE__)
#include <string.h>
#endif
#include <stdarg.h>

#if defined (_WIN32)
#include <minmax.h>
#elif defined (__unix__) 
#if !defined(__APPLE__)
#include <wchar.h>
#endif
#define min(l,r) (((l)>(r))?(r):(l))
#define max(l,r) (((l)>(r))?(l):(r))
#endif
static int _bt_serialize_int64(__int64 value, void** pbuffer, size_t* pbuffer_size, size_t* preq_size )
{
	unsigned char* buffer = (unsigned char*)*pbuffer;
	size_t buf_size = *pbuffer_size;
	size_t req_size = *preq_size;
	unsigned __int64 num = (unsigned __int64)value;
    do
    {
		unsigned char b;
		b = (unsigned char)(num & 0x7F);
		num >>= 7;
		if (num)
			b |= 0x80;
		if (buf_size)
		{
			*buffer = b;
			buffer++;
			buf_size--;
		}
		req_size++;
    } while (num);  
	*pbuffer = buffer;
	*pbuffer_size = buf_size;
	*preq_size = req_size;
	return 1;
}

static int _bt_serialize_int(int value, void** pbuffer, size_t* pbuffer_size, size_t* preq_size )
{
	unsigned char* buffer = (unsigned char*)*pbuffer;
	size_t buf_size = *pbuffer_size;
	size_t req_size = *preq_size;
	unsigned long num = (unsigned long)value;
	if ((num & 0xC0000000) == 0xC0000000)
		num = ((~num) << 2) | 0x03;
	else
		num = (num << 2) | (num >> 30);
    do
    {
		unsigned char b;
		b = (unsigned char)(num & 0x7F);
		num >>= 7;
		if (num)
			b |= 0x80;
		if (buf_size)
		{
			*buffer = b;
			buffer++;
			buf_size--;
		}
		req_size++;
    } while (num);  
	*pbuffer = buffer;
	*pbuffer_size = buf_size;
	*preq_size = req_size;
	return 1;
}

static int _bt_serialize_byte(unsigned char b, void** pbuffer, size_t* pbuffer_size, size_t* preq_size )
{
	unsigned char* buffer = (unsigned char*)*pbuffer;
	size_t buf_size = *pbuffer_size;
	size_t req_size = *preq_size;
	if (buf_size)
	{
		*buffer = b;
		buffer++;
		buf_size--;
	}
	req_size++;
	*pbuffer = buffer;
	*pbuffer_size = buf_size;
	*preq_size = req_size;
	return 1;
}

static int _bt_serialize_str_ansi(char* str, void** pbuffer, size_t* pbuffer_size, size_t* preq_size )
{
	unsigned int len, copy_len;
	if (!str)
		str = "(null)";
	len = strlen(str)+1;
	*preq_size += len;
	copy_len = min(*pbuffer_size, len);
	memcpy(*pbuffer, str, len);
	*pbuffer_size -= copy_len;
	*pbuffer = (char*)*pbuffer + copy_len;
	return 1;
}

static int _bt_serialize_str_uni(wchar_t* wstr, void** pbuffer, size_t* pbuffer_size, size_t* preq_size )
{
	int len, copy_as_ansi=1;
	wchar_t wc;
	unsigned char* buffer = (unsigned char*)*pbuffer;
	size_t buf_size = *pbuffer_size;
	size_t req_size = *preq_size;
	wchar_t* ptr = wstr;
	if (!wstr)
		wstr = L"(null)";
	if (*(unsigned char*)wstr == 0x01)
		copy_as_ansi = 0;
	len = 1;
	while(wc = *ptr++) 
	{
		len++;
		if (wc > 0xFF)
			copy_as_ansi = 0;
	};
	if (copy_as_ansi)
	{
		req_size += len;
		ptr = wstr;
		if (buf_size)
		{
			do {
				wc = *ptr++;
				*buffer++ = (unsigned char)wc;
				buf_size--;
			} while (wc && buf_size);
		}
	}
	else
	{
		wchar_t* wbuffer;
		req_size += len*sizeof(wchar_t)+1;
		if (buf_size)
		{
			*buffer++ = 0x01;
			buf_size--;
		}
		ptr = wstr;
		wbuffer = (wchar_t*)buffer;
		if (buf_size>=sizeof(wchar_t))
		{
			do {
				wc = *ptr++;
				*wbuffer++ = wc;
				buf_size-=sizeof(wchar_t);
			} while (wc && buf_size>=sizeof(wchar_t));
		}
		buffer = (unsigned char*)wbuffer;
	}
	*pbuffer = buffer;
	*pbuffer_size = buf_size;
	*preq_size = req_size;
	return 1;
}

/*  TODO:
  the funciton cause endless recursion in all control paths
int bt_serialize(void* buffer, size_t buffer_size, size_t* preq_size, void** pbtdata, size_t* pbtsize, unsigned __int64 _time, const char* trace_format, void* trace_args, const char* hdr_format, ...)
{
	int res;
	va_list hdr_args;
	va_start(hdr_args, hdr_format);

	res = bt_serialize_v(buffer, buffer_size, preq_size, pbtdata, pbtsize, _time, trace_format, trace_args, hdr_format, hdr_args);
	va_end(hdr_args);
	return res;
}*/

int bt_serialize_v(void* buffer, size_t buffer_size, size_t* preq_size, void** pbtdata, size_t* pbtsize, unsigned __int64 _time, const char* trace_format, void* trace_args, const char* hdr_format, void* hdr_args)
{
	int*  parg;
	void* buffptr = buffer;
	unsigned int fileid;
	unsigned int traceid;
	size_t buffsize = buffer_size;
	size_t req_size = 0;
	char c;
	size_t hdr_size = 0;
	size_t data_offset = 0;
	void* btdata; 
	int ser_type = 2; // trace
	int i;
	const char* format;

	if (!preq_size)
		return 0;
	*preq_size = 0;
	if (pbtdata)
		*pbtdata = 0;
	if (pbtsize)
		*pbtsize = 0;
	if (trace_format)
	{
		if (trace_format[0] != 0x02)
			return 0;
		traceid = (*(unsigned int*)(trace_format+1)) & 0x00FFFFFF;
		fileid = *(unsigned int*)(trace_format+4);
		trace_format += 8;
	}
	else
		ser_type = 1; // config

/*	if (!specials_saved)
	{
		_bt_serialize_byte(ser_type, &buffptr, &buffsize, &req_size);
		// special fields format
		_bt_serialize_str_ansi("Iiss", &buffptr, &buffsize, &req_size);
		// time units (in nanoseconds)
		_bt_serialize_int64(1000000000L, &buffptr, &buffsize, &req_size);
		// flags
		_bt_serialize_int(0, &buffptr, &buffsize, &req_size);
		// header internal format
		_bt_serialize_str_ansi("", &buffptr, &buffsize, &req_size);
		// header output format
		_bt_serialize_str_ansi("", &buffptr, &buffsize, &req_size);
	}
*/	
	_bt_serialize_byte((unsigned char)ser_type, &buffptr, &buffsize, &req_size);
	_bt_serialize_int64(_time, &buffptr, &buffsize, &req_size);

	if (ser_type != 1) // !config
	{
		_bt_serialize_int(fileid, &buffptr, &buffsize, &req_size);
		_bt_serialize_int(traceid, &buffptr, &buffsize, &req_size);
	}

	for (i=1; i<=2; i++)
	{
		switch (i)
		{
		case 1:
			format = hdr_format;
			parg = (int*)hdr_args;
			break;
		case 2:
			format = trace_format;
			parg = (int*)trace_args;
			break;
		}

		if (!format)
			continue;

		if (!hdr_size)
		{
			hdr_size = req_size;
			req_size += 8;
			data_offset = req_size;
			if (buffsize >= 8)
			{
				buffptr = (char*)buffptr + 8;
				buffsize -= 8;
			}
			else
			{
				buffsize = 0;
			}
		}
		
		do
		{
			c = *format++;
			switch(c)
			{
			case 0: // end
				break;
			case 'I': // __int64
				_bt_serialize_int64(*(__int64*)parg, &buffptr, &buffsize, &req_size); 
				parg+=2;
				break;
			case 'f': // float
				_bt_serialize_int(*parg++, &buffptr, &buffsize, &req_size); 
			case 'i': // int
			case 'p': // ptr
			case '*': // int (width)
				_bt_serialize_int(*parg++, &buffptr, &buffsize, &req_size); 
				break;
			case 's': // string ANSI
				_bt_serialize_str_ansi((char*)*parg++, &buffptr, &buffsize, &req_size); 
				break;
			case 'S': // string UNI
				_bt_serialize_str_uni((wchar_t*)*parg++, &buffptr, &buffsize, &req_size); 
				break;
			case 'P': // string prague hSTRING
				_bt_serialize_int(0, &buffptr, &buffsize, &req_size); 
				parg++;
				break;
			default:
				// ??? unknown format?
#if defined (_WIN32)
				__asm int 3;
#endif
				_bt_serialize_int(*parg++, &buffptr, &buffsize, &req_size); 
			}
		} while (c);
	}
	*preq_size = req_size;
	if ( buffer_size < req_size )
		return 0;
	buffptr = (char*)buffer+hdr_size;
	buffsize = buffer_size-hdr_size;
	_bt_serialize_int(req_size-data_offset, &buffptr, &buffsize, &hdr_size);
	btdata = (char*)buffer+(data_offset-hdr_size);
	memmove(btdata, buffer, hdr_size); // move header
	if (pbtdata)
		*pbtdata = btdata;
	if (pbtsize)
		*pbtsize = req_size - (data_offset-hdr_size);
	return 1;
}

