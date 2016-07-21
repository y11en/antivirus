// kdu.h
//

#ifndef kdu_h_INCLUDED
#define kdu_h_INCLUDED

#include <kl_types.h>
#include <kl_byteswap.h>

#include <kl_pushpack.h>

////////////////////////////////////////////////////////////////
// KDU block header

typedef struct tagKDU_BLOCK_HEADER_V1
{
	uint8_t  kdu_magic[4];
	uint32_t kdu_bflen;
}
KL_PACKED
KDU_BLOCK_HEADER_V1;

typedef struct tagKDU_BLOCK_HEADER_V2
{
	uint8_t  kdu_magic[4];
	uint8_t  kdu_type;
	uint8_t  kdu_reserved1;
	uint16_t kdu_shlen;
	uint32_t kdu_size;
}
KL_PACKED
KDU_BLOCK_HEADER_V2;

#define KDU_MAGIC_0    0x4B
#define KDU_MAGIC_1    0x44
#define KDU_MAGIC_2    0x55
#define KDU_MAGIC_3_V1 0x31
#define KDU_MAGIC_3_V2 0x32

#define KDU_BLOCK_LIMIT_V1 0x01000000

#define KDU_BLOCK_CHECK_MAGIC(PHDR) \
      ((PHDR)->kdu_magic[0] == KDU_MAGIC_0 \
	&& (PHDR)->kdu_magic[1] == KDU_MAGIC_1 \
	&& (PHDR)->kdu_magic[2] == KDU_MAGIC_2 \
	&& ((PHDR)->kdu_magic[3] == KDU_MAGIC_3_V1 \
	 || (PHDR)->kdu_magic[3] == KDU_MAGIC_3_V2))

#define KDU_BLOCK_MAGIC_IS_V1(PHDR) \
	((PHDR)->kdu_magic[3] == KDU_MAGIC_3_V1)

#define KDU_BLOCK_MAGIC_IS_V2(PHDR) \
	((PHDR)->kdu_magic[3] == KDU_MAGIC_3_V2)

#define KDU_BLOCK_SET_MAGIC_V1(PHDR) do { \
		(PHDR)->kdu_magic[0] = KDU_MAGIC_0; \
		(PHDR)->kdu_magic[1] = KDU_MAGIC_1; \
		(PHDR)->kdu_magic[2] = KDU_MAGIC_2; \
		(PHDR)->kdu_magic[3] = KDU_MAGIC_3_V1; \
	} while (0)

#define KDU_BLOCK_SET_MAGIC_V2(PHDR) do { \
		(PHDR)->kdu_magic[0] = KDU_MAGIC_0; \
		(PHDR)->kdu_magic[1] = KDU_MAGIC_1; \
		(PHDR)->kdu_magic[2] = KDU_MAGIC_2; \
		(PHDR)->kdu_magic[3] = KDU_MAGIC_3_V2; \
	} while (0)

#define KDU_BLOCK_BTF_V1(PHDR) (*((uint8_t *)&((PHDR)->kdu_bflen)))
#define KDU_BLOCK_TYPE_V1(PHDR)  ((KDU_BLOCK_BTF_V1(PHDR) >> 3) & 0x7)
#define KDU_BLOCK_SHLEN_V1(PHDR) ((KDU_BLOCK_BTF_V1(PHDR)) & 0x7)
#define KDU_BLOCK_IS_SHLEN_EXTENDED(PHDR) (KDU_BLOCK_SHLEN_V1(PHDR) == 0x7)
#define KDU_BLOCK_SIZE_V1(PHDR)  ((kl_bswap_32_be((PHDR)->kdu_bflen)) & 0x00FFFFFF)

#define KDU_BLOCK_SET_INFO_V1(PHDR,TYPE,SHLEN,SIZE) do { \
		const uint32_t KDU_BLOCK_SET_INFO_bflen = (((((TYPE) << 3) | (SHLEN)) << 24) | (SIZE)); \
		(PHDR)->kdu_bflen = kl_bswap_32_be(KDU_BLOCK_SET_INFO_bflen); \
	} while (0)

