// file_blkmgr.cpp
//
//

#include "tns_db.h"
#include <klav_utils.h>
#include "ntreelink/ntreelink.h"

////////////////////////////////////////////////////////////////

File_Block_Manager::File_Block_Manager (KLAV_Alloc * alloc) :
		m_allocator (alloc),
		m_device (0),
		m_file_hdr (0),
		m_bams (alloc),
		m_norm_blks (alloc),
		m_priv_blks (alloc),
		m_blk_cnt (0),
		m_seqno (0)
{
}

File_Block_Manager::~File_Block_Manager ()
{
	close ();
}

KLAV_ERR File_Block_Manager::open (TNS_Block_Device *dev)
{
	if (m_device != 0)
		return KLAV_EALREADY;

	if (dev == 0)
		return KLAV_EINVAL;

	KLAV_ERR err = do_open (dev, 0);
	if (KLAV_FAILED (err))
	{
		close ();
		return err;
	}

	return KLAV_OK;
}

KLAV_ERR File_Block_Manager::open_version (TNS_Block_Device *dev, uint32_t version)
{
	if (m_device != 0)
		return KLAV_EALREADY;

	if (dev == 0)
		return KLAV_EINVAL;

	KLAV_ERR err = do_open (dev, & version);
	if (KLAV_FAILED (err))
	{
		close ();
		return err;
	}

	return KLAV_OK;
}

void File_Block_Manager::close ()
{
	if (m_device != 0)
	{
		rollback_block_data (m_norm_blks);
		rollback_block_data (m_priv_blks);
		unload_bams ();
	}

	m_norm_blks.clear ();
	m_priv_blks.clear ();
	m_bams.clear ();

	m_blk_cnt = 0;
	m_seqno = 0;
	m_file_hdr = 0;
	m_device = 0;
}

KLAV_ERR File_Block_Manager::do_open (TNS_Block_Device *dev, uint32_t *pversion)
{
	KLAV_ERR err = KLAV_OK;
	m_device = dev;

	err = read_header (pversion);
	if (KLAV_FAILED (err))
		return err;

	err = load_bams ();
	if (KLAV_FAILED (err))
		return err;

	err = load_block_descs ();
	if (KLAV_FAILED (err))
		return err;

	return KLAV_OK;
}

KLAV_ERR File_Block_Manager::read_header (uint32_t *pversion)
{
	m_file_hdr = (TNS_DBF_HDR *) m_device->read_block (0);
	if (m_file_hdr == 0)
		return KLAV_EREAD; // error reading header

	m_blk_cnt = m_file_hdr->tndb_blk_cnt;
	m_seqno   = m_file_hdr->tndb_seqno;
	
	if (pversion != 0)
	{
		if (*pversion + 1 != m_seqno)
			return KLAV_EINVAL;

		m_seqno = *pversion;
	}

	return KLAV_OK;
}

////////////////////////////////////////////////////////////////

KLAV_ERR KLAV_CALL File_Block_Manager::commit (const nodeid_t root_nid, uint32_t hash_level)
{
	if (m_device == 0)
		return KLAV_ENOINIT;

	TNS_BLK_TRACE (("BLK_COMMIT_START (seqno=0x%X)\n", m_seqno));

	uint32_t seqno = m_seqno;

	KLAV_ERR err = do_commit (root_nid, hash_level);
	if (KLAV_FAILED (err))
	{
		TNS_BLK_TRACE (("BLK_COMMIT_ERROR (code=0x%X)\n", err));
		return err;
	}

	TNS_BLK_TRACE (("BLK_COMMIT_DONE (seqno=0x%X)\n", m_seqno));
	return KLAV_OK;
}

KLAV_ERR File_Block_Manager::do_commit (const nodeid_t root_node, uint32_t hash_level)
{
	KLAV_ERR err = KLAV_OK;

	err = commit_block_data (m_norm_blks, TNS_NODEKIND_NORM);
	if (KLAV_FAILED (err))
		return err;

	err = commit_block_data (m_priv_blks, TNS_NODEKIND_PRIV);
	if (KLAV_FAILED (err))
		return err;

	err = commit_bams ();
	if (KLAV_FAILED (err))
		return err;

	// fill block header and rewrite block
	m_file_hdr->tndb_blk_cnt = m_blk_cnt;
	m_file_hdr->tndb_seqno = m_seqno + 1;
	m_file_hdr->tndb_parms [BAM_NEXT ()].tndb_root_node = root_node;
	m_file_hdr->tndb_parms [BAM_NEXT ()].tndb_hash_level = hash_level;

	// write header block
	err = m_device->write_block (0);
	if (KLAV_FAILED (err))
		return err;

	m_seqno ++;
	switch_bams ();

	return KLAV_OK;
}

