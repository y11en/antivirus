// printer.cpp
//
//

#include "tns_impl.h"
#include <stdio.h>
#include <crt_s.h>

////////////////////////////////////////////////////////////////

const char * Tree_Node_Type_Name (uint32_t type)
{
	switch (type)
	{
	case TNT_INVALID:
		return "INVALID";
	case TNT_INDEX:
		return "INDEX";
	case TNT_ARRAY:
		return "ARRAY";
	case TNT_JOINED:
		return "JOINED";
	case TNT_HASHED:
		return "HASHED";
	case TNT_CONDITION:
		return "CONDITION";
	case TNT_TAIL:
		return "TAIL";
	default:
		break;
	}
	return "UNKNOWN";
};

const char * Tree_NID_Format (nodeid_t nid, char *buf)
{
	unsigned int blk = NODEID_BLK (nid);
	unsigned int idx = NODEID_IDX (nid);
	unsigned int kind = NODEID_KIND (nid);
	const char * kind_name = "";
	switch (kind)
	{
	case TNS_NODEKIND_TEMP:
		kind_name = "T";
		break;
	case TNS_NODEKIND_PRIV:
		kind_name = "P";
		break;
	default:
		break;
	}

	sprintf_s (buf, TREE_NID_BUF_SIZE, "$%s%X.%X", kind_name, blk, idx);

	return buf;
}

////////////////////////////////////////////////////////////////

class Tree_Print_Context : public Tree_Manager
{
public:
	Tree_Print_Context (TNS_Tree_Manager *node_mgr, TNS_Printer *printer)
		: Tree_Manager (node_mgr, 0, 0), m_printer (printer), m_seqno (0), m_norm (0)
	{
	}

	void print_tree (nodeid_t nid_root, klav_bool_t norm);

private:
	TNS_Printer * m_printer;
	uint32_t      m_seqno;
	klav_bool_t   m_norm;

	void print (const char *fmt, ...);

	void endl ()
	{
		if (m_printer != 0)
			m_printer->endl ();
	}

	void indent (int level)
	{
		if (m_printer != 0)
			m_printer->indent (level);
	}

	void node_print (
				nodeid_t nid,
				const char *parent_name
			);

	void node_data_print (
				uint32_t   hdr,
				const uint8_t * node_data
			);

	void node_childs_print (
				uint32_t   hdr,
				const uint8_t * node_data,
				uint32_t   node_no
			);

	void index_data_print (
				uint32_t   hdr,
				const uint8_t * node_data
			);

	void array_data_print (
				uint32_t   hdr,
				const uint8_t * node_data
		);

	void index_childs_print (
				uint32_t   hdr,
				const uint8_t * node_data,
				uint32_t   node_no
			);

	void array_childs_print (
				uint32_t   hdr,
				const uint8_t * node_data,
				uint32_t   node_no
			);

	void join_data_print (
				uint32_t   hdr,
				const uint8_t * node_data
			);

	void join_childs_print (
				uint32_t   hdr,
				const uint8_t * node_data,
				uint32_t   node_no
			);

	void hash_data_print (
				uint32_t   hdr,
				const uint8_t * node_data
			);

	void hash_childs_print (
				uint32_t   hdr,
				const uint8_t * node_data,
				uint32_t   node_no
			);

	void cond_data_print (
				uint32_t   hdr,
				const uint8_t * node_data
			);

	void cond_childs_print (
				uint32_t   hdr,
				const uint8_t * node_data,
				uint32_t   node_no
			);
};

////////////////////////////////////////////////////////////////

KLAV_EXTERN_C
void KLAV_CALL KLAV_Tree_Print (
			TNS_Tree_Manager * node_mgr,
			nodeid_t nid_root,
			TNS_Printer * printer,
			klav_bool_t norm
		)
{
	Tree_Print_Context print_ctx (node_mgr, printer);
	print_ctx.print_tree (nid_root, norm);
}

////////////////////////////////////////////////////////////////

void Tree_Print_Context::print (const char *fmt, ...)
{
	if (m_printer != 0)
	{
		va_list ap;
		va_start (ap, fmt);
		m_printer->print (fmt, ap);
		va_end (ap);
	}
}

