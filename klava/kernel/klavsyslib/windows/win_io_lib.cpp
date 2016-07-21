// win_io_lib.cpp
//
// Win32 IO library
//

#if defined (_WIN32)

#include <klava/klavsys_windows.h>
#include <klava/klav_string.h>

#ifdef KL_PLATFORM_WINCE
# define USE_UNC_PATH 0
#else
# define USE_UNC_PATH 1
#endif

#if USE_UNC_PATH
# define MAKE_UNC_PATH(SRC,RES) do { \
			klav_wstring unc_path_tmp; \
			if (! unc_path_tmp.assign (SRC, allocator ())) \
				return KLAV_ENOMEM; \
			KLAV_ERR err = GetUncName(winapi, unc_path_tmp.c_str(), RES, allocator ()); \
			if (KLAV_FAILED(err)) \
				return err; \
		} while (0)
#else // USE_UNC_PATH
# define MAKE_UNC_PATH(SRC,RES) do { \
			klav_wstring RES; \
			if (! RES.assign (SRC, allocator ())) \
				return KLAV_ENOMEM; \
		} while (0)
#endif // USE_UNC_PATH

////////////////////////////////////////////////////////////////

class KLAV_IO_Library_Win32 :
		public KLAV_IFACE_IMPL(KLAV_IO_Library)
{
public:
	KLAV_IO_Library_Win32 (KLAV_Alloc *alloc);
	virtual ~KLAV_IO_Library_Win32 ();

	virtual uint32_t KLAV_CALL get_io_version ();

	virtual void * KLAV_CALL get_io_iface (
				uint32_t io_iface_id
			);

	virtual KLAV_ERR KLAV_CALL create_file (
				KLAV_CONTEXT_TYPE context,
				const char * path,
				uint32_t access,
				uint32_t flags,
				KLAV_Properties *props,
				KLAV_IO_Object **pobject
			);

	virtual KLAV_ERR KLAV_CALL delete_file (
				KLAV_CONTEXT_TYPE context,
				const char * path,
				uint32_t     flags
			);

	virtual KLAV_ERR KLAV_CALL get_file_attrs (
				KLAV_CONTEXT_TYPE context,
				const char * path,
				uint32_t     attrs_grp,
				uint32_t *   pvalue
			);

	virtual KLAV_ERR KLAV_CALL set_file_attrs (
				KLAV_CONTEXT_TYPE context,
				const char *    path,
				uint32_t        attrs_grp,
				uint32_t		value
			);

	virtual KLAV_ERR KLAV_CALL move_file (
				KLAV_CONTEXT_TYPE context,
				const char * src_path,
				const char * dst_path,
				uint32_t     flags
			);

	virtual KLAV_ERR KLAV_CALL make_full_path (
				KLAV_CONTEXT_TYPE context,
				const char * src_path,
				uint32_t     flags,
				KLAV_Alloc * path_allocator,
				char ** pdst_path
			);

	KLAV_Alloc * allocator () const
		{ return m_allocator; }


private:
	KLAV_Alloc  * m_allocator;
};

////////////////////////////////////////////////////////////////

KLAV_IO_Library_Win32::KLAV_IO_Library_Win32 (KLAV_Alloc *alloc) :
	m_allocator (alloc)
{
	if (m_allocator == 0)
		m_allocator = KLAV_Get_System_Allocator ();
}

KLAV_IO_Library_Win32::~KLAV_IO_Library_Win32 ()
{
}

uint32_t KLAV_CALL KLAV_IO_Library_Win32::get_io_version ()
{
	return KLAV_IO_VERSION_CURRENT;
}

void * KLAV_CALL KLAV_IO_Library_Win32::get_io_iface (
				uint32_t io_iface_id
			)
{
	return 0;
}

