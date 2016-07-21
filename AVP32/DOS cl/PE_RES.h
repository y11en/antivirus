// WINUSER.H

#define MAKEINTRESOURCEA(i) (LPSTR)((DWORD)((WORD)(i)))
#define MAKEINTRESOURCEW(i) (LPWSTR)((DWORD)((WORD)(i)))
#ifdef UNICODE
#define MAKEINTRESOURCE  MAKEINTRESOURCEW
#else
#define MAKEINTRESOURCE  MAKEINTRESOURCEA
#endif // !UNICODE

/* Predefined Resource Types */
#define PE_RT_CURSOR           MAKEINTRESOURCE(1)
#define PE_RT_BITMAP           MAKEINTRESOURCE(2)
#define PE_RT_ICON             MAKEINTRESOURCE(3)
#define PE_RT_MENU             MAKEINTRESOURCE(4)
#define PE_RT_DIALOG           MAKEINTRESOURCE(5)
#define PE_RT_STRING           MAKEINTRESOURCE(6)
#define PE_RT_FONTDIR          MAKEINTRESOURCE(7)
#define PE_RT_FONT             MAKEINTRESOURCE(8)
#define PE_RT_ACCELERATOR      MAKEINTRESOURCE(9)
#define PE_RT_RCDATA           MAKEINTRESOURCE(10)
#define PE_RT_MESSAGETABLE     MAKEINTRESOURCE(11)

// WINNT.H
// Directory Entries

#define IMAGE_DIRECTORY_ENTRY_EXPORT        0 // Export Directory
#define IMAGE_DIRECTORY_ENTRY_IMPORT        1 // Import Directory
#define IMAGE_DIRECTORY_ENTRY_RESOURCE      2 // Resource Directory
#define IMAGE_DIRECTORY_ENTRY_EXCEPTION     3 // Exception Directory
#define IMAGE_DIRECTORY_ENTRY_SECURITY      4 // Security Directory
#define IMAGE_DIRECTORY_ENTRY_BASERELOC     5 // Base Relocation Table
#define IMAGE_DIRECTORY_ENTRY_DEBUG         6 // Debug Directory
#define IMAGE_DIRECTORY_ENTRY_COPYRIGHT     7 // Description String
#define IMAGE_DIRECTORY_ENTRY_GLOBALPTR     8 // Machine Value (MIPS GP)
#define IMAGE_DIRECTORY_ENTRY_TLS           9 // TLS Directory
#define IMAGE_DIRECTORY_ENTRY_LOAD_CONFIG  10 // Load Configuration Directory
#define IMAGE_DIRECTORY_ENTRY_BOUND_IMPORT 11 // Bound Import Directory in headers
#define IMAGE_DIRECTORY_ENTRY_IAT          12 // Import Address Table

//
// File header format.
//

typedef struct _IMAGE_FILE_HEADER
 {
  WORD    Machine;
  WORD    NumberOfSections;
  DWORD   TimeDateStamp;
  DWORD   PointerToSymbolTable;
  DWORD   NumberOfSymbols;
  WORD    SizeOfOptionalHeader;
  WORD    Characteristics;
 } IMAGE_FILE_HEADER, *PIMAGE_FILE_HEADER;

#define IMAGE_SIZEOF_FILE_HEADER             20

#define IMAGE_FILE_RELOCS_STRIPPED           0x0001  // Relocation info stripped from file.
#define IMAGE_FILE_EXECUTABLE_IMAGE          0x0002  // File is executable  (i.e. no unresolved externel references).
#define IMAGE_FILE_LINE_NUMS_STRIPPED        0x0004  // Line nunbers stripped from file.
#define IMAGE_FILE_LOCAL_SYMS_STRIPPED       0x0008  // Local symbols stripped from file.
#define IMAGE_FILE_BYTES_REVERSED_LO         0x0080  // Bytes of machine word are reversed.
#define IMAGE_FILE_32BIT_MACHINE             0x0100  // 32 bit word machine.
#define IMAGE_FILE_DEBUG_STRIPPED            0x0200  // Debugging info stripped from file in .DBG file
#define IMAGE_FILE_REMOVABLE_RUN_FROM_SWAP   0x0400  // If Image is on removable media, copy and run from the swap file.
#define IMAGE_FILE_NET_RUN_FROM_SWAP         0x0800  // If Image is on Net, copy and run from the swap file.
#define IMAGE_FILE_SYSTEM                    0x1000  // System File.
#define IMAGE_FILE_DLL                       0x2000  // File is a DLL.
#define IMAGE_FILE_UP_SYSTEM_ONLY            0x4000  // File should only be run on a UP machine
#define IMAGE_FILE_BYTES_REVERSED_HI         0x8000  // Bytes of machine word are reversed.

