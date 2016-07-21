// nodemgr_blk.cpp
//
// node manager, based on block mgr
//

#include "tns_mgr.h"
#include <string.h>

static void raise_error (KLAV_ERR err, const char *info)
{
	tns_raise_error (err, info);
}

////////////////////////////////////////////////////////////////

static inline unsigned int get_unit_size_idx (size_t size)
{
	unsigned int i = 1;
	for (; i < TNS_UNIT_SIZE_CNT + 1; ++i)
		if (size <= TNS_BLK_UNIT_SIZE [i])
			break;
	return i;
}

static uint32_t NODEKIND_FROM_FLAGS [4] = {
	TNS_NODEKIND_NORM,
	TNS_NODEKIND_PRIV,
	TNS_NODEKIND_TEMP,
	TNS_NODEKIND_TEMP
};

inline uint32_t nodekind_from_flags (uint32_t flags)
{
	return NODEKIND_FROM_FLAGS [flags & 0x03];
}

////////////////////////////////////////////////////////////////

Tree_Node_Block_Manager::Tree_Node_Block_Manager (
		KLAV_Alloc *alloc
	) :
		m_allocator (alloc),
		m_block_mgr (0),
		m_temp_block_mgr (alloc),
		m_norm_blks (alloc),
		m_priv_blks (alloc),
		m_temp_blks (alloc)
{
	memset (m_blk_tables, 0, sizeof (m_blk_tables));
	memset (m_blk_mgrs, 0, sizeof (m_blk_mgrs));

	m_blk_tables [TNS_NODEKIND_NORM] = & m_norm_blks;
	m_blk_tables [TNS_NODEKIND_PRIV] = & m_priv_blks;
	m_blk_tables [TNS_NODEKIND_TEMP] = & m_temp_blks;

	m_blk_mgrs [TNS_NODEKIND_TEMP] = & m_temp_block_mgr;
}

Tree_Node_Block_Manager::~Tree_Node_Block_Manager ()
{
	close ();
}

KLAV_ERR Tree_Node_Block_Manager::open (TNS_Block_Manager *block_mgr)
{
	KLAV_ERR err = KLAV_OK;

	if (m_block_mgr != 0)
		return KLAV_EALREADY;

	if (block_mgr == 0)
		return KLAV_EINVAL;

	m_block_mgr = block_mgr;

	m_blk_mgrs [TNS_NODEKIND_NORM] = m_block_mgr;
	m_blk_mgrs [TNS_NODEKIND_PRIV] = m_block_mgr;

	m_temp_block_mgr.clear_blocks ();

	// load blocks
	err = block_mgr->enum_blocks (this);
	if (KLAV_FAILED (err))
	{
		return err;
	}

	return KLAV_OK;
}

void Tree_Node_Block_Manager::close ()
{
	m_block_mgr = 0;

	m_norm_blks.clear ();
	m_priv_blks.clear ();
	m_temp_blks.clear ();

	m_temp_block_mgr.clear_blocks ();

	m_blk_mgrs [TNS_NODEKIND_NORM] = m_block_mgr;
	m_blk_mgrs [TNS_NODEKIND_PRIV] = m_block_mgr;

	m_block_mgr = 0;
}

klav_bool_t Tree_Node_Block_Manager::enum_blocks (
			uint32_t count,
			const TNS_Block_Desc * descs
		)
{
	for (uint32_t i = 0; i < count; ++i)
	{
		const TNS_Block_Desc& desc = descs [i];
		
		uint32_t idx  = TNS_BLKID_GET_IDX (desc.tnb_id);
		uint32_t kind = TNS_BLKID_GET_KIND (desc.tnb_id);

		Block_Desc_Table& table = * (m_blk_tables [kind]);

		if (idx >= table.size ())
		{
			if (! table.resize (idx + 1))
				return 0;
		}

		Block_Desc& blk = table [idx];
		if (blk.is_valid ())
			return 0; // conflict: block already used?

		blk.m_usize = desc.tnb_usize;
		blk.m_capa  = (uint8_t) desc.tnb_capa;
		blk.set_valid ();
	}

	return 1;
}

