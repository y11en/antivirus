// savedyn.c
//

#include "common.h"
#include "savedyn.h"

// ------------------------------------------------------------------------------------------------

//TREE_ERR _Write(pIOCtx, tVOID* pData, uint32_t dwSize, sIO_CTX* pIOCtx);
//TREE_ERR TreeRead(tVOID* pData, uint32_t dwSize, sIO_CTX* pIOCtx);

// ------------------------------------------------------------------------------------------------
TREE_ERR TreeSaveNode (DNODE* pNode, sIO_CTX* pIOCtx);

TREE_ERR TreeSaveNodeItself (DNODE* pNode, uint8_t bBitFlags, sIO_CTX* pIOCtx)
{
	TREE_ERR error = TREE_OK;
	NODEFLAGS nf;
	ZeroMemory(&nf, sizeof(nf));
	
	if (pNode->pCondition)
		nf.bCondition = 1;
	if (pNode->SignatureID)
		nf.bSignatureID = 1;
	if (pNode->NodeData.pNodeDataList)
		nf.bNodeDataPtr = 1;
	nf.bReserved1 = (bBitFlags >> 0) & 1;
	nf.bReserved2 = (bBitFlags >> 1) & 1;

	nf.bNodeType = (uint8_t)pNode->N.NodeType.NodeTypeFields.NodeDataType;
	error = _Write(pIOCtx, &nf, sizeof(nf));
	RET_ON_FAIL;
	if (pNode->SignatureID)
		error = _Write(pIOCtx, &pNode->SignatureID, sizeof(pNode->SignatureID));
	return error;	
}

// ------------------------------------------------------------------------------------------------

TREE_ERR TreeSaveNodeDataList(DNODE* pNode, sIO_CTX* pIOCtx)
{
	TREE_ERR error = TREE_OK;
	NODEDATA_P_LIST* pCNode = NULL;

//	_Write(pIOCtx, "<C>>", sizeof(uint32_t));

	pCNode = pNode->NodeData.pNodeDataList;
	while (pCNode!=NULL)
	{
		uint8_t bBitFlags = 0;

		if (pCNode->pNextChild) // node has child
			bBitFlags = 1;

		error = _Write(pIOCtx, &pCNode->NodeByte, sizeof(pCNode->NodeByte));
		RET_ON_FAIL;
		error = TreeSaveNodeItself(&pCNode->Node, bBitFlags, pIOCtx);
		RET_ON_FAIL;
		pCNode = pCNode->pNextChild;
	}
	
	pCNode = pNode->NodeData.pNodeDataList;
	while (pCNode!=NULL)
	{
		error = TreeSaveNode(&(pCNode->Node), pIOCtx);
		RET_ON_FAIL;
		pCNode = pCNode->pNextChild;
	}
	

	return error;
}

// ------------------------------------------------------------------------------------------------

TREE_ERR TreeSaveNodeDataJoined(DNODE* pNode, sIO_CTX* pIOCtx)
{
	TREE_ERR error = TREE_OK;
	uint8_t bBitFlags = 0;
	uint8_t bNumOfBytes;
	NODEDATA_P_JOINED* pNDJoined = NULL;
	
//	_Write(pIOCtx, "<J>>", sizeof(uint32_t));

	pNDJoined = pNode->NodeData.pNodeDataJoined;
	if (pNDJoined->dwNumOfBytes < 2)
	{
		DBG_STOP
	}
	
	// short data len
	if (pNDJoined->dwNumOfBytes < 0x100) 
		bBitFlags |= (1<<0); 

	// child node is tail node
	if (pNDJoined->Node.N.NodeType.NodeTypeFields.NodeDataType == NDT_P_List && TreeGetNodeChildrenCount(&pNDJoined->Node) == 0 &&  pNDJoined->Node.pCondition == NULL && pNDJoined->Node.SignatureID != 0) 
		bBitFlags |= (1<<1);
		
	error = TreeSaveNodeItself(&pNDJoined->Node, bBitFlags, pIOCtx);
	RET_ON_FAIL;
	if ((bBitFlags & 1) != 0)
	{
		// short (uint8_t) data len
		bNumOfBytes = (uint8_t)pNDJoined->dwNumOfBytes;
		error = _Write(pIOCtx, &bNumOfBytes, sizeof(bNumOfBytes));
	}
	else
	{
		// long (uint32_t) data len
		error = _Write(pIOCtx, &pNDJoined->dwNumOfBytes, sizeof(pNDJoined->dwNumOfBytes));
	}
	RET_ON_FAIL;
	
	error = _Write(pIOCtx, pNDJoined+1, pNDJoined->dwNumOfBytes);
	RET_ON_FAIL;
	
	if ((bBitFlags & (1<<1)) == 0)
	{
		// child node is not tail node
		error = TreeSaveNode(&(pNDJoined->Node), pIOCtx);
	}
	/*
	else
	{
		// child node is tail node
		error = _Write(pIOCtx, &pNDJoined->Node.SignatureID, sizeof(pNDJoined->Node.SignatureID));
	}
	*/
	
	return error;
}