uint32_t File_Block_Manager::get_version () const
{
	return m_seqno;
}

nodeid_t File_Block_Manager::get_root_node () const
{
	return m_file_hdr->tndb_parms [BAM_CURR ()].tndb_root_node;
}

uint32_t File_Block_Manager::get_hash_level () const
{
	return m_file_hdr->tndb_parms [BAM_CURR ()].tndb_hash_level;
}

uint32_t File_Block_Manager::get_norm_block_count () const
{
	return (uint32_t) m_norm_blks.size ();
}

////////////////////////////////////////////////////////////////

void KLAV_CALL File_Block_Manager::rollback ()
{
	if (m_device == 0)
		return;

	TNS_BLK_TRACE (("BLK_ROLLBACK\n"));

	// rollback block data
	rollback_block_data (m_norm_blks);
	rollback_block_data (m_priv_blks);

	// rollback BAMs
	rollback_bams ();

	if (m_file_hdr != 0)
		m_device->release_block (0);

	uint32_t curr_version = m_seqno;

	KLAV_ERR err = read_header (& curr_version);
	if (KLAV_FAILED (err))
	{
		// FATAL: 
		TNS_BLK_TRACE (("BLK_ROLLBACK_ERROR (code=0x%X), closing database\n", err));
		close ();
		return;
	}

	// recalculate BAM count
	uint32_t bams_cnt = BAM_SECTS_NEEDED (m_blk_cnt);
	m_bams.resize (bams_cnt);

	// resize file, if needed
	m_device->set_size (TNS_HDR_BLKS + m_blk_cnt);
}

////////////////////////////////////////////////////////////////

KLAV_ERR KLAV_CALL File_Block_Manager::generate_patches (KLAV_FlatFileBlockDiffer* differ)
{
	KLAV_ERR err = KLAV_OK;
	size_t cnt = m_norm_blks.size ();

	for (size_t i = 0; i < cnt; ++i)
	{
		Blk_Desc& desc = m_norm_blks [i];
		
		uint32_t offset = (uint32_t) i * TNS_BLK_SIZE;
		uint32_t start = 0; // TODO

		if (desc.m_old_idx < 0 && desc.m_old_data != 0 && desc.m_new_idx < 0)
		{
			differ->diffBinBlockVersions(0, 0, TNS_BLK_SIZE - start, offset + start);
		}
		else if (desc.m_new_data != 0)
		{
			if (desc.m_old_data == 0)
			{
				// block created
				differ->diffBinBlockVersions(0, desc.m_new_data + start, TNS_BLK_SIZE - start, offset + start);
			}
			else
			{
				// block data modified
				differ->diffBinBlockVersions(desc.m_old_data + start, desc.m_new_data + start, TNS_BLK_SIZE - start, offset + start);
			}
		}
		else
		{
			if (desc.m_new_idx >= 0)
				return KLAV_EUNKNOWN; // unexpected condition
		}
	}

	return KLAV_OK;
}

////////////////////////////////////////////////////////////////

static inline int find_bdm_index (const uint8_t *bdm)
{
	for (unsigned int b = 0; b < TNS_BDM_BYTES; ++b)
	{
		if (bdm [b] != 0xFF)
		{
			unsigned char c = bdm [b];
			b = b * 8;

			if ((c & 0x0F) == 0x0F)
			{
				c >>= 4;
				b += 4;
			}
			if ((c & 0x03) == 0x03)
			{
				c >>= 2;
				b += 2;
			}

			return b + (c & 0x01);
		}
	}
	return -1;
}