#define KDU_BLOCK_TYPE_V2(PHDR)  ((PHDR)->kdu_type)
#define KDU_BLOCK_SHLEN_V2(PHDR) (kl_bswap_16_be((PHDR)->kdu_shlen))
#define KDU_BLOCK_SIZE_V2(PHDR)  (kl_bswap_32_be((PHDR)->kdu_size))

#define KDU_BLOCK_SET_INFO_V2(PHDR,TYPE,SHLEN,SIZE) do { \
		(PHDR)->kdu_type = (uint8_t) (TYPE); \
		(PHDR)->kdu_reserved1 = 0; \
		(PHDR)->kdu_shlen = kl_bswap_16_be (SHLEN); \
		(PHDR)->kdu_size  = kl_bswap_32_be (SIZE); \
	} while (0)

#define KDU_BLOCK_SET_INFO_EX_V1(PHDR,TYPE,SHLEN_BYTES,SIZE,NEED_EXTRA_SHLEN,PAD_BYTES) do { \
		uint32_t KDU_BLOCK_SET_INFO_EX_shlen = (SHLEN_BYTES); \
		uint32_t const KDU_BLOCK_SET_INFO_EX_q = KDU_BLOCK_SET_INFO_EX_shlen / sizeof (uint32_t); \
		uint32_t const KDU_BLOCK_SET_INFO_EX_r = KDU_BLOCK_SET_INFO_EX_shlen % sizeof (uint32_t); \
		if (KDU_BLOCK_SET_INFO_EX_r || KDU_BLOCK_SET_INFO_EX_q >= 0x07) \
		{ \
			KDU_BLOCK_SET_INFO_EX_shlen = 0x07; \
			(NEED_EXTRA_SHLEN) = 1; \
			(PAD_BYTES) = KDU_BLOCK_SET_INFO_EX_r ? sizeof (uint32_t) - KDU_BLOCK_SET_INFO_EX_r : 0; \
		} else \
		{ \
			KDU_BLOCK_SET_INFO_EX_shlen = KDU_BLOCK_SET_INFO_EX_q; \
			(NEED_EXTRA_SHLEN) = 0; \
			(PAD_BYTES) = 0; \
		} \
		uint32_t const KDU_BLOCK_SET_INFO_EX_bflen = (((((TYPE) << 3) | KDU_BLOCK_SET_INFO_EX_shlen) << 24) | (SIZE)); \
		(PHDR)->kdu_bflen = kl_bswap_32_be(KDU_BLOCK_SET_INFO_EX_bflen); \
	} while (0)

#define KDU_BLOCK_ALIGN(N)   (((N) + (sizeof (uint32_t) - 1)) & ~(sizeof (uint32_t) - 1))

////////////////////////////////////////////////////////////////
// KDU block sub-headers

enum
{
	KDU_BLOCK_TYPE_LINK   = 0,
	KDU_BLOCK_TYPE_PATCH  = 1,
	KDU_BLOCK_TYPE_DBFILE = 2,
	KDU_BLOCK_TYPE_MAP    = 3,
	KDU_BLOCK_TYPE_CDFILE = 4,
	KDU_BLOCK_TYPE_PET    = 5,
};

// sub-header: link

typedef struct tagKDU_SUB_HEADER_LINK
{
	uint32_t kdu_vendor_id;
	uint32_t kdu_block_id;
	uint32_t kdu_arch_id;
	uint32_t kdu_group_id;
	uint32_t kdu_block_seq;
}
KL_PACKED
KDU_SUB_HEADER_LINK;

typedef struct tagKDU_SUB_HEADER_LINK_V2
{
	uint32_t kdu_vendor_id;
	uint32_t kdu_block_id; // Compatibility ID; intended to be zero
	uint32_t kdu_arch_id;
	uint32_t kdu_group_id;
	uint32_t kdu_block_seq;
	uint8_t kdu_block_long_id[16];
}
KL_PACKED
KDU_SUB_HEADER_LINK_V2;

// For version 3, subheader is a tagged attributed section body

