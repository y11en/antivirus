#ifndef __LOADKAV46SET_DEFS_H
#define __LOADKAV46SET_DEFS_H

#if defined ( BUILD_LOADKAV46SET_DLL )
#	define LOADKAV46SET_API _declspec ( dllexport )
#else
#	define LOADKAV46SET_API _declspec ( dllimport )
#endif

#endif // __LOADKAV46SET_DEFS_H