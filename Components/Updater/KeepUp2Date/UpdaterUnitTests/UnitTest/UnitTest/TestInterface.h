#ifndef TESTINTERFACE_H
#define TESTINTERFACE_H


#include "..\..\..\Client\Include\journal_iface.h"
#include "..\..\..\Client\Include\callback_iface.h"
#include "..\..\..\Client\Include\log_iface.h"
#include "..\..\..\Client\Include\requests_ids.h"
#include "..\..\..\Client\Include\message.h"
#include "..\..\..\Client\ini_libs\ini_common.h"
#include "ini_updater.h"


#include <time.h>
#include <process.h>

#include <stdarg.h>


class INI_LIBS_API Ini_Log : public Log
{
public:
    /// @param enabled [in] specifies if trace is enabled
    Ini_Log(const bool enabled);

    virtual ~Ini_Log();

    virtual void print(const char *format, ...);

    /// you must open log file before write messages to it
    /// @param fileName [in] log file name
    bool openFile(const std::string &fileName);
    /// close trace file
    void closeFile();


private:
    // disable copy operations
    Ini_Log &operator=(const Ini_Log &);
    Ini_Log(const Ini_Log &);

    /// really pring log message with format string similar to vprintf()
    /// @param format [in] message's format string
    /// @param va [in] va_list argument
    void reallyPrint(const char *format, va_list ap);


    // indicates in trace is enabled
    const bool m_enabled;
    /// pointer to file stream
    FILE *m_traceFileHandle;

};


/// interface for journaling events
class Ini_Journal : public JournalInterface
{
public:
    Ini_Journal(Log * = 0);
    
    virtual void publishMessage(const KLUPD::CoreError &, const STRING &parameter = STRING());
    
private:
    // disable copy operations
    Ini_Journal &operator=(const Ini_Journal &);
    Ini_Journal(const Ini_Journal &);
    
    Log *pLog;
};

class IniCallbacks : public CallbackInterface
{
public:
    virtual Command checkCallback(const Request &);
    virtual void percentCallback(const Message *);

private:
    Reply requesterWrapper(const int requestType);
};



#endif