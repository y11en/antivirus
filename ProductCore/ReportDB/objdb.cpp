#include <memory.h>
#include <minmax.h>
#include "objdb.h"
#include <Abstract/al_file.h>

#ifndef countof
#define countof(array) (sizeof(array)/sizeof((array)[0]))
#endif

//#if sizeof(tObjectId) > sizeof(tODB_BT_OFFSET)
//#pragma error The tObjectId must fit into tODB_BT_OFFSET
//#endif

const size_t arrNodeSizePowerOf2[] = {8,4,2,1};  // power of 2. Examples: {1} | {8,8,1} | {8,4,2,1}
#define BT_GET_NODE_POWER2(level) (level<countof(arrNodeSizePowerOf2) ? arrNodeSizePowerOf2[level] : arrNodeSizePowerOf2[countof(arrNodeSizePowerOf2)-1])
#define BT_GET_NODE_COUNT(level) (1<<BT_GET_NODE_POWER2(level))
#define BT_GET_NODE_MASK(level) ((unsigned int)(BT_GET_NODE_COUNT(level)-1))
#define BT_SHIFT_HASH(nObjectHash, level) (nObjectHash>>BT_GET_NODE_POWER2(level))

#define MAX_LEVEL (sizeof(tObjectHash)*8/BT_GET_NODE_POWER2(128))

#define FLAG_NODE_OFFSET ((tODB_BT_OFFSET)1<<(sizeof(tODB_BT_OFFSET)*8-1))
#define IS_NODE_OFFSET(offset) ((offset & FLAG_NODE_OFFSET) == FLAG_NODE_OFFSET)
#define IS_DATA_OFFSET(offset) (offset && !IS_NODE_OFFSET(offset))

typedef uint32 tShortObjectId;
#define MAKE_LONG_OBJECT_ID(nShortObjectId) ((((tObjectId)m_nSegment)<<32) | (nShortObjectId))
#define MAKE_SHORT_OBJECT_ID(nLongObjectId) ((tShortObjectId)(nLongObjectId))

#define AUTO_LOCK_W() cAutoLock _lock(&m_Lock, true);
#define AUTO_LOCK_R() cAutoLock _lock(&m_Lock, false);


const unsigned char BT_MAGIC[4]   = {'O','D','B','T'};
const unsigned char ID_MAGIC[4]  = {'O','D','B','I'};
const unsigned char DATA_MAGIC[4] = {'O','D','B','D'};

// file structures
#include <pshpack1.h>

typedef struct tag_BT_HEADER 
{
	unsigned char Magic[4];
	uint32        nHeaderSize;
	uint64        nDBSize;
} tBT_HEADER;

typedef struct tag_BT_DATA_HDR {
	tObjectHash nObjectHash;
	uint32   nDataSize;
	uint32   nExtraDataOffset;
} tBT_DATA_HDR;

#include <poppack.h>


cObjectDB::cObjectDB()
{
	m_pFileBT = NULL;
	m_pFileData = NULL;
	m_nRootNodeOffset = 0;
	m_nSizeBT = 0;
	m_nSizeID = 0;
	m_nSizeDT = 0;
	memset(&m_Cache, 0, sizeof(m_Cache));
}

cObjectDB::~cObjectDB()
{
	//printf("Cache hits=%d, miss=%d\n", m_Cache.Hits, m_Cache.Miss);
	m_Cache.Hits, m_Cache.Miss;
	Close();
}