#define IMAGE_FILE_MACHINE_UNKNOWN           0
#define IMAGE_FILE_MACHINE_I386              0x14c   // Intel 386.
#define IMAGE_FILE_MACHINE_R3000             0x162   // MIPS little-endian, 0x160 big-endian
#define IMAGE_FILE_MACHINE_R4000             0x166   // MIPS little-endian
#define IMAGE_FILE_MACHINE_R10000            0x168   // MIPS little-endian
#define IMAGE_FILE_MACHINE_ALPHA             0x184   // Alpha_AXP
#define IMAGE_FILE_MACHINE_POWERPC           0x1F0   // IBM PowerPC Little-Endian

//
// Directory format.
//

typedef struct _IMAGE_DATA_DIRECTORY
 {
  DWORD   VirtualAddress;
  DWORD   Size;
 } IMAGE_DATA_DIRECTORY, *PIMAGE_DATA_DIRECTORY;

#define IMAGE_NUMBEROF_DIRECTORY_ENTRIES    16

//
// Optional header format.
//

typedef struct _IMAGE_OPTIONAL_HEADER
 {
  // Standard fields.
  WORD    Magic;
  BYTE    MajorLinkerVersion;
  BYTE    MinorLinkerVersion;
  DWORD   SizeOfCode;
  DWORD   SizeOfInitializedData;
  DWORD   SizeOfUninitializedData;
  DWORD   AddressOfEntryPoint;
  DWORD   BaseOfCode;
  DWORD   BaseOfData;

  // NT additional fields.
  DWORD   ImageBase;
  DWORD   SectionAlignment;             //56 object align
  DWORD   FileAlignment;
  WORD    MajorOperatingSystemVersion;  //64
  WORD    MinorOperatingSystemVersion;
  WORD    MajorImageVersion;
  WORD    MinorImageVersion;
  WORD    MajorSubsystemVersion;        //72
  WORD    MinorSubsystemVersion;
  DWORD   Win32VersionValue;
  DWORD   SizeOfImage;                  //80
  DWORD   SizeOfHeaders;
  DWORD   CheckSum;                     //88 file checksum
  WORD    Subsystem;
  WORD    DllCharacteristics;           //flags
  DWORD   SizeOfStackReserve;           //96
  DWORD   SizeOfStackCommit;
  DWORD   SizeOfHeapReserve;            //104
  DWORD   SizeOfHeapCommit;
  DWORD   LoaderFlags;                  //112
  DWORD   NumberOfRvaAndSizes;
  IMAGE_DATA_DIRECTORY DataDirectory[IMAGE_NUMBEROF_DIRECTORY_ENTRIES];
} IMAGE_OPTIONAL_HEADER, *PIMAGE_OPTIONAL_HEADER;

#define IMAGE_SIZEOF_ROM_OPTIONAL_HEADER      56
#define IMAGE_SIZEOF_STD_OPTIONAL_HEADER      28
#define IMAGE_SIZEOF_NT_OPTIONAL_HEADER      224

#define IMAGE_NT_OPTIONAL_HDR_MAGIC        0x10b
#define IMAGE_ROM_OPTIONAL_HDR_MAGIC       0x107

typedef struct _IMAGE_NT_HEADERS
 {
  DWORD Signature;
  IMAGE_FILE_HEADER FileHeader;
  IMAGE_OPTIONAL_HEADER OptionalHeader;
 } IMAGE_NT_HEADERS, *PIMAGE_NT_HEADERS;

//
// Section header format.
//

