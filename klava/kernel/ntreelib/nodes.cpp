// nodes.cpp
//
//

#include "tns_impl.h"
#include <string.h>

////////////////////////////////////////////////////////////////


nodeid_t Tree_Manager::node_copy (nodeid_t src_nid, uint32_t node_flags)
{
	if (src_nid == NODEID_INVALID)
		return NODEID_INVALID;

	Tree_Node src_node (node_mgr (), src_nid);
	Tree_Node dst_node (node_mgr ());

	uint32_t evid = src_node.evid ();
	if (evid == EVID_DELETED && ! is_temp_tree ())
	{
		tns_raise_error (KLAV_ENOTFOUND, "signature to be deleted is not present");
	}

	dst_node.alloc (node_flags);
	TNS_TRACE (("NODE_COPY (0x%08X -> 0x%08X)\n", src_nid, dst_node.id ()));

	dst_node.set_hdr (src_node.hdr ());
	dst_node.set_evid (evid);

	nodeid_t data_copy_ref = node_data_copy (src_node.hdr (), src_node.data_ref (), node_flags);
	dst_node.set_data (data_copy_ref);

	dst_node.set_chain (0);

	// copy condition chain from src to dst
	nodeid_t chain_nid = src_node.chain ();
	nodeid_t prev_nid = dst_node.id ();

	while (chain_nid != NODEID_INVALID)
	{
		Tree_Node cond (node_mgr (), chain_nid);
		nodeid_t next = cond.chain ();

		nodeid_t cond_copy_nid = node_copy (chain_nid, node_flags);

		Tree_Node prev_node (node_mgr (), prev_nid);
		prev_node.set_chain (cond_copy_nid);

		chain_nid = next;
		prev_nid = cond_copy_nid;
	}

	return dst_node.id ();
}

nodeid_t Tree_Manager::node_data_copy (
			uint32_t   hdr,
			nodeid_t   srcid,
			uint32_t   node_flags
		)
{
	if (srcid == NODEID_INVALID)
		return NODEID_INVALID;

	uint8_t * data = read_node_data (srcid, false);

	nodeid_t data_copy_ref = NODEID_INVALID;

	switch (TNS_NODE_TYPE (hdr))
	{
	case TNT_INDEX:
	case TNT_ARRAY:
		data_copy_ref = index_data_copy (hdr, data, node_flags);
		break;
	case TNT_JOINED:
		data_copy_ref = join_data_copy (hdr, data, node_flags);
		break;
	case TNT_HASHED:
		data_copy_ref = hash_data_copy (hdr, data, node_flags);
		break;
	case TNT_TAIL:
		data_copy_ref = tail_data_copy (hdr, data, node_flags);
		break;
	case TNT_CONDITION:
		data_copy_ref = cond_data_copy (hdr, data, node_flags);
		break;
	default:
		tns_raise_error (KLAV_ENOTIMPL, "node_data_copy: unsupported node type");
		break;
	}

	return data_copy_ref;
}

void Tree_Manager::node_delete (nodeid_t nid, bool deep)
{
	TNS_TRACE (("NODE_DELETE(0x%08X%s)\n", nid, deep ? ", deep" : ""));

	KLAV_ERR err = KLAV_OK;

	if (nid == NODEID_INVALID)
		return;

	TNS_NODE * node = read_node (nid, true);
	node_data_delete (node->tns_hdr, node->tns_data, deep);
	nodeid_t chain = node->tns_chain;

	// delete node
	free_node (nid);

	// delete chained nodes
	if (deep)
	{
		while (chain != NODEID_INVALID)
		{
			node = read_node (chain, true);

			nodeid_t next = node->tns_chain;

			node_delete (chain, deep);

			chain = next;
		}
	}
}

void Tree_Manager::node_data_delete (uint32_t hdr, nodeid_t data_ref, bool deep)
{
	KLAV_ERR err = KLAV_OK;

	uint8_t * data = 0;

	if (data_ref != NODEID_INVALID)
	{
		data = read_node_data (data_ref, true);
	}

	switch (TNS_NODE_TYPE (hdr))
	{
	case TNT_INDEX:
	case TNT_ARRAY:
		index_data_clear (hdr, data, deep);
		break;
	case TNT_JOINED:
		join_data_clear (hdr, data, deep);
		break;
	case TNT_HASHED:
		hash_data_clear (hdr, data, deep);
		break;
	case TNT_TAIL:
		tail_data_clear (hdr, data, deep);
		break;
	case TNT_CONDITION:
		cond_data_clear (hdr, data, deep);
		break;
	default:
		tns_raise_error (KLAV_ENOTIMPL, "node_data_delete: unsupported node type");
		break;
	}

	if (data != 0)
	{
		free_node_data (data_ref);
	}
}

////////////////////////////////////////////////////////////////
// Node class: INDEX

nodeid_t Tree_Manager::index_node_create (
			uint32_t evid,
			uint32_t flags
		)
{
	Tree_Node node (node_mgr ());

	node.alloc (flags);
	node.set_hdr (TNS_NODE_HDR (TNT_ARRAY,0));
	node.set_evid (evid);

	TNS_TRACE (("INDEX_NODE_CREATE (0x%08X)\n", node.id ()));

	return node.id ();
}

