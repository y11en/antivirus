//#include <windows.h>


#include "startupenum2.h"
#include "enumctx.h"
#include "Initialization.h"
#include "util.h"

#include <Prague/prague.h>
#include <Prague/iface/i_hash.h>
#include <Prague/iface/i_inifile.h>
#include <Prague/iface/i_string.h>

#include <AV/plugin/p_startupenum2.h>
#include <AV/structs/s_startupenum2.h>

#include <hook\avpgcom.h>

void	pfCoTaskMemFree(void *pv);
BOOL	pfOpenThreadToken(DWORD DesiredAccess,BOOL OpenAsSelf,PHANDLE TokenHandle);
BOOL	pfRevertToSelf(VOID);
BOOL	pfSetThreadToken(PHANDLE Thread,HANDLE Token);

#ifndef _DEFINES_H_
#define _DEFINES_H_

#define warnOBJECT_FOUND				PR_MAKE_IMP_WARN( PID_STARTUPENUM2, 0x0000 )  // 70
#define warnOBJECT_INFECTED				PR_MAKE_IMP_WARN( PID_STARTUPENUM2, 0x0100 )  // 70
#define warnOBJECT_FALSE				PR_MAKE_IMP_WARN( PID_STARTUPENUM2, 0x0101 )  // 70
#define warnOBJECT_DELETED              PR_MAKE_IMP_WARN( PID_STARTUPENUM2, 0x0102 )  // 70

//General
typedef enum _SECTIONS_TYPE{
	UNKNOWN=0,
	REGISTRY_TYPE,
	INI_TYPE,
	BAT_TYPE,
}SECTIONS_TYPE;

typedef struct _HOSTS_ANALYSE_CONTEXT{
	tDWORD					nCount;
	tDWORD                  nAllocated;
	tCHAR**					ppLinks;
} HOSTS_ANALYSE_CONTEXT, *pHOSTS_ANALYSE_CONTEXT;


typedef struct _LSP_CONTEXT{
	EnumContext*	pContext;
	tDWORD			dType;
	tDWORD			dValueCount;
	tBOOL			NeedToRestoreCount;

	const wchar_t*  sCopy2Key;
	tDWORD          nOrigKeyPathLen;
}LSP_CONTEXT, *pLSP_CONTEXT;

typedef struct tag_RegFunc {
	const char* name;
	FARPROC* ppfunc;
} tRegFunc;

typedef struct _RESTORE_ORIGIN_STRING{
	tDWORD	oPoint;
	tDWORD	oSize;
	tDWORD	nPoint;
	tDWORD	nSize;
}RESTORE_ORIGIN_STRING, *pRESTORE_ORIGIN_STRING;

#define LCASE(c) (c>='A'? (c<='Z'? (c|0x20) : c) : c)
#ifndef countof
	#define countof(array) (sizeof(array)/sizeof((array)[0]))
#endif

//RegEnumCallback return codes
#define ERROR_REGOP_FALSE					0
#define ERROR_REGOP_TRUE					1
#define ERROR_REGOP_MISS_VALUE				2
#define ERROR_REGOP_DEL_KEY					3
#define ERROR_REGOP_RECOUNT_KEY				4
#define ERROR_REGOP_DEL_THISKEY				5
#define ERROR_REGOP_CANCELED				6
#define ERROR_REGOP_RECOUNT_KEY_FROM_VALUE	7
#define ERROR_REGOP_NOT_CLOSE_THIS_KEY		8

//Special REG Flags
#define FLAG_REG_WITH_SUBKEYS				0x1
#define FLAG_REG_SUBKEYS_PROCIED			0x2
#define FLAG_REG_CLSID_INIT					0x4
//#define FLAG_REG_DELETE_ROOT				0x4

#define FLAG_CLEAR_REG_SUBKEYS_PROCIED		0xfffffffd
//#define FLAG_CLEAR_REG_DELETE_ROOT			0xfffffffb

#define REG_KEY								0xff

//Flags
#define FLAG_DEL_VALUE						0x1
#define FLAG_CUT_DATA						0x2
#define FLAG_RESTORE_DEFAULT				0x4
#define FLAG_ENUM_VALUES					0x8
#define FLAG_DEL_KEY						0x10
#define FLAG_ENUM_KEYS						0x20
#define FLAG_ENUM_WIN9X_ONLY				0x40
#define FLAG_ENUM_WINNT_ONLY				0x80
#define FLAG_REG_FILTERS_EXIST				0x100

