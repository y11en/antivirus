#ifndef ___TRACE_H_
#define ___TRACE_H_

#include <stdio.h>

extern FILE* g_output;
extern unsigned int g_trace_level;

void _Trace(unsigned int level, const char* format, ...);
void _Trace2(unsigned int level, const char* format, ...);
void _Trace1v(unsigned int level, const char* format, va_list _list);
void _Trace2v(unsigned int level, const char* format, va_list _list);

#endif //___TRACE_H_