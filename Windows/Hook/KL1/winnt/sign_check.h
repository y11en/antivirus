#ifndef _SIGN_CHECK_H__
#define _SIGN_CHECK_H__

#include "kldef.h"

struct CReadObj {
    PCHAR m_pBase;
    ULONG m_Size;
    ULONG m_Offset;

    CReadObj( PCHAR base, ULONG size ) : m_Offset(0), m_pBase(base), m_Size(size) {};


    ULONG Read( PCHAR TargetBuffer, ULONG Size )
    {
        ULONG CopySize = Size;

        if ( m_Offset + Size > m_Size )
            CopySize = m_Size - m_Offset;

        if ( CopySize )
        {
            memcpy ( TargetBuffer, m_pBase + m_Offset, CopySize );
            m_Offset += CopySize;
            return CopySize;
        }

        return 0;
    }
};

bool
SignCheck( 
          PCHAR pBasePtr, 
          ULONG Size,
          wchar_t* ModuleName
          );

#endif