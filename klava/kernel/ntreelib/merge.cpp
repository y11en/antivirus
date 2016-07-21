// merge.cpp
//
//

#include "tns_impl.h"
#include <string.h>

////////////////////////////////////////////////////////////////

class Tree_Merger : public Tree_Manager
{
public:
	Tree_Merger (
			TNS_Tree_Manager *node_mgr,
			uint32_t node_flags,
			uint32_t hash_level
		) : Tree_Manager (node_mgr, node_flags, hash_level)
	{
	}

	nodeid_t merge_tree (nodeid_t nid_root, nodeid_t nid_src);

private:

	// mergers

	nodeid_t merge_node (
			nodeid_t tgt_nid,
			nodeid_t src_nid,
			uint32_t byte_level
		);

	nodeid_t merge_index (
			nodeid_t tgt_nid,
			nodeid_t src_nid,
			uint32_t byte_level
		);

	nodeid_t merge_linear (
			nodeid_t tgt_nid,
			nodeid_t src_nid,
			uint32_t byte_level
		);

	nodeid_t merge_index_index (
			nodeid_t tgt_nid,
			nodeid_t src_nid,
			uint32_t byte_level
		);

	nodeid_t merge_index_linear (
			nodeid_t tgt_nid,
			nodeid_t src_nid,
			uint32_t byte_level
		);

	nodeid_t merge_linear_linear (
			nodeid_t tgt_nid,
			nodeid_t src_nid,
			uint32_t byte_level
		);

	nodeid_t merge_tail (
			nodeid_t tgt_nid,
			nodeid_t src_nid
		);

	nodeid_t merge_cond_node (
			Tree_Node& tgt_node,
			Tree_Node& src_node,
			uint32_t byte_level
		);

	nodeid_t merge_cond_chain (
			nodeid_t tgt_chain,
			nodeid_t src_chain,
			uint32_t byte_level
		);

	nodeid_t merge_cond_chain_node (
			nodeid_t tgt_chain,
			nodeid_t src_cond,
			uint32_t byte_level
		);
};

////////////////////////////////////////////////////////////////

nodeid_t Tree_Merger::merge_tree (
		nodeid_t nid_root,
		nodeid_t nid_src
	)
{
	return merge_node (nid_root, nid_src, 0);
}

