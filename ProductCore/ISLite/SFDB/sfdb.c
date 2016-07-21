// AVP Prague stamp begin( Interface header,  )
// -------- ANSI C Code Generator 2.0 --------
// -------- Wednesday,  16 February 2005,  11:39 --------
// -------------------------------------------
// Copyright © Kaspersky Labs 1996-2004.
// -------------------------------------------
// Project     -- Integrated Solution
// Sub project -- Integrated Solution Lite
// Purpose     -- Secure Files Database Plugin
// Author      -- mike
// File Name   -- sfdb.c
// -------------------------------------------
// AVP Prague stamp end



// AVP Prague stamp begin( Private definitions,  )
#define SecureFileDatabase_PRIVATE_DEFINITION
// AVP Prague stamp end



// AVP Prague stamp begin( Includes for interface,  )
#include <Prague/prague.h>
#include <aclapi.h>

#include "sfdb.h"
// AVP Prague stamp end



#include <wincompat.h>

#include <ProductCore/plugin/p_sfdb.h>
#include <Prague/iface/i_csect.h>
#include <Prague/iface/i_mutex.h>
#include <Prague/iface/e_loader.h>
#include <Prague/iface/e_ktime.h>
#include "database.h"
#include "../../../prague/hash/hash_api/md5/hash_md5.h"
#include "sa.h"
#include "locals.h"


#define TEMP_FILE_PREFIX "SFDB"
#define CURRENT_DATETIME_GET_FREQUENCY	(1<<8)
#ifdef _DEBUG
#define _FILE_SHARE_MODE (FILE_SHARE_READ)
#else
#define _FILE_SHARE_MODE 0
#endif



//#define _USE_CRITICAL_SECTION_
//#define _USE_MUTEX_

#ifdef _DEBUG
#define _USE_MUTEX_TIMED_
#else 
#define _USE_MUTEX_
#endif

#define MUTEX_NAME		"SFDBv2"
#define MUTEX_TIMEOUT	500
#define MUTEX_LONG_TIMEOUT	30*1000

#if defined(_USE_CRITICAL_SECTION_)
#define READING_LOCK()						CALL_CriticalSection_Enter(ldata->hCriticalSection, SHARE_LEVEL_WRITE)
#define WRITING_LOCK()						CALL_CriticalSection_Enter(ldata->hCriticalSection, SHARE_LEVEL_WRITE)
#define WRITING_LOCK_EX(timeout)			CALL_CriticalSection_Enter(ldata->hCriticalSection, SHARE_LEVEL_WRITE)
#define RELEASE()							CALL_CriticalSection_Leave(ldata->hCriticalSection)

#elif defined(_USE_MUTEX_)
#define READING_LOCK()						CALL_Mutex_Lock(ldata->hMutex, MUTEX_TIMEOUT)
#define WRITING_LOCK()						CALL_Mutex_Lock(ldata->hMutex, MUTEX_TIMEOUT)
#define WRITING_LOCK_EX(timeout)					CALL_Mutex_Lock(ldata->hMutex, timeout)
#define RELEASE()							CALL_Mutex_Release(ldata->hMutex)

#elif defined(_USE_MUTEX_TIMED_)
#define READING_LOCK()						Mutex_Lock_Timed(_this, ldata->hMutex, MUTEX_TIMEOUT)
#define WRITING_LOCK()						Mutex_Lock_Timed(_this, ldata->hMutex, MUTEX_TIMEOUT)
#define WRITING_LOCK_EX(timeout)					Mutex_Lock_Timed(_this, ldata->hMutex, timeout)
#define RELEASE()							Mutex_Release_Timed(_this, ldata->hMutex)

#else
#pragma error ("Sync object undefined")
#endif



//#define _USE_TRY_
#if !defined(_DEBUG) || defined(_USE_TRY_)
#define __TRY __try
#define __TRY_F __try
#define __EXCEPT __except
#define __FINALLY __finally
#else
#define __TRY 
#define __TRY_F __try
#define __EXCEPT(x) if (0)
#define __FINALLY __finally
#undef GetExceptionCode
#define GetExceptionCode() 0
#endif

// AVP Prague stamp begin( Interface comment,  )
// SecureFileDatabase interface implementation
// Short comments -- Secure Files Database
//    Интерфейс предоставляет сервис по хранению данных в базе, ее быстрому поиску, обновлению и удалению. Каждая запись в база данных идентифицируется по уникальному идентификатору и имеет постоянный набор данных. Размеры  идентификатора и данных записи определяются при создании БД.
// AVP Prague stamp end




// AVP Prague stamp begin( Interface version,  )
#define SecureFileDatabase_VERSION ((tVERSION)1)
// AVP Prague stamp end



// AVP Prague stamp begin( Header includes,  )
#include <Prague/prague.h>
#include <Prague/iface/i_root.h>
#include <Prague/iface/i_csect.h>
#include <ProductCore/plugin/p_sfdb.h>
// AVP Prague stamp end



// AVP Prague stamp begin( Data structure,  )
// Interface SecureFileDatabase. Inner data structure

typedef struct tag__LOCALDATA {
	tDWORD dummy; // --
} _LOCALDATA;

// AVP Prague stamp end



#define _LOCALDATA LOCALDATA // trick - override local data struct declared in IMP with own struct LOCALDATA

// AVP Prague stamp begin( Object declaration,  )

typedef struct tag_hi_SecureFileDatabase {
	const iSecureFileDatabaseVtbl* vtbl; // pointer to the "SecureFileDatabase" virtual table
	const iSYSTEMVTBL*             sys;  // pointer to the "SYSTEM" virtual table
	_LOCALDATA*                    data; // pointer to the "SecureFileDatabase" data structure
} *hi_SecureFileDatabase;

// AVP Prague stamp end



// AVP Prague stamp begin( Internal method table prototypes,  )
tERROR pr_call SecureFileDatabase_ObjectInit( hi_SecureFileDatabase _this );
tERROR pr_call SecureFileDatabase_ObjectInitDone( hi_SecureFileDatabase _this );
tERROR pr_call SecureFileDatabase_ObjectPreClose( hi_SecureFileDatabase _this );
// AVP Prague stamp end



// AVP Prague stamp begin( Internal method table,  )
static iINTERNAL i_SecureFileDatabase_vtbl = {
	(tIntFnRecognize)         NULL,
	(tIntFnObjectNew)         NULL,
	(tIntFnObjectInit)        SecureFileDatabase_ObjectInit,
	(tIntFnObjectInitDone)    SecureFileDatabase_ObjectInitDone,
	(tIntFnObjectCheck)       NULL,
	(tIntFnObjectPreClose)    SecureFileDatabase_ObjectPreClose,
	(tIntFnObjectClose)       NULL,
	(tIntFnChildNew)          NULL,
	(tIntFnChildInitDone)     NULL,
	(tIntFnChildClose)        NULL,
	(tIntFnMsgReceive)        NULL,
	(tIntFnIFaceTransfer)     NULL
};
// AVP Prague stamp end



// AVP Prague stamp begin( Interface function forward declarations,  )
typedef   tERROR (pr_call *fnpSecureFileDatabase_InitNew)      ( hi_SecureFileDatabase _this, tDWORD dwRecordIDAlg, tDWORD dwRecordIDSize, tDWORD dwRecordDataSize, tDWORD dwDBInitialSize ); // -- Создает новую базу данных;
typedef   tERROR (pr_call *fnpSecureFileDatabase_Load)         ( hi_SecureFileDatabase _this, tCHAR* szFileName ); // -- Загружает базу данных из файла;
typedef   tERROR (pr_call *fnpSecureFileDatabase_Save)         ( hi_SecureFileDatabase _this, tCHAR* szFileName ); // -- Сохраняет базу данных в файл;
typedef   tERROR (pr_call *fnpSecureFileDatabase_GetRecord)    ( hi_SecureFileDatabase _this, tVOID* pRecordID, tVOID* pRecordData ); // -- Получает данные из записи;
typedef   tERROR (pr_call *fnpSecureFileDatabase_UpdateRecord) ( hi_SecureFileDatabase _this, tVOID* pRecordID, tVOID* pRecordData ); // -- Обновляет данные записи;
typedef   tERROR (pr_call *fnpSecureFileDatabase_DeleteRecord) ( hi_SecureFileDatabase _this, tVOID* pRecordID ); // -- Удаляет указанную запись вместе с ее данными;
typedef   tERROR (pr_call *fnpSecureFileDatabase_GetDBInfo)    ( hi_SecureFileDatabase _this, tDWORD* pRecordIDAlg, tDWORD* pRecordIDSize, tDWORD* pRecordDataSize ); // -- Получает информацию о конфигурации базы данных;
typedef   tERROR (pr_call *fnpSecureFileDatabase_CleanDB)      ( hi_SecureFileDatabase _this); // -- Очищает базу
typedef   tERROR (pr_call *fnpSecureFileDatabase_EnumRecords)  ( hi_SecureFileDatabase _this, tDWORD nRecordID, tVOID* pRecordID, tVOID* pRecordData ); // -- Получает запись по номеру
// AVP Prague stamp end



// AVP Prague stamp begin( Interface declaration,  )
struct iSecureFileDatabaseVtbl {
	fnpSecureFileDatabase_InitNew       InitNew;
	fnpSecureFileDatabase_Load          Load;
	fnpSecureFileDatabase_Save          Save;
	fnpSecureFileDatabase_GetRecord     GetRecord;
	fnpSecureFileDatabase_UpdateRecord  UpdateRecord;
	fnpSecureFileDatabase_DeleteRecord  DeleteRecord;
	fnpSecureFileDatabase_GetDBInfo     GetDBInfo;
	fnpSecureFileDatabase_CleanDB       CleanDB;
	fnpSecureFileDatabase_EnumRecords   EnumRecords;
}; // "SecureFileDatabase" external virtual table prototype
// AVP Prague stamp end



// AVP Prague stamp begin( External method table prototypes,  )
tERROR pr_call SecureFileDatabase_InitNew( hi_SecureFileDatabase _this, tDWORD p_dwRecordIDAlg, tDWORD p_dwRecordIDSize, tDWORD p_dwRecordDataSize, tDWORD p_dwDBInitialSize );
tERROR pr_call SecureFileDatabase_Load( hi_SecureFileDatabase _this, tCHAR* p_szFileName );
tERROR pr_call SecureFileDatabase_Save( hi_SecureFileDatabase _this, tCHAR* p_szFileName );
tERROR pr_call SecureFileDatabase_GetRecord( hi_SecureFileDatabase _this, tVOID* p_pRecordID, tVOID* p_pRecordData );
tERROR pr_call SecureFileDatabase_UpdateRecord( hi_SecureFileDatabase _this, tVOID* p_pRecordID, tVOID* p_pRecordData );
tERROR pr_call SecureFileDatabase_DeleteRecord( hi_SecureFileDatabase _this, tVOID* p_pRecordID );
tERROR pr_call SecureFileDatabase_GetDBInfo( hi_SecureFileDatabase _this, tDWORD* p_pRecordIDAlg, tDWORD* p_pRecordIDSize, tDWORD* p_pRecordDataSize );
tERROR pr_call SecureFileDatabase_CleanDB( hi_SecureFileDatabase _this );
tERROR pr_call SecureFileDatabase_EnumRecords( hi_SecureFileDatabase _this, tDWORD nRecordID, tVOID* p_pRecordID, tVOID* p_pRecordData );
// AVP Prague stamp end



// AVP Prague stamp begin( External method table,  )
static iSecureFileDatabaseVtbl e_SecureFileDatabase_vtbl = {
	(fnpSecureFileDatabase_InitNew) SecureFileDatabase_InitNew,
	(fnpSecureFileDatabase_Load) SecureFileDatabase_Load,
	(fnpSecureFileDatabase_Save) SecureFileDatabase_Save,
	(fnpSecureFileDatabase_GetRecord) SecureFileDatabase_GetRecord,
	(fnpSecureFileDatabase_UpdateRecord) SecureFileDatabase_UpdateRecord,
	(fnpSecureFileDatabase_DeleteRecord) SecureFileDatabase_DeleteRecord,
	(fnpSecureFileDatabase_GetDBInfo) SecureFileDatabase_GetDBInfo,
	(fnpSecureFileDatabase_CleanDB) SecureFileDatabase_CleanDB,
	(fnpSecureFileDatabase_EnumRecords) SecureFileDatabase_EnumRecords
};
// AVP Prague stamp end



// AVP Prague stamp begin( Forwarded property methods declarations,  )
tERROR pr_call SecureFileDatabase_PROP_GetSerializeOptions( hi_SecureFileDatabase _this, tDWORD* out_size, tPROPID prop, tCHAR* buffer, tDWORD size );
tERROR pr_call SecureFileDatabase_PROP_SetSerializeOptions( hi_SecureFileDatabase _this, tDWORD* out_size, tPROPID prop, tCHAR* buffer, tDWORD size );
tERROR pr_call SecureFileDatabase_PROP_GetUnusedRecordsLimitTime( hi_SecureFileDatabase _this, tDWORD* out_size, tPROPID prop, tCHAR* buffer, tDWORD size );
tERROR pr_call SecureFileDatabase_PROP_SetUnusedRecordsLimitTime( hi_SecureFileDatabase _this, tDWORD* out_size, tPROPID prop, tCHAR* buffer, tDWORD size );
tERROR pr_call SecureFileDatabase_PROP_GetDatabaseSizeLimit( hi_SecureFileDatabase _this, tDWORD* out_size, tPROPID prop, tCHAR* buffer, tDWORD size );
tERROR pr_call SecureFileDatabase_PROP_SetDatabaseSizeLimit( hi_SecureFileDatabase _this, tDWORD* out_size, tPROPID prop, tCHAR* buffer, tDWORD size );
tERROR pr_call SecureFileDatabase_PROP_GetSFDBFilePath( hi_SecureFileDatabase _this, tDWORD* out_size, tPROPID prop, tCHAR* buffer, tDWORD size );
tERROR pr_call SecureFileDatabase_PROP_SetSFDBFilePath( hi_SecureFileDatabase _this, tDWORD* out_size, tPROPID prop, tCHAR* buffer, tDWORD size );
// AVP Prague stamp end



// AVP Prague stamp begin( Global property variable declaration,  )
// Interface "SecureFileDatabase". Static(shared) property table variables
// AVP Prague stamp end



// AVP Prague stamp begin( Property table,  )
mPROPERTY_TABLE(SecureFileDatabase_PropTable)
	mSHARED_PROPERTY( pgINTERFACE_VERSION, SecureFileDatabase_VERSION )
	mSHARED_PROPERTY_PTR( pgINTERFACE_COMMENT, "Secure Files Database", 22 )
	mLOCAL_PROPERTY_FN( plSERIALIZE_OPTIONS, SecureFileDatabase_PROP_GetSerializeOptions, SecureFileDatabase_PROP_SetSerializeOptions )
	mLOCAL_PROPERTY_FN( plUNUSED_RECORDS_LIMIT_TIME, SecureFileDatabase_PROP_GetUnusedRecordsLimitTime, SecureFileDatabase_PROP_SetUnusedRecordsLimitTime )
	mLOCAL_PROPERTY_FN( plDATABASE_SIZE_LIMIT, SecureFileDatabase_PROP_GetDatabaseSizeLimit, SecureFileDatabase_PROP_SetDatabaseSizeLimit )
	mLOCAL_PROPERTY_FN( plSFDB_FILEPATH, SecureFileDatabase_PROP_GetSFDBFilePath, SecureFileDatabase_PROP_SetSFDBFilePath )
mPROPERTY_TABLE_END(SecureFileDatabase_PropTable)
// AVP Prague stamp end



BOOL CreateSA( hi_SecureFileDatabase _this, SECURITY_ATTRIBUTES** ppSA,  IN DWORD EveryoneAccessPermissions);
tERROR RemapViewOfFile(hi_SecureFileDatabase _this, tDWORD dwNewViewSize);


// AVP Prague stamp begin( Registration call,  )
// Interface "SecureFileDatabase". Register function
tERROR pr_call SecureFileDatabase_Register( hROOT root ) {
	tERROR error;
	
	
	PR_TRACE_A0( root, "Enter SecureFileDatabase::Register method" );
	
	error = CALL_Root_RegisterIFace(
		root,                                   // root object
		IID_SECUREFILEDB,                       // interface identifier
		PID_SFDB,                               // plugin identifier
		0x00000000,                             // subtype identifier
		SecureFileDatabase_VERSION,             // interface version
		VID_MIKE,                               // interface developer
		&i_SecureFileDatabase_vtbl,             // internal(kernel called) function table
		(sizeof(i_SecureFileDatabase_vtbl)/sizeof(tPTR)),// internal function table size
		&e_SecureFileDatabase_vtbl,             // external function table
		(sizeof(e_SecureFileDatabase_vtbl)/sizeof(tPTR)),// external function table size
		SecureFileDatabase_PropTable,           // property table
		mPROPERTY_TABLE_SIZE(SecureFileDatabase_PropTable),// property table size
		sizeof(_LOCALDATA),                     // memory size
		0                                       // interface flags
	);
	
#ifdef _PRAGUE_TRACE_
	if ( PR_FAIL(error) )
		PR_TRACE( (root,prtDANGER,"sfdb\t(IID_SECUREFILEDB) registered [error=0x%08x]",error) );
#endif // _PRAGUE_TRACE_
	
	
	PR_TRACE_A1( root, "Leave SecureFileDatabase::Register method, ret tERROR = 0x%08x", error );
	return error;
}
// AVP Prague stamp end