nodeid_t Tree_Manager::index_data_copy (
			uint32_t        hdr,
			const uint8_t * src_data,
			uint32_t        flags
		)
{
	uint32_t child_cnt = 0;
	uint32_t child_off = 0;

	uint32_t type = TNS_NODE_TYPE (hdr);
	if (type == TNT_INDEX)
	{
		child_cnt = 256;
	}
	else if (type == TNT_ARRAY)
	{
		child_cnt = TNS_NODE_SIZE (hdr) / 5;
		child_off = TNS_ALIGN4 (child_cnt);
	}
	else
	{
		tns_raise_error (KLAV_ENOTIMPL, "index_data_copy: invalid node type");
	}

	uint32_t data_size = child_off + 4 * child_cnt;

	nodeid_t data_ref = NODEID_INVALID;
	uint8_t * dst_data = alloc_node_data (flags, data_size, & data_ref);

	if (child_off != 0)
		memcpy (dst_data, src_data, child_off);

	const nodeid_t * src_childs = (const uint32_t *) (src_data + child_off);
	nodeid_t * dst_childs = (uint32_t *) (dst_data + child_off);

	for (uint32_t i = 0; i < child_cnt; ++i)
	{
		if (src_childs [i] != NODEID_INVALID)
		{
			dst_childs [i] = node_copy (src_childs [i], flags);
		}
		else
		{
			dst_childs [i] = NODEID_INVALID;
		}
	}

	return data_ref;
}


void Tree_Manager::index_data_clear (
			uint32_t  hdr,
			uint8_t * data,
			bool deep
		)
{
	if (deep)
	{
		uint32_t child_cnt = 0;
		uint32_t child_off = 0;

		uint32_t type = TNS_NODE_TYPE (hdr);
		if (type == TNT_INDEX)
		{
			child_cnt = 256;
		}
		else if (type == TNT_ARRAY)
		{
			child_cnt = TNS_NODE_SIZE (hdr) / 5;
			child_off = TNS_ALIGN4 (child_cnt);
		}
		else
		{
			tns_raise_error (KLAV_EINVAL, "index_data_clear: invalid node type");
		}

		nodeid_t * childs = (uint32_t *) (data + child_off);
	
		for (uint32_t i = 0; i < child_cnt; ++i)
		{
			if (childs [i] != NODEID_INVALID)
			{
				node_delete (childs [i], deep);
			}
		}
	}
}

// quick-and-dirty index node utilites

struct Index_Node
{
	nodeid_t m_child [256];

	Index_Node ()
	{
		clear ();
	}

	void clear ()
	{
		for (int i = 0; i < 256; ++i)
		{
			m_child [i] = NODEID_INVALID;
		}
	}
};

int Tree_Manager::index_set_item (
			nodeid_t nid,
			uint8_t  byte,
			nodeid_t item_nid,
			uint32_t node_flags
		)
{
	Tree_Node node (node_mgr (), nid);
	uint32_t type = node.type ();

	Index_Node idx_node;

	uint32_t item_cnt = 0;

	// phase 1: build temp node
	if (type == TNT_INDEX)
	{
		const nodeid_t * witems = (const uint32_t *) node.data ();

		for (int i = 0; i < 256; ++i)
		{
			idx_node.m_child [i] = witems [i];

			if (witems [i] != NODEID_INVALID)
				item_cnt ++;
		}
	}
	else if (type == TNT_ARRAY)
	{
		uint32_t child_cnt = node.data_size () / 5;
		uint32_t child_off = TNS_ALIGN4 (child_cnt);

		const uint8_t * node_data = 0;
		const nodeid_t * childs = 0;
		
		if (child_cnt != 0)
		{
			node_data = node.data ();
			childs = (const uint32_t *) (node_data + TNS_ALIGN4 (child_cnt));

			for (uint32_t i = 0; i < child_cnt; ++i)
			{
				uint8_t byte = node_data [i];
				nodeid_t child = childs [i];
				idx_node.m_child [byte] = child;
			}
		}

		item_cnt = child_cnt;
	}
	else
	{
		tns_raise_error (KLAV_EINVAL, "index_set_item: invalid node type");
	}

	// phase 2: add/remove item
	if (idx_node.m_child [byte] != NODEID_INVALID)
	{
		if (item_nid == NODEID_INVALID)
		{
			idx_node.m_child [byte] = NODEID_INVALID;
			item_cnt --;
		}
		else
		{
			// just replace
			idx_node.m_child [byte] = item_nid;
			// tns_raise_error (KLAV_EINVAL, "index_set_item: index slot is already occupied");
		}
	}
	else
	{
		if (item_nid == NODEID_INVALID)
		{
			tns_raise_error (KLAV_EINVAL, "index_set_item: invalid item ID");
		}
		else
		{
			idx_node.m_child [byte] = item_nid;
			item_cnt ++;
		}
	}

	// phase 3: save node
	if (item_cnt > TNS_ARRAY_MAX_SIZE)
	{
		// save as INDEX node
		uint32_t data_size = 256 * 4;
		nodeid_t data_ref = NODEID_INVALID;
		uint32_t *items = (uint32_t *) realloc_node_data (node_flags, node.data_ref (), data_size, & data_ref);
		for (int i = 0; i < 256; ++i)
		{
			items [i] = idx_node.m_child [i];
		}
		node.set_hdr (TNS_NODE_HDR(TNT_INDEX, data_size));
		node.set_data (data_ref);
	}
	else
	{
		// save as ARRAY node
		uint32_t data_size = TNS_ALIGN4 (item_cnt) + (4 * item_cnt);
		nodeid_t data_ref = NODEID_INVALID;

		node.realloc_data (node_flags, data_size);
		uint8_t * data = node.wdata ();
		nodeid_t * childs = (uint32_t *) (data + TNS_ALIGN4 (item_cnt));
		memset (data, 0, (uint8_t *) childs - data);
		uint32_t cnt = 0;
		for (int i = 0; i < 256; ++i)
		{
			nodeid_t item = idx_node.m_child [i];
			if (item != NODEID_INVALID)
			{
				if (cnt >= item_cnt)
					tns_raise_error (KLAV_EUNKNOWN, "WTF: array item count too large");
					data [cnt] = (uint8_t) i;
				childs [cnt] = item;
				++cnt;
			}
		}
		node.set_hdr (TNS_NODE_HDR(TNT_ARRAY, data_size));
	}

	return item_cnt;
}

