#ifndef _KL_LOCK_H
#define _KL_LOCK_H

#include "kldef.h"
#include "kl_object.h"

#define LOCK_INITIALIZED    0x1
#define LOCK_ACQUIRED       0x2
#define LOCK_RELEASED       0x3
#define LOCK_DESTROYED      0x4

#pragma pack(push, 1)
class CKl_Lock : public CKl_Object
{
public:

    SPIN_LOCK   Lock;
    char        bState;
    IRQL        OldIrql;

    int Acquire();
    int Release();
    
    CKl_Lock();
    ~CKl_Lock();
};

// Класс для автоматической лочки. Выделяется исключительно на стеке.
class CKl_AutoLock
{
public:
    IRQL        m_OldIrql;
    SPIN_LOCK*  m_pLock;

    CKl_AutoLock(SPIN_LOCK* Lock);
    ~CKl_AutoLock();
};


class CKl_AutoRef
{
    CKl_Object* m_Object;
public:
    CKl_AutoRef(CKl_Object* Obj) : m_Object(Obj) 
    { 
        if ( m_Object )
            m_Object->Ref();
    }
    ~CKl_AutoRef()
    {
        if ( m_Object )
            m_Object->Deref();
    }
};
#pragma pack(pop)

#endif // _KL_LOCK_H