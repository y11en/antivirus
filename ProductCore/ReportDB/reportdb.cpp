// AVP Prague stamp begin( Interface header,  )
// -------- ANSI C++ Code Generator 1.0 --------
// -------- Monday,  29 October 2007,  10:14 --------
// -------------------------------------------
// Copyright © Kaspersky Lab 1996-2006.
// -------------------------------------------
// Project     -- Not defined
// Sub project -- Not defined
// Purpose     -- Not defined
// Author      -- Mezhuev
// File Name   -- reportdb.cpp
// -------------------------------------------
// AVP Prague stamp end



// AVP Prague stamp begin( Interface version,  )
#define ReportDB_VERSION ((tVERSION)1)
// AVP Prague stamp end



// AVP Prague stamp begin( Header includes,  )
#include <prague.h>
#include <iface/i_root.h>
#include <pr_vtbl.h>
#include <plugin/p_reportdb.h>
// AVP Prague stamp end



#include <prague/pr_cpp.h>

#include "rdb_smgr.h"
#include "rdb_helper_prague.h"

#include <report.h>

#include <Prague/iface/e_loader.h>
#include <pr_serializable.h>
#include <structs/s_report.h>

// AVP Prague stamp begin( C++ class declaration,  )
struct pr_novtable cReportDBImpl : public cReportDB 
{
private:
// Internal function declarations
	tERROR pr_call ObjectInitDone();
	tERROR pr_call ObjectPreClose();
	tERROR pr_call MsgReceive( tDWORD p_msg_cls_id, tDWORD p_msg_id, hOBJECT p_send_point, hOBJECT p_ctx, hOBJECT p_receive_point, tVOID* p_par_buf, tDWORD* p_par_buf_len );

// Property function declarations

public:
// External function declarations
	tERROR pr_call AddEvent( tDWORD p_db, const tPTR p_event, tDWORD p_size, tQWORD* p_nEventId );
	tERROR pr_call GetEvents( tDWORD p_db, int64 p_from, tPTR p_events, tDWORD p_buffsize, tDWORD p_count, tDWORD* p_readed, cSerializable* p_info );
	tERROR pr_call UpdateEvent( tQWORD p_nEventId, const tPTR p_event, tDWORD p_size );
	tERROR pr_call FindObject( tDWORD p_db, tDWORD p_objtype, const tPTR p_objid_data, tDWORD p_objid_data_size, tBOOL p_bCreate, tDWORD p_flags, tObjectId* p_pnObjectId );
	tERROR pr_call GetObjectName( tObjectId p_nObjectId, tPTR p_name_buff, tDWORD p_name_size, tDWORD* p_name_outsize, tDWORD p_flags, tDWORD* p_pnObjectType );
	tERROR pr_call GetObjectData( tObjectId p_nObjectId, tPTR p_data_buff, tDWORD p_data_size, tDWORD* p_data_outsize, tDWORD p_nDataType );
	tERROR pr_call SetObjectData( tObjectId p_nObjectId, const tPTR p_data_buff, tDWORD p_data_size, tDWORD p_nDataType );
	tERROR pr_call GetObjectInfo( tObjectId p_nObjectId, cSerializable* p_pInfo, tDWORD p_nInfoType );
	tERROR pr_call SetObjectInfo( tObjectId p_nObjectId, cSerializable* p_pInfo, tDWORD p_nInfoType );
	tERROR pr_call GetDBInfo( tDWORD p_db, tDWORD* p_records, tQWORD* p_datasize, tQWORD* p_timestamp );
	tERROR pr_call SetDBParams( tDWORD p_db, tDWORD p_maxDataSize, tDWORD p_maxPeriod, tDWORD p_maxSegmentSize, tDWORD p_maxSegmentPeriod );
	tERROR pr_call InitDB( tDWORD p_db, tDWORD p_nEventSize );
	tERROR pr_call ClearDB( tDWORD p_db );
	tERROR pr_call FindObjectEx( tDWORD p_db, tDWORD p_objtype, const tPTR p_objid_data, tDWORD p_objid_data_size, tBOOL p_bCreate, tDWORD p_flags, tObjectId p_nParentObjectId, tObjectId* p_pnObjectId );

// Data declaration
// AVP Prague stamp end



