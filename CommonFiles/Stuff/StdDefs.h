////////////////////////////////////////////////////////////////////
//
//  StdDefs.h
//  Standard definitions
//  General purpose
//  Victor Matiouchenkov [victor@avp.ru], Kaspersky Labs. 1999
//  Copyright (c) Kaspersky Labs
//
////////////////////////////////////////////////////////////////////

#ifndef STDDEFS_H
#define STDDEFS_H

typedef signed char    int8;
typedef signed short   int16;
typedef signed long    int32;

typedef unsigned char  uint8;
typedef unsigned short uint16;
typedef unsigned long  uint32;

typedef unsigned long  ulong;
typedef unsigned int   uint;
typedef unsigned short ushort;
typedef unsigned char  uchar;

//
// Integer byte, word and long word manipulation
//
inline uint16 MkUint16(uint8 lo, uint8 hi) {
  return uint16(lo | (uint16(hi) << 8));
}
inline uint32 MkUint32(uint16 lo, uint16 hi) {
  return lo | (uint32(hi) << 16);
}
inline uint16 LoUint16(uint32 u32) {
  return uint16(u32);
}
inline int16 LoInt16(uint32 u32) {
  return int16(u32);
}
inline uint16 HiUint16(uint32 u32) {
  return uint16(u32 >> 16);
}
inline int16 HiInt16(uint32 u32) {
  return int16(u32 >> 16);
}
inline uint8 LoUint8(uint16 u16) {
  return uint8(u16);
}
inline int8 LoInt8(uint16 u16) {
  return int8(u16);
}
inline uint8 HiUint8(uint16 u16) {
  return uint8(u16 >> 8);
}
inline int8 HiInt8(uint16 u16) {
  return int8(u16 >> 8);
}

typedef enum { defDelete, noDelete, Delete } DelType;


#endif
