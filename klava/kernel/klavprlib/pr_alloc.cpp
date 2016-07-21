// prgalloc.cpp
//
// KlavPragueAlloc implementation
//

#include <klava/klav_prague.h>

uint8_t * KLAV_Pr_Alloc::alloc (size_t size)
{
	tPTR ptr = 0;
	tERROR err = CALL_SYS_ObjHeapAlloc (m_hObject, &ptr, (tDWORD)size);
	if (PR_FAIL (err)) return 0;
	return (uint8_t *)ptr;
}

uint8_t * KLAV_Pr_Alloc::realloc (void *ptr, size_t newsize)
{
	tPTR nptr = 0;
	tERROR err = CALL_SYS_ObjHeapRealloc (m_hObject, &nptr, ptr, (tDWORD)newsize);
	if (PR_FAIL (err)) return 0;
	return (uint8_t *)nptr;
}

void KLAV_Pr_Alloc::free (void *ptr)
{
	CALL_SYS_ObjHeapFree (m_hObject, ptr);
}

