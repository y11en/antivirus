#include "stdafx.h"
#include "msi_misc.h"
#include "../common/ca_misc.h"
#include "../common/ca_data_parser.h"
#include "wowredir.h"

MSI_MISC_API UINT __stdcall SelfRegOnRebootInit(MSIHANDLE hInstall)
{
	using namespace MsiSupport::Utility;
	//MessageBox(NULL, "SelfRegModulesOnReboot", "SelfRegModulesOnReboot", MB_OK);
	installLog(hInstall,"SelfRegOnRebootInit entry point");
	cstring sDir;
	char sSelfRegFile[MAX_PATH];
	char szQuery[256];
	PMSIHANDLE hDatabase;
    PMSIHANDLE hView;
	PMSIHANDLE hRecord;
	// Set the table name 
	char szTableName[] = "SelfReg";
    
	DWORD	dwErr;

	// Define the query to be used to create the view.
	wsprintf(szQuery,"SELECT * FROM  %s ", szTableName);

	// Get the handle to the active database.
	hDatabase = MsiGetActiveDatabase(hInstall);

	// Check for success of getting the database handle.
	if(hDatabase == 0)
	{
		//MessageBox( NULL,"Error", "Unable to get handle to active database.",MB_OK);
		return ERROR_INSTALL_FAILURE;
	}

	// Open a view using the SQL query statement.
	if(MsiDatabaseOpenView(hDatabase, szQuery, &hView) 
                                          != ERROR_SUCCESS)
	{
		dwErr = GetLastError();
		//MessageBox(NULL, "Error", "Unable to open the view for the table.", MB_OK);
		return ERROR_INSTALL_FAILURE;
	}

	// Execute the view just opened.
	if(MsiViewExecute(hView, 0) != ERROR_SUCCESS)
	{
		//MessageBox(NULL, "Error", "Unable to execute the view for the table.", MB_OK);
        MsiViewClose(hView);
		return ERROR_INSTALL_FAILURE;
	}

	// Fetch the record from the view just executed.
	UINT res=MsiViewFetch(hView, &hRecord);
	if( res!= ERROR_SUCCESS)
	{
		dwErr = GetLastError();
		//MessageBox(NULL, "Error", "Unable to fetch the record from the view for the table.", MB_OK);
        MsiViewClose(hView);
		return ERROR_INSTALL_FAILURE;
	}
	else
	{
		// Continue to read  until nothing is left.
		//int i=0;
		cstring sRegFileList;
		do 
		{
			char szValueBuf[MAX_PATH] = "";
			DWORD cchValueBuf = MAX_PATH;
			UINT uiStat;
			//char sKeyName[MAX_PATH];			
			
			uiStat = MsiRecordGetString(hRecord, 1, szValueBuf, &cchValueBuf);
			if (ERROR_SUCCESS == uiStat)
			{
				//MessageBox(NULL, szValueBuf , "SelfReg", MB_OK);
                INSTALLSTATE iAction;
				if ( ResolveFileTableToFullPath(hInstall, szValueBuf, sSelfRegFile, 0, &iAction) == ERROR_SUCCESS &&
                     iAction == INSTALLSTATE_LOCAL
                   )
				{
					sRegFileList = sRegFileList + sSelfRegFile;
					sRegFileList = sRegFileList + "|";;
				}
			}
			res=MsiViewFetch(hView, &hRecord);	
		} 
        while (res == ERROR_SUCCESS);

		MsiSetProperty(hInstall, "SELFREGLIST", sRegFileList.c_str());
		installLog(hInstall, "SelfRegOnRebootInit sets SELFREGLIST property: %s", sRegFileList.c_str());
	}
	
	//Close all handles
	MsiViewClose(hView);

	return ERROR_SUCCESS;

}

