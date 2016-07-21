#include "kldef.h"
#include "kl_object.h"
#include "kl_debug.h"

CKl_Object::CKl_Object() : m_nRefCount(0)
{
}

CKl_Object::~CKl_Object()
{
}

void* CKl_Object::operator new(size_t size)
{
    void*   Memory = KL_MEM_ALLOC(size);

#ifdef TRACK_MEM
    //Dbg->Print(BIT_TRACK_MEM, LEVEL_INTERESTING, "Alloc %x bytes +(%x)\n", size, Memory );
#endif

    return Memory;
}

void CKl_Object::operator delete(void* p)
{    
    ASSERT ( p );
    if ( p )
    {
#ifdef TRACK_MEM
      //  Dbg->Print(BIT_TRACK_MEM, LEVEL_INTERESTING, "Free -(%x)\n", p );
#endif
        KL_MEM_FREE( p );
    }
}

ULONG
CKl_Object::Ref()
{
    InterlockedIncrement( &m_nRefCount );

    return m_nRefCount;
}

ULONG
CKl_Object::Deref()
{
    ULONG   nRefCount = InterlockedDecrement( &m_nRefCount );

    if ( nRefCount == 0 )
    {
        delete this;
    }

    return nRefCount;
}
