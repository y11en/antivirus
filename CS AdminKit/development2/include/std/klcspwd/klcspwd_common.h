/*KLCSAK_PUBLIC_HEADER*/
#ifndef __KLCSPWD_COMMON_H__
#define __KLCSPWD_COMMON_H__

#ifdef KLCS_USE_CSPWDLIB
    #define KLCSPWD_DECL
#else
    #include <std/base/kldefs.h>
    #define KLCSPWD_DECL  KLCSC_DECL
#endif

#endif //__KLCSPWD_COMMON_H__
