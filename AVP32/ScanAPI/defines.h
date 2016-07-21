////////////////////////////////////////////////////////////////////
//
//  DEFINES.H
//  AVP 3.0 Engine: BASEWORK.LIB API
//  AVP project
//  Alexey de Mont de Rique [graf@avp.ru], Kaspersky Labs. 2003
//  Copyright (c) Kaspersky Labs.
//
////////////////////////////////////////////////////////////////////

#ifndef __DEFINES_H
#define __DEFINES_H

#define AVP_MAX_PATH 0x200


#define strncpyz(dest,src,size)\
(strncpy(dest,src,size), ((char*)dest)[size-1]=0)


#endif //__DEFINES_H
