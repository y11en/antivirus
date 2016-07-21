// prgvm.cpp
//
// Prague-based virtual memory interface
//

#include <klava/klav_prague.h>

#define IMPEX_IMPORT_LIB
#include <plugin/p_vmarea.h>

KLAV_Pr_Virtual_Memory::KLAV_Pr_Virtual_Memory ()
{
}

KLAV_Pr_Virtual_Memory::~KLAV_Pr_Virtual_Memory ()
{
}

uint32_t KLAV_CALL KLAV_Pr_Virtual_Memory::vm_pagesize ()
{
	if (::vm_pagesize == 0)
		return 0;

	return ::vm_pagesize ();
}

KLAV_ERR KLAV_CALL KLAV_Pr_Virtual_Memory::vm_alloc (void **pptr, uint32_t size, uint32_t prot, void * pref_addr)
{
	if (::vm_allocate == 0)
		return KLAV_ENOTIMPL;

	void *ptr = ::vm_allocate (size, prot, pref_addr);
	if (ptr == 0)
		return KLAV_EUNKNOWN;

	*pptr = ptr;
	return KLAV_OK;
}

KLAV_ERR KLAV_CALL KLAV_Pr_Virtual_Memory::vm_free (void *ptr, uint32_t size)
{
	if (::vm_deallocate == 0)
		return KLAV_ENOTIMPL;

	return ::vm_deallocate (ptr, size);
}

KLAV_ERR KLAV_CALL KLAV_Pr_Virtual_Memory::vm_protect (void *ptr, uint32_t size, uint32_t newprot)
{
	if (::vm_protect == 0)
		return KLAV_ENOTIMPL;

	if (! ::vm_protect (ptr, size, newprot))
		return KLAV_EUNKNOWN;

	return KLAV_OK;
}

