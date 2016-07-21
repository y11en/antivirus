// search.c
//

// actually the same context data format
#define tag_sSEARCHER_RT_CTX tag_sRT_CTX

#include "common.h"

#include "static.c"

#ifdef _TREE_DEBUG
#include "string.h"

#define SET_NAME(name) {name, #name},
const Names NodeDataTypeNames[] = 
{
	SET_NAME(NDT_Empty)

	SET_NAME(NDT_P_Index32)
	SET_NAME(NDT_P_List)
	SET_NAME(NDT_P_Joined)
	SET_NAME(NDT_P_JoinedRT)
	SET_NAME(NDT_P_Hashed)
	SET_NAME(NDT_P_HashedRT)

	SET_NAME(NDT_O_Index32)
	SET_NAME(NDT_O_Index32Fast)
	SET_NAME(NDT_O_List) 
	SET_NAME(NDT_O_Joined32)
	SET_NAME(NDT_O_JoinedRT32)
	SET_NAME(NDT_O_Joined8)
	SET_NAME(NDT_O_JoinedRT8)
	SET_NAME(NDT_O_Hashed)
	SET_NAME(NDT_O_HashedRT)
	SET_NAME(NDT_O_AnyByte)
	SET_NAME(NDT_O_AnyByteRT)
	
	SET_NAME(NDT_O_Array)
	SET_NAME(NDT_O_ArrayFast)
	SET_NAME(NDT_O_Index8)
	SET_NAME(NDT_O_Tail)
	
	SET_NAME(NDT_InvalidType)
};
#undef SET_NAME

#define SET_NAME(name) {name, #name},
const Names NodeTypeNames[] = {
	SET_NAME(NT_Full)
	SET_NAME(NT_S)
	SET_NAME(NT_D)
	SET_NAME(NT_SD)
	SET_NAME(NT_C)
};
#undef SET_NAME



#define DbgSearchName(names, id) DbgSearchNameEx((Names*)&names, sizeof(names)/sizeof(names[0]), id)
const char * DbgSearchNameEx(Names* pNames, uint32_t dwCount, uint32_t id)
{
	uint32_t i;
	for (i=0; i<dwCount; i++)
	{
		if (pNames[i].id == id)
			return pNames[i].name;
	}
	return "<undefined>";
}

char * _TREE_CALL DbgNodeType(uint32_t dwFullNodeType)
{
	NODE* pNode = (NODE*)&dwFullNodeType;
	static char szText[0x100];
	uint32_t ndt;
	if (pNode == NULL)
	{
		strcpy(szText, "<NULL>");
		return szText;
	}
	szText[0] = 0;
	ndt = TreeGetNodeDataType(pNode);
	strcat(szText, DbgSearchName(NodeTypeNames, pNode->NodeType.NodeTypeFields.NodeType));
	strcat(szText, " ");
	strcat(szText, DbgSearchName(NodeDataTypeNames, ndt));
	return szText;
}

char* _TREE_CALL DbgNodeInfo(sRT_CTX* pRTCtx, NODE* pNode)
{
	char* szText;

	if (!pNode)
		return "<NULL>";

	szText = DbgNodeType(pNode->NodeType.dwFullNodeType);

	switch(TreeGetNodeDataType(pNode))
	{
	case NDT_O_HashedRT:
	case NDT_O_JoinedRT8:
	case NDT_O_JoinedRT32:
		break;
	default:
		sprintf(szText + strlen(szText), " node=%08x", (uint8_t*)pNode - pRTCtx->pTreeBase);
		if (TreeGetNodeCondition(pNode))
			sprintf(szText + strlen(szText), " cond=%08x", (uint8_t*)TreeGetNodeCondition(pNode) - pRTCtx->pTreeBase);
		if (TreeGetNodeData(pNode))
			sprintf(szText + strlen(szText), " data=%08x", (uint8_t*)pNode + (TreeGetNodeData(pNode) - pRTCtx->pTreeBase));
	}
	return szText;
}

#endif // _TREE_DEBUG

// ------------------------------------------------------------------------------------------------