TREE_ERR TreeSaveNodeDataHashed(DNODE* pNode, sIO_CTX* pIOCtx)
{
	TREE_ERR error = TREE_OK;
	uint8_t bBitFlags = 0;
	uint8_t bNumOfBytes;
	NODEDATA_P_HASHED* pNDHashed = NULL;

	//	_Write(pIOCtx, "<H>>", sizeof(uint32_t));
	
	pNDHashed = pNode->NodeData.pNodeDataHashed;
	
	// short data len
	if (pNDHashed->dwNumOfBytes < 0x100) 
		bBitFlags |= (1<<0); 
	
	// child node is tail node
	if (pNDHashed->Node.N.NodeType.NodeTypeFields.NodeDataType == NDT_P_List
	   && TreeGetNodeChildrenCount (PDNODE (&pNDHashed->Node)) == 0
	   && PDNODE (&pNDHashed->Node)->pCondition == NULL
	   && PDNODE (&pNDHashed->Node)->SignatureID != 0) 
		bBitFlags |= (1<<1);
	
	error = TreeSaveNodeItself (PDNODE (&pNDHashed->Node), bBitFlags, pIOCtx);
	RET_ON_FAIL;

	if ((bBitFlags & 1) != 0)
	{
		// short (uint8_t) data len
		bNumOfBytes = (uint8_t)pNDHashed->dwNumOfBytes;
		error = _Write(pIOCtx, &bNumOfBytes, sizeof(bNumOfBytes));
	}
	else
	{
		// long (uint32_t) data len
		error = _Write(pIOCtx, &pNDHashed->dwNumOfBytes, sizeof(pNDHashed->dwNumOfBytes));
	}
	RET_ON_FAIL;
	
	error = _Write(pIOCtx, &pNDHashed->dwHashVal, sizeof(pNDHashed->dwHashVal));
	RET_ON_FAIL;
	
	if ((bBitFlags & (1<<1)) == 0)
	{
		// child node is not tail node
		error = TreeSaveNode (PDNODE (&pNDHashed->Node), pIOCtx);
	}
	/*
	else
	{
		// child node is tail node
		error = _Write(pIOCtx, &pNDHashed->Node.SignatureID, sizeof(pNDHashed->Node.SignatureID));
	}
	*/
	
	return error;
}

// ------------------------------------------------------------------------------------------------

TREE_ERR TreeSaveNodeDataIndexed(DNODE* pNode, sIO_CTX* pIOCtx)
{

	TREE_ERR error = TREE_OK;
	uint32_t i;
	uint32_t dwChildrenCount = 0;
	uint8_t bIndexStoreType = cFull;
	uint32_t dw = 0;
	NODEDATA_P_INDEX32* pIndex = NULL;
		
//	_Write(pIOCtx, "<I>>", sizeof(uint32_t));
	pIndex = pNode->NodeData.pNodeDataIndex;

	for (i=0; i<(sizeof(pIndex->Index)/sizeof(pIndex->Index[0]));i++)
	{
		if (pIndex->Index[i] != NULL)
			dwChildrenCount++;
	}

	if (dwChildrenCount > 30)
		bIndexStoreType = cBitmap;
	else
		bIndexStoreType = cArray;

	error = _Write(pIOCtx, &bIndexStoreType, sizeof(bIndexStoreType));
	
	switch(bIndexStoreType)
	{
	case cFull:
		error = _Write(pIOCtx, pIndex, sizeof(NODEDATA_P_INDEX32));
		break;
	case cBitmap:
		for (i=0; i<(sizeof(pIndex->Index)/sizeof(pIndex->Index[0]));i++)
		{
			dw <<= 1;
			if (pIndex->Index[i] != NULL)
				dw |= 1;
			if (i%32 == 31)
			{
				error = _Write(pIOCtx, &dw, sizeof(uint32_t));
				RET_ON_FAIL;
				dw = 0;
			}
		}
		break;
	case cArray:
		error = _Write(pIOCtx, (uint8_t*)&dwChildrenCount, sizeof(uint8_t));
		RET_ON_FAIL;
		for (i=0; i<(sizeof(pIndex->Index)/sizeof(pIndex->Index[0]));i++)
		{
			if (pIndex->Index[i] != NULL)
			{
				error = _Write(pIOCtx, (uint8_t*)&i, sizeof(uint8_t));
				RET_ON_FAIL;
			}
		}
		break;
	}

	for (i=0; i<(sizeof(pIndex->Index)/sizeof(pIndex->Index[0]));i++)
	{
		if (pIndex->Index[i] != NULL)
		{
			error = TreeSaveNodeItself(pIndex->Index[i], 0, pIOCtx);
			RET_ON_FAIL;
		}
	}

	for (i=0; i<(sizeof(pIndex->Index)/sizeof(pIndex->Index[0]));i++)
	{
		if (pIndex->Index[i] != NULL)
		{
			error = TreeSaveNode(pIndex->Index[i], pIOCtx);
			RET_ON_FAIL;
		}
	}

	return error;
}