tERROR cObjectDB::Init(uint32 nSegment, alFile* pFileBT, alFile* pFileId, alFile* pFileData)
{
	tBT_HEADER hdr;
	unsigned char magic[4];
	
	if (!pFileBT || !pFileId || !pFileData)
		return errPARAMETER_INVALID;
	
	m_nSegment = nSegment;
	
	if (!pFileData->SeekRead(0, magic, sizeof(magic), 0) || 0!=memcmp(magic, DATA_MAGIC, sizeof(DATA_MAGIC)))
	{
		if (!pFileData->SeekWrite(0, &DATA_MAGIC, sizeof(DATA_MAGIC), 0))
			return errOBJECT_WRITE;
	}
	
	if (!m_IdDB.Init(pFileId, sizeof(tODB_ID_ENTRY)))
		return errOBJECT_CANNOT_BE_INITIALIZED;
	
	
	if (!pFileBT->SeekRead(0, &hdr, sizeof(hdr), 0) || 0!=memcmp(hdr.Magic, BT_MAGIC, sizeof(BT_MAGIC)) || hdr.nHeaderSize != sizeof(tBT_HEADER))
	{
		memset(&hdr, 0, sizeof(hdr));
		memcpy(hdr.Magic, BT_MAGIC, sizeof(BT_MAGIC));
		m_nRootNodeOffset = hdr.nHeaderSize = sizeof(tBT_HEADER);
		hdr.nDBSize = hdr.nHeaderSize = sizeof(tBT_HEADER);
		if (!pFileBT->SeekWrite(0, &hdr, sizeof(hdr), 0))
			return errOBJECT_WRITE;
		tODB_BT_OFFSET root_node;
		m_pFileBT = pFileBT;
		tERROR error = BT_AllocNode(0, &root_node);
		if (PR_FAIL(error))
		{
			m_pFileBT = NULL;
			return error;
		}
	}
	m_pFileBT = pFileBT;
	m_pFileData = pFileData;
	m_nRootNodeOffset = hdr.nHeaderSize;
	pFileBT->GetSize(&m_nSizeBT);
	pFileData->GetSize(&m_nSizeDT);
	pFileId->GetSize(&m_nSizeID);
	return errOK;
}

tERROR cObjectDB::Close()
{
	ID_CacheFree();
	if (m_pFileBT)
	{
		m_pFileBT->Release();
		m_pFileBT = NULL;
	}
	if (m_pFileData)
	{
		m_pFileData->Release();
		m_pFileData = NULL;
	}
	return errOK;
}

uint64 cObjectDB::GetSize()
{
	AUTO_LOCK_R();
	return m_nSizeBT + m_nSizeID + m_nSizeDT;
}

tODB_BT_OFFSET cObjectDB::BT_GetRootNodeOffset()
{
	return m_nRootNodeOffset;
}

tERROR cObjectDB::BT_Alloc(size_t nSize, tODB_BT_OFFSET* pnOffset)
{
	tBT_HEADER hdr;
	if (!pnOffset)
		return errPARAMETER_INVALID;
	if (!m_pFileBT->SeekRead(0, &hdr, sizeof(hdr), NULL))
		return errOBJECT_READ;
	*pnOffset = (tODB_BT_OFFSET)hdr.nDBSize;
	hdr.nDBSize += nSize;
	m_nSizeBT = hdr.nDBSize;
	if (!m_pFileBT->SeekWrite(0, &hdr, sizeof(hdr), NULL))
		return errOBJECT_WRITE;
	return errOK;
}

tERROR cObjectDB::BT_AllocNode(size_t level, tODB_BT_OFFSET* pNodeOffset)
{
	tERROR error;
	tODB_BT_OFFSET offsets[4] = {0};
	if (!pNodeOffset)
		return errPARAMETER_INVALID;
	size_t node_count = BT_GET_NODE_COUNT(level);
	tODB_BT_OFFSET nNodeOffset;
	error = BT_Alloc(node_count*sizeof(tODB_BT_OFFSET), &nNodeOffset);
	if (PR_FAIL(error))
		return error;
	*pNodeOffset = nNodeOffset;
	while (node_count)
	{
		size_t write_node_count = min(node_count, countof(offsets));
		size_t write_size = write_node_count * sizeof(tODB_BT_OFFSET);
		if (!m_pFileBT->SeekWrite(nNodeOffset, offsets, write_size, 0))
			return errOBJECT_WRITE;
		nNodeOffset += (tODB_BT_OFFSET)write_size;
		node_count -= write_node_count;
	}
	return errOK;
}

tERROR cObjectDB::BT_GetNodeEntry(tODB_BT_OFFSET nNodeOffset, size_t nEntryNo, tODB_BT_OFFSET* pOffset)
{
	size_t bytes;
	nNodeOffset &= ~FLAG_NODE_OFFSET;
	if (nNodeOffset < BT_GetRootNodeOffset())
		return errPARAMETER_INVALID;
	if (!m_pFileBT->SeekRead(nNodeOffset+nEntryNo*sizeof(tODB_BT_OFFSET), pOffset, sizeof(tODB_BT_OFFSET), &bytes) || bytes!=sizeof(tODB_BT_OFFSET))
		return errOBJECT_READ;
	return errOK;
}

