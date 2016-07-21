#include "objidindex.h"

#define OBJECT_ID_SIZE 16
#define VOLUME_ID_SIZE 16

//#define _USE_CACHE

#if !defined(countof)
#define countof(arr) (sizeof(arr)/sizeof(arr[0]))
#endif

typedef struct tag_ID_LIST_ITEM {
	BYTE   Id[OBJECT_ID_SIZE];
	union {
		PVOID pData;
		size_t  dwData;
	};
	struct tag_ID_LIST_ITEM* pNext;
	size_t  LastUsed;
} ID_LIST_ITEM;

typedef struct tag_OBJID_CTX {
	tFile* pFileData;
	tFile* pFileIndex;
	tHeap* pHeap;
	ID_LIST_ITEM* VolumeListHead;
	PVOID pSync;             // synchronization object
	size_t nCurrentDate;       // current date for LRU tracking
	tFileSize nDataSize;          // current data file size = next range offset
	size_t nRecordSize;        // record size in bytes
	size_t nRecordsPerCluster; // records per cluster (1-256)
#if defined(_USE_CACHE)
	size_t dwCacheOffset;
	size_t dwCacheSize;
	size_t dwCacheDataSize;
	BYTE* pCache;
#endif
} OBJID_CTX;

typedef struct tag_OBJID_VOLUME_DATA {
	BYTE VolumeId[VOLUME_ID_SIZE];
	ID_LIST_ITEM* pRangesList[256];
} OBJID_VOLUME_DATA;

#define OBJID_DATA_FILE_SIGN "FIDD"

typedef struct tag_OBJID_DATA_FILE_HDR {
	BYTE  Signature[4];
	size_t Magic;        // for data/index consistency verification
	size_t DataSize;     // last known file size
	size_t RecordSize;   // record size in bytes
	size_t RecordsPerCluster;
	size_t Reserved[3];
} OBJID_DATA_FILE_HDR;

#define OBJID_INDEX_FILE_SIGN "FIDI"

typedef struct tag_OBJID_INDEX_FILE_HDR {
	BYTE  Signature[4];
	size_t Magic;        // for data/index consistency verification
	size_t VolumeIdCount;
	size_t DataSize;     // last known file size
	size_t RecordSize;   // record size in bytes
	size_t RecordsPerCluster;
	size_t Reserved[2];
} OBJID_INDEX_FILE_HDR;

static OBJID_VOLUME_DATA* GetVolumeData(OBJID_CTX* pObjIdCtx, const BYTE VolumeId[VOLUME_ID_SIZE], bool bAdd)
{
	ID_LIST_ITEM* pItem = pObjIdCtx->VolumeListHead;
	ID_LIST_ITEM* pItemPrev = NULL;
	OBJID_VOLUME_DATA* pVolumeData;
	while (pItem)
	{
		if (!memcmp(pItem->Id, VolumeId, VOLUME_ID_SIZE))
			break;
		pItem = pItem->pNext;
	}
	if (!pItem) 
	{
		if (!bAdd)
			return NULL;
		// not found - add
		if (!pObjIdCtx->pHeap->Alloc(pObjIdCtx->pHeap, sizeof(ID_LIST_ITEM), &pItem))
			return NULL;
		if (!pObjIdCtx->pHeap->Alloc(pObjIdCtx->pHeap, sizeof(OBJID_VOLUME_DATA), &pVolumeData))
		{
			pObjIdCtx->pHeap->Free(pObjIdCtx->pHeap, pItem);
			return NULL;
		}
		memcpy(pItem->Id, VolumeId, VOLUME_ID_SIZE);
		memcpy(pVolumeData->VolumeId, VolumeId, VOLUME_ID_SIZE);
		pItem->pData = pVolumeData;
		pItem->pNext = pObjIdCtx->VolumeListHead;
		pObjIdCtx->VolumeListHead = pItem;
	}
	else
	{
		pVolumeData = (OBJID_VOLUME_DATA*)pItem->pData;
		if (pItemPrev)
		{
			// move to head
			pItemPrev->pNext = pItem->pNext;
			pItem->pNext = pObjIdCtx->VolumeListHead;
			pObjIdCtx->VolumeListHead = pItem;
		}
	}
	pItem->LastUsed = pObjIdCtx->nCurrentDate;
	return pVolumeData;
}

