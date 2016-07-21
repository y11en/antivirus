/*!
 * (C) 2003 Kaspersky Lab 
 * 
 * \file	osdep.h
 * \author	Mikhail Karmazine
 * \date	28.01.2003 11:33:34
 * \brief	Определения и include'ы, зависящие от платформы
 * 
 */

#ifndef __AK_OSDEP_H__
#define __AK_OSDEP_H__

#ifdef _WIN32
#include <tchar.h>
typedef int   AVP_int32;   // signed 4-byte integer

#endif // _WIN32

#ifdef __unix

#include <cctype>
typedef __int32_t AVP_int32;

#endif // __unix

#ifdef N_PLAT_NLM

#include <ntypes.h>

typedef nint32 AVP_int32;

#endif // N_PLAT_NLM

    /* TODO - in addition to symbols listed above in _WIN32 section,
        define also function

        FILE *_wfsopen( 
           const wchar_t *filename,
           const wchar_t *mode,
           int shflag 
        );
    */

#endif // __AK_OSDEP_H__

// Local Variables:
// mode: C++
// End:
