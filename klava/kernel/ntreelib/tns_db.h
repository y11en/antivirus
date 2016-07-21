// tns_db.h
//
//

#ifndef tns_db_h_INCLUDED
#define tns_db_h_INCLUDED

#include "tns_mgr.h"

#include <klava/klavstl/vector.h>

////////////////////////////////////////////////////////////////

struct TNS_Block_Device
{
	// get device size, in blocks (TNS_BLOCK_SIZE)
	virtual KLAV_ERR KLAV_CALL get_size (uint32_t *pnblks) = 0;

	// set device size, in blocks
	virtual KLAV_ERR KLAV_CALL set_size (uint32_t nblks) = 0;

	// read block data
	virtual uint8_t * KLAV_CALL read_block (uint32_t blkno) = 0;

	// write block data
	virtual KLAV_ERR KLAV_CALL write_block (uint32_t blkno) = 0;

	// release block data
	virtual KLAV_ERR KLAV_CALL release_block (uint32_t blkno) = 0;

	// flush (ensure all pending data has been written)
	virtual KLAV_ERR KLAV_CALL flush () = 0;
};

////////////////////////////////////////////////////////////////
// Block database header

struct TNDB_TREE_VERSION
{
	uint32_t tndb_release;
	uint32_t tndb_seqno;
};

struct TNDB_TREE_PARMS
{
	uint32_t tndb_root_node;  // root node ID
	uint32_t tndb_hash_level; // hash level
};

struct TNS_DBF_HDR
{
	uint8_t  tndb_magic [4];   // magic ('T' 'N' 'D' 'B')
	uint8_t  tndb_ver_major;   // format version: major
	uint8_t  tndb_ver_minor;   // format version: minor
	uint8_t  tndb_endian;      // endianness
	uint8_t  tndb_flags;       // flags
	uint32_t tndb_blk_cnt;     // data block count (including BAM blocks, but excluding header block)
	uint32_t tndb_release;     // release ID (aka incarnation)
	uint32_t tndb_seqno;       // generation sequence number, low bit = active tree (BAM,INFO) index
	TNDB_TREE_PARMS tndb_parms [2]; // tree parameters for different tree states
};

#define TNS_DBF_MAGIC_0  'T'
#define TNS_DBF_MAGIC_1  'N'
#define TNS_DBF_MAGIC_2  'D'
#define TNS_DBF_MAGIC_3  'B'

#define TNS_DBF_VER_MAJOR 0x01
#define TNS_DBF_VER_MINOR 0x00

#define TNS_DBF_ENDIAN_LITTLE 0x00
#define TNS_DBF_ENDIAN_BIG    0x01

#ifdef KL_BIG_ENDIAN
# define TNS_DBF_ENDIAN_NATIVE TNS_DBF_ENDIAN_BIG
#else
# define TNS_DBF_ENDIAN_NATIVE TNS_DBF_ENDIAN_LITTLE
#endif

// linker-generated section header
struct TNS_LDB_HDR
{
	uint8_t  tnld_magic[4];   // magic ('T' 'N' 'L' 'D')
	uint32_t tnld_root_node;
	uint32_t tnld_hash_level;
	uint32_t reserved_1;
};

#define TNS_LDB_MAGIC_0  'T'
#define TNS_LDB_MAGIC_1  'N'
#define TNS_LDB_MAGIC_2  'L'
#define TNS_LDB_MAGIC_3  'D'

////////////////////////////////////////////////////////////////

// BAM/BDM (Block Allocation Map / Block Dirty Map) parameters

// Blocks-per-BAM section (1024 for 4K block)
#define TNS_BLKS_PER_BAM (TNS_BLK_SIZE / 4)

// number of blocks at header
#define TNS_HDR_BLKS 1
// number of BAM blocks at each BAM section
#define TNS_BAM_BLKS 2

// number of bytes per BDM (block dirty map)
// each block occupies one bit in BDM
#define TNS_BDM_BYTES (TNS_BLKS_PER_BAM / 8)

