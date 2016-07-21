// nodes_rd.cpp
//
// read-only node management methods
//

#include "tns_impl.h"

// index node management

nodeid_t Tree_Manager::index_get_item (
			nodeid_t nid,
			uint8_t  byte
		)
{
	Tree_Node node (node_mgr (), nid);
	uint32_t type = node.type ();

	if (type == TNT_INDEX)
	{
		const nodeid_t * items = (const uint32_t *) node.data ();
		return items [byte];
	}
	else if (type == TNT_ARRAY)
	{
		uint32_t cnt = TNS_NODE_SIZE (node.hdr ()) / 5;
		if (cnt == 0)
			return NODEID_INVALID;

		const uint8_t * data = node.data ();

		nodeid_t * items = (uint32_t *) (data + TNS_ALIGN4 (cnt));

		for (uint32_t i = 0; i < cnt; ++i)
		{
			if (data [i] == byte)
				return items [i];
		}

		return NODEID_INVALID;
	}

	tns_raise_error (KLAV_EINVAL, "index_get_item: invalid node type");

	return NODEID_INVALID;
}

uint8_t Tree_Manager::index_get_single_item (
			nodeid_t nid,
			nodeid_t *pitem_nid
		)
{
	Tree_Node node (node_mgr (), nid);
	uint32_t type = node.type ();

	if (type == TNT_INDEX)
	{
		const nodeid_t * items = (const uint32_t *) node.data ();
		for (int i = 0; i < 256; ++i)
		{
			if (items [i] != NODEID_INVALID)
			{
				*pitem_nid = items [i];
				return (uint8_t) i;
			}
		}
		tns_raise_error (KLAV_EINVAL, "index_get_single_item: not items present");
		return 0;
	}
	else if (type == TNT_ARRAY)
	{
		uint32_t cnt = TNS_NODE_SIZE (node.hdr ()) / 5;
		if (cnt == 0)
		{
			tns_raise_error (KLAV_EINVAL, "index_get_single_item: not items present");
			return 0;
		}

		const uint8_t * data = node.data ();

		nodeid_t * items = (uint32_t *) (data + TNS_ALIGN4 (cnt));

		*pitem_nid = items [0];
		return data [0];
	}

	tns_raise_error (KLAV_EINVAL, "index_get_single_item: invalid node type");
	return 0;
}


