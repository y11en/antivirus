////////////////////////////////////////////////////////////////////
//
//  RETFLAGS.H
//  Return Flags for AVPScanObject.RFlags
//  AVP 3.0 Engine
//  Alexey de Mont de Rique [graf@avp.ru], Kaspersky Labs. 1996
//  Copyright (c) Kaspersky Labs.
//
////////////////////////////////////////////////////////////////////

#ifndef __RETFLAGS_H
#define __RETFLAGS_H

#define RF_DETECT                               0x00000001
#define RF_CURE                                 0x00000002
#define RF_SUSPIC                               0x00000004
#define RF_WARNING                              0x00000008
#define RF_CURE_FAIL                            0x00000010
#define RF_CORRUPT                              0x00000020
#define RF_INTERNAL_ERROR                       0x00000040
#define RF_NOT_A_PROGRAMM                       0x00000080
#define RF_DELETE                               0x00000100
#define RF_DELETED                              0x00000200
#define RF_ENCRYPTED                            0x00000400
#define RF_PROTECTED                            0x00000800


#define RF_DISK_WRITE_PROTECTED                 0x00001000
#define RF_DISK_NOT_READY                       0x00002000
#define RF_DISK_OUT_OF_SPACE                    0x00004000
#define RF_IO_ERROR                             0x00008000
#define RF_NOT_OPEN                             0x00010000
#define RF_OUT_OF_MEMORY                        0x00020000
#define RF_IN_CURABLE_EMBEDDED                  0x00040000

#define RF_INTERFACE_FAULT                      0x00100000
#define RF_KERNEL_FAULT                         0x00200000
#define RF_NOT_SCANNED                          0x00400000

#define RF_CURE_DELETE                          0x00800000

#define RF_REPORT_DONE                          0x01000000
#define RF_PACKED                               0x02000000
#define RF_ARCHIVE                              0x04000000
#define RF_NONCURABLE                           0x08000000

#define RF_MODIFIED                             0x10000000

#define RF_ERROR (RF_DISK_NOT_READY | RF_DISK_OUT_OF_SPACE | RF_IO_ERROR | RF_OUT_OF_MEMORY )

#endif//__RETFLAGS_H
