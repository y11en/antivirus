#include "file_al.h"
#include "malloc.h"
#include "btdeser.h"


static int _bt_deserialize_int64(__int64* pvalue, void** pbuffer, size_t* pbuffer_size, size_t* preq_size )
{
	unsigned char* buffer = (unsigned char*)*pbuffer;
	size_t buf_size = *pbuffer_size;
	unsigned __int64 num = 0;
	unsigned char b;
    do
    {
		if (!buf_size)
		{
			*preq_size = 10;
			return 0;
		}
		b = *buffer;
		buffer++;
		buf_size--;
		num <<= 7;
		num |= (b & 0x7F);
    } while (b & 0x80); 
	*pvalue = num;
	*pbuffer = buffer;
	*pbuffer_size = buf_size;
	return 1;
}

static int _bt_deserialize_int(int* pvalue, void** pbuffer, size_t* pbuffer_size, size_t* preq_size )
{
	unsigned char* buffer = (unsigned char*)*pbuffer;
	size_t buf_size = *pbuffer_size;
	unsigned int num = 0;
	unsigned char b;
    do
    {
		if (!buf_size)
		{
			*preq_size = 5;
			return 0;
		}
		b = *buffer;
		buffer++;
		buf_size--;
		num <<= 7;
		num |= (b & 0x7F);
    } while (b & 0x80); 
	if ((num & 3) == 3)
		num = 0xC0000000 | ((~num) >> 2);
	else
		num = (num << 30) | (num >> 2);
	*pvalue = num;
	*pbuffer = buffer;
	*pbuffer_size = buf_size;
	return 1;
}

static int _bt_deserialize_byte(unsigned char* pvalue, void** pbuffer, size_t* pbuffer_size, size_t* preq_size )
{
	unsigned char* buffer = (unsigned char*)*pbuffer;
	size_t buf_size = *pbuffer_size;
	if (!buf_size)
	{
		*preq_size = 5;
		return 0;
	}
	*pvalue = *buffer;
	buffer++;
	buf_size--;
	*pbuffer = buffer;
	*pbuffer_size = buf_size;
	return 1;
}

static int _bt_deserialize_str_ansi(char** pstr, void** pbuffer, size_t* pbuffer_size, size_t* preq_size )
{
	unsigned int len;
	unsigned char* buffer = (unsigned char*)*pbuffer;
	size_t buf_size = *pbuffer_size;
	for (len=0; len<buf_size; len++)
	{
		if (buffer[len] == 0)
		{
			len++;
			*pstr = (char*)malloc(len);
			if (!*pstr)
			{
				*preq_size = 0;
				return 0;
			}
			memcpy(*pstr, buffer, len);
			buffer += len;
			buf_size -= len;
			*pbuffer = buffer;
			*pbuffer_size = buf_size;
			return 1;
		}
	}
	*preq_size = buf_size + 0x100;
	return 0;
}

static int _bt_deserialize_str_uni(wchar_t** pwstr, void** pbuffer, size_t* pbuffer_size, size_t* preq_size )
{
	unsigned int i, len;
	unsigned char* buffer = (unsigned char*)*pbuffer;
	wchar_t* wstr;
	size_t buf_size = *pbuffer_size;
	if (!buf_size)
	{
		*preq_size = 0x100;
		return 0;
	}

	if (*buffer != 0x01)
	{
		// stored as ansi
		for (len=0; len<buf_size; len++)
		{
			if (buffer[len] == 0)
			{
				len++;
				*pwstr = wstr = (wchar_t*)malloc(len*sizeof(wchar_t));
				if (!wstr)
				{
					*preq_size = 0;
					return 0;
				}
				for (i=0; i<len; i++)
					wstr[i] = buffer[i];
					
				buffer += len;
				buf_size -= len;
				*pbuffer = buffer;
				*pbuffer_size = buf_size;
				return 1;
			}
		}
	}
	else
	{
		buffer++; buf_size--; // skip 0x01
		wstr = (wchar_t*)buffer;
		for (len=0; len<buf_size/sizeof(wchar_t); len++)
		{
			if (wstr[len] == 0)
			{
				len++;
				len *= sizeof(wchar_t);
				*pwstr = wstr = (wchar_t*)malloc(len);
				if (!wstr)
				{
					*preq_size = 0;
					return 0;
				}
				memcpy(wstr, buffer, len);
					
				buffer += len;
				buf_size -= len;
				*pbuffer = buffer;
				*pbuffer_size = buf_size;
				return 1;
			}
		}
	}
	*preq_size = buf_size + 0x100;
	return 0;
}

