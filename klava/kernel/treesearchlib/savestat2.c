// savestat2.c
//

#include "common.h"
#include "savedyn.h"
#include "aktree/avl.h"
#include "crc32.h"
#include "dynarray.h"

#define cITERATION_SAVE		1
#define cITERATION_BUILD_INDEX	2

// number of first signature characters to keep
#define FIRST_SIG_CHAR_COUNT 4

enum RegisterDataTypes {
	DT_Node      = 1,
	DT_NodeData  = 2,
	DT_NodeType  = 3,
	DT_Offset    = 4,
	DT_SubstNode = 5,
	DT_Condition = 6,
	DT_OptimisedAnyByteCondition = 7,

	DT_MAX,
};

#define GET_OFFSET(base_ptr, dest_ptr) ((uint32_t)((uint8_t *)(dest_ptr) - (uint8_t *)(base_ptr)))
#define REGISTER_DELAYED_POINTER(pSaveCtx, ptr, field, type) \
		(error = RegisterDelayedPointer(pSaveCtx, field, type, GetCurrOffset(pSaveCtx->pIOCtx) + GET_OFFSET(ptr, &(field))))
#define REGISTER_DELAYED_NDT(pSaveCtx, pNode, psNode) \
		(error = RegisterDelayedPointerEx(pSaveCtx, (void *)((uintptr_t)pNode+0x80000000), DT_NodeType, GetCurrOffset(pSaveCtx->pIOCtx), 0x1F, *(uint32_t*)psNode, 0, 0))
#define UPDATE_DELAYED_NDT(pSaveCtx, pNode, nodetype) \
		(error = UpdateDelayedPointerEx(pSaveCtx, (void *)((uintptr_t)pNode+0x80000000), DT_NodeType, (uint32_t)nodetype))
#define WRITE_ALIGN(pSaveCtx) \
		if (GetCurrOffset(pSaveCtx->pIOCtx) & 3) \
			error = _Write(pSaveCtx->pIOCtx, "\x0f\x0f\x0f\x0f", 4-(GetCurrOffset(pSaveCtx->pIOCtx) & 3));
#ifndef min 
#define min(a,b)  (((a) < (b)) ? (a) : (b))
#endif

// ------------------------------------------------------------------------------------------------

// structure to enforce proper alignment
typedef union tagBYTE4
{
	uint8_t  b [4];
	uint16_t  w [2];
	uint32_t l;
} BYTE4;

typedef struct tag_SPLIT_JOINED_NODE {
	DNODE Node;
	NODEDATA_P_HASHED NodeDataHashed;
	NODEDATA_P_JOINED NodeDataJoined;
} SPLIT_JOINED_NODE;

typedef struct tag_DELAYED_POINTER {
	void *   ptr;
	uint32_t dwType;
	uint32_t dwOffset;
	uint32_t dwBaseOffset;
	uint32_t dwBitFieldMask;
	uint32_t dwBitFieldData;
	uint32_t dwAlignment;
} DELAYED_POINTER;


typedef struct tag_SAVE_STAT_INFO {
	uint32_t dwNodeItself;
	uint32_t dwNodeDataList;
	uint32_t dwNodeDataJoined;
	uint32_t dwNodeDataJoinedBytes;
	uint32_t dwNodeDataIndexed;
	uint32_t dwNodeDataHashed;
	uint32_t dwNodeConditions;
	uint32_t dwNodeOrfaned;
} SAVE_STAT_INFO;

enum SigCharConditionType
{
	cSCT_BYTE = 0,
	cSCT_AND,
	cSCT_OR,
	cSCT_RANGE,
	cSCT_ANY,
	cSCT_ASTER
};

// character information, saved for first 4 bytes of signatures
typedef struct tag_SIG_CHAR_INFO {
	uint8_t bCondType;	 // cSCT_XXX
	uint8_t bCondByte1;    // AND,OR: mask, RANGE: lower bound
	uint8_t bCondByte2;    // BYTE,AND,OR: value, RANGE: upper bound
	uint8_t bCondReserved; // reserved, not used
} SIG_CHAR_INFO;

typedef struct tag_SAVE_CTX {
	sIO_CTX* pIOCtx;
	uint32_t  dwNodesToSave;
	sRT_CTX*  pRTCtx;
	DELAYED_POINTER* pDelayedPointers;
	uint32_t dwDelayedOffsetsCount;
	uint32_t dwDelayedOffsetsAllocated;
	SIG_CHAR_INFO pSigChars [FIRST_SIG_CHAR_COUNT];   // first N signature characters
	uint8_t* pCacheTable;
	TREEHANDLE hTree[DT_MAX];
	TREE_INFO sTreeInfo;
	DYN_ARRAY* pDynArray;
	uint32_t  dwLastAnyByteConditionCounterOffset;
} SAVE_CTX;

// structure for storing information about node data to save
typedef struct tag_NODE_BODY {
	uint8_t  bCondType;     // cSCT_XXX
	uint8_t  bCondByte1;    // AND,OR: mask, RANGE: lower bound, else UNDEF
	uint8_t  bCondByte2;    // AND,OR: value, RANGEe: upper bound, else UNDEF
	uint8_t  bCondReserved; // reserved, not used
	uint32_t dwByteCount;   // BYTE,ANY: byte count, else 1 (always defined)
	uint8_t *pBytes;        // BYTE (opt): byte values, else NULL
} NODE_BODY;

// ------------------------------------------------------------------------------------------------

// ------------------------------------------------------------------------------------------------

TREE_ERR TreeSaveNode3 (DNODE* pNode, SAVE_CTX* pSaveCtx, uint32_t dwCurLevel, uint32_t dwSaveIteration, NODE_BODY *nodeBody);

static TREE_ERR TreeSetFilterByte (SAVE_CTX *pSaveCtx, BYTE4 *bytes, uint32_t idx);

TREE_ERR TreeSaveNodeBytes3 (DNODE* pNode, SAVE_CTX* pSaveCtx, uint32_t dwCurLevel, uint32_t dwSaveIteration, uint8_t *pBytes, uint32_t dwByteCount)
{
	NODE_BODY nb;
	nb.bCondType = cSCT_BYTE;
	nb.dwByteCount = dwByteCount;
	nb.pBytes = pBytes;
	return TreeSaveNode3 (pNode, pSaveCtx, dwCurLevel, dwSaveIteration, &nb);
}

// ------------------------------------------------------------------------------------------------

int compare_qword(void* data1, void* data2)
{
	if (*(uint64_t*)data1 > *(uint64_t*)data2)
		return 1;
	else if (*(uint64_t*)data1 < *(uint64_t*)data2)
		return -1;
	return 0;
}

TREE_ERR InitDelayedPointers(SAVE_CTX* pSaveCtx)
{
	int i;
	for (i=0; i<DT_MAX; i++)
	{
		pSaveCtx->hTree[i] = _TreeCreate(NULL, pSaveCtx->pRTCtx->pHeapCtx);
		if (pSaveCtx->hTree[i] == 0)
			return TREE_EUNEXPECTED;
	}

	return TREE_OK;
}

TREE_ERR FreeDelayedPointersData(SAVE_CTX* pSaveCtx)
{
	TREENODE node;
	
	node = _TreeFirst(pSaveCtx->hTree[DT_SubstNode]);
	while (node)
	{
		DELAYED_POINTER* pdp;
		pdp = _TreeNodeData(node);
		if (pdp)
			_HeapFree(pSaveCtx->pRTCtx->pHeapCtx, (void *)pdp->dwOffset);
		node = _TreeNext(node);
	} 
	return TREE_OK;
}

TREE_ERR DeinitDelayedPointers(SAVE_CTX* pSaveCtx)
{
	int i;
	TREE_ERR error = TREE_OK;
	error = FreeDelayedPointersData(pSaveCtx);

	for (i=0; i<DT_MAX; i++)
	{
		if (pSaveCtx->hTree[i])
			_TreeDelete(pSaveCtx->hTree[i]);
	}

	return error;
}


TREE_ERR RegisterDelayedPointerEx(SAVE_CTX* pSaveCtx, void* ptr, enum RegisterDataTypes dwType, uint32_t dwOffset, uint32_t dwBitFieldMask, uint32_t dwBitFieldData, uint32_t dwBaseOffset, uint32_t dwAlignment)
{
	TREE_ERR error = TREE_OK;
	DELAYED_POINTER dp;
	
	if (ptr == NULL)
		return TREE_OK;

	if (dwType >= DT_MAX)
		return TREE_EINVAL;

	dp.dwOffset = dwOffset;
	dp.dwBaseOffset = dwBaseOffset;
	dp.ptr = ptr;
	dp.dwType = dwType;
	dp.dwBitFieldMask = dwBitFieldMask;
	dp.dwBitFieldData = dwBitFieldData;
	dp.dwAlignment = dwAlignment;

	error = _TreeAdd(pSaveCtx->hTree[dwType], &dp, sizeof(dp));
	if (error == 1)
		error = TREE_EDUPLICATE;
	else if (error != 0)
		error = TREE_EUNEXPECTED;
	if (TREE_FAILED(error))
		DBG_STOP;
	return error;
}

