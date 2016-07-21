// db_mgr.cpp
//
// KLAV_DB_Manager implementation
//

#include <klava/klavdb.h>
#include <klava/klav_utils.h>
#include <klava/klavstl/string.h>

#define KLAV_DB_MGR_VERSION_CURRENT KLAV_ENGINE_VERSION_CURRENT

typedef klavstl::string <klav_allocator> klav_stringbuf;

#define KDBM_DEBUG(ARGLIST) this->trace_debug ARGLIST
#define KDBM_TRACE(ARGLIST) this->trace ARGLIST

////////////////////////////////////////////////////////////////

class KLAV_DBM_Impl : public KLAV_IFACE_IMPL (KLAV_DB_Manager)
{
public:
			KLAV_DBM_Impl (const KLAV_DBM_Init_Parms& init_parms);
	virtual ~KLAV_DBM_Impl ();

	KLAV_ERR open (const char *base_dir);

	KLAV_ERR close ();

	bool is_open () const
		{ return m_is_open; }

	virtual uint32_t KLAV_CALL get_dbm_version ();

	virtual void * KLAV_CALL get_dbm_iface (uint32_t ifc_id);

	virtual KLAV_ERR KLAV_CALL query_db_file ( // TODO: 'stat' interface
				const char *      relpath     // relative path
			);

	virtual KLAV_ERR KLAV_CALL open_db_file (
				const char *      name,       // relative path
				uint32_t          access,     // access mode
				uint32_t          flags,      // open flags
				hKLAV_Properties  hprops,     // additional extension properties
				hKLAV_IO_Object * phfile
			);

	hKLAV_Alloc allocator () const
		{ return m_init_parms.allocator; }

	hKLAV_IO_Library io_library () const
		{ return m_init_parms.io_library; }

private:
	KLAV_DBM_Init_Parms m_init_parms;
	klav_stringbuf      m_db_folder;
	bool                m_is_open;         // flag: loader is opened

	KLAV_ERR compose_db_path (const char *name, klav_stringbuf& path);
	KLAV_ERR do_open (const char *base_dir);
	void cleanup ();

	void trace_debug (const char *fmt, ...);
	void trace (int level, const char *fmt, ...);
	void trace_v (int level, const char *fmt, va_list ap);
};

////////////////////////////////////////////////////////////////
// KLAV_DBM_Impl implementation


KLAV_DBM_Impl::KLAV_DBM_Impl (const KLAV_DBM_Init_Parms& init_parms)
	: m_init_parms (init_parms), m_db_folder (init_parms.allocator), m_is_open (false)
{
}

KLAV_DBM_Impl::~KLAV_DBM_Impl ()
{
	cleanup ();
}

void KLAV_DBM_Impl::trace_debug (const char *fmt, ...)
{
	va_list ap;
	va_start (ap, fmt);
	trace_v (KLAV_TRACE_LEVEL_DEBUG, fmt, ap);
	va_end (ap);
}

void KLAV_DBM_Impl::trace (int level, const char *fmt, ...)
{
	va_list ap;
	va_start (ap, fmt);
	trace_v (level, fmt, ap);
	va_end (ap);
}

void KLAV_DBM_Impl::trace_v (int level, const char *fmt, va_list ap)
{
	if (m_init_parms.tracer != 0)
	{
		m_init_parms.tracer->print_message_v (0, level, fmt, ap);
	}
}

KLAV_ERR KLAV_DBM_Impl::open (const char *base_dir)
{
	KLAV_ERR err = KLAV_OK;

	close ();

	KDBM_DEBUG (("KDBM: loading database from %s", base_dir));

	err = do_open (base_dir);
	if (KLAV_FAIL (err))
	{
		KDBM_DEBUG (("KDBM: error loading database, code 0x%X", err));
		cleanup ();
		return err;
	}

	m_is_open = true;

	KDBM_DEBUG (("KDBM: database loader initialized"));
	return KLAV_OK;
}

