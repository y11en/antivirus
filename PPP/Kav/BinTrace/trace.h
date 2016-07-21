#ifndef ___TRACE_H_
#define ___TRACE_H_

#include <stdio.h>

#ifdef __cplusplus
extern "C" {
#endif 

extern FILE* g_output;
extern unsigned int g_trace_level;

void _Trace(unsigned int level, const char* format, ...);

#ifdef __cplusplus
} // extern "C" {
#endif 


#endif //___TRACE_H_