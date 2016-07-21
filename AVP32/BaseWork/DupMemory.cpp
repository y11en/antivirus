#include "StdAfx.h"
#if defined (_WIN32)

#include <dupmem/dupmem.h>

void* DupAlloc(unsigned size, BOOL static_, BOOL* allocated_by_new)
{
	if(allocated_by_new)
		*allocated_by_new=true;
	return new char[size];
}

void  DupFree(void* ptr)
{
	delete ptr;
}

#endif //_WIN32
