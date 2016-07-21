//==============================================================================
// PRODUCT: CCIFACE
//==============================================================================
#ifndef __CCIFACE_DEFS_H
#define __CCIFACE_DEFS_H

#if defined ( BUILD_CCIFACE_DLL )
#define CCIFACEEXPORT _declspec ( dllexport )
#else
#define CCIFACEEXPORT _declspec ( dllimport )
#endif

#endif // __CCIFACE_DEFS_H

//==============================================================================
