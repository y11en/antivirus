// static.c
//

#include "common.h"
#include "crc32.h"

#ifdef _USE_TAGS
#define CHECK_TAG(pNode, pTag) { if (*(((uint32_t*)pNode)-1) != *(uint32_t*)pTag) DBG_STOP; }
#else
#define CHECK_TAG(pNode, pTag) 
#endif

// ------------------------------------------------------------------------------------------------

TREE_STATIC
NODE* TreeGetNextNode_O_Index32 (NODE* pNode, uint8_t NodeByte, sRT_CTX* pRTCtx)
{
	uint8_t* pNodeData = TreeGetNodeData (pNode);
	uint32_t dwOffset;
	NODE* pNextNode;
	CHECK_TAG(pNodeData, "<I>>");
	dwOffset = *((uint32_t*)pNodeData + NodeByte)  * 4;
	if (dwOffset == 0)
		return NULL;
	pNextNode = (NODE*)(pNodeData + dwOffset + 0x3FC);
	return pNextNode;
}

// ------------------------------------------------------------------------------------------------

TREE_STATIC
NODE* TreeGetNextNode_O_Index32Fast(NODE* pNode, uint8_t NodeByte, sRT_CTX* pRTCtx)
{
	uint8_t* pNodeData = TreeGetNodeData(pNode);
	NODE* pNextNode;
	CHECK_TAG(pNodeData, "<IF>");
	pNextNode = (NODE*) (pNodeData + NodeByte*sizeof(SNODE_D));
	if (*(uint32_t*)pNextNode == 0)
		return NULL;
	return pNextNode;
}

// ------------------------------------------------------------------------------------------------

TREE_STATIC
NODE* TreeGetNextNode_O_Index8(NODE* pNode, uint8_t NodeByte, sRT_CTX* pRTCtx)
{
	uint8_t* pNodeData = TreeGetNodeData(pNode);
	uint32_t dwOffset = pNodeData[NodeByte];
	CHECK_TAG(pNodeData, "<I8>");
	if (dwOffset)
		return (NODE*)(pNodeData + 256 - 4 + dwOffset*4);
	return NULL;
}

// ------------------------------------------------------------------------------------------------

TREE_STATIC
NODE* TreeGetNextNode_O_List(NODE* pNode, uint8_t NodeByte, sRT_CTX* pRTCtx)
{
	// this node type always serialized as O_Array[Fast]
	//uint8_t* pNodeData = (uint8_t*)pNode + TreeGetNodeData(pNode);
	ODSN(("TreeGetNextNode: Unsupported node type"));
	DBG_STOP;
	return NULL;
}

// ------------------------------------------------------------------------------------------------

TREE_STATIC
NODE * TreeGetNextNode_O_Joined32 (NODE* pNode, uint8_t NodeByte, sRT_CTX* pRTCtx)
{
	uint8_t * pNodeData = TreeGetNodeData (pNode);
	DNODE* pJoinedRTNode;
	NODEDATA_P_JOINED_RT* pNDJoinedRT;
	
	/* node data layout: <uint32_t count> <uint8_t bytes [count]> */
	CHECK_TAG(pNodeData, "<J>>");

	if (pNodeData [4] != (NodeByte ^ XOR_DATA_MASK)) // NodeBytes[0]
		return NULL;
	
#ifdef _TREE_DEBUG
	if (*(uint32_t*)pNodeData == 1) // dwNumOfBytes
	{
		DBG_STOP; // ???? Joined node 1-byte size???
		//*ppNewNode = (NODE*)dwBase;
		//return errOK;
		return NULL;
	}
#endif

	if (TREE_FAILED(AllocJoinedRTNode(pRTCtx, &pJoinedRTNode, &pNDJoinedRT)))
	{
		DBG_STOP;
		return NULL;
	}
	
	pJoinedRTNode->N.NodeType.NodeTypeFields.NodeType = NT_Dynamic;
	pJoinedRTNode->N.NodeType.NodeTypeFields.NodeDataType = NDT_O_JoinedRT32;
	pJoinedRTNode->SignatureID = 0;
	pJoinedRTNode->NodeData.pNodeDataJoinedRT = pNDJoinedRT;
	pNDJoinedRT->dwCurrentByte = 0;
	pNDJoinedRT->pParent.Data = pNodeData;
	return PNODE (pJoinedRTNode);
}