// AVP Prague stamp begin( Property method implementation, GetSerializeOptions )
// Interface "SecureFileDatabase". Method "Get" for property(s):
//  -- SERIALIZE_OPTIONS
tERROR pr_call SecureFileDatabase_PROP_GetSerializeOptions( hi_SecureFileDatabase _this, tDWORD* out_size, tPROPID prop, tCHAR* buffer, tDWORD size )
{
	tERROR error = errOK;
	LOCALDATA* ldata = (LOCALDATA*)(_this->data);
	
	PR_TRACE_A0( _this, "Enter *GET* method SecureFileDatabase_PROP_GetSerializeOptions for property plSERIALIZE_OPTIONS" );
	
	*out_size = sizeof(tDWORD);
	if (buffer != NULL){
		if (size<sizeof(tDWORD))
			return errBUFFER_TOO_SMALL;
		*(tDWORD*)buffer = ldata->dwSerializeOptions;
	}
	
	PR_TRACE_A2( _this, "Leave *GET* method SecureFileDatabase_PROP_GetSerializeOptions for property plSERIALIZE_OPTIONS, ret tDWORD = %u(size), error = 0x%08x", *out_size, error );
	return error;
}
// AVP Prague stamp end



// AVP Prague stamp begin( Property method implementation, SetSerializeOptions )
// Interface "SecureFileDatabase". Method "Set" for property(s):
//  -- SERIALIZE_OPTIONS
tERROR pr_call SecureFileDatabase_PROP_SetSerializeOptions( hi_SecureFileDatabase _this, tDWORD* out_size, tPROPID prop, tCHAR* buffer, tDWORD size )
{
	tERROR error = errOK;
	LOCALDATA* ldata = (LOCALDATA*)(_this->data);
	
	PR_TRACE_A0( _this, "Enter *SET* method SecureFileDatabase_PROP_SetSerializeOptions for property plSERIALIZE_OPTIONS" );
	
	*out_size = sizeof(tDWORD);
	if (buffer != NULL){
		if (size<sizeof(tDWORD))
			return errBUFFER_TOO_SMALL;
		ldata->dwSerializeOptions = *(tDWORD*)buffer;
	}
	
	PR_TRACE_A2( _this, "Leave *SET* method SecureFileDatabase_PROP_SetSerializeOptions for property plSERIALIZE_OPTIONS, ret tDWORD = %u(size), error = 0x%08x", *out_size, error );
	return error;
}
// AVP Prague stamp end



// AVP Prague stamp begin( Property method implementation, GetUnusedRecordsLimitTime )
// Interface "SecureFileDatabase". Method "Get" for property(s):
//  -- UNUSED_RECORDS_LIMIT_TIME
tERROR pr_call SecureFileDatabase_PROP_GetUnusedRecordsLimitTime( hi_SecureFileDatabase _this, tDWORD* out_size, tPROPID prop, tCHAR* buffer, tDWORD size )
{
	tERROR error = errOK;
	LOCALDATA* ldata = (LOCALDATA*)(_this->data);
	PR_TRACE_A0( _this, "Enter *GET* method SecureFileDatabase_PROP_GetUnusedRecordsLimitTime for property plUNUSED_RECORDS_LIMIT_TIME" );
	
	*out_size = sizeof(tDWORD);
	if (buffer != NULL){
		if (size<sizeof(tDWORD))
			return errBUFFER_TOO_SMALL;
		*(tDWORD*)buffer = ldata->dwUnusedRecordsLimitTime;
	}
	
	PR_TRACE_A2( _this, "Leave *GET* method SecureFileDatabase_PROP_GetUnusedRecordsLimitTime for property plUNUSED_RECORDS_LIMIT_TIME, ret tDWORD = %u(size), error = 0x%08x", *out_size, error );
	return error;
}
// AVP Prague stamp end



// AVP Prague stamp begin( Property method implementation, SetUnusedRecordsLimitTime )
// Interface "SecureFileDatabase". Method "Set" for property(s):
//  -- UNUSED_RECORDS_LIMIT_TIME
tERROR pr_call SecureFileDatabase_PROP_SetUnusedRecordsLimitTime( hi_SecureFileDatabase _this, tDWORD* out_size, tPROPID prop, tCHAR* buffer, tDWORD size )
{
	tERROR error = errOK;
	LOCALDATA* ldata = (LOCALDATA*)(_this->data);
	PR_TRACE_A0( _this, "Enter *SET* method SecureFileDatabase_PROP_SetUnusedRecordsLimitTime for property plUNUSED_RECORDS_LIMIT_TIME" );
	
	*out_size = sizeof(tDWORD);
	if (buffer != NULL){
		if (size<sizeof(tDWORD))
			return errBUFFER_TOO_SMALL;
		ldata->dwUnusedRecordsLimitTime = *(tDWORD*)buffer;
	}
	
	PR_TRACE_A2( _this, "Leave *SET* method SecureFileDatabase_PROP_SetUnusedRecordsLimitTime for property plUNUSED_RECORDS_LIMIT_TIME, ret tDWORD = %u(size), error = 0x%08x", *out_size, error );
	return error;
}
// AVP Prague stamp end



// AVP Prague stamp begin( Property method implementation, GetUnusedRecordsLimitPercent )
// Interface "SecureFileDatabase". Method "Get" for property(s):
//  -- UNUSED_RECORDS_LIMIT_PERCENT
tERROR pr_call SecureFileDatabase_PROP_GetUnusedRecordsLimitPercent( hi_SecureFileDatabase _this, tDWORD* out_size, tPROPID prop, tCHAR* buffer, tDWORD size )
{
	tERROR error = errOK;
	error = errNOT_IMPLEMENTED;
	//	LOCALDATA* ldata = (LOCALDATA*)(_this->data);
	//	PR_TRACE_A0( _this, "Enter *GET* method SecureFileDatabase_PROP_GetUnusedRecordsLimitPercent for property plUNUSED_RECORDS_LIMIT_PERCENT" );
	//
	//	*out_size = sizeof(tDWORD);
	//	if (buffer != NULL){
	//		if (size<sizeof(tDWORD))
	//			return errBUFFER_TOO_SMALL;
	//		*(tDWORD*)buffer = ldata->dwUnusedRecordsLimitPercent;
	//	}
	//
	//	PR_TRACE_A2( _this, "Leave *GET* method SecureFileDatabase_PROP_GetUnusedRecordsLimitPercent for property plUNUSED_RECORDS_LIMIT_PERCENT, ret tDWORD = %u(size), error = 0x%08x", *out_size, error );
	return error;
}
// AVP Prague stamp end



// AVP Prague stamp begin( Property method implementation, SetUnusedRecordsLimitPercent )
// Interface "SecureFileDatabase". Method "Set" for property(s):
//  -- UNUSED_RECORDS_LIMIT_PERCENT
tERROR pr_call SecureFileDatabase_PROP_SetUnusedRecordsLimitPercent( hi_SecureFileDatabase _this, tDWORD* out_size, tPROPID prop, tCHAR* buffer, tDWORD size )
{
	tERROR error = errOK;
	error = errNOT_IMPLEMENTED;
	//	LOCALDATA* ldata = (LOCALDATA*)(_this->data);
	//	PR_TRACE_A0( _this, "Enter *SET* method SecureFileDatabase_PROP_SetUnusedRecordsLimitPercent for property plUNUSED_RECORDS_LIMIT_PERCENT" );
	//
	//	*out_size = sizeof(tDWORD);
	//	if (buffer != NULL){
	//		if (size<sizeof(tDWORD))
	//			return errBUFFER_TOO_SMALL;
	//		ldata->dwUnusedRecordsLimitPercent = *(tDWORD*)buffer;
	//	}
	//
	//	PR_TRACE_A2( _this, "Leave *SET* method SecureFileDatabase_PROP_SetUnusedRecordsLimitPercent for property plUNUSED_RECORDS_LIMIT_PERCENT, ret tDWORD = %u(size), error = 0x%08x", *out_size, error );
	return error;
}
// AVP Prague stamp end



// AVP Prague stamp begin( Property method implementation, GetDatabaseSizeLimit )
// Interface "SecureFileDatabase". Method "Get" for property(s):
//  -- DATABASE_SIZE_LIMIT
tERROR pr_call SecureFileDatabase_PROP_GetDatabaseSizeLimit( hi_SecureFileDatabase _this, tDWORD* out_size, tPROPID prop, tCHAR* buffer, tDWORD size )
{
	tERROR error = errOK;
	LOCALDATA* ldata = (LOCALDATA*)(_this->data);
	PR_TRACE_A0( _this, "Enter *GET* method SecureFileDatabase_PROP_GetDatabaseSizeLimit for property plDATABASE_SIZE_LIMIT" );
	
	*out_size = sizeof(tDWORD);
	if (buffer != NULL){
		if (size<sizeof(tDWORD))
			return errBUFFER_TOO_SMALL;
		*(tDWORD*)buffer = ldata->dwDatabaseSizeLimit;
	}
	
	PR_TRACE_A2( _this, "Leave *GET* method SecureFileDatabase_PROP_GetDatabaseSizeLimit for property plDATABASE_SIZE_LIMIT, ret tDWORD = %u(size), error = 0x%08x", *out_size, error );
	return error;
}
// AVP Prague stamp end



// AVP Prague stamp begin( Property method implementation, SetDatabaseSizeLimit )
// Interface "SecureFileDatabase". Method "Set" for property(s):
//  -- DATABASE_SIZE_LIMIT
tERROR pr_call SecureFileDatabase_PROP_SetDatabaseSizeLimit( hi_SecureFileDatabase _this, tDWORD* out_size, tPROPID prop, tCHAR* buffer, tDWORD size )
{
	tERROR error = errOK;
	LOCALDATA* ldata = (LOCALDATA*)(_this->data);
	PR_TRACE_A0( _this, "Enter *SET* method SecureFileDatabase_PROP_SetDatabaseSizeLimit for property plDATABASE_SIZE_LIMIT" );
	
	*out_size = sizeof(tDWORD);
	if (buffer != NULL){
		if (size<sizeof(tDWORD))
			return errBUFFER_TOO_SMALL;
		ldata->dwDatabaseSizeLimit= *(tDWORD*)buffer;
	}
	
	PR_TRACE_A2( _this, "Leave *SET* method SecureFileDatabase_PROP_SetDatabaseSizeLimit for property plDATABASE_SIZE_LIMIT, ret tDWORD = %u(size), error = 0x%08x", *out_size, error );
	return error;
}
// AVP Prague stamp end



// AVP Prague stamp begin( Property method implementation, GetSFDBFilePath )
// Interface "SecureFileDatabase". Method "Get" for property(s):
//  -- SFDB_FILEPATH
tERROR pr_call SecureFileDatabase_PROP_GetSFDBFilePath( hi_SecureFileDatabase _this, tDWORD* out_size, tPROPID prop, tCHAR* buffer, tDWORD size )
{
	tERROR error = errOK;
	LOCALDATA* ldata = (LOCALDATA*)(_this->data);
	PR_TRACE_A0( _this, "Enter *GET* method SecureFileDatabase_PROP_GetSFDBFilePath for property plSFDB_FILEPATH" );
	
	*out_size = (tDWORD)(strlen(ldata->szSFDBPathName)) + 1;
	if (buffer != NULL){
		if (size<*out_size)
			return errBUFFER_TOO_SMALL;
		strcpy(buffer, ldata->szSFDBPathName);
	}
	
	PR_TRACE_A2( _this, "Leave *GET* method SecureFileDatabase_PROP_GetSFDBFilePath for property plSFDB_FILEPATH, ret tDWORD = %u(size), error = 0x%08x", *out_size, error );
	return error;
}
// AVP Prague stamp end



// AVP Prague stamp begin( Property method implementation, SetSFDBFilePath )
// Interface "SecureFileDatabase". Method "Set" for property(s):
//  -- SFDB_FILEPATH
tERROR pr_call SecureFileDatabase_PROP_SetSFDBFilePath( hi_SecureFileDatabase _this, tDWORD* out_size, tPROPID prop, tCHAR* buffer, tDWORD size )
{
	tERROR error = errOK;
	LOCALDATA* ldata = (LOCALDATA*)(_this->data);
	PR_TRACE_A0( _this, "Enter *SET* method SecureFileDatabase_PROP_SetSFDBFilePath for property plSFDB_FILEPATH" );
	
	*out_size = sizeof(ldata->szSFDBPathName);
	if (buffer != NULL){
		if (size>sizeof(ldata->szSFDBPathName)+1)
			return errBUFFER_TOO_SMALL;
		strcpy(ldata->szSFDBPathName, buffer);
		*out_size = (tDWORD)(strlen(ldata->szSFDBPathName)) + 1;
		
		if ((ldata->dwSerializeOptions & cSFDB_AUTOLOAD) && ldata->szSFDBPathName[0] && ldata->pDB)
			SecureFileDatabase_Load(_this, ldata->szSFDBPathName);
		
	}
	
	PR_TRACE_A2( _this, "Leave *SET* method SecureFileDatabase_PROP_SetSFDBFilePath for property plSFDB_FILEPATH, ret tDWORD = %u(size), error = 0x%08x", *out_size, error );
	return error;
}
// AVP Prague stamp end



__inline tERROR Mutex_Lock_Timed(hi_SecureFileDatabase _this, hMUTEX hMutex, tDWORD dwTimeout)
{
	tERROR error;
	LOCALDATA* ldata = (LOCALDATA*)(_this->data);
	
	LARGE_INTEGER liStart;
	LARGE_INTEGER liStop;
	
	QueryPerformanceCounter(&liStart);
	error = CALL_Mutex_Lock(hMutex, dwTimeout);
	QueryPerformanceCounter(&liStop);
	*(unsigned __int64*)&(ldata->liSyncOverhit) += *(unsigned __int64*)&liStop - *(unsigned __int64*)&liStart;
	ldata->dwSyncCalls++;
	
	return error;
}

__inline tERROR Mutex_Release_Timed(hi_SecureFileDatabase _this, hMUTEX hMutex)
{
	tERROR error;
	LOCALDATA* ldata = (LOCALDATA*)(_this->data);
	
	LARGE_INTEGER liStart;
	LARGE_INTEGER liStop;
	
	QueryPerformanceCounter(&liStart);
	error = CALL_Mutex_Release(hMutex);
	QueryPerformanceCounter(&liStop);
	*(unsigned __int64*)&(ldata->liSyncOverhit) += *(unsigned __int64*)&liStop - *(unsigned __int64*)&liStart;
	ldata->dwSyncCalls++;
	
	return error;
}

__inline tERROR GetCurrentDateTime(hi_SecureFileDatabase _this, tDWORD* pdwDate)
{
	tERROR error = errOK;
	LOCALDATA* ldata = (LOCALDATA*)(_this->data);
	
	if ((ldata->dwCurrentDateTimeGetCounter++ % CURRENT_DATETIME_GET_FREQUENCY) == 0)
	{
		tDATETIME dt;
		tQWORD qwDays;
		if (PR_FAIL(error = Now(&dt)))
			PR_TRACE((_this, prtERROR, "sfdb\tNow() failed with %terr", error));
		else if (PR_FAIL(error = DTIntervalGet(&dt, 0, &qwDays, (tQWORD)24*(tQWORD)60*(tQWORD)60*(tQWORD)1000000000)))
			PR_TRACE((_this, prtERROR, "sfdb\tDTIntervalGet() failed with %terr", error));
		else
			ldata->dwCurrentDate = (tDWORD)qwDays;
	}
	if (PR_SUCC(error))
		*pdwDate = ldata->dwCurrentDate;
	return error;
}

int pr_call CompareRecords8(const void *data1, const void *data2)
{
	unsigned __int64* i1 = (unsigned __int64*)data1;
	unsigned __int64* i2 = (unsigned __int64*)data2;
	if (*i1 == *i2)
		return 0;
	if (*i1 < *i2)
		return -1;
	return 1;
}

tDWORD pr_call GetIndex8(struct tag_LOCALDATA* ldata, tVOID* pRecordID)
{
	LARGE_INTEGER i = *(LARGE_INTEGER*)pRecordID;
	
	if (ldata->pDB == NULL)
	{
		PR_TRACE(( NULL, prtERROR, "sfdb\tGetIndex8: database not initialized"));
		return errOBJECT_NOT_INITIALIZED;
	}
	
	return ((ldata->pDB->dwIndexMask & i.HighPart) >> (32 - ldata->pDB->dwIndexBits));
}

tDWORD GetLeftBitPos(tDWORD dwNumber)
{
	if (dwNumber >> 1)
		return GetLeftBitPos(dwNumber >> 1) + 1;
	return 0;
}

