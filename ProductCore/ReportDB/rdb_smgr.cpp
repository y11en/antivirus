#include <minmax.h>
#include "rdb_smgr.h"
#include "md5.h"

#define AUTO_LOCK_W() cAutoLock _lock(&m_Lock, true);
#define AUTO_LOCK_R() cAutoLock _lock(&m_Lock, false);

const unsigned char MGR_MAGIC[4]   = {'S','D','B','H'};

cReportDBSegmentMgr::cReportDBSegmentMgr()
{
	m_bInited = false;
	m_pHelper = NULL;
	m_nNextSegment = 1;
	m_bWriter = false;
	m_pSegmentDB = NULL;
	m_nMaxSegmentEvents = 0;
	m_nMaxSegmentSize = 0;
	m_nMaxSegmentTime = 0;
	m_nMaxReportRecords = 0;
	m_nMaxReportSize = 0;
	m_nMaxReportTime = 0;
	m_nReportRecords = 0;
	m_nReportSize = 0;
}

cReportDBSegmentMgr::~cReportDBSegmentMgr()
{
	Close();
}

tERROR cReportDBSegmentMgr::Close()
{
	AUTO_LOCK_W();
	m_bInited = false;
	for (size_t c=0; c<m_vSegments.size(); c++)
	{
		cReportDBSegment* pSegment = m_vSegments[c].pSegment;
		if (pSegment)
			delete pSegment;
	}
	m_vSegments.clear();
	if (m_pSegmentDB)
		m_pSegmentDB->Release();
	m_pSegmentDB = NULL;
	return errOK;
}

tERROR cReportDBSegmentMgr::Clear()
{
	if (!m_bInited)
		return errOBJECT_NOT_INITIALIZED;
	if (!m_bWriter)
		return errOBJECT_READ_ONLY;
	AUTO_LOCK_W();
	Close();
	m_pHelper->DeleteFile(L"*.*", 0);
	return Init(m_pHelper, m_Info.nEventSize, true, m_pGlobalData);
}


tERROR cReportDBSegmentMgr::Init(
			IN cReportDBHelper* pReportHelper,
			IN size_t nEventSize,
			IN bool bWriter,
			IN OPTIONAL cObjectDB* pGlobalData
			)
{
	tERROR error;
	if (m_bInited)
		return errALREADY;
	if (!pReportHelper)
		return errPARAMETER_INVALID;
	
	m_pHelper = pReportHelper;
	m_bWriter = bWriter;
	m_pGlobalData = pGlobalData;
	m_nReportRecords = 0;
	m_nReportSize = 0;

	// init segments database
	{
		bool bInit = false;
		error = m_pHelper->OpenFile(L"segments.dat", 0, bWriter, bWriter, &m_pSegmentDB);
		if (PR_FAIL(error))
			return error;
		if (!m_pSegmentDB->SeekRead(0, &m_Info, sizeof(m_Info), 0) || 0!=memcmp(m_Info.Magic, MGR_MAGIC, sizeof(MGR_MAGIC)) )
			bInit = true;
		else if (m_Info.nEventSize != nEventSize)
			bInit = true;
		if (bInit)
		{
			if (!bWriter)
				return errOBJECT_CANNOT_BE_INITIALIZED;
			memset(&m_Info, 0, sizeof(m_Info));
			memcpy(m_Info.Magic, MGR_MAGIC, sizeof(MGR_MAGIC));
			m_Info.nEventSize = (uint32)nEventSize;
			if (!m_pSegmentDB->SeekWrite(0, &m_Info, sizeof(m_Info), 0))
				return errOBJECT_WRITE;
		}

		if (0 != m_Info.nFirstSegment)
		{
			// load segments
			m_nNextSegment = m_Info.nFirstSegment;
			uint64 nVirtualRecordCounter = 0;
			do
			{
				tSegmentInfo seg;
				memset(&seg, 0, sizeof(seg));
				seg.pSegment = new cReportDBSegment();
				if (!seg.pSegment)
					return errNOT_ENOUGH_MEMORY;
				tERROR error = seg.pSegment->Init(m_pHelper, nEventSize, bWriter, false, m_nNextSegment, pGlobalData);
				if (PR_FAIL(error))
				{
					delete seg.pSegment;
					break;
				}
				seg.nSegment = m_nNextSegment;
				seg.nFirstRecord = nVirtualRecordCounter;
				seg.nRecordsCount = seg.pSegment->GetRecordCount();
				seg.nSize = seg.pSegment->GetSize();
				m_nReportRecords += seg.nRecordsCount;
				m_nReportSize += seg.nSize;
				nVirtualRecordCounter += seg.nRecordsCount;
				m_vSegments.push_back(seg);
				m_nNextSegment++;
			} while (true);
		}
		if (m_vSegments.size() == 0)
		{
			if (!bWriter)
				return errOBJECT_CANNOT_BE_INITIALIZED;
			m_Info.nFirstSegment = m_nNextSegment = 1;
			error = CreateSegment();
			if (PR_FAIL(error))
				return error;
		}
	}
	
	m_bInited = true;
	
	return errOK;
}

