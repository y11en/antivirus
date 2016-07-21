// tns_impl.h
//
//

#ifndef tns_impl_h_INCLUDED
#define tns_impl_h_INCLUDED

#include "tns_mgr.h"
#include <crc32.h>

////////////////////////////////////////////////////////////////
// Utility node classes

class Tree_Node_Base
{
public:
	Tree_Node_Base (TNS_Tree_Manager * mgr) :
		m_mgr (mgr),
		m_id (NODEID_INVALID),
		m_node (0),
		m_data (0),
		m_node_w (false),
		m_data_w (false)
	{
	}

	void init (TNS_Tree_Manager *mgr)
	{
		m_mgr = mgr;
		clear ();
	}

	void clear ()
	{
		m_id = NODEID_INVALID;
		m_node = 0;
		m_data = 0;
		m_node_w = false;
		m_data_w = false;
	}

protected:
	TNS_Tree_Manager * m_mgr;
	nodeid_t       m_id;
	TNS_NODE *     m_node;
	uint8_t *      m_data;
	bool           m_node_w; // node is writable
	bool           m_data_w; // data is writable
};

class Tree_Node : public Tree_Node_Base
{
public:
	Tree_Node (TNS_Tree_Manager * mgr = 0)
		: Tree_Node_Base (mgr)
	{
	}

	Tree_Node (TNS_Tree_Manager * mgr, nodeid_t id)
		: Tree_Node_Base (mgr)
	{
		m_id = id;
	}

	TNS_Tree_Manager * mgr () const
	{
		return m_mgr;
	}

	nodeid_t id () const
	{
		return m_id;
	}

	void assign (nodeid_t id)
	{
		clear ();
		m_id = id;
	}

	const TNS_NODE * node ()
	{
		if (m_node == 0)
			m_node = mgr ()->read_node (m_id, false);
		return m_node;
	}

	TNS_NODE * wnode ()
	{
		if (! m_node_w)
		{
			m_node = mgr ()->read_node (m_id, true);
			m_node_w = true;
		}
		return m_node;
	}

	nodeid_t data_ref ()
	{
		return node ()->tns_data;
	}

	const uint8_t * data ()
	{
		if (m_data == 0)
		{
			nodeid_t data_ref = node ()->tns_data;
			m_data = mgr ()->read_node_data (data_ref, false);
		}
		return m_data;
	}

	uint8_t * wdata ()
	{
		if (! m_data_w)
		{
			nodeid_t data_ref = node ()->tns_data;
			m_data = mgr ()->read_node_data (data_ref, true);
			m_data_w = true;
		}
		return m_data;
	}

	void set_data (nodeid_t data_ref)
	{
		if (node ()->tns_data != data_ref)
		{
			wnode ()->tns_data = data_ref;
			m_data = 0;
			m_data_w = false;
		}
	}

	uint32_t hdr ()
	{
		return node ()->tns_hdr;
	}

	void set_hdr (uint32_t hdr)
	{
		if (node ()->tns_hdr != hdr)
		{
			wnode ()->tns_hdr = hdr;
		}
	}

	uint32_t type ()
	{
		return TNS_NODE_TYPE (hdr ());
	}

	void set_type (uint32_t type)
	{
		set_hdr (TNS_NODE_SET_TYPE(hdr (), (uint8_t)type));
	}

	uint32_t data_size ()
	{
		return TNS_NODE_SIZE(hdr ());
	}

	uint32_t evid ()
	{
		return node ()->tns_evid;
	}

	void set_evid (uint32_t evid)
	{
		if (node ()->tns_evid != evid)
		{
			wnode ()->tns_evid = evid;
		}
	}

	nodeid_t chain ()
	{
		return node ()->tns_chain;
	}

	void set_chain (nodeid_t chain)
	{
		if (node ()->tns_chain != chain)
		{
			wnode ()->tns_chain = chain;
		}
	}

	void alloc (uint32_t flags)
	{
		clear ();

		m_node = mgr ()->alloc_node (flags, & m_id);
		m_node_w = true;
	}

