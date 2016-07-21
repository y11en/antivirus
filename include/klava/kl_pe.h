// kl_pe.h
//
// PE format structures
//

#ifndef kl_pe_h_INCLUDED
#define kl_pe_h_INCLUDED

/*-------------------------------------------------------------------------*/
#include <kl_platform.h>
#include <kl_pushpack.h>

#ifndef STATIC_ASSERT
# define STATIC_ASSERT(X)
# define NULL_STATIC_ASSERT
#endif

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus


#define KL_PE_DOS_SIGNATURE                 0x5A4D      // MZ
#define KL_PE_NT_SIGNATURE                  0x00004550  // PE00

typedef struct tagKL_PE_DOS_HEADER { // DOS EXE header
    uint16_t   e_magic;                 // Magic number
    uint16_t   e_cblp;                  // Bytes on last page of file
    uint16_t   e_cp;                    // Pages in file
    uint16_t   e_crlc;                  // Relocations
    uint16_t   e_cparhdr;               // Size of header in paragraphs
    uint16_t   e_minalloc;              // Minimum extra paragraphs needed
    uint16_t   e_maxalloc;              // Maximum extra paragraphs needed
    uint16_t   e_ss;                    // Initial (relative) SS value
    uint16_t   e_sp;                    // Initial SP value
    uint16_t   e_csum;                  // Checksum
    uint16_t   e_ip;                    // Initial IP value
    uint16_t   e_cs;                    // Initial (relative) CS value
    uint16_t   e_lfarlc;                // File address of relocation table
    uint16_t   e_ovno;                  // Overlay number
    uint16_t   e_res[4];                // Reserved words
    uint16_t   e_oemid;                 // OEM identifier (for e_oeminfo)
    uint16_t   e_oeminfo;               // OEM information; e_oemid specific
    uint16_t   e_res2[10];              // Reserved words
    uint32_t   e_lfanew;                // File address of new exe header
} KL_PACKED KL_PE_DOS_HEADER;
STATIC_ASSERT(sizeof(KL_PE_DOS_HEADER) == 2+2+2+2+2+2+2+2+2+2+2+2+2+2+2*4+2+2+2*10+4)

typedef struct tagKL_PE_FILE_HEADER {
    uint16_t    Machine;
    uint16_t    NumberOfSections;
    uint32_t    TimeDateStamp;
    uint32_t    PointerToSymbolTable;
    uint32_t    NumberOfSymbols;
    uint16_t    SizeOfOptionalHeader;
    uint16_t    Characteristics;
} KL_PACKED KL_PE_FILE_HEADER;
STATIC_ASSERT(sizeof(KL_PE_FILE_HEADER) == 2+2+4+4+4+2+2)

#define KL_PE_SIZEOF_FILE_HEADER    20
STATIC_ASSERT(sizeof(KL_PE_FILE_HEADER) == KL_PE_SIZEOF_FILE_HEADER)

// Mind these extra definitions in dg_types.h:
//#define KL_PE_FILE_AGGRESIVE_WS_TRIM         0x0010  // Agressively trim working set
//#define KL_PE_FILE_LARGE_ADDRESS_AWARE       0x0020  // App can handle >2gb addresses
//#define KL_PE_FILE_REMOVABLE_RUN_FROM_SWAP   0x0400  // If Image is on removable media, copy and run from the swap file.

