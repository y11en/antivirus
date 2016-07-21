// kdb_ids.h
//
//

#ifndef kdb_ids_h_INCLUDED
#define kdb_ids_h_INCLUDED

#include <kl_types.h>

////////////////////////////////////////////////////////////////
// KDBID - database fragment ID

typedef uint32_t KDBID;

#define KDBID_INVALID ((KDBID)0xFFFFFFFF)

#define MAKE_KDBID(A,B,C,D) \
	((((((((KDBID)(A)) << 8) | (B)) << 8) | (C)) << 8) | (D))

#define KDBID_A(ID) (((ID) >> 24) & 0xFF)
#define KDBID_B(ID) (((ID) >> 16) & 0xFF)
#define KDBID_C(ID) (((ID) >> 8) & 0xFF)
#define KDBID_D(ID) ((ID) & 0xFF)

#define KDBID_SET_A(ID,A) (((ID) & 0x00FFFFFF) | ((A) << 24))
#define KDBID_SET_B(ID,B) (((ID) & 0xFF00FFFF) | ((A) << 16))
#define KDBID_SET_C(ID,C) (((ID) & 0xFFFF00FF) | ((C) << 8))
#define KDBID_SET_D(ID,D) (((ID) & 0xFFFFFF00) | (D))

////////////////////////////////////////////////////////////////
// well-known kernel database section types

// Manifest
#define KDBID_MANIFEST	      MAKE_KDBID(0,'M','f','t')
#define KDBID_KERNEL_PARMS    MAKE_KDBID(0,'K','p','m')
#define KDBID_DBFLIST         MAKE_KDBID(0,'D','b','f')
#define KDBID_CLUSTERLIST     MAKE_KDBID(0,'C','l','L')

// Merge schemata
#define KDBID_MERGE_SCHEMATA  MAKE_KDBID(0,0,'M','S')

// Backtracing info (for debug purposes)
#define KDBID_FF_BACKTRACING        MAKE_KDBID(0,'B','F','P')
#define KDBID_FF_BACKTRACING_DELTA  MAKE_KDBID(0,'B','F','D')
#define KDBID_BLK_BACKTRACING       MAKE_KDBID(0,'B','B','P')
#define KDBID_BLK_BACKTRACING_DELTA MAKE_KDBID(0,'B','B','D')

// Debug-time only sections
#define KDBID_CLUSTER_EXPORT_MAP    MAKE_KDBID('!','C','X','M')
#define KDBID_DEBUG_STRINGS         MAKE_KDBID('!','D','S','T')
#define KDBID_EVENT_INFO            MAKE_KDBID('!','E','D','I')
#define KDBID_VERDICT_RID           MAKE_KDBID('!','V','R','I')

#define KDBID_VERDICT_RID_OLD       MAKE_KDBID('V','d','c','R')

// Logical block configuration
#define KDBID_LB_CONFIG      MAKE_KDBID('L','B','c','f')
#define KDBID_LB_CONFIG_V2   MAKE_KDBID('L','B','c','2')
#define KDBID_LB_EOS         MAKE_KDBID('L','B','e','t')
// Logical block variables
#define KDBID_VARIABLES      MAKE_KDBID('V','a','r','s')
// Actions (scenario manager)
#define KDBID_ACTIONS        MAKE_KDBID('A','c','t','n')
// Action table - version 2
#define KDBID_ACTIONS_V2     MAKE_KDBID('A','c','t','2')
// Verdicts
#define KDBID_VERDICTS       MAKE_KDBID('V','d','c','t')
#define KDBID_VERDICTS_V2    MAKE_KDBID('V','d','c','2')
#define KDBID_THREAT_TYPES   MAKE_KDBID('T','h','r','t')
#define KDBID_VERDICT_INFO   MAKE_KDBID('V','d','c','I')

// Procedures
#define KDBID_PROCTABLE      MAKE_KDBID('P','r','o','c')
#define KDBID_PROCTHK_C_LINK MAKE_KDBID('P','r','T','C')
#define KDBID_PROCTHK_S_LINK MAKE_KDBID('P','r','T','S')

// Object code and constant data
#define KDBID_OBJCODE       MAKE_KDBID('O','b','j','C')
#define KDBID_OBJREL        MAKE_KDBID('O','b','j','R')
#define KDBID_OBJREL_TYPE   MAKE_KDBID('O','c','R','T')
#define KDBID_OBJREL_OFFSET MAKE_KDBID('O','c','R','O')

