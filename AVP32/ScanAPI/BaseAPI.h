////////////////////////////////////////////////////////////////////
//
//  BASEAPI.H
//  AVP 3.0 Engine: BASEWORK.LIB API
//  AVP project
//  Alexey de Mont de Rique [graf@avp.ru], Kaspersky Labs. 1999
//  Copyright (c) Kaspersky Labs.
//
////////////////////////////////////////////////////////////////////

#ifndef __BASEAPI_H
#define __BASEAPI_H

#include <_AVPIO.h>
#include <ScanAPI/Scanobj.h>

#ifdef __cplusplus
 #ifdef __unix__
  #define decl
 #else
  #define decl __cdecl
 #endif
extern "C" {
#else
#define decl
#endif //__cplusplus

#if (defined (_MSC_VER) || defined(__MWERKS__))
#define prefix(x) decl x
#else //Watcom
 #if ((defined(__WATCOMC__) && (defined(__DOS__)||defined(__OS2__))) || defined(__unix__))
#define prefix(x) decl x
 #else
#define prefix(x) decl _##x
 #endif
#endif

DWORD prefix( BAvpGetVersion)();  // major==HIBYTE(HIWORD(ver)), minor==LOBYTE(HIWORD(ver)), build==LOWORD(ver)
DWORD prefix( BAvpInit)();        // Allocate internal Buffers
DWORD prefix( BAvpDestroy)();
DWORD prefix( BAvpLoadBase)(PCHAR dbname_);
DWORD prefix( BAvpRegisterCallback)(t_AvpCallback* callback_);
DWORD prefix( BAvpProcessObject)(AVPScanObject * obj_);
DWORD prefix( BAvpDispatch)(AVPScanObject * obj_, BYTE SubType_);
PCHAR prefix( BAvpMakeFullName)(PCHAR buf_, PCHAR ptr_);
VOID  prefix( BAvpCancelProcessObject)(LONG code_);
DWORD prefix( BAvpAddMask)(PCHAR buf_);
DWORD prefix( BAvpAddExcludeMask)(PCHAR buf_);
DWORD prefix( BAvpErrorFlush)(); // to call in case of GPF

VOID* prefix( BaseAvpInit)();
DWORD prefix( BaseAvpDestroy)(VOID* base);
DWORD prefix( BaseAvpLoadBase)(VOID* base, PCHAR dbname_);
DWORD prefix( BaseAvpProcessObject)(VOID* base, AVPScanObject * obj_);
PCHAR prefix( BaseAvpMakeFullName)(VOID* base, PCHAR buf_, PCHAR ptr_);
VOID  prefix( BaseAvpCancelProcessObject)(VOID* base, LONG code_);
BOOL  prefix( BaseIsLinkedCode)(VOID* base, PCHAR ptr_);

#ifdef __cplusplus
}
#endif //__cplusplus

#endif //__BASEAPI_H