#define IMAGE_SIZEOF_SHORT_NAME              8

typedef struct _IMAGE_SECTION_HEADER
 {
  BYTE    Name[IMAGE_SIZEOF_SHORT_NAME];
  union
   {
    DWORD   PhysicalAddress;
    DWORD   VirtualSize;
   } Misc;
  DWORD   VirtualAddress;
  DWORD   SizeOfRawData;
  DWORD   PointerToRawData;
  DWORD   PointerToRelocations;
  DWORD   PointerToLinenumbers;
  WORD    NumberOfRelocations;
  WORD    NumberOfLinenumbers;
  DWORD   Characteristics;
} IMAGE_SECTION_HEADER, *PIMAGE_SECTION_HEADER;

#define IMAGE_SIZEOF_SECTION_HEADER          40

//
// Calculate the byte offset of a field in a structure of type type.
//

#define FIELD_OFFSET(type, field)    ((LONG)&(((type *)0)->field))

#define IMAGE_FIRST_SECTION( ntheader ) ((PIMAGE_SECTION_HEADER)        \
    ((DWORD)ntheader +                                                  \
     FIELD_OFFSET( IMAGE_NT_HEADERS, OptionalHeader ) +                 \
     ((PIMAGE_NT_HEADERS)(ntheader))->FileHeader.SizeOfOptionalHeader   \
    ))

#define IMAGE_DOS_SIGNATURE                 0x5A4D      // MZ
#define IMAGE_OS2_SIGNATURE                 0x454E      // NE
#define IMAGE_OS2_SIGNATURE_LE              0x454C      // LE
#define IMAGE_VXD_SIGNATURE                 0x454C      // LE
#define IMAGE_NT_SIGNATURE                  0x00004550  // PE00

typedef struct _IMAGE_DOS_HEADER {      // DOS .EXE header
    WORD   e_magic;                     // Magic number
    WORD   e_cblp;                      // Bytes on last page of file
    WORD   e_cp;                        // Pages in file
    WORD   e_crlc;                      // Relocations
    WORD   e_cparhdr;                   // Size of header in paragraphs
    WORD   e_minalloc;                  // Minimum extra paragraphs needed
    WORD   e_maxalloc;                  // Maximum extra paragraphs needed
    WORD   e_ss;                        // Initial (relative) SS value
    WORD   e_sp;                        // Initial SP value
    WORD   e_csum;                      // Checksum
    WORD   e_ip;                        // Initial IP value
    WORD   e_cs;                        // Initial (relative) CS value
    WORD   e_lfarlc;                    // File address of relocation table
    WORD   e_ovno;                      // Overlay number
    WORD   e_res[4];                    // Reserved words
    WORD   e_oemid;                     // OEM identifier (for e_oeminfo)
    WORD   e_oeminfo;                   // OEM information; e_oemid specific
    WORD   e_res2[10];                  // Reserved words
    LONG   e_lfanew;                    // File address of new exe header
  } IMAGE_DOS_HEADER, *PIMAGE_DOS_HEADER;

// DLL support.

// Export Format

typedef struct _IMAGE_EXPORT_DIRECTORY
 {
  DWORD   Characteristics;
  DWORD   TimeDateStamp;
  WORD    MajorVersion;
  WORD    MinorVersion;
  DWORD   Name;
  DWORD   Base;
  DWORD   NumberOfFunctions;
  DWORD   NumberOfNames;
  PDWORD  *AddressOfFunctions;
  PDWORD  *AddressOfNames;
  PWORD   *AddressOfNameOrdinals;
 } IMAGE_EXPORT_DIRECTORY, *PIMAGE_EXPORT_DIRECTORY;

// Relocation format.

typedef struct _IMAGE_RELOCATION
 {
  union
   {
    DWORD   VirtualAddress;    //page RVA
    DWORD   RelocCount; // Set to the real count when IMAGE_SCN_LNK_NRELOC_OVFL is set
   };
  DWORD   SymbolTableIndex;    //block size
  WORD    TypeOffset[1];
 } IMAGE_RELOCATION;
//typedef IMAGE_RELOCATION UNALIGNED *PIMAGE_RELOCATION;
typedef IMAGE_RELOCATION *PIMAGE_RELOCATION;