tERROR cObjectDB::BT_SetNodeEntry(tODB_BT_OFFSET nNodeOffset, size_t nEntryNo, tODB_BT_OFFSET nOffset)
{
	size_t bytes;
	nNodeOffset &= ~FLAG_NODE_OFFSET;
	if (nNodeOffset < BT_GetRootNodeOffset())
		return errPARAMETER_INVALID;
	if (!m_pFileBT->SeekWrite(nNodeOffset+nEntryNo*sizeof(tODB_BT_OFFSET), &nOffset, sizeof(nOffset), &bytes) || bytes!=sizeof(tODB_BT_OFFSET))
		return errOBJECT_WRITE;
	return errOK;
}

size_t cObjectDB::BT_GetEntryNo(tObjectHash nObjectHash, size_t level)
{
	for (size_t i=0;i<level;i++)
		nObjectHash = BT_SHIFT_HASH(nObjectHash, i);
	return (size_t)(nObjectHash & BT_GET_NODE_MASK(level));
}

#define GET_CACHE_KEY(value) ((size_t)value % ID_CACHE_SIZE)
#define USE_CACHE_KEYS

bool cObjectDB::ID_CacheGetObjFromHash(tObjectHash nObjectHash, tObjectId* pnObjectId, tODB_ID_ENTRY* pEntry)
{
	if (!nObjectHash)
		return false;
	size_t i = GET_CACHE_KEY(nObjectHash);
	if (m_Cache.IdEntry[i].nObjectHash == nObjectHash)
	{
		m_Cache.Hits++;
		if (pnObjectId)
			*pnObjectId = m_Cache.ObjId[i];
		if (pEntry)
			memcpy(pEntry, m_Cache.IdEntry+i, sizeof(tODB_ID_ENTRY));
		return true;
	}
	m_Cache.Miss++;
	return false;
}

bool cObjectDB::ID_CacheGetObjFromId(tObjectId nObjectId, tObjectHash* pnObjectHash, tODB_ID_ENTRY* pEntry)
{
	if (!nObjectId)
		return false;
	size_t i = m_Cache.KeyId2Entry[GET_CACHE_KEY(nObjectId)];
	if (nObjectId == m_Cache.ObjId[i])
	{
		m_Cache.Hits++;
		if (pnObjectHash)
			*pnObjectHash = m_Cache.IdEntry[i].nObjectHash;
		if (pEntry)
			memcpy(pEntry, m_Cache.IdEntry+i, sizeof(tODB_ID_ENTRY));
		return true;
	}
	m_Cache.Miss++;
	return false;
}

bool cObjectDB::ID_CacheFree()
{
	for (size_t i=0; i<ID_CACHE_SIZE; i++)
	{
		// free cache element
	}
	return true;
}

bool cObjectDB::ID_CacheAdd(tObjectId nObjectId, const tODB_ID_ENTRY* pEntry)
{
	size_t nEntry = GET_CACHE_KEY(pEntry->nObjectHash);
	m_Cache.ObjId[nEntry] = nObjectId;
	memcpy(&m_Cache.IdEntry[nEntry], pEntry, sizeof(tODB_ID_ENTRY));
	m_Cache.KeyId2Entry[GET_CACHE_KEY(nObjectId)] = (tKeyId2EntryType)nEntry;
	return true;
}


tERROR cObjectDB::ID_GetObject(tObjectId nObjectId, tODB_ID_ENTRY* pEntry, bool bCache)
{
	if (!pEntry)
		return errPARAMETER_INVALID;
	if (GET_SEGMENT_FROM_ID(nObjectId) != m_nSegment)
		return errOBJECT_NOT_FOUND;
	if (bCache)
	{
		if (ID_CacheGetObjFromId(nObjectId, NULL, pEntry))
			return errOK;
	}
	if (!m_IdDB.GetRecord(MAKE_SHORT_OBJECT_ID(nObjectId)-1, pEntry, sizeof(tODB_ID_ENTRY), NULL))
		return errOBJECT_READ;
	if (bCache)
		ID_CacheAdd(nObjectId, pEntry);
	return errOK;
}

