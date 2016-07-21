// tns_mgr.h
//
//

#ifndef tns_mgr_h_INCLUDED
#define tns_mgr_h_INCLUDED

#include "tns_defs.h"
#include <stdarg.h>

#include <klava/klavstl/vector.h>

////////////////////////////////////////////////////////////////
// error handling

void tns_raise_error (KLAV_ERR err, const char *info = 0);

////////////////////////////////////////////////////////////////
// checks

#define TNS_ASSERT(EXPR) if(!(EXPR)) tns_raise_error (KLAV_EUNKNOWN, "assertion failed: " #EXPR)

////////////////////////////////////////////////////////////////
// Node flags

// flag: node data is private (not exported)
#define TNS_F_PRIV     0x01
// flag: node will be created in temporary storage
#define TNS_F_TEMP     0x02
// flag: turn on tracing
#define TNS_F_TRACE    0x10

////////////////////////////////////////////////////////////////
// Block Manager interface

// Block descriptor (BLKDESC) layout
// (I - ID, T - type, C - capa, U - unit index)
// ......CC UUUU..TT IIIIIIII IIIIIIII

// block capacity (CAPA) encoding:
// block is completely occupied, no free slots
#define TNS_BLK_CAPA_0 0x00
// less than 25% slots are free
#define TNS_BLK_CAPA_1 0x01
// less 25-75% slots are free
#define TNS_BLK_CAPA_2 0x02
// more than 75% slots are free
#define TNS_BLK_CAPA_3 0x03

// NOTE: logical block ID is stored incremented by 1 (so it can never be 0)
#define TNS_BLKDESC_GET_ID(BF)   ((BF) & 0xFFFF)
#define TNS_BLKDESC_GET_KIND(BF) (((BF) >> 16) & 0x03)
#define TNS_BLKDESC_GET_UNIT(BF) (((BF) >> 20) & 0x0F)
#define TNS_BLKDESC_GET_CAPA(BF) (((BF) >> 24) & 0x03)

#define TNS_MAKE_BLKDESC(ID,KIND,UNIT,CAPA) \
	(((((((CAPA) << 4) | (UNIT)) << 4) | (KIND)) << 16) | (ID))

#define TNS_BLKDESC_SET_CAPA(DESC,CAPA) \
	(((DESC) & 0xFCFFFFFF) | ((CAPA) << 24))

// calculate capacity index
uint32_t tns_blk_capa (uint32_t unit_size_idx, uint32_t used);

////////////////////////////////////////////////////////////////

// creates BLKID from block# and block type
#define TNS_MAKE_BLKID(IDX,KIND)  ((IDX) | ((KIND) << 30))

// get block# from BLKID
#define TNS_BLKID_GET_IDX(BLKID)  ((BLKID) & 0xFFFFU)
// get node kind from BLKID
#define TNS_BLKID_GET_KIND(BLKID) (((BLKID) >> 30) & 0x03)

// logical block descriptor
struct TNS_Block_Desc
{
	uint32_t    tnb_id;       // block ID + flags
	uint16_t    tnb_usize;    // unit size index
	uint16_t    tnb_capa;     // capacity level
};

struct TNS_Block_Enum_Callback
{
	// may be called multiple times
	virtual klav_bool_t KLAV_CALL enum_blocks (
				uint32_t count,
				const TNS_Block_Desc * descs
			) = 0;
};

enum // flags for blk_read
{
	TNS_BLK_READ_RO  = 0, // read-only
	TNS_BLK_READ_RW  = 1, // read-write
	TNS_BLK_READ_OLD = 2, // read-only, old version
};

class KLAV_FlatFileBlockDiffer;

struct TNS_Block_Manager
{
	virtual KLAV_ERR KLAV_CALL commit (
				nodeid_t root_nid,
				uint32_t hash_level
			) = 0;

	virtual void KLAV_CALL rollback (
			) = 0;

	virtual KLAV_ERR KLAV_CALL generate_patches (
				KLAV_FlatFileBlockDiffer* differ
			) = 0;

	// enumerate block descriptors
	virtual KLAV_ERR KLAV_CALL enum_blocks (
				TNS_Block_Enum_Callback *pcb
			) = 0;

	// clear all blocks
	virtual KLAV_ERR KLAV_CALL clear_blocks (
			) = 0;

	// allocate new block with the specified ID
	virtual KLAV_ERR KLAV_CALL blk_alloc (
				uint32_t id,    // logical block ID (+flags)
				uint32_t usize, // block unit size index
				TNS_BLK_HDR **pphdr
			) = 0;

	// mark the block as unused
	virtual KLAV_ERR KLAV_CALL blk_free (
				uint32_t id     // block ID (+flags)
			) = 0;

