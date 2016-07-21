// node_rdr.cpp
//
//

#include "tns_mgr.h"

////////////////////////////////////////////////////////////////

Tree_Node_Reader::Tree_Node_Reader (KLAV_Alloc *alloc)
	: m_allocator (alloc), m_data (0), m_size (0)
{
}

Tree_Node_Reader::Tree_Node_Reader (KLAV_Alloc *alloc, const void *data, size_t size)
	: m_allocator (alloc), m_data ((const uint8_t *) data), m_size (size)
{
}

Tree_Node_Reader::~Tree_Node_Reader ()
{
}

void Tree_Node_Reader::open (const void *data, size_t size)
{
	m_data = (const uint8_t *) data;
	m_size = size;
}

void Tree_Node_Reader::close ()
{
	m_data = 0;
	m_size = 0;
}

void  KLAV_CALL Tree_Node_Reader::destroy ()
{
	KLAV_DELETE (this, allocator ());
}

KLAV_Alloc * KLAV_CALL Tree_Node_Reader::get_allocator ()
{
	return allocator ();
}

TNS_NODE * KLAV_CALL Tree_Node_Reader::read_node (nodeid_t node, klav_bool_t write)
{
	return (TNS_NODE *) read_node_data (node, write);
}

TNS_NODE * KLAV_CALL Tree_Node_Reader::alloc_node (uint32_t flags, nodeid_t *pnewid)
{
	*pnewid = NODEID_INVALID;
	return 0;
}

void KLAV_CALL Tree_Node_Reader::free_node (nodeid_t nid)
{
}

uint8_t * KLAV_CALL Tree_Node_Reader::read_node_data (nodeid_t data_ref, klav_bool_t write)
{
	if (write)
		return 0;

	uint32_t nodeblk = NODEID_BLK(data_ref) - 1;
	uint32_t nodeidx = NODEID_IDX(data_ref);
	uint32_t nodeKind = NODEID_KIND(data_ref);

	if (nodeKind != TNS_NODEKIND_NORM)
		return 0;

	uint32_t blkoffset = nodeblk * TNS_BLK_SIZE;

	if (m_size < blkoffset + TNS_BLK_SIZE) // must be whole of a block
		return 0;

	const uint8_t * blkstart = m_data + blkoffset;

	TNS_BLK_HDR const* hdr = reinterpret_cast<TNS_BLK_HDR const*>(blkstart);

	uint32_t unit_size_idx = hdr->tdb_unit_size - '0';
	uint32_t unit_size = TNS_BLK_UNIT_SIZE [unit_size_idx];

	uint32_t offset = TNS_BLK_DATA_START[unit_size_idx] + (unit_size * nodeidx);
	if (offset >= TNS_BLK_SIZE)
		return 0;

	return (uint8_t *) (blkstart + offset);
}

uint8_t * KLAV_CALL Tree_Node_Reader::alloc_node_data (uint32_t flags, size_t size, nodeid_t *pnewid)
{
	*pnewid = NODEID_INVALID;
	return 0;
}

uint8_t * KLAV_CALL Tree_Node_Reader::realloc_node_data (uint32_t flags, nodeid_t oldid, size_t size, nodeid_t *pnewid)
{
	*pnewid = NODEID_INVALID;
	return 0;
}

void KLAV_CALL Tree_Node_Reader::free_node_data (nodeid_t data_ref)
{
}

void KLAV_CALL Tree_Node_Reader::clear_tree ()
{
}

void KLAV_CALL Tree_Node_Reader::commit (nodeid_t root_nid, uint32_t hash_level)
{
}

void KLAV_CALL Tree_Node_Reader::rollback ()
{
}


