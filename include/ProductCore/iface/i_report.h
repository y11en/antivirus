// AVP Prague stamp begin( Interface header,  )
// -------- Monday,  14 June 2004,  14:30 --------
// File Name   -- (null)i_report.cpp
// Copyright © Kaspersky Lab 1996-2004.
// -------------------------------------------
// AVP Prague stamp end



// AVP Prague stamp begin( Header ifndef,  )
#if !defined( __i_report__5ab454be_627b_48ac_9dfe_7ff56120048a )
#define __i_report__5ab454be_627b_48ac_9dfe_7ff56120048a
// AVP Prague stamp end



// AVP Prague stamp begin( Header includes,  )
#include <Prague/iface/i_io.h>
// AVP Prague stamp end



// AVP Prague stamp begin( Interface comment,  )
// Report interface implementation
// Short comments -- //eng:Report Storage
//    //eng: Report Storage interface implements methods to add, update, delete and retrive data on record by record basis in storage.
// AVP Prague stamp end



// AVP Prague stamp begin( Interface menonic identificator,  )
#define IID_REPORT  ((tIID)(31))
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
	typedef tUINT hREPORT;
#else
// AVP Prague stamp end



// AVP Prague stamp begin( Interface forward declaration,  )
struct iReportVtbl;
typedef struct iReportVtbl iReportVtbl;
// AVP Prague stamp end



// AVP Prague stamp begin( Object declaration,  )
#if defined( __cplusplus ) && !defined(_USE_VTBL)
	struct cReport;
	typedef cReport* hREPORT;
#else
	typedef struct tag_hREPORT {
		const iReportVtbl* vtbl; // pointer to the "Report" virtual table
		const iSYSTEMVtbl* sys;  // pointer to the "SYSTEM" virtual table
	} *hREPORT;
#endif // if defined( __cplusplus )
// AVP Prague stamp end



// AVP Prague stamp begin( RPC_ end declaration,  )
#endif // if defined( __RPC__ )
// AVP Prague stamp end



// AVP Prague stamp begin( Private definitions begin,  )
#if !defined( Report_PRIVATE_DEFINITION )
// AVP Prague stamp end



// AVP Prague stamp begin( Interface function forward declarations,  )


	typedef   tERROR (pr_call *fnpReport_GetRecordCount)   ( hREPORT _this, tDWORD* pdwRecords ); // -- //eng:Get count of records in report;
	typedef   tERROR (pr_call *fnpReport_GetRecord)        ( hREPORT _this, tDWORD* result, tDWORD dwIndex, tVOID* pBuffer, tDWORD dwSize, tDATETIME* pdtTimestamp ); // -- //eng:Retrive record;
	typedef   tERROR (pr_call *fnpReport_AddRecord)        ( hREPORT _this, const tVOID* pBuffer, tDWORD dwSize, tDWORD* pdwRecordIndex ); // -- //eng:Add record;
	typedef   tERROR (pr_call *fnpReport_UpdateRecord)     ( hREPORT _this, tDWORD dwIndex, const tVOID* pBuffer, tDWORD dwSize ); // -- //eng:Update record;
	typedef   tERROR (pr_call *fnpReport_DeleteRecord)     ( hREPORT _this, tDWORD dwIndex ); // -- //eng:Delete record;
	typedef   tERROR (pr_call *fnpReport_Compact)          ( hREPORT _this );                 // -- //eng:Compact report;
	typedef   tERROR (pr_call *fnpReport_DeleteOldRecords) ( hREPORT _this, const tDATETIME* pdtTime ); // -- //eng: Deletes old records from report;


// AVP Prague stamp end



// AVP Prague stamp begin( Interface declaration,  )
struct iReportVtbl {
	fnpReport_GetRecordCount    GetRecordCount;
	fnpReport_GetRecord         GetRecord;
	fnpReport_AddRecord         AddRecord;
	fnpReport_UpdateRecord      UpdateRecord;
	fnpReport_DeleteRecord      DeleteRecord;
	fnpReport_Compact           Compact;
	fnpReport_DeleteOldRecords  DeleteOldRecords;
}; // "Report" external virtual table prototype
// AVP Prague stamp end



// AVP Prague stamp begin( Private definitions end,  )
#endif // if !defined( Report_PRIVATE_DEFINITION )
// AVP Prague stamp end



// AVP Prague stamp begin( Interface declared properties,  )
//! #define pgINTERFACE_VERSION  mPROPERTY_MAKE_GLOBAL( pTYPE_VERSION , 0x00000082 )
//! #define pgINTERFACE_COMMENT  mPROPERTY_MAKE_GLOBAL( pTYPE_STRING  , 0x00000083 )
//! #define pgOBJECT_ACCESS_MODE mPROPERTY_MAKE_GLOBAL( pTYPE_DWORD   , 0x0000004e )
// AVP Prague stamp end