#define KL_PE_FILE_RELOCS_STRIPPED           0x0001  // Relocation info stripped from file.
#define KL_PE_FILE_EXECUTABLE_IMAGE          0x0002  // File is executable  (i.e. no unresolved externel references).
#define KL_PE_FILE_LINE_NUMS_STRIPPED        0x0004  // Line nunbers stripped from file.
#define KL_PE_FILE_LOCAL_SYMS_STRIPPED       0x0008  // Local symbols stripped from file.
#define KL_PE_FILE_MINIMAL_OBJECT            0x0010  // Reserved.
#define KL_PE_FILE_UPDATE_OBJECT             0x0020  // Reserved.
#define KL_PE_FILE_16BIT_MACHINE             0x0040  // 16 bit word machine.
#define KL_PE_FILE_BYTES_REVERSED_LO         0x0080  // Bytes of machine word are reversed.
#define KL_PE_FILE_32BIT_MACHINE             0x0100  // 32 bit word machine.
#define KL_PE_FILE_DEBUG_STRIPPED            0x0200  // Debugging info stripped from file in .DBG file
#define KL_PE_FILE_PATCH                     0x0400  // Reserved.
#define KL_PE_FILE_NET_RUN_FROM_SWAP         0x0800  // If Image is on Net, copy and run from the swap file.
#define KL_PE_FILE_SYSTEM                    0x1000  // System File.
#define KL_PE_FILE_DLL                       0x2000  // File is a DLL.
#define KL_PE_FILE_UP_SYSTEM_ONLY            0x4000  // File should only be run on a UP machine
#define KL_PE_FILE_BYTES_REVERSED_HI         0x8000  // Bytes of machine word are reversed.


#define KL_PE_FILE_MACHINE_UNKNOWN           0
#define KL_PE_FILE_MACHINE_I386              0x014c  // Intel 386 or better
#define KL_PE_FILE_MACHINE_I486              0x014d  // Intel 486 or better
#define KL_PE_FILE_MACHINE_PENTIUM           0x014e  // Intel Pentium or better
#define KL_PE_FILE_MACHINE_R3000             0x0162  // MIPS little-endian, 0x160 big-endian
#define KL_PE_FILE_MACHINE_R4000             0x0166  // MIPS little-endian
#define KL_PE_FILE_MACHINE_R10000            0x0168  // MIPS little-endian
#define KL_PE_FILE_MACHINE_WCEMIPSV2         0x0169  // MIPS little-endian WCE v2
#define KL_PE_FILE_MACHINE_ALPHA             0x0184  // Alpha_AXP
#define KL_PE_FILE_MACHINE_POWERPC           0x01F0  // IBM PowerPC Little-Endian
#define KL_PE_FILE_MACHINE_SH3               0x01a2  // SH3 little-endian
#define KL_PE_FILE_MACHINE_SH3E              0x01a4  // SH3E little-endian
#define KL_PE_FILE_MACHINE_SH4               0x01a6  // SH4 little-endian
#define KL_PE_FILE_MACHINE_ARM               0x01c0  // ARM Little-Endian
#define KL_PE_FILE_MACHINE_THUMB             0x01c2
#define KL_PE_FILE_MACHINE_IA64              0x0200  // Intel 64
#define KL_PE_FILE_MACHINE_MIPS16            0x0266  // MIPS
#define KL_PE_FILE_MACHINE_MIPSFPU           0x0366  // MIPS
#define KL_PE_FILE_MACHINE_MIPSFPU16         0x0466  // MIPS
#define KL_PE_FILE_MACHINE_ALPHA64           0x0284  // ALPHA64
#define KL_PE_FILE_MACHINE_AXP64             KL_PE_FILE_MACHINE_ALPHA64

// Directory format.

typedef struct tagKL_PE_DATA_DIRECTORY {
    uint32_t   VirtualAddress;
    uint32_t   Size;
} KL_PACKED KL_PE_DATA_DIRECTORY;
STATIC_ASSERT(sizeof(KL_PE_DATA_DIRECTORY) == 4+4)

#define KL_PE_NUMBEROF_DIRECTORY_ENTRIES    16

// Optional header format.