static BYTE __inline GetKeyByObjectId(const BYTE ObjectId[OBJECT_ID_SIZE])
{
	return ObjectId[1];
}

static size_t GetObjectDataOffset(OBJID_CTX* pObjIdCtx, const BYTE VolumeId[VOLUME_ID_SIZE], const BYTE ObjectId[OBJECT_ID_SIZE], bool bAdd)
{
	ID_LIST_ITEM* pItem = NULL;
	ID_LIST_ITEM* pItemPrev = NULL;
	OBJID_VOLUME_DATA* pVolumeData;
	unsigned int nClusterOffset;
	BYTE _ObjectId[OBJECT_ID_SIZE];
	unsigned int nBitMask = pObjIdCtx->nRecordsPerCluster - 1;
	size_t dwOffset;
	char zero_buff[64];
	
	memcpy(_ObjectId, ObjectId, OBJECT_ID_SIZE);
	nClusterOffset = _ObjectId[0] & nBitMask;
	_ObjectId[0] &= ~nBitMask; // drop cluster offset from id

	// get volume data
	pVolumeData = GetVolumeData(pObjIdCtx, VolumeId, bAdd);
	if (!pVolumeData)
		return 0;

	// get range
	pItem = pVolumeData->pRangesList[GetKeyByObjectId(ObjectId)];
	while (pItem)
	{
		if (!memcmp(pItem->Id, _ObjectId, OBJECT_ID_SIZE))
			break;
		pItem = pItem->pNext;
	}
	
	if (!pItem) 
	{
		if (!bAdd)
			return 0;

		// not found - add
		if (!pObjIdCtx->pHeap->Alloc(pObjIdCtx->pHeap, sizeof(ID_LIST_ITEM), &pItem))
			return 0;

		memcpy(pItem->Id, _ObjectId, OBJECT_ID_SIZE);
		// allocate new range data offset
		pItem->dwData = pObjIdCtx->nDataSize;
		dwOffset = pItem->dwData;
		pObjIdCtx->nDataSize += pObjIdCtx->nRecordSize * pObjIdCtx->nRecordsPerCluster;
		// zero cluster data
		memset(zero_buff, 0, sizeof(zero_buff));
		while (dwOffset < pObjIdCtx->nDataSize)
		{
			size_t size = min(pObjIdCtx->nDataSize - dwOffset, sizeof(zero_buff));
			pObjIdCtx->pFileData->SeekWrite(pObjIdCtx->pFileData, dwOffset, zero_buff, size, NULL);
			dwOffset += size;
		}
		pItem->pNext = pVolumeData->pRangesList[GetKeyByObjectId(ObjectId)];
		pVolumeData->pRangesList[GetKeyByObjectId(ObjectId)] = pItem;
	}
	else
	{
		if (pItemPrev)
		{
			// move to head
			pItemPrev->pNext = pItem->pNext;
			pItem->pNext = pVolumeData->pRangesList[GetKeyByObjectId(ObjectId)];
			pVolumeData->pRangesList[GetKeyByObjectId(ObjectId)] = pItem;
		}
	}

	pItem->LastUsed = pObjIdCtx->nCurrentDate;
	dwOffset = pItem->dwData + nClusterOffset*pObjIdCtx->nRecordSize;

	return dwOffset;
}

static size_t GetListCount(ID_LIST_ITEM* pListHead)
{
	size_t count = 0;
	ID_LIST_ITEM* pItem = pListHead;
	while (pItem)
	{
		count++;
		pItem = pItem->pNext;
	}
	return count;
}

