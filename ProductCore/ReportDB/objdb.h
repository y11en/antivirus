#ifndef __HDB_H
#define __HDB_H

#include "rdb_types.h"
typedef uint32 tODB_BT_OFFSET;
typedef uint32 tODB_DATA_OFFSET;

#include "lock.h"
#include "plaindb.h"
#include <Abstract/al_file.h>

#include <pshpack1.h>
typedef struct tag_ODB_ID_ENTRY
{
	tObjectHash      nObjectHash;
	tObjectId        nParentId;
	tODB_DATA_OFFSET nName;
	uint16           nNameSize;
	uint8            nFlags;
	uint8            nObjectType;
	tODB_DATA_OFFSET nDataEntriesOffset;
	uint8            nDataEntriesCount;
} tODB_ID_ENTRY;

#define MAX_DATA_ENTRIES 0x100
typedef struct tag_ODB_DATA_ENTRY
{
	uint32 nDataType;
	tODB_DATA_OFFSET nDataOffset;
	uint32 nDataSize;
} tODB_DATA_ENTRY;
#include <poppack.h>

class cObjectDB {

public:
	cObjectDB();
	~cObjectDB();

	tERROR Init(uint32 nSegment, alFile* pFileBT, alFile* pFileId, alFile* pFileData);
	tERROR Close();
	
	tERROR 
		AddObject (
		IN tObjectHash nObjectHash, 
		IN tObjectId   nParentId,
		IN uint32      nFlags, 
		IN uint8       nObjectType,
		IN const void* pName, 
		IN size_t   nNameSize, 
		OUT OPTIONAL tObjectId* pnObjectId, 
		OUT OPTIONAL bool*   pbAlreadyExist
		);

	tERROR
		SetObjectData (
		IN tObjectId nObjectId,
		IN void* pData, 
		IN size_t nDataSize,
		IN uint32 nDataType
		);
	
	tERROR 
		FindObject (
		IN tObjectHash nObjectHash, 
		OUT tObjectId* pnObjectId
		);

	tERROR
		GetObjectName (
		IN tObjectId nObjectId,
		OUT OPTIONAL void* pNameBuff, 
		IN size_t nNameBuffSize, 
		OUT OPTIONAL size_t* pnNameSize,
		OUT OPTIONAL tObjectId* pnParentId,
		OUT OPTIONAL uint8* pnFlags,
		OUT OPTIONAL uint8* pnObjectType
		);

	tERROR
		GetObjectData (
		IN tObjectId nObjectId,
		OUT OPTIONAL void* pDataBuff, 
		IN size_t nDataBuffSize, 
		OUT OPTIONAL size_t* pnDataSize,
		IN uint32 nDataType
		);

	uint64 GetSize();
	
private:
	tODB_BT_OFFSET BT_GetRootNodeOffset();
	tERROR BT_Alloc(size_t nSize, tODB_BT_OFFSET* pnOffset);
	tERROR BT_AllocNode(size_t level, tODB_BT_OFFSET* pNodeOffset);
	size_t BT_GetEntryNo(tObjectHash nObjectHash, size_t level);
	tERROR BT_GetNodeEntry(tODB_BT_OFFSET nNodeOffset, size_t nEntryNo, tODB_BT_OFFSET* pOffset);
	tERROR BT_SetNodeEntry(tODB_BT_OFFSET nNodeOffset, size_t nEntryNo, tODB_BT_OFFSET nOffset);
	
	bool   ID_CacheAdd(tObjectId nObjectId, const tODB_ID_ENTRY* pIdEntry);
	bool   ID_CacheGetObjFromHash(tObjectHash nObjectHash, tObjectId* pnObjectId, tODB_ID_ENTRY* pIdEntry);
	bool   ID_CacheGetObjFromId(tObjectId nObjectId, tObjectHash* pnObjectHash, tODB_ID_ENTRY* pIdEntry);
	bool   ID_CacheFree();

	tERROR ID_GetObject(tObjectId nObjectId, tODB_ID_ENTRY* pIdEntry, bool bCache);
	tERROR ID_AddObject(tObjectHash nObjectHash, tObjectId nParentId, uint8 nFlags, uint8 nObjectType, const void* pName, size_t nNameSize, tObjectId* pnObjectId);
	tERROR ID_UpdateObject(tObjectId nObjectId, const tODB_ID_ENTRY* pIdEntry);
	
	tERROR DATA_AllocData(const void* pData, size_t nDataSize, tODB_DATA_OFFSET* pnOffset);
	
	tERROR WriteZeroes(alFile* pFile, uint64 nOffset, size_t nSize);
private:
	uint32 m_nSegment;
	alFile* m_pFileBT;
	alFile* m_pFileData;
	cPlainDB m_IdDB;
	tODB_BT_OFFSET m_nRootNodeOffset;
	uint64 m_nSizeBT;
	uint64 m_nSizeID;
	uint64 m_nSizeDT;
	
#define ID_CACHE_SIZE 0x100
typedef uint8 tKeyId2EntryType; // this type must hold all ID_CACHE_SIZE values
	struct 
	{
		tKeyId2EntryType KeyId2Entry[ID_CACHE_SIZE];
		tODB_ID_ENTRY  IdEntry[ID_CACHE_SIZE];
		tObjectId ObjId[ID_CACHE_SIZE];
//		tODB_DATA_ENTRY* pDataEntry;
//		size_t         nDataEntries;
		size_t         Hits;
		size_t         Miss;
	} m_Cache;
	cLock m_Lock;
};

#endif // __HDB_H

