#ifndef __HDB_H
#define __HDB_H

#include "db_types.h"
typedef uint64 tHDBOFFSET;
typedef uint64 tHDBHASH;

#include "al_file.h"

typedef struct tag_HDB_DATA_HDR tHDB_DATA_HDR;

#define INVALID_OFFSET (~((tHDBOFFSET)0))

class cHashDB {

public:
	cHashDB();
	~cHashDB();

	bool Init(alFile* pFile);
	bool Close();
	
	bool 
	AddHash (
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
		);

	bool 
	GetHashData (
		IN tHDBHASH nHash, 
		IN OUT OPTIONAL tHDBOFFSET* pnDataOffset, 
		OUT OPTIONAL void* pNameBuff, 
		OUT OPTIONAL size_t nNameBuffSize, 
		OUT OPTIONAL size_t* pnNameSize, 
		OUT OPTIONAL void* pExtraDataBuff, 
		OUT OPTIONAL size_t nExtraDataBuffSize, 
		OUT OPTIONAL size_t* pnExtraDataSize 
		);
	
	bool
	UpdateHashData (
		IN tHDBHASH nHash, 
		IN OPTIONAL tHDBOFFSET nDataOffset, 
		IN const void* pExtraData, 
		IN size_t nExtraDataSize
		);

private:
	tHDBOFFSET HDBGetRootNode();
	tHDBOFFSET HDBAlloc(size_t nSize);
	bool HDBAllocNode(size_t level, tHDBOFFSET* pNodeOffset);
	bool HDBAllocData(tHDBHASH nHash, const void* pName, size_t nNameSize, size_t nExtraDataSize, tHDBOFFSET* pDataOffset, tHDBOFFSET* pExtraDataOffset);
	bool HDBGetNodeEntry(tHDBOFFSET nNodeOffset, size_t nEntryNo, tHDBOFFSET* pOffset);
	bool HDBSetNodeEntry(tHDBOFFSET nNodeOffset, size_t nEntryNo, tHDBOFFSET nOffset);
	bool HDBCheckHash(tHDBOFFSET nDataOffset, tHDBHASH nHash, tHDB_DATA_HDR* pDataHeader);
	size_t HDBGetEntryNo(tHDBHASH nHash, size_t level);
	bool 
	GetHashDataEx(
		IN tHDBHASH nHash, 
		IN OUT OPTIONAL tHDBOFFSET* pnDataOffset, 
		OUT OPTIONAL void* pNameBuff, 
		OUT OPTIONAL size_t nNameBuffSize, 
		OUT OPTIONAL size_t* pnNameSize, 
		OUT OPTIONAL void* pExtraDataBuff, 
		OUT OPTIONAL size_t nExtraDataBuffSize, 
		OUT OPTIONAL size_t* pnExtraDataSize,
		IN OUT OPTIONAL tHDB_DATA_HDR* pDataHdr
		);

	alFile* m_pFile;
	uint32 m_nHeaderSize;
};

#endif // __HDB_H

