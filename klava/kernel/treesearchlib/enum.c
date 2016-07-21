// enum.c
//

#include "common.h"
#include "dynarray.h"

#ifndef TREE_STATIC_ONLY

#ifndef _DISABLE_ODS
#define INDENT(level) {unsigned int i; for (i=0; i<level+2; i++) ODSN(("  "));}
#else
#define INDENT(level)
#endif
#define RET_ON_ERR(x) { TREE_ERR error = x; if TREE_FAILED(error) return error; }

typedef struct tag_ENUM_CTX2
{
	NODE *        pNode;
	unsigned int  iteration;
	void *        pNodeData;
	CONDITION*    pCondition;
	tSIGNATURE_ID SignatureID;
	unsigned int  level;
}
tENUM_CTX2;

typedef struct tag_ENUM_CTX
{
	sRT_CTX *    pRTCtx;
	tTreeEnumCallbackFunc EnumCallback;
	CONST void * pCallbackCtx;
	uint8_t  *   pSignature;
	uint32_t     dwAllocatedSize;
	tENUM_CTX2   sCtx2;
	DYN_ARRAY*   pCtxDynArray;
}
tENUM_CTX;

static TREE_ERR iTreeEnumAddByte2Sig(tENUM_CTX* pEnumCtx, unsigned int* pLevel, uint8_t b, bool_t bDoubleEscChar)
{
	if (*pLevel + 2 >= pEnumCtx->dwAllocatedSize)
	{
		sHEAP_CTX* pHeapCtx = pEnumCtx->pRTCtx->pHeapCtx;
		pEnumCtx->pSignature = pHeapCtx->Realloc(pHeapCtx->pHeap, pEnumCtx->pSignature, *pLevel + 256);
		if (!pEnumCtx->pSignature)
		{
			pEnumCtx->dwAllocatedSize = 0;
			return TREE_ENOMEM;
		}
		pEnumCtx->dwAllocatedSize = *pLevel + 256;
	}
	pEnumCtx->pSignature[*pLevel] = b;
	(*pLevel)++;
	if (bDoubleEscChar && b==SIGNATURE_ESC_CHAR)
	{
		pEnumCtx->pSignature[*pLevel] = b;
		(*pLevel)++;
	}
	return TREE_OK;
}

static TREE_ERR TreeEnumAddByte2Sig(tENUM_CTX* pEnumCtx, unsigned int* pLevel, uint8_t b)
{
	return iTreeEnumAddByte2Sig(pEnumCtx, pLevel, b, 1);
}

static TREE_ERR TreeEnumAddCondition2Sig(tENUM_CTX* pEnumCtx, unsigned int* pLevel, uint8_t nConditionType, uint8_t bByte1, uint8_t bByte2)
{
	RET_ON_ERR(iTreeEnumAddByte2Sig(pEnumCtx, pLevel, SIGNATURE_ESC_CHAR, 0));
	RET_ON_ERR(iTreeEnumAddByte2Sig(pEnumCtx, pLevel, nConditionType, 0));
	RET_ON_ERR(iTreeEnumAddByte2Sig(pEnumCtx, pLevel, bByte1, 0));
	RET_ON_ERR(iTreeEnumAddByte2Sig(pEnumCtx, pLevel, bByte2, 0));
	return TREE_OK;
}

