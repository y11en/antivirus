#ifndef _KDSHARED_H
#define _KDSHARED_H
//----------------------------------------------------------------------------------------------------------------------
#define KD_MAX_STR_SIZE			0x0400
#define KD_BUFFER_LEN			0x1000

#include <pshpack4.h>

typedef
struct _KDPRINT_SECTION
{
	ULONG		m_Signature;

	ULONG		m_BufferTop;
	UCHAR		m_Buffer[KD_BUFFER_LEN];
} KDPRINT_SECTION, *PKDPRINT_SECTION;

#include <poppack.h>
//----------------------------------------------------------------------------------------------------------------------
#endif