#define FLAG_CLEAR_ACTIONS_SENDING			0xffffb000
#define FLAG_CLEAR_ENUM_KEYS				0xffffffdf

#define CURE_ACTION_ACTIVE					0x1000
#define FLAG_REG_DETECTED					0x4000

#define FLAG_CLEAR_REG_DETECTED				0xffffbfff

//Registry
#define MAX_KEY_LENGTH						255
#define MAX_VALUE_NAME						16383

#define REG_OPEN_KEY						0x10000000
#define REG_CLOSE_KEY						0x20000000

#define _sec_buffer_size					65535

//crc64 defines
typedef unsigned int hint_t;	/* we want a 32 bit unsigned integer here */

typedef struct hash_t {
    hint_t	h1;
    hint_t	h2;
} hash_t;


class cAutoInterlockedCounter
{
public:
	cAutoInterlockedCounter(LONG* pCounter) : m_pCounter(pCounter) { 
		LONG n = InterlockedIncrement(m_pCounter); 
		PR_TRACE((g_root, prtIMPORTANT, "startupenum2\tInterlockedIncrement *0x%08X=>%d ", m_pCounter, n));
	};
	~cAutoInterlockedCounter() 
	{ 
		destroy();
	};
	void destroy()
	{
		if (!m_pCounter)
			return;
		LONG n = InterlockedDecrement(m_pCounter); 
		PR_TRACE((g_root, prtIMPORTANT, "startupenum2\tInterlockedDecrement *0x%08X=>%d ", m_pCounter, n));
		m_pCounter = NULL;
	}
private:
	LONG* m_pCounter;
};

class cAutoNativeCS
{
public:
	cAutoNativeCS(CRITICAL_SECTION* pCS) : m_pCS(pCS) 
	{ 
		if (m_pCS)
			EnterCriticalSection(m_pCS);
	}
	~cAutoNativeCS()
	{
		if (m_pCS)
			LeaveCriticalSection(m_pCS);
	}
private:
	CRITICAL_SECTION* m_pCS;
};

class cAutoRelease
{
public:
	cAutoRelease(IUnknown* pUnknown) : m_pUnknown(pUnknown) {};
	~cAutoRelease()
	{
		if (m_pUnknown)
			m_pUnknown->Release();
	}
private:
	IUnknown* m_pUnknown;
};



class cAutoCoTaskMemFree
{
public:
	cAutoCoTaskMemFree(void** ppMem) : m_ppMem(ppMem) {};
	cAutoCoTaskMemFree(wchar_t** ppMem) : m_ppMem((void**)ppMem) {};
	~cAutoCoTaskMemFree()
	{
		if (m_ppMem && *m_ppMem)
		{
			pfCoTaskMemFree(*m_ppMem);
			*m_ppMem = NULL;
		}
	}
private:
	void** m_ppMem;
};

class cAutoPrMemFree
{
public:
	cAutoPrMemFree(hOBJECT hObj, void** ppMem) : m_ppMem(ppMem), m_hObj(hObj) {};
	~cAutoPrMemFree()
	{
		if (m_ppMem && *m_ppMem)
		{
			(m_hObj?m_hObj:*g_root)->heapFree(*m_ppMem);
			*m_ppMem = NULL;
		}
	}
private:
	void** m_ppMem;
	hOBJECT m_hObj;
};

class cAutoCoTaskMemFreeArray
{
public:
	cAutoCoTaskMemFreeArray(LPWSTR** ppArray, size_t count) : m_ppArray(ppArray), m_count(count) {};
	~cAutoCoTaskMemFreeArray()
	{
		if (m_ppArray && *m_ppArray)
		{
			LPWSTR* pArray = *m_ppArray;
			for (size_t i=0; i<m_count; i++)
			{
				if (pArray[i])
					pfCoTaskMemFree(pArray[i]);
			}
			pfCoTaskMemFree(pArray);
			*m_ppArray = NULL;
		}
	}
private:
	LPWSTR** m_ppArray;
	size_t m_count;
};