int32_t File_Block_Manager::alloc_block_index ()
{
	size_t bam_cnt = m_bams.size ();

	for (size_t b = 0; b < bam_cnt; ++b)
	{
		BAM_Section& bam = m_bams [b];

		int freeidx = find_bdm_index (bam.dirty);
		if (freeidx >= 0)
		{
			int32_t freeblk = (int) b * TNS_BLKS_PER_BAM + freeidx;

			// check if block index is outside allocate area
			if ((uint32_t) freeblk >= m_blk_cnt)
			{
				// resize file
				KLAV_ERR err = m_device->set_size (TNS_HDR_BLKS + freeblk + 1);
				if (KLAV_FAILED (err))
					return -1;

				m_blk_cnt = freeblk + 1;
			}

			return freeblk;
		}
	}

	// allocate new BAM section
	int freeidx = TNS_BAM_BLKS; // first block after BAM
	int freeblk = (int) bam_cnt * TNS_BLKS_PER_BAM + freeidx;

	KLAV_ERR err = m_device->set_size (TNS_HDR_BLKS + freeblk + 1);
	if (KLAV_FAILED (err))
		return -1;

	// initialize BAM blocks
	uint32_t cur_bam_idx = TNS_HDR_BLKS + (uint32_t) bam_cnt * TNS_BLKS_PER_BAM + BAM_NEXT();
	uint32_t *cur_bam_data = (uint32_t *) m_device->read_block (cur_bam_idx);
	if (cur_bam_data == 0)
		return -1;

	memset (cur_bam_data, 0, sizeof (TNS_BLK_HDR));
	cur_bam_data [0] = 0xFFFFFFFF;
	cur_bam_data [1] = 0xFFFFFFFF;

	err = m_device->write_block (cur_bam_idx);
	if (KLAV_FAILED (err))
		return -1;

	uint32_t old_bam_idx = TNS_HDR_BLKS + (uint32_t) bam_cnt * TNS_BLKS_PER_BAM + BAM_CURR();
	uint32_t *old_bam_data = (uint32_t *) m_device->read_block (old_bam_idx);
	if (old_bam_data == 0)
		return -1;

	memset (old_bam_data, 0, sizeof (TNS_BLK_HDR));
	old_bam_data [0] = 0xFFFFFFFF;
	old_bam_data [1] = 0xFFFFFFFF;

	err = m_device->write_block (old_bam_idx);
	if (KLAV_FAILED (err))
		return -1;

	if (! m_bams.resize (bam_cnt + 1))
		return -1;

	BAM_Section& bam = m_bams [bam_cnt];
	bam.cur_desc = cur_bam_data;
	bam.old_desc = old_bam_data;

	rebuild_dirty_map ((uint32_t) bam_cnt);

	m_blk_cnt = freeblk + 1;
	return freeblk;
}

void File_Block_Manager::rebuild_dirty_map (uint32_t bam_idx)
{
	BAM_Section& bam = m_bams [bam_idx];
	memset (bam.dirty, 0, TNS_BDM_BYTES);

	uint32_t cnt = m_blk_cnt - (bam_idx * TNS_BLKS_PER_BAM);
	if (cnt > TNS_BLKS_PER_BAM)
		cnt = TNS_BLKS_PER_BAM;

	uint32_t idx = 0;
	for (idx = 0; idx < TNS_BAM_BLKS; ++idx)
	{
		BAM_BLK_SET_DIRTY (bam, idx);
	}

	for (; idx < cnt; ++idx)
	{
		uint32_t bam_desc = bam.cur_desc [idx];

		if (bam_desc != 0)
		{
			BAM_BLK_SET_DIRTY (bam, idx);
		}
	}
}

void File_Block_Manager::rebuild_dirty_maps ()
{
	uint32_t bams_cnt = (uint32_t) m_bams.size ();

	for (uint32_t b = 0; b < bams_cnt; ++b)
	{
		rebuild_dirty_map (b);
	}
}

KLAV_ERR File_Block_Manager::load_bams ()
{
	m_bams.clear ();

	uint32_t bams_cnt = BAM_SECTS_NEEDED (m_blk_cnt);
	if (! m_bams.resize (bams_cnt))
		return KLAV_ENOMEM;

	for (uint32_t b = 0; b < bams_cnt; ++b)
	{
		BAM_Section& bam = m_bams [b];
		uint32_t blk_idx = TNS_HDR_BLKS + b * TNS_BLKS_PER_BAM;
		bam.old_desc = (uint32_t *) m_device->read_block (blk_idx + BAM_CURR());
		bam.cur_desc = (uint32_t *) m_device->read_block (blk_idx + BAM_NEXT());
		if (bam.old_desc == 0 || bam.cur_desc == 0)
			return KLAV_EREAD;

		// open transaction (TODO: do it on-demand on first access)
		memcpy (bam.cur_desc, bam.old_desc, TNS_BLK_SIZE);

		rebuild_dirty_map (b);
	}

	return KLAV_OK;
}