TREE_STATIC
_INLINE NODE* TreeGetNextNode(NODE* pNode, uint8_t NodeByte, sRT_CTX* pRTCtx)
{
	NODE* pNewNode;
	switch(pNode->NodeType.NodeTypeFields.NodeDataType)
	{
//	case NDT_P_Index32:
//		pNewNode = TreeGetNextNodeIndexed(pNode, NodeByte, pRTCtx);
//		break;
//	case NDT_P_List:
//		pNewNode = TreeGetNextNodeList(pNode, NodeByte, pRTCtx);
//		break;
//	case NDT_P_Joined:
//		pNewNode = TreeGetNextNodeJoined(pNode, NodeByte, pRTCtx);
//		break;
//	case NDT_P_JoinedRT:
//		pNewNode = TreeGetNextNodeJoinedRT(pNode, NodeByte, pRTCtx);
//		break;
//	case NDT_P_Hashed:
//		pNewNode = TreeGetNextNodeHashed(pNode, NodeByte, pRTCtx);
//		break;
//	case NDT_P_HashedRT:
//		pNewNode = TreeGetNextNodeHashedRT(pNode, NodeByte, pRTCtx);
//		break;

//	case NDT_O_Index32:
//		pNewNode = TreeGetNextNode_O_Index32(pNode, NodeByte, pRTCtx);
//		break;
//	case NDT_O_Index32Fast:
//		pNewNode = TreeGetNextNode_O_Index32Fast(pNode, NodeByte, pRTCtx);
//		break;
//	case NDT_O_List:
//		pNewNode = TreeGetNextNode_O_List(pNode, NodeByte, pRTCtx);
//		break;
//	case NDT_O_Joined32:
//		pNewNode = TreeGetNextNode_O_Joined32(pNode, NodeByte, pRTCtx);
//		break;
//	case NDT_O_JoinedRT32:
//		pNewNode = TreeGetNextNode_O_JoinedRT32(pNode, NodeByte, pRTCtx);
//		break;
//	case NDT_O_JoinedRT8:
//		pNewNode = TreeGetNextNode_O_JoinedRT8(pNode, NodeByte, pRTCtx);
//		break;
//	case NDT_O_Hashed:
//		pNewNode = TreeGetNextNode_O_Hashed(pNode, NodeByte, pRTCtx);
//		break;
//	case NDT_O_HashedRT:
//		pNewNode = TreeGetNextNode_O_HashedRT(pNode, NodeByte, pRTCtx);
//		break;
////	case NDT_O_AnyByte:
////		pNewNode = TreeGetNextNode_O_AnyByte(pNode, NodeByte, pRTCtx);
////		break;
//	case NDT_O_AnyByteRT:
//		pNewNode = TreeGetNextNode_O_AnyByteRT(pNode, NodeByte, pRTCtx);
//		break;
//
//	case NDT_O_Array:
//		pNewNode = TreeGetNextNode_O_Array(pNode, NodeByte, pRTCtx);
//		break;
//	case NDT_O_ArrayFast:
//		pNewNode = TreeGetNextNode_O_ArrayFast(pNode, NodeByte, pRTCtx);
//		break;
//	case NDT_O_Index8:
//		pNewNode = TreeGetNextNode_O_Index8(pNode, NodeByte, pRTCtx);
//		break;
//	case NDT_O_Tail:
//		pNewNode = TreeGetNextNode_O_Tail(pNode, NodeByte, pRTCtx);
//		break;

		
	default:
//		if ((uint32_t)pNode+0x20-(uint32_t)pRTCtx->pTreeBase == 0xe810)
//			DBG_STOP;
		pNewNode = pRTCtx->TreeGetNextNodeXXX[pNode->NodeType.NodeTypeFields.NodeDataType](pNode, NodeByte, pRTCtx);
#ifdef _TREE_DEBUG
		// validate node
		if (pNewNode)
		{
			if (pNewNode->NodeType.NodeTypeFields.NodeType == NT_Empty)
				DBG_STOP;
			if (pNewNode->NodeType.NodeTypeFields.NodeDataType==NDT_Empty  || pNewNode->NodeType.NodeTypeFields.NodeDataType>NDT_O_Tail)
				DBG_STOP;
		}
#endif
//		if ((uint32_t)pNewNode+0x20-(uint32_t)pRTCtx->pTreeBase == 0x5ab4)
//			DBG_STOP;
	}
	return pNewNode;
}

// ------------------------------------------------------------------------------------------------

