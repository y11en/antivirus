// AVP Prague stamp begin( Interface header,  )
// -------- Friday,  26 September 2003,  10:17 --------
// File Name   -- (null)i_uacall.cpp
// Copyright © Kaspersky Labs 1996-2003.
// -------------------------------------------
// AVP Prague stamp end

// AVP Prague stamp begin( Header ifndef,  )
#if !defined( __i_uacall__19b8a016_793a_49b6_ac0e_475c0125c365 )
#define __i_uacall__19b8a016_793a_49b6_ac0e_475c0125c365
// AVP Prague stamp end

// AVP Prague stamp begin( Header includes,  )
// AVP Prague stamp end

#include <Prague/pr_vtbl.h>


// AVP Prague stamp begin( Interface comment,  )
// UniArchiverCallback interface implementation
// Short comments -- Интерфейс связи с Uni Archiver для получения доступа к некоторым внутренним спискам
// AVP Prague stamp end

// AVP Prague stamp begin( Interface menonic identificator,  )
#define IID_UNIARCHIVERCALLBACK  ((tIID)(63006))
// AVP Prague stamp end

// AVP Prague stamp begin( Interface typedefs,  )
typedef tDATA                         hNEWOBJECT; // Хэндл нового объекта
// AVP Prague stamp end

// AVP Prague stamp begin( Interface constants,  )
#define cCHANGED_OBJECT                ((tDWORD)(2)) // dwTable in FindObjectInList
#define cDELETED_OBJECT                ((tDWORD)(3)) // dwTable in FindObjectInList
// AVP Prague stamp end

// AVP Prague stamp begin( Interface defined messages,  )
// AVP Prague stamp end

// AVP Prague stamp begin( Interface defined errors,  )
// AVP Prague stamp end

// AVP Prague stamp begin( RPC start declaration,  )
#if defined( __RPC__ )
	typedef tUINT hUNIARCCALLBACK;
#else
// AVP Prague stamp end

// AVP Prague stamp begin( Interface forward declaration,  )
struct iUniArchiverCallbackVtbl;
typedef struct iUniArchiverCallbackVtbl iUniArchiverCallbackVtbl;
// AVP Prague stamp end

// AVP Prague stamp begin( Object declaration,  )
#if defined( __cplusplus ) && !defined(_USE_VTBL)
	struct cUniArchiverCallback;
	typedef cUniArchiverCallback* hUNIARCCALLBACK;
#else
	typedef struct tag_hUNIARCCALLBACK {
		const iUniArchiverCallbackVtbl* vtbl; // pointer to the "UniArchiverCallback" virtual table
		const iSYSTEMVtbl*              sys;  // pointer to the "SYSTEM" virtual table
	} *hUNIARCCALLBACK;
#endif // if defined( __cplusplus )
// AVP Prague stamp end

// AVP Prague stamp begin( RPC_ end declaration,  )
#endif // if defined( __RPC__ )
// AVP Prague stamp end

// AVP Prague stamp begin( Private definitions begin,  )
#if !defined( UniArchiverCallback_PRIVATE_DEFINITION )
// AVP Prague stamp end

// AVP Prague stamp begin( Interface function forward declarations,  )


	typedef   tERROR (pr_call *fnpUniArchiverCallback_FindObjectInList)  ( hUNIARCCALLBACK _this, hOBJECT* result, tDWORD dwTable, tQWORD qwObjectId ); // -- Найти элемент в списке;
	typedef   tERROR (pr_call *fnpUniArchiverCallback_GetFirstNewObject) ( hUNIARCCALLBACK _this, hNEWOBJECT* result );   // -- Получить первый элемент в списке новых объектов;
	typedef   tERROR (pr_call *fnpUniArchiverCallback_GetNextNewObject)  ( hUNIARCCALLBACK _this, hNEWOBJECT* result );   // -- Получить следующий элемент в списке новых объектов;
	typedef   tERROR (pr_call *fnpUniArchiverCallback_GetNewObjectIo)    ( hUNIARCCALLBACK _this, hOBJECT* result, hNEWOBJECT node ); // -- Получить объект типа hIO для нового файла;