TREE_ERR RegisterDelayedPointer(SAVE_CTX* pSaveCtx, void* ptr, enum RegisterDataTypes type,  uint32_t dwOffset)
{
	return RegisterDelayedPointerEx(pSaveCtx, ptr, type, dwOffset, 0, 0, 0, 4);
}


TREENODE iGetDelayedPointerNode(SAVE_CTX* pSaveCtx, void* ptr, enum RegisterDataTypes dwType)
{
	DELAYED_POINTER dp;
	TREENODE node;

	if (dwType >= DT_MAX)
		return TREE_EINVAL;

	dp.ptr = ptr;
	node =  _TreeSearch(pSaveCtx->hTree[dwType], &dp, sizeof(dp));
	return node;
}


bool_t IsDelayedPointer(SAVE_CTX* pSaveCtx, void* ptr, enum RegisterDataTypes dwType)
{
	TREENODE node;
	node = iGetDelayedPointerNode(pSaveCtx, ptr, dwType);
	return (node != 0);
}

bool_t RemoveDelayedPointer(SAVE_CTX* pSaveCtx, void* ptr, enum RegisterDataTypes dwType)
{
	DELAYED_POINTER dp;

	if (dwType >= DT_MAX)
		return TREE_EINVAL;

	dp.ptr = ptr;
	return (0 == _TreeRemove(pSaveCtx->hTree[dwType], &dp, sizeof(dp)));
}


void * GetDelayedPointer(SAVE_CTX* pSaveCtx, void* ptr, enum RegisterDataTypes dwType)
{
	DELAYED_POINTER dp;
	DELAYED_POINTER* pdp = &dp;
	TREENODE node;

	node = iGetDelayedPointerNode(pSaveCtx, ptr, dwType);
	if (node)
	{
		pdp = _TreeNodeData(node);
		if (pdp)
			return (void *)pdp->dwOffset;
	}

	return NULL;
}

TREE_ERR ReplaceDelayedPointer(SAVE_CTX* pSaveCtx, enum RegisterDataTypes dwType, void* oldptr, void* ptr)
{
	DELAYED_POINTER dp;
	DELAYED_POINTER* pdp = &dp;
	TREENODE node;
	TREE_ERR error = TREE_OK;

	if (dwType >= DT_MAX)
		return TREE_EINVAL;

	dp.ptr = oldptr;
	//dp.dwType = dwType;
	node = _TreeSearch(pSaveCtx->hTree[dwType], &dp, sizeof(dp));
	if (!node)
		return TREE_ENOTFOUND;
	pdp = _TreeNodeData(node);
	memcpy(&dp, pdp, sizeof(dp));
	dp.ptr = ptr;
	_TreeRemove(pSaveCtx->hTree[dwType], &dp, sizeof(dp));
	_TreeAdd(pSaveCtx->hTree[dwType], &dp, sizeof(dp));
	if (error == 1)
		error = TREE_EDUPLICATE;
	else if (error != 0)
		error = TREE_EUNEXPECTED;
	return error;
}

TREE_ERR UpdateDelayedPointerEx(SAVE_CTX* pSaveCtx, void* ptr, uint32_t dwType, uint32_t dwOffset)
{
	TREE_ERR error = TREE_OK;
	DELAYED_POINTER dp;
	DELAYED_POINTER* pdp = &dp;
	TREENODE node;

	if (dwType >= DT_MAX)
		return TREE_EINVAL;
	

	dp.ptr = ptr;
	//dp.dwType = dwType;
	node = _TreeSearch(pSaveCtx->hTree[dwType], &dp, sizeof(dp));
	if (node)
	{
		pdp = _TreeNodeData(node);
		if (pdp)
		{
			memcpy(&dp, pdp, sizeof(dp));
			_TreeRemove(pSaveCtx->hTree[dwType], pdp, sizeof(dp));
			pdp = &dp;
		}
	}
	
	if (node)
	{
		dwOffset -= pdp->dwBaseOffset;
		if (pdp->dwAlignment)
			dwOffset /= pdp->dwAlignment;
		
		if (pdp->dwBitFieldMask != 0)
		{
			uint32_t dwShift = 0;
			uint32_t dwData;
			uint32_t dwBitFieldMask = pdp->dwBitFieldMask;
			uint32_t dwShiftedMask = dwBitFieldMask;
			
			while ((dwShiftedMask & 1) == 0)
			{
				dwShiftedMask >>= 1;
				dwShift++;
			}
			if ((dwOffset & dwShiftedMask) != dwOffset)
			{
				// Cannot update delayed pointer - offset to large for bitfield
				DBG_STOP;
				return TREE_EUNEXPECTED;
			}
			
			if (dwShiftedMask == 0xFF && (dwShift & 7)==0) // write 1 byte
			{
                                uint8_t bOffset = (uint8_t)dwOffset;
				error = _SeekWrite(pSaveCtx->pIOCtx, pdp->dwOffset+dwShift/8, &bOffset, sizeof(uint8_t));
			}
			else
			{
				//dwData = pdp->dwBitFieldData;
				error = _SeekRead(pSaveCtx->pIOCtx, pdp->dwOffset, &dwData, sizeof(dwData));
				dwData &= ~dwBitFieldMask;
				dwOffset <<= dwShift;
				dwData |= dwOffset;
				if (TREE_SUCCEEDED(error))
					error = _SeekWrite(pSaveCtx->pIOCtx, pdp->dwOffset, &dwData, sizeof(dwData));
			}
		}
		else
		{
			error = _SeekWrite(pSaveCtx->pIOCtx, pdp->dwOffset, &dwOffset, sizeof(dwOffset));
		}
		
		if (TREE_SUCCEEDED(error))
			return TREE_OK;
	}
	return error;
}

uint32_t GetCurrOffset(sIO_CTX* pIOCtx)
{
	uint64_t qwOffset;
	_Seek(pIOCtx, &qwOffset, 0, TREE_IO_CTX_SEEK_CUR);
	return (uint32_t)qwOffset;
}


enum NodeTypes TreeGetNodeOptimalType(SAVE_CTX* pSaveCtx, DNODE* pFullNode, uint32_t* pdwNodeSize)
{
	DCONDITION *pCondition = pFullNode->pCondition;
	
	if (IsDelayedPointer(pSaveCtx, pCondition, DT_OptimisedAnyByteCondition))
		pCondition = NULL;

	if (pFullNode->SignatureID==0 && pFullNode->NodeData.pNodeData==NULL && pCondition!=NULL)
	{
		if (pdwNodeSize)
			*pdwNodeSize = sizeof (SNODE_C);
		return NT_C;
	}

	if (pFullNode->SignatureID!=0 && (pFullNode->SignatureID & 0xFF000000)==0 && pFullNode->NodeData.pNodeData==NULL && pCondition==NULL)
	{
		// tail node
		if (pdwNodeSize)
			*pdwNodeSize = sizeof (SNODE_S);
		return NT_S;
	}
	
	if (pFullNode->SignatureID==0 && pFullNode->NodeData.pNodeData!=0 && pCondition==NULL)
	{
		if (pdwNodeSize)
			*pdwNodeSize = sizeof(SNODE_D);
		return NT_D;
	}
	
	if (pFullNode->SignatureID!=0 && (pFullNode->SignatureID & 0xFF000000)==0 && pFullNode->NodeData.pNodeData!=0 && pCondition==NULL)
	{
		if (pdwNodeSize)
			*pdwNodeSize = sizeof(SNODE_SD);
		return NT_SD;
	}

	if (pdwNodeSize)
		*pdwNodeSize = sizeof(SNODE);

	return NT_Full;
};

// ------------------------------------------------------------------------------------------------

