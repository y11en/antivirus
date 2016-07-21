// unix_alloc.cpp
//
//

#ifdef __unix__

#include <klavsys_unix.h>

struct KLAV_Unix_Alloc : public KLAV_IFACE_IMPL(KLAV_Alloc)
{
	KLAV_Unix_Alloc ();
	virtual ~KLAV_Unix_Alloc ();

    virtual uint8_t * KLAV_CALL alloc (size_t size);
    virtual uint8_t * KLAV_CALL realloc (void *ptr, size_t newsize);
    virtual void      KLAV_CALL free (void *ptr);
};

KLAV_Unix_Alloc::KLAV_Unix_Alloc ()
{
}

KLAV_Unix_Alloc::~KLAV_Unix_Alloc ()
{
}

uint8_t * KLAV_CALL KLAV_Unix_Alloc::alloc (size_t size)
{
	return (uint8_t *) ::calloc (1, size);
}

uint8_t * KLAV_CALL KLAV_Unix_Alloc::realloc (void *ptr, size_t newsize)
{
	return (uint8_t *) ::realloc (ptr, newsize);
}

void KLAV_CALL KLAV_Unix_Alloc::free (void *ptr)
{
	if (ptr != 0)
		::free (ptr);
}

////////////////////////////////////////////////////////////////

static KLAV_Unix_Alloc g_alloc;

KLAV_EXTERN_C
hKLAV_Alloc KLAV_Get_System_Allocator ()
{
	return & g_alloc;
}

#endif // __unix__

