/*
    ct_typs.h

    A Cryptographic Toolkit types.
    Version 2.1

    Last changes: 20.11.96 15:04

    Copyright (c) LAN Crypto. All Rights Reserved.
*/

#ifndef __CT_TYPES__
#define __CT_TYPES__

typedef short int       CT_INT;
typedef unsigned short  CT_UINT;
typedef long            CT_LONG;
typedef unsigned long   CT_ULONG;
typedef unsigned char   CT_UCHAR;
typedef char            CT_CHAR;
typedef unsigned char * CT_PTR;

#define CT_NULL         ((CT_PTR)0)

#if defined( unix ) || defined(__unix__) ||defined( __TANDEM ) || defined( VAX )
 #define CTAPIPREFIX
 #define CTAPI
 #define CTCALLBACK
#elif defined( _WINSYS )
 #define CTAPIPREFIX
 #define CTAPI                           __cdecl
 #define CTCALLBACK                      __cdecl
#elif defined( _WIN32 )
 #define CTAPIPREFIX         __declspec( dllexport )
 #define CTAPI
 #define CTCALLBACK
#elif defined( _WINDOWS ) || defined( _Windows )
 #define CTAPIPREFIX
 #define CTAPI               _far _pascal _export
 #define CTCALLBACK          _far _pascal
#elif defined( __OS2__ )
 #define CTAPIPREFIX
 //#define CTAPI               _pascal _export
 //#define CTCALLBACK          _pascal
 #define CTAPI               __cdecl
 #define CTCALLBACK          __cdecl
#elif defined(__DOS__)
 #define CTAPIPREFIX
 #define CTAPI               __cdecl
 #define CTCALLBACK          __cdecl
#else
#define CTAPIPREFIX
#define CTAPI               _far _pascal
#define CTCALLBACK          _far _pascal
#endif
typedef CT_INT (CTCALLBACK*  CTBACKPROC)( CT_INT );

#define CT_FNULL        ((CTBACKPROC)0)

#endif