tBOOL AssignRecordCompareFunc(hi_SecureFileDatabase _this)
{
	LOCALDATA* ldata = (LOCALDATA*)(_this->data);
	
	if (ldata->pDB == NULL)
	{
		PR_TRACE(( _this, prtERROR, "sfdb\tAssignRecordCompareFunc: database not initialized"));
		return errOBJECT_NOT_INITIALIZED;
	}
#if !defined (__unix__)
	__TRY
	{
#endif
		switch (ldata->pDB->dwRecordIDSize)
		{
		case 8:
			ldata->RecordCompareFunc = CompareRecords8;
			ldata->GetIndex = GetIndex8;
			break;
		default:
			PR_TRACE(( _this, prtERROR, "sfdb\tAssignRecordCompareFunc: incompatible RecordID size (%d)", ldata->pDB->dwRecordIDSize));
			return cFALSE;
		}
#if !defined (__unix__)
	} __EXCEPT (EXCEPTION_EXECUTE_HANDLER) {
		return cFALSE;
	}
#endif
	return cTRUE;
}

tERROR RemoveTempFiles(hi_SecureFileDatabase _this)
{
	CHAR szFullPath[MAX_PATH];
	LPSTR szFilePart;
	tCHAR szTempFilePath[MAX_PATH];
	tDWORD dwPathSize;
	WIN32_FIND_DATA sFindData;
	HANDLE hFindFile;
	
	dwPathSize = GetTempPath(countof(szTempFilePath), szTempFilePath);
	if (dwPathSize > countof(szTempFilePath) || dwPathSize == 0){
		PR_TRACE(( _this, prtERROR, "sfdb\tRemoveTempFiles: GetTempFile failed with error=0x%08X", GetLastError()));
		return errUNEXPECTED;
	}
	
	// remove crap
	szFilePart = szFullPath + dwPathSize;
	strcpy(szFullPath, szTempFilePath);
	strcpy(szFilePart, TEMP_FILE_PREFIX "*.tmp");
	hFindFile = FindFirstFile(szFullPath, &sFindData);
	if (hFindFile != INVALID_HANDLE_VALUE)
	{
		do
		{
			lstrcpy(szFilePart, sFindData.cFileName);
			DeleteFile(szFullPath);
			PR_TRACE(( _this, prtNOT_IMPORTANT, "sfdb\tRemoveTempFiles: DeleteFile %s err=%08X", szFullPath, GetLastError()));
		} while (FindNextFile(hFindFile, &sFindData));
		FindClose(hFindFile);
	}
	
	return errOK;
}

tERROR GetMappedFileName(hi_SecureFileDatabase _this)
{
	LOCALDATA* ldata = (LOCALDATA*)(_this->data);
	tCHAR szTempFilePath[MAX_PATH];
	tDWORD dwPathSize;
	tUINT nUnique = 0;
	
	RemoveTempFiles(_this);
	
	dwPathSize = GetTempPath(countof(szTempFilePath), szTempFilePath);
	if (dwPathSize > countof(szTempFilePath) || dwPathSize == 0){
		PR_TRACE(( _this, prtERROR, "sfdb\tGetMappedFileName: GetTempFile failed with error=0x%08X", GetLastError()));
		return errUNEXPECTED;
	}
	
	ldata->szSFDBSharedName[0] = 0;
	if (ldata->szSFDBPathName[0])
	{
		tBYTE sMD5Ctx[0x80];
		BYTE byCheckSum[0x10];
		DWORD dwBytes = sizeof(byCheckSum);
		int i;
		
		if (!hash_api_md5.Init(0, &sMD5Ctx, sizeof(sMD5Ctx), 16, NULL))
			return errUNEXPECTED;
		if (!hash_api_md5.Update(&sMD5Ctx, ldata->szSFDBPathName, (unsigned long)strlen(ldata->szSFDBPathName)))
			return errUNEXPECTED;
		if (!hash_api_md5.Finalize(&sMD5Ctx, (unsigned long*)&dwBytes, &byCheckSum))
		{
			PR_TRACE(( _this, prtERROR, "sfdb\tGetMappedFileName: dwUnique hash_api_md5.Finalize failed!"));
			return errUNEXPECTED;
		}
		for (i=0; i<4; i++)
			nUnique ^= *(((tDWORD*)byCheckSum)+i);
		wsprintf(ldata->szSFDBSharedName, TEMP_FILE_PREFIX"%08X", nUnique);
		strcpy(ldata->szTempFile, szTempFilePath);
		strcat(ldata->szTempFile, ldata->szSFDBSharedName);
		strcat(ldata->szTempFile, ".tmp");
	}
	else if (GetTempFileName(szTempFilePath, TEMP_FILE_PREFIX, 0, ldata->szTempFile) == 0)
	{
		PR_TRACE(( _this, prtERROR, "sfdb\tGetMappedFileName: GetTempFileName failed with error=0x%08X", GetLastError()));
		return errUNEXPECTED;
	}
	else
	{
		DeleteFile(ldata->szTempFile); // remove newly created file
	}
		
	PR_TRACE(( _this, prtNOT_IMPORTANT, "sfdb\tGetMappedFileName: szTempFile = %s", ldata->szTempFile));
	
	return errOK;
}

tERROR CloseMappedFile(hi_SecureFileDatabase _this)
{
	LOCALDATA* ldata = (LOCALDATA*)(_this->data);
	
	PR_TRACE(( _this, prtNOT_IMPORTANT, "sfdb\tCloseMappedFile: %s", ldata->szTempFile));
	
	if (ldata->pMappingView)
	{
		ldata->pDB->dwActiveClients--; // disconnect
		if (!FlushViewOfFile(ldata->pMappingView, 0))
			PR_TRACE(( _this, prtERROR, "sfdb\tCloseMappedFile: FlushViewOfFile failed with error=0x%08X", GetLastError()));
		if (!UnmapViewOfFile(ldata->pMappingView))
			PR_TRACE(( _this, prtERROR, "sfdb\tCloseMappedFile: UnmapViewOfFile failed with error=0x%08X", GetLastError()));
		ldata->pMappingView = NULL;
		ldata->dwMapViewSize = 0;
	}
	if (ldata->hFileMapping)
	{
		CloseHandle(ldata->hFileMapping);
		ldata->hFileMapping = NULL;
	}
	if (ldata->hTempFile && ldata->hTempFile != INVALID_HANDLE_VALUE)
	{
		CloseHandle(ldata->hTempFile);
		ldata->hTempFile = INVALID_HANDLE_VALUE;
	}
	ldata->pDB = NULL;
	
	RemoveTempFiles(_this);
	
	return errOK;
}

tERROR CreateMappedFile(hi_SecureFileDatabase _this, SFDB_HDR* pDBHdr)
{
	LOCALDATA* ldata = (LOCALDATA*)(_this->data);
	tERROR error;
	tDWORD dwWin32Error;
	tBOOL bConnectedToSharedMMF = cFALSE;
	
	CloseMappedFile(_this);
	
	if (PR_FAIL(error = GetMappedFileName(_this)))
	{
		PR_TRACE(( _this, prtERROR, "sfdb\tCreateMappedFile: GetMappedFileName failed with error=0x%08X", error));
		return errUNEXPECTED;
	}
	
	PR_TRACE((_this, prtNOTIFY, "sfdb\tCreateMappedFile: %s", ldata->szTempFile));
#if !defined (__unix__)
	__TRY 
	{
#endif
	/*
	ldata->hTempFile = CreateFile(ldata->szTempFile, GENERIC_READ | GENERIC_WRITE, _FILE_SHARE_MODE, NULL, CREATE_ALWAYS, 0, NULL);
	if (ldata->hTempFile == INVALID_HANDLE_VALUE)
	{
	PR_TRACE(( _this, prtERROR, "sfdb\tCreateMappedFile: CreateFile failed with error=0x%08X", GetLastError()));
	return errUNEXPECTED;
	}
		*/
		
		//ldata->hFileMapping = CreateFileMapping(ldata->hTempFile, NULL, PAGE_READWRITE, 0, pDBHdr->dwFileSize, (ldata->szSFDBSharedName[0] ? ldata->szSFDBSharedName : NULL));
		//ldata->hFileMapping = CreateFileMapping(INVALID_HANDLE_VALUE, NULL, PAGE_READWRITE, 0, pDBHdr->dwFileSize, (ldata->szSFDBSharedName[0] ? ldata->szSFDBSharedName : NULL));
		

		ldata->hFileMapping = CreateFileMapping(INVALID_HANDLE_VALUE, SA_Get(ldata->pHSA), PAGE_READWRITE|SEC_RESERVE, 0, ldata->dwDatabaseSizeLimit + pDBHdr->dwHeaderSize, (ldata->szSFDBSharedName[0] ? ldata->szSFDBSharedName : NULL));
		dwWin32Error = GetLastError();

		PR_TRACE(( _this, prtIMPORTANT, "sfdb\tCreateMappedFile: CreateFileMapping with size %d (%d)", ldata->dwDatabaseSizeLimit + pDBHdr->dwHeaderSize, dwWin32Error));

		if (dwWin32Error == ERROR_ALREADY_EXISTS)
			bConnectedToSharedMMF = cTRUE;
		
		if (ldata->hFileMapping == NULL)
		{
			PR_TRACE(( _this, prtERROR, "sfdb\tCreateMappedFile: CreateFileMapping failed with error=0x%08X", dwWin32Error));
			return errUNEXPECTED;
		}
		
		ldata->dwMapViewSize = pDBHdr->dwFileSize;
		ldata->pMappingView = MapViewOfFile(ldata->hFileMapping, FILE_MAP_ALL_ACCESS, 0, 0, ldata->dwMapViewSize);
		if (ldata->pMappingView == NULL)
		{
			PR_TRACE(( _this, prtERROR, "sfdb\tExpandMappedFile: MapViewOfFile failed with error=0x%08X", GetLastError()));
			return errUNEXPECTED;
		}
#if defined (_WIN32)		
		ldata->pMappingView = VirtualAlloc(ldata->pMappingView, ldata->dwMapViewSize, MEM_COMMIT, PAGE_READWRITE);
#endif
		if (ldata->pMappingView == NULL)
		{
			PR_TRACE(( _this, prtERROR, "sfdb\tExpandMappedFile: VirtualAlloc failed with error=0x%08X", GetLastError()));
			return errUNEXPECTED;
		}
		
		// fix troubles under WinME where MMF not inited with zeros while growing...
		if (ldata->bWin9xSystem)
			memset((tBYTE*)ldata->pMappingView, 0, ldata->dwMapViewSize);
		
#ifdef _DEBUG_MEMIMAGE
		{
			DWORD dw;
			HANDLE hFile = CreateFile("C:\\memimage.dat", GENERIC_READ | GENERIC_WRITE, 0, 0, CREATE_ALWAYS, 0, 0);
			if (hFile != INVALID_HANDLE_VALUE)
			{
				WriteFile(hFile, ldata->pMappingView, pDBHdr->dwFileSize, &dw, 0);
				CloseHandle(hFile);
			}
		}
#endif
		ldata->pDB = (SFDB_HDR*)ldata->pMappingView;
		
		if (bConnectedToSharedMMF == cFALSE)
		{
			memcpy(ldata->pDB, pDBHdr, sizeof(SFDB_HDR));
			ldata->pDB->dwActiveClients = 1;
		}
		else
			ldata->pDB->dwActiveClients++;
		
		if (!AssignRecordCompareFunc(_this))
		{
			PR_TRACE(( _this, prtERROR, "sfdb\tCreateMappedFile: AssignRecordCompareFunc failed"));
			return errUNEXPECTED;
		}
		
#ifdef _DEBUG_MEMIMAGE
		{
			DWORD dw;
			HANDLE hFile = CreateFile("C:\\memimage2.dat", GENERIC_READ | GENERIC_WRITE, 0, 0, CREATE_ALWAYS, 0, 0);
			if (hFile != INVALID_HANDLE_VALUE)
			{
				WriteFile(hFile, ldata->pMappingView, pDBHdr->dwFileSize, &dw, 0);
				CloseHandle(hFile);
			}
		}
#endif
#if !defined (__unix__)
	} 
	__EXCEPT (EXCEPTION_EXECUTE_HANDLER) 
	{
		PR_TRACE(( _this, prtERROR, "sfdb\tCreateMappedFile: exception handled 0x%08X", GetExceptionCode()));
		CloseMappedFile(_this);
		return errUNEXPECTED;
	}
#endif	
	PR_TRACE((_this, prtNOTIFY, "sfdb\tCreateMappedFile: OK. pMappingView=%08X, size=%08X", ldata->pMappingView, pDBHdr->dwFileSize));
	return errOK;
}

tERROR OpenMappedFile(hi_SecureFileDatabase _this)
{
	LOCALDATA* ldata = (LOCALDATA*)(_this->data);
	SFDB_HDR sDBHdr;
	tDWORD dwBytes;
	tDWORD dwWin32Error;
	tBOOL bConnectedToSharedMMF = cFALSE;
	
	CloseMappedFile(_this);
	
	PR_TRACE((_this, prtNOT_IMPORTANT, "sfdb\tOpenMappedFile: %s", ldata->szTempFile));
	
	if (PR_FAIL(GetMappedFileName(_this)))
	{
		PR_TRACE((_this, prtERROR, "sfdb\tGetMappedFileName failed %terr"));
		return errUNEXPECTED;
	}
#if !defined (__unix__)	
	__TRY
	{
#endif
		if (ldata->szSFDBSharedName[0])
		{
			PR_TRACE(( _this, prtIMPORTANT, "sfdb\tOpenMappedFile: MapViewOfFile %s", ldata->szSFDBSharedName));
			ldata->hFileMapping = OpenFileMapping(FILE_MAP_ALL_ACCESS, 0, ldata->szSFDBSharedName);
		}
		
		if (ldata->hFileMapping != NULL)
		{
			ldata->pMappingView = MapViewOfFile(ldata->hFileMapping, FILE_MAP_ALL_ACCESS, 0, 0, sizeof(SFDB_HDR));
			if (ldata->pMappingView == NULL)
			{
				PR_TRACE(( _this, prtNOT_IMPORTANT, "sfdb\tOpenMappedFile: MapViewOfFile failed with error=0x%08X", GetLastError()));
				return errUNEXPECTED;
			}
			memcpy(&sDBHdr, ldata->pMappingView, sizeof(SFDB_HDR));
			UnmapViewOfFile(ldata->pMappingView);
			bConnectedToSharedMMF = cTRUE;
			PR_TRACE(( _this, prtNOTIFY, "sfdb\tOpenMappedFile: connected to MMF '%s'", ldata->szSFDBSharedName));
		}
		else
		{
			ldata->hTempFile = CreateFile(ldata->szTempFile, GENERIC_READ | GENERIC_WRITE, _FILE_SHARE_MODE, NULL, OPEN_EXISTING, 0, NULL);
			if (ldata->hTempFile == INVALID_HANDLE_VALUE)
			{
				PR_TRACE(( _this, prtNOT_IMPORTANT, "sfdb\tOpenMappedFile: CreateFile failed with error=0x%08X", GetLastError()));
				return errUNEXPECTED;
			}
			
			ReadFile(ldata->hTempFile, &sDBHdr, sizeof(SFDB_HDR), &dwBytes, NULL);
			if (dwBytes != sizeof(SFDB_HDR))
			{
				PR_TRACE(( _this, prtERROR, "sfdb\tOpenMappedFile: Cannot read database header. Error=0x%08X", GetLastError()));
				return errUNEXPECTED;
			}
			
			ldata->hFileMapping = CreateFileMapping(ldata->hTempFile, SA_Get(ldata->pHSA), PAGE_READWRITE, 0, sDBHdr.dwFileSize, (ldata->szSFDBSharedName[0] ? ldata->szSFDBSharedName : NULL));
			dwWin32Error = GetLastError();
			if (dwWin32Error == ERROR_ALREADY_EXISTS)
				bConnectedToSharedMMF = cTRUE;
			if (ldata->hFileMapping == NULL)
			{
				PR_TRACE(( _this, prtERROR, "sfdb\tOpenMappedFile: CreateFileMapping failed with error=0x%08X", GetLastError()));
				return errUNEXPECTED;
			}
		}
		
		ldata->dwMapViewSize = sDBHdr.dwFileSize;
		ldata->pMappingView = MapViewOfFile(ldata->hFileMapping, FILE_MAP_ALL_ACCESS, 0, 0, ldata->dwMapViewSize);
		if (ldata->pMappingView == NULL)
		{
			PR_TRACE(( _this, prtERROR, "sfdb\tOpenMappedFile: MapViewOfFile failed with error=0x%08X", GetLastError()));
			return errUNEXPECTED;
		}
		
		ldata->pDB = (SFDB_HDR*)ldata->pMappingView;
		if (!AssignRecordCompareFunc(_this))
		{
			PR_TRACE(( _this, prtERROR, "sfdb\tOpenMappedFile: AssignRecordCompareFunc failed"));
			return errUNEXPECTED;
		}
		
		if (bConnectedToSharedMMF == cFALSE)
			ldata->pDB->dwActiveClients = 1;
		else
			ldata->pDB->dwActiveClients++;
#if !defined (__unix__)	
	} 
	__EXCEPT (EXCEPTION_EXECUTE_HANDLER) 
	{
		PR_TRACE(( _this, prtERROR, "sfdb\tOpenMappedFile: exception handled 0x%08X", GetExceptionCode()));
		CloseMappedFile(_this);
		return errUNEXPECTED;
	}
#endif	
	
	PR_TRACE((_this, prtNOTIFY, "sfdb\tOpenMappedFile: OK. pMappingView=%08X, size=%08X", ldata->pMappingView, sDBHdr.dwFileSize));
	return errOK;
}

