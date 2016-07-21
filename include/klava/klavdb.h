// klavdb.h
//
// KLAV runtime database data structures
// This file is considered obsolete, its contents will be moved to klava/kdb.h or elsewhere
//

#ifndef klavdb_h_INCLUDED
#define klavdb_h_INCLUDED

#include <klava/kdb.h>
#include <klava/kdb_ids.h>
#include <klava/kdu.h>
#include <klava/kfb.h>
#include <klava/klavdef.h>
#include <klava/klaveng.h>

#include <klava/kdb_utils.h> // temporary
#include <klava/klav_io.h>

#include <kl_pushpack.h>

////////////////////////////////////////////////////////////////
// Object code

typedef struct tagOBJCODE_HEADER
{
	uint32_t hdr_size;
	uint32_t code_size;
	uint32_t load_addr;
	uint32_t load_addr_hi;
}
KL_PACKED
OBJCODE_HEADER;

typedef struct tagPROC_TABLE_RECORD
{
	uint32_t pt_type_id;
	uint32_t pt_parm_1;
	uint32_t pt_parm_2;
}
KL_PACKED
PROC_TABLE_RECORD;

// flags for parameter interpretation
#define KLAV_PROC_TABLE_PARM1_STR 0x10000
#define KLAV_PROC_TABLE_PARM2_STR 0x20000

// component table record (for component loader)
typedef struct tagCOMP_TABLE_RECORD
{
	uint32_t ct_key_idx;  // component key index in string table
	uint32_t ct_type_id;  // creator procedure type
	uint32_t ct_parm_1;   // creator procedure load parameter 1
	uint32_t ct_parm_2;   // creator procedure load parameter 2
}
KL_PACKED
COMP_TABLE_RECORD;

typedef struct tagOBJCODE_RELOC
{
	uint32_t reloc;
}
KL_PACKED
OBJCODE_RELOC;

typedef struct tagOBJCODE_RELOC_TYPE
{
	uint8_t reloc_type;
}
KL_PACKED
OBJCODE_RELOC_TYPE;

typedef struct tagOBJCODE_RELOC_OFFSET
{
	uint32_t reloc_offset;
}
KL_PACKED
OBJCODE_RELOC_OFFSET;

typedef struct tagOBJCODE_RELOC_APPID
{
	uint32_t reloc_appid;
}
KL_PACKED
OBJCODE_RELOC_APPID;

////////////////////////////////////////////////////////////////
// Procedure types

enum
{
	KLAV_PROCTYPE_UNKNOWN  = 0,
	KLAV_PROCTYPE_OBJCODE  = 1,
	KLAV_PROCTYPE_PRAGUE   = 2,
	KLAV_PROCTYPE_EXPORT   = 3,
	KLAV_PROCTYPE_LIB      = 4,
	KLAV_PROCTYPE_APP      = 5,
};

// NOTE: the following definitions are for LINK-block
#define KLAV_PROC_TYPE_MASK          0xFF
#define KLAV_PROC_TYPE(TYPE_FLAGS)   ((TYPE_FLAGS) & KLAV_PROC_TYPE_MASK)
#define KLAV_PROC_FLAGS(TYPE_FLAGS)  ((TYPE_FLAGS) & ~KLAV_PROC_TYPE_MASK)

#define KLAV_PROC_FLAG_HAS_ID  0x80000000


// NOTE: the following definitions are for LOAD-block
#define KLAV_MAKE_PROCTYPEID(TYPE,ID) ((((TYPE) & 0xFF) << 24) | ((ID) & 0xFFFFFF))
#define KLAV_PROCTYPE_ID(ID)   ((ID) & 0x00FFFFFF)
#define KLAV_PROCTYPE_TYPE(ID) (((ID) & 0xFF000000)>>24)

#define KLAV_RELOC_OFFSET(id) (id & 0x00FFFFFF)
#define KLAV_RELOC_TYPE(id)   ((id & 0xFF000000)>>24)

////////////////////////////////////////////////////////////////
// Kernel parameters

