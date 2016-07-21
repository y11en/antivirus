// writer.cpp
//
//

#include "tns_write.h"
#include "tns_impl.h"
#include <kl_byteswap.h>

#include "../treesearchlib/internal.h"

#include <string.h>
#include <stdlib.h>

#define TREE_MAGIC_0 'T'
#define TREE_MAGIC_1 'D'
#define TREE_MAGIC_2 'B'
#define TREE_MAGIC_3 '2'

// array nodes with greater number of childs are saved as INDEX
#define TREE_ARRAY_MAX_NODE_COUNT 10

#define HASH_BYTE_CNT 4

enum
{
	NODE_REQ_DATA = 0,
	NODE_REQ_COND
};

// Request to write node data
struct Node_Req
{
	Node_Req * m_next;
	Node_Req * m_prev;
	nodeid_t   m_nid;
	uint32_t   m_offset;    // offset to node header
	uint16_t   m_type;      // NODE_REQ_XXX
	uint16_t   m_cond_word; // index of word, containing condition offset

	Node_Req (nodeid_t nid, uint32_t offset, uint16_t type, uint16_t cond_word) :
		m_next (0),
		m_prev (0),
		m_nid (nid),
		m_offset (offset),
		m_type (type),
		m_cond_word (cond_word)
	{
	}
};

// utility structure for writing INDEX/ARRAY node data

struct Index_Data
{
	TNS_Tree_Manager * m_mgr;
	uint32_t  m_count;  // number of bytes
	uint32_t  m_max_nw_size;
	uint32_t  m_tot_nw_size;

	uint8_t   m_bytes [256]; // child index bytes
	uint8_t   m_nts   [256]; // child node storage types
	Tree_Node m_nodes [256]; // child nodes

	Index_Data (TNS_Tree_Manager *mgr);

	void clear ();
	void reset ();

	void load (Tree_Node& node);
	void load_index (Tree_Node& node);
	void load_array (Tree_Node& node);

	void calc_metrics ();
};

// Write context

static uint8_t HASH_BIT_MASK [8] =
{ 0x01, 0x02, 0x04, 0x08, 0x10, 0x20, 0x40, 0x80 };

class Tree_Write_Context : public Tree_Manager
{
public:

	Tree_Write_Context (
			TNS_Tree_Manager * node_mgr,
			TNS_Writer * writer,
			TNS_Write_Options * options,
			TNS_Id_Subst* idSubst
		);

	~Tree_Write_Context ();

	void write_tree (nodeid_t nid_root);

private:
	TNS_Id_Subst*      m_idSubst;
	TNS_Writer  *      m_writer;
	TNS_Write_Options  m_options;
	uint8_t *          m_hash_table;
	uint32_t           m_hash_size;
	uint32_t           m_hash_mask;
	uint32_t           m_hash_blen;      // number of bytes used for hash
	uint8_t            m_hash_bytes [HASH_BYTE_CNT]; // bytes forming hash

	Node_Req *         m_first_nreq;
	Node_Req *         m_last_nreq;

	Index_Data         m_index_data;

	bool               m_need_swap;

	uint32_t make32 (uint32_t v)
	{
		return m_need_swap ? kl_bswap_32 (v) : v;
	}

	uint16_t make16 (uint16_t v)
	{
		return m_need_swap ? kl_bswap_16 (v) : v;
	}

	void append_data (const void * data, uint32_t size);
	void write_data (uint32_t pos, const void * data, uint32_t size);
	void read_data (uint32_t pos, void* data, uint32_t size);
	uint32_t align_data ();

	void append_word (uint32_t word)
	{
		append_data (&word, 4);
	}

	uint32_t read_word_at (uint32_t pos)
	{
		uint32_t w = 0;
		read_data (pos, &w, 4);
		return make32 (w);
	}

	void write_word_at (uint32_t pos, uint32_t word)
	{
		word = make32 (word);
		write_data (pos, &word, 4);
	}

	uint32_t data_size ()
	{
		return m_writer->size ();
	}

	void write_node (nodeid_t nid);

	void write_node_data (nodeid_t nid, uint32_t node_off);
	void write_node_cond (nodeid_t nid, uint32_t node_off, uint32_t cond_word);

	// returns actual node data type
	uint32_t write_index_node_data (Tree_Node& node);
	uint32_t write_joined_node_data (Tree_Node& node);
	uint32_t write_hashed_node_data (Tree_Node& node);

	void write_data_index32 (Tree_Node& node, Index_Data& index_data);
	void write_data_index32_fast (Tree_Node& node, Index_Data& index_data);
	void write_data_index8 (Tree_Node& node, Index_Data& index_data);
	void write_data_array  (Tree_Node& node, Index_Data& index_data);
	void write_data_array_fast (Tree_Node& node, Index_Data& index_data);

	void write_condition (Tree_Node& node);