void File_Block_Manager::unload_bams ()
{
	uint32_t cnt = (uint32_t) m_bams.size ();

	for (uint32_t b = 0; b < cnt; ++b)
	{
		BAM_Section& bam = m_bams [b];

		uint32_t blk_idx = TNS_HDR_BLKS + b * TNS_BLKS_PER_BAM;
		m_device->release_block (blk_idx);
		m_device->release_block (blk_idx + 1);

		bam.cur_desc = 0;
		bam.old_desc = 0;
	}
}

KLAV_ERR File_Block_Manager::commit_bams ()
{
	KLAV_ERR err = KLAV_OK;
	uint32_t cnt = (uint32_t) m_bams.size ();

	for (uint32_t b = 0; b < cnt; ++b)
	{
		BAM_Section& bam = m_bams [b];

		uint32_t blk_idx = TNS_HDR_BLKS + b * TNS_BLKS_PER_BAM + BAM_NEXT ();
		
		// write back current BAM block (TODO: optimize: write only if dirty?)
		err = m_device->write_block (blk_idx);
		if (KLAV_FAILED (err))
			return err;
	}

	return KLAV_OK;
}

void File_Block_Manager::switch_bams ()
{
	KLAV_ERR err = KLAV_OK;
	uint32_t cnt = (uint32_t) m_bams.size ();

	for (uint32_t b = 0; b < cnt; ++b)
	{
		BAM_Section& bam = m_bams [b];

		uint32_t * new_bam = bam.cur_desc;
		bam.cur_desc = bam.old_desc;
		bam.old_desc = new_bam;

		// open transaction (TODO: do it on-demand on first access)
		memcpy (bam.cur_desc, new_bam, TNS_BLK_SIZE);

		rebuild_dirty_map (b);
	}
}

KLAV_ERR File_Block_Manager::load_block_descs ()
{
	uint32_t cnt = (uint32_t) m_bams.size ();

	for (uint32_t b = 0; b < cnt; ++b)
	{
		BAM_Section& bam = m_bams [b];

		for (uint32_t idx = TNS_BAM_BLKS; idx < TNS_BLKS_PER_BAM; ++idx)
		{
			uint32_t blk_desc = bam.cur_desc [idx];
			if (blk_desc != 0)
			{
				uint32_t blk_id = TNS_BLKDESC_GET_ID (blk_desc) - 1;
				uint32_t blk_kind = TNS_BLKDESC_GET_KIND (blk_desc);
				blk_table *table = (blk_kind == TNS_NODEKIND_PRIV) ? & m_priv_blks : & m_norm_blks;

				if (blk_id >= table->size ())
				{
					if (! table->resize (blk_id + 1))
						return KLAV_ENOMEM;
				}

				Blk_Desc& desc = (*table) [blk_id];
				if (desc.m_old_idx >= 0)
					return KLAV_ECORRUPT;  // logical block already mapped?

				desc.m_old_idx = b * TNS_BLKS_PER_BAM + idx;
			}
		}
	}

	return KLAV_OK;
}

KLAV_ERR File_Block_Manager::commit_block_data (blk_table& table, uint32_t kind)
{
	KLAV_ERR err = KLAV_OK;
	size_t cnt = table.size ();

	for (size_t i = 0; i < cnt; ++i)
	{
		Blk_Desc& desc = table [i];
		if (desc.m_new_data != 0)
		{
			// update block information in BAM
			TNS_BLK_HDR * hdr = (TNS_BLK_HDR *) desc.m_new_data;

			BAM_Section& bam = m_bams [BAM_SECT (desc.m_new_idx)];
			uint32_t bam_idx = BAM_IDX (desc.m_new_idx);

			uint32_t unit_size_idx = hdr->tdb_unit_size - '0';

			uint32_t capa = tns_blk_capa (unit_size_idx, hdr->tdb_used);
			bam.cur_desc [bam_idx] = TNS_MAKE_BLKDESC ((uint32_t) i + 1, kind, unit_size_idx, capa);

			TNS_BLK_TRACE (("BLK_FLUSH (id=0x%X, kind=%u, usize=%u, used=%u, phys=0x%X, mem=%p)\n",
				(uint32_t) i, kind, unit_size_idx, hdr->tdb_used, desc.m_new_idx, hdr));

			err = m_device->write_block (TNS_HDR_BLKS + desc.m_new_idx);
			if (KLAV_FAILED (err))
				return err;

			m_device->release_block (TNS_HDR_BLKS + desc.m_new_idx);
			desc.m_new_data = 0;
		}

		if (desc.m_old_data != 0)
		{
			m_device->release_block (TNS_HDR_BLKS + desc.m_old_idx);
			desc.m_old_data = 0;
		}

		desc.m_old_idx = desc.m_new_idx;
		desc.m_new_idx = -1;
	}

	return KLAV_OK;
}