tERROR RemapViewOfFile(hi_SecureFileDatabase _this, tDWORD dwNewViewSize)
{
	LOCALDATA* ldata = (LOCALDATA*)(_this->data);
	tVOID* pOldMappingView = ldata->pMappingView;
	
	if (pOldMappingView)
	{
		if (!FlushViewOfFile(pOldMappingView, 0))
			PR_TRACE(( _this, prtERROR, "sfdb\tExpandMappedFile: FlushViewOfFile failed with error=0x%08X", GetLastError()));
	}
	
	ldata->pMappingView = MapViewOfFile(ldata->hFileMapping, FILE_MAP_ALL_ACCESS, 0, 0, dwNewViewSize);
	if (ldata->pMappingView == NULL)
	{
		PR_TRACE(( _this, prtERROR, "sfdb\tExpandMappedFile: MapViewOfFile failed with error=0x%08X", GetLastError()));
		return errUNEXPECTED;
	}
	
#if defined (_WIN32)
	ldata->pMappingView = VirtualAlloc(ldata->pMappingView, dwNewViewSize, MEM_COMMIT, PAGE_READWRITE);
#endif
	if (ldata->pMappingView == NULL)
	{
		PR_TRACE(( _this, prtERROR, "sfdb\tExpandMappedFile: VirtualAlloc failed with error=0x%08X", GetLastError()));
		return errUNEXPECTED;
	}
	
	// fix troubles under WinME where MMF not inited with zeros while growing...
	if (ldata->bWin9xSystem && ldata->dwMapViewSize < dwNewViewSize )
		memset(((tBYTE*)ldata->pMappingView) + ldata->dwMapViewSize, 0, dwNewViewSize-ldata->dwMapViewSize);
	
	if (pOldMappingView)
	{
		if (!UnmapViewOfFile(pOldMappingView))
			PR_TRACE(( _this, prtERROR, "sfdb\tExpandMappedFile: UnmapViewOfFile failed with error=0x%08X", GetLastError()));
	}
	
	ldata->dwMapViewSize = dwNewViewSize;
	ldata->pDB = (SFDB_HDR*)ldata->pMappingView;
	if (!AssignRecordCompareFunc(_this))
	{
		PR_TRACE(( _this, prtERROR, "sfdb\tExpandMappedFile: AssignRecordCompareFunc failed"));
		CloseMappedFile(_this);
		return errUNEXPECTED;
	}
	
	return errOK;
}

tERROR ExpandMappedFile(hi_SecureFileDatabase _this, tDWORD dwNewFileSize)
{
	LOCALDATA* ldata = (LOCALDATA*)(_this->data);
	
	if (ldata->hTempFile == NULL)
	{
		PR_TRACE(( _this, prtERROR, "sfdb\tExpandMappedFile: database not initialized"));
		return errOBJECT_NOT_INITIALIZED;
	}
	
	if (ldata->pMappingView)
	{
		if (!FlushViewOfFile(ldata->pMappingView, 0))
			PR_TRACE(( _this, prtERROR, "sfdb\tExpandMappedFile: FlushViewOfFile failed with error=0x%08X", GetLastError()));
		if (!UnmapViewOfFile(ldata->pMappingView))
			PR_TRACE(( _this, prtERROR, "sfdb\tExpandMappedFile: UnmapViewOfFile failed with error=0x%08X", GetLastError()));
	}
	
	if (ldata->hFileMapping)
		CloseHandle(ldata->hFileMapping);
	
	ldata->hFileMapping = CreateFileMapping(ldata->hTempFile, SA_Get(ldata->pHSA), PAGE_READWRITE, 0, dwNewFileSize, (ldata->szSFDBSharedName[0] ? ldata->szSFDBSharedName : NULL));
	if (ldata->hFileMapping == NULL)
	{
		PR_TRACE(( _this, prtERROR, "sfdb\tExpandMappedFile: CreateFileMapping failed with error=0x%08X", GetLastError()));
		CloseMappedFile(_this);
		return errUNEXPECTED;
	}
	
	ldata->pMappingView = MapViewOfFile(ldata->hFileMapping, FILE_MAP_ALL_ACCESS, 0, 0, dwNewFileSize);
	if (ldata->pMappingView == NULL)
	{
		PR_TRACE(( _this, prtERROR, "sfdb\tExpandMappedFile: MapViewOfFile failed with error=0x%08X", GetLastError()));
		CloseMappedFile(_this);
		return errUNEXPECTED;
	}
	
	ldata->pDB = (SFDB_HDR*)ldata->pMappingView;
	if (!AssignRecordCompareFunc(_this))
	{
		PR_TRACE(( _this, prtERROR, "sfdb\tExpandMappedFile: AssignRecordCompareFunc failed"));
		CloseMappedFile(_this);
		return errUNEXPECTED;
	}
	
	return errOK;
}

tERROR ExpandDatabase(hi_SecureFileDatabase _this)
{
	SFDB_CLUSTER_HDR* pClusterNewH;
	SFDB_CLUSTER_HDR* pClusterNewL;
	SFDB_CLUSTER_HDR* pClusterOld;
	tBYTE* pRecord;
	LOCALDATA* ldata = (LOCALDATA*)(_this->data);
	SFDB_HDR* pDB;
	tDWORD i;
	tERROR error;
	
	if (ldata->pDB == NULL)
	{
		PR_TRACE(( _this, prtERROR, "sfdb\tExpandDatabase: database not initialized"));
		return errOBJECT_NOT_INITIALIZED;
	}
	
	PR_TRACE(( _this, prtNOTIFY, "sfdb\tEnter ExpandDatabase method"));
#if !defined (__unix__)	
	__TRY {
#endif
		SFDB_HDR sDBHdr;
		tDWORD dwOldSize = ldata->pDB->dwFileSize;
		
		memcpy(&sDBHdr, ldata->pDB, sizeof(SFDB_HDR));
		
		sDBHdr.dwIndexBits++;
		sDBHdr.dwIndexMask >>= 1;
		sDBHdr.dwIndexMask |= 0x80000000;
		sDBHdr.dwClusters = 1 << sDBHdr.dwIndexBits;
		sDBHdr.dwFileSize = sDBHdr.dwHeaderSize + sDBHdr.dwClusters * sDBHdr.dwClusterSize;
		
		if (sDBHdr.dwFileSize > ldata->dwDatabaseSizeLimit + sDBHdr.dwHeaderSize)
			return errOUT_OF_SPACE;
		
		//if (PR_FAIL(error = ExpandMappedFile(_this, sDBHdr.dwFileSize)))
		if (PR_FAIL(error = RemapViewOfFile(_this, sDBHdr.dwFileSize)))
		{
			PR_TRACE((_this, prtERROR, "sfdb\tExpandDatabase: RemapViewOfFile failed with %terr", error));
			return error;
		}
		
		// fix troubles under WinME where MMF not inited with zeros while growing...
		if (ldata->bWin9xSystem && sDBHdr.dwFileSize > dwOldSize)
			memset((tBYTE*)ldata->pMappingView + dwOldSize, 0, sDBHdr.dwFileSize - dwOldSize);
		
		pDB = ldata->pDB;
		memcpy(pDB, &sDBHdr, sizeof(SFDB_HDR));
		
		pClusterOld = (SFDB_CLUSTER_HDR*)((BYTE*)ldata->pMappingView + pDB->dwHeaderSize + (pDB->dwClusters / 2 - 1) * pDB->dwClusterSize);
		pClusterNewH = (SFDB_CLUSTER_HDR*)((BYTE*)ldata->pMappingView + pDB->dwHeaderSize + (pDB->dwClusters - 1) * pDB->dwClusterSize);
		pClusterNewL = (SFDB_CLUSTER_HDR*)((BYTE*)pClusterNewH - pDB->dwClusterSize);
		
		while (1) 
		{
			if (pClusterOld->dwRecords == 0 && (pClusterNewL->dwRecords | pClusterNewH->dwRecords) != 0)
			{
				// clean up 2 clusters
				pClusterNewL->dwRecords = pClusterNewH->dwRecords = 0;
#ifdef _DEBUG
				memset(pClusterNewL, 0, pDB->dwClusterSize * 2);
#endif
			}
			else
			{
				pRecord = (tBYTE*)(pClusterOld+1);
				for (i=0;i<pClusterOld->dwRecords; i++)
				{
					if (ldata->GetIndex(ldata, pRecord) & 1)
						break;
					pRecord += pDB->dwRecordSize;
				}
				
				pClusterNewH->dwRecords = pClusterOld->dwRecords - i;
				memcpy(pClusterNewH+1, pRecord, pClusterNewH->dwRecords * pDB->dwRecordSize);
#ifdef _DEBUG
				memset((tBYTE*)(pClusterNewH+1) + pClusterNewH->dwRecords * pDB->dwRecordSize, 0, (pDB->dwRecordsPerCluster - pClusterNewH->dwRecords)*pDB->dwRecordSize);
#endif				
				pClusterNewL->dwRecords = i;
				memcpy(pClusterNewL+1, pClusterOld+1, pClusterNewL->dwRecords*pDB->dwRecordSize);
#ifdef _DEBUG
				memset((tBYTE*)(pClusterNewL+1) + pClusterNewL->dwRecords*pDB->dwRecordSize, 0, (pDB->dwRecordsPerCluster - pClusterNewL->dwRecords)*pDB->dwRecordSize);
#endif				
			}
			
			if (pClusterOld == pClusterNewL) // exit while
				break;
			
			pClusterOld = (SFDB_CLUSTER_HDR*)((BYTE*)pClusterOld - pDB->dwClusterSize);;
			pClusterNewH = (SFDB_CLUSTER_HDR*)((BYTE*)pClusterNewH - pDB->dwClusterSize*2);
			pClusterNewL = (SFDB_CLUSTER_HDR*)((BYTE*)pClusterNewL - pDB->dwClusterSize*2);
		}
#if !defined (__unix__)	
	}
	__EXCEPT (EXCEPTION_EXECUTE_HANDLER) 
	{
		PR_TRACE(( _this, prtERROR, "sfdb\tExpandDatabase: exception handled 0x%08X", GetExceptionCode()));
		return errUNEXPECTED;
	}
#endif	
	PR_TRACE(( _this, prtNOTIFY, "sfdb\tLeave ExpandDatabase method, ret tERROR = 0x%08x", error));
	return errOK;
}

#define DELETE_OLDEST_RECORDS_IN_CLUSTER_COUNT		3
#define DELETE_OLDEST_RECORDS_IN_CLUSTER_COUNT_MAX	10
tERROR DeleteOldestRecordsInCluster(hi_SecureFileDatabase _this, SFDB_CLUSTER_HDR* pCluster)
{
	LOCALDATA* ldata = (LOCALDATA*)(_this->data);
	SFDB_HDR* pDB = ldata->pDB;
	tBYTE* pRecord;
	tDWORD dwRecord;
	SFDB_RECORD_INTERNAL_DATA* pRecordInternalData;
	tDWORD dwOldestDate[DELETE_OLDEST_RECORDS_IN_CLUSTER_COUNT];
	tDWORD i;
	tDWORD dwRecordsDeleted = 0;

	if (pCluster->dwRecords < ldata->pDB->dwRecordsPerCluster)
		return errOK;
	
	// ask client to delete some records
	{
		tERROR error;
		tBYTE* pBuff;
		tDWORD dwBuffSize = pCluster->dwRecords * pDB->dwRecordSize;
		error = CALL_SYS_ObjHeapAlloc(_this, &pBuff, dwBuffSize);
		if (PR_SUCC(error))
		{
			pRecord = (tBYTE*)(pCluster+1);
			memcpy(pBuff, pRecord, pCluster->dwRecords);
			CALL_SYS_SendMsg(_this, pmcSFDB, pmUPDATE_CLUSTER_FULL, NULL, pBuff, &dwBuffSize);
			CALL_SYS_ObjHeapFree(_this, pBuff);
		}
	}

	// client freed cluster
	if (pCluster->dwRecords < ldata->pDB->dwRecordsPerCluster)
		return errOK;

	for (i=0; i<DELETE_OLDEST_RECORDS_IN_CLUSTER_COUNT; i++)
		dwOldestDate[i] = (tDWORD)-1;
	
	if (pCluster->dwRecords > ldata->pDB->dwRecordsPerCluster) // validation
	{
		PR_TRACE(( _this, prtERROR, "sfdb\tDeleteOldestRecordsInCluster: Cluster contain %d records? (RecordsPerCluster=%d)", pCluster->dwRecords, pDB->dwRecordsPerCluster));
		return errUNEXPECTED;
	}
	
	// look for oldest date
	pRecord = (tBYTE*)(pCluster+1);
	for (dwRecord=0; dwRecord<pCluster->dwRecords; dwRecord++){
		pRecordInternalData = (SFDB_RECORD_INTERNAL_DATA*)(pRecord + pDB->dwRecordSize - sizeof(SFDB_RECORD_INTERNAL_DATA) - sizeof(SFDB_RECORD_SESSION_DATA));
		if (dwOldestDate[0] > pRecordInternalData->dwLastUsedDate) 
		{
			for (i=DELETE_OLDEST_RECORDS_IN_CLUSTER_COUNT-1; i>0; i--)
				dwOldestDate[i] = dwOldestDate[i-1];
			dwOldestDate[0] = pRecordInternalData->dwLastUsedDate;
		}
		pRecord += pDB->dwRecordSize;
	}
	
	// delete oldest records
	pRecord = (tBYTE*)(pCluster+1);
	for (dwRecord=0; dwRecord<pCluster->dwRecords; dwRecord++){
		pRecordInternalData = (SFDB_RECORD_INTERNAL_DATA*)(pRecord + pDB->dwRecordSize - sizeof(SFDB_RECORD_INTERNAL_DATA) - sizeof(SFDB_RECORD_SESSION_DATA));
		for (i=0; i<DELETE_OLDEST_RECORDS_IN_CLUSTER_COUNT; i++)
		{
			if (dwOldestDate[i] == pRecordInternalData->dwLastUsedDate) // one of oldest records
			{
				PR_TRACE((_this, prtIMPORTANT, "sfdb\tRecord %016I64X discarded", *(tQWORD*)pRecord));
				pCluster->dwRecords--;
				memmove(pRecord, pRecord + pDB->dwRecordSize, pDB->dwRecordSize * (pCluster->dwRecords - dwRecord));
				dwRecord--;
				pRecord -= pDB->dwRecordSize;
				if (pCluster->dwRecords == 0)
					return errOK;
				if (++dwRecordsDeleted == DELETE_OLDEST_RECORDS_IN_CLUSTER_COUNT_MAX) // enougth
					return errOK;
				break;
			}
		}
		pRecord += pDB->dwRecordSize;
	}
	
	return errOK;
}

tERROR CompactDatabase(hi_SecureFileDatabase _this)
{
	LOCALDATA* ldata = (LOCALDATA*)(_this->data);
	SFDB_HDR* pDB = ldata->pDB;
	tDWORD dwCluster;
	SFDB_CLUSTER_HDR* pCluster;
	tBYTE* pRecord;
	tDWORD dwRecord;
	SFDB_RECORD_INTERNAL_DATA* pRecordInternalData;
	tDWORD dwCurrentDate;
	tERROR error = errNOT_OK;
	
	if (ldata->dwUnusedRecordsLimitTime == 0)
		return errNOT_OK;
	
	GetCurrentDateTime(_this, &dwCurrentDate);
	
	for (dwCluster=0; dwCluster<ldata->pDB->dwClusters; dwCluster++)
	{
		pCluster = (SFDB_CLUSTER_HDR*)((tBYTE*)ldata->pMappingView + pDB->dwHeaderSize + dwCluster * pDB->dwClusterSize);
		pRecord = (tBYTE*)(pCluster+1);
		if (pCluster->dwRecords > ldata->pDB->dwRecordsPerCluster) // validation
		{
			PR_TRACE(( _this, prtERROR, "sfdb\tCompactDatabase: Cluster %d contain %d records? (RecordsPerCluster=%d)", dwCluster, pCluster->dwRecords, pDB->dwRecordsPerCluster));
			return errUNEXPECTED;
		}
		for (dwRecord=0; dwRecord<pCluster->dwRecords; dwRecord++){
			pRecordInternalData = (SFDB_RECORD_INTERNAL_DATA*)(pRecord + pDB->dwRecordSize - sizeof(SFDB_RECORD_INTERNAL_DATA) - sizeof(SFDB_RECORD_SESSION_DATA));
			if (dwCurrentDate - pRecordInternalData->dwLastUsedDate > ldata->dwUnusedRecordsLimitTime) // old unused record!
			{
				pCluster->dwRecords--;
				memmove(pRecord, pRecord + pDB->dwRecordSize, pDB->dwRecordSize * (pCluster->dwRecords - dwRecord));
				dwRecord--;
				error = errOK;
			}
			else
				pRecord += pDB->dwRecordSize;
		}
	}
	
	return error;
}