typedef struct tagKDB_KERNEL_PARM
{
	uint32_t kdb_kparm_id;
	uint32_t kdb_kparm_value;
}
KL_PACKED
KDB_KERNEL_PARM;

////////////////////////////////////////////////////////////////
// old objcode header format

typedef struct tagOBJCODE_HEADER_OLD
{
	uint32_t code_size;
	uint32_t reloc_cnt;
	uint32_t load_addr;
	uint32_t reserved;
}
KL_PACKED
OBJCODE_HEADER_OLD;

////////////////////////////////////////////////////////////////
// Action manager

typedef struct tagACTION_RECORD
{
	uint32_t act_event_id;
	uint32_t act_name_id;
}
KL_PACKED
ACTION_RECORD;

////////////////////////////////////////////////////////////////
// Verdict Manager

typedef struct tagTHREAT_TYPE_RECORD
{
	uint32_t threat_id;
	uint32_t threat_danger;
}
KL_PACKED
THREAT_TYPE_RECORD;

#define VERDICT_THREAT_TYPE_ID(VDID) (((VDID) >> 24) & 0xFF)
#define VERDICT_INFO_OFFSET(VDID) ((VDID) & 0x00FFFFFF)

typedef struct tagVERDICT_RECORD
{
	uint32_t vid_info_id;
	uint32_t vid_cure_id;
}
KL_PACKED
VERDICT_RECORD;

typedef struct tagVERDICT_RECORD_V2
{
	uint32_t vid2_type_flags;
	uint32_t vid2_rnam_ref;
}
KL_PACKED
VERDICT_RECORD_V2;

#define RNAM_MAX_DEPTH 32

////////////////////////////////////////////////////////////////
// Objcode

#define KDU_OBJCODE_F_COMMUNAL  (1<<0)
#define KDU_OBJCODE_F_BIGENDIAN (1<<1)

typedef struct tagKDU_OBJCODE_HEADER
{
	uint32_t obj_alignment;
	uint32_t obj_flags;
}
KL_PACKED
KDU_OBJCODE_HEADER;

////////////////////////////////////////////////////////////////
// Non-positional (tree) signatures

// post-check logic
enum SIG_RGN_CHECK_MODE {
	SIG_RGN_CHECK_NONE        = 0,
	SIG_RGN_CHECK_TYPE        = 1,
	SIG_RGN_CHECK_TYPE_ID     = 2,
	SIG_RGN_CHECK_TYPE_POS    = 3,
	SIG_RGN_CHECK_TYPE_ID_POS = 4
};

#define SIG_RGN_CHECK_HAS_TYPE(MODE) ((MODE) != 0)
#define SIG_RGN_CHECK_HAS_ID(MODE)   (((MODE) - 1) & 0x01)
#define SIG_RGN_CHECK_HAS_POS(MODE)  (((MODE) - 1) & 0x02)

#define SIG_RGN_CHECK_F_TIGHT 0x80000000
#define SIG_RGN_CHECK_F_LAST  0x40000000

// in signature tree
#define SIG_RGN_CHECK_EF_EVID  0x00800000
#define SIG_RGN_CHECK_EF_LAST  0x00400000

// 'tight' mode encoding
#define SIG_RGN_CHECK_MAKE_TIGHT(TYPE,ID,POS) \
	(SIG_RGN_CHECK_F_TIGHT | ((((((POS) & 0x3FFF) << 8) | ((TYPE) & 0xFF)) << 8) | ((ID) & 0xFF)))

#define SIG_RGN_CHECK_TIGHT_TYPE(W) (((W) >> 8) & 0xFF)
#define SIG_RGN_CHECK_TIGHT_ID(W)   ((W) & 0xFF)
#define SIG_RGN_CHECK_TIGHT_POS(W)  (KLAV_SIGN_EXTEND((((W) >> 16) & 0xFFFF), 0x2000))

