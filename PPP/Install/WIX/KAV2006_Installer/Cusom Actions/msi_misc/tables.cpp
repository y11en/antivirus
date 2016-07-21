#include "stdafx.h"
#include "msi_misc.h"
#include "../common/ca_misc.h"
#include "../common/ca_data_parser.h"

HRESULT ResolveComponentTable(MSIHANDLE hInstall, MSIHANDLE hDatabase, char *ComponentColumn, char *sDir)
{
	//MessageBox(NULL, "ResolveComponentTable", "ResolveComponentTable", MB_OK);
	installLog(hInstall,"ResolveComponentTable entry point");
	char szQuery[256];
	PMSIHANDLE hView;
	PMSIHANDLE hRecord;
	DWORD	dwErr;
	// Set the table name 
	char szTableName[] = "Component";

	wsprintf(szQuery,"SELECT * FROM  %s WHERE Component=\'%s\'", szTableName, ComponentColumn);

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
		char szValueBuf[MAX_PATH] = "";
		DWORD cchValueBuf = MAX_PATH;
		UINT uiStat;
		uiStat = MsiRecordGetString(hRecord, 3, szValueBuf, &cchValueBuf);
		if (ERROR_SUCCESS == uiStat)
		{
			//MessageBox(NULL, szValueBuf, "ResolveComponentsTable", MB_OK);
			strcpy(sDir, szValueBuf);
			installLog(hInstall, "ResolveComponentTable return %s", sDir);
		}
	}
	
	//Close all handles
	MsiViewClose(hView);

	return ERROR_SUCCESS;

}

HRESULT ResolveFileTableToFileName(MSIHANDLE hInstall, const char *FileColumn, char *sFileName)
{
    PMSIHANDLE hDatabase = MsiGetActiveDatabase(hInstall);

    installLog(hInstall,"ResolveFileTableToFileName: entry point (%s)", FileColumn);
	char szQuery[256];
	PMSIHANDLE hView;
	PMSIHANDLE hRecord;
	// Set the table name 
	char szTableName[] = "File";
	DWORD	dwErr;
	HRESULT hResult = S_OK;

	wsprintf(szQuery,"SELECT * FROM  %s WHERE File=\'%s\'", szTableName, FileColumn);
	// Open a view using the SQL query statement.
	if( (hResult = MsiDatabaseOpenView(hDatabase, szQuery, &hView)) 
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
		char szFileName[MAX_PATH] = "";
		DWORD cchValueBuf = MAX_PATH;
        UINT uiStat;
		uiStat = MsiRecordGetString(hRecord, 3, szFileName, &cchValueBuf);
		if (ERROR_SUCCESS == uiStat)
		{
			//MessageBox(NULL, szFileName, "FileName", MB_OK);
			// check if filename long
			if (strchr(szFileName, '|') != 0)
			{
				std::vector<std::string> szFileNameList;
				MySplitString(szFileName,"|", szFileNameList);
				strcpy(szFileName, szFileNameList[1].c_str());
			}
		}

        strcpy(sFileName, szFileName);
        installLog(hInstall, "ResolveFileTableToFileName: return %s", sFileName);
		
	}
	
	//Close all handles
	MsiViewClose(hView);
	
	return ERROR_SUCCESS;
}

HRESULT ResolveFileTableToFullPath(MSIHANDLE hInstall, const char *FileColumn, char *sFullFilePath, INSTALLSTATE * pState, INSTALLSTATE * pAction)
{
    PMSIHANDLE hDatabase = MsiGetActiveDatabase(hInstall);

    installLog(hInstall,"ResolveFileTableToFullPath: entry point (%s)", FileColumn);
	char szQuery[256];
	PMSIHANDLE hView;
	PMSIHANDLE hRecord;
	// Set the table name 
	char szTableName[] = "File";
	DWORD	dwErr;
	HRESULT hResult = S_OK;

	wsprintf(szQuery,"SELECT * FROM  %s WHERE File=\'%s\'", szTableName, FileColumn);
	// Open a view using the SQL query statement.
	if( (hResult = MsiDatabaseOpenView(hDatabase, szQuery, &hView)) 
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
		char szComponent[MAX_PATH] = "";		
		cstring sDir;
		char sDirectory[100];
		DWORD cchValueBuf = MAX_PATH;
		UINT uiStat;
		uiStat = MsiRecordGetString(hRecord, 2, szComponent, &cchValueBuf);
		if (ERROR_SUCCESS == uiStat)
		{
			//MessageBox(NULL, szComponent, "Component", MB_OK);
			// check if component really installed
			INSTALLSTATE iInstalled;
			INSTALLSTATE iAction;
			if (MsiGetComponentState(hInstall, szComponent, &iInstalled, &iAction) == ERROR_SUCCESS)
            {
                if (pState)  *pState = iInstalled;
                if (pAction) *pAction = iAction;

                ResolveComponentTable(hInstall, hDatabase,szComponent, sDirectory);
			    sDir = GetStrPropValue(hInstall, sDirectory);
            }
            else
            {
                MsiViewClose(hView);
                return ERROR_INSTALL_FAILURE;
            }
		}

		char szFileName[MAX_PATH] = "";
		cchValueBuf = MAX_PATH;
		uiStat = MsiRecordGetString(hRecord, 3, szFileName, &cchValueBuf);
		if (ERROR_SUCCESS == uiStat)
		{
			//MessageBox(NULL, szFileName, "FileName", MB_OK);
			// check if filename long
			if (strchr(szFileName, '|') != 0)
			{
				std::vector<std::string> szFileNameList;
				MySplitString(szFileName,"|", szFileNameList);
				strcpy(szFileName, szFileNameList[1].c_str());
			}
		}

		strcpy(sFullFilePath, sDir.c_str());
		strcat(sFullFilePath, szFileName);
        installLog(hInstall, "ResolveFileTableToFullPath: return %s", sFullFilePath);
		
	}

    MsiViewClose(hView);

	return ERROR_SUCCESS;
}
