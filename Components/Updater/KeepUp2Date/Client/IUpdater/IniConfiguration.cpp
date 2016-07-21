#include "IniConfiguration.h"


IniConfiguration::IniConfiguration()
 : m_configurationFileName(L"ss_storage.ini"),
   m_applicaitonInstanceMutexName("MUTEX_UPDATER_INI_VERSION_13_"),
   m_currentFolder(false),
   m_suppressErrorDialog(false),
   m_writeDump(true),
   m_operation(update)
{
}

bool IniConfiguration::parse(int argc, char *argv[])
{
    // parse options
    for(int arg = 1; arg < argc; ++arg)
    {
        const KLUPD::NoCaseString currentParameter = KLUPD::asciiToWideChar(argv[arg]).c_str();
        const std::string secondParameter = (arg < argc - 1) ? argv[arg + 1] : "";

        // trace file name
        if((currentParameter == L"-tf") && !secondParameter.empty())
        {
            arg++;
            m_traceFileName = KLUPD::asciiToWideChar(secondParameter);
        }

        // instance mutex name
        else if((currentParameter == L"-m") && !secondParameter.empty())
        {
            arg++;
            m_applicaitonInstanceMutexName += secondParameter;
        }

        // configuration file name
        else if((currentParameter == L"-o") && !secondParameter.empty())
        {
            arg++;
            m_configurationFileName = KLUPD::asciiToWideChar(secondParameter);
        }

        // strict syntax - first argument is 'mode' identifier, others - options
        else if((currentParameter == L"-u") || (currentParameter == L"--update"))
            m_operation = update;
        else if((currentParameter == L"-r") || (currentParameter == L"--rollback"))
            m_operation = rollback;

        // use current folder
        else if(currentParameter == L"-c")
            m_currentFolder = true;

        // suppress error dialog on write dump
        else if(currentParameter == L"-s")
            m_suppressErrorDialog = true;

        // disable dump write on access violation
        else if(currentParameter == L"-disableDumpWrite")
            m_writeDump = false;

        else
        {
            usage();
            return false;
        }
    }

    if(argc <= 1)
    {
        usage();
        return false;
    }
    return true;
}

void IniConfiguration::usage()const
{
    std::cerr << "Usage: updater.exe {-u|-r|-h} [-o <file name>] [-c] [-tf <file name>] [-m <instance name>][-s]\n"
        "\t-u  - perform update\n"
        "\t-r  - perform rollback\n"
        "\t-h  - print help\n"
        "\t-o <file name> - configuration file to read settings from, 'ss_strorage.ini' file is used in case option is omitted\n"
        "\t-tf <file name> - turn trace on, trace file name\n"
        "\t-c - use current folder. In case parameter is not set use folder relative to updater.exe\n"
        "\t-m <instance name> - unique IUpdater identifier to allow run more then one IUpdater at the same time\n"
        "\t-s - suppress error dialog if Updater crashes\n"
        "\t-disableDumpWrite - turn off dump write on access violations\n"
        "\nSamples:\n"
        "\tupdater.exe -u -o ss_storage.ini -m Development -tf IUpdater.log -c\n"
        "\tupdater.exe -r -tf IUpdater.log\n";
}

KLUPD::Path IniConfiguration::configurationFileName()const
{
    return m_configurationFileName;
}

std::string IniConfiguration::applicaitonInstanceMutexName()const
{
    return m_applicaitonInstanceMutexName;
}

bool IniConfiguration::currentFolder()const
{
    return m_currentFolder;
}

KLUPD::Path IniConfiguration::updateReadableResultFileName()const
{
    return L"iupdater.txt";
}

KLUPD::Path IniConfiguration::traceFileName()const
{
    return m_traceFileName;
}

bool IniConfiguration::traceEnabled()const
{
    return !m_traceFileName.empty();
}

bool IniConfiguration::suppressErrorDialog()const
{
    return m_suppressErrorDialog;
}

bool IniConfiguration::writeDump()const
{
    return m_writeDump;
}

IniConfiguration::Operation IniConfiguration::operation()const
{
    return m_operation;
}