#define IMAGE_SIZEOF_RELOCATION         10

// Resource Format.

// Resource directory consists of two counts, following by a variable length
// array of directory entries.  The first count is the number of entries at
// beginning of the array that have actual names associated with each entry.
// The entries are in ascending order, case insensitive strings.  The second
// count is the number of entries that immediately follow the named entries.
// This second count identifies the number of entries that have 16-bit integer
// Ids as their name.  These entries are also sorted in ascending order.
//
// This structure allows fast lookup by either name or number, but for any
// given resource entry only one form of lookup is supported, not both.
// This is consistant with the syntax of the .RC file and the .RES file.

typedef struct _IMAGE_RESOURCE_DIRECTORY
 {
  DWORD   Characteristics;
  DWORD   TimeDateStamp;
  WORD    MajorVersion;
  WORD    MinorVersion;
  WORD    NumberOfNamedEntries;
  WORD    NumberOfIdEntries;
  //  IMAGE_RESOURCE_DIRECTORY_ENTRY DirectoryEntries[];
 } IMAGE_RESOURCE_DIRECTORY, *PIMAGE_RESOURCE_DIRECTORY;

#define IMAGE_RESOURCE_NAME_IS_STRING        0x80000000
#define IMAGE_RESOURCE_DATA_IS_DIRECTORY     0x80000000

// Each directory contains the 32-bit Name of the entry and an offset,
// relative to the beginning of the resource directory of the data associated
// with this directory entry.  If the name of the entry is an actual text
// string instead of an integer Id, then the high order bit of the name field
// is set to one and the low order 31-bits are an offset, relative to the
// beginning of the resource directory of the string, which is of type
// IMAGE_RESOURCE_DIRECTORY_STRING.  Otherwise the high bit is clear and the
// low-order 16-bits are the integer Id that identify this resource directory
// entry. If the directory entry is yet another resource directory (i.e. a
// subdirectory), then the high order bit of the offset field will be
// set to indicate this.  Otherwise the high bit is clear and the offset
// field points to a resource data entry.

typedef struct _IMAGE_RESOURCE_DIRECTORY_ENTRY
 {
  union
   {
    struct
     {
      DWORD NameOffset:31;
      DWORD NameIsString:1;
     };
    DWORD   Name;
    WORD    Id;
   };
  union
   {
    DWORD   OffsetToData;
    struct
     {
      DWORD   OffsetToDirectory:31;
      DWORD   DataIsDirectory:1;
     };
   };
 } IMAGE_RESOURCE_DIRECTORY_ENTRY, *PIMAGE_RESOURCE_DIRECTORY_ENTRY;

// For resource directory entries that have actual string names, the Name
// field of the directory entry points to an object of the following type.
// All of these string objects are stored together after the last resource
// directory entry and before the first resource data object.  This minimizes
// the impact of these variable length objects on the alignment of the fixed
// size directory entry objects.

typedef struct _IMAGE_RESOURCE_DIRECTORY_STRING
 {
  WORD    Length;
  CHAR    NameString[ 1 ];
 } IMAGE_RESOURCE_DIRECTORY_STRING, *PIMAGE_RESOURCE_DIRECTORY_STRING;


typedef struct _IMAGE_RESOURCE_DIR_STRING_U
 {
  WORD    Length;
  WCHAR   NameString[ 1 ];
 } IMAGE_RESOURCE_DIR_STRING_U, *PIMAGE_RESOURCE_DIR_STRING_U;

// Each resource data entry describes a leaf node in the resource directory
// tree.  It contains an offset, relative to the beginning of the resource
// directory of the data for the resource, a size field that gives the number
// of bytes of data at that offset, a CodePage that should be used when
// decoding code point values within the resource data.  Typically for new
// applications the code page would be the unicode code page.

typedef struct _IMAGE_RESOURCE_DATA_ENTRY
 {
  DWORD   OffsetToData;
  DWORD   Size;
  DWORD   CodePage;
  DWORD   Reserved;
 } IMAGE_RESOURCE_DATA_ENTRY, *PIMAGE_RESOURCE_DATA_ENTRY;