////////////////////////////////////////////////////////////////
// Linear node management

void Tree_Manager::lnode_read (
			Tree_Node& node,
			LNode_Data& lnd
		)
{
	uint32_t type = node.type ();

	if (type == TNT_JOINED)
	{
		uint32_t jdata_size = node.data_size ();
		TNS_ASSERT (jdata_size > sizeof (TNS_JOIN_DATA));

		const TNS_JOIN_DATA * jdata = (const TNS_JOIN_DATA *) node.data ();

		lnd.m_data = ((uint8_t *) jdata) + sizeof (TNS_JOIN_DATA);
		lnd.m_size = jdata_size - sizeof (TNS_JOIN_DATA);
		lnd.m_next = jdata->ndj_next;
	}
	else if (type == TNT_HASHED)
	{
		TNS_ASSERT (node.data_size () == sizeof (TNS_HASH_DATA));

		const TNS_HASH_DATA * hdata = (const TNS_HASH_DATA *) node.data ();

		TNS_ASSERT (hdata->ndh_data != NODEID_INVALID);

		lnd.m_data = node_mgr ()->read_node_data (hdata->ndh_data, false);
		lnd.m_size = hdata->ndh_len;
		lnd.m_next = hdata->ndh_next;
	}
	else
	{
		tns_raise_error (KLAV_EINVAL, "lnode_read: invalid node type");
	}
}