#define KDBID_APP_OBJECTS   MAKE_KDBID('O','b','j','A')
#define KDBID_OBJREL_APPID  MAKE_KDBID('O','c','R','A')

// Link table
#define KDBID_LINKTABLE MAKE_KDBID('L','n','k','T')

// Public names (symbols)
#define KDBID_PUBNAMES  MAKE_KDBID('P','u','b','N')

// String table
#define KDBID_STRINGTABLE    MAKE_KDBID('S','t','r','T')

// Event proxies
#define KDBID_EVENTS         MAKE_KDBID('E','v','n','t')
#define KDBID_EVENT_LINKAGE  MAKE_KDBID('E','v','L','T')

// Component loader table
#define KDBID_COMPLDR        MAKE_KDBID('C','m','L','d')

////////////////////////////////////////////////////////////////
// Kernel parameter IDs

#define KLAV_KERNEL_PARM_CONTEXT_SIZE  MAKE_KDBID('C','t','x','N')
#define KLAV_KERNEL_PARM_PROC_COUNT    MAKE_KDBID('P','r','c','N')
#define KLAV_KERNEL_PARM_DYN_DATA_SIZE MAKE_KDBID('D','D','s','z')
#define KLAV_KERNEL_PARM_ROOT_SCENARIO MAKE_KDBID('S','c','e','n')
#define KLAV_KERNEL_PARM_PERF_SLOT_CNT MAKE_KDBID('P','f','s','N')
#define KLAV_KERNEL_PARM_ENGINE_FLAGS  MAKE_KDBID('E','N','G','F')
#define KLAV_KERNEL_PARM_VERDICT_COUNT MAKE_KDBID('V','d','c','N')
#define KLAV_KERNEL_PARM_DB_TIMESTAMP  MAKE_KDBID('T','i','m','e')

enum // engine flags
{
	KLAV_ENGINE_FLAG_OBJCODE_WRITABLE = 0x0001,  // make objcode section writable
};

////////////////////////////////////////////////////////////////
// Other well-known KDBIDs

// Test events ($word)
#define KDBID_TEST_WORDS      MAKE_KDBID('T','s','t','W')

// Signature tree data
#define KDBID_SIG_TREE_DATA   MAKE_KDBID('S','g','n','T')
#define KDBID_SIG_TREE_CHK    MAKE_KDBID('S','g','c','T')
#define KDBID_SIG_TREE_TEXT   MAKE_KDBID('S','g','t','T')
#define KDBID_SIG_TREE_ENTREF MAKE_KDBID('S','g','e','T')
#define KDBID_SIG_TREE_EVID   MAKE_KDBID('S','g','x','T')

// For KDCs
#define KDBID_SIG_TREE2_REC   MAKE_KDBID('S','g','2','R')
#define KDBID_SIG_TREE2_BODY  MAKE_KDBID('S','g','2','B')

// New-style tree signatures
#define KDBID_NSIG            MAKE_KDBID('S','g','Z','N')
// It is important that
// id above must come after all of the ids below
// in a lexicographically-ordered sequence,
// otherwise merging may produce incorrect results
#define KDBID_NSIG_TABLE      MAKE_KDBID('S','g','I','N')
#define KDBID_NSIG_RGNID      MAKE_KDBID('S','g','R','N')
#define KDBID_NSIG_RGNTYPE    MAKE_KDBID('S','g','T','N')
#define KDBID_NSIG_POS        MAKE_KDBID('S','g','P','N')
#define KDBID_NSIG_TREE       MAKE_KDBID('S','g','D','N')
#define KDBID_NSIG_CHECKS     MAKE_KDBID('S','g','C','N')
#define KDBID_NSIG_ENTREF     MAKE_KDBID('S','g','E','N')

// Positional signature search data
#define KDBID_SIG_POS_DATA    MAKE_KDBID('S','g','n','P')
#define KDBID_SIG_POS_TEXT    MAKE_KDBID('S','g','t','P')

// Positional signature check data
#define KDBID_SIG_CHECK_DATA  MAKE_KDBID('S','g','n','C')
#define KDBID_SIG_CHECK_TEXT  MAKE_KDBID('S','g','t','C')

// Region class table
#define KDBID_REGION_CLASSES  MAKE_KDBID('R','g','n','C')

// Subengine loading list (TODO: obsolete, to be removed)
#define KDBID_SUBENGINES_LIST MAKE_KDBID('S','u','b','L')

