// tns_defs.h
//
//

#ifndef tns_defs_h_INCLUDED
#define tns_defs_h_INCLUDED

#include <klava/klavdef.h>

////////////////////////////////////////////////////////////////

#ifdef _DEBUG
# define TNS_ENABLE_TRACE
#endif

//#define TNS_NODE_TRACE_ON
//#define TNS_BLK_TRACE_ON

#ifdef TNS_ENABLE_TRACE
# define TNS_TRACE(VARS) tns_trace VARS
#else // TNS_ENABLE_TRACE
# define TNS_TRACE(VARS)
#endif // TNS_ENABLE_TRACE

#ifdef TNS_NODE_TRACE_ON
# include <stdio.h>
# define TNS_NODE_TRACE(VARS) printf VARS
#else // TNS_NODE_TRACE_ON
# define TNS_NODE_TRACE(VARS)
#endif // TNS_NODE_TRACE_ON

#ifdef TNS_BLK_TRACE_ON
# include <stdio.h>
# define TNS_BLK_TRACE(VARS) printf VARS
#else // TNS_BLK_TRACE_ON
# define TNS_BLK_TRACE(VARS)
#endif // TNS_BLK_TRACE_ON

////////////////////////////////////////////////////////////////

// node types
enum
{
	TNT_INVALID   = 0x00,
	TNT_INDEX     = 0x01,
	TNT_ARRAY     = 0x02,
	TNT_JOINED    = 0x03,
	TNT_HASHED    = 0x04,
	TNT_CONDITION = 0x05,
	TNT_TAIL      = 0x06,
};

// condition types
enum
{
	TNS_COND_MASK  = 0x00,  // &-mask
	TNS_COND_RANGE = 0x01,  // byte range
	TNS_COND_ANY   = 0x02,  // fixed number of anybytes (16-bit counter)
	TNS_COND_STAR  = 0x03,  // variable number of anybytes (0..N) (16-bit counter)
	TNS_COND_LAST = TNS_COND_STAR
};

////////////////////////////////////////////////////////////////

#define TNS_ALIGN4(A) (((A) + 3) & ~3)

////////////////////////////////////////////////////////////////

#define TNS_BLK_MAGIC_0 'T'
#define TNS_BLK_MAGIC_1 'N'
#define TNS_BLK_MAGIC_2 'b'

struct TNS_NODE
{
	uint32_t tns_hdr;
	uint32_t tns_data;
	uint32_t tns_evid;
	uint32_t tns_chain;
};

struct TNS_BLK_HDR
{
	uint8_t  tdb_magic [3];
	uint8_t  tdb_unit_size;     // ASCII unit size index ('0':node, '1':8, '2':16, etc)
	uint32_t tdb_used;          // in items
	uint32_t tdb_reserved1;     // in items
	uint32_t tdb_reserved2;     // next block in chain
};

#define TNS_UNIT_SIZE_MIN 8
#define TNS_UNIT_SIZE_MAX 1024
// sizeof(TNS_NODE),8,16,32,64,128,256,512,1024
#define TNS_UNIT_SIZE_CNT 9

// block size: 4K
#define TNS_BLK_SIZE 0x1000

// tables
extern uint32_t TNS_BLK_UNIT_SIZE [];
extern uint32_t TNS_BLK_DATA_START [];
extern uint32_t TNS_BLK_UNIT_COUNT [];

// node bitmap (in each block)
#define TNS_BLK_BITMAP_START sizeof(TNS_BLK_HDR)

#define TNS_BLK_BITMAP_BYTE(BLK,IDX) ((uint8_t *)(BLK))[TNS_BLK_BITMAP_START + ((IDX) >> 3)]
#define TNS_BLK_GET_BIT(BLK,IDX)  (TNS_BLK_BITMAP_BYTE(BLK,IDX) & (1 << ((IDX) & 0x7)))
#define TNS_BLK_SET_BIT(BLK,IDX)  (TNS_BLK_BITMAP_BYTE(BLK,IDX) |= (1 << ((IDX) & 0x7)))
#define TNS_BLK_CLR_BIT(BLK,IDX)  (TNS_BLK_BITMAP_BYTE(BLK,IDX) &= (uint8_t) ~(1 << ((IDX) & 0x7)))

// returns -1 if could not allocate index (bitmap is full)
int tns_blk_find_free_item (const TNS_BLK_HDR *hdr);

typedef uint32_t nodeid_t;

#define NODEID_INVALID 0

#define EVID_DELETED 0xFFFFFFFFU

// node kind (encoded into nodeid)
#define TNS_NODEKIND_NORM 0x00
#define TNS_NODEKIND_PRIV 0x01
#define TNS_NODEKIND_TEMP 0x02

