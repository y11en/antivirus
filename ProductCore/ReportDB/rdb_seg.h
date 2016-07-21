#ifndef __REPORT_SEG__1DD8842_
#define __REPORT_SEG__1DD8842_

#include "rdb_helper.h"
#include "objdb.h"

class cReportDBSegment
{
public:
	
	cReportDBSegment();
	~cReportDBSegment();
	
	tERROR 
	Init(
		IN cReportDBHelper* pReportHelper, 
		IN size_t nEventSize,
		IN bool bWriter, 
		IN bool bCreate,
		IN OPTIONAL uint32 nSegment, 
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
		IN tEventId nFirstEvent, 
		IN size_t nEventsCount, 
		IN void* pEvents, 
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
		FindObject(
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
		OUT OPTIONAL uint8*  pnObjectType
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

	uint64 GetRecordCount();
	uint64 GetSize();
	uint64 GetTime(); // how old is segment, in seconds


private:
	uint32 m_nSegment;
	bool m_bInited;
	cPlainDB* m_evtdb;
	cObjectDB* m_objdb;
	cObjectDB* m_pGlobalData;
	cReportDBHelper* m_pHelper;
	uint64 m_nCreateTime; // segment create time as Windows system FILETIME

	tObjectHash
	CalcObjectHash(
		IN uint32       ObjType,
		IN const void*  pName,
		IN size_t       cbNameSize,
		IN tObjectId	nParentId,
		IN uint32       nFlags
		);


	tERROR 
	FindObjectEx(
		IN bool         bGlobal,
		IN uint8        nObjType,
		IN uint32       nFlags,
		IN const void*  pName,
		IN size_t       cbNameSize,
		IN tObjectId    nParentId,
		IN bool         bCreate,
		OUT tObjectId*  pnObjectId
		);			
};


#endif // __REPORT_SEG__1DD8842_
