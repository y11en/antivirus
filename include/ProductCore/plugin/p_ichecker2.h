// AVP Prague stamp begin( Interface header,  )
// -------- ANSI C Code Generator 2.0 --------
// -------- Tuesday,  20 January 2004,  16:36 --------
// -------------------------------------------
// Copyright © Kaspersky Labs 1996-2003.
// -------------------------------------------
// Project     -- Kaspersky Anti-Virus
// Sub project -- Not defined
// Purpose     -- IChecker
// Author      -- Pavlushchik
// File Name   -- plugin_ichecker2.c
// Additional info
//    Integrated Solution Lite
// -------------------------------------------
// AVP Prague stamp end



// AVP Prague stamp begin( Header ifndef,  )
#if !defined( __pligin_ichecker2 )
#define __pligin_ichecker2
// AVP Prague stamp end



// AVP Prague stamp begin( Header includes,  )
#include <Prague/prague.h>
#include <ProductCore/iface/i_ichecker2.h>
// AVP Prague stamp end



// AVP Prague stamp begin( Plugin identifier,  )
#define PID_ICHECKER2  ((tPID)(58029))
// AVP Prague stamp end



// AVP Prague stamp begin( Vendor identifier,  )
#define VID_MIKE  ((tVID)(70))
// AVP Prague stamp end



// AVP Prague stamp begin( Plugin public definitions, IChecker )
// IChecker interface implementation
// Short comments --

// constants
#define cICHECKER_ARCHIVES_OFF         ((tDWORD)(1)) // Отключает подсчет хеша для архивов

// properties
#define plDB_PATHNAME       mPROPERTY_MAKE_LOCAL( pTYPE_STRING  , 0x00002000 )
#define plDB_USER_DATA_SIZE mPROPERTY_MAKE_LOCAL( pTYPE_DWORD   , 0x00002001 )
#define plDB_USER_VERSION   mPROPERTY_MAKE_LOCAL( pTYPE_DWORD   , 0x00002002 )

typedef struct tag_ICHECKER2IChecker *hICHECKER2IChecker;
// plugin interface
typedef   tERROR (pr_call *fnpICHECKER2IChecker_GetStatusEx)( hICHECKER2IChecker _this, tDWORD* result, hIO io, tBYTE* pData, tDWORD dwDataSize, tDWORD* pStatusBuffer, tDWORD dwStatusBufferSize, tDWORD dwFlags ); // -- Возвращает данные, ассоциированные с файлом.;

typedef struct tag_iICHECKER2ICheckerVtbl 
{
	struct iICheckerVtbl prt;
	fnpICHECKER2IChecker_GetStatusEx  GetStatusEx;
} iICHECKER2ICheckerVtbl;

struct tag_ICHECKER2IChecker 
{
	const iICHECKER2ICheckerVtbl* vtbl; // pointer to the "ICHECKER2IChecker" virtual table
	const iSYSTEMVtbl* sys;  // pointer to the "SYSTEM" virtual table
};

#define CALL_ICHECKER2IChecker_GetStatusEx( _this, result, io, pData, dwDataSize, pStatusBuffer, dwStatusBufferSize, dwFlags )      ((_this)->vtbl->GetStatusEx( (_this), result, io, pData, dwDataSize, pStatusBuffer, dwStatusBufferSize, dwFlags ))

#if defined (__cplusplus)
struct cICHECKER2IChecker : public cIChecker 
{
	virtual tERROR pr_call GetStatusEx( tDWORD* result, hIO io, tBYTE* pData, tDWORD dwDataSize, tDWORD* pStatusBuffer, tDWORD dwStatusBufferSize, tDWORD dwFlags ) = 0; // -- Возвращает данные, ассоциированные с файлом.

	operator hOBJECT() { return (hOBJECT)this; }
	operator hICHECKER2IChecker()   { return (hICHECKER2IChecker)this; }

	tERROR pr_call get_plDB_PATHNAME( tPTR buff, tDWORD size, tCODEPAGE cp=cCP_ANSI ) { return getStr(plDB_PATHNAME,buff,size,cp); }
	tERROR pr_call set_plDB_PATHNAME( tPTR buff, tDWORD size, tCODEPAGE cp=cCP_ANSI ) { return setStr(plDB_PATHNAME,buff,size,cp); }

	tDWORD pr_call get_plDB_USER_DATA_SIZE() { return (tDWORD)getDWord(plDB_USER_DATA_SIZE); }
	tERROR pr_call set_plDB_USER_DATA_SIZE( tDWORD value ) { return setDWord(plDB_USER_DATA_SIZE,(tDWORD)value); }

	tDWORD pr_call get_plDB_USER_VERSION() { return (tDWORD)getDWord(plDB_USER_VERSION); }
	tERROR pr_call set_plDB_USER_VERSION( tDWORD value ) { return setDWord(plDB_USER_VERSION,(tDWORD)value); }

};

#endif // if defined (__cplusplus)
// AVP Prague stamp end



// AVP Prague stamp begin( Header endif,  )
#endif // __pligin_ichecker2
// AVP Prague stamp end