/*
typedef struct tagKDU_SUB_HEADER_LINK_V3
{
	uint32_t kdu_block_id;
	uint32_t kdu_block_seq;
	uint16_t kdu_block_cluster_name_len; // cluster name follows this structure
	uint16_t padding0;
}
KL_PACKED
KDU_SUB_HEADER_LINK_V3;
*/

#define KDU_SUBHDR_LINK_SIZE (sizeof (KDU_SUB_HEADER_LINK))

#define KDU_SUBHDR_LINK_VENDOR_ID(PSHDR) kl_bswap_32_be((PSHDR)->kdu_vendor_id)
#define KDU_SUBHDR_LINK_BLOCK_ID(PSHDR)  kl_bswap_32_be((PSHDR)->kdu_block_id)
#define KDU_SUBHDR_LINK_ARCH_ID(PSHDR)   kl_bswap_32_be((PSHDR)->kdu_arch_id)
#define KDU_SUBHDR_LINK_GROUP_ID(PSHDR)  kl_bswap_32_be((PSHDR)->kdu_group_id)
#define KDU_SUBHDR_LINK_BLOCK_SEQ(PSHDR) kl_bswap_32_be((PSHDR)->kdu_block_seq)

#define KDU_SUBHDR_LINK_V2_LONG_BLOCK_ID(PSHDR)  ((PSHDR)->kdu_block_long_id)

/*
#define KDU_SUBHDR_LINK_V3_CLUSTER_NAME(PSHDR) ((char const*)(((KDU_SUB_HEADER_LINK_V3 const*)(PSHDR)) + 1))
#define KDU_SUBHDR_LINK_V3_CLUSTER_NAME_LEN(PSHDR) kl_bswap_16_be((PSHDR)->kdu_block_cluster_name_len)
*/

#define KDU_SUBHDR_LINK_FILL(PSHDR,VID,ID,AID,GID,SEQ) do { \
		(PSHDR)->kdu_vendor_id = kl_bswap_32_be((VID)); \
		(PSHDR)->kdu_block_id = kl_bswap_32_be((ID)); \
		(PSHDR)->kdu_arch_id = kl_bswap_32_be((AID)); \
		(PSHDR)->kdu_group_id = kl_bswap_32_be((GID)); \
		(PSHDR)->kdu_block_seq = kl_bswap_32_be((SEQ)); \
	} while (0)

#define KDU_SUBHDR_LINK_V2_FILL(PSHDR,VID,ID,AID,GID,SEQ,LID) do { \
		(PSHDR)->kdu_vendor_id = kl_bswap_32_be((VID)); \
		(PSHDR)->kdu_block_id = kl_bswap_32_be((ID)); \
		(PSHDR)->kdu_arch_id = kl_bswap_32_be((AID)); \
		(PSHDR)->kdu_group_id = kl_bswap_32_be((GID)); \
		(PSHDR)->kdu_block_seq = kl_bswap_32_be((SEQ)); \
		memcpy(&((PSHDR)->kdu_block_long_id), (LID), sizeof (PSHDR)->kdu_block_long_id); \
	} while (0)

/*
#define KDU_SUBHDR_LINK_V3_FILL(PSHDR,ID,SEQ,CLUSTER_NAME_LEN) do { \
		(PSHDR)->kdu_block_id = kl_bswap_32_be((ID)); \
		(PSHDR)->kdu_block_seq = kl_bswap_32_be((SEQ)); \
		(PSHDR)->kdu_block_cluster_name_len = kl_bswap_16_be((CLUSTER_NAME_LEN)); \
	} while (0)
*/

// sub-header: patch

typedef struct tagKDU_SUB_HEADER_PATCH
{
	uint32_t kdu_patch_seq;
}
KDU_SUB_HEADER_PATCH;

#define KDU_SUBHDR_PATCH_SIZE (sizeof (KDU_SUB_HEADER_PATCH))

#define KDU_SUBHDR_PATCH_SEQ(PSHDR) kl_bswap_32_be((PSHDR)->kdu_patch_seq)

#define KDU_SUBHDR_PATCH_FILL(PSHDR,SEQ) do { \
		(PSHDR)->kdu_patch_seq = kl_bswap_32_be((SEQ)); \
	} while (0)