// ------------------------------------------------------------------------------------------------

TREE_STATIC
NODE* TreeGetNextNode_O_JoinedRT32 (NODE* pNode, uint8_t NodeByte, sRT_CTX* pRTCtx)
{
	NODEDATA_P_JOINED_RT* pNDJoinedRT = PDNODE(pNode)->NodeData.pNodeDataJoinedRT;
	uint8_t * pNodeData = pNDJoinedRT->pParent.Data;
	uint32_t dwNumOfBytes = * ((uint32_t *) pNodeData);

	pNDJoinedRT->dwCurrentByte++;

	if (pNDJoinedRT->dwCurrentByte >= dwNumOfBytes)
	{
		ODSN(("Oops: (pNDJoined->dwNumOfBytes >= pNDJoinedRT->dwCurrentByte)n"));
		DBG_STOP;
	}

	if (pNodeData [4 + pNDJoinedRT->dwCurrentByte] == (NodeByte ^ XOR_DATA_MASK))
	{
		if (dwNumOfBytes == pNDJoinedRT->dwCurrentByte + 1) // last byte in array
		{
			uint32_t dwAlign = (dwNumOfBytes & 3);
			if (dwAlign)
				dwAlign = 4 - dwAlign;
			FreeJoinedRTNode(pRTCtx, PDNODE (pNode));
			return PNODE(pNodeData + 4 + dwNumOfBytes + dwAlign);
		}
		return pNode;
	}

	FreeJoinedRTNode (pRTCtx, PDNODE (pNode));
	return NULL;
}

// ------------------------------------------------------------------------------------------------

TREE_STATIC
NODE* TreeGetNextNode_O_Joined8(NODE* pNode, uint8_t NodeByte, sRT_CTX* pRTCtx)
{
	uint8_t * pNodeData = TreeGetNodeData(pNode);
	DNODE * pJoinedRTNode;
	NODEDATA_P_JOINED_RT * pNDJoinedRT;
	
	/* node data layout: <uint8_t count> <uint8_t bytes [count]> */
	CHECK_TAG(pNodeData, "<J8>");

	if (pNodeData [1] != (NodeByte ^ XOR_DATA_MASK)) // NodeBytes[0]
		return NULL;

#ifdef _TREE_DEBUG
	if (pNodeData [0] == 1) // bNumOfBytes
	{
		DBG_STOP; // ???? Joined node 1-byte size???
		//*ppNewNode = (NODE*)dwBase;
		//return errOK;
		return NULL;
	}
#endif

	if (TREE_FAILED(AllocJoinedRTNode(pRTCtx, &pJoinedRTNode, &pNDJoinedRT)))
	{
		DBG_STOP;
		return NULL;
	}
	
	pJoinedRTNode->N.NodeType.NodeTypeFields.NodeType = NT_Dynamic;
	pJoinedRTNode->N.NodeType.NodeTypeFields.NodeDataType = NDT_O_JoinedRT8;
	pJoinedRTNode->SignatureID = 0;
	pJoinedRTNode->NodeData.pNodeDataJoinedRT = pNDJoinedRT;
	pNDJoinedRT->dwCurrentByte = 0;
	pNDJoinedRT->pParent.Data = pNodeData;
	return PNODE (pJoinedRTNode);
}

// ------------------------------------------------------------------------------------------------

