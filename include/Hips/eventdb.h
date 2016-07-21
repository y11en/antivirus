#ifndef __EVENTDB_H_
#define __EVENTDB_H_

#include "db_types.h"
#include "plaindb.h"
#include "hashdb.h"

// file structures
#include <pshpack1.h>

typedef struct tag_EVENT_RECORD {
	uint64 Time;
	uint64 ObjId;
	uint64 ObjId2;
	uint32 UniqPid;
	uint8  ObjectType;
	uint8  EventType;
	uint16 UserId;
//	uint32 BaseEvent;
	uint32 ExtraData;
} EVENT_RECORD, *PEVENT_RECORD;

typedef struct tag_EVENT_FIND_DATA : public EVENT_RECORD
{
	tEventId EventId;
} EVENT_FIND_DATA, *PEVENT_FIND_DATA;

typedef struct tag_EVENT_FIND_DATA_EX : public EVENT_FIND_DATA
{
	tEventId  FindEventFrom;
	tEventId  FindEventTo;
	uint64    FindTimeFrom;
	uint64    FindTimeTo;
	uint64    FindObjId;
	tUniqPid  FindUniqPid;
	size_t    FindChildUniqPidsCount;
	tUniqPid* FindChildUniqPids;
	uint32    FindObjectTypeFlags; 
	uint32    FindEventTypeFlags;
	uint16    FindUserId;
} EVENT_FIND_DATA_EX, *PEVENT_FIND_DATA_EX;


enum eObjectType 
{
	objUnknown = 0,
	objFile = 1,
	objDirectory = 2,
	objRegValue = 3,
	objRegKey = 4,
	objShare = 5,
	objUserAccount = 6,
	objURL = 7,
	objIPAddress = 8,
	objLast
};

enum eEventType 
{
	evtUnknown = 0,
	evtCreate = 1,
	evtModify = 2,
	evtDelete = 3,
	evtRename = 4,
	evtProcessStart = 5,
	evtProcessStop = 6,
	evtImageLoad = 7,
	evtImageUnload = 8,
	evtLast
};

#include <poppack.h>
/*
typedef union tag_CRITERIA_PARAM {
	void*  ptr;
	uint8  ui8;
	uint16 ui16;
	uint32 ui32;
	uint64 ui64;
} CRITERIA_PARAM;

typedef struct tag_CRITERIA {
	uint Op;
	uint Type;
	CRITERIA_PARAM param1;
	CRITERIA_PARAM param2;
} CRITERIA;

enum eCriteriaOp {
	opAND,
	opOR,
	opEQ,
	opNE,
	opLE,
	opLS,
	opGE,
	opGT,
};
*/

class cEventDB
{
public:
	cEventDB();
	~cEventDB();

	bool 
	Init (
		alFile* pEvtDBFile
		);
	
	bool 
	AddEvent (
		uint64   Time, 
		uint64   ObjId,
		uint64   ObjId2,
		uint32   UniqPid,
		eObjectType ObjectType,
		eEventType EventType,
		uint16   UserId,
//		tEventId BaseEvent,
		uint32   ExtraData,
		tEventId* pEventId
		);


	PEVENT_FIND_DATA
	FindFirstEvent (
		uint32    ObjectTypeFlags = 0, 
		uint32    EventTypeFlags = 0,
		tUniqPid  UniqPid = 0,
		uint64    ObjId = 0,
		size_t    nChildUniqPidsCount = 0,
		tUniqPid* arrChildUniqPids = 0,
		uint16    UserId = 0,
		tEventId  EventFrom = 0,
		tEventId  EventTo = 0,
		uint64    TimeFrom = 0,
		uint64    TimeTo = 0
		);
	
	bool 
	FindNextEvent (
		PEVENT_FIND_DATA pFindData
		);

	bool
	FindEventClose (
		PEVENT_FIND_DATA pFindData
		);

private:
	cPlainDB m_evtdb;
};

#endif // __EVENTDB_H_