nodeid_t Tree_Merger::merge_node (
			nodeid_t tgt_nid,
			nodeid_t src_nid,
			uint32_t byte_level
		)
{
	TNS_TRACE (("MERGE[0x%04X]: 0x%08X, 0x%08X\n", byte_level, tgt_nid, src_nid));

	if (src_nid == NODEID_INVALID) // no-op
	{
		TNS_TRACE (("MERGE[0x%04X]: 0x%08X, 0x%08X => 0x%08X\n", byte_level, tgt_nid, src_nid, tgt_nid));
		return tgt_nid;
	}

	if (tgt_nid == NODEID_INVALID)
	{
		nodeid_t res_nid = node_copy (src_nid, node_flags ());
		TNS_TRACE (("MERGE[0x%04X]: 0x%08X, 0x%08X => 0x%08X\n", byte_level, tgt_nid, src_nid, res_nid));
		return res_nid;
	}

	Tree_Node tgt_node (node_mgr (), tgt_nid);
	Tree_Node src_node (node_mgr (), src_nid);

	// merge EVIDs (res_evid == effective EVID)
	uint32_t res_evid = tgt_node.evid ();
	uint32_t src_evid = src_node.evid ();

	if (res_evid == 0)
	{
		if (src_evid != 0)
		{
			if (src_evid == EVID_DELETED && ! is_temp_tree ())
			{
				tns_raise_error (KLAV_ENOTFOUND, "signature to be deleted is not present");
			}
			res_evid = src_evid;
		}
	}
	else
	{
		if (src_evid != 0)
		{
			if (src_evid == EVID_DELETED && ! is_temp_tree ())
			{
				// clear target EVID
				res_evid = 0;
			}
			else
			{
				tns_raise_error (KLAV_ECONFLICT, "conflicting signatures");
			}
		}
	}

	// separate nodes to 'normal' nodes and 'condition' chains

	nodeid_t tgt_cond = tgt_node.chain ();
	nodeid_t src_cond = src_node.chain ();

	if (tgt_node.type () == TNT_CONDITION)
	{
		tgt_cond = tgt_node.id ();
		tgt_node.clear ();
	}

	if (src_node.type () == TNT_CONDITION)
	{
		src_cond = src_node.id ();
		src_node.clear ();
	}
	else if (src_node.type () == TNT_TAIL)
	{
		// if source node is just a 'tail', ignore it
		src_node.clear ();
	}

	// merge 'normal' nodes

	nodeid_t res_nid = tgt_node.id ();

	if (res_nid != NODEID_INVALID)
	{
		if (src_node.id () != NODEID_INVALID)
		{
			switch (tgt_node.type ())
			{
			case TNT_INDEX:
			case TNT_ARRAY:
				res_nid = merge_index (tgt_node.id (), src_node.id (), byte_level);
				break;
			case TNT_JOINED:
			case TNT_HASHED:
				res_nid = merge_linear (tgt_node.id (), src_node.id (), byte_level);
				break;
			case TNT_TAIL:
				res_nid = merge_tail (tgt_node.id (), src_node.id ());
				break;
			default:
				tns_raise_error (KLAV_EUNKNOWN, "unexpected node type");
				break;
			}
		}
	}
	else
	{
		if (src_node.id () != NODEID_INVALID)
		{
			res_nid = node_copy (src_node.id (), node_flags ());
		}
	}

	Tree_Node res_node (node_mgr (), res_nid);

	// merge conditions
	nodeid_t res_cond = NODEID_INVALID;
	if (tgt_cond != NODEID_INVALID || src_cond != NODEID_INVALID)
	{
		res_cond = merge_cond_chain (tgt_cond, src_cond, byte_level);
	}

	// delete empty TAIL nodes
	if (res_evid == 0 && res_node.id () != NODEID_INVALID)
	{
		if (res_node.type () == TNT_TAIL)
		{
			node_delete (res_node.id (), false);
			res_node.clear ();
		}
	}

	if (res_node.id () == NODEID_INVALID)
	{
		res_node.assign (res_cond);
		if (res_cond != NODEID_INVALID)
		{
			res_cond = res_node.chain ();
		}
	}

	TNS_ASSERT (!(res_evid == EVID_DELETED && ! is_temp_tree ()));

	if (res_node.id () == NODEID_INVALID)
	{
		if (res_evid != 0)
		{
			// create tail node
			res_node.assign (tail_node_create (res_evid, node_flags ()));
		}
	}
	else
	{
		if (res_evid != res_node.evid ())
			res_node.set_evid (res_evid);

		if (res_cond != res_node.chain ())
			res_node.set_chain (res_cond);
	}

	TNS_TRACE (("MERGE[0x%04X]: 0x%08X, 0x%08X => 0x%08X\n", byte_level, tgt_nid, src_nid, res_node.id ()));
	return res_node.id ();
}

////////////////////////////////////////////////////////////////

nodeid_t Tree_Merger::merge_index (
			nodeid_t tgt_nid,
			nodeid_t src_nid,
			uint32_t byte_level
		)
{
	Tree_Node src_node (node_mgr (), src_nid);

	nodeid_t res_nid = NODEID_INVALID;

	switch (src_node.type ())
	{
	case TNT_INDEX:
	case TNT_ARRAY:
		res_nid = merge_index_index (tgt_nid, src_nid, byte_level);
		break;
	case TNT_JOINED:
	case TNT_HASHED:
		res_nid = merge_index_linear (tgt_nid, src_nid, byte_level);
		break;
	default:
		tns_raise_error (KLAV_EINVAL, "unexpected node type");
		break;
	}

	return res_nid;
}

