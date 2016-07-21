// tree_internal.h
//

#ifndef tree_internal_h_INCLUDED
#define tree_internal_h_INCLUDED

#include "treedefs.h"

#undef NODE
#undef sRT_CTX

#ifdef _TREE_DEBUG
#define DBG_MAX_SIGNATURE_LEN 0x200
#endif

#define _Read(pIOCtx, pBuffer, dwSize) pIOCtx->Read(pIOCtx->pIO, NULL, (uint8_t*)(pBuffer), dwSize)
#define _Write(pIOCtx, pBuffer, dwSize) pIOCtx->Write(pIOCtx->pIO, NULL, (uint8_t*)(pBuffer), dwSize)
#define _SeekRead(pIOCtx, qwOffset, pBuffer, dwSize) pIOCtx->SeekRead(pIOCtx->pIO, NULL, qwOffset, (uint8_t*)(pBuffer), dwSize)
#define _SeekWrite(pIOCtx, qwOffset, pBuffer, dwSize) pIOCtx->SeekWrite(pIOCtx->pIO, NULL, qwOffset, (uint8_t*)(pBuffer), dwSize)
#define _Seek(pIOCtx, pqwNewOffset, qwOffset, dwOrigin) pIOCtx->Seek(pIOCtx->pIO, pqwNewOffset, qwOffset, dwOrigin)

#define _HeapAlloc(pHeapCtx, size) pHeapCtx->Alloc(pHeapCtx->pHeap, size)
#define _HeapReAlloc(pHeapCtx, pMem, size) pHeapCtx->Realloc(pHeapCtx->pHeap, pMem, size)
#define _HeapFree(pHeapCtx, pMem) pHeapCtx->Free(pHeapCtx->pHeap, pMem)

#define _HeapAlloc2(pHeapCtx, ppMem, size) (*ppMem = pHeapCtx->Alloc(pHeapCtx->pHeap, size), (*ppMem==NULL ? TREE_ENOMEM : TREE_OK))
#define _HeapReAlloc2(pHeapCtx, ppMem, pMem, size) (*ppMem = pHeapCtx->Realloc(pHeapCtx->pHeap, pMem, size), (*ppMem==NULL ? TREE_ENOMEM : TREE_OK))

#define XOR_DATA_MASK		0xA5

#define MAGIC_TREE_SST_V1	'1TSS' // obsolete
#define MAGIC_TREE_SST_V2	'2TSS' // obsolete
#define MAGIC_TREE_DB_V2	'2BDT'

typedef struct tag_Names {
	uint32_t id;
	const char* name;
} Names;

enum NodeDataTypes
{
	NDT_Empty         = 0x00,
	
	NDT_P_Index32     = 0x01,
	NDT_P_List        = 0x02,
	NDT_P_Joined      = 0x03,
	NDT_P_JoinedRT    = 0x04,
	NDT_P_Hashed      = 0x05,
	NDT_P_HashedRT    = 0x06,
	
	// new types
	NDT_O_Index32     = 0x07,
	NDT_O_Index32Fast = 0x08,
	NDT_O_List        = 0x09, 
	NDT_O_Joined32    = 0x0a,
	NDT_O_JoinedRT32  = 0x0b, 
	NDT_O_Joined8     = 0x0c,
	NDT_O_JoinedRT8   = 0x0d,
	NDT_O_Hashed      = 0x0e,
	NDT_O_HashedRT    = 0x0f,
	NDT_O_AnyByte     = 0x10,
	NDT_O_AnyByteRT   = 0x11,

	NDT_O_Array       = 0x12,
	NDT_O_ArrayFast   = 0x13,
	NDT_O_Index8      = 0x14,
	NDT_O_Tail        = 0x15,

	NDT_Aster         = 0x16,
	NDT_Reserved8     = 0x17,
	NDT_Reserved7     = 0x18,
	NDT_Reserved6     = 0x19,
	NDT_Reserved5     = 0x1A,
	NDT_Reserved4     = 0x1B,
	NDT_Reserved3     = 0x1C,
	NDT_Reserved2     = 0x1D,
	NDT_Reserved1     = 0x1E,