	// read block data (open in read or read-write mode)
	virtual KLAV_ERR KLAV_CALL blk_read (
				uint32_t id,    // block ID (+flags)
				uint32_t mode,  // TNS_BLK_READ_XXX
				TNS_BLK_HDR **pphdr
			) = 0;
};

////////////////////////////////////////////////////////////////

struct Tree_Node_Reporter
{
	virtual void report_node (nodeid_t nid) = 0;
};

enum // node report modes
{
	TREE_NODE_REPORT_ALL  = 0,
	TREE_NODE_REPORT_USED,
	TREE_NODE_REPORT_UNUSED,
};

////////////////////////////////////////////////////////////////

class Temp_Block_Manager : public TNS_Block_Manager
{
public:
	Temp_Block_Manager (KLAV_Alloc * alloc);
	virtual ~Temp_Block_Manager ();

	virtual KLAV_ERR KLAV_CALL commit (
				nodeid_t root_nid,
				uint32_t hash_level
			);

	virtual void KLAV_CALL rollback (
			);

	virtual KLAV_ERR KLAV_CALL generate_patches (
				KLAV_FlatFileBlockDiffer*
			);

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
				uint32_t id,     // block ID (+flags)
				uint32_t mode,
				TNS_BLK_HDR **pphdr
			);

	KLAV_Alloc * allocator () const
		{ return m_allocator; }

private:
	KLAV_Alloc *  m_allocator;

	typedef klavstl::trivial_vector <TNS_BLK_HDR *, klav_allocator> Block_Table;
	Block_Table  m_block_table;
};

////////////////////////////////////////////////////////////////
// tree reader

class Tree_Node_Reader :
		public TNS_Tree_Manager
{
public:
		Tree_Node_Reader (KLAV_Alloc *alloc);
		Tree_Node_Reader (KLAV_Alloc *alloc, const void *data, size_t size);

	virtual ~Tree_Node_Reader ();

	void open (const void *data, size_t size);
	void close ();

	// TNS_Tree_Manager
	virtual void         KLAV_CALL destroy ();
	virtual KLAV_Alloc * KLAV_CALL get_allocator ();
	virtual TNS_NODE *   KLAV_CALL read_node (nodeid_t node, klav_bool_t write);
	virtual TNS_NODE *   KLAV_CALL alloc_node (uint32_t flags, nodeid_t *pnewid);
	virtual void         KLAV_CALL free_node (nodeid_t nid);
	virtual uint8_t *    KLAV_CALL read_node_data (nodeid_t data_ref, klav_bool_t write);
	virtual uint8_t *    KLAV_CALL alloc_node_data (uint32_t flags, size_t size, nodeid_t *pnewid);
	virtual uint8_t *    KLAV_CALL realloc_node_data (uint32_t flags, nodeid_t oldid, size_t size, nodeid_t *pnewid);
	virtual void         KLAV_CALL free_node_data (nodeid_t data_ref);
	virtual void         KLAV_CALL clear_tree ();
	virtual void         KLAV_CALL commit (nodeid_t root_nid, uint32_t hash_level);
	virtual void         KLAV_CALL rollback ();

	KLAV_Alloc * allocator () const
		{ return m_allocator; }

private:
	KLAV_Alloc *    m_allocator;
	const uint8_t * m_data;
	size_t          m_size;
};

////////////////////////////////////////////////////////////////