void Tree_Print_Context::print_tree (nodeid_t root_nid, klav_bool_t norm)
{
	m_seqno = 0;
	m_norm = norm;
	node_print (root_nid, "ROOT");
}

void Tree_Print_Context::node_print (nodeid_t nid, const char *parent_name)
{
	if (nid == NODEID_INVALID)
	{
		print ("<BAD_NODE>");
		endl ();
		return;
	}

	Tree_Node node (node_mgr (), nid);
	const char *node_name = Tree_Node_Type_Name (TNS_NODE_TYPE (node.hdr ()));

	char nidbuf [TREE_NID_BUF_SIZE];

	uint32_t node_no = ++m_seqno;

	if (m_norm)
		print ("%u (%s) <%s>", node_no, parent_name, node_name);
	else
		print ("%s <%s>", Tree_NID_Format (nid, nidbuf), node_name);

	nodeid_t data_ref = node.data_ref ();
	if (data_ref != NODEID_INVALID)
	{
		if (m_norm)
			print (" DATA [0x%X]", node.data_size ());
		else
			print (" DATA:%s [0x%X]", Tree_NID_Format (data_ref, nidbuf), node.data_size ());
	}

	uint32_t evid = node.evid ();
	if (evid != 0)
	{
		print (" EVID:0x%08X", evid);
	}

	nodeid_t nid_chain = node.chain ();
	if (nid_chain != NODEID_INVALID)
	{
		if (m_norm)
			print (" CHAIN");
		else
			print (" CHAIN:%s", Tree_NID_Format (nid_chain, nidbuf));
	}

	endl ();

	if (node.data_ref () != NODEID_INVALID)
	{
		indent (1);
		node_data_print (node.hdr (), node.data ());
		indent (-1);
	}

	if (nid_chain != NODEID_INVALID)
	{
//		print ("CHAIN %s ", Tree_NID_Format (nid_chain, nidbuf));
//		print ("(from %s)", Tree_NID_Format (nid, nidbuf));
//		endl ();

		if (! m_norm)
		{
			indent (1);
			nodeid_t cnid = nid_chain;
			while (cnid != NODEID_INVALID)
			{
				Tree_Node chain_node (node_mgr (), cnid);
				print ("COND: %s", Tree_NID_Format (cnid, nidbuf));
				endl ();
				cnid = chain_node.chain ();
			}
			indent (-1);
		}
	}

	endl ();

	// print childs and chained nodes
	const uint8_t * node_data = 0;
	if (node.data_ref () != NODEID_INVALID)
		node_data = node.data ();

	node_childs_print (node.hdr (), node_data, node_no);

	if (nid_chain != NODEID_INVALID)
	{
		char parent_name_buf [50];
		sprintf_s (parent_name_buf, sizeof (parent_name_buf), "%u.COND", node_no);

		node_print (nid_chain, parent_name_buf);
	}
}

void Tree_Print_Context::node_data_print (
			uint32_t   hdr,
			const uint8_t * node_data
		)
{
	switch (TNS_NODE_TYPE (hdr))
	{
	case TNT_INDEX:
		index_data_print (hdr, node_data);
		break;
	case TNT_ARRAY:
		array_data_print (hdr, node_data);
		break;
	case TNT_JOINED:
		join_data_print (hdr, node_data);
		break;
	case TNT_HASHED:
		hash_data_print (hdr, node_data);
		break;
	case TNT_TAIL:
		break;
	case TNT_CONDITION:
		cond_data_print (hdr, node_data);
		break;
	default:
		print ("<CANNOT PRINT THIS NODE DATA>");
		endl ();
		break;
	}
}

void Tree_Print_Context::node_childs_print (
			uint32_t   hdr,
			const uint8_t * node_data,
			uint32_t node_no
		)
{
	switch (TNS_NODE_TYPE (hdr))
	{
	case TNT_INDEX:
		index_childs_print (hdr, node_data, node_no);
		break;
	case TNT_ARRAY:
		array_childs_print (hdr, node_data, node_no);
		break;
	case TNT_JOINED:
		join_childs_print (hdr, node_data, node_no);
		break;
	case TNT_HASHED:
		hash_childs_print (hdr, node_data, node_no);
		break;
	case TNT_TAIL:
		break;
	case TNT_CONDITION:
		cond_childs_print (hdr, node_data, node_no);
		break;
	default:
		print ("<CANNOT PRINT THIS NODE CHILDS>");
		endl ();
		break;
	}
}