void cReportDBSegmentMgr::SetMaxSegmentEvents(uint64 nCount)
{
	AUTO_LOCK_W();
	m_nMaxSegmentEvents = nCount;
	CheckLimits();
}

void cReportDBSegmentMgr::SetMaxSegmentSize(uint64 nSize)
{
	AUTO_LOCK_W();
	m_nMaxSegmentSize = nSize;
	CheckLimits();
}

void cReportDBSegmentMgr::SetMaxSegmentTime(uint64 nSeconds)
{
	AUTO_LOCK_W();
	m_nMaxSegmentTime = nSeconds;
	CheckLimits();
}

void cReportDBSegmentMgr::SetMaxReportEvents(uint64 nCount)
{
	AUTO_LOCK_W();
	m_nMaxReportRecords = nCount;
	CheckLimits();
}

void cReportDBSegmentMgr::SetMaxReportSize(uint64 nSize)
{
	AUTO_LOCK_W();
	m_nMaxReportSize = nSize;
	CheckLimits();
}

void cReportDBSegmentMgr::SetMaxReportTime(uint64 nSeconds)
{
	AUTO_LOCK_W();
	m_nMaxReportTime = nSeconds;
	CheckLimits();
}

uint64 cReportDBSegmentMgr::GetRecordsCount()
{
	AUTO_LOCK_R();
	return m_nReportRecords;
}

uint64 cReportDBSegmentMgr::GetSize()
{
	AUTO_LOCK_R();
	return m_nReportSize;
}

tSegmentInfo* cReportDBSegmentMgr::GetLastSegment()
{
	size_t count = m_vSegments.size();
	if (count == 0)
		return NULL;
	return &m_vSegments.at(count-1);
}

tSegmentInfo* cReportDBSegmentMgr::GetSegment(uint32 nSegment)
{
	// try last segment
	tSegmentInfo* pSegmentInfo = GetLastSegment();
	if (pSegmentInfo == NULL)
		return NULL;
	if (0 == nSegment) // global segment - return last
		return pSegmentInfo;
	if (nSegment != pSegmentInfo->nSegment)
	{
		// if not last - find matching segment
		pSegmentInfo = NULL;
		for (size_t i=0; i<m_vSegments.size(); i++)
		{
			if (m_vSegments[i].nSegment == nSegment)
			{
				pSegmentInfo = &m_vSegments[i];
				break;
			}
		}
	}
	return pSegmentInfo;
}

