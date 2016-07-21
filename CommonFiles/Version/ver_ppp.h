////////////////////////////////////////////////////////////////////
//
//  VER_PPP.H
//  KL product Version definitions
//  Project Kaspersky AntiVirus PPP 5.0
//  Alexey de Mont de Rique [graf@avp.ru], Kaspersky Labs. 2003
//  Copyright (c) Kaspersky Labs.
//
////////////////////////////////////////////////////////////////////

#ifndef __VER_PPP_H
#define __VER_PPP_H


#define VER_PRODUCTNAME_STR                "Kaspersky Anti-Virus"
#define VER_PRODUCTVERSION_HIGH             6
#define VER_PRODUCTVERSION_LOW              0

// should be defined externally by release engineer 
#ifndef VER_PRODUCTVERSION_BUILD
#define VER_PRODUCTVERSION_BUILD            1
#endif

#ifndef VER_PRODUCTVERSION_COMPILATION
#define VER_PRODUCTVERSION_COMPILATION      8
#endif

#include "ver_kl.h"

#endif//__VER_PPP_H

