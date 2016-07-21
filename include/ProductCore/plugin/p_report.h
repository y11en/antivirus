// AVP Prague stamp begin( Interface header,  )
// -------- ANSI C++ Code Generator 1.0 --------
// -------- Friday,  18 August 2006,  14:21 --------
// -------------------------------------------
// Copyright © Kaspersky Lab 1996-2005.
// -------------------------------------------
// Project     -- PPP
// Sub project -- Not defined
// Purpose     -- Report Storage
// Author      -- Pavlushchik
// File Name   -- plugin_report.cpp
// -------------------------------------------
// AVP Prague stamp end



// AVP Prague stamp begin( Header ifndef,  )
#if !defined( __public_plugin_report )
#define __public_plugin_report
// AVP Prague stamp end



// AVP Prague stamp begin( Header includes,  )
#include <Prague/prague.h>
#include <ProductCore/iface/i_report.h>
// AVP Prague stamp end



// AVP Prague stamp begin( Plugin identifier,  )
#define PID_REPORT  ((tPID)(58034))
// AVP Prague stamp end



// AVP Prague stamp begin( Vendor identifier,  )
#define VID_MIKE  ((tVID)(70))
// AVP Prague stamp end



// AVP Prague stamp begin( Plugin public definitions, Report )
// Report interface implementation
// Short comments -- //eng:Report Storage

// constants
#define cOPTIONAL_HEADER               ((tDWORD)(-1)) //  --
#define cOPTIONAL_FOOTER               ((tDWORD)(-2)) //  --

// properties
#define plIO                     mPROPERTY_MAKE_LOCAL( pTYPE_OBJECT  , 0x00002000 )
#define plFORCE_FLUSH            mPROPERTY_MAKE_LOCAL( pTYPE_BOOL    , 0x00002001 )
#define plSYNCHRONIZED           mPROPERTY_MAKE_LOCAL( pTYPE_BOOL    , 0x00002002 )
#define plOPTIONAL_HEADER_DATA   mPROPERTY_MAKE_LOCAL( pTYPE_BINARY  , 0x00002003 )
#define plOPTIONAL_FOOTER_DATA   mPROPERTY_MAKE_LOCAL( pTYPE_BINARY  , 0x00002004 )
#define plOBJECT_DELETE_ON_CLOSE mPROPERTY_MAKE_LOCAL( pTYPE_BOOL    , 0x00002005 )
#define plSHARE_WRITE            mPROPERTY_MAKE_LOCAL( pTYPE_DWORD   , 0x00002006 )
#define plLAST_RECORD_ADD        mPROPERTY_MAKE_LOCAL( pTYPE_DATETIME, 0x00002007 )
#define plLAST_RECORD_UPDATE     mPROPERTY_MAKE_LOCAL( pTYPE_DATETIME, 0x00002008 )
#define plLAST_RECORD_DELETE     mPROPERTY_MAKE_LOCAL( pTYPE_DATETIME, 0x00002009 )
#define plLAST_MODIFIED          mPROPERTY_MAKE_LOCAL( pTYPE_DATETIME, 0x0000200a )
#define plDELAYED_IO_CREATE      mPROPERTY_MAKE_LOCAL( pTYPE_BOOL    , 0x0000200b )
#define plTASK_MANAGER           mPROPERTY_MAKE_LOCAL( pTYPE_OBJECT  , 0x0000200c )

// message class
#define pmc_REPORT 0xb603a4ba // Класс сообщений, обрабатываемых отчетом

	#define pm_ADD_RECORD 0xb879ec6a // (3094998122) --
	// context comment
	//    Не используется
	// data content comment
	//    Указатель на даные, которые должны быть добавлены в отчет. Обработчик вызывает AddRecord для помещения данных в отчет.

	#define pm_BEFORE_DELETE_RECORD 0xb879ec6d // (3094998125) --
	// data content comment
	//    Содержит tDWORD индекса записи, которая будет удалена.

	#define pm_INTERNAL_NOTIFY_REPORT_CREATED 0xb879ed6b // (3094998379) -- internal syncronization message