// Export records
#define KDBID_EXPORT_RECORDS_EXP1 MAKE_KDBID('E','x','p','1')
#define KDBID_EXPORT_RECORDS_EXP2 MAKE_KDBID('E','x','p','2')
// obsolete export record format
#define KDBID_EXPORT_RECORDS_OLD  MAKE_KDBID('X','R','e','c')

// KDBID for AVP3 export records
#define KDBID_AVP3 MAKE_KDBID('A','V','P','3')

#define KDBID_AVP3_KERNEL  MAKE_KDBID('A','V','P','k')
#define KDBID_AVP3_JUMP    MAKE_KDBID('A','V','P','j')
#define KDBID_AVP3_SECTOR  MAKE_KDBID('A','V','P','s')
#define KDBID_AVP3_MEMORY  MAKE_KDBID('A','V','P','m')
#define KDBID_AVP3_FILE    MAKE_KDBID('A','V','P','f')
#define KDBID_AVP3_CA      MAKE_KDBID('A','V','P','c')
#define KDBID_AVP3_UNPACK  MAKE_KDBID('A','V','P','u')
#define KDBID_AVP3_EXTRACT MAKE_KDBID('A','V','P','e')

// AVC_FILE grouping
#define KDBID_AVP3_GROUP   MAKE_KDBID('A','V','P','g')
// record ordering (leader/group)
#define KDBID_AVP3_ORDER   MAKE_KDBID('A','V','P','o')
// Link object code
#define KDBID_AVP3_LINK    MAKE_KDBID('A','V','P','l')

// Namespace for generic linker-assigned identifiers
#define KDBID_GENERIC_ID_ALLOC MAKE_KDBID(0, 'G', 'I', 'A')

// KDBID for vulnerability database
#define KDBID_VLNS_FS_DETECTIONS		MAKE_KDBID('V','l','F','D')
#define KDBID_VLNS_FS_DETECTIONS_SORTED	MAKE_KDBID('V','l','F','s')
#define KDBID_VLNS_FS_SECURIES			MAKE_KDBID('V','l','F','S')
#define KDBID_VLNS_OS_DETECTIONS		MAKE_KDBID('V','l','O','D')
#define KDBID_VLNS_OS_DETECTIONS_SORTED	MAKE_KDBID('V','l','O','s')
#define KDBID_VLNS_OS_KBNUMBERS			MAKE_KDBID('V','l','O','N')
#define KDBID_VLNS_INTEGERS    			MAKE_KDBID('V','l','I','n')
#define KDBID_VLNS_STRINGS    			MAKE_KDBID('V','l','S','t')

////////////////////////////////////////////////////////////////
// Verdict KDU flags

#define KDU_VERDICT_FMT_MASK 0x0F
#define KDU_VERDICT_FMT_VERSION(V) ((V) & KDU_VERDICT_FMT_MASK)
#define KDU_VERDICT_MAKE_FMT_VERSION(V) ((V) & KDU_VERDICT_FMT_MASK)

enum
{
	KDU_VERDICT_FMT_VERSION_1 = 1,  // current version
};

// flags
enum {
	KDU_VERDICT_HAS_RECORD_NO   = 0x10,
	KDU_VERDICT_HAS_FIXED_ID    = 0x20,
	KDU_VERDICT_HAS_THREAT_TYPE = 0x40,
	KDU_VERDICT_NAME_EXPLICIT   = 0x80,  // verdict name is specified explicitly
};

////////////////////////////////////////////////////////////////
// Flags for AVP3 records

// mask to extract AVP3 record flags
#define KLAV_AVP3_FLAGS_MASK  0xFFFF

// flag: record is 'ordered group leader'
#define KLAV_AVP3_F_IS_LEADER 0x0001

// flags to indicate presense of optional attributes in KDBID_AVP3_xxx records
#define KLAV_AVP3_F_HAS_LEADER    0x010000
#define KLAV_AVP3_F_HAS_ORDER     0x020000
#define KLAV_AVP3_F_HAS_CONTAINER 0x040000

////////////////////////////////////////////////////////////////
// Fragment IDs for signature domains

#define SIG_DOMAIN_X86    MAKE_KDBID('X','8','6',0)
#define SIG_DOMAIN_VBA    MAKE_KDBID('V','B','A',0)
#define SIG_DOMAIN_SCRIPT MAKE_KDBID('S','C','R',0)

////////////////////////////////////////////////////////////////
// Export records

// KDU record layout:
// <XTYPE> <ID> <"NAME"> <FLAGS> [<payload>]
// <XTYPE + ID> is the record identifier

