////////////////////////////////////////////////////////////////////
//
//  VER_ITOA.H
//  Useful macros
//  General purpose
//  Alexey de Mont de Rique [graf@avp.ru], Kaspersky Labs. 1999
//  Copyright (c) Kaspersky Labs.
//
////////////////////////////////////////////////////////////////////

#ifndef __VER_ITOA_H
#define __VER_ITOA_H

// Works only with digit numbers. Any calculations in parameter is not allowed.
#define VER_ITOA2(x) #x
#define VER_ITOA(x)  VER_ITOA2(x)

#endif//__VER_ITOA_H