static TREE_ERR TreeEnumNode(tENUM_CTX* pEnumCtx, NODE* pNode, unsigned int level)
{
	unsigned int nNodeLevel = level;
	if (pNode == NULL)
	{
		DBG_STOP;
		return TREE_EINVAL;
	}

	if (level==0)
		ODSN(("ROOT"));

	switch (pNode->NodeType.NodeTypeFields.NodeDataType)
	{
	case NDT_P_List:
		ODSN((".C"));
		break;
	case NDT_P_Index32:
		ODSN((".I"));
		break;
	case NDT_P_Joined:
		ODSN((".J"));
		break;
	case NDT_P_JoinedRT:
		ODSN((".JRT"));
		break;
	case NDT_P_Hashed:
		ODSN((".H"));
		break;
	case NDT_P_HashedRT:
		ODSN((".HRT"));
		break;
	case NDT_O_Index32:
		ODSN((".OI32"));
		break;
	case NDT_O_Index32Fast:
		ODSN((".OI32F"));
		break;
	case NDT_O_List: 
		ODSN((".OL"));
		break;
	case NDT_O_Joined32:
		ODSN((".OJ32"));
		break;
	case NDT_O_JoinedRT32:
		ODSN((".OJRT32"));
		break;
	case NDT_O_Joined8:
		ODSN((".OJ8"));
		break;
	case NDT_O_JoinedRT8:
		ODSN((".OJRT8"));
		break;
	case NDT_O_Hashed:
		ODSN((".OH"));
		break;
	case NDT_O_HashedRT:
		ODSN((".OHRT"));
		break;
	case NDT_O_Array:
		ODSN((".OA"));
		break;
	case NDT_O_ArrayFast:
		ODSN((".OAF"));
		break;
	case NDT_O_Index8:
		ODSN((".OI8"));
		break;
	case NDT_O_Tail:
		ODSN((".OT"));
		break;
	default:
		ODSN((".unknown type: %02X", pNode->NodeType.NodeTypeFields.NodeDataType));
		DBG_STOP;
		return TREE_EUNEXPECTED;
	}

	{
		tSIGNATURE_ID SignatureID = TreeGetNodeSignatureID(pNode);
		if (SignatureID)
		{
			ODSN((" ID=%08X", SignatureID));
			if (TREE_ECANCEL == pEnumCtx->EnumCallback(pEnumCtx->pCallbackCtx, SignatureID, pEnumCtx->pSignature, level))
				return TREE_ECANCEL;
		}
	}
	
	ODSN(("\n"));
	
	{
		CONDITION* pCondition = TreeGetNodeCondition(pNode);
		uint32_t i;
		while (pCondition != NULL)
		{
			INDENT(level);
			switch(pCondition->ConditionType.nType)
			{
			case cCT_BitMaskAnd:
				if (pCondition->Data.Byte1==0 && pCondition->Data.Byte1==0)
					ODSN(("[??    ]"))
				else
					ODSN((" [(uint8_t & 0x%02X) == 0x%02X]", pCondition->Data.Byte1, pCondition->Data.Byte2));
				break;
			case cCT_BitMaskOr:
				if (pCondition->Data.Byte1==0xFF && pCondition->Data.Byte1==0xFF)
				{
					CONDITION dummy;
					memcpy(&dummy, pCondition, sizeof(DCONDITION));
					pCondition = &dummy;
					pCondition->ConditionType.nType = cCT_BitMaskAnd;
					pCondition->Data.Byte1 = 0;
					pCondition->Data.Byte2 = 0;
					ODSN(("[??    ]"));
				}
				else
					ODSN((" [(uint8_t | 0x%02X) == 0x%02X]", pCondition->Data.Byte1, pCondition->Data.Byte2));
				break;
	//		case cCT_BitMaskXor:
	//			ODSN((" [(uint8_t ^ 0x%02X) == 0x%02X]", pCondition->Data.Byte1, pCondition->Data.Byte2));
	//			break;
			case cCT_Range:
				ODSN((" [(uint8_t >= 0x%02X) && (uint8_t <= 0x%02X)]", pCondition->Data.Byte1, pCondition->Data.Byte2));
				break;
			case cCT_AnyByte:
				{
					NODE* pCondNode = TreeGetConditionNode(pNode, pCondition);
					uint32_t dwCount = *(uint16_t*)((uint8_t*)pCondition+1);
					for (i=0; i<dwCount; i++)
					{
						
						ODSN(("[??    ]"));
						RET_ON_ERR(TreeEnumAddCondition2Sig(pEnumCtx, &level, cCT_BitMaskAnd, 0, 0));
						if (i!=dwCount-1){ODS((""));INDENT(level);}
					}
					RET_ON_ERR(TreeEnumNode(pEnumCtx, pCondNode, level));
				}
				break;
			case cCT_Aster:
				ODSN((" [*(window=0x%X)]", pCondition->Data.Byte1 + (pCondition->Data.Byte2 << 8)));
				break;
			default:
				ODSN((" [unknown condition]"));
				DBG_STOP;
				return TREE_EUNEXPECTED;
			}

			if (pCondition->ConditionType.nType == cCT_AnyByte) // always last in list
				break;

			RET_ON_ERR(TreeEnumAddCondition2Sig(pEnumCtx, &level, pCondition->ConditionType.nType, pCondition->Data.Byte1, pCondition->Data.Byte2));
			RET_ON_ERR(TreeEnumNode(pEnumCtx, TreeGetConditionNode (pNode, pCondition), level));
			pCondition = TreeGetNextNodeCondition (pNode, pCondition);
			level = nNodeLevel;
		}
	}
	
	level = nNodeLevel;
	switch (pNode->NodeType.NodeTypeFields.NodeDataType)
	{
	case NDT_P_List:
		{
			NODEDATA_P_LIST* pListChild = PDNODE(pNode)->NodeData.pNodeDataList;
			while (pListChild != NULL) {
				INDENT(level); ODSN(("%02X",  pListChild->NodeByte));
				RET_ON_ERR(TreeEnumAddByte2Sig(pEnumCtx, &level, pListChild->NodeByte));
				RET_ON_ERR(TreeEnumNode(pEnumCtx, PNODE (&(pListChild->Node)), level));
				level = nNodeLevel;
				pListChild = pListChild->pNextChild;
			}
		}
		break;
	case NDT_P_Index32:
		{
			unsigned int i;
			NODEDATA_P_INDEX32* pIndex;
			pIndex = PDNODE(pNode)->NodeData.pNodeDataIndex; 
			for (i=0; i<(sizeof(pIndex->Index)/sizeof(pIndex->Index[0]));i++)
			{
				if (pIndex->Index[i]!=NULL)
				{
					INDENT(level); ODSN(("%02X", i));
					RET_ON_ERR(TreeEnumAddByte2Sig(pEnumCtx, &level, (uint8_t)i));
					RET_ON_ERR(TreeEnumNode(pEnumCtx, PNODE (pIndex->Index[i]), level));
					level = nNodeLevel;
				}
			}
		}
		break;
	case NDT_P_Joined:
		{
			int i;
			NODEDATA_P_JOINED* pJNode = PDNODE(pNode)->NodeData.pNodeDataJoined;
			INDENT(level); 
			for (i=0; i<(int)pJNode->dwNumOfBytes; i++)
			{
				ODSN(("%02X", pJNode->NodeBytes[i]));
				RET_ON_ERR(TreeEnumAddByte2Sig(pEnumCtx, &level, pJNode->NodeBytes[i]));
			}
			RET_ON_ERR(TreeEnumNode(pEnumCtx, PNODE(&(pJNode->Node)), level));
		}
		break;
	case NDT_P_JoinedRT:
		ODS(("TreeDumpNode: Run-Time node in tree???"));
		DBG_STOP;
		return TREE_EUNEXPECTED;
	case NDT_P_Hashed:
		ODS(("TreeDumpNode: Hashed node in dynamic tree???"));
		DBG_STOP;
		return TREE_EUNEXPECTED;
	case NDT_P_HashedRT:
		ODS(("TreeDumpNode: Run-Time node in tree???"));
		DBG_STOP;
		return TREE_EUNEXPECTED;

	case NDT_O_Index32:
		{
			unsigned int NodeByte;
			uint32_t dwOffset;
			NODE* pNextNode;
			uint8_t* pNodeData = TreeGetNodeData(pNode);
			for (NodeByte=0; NodeByte<=0xFF; NodeByte++)
			{
				dwOffset = *((uint32_t*)pNodeData + NodeByte)  * 4;
				if (dwOffset != 0)
				{
					INDENT(level); ODSN(("%02X", NodeByte));
					pNextNode = (NODE*)(pNodeData + dwOffset + 0x3FC);
					RET_ON_ERR(TreeEnumAddByte2Sig(pEnumCtx, &level, (uint8_t)NodeByte));
					RET_ON_ERR(TreeEnumNode(pEnumCtx, pNextNode, level));
					level = nNodeLevel;
				}
			}
		}
		break;

	case NDT_O_Index32Fast:
		{
			unsigned int NodeByte;
			uint32_t dwOffset;
			NODE* pNextNode;
			uint8_t* pNodeData = TreeGetNodeData(pNode);

			for (NodeByte=0; NodeByte<=0xFF; NodeByte++)
			{
				pNextNode = (NODE*) (pNodeData + NodeByte*sizeof(SNODE_D));
				if (*(uint32_t*)pNextNode != 0)
				{
					INDENT(level); ODSN(("%02X", NodeByte));
					dwOffset = *((uint32_t*)pNodeData + NodeByte)  * 4;
					pNextNode = (NODE*)(pNodeData + dwOffset + 0x3FC);
					RET_ON_ERR(TreeEnumAddByte2Sig(pEnumCtx, &level, (uint8_t)NodeByte));
					RET_ON_ERR(TreeEnumNode(pEnumCtx, pNextNode, level));
					level = nNodeLevel;
				}
			}
		}
		break;

	case NDT_O_Index8:
		{
			unsigned int NodeByte;
			NODE* pNextNode;
			uint8_t* pNodeData = TreeGetNodeData(pNode);
			for (NodeByte=0; NodeByte<=0xFF; NodeByte++)
			{
				uint32_t dwOffset = pNodeData[NodeByte];
				if (dwOffset)
				{
					INDENT(level); ODSN(("%02X", NodeByte));
					pNextNode = (NODE*)(pNodeData + 256 - 4 + dwOffset*4);
					RET_ON_ERR(TreeEnumAddByte2Sig(pEnumCtx, &level, (uint8_t)NodeByte));
					RET_ON_ERR(TreeEnumNode(pEnumCtx, pNextNode, level));
					level = nNodeLevel;
				}
			}
		}
		break;

	case NDT_O_List: 
		ODSN(("TreeEnumNode: Unsupported node type"));
		DBG_STOP;
		return TREE_EUNEXPECTED;

	case NDT_O_Joined32:
		{
			uint32_t i;
			uint8_t NodeByte;
			NODE* pNextNode;
			uint32_t dwAlign;
			uint8_t* pNodeData = TreeGetNodeData(pNode);
			uint32_t dwNumOfBytes = *(uint32_t*)pNodeData;
			
			if (dwNumOfBytes == 1)
			{
				DBG_STOP; // ???? Joined node 1-byte size???
				return TREE_EUNEXPECTED;
			}
		
			INDENT(level); 
			for (i=0; i<dwNumOfBytes; i++)
			{
				NodeByte = pNodeData[i+4];
				ODSN(("%02X", NodeByte));
				RET_ON_ERR(TreeEnumAddByte2Sig(pEnumCtx, &level, NodeByte));
			}
			dwAlign = ((dwNumOfBytes) & 3);
			if (dwAlign)
				dwAlign = 4 - dwAlign;
			pNextNode = (NODE*)(pNodeData + (dwNumOfBytes+4) + dwAlign);
			RET_ON_ERR(TreeEnumNode(pEnumCtx, pNextNode, level));
		}
		break;
		
	case NDT_O_JoinedRT32:
		ODS(("TreeDumpNode: Run-Time node in tree???"));
		DBG_STOP;
		return TREE_EUNEXPECTED;

	case NDT_O_Joined8:
		{
			uint32_t i;
			NODE* pNextNode;
			uint32_t dwAlign;
			uint8_t* pNodeData = TreeGetNodeData(pNode);
			uint32_t dwNumOfBytes = pNodeData[0];

			if (dwNumOfBytes == 1)
			{
				DBG_STOP; // ???? Joined node 1-byte size???
				return TREE_EUNEXPECTED;
			}

			INDENT(level); 
			for (i=0; i<dwNumOfBytes; i++)
			{
				ODSN(("%02X", pNodeData[i+1]));
				RET_ON_ERR(TreeEnumAddByte2Sig(pEnumCtx, &level, pNodeData[i+1]));
			}
			dwAlign = ((dwNumOfBytes+1) & 3);
			if (dwAlign)
				dwAlign = 4 - dwAlign;
			pNextNode = (NODE*)(pNodeData + (dwNumOfBytes+1) + dwAlign);
			RET_ON_ERR(TreeEnumNode(pEnumCtx, pNextNode, level));
		}
		break;
		
	case NDT_O_JoinedRT8:
		ODS(("TreeDumpNode: Run-Time node in tree???"));
		DBG_STOP;
		return TREE_EUNEXPECTED;

	case NDT_O_Hashed:
		ODSN(("TreeEnumNode: Unsupported node type"));
		DBG_STOP;
		return TREE_EUNEXPECTED;

	case NDT_O_HashedRT:
		ODS(("TreeDumpNode: Run-Time node in tree???"));
		DBG_STOP;
		return TREE_EUNEXPECTED;

	case NDT_O_Array:
		{
			uint32_t i;
			uint8_t NodeByte;
			uint32_t dwOffset;
			NODE* pNextNode;
			uint8_t* pNodeData = TreeGetNodeData(pNode);
			uint32_t dwNumOfBytes = pNodeData[0];
			
			for (i=0; i<dwNumOfBytes; i++)
			{
				NodeByte = pNodeData[1+i];
				INDENT(level); ODSN(("%02X", NodeByte));
				dwOffset = pNodeData[1+dwNumOfBytes+i];
				pNextNode = (NODE*)(pNodeData + dwOffset*4);
				RET_ON_ERR(TreeEnumAddByte2Sig(pEnumCtx, &level, NodeByte));
				RET_ON_ERR(TreeEnumNode(pEnumCtx, pNextNode, level));
				level = nNodeLevel;
			}
		}
		break;
		
	case NDT_O_ArrayFast:
		{
			uint32_t i;
			uint8_t NodeByte;
			NODE* pNextNode;
			uint8_t* pNodeData = TreeGetNodeData(pNode);
			uint32_t dwNumOfBytes = pNodeData[0];
			uint32_t dwAlign = ((1+dwNumOfBytes) & 3);
			if (dwAlign)
				dwAlign = 4 - dwAlign;
			
			for (i=0; i<dwNumOfBytes; i++)
			{
				NodeByte = pNodeData[1+i];
				INDENT(level); ODSN(("%02X", NodeByte));
				pNextNode =  (NODE*) (pNodeData + (1+dwNumOfBytes) + dwAlign + i*sizeof(SNODE_D));
				RET_ON_ERR(TreeEnumAddByte2Sig(pEnumCtx, &level, NodeByte));
				RET_ON_ERR(TreeEnumNode(pEnumCtx, pNextNode, level));
				level = nNodeLevel;
			}
		}
		break;

	case NDT_O_Tail:
		// empty node
		break;

	default:
		ODSN(("TreeDumpNode: unknown node type"));
		DBG_STOP;
		return TREE_EUNEXPECTED;
	}
	
	return TREE_OK;	
}	