	void build_hash (nodeid_t nid, uint32_t level);
	void build_hash_empty (uint32_t level);
	void build_hash_index (Tree_Node& node, uint32_t level);
	void build_hash_array (Tree_Node& node, uint32_t level);
	void build_hash_joined (Tree_Node& node, uint32_t level);
	void build_hash_cond (Tree_Node& node, uint32_t level);
	void build_hash_cond_mask (const TNS_COND_DATA *cond, uint32_t level);
	void build_hash_cond_range (const TNS_COND_DATA *cond, uint32_t level);
	void build_hash_cond_any (nodeid_t next, uint32_t level, uint32_t remain);

	void add_nreq_data (nodeid_t nid, uint32_t offset);
	void add_nreq_cond (nodeid_t nid, uint32_t offset, uint32_t cond_off);
	void delete_nreq (Node_Req *nreq);

	void add_nreq_to_list (Node_Req *nreq);
	void remove_nreq_from_list (Node_Req *nreq);

	void delete_all_nreqs ();

	void set_hash ()
	{
		uint16_t w1, w2;
		if (m_need_swap)
		{
			w1 = (m_hash_bytes[0] << 8) | m_hash_bytes [1];
			w2 = (m_hash_bytes[2] << 8) | m_hash_bytes [3];
		}
		else
		{
			w1 = (m_hash_bytes[1] << 8) | m_hash_bytes [0];
			w2 = (m_hash_bytes[3] << 8) | m_hash_bytes [2];
		}
		uint16_t idx = (w1 + w2) & m_hash_mask;
		m_hash_table [idx] |= HASH_BIT_MASK [m_hash_bytes [3] & 0x07];
	}
};

inline void SET_NODE_HDR_NT (uint32_t& hdr, uint32_t nt)
{
	hdr =  (hdr & 0xFFFFFF1F) | (nt << 5);
}

inline void SET_NODE_HDR_NDT (uint32_t& hdr, uint32_t ndt)
{
	hdr = (hdr & 0xFFFFFFE0) | ndt;
}

inline void SET_NODE_HDR_DATA (uint32_t& hdr, uint32_t data)
{
	hdr = (hdr & 0xFF) | (data << 8);
}

////////////////////////////////////////////////////////////////

KLAV_EXTERN_C
void KLAV_CALL KLAV_Tree_Write (
			TNS_Tree_Manager * node_mgr,
			nodeid_t nid_root,
			TNS_Writer * writer,
			TNS_Write_Options * options,
			TNS_Id_Subst* idSubst
		)
{
	// check dwHashTableSizeKb to be power of 2 <= 64
	if (options->m_hash_size != 0)
	{
		if (options->m_hash_size > 64
		 || (options->m_hash_size & (options->m_hash_size - 1)) != 0)
		{
			tns_raise_error (KLAV_EINVAL, "invalid hash table size");
		}
	}

	if (options->m_hash_level != 0)
	{
		if (options->m_hash_level < HASH_BYTE_CNT || options->m_hash_level >= 0xFF)
		{
			tns_raise_error (KLAV_EINVAL, "invalid hash level");
		}
	}

	Tree_Write_Context ctx (node_mgr, writer, options, idSubst);

	ctx.write_tree (nid_root);
}

////////////////////////////////////////////////////////////////

uint32_t get_node_storage_type (Tree_Node& node)
{
	uint32_t evid = node.evid ();

	bool has_cond = node.chain () != NODEID_INVALID;
	bool has_data = false;
	bool has_evid = evid != 0;

	switch (node.type ())
	{
	case TNT_INDEX:
	case TNT_ARRAY:
	case TNT_JOINED:
		has_data = true;
		break;
	default:
		break;
	}

	uint32_t nt = 0;

	if (has_data)
	{
		if (has_cond)
			nt = NT_Full;
		else
		{
			if (has_evid)
				nt = NT_SD;
			else
				nt = NT_D;
		}
	}
	else
	{
		if (has_cond && ! has_evid)
			nt = NT_C;
		else if (! has_cond && has_evid && evid <= 0x00FFFFFF)
			nt = NT_S;
		else
		{
			if (! has_evid && ! has_cond)
			{
				tns_raise_error (KLAV_ECORRUPT, "node is empty");
			}
			nt = NT_Full;
		}
	}

	return nt;
}

static uint32_t NT_WORDS [] = 
{
	0,  // 0: invalid
	1,  // 1: NT_S
	1,  // 2: NT_D
	2,  // 3: NT_SD
	1,  // 4: NT_C
	0,  // 5: invalid
	0,  // 6: invalid
	3,  // 7: Full
};

////////////////////////////////////////////////////////////////
// Index_Data

Index_Data::Index_Data (TNS_Tree_Manager *mgr)
	: m_mgr (mgr)
{
	reset ();
	memset (m_bytes, 0, sizeof (m_bytes));
	memset (m_nts,   0, sizeof (m_nts));
	for (int i = 0; i < 256; ++i)
		m_nodes [i].init (mgr);
}

void Index_Data::reset ()
{
	m_count = 0;
	m_max_nw_size = 0;
	m_tot_nw_size = 0;
}

