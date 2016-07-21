// validate.cpp
//
//

#include "tns_impl.h"

#define TDX_INVALID(NID,MSG) print_invalid_node (NID, MSG), f_valid = false
#define TDX_VALIDATE(NID, EXPR) if (!(EXPR)) TDX_INVALID (NID,"FAILED:" #EXPR)

////////////////////////////////////////////////////////////////

class Tree_Validator : public Tree_Manager
{
public:
	Tree_Validator (
			TNS_Tree_Manager * node_mgr,
			TNS_Printer * printer,
			uint32_t node_flags,
			uint32_t hash_level,
			uint32_t val_mode
		) : Tree_Manager (node_mgr, node_flags, hash_level),
		    m_printer (printer),
		    m_val_mode (val_mode)
	{
	}

	klav_bool_t validate_tree (nodeid_t nid_root);

private:
	TNS_Printer *   m_printer;
	uint32_t        m_val_mode;

	void print (const char *fmt, ...);

	void endl ()
	{
		if (m_printer != 0)
			m_printer->endl ();
	}

	void indent (int level)
	{
		if (m_printer != 0)
			indent (level);
	}

	void print_invalid_node (
			nodeid_t nid,
			const char *text
		);

	bool node_validate (
			nodeid_t nid,
			uint32_t byte_level
		);

	bool index_node_validate (
			Tree_Node& node,
			uint32_t byte_level
		);

	bool array_node_validate (
			Tree_Node& node,
			uint32_t byte_level
		);

	bool join_node_validate (
			Tree_Node& node,
			uint32_t byte_level
		);

	bool hash_node_validate (
			Tree_Node& node,
			uint32_t byte_level
		);

	bool tail_node_validate (
			Tree_Node& node,
			uint32_t byte_level
		);

	bool cond_node_validate (
			Tree_Node& node,
			uint32_t byte_level
		);

	bool cond_chain_validate (
			nodeid_t nid,
			uint32_t byte_level
		);
};

////////////////////////////////////////////////////////////////

KLAV_EXTERN_C
klav_bool_t KLAV_CALL KLAV_Tree_Validate (
			TNS_Tree_Manager * node_mgr,
			nodeid_t nid_root,
			TNS_Printer * printer,
			uint32_t tree_flags,
			uint32_t hash_level,
			uint32_t mode
		)
{
	Tree_Validator validator (node_mgr, printer, tree_flags, hash_level, mode);
	return validator.validate_tree (nid_root);
}

////////////////////////////////////////////////////////////////

void Tree_Validator::print (const char *fmt, ...)
{
	if (m_printer != 0)
	{
		va_list ap;
		va_start (ap, fmt);
		m_printer->print (fmt, ap);
		va_end (ap);
	}
}

void Tree_Validator::print_invalid_node (
			nodeid_t nid,
			const char *text
		)
{
	char nidbuf [TREE_NID_BUF_SIZE];

	print ("INVALID NODE: %s (%s)", Tree_NID_Format (nid, nidbuf), text);
	endl ();
}

klav_bool_t Tree_Validator::validate_tree (nodeid_t nid_root)
{
	if (nid_root == NODEID_INVALID)
	{
		print ("TREE OK (EMPTY)");
		endl ();
		return true;
	}

	bool f = node_validate (nid_root, 0);

	if (! f)
	{
		print ("TREE INVALID");
		endl ();
	}
	else
	{
		print ("TREE OK");
		endl ();
	}

	return f;
}