	uint8_t * alloc_data (uint32_t flags, size_t size)
	{
		m_data = 0;
		m_data_w = false;

		m_data = mgr ()->alloc_node_data (flags, size, & (wnode ()->tns_data));
		m_data_w = true;

		set_hdr (TNS_NODE_SET_SIZE (hdr (), (uint32_t) size));
		return m_data;
	}

	uint8_t * realloc_data (uint32_t flags, uint32_t newsize)
	{
		nodeid_t old_ref = node ()->tns_data;

		m_data = 0;
		m_data_w = false;

		nodeid_t new_ref = NODEID_INVALID;
		m_data = mgr ()->realloc_node_data (flags, old_ref, newsize, & new_ref);
		m_data_w = true;

		set_hdr (TNS_NODE_SET_SIZE (hdr (), newsize));

		if (new_ref != old_ref)
			wnode ()->tns_data = new_ref;
		return m_data;
	}

	void delete_data ()
	{
		m_data = 0;
		m_data_w = false;

		nodeid_t data_ref = node ()->tns_data;
		if (data_ref != NODEID_INVALID)
		{
			mgr ()->free_node_data (data_ref);
			wnode ()->tns_data = NODEID_INVALID;
			set_hdr (TNS_NODE_SET_SIZE (hdr (), 0));
		}
	}

	void delete_node ()
	{
		delete_data ();
		mgr ()->free_node (m_id);
		clear ();
	}
};

////////////////////////////////////////////////////////////////

class Tree_Manager
{
public:
	Tree_Manager (
			TNS_Tree_Manager *node_mgr,
			uint32_t node_flags,
			uint32_t hash_level
		) :
		m_node_mgr (node_mgr),
		m_node_flags (node_flags),
		m_hash_level (hash_level)
		{ }


	~Tree_Manager ()
		{ }

	TNS_Tree_Manager * node_mgr () const
		{ return m_node_mgr; }

	uint32_t node_flags () const
		{ return m_node_flags; }

	bool is_temp_tree () const
		{ return ((m_node_flags & TNS_F_TEMP) != 0); }

	uint32_t hash_level () const
		{ return m_hash_level; }

	KLAV_Alloc * allocator () const
		{ return node_mgr ()->get_allocator (); }

	TNS_NODE * read_node (nodeid_t node, bool write)
		{ return m_node_mgr->read_node (node, write); }

	TNS_NODE * alloc_node (uint32_t flags, nodeid_t *pnewid)
		{ return m_node_mgr->alloc_node (flags, pnewid); }

	void       free_node (nodeid_t nid)
		{ m_node_mgr->free_node (nid); }

	uint8_t *  read_node_data (nodeid_t data_ref, bool write)
		{ return m_node_mgr->read_node_data (data_ref, write); }

	uint8_t *  alloc_node_data (uint32_t flags, uint32_t size, nodeid_t *pnewid)
		{ return m_node_mgr->alloc_node_data (flags, size, pnewid); }

	uint8_t *  realloc_node_data (uint32_t flags, nodeid_t oldid, uint32_t size, nodeid_t *pnewid)
		{ return m_node_mgr->realloc_node_data (flags, oldid, size, pnewid); }

	void       free_node_data (nodeid_t data_ref)
		{ m_node_mgr->free_node_data (data_ref); }

	// general node management

	nodeid_t node_copy (
				nodeid_t src_nid,
				uint32_t node_flags
			);

	nodeid_t node_data_copy (
				uint32_t  hdr,
				nodeid_t  src_ref,
				uint32_t  node_flags
			);

	void node_delete (
				nodeid_t nid,
				bool deep
			);

	void node_data_delete (
				uint32_t hdr,
				nodeid_t data_ref,
				bool deep
			);

	// INDEX (INDEX/ARRAY) node management

	nodeid_t index_node_create (
				uint32_t evid,
				uint32_t flags
			);

	nodeid_t index_data_copy (
				uint32_t        hdr,
				const uint8_t * src_data,
				uint32_t        flags
			);

