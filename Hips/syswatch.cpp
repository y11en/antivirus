#include <memory.h>

#include "syswatch.h"
#include "md5.h"

typedef struct tOBJ_EXTRA_DATA {
	tEventId Created;
	tEventId LastModified;
	tEventId Deleted;
	tUniqPid OwnerUniqPid;
} OBJ_EXTRA_DATA;

uint64 cSystemWatcher::CalcObjId(eObjectType ObjectType, const void* pName, size_t nNameSize)
{
	MD5_CTX md5_ctx;
	MD5Init(&md5_ctx);
	MD5Update(&md5_ctx, (unsigned char*)&ObjectType, 1);
	MD5Update(&md5_ctx, (unsigned char*)pName, (unsigned int)nNameSize);
	MD5Final(&md5_ctx);
	if (sizeof(tHDBHASH) < 16)
	{
		int i;
		for (i=sizeof(uint64); i<16; i++)
			md5_ctx.digest[i % sizeof(uint64)] ^= md5_ctx.digest[i];
	}
	return *(uint64*)md5_ctx.digest;
}

bool cSystemWatcher::AddEvent(uint64 Time, tNativePid pid, eEventType EventType, eObjectType ObjectType, const void* pName, size_t nNameSize, const void* pName2, size_t nName2Size, uint16 UserId, /*tEventId BaseEvent,*/ uint32 ExtraData)
{
	bool res = false;
	tUniqPid uniq_pid;
	if (!ConvertPid2Uniq(pid, &uniq_pid))
		return false;
	switch(ObjectType)
	{
	case objFile:
	case objDirectory:
	case objRegValue:
	case objRegKey:
		res = AddEventFile(Time, uniq_pid, EventType, ObjectType, pName, nNameSize, pName2, nName2Size, UserId, /*BaseEvent,*/ ExtraData);
		return res;
	default:
		return false;
	}
	return false;
}

bool cSystemWatcher::AddEventFile(uint64 Time, tUniqPid pid, eEventType EventType, eObjectType ObjectType, const void* pName, size_t nNameSize, const void* pName2, size_t nName2Size, uint16 UserId = 0, /*tEventId BaseEvent = 0,*/ uint32 ExtraData = 0)
{
	OBJ_EXTRA_DATA edata;
	tEventId EventId;
	bool bAlreadyExist;
	tHDBOFFSET nDataOffset;
	uint64 ObjId = 0, ObjId2 = 0;

	memset(&edata, 0, sizeof(edata));
	if (pName)
		ObjId = CalcObjId(ObjectType, pName, nNameSize);
	if (pName2)
		ObjId2 = CalcObjId(ObjectType, pName2, nNameSize);
	edata.OwnerUniqPid = pid; 
	if (!AddHash(ObjId, pName, nNameSize, &edata, sizeof(edata), 
		&bAlreadyExist, &nDataOffset, &edata, sizeof(edata), 0))
	{
		return false;
	}
	if (bAlreadyExist && EventType==evtModify && edata.OwnerUniqPid==pid)
		return true; 

	if (!cEventDB::AddEvent(Time, ObjId, ObjId2, pid, ObjectType, EventType, UserId, /*BaseEvent,*/
		EventType == evtProcessStart ? cProcessDB::GetNextUniquePid() : ExtraData, &EventId))
		return false;

	switch(EventType)
	{
	case evtCreate: 
		edata.Created = EventId; 
		edata.LastModified = 0;
		edata.Deleted = 0; 
		break;
	case evtModify:
		if (edata.Deleted)
		{
			edata.Created = 0;
			edata.Deleted = 0;
		}
		edata.LastModified = EventId; 
		break;
	case evtDelete: 
		edata.Deleted = EventId; 
		break;
	case evtProcessStart:
		OnProcessStart(ExtraData, pid, Time, EventId, 0);
		break;
	case evtProcessStop:
		OnProcessExit(pid, Time, EventId);
		break;
	}
	edata.OwnerUniqPid = pid; 
	if (!UpdateHashData(ObjId, nDataOffset, &edata, sizeof(edata)))
		return false;
	return true;
}