// returns number of bytes prepended to the next node
//
uint32_t Tree_Manager::lnode_prepend (
			nodeid_t nid_next,
			const uint8_t * data,
			uint32_t data_len,
			uint32_t node_flags,
			uint32_t byte_level
		)
{
	if (nid_next == NODEID_INVALID)
		return 0;

	Tree_Node next_node (node_mgr (), nid_next);

	// node must be 'free'
	if (next_node.evid () != 0)
		return 0;
	if (next_node.chain () != NODEID_INVALID)
		return 0;

	uint32_t prepend_len = 0;
	
	uint32_t type = next_node.type ();
	if (type == TNT_JOINED)
	{
		uint32_t next_size = next_node.data_size ();

		TNS_ASSERT (next_size > sizeof (TNS_JOIN_DATA));
		next_size -= sizeof (TNS_JOIN_DATA);

		bool convert_to_hash = false;

		if (m_hash_level == 0)
		{
			prepend_len = data_len;
		}
		else
		{
			if (m_hash_level >= byte_level + data_len)
			{
				prepend_len = data_len;
			}
			else if (m_hash_level < byte_level)
			{
				prepend_len = data_len;
				if (prepend_len + next_size >= TNS_HASH_SIZE_MIN)
					convert_to_hash = true;
			}
			else
			{
				// if the next node is too short to be converted to hash, prepend it too
				prepend_len = byte_level + data_len - m_hash_level;
				if (prepend_len + next_size < TNS_HASH_SIZE_MIN)
					prepend_len = data_len;
				else
					convert_to_hash = true;
			}
		}

		if (prepend_len != 0)
		{
			TNS_TRACE (("LNODE_PREPEND[0x%04X] (0x%08X: prepending 0x%X)\n", byte_level, next_node.id (), prepend_len));

			if (prepend_len + next_size < TNS_HASH_SIZE_MIN)
				convert_to_hash = false;

			TNS_JOIN_DATA * jhdr = (TNS_JOIN_DATA *) next_node.wdata ();
			uint8_t * jdata = ((uint8_t *)jhdr) + sizeof (TNS_JOIN_DATA);

			if (convert_to_hash)
			{
				// converting next node to HASHED
				if (prepend_len + next_size > TNS_HASH_SIZE_MAX)
					prepend_len = TNS_HASH_SIZE_MAX - next_size;

				nodeid_t data_ref = NODEID_INVALID;

				uint8_t * next_data = node_mgr ()->alloc_node_data (
							(node_flags | TNS_F_PRIV),
							next_size + prepend_len,
							& data_ref
						);
				memcpy (next_data, data + data_len - prepend_len, prepend_len);
				memcpy (next_data + prepend_len, jdata, next_size);

				nodeid_t next_next_nid = jhdr->ndj_next;

				next_node.delete_data ();
				TNS_HASH_DATA * hh = (TNS_HASH_DATA *) next_node.alloc_data (node_flags, sizeof (TNS_HASH_DATA));
				hh->ndh_data = data_ref;
				hh->ndh_hash = KlavCRC32 (next_data, next_size + prepend_len, 0);
				hh->ndh_len = next_size + prepend_len;
				hh->ndh_next = next_next_nid;
				next_node.set_type (TNT_HASHED);
			}
			else
			{
				// leaving next node as JOINED
				if (prepend_len + next_size > TNS_JOIN_SIZE_MAX)
					prepend_len = TNS_JOIN_SIZE_MAX - next_size;

				jhdr = (TNS_JOIN_DATA *) next_node.realloc_data (node_flags, prepend_len + next_size + sizeof (TNS_JOIN_DATA));
				jdata = ((uint8_t *)jhdr) + sizeof (TNS_JOIN_DATA);

				for (unsigned int i = next_size; i > 0; --i)
					jdata [i + prepend_len - 1] = jdata [i - 1];

				memcpy (jdata, data + data_len - prepend_len, prepend_len);
			}
		}
	}
	else if (type == TNT_HASHED)
	{
		// determine number of bytes that can be prepended to HASH
		TNS_ASSERT (m_hash_level != 0); // HASH nodes cannot be present otherwise

		uint32_t h_level = byte_level + data_len;
//		TNS_ASSERT (h_level >= m_hash_level);

		if (m_hash_level != 0 && m_hash_level < h_level)
			prepend_len = h_level - m_hash_level;

		if (prepend_len > data_len)
			prepend_len = data_len;

		if (prepend_len != 0)
		{
			TNS_TRACE (("LNODE_PREPEND[0x%04X] (0x%08X: prepending 0x%X)\n", byte_level, next_node.id (), prepend_len));

			TNS_ASSERT (next_node.data_size () == sizeof (TNS_HASH_DATA));
			TNS_HASH_DATA * hdata = (TNS_HASH_DATA *) next_node.data ();

			TNS_ASSERT (hdata->ndh_len >= TNS_HASH_SIZE_MIN);
			TNS_ASSERT (hdata->ndh_len <= TNS_HASH_SIZE_MAX);

			// limit by maximum HASHED data size
			if (hdata->ndh_len + prepend_len > TNS_HASH_SIZE_MAX)
				prepend_len = TNS_HASH_SIZE_MAX - hdata->ndh_len;

			if (prepend_len != 0)
			{
				uint8_t * next_data = node_mgr ()->realloc_node_data (
							(node_flags | TNS_F_PRIV),
							hdata->ndh_data,
							hdata->ndh_len + prepend_len,
							& (hdata->ndh_data)
						);

				// memmove old data to the tail
				for (unsigned int i = hdata->ndh_len; i > 0; --i)
					next_data [i + prepend_len - 1] = next_data [i - 1];

				memcpy (next_data, data + data_len - prepend_len, prepend_len);
				hdata->ndh_len += prepend_len;

				// update hash
				hdata->ndh_hash = KlavCRC32 (next_data, hdata->ndh_len, 0);
			}
		}
	}

	TNS_ASSERT (prepend_len <= data_len);
	return prepend_len;
}

