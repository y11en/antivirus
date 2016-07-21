// kdc.h
//

#ifndef kdc_h_INCLUDED
#define kdc_h_INCLUDED

#include <kl_types.h>
#include <kl_pushpack.h>

////////////////////////////////////////////////////////////////

// KDC1 file signature 'KDC1'
#define KDC_MAGIC_0 0x4B
#define KDC_MAGIC_1 0x44
#define KDC_MAGIC_2 0x43
#define KDC_MAGIC_3 0x31

// KDC entity ID
typedef uint32_t kdc_eid_t;

// make internal EID from section index and offset
#define KDC_MAKE_INT_EID(SEC,OFFT)  (0x80000000 | (((uint32_t)(SEC) & 0x7F) << 24) | ((uint32_t)(OFFT) & 0xFFFFFF))
#define KDC_MAKE_EXT_EID(ID) ((uint32_t)(ID) & 0x7FFFFFFF)

#define KDC_IS_EID_INT(EID)          (((uint32_t)(EID) & 0x80000000) != 0)
#define KDC_IS_EID_EXT(EID)          (!KDC_IS_EID_INT((EID)))
#define KDC_INT_EID_SECT(EID)        (((uint32_t)(EID) >> 24) & 0x7F)
#define KDC_INT_EID_OFFT(EID)        ((uint32_t)(EID) & 0xFFFFFF)
#define KDC_EXT_EID_ID(EID)          ((uint32_t)(EID) & 0x7FFFFFFF)
#define KDC_INT_EID_ADDR(EID)        ((uint32_t)(EID) & 0x7FFFFFFF)

// KDC fix-up target (type + intra-section address)
typedef uint32_t kdc_fxp_t;

#define KDC_MAKE_FIXUP(TYPE, ADDR) (((uint32_t)(TYPE) << 24) | ((uint32_t)(ADDR) & 0xFFFFFF))
#define KDC_FIXUP_TYPE(FXP)             (((FXP) >> 24) & 0xFF)
#define KDC_FIXUP_ADDR(FXP)             ((FXP) & 0xFFFFFF)

////////////////////////////////////////////////////////////////
// structures

// file header
struct KDC_File_Header
{
	uint8_t  kdc_file_magic [4];     // +0x00 KDC signature/version
	uint8_t  kdc_file_flags;         // +0x04 file flags
	uint8_t  kdc_file_cmethod;       // +0x05 data compression method
	uint8_t  kdc_file_hdrsize;       // +0x06 header size, in 4-byte words
	uint8_t  kdc_file_arch;          // +0x07 architecture ID
	uint32_t kdc_file_release;       // +0x08 database release
	uint32_t kdc_file_version;       // +0x0C cluster version
	uint32_t kdc_file_time;          // +0x10 file time (seconds since 2000-01-01T00:00:00 UTC)
	uint32_t kdc_file_data;          // +0x14 file data offset
	uint32_t kdc_file_csize;         // +0x18 file data size (compressed)
	uint32_t kdc_file_usize;         // +0x1C file data size (uncompressed)
	uint32_t kdc_file_data_crc;      // +0x20 CRC32 of (possibly compressed) file data
	uint32_t kdc_file_hdr_crc;       // +0x24 CRC32 of header (assuming this field contains 0)
} KL_PACKED;	// total size = 0x28

#define KDC_TIMESTAMP_BASE            (((uint32_t)30 * 365 + 7) * 24 * 60 * 60) // what time() had to return at 2000-01-01T00:00:00
#define KDC_TIMESTAMP_FROM_TIME_T(t)  ((uint32_t)((t) - KDC_TIMESTAMP_BASE))
#define KDC_TIMESTAMP_CURRENT()       KDC_TIMESTAMP_FROM_TIME_T(time(0))
#define KDC_timestamp()               KDC_TIMESTAMP_CURRENT() // compatibility

#define KDC_F_BIG_ENDIAN       0x01 // Must agree over all the KDCs in database

#define KDC_COMPRESSION_METHOD_NONE 0
#define KDC_COMPRESSION_METHOD_LZMA 1

#define KDC_FILL_MAGIC(phdr) \
	do \
	{ \
		(phdr)->kdc_file_magic[0] = KDC_MAGIC_0; \
		(phdr)->kdc_file_magic[1] = KDC_MAGIC_1; \
		(phdr)->kdc_file_magic[2] = KDC_MAGIC_2; \
		(phdr)->kdc_file_magic[3] = KDC_MAGIC_3; \
	} while (0)

// NOTE: all offsets in KDC data are given relative to the beginning of KDC_Data_Header

// file data header
struct KDC_Data_Header
{
	uint8_t  kdc_file_hdrsize;        // header size, in 4-byte words
	uint8_t  kdc_file_fmtversion;     // cluster format version
	uint8_t  padding[2];
	uint32_t kdc_data_sectbl;         // section table offset
	uint32_t kdc_data_seccnt;         // section count (section table follows)
	uint32_t kdc_data_exptbl;         // export table offset
	uint32_t kdc_data_expcnt;         // export records count
} KL_PACKED; // total size = 0x10

