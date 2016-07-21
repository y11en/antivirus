// tmp_blkmgr.cpp
//
//

#include "tns_mgr.h"
#include <klav_utils.h>

////////////////////////////////////////////////////////////////

Temp_Block_Manager::Temp_Block_Manager (KLAV_Alloc * alloc) :
		m_allocator (alloc),
		m_block_table (alloc)
{
}

Temp_Block_Manager::~Temp_Block_Manager ()
{
	clear_blocks ();
}

// enumerate block descriptors
KLAV_ERR KLAV_CALL Temp_Block_Manager::enum_blocks (
				TNS_Block_Enum_Callback *pcb
			)
{
	TNS_Block_Desc desc;
	uint32_t cnt = (uint32_t) m_block_table.size ();

	for (uint32_t i = 0; i < cnt; ++i)
	{
		TNS_BLK_HDR * blk = m_block_table [i];
		if (blk != 0)
		{
			desc.tnb_id = TNS_MAKE_BLKID(i,TNS_NODEKIND_TEMP);
			desc.tnb_usize = blk->tdb_unit_size - '0';
			desc.tnb_capa = tns_blk_capa (desc.tnb_usize, blk->tdb_used);

			if (! pcb->enum_blocks (1, & desc))
				return KLAV_EUNKNOWN;
		}
	}

	return KLAV_OK;
}

KLAV_ERR KLAV_CALL Temp_Block_Manager::clear_blocks ()
{
	uint32_t size = (uint32_t) m_block_table.size ();

	for (uint32_t i = 0; i < size; ++i)
	{
		TNS_BLK_HDR * blk = m_block_table [i];
		if (blk != 0)
		{
			allocator ()->free (blk);
		}
	}

	m_block_table.clear ();

	return KLAV_OK;
}

// allocate new block with the specified ID
KLAV_ERR KLAV_CALL Temp_Block_Manager::blk_alloc (
			uint32_t id,       // logical block ID (+flags)
			uint32_t usize,    // block unit size index
			TNS_BLK_HDR **pphdr
		)
{
	*pphdr = 0;

	if (TNS_BLKID_GET_KIND (id) != TNS_NODEKIND_TEMP)
		return KLAV_EINVAL; // invalid node kind

	uint32_t blk_idx = TNS_BLKID_GET_IDX (id);

	if (blk_idx >= m_block_table.size ())
	{
		if (! m_block_table.resize (blk_idx + 1))
			return KLAV_ENOMEM; // no memory
	}
	else
	{
		if (m_block_table [blk_idx] != 0)
			return KLAV_EUNKNOWN; // already occupied
	}

	if (usize >= TNS_UNIT_SIZE_CNT)
		return KLAV_EINVAL; // invalid size index

	TNS_BLK_HDR * blk = (TNS_BLK_HDR *) allocator ()->alloc (TNS_BLK_SIZE);
	if (blk == 0)
		return KLAV_ENOMEM; // no memory

	memset (blk, 0, TNS_BLK_SIZE);

	blk->tdb_magic [0] = TNS_BLK_MAGIC_0;
	blk->tdb_magic [1] = TNS_BLK_MAGIC_1;
	blk->tdb_magic [2] = TNS_BLK_MAGIC_2;
	blk->tdb_unit_size = '0' + usize;
	blk->tdb_used = 0;

	m_block_table [blk_idx] = blk;

	*pphdr = blk;
	return KLAV_OK;
}

KLAV_ERR KLAV_CALL Temp_Block_Manager::blk_free (
			uint32_t id             // block ID (+flags)
		)
{
	if (TNS_BLKID_GET_KIND (id) != TNS_NODEKIND_TEMP)
		return KLAV_EINVAL; // invalid node kind

	uint32_t blk_idx = TNS_BLKID_GET_IDX (id);

	if (blk_idx >= m_block_table.size ())
		return KLAV_EINVAL; // not allocated

	TNS_BLK_HDR * blk = m_block_table [blk_idx];
	if (blk == 0)
		return KLAV_EINVAL; // not allocated

	// TODO: assert (block is empty)

	m_block_table [blk_idx] = 0;
	allocator ()->free (blk);

	return KLAV_OK;
}

KLAV_ERR KLAV_CALL Temp_Block_Manager::blk_read (
			uint32_t id,        // block ID (+flags)
			uint32_t mode,
			TNS_BLK_HDR **pphdr
		)
{
	if (TNS_BLKID_GET_KIND (id) != TNS_NODEKIND_TEMP)
		return KLAV_EINVAL; // invalid node kind

	if (mode == TNS_BLK_READ_OLD)
		return KLAV_EINVAL;

	uint32_t blk_idx = TNS_BLKID_GET_IDX (id);

	if (blk_idx >= m_block_table.size ())
		return KLAV_EINVAL; // not allocated

	TNS_BLK_HDR * blk = m_block_table [blk_idx];
	if (blk == 0)
		return KLAV_EINVAL; // not allocated

	*pphdr = blk;
	return KLAV_OK;
}

KLAV_ERR KLAV_CALL Temp_Block_Manager::commit (const nodeid_t root_nid, uint32_t hash_level)
{
	return KLAV_OK;
}

void KLAV_CALL Temp_Block_Manager::rollback ()
{
}

KLAV_ERR KLAV_CALL Temp_Block_Manager::generate_patches (KLAV_FlatFileBlockDiffer*)
{
	return KLAV_OK;
}
