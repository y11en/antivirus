#ifndef __PLAINDB_H_
#define __PLAINDB_H_

#include "rdb_types.h"

#include <Abstract/al_file.h>

class cPlainDB {
public:
	cPlainDB();
	~cPlainDB();
	bool Init(alFile* pFile, uint32 nRecordSize);
	bool Close();

	bool AddRecord(const void* pData, size_t nDataSize, uint64* rec_id );
	bool UpdateRecord(uint64 rec_id, const void* pData, size_t nDataSize);
	bool GetRecord(uint64 rec_id, void* pDataBuff, size_t nDataBuffSize, size_t* pnDataSize);
	bool GetRecords(uint64 rec_id, size_t nRecordsCount, void* pDataBuff, size_t nDataBuffSize, size_t* pnRecordsRead);
	uint64 GetRecordCount();
	uint64 GetSize();
	
private:
	alFile* m_pFile;
	size_t m_nRecordSize;
	uint64 m_nRecordCount;
};


#endif __PLAINDB_H_