static void CtxInit(tENUM_CTX* pEnumCtx, NODE* pNode, unsigned int level)
{
	tENUM_CTX2* pCtx = &pEnumCtx->sCtx2;
	memset(pCtx, 0, sizeof(tENUM_CTX2));
	pCtx->pNode = pNode;
	pCtx->level = level;
	pCtx->SignatureID = TreeGetNodeSignatureID(pNode);
	pCtx->pCondition = TreeGetNodeCondition(pNode);
}

static TREE_ERR CtxPush(tENUM_CTX* pEnumCtx, NODE* pNode, unsigned int level)
{
	TREE_ERR error;
	tENUM_CTX2* pCtx = &pEnumCtx->sCtx2;
	if (TREE_SUCCEEDED(error = DynArrayPush(pEnumCtx->pCtxDynArray, pCtx)))
		CtxInit(pEnumCtx, pNode, level);
	return error;
}

static TREE_ERR CtxPop(tENUM_CTX* pEnumCtx)
{
	TREE_ERR error;
	error = DynArrayPop(pEnumCtx->pCtxDynArray, &pEnumCtx->sCtx2);
	return error;
}

static TREE_ERR TreeEnumNode2(tENUM_CTX* pEnumCtx, NODE* pNode, unsigned int level)
{

	tENUM_CTX2* pCtx = &pEnumCtx->sCtx2;
	if (pNode == NULL)
	{
		DBG_STOP;
		return TREE_EINVAL;
	}
	
	if (level==0)
		ODSN(("ROOT"));

	CtxInit(pEnumCtx, pNode, level);

	
	switch (pNode->NodeType.NodeTypeFields.NodeDataType)
	{
	case NDT_P_List:
		ODSN((".C"));
		break;
	case NDT_P_Index32:
		ODSN((".I"));
		break;
	case NDT_P_Joined:
		ODSN((".J"));
		break;
	case NDT_P_JoinedRT:
		ODSN((".JRT"));
		break;
	case NDT_P_Hashed:
		ODSN((".H"));
		break;
	case NDT_P_HashedRT:
		ODSN((".HRT"));
		break;
	case NDT_O_Index32:
		ODSN((".OI32"));
		break;
	case NDT_O_Index32Fast:
		ODSN((".OI32F"));
		break;
	case NDT_O_List: 
		ODSN((".OL"));
		break;
	case NDT_O_Joined32:
		ODSN((".OJ32"));
		break;
	case NDT_O_JoinedRT32:
		ODSN((".OJRT32"));
		break;
	case NDT_O_Joined8:
		ODSN((".OJ8"));
		break;
	case NDT_O_JoinedRT8:
		ODSN((".OJRT8"));
		break;
	case NDT_O_Hashed:
		ODSN((".OH"));
		break;
	case NDT_O_HashedRT:
		ODSN((".OHRT"));
		break;
	case NDT_O_Array:
		ODSN((".OA"));
		break;
	case NDT_O_ArrayFast:
		ODSN((".OAF"));
		break;
	case NDT_O_Index8:
		ODSN((".OI8"));
		break;
	case NDT_O_Tail:
		ODSN((".OT"));
		break;
	default:
		ODSN((".unknown type: %02X", pNode->NodeType.NodeTypeFields.NodeDataType));
		DBG_STOP;
		return TREE_EUNEXPECTED;
	}
	
	do {
		CONDITION* pCondition = pCtx->pCondition;

		unsigned int iteration;
		//unsigned int nNodeLevel;
		pNode = pCtx->pNode;
		level = pCtx->level;
		//nNodeLevel = level;

		if (pCtx->SignatureID)
		{
			ODSN((" ID=%08X", pCtx->SignatureID));
			if (TREE_ECANCEL == pEnumCtx->EnumCallback(pEnumCtx->pCallbackCtx, pCtx->SignatureID, pEnumCtx->pSignature, level))
				return TREE_ECANCEL;
			pCtx->SignatureID = 0;
		}

		if (pCondition != NULL)
		{
			INDENT(level);
			switch(pCondition->ConditionType.nType)
			{
			case cCT_BitMaskAnd:
				if (pCondition->Data.Byte1==0 && pCondition->Data.Byte1==0)
					ODSN(("[??    ]"))
				else
					ODSN((" [(uint8_t & 0x%02X) == 0x%02X]", pCondition->Data.Byte1, pCondition->Data.Byte2));
				break;
			case cCT_BitMaskOr:
				if (pCondition->Data.Byte1==0xFF && pCondition->Data.Byte1==0xFF)
				{
//					CONDITION dummy;
//					memcpy(&dummy, pCondition, sizeof(DCONDITION));
//					pCondition = &dummy;
//					pCondition->nConditionType = cCT_BitMaskAnd;
//					pCondition->Data.Byte1 = 0;
//					pCondition->Data.Byte2 = 0;

					ODSN(("[??    ]"));
					// special case here - to ensure "any byte" is always enumerated the same
					pCtx->pCondition = TreeGetNextNodeCondition (pNode, pCondition); 
					RET_ON_ERR(TreeEnumAddCondition2Sig(pEnumCtx, &level, cCT_BitMaskAnd, 0, 0));
					RET_ON_ERR(CtxPush(pEnumCtx, TreeGetConditionNode (pNode, pCondition), level));
					continue;
				}
				else
					ODSN((" [(uint8_t | 0x%02X) == 0x%02X]", pCondition->Data.Byte1, pCondition->Data.Byte2));
				break;
	//		case cCT_BitMaskXor:
	//			ODSN((" [(uint8_t ^ 0x%02X) == 0x%02X]", pCondition->Data.Byte1, pCondition->Data.Byte2));
	//			break;
			case cCT_Range:
				ODSN((" [(uint8_t >= 0x%02X) && (uint8_t <= 0x%02X)]", pCondition->Data.Byte1, pCondition->Data.Byte2));
				break;
			case cCT_AnyByte:
				{
					NODE* pCondNode = TreeGetConditionNode (pNode, pCondition);
					uint32_t dwCount = *(uint16_t*)((uint8_t*)pCondition+1);
					uint32_t i;
					for (i=0; i<dwCount; i++)
					{
						
						ODSN(("[??    ]"));
						RET_ON_ERR(TreeEnumAddCondition2Sig(pEnumCtx, &level, cCT_BitMaskAnd, 0, 0));
						if (i!=dwCount-1){ODS((""));INDENT(level);}
					}
					pCtx->pCondition = NULL;
					RET_ON_ERR(CtxPush(pEnumCtx, pCondNode, level));
					continue;
				}
				break;
			case cCT_Aster:
				ODSN((" [*(window=0x%X)]", pCondition->Data.Byte1 + (pCondition->Data.Byte2 << 8)));
				break;
			default:
				ODSN((" [unknown condition]"));
				DBG_STOP;
				return TREE_EUNEXPECTED;
			}

			pCtx->pCondition = TreeGetNextNodeCondition (pNode, pCondition);
			RET_ON_ERR(TreeEnumAddCondition2Sig(pEnumCtx, &level, pCondition->ConditionType.nType, pCondition->Data.Byte1, pCondition->Data.Byte2));
			RET_ON_ERR(CtxPush(pEnumCtx, TreeGetConditionNode (pNode,pCondition), level));
			continue;
		}

		iteration = pCtx->iteration++;
		if (iteration == 0) // only on 1'st iteration
			ODSN(("\n"));
		
		//level = nNodeLevel;
		switch (pNode->NodeType.NodeTypeFields.NodeDataType)
		{
		case NDT_P_List:
			{
				NODEDATA_P_LIST* pListChild;
				if (iteration == 0)
					pCtx->pNodeData = PDNODE(pNode)->NodeData.pNodeDataList;
				pListChild = (NODEDATA_P_LIST*)pCtx->pNodeData;
				if (pListChild != NULL) {
					INDENT(level); ODSN(("%02X",  pListChild->NodeByte));
					RET_ON_ERR(TreeEnumAddByte2Sig(pEnumCtx, &level, pListChild->NodeByte));
					pCtx->pNodeData = pListChild->pNextChild;
					RET_ON_ERR(CtxPush(pEnumCtx, PNODE (&(pListChild->Node)), level));
					continue;
				}
			}
			break;
		case NDT_P_Index32:
			{
				unsigned int i = iteration;
				NODEDATA_P_INDEX32* pIndex;
				pIndex = PDNODE(pNode)->NodeData.pNodeDataIndex; 
				
				if (iteration >= (sizeof(pIndex->Index)/sizeof(pIndex->Index[0])))
					break;

				if (pIndex->Index[i]!=NULL)
				{
					INDENT(level); ODSN(("%02X", i));
					RET_ON_ERR(TreeEnumAddByte2Sig(pEnumCtx, &level, (uint8_t)i));
					RET_ON_ERR(CtxPush(pEnumCtx, PNODE (pIndex->Index[i]), level));
				}
				continue;
			}
		case NDT_P_Joined:
			{
				int i;
				NODEDATA_P_JOINED* pJNode = PDNODE(pNode)->NodeData.pNodeDataJoined;
				if (iteration != 0)
					break;
				INDENT(level); 
				for (i=0; i<(int)pJNode->dwNumOfBytes; i++)
				{
					ODSN(("%02X", pJNode->NodeBytes[i]));
					RET_ON_ERR(TreeEnumAddByte2Sig(pEnumCtx, &level, pJNode->NodeBytes[i]));
				}
				RET_ON_ERR(CtxPush(pEnumCtx, PNODE(&(pJNode->Node)), level));
				continue;
			}
		case NDT_P_JoinedRT:
			ODS(("TreeDumpNode: Run-Time node in tree???"));
			DBG_STOP;
			return TREE_EUNEXPECTED;
		case NDT_P_Hashed:
			ODS(("TreeDumpNode: Hashed node in dynamic tree???"));
			DBG_STOP;
			return TREE_EUNEXPECTED;
		case NDT_P_HashedRT:
			ODS(("TreeDumpNode: Run-Time node in tree???"));
			DBG_STOP;
			return TREE_EUNEXPECTED;

		case NDT_O_Index32:
			{
				unsigned int NodeByte = iteration;
				uint32_t dwOffset;
				NODE* pNextNode;
				uint8_t* pNodeData = TreeGetNodeData(pNode);
				if (NodeByte>0xFF)
					break;
				dwOffset = *((uint32_t*)pNodeData + NodeByte)  * 4;
				if (dwOffset != 0)
				{
					INDENT(level); ODSN(("%02X", NodeByte));
					pNextNode = (NODE*)(pNodeData + dwOffset + 0x3FC);
					RET_ON_ERR(TreeEnumAddByte2Sig(pEnumCtx, &level, (uint8_t)NodeByte));
					RET_ON_ERR(CtxPush(pEnumCtx, pNextNode, level));
					//level = nNodeLevel;
				}
				continue;
			}
			break;

		case NDT_O_Index32Fast:
			{
				unsigned int NodeByte = iteration;
				NODE* pNextNode;
				uint8_t* pNodeData = TreeGetNodeData(pNode);
				if (NodeByte>0xFF)
					break;
				pNextNode = (NODE*) (pNodeData + NodeByte*sizeof(SNODE_D));
				if (*(uint32_t*)pNextNode != 0)
				{
					INDENT(level); ODSN(("%02X", NodeByte));
					//dwOffset = *((uint32_t*)pNodeData + NodeByte)  * 4;
					//pNextNode = (NODE*)(pNodeData + dwOffset + 0x3FC);
					RET_ON_ERR(TreeEnumAddByte2Sig(pEnumCtx, &level, (uint8_t)NodeByte));
					RET_ON_ERR(CtxPush(pEnumCtx, pNextNode, level));
					//level = nNodeLevel;
				}
				continue;
			}
			break;

		case NDT_O_Index8:
			{
				unsigned int NodeByte = iteration;
				NODE* pNextNode;
				uint8_t* pNodeData = TreeGetNodeData(pNode);
				uint32_t dwOffset;
				if (NodeByte>0xFF)
					break;
				dwOffset = pNodeData[NodeByte];
				if (dwOffset)
				{
					INDENT(level); ODSN(("%02X", NodeByte));
					pNextNode = (NODE*)(pNodeData + 256 - 4 + dwOffset*4);
					RET_ON_ERR(TreeEnumAddByte2Sig(pEnumCtx, &level, (uint8_t)NodeByte));
					RET_ON_ERR(CtxPush(pEnumCtx, pNextNode, level));
				}
				continue;
			}
			break;

		case NDT_O_List: 
			ODSN(("TreeEnumNode: Unsupported node type"));
			DBG_STOP;
			return TREE_EUNEXPECTED;

		case NDT_O_Joined32:
			{
				uint32_t i;
				uint8_t NodeByte;
				uint32_t dwAlign;
				NODE* pNextNode;
				uint8_t* pNodeData = TreeGetNodeData(pNode);
				uint32_t dwNumOfBytes = *(uint32_t*)pNodeData;
				
				if (iteration != 0)
					break;

				if (dwNumOfBytes == 1)
				{
					DBG_STOP; // ???? Joined node 1-byte size???
					return TREE_EUNEXPECTED;
				}
			
				INDENT(level); 
				for (i=0; i<dwNumOfBytes; i++)
				{
					NodeByte = pNodeData[i+4];
					ODSN(("%02X", NodeByte));
					RET_ON_ERR(TreeEnumAddByte2Sig(pEnumCtx, &level, NodeByte));
				}
				dwAlign = ((dwNumOfBytes+4) & 3);
				if (dwAlign)
					dwAlign = 4 - dwAlign;
				pNextNode = (NODE*)(pNodeData + dwNumOfBytes + 4 + dwAlign);
				RET_ON_ERR(CtxPush(pEnumCtx, pNextNode, level));
				continue;
			}
			break;
			
		case NDT_O_JoinedRT32:
			ODS(("TreeDumpNode: Run-Time node in tree???"));
			DBG_STOP;
			return TREE_EUNEXPECTED;

		case NDT_O_Joined8:
			{
				uint32_t i;
				NODE* pNextNode;
				uint32_t dwAlign;
				uint8_t* pNodeData = TreeGetNodeData(pNode);
				uint32_t dwNumOfBytes = pNodeData[0];

				if (iteration != 0)
					break;

				if (dwNumOfBytes == 1)
				{
					DBG_STOP; // ???? Joined node 1-byte size???
					return TREE_EUNEXPECTED;
				}

				INDENT(level); 
				for (i=0; i<dwNumOfBytes; i++)
				{
					ODSN(("%02X", pNodeData[i+1]));
					RET_ON_ERR(TreeEnumAddByte2Sig(pEnumCtx, &level, pNodeData[i+1]));
				}
				dwAlign = ((dwNumOfBytes+1) & 3);
				if (dwAlign)
					dwAlign = 4 - dwAlign;
				pNextNode = (NODE*)(pNodeData + (dwNumOfBytes+1) + dwAlign);
				RET_ON_ERR(CtxPush(pEnumCtx, pNextNode, level));
				continue;
			}
			break;
			
		case NDT_O_JoinedRT8:
			ODS(("TreeDumpNode: Run-Time node in tree???"));
			DBG_STOP;
			return TREE_EUNEXPECTED;

		case NDT_O_Hashed:
			ODSN(("TreeEnumNode: Unsupported node type"));
			DBG_STOP;
			return TREE_EUNEXPECTED;

		case NDT_O_HashedRT:
			ODS(("TreeDumpNode: Run-Time node in tree???"));
			DBG_STOP;
			return TREE_EUNEXPECTED;

		case NDT_O_Array:
			{
				uint32_t i = iteration;
				uint8_t NodeByte;
				uint32_t dwOffset;
				NODE* pNextNode;
				uint8_t* pNodeData = TreeGetNodeData(pNode);
				uint32_t dwNumOfBytes = pNodeData[0];
				
				if (i>=dwNumOfBytes)
					break;
				NodeByte = pNodeData[1+i];
				INDENT(level); ODSN(("%02X", NodeByte));
				dwOffset = pNodeData[1+dwNumOfBytes+i];
				pNextNode = (NODE*)(pNodeData + dwOffset*4);
				RET_ON_ERR(TreeEnumAddByte2Sig(pEnumCtx, &level, NodeByte));
				RET_ON_ERR(CtxPush(pEnumCtx, pNextNode, level));
				continue;
			}
			break;
			
		case NDT_O_ArrayFast:
			{
				uint32_t i=iteration;
				uint8_t NodeByte;
				NODE* pNextNode;
				uint8_t* pNodeData = TreeGetNodeData(pNode);
				uint32_t dwNumOfBytes = pNodeData[0];
				uint32_t dwAlign = ((1+dwNumOfBytes) & 3);
				if (i >= dwNumOfBytes)
					break;
				if (dwAlign)
					dwAlign = 4 - dwAlign;
				NodeByte = pNodeData[1+i];
				INDENT(level); ODSN(("%02X", NodeByte));
				pNextNode =  (NODE*) (pNodeData + (1+dwNumOfBytes) + dwAlign + i*sizeof(SNODE_D));
				RET_ON_ERR(TreeEnumAddByte2Sig(pEnumCtx, &level, NodeByte));
				RET_ON_ERR(CtxPush(pEnumCtx, pNextNode, level));
				continue;
			}
			break;

		case NDT_O_Tail:
			// empty node
			break;

		default:
			ODSN(("TreeDumpNode: unknown node type"));
			DBG_STOP;
			return TREE_EUNEXPECTED;
		}
	
		if (TREE_FAILED(CtxPop(pEnumCtx))) // no more data
			break;
	
	} while(1);
	
	return TREE_OK;	
}	

