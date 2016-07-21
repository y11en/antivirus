// dynamic.c
//

#include "common.h"
#include "crc32.h"

#ifndef TREE_STATIC_ONLY

// ------------------------------------------------------------------------------------------

_INLINE void TreeInitNode (DNODE* pNode)
{
	ZeroMemory(pNode, sizeof(DNODE));
	pNode->N.NodeType.NodeTypeFields.NodeType = NT_Dynamic;
	pNode->N.NodeType.NodeTypeFields.NodeDataType = NDT_P_List;
}

// ------------------------------------------------------------------------------------------------

TREE_ERR TreeCreateRootNode(sRT_CTX* pRTCtx)
{
	NODE* pRoot;

	pRoot = _HeapAlloc(pRTCtx->pHeapCtx, sizeof(DNODE));
	if (pRoot == NULL)
		return TREE_ENOMEM;
	TreeInitNode (PDNODE(pRoot));
	pRTCtx->pRoot = pRoot;
	ZeroMemory(&pRTCtx->sTreeInfo, sizeof(pRTCtx->sTreeInfo));
	pRTCtx->sTreeInfo.dwMagic = MAGIC_TREE_DB_V2;
	pRTCtx->sTreeInfo.dwFormat = cFORMAT_DYNAMIC;
	
	// init TreeGetNextNodeXXX array
	pRTCtx->TreeGetNextNodeXXX[NDT_P_Index32]  = (tTreeGetNextNodeXXX)TreeGetNextNodeIndexed;
	pRTCtx->TreeGetNextNodeXXX[NDT_P_List]     = (tTreeGetNextNodeXXX)TreeGetNextNodeList;
	pRTCtx->TreeGetNextNodeXXX[NDT_P_Joined]   = (tTreeGetNextNodeXXX)TreeGetNextNodeJoined;
	pRTCtx->TreeGetNextNodeXXX[NDT_P_JoinedRT] = (tTreeGetNextNodeXXX)TreeGetNextNodeJoinedRT;
	pRTCtx->TreeGetNextNodeXXX[NDT_P_Hashed]   = (tTreeGetNextNodeXXX)TreeGetNextNodeHashed;
	pRTCtx->TreeGetNextNodeXXX[NDT_P_HashedRT] = (tTreeGetNextNodeXXX)TreeGetNextNodeHashedRT;
	pRTCtx->TreeGetNextNodeXXX[NDT_Aster]      = (tTreeGetNextNodeXXX)TreeGetNextNodeAsterRT;

	return TREE_OK;
}

// ------------------------------------------------------------------------------------------------

