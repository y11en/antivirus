#include "stdafx.h"
#include "debug.h"

#ifdef _DEBUG

void DbPrint(TCHAR* Format, ...)
{
	TCHAR Buf[512];

	va_list args;
	va_start(args, Format);

	_vstprintf_s(Buf, _countof(Buf), Format, args);

	va_end(args);

	OutputDebugString(Buf);
}

#endif
