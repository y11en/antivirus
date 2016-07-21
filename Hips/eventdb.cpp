#include <memory.h>

#include "eventdb.h"

#define __check_zero_neg(arg) if(!(arg)) (arg)=~(arg);
#define FLAG_FROM_VAL(val) (((uint)1)<<(val))

//#define FIELD_OFFSET(type, field)    ((unsigned int)&(((type *)0)->field))
//CRITERIA cr1 = {opEQ, FIELD_OFFSET(EVENT_RECORD, Time), 0};



cEventDB::cEventDB()
{

}

cEventDB::~cEventDB()
{
}

bool cEventDB::Init(alFile* pEvtDBFile)
{
	if (!m_evtdb.Init(pEvtDBFile, sizeof(EVENT_RECORD)))
		return false;
	if (!m_evtdb.GetRecordCount())
	{ 
		EVENT_RECORD rec = {0};
		m_evtdb.AddRecord(&rec, sizeof(rec), NULL);
	}
	return true;
}


bool 
cEventDB::AddEvent (
	uint64   Time, 
	uint64   ObjId,
	uint64   ObjId2,
	uint32   UniqPid,
	eObjectType ObjectType,
	eEventType EventType,
	uint16   UserId,
//	tEventId BaseEvent,
	uint32   ExtraData,
	tEventId* pEventId
	)
{
	EVENT_RECORD rec;
	rec.Time = Time;
	rec.ObjId = ObjId;
	rec.ObjId2 = ObjId2;
	rec.UniqPid = UniqPid;
	rec.ObjectType = ObjectType;
	rec.EventType = EventType;
	rec.UserId = UserId;
//	rec.BaseEvent = BaseEvent;
	rec.ExtraData = ExtraData;
	return m_evtdb.AddRecord(&rec, sizeof(rec), pEventId);
}

PEVENT_FIND_DATA
cEventDB::FindFirstEvent (
	uint32    ObjectTypeFlags, 
	uint32    EventTypeFlags,
	tUniqPid  UniqPid,
	uint64    ObjId,
	size_t    nChildUniqPidsCount,
	tUniqPid* arrChildUniqPids,
	uint16    UserId,
	tEventId  EventFrom,
	tEventId  EventTo,
	uint64    TimeFrom,
	uint64    TimeTo 
	)
{
	if (nChildUniqPidsCount && !arrChildUniqPids)
		return NULL;
	PEVENT_FIND_DATA_EX pFindEx = new EVENT_FIND_DATA_EX;
	memset(pFindEx, 0, sizeof(*pFindEx));
	pFindEx->FindEventFrom = EventFrom;
	pFindEx->FindEventTo = EventTo;
	pFindEx->FindTimeFrom = TimeFrom;
	pFindEx->FindTimeTo = TimeTo;
	pFindEx->FindObjId = ObjId;
	pFindEx->FindUniqPid = UniqPid;
	pFindEx->FindChildUniqPidsCount = nChildUniqPidsCount;
	if (nChildUniqPidsCount)
	{
		pFindEx->FindChildUniqPids = new tUniqPid[nChildUniqPidsCount];
		if (!pFindEx->FindChildUniqPids)
		{
			delete pFindEx;
			return NULL;
		}
		for (size_t i=0;i<nChildUniqPidsCount;i++)
			pFindEx->FindChildUniqPids[i] = arrChildUniqPids[i];
	}
	pFindEx->FindObjectTypeFlags = ObjectTypeFlags; 
	pFindEx->FindEventTypeFlags = EventTypeFlags;
	pFindEx->FindUserId = UserId;
	__check_zero_neg(pFindEx->FindEventTo);
	__check_zero_neg(pFindEx->FindTimeTo);
	__check_zero_neg(pFindEx->FindObjectTypeFlags);
	__check_zero_neg(pFindEx->FindEventTypeFlags);
	if (!FindNextEvent(pFindEx))
	{
		delete pFindEx;
		return NULL;
	}
	return pFindEx;
}

bool 
cEventDB::FindNextEvent (
	PEVENT_FIND_DATA pFindData
	)
{
	if(!pFindData)
		return false;
	PEVENT_FIND_DATA_EX pFindEx = (PEVENT_FIND_DATA_EX)pFindData;
	tEventId EventId = pFindEx->EventId;
	if (EventId)
		EventId++;
	else
		EventId = pFindEx->FindEventFrom;
	if (!EventId)
		EventId++;
	for (;EventId<pFindEx->FindEventTo;EventId++)
	{
		if (!m_evtdb.GetRecord(EventId, pFindEx, sizeof(EVENT_RECORD), 0))
			break;
		if (pFindEx->FindObjId && pFindEx->FindObjId != pFindEx->ObjId)
			continue;
		if (pFindEx->FindUniqPid && pFindEx->UniqPid != pFindEx->FindUniqPid)
			continue;
		if(pFindEx->FindChildUniqPidsCount)
		{
			size_t i;
			for (i=0;i<pFindEx->FindChildUniqPidsCount;i++)
			{
				if (pFindEx->UniqPid == pFindEx->FindChildUniqPids[i])
					break;
			}
			if (i == pFindEx->FindChildUniqPidsCount)
				continue;
		}
		if (!(pFindEx->FindObjectTypeFlags & FLAG_FROM_VAL(pFindEx->ObjectType)))
			continue;
		if (!(pFindEx->FindEventTypeFlags & FLAG_FROM_VAL(pFindEx->EventType)))
			continue;
		if (pFindEx->Time < pFindEx->FindTimeFrom)
			continue;
		if (pFindEx->Time > pFindEx->FindTimeTo)
			break;
		if (pFindEx->FindUserId && pFindEx->FindUserId != pFindEx->UserId)
			continue;
		pFindEx->EventId = EventId;
		return true;
	}
	return false;
}

bool
cEventDB::FindEventClose (
	PEVENT_FIND_DATA pFindData
	)
{
	if(!pFindData)
		return false;
	PEVENT_FIND_DATA_EX pFindEx = (PEVENT_FIND_DATA_EX)pFindData;
	if(pFindEx->FindChildUniqPids)
		delete[] pFindEx->FindChildUniqPids;
	delete pFindEx;
	return true;
}
