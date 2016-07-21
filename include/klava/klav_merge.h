// klav_merge.h
//
//

#ifndef klav_merge_h_INCLUDED
#define klav_merge_h_INCLUDED

#include <klava/klaveng.h>
#include <klava/klav_io.h>

////////////////////////////////////////////////////////////////
// Patch merger module interface

struct KLAV_DB_Merger;
typedef struct KLAV_DB_Merger * hKLAV_DB_Merger;

struct KLAV_DB_Version
{
	uint32_t kdb_release;  // aka 'incarnation'
	uint32_t kdb_version;  // sequence number
};

#define KLAV_DB_RELEASE_INVALID 0

#ifdef __cplusplus

struct KLAV_DB_Merger
{
	virtual void KLAV_CALL destroy (
			) = 0;

	virtual hKLAV_Properties KLAV_CALL get_properties (
			) = 0;

	virtual KLAV_ERR KLAV_CALL query_db_version (
				hKLAV_IO_Object   db,
				KLAV_DB_Version * pversion
			) = 0;

	virtual KLAV_ERR KLAV_CALL query_manifest_version (
				hKLAV_IO_Object   manifest,
				KLAV_DB_Version * pversion
			) = 0;

	virtual KLAV_ERR KLAV_CALL init_db (
				hKLAV_IO_Object   old_db,
				hKLAV_IO_Object   new_mft,
				hKLAV_IO_Object   tmp_db,
				hKLAV_IO_Object   new_db
			) = 0;

	virtual KLAV_ERR KLAV_CALL add_patch (
				hKLAV_IO_Object   patch_data,
				const char *      patch_info
			) = 0;

	virtual KLAV_ERR KLAV_CALL merge_db (
			) = 0;

	virtual KLAV_ERR KLAV_CALL apply_diff (
				hKLAV_IO_Object   old_db,
				hKLAV_IO_Object   diff,
				const char *      diff_info,
				hKLAV_IO_Object   tmp_db,
				hKLAV_IO_Object   new_db
			) = 0;
};

#else // __cplusplus

KLAV_DECLARE_C_INTERFACE_VTBL (KLAV_DB_Merger)
{
	void (KLAV_CALL * destroy) (
			hKLAV_DB_Merger
		);

	hKLAV_Properties (KLAV_CALL * get_properties) (
			hKLAV_DB_Merger
		);

	KLAV_ERR (KLAV_CALL * query_db_version) (
			hKLAV_DB_Merger,
			hKLAV_IO_Object,
			struct KLAV_DB_Version *
		);

	KLAV_ERR (KLAV_CALL * query_manifest_version) (
			hKLAV_DB_Merger,
			hKLAV_IO_Object,
			struct KLAV_DB_Version *
		);

	KLAV_ERR (KLAV_CALL * init_db) (
			hKLAV_DB_Merger,
			hKLAV_IO_Object,
			hKLAV_IO_Object,
			hKLAV_IO_Object,
			hKLAV_IO_Object
		);

	KLAV_ERR (KLAV_CALL * add_patch) (
			hKLAV_DB_Merger,
			hKLAV_IO_Object,
			const char *
		);

	KLAV_ERR (KLAV_CALL * merge_db) (
			hKLAV_DB_Merger
		);

	KLAV_ERR (KLAV_CALL * apply_diff) (
			hKLAV_DB_Merger,
			hKLAV_IO_Object,
			hKLAV_IO_Object,
			const char *,
			hKLAV_IO_Object,
			hKLAV_IO_Object
		);
};

KLAV_DECLARE_C_INTERFACE (KLAV_DB_Merger);

#endif // __cplusplus

typedef KLAV_ERR (KLAV_CALL * PFN_KLAV_DB_Merger_Create)(
		uint32_t version,   // usually KLAV_ENGINE_VERSION_CURRENT
		KLAV_Alloc * alloc, // memory allocator to use
		hKLAV_DB_Merger *      // returned handle to engine object
	);

#define KLAV_DB_MERGER_CREATE_FN   KLAV_DB_Merger_Create
#define KLAV_DB_MERGER_CREATE_NAME "KLAV_DB_Merger_Create"

// merger properties
enum KLAV_MERGE_PROPID
{
#define KLAV_MERGE_PROPERTY_DEF(NAME,TYPE,ID) KLAV_PROPID_##NAME = KLAV_MAKE_PROPID(TYPE,ID),
#include <klava/klav_merge.h>
};

// merger flags (for KLAV_PROPID_MERGE_FLAGS)
enum
{
	KLAV_MERGE_F_NO_RGN_CHECK = 0x01,
	KLAV_MERGE_F_DYNAMIC_TREE = 0x02,
	KLAV_MERGE_F_SIG_ENTREF   = 0x04,
	KLAV_MERGE_F_COMPACT_TREE = 0x08,
	KLAV_MERGE_F_FORMAT_KFB1  = 0x10,
};

#endif // klav_merge_h_INCLUDED

// merger property definitions
#ifdef KLAV_MERGE_PROPERTY_DEF

// merge flags (KLAV_MERGE_F_XXX)
KLAV_MERGE_PROPERTY_DEF(MERGE_FLAGS,       UINT,  0x0001)
// product ID to write to KFB header (kfb_product_id)
KLAV_MERGE_PROPERTY_DEF(MERGE_PRODUCT_ID,  UINT,  0x0002)
// arch ID to write to KFB header (kfb_arch_id)
KLAV_MERGE_PROPERTY_DEF(MERGE_ARCH_ID,     UINT,  0x0003)
// database manifest info (from manifest description)
KLAV_MERGE_PROPERTY_DEF(MERGE_MFT_INFO,    STR,   0x0004)
// Event logger interface (KLAV_Message_Printer interface)
KLAV_MERGE_PROPERTY_DEF(MERGE_LOGGER,      PTR,   0x0010)

#undef KLAV_MERGE_PROPERTY_DEF
#endif // KLAV_MERGE_PROPERTY_DEF