void Tree_Print_Context::index_data_print (
			uint32_t   hdr,
			const uint8_t * node_data
		)
{
	const nodeid_t * items = (const uint32_t *) node_data;

	char nidbuf [TREE_NID_BUF_SIZE];

	for (uint32_t i = 0; i < 256; ++i)
	{
		nodeid_t child_nid = items [i];
		if (child_nid != NODEID_INVALID)
		{
			if (m_norm)
				print ("0x%02X", i);
			else
				print ("0x%02X - %s", i, Tree_NID_Format (child_nid, nidbuf));
			endl ();
		}
	}
}

void Tree_Print_Context::index_childs_print (
			uint32_t   hdr,
			const uint8_t * node_data,
			uint32_t   node_no
		)
{
	const nodeid_t * items = (const uint32_t *) node_data;
	char parent_name_buf [50];

	for (uint32_t i = 0; i < 256; ++i)
	{
		nodeid_t child_nid = items [i];
		if (child_nid != NODEID_INVALID)
		{
			sprintf_s (parent_name_buf, sizeof (parent_name_buf), "%u[%02X]", node_no, i);
			node_print (child_nid, parent_name_buf);
		}
	}
}

void Tree_Print_Context::array_data_print (
			uint32_t   hdr,
			const uint8_t * node_data
		)
{
	uint32_t cnt = TNS_NODE_SIZE (hdr) / 5;
	if (cnt == 0)
		return;

	char nidbuf [TREE_NID_BUF_SIZE];

	const uint8_t * bytes = node_data;
	nodeid_t * items = (uint32_t *) (node_data + TNS_ALIGN4 (cnt));

	for (uint32_t i = 0; i < cnt; ++i)
	{
		if (m_norm)
			print ("0x%02X", bytes [i]);
		else
			print ("0x%02X - %s", bytes [i], Tree_NID_Format (items [i], nidbuf));
		endl ();
	}
}

void Tree_Print_Context::array_childs_print (
			uint32_t   hdr,
			const uint8_t * node_data,
			uint32_t   node_no
		)
{
	uint32_t cnt = TNS_NODE_SIZE (hdr) / 5;
	if (cnt == 0)
		return;

	char parent_name_buf [50];

	const uint8_t * bytes = node_data;
	const nodeid_t * items = (const uint32_t *) (node_data + TNS_ALIGN4 (cnt));

	for (uint32_t i = 0; i < cnt; ++i)
	{
		sprintf_s (parent_name_buf, sizeof (parent_name_buf), "%u[%02X]", node_no, bytes[i]);
		node_print (items [i], parent_name_buf);
	}
}

void Tree_Print_Context::join_data_print (
			uint32_t        hdr,
			const uint8_t * node_data
		)
{
	TNS_ASSERT (TNS_NODE_TYPE (hdr) == TNT_JOINED);
	TNS_ASSERT (node_data != 0);
	TNS_ASSERT (TNS_NODE_SIZE (hdr) > sizeof (TNS_JOIN_DATA));

	const TNS_JOIN_DATA * data = (const TNS_JOIN_DATA *) node_data;

	char nidbuf [TREE_NID_BUF_SIZE];

	if (! m_norm)
	{
		print ("NEXT: %s", Tree_NID_Format (data->ndj_next, nidbuf));
		endl ();
	}

	const uint8_t * bp = node_data + sizeof (TNS_JOIN_DATA);
	uint32_t len = TNS_NODE_SIZE (hdr) - sizeof (TNS_JOIN_DATA);

	print ("DATA [0x%X]:", len);

	for (uint32_t i = 0; i < len; ++i)
	{
		print (" %02X", bp [i]);
	}

	endl ();
}

void Tree_Print_Context::join_childs_print (
			uint32_t   hdr,
			const uint8_t * node_data,
			uint32_t   node_no
		)
{
	TNS_ASSERT (TNS_NODE_TYPE (hdr) == TNT_JOINED);
	TNS_ASSERT (node_data != 0);
	TNS_ASSERT (TNS_NODE_SIZE (hdr) > sizeof (TNS_JOIN_DATA));

	const TNS_JOIN_DATA * data = (const TNS_JOIN_DATA *) node_data;

	char parent_name_buf [50];
	sprintf_s (parent_name_buf, sizeof (parent_name_buf), "%u.NXT", node_no);

	node_print (data->ndj_next, parent_name_buf);
}

