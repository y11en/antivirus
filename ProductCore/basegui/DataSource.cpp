// DataSource.cpp: implementation of the CDataSource1 class.
//
//////////////////////////////////////////////////////////////////////

#include "StdAfx.h"
#include "DataSource.h"

//////////////////////////////////////////////////////////////////////

CDataSource::CDataSource(CDataReader *pReader, int nCapacity) :
	m_pReader(pReader),
	m_nCacheCapacity(nCapacity),
	m_pCacheHead(NULL),
	m_pCacheTail(NULL),
	m_nCacheSize(0),
	m_nRecordCount(0)
{
}

CDataSource::CDataSource(CDataSource &c) :
	m_pCacheHead(NULL),
	m_pCacheTail(NULL),
	m_nCacheSize(0)
{
	m_pReader = c.m_pReader;
	m_nRecordCount = c.m_nRecordCount;
	m_nCacheCapacity = c.m_nCacheCapacity;

	int nSize = m_nRecordCount / RECORD_BLOCK_SIZE + 1;
	resize(nSize);

	for(int i = 0; i < nSize; i++)
	{
		CRecord * pCopyBlock = c.GetBlock(i);
		CRecord *& pBlock = at(i);
		pBlock = (CRecord *)calloc(1, RECORD_BLOCK_SIZE*sizeof(CRecord));

		for(int j = 0; j < RECORD_BLOCK_SIZE; j++)
		{
			pBlock[j].m_nPos = pCopyBlock[j].m_nPos;
			pBlock[j].m_nViewPos = i * RECORD_BLOCK_SIZE + j;
		}
	}
}

void CDataSource::Clear()
{
	ClearCache();

	for(int i = 0; i < size(); i++)
	{
		CRecord *pBlock = at(i);
		if( pBlock )
			free(pBlock);
	}
	clear();

	m_nRecordCount = 0;
}

void CDataSource::ClearCache()
{
	for(CRecordData *pData = m_pCacheHead; pData; )
	{
		CRecordData *pNext = pData->m_Next;

		if( pData->m_Owner )
			pData->m_Owner->m_pData = NULL;

		free(pData);
		pData = pNext;
	}
	m_pCacheHead = m_pCacheTail = NULL;
	m_nCacheSize = 0;
}

inline CRecord * CDataSource::GetBlock(int nBlock)
{
	if( size() <= nBlock )
		resize(nBlock+1, 0);

	CRecord *& pBlock = at(nBlock);
	if( !pBlock )
	{
		pBlock = (CRecord *)calloc(1, RECORD_BLOCK_SIZE*sizeof(CRecord));

		int nFromId = nBlock * RECORD_BLOCK_SIZE;
		for(int i = 0; i < RECORD_BLOCK_SIZE; i++)
			pBlock[i].m_nPos = nFromId++;
	}
	return pBlock;
}

inline void CDataSource::AddToCache(CRecordData *pData)
{
	if( pData->m_Next = m_pCacheHead )
		m_pCacheHead->m_Prev = pData;
	else
		m_pCacheTail = pData;
	m_pCacheHead = pData;
	m_pCacheHead->m_Prev = NULL;
	m_nCacheSize++;
}

inline void CDataSource::RemoveFromCache(CRecordData *pData)
{
	if( pData->m_Prev )
		pData->m_Prev->m_Next = pData->m_Next;

	if( pData->m_Next )
		pData->m_Next->m_Prev = pData->m_Prev;

	if( pData == m_pCacheHead )
		m_pCacheHead = pData->m_Next;

	if( pData == m_pCacheTail )
		m_pCacheTail = pData->m_Prev;

	m_nCacheSize--;
}

inline void CDataSource::PeekFromCache(CRecordData *&pData)
{
	if( !pData && m_nCacheSize == m_nCacheCapacity )
	{
		pData = m_pCacheTail;
		pData->m_Owner->m_pData = NULL;
	}

	if( pData )
		RemoveFromCache(pData);
}

void CDataSource::ClearRecordData(CRecord * pRecord)
{
	if( pRecord->m_pData )
	{
		RemoveFromCache(pRecord->m_pData);
		pRecord->Clear();
	}
}

CRecord * CDataSource::GetRecord(int nPos, bool fData)
{
	if( nPos < 0 || nPos >= m_nRecordCount )
		return NULL;

	CRecord & pRecord = GetBlock(nPos / RECORD_BLOCK_SIZE)[nPos % RECORD_BLOCK_SIZE];
	if( !fData )
		return &pRecord;

	CRecordData *& pData = pRecord.m_pData;

	PeekFromCache(pData);

	if( !pData || pData->m_Owner != &pRecord )
	{
		bool fRet = m_pReader->GetRecord(pRecord);
		if( !fRet || !pData || pData->m_Owner != &pRecord )
		{
			if( pData )
				pRecord.Clear();
			return NULL;
		}
	}

	AddToCache(pData);
	return &pRecord;
}

bool CDataSource::AddRecords(int nCount)
{
	if( !nCount )
		return true;

	m_nRecordCount += nCount;
	return true;
}

bool CDataSource::InsertRecord(int nPos)
{
	for(int i = ++m_nRecordCount - 1; i >= 0 ; i--)
	{
		CRecord *pEvt = GetRecord(i);

		if( i > nPos )
			pEvt->Move(*GetRecord(i-1));

		if( pEvt->m_nPos >= nPos )
			pEvt->m_nPos++;
	}

	GetRecord(nPos)->m_nPos = nPos;
	return true;
}

bool CDataSource::DeleteRecord(int nPos)
{
	CRecord *pRecord = GetRecord(nPos);
	if( !pRecord )
		return false;

	int nRecordPos = pRecord->m_nPos;
	ClearRecordData(pRecord);

	for(int i = 0; i < m_nRecordCount - 1; i++)
	{
		CRecord *pEvt = GetRecord(i);

		if( i >= nPos )
			pEvt->Move(*GetRecord(i+1));

		if( pEvt->m_nPos >= nRecordPos )
			pEvt->m_nPos--;
	}

	m_nRecordCount--;
	return true;
}

int CDataSource::FindRecord(long nIdx)
{
	for(int i = 0; i < m_nRecordCount; i++)
		if( GetRecord(i)->m_nPos == nIdx )
			return i;
	return -1;
}

void CDataSource::SwapRecords(int nPos1, int nPos2)
{
	CRecord * pRecord1 = GetRecord(nPos1);
	CRecord * pRecord2 = GetRecord(nPos2);
	if( pRecord1 && pRecord2 )
		pRecord1->Swap(*pRecord2);
}

void CDataSource::SwapRecordState(CRecord* pRecord, int nPos, CDataSource &c)
{
	CRecord *pSwapRecord = c.GetRecord(nPos);

	pRecord->m_nPos = pSwapRecord->m_nPos;
	ClearRecordData(pRecord);
	
	int nViewPos = pSwapRecord->m_nViewPos;
	pSwapRecord->CopyState(*pRecord);

	if( nPos < nViewPos )
		pRecord->CopyState(*GetRecord(nViewPos));
	else
		pRecord->CopyState(*c.GetRecord(nViewPos));
}

//////////////////////////////////////////////////////////////////////