// ------------------------------------------------------------------------------------------------

TREE_ERR TreeSaveNodeConditions(DNODE* pNode, sIO_CTX* pIOCtx)
{
	TREE_ERR error = TREE_OK;
	DCONDITION* pCondition = NULL;

	pCondition = pNode->pCondition;
	while (pCondition!=NULL)
	{
		uint8_t bBitFlags = 0;

		if (pCondition->pNextCondition)
			bBitFlags |= 1<<0;

		error = TreeSaveNodeItself(&pCondition->Node, bBitFlags, pIOCtx);
		RET_ON_FAIL;
		
		error = _Write(pIOCtx, &pCondition->C.ConditionType, sizeof(pCondition->C.ConditionType));
		RET_ON_FAIL;
		error = _Write(pIOCtx, &pCondition->C.Data, sizeof(pCondition->C.Data));
		RET_ON_FAIL;
		pCondition = pCondition->pNextCondition;
	}
	pCondition = pNode->pCondition;
	while (pCondition!=NULL)
	{
		error = TreeSaveNode(&(pCondition->Node), pIOCtx);
		RET_ON_FAIL;
		pCondition = pCondition->pNextCondition;
	}

	return error;
}

// ------------------------------------------------------------------------------------------------

TREE_ERR TreeSaveNode(DNODE* pNode, sIO_CTX* pIOCtx)
{
	TREE_ERR error = TREE_OK;

	error = TreeSaveNodeConditions(pNode, pIOCtx);

	switch(pNode->N.NodeType.NodeTypeFields.NodeDataType)
	{
	case NDT_P_List:
		error = TreeSaveNodeDataList(pNode, pIOCtx);
		break;
	case NDT_P_Index32:
		error = TreeSaveNodeDataIndexed(pNode, pIOCtx);
		break;
	case NDT_P_Joined:
		error = TreeSaveNodeDataJoined(pNode, pIOCtx);
		break;
	case NDT_P_JoinedRT:
		ODS(("TreeSaveNode: Run-Time node in tree???"));
		DBG_STOP;
		break;
	case NDT_P_Hashed:
		error = TreeSaveNodeDataHashed(pNode, pIOCtx);
		break;
	case NDT_P_HashedRT:
		ODS(("TreeSaveNode: Run-Time node in tree???"));
		DBG_STOP;
		break;
	default:
		ODSN(("TreeSaveNode: Unsupported node type"));
		DBG_STOP;
		return TREE_EUNEXPECTED;
	}
	return error;
}

// ------------------------------------------------------------------------------------------------

TREE_ERR _TREE_CALL TreeSave_Dynamic(sRT_CTX* pRTCtx, sIO_CTX* pIOCtx)
{
	TREE_ERR error = TREE_OK;
	DNODE* pRoot = PDNODE (pRTCtx->pRoot);
	uint64_t qwStartPos;
	
	if (pRTCtx == NULL)
		return TREE_EINVAL;
	if (pRTCtx->sTreeInfo.dwMagic != MAGIC_TREE_DB_V2)
		return TREE_ECORRUPT;
	if (pRTCtx->sTreeInfo.dwFormat != cFORMAT_DYNAMIC)
		return TREE_ECORRUPT;

	error = _Seek(pIOCtx, &qwStartPos, 0, TREE_IO_CTX_SEEK_CUR);
	if (TREE_SUCCEEDED(error))
		error = _Write(pIOCtx, &pRTCtx->sTreeInfo, sizeof(pRTCtx->sTreeInfo));
	if (TREE_SUCCEEDED(error))
		error = TreeSaveNodeItself(pRoot, 0, pIOCtx);
	if (TREE_SUCCEEDED(error))
		error = TreeSaveNode(pRoot, pIOCtx);
	if (TREE_FAILED(error))
		_Seek(pIOCtx, NULL, qwStartPos, TREE_IO_CTX_SEEK_SET);

	return error;
}

// ------------------------------------------------------------------------------------------------