TREE_ERR TreeSaveNodeItself3(DNODE* pNode, SAVE_CTX* pSaveCtx, uint32_t dwCurLevel, uint32_t dwSaveIteration)
{
	TREE_ERR error = TREE_OK;

	if (dwSaveIteration == cITERATION_SAVE)
	{
		enum NodeTypes OptNodeType;
		uint32_t dwNodeBase;
#ifdef _USE_TAGS
//		_Write(pSaveCtx->pIOCtx, "<N>>", sizeof(uint32_t));
#endif
		dwNodeBase = GetCurrOffset(pSaveCtx->pIOCtx);

		error = UpdateDelayedPointerEx(pSaveCtx, pNode, DT_Node, dwNodeBase);
		//	RET_ON_FAIL; - some nodes are embedded into NODEDATA_P_XXX and has no references
		{
			DNODE* pSubstNode = GetDelayedPointer(pSaveCtx, pNode, DT_SubstNode);
			if (pSubstNode)
			{
				pNode = pSubstNode;
				error = UpdateDelayedPointerEx(pSaveCtx, pNode, DT_Node, dwNodeBase);
			}
		}
		//	RET_ON_FAIL; - some nodes are embedded into NODEDATA_P_XXX and has no references

//pSaveCtx->sSaveStatInfo.dwNodeItself++;

		OptNodeType = TreeGetNodeOptimalType(pSaveCtx, pNode, NULL);

		switch (OptNodeType)
		{
		case NT_S:
			{
				SNODE_S TmpNode;
				TmpNode.NodeType.NodeType = NT_S;
				TmpNode.NodeType.NodeDataType = NDT_InvalidType;
				TmpNode.NodeType.SignatureID = pNode->SignatureID;
				REGISTER_DELAYED_NDT(pSaveCtx, pNode, &TmpNode);
				RET_ON_FAIL;
				error = _Write(pSaveCtx->pIOCtx, &TmpNode, sizeof(TmpNode));
			}
			break;
		case NT_D:
			{
				SNODE_D TmpNode;
				TmpNode.NodeType.NodeType = NT_D;
				TmpNode.NodeType.NodeDataType = NDT_InvalidType;
				TmpNode.NodeType.NodeDataOffset = 0xCCCCCCCC;
				REGISTER_DELAYED_NDT(pSaveCtx, pNode, &TmpNode);
				RET_ON_FAIL;
				error = RegisterDelayedPointerEx(pSaveCtx, pNode->NodeData.pNodeData, DT_NodeData, dwNodeBase, 0xFFFFFF00, *(uint32_t*)&TmpNode, dwNodeBase, 4);
				RET_ON_FAIL;
				error = _Write(pSaveCtx->pIOCtx, &TmpNode, sizeof(TmpNode));
			}
			break;
		case NT_C:
			{
				SNODE_C TmpNode;
				TmpNode.NodeType.NodeType = NT_C;
				TmpNode.NodeType.NodeDataType = NDT_InvalidType;
				TmpNode.NodeType.ConditionOffset = 0xCCCCCCCC;
				REGISTER_DELAYED_NDT(pSaveCtx, pNode, &TmpNode);
				RET_ON_FAIL;
				error = RegisterDelayedPointerEx(pSaveCtx, pNode->pCondition, DT_Condition, dwNodeBase, 0xFFFFFF00, *(uint32_t*)&TmpNode, dwNodeBase, 4);
				RET_ON_FAIL;
				error = _Write(pSaveCtx->pIOCtx, &TmpNode, sizeof(TmpNode));
			}
			break;
		case NT_SD:
			{
				SNODE_SD TmpNode;
				TmpNode.NodeType.NodeType = NT_SD;
				TmpNode.NodeType.NodeDataType = NDT_InvalidType;
				TmpNode.SignatureID = pNode->SignatureID;
				TmpNode.NodeType.NodeDataOffset = 0xCCCCCCCC;
				REGISTER_DELAYED_NDT(pSaveCtx, pNode, &TmpNode);
				RET_ON_FAIL;
				//error = RegisterDelayedPointerEx(pSaveCtx, pNode->NodeData.pNodeData, DT_NodeData, dwNodeBase+((uint32_t)&TmpNode.NodeDataOffset - (uint32_t)&TmpNode), 0, 0, dwNodeBase, 4);
				error = RegisterDelayedPointerEx(pSaveCtx, pNode->NodeData.pNodeData, DT_NodeData, dwNodeBase, 0xFFFFFF00, *(uint32_t*)&TmpNode, dwNodeBase, 4);
				RET_ON_FAIL;
				error = _Write(pSaveCtx->pIOCtx, &TmpNode, sizeof(TmpNode));
			}
			break;
		default:
			{
				SNODE TmpNode;
				TmpNode.N.NodeType.NodeTypeFields.NodeType = NT_Full;
				TmpNode.N.NodeType.NodeTypeFields.NodeDataType = NDT_InvalidType;
				TmpNode.N.NodeType.NodeTypeFields.NodeData = 0;
				TmpNode.SignatureID = pNode->SignatureID;
				TmpNode.dwNodeDataOffset  = 0xCCCCCCCC;
				TmpNode.dwConditionOffset = 0xCCCCCCCC;
				REGISTER_DELAYED_NDT(pSaveCtx, pNode, &TmpNode);
				RET_ON_FAIL;
				//REGISTER_DELAYED_POINTER(pSaveCtx, pNode, pNode->pCondition, DT_Condition);
				error = RegisterDelayedPointerEx(pSaveCtx, pNode->pCondition, DT_Condition, dwNodeBase + GET_OFFSET(&TmpNode, &TmpNode.dwConditionOffset), 0, 0, dwNodeBase, 4);
				RET_ON_FAIL;
				error = RegisterDelayedPointerEx(pSaveCtx, pNode->NodeData.pNodeData, DT_NodeData, dwNodeBase +  GET_OFFSET(&TmpNode, &TmpNode.dwNodeDataOffset), 0, 0, dwNodeBase, 4);
				RET_ON_FAIL;
				error = _Write(pSaveCtx->pIOCtx, &TmpNode, sizeof(TmpNode));
			}
		}
		RET_ON_FAIL;
	}
	
	return error;	
}

// ------------------------------------------------------------------------------------------------

TREE_ERR TreeSaveNodeDataListAsArray3(DNODE* pNode, SAVE_CTX* pSaveCtx, uint32_t dwCurLevel, uint32_t dwSaveIteration)
{
	TREE_ERR error = TREE_OK;
	NODEDATA_P_LIST* pCNode = NULL;
	uint32_t dwChildCount;


	if (dwSaveIteration == cITERATION_SAVE)
	{
		uint32_t dwNodeOffset;
		uint32_t dwBitFieldMask = 0;
		uint32_t dwListN = 0;
		bool_t   bUseOffsets = 0;
		uint8_t  bChildCount = 0;
		pCNode = pNode->NodeData.pNodeDataList;
		while (pCNode!=NULL) 
		{
			uint32_t dwNodeSize;
			if (pCNode->pNextChild == NULL)
				break; // size of last child doesn't matter
			TreeGetNodeOptimalType(pSaveCtx, &pCNode->Node, &dwNodeSize);
			if (dwNodeSize != sizeof(uint32_t))
				bUseOffsets = 1;
			pCNode = pCNode->pNextChild;
		}

#ifdef _USE_TAGS
		if (bUseOffsets)
			_Write(pSaveCtx->pIOCtx, "<A>>", sizeof(uint32_t));
		else
			_Write(pSaveCtx->pIOCtx, "<AF>", sizeof(uint32_t));
#endif
		dwNodeOffset = GetCurrOffset(pSaveCtx->pIOCtx);
		dwChildCount = TreeGetNodeChildrenCount (pNode);

//		if (pNode->NodeData.NodeData.pNodeDataList == NULL)
//	pSaveCtx->sSaveStatInfo.dwNodeOrfaned++;
		
		error = UpdateDelayedPointerEx(pSaveCtx, pNode->NodeData.pNodeData, DT_NodeData, dwNodeOffset);
		RET_ON_FAIL;
                
                bChildCount = dwChildCount;
		error = _Write(pSaveCtx->pIOCtx, &bChildCount, sizeof(uint8_t));
		RET_ON_FAIL;

		pCNode = pNode->NodeData.pNodeDataList;
		while (pCNode!=NULL)
		{
			error = _Write(pSaveCtx->pIOCtx, &pCNode->NodeByte, sizeof(uint8_t));
			RET_ON_FAIL;
			pCNode = pCNode->pNextChild;
		}
		
		if (bUseOffsets)
		{
			UPDATE_DELAYED_NDT(pSaveCtx, pNode, NDT_O_Array);
			RET_ON_FAIL;
			pCNode = pNode->NodeData.pNodeDataList;
			while (pCNode!=NULL)
			{
				uint8_t bFill = 0xCC;
				dwBitFieldMask <<= 8;
				if (dwBitFieldMask == 0)
					dwBitFieldMask = 0xFF;
				error = RegisterDelayedPointerEx(pSaveCtx, &pCNode->Node, DT_Node, dwNodeOffset+dwChildCount+1+(dwListN & (~3)), dwBitFieldMask, 0xCCCCCCCC, dwNodeOffset, 4);
				RET_ON_FAIL;
				error = _Write(pSaveCtx->pIOCtx, &bFill, sizeof(uint8_t));
				RET_ON_FAIL;
				pCNode = pCNode->pNextChild;
				dwListN++;
			}
		}
		else
		{
			UPDATE_DELAYED_NDT(pSaveCtx, pNode, NDT_O_ArrayFast);
			RET_ON_FAIL;
		}

		WRITE_ALIGN(pSaveCtx);
		RET_ON_FAIL;

		pCNode = pNode->NodeData.pNodeDataList;
		while (pCNode!=NULL)
		{
			error = TreeSaveNodeItself3(&pCNode->Node, pSaveCtx, dwCurLevel, dwSaveIteration);
			RET_ON_FAIL;
			pCNode = pCNode->pNextChild;
		}
	}
	else
	{
		pCNode = pNode->NodeData.pNodeDataList;
		while (pCNode!=NULL)
		{
			error = TreeSaveNodeBytes3 (&(pCNode->Node), pSaveCtx, dwCurLevel+1, dwSaveIteration, &pCNode->NodeByte, 1);
			RET_ON_FAIL;
			pCNode = pCNode->pNextChild;
		}
	}
	

	return error;
}