TREE_STATIC
NODE* TreeGetNextNode_O_JoinedRT8(NODE* pNode, uint8_t NodeByte, sRT_CTX* pRTCtx)
{
	NODEDATA_P_JOINED_RT* pNDJoinedRT = PDNODE (pNode)->NodeData.pNodeDataJoinedRT;
	uint8_t* pNodeData = pNDJoinedRT->pParent.Data;
	uint32_t dwNumOfBytes = pNodeData [0];

	pNDJoinedRT->dwCurrentByte++;

	if (pNDJoinedRT->dwCurrentByte >= dwNumOfBytes)
	{
		ODSN(("Oops: (pNDJoined->dwNumOfBytes >= pNDJoinedRT->dwCurrentByte)n"));
		DBG_STOP;
	}

	if (pNodeData [1 + pNDJoinedRT->dwCurrentByte] == (NodeByte ^ XOR_DATA_MASK))
	{
		if (dwNumOfBytes == pNDJoinedRT->dwCurrentByte + 1) // last byte in array
		{
			uint32_t dwAlign = ((dwNumOfBytes + 1) & 3);
			if (dwAlign)
				dwAlign = 4 - dwAlign;
			FreeJoinedRTNode(pRTCtx, PDNODE (pNode));
			return PNODE(pNodeData + 1 + dwNumOfBytes + dwAlign);
		}
		return pNode;
	}

	FreeJoinedRTNode(pRTCtx, PDNODE (pNode));
	return NULL;
}

// ------------------------------------------------------------------------------------------------

TREE_STATIC
NODE* TreeGetNextNode_O_Hashed (NODE* pNode, uint8_t NodeByte, sRT_CTX* pRTCtx)
{
	NODEDATA_P_HASHED_RT* pNDHashedRT;
	DNODE* pHashedRTNode;
	uint8_t * pNodeData = TreeGetNodeData(pNode);

	/* node data layout: <uint32_t count> <uint32_t hash> */
	CHECK_TAG(pNodeData, "<H>>");
	
	if (TREE_FAILED(AllocHashedRTNode(pRTCtx, &pHashedRTNode, &pNDHashedRT)))
	{
		DBG_STOP;
		return NULL;
	}

	pHashedRTNode->N.NodeType.NodeTypeFields.NodeType = NT_Dynamic;
	pHashedRTNode->N.NodeType.NodeTypeFields.NodeDataType = NDT_O_HashedRT;
	pHashedRTNode->SignatureID = 0;
	pHashedRTNode->NodeData.pNodeDataHashedRT = pNDHashedRT;
	pNDHashedRT->dwCurrentByte = 0;
	pNDHashedRT->pParent.Data = pNodeData;

	return TreeGetNextNode_O_HashedRT (PNODE (pHashedRTNode), NodeByte, pRTCtx);
}

// ------------------------------------------------------------------------------------------------

TREE_STATIC
NODE* TreeGetNextNode_O_HashedRT(NODE* pNode, uint8_t NodeByte, sRT_CTX* pRTCtx)
{
	NODEDATA_P_HASHED_RT* pNDHashedRT = PDNODE (pNode)->NodeData.pNodeDataHashedRT;
	NODEDATA_O_HASHED *pNDHashed = (NODEDATA_O_HASHED *) pNDHashedRT->pParent.Data;
	uint32_t hash;

	if (pNDHashedRT->dwCurrentByte >= pNDHashed->dwNumOfBytes)
	{
		ODSN(("Oops: (pNDHashed->dwNumOfBytes >= pNDHashedRT->dwCurrentByte)"));
		DBG_STOP;
	}

	hash = ~(pNDHashedRT->dwCurrentHashVal);
	hash = UPD_CRC32 (NodeByte, hash);
	pNDHashedRT->dwCurrentHashVal = ~hash;

	pNDHashedRT->dwCurrentByte += sizeof(NodeByte);
		
	if (pNDHashedRT->dwCurrentByte == pNDHashed->dwNumOfBytes)
	{
		if (pNDHashedRT->dwCurrentHashVal == pNDHashed->dwHashVal)
		{
			FreeHashedRTNode (pRTCtx, PDNODE (pNode));
			return PNODE (pNDHashed + 1);
		}
		FreeHashedRTNode(pRTCtx, PDNODE(pNode));
		return NULL;
	}

	return pNode;
}

// ------------------------------------------------------------------------------------------------

