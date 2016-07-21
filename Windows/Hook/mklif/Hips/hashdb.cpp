#include <memory.h>
#include <minmax.h>

#include "hashdb.h"
#include "al_file.h"

#ifndef countof
#define countof(array) (sizeof(array)/sizeof((array)[0]))
#endif

const size_t arrNodeSizePowerOf2[] = {8,4,2,1};  // power of 2 Examples: {1} | {8,8,1} | {8,4,2,1}
#define HDB_GET_NODE_POWER2(level) (level<countof(arrNodeSizePowerOf2) ? arrNodeSizePowerOf2[level] : arrNodeSizePowerOf2[countof(arrNodeSizePowerOf2)-1])
#define HDB_GET_NODE_COUNT(level) (1<<HDB_GET_NODE_POWER2(level))
#define HDB_GET_NODE_MASK(level) ((unsigned int)(HDB_GET_NODE_COUNT(level)-1))
#define HDB_SHIFT_HASH(nHash, level) (nHash>>HDB_GET_NODE_POWER2(level))

#define MAX_LEVEL (sizeof(tHDBHASH)*8/HDB_GET_NODE_POWER2(128))

#define FLAG_NODE_OFFSET ((tHDBOFFSET)1<<(sizeof(tHDBOFFSET)*8-1))
#define IS_NODE_OFFSET(offset) ((offset & FLAG_NODE_OFFSET) == FLAG_NODE_OFFSET)
#define IS_DATA_OFFSET(offset) (offset && !IS_NODE_OFFSET(offset))

#define AUTO_LOCK_W()
#define AUTO_LOCK_R()

const unsigned char HDB_MAGIC[4] = {'H','D','B','1'};

// file structures
#include <pshpack1.h>

typedef struct tag_HDB_HEADER 
{
	unsigned char Magic[4];
	uint32        nHeaderSize;
	tHDBOFFSET    nDBSize;
} tHDB_HEADER;

typedef struct tag_HDB_DATA_HDR {
	tHDBHASH nHash;
	uint32   nDataSize;
	uint32   nExtraDataOffset;
} tHDB_DATA_HDR;

#include <poppack.h>


cHashDB::cHashDB()
{
	m_pFile = NULL;
}

cHashDB::~cHashDB()
{
	Close();
}


tHDBOFFSET cHashDB::HDBGetRootNode()
{
	return m_nHeaderSize;
}

bool cHashDB::HDBAllocNode(size_t level, tHDBOFFSET* pNodeOffset)
{
	tHDBOFFSET offsets[4] = {0};
	if (!pNodeOffset)
		return false;
	size_t node_count = HDB_GET_NODE_COUNT(level);
	tHDBOFFSET nNodeOffset = HDBAlloc(node_count*sizeof(tHDBOFFSET));
	if (!nNodeOffset)
		return false;
	if (pNodeOffset)
		*pNodeOffset = nNodeOffset;
	while (node_count)
	{
		size_t write_node_count = min(node_count, countof(offsets));
		size_t write_size = write_node_count * sizeof(tHDBOFFSET);
		if (!m_pFile->SeekWrite(nNodeOffset, offsets, write_size, 0))
			return false;
		nNodeOffset += write_size;
		node_count -= write_node_count;
	}
	return true;
}

bool cHashDB::HDBGetNodeEntry(tHDBOFFSET nNodeOffset, size_t nEntryNo, tHDBOFFSET* pOffset)
{
	size_t bytes;
	nNodeOffset &= ~FLAG_NODE_OFFSET;
	if (nNodeOffset < HDBGetRootNode())
		return false;
	return (m_pFile->SeekRead(nNodeOffset+nEntryNo*sizeof(tHDBOFFSET), pOffset, sizeof(tHDBOFFSET), &bytes) && bytes==sizeof(tHDBOFFSET));
	
}