typedef struct tagKL_PE_OPTIONAL_HEADER {

    // Standard fields.

    uint16_t    Magic;
    uint8_t     MajorLinkerVersion;
    uint8_t     MinorLinkerVersion;
    uint32_t    SizeOfCode;
    uint32_t    SizeOfInitializedData;
    uint32_t    SizeOfUninitializedData;
    uint32_t    AddressOfEntryPoint;
    uint32_t    BaseOfCode;
    uint32_t    BaseOfData;

    // NT additional fields.

    uint32_t    ImageBase;
    uint32_t    SectionAlignment;
    uint32_t    FileAlignment;
    uint16_t    MajorOperatingSystemVersion;
    uint16_t    MinorOperatingSystemVersion;
    uint16_t    MajorImageVersion;
    uint16_t    MinorImageVersion;
    uint16_t    MajorSubsystemVersion;
    uint16_t    MinorSubsystemVersion;
    uint32_t    Reserved1;
    uint32_t    SizeOfImage;
    uint32_t    SizeOfHeaders;
    uint32_t    CheckSum;
    uint16_t    Subsystem;
    uint16_t    DllCharacteristics;
    uint32_t    SizeOfStackReserve;
    uint32_t    SizeOfStackCommit;
    uint32_t    SizeOfHeapReserve;
    uint32_t    SizeOfHeapCommit;
    uint32_t    LoaderFlags;
    uint32_t    NumberOfRvaAndSizes;
    KL_PE_DATA_DIRECTORY DataDirectory[KL_PE_NUMBEROF_DIRECTORY_ENTRIES];
} KL_PACKED KL_PE_OPTIONAL_HEADER;

STATIC_ASSERT(sizeof(KL_PE_OPTIONAL_HEADER) ==
    2+1+1+4+4+4+4+4+4+
    4+4+4+2+2+2+2+2+2+4+4+4+4+2+2+4+4+4+4+4+4+
    sizeof(KL_PE_DATA_DIRECTORY) * KL_PE_NUMBEROF_DIRECTORY_ENTRIES)

#define KL_PE_SIZEOF_STD_OPTIONAL_HEADER      28
STATIC_ASSERT(offsetof(KL_PE_OPTIONAL_HEADER, ImageBase) == KL_PE_SIZEOF_STD_OPTIONAL_HEADER)

#define KL_PE_SIZEOF_NT_OPTIONAL_HEADER      224
STATIC_ASSERT(sizeof(KL_PE_OPTIONAL_HEADER) == KL_PE_SIZEOF_NT_OPTIONAL_HEADER)

#define KL_PE_NT_OPTIONAL_HDR_MAGIC        0x10b

typedef struct KL_PE_NT_HEADERS {
    uint32_t               Signature;
    KL_PE_FILE_HEADER      FileHeader;
    KL_PE_OPTIONAL_HEADER  OptionalHeader;
} KL_PACKED KL_PE_NT_HEADERS;
STATIC_ASSERT(sizeof(KL_PE_NT_HEADERS) == 4 + sizeof(KL_PE_FILE_HEADER) + sizeof(KL_PE_OPTIONAL_HEADER))


// Calculate the first section header

#define KL_PE_FIRST_SECTION( ntheader ) ((KL_PE_SECTION_HEADER *)       \
    ((uintptr_t)ntheader +                                              \
     offsetof(KL_PE_NT_HEADERS, OptionalHeader) +                       \
     ((KL_PE_NT_HEADERS *)(ntheader))->FileHeader.SizeOfOptionalHeader  \
    ))

// Subsystem Values

#define KL_PE_SUBSYSTEM_UNKNOWN              0   // Unknown subsystem.
#define KL_PE_SUBSYSTEM_NATIVE               1   // Image doesn't require a subsystem.
#define KL_PE_SUBSYSTEM_WINDOWS_GUI          2   // Image runs in the Windows GUI subsystem.
#define KL_PE_SUBSYSTEM_WINDOWS_CUI          3   // Image runs in the Windows character subsystem.
#define KL_PE_SUBSYSTEM_OS2_CUI              5   // image runs in the OS/2 character subsystem.
#define KL_PE_SUBSYSTEM_POSIX_CUI            7   // image run  in the Posix character subsystem.

// Dll Characteristics

