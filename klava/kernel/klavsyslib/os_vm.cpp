// os_vm.cpp
//

#include <klava/klavsys_os.h>
#include <klava/kl_sys_vmem.h>

////////////////////////////////////////////////////////////////
// Generic virtual memory wrapper

class KLAVSYS_Virtual_Memory : public KLAV_IFACE_IMPL(KLAV_Virtual_Memory)
{
public:
	KLAVSYS_Virtual_Memory ();
	virtual ~KLAVSYS_Virtual_Memory ();

	virtual uint32_t KLAV_CALL vm_pagesize ();
	virtual KLAV_ERR KLAV_CALL vm_alloc (void **pptr, uint32_t size, uint32_t prot, void * pref_addr);
	virtual KLAV_ERR KLAV_CALL vm_free (void *ptr, uint32_t size);
	virtual KLAV_ERR KLAV_CALL vm_protect (void *ptr, uint32_t size, uint32_t newprot);

private:
	KLAVSYS_Virtual_Memory (const KLAVSYS_Virtual_Memory&);
	KLAVSYS_Virtual_Memory& operator= (const KLAVSYS_Virtual_Memory&);
};

////////////////////////////////////////////////////////////////

static unsigned int map_prot (unsigned int prot)
{
	unsigned int n = 0;
	if (prot & KLAV_MEM_PROT_READ) n |= KL_VIRTMEM_READ;
	if (prot & KLAV_MEM_PROT_WRITE) n |= KL_VIRTMEM_WRITE;
	if (prot & KLAV_MEM_PROT_EXEC) n |= KL_VIRTMEM_EXECUTE;
	return n;
}

KLAVSYS_Virtual_Memory::KLAVSYS_Virtual_Memory ()
{
}

KLAVSYS_Virtual_Memory::~KLAVSYS_Virtual_Memory ()
{
}

uint32_t KLAVSYS_Virtual_Memory::vm_pagesize ()
{
	return KLSysNS::vm_pagesize ();
}

KLAV_ERR KLAVSYS_Virtual_Memory::vm_alloc (void **pptr, uint32_t size, uint32_t prot, void * pref_addr)
{
	*pptr = KLSysNS::vm_allocate (size, map_prot (prot), pref_addr);
	if (*pptr == 0) return KLAV_EUNKNOWN;
	return KLAV_OK;
}

KLAV_ERR KLAVSYS_Virtual_Memory::vm_free (void *ptr, uint32_t size)
{
	KLSysNS::vm_deallocate (ptr, size);
	return KLAV_OK;
}

KLAV_ERR KLAVSYS_Virtual_Memory::vm_protect (void *ptr, uint32_t size, uint32_t newprot)
{
	bool ok = KLSysNS::vm_protect (ptr, size, map_prot (newprot));
	if (! ok) return KLAV_EUNKNOWN;
	return KLAV_OK;
}

KLAV_Virtual_Memory * KLAV_CALL KLAVSYS_Get_Virtual_Memory ()
{
	static KLAVSYS_Virtual_Memory g_vmem;
	return & g_vmem;
}

