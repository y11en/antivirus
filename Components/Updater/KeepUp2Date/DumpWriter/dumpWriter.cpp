#include "dumpWriter.h"

#include <TCHAR.h>
#include <shlwapi.h>
#include <time.h>
#include <dbghelp.h>

#include <string>

// TODO: when all moved to VS 2005 use new secure API, and fix secure bug
#ifdef WIN32
    // 4996 'function': was declared deprecated
    #pragma warning( disable : 4996 )
#endif


/// the DbgHelpWrapper class initialized and clean resources in safe way
///  and is a wrapper for dbghelp.dll library
class MiniDumper::DbgHelpWrapper
{
public:
    DbgHelpWrapper();
    ~DbgHelpWrapper();

    LONG miniDumpWriteDump(_EXCEPTION_POINTERS *);

    /// indicates whether dbghelp.dll has been found
    bool loaded()const;

    /// bool suppress error dialog box
    static void suppressErrorDialog();

private:
    typedef BOOL (WINAPI *MINIDUMPWRITEDUMP)(HANDLE hProcess,
                                     DWORD dwPid,
                                     HANDLE hFile,
                                     MINIDUMP_TYPE DumpType,
                                     CONST PMINIDUMP_EXCEPTION_INFORMATION,
                                     CONST PMINIDUMP_USER_STREAM_INFORMATION,
                                     CONST PMINIDUMP_CALLBACK_INFORMATION);

    /// generates dump filename from current date
    void generateDumpFileName();

    HMODULE m_module;
    MINIDUMPWRITEDUMP m_miniDumpWriteDump;
    HANDLE m_dumpFileHandle;

    // while writing dump the STL should be avoided, that is why buffer is used for dump file name
    TCHAR m_dumpFileName[_MAX_PATH];

    /// flag indicates whether dialog on error should be suppressed
    static bool s_suppressErrorDialog;
};

bool MiniDumper::DbgHelpWrapper::s_suppressErrorDialog = false;

MiniDumper::DbgHelpWrapper::DbgHelpWrapper()
 : m_module(0),
   m_miniDumpWriteDump(0),
   m_dumpFileHandle(INVALID_HANDLE_VALUE)
{
    // firstly see if dbghelp.dll is around and has the function we need look next to the EXE first,
    //  as the one in System32 might be old (e.g. Windows 2000)
    TCHAR dbgHelpPath[_MAX_PATH];
    if(GetModuleFileName(0, dbgHelpPath, _MAX_PATH))
    {
        TCHAR *pSlash = _tcsrchr(dbgHelpPath, _T('\\'));
        if(pSlash)
        {
            _tcscpy(pSlash + 1, _T("DBGHELP.DLL"));
            m_module = ::LoadLibrary(dbgHelpPath);
        }
    }
    
    // load any version we can
    if(!m_module)
        m_module = ::LoadLibrary(_T("DBGHELP.DLL"));

    m_miniDumpWriteDump = (MINIDUMPWRITEDUMP)::GetProcAddress(m_module, "MiniDumpWriteDump");

    memset(m_dumpFileName, 0, _MAX_PATH * sizeof(TCHAR));
    generateDumpFileName();
}
MiniDumper::DbgHelpWrapper::~DbgHelpWrapper()
{
}
bool MiniDumper::DbgHelpWrapper::loaded()const
{
    return (m_module != 0) && (m_miniDumpWriteDump != 0); //  !! to suppress compiler warning
}

void MiniDumper::DbgHelpWrapper::suppressErrorDialog()
{
    s_suppressErrorDialog = true;
}


LONG MiniDumper::DbgHelpWrapper::miniDumpWriteDump(_EXCEPTION_POINTERS *exceptionInfo)
{
    // create the file
    m_dumpFileHandle = ::CreateFile(m_dumpFileName, GENERIC_WRITE, FILE_SHARE_WRITE, 0, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, 0);
    // failed to create dump file
    if(m_dumpFileHandle == INVALID_HANDLE_VALUE)
        return EXCEPTION_CONTINUE_SEARCH;

    _MINIDUMP_EXCEPTION_INFORMATION ExInfo;
    ExInfo.ThreadId = ::GetCurrentThreadId();
    ExInfo.ExceptionPointers = exceptionInfo;
    ExInfo.ClientPointers = 0;

    // write the dump to file
    if(m_miniDumpWriteDump(GetCurrentProcess(), GetCurrentProcessId(), m_dumpFileHandle, MiniDumpWithFullMemory, &ExInfo, 0, 0))
    {
        if(!s_suppressErrorDialog)
            MessageBox(0, m_dumpFileName, _T("Updater component crashed. Dump written"), MB_ICONSTOP | MB_SERVICE_NOTIFICATION);
    }

    TerminateProcess(GetCurrentProcess(), -1);
    return EXCEPTION_CONTINUE_SEARCH;
}

void MiniDumper::DbgHelpWrapper::generateDumpFileName()
{
#if defined(UNICODE) || defined(_UNICODE)
    std::wstring
#else
    std::string
#endif
        result(_T("kasperskyUpdater_"));

    // append current time
    TCHAR buffer[32];
    memset(buffer, 0, sizeof(buffer));

#if defined(UNICODE) || defined(_UNICODE)
    _itow(static_cast<const int>(time(0)), buffer, 10);
#else
    itoa(static_cast<const int>(time(0)), buffer, 10);
#endif

    result += buffer;

    // append extention
    result += _T(".dmp");
    memcpy(m_dumpFileName, result.c_str(), result.size() * sizeof(TCHAR));
}


void MiniDumper::enable()
{
    ::SetUnhandledExceptionFilter(topLevelFilter);
}

void MiniDumper::suppressErrorDialog()
{
    MiniDumper::DbgHelpWrapper::suppressErrorDialog();
}

LONG MiniDumper::topLevelFilter(_EXCEPTION_POINTERS *exceptionInfo)
{
    // loading dbghelp.dll
    DbgHelpWrapper dbgHelpWrapper;
    if(!dbgHelpWrapper.loaded())
        return EXCEPTION_CONTINUE_SEARCH;


    return dbgHelpWrapper.miniDumpWriteDump(exceptionInfo);
}

