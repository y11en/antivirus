#include "PragueLog.h"

EXTERN_C hROOT g_root;

PRAGUE_HELPERS::PragueLog::PragueLog(const char *decorationPrefix, const tTRACE_LEVEL &traceLevel)
  : m_decorationPrefix(decorationPrefix),
    m_traceLevel(traceLevel)
{
}

void PRAGUE_HELPERS::PragueLog::print(const char *format, ...)
{
    std::string logMessage;

	va_list ap;
	va_start(ap, format);
    makeTraceMessageFromEllipsis(logMessage, format, noTimeStampNoTrailingNewLine, m_decorationPrefix, ap);
	va_end(ap);

    PR_TRACE((g_root, m_traceLevel, "%s", logMessage.c_str()));
}