	NDT_InvalidType   = 0x1F,
};

enum NodeTypes
{
	NT_Dynamic = 0, // dynamic node
	NT_S    = 1,	// node contains only SignatureID
	NT_D    = 2,	// node contains only NodeData
	NT_SD   = 3,	// node contains only SignatureID and NodeData
	NT_C    = 4,	// node contains only Condition
	NT_Full = 7,	// node contains SignatureID, NodeData and Condition
};

/* #define DW_NDT_MASK ((1 << 5) - 1) // 00011111 */

/* #define DW_NT_MASK  (7       << 5) // 11100000 */
/* #define DW_NT_Full	(NT_Full << 5) // 11100000 */
/* #define DW_NT_S		(NT_S    << 5) // 00100000 */
/* #define DW_NT_D		(NT_D    << 5) // 01000000 */
/* #define DW_NT_SD	(NT_SD   << 5) // 01100000 */
/* #define DW_NT_C		(NT_C    << 5) // 10000000 */

#if defined(KL_LITTLE_ENDIAN)
#define MAKE_NODE_TYPE_STRUCT(Name,FieldType,FieldName) \
struct{ \
  uint32_t NodeDataType  : 5; \
  uint32_t NodeType : 3; \
  FieldType FieldName : 8*3; \
} Name;
#elif defined(KL_BIG_ENDIAN)
#define MAKE_NODE_TYPE_STRUCT(Name,FieldType,FieldName) \
struct{ \
  FieldType FieldName : 8*3; \
  uint32_t NodeType : 3; \
  uint32_t NodeDataType  : 5; \
} Name;
#else
#error "undefined byte order"
#endif

typedef struct tag_NODE
{
	union
	{
		uint32_t dwFullNodeType;
		MAKE_NODE_TYPE_STRUCT(NodeTypeFields,uint32_t,NodeData)
	} NodeType;
} NODE, *PNODE;

#define PNODE(P) ((NODE *)(P))
#define NODE_TYPE(P)      (PNODE(P)->NodeType.NodeTypeFields.NodeType)
#define NODE_DATA_TYPE(P) (PNODE(P)->NodeType.NodeTypeFields.NodeDataType)

/* static node */
typedef struct tag_SNODE
{
	struct tag_NODE N;
	uint32_t        dwNodeDataOffset;
	tSIGNATURE_ID   SignatureID;
	uint32_t        dwConditionOffset;
} SNODE;

#define PSNODE(P) ((SNODE *)(P))

/* dynamic node (RT nodes are also dynamic) */
typedef struct tag_DNODE
{
	struct tag_NODE N;
	tSIGNATURE_ID   SignatureID;
	union {
		uint8_t * pNodeData;
		struct tag_NODEDATA_P_LIST * pNodeDataList;
		struct tag_NODEDATA_P_INDEX32 * pNodeDataIndex;
		struct tag_NODEDATA_P_JOINED *  pNodeDataJoined;
		struct tag_NODEDATA_P_HASHED *  pNodeDataHashed;
		struct tag_RT_NODE_DATA *       pNodeDataRT;
		struct tag_NODEDATA_P_JOINED_RT  * pNodeDataJoinedRT;
		struct tag_NODEDATA_P_HASHED_RT  * pNodeDataHashedRT;
		struct tag_NODEDATA_O_ANYBYTE_RT * pNodeDataAnybyteRT;
		struct tag_NODEDATA_ASTER_RT     * pNodeDataAsterRT;
	} NodeData;
	struct tag_DCONDITION* pCondition;
} DNODE;

#define PDNODE(P) ((DNODE *)(P))

/* compact forms of static nodes */
typedef struct tag_SNODE_S
{
	MAKE_NODE_TYPE_STRUCT(NodeType,tSIGNATURE_ID,SignatureID)
} SNODE_S;