class cAutoToken
{
public:
	cAutoToken(HANDLE hToken)
	{
		m_hOldToken = NULL;
		pfOpenThreadToken(TOKEN_QUERY|TOKEN_IMPERSONATE|TOKEN_DUPLICATE, TRUE, &m_hOldToken);
		if( hToken )
		{
			pfRevertToSelf();
			GetLastError();
		}
	}
	~cAutoToken()
	{
		if( m_hOldToken )
		{
			pfSetThreadToken(NULL, m_hOldToken);
			CloseHandle(m_hOldToken);
		}
	}
private:
	HANDLE m_hOldToken;
};

class cAutoCloseHandle
{
public:
	cAutoCloseHandle(HANDLE* pHand) : m_pHand(pHand) {};
	~cAutoCloseHandle()
	{
		if (m_pHand && *m_pHand && (*m_pHand!=INVALID_HANDLE_VALUE))
		{
			CloseHandle(*m_pHand);
			*m_pHand = NULL;
		}
	}
private:
	HANDLE* m_pHand;
};


#define cAutoPrBuff_ALLOC_ALIGN 1
class cAutoPrBuff
{
public:
	cAutoPrBuff()
	{
		init();
	}
	cAutoPrBuff(hOBJECT hObj)
	{
		init();
		set_obj(hObj);
	};
	cAutoPrBuff(hOBJECT hObj, tUINT nSize)
	{
		init();
		set_obj(hObj);
		reserve(nSize);
	};
	~cAutoPrBuff()
	{
		if (m_ptr && m_nAllocated && !m_bStatic)
		{
#if 0
			if (*(DWORD*)((BYTE*)m_ptr+m_nAllocated) != 0xA1F3D5C8)
				INT3;
			VirtualFree(m_ptr, 0, MEM_RELEASE);
#else
			m_hObj->heapFree(m_ptr);
#endif
		}
		m_nAllocated = 0;
		m_ptr = NULL;
	};
	bool operator !()   { return (m_ptr==NULL);  };
	bool reserve(tUINT nSize) 	
	{
		tUINT align;
		if (m_nAllocated >= nSize)
			return true;

		align = nSize % cAutoPrBuff_ALLOC_ALIGN;
		nSize += (align ? cAutoPrBuff_ALLOC_ALIGN - align : 0);
		if (m_bStatic)
		{
			tPTR static_ptr = m_ptr;
			m_bStatic = false;
#if 0
			if (nSize == 0)
			{
				m_ptr = NULL;
				m_nAllocated = nSize;
				return true;
			}

			m_ptr = VirtualAlloc(NULL, nSize+4, MEM_COMMIT, PAGE_READWRITE);
			if (!m_ptr)
			{
				INT3;
			}
			else
			{
				DWORD increment = (nSize+4) % 0x1000;
				if (increment) increment = 0x1000 - increment;
				*(DWORD*)&m_ptr = (DWORD)m_ptr + increment;
				*(DWORD*)((BYTE*)m_ptr+nSize) = 0xA1F3D5C8;
#else
			if (PR_SUCC(m_hObj->heapAlloc(&m_ptr, nSize)))
			{
#endif
				memcpy(m_ptr, static_ptr, min(m_nAllocated,nSize));
				m_nAllocated = nSize;
				return true;
			}
		}
		else
		{
#if 0
			tPTR new_ptr = NULL;
			tERROR error = errOK;
			if (nSize)
			{
				new_ptr = VirtualAlloc(NULL, nSize+4, MEM_COMMIT, PAGE_READWRITE);
				if (new_ptr)
				{
					DWORD increment = (nSize+4) % 0x1000;
					if (increment) increment = 0x1000 - increment;
					*(DWORD*)&new_ptr = (DWORD)new_ptr + increment;
					error = errOK;
				}
				else
				{
					error = errNOT_ENOUGH_MEMORY;
					INT3;
				}
			}
			if (m_nAllocated && m_ptr)
			{
				if (PR_SUCC(error))
					memcpy(new_ptr, m_ptr, min(m_nAllocated,nSize));
				if (*(DWORD*)((BYTE*)m_ptr+m_nAllocated) != 0xA1F3D5C8)
					INT3;
				VirtualFree(m_ptr, 0, MEM_RELEASE);
			}
			if (PR_SUCC(error))
			{
				*(DWORD*)((BYTE*)new_ptr+nSize) = 0xA1F3D5C8;
				m_nAllocated = nSize;
				m_ptr = new_ptr;
				return true;
			}
			m_ptr = NULL;
#else
			if (PR_SUCC(m_hObj->heapRealloc(&m_ptr, (m_nAllocated ? m_ptr : NULL), nSize)))
			{
				m_nAllocated = nSize;
				return true;
			}
#endif
		}
		m_nAllocated = 0;
		m_ptr = NULL;
		m_ptr = "<not enought memory>";
		m_bStatic = true;
		return false;
	}
	void set_obj(hOBJECT hObj)
	{
		m_hObj = (hObj ? hObj : *g_root);
	}
	tUINT allocated()
	{
		return m_nAllocated;
	}
	void init()
	{
		memset(this, 0, sizeof(*this));
		set_obj(*g_root);
	}
	
protected:
	hOBJECT m_hObj;
	tPTR    m_ptr;
	tUINT   m_nAllocated;
	tUINT   m_nUsed;
	bool    m_bStatic;
};

class cPrStrA : public cAutoPrBuff
{
public:
	cPrStrA() : cAutoPrBuff() {};
	cPrStrA(hOBJECT hObj) : cAutoPrBuff(hObj) {};
	cPrStrA(hOBJECT hObj, tUINT nSize) : cAutoPrBuff(hObj, nSize) {};
	cPrStrA(char* buff, tUINT nChars)
	{
		m_ptr = buff;
		m_nAllocated = nChars;
		m_bStatic = true;
	}
	char* operator= (const char* str)
	{
		if (!str)
			str = "";
		tUINT len = strlen(str)+1;
		if (reserve(len))
			memcpy(m_ptr, str, len);
		return (char*)m_ptr;
	}
	char* operator= (const wchar_t* str)
	{
		if(!str)
			str = L"";
		tUINT len = WideCharToMultiByte(CP_ACP, 0, str, -1, NULL, 0, NULL, NULL);
		if (reserve(len*sizeof(wchar_t)))
			FastUni2Ansi(str, (char*)m_ptr, len);
		return (char*)m_ptr;
	}
	char* operator= (cPrStrA& str)
	{
		return *this = (char*)str;
	}
	cPrStrA(const char* str)
	{
		*this = str;
	};
	cPrStrA(const wchar_t* str)
	{
		*this = str;
	};
	tUINT getlen()
	{
		return (m_ptr ? strlen((char*)m_ptr) : 0);
	}
	bool reserve_len(tUINT nChars)
	{
		return reserve((nChars+1) * sizeof(char));
	}
	tUINT allocated_len()
	{
		return m_nAllocated;
	}
	operator char*()   { return (tCHAR*)m_ptr;  };
};

class cPrStrW : public cAutoPrBuff
{
public:
	cPrStrW() : cAutoPrBuff() {};
	cPrStrW(hOBJECT hObj) : cAutoPrBuff(hObj) {};
	cPrStrW(hOBJECT hObj, tUINT nSize) : cAutoPrBuff(hObj, nSize) {};
	cPrStrW(wchar_t* buff, tUINT nChars)
	{
		m_ptr = buff;
		m_nAllocated = nChars*sizeof(wchar_t);
		m_bStatic = true;
	}
	char* operator= (const char* str)
	{
		if (!str)
			str = "";
		tUINT len = strlen(str)+1;
		if (reserve(len*sizeof(wchar_t)))
			FastAnsi2Uni(str, (wchar_t*)m_ptr, len);
		return (char*)m_ptr;
	}
	wchar_t* operator= (const wchar_t* str)
	{
		if (!str)
			str = L"";
		tUINT len = (wcslen(str)+1)*sizeof(wchar_t);
		if (m_nAllocated<len)
		{
			if (reserve(len))
				memcpy(m_ptr, str, len);
		}
		else
			memcpy(m_ptr, str, len);
		return (wchar_t*)m_ptr;
	}
	wchar_t* operator= (cPrStrW& str)
	{
		return *this = (wchar_t*)str;
	}
	bool reserve(tUINT nSize) 	
	{
		if (!cAutoPrBuff::reserve(nSize))
		{
			m_ptr = L"<not enought memory>";
			m_bStatic = true;
			return false;
		}
		return true;
	}
	cPrStrW(const char* str)
	{
		*this = str;
	};
	cPrStrW(const wchar_t* str)
	{
		*this = str;
	};
	tUINT getlen()
	{
		return (m_ptr ? wcslen((wchar_t*)m_ptr) : 0);
	}
	bool reserve_len(tUINT nChars)
	{
		return reserve((nChars+1) * sizeof(wchar_t));
	}
	wchar_t* append_path(const wchar_t* str)
	{
		if (!str || !*str)
			return (wchar_t*)m_ptr;
		int len = (m_ptr ? wcslen((wchar_t*)m_ptr) : 0);
		int len2 = wcslen(str);
		if (!reserve_len(len + 1 + len2 + 1))
			return (wchar_t*)m_ptr;
		wchar_t* ptr = (wchar_t*)m_ptr;
		if (len && ptr[len-1] != '\\')
		{
			ptr[len] = '\\';
			len++;
		}
		while (*str == '\\')
			str++;
		wcscpy(ptr+len, str);
		return (wchar_t*)m_ptr;
	}
	wchar_t* append(const wchar_t* str)
	{
		if (!str || !*str)
			return (wchar_t*)m_ptr;
		int len = (m_ptr ? wcslen((wchar_t*)m_ptr) : 0);
		int len2 = wcslen(str);
		if (!reserve_len(len + len2 + 1))
			return (wchar_t*)m_ptr;
		wchar_t* ptr = (wchar_t*)m_ptr;
		wcscpy(ptr+len, str);
		return (wchar_t*)m_ptr;
	}
	wchar_t* append(const char* str)
	{
		if (!str || !*str)
			return (wchar_t*)m_ptr;
		int len = (m_ptr ? wcslen((wchar_t*)m_ptr) : 0);
		int len2 = strlen(str);
		if (!reserve_len(len + len2 + 1))
			return (wchar_t*)m_ptr;
		wchar_t* ptr = (wchar_t*)m_ptr;
		FastAnsi2Uni(str,ptr+len,len2+1);
		return (wchar_t*)m_ptr;
	}
	tUINT allocated_len()
	{
		return m_nAllocated/sizeof(wchar_t);
	}
	operator wchar_t*()  { 
#if 0
		if (m_ptr && m_nAllocated && !m_bStatic)
			if (*(DWORD*)((BYTE*)m_ptr+m_nAllocated) != 0xA1F3D5C8)
				INT3;
#endif
		return (wchar_t*)m_ptr; 
	};
};

typedef struct _UNLOAD_PROFILE_NAMES{
	DWORD		Count;
	DWORD		cProfileName[1];
}UNLOAD_PROFILE_NAMES, *pUNLOAD_PROFILE_NAMES;

typedef struct _CLSID_HKEYS{
	HKEY	hKey[1];
}CLSID_HKEYS, *pCLSID_HKEYS;

typedef struct _CLSID_HKEYS_STRUCT{
	DWORD			Count;
	pCLSID_HKEYS	pKeys;
}CLSID_HKEYS_STRUCT, *pCLSID_HKEYS_STRUCT;

extern bool		g_bIsWin9x;

//--------------------------------RegEnum---------------------------------------------------------------

typedef long (__stdcall*tEnumCallbackFuncW)(LPVOID pEnumCallbackContext, HKEY hKey, const wchar_t* sPath, const wchar_t* sName, BYTE* pValue, DWORD dwSize, DWORD dwType);
typedef long (__stdcall*tEnumCallbackFuncA)(LPVOID pEnumCallbackContext, HKEY hKey, const char* sPath, const char* sName, BYTE* pValue, DWORD dwSize, DWORD dwType);
typedef long (__stdcall*tEnumCallbackErrMessageFuncW)(LPVOID pEnumCallbackContext, const wchar_t* sPath, const wchar_t* sName, long dError, bool Error);
typedef long (__stdcall*tEnumCallbackErrMessageFuncA)(LPVOID pEnumCallbackContext, const char* sPath, const char* sName, long dError, bool Error);

typedef struct tag_RegEnumCtx {
	const wchar_t*					m_sValue;
	DWORD							m_dwFlags;
	LPVOID							m_pEnumCallbackContext;
	tEnumCallbackFuncW				m_pEnumCallbackFuncW;
	tEnumCallbackErrMessageFuncW	m_pEnumCallbackErrMessageFuncW;
	wchar_t*						m_sPath;
	DWORD							m_dwPathSize;
	DWORD							m_dwPathAllocated;
} tRegEnumCtx;

class cRegEnumCtx
{
public:
	cRegEnumCtx(StartUpEnum2* pStartUpEnum,DWORD AppID);
	~cRegEnumCtx();
protected:
	tBOOL		AllocIOCTLBuffer(tBOOL Result,tDWORD Size,ULONG IOCTL);
	bool		TryIfFunctionExist();
	tBOOL		CheckIfRegRootDrv(HKEY Result,cPrStrW& phSKey);
	wchar_t*	wcscpyDrv(wchar_t* wchTo,HKEY wchFrom);
	LONG		_pfRegOpenKeyDrv(HKEY hKey,const wchar_t* lpSubKey,cPrStrW** phkResult);
	LONG		_pfRegOpenKey(HKEY hKey,const wchar_t* lpSubKey,PHKEY phkResult);
	LONG		_pfRegCloseKeyDrv(cPrStrW* hKey);
	LONG		_pfRegEnumKeyDrv(HKEY hKey, DWORD dwIndex, cPrStrW& sKeyName);
	LONG		_pfRegEnumKey(HKEY hKey, DWORD dwIndex, cPrStrW& sKeyName);
	LONG		_pfRegCreateKeyDrv(HKEY hKey,const wchar_t* lpSubKey,cPrStrW** phkResult);
	LONG		_pfRegCreateKey(HKEY hKey,const wchar_t* lpSubKey,PHKEY phkResult);
	LONG		_pfRegEnumValueDrv(HKEY hKey,DWORD dwIndex, cPrStrW& sKeyName);
	LONG		_pfRegEnumValue(HKEY hKey,DWORD dwIndex, cPrStrW& sKeyName);
	LONG		_pfRegQueryValueExDrv(HKEY hKey,const wchar_t* lpValueName,LPDWORD lpType,LPBYTE lpData,LPDWORD lpcbData);
	LONG		_pfRegQueryValueEx(HKEY hKey,const wchar_t* lpValueName,LPDWORD lpType,LPBYTE lpData,LPDWORD lpcbData);
	LONG		_pfRegDeleteValueDrv(HKEY hKey,const wchar_t* lpValueName);
	LONG		_pfRegDeleteValue(HKEY hKey,const wchar_t* lpValueName);
	LONG		_pfRegSetValueExDrv(HKEY hKey,const wchar_t* lpValueName,DWORD dwType,CONST BYTE* lpData,DWORD cbData);
	LONG		_pfRegSetValueEx(HKEY hKey,const wchar_t* lpValueName,DWORD dwType,CONST BYTE* lpData,DWORD cbData);
	LONG		_pfRegDeleteKeyDrv(HKEY hKey,const wchar_t* lpSubKey);
	LONG		_pfRegDeleteKey(HKEY hKey,const wchar_t* lpSubKey);
	LONG		_pfRegQueryInfoKeyDrv(HKEY hKey,LPDWORD lpcSubKeys,LPDWORD lpcMaxSubKeyLen,LPDWORD lpcValues,LPDWORD lpcMaxValueNameLen,LPDWORD lpcMaxValueLen);
	LONG		_pfRegQueryInfoKey(HKEY hKey,LPDWORD lpcSubKeys,LPDWORD lpcMaxSubKeyLen,LPDWORD lpcValues,LPDWORD lpcMaxValueNameLen,LPDWORD lpcMaxValueLen);
	bool		_pfCheckHkeyDrv(HKEY hKeyF,HKEY hKeyS);
	long		_iRegEnum(HKEY hKey, const wchar_t* sKey, const wchar_t* sSubKeys, DWORD dwPathSize, tRegEnumCtx* pCtx, wchar_t*& sName, BYTE*& pValue);
	long		_RegEnum(HKEY hKey, const wchar_t* sKey, const wchar_t* sSubKeys, DWORD dwPathSize, tRegEnumCtx* pCtx);
private:
	const wchar_t*			sSubKeyName;
	StartUpEnum2*			m_pStartUpEnum;
	HANDLE					m_hpDevice;
	DWORD					m_AppID;
	tDWORD					cbRequestBuffer;
	tBYTE*					RequestBuf;
	tDWORD					cbResultBuffer;
	tBYTE*					ResultBuffer;
	PREG_OP_RESULT			pRegOp_Result;
	PEXTERNAL_DRV_REQUEST	pRequest;
	PREG_OP_REQUEST			pRegRequest;
	tBOOL					CLSIDInited;
	CLSID_HKEYS_STRUCT		CLSIDKeys;
	bool					m_bWOW3264Exist;
public:
	long		iCheckFilter(HKEY hKey, const wchar_t* sPath, const wchar_t* sName, BYTE* pValue, DWORD dwSize, DWORD dwType);
	LONG		pfRegOpenKey(HKEY hKey,const wchar_t* lpSubKey,PHKEY phkResult);
	LONG		pfRegCloseKey(HKEY hKey);
	LONG		pfRegEnumKey(HKEY hKey, DWORD dwIndex, cPrStrW& sKeyName);
	LONG		pfRegCreateKey(HKEY hKey,const wchar_t* lpSubKey,PHKEY phkResult);
	LONG		pfRegEnumValue(HKEY hKey,DWORD dwIndex, cPrStrW& sKeyName);
	LONG		pfRegQueryValueEx(HKEY hKey,const wchar_t* lpValueName,LPDWORD lpType,LPBYTE lpData,LPDWORD lpcbData);
	LONG		pfRegDeleteValue(HKEY hKey,const wchar_t* lpValueName);
	LONG		pfRegSetValueEx(HKEY hKey,const wchar_t* lpValueName,DWORD dwType,CONST BYTE* lpData,DWORD cbData);
	LONG		pfRegDeleteKey(HKEY hKey,const wchar_t* lpSubKey);
	LONG		pfRegLoadKey(const wchar_t* lpSubKey,const wchar_t* lpFile);
	LONG		pfRegUnLoadKey(const wchar_t* lpSubKey);
	LONG		pfRegQueryInfoKey(HKEY hKey,LPDWORD lpcSubKeys,LPDWORD lpcMaxSubKeyLen,LPDWORD lpcValues,LPDWORD lpcMaxValueNameLen,LPDWORD lpcMaxValueLen);
	HKEY		GetRegistryRoot(const wchar_t* sRoot, wchar_t** ppSubPath, const wchar_t** ppPath);
	LONG		RegEnumWW(const wchar_t* sKey, const wchar_t* sValue, DWORD dwFlags, LPVOID m_pEnumCallbackContext, tEnumCallbackFuncW m_pEnumCallbackFuncW, tEnumCallbackErrMessageFuncW pEnumCallbackErrMessageFunc);
	LONG		RegEnumAW(const char* sKey, const char* sValue, DWORD dwFlags, LPVOID m_pEnumCallbackContext, tEnumCallbackFuncW m_pEnumCallbackFuncW, tEnumCallbackErrMessageFuncW pEnumCallbackErrMessageFunc);
	tBOOL		ReadKeyValueDword(HKEY BaseKeyEnter,const wchar_t KeyPath[],const wchar_t NameValue[],tDWORD* pDword);
	tBOOL		ReadKeyValueStr(HKEY BaseKeyEnter, const wchar_t KeyPath[], const wchar_t NameValue[],cPrStrW& sValue, bool NeedStr = true);
	tBOOL		pfRegDeleteKeyByName(const wchar_t* sLastKeyName);
	tERROR		pfRegRenameKey(pLSP_CONTEXT pLSPContext, const wchar_t* sFromKey, const wchar_t* sToKey);
	bool		pfCheckHkey(HKEY hKeyF,HKEY hKeyS);
	void		CLSIDInit(void);
	void		CLSIDDeinit(void);
	long		CLSIDScan(const wchar_t* chCLSID,LPVOID pEnumCallbackContext,tEnumCallbackFuncW m_pEnumCallbackFuncW,tEnumCallbackErrMessageFuncW pEnumCallbackErrMessageFunc);
};

//---------------------------KLSubCure.dll-------------------------------------------------------------

typedef tPTR hPLUGININSTANCE;

typedef void (_stdcall *pfTrace)(tag_TRACE_LEVEL dTraceLevel,char* chMask);
typedef DWORD (/*_stdcall */ *pfSpecialRepairPtr)(hSTRING p_DelFName, hOBJECT p_pRetObject, char* p_DetectedVirusName);

//typedef struct _GET_FUNCTIONS{
//	char*	chFuncName;
//	void**	pfFunc;
//}GET_FUNCTIONS,*pGET_FUNCTIONS;

#endif //_DEFINES_H_