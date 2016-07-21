#include <memory.h>

#include "plaindb.h"

const unsigned char PLAINDB_MAGIC[4] = {'P','L','D','B'};

#define AUTO_LOCK_W()
#define AUTO_LOCK_R()

// file structures
#include <pshpack1.h>
typedef struct tag_PLAINDB_HEADER 
{
	unsigned char    Magic[4];
	uint32           nHeaderSize;
	uint32           m_nRecordSize;
} tPLAINDB_HEADER;

#include <poppack.h>

cPlainDB::cPlainDB()
{
	m_pFile = NULL;
}

cPlainDB::~cPlainDB()
{
	Close();
}

bool cPlainDB::Init(alFile* pFile, uint32 nRecordSize)
{
	tPLAINDB_HEADER hdr;
	bool bInit = false;
	m_pFile = pFile;
	m_nRecordSize = nRecordSize;
	if (!pFile || !nRecordSize)
		return false;
	if (!pFile->SeekRead(0, &hdr, sizeof(hdr), 0) )
		bInit = true;
	else if (0!=memcmp(hdr.Magic, PLAINDB_MAGIC, sizeof(PLAINDB_MAGIC)))
		bInit = true;
	else if (hdr.nHeaderSize != sizeof(tPLAINDB_HEADER))
		bInit = true;
	else if (hdr.m_nRecordSize != nRecordSize)
		bInit = true;
	else if (!m_pFile->GetSize(&m_nRecordCount))
		bInit = true;
	if (!bInit)
	{
		m_nRecordCount = (m_nRecordCount - sizeof(tPLAINDB_HEADER)) / m_nRecordSize;
		return true;
	}
	m_nRecordCount = 0;
	memset(&hdr, 0, sizeof(hdr));
	memcpy(hdr.Magic, PLAINDB_MAGIC, sizeof(PLAINDB_MAGIC));
	hdr.nHeaderSize = sizeof(tPLAINDB_HEADER);
	hdr.m_nRecordSize = nRecordSize;
	if (!pFile->SetSize(0))
		return false;
	if (!pFile->SeekWrite(0, &hdr, sizeof(hdr), 0))
		return false;
	return true;
}

bool cPlainDB::Close()
{
	if (m_pFile)
	{
		m_pFile->Release();
		m_pFile = NULL;
	}
	return true;
}

uint64 cPlainDB::GetRecordCount()
{

	AUTO_LOCK_R();
	return m_nRecordCount;
}

uint64 cPlainDB::GetSize()
{
	
	AUTO_LOCK_R();
	return m_nRecordCount * m_nRecordSize;
}


bool cPlainDB::AddRecord(const void* pData, size_t nDataSize, uint64* prec_id )
{
	uint64 pos;
	if (!pData)
		return false;
	if (nDataSize != m_nRecordSize)
		return false;
	AUTO_LOCK_W();
	if (!m_pFile->GetSize(&pos))
		return false;
	if (pos < sizeof(tPLAINDB_HEADER))
	{
		// db corrupted
		// TODO: re-init header
		pos = 0;
	}
	else
	{
		pos -= sizeof(tPLAINDB_HEADER);
		pos -= (pos % m_nRecordSize); // align
	}
	uint64 nRecords = (uint64)(pos / m_nRecordSize);
	if (nRecords >= 0xFFFFFFFF)
		return false;
	if (!m_pFile->SeekWrite(pos + sizeof(tPLAINDB_HEADER), pData, nDataSize, 0))
		return false;
	m_nRecordCount = nRecords+1;
	if (prec_id)
		*prec_id = nRecords;
	return true;
}

bool cPlainDB::GetRecord(uint64 rec_id, void* pDataBuff, size_t nDataBuffSize, size_t* pnDataSize)
{
	if (pnDataSize)
		*pnDataSize = m_nRecordSize;
	if (!pDataBuff)
		return false;
	if (nDataBuffSize != m_nRecordSize)
		return false;
	AUTO_LOCK_R();
	if ((uint64)rec_id >= m_nRecordCount)
		return false;
	return m_pFile->SeekRead((uint64)rec_id * m_nRecordSize + sizeof(tPLAINDB_HEADER), pDataBuff, nDataBuffSize, 0);
}

bool cPlainDB::GetRecords(uint64 rec_id, size_t nRecordsCount, void* pDataBuff, size_t nDataBuffSize, size_t* pnRecordsRead)
{
	size_t nDataRead;
	size_t nRecordsRead;
	if (!pDataBuff)
		return false;
//	if (nDataBuffSize != m_nRecordSize)
//		return false;
	AUTO_LOCK_R();
	m_pFile->SeekRead((uint64)rec_id * m_nRecordSize + sizeof(tPLAINDB_HEADER), pDataBuff, nDataBuffSize, &nDataRead);
	nRecordsRead = nDataRead / m_nRecordSize;
	if (pnRecordsRead)
		*pnRecordsRead = nRecordsRead;
	if (!nRecordsRead)
		return false;
	return true;
}

bool cPlainDB::UpdateRecord(uint64 rec_id, const void* pData, size_t nDataSize)
{
	if (!pData || nDataSize != m_nRecordSize)
		return false;
	AUTO_LOCK_W();
	return m_pFile->SeekWrite((uint64)rec_id * m_nRecordSize + sizeof(tPLAINDB_HEADER), pData, nDataSize, 0);
}