// ------------------------------------------------------------------------------------------------

static TREE_ERR TreeSave_NDT_O_Tail(DNODE* pNode, SAVE_CTX* pSaveCtx, uint32_t dwCurLevel, uint32_t dwSaveIteration)
{
	TREE_ERR error = TREE_OK;

	if (dwSaveIteration == cITERATION_SAVE)
	{
		UPDATE_DELAYED_NDT(pSaveCtx, pNode, NDT_O_Tail);
		RET_ON_FAIL;
//pSaveCtx->sSaveStatInfo.dwNodeOrfaned++;
	}
	
	return error;
}

// ------------------------------------------------------------------------------------------------

static TREE_ERR TreeSave_NDT_O_List(DNODE* pNode, SAVE_CTX* pSaveCtx, uint32_t dwCurLevel, uint32_t dwSaveIteration)
{
	if (pNode->NodeData.pNodeDataList)
		return TreeSaveNodeDataListAsArray3(pNode, pSaveCtx, dwCurLevel, dwSaveIteration);

	return TreeSave_NDT_O_Tail(pNode, pSaveCtx, dwCurLevel, dwSaveIteration);
}

// ------------------------------------------------------------------------------------------------

static TREE_ERR TreeSave_NDT_O_Hashed(DNODE* pNode, SAVE_CTX* pSaveCtx, uint32_t dwCurLevel, uint32_t dwSaveIteration)
{
	TREE_ERR error = TREE_OK;
	NODEDATA_P_HASHED* pNDHashed = pNode->NodeData.pNodeDataHashed;

	if (dwSaveIteration == cITERATION_SAVE)
	{
		NODEDATA_O_HASHED sHashed;

		sHashed.dwNumOfBytes = pNDHashed->dwNumOfBytes;
		sHashed.dwHashVal = pNDHashed->dwHashVal;

#ifdef _USE_TAGS
		_Write(pSaveCtx->pIOCtx, "<H>>", sizeof(uint32_t));
#endif
		UPDATE_DELAYED_NDT(pSaveCtx, pNode, NDT_O_Hashed);
		RET_ON_FAIL;
//pSaveCtx->sSaveStatInfo.dwNodeDataHashed++;
		error = UpdateDelayedPointerEx(pSaveCtx, pNode->NodeData.pNodeData, DT_NodeData, GetCurrOffset(pSaveCtx->pIOCtx));
		RET_ON_FAIL;

		error = _Write(pSaveCtx->pIOCtx, & sHashed, sizeof (sHashed));
		RET_ON_FAIL;
		
		error = TreeSaveNodeItself3(PDNODE (&pNDHashed->Node), pSaveCtx, dwCurLevel, dwSaveIteration);
		RET_ON_FAIL;
	}
	else
	{
		error = TreeSaveNodeBytes3(PDNODE (&pNDHashed->Node), pSaveCtx, dwCurLevel+pNDHashed->dwNumOfBytes, dwSaveIteration, 0, pNDHashed->dwNumOfBytes);
		RET_ON_FAIL;
	}

	return error;
}

// ------------------------------------------------------------------------------------------------

static TREE_ERR TreeSave_NDT_O_Joined8(DNODE* pNode, SAVE_CTX* pSaveCtx, uint32_t dwCurLevel, uint32_t dwSaveIteration)
{
	uint8_t *pBegNodeData, *pEndNodeData, *pCurNodeData;

	TREE_ERR error = TREE_OK;
	NODEDATA_P_JOINED* pNDJoined = NULL;
	uint8_t bNumOfBytes = 0;
	pNDJoined = pNode->NodeData.pNodeDataJoined;
	if (dwSaveIteration == cITERATION_SAVE)
	{
#ifdef _USE_TAGS
	_Write(pSaveCtx->pIOCtx, "<J8>", sizeof(uint32_t));
#endif

		error = UpdateDelayedPointerEx(pSaveCtx, pNode->NodeData.pNodeData, DT_NodeData, GetCurrOffset(pSaveCtx->pIOCtx));
		RET_ON_FAIL;
		UPDATE_DELAYED_NDT(pSaveCtx, pNode, NDT_O_Joined8);
		RET_ON_FAIL;
                bNumOfBytes = pNDJoined->dwNumOfBytes;
		error = _Write(pSaveCtx->pIOCtx, &bNumOfBytes, sizeof(bNumOfBytes));
		RET_ON_FAIL;

		// xor data
		pBegNodeData = pNDJoined->NodeBytes;
		pEndNodeData = pBegNodeData + pNDJoined->dwNumOfBytes;
		for(pCurNodeData=pBegNodeData; pCurNodeData<pEndNodeData; ++pCurNodeData) *pCurNodeData ^= XOR_DATA_MASK;

		error = _Write(pSaveCtx->pIOCtx, pNDJoined->NodeBytes, pNDJoined->dwNumOfBytes);
		// restore data
		for(pCurNodeData=pBegNodeData; pCurNodeData<pEndNodeData; ++pCurNodeData) *pCurNodeData ^= XOR_DATA_MASK;

		RET_ON_FAIL;
		WRITE_ALIGN(pSaveCtx);
		error = TreeSaveNodeItself3(&pNDJoined->Node, pSaveCtx, dwCurLevel, dwSaveIteration);
		RET_ON_FAIL;
	}
	else
	{
		error = TreeSaveNodeBytes3(&pNDJoined->Node, pSaveCtx, dwCurLevel+pNDJoined->dwNumOfBytes, dwSaveIteration, pNDJoined->NodeBytes, pNDJoined->dwNumOfBytes);
		RET_ON_FAIL;
	}

	return error;
}

// ------------------------------------------------------------------------------------------------

