// kfb_file_loader.cpp
//

#include <klava/klav_string.h>
#include <klava/klav_utils.h>
#include <klava/kl_sys_mmap.h>

struct KFB_File_Loader_Impl
{
	WholeFileMapping m_fm;
	KLAV_Alloc * m_alloc;

	KFB_File_Loader_Impl (KLAV_Alloc *alloc) : m_alloc (alloc) {}
};

KLAV_KFB_File_Loader::KLAV_KFB_File_Loader ()
	: m_pimpl (0)
{
}

KLAV_KFB_File_Loader::~KLAV_KFB_File_Loader ()
{
	close ();
}

KLAV_ERR KLAV_KFB_File_Loader::open (KLAV_Alloc *alloc, KLAV_Virtual_Memory *vm, const char *filepath)
{
	if (m_pimpl != 0) return KLAV_EALREADY;

	m_pimpl = KLAV_NEW (alloc) KFB_File_Loader_Impl (alloc);
	if (m_pimpl == 0) return KLAV_ENOMEM;

	uint32_t prot = KL_FILEMAP_READ;
	if (vm == 0)
		prot |= KL_FILEMAP_COPY_ON_WRITE; // assume in-place relocate & execute

	FileMapping::kl_size_t fileSize = 0;

#ifdef KL_PLATFORM_WINDOWS
	klav_wstring wfilepath;
	if (! wfilepath.assign (filepath, alloc))
		return KLAV_ENOMEM;
		
	void *data = m_pimpl->m_fm.map (wfilepath.c_str (), prot, &fileSize);
#else
	void *data = m_pimpl->m_fm.map (filepath, prot, &fileSize);
#endif
	if (data == 0)
	{
		close ();
		return KLAV_EUNKNOWN;
	}

	KLAV_KFB_Loader::init (alloc, vm);

	KLAV_ERR err = KLAV_KFB_Loader::open (data, (uint32_t)fileSize);
	if (err != KLAV_OK) { close (); return err; }

	return KLAV_OK;
}

KLAV_ERR KLAV_KFB_File_Loader::close ()
{
	if (m_pimpl == 0) return KLAV_EALREADY;

	KLAV_KFB_Loader::close ();

	KLAV_DELETE (m_pimpl, m_pimpl->m_alloc);
	m_pimpl = 0;

	return KLAV_OK;
}

