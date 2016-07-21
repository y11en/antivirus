// AVP Prague stamp begin( Interface header,  )
// -------- Tuesday,  03 August 2004,  11:08 --------
// File Name   -- (null)i_btimages.c
// Copyright © Kaspersky Lab 1996-2004.
// -------------------------------------------
// AVP Prague stamp end



// AVP Prague stamp begin( Header ifndef,  )
#if !defined( __i_btimages__1b85a602_b2dd_459a_94fd_5593097e4d29 )
#define __i_btimages__1b85a602_b2dd_459a_94fd_5593097e4d29
// AVP Prague stamp end



// AVP Prague stamp begin( Header includes,  )
// AVP Prague stamp end

#include <Prague/pr_vtbl.h>


// AVP Prague stamp begin( Interface comment,  )
// BtImages interface implementation
// Short comments -- хранение бутовых секторов
// AVP Prague stamp end



// AVP Prague stamp begin( Interface menonic identificator,  )
#define IID_BTIMAGES  ((tIID)(61012))
// AVP Prague stamp end



// AVP Prague stamp begin( Interface typedefs,  )
// AVP Prague stamp end



// AVP Prague stamp begin( Interface constants,  )
#define BTIMAGES_LAST                 ((tDWORD)(-1)) // индекс последнего сохранения сектора
// AVP Prague stamp end



// AVP Prague stamp begin( Interface defined messages,  )
// AVP Prague stamp end



// AVP Prague stamp begin( Interface defined errors,  )
// AVP Prague stamp end



// AVP Prague stamp begin( RPC start declaration,  )
#if defined( __RPC__ )
	typedef tUINT hBTIMAGES;
#else
// AVP Prague stamp end



// AVP Prague stamp begin( Interface forward declaration,  )
struct iBtImagesVtbl;
typedef struct iBtImagesVtbl iBtImagesVtbl;
// AVP Prague stamp end



// AVP Prague stamp begin( Object declaration,  )
#if defined( __cplusplus ) && !defined(_USE_VTBL)
	struct cBtImages;
	typedef cBtImages* hBTIMAGES;
#else
	typedef struct tag_hBTIMAGES 
	{
		const iBtImagesVtbl* vtbl; // pointer to the "BtImages" virtual table
		const iSYSTEMVtbl*   sys;  // pointer to the "SYSTEM" virtual table
	} *hBTIMAGES;
#endif // if defined( __cplusplus )
// AVP Prague stamp end



// AVP Prague stamp begin( RPC_ end declaration,  )
#endif // if defined( __RPC__ )
// AVP Prague stamp end



// AVP Prague stamp begin( Private definitions begin,  )
#if !defined( BtImages_PRIVATE_DEFINITION )
// AVP Prague stamp end



// AVP Prague stamp begin( Interface function forward declarations,  )


	typedef   tERROR (pr_call *fnpBtImages_Save)             ( hBTIMAGES _this, hOBJECT io );   // -- сохранить образ;
	typedef   tERROR (pr_call *fnpBtImages_Restore)          ( hBTIMAGES _this, hOBJECT io, tDWORD index ); // -- восстановить;
	typedef   tERROR (pr_call *fnpBtImages_GetImageDateTime) ( hBTIMAGES _this, hOBJECT io, tDWORD index, tDATETIME* dt ); // -- дата сохранения образа;


// AVP Prague stamp end



// AVP Prague stamp begin( Interface declaration,  )
struct iBtImagesVtbl 
{
	fnpBtImages_Save              Save;
	fnpBtImages_Restore           Restore;
	fnpBtImages_GetImageDateTime  GetImageDateTime;
}; // "BtImages" external virtual table prototype
// AVP Prague stamp end



// AVP Prague stamp begin( Private definitions end,  )
#endif // if !defined( BtImages_PRIVATE_DEFINITION )
// AVP Prague stamp end



// AVP Prague stamp begin( Interface declared properties,  )
//! #define pgINTERFACE_VERSION mPROPERTY_MAKE_GLOBAL( pTYPE_VERSION , 0x00000082 )
//! #define pgINTERFACE_COMMENT mPROPERTY_MAKE_GLOBAL( pTYPE_STRING  , 0x00000083 )
#define pgRepositoryName    mPROPERTY_MAKE_GLOBAL( pTYPE_STRING  , 0x00001000 )
// AVP Prague stamp end



// AVP Prague stamp begin( Defines to simplify interface method calls,  )
#if !defined( __cplusplus ) || defined(_USE_VTBL)
	#define CALL_BtImages_Save( _this, io )                                     ((_this)->vtbl->Save( (_this), io ))
	#define CALL_BtImages_Restore( _this, io, index )                           ((_this)->vtbl->Restore( (_this), io, index ))
	#define CALL_BtImages_GetImageDateTime( _this, io, index, dt )              ((_this)->vtbl->GetImageDateTime( (_this), io, index, dt ))
#else // if !defined( __cplusplus )
	#define CALL_BtImages_Save( _this, io )                                     ((_this)->Save( io ))
	#define CALL_BtImages_Restore( _this, io, index )                           ((_this)->Restore( io, index ))
	#define CALL_BtImages_GetImageDateTime( _this, io, index, dt )              ((_this)->GetImageDateTime( io, index, dt ))
#endif // if !defined( __cplusplus )
// AVP Prague stamp end



// AVP Prague stamp begin( C++ interface declaration,  )
#if defined (__cplusplus) && !defined(_USE_VTBL)
	struct pr_abstract iBtImages 
	{
		virtual tERROR pr_call Save( hOBJECT io ) = 0; // -- сохранить образ
		virtual tERROR pr_call Restore( hOBJECT io, tDWORD index ) = 0; // -- восстановить
		virtual tERROR pr_call GetImageDateTime( hOBJECT io, tDWORD index, tDATETIME* dt ) = 0; // -- дата сохранения образа
	};

	struct pr_abstract cBtImages : public iBtImages, public iObj 
	{
		OBJ_IMPL( cBtImages );

		operator hOBJECT() { return (hOBJECT)this; }
		operator hBTIMAGES()   { return (hBTIMAGES)this; }

		tERROR pr_call get_pgRepositoryName( tPTR buff, tDWORD size, tCODEPAGE cp=cCP_ANSI ) { return getStr(pgRepositoryName,buff,size,cp); }
		tERROR pr_call set_pgRepositoryName( tPTR buff, tDWORD size, tCODEPAGE cp=cCP_ANSI ) { return setStr(pgRepositoryName,buff,size,cp); }

	};

#endif // if defined (__cplusplus) && !defined(_USE_VTBL)
// AVP Prague stamp end



// AVP Prague stamp begin( Header endif,  )
#endif // __i_btimages__1b85a602_b2dd_459a_94fd_5593097e4d29
// AVP Prague stamp end



