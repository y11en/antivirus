#include "StdAfx.h"

#if defined (_WIN32)

#include <dupmem/dupmem.h>


DWORD __InstanceData=NULL;
DWORD DupSetCustomDWord(unsigned index, DWORD data)
{
	DWORD old_data=__InstanceData;
	__InstanceData=data;
	return old_data;
}
DWORD DupGetCustomDWord(unsigned index)
{
	return __InstanceData;
}

#endif // _WIN32