// AVP Prague stamp begin( Internal (kernel called) interface method implementation, ObjectInit )
// Extended method comment
//  Kernel notifies an object about successful creating of object
// Behaviour comment
//  Initializes internal object data
// Result comment
//  Returns value differ from errOK if object cannot be initialized
// Parameters are:
tERROR pr_call SecureFileDatabase_ObjectInit( hi_SecureFileDatabase _this )
{
	//tERROR error;
	//tDWORD dwSize;
	OSVERSIONINFO sVerInfo;
	LOCALDATA* ldata;
	PR_TRACE_A0( _this, "Enter SecureFileDatabase::ObjectInit method" );
	
	//	if(PR_FAIL(error = CALL_SYS_MemorySize(_this, &dwSize))){
	//		PR_TRACE(( _this, prtERROR, "sfdb\tObjectInit: Cannot get object memory size. Error=0x%08X", error));
	//		return errOBJECT_CANNOT_BE_INITIALIZED;
	//	}

	//	if (dwSize != 0){
	//		PR_TRACE(( _this, prtERROR, "sfdb\tObjectInit: Unexpected object memory size - must be zero. dwSize=0x%08X", dwSize));
	//		return errOBJECT_CANNOT_BE_INITIALIZED;
	//	}
	
	//	if(PR_FAIL(error = CALL_SYS_MemoryRealloc(_this, sizeof(LOCALDATA)))){
	//		PR_TRACE(( _this, prtERROR, "sfdb\tObjectInit: Cannot realloc object memory. Error=0x%08X", error));
	//		return errOBJECT_CANNOT_BE_INITIALIZED;
	//	}
	
	sVerInfo.dwOSVersionInfoSize = sizeof(OSVERSIONINFO);
	if (!GetVersionEx(&sVerInfo)){
		PR_TRACE(( _this, prtERROR, "sfdb\tObjectInit: Cannot get version ifo. Win32Error=0x%08X", GetLastError()));
		return errOBJECT_CANNOT_BE_INITIALIZED;
	}
	
	ldata = (LOCALDATA*)(_this->data);
	if (sVerInfo.dwPlatformId == VER_PLATFORM_WIN32_WINDOWS)
		ldata->bWin9xSystem = cTRUE;
	else
		ldata->pHSA = SA_Create(SA_AUTHORITY_EVERYONE, SA_ACCESS_MASK_ALL);
	
	ldata->dwDatabaseSizeLimit = 0x100000; // 1Mb
	ldata->dwUnusedRecordsLimitTime = 180;
	
	PR_TRACE_A1( _this, "Leave SecureFileDatabase::ObjectInit method, ret tERROR = 0x%08x", error );
	return errOK;
}
// AVP Prague stamp end



// AVP Prague stamp begin( Internal (kernel called) interface method implementation, ObjectInitDone )
// Extended method comment
//  Notification that all necessary properties have been set and object must go to operation state
// Result comment
//  Returns value differ from errOK if object cannot function properly
// Parameters are:
tERROR pr_call SecureFileDatabase_ObjectInitDone( hi_SecureFileDatabase _this )
{
	tERROR error;
	
	LOCALDATA* ldata = (LOCALDATA*)(_this->data);
	
	PR_TRACE_A0( _this, "Enter SecureFileDatabase::ObjectInitDone method" );
	
#if defined(_USE_CRITICAL_SECTION_)
	error = CALL_SYS_ObjectCreateQuick(_this, &ldata->hCriticalSection, IID_CRITICAL_SECTION, PID_ANY, SUBTYPE_ANY);
#elif defined(_USE_MUTEX_) || defined(_USE_MUTEX_TIMED_)
	error = CALL_SYS_ObjectCreate(_this, &ldata->hMutex, IID_MUTEX, PID_ANY, SUBTYPE_ANY);
	if (PR_SUCC(error))
	{
		error = CALL_SYS_PropertySetStr(ldata->hMutex, NULL, pgOBJECT_NAME, MUTEX_NAME, sizeof(MUTEX_NAME), cCP_ANSI);
		if (PR_SUCC(error))
			error = CALL_SYS_ObjectCreateDone(ldata->hMutex);
	}
#endif
	
	if(PR_FAIL(error)){
		PR_TRACE(( _this, prtERROR, "sfdb\tObjectInitDone: Cannot init sync object. Error=0x%08X", error));
		return errOBJECT_CANNOT_BE_INITIALIZED;
	}
	
	PR_TRACE_A1( _this, "Leave SecureFileDatabase::ObjectInitDone method, ret tERROR = 0x%08x", error );
	return errOK;
}
// AVP Prague stamp end



// AVP Prague stamp begin( Internal (kernel called) interface method implementation, ObjectPreClose )
// Extended method comment
//  Kernel warns object it is going to close all children
// Behaviour comment
//  Object takes all necessary "before preclosing" actions
// Parameters are:
tERROR pr_call SecureFileDatabase_ObjectPreClose( hi_SecureFileDatabase _this )
{
	tERROR error = errOK;
	LOCALDATA* ldata = (LOCALDATA*)(_this->data);
	
	PR_TRACE_A0( _this, "Enter SecureFileDatabase::ObjectPreClose method" );
	
	if ((ldata->dwSerializeOptions & cSFDB_AUTOSAVE) && ldata->szSFDBPathName[0] && ldata->pDB)
	{
		if (ldata->pDB->dwActiveClients == 1) // this is last client
			SecureFileDatabase_Save(_this, ldata->szSFDBPathName);
	}
	CloseMappedFile(_this);
	
#ifdef _USE_MUTEX_TIMED_
	{
		LARGE_INTEGER liFreq;
		LARGE_INTEGER liStart;
		LARGE_INTEGER liStop;
		tDWORD i;
		__int64 i1;
		__int64 i2;
		__int64 i3;
		char buff[0x100];
		QueryPerformanceFrequency(&liFreq);
		i1 = *(__int64*)&(ldata->liSyncOverhit);
		i2 = *(__int64*)&liFreq;
		QueryPerformanceCounter(&liStart);
		for (i=0; i<ldata->dwSyncCalls; i++)
			QueryPerformanceCounter(&liStop);
		i3 = *(unsigned __int64*)&liStop - *(unsigned __int64*)&liStart;
		wsprintf(buff, "SFDB Mutex overhit: %3u.%06u     QPC: %3u.%06u     calls=%u", (tDWORD)(i1/i2), (tDWORD)(((i1%i2)*1000000)/i2), (tDWORD)(i3/i2), (tDWORD)(((i3%i2)*1000000)/i2), ldata->dwSyncCalls);
		OutputDebugString(buff);
		OutputDebugString("\n");
		PR_TRACE((_this, prtNOTIFY, "sfdb\t%s", buff));
		
	}
#endif
	SA_Destroy(ldata->pHSA);
	
	PR_TRACE_A1( _this, "Leave SecureFileDatabase::ObjectPreClose method, ret tERROR = 0x%08x", error );
	return error;
}
// AVP Prague stamp end



// AVP Prague stamp begin( External (user called) interface method implementation, InitNew )
// Extended method comment
//  Метод подготавливает новую базу к работе.
// Parameters are:
// "dwRecordIDAlg"    : Впоследствии, его можно получить через GetDBInfo.
// "dwRecordIDSize"   : Указывается в байтах
// "dwRecordDataSize" : Указывается в байтах
// "dwIndexSize"      : Задает начальный размер индекса базы данных.
tERROR pr_call SecureFileDatabase_InitNew( hi_SecureFileDatabase _this, tDWORD dwRecordIDAlg, tDWORD dwRecordIDSize, tDWORD dwRecordDataSize, tDWORD dwDBInitialSize )
{
	tERROR error = errOK;
	SYSTEM_INFO sSystemInfo;
	SFDB_HDR sDBHdr;
	LOCALDATA* ldata = (LOCALDATA*)(_this->data);
	
	PR_TRACE_A0( _this, "Enter SecureFileDatabase::InitNew method" );
	
	PR_TRACE(( _this, prtNOTIFY, "sfdb\tInitNew: size=%d", dwDBInitialSize));
	
	if (PR_FAIL(error = WRITING_LOCK_EX(MUTEX_LONG_TIMEOUT)))
	{
		PR_TRACE((_this, prtERROR, "sfdb\tSyncronisation failed with %terr", error));
		return error;
	}
#if !defined (__unix__)	
	__TRY_F {
		__TRY {
#endif			
			
			// try to connect to existing MMF
			error = OpenMappedFile(_this);
			
			// if connect failed, create new MMF
			if (PR_FAIL(error))
			{
				PR_TRACE((_this, prtERROR, "sfdb\tOpen mapped (existing) file failed %terr", error));

				GetSystemInfo(&sSystemInfo);
				sDBHdr.dwSignature = SFDB_SIGNATURE; // SFDB
				sDBHdr.vDBVersion = 0x10000;
				
				sDBHdr.dwClusterSize = sSystemInfo.dwPageSize;
				
				sDBHdr.dwIndexBits = GetLeftBitPos(dwDBInitialSize/sDBHdr.dwClusterSize);
				if (sDBHdr.dwIndexBits)
					sDBHdr.dwIndexMask = ~((1 << (32-sDBHdr.dwIndexBits)) - 1);
				else
					sDBHdr.dwIndexMask = 0;
				
				sDBHdr.dwClusterHeaderSize = sizeof(SFDB_CLUSTER_HDR);
				sDBHdr.dwClusters = 1 << sDBHdr.dwIndexBits;
				
				sDBHdr.dwHeaderSize = (sizeof(SFDB_HDR) / sDBHdr.dwClusterSize + (sizeof(SFDB_HDR) % sDBHdr.dwClusterSize ?  1 : 0)) * sDBHdr.dwClusterSize;
				sDBHdr.dwFileSize = sDBHdr.dwHeaderSize + sDBHdr.dwClusters * sDBHdr.dwClusterSize;
				
				sDBHdr.dwRecordIDAlg = dwRecordIDAlg;
				sDBHdr.dwRecordIDSize = dwRecordIDSize;
				sDBHdr.dwRecordDataSize = dwRecordDataSize;
				sDBHdr.dwRecordSize = dwRecordIDSize + dwRecordDataSize + sizeof(SFDB_RECORD_INTERNAL_DATA) + sizeof(SFDB_RECORD_SESSION_DATA);
				sDBHdr.dwRecordsPerCluster = (sDBHdr.dwClusterSize - sDBHdr.dwClusterHeaderSize) / sDBHdr.dwRecordSize;
				sDBHdr.dwActiveClients = 1;
				
				error = CreateMappedFile(_this, &sDBHdr);
				
				if (PR_FAIL(error))
				{
					PR_TRACE((_this, prtERROR, "sfdb\tCreate mapped file failed %terr", error));
				}
			}
			
			if (PR_SUCC(error))
			{
				if (ldata->pDB->dwRecordIDAlg != dwRecordIDAlg ||
					ldata->pDB->dwRecordIDSize != dwRecordIDSize ||
					ldata->pDB->dwRecordDataSize != dwRecordDataSize)
				{
					CloseMappedFile(_this);
					error = errOBJECT_ALREADY_EXISTS;

					PR_TRACE((_this, prtERROR, "sfdb\tCreate mapped errOBJECT_ALREADY_EXISTS"));
				}
			}
			
#if !defined (__unix__)	
		}
		__EXCEPT (EXCEPTION_EXECUTE_HANDLER) {
			PR_TRACE(( _this, prtERROR, "sfdb\tInitNew: exception handled 0x%08X", GetExceptionCode()));
			error = errUNEXPECTED;
		}
	}
	__FINALLY {
#endif
		RELEASE();
#if !defined (__unix__)	
	}
#endif
	
	PR_TRACE_A1( _this, "Leave SecureFileDatabase::InitNew method, ret tERROR = 0x%08x", error );
	return error;
}
// AVP Prague stamp end



void __inline AjustCryptKey(hi_SecureFileDatabase _this, LPCVOID lpBuffer, DWORD nNumberOfBytes)
{
	tDWORD dwKeyDiff = 0;
	LOCALDATA* ldata = (LOCALDATA*)_this->data;
	if (nNumberOfBytes >= 4)
		dwKeyDiff = *(DWORD*)lpBuffer;
	ldata->dwCryptKey = (ldata->dwCryptKey + dwKeyDiff) ^ dwKeyDiff;
}

void __inline CryptBuffer(hi_SecureFileDatabase _this, LPCVOID lpBuffer, DWORD nNumberOfBytes)
{
	LOCALDATA* ldata = (LOCALDATA*)_this->data;
	DWORD dwKey = ldata->dwCryptKey;
	DWORD* pdwBuffer = (DWORD*)lpBuffer;
	DWORD i;
	for (i=0; i<(nNumberOfBytes/4);i++)
	{
		pdwBuffer[i] ^= dwKey;
		dwKey += pdwBuffer[i];
		dwKey ^= pdwBuffer[i];
	}
}

void __inline DecryptBuffer(hi_SecureFileDatabase _this, LPCVOID lpBuffer, DWORD nNumberOfBytes)
{
	LOCALDATA* ldata = (LOCALDATA*)_this->data;
	DWORD dwKey = ldata->dwCryptKey;
	DWORD* pdwBuffer = (DWORD*)lpBuffer;
	DWORD i;
	DWORD dwKeyNew;
	for (i=0; i<(nNumberOfBytes/4);i++)
	{
		dwKeyNew = dwKey;
		dwKeyNew += pdwBuffer[i];
		dwKeyNew ^= pdwBuffer[i];
		pdwBuffer[i] ^= dwKey;
		dwKey = dwKeyNew;
	}
	
}

BOOL WriteBuffEncrypted(hi_SecureFileDatabase _this, LPBYTE lpOut, LPCVOID lpBuffer, DWORD nNumberOfBytesToWrite, LPDWORD lpNumberOfBytesWritten)
{
	BOOL bResult = cTRUE;
#if !defined (__unix__)		
	__TRY {
#endif
		memcpy(lpOut, lpBuffer, nNumberOfBytesToWrite);
		if (lpNumberOfBytesWritten)
			*lpNumberOfBytesWritten = nNumberOfBytesToWrite;
		CryptBuffer(_this, lpOut, nNumberOfBytesToWrite);
		AjustCryptKey(_this, lpBuffer, nNumberOfBytesToWrite);
#if !defined (__unix__)	
	}
	__EXCEPT (EXCEPTION_EXECUTE_HANDLER) {
		PR_TRACE(( _this, prtERROR, "sfdb\tSave: exception handled 0x%08X", GetExceptionCode()));
		CloseMappedFile(_this);
		bResult = cFALSE;
	}
#endif
	return bResult;
}

BOOL WriteFileEncrypted(hi_SecureFileDatabase _this, HANDLE hFile, LPCVOID lpBuffer, DWORD nNumberOfBytesToWrite, LPDWORD lpNumberOfBytesWritten, LPOVERLAPPED lpOverlapped)
{
	BOOL bResult;
	LOCALDATA* ldata = (LOCALDATA*)_this->data;
#if !defined (__unix__)		
	__TRY {
#endif
		CryptBuffer(_this, lpBuffer, nNumberOfBytesToWrite);
		bResult = WriteFile(hFile, lpBuffer, nNumberOfBytesToWrite, lpNumberOfBytesWritten, lpOverlapped);
		DecryptBuffer(_this, lpBuffer, nNumberOfBytesToWrite);
		AjustCryptKey(_this, lpBuffer, nNumberOfBytesToWrite);
#if !defined (__unix__)	
	}
	__EXCEPT (EXCEPTION_EXECUTE_HANDLER) {
		PR_TRACE(( _this, prtERROR, "sfdb\tSave: exception handled 0x%08X", GetExceptionCode()));
		CloseMappedFile(_this);
		bResult = cFALSE;
	}
#endif
	return bResult;
}

BOOL ReadFileEncrypted(hi_SecureFileDatabase _this, HANDLE hFile, LPVOID lpBuffer, DWORD nNumberOfBytesToRead, LPDWORD lpNumberOfBytesRead, LPOVERLAPPED lpOverlapped)
{
	BOOL bResult;
	DWORD dwKeyDiff = 0;
#if !defined (__unix__)		
	__TRY {
#endif
		bResult = ReadFile(hFile, lpBuffer, nNumberOfBytesToRead, lpNumberOfBytesRead, lpOverlapped);
		DecryptBuffer(_this, lpBuffer, nNumberOfBytesToRead);
		AjustCryptKey(_this, lpBuffer, nNumberOfBytesToRead);
#if !defined (__unix__)	
	}
	__EXCEPT (EXCEPTION_EXECUTE_HANDLER) {
		PR_TRACE(( _this, prtERROR, "sfdb\tSave: exception handled 0x%08X", GetExceptionCode()));
		*lpNumberOfBytesRead = 0;
		bResult = cFALSE;
	}
#endif
	return bResult;
}