// =============================================================================================

bool _bt_deserialize_int64(__int64* pvalue, cCachedRead& data )
{
	unsigned __int64 num = 0;
	unsigned char b;
	unsigned int shift = 0;
    do
    {
		if (!data.Read(&b))
			return false;
		num |= (b & 0x7F) << shift;
		shift += 7;
    } while (b & 0x80); 
	*pvalue = num;
	return true;
}

bool _bt_deserialize_int(int* pvalue, cCachedRead& data )
{
	unsigned int num = 0;
	unsigned char b;
	unsigned int shift = 0;
    do
    {
		if (!data.Read(&b))
			return false;
		num |= (b & 0x7F) << shift;
		shift += 7;
    } while (b & 0x80); 
	if ((num & 3) == 3)
		num = 0xC0000000 | ((~num) >> 2);
	else
		num = (num << 30) | (num >> 2);
	*pvalue = num;
	return true;
}

bool _bt_deserialize_byte(unsigned char* pvalue, cCachedRead& data )
{
	return data.Read(pvalue);
}

bool _bt_deserialize_str_ansi(char** pstr, cCachedRead& data )
{
	unsigned int len = 0;
	tFilePos pos = data.GetPos();
	unsigned char b;
	do { 
		if (!data.Read(&b))
			return false;
		len++;
	} while (b);
	*pstr = (char*)malloc(len);
	if (!*pstr)
		return false;
	if (!data.SetPos(pos))
		return false;
	return data.Read((unsigned char*)*pstr, len, 0);
}

bool _bt_deserialize_str_uni(wchar_t** pwstr, cCachedRead& data )
{
	unsigned int len=0;
	wchar_t* wstr;
	unsigned char b;
	wchar_t wc;
	tFilePos pos = data.GetPos();
	if (!data.Read(&b))
		return false;

	if (b != 0x01)
	{
		// stored as ansi
		len++;
		while (b) 
		{ 
			if (!data.Read(&b))
				return false;
			len++;
		};

		*pwstr = wstr = (wchar_t*)malloc(len*sizeof(wchar_t));
		if (!wstr)
			return false;
		if (!data.SetPos(pos))
			return false;
		len = 0;
		do { 
			if (!data.Read(&b))
				return false;
			wstr[len] = b;
			len++;
		} while (b);
		return true;
	}

	// stored as uni
	pos++; // skip 0x01

	do { 
		if (!data.Read((unsigned char*)&wc, sizeof(wchar_t), 0))
			return false;
		len++;
	} while (wc);

	*pwstr = wstr = (wchar_t*)malloc(len*sizeof(wchar_t));
	if (!wstr)
		return false;
	if (!data.SetPos(pos))
		return false;
	len = 0;
	do { 
		if (!data.Read((unsigned char*)&wc, sizeof(wchar_t), 0))
			return false;
		wstr[len] = wc;
		len++;
	} while (wc);
	return true;
}

bool _bt_deserialize(char* format, cCachedRead& data, void* args[], bool allocated[], size_t* pargc)
{
	char fmt;
	*pargc = 0;
	void** pargs = args;
	do 
	{
		fmt = *format++;
		switch (fmt)
		{
			case 0: // end
				break;
			case 'I': // __int64
				_bt_deserialize_int64((__int64*)args, data); 
				args+=2;
				break;
			case 'f': // float
				_bt_deserialize_int((int*)args, data); 
				args++;
			case 'i': // int
			case 'p': // ptr
			case '*': // int (width)
				_bt_deserialize_int((int*)args, data); 
				args++;
				break;
			case 's': // string ANSI
				if (!_bt_deserialize_str_ansi((char**)args, data))
					args = args;
				allocated[args - pargs] = true;
				args++;
				break;
			case 'S': // string UNI
				_bt_deserialize_str_uni((wchar_t**)args, data); 
				allocated[args - pargs] = true;
				args++;
				break;
			case 'P': // string prague hSTRING
				_bt_deserialize_int((int*)args, data); 
				args++;
				break;
			default:
				// ??? unknown format?
				__asm int 3;
				_bt_deserialize_int((int*)args, data); 
				args++;
		}
	} while (fmt);
	*pargc = args - pargs;
	return true;
}