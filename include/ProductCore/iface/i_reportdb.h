// AVP Prague stamp begin( Interface header,  )
// -------- Monday,  29 October 2007,  10:05 --------
// File Name   -- (null)i_reportdb.cpp
// Copyright © Kaspersky Lab 1996-2006.
// -------------------------------------------
// AVP Prague stamp end



// AVP Prague stamp begin( Header ifndef,  )
#if !defined( __i_reportdb__33d2cdbc_ada0_438c_b730_6114873bed26 )
#define __i_reportdb__33d2cdbc_ada0_438c_b730_6114873bed26
// AVP Prague stamp end



typedef __int64          int64;
typedef unsigned __int64 uint64;
typedef uint64           tObjectId;


// AVP Prague stamp begin( Header includes,  )
#include <Prague/pr_vtbl.h>
// AVP Prague stamp end



// AVP Prague stamp begin( Interface comment,  )
// ReportDB interface implementation
// AVP Prague stamp end



// AVP Prague stamp begin( Interface menonic identificator,  )
#define IID_REPORTDB  ((tIID)(49036))
// AVP Prague stamp end



// AVP Prague stamp begin( Interface typedefs,  )
// AVP Prague stamp end



// AVP Prague stamp begin( Interface constants,  )
// AVP Prague stamp end



// AVP Prague stamp begin( Interface defined messages,  )
// AVP Prague stamp end



// AVP Prague stamp begin( Interface defined errors,  )
// AVP Prague stamp end



// AVP Prague stamp begin( RPC start declaration,  )
#if defined( __RPC__ )
	typedef tUINT hREPORTDB;
#else
// AVP Prague stamp end



// AVP Prague stamp begin( Interface forward declaration,  )
struct iReportDBVtbl;
typedef struct iReportDBVtbl iReportDBVtbl;
// AVP Prague stamp end



// AVP Prague stamp begin( Object declaration,  )
#if defined( __cplusplus ) && !defined(_USE_VTBL)
	struct cReportDB;
	typedef cReportDB* hREPORTDB;
#else
	typedef struct tag_hREPORTDB {
		const iReportDBVtbl* vtbl; // pointer to the "ReportDB" virtual table
		const iSYSTEMVtbl*   sys;  // pointer to the "SYSTEM" virtual table
	} *hREPORTDB;
#endif // if defined( __cplusplus )
// AVP Prague stamp end



// AVP Prague stamp begin( RPC_ end declaration,  )
#endif // if defined( __RPC__ )
// AVP Prague stamp end



// AVP Prague stamp begin( Private definitions begin,  )
#if !defined( ReportDB_PRIVATE_DEFINITION )
// AVP Prague stamp end