#define KDC_DATA_VERSION_CURRENT 0x0

// section descriptor record
struct KDC_Section_Rec
{
	uint32_t kdc_section_type;    // section type
	uint32_t kdc_section_frag;    // fragment ID
	uint32_t kdc_section_offt;    // data offset, -1 for sections with no nonzero body and no fixups
	uint32_t kdc_section_size;    // section data size
	uint32_t kdc_section_nfxp;    // number of fixup records
	uint8_t  kdc_section_algn;    // alignment requirement for this section
	uint8_t  kdc_section_no;      // section logical number
	uint16_t kdc_section_flags;   // section flags
	// (fixup records follow section data, aligned to 4)
} KL_PACKED; // total size = 0x18

#define KDC_SECTION_F_BODILESS 0x0001

// macro to get alignment value from kdc_section_algn
#define KDC_SECTION_ALIGNMENT(ALGN) (1U << (ALGN))

// fixup descriptor record
struct KDC_Fixup_Rec
{
	kdc_fxp_t kdc_fixup_tgt; // type and target offset
	kdc_eid_t kdc_fixup_src; // source
} KL_PACKED; // total size = 0x08

// export record
struct KDC_Export_Rec
{
	uint32_t  kdc_exp_id;
	kdc_eid_t kdc_exp_def;
} KL_PACKED; // total size = 0x08

////////////////////////////////////////////////////////////////
// helpers

// alignment macro
#define KDC_ALIGN4(A) (((A) + 3) & ~(3))

// calculate offset to fix-up
#define KDC_FIXUPS_START(SIZE) KDC_ALIGN4(SIZE)

// calculate total size of sections
#define KDC_SECT_TOTAL_SIZE(SIZE,NFIX) (KDC_FIXUPS_START(SIZE) + (NFIX) * sizeof(KDC_Fixup_Rec))

////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////
// KDC fix-up types

#define KLAV_KDC_FIXUP_EVID32 0x00

#define KLAV_MAKE_PATCHOBJ_FIXUP(RELOC_TYPE) (0x80 | ((RELOC_TYPE) & 0x7F))
#define KLAV_KDC_FIXUP_IS_PATCHOBJ(FIXUP_TYPE) (((FIXUP_TYPE) & 0x80) == 0x80)
#define KLAV_KDC_PATCHOBJ_FIXUP_RELOC(FIXUP_TYPE) ((FIXUP_TYPE) & 0x7F)

#define KLAV_MAKE_PATCH32_FIXUP(DIVISOR) (0x20 | (DIVISOR & 0x1F))
#define KLAV_MAKE_PATCH24_FIXUP(DIVISOR) (0x40 | (DIVISOR & 0x1F))
#define KLAV_MAKE_PATCH16_FIXUP(DIVISOR) (0x60 | (DIVISOR & 0x1F))
#define KLAV_KDC_FIXUP_IS_PATCH32(FIXUP_TYPE) (((FIXUP_TYPE) & 0xE0) == 0x20)
#define KLAV_KDC_FIXUP_IS_PATCH24(FIXUP_TYPE) (((FIXUP_TYPE) & 0xE0) == 0x40)
#define KLAV_KDC_FIXUP_IS_PATCH16(FIXUP_TYPE) (((FIXUP_TYPE) & 0xE0) == 0x60)
#define KLAV_KDC_PATCH_FIXUP_DIVISOR(FIXUP_TYPE) ((FIXUP_TYPE) & 0x1F)

#define KLAV_MAKE_PATCH8_FIXUP(DATA) (DATA & 0x1F)
#define KLAV_KDC_FIXUP_IS_PATCH8(FIXUP_TYPE) ((((FIXUP_TYPE) & 0xE0) == 0x00) && (((FIXUP_TYPE) & 0x1F) != 0x00))
#define KLAV_KDC_PATCH8_FIXUP_DATA(FIXUP_TYPE) ((FIXUP_TYPE) & 0x1F)

//////////////////////////////////////////////////////////////////////////
// CDFILE (manifest) cluster list tags

typedef uint32_t KCDF_TAG;

#define KCDF_TAG_INVALID ((KCDF_TAG)0xFFFFFFFF)

#define MAKE_KCDF_TAG(A,B,C,D) \
	((((((((KCDF_TAG)(A)) << 8) | (B)) << 8) | (C)) << 8) | (D))

#define KCDF_TAG_CLUSTER_NAME    MAKE_KCDF_TAG(0, 0, 0, 'N')
#define KCDF_TAG_CLUSTER_FILE    MAKE_KCDF_TAG(0, 0, 0, 'F')
#define KCDF_TAG_CLUSTER_VERSION MAKE_KCDF_TAG(0, 0, 0, 'V')

#define KCDF_TAG_STRING_MAXSIZE 20
#define KCDF_TAG_STRING_FORMAT KDBID_STRING_FORMAT

#include <kl_poppack.h>

#endif // kdc_h_INCLUDED
