// pr_io_lib.cpp
//
// Prague-based KLAV_IO_Library implementation
//

#include <klava/klav_prague.h>
#include <klava/klavstl/string.h>

typedef klavstl::string<klav_allocator> string;

#include <plugin/p_nfio.h>
#include <pr_pid.h>

#ifdef _MSC_VER
# pragma intrinsic (memset)
# pragma intrinsic (memcpy)
# pragma intrinsic (strlen)
#endif // _MSC_VER

////////////////////////////////////////////////////////////////
// IO Library

class KLAV_Pr_IO_Library :
		public KLAV_IFACE_IMPL(KLAV_IO_Library)
{
public:
	KLAV_Pr_IO_Library (hOBJECT hparent, KLAV_IO_Library_Create_Parms * parms);
	virtual ~KLAV_Pr_IO_Library ();

	virtual uint32_t KLAV_CALL get_io_version ();

	// get IO extension interface
	virtual void * KLAV_CALL get_io_iface (
				uint32_t ext_iface_id
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
			) { return KLAV_ENOTIMPL; }  // TODO: !!!

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
				uint32_t		pvalue
			) { return KLAV_ENOTIMPL; }  // TODO: !!!

	virtual KLAV_ERR KLAV_CALL move_file (
				KLAV_CONTEXT_TYPE context,
				const char * src_path,
				const char * dst_path,
				uint32_t     flags
			) { return KLAV_ENOTIMPL; }  // TODO: !!!

	virtual KLAV_ERR KLAV_CALL make_full_path (
				KLAV_CONTEXT_TYPE context,
				const char * src_path,
				uint32_t     flags,
				KLAV_Alloc * path_allocator,
				char ** pdst_path
#if defined(KL_PLATFORM_WINDOWS)
			);
#else
			) { return KLAV_ENOTIMPL; }  // TODO: !!!
#endif

	KLAV_Alloc * allocator ()
		{ return & m_allocator; }

private:
	hOBJECT         m_parent;
	KLAV_Pr_Alloc   m_allocator;
	klav_allocator  m_klavstl_allocator;
	string m_base_dir;
	string m_product_dir;
};

////////////////////////////////////////////////////////////////

#if defined(KL_PLATFORM_WINDOWS)

extern "C"
__declspec(dllimport)
uint32_t __stdcall
#if defined(KL_UNICODE)
GetModuleFileNameW(
				   uintptr_t       hModule,
				   const wchar_t * lpFilename,
				   uint32_t        nSize
				   );
#define GetModuleFileName GetModuleFileNameW
#else
GetModuleFileNameA(
				   uintptr_t       hModule,
				   const char *    lpFilename,
				   uint32_t        nSize
				   );
#define GetModuleFileName GetModuleFileNameA
#endif /* KL_UNICODE */

#endif /* KL_PLATFORM_WINDOWS */

static void strip_trailing_slashes(string & path)
{
	size_t len = path.length();
	while (len >= 2 && (path[len - 1] == '\\' || path[len - 1] == '/'))
		path.resize(--len);
}

KLAV_Pr_IO_Library::KLAV_Pr_IO_Library (hOBJECT hparent, KLAV_IO_Library_Create_Parms * parms) :
	m_parent (hparent), 
	m_allocator (hparent), 
	m_klavstl_allocator (&m_allocator),
	m_base_dir (m_klavstl_allocator), 
	m_product_dir (m_klavstl_allocator)
{
	if (parms->iolib_base_dir)
	{
		if (m_base_dir.assign(parms->iolib_base_dir))
			strip_trailing_slashes(m_base_dir);
	}
	if (parms->iolib_product_dir)
	{
		if (m_product_dir.assign(parms->iolib_product_dir))
			strip_trailing_slashes(m_product_dir);
	}
#if defined(KL_PLATFORM_WINDOWS)
	else
	{
		kl_tchar_t self_path[0x200 + 1] = { 0 };
		if (GetModuleFileName(0, self_path, 0x200))
		{
			if (kl_tchar_t * last_backslash = kl_tcsrchr(self_path, '\\'))
			{
				*last_backslash = 0;
				klav_string self_path_utf8;
				if (self_path_utf8.assign(self_path, &m_allocator))
				{
					m_product_dir.assign(self_path_utf8.c_str(), self_path_utf8.length());
				}
			}
		}
	}
#endif /* KL_PLATFORM_WINDOWS */
}

KLAV_Pr_IO_Library::~KLAV_Pr_IO_Library ()
{
}

uint32_t KLAV_CALL KLAV_Pr_IO_Library::get_io_version ()
{
	return KLAV_IO_VERSION_CURRENT;
}