// plugin interface
typedef   tERROR (pr_call *fnpReportReport_GetRecordEx)( hREPORT _this, tDWORD* result, tQWORD qwRecordId, tVOID* pBuffer, tDWORD dwSize, tDATETIME* pdtTimestamp, tDWORD* pdwIndex ); // -- ;
typedef   tERROR (pr_call *fnpReportReport_AddRecordEx)( hREPORT _this, const tVOID* pBuffer, tDWORD dwSize, tDWORD* pdwRecordIndex, tQWORD* pqwRecordId ); // -- ;
typedef   tERROR (pr_call *fnpReportReport_UpdateRecordEx)( hREPORT _this, tQWORD qwRecordId, const tVOID* pBuffer, tDWORD dwSize ); // -- ;
typedef   tERROR (pr_call *fnpReportReport_DeleteRecordEx)( hREPORT _this, tQWORD qwRecordId ); // -- ;

typedef struct tag_iReportReportVtbl {
	struct iReportVtbl prt;
	fnpReportReport_GetRecordEx     GetRecordEx;
	fnpReportReport_AddRecordEx     AddRecordEx;
	fnpReportReport_UpdateRecordEx  UpdateRecordEx;
	fnpReportReport_DeleteRecordEx  DeleteRecordEx;
} iReportReportVtbl;

typedef struct tag_ReportReport {
	const iReportReportVtbl* vtbl; // pointer to the "ReportReport" virtual table
	const iSYSTEMVtbl* sys;  // pointer to the "SYSTEM" virtual table
} *hReportReport;

#define CALL_ReportReport_GetRecordEx( _this, result, qwRecordId, pBuffer, dwSize, pdtTimestamp, pdwIndex )            ((_this)->vtbl->GetRecordEx( (_this), result, qwRecordId, pBuffer, dwSize, pdtTimestamp, pdwIndex ))
#define CALL_ReportReport_AddRecordEx( _this, pBuffer, dwSize, pdwRecordIndex, pqwRecordId )                           ((_this)->vtbl->AddRecordEx( (_this), pBuffer, dwSize, pdwRecordIndex, pqwRecordId ))
#define CALL_ReportReport_UpdateRecordEx( _this, qwRecordId, pBuffer, dwSize )                                         ((_this)->vtbl->UpdateRecordEx( (_this), qwRecordId, pBuffer, dwSize ))
#define CALL_ReportReport_DeleteRecordEx( _this, qwRecordId )                                                          ((_this)->vtbl->DeleteRecordEx( (_this), qwRecordId ))

#if defined (__cplusplus)
struct cReportReport : public cReport {
	virtual tERROR pr_call GetRecordEx( tDWORD* result, tQWORD qwRecordId, tVOID* pBuffer, tDWORD dwSize, tDATETIME* pdtTimestamp, tDWORD* pdwIndex ) = 0;
	virtual tERROR pr_call AddRecordEx( const tVOID* pBuffer, tDWORD dwSize, tDWORD* pdwRecordIndex, tQWORD* pqwRecordId ) = 0;
	virtual tERROR pr_call UpdateRecordEx( tQWORD qwRecordId, const tVOID* pBuffer, tDWORD dwSize ) = 0;
	virtual tERROR pr_call DeleteRecordEx( tQWORD qwRecordId ) = 0;

	operator hOBJECT() { return (hOBJECT)this; }
	operator hReportReport()   { return (hReportReport)this; }

	hOBJECT pr_call get_plIO() { return (hOBJECT)getObj(plIO); }
	tERROR pr_call set_plIO( hOBJECT value ) { return setObj(plIO,(hOBJECT)value); }

	tBOOL pr_call get_plFORCE_FLUSH() { return (tBOOL)getBool(plFORCE_FLUSH); }
	tERROR pr_call set_plFORCE_FLUSH( tBOOL value ) { return setBool(plFORCE_FLUSH,(tBOOL)value); }

