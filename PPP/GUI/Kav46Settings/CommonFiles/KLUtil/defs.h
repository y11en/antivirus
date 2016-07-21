#ifndef __KLUTIL_DEFS_H
#define __KLUTIL_DEFS_H

#ifdef BUILD_KLUTIL_DLL
#	define KLUTIL_API __declspec(dllexport)
#else
#	define KLUTIL_API __declspec(dllimport)
#endif

#endif