// sub-header: dbfile

typedef struct tagKDU_SUB_HEADER_DBFILE
{
	uint32_t kdu_dbfile_seq;
}
KDU_SUB_HEADER_DBFILE;

#define KDU_SUBHDR_DBFILE_SIZE (sizeof (KDU_SUB_HEADER_DBFILE))

#define KDU_SUBHDR_DBFILE_SEQ(PSHDR) kl_bswap_32_be((PSHDR)->kdu_dbfile_seq)

#define KDU_SUBHDR_DBFILE_FILL(PSHDR,SEQ) do { \
		(PSHDR)->kdu_dbfile_seq = kl_bswap_32_be((SEQ)); \
	} while (0)

// new version (with incarnation ID)

typedef struct tagKDU_SUB_HEADER_DBFILE_V2
{
	uint32_t kdu_dbfile_seq;
	uint32_t kdu_dbfile_inc;
}
KDU_SUB_HEADER_DBFILE_V2;

#define KDU_SUBHDR_DBFILE_V2_SIZE (sizeof (KDU_SUB_HEADER_DBFILE_V2))

#define KDU_SUBHDR_DBFILE_V2_SEQ(PSHDR) kl_bswap_32_be((PSHDR)->kdu_dbfile_seq)
#define KDU_SUBHDR_DBFILE_V2_INC(PSHDR) kl_bswap_32_be((PSHDR)->kdu_dbfile_inc)

#define KDU_SUBHDR_DBFILE_V2_FILL(PSHDR,SEQ,INC) do { \
		(PSHDR)->kdu_dbfile_seq = kl_bswap_32_be((SEQ)); \
		(PSHDR)->kdu_dbfile_inc = kl_bswap_32_be((INC)); \
	} while (0)

// sub-header: map

typedef struct tagKDU_SUB_HEADER_MAP
{
	uint32_t kdu_map_seq;
	uint32_t kdu_map_inc;
}
KDU_SUB_HEADER_MAP;

#define KDU_SUBHDR_MAP_SIZE (sizeof (KDU_SUB_HEADER_MAP))

#define KDU_SUBHDR_MAP_SEQ(PSHDR) kl_bswap_32_be((PSHDR)->kdu_map_seq)
#define KDU_SUBHDR_MAP_INC(PSHDR) kl_bswap_32_be((PSHDR)->kdu_map_inc)

#define KDU_SUBHDR_MAP_FILL(PSHDR,SEQ,INC) do { \
		(PSHDR)->kdu_map_seq = kl_bswap_32_be((SEQ)); \
		(PSHDR)->kdu_map_inc = kl_bswap_32_be((INC)); \
	} while (0)

// sub-header: persistent entity table

typedef struct tagKDU_SUB_HEADER_PET
{
	uint32_t kdu_pet_release;
	uint32_t kdu_pet_seq;
}
KDU_SUB_HEADER_PET;

#define KDU_SUBHDR_PET_RELEASE(PSHDR) kl_bswap_32_be((PSHDR)->kdu_pet_release)
#define KDU_SUBHDR_PET_SEQ(PSHDR) kl_bswap_32_be((PSHDR)->kdu_pet_seq)

#define KDU_SUBHDR_PET_FILL(PSHDR,RELEASE,SEQ) do { \
		(PSHDR)->kdu_pet_release = kl_bswap_32_be((RELEASE)); \
		(PSHDR)->kdu_pet_seq = kl_bswap_32_be((SEQ)); \
	} while (0)

// clustered database manifest

/*
typedef struct tagKDU_SUB_HEADER_CDFILE
{
	uint32_t kdu_cdfile_release;
	uint32_t kdu_cdfile_seq;
}
KDU_SUB_HEADER_CDFILE;

#define KDU_SUBHDR_CDFILE_RELEASE(PSHDR) kl_bswap_32_be((PSHDR)->kdu_cdfile_release)
#define KDU_SUBHDR_CDFILE_SEQ(PSHDR) kl_bswap_32_be((PSHDR)->kdu_cdfile_seq)

#define KDU_SUBHDR_CDFILE_FILL(PSHDR,RELEASE,SEQ) do { \
		(PSHDR)->kdu_cdfile_release = kl_bswap_32_be((RELEASE)); \
		(PSHDR)->kdu_cdfile_seq = kl_bswap_32_be((SEQ)); \
	} while (0)
*/

