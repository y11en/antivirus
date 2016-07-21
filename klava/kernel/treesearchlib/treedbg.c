// treedbg.c
//

#include "common.h"
#define INDENT(level) {int i; for (i=0; i<level; i++) ODSN(("  "));}

#ifndef TREE_STATIC_ONLY

TREE_ERR TreeDumpNode (NODE* pNode, int level)
{
	tSIGNATURE_ID SignatureID = 0;
	CONDITION * pCondition = 0;
	
	if (pNode == NULL)
	{
		return TREE_EINVAL;
	}
	
	if (level==0)
	{
		ODSN(("ROOT"));
		level++;
	}
	
/*	if (TreeGetNodeChildrenCount(pNode)!=0 || pNode->pCondition != NULL)
	{
		switch (pNode->NodeDataType)
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
		default:
			ODSN((".unknown type: %02X", pNode->NodeDataType));
			DBG_STOP;
			return TREE_EUNEXPECTED;
		}
	}
*/	
	SignatureID = TreeGetNodeSignatureID(pNode);
	if (SignatureID)
		ODSN((" ID=%08X", SignatureID));
	
	ODSN(("\n"));
	
	level++;
	
	pCondition = TreeGetNodeCondition(pNode);
	while (pCondition != NULL)
	{
		INDENT(level); //ODSN(("??"));
		switch(pCondition->ConditionType.nType)
		{
		case cCT_BitMaskAnd:
				if (pCondition->Data.Byte1==0 && pCondition->Data.Byte1==0)
					ODSN(("[??    ]"))
				else
					ODSN((" [(tBYTE & 0x%02X) == 0x%02X]", pCondition->Data.Byte1, pCondition->Data.Byte2));
				break;
			break;
		case cCT_BitMaskOr:
				if (pCondition->Data.Byte1==0xFF && pCondition->Data.Byte1==0xFF)
					ODSN(("[??    ]"))
				else
					ODSN((" [(tBYTE | 0x%02X) == 0x%02X]", pCondition->Data.Byte1, pCondition->Data.Byte2));
			break;
//		case cCT_BitMaskXor:
//			ODSN((" [(tBYTE ^ 0x%02X) == 0x%02X]", pCondition->Data.Byte1, pCondition->Data.Byte2));
//			break;
		case cCT_Range:
			ODSN((" [(tBYTE >= 0x%02X) && (tBYTE <= 0x%02X)]", pCondition->Data.Byte1, pCondition->Data.Byte2));
			break;
		default:
			ODSN((" [unknown condition]"));
			DBG_STOP;
			return TREE_EUNEXPECTED;
		}

		if (pNode->NodeType.NodeTypeFields.NodeType == NT_Dynamic)
			TreeDumpNode(&(PDCOND(pCondition)->Node.N), level+3);
		else
			TreeDumpNode(&(PSCOND(pCondition)->Node.N), level+3);

		pCondition = TreeGetNextNodeCondition (pNode, pCondition);
	}
	
	switch (pNode->NodeType.NodeTypeFields.NodeDataType)
	{
	case NDT_P_List:
		{
			NODEDATA_P_LIST* pListChild = PDNODE(pNode)->NodeData.pNodeDataList;
			while (pListChild != NULL) {
				INDENT(level); ODSN(("%02X",  pListChild->NodeByte));
				TreeDumpNode(&(pListChild->Node.N), level);
				pListChild = pListChild->pNextChild;
			}
		}
		break;
	case NDT_P_Index32:
		{
			int i;
			NODEDATA_P_INDEX32* pIndex;
			pIndex = PDNODE(pNode)->NodeData.pNodeDataIndex; 
			for (i=0; i<(sizeof(pIndex->Index)/sizeof(pIndex->Index[0]));i++)
			{
				if (pIndex->Index[i]!=NULL)
				{
					INDENT(level); ODSN(("%02X", i));
					TreeDumpNode(& (pIndex->Index[i]->N), level);
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
			}
			level+=pJNode->dwNumOfBytes-1;
			TreeDumpNode(&(pJNode->Node.N), level);
			level-=pJNode->dwNumOfBytes-1;
		}
		break;
	case NDT_P_JoinedRT:
		ODS(("TreeDumpNode: Run-Time node in tree???"));
		break;
	case NDT_P_Hashed:
		{
			NODEDATA_P_HASHED* pHNode = PDNODE(pNode)->NodeData.pNodeDataHashed;
			INDENT(level); ODSN(("[%08X]", pHNode->dwHashVal));
			level+=4;
			TreeDumpNode(PNODE (&pHNode->Node), level);
			level-=4;
		}
		break;
	case NDT_P_HashedRT:
		ODS(("TreeDumpNode: Run-Time node in tree???"));
		break;
	default:
		ODSN(("TreeDumpNode: unknown node type"));
		DBG_STOP;
		return TREE_EUNEXPECTED;
	}
	
	level--;
	
	if (level == 1)
	{
		ODSN(("\n"));
		level=0;
	}

	return TREE_OK;	
}	

TREE_ERR _TREE_CALL TreeDump(sRT_CTX* pRTCtx)
{
	if (pRTCtx == NULL)
		return TREE_EINVAL;
	if (pRTCtx->sTreeInfo.dwMagic != MAGIC_TREE_DB_V2)
		return TREE_OK;
	return TreeDumpNode(pRTCtx->pRoot, 0);
}

#endif // TREE_STATIC_ONLY