// 'standard' mode encoding
#define SIG_RGN_CHECK_MAKE_STD(MODE,A,B) (((((MODE) << 8) | ((A) & 0xFF)) << 16) | ((B) & 0xFFFF))
// single argument in first word
#define SIG_RGN_CHECK_MAKE_STD1(MODE,W) (((MODE) << 24) | ((W) & 0xFFFFFF))

#define SIG_RGN_CHECK_STD_MODE(W) (((W) >> 24) & 0x3F)
#define SIG_RGN_CHECK_STD_A(W)    (((W) >> 16) & 0xFF)
#define SIG_RGN_CHECK_STD_B(W)    ((W) & 0xFFFF)

// encoding of pair of 16-bit values
#define SIG_RGN_CHECK_MAKE_PAIR16(HI,LO) ((((HI) & 0xFFFF) << 16) | ((LO) & 0xFFFF))
#define SIG_RGN_CHECK_PAIR16_HI(W)      (((W) >> 16) & 0xFFFF)
#define SIG_RGN_CHECK_PAIR16_LO(W)      ((W) & 0xFFFF)

////////////////////////////////////////////////////////////////
// Positional signatures

// region match mode for positional signatures

enum RGN_MATCH_MODE {
	RGN_MATCH_ANY = 0,
	RGN_MATCH_TYPE = 1,
	RGN_MATCH_TYPE_ID = 2,
	RGN_MATCH_CLASS = 3
};

#define SIG_POS_SECTION_MODE(MODE,RGNTYPE) ((((MODE) & 0xFF) << 24) | ((RGNTYPE) & 0xFFFFFF))

#define RGN_CLASS_TYPEFLAGS(W)  (((W) >> 24) & 0xFF)
#define RGN_CLASS_COUNT(W)      ((W) & 0xFFFFFF)

////////////////////////////////////////////////////////////////
// AVP3-style positional signatures

typedef struct tagSIG_POS_2_RECORD
{
	uint8_t  m_len1;   // +00h SIG1 length
	uint8_t  m_len2;   // +01h SIG2 length
	uint16_t m_word1;  // +02h SIG1 first word
	uint16_t m_off1;   // +04h SIG1 offset
	uint16_t m_off2;   // +06h SIG2 offset
	uint32_t m_hash1;  // +08h SIG1 hash (AVP3 chksum)
	uint32_t m_hash2;  // +0Ch SIG2 hash (AVP3 chksum)
} // size: 10h
KL_PACKED
KLAV_SIG_POS_2_RECORD;

////////////////////////////////////////////////////////////////
// Signature checks

// Signature check record layout:
//
// <TF|LEN>
// <HASH>
// [<POS>]
// [<RGNTYPE>]
// [<RGNID>]

#define MAKE_SIG_CHECK_RECORD(FLAGS,LEN) (((FLAGS) << 24) | ((LEN) & 0x00FFFFFF))
#define SIG_CHECK_RECORD_FLAGS(REC) (((REC) >> 24) & 0xFF)
#define SIG_CHECK_RECORD_LEN(REC)   ((REC) & 0x00FFFFFF)

// Signature check flags
enum {
	SIG_CHECK_F_HAS_POS      = 0x10, // <POS> field is present
	SIG_CHECK_F_HAS_RGN_TYPE = 0x20, // <RGN_TYPE> field is present
	SIG_CHECK_F_HAS_RGN_ID   = 0x40, // <RGN_ID> field is present
};

////////////////////////////////////////////////////////////////
// AVP3 records
// (OBSOLETE)

// Primary AVP records (header: fixed-size part)
typedef struct tagAVP3_RECORD_HDR
{
	uint8_t  avc_record_uuid [16];
	uint16_t avc_record_flags;
	uint16_t avc_file_idx;
}
KL_PACKED
AVP3_RECORD_HDR;

// AVP link records (header: fixed-size part)
typedef struct tagAVP3_LINK_HDR
{
	uint8_t  avc_record_uuid [16];
	uint32_t avc_size_flags;
}
KL_PACKED
AVP3_LINK_HDR;