tERROR 
cReportDBSegmentMgr::CreateSegment()
{
	if (!m_bWriter)
		return errOBJECT_READ_ONLY;
	if (m_nNextSegment >= 0x7FFFFFF0)
		return errOUT_OF_SPACE;
	cReportDBSegment* pSegment = new cReportDBSegment();
	if (!pSegment)
		return errNOT_ENOUGH_MEMORY;
	tERROR error = pSegment->Init(m_pHelper, m_Info.nEventSize, true, true, m_nNextSegment, m_pGlobalData);
	if (PR_FAIL(error))
	{
		delete pSegment;
		return error;
	}
	if (!m_pSegmentDB->SeekWrite(0, &m_Info, sizeof(m_Info), 0))
	{
		delete pSegment;
		return errOBJECT_WRITE;
	}
	tSegmentInfo seg;
	memset(&seg, 0, sizeof(seg));
	seg.pSegment = pSegment;
	seg.nSegment = m_nNextSegment;
	tSegmentInfo* pLastSeg = GetLastSegment();
	if (pLastSeg)
		seg.nFirstRecord = pLastSeg->nFirstRecord + pLastSeg->nRecordsCount;
	seg.nRecordsCount = pSegment->GetRecordCount();
	seg.nSize = pSegment->GetSize();
	m_nReportSize += seg.nSize;
	m_vSegments.push_back(seg);
	m_nNextSegment++;
	return errOK;
}

tERROR 
cReportDBSegmentMgr::CheckLimits()
{
	tERROR error = errOK;
	if (!m_bWriter)
		return errOK;
	AUTO_LOCK_W();
	tSegmentInfo* pLastSegment = GetLastSegment();
	if ((m_nMaxSegmentEvents && pLastSegment->nRecordsCount >= m_nMaxSegmentEvents)
		|| (m_nMaxSegmentSize && pLastSegment->pSegment->GetSize() >= m_nMaxSegmentSize)
		|| (m_nMaxSegmentTime && pLastSegment->pSegment->GetTime() >= m_nMaxSegmentTime))
	{
		error = CreateSegment();
		if (PR_FAIL(error))
			return error;
	}

	bool bDeleted;
	do 
	{
		bDeleted = false;
		if (m_vSegments.size() > 1)
		{
			tSegmentInfo* pFirstSegment = &m_vSegments[0];
			if ((m_nMaxReportSize && m_nReportSize > m_nMaxReportSize)
				|| (m_nMaxReportRecords && m_nReportRecords > m_nMaxReportRecords)
				|| (m_nMaxReportTime && m_nMaxReportTime > pFirstSegment->pSegment->GetTime()))
			{
				delete pFirstSegment->pSegment;
				m_pHelper->DeleteFile(L"*.*", pFirstSegment->nSegment);
				m_nReportRecords -= pFirstSegment->nRecordsCount;
				m_nReportSize -= pFirstSegment->nSize;
				m_vSegments.erase(m_vSegments.begin());
				m_Info.nFirstSegment = m_vSegments[0].nSegment;
				if (!m_pSegmentDB->SeekWrite(0, &m_Info, sizeof(m_Info), 0))
					return errOBJECT_WRITE;
			}
		}
	} while(bDeleted);

	return error;
}

tERROR 
cReportDBSegmentMgr::AddEvent (
							   IN const void* pEvent, 
							   IN size_t nEventSize,
							   OUT OPTIONAL tEventId* pnEventId
							   )
{
	if (!m_bInited)
		return errOBJECT_NOT_INITIALIZED;
	if (!m_bWriter)
		return errOBJECT_READ_ONLY;
	AUTO_LOCK_W();
	tSegmentInfo* pLastSegment = GetLastSegment();
	if (!pLastSegment)
		return errUNEXPECTED;
	tERROR error = pLastSegment->pSegment->AddEvent(pEvent, nEventSize, pnEventId);
	if (PR_FAIL(error))
		return error;
	pLastSegment->nRecordsCount = pLastSegment->pSegment->GetRecordCount();
	m_nReportRecords++;
	m_nReportSize -= pLastSegment->nSize;
	pLastSegment->nSize = pLastSegment->pSegment->GetSize();
	m_nReportSize += pLastSegment->nSize;
	CheckLimits();
	return error;
}

