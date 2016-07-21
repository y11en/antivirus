////////////////////////////////////////////////////////////////////
//
//  AVPMEM.H
//  AVP Engine: AVPMEM.LIB API
//  AVP project
//  Alexey de Mont de Rique [graf@avp.ru], Kaspersky Labs. 1999
//  Copyright (c) Kaspersky Labs.
//
////////////////////////////////////////////////////////////////////

#ifndef __AVPMEM_H
#define __AVPMEM_H

#if (defined(__WATCOMC__) && defined(__DOS__))
extern "C" void* _AvpMemAlloc( unsigned int cb );
#else
extern "C" void* AvpMemAlloc( unsigned int cb );
#endif

#if (defined(__WATCOMC__) && defined(__DOS__))
extern "C" void  _AvpMemFree( void* buf);
#else
extern "C" void  AvpMemFree( void* buf);
#endif

#endif//__AVPMEM_H
