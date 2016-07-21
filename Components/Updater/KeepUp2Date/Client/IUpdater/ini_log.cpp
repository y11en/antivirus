#include "ini_log.h"

IniLog::IniLog(const bool enabled)
 : m_enabled(enabled),
   m_traceFileHandle(0)
{
}

IniLog::~IniLog()
{
	closeFile();
}

void IniLog::print(const char *format, ...)
{
	va_list ap;
	va_start(ap, format);
	reallyPrint(format, ap);
	va_end(ap);
}

bool IniLog::openFile(const KLUPD::Path &fileName)
{
    if(!m_enabled)
        return true;

    if(fileName.empty())
    {
        std::cerr << "Failed to open trace file, because no file name is supplied" << std::endl;
        return false;
    }

    closeFile();

    // create folder for trace file
    const KLUPD::Path traceFolder = fileName.getFolderFromPath();
    if(!traceFolder.empty() && !KLUPD::mkdirWrapper(traceFolder))
        std::cerr << "Failed to create folder for trace file '" << traceFolder.toAscii() << "'" << std::endl;

    const KLUPD::CoreError openLogFileResult = m_traceFileHandle.open(fileName, L"an");
    if(!KLUPD::isSuccess(openLogFileResult))
    {
        std::cerr << "Failed to open trace file '" << fileName.toAscii()
            << "', result " << KLUPD::toString(openLogFileResult).toAscii() << std::endl;
        return false;
    }

    // file is opened for append
    fseek(m_traceFileHandle.stream(), 0, SEEK_END);
    
    ////////////////////
    time_t ltime;
    time(&ltime);
    char tmstr[200];
    sprintf(tmstr, "%s", (ltime != -1) ? ctime(&ltime) : "");
    tmstr[strlen(tmstr) - 1] = 0; // erase 0d0a from the end
    print("=========================================================");
    print("=====  Trace log started. %24s  =====", tmstr);
    print("=========================================================");

    return true;
}

void IniLog::closeFile()
{
    if(!m_traceFileHandle.stream() || !m_enabled)
        return;

    time_t ltime;
    time(&ltime);
    char tmstr[200];
    sprintf(tmstr, "%s", (ltime != -1) ? ctime(&ltime) : "");
    tmstr[strlen(tmstr) - 1] = 0; // erase 0d0a from the end
    print("=========================================================");
    print("=====  Trace log finished. %24s =====", tmstr);
    print("=========================================================");
}

void IniLog::reallyPrint(const char *format, va_list ap) 
{
    if(!m_enabled || !m_traceFileHandle.stream()) 
        return;

    std::string logMessage;
    makeTraceMessageFromEllipsis(logMessage, format, withTimeStamp, "", ap);

    fwrite(logMessage.c_str(), logMessage.size(), 1, m_traceFileHandle.stream());
    fflush(m_traceFileHandle.stream());
}

