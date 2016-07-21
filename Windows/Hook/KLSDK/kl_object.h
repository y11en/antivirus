#ifndef _KL_OBJECT_
#define _KL_OBJECT_

#include "kldef.h"

// базовый класс всех объектов.
// выделяет/освобождает память.
// выполняет простой подсчет ссылок. Когда кол-во ссылок равно 0, объект уничтожается.
#pragma  pack( push, 1)

class CKl_Object 
{
public:
    long            m_nRefCount;

    CKl_Object();
    virtual ~CKl_Object();
    
    void* operator new(size_t size);
    void operator delete(void* p); 
    
    virtual unsigned long   Ref();
    virtual unsigned long   Deref();
};

#pragma pack ( pop )

#endif // _KL_OBJECT_