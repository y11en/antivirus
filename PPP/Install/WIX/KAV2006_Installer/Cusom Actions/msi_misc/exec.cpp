#include "stdafx.h"
#include "msi_misc.h"
#include "../common/ca_misc.h"
#include "../common/ca_data_parser.h"

int Exec(const char *appname, const char * cmdline, const char * dir, bool wait, DWORD& retcode, DWORD timeout)
{
    int ret = EXEC_FAILED;

    STARTUPINFO si;
    PROCESS_INFORMATION pi;

    ZeroMemory( &si, sizeof(si) );
    si.cb = sizeof(si);
    ZeroMemory( &pi, sizeof(pi) );

    //MessageBox(NULL, cmdline, appname, MB_OK); 

    if (!CreateProcess(appname, (char*)cmdline, NULL, NULL, FALSE, 0, NULL, dir, &si, &pi))
    {
        return EXEC_FAILED;
    }

    if (wait)
    {
        switch (WaitForSingleObject(pi.hProcess, timeout))
        {
        case WAIT_TIMEOUT:
            ret = EXEC_TIMEDOUT;
            break;

        default:
            GetExitCodeProcess(pi.hProcess, &retcode);
            ret = EXEC_OK;
            break;
        }
    }
    else
    {
        ret = EXEC_OK;
    }

    CloseHandle(pi.hProcess);
    CloseHandle(pi.hThread);

    //DWORD err = GetLastError();
    //char str[256];
    //sprintf(str, "retcode=%d", retcode);
    //MessageBox(NULL, str, "CreateProcess", MB_OK);


    return ret;
}

UINT ExecCommon(MSIHANDLE hInstall, const cstring& sData)
{
    UINT err = ERROR_INSTALL_FAILURE;

	using namespace MsiSupport::Utility;

	CADataParser<char> parser;

	if (parser.parse(sData))
	{
	    const cstring& sDir     = parser.value("Dir");
	    const cstring& sExe     = parser.value("Exe");
	    const cstring& sCmd     = parser.value("Cmd");
	    bool bWait              = parser.value("Wait") != "0";

        cstring sCmdLine = cstring("\"") + sExe + "\"";

        if (!sCmd.empty())
            sCmdLine += cstring(" ") + sCmd;

        installLog(hInstall, "Exec: CommandLine = %s", sCmdLine.c_str());
        installLog(hInstall, "Exec: CurrentDir  = %s", sDir.c_str());
        installLog(hInstall, "Exec: Wait        = %s", bWait ? "yes" : "no");

        CHAR szUserName[0x1000];
        DWORD dwUserNameSize = 0x1000;
        GetUserName(szUserName, &dwUserNameSize);
        installLog(hInstall, "Exec: Username    = %s", szUserName);

        DWORD retcode = 0;

        int execret = Exec(NULL, sCmdLine.c_str(), sDir.empty() ? NULL : sDir.c_str(), bWait, retcode);
        if (execret == EXEC_OK)
        {
            if (bWait)
                installLog(hInstall, "Exec: process returned (0x%08X)", retcode);
            else
                installLog(hInstall, "Exec: process started");

            err = ERROR_SUCCESS;
        }
        else if (execret == EXEC_TIMEDOUT)
        {
            installLog(hInstall, "Exec: process timed out");
        }
        else
        {
            installLog(hInstall, "Exec: CreateProcess failed: 0x%08X", GetLastError());
        }
    }
    else
        installLog(hInstall, "Exec: unable to parse CData: %s", sData.c_str());

    return err;
}


MSI_MISC_API UINT __stdcall ExecDeferred(MSIHANDLE hInstall)
{
    UINT err = ERROR_INSTALL_FAILURE;

    installLog(hInstall, "Exec: entry point (deferred)");

	using namespace MsiSupport::Utility;
	cstring sData;

	if (CAGetData(hInstall, sData))
	{
        err = ExecCommon(hInstall, sData);
    }
    else
        installLog(hInstall, "Exec: unable to get cdata");

    return err;
}

MSI_MISC_API UINT __stdcall ExecImmediate(MSIHANDLE hInstall)
{
    UINT err = ERROR_INSTALL_FAILURE;

    installLog(hInstall, "Exec: entry point (immediate)");

	using namespace MsiSupport::Utility;
	cstring sData;

    if (CAGetProperty(hInstall, "Exec", sData))
    {
        err = ExecCommon(hInstall, sData);
    }
    else
        installLog(hInstall, "Exec: unable to get 'Exec' property");

    return err;
}

MSI_MISC_API UINT __stdcall ImportReg(MSIHANDLE hInstall)
{
	//MessageBox(NULL, NULL, "ImportReg entry point", MB_OK);
	using namespace MsiSupport::Utility;
	WIN32_FIND_DATA FindFileData;
	HANDLE hFind;

    UINT err = ERROR_INSTALL_FAILURE;

    installLog(hInstall, "ImportReg: entry point (deferred)");

	using namespace MsiSupport::Utility;
	cstring sData;

	if (CAGetData(hInstall, sData))
	{
		CADataParser<char> parser;
		if (parser.parse(sData))
		{
			const cstring& sDir = parser.value("Dir");
			const cstring& sFileName = parser.value("Name");
			installLog(hInstall,"sDir %s\n", sDir.c_str());
			installLog(hInstall,"sFileName %s\n", sFileName.c_str());
			if (!sDir.empty()&& !sFileName.empty())
			{
				SetCurrentDirectory(sDir.c_str());
				installLog(hInstall,"FindFirstFile %s", sFileName.c_str());
				hFind = FindFirstFile(sFileName.c_str(), &FindFileData);
				if (hFind == INVALID_HANDLE_VALUE) 
				{
					//printf ("Invalid File Handle. GetLastError reports %d\n", GetLastError ());
					installLog(hInstall,"FindFirstFile failed. Invalid File Handle. GetLastError reports %d\n", GetLastError());
				} 
				else 
				{
					installLog(hInstall,"FindFirstFile succeded %s", sFileName.c_str());
		            err = ExecCommon(hInstall, sData);
					FindClose(hFind);
				}
			}
        }
        else
            installLog(hInstall,"Extract: CData parsing failed: %s", sData.c_str());
        
    }
    else
        installLog(hInstall, "ImportReg: unable to get cdata");

    return err;
}
