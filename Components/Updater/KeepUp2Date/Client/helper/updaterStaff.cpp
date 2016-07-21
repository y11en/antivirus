#include "local_file.h"

#include "updaterStaff.h"

KAVUPDCORE_API KLUPD::NoCaseString KLUPD::errnoToString(const int lastError, const Style &style)
{
    std::ostringstream stream;
    stream.imbue(std::locale::classic());

    // platform specific code, because strerror() is not exact on Windows
#ifdef WIN32
    if(style == posixStyle)
        stream << strerror(lastError);
    else
    {
        LPTSTR buffer = 0;
        const int resultSize = FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM,
            0, lastError, 0, reinterpret_cast<LPTSTR>(&buffer), 0, 0);
        if(resultSize)
        {
            std::string errorString = tStringToAscii(buffer);

            // removing '\r\n\r\n' for more convenient output
            errorString.erase(std::remove_if(errorString.begin(), errorString.end(),
                std::bind2nd(std::equal_to<std::string::value_type>(), '\r')), errorString.end());
            errorString.erase(std::remove_if(errorString.begin(), errorString.end(),
                std::bind2nd(std::equal_to<std::string::value_type>(), '\n')), errorString.end());

            stream << errorString;
        }

        if(buffer)
            LocalFree(buffer);
    }

#else
    stream << strerror(lastError);
#endif

    stream << "(" << lastError << ")";
    return asciiToWideChar(stream.str());
}


KLUPD::CoreError KLUPD::lastErrorToUpdaterFileErrorCode(const int lastError)
{
    switch(lastError)
    {
    // Operation not permitted
    case EPERM:
    // Permission denied
    case EACCES:
     // Read-only file system
    case EROFS:
        return CORE_NotEnoughPermissions;

    // No such file or directory
    case ENOENT:
        return CORE_NoSuchFileOrDirectory;

    // no detalization for other codes
    default:
        return CORE_GenericFileOperationFailure;
    }
}

bool KLUPD::isFTPorHTTP(const EUpdateSourceType &type, const Path &url)
{
    if((type == UST_KLServer) && url.emptyIfSingleSlash().empty())
        return true;
    
    const Path httpPrefix = L"http://";
    if(httpPrefix.size() <= url.size())
    {
        if(url.find(httpPrefix) != NoCaseStringImplementation::npos)
            return true;
    }

    const Path ftpPrefix = L"ftp://";
    if(ftpPrefix.size() <= url.size())
    {
        if(url.find(ftpPrefix) != NoCaseStringImplementation::npos)
            return true;
    }

    return false;
}

#ifndef WIN32
    #ifndef HAVE_TIMEGM 
        // timegm() function doesn't exist under some UNIX compatible OS
        time_t my_timegm(struct tm *tm)
        {
            char *tz = getenv("TZ");
            setenv("TZ", "", 1);
            tzset();
            const time_t ret = mktime(tm);
            if(tz)
                setenv("TZ", tz, 1);
            else
                unsetenv("TZ");
            tzset();
            return ret;
        }
    #endif
#endif


unsigned long KLUPD::updateDateStringToSequenceNumber(const NoCaseString &dateString)
{
    struct tm timeTm;
    memset(&timeTm, 0, sizeof(timeTm));
    if(!strptime(dateString.toAscii().c_str(), "%d%m%Y %H%M", &timeTm))
        return 0;

    time_t timeT;
#ifdef WIN32
    struct timeb timeB;
    ftime(&timeB);
    timeT = mktime(&timeTm) - timeB.timezone * 60;
#else
    #ifdef HAVE_TIMEGM
        timeT = timegm(&timeTm);
    #else
        timeT = my_timegm(&timeTm);
    #endif
#endif  // WIN32
    return static_cast<unsigned int>(timeT);
}

unsigned long KLUPD::addDays(const unsigned long date, const unsigned int daysToAdd)
{
    // internal representation is time_t structure
    return date + daysToAdd * 24 * 60 * 60;
}

KLUPD::CoreError KLUPD::saveDataToFile(const Path &fullFileName,
                                       const unsigned char *data, const size_t length, const bool append,
                                       AutoStream &stream, Log *pLog)
{
    if(!data || length < 0)
    {
        TRACE_MESSAGE2("Failed to save data to file, invalid parameter, for file '%S'", fullFileName.toWideChar());
        return CORE_GenericFileOperationFailure;
    }

    if(!stream.stream())
    {
        const CoreError openResult = stream.open(fullFileName, append ? L"a+b" : L"wb");
        if(!isSuccess(openResult))
        {
            TRACE_MESSAGE3("Failed to save data to file '%S', result '%S'",
                fullFileName.toWideChar(), toString(openResult).toWideChar());
            return openResult;
        }
    }

    // error while writing
    if((length > 0) && (fwrite(data, 1, length, stream.stream()) != length))
    {
        const int lastError = errno;
        TRACE_MESSAGE3("Failed to save data, unable to write to file '%S', last error '%S'",
            fullFileName.toWideChar(), errnoToString(lastError, posixStyle).toWideChar());
        return lastErrorToUpdaterFileErrorCode(lastError);
    }

    return CORE_NO_ERROR;
}

//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