// AVP Prague stamp begin( Interface function forward declarations,  )


	typedef   tERROR (pr_call *fnpReportDB_AddEvent)      ( hREPORTDB _this, tDWORD db, const tPTR event, tDWORD size, tQWORD* nEventId ); // -- ;
	typedef   tERROR (pr_call *fnpReportDB_GetEvents)     ( hREPORTDB _this, tDWORD db, int64 from, tPTR events, tDWORD buffsize, tDWORD count, tDWORD* readed, cSerializable* pInfo ); // -- ;
	typedef   tERROR (pr_call *fnpReportDB_UpdateEvent)   ( hREPORTDB _this, tQWORD nEventId, const tPTR event, tDWORD size ); // -- ;
	typedef   tERROR (pr_call *fnpReportDB_FindObject)    ( hREPORTDB _this, tDWORD db, tDWORD objtype, const tPTR objid_data, tDWORD objid_data_size, tBOOL bCreate, tDWORD flags, tObjectId* pnObjectId ); // -- ;
	typedef   tERROR (pr_call *fnpReportDB_GetObjectName) ( hREPORTDB _this, tObjectId nObjectId, tPTR name_buff, tDWORD name_size, tDWORD* name_outsize, tDWORD flags, tDWORD* pnObjectType ); // -- ;
	typedef   tERROR (pr_call *fnpReportDB_GetObjectData) ( hREPORTDB _this, tObjectId nObjectId, tPTR data_buff, tDWORD data_size, tDWORD* data_outsize, tDWORD nDataType ); // -- ;
	typedef   tERROR (pr_call *fnpReportDB_SetObjectData) ( hREPORTDB _this, tObjectId nObjectId, const tPTR data_buff, tDWORD data_size, tDWORD nDataType ); // -- ;
	typedef   tERROR (pr_call *fnpReportDB_GetObjectInfo) ( hREPORTDB _this, tObjectId nObjectId, cSerializable* pInfo, tDWORD nInfoType ); // -- ;
	typedef   tERROR (pr_call *fnpReportDB_SetObjectInfo) ( hREPORTDB _this, tObjectId nObjectId, cSerializable* pInfo, tDWORD nInfoType ); // -- ;
	typedef   tERROR (pr_call *fnpReportDB_GetDBInfo)     ( hREPORTDB _this, tDWORD db, tDWORD* records, tQWORD* datasize, tQWORD* timestamp ); // -- ;
	typedef   tERROR (pr_call *fnpReportDB_SetDBParams)   ( hREPORTDB _this, tDWORD db, tDWORD maxDataSize, tDWORD maxPeriod, tDWORD maxSegmentSize, tDWORD maxSegmentPeriod ); // -- ;
	typedef   tERROR (pr_call *fnpReportDB_InitDB)        ( hREPORTDB _this, tDWORD db, tDWORD nEventSize ); // -- ;
	typedef   tERROR (pr_call *fnpReportDB_ClearDB)       ( hREPORTDB _this, tDWORD db );       // -- ;
	typedef   tERROR (pr_call *fnpReportDB_FindObjectEx)  ( hREPORTDB _this, tDWORD db, tDWORD objtype, const tPTR objid_data, tDWORD objid_data_size, tBOOL bCreate, tDWORD flags, tObjectId nParentObjectId, tObjectId* pnObjectId ); // -- ;


// AVP Prague stamp end



// AVP Prague stamp begin( Interface declaration,  )
struct iReportDBVtbl {
	fnpReportDB_AddEvent       AddEvent;
	fnpReportDB_GetEvents      GetEvents;
	fnpReportDB_UpdateEvent    UpdateEvent;
	fnpReportDB_FindObject     FindObject;
	fnpReportDB_GetObjectName  GetObjectName;
	fnpReportDB_GetObjectData  GetObjectData;
	fnpReportDB_SetObjectData  SetObjectData;
	fnpReportDB_GetObjectInfo  GetObjectInfo;
	fnpReportDB_SetObjectInfo  SetObjectInfo;
	fnpReportDB_GetDBInfo      GetDBInfo;
	fnpReportDB_SetDBParams    SetDBParams;
	fnpReportDB_InitDB         InitDB;
	fnpReportDB_ClearDB        ClearDB;
	fnpReportDB_FindObjectEx   FindObjectEx;
}; // "ReportDB" external virtual table prototype
// AVP Prague stamp end



// AVP Prague stamp begin( Private definitions end,  )
#endif // if !defined( ReportDB_PRIVATE_DEFINITION )
// AVP Prague stamp end



// AVP Prague stamp begin( Interface declared properties,  )
//! #define pgINTERFACE_VERSION mPROPERTY_MAKE_GLOBAL( pTYPE_VERSION , 0x00000082 )
//! #define pgINTERFACE_COMMENT mPROPERTY_MAKE_GLOBAL( pTYPE_STRING  , 0x00000083 )
// AVP Prague stamp end