#if USE_UNC_PATH
KLAV_ERR GetUncName(const KLAVSYS_WINAPI *winapi, const WCHAR* src, klav_wstring& dst_path, KLAV_Alloc * path_allocator)
{
	KLAV_ERR err = KLAV_OK;

	const WCHAR *prefix = L"\\\\?\\";
	const WCHAR *uncprefix = L"\\\\?\\UNC";
	uint32_t prefix_len = 6; // strlen (uncprefix) - 1

	WCHAR stkbuf [MAX_PATH + 7]; // MAX_PATH + prefix_len + 1
	WCHAR * buf = stkbuf;

	uint32_t n = 0;
	LPWSTR pname = 0;
	
	n = winapi->w.pfn_GetFullPathName (src, MAX_PATH, buf + prefix_len, & pname);
	if (n == 0)
		return  KLAV_Get_System_Error ();

	WCHAR * alloc_buf = 0;

	if (n >= MAX_PATH)
	{
		// allocate buffer and retry
		alloc_buf = (WCHAR *) (path_allocator->alloc ((n + prefix_len + 16) * sizeof (WCHAR)));
		if (alloc_buf == 0)
			return KLAV_ENOMEM;

		buf = alloc_buf;

		n = winapi->w.pfn_GetFullPathName (src, n + 1, buf + prefix_len, & pname);
		if (n == 0)
		{
			err = KLAV_Get_System_Error ();
			path_allocator->free (alloc_buf);
			return err;
		}
		if (n > n + 1)
		{
			path_allocator->free (alloc_buf);
			return KLAV_EUNKNOWN;
		}
	}

	if (buf [prefix_len] == '\\' && buf [prefix_len + 1] == '\\')
	{
		// UNC path
		for (WCHAR *s = (WCHAR *) uncprefix, *d = buf; *s != 0; ++s, ++d)
			*d = *s;
	}
	else
	{
		buf += 2; // strlen (uncprefix) - 1 - strlen (prefix)
		for (WCHAR *s = (WCHAR *) prefix, *d = buf; *s != 0; ++s, ++d)
			*d = *s;
	}

	if (! dst_path.assign (buf, path_allocator))
		err = KLAV_ENOMEM;

	if (alloc_buf != 0)
		path_allocator->free (alloc_buf);

	return err;
}
#endif // USE_UNC_PATH

KLAV_ERR KLAV_CALL KLAV_IO_Library_Win32::create_file (
			KLAV_CONTEXT_TYPE context,
			const char * path,
			uint32_t access,
			uint32_t flags,
			KLAV_Properties *props,
			KLAV_IO_Object **pobject
		)
{
	KLAV_ERR err = KLAV_OK;
	*pobject = 0;

	if (path == 0 || path [0] == 0)
		return KLAV_EINVAL;

	const KLAVSYS_WINAPI *winapi = KLAVSYS_WINAPI_Load ();
	if (winapi == 0)
		return KLAV_ENOIMPL;

	DWORD access_mode = 0;
	DWORD share_mode = 0;
	if (access == KLAV_IO_READWRITE)
	{
		access_mode = GENERIC_READ | GENERIC_WRITE;
		share_mode = FILE_SHARE_READ;
	}
	else if (access == KLAV_IO_READONLY)
	{
		access_mode = GENERIC_READ;
		share_mode = FILE_SHARE_READ|FILE_SHARE_WRITE|FILE_SHARE_DELETE;
	}
	else
	{
		return KLAV_EINVAL;
	}

	DWORD create_mode = 0;
	switch (flags & KLAV_FILE_CREATE_MODE)
	{
	case KLAV_FILE_CREATE_NEW:
		create_mode = CREATE_NEW;
		break;
	case KLAV_FILE_CREATE_ALWAYS:
		create_mode = CREATE_ALWAYS;
		break;
	case KLAV_FILE_OPEN_EXISTING:
		create_mode = OPEN_EXISTING;
		break;
	case KLAV_FILE_OPEN_ALWAYS:
		create_mode = OPEN_ALWAYS;
		break;
	case KLAV_FILE_TRUNCATE_EXISTING:
		create_mode = TRUNCATE_EXISTING;
		break;
	default:
		return KLAV_EINVAL;
	}

	DWORD create_attrs = 0;
	if ((flags & KLAV_FILE_F_READ_ONLY) != 0)
	{
		create_attrs |= FILE_ATTRIBUTE_READONLY;
	}
	
	if ((flags & KLAV_FILE_F_TEMPORARY) != 0)
	{
		create_attrs |= FILE_ATTRIBUTE_TEMPORARY | FILE_FLAG_DELETE_ON_CLOSE;
	}

	HANDLE hfile = INVALID_HANDLE_VALUE;

	// TODO: for WCE: create file for mapping, if read-only access is requested

#ifndef KL_PLATFORM_WINCE
	if (winapi->w.present)
	{
#endif // KL_PLATFORM_WINCE
		klav_wstring wpath;
		MAKE_UNC_PATH (path, wpath);

		hfile = winapi->w.pfn_CreateFile (wpath.c_str (), access_mode, share_mode, 0, create_mode, create_attrs, 0);
		if (hfile == INVALID_HANDLE_VALUE)
			return KLAV_Get_System_Error ();
#ifndef KL_PLATFORM_WINCE
	}
	else
	{
		hfile = winapi->a.pfn_CreateFile (path, access_mode, share_mode, 0, create_mode, create_attrs, 0);
		if (hfile == INVALID_HANDLE_VALUE)
			return KLAV_Get_System_Error ();
	}
#endif // KL_PLATFORM_WINCE

	KLAV_IO_Object *io_object = 0;

	err = KLAVSYS_Create_IO_Object (allocator (), hfile, props, & io_object);
	if (io_object == 0)
	{
		CloseHandle (hfile);
		return err;
	}

	*pobject = io_object;
	return KLAV_OK;
}

