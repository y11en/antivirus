// AVP Prague stamp begin( Interface header,  )
// -------- Friday,  15 April 2005,  11:50 --------
// File Name   -- (null)i_qb.cpp
// Copyright © Kaspersky Labs 1996-2004.
// -------------------------------------------
// AVP Prague stamp end



// AVP Prague stamp begin( Header ifndef,  )
#if !defined( __i_qb__c6bc13d3_6791_454f_a5d8_4c11ec39c4da )
#define __i_qb__c6bc13d3_6791_454f_a5d8_4c11ec39c4da
// AVP Prague stamp end



// AVP Prague stamp begin( Header includes,  )
#include <Prague/iface/i_reg.h>

#include <ProductCore/iface/i_posio_sp.h>
#include <ProductCore/iface/i_task.h>
// AVP Prague stamp end




// AVP Prague stamp begin( Interface comment,  )
// QB interface implementation
// AVP Prague stamp end



// AVP Prague stamp begin( Interface menonic identificator,  )
#define IID_QB  ((tIID)(35))
// AVP Prague stamp end



// AVP Prague stamp begin( Interface typedefs,  )
typedef tQWORD                         tOBJECT_ID; //  --
typedef tDWORD                         tCOUNT_TYPE; //  --
// AVP Prague stamp end




// AVP Prague stamp begin( Interface constants,  )
#define cCOUNT_TOTAL                   ((tCOUNT_TYPE)(0)) //  --
// AVP Prague stamp end



// AVP Prague stamp begin( Interface defined messages,  )
// AVP Prague stamp end



// AVP Prague stamp begin( Interface defined errors,  )
// AVP Prague stamp end



// AVP Prague stamp begin( RPC start declaration,  )
#if defined( __RPC__ )
	typedef tUINT hQB;
#else
// AVP Prague stamp end



// AVP Prague stamp begin( Interface forward declaration,  )
struct iQBVtbl;
typedef struct iQBVtbl iQBVtbl;
// AVP Prague stamp end



// AVP Prague stamp begin( Object declaration,  )
#if defined( __cplusplus ) && !defined(_USE_VTBL)
	struct cQB;
	typedef cQB* hQB;
#else
	typedef struct tag_hQB {
		const iQBVtbl*     vtbl; // pointer to the "QB" virtual table
		const iSYSTEMVtbl* sys;  // pointer to the "SYSTEM" virtual table
	} *hQB;
#endif // if defined( __cplusplus )
// AVP Prague stamp end



// AVP Prague stamp begin( RPC_ end declaration,  )
#endif // if defined( __RPC__ )
// AVP Prague stamp end



// AVP Prague stamp begin( Private definitions begin,  )
#if !defined( QB_PRIVATE_DEFINITION )
// AVP Prague stamp end



// AVP Prague stamp begin( Interface function forward declarations,  )


	typedef   tERROR (pr_call *fnpQB_StoreObject)      ( hQB _this, hOBJECT hObject, tPID RestorerPID, const cSerializable* pInfo, hPOSIO_SP* phSecIO, tOBJECT_ID* pObjectID ); // -- Сохранить объект в хранилище;
	typedef   tERROR (pr_call *fnpQB_RestoreObject)    ( hQB _this, tOBJECT_ID ObjectID, const tWSTRING sRestoreFullName, tBOOL bReplaceExisting ); // -- Восстановить объект;
	typedef   tERROR (pr_call *fnpQB_DeleteObject)     ( hQB _this, tOBJECT_ID ObjectID ); // -- Удалить объект;
	typedef   tERROR (pr_call *fnpQB_GetObject)        ( hQB _this, hPOSIO_SP* phSecIO, tOBJECT_ID ObjectID, tBOOL bWriteAccess, cSerializable* pInfo ); // -- Получить объект;
	typedef   tERROR (pr_call *fnpQB_GetCount)         ( hQB _this, tDWORD* pdwCount, tCOUNT_TYPE CountType ); // -- Получить кол-во объектов;
	typedef   tERROR (pr_call *fnpQB_GetObjectByIndex) ( hQB _this, hPOSIO_SP* phSecIO, tOBJECT_ID* pObjectID, tDWORD dwIndex, tBOOL bWriteAccess, cSerializable* pInfo ); // -- Получить объект и/или его идентификатор;
	typedef   tERROR (pr_call *fnpQB_UpdateObjectInfo) ( hQB _this, tOBJECT_ID ObjectID, const cSerializable* pInfo ); // -- Обновить информацию об объекте;