// AVP Prague stamp begin( Defines to simplify interface method calls,  )
#if !defined( __cplusplus ) || defined(_USE_VTBL)
	#define CALL_ReportDB_AddEvent( _this, db, event, size, nEventId )                                                                              ((_this)->vtbl->AddEvent( (_this), db, event, size, nEventId ))
	#define CALL_ReportDB_GetEvents( _this, db, from, events, buffsize, count, readed, pInfo )                                                      ((_this)->vtbl->GetEvents( (_this), db, from, events, buffsize, count, readed, pInfo ))
	#define CALL_ReportDB_UpdateEvent( _this, nEventId, event, size )                                                                               ((_this)->vtbl->UpdateEvent( (_this), nEventId, event, size ))
	#define CALL_ReportDB_FindObject( _this, db, objtype, objid_data, objid_data_size, bCreate, flags, pnObjectId )                                 ((_this)->vtbl->FindObject( (_this), db, objtype, objid_data, objid_data_size, bCreate, flags, pnObjectId ))
	#define CALL_ReportDB_GetObjectName( _this, nObjectId, name_buff, name_size, name_outsize, flags, pnObjectType )                                ((_this)->vtbl->GetObjectName( (_this), nObjectId, name_buff, name_size, name_outsize, flags, pnObjectType ))
	#define CALL_ReportDB_GetObjectData( _this, nObjectId, data_buff, data_size, data_outsize, nDataType )                                          ((_this)->vtbl->GetObjectData( (_this), nObjectId, data_buff, data_size, data_outsize, nDataType ))
	#define CALL_ReportDB_SetObjectData( _this, nObjectId, data_buff, data_size, nDataType )                                                        ((_this)->vtbl->SetObjectData( (_this), nObjectId, data_buff, data_size, nDataType ))
	#define CALL_ReportDB_GetObjectInfo( _this, nObjectId, pInfo, nInfoType )                                                                       ((_this)->vtbl->GetObjectInfo( (_this), nObjectId, pInfo, nInfoType ))
	#define CALL_ReportDB_SetObjectInfo( _this, nObjectId, pInfo, nInfoType )                                                                       ((_this)->vtbl->SetObjectInfo( (_this), nObjectId, pInfo, nInfoType ))
	#define CALL_ReportDB_GetDBInfo( _this, db, records, datasize, timestamp )                                                                      ((_this)->vtbl->GetDBInfo( (_this), db, records, datasize, timestamp ))
	#define CALL_ReportDB_SetDBParams( _this, db, maxDataSize, maxPeriod, maxSegmentSize, maxSegmentPeriod )                                        ((_this)->vtbl->SetDBParams( (_this), db, maxDataSize, maxPeriod, maxSegmentSize, maxSegmentPeriod ))
	#define CALL_ReportDB_InitDB( _this, db, nEventSize )                                                                                           ((_this)->vtbl->InitDB( (_this), db, nEventSize ))
	#define CALL_ReportDB_ClearDB( _this, db )                                                                                                      ((_this)->vtbl->ClearDB( (_this), db ))
	#define CALL_ReportDB_FindObjectEx( _this, db, objtype, objid_data, objid_data_size, bCreate, flags, nParentObjectId, pnObjectId )              ((_this)->vtbl->FindObjectEx( (_this), db, objtype, objid_data, objid_data_size, bCreate, flags, nParentObjectId, pnObjectId ))
