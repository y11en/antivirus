#ifndef __memorymanagment__
#define __memorymanagment__

#include <windows.h>

#include <vector>
typedef std::vector<void*> _tvecPtrs;

class CMemMan
{
public:
	CMemMan();
	~CMemMan();
	void* Alloc(DWORD size);
	void Free(void** ptr);
	void Free(unsigned short** ptr) { Free((void**)ptr); }

private:
	LONG m_AllocCount;
	
	_tvecPtrs m_ptrs;
};


void* CopyBufToNewBlock(void* ptr, int len);
PWCHAR GetUpperStr( PWCHAR pwchStr );
PWCHAR GetLowerStr( PWCHAR pwchStr );
#define GetCopyStr( pwchStr ) ( (PWCHAR) CopyBufToNewBlock(pwchStr, _WSTR_LEN_B(pwchStr)) )
#define FreeStr( pwchStr ) global_Free(&pwchStr)
//+ ------------------------------------------------------------------------------------------

extern CMemMan global_MemMan;

#define global_Free(_ptr)		global_MemMan.Free(/*(void**)*/ _ptr)
#define global_Alloc(_size)		global_MemMan.Alloc(_size)

#define _WSTR_CONST_LEN_B(_str)			(sizeof(_str) - sizeof(WCHAR))
#define _WSTR_CONST_LEN_WC(_str)		((sizeof(_str) - sizeof(WCHAR)) / sizeof(WCHAR))
#define _WSTR_LEN_B(_wstr)				((lstrlen(_wstr) + 1) * sizeof(WCHAR))

//+ ------------------------------------------------------------------------------------------

class _autoFree
{
public:
	_autoFree(void** ptr)
	{
		m_ptr = ptr;
	}
	_autoFree(unsigned short** ptr)
	{
		m_ptr = (void**) ptr;
	}
	~_autoFree()
	{
		global_Free(m_ptr);
	}

private:
	void** m_ptr;
};
#endif // __memorymanagment__