////////////////////////////////////////////////////////////////

void File_Block_Manager::rollback_bams ()
{
	uint32_t bams_cnt = (uint32_t) m_bams.size ();

	for (uint32_t b = 0; b < bams_cnt; ++b)
	{
		BAM_Section& bam = m_bams [b];
		memcpy (bam.cur_desc, bam.old_desc, TNS_BLK_SIZE);

		rebuild_dirty_map (b);
	}
}

void File_Block_Manager::rollback_block_data (blk_table& table)
{
	size_t cnt = table.size ();

	for (size_t i = 0; i < cnt; ++i)
	{
		Blk_Desc& desc = table [i];
		if (desc.m_new_data != 0)
		{
			m_device->release_block (TNS_HDR_BLKS + desc.m_new_idx);
			desc.m_new_data = 0;
		}

		if (desc.m_old_data != 0)
		{
			m_device->release_block (TNS_HDR_BLKS + desc.m_old_idx);
			desc.m_old_data = 0;
		}

		desc.m_new_idx = -1;
	}
}

////////////////////////////////////////////////////////////////

// enumerate block descriptors
KLAV_ERR KLAV_CALL File_Block_Manager::enum_blocks (
				TNS_Block_Enum_Callback *pcb
			)
{
	KLAV_ERR err = KLAV_OK;
	TNS_Block_Desc blk_desc;

	uint32_t bams_cnt = (uint32_t) m_bams.size ();

	for (uint32_t b = 0; b < bams_cnt; ++b)
	{
		const BAM_Section& bam_sect = m_bams [b];

		uint32_t cnt = m_blk_cnt - (b * TNS_BLKS_PER_BAM);
		if (cnt > TNS_BLKS_PER_BAM)
			cnt = TNS_BLKS_PER_BAM;

		for (uint32_t idx = TNS_BAM_BLKS; idx < cnt; ++idx)
		{
			uint32_t bam_desc = bam_sect.cur_desc [idx];

			if (bam_desc != 0)
			{
				uint32_t id   = TNS_BLKDESC_GET_ID(bam_desc) - 1;
				uint32_t kind = TNS_BLKDESC_GET_KIND(bam_desc);
				blk_desc.tnb_id    = TNS_MAKE_BLKID(id,kind);
				blk_desc.tnb_usize = TNS_BLKDESC_GET_UNIT(bam_desc);
				blk_desc.tnb_capa  = TNS_BLKDESC_GET_CAPA(bam_desc);

				if (! pcb->enum_blocks (1, & blk_desc))
					return KLAV_EUNKNOWN;
			}
		}
	}

	return KLAV_OK;
}

KLAV_ERR KLAV_CALL File_Block_Manager::clear_blocks ()
{
	KLAV_ERR err = KLAV_OK;

	if (m_device == 0)
		return KLAV_ENOINIT;

	unload_bams ();

	m_norm_blks.clear ();
	m_priv_blks.clear ();
	m_bams.clear ();

	m_blk_cnt = 0;

	// cleanup file header
	m_file_hdr->tndb_blk_cnt = 0;

	// synchronise copies of tree parameters descriptors
	m_file_hdr->tndb_parms [BAM_NEXT ()] = m_file_hdr->tndb_parms [BAM_CURR ()];
	m_file_hdr->tndb_parms [BAM_NEXT ()].tndb_root_node = NODEID_INVALID;
	m_file_hdr->tndb_seqno ++;

	// save header
	err = m_device->write_block (0);
	if (KLAV_FAILED (err))
		return err;

	// shrink file
	err = m_device->set_size (1);
	if (KLAV_FAILED (err))
		return err;

	return KLAV_OK;
}

