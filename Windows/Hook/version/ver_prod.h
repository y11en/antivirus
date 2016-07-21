////////////////////////////////////////////////////////////////////
//
//  VER_PRODUCT.H
//  KL product Version definitions
//  Alexey de Mont de Rique [graf@avp.ru], Kaspersky Labs. 2004
//  Copyright (c) Kaspersky Labs.
//
//  This file should be branched and modified for each project.
//
////////////////////////////////////////////////////////////////////

#ifndef __VER_PRODUCT_H
#define __VER_PRODUCT_H

#ifndef VER_PRODUCTNAME_STR
#define VER_PRODUCTNAME_STR                "Kaspersky Anti-Virus"
#endif
#ifndef VER_PRODUCTVERSION_HIGH
#define VER_PRODUCTVERSION_HIGH             6
#endif
#ifndef VER_PRODUCTVERSION_LOW
#define VER_PRODUCTVERSION_LOW              0
#endif
// should be defined externally by release engineer
#ifndef VER_PRODUCTVERSION_BUILD
#define VER_PRODUCTVERSION_BUILD            4
#endif
// should be defined externally by release engineer
#ifndef VER_PRODUCTVERSION_COMPILATION
#define VER_PRODUCTVERSION_COMPILATION     23
#endif

#include "ver_kl.h"

#endif//__VER_PRODUCT_H