TREE_ERR TreeDeleteNode (sRT_CTX* pRTCtx, DNODE** ppNode)
{
	TREE_ERR error = TREE_OK;
	DNODE* pChild = NULL;
	static int level=0;
	DNODE* pRoot = *ppNode;
	DCONDITION* pCondition;

	//if (level>10000)
	//{
		//DBG_STOP;
	//}

	if (level==0)
	{
	}
	if (pRoot == NULL)
		return TREE_EINVAL;
	
	if (pRoot->SignatureID)
	{
		pRoot->SignatureID = 0;
	}
	
	TreeGetNextNodeChild (pRTCtx, pRoot, &pChild);
	while (pChild != NULL)
	{
		level++;
		error = TreeDeleteNode(pRTCtx, &pChild);
		if (TREE_FAILED(error))
			return error;
		level--;
		TreeGetNextNodeChild(pRTCtx, pRoot, &pChild);
	};
	
	pCondition = pRoot->pCondition;
	while (pCondition != NULL) {
		DCONDITION* pConditionNext = pCondition->pNextCondition;
		_HeapFree(pRTCtx->pHeapCtx, pCondition);
		pCondition = pConditionNext;
	}
	
	switch (pRoot->N.NodeType.NodeTypeFields.NodeDataType)
	{
	case NDT_P_List:
		{
			NODEDATA_P_LIST* pListChild = pRoot->NodeData.pNodeDataList;
			while (pListChild != NULL) {
				NODEDATA_P_LIST* pListChildNext = pListChild->pNextChild;
				_HeapFree(pRTCtx->pHeapCtx, pListChild);
				pListChild = pListChildNext;
			}
		}
		break;
	case NDT_P_Index32:
		{
			int i;
			NODEDATA_P_INDEX32* pIndex;
			pIndex = pRoot->NodeData.pNodeDataIndex; 
			for (i=0; i<(sizeof(pIndex->Index)/sizeof(pIndex->Index[0]));i++)
			{
				if (pIndex->Index[i]!=NULL)
					_HeapFree(pRTCtx->pHeapCtx, pIndex->Index[i]);
				pIndex->Index[i] = NULL;
			}
		}
		_HeapFree(pRTCtx->pHeapCtx, pRoot->NodeData.pNodeDataIndex);
		pRoot->NodeData.pNodeDataIndex = NULL;
		break;
	case NDT_P_Joined:
		_HeapFree(pRTCtx->pHeapCtx, pRoot->NodeData.pNodeDataJoined);
		pRoot->NodeData.pNodeDataJoined = NULL;
		break;
	case NDT_P_JoinedRT:
		//ODS(("TreeDeleteNode: Run-Time node in tree???"));
		///DBG_STOP;
		break;
	case NDT_P_Hashed:
		_HeapFree(pRTCtx->pHeapCtx, pRoot->NodeData.pNodeDataHashed);
		pRoot->NodeData.pNodeDataHashed = NULL;
		break;
	case NDT_P_HashedRT:
		//ODS(("TreeDeleteNode: Run-Time node in tree???"));
		//DBG_STOP;
		break;
	default:
		DBG_STOP;
	}
	
	if (level == 0)
	{
		_HeapFree(pRTCtx->pHeapCtx, pRoot);
		*ppNode = NULL;
		//		ODSN(("Nodes joined: %d\n", joined);
	}
	return error;
}	

// ------------------------------------------------------------------------------------------

TREE_ERR TreeDeleteRootNode (sRT_CTX* pRTCtx)
{
	return TreeDeleteNode (pRTCtx, (DNODE **) &pRTCtx->pRoot);
}	

// ------------------------------------------------------------------------------------------

NODE* TreeGetNextNodeList (NODE* pNode, uint8_t NodeByte, sRT_CTX* pRTCtx)
{
	NODEDATA_P_LIST* pChild;
	
	pChild = PDNODE(pNode)->NodeData.pNodeDataList; 

	while (pChild != NULL) 
	{
		if (pChild->NodeByte == NodeByte)
		{
			// Node already exist
			return (NODE *) &(pChild->Node);
		}
		if (pChild->NodeByte > NodeByte) // No node
			break;
		pChild = pChild->pNextChild;
	} 
	
	return NULL;
}

// ------------------------------------------------------------------------------------------------

NODE* TreeGetNextNodeIndexed (NODE* pNode, uint8_t NodeByte, sRT_CTX* pRTCtx)
{
	return (NODE *)(PDNODE(pNode)->NodeData.pNodeDataIndex->Index [NodeByte]);
}

// ------------------------------------------------------------------------------------------------

NODE* TreeGetNextNodeJoined (NODE* pNode, uint8_t NodeByte, sRT_CTX* pRTCtx)
{
	DNODE* pJoinedRTNode;
	NODEDATA_P_JOINED_RT* pNDJoinedRT;
	NODEDATA_P_JOINED* pNDJoined;
	
	pNDJoined = PDNODE(pNode)->NodeData.pNodeDataJoined;
	if (pNDJoined->NodeBytes[0] != NodeByte)
		return NULL;
	
	if (pNDJoined->dwNumOfBytes == 1)
		return & pNDJoined->Node.N;

	if (TREE_FAILED(AllocJoinedRTNode(pRTCtx, &pJoinedRTNode, &pNDJoinedRT)))
	{
		DBG_STOP;
		return NULL;
	}
	/*
	error = HeapAlloc(pRTCtx->pHeapCtx, (void**)&pJoinedRTNode, sizeof(NODE));
	if (pJoinedRTNode == NULL)
		return NULL;
	error = HeapAlloc(pRTCtx->pHeapCtx, (void**)&pNDJoinedRT, sizeof(NODEDATA_P_JOINED_RT));
	if (pNDJoinedRT == NULL)
		return NULL;
	*/
	
	pJoinedRTNode->N.NodeType.NodeTypeFields.NodeType = NT_Dynamic;
	pJoinedRTNode->N.NodeType.NodeTypeFields.NodeDataType = NDT_P_JoinedRT;
	pJoinedRTNode->SignatureID = 0;
	pJoinedRTNode->NodeData.pNodeDataJoinedRT = pNDJoinedRT;
	pNDJoinedRT->dwCurrentByte = 0;
	pNDJoinedRT->pParent.Node = PDNODE(pNode);

	return PNODE (pJoinedRTNode);
}

