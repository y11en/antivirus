// DataSource.h: interface for the CDataSource1 class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_DATASOURCE_H__9469B2DF_F757_413B_95EB_6BDD5F544FC1__INCLUDED_)
#define AFX_DATASOURCE_H__9469B2DF_F757_413B_95EB_6BDD5F544FC1__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#define RECORD_BLOCK_SIZE    0x200
#define RECORD_MAX_FIELDS    20

#define RECORD_STATUS_UNDEF  -1
#define RECORD_POS_HEADER    -1

//////////////////////////////////////////////////////////////////////

struct CRecord;

struct CRecordData
{
	int            m_Size;
	int            m_Status;
	tQWORD         m_Unique;
	CRecord *      m_Owner;
	CRecordData *  m_Prev;
	CRecordData *  m_Next;
	void *         m_FieldVals[1];
};

struct CRecord
{
	struct DATA
	{
		DATA() { memset(this, 0, sizeof(*this)); }

		tCHAR * pStr;
		tPTR    pData;
		tDWORD  nSize;
	};
	
	void           SetTextData(tQWORD qwUnique, int nStatus, int nNum, /*fld_val1,*/ ...);
	void           SetData(tQWORD qwUnique, int nStatus, int nNum, DATA * pData);
	void           CopyData(CRecord &p, int nNum);
	tCHAR*         GetData(int nColumn, tPTR * ppData = NULL, tDWORD nSize = 0);
	int            GetStatus(){ return m_pData ? m_pData->m_Status : RECORD_STATUS_UNDEF; }
	void           Move(CRecord &p);
	void           Swap(CRecord &p);
	void           Clear();
	void           CopyState(CRecord &p);
	bool           GetCheck(int nColumn) const;
	void           SetCheck(int nColumn, bool nVal);
	void           EnableCheck(int nColumn, bool bEnable);
	bool           IsCheckDisabled(int nColumn);

	long           m_nPos;
	CRecordData*   m_pData;
	unsigned       m_nViewPos : 29;
	unsigned       m_fSelected : 1;
	unsigned       m_fFocused : 1;
	unsigned       m_fHidden : 1;
	unsigned       m_nIndent : 11;
	unsigned       m_nChecked : 10;
	unsigned       m_nChkDisable : 10;
};

struct CRecordEx : public CRecord
{
	CRecordEx() { memset(this, 0, sizeof(CRecordEx)); }
	~CRecordEx(){ if( m_pData ) free(m_pData); }
};

//////////////////////////////////////////////////////////////////////

class CDataReader
{
public:
	virtual bool GetRecord(CRecord &pRecord)=0;
	virtual long GetRecordsCount()=0;
	virtual int GetColsCount(){ return 0; }
};

//////////////////////////////////////////////////////////////////////

class CDataSource : public std::vector<CRecord*>
{
public:
	CDataSource(CDataReader * pReader, int nCapacity);
	CDataSource(CDataSource &c, int nCapacity);
	~CDataSource(){ Clear(); }

	void Clear();

	CRecord * GetRecord(int nPos, bool fData=false);
	CRecord * GetBlock(int nBlock);
	long      GetCount() const { return m_nRecordCount; }

	bool AddRecords(int nCount);
	bool InsertRecord(int nPos);
	bool DeleteRecord(int nPos);
	int  FindRecord(long nIdx);
	void SwapRecords(int nPos1, int nPos2);
	void SwapRecordState(CRecord* pRecord, int nPos, CDataSource &c);
	void ClearRecordData(CRecord * pRecord);
	void ClearCache();

private:
	void AddToCache(CRecordData *pData);
	void RemoveFromCache(CRecordData *pData);
	void PeekFromCache(CRecordData *&pData);

private:
	CDataReader * m_pReader;

	long          m_nRecordCount;
	long          m_nCacheCapacity;
	long          m_nCacheSize;