	void index_data_clear (
				uint32_t  hdr,
				uint8_t * data,
				bool deep
			);

	nodeid_t index_get_item (
				nodeid_t nid,
				uint8_t  byte
			);

	uint8_t index_get_single_item (
				nodeid_t nid,
				nodeid_t *pitem_nid
			);

	int index_set_item (   // returns new child count
				nodeid_t nid,
				uint8_t  byte,
				nodeid_t item_nid,
				uint32_t node_flags
			);

	// LINEAR (JOINED/HASHED) node management

	struct LNode_Data
	{
		const uint8_t * m_data;
		uint32_t        m_size;
		nodeid_t        m_next;

		LNode_Data () { clear (); }

		void clear ()
		{
			m_data = 0;
			m_size = 0;
			m_next = NODEID_INVALID;
		}
	};

	void lnode_read (
				Tree_Node& node,
				LNode_Data& data
			);

	nodeid_t lnode_create (
				const LNode_Data& lnd,
				uint32_t node_flags,
				uint32_t byte_level
			);

	uint32_t lnode_prepend (
				nodeid_t nid_next,
				const uint8_t * data,
				uint32_t data_size,
				uint32_t node_flags,
				uint32_t byte_level
			);

	void lnode_strip_first_byte (
				Tree_Node& node,
				uint32_t   node_flags,
				uint32_t   byte_level
			);

	void lnode_strip_tail (
				Tree_Node& node,
				uint32_t   new_len,
				uint32_t   node_flags,
				uint32_t   byte_level
			);

	void lnode_set_next (
				Tree_Node& node,
				nodeid_t   nid_next
			);

	// JOINED node management

	nodeid_t join_data_copy (
				uint32_t        hdr,
				const uint8_t * src_data,
				uint32_t        flags
			);

	void join_data_clear (
				uint32_t   hdr,
				uint8_t *  data,
				bool       deep
			);

	// HASHED node management

	nodeid_t hash_data_copy (
				uint32_t        hdr,
				const uint8_t * src_data,
				uint32_t        flags
			);

	void hash_data_clear (
				uint32_t   hdr,
				uint8_t *  data,
				bool       deep
			);

	void hash_set_next (
				nodeid_t  nid,
				nodeid_t  next
			);

	// TAIL node management

	nodeid_t tail_node_create (
				uint32_t   evid,
				uint32_t   flags
			);

	nodeid_t tail_data_copy (
				uint32_t        hdr,
				const uint8_t * src_data,
				uint32_t        flags
			);

	void tail_data_clear (
				uint32_t   hdr,
				uint8_t *  node_data,
				bool       deep
			);

	// CONDITION node management

	nodeid_t cond_node_create (
				uint8_t  cond_type,
				uint8_t  cond_arg1,
				uint8_t  cond_arg2,
				nodeid_t next,
				uint32_t evid,
				uint32_t node_flags
			);

	nodeid_t cond16_node_create (
				uint8_t  cond_type,
				uint16_t cond_arg,
				nodeid_t next,
				uint32_t evid,
				uint32_t node_flags
			);

	void cond_set_next (
				nodeid_t  nid,
				nodeid_t  next
			);

	nodeid_t cond_data_copy (
				uint32_t        hdr,
				const uint8_t * src_data,
				uint32_t        node_flags
			);

	void cond_data_clear (
				uint32_t   hdr,
				uint8_t *  data,
				bool       deep
			);

	////////////////////////////////////////////////////////////

	void tns_trace (const char *fmt, ...);

private:
	TNS_Tree_Manager * m_node_mgr;
	uint32_t           m_node_flags;
	uint32_t           m_hash_level;
};

////////////////////////////////////////////////////////////////
// Printing

#define TREE_NID_BUF_SIZE 80

const char * Tree_NID_Format (nodeid_t nid, char *buf);

const char * Tree_Node_Type_Name (uint32_t type);

////////////////////////////////////////////////////////////////

#endif // tns_impl_h_INCLUDED