TREE_STATIC
NODE* TreeGetNextNodeUnsupported(NODE* pNode, uint8_t NodeByte, sRT_CTX* pRTCtx)
{
	ODSN(("TreeGetNextNodeUnsupported: Unsupported node type"));
	DBG_STOP;
	return NULL;
}

// ------------------------------------------------------------------------------------------------

TREE_STATIC
_INLINE TREE_ERR AddSpinner(sRT_CTX* pRTCtx, SPINNERS_DATA* pSpinners, NODE* pNode, uint64_t qwSignatureStartPosLinear)
{
	TREE_ERR error = TREE_OK;
	uint32_t nInUse = pSpinners->nInUse;
	SPINNER* pSpinner;

	if (pSpinners->nAllocated <= nInUse+1)
	{
		pSpinners->nAllocated += 0x10;
		
		if (pSpinners->Spinner == NULL)
			error = _HeapAlloc2(pRTCtx->pHeapCtx, (void**)&pSpinners->Spinner, pSpinners->nAllocated * sizeof(SPINNER));
		else
			error = _HeapReAlloc2(pRTCtx->pHeapCtx, (void**)&pSpinners->Spinner, pSpinners->Spinner, pSpinners->nAllocated * sizeof(SPINNER));
		if (TREE_FAILED(error))
		{
			pSpinners->nAllocated -= 0x10;
			return error;
		}

#ifdef _TREE_DEBUG
		{
			unsigned int i;
			for (i=nInUse; i<pSpinners->nAllocated; i++)
				pSpinners->Spinner[i].pNode = NULL;
		}
#endif

	}
	
	pSpinner = &pSpinners->Spinner[nInUse];
	pSpinner->pNode = pNode;
	pSpinner->qwSignatureStartPosLinear = qwSignatureStartPosLinear;
	pSpinners->nInUse++;
	return TREE_OK;
}

// ------------------------------------------------------------------------------------------------

TREE_STATIC
_INLINE TREE_ERR MoveSpinners(sRT_CTX* pRTCtx, SPINNERS_DATA* pSpinnersDst, SPINNERS_DATA* pSpinnersSrc)
{
	TREE_ERR error = TREE_OK;
	uint32_t i;
	if (pSpinnersSrc->nInUse > 0)
	{
		if (pSpinnersDst->nAllocated > pSpinnersDst->nInUse + pSpinnersSrc->nInUse)
		{
			memcpy(&pSpinnersDst->Spinner[pSpinnersDst->nInUse], &pSpinnersSrc->Spinner[0], pSpinnersSrc->nInUse*sizeof(SPINNER));
			pSpinnersDst->nInUse += pSpinnersSrc->nInUse;
		}
		else
		{
			for (i=0; i<pSpinnersSrc->nInUse; i++)
			{
				if (TREE_FAILED(error = AddSpinner(pRTCtx, pSpinnersDst, pSpinnersSrc->Spinner[i].pNode, pSpinnersSrc->Spinner[i].qwSignatureStartPosLinear)))
					return error;
#ifdef _TREE_DEBUG
				pSpinnersSrc->Spinner[i].pNode = NULL;
#endif
			}
		}
		pSpinnersSrc->nInUse = 0;
	}
	return TREE_OK;
}

// ------------------------------------------------------------------------------------------------

TREE_STATIC
_INLINE TREE_ERR FreeAllSpinners(sRT_CTX* pRTCtx, SPINNERS_DATA* pSpinners)
{
	uint32_t i;
	if (pSpinners->Spinner == NULL)
		return TREE_EUNEXPECTED;
	for (i=0; i<pSpinners->nInUse; i++)
	{
		NODE* pNode = pSpinners->Spinner[i].pNode;
		while (pNode)
			pNode = TreeGetNextNode(pNode, 0, pRTCtx);
	}
	_HeapFree(pRTCtx->pHeapCtx, pSpinners->Spinner);
	ZeroMemory(pSpinners, sizeof(SPINNERS_DATA));
	return TREE_OK;
}

// ------------------------------------------------------------------------------------------------