#define KL_PE_LIBRARY_PROCESS_INIT           1   // Dll has a process initialization routine.
#define KL_PE_LIBRARY_PROCESS_TERM           2   // Dll has a thread termination routine.
#define KL_PE_LIBRARY_THREAD_INIT            4   // Dll has a thread initialization routine.
#define KL_PE_LIBRARY_THREAD_TERM            8   // Dll has a thread termination routine.

// Loader Flags

#define KL_PE_LOADER_FLAGS_BREAK_ON_LOAD    0x00000001
#define KL_PE_LOADER_FLAGS_DEBUG_ON_LOAD    0x00000002


// Directory Entries

#define KL_PE_DIRECTORY_ENTRY_EXPORT         0   // Export Directory
#define KL_PE_DIRECTORY_ENTRY_IMPORT         1   // Import Directory
#define KL_PE_DIRECTORY_ENTRY_RESOURCE       2   // Resource Directory
#define KL_PE_DIRECTORY_ENTRY_EXCEPTION      3   // Exception Directory
#define KL_PE_DIRECTORY_ENTRY_SECURITY       4   // Security Directory
#define KL_PE_DIRECTORY_ENTRY_BASERELOC      5   // Base Relocation Table
#define KL_PE_DIRECTORY_ENTRY_DEBUG          6   // Debug Directory
#define KL_PE_DIRECTORY_ENTRY_COPYRIGHT      7   // Description String
#define KL_PE_DIRECTORY_ENTRY_ARCHITECTURE   7 
#define KL_PE_DIRECTORY_ENTRY_GLOBALPTR      8   // Machine Value (MIPS GP)
#define KL_PE_DIRECTORY_ENTRY_TLS            9   // TLS Directory
#define KL_PE_DIRECTORY_ENTRY_LOAD_CONFIG   10   // Load Configuration Directory
#define KL_PE_DIRECTORY_ENTRY_BOUND_IMPORT  11 
#define KL_PE_DIRECTORY_ENTRY_IAT           12

// Section header format.

#define KL_PE_SIZEOF_SHORT_NAME              8

typedef struct tagKL_PE_SECTION_HEADER {
    uint8_t    Name[KL_PE_SIZEOF_SHORT_NAME];
//    union {
//            UNION_MEMBER(uint32_t)         PhysicalAddress_DoNotUse;
//            #define PhysicalAddress     UNION_MEMBER_ACCESS(PhysicalAddress_DoNotUse)
//
//            UNION_MEMBER(uint32_t)         VirtualSize_DoNotUse;
//            #define VirtualSize         UNION_MEMBER_ACCESS(VirtualSize_DoNotUse)
//    } Misc;
    uint32_t   VirtualSize;
    uint32_t   VirtualAddress;
    uint32_t   SizeOfRawData;
    uint32_t   PointerToRawData;
    uint32_t   PointerToRelocations;
    uint32_t   PointerToLinenumbers;
    uint16_t   NumberOfRelocations;
    uint16_t   NumberOfLinenumbers;
    uint32_t   Characteristics;
} KL_PE_SECTION_HEADER;
STATIC_ASSERT(sizeof(KL_PE_SECTION_HEADER) == 1*KL_PE_SIZEOF_SHORT_NAME + 4 +4+4+4+4+4+2+2+4)

#define KL_PE_SIZEOF_SECTION_HEADER          40
STATIC_ASSERT(sizeof(KL_PE_SECTION_HEADER) == KL_PE_SIZEOF_SECTION_HEADER)


#define KL_PE_SCN_TYPE_REGULAR               0x00000000  //
#define KL_PE_SCN_TYPE_DUMMY                 0x00000001  // Reserved.
#define KL_PE_SCN_TYPE_NO_LOAD               0x00000002  // Reserved.
#define KL_PE_SCN_TYPE_GROUPED               0x00000004  // Used for 16-bit offset code.
#define KL_PE_SCN_TYPE_NO_PAD                0x00000008  // Reserved.
#define KL_PE_SCN_TYPE_COPY                  0x00000010  // Reserved.

