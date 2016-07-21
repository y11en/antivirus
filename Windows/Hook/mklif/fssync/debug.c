#include <stdlib.h>
#include <stdio.h>
#include <stdarg.h>
#include <windows.h>

#include "debug.h"

#ifdef _DEBUG
void DbPrint(CHAR* Format, ...)
{
	CHAR Buf[512];

	va_list args;
	va_start(Format, args);

	vsnprintf(Buf, _countof(Buf), Format, args);

	va_end(args);

	OutputDebugStringA(Buf);
}
#else
void DbPrint(CHAR* Format, ...)
{
	// nothing todo
}
#endif
