#include <string>
#include <iostream>

#include <windows.h>
#include <time.h>


SERVICE_STATUS_HANDLE g_handle = 0;

static char *g_serviceName = "startCmdAsNTService";

VOID WINAPI handler(DWORD fdwControl)
{
    SERVICE_STATUS serviceStatus;
    serviceStatus.dwServiceType = SERVICE_WIN32_OWN_PROCESS;
    serviceStatus.dwControlsAccepted = SERVICE_ACCEPT_STOP;
    serviceStatus.dwWin32ExitCode = 0;
    serviceStatus.dwServiceSpecificExitCode = 0;
    serviceStatus.dwCheckPoint = static_cast<DWORD>(time(0));
    serviceStatus.dwWaitHint = 1000;

    if(fdwControl == SERVICE_CONTROL_STOP || fdwControl == SERVICE_CONTROL_SHUTDOWN)
        serviceStatus.dwCurrentState = SERVICE_STOPPED;
    else
        serviceStatus.dwCurrentState = SERVICE_RUNNING;

    SetServiceStatus(g_handle, &serviceStatus);
}



VOID WINAPI ServiceMain(DWORD dwArgc, LPTSTR* lpszArgv)
{
    g_handle = RegisterServiceCtrlHandler(g_serviceName, handler);
    if(!g_handle)
    {
        std::cerr << "register service control handler failed" << std::endl;
        return;
    }


    handler(SERVICE_RUNNING);

    ShellExecute(0, 0, "cmd.exe", 0, 0, SW_SHOW);
}




int main(int argc, char *argv[])
{
    // regestering service
    if(argc == 2)
    {
        std::string parameter(argv[1]);


        // start NT-service
        if(parameter == "-s")
        {
            SERVICE_TABLE_ENTRY serviceTable[] = { { g_serviceName, ServiceMain }, { 0, 0 }  }; 
            if(StartServiceCtrlDispatcher(serviceTable) == FALSE)
            {
                std::cerr << "start service control dispatcher failed, error " << GetLastError() << std::endl;
                return -1;
            }
	        ServiceMain(argc, argv);
            return 0;
        }

        SC_HANDLE scManager = OpenSCManager(0, 0, SC_MANAGER_ALL_ACCESS);
        if(!scManager)
        {
            std::cerr << "failed to open SCM" << std::endl;
            return -1;

        }

        // install NT-service
        if(parameter == "-i")
        {
            const size_t bufferSize = 1024;
            char filename[bufferSize];
            if(!GetModuleFileName(0, filename, bufferSize))
            {
                std::cerr << "unable to get module file name" << std::endl;
                return -1;
            }
            std::string commandLine = filename;
            commandLine = std::string("\"") + commandLine + "\" -s";

            if(!CreateService(scManager, g_serviceName, g_serviceName,
                SC_MANAGER_ALL_ACCESS, SERVICE_WIN32_OWN_PROCESS, SERVICE_DEMAND_START,
                SERVICE_ERROR_IGNORE, commandLine.c_str(), 0, 0, 0, 0, 0))
            {
                std::cerr << "failed to create service, error " << GetLastError() << std::endl;
                return -1;
            }


            std::cout << "service installed" << std::endl;
            return 0;
        }

        // remove NT-service
        if(parameter == "-r")
        {
            // deleting service if start with any other parameter
            SC_HANDLE serviceHandle = OpenService(scManager, g_serviceName, SERVICE_ALL_ACCESS);
            if(!serviceHandle)
            {
                std::cerr << "failed to open service" << std::endl;
                return -1;
            }

            if(!DeleteService(serviceHandle))
            {
                std::cerr << "failed to delete service from SCM" << std::endl;
                return -1;
            }

            std::cout << "service deleted from SCM" << std::endl;
            return 0;
        }
    }


    std::cout << "This application allows to start command line as NT Service." << std::endl
        << "  To be able to see command-line run under local system account"  << std::endl
        << " you should configure service to interact with user. Start command"  << std::endl
        << " line as NT service via Service Management snap-in after installation."  << std::endl
        << std::endl
        << "Usage: startCmdAsNTService <-i|-r|-h>"  << std::endl
        << "\t-i  install application"  << std::endl
        << "\t-r  remove application"  << std::endl
        << "\t-h  print this help"  << std::endl;

    return 0;
}