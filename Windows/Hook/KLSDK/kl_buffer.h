#ifndef _KL_BUFFER_H_
#define _KL_BUFFER_H_

#include "kldef.h"
#include "kl_list.h"
#include "klstatus.h"
#include "kl_file.h"

template <class T>
class CKl_Array : public CKl_Object
{
    T*          m_buffer;
    size_t      m_size;
    size_t      m_count;
    POOL_TYPE   m_pooltype;
    ULONG       m_tag;
public:
    CKl_Array( size_t  Size, POOL_TYPE PoolType = NonPagedPool, ULONG Tag = COMMON_TAG ) :
      m_pooltype(PoolType), m_count(Size), m_size ( Size * sizeof ( T ) ), m_tag(Tag)
    {   
        m_buffer = (T*)ExAllocatePoolWithTag( m_pooltype, m_size, m_tag );
        
        if ( m_buffer == NULL )
        {            
            m_count = m_size = 0;
        }
    }
    virtual ~CKl_Array()
    {
        if ( m_buffer )
            ExFreePool( m_buffer );
    }

    void    Zeromem()
    {
        if ( m_buffer )
            RtlZeroMemory ( m_buffer, m_size );
    }

    T*      Buffer()
    {
        return m_buffer;
    }
    
    size_t  Size()
    {
        return m_size;
    }

    T&  operator[] (size_t  i)
    {
        return m_buffer[i];
    }
};

// класс-обертка для работы с буффером
class CKl_Buffer : public CKl_Object
{
public:
    char*   m_Buffer;
    ULONG   m_Length;
    bool    m_AutoFree;

    QUEUE_ENTRY;
    CKl_Buffer  ( char*    buf = NULL, ULONG size = 0, bool AutoFree = false );
    virtual ~CKl_Buffer ( );

    // копирует содержимое буффера в m_Buffer. 
    // Возвращает размер скопированных данных
    ULONG     CopyFrom(PCHAR  SrcBuffer, ULONG    BytesToCopy);

    // копирует данные из m_Buffer. BytesToCopy байт, начиная с Offset-байта 
    // Возвращает размер скопированных данных
    ULONG     CopyTo(PCHAR    DstBuffer, ULONG Offset, ULONG   BytesToCopy);

    bool      Find(PCHAR    DstBuffer, ULONG    DstBufferLength, ULONG* DstOffset);
    
    char&  operator[] (size_t  i)
    {
        return m_Buffer[i];
    }

    char*     GetBuffer();

    ULONG     GetBufferLength();

//    KLSTATUS  Process(void* ctx);
};

#endif