bool Tree_Validator::node_validate (
			nodeid_t nid,
			uint32_t byte_level
		)
{
	bool f_valid = true;

	if (nid == NODEID_INVALID)
	{
		TDX_INVALID (nid, "invalid node ID");
		return false;
	}

	Tree_Node node (node_mgr (), nid);

	uint32_t hdr = node.hdr ();

	switch (m_val_mode)
	{
	case KLAV_TREE_VALIDATE_MARK_USED:
		TNS_NODE_SET_FLAG (hdr, USED);
		break;
	case KLAV_TREE_VALIDATE_CLEAR_USED:
		TNS_NODE_CLR_FLAG (hdr, USED);
		break;
	default:
		break;
	}

	if (hdr != node.hdr ())
		node.set_hdr (hdr);

	switch (node.type ())
	{
	case TNT_INDEX:
		f_valid = index_node_validate (node, byte_level);
		break;
	case TNT_ARRAY:
		f_valid = array_node_validate (node, byte_level);
		break;
	case TNT_JOINED:
		f_valid = join_node_validate (node, byte_level);
		break;
	case TNT_HASHED:
		f_valid = hash_node_validate (node, byte_level);
		break;
	case TNT_CONDITION:
		f_valid = cond_node_validate (node, byte_level);
		break;
	case TNT_TAIL:
		f_valid = tail_node_validate (node, byte_level);
		break;
	default:
		TDX_INVALID (nid, "Invalid node type");
		break;
	}

	if (! is_temp_tree () && node.evid () == EVID_DELETED)
	{
		TDX_INVALID (nid, "EVID_DELETED in final tree");
	}

	if (! cond_chain_validate (node.chain (), byte_level))
		f_valid = false;

	return f_valid;
}

bool Tree_Validator::cond_chain_validate (
			nodeid_t nid,
			uint32_t byte_level
		)
{
	bool f_valid = true;

	while (nid != NODEID_INVALID)
	{
		Tree_Node node (node_mgr (), nid);
		if (node.type () != TNT_CONDITION)
		{
			TDX_INVALID (nid, "non-condition node in chain");
		}
		if (! node_validate (nid, byte_level))
			f_valid = false;
	
		nid = node.chain ();
	}

	return f_valid;
}

bool Tree_Validator::index_node_validate (
			Tree_Node& node,
			uint32_t byte_level
		)
{
	nodeid_t nid = node.id ();
	bool f_valid = true;

	TDX_VALIDATE (nid, node.type () == TNT_INDEX);
	TDX_VALIDATE (nid, node.data_ref () != NODEID_INVALID);
	TDX_VALIDATE (nid, node.data_size () == 256 * 4);

	const uint32_t * child_nodes = (const uint32_t *) node.data ();
	uint32_t i, child_cnt = 0;

	for (i = 0; i < 256; ++i)
	{
		if (child_nodes [i] != NODEID_INVALID)
			++ child_cnt;
	}

	TDX_VALIDATE (nid, child_cnt > TNS_ARRAY_MAX_SIZE);

	for (i = 0; i < 256; ++i)
	{
		if (child_nodes [i] != NODEID_INVALID)
		{
			if (! node_validate (child_nodes [i], byte_level + 1))
				f_valid = false;
		}
	}

	return f_valid;
}

bool Tree_Validator::array_node_validate (
			Tree_Node& node,
			uint32_t byte_level
		)
{
	nodeid_t nid = node.id ();
	bool f_valid = true;

	TDX_VALIDATE (nid, node.type () == TNT_ARRAY);
	TDX_VALIDATE (nid, node.data_ref () != NODEID_INVALID);
	TDX_VALIDATE (nid, node.data_size () <= 256 * 5);

	uint32_t child_cnt = node.data_size () / 5;
	TDX_VALIDATE (nid, child_cnt > 1);

	uint32_t good_size = TNS_ALIGN4 (child_cnt) + (4 * child_cnt);
	TDX_VALIDATE (nid, node.data_size () == good_size);

	const uint8_t  * child_bytes = node.data ();
	const uint32_t * child_nodes = (uint32_t *) (child_bytes + TNS_ALIGN4 (child_cnt));

	int last_byte = -1;

	for (uint32_t i = 0; i < child_cnt; ++i)
	{
		TDX_VALIDATE (nid, child_nodes [i] != NODEID_INVALID);

		if (child_bytes [i] <= last_byte)
			TDX_INVALID (nid, "invalid array child ordering");

		last_byte = child_bytes [i];

		if (! node_validate (child_nodes [i], byte_level + 1))
			f_valid = false;
	}

	return f_valid;
}