KLAV_ERR KLAV_DBM_Impl::do_open (const char *base_dir)
{
	KLAV_ERR err = KLAV_OK;

	if (base_dir == 0)
		return KLAV_ENOTIMPL;

	if (is_open ())
	{
		KDBM_DEBUG (("KDBM: database already opened"));
		return KLAV_EALREADY;
	}

	if (base_dir == 0)
	{
		KDBM_DEBUG (("KDBM: invalid base_dir"));
		return KLAV_EINVAL;
	}

	hKLAV_IO_Library iolib = io_library ();
	if (iolib == 0)
	{
		KDBM_DEBUG (("KDBM: IO library not available"));
		return KLAV_ENOTIMPL;
	}

	if (base_dir [0] != 0)
	{
		uint32_t attrs = 0;
		err = iolib->get_file_attrs (0, base_dir, KLAV_IO_ATTRS_GENERAL, & attrs);
		if (err != KLAV_ENOTIMPL)
		{
			if (KLAV_FAIL (err))
			{
				KDBM_TRACE ((KLAV_TRACE_LEVEL_ERROR, "KDBM: invalid database path: %s, error 0x%X", base_dir, err));
				return err;
			}

			if ((attrs & KLAV_IO_ATTR_IS_DIRECTORY) == 0)
			{
				// error: not a directory
				KDBM_TRACE ((KLAV_TRACE_LEVEL_ERROR, "KDBM: invalid database path: %s (not a directory)", base_dir));
				return KLAV_EINVAL;
			}
		}
	}

	// terminate base folder with separator
	m_db_folder = base_dir;
	if (m_db_folder.length () > 0 && ! KLAV_Fname_Is_Path_Separator (m_db_folder [m_db_folder.length () - 1]))
		m_db_folder += KLAV_Fname_Path_Separator ();

	return KLAV_OK;
}

KLAV_ERR KLAV_DBM_Impl::close ()
{
	if (! is_open ())
		return KLAV_EALREADY;

	cleanup ();
	return KLAV_OK;
}

void KLAV_DBM_Impl::cleanup ()
{
	m_db_folder.clear ();
	m_is_open = false;
}

uint32_t KLAV_DBM_Impl::get_dbm_version ()
{
	return KLAV_DB_MGR_VERSION_CURRENT;
}

void * KLAV_DBM_Impl::get_dbm_iface (uint32_t ifc_id)
{
	return 0;
}

KLAV_ERR KLAV_DBM_Impl::compose_db_path (const char *name, klav_stringbuf& path)
{
	path.clear ();
	if (name == 0 || name [0] == 0 || KLAV_Fname_Is_Absolute_Path (name))
	{
		KDBM_TRACE ((KLAV_TRACE_LEVEL_ERROR, "KDBM: invalid database file name: %s", name));
		return KLAV_EINVAL;
	}

	path.assign (m_db_folder);
	path.append (name);

	return KLAV_OK;
}

KLAV_ERR KLAV_DBM_Impl::query_db_file (
			const char *  name     // relative path
		)
{
	KLAV_ERR err = KLAV_OK;

	if (! is_open ())
	{
		KDBM_DEBUG (("KDBM: database not opened"));
		return KLAV_ENOINIT;
	}

	klav_stringbuf path (allocator ());
	err = compose_db_path (name, path);
	if (KLAV_FAIL (err))
		return err;

	hKLAV_IO_Library iolib = io_library ();
	if (iolib == 0)
	{
		KDBM_DEBUG (("KDBM: IO library not available"));
		return KLAV_ENOTIMPL;
	}

	uint32_t attrs = 0;
	err = iolib->get_file_attrs (0, path.c_str (), KLAV_IO_ATTRS_GENERAL, & attrs);
	if (KLAV_FAIL (err))
	{
		KDBM_TRACE ((KLAV_TRACE_LEVEL_ERROR, "KDBM: cannot query file: %s, error=0x%X", path.c_str (), err));
		return err;
	}

	if ((attrs & KLAV_IO_ATTR_IS_FILE) == 0)
	{
		// not a regular file
		KDBM_TRACE ((KLAV_TRACE_LEVEL_ERROR, "KDBM: cannot open file: %s (not a file)", path.c_str (), err));
		return KLAV_EINVAL;
	}

	return KLAV_OK;
}