typedef struct tag_SNODE_D
{
	MAKE_NODE_TYPE_STRUCT(NodeType,uint32_t,NodeDataOffset)
} SNODE_D;

typedef struct tag_SNODE_C
{
	MAKE_NODE_TYPE_STRUCT(NodeType,uint32_t,ConditionOffset)
} SNODE_C;

typedef struct tag_SNODE_SD
{
	MAKE_NODE_TYPE_STRUCT(NodeType,tSIGNATURE_ID,NodeDataOffset)
	uint32_t SignatureID;
} SNODE_SD;

typedef struct tag_CONDITION_DATA
{
    uint8_t Byte1;
    uint8_t Byte2;
    uint8_t Unused [2]; /* hmm */
} CONDITION_DATA;

typedef struct tag_CONDITION_TYPE
{
  uint8_t nType;
  uint8_t nCounter [2];
  uint8_t nReserved;
} CONDITION_TYPE;

typedef struct tag_CONDITION
{
    CONDITION_TYPE   ConditionType;
    struct tag_CONDITION_DATA Data;
} CONDITION;

typedef struct tag_SCONDITION
{
	struct tag_CONDITION C;
	uint32_t dwNextConditionOffset; /* hmm.. */
    struct tag_SNODE Node;
} SCONDITION;

#define PSCOND(P) ((SCONDITION *)(P))

typedef struct tag_DCONDITION
{
	struct tag_CONDITION C;
    struct tag_DNODE Node;
    struct tag_DCONDITION* pNextCondition;
} DCONDITION;

#define PDCOND(P) ((DCONDITION *)(P))

typedef struct tag_NODEDATA_P_INDEX32
{
    DNODE* Index[0x100];
} NODEDATA_P_INDEX32;

typedef struct tag_NODEDATA_P_LIST
{
    uint8_t NodeByte;
    struct tag_NODEDATA_P_LIST* pNextChild;
    struct tag_DNODE Node;
} NODEDATA_P_LIST;

#if defined (_MSC_VER)
#pragma warning ( disable : 4200 )
#endif
typedef struct tag_NODEDATA_P_JOINED
{
    struct tag_DNODE Node;
    uint32_t dwNumOfBytes;
    uint8_t NodeBytes[0];
} NODEDATA_P_JOINED;
#if defined (_MSC_VER)
#pragma warning ( default : 4200 )
#endif

typedef struct tag_NODEDATA_P_HASHED
{
	uint32_t dwNumOfBytes;
	uint32_t dwHashVal;
	struct tag_DNODE Node;
} NODEDATA_P_HASHED;

typedef struct tag_NODEDATA_O_HASHED
{
	uint32_t dwNumOfBytes;
	uint32_t dwHashVal;
} NODEDATA_O_HASHED;

typedef struct tag_SPINNER
{
	NODE*	pNode;
	uint64_t  qwSignatureStartPosLinear;
} SPINNER, *PSPINNER;

typedef struct tag_SPINNERS_DATA 
{
	uint32_t nAllocated;
	uint32_t nInUse;
	SPINNER*	Spinner;
} SPINNERS_DATA;

typedef struct tag_NODEDATA_P_JOINED_RT
{
	uint32_t  dwCurrentByte;
	union {
		DNODE   * Node;  /* for dynamic nodes: ptr to node */
		uint8_t * Data;  /* for static nodes: ptr to data */
	} pParent;
} NODEDATA_P_JOINED_RT;

typedef struct tag_NODEDATA_P_HASHED_RT
{
	uint32_t  dwCurrentByte;
	uint32_t  dwCurrentHashVal;
	union {
		DNODE   * Node;  /* for dynamic nodes: ptr to node */
		uint8_t * Data;  /* for static nodes: ptr to data */
	} pParent;
} NODEDATA_P_HASHED_RT;

typedef struct tag_NODEDATA_O_ANYBYTE_RT
{
	uint32_t dwCurrentByte;
	uint32_t dwNumOfBytes;
	NODE *   pNextNode;
} NODEDATA_O_ANYBYTE_RT;

