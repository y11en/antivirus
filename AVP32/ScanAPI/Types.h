////////////////////////////////////////////////////////////////////
//
//  TYPES.H
//  SType and Type definition for AVPScanObject
//  AVP 3.0 Engine
//  Alexey de Mont de Rique [graf@avp.ru], Kaspersky Labs. 1996
//  Copyright (c) Kaspersky Labs.
//
////////////////////////////////////////////////////////////////////

#ifndef __TYPES_H
#define __TYPES_H

#define OT_FILE         0x00            // Object type File
//#define OT_D_FILE     0x01
//#define OT_P_FILE     0x02
//#define OT_M_FILE     0x04

#define OT_MEMORY_FILE	0x40

#define OT_SECTOR       0x10
#define OT_MBR          0x11            // Object type MBR Sector
#define OT_BOOT         0x12            // Object type Boot Sector

#define OT_MEMORY       0x20
#define OT_D_MEMORY 0x20
#define OT_W_MEMORY 0x21
#define OT_N_MEMORY 0x22
#define OT_2_MEMORY 0x24

//Internal constants
#define ST_COM  0x01
#define ST_EXE  0x02
#define ST_SYS  0x04
#define ST_NE   0x08
#define ST_OLE2 0x10
#define ST_SCRIPT 0x20
#define ST_HLP    0x40

#define ST_NABOOT       0x81
#define ST_ABOOT        0x80
#define ST_HDBOOT       0x40
#define ST_FDBOOT       0x20
#define ST_MBR          0x10

#endif//__TYPES_H
