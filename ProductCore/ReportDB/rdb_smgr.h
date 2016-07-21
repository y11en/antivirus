#ifndef __REPORT_SEG_MGR_6A8D422E_
#define __REPORT_SEG_MGR_6A8D422E_

#include "rdb_helper.h"
#include "rdb_seg.h"
#include <vector>

#include <pshpack1.h>
typedef struct
{
	byte   Magic[4];
	uint32 nFirstSegment;
	uint32 nEventSize;
} tMgrInfo;
#include <poppack.h>

typedef struct tag_tSegmentInfo
{
	cReportDBSegment* pSegment;
	uint32            nSegment;
	uint64            nFirstRecord;
	uint64            nRecordsCount;
	uint64            nSize;
} tSegmentInfo;


class cReportDBSegmentMgr
{
public:
	
	cReportDBSegmentMgr();
	~cReportDBSegmentMgr();

public:
	tERROR 
		Init(
		IN cReportDBHelper* pReportHelper, 
		IN size_t nEventSize,
		IN bool bWriter,
		IN OPTIONAL cObjectDB* pGlobalData
		);

	tERROR
		AddEvent (
		IN const void* pEvent,
		IN size_t nEventSize,
		OUT OPTIONAL tEventId* pnEventId
		);

	tERROR 
		GetEvents (
		IN int64 nFirstEvent, 
		IN void* pEvents, 
		IN size_t nEventsCount, 
		IN size_t nBufferSize, 
		OUT OPTIONAL size_t* pnEventsRead
		);
	
	tERROR 
		UpdateEvent (
		IN tEventId nEventId, 
		IN const void* pEvent,
		IN size_t nEventSize
		);

	tERROR 
		FindObject (
		IN bool         bGlobal,
		IN uint8        nObjType, 
		IN uint32       nFlags,
		IN const void*  pName,
		IN size_t       cbNameSize,
		IN bool         bCreate,
		IN tObjectId    nParentObjectId,
		OUT tObjectId*  pnObjectId
		);

	tERROR
		GetObjectName (
		IN tObjectId nObjectId,
		OUT OPTIONAL void* pNameBuff, 
		IN size_t nNameBuffSize, 
		OUT OPTIONAL size_t* pnNameSize,
		IN uint32 nFlags,
		OUT OPTIONAL uint8*  pnObjectType
		);

	tERROR
		GetObjectParent (
		IN tObjectId nObjectId,
		OUT tObjectId* pnParentId,
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

	tERROR
		SetObjectData (
		IN tObjectId nObjectId,
		IN void* pData, 
		IN size_t nDataSize,
		IN uint32 nDataType
		);

	tERROR 
		Close();

	tERROR 
		Clear();

	uint64 GetSize();
	uint64 GetRecordsCount();

	void SetMaxSegmentEvents(uint64 nCount);
	void SetMaxSegmentSize(uint64 nSize);
	void SetMaxSegmentTime(uint64 nSeconds);
	
	void SetMaxReportEvents(uint64 nCount);
	void SetMaxReportSize(uint64 nSize);
	void SetMaxReportTime(uint64 nSeconds);

	tERROR CheckLimits();

	cReportDBHelper* m_pHelper;

private:
	bool m_bInited;
	cLock m_Lock;
	std::vector<tSegmentInfo>  m_vSegments;
	uint64 m_nMaxSegmentEvents;
	uint64 m_nMaxSegmentSize;
	uint64 m_nMaxSegmentTime; // seconds
	uint64 m_nMaxReportRecords;
	uint64 m_nMaxReportSize;
	uint64 m_nMaxReportTime; // seconds
	uint32 m_nNextSegment;
	bool m_bWriter;
	alFile* m_pSegmentDB;
	tMgrInfo m_Info;
	uint64 m_nReportSize;
	uint64 m_nReportRecords;
	cObjectDB* m_pGlobalData;

	tERROR CreateSegment();
	tSegmentInfo* GetLastSegment();
	tSegmentInfo* GetSegment(uint32 nSegment);
};


#endif // __REPORT_SEG_MGR_6A8D422E_