////////////////////////////////////////////////////////////////
// KDU section header

typedef struct tagKDU_SECTION_HEADER
{
	uint32_t kdu_sflen;
}
KL_PACKED
KDU_SECTION_HEADER;

#define KDU_SECTION_LIMIT_V1 0x01000000

#define KDU_SECT_STF(PHDR) (*((uint8_t *)&((PHDR)->kdu_sflen)))

// macro to test for V2 header
#define KDU_SECT_IS_V2(PHDR) ((KDU_SECT_STF(PHDR) & 0x40) != 0)

#define KDU_SECT_RAW_V1(PHDR)   ((KDU_SECT_STF(PHDR) >> 7) & 0x1)
#define KDU_SECT_TYPE_V1(PHDR)  ((KDU_SECT_STF(PHDR)) & 0x7)
#define KDU_SECT_FRAG_V1(PHDR)  ((KDU_SECT_STF(PHDR) >> 3) & 0x7)
#define KDU_SECT_SIZE_V1(PHDR)  ((kl_bswap_32_be((PHDR)->kdu_sflen)) & 0x00FFFFFF)

// flag: section has raw format RAW
#define KDU_SECT_RAW_V2(PHDR)   ((KDU_SECT_STF(PHDR) >> 7) & 0x1)
// flag: section has non-zero fragmentID
#define KDU_SECT_HAS_FRAG_V2(PHDR) ((KDU_SECT_STF(PHDR) & 0x20) != 0)
// get section size
#define KDU_SECT_SIZE_V2(PHDR)     ((kl_bswap_32_be((PHDR)->kdu_sflen)) & 0x1FFFFFFF)

#define KDU_SECT_SET_INFO_V1(PHDR,RAW,TYPE,FRAG,SIZE) do { \
		const uint32_t KDU_SECT_SET_INFO_sflen = (((((((RAW) << 4) | (FRAG)) << 3) | (TYPE)) << 24) | (SIZE)); \
		(PHDR)->kdu_sflen = kl_bswap_32_be (KDU_SECT_SET_INFO_sflen); \
	} while (0)

#define KDU_SECT_SET_INFO_V2(PHDR,RAW,FRAG,SIZE) do { \
		const uint32_t KDU_SECT_SET_INFO_sflen = (((((RAW) << 2) | 0x02 | (FRAG)) << 29) | (SIZE)); \
		(PHDR)->kdu_sflen = kl_bswap_32_be (KDU_SECT_SET_INFO_sflen); \
	} while (0)

#define KDU_SECT_ALIGN(N) (((N) + 3) & ~3)

enum
{
	KDU_SECT_TYPE_UNKNOWN   = 0,
	KDU_SECT_TYPE_LB        = 2,
	KDU_SECT_TYPE_MANIFEST  = 3,
};

enum
{
	KDU_SECT_FRAG_UNKNOWN = 0,
	KDU_SECT_FRAG_DEFAULT = 1,
};

////////////////////////////////////////////////////////////////
// KDU attribute header

#define KDU_ATF_NB(ATF)   ((((ATF) >> 4) & 0x3) + 1)
#define KDU_ATF_TYPE(ATF) ((ATF) & 0xF)

#define KDU_ATTR_F_SOR   0x80
#define KDU_ATTR_F_EXB   0x40

#define KDU_ATF_FLAGS(ATF) ((ATF) & (KDU_ATTR_F_SOR|KDU_ATTR_F_EXB))

#define KDU_ATTR_TYPE_IS_VECTOR(TYPE) ((TYPE) & 0x08)
#define KDU_ATTR_TYPE_VECTOR(TYPE) ((TYPE) | 0x08)

#define KDU_MAKE_ATF(TYPE,NB,FLAGS) \
	((FLAGS) | (((NB) - 1) << 4) | (TYPE))