// AVP Prague stamp end

// AVP Prague stamp begin( Interface declaration,  )
struct iUniArchiverCallbackVtbl {
	fnpUniArchiverCallback_FindObjectInList   FindObjectInList;
	fnpUniArchiverCallback_GetFirstNewObject  GetFirstNewObject;
	fnpUniArchiverCallback_GetNextNewObject   GetNextNewObject;
	fnpUniArchiverCallback_GetNewObjectIo     GetNewObjectIo;
}; // "UniArchiverCallback" external virtual table prototype
// AVP Prague stamp end

// AVP Prague stamp begin( Private definitions end,  )
#endif // if !defined( UniArchiverCallback_PRIVATE_DEFINITION )
// AVP Prague stamp end

// AVP Prague stamp begin( Interface declared properties,  )
//! #define pgINTERFACE_VERSION mPROPERTY_MAKE_GLOBAL( pTYPE_VERSION , 0x00000082 )
//! #define pgINTERFACE_COMMENT mPROPERTY_MAKE_GLOBAL( pTYPE_STRING  , 0x00000083 )
// AVP Prague stamp end

// AVP Prague stamp begin( Defines to simplify interface method calls,  )
#if !defined( __cplusplus ) || defined(_USE_VTBL)
	#define CALL_UniArchiverCallback_FindObjectInList( _this, result, dwTable, qwObjectId )                 ((_this)->vtbl->FindObjectInList( (_this), result, dwTable, qwObjectId ))
	#define CALL_UniArchiverCallback_GetFirstNewObject( _this, result )                                     ((_this)->vtbl->GetFirstNewObject( (_this), result ))
	#define CALL_UniArchiverCallback_GetNextNewObject( _this, result )                                      ((_this)->vtbl->GetNextNewObject( (_this), result ))
	#define CALL_UniArchiverCallback_GetNewObjectIo( _this, result, node )                                  ((_this)->vtbl->GetNewObjectIo( (_this), result, node ))
#else // if !defined( __cplusplus )
	#define CALL_UniArchiverCallback_FindObjectInList( _this, result, dwTable, qwObjectId )                 ((_this)->FindObjectInList( result, dwTable, qwObjectId ))
	#define CALL_UniArchiverCallback_GetFirstNewObject( _this, result )                                     ((_this)->GetFirstNewObject( result ))
	#define CALL_UniArchiverCallback_GetNextNewObject( _this, result )                                      ((_this)->GetNextNewObject( result ))
	#define CALL_UniArchiverCallback_GetNewObjectIo( _this, result, node )                                  ((_this)->GetNewObjectIo( result, node ))
#endif // if !defined( __cplusplus )
// AVP Prague stamp end

// AVP Prague stamp begin( C++ interface declaration,  )
#if defined (__cplusplus)
	struct pr_abstract iUniArchiverCallback {
		virtual tERROR pr_call FindObjectInList( hOBJECT* result, tDWORD dwTable, tQWORD qwObjectId ) = 0; // -- Найти элемент в списке
		virtual tERROR pr_call GetFirstNewObject( hNEWOBJECT* result ) = 0; // -- Получить первый элемент в списке новых объектов
		virtual tERROR pr_call GetNextNewObject( hNEWOBJECT* result ) = 0; // -- Получить следующий элемент в списке новых объектов
		virtual tERROR pr_call GetNewObjectIo( hOBJECT* result, hNEWOBJECT node ) = 0; // -- Получить объект типа hIO для нового файла
	};

	struct pr_abstract cUniArchiverCallback : public iUniArchiverCallback, public iObj {

		OBJ_IMPL( cUniArchiverCallback );

		operator hOBJECT() { return (hOBJECT)this; }
		operator hUNIARCCALLBACK()   { return (hUNIARCCALLBACK)this; }

	};

#endif // if defined (__cplusplus)
// AVP Prague stamp end

// AVP Prague stamp begin( Header endif,  )
#endif // __i_uacall__807e2bd9_9482_400b_b0cc_a218f2d7ebd0
// AVP Prague stamp end