typedef struct tag_NODEDATA_ASTER_RT
{
	uint32_t dwCurrentByte;
	uint32_t dwNumOfBytes;
	NODE *   pNextNode;
} NODEDATA_ASTER_RT;

typedef struct tag_RT_NODE_DATA
{
	union {
		NODEDATA_P_JOINED_RT  NDJoined;
		NODEDATA_P_HASHED_RT  NDHashed;
		NODEDATA_O_ANYBYTE_RT NDAnyByte;
	} NodeData;
} RT_NODE_DATA;

typedef struct tag_RT_NODES_POOL
{
	DNODE Node;
	RT_NODE_DATA RTNodeData;
} RT_NODES_POOL;

typedef NODE* (*tTreeGetNextNodeXXX)(NODE* pNode, uint8_t NodeByte, struct tag_sRT_CTX* pRTCtx);

typedef struct tag_sRT_CTX
{
	// database info
	TREE_INFO sTreeInfo; // current database header
	bool_t    bDataMapped;
	uint8_t*  pTreeBase;    // pointer to database mapped into memory
	NODE*	  pRoot;        // root node
	uint8_t*  pTreeHashTable;    // size = pTreeInfo->dwCachTableSize;
	uint16_t  wHashTableANDMask; // = (pTreeInfo->dwCachTableSize - 1)

	// seacher data
	SPINNERS_DATA sSpinners;
	SPINNERS_DATA sNewSpinners;
	uint64_t  qwBufferStartPositionLinear; 
	uint32_t  dwSearcherCurrentPositionInBuffer;
	uint8_t   bBitMask[8];   // used to replace shift (1 << n)
	SPINNER*  pCurrentSpinner;

	// run-time data
	sHEAP_CTX* pHeapCtx;
	RT_NODES_POOL* pRTNodesPool;
	
	tTreeGetNextNodeXXX TreeGetNextNodeXXX[NDT_InvalidType+1];

	// some statistic counters
#ifdef _TREE_DEBUG
	uint32_t  dwStatSpinnersUsed;
	uint32_t  dwStatBytesTotal;
	uint32_t  SpinnerLevel[DBG_MAX_SIGNATURE_LEN];
	uint32_t  DeathLevelStat[DBG_MAX_SIGNATURE_LEN];
	uint32_t  FilterMap[0x10000];
#endif
} sRT_CTX;

// ------------------------------------------------------------------------------------------

EXTERN_C void TreeInitNode(DNODE* pNode);
EXTERN_C void TreeCopyNode(DNODE* pNodeDst, DNODE* pNodeSrc);

EXTERN_C TREE_ERR TreeAddNode(sRT_CTX* pRTCtx, DNODE** pNode, uint8_t NodeByte, DNODE** pNewNode);
EXTERN_C TREE_ERR TreeAddNodeList(sRT_CTX* pRTCtx, DNODE** pNode, uint8_t NodeByte, DNODE** ppNewNode);
EXTERN_C TREE_ERR TreeAddNodeIndexed(sRT_CTX* pRTCtx, DNODE** pNode, uint8_t NodeByte, DNODE** ppNewNode);
EXTERN_C TREE_ERR TreeAddNodeJoined(sRT_CTX* pRTCtx, DNODE** pNode, uint8_t NodeByte, DNODE** ppNewNode);
EXTERN_C TREE_ERR TreeAddNodeJoinedRT(sRT_CTX* pRTCtx, DNODE** pNode, uint8_t NodeByte, DNODE** ppNewNode);
EXTERN_C TREE_ERR TreeAddNodeHashed(sRT_CTX* pRTCtx, DNODE** pNode, uint8_t NodeByte, DNODE** ppNewNode);
EXTERN_C TREE_ERR TreeAddNodeHashedRT(sRT_CTX* pRTCtx, DNODE** pNode, uint8_t NodeByte, DNODE** ppNewNode);