static HRESULT SaveIndex(OBJID_CTX* pObjIdCtx)
{
	OBJID_INDEX_FILE_HDR IndexHdr;
	OBJID_DATA_FILE_HDR  DataHdr;
	ID_LIST_ITEM* pItem;

	// save index
	memset(&IndexHdr, 0, sizeof(IndexHdr));
	memcpy(&IndexHdr.Signature, OBJID_INDEX_FILE_SIGN, 4);
	IndexHdr.Magic = (size_t) GetCurTimeFast();
	IndexHdr.VolumeIdCount = GetListCount(pObjIdCtx->VolumeListHead);
	IndexHdr.DataSize = pObjIdCtx->nDataSize;
	IndexHdr.RecordSize = pObjIdCtx->nRecordSize;
	IndexHdr.RecordsPerCluster = pObjIdCtx->nRecordsPerCluster;

	if (!pObjIdCtx->pFileIndex->Seek(pObjIdCtx->pFileIndex, 0, SEEK_SET, NULL))
		return ERROR_SEEK;

	if (!pObjIdCtx->pFileIndex->Write(pObjIdCtx->pFileIndex, &IndexHdr, sizeof(IndexHdr), NULL))
		return ERROR_WRITE_FAULT;
	pItem = pObjIdCtx->VolumeListHead;
	while (pItem)
	{
		if (!pObjIdCtx->pFileIndex->Write(pObjIdCtx->pFileIndex, pItem->Id, OBJECT_ID_SIZE, NULL))
			return ERROR_WRITE_FAULT;
		if (!pObjIdCtx->pFileIndex->Write(pObjIdCtx->pFileIndex, &pItem->LastUsed, sizeof(pItem->LastUsed), NULL))
			return ERROR_WRITE_FAULT;
		{
			OBJID_VOLUME_DATA* pVolumeData;
			int i;

			pVolumeData = (OBJID_VOLUME_DATA*)pItem->pData;
			for (i=0; i<countof(pVolumeData->pRangesList); i++)
			{
				ID_LIST_ITEM* pItem2;
				size_t count;
				pItem2 = pVolumeData->pRangesList[i];
				count = GetListCount(pItem2);
				if (!pObjIdCtx->pFileIndex->Write(pObjIdCtx->pFileIndex, &count, sizeof(count), NULL))
					return ERROR_WRITE_FAULT;
				while (pItem2)
				{
					if (!pObjIdCtx->pFileIndex->Write(pObjIdCtx->pFileIndex, pItem2->Id, OBJECT_ID_SIZE, NULL))
						return ERROR_WRITE_FAULT;
					if (!pObjIdCtx->pFileIndex->Write(pObjIdCtx->pFileIndex, &pItem2->LastUsed, sizeof(pItem2->LastUsed), NULL))
						return ERROR_WRITE_FAULT;
					if (!pObjIdCtx->pFileIndex->Write(pObjIdCtx->pFileIndex, &pItem2->dwData, sizeof(pItem2->dwData), NULL))
						return ERROR_WRITE_FAULT;
					pItem2 = pItem2->pNext;
				}
			}
		}
		pItem = pItem->pNext;
	}

	// update data file header
	memset(&DataHdr, 0, sizeof(DataHdr));
	memcpy(&DataHdr.Signature, OBJID_DATA_FILE_SIGN, 4);
	DataHdr.Magic = IndexHdr.Magic;
	DataHdr.DataSize = pObjIdCtx->nDataSize;
	DataHdr.RecordSize = pObjIdCtx->nRecordSize;
	DataHdr.RecordsPerCluster = pObjIdCtx->nRecordsPerCluster;

	if (!pObjIdCtx->pFileData->Seek(pObjIdCtx->pFileData, 0, SEEK_SET, NULL))
		return ERROR_SEEK;
	if (!pObjIdCtx->pFileData->Write(pObjIdCtx->pFileData, &DataHdr, sizeof(DataHdr), NULL))
		return ERROR_WRITE_FAULT;
	
	return ERROR_SUCCESS;
}

