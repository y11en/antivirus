// win_tmpfile.cpp
//
//

#if defined (_WIN32)

#include <klava/klavsys_windows.h>
#include <klava/klav_string.h>

////////////////////////////////////////////////////////////////

class KLAV_Temp_Object_Manager_Win32 :
		public KLAV_IFACE_IMPL(KLAV_Temp_Object_Manager)
{
public:
	KLAV_Temp_Object_Manager_Win32 (KLAV_Alloc *alloc);
	virtual ~KLAV_Temp_Object_Manager_Win32 ();

	virtual KLAV_ERR KLAV_CALL create_temp_object (
				KLAV_CONTEXT_TYPE context,
				KLAV_Properties * props,
				KLAV_IO_Object **ptempobj
			);

	KLAV_Alloc * allocator () const
		{ return m_allocator; }

private:
	KLAV_Alloc           * m_allocator;
	const KLAVSYS_WINAPI * m_winapi;
};

KLAV_Temp_Object_Manager_Win32::KLAV_Temp_Object_Manager_Win32 (KLAV_Alloc *alloc)
	: m_allocator (alloc)
{
	if (m_allocator == 0)
		m_allocator = KLAV_Get_System_Allocator ();
}

KLAV_Temp_Object_Manager_Win32::~KLAV_Temp_Object_Manager_Win32 ()
{
}

KLAV_ERR KLAV_CALL KLAV_Temp_Object_Manager_Win32::create_temp_object (
			KLAV_CONTEXT_TYPE context,
			KLAV_Properties * props,
			KLAV_IO_Object **ptempobj
		)
{
	KLAV_ERR err = KLAV_OK;
	*ptempobj = 0;

	const KLAVSYS_WINAPI *winapi = KLAVSYS_WINAPI_Load ();
	if (winapi == 0)
		return KLAV_ENOTIMPL;

	const char * name_prefix = "";
	HANDLE hfile = INVALID_HANDLE_VALUE;

	klav_string temp_path;

#ifndef KL_PLATFORM_WINCE
	if (winapi->w.present)
	{
#endif // KL_PLATFORM_WINCE
		klav_wstring wprefix;
		if (name_prefix != 0 && ! wprefix.assign (name_prefix, allocator ()))
			return KLAV_ENOMEM;

		const wchar_t *wname_prefix = wprefix.c_str ();
		if (wname_prefix == 0)
			wname_prefix = L"";

		wchar_t temp_path_buf [MAX_PATH + 1];
		uint32_t n = winapi->w.pfn_GetTempPath (MAX_PATH, temp_path_buf);
		if (n == 0)
			return KLAV_Get_System_Error ();
		if (n > MAX_PATH)
			return KLAV_EUNKNOWN;

		wchar_t temp_fname_buf [MAX_PATH + 1];
		n = winapi->w.pfn_GetTempFileName (temp_path_buf, wprefix.c_str (), 0, temp_fname_buf);
		if (n == 0)
			return KLAV_Get_System_Error ();

		temp_path.assign (temp_fname_buf, allocator ());
#ifndef KL_PLATFORM_WINCE
	}
	else
	{
		if (name_prefix == 0)
			name_prefix = "";

		char temp_path_buf [MAX_PATH + 1];
		uint32_t n = winapi->a.pfn_GetTempPath (MAX_PATH, temp_path_buf);
		if (n == 0)
			return KLAV_Get_System_Error ();
		if (n > MAX_PATH)
			return KLAV_EUNKNOWN;

		char temp_fname_buf [MAX_PATH + 1];
		n = winapi->a.pfn_GetTempFileName (temp_path_buf, name_prefix, 0, temp_fname_buf);
		if (n == 0)
			return KLAV_Get_System_Error ();

		temp_path.assign (temp_fname_buf, allocator ());
	}
#endif // KL_PLATFORM_WINCE

	KLAV_IO_Object *io_object = 0;
	
	err = KLAVSYS_Create_File (
			temp_path.c_str (),
			KLAV_IO_READWRITE,
			KLAV_FILE_CREATE_ALWAYS | KLAV_FILE_F_TEMPORARY,
			props,
			& io_object
		);

	if (KLAV_FAILED (err))
	{
		// TODO: delete file ?
		return err;
	}

	*ptempobj = io_object;
	return KLAV_OK;
}

////////////////////////////////////////////////////////////////

hKLAV_Temp_Object_Manager KLAV_CALL KLAVSYS_Get_Temp_Object_Manager ()
{
	static KLAV_Temp_Object_Manager_Win32 g_temp_object_manager (0);
	return & g_temp_object_manager;
}

#endif // defined (_WIN32)

