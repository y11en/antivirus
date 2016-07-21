// win_api.cpp
//
// Windows API dynamic loade
//

#if defined (_WIN32)

#include <klava/klavsys_windows.h>

////////////////////////////////////////////////////////////////

struct WINAPI_Loader
{
	WINAPI_Loader ();
	~WINAPI_Loader ();

	KLAVSYS_WINAPI * load ();
	void cleanup ();

	klav_bool_t	do_load_api ();

	CRITICAL_SECTION m_csect;
	KLAVSYS_WINAPI   m_api;
	klav_bool_t      m_loaded;
};

////////////////////////////////////////////////////////////////

WINAPI_Loader::WINAPI_Loader ()
	: m_loaded (0)
{
	InitializeCriticalSection (& m_csect);
	memset (& m_api, 0, sizeof (KLAVSYS_WINAPI));
}

WINAPI_Loader::~WINAPI_Loader ()
{
	cleanup ();
	DeleteCriticalSection (& m_csect);
}

void WINAPI_Loader::cleanup ()
{
	memset (& m_api, 0, sizeof (KLAVSYS_WINAPI));
	m_loaded = 0;
}

KLAVSYS_WINAPI * WINAPI_Loader::load ()
{
	KLAVSYS_WINAPI * api = 0;

	EnterCriticalSection (& m_csect);

	api = & m_api;
	if (! m_loaded)
	{
		if (do_load_api ())
		{
			m_loaded = 1;
		}
		else
		{
			cleanup ();
			api = 0;
		}
	}
	LeaveCriticalSection (& m_csect);

	return api;
}

klav_bool_t WINAPI_Loader::do_load_api ()
{
	OSVERSIONINFO ovi;
	memset (&ovi, 0, sizeof (ovi));
	ovi.dwOSVersionInfoSize = sizeof (ovi);

	GetVersionEx (&ovi);

#ifdef KL_PLATFORM_WINCE
	HMODULE h_api_dll = GetModuleHandleW (L"coredll.dll");
#else
	HMODULE h_api_dll = GetModuleHandle ("kernel32.dll");
#endif
	if (h_api_dll == 0)
		return 0;

#ifndef KL_PLATFORM_WINCE
	bool need_unicode = false;
	bool need_ansi = true;

	if (ovi.dwPlatformId == VER_PLATFORM_WIN32_NT)
		need_unicode = true;
#endif // KL_PLATFORM_WINCE

	#define LOAD_WINAPI_CALL_W(NAME) \
		if ((m_api.w.pfn_##NAME = (KLAVSYS_PFN_##NAME##W) GetProcAddress (h_api_dll, #NAME "W")) == 0) return 0

	#define LOAD_WINAPI_CALL_A(NAME) \
		if ((m_api.a.pfn_##NAME = (KLAVSYS_PFN_##NAME##A) GetProcAddress (h_api_dll, #NAME "A")) == 0) return 0

#ifndef KL_PLATFORM_WINCE
	if (need_unicode)
#endif // KL_PLATFORM_WINCE
	{
		LOAD_WINAPI_CALL_W (CreateFile);
		LOAD_WINAPI_CALL_W (GetTempPath);
		LOAD_WINAPI_CALL_W (GetTempFileName);
		LOAD_WINAPI_CALL_W (DeleteFile);
		LOAD_WINAPI_CALL_W (MoveFile);
		LOAD_WINAPI_CALL_W (LoadLibrary);
		LOAD_WINAPI_CALL_W (GetFileAttributes);
		LOAD_WINAPI_CALL_W (SetFileAttributes);
		LOAD_WINAPI_CALL_W (GetFullPathName);
#ifdef KL_PLATFORM_WINCE
		LOAD_WINAPI_CALL_W (CreateFileForMapping);
#endif // KL_PLATFORM_WINCE
		m_api.w.present = 1;
	}

#ifndef KL_PLATFORM_WINCE
	LOAD_WINAPI_CALL_A (CreateFile);
	LOAD_WINAPI_CALL_A (GetTempPath);
	LOAD_WINAPI_CALL_A (GetTempFileName);
	LOAD_WINAPI_CALL_A (DeleteFile);
	LOAD_WINAPI_CALL_A (MoveFile);
	LOAD_WINAPI_CALL_A (LoadLibrary);
	LOAD_WINAPI_CALL_A (GetFileAttributes);
	LOAD_WINAPI_CALL_A (SetFileAttributes);
	LOAD_WINAPI_CALL_A (GetFullPathName);
	m_api.a.present = 1;
#endif // KL_PLATFORM_WINCE

	return 1;
}

////////////////////////////////////////////////////////////////

static WINAPI_Loader g_winapi_loader;

const struct KLAVSYS_WINAPI * KLAVSYS_WINAPI_Load ()
{
	return g_winapi_loader.load ();
}

#endif // _WIN32
