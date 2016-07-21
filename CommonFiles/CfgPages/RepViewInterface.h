#ifndef REP_VIEW_INTERFACE_H
#define REP_VIEW_INTERFACE_H

/////////////////////////////////////////////////////////////////////

BOOL ShowReportLocale(LPCTSTR pszReportFilePath,	// [REQ] Report file path
					  LPCTSTR pszTitle,				// Title of report viewer window
					  LPCTSTR pszRegKey,			// Registry key (under "HKCU\Software\KasperskyLab\SharedFiles\AVPReportViewer") to save report viewer settings and window position
					  DWORD dwCompnentType,			// Type of component
					  DWORD dwTaskType,				// Type of task
					  LPCTSTR pszLocDLLPath);		// Localization DLL path

BOOL ShowReportNetwork(LPCTSTR pszReportFilePath,	// [REQ] Report file path
					   LPCTSTR pszTitle,			// Title of report viewer window
					   LPCTSTR pszRegKey,			// Registry key (under "HKCU\Software\KasperskyLab\SharedFiles\AVPReportViewer") to save report viewer settings and window position
					   DWORD dwCompnentType,		// Type of component
					   DWORD dwTaskType,			// Type of task
 					   LPCTSTR pszLocDLLPath,		// Localization DLL path
					   PVOID pParams);				// parameters policy

BOOL ShowReportCustom(LPCTSTR pszReportFilePath,	// [REQ] Report file path
					  LPCTSTR pszTitle,				// Title of report viewer window
					  LPCTSTR pszCLSID,				// CLSID of IO control object
					  LPCTSTR pszRegKey,			// Registry key (under "HKCU\Software\KasperskyLab\SharedFiles\AVPReportViewer") to save report viewer settings and window position
					  DWORD dwCompnentType,			// Type of component
					  DWORD dwTaskType,				// Type of task
					  LPCTSTR pszLocDLLPath,		// Localization DLL path
					  LPCTSTR pszUserName,			// User name
					  LPCTSTR pszPassword,			// Password
					  LPCTSTR pszAddress,			// Remote server address
					  DWORD dwPort);				// Remote server port

SCODE ShowReportLocaleEx(LPCTSTR pszReportFileName, 
						 LPCTSTR pszWindowTitle, 
						 LPCTSTR pszReportFolderRegKey, 
						 DWORD dwCompnentType, 
						 DWORD dwTaskType);

SCODE ShowReportNetworkEx(LPCTSTR pszReportFileName, 
						  LPCTSTR pszWindowTitle, 
						  LPCTSTR pszReportFolderRegKey, 
						  DWORD dwCompnentType, 
						  DWORD dwTaskType, 
						  PVOID pParams);

SCODE ShowReportCustomEx(LPCTSTR pszReportFileName, 
						 LPCTSTR pszWindowTitle,
						 LPCTSTR pszCLSIDCommunicationObject, 
						 LPCTSTR pszReportFolderRegKey, 
						 DWORD dwCompnentType, 
						 DWORD dwTaskType,
						 LPCTSTR pszUserName, 
						 LPCTSTR pszPassword,
						 LPCTSTR pszAddress, 
						 DWORD dwPort);
						 
/////////////////////////////////////////////////////////////////////

#endif