void Index_Data::clear ()
{
	reset ();
	memset (m_bytes, 0, sizeof (m_bytes));
	memset (m_nts,   0, sizeof (m_nts));

	for (int i = 0; i < 256; ++i)
		m_nodes [i].clear ();
}

void Index_Data::load (Tree_Node& node)
{
	switch (node.type ())
	{
	case TNT_INDEX:
		load_index (node);
		break;
	case TNT_ARRAY:
		load_array (node);
		break;
	default:
		tns_raise_error (KLAV_ENOTIMPL, "invalid index node type");
		break;
	}
}

void Index_Data::load_index (Tree_Node& node)
{
	reset ();

	TNS_ASSERT (node.data_size () == 256 * 4);

	const nodeid_t * child_nids = (const uint32_t *) node.data ();
	for (int i = 0; i < 256; ++i)
	{
		nodeid_t child_nid = child_nids [i];
		if (child_nid != NODEID_INVALID)
		{
			m_bytes [m_count] = (uint8_t) i;
			m_nodes [m_count].assign (child_nid);
			m_count ++;
		}
	}

	calc_metrics ();
}

void Index_Data::load_array (Tree_Node& node)
{
	reset ();

	uint32_t child_cnt = node.data_size () / 5;
	uint32_t child_off = TNS_ALIGN4 (child_cnt);

	TNS_ASSERT (child_cnt > 1 && child_cnt <= 256);
	TNS_ASSERT (node.data_size () == child_off + child_cnt * 4);

	const uint8_t  * child_bytes = node.data ();
	const nodeid_t * child_nids = (const uint32_t *) (child_bytes + child_off);

	memcpy (m_bytes, child_bytes, child_cnt);

	int last_byte = -1;

	for (uint32_t i = 0; i < child_cnt; ++i)
	{
		if (child_bytes [i] <= last_byte)
			tns_raise_error (KLAV_EINVAL, "invalid array child ordering");

		m_nodes [i].assign (child_nids [i]);
	}

	m_count = child_cnt;

	calc_metrics ();
}

void Index_Data::calc_metrics ()
{
	m_max_nw_size = 1;
	m_tot_nw_size = 0;

	for (uint32_t i = 0; i < m_count; ++i)
	{
		Tree_Node& node = m_nodes [i];
		uint32_t nt = get_node_storage_type (node);
		if (nt > 8 || NT_WORDS [nt] == 0)
			tns_raise_error (KLAV_EINVAL, "invalid node storage type");

		m_nts [i] = (uint8_t) nt;

		uint32_t nw = NT_WORDS [nt];

		if (nw > m_max_nw_size)
			m_max_nw_size = nw;
		m_tot_nw_size += nw;
	}
}

////////////////////////////////////////////////////////////////

Tree_Write_Context::Tree_Write_Context (
		TNS_Tree_Manager *node_mgr,
		TNS_Writer *writer,
		TNS_Write_Options *options,
		TNS_Id_Subst* idSubst
	) : Tree_Manager (node_mgr, 0, 0),
		m_idSubst(idSubst),
		m_writer (writer),
		m_options (*options),
		m_hash_table (0),
		m_hash_size (0),
		m_hash_mask (0),
		m_hash_blen (0),
		m_first_nreq (0),
		m_last_nreq (0),
		m_index_data (node_mgr),
#ifdef KL_BIG_ENDIAN
		m_need_swap ((options->m_flags & TREE_WRITE_F_BIG_ENDIAN) == 0)
#else
		m_need_swap ((options->m_flags & TREE_WRITE_F_BIG_ENDIAN) != 0)
#endif 
{
}

Tree_Write_Context::~Tree_Write_Context ()
{
	delete_all_nreqs ();

	if (m_hash_table != 0)
	{
		allocator ()->free (m_hash_table);
		m_hash_table = 0;
	}
}

void Tree_Write_Context::append_data (const void * data, uint32_t size)
{
	if (! m_writer->append (data, size))
	{
		tns_raise_error (KLAV_EWRITE, "error writing tree data");
	}
}

void Tree_Write_Context::write_data (uint32_t pos, const void * data, uint32_t size)
{
	if (! m_writer->write (pos, data, size))
	{
		tns_raise_error (KLAV_EWRITE, "error writing tree data");
	}
}

void Tree_Write_Context::read_data (uint32_t pos, void * data, uint32_t size)
{
	if (! m_writer->read (pos, data, size))
	{
		tns_raise_error (KLAV_EREAD, "error reading tree data");
	}
}

uint32_t Tree_Write_Context::align_data ()
{
	uint32_t off = m_writer->size ();
	uint32_t aoff = TNS_ALIGN4 (off);
	if (aoff > off)
	{
		uint32_t pad = 0;
		append_data (& pad, aoff - off);
	}
	return aoff;
}

