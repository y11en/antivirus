// AVP Prague stamp begin( Interface header,  )
// -------- Friday,  26 September 2003,  10:20 --------
// File Name   -- (null)i_ichecker2.cpp
// Copyright © Kaspersky Labs 1996-2003.
// -------------------------------------------
// AVP Prague stamp end

// AVP Prague stamp begin( Header ifndef,  )
#if !defined( __i_ichecker2__75e1de2a_f20f_4b95_b430_33ee00b67046 )
#define __i_ichecker2__75e1de2a_f20f_4b95_b430_33ee00b67046
// AVP Prague stamp end

// AVP Prague stamp begin( Header includes,  )
#include <Prague/iface/i_io.h>
// AVP Prague stamp end

#include <Prague/pr_vtbl.h>

// AVP Prague stamp begin( Interface comment,  )
// IChecker interface implementation
// AVP Prague stamp end

// AVP Prague stamp begin( Interface menonic identificator,  )
#define IID_ICHECKER2  ((tIID)(58007))
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
	typedef tUINT hICHECKER;
#else
// AVP Prague stamp end

// AVP Prague stamp begin( Interface forward declaration,  )
struct iICheckerVtbl;
typedef struct iICheckerVtbl iICheckerVtbl;
// AVP Prague stamp end

// AVP Prague stamp begin( Object declaration,  )
#if defined( __cplusplus ) && !defined(_USE_VTBL)
	struct cIChecker;
	typedef cIChecker* hICHECKER;
#else
	typedef struct tag_hICHECKER {
		const iICheckerVtbl* vtbl; // pointer to the "IChecker" virtual table
		const iSYSTEMVtbl*   sys;  // pointer to the "SYSTEM" virtual table
	} *hICHECKER;
#endif // if defined( __cplusplus )
// AVP Prague stamp end

// AVP Prague stamp begin( RPC_ end declaration,  )
#endif // if defined( __RPC__ )
// AVP Prague stamp end

// AVP Prague stamp begin( Private definitions begin,  )
#if !defined( IChecker_PRIVATE_DEFINITION )
// AVP Prague stamp end

// AVP Prague stamp begin( Interface function forward declarations,  )


	typedef   tERROR (pr_call *fnpIChecker_GetStatus)     ( hICHECKER _this, tDWORD* result, hIO io, tBYTE* pData, tDWORD dwDataSize, tDWORD* pStatusBuffer, tDWORD dwStatusBufferSize ); // -- Возвращает данные, ассоциированные с файлом.;
	typedef   tERROR (pr_call *fnpIChecker_UpdateStatus)  ( hICHECKER _this, hIO io, tDWORD* pStatusBuffer, tDWORD dwStatusBufferSize ); // -- Устанавливает данные, ассоциированные с файлом.;
	typedef   tERROR (pr_call *fnpIChecker_DeleteStatus)  ( hICHECKER _this, hIO io );          // -- Удаляет данные, ассоциированные с файлом.;
	typedef   tERROR (pr_call *fnpIChecker_FlushStatusDB) ( hICHECKER _this );                  // -- Сохраняет БД ассоциированных данных;
	typedef   tERROR (pr_call *fnpIChecker_ClearStatusDB) ( hICHECKER _this );                  // -- Очищает БД ассоциированных данных.;


// AVP Prague stamp end

// AVP Prague stamp begin( Interface declaration,  )
struct iICheckerVtbl {
	fnpIChecker_GetStatus      GetStatus;
	fnpIChecker_UpdateStatus   UpdateStatus;
	fnpIChecker_DeleteStatus   DeleteStatus;
	fnpIChecker_FlushStatusDB  FlushStatusDB;
	fnpIChecker_ClearStatusDB  ClearStatusDB;
}; // "IChecker" external virtual table prototype
// AVP Prague stamp end

// AVP Prague stamp begin( Private definitions end,  )
#endif // if !defined( IChecker_PRIVATE_DEFINITION )
// AVP Prague stamp end