KLAV_ERR KLAV_CALL KLAV_IO_Library_Win32::delete_file (
			KLAV_CONTEXT_TYPE context,
			const char * path,
			uint32_t     flags
		)
{
	KLAV_ERR err = KLAV_OK;

	if (path == 0 || path [0] == 0)
		return KLAV_EINVAL;

	const KLAVSYS_WINAPI *winapi = KLAVSYS_WINAPI_Load ();
	if (winapi == 0)
		return KLAV_ENOIMPL;

#ifndef KL_PLATFORM_WINCE
	if (winapi->w.present)
	{
#endif // KL_PLATFORM_WINCE
		klav_wstring wpath;
		MAKE_UNC_PATH (path, wpath);

		if (! winapi->w.pfn_DeleteFile (wpath.c_str ()))
			err = KLAV_Get_System_Error ();
#ifndef KL_PLATFORM_WINCE
	}
	else
	{
		if (! winapi->a.pfn_DeleteFile (path))
			err = KLAV_Get_System_Error ();
	}
#endif // KL_PLATFORM_WINCE

	return err;
}

KLAV_ERR KLAV_CALL KLAV_IO_Library_Win32::get_file_attrs (
			KLAV_CONTEXT_TYPE context,
			const char * path,
			uint32_t     attrs_grp,
			uint32_t *   pvalue
		)
{
	if (path == 0 || pvalue == 0 || attrs_grp != KLAV_IO_ATTRS_GENERAL)
		return KLAV_EINVAL;

	const KLAVSYS_WINAPI *winapi = KLAVSYS_WINAPI_Load ();
	if (winapi == 0)
		return KLAV_ENOIMPL;

	DWORD attrs = 0;

#ifndef KL_PLATFORM_WINCE
	if (winapi->w.present)
	{
#endif // KL_PLATFORM_WINCE
		klav_wstring wpath;
		MAKE_UNC_PATH (path, wpath);

		attrs = winapi->w.pfn_GetFileAttributes (wpath.c_str ());
#ifndef KL_PLATFORM_WINCE
	}
	else
	{
		attrs = winapi->a.pfn_GetFileAttributes (path);
	}
#endif // KL_PLATFORM_WINCE

	if (attrs == INVALID_FILE_ATTRIBUTES)
	{
		return KLAV_Get_System_Error ();
	}

	uint32_t io_attrs = 0;

	if ((attrs & FILE_ATTRIBUTE_DIRECTORY) != 0)
		io_attrs |= KLAV_IO_ATTR_IS_DIRECTORY;
	if ((attrs & FILE_ATTRIBUTE_DEVICE) != 0)
		io_attrs |= KLAV_IO_ATTR_IS_DEVICE;
	if ((attrs & (FILE_ATTRIBUTE_DIRECTORY|FILE_ATTRIBUTE_DEVICE)) == 0)
		io_attrs |= KLAV_IO_ATTR_IS_FILE;
	if ((attrs & FILE_ATTRIBUTE_READONLY) != 0)
		io_attrs |= KLAV_IO_ATTR_READ_ONLY;

	*pvalue = io_attrs;

	return KLAV_OK;
}

KLAV_ERR KLAV_CALL KLAV_IO_Library_Win32::set_file_attrs (
			KLAV_CONTEXT_TYPE context,
			const char *    path,
			uint32_t        attrs_grp,
			uint32_t		value
		)
{
	if (path == 0 || attrs_grp != KLAV_IO_ATTRS_GENERAL)
		return KLAV_EINVAL;


	const KLAVSYS_WINAPI *winapi = KLAVSYS_WINAPI_Load ();
	if (winapi == 0)
		return KLAV_ENOIMPL;

	DWORD attrs = 0;

	if ((value & KLAV_IO_ATTR_READ_ONLY) != 0)
		attrs |= FILE_ATTRIBUTE_READONLY;
	

#ifndef KL_PLATFORM_WINCE
	if (winapi->w.present)
	{
#endif // KL_PLATFORM_WINCE
		klav_wstring wpath;
		MAKE_UNC_PATH (path, wpath);

		if (0 == winapi->w.pfn_SetFileAttributes (wpath.c_str (), attrs)){
			return KLAV_Get_System_Error();
		}
#ifndef KL_PLATFORM_WINCE
	}
	else
	{
		if (0 == winapi->a.pfn_SetFileAttributes (path, attrs)){
			return KLAV_Get_System_Error();
		}
	}
#endif // KL_PLATFORM_WINCE


	return KLAV_OK;
}

