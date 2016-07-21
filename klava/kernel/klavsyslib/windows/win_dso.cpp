// win_dso.cpp
//
// DSO (dynamic shared object) support for Win32 platforms
//

#if defined (_WIN32)

#include <klava/klavsys_windows.h>
#include <klava/klav_string.h>

typedef HMODULE (WINAPI * PFN_LoadLibraryW)(LPCWSTR);
typedef HMODULE (WINAPI * PFN_LoadLibraryA)(LPCSTR);

struct KLAV_DSO_Loader_Win32 : public KLAV_IFACE_IMPL(KLAV_DSO_Loader)
{
public:
	KLAV_DSO_Loader_Win32 (KLAV_Alloc *alloc = 0);

	virtual ~KLAV_DSO_Loader_Win32 ();

	virtual KLAV_ERR KLAV_CALL dl_open  (const char *path, void **phdl);
	virtual void *   KLAV_CALL dl_sym   (void *hdl, const char *name);
	virtual void     KLAV_CALL dl_close (void *hdl);

	KLAV_Alloc * allocator () const
		{ return m_allocator; }

private:
	KLAV_Alloc * m_allocator;
};

KLAV_DSO_Loader_Win32::KLAV_DSO_Loader_Win32 (KLAV_Alloc *alloc) :
		m_allocator (alloc)
{
	if (m_allocator == 0)
		m_allocator = KLAV_Get_System_Allocator ();
}

KLAV_DSO_Loader_Win32::~KLAV_DSO_Loader_Win32 ()
{
}

KLAV_ERR KLAV_CALL KLAV_DSO_Loader_Win32::dl_open (const char *path, void **phdl)
{
	KLAV_ERR err = KLAV_OK;
	*phdl = 0;
	
	if (path == 0 || path [0] == 0)
		return KLAV_EINVAL;

	HMODULE hmod = 0;

	const KLAVSYS_WINAPI *winapi = KLAVSYS_WINAPI_Load ();
	if (winapi == 0)
		return KLAV_ENOTIMPL;

	unsigned int old_mode = SetErrorMode (SEM_FAILCRITICALERRORS|SEM_NOOPENFILEERRORBOX);

#ifndef KL_PLATFORM_WINCE
	if (winapi->w.present)
	{
#endif // KL_PLATFORM_WINCE
		klav_wstring wpath;
		wpath.assign (path, allocator ());
		hmod = winapi->w.pfn_LoadLibrary (wpath.c_str ());
#ifndef KL_PLATFORM_WINCE
	}
	else
	{
		hmod = winapi->a.pfn_LoadLibrary (path);
	}
#endif // KL_PLATFORM_WINCE

	if (hmod == 0)
		err = KLAV_Get_System_Error ();

	SetErrorMode (old_mode);

	*phdl = hmod;
	return err;
}

void * KLAV_CALL KLAV_DSO_Loader_Win32::dl_sym (void *hdl, const char *name)
{
	if (hdl == 0)
		return 0;

	if (name == 0 || name [0] == 0)
		return 0;

	void * sym = GetProcAddress ((HMODULE) hdl, name);
	return sym;
}

void KLAV_CALL KLAV_DSO_Loader_Win32::dl_close (void *hdl)
{
	if (hdl != 0)
	{
		FreeLibrary ((HMODULE) hdl);
	}
}

////////////////////////////////////////////////////////////////

hKLAV_DSO_Loader KLAV_CALL KLAVSYS_Get_DSO_Loader ()
{
	static KLAV_DSO_Loader_Win32 g_dso_loader (0);
	return & g_dso_loader;
}

#endif //_WIN32