bool Tree_Validator::join_node_validate (
			Tree_Node& node,
			uint32_t byte_level
		)
{
	nodeid_t nid = node.id ();
	bool f_valid = true;

	TDX_VALIDATE (nid, node.type () == TNT_JOINED);
	TDX_VALIDATE (nid, node.data_ref () != NODEID_INVALID);
	TDX_VALIDATE (nid, node.data_size () > sizeof (TNS_JOIN_DATA));
	TDX_VALIDATE (nid, node.data_size () <= TNS_UNIT_SIZE_MAX);

	const TNS_JOIN_DATA * join_data = (const TNS_JOIN_DATA *) node.data ();
	TDX_VALIDATE (nid, join_data->ndj_next != NODEID_INVALID);

	uint32_t data_len = node.data_size () - sizeof (TNS_JOIN_DATA);

	Tree_Node next_node (node_mgr (), join_data->ndj_next);

	// check if the joined node is used correctly instead of hashed
	if (data_len >= TNS_HASH_SIZE_MIN && hash_level () != 0 && byte_level + data_len > hash_level ())
	{
		// the only case is when the next node is not HASHED
		// and the excess length is lesser than min.hashed node size
		uint32_t excess = byte_level + data_len - hash_level ();
		if (! ((next_node.type () != TNT_HASHED || next_node.evid () != 0)
		  && excess < TNS_HASH_SIZE_MIN))
		{
			TDX_INVALID (nid, "JOINED node used instead of HASHED");
		}
	}

	if (next_node.type () == TNT_JOINED
		&& next_node.evid () == 0
		&& next_node.chain () == NODEID_INVALID)
	{
		TDX_VALIDATE (next_node.id (), next_node.data_ref () != NODEID_INVALID);
		TDX_VALIDATE (next_node.id (), next_node.data_size () > sizeof (TNS_JOIN_DATA));

		uint32_t next_data_len = next_node.data_size () - sizeof (TNS_JOIN_DATA);
		
		if (data_len + next_data_len <= TNS_JOIN_SIZE_MAX)
		{
			TDX_INVALID (nid, "next node is a pure JOINED node, should have been merged");
		}
	}

	if (! node_validate (next_node.id (), byte_level + data_len))
		f_valid = false;

	return f_valid;
}

bool Tree_Validator::hash_node_validate (
			Tree_Node& node,
			uint32_t byte_level
		)
{
	nodeid_t nid = node.id ();
	bool f_valid = true;

	TDX_VALIDATE (nid, node.type () == TNT_HASHED);
	TDX_VALIDATE (nid, node.data_ref () != NODEID_INVALID);
	TDX_VALIDATE (nid, node.data_size () == sizeof (TNS_HASH_DATA));

	const TNS_HASH_DATA * hash_data = (const TNS_HASH_DATA *) node.data ();
	TDX_VALIDATE (nid, hash_data->ndh_len  >= TNS_HASH_SIZE_MIN);
	TDX_VALIDATE (nid, hash_data->ndh_len <= TNS_HASH_SIZE_MAX);
	TDX_VALIDATE (nid, hash_data->ndh_data != NODEID_INVALID);
	TDX_VALIDATE (nid, NODEID_KIND(hash_data->ndh_data) == TNS_NODEKIND_PRIV);
	TDX_VALIDATE (nid, hash_data->ndh_next != NODEID_INVALID);
	TDX_VALIDATE (nid, NODEID_KIND(hash_data->ndh_next) == TNS_NODEKIND_NORM);

	if (m_val_mode != KLAV_TREE_VALIDATE_FINAL_NORM)
	{
		// calculate hash and compare it to stored
		const uint8_t * bp = node_mgr ()->read_node_data (hash_data->ndh_data, false);
		uint32_t data_hash = KlavCRC32 (bp, hash_data->ndh_len, 0);
		TDX_VALIDATE (nid, hash_data->ndh_hash == data_hash);
	}

	Tree_Node next_node (node_mgr (), hash_data->ndh_next);

	// check if HASHED node is used correctly instead of JOINED
	if (hash_level () == 0 && byte_level < hash_level ())
	{
		TDX_INVALID (nid, "HASHED node used instead of JOINED");
	}

	if (next_node.type () == TNT_HASHED
		&& next_node.evid () == 0
		&& next_node.chain () == NODEID_INVALID)
	{
		TDX_VALIDATE (next_node.id (), next_node.data_ref () != NODEID_INVALID);
		const TNS_HASH_DATA * next_hash_data = (const TNS_HASH_DATA *) next_node.data ();

		if (hash_data->ndh_len + next_hash_data->ndh_len <= TNS_HASH_SIZE_MAX)
		{
			TDX_INVALID (nid, "next node is a pure HASHED node, should have been merged");
		}
	}

	if (! node_validate (next_node.id (), byte_level + hash_data->ndh_len))
		f_valid = false;

	return f_valid;
}