nodeid_t Tree_Manager::lnode_create (
			const LNode_Data& lnd,
			uint32_t          node_flags,
			uint32_t          byte_level
		)
{
	TNS_TRACE (("LNODE_CREATE[0x%04X] (SIZE:0x%X, NEXT:0x%08X)\n", byte_level, lnd.m_size, lnd.m_next));
	nodeid_t nid_next = lnd.m_next;

	uint32_t data_len = lnd.m_size;
	TNS_ASSERT (data_len > 0);

	if (nid_next != NODEID_INVALID)
	{
		uint32_t sfx = lnode_prepend (nid_next, lnd.m_data, data_len, node_flags, byte_level);
		data_len -= sfx;

		if (data_len == 0)
		{
			TNS_TRACE (("LNODE_CREATE[0x%04X] result:0x%08X\n", byte_level, nid_next));
			return nid_next;
		}
	}

	// split to 'joined' and 'hashed' parts
	// calculate length of 'joined' part
	uint32_t join_len = data_len;
	uint32_t hash_len = 0;

	if (m_hash_level != 0)
	{
		if (byte_level > m_hash_level)
		{
			join_len = 0;
		}
		else
		{
			uint32_t end_level = byte_level + data_len;
			if (end_level > m_hash_level)
				end_level = m_hash_level;

			join_len = end_level - byte_level;

		}
		hash_len = data_len - join_len;
		if (hash_len != 0 && hash_len < TNS_HASH_SIZE_MIN)
		{
			join_len = data_len;
			hash_len = 0;
		}
	}

	if (hash_len != 0)
	{
		// create HASHED node
		Tree_Node hash_node (node_mgr ());
		hash_node.alloc (node_flags);
		hash_node.set_type (TNT_HASHED);
		hash_node.alloc_data (node_flags, sizeof (TNS_HASH_DATA));

		TNS_HASH_DATA * hash_data = (TNS_HASH_DATA *) hash_node.wdata ();
		uint8_t * buf = node_mgr ()->alloc_node_data ((node_flags | TNS_F_PRIV), hash_len, & (hash_data->ndh_data));
		memcpy (buf, lnd.m_data + join_len, hash_len);

		hash_data->ndh_len = hash_len;
		hash_data->ndh_next = nid_next;
		hash_data->ndh_hash = KlavCRC32 (buf, hash_len, 0);

		nid_next = hash_node.id ();

		TNS_TRACE (("LNODE_CREATE[0x%04X] new node (HASH):0x%08X\n", byte_level, hash_node.id ()));
	}

	if (join_len != 0)
	{
		// create JOINED node
		Tree_Node join_node (node_mgr ());
		join_node.alloc (node_flags);
		join_node.set_type (TNT_JOINED);
		join_node.alloc_data (node_flags, sizeof (TNS_JOIN_DATA) + join_len);

		TNS_JOIN_DATA * join_data = (TNS_JOIN_DATA *) join_node.wdata ();
		uint8_t * buf = ((uint8_t *) join_data) + sizeof (TNS_JOIN_DATA);
		memcpy (buf, lnd.m_data, join_len);

		join_data->ndj_next = nid_next;

		nid_next = join_node.id ();

		TNS_TRACE (("LNODE_CREATE[0x%04X] new node (JOIN):0x%08X\n", byte_level, join_node.id ()));
	}

	TNS_TRACE (("LNODE_CREATE[0x%04X] result:0x%08X\n", byte_level, nid_next));
	return nid_next;
}

void Tree_Manager::lnode_set_next (
			Tree_Node& node,
			nodeid_t   nid_next
		)
{
	TNS_ASSERT (node.id () != NODEID_INVALID);

	uint32_t type = node.type ();
	
	if (type == TNT_JOINED)
	{
		TNS_ASSERT (node.data_size () > sizeof (TNS_JOIN_DATA));
		TNS_JOIN_DATA *pd = (TNS_JOIN_DATA *) node.data ();

		if (pd->ndj_next != nid_next)
		{
			pd = (TNS_JOIN_DATA *) node.wdata ();
			pd->ndj_next = nid_next;
		}
	}
	else if (type == TNT_HASHED)
	{
		TNS_ASSERT (node.data_size () == sizeof (TNS_HASH_DATA));
		TNS_HASH_DATA *pd = (TNS_HASH_DATA *) node.data ();

		if (pd->ndh_next != nid_next)
		{
			pd = (TNS_HASH_DATA *) node.wdata ();
			pd->ndh_next = nid_next;
		}
	}
	else
	{
		tns_raise_error (KLAV_EINVAL, "lnode_set_next(): invalid node type");
	}
}

void Tree_Manager::lnode_strip_first_byte (
			Tree_Node& node,
			uint32_t   node_flags,
			uint32_t   byte_level
		)
{
	TNS_TRACE (("LNODE_STRIP_FIRST_BYTE[0x%04X] (0x%08X)\n", byte_level, node.id ()));
	TNS_ASSERT (node.id () != NODEID_INVALID);

	LNode_Data ld;
	lnode_read (node, ld);

	if (ld.m_size <= 1)
	{
		tns_raise_error (KLAV_EINVAL, "cannot set linear node size to zero");
//		node_delete (node.id (), false);
//		return ld.m_next;
	}

	// save either as JOINED or as HASHED
	bool save_as_hashed = false;
	if (m_hash_level != 0 && ld.m_size - 1 >= TNS_HASH_SIZE_MIN)
	{
		if (byte_level + 1 > m_hash_level)
			save_as_hashed = true;
	}

	if (! save_as_hashed)
	{
		// save as JOINED
		if (node.type () == TNT_JOINED)
		{
			// just resize JOINED data
			TNS_JOIN_DATA * jj = (TNS_JOIN_DATA *) node.wdata ();

			uint8_t * pd = ((uint8_t *) jj) + sizeof (TNS_JOIN_DATA);
			for (uint32_t i = 0; i < ld.m_size - 1; ++i)
				pd [i] = pd [i + 1];
			pd [ld.m_size - 1] = 0;

			node.realloc_data (node_flags, sizeof (TNS_JOIN_DATA) + ld.m_size - 1);
		}
		else // convert HASHED to JOINED
		{
			TNS_HASH_DATA hh = * (const TNS_HASH_DATA *) node.data ();

			node.set_type (TNT_JOINED);
			TNS_JOIN_DATA * jj = (TNS_JOIN_DATA *) node.realloc_data (node_flags, sizeof (TNS_JOIN_DATA) + ld.m_size - 1);

			uint8_t * pd = ((uint8_t *) jj) + sizeof (TNS_JOIN_DATA);
			memcpy (pd, ld.m_data + 1, ld.m_size - 1);

			jj->ndj_next = ld.m_next;

			node_mgr ()->free_node_data (hh.ndh_data);
		}
	}
	else
	{
		// save as HASHED
		if (node.type () == TNT_JOINED)
		{
			// convert JOINED to HASHED
			nodeid_t data_ref = NODEID_INVALID;
			uint8_t * pd = node_mgr ()->alloc_node_data ((node_flags | TNS_F_PRIV),	ld.m_size - 1, & data_ref);

			memcpy (pd, ld.m_data + 1, ld.m_size - 1);

			node.set_type (TNT_HASHED);
			TNS_HASH_DATA * hh = (TNS_HASH_DATA *) node.realloc_data (node_flags, sizeof (TNS_HASH_DATA));
			hh->ndh_data = data_ref;
			hh->ndh_len = ld.m_size - 1;
			hh->ndh_next = ld.m_next;
			hh->ndh_hash = KlavCRC32 (pd, ld.m_size - 1, 0);
		}
		else
		{
			// just resize HASHED data
			TNS_HASH_DATA * hh = (TNS_HASH_DATA *) node.wdata ();

			uint8_t * pd = node_mgr ()->read_node_data (hh->ndh_data, true);
			for (uint32_t i = 0; i < ld.m_size - 1; ++i)
				pd [i] = pd [i + 1];
			pd [ld.m_size - 1] = 0;

			hh->ndh_len --;
			hh->ndh_hash = KlavCRC32 (pd, hh->ndh_len, 0);

			node_mgr ()->realloc_node_data (
				(node_flags | TNS_F_PRIV),
				hh->ndh_data,
				hh->ndh_len,
				& (hh->ndh_data));
		}
	}

//	return node.id ();
}