nodeid_t Tree_Merger::merge_linear (
			nodeid_t tgt_nid,
			nodeid_t src_nid,
			uint32_t byte_level
		)
{
	Tree_Node src_node (node_mgr (), src_nid);

	nodeid_t res_nid = NODEID_INVALID;

	switch (src_node.type ())
	{
	case TNT_INDEX:
	case TNT_ARRAY:
		tns_raise_error (KLAV_ENOTIMPL, "not implemented: merge_linear_index");
		break;
	case TNT_JOINED:
	case TNT_HASHED:
		res_nid = merge_linear_linear (tgt_nid, src_nid, byte_level);
		break;
	default:
		tns_raise_error (KLAV_EINVAL, "unexpected node type");
		break;
	}

	return res_nid;
}

nodeid_t Tree_Merger::merge_tail (
			nodeid_t tgt_nid,
			nodeid_t src_nid
		)
{
	// just migrate TAIL evid to copy of source node
	Tree_Node tgt_node (node_mgr (), tgt_nid);

	TNS_ASSERT (tgt_node.type () == TNT_TAIL);
	node_delete (tgt_node.id (), false);

	Tree_Node src_node (node_mgr (), node_copy (src_nid, node_flags ()));

	return src_node.id ();
}

////////////////////////////////////////////////////////////////

nodeid_t Tree_Merger::merge_index_index (
			nodeid_t tgt_nid,
			nodeid_t src_nid,
			uint32_t byte_level
		)
{
	TNS_TRACE (("MERGE[0x%04X] index+index (0x%08X, 0x%08X)\n", byte_level, tgt_nid, src_nid));
	tns_raise_error (KLAV_EINVAL, "merge_index_index: not implemented");
	return NODEID_INVALID;
}

////////////////////////////////////////////////////////////////

nodeid_t Tree_Merger::merge_index_linear (
			nodeid_t tgt_nid,
			nodeid_t src_nid,
			uint32_t byte_level
		)
{
	TNS_TRACE (("MERGE[0x%04X] index+linear (0x%08X, 0x%08X)\n", byte_level, tgt_nid, src_nid));
	Tree_Node tgt_node (node_mgr (), tgt_nid);
	Tree_Node src_node (node_mgr (), src_nid);

	nodeid_t merge_tgt = NODEID_INVALID;
	nodeid_t merge_src = NODEID_INVALID;

	// get access to linear node data
	LNode_Data src_ld;
	lnode_read (src_node, src_ld);

	TNS_ASSERT (src_ld.m_size > 0);
	TNS_ASSERT (src_ld.m_next != NODEID_INVALID);

	uint8_t byte = src_ld.m_data [0]; // first byte

	nodeid_t tmp_src_nid = NODEID_INVALID;

	if (src_ld.m_size == 1)
	{
		merge_src = src_ld.m_next;
	}
	else
	{
		LNode_Data tmp_ld;
		tmp_ld.m_data = src_ld.m_data + 1;
		tmp_ld.m_size = src_ld.m_size - 1;
		tmp_ld.m_next = NODEID_INVALID; // disable prepend logic

		tmp_src_nid = lnode_create (tmp_ld, TNS_F_TEMP, byte_level + 1);
		TNS_TRACE (("MERGE_INDEX_LINEAR: TMP_SRC_NID=0x%08X\n", tmp_src_nid));

		Tree_Node tmp_src_node (node_mgr (), tmp_src_nid);
		lnode_set_next (tmp_src_node, src_ld.m_next);

		merge_src = tmp_src_nid;
	}

	merge_tgt = index_get_item (tgt_nid, byte);

	nodeid_t new_item_nid = merge_node (merge_tgt, merge_src, byte_level + 1);

	if (tmp_src_nid != NODEID_INVALID)
	{
		node_delete (tmp_src_nid, false);
	}

	int new_cnt = index_set_item (tgt_nid, byte, new_item_nid, node_flags ());
	if (new_cnt == 0)
	{
		node_delete (tgt_node.id (), false);
		tgt_node.clear ();
	}
	else if (new_cnt == 1)
	{
		TNS_TRACE (("MERGE[0x%04X] convert: index to linear, old node:0x%08X\n", byte_level, tgt_nid));

		// get single item from index node
		nodeid_t next_nid = NODEID_INVALID;
		uint8_t next_byte = index_get_single_item (tgt_nid, & next_nid);

		// transform INDEX/ARRAY node into LINEAR node
		node_delete (tgt_node.id (), false);

		LNode_Data new_ld;
		new_ld.m_data = & next_byte;
		new_ld.m_size = 1;
		new_ld.m_next = next_nid;

		nodeid_t new_nid = lnode_create (new_ld, node_flags (), byte_level);
		tgt_node.assign (new_nid);

		TNS_TRACE (("MERGE[0x%04X] convert: index to linear, new node:0x%08X\n", byte_level, tgt_node.id ()));
	}

	return tgt_node.id ();
}