// AVP Prague stamp end



// AVP Prague stamp begin( Interface declaration,  )
struct iQBVtbl {
	fnpQB_StoreObject       StoreObject;
	fnpQB_RestoreObject     RestoreObject;
	fnpQB_DeleteObject      DeleteObject;
	fnpQB_GetObject         GetObject;
	fnpQB_GetCount          GetCount;
	fnpQB_GetObjectByIndex  GetObjectByIndex;
	fnpQB_UpdateObjectInfo  UpdateObjectInfo;
}; // "QB" external virtual table prototype
// AVP Prague stamp end



// AVP Prague stamp begin( Private definitions end,  )
#endif // if !defined( QB_PRIVATE_DEFINITION )
// AVP Prague stamp end



// AVP Prague stamp begin( Interface declared properties,  )
//! #define pgINTERFACE_VERSION mPROPERTY_MAKE_GLOBAL( pTYPE_VERSION , 0x00000082 )
//! #define pgINTERFACE_COMMENT mPROPERTY_MAKE_GLOBAL( pTYPE_STRING  , 0x00000083 )
// AVP Prague stamp end



// AVP Prague stamp begin( Defines to simplify interface method calls,  )
#if !defined( __cplusplus ) || defined(_USE_VTBL)
	#define CALL_QB_StoreObject( _this, hObject, RestorerPID, pInfo, phSecIO, pObjectID )              ((_this)->vtbl->StoreObject( (_this), hObject, RestorerPID, pInfo, phSecIO, pObjectID ))
	#define CALL_QB_RestoreObject( _this, ObjectID, sRestoreFullName, bReplaceExisting )               ((_this)->vtbl->RestoreObject( (_this), ObjectID, sRestoreFullName, bReplaceExisting ))
	#define CALL_QB_DeleteObject( _this, ObjectID )                                                    ((_this)->vtbl->DeleteObject( (_this), ObjectID ))
	#define CALL_QB_GetObject( _this, phSecIO, ObjectID, bWriteAccess, pInfo )                         ((_this)->vtbl->GetObject( (_this), phSecIO, ObjectID, bWriteAccess, pInfo ))
	#define CALL_QB_GetCount( _this, pdwCount, CountType )                                             ((_this)->vtbl->GetCount( (_this), pdwCount, CountType ))
	#define CALL_QB_GetObjectByIndex( _this, phSecIO, pObjectID, dwIndex, bWriteAccess, pInfo )        ((_this)->vtbl->GetObjectByIndex( (_this), phSecIO, pObjectID, dwIndex, bWriteAccess, pInfo ))
	#define CALL_QB_UpdateObjectInfo( _this, ObjectID, pInfo )                                         ((_this)->vtbl->UpdateObjectInfo( (_this), ObjectID, pInfo ))
#else // if !defined( __cplusplus )
	#define CALL_QB_StoreObject( _this, hObject, RestorerPID, pInfo, phSecIO, pObjectID )              ((_this)->StoreObject( hObject, RestorerPID, pInfo, phSecIO, pObjectID ))
	#define CALL_QB_RestoreObject( _this, ObjectID, sRestoreFullName, bReplaceExisting )               ((_this)->RestoreObject( ObjectID, sRestoreFullName, bReplaceExisting ))
	#define CALL_QB_DeleteObject( _this, ObjectID )                                                    ((_this)->DeleteObject( ObjectID ))
	#define CALL_QB_GetObject( _this, phSecIO, ObjectID, bWriteAccess, pInfo )                         ((_this)->GetObject( phSecIO, ObjectID, bWriteAccess, pInfo ))
	#define CALL_QB_GetCount( _this, pdwCount, CountType )                                             ((_this)->GetCount( pdwCount, CountType ))
	#define CALL_QB_GetObjectByIndex( _this, phSecIO, pObjectID, dwIndex, bWriteAccess, pInfo )        ((_this)->GetObjectByIndex( phSecIO, pObjectID, dwIndex, bWriteAccess, pInfo ))
	#define CALL_QB_UpdateObjectInfo( _this, ObjectID, pInfo )                                         ((_this)->UpdateObjectInfo( ObjectID, pInfo ))