static HRESULT LoadIndex(OBJID_CTX* pObjIdCtx)
{
	OBJID_INDEX_FILE_HDR IndexHdr;
	OBJID_DATA_FILE_HDR  DataHdr;
	ID_LIST_ITEM* pItem;
	OBJID_VOLUME_DATA* pVolumeData;
	size_t i;

	// get index file header
	memset(&IndexHdr, 0, sizeof(IndexHdr));
	if (!pObjIdCtx->pFileIndex->Seek(pObjIdCtx->pFileIndex, 0, SEEK_SET, NULL))
		return ERROR_SEEK;
	if (!pObjIdCtx->pFileIndex->Read(pObjIdCtx->pFileIndex, &IndexHdr, sizeof(IndexHdr), NULL))
		return ERROR_READ_FAULT;
	if (0 != memcmp(&IndexHdr.Signature, OBJID_INDEX_FILE_SIGN, 4))
		return ERROR_FILE_INVALID;
	// get data file header
	memset(&DataHdr, 0, sizeof(DataHdr));
	if (!pObjIdCtx->pFileData->Seek(pObjIdCtx->pFileData, 0, SEEK_SET, NULL))
		return ERROR_SEEK;
	if (!pObjIdCtx->pFileData->Read(pObjIdCtx->pFileData, &DataHdr, sizeof(DataHdr), NULL))
		return ERROR_READ_FAULT;
	if (0 != memcmp(&DataHdr.Signature, OBJID_DATA_FILE_SIGN, 4))
		return ERROR_FILE_INVALID;
	if ((DataHdr.Magic != IndexHdr.Magic) 
		|| (DataHdr.DataSize != IndexHdr.DataSize)
		|| (DataHdr.RecordsPerCluster != IndexHdr.RecordsPerCluster))
		return ERROR_FILE_INVALID;
	
	if (!pObjIdCtx->nRecordSize)
		pObjIdCtx->nRecordSize = DataHdr.RecordSize;
	else
	{
		if (pObjIdCtx->nRecordSize != DataHdr.RecordSize)	// user want work with new size. recreate file
			return ERROR_FILE_INVALID;
	}

	pObjIdCtx->nDataSize = DataHdr.DataSize;
	pObjIdCtx->nRecordsPerCluster = DataHdr.RecordsPerCluster;

	for (i=0; i<IndexHdr.VolumeIdCount; i++)
	{
		if (!pObjIdCtx->pHeap->Alloc(pObjIdCtx->pHeap, sizeof(ID_LIST_ITEM), &pItem))
			return ERROR_NOT_ENOUGH_MEMORY;
		if (!pObjIdCtx->pHeap->Alloc(pObjIdCtx->pHeap, sizeof(OBJID_VOLUME_DATA), &pVolumeData))
			return ERROR_NOT_ENOUGH_MEMORY;
		if (!pObjIdCtx->pFileIndex->Read(pObjIdCtx->pFileIndex, &pItem->Id, OBJECT_ID_SIZE, NULL))
			return ERROR_READ_FAULT;
		if (!pObjIdCtx->pFileIndex->Read(pObjIdCtx->pFileIndex, &pItem->LastUsed, sizeof(pItem->LastUsed), NULL))
			return ERROR_READ_FAULT;

		memcpy(pVolumeData->VolumeId, pItem->Id, VOLUME_ID_SIZE);
		pItem->pData = pVolumeData;
		pItem->pNext = pObjIdCtx->VolumeListHead;
		pObjIdCtx->VolumeListHead = pItem;
		{
			int i;
			for (i=0; i<countof(pVolumeData->pRangesList); i++)
			{
				ID_LIST_ITEM* pItem2;
				size_t count;
				if (!pObjIdCtx->pFileIndex->Read(pObjIdCtx->pFileIndex, &count, sizeof(count), NULL))
					return ERROR_READ_FAULT;
				for (; count>0; count--)
				{
					if (!pObjIdCtx->pHeap->Alloc(pObjIdCtx->pHeap, sizeof(ID_LIST_ITEM), &pItem2))
						return ERROR_NOT_ENOUGH_MEMORY;
					if (!pObjIdCtx->pFileIndex->Read(pObjIdCtx->pFileIndex, &pItem2->Id, OBJECT_ID_SIZE, NULL))
						return ERROR_READ_FAULT;
					if (!pObjIdCtx->pFileIndex->Read(pObjIdCtx->pFileIndex, &pItem2->LastUsed, sizeof(pItem2->LastUsed), NULL))
						return ERROR_READ_FAULT;
					if (!pObjIdCtx->pFileIndex->Read(pObjIdCtx->pFileIndex, &pItem2->dwData, sizeof(pItem2->dwData), NULL))
						return ERROR_READ_FAULT;
					pItem2->pNext = pVolumeData->pRangesList[i];
					pVolumeData->pRangesList[i] = pItem2;
				}
			}
		}
	}

	return ERROR_SUCCESS;
}