////////////////////////////////////////////////////////////////

nodeid_t Tree_Merger::merge_linear_linear (
			nodeid_t tgt_nid,
			nodeid_t src_nid,
			uint32_t byte_level
		)
{
	TNS_TRACE (("MERGE[0x%04X] linear+linear (0x%08X, 0x%08X)\n", byte_level, tgt_nid, src_nid));
	Tree_Node tgt_node (node_mgr (), tgt_nid);
	Tree_Node src_node (node_mgr (), src_nid);

	LNode_Data tgt_ld;
	LNode_Data src_ld;

	lnode_read (tgt_node, tgt_ld);
	lnode_read (src_node, src_ld);

	// calculate common prefix length
	uint32_t prefix_max = (tgt_ld.m_size < src_ld.m_size) ? tgt_ld.m_size : src_ld.m_size;
	uint32_t prefix_len = 0;

	while (prefix_len < prefix_max && tgt_ld.m_data [prefix_len] == src_ld.m_data [prefix_len])
	     ++prefix_len;

	nodeid_t res_nid = NODEID_INVALID;

	if (prefix_len == 0)
	{
		// merge into index node
		uint8_t tb0 = tgt_ld.m_data [0];
		uint8_t sb0 = src_ld.m_data [0];

		nodeid_t tail_tgt = tgt_node.id ();
		if (tgt_ld.m_size == 1)
		{
			node_delete (tail_tgt, false);
			tail_tgt = tgt_ld.m_next;
		}
		else
		{
			lnode_strip_first_byte (tgt_node, node_flags (), byte_level);
		}

		nodeid_t tail_src = src_node.id ();
		if (src_ld.m_size == 1)
		{
			tail_src = node_copy (src_ld.m_next, node_flags ());
		}
		else
		{
			// create stripped node
			LNode_Data new_src_ld;
			new_src_ld.m_data = src_ld.m_data + 1;
			new_src_ld.m_size = src_ld.m_size - 1;
			new_src_ld.m_next = node_copy (src_ld.m_next, node_flags ());

			tail_src = lnode_create (new_src_ld, node_flags (), byte_level + 1);
		}

		// create new node (INDEX)
		res_nid = index_node_create (0, node_flags ());
		index_set_item (res_nid, tb0, tail_tgt, node_flags ());
		index_set_item (res_nid, sb0, tail_src, node_flags ());

		// unlink condition chain (it will be set for INDEX node)
		tgt_node.set_chain (NODEID_INVALID);
	}
	else
	{
		nodeid_t merge_tgt = NODEID_INVALID;
		nodeid_t merge_src = NODEID_INVALID;

		nodeid_t tmp_src_nid = NODEID_INVALID;

		if (prefix_len == src_ld.m_size)
		{
			merge_src = src_ld.m_next;
		}
		else
		{
			// create temp 'short' node
			LNode_Data tmp_ld;
			tmp_ld.m_data = src_ld.m_data + prefix_len;
			tmp_ld.m_size = src_ld.m_size - prefix_len;
			tmp_ld.m_next = NODEID_INVALID; // disable prepend logic

			tmp_src_nid = lnode_create (tmp_ld, TNS_F_TEMP, byte_level + prefix_len);
			TNS_TRACE (("MERGE_LINEAR_LINEAR: TMP_SRC_NID=0x%08X\n", tmp_src_nid));

			Tree_Node tmp_src_node (node_mgr (), tmp_src_nid);
			lnode_set_next (tmp_src_node, src_ld.m_next);

			merge_src = tmp_src_nid;

//			node_delete (src_nid, false);
		}

		uint32_t tgt_len = tgt_ld.m_size;

		if (prefix_len == tgt_len)
		{
			merge_tgt = tgt_ld.m_next;
		}
		else
		{
			if (! is_temp_tree ())
			{
				Tree_Node merge_src_node (node_mgr (), merge_src);
				if (merge_src_node.evid () == EVID_DELETED)
				{
					tns_raise_error (KLAV_ENOTFOUND, "signature to be deleted is not present");
				}
			}

			// split target node in pair
			LNode_Data nxt_ld;
			nxt_ld.m_data = tgt_ld.m_data + prefix_len;
			nxt_ld.m_size = tgt_ld.m_size - prefix_len;
			nxt_ld.m_next = tgt_ld.m_next;

			nodeid_t nxt_nid = lnode_create (nxt_ld, node_flags (), byte_level + prefix_len);

			// request that target node must be stripped
			tgt_len = prefix_len;

			merge_tgt = nxt_nid;
		}

		res_nid = merge_node (merge_tgt, merge_src, byte_level + prefix_len);

		if (tmp_src_nid != NODEID_INVALID)
		{
			node_delete (tmp_src_nid, false);
		}

		if (res_nid == NODEID_INVALID)
		{
			node_delete (tgt_node.id (), false);
			tgt_node.clear ();
		}
		else
		{
			// prepend data if possible
			uint32_t prepend_len = lnode_prepend (res_nid, tgt_ld.m_data, tgt_len, node_flags (), byte_level);
			uint32_t new_size = tgt_len - prepend_len;

			if (new_size != tgt_ld.m_size)
			{
				if (new_size == 0)
				{
					node_delete (tgt_node.id (), false);
					tgt_node.assign (res_nid);
				}
				else
				{
					lnode_strip_tail (tgt_node, tgt_len - prepend_len, node_flags (), byte_level);
					lnode_set_next (tgt_node, res_nid);
				}
			}
			else
			{
				lnode_set_next (tgt_node, res_nid);
			}
		
			res_nid = tgt_node.id ();
		}
	}

	return res_nid;
}

