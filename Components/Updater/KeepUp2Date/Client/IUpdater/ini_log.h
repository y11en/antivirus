#if !defined(INI_LOG_H_INCLUDED_0E0E3E8D_CD6D_454d_B534_2BFB454AF337)
#define INI_LOG_H_INCLUDED_0E0E3E8D_CD6D_454d_B534_2BFB454AF337

#include "../helper/stdinc.h"

// TODO: when all moved to VS 2005 use new secure API, and fix secure bug
#ifdef WIN32
    // 4996 'function': was declared deprecated
    #pragma warning( disable : 4996 )
#endif

// logger class to deal with log (debug) messages
class IniLog : public KLUPD::Log
{
public:
    // enabled [in] - specifies if trace is enabled and messages should be output to trace
    IniLog(const bool enabled);
    virtual ~IniLog();

    virtual void print(const char *format, ...);

    // you must open log file before write messages to it
    // fileName [in] - log file name
    bool openFile(const KLUPD::Path &fileName);
    // close trace file
    void closeFile();


private:
    // disable copy operations
    IniLog &operator=(const IniLog &);
    IniLog(const IniLog &);

    // really pring log message with format string similar to vprintf()
    // format [in] - message's format string
    void reallyPrint(const char *format, va_list);


    // indicates in trace is enabled
    const bool m_enabled;
    // pointer to file stream
    KLUPD::AutoStream m_traceFileHandle;
};

#endif // INI_LOG_H_INCLUDED_0E0E3E8D_CD6D_454d_B534_2BFB454AF337
