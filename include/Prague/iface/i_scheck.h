// --------------------------------------------------------------------------------
// AVP Prague stamp begin( Interface header,  )
// -------- Tuesday,  08 November 2005,  14:25 --------
// File Name   -- (null)i_scheck.cpp
// Copyright © Kaspersky Lab 1996-2005.
// -------------------------------------------
// AVP Prague stamp end



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( Header ifndef,  )
#if !defined( __i_scheck__e4e19104_c9cd_4160_b14e_49e491f04660 )
#define __i_scheck__e4e19104_c9cd_4160_b14e_49e491f04660
// AVP Prague stamp end



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( Header includes,  )
#include <Prague/pr_vtbl.h>
#include <Prague/iface/i_schecklist.h>
// AVP Prague stamp end



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( Interface comment,  )
// SCheck interface implementation
// Short comments -- check digitally signet objects
// AVP Prague stamp end



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( Interface menonic identificator,  )
#define IID_SCHECK  ((tIID)(46))
// AVP Prague stamp end



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( Interface typedefs,  )
// AVP Prague stamp end



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( Interface constants,  )
// AVP Prague stamp end



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( Interface defined messages,  )
// AVP Prague stamp end



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( Interface defined errors,  )
// AVP Prague stamp end



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( RPC start declaration,  )
#if defined( __RPC__ )
	typedef tUINT hSCHECK;
#else
// AVP Prague stamp end



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( Interface forward declaration,  )
struct iSCheckVtbl;
typedef struct iSCheckVtbl iSCheckVtbl;
// AVP Prague stamp end



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( Object declaration,  )
#if defined( __cplusplus ) && !defined(_USE_VTBL)
	struct cSCheck;
	typedef cSCheck* hSCHECK;
#else
	typedef struct tag_hSCHECK {
		const iSCheckVtbl* vtbl; // pointer to the "SCheck" virtual table
		const iSYSTEMVtbl* sys;  // pointer to the "SYSTEM" virtual table
	} *hSCHECK;
#endif // if defined( __cplusplus )
// AVP Prague stamp end



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( RPC_ end declaration,  )
#endif // if defined( __RPC__ )
// AVP Prague stamp end



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( Private definitions begin,  )
#if !defined( SCheck_PRIVATE_DEFINITION )
// AVP Prague stamp end



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( Interface function forward declarations,  )


	typedef   tERROR (pr_call *fnpSCheck_InitParamByObj)  ( hSCHECK _this, hOBJECT obj );     // -- add parameter (key, reg) to init DSKM session by object (hIO,hSeqIO,hString);
	typedef   tERROR (pr_call *fnpSCheck_InitParamByMask) ( hSCHECK _this, tDWORD* params_found, const tVOID* mask, tCODEPAGE mask_cp ); // -- add parameter (key, reg) to init DSKM session by file mask;
	typedef   tERROR (pr_call *fnpSCheck_InitCompleted)   ( hSCHECK _this );                  // -- ;
	typedef   tERROR (pr_call *fnpSCheck_CheckObj)        ( hSCHECK _this, hOBJECT obj, const tCHAR* dskm_comp_id, const tCHAR* dskm_obj_type ); // -- check single object;
	typedef   tERROR (pr_call *fnpSCheck_CheckObjByMask)  ( hSCHECK _this, tDWORD* objs_found, const tVOID* mask, tCODEPAGE mask_cp, const tCHAR* dskm_comp_id, const tCHAR* dskm_obj_type ); // -- check single object;
	typedef   tERROR (pr_call *fnpSCheck_CheckObjByList)  ( hSCHECK _this, hSCHECKLIST list ); // -- check object list;
	typedef   tERROR (pr_call *fnpSCheck_GetObjHash)      ( hSCHECK _this, tDWORD* hash_size, hOBJECT obj, tBYTE* hash_buff, tDWORD hash_buff_size ); // -- ;


