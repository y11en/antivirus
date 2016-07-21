#ifndef _cEnvironmentHelper_H
#define _cEnvironmentHelper_H

class cEventMgr;

#include "types.h"
#include "cpath.h"
#include "cregkey.h"
#include "envhelper.h"
#include "eventmgr.h"
#include "eventhandler.h"

enum eMemState {
	cMEM_COMMIT  = 0x01000,
	cMEM_RESERVE = 0x02000,
	cMEM_FREE    = 0x10000,
};

enum eMemType {
	cMEM_PRIVATE = 0x20000,
	cMEM_MAPPED  = 0x40000,
	cMEM_IMAGE   = 0x1000000,
};

enum eMemProtect {
	cPAGE_NOACCESS          = 0x01,
	cPAGE_READONLY          = 0x02,     
	cPAGE_READWRITE         = 0x04,     
	cPAGE_WRITECOPY         = 0x08,     
	cPAGE_EXECUTE           = 0x10,     
	cPAGE_EXECUTE_READ      = 0x20,     
	cPAGE_EXECUTE_READWRITE = 0x40,     
	cPAGE_EXECUTE_WRITECOPY = 0x80,     
};

typedef struct tag_PROCMEMINFO {
    tAddr BaseAddress;
    tAddr AllocationBase;
    eMemProtect AllocationProtect;
    size_t RegionSize;
	eMemState State;
    eMemProtect Protect;
    eMemType Type;
} PROCMEMINFO, *PPROCMEMINFO;


// MS compiler v.1200 assigns to function declared with typedef __cdecl calling convention. 
// This leading to colling convention conflict at link time (__cdecl vs __thiscall).
// As workaround we declare handlers with __fastcall calling convention
#if defined(_MSC_VER)
#if _MSC_VER <= 1200
#define __ENVFUNC_CALL __fastcall
#else // _MSC_VER > 1200
#define __ENVFUNC_CALL 
#endif // _MSC_VER
#else // _MSC_VER not defined
#define __ENVFUNC_CALL 
#endif


#define DECLARE_ENV_FUNC(rettype, name, params) \
	typedef rettype __ENVFUNC_CALL _t##name params; \
	virtual rettype __ENVFUNC_CALL name params

#define _IMPLEMENT(handler) _t##handler handler;


class cEnvironmentHelper
{
public:
	cEnvironmentHelper() {}
	~cEnvironmentHelper() {}

	// memory
	// !!! Alloc MUST zero allocated memory !!!
	DECLARE_ENV_FUNC( void*, Alloc, (size_t size)) = 0; // { return NULL; }
	DECLARE_ENV_FUNC( void,  Free, (void* mem)) = 0; // { };

	// environment
	DECLARE_ENV_FUNC( tstring, ExpandEnvironmentStr, (tcstring value)) { return tstrdup(value); }
	
	// Interlocked Variable Access
	DECLARE_ENV_FUNC( long,    InterlockedInc, (long* addend)) {return ++*addend;}
	DECLARE_ENV_FUNC( long,    InterlockedDec, (long* addend)) {return --*addend;}
	DECLARE_ENV_FUNC( long,    InterlockedExchAdd, (long* addend, long value)) {long old = *addend; *addend += value; return old;}

	// Path 
	DECLARE_ENV_FUNC( tstring, PathGetLong, (tcstring path)) { return tstrdup(path); }
	DECLARE_ENV_FUNC( bool,    PathIsNetwork, (tcstring path)) { return 0 == tstrncmp(path, _T("\\\\"), 2) || 0 == tstrncmp(path, _T("UNC\\"), 4); }
	DECLARE_ENV_FUNC( bool,    PathIsRemovable, (tcstring path)) { return false; }
//	DECLARE_ENV_FUNC( tcstring, PathSkipUnicodePrefix, (tcstring path) = 0;
	DECLARE_ENV_FUNC( tstring, PathFindExecutable, (tcstring file, tcstring default_dir)) { return NULL; }

	// File operations
	DECLARE_ENV_FUNC( uint32_t, FileAttr, (tcstring fullpath)) { return 0; }
	DECLARE_ENV_FUNC( tHANDLE,  FileOpen, (tcstring filename)) { return 0; }
	DECLARE_ENV_FUNC( bool,     FileClose, (tHANDLE hFile)) { return false; }
	DECLARE_ENV_FUNC( bool,     FileRead, (tHANDLE hFile, uint64_t pos, void* buff, uint32_t buff_size, uint32_t* bytes_read)) { if (bytes_read) *bytes_read = 0; return false; }
	DECLARE_ENV_FUNC( uint64_t, FileSizeByHandle, (tHANDLE hFile)) { return 0; }
	DECLARE_ENV_FUNC( uint64_t, FileSizeByName, (tcstring filename)) { return 0; }

	// Registry
	DECLARE_ENV_FUNC( bool,    Reg_OpenKey, (tRegistryKey hKey, tcstring lpSubKey, tRegistryKey* phkResult)) { return false; }
	DECLARE_ENV_FUNC( bool,    Reg_EnumValue, (tRegistryKey hKey, uint32_t dwIndex, tstring* lpValueName, uint32_t* lpType)) { return false; }
	DECLARE_ENV_FUNC( bool,    Reg_QueryValue, (tRegistryKey hKey, tcstring lpValueName, uint32_t* reg_type, void* buffer, size_t buff_size, size_t* value_size)) { if (value_size) *value_size=0; return false; }
	DECLARE_ENV_FUNC( tstring, Reg_QueryValueStr, (tRegistryKey hKey, tcstring lpValueName)) { return NULL; }
	DECLARE_ENV_FUNC( bool,    Reg_CloseKey, (tRegistryKey hKey)) { return false; }