void Tree_Write_Context::write_tree (nodeid_t nid_root)
{
	TREE_INFO tree_info;
	memset (& tree_info, 0, sizeof (TREE_INFO));

	if (nid_root == NODEID_INVALID)
		tns_raise_error (KLAV_EREAD, "cannot write empty tree");

	uint32_t magic = ((((('2' << 8) + 'B') << 8) + 'D') << 8) + 'T';
	tree_info.dwMagic = make32 (magic);
	tree_info.dwFormat = make32 (cFORMAT_STATIC);

	m_hash_size = m_options.m_hash_size * 1024;
	m_hash_mask = 0;
	m_hash_blen = 0;

	tree_info.dwHashLevel = make32 (m_options.m_hash_level);
	tree_info.dwHashTableSize = make32 (m_hash_size);

	append_data (& tree_info, sizeof (TREE_INFO));

	if (m_hash_size != 0)
	{
		m_hash_table = (uint8_t *) allocator ()->alloc (m_hash_size);
		if (m_hash_table == 0)
			tns_raise_error (KLAV_ENOMEM, "not enough memory for hash table");
		memset (m_hash_table, 0, m_hash_size);

		m_hash_mask = m_hash_size - 1;
		m_hash_blen = 4;

		// build and write hash table
		build_hash (nid_root, 0);

		append_data (m_hash_table, m_hash_size);
	}

	m_index_data.clear ();

	// write root node
	write_node (nid_root);

	// iteratively write other nodes
	while (m_first_nreq != 0)
	{
		nodeid_t nid = m_first_nreq->m_nid;
		uint32_t node_off = m_first_nreq->m_offset;
		uint32_t nreq_type = m_first_nreq->m_type;
		uint32_t cond_word = m_first_nreq->m_cond_word;

		delete_nreq (m_first_nreq);

		switch (nreq_type)
		{
		case NODE_REQ_DATA:
			write_node_data (nid, node_off);
			break;
		case NODE_REQ_COND:
			write_node_cond (nid, node_off, cond_word);
			break;
		default:
			tns_raise_error (KLAV_EUNKNOWN, "unexpected nreq type");
			break;
		}
	}

	m_index_data.clear ();

	// write header
	tree_info.dwTotalSize = make32 (data_size ());
	write_data (0, & tree_info, sizeof (TREE_INFO));
}

////////////////////////////////////////////////////////////////

void Tree_Write_Context::write_node (nodeid_t nid)
{
	uint32_t node_off = data_size ();

	Tree_Node node (node_mgr (), nid);

	uint32_t evid = node.evid ();
	nodeid_t nid_cond = node.chain ();

	bool has_cond = (nid_cond != NODEID_INVALID);
	bool has_data = false;
	bool has_evid = evid != 0;

	if (has_evid && m_idSubst != 0)
		evid = m_idSubst->getIdSubst(evid);

	// TODO: how to store CONDITION?

	uint32_t nt = 0;
	uint32_t ndt = 0;
	uint32_t nw_size = 0;  // node size in words
	uint32_t cond_off = 0; // number of word to hold condition ptr

	uint32_t node_words [3];
	node_words [0] = 0;
	node_words [1] = 0;
	node_words [2] = 0;

	switch (node.type ())
	{
	case TNT_INDEX:
	case TNT_ARRAY:
	case TNT_JOINED:
	case TNT_HASHED:
		has_data = true;
		break;
	case TNT_TAIL:
		ndt = NDT_O_Tail;
		break;
	case TNT_CONDITION:   // compile as special case: TAIL with condition
		ndt = NDT_O_Tail;
		nid_cond = nid;
		has_cond = true;
		break;
	default:
		tns_raise_error (KLAV_ECORRUPT, "invalid node type");
		break;
	}

	if (has_data)
	{
		if (has_cond)
		{
			nt = NT_Full;
			nw_size = 3;
			node_words [1] = evid;
			cond_off = 2;
		}
		else
		{
			if (has_evid)
			{
				nt = NT_SD;
				nw_size = 2;
				node_words [1] = evid;
			}
			else
			{
				nt = NT_D;
				nw_size = 1;
			}
		}
	}
	else
	{
		if (has_cond && ! has_evid)
		{
			nt = NT_C;
			nw_size = 1;
			cond_off = 0;
		}
		else if (! has_cond && has_evid && evid <= 0x00FFFFFF)
		{
			nt = NT_S;
			nw_size = 1;
			SET_NODE_HDR_DATA (node_words [0], evid);
		}
		else
		{
			if (! has_evid && ! has_cond)
			{
				tns_raise_error (KLAV_ECORRUPT, "node is empty");
			}
			nt = NT_Full;
			nw_size = 3;
			node_words [1] = evid;
			cond_off = 2;
		}
	}

	SET_NODE_HDR_NT (node_words [0], nt);
	SET_NODE_HDR_NDT (node_words [0], ndt);

	if (has_cond)
	{
		add_nreq_cond (nid_cond, node_off, cond_off);
	}

	if (has_data)
	{
		add_nreq_data (nid, node_off);
	}

	// swap bytes if needed
	node_words [0] = make32 (node_words [0]);
	node_words [1] = make32 (node_words [1]);
	node_words [2] = make32 (node_words [2]);

	append_data (node_words, nw_size * 4);
}