class Tree_Node_Block_Manager :
		public TNS_Tree_Manager, 
		public TNS_Block_Enum_Callback
{
public:
	Tree_Node_Block_Manager (KLAV_Alloc *alloc);
	virtual ~Tree_Node_Block_Manager ();

	KLAV_ERR open (TNS_Block_Manager * blk_mgr);
	void     close ();

	// returns number of nodes reported
	uint32_t report_nodes (Tree_Node_Reporter *reporter, unsigned int mode);

	// TNS_Tree_Manager
	virtual void         KLAV_CALL destroy ();
	virtual KLAV_Alloc * KLAV_CALL get_allocator ();
	virtual TNS_NODE *   KLAV_CALL read_node (nodeid_t node, klav_bool_t write);
	virtual TNS_NODE *   KLAV_CALL alloc_node (uint32_t flags, nodeid_t *pnewid);
	virtual void         KLAV_CALL free_node (nodeid_t nid);
	virtual uint8_t *    KLAV_CALL read_node_data (nodeid_t data_ref, klav_bool_t write);
	virtual uint8_t *    KLAV_CALL alloc_node_data (uint32_t flags, size_t size, nodeid_t *pnewid);
	virtual uint8_t *    KLAV_CALL realloc_node_data (uint32_t flags, nodeid_t oldid, size_t size, nodeid_t *pnewid);
	virtual void         KLAV_CALL free_node_data (nodeid_t data_ref);
	virtual void         KLAV_CALL clear_tree ();
	virtual void         KLAV_CALL commit (nodeid_t root_nid, uint32_t hash_level);
	virtual void         KLAV_CALL rollback ();

	// TNS_Block_Enum_Callback
	virtual klav_bool_t  KLAV_CALL enum_blocks (uint32_t count, const TNS_Block_Desc * descs);

private:
	KLAV_Alloc *         m_allocator;
	TNS_Block_Manager *  m_block_mgr;
	Temp_Block_Manager   m_temp_block_mgr;

	enum
	{
		BF_WRITE = 0x01,  // block is writable (modified)
		BF_PREV  = 0x02,  // block has previous data
		BF_VALID = 0x10,  // block is valid
	};

	struct Block_Desc;
	friend struct Block_Desc;

	struct Block_Desc
	{
		TNS_BLK_HDR * m_data;     // block data
		uint16_t      m_usize;    // unit size index
		uint8_t       m_flags;    // BF_XXX
		uint8_t       m_capa;     // capacity index (if block data is not available)

		Block_Desc () : m_data (0), m_usize (0), m_flags (0), m_capa (0) {}

		bool is_valid () const
			{ return (m_flags & BF_VALID) != 0; }

		bool is_writable () const
			{ return (m_flags & BF_WRITE) != 0; }

		void set_writable ()
			{ m_flags |= BF_WRITE; }

		void set_valid ()
			{ m_flags |= BF_VALID; }

		void clr_valid ()
			{ m_flags &= ~BF_VALID; }
	};

	typedef klavstl::trivial_vector <Block_Desc, klav_allocator> Block_Desc_Table;

	Block_Desc_Table  m_norm_blks;
	Block_Desc_Table  m_priv_blks;
	Block_Desc_Table  m_temp_blks;

	Block_Desc_Table  * m_blk_tables [4];
	TNS_Block_Manager * m_blk_mgrs [4];

	uint32_t enum_blocks (
				uint32_t node_kind,
				Tree_Node_Reporter *reporter,
				uint32_t mode
			);

	uint32_t enum_block_nodes (
				uint32_t blk_idx,
				uint32_t node_kind,
				const TNS_BLK_HDR *hdr,
				Tree_Node_Reporter *reporter,
				uint32_t mode
			);

	uint8_t * alloc_node_data_idx (uint32_t flags, uint32_t unit_size_idx, nodeid_t *pnewid);
	void do_remove_block (uint32_t blk, uint32_t blk_next, uint32_t unit_size_idx);

	Tree_Node_Block_Manager (const Tree_Node_Block_Manager&);
	Tree_Node_Block_Manager& operator= (const Tree_Node_Block_Manager&);
};

////////////////////////////////////////////////////////////////
// Tree printer interface

struct TNS_Printer
{
	virtual void print (const char * fmt, va_list ap) = 0;
	virtual void endl () = 0;
	virtual void indent (int level) = 0;
};

////////////////////////////////////////////////////////////////

enum // validation modes
{
	KLAV_TREE_VALIDATE_DEFAULT    = 0,
	KLAV_TREE_VALIDATE_MARK_USED  = 1,
	KLAV_TREE_VALIDATE_CLEAR_USED = 2,
	KLAV_TREE_VALIDATE_FINAL_NORM = 3  // validate final read-only norm tree
};

KLAV_EXTERN_C
klav_bool_t KLAV_CALL KLAV_Tree_Validate (
			TNS_Tree_Manager * node_mgr,
			nodeid_t nid_root,
			TNS_Printer * printer,
			uint32_t tree_flags,
			uint32_t hash_level,
			uint32_t mode
		);

KLAV_EXTERN_C
void KLAV_CALL KLAV_Tree_Print (
			TNS_Tree_Manager * node_mgr,
			nodeid_t nid_root,
			TNS_Printer * printer,
			klav_bool_t norm // normalized format
		);

// finders (return nonzero EVID if signature has been found)

KLAV_EXTERN_C
uint32_t KLAV_CALL KLAV_Tree_Find_Signature (
			TNS_Tree_Manager * node_mgr,
			nodeid_t nid_root,
			const uint8_t * sig_data,
			size_t sig_size
		);

// mergers
KLAV_EXTERN_C
nodeid_t KLAV_CALL KLAV_Tree_Merge (
			TNS_Tree_Manager *node_mgr,
			nodeid_t nid_root,
			nodeid_t nid_merge,
			uint32_t flags,  // TNS_F_TEMP
			uint32_t hash_level
		);

KLAV_EXTERN_C
nodeid_t KLAV_CALL KLAV_Tree_Merge_Signature (
			TNS_Tree_Manager *node_mgr,
			nodeid_t nid_root,
			const uint8_t * sig_data,
			size_t sig_size,
			uint32_t evid,
			uint32_t flags,  // TNS_F_TEMP
			uint32_t hash_level
		);

#endif // tns_mgr_h_INCLUDED

