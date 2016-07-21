// finder.cpp
//
//

#include "tns_impl.h"
#include <klav_sig.h>
#include <string.h>

////////////////////////////////////////////////////////////////

struct Sig_Iter
{
	nodeid_t m_nid;
	uint32_t m_pos;

	Sig_Iter (nodeid_t nid = NODEID_INVALID, uint32_t pos = 0)
		: m_nid (nid), m_pos (pos)
	{
	}
};

////////////////////////////////////////////////////////////////

class Tree_Sig_Finder : public Tree_Manager
{
public:
	Tree_Sig_Finder (TNS_Tree_Manager * node_mgr)
		: Tree_Manager (node_mgr, 0, 0)
	{
	}

	uint32_t find_signature (nodeid_t nid_root, const uint8_t * data, size_t size);

private:
	bool find_bytes (
			Sig_Iter& iter,
			const uint8_t * sig_data,
			size_t sig_size
		);

	bool find_cond (
			Sig_Iter& iter,
			uint8_t cond_type,
			uint8_t cond_parm1,
			uint8_t cond_parm2
		);

	bool find_cond16 (
			Sig_Iter& iter,
			uint8_t cond_type,
			uint16_t cond_parm
		);
};

////////////////////////////////////////////////////////////////

KLAV_EXTERN_C
uint32_t KLAV_CALL KLAV_Tree_Find_Signature (
			TNS_Tree_Manager * node_mgr,
			nodeid_t nid_root,
			const uint8_t * sig_data,
			size_t sig_size
		)
{
	Tree_Sig_Finder finder (node_mgr);

	return finder.find_signature (nid_root, sig_data, sig_size);
}

////////////////////////////////////////////////////////////////

uint32_t Tree_Sig_Finder::find_signature (
			nodeid_t nid_root,
			const uint8_t * sig_data,
			size_t sig_size
		)
{
	Sig_Iter iter (nid_root, 0);

	const uint8_t * sig = sig_data;
	const uint8_t * sig_end = sig_data + sig_size;

	while (sig < sig_end)
	{
		KLAV_Sig_Element elt;

		size_t sz = elt.scan (sig, sig_end - sig);
		if (sz == 0)
			tns_raise_error (KLAV_EINVAL, "invalid signature data");

		switch (elt.type ())
		{
		case KLAV_SIG_ELT_BYTES: {
				if (! find_bytes (iter, sig + elt.get_data_offset (), elt.get_data_size ()))
					return false;
			} break;
		case KLAV_SIG_ELT_IBYTES: {
				const uint8_t * ibytes = sig + elt.get_data_offset ();
				uint32_t size = elt.get_data_size ();
				for (uint32_t i = 0; i < size; ++i)
				{
					if (! find_cond (iter, TNS_COND_MASK, 0xDF, (ibytes [i] & 0xDF)))
						return false;
				}
			} break;
		case KLAV_SIG_ELT_ANYBYTES:
			if (! find_cond16 (iter, TNS_COND_ANY, elt.get_anybyte_count ()))
				return false;
			break;
		case KLAV_SIG_ELT_BITMASK:
			if (! find_cond (iter, TNS_COND_MASK, elt.get_bitmask_mask (), elt.get_bitmask_value ()))
				return false;
			break;
		case KLAV_SIG_ELT_RANGE:
			if (! find_cond (iter, TNS_COND_RANGE, elt.get_range_lbound (), elt.get_range_hbound ()))
				return false;
			break;
		case KLAV_SIG_ELT_STAR:
			if (! find_cond16 (iter, TNS_COND_ANY, elt.get_star_count ()))
				return false;
			break;
		default:
			tns_raise_error (KLAV_EINVAL, "invalid signature element");
			break;
		}
		
		sig += sz;
	}

	if (iter.m_nid == NODEID_INVALID || iter.m_pos != 0)
		return 0;

	Tree_Node node (node_mgr (), iter.m_nid);
	return node.evid ();
}