void Tree_Write_Context::write_node_data (nodeid_t nid, uint32_t node_off)
{
	Tree_Node node (node_mgr (), nid);

	uint32_t data_off = m_writer->size ();

	uint32_t ndt = 0;

	switch (node.type ())
	{
	case TNT_INDEX:
	case TNT_ARRAY:
		ndt = write_index_node_data (node);
		break;
	case TNT_JOINED:
		ndt = write_joined_node_data (node);
		break;
	case TNT_HASHED:
		ndt = write_hashed_node_data (node);
		break;
	default:
		tns_raise_error (KLAV_ENOTIMPL, "node type not supported");
		break;
	}

	// update header (write data pointer)
	uint32_t hdr = read_word_at (node_off);
	SET_NODE_HDR_NDT (hdr, ndt);
	SET_NODE_HDR_DATA (hdr, (data_off - node_off) / 4);
	write_word_at (node_off, hdr);
}

void Tree_Write_Context::write_node_cond (nodeid_t nid, uint32_t node_off, uint32_t cond_word)
{
	Tree_Node node (node_mgr (), nid);

	uint32_t cond_off = m_writer->size ();

	switch (node.type ())
	{
	case TNT_CONDITION:
		write_condition (node);
		break;
	default:
		tns_raise_error (KLAV_EUNKNOWN, "invalid condition node type");
		break;
	}

	// write condition pointer to parent node
	uint32_t off = (cond_off - node_off) / 4;

	if (cond_word == 0)
	{
		uint32_t hdr = read_word_at (node_off);
		SET_NODE_HDR_DATA (hdr, off);
		write_word_at (node_off, hdr);
	}
	else
	{
		write_word_at (node_off + cond_word, off);
	}
}

////////////////////////////////////////////////////////////////

uint32_t Tree_Write_Context::write_index_node_data (Tree_Node& node)
{
	uint32_t ndt = 0;

	Index_Data& index_data = m_index_data;
	index_data.load (node);

	// determine node class: INDEX or ARRAY
	if (index_data.m_count > TREE_ARRAY_MAX_NODE_COUNT)
	{
		if (index_data.m_max_nw_size == 1)
		{
			write_data_index32_fast (node, index_data);
			ndt = NDT_O_Index32Fast;
		}
		else if (index_data.m_tot_nw_size <= 0xFF)
		{
			write_data_index8 (node, index_data);
			ndt = NDT_O_Index8;
		}
		else
		{
			write_data_index32 (node, index_data);
			ndt = NDT_O_Index32;
		}
	}
	else
	{
		if (index_data.m_max_nw_size == 1)
		{
			write_data_array_fast (node, index_data);
			ndt = NDT_O_ArrayFast;
		}
		else
		{
			write_data_array (node, index_data);
			ndt = NDT_O_Array;
		}
	}

	return ndt;
}

void Tree_Write_Context::write_data_index32 (Tree_Node& node, Index_Data& index_data)
{
	uint32_t child_offs [256]; // index node offsets

	uint32_t node_off = 1; // starting child node offset (in words, starting with 1)

	uint32_t beg_off = data_size ();

	// fill index structure
	memset (child_offs, 0, sizeof (child_offs));

	uint32_t i, cnt = index_data.m_count;
	for (i = 0; i < cnt; ++i)
	{
		uint32_t b = index_data.m_bytes [i];
		child_offs [b] = make32 (node_off);
		
		uint32_t nw = NT_WORDS [index_data.m_nts [i]];
		node_off += nw;
	}

	append_data (child_offs, sizeof (child_offs));

	for (i = 0; i < cnt; ++i)
	{
		write_node (index_data.m_nodes [i].id ());
	}

	TNS_ASSERT (data_size () - beg_off == (0x100 + node_off) * 4);
}

void Tree_Write_Context::write_data_index32_fast (Tree_Node& node, Index_Data& index_data)
{
	uint32_t zero [256];
	memset (zero, 0, sizeof (zero));

	uint32_t beg_off = data_size ();

	uint32_t cnt = index_data.m_count;
	uint32_t off = 0;
	for (uint32_t i = 0; i < cnt; ++i)
	{
		uint32_t b = index_data.m_bytes [i];
		uint32_t dif = b - off;
		if (b > off)
		{
			append_data (zero, (b - off) * 4);
			off = b;
		}
		write_node (index_data.m_nodes [i].id ());
		off ++;
	}
	if (off < 256)
		append_data (zero, (256 - off) * 4);

	TNS_ASSERT ((data_size () - beg_off) == 256 * 4);
}

