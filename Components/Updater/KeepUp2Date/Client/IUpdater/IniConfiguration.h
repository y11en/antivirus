#ifndef _INICONFIGURATION_INCLUDED_3DF43686_BABA_4d84_8A85_1CD236448215
#define _INICONFIGURATION_INCLUDED_3DF43686_BABA_4d84_8A85_1CD236448215

#if _MSC_VER > 1000
    #pragma once
#endif // _MSC_VER > 1000

#include "../helper/stdinc.h"


class IniConfiguration
{
public:
    enum Operation
    {
        update,
        rollback,
    };

    IniConfiguration();
    // parse configuration
    bool parse(int argc, char *argv[]);
    // print to cerr command line parameters
    void usage()const;


    // configuration file to read settings from, default value is "ss_storage.ini"
    KLUPD::Path configurationFileName()const;

    // ini updater has a feature against run several 
    std::string applicaitonInstanceMutexName()const;

    // in case return false, the folder where executable file
     // is located is substituted instead current folder
    bool currentFolder()const;

    // a file to write readable result of update operation
    KLUPD::Path updateReadableResultFileName()const;

    // file name to write trace into
    KLUPD::Path traceFileName()const;

    // return true in case trace is enabled
    bool traceEnabled()const;

    // need suppress error dialog on access violation
    bool suppressErrorDialog()const;

    // return true in case Dump needs being writen on access violations,
     // otherwise Dump write function is turned off
    bool writeDump()const;

    // operation request
    Operation operation()const;


private:
    KLUPD::Path m_configurationFileName;
    std::string m_applicaitonInstanceMutexName;
    KLUPD::Path m_traceFileName;
    bool m_currentFolder;
    bool m_suppressErrorDialog;
    bool m_writeDump;
    Operation m_operation;
};

#endif