TREE_STATIC
_INLINE bool_t TreeReportSignatureFound(tTreeSignatureFoundCallbackFunc TreeSignatureFoundCallbackFunc, CONST void* pCallbackCtx, sRT_CTX* pRTCtx, SPINNER* pSpinner, tSIGNATURE_ID SignatureID)
{
	uint32_t dwSearcherCurrentPositionInBuffer;
	uint64_t qwSearcherCurrentPositionLinear;
	
	uint32_t dwSignatureStartPositionInBuffer;
	//uint64_t qwSignatureStartPositionInObject;
	uint32_t dwSignatureLength;

	//ODS(("Signature #%08X found at 0x%08X", pNode->SignatureID, dwPos));

	dwSearcherCurrentPositionInBuffer = pRTCtx->dwSearcherCurrentPositionInBuffer + 1;
	qwSearcherCurrentPositionLinear = pRTCtx->qwBufferStartPositionLinear + dwSearcherCurrentPositionInBuffer;
	
	dwSignatureLength = (uint32_t)(qwSearcherCurrentPositionLinear - pSpinner->qwSignatureStartPosLinear);
	dwSignatureStartPositionInBuffer = dwSearcherCurrentPositionInBuffer - dwSignatureLength;
	//qwSignatureStartPositionInObject = pRTCtx->qwLinearZeroPositionInObject + pSpinner->qwSignatureStartPosLinear;
	
	if (TREE_ECANCEL == TreeSignatureFoundCallbackFunc(pCallbackCtx, SignatureID, dwSignatureStartPositionInBuffer, pSpinner->qwSignatureStartPosLinear, dwSignatureLength))
		return 1;
	return 0;
}

TREE_STATIC
_INLINE tSIGNATURE_ID TreeGetNodeSignatureID(NODE* pNode)
{
	switch (pNode->NodeType.NodeTypeFields.NodeType)
	{
	case NT_Full:
		return PSNODE(pNode)->SignatureID;
	case NT_S:
		return ((SNODE_S*)pNode)->NodeType.SignatureID;
	case NT_SD:
		return ((SNODE_SD*)pNode)->SignatureID;
	case NT_Dynamic:
		return PDNODE (pNode)->SignatureID;
	default:
		break;
	}
	return 0;
}

//TREE_STATIC
//uint32_t TreeGetNodeDataType(NODE* pNode)
//{
//   return pNode->NodeType.NodeTypeFields.NodeDataType;
//}

TREE_STATIC
_INLINE
uint8_t * TreeGetNodeData (NODE* pNode)
{
	switch (pNode->NodeType.NodeTypeFields.NodeType)
	{
	case NT_Dynamic:
		return PDNODE(pNode)->NodeData.pNodeData;
	case NT_Full:
		return ((uint8_t *)pNode) + PSNODE(pNode)->dwNodeDataOffset * 4;
	case NT_D:
		return ((uint8_t *)pNode) + pNode->NodeType.NodeTypeFields.NodeData * 4;
	case NT_SD:
		return ((uint8_t *)pNode) + pNode->NodeType.NodeTypeFields.NodeData * 4;
	case NT_S:
		return 0;
	case NT_C:
		return 0;
	default:
		DBG_STOP;
		return 0;
	}
}

TREE_STATIC
_INLINE
CONDITION* TreeGetNodeCondition(NODE* pNode)
{
	switch (pNode->NodeType.NodeTypeFields.NodeType)
	{
	case NT_Dynamic:
		return (CONDITION *)(PDNODE (pNode)->pCondition);
	case NT_Full:
		if (PSNODE (pNode)->dwConditionOffset != 0)
			return (CONDITION *)((uint8_t*)pNode + PSNODE (pNode)->dwConditionOffset * 4);
		break;
	case NT_C: // only for static tree
		return (CONDITION*)((uint8_t*)pNode + pNode->NodeType.NodeTypeFields.NodeData * 4);
	default:
		break;
	}
	return 0;
}

TREE_STATIC
_INLINE NODE* TreeGetConditionNode(NODE* pNode, CONDITION* pCond)
{
	if (NODE_TYPE (pNode) == NT_Dynamic)
		return PNODE (& (PDCOND (pCond)->Node));
	else
		return PNODE (& (PSCOND (pCond)->Node));
}

TREE_STATIC
_INLINE CONDITION* TreeGetNextNodeCondition(NODE* pNode, CONDITION* pCond)
{
	if (pCond->ConditionType.nType == cCT_AnyByte)
		return NULL;

	switch (pNode->NodeType.NodeTypeFields.NodeType)
	{
	case NT_Dynamic:
		return (CONDITION *) (PDCOND (pCond)->pNextCondition);
	case NT_Full:
    case NT_C:
		if (PSCOND(pCond)->dwNextConditionOffset != 0)
			return (CONDITION *) ((uint8_t *) (& PSCOND(pCond)->Node) + TreeGetNodeSize(& PSCOND(pCond)->Node.N));
		break;
    default:
	    break;
    }
    return NULL;
}

