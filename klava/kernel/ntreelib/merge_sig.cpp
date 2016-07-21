// merge_sig.cpp
//
//

#include "tns_impl.h"
#include <klav_sig.h>

////////////////////////////////////////////////////////////////

class Tree_Sig_Merger : public Tree_Manager
{
public:
	Tree_Sig_Merger (TNS_Tree_Manager * node_mgr, uint32_t hash_level)
		: Tree_Manager (node_mgr, TNS_F_TEMP, hash_level)
	{
	}

	nodeid_t create_signature_raw (
			const uint8_t * sig_data,
			size_t sig_size,
			uint32_t evid
		);

	nodeid_t merge_signature_raw (
			nodeid_t nid_root,
			const uint8_t * sig_data,
			size_t sig_size,
			uint32_t evid,
			uint32_t node_flags
		);

	nodeid_t create_signature (
			const uint8_t * sig_data,
			size_t sig_size,
			uint32_t evid,
			uint32_t byte_level
		);

	nodeid_t merge_signature (
			nodeid_t nid_root,
			const uint8_t * sig_data,
			size_t sig_size,
			uint32_t evid,
			uint32_t node_flags
		);

private:
	nodeid_t ibyte_nodes_create (const uint8_t *data, size_t size, nodeid_t next);
};

////////////////////////////////////////////////////////////////
// create signature node

nodeid_t Tree_Sig_Merger::create_signature_raw (
			const uint8_t * sig_data,
			size_t sig_size,
			uint32_t evid
		)
{
	if (sig_size == 0)
		tns_raise_error (KLAV_EINVAL, "signature size cannot be zero");

	if (evid == 0)
		tns_raise_error (KLAV_EINVAL, "signature must have nonzero event ID");

	LNode_Data new_ld;
	new_ld.m_data = sig_data;
	new_ld.m_size = (uint32_t) sig_size;
	new_ld.m_next = tail_node_create (evid, TNS_F_TEMP);

	nodeid_t nid_sig = lnode_create (new_ld, TNS_F_TEMP, 0);

	return nid_sig;
}

nodeid_t Tree_Sig_Merger::merge_signature_raw (
			nodeid_t nid_root,
			const uint8_t * sig_data,
			size_t sig_size,
			uint32_t evid,
			uint32_t node_flags
		)
{
	nodeid_t sig_nid = create_signature_raw (sig_data, sig_size, evid);

	nid_root = KLAV_Tree_Merge (node_mgr (), nid_root, sig_nid, node_flags, hash_level ());

	node_delete (sig_nid, true);

	return nid_root;
}

////////////////////////////////////////////////////////////////