static TREE_ERR TreeSave_NDT_O_Joined32(DNODE* pNode, SAVE_CTX* pSaveCtx, uint32_t dwCurLevel, uint32_t dwSaveIteration)
{
	uint8_t *pBegNodeData, *pEndNodeData, *pCurNodeData;

	TREE_ERR error = TREE_OK;
	NODEDATA_P_JOINED* pNDJoined = NULL;
	uint32_t dwSaveAsJoined;
	uint32_t dwSaveAsHashed;
	uint32_t dwNumOfBytes;

	pNDJoined = pNode->NodeData.pNodeDataJoined;
	dwNumOfBytes = pNDJoined->dwNumOfBytes;

	if (dwSaveIteration == cITERATION_SAVE)
	{
		if (pNDJoined->dwNumOfBytes < 0xFF /*&& (pNDJoined->dwNumOfBytes & 3)!=0*/)
			return TreeSave_NDT_O_Joined8(pNode, pSaveCtx, dwCurLevel, dwSaveIteration);

#ifdef _USE_TAGS
		_Write(pSaveCtx->pIOCtx, "<J>>", sizeof(uint32_t));
#endif
		UPDATE_DELAYED_NDT(pSaveCtx, pNode, NDT_O_Joined32);
		RET_ON_FAIL;
//	pSaveCtx->sSaveStatInfo.dwNodeDataJoined++;
//	pSaveCtx->sSaveStatInfo.dwNodeDataJoinedBytes+=pNDJoined->dwNumOfBytes;
		if (pNDJoined->dwNumOfBytes < 2)
		{
			DBG_STOP
		}
		
		error = UpdateDelayedPointerEx(pSaveCtx, pNode->NodeData.pNodeData, DT_NodeData, GetCurrOffset(pSaveCtx->pIOCtx));
		RET_ON_FAIL;

		// xor data
		pBegNodeData = (uint8_t*)pNDJoined + sizeof(NODEDATA_P_JOINED);
		pEndNodeData = pBegNodeData + pNDJoined->dwNumOfBytes;
		for(pCurNodeData=pBegNodeData; pCurNodeData<pEndNodeData; ++pCurNodeData) *pCurNodeData ^= XOR_DATA_MASK;
		error = _Write(pSaveCtx->pIOCtx, (uint8_t*)pNDJoined+sizeof(DNODE), sizeof(NODEDATA_P_JOINED) - sizeof(DNODE) + pNDJoined->dwNumOfBytes);
		// restore data
		for(pCurNodeData=pBegNodeData; pCurNodeData<pEndNodeData; ++pCurNodeData) *pCurNodeData ^= XOR_DATA_MASK;

		RET_ON_FAIL;
		WRITE_ALIGN(pSaveCtx);
		error = TreeSaveNodeItself3(&pNDJoined->Node, pSaveCtx, dwCurLevel, dwSaveIteration);
		RET_ON_FAIL;
	}
	else
	{
		if (pNDJoined->dwNumOfBytes < 2)
		{
			DBG_STOP
		}
		
		if (pSaveCtx->sTreeInfo.dwHashLevel && dwCurLevel >= pSaveCtx->sTreeInfo.dwHashLevel)
		{
			dwSaveAsJoined = 0;
			dwSaveAsHashed = dwNumOfBytes;
		}
		else 
		{
			dwSaveAsJoined = min(dwNumOfBytes, pSaveCtx->sTreeInfo.dwHashLevel - dwCurLevel);
			dwSaveAsHashed = dwNumOfBytes - dwSaveAsJoined;
		}

		if (dwSaveAsJoined < 2 && dwCurLevel < pSaveCtx->sTreeInfo.dwHashLevel)
		{
			if (dwSaveAsHashed > 2)
			{
				dwSaveAsJoined += 2;
				dwSaveAsHashed -= 2;
			}
			else
			{
				dwSaveAsJoined += dwSaveAsHashed;
				dwSaveAsHashed = 0;
			}
		}

		if (dwSaveAsHashed <= 4)
		{
			dwSaveAsJoined += dwSaveAsHashed;
			dwSaveAsHashed = 0;
		}

		if (dwSaveAsHashed)
		{
			SPLIT_JOINED_NODE* pSplitNode;
			error = _HeapAlloc2(pSaveCtx->pRTCtx->pHeapCtx, &pSplitNode, sizeof(SPLIT_JOINED_NODE)+dwSaveAsJoined);
			RET_ON_FAIL;
			pSplitNode->Node.pCondition = pNode->pCondition;
			pSplitNode->Node.SignatureID = pNode->SignatureID;

			if (dwSaveAsJoined)
			{
				pSplitNode->Node.N.NodeType.NodeTypeFields.NodeType = NT_Dynamic;
				pSplitNode->Node.N.NodeType.NodeTypeFields.NodeDataType = NDT_P_Joined;
				pSplitNode->Node.NodeData.pNodeData = (uint8_t *)&pSplitNode->NodeDataJoined;
				pSplitNode->NodeDataJoined.dwNumOfBytes = dwSaveAsJoined;
				memcpy(pSplitNode->NodeDataJoined.NodeBytes, pNode->NodeData.pNodeDataJoined->NodeBytes, dwSaveAsJoined);
				pSplitNode->NodeDataJoined.Node.N.NodeType.NodeTypeFields.NodeType = NT_Dynamic;
				pSplitNode->NodeDataJoined.Node.N.NodeType.NodeTypeFields.NodeDataType = NDT_P_Hashed;
				pSplitNode->NodeDataJoined.Node.NodeData.pNodeDataHashed = &pSplitNode->NodeDataHashed;
			}
			else
			{
				pSplitNode->Node.N.NodeType.NodeTypeFields.NodeType = NT_Dynamic;
				pSplitNode->Node.N.NodeType.NodeTypeFields.NodeDataType = NDT_P_Hashed;
				pSplitNode->Node.NodeData.pNodeDataHashed = &pSplitNode->NodeDataHashed;
			}

			pSplitNode->NodeDataHashed.dwNumOfBytes = dwSaveAsHashed;
			pSplitNode->NodeDataHashed.dwHashVal = CountCRC32(dwSaveAsHashed, pNDJoined->NodeBytes + dwSaveAsJoined, 0);
			memcpy(&pSplitNode->NodeDataHashed.Node, &pNode->NodeData.pNodeDataJoined->Node, sizeof(DNODE));

			error = RegisterDelayedPointerEx(pSaveCtx, pNode, DT_SubstNode, (uint32_t) pSplitNode,0,0,0,0);
			RET_ON_FAIL;
			{
				// replace joined node in save array
				DYN_ARRAY* pDynArrayNodes;
				DNODE* pArrNode;
				if (TREE_FAILED(error = DynArrayGetItem(pSaveCtx->pDynArray, dwCurLevel, &pDynArrayNodes)))
					RET_ON_FAIL;
				if (TREE_FAILED(error = DynArrayPop(pDynArrayNodes, &pArrNode)))
					RET_ON_FAIL;
				if (pNode != pArrNode)
					DBG_STOP;
				pArrNode = &pSplitNode->Node;
				if (TREE_FAILED(error = DynArrayPush(pDynArrayNodes, &pArrNode)))
					RET_ON_FAIL;
			}
			if (dwSaveAsJoined)
				error = TreeSaveNodeBytes3(PDNODE (&pSplitNode->Node.NodeData.pNodeDataJoined->Node), pSaveCtx, dwCurLevel+dwSaveAsJoined, dwSaveIteration, pNDJoined->NodeBytes, dwSaveAsJoined);
			else
				error = TreeSaveNodeBytes3(PDNODE (&pSplitNode->Node.NodeData.pNodeDataHashed->Node), pSaveCtx, dwCurLevel+dwSaveAsHashed, dwSaveIteration, pNDJoined->NodeBytes, dwSaveAsHashed);
		}
		else
		{
			error = TreeSaveNodeBytes3(&pNDJoined->Node, pSaveCtx, dwCurLevel+pNDJoined->dwNumOfBytes, dwSaveIteration, pNDJoined->NodeBytes, pNDJoined->dwNumOfBytes);
		}
		RET_ON_FAIL;
	}

	return error;
}

// ------------------------------------------------------------------------------------------------

static TREE_ERR TreeSave_NDT_O_Index8(DNODE* pNode, SAVE_CTX* pSaveCtx, uint32_t dwCurLevel, uint32_t dwSaveIteration)
{

	TREE_ERR error = TREE_OK;
	uint32_t i;
	NODEDATA_P_INDEX32* pIndex = NULL;

	pIndex = pNode->NodeData.pNodeDataIndex;

	if (dwSaveIteration == cITERATION_SAVE)
	{
		uint32_t dwNodeOffset;
		uint32_t dwBitFieldMask = 0;
#ifdef _USE_TAGS
		_Write(pSaveCtx->pIOCtx, "<I8>", sizeof(uint32_t));
#endif
		UPDATE_DELAYED_NDT(pSaveCtx, pNode, NDT_O_Index8);
		RET_ON_FAIL;
//		ODS(("TreeSave_NDT_O_Index32: %d", TreeGetNodeChildrenCount(pNode) ));
//pSaveCtx->sSaveStatInfo.dwNodeDataIndexed++;
		error = UpdateDelayedPointerEx(pSaveCtx, pNode->NodeData.pNodeData, DT_NodeData, GetCurrOffset(pSaveCtx->pIOCtx));
		RET_ON_FAIL;

		dwNodeOffset = GetCurrOffset(pSaveCtx->pIOCtx);

		for (i=0; i<(sizeof(pIndex->Index)/sizeof(pIndex->Index[0]));i++)
		{
			uint8_t bNodeNumber=0;
			uint8_t bZero = 0x00;
			uint8_t bFill = 0xCC;
			dwBitFieldMask <<= 8;
			if (dwBitFieldMask == 0)
				dwBitFieldMask = 0xFF;
			if (pIndex->Index[i] != NULL)
			{
				bNodeNumber++;
				error = RegisterDelayedPointerEx(pSaveCtx, pIndex->Index[i], DT_Node, dwNodeOffset+(i&(~3)), dwBitFieldMask, 0xFFFFFFFF, dwNodeOffset+256-4, 4);
				RET_ON_FAIL;
				error = _Write(pSaveCtx->pIOCtx, &bFill, sizeof(bFill));
				RET_ON_FAIL;
			}
			else
			{
				error = _Write(pSaveCtx->pIOCtx, &bZero, sizeof(bZero));
				RET_ON_FAIL;
			}
		}


		for (i=0; i<(sizeof(pIndex->Index)/sizeof(pIndex->Index[0]));i++)
		{
			if (pIndex->Index[i] != NULL)
			{
				error = TreeSaveNodeItself3(pIndex->Index[i], pSaveCtx, dwCurLevel, dwSaveIteration);
				RET_ON_FAIL;
			}
		}
	}
	else
	{
		for (i=0; i<(sizeof(pIndex->Index)/sizeof(pIndex->Index[0]));i++)
		{
			if (pIndex->Index[i] != NULL)
			{
				uint8_t b = (uint8_t)i;
				error = TreeSaveNodeBytes3(pIndex->Index[i], pSaveCtx, dwCurLevel+1, dwSaveIteration, &b, 1);
				RET_ON_FAIL;
			}
		}
	}

	return error;
}

// ------------------------------------------------------------------------------------------------

