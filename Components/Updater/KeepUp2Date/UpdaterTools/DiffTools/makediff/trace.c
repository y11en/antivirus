#include <stdio.h>
#include <stdarg.h>
#include <time.h>
#include <string.h>

#include "trace.h"

unsigned int g_trace_level = 0;
FILE*        g_output = NULL;

void _Trace(unsigned int level, const char* format, ...)
{
	va_list _list;
	struct tm* _tm;
	time_t _t;

	if (g_trace_level < level)
		return;
	if (!g_output)
		g_output = stdout;
	_t = time(0);
	_tm = localtime(&_t);
	fprintf(g_output, "%04d-%02d-%02d %02d:%02d:%02d ", _tm->tm_year + 1900, _tm->tm_mon+1, _tm->tm_mday, _tm->tm_hour, _tm->tm_min, _tm->tm_sec);
	va_start(_list, format);
	vfprintf(g_output, format, _list);
	va_end(_list);
	if (format[strlen(format)-1] != '\n')
		fprintf(g_output, "\n");
}

void _Trace1v(unsigned int level, const char* format, va_list _list)
{
	struct tm* _tm;
	time_t _t;
	if (g_trace_level < level)
		return;
	if (!g_output)
		g_output = stdout;
	_t = time(0);
	_tm = localtime(&_t);
	fprintf(g_output, "%04d-%02d-%02d %02d:%02d:%02d ", _tm->tm_year + 1900, _tm->tm_mon+1, _tm->tm_mday, _tm->tm_hour, _tm->tm_min, _tm->tm_sec);
	vfprintf(g_output, format, _list);
}

void _Trace2(unsigned int level, const char* format, ...)
{
	va_list _list;

	if (g_trace_level < level)
		return;
	if (!g_output)
		g_output = stdout;
	va_start(_list, format);
	vfprintf(g_output, format, _list);
	va_end(_list);
	if (format[strlen(format)-1] != '\n')
		fprintf(g_output, "\n");
}


void _Trace2v(unsigned int level, const char* format, va_list _list)
{
	if (g_trace_level < level)
		return;
	if (!g_output)
		g_output = stdout;
	vfprintf(g_output, format, _list);
}