tERROR cObjectDB::ID_UpdateObject(tObjectId nObjectId, const tODB_ID_ENTRY* pEntry)
{
	if (!nObjectId || !pEntry)
		return errPARAMETER_INVALID;
	if (GET_SEGMENT_FROM_ID(nObjectId) != m_nSegment)
		return errOBJECT_NOT_FOUND;
	if (!m_IdDB.UpdateRecord(MAKE_SHORT_OBJECT_ID(nObjectId)-1, pEntry, sizeof(tODB_ID_ENTRY)))
		return errOBJECT_WRITE;
	ID_CacheAdd(nObjectId, pEntry);
	return errOK;
}

tERROR cObjectDB::ID_AddObject(tObjectHash nObjectHash, tObjectId nParentId, uint8 nFlags, uint8 nObjectType, const void* pName, size_t nNameSize, tObjectId* pnObjectId)
{
	tERROR error = errOK;
	if (nNameSize > 0xFFFF)
		return errBAD_SIZE;
	tODB_ID_ENTRY object_entry = {0};
	object_entry.nObjectHash = nObjectHash;
	object_entry.nNameSize = (uint16)nNameSize;
	object_entry.nParentId = nParentId;
	object_entry.nFlags = nFlags;
	object_entry.nObjectType = nObjectType;
	if (nNameSize)
		error = DATA_AllocData(pName, nNameSize, &object_entry.nName);
	if (PR_FAIL(error))
		return error;
	uint64 objid;
	if (m_IdDB.GetRecordCount() > 0x7FFFFF0)
		return errOUT_OF_SPACE;
	if (!m_IdDB.AddRecord(&object_entry, sizeof(object_entry), &objid))
		return errOBJECT_WRITE;
	m_nSizeID += sizeof(object_entry);
	if (pnObjectId)
		*pnObjectId = MAKE_LONG_OBJECT_ID(objid+1);
	return errOK;
}

tERROR cObjectDB::DATA_AllocData(const void* pData, size_t nDataSize, tODB_DATA_OFFSET* pnOffset)
{
	uint64 nOffset;
	if (!pnOffset)
		return errPARAMETER_INVALID;
	if (!m_pFileData->GetSize(&nOffset))
		return errOBJECT_WRITE;
	if (!m_pFileData->SeekWrite(nOffset, pData, nDataSize, NULL))
		return errOBJECT_WRITE;
	m_nSizeDT += nDataSize;
	*pnOffset = (tODB_DATA_OFFSET)nOffset;
	return errOK;
}