void Tree_Manager::lnode_strip_tail (
			Tree_Node& node,
			uint32_t   new_size,
			uint32_t   node_flags,
			uint32_t   byte_level
		)
{
	TNS_TRACE (("LNODE_STRIP_TAIL[0x%04X] (0x%08X), new_size:0x%X\n", byte_level, node.id (), new_size));
	TNS_ASSERT (node.id () != NODEID_INVALID);

	LNode_Data ld;
	lnode_read (node, ld);

	TNS_ASSERT (new_size <= ld.m_size);
	if (new_size == ld.m_size)
		return; // node.id ();

	if (new_size == 0)
	{
		tns_raise_error (KLAV_EINVAL, "cannot set linear node size to zero");
//		node_delete (node.id (), false);
//		return NODEID_INVALID;
	}

	// save either as JOINED or as HASHED
	if (m_hash_level == 0
	 || new_size < TNS_HASH_SIZE_MIN
	 || byte_level + new_size <= m_hash_level)
	{
		// save as JOINED
		if (node.type () == TNT_JOINED)
		{
			// just resize JOINED data
			node.realloc_data (node_flags, sizeof (TNS_JOIN_DATA) + new_size);
		}
		else // convert HASHED to JOINED
		{
			TNS_TRACE (("LNODE_STRIP_TAIL[0x%04X] (0x%08X): converting to JOINED\n", byte_level, node.id ()));
			TNS_HASH_DATA hh = * (const TNS_HASH_DATA *) node.data ();

			node.set_type (TNT_JOINED);
			TNS_JOIN_DATA * jj = (TNS_JOIN_DATA *) node.realloc_data (node_flags, sizeof (TNS_JOIN_DATA) + new_size);

			uint8_t * pd = ((uint8_t *) jj) + sizeof (TNS_JOIN_DATA);
			memcpy (pd, ld.m_data, new_size);

			jj->ndj_next = ld.m_next;

			node_mgr ()->free_node_data (hh.ndh_data);
		}
	}
	else
	{
		// save as HASHED
		if (node.type () == TNT_JOINED)
		{
			// convert JOINED to HASHED
			TNS_TRACE (("LNODE_STRIP_TAIL[0x%04X] (0x%08X): converting to HASHED\n", byte_level, node.id ()));
			nodeid_t data_ref = NODEID_INVALID;
			uint8_t * pd = node_mgr ()->alloc_node_data (
				(node_flags | TNS_F_PRIV),
				new_size,
				& data_ref);

			memcpy (pd, ld.m_data, new_size);

			node.set_type (TNT_HASHED);
			TNS_HASH_DATA * hh = (TNS_HASH_DATA *) node.realloc_data (node_flags, sizeof (TNS_HASH_DATA));
			hh->ndh_data = data_ref;
			hh->ndh_len = new_size;
			hh->ndh_next = ld.m_next;
			hh->ndh_hash = KlavCRC32 (pd, new_size, 0);
		}
		else
		{
			// just resize HASHED data
			TNS_HASH_DATA * hh = (TNS_HASH_DATA *) node.wdata ();

			uint8_t * pd = node_mgr ()->realloc_node_data (
				(node_flags | TNS_F_PRIV), hh->ndh_data, new_size, & (hh->ndh_data));

			hh->ndh_len = new_size;
			hh->ndh_hash = KlavCRC32 (pd, new_size, 0);
		}
	}

//	return node.id ();
}

////////////////////////////////////////////////////////////////
// Node class: JOIN