// AVC file record (section: KDBID_AVP3_GROUP)
typedef struct tagAVP3_AVC_FILE_RECORD
{
	uint32_t avc_file_nameidx;
	uint32_t avc_file_order;
}
KL_PACKED
AVP3_AVC_FILE_RECORD;

// AVC group/order record (section: KDBID_AVP3_ORDER)
typedef struct tagAVP3_AVC_ORDER_RECORD
{
	uint8_t  avc_record_uuid [16];
	uint8_t  avc_leader_uuid [16];
	uint32_t avc_record_order;
}
KL_PACKED
AVP3_AVC_ORDER_RECORD;

////////////////////////////////////////////////////////////////

#include <kl_poppack.h>

////////////////////////////////////////////////////////////////
// Database loader interface

#define KLAV_DBLOADER_FLAG_READ  KLAV_MEM_PROT_READ
#define KLAV_DBLOADER_FLAG_WRITE KLAV_MEM_PROT_WRITE
#define KLAV_DBLOADER_FLAG_EXEC  KLAV_MEM_PROT_EXEC

struct KLAV_DB_Fragment
{
	void *          kdb_handle;
	const uint8_t * kdb_data;
	uint32_t        kdb_size;

#ifdef __cplusplus
	KLAV_DB_Fragment ()
		{ clear (); }

	inline void clear ()
	{
		kdb_handle = 0;
		kdb_data = 0;
		kdb_size = 0;
	}
#endif // cplusplus
};

#ifdef __cplusplus

struct KLAV_NO_VTABLE KLAV_DB_Loader
{
	virtual KLAV_ERR KLAV_CALL load_fragment (uint32_t sec_type, uint32_t frag_id, uint32_t flags, KLAV_DB_Fragment* frag) = 0;
	virtual KLAV_ERR KLAV_CALL release_fragment (KLAV_DB_Fragment *frag) = 0;
	virtual KLAV_ERR KLAV_CALL protect_fragment (KLAV_DB_Fragment *frag, uint32_t flags) = 0;
	virtual uint32_t KLAV_CALL get_engine_parm (uint32_t id) = 0;
	virtual KLAV_ERR KLAV_CALL get_ldr_iface (uint32_t ifc_id, void **ppifc) = 0;

    KLAV_DUMMY_METHODS_DECL(KLAV_DB_Loader)
};

#else // cplusplus

KLAV_DECLARE_C_INTERFACE_VTBL(KLAV_DB_Loader)
{
	KLAV_ERR (KLAV_CALL *load_fragment)    (hKLAV_DB_Loader, uint32_t, uint32_t, uint32_t, struct KLAV_DB_Fragment *);
	KLAV_ERR (KLAV_CALL *release_fragment) (hKLAV_DB_Loader, struct KLAV_DB_Fragment *);
	KLAV_ERR (KLAV_CALL *protect_fragment) (hKLAV_DB_Loader, struct KLAV_DB_Fragment *, uint32_t);
	uint32_t (KLAV_CALL *get_engine_parm)  (hKLAV_DB_Loader, uint32_t);
	KLAV_ERR (KLAV_CALL *get_ldr_iface)    (hKLAV_DB_Loader, uint32_t, void **);
};

KLAV_DECLARE_C_INTERFACE(KLAV_DB_Loader);

#endif // cplusplus

////////////////////////////////////////////////////////////////
// procedure/component loader

typedef union tag_KLAV_Proc_Parm
{
	const char * str;
	uint32_t     num;

#ifdef __cplusplus
	void clear () { str = 0; }
#endif // __cplusplus
} KLAV_Proc_Parm;

struct KLAV_Proc_Info
{
	uint32_t       id;         // procedure ID
	unsigned int   type;       // KLAV_PROCTYPE_XXX
	KLAV_Proc_Parm parm1;      // (old):name(str),           (new):pt_parm_1
	KLAV_Proc_Parm parm2;      // (old):PID/dllname/objname, (new):pt_parm_2

#ifdef __cplusplus
	KLAV_Proc_Info () { clear (); }

