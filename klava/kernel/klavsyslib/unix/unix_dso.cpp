// unix_dso.cpp
//
// DSO (dynamic shared object) support for unix platforms
//
#if defined (__unix__)
#include <dlfcn.h>
#include <klav_string.h>
#include <klavsys_unix.h>

struct KLAV_DSO_Loader_Unix : public KLAV_IFACE_IMPL(KLAV_DSO_Loader)
{
public:
  KLAV_DSO_Loader_Unix (KLAV_Alloc *allocator);
  virtual ~KLAV_DSO_Loader_Unix ();

  virtual KLAV_ERR KLAV_CALL dl_open  (const char *path, void **phdl);
  virtual void *   KLAV_CALL dl_sym   (void *hdl, const char *name);
  virtual void     KLAV_CALL dl_close (void *hdl);

  KLAV_Alloc * allocator () const
  { return m_allocator; }

private:
  KLAV_Alloc * m_allocator;
};

KLAV_DSO_Loader_Unix::KLAV_DSO_Loader_Unix (KLAV_Alloc *allocator)
  : m_allocator (allocator)
{
	if (m_allocator == 0)
		m_allocator = KLAV_Get_System_Allocator ();
}

KLAV_DSO_Loader_Unix::~KLAV_DSO_Loader_Unix ()
{
}

KLAV_ERR KLAV_CALL KLAV_DSO_Loader_Unix::dl_open (const char *path, void **phdl)
{
  *phdl = 0;
  if (path == 0 || path [0] == 0)
    return KLAV_EINVAL;

  void* h = ::dlopen (path, RTLD_NOW);
  if (h == 0)
    return KLAV_EUNKNOWN;

  *phdl = h;
  
  return KLAV_OK;
}

void * KLAV_CALL KLAV_DSO_Loader_Unix::dl_sym (void *hdl, const char *name)
{
  if (hdl == 0 || name == 0 || name [0] == 0)
    return 0;
  
  return ::dlsym (hdl, name);
}

void KLAV_CALL KLAV_DSO_Loader_Unix::dl_close (void *hdl)
{
  if (hdl != 0)
    ::dlclose (hdl);
}

////////////////////////////////////////////////////////////////

hKLAV_DSO_Loader KLAV_CALL KLAVSYS_Get_DSO_Loader ()
{
	static KLAV_DSO_Loader_Unix g_dso_loader (0);
	return & g_dso_loader;
}

#endif //__unix__
