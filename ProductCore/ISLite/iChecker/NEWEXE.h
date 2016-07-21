#pragma pack(push, 1)
typedef unsigned short UINT16;
#ifdef _WIN32
typedef unsigned int UINT32;
#else
typedef unsigned long UINT32;
#endif
typedef unsigned char UINT8;

typedef struct
{
 unsigned     dgroupType:2;
 unsigned     globalInit:1;
 unsigned     protectedMode:1;
 unsigned     i86Instructions:1;
 unsigned     i286Instructions:1;
 unsigned     i386Instructions:1;
 unsigned     i87Instructions:1;
 unsigned     applicationType:3;
 unsigned     familyApplication:1;
 unsigned     unused1:1;
 unsigned     linkErrors:1;
 unsigned     unused2:1;
 unsigned     libraryModule:1;
} MODULEBITFLAGS;

typedef union
{
  MODULEBITFLAGS b;
  UINT16   i;
} FLAGWORD;

typedef struct
{
  UINT16    signature;
  UINT8     link_version;       // Version of link used 
  UINT8     link_revision;      // Revision level of link used 
  UINT16    entry_offset;       // Offset to entry table 
  UINT16    entry_size;         // Size of entry table 
  UINT32    crc_32;             // CRC-32 for file load control 
  FLAGWORD  flag_word;          // Control for DOS loading 
  UINT16    auto_data_seg;      // Index to auto data segment 
  UINT16    org_heap_size;      // Original heap allocation size 
  UINT16    org_stack_size;     // Original stack size 
  UINT32    entry_point;        // CS:IP of entry point 
  UINT32    start_stack;        // SS:SP of beginning stack 
  UINT16    segment_count;      // # of entries in Segment Table 
  UINT16    res_table_size;     // Resident Name table size 
  UINT16    non_res_table_size; // Non-Resident name table size 
  UINT16    segment_offset;     // Offset to Segment Table 
  UINT16    resource_offset;    // Offset to Resource Table 
  UINT16    resident_offset;    // Offset to Resident Name Table 
  UINT16    module_offset;      // Offset to Module Table 
  UINT16    import_offset;      // Offset to imported name table 
  UINT32    nonres_name_offset; // Offset to non-res name table  
  UINT16    entry_count;        // Number of movable entry point 
  UINT16    shift_count;        // logical sector alignment 
  UINT16    resource_count;     // number of resources 
  UINT8     ne_exetyp;          // Target operating system 
  UINT8     ne_flagsother;      // Other .EXE flags 
  union
  {
    UINT16 ne_pretthunks;       // offset to return thunks 
    UINT16 ne_gang_start;       // start of gangload area 
  } x;
  union
  { 
    UINT16 ne_psegrefbytes;     // offset to segment ref. bytes 
    UINT16 ne_gang_length;      // length of gangload area      
  } y;
  UINT16 ne_swaparea;           // minimum code swap area size  
  UINT16 ne_expver;             // expected windows version num 
} NEWEXEHEADER;

typedef struct {
  UINT16            sector_offset;      // Offset to logical sector 
  UINT16            segment_length;     // Size in bytes of segment 
  unsigned          segment_type  : 3;  // Segment type identification 
  unsigned          iterated      : 1;  // Segment is iterated      
  unsigned          movable       : 1;  // Segment is movable       
  unsigned          pure          : 1;  // Segment is sharable      
  unsigned          preload       : 1;  // Segment is not demand loaded 
  unsigned          read_only     : 1;  // Segment is read-only     
  unsigned          reloc_info    : 1;  // Segment has reloc info   
  unsigned          debug_info    : 1;  // Segment has debug info   
  unsigned          dpl_bits      : 2;  // 80286 DPL bits       
  unsigned          discard       : 4;  // Segment Discard Priority 
  UINT16            alloc_size;         // Segment allocation size  
} SEGMENT_RECORD;

typedef struct              // Resource table record    
{
  UINT16    type_id;        // Type ID or string offset 
  UINT16    resource_count; // # of resources to this type  
  UINT32    reserved1;      // Reserved DD space        
} RESOURCE_HEADER;

typedef struct
{
 unsigned offset;
 unsigned length;
 unsigned reserved1:4;
 unsigned movable:1;
 unsigned pure:1;
 unsigned preload:1;
 unsigned reserved2:5;
 unsigned discardPriority:4;
 unsigned resourceNumber;
 UINT32  reserved3;
} RESOURCE_ENTRY;

typedef struct          // Basic entry record       
{
 UINT8  eflags;         // Export and Share Flags   
 UINT16 int3f;          // INT 03fh  (0xcd 0x3f)    
 UINT8  seg_number;     // Segment number       
 UINT16 offset;         // Segment offset       
} MOV_ENTRY;

typedef struct          // Basic entry record       
{
 UINT8  eflags;         // Export and Share Flags   
 UINT16 offset;         // Segment offset       
} FIX_ENTRY; 

#pragma pack(pop)