void * KLAV_CALL KLAV_Pr_IO_Library::get_io_iface (
				uint32_t io_iface_id
			)
{
	return 0;
}

KLAV_ERR KLAV_CALL KLAV_Pr_IO_Library::create_file (
				KLAV_CONTEXT_TYPE context,
				const char * path,
				uint32_t access,
				uint32_t flags,
				KLAV_Properties *props,
				KLAV_IO_Object **pobject
			)
{
	tERROR error = errOK;
	*pobject = 0;

	if (path == 0 || path [0] == 0)
		return KLAV_EINVAL;

	tDWORD access_mode = 0;
	tDWORD open_mode = 0;

	switch (access)
	{
	case KLAV_IO_READONLY:
		access_mode |= fACCESS_READ;
		break;
	case KLAV_IO_READWRITE:
		access_mode |= fACCESS_RW;
		open_mode |= fOMODE_SHARE_DENY_WRITE | fOMODE_SHARE_DENY_DELETE;
		break;
	default:
		return KLAV_EINVAL;
	}

	switch (flags & KLAV_FILE_CREATE_MODE)
	{
	case KLAV_FILE_CREATE_NEW:
		open_mode |= fOMODE_CREATE_IF_NOT_EXIST | fOMODE_TRUNCATE;
		break;
	case KLAV_FILE_CREATE_ALWAYS:
		open_mode |= fOMODE_OPEN_IF_EXIST | fOMODE_CREATE_IF_NOT_EXIST | fOMODE_TRUNCATE;
		break;
	case KLAV_FILE_OPEN_EXISTING:
		open_mode |= fOMODE_OPEN_IF_EXIST;
		break;
	case KLAV_FILE_OPEN_ALWAYS:
		open_mode |= fOMODE_OPEN_IF_EXIST | fOMODE_CREATE_IF_NOT_EXIST;
		break;
	case KLAV_FILE_TRUNCATE_EXISTING:
		open_mode |= fOMODE_OPEN_IF_EXIST | fOMODE_TRUNCATE;
		break;
	default:
		return KLAV_EINVAL;
	}

	if ((flags & KLAV_FILE_F_TEMPORARY) != 0)
		open_mode |= fOMODE_TEMPORARY;

	hIO hio = 0;
	error = m_parent->sysCreateObject ((hOBJECT *)& hio, IID_IO, PID_NFIO, SUBTYPE_ANY);
	if (PR_FAIL (error))
		return KLAV_PR_ERROR (error);

	tPTR pathbuf = 0;
	tDWORD pathlen = 0;
	tCODEPAGE cp = 0;
	
#ifdef KL_UNICODE
	klav_wstring wpath;
	wpath.assign (path, & m_allocator);
	pathbuf = (void *) wpath.c_str ();
	pathlen = (tDWORD) (wpath.length () * sizeof (wchar_t));
	cp = cCP_UNICODE;
#else // KL_UNICODE
	pathbuf = (void *) path;
	pathlen = (tDWORD) strlen (path);
	cp = cCP_ANSI;
#endif // KL_UNICODE

	error = hio->set_pgOBJECT_FULL_NAME(pathbuf, pathlen, cp);

	if (PR_SUCC (error))
		error = hio->set_pgOBJECT_ACCESS_MODE (access_mode);
	if (PR_SUCC (error))
		error = hio->set_pgOBJECT_OPEN_MODE (open_mode);
	if (PR_SUCC (error))
		error = hio->sysCreateObjectDone ();

	if (PR_FAIL (error))
	{
		hio->sysCloseObject ();
		return KLAV_PR_ERROR (error);
	}

	KLAV_IO_Object *io_obj = 0;
	error = KLAV_Pr_Create_IO_Object (m_parent, hio, true, & io_obj);
	if (PR_FAIL (error))
	{
		hio->sysCloseObject ();
		return error;
	}

	*pobject = io_obj;
	return KLAV_OK;
}

KLAV_ERR KLAV_CALL KLAV_Pr_IO_Library::get_file_attrs (
				KLAV_CONTEXT_TYPE context,
				const char * path,
				uint32_t     attrs_grp,
				uint32_t *   pvalue
			)
{
	return KLAV_ENOTIMPL;
}

#if defined(KL_PLATFORM_WINDOWS)

extern "C"
__declspec(dllimport)
uint32_t __stdcall
#if defined(KL_UNICODE)
SearchPathW(
			const wchar_t * lpPath,
			const wchar_t * lpFileName,
			const wchar_t * lpExtension,
			uint32_t        nBufferLength,
			wchar_t *       lpBuffer,
			wchar_t * *     lpFilePart
			);
