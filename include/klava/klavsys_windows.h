// klavsys_windows.h
//
// native platform support classes for Windows platform
//

#ifndef klavsys_windows_h_INCLUDED
#define klavsys_windows_h_INCLUDED

#include <klava/klavsys.h>
#include <klava/klav_props.h>

#ifdef KL_PLATFORM_WINDOWS

#ifndef WIN32_LEAN_AND_MEAN
# define WIN32_LEAN_AND_MEAN
#endif // WIN32_LEAN_AND_MEAN

#ifndef KL_PLATFORM_WINCE
#ifndef _WIN32_WINNT
#define _WIN32_WINNT 0x0400
#endif
#endif

#include <windows.h>

#ifdef __cplusplus

////////////////////////////////////////////////////////////////
// Windows API hooks

typedef HANDLE (WINAPI * KLAVSYS_PFN_CreateFileW) (LPCWSTR, DWORD, DWORD, LPSECURITY_ATTRIBUTES, DWORD, DWORD, HANDLE);
typedef HANDLE (WINAPI * KLAVSYS_PFN_CreateFileA) (LPCSTR, DWORD, DWORD, LPSECURITY_ATTRIBUTES, DWORD, DWORD, HANDLE);

typedef DWORD (WINAPI * KLAVSYS_PFN_GetTempPathW) (DWORD, LPWSTR);
typedef DWORD (WINAPI * KLAVSYS_PFN_GetTempPathA) (DWORD, LPSTR);

typedef UINT (WINAPI * KLAVSYS_PFN_GetTempFileNameW) (LPCWSTR, LPCWSTR, UINT, LPWSTR);
typedef UINT (WINAPI * KLAVSYS_PFN_GetTempFileNameA) (LPCSTR, LPCSTR, UINT, LPSTR);

typedef UINT (WINAPI * KLAVSYS_PFN_DeleteFileW) (LPCWSTR);
typedef UINT (WINAPI * KLAVSYS_PFN_DeleteFileA) (LPCSTR);

typedef UINT (WINAPI * KLAVSYS_PFN_MoveFileW) (LPCWSTR, LPCWSTR);
typedef UINT (WINAPI * KLAVSYS_PFN_MoveFileA) (LPCSTR, LPCSTR);

typedef HMODULE (WINAPI * KLAVSYS_PFN_LoadLibraryW) (LPCWSTR);
typedef HMODULE (WINAPI * KLAVSYS_PFN_LoadLibraryA) (LPCSTR);

typedef HANDLE (WINAPI * KLAVSYS_PFN_CreateFileForMappingW)(LPCWSTR, DWORD, DWORD , LPSECURITY_ATTRIBUTES, DWORD, DWORD, HANDLE);

typedef DWORD (WINAPI * KLAVSYS_PFN_GetFileAttributesW)(LPCWSTR);
typedef DWORD (WINAPI * KLAVSYS_PFN_GetFileAttributesA)(LPCSTR);

typedef DWORD (WINAPI * KLAVSYS_PFN_SetFileAttributesW)(LPCWSTR, DWORD);
typedef DWORD (WINAPI * KLAVSYS_PFN_SetFileAttributesA)(LPCSTR, DWORD);

typedef DWORD (WINAPI * KLAVSYS_PFN_GetFullPathNameW)(LPCWSTR, DWORD, LPWSTR, LPWSTR *);
typedef DWORD (WINAPI * KLAVSYS_PFN_GetFullPathNameA)(LPCSTR, DWORD, LPSTR, LPSTR *);

struct KLAVSYS_WINAPI_W
{
	KLAVSYS_PFN_CreateFileW       pfn_CreateFile;
	KLAVSYS_PFN_GetTempPathW      pfn_GetTempPath;
	KLAVSYS_PFN_GetTempFileNameW  pfn_GetTempFileName;
	KLAVSYS_PFN_DeleteFileW       pfn_DeleteFile;
	KLAVSYS_PFN_MoveFileW         pfn_MoveFile;
	KLAVSYS_PFN_LoadLibraryW      pfn_LoadLibrary;
	KLAVSYS_PFN_GetFileAttributesW pfn_GetFileAttributes;
	KLAVSYS_PFN_SetFileAttributesW pfn_SetFileAttributes;
	KLAVSYS_PFN_GetFullPathNameW  pfn_GetFullPathName;
#ifdef KL_PLATFORM_WINCE
	KLAVSYS_PFN_CreateFileForMappingW pfn_CreateFileForMapping;
#endif // KL_PLATFORM_WINCE
	klav_bool_t                   present;  // flag: API present
};

#ifndef KL_PLATFORM_WINCE
struct KLAVSYS_WINAPI_A
{
	KLAVSYS_PFN_CreateFileA       pfn_CreateFile;
	KLAVSYS_PFN_GetTempPathA      pfn_GetTempPath;
	KLAVSYS_PFN_GetTempFileNameA  pfn_GetTempFileName;
	KLAVSYS_PFN_DeleteFileA       pfn_DeleteFile;
	KLAVSYS_PFN_MoveFileA		  pfn_MoveFile; 
	KLAVSYS_PFN_LoadLibraryA      pfn_LoadLibrary;
	KLAVSYS_PFN_GetFileAttributesA pfn_GetFileAttributes;
	KLAVSYS_PFN_SetFileAttributesA pfn_SetFileAttributes;
	KLAVSYS_PFN_GetFullPathNameA  pfn_GetFullPathName;
	klav_bool_t                   present;  // flag: API present
};
#endif // KL_PLATFORM_WINCE