tERROR
cObjectDB::AddObject(
	   IN tObjectHash nObjectHash, 
	   IN tObjectId   nParentId,
	   IN uint32      nFlags, 
	   IN uint8       nObjectType,
	   IN const void* pName, 
	   IN size_t      nNameSize, 
	   OUT OPTIONAL tObjectId* pnObjectId, 
	   OUT OPTIONAL bool*   pbAlreadyExist
	   )
{
	tERROR error;
	tODB_BT_OFFSET node_offset, next_offset;
	tObjectId id;
	size_t level;
	size_t n;
	if (pbAlreadyExist)
		*pbAlreadyExist = false;
	if (!nObjectHash && !pName && !nNameSize && pnObjectId) // create unnamed object
		return ID_AddObject(nObjectHash, nParentId, nFlags, nObjectType, pName, nNameSize, pnObjectId);
	if (!nObjectHash || !pName || !nNameSize)
		return errPARAMETER_INVALID;
	AUTO_LOCK_W();
	if (ID_CacheGetObjFromHash(nObjectHash, pnObjectId, NULL))
	{
		if (pbAlreadyExist)
			*pbAlreadyExist = true;
		return errOK;
	}
	node_offset = BT_GetRootNodeOffset();
	tObjectHash nShiftObjectHash = nObjectHash;
	for (level=0; level<MAX_LEVEL; level++)
	{
		n = BT_GET_NODE_MASK(level);
		n &= (size_t)nShiftObjectHash;
		nShiftObjectHash = BT_SHIFT_HASH(nShiftObjectHash, level);
		error = BT_GetNodeEntry(node_offset, n, &next_offset);
		if (PR_FAIL(error)) // db corrupted
			return error;
		if (IS_DATA_OFFSET(next_offset))
		{
			tODB_ID_ENTRY object_entry={0};
			id = MAKE_LONG_OBJECT_ID((tShortObjectId)next_offset);
			error = ID_GetObject(id, &object_entry, true);
			if (PR_FAIL(error))
				return error;
			if (object_entry.nObjectHash == nObjectHash)
			{
				if (pnObjectId)
					*pnObjectId = id;
				if (pbAlreadyExist)
					*pbAlreadyExist = true;
				return errOK;
			}
			tODB_BT_OFFSET new_node;
			error = BT_AllocNode(level+1, &new_node);
			if (PR_FAIL(error))
				return error;
			size_t new_entry_no = BT_GetEntryNo(object_entry.nObjectHash, level+1);
			error = BT_SetNodeEntry(new_node, new_entry_no, next_offset);
			if (PR_FAIL(error))
				return error;
			error = BT_SetNodeEntry(node_offset, n, new_node | FLAG_NODE_OFFSET);
			if (PR_FAIL(error))
				return error;
			node_offset = new_node;
			continue;
		}
		if (!IS_NODE_OFFSET(next_offset))
			break;
		node_offset = next_offset;
	}
	if (0 != next_offset)
		return errOBJECT_DATA_CORRUPTED;
	error = ID_AddObject(nObjectHash, nParentId, nFlags, nObjectType, pName, nNameSize, &id);
	if (PR_FAIL(error))
		return error;
	error = BT_SetNodeEntry(node_offset, n, MAKE_SHORT_OBJECT_ID(id));
	if (pnObjectId)
		*pnObjectId = MAKE_LONG_OBJECT_ID(id);
	return error;
}

tERROR 
cObjectDB::FindObject (
					   IN tObjectHash nObjectHash, 
					   OUT tObjectId* pnObjectId
					   )
{
	tERROR error;
	tODB_BT_OFFSET nDataOffset = 0;
	if (!nObjectHash || !pnObjectId)
		return errPARAMETER_INVALID;
	AUTO_LOCK_R();
	if (ID_CacheGetObjFromHash(nObjectHash, pnObjectId, NULL))
		return errOK;

	tODB_BT_OFFSET node_offset = BT_GetRootNodeOffset();
	tODB_BT_OFFSET next_offset;
	size_t level;
	size_t n;
	tObjectHash nShiftObjectHash = nObjectHash;
	for (level = 0;;level++)
	{
		n = BT_GET_NODE_MASK(level);
		n &= (size_t)nShiftObjectHash;
		nShiftObjectHash = BT_SHIFT_HASH(nShiftObjectHash, level);
		error = BT_GetNodeEntry(node_offset, n, &next_offset);
		if (PR_FAIL(error)) // db corrupted
			return error;
		if (!IS_NODE_OFFSET(next_offset))
			break;
		if (level > MAX_LEVEL)
			return errOBJECT_DATA_CORRUPTED;
		node_offset = next_offset;
	}
	if (0 == next_offset)
		return errOBJECT_NOT_FOUND;
	tObjectId id = MAKE_LONG_OBJECT_ID((tShortObjectId)next_offset);
	tODB_ID_ENTRY object_entry={0};
	error = ID_GetObject(id, &object_entry, true);
	if (PR_FAIL(error))
		return error;
	if (object_entry.nObjectHash != nObjectHash)
		return errOBJECT_NOT_FOUND;
	if (pnObjectId)
		*pnObjectId = id;
	return errOK;
}

tERROR cObjectDB::WriteZeroes(alFile* pFile, uint64 nOffset, size_t nSize)
{
	uint8 zeroes[0x200] = {0};
	while (nSize)
	{
		size_t n = min(nSize, sizeof(zeroes));
		if (!pFile->SeekWrite(nOffset, zeroes, n, NULL))
			return errOBJECT_WRITE;
		nSize -= n;
	}
	return errOK;
}