#define SearchPath SearchPathW
#else
SearchPathA(
			const char * lpPath,
			const char * lpFileName,
			const char * lpExtension,
			uint32_t     nBufferLength,
			char *       lpBuffer,
			char * *     lpFilePart
			);
#define SearchPath SearchPathA
#endif

template<class T> KLAV_ERR win_search_path(const char * filename, string * p_res_path)
{
	PR_ASSERT(filename);
	PR_ASSERT(p_res_path);

	// allocate buffer for SearchPath
	klavstl::trivial_vector<T, klav_allocator> buf(p_res_path->get_allocator());
	if (!buf.assign(1024, T()))
		return KLAV_ENOMEM;
	// convert file name to WideChar/UTF-8
	klav_tstring tmp_filename;
	if (!tmp_filename.assign(filename, p_res_path->get_allocator()))
		return KLAV_ENOMEM;
	T * filepart;
	uint32_t pathlen = SearchPath(NULL, tmp_filename.c_str(), NULL, (uint32_t)buf.size(), &buf[0], &filepart);
	if (pathlen == 0)
		return KLAV_ENOTFOUND;
	if (pathlen >= buf.size())
		return KLAV_EUNEXPECTED;
	// convert SearchPath result to UTF-8
	klav_string tmp_path;
	if (!tmp_path.assign(&buf[0], pathlen, p_res_path->get_allocator()))
		return KLAV_ENOMEM;
	if (!p_res_path->assign(tmp_path.c_str(), tmp_path.length()))
		return KLAV_ENOMEM;
	return KLAV_OK;
}

KLAV_ERR KLAV_CALL KLAV_Pr_IO_Library::make_full_path (
				KLAV_CONTEXT_TYPE context,
				const char * src_path,
				uint32_t     flags,
				KLAV_Alloc * path_allocator,
				char ** pdst_path
			)
{
	if (flags & KLAV_PATH_F_NORMALIZE_CASE)
		return KLAV_ENOTIMPL;

	if (src_path == 0 || src_path [0] == 0 || pdst_path == 0)
		return KLAV_EINVAL;

	*pdst_path = 0;

	KLAV_ERR err = KLAV_EUNEXPECTED;
	klav_allocator klavstl_alloc(allocator());
	klavstl::string<klav_allocator> res_path(klavstl_alloc);

	switch (flags & KLAV_PATH_F_SEARCH_MODE_MASK)
	{
	case KLAV_PATH_SEARCH_MODE_EXE:
	case KLAV_PATH_SEARCH_MODE_DLL:
		err = win_search_path<kl_tchar_t>(src_path, &res_path);
		break;

	case KLAV_PATH_SEARCH_MODE_BASE:
		if (m_base_dir.empty())
		{
			err = KLAV_ENOINIT;
		}
		else
		{
			if (!res_path.assign(m_base_dir))
				err = KLAV_ENOMEM;
			else if (!res_path.append(1, '\\'))
				err = KLAV_ENOMEM;
			else if (!res_path.append(src_path))
				err = KLAV_ENOMEM;
			else
				err = KLAV_OK;
		}
		break;

	case KLAV_PATH_SEARCH_MODE_PRODUCT:
		if (m_product_dir.empty())
		{
			err = KLAV_ENOINIT;
		}
		else
		{
			if (!res_path.assign(m_product_dir))
				err = KLAV_ENOMEM;
			else if (!res_path.append(1, '\\'))
				err = KLAV_ENOMEM;
			else if (!res_path.append(src_path))
				err = KLAV_ENOMEM;
			else
				err = KLAV_OK;
		}
		break;

	default:
		err = KLAV_EINVAL;
	}

	if (KLAV_FAILED(err))
		return err;

	*pdst_path = (char *) path_allocator->alloc(res_path.length() + 1);
	if (!*pdst_path)
		return KLAV_ENOMEM;

	memcpy(*pdst_path, res_path.c_str(), res_path.length() + 1);
	return KLAV_OK;
}

#endif /* KL_PLATFORM_WINDOWS */

////////////////////////////////////////////////////////////////

KLAV_EXTERN_C
hKLAV_IO_Library KLAV_CALL
KLAV_Pr_IO_Library_Create (hOBJECT parent, struct KLAV_IO_Library_Create_Parms * parms)
{
	KLAV_Pr_Alloc allocator (parent);
	return KLAV_NEW (& allocator) KLAV_Pr_IO_Library (parent, parms);
}

KLAV_EXTERN_C
void KLAV_CALL
KLAV_Pr_IO_Library_Destroy (hKLAV_IO_Library h)
{
	if (h != 0)
	{
		KLAV_Pr_IO_Library * impl = static_cast <KLAV_Pr_IO_Library *> (h);
		KLAV_DELETE (impl, impl->allocator ());
	}
}