// allocate new block with the specified ID
 KLAV_ERR KLAV_CALL File_Block_Manager::blk_alloc (
			uint32_t id,      // logical block ID (+flags)
			uint32_t usize,   // block unit size index
			TNS_BLK_HDR **pphdr
		)
{
	*pphdr = 0;

	if (! is_open ())
		return KLAV_ENOINIT;

	blk_table * table = 0;

	uint32_t kind = TNS_BLKID_GET_KIND (id);
	switch (kind)
	{
	case TNS_NODEKIND_NORM:
		table = & m_norm_blks;
		break;
	case TNS_NODEKIND_PRIV:
		table = & m_priv_blks;
		break;
	default:
		return KLAV_EINVAL; // invalid node kind
	}

	if (usize >= TNS_UNIT_SIZE_CNT)
		return KLAV_EINVAL; // invalid size index

	uint32_t blkidx = TNS_BLKID_GET_IDX (id);

	if (blkidx >= table->size ())
	{
		if (! table->resize (blkidx + 1))
			return KLAV_ENOMEM;  // not enough memory
	}

	Blk_Desc& desc = (*table) [blkidx];

	if (desc.m_old_idx >= 0 || desc.m_new_idx >= 0)
		return KLAV_EALREADY; // already allocated (error)

	desc.m_new_idx = alloc_block_index ();
	if (desc.m_new_idx < 0)
		return KLAV_EUNKNOWN;

	desc.m_new_data = m_device->read_block (TNS_HDR_BLKS + desc.m_new_idx);
	if (desc.m_new_data == 0)
		return KLAV_EREAD; // read error

	// fill BAM entry for given block
	BAM_Section& bam = m_bams [BAM_SECT (desc.m_new_idx)];
	uint32_t bam_idx = BAM_IDX (desc.m_new_idx);

	bam.cur_desc [bam_idx] = TNS_MAKE_BLKDESC (blkidx + 1, kind, usize, TNS_BLK_CAPA_3);
	BAM_BLK_SET_DIRTY (bam, bam_idx);

	// return block
	TNS_BLK_HDR * hdr = (TNS_BLK_HDR *) desc.m_new_data;

	memset (hdr, 0, TNS_BLK_SIZE);

	hdr->tdb_magic [0] = TNS_BLK_MAGIC_0;
	hdr->tdb_magic [1] = TNS_BLK_MAGIC_1;
	hdr->tdb_magic [2] = TNS_BLK_MAGIC_2;
	hdr->tdb_unit_size = '0' + usize;
	hdr->tdb_used = 0;

	TNS_BLK_TRACE (("BLK_ALLOC (id=0x%X, kind=%u, usize=%u, phys=0x%X, mem=%p)\n", blkidx, kind, usize, desc.m_new_idx, hdr));

	*pphdr = hdr;
	return KLAV_OK;
}

KLAV_ERR KLAV_CALL File_Block_Manager::blk_free (
			uint32_t id             // block ID (+flags)
		)
{
	if (! is_open ())
		return KLAV_ENOINIT;

	blk_table * table = 0;

	uint32_t kind = TNS_BLKID_GET_KIND (id);
	switch (kind)
	{
	case TNS_NODEKIND_NORM:
		table = & m_norm_blks;
		break;
	case TNS_NODEKIND_PRIV:
		table = & m_priv_blks;
		break;
	default:
		return KLAV_EINVAL;
	}

	uint32_t blkidx = TNS_BLKID_GET_IDX (id);

	if (blkidx >= table->size ())
		return KLAV_ENOTFOUND; // not present (error)

	Blk_Desc& desc = (*table) [blkidx];
	if (desc.m_new_data != 0)
	{
		m_device->release_block (TNS_HDR_BLKS + desc.m_new_idx);
		desc.m_new_data  = 0;
	}
	if (desc.m_old_data != 0)
	{
		m_device->release_block (TNS_HDR_BLKS + desc.m_old_idx);
		desc.m_old_data = 0;
	}

	TNS_BLK_TRACE (("BLK_FREE (id=0x%X, kind=%u)\n", blkidx, kind));

	if (desc.m_new_idx >= 0)
	{
		// clear BAM descriptor for this block
		BAM_Section& bam = m_bams [BAM_SECT (desc.m_new_idx)];
		uint32_t idx = desc.m_new_idx % TNS_BLKS_PER_BAM;
		bam.cur_desc [idx] = 0;

		// mark new block as free
		BAM_BLK_CLR_DIRTY (bam, idx);
		desc.m_new_idx = -1;
	}
	if (desc.m_old_idx >= 0)
	{
		// clear BAM descriptor for this block
		BAM_Section& bam = m_bams [BAM_SECT (desc.m_old_idx)];
		uint32_t idx = BAM_IDX (desc.m_old_idx);
		bam.cur_desc [idx] = 0;

		desc.m_old_idx = -1;
	}

	// shrink block table, if last block is freed
	size_t ntabsz = table->size ();
	if (blkidx == ntabsz - 1)
	{
		for (--ntabsz; ntabsz > 0; --ntabsz)
		{
			Blk_Desc& prev = (*table) [ntabsz - 1];
			if (prev.m_old_idx != -1 || prev.m_new_idx != -1)
				break;
		}
		// ntabsz == new table size
		table->resize (ntabsz);
	}

	return KLAV_OK;
}

