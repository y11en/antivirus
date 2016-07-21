#include <stdlib.h>
#include <stdio.h>
#include <stdarg.h>
#include <windows.h>

#include "debug.h"

#pragma warning(disable:4996)

#ifdef _DEBUG

void DbPrint(CHAR* Format, ...)
{
	CHAR Buf[512];

	va_list args;
	va_start(args, Format);

	vsnprintf(Buf, _countof(Buf), Format, args);

	va_end(args);

	OutputDebugStringA(Buf);
}

#endif