void Tree_Write_Context::write_data_index8 (Tree_Node& node, Index_Data& index_data)
{
	uint8_t child_offs [256]; // index node offsets

	uint32_t child_off = 1; // starting child node offset (in words, starting with 1)

	uint32_t beg_off = data_size ();

	// fill index structure
	memset (child_offs, 0, sizeof (child_offs));

	uint32_t i, cnt = index_data.m_count;
	for (i = 0; i < cnt; ++i)
	{
		uint32_t b = index_data.m_bytes [i];

		TNS_ASSERT (child_off <= 0xFF);
		child_offs [b] = (uint8_t) child_off;
		
		uint32_t nw = NT_WORDS [index_data.m_nts [i]];
		child_off += nw;
	}

	append_data (child_offs, sizeof (child_offs));

	for (i = 0; i < cnt; ++i)
	{
		write_node (index_data.m_nodes [i].id ());
	}

	TNS_ASSERT (data_size () - beg_off == 0xFC + child_off * 4);
}

void Tree_Write_Context::write_data_array  (Tree_Node& node, Index_Data& index_data)
{
	uint8_t bytes [0x210];  // byte values + offsets

	uint32_t beg_off = data_size ();

	uint32_t i, cnt = index_data.m_count;

	bytes [0] = (uint8_t) cnt;
	memcpy (bytes + 1, index_data.m_bytes, cnt);

	uint32_t tot_bytes = cnt + cnt + 1;
	bytes [tot_bytes] = 0;
	bytes [tot_bytes + 1] = 0;
	bytes [tot_bytes + 2] = 0;

	uint32_t hdr_len = TNS_ALIGN4 (tot_bytes);

	uint32_t child_off = hdr_len / 4;

	for (i = 0; i < cnt; ++i)
	{
		TNS_ASSERT (child_off <= 0xFF);

		uint32_t nw = NT_WORDS [index_data.m_nts [i]];
		child_off += nw;
	}

	append_data (bytes, hdr_len);

	for (i = 0; i < cnt; ++i)
	{
		write_node (index_data.m_nodes [i].id ());
	}

	TNS_ASSERT (data_size () - beg_off == child_off * 4);
}

void Tree_Write_Context::write_data_array_fast (Tree_Node& node, Index_Data& index_data)
{
	uint8_t bytes [0x110];

	uint32_t beg_off = data_size ();

	uint32_t i, cnt = index_data.m_count;

	bytes [0] = (uint8_t) cnt;
	memcpy (bytes + 1, index_data.m_bytes, cnt);

	uint32_t hdr_len = TNS_ALIGN4 (cnt + 1);
	bytes [cnt + 1] = 0;
	bytes [cnt + 2] = 0;
	bytes [cnt + 3] = 0;

	append_data (bytes, hdr_len);

	for (i = 0; i < cnt; ++i)
	{
		write_node (index_data.m_nodes [i].id ());
	}

	TNS_ASSERT (data_size () - beg_off == hdr_len + cnt * 4);
}

uint32_t Tree_Write_Context::write_joined_node_data (Tree_Node& node)
{
	uint32_t ndt = 0;

	// access node data
	uint32_t data_size = node.data_size ();
	const TNS_JOIN_DATA * jh = (const TNS_JOIN_DATA *) node.data ();

	const uint8_t *data = (const uint8_t *)jh + sizeof (TNS_JOIN_DATA);
	uint32_t len = data_size - sizeof (TNS_JOIN_DATA);

	if (len > 0xFF)
	{
		// save JOIN32 node
		// TODO: optimize (avoid writing byte-by-byte)
		ndt = NDT_O_Joined32;
		uint32_t wlen = make32 (len);
		append_data (& wlen, 4);
		for (uint32_t i = 0; i < len; ++i)
		{
			uint8_t b = data [i] ^ XOR_DATA_MASK;
			append_data (& b, 1);
		}
		align_data ();
		write_node (jh->ndj_next);
	}
	else
	{
		// save JOIN8 node
		// TODO: optimize
		ndt = NDT_O_Joined8;
		uint8_t b = (uint8_t) len;
		append_data (& b, 1);
		for (uint32_t i = 0; i < len; ++i)
		{
			b = data [i] ^ XOR_DATA_MASK;
			append_data (& b, 1);
		}
		align_data ();
		write_node (jh->ndj_next);
	}

	return ndt;
}

uint32_t Tree_Write_Context::write_hashed_node_data (Tree_Node& node)
{
	uint32_t ndt = 0;

	// access node data
	const TNS_HASH_DATA * hh = (const TNS_HASH_DATA *) node.data ();

	ndt = NDT_O_Hashed;

	struct HASH_NODE_DATA
	{
		uint32_t size;
		uint32_t hash;
	} hash_node_data;

	hash_node_data.size = make32 (hh->ndh_len);
	hash_node_data.hash = make32 (hh->ndh_hash);

	append_data (& hash_node_data, sizeof (hash_node_data));

	return ndt;
}

////////////////////////////////////////////////////////////////

