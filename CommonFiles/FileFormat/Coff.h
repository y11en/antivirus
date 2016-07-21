////////////////////////////////////////////////////////////////////
//
//  COFF.H
//  COFF format 
//  General purpose
//  Alexey de Mont de Rique [graf@avp.ru], Kaspersky Labs. 1996
//  Copyright (c) Kaspersky Labs.
//
////////////////////////////////////////////////////////////////////

#ifndef __COFF_H
#define __COFF_H

//#define HANDLE_PRAGMA_PACK_PUSH_POP 1
//Added __attribute__ ((packed)) to all structures

#if !((defined(__WATCOMC__) && (defined(__DOS__)||defined(__OS2__))) || defined(__unix__))
#pragma pack(push,pack_coff)
# define PACKSTRUCT 
#else
# define PACKSTRUCT __attribute ((packed))
#endif
#pragma pack(1)

#define MAX_SYMBOL_NAME 0x200

typedef struct _COFF_Header
{
        WORD    Machine;
        WORD    N_Sections;
        DWORD   Timestamp;
        DWORD   P_SymbolTable;
        DWORD   N_Symbols;
        WORD    OptionHeaderSize;
        WORD    Characteristics;
} PACKSTRUCT COFF_Header;

typedef struct _COFF_SectionHeader
{
        union{
                char Name[8];
                struct{
                        DWORD NameWord;
                        DWORD NameOffset;
#ifdef __unix__
                }Cs;
#else
                };
#endif
        };
        DWORD   PhysicalAddress;
        DWORD   VirtualAddress;
        DWORD   RawDataSize;
        DWORD   P_RawData;
        DWORD   P_RelocationTable;
        DWORD   P_LineNumbers;
        WORD    N_Relocations;
        WORD    N_LineNumbers;
        DWORD   Flags;
} PACKSTRUCT COFF_SectionHeader;

#define COFF_SF_COMMUNAL         0x00001000
#define COFF_SF_NOPAD            0x00000008
#define COFF_SF_REMOVE           0x00000800
#define COFF_SF_DISCARDABLE      0x02000000

#define COFF_SF_INFO             0x00000200

#define IMAGE_SCN_LNK_OTHER                  0x00000100  // Reserved.
#define IMAGE_SCN_LNK_INFO                   0x00000200  // Section contains comments or some other type of information.
//      IMAGE_SCN_TYPE_OVER                  0x00000400  // Reserved.
#define IMAGE_SCN_LNK_REMOVE                 0x00000800  // Section contents will not become part of image.
#define IMAGE_SCN_LNK_COMDAT                 0x00001000  // Section contents comdat.

#define COFF_SF_CODE             0x00000020 // Section contains code.
#define COFF_SF_INIT_DATA        0x00000040 // Section contains initialized data.
#define COFF_SF_NONINIT_DATA     0x00000080 // Section contains uninitialized data.

#define COFF_SF_READ             0x40000000
#define COFF_SF_WRITE            0x80000000
#define COFF_SF_EXECUTE          0x20000000

#define COFF_SF_ALIGN_MASK       0x00F00000

typedef struct _COFF_Relocation
{
        DWORD   Offset;
        DWORD   SymbolIndex;
        WORD    Type;
} PACKSTRUCT COFF_Relocation;

typedef struct _COFF_Symbol
{
        union{
                char Name[8];
                struct{
                        DWORD NameWord;
                        DWORD NameOffset;
#ifdef __unix__
                }Cs;
#else
                };
#endif
        };
        DWORD   Value;          //For fixupp
        WORD    Section;        //No of section
        WORD    Access;         // 0x20-Function
        BYTE    Type;           // 0x02-always
        BYTE    Skip;           // size
} PACKSTRUCT COFF_Symbol;

typedef struct _COFF_SectionData
{
        DWORD   Length;
        WORD    N_Relocations;
        WORD    N_LineNumbers;
        DWORD   CheckSum;
        WORD    PickData;
        DWORD   Selection;
} PACKSTRUCT COFF_SectionData;

#pragma pack()
#if !((defined(__WATCOMC__) && (defined(__DOS__)||defined(__OS2__))) || defined(__unix__))
#pragma pack(pop,pack_coff)
#endif

#endif//__COFF_H
