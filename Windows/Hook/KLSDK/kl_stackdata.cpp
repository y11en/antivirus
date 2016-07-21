#include "kl_stackdata.h"

ULONG CKl_Fifo::PushData(PVOID srcBuf, ULONG Size)
{
    PVOID   NewBuffer;
    ULONG   NewBufferSize;

    if ( srcBuf == NULL || Size == 0 )
    {
        return 0;
    }

    NewBufferSize = m_Length + Size;

    if ( NewBuffer = KL_MEM_ALLOC ( NewBufferSize ) )
    {
        if ( m_Length && m_Buffer )
        {
            RtlCopyMemory ( NewBuffer, m_Buffer, m_Length );
            KL_MEM_FREE ( m_Buffer );
        }

        RtlCopyMemory( (char*)NewBuffer + m_Length, srcBuf, Size );

        m_Buffer = (char*)NewBuffer;
        m_Length = NewBufferSize;
    }
    
    return Size;
}

ULONG CKl_Fifo::PopData(PVOID dstBuf, ULONG Size)
{
    ULONG   bCopied;
    
    PVOID   NewBuffer       = NULL;
    ULONG   NewBufferSize   = 0;

    if ( Size == 0 || dstBuf == NULL )
        return 0;

    bCopied = CopyTo( (char*)dstBuf, 0, Size );

    if ( NewBufferSize = m_Length - bCopied )
    {
        if ( NewBuffer = KL_MEM_ALLOC ( NewBufferSize ) )
        {
            RtlCopyMemory ( NewBuffer, (char*)m_Buffer + bCopied, NewBufferSize );
        }
    }
    
    KL_MEM_FREE ( m_Buffer );

    m_Buffer = (char*)NewBuffer;
    m_Length = NewBufferSize;

    return bCopied;
}