EXTERN_C TREE_ERR _TREE_CALL TreeEnumSignatures(sRT_CTX* pRTCtx, tTreeEnumCallbackFunc TreeEnumCallbackFunc, CONST void* pCallbackCtx)
{
	TREE_ERR error;
	tENUM_CTX sEnumCtx;
	if (pRTCtx == NULL)
		return TREE_EINVAL;
	if (pRTCtx->sTreeInfo.dwMagic != MAGIC_TREE_DB_V2) // empty database
		return TREE_OK;
	memset(&sEnumCtx, 0, sizeof(tENUM_CTX));
	sEnumCtx.pRTCtx = pRTCtx;
	sEnumCtx.EnumCallback = TreeEnumCallbackFunc;
	sEnumCtx.pCallbackCtx = pCallbackCtx;
	if (TREE_FAILED(error = CreateDynArray(pRTCtx->pHeapCtx, &sEnumCtx.pCtxDynArray, sizeof(tENUM_CTX2))))
		return error;
	error = TreeEnumNode2(&sEnumCtx, pRTCtx->pRoot, 0);
	if (sEnumCtx.pSignature)
		pRTCtx->pHeapCtx->Free(pRTCtx->pHeapCtx->pHeap, sEnumCtx.pSignature);
	DestroyDynArray(&sEnumCtx.pCtxDynArray);
	return error;
}

#endif // TREE_STATIC_ONLY

