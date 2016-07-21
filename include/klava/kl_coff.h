// kl__coff.h
//
// COFF constants and structures
//

#ifndef kl_coff_h_INCLUDED
#define kl_coff_h_INCLUDED

#include <kl_types.h>

////////////////////////////////////////////////////////////////
// Object file format: COFF

#define KL_COFF_SCN_ALIGN_1BYTES               0x00100000  //
#define KL_COFF_SCN_ALIGN_2BYTES               0x00200000  //
#define KL_COFF_SCN_ALIGN_4BYTES               0x00300000  //
#define KL_COFF_SCN_ALIGN_8BYTES               0x00400000  //
#define KL_COFF_SCN_ALIGN_16BYTES              0x00500000  // Default alignment if no others are specified.
#define KL_COFF_SCN_ALIGN_32BYTES              0x00600000  //
#define KL_COFF_SCN_ALIGN_64BYTES              0x00700000  //
#define KL_COFF_SCN_ALIGN_128BYTES             0x00800000  //
#define KL_COFF_SCN_ALIGN_256BYTES             0x00900000  //
#define KL_COFF_SCN_ALIGN_512BYTES             0x00A00000  //
#define KL_COFF_SCN_ALIGN_1024BYTES            0x00B00000  //
#define KL_COFF_SCN_ALIGN_2048BYTES            0x00C00000  //
#define KL_COFF_SCN_ALIGN_4096BYTES            0x00D00000  //
#define KL_COFF_SCN_ALIGN_8192BYTES            0x00E00000  //

#define KL_COFF_SCN_CNT_CODE                   0x00000020  // Section contains code.
#define KL_COFF_SCN_CNT_INITIALIZED_DATA       0x00000040  // Section contains initialized data.
#define KL_COFF_SCN_CNT_UNINITIALIZED_DATA     0x00000080  // Section contains uninitialized data.

#define KL_COFF_SCN_LNK_OTHER                  0x00000100  // Reserved.
#define KL_COFF_SCN_LNK_INFO                   0x00000200  // Section contains comments or some other type of information.
//      KL_COFF_SCN_TYPE_OVER                  0x00000400  // Reserved.
#define KL_COFF_SCN_LNK_REMOVE                 0x00000800  // Section contents will not become part of image.
#define KL_COFF_SCN_LNK_COMDAT                 0x00001000  // Section contents comdat.
//                                           0x00002000  // Reserved.
//      KL_COFF_SCN_MEM_PROTECTED - Obsolete   0x00004000
#define KL_COFF_SCN_NO_DEFER_SPEC_EXC          0x00004000  // Reset speculative exceptions handling bits in the TLB entries for this section.
#define KL_COFF_SCN_GPREL                      0x00008000  // Section content can be accessed relative to GP
#define KL_COFF_SCN_MEM_FARDATA                0x00008000
//      KL_COFF_SCN_MEM_SYSHEAP  - Obsolete    0x00010000
#define KL_COFF_SCN_MEM_PURGEABLE              0x00020000
#define KL_COFF_SCN_MEM_16BIT                  0x00020000
#define KL_COFF_SCN_MEM_LOCKED                 0x00040000
#define KL_COFF_SCN_MEM_PRELOAD                0x00080000


#define KL_COFF_SCN_LNK_NRELOC_OVFL            0x01000000  // Section contains extended relocations.
#define KL_COFF_SCN_MEM_DISCARDABLE            0x02000000  // Section can be discarded.
#define KL_COFF_SCN_MEM_NOT_CACHED             0x04000000  // Section is not cachable.
#define KL_COFF_SCN_MEM_NOT_PAGED              0x08000000  // Section is not pageable.
#define KL_COFF_SCN_MEM_SHARED                 0x10000000  // Section is shareable.
#define KL_COFF_SCN_MEM_EXECUTE                0x20000000  // Section is executable.
#define KL_COFF_SCN_MEM_READ                   0x40000000  // Section is readable.
#define KL_COFF_SCN_MEM_WRITE                  0x80000000  // Section is writeable.

