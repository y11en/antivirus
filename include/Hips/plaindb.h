#ifndef __PLAINDB_H_
#define __PLAINDB_H_

#include "db_types.h"
typedef uint32   tRECID;

#include "..\Abstract\al_file.h"

class cPlainDB {
public:
	cPlainDB();
	~cPlainDB();
	bool Init(alFile* pFile, uint32 nRecordSize);
	bool Close();

	size_t GetRecordCount();
	bool AddRecord(void* pData, size_t nDataSize, tRECID* rec_id );
	bool GetRecord(tRECID rec_id, void* pDataBuff, size_t nDataBuffSize, size_t* pnDataSize);
	bool UpdateRecord(tRECID rec_id, void* pData, size_t nDataSize);

private:
	alFile* m_pFile;
	size_t m_nRecordSize;
};


#endif __PLAINDB_H_