bool cHashDB::HDBSetNodeEntry(tHDBOFFSET nNodeOffset, size_t nEntryNo, tHDBOFFSET nOffset)
{
	size_t bytes;
	nNodeOffset &= ~FLAG_NODE_OFFSET;
	if (nNodeOffset < HDBGetRootNode())
		return false;
	return (m_pFile->SeekWrite(nNodeOffset+nEntryNo*sizeof(tHDBOFFSET), &nOffset, sizeof(nOffset), &bytes) && bytes==sizeof(tHDBOFFSET));
	
}

bool cHashDB::Init(alFile* pFile)
{
	tHDB_HEADER hdr;
	bool bInit = false;
	m_pFile = pFile;
	if (!pFile)
		return false;
	if (!pFile->SeekRead(0, &hdr, sizeof(hdr), 0) )
		bInit = true;
	else if (0!=memcmp(hdr.Magic, HDB_MAGIC, sizeof(HDB_MAGIC)))
		bInit = true;
	else if(hdr.nHeaderSize != sizeof(tHDB_HEADER))
		bInit = true;
	m_nHeaderSize = hdr.nHeaderSize;
	if (!bInit)
		return true;
	memset(&hdr, 0, sizeof(hdr));
	memcpy(hdr.Magic, HDB_MAGIC, sizeof(HDB_MAGIC));
	m_nHeaderSize = hdr.nHeaderSize = sizeof(tHDB_HEADER);
	hdr.nDBSize = sizeof(tHDB_HEADER);
	if (!pFile->SeekWrite(0, &hdr, sizeof(hdr), 0))
		return false;
	tHDBOFFSET root_node;
	if (!HDBAllocNode(0, &root_node))
		return false;
	return true;
}

bool cHashDB::Close()
{
	if (m_pFile)
	{
		m_pFile->Close();
		m_pFile = NULL;
	}
	return true;
}

bool cHashDB::HDBCheckHash(tHDBOFFSET nDataOffset, tHDBHASH nHash, tHDB_DATA_HDR* pDataHeader)
{
	tHDB_DATA_HDR data_hdr;
	size_t bytes;
	if (!IS_DATA_OFFSET(nDataOffset))
		return false;
	if (!m_pFile->SeekRead(nDataOffset, &data_hdr, sizeof(data_hdr), &bytes))
		return false;
	if (bytes != sizeof(data_hdr))
		return false;
	if (pDataHeader)
		memcpy(pDataHeader, &data_hdr, sizeof(data_hdr));
	if (0 != memcmp(&data_hdr.nHash, &nHash, sizeof(nHash)))
		return false;
	return true;
}

size_t cHashDB::HDBGetEntryNo(tHDBHASH nHash, size_t level)
{
	for (size_t i=0;i<level;i++)
		nHash = HDB_SHIFT_HASH(nHash, i);
	return (size_t)(nHash & HDB_GET_NODE_MASK(level));
}

tHDBOFFSET cHashDB::HDBAlloc(size_t nSize)
{
	tHDB_HEADER hdr;
	tHDBOFFSET  result;
	if (!m_pFile->SeekRead(0, &hdr, sizeof(hdr), NULL))
		return 0;
	result = hdr.nDBSize;
	hdr.nDBSize += nSize;
	if (!m_pFile->SeekWrite(0, &hdr, sizeof(hdr), NULL))
		return 0;
	return result;
}

bool cHashDB::HDBAllocData(tHDBHASH nHash, const void* pName, size_t nNameSize, size_t nExtraDataSize, tHDBOFFSET* pDataOffset, tHDBOFFSET* pExtraDataOffset)
{
	tHDB_DATA_HDR data_hdr;
	memset(&data_hdr, 0, sizeof(data_hdr));
	data_hdr.nHash = nHash;
	data_hdr.nExtraDataOffset = sizeof(tHDB_DATA_HDR) + (uint32)nNameSize;
	data_hdr.nDataSize = sizeof(tHDB_DATA_HDR) + (uint32)nNameSize + (uint32)nExtraDataSize;
	tHDBOFFSET nDataOffset = HDBAlloc(data_hdr.nDataSize);
	if (!nDataOffset)
		return false;
	if (!m_pFile->SeekWrite(nDataOffset, &data_hdr, sizeof(data_hdr), 0))
		return false;
	if (!m_pFile->SeekWrite(nDataOffset+sizeof(data_hdr), pName, nNameSize, 0))
		return false;
	if (pDataOffset)
		*pDataOffset = nDataOffset;
	if (pExtraDataOffset)
		*pExtraDataOffset = nDataOffset + nExtraDataSize;
	return true;
}