nodeid_t Tree_Merger::merge_cond_node (
		Tree_Node& tgt_node,
		Tree_Node& src_node,
		uint32_t byte_level
	)
{
	TNS_COND_DATA * tgt_cond = (TNS_COND_DATA *) tgt_node.data ();
	const TNS_COND_DATA * src_cond = (const TNS_COND_DATA *) src_node.data ();

	// this method assumes that nodes have the same type
	TNS_ASSERT (tgt_cond->ndc_type == src_cond->ndc_type);

	nodeid_t new_tail = NODEID_INVALID;
	
	if (tgt_cond->ndc_parm1 == src_cond->ndc_parm1
	 && tgt_cond->ndc_parm2 == src_cond->ndc_parm2)
	{
		// easy case: complete match
		new_tail = merge_node (tgt_cond->ndc_next, src_cond->ndc_next, byte_level + tgt_cond->ndc_parm1);
	}
	else if (tgt_cond->ndc_type == TNS_COND_ANY
	      || tgt_cond->ndc_type == TNS_COND_STAR)
	{
		// ANY[N] + ANY[M] or STAR[N] + STAR[M]
		uint32_t src_cnt = TNS_COND_GET16(src_cond);
		uint32_t tgt_cnt = TNS_COND_GET16(tgt_cond);

		uint32_t byte_level_inc = 0;

		if (tgt_cnt < src_cnt)
		{
			// split source
			nodeid_t tmp_src_nid = cond16_node_create (
					tgt_cond->ndc_type,
					(uint16_t)(src_cnt - tgt_cnt),
					src_cond->ndc_next,
					src_node.evid (),
					TNS_F_TEMP);

			if (tgt_cond->ndc_type == TNS_COND_ANY)
				byte_level_inc = tgt_cnt;

			new_tail = merge_node (tgt_cond->ndc_next, tmp_src_nid, byte_level + byte_level_inc);

			node_delete (tmp_src_nid, false);
		}
		else
		{
			// split target
			tgt_cond = (TNS_COND_DATA *) tgt_node.wdata ();

			TNS_COND_SET16 (tgt_cond, src_cnt);

			tgt_cond->ndc_next = cond16_node_create (
					tgt_cond->ndc_type,
					(uint16_t)(tgt_cnt - src_cnt),
					tgt_cond->ndc_next,
					0, 0);
			
			if (tgt_cond->ndc_type == TNS_COND_ANY)
				byte_level_inc = src_cnt;

			new_tail = merge_node (tgt_cond->ndc_next, src_cond->ndc_next, byte_level + src_cnt);
		}
	}

	nodeid_t res_nid = tgt_node.id ();

	if (new_tail == NODEID_INVALID)
	{
		if (tgt_node.evid () != 0)
		{
			tgt_node.delete_data ();
			tgt_node.set_hdr (TNS_NODE_HDR (TNT_TAIL, 0));
		}
		else
		{
			// unbind condition chain (return condition chain as the resulting node)
			res_nid = NODEID_INVALID; // tgt_node.chain ();
			node_delete (tgt_node.id (), false);
		}
	}
	else
	{
		// if the current node is ANY/STAR and the next node has the same type, merge nodes
		if (new_tail != tgt_cond->ndc_next)
		{
			tgt_cond = (TNS_COND_DATA *) tgt_node.wdata ();
			tgt_cond->ndc_next = new_tail;

			if (tgt_cond->ndc_type == TNS_COND_ANY || tgt_cond->ndc_type == TNS_COND_STAR)
			{
				Tree_Node next_node (node_mgr (), new_tail);
				if (next_node.type () == TNT_CONDITION)
				{
					TNS_COND_DATA * next_cond = (TNS_COND_DATA *) next_node.data ();

					if (next_cond->ndc_type == tgt_cond->ndc_type)
					{
						uint32_t tot_cnt = TNS_COND_GET16(tgt_cond);
						tot_cnt += TNS_COND_GET16(next_cond);

						if (tot_cnt <= 0xFFFF)
						{
							// merge nodes
							TNS_ASSERT (next_node.chain () == NODEID_INVALID);
							tgt_cond = (TNS_COND_DATA *) tgt_node.wdata ();
							TNS_COND_SET16 (tgt_cond, tot_cnt);
							tgt_cond->ndc_next = next_cond->ndc_next;
							node_delete (next_node.id (), false);
						}
					}
				}
			}
		}
	}

	return res_nid;
}

