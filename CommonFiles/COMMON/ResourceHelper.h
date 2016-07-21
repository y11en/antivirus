//==============================================================================
// PRODUCT: COMMON
//==============================================================================
#ifndef __COMMON_RESOURCE_HELPER_H
#define __COMMON_RESOURCE_HELPER_H

#include "defs.h"

extern HMODULE g_hCommonLocRes;
//------------------------------------------------------------------------------
// 
//------------------------------------------------------------------------------
inline HINSTANCE GetResourceHandleLR ()
{
	MEMORY_BASIC_INFORMATION mbi;
	return VirtualQuery ( GetResourceHandleLR, &mbi, sizeof ( mbi ) ) ? ( HINSTANCE ) mbi.AllocationBase : NULL;
}

COMMONEXPORT HICON LoadIconLR(DWORD dwIconID, DWORD dwWidth);

#endif // __COMMON_RESOURCE_HELPER_H

//==============================================================================