static TREE_ERR TreeSave_NDT_O_Index32Fast(DNODE* pNode, SAVE_CTX* pSaveCtx, uint32_t dwCurLevel, uint32_t dwSaveIteration)
{

	TREE_ERR error = TREE_OK;
	uint32_t i;
	NODEDATA_P_INDEX32* pIndex = NULL;
		
	pIndex = pNode->NodeData.pNodeDataIndex;

	if (dwSaveIteration == cITERATION_SAVE)
	{
		uint32_t dwZero = 0;
#ifdef _USE_TAGS
		_Write(pSaveCtx->pIOCtx, "<IF>", sizeof(uint32_t));
#endif
//pSaveCtx->sSaveStatInfo.dwNodeDataIndexed++;
		
		UPDATE_DELAYED_NDT(pSaveCtx, pNode, NDT_O_Index32Fast);
		RET_ON_FAIL;
		error = UpdateDelayedPointerEx(pSaveCtx, pNode->NodeData.pNodeData, DT_NodeData, GetCurrOffset(pSaveCtx->pIOCtx));
		RET_ON_FAIL;

		for (i=0; i<(sizeof(pIndex->Index)/sizeof(pIndex->Index[0]));i++)
		{
			if (pIndex->Index[i] != NULL)
			{
				error = TreeSaveNodeItself3(pIndex->Index[i], pSaveCtx, dwCurLevel, dwSaveIteration);
				RET_ON_FAIL;
			}
			else
			{
				error = _Write(pSaveCtx->pIOCtx, &dwZero, sizeof(uint32_t));
				RET_ON_FAIL;
			}
		}
	}
	else
	{
		for (i=0; i<(sizeof(pIndex->Index)/sizeof(pIndex->Index[0]));i++)
		{
			if (pIndex->Index[i] != NULL)
			{
				uint8_t b = (uint8_t)i;
				error = TreeSaveNodeBytes3(pIndex->Index[i], pSaveCtx, dwCurLevel+1, dwSaveIteration, &b, 1);
				RET_ON_FAIL;
			}
		}
	}

	return error;
}

// ------------------------------------------------------------------------------------------------

static TREE_ERR TreeSave_NDT_O_Index32(DNODE* pNode, SAVE_CTX* pSaveCtx, uint32_t dwCurLevel, uint32_t dwSaveIteration)
{

	TREE_ERR error = TREE_OK;
	uint32_t i;
	NODEDATA_P_INDEX32* pIndex = NULL;
		
	pIndex = pNode->NodeData.pNodeDataIndex;

	if (dwSaveIteration == cITERATION_SAVE)
	{
		bool_t bUseOffsets = 0;
		uint32_t dwNodeBase = GetCurrOffset(pSaveCtx->pIOCtx);
		uint32_t dwNodesSize = 0;

		for (i=0; i<(sizeof(pIndex->Index)/sizeof(pIndex->Index[0]));i++)
		{
			if (pIndex->Index[i] != NULL)
			{
				uint32_t dwNodeSize;
				TreeGetNodeOptimalType(pSaveCtx, pIndex->Index[i], &dwNodeSize);
				dwNodesSize += dwNodeSize;
				if (dwNodeSize != sizeof(uint32_t))
					bUseOffsets = 1;
			}
		}
		if (TreeGetNodeChildrenCount (pNode) > 62 && bUseOffsets == 0)
			return TreeSave_NDT_O_Index32Fast(pNode, pSaveCtx, dwCurLevel, dwSaveIteration);
		if (dwNodesSize / 4 < 0xFF)
			return TreeSave_NDT_O_Index8(pNode, pSaveCtx, dwCurLevel, dwSaveIteration);

#ifdef _USE_TAGS
		_Write(pSaveCtx->pIOCtx, "<I>>", sizeof(uint32_t));
#endif
		UPDATE_DELAYED_NDT(pSaveCtx, pNode, NDT_O_Index32);
		RET_ON_FAIL;
//		ODS(("TreeSave_NDT_O_Index32: %d", TreeGetNodeChildrenCount(pNode) ));
//pSaveCtx->sSaveStatInfo.dwNodeDataIndexed++;
		error = UpdateDelayedPointerEx(pSaveCtx, pNode->NodeData.pNodeData, DT_NodeData, dwNodeBase);
		RET_ON_FAIL;

		for (i=0; i<(sizeof(pIndex->Index)/sizeof(pIndex->Index[0]));i++)
		{
			uint32_t dwData = (pIndex->Index[i] != NULL ? 0xCCCCCCCC : 0);
			if (pIndex->Index[i] != NULL)
			{
				error = RegisterDelayedPointerEx(pSaveCtx, pIndex->Index[i], DT_Node, dwNodeBase + i*sizeof(uint32_t), 0, 0, dwNodeBase+0x3FC, 4);
				RET_ON_FAIL;
			}
			error = _Write(pSaveCtx->pIOCtx, &dwData, sizeof(uint32_t));
			RET_ON_FAIL;
		}

		for (i=0; i<(sizeof(pIndex->Index)/sizeof(pIndex->Index[0]));i++)
		{
			if (pIndex->Index[i] != NULL)
			{
				error = TreeSaveNodeItself3(pIndex->Index[i], pSaveCtx, dwCurLevel, dwSaveIteration);
				RET_ON_FAIL;
			}
		}
	}
	else
	{
		for (i=0; i<(sizeof(pIndex->Index)/sizeof(pIndex->Index[0]));i++)
		{
			if (pIndex->Index[i] != NULL)
			{
				uint8_t b = (uint8_t)i;
				error = TreeSaveNodeBytes3(pIndex->Index[i], pSaveCtx, dwCurLevel+1, dwSaveIteration, &b, 1);
				RET_ON_FAIL;
			}
		}
	}

	return error;
}

// ------------------------------------------------------------------------------------------------

TREE_ERR TreeSaveNodeConditions3(DNODE* pNode, SAVE_CTX* pSaveCtx, uint32_t dwCurLevel, uint32_t dwSaveIteration, uint32_t dwNodeBase)
{
	TREE_ERR error = TREE_OK;
	DCONDITION* pCondition = NULL;
	
	if (dwSaveIteration == cITERATION_SAVE)
	{
		if (pNode->N.NodeType.NodeTypeFields.NodeType != NT_Dynamic)
			DBG_STOP;
		//pCondition = TreeGetNodeCondition(pNode);
		pCondition = pNode->pCondition;
		while (pCondition!=NULL)
		{
//	pSaveCtx->sSaveStatInfo.dwNodeConditions++;
			if (!GetDelayedPointer(pSaveCtx, pCondition, DT_OptimisedAnyByteCondition))
			{
				error = UpdateDelayedPointerEx(pSaveCtx, pCondition, DT_Condition, GetCurrOffset(pSaveCtx->pIOCtx));
				RET_ON_FAIL;

				if (pCondition->pNextCondition == NULL && 
                                    (((pCondition->C.ConditionType.nType == cCT_BitMaskAnd) && 
                                      (pCondition->C.Data.Byte1 == 0) && 
                                      (pCondition->C.Data.Byte2 == 0))|| 
                                     ((pCondition->C.ConditionType.nType == cCT_BitMaskOr) && 
                                      (pCondition->C.Data.Byte1 == 0xFF) && 
                                      (pCondition->C.Data.Byte2 == 0xFF))))
				{
					CONDITION_TYPE sConditionType;
					uint16_t nCounter = 1;
					uint8_t nType = cCT_AnyByte;
					DCONDITION* pChildCondition;
					DNODE* pSaveNode = &pCondition->Node;
					memset ( &sConditionType, 0, sizeof ( sConditionType ) );					
					pChildCondition = pCondition->Node.pCondition;
					while(pChildCondition)
					{
						if (pSaveNode->SignatureID || pSaveNode->NodeData.pNodeData) // save this node
						{
							if (pChildCondition->Node.pCondition)
								pChildCondition->Node.pCondition = pChildCondition->Node.pCondition;
							break;
						}
						while (pChildCondition->pNextCondition) // get last in list (AnyByte always last)
							pChildCondition = pChildCondition->pNextCondition;
						if (((pChildCondition->C.ConditionType.nType == cCT_BitMaskAnd) && 
                                                     (pChildCondition->C.Data.Byte1 == 0) && 
                                                     (pChildCondition->C.Data.Byte2 == 0)) || 
                                                    ((pChildCondition->C.ConditionType.nType == cCT_BitMaskOr) && 
                                                     (pChildCondition->C.Data.Byte1 == 0xFF) && 
                                                     (pChildCondition->C.Data.Byte2 == 0xFF)))
						{
							error = RegisterDelayedPointer(pSaveCtx, pChildCondition, DT_OptimisedAnyByteCondition, 1);
							RET_ON_FAIL;
							nCounter++; // skip 1 more byte
							pSaveNode = &pChildCondition->Node;
							pChildCondition = pChildCondition->Node.pCondition;
							continue;
						}
						break;
					}
					sConditionType.nType = nType;
#if defined (KL_NEED_ALIGNMENT)
					union {
					    uint16_t nWord;
					    uint8_t nBytes [2];
					} Converter;
					Converter.nWord = nCounter;
					sConditionType.nCounter [0] = Converter.nBytes [0];
					sConditionType.nCounter [1] = Converter.nBytes [1];
#else
					*((uint16_t*)sConditionType.nCounter) = nCounter;
#endif					
					error = _Write(pSaveCtx->pIOCtx, &sConditionType, sizeof(sConditionType));
					RET_ON_FAIL;

					error = TreeSaveNodeItself3(pSaveNode, pSaveCtx, dwCurLevel, dwSaveIteration);
					RET_ON_FAIL;
				}
				else
				{
					SCONDITION scond;
					memcpy (& scond.C, pCondition, sizeof (CONDITION));
					scond.C.Data.Unused [0] = 0;
					scond.C.Data.Unused [1] = 0;
					scond.dwNextConditionOffset = 0;

					error = RegisterDelayedPointerEx(pSaveCtx, pCondition->pNextCondition, DT_Condition, GetCurrOffset(pSaveCtx->pIOCtx) + GET_OFFSET(&scond, &scond.dwNextConditionOffset), 0, 0, dwNodeBase, 4);
					RET_ON_FAIL;

					error = _Write(pSaveCtx->pIOCtx, &scond, sizeof(SCONDITION) - sizeof (SNODE));
					RET_ON_FAIL;

					error = TreeSaveNodeItself3(&pCondition->Node, pSaveCtx, dwCurLevel, dwSaveIteration);
					RET_ON_FAIL;
				}
			}
			else
			{
				RemoveDelayedPointer(pSaveCtx, pCondition, DT_OptimisedAnyByteCondition);
			}

			pCondition = pCondition->pNextCondition;
		}
	}
	else
	{
		pCondition = pNode->pCondition;
		while (pCondition!=NULL)
		{
			NODE_BODY nb;

			switch (pCondition->C.ConditionType.nType)
			{
			case cCT_BitMaskAnd:
				nb.bCondType = cSCT_AND;
				break;
			case cCT_BitMaskOr:
				nb.bCondType = cSCT_OR;
				break;
			case cCT_Range:
				nb.bCondType = cSCT_RANGE;
				break;
			case cCT_AnyByte:
				nb.bCondType = cSCT_ANY;
				break;
			case cCT_Aster:
				nb.bCondType = cSCT_ASTER;
				break;
			default:
				DBG_STOP;
				return TREE_EUNEXPECTED;
			};

			nb.bCondByte1  = pCondition->C.Data.Byte1;
			nb.bCondByte2  = pCondition->C.Data.Byte2;
			nb.dwByteCount = 1;
			nb.pBytes = NULL;

			error = TreeSaveNode3(&(pCondition->Node), pSaveCtx, dwCurLevel+1, dwSaveIteration, &nb);
			RET_ON_FAIL;
			pCondition = pCondition->pNextCondition;
		}
	}

	return error;
}

