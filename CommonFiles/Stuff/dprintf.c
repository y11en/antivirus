#if defined (_WIN32)
#ifndef __MWERKS__
	#include <windows.h> //vsprintf
#endif
#include <stdio.h> //vsprintf

int dprintf(const char* msg, ...)
{
	char s[512];
	int ret=0;
	va_list ap;
	va_start(ap,msg);
	ret=wvsprintf(s, msg, ap);
	va_end(ap);

	OutputDebugString(s);

	return ret;
}

#endif // _WIN32