// AVP Prague stamp begin( Defines to simplify interface method calls,  )
#if !defined( __cplusplus ) || defined(_USE_VTBL)
	#define CALL_Report_GetRecordCount( _this, pdwRecords )                                           ((_this)->vtbl->GetRecordCount( (_this), pdwRecords ))
	#define CALL_Report_GetRecord( _this, result, dwIndex, pBuffer, dwSize, pdtTimestamp )            ((_this)->vtbl->GetRecord( (_this), result, dwIndex, pBuffer, dwSize, pdtTimestamp ))
	#define CALL_Report_AddRecord( _this, pBuffer, dwSize, pdwRecordIndex )                           ((_this)->vtbl->AddRecord( (_this), pBuffer, dwSize, pdwRecordIndex ))
	#define CALL_Report_UpdateRecord( _this, dwIndex, pBuffer, dwSize )                               ((_this)->vtbl->UpdateRecord( (_this), dwIndex, pBuffer, dwSize ))
	#define CALL_Report_DeleteRecord( _this, dwIndex )                                                ((_this)->vtbl->DeleteRecord( (_this), dwIndex ))
	#define CALL_Report_Compact( _this )                                                              ((_this)->vtbl->Compact( (_this) ))
	#define CALL_Report_DeleteOldRecords( _this, pdtTime )                                            ((_this)->vtbl->DeleteOldRecords( (_this), pdtTime ))
#else // if !defined( __cplusplus )
	#define CALL_Report_GetRecordCount( _this, pdwRecords )                                           ((_this)->GetRecordCount( pdwRecords ))
	#define CALL_Report_GetRecord( _this, result, dwIndex, pBuffer, dwSize, pdtTimestamp )            ((_this)->GetRecord( result, dwIndex, pBuffer, dwSize, pdtTimestamp ))
	#define CALL_Report_AddRecord( _this, pBuffer, dwSize, pdwRecordIndex )                           ((_this)->AddRecord( pBuffer, dwSize, pdwRecordIndex ))
	#define CALL_Report_UpdateRecord( _this, dwIndex, pBuffer, dwSize )                               ((_this)->UpdateRecord( dwIndex, pBuffer, dwSize ))
	#define CALL_Report_DeleteRecord( _this, dwIndex )                                                ((_this)->DeleteRecord( dwIndex ))
	#define CALL_Report_Compact( _this )                                                              ((_this)->Compact( ))
	#define CALL_Report_DeleteOldRecords( _this, pdtTime )                                            ((_this)->DeleteOldRecords( pdtTime ))
#endif // if !defined( __cplusplus )
// AVP Prague stamp end



// AVP Prague stamp begin( C++ interface declaration,  )
#if defined (__cplusplus) && !defined(_USE_VTBL)
	struct pr_abstract iReport {
		virtual tERROR pr_call GetRecordCount( tDWORD* pdwRecords ) = 0; // -- //eng:Get count of records in report
		virtual tERROR pr_call GetRecord( tDWORD* result, tDWORD dwIndex, tVOID* pBuffer, tDWORD dwSize, tDATETIME* pdtTimestamp ) = 0; // -- //eng:Retrive record
		virtual tERROR pr_call AddRecord( const tVOID* pBuffer, tDWORD dwSize, tDWORD* pdwRecordIndex ) = 0; // -- //eng:Add record
		virtual tERROR pr_call UpdateRecord( tDWORD dwIndex, const tVOID* pBuffer, tDWORD dwSize ) = 0; // -- //eng:Update record
		virtual tERROR pr_call DeleteRecord( tDWORD dwIndex ) = 0; // -- //eng:Delete record
		virtual tERROR pr_call Compact() = 0; // -- //eng:Compact report
		virtual tERROR pr_call DeleteOldRecords( const tDATETIME* pdtTime ) = 0; // -- //eng: Deletes old records from report
	};

	struct pr_abstract cReport : public iReport, public iObj {
		OBJ_IMPL( cReport );

		operator hOBJECT() { return (hOBJECT)this; }
		operator hREPORT()   { return (hREPORT)this; }

		tDWORD pr_call get_pgOBJECT_ACCESS_MODE() { return (tDWORD)getDWord(pgOBJECT_ACCESS_MODE); }
		tERROR pr_call set_pgOBJECT_ACCESS_MODE( tDWORD value ) { return setDWord(pgOBJECT_ACCESS_MODE,(tDWORD)value); }

	};

#endif // if defined (__cplusplus) && !defined(_USE_VTBL)
// AVP Prague stamp end



// AVP Prague stamp begin( Header endif,  )
#endif // __i_report__5ab454be_627b_48ac_9dfe_7ff56120048a
// AVP Prague stamp end



#if defined(__PROXY_DECLARATION) && !defined(Report_PROXY_DEFINITION)
#define Report_PROXY_DEFINITION

PR_PROXY_BEGIN(Report)
	PR_PROXY(Report, GetRecordCount,   PR_ARG1(tid_DWORD|tid_POINTER))
	PR_PROXY(Report, GetRecord,        PR_ARG4(tid_DWORD|tid_POINTER,tid_DWORD,tid_BINARY|prf_OUT,tid_DATETIME|tid_POINTER))
	PR_PROXY(Report, AddRecord,        PR_ARG2(tid_BINARY|prf_IN,tid_DWORD|tid_POINTER))
	PR_PROXY(Report, UpdateRecord,     PR_ARG2(tid_DWORD,tid_BINARY|prf_IN))
	PR_SCPRX(Report, DeleteRecord,     PR_ARG1(tid_DWORD))
	PR_PROXY(Report, Compact,          PR_ARG0())
PR_PROXY_END(Report, IID_REPORT)

#endif // __PROXY_DECLARATION