void Tree_Write_Context::write_condition (Tree_Node& node)
{
	const TNS_COND_DATA * cond_data = (const TNS_COND_DATA *) node.data ();

	uint32_t cond_words [3];
	uint32_t cond_size = 3; // by default
	uint32_t chain_off = 2; // by default
	bool no_chain = false;

	switch (cond_data->ndc_type)
	{
	case TNS_COND_MASK:
		cond_words [0] = cCT_BitMaskAnd;
		cond_words [1] = (((uint32_t) cond_data->ndc_parm2) << 8) | cond_data->ndc_parm1;
		break;
	case TNS_COND_RANGE:
		cond_words [0] = cCT_Range;
		cond_words [1] = (((uint32_t) cond_data->ndc_parm2) << 8) | cond_data->ndc_parm1;
		break;
	case TNS_COND_ANY:
		if (cond_data->ndc_parm1 == 0 && cond_data->ndc_parm2 == 0)
			tns_raise_error (KLAV_EINVAL, "invalid \'??\' count");

		cond_words [0] = (((((uint32_t) cond_data->ndc_parm2) << 8) | cond_data->ndc_parm1) << 8) | (uint8_t) cCT_AnyByte;
		cond_size = 1;
		no_chain = true;
		break;
	case TNS_COND_STAR:
		if (cond_data->ndc_parm1 == 0 && cond_data->ndc_parm2 == 0)
			tns_raise_error (KLAV_EINVAL, "invalid \'*\' count");

		cond_words [0] = (((((uint32_t) cond_data->ndc_parm2) << 8) | cond_data->ndc_parm1) << 8) | (uint8_t) cCT_Aster;
		cond_size = 1;
		no_chain = true;
		break;
	default:
		tns_raise_error (KLAV_EINVAL, "invalid condition type");
		break;
	}

	cond_words [0] = make32 (cond_words [0]);
	cond_words [1] = make32 (cond_words [1]);
	cond_words [2] = 0;

	uint32_t cond_off = data_size ();

	append_data (& cond_words, cond_size * sizeof (uint32_t));
	write_node (cond_data->ndc_next);

	nodeid_t nid_chain = node.chain ();
	if (nid_chain != NODEID_INVALID)
	{
		if (no_chain)
			tns_raise_error (KLAV_EINVAL, "cannot write condition chain");

		add_nreq_cond (nid_chain, cond_off, chain_off);
	}
}

////////////////////////////////////////////////////////////////

void Tree_Write_Context::build_hash (nodeid_t nid, uint32_t level)
{
	if (level >= m_hash_blen)
	{
		set_hash ();
		return;
	}

	if (nid == NODEID_INVALID)
	{
		if (level != 0)
			build_hash_empty (level);
		return;
	}

	Tree_Node node (node_mgr (), nid);

	if (node.evid () != 0)
	{
		build_hash_cond_any (NODEID_INVALID, level, HASH_BYTE_CNT);
		return;
	}

	switch (node.type ())
	{
	case TNT_INDEX:
		build_hash_index (node, level);
		break;
	case TNT_ARRAY:
		build_hash_array (node, level);
		break;
	case TNT_JOINED:
		build_hash_joined (node, level);
		break;
	case TNT_HASHED:
		build_hash_cond_any (NODEID_INVALID, level, HASH_BYTE_CNT);
		break;
	case TNT_TAIL:
		build_hash_empty (level);
		break;
	case TNT_CONDITION:
		build_hash_cond (node, level);
		break;
	default:
		tns_raise_error (KLAV_EINVAL, "invalid node type");
		break;
	}

	// handle condition chains
	nodeid_t nid_chain = node.chain ();
	while (nid_chain != NODEID_INVALID)
	{
		node.assign (nid_chain);

		if (node.type () != TNT_CONDITION)
			tns_raise_error (KLAV_EINVAL, "invalid node type");

		build_hash_cond (node, level);

		nid_chain = node.chain ();
	}
}

void Tree_Write_Context::build_hash_empty (uint32_t level)
{
	if (level + 1 >= m_hash_blen)
	{
		for (int i = 0; i < 256; ++i)
		{
			m_hash_bytes [level] = (uint8_t) i;
			set_hash ();
		}
	}
	else
	{
		for (int i = 0; i < 256; ++i)
		{
			m_hash_bytes [level] = (uint8_t) i;
			build_hash_empty (level + 1);
		}
	}
}

void Tree_Write_Context::build_hash_index (Tree_Node& node, uint32_t level)
{
	const nodeid_t * child_nids = (const uint32_t *) node.data ();
	for (int i = 0; i < 256; ++i)
	{
		nodeid_t child_nid = child_nids [i];
		if (child_nid != NODEID_INVALID)
		{
			m_hash_bytes [level] = (uint8_t) i;
			build_hash (child_nid, level + 1);
		}
	}
}

void Tree_Write_Context::build_hash_array (Tree_Node& node, uint32_t level)
{
	uint32_t child_cnt = node.data_size () / 5;
	uint32_t child_off = TNS_ALIGN4 (child_cnt);

	const uint8_t  * child_bytes = node.data ();
	const nodeid_t * child_nids = (const uint32_t *) (child_bytes + child_off);

	for (uint32_t i = 0; i < child_cnt; ++i)
	{
		m_hash_bytes [level] = child_bytes [i];
		build_hash (child_nids [i], level + 1);
	}
}

