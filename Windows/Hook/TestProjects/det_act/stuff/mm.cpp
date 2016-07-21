#include "mm.h"
#include "debug.h"
//+ ------------------------------------------------------------------------------------------
CMemMan global_MemMan;
//+ ------------------------------------------------------------------------------------------



CMemMan::CMemMan()
{
	m_AllocCount = 0;
}

CMemMan::~CMemMan()
{
	__alertif(0 != m_AllocCount);
#ifdef _DEBUG
	for (_tvecPtrs::iterator _it = m_ptrs.begin(); _it != m_ptrs.end(); _it++)
		*_it = *_it;
#endif
}

void* CMemMan::Alloc(DWORD size)
{
	void* ptr = HeapAlloc(GetProcessHeap(), 0, size);
	if (0 != ptr)
	{
		InterlockedIncrement(&m_AllocCount);
#ifdef _DEBUG
		m_ptrs.push_back(ptr);
#endif
	}

	return ptr;
}

void CMemMan::Free(void** ptr)
{
	__alertif(0 == ptr);

	if (0 == *ptr)
		return;

#ifdef _DEBUG
	bool bExist = false;
	for (_tvecPtrs::iterator _it = m_ptrs.begin(); _it != m_ptrs.end(); _it++)
	{
		if (*_it == *ptr)
		{
			bExist = true;
			m_ptrs.erase(_it);
			break;
		}
	}
	__alertif(!bExist);
#endif

	InterlockedDecrement(&m_AllocCount);
	HeapFree(GetProcessHeap(), 0, *ptr);

	*ptr = 0;
}

void* CopyBufToNewBlock(void* ptr, int len)
{
	void* newptr = global_Alloc(len);
	if (NULL == newptr)
		return NULL;

	memcpy(newptr, ptr, len);
	
	return newptr;
}

PWCHAR GetUpperStr( PWCHAR pwchStr )
{
	PWCHAR pwchUpper = GetCopyStr( pwchStr );
	if (pwchUpper)
		CharUpper(pwchUpper);
	return pwchUpper;
}

PWCHAR GetLowerStr( PWCHAR pwchStr )
{
	PWCHAR pwchLower = GetCopyStr( pwchStr );
	if (pwchLower)
		CharLower(pwchLower);
	return pwchLower;
}