uint32_t Tree_Node_Block_Manager::enum_block_nodes (
			uint32_t blk_idx,
			uint32_t node_kind,
			const TNS_BLK_HDR *hdr,
			Tree_Node_Reporter *reporter,
			uint32_t mode
		)
{
	const uint32_t unit_size_idx = hdr->tdb_unit_size - '0';
	const uint32_t unit_size = TNS_BLK_UNIT_SIZE [unit_size_idx];
	const uint32_t unit_max = TNS_BLK_UNIT_COUNT [unit_size_idx];
	const uint8_t *item_data = ((const uint8_t *)hdr) + TNS_BLK_DATA_START [unit_size_idx];

	uint32_t node_cnt = 0;

	for (uint32_t i = 0; i < unit_max; ++i)
	{
		if (TNS_BLK_GET_BIT(hdr, i) != 0)
		{
			const TNS_NODE * node = (const TNS_NODE *) (item_data + (unit_size * i));
			bool report = false;

			switch (mode)
			{
			case TREE_NODE_REPORT_USED:
				if (TNS_NODE_FLAG (node->tns_hdr, USED) != 0)
					report = true;
				break;
			case TREE_NODE_REPORT_UNUSED:
				if (TNS_NODE_FLAG (node->tns_hdr, USED) == 0)
					report = true;
				break;
			default:
				report = true;
				break;
			}

			if (report)
			{
				nodeid_t nid = MAKE_NODEID (blk_idx + 1, i, node_kind);

				node_cnt ++;

				if (reporter != 0)
					reporter->report_node (nid);
			}
		}
	}

	return node_cnt;
}

uint32_t Tree_Node_Block_Manager::enum_blocks (
			uint32_t node_kind,
			Tree_Node_Reporter *reporter,
			uint32_t mode
		)
{
	KLAV_ERR err = KLAV_OK;
	Block_Desc_Table *table = m_blk_tables [node_kind];

	uint32_t blk_cnt = (uint32_t) table->size ();
	uint32_t cnt = 0;

	for (uint32_t i = 0; i < blk_cnt; ++i)
	{
		Block_Desc& desc = (*table) [i];

		if (desc.is_valid ())
		{
			if (desc.m_usize == 0)  // NODE type blocks
			{
				if (desc.m_data == 0)
				{
					TNS_Block_Manager * blk_mgr = m_blk_mgrs [node_kind];

					err = blk_mgr->blk_read (TNS_MAKE_BLKID (i, node_kind), TNS_BLK_READ_RO, & desc.m_data);

					if (KLAV_FAILED (err))
						raise_error (err, "error reading block data");

					cnt += enum_block_nodes (i, node_kind, desc.m_data, reporter, mode);
				}
			}
		}
	}

	return cnt;
}

uint32_t Tree_Node_Block_Manager::report_nodes (Tree_Node_Reporter *reporter, unsigned int mode)
{
	uint32_t cnt = 0;

	cnt += enum_blocks (TNS_NODEKIND_NORM, reporter, mode);
	cnt += enum_blocks (TNS_NODEKIND_PRIV, reporter, mode);
	cnt += enum_blocks (TNS_NODEKIND_TEMP, reporter, mode);

	return cnt;
}

void KLAV_CALL Tree_Node_Block_Manager::destroy ()
{
	KLAV_DELETE (this, m_allocator);
}

KLAV_Alloc * KLAV_CALL Tree_Node_Block_Manager::get_allocator ()
{
	return m_allocator;
}

TNS_NODE * KLAV_CALL Tree_Node_Block_Manager::read_node (
				nodeid_t nid,
				klav_bool_t write
			)
{
	return (TNS_NODE *) read_node_data (nid, write);
}

TNS_NODE * KLAV_CALL Tree_Node_Block_Manager::alloc_node (
				uint32_t node_flags,
				nodeid_t *pnewid
			)
{
	return (TNS_NODE *) alloc_node_data_idx (node_flags, 0, pnewid);
}

void KLAV_CALL Tree_Node_Block_Manager::free_node (nodeid_t nid)
{
	free_node_data (nid);
}