	cReportDBHelperPrague m_helper_global;
	cReportDBHelperPrague m_helper[dbMaxReportDatabaseId];
	cReportDBSegmentMgr  m_segmgr[dbMaxReportDatabaseId];
	static cObjectDB *   m_objdb;

// AVP Prague stamp begin( C++ class declaration end,  )
public:
	mDECLARE_INITIALIZATION(cReportDBImpl)
};
// AVP Prague stamp end



// AVP Prague stamp begin( Global property variable declaration,  )
// Interface "ReportDB". Static(shared) property table variables
// AVP Prague stamp end



cObjectDB * cReportDBImpl::m_objdb = 0;
static tLONG g_nObjDBCounter = 0;

#define IS_VALID_DATABASE_ID(db) (db < dbMaxReportDatabaseId)

#define GET_OBJECT_DB(objid) ((tDWORD)(((uint64)objid) >> 56))
#define CLEAR_OBJECT_DB(objid) objid &= 0x00FFFFFFFFFFFFFFL;
#define SET_OBJECT_DB(objid, db) (objid = ((objid)|(((uint64)db) << 56)))

// AVP Prague stamp begin( Internal (kernel called) interface method implementation, ObjectInitDone )
tERROR cReportDBImpl::ObjectInitDone()
{
	PR_TRACE_FUNC_FRAME( "ReportDB::ObjectInitDone method" );

	// Place your code here

	// init data database
	cStrObj sPath = "%Report%";
	sysSendMsg(pmc_PRODUCT_ENVIRONMENT, pm_EXPAND_ENVIRONMENT_STRING, (hOBJECT)cAutoString(sPath), 0, 0);
	sPath.check_last_slash();


	if(PrInterlockedIncrement(&g_nObjDBCounter) == 1)
	{
		if( !m_helper_global.Init(sPath.data(), false) )
		{
			PrInterlockedDecrement(&g_nObjDBCounter);
			return errOBJECT_CANNOT_BE_INITIALIZED;
		}
		m_objdb = new cObjectDB();
		alFile *pDBT, *pDBI, *pDBD;
		m_helper_global.OpenFile(L"g_objbt.dat", 0, true, true, &pDBT);
		m_helper_global.OpenFile(L"g_objid.dat", 0, true, true, &pDBI);
		m_helper_global.OpenFile(L"g_objdt.dat", 0, true, true, &pDBD);

		if( PR_FAIL(m_objdb->Init(0, pDBT, pDBI, pDBD)) )
		{
			if (pDBT) pDBT->Release();
			if (pDBI) pDBI->Release();
			if (pDBD) pDBD->Release();
			return errOBJECT_CANNOT_BE_INITIALIZED;
		}
	}

	// init all databases => moved into InitDB, now init only Global Objects
	sPath.append(L"00\\");
	if( !m_helper[dbGlobalObjects].Init(sPath.data(), false) )
		return errOBJECT_CANNOT_BE_INITIALIZED;
	tERROR error = m_segmgr[dbGlobalObjects].Init(&m_helper[dbGlobalObjects], 1, cTRUE, m_objdb);
	return error;
}
// AVP Prague stamp end



// AVP Prague stamp begin( Internal (kernel called) interface method implementation, ObjectPreClose )
tERROR cReportDBImpl::ObjectPreClose()
{
	tERROR error = errOK;
	PR_TRACE_FUNC_FRAME( "ReportDB::ObjectPreClose method" );

	if (PrInterlockedDecrement(&g_nObjDBCounter) == 0)
	{
		delete m_objdb;
		m_objdb = NULL;
	}

	return error;
}
// AVP Prague stamp end



// AVP Prague stamp begin( Internal (kernel called) interface method implementation, MsgReceive )
tERROR cReportDBImpl::MsgReceive( tDWORD p_msg_cls_id, tDWORD p_msg_id, hOBJECT p_send_point, hOBJECT p_ctx, hOBJECT p_receive_point, tVOID* p_par_buf, tDWORD* p_par_buf_len )
{
	tERROR error = errOK;
	PR_TRACE_FUNC_FRAME( "ReportDB::MsgReceive method" );

	// Place your code here

	return error;
}
// AVP Prague stamp end



// AVP Prague stamp begin( External (user called) interface method implementation, AddEvent )
tERROR cReportDBImpl::AddEvent( tDWORD p_db, const tPTR p_event, tDWORD p_size, tQWORD* p_nEventId )
{
	tERROR error = errOK;
	PR_TRACE_FUNC_FRAME( "ReportDB::AddEvent method" );

	// Place your code here
	if (!IS_VALID_DATABASE_ID(p_db))
		return errPARAMETER_INVALID;
	tEventId evid;
	error = m_segmgr[p_db].AddEvent(p_event, p_size, &evid);
	evid = SET_OBJECT_DB(evid, p_db);
	if (p_nEventId)
		*p_nEventId = evid;
	return error;
}
// AVP Prague stamp end