	void clear ()
	{
		id = 0;
		type = KLAV_PROCTYPE_UNKNOWN;
		parm1.clear ();
		parm2.clear ();
	}
#endif // __cplusplus
};

////////////////////////////////////////////////////////////////
// procedure resolver

#ifdef __cplusplus

struct KLAV_NO_VTABLE KLAV_Proc_Resolver
{
public:
	virtual KLAV_ERR KLAV_CALL resolve_procedure (
			KLAV_CONTEXT_TYPE,
			const KLAV_Proc_Info *proc,
			KLAV_PFN *res
		) = 0;

    KLAV_DUMMY_METHODS_DECL(KLAV_Proc_Resolver)
};

struct KLAV_NO_VTABLE KLAV_Debug_Proc_Resolver
{
public:
	// called multiple times after objcode is loaded
	virtual KLAV_ERR KLAV_CALL resolve_procedure (
			KLAV_CONTEXT_TYPE,
			const KLAV_Proc_Info *proc,
			KLAV_PFN *res
		) = 0;

	// called after objcode is loaded and relocated
	virtual KLAV_ERR KLAV_CALL process_objcode (
			KLAV_CONTEXT_TYPE,
			const void *  objcode_addr,
			uint32_t      objcode_size
		) = 0;

    KLAV_DUMMY_METHODS_DECL(KLAV_Debug_Proc_Resolver)
};

#endif // __cplusplus

#define KLAV_DBG_KDT_JMPTAB_NAME    "KLAV_KDT_JMPTAB"
#define KLAV_DBG_KDT_PROCTAB_NAME   "KLAV_KDT_PROCTAB"
#define KLAV_DBG_KDT_TIMESTAMP_NAME "KLAV_KDT_TIMESTAMP"

////////////////////////////////////////////////////////////////
// DB manager - access to the database files

#ifdef __cplusplus

struct KLAV_NO_VTABLE KLAV_DB_Manager
{
	virtual uint32_t KLAV_CALL get_dbm_version (  // get DB manager interface version
			) = 0;

	virtual void * KLAV_CALL get_dbm_iface (  // get extended interface
				uint32_t ifc_id
			) = 0;

	virtual KLAV_ERR KLAV_CALL query_db_file ( // TODO: 'stat' interface
				const char *      relpath     // relative path
			) = 0;

	virtual KLAV_ERR KLAV_CALL open_db_file (
				const char *      name,       // relative path
				uint32_t          access,     // access mode
				uint32_t          flags,      // 
				hKLAV_Properties  hprops,     // additional extension properties
				hKLAV_IO_Object * phfile
			) = 0;

    KLAV_DUMMY_METHODS_DECL(KLAV_DB_Manager)
};

#else // __cplusplus

KLAV_DECLARE_C_INTERFACE_VTBL(KLAV_DB_Manager)
{
	uint32_t (KLAV_CALL *get_dbm_version) (
				hKLAV_DB_Manager
			);

	void * (KLAV_CALL *get_dbm_iface) (
				hKLAV_DB_Manager,
				uint32_t
			);

	KLAV_ERR (KLAV_CALL *query_db_file) (
				hKLAV_DB_Manager,
				const char *
			);

	KLAV_ERR (KLAV_CALL *open_db_file) (
				hKLAV_DB_Manager,
				const char *,
				uint32_t,
				uint32_t,
				hKLAV_Properties,
				hKLAV_IO_Object *
			);
};

KLAV_DECLARE_C_INTERFACE(KLAV_DB_Manager);

#endif // __cplusplus

struct KLAV_DBM_Init_Parms
{
	uint32_t              struct_size;    // sizeof (struct)
	hKLAV_Alloc           allocator;      // memory allocator
	hKLAV_IO_Library      io_library;     // system IO library
	hKLAV_Message_Printer tracer;         // message printer (for tracing, optional)
};

KLAV_EXTERN_C
KLAV_ERR KLAV_CALL KLAV_DBM_Create (
			const struct KLAV_DBM_Init_Parms * init_parms,
			hKLAV_DB_Manager * phmgr
		);