uint8_t *  KLAV_CALL Tree_Node_Block_Manager::read_node_data (
				nodeid_t data_ref,
				klav_bool_t write
			)
{
	KLAV_ERR err = KLAV_OK;
	uint32_t blk_idx = NODEID_BLK (data_ref) - 1;
	uint32_t node_kind = NODEID_KIND (data_ref);

	Block_Desc_Table * table = m_blk_tables [node_kind];
	TNS_Block_Manager * blk_mgr = m_blk_mgrs [node_kind];

	if (blk_idx >= table->size ())
		tns_raise_error (KLAV_EINVAL, "invalid block ID (block not allocated)");

	Block_Desc& desc = (*table)[blk_idx];
	if (! desc.is_valid ())
		tns_raise_error (KLAV_EINVAL, "invalid block ID (block not allocated)");

	if (desc.m_data == 0 || (write && ! desc.is_writable ()))
	{
		uint32_t mode = write ? TNS_BLK_READ_RW : TNS_BLK_READ_RO;
		err = blk_mgr->blk_read (TNS_MAKE_BLKID (blk_idx, node_kind), mode, & desc.m_data);
		if (KLAV_FAILED (err))
			tns_raise_error (err, "error reading block");

		if (write)
			desc.set_writable ();
	}

	const TNS_BLK_HDR * hdr = desc.m_data;

	uint32_t unit_size_idx = desc.m_usize;
	uint32_t unit_size = TNS_BLK_UNIT_SIZE [unit_size_idx];

	uint32_t unit_idx = NODEID_IDX (data_ref);

	// TODO: ensure node data is allocated (check bitmap)

	uint8_t * ptr = ((uint8_t *)hdr) + TNS_BLK_DATA_START[unit_size_idx] + (unit_size * unit_idx);
	return ptr;
}


uint8_t *  KLAV_CALL Tree_Node_Block_Manager::alloc_node_data (
				uint32_t node_flags,
				size_t   size,
				nodeid_t *pnewid
			)
{
	uint32_t unit_size_idx = get_unit_size_idx (size);

	uint8_t *data = alloc_node_data_idx (node_flags, unit_size_idx, pnewid);

	TNS_NODE_TRACE (("NODE_DATA_ALLOC (NEW NID: 0x%08X)\n", *pnewid));

	return data;
}

uint8_t * Tree_Node_Block_Manager::alloc_node_data_idx (
				uint32_t node_flags,
				uint32_t unit_size_idx,
				nodeid_t *pnewid
			)
{
	KLAV_ERR err = KLAV_OK;
	*pnewid = NODEID_INVALID;

	uint32_t node_kind = nodekind_from_flags (node_flags);

	Block_Desc_Table * table = m_blk_tables [node_kind];
	TNS_Block_Manager * blk_mgr = m_blk_mgrs [node_kind];

	uint32_t unit_size = TNS_BLK_UNIT_SIZE [unit_size_idx];
	uint32_t max_units = TNS_BLK_UNIT_COUNT [unit_size_idx];

	Block_Desc * desc = 0;

	// find the first block with free slots
	uint32_t blk_cnt = (uint32_t) table->size ();
	uint32_t blk_idx = 0;
	uint32_t free_idx = blk_cnt;

	for (blk_idx = 0; blk_idx < blk_cnt; ++blk_idx)
	{
		desc = & ((*table) [blk_idx]);
		if (desc->is_valid ())
		{
			if (desc->m_usize == unit_size_idx)
			{
				if (desc->m_data != 0)
				{
					if (desc->m_data->tdb_used < max_units)
						break;
				}
				else
				{
					if (desc->m_capa != TNS_BLK_CAPA_0)
						break;
				}
			}
		}
		else
		{
			if (blk_idx < free_idx)
				free_idx = blk_idx;
		}
	}

	if (blk_idx == blk_cnt)
	{
		// need to allocate new block
		if (free_idx >= blk_cnt)
		{
			if (! table->resize (free_idx + 1))
				tns_raise_error (KLAV_ENOMEM, "error allocating block descriptor");
		}

		blk_idx = free_idx;
		desc = & ((*table) [blk_idx]);

		TNS_BLK_HDR * hdr = 0;
		err = blk_mgr->blk_alloc (TNS_MAKE_BLKID (blk_idx, node_kind), unit_size_idx, & hdr);
		if (KLAV_FAILED (err))
			tns_raise_error (err, "error allocating block");

		memset (desc, 0, sizeof (Block_Desc));
		desc->m_usize = (uint16_t) unit_size_idx;
		desc->m_capa = TNS_BLK_CAPA_3;
		desc->m_flags = BF_VALID | BF_WRITE;
		desc->m_data = hdr;
	}

	// NOTE: blk_idx == block index, desc - block descriptor
	// reopen block for write, if needed
	if (! desc->is_writable ())
	{
		desc->m_data = 0;
		err = blk_mgr->blk_read (TNS_MAKE_BLKID (blk_idx, node_kind), TNS_BLK_READ_RW, & desc->m_data);
		if (KLAV_FAILED (err))
			tns_raise_error (err, "error reading block (write mode)");
	}

	// find first free slot using block bitmap

	TNS_BLK_HDR * hdr = desc->m_data;

	int node_idx = tns_blk_find_free_item (hdr);
	if (node_idx < 0)
		tns_raise_error (KLAV_EUNEXPECTED, "block is full, but should not be");

	TNS_BLK_SET_BIT (hdr, node_idx);
	hdr->tdb_used ++;

	*pnewid = MAKE_NODEID (blk_idx + 1, node_idx, node_kind);

	uint8_t* p = ((uint8_t *) hdr) + TNS_BLK_DATA_START [unit_size_idx] + node_idx * unit_size;

	return p;
}

