#include "log.h"

	
void TestLog::print(const char *format, ...)
{
	va_list ap;
	va_start(ap, format);
	reallyPrint(format, ap);
	va_end(ap);
};
 
void TestLog::reallyPrint (const char *format, va_list ap) 
{
	std::string logMessage;
	makeTraceMessageFromEllipsis(logMessage, format, withTimeStamp, "", ap);
	std::wcout<<logMessage.c_str();
};