bool Tree_Validator::tail_node_validate (
			Tree_Node& node,
			uint32_t byte_level
		)
{
	nodeid_t nid = node.id ();
	bool f_valid = true;

	TDX_VALIDATE (nid, node.type () == TNT_TAIL);
	TDX_VALIDATE (nid, node.data_ref () == NODEID_INVALID);
	TDX_VALIDATE (nid, node.evid () != 0);

	return f_valid;
}

bool Tree_Validator::cond_node_validate (
			Tree_Node& node,
			uint32_t byte_level
		)
{
	nodeid_t nid = node.id ();
	bool f_valid = true;

	TDX_VALIDATE (nid, node.type () == TNT_CONDITION);
	TDX_VALIDATE (nid, node.data_ref () != NODEID_INVALID);
	TDX_VALIDATE (nid, node.data_size () == sizeof (TNS_COND_DATA));

	const TNS_COND_DATA * cond_data = (const TNS_COND_DATA *) node.data ();

	TDX_VALIDATE (nid, cond_data->ndc_next != NODEID_INVALID);

	uint32_t byte_count = 0;

	switch (cond_data->ndc_type)
	{
	case TNS_COND_MASK: {
			uint8_t mask = cond_data->ndc_parm1;
			uint8_t value = cond_data->ndc_parm2;
			TDX_VALIDATE (nid, mask != 0);
			TDX_VALIDATE (nid, mask != 0xFF);
			TDX_VALIDATE (nid, (value & ~mask) == 0);
			byte_count = 1;
		} break;
	case TNS_COND_RANGE: {
			uint8_t r_beg = cond_data->ndc_parm1;
			uint8_t r_end = cond_data->ndc_parm2;
			TDX_VALIDATE (nid, r_beg < r_end);
			byte_count = 1;
		} break;
	case TNS_COND_ANY: {
			uint32_t any_cnt = ((((uint32_t)cond_data->ndc_parm2) << 8) | cond_data->ndc_parm1);
			TDX_VALIDATE (nid, any_cnt != 0);
			TDX_VALIDATE (nid, node.chain () == NODEID_INVALID);
			byte_count = any_cnt;
		} break;
	case TNS_COND_STAR: {
			uint32_t any_cnt = ((((uint32_t)cond_data->ndc_parm2) << 8) | cond_data->ndc_parm1);
			TDX_VALIDATE (nid, any_cnt != 0);
			TDX_VALIDATE (nid, node.chain () == NODEID_INVALID);
			byte_count = 0;
		} break;
	default:
		TDX_INVALID (nid, "invalid condition code");
		break;
	}

	if (! node_validate (cond_data->ndc_next, byte_level + byte_count))
		f_valid = false;

	return f_valid;
}