uint8_t * KLAV_CALL Tree_Node_Block_Manager::realloc_node_data (
				uint32_t flags,
				nodeid_t oldid,
				size_t   size,
				nodeid_t *pnewid
			)
{
	KLAV_ERR err = KLAV_OK;
	*pnewid = NODEID_INVALID;

	TNS_NODE_TRACE (("NODE_DATA_REALLOC (0x%08X, NEWSIZE:0x%X)\n", oldid, size));

	if (size > TNS_UNIT_SIZE_MAX || size == 0)
		raise_error (KLAV_EINVAL, "realloc_node_data: invalid size");

	uint32_t new_unit_size_idx = get_unit_size_idx (size);
	uint32_t new_unit_size = TNS_BLK_UNIT_SIZE [new_unit_size_idx];

	if (oldid == NODEID_INVALID)
	{
		uint8_t *data = alloc_node_data_idx (flags, new_unit_size_idx, pnewid);
		TNS_NODE_TRACE (("NODE_DATA_REALLOC (NEW NID: 0x%08X)\n", *pnewid));
		return data;
	}

	uint32_t blk_idx = NODEID_BLK (oldid) - 1;
	uint32_t node_kind = NODEID_KIND (oldid);
	uint32_t node_idx = NODEID_IDX (oldid);

	Block_Desc_Table * table = m_blk_tables [node_kind];
	TNS_Block_Manager * blk_mgr = m_blk_mgrs [node_kind];

	if (blk_idx >= table->size ())
		tns_raise_error (KLAV_EINVAL, "attempt to reallocate invalid node");

	Block_Desc * desc = & ((*table) [blk_idx]);

	if (! desc->is_valid ())
		tns_raise_error (KLAV_EINVAL, "attempt to reallocate invalid node");

	// TODO: check if node is valid

	if (desc->m_data == 0 || ! desc->is_writable ())
	{
		desc->m_data = 0;
		err = blk_mgr->blk_read (TNS_MAKE_BLKID (blk_idx, node_kind), TNS_BLK_READ_RW, & (desc->m_data));
		if (KLAV_FAILED (err))
			tns_raise_error (err, "error opening block for write");
	}

	uint32_t old_unit_size = TNS_BLK_UNIT_SIZE [desc->m_usize];
	uint8_t * old_ptr = ((uint8_t *) desc->m_data) + TNS_BLK_DATA_START [desc->m_usize] + old_unit_size * node_idx;

	if (desc->m_usize == new_unit_size_idx)
	{
		// reallocate in-place
		// reuse old ref, clear tail
		if (size < old_unit_size)
			memset (old_ptr + size, 0, old_unit_size - size);

		*pnewid = oldid;
		return old_ptr;
	}

	// clear old ref, allocate new
	nodeid_t new_id = NODEID_INVALID;
	uint8_t * new_ptr = alloc_node_data_idx (flags, new_unit_size_idx, & new_id);

	// desc may be out of sync
	desc = & ((*table) [blk_idx]);

	TNS_NODE_TRACE (("NODE_DATA_REALLOC (NEW NID: 0x%08X)\n", new_id));

	size_t copysz = (size < old_unit_size) ? size : old_unit_size;
	memcpy (new_ptr, old_ptr, copysz);

	// free prev. slot
	memset (old_ptr, 0, old_unit_size);
	TNS_BLK_CLR_BIT (desc->m_data, node_idx);
	desc->m_data->tdb_used --;

	if (desc->m_data->tdb_used == 0)
	{
		// old block is empty, delete it

		err = blk_mgr->blk_free (TNS_MAKE_BLKID (blk_idx, node_kind));
		if (KLAV_FAILED (err))
			raise_error (err, "error freeing block");

		desc->clr_valid ();
		desc->m_data = 0;
	}

	*pnewid = new_id;
	return new_ptr;
}