#else // if !defined( __cplusplus )
	#define CALL_ReportDB_AddEvent( _this, db, event, size, nEventId )                                                                              ((_this)->AddEvent( db, event, size, nEventId ))
	#define CALL_ReportDB_GetEvents( _this, db, from, events, buffsize, count, readed, pInfo)                                                       ((_this)->GetEvents( db, from, events, buffsize, count, readed, pInfo))
	#define CALL_ReportDB_UpdateEvent( _this, nEventId, event, size )                                                                               ((_this)->UpdateEvent( nEventId, event, size ))
	#define CALL_ReportDB_FindObject( _this, db, objtype, objid_data, objid_data_size, bCreate, flags, pnObjectId )                                 ((_this)->FindObject( db, objtype, objid_data, objid_data_size, bCreate, flags, pnObjectId ))
	#define CALL_ReportDB_GetObjectName( _this, nObjectId, name_buff, name_size, name_outsize, flags, pnObjectType )                                ((_this)->GetObjectName( nObjectId, name_buff, name_size, name_outsize, flags, pnObjectType ))
	#define CALL_ReportDB_GetObjectData( _this, nObjectId, data_buff, data_size, data_outsize, nDataType )                                          ((_this)->GetObjectData( nObjectId, data_buff, data_size, data_outsize, nDataType ))
	#define CALL_ReportDB_SetObjectData( _this, nObjectId, data_buff, data_size, nDataType )                                                        ((_this)->SetObjectData( nObjectId, data_buff, data_size, nDataType ))
	#define CALL_ReportDB_GetObjectInfo( _this, nObjectId, pInfo, nInfoType )                                                                       ((_this)->GetObjectInfo( nObjectId, pInfo, nInfoType ))
	#define CALL_ReportDB_SetObjectInfo( _this, nObjectId, pInfo, nInfoType )                                                                       ((_this)->SetObjectInfo( nObjectId, pInfo, nInfoType ))
	#define CALL_ReportDB_GetDBInfo( _this, db, records, datasize, timestamp )                                                                      ((_this)->GetDBInfo( db, records, datasize, timestamp ))
	#define CALL_ReportDB_SetDBParams( _this, db, maxDataSize, maxPeriod, maxSegmentSize, maxSegmentPeriod )                                        ((_this)->SetDBParams( db, maxDataSize, maxPeriod, maxSegmentSize, maxSegmentPeriod ))
	#define CALL_ReportDB_InitDB( _this, db, nEventSize )                                                                                           ((_this)->InitDB( db, nEventSize ))
	#define CALL_ReportDB_ClearDB( _this, db )                                                                                                      ((_this)->ClearDB( db ))
	#define CALL_ReportDB_FindObjectEx( _this, db, objtype, objid_data, objid_data_size, bCreate, flags, nParentObjectId, pnObjectId )              ((_this)->FindObjectEx( db, objtype, objid_data, objid_data_size, bCreate, flags, nParentObjectId, pnObjectId ))
#endif // if !defined( __cplusplus )
// AVP Prague stamp end



// AVP Prague stamp begin( C++ interface declaration,  )
#if defined (__cplusplus) && !defined(_USE_VTBL)
	struct pr_abstract iReportDB {
		virtual tERROR pr_call AddEvent( tDWORD db, const tPTR event, tDWORD size, tQWORD* nEventId ) = 0;
		virtual tERROR pr_call GetEvents( tDWORD db, int64 from, tPTR events, tDWORD buffsize, tDWORD count, tDWORD* readed, cSerializable* pInfo = NULL ) = 0;
		virtual tERROR pr_call UpdateEvent( tQWORD nEventId, const tPTR event, tDWORD size ) = 0;
		virtual tERROR pr_call FindObject( tDWORD db, tDWORD objtype, const tPTR objid_data, tDWORD objid_data_size, tBOOL bCreate, tDWORD flags, tObjectId* pnObjectId ) = 0;
		virtual tERROR pr_call GetObjectName( tObjectId nObjectId, tPTR name_buff, tDWORD name_size, tDWORD* name_outsize, tDWORD flags, tDWORD* pnObjectType ) = 0;
		virtual tERROR pr_call GetObjectData( tObjectId nObjectId, tPTR data_buff, tDWORD data_size, tDWORD* data_outsize, tDWORD nDataType ) = 0;
		virtual tERROR pr_call SetObjectData( tObjectId nObjectId, const tPTR data_buff, tDWORD data_size, tDWORD nDataType ) = 0;
		virtual tERROR pr_call GetObjectInfo( tObjectId nObjectId, cSerializable* pInfo, tDWORD nInfoType ) = 0;
		virtual tERROR pr_call SetObjectInfo( tObjectId nObjectId, cSerializable* pInfo, tDWORD nInfoType ) = 0;
		virtual tERROR pr_call GetDBInfo( tDWORD db, tDWORD* records, tQWORD* datasize, tQWORD* timestamp ) = 0;
		virtual tERROR pr_call SetDBParams( tDWORD db, tDWORD maxDataSize, tDWORD maxPeriod, tDWORD maxSegmentSize, tDWORD maxSegmentPeriod ) = 0;
		virtual tERROR pr_call InitDB( tDWORD db, tDWORD nEventSize ) = 0;
		virtual tERROR pr_call ClearDB( tDWORD db ) = 0;
		virtual tERROR pr_call FindObjectEx( tDWORD db, tDWORD objtype, const tPTR objid_data, tDWORD objid_data_size, tBOOL bCreate, tDWORD flags, tObjectId nParentObjectId, tObjectId* pnObjectId ) = 0;
	};

	struct pr_abstract cReportDB : public iReportDB, public iObj {

		OBJ_IMPL( cReportDB );

		operator hOBJECT() { return (hOBJECT)this; }
		operator hREPORTDB()   { return (hREPORTDB)this; }

	};