// AVP Prague stamp begin( External (user called) interface method implementation, Load )
// Parameters are:
tERROR pr_call SecureFileDatabase_Load( hi_SecureFileDatabase _this, tCHAR* szFileName )
{
	tERROR error = errOK;
	HANDLE hFile = INVALID_HANDLE_VALUE;
	tDWORD dwClients = 0;
	LOCALDATA* ldata = (LOCALDATA*)(_this->data);
	
	PR_TRACE_A0( _this, "Enter SecureFileDatabase::Load method" );
	
	if (PR_FAIL(error = WRITING_LOCK_EX(MUTEX_LONG_TIMEOUT)))
	{
		PR_TRACE((_this, prtERROR, "sfdb\tSyncronisation failed with %terr", error));
		return error;
	}
#if !defined (__unix__)	
	__TRY_F {
		__TRY {
#endif
			if (strlen(szFileName) > sizeof(ldata->szSFDBPathName)+1)
			{
				PR_TRACE(( _this, prtERROR, "sfdb\tLoad: filename is too long"));
#if defined (__unix__)	
				RELEASE ();
#endif
				return errUNEXPECTED;
			}
			strcpy(ldata->szSFDBPathName, szFileName);
			
			if (ldata->pDB)
				dwClients = ldata->pDB->dwActiveClients;
			
			CloseMappedFile(_this);
			
#ifdef __JUST_COPY__
			if (!CopyFile(szFileName, ldata->szTempFile, FALSE)){
				PR_TRACE(( _this, prtERROR, "sfdb\tLoad: CopyFile failed with error=0x%08X", GetLastError()));
#if defined (__unix__)	
				RELEASE ();
#endif
				return errUNEXPECTED;
			}
			
			if (PR_FAIL(error = OpenMappedFile(_this, ldata->szTempFile))){
				PR_TRACE(( _this, prtERROR, "sfdb\tLoad: OpenMappedFile failed with error=0x%08X", error));
			}
#else
			{
				DWORD dwCluster;
				SFDB_CLUSTER_HDR* pCluster;
				tBYTE* pRecord;
				SFDB_HDR sDBHdr;
				tDWORD dwRecord;
				tDWORD dwBytes;
				tDWORD dwDataSize;
				SFDB_RECORD_SESSION_DATA* pRecordSessionData;
				SFDB_HDR* pDB;
				
				if (PR_SUCC(error = OpenMappedFile(_this))){
					PR_TRACE(( _this, prtNOTIFY, "sfdb\tLoad: connected to existing MMF, clients: %d", ldata->pDB->dwActiveClients));
#if defined (__unix__)	
					RELEASE ();
#endif
					return error;
				}
				
				hFile = CreateFile(szFileName, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, 0, NULL);
				if (hFile == INVALID_HANDLE_VALUE){
					PR_TRACE(( _this, prtERROR, "sfdb\tLoad: CreateFile failed with error=0x%08X", GetLastError()));
#if defined (__unix__)	
					RELEASE ();
#endif
					return errUNEXPECTED;
				}
				
				ReadFile(hFile, &sDBHdr, sizeof(SFDB_HDR), &dwBytes, NULL);
				if (sizeof(SFDB_HDR) != dwBytes){
					PR_TRACE(( _this, prtERROR, "sfdb\tLoad: WriteFile failed with error=0x%08X", GetLastError()));
#if defined (__unix__)	
					RELEASE ();
#endif
					return errUNEXPECTED;
				}
				
				if (sDBHdr.dwSignature != SFDB_SIGNATURE){
					PR_TRACE(( _this, prtERROR, "sfdb\tLoad: File signature not recognized. Wrong file name?"));
#if defined (__unix__)	
					RELEASE ();
#endif
					return errNOT_MATCHED;
				}
				if (sDBHdr.vDBVersion != 0x00010000){
					PR_TRACE(( _this, prtERROR, "sfdb\tLoad: Database version is not supported."));
#if defined (__unix__)	
					RELEASE ();
#endif
					return errNOT_SUPPORTED;
				}
				
				error = CreateMappedFile(_this, &sDBHdr);
				if (PR_FAIL(error)){
					PR_TRACE(( _this, prtERROR, "sfdb\tLoad: CreateMappedFile failed with error=0x%08X", error));
#if defined (__unix__)	
					RELEASE ();
#endif
					return errUNEXPECTED;
				}
				
				pDB = ldata->pDB;
				
				ldata->dwCryptKey = *(DWORD*)&pDB->byFileCheckSum;
				//dwDataSize = pDB->dwRecordIDSize + pDB->dwRecordDataSize;
				dwDataSize = pDB->dwRecordSize - sizeof(SFDB_RECORD_SESSION_DATA);
				
				for (dwCluster=0; dwCluster<pDB->dwClusters; dwCluster++){
					pCluster = (SFDB_CLUSTER_HDR*)((tBYTE*)ldata->pMappingView + pDB->dwHeaderSize + dwCluster * pDB->dwClusterSize);
					if (pDB->dwSerializeOptions & cSFDB_SERIALIZE_CRYPT)
						ReadFileEncrypted(_this, hFile, pCluster, sizeof(SFDB_CLUSTER_HDR), &dwBytes, NULL);
					else
						ReadFile(hFile, pCluster, sizeof(SFDB_CLUSTER_HDR), &dwBytes, NULL);
					
					PR_TRACE(( _this, prtNOT_IMPORTANT, "sfdb\tLoad: loading Cluster %d %08X %d", dwCluster, pCluster, pCluster->dwRecords));
					
					if (sizeof(SFDB_CLUSTER_HDR) != dwBytes){
						PR_TRACE(( _this, prtERROR, "sfdb\tLoad: ReadFile failed with error=0x%08X", GetLastError()));
#if defined (__unix__)	
						RELEASE ();
#endif
						return errUNEXPECTED;
					}
					
					pRecord = (tBYTE*)(pCluster+1);
					for (dwRecord=0; dwRecord<pCluster->dwRecords; dwRecord++){
						if (pDB->dwSerializeOptions & cSFDB_SERIALIZE_CRYPT)
							ReadFileEncrypted(_this, hFile, pRecord, dwDataSize, &dwBytes, NULL);
						else
							ReadFile(hFile, pRecord, dwDataSize, &dwBytes, NULL);
						
						PR_TRACE(( _this, prtNOT_IMPORTANT, "sfdb\tLoad: loading Record %d %08X", dwRecord, pRecord));
						
						if (dwBytes != dwDataSize){
							PR_TRACE(( _this, prtERROR, "sfdb\tLoad: ReadFile failed with error=0x%08X", GetLastError()));
#if defined (__unix__)	
							RELEASE ();
#endif
							return errUNEXPECTED;
						}
						pRecordSessionData = (SFDB_RECORD_SESSION_DATA*)(pRecord + pDB->dwRecordSize - sizeof(SFDB_RECORD_SESSION_DATA));
						memset(pRecordSessionData, 0, sizeof(SFDB_RECORD_SESSION_DATA));
						pRecord += pDB->dwRecordSize;
					}
				}
				
				if (pDB->dwSerializeOptions & cSFDB_SERIALIZE_CHECKSUM)
				{
					BYTE byCheckSum[0x10];
					DWORD dwBytes;
					tBYTE sMD5Ctx[0x80];
					tBOOL bRes;
					
					memcpy(byCheckSum, pDB->byFileCheckSum, sizeof(byCheckSum));
					memset(pDB->byFileCheckSum, 0, sizeof(pDB->byFileCheckSum));
					
					if (!hash_api_md5.Init(0, &sMD5Ctx, sizeof(sMD5Ctx), 16, NULL)) {
#if defined (__unix__)	
						RELEASE ();
#endif
						return errUNEXPECTED;
					}
					dwClients = pDB->dwActiveClients;
					pDB->dwActiveClients = 0;
					bRes = hash_api_md5.Update(&sMD5Ctx, pDB, pDB->dwHeaderSize);
					pDB->dwActiveClients = dwClients;
					if (!bRes) {
#if defined (__unix__)	
						RELEASE ();
#endif
						return errUNEXPECTED;
					}
					for (dwCluster=0; dwCluster<pDB->dwClusters; dwCluster++)
					{
						pCluster = (SFDB_CLUSTER_HDR*)((tBYTE*)ldata->pMappingView + pDB->dwHeaderSize + dwCluster * pDB->dwClusterSize);
						PR_TRACE(( _this, prtNOT_IMPORTANT, "sfdb\tLoad: checking Cluster %d %08X %d", dwCluster, pCluster, pCluster->dwRecords));
						pRecord = (tBYTE*)(pCluster+1);
						for (dwRecord=0; dwRecord<pCluster->dwRecords; dwRecord++)
						{
							PR_TRACE(( _this, prtNOT_IMPORTANT, "sfdb\tLoad: checking Record %d %08X", dwRecord, pRecord));
							if (!hash_api_md5.Update(&sMD5Ctx, pRecord, dwDataSize)) {
#if defined (__unix__)	
								RELEASE ();
#endif
								return errUNEXPECTED;
							}
							pRecord += pDB->dwRecordSize;
						}
					}
					
					// uninitialized variable - fixed by Mike 07.10.2002
					dwBytes = sizeof(pDB->byFileCheckSum);
					
					if (!hash_api_md5.Finalize(&sMD5Ctx, (unsigned long*)&dwBytes, pDB->byFileCheckSum))
					{
						PR_TRACE(( _this, prtERROR, "sfdb\tLoad: CheckSum hash_api_md5.Finalize failed!"));
#if defined (__unix__)	
						RELEASE ();
#endif
						return errUNEXPECTED;
					}
					else if (memcmp(byCheckSum, pDB->byFileCheckSum, sizeof(byCheckSum)) != 0)
					{
						PR_TRACE(( _this, prtERROR, "sfdb\tLoad: CheckSum not match was %08X%08X%08X%08X now %08X%08X%08X%08X", *((tQWORD*)byCheckSum), *(((tQWORD*)byCheckSum)+1), *((tQWORD*)pDB->byFileCheckSum), *(((tQWORD*)pDB->byFileCheckSum)+1)));
#if defined (__unix__)	
						RELEASE ();
#endif
						return errUNEXPECTED;
					}
				}
				
				CloseHandle(hFile);
				hFile = INVALID_HANDLE_VALUE;
			}
			
#endif
#if !defined (__unix__)				
		}
		__EXCEPT (EXCEPTION_EXECUTE_HANDLER) {
			PR_TRACE(( _this, prtERROR, "sfdb\tLoad: exception handled 0x%08X", GetExceptionCode()));
			error = errUNEXPECTED;
		}
	}
	__FINALLY {
		if (hFile != INVALID_HANDLE_VALUE)
			CloseHandle(hFile);
#endif
		RELEASE();
#if !defined (__unix__)	
	}
#endif	
	PR_TRACE_A1( _this, "Leave SecureFileDatabase::Load method, ret tERROR = 0x%08x", error );
	return error;
}
// AVP Prague stamp end