TREE_STATIC
_INLINE uint32_t TreeGetNodeSize(NODE* pNode)
{
	switch (pNode->NodeType.NodeTypeFields.NodeType)
	{
	case NT_Full:
		return sizeof(SNODE);
	case NT_S:
		return sizeof(SNODE_S);
	case NT_D:
		return sizeof(SNODE_D);
	case NT_SD:
		return sizeof(SNODE_SD);
	case NT_C:
		return sizeof(SNODE_C);
	default:
		DBG_STOP;
		return 0;
	}
};

TREE_STATIC
TREE_ERR _FORCEINLINE iTreeSearchData(sSEARCHER_RT_CTX* pRTCtx,
					  CONST uint8_t* pbData, // buffer to search in
					  uint32_t dwSize,     // data size in buffer
					  uint32_t dwStartPos, // start position for search
					  uint32_t dwEndPos,   // end position for search (signatures beginning before ulEndPos and continues after ulEndPos will be found)
					  tTreeSignatureFoundCallbackFunc TreeSignatureFoundCallbackFunc, // callback function
					  CONST void* pCallbackCtx) // callback context
{
	TREE_ERR error;
	unsigned int spinner;
	uint16_t wHashTableANDMask = 0;

	if (dwEndPos - dwStartPos >= 4)
	{
		wHashTableANDMask = pRTCtx->wHashTableANDMask;
	}

	for (pRTCtx->dwSearcherCurrentPositionInBuffer=dwStartPos; pRTCtx->dwSearcherCurrentPositionInBuffer<dwSize; pRTCtx->dwSearcherCurrentPositionInBuffer++)
	{
		// condition: run new spinner
		if (pRTCtx->dwSearcherCurrentPositionInBuffer <= dwEndPos) do
		{
#ifdef _TREE_DEBUG
			pRTCtx->dwStatBytesTotal++;
#endif
			// condition: check mask
			if (wHashTableANDMask && pRTCtx->dwSearcherCurrentPositionInBuffer <= dwEndPos - 4)
			{
				// check if there is any signature starting with given 4 bytes
#if defined (KL_NEED_ALIGNMENT)
				uint16_t indexBytes1 = 0;
				uint16_t indexBytes2 = 0;
				unsigned char* _indexBytes1 = (unsigned char*)&indexBytes1;
				unsigned char* _indexBytes2 = (unsigned char*)&indexBytes2;
				uint16_t indexWord;
				uint8_t  maskByte;
				_indexBytes1[0] = *(unsigned char*)(pbData + pRTCtx->dwSearcherCurrentPositionInBuffer);
				_indexBytes1[1] = *(unsigned char*)(pbData + pRTCtx->dwSearcherCurrentPositionInBuffer+1);
				_indexBytes2[0] = *(unsigned char*)(pbData + pRTCtx->dwSearcherCurrentPositionInBuffer+2);
				_indexBytes2[1] = *(unsigned char*)(pbData + pRTCtx->dwSearcherCurrentPositionInBuffer+3);
				indexWord = (const uint16_t)(indexBytes1 + indexBytes2) & pRTCtx->wHashTableANDMask;
				maskByte = pbData[pRTCtx->dwSearcherCurrentPositionInBuffer+3];
#else
				const uint16_t indexBytes1 = *(const uint16_t *)(pbData + pRTCtx->dwSearcherCurrentPositionInBuffer);
				const uint16_t indexBytes2 = *(const uint16_t *)(pbData + pRTCtx->dwSearcherCurrentPositionInBuffer+2);
				const uint16_t indexWord = (const uint16_t)(indexBytes1 + indexBytes2) & pRTCtx->wHashTableANDMask;
				const uint8_t  maskByte = pbData[pRTCtx->dwSearcherCurrentPositionInBuffer+3];
#endif
                               
				if (((pRTCtx->pTreeHashTable)[indexWord] & pRTCtx->bBitMask[maskByte & 0x07U]) == 0)
					break; // from fake 'do {...} while (0)'
#ifdef _TREE_DEBUG
				else
					pRTCtx->FilterMap[indexWord]++;
#endif
			}

			if (TREE_FAILED(error = AddSpinner(pRTCtx, &pRTCtx->sSpinners, pRTCtx->pRoot, pRTCtx->qwBufferStartPositionLinear + pRTCtx->dwSearcherCurrentPositionInBuffer)))
				return error;
#ifdef _TREE_DEBUG
			pRTCtx->dwStatSpinnersUsed++;
#endif // _TREE_DEBUG
		} while (0);
		else
		{
			// we are at termination point (endPos or endPos - 4)
			if (pRTCtx->sSpinners.nInUse == 0)
				break;
		}

		if (pRTCtx->sSpinners.nInUse == 0)
			continue;
		
		for (spinner=0; spinner<pRTCtx->sSpinners.nInUse; spinner++)
		{
			CONDITION* pCondition;
			SPINNER* pCurrentSpinner = &pRTCtx->sSpinners.Spinner[spinner];	
			SPINNER  sCurrentSpinner;

			pRTCtx->pCurrentSpinner = pCurrentSpinner;
			memcpy(&sCurrentSpinner, pCurrentSpinner, sizeof(SPINNER));

#ifdef _TREE_DEBUG
			if (sCurrentSpinner.pNode == NULL)
				ODS(("Opps: pRTCtx->sSpinners.Spinner[spinner].pNode == NULL"));
#endif
			pCondition = TreeGetNodeCondition(sCurrentSpinner.pNode);
			while (pCondition != NULL)
			{
				bool_t bConditionResult = 0;
				tSIGNATURE_ID SignatureID;
				NODE* pConditionNode = TreeGetConditionNode (sCurrentSpinner.pNode, pCondition);
				CONDITION* pNextCondition = TreeGetNextNodeCondition (sCurrentSpinner.pNode, pCondition);
				switch(pCondition->ConditionType.nType)
				{
				case cCT_BitMaskAnd:
					bConditionResult = ((pbData[pRTCtx->dwSearcherCurrentPositionInBuffer] & pCondition->Data.Byte1) == pCondition->Data.Byte2);
					break;
				case cCT_BitMaskOr:
					bConditionResult = ((pbData[pRTCtx->dwSearcherCurrentPositionInBuffer] | pCondition->Data.Byte1) == pCondition->Data.Byte2);
					break;
				case cCT_Range:
					bConditionResult = (pbData[pRTCtx->dwSearcherCurrentPositionInBuffer] >= pCondition->Data.Byte1 && pbData[pRTCtx->dwSearcherCurrentPositionInBuffer] <= pCondition->Data.Byte2);
					break;
				case cCT_AnyByte:
					{
						NODE* pNextNode = (NODE*)((uint8_t*)pCondition + 4);
						NODEDATA_O_ANYBYTE_RT *pNodeDataAnyByteRT = 0;
#if defined (KL_NEED_ALIGNMENT)
						union {
						    uint16_t nWord;
						    uint8_t nBytes [2];
						} Converter;
						uint16_t dwNumOfBytes;
						Converter.nBytes [0] = pCondition->ConditionType.nCounter [0];
						Converter.nBytes [1] = pCondition->ConditionType.nCounter [1];
						dwNumOfBytes = Converter.nWord;
#else
						uint16_t dwNumOfBytes = *((uint16_t*)pCondition->ConditionType.nCounter);
#endif
						pNextCondition = NULL;
						bConditionResult = 1;

						if (dwNumOfBytes <= 1)
						{
							pConditionNode = pNextNode;
							break;
						}
						if (TREE_FAILED(error = AllocRTNode(pRTCtx, (DNODE **) &pConditionNode, (RT_NODE_DATA **)(&pNodeDataAnyByteRT), NDT_O_AnyByteRT)))
						{
							DBG_STOP;
							return error;
						}

						pNodeDataAnyByteRT->dwNumOfBytes = dwNumOfBytes;
						pNodeDataAnyByteRT->dwCurrentByte = 1;
						pNodeDataAnyByteRT->pNextNode = pNextNode;
						break;
					}
				case cCT_Aster:
					{
						DNODE* pRTNode = 0;
						NODEDATA_ASTER_RT *pNodeDataAsterRT = 0;
						uint32_t dwNumOfBytes = pCondition->Data.Byte1 + (pCondition->Data.Byte2 << 8);
						if (TREE_FAILED(error = AddSpinner(pRTCtx, &pRTCtx->sSpinners, pConditionNode, sCurrentSpinner.qwSignatureStartPosLinear)))
							return error;
						if (TREE_FAILED(error = AddSpinner(pRTCtx, &pRTCtx->sNewSpinners, pConditionNode, sCurrentSpinner.qwSignatureStartPosLinear)))
							return error;
						if (dwNumOfBytes <= 1)
							break;
						if (TREE_FAILED(error = AllocRTNode(pRTCtx, &pRTNode, (RT_NODE_DATA **)(&pNodeDataAsterRT), NDT_Aster)))
							return error;
						pNodeDataAsterRT->dwNumOfBytes = dwNumOfBytes;
						pNodeDataAsterRT->dwCurrentByte = 1;
						pNodeDataAsterRT->pNextNode = pConditionNode;
						if (TREE_FAILED(error = AddSpinner(pRTCtx, &pRTCtx->sNewSpinners, & (pRTNode->N), sCurrentSpinner.qwSignatureStartPosLinear)))
							return error;
						break;
					}
				default:
					ODS(("Unknown condition type %02X", pCondition->ConditionType.nType));
					DBG_STOP;
					break;
				};
				
				if (bConditionResult == 1)
				{
					if (TREE_FAILED(error = AddSpinner(pRTCtx, &pRTCtx->sNewSpinners, pConditionNode, sCurrentSpinner.qwSignatureStartPosLinear)))
						return error;

					SignatureID = TreeGetNodeSignatureID(pConditionNode);
					if (SignatureID)
					{
						if (0 != TreeReportSignatureFound(TreeSignatureFoundCallbackFunc, pCallbackCtx, pRTCtx, &sCurrentSpinner, SignatureID))
							return TREE_ECANCEL;
					}
				}
				pCondition = pNextCondition;
			}
			
			sCurrentSpinner.pNode = TreeGetNextNode(sCurrentSpinner.pNode, pbData[pRTCtx->dwSearcherCurrentPositionInBuffer], pRTCtx);

			if (sCurrentSpinner.pNode == NULL) // RIP
			{
				pRTCtx->sSpinners.nInUse--;

#ifdef _TREE_DEBUG
				if (pRTCtx->SpinnerLevel[spinner] < DBG_MAX_SIGNATURE_LEN)
					pRTCtx->DeathLevelStat[pRTCtx->SpinnerLevel[spinner]]++;
#endif
				if (spinner == pRTCtx->sSpinners.nInUse) // it was last spinner
				{
#ifdef _TREE_DEBUG
					pRTCtx->sSpinners.Spinner[pRTCtx->sSpinners.nInUse].pNode = NULL;
#endif
					break;
				}
				
				memcpy(&pRTCtx->sSpinners.Spinner[spinner], &pRTCtx->sSpinners.Spinner[pRTCtx->sSpinners.nInUse], sizeof(SPINNER)); // move last spinner up
#ifdef _TREE_DEBUG
				pRTCtx->SpinnerLevel[spinner] = pRTCtx->SpinnerLevel[pRTCtx->sSpinners.nInUse];
				pRTCtx->SpinnerLevel[pRTCtx->sSpinners.nInUse] = 0;
				pRTCtx->sSpinners.Spinner[pRTCtx->sSpinners.nInUse].pNode = NULL;
#endif
				spinner--;
				continue;
			}
			else
			{
				tSIGNATURE_ID SignatureID = TreeGetNodeSignatureID(sCurrentSpinner.pNode);
#ifdef _TREE_DEBUG
				pRTCtx->SpinnerLevel[spinner]++;
#endif
				if (SignatureID)
				{
					if (0 != TreeReportSignatureFound(TreeSignatureFoundCallbackFunc, pCallbackCtx, pRTCtx, &sCurrentSpinner, SignatureID))
						return TREE_ECANCEL;
				}

				pCurrentSpinner->pNode = sCurrentSpinner.pNode; // update data from locals
			}
		}
		
#ifdef _TREE_DEBUG
		if (pRTCtx->sSpinners.Spinner && pRTCtx->sSpinners.Spinner[pRTCtx->sSpinners.nInUse].pNode != NULL)
		{
			ODS(("Opps! - pRTCtx->sSpinners.Spiners[pRTCtx->sSpinners.nInUse].pNode != NULL"));
			//return TREE_EUNEXPECTED;
		}
#endif
		if (pRTCtx->sNewSpinners.nInUse != 0)
			MoveSpinners(pRTCtx, &pRTCtx->sSpinners, &pRTCtx->sNewSpinners);
	}
	
	return TREE_OK;
}