KLAV_EXTERN_C
KLAV_ERR KLAV_CALL KLAV_DBM_Destroy (
			hKLAV_DB_Manager mgr
		);

KLAV_EXTERN_C
KLAV_ERR KLAV_CALL KLAV_DBM_Open (
			hKLAV_DB_Manager hmgr,
			const char *base_dir
		);

KLAV_EXTERN_C
KLAV_ERR KLAV_CALL KLAV_DBM_Close (
			hKLAV_DB_Manager hmgr
		);

////////////////////////////////////////////////////////////////
// PE module loader

KLAV_EXTERN_C
KLAV_ERR KLAV_CALL KLAV_PE_Loader_Create (
			hKLAV_Alloc          alloc,
			const char *         module_dir,
			hKLAV_IO_Library     iolib,
			hKLAV_Virtual_Memory vmem,
			hKLAV_DSO_Loader     system_ldr,
			hKLAV_DSO_Loader *   phldr
		);


KLAV_EXTERN_C
KLAV_ERR KLAV_CALL KLAV_PE_Loader_Destroy (
			hKLAV_DSO_Loader hldr
		);

enum
{
	KLAV_KDL_LOADER_SYSTEM     = 0x0001,  // always use system DSO loader, disable direct PE loading
	KLAV_KDL_LOADER_NO_ABSPATH = 0x0002,  // disallow passing absolute paths to dl_open
};

struct KLAV_KDL_Loader_Parms
{
	hKLAV_Alloc          m_alloc;   // allocator
	hKLAV_IO_Library     m_iolib;   // IO library (optional, for PE loader)
	hKLAV_Virtual_Memory m_vmem;    // virtual memory (optional, for PE loader)
	hKLAV_DSO_Loader     m_sysldr;  // system DSO loader
	const char *         m_dbdir;   // DB base directory
	uint32_t             m_flags;   // KLAV_KDL_LOADER_F_XXX
};

KLAV_EXTERN_C
KLAV_ERR KLAV_CALL KLAV_KDL_Loader_Create (
			const struct KLAV_KDL_Loader_Parms * parms,
			hKLAV_DSO_Loader *   p_kdl_ldr
		);


KLAV_EXTERN_C
KLAV_ERR KLAV_CALL KLAV_KDL_Loader_Destroy (
			hKLAV_DSO_Loader hldr
		);

////////////////////////////////////////////////////////////////
// DB loader factory

struct KLAV_DB_Loader_Factory
{
	KLAV_ERR (KLAV_CALL * pfn_create_db_loader)(
			hKLAV_Engine eng, 
			const char * fname,
			uint32_t     flags,
			hKLAV_DB_Loader *phldr
		); // returns ENOTFOUND if not our format

	KLAV_ERR (KLAV_CALL * pfn_destroy_db_loader)(
			hKLAV_DB_Loader hldr
		);
};

typedef const struct KLAV_DB_Loader_Factory * KLAV_DB_LOADER_IMPL;

extern const struct KLAV_DB_Loader_Factory KLAV_DB_Loader_Factory_KDC;
extern const struct KLAV_DB_Loader_Factory KLAV_DB_Loader_Factory_KFB;
extern const struct KLAV_DB_Loader_Factory KLAV_DB_Loader_Factory_NULL;

#define KLAV_DB_LOADER_KDC  &KLAV_DB_Loader_Factory_KDC
#define KLAV_DB_LOADER_KFB  &KLAV_DB_Loader_Factory_KFB
#define KLAV_DB_LOADER_NULL &KLAV_DB_Loader_Factory_NULL

////////////////////////////////////////////////////////////////
// LZMA decompression helper

KLAV_ERR KLAV_LZMA_Decompress (
			hKLAV_Alloc allocator,
			const void *src_data,
			uint32_t    src_size,
			void *      dst_data,
			uint32_t    dst_size
		);

////////////////////////////////////////////////////////////////

#endif // klavdb_h_INCLUDED