// AVP Prague stamp begin( External (user called) interface method implementation, GetEvents )
tERROR cReportDBImpl::GetEvents( tDWORD db, int64 p_from, tPTR p_events, tDWORD p_buffsize, tDWORD p_count, tDWORD* p_readed, cSerializable* p_info )
{
	tERROR error;
	PR_TRACE_FUNC_FRAME( "ReportDB::GetEvents method" );

	// Place your code here
	tDWORD temp_db = GET_OBJECT_DB(p_from);
	if (temp_db!=0 && temp_db!=0xFF)
	{
		CLEAR_OBJECT_DB(p_from);
		db = temp_db;
	}
	if (!IS_VALID_DATABASE_ID(db))
		return errPARAMETER_INVALID;

	error = m_segmgr[db].GetEvents(p_from, p_events, p_count, p_buffsize, p_readed);

	if( PR_SUCC(error) && p_info && p_info->isBasedOn(cReportParentIds::eIID) && p_readed )
	{
		cReportParentIds* pids = (cReportParentIds*)p_info;

		tDWORD i, n = *p_readed;
		tReportEvent* evt = (tReportEvent*)p_events;
		for(i = 0; i < n; i++, evt++)
		{
			uint8 nObjectType = 0;
			tQWORD id = evt->m_ObjectID, idParent = 0;
			tDWORD odb = GET_OBJECT_DB(id);
			if( IS_VALID_DATABASE_ID(odb) )
			{
				CLEAR_OBJECT_DB(id);
				m_segmgr[odb].GetObjectParent(id, &idParent, &nObjectType);
				if( idParent )
					SET_OBJECT_DB(idParent, odb);
			}
			pids->m_vParentIds.push_back(idParent);
		}
	}
	return error;
}
// AVP Prague stamp end



// AVP Prague stamp begin( External (user called) interface method implementation, UpdateEvent )
tERROR cReportDBImpl::UpdateEvent( tQWORD nEventId, const tPTR event, tDWORD size )
{
	tERROR error;
	PR_TRACE_FUNC_FRAME( "ReportDB::UpdateEvent method" );

	// Place your code here
	tDWORD db = GET_OBJECT_DB(nEventId);
	if (!IS_VALID_DATABASE_ID(db))
		return errPARAMETER_INVALID;
	CLEAR_OBJECT_DB(nEventId);
	error = m_segmgr[db].UpdateEvent(nEventId, event, size);
	return error;
}
// AVP Prague stamp end



// AVP Prague stamp begin( External (user called) interface method implementation, FindObject )
tERROR cReportDBImpl::FindObject( tDWORD db, tDWORD p_objtype, const tPTR p_name, tDWORD p_name_size, tBOOL p_bCreate, tDWORD p_flags, tObjectId* p_result )
{
	tERROR error = errOK;
	PR_TRACE_FUNC_FRAME( "ReportDB::FindObject method" );

	error = FindObjectEx(db, p_objtype, p_name, p_name_size, p_bCreate, p_flags, 0, p_result );

	return error;
}
// AVP Prague stamp end


// AVP Prague stamp begin( External (user called) interface method implementation, FindObjectEx )
// Parameters are:
tERROR cReportDBImpl::FindObjectEx( tDWORD db, tDWORD p_objtype, const tPTR p_name, tDWORD p_name_size, tBOOL p_bCreate, tDWORD p_flags, tObjectId p_nParentObjectId, tObjectId* p_pnObjectId )
{
	tERROR error = errOK;
	tObjectId objid;
	PR_TRACE_FUNC_FRAME( "ReportDB::FindObjectEx method" );

	if (!IS_VALID_DATABASE_ID(db))
		return errPARAMETER_INVALID;
	error = m_segmgr[db].FindObject(db == dbGlobalObjects, p_objtype, p_flags, p_name, p_name_size, !!p_bCreate, p_nParentObjectId, &objid);
	if (PR_SUCC(error) && p_pnObjectId)
	{
		objid = SET_OBJECT_DB(objid, db);
		*p_pnObjectId = objid;
	}
	return error;
}
// AVP Prague stamp end