bool Tree_Sig_Finder::find_bytes (
			Sig_Iter& iter,
			const uint8_t * sig_data,
			size_t sig_size
		)
{
	if (iter.m_nid == NODEID_INVALID)
		return false;

	if (sig_size == 0)
		return true;

	Tree_Node node (node_mgr (), iter.m_nid);

	const uint8_t * node_data = 0;
	if (node.data_ref () != NODEID_INVALID)
		node_data = node.data ();

	switch (node.type ())
	{
	case TNT_INDEX:
	case TNT_ARRAY: {
			TNS_ASSERT (iter.m_pos == 0);
			nodeid_t next = index_get_item (iter.m_nid, sig_data [0]);
			if (next == NODEID_INVALID)
				return false;

			iter.m_nid = next;
			iter.m_pos = 0;

			return find_bytes (iter, sig_data + 1, sig_size - 1);
		} break;

	case TNT_JOINED: {
			uint32_t data_size = node.data_size ();
			TNS_ASSERT (data_size > sizeof (TNS_JOIN_DATA));

			const TNS_JOIN_DATA * hdr = (const TNS_JOIN_DATA *) node_data;

			node_data += sizeof (TNS_JOIN_DATA);
			data_size -= sizeof (TNS_JOIN_DATA);

			TNS_ASSERT (iter.m_pos < data_size);
			
			const uint8_t * portion_data = node_data + iter.m_pos;
			uint32_t portion_size = data_size - iter.m_pos;

			if ((size_t) portion_size > sig_size)
				portion_size = (uint32_t) sig_size;

			if (memcmp (sig_data, portion_data, portion_size) != 0)
				return false;

			iter.m_pos += portion_size;

			if (iter.m_pos == data_size)
			{
				iter.m_nid = hdr->ndj_next;
				iter.m_pos = 0; 
			}

			if (sig_size == portion_size)
				return true;

			return find_bytes (iter, sig_data + portion_size, sig_size - portion_size);
		} break;

	case TNT_HASHED: {
			const TNS_HASH_DATA * hdr = (const TNS_HASH_DATA *) node_data;

			uint32_t data_size = hdr->ndh_len;
			const uint8_t * node_data = node_mgr ()->read_node_data (hdr->ndh_data, false);

			TNS_ASSERT (iter.m_pos < data_size);
			
			const uint8_t * portion_data = node_data + iter.m_pos;
			uint32_t portion_size = data_size - iter.m_pos;

			if ((size_t) portion_size > sig_size)
				portion_size = (uint32_t) sig_size;

			if (memcmp (sig_data, portion_data, portion_size) != 0)
				return false;

			iter.m_pos += portion_size;

			if (iter.m_pos == data_size)
			{
				iter.m_nid = hdr->ndh_next;
				iter.m_pos = 0; 
			}

			if (sig_size == portion_size)
				return true;

			return find_bytes (iter, sig_data + portion_size, sig_size - portion_size);
		} break;

	case TNT_TAIL:
		break;

	case TNT_CONDITION:
		break;

	default:
		tns_raise_error (KLAV_EINVAL, "unexpected node type");
		break;
	}

	return 0;
}

bool Tree_Sig_Finder::find_cond (
			Sig_Iter& iter,
			uint8_t cond_type,
			uint8_t cond_parm1,
			uint8_t cond_parm2
		)
{
	if (iter.m_nid == NODEID_INVALID)
		return false;

	if (iter.m_pos != 0)
		return false;

	while (iter.m_nid != NODEID_INVALID)
	{
		Tree_Node node (node_mgr (), iter.m_nid);

		if (node.type () == TNT_CONDITION)
		{
			const TNS_COND_DATA * cond = (const TNS_COND_DATA *) node.data ();
			
			if (cond->ndc_type == cond_type
			 && cond->ndc_parm1 == cond_parm1
			 && cond->ndc_parm2 == cond_parm2)
			{
				iter.m_nid = cond->ndc_next;
				return true;
			}
		}

		iter.m_nid = node.chain ();
	}
	
	return false;
}

bool Tree_Sig_Finder::find_cond16 (
			Sig_Iter& iter,
			uint8_t cond_type,
			uint16_t cond_parm
		)
{
	uint8_t parm1 = (uint8_t) (cond_parm & 0xFF);
	uint8_t parm2 = (uint8_t) ((cond_parm >> 8) & 0xFF);
	return find_cond (iter, cond_type, parm1, parm2);
}