////////////////////////////////////////////////////////////////
// known attribute types

enum {
	// generic types
	KDU_ATTR_TYPE_UINT = 1,
	KDU_ATTR_TYPE_INT  = 1, // synonym
	KDU_ATTR_TYPE_ID   = 2,
	KDU_ATTR_TYPE_SECT = 3,

	KDU_ATTR_TYPE_BINARY  = KDU_ATTR_TYPE_VECTOR(0),
	KDU_ATTR_TYPE_STRING  = KDU_ATTR_TYPE_VECTOR(1),
	KDU_ATTR_TYPE_KDU     = KDU_ATTR_TYPE_VECTOR(2),
	KDU_ATTR_TYPE_WORDS   = KDU_ATTR_TYPE_VECTOR(3),

	// Link KDU: (resource sections)
	KDU_ATTR_TYPE_RS_UINT = 1, 	// == KDU_ATTR_TYPE_UINT
	KDU_ATTR_TYPE_RS_ID   = 2,  // == KDU_ATTR_TYPE_ID
	// Link KDU: (link table section)
	KDU_ATTR_TYPE_LS_ID     = 2,  // == KDU_ATTR_TYPE_ID
	KDU_ATTR_TYPE_LS_SRCID  = 3,
	KDU_ATTR_TYPE_LS_OFF    = 4,

	KDU_ATTR_TYPE_REL  = 4,
	KDU_ATTR_TYPE_RELA = 5,

	// Patch KDU: (FlatFileMerger)
	KDU_ATTR_TYPE_PFF_SET = 1,
	KDU_ATTR_TYPE_PFF_CLR = 2,
	KDU_ATTR_TYPE_PFF_MOV = 3,
	KDU_ATTR_TYPE_PFF_RSZ = 4,

	// Patch KDU: (FlatFileBacktracingInfoMerger)
	KDU_ATTR_TYPE_PFB_ASSOC = 1,
	KDU_ATTR_TYPE_PFB_DEASSOC = 2,

	// Patch KDU: (BlockBacktracingInfoMerger)
	KDU_ATTR_TYPE_PBB_ATTACH = 1,
	KDU_ATTR_TYPE_PBB_DETACH = 2,

	// Cluster patch KDU:
	KDU_ATTR_TYPE_CP_ADDZ = 1,
	KDU_ATTR_TYPE_CP_DEL  = 2,
	KDU_ATTR_TYPE_CP_SETX = 3,
	KDU_ATTR_TYPE_CP_ADDF = 4,
	KDU_ATTR_TYPE_CP_DELF = 5,
	KDU_ATTR_TYPE_CP_ADD  = KDU_ATTR_TYPE_VECTOR(0),
	KDU_ATTR_TYPE_CP_SET  = KDU_ATTR_TYPE_VECTOR(1),
};

////////////////////////////////////////////////////////////////
// KPM patch merger instruction byte

// get patch merger type code
#define KDU_KPM_TYPE(B) ((B) & 0x0F)

// get patch merger flags
#define KDU_KPM_FLAGS(B) ((B) & 0xF0)

// flag: fragment is raw-aligned
#define KDU_KPM_F_RAW_ALIGN  0x80
// flag: fragment requires virtual memory upon loading
#define KDU_KPM_F_VIRTUAL    0x40

enum
{
	KDU_KPM_TYPE_TEXT      = 0,   // text files
	KDU_KPM_TYPE_LBCFG     = 1,   // LB configuration
	KDU_KPM_TYPE_WORDS     = 2,   // array of 32-bit words (LE)
	KDU_KPM_TYPE_RDATA     = 3,   // raw data (byte array)
	KDU_KPM_TYPE_OBJCODE   = 4,   // object code
	KDU_KPM_TYPE_SIGTREE   = 5,   // signature tree
	KDU_KPM_TYPE_SIGPOS    = 6,   // positional signatures
	KDU_KPM_TYPE_FFBKTRC   = 7,   // flat file area backtracing info
	KDU_KPM_TYPE_BLKBKTRC  = 8,   // block backtracing info
	KDU_KPM_TYPE_SCHEMA    = 9,   // data with schema (some mix of raw bytes and LE words)
	KDU_KPM_TYPE_SCHEMADEF = 10,  // data schema
	KDU_KPM_TYPE_NTREE     = 11,  // flat file containing tree building base
	KDU_KPM_TYPE_NTREE_ID  = 12,  // flat file containing id mapping
	KDU_KPM_TYPE_NTREE_ATTR= 13,  // flat file containing tree signature post-checks
};