TREE_STATIC
TREE_ERR _TREE_CALL TreeSearchData(sSEARCHER_RT_CTX* pRTCtx,
					  CONST uint8_t* pbData, // buffer to search in
					  uint32_t dwSize,     // data size in buffer
					  uint32_t dwStartPos, // start position for search
					  uint32_t dwEndPos,   // end position for search (signatures beginning before ulEndPos and continues after ulEndPos will be found)
					  tTreeSignatureFoundCallbackFunc TreeSignatureFoundCallbackFunc, // callback function
					  CONST void* pCallbackCtx) // callback context
{
	TREE_ERR error;

#if defined(_WIN32)
	sSEARCHER_RT_CTX* pOrigRTCtx;
	sSEARCHER_RT_CTX sLocalCtx;
	pOrigRTCtx = pRTCtx;
	memcpy(&sLocalCtx, pOrigRTCtx, sizeof(sRT_CTX));
	pRTCtx = &sLocalCtx;
#endif

	if (pRTCtx->pRoot == NULL)
		return TREE_EUNEXPECTED;

	pRTCtx->dwSearcherCurrentPositionInBuffer = 0;

	error = iTreeSearchData(pRTCtx, pbData, dwSize, dwStartPos, dwEndPos, TreeSignatureFoundCallbackFunc, pCallbackCtx);

#if defined(_WIN32)
	memcpy(pOrigRTCtx, &sLocalCtx, sizeof(sRT_CTX));
	pRTCtx = pOrigRTCtx;
#endif
	pRTCtx->qwBufferStartPositionLinear += pRTCtx->dwSearcherCurrentPositionInBuffer;

	return error;
}