// ------------------------------------------------------------------------------------------------

TREE_ERR TreeSaveNode3(DNODE* pNode, SAVE_CTX* pSaveCtx, uint32_t dwCurLevel, uint32_t dwSaveIteration, NODE_BODY *nb)
{
	TREE_ERR error = TREE_OK;
	uint32_t i, pos=0, dwNodeBase=0;
	DYN_ARRAY* pDynArrayCurLevelNodes;
	uint32_t dwBytesCount = 0;

	if (nb != 0)
	{
		dwBytesCount = nb->dwByteCount;
	}

	if (dwSaveIteration==cITERATION_BUILD_INDEX)
	{
		for (i=DynArrayGetCount(pSaveCtx->pDynArray); i<=dwCurLevel; i++)
		{
			DYN_ARRAY* pDynArray = NULL;
			if (TREE_FAILED(error = DynArraySetItem(pSaveCtx->pDynArray, i, &pDynArray)))
				return error;
		}

		if (TREE_FAILED(error = DynArrayGetItem(pSaveCtx->pDynArray, dwCurLevel, &pDynArrayCurLevelNodes)))
			return error;
		if (pDynArrayCurLevelNodes == NULL)
		{
			if (TREE_FAILED(error = CreateDynArray(pSaveCtx->pRTCtx->pHeapCtx, &pDynArrayCurLevelNodes, sizeof(NODE*))))
				return error;
			if (TREE_FAILED(error = DynArraySetItem(pSaveCtx->pDynArray, dwCurLevel, &pDynArrayCurLevelNodes)))
				return error;
		}
		if (TREE_FAILED(error = DynArrayAddItem(pDynArrayCurLevelNodes, &pNode)))
			return error;

		if (pSaveCtx->sTreeInfo.dwHashTableSize)
		{
			if (dwBytesCount+1 > dwCurLevel)
				DBG_STOP;

			pos = dwCurLevel-dwBytesCount-1;

			if (pos < FIRST_SIG_CHAR_COUNT && dwBytesCount > 0)
			{
				// determine actual number of signature symbols to copy
				uint32_t cnt = FIRST_SIG_CHAR_COUNT - pos;

				if (cnt > dwBytesCount)
					cnt = dwBytesCount;

				if (! nb)
				{
					DBG_STOP;
					return TREE_EUNEXPECTED;
				}
				else
				{
					// copy information from NODE_BODY to pSigChars
					for (i = 0; i < cnt; ++i)
					{
						SIG_CHAR_INFO *pCharInfo = &(pSaveCtx->pSigChars [pos + i]);

						pCharInfo->bCondType = nb->bCondType;

						if (nb->bCondType == cSCT_BYTE || nb->bCondType == cSCT_ANY)
						{
							if (nb->pBytes != 0)
							{
								pCharInfo->bCondByte2 = nb->pBytes [i];
							}
							else
							{
								pCharInfo->bCondByte2 = 0;
							}
						}
						else
						{
							pCharInfo->bCondByte1 = nb->bCondByte1;
							pCharInfo->bCondByte2 = nb->bCondByte2;
						}
					}
				}
			}
		}
	}

	if (dwSaveIteration == cITERATION_SAVE)
	{
		dwNodeBase = GetCurrOffset(pSaveCtx->pIOCtx);
		{
			DNODE* pSubstNode = GetDelayedPointer(pSaveCtx, pNode, DT_SubstNode);
			if (pSubstNode)
				pNode = pSubstNode;
		}
	}
	
	switch(pNode->N.NodeType.NodeTypeFields.NodeDataType)
	{
	case NDT_P_List:
		error = TreeSave_NDT_O_List(pNode, pSaveCtx, dwCurLevel, dwSaveIteration);
		break;
	case NDT_P_Index32:
		error = TreeSave_NDT_O_Index32(pNode, pSaveCtx, dwCurLevel, dwSaveIteration);
		break;
	case NDT_P_Joined:
		error = TreeSave_NDT_O_Joined32(pNode, pSaveCtx, dwCurLevel, dwSaveIteration);
		break;
	case NDT_P_JoinedRT:
		ODS(("TreeSaveNode2: Run-Time node in tree???"));
		DBG_STOP;
		break;
	case NDT_P_Hashed:
		error = TreeSave_NDT_O_Hashed(pNode, pSaveCtx, dwCurLevel, dwSaveIteration);
		break;
	case NDT_P_HashedRT:
		ODS(("TreeSaveNode2: Run-Time node in tree???"));
		DBG_STOP;
		break;
	default:
		ODSN(("TreeSaveNode2: Unsupported node type"));
		DBG_STOP;
		return TREE_EUNEXPECTED;
	}

	error = TreeSaveNodeConditions3(pNode, pSaveCtx, dwCurLevel, dwSaveIteration, dwNodeBase);

	if (dwSaveIteration == cITERATION_BUILD_INDEX && dwCurLevel>1)
	{
		if ((dwCurLevel - 1) < 4)
		{
			if (pNode->SignatureID)
			{
				// TODO: treat as signature with wildcarded tail
				pSaveCtx->sTreeInfo.dwHashTableSize = 0;
			}
//			else if (pNode->pCondition)
//			{
//				// condition in first 4 bytes - cannot use cache on this tree
//				// NOTE: this restriction has been removed
//				pSaveCtx->sTreeInfo.dwHashTableSize = 0;
//			}
		}

		if (pSaveCtx->sTreeInfo.dwHashTableSize)
		{
			if (pos<4 && pos+dwBytesCount>=4)
			{
				BYTE4 bytes;
				bytes.l = 0;
				error = TreeSetFilterByte (pSaveCtx, &bytes, 0);
				RET_ON_FAIL;
			}
		}
	}

	if (dwCurLevel==1 && dwSaveIteration==cITERATION_BUILD_INDEX && pSaveCtx->sTreeInfo.dwHashTableSize) // end of tree
	{
		// write bloom filter table
		error = _Write(pSaveCtx->pIOCtx, pSaveCtx->pCacheTable, pSaveCtx->sTreeInfo.dwHashTableSize);
		RET_ON_FAIL;
	}

	return error;
}

// -------------------------------------------------------------

