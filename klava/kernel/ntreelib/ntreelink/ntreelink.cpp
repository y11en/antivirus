// ntreelink.cpp
//
//

#include "ntreelink.h"
#include "tns_mgr.h"
#include "tns_db.h"

#include <klava/klav_utils.h>
#include <exception>

KLAV_Malloc_Alloc g_malloc;

#define TREE_HASH_LEVEL 16

////////////////////////////////////////////////////////////////

class Ntree_Linker_Impl : public KLAV_Ntree_Linker
{
public:
			Ntree_Linker_Impl ();
	virtual ~Ntree_Linker_Impl ();

	virtual void KLAV_CALL destroy ();

	virtual KLAV_ERR KLAV_CALL open (const char *path, uint32_t release, uint32_t version);
	virtual KLAV_ERR KLAV_CALL create (const char *path, uint32_t release);
	virtual KLAV_ERR KLAV_CALL close ();

	virtual KLAV_ERR KLAV_CALL add_signature (const void *data, uint32_t size, uint32_t sigid);
	virtual KLAV_ERR KLAV_CALL del_signature (const void *data, uint32_t size);

	virtual KLAV_ERR KLAV_CALL get_patches (KLAV_FlatFileBlockDiffer *patch_writer);
	virtual KLAV_ERR KLAV_CALL commit (uint32_t *new_version, uint32_t *total_size);

	virtual KLAV_ERR KLAV_CALL get_ldb_hdr(TNS_LDB_HDR* hdr);

// 	virtual KLAV_ERR KLAV_CALL rollback ();

private:
	File_Block_Device       m_blk_device;
	File_Block_Manager      m_blk_mgr;
	Tree_Node_Block_Manager m_node_mgr;

	nodeid_t  m_root_node;
	uint32_t  m_hash_level;
	bool      m_opened;

	KLAV_ERR do_open_treedb (bool load);
	KLAV_ERR do_close_treedb ();

	KLAV_ERR report_error_v (KLAV_ERR err, const char *fmt, va_list ap);
	KLAV_ERR report_error (KLAV_ERR err, const char *fmt, ...);
};

////////////////////////////////////////////////////////////////

Ntree_Linker_Impl::Ntree_Linker_Impl () :
	m_blk_device (),
	m_blk_mgr (& g_malloc),
	m_node_mgr (& g_malloc),
	m_root_node (NODEID_INVALID),
	m_hash_level (TREE_HASH_LEVEL),
	m_opened (false)
{
}

Ntree_Linker_Impl::~Ntree_Linker_Impl ()
{
	do_close_treedb ();
}

void KLAV_CALL Ntree_Linker_Impl::destroy ()
{
	delete this;
}

KLAV_ERR KLAV_CALL Ntree_Linker_Impl::open (const char *path, uint32_t release, uint32_t version)
{
	KLAV_ERR err = KLAV_OK;

	if (m_opened)
		return report_error (KLAV_EALREADY, "database already opened");

	try
	{
		TNDB_TREE_VERSION tree_ver;
		tree_ver.tndb_release = release;
		tree_ver.tndb_seqno   = version;
		
		err = m_blk_device.open (path, 0, & tree_ver, 0);
		if (KLAV_FAILED (err))
		{
			err = report_error (err, "error opening database file: %s", path);
			do_close_treedb ();
			return err;
		}

		err = do_open_treedb (true);
		if (KLAV_FAILED (err))
		{
			err = report_error (err, "error opening tree database on file: %s", path);
			do_close_treedb ();
			return err;
		}
	}
	catch (const std::exception& ex)
	{
		do_close_treedb ();
		err = report_error (KLAV_EUNKNOWN, "error (exception) opening tree database: %s", ex.what ());
	}

	return err;
}

KLAV_ERR KLAV_CALL Ntree_Linker_Impl::create (const char *path, uint32_t release)
{
	KLAV_ERR err = KLAV_OK;

	if (m_opened)
		return report_error (KLAV_EALREADY, "database already opened");

	try
	{
		TNDB_TREE_VERSION tree_ver;
		tree_ver.tndb_release = release;
		tree_ver.tndb_seqno   = 0;
		
		err = m_blk_device.open (
				path,
				File_Block_Device::ALLOW_CREATE|File_Block_Device::CREATE_ALWAYS,
				& tree_ver,
				0
			);
		if (KLAV_FAILED (err))
		{
			err = report_error (err, "error creating database file: %s", path);
			do_close_treedb ();
			return err;
		}

		err = do_open_treedb (false);
		if (KLAV_FAILED (err))
		{
			err = report_error (err, "error opening tree database on file: %s", path);
			do_close_treedb ();
			return err;
		}
	}
	catch (const std::exception& ex)
	{
		do_close_treedb ();
		err = report_error (KLAV_EUNKNOWN, "error (exception) opening tree database: %s", ex.what ());
	}

	return err;
}

KLAV_ERR KLAV_CALL Ntree_Linker_Impl::close ()
{
	KLAV_ERR err = KLAV_OK;

	if (! m_opened)
		return report_error (KLAV_ENOINIT, "database not opened");

	err = do_close_treedb ();

	return err;
}