// AVP Prague stamp end



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( Interface declaration,  )
struct iSCheckVtbl {
	fnpSCheck_InitParamByObj   InitParamByObj;
	fnpSCheck_InitParamByMask  InitParamByMask;
	fnpSCheck_InitCompleted    InitCompleted;
	fnpSCheck_CheckObj         CheckObj;
	fnpSCheck_CheckObjByMask   CheckObjByMask;
	fnpSCheck_CheckObjByList   CheckObjByList;
	fnpSCheck_GetObjHash       GetObjHash;
}; // "SCheck" external virtual table prototype
// AVP Prague stamp end



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( Private definitions end,  )
#endif // if !defined( SCheck_PRIVATE_DEFINITION )
// AVP Prague stamp end



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( Interface declared properties,  )
//! #define pgINTERFACE_VERSION mPROPERTY_MAKE_GLOBAL( pTYPE_VERSION , 0x00000082 )
//! #define pgINTERFACE_COMMENT mPROPERTY_MAKE_GLOBAL( pTYPE_STRING  , 0x00000083 )
#define pgDSKM_SESSION      mPROPERTY_MAKE_GLOBAL( pTYPE_PTR     , 0x00001000 )
#define pgDSKM_APP_ID       mPROPERTY_MAKE_GLOBAL( pTYPE_STRING  , 0x00001001 )
#define pgDSKM_COMP_ID      mPROPERTY_MAKE_GLOBAL( pTYPE_STRING  , 0x00001004 )
#define pgDSKM_OBJ_TYPE     mPROPERTY_MAKE_GLOBAL( pTYPE_STRING  , 0x00001005 )
#define pgDSKM_STORED_ERROR mPROPERTY_MAKE_GLOBAL( pTYPE_DWORD   , 0x00001002 )
#define pgPR_STORED_ERROR   mPROPERTY_MAKE_GLOBAL( pTYPE_ERROR   , 0x00001003 )
// AVP Prague stamp end



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( Defines to simplify interface method calls,  )
#if !defined( __cplusplus ) || defined(_USE_VTBL)
	#define CALL_SCheck_InitParamByObj( _this, obj )                                                               ((_this)->vtbl->InitParamByObj( (_this), obj ))
	#define CALL_SCheck_InitParamByMask( _this, params_found, mask, mask_cp )                                      ((_this)->vtbl->InitParamByMask( (_this), params_found, mask, mask_cp ))
	#define CALL_SCheck_InitCompleted( _this )                                                                     ((_this)->vtbl->InitCompleted( (_this) ))
	#define CALL_SCheck_CheckObj( _this, obj, dskm_comp_id, dskm_obj_type )                                        ((_this)->vtbl->CheckObj( (_this), obj, dskm_comp_id, dskm_obj_type ))
	#define CALL_SCheck_CheckObjByMask( _this, objs_found, mask, mask_cp, dskm_comp_id, dskm_obj_type )            ((_this)->vtbl->CheckObjByMask( (_this), objs_found, mask, mask_cp, dskm_comp_id, dskm_obj_type ))
	#define CALL_SCheck_CheckObjByList( _this, list )                                                              ((_this)->vtbl->CheckObjByList( (_this), list ))
	#define CALL_SCheck_GetObjHash( _this, hash_size, obj, hash_buff, hash_buff_size )                             ((_this)->vtbl->GetObjHash( (_this), hash_size, obj, hash_buff, hash_buff_size ))
#else // if !defined( __cplusplus )
	#define CALL_SCheck_InitParamByObj( _this, obj )                                                               ((_this)->InitParamByObj( obj ))
	#define CALL_SCheck_InitParamByMask( _this, params_found, mask, mask_cp )                                      ((_this)->InitParamByMask( params_found, mask, mask_cp ))
	#define CALL_SCheck_InitCompleted( _this )                                                                     ((_this)->InitCompleted( ))
	#define CALL_SCheck_CheckObj( _this, obj, dskm_comp_id, dskm_obj_type )                                        ((_this)->CheckObj( obj, dskm_comp_id, dskm_obj_type ))
	#define CALL_SCheck_CheckObjByMask( _this, objs_found, mask, mask_cp, dskm_comp_id, dskm_obj_type )            ((_this)->CheckObjByMask( objs_found, mask, mask_cp, dskm_comp_id, dskm_obj_type ))
	#define CALL_SCheck_CheckObjByList( _this, list )                                                              ((_this)->CheckObjByList( list ))
	#define CALL_SCheck_GetObjHash( _this, hash_size, obj, hash_buff, hash_buff_size )                             ((_this)->GetObjHash( hash_size, obj, hash_buff, hash_buff_size ))