tERROR 
cReportDBSegmentMgr::UpdateEvent (
			 IN tEventId nEventId, 
			 IN const void* pEvent,
			 IN size_t nEventSize
			 )
{
	if (!m_bInited)
		return errOBJECT_NOT_INITIALIZED;
	if (!m_bWriter)
		return errOBJECT_READ_ONLY;
	uint32 nSegment = GET_SEGMENT_FROM_ID(nEventId);
	AUTO_LOCK_W();
	tSegmentInfo* pSegmentInfo = GetSegment(GET_SEGMENT_FROM_ID(nEventId));
	if (!pSegmentInfo)
		return errOBJECT_NOT_FOUND;
	return pSegmentInfo->pSegment->UpdateEvent(nEventId, pEvent, nEventSize);
}

tERROR 
cReportDBSegmentMgr::GetEvents (
		   IN int64 nFirstEvent, 
		   IN void* pEvents, 
		   IN size_t nEventsCount, 
		   IN size_t nBufferSize,
		   OUT OPTIONAL size_t* pnEventsRead
		   )
{
	size_t nNextSegment=0;
	tEventId nNextEventId;
	size_t i;
	uint32 nSegment = 0;
	size_t nBufferUsed = 0;
	tERROR error;

	if (!m_bInited)
		return errOBJECT_NOT_INITIALIZED;

	AUTO_LOCK_R();

	if (pnEventsRead)
		*pnEventsRead = 0;
	
	if (GET_SEGMENT_FROM_ID(nFirstEvent) == 0)
		nNextEventId = (tEventId)nFirstEvent;
	else if (GET_SEGMENT_FROM_ID(nFirstEvent) == 0xFFFFFFFF) // relative from the end
	{
		tSegmentInfo* pLastSeg = GetLastSegment();
		if (!pLastSeg)
			return errUNEXPECTED;
		nFirstEvent += pLastSeg->nFirstRecord + pLastSeg->nRecordsCount;
		if (nFirstEvent < 0)
			nFirstEvent = 0;
		nNextEventId = (tEventId)nFirstEvent;
	}
	else // this is exact event id
	{
		nNextEventId = DROP_SIGN_BIT(nFirstEvent);
		nSegment = GET_SEGMENT_FROM_ID(nNextEventId);
		nNextEventId = (uint32)nNextEventId;
		for (i=0; i<m_vSegments.size(); i++)
		{
			if (m_vSegments[i].nSegment == nSegment)
			{
				if (nNextEventId >= m_vSegments[i].nRecordsCount)
					return errEND_OF_THE_LIST;
				nNextSegment = i;
			}
		}
	}

	for (i=nNextSegment; i<m_vSegments.size(); i++)
	{
		tSegmentInfo& seg = m_vSegments[i];
		if (nNextEventId >= seg.nRecordsCount)
		{
			nNextEventId -= seg.nRecordsCount;
			continue;
		}
		size_t nSegmentEventsRead;
		size_t nSegmentEvents2Read = min(nEventsCount, (uint32)(seg.nRecordsCount - nNextEventId));
		error = seg.pSegment->GetEvents(nNextEventId, nSegmentEvents2Read, pEvents, nBufferSize, &nSegmentEventsRead);
		if (error != errOK)
			return error;
		if (nSegmentEventsRead != nSegmentEvents2Read)
			return errUNEXPECTED;
		pEvents = (byte*)pEvents + (m_Info.nEventSize * nSegmentEventsRead);
		nEventsCount -= nSegmentEventsRead;
		nBufferSize -= nSegmentEventsRead * m_Info.nEventSize;
		if (pnEventsRead)
			*pnEventsRead = *pnEventsRead + nSegmentEventsRead;
		if (!nEventsCount) 
			return errOK; // all records readed
	}
	return errEOF;
}