// AVP Prague stamp begin( External (user called) interface method implementation, GetObjectData )
tERROR cReportDBImpl::GetObjectData( tObjectId p_id, tPTR p_data_buff, tDWORD p_data_size, tDWORD* p_data_outsize, tDWORD nDataType )
{
	tERROR error;
	PR_TRACE_FUNC_FRAME( "ReportDB::SetObjectData method" );

	tDWORD db = GET_OBJECT_DB(p_id);
	if (!IS_VALID_DATABASE_ID(db))
		return errPARAMETER_INVALID;
	CLEAR_OBJECT_DB(p_id);
	error = m_segmgr[db].GetObjectData(p_id, p_data_buff, p_data_size, p_data_outsize, nDataType);

	return error;
}
// AVP Prague stamp end



// AVP Prague stamp begin( External (user called) interface method implementation, SetObjectData )
tERROR cReportDBImpl::SetObjectData( tObjectId p_id, const tPTR p_data_buff, tDWORD p_data_size, tDWORD nDataType )
{
	tERROR error;
	PR_TRACE_FUNC_FRAME( "ReportDB::SetObjectData method" );

	tDWORD db = GET_OBJECT_DB(p_id);
	if (!IS_VALID_DATABASE_ID(db))
		return errPARAMETER_INVALID;
	CLEAR_OBJECT_DB(p_id);
	error = m_segmgr[db].SetObjectData(p_id, p_data_buff, p_data_size, nDataType);

	return error;
}
// AVP Prague stamp end



// AVP Prague stamp begin( External (user called) interface method implementation, GetObjectName )
tERROR cReportDBImpl::GetObjectName( tObjectId p_id, tPTR p_name_buff, tDWORD p_name_size, tDWORD* p_name_outsize, tDWORD p_flags, tDWORD* p_pnObjectType )
{
	tERROR error = errOK;
	PR_TRACE_FUNC_FRAME( "ReportDB::GetObjectInfo method" );

	// Place your code here
	tDWORD db = GET_OBJECT_DB(p_id);
	if (!IS_VALID_DATABASE_ID(db))
		return errPARAMETER_INVALID;
	CLEAR_OBJECT_DB(p_id);

	uint8 nObjectType;
	if( p_name_buff || p_name_size || p_pnObjectType)
		error = m_segmgr[db].GetObjectName(p_id, p_name_buff, p_name_size, p_name_outsize, p_flags, &nObjectType);
	if (p_pnObjectType)
		*p_pnObjectType = nObjectType;
	return error;
}
// AVP Prague stamp end



// AVP Prague stamp begin( External (user called) interface method implementation, GetObjectInfo )
tERROR cReportDBImpl::GetObjectInfo( tObjectId p_id, cSerializable* p_pSerializable, tDWORD nInfoType  )
{
	tERROR error = errOK;
	PR_TRACE_FUNC_FRAME( "ReportDB::GetObjectInfoEx method" );

	// Place your code here
	if (!p_pSerializable)
		return errPARAMETER_INVALID;

	tDWORD db = GET_OBJECT_DB(p_id);
	if (!IS_VALID_DATABASE_ID(db))
		return errPARAMETER_INVALID;
	CLEAR_OBJECT_DB(p_id);

	if( p_pSerializable && p_pSerializable->isBasedOn(cReportParentIds::eIID) )
	{
		cReportParentIds* pids = (cReportParentIds*)p_pSerializable;
		for(tQWORD idParent, id = p_id; id; id = idParent )
		{
			CLEAR_OBJECT_DB(id);
			m_segmgr[db].GetObjectParent(id, &idParent, NULL);
			if( idParent )
			{
				SET_OBJECT_DB(idParent, db);
				pids->m_vParentIds.push_back(idParent);
			}
		}
		return errOK;
	}

	tChunckBuff buff; tDWORD nSize = 0;
	error = m_segmgr[db].GetObjectData(p_id, (tPTR)buff.ptr(), buff.count(),  &nSize, nInfoType);
	if( error == errBUFFER_TOO_SMALL )
		error = m_segmgr[db].GetObjectData(p_id, (tPTR)buff.get(nSize, false), nSize, NULL, nInfoType);
	if( PR_SUCC(error) )
		error = g_root->StreamDeserialize(&p_pSerializable, (const tBYTE*)buff.ptr(), buff.size(), NULL);
	return error;
}
// AVP Prague stamp end



