// kdl_loader.cpp
//
//

#include <klava/klavsys.h>
#include <klava/klavdb.h>
#include <klava/klav_utils.h>
#include <crt_s.h>

////////////////////////////////////////////////////////////////

class KLAV_KDL_Loader : public KLAV_IFACE_IMPL(KLAV_DSO_Loader)
{
public:
	KLAV_KDL_Loader (const KLAV_KDL_Loader_Parms *parms);
	virtual ~KLAV_KDL_Loader ();

	KLAV_ERR init ();

	virtual KLAV_ERR KLAV_CALL dl_open  (const char *path, void **phdl);
	virtual void *   KLAV_CALL dl_sym   (void *hdl, const char *name);
	virtual void     KLAV_CALL dl_close (void *hdl);

	hKLAV_Alloc allocator () const
		{ return m_allocator; }

	hKLAV_DSO_Loader sys_loader () const
		{ return m_sysldr; }

private:
	KLAV_Alloc * m_allocator;
	KLAV_IO_Library * m_iolib;
	KLAV_Virtual_Memory * m_vmem;
	KLAV_DSO_Loader * m_sysldr;
	KLAV_DSO_Loader * m_pe_ldr;  // PE loader
	klav_string       m_db_folder;
	uint32_t          m_flags;

	void cleanup ();
};

////////////////////////////////////////////////////////////////

KLAV_KDL_Loader::KLAV_KDL_Loader (const KLAV_KDL_Loader_Parms *parms)
	: m_allocator (parms->m_alloc),
	  m_iolib (parms->m_iolib),
	  m_vmem (parms->m_vmem),
	  m_sysldr (parms->m_sysldr),
	  m_pe_ldr (0),
	  m_flags (parms->m_flags)
{
	if (parms->m_dbdir != 0)
		m_db_folder.assign (parms->m_dbdir, allocator ());
}

KLAV_KDL_Loader::~KLAV_KDL_Loader ()
{
	cleanup ();
}

KLAV_ERR KLAV_KDL_Loader::init ()
{
	KLAV_ERR err = KLAV_OK;

#ifdef KL_ARCH_X86
	if ((m_flags & KLAV_KDL_LOADER_SYSTEM) == 0)
	{
		err = KLAV_PE_Loader_Create (m_allocator, 0, m_iolib, m_vmem, m_sysldr, & m_pe_ldr);
		if (KLAV_FAIL (err))
			return err;

		m_sysldr = m_pe_ldr;
	}
#endif // KL_ARCH_X86

	return err;
}

void KLAV_KDL_Loader::cleanup ()
{
#ifdef KL_ARCH_X86
	if (m_pe_ldr != 0)
	{
		KLAV_PE_Loader_Destroy (m_pe_ldr);
		m_pe_ldr = 0;
	}
#endif // KL_ARCH_X86
}

KLAV_ERR KLAV_CALL KLAV_KDL_Loader::dl_open  (const char *path, void **phdl)
{
	KLAV_ERR err = KLAV_OK;

	if (phdl == 0)
		return KLAV_EINVAL;

	*phdl = 0;

	if (path == 0 || path [0] == 0)
		return KLAV_EINVAL;

	const char *full_path = 0;
	char * full_path_buf = 0;

	if (m_db_folder.length () == 0)
	{
		full_path = path; // search by system path
	}
	else
	{
		if (KLAV_Fname_Is_Absolute_Path (path))
		{
			if ((m_flags & KLAV_KDL_LOADER_NO_ABSPATH) != 0)
				return KLAV_EINVAL;

			full_path = path;
		}
		else
		{
			// compose full path
			size_t mod_dir_len = m_db_folder.length ();
			if (mod_dir_len != 0)
			{
				size_t path_len = strlen (path);
				const size_t path_bufsize = mod_dir_len + path_len + 10;

				full_path_buf = (char *) allocator ()->alloc (path_bufsize);
				if (full_path_buf == 0)
					return KLAV_ENOMEM;

				strcpy_s (full_path_buf, path_bufsize, m_db_folder.c_str ());
				
				char *p = full_path_buf + mod_dir_len;
				if (! KLAV_Fname_Is_Path_Separator (full_path_buf [mod_dir_len - 1]))
					*p++ = KLAV_Fname_Path_Separator ();

				strcpy_s (p, full_path_buf + path_bufsize - p, path);

				full_path = full_path_buf;
			}
		}
	}

	err = sys_loader ()->dl_open (full_path, phdl);

	if (full_path_buf != 0)
		allocator ()->free (full_path_buf);

	return err;
}

void *   KLAV_CALL KLAV_KDL_Loader::dl_sym (void *hdl, const char *name)
{
	return sys_loader ()->dl_sym (hdl, name);
}

void     KLAV_CALL KLAV_KDL_Loader::dl_close (void *hdl)
{
	sys_loader ()->dl_close (hdl);
}

////////////////////////////////////////////////////////////////

KLAV_EXTERN_C
KLAV_ERR KLAV_CALL KLAV_KDL_Loader_Create (
			const KLAV_KDL_Loader_Parms * parms,
			hKLAV_DSO_Loader *   p_kdl_ldr
		)
{
	if (p_kdl_ldr == 0)
		return KLAV_EINVAL;

	*p_kdl_ldr = 0;

	if (parms->m_alloc == 0)
		return KLAV_EINVAL;

	KLAV_KDL_Loader *ldr = KLAV_NEW (parms->m_alloc) KLAV_KDL_Loader (parms);
	if (ldr == 0)
		return KLAV_ENOMEM;

	KLAV_ERR err = ldr->init ();
	if (KLAV_FAIL (err))
	{
		KLAV_DELETE (ldr, ldr->allocator ());
		return err;
	}

	*p_kdl_ldr = (KLAV_DSO_Loader *) ldr;

	return KLAV_OK;
}

KLAV_EXTERN_C
KLAV_ERR KLAV_CALL KLAV_KDL_Loader_Destroy (
			hKLAV_DSO_Loader hldr
		)
{
	if (hldr == 0)
		return KLAV_EINVAL;

	KLAV_KDL_Loader * ldr = (KLAV_KDL_Loader *) hldr;

	KLAV_DELETE (ldr, ldr->allocator ());

	return KLAV_OK;
}