void KLAV_CALL Tree_Node_Block_Manager::free_node_data (nodeid_t nid)
{
	KLAV_ERR err = KLAV_OK;
	TNS_NODE_TRACE (("NODE_DATA_FREE (0x%08X)\n", nid));

	if (nid == NODEID_INVALID)
		return;

	uint32_t blk_idx = NODEID_BLK (nid) - 1;
	uint32_t node_kind = NODEID_KIND (nid);

	Block_Desc_Table * table = m_blk_tables [node_kind];
	TNS_Block_Manager * blk_mgr = m_blk_mgrs [node_kind];

	if (blk_idx >= table->size ())
		raise_error (KLAV_EUNKNOWN, "invalid node index: block not allocated");

	Block_Desc& desc = (*table) [blk_idx];
	if (! desc.is_valid ())
		raise_error (KLAV_EUNKNOWN, "invalid node index: block not allocated");

	if (desc.m_data == 0 || ! desc.is_writable ())
	{
		desc.m_data = 0;
		err = blk_mgr->blk_read (TNS_MAKE_BLKID (blk_idx, node_kind), TNS_BLK_READ_RW, & desc.m_data);
		if (KLAV_FAILED (err))
			raise_error (err, "error reading block data");

		desc.set_writable ();
	}

	TNS_BLK_HDR * whdr = desc.m_data;

	uint32_t unit_size_idx = whdr->tdb_unit_size - '0';
	uint32_t unit_size = TNS_BLK_UNIT_SIZE [unit_size_idx];

	// TODO: check if the slot is allocated

	uint32_t item_idx = NODEID_IDX (nid);
	uint8_t * item_data = ((uint8_t *)whdr) + TNS_BLK_DATA_START [unit_size_idx] + (unit_size * item_idx);

	memset (item_data, 0, unit_size);
	TNS_BLK_CLR_BIT (whdr, item_idx);

	whdr->tdb_used --;

	if (whdr->tdb_used == 0)
	{
		err = blk_mgr->blk_free (TNS_MAKE_BLKID (blk_idx, node_kind));
		if (KLAV_FAILED (err))
			raise_error (err, "error freeing block");

		desc.clr_valid ();
		desc.m_data = 0;
	}
}

void KLAV_CALL Tree_Node_Block_Manager::clear_tree ()
{
	KLAV_ERR err = KLAV_OK;
	
	if (m_block_mgr == 0)
		raise_error (KLAV_ENOINIT, "error clearing tree");

	m_norm_blks.clear ();
	m_priv_blks.clear ();
	m_temp_blks.clear ();

	err = m_temp_block_mgr.clear_blocks ();
	if (KLAV_FAILED (err))
		raise_error (err, "error clearing tree");

	err = m_block_mgr->clear_blocks ();
	if (KLAV_FAILED (err))
		raise_error (err, "error clearing tree");
}

void Tree_Node_Block_Manager::commit (nodeid_t root_nid, uint32_t hash_level)
{
	KLAV_ERR err = m_block_mgr->commit (root_nid, hash_level);
	if (KLAV_FAILED (err))
		raise_error (err, "error committing tree database");

	size_t i, cnt;

	// invalidate node data pointers (block data may have been unloaded by block manager)
	for (i = 0, cnt = m_norm_blks.size (); i < cnt; ++i)
		m_norm_blks [i].m_data = 0;

	for (i = 0, cnt = m_priv_blks.size (); i < cnt; ++i)
		m_priv_blks [i].m_data = 0;

	m_temp_blks.clear ();
	m_temp_block_mgr.clear_blocks ();
}

void Tree_Node_Block_Manager::rollback ()
{
	KLAV_ERR err = KLAV_OK;
	m_block_mgr->rollback ();

	m_temp_block_mgr.clear_blocks ();

	m_norm_blks.clear ();
	m_priv_blks.clear ();
	m_temp_blks.clear ();

	// reload blocks
	err = m_block_mgr->enum_blocks (this);
	if (KLAV_FAILED (err))
	{
		tns_raise_error (err, "error loading block list");
	}
}