// export type mask (XTYPE)
#define KDB_EXPORT_TYPE_MASK 0xFF

// export type (EXP1)
#define KDB_EXPORT_TYPE_EXP1 0x01
#define KDB_EXPORT_TYPE_EXP2 0x02

// export record flags
#define KDB_EXPORT_FLAG_URGENT  0x01
#define KDB_EXPORT_FLAG_PARTIAL 0x02

// payload layouts (tag sequence):
// for KDB_EXPORT_TYPE_EXP1: { <TAG> <VALUE> }

// tags
enum
{
	KDB_EXPORT_TAG_SIG1 = 0x01,  // single-signature body (binary)
	KDB_EXPORT_TAG_SIG2 = 0x05,  // binary signature format
	KDB_EXPORT_TAG_POS  = 0x02,  // signature position (unsigned integer)
	KDB_EXPORT_TAG_RANGE_BEGIN = 0x03, // signature position range begin (unsigned integer)
	KDB_EXPORT_TAG_RANGE_END   = 0x04, // signature position range end (unsigned integer)

	KDB_EXPORT_TAG_RECORD_ID      = 0x08, // record ID
	KDB_EXPORT_TAG_CONFIDENCE     = 0x09, // confidence level
	KDB_EXPORT_TAG_FILE_DATA      = 0x0A, // file data (for certain types of signatures)

	KDB_EXPORT_TAG_AVP3_R_FIRST   = 0x10, // start of AVP3 record types range (RT = TAG - FIRST)
	KDB_EXPORT_TAG_AVP3_R_KERNEL  = 0x10, // R_Kernel data
	KDB_EXPORT_TAG_AVP3_R_JUMP    = 0x11, // R_Jump data
	KDB_EXPORT_TAG_AVP3_R_MEMORY  = 0x12, // R_Memory data
	KDB_EXPORT_TAG_AVP3_R_SECTOR  = 0x13, // R_Sector data
	KDB_EXPORT_TAG_AVP3_R_FILE    = 0x14, // R_File data
	KDB_EXPORT_TAG_AVP3_R_CA      = 0x15, // R_CA data
	KDB_EXPORT_TAG_AVP3_R_UNPACK  = 0x16, // R_Unpack data
	KDB_EXPORT_TAG_AVP3_R_EXTRACT = 0x17, // R_Extract data
	KDB_EXPORT_TAG_AVP3_R_LAST    = 0x17, // end of AVP3 record types range

	KDB_EXPORT_TAG_AVP3_LINK32    = 0x1A, // AVP3 link32 data
	KDB_EXPORT_TAG_AVP3_LINK16    = 0x1B, // AVP3 link16 data
};

// canonical tag ordering:
// [SIG1] [POS] [RANGE_BEGIN] [RANGE_END]

////////////////////////////////////////////////////////////////
// Processor architectures

#define KDBID_ARCH_I386	 MAKE_KDBID('I','3','8','6')
#define KDBID_ARCH_AMD64 MAKE_KDBID('A','M','6','4')
#define KDBID_ARCH_SPARC MAKE_KDBID('S','p','r','c')
#define KDBID_ARCH_PPC   MAKE_KDBID('P','p','c', 0)
#define KDBID_ARCH_ARMEB MAKE_KDBID('A','R','M','B')
#define KDBID_ARCH_ARMEL MAKE_KDBID('A','R','M','L')
#define KDBID_ARCH_MIPS  MAKE_KDBID('M','I','P','S')

#ifdef KL_ARCH_X86
#define KDBID_ARCH_CURRENT KDBID_ARCH_I386
#elif defined KL_ARCH_AMD64
#define KDBID_ARCH_CURRENT KDBID_ARCH_AMD64
#elif defined KL_ARCH_SPARC
#define KDBID_ARCH_CURRENT KDBID_ARCH_SPARC
#elif defined KL_ARCH_ARM
#if defined KL_BIG_ENDIAN
#define KDBID_ARCH_CURRENT KDBID_ARCH_ARMEB
#else
#define KDBID_ARCH_CURRENT KDBID_ARCH_ARMEL
#endif
#elif defined KL_ARCH_PPC
#define KDBID_ARCH_CURRENT KDBID_ARCH_PPC
#elif defined KL_ARCH_MIPS
#define KDBID_ARCH_CURRENT KDBID_ARCH_MIPS
#endif

////////////////////////////////////////////////////////////////

#endif // kdb_ids_h_INCLUDED


