// klav_direnum.cpp
//
#include <string.h>

#include <klava/klav_direnum.h>
#include <klava/kl_sys_direnum.h>

#include <klava/klavsys.h>

#ifdef KL_PLATFORM_WINDOWS
# define KLAV_DIREN_UTF8
#endif // KL_PLATFORM_WINDOWS

#ifdef KL_PLATFORM_UNIX
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#endif

////////////////////////////////////////////////////////////////

struct KLAV_Dir_Enum_Impl
{
	klav_filepos_t m_file_size;
	char *         m_file_name;
	uint32_t       m_file_flags;
	uint32_t       m_fname_bufsz;
	uint32_t       m_search_flags;

	KLSysNS::DirectoryEnumerator m_diren;

#ifdef KLAV_DIREN_UTF8
	wchar_t *      m_pattern;
#else  // KLAV_DIREN_UTF8
	char *         m_pattern;
#endif // KLAV_DIREN_UTF8

	KLAV_Dir_Enum_Impl ();
	~KLAV_Dir_Enum_Impl ();

	klav_bool_t reserve_fname_buffer (uint32_t size);
	void free_fname_buffer ();
	void clear_cur_file_info ();

	KLAV_ERR open (const char *path, const char *pattern, uint32_t flags);
	void close ();

	klav_bool_t get_next_file ();
};

KLAV_Dir_Enum_Impl::KLAV_Dir_Enum_Impl () :
	m_file_size (0),
	m_file_name (0),
	m_file_flags (0),
	m_fname_bufsz (0),
	m_search_flags (0),
	m_pattern (0)
{
}

KLAV_Dir_Enum_Impl::~KLAV_Dir_Enum_Impl ()
{
	close ();
	
	free_fname_buffer ();
}

klav_bool_t KLAV_Dir_Enum_Impl::reserve_fname_buffer (uint32_t size)
{
	if (size <= m_fname_bufsz)
		return true;

	// align to next 32 bytes
	size = (size + 31) & ~31;
	char *nbuf = new char [size];
	if (nbuf == 0)
		return false;
		
	if (m_file_name != 0)
		delete [] m_file_name;
	m_file_name = nbuf;
	m_fname_bufsz = size;

	return true;
}

void KLAV_Dir_Enum_Impl::free_fname_buffer ()
{
	if (m_file_name != 0)
	{
		delete [] m_file_name;
		m_file_name = 0;
	}
	m_fname_bufsz = 0;
}

#ifdef KLAV_DIREN_UTF8
wchar_t * utf8_to_wchar (const char *s)
{
	if (s == 0)
		return 0;
	size_t len = strlen (s);
	size_t wlen = len + 1;
	wchar_t * ws = new wchar_t [wlen];
	if (ws == 0)
		return 0;

	int cnt = MultiByteToWideChar (CP_UTF8, 0, s, (unsigned int)len, ws, (unsigned int)wlen);
	if (cnt <= 0 || (size_t) cnt >= wlen)
	{
		delete [] ws;
		return 0;
	}
	ws [cnt] = 0;

	return ws;
}
#endif  // KLAV_DIREN_UTF8

char * copy_str (const char *s)
{
	if (s == 0)
		return 0;
	size_t len = strlen (s);
	char *ns = new char [len + 1];
	if (ns == 0)
		return 0;

	memcpy (ns, s, len);
	ns [len] = 0;

	return ns;
}

KLAV_ERR KLAV_Dir_Enum_Impl::open (const char *path, const char *pattern, uint32_t flags)
{
	close ();

	if (! reserve_fname_buffer (1024))
		return KLAV_ENOMEM;

	bool ok = false;

#ifdef KLAV_DIREN_UTF8
	wchar_t * wpath = utf8_to_wchar (path);
	if (wpath == 0)
		return KLAV_EINVAL;

	if (pattern != 0)
	{
		m_pattern = utf8_to_wchar (pattern);
		if (m_pattern == 0)
		{
			delete [] wpath;
			return KLAV_EINVAL;
		}
	}
	
	ok = m_diren.open (wpath);
	delete [] wpath;

#else  // KLAV_DIREN_UTF8
	if (pattern != 0)
	{
		m_pattern = copy_str (pattern);
		if (m_pattern == 0)
			return KLAV_ENOMEM;
	}

	ok = m_diren.open (path);

#endif // KLAV_DIREN_UTF8

	if (! ok)
	{
		close ();
		return KLAV_EINVAL;
	}

	m_search_flags = flags & (KLAV_FILE_F_REG | KLAV_FILE_F_DIR);

	return KLAV_OK;
}

void KLAV_Dir_Enum_Impl::clear_cur_file_info ()
{
	m_file_size = 0;
	if (m_file_name != 0)
		m_file_name [0] = 0;
	m_file_flags = 0;
}