////////////////////////////////////////////////////////////////

class File_Block_Manager : public TNS_Block_Manager
{
public:
	File_Block_Manager (KLAV_Alloc * alloc);
	virtual ~File_Block_Manager ();

	// open database device
	KLAV_ERR open (TNS_Block_Device *dev);

	// open database syncing to the specified version #
	KLAV_ERR open_version (TNS_Block_Device *dev, uint32_t version);

	// informational functions
	// get current sequence# and root node
	uint32_t get_version () const;
	nodeid_t get_root_node () const;
	uint32_t get_hash_level () const;
	uint32_t get_norm_block_count () const;

	// deassociate with the block device
	void close ();

	KLAV_ERR KLAV_CALL commit (
				nodeid_t root_nid,
				uint32_t hash_level
			);

	void KLAV_CALL rollback (
			);

	KLAV_ERR KLAV_CALL generate_patches (
				KLAV_FlatFileBlockDiffer* differ
			);

	// TNS_Block_Manager interface

	// enumerate block descriptors
	virtual KLAV_ERR KLAV_CALL enum_blocks (
				TNS_Block_Enum_Callback *pcb
			);

	// clear all blocks
	virtual KLAV_ERR KLAV_CALL clear_blocks (
			);

	// allocate new block with the specified ID
	virtual KLAV_ERR KLAV_CALL blk_alloc (
				uint32_t id,     // logical block ID (+flags)
				uint32_t usize,  // block unit size index
				TNS_BLK_HDR **pphdr
			);

	// mark the block as unused
	virtual KLAV_ERR KLAV_CALL blk_free (
				uint32_t id      // block ID (+flags)
			);

	// read block data (open in read or read-write mode)
	virtual KLAV_ERR KLAV_CALL blk_read (
				uint32_t id,        // block ID (+flags)
				uint32_t mode,
				TNS_BLK_HDR **phdr
			);

	KLAV_Alloc * allocator () const
		{ return m_allocator; }

private:
	KLAV_Alloc       * m_allocator;
	TNS_Block_Device * m_device;

	TNS_DBF_HDR      * m_file_hdr;

	struct BAM_Section
	{
		uint32_t * cur_desc;               // current BAM block data
		uint32_t * old_desc;               // previous BAM data
		uint8_t    dirty [TNS_BDM_BYTES];  // dirty bitmap

		BAM_Section () : cur_desc (0), old_desc (0)
			{ memset (dirty, 0, sizeof (dirty)); }
	};

	typedef klavstl::trivial_vector <BAM_Section, klav_allocator> bam_table;

	bam_table     m_bams;     // BAM sections

	// block descriptor for blocks read into memory
	struct Blk_Desc
	{
		uint8_t * m_new_data; // new (writable) data
		uint8_t * m_old_data; // old (read-only) data
		int32_t   m_new_idx;  // if >= 0 - new block index
		int32_t   m_old_idx;  // if >= 0 - old block index

		Blk_Desc () :
			m_new_data (0), m_old_data (0),
			m_new_idx (-1), m_old_idx (-1) {}
	};

	typedef klavstl::trivial_vector <Blk_Desc, klav_allocator> blk_table;
	blk_table m_norm_blks;
	blk_table m_priv_blks;

	uint32_t      m_blk_cnt;  // logical block count (minus TNS_HDR_BLKS)
	uint32_t      m_seqno;    // generation sequence number

	inline bool is_open () const
		{ return m_device != 0; }

	// last (current) BAM index
	inline unsigned int BAM_CURR () const
		{ return (m_seqno & 0x1); }

	// next BAM index
	inline unsigned int BAM_NEXT () const
		{ return ((m_seqno & 0x1) ^ 0x1); }

	// BAM section for given block
	inline static unsigned int BAM_SECT (uint32_t blk)
		{ return blk / TNS_BLKS_PER_BAM; }

	// calculates how many BAM sections are required for given block count
	inline static unsigned int BAM_SECTS_NEEDED (uint32_t blk_cnt)
	    { return (blk_cnt + (TNS_BLKS_PER_BAM - 1)) / TNS_BLKS_PER_BAM; }