// AVP Prague stamp begin( External (user called) interface method implementation, SetObjectInfo )
tERROR cReportDBImpl::SetObjectInfo( tObjectId p_id, cSerializable* p_pSerializable, tDWORD nInfoType  )
{
	tERROR error = errOK;
	PR_TRACE_FUNC_FRAME( "ReportDB::SetObjectInfoEx method" );

	tDWORD db = GET_OBJECT_DB(p_id);
	if (!IS_VALID_DATABASE_ID(db))
		return errPARAMETER_INVALID;
	CLEAR_OBJECT_DB(p_id);

	tChunckBuff buff; tDWORD size = 0;
	error = g_root->StreamSerialize(p_pSerializable, SERID_UNKNOWN, (tBYTE*)buff.ptr(), buff.size(), &size);
	if( error == errBUFFER_TOO_SMALL )
		error = g_root->StreamSerialize(p_pSerializable, SERID_UNKNOWN, (tBYTE*)buff.get(size, false), size, NULL);
	if( PR_SUCC(error) )
		error = m_segmgr[db].SetObjectData(p_id, buff, size, nInfoType);

	return error;
}
// AVP Prague stamp end



// AVP Prague stamp begin( External (user called) interface method implementation, GetDBInfo )
tERROR cReportDBImpl::GetDBInfo( tDWORD db, tDWORD* p_records, tQWORD* p_datasize, tQWORD* p_timestamp )
{
	tERROR error = errOK;
	PR_TRACE_FUNC_FRAME( "ReportDB::GetDBInfo method" );

	if (db == dbAllDatabases)
	{
		// TODO: return count for all databases
		return errNOT_IMPLEMENTED;
	}

	if (!IS_VALID_DATABASE_ID(db))
		return errPARAMETER_INVALID;

	if( p_records )
		*p_records = (tDWORD)m_segmgr[db].GetRecordsCount();

	if( p_datasize )
		*p_datasize = m_segmgr[db].GetSize();

	if (p_timestamp)
		*p_timestamp = 0; // TODO: return real timestamp

	return error;
}
// AVP Prague stamp end



// AVP Prague stamp begin( External (user called) interface method implementation, SetDBParams )
tERROR cReportDBImpl::SetDBParams( tDWORD db, tDWORD p_maxDataSize, tDWORD p_maxPeriod, tDWORD p_maxSegmentSize, tDWORD p_maxSegmentPeriod )
{
	tERROR error = errOK;
	PR_TRACE_FUNC_FRAME( "ReportDB::SetDBParams method" );

	if (db == dbAllDatabases)
	{
		// TODO: set counts for all databases
		return errNOT_IMPLEMENTED;
	}

	if (!IS_VALID_DATABASE_ID(db))
		return errPARAMETER_INVALID;

	m_segmgr[db].SetMaxReportSize(p_maxDataSize);
	m_segmgr[db].SetMaxReportTime(p_maxPeriod);
	m_segmgr[db].SetMaxSegmentSize(p_maxSegmentSize);
	m_segmgr[db].SetMaxSegmentTime(p_maxSegmentPeriod);

	return error;
}
// AVP Prague stamp end



// AVP Prague stamp begin( External (user called) interface method implementation, ClearDB )
tERROR cReportDBImpl::ClearDB( tDWORD p_db )
{
	tERROR error = errOK;
	PR_TRACE_FUNC_FRAME( "ReportDB::ClearDB method" );

	if (p_db == dbAllDatabases)
	{
		tERROR err_tmp;
		error = errOK;
		for (tDWORD i=1; i<dbMaxReportDatabaseId; i++)
		{
			err_tmp = ClearDB(i);
			if (PR_FAIL(err_tmp))
				error = err_tmp;
		}
		return error;
	}

	if (!IS_VALID_DATABASE_ID(p_db))
		return errPARAMETER_INVALID;

	error = m_segmgr[p_db].Clear();
	return error;
}
// AVP Prague stamp end