#define KL_COFF_FILE_RELOCS_STRIPPED           0x0001  // Relocation info stripped from file.
#define KL_COFF_FILE_EXECUTABLE_IMAGE          0x0002  // File is executable  (i.e. no unresolved externel references).
#define KL_COFF_FILE_LINE_NUMS_STRIPPED        0x0004  // Line nunbers stripped from file.
#define KL_COFF_FILE_LOCAL_SYMS_STRIPPED       0x0008  // Local symbols stripped from file.
#define KL_COFF_FILE_AGGRESIVE_WS_TRIM         0x0010  // Agressively trim working set
#define KL_COFF_FILE_LARGE_ADDRESS_AWARE       0x0020  // App can handle >2gb addresses
#define KL_COFF_FILE_BYTES_REVERSED_LO         0x0080  // Bytes of machine word are reversed.
#define KL_COFF_FILE_32BIT_MACHINE             0x0100  // 32 bit word machine.
#define KL_COFF_FILE_DEBUG_STRIPPED            0x0200  // Debugging info stripped from file in .DBG file
#define KL_COFF_FILE_REMOVABLE_RUN_FROM_SWAP   0x0400  // If Image is on removable media, copy and run from the swap file.
#define KL_COFF_FILE_NET_RUN_FROM_SWAP         0x0800  // If Image is on Net, copy and run from the swap file.
#define KL_COFF_FILE_SYSTEM                    0x1000  // System File.
#define KL_COFF_FILE_DLL                       0x2000  // File is a DLL.
#define KL_COFF_FILE_UP_SYSTEM_ONLY            0x4000  // File should only be run on a UP machine
#define KL_COFF_FILE_BYTES_REVERSED_HI         0x8000  // Bytes of machine word are reversed.

#define KL_COFF_FILE_MACHINE_UNKNOWN           0
#define KL_COFF_FILE_MACHINE_I386              0x014c  // Intel 386.
#define KL_COFF_FILE_MACHINE_R3000             0x0162  // MIPS little-endian, 0x160 big-endian
#define KL_COFF_FILE_MACHINE_R4000             0x0166  // MIPS little-endian
#define KL_COFF_FILE_MACHINE_R10000            0x0168  // MIPS little-endian
#define KL_COFF_FILE_MACHINE_WCEMIPSV2         0x0169  // MIPS little-endian WCE v2
#define KL_COFF_FILE_MACHINE_ALPHA             0x0184  // Alpha_AXP
#define KL_COFF_FILE_MACHINE_POWERPC           0x01F0  // IBM PowerPC Little-Endian
#define KL_COFF_FILE_MACHINE_SH3               0x01a2  // SH3 little-endian
#define KL_COFF_FILE_MACHINE_SH3E              0x01a4  // SH3E little-endian
#define KL_COFF_FILE_MACHINE_SH4               0x01a6  // SH4 little-endian
#define KL_COFF_FILE_MACHINE_ARM               0x01c0  // ARM Little-Endian
#define KL_COFF_FILE_MACHINE_THUMB             0x01c2
#define KL_COFF_FILE_MACHINE_IA64              0x0200  // Intel 64
#define KL_COFF_FILE_MACHINE_MIPS16            0x0266  // MIPS
#define KL_COFF_FILE_MACHINE_MIPSFPU           0x0366  // MIPS
#define KL_COFF_FILE_MACHINE_MIPSFPU16         0x0466  // MIPS
#define KL_COFF_FILE_MACHINE_ALPHA64           0x0284  // ALPHA64
#define KL_COFF_FILE_MACHINE_AMD64             0x8664  // AMD64 (K8)

#define KL_COFF_REL_I386_ABSOLUTE         0x0000  // Reference is absolute, no relocation is necessary
#define KL_COFF_REL_I386_DIR16            0x0001  // Direct 16-bit reference to the symbols virtual address
#define KL_COFF_REL_I386_REL16            0x0002  // PC-relative 16-bit reference to the symbols virtual address
#define KL_COFF_REL_I386_DIR32            0x0006  // Direct 32-bit reference to the symbols virtual address
#define KL_COFF_REL_I386_DIR32NB          0x0007  // Direct 32-bit reference to the symbols virtual address, base not included
#define KL_COFF_REL_I386_SEG12            0x0009  // Direct 16-bit reference to the segment-selector bits of a 32-bit virtual address
#define KL_COFF_REL_I386_SECTION          0x000A
#define KL_COFF_REL_I386_SECREL           0x000B
#define KL_COFF_REL_I386_REL32            0x0014  // PC-relative 32-bit reference to the symbols virtual address

