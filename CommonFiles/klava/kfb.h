// kfb.h
//
//

#ifndef kfb_h_INCLUDED
#define kfb_h_INCLUDED

#include <kl_types.h>

#include <kl_pushpack.h>

////////////////////////////////////////////////////////////////
// Klava Flat Database (KFB) format

// KFBv1 file header
struct KFB1_FILE_HEADER
{
	uint8_t  kfb_magic[4];
	uint8_t  kfb_flags;
	uint8_t  kfb_hdrsize;
	uint8_t  kfb_pageshift;
	uint8_t  kfb_reserved1;
	uint32_t kfb_product_id;
	uint32_t kfb_release;  // aka 'incarnation'
	uint32_t kfb_arch_id;
	uint32_t kfb_version;  // aka 'seqno'
	uint32_t kfb_file_size;
	uint32_t kfb_loadable_size;
	uint32_t kfb_resident_size;
	uint32_t kfb_sectbl_off;
	uint32_t kfb_sectbl_size;
}
KL_PACKED;

// KFBv2 file header
struct KFB2_FILE_HEADER
{
	uint8_t  kfb_magic[4];
	uint8_t  kfb_flags;
	uint8_t  kfb_hdrsize;
	uint8_t  kfb_reserved1;    // former 'kfb_pageshift'
	uint8_t  kfb_secrec_size;  // section table record size, in 32-bit words
	uint32_t kfb_product_id;
	uint32_t kfb_release;      // aka 'incarnation'
	uint32_t kfb_arch_id;
	uint32_t kfb_version;      // aka 'seqno'
	uint32_t kfb_file_size;
	uint32_t kfb_reserved2;    // former 'kfb_loadable_size'
	uint32_t kfb_reserved3;    // former 'kfb_resident_size'
	uint32_t kfb_sectbl_off;
	uint32_t kfb_sectbl_size;
}
KL_PACKED;

// KFBv1 file signature ('KFB1')
#define KFB1_MAGIC_0 0x4B
#define KFB1_MAGIC_1 0x46
#define KFB1_MAGIC_2 0x42
#define KFB1_MAGIC_3 0x31

// KFBv2 file signature ('KFB2')
#define KFB2_MAGIC_0 0x4B
#define KFB2_MAGIC_1 0x46
#define KFB2_MAGIC_2 0x42
#define KFB2_MAGIC_3 0x32

// KFB flags
#define KFB_F_BIG_ENDIAN       0x01
#define KFB_F_USES_COMPRESSION 0x02 // KFBv2 only

// macro to calculate page size from kfb_pageshift
#define KFB_PAGESIZE_VALUE(SHIFT) (1 << (SHIFT))

// compression methods
#define KFB_COMPRESSION_METHOD_NONE 0
#define KFB_COMPRESSION_METHOD_LZMA 1

// KFBv1 section table record
struct KFB1_SECTION_TABLE_RECORD
{
	uint32_t kfb_section_type;
	uint32_t kfb_fragment_id;
	uint32_t kfb_section_off;
	uint32_t kfb_section_size;
}
KL_PACKED;

// KFBv2 section table record
struct KFB2_SECTION_TABLE_RECORD
{
	uint32_t kfb_section_type;      // section type
	uint32_t kfb_fragment_id;       // fragment ID
	uint32_t kfb_section_off;       // file offset
	uint32_t kfb_section_csize;     // in-file (compressed) size
	uint32_t kfb_section_usize;     // in-memory (uncompressed) data size
	uint32_t kfb_section_crc32;     // compressed data CRC32
	uint8_t  kfb_section_reserved1; // reserved field
	uint8_t  kfb_section_alignment; // alignment requirements
	uint8_t  kfb_section_flags;     // section flags
	uint8_t  kfb_section_cmethod;   // compression method
}
KL_PACKED;

// KFB section flags (new format only)
#define KFB_SECT_F_LOADABILITY_MASK 0x0F
#define KFB_SECT_F_NONLOADABLE      0x01
#define KFB_SECT_F_LOADABLE         0x02
#define KFB_SECT_F_RESIDENT         0x03

// flag: section data is compressed
#define KFB_SECT_F_COMPRESSED       0x10
// flag: section must be placed in virtual memory (if possible)
#define KFB_SECT_F_VIRTUAL          0x20
// flag: crc32 is valid
#define KFB_SECT_F_CRC32            0x40

#define KFB_ALIGNMENT_SIZE(N) ((N) == 0 ? 16 : ((uint32_t) 1 << ((N) - 1)))

enum
{
	KFB_ALIGNMENT_DEFAULT = 0,
	KFB_ALIGNMENT_1    = 1,
	KFB_ALIGNMENT_2    = 2,
	KFB_ALIGNMENT_4    = 3,
	KFB_ALIGNMENT_8    = 4,
	KFB_ALIGNMENT_16   = 5,
	KFB_ALIGNMENT_32   = 6,
	KFB_ALIGNMENT_64   = 7,
	KFB_ALIGNMENT_128  = 8,
	KFB_ALIGNMENT_256  = 9,
	KFB_ALIGNMENT_512  = 10,
	KFB_ALIGNMENT_1024 = 11,
	KFB_ALIGNMENT_2048 = 12,
	KFB_ALIGNMENT_4096 = 13,
	KFB_ALIGNMENT_MAX  = KFB_ALIGNMENT_4096
};

// kernel parameter
struct KFB_KERNEL_PARM
{
	uint32_t kfb_kparm_id;
	uint32_t kfb_kparm_value;
}
KL_PACKED;

#include <kl_poppack.h>

#endif // kfb_h_INCLUDED