HRESULT OBJID_Init(tHeap* pHeap, tFile* pFileData, tFile* pFileIndex, size_t dwRecordSize, OBJID_CTX** ppObjIdCtx, ULONG* pCurrentItemSize)
{
	HRESULT hres = ERROR_SUCCESS;
	OBJID_CTX* pObjIdCtx = NULL;
	if (!pHeap || !pFileData || !pFileIndex || !ppObjIdCtx)
		return ERROR_INVALID_PARAMETER;
	if (!pHeap->Alloc(pHeap, sizeof(OBJID_CTX), &pObjIdCtx))
		return ERROR_NOT_ENOUGH_MEMORY;
	pObjIdCtx->pFileData = pFileData;
	pObjIdCtx->pFileIndex = pFileIndex;
	pObjIdCtx->pHeap = pHeap;
	pObjIdCtx->nRecordSize = dwRecordSize;

	if (!pObjIdCtx->pFileData->GetSize(pObjIdCtx->pFileData, &pObjIdCtx->nDataSize))
		hres = ERROR_BAD_LENGTH;
	if (hres == ERROR_SUCCESS)
		hres = OBJID_UpdateCurrentDate(pObjIdCtx);
	if (hres == ERROR_SUCCESS)
		hres = LoadIndex(pObjIdCtx);
	if (hres == ERROR_SUCCESS)
	{
		*pCurrentItemSize = pObjIdCtx->nRecordSize;
	}
	else
	{
		if (!dwRecordSize)
			dwRecordSize = 32; 		// reinit as new database

		pObjIdCtx->VolumeListHead = NULL;
		pObjIdCtx->nRecordSize = dwRecordSize;
		pObjIdCtx->nDataSize = sizeof(OBJID_DATA_FILE_HDR);
		pObjIdCtx->nRecordsPerCluster = 64;
			
		hres = SaveIndex(pObjIdCtx);

		*pCurrentItemSize = dwRecordSize;

		if (ERROR_SUCCESS != hres)
		{
			pHeap->Free(pHeap, pObjIdCtx);
			return hres;
		}
	}
#if defined(_USE_CACHE)
	pObjIdCtx->dwCacheSize = pObjIdCtx->nRecordSize*4;//pObjIdCtx->nRecordsPerCluster;
	if (!pHeap->Alloc(pHeap, pObjIdCtx->dwCacheSize, &pObjIdCtx->pCache, '8him'))
		return ERROR_NOT_ENOUGH_MEMORY;
#endif
	*ppObjIdCtx = pObjIdCtx;
	return ERROR_SUCCESS;
}