// ------------------------------------------------------------------------------------------------

NODE* TreeGetNextNodeJoinedRT (NODE* pNode, uint8_t NodeByte, sRT_CTX* pRTCtx)
{
	NODEDATA_P_JOINED_RT* pNDJoinedRT;
	NODEDATA_P_JOINED* pNDJoined;

	pNDJoinedRT = PDNODE(pNode)->NodeData.pNodeDataJoinedRT;
	pNDJoined = pNDJoinedRT->pParent.Node->NodeData.pNodeDataJoined;

	pNDJoinedRT->dwCurrentByte++;
	
//	if (pNDJoinedRT->pParentNode->NodeType.NodeTypeFields.NodeDataType != NDT_P_Joined)
//	{
//		ODSN(("Oops: pTNode->pParentNode->NodeType.NodeTypeFields.NodeDataType != Joinedn"));
//	}
	if (pNDJoinedRT->dwCurrentByte >= pNDJoined->dwNumOfBytes)
	{
		ODSN(("Oops: (pNDJoined->dwNumOfBytes >= pNDJoinedRT->dwCurrentByte)n"));
	}

	if (pNDJoined->NodeBytes[pNDJoinedRT->dwCurrentByte] == NodeByte)
	{
		if (pNDJoined->dwNumOfBytes == pNDJoinedRT->dwCurrentByte + 1) // last byte in array
		{
			FreeJoinedRTNode (pRTCtx, PDNODE (pNode));
			//_HeapFree(pRTCtx->pHeapCtx, pNDJoinedRT);
			//_HeapFree(pRTCtx->pHeapCtx, pNode);
			return (NODE *) & pNDJoined->Node;
		}

		return pNode;
	}
	
	FreeJoinedRTNode (pRTCtx, PDNODE(pNode));
	//_HeapFree(pRTCtx->pHeapCtx, pNDJoinedRT);
	//_HeapFree(pRTCtx->pHeapCtx, pNode);
	
	return NULL;
}

// ------------------------------------------------------------------------------------------------

NODE* TreeGetNextNodeHashed (NODE* pNode, uint8_t NodeByte, sRT_CTX* pRTCtx)
{
	DNODE* pHashedNode;
	NODEDATA_P_HASHED_RT* pNDHashedRT;
	NODEDATA_P_HASHED* pNDHashed;
	
	pNDHashed = PDNODE(pNode)->NodeData.pNodeDataHashed;
	if (TREE_FAILED(AllocHashedRTNode (pRTCtx, &pHashedNode, &pNDHashedRT)))
	{
		DBG_STOP;
		return NULL;
	}

	pHashedNode->N.NodeType.NodeTypeFields.NodeType = NT_Dynamic;
	pHashedNode->N.NodeType.NodeTypeFields.NodeDataType = NDT_P_HashedRT;
	pHashedNode->SignatureID = 0;
	pHashedNode->NodeData.pNodeDataHashedRT = pNDHashedRT;
	pNDHashedRT->dwCurrentByte = 0;
	pNDHashedRT->pParent.Node = PDNODE(pNode);
	return TreeGetNextNodeHashedRT (&pHashedNode->N, NodeByte, pRTCtx);
}

// ------------------------------------------------------------------------------------------------

