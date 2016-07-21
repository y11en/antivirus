


#include <windows.h>



BOOL InstallExe(LPCSTR p_pszExePath, LPSTR p_pszCmdLine)
{
    DWORD dwExitCode = 1 ;
    try {

        PROCESS_INFORMATION processInfo ;
        memset(&processInfo, 0, sizeof(processInfo)) ;

        STARTUPINFO startupInfo ;
        memset(&startupInfo, 0, sizeof(startupInfo)) ;
        startupInfo.cb = sizeof(STARTUPINFO) ;

        if (!::CreateProcess(p_pszExePath, p_pszCmdLine, 0, 0, false, 0, 0, 0, &startupInfo, &processInfo))
        {
            return FALSE ;
        }

        ::WaitForSingleObject(processInfo.hProcess, INFINITE) ;
        if (!::GetExitCodeProcess(processInfo.hProcess, &dwExitCode))
        {
            dwExitCode = 1 ;
        }

        ::CloseHandle(processInfo.hThread) ;
        ::CloseHandle(processInfo.hProcess) ;

    } catch (...) {

        return FALSE ;
    }

    return dwExitCode == 0 ;
}