nodeid_t Tree_Manager::join_data_copy (
			uint32_t        hdr,
			const uint8_t * src_data,
			uint32_t        node_flags
		)
{
	TNS_ASSERT (TNS_NODE_TYPE (hdr) == TNT_JOINED);
	TNS_ASSERT (src_data != 0);

	uint32_t data_size = TNS_NODE_SIZE (hdr);
	TNS_ASSERT (data_size > sizeof (TNS_JOIN_DATA));

	nodeid_t dst_nid = NODEID_INVALID;
	uint8_t * dst_data = alloc_node_data (node_flags, data_size, & dst_nid);
	if (dst_data == 0)
		return KLAV_EWRITE;

	memcpy (dst_data, src_data, data_size);

	// copy 'next' node

	const TNS_JOIN_DATA * src_jd = (const TNS_JOIN_DATA *) src_data;
	nodeid_t src_next_nid = src_jd->ndj_next;

	TNS_ASSERT (src_next_nid != NODEID_INVALID);

	nodeid_t dst_next_nid = node_copy (src_next_nid, node_flags);

	TNS_JOIN_DATA * dst_jd = (TNS_JOIN_DATA *) dst_data;
	dst_jd->ndj_next = dst_next_nid;

	return dst_nid;
}

void Tree_Manager::join_data_clear (
			uint32_t hdr,
			uint8_t * node_data,
			bool deep
		)
{
	TNS_ASSERT (TNS_NODE_TYPE (hdr) == TNT_JOINED);
	TNS_ASSERT (node_data != 0);
	TNS_ASSERT (TNS_NODE_SIZE (hdr) > sizeof (TNS_JOIN_DATA));

	if (deep)
	{
		const TNS_JOIN_DATA * jd = (const TNS_JOIN_DATA *) node_data;
		nodeid_t next_nid = jd->ndj_next;
		node_delete (next_nid, deep);
	}
}

////////////////////////////////////////////////////////////////
// Node class: HASH

nodeid_t Tree_Manager::hash_data_copy (
			uint32_t        hdr,
			const uint8_t * src_data,
			uint32_t        node_flags
		)
{
	TNS_ASSERT (TNS_NODE_TYPE (hdr) == TNT_HASHED);
	TNS_ASSERT (src_data != 0);

	TNS_ASSERT (TNS_NODE_SIZE (hdr) == sizeof (TNS_HASH_DATA));

	nodeid_t dst_data_ref = NODEID_INVALID;
	uint8_t * dst_data = alloc_node_data (node_flags, sizeof (TNS_HASH_DATA), & dst_data_ref);
	if (dst_data == 0)
		return KLAV_EWRITE;

	memcpy (dst_data, src_data, sizeof (TNS_HASH_DATA));

	TNS_HASH_DATA * hh = (TNS_HASH_DATA *) dst_data; 

	uint8_t * src_bytes = read_node_data (hh->ndh_data, false);
	uint8_t * dst_bytes = alloc_node_data (
		(node_flags | TNS_F_PRIV),
		hh->ndh_len,
		& (hh->ndh_data));

	memcpy (dst_bytes, src_bytes, hh->ndh_len);

	// copy 'next' node

	nodeid_t src_next_nid = hh->ndh_next;
	TNS_ASSERT (src_next_nid != NODEID_INVALID);

	hh->ndh_next = node_copy (src_next_nid, node_flags);

	return dst_data_ref;
}

void Tree_Manager::hash_data_clear (
			uint32_t hdr,
			uint8_t * node_data,
			bool deep
		)
{
	TNS_ASSERT (TNS_NODE_TYPE (hdr) == TNT_HASHED);
	TNS_ASSERT (node_data != 0);
	TNS_ASSERT (TNS_NODE_SIZE (hdr) == sizeof (TNS_HASH_DATA));

	const TNS_HASH_DATA * hash_data = (const TNS_HASH_DATA *) node_data;

	if (hash_data->ndh_data != NODEID_INVALID)
		node_mgr ()->free_node_data (hash_data->ndh_data);

	if (deep)
	{
		node_delete (hash_data->ndh_next, deep);
	}
}

void Tree_Manager::hash_set_next (
			nodeid_t  nid,
			nodeid_t  next
		)
{
	Tree_Node node (node_mgr (), nid);

	TNS_ASSERT (node.type () == TNT_HASHED);
	TNS_ASSERT (node.data_size () == sizeof (TNS_HASH_DATA));

	TNS_HASH_DATA * hash_data = (TNS_HASH_DATA *) node.data ();

	if (next != hash_data->ndh_next)
	{
		hash_data = (TNS_HASH_DATA *) node.wdata ();
		hash_data->ndh_next = next;
	}
}

////////////////////////////////////////////////////////////////
// Node class: TAIL

nodeid_t Tree_Manager::tail_node_create (
			uint32_t        evid,
			uint32_t        node_flags
		)
{
	nodeid_t tail_nid = NODEID_INVALID;
	TNS_NODE * tail_node = alloc_node (node_flags, & tail_nid);

	if ((node_flags & TNS_F_TEMP) == 0 && evid == EVID_DELETED)
	{
		tns_raise_error (KLAV_EUNKNOWN, "cannot set EVID_DELETED for final nodes");
	}

	tail_node->tns_hdr = TNS_NODE_HDR (TNT_TAIL, 0);
	tail_node->tns_data = NODEID_INVALID;
	tail_node->tns_evid = evid;
	tail_node->tns_chain = 0;

	TNS_TRACE (("TAIL_NODE_CREATE (0x%08X), EVID:0x%X\n", tail_nid, evid));
	return tail_nid;
}