#endif // if !defined( __cplusplus )
// AVP Prague stamp end



// AVP Prague stamp begin( C++ interface declaration,  )
#if defined (__cplusplus) && !defined(_USE_VTBL)
	struct pr_abstract iQB {
		virtual tERROR pr_call StoreObject( hOBJECT hObject, tPID RestorerPID, const cSerializable* pInfo, hPOSIO_SP* phSecIO, tOBJECT_ID* pObjectID ) = 0; // -- Сохранить объект в хранилище
		virtual tERROR pr_call RestoreObject( tOBJECT_ID ObjectID, const tWSTRING sRestoreFullName, tBOOL bReplaceExisting ) = 0; // -- Восстановить объект
		virtual tERROR pr_call DeleteObject( tOBJECT_ID ObjectID ) = 0; // -- Удалить объект
		virtual tERROR pr_call GetObject( hPOSIO_SP* phSecIO, tOBJECT_ID ObjectID, tBOOL bWriteAccess, cSerializable* pInfo ) = 0; // -- Получить объект
		virtual tERROR pr_call GetCount( tDWORD* pdwCount, tCOUNT_TYPE CountType ) = 0; // -- Получить кол-во объектов
		virtual tERROR pr_call GetObjectByIndex( hPOSIO_SP* phSecIO, tOBJECT_ID* pObjectID, tDWORD dwIndex, tBOOL bWriteAccess, cSerializable* pInfo ) = 0; // -- Получить объект и/или его идентификатор
		virtual tERROR pr_call UpdateObjectInfo( tOBJECT_ID ObjectID, const cSerializable* pInfo ) = 0; // -- Обновить информацию об объекте
	};

	struct pr_abstract cQB : public iQB, public iObj {
		OBJ_IMPL( cQB );

		operator hOBJECT() { return (hOBJECT)this; }
		operator hQB()   { return (hQB)this; }

	};

#endif // if defined (__cplusplus) && !defined(_USE_VTBL)
// AVP Prague stamp end



// AVP Prague stamp begin( Header endif,  )
#endif // __i_qb__c6bc13d3_6791_454f_a5d8_4c11ec39c4da
// AVP Prague stamp end


#if defined(__PROXY_DECLARATION) && !defined(QB_PROXY_DEFINITION)
#define QB_PROXY_DEFINITION

PR_PROXY_BEGIN(QB)
	PR_SCPRX(QB, StoreObject,        PR_ARG5(tid_OBJECT,tid_DWORD,tid_SERIALIZABLE,tid_OBJECT|tid_POINTER,tid_QWORD|tid_POINTER))
	PR_SCPRX(QB, RestoreObject,      PR_ARG3(tid_QWORD,tid_WSTRING,tid_BOOL))
	PR_SCPRX(QB, DeleteObject,       PR_ARG1(tid_QWORD))
	PR_SCPRX(QB, GetObject,          PR_ARG4(tid_OBJECT|tid_POINTER,tid_QWORD,tid_BOOL,tid_SERIALIZABLE))
	PR_SCPRX(QB, GetCount,           PR_ARG2(tid_DWORD|tid_POINTER,tid_DWORD))
	PR_SCPRX(QB, GetObjectByIndex,   PR_ARG5(tid_OBJECT|tid_POINTER,tid_QWORD|tid_POINTER,tid_DWORD,tid_BOOL,tid_SERIALIZABLE))
	PR_SCPRX(QB, UpdateObjectInfo,   PR_ARG2(tid_QWORD,tid_SERIALIZABLE))
PR_PROXY_END(QB, IID_QB)

#endif // __PROXY_DECLARATION