#define KL_PE_SCN_CNT_CODE                   0x00000020  // Section contains code.
#define KL_PE_SCN_CNT_INITIALIZED_DATA       0x00000040  // Section contains initialized data.
#define KL_PE_SCN_CNT_UNINITIALIZED_DATA     0x00000080  // Section contains uninitialized data.

#define KL_PE_SCN_LNK_OTHER                  0x00000100  // Reserved.
#define KL_PE_SCN_LNK_INFO                   0x00000200  // Section contains comments or some other type of information.
#define KL_PE_SCN_LNK_OVERLAY                0x00000400  // Section contains an overlay.
#define KL_PE_SCN_LNK_REMOVE                 0x00000800  // Section contents will not become part of image.
#define KL_PE_SCN_LNK_COMDAT                 0x00001000  // Section contents comdat.

#define KL_PE_SCN_ALIGN_1BYTES               0x00100000  //
#define KL_PE_SCN_ALIGN_2BYTES               0x00200000  //
#define KL_PE_SCN_ALIGN_4BYTES               0x00300000  //
#define KL_PE_SCN_ALIGN_8BYTES               0x00400000  //
#define KL_PE_SCN_ALIGN_16BYTES              0x00500000  // Default alignment if no others are specified.
#define KL_PE_SCN_ALIGN_32BYTES              0x00600000  //
#define KL_PE_SCN_ALIGN_64BYTES              0x00700000  //

#define KL_PE_SCN_MEM_DISCARDABLE            0x02000000  // Section can be discarded.
#define KL_PE_SCN_MEM_NOT_CACHED             0x04000000  // Section is not cachable.
#define KL_PE_SCN_MEM_NOT_PAGED              0x08000000  // Section is not pageable.
#define KL_PE_SCN_MEM_SHARED                 0x10000000  // Section is shareable.
#define KL_PE_SCN_MEM_EXECUTE                0x20000000  // Section is executable.
#define KL_PE_SCN_MEM_READ                   0x40000000  // Section is readable.
#define KL_PE_SCN_MEM_WRITE                  0x80000000  // Section is writeable.

//Resources
typedef struct tagKL_PE_RESOURCE_DIRECTORY {
    uint32_t   Characteristics;
    uint32_t   TimeDateStamp;
    uint16_t   MajorVersion;
    uint16_t   MinorVersion;
    uint16_t   NumberOfNamedEntries;
    uint16_t   NumberOfIdEntries;
//  KL_PE_RESOURCE_DIRECTORY_ENTRY DirectoryEntries[];
} KL_PACKED KL_PE_RESOURCE_DIRECTORY;
STATIC_ASSERT(sizeof(KL_PE_RESOURCE_DIRECTORY) == 4+4+2+2+2+2)


#define KL_PE_RESOURCE_NAME_IS_STRING        0x80000000
#define KL_PE_RESOURCE_DATA_IS_DIRECTORY     0x80000000

typedef struct tagKL_PE_RESOURCE_DIRECTORY_ENTRY {
    uint32_t   Name;
    uint32_t   OffsetToData;
} KL_PACKED KL_PE_RESOURCE_DIRECTORY_ENTRY;

STATIC_ASSERT(sizeof(KL_PE_RESOURCE_DIRECTORY_ENTRY) == 4+4)

typedef struct tagKL_PE_RESOURCE_DIR_STRING_U {
    uint16_t  Length;
    uint16_t  NameString[ 1 ];
} KL_PACKED KL_PE_RESOURCE_DIR_STRING_U;
STATIC_ASSERT(sizeof(KL_PE_RESOURCE_DIR_STRING_U) == 2+2)

typedef struct tagKL_PE_RESOURCE_DATA_ENTRY {
    uint32_t   OffsetToData;
    uint32_t   Size;
    uint32_t   CodePage;
    uint32_t   Reserved;
} KL_PACKED KL_PE_RESOURCE_DATA_ENTRY;
STATIC_ASSERT(sizeof(KL_PE_RESOURCE_DATA_ENTRY) == 4+4+4+4)