EXTERN_C _FORCEINLINE NODE* TreeGetNextNode(NODE* pNode, uint8_t NodeByte, sRT_CTX* pRTCtx);
EXTERN_C NODE* TreeGetNextNodeUnsupported(NODE* pNode, uint8_t NodeByte, sRT_CTX* pRTCtx);
EXTERN_C NODE* TreeGetNextNodeList(NODE* pNode, uint8_t NodeByte, sRT_CTX* pRTCtx);
EXTERN_C NODE* TreeGetNextNodeIndexed(NODE* pNode, uint8_t NodeByte, sRT_CTX* pRTCtx);
EXTERN_C NODE* TreeGetNextNodeJoined(NODE* pNode, uint8_t NodeByte, sRT_CTX* pRTCtx);
EXTERN_C NODE* TreeGetNextNodeJoinedRT(NODE* pNode, uint8_t NodeByte, sRT_CTX* pRTCtx);
EXTERN_C NODE* TreeGetNextNodeHashed(NODE* pNode, uint8_t NodeByte, sRT_CTX* pRTCtx);
EXTERN_C NODE* TreeGetNextNodeHashedRT(NODE* pNode, uint8_t NodeByte, sRT_CTX* pRTCtx);
EXTERN_C NODE* TreeGetNextNodeAsterRT(NODE* pNode, uint8_t NodeByte, sRT_CTX* pRTCtx);
EXTERN_C NODE* TreeGetNextNode_O_Index32(NODE* pNode, uint8_t NodeByte, sRT_CTX* pRTCtx);
EXTERN_C NODE* TreeGetNextNode_O_Index32Fast(NODE* pNode, uint8_t NodeByte, sRT_CTX* pRTCtx);
EXTERN_C NODE* TreeGetNextNode_O_List(NODE* pNode, uint8_t NodeByte, sRT_CTX* pRTCtx);
EXTERN_C NODE* TreeGetNextNode_O_Joined32(NODE* pNode, uint8_t NodeByte, sRT_CTX* pRTCtx);
EXTERN_C NODE* TreeGetNextNode_O_JoinedRT32(NODE* pNode, uint8_t NodeByte, sRT_CTX* pRTCtx);
EXTERN_C NODE* TreeGetNextNode_O_Joined8(NODE* pNode, uint8_t NodeByte, sRT_CTX* pRTCtx);
EXTERN_C NODE* TreeGetNextNode_O_JoinedRT8(NODE* pNode, uint8_t NodeByte, sRT_CTX* pRTCtx);
EXTERN_C NODE* TreeGetNextNode_O_Hashed(NODE* pNode, uint8_t NodeByte, sRT_CTX* pRTCtx);
EXTERN_C NODE* TreeGetNextNode_O_HashedRT(NODE* pNode, uint8_t NodeByte, sRT_CTX* pRTCtx);
EXTERN_C NODE* TreeGetNextNode_O_Array(NODE* pNode, uint8_t NodeByte, sRT_CTX* pRTCtx);
EXTERN_C NODE* TreeGetNextNode_O_ArrayFast(NODE* pNode, uint8_t NodeByte, sRT_CTX* pRTCtx);
EXTERN_C NODE* TreeGetNextNode_O_Index8(NODE* pNode, uint8_t NodeByte, sRT_CTX* pRTCtx);
EXTERN_C NODE* TreeGetNextNode_O_Tail(NODE* pNode, uint8_t NodeByte, sRT_CTX* pRTCtx);
EXTERN_C NODE* TreeGetNextNode_O_AnyByteRT(NODE* pNode, uint8_t NodeByte, sRT_CTX* pRTCtx);