#endif // if defined (__cplusplus) && !defined(_USE_VTBL)
// AVP Prague stamp end



// AVP Prague stamp begin( Header endif,  )
#endif // __i_reportdb__33d2cdbc_ada0_438c_b730_6114873bed26
// AVP Prague stamp end



#if defined(__PROXY_DECLARATION) && !defined(ReportDB_PROXY_DEFINITION)
#define ReportDB_PROXY_DEFINITION

PR_PROXY_BEGIN(ReportDB)
	PR_PROXY(ReportDB, AddEvent,           PR_ARG3(tid_DWORD,tid_BINARY|prf_IN,tid_QWORD|tid_POINTER))
	PR_PROXY(ReportDB, GetEvents,          PR_ARG6(tid_DWORD,tid_QWORD,tid_BINARY,tid_DWORD,tid_DWORD|tid_POINTER,tid_SERIALIZABLE))
	PR_PROXY(ReportDB, UpdateEvent,        PR_ARG2(tid_QWORD,tid_BINARY|prf_IN))
	PR_PROXY(ReportDB, FindObject,         PR_ARG6(tid_DWORD,tid_DWORD,tid_BINARY|prf_IN,tid_BOOL,tid_DWORD,tid_QWORD|tid_POINTER))
	PR_PROXY(ReportDB, GetObjectName,      PR_ARG5(tid_QWORD,tid_BINARY|prf_OUT,tid_DWORD|tid_POINTER,tid_DWORD,tid_DWORD|tid_POINTER))
	PR_PROXY(ReportDB, GetObjectData,      PR_ARG4(tid_QWORD,tid_BINARY|prf_OUT,tid_DWORD|tid_POINTER,tid_DWORD))
	PR_PROXY(ReportDB, SetObjectData,      PR_ARG3(tid_QWORD,tid_BINARY|prf_IN,tid_DWORD))
	PR_PROXY(ReportDB, GetObjectInfo,      PR_ARG3(tid_QWORD,tid_SERIALIZABLE,tid_DWORD))
	PR_PROXY(ReportDB, SetObjectInfo,      PR_ARG3(tid_QWORD,tid_SERIALIZABLE,tid_DWORD))
	PR_PROXY(ReportDB, GetDBInfo,          PR_ARG4(tid_DWORD,tid_DWORD|tid_POINTER,tid_QWORD|tid_POINTER,tid_QWORD|tid_POINTER))
	PR_PROXY(ReportDB, SetDBParams,        PR_ARG5(tid_DWORD,tid_DWORD,tid_DWORD,tid_DWORD,tid_DWORD))
	PR_PROXY(ReportDB, InitDB,             PR_ARG2(tid_DWORD,tid_DWORD))
	PR_PROXY(ReportDB, ClearDB,            PR_ARG1(tid_DWORD))
	PR_PROXY(ReportDB, FindObjectEx,       PR_ARG7(tid_DWORD,tid_DWORD,tid_BINARY|prf_IN,tid_BOOL,tid_DWORD,tid_QWORD,tid_QWORD|tid_POINTER))
PR_PROXY_END(ReportDB, IID_REPORTDB)

#endif // __PROXY_DECLARATION
