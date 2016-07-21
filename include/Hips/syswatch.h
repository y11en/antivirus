#ifndef __EVENTDB_HELPER_
#define __EVENTDB_HELPER_

#include "eventdb.h"
#include "procdb.h"

class cSystemWatcher : public cEventDB, public cHashDB, public cProcessDB
{
public:
	cSystemWatcher() : m_bInited(false)
	{
	}
	virtual bool 
		Init (
		alFile* pEvtDBFile, 
		alFile* pObjDBFile,
		alFile* pProcDBFile
		)
	{
		if(!cEventDB::Init(pEvtDBFile))
			return false;
		if(!cHashDB::Init(pObjDBFile))
			return false;
		if (!cProcessDB::Init(pProcDBFile))
			return false;
		m_bInited = true;
		return true;
	}

	bool IsInited()
	{
		return m_bInited;
	}

	bool 
		AddEvent (
		uint64 Time, 
		tNativePid pid, 
		eEventType EventType, 
		eObjectType ObjectType, 
		const void* pName, 
		size_t nNameSize, 
		const void* pName2 = 0, 
		size_t nName2Size = 0, 
		uint16 nUser = 0, 
//		tEventId BaseEvent = 0, 
		uint32 ExtraData = 0
		);

	uint64 
		CalcObjId (
		eObjectType ObjectType, 
		const void* pName, 
		size_t nNameSize
		);

private:
	bool AddEventFile(uint64 Time, tUniqPid uniq_pid, eEventType EventType, eObjectType ObjectType, 
		const void* pName, size_t nNameSize, 
		const void* pName2, size_t nName2Size, 
		uint16 UserId, 
//		tEventId BaseEvent, 
		uint32 ExtraData);
	bool m_bInited;
};




#endif // __EVENTDB_HELPER_