	CRecordData * m_pCacheHead;
	CRecordData * m_pCacheTail;
};

//////////////////////////////////////////////////////////////////////

inline void CRecord::SetTextData(tQWORD qwUnique, int nStatus, int nNum, /*fld_val1,*/ ...)
{
	DATA pData[RECORD_MAX_FIELDS];

	va_list args;
	va_start(args, nNum);
	for(int i = 0; (i < nNum) && (i < countof(pData)); i++)
		pData[i].pStr = va_arg(args, tCHAR*);
	va_end(args);

	SetData(qwUnique, nStatus, nNum, pData);
}

inline void CRecord::SetData(tQWORD qwUnique, int nStatus, int nNum, DATA * pData)
{
	int nOffset = offsetof(CRecordData, m_FieldVals) + nNum*sizeof(void*);
	int i, nSize = nOffset, nLen[RECORD_MAX_FIELDS];

	for(i = 0; (i < nNum) && (i < countof(nLen)); i++)
	{
		nLen[i] = (int)(sizeof(tCHAR)*(strlen(pData[i].pStr) + 1));
		nSize += nLen[i] + pData[i].nSize;
	}

	if( !m_pData || m_pData->m_Size < nSize )
	{
		m_pData = (CRecordData*)realloc(m_pData, nSize);
		m_pData->m_Size = nSize;
	}

	tBYTE* pPtr = (tBYTE*)m_pData + nOffset;
	for(i = 0; i < nNum; i++)
	{
		if( pData[i].nSize )
		{
			memcpy(pPtr, pData[i].pData, pData[i].nSize);
			pPtr += pData[i].nSize;
		}
		
		m_pData->m_FieldVals[i] = pPtr;
		
		memcpy(pPtr, pData[i].pStr, nLen[i]);
		pPtr += nLen[i];
	}

	m_pData->m_Owner = this;
	m_pData->m_Status = nStatus;
	m_pData->m_Unique = qwUnique;
}

inline tCHAR * CRecord::GetData(int nColumn, tPTR * ppData, tDWORD nSize)
{
	if( !m_pData )
		return "";
	
	tBYTE* pPtr = (tBYTE *)m_pData->m_FieldVals[nColumn];
	if( ppData )
		*ppData = pPtr - nSize;
	return (tCHAR *)pPtr;
}

inline void CRecord::CopyData(CRecord &p, int nNum)
{
	if( !p.m_pData )
		return;

	bool bNew = !m_pData;
	m_pData = (CRecordData*)realloc(m_pData, p.m_pData->m_Size);
	m_pData->m_Size = p.m_pData->m_Size;

	int nOffset = offsetof(CRecordData, m_FieldVals) + nNum*sizeof(void*);
	memcpy((tBYTE *)m_pData + nOffset, (tBYTE *)p.m_pData + nOffset, p.m_pData->m_Size - nOffset);

	int nDelta = (int)m_pData - (int)p.m_pData;
	for(int i = 0; i < nNum; i++)
		m_pData->m_FieldVals[i] = (tBYTE *)p.m_pData->m_FieldVals[i] + nDelta;

	m_pData->m_Owner = this;
	m_pData->m_Status = p.m_pData->m_Status;
	m_pData->m_Unique = p.m_pData->m_Unique;

	if( bNew )
		m_pData->m_Prev = m_pData->m_Next = NULL;
}

inline void CRecord::Move(CRecord &p)
{
	memcpy(this, &p, sizeof(CRecord));
	if( p.m_pData )
	{
		m_pData->m_Owner = this;
		p.m_pData = NULL;
	}
}

inline void CRecord::Swap(CRecord &p)
{
	std::swap(*this, p);
	if( m_pData )
		m_pData->m_Owner = this;
	if( p.m_pData )
		p.m_pData->m_Owner = &p;
}

inline void CRecord::Clear()
{
	free(m_pData);
	m_pData = NULL;
}