nodeid_t Tree_Merger::merge_cond_chain (nodeid_t tgt_chain, nodeid_t src_chain, uint32_t byte_level)
{
	if (src_chain == NODEID_INVALID)
		return tgt_chain;

	if (tgt_chain == NODEID_INVALID)
	{
		nodeid_t res_nid = node_copy (src_chain, node_flags ());  // NOTE: assuming proper ordering of src_chain
//		node_delete (src_chain, true);
		return res_nid;
	}

	while (src_chain != NODEID_INVALID)
	{
		Tree_Node src_node (node_mgr (), src_chain);
		nodeid_t next_nid = src_node.chain ();

		tgt_chain = merge_cond_chain_node (tgt_chain, src_node.id (), byte_level);
		src_chain = next_nid;
	}

	return tgt_chain;
}

static int tree_cond_compare (const TNS_COND_DATA *a, const TNS_COND_DATA *b)
{
	if (a->ndc_type != b->ndc_type)
		return (a->ndc_type < b->ndc_type) ? -1 : 1;

	if (a->ndc_parm2 != b->ndc_parm2)
		return a->ndc_parm2 < b->ndc_parm2 ? -1 : 1;

	if (a->ndc_parm1 != b->ndc_parm1)
		return a->ndc_parm1 < b->ndc_parm1 ? -1 : 1;

	return 0;
}

