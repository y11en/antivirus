// file_loader.cpp
//
// Platform-native DBFragmentLoader implementation
//

#include <klava/klavdef.h>
#include <klava/kl_sys_mmap.h>

#include <string.h>
#include <crt_s.h>
#include <klava/klav_string.h>
#include <klava/klav_utils.h>

KLAV_File_Fragment_Loader::KLAV_File_Fragment_Loader ()
	: m_allocator (0), m_dbFolder (0), m_dbPrefix (0), m_dbSuffix (0), m_namebuf (0), m_namebuflen (0)
{
}

KLAV_File_Fragment_Loader::~KLAV_File_Fragment_Loader ()
{
	deinit ();
}

KLAV_ERR KLAV_File_Fragment_Loader::init (KLAV_Alloc *alloc, const char *folder, const char *prefix, const char *suffix)
{
	if (alloc == 0) return KLAV_EINVAL;
	deinit ();

	m_allocator = alloc;
	if (folder == 0) folder = "";
	if (prefix == 0) prefix = "";
	if (suffix == 0) suffix = "";

	m_dbFolder = folder;
	m_dbPrefix = prefix;
	m_dbSuffix = suffix;

	size_t len = strlen (m_dbFolder) + 1 + strlen (m_dbPrefix) + strlen (m_dbSuffix) + 16;

	if (! allocbuf (len))
	{
		deinit ();
		return KLAV_ENOMEM;
	}

	m_namebuflen = len;

	return KLAV_OK;
}

void KLAV_File_Fragment_Loader::deinit ()
{
	freebuf ();
	m_dbFolder = 0;
	m_dbPrefix = 0;
	m_dbSuffix = 0;
	m_allocator = 0;
}

bool KLAV_File_Fragment_Loader::allocbuf (size_t len)
{
	if (m_namebuflen >= len) return true;

	char *nbuf = (char *) m_allocator->alloc (len);
	if (nbuf == 0) return false;

	if (m_namebuf != 0)
	{
		memcpy (nbuf, m_namebuf, m_namebuflen);
		freebuf ();
	}

	m_namebuf = nbuf;
	m_namebuflen = len;
	return true;
}

void KLAV_File_Fragment_Loader::freebuf ()
{
	if (m_namebuf != 0)
	{
		m_allocator->free (m_namebuf);
		m_namebuf = 0;
	}
	m_namebuflen = 0;
}

KLAV_ERR KLAV_CALL KLAV_File_Fragment_Loader::load_fragment (
			const KLAV_DB_Fragment_Desc& fragmentDesc,
			KLAV_DB_Fragment *dbFragment
		)
{
	size_t len = strlen (m_dbFolder) + 1 + strlen (m_dbPrefix) + strlen (m_dbSuffix) + strlen (fragmentDesc.filename);
	if (! allocbuf (len))
		return KLAV_ENOMEM;

	char *p = m_namebuf;
	char *pe = p + m_namebuflen;
	
	if (m_dbFolder[0] != 0)
	{
		strcpy_s (p, pe - p, m_dbFolder); p += strlen (p);
		*p++ = KLAV_Fname_Path_Separator ();
	}
	strcpy_s (p, pe - p, m_dbPrefix); p += strlen (p);
	strcpy_s (p, pe - p, fragmentDesc.filename); p += strlen (p);
	strcpy_s (p, pe - p, m_dbSuffix);

	uint32_t mapFlags = 0;
	if ((fragmentDesc.flags & KLAV_DBLOADER_FLAG_READ) != 0) mapFlags |= KL_FILEMAP_READ;
	if ((fragmentDesc.flags & KLAV_DBLOADER_FLAG_WRITE) != 0) mapFlags |= KL_FILEMAP_COPY_ON_WRITE;
//	TODO: contents must be copied to a (virtual) memory buffer with appropriate protection flags
//	if ((fragmentDesc.flags & KLAV_DBLOADER_FLAG_EXECUTE) != 0) mapFlags |= KL_FILEMAP_EXECUTE;
	if (mapFlags == 0) return KLAV_EINVAL;

	FileMapping *fm = KLAV_NEW (m_allocator) FileMapping ();
	if (fm == 0) return KLAV_ENOMEM;

	int map_err = 0;

#ifdef KL_PLATFORM_WINDOWS
	klav_wstring wnamebuf;
	if (! wnamebuf.assign (m_namebuf, m_allocator))
	{
		KLAV_DELETE (fm, m_allocator);
		return KLAV_ENOMEM;
	}
	map_err = fm->create (wnamebuf.c_str (), mapFlags);
#else
	map_err = fm->create (m_namebuf, mapFlags);
#endif

	if (map_err != 0)
	{
		KLAV_DELETE (fm, m_allocator);
		return KLAV_EUNKNOWN; // TODO: map system error ?
	}

	FileMapping::kl_size_t dataSize = 0;
	uint8_t * data = (uint8_t *)fm->map (0, 0, &dataSize);
	if (data == 0)
	{
		fm->destroy ();
		KLAV_DELETE (fm, m_allocator);
		return KLAV_EUNKNOWN; // TODO: map system error ?
	}

	dbFragment->kdb_handle = fm;
	dbFragment->kdb_data = data;
	dbFragment->kdb_size = (uint32_t)dataSize;

	return KLAV_OK;
}

KLAV_ERR KLAV_CALL KLAV_File_Fragment_Loader::release_fragment (
			const KLAV_DB_Fragment *dbFragment
		)
{
	FileMapping *fm = (FileMapping *)(dbFragment->kdb_handle);
	if (fm == 0) return KLAV_EINVAL;

	KLAV_ERR err = KLAV_OK;
	if (fm->unmap ((void *)dbFragment->kdb_data, dbFragment->kdb_size) != 0) err = KLAV_EUNKNOWN;
	if (fm->destroy () != 0) err = KLAV_EUNKNOWN;

	KLAV_DELETE (fm, m_allocator);

	return err;
}