////////////////////////////////////////////////////////////////
// Public name types

enum
{
	KDU_PUBNAME_TYPE_OBJ_CODE = 0x00,  // object code address (code)
	KDU_PUBNAME_TYPE_OBJ_DATA = 0x01,  // object code address (r/o data)

	KDU_PUBNAME_TYPE_PROC     = 0x10,  // procedure ID
	KDU_PUBNAME_TYPE_VAR      = 0x11,  // variable ID
	KDU_PUBNAME_TYPE_EVENT    = 0x12,  // secondary event
	KDU_PUBNAME_TYPE_VERDICT  = 0x13,  // verdict
	KDU_PUBNAME_TYPE_ENTITY   = 0x14,  // entity

	KDU_PUBNAME_TYPE_GENERIC  = 0x1F,  // generic

	KDU_PUBNAME_TYPE_AVC_FILE = 0x20,  // AVC file ID
};

////////////////////////////////////////////////////////////////
// KDU link V3 subheader tags

typedef uint32_t KDU_TAG;

#define KDU_TAG_INVALID ((KDU_TAG)0xFFFFFFFF)

#define MAKE_KDU_TAG(A,B,C,D) \
	((((((((KDU_TAG)(A)) << 8) | (B)) << 8) | (C)) << 8) | (D))

#define KDU_LINK_TAG_BLOCK_ID        MAKE_KDU_TAG(0, 0, 0, 'B')
#define KDU_LINK_TAG_BLOCK_ID2       MAKE_KDU_TAG(0, 0, 0, 'I')
#define KDU_LINK_TAG_BLOCK_VERSION   MAKE_KDU_TAG(0, 0, 0, 'S')
#define KDU_LINK_TAG_CLUSTER_NAME    MAKE_KDU_TAG(0, 0, 0, 'C')
#define KDU_LINK_TAG_ARCH_ID         MAKE_KDU_TAG(0, 0, 0, 'A')
#define KDU_LINK_TAG_VENDOR_ID       MAKE_KDU_TAG('V', 'n', 'I', 'd')
#define KDU_LINK_TAG_GROUP_ID        MAKE_KDU_TAG('G', 'r', 'I', 'd')
#define KDU_LINK_TAG_BLOCK_NAME      MAKE_KDU_TAG('B', 'l', 'k', 'N')

#define KDU_CDFILE_TAG_RELEASE           MAKE_KDU_TAG('D', 'R', 'e', 'l')
#define KDU_CDFILE_TAG_VERSION           MAKE_KDU_TAG('D', 'V', 'e', 'r')
#define KDU_CDFILE_TAG_TIMESTAMP         MAKE_KDU_TAG('T', 'i', 'm', 'e')
#define KDU_CDFILE_TAG_VERDICT_COUNT     MAKE_KDU_TAG('V', 'd', 'c', 'N')

#define KDU_TAG_STRING_MAXSIZE 20
#define KDU_TAG_STRING_FORMAT KDBID_STRING_FORMAT

////////////////////////////////////////////////////////////////
// KDU's most general raw data fixups

#define KDU_RAWPATCH_NONE     0x00
#define KDU_RAWPATCH_8        0x01
#define KDU_RAWPATCH_8_ADD    0x02
#define KDU_RAWPATCH_16       0x03
#define KDU_RAWPATCH_16_ADD   0x04
#define KDU_RAWPATCH_24       0x05
#define KDU_RAWPATCH_24_ADD   0x06
#define KDU_RAWPATCH_32       0x07
#define KDU_RAWPATCH_32_ADD   0x08
#define KDU_RAWPATCH_INVALID  0xFF

#include <kl_poppack.h>

#endif // kdu_h_INCLUDED

