//==============================================================================
// PRODUCT: COMMON
//==============================================================================
#ifndef __COMMON_DEFS_H
#define __COMMON_DEFS_H

#if defined ( BUILD_COMMON_DLL )
#define COMMONEXPORT _declspec ( dllexport )
#else
#define COMMONEXPORT _declspec ( dllimport )
#endif

typedef unsigned char    _byte;
typedef unsigned short   _word;
typedef unsigned long    _dword;

typedef const char * _PCSTR;
typedef char * _PSTR;

// This macro evaluates to the number of elements in an array.
#define ARRAY_SIZE(Array) ( sizeof ( Array ) / sizeof ( ( Array ) [ 0 ] ) )

// This macro evaluates to TRUE if val is between lo and hi inclusive.
#define INRANGE(lo, val, hi) ( ( ( lo ) <= ( val ) ) && ( ( val ) <= ( hi ) ) )

extern COMMONEXPORT HINSTANCE g_hInstanceCommon;

#define PACKVERSION(major,minor) MAKELONG(minor,major)

#endif // __COMMON_DEFS_H

//==============================================================================
