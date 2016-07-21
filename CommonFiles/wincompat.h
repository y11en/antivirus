#ifndef __WINCOMPAT_H__
#define __WINCOMPAT_H__
#if defined (_WIN32)
#include <windows.h>
#elif defined (__unix__)
#include <unix/libcompat/compat.h>
#endif // _WIN32
#endif // __WINCOMPAT_H__