KLAV_ERR KLAV_CALL KLAV_IO_Library_Win32::move_file (
				KLAV_CONTEXT_TYPE context,
				const char * src_path,
				const char * dst_path,
				uint32_t     flags
		)
{
	KLAV_ERR err = KLAV_OK;

	if (src_path == 0 || src_path [0] == 0)
		return KLAV_EINVAL;

	if (dst_path == 0 || dst_path [0] == 0)
		return KLAV_EINVAL;

	const KLAVSYS_WINAPI *winapi = KLAVSYS_WINAPI_Load ();
	if (winapi == 0)
		return KLAV_ENOIMPL;

#ifndef KL_PLATFORM_WINCE
	if (winapi->w.present)
	{
#endif // KL_PLATFORM_WINCE
		klav_wstring src_wpath, dst_wpath;
		MAKE_UNC_PATH (src_path, src_wpath);
		MAKE_UNC_PATH (dst_path, dst_wpath);

		if (! winapi->w.pfn_MoveFile (src_wpath.c_str (), dst_wpath.c_str()))
			err = KLAV_Get_System_Error ();
#ifndef KL_PLATFORM_WINCE
	}
	else
	{
		if (! winapi->a.pfn_MoveFile (src_path, dst_path))
			err = KLAV_Get_System_Error ();
	}
#endif // KL_PLATFORM_WINCE

	return err;
}

KLAV_ERR KLAV_CALL KLAV_IO_Library_Win32::make_full_path (
				KLAV_CONTEXT_TYPE context,
				const char * src_path,
				uint32_t     flags,
				KLAV_Alloc * path_allocator,
				char ** pdst_path
			)
{
	KLAV_ERR err = KLAV_OK;

	if (pdst_path == 0)
		return KLAV_EINVAL;
		
	*pdst_path = 0;
	if (src_path == 0 || src_path [0] == 0)
		return KLAV_EINVAL;

	const KLAVSYS_WINAPI *winapi = KLAVSYS_WINAPI_Load ();
	if (winapi == 0)
		return KLAV_ENOIMPL;

#ifndef KL_PLATFORM_WINCE
	if (winapi->w.present)
	{
#endif // KL_PLATFORM_WINCE
		klav_wstring src_wpath;
		src_wpath.assign (src_path, allocator ());

		uint32_t bufsz = 0;
		WCHAR * pathbuf = 0;

		uint32_t n = 0;

		for (;;)
		{
			bufsz += MAX_PATH;
			WCHAR * npathbuf = (WCHAR *) path_allocator->realloc (pathbuf, bufsz * sizeof (WCHAR));
			if (npathbuf == 0)
			{
				if (pathbuf != 0)
					path_allocator->free (pathbuf);

				return KLAV_ENOMEM;
			}

			pathbuf = npathbuf;

			LPWSTR pname = 0;
			n = winapi->w.pfn_GetFullPathName (src_wpath.c_str (), bufsz - 1, pathbuf, & pname);
			if (n <= (bufsz - 1))
				break;
		}

		if (n == 0)
		{
			path_allocator->free (pathbuf);
			err = KLAV_Translate_System_Error (err);
		}
		else
		{
			pathbuf [n] = 0;
			klav_string dst_path;
			dst_path.assign (pathbuf, n, allocator ());
			path_allocator->free (pathbuf);

			// TODO: normalize/fold

			*pdst_path = (char *) path_allocator->alloc (dst_path.length () + 1);
			if (*pdst_path == 0)
				return KLAV_ENOMEM;

			memcpy (*pdst_path, dst_path.c_str (), dst_path.length ());
			(*pdst_path) [dst_path.length ()] = 0;
		}
#ifndef KL_PLATFORM_WINCE
	}
	else
	{
		uint32_t bufsz = 0;
		char * pathbuf = 0;

		uint32_t n = 0;

		for (;;)
		{
			bufsz += MAX_PATH;
			char * npathbuf = (char *) path_allocator->realloc (pathbuf, bufsz);
			if (npathbuf == 0)
			{
				if (pathbuf != 0)
					path_allocator->free (pathbuf);

				return KLAV_ENOMEM;
			}

			LPSTR pname = 0;
			n = winapi->a.pfn_GetFullPathName (src_path, bufsz - 1, pathbuf, & pname);
			if (n <= (bufsz - 1))
				break;
			if (n == 0)
				break;
		}

		if (n == 0)
		{
			path_allocator->free (pathbuf);
			err = KLAV_Translate_System_Error (err);
		}
		else
		{
			pathbuf [n] = 0;

			// TODO: normalize/fold

			*pdst_path = pathbuf;
		}
	}
#endif // KL_PLATFORM_WINCE

	return KLAV_OK;
}

////////////////////////////////////////////////////////////////

hKLAV_IO_Library KLAV_CALL KLAVSYS_Get_IO_Library ()
{
	static KLAV_IO_Library_Win32 g_io_lib (0);
	return & g_io_lib;
}

#endif // defined (_WIN32)
