////////////////////////////////////////////////////////////////////
//
//  AVPMEM.CPP
//  AVP Engine: AVPMEM.LIB implementation
//  AVP project
//  Alexey de Mont de Rique [graf@avp.ru], Kaspersky Labs. 1999
//  Copyright (c) Kaspersky Labs.
//
////////////////////////////////////////////////////////////////////

#include <ScanAPI/AVPMEM.h>

/*
 * New versions of GCC don't handle "new" redirection well 
 * (actually, they don't handle it at all :()
 */
#if !defined (__unix__)

void * operator new( unsigned int cb )
 {
#if (defined(__WATCOMC__) && defined(__DOS__))
  return _AvpMemAlloc(cb);
#else
  return AvpMemAlloc(cb);
#endif
 }

void operator delete( void* buf )
 {
#if (defined(__WATCOMC__) && defined(__DOS__))
  if(buf)_AvpMemFree(buf);
#else
  if(buf)AvpMemFree(buf);
#endif
 }

#endif /* __unix__ */
