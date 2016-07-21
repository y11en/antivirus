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
	if (!bInit)
		return true;
	memset(&hdr, 0, sizeof(hdr));
	memcpy(hdr.Magic, PLAINDB_MAGIC, sizeof(PLAINDB_MAGIC));
	hdr.nHeaderSize = sizeof(tPLAINDB_HEADER);
	hdr.m_nRecordSize = nRecordSize;
	if (!pFile->SeekWrite(0, &hdr, sizeof(hdr), 0))
		return false;
	return true;
}

bool cPlainDB::Close()
{
	if (m_pFile)
	{
		m_pFile->Close();
		m_pFile = NULL;
	}
	return true;
}

size_t cPlainDB::GetRecordCount()
{

	AUTO_LOCK_R();
	uint64 nSize;
	if (!m_pFile->GetSize(&nSize))
		return 0;
	return (size_t)((nSize - sizeof(tPLAINDB_HEADER)) / m_nRecordSize);
}

bool cPlainDB::AddRecord(void* pData, size_t nDataSize, tRECID* prec_id )
{
	uint64 pos;
	if (!pData)
		return false;
	if (nDataSize != m_nRecordSize)
		return false;
	AUTO_LOCK_W();
	if (!m_pFile->GetSize(&pos))
		return false;
	pos -= sizeof(tPLAINDB_HEADER);
	pos -= (pos % m_nRecordSize); // align
	if (prec_id)
		*prec_id = (tRECID)(pos / m_nRecordSize);
	pos += sizeof(tPLAINDB_HEADER);
	return m_pFile->SeekWrite(pos, pData, nDataSize, 0);
}

bool cPlainDB::GetRecord(tRECID rec_id, void* pDataBuff, size_t nDataBuffSize, size_t* pnDataSize)
{
	if (pnDataSize)
		*pnDataSize = m_nRecordSize;
	if (!pDataBuff)
		return false;
	if (nDataBuffSize != m_nRecordSize)
		return false;
	AUTO_LOCK_R();
	return m_pFile->SeekRead((uint64)rec_id * m_nRecordSize + sizeof(tPLAINDB_HEADER), pDataBuff, nDataBuffSize, 0);
}

bool cPlainDB::UpdateRecord(tRECID rec_id, void* pData, size_t nDataSize)
{
	if (!pData || nDataSize != m_nRecordSize)
		return false;
	AUTO_LOCK_W();
	return m_pFile->SeekWrite((uint64)rec_id * m_nRecordSize + sizeof(tPLAINDB_HEADER), pData, nDataSize, 0);
}

