// win_alloc.cpp
//
//

#if defined (_WIN32)

#include <klava/klavsys_windows.h>

struct KLAV_Win_Alloc : public KLAV_IFACE_IMPL(KLAV_Alloc)
{
	KLAV_Win_Alloc ();
	virtual ~KLAV_Win_Alloc ();

    virtual uint8_t * KLAV_CALL alloc (size_t size);
    virtual uint8_t * KLAV_CALL realloc (void *ptr, size_t newsize);
    virtual void      KLAV_CALL free (void *ptr);
};

KLAV_Win_Alloc::KLAV_Win_Alloc ()
{
}

KLAV_Win_Alloc::~KLAV_Win_Alloc ()
{
}

uint8_t * KLAV_CALL KLAV_Win_Alloc::alloc (size_t size)
{
	return (uint8_t *) HeapAlloc (GetProcessHeap (), HEAP_ZERO_MEMORY, size);
}

uint8_t * KLAV_CALL KLAV_Win_Alloc::realloc (void *ptr, size_t newsize)
{
	if (ptr == 0)
		return (uint8_t *) HeapAlloc (GetProcessHeap (), HEAP_ZERO_MEMORY, newsize);
	else
		return (uint8_t *) HeapReAlloc (GetProcessHeap (), HEAP_ZERO_MEMORY, ptr, newsize);
}

void KLAV_CALL KLAV_Win_Alloc::free (void *ptr)
{
	if (ptr != 0)
		HeapFree (GetProcessHeap (), 0, ptr);
}

////////////////////////////////////////////////////////////////

static KLAV_Win_Alloc g_alloc;

KLAV_EXTERN_C
hKLAV_Alloc KLAV_Get_System_Allocator ()
{
	return & g_alloc;
}

#endif // _WIN32