KLAV_ERR KLAV_DBM_Impl::open_db_file (
			const char *      name,       // relative path
			uint32_t          access,     // access mode
			uint32_t          flags,      // open flags
			hKLAV_Properties  hprops,     // additional extension properties
			hKLAV_IO_Object * phfile
		)
{
	KLAV_ERR err = KLAV_OK;
	*phfile = 0;

	if (! is_open ())
	{
		KDBM_DEBUG (("KDBM: database not opened"));
		return KLAV_ENOINIT;
	}

	klav_stringbuf path (allocator ());
	err = compose_db_path (name, path);
	if (KLAV_FAIL (err))
		return err;

	hKLAV_IO_Library iolib = io_library ();
	if (iolib == 0)
	{
		KDBM_DEBUG (("KDBM: IO library not available"));
		return KLAV_ENOTIMPL;
	}

	KDBM_DEBUG (("KDBM: opening file: %s", path.c_str ()));

	hKLAV_IO_Object hio = 0;
	err = iolib->create_file (0, path.c_str (), access, flags, hprops, & hio);
	if (KLAV_FAIL (err))
	{
		KDBM_TRACE ((KLAV_TRACE_LEVEL_ERROR, "KDBM: cannot open file: %s, error=0x%X", path.c_str (), err));
		return err;
	}

	*phfile = hio;
	return KLAV_OK;
}

////////////////////////////////////////////////////////////////

KLAV_ERR KLAV_CALL KLAV_DBM_Create (
			const KLAV_DBM_Init_Parms * init_parms,
			hKLAV_DB_Manager *phmgr
		)
{
	*phmgr = 0;
	if (init_parms == 0)
		return KLAV_EINVAL;

	if (init_parms->struct_size != sizeof (KLAV_DBM_Init_Parms))
		return KLAV_EINVAL;

	if (init_parms->allocator == 0)
		return KLAV_EINVAL;

	KLAV_DBM_Impl * mgr = KLAV_NEW (init_parms->allocator) KLAV_DBM_Impl (*init_parms);
	if (mgr == 0)
		return KLAV_ENOMEM;

	*phmgr = mgr;
	return KLAV_OK;
}

KLAV_ERR KLAV_CALL KLAV_DBM_Open (
			hKLAV_DB_Manager hmgr,
			const char *base_dir
		)
{
	KLAV_ERR err = KLAV_OK;
	if (hmgr == 0)
		return KLAV_ENOINIT;

	KLAV_DBM_Impl *mgr = static_cast <KLAV_DBM_Impl *> (hmgr);
	err = mgr->open (base_dir);
	if (KLAV_FAIL (err))
		return err;

	return KLAV_OK;
}

KLAV_ERR KLAV_CALL KLAV_DBM_Close (
			hKLAV_DB_Manager hmgr
		)
{
	KLAV_ERR err = KLAV_OK;
	if (hmgr == 0)
		return KLAV_ENOINIT;

	KLAV_DBM_Impl *mgr = static_cast <KLAV_DBM_Impl *> (hmgr);
	err = mgr->close ();
	if (KLAV_FAIL (err))
		return err;

	return KLAV_OK;
}

KLAV_ERR KLAV_CALL KLAV_DBM_Destroy (
			hKLAV_DB_Manager hmgr
		)
{
	if (hmgr == 0)
		return KLAV_ENOINIT;

	KLAV_DBM_Impl *mgr = static_cast <KLAV_DBM_Impl *> (hmgr);
	KLAV_DELETE (mgr, mgr->allocator ());

	return KLAV_OK;
}