KLAV_ERR KLAV_CALL File_Block_Manager::blk_read (
			uint32_t id,        // block ID (+flags)
			uint32_t mode,
			TNS_BLK_HDR **pphdr
		)
{
	*pphdr = 0;
	if (! is_open ())
		return KLAV_ENOINIT;

	blk_table * table = 0;

	uint32_t kind = TNS_BLKID_GET_KIND (id);
	switch (kind)
	{
	case TNS_NODEKIND_NORM:
		table = & m_norm_blks;
 		break;
	case TNS_NODEKIND_PRIV:
		table = & m_priv_blks;
		break;
	default:
		return KLAV_EINVAL; // invalid node type
	}

	uint32_t blkidx = TNS_BLKID_GET_IDX (id);

	if (blkidx >= table->size ())
		return KLAV_EINVAL; // not present

	Blk_Desc& desc = (*table) [blkidx];

	if (mode == TNS_BLK_READ_OLD)
	{
		if (desc.m_old_idx < 0)
			return KLAV_EINVAL; // no old data

		if (desc.m_old_data == 0)
		{
			desc.m_old_data = m_device->read_block (TNS_HDR_BLKS + desc.m_old_idx);
			if (desc.m_old_data == 0)
				return 0;
		}

		*pphdr = (TNS_BLK_HDR *) desc.m_old_data;
		return KLAV_OK;
	}

	if (desc.m_new_idx >= 0)
	{
		// always return 'new' version, if available
		*pphdr = (TNS_BLK_HDR *) desc.m_new_data;
		return KLAV_OK;
	}

	if (desc.m_old_idx < 0)
		return KLAV_EINVAL; // block not allocated

	if (desc.m_old_data == 0)
	{
		desc.m_old_data = m_device->read_block (TNS_HDR_BLKS + desc.m_old_idx);
		if (desc.m_old_data == 0)
			return KLAV_EREAD; // error reading block
	}

	if (mode == TNS_BLK_READ_RW)
	{
		// read-write mode: allocate m_new_idx using bitmap
		desc.m_new_idx = alloc_block_index ();
		if (desc.m_new_idx < 0)
			return KLAV_EUNKNOWN;

		// mark as dirty
		desc.m_new_data = m_device->read_block (TNS_HDR_BLKS + desc.m_new_idx);
		if (desc.m_new_data == 0)
			return KLAV_EREAD; // error reading block

		memcpy (desc.m_new_data, desc.m_old_data, TNS_BLK_SIZE);
		TNS_BLK_HDR * hdr = (TNS_BLK_HDR *) desc.m_new_data;

		// fill BAM entry for remapped block
		BAM_Section& bam = m_bams [BAM_SECT (desc.m_new_idx)];
		uint32_t bam_idx = BAM_IDX (desc.m_new_idx);

		uint32_t unit_size_idx = hdr->tdb_unit_size - '0';

		uint32_t capa = tns_blk_capa (unit_size_idx, hdr->tdb_used);
		bam.cur_desc [bam_idx] = TNS_MAKE_BLKDESC (blkidx + 1, kind, unit_size_idx, capa);
		BAM_BLK_SET_DIRTY (bam, bam_idx);

		// clear BAM entry for previous mapping (but don't clear DIRTY bit)
		if (desc.m_old_idx >= 0)
		{
			BAM_Section& bam = m_bams [BAM_SECT (desc.m_old_idx)];
			uint32_t bam_idx = BAM_IDX (desc.m_old_idx);

			bam.cur_desc [bam_idx] = 0;
		}

		*pphdr = hdr;
		return KLAV_OK;
	}

	// read-only mode
	*pphdr = (TNS_BLK_HDR *) desc.m_old_data;
	return KLAV_OK;
}