	// BAM offset for given block
	inline static unsigned int BAM_IDX (uint32_t blk)
		{ return blk % TNS_BLKS_PER_BAM; }

	// BAM physical block number for given section
	inline uint32_t BAM_BLK (uint32_t sect) const
		{ return TNS_HDR_BLKS + (sect * TNS_BLKS_PER_BAM) + BAM_CURR (); }

	inline static bool BAM_BLK_IS_DIRTY (const BAM_Section& bam, uint32_t idx)
		{ return (bam.dirty [idx >> 3] & (uint8_t)(1 << (idx & 0x7))) != 0; }

	inline static void BAM_BLK_SET_DIRTY (BAM_Section& bam, uint32_t idx)
		{ bam.dirty [idx >> 3] |= (uint8_t)(1 << (idx & 0x7)); }

	inline static void BAM_BLK_CLR_DIRTY (BAM_Section& bam, uint32_t idx)
		{ bam.dirty [idx >> 3] &= (uint8_t)(~(1 << (idx & 0x7))); }

	inline bool BLK_IS_DIRTY (uint32_t blk) const
		{ return BAM_BLK_IS_DIRTY (m_bams [BAM_SECT(blk)], BAM_IDX (blk)); }

	inline void BLK_SET_DIRTY (uint32_t blk)
		{ BAM_BLK_SET_DIRTY (m_bams [BAM_SECT (blk)], BAM_IDX (blk)); }

	inline void BLK_CLR_DIRTY (uint32_t blk)
		{ BAM_BLK_CLR_DIRTY (m_bams [BAM_SECT(blk)], BAM_IDX (blk)); }

	KLAV_ERR do_open (TNS_Block_Device *dev, uint32_t *pversion);
	KLAV_ERR do_commit (nodeid_t root_nid, uint32_t hash_level);

	KLAV_ERR read_header (uint32_t *pversion);

	KLAV_ERR load_bams ();
	void unload_bams ();

	KLAV_ERR commit_bams ();
	void rollback_bams ();
	void switch_bams ();

	// resizes file, if needed
	int32_t alloc_block_index ();

	KLAV_ERR load_block_descs ();

	void rebuild_dirty_map (uint32_t bam_idx);
	void rebuild_dirty_maps ();

	KLAV_ERR commit_block_data (blk_table& table, uint32_t kind);
	void rollback_block_data (blk_table& table);
};

////////////////////////////////////////////////////////////////

#include <stdio.h>

struct File_Block_Device : public TNS_Block_Device
{
public:
			File_Block_Device ();
	virtual ~File_Block_Device ();

	enum // open flags
	{
		ALLOW_CREATE  = 0x01,
		CREATE_ALWAYS = 0x02,
		OPEN_READONLY = 0x10,  // readonly access
	};

	KLAV_ERR open (
				const char * name,
				uint32_t     flags,
				const TNDB_TREE_VERSION * ver,
				const TNDB_TREE_PARMS   * parms
			);

	KLAV_ERR close ();

	KLAV_ERR KLAV_CALL get_size (uint32_t *pnblks);
	KLAV_ERR KLAV_CALL set_size (uint32_t nblks);
	uint8_t* KLAV_CALL read_block (uint32_t blkno);
	KLAV_ERR KLAV_CALL write_block (uint32_t blkno);
	KLAV_ERR KLAV_CALL release_block (uint32_t blkno);
	KLAV_ERR KLAV_CALL flush ();

private:
	struct Impl;
	Impl * m_pimpl;

	File_Block_Device (const File_Block_Device&);
	File_Block_Device& operator= (const File_Block_Device&);
};

void KLAV_CALL KLAV_Tree_File_Dump (
		TNS_Block_Device * dev,
		TNS_Printer * printer,
		klav_bool_t dump_data
	);

////////////////////////////////////////////////////////////////

#endif // tns_db_h_INCLUDED