nodeid_t Tree_Manager::tail_data_copy (
			uint32_t        hdr,
			const uint8_t * src_data,
			uint32_t        node_flags
		)
{
	TNS_ASSERT (TNS_NODE_TYPE (hdr) == TNT_TAIL);
	TNS_ASSERT (src_data == 0);

	return NODEID_INVALID;
}

void Tree_Manager::tail_data_clear (
			uint32_t  hdr,
			uint8_t * node_data,
			bool deep
		)
{
	TNS_ASSERT (TNS_NODE_TYPE (hdr) == TNT_TAIL);
	TNS_ASSERT (node_data == 0);
}

////////////////////////////////////////////////////////////////
// Node class: COND

nodeid_t Tree_Manager::cond_node_create (
			uint8_t  cond_type,
			uint8_t  cond_parm1,
			uint8_t  cond_parm2,
			nodeid_t next,
			uint32_t evid,
			uint32_t node_flags
		)
{
	TNS_ASSERT (cond_type >= TNS_COND_MASK && cond_type <= TNS_COND_LAST);

	if ((node_flags & TNS_F_TEMP) == 0 && evid == EVID_DELETED)
	{
		tns_raise_error (KLAV_EUNKNOWN, "cannot set EVID_DELETED for final nodes");
	}

	Tree_Node cond_node (node_mgr ());
	cond_node.alloc (node_flags);

	cond_node.set_hdr (TNS_NODE_HDR (TNT_CONDITION, 0));
	cond_node.set_evid (evid);
	cond_node.set_chain (0);

	TNS_COND_DATA * cond_data = (TNS_COND_DATA *) cond_node.alloc_data (node_flags, sizeof (TNS_COND_DATA));
	cond_data->ndc_type = cond_type;
	cond_data->ndc_parm1 = cond_parm1;
	cond_data->ndc_parm2 = cond_parm2;
	cond_data->ndc_next = next;

	return cond_node.id ();
}

nodeid_t Tree_Manager::cond16_node_create (
				uint8_t  cond_type,
				uint16_t cond_arg,
				nodeid_t next,
				uint32_t evid,
				uint32_t node_flags
			)
{
	uint8_t arg1 = (uint8_t) (cond_arg & 0xFF);
	uint8_t arg2 = (uint8_t) ((cond_arg >> 8) & 0xFF);
	return cond_node_create (cond_type, arg1, arg2, next, evid, node_flags);
}

void Tree_Manager::cond_set_next (
			nodeid_t  nid,
			nodeid_t  next
		)
{
	Tree_Node node (node_mgr (), nid);

	TNS_ASSERT (node.type () == TNT_CONDITION);
	TNS_ASSERT (node.data_size () == sizeof (TNS_COND_DATA));

	const TNS_COND_DATA * data = (const TNS_COND_DATA *) node.data ();

	if (next != data->ndc_next)
	{
		TNS_COND_DATA * wdata = (TNS_COND_DATA *) node.wdata ();
		wdata->ndc_next = next;
	}
}

nodeid_t Tree_Manager::cond_data_copy (
			uint32_t        hdr,
			const uint8_t * src_data,
			uint32_t        node_flags
		)
{
	TNS_ASSERT (TNS_NODE_TYPE (hdr) == TNT_CONDITION);
	TNS_ASSERT (src_data != 0);

	TNS_ASSERT (TNS_NODE_SIZE (hdr) == sizeof (TNS_COND_DATA));

	const TNS_COND_DATA * src_cond = (const TNS_COND_DATA *) src_data;

	nodeid_t dst_nid = NODEID_INVALID;
	TNS_COND_DATA * dst_cond = (TNS_COND_DATA *) alloc_node_data (node_flags, sizeof (TNS_COND_DATA), & dst_nid);
	if (dst_cond == 0)
		return KLAV_EWRITE;

	dst_cond->ndc_type = src_cond->ndc_type;
	dst_cond->ndc_parm1 = src_cond->ndc_parm1;
	dst_cond->ndc_parm2 = src_cond->ndc_parm2;

	// copy 'next' node
	TNS_ASSERT (src_cond->ndc_next != NODEID_INVALID);
	dst_cond->ndc_next = node_copy (src_cond->ndc_next, node_flags);

	return dst_nid;
}

void Tree_Manager::cond_data_clear (
			uint32_t   hdr,
			uint8_t *  node_data,
			bool       deep
		)
{
	TNS_ASSERT (TNS_NODE_TYPE (hdr) == TNT_CONDITION);
	TNS_ASSERT (node_data != 0);
	TNS_ASSERT (TNS_NODE_SIZE (hdr) == sizeof (TNS_COND_DATA));

	if (deep)
	{
		nodeid_t next_nid = ((const TNS_COND_DATA *) node_data)->ndc_next;
		node_delete (next_nid, deep);
	}
}