bool 
cHashDB::AddHash(
				 IN tHDBHASH nHash, 
				 IN OPTIONAL const void* pName, 
				 IN OPTIONAL size_t   nNameSize, 
				 IN OPTIONAL const void* pExtraData, 
				 IN OPTIONAL size_t   nExtraDataSize, 
				 OUT OPTIONAL bool*   pbAlreadyExist, 
				 OUT OPTIONAL tHDBOFFSET* pDataOffset, 
				 OUT OPTIONAL void*   pExistingExtraDataBuff, 
				 OUT OPTIONAL size_t  nExistingExtraDataBuffSize, 
				 OUT OPTIONAL size_t* pnExistingExtraDataSize
				 )
{
	tHDBOFFSET node_offset, next_offset;
	size_t level;
	size_t n;
	if (!nHash)
		return false;
	if (pbAlreadyExist)
		*pbAlreadyExist = false;
	node_offset = HDBGetRootNode();
	tHDBHASH nShiftHash = nHash;
	AUTO_LOCK_R();
	for (level=0; level<MAX_LEVEL; level++)
	{
		n = HDB_GET_NODE_MASK(level);
		n &= (size_t)nShiftHash;
		nShiftHash = HDB_SHIFT_HASH(nShiftHash, level);
		if (!HDBGetNodeEntry(node_offset, n, &next_offset)) // db corrupted
			return false;
		if (IS_DATA_OFFSET(next_offset))
		{
			tHDB_DATA_HDR data_hdr = {0};
			if (GetHashDataEx(nHash, &next_offset, 0, 0, 0, pExistingExtraDataBuff, nExistingExtraDataBuffSize, pnExistingExtraDataSize, &data_hdr))
			{
				if (pbAlreadyExist)
					*pbAlreadyExist = true;
				if (pDataOffset)
					*pDataOffset = next_offset;
				return true;
			}
			if (!data_hdr.nHash)
				return false;
			tHDBOFFSET new_node;
			if (!HDBAllocNode(level+1, &new_node))
				return false;
			size_t new_entry_no = HDBGetEntryNo(data_hdr.nHash, level+1);
			if (!HDBSetNodeEntry(new_node, new_entry_no, next_offset))
				return false;
			if (!HDBSetNodeEntry(node_offset, n, new_node | FLAG_NODE_OFFSET))
				return false;
			node_offset = new_node;
			continue;
			
		}
		if (!IS_NODE_OFFSET(next_offset))
			break;
		node_offset = next_offset;
	}
	if (0 != next_offset)
		return false;
	if (!HDBAllocData(nHash, pName, nNameSize, nExtraDataSize, pDataOffset, NULL))
		return false;
	if (!HDBSetNodeEntry(node_offset, n, *pDataOffset))
		return false;
	return true;
}

bool 
cHashDB::GetHashData(
					 IN tHDBHASH nHash, 
					 IN OUT OPTIONAL tHDBOFFSET* pnDataOffset, 
					 OUT OPTIONAL void* pNameBuff, 
					 OUT OPTIONAL size_t nNameBuffSize, 
					 OUT OPTIONAL size_t* pnNameSize, 
					 OUT OPTIONAL void* pExtraDataBuff, 
					 OUT OPTIONAL size_t nExtraDataBuffSize, 
					 OUT OPTIONAL size_t* pnExtraDataSize
				    )
{
	return GetHashDataEx(nHash, pnDataOffset, pNameBuff, nNameBuffSize, pnNameSize, pExtraDataBuff, nExtraDataBuffSize, pnExtraDataSize, NULL);
}

