#ifndef __FORCE_DEBUG_OUTPUT__H__
#define __FORCE_DEBUG_OUTPUT__H__

#if defined(_FORCE_DEBUG_OUTPUT_)
#undef PR_TRACE

#if defined(_PRAGUE_TRACE_)
#define PR_TRACE(p)           ((OutputDbgString p), (trace_func p))
#else
#define PR_TRACE(p)           (OutputDbgString p)
#endif

#endif

#include <wincompat.h>
#include <stdio.h>

static tBOOL OutputDbgString(tPTR handle, int level, char* szFormat, ...)
{
	char szDbgStr[4096];
	int offset = 0;
	va_list marker;
	va_start( marker, szFormat );
	offset +=  _snprintf(szDbgStr+offset, sizeof(szDbgStr)-offset, "%08X %03d ", handle, level);
	offset += _vsnprintf(szDbgStr+offset, sizeof(szDbgStr)-offset, szFormat, marker);
	offset +=  _snprintf(szDbgStr+offset, sizeof(szDbgStr)-offset, "\n");
	va_end(marker);  
	OutputDebugString(szDbgStr);
	return TRUE;
}

#endif // #ifndef __FORCE_DEBUG_OUTPUT__H__
