#include "kl_buffer.h"

CKl_Buffer::CKl_Buffer(char* buf /* = NULL */, ULONG size /* = 0 */, bool AutoFree /* = false  */)
{
    m_AutoFree     = AutoFree;
    m_Buffer       = buf;
    m_Length       = size;
}

CKl_Buffer::~CKl_Buffer()
{
    if ( m_AutoFree )
    {
        KL_MEM_FREE ( m_Buffer );
    }
}

// копирует содержимое буффера в m_Buffer
ULONG     
CKl_Buffer::CopyFrom(PCHAR  SrcBuffer, ULONG    BytesToCopy)
{
    if ( BytesToCopy > m_Length || m_Buffer == NULL)
        return 0;
    
    RtlCopyMemory ( m_Buffer, SrcBuffer, BytesToCopy );
    
    return BytesToCopy;
}

// копирует данные из m_Buffer. BytesToCopy байт, начиная с Offset-байта 
ULONG     
CKl_Buffer::CopyTo(PCHAR    DstBuffer, ULONG Offset, ULONG   BytesToCopy)
{
    if ( Offset >= m_Length || m_Buffer == NULL )
        return 0;
    
    // Если нужно скопировать больше, чем есть на самом деле, то скопируем сколько можно
    if ( m_Length - Offset < BytesToCopy )
        BytesToCopy = m_Length - Offset;
    
    RtlCopyMemory ( DstBuffer, (PCHAR)m_Buffer + Offset, BytesToCopy );
    
    return BytesToCopy;
}

char*
CKl_Buffer::GetBuffer()
{
    return m_Buffer;
}

ULONG
CKl_Buffer::GetBufferLength()
{
    return m_Length;
}

bool
CKl_Buffer::Find( PCHAR DstBuffer, ULONG DstBufferLength, ULONG* DstOffset )
{    
    PCHAR   First   = m_Buffer;

    if ( m_Buffer == NULL || m_Length < DstBufferLength )
        return FALSE;
    
    while ( m_Length - (ULONG)(First - m_Buffer) >= DstBufferLength )
    {
        First = strchr( First, DstBuffer[0] );
        
        if ( First )
        {
            ULONG   i = 0;
            while ( (UCHAR)First[i] == (UCHAR)DstBuffer[i] && i < DstBufferLength )
                ++i;

            if ( i == DstBufferLength )
            {
                if ( DstOffset )
                {
                    *DstOffset = i;
                }
                return TRUE;
            }

            First += i;
        }
        else
        {
            return FALSE;
        }
    }    

    return FALSE;
}