// ------------------------------------------------------------------------------------------------

TREE_STATIC
TREE_ERR _TREE_CALL TreeNewSearch(sSEARCHER_RT_CTX* pRTCtx)
{

#ifdef _TREE_DEBUG
	if (pRTCtx->qwBufferStartPositionLinear)
		ODS(("Filtered: %d of %d (%f%%)", (unsigned int)pRTCtx->qwBufferStartPositionLinear - pRTCtx->dwStatSpinnersUsed, (unsigned int)pRTCtx->qwBufferStartPositionLinear, (float)(__int64)(pRTCtx->qwBufferStartPositionLinear - pRTCtx->dwStatSpinnersUsed) / (__int64)pRTCtx->qwBufferStartPositionLinear * 100));
	if (pRTCtx->DeathLevelStat[0]+pRTCtx->DeathLevelStat[1]+pRTCtx->DeathLevelStat[2]+pRTCtx->DeathLevelStat[3])
	{
		uint32_t i;
		ODS(("\nDeath level statistic:\n"));
		for (i=0; i<DBG_MAX_SIGNATURE_LEN;i++)
		{
			if (pRTCtx->DeathLevelStat[i] != 0)
				ODS(("%d	%d" , i, pRTCtx->DeathLevelStat[i]));
		}
		ODS(("-----------"));
	}
	pRTCtx->dwStatSpinnersUsed = 0;

	{
		uint32_t i;
		uint32_t total=0;
		for (i=0; i<countof(pRTCtx->FilterMap); i++)
		{
			if (pRTCtx->FilterMap[i])
			{
				ODS(("%04X	%d", i, pRTCtx->FilterMap[i]));
				total ++;
			}
		}
		ODS(("Total: %d", total));
	}

	ZeroMemory(pRTCtx->SpinnerLevel, sizeof(pRTCtx->SpinnerLevel));
	ZeroMemory(pRTCtx->DeathLevelStat, sizeof(pRTCtx->DeathLevelStat));
	ZeroMemory(pRTCtx->FilterMap, sizeof(pRTCtx->FilterMap));

#endif

	FreeAllSpinners(pRTCtx, &pRTCtx->sSpinners);
	FreeAllSpinners(pRTCtx, &pRTCtx->sNewSpinners);
	pRTCtx->qwBufferStartPositionLinear = 0;
	
	return TREE_OK;	
}

				  