tERROR
cObjectDB::SetObjectData (
			   IN tObjectId nObjectId,
			   IN void* pData, 
			   IN size_t nDataSize,
			   IN uint32 nDataType
			   )
{
	tERROR error;
	uint32 nDataAllocated;
	tODB_ID_ENTRY object_entry;
	tODB_DATA_ENTRY data_entries[MAX_DATA_ENTRIES];
	tODB_DATA_ENTRY* pde = NULL;
	size_t nEntry;
	tODB_DATA_OFFSET old_data_offset;
	uint32           old_data_size;
	if (!nObjectId)
		return errPARAMETER_INVALID;
	if (nDataSize && !pData)
		return errPARAMETER_INVALID;
	if (nDataSize >= 0x80000000)
		return errBAD_SIZE;
	if (GET_SEGMENT_FROM_ID(nObjectId) != m_nSegment)
		return errOBJECT_NOT_FOUND;
	AUTO_LOCK_W();
	error = ID_GetObject(nObjectId, &object_entry, true);
	if (PR_FAIL(error))
		return error;
	if (object_entry.nDataEntriesCount)
	{
		if (!m_pFileData->SeekRead(object_entry.nDataEntriesOffset, data_entries, object_entry.nDataEntriesCount*sizeof(tODB_DATA_ENTRY), NULL))
			return errOBJECT_READ;
	}
	for (nEntry=0, pde=data_entries; nEntry<object_entry.nDataEntriesCount; nEntry++, pde++)
	{
		if (pde->nDataType == nDataType)
			break;
	}
	if (nEntry >= MAX_DATA_ENTRIES)
		return errOUT_OF_SPACE;
	if (nEntry >= object_entry.nDataEntriesCount)
	{
		// allocate new object_entry
		memset(pde, 0, sizeof(tODB_DATA_ENTRY));
		pde->nDataType = nDataType;
	}
	if (pde->nDataSize & 0x80000000) // special case
		nDataAllocated = (pde->nDataSize >> 16) & 0x7FFF;
	else
		nDataAllocated = pde->nDataSize; 
	old_data_offset = pde->nDataOffset; // store for cleanup
	old_data_size = nDataAllocated;
	if (nDataAllocated >= nDataSize)
	{
		// just update data
		if (!m_pFileData->SeekWrite(pde->nDataOffset, pData, nDataSize, 0))
			return errOBJECT_WRITE;
	}
	else
	{
		// allocate new data block
		error = DATA_AllocData(pData, nDataSize, &pde->nDataOffset);
		if (PR_FAIL(error))
			return error;
		nDataAllocated = nDataSize;
	}

	if (old_data_size != nDataAllocated || old_data_offset != pde->nDataOffset)
	{
		// update data entry
		if (nDataSize > 0x7FFF)
			pde->nDataSize = (uint32)nDataSize;
		else
			pde->nDataSize = 0x80000000 | nDataSize | (nDataAllocated << 16);
		if (nEntry < object_entry.nDataEntriesCount) 
		{
			// update existing data entry
			if (!m_pFileData->SeekWrite(object_entry.nDataEntriesOffset+nEntry*sizeof(tODB_DATA_ENTRY), pde, sizeof(tODB_DATA_ENTRY), 0))
				return errOBJECT_WRITE;
		}
		else
		{
			// allocate new data entry
			tODB_DATA_OFFSET old_data_entries_offset = object_entry.nDataEntriesOffset; // store offset for cleanup
			object_entry.nDataEntriesCount++;
			error = DATA_AllocData(data_entries, object_entry.nDataEntriesCount*sizeof(tODB_DATA_ENTRY), &object_entry.nDataEntriesOffset);
			if (PR_FAIL(error))
				return error;
			error = ID_UpdateObject(nObjectId, &object_entry);
			if (PR_FAIL(error))
				return error;
			// clean unused data entries
			if (old_data_entries_offset && (object_entry.nDataEntriesCount > 1))
				WriteZeroes(m_pFileData, old_data_entries_offset, (object_entry.nDataEntriesCount-1)*sizeof(tODB_DATA_ENTRY));
		}
	}
	// clean unused data 
	if (old_data_offset != pde->nDataOffset)
		WriteZeroes(m_pFileData, old_data_offset, old_data_size);
	return errOK;
}