bool 
cHashDB::GetHashDataEx(
					 IN tHDBHASH nHash, 
					 IN OUT OPTIONAL tHDBOFFSET* pnDataOffset, 
					 OUT OPTIONAL void* pNameBuff, 
					 OUT OPTIONAL size_t nNameBuffSize, 
					 OUT OPTIONAL size_t* pnNameSize, 
					 OUT OPTIONAL void* pExtraDataBuff, 
					 OUT OPTIONAL size_t nExtraDataBuffSize, 
					 OUT OPTIONAL size_t* pnExtraDataSize,
					 IN OUT OPTIONAL tHDB_DATA_HDR* pDataHdr
				    )
{
	tHDBOFFSET nDataOffset = 0;
	if (nNameBuffSize && !pNameBuff)
		return false;
	if (nExtraDataBuffSize && !pExtraDataBuff)
		return false;
	if (pnDataOffset)
		nDataOffset = *pnDataOffset;
	AUTO_LOCK_R();
	if (!nDataOffset)
	{
		tHDBOFFSET node_offset = HDBGetRootNode();
		tHDBOFFSET next_offset;
		size_t level;
		size_t n;
		tHDBHASH nShiftHash = nHash;
		for (level = 0;;level++)
		{
			n = HDB_GET_NODE_MASK(level);
			n &= (size_t)nShiftHash;
			nShiftHash = HDB_SHIFT_HASH(nShiftHash, level);
			if (!HDBGetNodeEntry(node_offset, n, &next_offset)) // db corrupted
				return false;
			if (!IS_NODE_OFFSET(next_offset))
				break;
			if (level > MAX_LEVEL)
				return false;
			node_offset = next_offset;
		}
		if (!IS_DATA_OFFSET(next_offset))
			return false;
		nDataOffset = next_offset;
	}
	tHDB_DATA_HDR data_hdr;
	bool bCheckRes = HDBCheckHash(nDataOffset, nHash, &data_hdr);
	if (pDataHdr)
		memcpy(pDataHdr, &data_hdr, sizeof(data_hdr));
	if (!bCheckRes)
		return false;
	if (pnDataOffset)
		*pnDataOffset = nDataOffset;
	size_t nNameSize = data_hdr.nExtraDataOffset - sizeof(tHDB_DATA_HDR); 
	if (pnNameSize)
		*pnNameSize = nNameSize;
	size_t nExtraDataSize = data_hdr.nDataSize - data_hdr.nExtraDataOffset;
	if (pnExtraDataSize)
		*pnExtraDataSize = nExtraDataSize;
	if (pNameBuff && nNameBuffSize)
	{
		if (!m_pFile->SeekRead(nDataOffset + sizeof(tHDB_DATA_HDR), pNameBuff, min(nNameBuffSize, nNameSize), 0))
			return false;
	}
	if (pExtraDataBuff && nExtraDataBuffSize)
	{
		if (!m_pFile->SeekRead(nDataOffset + data_hdr.nExtraDataOffset, pExtraDataBuff, min(nExtraDataBuffSize, nExtraDataSize), 0))
			return false;
	}
	return true;
}

bool 
cHashDB::UpdateHashData(
						IN tHDBHASH nHash, 
						IN OPTIONAL tHDBOFFSET nDataOffset, 
						IN const void* pExtraData, 
						IN size_t nExtraDataSize
						)
{
	tHDB_DATA_HDR data_hdr;
	if (!HDBCheckHash(nDataOffset, nHash, &data_hdr))
		return false;
	if (data_hdr.nDataSize - data_hdr.nExtraDataOffset != nExtraDataSize)
		return false;
	return m_pFile->SeekWrite(nDataOffset+data_hdr.nExtraDataOffset, pExtraData, nExtraDataSize, 0);
}
