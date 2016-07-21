/* Copyright (C) 2001-2003 Ashmanov & Partners company, Moscow, Russia
 * All rigts reserved.
 *
 * PROJECT: PCOMMON
 * NAME:    Language names defintion
 *
 * COMPILER: MS VC++ 6.0, GCC     TARGET: Win32,FreeBSD-x86,Linux-x86
 *  LIB: none
 * XREF: none
 *==========================================================================
 * REVISION:
 * 15.10.01: v 1.00 created by AlexIV
 *==========================================================================
 */
#ifndef __LANGUAGE_H__
#define __LANGUAGE_H__

#define LG_UNKNOWN    0x0000
#define LG_INVALID    0xFFFF
#define LG_BULGARIAN  0x0402
#define LG_CZECH      0x0405
#define LG_DANISH     0x0406
#define LG_GERMAN     0x0407
#define LG_ENGLISH_US 0x0409
#define LG_SPANISH    0x040A
#define LG_FINNISH    0x040B
#define LG_FRENCH     0x040C
#define LG_HUNGARIAN  0x040E
#define LG_ITALIAN    0x0410
#define LG_DUTCH      0x0413
#define LG_POLISH     0x0415
#define LG_RUSSIAN    0x0419
#define LG_SLOVAK     0x041B
#define LG_SWEDISH    0x041D
#define LG_ENGLISH_UK 0x0809
#define LG_PORTUGUESE 0x0816

#define LG_ENGLISH    LG_ENGLISH_US

enum LangCodes
{
  LNG_UNKNOWN = LG_UNKNOWN,
  LNG_INVALID = LG_INVALID,
  LNG_BULGARIAN = LG_BULGARIAN,
  LNG_CZECH = LG_CZECH,
  LNG_DANISH = LG_DANISH,
  LNG_GERMAN = LG_GERMAN,
  LNG_ENGLISH_US = LG_ENGLISH_US,
  LNG_SPANISH = LG_SPANISH,
  LNG_FINNISH = LG_FINNISH,
  LNG_FRENCH = LG_FRENCH,
  LNG_HUNGARIAN = LG_HUNGARIAN,
  LNG_ITALIAN = LG_ITALIAN,
  LNG_DUTCH = LG_DUTCH,
  LNG_POLISH = LG_POLISH,
  LNG_RUSSIAN = LG_RUSSIAN,
  LNG_SLOVAK = LG_SLOVAK,
  LNG_SWEDISH = LG_SWEDISH,
  LNG_ENGLISH_UK = LG_ENGLISH_UK,
  LNG_PORTUGUESE = LG_PORTUGUESE,

  LNG_ENGLISH      = LNG_ENGLISH_US,
  LNG_ALL          = -1
};

#ifdef WITH_NEW_LINGPROC
typedef enum LangCodes language_id;
#else
typedef unsigned short language_id;
#endif


/* Codepages - standard */
#define CP_UNKNOWN    0
#define CP_INVALID    0xFFFF
#define CP_ASCII      20127
#define CP_UNICODE    1200 
#define CP_LATIN2     1250 /*Central Europe*/
#define CP_CYRILLIC   1251
#define CP_LATIN1     1252
#define CP_GREEK      1253
#define CP_TURKISH    1254
#define CP_HEBREW     1255
#define CP_ARABIC     1256
#define CP_BALTIC     1257

/* Codepages - cyrillic nonstandard */
#define CP_CYRILLIC_MAC 10007
#define CP_OEM_RUSSIAN    866
#define CP_KOI8R        20866
#define CP_KOI8U        21866
#define CP_ISO8859_5    28595

/* Codepages - Unicode nonstandard */
#define CP_UTF7         65000
#define CP_UTF8         65001

#endif /*__LANGUAGE_H__*/
