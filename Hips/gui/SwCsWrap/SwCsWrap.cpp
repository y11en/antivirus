#include <crtdefs.h>
#include <string>
#include <al_file_win.h>
#include <syswatch.h>

#using <mscorlib.dll>
#using <System.dll>
#using <System.Data.dll>

using namespace System;
using namespace System::Data;
using namespace System::Runtime::InteropServices;

class SharpStr2WcharStr
{
public:
	SharpStr2WcharStr(System::String^ str) { m_strBuff = Marshal::StringToHGlobalUni(str); }
	~SharpStr2WcharStr()                   { Marshal::FreeHGlobal(m_strBuff); }
	
	operator const wchar_t *()             { return (const wchar_t *)m_strBuff.ToPointer(); }

protected:
	IntPtr m_strBuff;
};

class WcharStr2SharpStr
{
public:
	WcharStr2SharpStr(const wchar_t *str) : m_str(str) {}

	operator System::String^()            { return Marshal::PtrToStringUni((IntPtr)(wchar_t *)m_str); }

protected:
	const wchar_t * m_str;
};

typedef std::wstring wstring;



//////////////////////////////////////////////////////////////////////////
// cSystemWatcherData

class cSystemWatcherData : public cSystemWatcher
{
public:
	class cMemChunk {};



	bool Init();
	bool GetProcessInfo(DataSet^ ds, uint32 uniqPid);
	bool GetEventsInfo(DataSet^ ds, uint32 uniqPid, uint64 objId);

protected:
	bool FetchObjInfo(uint64 objId, wstring &objName, cMemChunk &extraInfo);

protected:
	alAutoRelease<alFileWin32*> m_objdb;
	alAutoRelease<alFileWin32*> m_evtdb;
	alAutoRelease<alFileWin32*> m_procdb;
};

bool cSystemWatcherData::Init()
{
	m_objdb = new_alFileWin32();
	if( !m_objdb )
		return false;
	if( !m_objdb->CreateFileW(L"db_hashes.dat", GENERIC_READ, FILE_SHARE_READ, 0, OPEN_EXISTING, 0, 0) )
		return false;

	m_evtdb = new_alFileWin32();
	if( !m_evtdb )
		return false;
	if( !m_evtdb->CreateFileW(L"db_events.dat", GENERIC_READ, FILE_SHARE_READ, 0, OPEN_EXISTING, 0, 0) )
		return false;

	m_procdb = new_alFileWin32();
	if( !m_procdb )
		return false;
	if( !m_procdb->CreateFileW(L"db_procs.dat", GENERIC_READ, FILE_SHARE_READ, 0, OPEN_EXISTING, 0, 0) )
		return false;

	return cSystemWatcher::Init(m_evtdb, m_objdb, m_procdb);
}

bool cSystemWatcherData::FetchObjInfo(uint64 objId, wstring &objName, cMemChunk &extraInfo)
{
	wchar_t objNameBuff[32 * 1024];
	size_t  objNameBuffSize = sizeof(objNameBuff);
	char    extraInfoBuff[32 * 1024];
	size_t  extraInfoBuffSize = sizeof(objNameBuff);
	if( GetHashData(objId, NULL, objNameBuff, objNameBuffSize, &objNameBuffSize, extraInfoBuff, extraInfoBuffSize, &extraInfoBuffSize) )
	{
		objNameBuff[objNameBuffSize] = 0;
		objName = objNameBuff;
		return true;
	}
	return false;
}

bool cSystemWatcherData::GetProcessInfo(DataSet^ ds, uint32 uniqPid)
{
	if( !uniqPid )
		return false;

	uint64 objId;
	cProcessDB::RECORD procInfo;
	if( !GetProcRecord(uniqPid, &procInfo) )
		return false;

	EVENT_FIND_DATA *pFD = FindFirstEvent(0, 0, 0, 0, 0, 0, 0, procInfo.start_event);
	if( !pFD )
		return false;
	objId = pFD->ObjId;
	FindEventClose(pFD);

	wstring objectName;
	cMemChunk objectExtraInfo;
	if( !FetchObjInfo(objId, objectName, objectExtraInfo) )
		return false;

	DataTable^ procTable = ds->Tables->Add("processInfo");
	procTable->Columns->Add("imagePath");
	procTable->Columns->Add("startEvent",   UInt32().GetType());
	procTable->Columns->Add("finishEvent",  UInt32().GetType());
	procTable->Columns->Add("startTime",    UInt64().GetType());
	procTable->Columns->Add("finishTime",   UInt64().GetType());
	procTable->Columns->Add("parentUnique", UInt32().GetType());
	procTable->Columns->Add("unique",       UInt32().GetType());

	DataRow^ row = procTable->NewRow();
	row["imagePath"] = WcharStr2SharpStr(objectName.c_str());
	row["startEvent"] = procInfo.start_event;
	row["finishEvent"] = procInfo.exit_event;
	row["startTime"] = procInfo.start_time;
	row["finishTime"] = procInfo.exit_time;
	row["parentUnique"] = procInfo.parent_uniq;
	row["unique"] = uniqPid;
	procTable->Rows->Add(row);

	return true;
}

bool cSystemWatcherData::GetEventsInfo(DataSet^ ds, uint32 uniqPid, uint64 objId)
{
	if( uniqPid )
		GetProcessInfo(ds, uniqPid);

	DataTable^ eventsList = ds->Tables->Add("eventsList");
	eventsList->Columns->Add("time",         UInt64().GetType());
	eventsList->Columns->Add("type",         UInt32().GetType());
	eventsList->Columns->Add("action",       UInt32().GetType());
	eventsList->Columns->Add("objectID",     UInt64().GetType());
	eventsList->Columns->Add("process",      UInt32().GetType());
	eventsList->Columns->Add("childProcess", UInt32().GetType());
	eventsList->Columns->Add("description");

	if( EVENT_FIND_DATA *pFD = FindFirstEvent(0, 0, uniqPid, objId) )
	{
		do
		{
			wstring objectName;
			cMemChunk objectExtraInfo;
			if( !FetchObjInfo(pFD->ObjId, objectName, objectExtraInfo) )
				objectName = L"unknown";

			DataRow^ row = eventsList->NewRow();
			row["time"] = pFD->Time;
			row["type"] = pFD->ObjectType;
			row["action"] = pFD->EventType;
			row["objectID"] = pFD->ObjId;
			row["process"] = pFD->UniqPid;
			row["description"] = WcharStr2SharpStr(objectName.c_str());
			if( pFD->EventType == evtProcessStart )
				row["childProcess"] = pFD->ExtraData;
			eventsList->Rows->Add(row);
		} while(FindNextEvent(pFD));
		FindEventClose(pFD);
	}
	
	return true;
}

namespace SystemWatcher
{
	public ref class DataSource : public DataSet
	{
	public:
		DataSource() { m_pData = new cSystemWatcherData; m_pData->Init(); }
		~DataSource() { delete m_pData; m_pData = NULL; }
		
		void FetchInfo(uint32 uniquePid, uint64 objId) { m_pData->GetEventsInfo(this, uniquePid, objId); }
	
	protected:
		cSystemWatcherData *m_pData;
	};
};