static bool tree_cond_can_merge (
		const TNS_COND_DATA *tgt_cond,
		const TNS_COND_DATA *src_cond
	)
{
	// precondition: types must be equal
	if (tgt_cond->ndc_type != src_cond->ndc_type)
		return false;

	// ANY and STAR nodes always can be merged
	if (tgt_cond->ndc_type == TNS_COND_ANY)
		return true;

	if (tgt_cond->ndc_type == TNS_COND_STAR)
		return true;

	// otherwise, parameters must match completely
	if (tgt_cond->ndc_parm1 == src_cond->ndc_parm1 
	 && tgt_cond->ndc_parm2 == src_cond->ndc_parm2)
			return true;

	return false;
}


nodeid_t Tree_Merger::merge_cond_chain_node (
			nodeid_t tgt_chain,
			nodeid_t src_cond_nid,
			uint32_t byte_level
		)
{
	TNS_ASSERT (src_cond_nid != NODEID_INVALID);

	Tree_Node src_cond (node_mgr (), src_cond_nid);
	const TNS_COND_DATA * src_cond_data = (const TNS_COND_DATA *) src_cond.data ();

	// insert src_cond into the condition chain
	nodeid_t prev_nid = NODEID_INVALID;
	nodeid_t curr_nid = tgt_chain;
	nodeid_t new_nid = NODEID_INVALID;

	while (curr_nid != NODEID_INVALID)
	{
		Tree_Node curr_cond (node_mgr (), curr_nid);
		nodeid_t next_nid = curr_cond.chain ();

		TNS_ASSERT (curr_cond.type () == TNT_CONDITION);
		const TNS_COND_DATA * curr_data = (const TNS_COND_DATA *) curr_cond.data ();

		if (tree_cond_can_merge (curr_data, src_cond_data))
		{
			// merge condition nodes
			new_nid = merge_cond_node (curr_cond, src_cond, byte_level);
			if (new_nid == NODEID_INVALID)
			{
				if (prev_nid == NODEID_INVALID)
				{
					tgt_chain = next_nid;
				}
				else
				{
					Tree_Node prev_node (node_mgr (), prev_nid);
					prev_node.set_chain (next_nid);
				}
			}
			else
			{
				Tree_Node new_cond (node_mgr (), new_nid);
				new_cond.set_chain (next_nid);

				if (prev_nid == NODEID_INVALID)
				{
					tgt_chain = new_nid;
				}
				else
				{
					Tree_Node prev_node (node_mgr (), prev_nid);
					prev_node.set_chain (new_nid);
				}
			}
			return tgt_chain;
		}

		if (tree_cond_compare (src_cond_data, curr_data) < 0)
		{
			break;
		}

		prev_nid = curr_cond.id ();
		curr_nid = next_nid;
	}

	new_nid = node_copy (src_cond_nid, node_flags ());
//	node_delete (src_cond_nid, true);

	Tree_Node new_cond (node_mgr (), new_nid);
	new_cond.set_chain (curr_nid);

	if (prev_nid == NODEID_INVALID)
	{
		tgt_chain = new_nid;
	}
	else
	{
		Tree_Node prev_node (node_mgr (), prev_nid);
		prev_node.set_chain (new_nid);
	}

	return tgt_chain;
}

////////////////////////////////////////////////////////////////

KLAV_EXTERN_C
nodeid_t KLAV_CALL KLAV_Tree_Merge (
			TNS_Tree_Manager *node_mgr,
			nodeid_t nid_root,
			nodeid_t nid_merge,
			uint32_t node_flags,
			uint32_t hash_level
		)
{
	Tree_Merger merger (node_mgr, node_flags, hash_level);
	
	return merger.merge_tree (nid_root, nid_merge);
}