HRESULT OBJID_Done(OBJID_CTX* pObjIdCtx, size_t dwFlags)
{
	DWORD cou;
	if (!pObjIdCtx)
		return ERROR_INVALID_PARAMETER;

	if (dwFlags & OBJID_FLAG_FLUSH)
		OBJID_Flush(pObjIdCtx);
	
	if (dwFlags & OBJID_FLAG_DONTCLOSE_FILE)
	{
	}
	else
	{
		pObjIdCtx->pFileData->Close(pObjIdCtx->pFileData);
		pObjIdCtx->pFileData = NULL;
		pObjIdCtx->pFileIndex->Close(pObjIdCtx->pFileIndex);
		pObjIdCtx->pFileIndex = NULL;
	}

	while(pObjIdCtx->VolumeListHead)
	{
		OBJID_VOLUME_DATA* pVolData;
		ID_LIST_ITEM* pVol = pObjIdCtx->VolumeListHead;
		pObjIdCtx->VolumeListHead = pObjIdCtx->VolumeListHead->pNext;

		pVolData = pVol->pData;
		for (cou = 0; cou < sizeof(pVolData->pRangesList) / sizeof(pVolData->pRangesList[0]); cou++)
		{
			ID_LIST_ITEM *pItem;
			while(pVolData->pRangesList[cou])
			{
				pItem = pVolData->pRangesList[cou];
				pVolData->pRangesList[cou] = pItem->pNext;

				pObjIdCtx->pHeap->Free(pObjIdCtx->pHeap, pItem);
			}
		}

		pObjIdCtx->pHeap->Free(pObjIdCtx->pHeap, pVol->pData );
		pObjIdCtx->pHeap->Free(pObjIdCtx->pHeap, pVol );

	}
	
	pObjIdCtx->VolumeListHead = NULL;

#if defined(_USE_CACHE)
	if (pObjIdCtx->pCache)
		pObjIdCtx->pHeap->Free(pObjIdCtx->pHeap, pObjIdCtx->pCache);
#endif
	if (pObjIdCtx->pHeap)
	{
		tHeap* pHeap = pObjIdCtx->pHeap;
		pHeap->Free(pHeap, pObjIdCtx);
	}
	
	return ERROR_SUCCESS;
}

HRESULT
OBJID_GetObjectData (
	OBJID_CTX* pObjIdCtx,
	const BYTE VolumeId[VOLUME_ID_SIZE],
	const BYTE ObjectId[OBJECT_ID_SIZE],
	PVOID pDataBuffer,
	size_t dwBufferSize,
	size_t* pdwBytesRead)
{
	size_t dwOffset;

	if (!pObjIdCtx)
		return ERROR_INVALID_PARAMETER;
	
	if (pDataBuffer == NULL && dwBufferSize)
		return ERROR_INVALID_PARAMETER;

	if (pObjIdCtx->nRecordSize > dwBufferSize)
	{
		if (pdwBytesRead)
			*pdwBytesRead = pObjIdCtx->nRecordSize;
		return ERROR_INSUFFICIENT_BUFFER;
	}

	dwOffset = GetObjectDataOffset(pObjIdCtx, VolumeId, ObjectId, FALSE);

	if (dwOffset == 0)
	{
		if (pDataBuffer && dwBufferSize)
			memset(pDataBuffer, 0, dwBufferSize);
		if (pdwBytesRead)
			*pdwBytesRead = 0;
		return ERROR_NOT_FOUND;
	}
#if defined(_USE_CACHE)
	if (dwOffset < pObjIdCtx->dwCacheOffset || dwOffset + dwBufferSize > pObjIdCtx->dwCacheOffset + pObjIdCtx->dwCacheDataSize)
	{
		pObjIdCtx->dwCacheOffset = dwOffset;
		pObjIdCtx->dwCacheDataSize = 0;
		pObjIdCtx->pFileData->SeekRead(pObjIdCtx->pFileData, dwOffset, pObjIdCtx->pCache, pObjIdCtx->dwCacheSize, &pObjIdCtx->dwCacheDataSize);
		if (pObjIdCtx->dwCacheDataSize < dwBufferSize)
			return ERROR_READ_FAULT;
	}
	memcpy(pDataBuffer, pObjIdCtx->pCache + (dwOffset - pObjIdCtx->dwCacheOffset), dwBufferSize);
	if (pdwBytesRead)
		*pdwBytesRead = dwBufferSize;
#else
	if (!pObjIdCtx->pFileData->SeekRead(pObjIdCtx->pFileData, dwOffset, pDataBuffer, dwBufferSize, pdwBytesRead))
		return ERROR_READ_FAULT;
#endif
	return ERROR_SUCCESS;
}