void Tree_Write_Context::build_hash_joined (Tree_Node& node, uint32_t level)
{
	uint32_t data_size = node.data_size ();
	const TNS_JOIN_DATA * jh = (const TNS_JOIN_DATA *) node.data ();

	const uint8_t *data = (const uint8_t *)jh + sizeof (TNS_JOIN_DATA);
	uint32_t len = data_size - sizeof (TNS_JOIN_DATA);

	uint32_t i, cnt = m_hash_blen - level;
	if (cnt > len)
		cnt = len; 

	for (i = 0; i < cnt; ++i)
	{
		m_hash_bytes [level++] = data [i];
	}
	if (level >= m_hash_blen)
	{
		set_hash ();
	}
	else
	{
		build_hash (jh->ndj_next, level);
	}
}

void Tree_Write_Context::build_hash_cond (Tree_Node& node, uint32_t level)
{
	const TNS_COND_DATA * cond = (const TNS_COND_DATA *) node.data ();

	switch (cond->ndc_type)
	{
	case TNS_COND_MASK:
		build_hash_cond_mask (cond, level);
		break;
	case TNS_COND_RANGE:
		build_hash_cond_range (cond, level);
		break;
	case TNS_COND_ANY:
		build_hash_cond_any (cond->ndc_next, level, cond->ndc_parm2);
		break;
	default:
		tns_raise_error (KLAV_EINVAL, "invalid condition type");
		break;
	}
}

void Tree_Write_Context::build_hash_cond_mask (const TNS_COND_DATA *cond, uint32_t level)
{
	uint8_t bits [8];
	unsigned int nbits = 0;
	unsigned int i, j, cnt;
	uint8_t mask = cond->ndc_parm1;
	uint8_t val = cond->ndc_parm2;

	mask = ~mask; // for AND operation

	for (i = 0; i < 8; ++i)
	{
		if ((mask & (1 << i)) != 0)
			bits [nbits++] = (1 << i);
	}

	cnt = 1 << nbits;
	for (i = 0; i < cnt; ++i)
	{
		uint8_t b = val & ~mask;
		for (j = 0; j < nbits; ++j)
		{
			if ((i & (1 << j)) != 0)
				b |= bits [j];
		}
		m_hash_bytes [level] = b;
		build_hash (cond->ndc_next, level + 1);
	}
}

void Tree_Write_Context::build_hash_cond_range (const TNS_COND_DATA *cond, uint32_t level)
{
	unsigned int rb = cond->ndc_parm1;
	unsigned int re = cond->ndc_parm2;

	for (; rb <= re; ++rb)
	{
		m_hash_bytes [level] = (uint8_t) rb;
		build_hash (cond->ndc_next, level + 1);
	}
}

void Tree_Write_Context::build_hash_cond_any (nodeid_t next, uint32_t level, uint32_t remain)
{
	if (remain == 0)
	{
		build_hash (next, level);
		return;
	}

	for (unsigned int i = 0; i < 256; ++i)
	{
		m_hash_bytes [level] = (uint8_t) i;
		if (remain == 1)
			build_hash (next, level + 1);
		else
			build_hash_cond_any (next, level + 1, remain - 1);
	}
}

////////////////////////////////////////////////////////////////

void Tree_Write_Context::add_nreq_to_list (Node_Req *nreq)
{
	nreq->m_next = 0;
	nreq->m_prev = m_last_nreq;

	if (m_last_nreq != 0)
		m_last_nreq->m_next = nreq;
	else
		m_first_nreq = nreq;

	m_last_nreq = nreq;
}

void Tree_Write_Context::remove_nreq_from_list (Node_Req *nreq)
{
	if (nreq->m_next != 0)
		nreq->m_next->m_prev = nreq->m_prev;
	else
		m_last_nreq = nreq->m_prev;

	if (nreq->m_prev != 0)
		nreq->m_prev->m_next = nreq->m_next;
	else
		m_first_nreq = nreq->m_next;
}

void Tree_Write_Context::delete_all_nreqs ()
{
	while (m_first_nreq != 0)
	{
		Node_Req *nreq = m_first_nreq;
		m_first_nreq = nreq->m_next;

		delete nreq;
	}
	m_last_nreq = 0;
}

void Tree_Write_Context::add_nreq_data (nodeid_t nid, uint32_t offset)
{
	Node_Req * nreq = new Node_Req (nid, offset, NODE_REQ_DATA, 0);
	add_nreq_to_list (nreq);
}

void Tree_Write_Context::add_nreq_cond (nodeid_t nid, uint32_t offset, uint32_t cond_word)
{
	Node_Req * nreq = new Node_Req (nid, offset, NODE_REQ_COND, (uint16_t) cond_word);
	add_nreq_to_list (nreq);
}

void Tree_Write_Context::delete_nreq (Node_Req *nreq)
{
	if (nreq != 0)
	{
		remove_nreq_from_list (nreq);
		delete nreq;
	}
}

