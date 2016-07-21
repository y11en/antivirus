#include <stdio.h>
#include <stdarg.h>
#include <time.h>
#include <malloc.h>
#include <string.h>
#include <minmax.h>

#include "trace.h"

unsigned int g_trace_level = 0;
FILE*        g_output = stdout;

#define __malloc(heap, size) malloc(size);
#define __realloc(heap, mem, size) realloc(mem, size);
#define __free(heap, mem) free(mem);

#include "bintrace.h"

int _BinTraceSerializeV(__int64 _time, const char* trace_format, void* trace_args, const char* hdr_format, void* hdr_args)
{
	char buffer[0x200];
	char* pbuffer = buffer;
	size_t buff_size = sizeof(buffer);
	void* btdata;
	size_t btsize;
	static __int64 last_time = 0;
	int res = 0;


	if (!bt_serialize_v(pbuffer, buff_size, &buff_size, &btdata, &btsize, _time-last_time, trace_format, trace_args, hdr_format, hdr_args))
	{
		if (!buff_size)
			return 0;
		pbuffer = __malloc(0, buff_size);
		if (!pbuffer)
			return 0;
		if (!bt_serialize_v(pbuffer, buff_size, &buff_size, &btdata, &btsize, _time-last_time, trace_format, trace_args, hdr_format, hdr_args))
		{
			__free(0, pbuffer);
			return 0;
		}
	}
	res = fwrite(btdata, btsize, 1, g_output);
	if (pbuffer != buffer)
		__free(0, pbuffer);
	last_time = _time;
	return res;
}

int _BinTraceSerialize(__int64 _time, const char* trace_format, void* trace_args, const char* hdr_format, ...)
{
	int res;
	va_list hdr_args;
	va_start(hdr_args, hdr_format);
	res = _BinTraceSerializeV(_time, trace_format, trace_args, hdr_format, hdr_args);
	va_end(hdr_args);
	return res;
}
/*
int _BinTraceV(unsigned int level, const char* trace_format, void* trace_args)
{
	static int config_saved = 0;

	if (!config_saved)
	{
		_BinTraceSerialize(time(0), 0, 0, 
			"Iiss",        // config format
			1000000000L,   // time units (in nanoseconds)
			0,             // flags
			"",            // header internal format
			"");           // header output format
		config_saved = 1;
	}

	_BinTraceSerialize(time(0), 
		trace_format,        // data format
		trace_args,          // args
		NULL);                 // time

	return 1;
}

int _BinTrace(unsigned int level, const char* format, ...)
{
	int res;
	va_list args;
	va_start(args, format);
	res = _BinTraceV(level, format, args);
	va_end(args);
	return res;
}
*/
void _Trace(unsigned int level, const char* trace_format, ...)
{
	va_list trace_args;
	struct tm* _tm;
	time_t _t;

	if (g_trace_level < level)
		return;
	if (*trace_format == 0x02) // binary trace prefix
	{
		static int config_saved = 0;
	
		va_start(trace_args, trace_format);
		if (!config_saved)
		{
			_BinTraceSerialize(time(0), 
				0, 0,          // no trace format&args => config
				"iiss",        // config format
				eBTTimeFormat_time_t, // time format
				0,             // flags
				"i",            // header internal format
				"%d");           // header output format
			config_saved = 1;
		}

		_BinTraceSerialize(time(0), 
			trace_format,        // trace format
			trace_args,          // trace args
			"i",                 // internal header
			time(0));  // internal header data             
		va_end(trace_args);
		return;
	}
	if (*trace_format == 0x01) // binary trace, not converted (as text)
		trace_format += 8;
	_t = time(0);
	_tm = localtime(&_t);
	fprintf(g_output, "%04d-%02d-%02d %02d:%02d:%02d ", _tm->tm_year + 1900, _tm->tm_mon+1, _tm->tm_mday, _tm->tm_hour, _tm->tm_min, _tm->tm_sec);
	va_start(trace_args, trace_format);
	vfprintf(g_output, trace_format, trace_args);
	va_end(trace_args);
}

