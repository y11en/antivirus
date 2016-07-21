// ntreelink.h
//
//

#ifndef ntreelink_h_INCLUDED
#define ntreelink_h_INCLUDED

#include <klava/klavdef.h>
#include <klava/kl_dyn_obj.h>

class KLAV_FlatFileBlockDiffer
{
public:
	virtual void diffBinBlockVersions(void const* oldData, void const* newData, uint32_t blockSize, uint32_t blockOffset) = 0;
};

struct TNS_LDB_HDR;

struct KLAV_NO_VTABLE KLAV_Ntree_Linker : public KLDynObject
{
	virtual KLAV_ERR KLAV_CALL open (const char *path, uint32_t release, uint32_t version) = 0;
	virtual KLAV_ERR KLAV_CALL create (const char *path, uint32_t release) = 0;
	virtual KLAV_ERR KLAV_CALL close () = 0;

	virtual KLAV_ERR KLAV_CALL add_signature (const void *data, uint32_t size, uint32_t sigid) = 0;
	virtual KLAV_ERR KLAV_CALL del_signature (const void *data, uint32_t size) = 0;

	virtual KLAV_ERR KLAV_CALL get_patches (KLAV_FlatFileBlockDiffer *patch_differ) = 0;
	virtual KLAV_ERR KLAV_CALL commit (uint32_t *new_version, uint32_t *total_size) = 0;

	virtual KLAV_ERR KLAV_CALL get_ldb_hdr(TNS_LDB_HDR* hdr) = 0;
//	virtual KLAV_ERR KLAV_CALL rollback () = 0;
};

extern "C"
KLAV_ERR KLAV_CALL KLAV_Ntree_Linker_Create (KLAV_Ntree_Linker **pplinker);

#endif // ntreelink_h_INCLUDED

