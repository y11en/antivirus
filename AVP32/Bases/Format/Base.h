////////////////////////////////////////////////////////////////////
//
//  BASE.H
//  AVP 3.0 Engine: Antiviral Database format (begin 10.04.96)
//  AVP project
//  Alexey de Mont de Rique [graf@avp.ru], Kaspersky Labs. 1996
//  Copyright (c) Kaspersky Labs.
//
////////////////////////////////////////////////////////////////////

#ifndef __BASE_H
#define __BASE_H

#if !((defined(__WATCOMC__) && (defined(__DOS__)||defined(__OS2__))) || defined(__unix__))
 #pragma pack(push,pack_base)
#endif
#pragma pack(1)


#define AVP_BASE_VERSION_0x0300

#if !defined (DWORD) && !defined (__unix__)
#       define DWORD unsigned long
#       define WORD  unsigned short
#       define BYTE  unsigned char
#endif

/*
typedef struct _SYSTEMTIME {  // st
    WORD wYear;
    WORD wMonth;
    WORD wDayOfWeek;
    WORD wDay;
    WORD wHour;
    WORD wMinute;
    WORD wSecond;
    WORD wMilliseconds;
} SYSTEMTIME;
*/


#define AVP_MAGIC       0x382E4B45    //"EK",VxD id ->45,4B,2E,38

//this header pos is *0x40.
typedef struct _AVP_BaseHeader
{
        BYTE            Text[0x40];
        BYTE            Authenticity[0x40];    //Authenticity Any data
        DWORD           Magic;
        union{
                WORD    Version;          //version 03,00
                struct{
                BYTE    VerH;             //version 03
                BYTE    VerL;             //subversion 00
#ifdef __unix__
                }BXs;
#else
                };
#endif
        };
        WORD            Flags;     //editable=1
        DWORD           CreatorID;
        DWORD           FileSize;
        DWORD           BlockHeaderTableFO;
        DWORD           BlockHeaderTableSize;
        SYSTEMTIME      GMTModificationTime;
        SYSTEMTIME      ModificationTime;
        WORD            NumberOfSingleBlockTypes;
        WORD            NumberOfRecordBlockTypes;
        WORD            NumberOfRecordTypes;
        DWORD           AuthenticityCRC;
        DWORD           HeaderCRC;

} AVP_BaseHeader;

typedef struct _AVP_BlockHeader
{
       WORD  BlockType;         // BT_*
       WORD  RecordType;   // RT_*
       DWORD BlockFO;
       DWORD CompressedSize;
       DWORD UncompressedSize;
       WORD  Compression;
       WORD  RecordSize;
       DWORD NumRecords;
       DWORD CRC;
       DWORD OptHeaderFO;
       DWORD OptHeaderSize;
} AVP_BlockHeader;


#ifdef AVP_BASE_VERSION_0x0300
#       define NUMBER_OF_SINGLE_BLOCK_TYPES     3
#       define NUMBER_OF_RECORD_BLOCK_TYPES     3
#       define NUMBER_OF_RECORD_TYPES           8
#endif


#define BT_RECORD               0x0000
#define BT_LINK16               0x0001
#define BT_LINK32               0x0002

#define BT_NAME                 0x0100
#define BT_COMMENT              0x0101
#define BT_EDIT                 0x0102

#define RT_KERNEL               0x0000
#define RT_JUMP                 0x0001
#define RT_MEMORY               0x0002
#define RT_SECTOR               0x0003
#define RT_FILE                 0x0004
#define RT_CA                   0x0005
#define RT_UNPACK               0x0006
#define RT_EXTRACT              0x0007

#define RT_SEPARATOR                    0x0100   //Not a record, used in editor

#if !((defined(__WATCOMC__) && (defined(__DOS__)||defined(__OS2__))) || defined(__unix__))
 #pragma pack(pop,pack_base)
#endif
#pragma pack()

#endif//__BASE_H