void Tree_Print_Context::hash_data_print (
			uint32_t        hdr,
			const uint8_t * node_data
		)
{
	TNS_ASSERT (TNS_NODE_TYPE (hdr) == TNT_HASHED);
	TNS_ASSERT (node_data != 0);
	TNS_ASSERT (TNS_NODE_SIZE (hdr) == sizeof (TNS_HASH_DATA));

	const TNS_HASH_DATA * data = (const TNS_HASH_DATA *) node_data;

	char nidbuf [TREE_NID_BUF_SIZE];

	uint32_t len = data->ndh_len;
	print ("LEN:0x%X, HASH:0x%X", len, data->ndh_hash);
	if (! m_norm)
		print (", NEXT:%s", Tree_NID_Format (data->ndh_next, nidbuf));
	endl ();

	if (m_norm)
		print ("DATA:");
	else
		print ("DATA(%s):", Tree_NID_Format (data->ndh_data, nidbuf));

	const uint8_t * bp = node_mgr ()->read_node_data (data->ndh_data, false);

	for (uint32_t i = 0; i < len; ++i)
	{
		print (" %02X", bp [i]);
	}

	endl ();
}

void Tree_Print_Context::hash_childs_print (
			uint32_t   hdr,
			const uint8_t * node_data,
			uint32_t   node_no
		)
{
	TNS_ASSERT (TNS_NODE_TYPE (hdr) == TNT_HASHED);
	TNS_ASSERT (node_data != 0);
	TNS_ASSERT (TNS_NODE_SIZE (hdr) == sizeof (TNS_HASH_DATA));

	const TNS_HASH_DATA * data = (const TNS_HASH_DATA *) node_data;

	char parent_name_buf [50];
	sprintf_s (parent_name_buf, sizeof (parent_name_buf), "%u.NXT", node_no);

	node_print (data->ndh_next, parent_name_buf);
}

void Tree_Print_Context::cond_data_print (
			uint32_t hdr,
			const uint8_t * node_data
		)
{
	TNS_ASSERT (TNS_NODE_TYPE (hdr) == TNT_CONDITION);
	TNS_ASSERT (node_data != 0);
	TNS_ASSERT (TNS_NODE_SIZE (hdr) == sizeof (TNS_COND_DATA));

	const TNS_COND_DATA * data = (const TNS_COND_DATA *) node_data;

	char nidbuf [TREE_NID_BUF_SIZE];
	
	switch (data->ndc_type)
	{
	case TNS_COND_MASK:
		print ("[&%02X=%02X]", data->ndc_parm1, data->ndc_parm2);
		break;
	case TNS_COND_RANGE:
		print ("[R%02X-%02X]", data->ndc_parm1, data->ndc_parm2);
		break;
	case TNS_COND_ANY: {
		unsigned int cnt = ((((unsigned int)data->ndc_parm2) << 8) | data->ndc_parm1);
		print ("[? %u]", cnt);
		} break;
	case TNS_COND_STAR: {
		unsigned int cnt = ((((unsigned int)data->ndc_parm2) << 8) | data->ndc_parm1);
		print ("[* %u]", cnt);
		} break;
	default:
		print ("<UNKNOWN CONDITION CODE>");
		break;
	}

	if (! m_norm)
	{
		print (" NEXT: %s", Tree_NID_Format (data->ndc_next, nidbuf));
	}

	endl ();
}

void Tree_Print_Context::cond_childs_print (
			uint32_t hdr,
			const uint8_t * node_data,
			uint32_t node_no
		)
{
	TNS_ASSERT (TNS_NODE_TYPE (hdr) == TNT_CONDITION);
	TNS_ASSERT (node_data != 0);
	TNS_ASSERT (TNS_NODE_SIZE (hdr) == sizeof (TNS_COND_DATA));

	const TNS_COND_DATA * data = (const TNS_COND_DATA *) node_data;

	char parent_name_buf [50];
	sprintf_s (parent_name_buf, sizeof (parent_name_buf), "%u.NXT", node_no);

	node_print (data->ndc_next, parent_name_buf);
}

