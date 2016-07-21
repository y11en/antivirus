////////////////////////////////////////////////////////////////////
//
//  SIZES.H
//  Page size definition
//  AVP 3.0 Engine
//  Alexey de Mont de Rique [graf@avp.ru], Kaspersky Labs. 1996
//  Copyright (c) Kaspersky Labs.
//
////////////////////////////////////////////////////////////////////

#ifndef __SIZES_H
#define __SIZES_H

#define HEADER_SIZE 0x0400
#define PAGE_A_SIZE 0x0400
#define PAGE_B_SIZE 0x0400
#define PAGE_E_SIZE 0x0800
#define PAGE_C_SIZE 0x8000

#define PAGE_COUNT 5

#define HEADER_BEGIN    0
#define HEADER_END              (HEADER_BEGIN+HEADER_SIZE)
#define PAGE_A_BEGIN    HEADER_END
#define PAGE_A_END              (PAGE_A_BEGIN+PAGE_A_SIZE)
#define PAGE_B_BEGIN    PAGE_A_END
#define PAGE_B_END              (PAGE_B_BEGIN+PAGE_B_SIZE)
#define PAGE_E_BEGIN    PAGE_B_END
#define PAGE_E_END              (PAGE_E_BEGIN+PAGE_E_SIZE)
#define PAGE_C_BEGIN    PAGE_E_END
#define PAGE_C_END              (PAGE_C_BEGIN+PAGE_C_SIZE)

#define NAME_SIZE 0x200

#endif//__SIZES_H