// AVP Prague stamp begin( External (user called) interface method implementation, Save )
// Parameters are:
tERROR pr_call SecureFileDatabase_Save( hi_SecureFileDatabase _this, tCHAR* szFileName )
{
	tERROR error = errOK;
	HANDLE hFile = INVALID_HANDLE_VALUE;
	tDWORD dwClients;
	LOCALDATA* ldata = (LOCALDATA*)(_this->data);
	
	PR_TRACE_A0( _this, "Enter SecureFileDatabase::Save method" );
	
	if (PR_FAIL(error = WRITING_LOCK_EX(MUTEX_LONG_TIMEOUT)))
	{
		PR_TRACE((_this, prtERROR, "sfdb\tSyncronisation failed with %terr", error));
		return error;
	}
#if !defined (__unix__)	
	
	__TRY_F {
		__TRY {
#endif
			
			if (ldata->pDB == NULL || ldata->pMappingView == NULL){
				PR_TRACE(( _this, prtERROR, "sfdb\tSave: database not initialized"));
#if defined (__unix__)	
				if (hFile != INVALID_HANDLE_VALUE)
					CloseHandle(hFile);
				RELEASE();
#endif
				return errOBJECT_NOT_INITIALIZED;
			}
			
			// sync map view size
			if (ldata->dwMapViewSize != ldata->pDB->dwFileSize)
			{
				if (PR_FAIL(error = RemapViewOfFile(_this, ldata->pDB->dwFileSize)))
				{
					PR_TRACE(( _this, prtERROR, "sfdb\tRemapViewOfFile failed with %terr", error));
#if defined (__unix__)	
					if (hFile != INVALID_HANDLE_VALUE)
						CloseHandle(hFile);
					RELEASE();
#endif
					return error;
				}
			}
			
			if (ldata->dwSerializeOptions & cSFDB_SERIALIZE_COMPACT_ON_SAVE)
				CompactDatabase(_this);
			
			if (!FlushViewOfFile(ldata->pMappingView, 0)){
				PR_TRACE(( _this, prtERROR, "sfdb\tSave: FlushViewOfFile failed with error=0x%08X", GetLastError()));
#if defined (__unix__)	
				if (hFile != INVALID_HANDLE_VALUE)
					CloseHandle(hFile);
				RELEASE();
#endif
				return errUNEXPECTED;
			}
			
#ifdef __JUST_COPY__
			if (!CopyFile(ldata->szTempFile, szFileName, FALSE)){
				PR_TRACE(( _this, prtERROR, "sfdb\tSave: CopyFile failed with error=0x%08X", GetLastError()));
#if defined (__unix__)	
				if (hFile != INVALID_HANDLE_VALUE)
					CloseHandle(hFile);
				RELEASE();
#endif
				return errUNEXPECTED;
			}
#else
			{
				DWORD dwCluster;
				SFDB_CLUSTER_HDR* pCluster;
				tBYTE* pRecord;
				SFDB_HDR* pDB = ldata->pDB;
				tDWORD dwRecord;
				tDWORD dwBytes;
				tDWORD dwDataSize;
				SFDB_RECORD_SESSION_DATA* pRecordSessionData;
				tBYTE sMD5Ctx[0x80];
				{
					// Fix bug #4832 & #8610
					DWORD dwAttributes = GetFileAttributes(szFileName);
					if ((dwAttributes != (DWORD)-1) && (dwAttributes & (FILE_ATTRIBUTE_SYSTEM | FILE_ATTRIBUTE_READONLY | FILE_ATTRIBUTE_HIDDEN )))
						SetFileAttributes(szFileName, dwAttributes & (~(FILE_ATTRIBUTE_SYSTEM | FILE_ATTRIBUTE_READONLY | FILE_ATTRIBUTE_HIDDEN )));
				}
				hFile = CreateFile(szFileName, GENERIC_WRITE, FILE_SHARE_READ, NULL, CREATE_ALWAYS, 0, NULL);
				if (hFile == INVALID_HANDLE_VALUE){
					PR_TRACE(( _this, prtERROR, "sfdb\tSave: CreateFile failed with error=0x%08X", GetLastError()));
#if defined (__unix__)	
					if (hFile != INVALID_HANDLE_VALUE)
						CloseHandle(hFile);
					RELEASE();
#endif
					return errUNEXPECTED;
				}
				
				//dwDataSize = pDB->dwRecordIDSize + pDB->dwRecordDataSize;
				dwDataSize = pDB->dwRecordSize - sizeof(SFDB_RECORD_SESSION_DATA);
				
				pDB->dwSerializeOptions = ldata->dwSerializeOptions;
				
				// cleanup garbage
				for (dwCluster=0; dwCluster<pDB->dwClusters; dwCluster++){
					pCluster = (SFDB_CLUSTER_HDR*)((tBYTE*)ldata->pMappingView + pDB->dwHeaderSize + dwCluster * pDB->dwClusterSize);
					pRecord = (tBYTE*)(pCluster+1);
					if (pCluster->dwRecords > pDB->dwRecordsPerCluster) // validation
					{
						PR_TRACE(( _this, prtERROR, "sfdb\tSave: Cluster %d contain %d records? (RecordsPerCluster=%d)", dwCluster, pCluster->dwRecords, pDB->dwRecordsPerCluster));
#if defined (__unix__)	
						if (hFile != INVALID_HANDLE_VALUE)
							CloseHandle(hFile);
						RELEASE();
#endif
						return errUNEXPECTED;
					}
					for (dwRecord=0; dwRecord<pCluster->dwRecords; dwRecord++){
						pRecordSessionData = (SFDB_RECORD_SESSION_DATA*)(pRecord + pDB->dwRecordSize - sizeof(SFDB_RECORD_SESSION_DATA));
						memset(pRecordSessionData, 0, sizeof(SFDB_RECORD_SESSION_DATA));
						pRecord += pDB->dwRecordSize;
					}
				}
				memset(pDB->byFileCheckSum, 0, sizeof(pDB->byFileCheckSum));
				
				// calculate database checksum
				if (ldata->dwSerializeOptions & cSFDB_SERIALIZE_CHECKSUM)
				{
					tBOOL bRes;
					if (!hash_api_md5.Init(0, &sMD5Ctx, sizeof(sMD5Ctx), 16, NULL)) {
#if defined (__unix__)	
						if (hFile != INVALID_HANDLE_VALUE)
							CloseHandle(hFile);
						RELEASE();
#endif
						return errUNEXPECTED;
					}
					dwClients = ldata->pDB->dwActiveClients;
					pDB->dwActiveClients = 0;
					bRes = hash_api_md5.Update(&sMD5Ctx, pDB, pDB->dwHeaderSize);
					ldata->pDB->dwActiveClients = dwClients;
					if (!bRes) {
#if defined (__unix__)	
						if (hFile != INVALID_HANDLE_VALUE)
							CloseHandle(hFile);
						RELEASE();
#endif
						return errUNEXPECTED;
					}
					for (dwCluster=0; dwCluster<pDB->dwClusters; dwCluster++)
					{
						pCluster = (SFDB_CLUSTER_HDR*)((tBYTE*)ldata->pMappingView + pDB->dwHeaderSize + dwCluster * pDB->dwClusterSize);
						pRecord = (tBYTE*)(pCluster+1);
						for (dwRecord=0; dwRecord<pCluster->dwRecords; dwRecord++)
						{
							if (!hash_api_md5.Update(&sMD5Ctx, pRecord, dwDataSize)) {
#if defined (__unix__)	
								if (hFile != INVALID_HANDLE_VALUE)
									CloseHandle(hFile);
								RELEASE();
#endif
								return errUNEXPECTED;
							}
							pRecord += pDB->dwRecordSize;
						}
					}
					
					// uninitialized variable - fixed by Mike 07.10.2002
					dwBytes = sizeof(pDB->byFileCheckSum);
					
					if (!hash_api_md5.Finalize(&sMD5Ctx, (unsigned long*)&dwBytes, pDB->byFileCheckSum))
					{
						PR_TRACE(( _this, prtERROR, "sfdb\tSave: CheckSum failed"));
#if defined (__unix__)	
						if (hFile != INVALID_HANDLE_VALUE)
							CloseHandle(hFile);
						RELEASE();
#endif
						return errUNEXPECTED;
					}
				}
				
				// init crypt key
				if (ldata->dwSerializeOptions & cSFDB_SERIALIZE_CRYPT)
					ldata->dwCryptKey = *(DWORD*)&pDB->byFileCheckSum;
				
				// save database header
				WriteFile(hFile, pDB, sizeof(SFDB_HDR), &dwBytes, NULL);
				if (sizeof(SFDB_HDR) != dwBytes){
					PR_TRACE(( _this, prtERROR, "sfdb\tSave: WriteFile failed with error=0x%08X", GetLastError()));
#if defined (__unix__)	
					if (hFile != INVALID_HANDLE_VALUE)
						CloseHandle(hFile);
					RELEASE();
#endif
					return errUNEXPECTED;
				}
				
				{
					// Mike, 31.08.2004
					// saving record-by-record was too slow
					// optimized with cluster saving
					
					tBYTE* pClusterSaveDataBuff = NULL;
					tBYTE* pClusterSaveDataPtr;
					if (PR_FAIL(CALL_SYS_ObjHeapAlloc(_this, &pClusterSaveDataBuff, pDB->dwClusterSize)))
					{
#if defined (__unix__)	
						if (hFile != INVALID_HANDLE_VALUE)
							CloseHandle(hFile);
						RELEASE();
#endif
						return errNOT_ENOUGH_MEMORY;
					}
					
					// save clusters
					for (dwCluster=0; dwCluster<pDB->dwClusters; dwCluster++)
					{
						pCluster = (SFDB_CLUSTER_HDR*)((tBYTE*)ldata->pMappingView + pDB->dwHeaderSize + dwCluster * pDB->dwClusterSize);
						if (ldata->dwSerializeOptions & cSFDB_SERIALIZE_CRYPT)
							WriteFileEncrypted(_this, hFile, pCluster, sizeof(SFDB_CLUSTER_HDR), &dwBytes, NULL);
						else
							WriteFile(hFile, pCluster, sizeof(SFDB_CLUSTER_HDR), &dwBytes, NULL);
						if (sizeof(SFDB_CLUSTER_HDR) != dwBytes){
							PR_TRACE(( _this, prtERROR, "sfdb\tSave: WriteFile failed with error=0x%08X", GetLastError()));
#if defined (__unix__)	
							if (hFile != INVALID_HANDLE_VALUE)
								CloseHandle(hFile);
							RELEASE();
#endif
							return errUNEXPECTED;
						}
						
						pClusterSaveDataPtr = pClusterSaveDataBuff;

						// save records (in cluster)
						pRecord = (tBYTE*)(pCluster+1);
						for (dwRecord=0; dwRecord<pCluster->dwRecords; dwRecord++)
						{
							if (ldata->dwSerializeOptions & cSFDB_SERIALIZE_CRYPT)
							{
								WriteBuffEncrypted(_this, pClusterSaveDataPtr, pRecord, dwDataSize, &dwBytes);
							}
							else
							{
								memcpy(pClusterSaveDataPtr, pRecord, dwDataSize);
								dwBytes = dwDataSize;
							}
							pClusterSaveDataPtr += dwBytes;
							if (dwBytes != dwDataSize){
								PR_TRACE(( _this, prtERROR, "sfdb\tSave: WriteFile failed with error=0x%08X", GetLastError()));
#if defined (__unix__)	
								if (hFile != INVALID_HANDLE_VALUE)
									CloseHandle(hFile);
								RELEASE();
#endif
								return errUNEXPECTED;
							}
							pRecord += pDB->dwRecordSize;
						}
						WriteFile(hFile, pClusterSaveDataBuff, (tDWORD)(pClusterSaveDataPtr-pClusterSaveDataBuff), &dwBytes, NULL);
						if (dwBytes != (tDWORD)(pClusterSaveDataPtr-pClusterSaveDataBuff)){
							PR_TRACE(( _this, prtERROR, "sfdb\tSave: WriteFile failed with error=0x%08X", GetLastError()));
#if defined (__unix__)	
							if (hFile != INVALID_HANDLE_VALUE)
								CloseHandle(hFile);
							RELEASE();
#endif
							return errUNEXPECTED;
						}
					}
					CALL_SYS_ObjHeapFree(_this, pClusterSaveDataBuff);
				}
				
				//FlushFileBuffers(hFile); // Mike, 31.08.2004 - this slow down saving process
				CloseHandle(hFile);
				hFile = INVALID_HANDLE_VALUE;
			}
#endif
#if !defined (__unix__)	
		}
		__EXCEPT (EXCEPTION_EXECUTE_HANDLER) {
			PR_TRACE(( _this, prtERROR, "sfdb\tSave: exception handled 0x%08X", GetExceptionCode()));
			error = errUNEXPECTED;
		}
	}
	__FINALLY {
#endif
		if (hFile != INVALID_HANDLE_VALUE)
			CloseHandle(hFile);
		RELEASE();
#if !defined (__unix__)	
	}
#endif	
	PR_TRACE_A1( _this, "Leave SecureFileDatabase::Save method, ret tERROR = 0x%08x", error );
	return error;
}
// AVP Prague stamp end



// AVP Prague stamp begin( External (user called) interface method implementation, GetRecord )
// Extended method comment
//  Метод находит в базе данных запись по указанному идентификатору и возвращает данные, хранимые в ней. Размер данных можно получить через GetDBInfo.
// Parameters are:
tERROR pr_call SecureFileDatabase_GetRecord( hi_SecureFileDatabase _this, tVOID* pRecordID, tVOID* pRecordData )
{
	tERROR error = errOK;
	LOCALDATA* ldata = (LOCALDATA*)(_this->data);
	SFDB_HDR* pDB;
	tDWORD dwIndex;
	SFDB_CLUSTER_HDR* pCluster;
	tBYTE* pRecord;
	SFDB_RECORD_SESSION_DATA* pRecordSessionData;
	
	PR_TRACE_A0( _this, "Enter SecureFileDatabase::GetRecord method" );
	
	if (PR_FAIL(error = READING_LOCK()))
	{
		PR_TRACE((_this, prtERROR, "sfdb\tSyncronisation failed with %terr", error));
		return error;
	}
#if !defined (__unix__)	
	__TRY_F {
		__TRY {
#endif
			
			PR_TRACE(( _this, prtNOT_IMPORTANT, "sfdb\tGetRecord"));
			if (ldata->pDB == NULL){
				PR_TRACE(( _this, prtERROR, "sfdb\tGetRecord: database not initialized"));
#if defined (__unix__)		
				RELEASE ();
#endif
				return errOBJECT_NOT_INITIALIZED;
			}
			
			// sync map view size
			if (ldata->dwMapViewSize != ldata->pDB->dwFileSize)
			{
				if (PR_FAIL(error = RemapViewOfFile(_this, ldata->pDB->dwFileSize)))
				{
					PR_TRACE(( _this, prtERROR, "sfdb\tRemapViewOfFile failed with %terr", error));
#if defined (__unix__)		
					RELEASE ();
#endif
					
					return error;
				}
			}
			
			pDB = ldata->pDB;
			dwIndex = ldata->GetIndex(ldata, pRecordID);
			PR_TRACE(( _this, prtNOT_IMPORTANT, "sfdb\tGetRecord: dwIndex=%d", dwIndex));
			pCluster = (SFDB_CLUSTER_HDR*)((BYTE*)ldata->pMappingView + pDB->dwHeaderSize + dwIndex * pDB->dwClusterSize);
			PR_TRACE(( _this, prtNOT_IMPORTANT, "sfdb\tGetRecord: pCluster=%08X RecordsInCluster=%d", pCluster, pCluster->dwRecords));
			PR_TRACE(( _this, prtNOT_IMPORTANT, "sfdb\tGetRecord: bsearch(%08X, %08X, %08X, %08X, %08X)", pRecordID, pCluster+1, pCluster->dwRecords, pDB->dwRecordSize, ldata->RecordCompareFunc));
			pRecord = (tBYTE*)bsearch(pRecordID, pCluster+1, pCluster->dwRecords, pDB->dwRecordSize, ldata->RecordCompareFunc);
			PR_TRACE(( _this, prtNOT_IMPORTANT, "sfdb\tGetRecord: pRecord=%08X", pRecord));
			if (pRecord == NULL){
				PR_TRACE(( _this, prtNOT_IMPORTANT, "sfdb\tGetRecord: record not found"));
#if defined (__unix__)		
				RELEASE ();
#endif
				
				return errNOT_FOUND;
			}
			
			if (pRecordData != NULL)
				memcpy(pRecordData, pRecord + pDB->dwRecordIDSize, pDB->dwRecordDataSize);
			
			pRecordSessionData = (SFDB_RECORD_SESSION_DATA*)(pRecord + pDB->dwRecordSize - sizeof(SFDB_RECORD_INTERNAL_DATA));
			pRecordSessionData->dwUsageCount++;
			
			// set last used date timestamp
			{
				tDWORD dwCurrentDate;
				SFDB_RECORD_INTERNAL_DATA* pRecordInternalData = (SFDB_RECORD_INTERNAL_DATA*)pRecordSessionData - 1;
				GetCurrentDateTime(_this, &dwCurrentDate);
				pRecordInternalData->dwLastUsedDate = dwCurrentDate;
			}
#if !defined (__unix__)	
		}
		__EXCEPT (EXCEPTION_EXECUTE_HANDLER) {
			PR_TRACE(( _this, prtERROR, "sfdb\tGetRecord: exception handled 0x%08X", GetExceptionCode()));
			error = errUNEXPECTED;
		}
	}
	__FINALLY {
#endif
		RELEASE();
#if !defined (__unix__)	
	}
#endif
	PR_TRACE_A1( _this, "Leave SecureFileDatabase::GetRecord method, ret tERROR = 0x%08x", error );
	return error;
}
// AVP Prague stamp end



// AVP Prague stamp begin( External (user called) interface method implementation, UpdateRecord )
// Behaviour comment
//  Если запись с указанным идентификатором в базе отсутствовала, метод добавит новую запись.
// Parameters are:
// "pRecordData" : Данные сохраняются в базе данных в указанной записи
tERROR pr_call SecureFileDatabase_UpdateRecord( hi_SecureFileDatabase _this, tVOID* pRecordID, tVOID* pRecordData )
{
	tERROR error = errOK;
	LOCALDATA* ldata = (LOCALDATA*)(_this->data);
	SFDB_HDR* pDB;
	tDWORD dwIndex;
	SFDB_CLUSTER_HDR* pCluster;
	tBYTE* pRecord;
	tDWORD i;
	SFDB_RECORD_SESSION_DATA* pRecordSessionData;
	
	PR_TRACE_A0( _this, "Enter SecureFileDatabase::UpdateRecord method" );
	
	PR_TRACE((_this, prtNOTIFY, "sfdb\tUpdateRecord %016I64X", *(tQWORD*)pRecordID));
	
	if (PR_FAIL(error = WRITING_LOCK()))
	{
		PR_TRACE((_this, prtERROR, "sfdb\tSyncronisation failed with %terr", error));
		return error;
	}
#if !defined (__unix__)		
	__TRY_F {
		__TRY {
#endif
			
			if (ldata->pDB == NULL){
				PR_TRACE(( _this, prtERROR, "sfdb\tUpdateRecord: database not initialized"));
#if defined (__unix__)		
				RELEASE ();
#endif
				
				return errOBJECT_NOT_INITIALIZED;
			}
			
			// sync map view size
			if (ldata->dwMapViewSize != ldata->pDB->dwFileSize)
			{
				if (PR_FAIL(error = RemapViewOfFile(_this, ldata->pDB->dwFileSize)))
				{
					PR_TRACE(( _this, prtERROR, "sfdb\tRemapViewOfFile failed with %terr", error));
#if defined (__unix__)		
					RELEASE ();
#endif
					
					return error;
				}
			}
			
			pDB = ldata->pDB;
			dwIndex = ldata->GetIndex(ldata, pRecordID);
			pCluster = (SFDB_CLUSTER_HDR*)((BYTE*)ldata->pMappingView + pDB->dwHeaderSize + dwIndex * pDB->dwClusterSize);
			pRecord = (tBYTE*)bsearch(pRecordID, pCluster+1, pCluster->dwRecords, pDB->dwRecordSize, ldata->RecordCompareFunc);
			if (pRecord == NULL){
				// add new record
				if (pCluster->dwRecords == pDB->dwRecordsPerCluster)
				{
					//cluster is full
					while (pCluster->dwRecords == pDB->dwRecordsPerCluster){
						if (PR_FAIL(error = ExpandDatabase(_this)))
							break;
						PR_TRACE((_this, prtFATAL, "sfdb\tExpandDatabase returned %terr", error));
						pDB = ldata->pDB;
						pCluster = (SFDB_CLUSTER_HDR*)((BYTE*)ldata->pMappingView + pDB->dwHeaderSize + ((ldata->GetIndex(ldata, pRecordID)) * pDB->dwClusterSize));
					}
					if (error == errOUT_OF_SPACE)
						error = DeleteOldestRecordsInCluster(_this, pCluster);
					if (PR_FAIL(error))
					{
#if defined (__unix__)		
						RELEASE ();
#endif
						return error;
					}
				}
				pRecord = (tBYTE*)pCluster+sizeof(SFDB_CLUSTER_HDR);
				for (i=0; i<pCluster->dwRecords; i++){
					if (ldata->RecordCompareFunc(pRecord, pRecordID) > 0){
						// shift records down
						memmove(pRecord+pDB->dwRecordSize, pRecord, (pCluster->dwRecords - i) * pDB->dwRecordSize);
						break;
					}
					pRecord+=pDB->dwRecordSize;
				}
				pRecordSessionData = (SFDB_RECORD_SESSION_DATA*)(pRecord + pDB->dwRecordSize - sizeof(SFDB_RECORD_SESSION_DATA));
				memcpy(pRecord, pRecordID, pDB->dwRecordIDSize);
				memset(pRecordSessionData, 0, sizeof(SFDB_RECORD_SESSION_DATA));
				PR_TRACE((_this, prtNOT_IMPORTANT, "sfdb\tUpdateRecord %016I64X - added", *(tQWORD*)pRecord));
				pCluster->dwRecords++;
			}
			else{
				error = warnALREADY_EXIST; // already exist
				pRecordSessionData = (SFDB_RECORD_SESSION_DATA*)(pRecord + pDB->dwRecordSize - sizeof(SFDB_RECORD_SESSION_DATA));
				PR_TRACE((_this, prtNOT_IMPORTANT, "sfdb\tUpdateRecord %016I64X - updated", *(tQWORD*)pRecord));
			}
			memcpy(pRecord + pDB->dwRecordIDSize, pRecordData, pDB->dwRecordDataSize);
			pRecordSessionData->dwUsageCount++;
			
			// set last used date timestamp
			{
				tDWORD dwCurrentDate;
				SFDB_RECORD_INTERNAL_DATA* pRecordInternalData = (SFDB_RECORD_INTERNAL_DATA*)pRecordSessionData - 1;
				GetCurrentDateTime(_this, &dwCurrentDate);
				pRecordInternalData->dwLastUsedDate = dwCurrentDate;
			}
			
//			while (pCluster->dwRecords == pDB->dwRecordsPerCluster){
//				if (PR_FAIL(ExpandDatabase(_this)))
//					break;
//				PR_TRACE((_this, prtFATAL, "sfdb\tExpandDatabase returned %terr", error));
//				pDB = ldata->pDB;
//				pCluster = (SFDB_CLUSTER_HDR*)((BYTE*)ldata->pMappingView + pDB->dwHeaderSize + ((ldata->GetIndex(ldata, pRecordID)) * pDB->dwClusterSize));
//			}
#if !defined (__unix__)					
		}
		__EXCEPT (EXCEPTION_EXECUTE_HANDLER) {
			PR_TRACE(( _this, prtERROR, "sfdb\tUpdateRecord: exception handled 0x%08X", GetExceptionCode()));
			error = errUNEXPECTED;
		}
	}
	__FINALLY {
#endif
		RELEASE();
#if !defined (__unix__)		
	}
#endif
	
	
	PR_TRACE_A1( _this, "Leave SecureFileDatabase::UpdateRecord method, ret tERROR = 0x%08x", error );
	return error;
}
// AVP Prague stamp end