// AVP Prague stamp begin( External (user called) interface method implementation, InitDB )
tERROR cReportDBImpl::InitDB( tDWORD db, tDWORD nEventSize )
{
	tERROR error = errOK;
	PR_TRACE_FUNC_FRAME( "ReportDB::InitDB method" );
	if (!IS_VALID_DATABASE_ID(db))
		return errPARAMETER_INVALID;

	cStrObj sPath = "%Report%";
	sysSendMsg(pmc_PRODUCT_ENVIRONMENT, pm_EXPAND_ENVIRONMENT_STRING, (hOBJECT)cAutoString(sPath), 0, 0);
	sPath.check_last_slash();
	tWCHAR subdir[5] = L"00\\";
	subdir[0] = ((db & 0xF0) << 4) + '0';
	subdir[1] = (db & 0x0F) + '0';
	sPath.append(subdir);
	if( !m_helper[db].Init(sPath.data(), false) )
		return errOBJECT_CANNOT_BE_INITIALIZED;
	error = m_segmgr[db].Init(&m_helper[db], nEventSize, true, m_objdb);
	return error;
}
// AVP Prague stamp end



// AVP Prague stamp begin( C++ class regitration,  )
// Interface "ReportDB". Register function
tERROR cReportDBImpl::Register( hROOT root ) 
{
	tERROR error;
// AVP Prague stamp end



// AVP Prague stamp begin( Property table,  )
mpPROPERTY_TABLE(ReportDB_PropTable)
	mSHARED_PROPERTY( pgINTERFACE_VERSION, ReportDB_VERSION )
	mSHARED_PROPERTY_PTR( pgINTERFACE_COMMENT, "ReportDB", 9 )
mpPROPERTY_TABLE_END(ReportDB_PropTable)
// AVP Prague stamp end



// AVP Prague stamp begin( Internal method table,  )
mINTERNAL_TABLE_BEGIN(ReportDB)
	mINTERNAL_METHOD(ObjectInitDone)
	mINTERNAL_METHOD(ObjectPreClose)
	mINTERNAL_METHOD(MsgReceive)
mINTERNAL_TABLE_END(ReportDB)
// AVP Prague stamp end



// AVP Prague stamp begin( External method table,  )
mEXTERNAL_TABLE_BEGIN(ReportDB)
	mEXTERNAL_METHOD(ReportDB, AddEvent)
	mEXTERNAL_METHOD(ReportDB, GetEvents)
	mEXTERNAL_METHOD(ReportDB, UpdateEvent)
	mEXTERNAL_METHOD(ReportDB, FindObject)
	mEXTERNAL_METHOD(ReportDB, GetObjectName)
	mEXTERNAL_METHOD(ReportDB, GetObjectData)
	mEXTERNAL_METHOD(ReportDB, SetObjectData)
	mEXTERNAL_METHOD(ReportDB, GetObjectInfo)
	mEXTERNAL_METHOD(ReportDB, SetObjectInfo)
	mEXTERNAL_METHOD(ReportDB, GetDBInfo)
	mEXTERNAL_METHOD(ReportDB, SetDBParams)
	mEXTERNAL_METHOD(ReportDB, InitDB)
	mEXTERNAL_METHOD(ReportDB, ClearDB)
	mEXTERNAL_METHOD(ReportDB, FindObjectEx)
mEXTERNAL_TABLE_END(ReportDB)
// AVP Prague stamp end



// AVP Prague stamp begin( Registration call,  )

	PR_TRACE_FUNC_FRAME_( *root, "ReportDB::Register method", &error );

	error = CALL_Root_RegisterIFace(
		root,                                   // root object
		IID_REPORTDB,                           // interface identifier
		PID_REPORTDB,                           // plugin identifier
		0x00000000,                             // subtype identifier
		ReportDB_VERSION,                       // interface version
		VID_MEZHUEV,                            // interface developer
		&i_ReportDB_vtbl,                       // internal(kernel called) function table
		(sizeof(i_ReportDB_vtbl)/sizeof(tPTR)), // internal function table size
		&e_ReportDB_vtbl,                       // external function table
		(sizeof(e_ReportDB_vtbl)/sizeof(tPTR)), // external function table size
		ReportDB_PropTable,                     // property table
		mPROPERTY_TABLE_SIZE(ReportDB_PropTable),// property table size
		sizeof(cReportDBImpl)-sizeof(cObjImpl), // memory size
		0                                       // interface flags
	);

	#ifdef _PRAGUE_TRACE_
		if ( PR_FAIL(error) )
			PR_TRACE( (root,prtDANGER,"ReportDB(IID_REPORTDB) registered [%terr]",error) );
	#endif // _PRAGUE_TRACE_
// AVP Prague stamp end



// AVP Prague stamp begin( C++ class regitration end,  )
	return error;
}

tERROR pr_call ReportDB_Register( hROOT root ) { return cReportDBImpl::Register(root); }
// AVP Prague stamp end



