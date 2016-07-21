#ifndef __KLOS_WIN32V50_H__
#define __KLOS_WIN32V50_H__

#ifdef _WIN32

#ifdef WINVER
    #undef WINVER
#endif // WINVER

#ifdef _WIN32_WINNT
    #undef _WIN32_WINNT
#endif // _WIN32_WINNT

#define WINVER          0x0500
#define _WIN32_WINNT    0x0500
#define NOMINMAX

#include <atlbase.h>

#ifdef min
#undef min
#endif

#ifdef max
#undef max
#endif

#endif // _WIN32

#endif //__KLOS_WIN32V50_H__