typedef struct tagKL_PE_EXPORT_DIRECTORY {
    uint32_t   Characteristics;
    uint32_t   TimeDateStamp;
    uint16_t   MajorVersion;
    uint16_t   MinorVersion;
    uint32_t   Name;
    uint32_t   Base;
    uint32_t   NumberOfFunctions;
    uint32_t   NumberOfNames;
    uint32_t   AddressOfFunctions;     // RVA from base of image
    uint32_t   AddressOfNames;         // RVA from base of image
    uint32_t   AddressOfNameOrdinals;  // RVA from base of image
} KL_PACKED KL_PE_EXPORT_DIRECTORY;
STATIC_ASSERT(sizeof(KL_PE_EXPORT_DIRECTORY) == 4+4+2+2+4+4+4+4+4+4+4)

typedef struct tagKL_PE_BASE_RELOCATION {
	int32_t   VirtualAddress;
	int32_t   SizeOfBlock;
	//  kav_data16   TypeOffset[1];
} KL_PACKED KL_PE_BASE_RELOCATION;
STATIC_ASSERT(sizeof(KL_PE_BASE_RELOCATION) == 4+4)

typedef struct tagKL_PE_IMPORT_DESCRIPTOR {
	//union {
	//uint32_t   Characteristics;            // 0 for terminating null import descriptor
	uint32_t   OriginalFirstThunk;         // RVA to original unbound IAT (PKL_PE_THUNK_DATA)
	//};
	uint32_t   TimeDateStamp;                  // 0 if not bound,
	// -1 if bound, and real date\time stamp
	//     in KL_PE_DIRECTORY_ENTRY_BOUND_IMPORT (new BIND)
	// O.W. date/time stamp of DLL bound to (Old BIND)

	uint32_t   ForwarderChain;                 // -1 if no forwarders
	uint32_t   Name;
	uint32_t   FirstThunk;                     // RVA to IAT (if bound this IAT has actual addresses)
} KL_PACKED KL_PE_IMPORT_DESCRIPTOR;
STATIC_ASSERT(sizeof(KL_PE_IMPORT_DESCRIPTOR) == 4+4+4+4+4)

// standart pe import directory entry
typedef struct tagKL_PE_IMPORT_DIRECTORY_ENTRY {
	int32_t  ImportLookUp;      // +00h       // offset of lookup table.
	int32_t  TimeDateStamp;     // +04h       // junk, usually 0.
	int32_t  ForwardChain;      // +08h       // junk, usually -1.
	int32_t  NameRVA;           // +0Ch       // rva of dll name.
	int32_t  AddressTableRVA;   // +10h       // rva of import address table.
} KL_PACKED KL_PE_IMPORT_DIRECTORY_ENTRY;
STATIC_ASSERT(sizeof(KL_PE_IMPORT_DIRECTORY_ENTRY) == 4+4+4+4+4)

typedef struct tagKL_PE_THUNK_DATA {
    union {
        uint32_t ForwarderString;      // PBYTE 
        uint32_t Function;             // PDWORD
        uint32_t Ordinal;
        uint32_t AddressOfData;        // PKL_PE_IMPORT_BY_NAME
    };
} KL_PACKED KL_PE_THUNK_DATA;

typedef struct tagKL_PE_IMPORT_BY_NAME {
    uint16_t    Hint;
    uint8_t     Name[1];
} KL_PACKED KL_PE_IMPORT_BY_NAME;

#define KL_PE_ORDINAL_FLAG32 0x80000000

#ifdef __cplusplus
}
#endif // __cplusplus

#ifdef NULL_STATIC_ASSERT
# undef STATIC_ASSERT
# undef NULL_STATIC_ASSERT
#endif

#include <kl_poppack.h>

#endif // kl_pe.h