NODE* TreeGetNextNodeHashedRT (NODE* pNode, uint8_t NodeByte, sRT_CTX* pRTCtx)
{
	NODEDATA_P_HASHED_RT* pNDHashedRT;
	NODEDATA_P_HASHED* pNDHashed;
	
	// RT nodes: node data follows NODE structure
	pNDHashedRT = PDNODE (pNode)->NodeData.pNodeDataHashedRT;
	pNDHashed = pNDHashedRT->pParent.Node->NodeData.pNodeDataHashed;
	
//	if (pNDHashedRT->pParentNode->NodeType.NodeTypeFields.NodeDataType != NDT_P_Hashed)
//	{
//		ODSN(("Oops: pTNode->pParentNode->NodeType.NodeTypeFields.NodeDataType != NDT_P_Hashed"));
//		DBG_STOP;
//	}

	if (pNDHashedRT->dwCurrentByte >= pNDHashed->dwNumOfBytes)
	{
		ODSN(("Oops: (pNDHashed->dwNumOfBytes >= pNDHashedRT->dwCurrentByte)"));
		DBG_STOP;
	}

	pNDHashedRT->dwCurrentHashVal = CountCRC32(sizeof(NodeByte), &NodeByte, pNDHashedRT->dwCurrentHashVal);
	pNDHashedRT->dwCurrentByte += sizeof(NodeByte);
	
	
	if (pNDHashedRT->dwCurrentByte == pNDHashed->dwNumOfBytes)
	{
		if (pNDHashedRT->dwCurrentHashVal == pNDHashed->dwHashVal)
		{
			FreeHashedRTNode (pRTCtx, PDNODE (pNode));
			return PNODE (& pNDHashed->Node);
		}
		
		FreeHashedRTNode (pRTCtx, PDNODE (pNode));
		return NULL;
	}

	return pNode;
}

// ------------------------------------------------------------------------------------------------

uint32_t TreeGetNodeChildrenCount(DNODE* pNode)
{
	uint32_t dwCount = 0;
	
	if (pNode == NULL)
		return 0;
	
	switch (pNode->N.NodeType.NodeTypeFields.NodeDataType)
	{
	case NDT_P_List:
		{
			NODEDATA_P_LIST* pListChild = pNode->NodeData.pNodeDataList;
			while (pListChild != NULL) {
				dwCount++;
				pListChild = pListChild->pNextChild;
			}
		}
		break;
	case NDT_P_Index32:
		{
			uint32_t i;
			NODEDATA_P_INDEX32* pIndex;
			pIndex = pNode->NodeData.pNodeDataIndex; 
			for (i=0; i<(sizeof(pIndex->Index)/sizeof(pIndex->Index[0]));i++)
			{
				if (pIndex->Index[i]!=NULL)
					dwCount++;
			}
		}
		break;
	case NDT_P_Joined:
	case NDT_P_JoinedRT:
	case NDT_P_Hashed:
	case NDT_P_HashedRT:
		dwCount = 1;
		break;
	default:
		DBG_STOP;
	}
	
	return dwCount;
}	

// ------------------------------------------------------------------------------------------------

TREE_ERR _TREE_CALL TreeUnload_Dynamic(sRT_CTX* pRTCtx)
{
	TREE_ERR error;
	
	if (pRTCtx->sTreeInfo.dwMagic == 0) // no data
		return TREE_OK;

	if (pRTCtx->sTreeInfo.dwMagic != MAGIC_TREE_DB_V2) 
		return TREE_EUNEXPECTED;

	if (pRTCtx->sTreeInfo.dwFormat != cFORMAT_DYNAMIC) 
		return TREE_EUNEXPECTED;

	if (pRTCtx->pRoot)
		error = TreeDeleteRootNode(pRTCtx);
	ResetRTContext(pRTCtx, pRTCtx->pHeapCtx);

	return error;
}

// ------------------------------------------------------------------------------------------------

TREE_ERR _TREE_CALL TreeUnload_Any(sRT_CTX* pRTCtx)
{
	if (pRTCtx->sTreeInfo.dwMagic == 0) // no data
		return TREE_OK;

	if (pRTCtx->sTreeInfo.dwMagic != MAGIC_TREE_DB_V2) 
		return TREE_EUNEXPECTED;

	if (pRTCtx->sTreeInfo.dwFormat == cFORMAT_STATIC) 
		return TreeUnload_Static(pRTCtx);

	if (pRTCtx->sTreeInfo.dwFormat == cFORMAT_DYNAMIC) 
		return TreeUnload_Dynamic(pRTCtx);

	return TREE_EUNEXPECTED;
}

// ------------------------------------------------------------------------------------------------

#endif // TREE_STATIC_ONLY