MSI_MISC_API UINT __stdcall SelfRegOnRebootRun(MSIHANDLE hInstall)
{
	using namespace MsiSupport::Utility;
	//MessageBox(NULL, "SelfRegOnRebootRun", "SelfRegOnRebootRun", MB_OK);
	installLog(hInstall,"SelfRegOnRebootRun entry point");
	cstring sData;
	if (CAGetData(hInstall, sData))
	{
		CADataParser<char> parser;
		UINT res = ERROR_INSTALL_FAILURE;
		if (parser.parse(sData))
		{
			const cstring& sSelfRegApp = parser.value("SelfRegApp");
			const cstring& sSelfRegList = parser.value("SelfRegList");
			if (!sSelfRegList.empty())
			{
				std::vector<std::string> szFileNameList;
				int i =0;
				MySplitString(sSelfRegList,"|", szFileNameList);
				for (std::vector<std::string>::iterator it = szFileNameList.begin(); it != szFileNameList.end(); ++it)
				{
					if (!it->empty())
					{
						i++;
						//MessageBox(NULL, it->c_str() , "SelfReg", MB_OK);

						HKEY    hKey = NULL;
						if (RegOpenKeyEx( HKEY_LOCAL_MACHINE, "SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\RunOnce", 0, 
							KEY_ALL_ACCESS, &hKey ) == ERROR_SUCCESS) 
						{
							char sKeyName[MAX_PATH];
							char sValue[MAX_PATH*4];
							wsprintf(sKeyName, "SelfReg%i", i);
							wsprintf(sValue, "\"%s\" /s \"%s\"", sSelfRegApp.c_str(), it->c_str());
							if  (RegSetValueEx(hKey, sKeyName, 0, REG_SZ, (LPBYTE)(sValue), (DWORD)(lstrlen(sValue)+1)*sizeof(char)) == ERROR_SUCCESS)  
							{

								res=ERROR_SUCCESS;
								installLog(hInstall, "SelfRegOnRebootRun write %s", sValue);
							}

							RegCloseKey(hKey);
						}
					}
				}
			}
			else
				installLog(hInstall, "SelfRegOnRebootRun: SelfRegList is empty: %s", sSelfRegList.c_str());
		}
		else
            installLog(hInstall, "SelfRegOnRebootRun: unable to parse cadata: %s", sData.c_str());
    }
    else
        installLog(hInstall, "SelfRegOnRebootRun: unable to read cadata");

	 return ERROR_SUCCESS;

}
/*
MSI_MISC_API UINT __stdcall SelfRegx64Init(MSIHANDLE hInstall)
{
	using namespace MsiSupport::Utility;

    installLog(hInstall,"SelfRegx64Init: entry point");
	cstring sData;
	if (CAGetProperty(hInstall, "SELFREGX64", sData))
	{
        typedef std::vector<cstring> tstrvector;
        tstrvector modules;

        MySplitString(sData, ";", modules);

        cstring sRegFileList, sUnRegFileList;

        for (tstrvector::iterator i = modules.begin(); i != modules.end(); ++i)
        {
	        char sSelfRegFile[MAX_PATH];

            INSTALLSTATE iAction;
		    if ( ResolveFileTableToFullPath(hInstall, i->c_str(), sSelfRegFile, 0, &iAction) == ERROR_SUCCESS)
		    {
                switch (iAction)
                {
                case INSTALLSTATE_LOCAL:   
                    {
                        sRegFileList += sSelfRegFile;
                        sRegFileList += ";"; 
                        break;
                    }
                case INSTALLSTATE_ABSENT:
                    {
                        sUnRegFileList += sSelfRegFile;
                        sUnRegFileList += ";"; 
                        break;
                    }
                }
		    }
        }

        if (!sRegFileList.empty())
        {
            installLog(hInstall,"SelfRegx64Init: files to register: %s", sRegFileList.c_str());
            MsiSetProperty(hInstall, "SELFREGX64ADD", sRegFileList.c_str());
        }
        if (!sUnRegFileList.empty())
        {
            installLog(hInstall,"SelfRegx64Init: files to unregister: %s", sUnRegFileList.c_str());
            MsiSetProperty(hInstall, "SELFREGX64REMOVE", sUnRegFileList.c_str());
        }
    }
    else
        installLog(hInstall,"SelfRegx64: no x64 modules to self-register");

    return ERROR_SUCCESS;
}

MSI_MISC_API UINT __stdcall SelfRegx64Run(MSIHANDLE hInstall)
{
	using namespace MsiSupport::Utility;
    installLog(hInstall,"SelfRegx64Run: entry point");
	cstring sData;
	if (CAGetData(hInstall, sData))
	{
		CADataParser<char> parser;
		if (parser.parse(sData))
		{
			const cstring& sCmd = parser.value("Cmd");
			const cstring& sList = parser.value("List");

            typedef std::vector<cstring> tstrvector;
            tstrvector files;

            MySplitString(sList, ";", files);

			CWow64FsRedirectorDisabler ow64FsRedirectorDisabler;

            for (tstrvector::iterator i = files.begin() ; i != files.end (); ++i)
            {
                cstring cmdline = sCmd + " \"" + *i + "\"";

                installLog(hInstall, "SelfRegx64Run: registering %s", *i);

                DWORD retcode;
                if (Exec(NULL, cmdline.c_str(), NULL, true, retcode) != EXEC_OK)
                {
                    installLog(hInstall, "SelfRegx64Run: execution failed '%s': 0x%08X", cmdline.c_str(), GetLastError());
                    break;
                }
                installLog(hInstall, "SelfRegx64Run: return code 0x%08X", retcode);
            }
            installLog(hInstall, "SelfRegx64Run: end of the module list");
        }
        else
            installLog(hInstall, "SelfRegx64Run: unable to parse cadata: %s", sData.c_str());
    }
    else
        installLog(hInstall, "SelfRegx64Run: unable to read cadata");

    return ERROR_SUCCESS;
}

*/