tERROR 
cReportDBSegmentMgr::FindObject (
					IN bool         bGlobal,
					IN uint8        nObjType, 
					IN uint32       nFlags,
					IN const void*  pName,
					IN size_t       cbNameSize,
					IN bool         bCreate,
					IN tObjectId    nParentObjectId,
					OUT tObjectId*  pnObjectId
					)
{
	if (!m_bInited)
		return errOBJECT_NOT_INITIALIZED;
	AUTO_LOCK_R();
	if (!bCreate)
		bCreate = false;
	tSegmentInfo* pLastSegment = GetLastSegment();
	if (!pLastSegment)
		return errUNEXPECTED;
	return pLastSegment->pSegment->FindObject(bGlobal, nObjType, nFlags, pName, cbNameSize, bCreate, nParentObjectId, pnObjectId);
}

tERROR
cReportDBSegmentMgr::GetObjectName (
			   IN tObjectId nObjectId,
			   OUT OPTIONAL void* pNameBuff, 
			   IN size_t nNameBuffSize, 
			   OUT OPTIONAL size_t* pnNameSize,
			   IN uint32 nFlags,
			   OUT OPTIONAL uint8*  pnObjectType
			   )
{
	if (!m_bInited)
		return errOBJECT_NOT_INITIALIZED;
	AUTO_LOCK_R();
	tSegmentInfo* pSegmentInfo = GetSegment(GET_SEGMENT_FROM_ID(nObjectId));
	if (!pSegmentInfo)
		return errOBJECT_NOT_FOUND;
	return pSegmentInfo->pSegment->GetObjectName(nObjectId, pNameBuff, nNameBuffSize, pnNameSize, nFlags, pnObjectType);
}

tERROR
cReportDBSegmentMgr::GetObjectParent (
			   IN tObjectId nObjectId,
			   OUT tObjectId* pnParentId,
			   OUT OPTIONAL uint8* pnObjectType
			   )
{
	if (!m_bInited)
		return errOBJECT_NOT_INITIALIZED;
	AUTO_LOCK_R();
	tSegmentInfo* pSegmentInfo = GetSegment(GET_SEGMENT_FROM_ID(nObjectId));
	if (!pSegmentInfo)
		return errOBJECT_NOT_FOUND;
	return pSegmentInfo->pSegment->GetObjectParent(nObjectId, pnParentId, pnObjectType);
}

tERROR
cReportDBSegmentMgr::GetObjectData (
			   IN tObjectId nObjectId,
			   OUT OPTIONAL void* pDataBuff, 
			   IN size_t nDataBuffSize, 
			   OUT OPTIONAL size_t* pnDataSize,
			   IN uint32 nDataType
			   )
{
	if (!m_bInited)
		return errOBJECT_NOT_INITIALIZED;
	AUTO_LOCK_R();
	tSegmentInfo* pSegmentInfo = GetSegment(GET_SEGMENT_FROM_ID(nObjectId));
	if (!pSegmentInfo)
		return errOBJECT_NOT_FOUND;
	return pSegmentInfo->pSegment->GetObjectData(nObjectId, pDataBuff, nDataBuffSize, pnDataSize, nDataType);
}

tERROR
cReportDBSegmentMgr::SetObjectData (
			   IN tObjectId nObjectId,
			   IN void* pData, 
			   IN size_t nDataSize,
			   IN uint32 nDataType
			   )
{
	if (!m_bInited)
		return errOBJECT_NOT_INITIALIZED;
	if (!m_bWriter)
		return errOBJECT_READ_ONLY;
	AUTO_LOCK_W();
	tSegmentInfo* pSegmentInfo = GetSegment(GET_SEGMENT_FROM_ID(nObjectId));
	if (!pSegmentInfo)
		return errOBJECT_NOT_FOUND;
	return pSegmentInfo->pSegment->SetObjectData(nObjectId, pData, nDataSize, nDataType);
}
