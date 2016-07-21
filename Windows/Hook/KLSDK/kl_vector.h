#ifndef _KL_VECTOR_H_
#define _KL_VECTOR_H_

#include "kldef.h"
#include "kl_object.h"

template <class T> class CKl_Vector : public CKl_Object
{
public:
    void*   m_Memory;
    ULONG   m_MemorySize;

    CKl_Vector();
    virtual ~CKl_Vector();

    T*  operator[] (int n)
    {
        return (T*)( (char*)(m_Memory) + n * sizeof (T) );
    }

    void*   Alloc(int n);
    void*   Realloc(int n); // Add
    
    void*   push_back(T* Item);
};


template <class T> CKl_Vector<T>::CKl_Vector()
{    
}

template <class T> CKl_Vector<T>::~CKl_Vector()
{    
}

template <class T> inline void* CKl_Vector<T>::Alloc(int n)
{
    m_MemorySize    = n * sizeof (T);
    m_Memory        = KL_MEM_ALLOC ( m_MemorySize );
    return m_Memory;
}

template <class T> inline void* CKl_Vector<T>::Realloc(int n)
{
    void*   Memory = m_Memory;
    
    m_MemorySize += n * sizeof (T);
    m_Memory = KL_MEM_ALLOC ( m_MemorySize );

    if ( m_Memory == NULL )
    {
        m_Memory = Memory;
        return NULL;
    }

    RtlCopyMemory ( m_Memory, Memory, m_MemorySize - n * sizeof (T) );

    KL_MEM_FREE(Memory);

    return m_Memory;
}

#endif