KLAV_ERR Ntree_Linker_Impl::do_open_treedb (bool load)
{
	KLAV_ERR err = KLAV_OK;

	err = m_blk_mgr.open (& m_blk_device);
	if (KLAV_FAILED (err))
	{
		report_error (err, "error opening tree block manager");
		m_blk_device.close ();
		return err;
	}

	err = m_node_mgr.open (& m_blk_mgr);
	if (KLAV_FAILED (err))
	{
		report_error (err, "error opening tree node manager");
		m_blk_mgr.close ();
		m_blk_device.close ();
		return err;
	}

	if (load)
	{
		m_root_node = m_blk_mgr.get_root_node ();
		m_hash_level = m_blk_mgr.get_hash_level ();
	}
	m_opened = true;

	return KLAV_OK;
}

KLAV_ERR Ntree_Linker_Impl::do_close_treedb ()
{
	KLAV_ERR err = KLAV_OK;
	
	try
	{
		m_node_mgr.close ();
		m_blk_mgr.close ();
		m_blk_device.close ();

		m_root_node = NODEID_INVALID;
		m_hash_level = TREE_HASH_LEVEL;
		m_opened = false;
	}
	catch (const std::exception& ex)
	{
		err = report_error (KLAV_EUNKNOWN, "error (exception) closing tree database: %s", ex.what ());
	}
	
	return err;
}

KLAV_ERR KLAV_CALL Ntree_Linker_Impl::add_signature (const void *sig_data, uint32_t sig_size, uint32_t sig_id)
{
	KLAV_ERR err = KLAV_OK;

	try
	{
		m_root_node = KLAV_Tree_Merge_Signature (
				& m_node_mgr, m_root_node,
				(const uint8_t *)sig_data, sig_size, sig_id, 0, m_hash_level);
	}
	catch (const std::exception& ex)
	{
		err = report_error (KLAV_EUNKNOWN, "error merging signature: %s", ex.what ());
	}
	
	return err;
}

KLAV_ERR KLAV_CALL Ntree_Linker_Impl::del_signature (const void *sig_data, uint32_t sig_size)
{
	KLAV_ERR err = KLAV_OK;

	try
	{
		m_root_node = KLAV_Tree_Merge_Signature (
				& m_node_mgr, m_root_node,
				(const uint8_t *)sig_data, sig_size, EVID_DELETED, 0, m_hash_level);
	}
	catch (const std::exception& ex)
	{
		err = report_error (KLAV_EUNKNOWN, "error merging signature: %s", ex.what ());
	}
	
	return err;
}

KLAV_ERR KLAV_CALL Ntree_Linker_Impl::get_patches (KLAV_FlatFileBlockDiffer *patch_writer)
{
	KLAV_ERR err = KLAV_OK;

	try
	{
		m_blk_mgr.generate_patches(patch_writer);
	}
	catch (const std::exception& ex)
	{
		err = report_error (KLAV_EUNKNOWN, "error generating patches for database: %s", ex.what ());
	}

	return err;

	return KLAV_OK;
}

KLAV_ERR KLAV_CALL Ntree_Linker_Impl::commit (uint32_t *new_version, uint32_t *total_size)
{
	KLAV_ERR err = KLAV_OK;

	try
	{
		m_node_mgr.commit (m_root_node, m_hash_level);
		
		*new_version = m_blk_mgr.get_version ();
		*total_size = m_blk_mgr.get_norm_block_count () * TNS_BLK_SIZE;
	}
	catch (const std::exception& ex)
	{
		err = report_error (KLAV_EUNKNOWN, "error committing database: %s", ex.what ());
	}

	return err;
}

KLAV_ERR KLAV_CALL Ntree_Linker_Impl::get_ldb_hdr(TNS_LDB_HDR* hdr)
{
	hdr->tnld_magic[0] = TNS_LDB_MAGIC_0;
	hdr->tnld_magic[1] = TNS_LDB_MAGIC_1;
	hdr->tnld_magic[2] = TNS_LDB_MAGIC_2;
	hdr->tnld_magic[3] = TNS_LDB_MAGIC_3;

	hdr->tnld_root_node = m_root_node;
	hdr->tnld_hash_level = m_hash_level;
	hdr->reserved_1 = 0;

	return KLAV_OK;
}

/*
KLAV_ERR KLAV_CALL Ntree_Linker_Impl::rollback ()
{
	KLAV_ERR err = KLAV_OK;

	try
	{
		m_node_mgr.rollback ();
	}
	catch (const std::exception& ex)
	{
		err = report_error (KLAV_EUNKNOWN, "error rolling back database: %s", ex.what ());
	}
	
	return err;
}
*/

KLAV_ERR Ntree_Linker_Impl::report_error_v (KLAV_ERR err, const char *fmt, va_list ap)
{
	// TODO: use message reporting interface
	return err;
}

KLAV_ERR Ntree_Linker_Impl::report_error (KLAV_ERR err, const char *fmt, ...)
{
	va_list ap;
	va_start (ap, fmt);
	err = report_error_v (err, fmt, ap);
	va_end (ap);
	return err;
}

////////////////////////////////////////////////////////////////

KLAV_ERR KLAV_CALL KLAV_Ntree_Linker_Create (KLAV_Ntree_Linker **pplinker)
{
	*pplinker = new Ntree_Linker_Impl ();
	return (*pplinker == 0) ? KLAV_ENOMEM : KLAV_OK;
}