HRESULT
OBJID_SetObjectData (
	OBJID_CTX* pObjIdCtx,
	const BYTE VolumeId[VOLUME_ID_SIZE],
	const BYTE ObjectId[OBJECT_ID_SIZE],
	PVOID pDataBuffer,
	size_t dwBufferSize,
	size_t* pdwBytesWritten
	)
{
	size_t dwOffset;
	
	if (!pObjIdCtx)
		return ERROR_INVALID_PARAMETER;

	if (pDataBuffer == NULL && dwBufferSize)
		return ERROR_INVALID_PARAMETER;

	if (pObjIdCtx->nRecordSize > dwBufferSize)
	{
		if (pdwBytesWritten)
			*pdwBytesWritten = 0;
		return ERROR_INSUFFICIENT_BUFFER;
	}

	if (pObjIdCtx->nRecordSize < dwBufferSize)
	{
		if (pdwBytesWritten)
			*pdwBytesWritten = 0;
		return ERROR_BUFFER_OVERFLOW;
	}

	dwOffset = GetObjectDataOffset(pObjIdCtx, VolumeId, ObjectId, TRUE);
	
	if (dwOffset == 0)
	{
		if (pdwBytesWritten)
			*pdwBytesWritten = 0;
		return ERROR_INTERNAL_ERROR;
	}
	if (!pObjIdCtx->pFileData->SeekWrite(pObjIdCtx->pFileData, dwOffset, pDataBuffer, dwBufferSize, pdwBytesWritten))
	{
#if defined(_USE_CACHE)
		pObjIdCtx->dwCacheDataSize = 0; // invalidate cache
#endif
		return ERROR_WRITE_FAULT;
	}
#if defined(_USE_CACHE)
	if (dwOffset + dwBufferSize < pObjIdCtx->dwCacheOffset)
		; // cache miss - unchanged
	else if (dwOffset >= pObjIdCtx->dwCacheOffset + pObjIdCtx->dwCacheDataSize)
		; // cache miss - unchanged
	else if (dwOffset >= pObjIdCtx->dwCacheOffset && dwOffset + dwBufferSize <= pObjIdCtx->dwCacheOffset + pObjIdCtx->dwCacheDataSize)
		memcpy(pObjIdCtx->pCache + (dwOffset - pObjIdCtx->dwCacheOffset), pDataBuffer, dwBufferSize); // cache hit - update data
	else
		pObjIdCtx->dwCacheDataSize = 0; // partial cache hit - invalidate cache
#endif

	return ERROR_SUCCESS;
}

HRESULT OBJID_UpdateCurrentDate(OBJID_CTX* pObjIdCtx)
{
	__int64 ct = GetCurTimeFast();
	pObjIdCtx->nCurrentDate = (size_t) ct; //dw[1];
	
	return ERROR_SUCCESS;
}

HRESULT OBJID_Flush(OBJID_CTX* pObjIdCtx)
{
	if (!pObjIdCtx)
		return ERROR_INVALID_PARAMETER;
	if (ERROR_SUCCESS != SaveIndex(pObjIdCtx))
		return ERROR_WRITE_FAULT;
	pObjIdCtx->pFileData->Flush(pObjIdCtx->pFileData);
	pObjIdCtx->pFileIndex->Flush(pObjIdCtx->pFileIndex);
	
	return ERROR_SUCCESS;
}