	// Process
	DECLARE_ENV_FUNC( bool,		ProcessVirtualQuery, (tPid pid, tAddr addr, PROCMEMINFO* pMemInfo)) { return false; };
	DECLARE_ENV_FUNC( tAddr,	ProcessGetProcAddr, (tPid pid, tcstring sModuleName, const char* sProcName)) { return 0; };
	DECLARE_ENV_FUNC( tstring,	ProcessGetModuleFilename, (tPid pid, tHANDLE hModule)) { return NULL; };
	DECLARE_ENV_FUNC( bool,	    ProcessAssignUniqPid, (cProcessListItem* pProcess)) { return false; };

	// Thread
	DECLARE_ENV_FUNC( bool,	    ThreadGetProcess, (tTid tid, tPid * pPid)) { return false; };

	// Time
	DECLARE_ENV_FUNC( uint64_t,	GetTimeLocal, ()) { return 0; };

	// Driver
	DECLARE_ENV_FUNC( bool,     DriverPending, (cEvent& event, uint32_t nMilliseconds)) { return false; };

	// helper funcs
	bool Reg_QueryValueEx(const cRegKey& key, tcstring lpValueName, uint32_t* reg_type, void* buffer, size_t buff_size, size_t* value_size)
	{
		bool result;
		tRegistryKey open_key;
		if (!Reg_OpenKey(key.m_root, key.m_open_name, &open_key))
			return false;
		result = Reg_QueryValue(open_key, lpValueName, reg_type, buffer, buff_size, value_size);
		Reg_CloseKey(open_key);
		return result;
	}
	tstring Reg_QueryValueStrEx(const cRegKey& key, tcstring lpValueName)
	{ 
		tstring result;
		tRegistryKey open_key;
		if (!Reg_OpenKey(key.m_root, key.m_open_name, &open_key))
			return NULL;
		result = Reg_QueryValueStr(open_key, lpValueName);
		Reg_CloseKey(open_key);
		return result;
	}
	////////////////////////////////////////////////
	//	OutStrSize - char nuber (not bytes count)
	////////////////////////////////////////////////
	bool ConvertANSIToTStr(const char * InStr, tstring OutStr, size_t * OutStrSize)
	{
		if (InStr == 0 || OutStr == 0 || OutStrSize == 0)
			return false;
		size_t RealStrLen = strlen(InStr) + 1;
		if (RealStrLen > *OutStrSize)
		{
			*OutStrSize = RealStrLen;
			return false;
		}
		*OutStrSize = RealStrLen;
		size_t i = 0;
		while (InStr[i] != 0)
		{
			#ifdef UNICODE
				mbtowc(&OutStr[i], &InStr[i], MB_CUR_MAX);
			#else
				OutStr[i] = InStr[i];
			#endif
			i++;
		}
		OutStr[i] = 0;
		return true;
	}

};

#undef __ENVFUNC_CALL
#undef DECLARE_ENV_FUNC

class cAutoEnvFile
{
public:
	cAutoEnvFile(cEnvironmentHelper* pEnvironmentHelper, tHANDLE hFile = INVALID_HANDLE) : m_env(pEnvironmentHelper), m_hFile(hFile) {};
	~cAutoEnvFile() 
	{ 
		close();
	};

	void operator= (tHANDLE hFile) 
	{ 
		close();
		m_hFile = hFile; 
	}

	operator tHANDLE () 
	{ 
		return m_hFile; 
	}

	cAutoEnvFile& open(tcstring filename)
	{
		close();
		m_hFile = m_env->FileOpen(filename); 
		return *this;
	}

	void close()
	{
		if (INVALID_HANDLE != m_hFile) 
		{
			m_env->FileClose(m_hFile); 
			m_hFile = INVALID_HANDLE;
		}
	}
private:
	cEnvironmentHelper* m_env;
	tHANDLE m_hFile;
};

template <typename T> 
class cAutoEnvMem
{
public:
	cAutoEnvMem(cEnvironmentHelper* pEnvironmentHelper, T* pMem = NULL) : m_env(pEnvironmentHelper), m_pMem(pMem) 
	{
	};
	cAutoEnvMem(cEnvironmentHelper* pEnvironmentHelper, size_t count) : m_env(pEnvironmentHelper), m_pMem(NULL)
	{
		Alloc(count);
	};
	~cAutoEnvMem() 
	{ 
		Free();
	};

	void operator= (T* pMem) 
	{ 
		Free();
		m_pMem = pMem; 
	}

	operator T* () 
	{ 
		return m_pMem; 
	}

	T* Alloc(size_t count)
	{
		Free();
		m_pMem = (T*)m_env->Alloc(count * sizeof(T));
		return m_pMem;
	}

	void Free()
	{
		if (NULL != m_pMem) 
			m_env->Free(m_pMem); 
		m_pMem = NULL;
	}

	T* relinquish()
	{
		T* pMem = m_pMem;
		m_pMem = NULL;
		return pMem;
	}

private:
	cEnvironmentHelper* m_env;
	T* m_pMem;
};

#endif // _cEnvironmentHelper_H