/* dynamic tree */
EXTERN_C TREE_ERR TreeGetNextNodeChildList(DNODE* pNode, DNODE** pNextChild);
EXTERN_C TREE_ERR TreeGetNextNodeChildIndexed(DNODE* pNode, DNODE** ppNextChild);
EXTERN_C TREE_ERR TreeGetNextNodeChildJoined(sRT_CTX* pRTCtx, DNODE* pNode, DNODE** ppNewNode);
EXTERN_C TREE_ERR TreeGetNextNodeChildJoinedRT(sRT_CTX* pRTCtx, DNODE* pNode, DNODE** ppNewNode);
EXTERN_C TREE_ERR TreeGetNextNodeChildHashed(sRT_CTX* pRTCtx, DNODE* pNode, DNODE** ppNewNode);
EXTERN_C TREE_ERR TreeGetNextNodeChildHashedRT(sRT_CTX* pRTCtx, DNODE* pNode, DNODE** ppNewNode);
EXTERN_C TREE_ERR TreeGetNextNodeChild (sRT_CTX* pRTCtx, DNODE* pNode, DNODE** ppNextChild);

/* runtime nodes - always dynamic */
EXTERN_C TREE_ERR AllocJoinedRTNode(sRT_CTX* pRTCtx, DNODE** ppTempNode, NODEDATA_P_JOINED_RT** ppNDJoinedRT);
EXTERN_C TREE_ERR FreeJoinedRTNode(sRT_CTX* pRTCtx, DNODE* pJoinedRTNode);
EXTERN_C TREE_ERR AllocHashedRTNode(sRT_CTX* pRTCtx, DNODE** ppHashedNode, NODEDATA_P_HASHED_RT** ppNDHashedRT);
EXTERN_C TREE_ERR FreeHashedRTNode(sRT_CTX* pRTCtx, DNODE* pHashedNode);
EXTERN_C TREE_ERR AllocRTNode(sRT_CTX* pRTCtx, DNODE** ppRTNode, RT_NODE_DATA **ppRTNodeData, enum NodeDataTypes NodeDataType);

/* tree modification - dynamic */
EXTERN_C TREE_ERR TreeConvertNodeList2Indexed (sRT_CTX* pRTCtx, DNODE* pNode);
EXTERN_C TREE_ERR TreeJoinNodes2 (sRT_CTX* pRTCtx, DNODE* pNode, uint32_t* pdwNodesJoined);
EXTERN_C TREE_ERR TreeSplitNode (sRT_CTX* pRTCtx, DNODE* pNode, DNODE** ppNewNode);
EXTERN_C TREE_ERR TreeSplitJoinedNode (sRT_CTX* pRTCtx, DNODE* pNode, uint32_t nPos, DNODE** ppNewNode);
EXTERN_C uint32_t TreeGetNodeChildrenCount (DNODE* pNode);

EXTERN_C void   ResetRTContext (sRT_CTX* pRTCtx, sHEAP_CTX* pNewHeapCtx);
EXTERN_C TREE_ERR FreeRTNode (sRT_CTX* pRTCtx, DNODE* pNode);

EXTERN_C _FORCEINLINE uint8_t * TreeGetNodeData(NODE* pNode);
EXTERN_C _FORCEINLINE tSIGNATURE_ID TreeGetNodeSignatureID(NODE* pNode);
EXTERN_C _FORCEINLINE CONDITION* TreeGetNodeCondition(NODE* pNode);
EXTERN_C _FORCEINLINE uint32_t TreeGetNodeSize(NODE* pNode);

EXTERN_C _INLINE NODE* TreeGetConditionNode (NODE* pNode, CONDITION* pCond);
EXTERN_C _INLINE CONDITION* TreeGetNextNodeCondition (NODE* pNode, CONDITION* pCond);

EXTERN_C TREE_ERR TreeCreateRootNode(sRT_CTX* pRTCtx);
EXTERN_C TREE_ERR TreeDeleteRootNode(sRT_CTX* pRTCtx);

EXTERN_C TREE_ERR AddSpinner(sRT_CTX* pRTCtx, SPINNERS_DATA* pSpinners, NODE* pNode, uint64_t qwSignatureStartPosLinear);

// ------------------------------------------------------------------------------------------

#endif // tree_internal_h_INCLUDED

