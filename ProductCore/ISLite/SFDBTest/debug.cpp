#ifndef _DEBUG_CPP
#define _DEBUG_CPP

#include "windows.h"
#include "stdarg.h"
#include "stdio.h"

#ifdef _DEBUG
/*
void dbgprintf(const char* format, ...)
{
	char szDbgStr[4096];
	va_list marker;
	va_start( marker, format );     // Initialize variable arguments. 
	wvsprintf(szDbgStr, format, marker);
	va_end(marker);  
	OutputDebugString(szDbgStr);
}
*/
void dbgprintf(const char* format, ...)
{
	va_list marker;
	va_start(marker, format);     // Initialize variable arguments. 
	vprintf(format, marker);
	va_end(marker);  
}

#endif	

#endif