#include <std/base/klbase.h>

#include <string>
#include <vector>

namespace KLSTD
{
    class KlRegKey
    {
    public:
	    KlRegKey();
	    ~KlRegKey();

    // Attributes
    public:
	    operator HKEY() const;
	    HKEY m_hKey;

    // Operations
    public:
	    LONG SetValue(DWORD dwValue, LPCWSTR lpszValueName);
	    LONG SetValue(LPCWSTR lpszValue, LPCWSTR lpszValueName = NULL);

	    LONG QueryValue(DWORD& dwValue, LPCWSTR lpszValueName);
        LONG QueryValue(std::wstring& wstrValue, LPCWSTR lpszValueName);


	    LONG SetKeyValue(LPCWSTR lpszKeyName, LPCWSTR lpszValue, LPCWSTR lpszValueName = NULL);
	    static LONG WINAPI SetValue(HKEY hKeyParent, LPCWSTR lpszKeyName,
		    LPCWSTR lpszValue, LPCWSTR lpszValueName = NULL);

	    LONG Create(
                HKEY hKeyParent, 
                LPCWSTR lpszKeyName,
		        LPWSTR lpszClass = REG_NONE, 
                DWORD dwOptions = REG_OPTION_NON_VOLATILE,
		        REGSAM samDesired = KEY_READ|KEY_WRITE,
		        LPSECURITY_ATTRIBUTES lpSecAttr = NULL,
    		    LPDWORD lpdwDisposition = NULL);

	    LONG Open(HKEY hKeyParent, LPCWSTR lpszKeyName,
		        REGSAM samDesired);

	    LONG Close();
	    HKEY Detach();
	    void Attach(HKEY hKey);
	    LONG DeleteSubKey(LPCWSTR lpszSubKey);
	    LONG RecurseDeleteKey(LPCWSTR lpszKey);
	    LONG DeleteValue(LPCWSTR lpszValue);
        LONG EnumKey(
                    DWORD           dwIndex,              // subkey index
                    std::wstring&   wstrName,              // subkey name
                    PFILETIME       lpftLastWriteTime); // last write time
    };

    inline KlRegKey::KlRegKey()
    {m_hKey = NULL;}

    inline KlRegKey::~KlRegKey()
    {Close();}

    inline KlRegKey::operator HKEY() const
    {return m_hKey;}

    inline HKEY KlRegKey::Detach()
    {
	    HKEY hKey = m_hKey;
	    m_hKey = NULL;
	    return hKey;
    }

    inline void KlRegKey::Attach(HKEY hKey)
    {
	    ATLASSERT(m_hKey == NULL);
	    m_hKey = hKey;
    }

    inline LONG KlRegKey::DeleteSubKey(LPCWSTR lpszSubKey)
    {
	    ATLASSERT(m_hKey != NULL);
	    return RegDeleteKey(m_hKey, KLSTD_W2CT2(lpszSubKey));
    }

    inline LONG KlRegKey::DeleteValue(LPCWSTR lpszValue)
    {
	    ATLASSERT(m_hKey != NULL);
	    return RegDeleteValue(m_hKey, KLSTD_W2CT2(lpszValue));
    }

    inline LONG KlRegKey::Close()
    {
	    LONG lRes = ERROR_SUCCESS;
	    if (m_hKey != NULL)
	    {
		    lRes = RegCloseKey(m_hKey);
		    m_hKey = NULL;
	    }
	    return lRes;
    }

    inline LONG KlRegKey::Create(HKEY hKeyParent, LPCWSTR lpszKeyName,
	    LPWSTR lpszClass, DWORD dwOptions, REGSAM samDesired,
	    LPSECURITY_ATTRIBUTES lpSecAttr, LPDWORD lpdwDisposition)
    {
	    ATLASSERT(hKeyParent != NULL);
	    DWORD dw;
	    HKEY hKey = NULL;
        LONG lRes = ::RegCreateKeyEx(
                        hKeyParent, 
                        (KLSTD_W2CT2)lpszKeyName, 0,
		                (KLSTD_W2T2)lpszClass, 
                        dwOptions, 
                        samDesired, 
                        lpSecAttr, 
                        &hKey, 
                        &dw);
	    if (lpdwDisposition != NULL)
		    *lpdwDisposition = dw;
	    if (lRes == ERROR_SUCCESS)
	    {
		    lRes = Close();
		    m_hKey = hKey;
	    }
	    return lRes;
    }

    inline LONG KlRegKey::Open(HKEY hKeyParent, LPCWSTR lpszKeyName, REGSAM samDesired)
    {
	    ATLASSERT(hKeyParent != NULL);
	    HKEY hKey = NULL;
	    LONG lRes = RegOpenKeyEx(hKeyParent, (KLSTD_W2CT2)lpszKeyName, 0, samDesired, &hKey);
	    if (lRes == ERROR_SUCCESS)
	    {
		    lRes = Close();
		    ATLASSERT(lRes == ERROR_SUCCESS);
		    m_hKey = hKey;
	    }
	    return lRes;
    }

    inline LONG KlRegKey::QueryValue(DWORD& dwValue, LPCWSTR lpszValueName)
    {
	    DWORD dwType = NULL;
	    DWORD dwCount = sizeof(DWORD);
	    LONG lRes = RegQueryValueEx(m_hKey, (KLSTD_W2CT2)lpszValueName, NULL, &dwType,
		    (LPBYTE)&dwValue, &dwCount);
	    ATLASSERT((lRes!=ERROR_SUCCESS) || (dwType == REG_DWORD));
	    ATLASSERT((lRes!=ERROR_SUCCESS) || (dwCount == sizeof(DWORD)));
	    return lRes;
    }

