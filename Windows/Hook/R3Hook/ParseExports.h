#ifndef __PARSE_FILE_EXPORTS_
#define __PARSE_FILE_EXPORTS_

#include <windows.h>

BOOL LoadModuleData(TCHAR* pModulePath, PBYTE* ppModuleData, DWORD* pdwSize);
BOOL LoadModuleDataByHandle(HMODULE hModule, PBYTE* ppModuleData, DWORD* pdwSize);
void FreeModuleData(PBYTE* ppModuleData, DWORD* pdwSize);
DWORD GetProcRVA(PBYTE pModuleData, char* pProcName);
FARPROC GetRealProcAddress(HMODULE hBaseAddress, PBYTE pModuleData, char* pProcName);

#ifdef __cplusplus

class cFileExports {
public:
	cFileExports(TCHAR* pModuleName) 
	{
		m_pModuleData = NULL;
		LoadModuleData(pModuleName, &m_pModuleData, 0);
	};

	cFileExports(HMODULE hModule)
	{
		m_pModuleData = NULL;
		LoadModuleDataByHandle(hModule, &m_pModuleData, 0);
	}
	~cFileExports()
	{
		if (m_pModuleData)
			FreeModuleData(&m_pModuleData, 0);
	}
	DWORD GetProcRVA(char* pProcName)
	{
		if (!m_pModuleData)
			return 0;
		return ::GetProcRVA(m_pModuleData, pProcName);
	}
	void* GetProcAddress(HMODULE hBaseAddress, char* pProcName)
	{
		return GetRealProcAddress(hBaseAddress, m_pModuleData, pProcName);
	}
private:
	PBYTE m_pModuleData;
};

#endif // __cplusplus

#endif //__PARSE_FILE_EXPORTS_