nodeid_t Tree_Sig_Merger::create_signature (
			const uint8_t * sig_data,
			size_t sig_size,
			uint32_t evid,
			uint32_t byte_level
		)
{
	if (evid == 0)
		tns_raise_error (KLAV_EINVAL, "signature must have nonzero event ID");

	if (sig_size == 0)
	{
		// make TAIL node
		nodeid_t nid_tail = tail_node_create (evid, TNS_F_TEMP);
		return nid_tail;
	}

	const uint8_t * sig_end = sig_data + sig_size;

	KLAV_Sig_Element elt;

	size_t sz = elt.scan (sig_data, sig_end - sig_data);
	if (sz == 0)
		tns_raise_error (KLAV_EINVAL, "invalid signature data");

	// determine byte level for next element
	uint32_t byte_len = 0;

	switch (elt.type ())
	{
	case KLAV_SIG_ELT_BYTES:
	case KLAV_SIG_ELT_IBYTES:
		byte_len = elt.get_data_size ();
		break;
	case KLAV_SIG_ELT_ANYBYTES:
		byte_len = elt.get_anybyte_count ();
		break;
	case KLAV_SIG_ELT_BITMASK:
	case KLAV_SIG_ELT_RANGE:
		byte_len = 1;
		break;
	case KLAV_SIG_ELT_STAR:
		byte_len = 0; // lbound
		break;
	default:
		tns_raise_error (KLAV_EINVAL, "invalid signature element");
		break;
	}

	// recursively create next signature element
	nodeid_t nid_next = create_signature (sig_data + sz, sig_end - sig_data - sz, evid, byte_level + byte_len);
	nodeid_t nid = NODEID_INVALID;

	switch (elt.type ())
	{
	case KLAV_SIG_ELT_BYTES: {
			LNode_Data new_ld;
			new_ld.m_data = sig_data + elt.get_data_offset ();
			new_ld.m_size = elt.get_data_size ();
			new_ld.m_next = nid_next;
			nid = lnode_create (new_ld, TNS_F_TEMP, byte_level);
		} break;
	case KLAV_SIG_ELT_IBYTES:
		nid = ibyte_nodes_create (sig_data + elt.get_data_offset (), elt.get_data_size (), nid_next);
		break;
	case KLAV_SIG_ELT_ANYBYTES:
		nid = cond16_node_create (TNS_COND_ANY, elt.get_anybyte_count (), nid_next, 0, TNS_F_TEMP);
		break;
	case KLAV_SIG_ELT_BITMASK:
		nid = cond_node_create (TNS_COND_MASK, elt.get_bitmask_mask (), elt.get_bitmask_value (), nid_next, 0, TNS_F_TEMP);
		break;
	case KLAV_SIG_ELT_RANGE:
		nid = cond_node_create (TNS_COND_RANGE, elt.get_range_lbound (), elt.get_range_hbound (), nid_next, 0, TNS_F_TEMP);
		break;
	case KLAV_SIG_ELT_STAR:
		nid = cond16_node_create (TNS_COND_STAR, elt.get_star_count (), nid_next, 0, TNS_F_TEMP);
		break;
	default:
		tns_raise_error (KLAV_EINVAL, "invalid signature element");
		break;
	}

	return nid;
}

nodeid_t Tree_Sig_Merger::ibyte_nodes_create (
			const uint8_t *data,
			size_t size,
			nodeid_t nid_next
		)
{
	nodeid_t nid_first = nid_next;
	nodeid_t nid_last = NODEID_INVALID;

	for (size_t i = 0; i < size; ++i)
	{
		nodeid_t nid = cond_node_create (TNS_COND_MASK, 0xDF, (data [i] & 0xDF), 0, 0, TNS_F_TEMP);
		if (nid_last == NODEID_INVALID)
		{
			nid_first = nid;
		}
		else
		{
			cond_set_next (nid_last, nid);
		}
		nid_last = nid;
	}

	if (nid_last != NODEID_INVALID)
	{
		cond_set_next (nid_last, nid_next);
	}

	return nid_first;
}

nodeid_t Tree_Sig_Merger::merge_signature (
			nodeid_t nid_root,
			const uint8_t * sig_data,
			size_t sig_size,
			uint32_t evid,
			uint32_t node_flags
		)
{
	nodeid_t sig_nid = create_signature (sig_data, sig_size, evid, 0);

	nid_root = KLAV_Tree_Merge (node_mgr (), nid_root, sig_nid, node_flags, hash_level ());

	node_delete (sig_nid, true);

	return nid_root;
}

////////////////////////////////////////////////////////////////

KLAV_EXTERN_C
nodeid_t KLAV_CALL KLAV_Tree_Merge_Signature (
			TNS_Tree_Manager *node_mgr,
			nodeid_t nid_root,
			const uint8_t * sig_data,
			size_t sig_size,
			uint32_t evid,
			uint32_t node_flags,  // TNS_F_TEMP
			uint32_t hash_level
		)
{
	Tree_Sig_Merger sig_merger (node_mgr, hash_level);

	return sig_merger.merge_signature (nid_root, sig_data, sig_size, evid, node_flags);
}