void KLAV_Dir_Enum_Impl::close ()
{
	clear_cur_file_info ();

	m_diren.close ();
	m_search_flags = 0;

	if (m_pattern != 0)
	{
		delete [] m_pattern;
		m_pattern = 0;
	}
}

klav_bool_t KLAV_Dir_Enum_Impl::get_next_file ()
{
	uint32_t file_flags = 0;

	for (;;)
	{
		clear_cur_file_info ();
		
		bool f = m_diren.nextEntry (m_pattern, true);
		if (! f)
			return 0;

		file_flags = 0;
		if (m_diren.isDirectory ())
			file_flags |= KLAV_FILE_F_DIR;
		else
			file_flags |= KLAV_FILE_F_REG;

		if ((file_flags & m_search_flags) != 0)
			break;
	}

#ifdef KLAV_DIREN_UTF8
	const wchar_t *wfname = m_diren.getFileNameW ();
	uint32_t wfnamelen = (uint32_t) wcslen (wfname);
	uint32_t namelen = wfnamelen * 3 + 5;
	if (! reserve_fname_buffer (namelen))
		return 0;

	int cnt = WideCharToMultiByte (CP_UTF8, 0, wfname, wfnamelen, m_file_name, namelen, 0, 0);
	if (cnt == 0)
		return 0;

	m_file_name [cnt] = 0;

#else  // KLAV_DIREN_UTF8
	const char *fname = m_diren.getFileName ();
	uint32_t fnamelen = (uint32_t) strlen (fname);
	if (! reserve_fname_buffer (fnamelen + 1))
		return 0;
	strcpy (m_file_name, fname);
#endif // KLAV_DIREN_UTF8

	m_file_flags = file_flags;

	return 1;
}

////////////////////////////////////////////////////////////////

KLAV_Dir_Enum::KLAV_Dir_Enum () : m_pimpl (0)
{
	m_pimpl = new KLAV_Dir_Enum_Impl ();
}

KLAV_Dir_Enum::~KLAV_Dir_Enum ()
{
	delete m_pimpl;
}

KLAV_ERR KLAV_Dir_Enum::open (const char *dir, const char *fname_mask, uint32_t flags)
{
	return m_pimpl->open (dir, fname_mask, flags);
}

klav_bool_t KLAV_Dir_Enum::get_next_file ()
{
	return m_pimpl->get_next_file ();
}

void KLAV_Dir_Enum::close ()
{
	m_pimpl->close ();
}

const char * KLAV_Dir_Enum::file_name () const
{
	return m_pimpl->m_file_name;
}

uint32_t KLAV_Dir_Enum::file_flags () const
{
	return m_pimpl->m_file_flags;
}

klav_filepos_t KLAV_Dir_Enum::file_size () const
{
	return m_pimpl->m_file_size;
}

////////////////////////////////////////////////////////////////

// NOTE: for UNIX, escape character must be processed
klav_bool_t KLAV_Name_Has_Wildcards (const char *fname)
{
	const char *s = fname;

	for (; *s != 0; ++s)
	{
		char c = *s;
		if (c == '?' || c == '*' || c == '[' || c == ']')
			return 1;
	}

	return 0;
}

size_t KLAV_Dir_Name_Len (const char *path)
{
	if (path == 0)
		return 0;

	size_t n = strlen (path);
	for ( ; n > 0; --n)
	{
		unsigned char c = path [n-1];
		if (KL_IS_PATH_SEPARATOR (c))
			break;
	}
	return n;
}

// returns combination of KLAV_FILE_F_xxx flags
uint32_t KLAV_Query_File_Type (const char *path)
{
#if defined (KL_PLATFORM_WINDOWS)
	DWORD fa = 0;
	
#ifdef KLAV_DIREN_UTF8
	wchar_t * wpath = utf8_to_wchar (path);
	if (wpath == 0)
		return 0;
	
	fa = GetFileAttributesW (wpath);
	delete [] wpath;
#else   // KLAV_DIREN_UTF8
	fa = GetFileAttributesA (path);
#endif  // KLAV_DIREN_UTF8

	if (fa == INVALID_FILE_ATTRIBUTES)
		return 0;

	uint32_t file_flags = 0;
	if ((fa & FILE_ATTRIBUTE_DIRECTORY) != 0)
		file_flags |= KLAV_FILE_F_DIR;
	else
		file_flags |= KLAV_FILE_F_REG;

	return file_flags;

#elif defined (KL_PLATFORM_UNIX) // KL_PLATFORM_WINDOWS
        struct stat l_stat;
        if ( stat ( path, &l_stat ) )
		return 0;
	uint32_t file_flags = 0;
	if ( S_ISDIR(l_stat.st_mode) )
		file_flags |= KLAV_FILE_F_DIR;
	else if (  S_ISREG(l_stat.st_mode) )
		file_flags |= KLAV_FILE_F_REG;

	return file_flags;
#else
# error Implementation needed...
#endif // KL_PLATFORM_WINDOWS
}

////////////////////////////////////////////////////////////////