// Load Configuration Directory Entry

typedef struct _IMAGE_LOAD_CONFIG_DIRECTORY
 {
  DWORD   Characteristics;
  DWORD   TimeDateStamp;
  WORD    MajorVersion;
  WORD    MinorVersion;
  DWORD   GlobalFlagsClear;
  DWORD   GlobalFlagsSet;
  DWORD   CriticalSectionDefaultTimeout;
  DWORD   DeCommitFreeBlockThreshold;
  DWORD   DeCommitTotalFreeThreshold;
  PVOID   LockPrefixTable;
  DWORD   MaximumAllocationSize;
  DWORD   VirtualMemoryThreshold;
  DWORD   ProcessHeapFlags;
  DWORD   ProcessAffinityMask;
  DWORD   Reserved[ 3 ];
 } IMAGE_LOAD_CONFIG_DIRECTORY, *PIMAGE_LOAD_CONFIG_DIRECTORY;

// Function table entry format for MIPS/ALPHA images.  Function table is
// pointed to by the IMAGE_DIRECTORY_ENTRY_EXCEPTION directory entry.
// This definition duplicates ones in ntmips.h and ntalpha.h for use
// by portable image file mungers.

typedef struct _IMAGE_RUNTIME_FUNCTION_ENTRY
 {
  DWORD BeginAddress;
  DWORD EndAddress;
  PVOID ExceptionHandler;
  PVOID HandlerData;
  DWORD PrologEndAddress;
 } IMAGE_RUNTIME_FUNCTION_ENTRY, *PIMAGE_RUNTIME_FUNCTION_ENTRY;

typedef struct _IMAGE_COFF_SYMBOLS_HEADER
 {
  DWORD   NumberOfSymbols;
  DWORD   LvaToFirstSymbol;
  DWORD   NumberOfLinenumbers;
  DWORD   LvaToFirstLinenumber;
  DWORD   RvaToFirstByteOfCode;
  DWORD   RvaToLastByteOfCode;
  DWORD   RvaToFirstByteOfData;
  DWORD   RvaToLastByteOfData;
 } IMAGE_COFF_SYMBOLS_HEADER, *PIMAGE_COFF_SYMBOLS_HEADER;

#define FRAME_FPO       0
#define FRAME_TRAP      1
#define FRAME_TSS       2
#define FRAME_NONFPO    3

typedef struct _FPO_DATA {
    DWORD       ulOffStart;             // offset 1st byte of function code
    DWORD       cbProcSize;             // # bytes in function
    DWORD       cdwLocals;              // # bytes in locals/4
    WORD        cdwParams;              // # bytes in params/4
    WORD        cbProlog : 8;           // # bytes in prolog
    WORD        cbRegs   : 3;           // # regs saved
    WORD        fHasSEH  : 1;           // TRUE if SEH in func
    WORD        fUseBP   : 1;           // TRUE if EBP has been allocated
    WORD        reserved : 1;           // reserved for future use
    WORD        cbFrame  : 2;           // frame type
} FPO_DATA, *PFPO_DATA;
#define SIZEOF_RFPO_DATA 16


#define IMAGE_DEBUG_MISC_EXENAME    1

typedef struct _IMAGE_DEBUG_MISC
 {
  DWORD       DataType;               // type of misc data, see defines
  DWORD       Length;                 // total length of record, rounded to four
                                      // byte multiple.
  BOOLEAN     Unicode;                // TRUE if data is unicode string
  BYTE        Reserved[ 3 ];
  BYTE        Data[ 1 ];              // Actual data
 } IMAGE_DEBUG_MISC, *PIMAGE_DEBUG_MISC;


// Function table extracted from MIPS/ALPHA images.  Does not contain
// information needed only for runtime support.  Just those fields for
// each entry needed by a debugger.

typedef struct _IMAGE_FUNCTION_ENTRY
 {
  DWORD   StartingAddress;
  DWORD   EndingAddress;
  DWORD   EndOfPrologue;
 } IMAGE_FUNCTION_ENTRY, *PIMAGE_FUNCTION_ENTRY;

//
// End Image Format
//