TREE_STATIC
NODE* TreeGetNextNode_O_AnyByteRT(NODE* pNode, uint8_t NodeByte, sRT_CTX* pRTCtx)
{
	NODEDATA_O_ANYBYTE_RT * pNDAnyByteRT = PDNODE (pNode)->NodeData.pNodeDataAnybyteRT;
	
	if (pNDAnyByteRT->dwCurrentByte >= pNDAnyByteRT->dwNumOfBytes)
	{
		ODSN(("Oops: (pNDHashed->dwNumOfBytes >= pNDHashedRT->dwCurrentByte)"));
		DBG_STOP;
	}

	pNDAnyByteRT->dwCurrentByte++;
		
	if (pNDAnyByteRT->dwCurrentByte == pNDAnyByteRT->dwNumOfBytes)
	{
		NODE* pNextNode = pNDAnyByteRT->pNextNode;
		FreeRTNode (pRTCtx, PDNODE(pNode));
		return pNextNode;
	}

	return pNode;
}

// ------------------------------------------------------------------------------------------------

TREE_STATIC
NODE* TreeGetNextNodeAsterRT(NODE* pNode, uint8_t NodeByte, sRT_CTX* pRTCtx)
{
	NODEDATA_ASTER_RT* pNDAsterRT = PDNODE (pNode)->NodeData.pNodeDataAsterRT;
	NODE* pNextNode = pNDAsterRT->pNextNode;

	if (pNDAsterRT->dwCurrentByte >= pNDAsterRT->dwNumOfBytes)
	{
		ODSN(("Oops: (pNDHashed->dwNumOfBytes >= pNDHashedRT->dwCurrentByte)"));
		DBG_STOP;
	}

	pNDAsterRT->dwCurrentByte++;	
	if (pNDAsterRT->dwCurrentByte == pNDAsterRT->dwNumOfBytes)
		FreeRTNode(pRTCtx, PDNODE (pNode));
	else
		AddSpinner(pRTCtx, &pRTCtx->sNewSpinners, pNode, pRTCtx->pCurrentSpinner->qwSignatureStartPosLinear);
	return pNextNode;
}

// ------------------------------------------------------------------------------------------------

TREE_STATIC
NODE* TreeGetNextNode_O_Array(NODE* pNode, uint8_t NodeByte, sRT_CTX* pRTCtx)
{
	uint8_t* pNodeData = TreeGetNodeData(pNode);
	uint32_t dwNumOfBytes = pNodeData[0];
	uint32_t i;

	CHECK_TAG(pNodeData, "<A>>");
	
	for (i=0; i<dwNumOfBytes; i++)
	{
		if (pNodeData[1+i] == NodeByte)
		{
			uint32_t dwOffset = pNodeData[1+dwNumOfBytes+i];
			return (NODE*) (pNodeData + dwOffset*4);
		}
		if (pNodeData[1+i] > NodeByte)
			break;
	}
	return NULL;
}

// ------------------------------------------------------------------------------------------------

TREE_STATIC
NODE* TreeGetNextNode_O_ArrayFast(NODE* pNode, uint8_t NodeByte, sRT_CTX* pRTCtx)
{
	uint8_t* pNodeData = TreeGetNodeData(pNode);
	uint32_t dwNumOfBytes = pNodeData[0];
	uint32_t i;

	CHECK_TAG(pNodeData, "<AF>");
	
	for (i=0; i<dwNumOfBytes; i++)
	{
		if (pNodeData[1+i] == NodeByte)
		{
			uint32_t dwAlign = ((1+dwNumOfBytes) & 3);
			if (dwAlign)
				dwAlign = 4 - dwAlign;
			return (NODE*) (pNodeData + (1+dwNumOfBytes) + dwAlign + i*sizeof(SNODE_D));
		}
		if (pNodeData[1+i] > NodeByte)
			break;
	}
	return NULL;
}

// ------------------------------------------------------------------------------------------------

TREE_STATIC
NODE* TreeGetNextNode_O_Tail(NODE* pNode, uint8_t NodeByte, sRT_CTX* pRTCtx)
{
	return NULL;
}

// ------------------------------------------------------------------------------------------------