#define KL_COFF_REL_ARM_ABSOLUTE         0x0000  // Reference is absolute, no relocation is necessary
#define KL_COFF_REL_ARM_ADDR32           0x0001  // Direct 32-bit reference to the symbols virtual address
#define KL_COFF_REL_ARM_ADDR32NB         0x0002  // Direct 32-bit reference to the symbols virtual address, base not included
#define KL_COFF_REL_ARM_BRANCH24         0x0003  // Relative 24-bit reference to the symbol's virtual address
#define KL_COFF_REL_ARM_BRANCH11         0x0004  // Reference to a subroutine call, consisting of two 16-bit instructions with 11-bit offsets
#define KL_COFF_REL_ARM_SECTION          0x000E
#define KL_COFF_REL_ARM_SECREL           0x000F

#define KL_COFF_REL_AMD64_ABSOLUTE        0x0000  // Reference is absolute, no relocation is necessary
#define KL_COFF_REL_AMD64_ADDR64          0x0001  // 64-bit address (VA).
#define KL_COFF_REL_AMD64_ADDR32          0x0002  // 32-bit address (VA).
#define KL_COFF_REL_AMD64_ADDR32NB        0x0003  // 32-bit address w/o image base (RVA).
#define KL_COFF_REL_AMD64_REL32           0x0004  // 32-bit relative address from byte following reloc
#define KL_COFF_REL_AMD64_REL32_1         0x0005  // 32-bit relative address from byte distance 1 from reloc
#define KL_COFF_REL_AMD64_REL32_2         0x0006  // 32-bit relative address from byte distance 2 from reloc
#define KL_COFF_REL_AMD64_REL32_3         0x0007  // 32-bit relative address from byte distance 3 from reloc
#define KL_COFF_REL_AMD64_REL32_4         0x0008  // 32-bit relative address from byte distance 4 from reloc
#define KL_COFF_REL_AMD64_REL32_5         0x0009  // 32-bit relative address from byte distance 5 from reloc
#define KL_COFF_REL_AMD64_SECTION         0x000A  // Section index
#define KL_COFF_REL_AMD64_SECREL          0x000B  // 32 bit offset from base of section containing target
#define KL_COFF_REL_AMD64_SECREL7         0x000C  // 7 bit unsigned offset from base of section containing target
#define KL_COFF_REL_AMD64_TOKEN           0x000D  // 32 bit metadata token
#define KL_COFF_REL_AMD64_SREL32          0x000E  // 32 bit signed span-dependent value emitted into object
#define KL_COFF_REL_AMD64_PAIR            0x000F
#define KL_COFF_REL_AMD64_SSPAN32         0x0010  // 32 bit signed span-dependent value applied at link time


//
// Storage classes.
//
#define KL_COFF_SYM_CLASS_END_OF_FUNCTION     (BYTE )-1
#define KL_COFF_SYM_CLASS_NULL                0x0000
#define KL_COFF_SYM_CLASS_AUTOMATIC           0x0001
#define KL_COFF_SYM_CLASS_EXTERNAL            0x0002
#define KL_COFF_SYM_CLASS_STATIC              0x0003
#define KL_COFF_SYM_CLASS_REGISTER            0x0004
#define KL_COFF_SYM_CLASS_EXTERNAL_DEF        0x0005
#define KL_COFF_SYM_CLASS_LABEL               0x0006
#define KL_COFF_SYM_CLASS_UNDEFINED_LABEL     0x0007
#define KL_COFF_SYM_CLASS_MEMBER_OF_STRUCT    0x0008
#define KL_COFF_SYM_CLASS_ARGUMENT            0x0009
#define KL_COFF_SYM_CLASS_STRUCT_TAG          0x000A
#define KL_COFF_SYM_CLASS_MEMBER_OF_UNION     0x000B
#define KL_COFF_SYM_CLASS_UNION_TAG           0x000C
#define KL_COFF_SYM_CLASS_TYPE_DEFINITION     0x000D
#define KL_COFF_SYM_CLASS_UNDEFINED_STATIC    0x000E
#define KL_COFF_SYM_CLASS_ENUM_TAG            0x000F
#define KL_COFF_SYM_CLASS_MEMBER_OF_ENUM      0x0010
#define KL_COFF_SYM_CLASS_REGISTER_PARAM      0x0011
#define KL_COFF_SYM_CLASS_BIT_FIELD           0x0012

