// rtnodes.c
//

#include "common.h"

#define TEMP_NODES_CACHE_SIZE          ((uint32_t)(256)) 

// ------------------------------------------------------------------------------------------

TREE_STATIC
TREE_ERR iAllocRTNode(sRT_CTX* pRTCtx, DNODE** ppRTNode, RT_NODE_DATA** ppRTNodeData)
{
	RT_NODES_POOL* pPool = NULL;

	pPool = pRTCtx->pRTNodesPool;
	if (pPool == NULL)
	{
		if (TREE_SUCCEEDED(_HeapAlloc2(pRTCtx->pHeapCtx, (void**)&pPool, sizeof(RT_NODES_POOL)*TEMP_NODES_CACHE_SIZE)))
		{
			pRTCtx->pRTNodesPool = pPool;
			ZeroMemory (pPool, sizeof(RT_NODES_POOL)*TEMP_NODES_CACHE_SIZE);
		}
	}

	if (pPool != NULL)
	{
		uint32_t i;
		for (i=0; i<TEMP_NODES_CACHE_SIZE; i++)
		{
			if (pPool[i].Node.N.NodeType.NodeTypeFields.NodeDataType == NDT_Empty)
			{
				memset(&pPool[i], 0, sizeof(RT_NODES_POOL));
				pPool[i].Node.N.NodeType.NodeTypeFields.NodeDataType = NDT_InvalidType;
				pPool[i].Node.NodeData.pNodeDataRT = &pPool[i].RTNodeData;
				*ppRTNode = &pPool[i].Node;
				if (ppRTNodeData)
					*ppRTNodeData = &pPool[i].RTNodeData;
				return TREE_OK;
			}
		}
	}
	(void)_HeapAlloc2(pRTCtx->pHeapCtx, (void**)ppRTNode, sizeof(RT_NODES_POOL));
	if (*ppRTNode == NULL)
		return TREE_ENOMEM;
	(*ppRTNode)->N.NodeType.NodeTypeFields.NodeType = NT_Dynamic;
	(*ppRTNode)->N.NodeType.NodeTypeFields.NodeDataType = NDT_InvalidType;
	(*ppRTNode)->NodeData.pNodeDataRT = (RT_NODE_DATA*) (*ppRTNode+1);
	if (ppRTNodeData)
		*ppRTNodeData = (RT_NODE_DATA*) (*ppRTNode+1);
	return TREE_OK;
}

TREE_STATIC
TREE_ERR AllocRTNode(sRT_CTX* pRTCtx, DNODE** ppRTNode, RT_NODE_DATA** ppRTNodeData, enum NodeDataTypes NodeDataType)
{
	TREE_ERR error;
	error = iAllocRTNode(pRTCtx, ppRTNode, ppRTNodeData);
	if (TREE_SUCCEEDED(error))
	{
		(*ppRTNode)->N.NodeType.NodeTypeFields.NodeType = NT_Dynamic;
		(*ppRTNode)->N.NodeType.NodeTypeFields.NodeDataType = NodeDataType;
	}
	return error;
}

TREE_STATIC
TREE_ERR FreeRTNode(sRT_CTX* pRTCtx, DNODE* pRTNode)
{
	RT_NODES_POOL* pPool = pRTCtx->pRTNodesPool;
	
	if (pPool != NULL && (uint8_t*)pRTNode >= (uint8_t*)pPool && (uint8_t*)pRTNode < (uint8_t*)pPool + sizeof(RT_NODES_POOL)*TEMP_NODES_CACHE_SIZE) 
	{
		pRTNode->N.NodeType.NodeTypeFields.NodeType = NT_Dynamic;
		pRTNode->N.NodeType.NodeTypeFields.NodeDataType = NDT_Empty;
		return TREE_OK;
	}

	_HeapFree(pRTCtx->pHeapCtx, pRTNode);
	return TREE_OK;
}

TREE_STATIC
TREE_ERR AllocJoinedRTNode(sRT_CTX* pRTCtx, DNODE** ppRTNode, NODEDATA_P_JOINED_RT** ppNDJoinedRT)
{
	return iAllocRTNode(pRTCtx, ppRTNode, (RT_NODE_DATA**)ppNDJoinedRT);
}

TREE_STATIC
TREE_ERR FreeJoinedRTNode(sRT_CTX* pRTCtx, DNODE* pJoinedRTNode)
{
	return FreeRTNode(pRTCtx, pJoinedRTNode);
}

// ------------------------------------------------------------------------------------------

TREE_STATIC
TREE_ERR AllocHashedRTNode(sRT_CTX* pRTCtx, DNODE** ppHashedNode, NODEDATA_P_HASHED_RT** ppNDHashedRT)
{
	return iAllocRTNode(pRTCtx, ppHashedNode, (RT_NODE_DATA**)ppNDHashedRT);
}

TREE_STATIC
TREE_ERR FreeHashedRTNode(sRT_CTX* pRTCtx, DNODE* pHashedNode)
{
	return FreeRTNode(pRTCtx, pHashedNode);
}