    inline LONG KlRegKey::QueryValue(std::wstring& wstrVal, LPCWSTR lpszValueName)
    {
        wstrVal.clear();
        DWORD dwCount = 0;
        DWORD dwType = 0;
        KLSTD_W2CT2 tstrValueName = lpszValueName;

        LONG lResult = ::RegQueryValueEx(
                            m_hKey, 
                            tstrValueName, 
                            NULL,
                            &dwType, 
                            NULL, 
                            &dwCount);
        if(dwCount)
        {
            if( dwType == REG_SZ || dwType == REG_EXPAND_SZ || dwType == REG_MULTI_SZ )
            {
                std::vector<TCHAR> vecBuffer;
                vecBuffer.resize(dwCount/sizeof(TCHAR)+2);
                lResult = ::RegQueryValueEx(
                                            m_hKey, 
                                            tstrValueName, 
                                            NULL,
                                            &dwType, 
                                            (LPBYTE)(void*)&vecBuffer[0], 
                                            &dwCount);
                if(NO_ERROR == lResult)
                {
                    wstrVal = (const wchar_t*)KLSTD_T2CW2(&vecBuffer[0]);
                };
            }
            else
                lResult = ERROR_BAD_CONFIGURATION;
        };
#if 0
        vecBuffer.resize(MAX_PATH*4+1);
        DWORD dwCount = sizeof(TCHAR)*(vecBuffer.size()-1);
        DWORD dwType = REG_NONE;
        LONG lResult = ::RegQueryValueEx(
                            m_hKey, 
                            KLSTD_W2CT2(lpszValueName), 
                            NULL,
                            &dwType, 
                            (LPBYTE)(void*)&vecBuffer[0], 
                            &dwCount);
        if(NO_ERROR == lResult)
        {
            if( dwType == REG_SZ || dwType == REG_EXPAND_SZ || dwType == REG_MULTI_SZ )
                wstrVal = (const wchar_t*)KLSTD_T2CW2(&vecBuffer[0]);
            else
                lResult = ERROR_BAD_CONFIGURATION;
        };
#endif
        return lResult;
    }

    inline LONG WINAPI KlRegKey::SetValue(HKEY hKeyParent, LPCWSTR lpszKeyName, LPCWSTR lpszValue, LPCWSTR lpszValueName)
    {
	    ATLASSERT(lpszValue != NULL);
	    KlRegKey key;
	    LONG lRes = key.Create(hKeyParent, lpszKeyName);
	    if (lRes == ERROR_SUCCESS)
		    lRes = key.SetValue(lpszValue, lpszValueName);
	    return lRes;
    }

    inline LONG KlRegKey::SetKeyValue(LPCWSTR lpszKeyName, LPCWSTR lpszValue, LPCWSTR lpszValueName)
    {
	    ATLASSERT(lpszValue != NULL);
	    KlRegKey key;
	    LONG lRes = key.Create(m_hKey, lpszKeyName);
	    if (lRes == ERROR_SUCCESS)
		    lRes = key.SetValue(lpszValue, lpszValueName);
	    return lRes;
    }

    inline LONG KlRegKey::SetValue(DWORD dwValue, LPCWSTR lpszValueName)
    {
	    ATLASSERT(m_hKey != NULL);
	    return RegSetValueEx(m_hKey, (KLSTD_W2CT2)lpszValueName, NULL, REG_DWORD,
		    (BYTE * const)&dwValue, sizeof(DWORD));
    }

    inline LONG KlRegKey::SetValue(LPCWSTR lpszValue, LPCWSTR lpszValueName)
    {
	    ATLASSERT(lpszValue != NULL);
	    ATLASSERT(m_hKey != NULL);
        KLSTD_W2CT2 strVal = lpszValue;
	    return RegSetValueEx(m_hKey, (KLSTD_W2CT2)lpszValueName, NULL, REG_SZ,
		    (BYTE * const)(void*)(LPCTSTR)strVal, (lstrlen(strVal)+1)*sizeof(TCHAR));
    }

    inline LONG KlRegKey::RecurseDeleteKey(LPCWSTR lpszKey)
    {
	    KlRegKey key;
	    LONG lRes = key.Open(m_hKey, lpszKey, KEY_READ | KEY_WRITE);
	    if (lRes != ERROR_SUCCESS)
		    return lRes;
	    FILETIME time;

        std::vector<TCHAR> vecBuffer;
        vecBuffer.resize(MAX_PATH*4+1);
	    DWORD dwSize = vecBuffer.size()-1;

        while (RegEnumKeyEx(key.m_hKey, 0, &vecBuffer[0], &dwSize, NULL, NULL, NULL,
		    &time)==ERROR_SUCCESS)
	    {
		    lRes = key.RecurseDeleteKey(KLSTD_T2CW2(&vecBuffer[0]));
		    if (lRes != ERROR_SUCCESS)
			    return lRes;
		    dwSize = 256;
	    }
	    key.Close();
	    return DeleteSubKey(lpszKey);
    }

    inline LONG KlRegKey::EnumKey(
                DWORD           dwIndex,           // subkey index
                std::wstring&   wstrName,          // subkey name
                PFILETIME       lpftLastWriteTime) // last write time
    {
        std::vector<TCHAR> vecName;
        vecName.resize(MAX_PATH*4+1);
        DWORD dwSize = vecName.size()-1;
        LONG lRes = ::RegEnumKeyEx(
                    m_hKey,
                    dwIndex,
                    &vecName[0], 
                    &dwSize,
                    NULL,
                    NULL,
                    NULL,
                    lpftLastWriteTime);
        if(NO_ERROR == lRes)
            wstrName = KLSTD_T2CW2(&vecName[0]);
        return lRes;
    };
};