#define KL_COFF_SYM_CLASS_FAR_EXTERNAL        0x0044  //

#define KL_COFF_SYM_CLASS_BLOCK               0x0064
#define KL_COFF_SYM_CLASS_FUNCTION            0x0065
#define KL_COFF_SYM_CLASS_END_OF_STRUCT       0x0066
#define KL_COFF_SYM_CLASS_FILE                0x0067
// new
#define KL_COFF_SYM_CLASS_SECTION             0x0068
#define KL_COFF_SYM_CLASS_WEAK_EXTERNAL       0x0069

#define KL_COFF_SIZEOF_SHORT_NAME              8

#include <kl_pushpack.h>

typedef struct _KL_COFF_SECTION_HEADER {
    uint8_t    Name[KL_COFF_SIZEOF_SHORT_NAME];
    union {
            uint32_t   PhysicalAddress;
            uint32_t   VirtualSize;
    } Misc;
    uint32_t   VirtualAddress;
    uint32_t   SizeOfRawData;
    uint32_t   PointerToRawData;
    uint32_t   PointerToRelocations;
    uint32_t   PointerToLinenumbers;
    uint16_t   NumberOfRelocations;
    uint16_t   NumberOfLinenumbers;
    uint32_t   Characteristics;
} KL_COFF_SECTION_HEADER;

typedef struct _KL_COFF_SYMBOL {
    union {
        uint8_t    ShortName[8];
        struct {
            uint32_t   Short;     // if 0, use LongName
            uint32_t   Long;      // offset into string table
        } Name;
//      uint8_t*   LongName[2];
    } N;
    uint32_t   Value;
    int16_t    SectionNumber;
    uint16_t   Type;
    uint8_t    StorageClass;
    uint8_t    NumberOfAuxSymbols;
} KL_COFF_SYMBOL;

typedef struct _KL_COFF_SECTION_AUX_SYMBOL {
	uint32_t Length;
	uint16_t NumberOfRelocations;
	uint16_t NumberOfLinenumbers;
	uint32_t CheckSum;
	uint16_t Number;
	uint8_t  Selection;
	uint8_t  Unused[3];
} KL_COFF_SECTION_AUX_SYMBOL;

#define KL_COFF_COMDAT_SELECT_NODUPLICATES 1
#define KL_COFF_COMDAT_SELECT_ANY          2
#define KL_COFF_COMDAT_SELECT_SAME_SIZE    3
#define KL_COFF_COMDAT_SELECT_EXACT_MATCH  4
#define KL_COFF_COMDAT_SELECT_ASSOCIATIVE  5
#define KL_COFF_COMDAT_SELECT_LARGEST      6

typedef struct _KL_COFF_RELOCATION {
    union {
        uint32_t   VirtualAddress;
        uint32_t   RelocCount;             // Set to the real count when IMAGE_SCN_LNK_NRELOC_OVFL is set
    };
    uint32_t   SymbolTableIndex;
    uint16_t   Type;
} KL_COFF_RELOCATION;

typedef struct _KL_COFF_FILE_HEADER {
    uint16_t    Machine;
    uint16_t    NumberOfSections;
    uint32_t    TimeDateStamp;
    uint32_t    PointerToSymbolTable;
    uint32_t    NumberOfSymbols;
    uint16_t    SizeOfOptionalHeader;
    uint16_t    Characteristics;
} KL_COFF_FILE_HEADER;

// library member record
typedef struct _KL_COFF_ARCHIVE_MEMBER_HEADER {
	uint8_t      Name[16];
	uint8_t      Date[12];
	uint8_t      UserlD[6];
	uint8_t      GrouplD[6];
	uint8_t      Mode[8];
	uint8_t      Size[10];
	uint8_t      EndHeader[2];
} KL_COFF_ARCHIVE_MEMBER_HEADER;

#include <kl_poppack.h>

#endif // kl_coff_h_INCLUDED