inline void	CRecord::CopyState(CRecord &p)
{
	m_nViewPos = p.m_nViewPos;
	m_nChecked = p.m_nChecked;
	m_nChkDisable = p.m_nChkDisable;
	m_nIndent = p.m_nIndent;
	m_fSelected = p.m_fSelected;
	m_fFocused = p.m_fFocused;
}

inline bool CRecord::GetCheck(int nColumn) const
{
	return !!(m_nChecked & (1 << nColumn));
}

inline void CRecord::SetCheck(int nColumn, bool nVal)
{
	int bit = 1 << nColumn;
	if( nVal )
		m_nChecked |= bit;
	else
		m_nChecked &= ~bit;
}

inline bool CRecord::IsCheckDisabled(int nColumn)
{
	return !!(m_nChkDisable & (1 << nColumn));
}

inline void CRecord::EnableCheck(int nColumn, bool bEnable)
{
	int bit = 1 << nColumn;
	if( bEnable )
		m_nChkDisable &= ~bit;
	else
		m_nChkDisable |= bit;
}

//////////////////////////////////////////////////////////////////////

inline CDataSource::CDataSource(CDataReader *pReader, int nCapacity) :
	m_pReader(pReader),
	m_nCacheCapacity(nCapacity),
	m_pCacheHead(NULL),
	m_pCacheTail(NULL),
	m_nCacheSize(0),
	m_nRecordCount(0)
{
}

inline CDataSource::CDataSource(CDataSource &c, int nCapacity) :
	m_pCacheHead(NULL),
	m_pCacheTail(NULL),
	m_nCacheSize(0)
{
	m_pReader = c.m_pReader;
	m_nRecordCount = c.m_nRecordCount;
	m_nCacheCapacity = nCapacity;

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

inline void CDataSource::Clear()
{
	ClearCache();

	for(tDWORD i = 0; i < size(); i++)
	{
		CRecord *pBlock = at(i);
		if( pBlock )
			free(pBlock);
	}
	clear();

	m_nRecordCount = 0;
}

inline void CDataSource::ClearCache()
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
	if( size() <= (unsigned)nBlock )
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
	if( pData == m_pCacheHead )
		return;

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

	pData->m_Next = pData->m_Prev = NULL;

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

inline void CDataSource::ClearRecordData(CRecord * pRecord)
{
	if( pRecord->m_pData )
	{
		RemoveFromCache(pRecord->m_pData);
		pRecord->Clear();
	}
}

inline CRecord * CDataSource::GetRecord(int nPos, bool fData)
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
			return &pRecord;
		}
	}

	AddToCache(pData);
	return &pRecord;
}

inline bool CDataSource::AddRecords(int nCount)
{
	if( !nCount )
		return true;

	m_nRecordCount += nCount;
	return true;
}

inline bool CDataSource::InsertRecord(int nPos)
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

inline bool CDataSource::DeleteRecord(int nPos)
{
	CRecord *pRecord = GetRecord(nPos);
	if( !pRecord )
		return false;

	int nRecordPos = pRecord->m_nPos;
	ClearRecordData(pRecord);
	pRecord->m_nChecked = 0;

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

inline int CDataSource::FindRecord(long nIdx)
{
	for(int i = 0; i < m_nRecordCount; i++)
		if( GetRecord(i)->m_nPos == nIdx )
			return i;
	return -1;
}

inline void CDataSource::SwapRecords(int nPos1, int nPos2)
{
	CRecord * pRecord1 = GetRecord(nPos1);
	CRecord * pRecord2 = GetRecord(nPos2);
	if( pRecord1 && pRecord2 )
		pRecord1->Swap(*pRecord2);
}

inline void CDataSource::SwapRecordState(CRecord* pRecord, int nPos, CDataSource &c)
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

#endif // !defined(AFX_DATASOURCE_H__9469B2DF_F757_413B_95EB_6BDD5F544FC1__INCLUDED_)
