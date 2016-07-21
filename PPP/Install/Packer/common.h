


#include <windows.h>



class CAutoFile
{
public:

    CAutoFile(LPCSTR p_pFilePath, DWORD dwDesiredAccess, DWORD p_dwShareMode, DWORD p_dwCreationDisposition)
    {
        m_pFile = ::CreateFile(p_pFilePath, dwDesiredAccess, p_dwShareMode, NULL, p_dwCreationDisposition, FILE_ATTRIBUTE_NORMAL, NULL) ;
    }

    ~CAutoFile()
    {
        if (m_pFile)
        {
            ::CloseHandle(m_pFile) ;
        }
    }

    operator !()
    {
        return !m_pFile ;
    }

    operator HANDLE()
    {
        return m_pFile ;
    }

protected:
    HANDLE  m_pFile ;
};



BOOL DoPack(LPSTR lpCmdLine) ;

BOOL DoInstall(PSTR lpCmdLine) ;

std::string AddPath(const char* p_pszFirstPath, const char* p_pszSecondPath) ;

BOOL GetFileSize(LPCSTR p_pszFilePath, DWORD& dwFileSize) ;

std::string GetFileName(LPCSTR p_pszFilePath) ;

BOOL GetFileContent(LPCSTR p_pszFilePath, char** p_pBuff, DWORD& p_dwBuffSize) ;

BOOL SaveDword(HANDLE p_pStorage, DWORD p_dw) ;

BOOL SaveBuffer(HANDLE p_pStorage, void* p_pBuff, DWORD p_dwBuffSize) ;

BOOL SaveFile(HANDLE p_pStorage, LPCSTR p_pszFilePath, BOOL p_blSaveSize = TRUE) ;

BOOL LoadDword(HANDLE p_pStorage, DWORD& p_dw) ;

BOOL LoadBuffer(HANDLE p_pStorage, char** p_pBuff, DWORD& p_dwBuffSize) ;

BOOL LoadFile(HANDLE p_pStorage, LPCSTR p_pszFilePath) ;

BOOL Encode(const char* p_pBuff, DWORD p_dwBuffSize, char** p_pResult, DWORD& p_dwResultSize) ;

BOOL Decode(const char* p_pBuff, DWORD p_dwBuffSize, char** p_pResult, DWORD& p_dwResultSize) ;

BOOL InstallExe(LPCSTR p_pszExePath, LPSTR p_pszCmdLine) ;

BOOL InstallIni(LPCSTR p_pszFilePath) ;

BOOL UninstallIni() ;