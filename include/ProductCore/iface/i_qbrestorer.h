// AVP Prague stamp begin( Interface header,  )
// -------- Friday,  15 April 2005,  11:48 --------
// File Name   -- (null)i_qbrestorer.cpp
// Copyright © Kaspersky Labs 1996-2004.
// -------------------------------------------
// AVP Prague stamp end



// AVP Prague stamp begin( Header ifndef,  )
#if !defined( __i_qbrestorer__2892cdc1_e57d_4be7_b6b2_b096480e2884 )
#define __i_qbrestorer__2892cdc1_e57d_4be7_b6b2_b096480e2884
// AVP Prague stamp end



// AVP Prague stamp begin( Header includes,  )
#include <Prague/iface/i_posio.h>
#include <Prague/iface/i_io.h>
#include <Prague/iface/i_reg.h>

#include <ProductCore/iface/i_posio_sp.h>
#include <ProductCore/iface/i_qb.h>
// AVP Prague stamp end



// AVP Prague stamp begin( Interface comment,  )
// QBRestorer interface implementation
// AVP Prague stamp end



// AVP Prague stamp begin( Interface menonic identificator,  )
#define IID_QBRESTORER  ((tIID)(58013))
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
	typedef tUINT hQBRESTORER;
#else
// AVP Prague stamp end



// AVP Prague stamp begin( Interface forward declaration,  )
struct iQBRestorerVtbl;
typedef struct iQBRestorerVtbl iQBRestorerVtbl;
// AVP Prague stamp end



// AVP Prague stamp begin( Object declaration,  )
#if defined( __cplusplus ) && !defined(_USE_VTBL)
	struct cQBRestorer;
	typedef cQBRestorer* hQBRESTORER;
#else
	typedef struct tag_hQBRESTORER {
		const iQBRestorerVtbl* vtbl; // pointer to the "QBRestorer" virtual table
		const iSYSTEMVtbl*     sys;  // pointer to the "SYSTEM" virtual table
	} *hQBRESTORER;
#endif // if defined( __cplusplus )
// AVP Prague stamp end



// AVP Prague stamp begin( RPC_ end declaration,  )
#endif // if defined( __RPC__ )
// AVP Prague stamp end



// AVP Prague stamp begin( Private definitions begin,  )
#if !defined( QBRestorer_PRIVATE_DEFINITION )
// AVP Prague stamp end



// AVP Prague stamp begin( Interface function forward declarations,  )


	typedef   tERROR (pr_call *fnpQBRestorer_RestoreObject) ( hQBRESTORER _this, hPOSIO_SP hSecIO, const tWSTRING sRestoreFullName, tBOOL bReplaceExisting ); // -- Восстановить объект;
	typedef   tERROR (pr_call *fnpQBRestorer_StoreObject)   ( hQBRESTORER _this, hOBJECT hObject, hPOSIO_SP hSecIO ); // -- ;
	typedef   tERROR (pr_call *fnpQBRestorer_MakeObjectID)  ( hQBRESTORER _this, hOBJECT hObject, tOBJECT_ID* pObjectID ); // -- ;


// AVP Prague stamp end



// AVP Prague stamp begin( Interface declaration,  )
struct iQBRestorerVtbl {
	fnpQBRestorer_RestoreObject  RestoreObject;
	fnpQBRestorer_StoreObject    StoreObject;
	fnpQBRestorer_MakeObjectID   MakeObjectID;
}; // "QBRestorer" external virtual table prototype
// AVP Prague stamp end



// AVP Prague stamp begin( Private definitions end,  )
#endif // if !defined( QBRestorer_PRIVATE_DEFINITION )
// AVP Prague stamp end



// AVP Prague stamp begin( Interface declared properties,  )
//! #define pgINTERFACE_VERSION mPROPERTY_MAKE_GLOBAL( pTYPE_VERSION , 0x00000082 )
//! #define pgINTERFACE_COMMENT mPROPERTY_MAKE_GLOBAL( pTYPE_STRING  , 0x00000083 )
// AVP Prague stamp end



// AVP Prague stamp begin( Defines to simplify interface method calls,  )
#if !defined( __cplusplus ) || defined(_USE_VTBL)
	#define CALL_QBRestorer_RestoreObject( _this, hSecIO, sRestoreFullName, bReplaceExisting )                ((_this)->vtbl->RestoreObject( (_this), hSecIO, sRestoreFullName, bReplaceExisting ))
	#define CALL_QBRestorer_StoreObject( _this, hObject, hSecIO )                                             ((_this)->vtbl->StoreObject( (_this), hObject, hSecIO ))
	#define CALL_QBRestorer_MakeObjectID( _this, hObject, pObjectID )                                         ((_this)->vtbl->MakeObjectID( (_this), hObject, pObjectID ))
#else // if !defined( __cplusplus )
	#define CALL_QBRestorer_RestoreObject( _this, hSecIO, sRestoreFullName, bReplaceExisting )                ((_this)->RestoreObject( hSecIO, sRestoreFullName, bReplaceExisting ))
	#define CALL_QBRestorer_StoreObject( _this, hObject, hSecIO )                                             ((_this)->StoreObject( hObject, hSecIO ))
	#define CALL_QBRestorer_MakeObjectID( _this, hObject, pObjectID )                                         ((_this)->MakeObjectID( hObject, pObjectID ))
#endif // if !defined( __cplusplus )
// AVP Prague stamp end



// AVP Prague stamp begin( C++ interface declaration,  )
#if defined (__cplusplus) && !defined(_USE_VTBL)
	struct pr_abstract iQBRestorer {
		virtual tERROR pr_call RestoreObject( hPOSIO_SP hSecIO, const tWSTRING sRestoreFullName, tBOOL bReplaceExisting ) = 0; // -- Восстановить объект
		virtual tERROR pr_call StoreObject( hOBJECT hObject, hPOSIO_SP hSecIO ) = 0;
		virtual tERROR pr_call MakeObjectID( hOBJECT hObject, tOBJECT_ID* pObjectID ) = 0;
	};

	struct pr_abstract cQBRestorer : public iQBRestorer, public iObj {
		OBJ_IMPL( cQBRestorer );

		operator hOBJECT() { return (hOBJECT)this; }
		operator hQBRESTORER()   { return (hQBRESTORER)this; }

	};

#endif // if defined (__cplusplus) && !defined(_USE_VTBL)
// AVP Prague stamp end



// AVP Prague stamp begin( Header endif,  )
#endif // __i_qbrestorer__2892cdc1_e57d_4be7_b6b2_b096480e2884
// AVP Prague stamp end