#define MAKE_NODEID(BLK,IDX,KIND) (((((BLK) << 2) | (KIND)) << 14) | (IDX))
#define NODEID_BLK(NODEID)  ((NODEID) >> 16)
#define NODEID_IDX(NODEID)  ((NODEID) & 0x3FFF)
#define NODEID_KIND(NODEID) (((NODEID) >> 14) & 0x03)

#define NODEID_IDX_MAX ((TNS_BLOCK_SIZE - sizeof (TNS_DATABLK_HDR)) / sizeof (TNS_NODE))

////////////////////////////////////////////////////////////////

// flag (for validation purpose): node is used
#define TNS_NODE_F_USED  0x80U

#define TNS_NODE_HDR(TYPE,DATASIZE)       ((((uint32_t)DATASIZE) << 8) | TYPE)

#define TNS_NODE_TYPE(HDR) ((HDR) & 0x3F)
#define TNS_NODE_SIZE(HDR) ((HDR) >> 8)
#define TNS_NODE_FLAG(HDR,FLAG) ((HDR) & (TNS_NODE_F_##FLAG))

#define TNS_NODE_SET_TYPE(HDR,T8)  (((HDR) & 0xFFFFFF00) | (T8))
#define TNS_NODE_SET_SIZE(HDR,D24) (((HDR) & 0xFF) | ((D24) << 8))
#define TNS_NODE_SET_FLAG(HDR,FLAG) ((HDR) |= TNS_NODE_F_##FLAG)
#define TNS_NODE_CLR_FLAG(HDR,FLAG) ((HDR) &= ~ TNS_NODE_F_##FLAG)

////////////////////////////////////////////////////////////////
// sizes & limits

#include <kl_pushpack.h>

// Joined node data
struct TNS_JOIN_DATA
{
	nodeid_t ndj_next;
} KL_PACKED;
// node data follows
// data length is determined by node header field

// Hashed node data
struct TNS_HASH_DATA
{
	nodeid_t ndh_next;
	uint32_t ndh_len;
	uint32_t ndh_hash;
	nodeid_t ndh_data;
} KL_PACKED;

// Condition node data
struct TNS_COND_DATA
{
	nodeid_t ndc_next;
	uint8_t  ndc_type;
	uint8_t  ndc_parm1;
	uint8_t  ndc_parm2;
} KL_PACKED;

#define TNS_COND_GET16(COND) (((uint32_t)((COND)->ndc_parm2) << 8) | ((COND)->ndc_parm1))
#define TNS_COND_SET16(COND,CNT) ((COND)->ndc_parm1 = (uint8_t)(CNT), (COND)->ndc_parm2 = (uint8_t)((CNT) >> 8))

#include <kl_poppack.h>

#define TNS_JOIN_SIZE_MAX  (TNS_UNIT_SIZE_MAX - sizeof (TNS_JOIN_DATA))

#define TNS_HASH_SIZE_MIN 8
#define TNS_HASH_SIZE_MAX TNS_UNIT_SIZE_MAX

// max ARRAY item count (after which it is transformed to INDEX32) = 1020/5
#define TNS_ARRAY_MAX_SIZE 204

////////////////////////////////////////////////////////////////
// Node manager interface

struct KLAV_NO_VTABLE TNS_Tree_Manager
{
	// support methods
	virtual void         KLAV_CALL destroy () = 0;
	virtual KLAV_Alloc * KLAV_CALL get_allocator () = 0;
	// read-only methods
	virtual TNS_NODE *   KLAV_CALL read_node (nodeid_t node, klav_bool_t write) = 0;
	virtual uint8_t *    KLAV_CALL read_node_data (nodeid_t data_ref, klav_bool_t write) = 0;
	// read-write methods
	virtual TNS_NODE *   KLAV_CALL alloc_node (uint32_t flags, nodeid_t *pnewid) = 0;
	virtual void         KLAV_CALL free_node (nodeid_t nid) = 0;
	virtual uint8_t *    KLAV_CALL alloc_node_data (uint32_t flags, size_t size, nodeid_t *pnewid) = 0;
	virtual uint8_t *    KLAV_CALL realloc_node_data (uint32_t flags, nodeid_t oldid, size_t size, nodeid_t *pnewid) = 0;
	virtual void         KLAV_CALL free_node_data (nodeid_t data_ref) = 0;
	virtual void         KLAV_CALL clear_tree () = 0;
	virtual void         KLAV_CALL commit (nodeid_t root_nid, uint32_t hash_level) = 0;
	virtual void         KLAV_CALL rollback () = 0;
};

////////////////////////////////////////////////////////////////


#endif // tns_defs_h_INCLUDED