tERROR
cObjectDB::GetObjectName (
			   IN tObjectId nObjectId,
			   OUT OPTIONAL void* pNameBuff, 
			   IN size_t nNameBuffSize, 
			   OUT OPTIONAL size_t* pnNameSize,
			   OUT OPTIONAL tObjectId* pnParentId,
			   OUT OPTIONAL uint8* pnFlags,
			   OUT OPTIONAL uint8* pnObjectType
			   )
{
	tERROR error;
	tODB_ID_ENTRY object_entry;
	if (!nObjectId)
		return errPARAMETER_INVALID;
	if (nNameBuffSize && !pNameBuff)
		return errPARAMETER_INVALID;
//	if (!pnNameSize && !nNameBuffSize)
//		return errPARAMETER_INVALID;
	if (GET_SEGMENT_FROM_ID(nObjectId) != m_nSegment)
		return errOBJECT_NOT_FOUND;
	AUTO_LOCK_R();
	error = ID_GetObject(nObjectId, &object_entry, true);
	if (PR_FAIL(error))
		return error;
	if (pnNameSize)
		*pnNameSize = object_entry.nNameSize;
	if (pnParentId)
		*pnParentId = object_entry.nParentId;
	if (pnFlags)
		*pnFlags = object_entry.nFlags;
	if (pnObjectType)
		*pnObjectType = object_entry.nObjectType;
	if (nNameBuffSize > object_entry.nNameSize)
		nNameBuffSize = object_entry.nNameSize;
	if (nNameBuffSize)
	{
		if (!m_pFileData->SeekRead(object_entry.nName, pNameBuff, nNameBuffSize, 0))
			return errOBJECT_READ;
	}
	if (nNameBuffSize > 0 && nNameBuffSize < object_entry.nNameSize)
		return errBUFFER_TOO_SMALL;
	return errOK;

}

tERROR
cObjectDB::GetObjectData (
			   IN tObjectId nObjectId,
			   OUT OPTIONAL void* pDataBuff, 
			   IN size_t nDataBuffSize, 
			   OUT OPTIONAL size_t* pnDataSize,
			   IN uint32 nDataType
			   )
{
	tERROR error;
	tODB_ID_ENTRY object_entry;
	tODB_DATA_ENTRY data_entries[MAX_DATA_ENTRIES];
	tODB_DATA_ENTRY* pde = NULL;
	size_t nEntry;
	if (!nObjectId)
		return errPARAMETER_INVALID;
	if (nDataBuffSize && !pDataBuff)
		return errPARAMETER_INVALID;
	if (!pnDataSize && !nDataBuffSize)
		return errPARAMETER_INVALID;
	if (GET_SEGMENT_FROM_ID(nObjectId) != m_nSegment)
		return errOBJECT_NOT_FOUND;
	AUTO_LOCK_R();
	error = ID_GetObject(nObjectId, &object_entry, true);
	if (PR_FAIL(error))
		return error;
	if (!object_entry.nDataEntriesCount)
		return errNOT_FOUND;
	if (!m_pFileData->SeekRead(object_entry.nDataEntriesOffset, data_entries, object_entry.nDataEntriesCount*sizeof(tODB_DATA_ENTRY), NULL))
		return errOBJECT_READ;
	for (nEntry=0, pde=data_entries; nEntry<object_entry.nDataEntriesCount; nEntry++, pde++)
	{
		if (pde->nDataType == nDataType)
			break;
	}
	if (nEntry >= object_entry.nDataEntriesCount)
		return errNOT_FOUND;
	uint32 nDataSize = pde->nDataSize;
	if (nDataSize & 0x80000000)
		nDataSize &= 0x7FFF;
	if (pnDataSize)
		*pnDataSize = nDataSize;
	if (nDataBuffSize > nDataSize)
		nDataBuffSize = nDataSize;
	if (nDataBuffSize)
	{
		if (!m_pFileData->SeekRead(pde->nDataOffset, pDataBuff, nDataBuffSize, 0))
			return errOBJECT_READ;
	}
	if (nDataBuffSize > 0 && nDataBuffSize < nDataSize)
		return errBUFFER_TOO_SMALL;
	return errOK;
}
