#include "log_iface.h"

namespace {
    static const int g_logBufferSize = 100 * 1024;
    static const char *g_defaultErrorMessage = "Internal error: failed to make string from ellipsis";
}


KLUPD::Log::~Log()
{
}

void KLUPD::Log::makeTraceMessageFromEllipsis(std::string &result, const char *formatIn,
        const Formatting &formatting, const std::string &decorationPrefix, va_list ap)
{

// in case wide-char string is not supported, then replace '%S' into '%s'
#ifndef WSTRING_SUPPORTED
    std::string format = formatIn;

    const std::string replacePattern = "%S";
    const std::string replaceString = "%s";
    while(true)
    {
        const std::string::size_type offset = format.find(replacePattern);
        if(offset == std::string::npos)
            break;
        format.replace(offset, replacePattern.size(), replaceString);
    }
#else
    std::string format = formatIn;
#endif

	const size_t patternSize = g_logBufferSize + 1;
    std::vector<char> pattern(patternSize, 0);

    int bytesWritten = 0;
    if(formatting == withTimeStamp)
    {
        const time_t unparsedTime = time(0);
        const struct tm *currentTime = localtime(&unparsedTime);
        if(currentTime)
        {
            bytesWritten = _snprintf(&pattern[0], patternSize, "%.2d:%.2d:%.2d:\t%s",
                currentTime->tm_hour, currentTime->tm_min, currentTime->tm_sec, format.c_str());
        }
    }

    // protection against (currentTime == 0)
    if(!bytesWritten)
        bytesWritten = _snprintf(&pattern[0], patternSize, "%s\t%s", decorationPrefix.c_str(), format.c_str());

    if(!bytesWritten || (bytesWritten == -1))
    {
        result = g_defaultErrorMessage;
        return;
    }

    if(formatting == noTimeStampNoTrailingNewLine)
    {
        // remove trailing new line if presents
	    if(pattern[bytesWritten - 1] == '\n')
		    pattern[bytesWritten - 1] = 0;
    }
    else
    {
        // append new line character if needed
	    if(!bytesWritten || (pattern[bytesWritten - 1] != '\n'))
		    pattern[bytesWritten] = '\n';
    }

	const size_t resultSize = 2 * g_logBufferSize + 1;
    result.assign(resultSize, 0);
	const signed int resultBytesWritten = _vsnprintf(&result[0], resultSize, &pattern[0], ap);
    if(!resultBytesWritten || (resultBytesWritten == static_cast<const int>(resultSize)) || (resultBytesWritten == -1))
        result += "... [buffer truncated]";
    else
        result.resize(static_cast<size_t>(resultBytesWritten));
}