struct KLAVSYS_WINAPI
{
	struct KLAVSYS_WINAPI_W w;  // wide-character (UNICODE) APIs
#ifndef KL_PLATFORM_WINCE
	struct KLAVSYS_WINAPI_A a;  // narrow-character (ANSI)  APIs
#endif // KL_PLATFORM_WINCE
};

KLAV_EXTERN_C
KLAVSYS_API
const struct KLAVSYS_WINAPI * KLAVSYS_WINAPI_Load ();

////////////////////////////////////////////////////////////////
// Synchronization objects

class KLAV_Win32_Mutex : public KLAV_IFACE_IMPL(KLAV_Mutex)
{
public:
	KLAV_Win32_Mutex (KLAV_Alloc* alloc);
	virtual ~KLAV_Win32_Mutex ();

	virtual KLAV_ERR KLAV_CALL lock ();
	virtual KLAV_ERR KLAV_CALL unlock ();
	virtual void KLAV_CALL destroy ();

protected:
	CRITICAL_SECTION  m_cs;
	KLAV_Alloc *      m_alloc;
};

class KLAV_Win32_RWLock : public KLAV_IFACE_IMPL(KLAV_RWLock)
{
public:
	KLAV_Win32_RWLock (KLAV_Alloc* alloc);
	virtual ~KLAV_Win32_RWLock ();

	virtual KLAV_ERR KLAV_CALL lock_read ();
	virtual KLAV_ERR KLAV_CALL lock_write ();
	virtual KLAV_ERR KLAV_CALL unlock ();
	virtual void KLAV_CALL destroy () ;

protected:
	CRITICAL_SECTION m_cs;
	KLAV_Alloc *     m_alloc;
};

class KLAV_Win32_CondVar : public KLAV_IFACE_IMPL(KLAV_CondVar)
{
public:
	KLAV_Win32_CondVar (KLAV_Alloc* alloc);
	virtual ~KLAV_Win32_CondVar ();
	
	virtual KLAV_ERR KLAV_CALL lock ();
	virtual KLAV_ERR KLAV_CALL unlock ();
	virtual KLAV_ERR KLAV_CALL broadcast ();
	virtual KLAV_ERR KLAV_CALL wait ();
	virtual KLAV_ERR KLAV_CALL timedwait (uint32_t reltime);
	virtual void KLAV_CALL destroy ();

private:
	KLAV_Alloc* m_alloc;
	HANDLE m_mutex;
	HANDLE m_event;
};

class KLAV_Win32_Sync_Factory : public KLAV_IFACE_IMPL(KLAV_Sync_Factory)
{
public:
	KLAV_Win32_Sync_Factory (KLAV_Alloc* alloc);
	virtual ~KLAV_Win32_Sync_Factory ();

	virtual KLAV_Mutex * KLAV_CALL create_mutex (klav_bool_t recursive);
	virtual KLAV_RWLock * KLAV_CALL create_rwlock ();
	virtual KLAV_CondVar * KLAV_CALL create_condvar ();

protected:
	KLAV_Alloc * m_alloc;
};

#endif // __cplusplus

////////////////////////////////////////////////////////////////
// Generic virtual memory wrapper

class KLAV_OS_Virtual_Memory : public KLAV_IFACE_IMPL(KLAV_Virtual_Memory)
{
public:
	KLAV_OS_Virtual_Memory ();
	virtual ~KLAV_OS_Virtual_Memory ();

	virtual uint32_t KLAV_CALL vm_pagesize ();
	virtual KLAV_ERR KLAV_CALL vm_alloc (void **pptr, uint32_t size, uint32_t prot, void * pref_addr);
	virtual KLAV_ERR KLAV_CALL vm_free (void *ptr, uint32_t size);
	virtual KLAV_ERR KLAV_CALL vm_protect (void *ptr, uint32_t size, uint32_t newprot);

private:
	KLAV_OS_Virtual_Memory (const KLAV_OS_Virtual_Memory&);
	KLAV_OS_Virtual_Memory& operator= (const KLAV_OS_Virtual_Memory&);
};

////////////////////////////////////////////////////////////////
// IO object creation

KLAV_EXTERN_C
KLAVSYS_API
KLAV_ERR KLAVSYS_Create_IO_Object (
			KLAV_Alloc * alloc,
			HANDLE hdl,
			KLAV_Properties *props,
			KLAV_IO_Object **pobject
		);

////////////////////////////////////////////////////////////////

#if defined (KL_PLATFORM_WINCE)
#define GetProcAddress GetProcAddressA
#endif // KL_PLATFORM_WINCE

#endif // KL_PLATFORM_WINDOWS

#endif // klavsys_windows_h_INCLUDED