// AVP Prague stamp begin( External (user called) interface method implementation, DeleteRecord )
// Extended method comment
//  Удаляет указанную запись вместе с ее данными
// Parameters are:
tERROR pr_call SecureFileDatabase_DeleteRecord( hi_SecureFileDatabase _this, tVOID* pRecordID )
{
	tERROR error = errOK;
	LOCALDATA* ldata = (LOCALDATA*)(_this->data);
	SFDB_HDR* pDB;
	tDWORD dwIndex;
	SFDB_CLUSTER_HDR* pCluster;
	tBYTE* pRecord;
	tBYTE* pNextRecord;
	tBYTE* pLastRecord;
	
	PR_TRACE_A0( _this, "Enter SecureFileDatabase::DeleteRecord method" );
	
	if (PR_FAIL(error = WRITING_LOCK()))
	{
		PR_TRACE((_this, prtERROR, "sfdb\tSyncronisation failed with %terr", error));
		return error;
	}
#if !defined (__unix__)		
	__TRY_F {
		__TRY {
#endif			
			if (ldata->pDB == NULL){
				PR_TRACE(( _this, prtERROR, "sfdb\tDeleteRecord: database not initialized"));
#if defined (__unix__)		
				RELEASE ();
#endif
				return errOBJECT_NOT_INITIALIZED;
			}
			
			// sync map view size
			if (ldata->dwMapViewSize != ldata->pDB->dwFileSize)
			{
				if (PR_FAIL(error = RemapViewOfFile(_this, ldata->pDB->dwFileSize)))
				{
					PR_TRACE(( _this, prtERROR, "sfdb\tRemapViewOfFile failed with %terr", error));
#if defined (__unix__)		
					RELEASE ();
#endif
					return error;
					
				}
			}
			
			pDB = ldata->pDB;
			dwIndex = ldata->GetIndex(ldata, pRecordID);
			pCluster = (SFDB_CLUSTER_HDR*)((BYTE*)ldata->pMappingView + pDB->dwHeaderSize + dwIndex * pDB->dwClusterSize);
			pRecord = (tBYTE*)bsearch(pRecordID, (tBYTE*)pCluster+sizeof(SFDB_CLUSTER_HDR), pCluster->dwRecords, pDB->dwRecordSize, ldata->RecordCompareFunc);
			if (pRecord == NULL){
				PR_TRACE((_this, prtIMPORTANT, "sfdb\tDeleteRecord %016I64X - not found", *(tQWORD*)pRecordID));
#if defined (__unix__)		
				RELEASE ();
#endif
				return errNOT_FOUND;
			}
			
			pNextRecord = pRecord + pDB->dwRecordSize;
			pLastRecord = (tBYTE*)pCluster + sizeof(SFDB_CLUSTER_HDR) + pDB->dwRecordSize * pCluster->dwRecords;
			
			// shift records up
			memmove(pRecord, pNextRecord, pLastRecord-pNextRecord);
			pCluster->dwRecords--;
			PR_TRACE((_this, prtNOTIFY, "sfdb\tDeleteRecord %016I64X - deleted", *(tQWORD*)pRecord));
#ifdef _DEBUG
			pLastRecord -= pDB->dwRecordSize;
			memset(pLastRecord, 0, pDB->dwRecordSize);
#endif
#if !defined (__unix__)		
		}
		__EXCEPT (EXCEPTION_EXECUTE_HANDLER) {
			PR_TRACE(( _this, prtERROR, "sfdb\tDeleteRecord: exception handled 0x%08X", GetExceptionCode()));
			error = errUNEXPECTED;
		}
	}
	__FINALLY {
#endif
		RELEASE();
#if !defined (__unix__)		
	}
#endif
	
	PR_TRACE_A1( _this, "Leave SecureFileDatabase::DeleteRecord method, ret tERROR = 0x%08x", error );
	return error;
}
// AVP Prague stamp end



// AVP Prague stamp begin( External (user called) interface method implementation, GetDBInfo )
// Parameters are:
// "pRecordIDAlg"    : Получает алгоритм определения идентификатора записи, использовавшийся при создании базы данных
// "pRecordIDSize"   : Определяется в байтах
// "pRecordDataSize" : Определяется в байтах
tERROR pr_call SecureFileDatabase_GetDBInfo( hi_SecureFileDatabase _this, tDWORD* pRecordIDAlg, tDWORD* pRecordIDSize, tDWORD* pRecordDataSize )
{
	tERROR error = errOK;
	LOCALDATA* ldata = (LOCALDATA*)(_this->data);
	
	PR_TRACE_A0( _this, "Enter SecureFileDatabase::GetDBInfo method" );
	
	if (PR_FAIL(error = READING_LOCK()))
	{
		PR_TRACE((_this, prtERROR, "sfdb\tSyncronisation failed with %terr", error));
		return error;
	}
#if !defined (__unix__)		
	__TRY_F {
		__TRY {
#endif			
			if (ldata->pDB == NULL){
				PR_TRACE(( _this, prtERROR, "sfdb\tGetDBInfo: database not initialized"));
#if defined (__unix__)		
				RELEASE ();
#endif
				
				return errOBJECT_NOT_INITIALIZED;
			}
			
			if (pRecordIDAlg)
				*pRecordIDAlg = ldata->pDB->dwRecordIDAlg;
			
			if (pRecordIDSize)
				*pRecordIDSize = ldata->pDB->dwRecordIDSize;
			
			if (pRecordDataSize)
				*pRecordDataSize = ldata->pDB->dwRecordDataSize;
#if !defined (__unix__)		
			
		}
		__EXCEPT (EXCEPTION_EXECUTE_HANDLER) {
			PR_TRACE(( _this, prtERROR, "sfdb\tGetDBInfo: exception handled 0x%08X", GetExceptionCode()));
			error = errUNEXPECTED;
		}
	}
	__FINALLY {
#endif
		RELEASE();
#if !defined (__unix__)		
	}
#endif	
	PR_TRACE_A1( _this, "Leave SecureFileDatabase::GetDBInfo method, ret tERROR = 0x%08x", error );
	return error;
}
// AVP Prague stamp end



// AVP Prague stamp begin( External (user called) interface method implementation, CleanDB )
// Parameters are:
tERROR pr_call SecureFileDatabase_CleanDB( hi_SecureFileDatabase _this ) {
	tERROR error = errOK;
	LOCALDATA* ldata = (LOCALDATA*)(_this->data);
	SFDB_HDR* pDB;
	SFDB_CLUSTER_HDR* pCluster;

	PR_TRACE_A0( _this, "Enter SecureFileDatabase::CleanDB method" );

	if (PR_FAIL(error = WRITING_LOCK()))
	{
		PR_TRACE((_this, prtERROR, "sfdb\tSyncronisation failed with %terr", error));
		return error;
	}

	pDB = ldata->pDB;
	if (pDB == NULL)
	{
		PR_TRACE(( _this, prtERROR, "sfdb\tDeleteRecord: database not initialized"));
		RELEASE ();
		return errOBJECT_NOT_INITIALIZED;
	}

	pDB->dwClusters = 1;
	pDB->dwIndexBits = 0;
	pDB->dwIndexMask = 0;
	pCluster = (SFDB_CLUSTER_HDR*)((BYTE*)ldata->pMappingView + pDB->dwHeaderSize);
	pCluster->dwRecords = 0;

	RELEASE();

	PR_TRACE_A1( _this, "Leave SecureFileDatabase::CleanDB method, ret %terr", error );
	return error;
}
// AVP Prague stamp end



// AVP Prague stamp begin( External (user called) interface method implementation, EnumRecords )
// Parameters are:
tERROR pr_call SecureFileDatabase_EnumRecords( hi_SecureFileDatabase _this, tDWORD p_nRecordNum, tVOID* p_pRecordID, tVOID* p_pRecordData ) {
	tERROR error = errOK;
	LOCALDATA* ldata = (LOCALDATA*)(_this->data);
	SFDB_HDR* pDB;
	SFDB_CLUSTER_HDR* pCluster;
	tBYTE* pRecord;
	tDWORD dwCluster;
	PR_TRACE_A0( _this, "Enter SecureFileDatabase::EnumRecords method" );

	if (PR_FAIL(error = READING_LOCK()))
	{
		PR_TRACE((_this, prtERROR, "sfdb\tSyncronisation failed with %terr", error));
		return error;
	}
#if !defined (__unix__)	
	__TRY_F {
		__TRY {
#endif
			
			PR_TRACE(( _this, prtNOT_IMPORTANT, "sfdb\tEnumRecords"));
			if (ldata->pDB == NULL){
				PR_TRACE(( _this, prtERROR, "sfdb\tEnumRecords: database not initialized"));
#if defined (__unix__)		
				RELEASE ();
#endif
				return errOBJECT_NOT_INITIALIZED;
			}
			
			// sync map view size
			if (ldata->dwMapViewSize != ldata->pDB->dwFileSize)
			{
				if (PR_FAIL(error = RemapViewOfFile(_this, ldata->pDB->dwFileSize)))
				{
					PR_TRACE(( _this, prtERROR, "sfdb\tRemapViewOfFile failed with %terr", error));
#if defined (__unix__)		
					RELEASE ();
#endif
					
					return error;
				}
			}
			
			pDB = ldata->pDB;
			pRecord = NULL;
			for (dwCluster=0; dwCluster<pDB->dwClusters; dwCluster++){
				pCluster = (SFDB_CLUSTER_HDR*)((tBYTE*)ldata->pMappingView + pDB->dwHeaderSize + dwCluster * pDB->dwClusterSize);
				if (pCluster->dwRecords > pDB->dwRecordsPerCluster) // validation
				{
					PR_TRACE(( _this, prtERROR, "sfdb\tSave: Cluster %d contain %d records? (RecordsPerCluster=%d)", dwCluster, pCluster->dwRecords, pDB->dwRecordsPerCluster));
#if defined (__unix__)	
					if (hFile != INVALID_HANDLE_VALUE)
						CloseHandle(hFile);
					RELEASE();
#endif
					return errUNEXPECTED;
				}
				if (p_nRecordNum < pCluster->dwRecords)
				{
					// cluster found
					pRecord = (tBYTE*)(pCluster+1) + (pDB->dwRecordSize * p_nRecordNum);
					break;
				}
				p_nRecordNum -= pCluster->dwRecords;
			}

			if (pRecord == NULL){
				PR_TRACE(( _this, prtNOT_IMPORTANT, "sfdb\tEnumRecords: record not found"));
#if defined (__unix__)		
				RELEASE ();
#endif
				
				return errNOT_FOUND;
			}
			
			if (p_pRecordID != NULL)
				memcpy(p_pRecordID, pRecord, pDB->dwRecordIDSize);
			if (p_pRecordData != NULL)
				memcpy(p_pRecordData, pRecord + pDB->dwRecordIDSize, pDB->dwRecordDataSize);
			
#if !defined (__unix__)	
		}
		__EXCEPT (EXCEPTION_EXECUTE_HANDLER) {
			PR_TRACE(( _this, prtERROR, "sfdb\tEnumRecords: exception handled 0x%08X", GetExceptionCode()));
			error = errUNEXPECTED;
		}
	}
	__FINALLY {
#endif
		RELEASE();
#if !defined (__unix__)	
	}
#endif

	PR_TRACE_A1( _this, "Leave SecureFileDatabase::EnumRecords method, ret %terr", error );
	return error;
}
// AVP Prague stamp end


BOOL CreateSA ( hi_SecureFileDatabase _this, SECURITY_ATTRIBUTES** ppSA,  IN DWORD EveryoneAccessPermissions)
{
	BOOL bResult = FALSE;
	DWORD dwRes;
	PACL pACL = NULL;
	PSID pSID = NULL;
	PSECURITY_DESCRIPTOR pSD = NULL;
	SID_IDENTIFIER_AUTHORITY SIDAuth = SECURITY_WORLD_SID_AUTHORITY;
	TOKEN_DEFAULT_DACL *pDefDacl = NULL;
	HANDLE hProcessToken = NULL;
	DWORD dwRetLength;
	EXPLICIT_ACCESS ea;
	//SECURITY_ATTRIBUTES sa;

	// прочитаем DACL процесса по умолчанию

	if (!OpenProcessToken (GetCurrentProcess(), TOKEN_READ, &hProcessToken))
	{
		PR_TRACE(( _this, prtERROR, "sfdb\tOpenProcessToken Error %u\n", GetLastError() ));
		goto Cleanup;
	}

	GetTokenInformation (hProcessToken, TokenDefaultDacl, NULL, 0, &dwRetLength);
	if( GetLastError() != ERROR_INSUFFICIENT_BUFFER )
	{
		PR_TRACE(( _this, prtERROR, "sfdb\tGetTokenInformation Error %u\n", GetLastError() ));
		goto Cleanup;
	}

	pDefDacl = (TOKEN_DEFAULT_DACL *) _alloca ( dwRetLength );

	if (!GetTokenInformation (hProcessToken, TokenDefaultDacl, pDefDacl, dwRetLength, &dwRetLength))
	{
		PR_TRACE(( _this, prtERROR, "sfdb\tGetTokenInformation Error %u\n", GetLastError() ));
		goto Cleanup;
	}

	// создадим well-known SID для группы Everyone
	if(! AllocateAndInitializeSid( &SIDAuth, 1,
		SECURITY_WORLD_RID,
		0, 0, 0, 0, 0, 0, 0,
		&pSID) ) {
		PR_TRACE(( _this, prtERROR, "sfdb\tAllocateAndInitializeSid Error %u\n", GetLastError() ));
		goto Cleanup;
	}
	
	// инициализируем структуру EXPLICIT_ACCESS для ACE
	ZeroMemory(&ea, 1 * sizeof(EXPLICIT_ACCESS));
	ea.grfAccessPermissions = EveryoneAccessPermissions;
	ea.grfAccessMode = SET_ACCESS;
	ea.grfInheritance= NO_INHERITANCE;
	ea.Trustee.TrusteeForm = TRUSTEE_IS_SID;
	ea.Trustee.TrusteeType = TRUSTEE_IS_WELL_KNOWN_GROUP;
	ea.Trustee.ptstrName  = (LPTSTR)pSID;
	
	// добавим в ACL по умолчанию...
	dwRes = SetEntriesInAcl(1, &ea, pDefDacl->DefaultDacl, &pACL);
	if (ERROR_SUCCESS != dwRes) {
		PR_TRACE(( _this, prtERROR, "sfdb\tSetEntriesInAcl Error %u\n", GetLastError() ));
		goto Cleanup;
	}
	
	// инициализируем security descriptor
	CALL_SYS_ObjHeapAlloc(_this, &pSD, SECURITY_DESCRIPTOR_MIN_LENGTH); 
	if (pSD == NULL) { 
		PR_TRACE(( _this, prtERROR, "sfdb\tCALL_SYS_ObjHeapAlloc Error %u\n", GetLastError() ));
		goto Cleanup; 
	} 
	
	if (!InitializeSecurityDescriptor(pSD, SECURITY_DESCRIPTOR_REVISION)) {  
		PR_TRACE(( _this, prtERROR, "sfdb\tInitializeSecurityDescriptor Error %u\n", 
			GetLastError() ));
		goto Cleanup; 
	} 
	
	// добавим ACL в security descriptor. 
	if (!SetSecurityDescriptorDacl(pSD, 
        TRUE,     // fDaclPresent flag   
        pACL, 
        FALSE))   // not a default DACL 
	{  
		PR_TRACE(( _this, prtERROR, "sfdb\tSetSecurityDescriptorDacl Error %u\n", GetLastError() ));
		goto Cleanup; 
	} 

	CALL_SYS_ObjHeapAlloc(_this, ppSA, sizeof(SECURITY_ATTRIBUTES)); 
	if (*ppSA == NULL)
		goto Cleanup; 

	// создаем security attributes
	(*ppSA)->nLength = sizeof (SECURITY_ATTRIBUTES);
	(*ppSA)->lpSecurityDescriptor = pSD;
	(*ppSA)->bInheritHandle = FALSE;

	bResult = TRUE;

Cleanup:

	if (hProcessToken)
		CloseHandle (hProcessToken);
    if (pACL) 
        LocalFree(pACL);
	if (pSID)
		FreeSid(pSID);
	
	return bResult;
}


