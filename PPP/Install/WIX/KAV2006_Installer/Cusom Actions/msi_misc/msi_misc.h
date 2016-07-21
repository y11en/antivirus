
// The following ifdef block is the standard way of creating macros which make exporting 
// from a DLL simpler. All files within this DLL are compiled with the MSI_MISC_EXPORTS
// symbol defined on the command line. this symbol should not be defined on any project
// that uses this DLL. This way any other project whose source files include this file see 
// MSI_MISC_API functions as being imported from a DLL, wheras this DLL sees symbols
// defined with this macro as being exported.
#ifdef MSI_MISC_EXPORTS
#define MSI_MISC_API __declspec(dllexport)
#else
#define MSI_MISC_API __declspec(dllimport)
#endif

// property
cstring GetStrPropValue(MSIHANDLE hInstall, const char* sPropertyName);
wstring GetStrPropValueW(MSIHANDLE hInstall, const wchar_t* sPropertyName);

// logging
void installLog(MSIHANDLE hInstall, const char *logString...);
void installLogW(MSIHANDLE hInstall, const wchar_t *logString...);
void vinstallLog(MSIHANDLE hInstall, const char *logString, va_list list);
void vinstallLogW(MSIHANDLE hInstall, const wchar_t *logString, va_list list);
char * GetLastErrorStr(DWORD dwErr);
void FreeLastErrorStr(char * lpvMessageBuffer);

// AK logging
void ReportDeferredCAError(MSIHANDLE hInstall, const char*caname, int Result... );

// util
void MySplitString( const std::string & str,const std::string & strDelim, std::vector<std::string> & vectValues );

// files
bool CopyFilesA(const char * szFolderSrc, const char * szFolderTarget);
bool CopyFilesW(const wchar_t * szFolderSrc, const wchar_t * szFolderTarget);


// binary
HRESULT StreamOutBinaryData(MSIHANDLE hInstall,const char* szBinaryKey, char szFile[MAX_PATH]);

// tables
HRESULT ResolveFileTableToFileName(MSIHANDLE hInstall, const char *FileColumn, char *sFileName);
HRESULT ResolveFileTableToFullPath(MSIHANDLE hInstall, const char *FileColumn, char *sFullFilePath, INSTALLSTATE * pState = 0, INSTALLSTATE * pAction = 0);

// exec
#define EXEC_OK         0
#define EXEC_FAILED     1
#define EXEC_TIMEDOUT   2
int Exec(const char *appname, const char * cmdline, const char * dir, bool wait, DWORD& retcode, DWORD timeout = INFINITE);