#endif // if !defined( __cplusplus )
// AVP Prague stamp end



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( C++ interface declaration,  )
#if defined (__cplusplus) && !defined(_USE_VTBL)
	struct pr_abstract iSCheck {
		virtual tERROR pr_call InitParamByObj( hOBJECT obj ) = 0; // -- add parameter (key, reg) to init DSKM session by object (hIO,hSeqIO,hString)
		virtual tERROR pr_call InitParamByMask( tDWORD* params_found, const tVOID* mask, tCODEPAGE mask_cp ) = 0; // -- add parameter (key, reg) to init DSKM session by file mask
		virtual tERROR pr_call InitCompleted() = 0;
		virtual tERROR pr_call CheckObj( hOBJECT obj, const tCHAR* dskm_comp_id, const tCHAR* dskm_obj_type ) = 0; // -- check single object
		virtual tERROR pr_call CheckObjByMask( tDWORD* objs_found, const tVOID* mask, tCODEPAGE mask_cp, const tCHAR* dskm_comp_id, const tCHAR* dskm_obj_type ) = 0; // -- check single object
		virtual tERROR pr_call CheckObjByList( hSCHECKLIST list ) = 0; // -- check object list
		virtual tERROR pr_call GetObjHash( tDWORD* hash_size, hOBJECT obj, tBYTE* hash_buff, tDWORD hash_buff_size ) = 0;
	};

	struct pr_abstract cSCheck : public iSCheck, public iObj {

		OBJ_IMPL( cSCheck );

		operator hOBJECT() { return (hOBJECT)this; }
		operator hSCHECK()   { return (hSCHECK)this; }

		tPTR pr_call get_pgDSKM_SESSION() { return (tPTR)getPtr(pgDSKM_SESSION); }
		tERROR pr_call set_pgDSKM_SESSION( tPTR value ) { return setPtr(pgDSKM_SESSION,(tPTR)value); }

		tERROR pr_call get_pgDSKM_APP_ID( tPTR buff, tDWORD size, tCODEPAGE cp=cCP_ANSI ) { return getStr(pgDSKM_APP_ID,buff,size,cp); }
		tERROR pr_call set_pgDSKM_APP_ID( tPTR buff, tDWORD size, tCODEPAGE cp=cCP_ANSI ) { return setStr(pgDSKM_APP_ID,buff,size,cp); }

		tERROR pr_call get_pgDSKM_COMP_ID( tPTR buff, tDWORD size, tCODEPAGE cp=cCP_ANSI ) { return getStr(pgDSKM_COMP_ID,buff,size,cp); }
		tERROR pr_call set_pgDSKM_COMP_ID( tPTR buff, tDWORD size, tCODEPAGE cp=cCP_ANSI ) { return setStr(pgDSKM_COMP_ID,buff,size,cp); }

		tERROR pr_call get_pgDSKM_OBJ_TYPE( tPTR buff, tDWORD size, tCODEPAGE cp=cCP_ANSI ) { return getStr(pgDSKM_OBJ_TYPE,buff,size,cp); }
		tERROR pr_call set_pgDSKM_OBJ_TYPE( tPTR buff, tDWORD size, tCODEPAGE cp=cCP_ANSI ) { return setStr(pgDSKM_OBJ_TYPE,buff,size,cp); }

		tDWORD pr_call get_pgDSKM_STORED_ERROR() { return (tDWORD)getDWord(pgDSKM_STORED_ERROR); }
		tERROR pr_call set_pgDSKM_STORED_ERROR( tDWORD value ) { return setDWord(pgDSKM_STORED_ERROR,(tDWORD)value); }

		tERROR pr_call get_pgPR_STORED_ERROR() { return (tERROR)getDWord(pgPR_STORED_ERROR); }
		tERROR pr_call set_pgPR_STORED_ERROR( tERROR value ) { return setDWord(pgPR_STORED_ERROR,(tDWORD)value); }

	};

#endif // if defined (__cplusplus) && !defined(_USE_VTBL)
// AVP Prague stamp end



// --------------------------------------------------------------------------------
// AVP Prague stamp begin( Header endif,  )
#endif // __i_scheck__e4e19104_c9cd_4160_b14e_49e491f04660
// AVP Prague stamp end