static TREE_ERR TreeSetFilterByte (SAVE_CTX *pSaveCtx, BYTE4 *bytes, uint32_t idx)
{
	SIG_CHAR_INFO *pSigChars = pSaveCtx->pSigChars;
	unsigned int i;
	TREE_ERR error;

	if (idx < 4)
	{
		switch (pSigChars [idx].bCondType)
		{
		case cSCT_BYTE:
			bytes->b [idx] = pSigChars [idx].bCondByte2;
			return TreeSetFilterByte (pSaveCtx, bytes, idx + 1);

		case cSCT_ANY:
			for (i = 0; i < 256; ++i)
			{
				bytes->b [idx] = (uint8_t) i;
				error = TreeSetFilterByte (pSaveCtx, bytes, idx + 1);
				RET_ON_FAIL;
			}
			break;

		case cSCT_AND:
		case cSCT_OR: {
				uint8_t bits [8];
				unsigned int nbits = 0;
				unsigned int i, j, cnt;
				uint8_t mask = pSigChars [idx].bCondByte1;
				uint8_t val = pSigChars [idx].bCondByte2;

				if (pSigChars [idx].bCondType == cSCT_AND)
					mask = ~mask;

				for (i = 0; i < 8; ++i)
				{
					if ((mask & (1 << i)) != 0)
						bits [nbits++] = (1 << i);
				}

				cnt = 1 << nbits;
				for (i = 0; i < cnt; ++i)
				{
					uint8_t b = val & ~mask;
					for (j = 0; j < nbits; ++j)
					{
						if ((i & (1 << j)) != 0)
							b |= bits [j];
					}
					bytes->b [idx] = b;
					error = TreeSetFilterByte (pSaveCtx, bytes, idx + 1);
					RET_ON_FAIL;
				}
			} break;

		case cSCT_RANGE:
			for (i = pSigChars [idx].bCondByte1; i <= pSigChars [idx].bCondByte2; ++i)
			{
				bytes->b [idx] = (uint8_t) i;
				error = TreeSetFilterByte (pSaveCtx, bytes, idx + 1);
				RET_ON_FAIL;
			}
			break;

		default:
			DBG_STOP;
			return TREE_EUNEXPECTED;
		}
	}
	else
	{
		uint16_t wIndex;
		uint8_t bValue;
		uint8_t bMask;

		bValue = bytes->b [3] & 0x07;
		wIndex = (bytes->w[0] + bytes->w[1]) & (uint16_t)(pSaveCtx->sTreeInfo.dwHashTableSize - 1);

		bMask = (uint8_t)(1 << bValue);

		pSaveCtx->pCacheTable[wIndex] |= bMask;
	}

	return TREE_OK;
}

// -------------------------------------------------------------

TREE_ERR _TREE_CALL TreeSave_Static(sRT_CTX* pRTCtx, sIO_CTX* pIOCtx, uint32_t dwHashLevel, uint32_t dwHashTableSizeKb)
{
	TREE_ERR error = TREE_OK;
	SAVE_CTX* pSaveCtx = NULL;
	DNODE* pRoot = PDNODE(pRTCtx->pRoot);
	uint32_t dwStartOffset = GetCurrOffset(pIOCtx);
	uint32_t i;

	if (pRTCtx == NULL)
		return TREE_EINVAL;

	// check dwHashTableSizeKb to be power of 2 <= 64
	if (dwHashTableSizeKb != 0)
	{
		if (dwHashTableSizeKb > 64 || (dwHashTableSizeKb & (dwHashTableSizeKb - 1)) != 0)
			return TREE_EINVAL;
	}

	if (pRTCtx->sTreeInfo.dwMagic != MAGIC_TREE_DB_V2)
		return TREE_ECORRUPT;
	if (pRTCtx->sTreeInfo.dwFormat == cFORMAT_STATIC)
	{
		error = _Write(pIOCtx, pRTCtx->pTreeBase, pRTCtx->sTreeInfo.dwTotalSize);
		RET_ON_FAIL;
		return error;
	}
	if (pRTCtx->sTreeInfo.dwFormat != cFORMAT_DYNAMIC)
		return TREE_ECORRUPT;

	if (TREE_FAILED(error = _HeapAlloc2(pRTCtx->pHeapCtx, &pSaveCtx, sizeof(SAVE_CTX))))
		RET_ON_FAIL;

	ZeroMemory(&pSaveCtx->sTreeInfo, sizeof(pSaveCtx->sTreeInfo));
	pSaveCtx->sTreeInfo.dwMagic = MAGIC_TREE_DB_V2;
	pSaveCtx->sTreeInfo.dwFormat = cFORMAT_STATIC;
	pSaveCtx->sTreeInfo.dwHashTableSize = 1024 * dwHashTableSizeKb;
	if (dwHashLevel)
		pSaveCtx->sTreeInfo.dwHashLevel = dwHashLevel+1; // +1
	pSaveCtx->sTreeInfo.MinSignatureID = pRTCtx->sTreeInfo.MinSignatureID;
	pSaveCtx->sTreeInfo.MaxSignatureID = pRTCtx->sTreeInfo.MaxSignatureID;


	if (TREE_FAILED(error = CreateDynArray(pRTCtx->pHeapCtx, &pSaveCtx->pDynArray, sizeof(DYN_ARRAY*))))
		return error;

	pSaveCtx->pIOCtx = pIOCtx;
	pSaveCtx->pRTCtx = pRTCtx;
	if (pSaveCtx->sTreeInfo.dwHashTableSize)
		error = _HeapAlloc2(pRTCtx->pHeapCtx, &pSaveCtx->pCacheTable, pSaveCtx->sTreeInfo.dwHashTableSize);
	RET_ON_FAIL;
	InitDelayedPointers(pSaveCtx);
	
	error = _Write(pIOCtx, &pSaveCtx->sTreeInfo, sizeof(pSaveCtx->sTreeInfo));

	pSaveCtx->dwNodesToSave = 0;
	if (TREE_SUCCEEDED(error))
		error = TreeSaveNode3(pRoot, pSaveCtx, 1, cITERATION_BUILD_INDEX, NULL);

	if (TREE_SUCCEEDED(error))
		error = TreeSaveNodeItself3(pRoot, pSaveCtx, 1, cITERATION_SAVE);
	
	if (TREE_SUCCEEDED(error))
	{
		uint32_t dwItems = DynArrayGetCount(pSaveCtx->pDynArray);
		uint32_t dwLevelStartOffset;
		for (i=0; i<dwItems; i++)
		{
			uint32_t j;
			DYN_ARRAY* pDynArrayNodes;
			uint32_t dwLevelDataSize;
			DNODE* pNode; // UNINITIALIZED???
			uint32_t dwItems;
			dwLevelStartOffset = GetCurrOffset(pSaveCtx->pIOCtx);
			if (TREE_FAILED(error = DynArrayGetItem(pSaveCtx->pDynArray, i, &pDynArrayNodes)))
				break;
			if (pDynArrayNodes) // level may be empty
			{
				dwItems = DynArrayGetCount(pDynArrayNodes);
				//ppNode = (NODE**)pDynArrayNodes->pData;
				for (j=0; j<dwItems; j++)
				{
					if (TREE_FAILED(error = DynArrayGetItem(pDynArrayNodes, j, &pNode)))
						break;
					//error = TreeSaveNode3(*ppNode, pSaveCtx, 1, cITERATION_SAVE, NULL, 0);
					error = TreeSaveNode3(pNode, pSaveCtx, 1, cITERATION_SAVE, NULL);
					if (TREE_FAILED(error))
						break;
					//ppNode++;
				}
			}
			else
			{
				pDynArrayNodes = pDynArrayNodes;
			}
			dwLevelDataSize = GetCurrOffset(pSaveCtx->pIOCtx) - dwLevelStartOffset;
		}
	}

	// free resources
	if (pSaveCtx->pCacheTable)
		_HeapFree(pRTCtx->pHeapCtx, pSaveCtx->pCacheTable);

	for (i=0; i<DynArrayGetCount(pSaveCtx->pDynArray); i++)
	{
		DYN_ARRAY* pDynArrayNodes;
		if (TREE_FAILED(DynArrayGetItem(pSaveCtx->pDynArray, i, &pDynArrayNodes)))
			break;
		if (TREE_FAILED(DestroyDynArray(&pDynArrayNodes)))
			break;
	}
	DestroyDynArray(&pSaveCtx->pDynArray);

	if (TREE_SUCCEEDED(error))
	{
		pSaveCtx->sTreeInfo.dwTotalSize = GetCurrOffset(pIOCtx) - dwStartOffset;
		error = _SeekWrite(pIOCtx, dwStartOffset, &pSaveCtx->sTreeInfo, sizeof(pSaveCtx->sTreeInfo));
	}
	
	if (TREE_FAILED(error)) // restore pos
		pIOCtx->Seek(pIOCtx->pIO, NULL, dwStartOffset, TREE_IO_CTX_SEEK_SET);

	DeinitDelayedPointers(pSaveCtx);
	_HeapFree(pRTCtx->pHeapCtx, pSaveCtx);

	return error;
}

// ------------------------------------------------------------------------------------------------