// AVP Prague stamp begin( Interface declared properties,  )
//! #define pgINTERFACE_VERSION mPROPERTY_MAKE_GLOBAL( pTYPE_VERSION , 0x00000082 )
//! #define pgINTERFACE_COMMENT mPROPERTY_MAKE_GLOBAL( pTYPE_STRING  , 0x00000083 )
// AVP Prague stamp end

// AVP Prague stamp begin( Defines to simplify interface method calls,  )
#if !defined( __cplusplus ) || defined(_USE_VTBL)
	#define CALL_IChecker_GetStatus( _this, result, io, pData, dwDataSize, pStatusBuffer, dwStatusBufferSize )      ((_this)->vtbl->GetStatus( (_this), result, io, pData, dwDataSize, pStatusBuffer, dwStatusBufferSize ))
	#define CALL_IChecker_UpdateStatus( _this, io, pStatusBuffer, dwStatusBufferSize )                              ((_this)->vtbl->UpdateStatus( (_this), io, pStatusBuffer, dwStatusBufferSize ))
	#define CALL_IChecker_DeleteStatus( _this, io )                                                                 ((_this)->vtbl->DeleteStatus( (_this), io ))
	#define CALL_IChecker_FlushStatusDB( _this )                                                                    ((_this)->vtbl->FlushStatusDB( (_this) ))
	#define CALL_IChecker_ClearStatusDB( _this )                                                                    ((_this)->vtbl->ClearStatusDB( (_this) ))
#else // if !defined( __cplusplus )
	#define CALL_IChecker_GetStatus( _this, result, io, pData, dwDataSize, pStatusBuffer, dwStatusBufferSize )      ((_this)->GetStatus( result, io, pData, dwDataSize, pStatusBuffer, dwStatusBufferSize ))
	#define CALL_IChecker_UpdateStatus( _this, io, pStatusBuffer, dwStatusBufferSize )                              ((_this)->UpdateStatus( io, pStatusBuffer, dwStatusBufferSize ))
	#define CALL_IChecker_DeleteStatus( _this, io )                                                                 ((_this)->DeleteStatus( io ))
	#define CALL_IChecker_FlushStatusDB( _this )                                                                    ((_this)->FlushStatusDB( ))
	#define CALL_IChecker_ClearStatusDB( _this )                                                                    ((_this)->ClearStatusDB( ))
#endif // if !defined( __cplusplus )
// AVP Prague stamp end

// AVP Prague stamp begin( C++ interface declaration,  )
#if defined (__cplusplus)
	struct pr_abstract iIChecker {
		virtual tERROR pr_call GetStatus( tDWORD* result, hIO io, tBYTE* pData, tDWORD dwDataSize, tDWORD* pStatusBuffer, tDWORD dwStatusBufferSize ) = 0; // -- Возвращает данные, ассоциированные с файлом.
		virtual tERROR pr_call UpdateStatus( hIO io, tDWORD* pStatusBuffer, tDWORD dwStatusBufferSize ) = 0; // -- Устанавливает данные, ассоциированные с файлом.
		virtual tERROR pr_call DeleteStatus( hIO io ) = 0; // -- Удаляет данные, ассоциированные с файлом.
		virtual tERROR pr_call FlushStatusDB() = 0; // -- Сохраняет БД ассоциированных данных
		virtual tERROR pr_call ClearStatusDB() = 0; // -- Очищает БД ассоциированных данных.
	};

	struct pr_abstract cIChecker : public iIChecker, public iObj {

		OBJ_IMPL( cIChecker );

		operator hOBJECT() { return (hOBJECT)this; }
		operator hICHECKER()   { return (hICHECKER)this; }

	};

#endif // if defined (__cplusplus)
// AVP Prague stamp end

// AVP Prague stamp begin( Header endif,  )
#endif // __i_ichecker2__341c1fa2_a341_4668_95e6_903a84b78ef8
// AVP Prague stamp end