	tBOOL pr_call get_plSYNCHRONIZED() { return (tBOOL)getBool(plSYNCHRONIZED); }
	tERROR pr_call set_plSYNCHRONIZED( tBOOL value ) { return setBool(plSYNCHRONIZED,(tBOOL)value); }

	tERROR pr_call get_plOPTIONAL_HEADER_DATA( tPTR value, tDWORD size ) { return get(plOPTIONAL_HEADER_DATA,value,size); }
	tERROR pr_call set_plOPTIONAL_HEADER_DATA( tPTR value, tDWORD size ) { return set(plOPTIONAL_HEADER_DATA,value,size); }

	tERROR pr_call get_plOPTIONAL_FOOTER_DATA( tPTR value, tDWORD size ) { return get(plOPTIONAL_FOOTER_DATA,value,size); }
	tERROR pr_call set_plOPTIONAL_FOOTER_DATA( tPTR value, tDWORD size ) { return set(plOPTIONAL_FOOTER_DATA,value,size); }

	tBOOL pr_call get_plOBJECT_DELETE_ON_CLOSE() { return (tBOOL)getBool(plOBJECT_DELETE_ON_CLOSE); }
	tERROR pr_call set_plOBJECT_DELETE_ON_CLOSE( tBOOL value ) { return setBool(plOBJECT_DELETE_ON_CLOSE,(tBOOL)value); }

	tDWORD pr_call get_plSHARE_WRITE() { return (tDWORD)getDWord(plSHARE_WRITE); }
	tERROR pr_call set_plSHARE_WRITE( tDWORD value ) { return setDWord(plSHARE_WRITE,(tDWORD)value); }

	tERROR pr_call get_plLAST_RECORD_ADD( tDATETIME* value ) { return get(plLAST_RECORD_ADD,value,sizeof(tDATETIME)); }
	tERROR pr_call set_plLAST_RECORD_ADD( tDATETIME* value ) { return set(plLAST_RECORD_ADD,value,sizeof(tDATETIME)); }

	tERROR pr_call get_plLAST_RECORD_UPDATE( tDATETIME* value ) { return get(plLAST_RECORD_UPDATE,value,sizeof(tDATETIME)); }
	tERROR pr_call set_plLAST_RECORD_UPDATE( tDATETIME* value ) { return set(plLAST_RECORD_UPDATE,value,sizeof(tDATETIME)); }

	tERROR pr_call get_plLAST_RECORD_DELETE( tDATETIME* value ) { return get(plLAST_RECORD_DELETE,value,sizeof(tDATETIME)); }
	tERROR pr_call set_plLAST_RECORD_DELETE( tDATETIME* value ) { return set(plLAST_RECORD_DELETE,value,sizeof(tDATETIME)); }

	tERROR pr_call get_plLAST_MODIFIED( tDATETIME* value ) { return get(plLAST_MODIFIED,value,sizeof(tDATETIME)); }
	tERROR pr_call set_plLAST_MODIFIED( tDATETIME* value ) { return set(plLAST_MODIFIED,value,sizeof(tDATETIME)); }

	tBOOL pr_call get_plDELAYED_IO_CREATE() { return (tBOOL)getBool(plDELAYED_IO_CREATE); }
	tERROR pr_call set_plDELAYED_IO_CREATE( tBOOL value ) { return setBool(plDELAYED_IO_CREATE,(tBOOL)value); }

	hOBJECT pr_call get_plTASK_MANAGER() { return (hOBJECT)getObj(plTASK_MANAGER); }
	tERROR pr_call set_plTASK_MANAGER( hOBJECT value ) { return setObj(plTASK_MANAGER,(hOBJECT)value); }

};

#endif // if defined (__cplusplus)
// AVP Prague stamp end



// AVP Prague stamp begin( Header endif,  )
#endif // __public_plugin_report
// AVP Prague stamp end



